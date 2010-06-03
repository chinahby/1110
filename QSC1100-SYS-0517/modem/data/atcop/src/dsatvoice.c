/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S
                
                A T   C O M M A N D

                V O I C E  I N T E R F A C E

GENERAL DESCRIPTION
  This Module has the Voice command related State Machine.
  It has Call Back and event handlers. It also contains internal
  State machine
  
EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatvoice_init
    Initiate voice call related vaiables.

  dsatvoice_call_event_connect
    This function is called by dsatvoice_cm_call_event_handler when CM 
    report the CM_CALL_EVENT_CONNECT event.

  dsatvoice_answer_cmd
    Handle ATA voice answer commands.  This function is called by 
    dsatact_exec_ata_cmd when the current call's call type is VOICE.

  dsatvoice_end_voice_call
    This function is called by dsatact_exec_ath_cmd to end current
    active call if it is a voice call.

  dsatvoice_cmd_answer_cb_handler
    This function is the handler for CM_CALL_CMD_ANSWER cb cmd.

  dsatvoice_cmd_end_cb_handler
    This function is the handler for CM_CALL_CMD_END cb cmd.

  dsatvoice_voice_call_abort_handler
    This function is called by dsatact_call_abort_handler to abort
    voice dial command.

  dsatvoice_call_event_incom
    This function is called by dsatcmif_cm_call_event_handler when CM 
    report the CM_CALL_EVENT_INCOM event.

  dsatvoice_call_event_end
    This function is call by dsatcmif_cm_call_event_handler when CM report 
    the CM_CALL_EVENT_END event.

  dsatvoice_timer_expired_handler
    This function is the handler function for the RI timer expired cmd.

  dsatvoice_is_voice
    Test if we are doing voice call.

  dsatvoice_go_to_dialing
    Change to dialing state.

  dsatvoice_go_to_idle_state
    Change to IDLE_STATE state.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatvoice.c_v   1.7   12 Jul 2002 10:05:08   randrew  $
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvoice.c_v   1.1   30 Oct 2002 13:22:10   sramacha  $ $DateTime: 2007/11/05 02:30:24 $ $Author: nsivakum $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatvoice.c#1 $ $DateTime: 2007/11/05 02:30:24 $ $Author: nsivakum $ $DateTime: 2007/11/05 02:30:24 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/03   sb      CDMA result code handler cannot handle CRC result code for
                   voice RING indication. Use basic result code instead.
02/26/03   wx      Put dialed number into last dial ph book.
10/28/02   sb      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#include "dsati.h"
#include "dsatctab.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "dsatparm.h"
#ifdef FEATURE_DATA_MM
  #include "ds3gsiolib.h"
#else
  #include "ds3gsioif.h"
#endif /* FEATURE_DATA_MM */

#include "rex.h"

#include "assert.h"
#include "err.h"
#include "msg.h"
#include "dsat.h"

#ifdef FEATURE_ETSI_PBM
#include "dsatetsime.h"
#endif /* FEATURE_ETSI_PBM */

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* timer id */
#define VOICE_RING_TIMER_ID 0

#define VOICE_DEASSERT_INTERVAL 100;

#define VOICE_ASSERT_INTERVAL 5900;

/* current call's call_id */
cm_call_id_type   voice_call_id;

voice_state_type voice_state = {
  IDLE_STATE, 0, FALSE
};

LOCAL rex_timer_type voice_ring_timer;

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL void voice_go_to_connected( void );

LOCAL void voice_ring_te( void );

LOCAL void voice_timer_cb(unsigned long);

LOCAL void voice_go_to_waiting_for_answer( void );

LOCAL void voice_go_to_voice_call( void );

LOCAL void voice_go_to_connected_other_client( void );

LOCAL void voice_stop_ring( void );

/*-------------------------------------------------------------------------
    External Function Definitions
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION DSATVOICE_INIT

DESCRIPTION
  Initiate voice call related vaiables.
  It includes the ring timer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_init( void )
{
  rex_def_timer_ex(&voice_ring_timer, voice_timer_cb, 
                   VOICE_RING_TIMER_ID);

} /* dsatvoice_init */

/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_END

