/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ T C P I N . C

GENERAL DESCRIPTION
  Process incoming TCP segments. Page number references are to ARPA RFC-793,
  the TCP specification.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 * Copyright 1991-1993 Phil Karn, KA9Q
 * Additional material Copyright 1992-1995 William Allen Simpson

Copyright (c) 1995-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpin.c_v   1.9   20 Feb 2003 10:25:36   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_tcpin.c#3 $ $DateTime: 2008/11/25 06:15:45 $ $Author: spathuru $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/25/08   sp      Fix for DSM item check before reseq the TCP packet.
04/03/08   pp      Klocwork fixes.
10/18/07   sn      Replaced DSM_SMALL_ITEM_LESS_THAN_TRANSPORT_FEW and 
                   DSM_LARGE_ITEM_LESS_THAN_TRANSPORT_FEW macros with 
                   DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS macro. Pulled 
                   in this change from tip.
05/23/06   squ     Featurized async data
03/17/04   sv      Changed proc_syn to set mss to minimum of user specified
                   MSS and receiver advertised MSS.
03/01/04   sv      Fixed congestion window update bug in update function.
11/05/03   sv      Modified tx_upcall function to pass number of bytes acked
                   instead of available rcv window size.
06/27/03   sv      Removed the code that uses limit field in tcb to limit
                   the sndq size. It is now handled by sndbuf socket option.
06/04/03   om      Fixed MSS setting to adjust for IPSec ESP overhead.
06/02/03   ss      Renamed TCP_NO_DELAY to FEATURE_DS_TCP_NO_DELAY.
02/20/03   om      Removed debug message.
02/12/03   om      Setting routing cache in TCB.
11/13/02   rc      Wrapped code in !FEATURE_GSM_GPRS
09/25/02   om      Set the routing cache for LISTEN & unbound sockets.
09/20/02   na      Fix: In tcp_input(), for sockets calls when a ACK is recvd
                   tcp_output is called.
08/30/02   om      Updates to use receive iface for sending a TCP reset.
08/01/02   om      Removed dependecy on if_cb for interface's MTU.
07/31/02   usb     Added include ps_iputil.h
07/31/02   usb     Removed include files ps.h and psi.h, renamed psmisc.h
                   to ps_utils.h
07/15/02   aku     Replaced ps_in_rawdata_mode() with 
                   ps707_async_in_rawdata_mode() and ps_process_tcp_send_q()
                   with ps707_async_process_tcp_send_q()
07/10/02   ss      Made changes to incorporate PS timer changes
05/02/02   mvl     File cleanup and changed calls to ip_send() to reflect new
                   prototype
05/29/02   vr      fixed tcpin to do differential processing only if both
                   small and large pools hit transport few
04/17/02   rc      Wrapped code in !FEATURE_DATA_WCDMA_PS
03/05/02    pjb    Finished Accept Listen Code Review Items 
02/22/02    pjb    Added accept and listen
02/11/02   ss      Introduced processing of ack and discarding data of 
                   incoming segments when running low on memory.
                   tcp_cleanup_reseq() now cleans up whole reseq queue. 
12/21/01   dwp     Add or !FEATURE_DATA_PS to some DSM tracer settings. This
                   is temporary until 5200 syncs up to latest DSM.
12/18/01   rsl     Updated tracer field.
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module. Wrap out 
                   ps_process_tcp_send_queue() if FEATURE_DATA_PS is defined.
11/05/01   vr      Add call to reset if packet recvd for invalid connection
10/09/01   ss      Modified update() to cancel the timer before 
                   retransmitting due to triple duplicate acks. Also, when
                   some data is acked, ps_timer_cancel() is now called 
                   only when there is data in flight.
08/08/01   aku     Removed call to reset on reception of packet for invalid
                   connection
07/23/01   pjb     Changed call to dsm_offset_new_buffer to reserve space 
                   for header. Save the reserved bit from the TCP header.  
06/14/01   snn     Fixed the bug in tcp_input while reading the resequence 
                   queue. It was doing an extra network to host (get32) 
                   conversion on the sequence number.
01/19/01   na      Fixed bug in add_reseq where a extra network to host(get32)
                   was being done. Fixed some F3 msgs.
11/11/00   snn     Removed errno from the timer functions because they are
                   no longer used.
10/10/00   na/snn  Removed tcp_rexmit_timer_val as a global variable and 
                   added to the TCB. This was done because each TCP socket
                   can have its own timer value.
08/25/00   rc      Changed dsm_pushdown() to take take pool id as fourth 
                   parameter.Made changes to handle dsm_pushdown() 
                   returning FALSE.                                    
                   Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. Added Macros that check if we are running 
                   out of items.
                   Handled the case when htontcp() returns NULL.
                   Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK                   
06/07/00   rc      Removed ovld of link ptr in dsm items.
02/02/00   ak      Merged branch and tip.
12/23/99   na      Changed queue functions that use dsm items, to use the 
                   ovld of link ptr in dsm items.
12/06/99   hcg     In tcp_cleanup_reseq(), changed function call from 
                   dsm_q_last_get() to q_last_get() as the function is now 
                   provided as part of queue services.
11/16/99   rc      Fixed mis-aligned access problem in add_reseq() and 
                   tcp_input().
11/04/99   rc      Added code to make sure that if we get a FIN, regardless of 
                   RST, we return DS_EEOF when finished reading from the pipe, 
                   rather than returning DS_ECONNRESET.
04/25/99   hcg     Added INTLOCK()/INTFREE() around updates for TCB rcvq.  Fixes
                   task synchronization problem for sockets interface.  Also
                   added packet trimming to ensure length of packet is consistent.
04/09/99   hcg     Added FEATURE_Q_NO_SELF_QPTR to support queue services memory 
                   optimization.
03/31/99   hcg     Changed MSG_MED to MSG_LOW for duplicate ACKs received to
                   reduce number of DM messages during ASYNC calls.
02/25/99   na/hcg  Added support for DSSock sockets interface under 
                   FEATURE_DS_SOCKETS.  Added code for tcp_input() routine.
                   Also, added code to free dsm items, which could lead to
                   memory leaks.
12/14/98   ldg     Removed dangling comma from tcp_cleanup_reseq() parameter
12/05/98   na      Modified tcp_cleanup_reseq to clean one item at a time.
10/30/98   na      Added tcp_cleanup_reseq to clean up the resequence queue.
10/15/98   na      Changes for improved bidirectional performance - call
                   ps_process_tcp_send_queue instead of tcp_output(). 
07/26/98   jjn     Use more efficient DSM.
06/17/98   ldg     Stop waiting until backoffs=0 to copy backoffs to history.
05/06/98   na      Locked interrupts around changes in the sndcnt variable.
04/14/98   ldg     Added counters for payload received, and backoffs.
04/14/98   na      Added support for buffer based flow control.
01/26/98   ldg     Removed the DS_SLIM switch.
09/19/97   na      Fixed TCP problem where we were getting TCP segment which
                   passes the checksum, but the actual length does not match
                   the length in the IP header.
