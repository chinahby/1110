/*===========================================================================


            G S T K   E N V E L O P E   R S P

GENERAL DESCRIPTION
  This source file contains functions to handle envelope response from the
  card


FUNCTIONS
  gstk_uim_envelope_report
    This is the callback function for envelope response.

  gstk_menu_selection_rsp
    This function builds the menu selection rsp and sends it to client

  gstk_evt_dl_rsp
    This function builds the event download rsp and sends it to client

  gstk_sms_pp_dl_rsp
    This function builds the sms pp download rsp and sends it to client

  gstk_cc_rsp
    This function builds the call control rsp and sends it to client

  gstk_mo_sms_ctrl_rsp
    This function builds the sms control download rsp and sends it to client

  gstk_timer_expire_rsp
    This function builds the timer expire rsp and sends it to client

  gstk_cell_broadcast_dl_rsp
    This function builds the cell broadcast download rsp and sends it to client
    
  gstk_process_envelope_rsp
    This function performs a lookup on the command id with the
    envelope_cmd_ref_table and based on the envelope rsp type, it calls the
    corresponding envelope rsp function so that GSTK can build the corrresponding
    envelope response and sends it to the client

  gstk_raw_envelope_rsp
    This function builds the raw envelope response and sends it to client

  gstk_non_idle_st_raw_env_err_rsp
    This function builds the raw envelope non idle state error rsp and sends 
    it to client

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_envelope_rsp.c#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/02/09   xz      Set status of SMS PP DL rsp to FAIL for 0x62XX or 0x63XXX
06/22/09   xz      Fixed infinite loop issue when an error TLV exists in CC rsp
06/10/09   yb      Fix to send status word as received in get response to AT COP
                   even if card returns error for envelope sent.
04/28/09   sg      Added static keyword to function defintion in 
                   1)gstk_check_valid_tag_for_cc_rsp
                   2)gstk_is_tag_valid_for_command
03/02/09   xz      Provide SW1 and SW2 support for raw APDU responses
02/11/09   yb      Added support to handle envelope response for Channel
                   status event in gstk_process_envelope_rsp
11/13/08   xz      Fixed klockwork errors
09/14/08   sk      Fixed lint and klockwork errors
09/08/08   xz      Envelope rsp fails due to invalid client id
08/22/08   xz      Move processing of raw envelope rsp to switch statement
08/21/08   sk      Changed function name.
08/08/08   xz      Add support for raw envelope response
08/04/08   sk      Fixed sending of envelope response to clients.
06/20/08   xz      Added support for 6F C2 for call control envelope response
05/06/08   sk      Added support for event download envelope retry
03/17/08   sk      Fixed klockwork errors
02/23/08   yb      Added support for parsing unexpected and unknown tlvs for
                   envelope call control response
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
08/21/06   sk      Added featurization for GPRS call control
07/25/06   sk      Added support to handle envelope cmds in non idle state
06/24/06   sk      Added data available envelope enum to avoid holding up 
                   envelope ref id.
06/06/06   sk      Added support for network search mod
05/15/06   tml     GPRS Call Control support
05/03/06   tml     lint
04/06/06/  tml     Populate information even if UIM_FAILED returned
                   but do not copy the data from uim report
03/16/06   sk      Fixed potential data abort problem
11/09/05   sk      Fixed Lint Errors
09/15/05   tml     Fixed link error: move gstk_build_error_envelope_report
                   to gstk_envelope_rsp.h
08/10/05   sk      Fixed call control type in gstk_cc_rsp()
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added new fn gstk_build_error_envelope_report()
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
01/10/05  sk/tml   Changed parser function for ussd string
11/23/04    sk     Added SAT/USAT data support
08/10/04   tml     Added checking in CALL CONNECTED envelope response to
                   determine if more CALL CONNECTED envelope is required to be
                   sent to the card
06/04/04   tml     timer management and language notification support
06/03/04   tml     Added 6F00 handling for Call Ctrl and MO SMS Ctrl
05/28/04   tml     Added F3 message for envelope status word from UIM
03/01/04   tml     Merged from branch - fixes for location status env command
07/07/03   tml     Removed unnecessary messages
05/16/03   tml     Linted and fixed call control issue
04/29/03   tml     Properly initialized data struct
04/21/03   tml     Correctly interrept UIM status words
04/15/03   tml     Fixed F3 message for MO sms control response
03/20/03   tml     MO SMS control fix
03/14/03   tml     Call Control bug fix
03/11/03   tml     Fixed call control comparison error
03/06/03   tml     Updated enum names
02/25/03   tml     Added user data support
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "gstk_envelope_rsp.h"
#include "uim.h"
#include "gstkutil.h"

#include "string.h"
#include "tmc.h"
#include "msg.h"
#include "err.h"



/*===========================================================================
FUNCTION gstk_uim_envelope_report

DESCRIPTION

  This is the callback function for envelope response.  UIM will call
  this function when it receives envelope response from the Card
  Based on the return status of the envelope response, this function
  will build the corresponding envelope resp type and put it to the
  **front** of the gstk command queue

PARAMETERS
  report_ptr: [Input] Pointer to the UIM envelope report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_uim_envelope_report (uim_rpt_type *report_ptr)
{
  gstk_task_cmd_type   * task_cmd_ptr  = NULL;
  gstk_cmd_enum_type   type_of_command = GSTK_CMD_ENUM_MAX;
  
  if(report_ptr == NULL)
  {
    MSG_ERROR("report ERR:NULL",0,0,0);
    return;
  }

  if (report_ptr->user_data >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    MSG_ERROR("invalid envelope cmd ref id 0x%x", report_ptr->user_data, 0, 0);
    return;
  }

  MSG_HIGH("GSTK recv UIM envelope rsp, 0x%x, 0x%x", 
            report_ptr->sw1, report_ptr->sw2, 0);

  if (report_ptr->rpt_type != UIM_ENVELOPE_R) 
  {
    MSG_ERROR("ENVELOPE expected in CB", 0, 0, 0);
    return;
  }

  /* get command buf */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if ( task_cmd_ptr == NULL )
  {
    MSG_MED("No more gstk buf", 0, 0, 0);
    return;
  } 
  /* successfully obtained command buffer . Now build the envelope response */
  task_cmd_ptr->cmd.envelope_response.message_header.command_group = GSTK_ENVELOPE_RSP;
  task_cmd_ptr->cmd.envelope_response.message_header.command_id = (uint8)report_ptr->user_data;
  /* index location in the envelope_cmd_ref_table */
  task_cmd_ptr->cmd.envelope_response.message_header.user_data = report_ptr->user_data;
  if(report_ptr->rpt_status == UIM_FAIL)
  {
    MSG_ERROR("UIM Rpt Status UIM FAIL",0,0,0);
  }
  else
  {
    task_cmd_ptr->cmd.envelope_response.length = report_ptr->rpt.envelope.data_length;
    memset(task_cmd_ptr->cmd.envelope_response.data, 0x00, GSTK_MAX_DATA_SIZE);
    if(report_ptr->rpt.envelope.data_length > 0) 
    { /* has data */
      MSG_HIGH("Copy 0x%x bytes of envelope rsp.",
               report_ptr->rpt.envelope.data_length,
               0,
               0);
      gstk_memcpy(task_cmd_ptr->cmd.envelope_response.data,
                  report_ptr->rpt.envelope.data,
                  report_ptr->rpt.envelope.data_length,
                  sizeof(task_cmd_ptr->cmd.envelope_response.data),
                  sizeof(report_ptr->rpt.envelope.data));
    }
  }
  /* initialize rsp_status to FAIL */
  task_cmd_ptr->cmd.envelope_response.rsp_status = GSTK_ENVELOPE_CMD_FAIL;
  /* populate sw1 and sw 2 */
  if(SW1_DLOAD_ERR == report_ptr->rpt.envelope.get_resp_sw1)
  {
    task_cmd_ptr->cmd.envelope_response.sw1 = 
      (int)report_ptr->rpt.envelope.get_resp_sw1;
    task_cmd_ptr->cmd.envelope_response.sw2 = 
      (int)report_ptr->rpt.envelope.get_resp_sw2;
  }
  else
  {
    task_cmd_ptr->cmd.envelope_response.sw1 = (int)report_ptr->sw1;
    task_cmd_ptr->cmd.envelope_response.sw2 = (int)report_ptr->sw2;
  }
  
  /* pass warning 0x62XX or 0x63XX to SMS PP DL rsp */
  type_of_command =
    envelope_cmd_ref_table[report_ptr->user_data].expected_cmd_rsp_type;
  if (type_of_command == GSTK_SMS_PP_DOWNLOAD_IND_RSP)
  {
    if (SW1_WARNINGS1 == report_ptr->rpt.envelope.get_resp_sw1 ||
        SW1_WARNINGS2 == report_ptr->rpt.envelope.get_resp_sw1)
    {
      task_cmd_ptr->cmd.envelope_response.sw1 = 
        (int)report_ptr->rpt.envelope.get_resp_sw1;
      task_cmd_ptr->cmd.envelope_response.sw2 = 
        (int)report_ptr->rpt.envelope.get_resp_sw2;
    }
  }
  
  switch(task_cmd_ptr->cmd.envelope_response.sw1)
  {
  case SW1_NORMAL_END:
    if(task_cmd_ptr->cmd.envelope_response.sw2 == (int)SW2_NORMAL_END)
    {
      task_cmd_ptr->cmd.envelope_response.rsp_status = GSTK_ENVELOPE_CMD_SUCCESS;
    }
    break;
  case SW1_BUSY:
    if(task_cmd_ptr->cmd.envelope_response.sw2 == (int)SW2_NORMAL_END)
    {
      task_cmd_ptr->cmd.envelope_response.rsp_status = GSTK_ENVELOPE_CMD_CARD_BUSY;
    }
    break;
  default:
    break;
  }

  MSG_HIGH("UIM envelope rsp queued to front of GSTK queue, 0x%x, 0x%x", 
           task_cmd_ptr->cmd.envelope_response.sw1,
           task_cmd_ptr->cmd.envelope_response.sw2,
           0);

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  gstk_task_put_cmd_buf_to_head(task_cmd_ptr);
}/*lint !e818 *report_ptr suppression for externalized function */
/* gstk_uim_envelope_report */