DESCRIPTION
  This function is call by dsatcmif_cm_call_event_handler when CM report 
  the CM_CALL_EVENT_END event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_NO_CARRIER : if the current state is not one of IDLE_STATE, 
                    CONNECTED_OTHER_CLIENT, WAITING_FOR_ANSWER.
  DSAT_ASYNC_EVENT: otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_end
(
  const cm_mm_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  
  /* we only care about voice call and emergency call */
  if (call_info_ptr->call_type == CM_CALL_TYPE_VOICE ||
      call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY )
  {
    if(voice_state.state == WAITING_FOR_ANSWER)
    {
      voice_stop_ring();
      result = DSAT_ASYNC_EVENT;
    }
    else if( (voice_state.state != IDLE_STATE) &&
             (voice_state.state != CONNECTED_OTHER_CLIENT)
             )
    {
      result = DSAT_NO_CARRIER;
    }
    /* else: no special action for else */
    dsatvoice_go_to_idle_state();
  }
  
  return result;

} /* dsatvoice_call_event_end */

/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_INCOM

DESCRIPTION
  This function is called by dsatcmif_cm_call_event_handler when CM 
  report the CM_CALL_EVENT_INCOM event.

  If the call type is VOICE, it rings the TE and goes to WAITING_FOR_ANSWER
  state.

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_EVENT : Tell the async event handler this event is not
                      associated with an AT command line.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_incom
(
  const cm_mm_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  if ( (call_info_ptr->call_type == CM_CALL_TYPE_VOICE ||
        call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
       && (voice_state.state == IDLE_STATE))
  {
    /* save the call_id for ata */
    voice_call_id = call_info_ptr->call_id;
    
    /* used to distinguish voice from data for ATA handling */
    voice_go_to_waiting_for_answer();

    /* ring TE */
    voice_ring_te();

  }  /* else do nothing */

  /* This event is an async event but it is not associate
     with an AT command line.  Returning DSAT_ASYNC_EVENT
     tell the master async handler, dsat_process_async_cmd,
     not try to process the rest tokens of an AT command line */
  return DSAT_ASYNC_EVENT;
} /* dsatvoice_call_event_incom */

/*===========================================================================

FUNCTION DSATVOICE_CALL_EVENT_CONNECT

DESCRIPTION
  This function is called by dsatvoice_cm_call_event_handler when CM 
  report the CM_CALL_EVENT_CONNECT event.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK          : if current state is WAITING_FOR_ANSWER or DIALING
                     state, go to CONNECTED state
  DSAT_ASYNC_EVENT : if the call type is not VOICE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_connect
(
  const cm_mm_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
)
{
  dsat_result_enum_type result;

  /* only care about voice calls */
  if(call_info_ptr->call_type == CM_CALL_TYPE_VOICE ||
     call_info_ptr->call_type == CM_CALL_TYPE_EMERGENCY
     )
  {
    if(voice_state.state == IDLE_STATE)
    {
      voice_go_to_connected_other_client();
      result = DSAT_ASYNC_EVENT;
    }
    else
    {
      if (voice_state.state == WAITING_FOR_ANSWER)
      {
        voice_stop_ring();
      }
      voice_go_to_connected();
      result = DSAT_OK;
    }
  }
  else
  {
    /* unwanted event, do nothing */
    result = DSAT_ASYNC_EVENT;
  }

  return result;

} /* dsatvoice_call_event_connect */

/*===========================================================================

FUNCTION DSATVOICE_ANSWER_CMD

DESCRIPTION
  Handle ATA voice answer commands.  This function is called by 
  dsatact_exec_ata_cmd when the current call's call type is VOICE.

  This function sends request to CM to answer current incoming call.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : If the command succeeds.
  DSAT_NO_CARRIER: If the request is rejected by CM.
  DSAT_ERROR     : If ATA is not expected.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_answer_cmd( void )
{
  dsat_result_enum_type result;

  if(voice_state.state == WAITING_FOR_ANSWER)
  {
    voice_stop_ring();

    if(dsatcmif_answer_voice_call(&voice_call_id))
    {
      voice_go_to_voice_call();

      /* the command callback function will be called */
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      ERR("ATCOP's answer-call is denied by CM",0,0,0);
      /* reset voice flag */
      dsatvoice_go_to_idle_state();
      result = DSAT_NO_CARRIER;
    }

  }
  else
  {
    /* ATA here is rejected.
       ATA is valid only only when there is a incoming
       call waiting for answer. */
    result = DSAT_ERROR;
  }

  return result;
} /* dsatvoice_answer_cmd */