09/09/97   ldg     (na) Send ACK before passing segment to higher layer.
07/31/97   ldg     Added counter for segments received out of order.
06/17/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
11/14/96   jjw     Changes to support Unwired Planet (UDP operation)
10/08/96   jjw     Disabled Delayed ACK
07/08/96   jjw     Limited MSS setting to the DEF_MSS.
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)
#include "psglobal.h"
#include "ps_utils.h"
#include "dsm.h"
#include "netuser.h"
#include "internet.h"
#include "ps_tcp.h"
#include "ps_icmp.h"
#include "ps_ip.h"
#include "ps_iputil.h"
#include "iface.h"
#include "assert.h"
#include "memory.h"
#include "msg.h"
#include "err.h"

#ifdef FEATURE_DS_SOCKETS
#include "dsstcp.h"
#endif
#include "pstimer.h"
#include "assert.h"

#if defined(FEATURE_DATA_IS707) && !defined(FEATURE_ASYNC_DATA_NOOP) 
  #include "ps707_async.h"
#endif /* FEATURE_DATA_IS707 && !FEATURE_ASYNC_DATA_NOOP*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*===========================================================================

FUNCTION IN_WINDOW()

DESCRIPTION
  This function will Determine if the given sequence number is in our
  receiver window. 

DEPENDENCIES
  Must not be used when window is closed!

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static int in_window
(
  struct tcb *tcb,
  uint32     seq
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return seq_within(seq,tcb->rcv.nxt,(uint32)(tcb->rcv.nxt+tcb->rcv.wnd-1));
} /* in_window() */

/*===========================================================================

FUNCTION SEND_SYN()

DESCRIPTION
  This function will Generate an initial sequence number and put a SYN on the
  send queue 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void send_syn
( 
  register struct tcb *tcb
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  tcb->iss         = geniss();
  tcb->rttseq      = tcb->snd.wl2 = tcb->snd.una = tcb->iss;
  tcb->snd.ptr     = tcb->snd.nxt = tcb->rttseq;
  tcb->sndcnt++;
  tcb->flags.force = 1;
} /* send_syn() */

/*===========================================================================

FUNCTION TCP_CLEANUP_RESEQ()

DESCRIPTION
  This function cleans up all the elements from the TCP resequencing 
  queue for the given TCP connection

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_cleanup_reseq
(
  register struct tcb *tcb        /* ptr to TCP Task control block         */
)
{
   dsm_item_type* item_ptr;                          /* temporary item ptr */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Cleanup TCP reseq. queue.
   ------------------------------------------------------------------------*/
  while (( item_ptr = (dsm_item_type *)q_last_get( &(tcb->reseq))) != NULL)
  {
    (void)dsm_free_packet( &item_ptr);
  }

} /* tcp_cleanup_reseq() */

/*===========================================================================

FUNCTION GET_RESEQ()

DESCRIPTION
  This function will Fetch the first entry off the resequencing queue. This
  includes getting the segment data, loading the TCP Segment data structure
  with accompanying values, and loading the Segment data length variable.

DEPENDENCIES
1)The format of the data on the Re-sequencing queue must contain the TCP
  segment data structure values and the segment data length stored in front
  of the actual data, respectively.
2)There must be a Re-seq data item on the Re-sequencing queue.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void get_reseq
(
  register struct tcb *tcb,       /* ptr to TCP Task control block         */
  uint8               *tos,       /* ptr to Type of service variable       */
  struct tcp          *seg,       /* ptr to Segment data structure to load */
  struct dsm_item_s   **data_ptr, /* Addr of ptr for Retrieval of seg data */
  uint16              *length     /* ptr to length field of retrieved data */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set the Type of service then pullup the TCP Segment data  values into
    the passed data structure then do the same with the segment data length.
  -------------------------------------------------------------------------*/
  if((*data_ptr = (dsm_item_type *)q_get(&tcb->reseq)) == NULL)
  {
    ERR_FATAL( "Missing Re-seq. Data Item", 0, 0, 0);
  }
  *tos = (*data_ptr)->priority;
  (void)dsm_pullup( data_ptr, seg, sizeof(struct tcp));
  (void)dsm_pullup( data_ptr, length, sizeof(uint16));
} /* get_reseq() */

/*===========================================================================

FUNCTION ADD_RESEQ()

DESCRIPTION
  This function will add the passed TCP Segment onto the TCP Re-Sequencing
  queue. Along with the segment the segment length and the TCP head info
  is saved away.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The data item on the Re-sequencing queue will store the data item payload
  length and the TCP Segment data structure values.
===========================================================================*/

static void add_reseq
(
  struct tcb        *tcb,                 /* TCP control block for segment */
  uint8             tos,                  /* Type of service value         */
  struct tcp        *seg,                 /* TCP Seg data struct ptr       */
  struct dsm_item_s *seg_data_ptr,        /* ptr to TCP segment data       */
  uint16            length                /* length of TCP segment data    */
)
{
  dsm_item_type *reseq_item_ptr;          /* for looking thru Re-Seq list  */
  struct tcp     reseq_seg;               /* TCP hdr in reseq              */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
  /*-------------------------------------------------------------------------
    Push Data length and TCP header info (includes Sequence number) onto Data
    item for use during later Re-sequencing.
  -------------------------------------------------------------------------*/
  if (dsm_pushdown( &seg_data_ptr, &length, sizeof(uint16), 
                                          DSM_DS_SMALL_ITEM_POOL) < 
      sizeof(uint16))
  {
    ERR("Out of Memory!",0,0,0);
    dsm_free_packet(&seg_data_ptr);
    return;
  }
  if (dsm_pushdown( &seg_data_ptr, seg, sizeof(struct tcp),
                                          DSM_DS_SMALL_ITEM_POOL) < 
      sizeof(struct tcp))
  {
    ERR("Out of Memory!",0,0,0);
    dsm_free_packet(&seg_data_ptr);
    return;
  }
  seg_data_ptr->priority = tos;
 
  /*-------------------------------------------------------------------------
    Place complete 'package' onto the Re-sequencing queue in ascending order
    using the Sequence number. Start by finding correct spot in the list.
  -------------------------------------------------------------------------*/
  reseq_item_ptr = (dsm_item_type *)q_check( &tcb->reseq);
  while( reseq_item_ptr != NULL)
  {
    (void)dsm_extract(reseq_item_ptr, 0, &reseq_seg, sizeof(struct tcp));
 
    /*-----------------------------------------------------------------------
      If Seq# of received segment is less than the Seq# of current segment on
      the resequencing queue, then break from the while loop. This will
      ensure that the received segment gets inserted before current segment
      on the resequencing queue.
    -----------------------------------------------------------------------*/
    if( seq_lt( seg->seq, reseq_seg.seq))
    {
      break;  
    } 
    reseq_item_ptr = 
      (dsm_item_type *)q_next( &tcb->reseq, &reseq_item_ptr->link);
  } /* while */
 
  if ( reseq_item_ptr == NULL)
  {
    /*-----------------------------------------------------------------------
      Sequence number of passed segment > last item on list. Put to list end.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "PUT to Reseq: %lx, nxt=%lx", seg->seq, tcb->rcv.nxt, 0);
#if defined (FEATURE_DSM_MEM_CHK) && !defined (FEATURE_DATA_WCDMA_PS) && \
                                     !defined (FEATURE_GSM_GPRS)
    seg_data_ptr->tracer= DSM_TCP_RESEQ;
#endif //FEATURE_DSM_MEM_CHK

    q_put( &tcb->reseq, &seg_data_ptr->link);
  }
  else  /* Pointing at Seg with a greater Seq number. Insert prior to it. */
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert( &tcb->reseq, 
              &seg_data_ptr->link,
              &reseq_item_ptr->link);
#else
    q_insert(  &seg_data_ptr->link,
               &reseq_item_ptr->link);
#endif
    MSG_HIGH( "INSERT to Reseq: %lx, nxt=%lx", seg->seq, tcb->rcv.nxt, 0);
  } 
} /* add_reseq() */

