/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           G S T K  T I M E R  M A N A G E M E N T 


GENERAL DESCRIPTION
  This source file contains functions to process timer management 
  proactive command


FUNCTIONS
  gstk_send_timer_management_terminal_rsp
    This function populate the gstk_timer_manage_cnf_type and put the
    terminal response confirm type to the GSTK queue.

  gstk_build_timer_expire_envelope
    This function build an envelope command when the timer was started and
    expired

  gstk_rex_timer_elapsed_expire_cb
    This function will add the elapsed time value to the time field of the
    timer management table and restart the timer_elapsed

  gstk_rex_timer_expire_cb
    This function builds the timer expiration envelope command to be sent
    to the card

  gstk_timer_action
    This function process the timer management proactive command, start
    the timer, deactivate or get the current time information

  gstk_timer_management_req
    This function parses the timer management Proactive Command from the card
    and sends it to the client.

  gstk_timer_management_cnf
    This function packs the timer management terminal response and send
    the command to UIM server.

  gstk_timer_management_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004-2007, 2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_timer_management.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
03/09/09   gg      Fixed elapsed timer value in elapsed timer callback
09/08/08   sk      Fixed indentation issues
04/27/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Fixed indentation
08/03/04   tml     Memory leak fix, changed to use pointer for additional info
                   in terminal response
02/12/04   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

#include "err.h"
#include "string.h"
#include "tmc.h"

#include "rex.h"

typedef enum
{
  GSTK_TIMER_START     = 0x00,
  GSTK_TIMER_END       = 0x01,
  GSTK_TIMER_GET_VALUE = 0x02
}
gstk_timer_action_enum_type;




/*===========================================================================
FUNCTION gstk_timer_management_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  timer_manage_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM
  STK_response: [Input] Pointer to the STK intermediate message

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
static void gstk_timer_management_cnf_cleanup(
  gstk_timer_manage_cnf_type               * timer_manage_cnf,
  timer_management_terminal_response_type  * STK_response)
{

  if(timer_manage_cnf == NULL)
  {
    MSG_ERROR("timer_manage_cnf ERR:NULL",0,0,0);
    return;
  }
  if(STK_response == NULL)
  {
    MSG_ERROR("STK_response ERR:NULL",0,0,0);
    return;
  }
  /* free timer management cnf and tlv */
  if (STK_response->header.result.additional_info != NULL) {
    gstk_free(STK_response->header.result.additional_info);
    STK_response->header.result.additional_info = NULL;
  }
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)timer_manage_cnf);/*lint !e826 area too small */
} /* gstk_timer_management_cnf_cleanup */


/*===========================================================================
FUNCTION gstk_rex_timer_elapsed_expire_cb

DESCRIPTION
  This function will add the elapsed time value to the time field of the
  timer management table and restart the timer_elapsed

PARAMETERS
  param: [Input] Specifies which timer has expired 

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
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gstk_rex_timer_elapsed_expire_cb( unsigned long param)
{
  MSG_HIGH("timer elapsed expire", 0, 0, 0);
  gstk_timer_management[param].time += GSTK_TIMER_ELAPSED; 
  /* restart the timer */
  (void)rex_set_timer(&(gstk_timer_management[param].timer_elapsed), 
                      GSTK_TIMER_ELAPSED);
} /* gstk_rex_timer_elapsed_expire_cb */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION gstk_rex_timer_expire_cb

DESCRIPTION
  This function trigger the timer expiration event envelope command.
  This will only build an envelope command when the timer was started and
  expired 

PARAMETERS
  param: [Input] Specifies which timer has expired 

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
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gstk_rex_timer_expire_cb ( unsigned long param)
{
  if (gstk_timer_management[param].timer_started) {
    (void)rex_set_timer(&(gstk_timer_management[param].timer_elapsed), GSTK_TIMER_ELAPSED);
  }
  gstk_build_timer_expire_envelope(param);
} /* gstk_rex_timer_expire_cb */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION gstk_build_timer_expire_envelope

DESCRIPTION
  This function build an envelope command when the timer was started and
  expired 