/*===========================================================================
FUNCTION gstk_build_error_envelope_report

DESCRIPTION
  This is the function used to build the error envelope 
report to be sent back to the client.

PARAMETERS
  report: [Input] Pointer to the UIM envelope report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_build_error_envelope_report 
(
  uint32 env_ref_id, 
  uint32                                command_id,
  gstk_general_envelope_rsp_enum_type   rsp_type
)
{
  gstk_task_cmd_type   * task_cmd;

  MSG_HIGH("Building an error envelope response",0,0,0);

  /* get command buf */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    MSG_HIGH("No more gstk buf", 0, 0, 0);
    return GSTK_ERROR;
  }
  task_cmd->cmd.envelope_response.message_header.command_group = GSTK_ENVELOPE_RSP;
  task_cmd->cmd.envelope_response.message_header.command_id = command_id;
  /* index location in the envelope_cmd_ref_table */
  task_cmd->cmd.envelope_response.message_header.user_data = env_ref_id;
  task_cmd->cmd.envelope_response.length = 0;
  memset(task_cmd->cmd.envelope_response.data, 0x00, GSTK_MAX_DATA_SIZE);

  /* initialize rsp_status to FAIL */
  task_cmd->cmd.envelope_response.rsp_status = rsp_type;

  /* populate sw1 and sw 2 */
  task_cmd->cmd.envelope_response.sw1 = 0;
  task_cmd->cmd.envelope_response.sw2 = 0;

  /* set GSTK_CMD_Q_SIG and put on GSTK queue */
  gstk_task_put_cmd_buf(task_cmd);

  return GSTK_SUCCESS;

}/* gstk_build_error_envelope_report */

/*===========================================================================
FUNCTION gstk_menu_selection_rsp

DESCRIPTION

  This function builds the menu selection rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_menu_selection_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   if(cmd == NULL)
   {
     MSG_ERROR("cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }
   MSG_HIGH(" IN GSTK_MENU_SELECTION_RSP ",0,0,0);

   /* build the command */
   response.hdr_cmd.command_id = GSTK_MENU_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;
   if(cmd->envelope_response.message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cmd->envelope_response.message_header.user_data);

   return gstk_status;

} /* gstk_menu_selection_rsp */

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

