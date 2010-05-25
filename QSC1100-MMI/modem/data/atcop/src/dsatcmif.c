/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( C A L L  M A N A G E R  I N T E R F A C E )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to Call Manager.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatcmif_cm_call_cmd_handler
    This function is the handler function for the CM call related commands

  dsatcmif_cm_call_event_handler
    This function is the handler function for the CM call related events

  dsatcmif_cm_ss_cmd_handler
    This function is the handler function for the CM serving system commands

  dsatcmif_cm_ss_event_handler
    This function is the handler function for the CM serving system
    related events

  dsatcmif_get_cm_call_info
    This function issue query to Call Manager to get call info

  dsatcmif_get_cm_ss_info
    This function issue query to Call Manager to get serving system (SS) info

  dsatcmif_initiate_voice_call
    This function requests CM to start a voice call.

  dsatcmif_end_call
    This function issues a request to CM to end a call.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2002 - 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatcmif.c_v   1.4   12 Nov 2002 13:00:24   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatcmif.c#1 $ $DateTime: 2007/11/05 02:30:24 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/03   ar      Ensure call end event processed by both voice and packet 
                   call handlers.
03/07/03   ar      Adjust CM event reg sequence for better error reporting
                   Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/26/03   ar      Add support for PDP context activation
02/21/03   ar      Adjust +CPAS no call ID error handling.
02/12/03   wx      Accommodate cm_mm_call_cmd_end api change and 
                   cm_end_params_s_type struct change
01/16/03   ar      Featurized USSD event registration.
11/14/02   jd      Featurized gw_cs_ans under (GSM|WCDMA) for other builds
11/11/02   sb      Added CM Supplementary Services Registration.
11/08/02   sb      Initialize cm_status to CM_CLIENT_OK.
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/28/02   sb      Created module.


===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

/* this file should be included only in GSM or WCDMA mode */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#include "dsati.h"
#include "dsatcmif.h"
#include "dsatvoice.h"
#include "dsatparm.h"
#include "cm.h"
#include "assert.h"
#include "err.h"
#include "msg.h"
#include "string.h"



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
    Local definitions:
-------------------------------------------------------------------------*/
/* CM client (ATCOP) Identifier */
cm_client_id_type dsatcm_client_id;

/* CM call identifier */
byte dsatcm_current_call_id = 0;

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */

/*-------------------------------------------------------------------------
    Local functions:
-------------------------------------------------------------------------*/
LOCAL void cmif_client_init ( void );

LOCAL void cmif_call_event_cb_func 
(
  cm_call_event_e_type            event,            /* Event ID              */
  const cm_mm_call_info_s_type   *event_ptr         /* Pointer to Event info */
);

LOCAL void cmif_register_cb_func ( void );

/*-------------------------------------------------------------------------
    External function definitions:
-------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION  DSATCMIF_CALL_CMD_CB_FUNC

DESCRIPTION
  CM Call status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
void dsatcmif_call_cmd_cb_func 
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_call_cmd_e_type            cmd,              /* Command ID            */
  cm_call_cmd_err_e_type        cmd_err           /* Command error code    */
)
{
  ds_cmd_type * cmd_buf;
    
  if( (cmd_buf = ds_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("No ds command buffer",0,0,0);
  }
  else
  {
    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_AT_CM_CALL_CMD;
    cmd_buf->cmd.call_cmd.cmd = cmd;
    cmd_buf->cmd.call_cmd.cmd_err = cmd_err;
    ds_put_cmd(cmd_buf);
  }
}  /* dsatcmif_call_cmd_cb_func */

/*===========================================================================

FUNCTION  DSATCMIF_GET_CM_CALL_INFO

DESCRIPTION
  This function issue query to Call Manager to get call info

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_get_cm_call_info
(
  byte cmif_call_id
)
{
  dsat_result_enum_type result = DSAT_OK;

  if (TRUE !=  cm_mm_call_cmd_get_call_info (
                                             dsatcmif_call_cmd_cb_func,
                                             NULL,
                                             dsatcm_client_id,
                                             cmif_call_id
                                            )
     )
  {
    MSG_ERROR( "Could not request CM call info for call_id=%d",
               cmif_call_id, 0, 0 );
    result = DSAT_ERROR;
  }
  return result;
  
} /* dsatcmif_get_cm_call_info */

/*===========================================================================

FUNCTION DSATCMIF_END_CALL

DESCRIPTION
  This function issues a request to CM to end a call.

DEPENDENCIES

RETURN VALUE
  TRUE:     The end request is sent, callback function
              will be called.
  FALSE:    The end request failed.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
boolean dsatcmif_end_call( uint8 * call_id_ptr )
{
  /* Hangup the call */
  boolean                 call_status;
  cm_end_params_s_type    end_params[1];
  dsati_mode_e_type       mode;

  mode = dsatcmdp_get_current_mode();
  memset( &end_params[0],
          CM_CALL_CMD_PARAM_DEFAULT_VALUE,
          sizeof ( cm_end_params_s_type ));
  
  if (mode == DSAT_MODE_GSM || mode == DSAT_MODE_WCDMA) 
  {
    end_params[0].info_type = CM_CALL_MODE_INFO_GW_CS;
  }
  else if (mode == DSAT_MODE_CDMA)
  {
    end_params[0].info_type = CM_CALL_MODE_INFO_CDMA;
  }
  else
  {
    ERR("Invalid mode", 0, 0, 0);
    return FALSE;
  }

  end_params[0].call_id = *call_id_ptr;

  call_status = cm_mm_call_cmd_end(dsatcmif_call_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id,
                                   1,
                                   end_params);

  return call_status;
} /* dsatcmif_end_call */