/*===========================================================================

FUNCTION SEND_RESET()

DESCRIPTION
  This function will Send an acceptable reset (RST) response for this segment
  The RST reply is composed in place on the input segment

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void send_reset
(
  struct ip           *ip,             /* Offending IP header */
  register struct tcp *seg,            /* Offending TCP header */
  ps_iface_type       *iface           /* Iface that received bad hdr */
)
{
  struct dsm_item_s    *header_ptr = NULL;  /* ptr to item with TCP header */
  pseudo_header_type ph;                    /* holds IP header info        */
  uint16 tmp;                               /* temp source port holder     */
  int16  ps_errno;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(seg->flags.rst)
  {
    return; /* Never send an RST in response to an RST */
  }

  /*-------------------------------------------------------------------------
    Compose the RST IP pseudo-header, swapping addresses
  -------------------------------------------------------------------------*/
  ph.source   = ip->dest;
  ph.dest     = ip->source;
  ph.protocol = TCP_PTCL;
  ph.length   = TCPLEN;

  /*-------------------------------------------------------------------------
    Swap port numbers
  -------------------------------------------------------------------------*/
  tmp         = seg->source;
  seg->source = seg->dest;
  seg->dest   = tmp;

  if(seg->flags.ack)
  {
    /*-----------------------------------------------------------------------
      This reset is being sent to clear a half-open connection.  Set the
      sequence number of the RST to the incoming ACK so it will be
      acceptable.
    -----------------------------------------------------------------------*/
    seg->flags.ack = 0;
    seg->seq       = seg->ack;
    seg->ack       = 0;
  } 
  else 
  {
    /*-----------------------------------------------------------------------
      We're rejecting a connect request (SYN) from TCP_LISTEN state so we
      have to "acknowledge" their SYN.
    -----------------------------------------------------------------------*/
    seg->flags.ack = 1;
    seg->ack       = seg->seq;
    seg->seq       = 0;
    if(seg->flags.syn)
    {
      seg->ack++;
    }
  }
  /*-------------------------------------------------------------------------
    Set remaining parts of packet
  -------------------------------------------------------------------------*/
  seg->flags.urg = 0;
  seg->flags.psh = 0;
  seg->flags.rst = 1;
  seg->flags.syn = 0;
  seg->flags.fin = 0;
  seg->wnd       = 0;
  seg->up        = 0;
  seg->mss       = 0;
  seg->optlen    = 0;
  htontcp( &header_ptr, seg, &ph);
  /*---------------------------------------------------------------------------
    Check to see if we ran out of items.
  ---------------------------------------------------------------------------*/
  if (header_ptr != NULL)
  {
    /*-----------------------------------------------------------------------
      Ship it out (note swap of addresses)
    -----------------------------------------------------------------------*/
    (void)ip_send(ip->dest,
                  ip->source,
                  TCP_PTCL,
                  ip->tos,
                  0,
                  header_ptr,
                  ph.length,
                  0,
                  0,
                  NULL,
                  iface,
                  &ps_errno);
    tcpOutRsts++;
  }
} /* send_reset() */

/*===========================================================================

FUNCTION TRIM()

DESCRIPTION
  This function will Trim the passed segment to fit the TCP window.

DEPENDENCIES
  None

RETURN VALUE
  0 if the trim yielded payload data bytes, else -1

SIDE EFFECTS
  None
===========================================================================*/

static sint15 trim
(
  register struct tcb *tcb,          /* ptr to TCP control block           */
  register struct tcp *seg,          /* ptr to TCP segment info            */
  struct dsm_item_s   **data_ptr,    /* address of ptr to TCP segment data */
  uint16              *length        /* ptr to length of TCP segment       */
)
{
  sint31 dupcnt;                   /* holds front end duplicate byte count */
  sint31 excess;                   /* holds excess from end of segment     */
  uint16 len;                      /* Segment length including flags       */
  uint8  accept = 0;               /* Accept/ No accept flag               */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  len = *length;
  if(seg->flags.syn)
  {
    len++;
  }
  if(seg->flags.fin)
  {
    len++;
  }
  /* Acceptability tests */
  if(tcb->rcv.wnd == 0)
  {
    /* Only in-order, zero-length segments are acceptable when
     * our window is closed.
     */
    if(seg->seq == tcb->rcv.nxt && len == 0)
    {
      return 0;  /* Acceptable, no trimming needed */
    }
  }
  else 
  {
    /* Some part of the segment must be in the window */
    if( in_window( tcb, seg->seq))
    {
      accept++;  /* Beginning is */
    } 
    else if( len != 0)
    {
      if( in_window( tcb, (uint32)(seg->seq+len-1)) ||     /* End is */
          seq_within( tcb->rcv.nxt,seg->seq, (uint32)(seg->seq+len-1)))
      { /* Straddles */
        accept++;
      }
    }
  }
  if(!accept)
  {
    MSG_MED( "Seg: %lx not in window",seg->seq, 0, 0);
    tcb->rerecv += len;  /* Assume all of it was a duplicate */
    dsm_free_packet(data_ptr);
    return -1;
  }
  if((dupcnt = (sint31)(tcb->rcv.nxt - seg->seq)) > 0)
  {
    tcb->rerecv += dupcnt;
    /* Trim off SYN if present */
    if(seg->flags.syn)
    {
      /* SYN is before first data byte */
      seg->flags.syn = 0;
      seg->seq++;
      dupcnt--;
    }
    if(dupcnt > 0)
    {
      MSG_MED( "Trimming %d bytes from Seg front", dupcnt, 0, 0);
      (void)dsm_pullup(data_ptr,NULL,(uint16)dupcnt);
      seg->seq += dupcnt;
      *length  -= dupcnt;
    }
  }
  if((excess = (sint31)(seg->seq + *length - (tcb->rcv.nxt+tcb->rcv.wnd))) > 0)
  {
    /* Trim right edge */
    MSG_MED( "Trimming %d bytes from Seg end", excess, 0, 0);
    tcb->rerecv   += excess;
    *length       -= excess;
    (void)dsm_trim_packet(data_ptr,*length);
    seg->flags.fin = 0;  /* FIN follows last data byte */
  }
  return 0;
} /* trim() */