/*===========================================================================

FUNCTION DSATVOICE_CMD_ANSWER_CB_HANDLER

DESCRIPTION
  This function is the handler for CM_CALL_CMD_ANSWER cb cmd.

  If status shows cm_call_cmd_answer failed, return NO CARRIER to TE and
  go back to IDLE_STATE.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : no error.
  DSAT_NO_CARRIER: there is error.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_answer_cb_handler
(
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
)
{
  dsat_result_enum_type result;

  MSG_MED("Answer cb called: %d", call_cmd_err, 0,0);
  ASSERT(voice_state.state == VOICE_CALL);

  if(call_cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
    /* Waiting for call events */
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    dsatvoice_go_to_idle_state();
    result = DSAT_NO_CARRIER;
  }

  return result;
} /* dsatvoice_cmd_answer_cb_handler */

/*===========================================================================

FUNCTION  DSATVOICE_TIMER_EXPIRED_HANDLER 

DESCRIPTION
  This function is the handler function for the RI timer expired cmd.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_EVENT

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvoice_timer_expired_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  switch (cmd_ptr->cmd.dsat_timer_id)
  {
    case VOICE_RING_TIMER_ID:
      /* continue ring te */
      voice_ring_te();
      break;

    default:
      ERR_FATAL("Illegal timer_id: %d", cmd_ptr->cmd.dsat_timer_id,0,0);
  }

  /* This event is an async event but it is not associate
     with an AT command line.  Returning DSAT_ASYNC_EVENT
     tell the master async handler, dsat_process_async_cmd,
     not try to process the rest tokens of an AT command line */
  return DSAT_ASYNC_EVENT;
} /* dsatvoice_timer_expired_handler */


/*===========================================================================

FUNCTION DSATVOICE_CMD_ORIG_CB_HANDLER

DESCRIPTION
  This function is the handler for a CM_CALL_CMD_ORIG cb cmd.

  This function handles cm_call_cmd_orig call status.  It returns
  dsat_result based on err conditions.  Go to IDLE_STATE if there is
  error.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if no error reported by CM
  DSAT_NO_CARRIER: if the one of the following errors is reported by CM,
                   CM_CALL_CMD_ERR_OFFLINE_S, 
                   CM_CALL_CMD_ERR_ORIG_RES_S,
                   CM_CALL_CMD_ERR_EMERGENCY_MODE_S, 
                   CM_CALL_CMD_ERR_SRV_TYPE_S,
                   CM_CALL_CMD_ERR_CALL_STATE_S,
                   CM_CALL_CMD_ERR_SRV_STATE_S
  DSAT_BUSY      : if the following error is reported by CM,
                   CM_CALL_CMD_ERR_IN_USE_S
  DSAT_ERROR     : other CM errors

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_orig_cb_handler
(
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
)
{
  dsat_result_enum_type result;

  MSG_MED("Orig cb called: %d.", call_cmd_err, 0,0);

  if(call_cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
    /* Waiting for call events */
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    if (call_cmd_err == CM_CALL_CMD_ERR_OFFLINE_S ||
        call_cmd_err == CM_CALL_CMD_ERR_ORIG_RES_S ||
        call_cmd_err == CM_CALL_CMD_ERR_EMERGENCY_MODE_S ||
        call_cmd_err == CM_CALL_CMD_ERR_SRV_TYPE_S ||
        call_cmd_err == CM_CALL_CMD_ERR_CALL_STATE_S ||
        call_cmd_err == CM_CALL_CMD_ERR_SRV_STATE_S)
    {
      result = DSAT_NO_CARRIER;
    }
    else if (call_cmd_err == CM_CALL_CMD_ERR_IN_USE_S)
    {
      result = DSAT_BUSY;
    }
    else
    {
      result = DSAT_ERROR;
    }

    dsatvoice_go_to_idle_state();
  }
  
  return result;
} /* dsatvoice_cmd_orig_cb_handler */

