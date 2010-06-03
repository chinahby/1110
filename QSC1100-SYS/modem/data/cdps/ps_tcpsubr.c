/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             T C P S U B R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpsubr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_tcpsubr.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/03   sv      Modified tx_upcall function to pass number of bytes acked
                   instead of available rcv window size.
07/31/02   usb     Removed include psi.h
03/05/02    pjb    Finished Accept Listen Code Review Items 
02/22/02    pjb    Added accept and listen
02/12/02   dwp     Cleanup header inclusion(s) for non DS builds.
12/06/01   ss      Corrected the wrong order of memset() parameters
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module. Wrap out old DS
                   dependencies if FEATURE_DATA is defined.
11/11/00   snn     Removed errno from the timer functions because they are
                   no longer used.
                   Renamed tcp_state to tcp_state_enum_type
10/10/00   na      Added support for multiple TCP sockets.
10/27/98   ldg     ARM porting: ROM to ROM const.
04/14/98   ldg     Added counters for payload sent/received, and backoffs.
                   Removed rcvcnt and sndcnt from stats dump.
01/26/98   ldg     Removed the DS_SLIM switch.
12/15/97   ldg     Moved TCP out-of-order segment count from UI stats to
                   general stats, and added to legend for AT stats dump.
                   Also, out-of-order segment count was counting TCP active
                   opens rather than out-of-order segments.  Fixed this.
11/06/97   ldg     Removed externs.
09/26/97   ldg     Removed writes to Tcb from tcp_reset_stats.
08/06/97   na      Fixed bug caused by missing parens. around ERR_FATAL.
07/31/97   ldg     Added accessor function for UI to see some TCP vars.
06/25/97   jgr     Added ifdef FEATURE_DS over whole file
06/12/97   ldg     Added new FEATURE_DS_PSTATS
01/23/97   na      Included support for UI function that returns last status
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Low level TCP routines:
 *  control block management
 *  sequence number logical operations
 *  state transitions
 *  RTT cacheing
 *  garbage collection
 *
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992-1995 William Allen Simpson
 */
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)
#include <memory.h>
#include "psglobal.h"
#ifndef FEATURE_DATA
#include "dsi.h"
#endif
#include "dsm.h"
#include "netuser.h"
#include "internet.h"
#include "ps_tcp.h"
#include "msg.h"
#include "err.h"
#include "pstimer.h"
#include "assert.h"




/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/


struct tcb Tcb;

struct tcp_rtt Tcp_rtt;

struct mib_entry Tcp_mib[] = 
{
 /* NULL */                 0,
 /* tcpRtoAlgorithm */      4,              /* Van Jacobsen's algorithm */
 /* tcpRtoMin */            0,              /* No lower bound */
 /* tcpRtoMax */            MAXINT32,       /* No upper bound */
 /* tcpMaxConn */           -1L,            /* No limit */
 /* tcpActiveOpens */       0,
 /* tcpPassiveOpens */      0,
 /* tcpAttemptFails */      0,
 /* tcpEstabResets */       0,
 /* tcpCurrEstab */         0,
 /* tcpInSegs */            0,
 /* tcpOutSegs */           0,
 /* tcpRetransSegs */       0,
 /* NULL */                 0,              /* Connection state goes here */
 /* tcpInErrs */            0,
 /* tcpOutRsts */           0,
};

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */


/*===========================================================================

FUNCTION CREATE_TCB

DESCRIPTION
  This function will Create a TCB, return pointer. Return pointer if TCB
  already exists.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to tcb.

SIDE EFFECTS
  None
===========================================================================*/

struct tcb *create_tcb
( 
  struct connection *conn 
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( q_cnt( &Tcb.reseq ) != 0 )
  { 
    ERR_FATAL( "Ahhhaaahh", 0, 0, 0 );
  }
  memset( &Tcb, 0, sizeof(struct tcb) );

  q_init(&Tcb.reseq);

  Tcb.state    = TCP_CLOSED;
  Tcb.cwind    = DEF_MSS;
  Tcb.mss      = DEF_MSS;
  Tcb.ssthresh = SSTHRESH;
  Tcb.srtt     = DEF_RTT;  /* mdev = 0 */
  Tcb.minrto   = MIN_RTO;
  Tcb.maxrto   = MAX_RTO;
  return( &Tcb);
}

/*===========================================================================

FUNCTION CLOSE_SELF

DESCRIPTION
  This function will close the passed tcb.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void close_self
( 
  register struct tcb *tcb, 
  int                 reason
)
{
  struct dsm_item_s *rp;
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(tcb == NULL)
  {
    return;
  }

  timer_ret_val = ps_timer_cancel( tcb->timer);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

  timer_ret_val = ps_timer_cancel( tcb->acker);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

  
  tcb->reason = reason;

  /* Dump reassembly queue; nothing more can arrive */

  while( (rp = (struct dsm_item_s *)q_get(&tcb->reseq)) != NULL)
  {
    dsm_free_packet(&rp);
  }
  setstate(tcb,TCP_CLOSED);
}