/*===========================================================================

FUNCTION PROC_SYN()

DESCRIPTION
  This function will Process an incoming SYN


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void proc_syn
(
  register struct tcb *tcb,                /* pointer to TCP control block */
  uint8               tos,                 /* Type of service variable     */
  struct   tcp        *seg                 /* pointer to TCP header info   */
)
{
  uint16         mtu;           /*  */
  struct tcp_rtt *tp;           /*  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  tcb->flags.force = 1;  /* Always send a response */

  /* Note: It's not specified in RFC 793, but SND.WL1 and
   * SND.WND are initialized here since it's possible for the
   * window update routine in update() to fail depending on the
   * IRS if they are left unitialized.
   */
  /* Check incoming precedence and increase if higher */
  if(PREC(tos) > PREC(tcb->tos))
  {
    tcb->tos = tos;
  }
  tcb->rcv.nxt = seg->seq + 1;  /* p 68 */
  tcb->snd.wl1 = tcb->irs = seg->seq;
  tcb->snd.wnd = seg->wnd;
  if(seg->mss != 0)
  {
    /*----------------------------------------------------------
      Set the Max segment size (MSS) to no greater than DEF_MSS,
      as the memory pool large items are fitted to DEF_MSS
    ----------------------------------------------------------*/
    tcb->mss = MIN(seg->mss, tcb->mss);  //DDHACK fix
  }

  /* Check the MTU of the interface we'll use to reach this guy
   * and lower the MSS so that unnecessary fragmentation won't occur
   */
  if((mtu = 1500) != 0)  // Get this from ps_iface.netinfo
  {
    /* Allow space for the TCP and IP headers */
    mtu        -= (TCPLEN + IPLEN + tcb->scb_ptr->ipsec_hdr_size);
    tcb->cwind  = tcb->mss = MIN(mtu,tcb->mss);
  }

  /* See if there's round-trip time experience */
  if((tp = rtt_get(tcb->conn.remote.address)) != NULL)
  {
    tcb->srtt = tp->srtt;
    tcb->mdev = tp->mdev;
  }
} /* proc_syn() */

/*===========================================================================

FUNCTION UPDATE()   

DESCRIPTION
  This function will process an incoming acknowledgement and window indication.
  See RFC 793 (TCP) page 72.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void update
(
  register struct tcb *tcb,                /* pointer to TCP control block */
  register struct tcp *seg,                /* pointer to TCP header info   */
  uint16              length               /* length of data for segment   */
)
{
  uint32 acked  = 0L;
  int    winupd = FALSE;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(seq_gt( seg->ack, tcb->snd.nxt))
  {
    MSG_HIGH( "ACKing Bogus Segment", 0, 0, 0);
    tcb->flags.force = 1;  /* Acks something not yet sent */
    return;
  }
  /* Decide if we need to do a window update.
   * This is always checked whenever a legal ACK is received,
   * even if it doesn't actually acknowledge anything,
   * because it might be a spontaneous window reopening.
   */
  if( seq_gt( seg->seq,tcb->snd.wl1) || (( seg->seq == tcb->snd.wl1) && 
      seq_ge( seg->ack,tcb->snd.wl2)))
  {
    if(seg->wnd > tcb->snd.wnd)
    {
      winupd = 1;  /* Don't count as duplicate ack */

      /* If the window had been closed, crank back the send
       * pointer so we'll immediately resume transmission.
       * Otherwise we'd have to wait until the next probe.
       */
      if(tcb->snd.wnd == 0)
      {
        MSG_MED( "Send Window Zero", 0, 0, 0);
        tcb->snd.ptr = tcb->snd.una;
      }
    }
    /* Remember for next time */
    tcb->snd.wnd = seg->wnd;
    tcb->snd.wl1 = seg->seq;
    tcb->snd.wl2 = seg->ack;
  }
  /* See if anything new is being acknowledged */
  if(seq_lt(seg->ack,tcb->snd.una))
  {
    MSG_MED("Old ACK Rx'd: %lx, una: %lx",seg->ack, tcb->snd.una, 0);
    return; /* Old ack, ignore */
  }

  if(seg->ack == tcb->snd.una)
  {
    /* Ack current, but doesn't ack anything */
    if( tcb->sndcnt == 0 || winupd || length != 0 || seg->flags.syn || 
        seg->flags.fin)
    {
       MSG_LOW("Duplicate ACK Rx'd (Good Reason)", 0, 0, 0);
      /* Either we have nothing in the pipe, this segment
       * was sent to update the window, or it carries
       * data/syn/fin. In any of these cases we
       * wouldn't necessarily expect an ACK.
       */
      return;
    }

    MSG_MED("Duplicate ACK Rx'd (apparently BOGUS)" ,0,0, 0);
    /* Van Jacobson "fast recovery" code */
    if(++tcb->dupacks == TCPDUPACKS)
    {
      /* We've had a burst of do-nothing acks, so
       * we almost certainly lost a packet.
       * Resend it now to avoid a timeout. (This is
       * Van Jacobson's 'quick recovery' algorithm.)
       */
       uint32 ptrsave;

      /* Knock the threshold down just as though
       * this were a timeout, since we've had
       * network congestion.
       */
      tcb->ssthresh = tcb->cwind/2;
      tcb->ssthresh = MAX(tcb->ssthresh,tcb->mss);

      /* Manipulate the machinery in tcp_output() to
       * retransmit just the missing packet
       */
      ptrsave      = tcb->snd.ptr;
      tcb->snd.ptr = tcb->snd.una;
      tcb->cwind   = tcb->mss;

      /* We want to cancel the timer here otherwise 
       * tcp_output() would find that the timer is 
       * already running and won't restart it. This 
       * would result in timer firing relative to the
       * original transmission and not retransmission.
       */

      timer_ret_val = ps_timer_cancel(tcb->timer);
      ASSERT( timer_ret_val == PS_TIMER_SUCCESS );

      MSG_MED( "Retransmitting due to triple dupacks", 0, 0, 0);
      tcp_output(tcb);
      tcb->snd.ptr = ptrsave;

      /* "Inflate" the congestion window, pretending as
       * though the duplicate acks were normally acking
       * the packets beyond the one that was lost.
       */
      tcb->cwind = tcb->ssthresh + TCPDUPACKS*tcb->mss;
    } 
    else if(tcb->dupacks > TCPDUPACKS)
    {
      /* Continue to inflate the congestion window
       * until the acks finally get "unstuck".
       */
      tcb->cwind += tcb->mss;
    }
    /* Clamp the congestion window at the amount currently
     * on the send queue, with a minimum of one packet.
     * This keeps us from increasing the cwind beyond what
     * we're actually putting in the pipe; otherwise a big
     * burst of data could overwhelm the net.
     */
    tcb->cwind = MIN(tcb->cwind,tcb->sndcnt);
    tcb->cwind = MAX(tcb->cwind,tcb->mss);
    return;
  }
  /* We're here, so the ACK must have actually acked something */

  MSG_LOW( "Received ACK: %d bytes",seg->ack-tcb->snd.una,0,0);
  if(tcb->dupacks >= TCPDUPACKS && tcb->cwind > tcb->ssthresh)
  {
    /* The acks have finally gotten "unstuck". So now we
     * can "deflate" the congestion window, i.e. take it
     * back down to where it would be after slow start
     * finishes.
     */
    tcb->cwind = tcb->ssthresh;
  }
  tcb->dupacks = 0;
  acked        = seg->ack - tcb->snd.una;

  /* Expand congestion window if not already at limit and if
   * this packet wasn't retransmitted
   */
  if(tcb->cwind < tcb->snd.wnd && !tcb->flags.retran)
  {
    if(tcb->cwind < tcb->ssthresh)
    {
      /* Still doing slow start/CUTE, expand by amount acked */
      tcb->cwind += MIN(acked,tcb->mss);
    } 
    else 
    {
      /* Steady-state test of extra path capacity */
      tcb->cwind += (tcb->mss * tcb->mss) / tcb->cwind;
    }
    /* Don't expand beyond the offered window */
    if(tcb->cwind > tcb->snd.wnd)
    {
      tcb->cwind = tcb->snd.wnd;
    }
  }
  tcb->cwind = MAX(tcb->cwind,tcb->mss);

  /* Round trip time estimation */
  if(tcb->flags.rtt_run && seq_ge(seg->ack,tcb->rttseq))
  {
    /* A timed sequence number has been acked */
    tcb->flags.rtt_run = 0;
    if(!(tcb->flags.retran))
    {
      uint32 rtt;      /* measured round trip time */
      uint32 abserr;   /* abs(rtt - srtt) */

      /* This packet was sent only once and now
       * it's been acked, so process the round trip time
       */
      rtt      = msclock() - tcb->rtt_time;
      tcb->rtt = rtt; /* Save for display */

      abserr = (rtt > tcb->srtt) ? rtt - tcb->srtt : tcb->srtt - rtt;

      /* Run SRTT and MDEV integrators, with rounding */

      tcb->srtt = ((AGAIN-1)*tcb->srtt + rtt + (AGAIN/2)) >> LAGAIN;
      tcb->mdev = ((DGAIN-1)*tcb->mdev + abserr + (DGAIN/2)) >> LDGAIN;

      rtt_add( tcb->conn.remote.address, rtt);
      
      /* Reset the backoff level */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
      tcb->backoff = 0;

      /* Update our tx throughput estimate */
      
      if(rtt != 0)  /* Avoid division by zero */
      {
        tcb->txbw = 1000*(seg->ack - tcb->rttack)/rtt;
      }
    }
  }

  INTLOCK();

  tcb->sndcnt  -= acked;  /* Update virtual byte count on snd queue */
  tcb->snd.una  = seg->ack;

  INTFREE();

  /* If we're waiting for an ack of our SYN, note it and adjust count */
  if(!(tcb->flags.synack))
  {
    tcb->flags.synack = 1;
    acked--;  /* One less byte to pull from real snd queue */
  }
  /* Remove acknowledged bytes from the send queue and update the
   * unacknowledged pointer. If a FIN is being acked,
   * pullup won't be able to remove it from the queue, but that
   * causes no harm.
   */

   (void)dsm_pullup(&tcb->sndq,NULL,(uint16)acked);

  /* If there are outstanding segments, start the retransmission
   * timer. Otherwise, there is no data in flight and we cancel
   * the timer.
   */
   
   if(tcb->snd.una != tcb->snd.nxt) 
   {
     /* There are segments in flight. Start the retransmission timer.
      */

     timer_ret_val = ps_timer_start( tcb->timer, 
                                     tcb->tcp_rexmit_timer_val
                                   );
     ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
   }
   else
   {
     timer_ret_val = ps_timer_cancel( tcb->timer);
     ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
   }


  /* If retransmissions have been occurring, make sure the
   * send pointer doesn't repeat ancient history
   */
  if(seq_lt(tcb->snd.ptr,tcb->snd.una))
  {
   tcb->snd.ptr = tcb->snd.una;
  }

  /* Clear the retransmission flag since the oldest
   * unacknowledged segment (the only one that is ever retransmitted)
   * has now been acked.
   */
  tcb->flags.retran = 0;

  /* If outgoing data was acked, notify the user so he can send more
   * unless we've already sent a FIN.
   */

   if( acked != 0 && tcb->t_upcall && (tcb->state == TCP_ESTABLISHED || 
      tcb->state == TCP_CLOSE_WAIT))
  {
    (*tcb->t_upcall)(tcb, acked);
  }

} /* update() */


