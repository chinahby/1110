/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               T C P O U T . C

GENERAL DESCRIPTION
  TCP output segment processing

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright 1991 Phil Karn, KA9Q
  Additional material Copyright 1992-1995 William Allen Simpson
  Copyright (c) 1995-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpout.c_v   1.6   25 Feb 2003 18:00:16   ssinghai  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_tcpout.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/19/04    sv     Cleared DF bit for outgoing TCP segments.
03/08/04    sv     Fixed re-tranmission timeout value computation in 
                   tcp_output.
01/20/04    sv     Modified code to advertize mss from TCB instead
                   of DEF_MSS for all the connections.
12/01/03    sv     Added missing return from tcp_ouput if we ran out of 
                   memory.
08/25/03    sv     Fixed a bug in datagram size computation during forced
                   re-transmission.
08/04/03    sv     Modified code to disable Nagle's algorithm if NODELAY field
                   in the TCB is set.
06/06/03    om     Use ID field in ip_send() to indicate ESP processing.
06/03/03    ss     Added TCP_CLOSING state check for retransmitting FIN for 
                   simulatenous close scenario.
06/02/03    ss     Renamed TCP_NO_DELAY to FEATURE_DS_TCP_NO_DELAY.
04/23/03    ss     Modified code to reflect new socket options handling
02/24/03    ss     Modified tcp_output() to form DSM item chain to support
                   MSS larger than a single DSM item size.
11/07/02    mvl    Added support for variable window size.
09/25/02    om     Use TCB's or SCB's routing cache for ip_send().
09/17/02    atp    tcp_output() to use tcb routing cache for non-socket build.
08/30/02    om     Updates to use socket's routing cache.
07/31/02    usb    Removed include files ps.h and psi.h, renamed psmisc.h
                   to ps_utils.h
07/10/02    ss     Cancel the delayed ack timer when a segment is transmitted
05/02/02    mvl    File cleanup and changed calls to ip_send() to reflect new
                   prototype 
12/21/01    dwp    Add or !FEATURE_DATA_PS to some DSM tracer settings. This
                   is temporary until 5200 syncs up to latest DSM.
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Add "|| FEATURE_DATA_PS" to whole module.
10/09/01    ss     Removed the code to cancel the retransmission timer 
                   in tcp_output(). This fixes the bug when the timer is
                   cancelled but not started even though there is 
                   unacknowledged data.
07/23/01    pjb    Changed call to dsm_offset_new_buffer to reserve space 
                   for header.   Always set the push bit on TCP segments 
                   with data.  
02/14/01    js     Modified calls to ip_send() to support socket option
                   DSS_IP_TTL. 
11/11/00    snn    Removed errno from the timer functions because they are
                   no longer used.
11/02/00    snn    Modified the back off to be Exponential backoff.
10/10/00    na     Added support for Multiple TCP socket support.
08/25/00    rc     Handled the case when htontcp() returns NULL.
                   Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. So, if pool size is to be passed in as 
                   parameter, a new macro DSM_DS_POOL_SIZE is used to 
                   return pool id based on pool size.
08/17/99    smp    Added include files msg.h & err.h.
06/06/98    na     Added a dog_report() in the main loop of tcp_output().
01/26/98    ldg    Removed the DS_SLIM switch.
09/24/97    na     Changed tcp backoff to linear from exponential. Put a limit
                   of max 6 backoffs.
                   Put code to bring down call if Tcb gets hosed.
06/25/97    jgr    Added ifdef FEATURE_DS over whole file
11/14/96    jjw    Changes to support Unwired Planet (UDP operation)
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/13/95    jjw    Changed min/max macros to upper case
11/09/95    jjw    IS-99 Compliant Release
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined(FEATURE_DATA_PS)
#include "psglobal.h"
#include "ps_utils.h"
#include "dsm.h"
#include "netuser.h"
#include "internet.h"
#include "ps_tcp.h"
#include "ps_ip.h"
#include "dog.h"
#include "msg.h"
#include "err.h"
#include "pstimer.h"
#include "assert.h"


