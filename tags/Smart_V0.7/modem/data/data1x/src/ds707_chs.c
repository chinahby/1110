/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ C H S

GENERAL DESCRIPTION
  This file contains functions that are used to implement the control/hold
  state processing.
  
  Control/Hold can happen during SO33 data calls.  The mobile may request
  to go into/come out of control/hold at anytime during a SO 33 call.  
  
  The base station controls when the mobile goes into/out of control hold.
  The mobile can only make REQUESTS to go into/come out of control hold.
  
  This module is tied to ds707_rrm.c, which also manages ds707_scrm.  That 
  means that SCRM and Control/Hold processing are somewhat related (for 
  example, if in control/hold, the mobile can either SCRM or send an 
  RRM (for control/hold) to come out of the control/hold state.
  
 
  By default, the mobile does not request to go into control/hold.  The user
  can configure this by setting AT$QCCHS = 0 - 255:
    0 = mobile never initiates/requests to go into control/hold
    1 - 255 = number of 20-msec frames of idleness (both tx and rx) before
              mobile asks to go into control/hold.
  
EXTERNALIZED FUNCTIONS
  DS707_CHS_TXED_DATA
    Called by RLP TX function.  Indicates if "real" data was transmitted in
    the past 20-msec.
    
  DS707_CHS_RXED_DATA
    Called by RLP RX function.  Indicates if "real" data was received in
    the past 20-msec.
    
  DS707_CHS_TOGGLE_CH
    Registered with MC at startup.  Called by MC when going into and out of 
    control hold state.  Input = TRUE means going into Control/Hold state.
  
  DS707_CHS_LEAVE_CHS
    Called by RLP Tx if there is any data or control frames which need to
    be sent.  Called every 20-msecs regardless if in/not in c/h state.
   
  DS707_CHS_BUILD_BLOB
    Called by ds707_rrm_build_blob, when MC asks data to build a BLOB.  This
    builds the actual BLOB requesting mobile to come out of control/hold
    state.
  
  DS707_CHS_RETRY_DELAY
    Called when a retry order is received for control/hold.
  
  DS707_CHS_CALL_INIT
    Called at the beginning of every SO 33 call, to clear all control/hold
    state variables.
    
  DS707_CHS_REG_FUNCS
    Called at startup, to register appropriate functions with MC.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  At startup, must call ds707_chs_reg_funcs.  Currently called by
  ds707_rrm_reg_funcs() at startup..
  
  ds707_chs_call_init must be called at the beginning of every SO 33 call.
  Currently done via ds707_rrm_call_init.

  Implicit in this module is that rlp 3 calls ds_rrm_send_msg() every 20
  msec from both tx and rx.  This reduces complexity in this module.

 Copyright (c) 2001 by QUALCOMM Incorporated.  All Rights Reserved.
 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_chs.c_v   1.2   15 Nov 2002 17:55:18   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_chs.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
11/10/06   an         Fixed compilation errors for 6020
11/01/06   an         Featurize ATCOP, RMSM, Async Fax
11/15/02   ak         Updated header comments.
10/15/02   ak         Updated for Multimode DS.
05/11/01   ak         First revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_IS2000_CHS
#include "ds707_mc.h"
#include "ds707_rrm.h"
#include "dsrlp.h"
#include "err.h"
#include "mccdma.h"
#include "mccds.h"
#include "msg.h"

#ifndef FEATURE_DATA_STRIP_ATCOP
#include "dsat707vendctab.h"
#else
#include "dsatprofile.h"
#endif /* FEATURE_DATA_STRIP_ATCOP */

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  FALSE means not in Control/Hold.  TRUE means in Control/Hold.
---------------------------------------------------------------------------*/
LOCAL boolean ds707_chs_in_chs_state = FALSE;

/*---------------------------------------------------------------------------
  Timer variables.  All are in terms of 20-msec frame times.  So if the
  value is 1, then that timer is for 20 msec.
---------------------------------------------------------------------------*/
LOCAL uint32  ds707_chs_retry_timer;     /* time between requests to go to */
                                         /* Control/Hold State.            */
     
LOCAL uint32  ds707_chs_inactivity_timer; /* time there is no activity on  */
                                          /* Rx & Tx link before request to*/
                                          /* go to Control/Hold state is   */
                                          /* sent                          */
                                        
/*---------------------------------------------------------------------------
   #defines indicating number of frames in a second, number of milliseconds
   in a frame, and also a max time for the timer.  The max time, is spec'd
   in 20 msec-frames
---------------------------------------------------------------------------*/
#define CHS_MAX_TIMER_TIME         0xFFFFFFFF     /* in frame-times        */
#define CHS_FRAMES_PER_SEC         50             /* 50 frames per second  */
#define CHS_MSEC_PER_FRAME         20             /* 20 msec per frame     */

/*---------------------------------------------------------------------------
  How long to wait between requests sent to MC/BS.  Request can be either to
  go into Control Hold or to leave Control Hold.
---------------------------------------------------------------------------*/
#define CHS_MIN_RETRY              CHS_FRAMES_PER_SEC

#define CHS_DEBUG(string,a,b,c)    MSG_HIGH(string,a,b,c)


/*===========================================================================
                   INTERNAL MACRO DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CHS_INACTIVITY_SCALER

DESCRIPTION   Normally have to multiply the inactivity timer by 2, as it is
              decremented in two places per 20 msec.  Use a macro in case
              this ever changes.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
#define DS707_CHS_INACTIVITY_SCALER(a) (2 * a)


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_CHS_TXED_DATA

DESCRIPTION   Called by TXC task, via RLP 3, every 20 msec.  Input param:

              data_was_txed - TRUE  -> data was transmitted this 20 msec
                              FALSE -> DTX or IDLE sent.  No relevant data.
                             
              Whenever the input is FALSE, then decrement the timer.  If
              the input is TRUE, then reset the timer.

DEPENDENCIES  Must be called every 20 msec.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_txed_data
(
  boolean data_was_txed
)
{
  /*-------------------------------------------------------------------------
    If default timer is zero, then mobile does not initiate transition to
    Control/Hold.  If mobile already in CHS, then no need to decrement
    timers.
  -------------------------------------------------------------------------*/
  if ((dsat707_qcchs_val == 0) || (ds707_chs_in_chs_state == TRUE))
  {
    return;
  }

  INTLOCK();
  if (data_was_txed == TRUE)
  {
    /*-----------------------------------------------------------------------
      The value is decremented by rx and tx, so have to double the value.
    -----------------------------------------------------------------------*/
    ds707_chs_inactivity_timer = 
                             DS707_CHS_INACTIVITY_SCALER(dsat707_qcchs_val);
  }
  else if (ds707_chs_inactivity_timer > 0)
  {
    ds707_chs_inactivity_timer--;
  }
  INTFREE();
} /* ds707 chs_txed_data() */