/*===========================================================================

FUNCTION UPDATE_MINIMUM()   

DESCRIPTION
  This function performs the minimum processing of an incoming 
  acknowledgement. Basically, this function is called when we want to process
  the acknowledgement but not the data in an incoming segment in some special
  circumstances (e.g., running low on memory). Mainly, this function removes 
  the acknowledged bytes from the sndq. For all the other purposes (e.g.,
  counting duplicate acks), we pretend that we never saw this segment. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void update_minimum
(
  register struct tcb *tcb,                /* pointer to TCP control block */
  register struct tcp *seg,                /* pointer to TCP header info   */
  uint16              length               /* length of data for segment   */
)
{
  uint32 acked  = 0L;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_MED( "entering update_minimum()", 0, 0, 0);

  /*------------------------------------------------------------------------  

  ------------------------------------------------------------------------*/
  if ( seg->flags.syn || seg->flags.fin || seg->flags.rst )
  {
    return;
  }

  if(seq_gt( seg->ack, tcb->snd.nxt))
  {
    MSG_HIGH( "ACKing Bogus Segment: %lx", seg->ack, 0, 0);
    return;
  }

  /*------------------------------------------------------------------------  
    See if anything new is being acknowledged
  ------------------------------------------------------------------------*/
  if(seq_lt(seg->ack,tcb->snd.una))
  {
    MSG_MED("Old ACK Rx'd: %lx, una: %lx", seg->ack, tcb->snd.una, 0);
    return; /* Old ack, ignore */
  }

  if(seg->ack == tcb->snd.una)
  {
    if( tcb->sndcnt == 0 || length != 0 || seg->flags.syn || seg->flags.fin)
    {
      /*--------------------------------------------------------------------  
        Either we have nothing in the pipe or it carries data/syn/fin. In 
        any of these cases we wouldn't necessarily expect an ACK.
      --------------------------------------------------------------------*/

      ERR("Duplicate ACK Rx'd (Good Reason)", 0, 0, 0);
      return;
    }

    ERR("Duplicate ACK Rx'd (apparently BOGUS)" ,0,0, 0);
    return;
  }

  /*------------------------------------------------------------------------  
    We're here, so the ACK must have actually acked something
  ------------------------------------------------------------------------*/

  acked = seg->ack - tcb->snd.una;

  MSG_MED( "Received ACK: %d bytes", acked, 0, 0);

  INTLOCK();

  tcb->sndcnt  -= acked;  /* Update virtual byte count on snd queue */
  tcb->snd.una  = seg->ack;

  INTFREE();

  /*------------------------------------------------------------------------  
    If we're waiting for an ack of our SYN, note it and adjust count
  ------------------------------------------------------------------------*/
  if(!(tcb->flags.synack))
  {
    tcb->flags.synack = 1;
    acked--;  /* One less byte to pull from real snd queue */
  }

  /*------------------------------------------------------------------------  
    Remove acknowledged bytes from the send queue and update the
    unacknowledged pointer. If a FIN is being acked, pullup won't be able 
    to remove it from the queue, but that causes no harm.
  ------------------------------------------------------------------------*/

  (void)dsm_pullup(&tcb->sndq,NULL,(uint16)acked);

  /*------------------------------------------------------------------------  
    If there are outstanding segments, start the retransmission timer. 
    Otherwise, there is no data in flight and we cancel the timer.
  ------------------------------------------------------------------------*/
   
  if(tcb->snd.una != tcb->snd.nxt) 
  {
    timer_ret_val = ps_timer_start(tcb->timer, tcb->tcp_rexmit_timer_val);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
  }
  else
  {
    timer_ret_val = ps_timer_cancel( tcb->timer);
    ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
  }

  /*------------------------------------------------------------------------  
    If retransmissions have been occurring, make sure the send pointer 
    doesn't repeat ancient history
  ------------------------------------------------------------------------*/

  if(seq_lt(tcb->snd.ptr,tcb->snd.una))
  {
    tcb->snd.ptr = tcb->snd.una;
  }

  /*------------------------------------------------------------------------  
    Clear the retransmission flag since the oldest unacknowledged segment 
    (the only one that is ever retransmitted) has now been acked.
  ------------------------------------------------------------------------*/
  tcb->flags.retran = 0;

  /*------------------------------------------------------------------------  
    If outgoing data was acked, notify the user so he can send more
    unless we've already sent a FIN.
  ------------------------------------------------------------------------*/

  if( acked != 0 && tcb->t_upcall && (tcb->state == TCP_ESTABLISHED || 
      tcb->state == TCP_CLOSE_WAIT))
  {
    (*tcb->t_upcall)(tcb, acked);
  }

  MSG_MED( "exiting update_minimum()", 0, 0, 0);

  return;
} /* update_minimum() */