PARAMETERS
  param: [Input] Specifies which timer has expired 

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
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
void gstk_build_timer_expire_envelope( unsigned long param)
{
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type      * task_cmd;
  uint32                  env_ref_id;
  uint32                  user_data;
  uint32                  time_elapsed;
  uint32                  time_left;

  MSG_HIGH("timer expire dl", 0, 0, 0);

  /* Timer expire, needs to send timer envelope */
  if ((gstk_timer_management[param].timer_started) ||
      (gstk_timer_management[param].pending)){
    /* get the envelope refernce id */
    user_data = param; /* set user_data as the timer index */
    /* index location at the array, start with 0! */
    gstk_status =  gstk_util_save_env_cmd_info(
                            GSTK_CLIENT_ID,
                            NULL,
                            GSTK_TIMER_EXPIRE_IND, 
                            user_data,
                            &env_ref_id);
    if(gstk_status == GSTK_SUCCESS) {

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
        /* build the envelope command */
        task_cmd->cmd.timer_expire_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
        task_cmd->cmd.timer_expire_envelope_ind.message_header.command_id = (int)GSTK_TIMER_EXPIRE_IND;
        /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
        task_cmd->cmd.timer_expire_envelope_ind.message_header.user_data = env_ref_id;

        /* get the current time elapsed */
        time_elapsed = rex_get_timer(&(gstk_timer_management[param].timer_elapsed));
        time_elapsed = GSTK_TIMER_ELAPSED - time_elapsed;

        /* timer info */
        task_cmd->cmd.timer_expire_envelope_ind.timer_info.timer_id = uint32touint8(param+1);
        task_cmd->cmd.timer_expire_envelope_ind.timer_info.time_value.hour =
            uint32touint8((gstk_timer_management[param].time + time_elapsed) / GSTK_MILISECONDS_IN_AN_HOUR);
        time_left = (gstk_timer_management[param].time + time_elapsed) % GSTK_MILISECONDS_IN_AN_HOUR;
        task_cmd->cmd.timer_expire_envelope_ind.timer_info.time_value.minute =
            uint32touint8(time_left / GSTK_MILISECONDS_IN_A_MINUTE);
        time_left = time_left % GSTK_MILISECONDS_IN_A_MINUTE;
        task_cmd->cmd.timer_expire_envelope_ind.timer_info.time_value.second =
            uint32touint8(time_left / GSTK_MILISECONDS_IN_A_SECOND);

        /* reset timer_start boolean */
        gstk_timer_management[param].timer_started = FALSE;
        gstk_timer_management[param].pending = TRUE;
        gstk_timer_management[param].waiting_rsp = TRUE;
        /* set GSTK_CMD_Q_SIG and put on GSTK queue */
        gstk_task_put_cmd_buf(task_cmd);
      }
      else {
        ERR("No more gstk buf", 0, 0, 0);
        /* free the env slot */
        gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
        return;
      }
    }
  }
  else { /* timer wasn't started! */
    MSG_HIGH("Timer[%d] was not running", param, 0, 0);
  }
} /* gstk_build_timer_expire_envelope */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION gstk_send_timer_management_terminal_rsp

DESCRIPTION
  This function populate the gstk_timer_manage_cnf_type and put the
  terminal response confirm type to the GSTK queue.