/*===========================================================================
FUNCTION gstk_evt_dl_rsp

DESCRIPTION

  This function builds the event download rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_evt_dl_rsp(
  const gstk_cmd_type * cmd)
{
    gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
    gstk_cmd_from_card_type                     response;
    gstk_cmd_enum_type                          type_of_command;
    uint32                                      env_ref_id;

    if(cmd == NULL)
    {
      MSG_ERROR("cmd ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }

    /* the envelope response is enum, so there is no need to initialize
       response */

    MSG_HIGH(" IN GSTK_EVT_DL_RSP ",0,0,0);
    env_ref_id = cmd->envelope_response.message_header.user_data;
    if(env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND) {
      MSG_ERROR("Recd bad env ref id, %d", env_ref_id, 0, 0);
      gstk_status = GSTK_ERROR;
      return gstk_status;
    }
    response.hdr_cmd.user_data =
       envelope_cmd_ref_table[env_ref_id].user_data;

    /* Get the type of response */
    type_of_command = envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type;

    /* if retry envelope is successful , clear the global pending envelope structure */
    if((env_ref_id == gstk_curr_envelope_info.env_ref_id) && 
       (gstk_curr_envelope_info.gstk_env_state == GSTK_SUB_ST_PENDING_EVT_DL_RSP_FROM_CARD))
    {
      /* Check envelope response status words received from the card */
      if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_SUCCESS)
        gstk_util_clear_retry_envelope_data();
      else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY)
      {
        if(gstk_curr_envelope_info.retry_info.retry_counter < 
           gstk_curr_envelope_info.retry_info.max_no_of_retries)
        {
          /* Change state to pending retry */
          gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_EVT_DL_IN_QUEUE;
          /* start the timer now */
          (void)rex_set_timer(&(gstk_curr_envelope_info.gstk_env_retry_timer), 
                              GSTK_ENV_RETRY_CMD_WAIT_TIME);
          return gstk_status;
        }
        else
        {
          MSG_HIGH("Reached Max number of retries for envelope",0,0,0);
          gstk_util_clear_retry_envelope_data();
        }
      }/* else if (cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_CARD_BUSY) */
      else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL)
      {
        MSG_HIGH("Envelope command response FAIL",0,0,0);
        gstk_util_clear_retry_envelope_data();
      } /* else if(cmd->envelope_response.rsp_status == GSTK_ENVELOPE_CMD_FAIL) */
    }
    
    switch( type_of_command ) {
    case GSTK_CALL_CONNECTED_EVT_IND_RSP:
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      if (gstk_util_find_first_non_timer_expire_postpone_env_command() 
           != GSTK_MAX_PROACTIVE_COMMAND) {
         /* Set the signal to send the envelope */
         (void)rex_set_sigs(&gstk_tcb,  GSTK_SEND_POSTPONE_ENVELOPE_SIG);
      }
      break;
    case GSTK_MT_CALL_EVT_IND_RSP:
    case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
    case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
    case GSTK_DATA_AVAIL_EVT_IND_RSP:
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      break;
    case GSTK_CH_STATUS_EVT_IND_RSP:
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      break;
    case GSTK_LANG_SELECTION_EVT_IND_RSP:
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      break; /* ignore, doesn't have to sent back! */
    case GSTK_ACCESS_TECH_EVT_IND_RSP:
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      break;
    case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
#ifdef FEATURE_CAT_REL6
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
#else
      MSG_HIGH("GSTK_NW_SEARCH_MODE_EVT_IND_RSP : FEATURE_CAT_REL6 not supported",0,0,0);
#endif
      break;
    case GSTK_LOCATION_STATUS_EVT_IND_RSP:
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      gstk_pending_location_evt_envelope = FALSE;
      if((gstk_evt_list[GSTK_LOCATION_STATUS] & 0x01) == 0x01) { /* has registered for this event */
        if (gstk_util_compare_curr_and_next_location_info())
        {
          gstk_evt_dl_ind_type            evt_list;
          /* there are location info changes that cause the MM to enter idle */
          /* update the gstk static values */
          MSG_HIGH("There are Pending Location Status", 0, 0, 0);
          memcpy(gstk_curr_mcc_mnc, gstk_next_curr_mcc_mnc, 3);
          memcpy(gstk_curr_lac, gstk_next_curr_lac, 2);
          memcpy(&gstk_curr_cell_id, &gstk_next_curr_cell_id, sizeof(gstk_cell_id_type));
          gstk_curr_location_status = gstk_next_curr_location_status;
          /* Send an envelope to the card */
          evt_list.evt_type = GSTK_LOCATION_STATUS_EVT_IND;
          gstk_status = gstk_send_envelope_evt_download_command(
                             GSTK_CLIENT_ID,
                             0,
                             &evt_list);
          if (gstk_status != GSTK_SUCCESS) {
            ERR("Have problem sending location status envelope", 0, 0, 0);
          }
          else {
            gstk_pending_location_evt_envelope = TRUE;
          }
        }
      }
      break; /* ignore, doesn't have to sent back! */

      /* Send back to user */
    case GSTK_IDLE_SCRN_EVT_IND_RSP:
      /* build the command */
      response.hdr_cmd.command_id = type_of_command;
      response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
      response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;

      gstk_status = gstk_util_send_response_to_clients(&response,
       cmd->envelope_response.message_header.user_data);

      /* remove the bitmask */
      gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE]= 0x00;

      /* notify client to remove their mask */
      response.hdr_cmd.command_id = GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY;
      response.cmd.evt_notify_req.require_notification = FALSE;
      gstk_status = gstk_util_send_message_to_clients(&response);
      break;

    case GSTK_USER_ACTIVITY_EVT_IND_RSP:
      /* build the command */
      response.hdr_cmd.command_id = type_of_command;
      response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
      response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;

      gstk_status = gstk_util_send_response_to_clients(&response,
       cmd->envelope_response.message_header.user_data);

      /* remove the bitmask */
      gstk_evt_list[GSTK_USER_ACTIVITY]= 0x00;

      /* notify client to remove their mask */
      response.hdr_cmd.command_id = GSTK_SETUP_EVT_USER_ACT_NOTIFY;
      response.cmd.evt_notify_req.require_notification = FALSE;
      gstk_status = gstk_util_send_message_to_clients(&response);
      break;

    default:
      /* do nothing */
      MSG_ERROR("In Evt DL: unknown type of event 0x%x", type_of_command, 0, 0);
      break;
    }
    return gstk_status;

} /* gstk_evt_dl_rsp */

/*===========================================================================
FUNCTION gstk_sms_pp_dl_rsp

DESCRIPTION

  This function builds the sms pp download rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_sms_pp_dl_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   MSG_HIGH(" IN GSTK_SMS_PP_DL_RSP ",0,0,0);

   if(cmd == NULL)
   {
     MSG_ERROR("cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize response */
   memset(&response, 0, sizeof(gstk_sms_pp_download_rsp_type)+sizeof(gstk_exp_hdr_type));

   /* build the command */
   response.hdr_cmd.command_id = GSTK_SMS_PP_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   if(cmd->envelope_response.message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
   }
   response.cmd.sms_pp_download_rsp.general_resp = cmd->envelope_response.rsp_status;
   if(cmd->envelope_response.length > 0) { /* reponse data present */
     MSG_HIGH("In SMS PP: There are extra data len: 0x%x", cmd->envelope_response.length, 0, 0);
     response.cmd.sms_pp_download_rsp.response_data_present = TRUE;
     if (cmd->envelope_response.length < GSTK_MAX_DATA_SIZE)
     {     
       response.cmd.sms_pp_download_rsp.ack.ack =
         gstk_malloc(cmd->envelope_response.length);
       if(response.cmd.sms_pp_download_rsp.ack.ack != NULL) {
         response.cmd.sms_pp_download_rsp.ack.length = (uint8)cmd->envelope_response.length;
         memcpy(response.cmd.sms_pp_download_rsp.ack.ack,
           cmd->envelope_response.data,
           cmd->envelope_response.length);
       }
       else {
         MSG_ERROR("can't allocate sms pp.ack ", 0, 0, 0);
         return GSTK_MEMORY_ERROR;
       }
     }
     else
     {
       MSG_ERROR("cmd->envelope_response.length (0x%x) is too long (>= 0x%x)!",
                 cmd->envelope_response.length, GSTK_MAX_DATA_SIZE, 0);
       return GSTK_BAD_PARAM;
     }
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cmd->envelope_response.message_header.user_data);

   return gstk_status;

} /* gstk_sms_pp_dl_rsp */

/*===========================================================================
FUNCTION gstk_check_valid_tag_for_cc_rsp

DESCRIPTION

  This function if the tag [input] is a valid tag for call control envelope
  response

PARAMETERS
  sim_tlv_tag    : [Input] tag
  curr_tag_level : [input] current postion of the tag in the list of tlvs

DEPENDENCIES
  None

RETURN VALUE
  int32 - postion that the tag should have in the list of tlvs

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int32 gstk_check_valid_tag_for_cc_rsp(
 uint8              sim_tlv_tag,
 int32              curr_tag_level)
{
    if (sim_tlv_tag > 0x80) 
    { /* tag with CR */
      sim_tlv_tag -= 0x80;
    }

    switch(sim_tlv_tag)
    {
    case GSTK_ADDRESS_TAG:
    case GSTK_SS_STRING_TAG:
    case GSTK_USSD_STRING_TAG:
    case GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_1)
        return GSTK_TAG_LEVEL_1;
      break;
    case GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_2)
        return GSTK_TAG_LEVEL_2;
      else if(curr_tag_level <  GSTK_TAG_LEVEL_6)
        return GSTK_TAG_LEVEL_6;
      break;
    case GSTK_CALLED_PARTY_SUB_ADDRESS_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_3)
        return GSTK_TAG_LEVEL_3;
      break;
    case GSTK_ALPHA_IDENTIFIER_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_4)
        return GSTK_TAG_LEVEL_4;
      break;
    case GSTK_BC_REPEAT_INDICATOR_TAG:
      if(curr_tag_level <  GSTK_TAG_LEVEL_5)
        return GSTK_TAG_LEVEL_5;
      break;
    default:
      break;
    }
    return 0;
}

