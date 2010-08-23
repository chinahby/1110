/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            T C P S H E L L

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpshell.c_v   1.1   30 Aug 2002 20:59:26   omichael  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_tcpshell.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/04   vp      Removed a byte ordering bug for assignment of port numbers 
                   and corrected the assignment of sequence number in RESET 
                   segment in reset_tcp().
05/19/04   vp      Assigned tcb_ptr->cwind according to passed mss in open_tcp
10/22/03   sv      Modified open_tcp to take MSS as an additional argument.
08/30/02   om      Updates for using routing cache when sending TCP resets.
07/31/02   usb     Added include ps_iface.h
07/31/02   usb     Removed include ps.h
03/05/02   pjb     Finished Accept Listen Code Review Items 
02/22/02   pjb     Added accept and listen
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module. Wrap out
                   ps_initiate_upper_layer() if FEATURE_DATA_PS is defined.
10/30/00   snn     Replaced Tcb_ptr with tcb_ptr to follow naming convention.
10/10/00   na/snn  Removed the sndq_tail_ptr and put it in the TCB. This was
                   done to support multiple TCP sockets.
08/25/00   rc      Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
09/14/98   na      Made changes for improved bidirectional performance.
05/05/98   na      Fixed a pointer problem in append_to_sendq.
04/14/98   ldg     Added counter for payload sent.
04/14/98   na      Changed append_to_sendq to save memory items by copying 
                   small ones.
01/27/98   ldg     Removed the DS_SLIM switch.
01/19/98   na      Changes in append to sendq to facilitate escaping at task
                   level. Also, removed ISS2 target dependent code that needed
                   smaller memory items.
06/17/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
06/12/97   ldg     Reset stats in open_tcp for new FEATURE_DS_PSTATS.
01/23/97   na      Optimized append to sndq for better throughput. Uses small
                   items on the TX stream instead of large ones. Older code
                   under #if T_I2 due to ISS2 memory limitations.
01/20/97   na      Fixed bug for proper port assignment when TCP is opened
                   passively in a LtoM call
11/14/96   jjw     Upgraded (sped up) TCP open scheme
10/17/96   jjw     Minor change to initiate TCP Open after PPP (IPCP) Opens
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* User calls to TCP
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992 Glenn Hamilton McGregor
 * Additional material Copyright 1992 William Allen Simpson
 */


#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined(FEATURE_DATA_PS)
#include "psglobal.h"
#include "dsm.h"
#include "netuser.h"
#include "internet.h"
#include "ps_iface.h"
#include "ps_tcp.h"
#include "ps_ip.h"
#include "memory.h"
#include "msg.h"
#include "err.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

uint16 Tcp_mss    = DEF_MSS;
uint16 Tcp_window = DEF_WND;