PARAMETERS
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  cmd_num: [Input] This is the same value as that was being passed to
                   the client in the corresponding proactive command request
  general_result: [Input] Specifies the result of the proactive command
                          operation
  additional_result: [Input] Specifies the optional additional result for the
                             proactive command, the length field in the
                             gstk_additonal_info_type specifies number of
                             additional_info in the structure
  other_info_present: [Input] Other info present or not
  other_info: [Input] Pointer to the gstk_timer_info_type

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
static gstk_status_enum_type  gstk_send_timer_management_terminal_rsp(
  uint32                                  cmd_details_ref_id,
  uint8                                   cmd_num,
  gstk_general_result_enum_type           general_result,
  const gstk_additional_info_type         *additional_result,
  boolean                                 other_info_present,
  const gstk_timer_info_type              *other_info)
{
  gstk_task_cmd_type              *command;

  if (general_result == GSTK_COMMAND_PERFORMED_SUCCESSFULLY)
  {
    if (command_details_ref_table[cmd_details_ref_id].partial_comprehension) {
      general_result = GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION;
    }
  }

  if(additional_result == NULL)
  {
    MSG_ERROR("additional_result ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  MSG_HIGH("TIMER MANAGE rsp: result=0x%x, additional info len=0x%x",
            general_result,additional_result->length,0);
  /* get command buf */
  command = gstk_task_get_cmd_buf();
  if ( command != NULL ) { /* successfully obtained command buffer */
    /* build the terminal response */
    command->cmd.timer_manage_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
    command->cmd.timer_manage_term_rsp_cnf.message_header.command_id = (uint32)GSTK_TIMER_MANAGEMENT_CNF;
    command->cmd.timer_manage_term_rsp_cnf.message_header.user_data = 0;
    command->cmd.timer_manage_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
    command->cmd.timer_manage_term_rsp_cnf.command_number = cmd_num;
    command->cmd.timer_manage_term_rsp_cnf.command_result = general_result;
    /* initialize additional info length to zero */
    command->cmd.timer_manage_term_rsp_cnf.result_additional_info.length = 0;
    command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

    if (additional_result->length != 0) {
      command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr = 
        gstk_malloc(command->cmd.timer_manage_term_rsp_cnf.result_additional_info.length);
      if (command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
        MSG_ERROR("Malloc additional info fail", 0, 0, 0);
        /* put the queue back to the free queue */
        (void)q_link(command, &command->cmd_hdr.link);
        q_put(command->cmd_hdr.done_q_ptr, &(command->cmd_hdr.link));
        return GSTK_MEMORY_ERROR;
      }
      memcpy(command->cmd.timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr,
             additional_result->additional_info,
             additional_result->length);
      command->cmd.timer_manage_term_rsp_cnf.result_additional_info.length = additional_result->length;
    }
    /* initialize the input data field present to FALSE */
    command->cmd.timer_manage_term_rsp_cnf.timer_info_present = FALSE;
    if (other_info_present && (other_info != NULL)) {
      command->cmd.timer_manage_term_rsp_cnf.timer_info_present = TRUE;
      memcpy(&command->cmd.timer_manage_term_rsp_cnf.time_info,
              other_info, sizeof(gstk_timer_info_type));
    } 

    /* command performed successfully */
    /* set GSTK_CMD_Q_SIG and put on GSTK queue */
    gstk_task_put_cmd_buf(command);
    return GSTK_SUCCESS;
  }
  else {
    // build error message to GSTK?????????????
    ERR("No more gstk buf", 0, 0, 0);
    return GSTK_MEMORY_ERROR;
  }
} /* gstk_send_timer_management_terminal_rsp */

/*===========================================================================
FUNCTION gstk_timer_action

DESCRIPTION
  This function performs the corresponding timer action

PARAMETERS
  action: [Input] Specifies whether the timer is to be started, end or 
                  get the current value of the timer
  timer_info: [Input/Output] Pointer that contains the timer information,
                             In case of start, this contains the length of
                             of the timer
                             In case of end, this will be populated with 
                             the remaining time for the timer
                             In case of get current timer value, this
                             will be populated with the current time of the
                             timer

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
static gstk_status_enum_type gstk_timer_action(
  gstk_timer_action_enum_type action,
  gstk_timer_info_type        *timer_info)
{
  uint32                current_time = 0;
  uint32                current_time_left = 0;

  if (timer_info == NULL) {
    MSG_ERROR("Null Input Param", 0, 0, 0);
    return GSTK_ERROR;
  }
  
  switch (action)
  {
  case GSTK_TIMER_START:
    MSG_HIGH("Start the timer", 0, 0, 0);
    /* calculate the time in milliseconds */
    current_time = timer_info->time_value.hour * GSTK_MILISECONDS_IN_AN_HOUR;
    current_time += timer_info->time_value.minute * GSTK_MILISECONDS_IN_A_MINUTE;
    current_time += timer_info->time_value.second * GSTK_MILISECONDS_IN_A_SECOND;
    gstk_timer_management[timer_info->timer_id - 1].time = current_time;

    /* start the timer now */
    (void)rex_set_timer(&(gstk_timer_management[timer_info->timer_id - 1].timer), 
                  gstk_timer_management[timer_info->timer_id - 1].time);
    gstk_timer_management[timer_info->timer_id - 1].timer_started = TRUE;
    gstk_timer_management[timer_info->timer_id - 1].pending = FALSE;
    break;

  case GSTK_TIMER_END:
    MSG_HIGH("End the timer", 0, 0, 0);
    if (gstk_timer_management[timer_info->timer_id - 1].timer_started) {
      current_time = rex_clr_timer(&(gstk_timer_management[timer_info->timer_id - 1].timer));
      gstk_timer_management[timer_info->timer_id - 1].timer_started = FALSE;
      gstk_timer_management[timer_info->timer_id - 1].pending = FALSE;
      gstk_timer_management[timer_info->timer_id - 1].time = 0;

      /* find out the remaining time left on the timer */
      timer_info->time_value.hour = uint32touint8(current_time / GSTK_MILISECONDS_IN_AN_HOUR);
      current_time_left = current_time % GSTK_MILISECONDS_IN_AN_HOUR;
      timer_info->time_value.minute = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_MINUTE);
      current_time_left = current_time_left % GSTK_MILISECONDS_IN_A_MINUTE;
      timer_info->time_value.second = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_SECOND);
    }
    else { /* timer already ended, send TR with timer action contradiction */
      MSG_HIGH("Timer hasn't been started", 0, 0, 0);
      return GSTK_CONTRADICTING_TIMER_REQUEST;
    }
    break;

  case GSTK_TIMER_GET_VALUE:
    MSG_HIGH("Get the timer", 0, 0, 0);
    if (gstk_timer_management[timer_info->timer_id - 1].timer_started) {
      current_time = rex_get_timer(&(gstk_timer_management[timer_info->timer_id - 1].timer));

      /* calculate the current time */
      timer_info->time_value.hour = uint32touint8(current_time / GSTK_MILISECONDS_IN_AN_HOUR);
      current_time_left = current_time % GSTK_MILISECONDS_IN_AN_HOUR;
      timer_info->time_value.minute = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_MINUTE);
      current_time_left = current_time % GSTK_MILISECONDS_IN_A_MINUTE;
      timer_info->time_value.second = uint32touint8(current_time_left / GSTK_MILISECONDS_IN_A_SECOND);
    }
    else { /* timer already ended, send TR with timer action contradiction */
      MSG_HIGH("Timer hasn't been started", 0, 0, 0);
      return GSTK_CONTRADICTING_TIMER_REQUEST;
    }
    break;

  }
  return GSTK_SUCCESS;
} /* gstk_timer_action */ 