/*===========================================================================
FUNCTION gstk_is_tag_valid_for_command

DESCRIPTION

  This function checks the validity of the tag for the command specified

PARAMETERS
  cmd_type:          [Input] command type
  sim_tlv_tag:       [Input] tag
  tlv_tag_level_ptr: [Input] ptr to the tag level
  curr_tag_level:    [Input] current tag postion in the tlv

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE  - if tag is valid
    FALSE - if tag is not valid

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static boolean gstk_is_tag_valid_for_command(
 gstk_cmd_enum_type cmd_type,
 uint8              sim_tlv_tag,
 int32              *tlv_tag_level_ptr,
 int32              curr_tag_level)
{

  if(tlv_tag_level_ptr == NULL)
  {
    MSG_HIGH("tlv_tag_level_ptr NULL", 0,0, 0);
    return FALSE;
  }

  switch(cmd_type)
  {
  case GSTK_CC_IND_RSP:
    *tlv_tag_level_ptr = gstk_check_valid_tag_for_cc_rsp(sim_tlv_tag, curr_tag_level);
    if(*tlv_tag_level_ptr != 0)
    {
      return TRUE;
    }
    break;
  default:
    break;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION gstk_cc_rsp

DESCRIPTION

  This function builds the call control rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_cc_rsp(
  gstk_cmd_type * cmd)
{
  gstk_status_enum_type                       gstk_status          = GSTK_SUCCESS;
  gstk_cmd_from_card_type                     response;
  int32                                       curr_offset          = 0;
  boolean                                     more_tlvs            = TRUE;
  int32                                       new_offset           = 0;
  boolean                                     error_already_raised = FALSE;
  boolean                                     voice_call           = FALSE;
  int32                                       tlv_tag_level        = 0;
  int32                                       curr_tag_level       = 0;
  boolean                                     parsing_done         = FALSE;
  boolean                                     comprehension_flag   = FALSE;


  MSG_HIGH(" IN GSTK_CC_RSP ",0,0,0);

  if(cmd == NULL)
  {
    MSG_ERROR("cmd ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }
  /* initialize response */
  memset(&response, 0, sizeof(gstk_cc_rsp_type)+sizeof(gstk_exp_hdr_type));

  /* build the command */
  response.hdr_cmd.command_id           = GSTK_CC_IND_RSP;
  response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
  if(cmd->envelope_response.message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
    response.hdr_cmd.user_data =
      envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
  }
  response.cmd.cc_rsp.general_resp = cmd->envelope_response.rsp_status;

  /* Check for sw 1 = 6F, sw 2 = 00 || C2 */
  if ((cmd->envelope_response.sw1 == 0x6F) && 
      (cmd->envelope_response.sw2 == 0x00 || cmd->envelope_response.sw2 == 0xC2)) {
    /* treat as success */
    MSG_HIGH("Recv 0x6F%x for CC, changed to SUCCESS, No MOD", cmd->envelope_response.sw2, 0, 0);
    response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_SUCCESS;
    response.cmd.cc_rsp.call_ctrl_result = GSTK_ALLOWED_NO_MOD;
  }
  else {
    /* SW != 6F00, proceed with normal parsing */
    if (response.cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS) {
      if(cmd->envelope_response.length > 0) { /* response data present */
        response.cmd.cc_rsp.call_ctrl_result = (gstk_control_result_enum_type)cmd->envelope_response.data[0];
        switch(response.cmd.cc_rsp.call_ctrl_result) {
        case GSTK_ALLOWED_NO_MOD:
          break;
        case GSTK_NOT_ALLOWED:
          break;
        case GSTK_ALLOWED_BUT_MOD:
          if(cmd->envelope_response.data[1] == 0) { /* error */
            MSG_ERROR("In CC: Allowed but Mod with no more data ", 0, 0, 0);
            response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
          }
          break;
        case GSTK_CONTROL_RESULT_MAX:
          break;
        }/*  switch(response.cmd.cc_rsp.call_ctrl_result) */

        if(response.cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS) {
          response.cmd.cc_rsp.cc_req_action.call_type = GSTK_NONE;
          response.cmd.cc_rsp.cc_req_action.alpha.length = 0;
          /* success and length and has more data */
          if(cmd->envelope_response.data[1] > 0) {

            /* Current offset starts at 2, as call control result is byte 0 and 
               length is byte 1  */

            curr_offset = 2;

            while(!parsing_done && !error_already_raised)
            {
              /* Check for tag validity */
              if(FALSE == gstk_is_tag_valid_for_command(
                            GSTK_CC_IND_RSP,
                            cmd->envelope_response.data[curr_offset],
                            &tlv_tag_level,
                            curr_tag_level))
              {
                /* check for tag comprehension required flag */
                comprehension_flag = gstk_is_tag_comprehension_req(cmd->envelope_response.data[curr_offset]);
                if(comprehension_flag)
                {
                    response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
                    break; /* break out of the while loop */
                }

                if(!more_tlvs)
                  parsing_done = TRUE;

                /* curr_offset+1 if valid will hold the tag value of the next tlv */    
                if(curr_offset+1 <= cmd->envelope_response.data[1] + 2)
                {
                  curr_offset += cmd->envelope_response.data[curr_offset + 1] + GSTK_TAG_LENGTH_LEN;
                }
                else
                {
                  break; /* break out of the while loop */
                }

                /* ignore the current tlv and get the next tlv */
                gstk_status = gstk_parser_get_next_tlv_offset(
                  cmd->envelope_response.data[1] + 2, curr_offset, &new_offset, &more_tlvs);

                curr_offset = new_offset;

                if(gstk_status != GSTK_SUCCESS)
                {
                  response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
                  break; /* break out of the while loop */
                }
                continue; /* continue with the next iteration and check for new tag validity */

              } /* tag validity check */

              switch(tlv_tag_level)
              {
              case GSTK_TAG_LEVEL_1:
              {
                /* Parse Address String */
                new_offset = curr_offset;
                gstk_status = gstk_parser_address_ss_string_tlv(
                  GSTK_ADDRESS_TAG,
                  cmd->envelope_response.data,
                  cmd->envelope_response.data[1] + 2, /* data length */
                  curr_offset,
                  &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.address),
                  &new_offset,
                  &more_tlvs,
                  OPTIONAL,
                  NULL);

                if(gstk_status != GSTK_SUCCESS)
                {
                  MSG_ERROR("In CC: Parse Address Fail %x", gstk_status, 0, 0);
                  error_already_raised = TRUE;
                }
                else
                {
                  if(new_offset == curr_offset)
                  {
                    /* Seems that the offset has not moved */
                    MSG_HIGH("NOT a GSTK_ADDRESS_STRING_TAG",0,0,0);
                  }
                  else
                  {
                    response.cmd.cc_rsp.cc_req_action.call_type = GSTK_VOICE;
                    curr_offset = new_offset;
                  }
                }

                /* Parse SS String */

                if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_VOICE && !error_already_raised)
                {
                  /* check to see if there is SS/USSD */
                  new_offset = curr_offset;
                  gstk_status = gstk_parser_address_ss_string_tlv(
                    GSTK_SS_STRING_TAG,
                    cmd->envelope_response.data,
                    cmd->envelope_response.data[1] + 2, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.ss_string),
                    &new_offset,
                    &more_tlvs,
                    OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    MSG_ERROR("In CC: Parse SS String Fail %x", gstk_status, 0, 0);
                      error_already_raised = TRUE;
                  }
                  else
                  {
                    if(new_offset == curr_offset)
                    {
                      /* Seems that the offset has not moved */
                      MSG_HIGH("NOT a GSTK_SS_STRING_TAG",0,0,0);
                    }
                    else
                    {
                      response.cmd.cc_rsp.cc_req_action.call_type = GSTK_SS;
                      curr_offset = new_offset;
                    }
                  }

                  /* Parse USSD String */
                  if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_SS && !error_already_raised ) {
                    /* check to see if there is a ussd */
                    new_offset = curr_offset;
                    gstk_status = gstk_parser_ussd_string_tlv(
                      cmd->envelope_response.data,
                      cmd->envelope_response.data[1] + 2, /* data length */
                      curr_offset,
                      &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string),
                      &new_offset,
                      &more_tlvs,
                      OPTIONAL,
                      NULL);

                    if(gstk_status != GSTK_SUCCESS) {
                      MSG_ERROR("In CC: Parse USSD Fail %x", gstk_status, 0, 0);
                      error_already_raised = TRUE;
                    }
                    else
                    {
                      if(new_offset == curr_offset)
                      {
                        /* Seems that the offset has not moved */
                        MSG_HIGH("NOT a GSTK_USSD_STRING_TAG",0,0,0);
                      }
                      else
                      {
                        response.cmd.cc_rsp.cc_req_action.call_type = GSTK_USSD;
                        curr_offset = new_offset;
                      }
                    }

                    /* Parse PDP Context Act Param  */
#ifdef FEATURE_GPRS_CALLCONTROL
#error code not present
#endif /* #ifdef FEATURE_GPRS_CALLCONTROL */
                  } /* no SS string either, try ussd */
                } /* check if there is SS/USSD */

                /* At this point, either we have address/ss/ussd or we haven't got any of that info yet */
                if(response.cmd.cc_rsp.cc_req_action.call_type != GSTK_SS &&
                   response.cmd.cc_rsp.cc_req_action.call_type != GSTK_USSD
#ifdef FEATURE_GPRS_CALLCONTROL
#error code not present
#endif /* #ifdef FEATURE_NO_GPRS_CALLCONTROL */
                   && (!error_already_raised))
                {
                   voice_call = TRUE;
                }
                break;
              }

              case GSTK_TAG_LEVEL_2:
              {
                if(!error_already_raised && more_tlvs && voice_call) {
                  /* initialize all other optional to zero length */
                  response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp1.length = 0;
                  response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp2.length = 0;
                  response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.subaddress.length = 0;
                  /* ccp1? */
                  gstk_status = gstk_parser_cap_config_tlv(
                    cmd->envelope_response.data,
                    cmd->envelope_response.data[1] + 2, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp1),
                    &new_offset,
                    &more_tlvs,
                    OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    MSG_ERROR("In CC: Parse CCP1 Fail %x", gstk_status, 0, 0);
                    error_already_raised = TRUE;
                  }
                  else
                  {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_3:
              {
                if(!error_already_raised && more_tlvs && voice_call)
                {/* more tlv after ccp1 parsing attempt */
                /* subaddress? */
                  gstk_status = gstk_parser_subaddress_tlv(
                    cmd->envelope_response.data,
                    cmd->envelope_response.data[1] + 2, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.subaddress),
                    &new_offset,
                    &more_tlvs,
                    OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    MSG_ERROR("In CC: Parse SubAddr Fail %x", gstk_status, 0, 0);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_4:
              {
                if(!error_already_raised && more_tlvs) {/* more tlv after subaddress parsing attempt */
                  /* alpha? */
                  gstk_status = gstk_parser_alpha_identifier_tlv(
                    cmd->envelope_response.data,
                    cmd->envelope_response.data[1] + 2, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.alpha),
                    &new_offset,
                    &more_tlvs,
                    OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                    MSG_ERROR("In CC: Parse Alpha Fail %x", gstk_status, 0, 0);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_5:
              {
                if(!error_already_raised && more_tlvs && voice_call) {/* more tlv after alpha parsing attempt */
                  /* bc repeat indicator */
                  gstk_status = gstk_parser_bc_repeat_ind_tlv(
                    cmd->envelope_response.data,
                    cmd->envelope_response.data[1] + 2, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.bc_repeat_ind),
                    &new_offset,
                    &more_tlvs,
                    OPTIONAL,
                    NULL);

                  if ((gstk_status != GSTK_SUCCESS) || (!more_tlvs)) {
                    /* bc repeat indicator always has ccp2 */
                    MSG_ERROR("In CC: Parse BC Repeat Fail %x", gstk_status, 0, 0);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                }
                break;
              }

              case GSTK_TAG_LEVEL_6:
              {
                if(!error_already_raised && more_tlvs && voice_call) { /* more tlv after bc repeat indicator parsing attempt */
                  /* ccp2 indicator */
                  gstk_status = gstk_parser_cap_config_tlv(
                    cmd->envelope_response.data,
                    cmd->envelope_response.data[1] + 2, /* data length */
                    curr_offset,
                    &(response.cmd.cc_rsp.cc_req_action.gstk_address_string.voice_info.ccp2),
                    &new_offset,
                    &more_tlvs,
                    OPTIONAL,
                    NULL);

                  if(gstk_status != GSTK_SUCCESS || !more_tlvs) {
                    MSG_ERROR("In CC: Parse CCP2 Fail %x", gstk_status, 0, 0);
                    error_already_raised = TRUE;
                  }
                  else {
                    curr_offset = new_offset;
                  }
                } /* more tlv after bc repeat indicator parsing */
                break;
              }

              default:
                break;
              }/* switch(tlv_tag_level) */
              curr_tag_level = tlv_tag_level;
              if(!more_tlvs)
                parsing_done = TRUE;
             }/*  while(more_tlvs && !error_already_raised) */
          } /* more data cmd[1] > 0 */
        } /* envelope cmd success */
        if(error_already_raised) {
          response.cmd.cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
        }
      } /* cmd_data[0] > 0 */
      else { /* response length == 0 */
        MSG_HIGH("Resp Len = 0 in CC", 0, 0, 0);
        response.cmd.cc_rsp.call_ctrl_result = GSTK_ALLOWED_NO_MOD;
      }
    } /* general resp = SUCCESS */
  }

  gstk_status = gstk_util_send_response_to_clients(&response,
    cmd->envelope_response.message_header.user_data);

  return gstk_status;

} /* gstk_cc_rsp */