/*===========================================================================

FUNCTION SEQ_WITHIN

DESCRIPTION
  This function will perform Sequence number comparisons.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if x is between low and high inclusive, otherwise FALSE.

SIDE EFFECTS
  None
===========================================================================*/

int seq_within
(
  register uint32 x,
  register uint32 low,
  register uint32 high 
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(low <= high)
  {
    if(low <= x && x <= high)
   {
      return 1;
    }
  } 
  else 
  {
    if(low >= x && x >= high)
    {
      return 1;
    }
  }
  return 0;
}

#ifdef  notinline

/*===========================================================================

FUNCTION SEQ_LT

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

int seq_lt
(
  register uint32 x,
  register uint32 y
)
{
  return (long)(x-y) < 0;
}

/*===========================================================================

FUNCTION SEQ_GT

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

int seq_gt
(
  register uint32 x,
  register uint32 y
)
{
  return (long)(x-y) > 0;
}

/*===========================================================================

FUNCTION SEQ_GE

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

int seq_ge
(
  register uint32 x,
  register uint32 y 
)
{
  return (long)(x-y) >= 0;
}
#endif

/*===========================================================================

FUNCTION SETSTATE

DESCRIPTION
  This function will set the TCP state to the passed state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void setstate
(
  register struct      tcb *tcb,
  tcp_state_enum_type  newstate 
)
{
  tcp_state_enum_type oldstate = tcb->state;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*-------------------------------------------------------------------------
  Setting state for the UI last call status function
 ------------------------------------------------------------------------*/
#ifndef FEATURE_DATA
  if ( newstate == TCP_ESTABLISHED)
  {
     ds_last_call_var     = DS_LAST_ESTABLISHED;
  }
#endif

  tcb->state = newstate;

  /* Update MIB variables */

  switch( oldstate)
  {
  case TCP_CLOSED:
    if( newstate == TCP_SYN_SENT)
    {
      tcpActiveOpens++;
    }
    break;

  case TCP_LISTEN:
    if( newstate == TCP_SYN_RECEIVED)
    {
      tcpPassiveOpens++;
    }
    break;

  case TCP_SYN_SENT:
    if( newstate == TCP_CLOSED)
    {
      tcpAttemptFails++;
    }
    break;

  case TCP_SYN_RECEIVED:
    if( newstate == TCP_CLOSED || newstate == TCP_LISTEN)
    {
      tcpAttemptFails++;
      break;
    }
    break;

  case TCP_ESTABLISHED:
  case TCP_CLOSE_WAIT:
    if( newstate == TCP_CLOSED || newstate == TCP_LISTEN)
    {
      tcpEstabResets++;
      break;
    }
    tcpCurrEstab--;
    break;
  }
  if(newstate == TCP_ESTABLISHED || newstate == TCP_CLOSE_WAIT)
  {
    tcpCurrEstab++;
  }

  if(tcb->s_upcall)
  {
    (*tcb->s_upcall)(tcb,oldstate,newstate);
  }

  switch( newstate)
  {
  case TCP_SYN_RECEIVED:  /***/
  case TCP_ESTABLISHED:
    /* Notify the user that he can begin sending data */
    if(tcb->t_upcall)
    {
      (*tcb->t_upcall)(tcb,0);
    }
    break;
  }
}

/*===========================================================================

FUNCTION 

DESCRIPTION
  This function performs Round trip timing cache routines.
  These functions implement a very simple system for keeping track of
  network performance for future use in new connections.
  The emphasis here is on speed of update (rather than optimum cache hit
  ratio) since rtt_add is called every time a TCP connection updates
  its round trip estimate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rtt_add
(
  ip4a   addr,    /* Destination IP address */
  uint32 rtt 
)
{
  register struct tcp_rtt *tp;
  uint32 abserr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(addr == 0)
  {
    return;
  }
  tp = &Tcp_rtt;
  if(tp->addr != addr)
  {
    /* New entry */
    tp->addr = addr;
    tp->srtt = rtt;
    tp->mdev = 0;
  }
  else 
  {
    /* Run our own SRTT and MDEV integrators, with rounding */

    abserr   = (rtt > tp->srtt) ? rtt - tp->srtt : tp->srtt - rtt;
    tp->srtt = ((AGAIN-1)*tp->srtt + rtt + (AGAIN/2)) >> LAGAIN;
    tp->mdev = ((DGAIN-1)*tp->mdev + abserr + (DGAIN/2)) >> LDGAIN;
  }
}

/*===========================================================================

FUNCTION RTT_GET

DESCRIPTION
  This function will

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

struct tcp_rtt *rtt_get
( 
  ip4a addr 
)
{
  register struct tcp_rtt *tp;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(addr == 0)
  {
    return NULL;
  }
  tp = &Tcp_rtt;
  if(tp->addr != addr)
  {
    return NULL;
  }
  return tp;
}

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

#endif /* FEATURE_DS || FEATURE_DATA_PS */
