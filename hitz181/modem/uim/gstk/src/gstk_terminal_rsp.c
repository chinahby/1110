/*===========================================================================


              G S T K    T E R M I N A L    R E S P O N S E



GENERAL DESCRIPTION
  This source file contains the major terminal response handling, based on
  various type of proactive command, the corresponding terminal response
  function handlers will be called.


FUNCTIONS
  gstk_populate_terminal_response_cnf
    This function build terminal response confirm type which will be put onto
    the GSTK command task later

  gstk_process_terminal_response
    This function switches on the command id and call the corresponding
    terminal response cnf function so that GSTK can build the corrresponding
    terminal response tlvs and send it to UIM Server

  gstk_uim_terminal_response_report
    This is the callback function for terminal response response from UIM.

  gstk_util_error_check_terminal_rsp
    This function will perform some error checking to ensure that
    the Application sending the Terminal Response has an
    opportunity to resend the Terminal Response incase it is
    invalid

  gstk_process_raw_terminal_response
    This function packs raw terminal response and sends response to the card
    through the UIM server.

  gstk_get_term_rsp_timer_exp_error_code
    This function returns appropriate term rsp result code when a proactive
    command timer expires.

  gstk_process_term_rsp_timer_expire
    This function process timer expiration while waiting for the terminal
    for a proactive command already sent to the client. 

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


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_terminal_rsp.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/09   xz      Fix issue of not sending NO RESPONSE FROM USER TR
03/16/09   yb      Fixed error in parameter checking and struct access
03/13/09   gg      Fixed error in parameter checking and struct access
03/02/09   xz      Provide SW1 and SW2 support for raw APDU responses
01/16/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
11/20/08   sk      Allow extra param to be a part of the get inkey terminal
                   response even if the result is not a success 
10/21/08   xz      Fix an issue of failing to send TR if term rsp timer expires
09/14/08   sk      Fixed lint errors
09/08/08   sk      reset timer for client response when a TR is being sent to
                   the card.
08/22/08   sk      Fixed paramter for gstk_process_term_rsp_timer_expire()
08/21/08   sk      Fixed warning and compilation errors.
08/14/08   sk      Fixed warnings
08/08/08   xz      Add support for raw terminal response
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
10/19/06   sk      Fixed lint errors.
10/11/06   sk      Fixed integer conversions.
10/08/06   sk      Added support for unknown unsupported proactive commands.
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control support and lint
04/04/06   sk      Added support for 1X related to location information
                   Added support for immediate digit response and variable 
                   timeout duration object for get inkey cmd.
04/04/05   sk      Added support for pli - battery status
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
09/30/05   sk      Added deregistration for CM events after receiving response
                   for Access Technlogy PLI.
09/01/05   sk      Added additional result field to 
                   gstk_util_error_check_terminal_rsp_other_
                   info() to support extra information for 
                   GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR
07/29/05   sk      Fixed compilation warnings.
07/13/05   sk      Added support for Access Technology.
07/11/05   sk      Reverted changes in gstk_util_error_check_terminal_rsp_other_
                   info() to correct problems with different result values.
06/17/05   jar     Fixed Compiler Error for Missing "{"
06/16/05   sk      Added support for BEARER_INDEPENDANT PROTOCOL
                   ERROR in gstk_util_error_check_terminal_rsp_other_info()
05/11/05   sst     Lint fixes
01/10/04  sk/tml   Open channel and get channel status terminal response fixes
12/06/04   sk      Changed case 'GSTK_GET_CH_STATUS_CNF' under function
                   gstk_populate_terminal_response_cnf() to include
                   status of all the open and dropped channels.
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Added support for get inkey UCSII input support
08/10/04   tml     Added checking in terminal response to determine if
                   any CALL CONNECTED envelope is required to be sent to the
                   card
08/05/04   tml     Fixed send USSD cc population issue
07/22/04   tml     Changed to use pointer for additional info
07/14/04   tml     Allow help request to have other info for Get Inkey, Get
                   Input and Select Item
06/14/04   tml     Added support to differentiate between GSTK generated END
                   proactive session verse card generated END
05/26/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
03/01/04   tml     Added provide language support
09/03/03   tml     Fixed Select Item terminal response extra param info
07/07/03   tml     Removed meaningless message
06/04/03   tml     Remove checking for 0 length get inkey response
05/18/03   tml     Linted
04/21/03   tml     Added address NPI support
03/19/03   tml     Added data_present boolean assignment for get inkey, get
                   input and select item
03/06/03   tml     Updated enum names
02/24/03   jar     Added Refresh Code.
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstk.h"
#include "gstk_terminal_rsp.h"
#include "gstk_proactive_cmd.h"

#include "gstkutil.h"

#include "string.h"
#include "msg.h"
#include "err.h"
#include "tmc.h"
#include "task.h"
#include "cm.h"

/*===========================================================================
   GLOBAL VARIABLES
===========================================================================*/

