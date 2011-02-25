#ifndef DSATVOICE_H
#define DSATVOICE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                        U M T S

                        C A L L   C O N T R O L
                
                A T   C O M M A N D   P R O C E S S I N G

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( UMTS call control commands ) processor.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.   
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatvoice.h_v   1.5   12 Jul 2002 10:05:20   randrew  $
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvoice.h_v   1.0   29 Oct 2002 13:42:24   sbandaru  $ $DateTime: 2008/04/11 07:54:07 $ $Author: nsivakum $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatvoice.h#1 $ $DateTime: 2008/04/11 07:54:07 $ $Author: nsivakum $ $DateTime: 2008/04/11 07:54:07 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/01   SB      created file

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

/*===========================================================================

            PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains public definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* need to keep a small state machine to handle 
   events little differently in different states.
   Search for "voice_state.state ==" for the cases. */
typedef enum 
{
  IDLE_STATE,/* not in voice call */
  DIALING,   /* dialed a voice call before call connected */
  WAITING_FOR_ANSWER,
             /* CM INCOM event recved, ringing the ph and expecting ATA 
                ATA will be accpected only in this state */
  CONNECTED, /* call connected before being ended */
  CONNECTED_OTHER_CLIENT,
             /* Other client originated call is connected. We can
                go to this state directly from IDLE_STATE.
                If we get an END event in this state we do nothing.
                If we get an END event in CONNECTED state, we return
                NO CARRIER to TE */
  VOICE_CALL /* doing voice, but not in DIALING, WAITING_FOR_ANSWER, 
                or CONNECTED, including
                call end issued to CM before going back to IDLE_STATE
                call answer issued to CM before get connected
                and voice_timer_cb */
} voice_state_e_type;

typedef struct {
  voice_state_e_type state;
  uint8 ring_counter;
  boolean ri_asserted;
} voice_state_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
void dsatvoice_init( void );


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
dsat_result_enum_type dsatvoice_answer_cmd( void );


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
dsat_result_enum_type dsatvoice_end_voice_call( void );


/*===========================================================================

FUNCTION DSATVOICE_VOICE_CALL_ABORT_HANDLER

DESCRIPTION
  This function is called by dsatact_call_abort_handler to abort
  voice dial command.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvoice_voice_call_abort_handler( void );


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
);


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
  DSAT_NO_RSP      : if current state is IDLE_STATE go to 
                     CONNECTED_OTHER_CLIENT state
  DSAT_ASYNC_EVENT : if the call type is not VOICE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_connect
(
  const cm_mm_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);
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
  DSAT_NO_RSP     : if the current state is WAITING_FOR_ANSWER.
  DSAT_ASYNC_EVENT: otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvoice_call_event_end
(
  const cm_mm_call_info_s_type    *call_info_ptr
  /* A pointer to a call state information structure */
);


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
);


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
);

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
);


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
);


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
boolean dsatvoice_is_voice( void );

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
void dsatvoice_go_to_dialing( void );

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
void dsatvoice_go_to_idle_state( void );

#endif /* FEATURE_DATA */

#endif /* DSATVOICE_H */