/*===========================================================================

FUNCTION TCP_INPUT()

DESCRIPTION
  This function performs TCP input processing. This function is called from
  IP with the IP header in machine byte order, along with a mbuf chain
  pointing to the TCP header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tcp_input
(
  struct ps_iface_s  *iface,       /* Incoming interface (ignored)         */
  struct ip          *ip,          /* IP header                            */
  struct dsm_item_s  *tcp_seg_ptr, /* Data field, if any                   */
  int                 rxbroadcast  /* Incoming broadcast - discard if true */
)
{
  struct   dsm_item_s  *reseq_ptr;       /* ptr to re-seq item             */
  register struct tcb  *tcb;             /* TCP Protocol control block     */
  struct   tcp         seg = { 0 };     /* Local copy of segment header   */
  pseudo_header_type   ph;               /* Pseudo-header for checksumming */
  int                  hdrlen;           /* Length of TCP header           */
  uint16               length;           /* length of data portion of seg. */
  uint32               ms_tick;          /* Millisecond tick holder        */
  boolean              notify_app = FALSE;
  struct connection    conn;             /* temp connection variable       */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
  struct tcp     reseq_seg;               /* TCP hdr in reseq              */
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcp_seg_ptr == NULL)
  {
    return;
  }
  tcpInSegs++;
  if(rxbroadcast)
  {
    /* Any TCP packet arriving as a broadcast is
     * to be completely IGNORED!!
     */
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }
  length      = ip->length - IPLEN - ip->optlen;
  ph.source   = ip->source;
  ph.dest     = ip->dest;
  ph.protocol = ip->protocol;
  ph.length   = length;
  if(cksum(&ph,tcp_seg_ptr,length) != 0)
  {
    /* Checksum failed, ignore segment completely */
    ntohtcp(&seg,&tcp_seg_ptr);
    MSG_HIGH( "TCP Checksum Err, Seq: %lx", seg.seq, 0, 0);
    tcpInErrs++;
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }
  if(length > dsm_length_packet( tcp_seg_ptr)) 
  /* Sanity check to match IP header length to actual payload length */
  {
    ERR("Actual length of segment doesnt match  header len",0,0,0);
     /* Senders TCP is sending bogus segments */
    tcpInErrs++;
    dsm_free_packet(&tcp_seg_ptr);
    return;
  } 
  else if ( length < dsm_length_packet(tcp_seg_ptr))
  {
    dsm_trim_packet( &tcp_seg_ptr, length);
  }

  /* Form local copy of TCP header in host byte order */
  if((hdrlen = ntohtcp(&seg,&tcp_seg_ptr)) < 0)
  {
    /* TCP header is too small */
    MSG_HIGH( "Bad TCP Header, Ignoring Segment", 0, 0, 0);
    dsm_free_packet(&tcp_seg_ptr);
    return;
  }
  length -= hdrlen;

  MSG_LOW("TCP In: seq = %x, ack = %x", seg.seq, seg.ack, 0);

  conn.local.address  = ip->dest;
  conn.local.port     = seg.dest;
  conn.remote.address = ip->source;
  conn.remote.port    = seg.source;

  /*-------------------------------------------------------------------------
    Lookup the TCP control block for this received TCP packet.
    During async/fax calls, a special tcb is used. In sockets calls, the tcb
    from dsstcp.c is used.
  -------------------------------------------------------------------------*/
#if defined(FEATURE_DATA_IS707) && !defined(FEATURE_ASYNC_DATA_NOOP) 
  if( ps707_async_in_rawdata_mode() == TRUE)
  {
    tcb = &Tcb;
  }
  else