/*===========================================================================
FUNCTION      DS707_CHS_RXED_DATA

DESCRIPTION   Called by RXC task, via RLP 3, every 20 msec.  Input param:

              data_was_rxed - TRUE  -> data was received this 20 msec
                              FALSE -> DTX or IDLE rx'ed.  No relevant data.
                             
              Whenever the input is FALSE, then decrement the timer.  If
              the input is TRUE, then reset the timer.
              
              If the timer expires, then send a message to MC which will
              cause MC to send a message to the BS.  This message will 
              request that we go to Control/Hold state.  As opposed to 
              leaving control/hold state, MC will not come back and ask us
              to build a BLOB.

DEPENDENCIES  Must be called every 20 msec.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_rxed_data
(
  boolean data_was_rxed
)
{
  mc_msg_type*        mc_cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - -*/

  /*-------------------------------------------------------------------------
    Always decrement retry timer, if greater than zero.
  -------------------------------------------------------------------------*/
  if (ds707_chs_retry_timer > 0)
  {
    ds707_chs_retry_timer--;
  }

  /*-------------------------------------------------------------------------
    If default timer is zero, then mobile does not initiate transition to
    Control/Hold.  If mobile already in CHS, then no need to decrement
    timers.
  -------------------------------------------------------------------------*/
  if ((dsat707_qcchs_val == 0) || (ds707_chs_in_chs_state == TRUE))
  {
    return;
  }

  INTLOCK();
  if (data_was_rxed == TRUE)
  {
    /*-----------------------------------------------------------------------
      The value is decremented by rx and tx, so have to double the value.
    -----------------------------------------------------------------------*/
    ds707_chs_inactivity_timer = 
                             DS707_CHS_INACTIVITY_SCALER(dsat707_qcchs_val);
  }
  else if (ds707_chs_inactivity_timer > 0)
  {
    ds707_chs_inactivity_timer--;
  }
  INTFREE();

  if ((ds707_chs_inactivity_timer == 0) && (ds707_chs_retry_timer == 0))
  {
    /*-----------------------------------------------------------------------
      Send MSG to MC. This instantly sends a message to the BS.  There is
      no callback from MC asking for a BLOB (i.e., this is different than
      when we want to LEAVE control/hold state).  Set a timer so that we
      don't pester MC & BS too much.
    -----------------------------------------------------------------------*/
    INTLOCK();
    mc_cmd_ptr           = ds707_mc_get_cmd();
    mc_cmd_ptr->hdr.cmd  = MC_RES_REL_F;
    mc_cmd(mc_cmd_ptr);

    ds707_chs_retry_timer = CHS_MIN_RETRY;
    INTFREE();

    CHS_DEBUG("Tell MC go to CHS",0,0,0);
  }
} /* ds707_chs_rxed_data() */