/*===========================================================================

FUNCTION OPEN_TCP

DESCRIPTION
  This function will Open a TCP connection using the specified parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

struct tcb *open_tcp
(
  struct socket *lsocket,  /* Local socket */
  struct socket *fsocket,  /* Remote socket */
  int           mode,      /* Active/passive/server */
  uint16        window,    /* Receive window (and send buffer) sizes */
  uint16        mss,       /* TCP mss  */
  int           tos,       /* Type of service   */
  int           user,      /* User linkage area */
  struct tcb    *tcb_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

  if(lsocket == NULL)
  {
    return NULL;
  }
  tcb_ptr->conn.local.address = lsocket->address;
  tcb_ptr->conn.local.port    = lsocket->port;
  if(fsocket != NULL)
  {
    tcb_ptr->conn.remote.address = fsocket->address;
    tcb_ptr->conn.remote.port    = fsocket->port;
  }
  else
  {
    tcb_ptr->conn.remote.address = 0;
    tcb_ptr->conn.remote.port    = 0;
  }

  tcb_ptr->user = user;
  if( window != 0)
  {
    tcb_ptr->window = window;
  }
  else if( Tcp_window >= Tcp_mss )
  {
    tcb_ptr->window = Tcp_window;
  }
  else
  {
    tcb_ptr->window = Tcp_window * Tcp_mss;
  }

  tcb_ptr->rcv.wnd = tcb_ptr->window;
  tcb_ptr->snd.wnd = 1;      /* Allow space for sending a SYN */
  tcb_ptr->tos     = tos;
  tcb_ptr->mss     = mss;
  tcb_ptr->cwind   = mss;

  switch( mode)
  {
    case TCP_SERVER:
      tcb_ptr->flags.clone = 1;

    case TCP_PASSIVE:      /* Note fall-thru */
      setstate( tcb_ptr, TCP_LISTEN);
      break;

    case TCP_ACTIVE:
      /* Send SYN, go into TCP_SYN_SENT state */
      tcb_ptr->flags.active = 1;
      send_syn( tcb_ptr);
      setstate( tcb_ptr, TCP_SYN_SENT);
      tcp_output( tcb_ptr);
      break;
   default:   /* Error Trap,  Path-whatever-logical */
    ERR_FATAL( "Invalid TCP Open", 0, 0, 0);
  }
  return( tcb_ptr);
}


/*===========================================================================

FUNCTION APPEND_TO_TCP

DESCRIPTION
  This function will append the passed data item to the TCP send Q.

  To optimize for CPU, the TCP sndq is traversed manually instead of using
  dsm routines like dsm_append() and dsm_length_packet().
                                                                        
DEPENDENCIES
  sndq_tail_ptr is only updated in this function. So, no item from the 
  TCP sendq should be removed from end of the sndq.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL word append_to_sndq
(
  register struct tcb  *tcb,            /* ptr to TCP Control Block        */
  dsm_item_type        *new_item_ptr    /* ptr to data item for TCP output */
)
{
  word          cnt=0;               /* holds count of new data bytes      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    Appending to end of tcb_sndq - using small item pointers as against
     large items previously; improves throughput
   ----------------------------------------------------------------------*/
  if ( tcb->sndq == NULL)
  {
    tcb->sndq               = new_item_ptr;
  }
  else
  {
    (tcb->sndq_tail_ptr)->pkt_ptr  = new_item_ptr;
  }

  while ( new_item_ptr->pkt_ptr != NULL)
  {
    cnt += new_item_ptr->used;
    new_item_ptr = new_item_ptr->pkt_ptr;
  }
  cnt += new_item_ptr->used;
  tcb->sndq_tail_ptr = new_item_ptr;

  /*-------------------------------------------------------------------------
    Now update the TCP send Q count, free the passed in data item, and return
    the number of bytes loaded.
  -------------------------------------------------------------------------*/
  tcb->sndcnt += cnt;

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

#ifdef FEATURE_DSM_MEM_CHK
  if( tcb->sndcnt > tcb->sndcnt_himark)
  {
    tcb->sndcnt_himark = tcb->sndcnt;
  }
#endif

  return( cnt);
} /* append_to_sndq */