/*===========================================================================

FUNCTION DSATVOICE_END_VOICE_CALL

DESCRIPTION
  This function is called by dsatact_exec_ath_cmd to end current
  active call if it is a voice call.

  This function issues a request to CM to end a voice call.

DEPENDENCIES

RETURN VALUE
  DSAT_ASYNC_CMD : The end request is sent, callback function
                   will be called.
  DSAT_ERROR     : The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvoice_end_voice_call( void )
{
  dsat_result_enum_type result;

  if(dsatcmif_end_call(&voice_call_id))
  {
    /* the command callback function will be called */
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    /* no buffer to send the request */
    ERR("ATCOP's end call request is denied by CM",0,0,0);
    result = DSAT_ERROR;
  }
  return result;
} /* dsatvoice_end_voice_call */

/*===========================================================================

FUNCTION DSATVOICE_CMD_END_CB_HANDLER

DESCRIPTION
  This function is the handler for CM_CALL_CMD_END cb cmd.

  If status shows cm_call_cmd_end succeeded, return OK to TE and 
  go to IDLE_STATE. 
  If status shows cm_call_cmd_end failed, return ERROR to TE.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK   : if no error.
  DSAT_ERROR: if there is error.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_cmd_end_cb_handler
(
  cm_call_cmd_err_e_type    call_cmd_err
  /* Call command error code, indicating whether command
  ** is rejected and the reason */
)
{
  dsat_result_enum_type result;

  MSG_MED("End cb called: %d.", call_cmd_err, 0,0);
  
  if(call_cmd_err == CM_CALL_CMD_ERR_NOERR)
  {
    if(voice_state.state == WAITING_FOR_ANSWER)
    {
      voice_stop_ring();
    }

    dsatvoice_go_to_idle_state();
    result = DSAT_OK;
  }
  else
  {
    /* we could not end the call */
    ERR("End call failed",0,0,0);
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatvoice_cmd_end_cb_handler */

/*===========================================================================

FUNCTION DSATVOICE_VOICE_CALL_ABORT_HANDLER

DESCRIPTION
  This function is called by dsatact_call_abort_handler to abort
  voice dial command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_voice_call_abort_handler( void )
{
  boolean                 call_status;

  ASSERT(dsatvoice_is_voice());

  call_status = dsatcmif_end_call(&voice_call_id);

  if(!call_status)
  {
    ERR("ATCOP's end call request is denied by CM",0,0,0);
  }

} /* dsatvoice_voice_call_abort_handler */

/*===========================================================================

FUNCTION DSATVOICE_IS_VOICE

DESCRIPTION
  Test if we are doing voice call.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if we are doing voice call.
  FALSE: otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatvoice_is_voice( void )
{
  return (voice_state.state != IDLE_STATE);
}

/*===========================================================================

FUNCTION DSATVOICE_GO_TO_IDLE_STATE

DESCRIPTION
  Change to IDLE_STATE state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_idle_state( void )
{
  voice_state.state = IDLE_STATE;

#ifdef FEATURE_ETSI_PBM
  /* save last dial record to ph book if we were dialing */
  {
    dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();

    if ((current_mode == DSAT_MODE_GSM) || (current_mode == DSAT_MODE_WCDMA))
    {
      dsatetsime_ld_save();
    }
  }
#endif /* FEATURE_ETSI_PBM */

}/* dsatvoice_go_to_idle_state */

/*===========================================================================

FUNCTION DSATVOICE_GO_TO_DIALING

DESCRIPTION
  Change to dialing state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_go_to_dialing( void )
{
  voice_state.state = DIALING;
}/* dsatvoice_go_to_dialing */

/*-------------------------------------------------------------------------
    LOCAL Function Definitions
-------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION VOICE_RING_TE

DESCRIPTION
  Ring TE and activate auto anwser depending on S0 value.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_ring_te()
{
  boolean call_answered = FALSE;
  int duration = 0;

  if(voice_state.state != WAITING_FOR_ANSWER)
  {
    ERR("Ring TE in state %d", voice_state.state,0,0);
    voice_stop_ring();
    return;
  }

  if(voice_state.ri_asserted)
  {
    /* call ds3g to deassert ri */
#ifdef FEATURE_DATA_MM
     (void)ds3g_siolib_ri_deassert();
#else
    (void)ds3g_sio_ri_deassert();
#endif /* FEATURE_DATA_MM */
    duration = VOICE_DEASSERT_INTERVAL;
    voice_state.ri_asserted = FALSE;

    if( dsat_s0_val != 0 )
    {
      if(++voice_state.ring_counter == dsat_s0_val)
      {
        voice_stop_ring();

        /* S0 times ring reached, answer the call */
        if(dsatcmif_answer_voice_call(&voice_call_id))
        {
          /* we are not waiting for an ATA anymore */
          voice_go_to_voice_call();
          call_answered = TRUE;
        }
        else
        {
          /* no buffer to send the request */
          ERR("ATCOP's auto answer-call is denied by CM",0,0,0);
          dsat_send_result_code(DSAT_NO_CARRIER);
          dsatvoice_go_to_idle_state();
          call_answered = TRUE;
        }
      } /* ring counter reach auto-answer num */
    } /* don't do auto answer */
  } /* if(voice_state.ri_asserted) */
  else /* ri is not asserted */
  {
    /* Send the RING result code to the TE. */
    dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();

    if ( current_mode == DSAT_MODE_CDMA )
    {
      /* CDMA result code handler cannot handle CRC result code */
      dsat_send_result_code(DSAT_RING);
    }
    else
    {
      dsat_send_result_code(DSAT_CRC_CODE_VOICE);
    }

    /* assert RI */
#ifdef FEATURE_DATA_MM
     (void)ds3g_siolib_ri_assert();
#else
    (void)ds3g_sio_ri_assert();
#endif /* FEATURE_DATA_MM */
    duration = VOICE_ASSERT_INTERVAL;
    voice_state.ri_asserted = TRUE;
  }

  if( !call_answered )
  {
    /*---------------------------------------------------------------------
      Call was not answered, re-start the ring timer.
      ---------------------------------------------------------------------*/
    (void) rex_set_timer(&voice_ring_timer, (dword) duration );
  }

} /* voice_ring_te */