/*===========================================================================
FUNCTION gstk_timer_management_req

DESCRIPTION
  This function parses the Timer Management Proactive Command from the card
  and get the information from the various module and send the terminal
  response to the Card.
  In case of any parsing errors or when GSTK can't set up the timer, 
  it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
gstk_status_enum_type gstk_timer_management_req (gstk_proactive_cmd_type* command)
{
  uint8                               *cmd_data = NULL;
  int32                               cmd_data_len;
  uint32                              cmd_details_ref_id;
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_additional_info_type           additional_info;
  gstk_general_result_enum_type       general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  boolean                             more_tlv = TRUE;
  int32                               curr_offset = 0;
  int32                               new_offset = 0;
  gstk_timer_info_type                timer_info;
  boolean                             timer_info_present = FALSE;

  MSG_HIGH(" IN GSTK_TIMER_MANAGEMENT_REQ ",0,0,0);

  if(command == NULL)
  {
    MSG_ERROR("Command ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize additional_info's length to 0 */
  additional_info.length = 0;
  /* initialize extra param present to FALSE */
  timer_info_present = FALSE;
  /* initialize timer value to 0 */
  memset(&timer_info, 0x00, sizeof(gstk_timer_info_type));

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data, and specifies the length of
  ** the cmd_data in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command,
                                              &cmd_data,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );


  /* GSTK_SUCCESS from preprocess_command */
  if((gstk_status == GSTK_SUCCESS) && (cmd_data != NULL))
  {
    /* Timer ID */
    gstk_status = gstk_parser_timer_id_tlv(cmd_data,
                                    cmd_data_len,
                                    curr_offset,
                                    &(timer_info.timer_id),
                                    &new_offset,
                                    &more_tlv,
                                    MANDATORY,
                                    &cmd_details_ref_id);
    if (gstk_status != GSTK_SUCCESS) {
      general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
    }
    else {
      /* timer id is good, continue... */
      switch (command_details_ref_table[cmd_details_ref_id].command_details.qualifier)
      {
      case 0x00: /* Start */
        if (more_tlv) {
          curr_offset = new_offset;
          /* get timer value */
          gstk_status = gstk_parser_timer_value_tlv(cmd_data,
                                    cmd_data_len,
                                    curr_offset,
                                    &(timer_info.time_value),
                                    &new_offset,
                                    &more_tlv,
                                    OPTIONAL,
                                    &cmd_details_ref_id);
          if (gstk_status != GSTK_SUCCESS)
          {
            general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
          }
          else {
            /* Start the timer */
            timer_info_present = TRUE;
            gstk_status = gstk_timer_action(GSTK_TIMER_START, &timer_info);
            if (gstk_status != GSTK_SUCCESS) {
              if (gstk_status == GSTK_CONTRADICTING_TIMER_REQUEST) {
                general_result = GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE;
              }
              else { /* other error */
                general_result =  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
                gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
                timer_info_present = FALSE;
              }
            }
          }
        }
        else {
          general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
        }
        break;
      case 0x01: /* Deactivate */
        if (more_tlv) {
          MSG_HIGH("Ignore Card indicates more info...", 0, 0, 0);
        }
        /* Deactivate the timer */
        timer_info_present = TRUE;
        gstk_status = gstk_timer_action(GSTK_TIMER_END, &timer_info);
        if (gstk_status != GSTK_SUCCESS) {
          if (gstk_status == GSTK_CONTRADICTING_TIMER_REQUEST) {
            general_result = GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE;
          }
          else { /* other error */
            general_result =  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
            timer_info_present = FALSE;
          }
        }
        /* deactivate the timer */
        break;
      case 0x02: /* Get current time */
        if (more_tlv) {
          MSG_HIGH("Ignore Card indicates more info...", 0, 0, 0);
        }
        /* Get current value */
        timer_info_present = TRUE;
        gstk_status = gstk_timer_action(GSTK_TIMER_GET_VALUE, &timer_info);
        if (gstk_status != GSTK_SUCCESS) {
          if (gstk_status == GSTK_CONTRADICTING_TIMER_REQUEST) {
            general_result = GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE;
          }
          else { /* other error */
            general_result =  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
            gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
            timer_info_present = FALSE;
          }
        }
        /* get the timer value */
        break;
      default:
          general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
          MSG_ERROR("Unknown qualifier for timer management", 0, 0, 0);
          break;
      }
    }
          
    /* No error has been encountered in parsing */
    /* general_result default to success at the beginning of this function */

  } /* GSTK_SUCCESS from preprocess_command */
  else { /*  preprocess command parsing error */

    ERR("Timer Management TLV parsing err", 0, 0, 0);
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  } /* if no error from preprocess command */

  /* send the Terminal Response */
  gstk_status = gstk_send_timer_management_terminal_rsp(
                              cmd_details_ref_id, /* command_details_ref */
                              command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                              /* command_num */
                              general_result, /* general_result */
                              &additional_info, /* additional result */
                              timer_info_present,
                              &timer_info /* timer info pointer */ );
  if (gstk_status != GSTK_SUCCESS) {
    MSG_MED("Can't send term rsp", 0, 0, 0);
  }

  /* nothing to release in memory location */
  if(cmd_data != NULL) {
    gstk_free(cmd_data);
    cmd_data = NULL;
  }

  return gstk_status;
} /* gstk_timer_management_req */