/*===========================================================================
FUNCTION gstk_mo_sms_ctrl_rsp

DESCRIPTION

  This function builds the sms control download rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_mo_sms_ctrl_rsp(
  gstk_cmd_type * cmd)
{
  gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
  gstk_cmd_from_card_type                     response;
  int32                                       offset = 0;
  int32                                       new_offset = 0;
  boolean                                     more_tlv = TRUE;
  boolean                                     error_already_raised = FALSE;

  MSG_HIGH(" IN GSTK_MO_SMS_CTRL_RSP ",0,0,0);

  if(cmd == NULL)
  {
    MSG_ERROR("cmd ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize response */
  memset(&response, 0, sizeof(gstk_mo_sms_ctrl_rsp_type)+sizeof(gstk_exp_hdr_type));

  /* build the command */
  response.hdr_cmd.command_id = GSTK_MO_SMS_CTRL_IND_RSP;
  response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
  if(cmd->envelope_response.message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
    response.hdr_cmd.user_data =
      envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
  }
  response.cmd.mo_sms_cc_rsp.general_resp = cmd->envelope_response.rsp_status;

  /* Check for sw 1 = 6F, sw 2 = 00 */
  if ((cmd->envelope_response.sw1 == 0x6F) && (cmd->envelope_response.sw2 == 0x00)) {
    /* treat as success */
    MSG_HIGH("Recv 0x6F00 for MO SMS Ctrl, changed to SUCCESS, No MOD", 0, 0, 0);
    response.cmd.mo_sms_cc_rsp.general_resp = GSTK_ENVELOPE_CMD_SUCCESS;
    response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = GSTK_ALLOWED_NO_MOD;
  }
  else {
    if (response.cmd.mo_sms_cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS) {
      if(cmd->envelope_response.length > 0) { /* response data present */
        response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = (gstk_control_result_enum_type)cmd->envelope_response.data[0];
        if(cmd->envelope_response.data[1] > 0) {
          offset = 2;
          if(gstk_valid_tag(cmd->envelope_response.data[offset], GSTK_ADDRESS_TAG)) {
            gstk_status = gstk_parser_address_ss_string_tlv(
                      GSTK_ADDRESS_TAG,
                      cmd->envelope_response.data,
                      cmd->envelope_response.data[1] + 2, /* data length */
                      offset,
                      &(response.cmd.mo_sms_cc_rsp.rp_dest_address),
                      &new_offset,
                      &more_tlv,
                      OPTIONAL,
                      NULL);

            if(gstk_status != GSTK_SUCCESS || !more_tlv) {
              /* with rp address, there has to be tp address */
              MSG_ERROR("In MO SMS Ctrl: Parse RP Addr Fail %x", gstk_status, 0, 0);
              error_already_raised = TRUE;
            }
            else {
              offset = new_offset;
            }

            if(!error_already_raised && more_tlv) {
              /* check tp address */
              if (offset > GSTK_MAX_DATA_SIZE - 1) {
                MSG_ERROR("In MO SMS Ctrl: Invalid data index: %d, max index allowed: %d", 
                          offset, GSTK_MAX_DATA_SIZE - 1,0);
                error_already_raised = TRUE;
              }
              else {
                if(gstk_valid_tag(cmd->envelope_response.data[offset], 
                                  GSTK_ADDRESS_TAG)) {
                  gstk_status = gstk_parser_address_ss_string_tlv(
                                  GSTK_ADDRESS_TAG,
                                  cmd->envelope_response.data,
                                  cmd->envelope_response.data[1] + 2, /* data length */
                                  offset,
                                  &(response.cmd.mo_sms_cc_rsp.tp_dest_address),
                                  &new_offset,
                                  &more_tlv,
                                  OPTIONAL,
                                  NULL);

                  if(gstk_status != GSTK_SUCCESS) {
                     MSG_ERROR("In MO SMS Ctrl: Parse TP Addr Fail %x", gstk_status, 0, 0);
                     error_already_raised = TRUE;
                  }
                  else {
                    offset = new_offset;
                  }
                } /* has tp address */
                else { /* error! can't have just rp and no tp address */
                  error_already_raised = TRUE;
                } 
              } /* offset is okay */
            }  /* no error raised and more tlv */
            else { /* error already raised from rp parsing or only rp address! */
              error_already_raised = TRUE;
            }
          } /* has rp address */
          /* check for alpha */
          if(!error_already_raised && more_tlv) {
            if (offset > GSTK_MAX_DATA_SIZE - 1) {
              MSG_ERROR("In MO SMS Ctrl: Invalid data index: %d, max index allowed: %d", 
                        offset, GSTK_MAX_DATA_SIZE - 1,0);
              error_already_raised = TRUE;
            }
            else {
              if(gstk_valid_tag(cmd->envelope_response.data[offset], 
                                GSTK_ALPHA_IDENTIFIER_TAG)) {
                /* there is alpha */
                gstk_status = gstk_parser_alpha_identifier_tlv(cmd->envelope_response.data,
                                                  cmd->envelope_response.data[1] + 2, /* data length */
                                                  offset,
                                                  &(response.cmd.mo_sms_cc_rsp.alpha),
                                                  &new_offset,
                                                  &more_tlv,
                                                  OPTIONAL,
                                                  NULL);

                if (gstk_status != GSTK_SUCCESS || more_tlv) { /* there shouldn't be any more tlvs */
                  MSG_ERROR("In MO SMS Ctrl: Parse Alpha Fail %x", gstk_status, 0, 0);
                  error_already_raised = TRUE;
                }
                else {
                  offset = new_offset;
                }

              } /* has alpha */
              else { /* error!, since alpha is the last possible tlv */
                error_already_raised = TRUE;
              }
            } /* offset is okay */
            
          } /* no error raised from rp and tp checking and there is  more tlv */
        } /* if data[1] > 0 */
        if (!error_already_raised) {
          if (response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result == GSTK_ALLOWED_BUT_MOD) {
            if (response.cmd.mo_sms_cc_rsp.rp_dest_address.length == 0 ||
                  response.cmd.mo_sms_cc_rsp.tp_dest_address.length == 0) {
              MSG_ERROR("In MO SMS Ctrl: GSTK_ALLOWED_BUT_MOD with no data => Fail ", 0, 0, 0);
              response.cmd.mo_sms_cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
            }
          }
        }
      } /* data is > 0 */
      else { /* no response data, -> ALLOW NO MOD */
        MSG_ERROR("In MO SMS Ctrl: No response data when gen rsp = SUCCESS ", 0, 0, 0);
        response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = GSTK_ALLOWED_NO_MOD;
      }
      if(error_already_raised) {
        response.cmd.mo_sms_cc_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
      }
    } /* envelope command success */
  }

  gstk_status = gstk_util_send_response_to_clients(&response,
    cmd->envelope_response.message_header.user_data);

  return gstk_status;

} /* gstk_mo_sms_ctrl_rsp */