#endif /* FEATURE_DATA_IS707 && !FEATURE_ASYNC_DATA_NOOP*/
  {
#ifdef FEATURE_DS_SOCKETS
    if(!seg.flags.syn || seg.flags.ack || seg.flags.rst) 
    {
      tcb = dsstcp_lookup_conn( &conn );
    }
    else
    {
      tcb = dsstcp_lookup_serv( &conn );
    }
#endif /* FEATURE_DS_SOCKETS */
  }

  /*-------------------------------------------------------------------------
    If a valid TCP control block could not be found for this packet, send a
    reset to the peer and return.
  -------------------------------------------------------------------------*/
  if( tcb == NULL)
  {
    dsm_free_packet(&tcp_seg_ptr);
    MSG_HIGH("Received segment for invalid connection", 0, 0, 0);
    send_reset(ip,&seg,iface);
    return;
  }

  if ( tcb->state == TCP_LISTEN ) 
  {
    /* If this segment doesn't carry a SYN, reject it */
    if(!seg.flags.syn)
    {
      MSG_MED( "Missing SYN, Listen State", 0, 0, 0);
      dsm_free_packet(&tcp_seg_ptr);
      send_reset(ip,&seg,iface);
      return;
    }
    /* Put all the socket info into the TCB */
    tcb->conn  = conn;

    /* Set the routing cache to the interface we received this on */
    /* unless we bound to a specific interface.                   */
    tcb->routing_cache = iface;
    if ( tcb->scb_ptr && (tcb->scb_ptr->routing_cache == NULL) )
    {
      tcb->scb_ptr->routing_cache = iface;
    }
  }

  tcb->flags.congest = ip->flags.congest;

  /* Do unsynchronized-state processing (p. 65-68) */

  switch(tcb->state)
  {
  case TCP_CLOSED:
    dsm_free_packet(&tcp_seg_ptr);
    send_reset(ip,&seg,iface);
    return;

  case TCP_LISTEN:
    if(seg.flags.rst)
    {
      dsm_free_packet(&tcp_seg_ptr);
      return;
    }
    if(seg.flags.ack)
    {
      dsm_free_packet(&tcp_seg_ptr);
      send_reset(ip,&seg,iface);
      return;
    }
    if(seg.flags.syn)
    {
      /* (Security check is bypassed) */
      /* page 66 */
      proc_syn(tcb,ip->tos,&seg);
      send_syn(tcb);
      setstate(tcb,TCP_SYN_RECEIVED);

      if(length != 0 || seg.flags.fin) 
      {
        /* Continue processing if there's more */
        break;
      }
      tcp_output(tcb);
    }
    dsm_free_packet(&tcp_seg_ptr);       /* Unlikely to get here directly */
    return;

  case TCP_SYN_SENT:
    if(seg.flags.ack)
    {
      if(!seq_within(seg.ack,tcb->iss+1,tcb->snd.nxt))
      {
        dsm_free_packet(&tcp_seg_ptr);
        send_reset(ip,&seg,iface);
        return;
      }
    }
    if(seg.flags.rst)
    {  /* p 67 */
      if(seg.flags.ack)
      {
        /* The ack must be acceptable since we just checked it.
         * This is how the remote side refuses connect requests.
         */
        close_self(tcb,RESET);
      }
      dsm_free_packet(&tcp_seg_ptr);
      return;
    }
    /* (Security check skipped here) */
#ifdef  PREC_CHECK  /* Turned off for compatibility with BSD */
    /* Check incoming precedence; it must match if there's an ACK */
    if(seg.flags.ack && PREC(ip->tos) != PREC(tcb->tos))
    {
      dsm_free_packet(&tcp_seg_ptr);
      send_reset(ip,&seg,iface);
      return;
    }
#endif
    if(seg.flags.syn)
    {
      proc_syn(tcb,ip->tos,&seg);
      if(seg.flags.ack)
      {
        /* Our SYN has been acked, otherwise the ACK
         * wouldn't have been valid.
         */
        update(tcb,&seg,length);
        setstate(tcb,TCP_ESTABLISHED);
      } else 
      {
        setstate(tcb,TCP_SYN_RECEIVED);
      }

      if(length != 0 || seg.flags.fin) 
      {
        break;    /* Continue processing if there's more */
      }
      tcp_output(tcb);
    } else 
    {
      dsm_free_packet(&tcp_seg_ptr);  /* Ignore if neither SYN or RST is set */
    }
    dsm_free_packet(&tcp_seg_ptr); 
    return;
  }
  /* We reach this point directly in any synchronized state. Note that
   * if we fell through from LISTEN or SYN_SENT processing because of a
   * data-bearing SYN, window trimming and sequence testing "cannot fail".
   */

  /* Trim segment to fit receive window. */
  if( trim( tcb, &seg, &tcp_seg_ptr, &length) == -1)
  {
    /* Segment is unacceptable */
    MSG_HIGH( "Toss Seg, seq: %lx, nxt: %lx", seg.seq, tcb->rcv.nxt, 0);
    if(!seg.flags.rst)
    {  /* NEVER answer RSTs */
      /* In SYN_RECEIVED state, answer a retransmitted SYN
       * with a retransmitted SYN/ACK.
       */
      if(tcb->state == TCP_SYN_RECEIVED)
      {
        tcb->snd.ptr = tcb->snd.una;
      }
      tcb->flags.force = 1;
      tcp_output(tcb);
    }
    return;
  }
  /* If segment isn't the next one expected, and there's data
   * or flags associated with it, put it on the resequencing
   * queue, ACK it and return.
   *
   * Processing the ACK in an out-of-sequence segment without
   * flags or data should be safe, however.
   */
  if( seg.seq != tcb->rcv.nxt && 
    ( length != 0 || seg.flags.syn || seg.flags.fin))
  {
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
    /*-----------------------------------------------------------------------
      If we are running out of items, then stop queueing new TCP segments to
      the resequencing queue.
    -----------------------------------------------------------------------*/
    if (DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) ||
        DSM_LESS_THAN_TRANSPORT_LAYER_FEW_FREE_ITEMS(DSM_DS_LARGE_ITEM_POOL))
    {
      MSG_HIGH("out of memory: dumping out-of-seq seg", 0, 0, 0);
      /*------------------------------------------------------------------
        If this out-of-seq segment does not contain SYN/FIN flags, we want
        to process the ack before throwing away the segment. The ack 
        processing may free up some memory from the sndq.
      ------------------------------------------------------------------*/
      update_minimum(tcb,&seg,length);
      dsm_free_packet( &tcp_seg_ptr); 
    }
    else
    {
      add_reseq(tcb,ip->tos,&seg,tcp_seg_ptr,length);
    }
    tcb->flags.force = 1;
    tcp_output(tcb);
    return;
  }
  /* This loop first processes the current segment, and then
   * repeats if it can process the resequencing queue.
   */
  for(;;)
  {
    /* We reach this point with an acceptable segment; all data and flags
     * are in the window, and the starting sequence number equals rcv.nxt
     * (p. 70)
     */
    if(seg.flags.rst)
    {
      if( tcb->state == TCP_SYN_RECEIVED && 
          !tcb->flags.clone && !tcb->flags.active)
      {
        /* Go back to listen state only if this was
         * not a cloned or active server TCB
         */
        setstate(tcb,TCP_LISTEN);
      } 
      else if (tcb->state == TCP_CLOSE_WAIT ||
               tcb->state == TCP_LAST_ACK) 
      {
        close_self(tcb,NORMAL);
      }
      else 
      {
        close_self(tcb,RESET);
      }
      dsm_free_packet(&tcp_seg_ptr);
      return;
    }
    /* (Security check skipped here) p. 71 */
#ifdef  PREC_CHECK
    /* Check for precedence mismatch */
    if(PREC(ip->tos) != PREC(tcb->tos))
    {
      dsm_free_packet(&tcp_seg_ptr);
      send_reset(ip,&seg,iface);
      return;
    }
#endif
    /* Check for erroneous extra SYN */
    if(seg.flags.syn)
    {
      dsm_free_packet(&tcp_seg_ptr);
      send_reset(ip,&seg,iface);
      return;
    }
    /* Check ack field p. 72 */
    if(!seg.flags.ack)
    {
      /* All segments after synchronization must have ACK */
      MSG_HIGH( "Missing ACK, Discard Seg: %lx", seg.seq, 0, 0);
      dsm_free_packet(&tcp_seg_ptr);
      return;
    }
    /* Process ACK */
    switch(tcb->state)
    {
    case TCP_SYN_RECEIVED:
      if(seq_within(seg.ack,tcb->snd.una+1,tcb->snd.nxt))
      {
        update(tcb,&seg,length);
        setstate(tcb,TCP_ESTABLISHED);
      } 
      else 
      {
        dsm_free_packet(&tcp_seg_ptr);
        send_reset(ip,&seg,iface);
        return;
      }
      break;

    case TCP_ESTABLISHED:
    case TCP_CLOSE_WAIT:
      update(tcb,&seg,length);
      break;

    case TCP_FINWAIT1:  /* p. 73 */
      update(tcb,&seg,length);
      if(tcb->sndcnt == 0)
      {
        /* Our FIN is acknowledged */
        setstate(tcb,TCP_FINWAIT2);
      }
      break;

    case TCP_FINWAIT2:
      update(tcb,&seg,length);
      break;

    case TCP_CLOSING:
      update(tcb,&seg,length);
      if(tcb->sndcnt == 0)
      {
        /* Our FIN is acknowledged */
        setstate(tcb,TCP_TIME_WAIT);
        tcb->tcp_rexmit_timer_val = MSL2*1000L;
        timer_ret_val = ps_timer_start( tcb->timer, 
                                        tcb->tcp_rexmit_timer_val
                                      );
        ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
      }
      break;

    case TCP_LAST_ACK:
      update(tcb,&seg,length);
      if(tcb->sndcnt == 0)
      {
        /* Our FIN is acknowledged, close connection */
        close_self(tcb,NORMAL);
        MSG_HIGH("LAST_ACK ack recvd - Exiting", 0, 0, 0);
        dsm_free_packet(&tcp_seg_ptr);
        return;
      }
      break;

    case TCP_TIME_WAIT:
      timer_ret_val = ps_timer_start( tcb->timer, 
                                      tcb->tcp_rexmit_timer_val
                                    );
      ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
      break;
    } /* switch( state) */

    /* (URGent bit processing skipped here) */

    /* Process the segment text, if any, beginning at rcv.nxt (p. 74) */
    if(length != 0)
    {
      switch(tcb->state)
      {
      case TCP_SYN_RECEIVED:
      case TCP_ESTABLISHED:
      case TCP_FINWAIT1:
      case TCP_FINWAIT2:
        /* Place on receive queue */
        ms_tick = msclock();
        if(ms_tick > tcb->lastrx)
        {
          tcb->rxbw   = 1000L*length / (ms_tick - tcb->lastrx);
          tcb->lastrx = ms_tick;
        }
#ifdef FEATURE_DSM_MEM_CHK
        /*---------------------------------------------------------
         Set tracer to indicate where the item is being stored
        ---------------------------------------------------------*/
        tcp_seg_ptr->tracer = DSM_TCP_RCVQ;
#endif

        INTLOCK();
        dsm_append( &tcb->rcvq, &tcp_seg_ptr);
        tcb->rcvcnt  += length;
        tcb->rcv.nxt += length;
        tcb->rcv.wnd -= length;
        INTFREE();

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

#ifdef FEATURE_DSM_MEM_CHK
  if( tcb->rcvcnt > tcb->rcvcnt_himark)
  {
    tcb->rcvcnt_himark = tcb->rcvcnt;
  }
#endif
        MSG_LOW( "Data in, Seq: %lx, rcvcnt: %lx", seg.seq, tcb->rcvcnt, 0);
        MSG_LOW( "rcv.nxt: %lx, rcv.wnd: %lx", tcb->rcv.nxt, tcb->rcv.wnd, 0);

        /* Decide whether to delay Ack.
         * If previous Ack has been delayed,
         * Ack now (cuts number of Acks in half).
         * Otherwise, wait for half send RTT,
         * which shouldn't hurt deviation too badly.
         * Hopefully, the user upcall will result
         * in more data to send.
         */
#ifdef FEATURE_DS_TCP_NO_DELAY
        tcb->flags.force = TRUE;
#else
        if ( ps_timer_is_running(tcb->acker) )
        {
          MSG_MED( "TCP Delayed Ack Timer Running", 0, 0, 0);
          tcb->flags.force = TRUE;
        }
        else
        {
          uint32 ato = tcb->srtt / 2 + tcb->mdev;
          ato        = MAX( ato, MIN_ATO );
          ato        = MIN( ato, MAX_ATO );
          MSG_LOW("Starting delayed ack timer", 0, 0, 0);
          timer_ret_val = ps_timer_start( tcb->acker,
                                          ato
                                        );
          ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
        }
#endif /* FEATURE_DS_TCP_NO_DELAY */

        /* Notify user */
        if(tcb->r_upcall)
        {
          notify_app = TRUE;
        }
        break;

      default:
        /* Ignore segment text */
        dsm_free_packet(&tcp_seg_ptr);
      } /* switch( state) */
    } /* if( length != 0) */

    /* process FIN bit (p 75) */
    if(seg.flags.fin)
    {
      tcb->flags.force = 1;  /* Always respond with an ACK */

      switch(tcb->state)
      {
      case TCP_SYN_RECEIVED:
      case TCP_ESTABLISHED:
        tcb->rcv.nxt++;
        setstate(tcb,TCP_CLOSE_WAIT);
        break;

      case TCP_FINWAIT1:
        tcb->rcv.nxt++;
        if(tcb->sndcnt == 0)
        {
          /* Our FIN has been acked; bypass TCP_CLOSING state */
          setstate(tcb,TCP_TIME_WAIT);
          tcb->tcp_rexmit_timer_val = MSL2*1000L;
          timer_ret_val = ps_timer_start( tcb->timer, 
                                          tcb->tcp_rexmit_timer_val
                                        );
          ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
        } 
        else 
        {
          setstate(tcb,TCP_CLOSING);
        }
        break;

      case TCP_FINWAIT2:
        tcb->rcv.nxt++;
        setstate(tcb,TCP_TIME_WAIT);
        tcb->tcp_rexmit_timer_val = MSL2*1000L;
        timer_ret_val = ps_timer_start( tcb->timer, 
                                        tcb->tcp_rexmit_timer_val
                                      );
        ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
        break;

      case TCP_CLOSE_WAIT:
      case TCP_CLOSING:
      case TCP_LAST_ACK:
        break;    /* Ignore */

      case TCP_TIME_WAIT:  /* p 76 */
        timer_ret_val = ps_timer_start( tcb->timer, 
                                        tcb->tcp_rexmit_timer_val
                                      );
        ASSERT( timer_ret_val == PS_TIMER_SUCCESS);
        break;
      } /* switch( state) */

      /* Call the client again so he can see EOF */
      if(tcb->r_upcall)                                 
      {
        (*tcb->r_upcall)(tcb,tcb->rcvcnt);
      }
    } /* if(..fin) */

    /* Scan the resequencing queue, looking for a segment we can handle,
     * and freeing all those that are now obsolete.
     */
    while(( reseq_ptr = (dsm_item_type *)q_check( &tcb->reseq)) != NULL)
    {
      /*---------------------------------------------------------------------
        Extract the TCP segment header into the reseq_seg structure.
        This structure is then accessed to obtain the sequence number of 
        the stored packets.
        check if the sequence number of received segment is less than the
        one in reseq-q. If YES, pass it on to higher layer. If no, it means
        we need to get the segment from reseq-q.
      ---------------------------------------------------------------------*/
      (void)dsm_extract(reseq_ptr, 0, &reseq_seg, sizeof(struct tcp));

      if (seq_lt(tcb->rcv.nxt, reseq_seg.seq))
      {
         break;
      }
      get_reseq( tcb, &ip->tos, &seg, &tcp_seg_ptr, &length);
      MSG_HIGH( "Getting Seg from Re-Seq Q: %lx", seg.seq, 0, 0);
      if( trim( tcb, &seg, &tcp_seg_ptr, &length) == 0)
      {
        goto barff;  
        /* Segment needs processing, barff to process segment */
      }
    }
    break;
barff: ;
  } /* for(;;) */

  dsm_free_packet(&tcp_seg_ptr);

#if defined(FEATURE_DATA_IS707) && !defined(FEATURE_ASYNC_DATA_NOOP) 
  if( ps707_async_in_rawdata_mode() == TRUE)
  {
    ps707_async_process_tcp_send_q( tcb, TRUE);  /* Send ACK and any new data */
  }
  else
#endif /* FEATURE_DATA_IS707 && !FEATURE_ASYNC_DATA_NOOP*/
  {
    tcp_output( tcb);
  }

  if ( TRUE == notify_app)
  {
     (*tcb->r_upcall)(tcb,tcb->rcvcnt);
     notify_app = FALSE;
  }
} /* tcp_input() */

#endif /* FEATURE_DS || FEATURE_DATA_PS */
