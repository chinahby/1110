/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            T C P T I M E R 

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

tcp_timeout_cb()
  Gets called when TCP rexmit timer goes off.

tcp_delayed_cb()
  Gets called when TCP delayed ack timer goes off.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcptimer.c_v   1.0   08 Aug 2002 11:19:54   akhare  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_tcptimer.c#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/01   usb     Removed include psi.h
11/12/01   dwp     Add "|| FEATURE_DATA_PS" to whole module.
11/11/00   snn     Renamed the callback functions with _cb postfix.
                   Removed errno from the timer functions because they are no
                    longer used.
10/10/00   na/snn  Added support for multiple TCP sockets.
08/17/99   smp     Added include files msg.h & err.h.
06/17/98   ldg     Update backoff history at time of backoff.
01/27/98   ldg     Removed the DS_SLIM switch.
09/24/97   na      Changed TCP backoff from exponential to linear. Put a limit
                   of 6 attempts at backoff.
06/25/97   jgr     Added ifdef FEATURE_DS over whole file
05/24/96   jjw     Gemini & ISS2 Data Services release.
11/13/95   jjw     Changed min/max macros to upper case
11/09/95   jjw     IS-99 Compliant Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* TCP timeout routines
 * Copyright 1991 Phil Karn, KA9Q
 * Additional material Copyright 1992-1995 William Allen Simpson
 */
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#if defined (FEATURE_DS) || defined (FEATURE_DATA_PS)
#include "psglobal.h"
#include "dsm.h"
#include "netuser.h"
#include "internet.h"
#include "ps_tcp.h"
#include "msg.h"
#include "err.h"
#include "pstimer.h"
#include "assert.h"


/*===========================================================================

                       EXTERNAL FUNCTION DEFINITIONS
                       
===========================================================================*/

/*===========================================================================

FUNCTION TCP_TIMEOUT_CB

DESCRIPTION
  This function will perform Re-transmission processing for TCP. This function
  is invoked upon expiration of the TCP Re-xmit timer. This timer expires
  when a TCP segment that was TX'd has not been ACKnowledged.
  
  This function is called in the context of the PS task.
  
  The passed parameter in_tcb_ptr is the pointer to the TCP control block
  for the TCP connection whose close timer went off. This should not be NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tcp_timeout_cb
( 
  void  *in_tcb_ptr                        /* pointer to TCP control block */
)
{
  struct tcb *tcb_ptr = NULL;                                /* Ptr to tcb */
  ps_timer_error_type timer_ret_val;  /* Return value from timer fn. calls */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check to make sure the parameters to this function are valid.
    If ASSERTs are turned on, this condition should be caught. 
    If ASSERTs are turned off, we exit from this function so that invalid 
    pointers will not be referenced or modified.
  -------------------------------------------------------------------------*/
  if( in_tcb_ptr == NULL)
  {
    ASSERT( 0);
    return;
  }

  tcb_ptr = (struct tcb *) in_tcb_ptr;

  /*-------------------------------------------------------------------------
    Cancel timer.
  -------------------------------------------------------------------------*/
  timer_ret_val = ps_timer_cancel( tcb_ptr->timer);
  ASSERT( timer_ret_val == PS_TIMER_SUCCESS);

  /*-------------------------------------------------------------------------
    If the TCP connection is in TIME_WAIT state, then close the TCP 
    connection - 2MSL timer has expired. 
    Otherwise setup for retransmission - retransmission timer has expired.
  -------------------------------------------------------------------------*/
  switch(tcb_ptr->state)
  {
    case TCP_TIME_WAIT:                
      close_self(tcb_ptr,NORMAL);
      break;

    default:                         
      MSG_MED( "TCP Re-xmit", 0, 0, 0);
      tcb_ptr->timeouts++;
      tcb_ptr->backoff++;

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif

      if ( tcb_ptr->backoff < TCP_MAX_BACKOFFS)
        /* Check if we have exceeded the max allowable backoff tries   */
      {
        tcb_ptr->flags.retran = 1;           /* Indicate > 1  transmission */
        tcb_ptr->snd.ptr      = tcb_ptr->snd.una;

        /* Reduce slowstart threshold to half current window */

        tcb_ptr->ssthresh = tcb_ptr->cwind / 2;
        tcb_ptr->ssthresh = MAX(tcb_ptr->ssthresh,tcb_ptr->mss);

        /* Shrink congestion window to 1 packet */

        tcb_ptr->cwind = tcb_ptr->mss;
        tcp_output(tcb_ptr);
      }
      else    
      /* exceed max allowable backoffs - bring down call */
      {
        ERR("TCP reset because of max backoffs",0,0,0);
        reset_tcp( tcb_ptr);
        /*------------------------------------------------------------------
         We reset tcp. A TCP RST segment will be sent and the state set to
         closed. On state change, psmgr will initiate ppp_close and bring
         down call.
       ----------------------------------------------------------------*/
      }
  }  /* switch on tcp state */

}  /* tcp_timeout_cb() */

/*===========================================================================

FUNCTION TCP_DELAYED_CB

DESCRIPTION
  This function performs the Delayed-ACK processing for TCP. This function
  is invoked to perform an ACK to the other side when there was no data to
  TX that could have an ACK piggybacked onto.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tcp_delayed_cb
( 
  void  *in_tcb_ptr                        /* pointer to TCP control block */
)
{
  struct tcb *tcb_ptr = NULL;                                /* Ptr to tcb */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check to make sure the parameters to this function are valid.
    If ASSERTs are turned on, this condition should be caught. 
    If ASSERTs are turned off, we exit from this function so that invalid 
    pointers will not be referenced or modified.
  -------------------------------------------------------------------------*/
  if( in_tcb_ptr == NULL)
  {
    ASSERT( 0);
    return;
  }

  tcb_ptr = (struct tcb *) in_tcb_ptr;

  /*-------------------------------------------------------------------------
    Since the timer has gone off, send a delayed ACK.
  -------------------------------------------------------------------------*/
  tcb_ptr->flags.force = TRUE;
  tcp_output(tcb_ptr);

} /* tcp_delayed_cb() */
#endif /* FEATURE_DS || FEATURE_DATA_PS */