/*===========================================================================
FUNCTION gstk_populate_terminal_response_cnf

DESCRIPTION
  This function build terminal response confirm type which will be put onto
  the GSTK command task later

PARAMETERS
  command: [Input/Output] Pointer to the task_cmd_type to which the terminal
                          response is going to be populated
  terminal_response_enum: [Input] Response type for thie terminal response
  general_result: [Input] General response
  additional_result: [Input] Pointer of additional info ptr for response
  other_info: [Input] Pointer of extra info for some terminal response type
  cmd_num: [Input] command number of the proactive command that this terminal
                   response is responding
  cmd_details_ref_id: [Input] reference id in the command details table
  user_data: [Input] client user data

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
gstk_status_enum_type gstk_populate_terminal_response_cnf(
        gstk_task_cmd_type                       *command,
        gstk_cmd_enum_type                       terminal_response_enum,
        gstk_general_result_enum_type            general_result,
        const gstk_additional_info_ptr_type      *additional_result,
        const gstk_terminal_rsp_extra_param_type *other_info,
        uint8                                    cmd_num,
        uint32                                   cmd_details_ref_id,
        uint32                                   user_data)
{

  gstk_status_enum_type  gstk_status = GSTK_SUCCESS;

  if (command == NULL) {
    MSG_ERROR("Null Input Param", 0, 0, 0);
    return GSTK_MEMORY_ERROR;
  }

  if(other_info == NULL)
  {
    MSG_ERROR("other_info ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  if(additional_result == NULL)
  {
    MSG_ERROR("additional_result ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  MSG_HIGH("Other_info is %d",other_info->present,0,0);

  switch(terminal_response_enum) {
    case GSTK_REFRESH_CNF:
        MSG_HIGH("REFRESH rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.refresh_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.refresh_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.refresh_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.refresh_term_rsp_cnf.command_number = cmd_num;
        command->cmd.refresh_term_rsp_cnf.command_result = general_result;
        command->cmd.refresh_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

        /* No Additional Information necessary for the REFRESH */
        /* Command's Terminal Response                         */
        command->cmd.refresh_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.refresh_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.refresh_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.refresh_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.refresh_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.refresh_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_MORE_TIME_CNF:
        MSG_HIGH("MORE TIME rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.more_time_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.more_time_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.more_time_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.more_time_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.more_time_term_rsp_cnf.command_number = cmd_num;
        command->cmd.more_time_term_rsp_cnf.command_result = general_result;
        /* initialize length to zero */
        command->cmd.more_time_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.more_time_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.more_time_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.more_time_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.more_time_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.more_time_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_POLLING_OFF_CNF:
    case GSTK_POLL_INTERVAL_CNF:
        MSG_HIGH("POLLING rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.poll_interval_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.poll_interval_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.poll_interval_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.poll_interval_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.poll_interval_term_rsp_cnf.command_number = cmd_num;
        command->cmd.poll_interval_term_rsp_cnf.command_result = general_result;
        /* initialize length to zero */
        command->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.poll_interval_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        /* duration */
        if(other_info->present) {
          command->cmd.poll_interval_term_rsp_cnf.duration.present = TRUE;
          command->cmd.poll_interval_term_rsp_cnf.duration.length = other_info->extra_param.poll_duration.length;
          command->cmd.poll_interval_term_rsp_cnf.duration.units = other_info->extra_param.poll_duration.units;
        }
        else {
          command->cmd.poll_interval_term_rsp_cnf.duration.present = FALSE;
          command->cmd.poll_interval_term_rsp_cnf.duration.length = 0;
          command->cmd.poll_interval_term_rsp_cnf.duration.units = 0;
        }
        break;
    case GSTK_SETUP_EVENT_LIST_CNF:
        MSG_HIGH("SETUP EVT LIST rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.setup_evt_list_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.setup_evt_list_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.setup_evt_list_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.setup_evt_list_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.setup_evt_list_term_rsp_cnf.command_number = cmd_num;
        command->cmd.setup_evt_list_term_rsp_cnf.command_result = general_result;
        /* initialize length to zero */
        command->cmd.setup_evt_list_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.setup_evt_list_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.setup_evt_list_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.setup_evt_list_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.setup_evt_list_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.setup_evt_list_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_SETUP_CALL_CNF:
        MSG_HIGH("SETUP CALL rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.setup_call_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.setup_call_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.setup_call_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.setup_call_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.setup_call_term_rsp_cnf.command_number = cmd_num;
        command->cmd.setup_call_term_rsp_cnf.command_result = general_result;
        /* initialize additional info length to zero */
        command->cmd.setup_call_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.setup_call_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }

        command->cmd.setup_call_term_rsp_cnf.extra_param.has_cc_type_modification = FALSE;

        if (other_info->present) {
          command->cmd.setup_call_term_rsp_cnf.extra_param.has_cc_type_modification = TRUE;
          switch (other_info->extra_param.setup_call_extra_param.cc_req_action.call_type) {
          case GSTK_VOICE:
          case GSTK_VOICE_PTR:
            command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.call_type = GSTK_VOICE;
            MSG_ERROR("Voice Call type: Extra Param not required!", 0, 0, 0);
            /* free the additional result */
            if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
              gstk_free(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr);
              command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
            }
            return GSTK_BAD_PARAM;

          case GSTK_USSD:
          case GSTK_USSD_PTR:
            /* ussd string */
            command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.call_type = GSTK_USSD;

            /* initialize text string to zero length and null pointer */
            command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.length = 0;
            command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;

            if (other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ussd_string.length != 0) {
              command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text =
                gstk_malloc(
                      other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ussd_string.length);
              if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                memcpy(command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text,
                       other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ussd_string.text,
                       other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ussd_string.length );
                command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.dcs =
                  other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ussd_string.dcs;
                command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.length =
                  other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ussd_string.length;
              }
              else {
                command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.length = 0;
                MSG_ERROR("Memalloc for USSD String Fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr);

                  command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                return GSTK_MEMORY_ERROR;
              }
            }
            break;
          case GSTK_SS:
          case GSTK_SS_PTR:
            command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.call_type = GSTK_SS;
            /* ss string */
            command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ss_string.length = 0;
            if (other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ss_string.length != 0) {
              memcpy(command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ss_string.address,
                     other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ss_string.address,
                     other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ss_string.length);
              command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ss_string.length =
                other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ss_string.length;
              command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ss_string.TON =
                other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ss_string.TON;
              command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ss_string.NPI =
                other_info->extra_param.setup_call_extra_param.cc_req_action.gstk_address_string.ss_string.NPI;
            }
            break;

          default:
            MSG_ERROR("Unknown call type", 0, 0, 0);
            /* free the additional result */
            if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
              gstk_free(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr);
              command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
            }
            return GSTK_BAD_PARAM;
          }

          /* initialize the additional info 2 to zero, the additional_info2 is not used, gstk internally is
             going to use additional_info2_ptr */
          command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2.length = 0;
          memset(command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2.additional_info,
                 0x00,
                 GSTK_MAX_ADDITIONAL_INFO);
          command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.length = 0;
          command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;

          switch (other_info->extra_param.setup_call_extra_param.cc_req_action.call_type) {
          case GSTK_VOICE:
          case GSTK_SS:
          case GSTK_USSD:
            if (other_info->extra_param.setup_call_extra_param.additional_info2.length != 0) {
              command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr =
                 gstk_malloc(other_info->extra_param.setup_call_extra_param.additional_info2.length);
              if (command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr == NULL) {
                /* free data and return */
                MSG_ERROR("Memalloc for additional info 2 Fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                if (other_info->extra_param.setup_call_extra_param.cc_req_action.call_type == GSTK_USSD) {
                  if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                    gstk_free(
                      command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                    command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                  }
                }
                return GSTK_MEMORY_ERROR;
              }
              command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.length =
                other_info->extra_param.setup_call_extra_param.additional_info2.length;
              memcpy(command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr,
                      other_info->extra_param.setup_call_extra_param.additional_info2.additional_info,
                      other_info->extra_param.setup_call_extra_param.additional_info2.length);
            }
            break;

          case GSTK_VOICE_PTR:
          case GSTK_SS_PTR:
          case GSTK_USSD_PTR:
            if (other_info->extra_param.setup_call_extra_param.additional_info2_ptr.length != 0) {
              command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr =
                 gstk_malloc(other_info->extra_param.setup_call_extra_param.additional_info2_ptr.length);
              if (command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr == NULL) {
                /* free data and return */
                MSG_ERROR("Memalloc for additional info 2 Fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                if (other_info->extra_param.setup_call_extra_param.cc_req_action.call_type == GSTK_USSD_PTR) {
                  if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                    gstk_free(
                      command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                    command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                  }
                }
                return GSTK_MEMORY_ERROR;
              }
              command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.length =
                other_info->extra_param.setup_call_extra_param.additional_info2_ptr.length;
              memcpy(command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr,
                      other_info->extra_param.setup_call_extra_param.additional_info2_ptr.additional_info_ptr,
                      other_info->extra_param.setup_call_extra_param.additional_info2_ptr.length);
            }
            break;
          case GSTK_PDP_CONTEXT_ACT:
          case GSTK_NONE:
          case GSTK_CALL_MAX:
            break;
          }

          /* second command result */
          command->cmd.setup_call_term_rsp_cnf.extra_param.command_result2 =
            other_info->extra_param.setup_call_extra_param.command_result2;

          /* alpha present? */
          /* initialize to zero length */
          command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.length = 0;
          command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text = NULL;

          if (other_info->extra_param.setup_call_extra_param.cc_req_action.alpha.length != 0){
            command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text =
              gstk_malloc(
                         other_info->extra_param.setup_call_extra_param.cc_req_action.alpha.length);
            if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text != NULL) {
              memcpy(command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text,
                other_info->extra_param.setup_call_extra_param.cc_req_action.alpha.text,
                other_info->extra_param.setup_call_extra_param.cc_req_action.alpha.length);
              command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.length =
                other_info->extra_param.setup_call_extra_param.cc_req_action.alpha.length;
            }
            else {
              MSG_ERROR("Memalloc for alpha fail", 0, 0, 0);
              /* free the additional result */
              if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                gstk_free(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr);
                command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
              }
              /* free the USSD string */
              if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.call_type == GSTK_USSD) {
                if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                  gstk_free(
                      command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                  command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                }
              }
              /* free the additional info 2 */
              if (command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
                gstk_free(command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
                command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
              }
              return GSTK_MEMORY_ERROR;
            }
          }

          /* text string 2, only if change to USSD */
          command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.length = 0;
          command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.text = NULL;

          if (other_info->extra_param.setup_call_extra_param.text_string2.length != 0) {
            if(other_info->extra_param.setup_call_extra_param.text_string2.text != NULL) {
              command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.text =
                gstk_malloc(
                      other_info->extra_param.setup_call_extra_param.text_string2.length);
              if (command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.text != NULL) {
                memcpy(command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.text,
                       other_info->extra_param.setup_call_extra_param.text_string2.text,
                       other_info->extra_param.setup_call_extra_param.text_string2.length );
                command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.length =
                  other_info->extra_param.setup_call_extra_param.text_string2.length;
                command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.dcs =
                  other_info->extra_param.setup_call_extra_param.text_string2.dcs;
              }
              else {
                command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.length = 0;
                MSG_ERROR("Memalloc for txt string 2 fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                /* free the USSD string */
                if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.call_type == GSTK_USSD) {
                  if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                    gstk_free(
                      command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                    command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                  }
                }
                /* free the additional info 2 */
                if (command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
                  command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
                }
                /* free the alpha */
                if (command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text != NULL) {
                  gstk_free(command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text);
                  command->cmd.setup_call_term_rsp_cnf.extra_param.cc_req_action.alpha.text = NULL;
                }
                return GSTK_MEMORY_ERROR;
              }
            }
            else {
              MSG_MED("Length != 0 but no ussd text", 0, 0, 0);
            }
          }

        } /* has cc type modification */
        else {
          /* just to make sure */
          command->cmd.setup_call_term_rsp_cnf.extra_param.has_cc_type_modification = FALSE;
          command->cmd.setup_call_term_rsp_cnf.extra_param.text_string2.length = 0;
          command->cmd.setup_call_term_rsp_cnf.extra_param.additional_info2.length = 0;
        }
        break;
    case GSTK_SEND_SS_CNF:
        MSG_HIGH("SEND SS rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.send_ss_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.send_ss_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.send_ss_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.send_ss_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.send_ss_term_rsp_cnf.command_number = cmd_num;
        command->cmd.send_ss_term_rsp_cnf.command_result = general_result;
        /* initialize additional info length to zero */
        command->cmd.send_ss_term_rsp_cnf.result_additional_info.length = 0;
        /* initialize additional_info to NULL */
        command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.send_ss_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }

        command->cmd.send_ss_term_rsp_cnf.extra_param.has_cc_type_modification = FALSE;

        if (other_info->present) {
          command->cmd.send_ss_term_rsp_cnf.extra_param.has_cc_type_modification = TRUE;
          switch (other_info->extra_param.send_ss_extra_param.cc_req_action.call_type) {
          case GSTK_VOICE:
          case GSTK_VOICE_PTR:
            command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.call_type = GSTK_VOICE;
            /* address */
            if (other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.address.length != 0) {
              memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.address.address,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.address.address,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.address.length);
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.address.length =
                other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.address.length;
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.address.TON =
                other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.address.TON;
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.address.NPI =
                other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.address.NPI;
            }
            else {
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.address.length = 0;
            }

            /* subaddress */
            if (other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length != 0) {
              memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.subaddress,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.subaddress,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length);
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length =
                other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length;
            }
            else {
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length = 0;
            }

            /* ccp1 */
            if (other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length != 0) {
              memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.ccp,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.ccp,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length);
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length =
                other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length;
            }
            else {
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length = 0;
            }

            /* ccp2 */
            if (other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length != 0) {
              memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.ccp,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.ccp,
                     other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length);
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length =
                other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length;

              /* bc_repeat indicator */
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.bc_repeat_ind =
                other_info->extra_param.send_ss_extra_param.cc_req_action.bc_repeat_ind;
            }
            else {
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length = 0;
            }
            break;

          case GSTK_USSD:
          case GSTK_USSD_PTR:
            /* ussd string */
            command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.call_type = GSTK_USSD;

            /* initialize text string to zero length and null pointer */
            command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.length = 0;
            command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;

            if (other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.ussd_string.length != 0) {
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text =
                gstk_malloc(
                      other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.ussd_string.length);
              if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text,
                       other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.ussd_string.text,
                       other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.ussd_string.length );
                command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.dcs =
                  other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.ussd_string.dcs;
                command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.length =
                  other_info->extra_param.send_ss_extra_param.cc_req_action.gstk_address_string.ussd_string.length;
              }
              else {
                command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.length = 0;
                MSG_ERROR("Memalloc for ussd string fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                return GSTK_MEMORY_ERROR;
              }
            }
            break;
          case GSTK_SS:
          case GSTK_SS_PTR:
            MSG_ERROR("SS type, Extra Param not required", 0, 0, 0);
            /* free the additional result */
            if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
              gstk_free(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr);
              command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
            }
            return GSTK_BAD_PARAM;
          default:
            MSG_ERROR("Bad call type", 0, 0, 0);
            /* free the additional result */
            if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
              gstk_free(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr);
              command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
            }
            return GSTK_BAD_PARAM;
          }

          /* initialize the additional info 2 to zero, the additional_info2 is not used, gstk internally is
             going to use additional_info2_ptr */
          command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2.length = 0;
          memset(command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2.additional_info,
                 0x00,
                 GSTK_MAX_ADDITIONAL_INFO);
          command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.length = 0;
          command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;

          switch (other_info->extra_param.send_ss_extra_param.cc_req_action.call_type) {
          case GSTK_VOICE:
          case GSTK_SS:
          case GSTK_USSD:
            if (other_info->extra_param.send_ss_extra_param.additional_info2.length != 0) {
              command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr =
                 gstk_malloc(other_info->extra_param.send_ss_extra_param.additional_info2.length);
              if (command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr == NULL) {
                /* free data and return */
                MSG_ERROR("Memalloc for additional info 2 Fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                if (other_info->extra_param.send_ss_extra_param.cc_req_action.call_type == GSTK_USSD) {
                  if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                    gstk_free(
                         command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                    command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                  }
                }
                return GSTK_MEMORY_ERROR;
              }
              command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.length =
                other_info->extra_param.send_ss_extra_param.additional_info2.length;
              memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr,
                      other_info->extra_param.send_ss_extra_param.additional_info2.additional_info,
                      other_info->extra_param.send_ss_extra_param.additional_info2.length);
            }
            break;

          case GSTK_VOICE_PTR:
          case GSTK_SS_PTR:
          case GSTK_USSD_PTR:
            if (other_info->extra_param.send_ss_extra_param.additional_info2_ptr.length != 0) {
              command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr =
                 gstk_malloc(other_info->extra_param.send_ss_extra_param.additional_info2_ptr.length);
              if (command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr == NULL) {
                /* free data and return */
                MSG_ERROR("Memalloc for additional info 2 Fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                if (other_info->extra_param.send_ss_extra_param.cc_req_action.call_type == GSTK_USSD_PTR) {
                  if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                    gstk_free(
                         command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                    command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                  }
                }
                return GSTK_MEMORY_ERROR;
              }
              command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.length =
                other_info->extra_param.send_ss_extra_param.additional_info2_ptr.length;
              memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr,
                      other_info->extra_param.send_ss_extra_param.additional_info2_ptr.additional_info_ptr,
                      other_info->extra_param.send_ss_extra_param.additional_info2_ptr.length);
            }
            break;
          case GSTK_PDP_CONTEXT_ACT:
          case GSTK_NONE:
          case GSTK_CALL_MAX:
            break;
          }

          /* second command result */
          command->cmd.send_ss_term_rsp_cnf.extra_param.command_result2 =
            other_info->extra_param.send_ss_extra_param.command_result2;

          /* alpha present? */
          /* initialize to zero length */
          command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.length = 0;
          command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text = NULL;

          if (other_info->extra_param.send_ss_extra_param.cc_req_action.alpha.length != 0){
            command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text =
              gstk_malloc(
                         other_info->extra_param.send_ss_extra_param.cc_req_action.alpha.length);
            if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text != NULL) {
              memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text,
                other_info->extra_param.send_ss_extra_param.cc_req_action.alpha.text,
                other_info->extra_param.send_ss_extra_param.cc_req_action.alpha.length);
              command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.length =
                other_info->extra_param.send_ss_extra_param.cc_req_action.alpha.length;
            }
            else {
              MSG_ERROR("Memallo alpha fail", 0, 0, 0);
              /* free the additional result */
              if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                gstk_free(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr);
                command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
              }
              /* free the USSD string */
              if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.call_type == GSTK_USSD) {
                if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                  gstk_free(
                        command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                  command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                }
              }
              /* free the additional info 2 */
              if (command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
                gstk_free(command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
                command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
              }
              return GSTK_MEMORY_ERROR;
            }
          }

          /* text string 2, only if change to USSD */
          command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.length = 0;
          command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.text = NULL;

          if (other_info->extra_param.send_ss_extra_param.text_string2.length != 0) {
            if(other_info->extra_param.send_ss_extra_param.text_string2.text != NULL) {
              command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.text =
                gstk_malloc(
                      other_info->extra_param.send_ss_extra_param.text_string2.length);
              if (command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.text != NULL) {
                memcpy(command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.text,
                       other_info->extra_param.send_ss_extra_param.text_string2.text,
                       other_info->extra_param.send_ss_extra_param.text_string2.length );
                command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.dcs =
                  other_info->extra_param.send_ss_extra_param.text_string2.dcs;
                command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.length =
                  other_info->extra_param.send_ss_extra_param.text_string2.length;
              }
              else {
                command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.length = 0;
                MSG_ERROR("memalloc text string 2 fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                /* free the USSD string */
                if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.call_type == GSTK_USSD) {
                  if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
                    gstk_free(
                          command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text);
                    command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.gstk_address_string.ussd_string.text = NULL;
                  }
                }
                /* free the additional info 2 */
                if (command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
                  command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
                }
                /* free the alpha */
                if (command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text != NULL) {
                  gstk_free(command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text);
                  command->cmd.send_ss_term_rsp_cnf.extra_param.cc_req_action.alpha.text = NULL;
                }
                return GSTK_MEMORY_ERROR;
              }
            }
            else {
              MSG_MED("text string == NULl but length != 0", 0, 0, 0);
            }
          }
        } /* has cc type modification */
        else {
          /* just to make sure */
          command->cmd.send_ss_term_rsp_cnf.extra_param.has_cc_type_modification = FALSE;
          command->cmd.send_ss_term_rsp_cnf.extra_param.text_string2.length = 0;
          command->cmd.send_ss_term_rsp_cnf.extra_param.additional_info2.length = 0;
        }
        break;
    case GSTK_SEND_USSD_CNF:
        MSG_HIGH("SEND USSD rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.send_ussd_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.send_ussd_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.send_ussd_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.send_ussd_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.send_ussd_term_rsp_cnf.command_number = cmd_num;
        command->cmd.send_ussd_term_rsp_cnf.command_result = general_result;
        /* initialize additional info length to zero */
        command->cmd.send_ussd_term_rsp_cnf.result_additional_info.length = 0;
        /* initialize additional_info to NULL */
        command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.send_ussd_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }


        command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.has_cc_type_modification = FALSE;

        if (other_info->present) {
          /* text */
          command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.length =
            other_info->extra_param.send_ussd_extra_param.data.length;
          command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;

          if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.length > 0) {
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text = (uint8*)gstk_malloc(
                             command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.length);
            if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text == NULL) {
              MSG_ERROR("Malloc for ussd data text fail", 0, 0, 0);
              /* free the additional result */
              if (command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                gstk_free(command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr);
                command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
              }
              return GSTK_BAD_PARAM;
            }
            memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text,
                   other_info->extra_param.send_ussd_extra_param.data.text,
                   other_info->extra_param.send_ussd_extra_param.data.length);
          }
          command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.dcs =
            other_info->extra_param.send_ussd_extra_param.data.dcs;

          /* CC */
          if ((other_info->extra_param.send_ussd_extra_param.cc_req_action.length > 0)  &&
              (other_info->extra_param.send_ussd_extra_param.has_cc_type_modification)) {
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.has_cc_type_modification = TRUE;
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.call_type =
              other_info->extra_param.send_ussd_extra_param.cc_req_action.call_type;
            switch (other_info->extra_param.send_ussd_extra_param.cc_req_action.call_type) {
            case GSTK_VOICE:
            case GSTK_VOICE_PTR:
              command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.call_type = GSTK_VOICE;
              /* address */
              if (other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.length != 0) {
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.address,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.address,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.length);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.length =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.length;
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.TON =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.TON;
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.NPI =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.NPI;
              }
              else {
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.address.length = 0;
              }

              /* subaddress */
              if (other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length != 0) {
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.subaddress,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.subaddress,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length;
              }
              else {
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.subaddress.length = 0;
              }

              /* ccp1 */
              if (other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length != 0) {
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.ccp,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.ccp,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length;
              }
              else {
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp1.length = 0;
              }

              /* ccp2 */
              if (other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length != 0) {
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.ccp,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.ccp,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length;

                /* bc_repeat indicator */
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.bc_repeat_ind =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.bc_repeat_ind;
              }
              else {
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.voice_info.ccp2.length = 0;
              }
              break;

            case GSTK_USSD:
            case GSTK_USSD_PTR:
              MSG_ERROR("USSD type, Extra Param not required", 0, 0, 0);
              /* free the additional result */
              if (command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                gstk_free(command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr);
                command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
              }
              if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text != NULL) {
                gstk_free(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;
              }
              return GSTK_BAD_PARAM;
            case GSTK_SS:
              /* ss string */
              command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ss_string.length = 0;
              if (other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.ss_string.length != 0) {
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ss_string.address,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.ss_string.address,
                      other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.ss_string.length);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ss_string.length =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.ss_string.length;
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ss_string.TON =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.ss_string.TON;
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.gstk_address_string.ss_string.NPI =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.gstk_address_string.ss_string.NPI;
              }
              break;
            default:
              MSG_ERROR("Bad call type", 0, 0, 0);
              /* free the additional result */
              if (command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                gstk_free(command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr);
                command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
              }
              if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text != NULL) {
                gstk_free(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;
              }
              return GSTK_BAD_PARAM;
            }

            /* alpha present? */
            /* initialize to zero length */
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.length = 0;
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text = NULL;

            if (other_info->extra_param.send_ussd_extra_param.cc_req_action.alpha.length != 0){
              command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text =
                gstk_malloc(
                          other_info->extra_param.send_ussd_extra_param.cc_req_action.alpha.length);
              if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text != NULL) {
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text,
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.alpha.text,
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.alpha.length);
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.length =
                  other_info->extra_param.send_ussd_extra_param.cc_req_action.alpha.length;
              }
              else {
                MSG_ERROR("Memalloc alpha fail", 0, 0, 0);
                /* free the additional result */
                if (command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text != NULL) {
                  gstk_free(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text);
                  command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;
                }
                return GSTK_MEMORY_ERROR;
              }
            }

            /* initialize the additional info 2 to zero, the additional_info2 is not used, gstk internally is
              going to use additional_info2_ptr */
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2.length = 0;
            memset(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2.additional_info,
                  0x00,
                  GSTK_MAX_ADDITIONAL_INFO);
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.length = 0;
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr = NULL;

            switch (other_info->extra_param.send_ussd_extra_param.cc_req_action.call_type) {
            case GSTK_VOICE:
            case GSTK_SS:
            case GSTK_USSD:
              if (other_info->extra_param.send_ussd_extra_param.additional_info2.length != 0) {
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr =
                  gstk_malloc(other_info->extra_param.send_ussd_extra_param.additional_info2.length);
                if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr == NULL) {
                  /* free data and return */
                  MSG_ERROR("Memalloc for additional info 2 Fail", 0, 0, 0);
                  /* free the additional result */
                  if (command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                    gstk_free(command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr);
                    command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                  }
                  if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text != NULL) {
                    gstk_free(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text);
                    command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;
                  }
                  if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text != NULL) {
                    gstk_free(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text);
                    command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text = NULL;
                  }
                  return GSTK_MEMORY_ERROR;
                }
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.length =
                  other_info->extra_param.send_ussd_extra_param.additional_info2.length;
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr,
                        other_info->extra_param.send_ussd_extra_param.additional_info2.additional_info,
                        other_info->extra_param.send_ussd_extra_param.additional_info2.length);
              }
              break;

            case GSTK_VOICE_PTR:
            case GSTK_SS_PTR:
            case GSTK_USSD_PTR:
              if (other_info->extra_param.send_ussd_extra_param.additional_info2_ptr.length != 0) {
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr =
                  gstk_malloc(other_info->extra_param.send_ussd_extra_param.additional_info2_ptr.length);
                if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr == NULL) {
                  /* free data and return */
                  MSG_ERROR("Memalloc for additional info 2 Fail", 0, 0, 0);
                  /* free the additional result */
                  if (command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                    gstk_free(command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr);
                    command->cmd.send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                  }
                  if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text != NULL) {
                    gstk_free(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text);
                    command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;
                  }
                  if (command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text != NULL) {
                    gstk_free(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text);
                    command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.cc_req_action.alpha.text = NULL;
                  }
                  return GSTK_MEMORY_ERROR;
                }
                command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.length =
                  other_info->extra_param.send_ussd_extra_param.additional_info2_ptr.length;
                memcpy(command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr,
                        other_info->extra_param.send_ussd_extra_param.additional_info2_ptr.additional_info_ptr,
                        other_info->extra_param.send_ussd_extra_param.additional_info2_ptr.length);
              }
              break;
            case GSTK_PDP_CONTEXT_ACT:
            case GSTK_NONE:
            case GSTK_CALL_MAX:
              break;
            }

            /* second command result */
            command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.command_result2 =
              other_info->extra_param.send_ussd_extra_param.command_result2;
          } /* has cc */
        } /* has extra param*/
        else {
          /* just to make sure */
          command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.has_cc_type_modification = FALSE;
          command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.length = 0;
          command->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2.length = 0;
        }
        break;
    case GSTK_SEND_SMS_CNF:
        MSG_HIGH("SEND SMS rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.send_sms_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.send_sms_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.send_sms_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.send_sms_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.send_sms_term_rsp_cnf.command_number = cmd_num;
        command->cmd.send_sms_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.send_sms_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.send_sms_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.send_sms_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.send_sms_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.send_sms_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.send_sms_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_LAUNCH_BROWSER_CNF:
        MSG_HIGH("LAUNCH BROWSER rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.launch_browser_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.launch_browser_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.launch_browser_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.launch_browser_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.launch_browser_term_rsp_cnf.command_number = cmd_num;
        command->cmd.launch_browser_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.launch_browser_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.launch_browser_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.launch_browser_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.launch_browser_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.launch_browser_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.launch_browser_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_PLAY_TONE_CNF:
        MSG_HIGH("PLAY TONE rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.play_tone_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.play_tone_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.play_tone_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.play_tone_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.play_tone_term_rsp_cnf.command_number = cmd_num;
        command->cmd.play_tone_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.play_tone_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.play_tone_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.play_tone_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.play_tone_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.play_tone_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.play_tone_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_DISPLAY_TEXT_CNF:
        MSG_HIGH("DISPLAY TXT rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.display_text_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.display_text_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.display_text_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.display_text_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.display_text_term_rsp_cnf.command_number = cmd_num;
        command->cmd.display_text_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.display_text_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.display_text_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.display_text_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.display_text_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.display_text_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.display_text_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_GET_INKEY_CNF:
        MSG_HIGH("GET INKEY rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.get_inkey_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.get_inkey_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.get_inkey_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.get_inkey_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.get_inkey_term_rsp_cnf.command_number = cmd_num;
        command->cmd.get_inkey_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.get_inkey_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.get_inkey_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the input data field present to FALSE */
        command->cmd.get_inkey_term_rsp_cnf.data_present = FALSE;
        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.length != 0) {
            command->cmd.get_inkey_term_rsp_cnf.data_present = TRUE;
            command->cmd.get_inkey_term_rsp_cnf.returned_data_len = other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.length;
            memset(command->cmd.get_inkey_term_rsp_cnf.data, 0x00, GSTK_GET_INKEY_USER_INPUT_DATA_LEN);
            if (command->cmd.get_inkey_term_rsp_cnf.returned_data_len <= GSTK_GET_INKEY_USER_INPUT_DATA_LEN) { /* should be 1 only */
              memcpy(command->cmd.get_inkey_term_rsp_cnf.data,
                     other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.text,
                     command->cmd.get_inkey_term_rsp_cnf.returned_data_len);
            }
            command->cmd.get_inkey_term_rsp_cnf.data_coding_scheme = other_info->extra_param.get_inkey_extra_param.user_get_inkey_input.dcs;
          }
          else {
            command->cmd.get_inkey_term_rsp_cnf.returned_data_len = 0;
          }
#ifdef FEATURE_CAT_REL6
          /* Copy the duration of execution if present */
          if(other_info->extra_param.get_inkey_extra_param.duration_of_exec.present)
          {
            command->cmd.get_inkey_term_rsp_cnf.duration_of_exec 
                = other_info->extra_param.get_inkey_extra_param.duration_of_exec;
          }
          else
#endif /* #ifdef FEATURE_CAT_REL6 */
          {
            command->cmd.get_inkey_term_rsp_cnf.duration_of_exec.present = FALSE;
          }
        }
        else {
          if (command->cmd.get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;
    case GSTK_GET_INPUT_CNF:
        MSG_HIGH("GET INPUT rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.get_input_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.get_input_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.get_input_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.get_input_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.get_input_term_rsp_cnf.command_number = cmd_num;
        command->cmd.get_input_term_rsp_cnf.command_result = general_result;
        /* initialize additional info length to zero */
        command->cmd.get_input_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.get_input_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }

        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the input data field present to FALSE */
        command->cmd.get_input_term_rsp_cnf.data_present = FALSE;
        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.get_input_term_rsp_cnf.data_present = TRUE;
            command->cmd.get_input_term_rsp_cnf.returned_data_len = other_info->extra_param.user_get_input.length;
            if (command->cmd.get_input_term_rsp_cnf.returned_data_len > 0) {
              command->cmd.get_input_term_rsp_cnf.data = (uint8*)gstk_malloc(
                               other_info->extra_param.user_get_input.length);
              if (command->cmd.get_input_term_rsp_cnf.data == NULL) {
                MSG_ERROR("Memalloc for input data Fail", 0, 0, 0);
                if (command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
                  gstk_free(command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr);
                  command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
                }
                return GSTK_MEMORY_ERROR;
              }
              memcpy(command->cmd.get_input_term_rsp_cnf.data,
                     other_info->extra_param.user_get_input.text,
                     other_info->extra_param.user_get_input.length);
            }
            command->cmd.get_input_term_rsp_cnf.data_coding_scheme = other_info->extra_param.user_get_input.dcs;
          }
          else {

            command->cmd.get_input_term_rsp_cnf.returned_data_len = 0;
          }
        }
        else {
          if (command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.get_input_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;
    case GSTK_SELECT_ITEM_CNF:
        MSG_HIGH("SELECT ITEM rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.select_item_term_rsp_cnf.message_header.command_group  = GSTK_TERMINAL_RSP;
        command->cmd.select_item_term_rsp_cnf.message_header.command_id     = (uint32)terminal_response_enum;
        command->cmd.select_item_term_rsp_cnf.message_header.user_data      = user_data;
        command->cmd.select_item_term_rsp_cnf.cmd_details_ref_id            = cmd_details_ref_id;
        command->cmd.select_item_term_rsp_cnf.command_number                = cmd_num;
        command->cmd.select_item_term_rsp_cnf.command_result                = general_result;

        /* initialize additional info length to zero */
        command->cmd.select_item_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.select_item_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.select_item_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.select_item_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.select_item_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.select_item_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }

        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the item chosen id field present to FALSE */
        command->cmd.select_item_term_rsp_cnf.data_present = FALSE;
        if (gstk_status == GSTK_SUCCESS)
        {
          if (other_info->present) {
            command->cmd.select_item_term_rsp_cnf.data_present = TRUE;
            command->cmd.select_item_term_rsp_cnf.chosen_item_identifier =
                other_info->extra_param.select_item_chosen_item;
          }
        }
        else {
          if (command->cmd.select_item_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.select_item_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.select_item_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;
    case GSTK_SETUP_MENU_CNF:
        MSG_HIGH("SETUP MENU rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.setup_menu_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.setup_menu_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.setup_menu_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.setup_menu_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.setup_menu_term_rsp_cnf.command_number = cmd_num;
        command->cmd.setup_menu_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.setup_menu_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.setup_menu_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.setup_menu_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.setup_menu_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.setup_menu_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.setup_menu_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_PROVIDE_LOCAL_INFO_CNF:
    case GSTK_TIMER_MANAGEMENT_CNF:
        /* internal */
        break;
    case GSTK_PROVIDE_BATT_STATUS_CNF:
#ifdef FEATURE_CAT_REL6
        MSG_HIGH("PROVIDE BATTERY STATUS rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
                command->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.provide_local_info_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.provide_local_info_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.provide_local_info_term_rsp_cnf.command_number = cmd_num;
        command->cmd.provide_local_info_term_rsp_cnf.command_result = general_result;
        /* initialize additional info length to zero */
        command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
        if (additional_result->length != 0) 
        {
          command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) 
          {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the extra param for local info to no info */
        command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;
        if(gstk_status == GSTK_SUCCESS) 
        {
          if (other_info->present) 
          {
            command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_BATTERY_STATUS_INFO;
            command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.battery_status = 
                other_info->extra_param.battery_status_info_extra_param;
          }
        }
        else 
        {
          if (command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) 
          {
            gstk_free(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
#endif /* #ifdef FEATURE_CAT_REL6 */
        break;
    case GSTK_PROVIDE_LANG_INFO_CNF:
        MSG_HIGH("PROVIDE LANG INFO rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.provide_local_info_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.provide_local_info_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.provide_local_info_term_rsp_cnf.command_number = cmd_num;
        command->cmd.provide_local_info_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }

        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the extra param for local info to no info */
        command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;
        memset(command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.lang_code,0x00,
               2);
        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_LANGUAGE_SETTING_INFO;
            memcpy(command->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.lang_code,
               other_info->extra_param.provide_lang_info_extra_param.lang_code,
               2);
          }
        }
        else {
          if (command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;

    case GSTK_SETUP_IDLE_TEXT_CNF:
        MSG_HIGH("SETUP IDLE TXT rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.setup_idle_text_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.setup_idle_text_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.setup_idle_text_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.setup_idle_text_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.setup_idle_text_term_rsp_cnf.command_number = cmd_num;
        command->cmd.setup_idle_text_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.setup_idle_text_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.setup_idle_text_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.setup_idle_text_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.setup_idle_text_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.setup_idle_text_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.setup_idle_text_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;
    case GSTK_RUN_AT_CMDS_CNF:
        break;
    case GSTK_LANG_NOTIFICATION_CNF:
        MSG_HIGH("LANG NOTIF rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.lang_notification_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.lang_notification_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.lang_notification_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.lang_notification_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;
        command->cmd.lang_notification_term_rsp_cnf.command_number = cmd_num;
        command->cmd.lang_notification_term_rsp_cnf.command_result = general_result;

        /* initialize additional info length to zero */
        command->cmd.lang_notification_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.lang_notification_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.lang_notification_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.lang_notification_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.lang_notification_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.lang_notification_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;

    case GSTK_SEND_DTMF_CNF:
        MSG_HIGH("SEND DTMF rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.send_dtmf_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.send_dtmf_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.send_dtmf_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.send_dtmf_term_rsp_cnf.command_number = cmd_num;
        command->cmd.send_dtmf_term_rsp_cnf.command_result = general_result;
        command->cmd.send_dtmf_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

        /* No Additional Information necessary for the SEND DTMF */
        /* Command's Terminal Response                         */
        command->cmd.send_dtmf_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.send_dtmf_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.send_dtmf_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.send_dtmf_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.send_dtmf_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.send_dtmf_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;

     case GSTK_OPEN_CH_CNF:
        MSG_HIGH("OPEN CHANNEL rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.open_ch_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.open_ch_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.open_ch_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.open_ch_term_rsp_cnf.command_number = cmd_num;
        command->cmd.open_ch_term_rsp_cnf.command_result = general_result;
        command->cmd.open_ch_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

        /* Initialize the additional info to zero */
        command->cmd.open_ch_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.open_ch_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the extra param for open channel rsp */
        memset(&command->cmd.open_ch_term_rsp_cnf.ch_status, 0x00, sizeof(gstk_ch_status_type));
        memset(&command->cmd.open_ch_term_rsp_cnf.bearer_description, 0x00, sizeof(gstk_bearer_description_type));
        command->cmd.open_ch_term_rsp_cnf.buffer_size = 0;
        command->cmd.open_ch_term_rsp_cnf.other_data_present = FALSE;
        command->cmd.open_ch_term_rsp_cnf.ch_status_present = FALSE;

        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.open_ch_term_rsp_cnf.other_data_present = TRUE;
            switch(general_result) {
            case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
            case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
            case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
            case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
            case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
              command->cmd.open_ch_term_rsp_cnf.ch_status_present = TRUE;
              memcpy(&command->cmd.open_ch_term_rsp_cnf.ch_status,
                &other_info->extra_param.open_ch_extra_param.ch_status,
                sizeof(gstk_ch_status_type));
              break;
            default:
              /* no need to include ch status in case of failure */
              break;
            }

            /* bearer description and buffer size */
            memcpy(&command->cmd.open_ch_term_rsp_cnf.bearer_description,
                &other_info->extra_param.open_ch_extra_param.bearer_description,
                sizeof(gstk_bearer_description_type));
            command->cmd.open_ch_term_rsp_cnf.buffer_size = other_info->extra_param.open_ch_extra_param.buffer_size;
          }
        }
        else {
          if (command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;

     case GSTK_CLOSE_CH_CNF:
        MSG_HIGH("CLOSE CHANNEL rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.close_ch_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.close_ch_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.close_ch_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.close_ch_term_rsp_cnf.command_number = cmd_num;
        command->cmd.close_ch_term_rsp_cnf.command_result = general_result;
        command->cmd.close_ch_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

        /* Initialize the additional info to zero */
        command->cmd.close_ch_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.close_ch_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.close_ch_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.close_ch_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.close_ch_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.close_ch_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        break;

     case GSTK_RECEIVE_DATA_CNF:
        MSG_HIGH("RECEIVE DATA rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.receive_data_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.receive_data_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.receive_data_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.receive_data_term_rsp_cnf.command_number = cmd_num;
        command->cmd.receive_data_term_rsp_cnf.command_result = general_result;
        command->cmd.receive_data_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

        /* Initialize the additional info to zero */
        command->cmd.receive_data_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.receive_data_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the extra param for receive data rsp */
        command->cmd.receive_data_term_rsp_cnf.ch_data.data_len = 0;
        command->cmd.receive_data_term_rsp_cnf.ch_data.data = NULL;
        command->cmd.receive_data_term_rsp_cnf.ch_data_remaining_len = 0;
        command->cmd.receive_data_term_rsp_cnf.data_present = FALSE;

        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.receive_data_term_rsp_cnf.data_present = TRUE;
            if ((other_info->extra_param.receive_data_extra_param.ch_data.data_len > 0) &&
                (other_info->extra_param.receive_data_extra_param.ch_data.data != NULL)) {
              command->cmd.receive_data_term_rsp_cnf.ch_data.data = gstk_malloc(
                       int32touint32(other_info->extra_param.receive_data_extra_param.ch_data.data_len));
              if (command->cmd.receive_data_term_rsp_cnf.ch_data.data != NULL) {
                memcpy(command->cmd.receive_data_term_rsp_cnf.ch_data.data,
                  other_info->extra_param.receive_data_extra_param.ch_data.data,
                  int32touint32(other_info->extra_param.receive_data_extra_param.ch_data.data_len));
                command->cmd.receive_data_term_rsp_cnf.ch_data.data_len =
                  other_info->extra_param.receive_data_extra_param.ch_data.data_len;
                if (other_info->extra_param.receive_data_extra_param.ch_data_remaining_len > 0xFF) {
                  command->cmd.receive_data_term_rsp_cnf.ch_data_remaining_len = 0xFF;
                }
                else {
                  command->cmd.receive_data_term_rsp_cnf.ch_data_remaining_len =
                    other_info->extra_param.receive_data_extra_param.ch_data_remaining_len;
                }
              }
              else {
                gstk_status = GSTK_MEMORY_ERROR;
              }
            }
          }
        }

        if (gstk_status != GSTK_SUCCESS) {
          if (command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;

     case GSTK_SEND_DATA_CNF:
        MSG_HIGH("SEND DATA rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.send_data_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.send_data_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.send_data_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.send_data_term_rsp_cnf.command_number = cmd_num;
        command->cmd.send_data_term_rsp_cnf.command_result = general_result;
        command->cmd.send_data_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

        /* Initialize the additional info to zero */
        command->cmd.send_data_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.send_data_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }

        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                           terminal_response_enum,
                           general_result,
                           other_info,
                           additional_result);
        /* initialize the extra param for send data rsp */
        command->cmd.send_data_term_rsp_cnf.ch_data_remain_buf_len = 0;
        command->cmd.send_data_term_rsp_cnf.data_present = FALSE;

        if(gstk_status == GSTK_SUCCESS) {
          if (other_info->present) {
            command->cmd.send_data_term_rsp_cnf.data_present = TRUE;
            if (other_info->extra_param.send_data_extra_param.ch_data_remain_buf_len > 0xFF) {
              command->cmd.send_data_term_rsp_cnf.ch_data_remain_buf_len = 0xFF;
            }
            else {
              command->cmd.send_data_term_rsp_cnf.ch_data_remain_buf_len =
                other_info->extra_param.send_data_extra_param.ch_data_remain_buf_len;
            }
          }
        }
        else {
          if (command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.send_data_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }

        break;

     case GSTK_GET_CH_STATUS_CNF:
        MSG_HIGH("GET CH STATUS rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.get_ch_status_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.get_ch_status_term_rsp_cnf.message_header.command_id = (uint32)terminal_response_enum;
        command->cmd.get_ch_status_term_rsp_cnf.message_header.user_data = user_data;
        command->cmd.get_ch_status_term_rsp_cnf.command_number = cmd_num;
        command->cmd.get_ch_status_term_rsp_cnf.command_result = general_result;
        command->cmd.get_ch_status_term_rsp_cnf.cmd_details_ref_id = cmd_details_ref_id;

        /* Initialize the additional info to zero */
        command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.length = 0;
        command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;

        if (additional_result->length != 0) {
          command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr =
                            gstk_malloc(additional_result->length);
          if (command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr == NULL) {
            MSG_ERROR("Memalloc for additional info Fail", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr,
                 additional_result->additional_info_ptr,
                 additional_result->length);
          command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.length = additional_result->length;
        }
        gstk_status = gstk_util_error_check_terminal_rsp_other_info(
                            terminal_response_enum,
                            general_result,
                            other_info,
                            additional_result);
        /* initialize the extra param for open channel rsp */
        memset(command->cmd.get_ch_status_term_rsp_cnf.ch_status, 0x00, sizeof(gstk_ch_status_type));
        command->cmd.get_ch_status_term_rsp_cnf.data_present = FALSE;

        if(gstk_status == GSTK_SUCCESS)
        {
          if (other_info->present)
          {
            command->cmd.get_ch_status_term_rsp_cnf.data_present = TRUE;
            if(other_info->extra_param.get_ch_status_extram_param.cnt > GSTK_MAX_GPRS_CHANNELS)
            {
              MSG_ERROR("No of Channels cannot be greater than MAX 0x%x", 0, 0, 0);
              return GSTK_BAD_PARAM;
            }
            command->cmd.get_ch_status_term_rsp_cnf.ch_cnt = other_info->extra_param.get_ch_status_extram_param.cnt;
            memcpy(command->cmd.get_ch_status_term_rsp_cnf.ch_status,
                    other_info->extra_param.get_ch_status_extram_param.ch_status,
                    sizeof(gstk_ch_status_type) *
                    int32touint32(command->cmd.get_ch_status_term_rsp_cnf.ch_cnt));
          }
        }
        else {
          if (command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
            gstk_free(command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr);
            command->cmd.get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
          }
        }
        break;
    case GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF:
        MSG_HIGH("UNSUPPORTED/UNKNOWN rsp, result=0x%x additional info len=0x%x",
                   general_result, additional_result->length, 0);
        command->cmd.unknown_unsupported_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
        command->cmd.unknown_unsupported_term_rsp_cnf.message_header.command_id    = (uint32)terminal_response_enum;
        command->cmd.unknown_unsupported_term_rsp_cnf.message_header.user_data     = user_data;
        command->cmd.unknown_unsupported_term_rsp_cnf.command_number               = cmd_num;
        command->cmd.unknown_unsupported_term_rsp_cnf.command_result               = general_result;
        command->cmd.unknown_unsupported_term_rsp_cnf.cmd_details_ref_id           = cmd_details_ref_id;
        break;
    default:
        MSG_ERROR("TR enum unexpected 0x%x", terminal_response_enum, 0, 0);
        return GSTK_BAD_PARAM;
    }

    return gstk_status;
} /* gstk_populate_terminal_response_cnf*/

/*===========================================================================
FUNCTION gstk_process_terminal_response

DESCRIPTION

  This function switches on the command id and call the corresponding
  terminal response cnf function so that GSTK can build the corrresponding
  terminal response tlvs and send it to UIM Server

PARAMETERS
  cmd: [Input] Pointer to the terminal response confirm types to be processed

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
gstk_status_enum_type gstk_process_terminal_response(
    gstk_cmd_type* cmd )
{
    gstk_status_enum_type    gstk_status = GSTK_SUCCESS;
    uint32                   type_of_command;
#ifdef FEATURE_CAT_REL6
    boolean                  found_flg   = FALSE;   
    cm_client_status_e_type  cm_reg_return_status;
#endif /* FEATURE_CAT_REL6 */

    if(cmd == NULL)
    {
      MSG_ERROR("cmd ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }

    /* Get the type of command */
    type_of_command = cmd->general_cmd.message_header.command_id;

    switch( type_of_command ) {
    case GSTK_REFRESH_CNF:
        gstk_status = gstk_refresh_cnf(&(cmd->refresh_term_rsp_cnf));
        break;
    case GSTK_MORE_TIME_CNF:
        gstk_status = gstk_more_time_cnf(&(cmd->more_time_term_rsp_cnf));
        break;
    case GSTK_POLLING_OFF_CNF:
    case GSTK_POLL_INTERVAL_CNF:
        gstk_status = gstk_polling_cnf(&(cmd->poll_interval_term_rsp_cnf));
        break;
    case GSTK_SETUP_EVENT_LIST_CNF:
        gstk_status = gstk_consolidate_setup_evt_list_cnf(
                        &(cmd->setup_evt_list_term_rsp_cnf));
        break;
    case GSTK_SETUP_CALL_CNF:
        gstk_status = gstk_setup_mo_call_cnf(&(cmd->setup_call_term_rsp_cnf));
        break;
    case GSTK_SEND_SS_CNF:
        gstk_status = gstk_send_ss_cnf(&(cmd->send_ss_term_rsp_cnf));
        break;
    case GSTK_SEND_USSD_CNF:
        gstk_status = gstk_send_ussd_cnf(&(cmd->send_ussd_term_rsp_cnf));
        break;
    case GSTK_SEND_SMS_CNF:
        gstk_status = gstk_send_sms_cnf(&(cmd->send_sms_term_rsp_cnf));
        break;
    case GSTK_LAUNCH_BROWSER_CNF:
        gstk_status = gstk_launch_browser_cnf(&(cmd->launch_browser_term_rsp_cnf));
        break;
    case GSTK_PLAY_TONE_CNF:
        gstk_status = gstk_play_tone_cnf(&(cmd->play_tone_term_rsp_cnf));
        break;
    case GSTK_DISPLAY_TEXT_CNF:
        gstk_status = gstk_display_text_cnf(&(cmd->display_text_term_rsp_cnf));
        break;
    case GSTK_GET_INKEY_CNF:
        gstk_status = gstk_get_inkey_cnf(&(cmd->get_inkey_term_rsp_cnf));
        break;
    case GSTK_GET_INPUT_CNF:
        gstk_status = gstk_get_input_cnf(&(cmd->get_input_term_rsp_cnf));
        break;
    case GSTK_SELECT_ITEM_CNF:
        gstk_status = gstk_select_item_cnf(&(cmd->select_item_term_rsp_cnf));
        break;
    case GSTK_SETUP_MENU_CNF:
        gstk_status = gstk_setup_menu_cnf(&(cmd->setup_menu_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_ACCESS_TECH_CNF:
        MSG_HIGH("GSTK_PROVIDE_ACCESS_TECH_CNF...",0,0,0);
        gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_NW_SEARCH_MODE_CNF:
#ifdef FEATURE_CAT_REL6
        MSG_HIGH("GSTK_PROVIDE_NW_SEARCH_MODE_CNF...",0,0,0);
         /* Find a match for the data block pointer info in the command_details_ref_table */
        found_flg = gstk_util_find_match_in_cmd_details_ref_table(
                      &(cmd->provide_local_info_term_rsp_cnf.command_number),
                      GSTK_PROVIDE_NW_SEARCH_MODE_CNF);

        /* If a match is found in the cmd details reference table */
        if(found_flg ==  FALSE)
        {
          MSG_HIGH("No GSTK relevant request found in the cmd details table", 0, 0, 0);
          gstk_status = GSTK_INVALID_COMMAND; 
        }
        else
        {
          gstk_status = gstk_provide_local_information_cnf(&(cmd->provide_local_info_term_rsp_cnf));
          /* DeRegister with CM to not receive the CM_PH_EVENT_SYS_SEL_PREF*/
          cm_reg_return_status = cm_client_ph_reg(
            gstk_client_id_from_cm,               /* pointer to the client struct */
            gstk_cm_ph_event_cb,                  /* pointer to a cb function for ph events */
            CM_CLIENT_EVENT_DEREG,                /* event registration */
            CM_PH_EVENT_INFO,                     /* from this call event */
            CM_PH_EVENT_INFO,                     /* to this call event */
            NULL );                               /* pointer to a cb function for errors */
          if (cm_reg_return_status != CM_CLIENT_OK) 
          {
            MSG_ERROR("REGISTRATION UNSUCCESSFUL WITH CM", 0, 0, 0);
            /* Do we send a terminal response to the card for this */
          }
        }
#endif /* FEATURE_CAT_REL6 */
        break;
    case GSTK_PROVIDE_LOCAL_INFO_CNF:
        MSG_HIGH("GSTK_PROVIDE_LOCAL_INFO_CNF...",0,0,0);
        gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_LANG_INFO_CNF:
        gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
        break;
    case GSTK_PROVIDE_BATT_STATUS_CNF:
#ifdef FEATURE_CAT_REL6
        gstk_status = gstk_provide_local_info_cnf(&(cmd->provide_local_info_term_rsp_cnf));
#endif /* #ifdef FEATURE_CAT_REL6 */
        break;
    case GSTK_SETUP_IDLE_TEXT_CNF:
        gstk_status = gstk_setup_idle_mode_text_cnf(&(cmd->setup_idle_text_term_rsp_cnf));
        break;
    case GSTK_TIMER_MANAGEMENT_CNF:
        gstk_status = gstk_timer_management_cnf(&(cmd->timer_manage_term_rsp_cnf));
        break;
    case GSTK_LANG_NOTIFICATION_CNF:
        gstk_status = gstk_lang_notification_cnf(&(cmd->lang_notification_term_rsp_cnf));
        break;
    case GSTK_SEND_DTMF_CNF:
        gstk_status = gstk_send_dtmf_cnf(&(cmd->send_dtmf_term_rsp_cnf));
        break;
    case GSTK_OPEN_CH_CNF:
        gstk_status = gstk_open_ch_cnf(&(cmd->open_ch_term_rsp_cnf));
        break;
    case GSTK_CLOSE_CH_CNF:
        gstk_status = gstk_close_ch_cnf(&(cmd->close_ch_term_rsp_cnf));
        break;
    case GSTK_SEND_DATA_CNF:
        gstk_status = gstk_send_data_cnf(&(cmd->send_data_term_rsp_cnf));
        break;
    case GSTK_RECEIVE_DATA_CNF:
        gstk_status = gstk_receive_data_cnf(&(cmd->receive_data_term_rsp_cnf));
        break;
    case GSTK_GET_CH_STATUS_CNF:
        gstk_status = gstk_get_ch_status_cnf(&(cmd->get_ch_status_term_rsp_cnf));
        break;
    case GSTK_RUN_AT_CMDS_CNF:
    case GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF:   
        gstk_status = gstk_unsupported_unknown_cnf(&(cmd->unknown_unsupported_term_rsp_cnf));
        break;
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
    default:
        gstk_status = GSTK_INVALID_COMMAND;
        break;
    }

    return(gstk_status);
} /* gstk_process_terminal_response */


/*===========================================================================
FUNCTION gstk_uim_terminal_response_report

DESCRIPTION

  This is the callback function for terminal response response from UIM.
  UIM will call this function when it receives terminal response response
  from the Card.
  Based on the return status of the terminal response response, this function
  will decide whether an end proactive session request should be sent
  to the client or not

PARAMETERS
  report: [Input] Pointer to the UIM terminal response report

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
void gstk_uim_terminal_response_report (uim_rpt_type *report)
{
    gstk_task_cmd_type      *task_cmd;
    
#if defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT) && defined(FEATURE_GSTK_REPORT_SW1_SW2)
    gstk_cmd_from_card_type sw1_sw2_rpt;
    
    memset(&sw1_sw2_rpt, 0x00, sizeof(sw1_sw2_rpt));
#endif /* (FEATURE_GSTK_STREAMING_APDU_SUPPORT && FEATURE_GSTK_REPORT_SW1_SW2) */

    if(report == NULL)
    {
      MSG_ERROR("report ERR:NULL",0,0,0);
      return;
    }

    MSG_HIGH("GSTK rec'd Term Rsp from UIM, 0x%x, 0x%x",
              report->sw1, report->sw2, 0);

    if (report->rpt_type != UIM_TERMINAL_RESPONSE_R) {
        // build error message to GSTK?????????????
        ERR("TR expected in CB", 0, 0, 0);
        return;
    }

#if defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT) && defined(FEATURE_GSTK_REPORT_SW1_SW2)
    if (gstk_util_is_valid_client_id(report->user_data))
    {      
      MSG_HIGH("Send SW1 and SW2 to client 0x%x.", report->user_data, 0, 0);
      sw1_sw2_rpt.hdr_cmd.command_id = GSTK_TERM_RSP_SW1_SW2_RPT;
      sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw1 = (uint8)report->sw1;
      sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw2 = (uint8)report->sw2;
      gstk_util_call_client_evt_cb(report->user_data, &sw1_sw2_rpt);
    }
#endif /* (FEATURE_GSTK_STREAMING_APDU_SUPPORT && FEATURE_GSTK_REPORT_SW1_SW2) */

    if ((report->sw1 == SW1_NORMAL_END) &&
        (report->sw2 == SW2_NORMAL_END))
    {
       /* check if any pending envelope commands to be sent due to last failure */
       if (gstk_util_has_pending_timer_envelope()) {
         /* Set the signal to send the envelope */
         (void)rex_set_sigs(&gstk_tcb,  GSTK_RETRY_ENVELOPE_SIG);
       }
       if (gstk_util_find_first_non_timer_expire_postpone_env_command()
           != GSTK_MAX_PROACTIVE_COMMAND) {
         /* Set the signal to send the envelope */
         (void)rex_set_sigs(&gstk_tcb,  GSTK_SEND_POSTPONE_ENVELOPE_SIG);
       }

    }
    /* no more proactive command pending -> build exit proactive command session
    request to clients */
    if (!report->rpt.terminal_response.proactive_cmd_pending) {
      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
          /* build the terminal profile response */
          task_cmd->cmd.proactive_cmd.message_header.command_group = GSTK_PROACTIVE_CMD;
          task_cmd->cmd.proactive_cmd.message_header.command_id = 0; /* doesn't matter */
          task_cmd->cmd.proactive_cmd.message_header.user_data = report->user_data;

          /* initialize to 0 */
          memset(task_cmd->cmd.proactive_cmd.data, 0x00, GSTK_MAX_DATA_SIZE);

          task_cmd->cmd.proactive_cmd.data[0] = GSTK_PROACTIVE_SIM_COMMAND_TAG;
          task_cmd->cmd.proactive_cmd.data[1] = 5;
          task_cmd->cmd.proactive_cmd.data[5] = GSTK_CMD_STK_END_OF_PROACTIVE_SES;
          task_cmd->cmd.proactive_cmd.data[GSTK_GENERATED_END_SES_INDEX] = GSTK_GENERATED_END_SES;
          /* set GSTK_CMD_Q_SIG and put on GSTK queue */
          gstk_task_put_cmd_buf(task_cmd);
      }
      else {
          // build error message to GSTK?????????????
          ERR("No more gstk buf", 0, 0, 0);
      }
    }
}/*lint !e818 *report suppression for externalized function */
/* gstk_uim_terminal_response_report */

/*===========================================================================
FUNCTION gstk_util_error_check_terminal_rsp

DESCRIPTION
  This function will perform some error checking to ensure that
  the Application sending the Terminal Response has an
  opportunity to resend the Terminal Response incase it is
  invalid

PARAMETERS
  terminal_response_enum: [Input] Used to indicate which Terminal Response
                                   to check.
  general_result:         [Input] Used to check the result of the command
  *other_info             [Input] Union of Proactive Commands
                                  Poll Interval
                                  Get Inkey
                                  Get Input
                                  Select Item
                                  Setup Call
                                  Send SS
                                  Send USSD



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
gstk_status_enum_type gstk_util_error_check_terminal_rsp_other_info (
    gstk_cmd_enum_type                          terminal_response_enum,
    gstk_general_result_enum_type               general_result,
    const gstk_terminal_rsp_extra_param_type   *other_info,
    const gstk_additional_info_ptr_type        *additional_result
)
{
    gstk_status_enum_type gstk_status = GSTK_SUCCESS;

    if(other_info == NULL)
    {
      MSG_ERROR("other_info ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }


    switch(terminal_response_enum) {

    case GSTK_POLL_INTERVAL_CNF:
      switch(general_result) {
      case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
      case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
      case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
      case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
      case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        /* check for input length */
        if(!other_info->present) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
        else { /* other info present, check input len */
          if(!other_info->extra_param.poll_duration.present) {
            return GSTK_EXTRA_PARAM_MISSING;
          }
          else {
            if(other_info->extra_param.poll_duration.units != GSTK_DURATION_MINUTES &&
               other_info->extra_param.poll_duration.units != GSTK_DURATION_SECONDS &&
               other_info->extra_param.poll_duration.units != GSTK_DURATION_TENTHS_OF_SECONDS) {
              return GSTK_BAD_PARAM;
            }
          }
        }
        break;
      default: /* other error cases */
        if(other_info->present) {
          return GSTK_EXTRA_PARAM_NOT_REQUIRED;
        }
        break;
      }
      break;

    case GSTK_SETUP_CALL_CNF:
        break;
    case GSTK_SEND_SS_CNF:
        break;
    case GSTK_SEND_USSD_CNF:
        break;

    case GSTK_SELECT_ITEM_CNF:
      switch(general_result) {
      case GSTK_HELP_INFORMATION_REQUIRED_BY_THE_USER:
      case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
      case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
      case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
      case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
      case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        /* check for input length */
        if(!other_info->present) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
        break;
      default: /* other error cases */
        if(other_info->present) {
          return GSTK_EXTRA_PARAM_NOT_REQUIRED;
        }
        break;
      }
      break;

    case GSTK_GET_INKEY_CNF:
    case GSTK_GET_INPUT_CNF:
    case GSTK_PROVIDE_LANG_INFO_CNF:
#ifdef FEATURE_CAT_REL6
    case GSTK_PROVIDE_BATT_STATUS_CNF:
#endif /* #ifdef FEATURE_CAT_REL6 */
    case GSTK_SEND_DATA_CNF:
    case GSTK_GET_CH_STATUS_CNF:
    case GSTK_RECEIVE_DATA_CNF:
      switch(general_result) {
      case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
      case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
      case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
      case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
      case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        /* check for input length */
        if(!other_info->present) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
        break;
      case GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR:
       if(additional_result != NULL) 
       {
          if(additional_result->length == 0)
          {
            MSG_HIGH("No other_info",0,0,0);
            return GSTK_EXTRA_PARAM_MISSING;
          } 
        }
        else
        {
          MSG_HIGH("No other_info",0,0,0);
          return GSTK_EXTRA_PARAM_MISSING;
        }
        break;
      default: /* other error cases */
        if (other_info->present
#ifdef FEATURE_CAT_REL6
           /* GET INKEY may have other info */
           && (terminal_response_enum != GSTK_GET_INKEY_CNF)
#endif /* FEATURE_CAT_REL6 */
           )
        {
            return GSTK_EXTRA_PARAM_NOT_REQUIRED;
        }
        break;
      }
      break;

    case GSTK_OPEN_CH_CNF:  /* at least have bearer and buffer size */
      if(!other_info->present) {
        if (general_result != GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME) {
          return GSTK_EXTRA_PARAM_MISSING;
        }
      }
      break;

    default:
      MSG_ERROR("unknown confirm type, %d", terminal_response_enum, 0, 0);
      break;

    }

    return gstk_status;
}

/*===========================================================================
FUNCTION gstk_unsupported_unknown_cnf

DESCRIPTION
  This function packs the Terminal response for an unsupported or unknown 
  proactive command the command to the card through the UIM server.

PARAMETERS
  gstk_unsupported_unknown_cnf: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_unsupported_unknown_cnf (
  gstk_unsupported_proactive_cmd_cnf_type* unsupported_unknown_cnf )
{
  gstk_status_enum_type                     gstk_status    = GSTK_SUCCESS;
  uim_cmd_type                              *uim_cmd_ptr   = NULL;
  uint32                                     term_resp_len = 0;
  int32                                      offset        = 0;
  int                                        i             = 0;
  gstk_cmd_term_rsp_common_type              mandatory_tlv_info;

  MSG_HIGH("IN gstk_unsupported_unknown_cnf ",0,0,0);

  if(unsupported_unknown_cnf == NULL)
  {
    MSG_ERROR("gstk_unsupported_unknown_cnf  ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  uim_cmd_ptr = gstk_get_uim_buf();

  if(uim_cmd_ptr == NULL)
  {
    ERR ("no UIM cmd buf", 0, 0, 0);
    return GSTK_MEMORY_ERROR;
  }

  gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F,  &uim_cmd_ptr);
  if (gstk_status != GSTK_SUCCESS) 
  {
    /* Release the upper cmd layer memory and the uim_cmd_ptr */
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type*)((void*)unsupported_unknown_cnf));/*lint !e826 area too small */
    return GSTK_ERROR;
  }

  /* set user_data */
  uim_cmd_ptr->hdr.user_data               = 0;
  /* populate the terminal_response info */

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */

  offset = 0;

  /* Populate
   ** Command details tlv
   ** Device identity tlv
   ** Result tlv 
  */

  /* Command details info */
  mandatory_tlv_info.cmd_info_data.ref_id           = unsupported_unknown_cnf->cmd_details_ref_id;
  mandatory_tlv_info.cmd_info_data.command_rsp_type =
    (uint32)GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF;
  mandatory_tlv_info.cmd_info_data.command_number   = unsupported_unknown_cnf->command_number;

  /* device ID */
  mandatory_tlv_info.dev_id_data.src = GSTK_ME_DEVICE;
  mandatory_tlv_info.dev_id_data.dest = GSTK_UICC_SIM_DEVICE;

  /* result tag */
  mandatory_tlv_info.result_info_data.command_result = unsupported_unknown_cnf->command_result;
  mandatory_tlv_info.result_info_data.result_additional_info.length = 0;
  mandatory_tlv_info.result_info_data.result_additional_info.additional_info_ptr = NULL;

  /* Populate the command details, device identity and the result tlvs */
  gstk_status = gstk_util_populate_term_rsp_common_tlv(
                  offset,
                  &term_resp_len,
                  mandatory_tlv_info,
                  uim_cmd_ptr->terminal_response.data);

  if (gstk_status != GSTK_SUCCESS) 
  {
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type*)((void*)unsupported_unknown_cnf));/*lint !e826 area too small */
    gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
    uim_cmd_ptr = NULL;
    return GSTK_ERROR;
  }

  offset = offset + uint32toint32(term_resp_len);

  /* populate the terminal_response info - total number of bytes*/
  uim_cmd_ptr->terminal_response.num_bytes = uint32touint8(term_resp_len);

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  MSG_HIGH("Uknown Unsupported command: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
  for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) 
  {
    MSG_HIGH("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
  }

  /* clear the command detail table reference slot before since we are about to reply to
  the card */
  gstk_util_cmd_details_ref_table_free_slot(unsupported_unknown_cnf->cmd_details_ref_id);

  MSG_HIGH("Sending terminal response cmd to uim server..",0,0,0);
            gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */

  return gstk_status;
  
} /* gstk_unsupported_unknown_cnf */

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

/*===========================================================================
FUNCTION gstk_get_term_rsp_timer_exp_error_code

DESCRIPTION
  This function returns appropriate terminal response error code when a proactive
  command timer expires. If the proactive command whose timer expires is 
  DISPLAY TEXT, GET INKEY, GET INPUT or SELECT ITEM, the function returns 
  GSTK_NO_RESPONSE_FROM_USER; otherwise, the function returns 
  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND.

PARAMETERS
  cmd_type: [Input] Type of command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_NO_RESPONSE_FROM_USER,
    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_general_result_enum_type gstk_get_term_rsp_timer_exp_error_code (
  gstk_cmd_enum_type cmd_type
)
{
  switch (cmd_type)
  {
  case GSTK_DISPLAY_TEXT_REQ:
  case GSTK_GET_INKEY_REQ:
  case GSTK_GET_INPUT_REQ:
  case GSTK_SELECT_ITEM_REQ:
    return GSTK_NO_RESPONSE_FROM_USER;
  default:
    return GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  }
} /* gstk_get_term_rsp_timer_exp_error_code */

/*===========================================================================
FUNCTION gstk_process_term_rsp_timer_expire

DESCRIPTION
  For each proactive command sent to client, there is a timer associated with
  it. The corresponding terminal response has to be received within this period.
  And if the timer expires before the terminal response is revceived, this function
  will free up the entry in command details reference table and send "No Response" 
  terminal response down to UIM server.

PARAMETERS
  ref_id: [Input] The reference id of the entry in command_details_ref_table 
                  whose timer expired.

DEPENDENCIES
  None

RETURN VALUE
  GSTK_SUCCESS,
  GSTK_ERROR,
  GSTK_CLIENT_NOT_REGISTERED,
  GSTK_MEMORY_ERROR,
  GSTK_BAD_PARAM,
  GSTK_NULL_INPUT_PARAM,
  GSTK_PARAM_EXCEED_BOUNDARY,
  GSTK_INVALID_LENGTH,
  GSTK_EXTRA_PARAM_MISSING,
  GSTK_EXTRA_PARAM_NOT_REQUIRED,
  GSTK_UNSUPPORTED_COMMAND,
  GSTK_UNKNOWN_COMMAND,
  GSTK_INSUFFICIENT_INFO,
  GSTK_NOT_INIT

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_process_term_rsp_timer_expire (
  uint32  ref_id
)
{
  gstk_status_enum_type              gstk_status    = GSTK_SUCCESS;
  gstk_cmd_enum_type                 cmd_type       = GSTK_CMD_ENUM_MAX;
  gstk_additional_info_type          additional_info;
  gstk_terminal_rsp_extra_param_type extra_param;

  if (ref_id >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    MSG_ERROR("Invalid timer ref id %d!", ref_id, 0, 0);
    return GSTK_BAD_PARAM;
  }
  
  additional_info.length = 0;
  extra_param.present    = FALSE;

  gstk_status = gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type(
    command_details_ref_table[ref_id].command_details.type_of_command,
    command_details_ref_table[ref_id].command_details.qualifier,
    &cmd_type);
  if (gstk_status != GSTK_SUCCESS)
  {
    MSG_ERROR("Failed to convert stk cmd to gstk cmd!", 0, 0, 0);
    return GSTK_ERROR;
  }

  /* clean up memory if there is any */
  switch (command_details_ref_table[ref_id].command_details.type_of_command)
  {
  case GSTK_CMD_STK_SET_UP_EVENT_LIST:
#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
    /* set pending_response to 1 so that we can send terminal response below */
    command_details_ref_table[ref_id].pending_responses = 1;
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */
    break;

  default:
    /* do nothing */
    break;
  }

  /* send NO RESPONSE FROM USER terminal response */
  gstk_status = gstk_send_terminal_response(
                  GSTK_CLIENT_ID,
                  0,  /* user data doesn't matter */
                  ref_id,
                  command_details_ref_table[ref_id].command_details.command_number,
                  command_details_ref_table[ref_id].expected_cmd_rsp_type,
                  gstk_get_term_rsp_timer_exp_error_code(cmd_type),
                  &additional_info,
                  &extra_param);
  if (gstk_status != GSTK_SUCCESS)
  {
    MSG_ERROR("Failed to send NO RESPONSE term rsp!", 0, 0, 0);
    return gstk_status;
  }

  return GSTK_SUCCESS;
} /* gstk_process_term_rsp_timer_expire */