/*===========================================================================

FUNCTION VOICE_TIMER_CB

DESCRIPTION
  This function is called when ring timer expires.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_timer_cb(unsigned long timer_id)
{
  /* submit dstask a cmd */
  ds_cmd_type *cmd_ptr;

    /*-------------------------------------------------------------------------
    Send a DS_TIMER_EXPIRED_CMD to the DS task, and indicate which timer
    expired.
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ds_get_cmd_buf() ) != NULL )
  {
    cmd_ptr->hdr.cmd_id = DS_AT_TIMER_EXPIRED_CMD;
    cmd_ptr->cmd.dsat_timer_id = timer_id;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "Can't get cmd buf from DS task", 0, 0, 0 );
  }

} /* voice_timer_cb */

/*===========================================================================

FUNCTION VOICE_GO_CONNECTED

DESCRIPTION
  Change to CONNECTED state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_go_to_connected( void )
{
  voice_state.state = CONNECTED;

#ifdef FEATURE_ETSI_PBM
  /* change last dial record of ph book to be connected */
  dsatetsime_ld_connected();
#endif /* FEATURE_ETSI_PBM */
} /* voice_go_to_connected */

/*===========================================================================

FUNCTION VOICE_GO_WAITING_FOR_ANSWER

DESCRIPTION
  Change to WAITING_FOR_ANSWER state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_go_to_waiting_for_answer( void )
{
  voice_state.state = WAITING_FOR_ANSWER;
}

/*===========================================================================

FUNCTION VOICE_GO_TO_VOICE_CALL

DESCRIPTION
  Change to VOICE_CALL state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_go_to_voice_call( void )
{
  voice_state.state = VOICE_CALL;
}

/*===========================================================================

FUNCTION VOICE_GO_CONNECTED_OTHER_CLIENT

DESCRIPTION
  Change to CONNECTED_OTHER_CLIENT state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void voice_go_to_connected_other_client( void )
{
  voice_state.state = CONNECTED_OTHER_CLIENT;
}

/*===========================================================================

FUNCTION VOICE_STOP_RING

DESCRIPTION
  Stop ringing TE.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void voice_stop_ring( void )
{
  /* stop ringing */
  (void) rex_clr_timer(&voice_ring_timer);
  voice_state.ring_counter = 0;
  /* de-assert the RI */
#ifdef FEATURE_DATA_MM
  (void)ds3g_siolib_ri_deassert();
#else
  (void)ds3g_sio_ri_deassert();
#endif /* FEATURE_DATA_MM */
  voice_state.ri_asserted = FALSE;
} /* voice_stop_ring */
/*-------------------------------------------------------------------------*/
#endif /* FEATURE_DATA */