/*===========================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================
FUNCTION TCP_OUTPUT()

DESCRIPTION
  This function will send a TCP segment on the specified connection. One gets
  sent only if there is data to be sent or if "force" is non zero.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tcp_output
(
  register struct tcb *tcb
)
{
  pseudo_header_type  ph;            /* Pseudo-header for checksum calcs   */
  struct tcp          seg;           /* Local working copy of header       */
  dsm_item_type      *pkt_head_ptr;  /* Head pointer for header+data buffer*/
  uint32              usable;        /* Usable window                      */
  uint32              sent;          /* Sequence count (incl SYN/FIN) already
                                        in the pipe but not yet acked      */
  uint32              rto;           /* Retransmit timeout setting         */
  uint16              hsize;         /* Size of header                     */
  uint16              ssize;         /* Size of current segment being sent,
                                        including SYN and FIN flags        */
  uint16              dsize;         /* Size of segment less SYN and FIN   */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  int16               ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( tcb == NULL || tcb->state == TCP_LISTEN || tcb->state == TCP_CLOSED)
  {
    return;
  }

  for(;;)
  {
    /*-----------------------------------------------------------------------
      Compute data already in flight
    -----------------------------------------------------------------------*/
    sent = tcb->snd.ptr - tcb->snd.una;

    /*-----------------------------------------------------------------------
      Compute usable send window as minimum of offered and congestion
      windows, minus data already in flight.  Be careful that the window
      hasn't shrunk -- these are unsigned vars.
    -----------------------------------------------------------------------*/
    usable = MIN(tcb->snd.wnd,tcb->cwind);
    if(usable > sent)
    {
      usable -= sent; /* Most common case */
    }
    else if(usable == 0 && sent == 0)
    {
      usable = 1;  /* Closed window probe */
    }
    else
    {
      usable = 0;  /* Window closed or shrunken */
    }

    /*-----------------------------------------------------------------------
      Compute size of segment we *could* send. This is the smallest of the
      usable window, the mss, or the amount we have on hand. (I don't like
      optimistic windows)
    -----------------------------------------------------------------------*/
    ssize = MIN(tcb->sndcnt - sent,usable);
    ssize = MIN(ssize,tcb->mss);

    /*-----------------------------------------------------------------------
      Now we decide if we actually want to send it.  Apply John Nagle's
      "single outstanding segment" rule.  If data is already in the pipeline,
      don't send more unless it is MSS-sized, the very last packet, or we're
      being forced to transmit anyway (e.g., to ack incoming data).
    -----------------------------------------------------------------------*/
    if( !tcb->flags.force && !tcb->nodelay && sent != 0 && ssize < tcb->mss &&
        !(tcb->state == TCP_FINWAIT1 && ssize == tcb->sndcnt-sent))
    {
      ssize = 0;
    }

    /*-----------------------------------------------------------------------
      Unless the tcp syndata option is on, inhibit data until our SYN has
      been acked. This ought to be OK, but some old TCPs have problems with
      data piggybacked on SYNs.
    -----------------------------------------------------------------------*/
    if(!tcb->flags.synack && !TCP_SYNDATA)
    {
      if(tcb->snd.ptr == tcb->iss)
      {
        ssize = MIN(1,ssize);       /* Send only SYN */
      }
      else
      {
        ssize = 0;  /* Don't send anything */
      }
    }

    if(tcb->flags.force && tcb->snd.ptr != tcb->snd.nxt)
    {
      /*---------------------------------------------------------------------
        When forced during a retransmission, we must send just an empty ack
        with the current sequence number to ensure it's accepted
      ---------------------------------------------------------------------*/
      if(tcb->snd.ptr == tcb->iss)
      {
        ssize = MIN(1,ssize);       /* Send only SYN */
      }
      else
      {
        ssize = 0;  /* Don't send anything */
      }
    }

    if(ssize == 0 && !tcb->flags.force)
    {
      break;    /* No need to send anything */
    }
    tcb->flags.force = FALSE; /* Only one forced segment! */

    seg.source = tcb->conn.local.port;
    seg.dest   = tcb->conn.remote.port;

    /*-----------------------------------------------------------------------
      Set the flags according to the state we're in. It is assumed that if
      this segment is associated with a state transition, then the state
      change will already have been made. This allows this routine to be
      called from a retransmission timeout with force=1.
    -----------------------------------------------------------------------*/
    seg.flags.urg = 0;
    seg.flags.rst = 0;
    seg.flags.ack = 1; /* Every state except TCP_SYN_SENT */
    seg.flags.syn = 0; /* syn/fin/psh set later if needed */
    seg.flags.fin = 0;
    seg.flags.psh = 0;
    seg.flags.reserved = 0;
    seg.flags.congest = tcb->flags.congest;

    hsize      = TCPLEN; /* Except when SYN being sent */
    seg.mss    = 0;
    seg.optlen = 0;

    if(tcb->state == TCP_SYN_SENT)
    {
      seg.flags.ack = 0; /* Haven't seen anything yet */
    }

    dsize = ssize;

    if(!tcb->flags.synack && tcb->snd.ptr == tcb->iss)
    {
      /*---------------------------------------------------------------------
        Send SYN and MSS
      ---------------------------------------------------------------------*/
      seg.flags.syn = 1;
      dsize--;            /* SYN isn't really in snd queue */

      seg.mss = tcb->mss;
      hsize   = TCPLEN + MSS_LENGTH;
    }
    /*-----------------------------------------------------------------------
      When sending an empty ack, use the latest sequence number to ensure
      acceptance in case we've been retransmitting
    -----------------------------------------------------------------------*/
    if(ssize == 0)
    {
      seg.seq = tcb->snd.nxt;
    }
    else
    {
      seg.seq = tcb->snd.ptr;
    }
    seg.ack = tcb->rcv.nxt;
    seg.wnd = tcb->rcv.wnd;
    seg.up = 0;

    /*-----------------------------------------------------------------------
      Now try to extract some data from the send queue. Since SYN and FIN
      occupy sequence space and are reflected in sndcnt but don't actually
      sit in the send queue, extract will return one less than dsize if a FIN
      needs to be sent.
    -----------------------------------------------------------------------*/
    if(dsize != 0)
    {
      uint32 offset;                               /* offset into the sndq */
      uint32 copied = 0;      /* no. of bytes copied into the packet chain */
      uint32 extracted = 0;        /* no. of bytes extracted from the sndq */
      uint32 to_copy;            /* no. of bytes to copy into the dsm item */
      uint32 item_offset = PS_MAX_HDR;         /* offset into the dsm item */
      dsm_item_type *item_ptr;            /* tmp ptr to item to be chained */
      dsm_item_type *pkt_tail_ptr = NULL; /* tmp ptr to the pkt chain tail */
      dsm_mempool_id_enum_type pool_id;                     /* dsm pool id */

      pkt_head_ptr = NULL;

      offset = sent;
      /*---------------------------------------------------------------------
        SYN doesn't actually take up space on the sndq, so take it out of the
        sent count
      ---------------------------------------------------------------------*/
      if(!tcb->flags.synack && sent != 0)
      {
        offset--;
      }
      
      /*---------------------------------------------------------------------
        Allocate an appropriate size DSM item and copy data from the sndq.
        If the MSS is larger than the largest size DSM item available, we
        link the DSM items together to form a packet chain.       
      ---------------------------------------------------------------------*/
      while ( copied < dsize )
      {
        pool_id = DSM_DS_POOL_SIZE( dsize - copied + item_offset );
        item_ptr = dsm_offset_new_buffer(pool_id, item_offset);
        if (item_ptr == NULL)
        {
          ERR("Running out of buffers",0,0,0);
          dsm_free_packet( &pkt_head_ptr);
          return;
        }
        to_copy = MIN( item_ptr->size, dsize - copied );
        extracted = dsm_extract(tcb->sndq, 
                                offset, 
                                item_ptr->data_ptr, 
                                to_copy);
        copied += extracted;
        offset += extracted;
        item_ptr->used = extracted;

        /*-------------------------------------------------------------------
          Only the first item needs to be offset for the header
        -------------------------------------------------------------------*/
        item_offset = 0; 
        
        /*-------------------------------------------------------------------
          Form a packet chain. Note that we avoid calling dsm_append() for
          optimization reasons.
        -------------------------------------------------------------------*/
        if ( pkt_head_ptr == NULL )
        {
          /*-----------------------------------------------------------------
            This is the first item on the chain.
          -----------------------------------------------------------------*/
          pkt_head_ptr = item_ptr;
          pkt_tail_ptr = item_ptr;
        }
        else
        {
          pkt_tail_ptr->pkt_ptr = item_ptr;
          pkt_tail_ptr = item_ptr;
        }

        /*-------------------------------------------------------------------
          The only legal scenario for extracted not being equal to to_copy is 
          that we have run past the send queue and need to send a FIN. In
          such a case, we will extract one byte less than to_copy and tcp 
          state will be either FINWAIT1 or LAST_ACK.
        -------------------------------------------------------------------*/
        if ( extracted != to_copy )
        {
          /*-----------------------------------------------------------------
            Send a FIN. Without the check for states, this was the source of 
            many bugs.
          -----------------------------------------------------------------*/
          if (( extracted == (to_copy - 1)) && 
                ( TCP_FINWAIT1 == tcb->state || 
                  TCP_LAST_ACK == tcb->state ||
                  TCP_CLOSING == tcb->state ))
          {
            seg.flags.fin = 1;
            dsize--;
          }
          else
          {
            /*---------------------------------------------------------------
              Could happen if sendq is hosed, so just abort and clean up
            ---------------------------------------------------------------*/
            ERR("tcp sndq is corrupted", 0, 0, 0);
            dsm_free_packet( &pkt_head_ptr);
            dsm_free_packet( &( tcb->sndq));

            /*---------------------------------------------------------------
              No point in sending a reset, because tcb might be corrupted 
              too, so just close our TCP and then close PPP.
            ---------------------------------------------------------------*/
            close_self( tcb, RESET);
            return;
          }
          break;
        }
      }
    }
    else
    {
      pkt_head_ptr = NULL;
    }

    /*-----------------------------------------------------------------------
      If the entire send queue will now be in the pipe, set the push flag
     
      Note: Previous comment applies to following statement in the original
      TCP spec.  This statement applies when the commented section of the
      condition is included.  All modern TCP implementations set the push
      flag on all segments with data.
    -----------------------------------------------------------------------*/
    if(dsize != 0) /* ( && sent + ssize == tcb->sndcnt) */
    {
      seg.flags.psh = 1;
    }

    /*-----------------------------------------------------------------------
      If this transmission includes previously transmitted data, snd.nxt will
      already be past snd.ptr. In this case, compute the amount of
      retransmitted data and keep score
    -----------------------------------------------------------------------*/
    if(tcb->snd.ptr < tcb->snd.nxt)
    {
      tcb->resent += MIN(tcb->snd.nxt - tcb->snd.ptr,ssize);
    }
    tcb->snd.ptr += ssize;

    /*-----------------------------------------------------------------------
      If this is the first transmission of a range of sequence numbers,
      record it so we'll accept acknowledgments for it later
    -----------------------------------------------------------------------*/
    if(seq_gt(tcb->snd.ptr,tcb->snd.nxt))
    {
      tcb->snd.nxt = tcb->snd.ptr;
    }
    /*-----------------------------------------------------------------------
      Fill in fields of pseudo IP header
    -----------------------------------------------------------------------*/
    ph.source   = tcb->conn.local.address;
    ph.dest     = tcb->conn.remote.address;
    ph.protocol = TCP_PTCL;
    ph.length   = hsize + dsize;

    /*-----------------------------------------------------------------------
      Generate TCP header, compute checksum, and link in data. NOTE: if
      'pkt_head_ptr' is NULL it will be reassigned in 'htontcp' processing.
    -----------------------------------------------------------------------*/
    htontcp(&pkt_head_ptr, &seg,&ph);
    if (pkt_head_ptr == NULL)
    {
      ERR("Out of Memory",0,0,0);
      return;
    }

    /*-----------------------------------------------------------------------
      If we're sending some data or flags, start retransmission and round
      trip timers if they aren't already running.
    -----------------------------------------------------------------------*/
    if(ssize != 0)
    {
      uint32 n = tcb->backoff;

      rto = tcb->srtt;

      /*---------------------------------------------------------------------
        Backoff function -- the subject of much research
        Using exponential backoff.
        Since n is uint32 type, check if n is greater than 31 and limit to
        31.  Multiply the rto with 2^n. This makes it exponential backoff.
      ---------------------------------------------------------------------*/
      if (n > 31) 
      {
          n = 31;
      }
      else
      {
          n = (1L << n);
      }
       
      if( n > 0)
      {
         rto *= n;    
      }

      rto += tcb->mdev << 2;

      if ( rto > tcb->maxrto )
      {
        rto = tcb->maxrto;
      }

      /*---------------------------------------------------------------------
        Set round trip timer.
      ---------------------------------------------------------------------*/
      tcb->tcp_rexmit_timer_val = MAX(tcb->minrto,rto);
      
      if( ps_timer_is_running(tcb->timer) == FALSE) 
      {
        timer_ret_val = ps_timer_start( tcb->timer, tcb->tcp_rexmit_timer_val);
        ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
      }
  
      /*---------------------------------------------------------------------
        If round trip timer isn't running, start it
      ---------------------------------------------------------------------*/
      if(!tcb->flags.rtt_run)
      {
        tcb->flags.rtt_run = TRUE;
        tcb->rtt_time      = msclock();
        tcb->rttds         = ph.length;
        tcb->rttseq        = tcb->snd.ptr;
        tcb->rttack        = tcb->snd.una;
      }
    }
    if(tcb->flags.retran)
    {
      tcpRetransSegs++;
    }
    else
    {
      tcpOutSegs++;
    }

#ifdef FEATURE_DSM_MEM_CHK
      pkt_head_ptr->tracer = DSM_IP_SND_PROC;
#endif //FEATURE_DSM_MEM_CHK

    MSG_LOW("TCP Out: seq = %x, ack = %x", seg.seq, seg.ack, 0);

#ifndef FEATURE_DS_TCP_NO_DELAY
    if ( ps_timer_is_running(tcb->acker) )
    {
      MSG_MED("Cancelling TCP Ack Timer", 0, 0, 0);
      timer_ret_val = ps_timer_cancel(tcb->acker);
      ASSERT(timer_ret_val == PS_TIMER_SUCCESS);
    }
#endif /* FEATURE_DS_TCP_NO_DELAY */

#ifdef FEATURE_DS_SOCKETS
    /*-----------------------------------------------------------------------
      Check that sockfd is valid if the scb pointer is NULL
    -----------------------------------------------------------------------*/
    if (tcb->scb_ptr != NULL)
    {
      ASSERT(tcb->scb_ptr->sockfd != SOCKAVAIL);
    }

    /*-----------------------------------------------------------------------
      Call ip_send with customized or default TTL value depending on whether
      the scb_ptr is NULL or not
    -----------------------------------------------------------------------*/
    ip_send( tcb->conn.local.address,
             tcb->conn.remote.address,
             TCP_PTCL,
             tcb->tos,
             tcb->scb_ptr?tcb->scb_ptr->ip_ttl : 0,
             pkt_head_ptr,
             ph.length,
             (tcb->scb_ptr ? (tcb->scb_ptr->ipsec_hdr_size) : 0),
             0,
             NULL,
             (tcb->routing_cache != NULL || tcb->scb_ptr == NULL) ? 
               tcb->routing_cache : tcb->scb_ptr->routing_cache,
             &ps_errno);
#else

    ip_send( tcb->conn.local.address,
             tcb->conn.remote.address,
             TCP_PTCL,
             tcb->tos,
             0,
             pkt_head_ptr,
             ph.length,
             0,
             0,
             NULL,
             tcb->routing_cache,
             &ps_errno);
#endif

    dog_report( DOG_PS_RPT);

  } /* for(ever) */

} /* tcp_output() */
#endif /* FEATURE_DS || FEATURE_DATA_PS */