/*===========================================================================

FUNCTION SEND_TCP

DESCRIPTION
  This function will queue the passed data for TXing on the passed TCP
  connection.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

sint15 send_tcp
(
  register struct tcb  *tcb,            /* ptr to TCP Control Block        */
  dsm_item_type        *new_item_ptr   /* ptr to data item for TCP output */
)
{
  word          cnt;                 /* holds count of new data bytes      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_DSM_MEM_CHK
  /*---------------------------------------------------------
   Set tracer to indicate where the item is being stored
  ---------------------------------------------------------*/
  new_item_ptr->tracer = DSM_TCP_SNDQ;
#endif
  if(tcb == NULL || new_item_ptr == NULL)
  {
    dsm_free_packet( &new_item_ptr);
    return -1;
  }
  cnt = 0;
  switch(tcb->state)
  {
    case TCP_CLOSED:
      dsm_free_packet( &new_item_ptr);
      return -1;

    case TCP_LISTEN:
      /* --------------------------------------------------------------
        If any remote address is 0, then it is a passive connection (LtoM).
        We stay in LISTEN state till we receive TCP packets from the IWF.
        Earlier the remote address was set to 0 in psmgr.c, so we enter
        the if when we are in passive mode and don't have dest. port #
        yet
      ----------------------------------------------------------------*/

      if(tcb->conn.remote.address == 0 || tcb->conn.remote.port == 0)
      {
        /* Save data for later */
        cnt = append_to_sndq( tcb, new_item_ptr);
        break;
      }
      /* Change state from passive to active */
      tcb->flags.active = 1;
      send_syn(tcb);
      setstate(tcb,TCP_SYN_SENT);      /* Note fall-thru */

    case TCP_SYN_SENT:
    case TCP_SYN_RECEIVED:
    case TCP_ESTABLISHED:
    case TCP_CLOSE_WAIT:
      cnt = append_to_sndq( tcb, new_item_ptr);
      tcp_output(tcb);
      break;

    case TCP_FINWAIT1:
    case TCP_FINWAIT2:
    case TCP_CLOSING:
    case TCP_LAST_ACK:
    case TCP_TIME_WAIT:
      dsm_free_packet( &new_item_ptr);
      return -1;
  }
  return cnt;
}

/*===========================================================================

FUNCTION RESET_TCP

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void reset_tcp
(
  register struct tcb *tcb
)
{
  struct tcp fakeseg;
  struct ip fakeip;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcb == NULL)
  {
    return;
  }
  if(tcb->state != TCP_LISTEN)
  {
    /* Compose a fake segment with just enough info to generate the
     * correct RST reply
     */
    memset(&fakeseg,0,sizeof(fakeseg));
    memset(&fakeip,0,sizeof(fakeip));
    fakeseg.dest = ntohs(tcb->conn.local.port);
    fakeseg.source = ntohs(tcb->conn.remote.port);
    fakeseg.flags.ack = 1;
    /* Here we try to pick a sequence number with the greatest likelihood
     * of being in his receive window.
     */
    fakeseg.ack = tcb->snd.nxt;
    fakeip.dest = tcb->conn.local.address;
    fakeip.source = tcb->conn.remote.address;
    fakeip.tos = tcb->tos;
    send_reset(&fakeip,&fakeseg,tcb->scb_ptr->routing_cache);
  }
  close_self(tcb,RESET);
}

/*===========================================================================

FUNCTION CLOSE_TCP

DESCRIPTION
  This function will close the TCP connection for the passed TCB. This really
  means "I have no more data to send". It only closes the  connection in one
  direction, and we can continue to receive data indefinitely.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

int close_tcp
(
  register struct tcb *tcb
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

      if(tcb == NULL)
      {
            return -1;
      }
      switch(tcb->state)
      {
      case TCP_CLOSED:
            return 0;      /* Unlikely */
      case TCP_LISTEN:
      case TCP_SYN_SENT:
            close_self(tcb,NORMAL);
            return 0;
      case TCP_SYN_RECEIVED:
      case TCP_ESTABLISHED:
            tcb->sndcnt++;
            tcb->snd.nxt++;
            setstate(tcb,TCP_FINWAIT1);
            tcp_output(tcb);
            return 0;
      case TCP_CLOSE_WAIT:
            tcb->sndcnt++;
            tcb->snd.nxt++;
            setstate(tcb,TCP_LAST_ACK);
            tcp_output(tcb);
            return 0;
      case TCP_FINWAIT1:
      case TCP_FINWAIT2:
      case TCP_CLOSING:
      case TCP_LAST_ACK:
      case TCP_TIME_WAIT:
            return -1;
      }
      return -1;      /* "Can't happen" */
}
#endif /* FEATURE_DS || FEATURE_DATA_PS */