/*===========================================================================
FUNCTION gstk_timer_management_cnf

DESCRIPTION
  This function packs the timer management terminal response and send
  the command to UIM server.

PARAMETERS
  timer_manage_cnf: [Input] Pointer to message that is required to
                            be processed and sent to UIM

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
gstk_status_enum_type gstk_timer_management_cnf(
  gstk_timer_manage_cnf_type* timer_manage_cnf)
{

  gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;
  timer_management_terminal_response_type       STK_response;
  uim_cmd_type                                  *uim_cmd_ptr;
  int                                           offset = 0;
  int                                           i = 0;

  MSG_HIGH("IN GSTK timer management cnf ",0,0,0);

  if(timer_manage_cnf == NULL)
  {
    MSG_ERROR("timer_manage_cnf ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0, sizeof(timer_management_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      timer_manage_cnf->cmd_details_ref_id,
      timer_manage_cnf->message_header.command_id,
      timer_manage_cnf->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      timer_manage_cnf->command_result,
      timer_manage_cnf->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
    return gstk_status;
  }

  /* header length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  STK_response.num_bytes = STK_response.header.command_details.length +
                          STK_response.header.device_id.device_tag_length +
                          STK_response.header.result.result_tag_length +
                          6;

  /* Timer ID */
  if (timer_manage_cnf->timer_info_present) {
    gstk_status = gstk_packer_timer_id_tlv(
                      &timer_manage_cnf->time_info.timer_id,
                      &STK_response.timer_id);
    if (gstk_status != GSTK_SUCCESS) {
      gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
      return gstk_status;
    }

    /* header length: original data length + timer ID + 2 */
    STK_response.num_bytes += STK_response.timer_id.length + 2;

    /* check if it is a start timer cmd */
    /* check if it is success */
    if ((STK_response.header.command_details.qualifier != 0x00) &&
        ((STK_response.header.result.result == (int)GSTK_COMMAND_PERFORMED_SUCCESSFULLY) ||
         (STK_response.header.result.result == (int)GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION))) {
      /* mandatory timer value */
      gstk_status = gstk_packer_timer_value_tlv(
          &timer_manage_cnf->time_info.time_value,
          &STK_response.timer_value);
      if (gstk_status != GSTK_SUCCESS) {
        gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);
        return gstk_status;
      }
      /* header length: original data length + timer value + 2 */
      STK_response.num_bytes += STK_response.timer_value.length + 2;
    }
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL)) {
      /* clear the command detail table reference slot before since we are about to reply to
        the card */
      gstk_util_cmd_details_ref_table_free_slot(timer_manage_cnf->cmd_details_ref_id);

      /* set user_data */
      uim_cmd_ptr->hdr.user_data               = 0;
      /* populate the terminal_response info */
      uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

      /* copy command details */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                              &STK_response.header.command_details,
                              offset,
                              (STK_response.header.command_details.length +
                               GSTK_TAG_LENGTH_LEN ),
                              UIM_MAX_CHARS,
                              sizeof(gstk_command_details_tlv_type));
      offset = (int)(offset + STK_response.header.command_details.length + 
                     GSTK_TAG_LENGTH_LEN );
      /* copy device */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                             &STK_response.header.device_id,
                             offset,
                             (STK_response.header.device_id.device_tag_length + 
                              GSTK_TAG_LENGTH_LEN ),
                             UIM_MAX_CHARS,
                             sizeof(gstk_device_identities_tag_tlv_type));
      offset = (int)(offset + STK_response.header.device_id.device_tag_length + 
                     GSTK_TAG_LENGTH_LEN );
      /* copy result */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                              &STK_response.header.result,
                              offset,
                              3, /* result tag +  length + general result */
                              UIM_MAX_CHARS,
                              sizeof(gstk_result_tag_tlv_type));
      offset = offset + 3;
      if(STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                            STK_response.header.result.additional_info,
                            offset,
                            (STK_response.header.result.result_tag_length - 1), 
                            UIM_MAX_CHARS,
                            (timer_manage_cnf->result_additional_info.length * 
                             sizeof(byte)));
        offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
      }

      /* time info */
      if (offset < STK_response.num_bytes) { /* has timer info */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                &STK_response.timer_id,
                                offset,
                                (STK_response.timer_id.length + 2 ), 
                                UIM_MAX_CHARS,
                                sizeof(gstk_timer_id_tlv_type));
        offset = (int)(offset + STK_response.timer_id.length + 2);
        
        if (offset < STK_response.num_bytes) {
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                  &STK_response.timer_value,
                                  offset,
                                  (STK_response.timer_value.length + 2 ), 
                                  UIM_MAX_CHARS,
                                  sizeof(gstk_timer_value_tlv_type));
          offset = (int)(offset + STK_response.timer_value.length + 2);
        }
      }

      MSG_MED("Timer Management: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
      for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) {
        MSG_MED("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
      }

      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    }
  } /* uim_cmd_ptr != NULL */
  else {
    ERR ("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_timer_management_cnf_cleanup(timer_manage_cnf, &STK_response);  
  return gstk_status;
} /* gstk_timer_manage_cnf */