/*===========================================================================
FUNCTION      DS707_CHS_TOGGLE_CH

DESCRIPTION   Called by MC, when Control/Hold is turned on or off.  Two
              parameters:
                 TRUE - Going into Control/Hold State
                 FALSE - No longer in Control/Hold State.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_toggle_ch
(
  boolean      in_chs_state
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  INTLOCK();
  ds707_chs_in_chs_state = in_chs_state;
  if (ds707_chs_in_chs_state == FALSE)
  {
    ds707_chs_inactivity_timer = 
                             DS707_CHS_INACTIVITY_SCALER(dsat707_qcchs_val);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Have entered CHS.  Do not prevent mobile from trying to leave CHS ASAP
    -----------------------------------------------------------------------*/
    ds707_chs_retry_timer = 0;
  }
  INTFREE();
  CHS_DEBUG("TOGGLED CHS State: %d",ds707_chs_in_chs_state,0,0);
} /* ds707_chs_toggle_ch() */


/*===========================================================================
FUNCTION     DS707_CHS_LEAVE_CHS

DESCRIPTION  Called by data, when it wants to send a msg to the BS to leave
             Control/Hold state.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
void ds707_chs_leave_chs(void)
{
  mc_msg_type*        mc_cmd_ptr;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - -*/
  if ((ds707_chs_in_chs_state == FALSE) ||
      (ds707_rrm_awaiting_mc == TRUE)  ||
      (ds707_chs_retry_timer > 0)
     )
  {
    /*-----------------------------------------------------------------------
      Waiting for MC to ask us for BLOB info, or waiting for timer to expire.
      Or no longer in Control/Hold, and so no need to continue.
    -----------------------------------------------------------------------*/
    return;
  }

  INTLOCK();
  ds707_rrm_awaiting_mc  = TRUE;
  mc_cmd_ptr             = ds707_mc_get_cmd();
  mc_cmd_ptr->hdr.cmd    = MC_RES_REQ_F;
  mc_cmd(mc_cmd_ptr);
  INTFREE();

  CHS_DEBUG("Send MC Leave CHS",0,0,0);
} /* ds707_chs_leave_chs() */


/*===========================================================================
FUNCTION     DS707_CHS_BUILD_BLOB

DESCRIPTION  Called by ds_rsrc_request_func, to see if an RRM should be
             built.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
boolean ds707_chs_build_blob(void)
{
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((ds707_chs_in_chs_state == TRUE) && (ds707_chs_retry_timer == 0))
  {
    ret_val = TRUE;
  }

  /*-------------------------------------------------------------------------
    Since got called by MC, set timer to 1-second to prevent asking to
    soon again
  -------------------------------------------------------------------------*/
  ds707_chs_retry_timer = CHS_MIN_RETRY;
  return(ret_val);
} /* ds707_chs_build_blob() */

/*===========================================================================
FUNCTION     DS707_CHS_RETRY_PROCESSING

DESCRIPTION  Called to clear RRM timer.

DEPENDENCIES None.

RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_chs_retry_processing(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ds707_chs_retry_timer         = 0;
} /* ds707_chs_retry_processing() */


/*===========================================================================
FUNCTION     DS707_CHS_RETRY_DELAY

DESCRIPTION  Called by MC when a RRM retry delay is received.

DEPENDENCIES None.

RETURN VALUE None

SIDE EFFECTS None.
===========================================================================*/
void ds707_chs_retry_delay
(
  retry_delay_type retry_parm
)
{
  if (retry_parm.infinite_delay == TRUE)
  {
    /*-----------------------------------------------------------------------
      If the delay is infinite set it to its max value.  At this value, it
      will take > 900 days for it to expire.
    -----------------------------------------------------------------------*/
    INTLOCK();
    ds707_chs_retry_timer = CHS_MAX_TIMER_TIME;
    INTFREE();
  }
  else
  {
    INTLOCK();
    if (retry_parm.retry_delay_duration == 0)
    {
      ds707_chs_retry_timer = CHS_FRAMES_PER_SEC;
    }
    else
    {
      ds707_chs_retry_timer = 
                    (retry_parm.retry_delay_duration/CHS_MSEC_PER_FRAME) + 1;
    }
    INTFREE();
  }
} /* ds707_chs_retry_delay() */


/*===========================================================================
FUNCTION      DS707_CHS_CALL_INIT

DESCRIPTION   Called at beginning of a call to reset values.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_call_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  INTLOCK();
  ds707_chs_in_chs_state      = FALSE;
  ds707_chs_retry_timer       = CHS_MIN_RETRY;
  ds707_chs_inactivity_timer  = 
                             DS707_CHS_INACTIVITY_SCALER(dsat707_qcchs_val);
  INTFREE();
} /* ds707_chs_call_init() */

/*===========================================================================
FUNCTION      DS707_CHS_REG_FUNCS

DESCRIPTION   Causes module to register callbacks with MC.  Should only be
              called once, at startup.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_chs_reg_funcs(void)
{
  mccds_reg_chs_notifier(ds707_chs_toggle_ch);
}
#endif /* FEATURE_IS2000_CHS */
#endif /* FEATURE_DATA_IS707 */