/*===========================================================================
FUNCTION gstk_timer_expire_rsp

DESCRIPTION

  This function builds the timer expiry rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_timer_expire_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   uint32                                      env_ref_id;
   uint8                                       timer_table_index;

   /* the envelope response is enum, so there is no need to initialize
   response */

   MSG_HIGH(" IN GSTK_TIMER_EXPIRE_RSP ",0,0,0);

   if(cmd == NULL)
   {
     MSG_ERROR("cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   env_ref_id = cmd->envelope_response.message_header.user_data;
   if(env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND) {
     MSG_ERROR("Recd bad env ref id, %d", env_ref_id, 0, 0);
     gstk_status = GSTK_ERROR;
     return gstk_status;
   }

   /* re-schedule next time expire envelope command if the response from card
      is busy.
      the next envelope cmd for the timer expiry envelope will be sent to card
      when the next successful terminal response is received from the card */

   /* reset the entry in the timer management table */
   timer_table_index = uint32touint8(envelope_cmd_ref_table[env_ref_id].user_data);
   if (timer_table_index >= GSTK_MAX_TIMER_SUPPORTED) {
     MSG_ERROR("Invalid Timer Table Index %d", timer_table_index, 0, 0);
   }
   else {
     switch (cmd->envelope_response.rsp_status)
     {
     case GSTK_ENVELOPE_CMD_CARD_BUSY:
        MSG_HIGH("SIM Busy, Timer Expire Retry later", 0, 0, 0);
        gstk_timer_management[timer_table_index].waiting_rsp = FALSE;
        break;
  
     case GSTK_ENVELOPE_CMD_SUCCESS:
        gstk_timer_management[timer_table_index].timer_started = FALSE;
        gstk_timer_management[timer_table_index].pending = FALSE;
        gstk_timer_management[timer_table_index].time = 0;
        (void)rex_clr_timer( &(gstk_timer_management[timer_table_index].timer_elapsed));
        gstk_timer_management[timer_table_index].waiting_rsp = FALSE;
        break;

     default:            
        MSG_ERROR("Timer Expire Envelope Err", 0, 0, 0);
        gstk_timer_management[timer_table_index].timer_started = FALSE;
        gstk_timer_management[timer_table_index].pending = FALSE;
        gstk_timer_management[timer_table_index].time = 0;
        (void)rex_clr_timer( &(gstk_timer_management[timer_table_index].timer_elapsed));
        gstk_timer_management[timer_table_index].waiting_rsp = FALSE;
        break;
     }
   }

   /* free the env cmd ref table */
   gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
   return gstk_status;

} /* gstk_timer_expire_rsp */

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_rsp

DESCRIPTION

  This function builds the cell broadcast rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_cell_broadcast_dl_rsp(
  const gstk_cmd_type * cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   MSG_HIGH(" IN GSTK_CELL_BROADCAST_DL_RSP ",0,0,0);

   if(cmd == NULL)
   {
     MSG_ERROR("cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* build the command */
   response.hdr_cmd.command_id = GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   response.cmd.envelope_general_rsp = cmd->envelope_response.rsp_status;
   if(cmd->envelope_response.message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       envelope_cmd_ref_table[cmd->envelope_response.message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     cmd->envelope_response.message_header.user_data);

   return gstk_status;

} /* gstk_cell_broadcast_dl_rsp */

/*===========================================================================
FUNCTION gstk_process_envelope_rsp

DESCRIPTION

  This function performs a lookup on the command id with the
  envelope_cmd_ref_table and based on the envelope rsp type, it calls the
  corresponding envelope rsp function so that GSTK can build the corrresponding
  envelope response and sends it to the client

PARAMETERS
  cmd_ptr: [Input] Pointer to the envelope response types to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_process_envelope_rsp(
    gstk_cmd_type* cmd_ptr )
{
    gstk_status_enum_type    gstk_status    = GSTK_SUCCESS;
    gstk_cmd_enum_type       type_of_command= GSTK_CMD_ENUM_MAX;
    uint32                   env_ref_id     = GSTK_MAX_PROACTIVE_COMMAND;

    if(cmd_ptr == NULL)
    {
      MSG_ERROR("cmd_ptr ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }
    env_ref_id = cmd_ptr->envelope_response.message_header.user_data;

    if (env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND)
    {
      MSG_ERROR("envelope ref id (0x%x) is invalid!",
                cmd_ptr->envelope_response.message_header.user_data,
                0,
                0);
      return GSTK_BAD_PARAM;
    }

    /* Get the type of response */
    type_of_command = envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type;

    switch( type_of_command ) {
    case GSTK_MENU_IND_RSP:
      gstk_status = gstk_menu_selection_rsp(cmd_ptr);
      break;
    case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
      gstk_status = gstk_sms_pp_dl_rsp(cmd_ptr);
      break;
    case GSTK_MT_CALL_EVT_IND_RSP:
    case GSTK_CALL_CONNECTED_EVT_IND_RSP:
    case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
    case GSTK_IDLE_SCRN_EVT_IND_RSP:
    case GSTK_LOCATION_STATUS_EVT_IND_RSP:
    case GSTK_USER_ACTIVITY_EVT_IND_RSP:
    case GSTK_LANG_SELECTION_EVT_IND_RSP:
    case GSTK_ACCESS_TECH_EVT_IND_RSP:
    case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
    case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
    case GSTK_DATA_AVAIL_EVT_IND_RSP:
    case GSTK_CH_STATUS_EVT_IND_RSP:
      gstk_status = gstk_evt_dl_rsp(cmd_ptr);
      break;
    case GSTK_MO_SMS_CTRL_IND_RSP:
      gstk_status = gstk_mo_sms_ctrl_rsp(cmd_ptr);
      break;
    case GSTK_CC_IND_RSP:
      gstk_status = gstk_cc_rsp(cmd_ptr);
      break;
    case GSTK_TIMER_EXPIRE_IND_RSP:
      gstk_status = gstk_timer_expire_rsp(cmd_ptr);
      break;
    case GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP:
      gstk_status = gstk_cell_broadcast_dl_rsp(cmd_ptr);
      break;
#if defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT)
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
    default:
      MSG_ERROR("In Env Rsp Process, unknown cmd type 0x%x", type_of_command, 0, 0);
      gstk_status = GSTK_ERROR;
      break;
    }/* End of switch */
    return gstk_status;
} /* gstk_process_envelope_rsp*/

/*===========================================================================
FUNCTION gstk_non_idle_st_menu_selection_err_rsp

DESCRIPTION

  This function builds the menu selection error rsp in non idle state
  and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
  GSTK_BAD_PARAM,
  GSTK_SUCCESS

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_menu_selection_err_rsp(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   /* the envelope response is enum, so there is no need to initialize
   response */

   MSG_HIGH(" IN GSTK_NON_IDLE_ST_MENU_SELECITON_ERR_RSP ",0,0,0);

   if(menu_sel_cmd == NULL)
   {
     MSG_ERROR("menu_sel_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_MENU_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
   response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;
   if(menu_sel_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       envelope_cmd_ref_table[menu_sel_cmd->message_header.user_data].user_data;
   }

   gstk_status = gstk_util_send_response_to_clients(&response,
     menu_sel_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_menu_selection_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_evt_dl_err_rsp

DESCRIPTION

  This function builds the event download rsp error in non idle state
  and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_evt_dl_err_rsp(
  const gstk_cmd_type * cmd)
{
    gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
    gstk_cmd_from_card_type                     response;
    gstk_cmd_enum_type                          type_of_command;
    uint32                                      env_ref_id;

    /* the envelope response is enum, so there is no need to initialize
       response */

    MSG_HIGH(" IN GSTK_NON_IDLE_ST_EVT_DL_ERR_RSP ",0,0,0);

    if(cmd == NULL)
    {
      MSG_ERROR("cmd ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }

    env_ref_id = cmd->call_connected_envelope_ind.message_header.user_data;
    if(env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND) {
      MSG_ERROR("Recd bad env ref id, %d", env_ref_id, 0, 0);
      gstk_status = GSTK_ERROR;
      return gstk_status;
    }
    response.hdr_cmd.user_data =
       envelope_cmd_ref_table[env_ref_id].user_data;

    /* Get the type of response */
    type_of_command = envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type;

    switch( type_of_command ) {

    case GSTK_MT_CALL_EVT_IND_RSP:
    case GSTK_CALL_CONNECTED_EVT_IND_RSP:
    case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
    case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
    case GSTK_LANG_SELECTION_EVT_IND_RSP:
    case GSTK_LOCATION_STATUS_EVT_IND_RSP:
    case GSTK_DATA_AVAIL_EVT_IND_RSP:
    case GSTK_CH_STATUS_EVT_IND_RSP:
    case GSTK_ACCESS_TECH_EVT_IND_RSP:    
    case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
      gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
      break; /* ignore, doesn't have to sent back! */
      /* Send back to user */
    case GSTK_IDLE_SCRN_EVT_IND_RSP:
      /* build the command */
      response.hdr_cmd.command_id           = type_of_command;
      response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
      response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;

      gstk_status = gstk_util_send_response_to_clients(&response,
        env_ref_id);

      /* remove the bitmask */
      gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE]= 0x00;

      /* notify client to remove their mask */
      response.hdr_cmd.command_id = GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY;
      response.cmd.evt_notify_req.require_notification = FALSE;
      gstk_status = gstk_util_send_message_to_clients(&response);
      break;

    case GSTK_USER_ACTIVITY_EVT_IND_RSP:
      /* build the command */
      response.hdr_cmd.command_id           = type_of_command;
      response.hdr_cmd.cmd_detail_reference = 0; /* doesn't matter */
      response.cmd.envelope_general_rsp     = GSTK_ENVELOPE_CMD_FAIL;

      gstk_status = gstk_util_send_response_to_clients(&response,
        env_ref_id);

      /* remove the bitmask */
      gstk_evt_list[GSTK_USER_ACTIVITY]= 0x00;

      /* notify client to remove their mask */
      response.hdr_cmd.command_id = GSTK_SETUP_EVT_USER_ACT_NOTIFY;
      response.cmd.evt_notify_req.require_notification = FALSE;
      gstk_status = gstk_util_send_message_to_clients(&response);
      break;
    default:
      /* no nothing */
      break;
    }
    return gstk_status;

} /* gstk_non_idle_st_evt_dl_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_sms_pp_err_rsp

DESCRIPTION

  This function builds the envelope SMS PP error rsp and sends it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_sms_pp_err_rsp(
  const gstk_envelope_sms_pp_download_ind_type * sms_pp_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   MSG_HIGH(" IN GSDI_NON_IDLE_ST_SMS_PP_DL_ERR_RSP ",0,0,0);

   if(sms_pp_cmd == NULL)
   {
     MSG_ERROR("sms_pp_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize response */
   memset(&response, 0, sizeof(gstk_sms_pp_download_rsp_type)+sizeof(gstk_exp_hdr_type));

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_SMS_PP_DOWNLOAD_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   if(sms_pp_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       envelope_cmd_ref_table[sms_pp_cmd->message_header.user_data].user_data;
   }
   response.cmd.sms_pp_download_rsp.general_resp = GSTK_ENVELOPE_CMD_FAIL;
   response.cmd.sms_pp_download_rsp.response_data_present = FALSE;
   
   gstk_status = gstk_util_send_response_to_clients(&response,
     sms_pp_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_sms_pp_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_cc_err_rsp

DESCRIPTION

  This function builds the call control rsp error message in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the call as the Toolkit
  is no longer valid

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_cc_err_rsp(
  const gstk_envelope_cc_ind_type * cc_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;

   MSG_HIGH(" IN GSTK_NON_IDLE_ST_CC_ERR_RSP ",0,0,0);

   if(cc_cmd == NULL)
   {
     MSG_ERROR("cc_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize response */
   memset(&response, 0, sizeof(gstk_cc_rsp_type)+sizeof(gstk_exp_hdr_type));

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_CC_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   if(cc_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       envelope_cmd_ref_table[cc_cmd->message_header.user_data].user_data;
   }
   response.cmd.cc_rsp.general_resp     = GSTK_ENVELOPE_CMD_SUCCESS;
   response.cmd.cc_rsp.call_ctrl_result = GSTK_ALLOWED_NO_MOD;
   
   gstk_status = gstk_util_send_response_to_clients(&response,
     cc_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_cc_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_mo_sms_ctrl_err_rsp

DESCRIPTION

  This function builds the sms control download error rsp in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the SMS as the Toolkit
  is no longer valid

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_mo_sms_ctrl_err_rsp(
  const gstk_envelope_mo_sms_ctrl_ind_type * mo_sms_ctrl_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   gstk_cmd_from_card_type                     response;
   
   MSG_HIGH(" IN GSTK_NON_IDLE_ST_MO_SMS_CTRL_ERR_RSP ",0,0,0);

   if(mo_sms_ctrl_cmd == NULL)
   {
     MSG_ERROR("mo_sms_ctrl_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   /* initialize response */
   memset(&response, 0, sizeof(gstk_mo_sms_ctrl_rsp_type)+sizeof(gstk_exp_hdr_type));

   /* build the command */
   response.hdr_cmd.command_id           = GSTK_MO_SMS_CTRL_IND_RSP;
   response.hdr_cmd.cmd_detail_reference = 0; /* doens't matter */
   if(mo_sms_ctrl_cmd->message_header.user_data < GSTK_MAX_PROACTIVE_COMMAND) {
     response.hdr_cmd.user_data =
       envelope_cmd_ref_table[mo_sms_ctrl_cmd->message_header.user_data].user_data;
   }
   response.cmd.mo_sms_cc_rsp.general_resp       = GSTK_ENVELOPE_CMD_SUCCESS;
   response.cmd.mo_sms_cc_rsp.mo_sms_ctrl_result = GSTK_ALLOWED_NO_MOD;
   
   gstk_status = gstk_util_send_response_to_clients(&response,
     mo_sms_ctrl_cmd->message_header.user_data);

   return gstk_status;

} /* gstk_non_idle_st_mo_sms_ctrl_err_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_timer_expire_err_rsp

DESCRIPTION

  This function builds the timer expiry non idle state error rsp and sends 
  it to client

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_non_idle_st_timer_expire_err_rsp(
  const gstk_envelope_timer_expire_ind_type * timer_expire_cmd)
{
   gstk_status_enum_type                       gstk_status = GSTK_SUCCESS;
   uint32                                      env_ref_id;
   uint8                                       timer_table_index;

   /* the envelope response is enum, so there is no need to initialize
   response */

   MSG_HIGH(" IN GSTK_NON_IDLE_ST_TIMER_EXPIRE_ERR_RSP ",0,0,0);

   if(timer_expire_cmd == NULL)
   {
     MSG_ERROR("timer_expire_cmd ERR:NULL",0,0,0);
     return GSTK_BAD_PARAM;
   }

   env_ref_id = timer_expire_cmd->message_header.user_data;
   if(env_ref_id >= GSTK_MAX_PROACTIVE_COMMAND) {
     MSG_ERROR("Recd bad env ref id, %d", env_ref_id, 0, 0);
     gstk_status = GSTK_ERROR;
     return gstk_status;
   }

   /* re-schedule next time expire envelope command if the response from card
      is busy.
      the next envelope cmd for the timer expiry envelope will be sent to card
      when the next successful terminal response is received from the card */

   /* reset the entry in the timer management table */
   timer_table_index = uint32touint8(envelope_cmd_ref_table[env_ref_id].user_data);
   if (timer_table_index >= GSTK_MAX_TIMER_SUPPORTED) {
     MSG_ERROR("Invalid Timer Table Index %d", timer_table_index, 0, 0);
   }
   else {
      MSG_ERROR("Timer Expire Envelope Err", 0, 0, 0);
      gstk_timer_management[timer_table_index].timer_started = FALSE;
      gstk_timer_management[timer_table_index].pending       = FALSE;
      gstk_timer_management[timer_table_index].time          = 0;
      (void)rex_clr_timer( &(gstk_timer_management[timer_table_index].timer_elapsed));
      gstk_timer_management[timer_table_index].waiting_rsp = FALSE;
   }

   /* free the env cmd ref table */
   gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
   return gstk_status;

} /* gstk_non_idle_st_timer_expire_err_rsp */

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