/*===========================================================================

FUNCTION DSATCMIF_ANSWER_VOICE_CALL

DESCRIPTION
  This command answer a voice call by calling cm_call_cmd_answer

DEPENDENCIES

RETURN VALUE
  TRUE : If the command succeeds.
  FALSE: If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatcmif_answer_voice_call( uint8 * call_id_ptr)
{
  boolean                 call_status;
  cm_ans_params_s_type    answer_params;
  dsati_mode_e_type       mode;

  memset( &answer_params, CM_CALL_CMD_PARAM_DEFAULT_VALUE, sizeof( cm_ans_params_s_type));

  mode = dsatcmdp_get_current_mode();
  if (mode == DSAT_MODE_GSM || mode == DSAT_MODE_WCDMA) 
  {
    answer_params.info_type = CM_CALL_MODE_INFO_GW_CS;
#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif /* (defined(FEATURE_GSM) || defined(FEATURE_WCDMA)) */
  }
  else if (mode == DSAT_MODE_CDMA)
  {
    answer_params.info_type = CM_CALL_MODE_INFO_CDMA;
    answer_params.ans_params.cdma_ans.call_type = CM_CALL_TYPE_VOICE;
  }
  else
  {
    ERR("Invalid mode", 0, 0, 0);
  }

  call_status = cm_mm_call_cmd_answer(dsatcmif_call_cmd_cb_func,
                                   NULL,
                                   dsatcm_client_id,
                                   *call_id_ptr,
                                   /* not sure how mode specific data is used.
                                      I know I don't need them for voice dial. */
                                   &answer_params
                                   );
  return call_status;
} /* dsatcmif_answer_voice_call */

/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the CM call-related events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_call_event_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;

  switch (cmd_ptr->cmd.call_event.event)
  {
    case CM_CALL_EVENT_INCOM:
      result = dsatvoice_call_event_incom(
                 &cmd_ptr->cmd.call_event.event_info);
      break;
      
    case CM_CALL_EVENT_CONNECT:
      /* Check for call cmd originator */
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#else
      /* Voice call */
      result = dsatvoice_call_event_connect(
                 &cmd_ptr->cmd.call_event.event_info);
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
      break;
  
    case CM_CALL_EVENT_END:
      /* Voice call */
      result = dsatvoice_call_event_end(
                 &cmd_ptr->cmd.call_event.event_info);
        
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
      break;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

    default:
      MSG_HIGH ("Unsupport CM CALL event: %d",cmd_ptr->cmd.call_event.event,0,0);
      result = DSAT_ERROR;
      break;
  }

  return result;
} /* dsatcmif_cm_call_event_handler */

/*===========================================================================

FUNCTION  DSATCMIF_CM_CALL_CMD_HANDLER

DESCRIPTION
  This function is the handler function for the CM call-related commands

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatcmif_cm_call_cmd_handler
(
  dsat_mode_enum_type   mode,                /* AT command mode            */
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type result = DSAT_NO_RSP;

  ASSERT(mode == DSAT_CMD);

  switch (cmd_ptr->cmd.call_cmd.cmd)
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */    
    
    case CM_CALL_CMD_ORIG:
      /* Check for call cmd originator */
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#else
      /* Voice call */
      result = dsatvoice_cmd_orig_cb_handler(
                 cmd_ptr->cmd.call_cmd.cmd_err);

#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
      break;

    case CM_CALL_CMD_ANSWER:
      result = dsatvoice_cmd_answer_cb_handler(
               cmd_ptr->cmd.call_cmd.cmd_err);
      break;
  
    case CM_CALL_CMD_END:
      /* Check for call cmd originator */
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#else
      /* Voice call */
      result =
        dsatvoice_cmd_end_cb_handler(cmd_ptr->cmd.call_cmd.cmd_err);

#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */
      break;
    
  default:
    MSG_HIGH ("Unsupport CM CALL command: %d",cmd_ptr->cmd.call_cmd.cmd,0,0);
    result = DSAT_ERROR;
    break;
  }
  
  return result;
} /* dsatcmif_cm_call_cmd_handler */

/*===========================================================================

FUNCTION  DSATCMIF_INIT_CMIF

DESCRIPTION
  CM Interface initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to CM.
  - Registers the client call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatcmif_init_cmif ( void )
{
  /* Register ATCoP as a client to Call Manager */
  cmif_client_init ( );

  /* Call the registration function to register call back functions */
  cmif_register_cb_func ( );

  return;
}/* dsatcmif_init_cmif */

/************************ LOCAL FUNCTIONS *************************/
/*===========================================================================

FUNCTION   CMIF_CLIENT_INIT

DESCRIPTION
  Initializes ATCOP as a client with Call Manager task.
  
DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_client_init ( void )
{
  cm_client_status_e_type status;

  /* Initialize ATCOP as one of the clients of CM */
  status = cm_client_init (
                           CM_CLIENT_TYPE_ATCOP,
                           &dsatcm_client_id 
                          );

  /* This should not happen, raise an error */
  if (status != CM_CLIENT_OK)
  {
    ERR_FATAL( "ATCOP unable to register as CM client: %d", 
               status, 0, 0 );
  }

  return;
}/* cmif_client_init */

/*===========================================================================
FUNCTION  CMIF_CALL_EVENT_CB_FUNC

DESCRIPTION
  CM Call status event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer
===========================================================================*/
LOCAL void cmif_call_event_cb_func 
(
  cm_call_event_e_type            event,            /* Event ID              */
  const cm_mm_call_info_s_type   *event_ptr         /* Pointer to Event info */
)
{
  ds_cmd_type * cmd_buf;
    
  ASSERT (event_ptr != NULL);

  MSG_MED("ATCOP: cm event cb, call_event: %d, call_state: %d",
          event, event_ptr->call_state,0);

  if( (cmd_buf = ds_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("No ds command buffer",0,0,0);
  }
  else
  {
    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_AT_CM_CALL_INFO_CMD;

    /* If Event_ptr is NULL we should get this far */
    /* Pleasing lint here                          */
    if( NULL != event_ptr )
    {
      cmd_buf->cmd.call_event.event_info = *event_ptr;
    }
    cmd_buf->cmd.call_event.event      = event;
    ds_put_cmd(cmd_buf);
  }

} /* cmif_call_event_cb_func */

/*===========================================================================

FUNCTION  CMIF_REGISTER_CB_FUNC

DESCRIPTION
  Registers the callback functions.with other tasks.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void cmif_register_cb_func ( void )
{
  cm_client_status_e_type cm_status = CM_CLIENT_OK;

  /* Register the CM call event callback function */
  cm_status = cm_mm_client_call_reg (
                                   dsatcm_client_id,
                                   cmif_call_event_cb_func,
                                   CM_CLIENT_EVENT_REG,
                                   CM_CALL_EVENT_INCOM,
                                   CM_CALL_EVENT_CONNECT,  
                                   NULL
                                  );

  cm_status |= cm_mm_client_call_reg(dsatcm_client_id,
                                  cmif_call_event_cb_func,
                                  CM_CLIENT_EVENT_REG,
                                  CM_CALL_EVENT_END,
                                  CM_CALL_EVENT_END,
                                  NULL );

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif
  
  /* This should not happen, raise an error */
  if (cm_status!= CM_CLIENT_OK)
  {
    ERR_FATAL( "ATCOP unable to register CALL events: %d",
               cm_status, 0,0 );
  }

  
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

  /*-----------------------------------------------------------------------
    Activate the registered callback functions.
  -----------------------------------------------------------------------*/
  cm_status = cm_client_act( dsatcm_client_id );
  
  if (cm_status!= CM_CLIENT_OK)
  {
    ERR_FATAL( "ATCOP unable to activate client: %d",
               cm_status, 0,0 );
  }

  return;
}/* cmif_register_cb_func */ 

#endif /* FEATURE_DATA */

