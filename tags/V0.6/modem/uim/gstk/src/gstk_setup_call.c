/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E T U P  C A L L


GENERAL DESCRIPTION
  This source file contains functions to process setup call
  proactive command


FUNCTIONS
  gstk_setup_mo_call_req
    This function parses the Setup call Proactive Command from the card
    and sends it to the client.

  gstk_setup_mo_call_cnf
    This function packs the setup call terminal response and sends
    the command to UIM server.

  gstk_setup_mo_call_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_setup_call.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Centralize release memory condition in the request function
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Memory leak issue fix
05/26/04   tml     Ensure successfully send TR to card when parsing error 
                   occurs.  Added More debug messages 
09/12/03   tml     Changed Source from network to ME
05/16/03   tml     Linted
04/29/03   tml     Properly initialize data struct
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

#include "err.h"
#include "string.h"
#include "tmc.h"

/*===========================================================================
FUNCTION gstk_setup_mo_call_req_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  request_ptr : [Input] Pointer to gstk_cmd_from_card_type
  cmd_data_ptr: [Input] Pointer to cmd_data_ptr

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
static void gstk_setup_mo_call_req_cleanup(
  gstk_cmd_from_card_type *request_ptr, 
  uint8                   *cmd_data_ptr)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_setup_mo_call_req_cleanup */

/*===========================================================================
FUNCTION gstk_setup_mo_call_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  setup_call_cnf_ptr: [Input] Pointer to message that is required to
                              be processed and sent to UIM
  STK_response_ptr      : [Input] Pointer to the STK intermediate message

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
static void gstk_setup_mo_call_cnf_cleanup(
  gstk_setup_call_cnf_type                        * setup_call_cnf_ptr,
  setup_call_send_ss_terminal_response_type       * STK_response_ptr,
  boolean                                           has_cc,
  boolean                                           has_ussd)
{
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK_response_ptr ERR:NULL",0,0,0);
    return;
  }
  if(setup_call_cnf_ptr == NULL)
  {
    MSG_ERROR("setup_call_cnf_ptr ERR:NULL",0,0,0);
    return;
  }
  /* free the text string byte pointer from STK_response_ptr and setup_call_cnf_ptr */
  /* STK_response_ptr:
        cc_req_action: ussd string, alpha
        text string 2
        result 2's additional info */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)setup_call_cnf_ptr);/*lint !e826 area too small */

  /* has CC */
  if (has_cc) {
    if (STK_response_ptr->cc_req_action.alpha.alpha != NULL) {
      gstk_free(STK_response_ptr->cc_req_action.alpha.alpha);
      STK_response_ptr->cc_req_action.alpha.alpha = NULL;
    }
    if (STK_response_ptr->result2_for_cc.additional_info != NULL) {
      gstk_free(STK_response_ptr->result2_for_cc.additional_info);
      STK_response_ptr->result2_for_cc.additional_info = NULL;
    }
  
    if (has_ussd) {
      /* there is ussd string */
      if (STK_response_ptr->cc_req_action.gstk_address_string_tlv.ussd_string.text != NULL) {
        gstk_free(STK_response_ptr->cc_req_action.gstk_address_string_tlv.ussd_string.text);
        STK_response_ptr->cc_req_action.gstk_address_string_tlv.ussd_string.text = NULL;
      }
      if (STK_response_ptr->cc_mod_text2.text != NULL) {
        gstk_free(STK_response_ptr->cc_mod_text2.text);
        STK_response_ptr->cc_mod_text2.text= NULL;
      }
    }
  }
   
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
  }

} /* gstk_setup_mo_call_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_setup_mo_call_req

DESCRIPTION
  This function parses the Set Up Call Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
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
gstk_status_enum_type gstk_setup_mo_call_req  (gstk_proactive_cmd_type* command_ptr)
{

  uint8                              *cmd_data_ptr         = NULL;
  int32                               cmd_data_len         = 0;
  uint32                              cmd_details_ref_id   = 0;
  int32                               curr_offset          = 0;
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  boolean                             error_already_raised = FALSE;
  int32                               new_offset           = 0;
  gstk_additional_info_type           additional_info      = {0};
  gstk_terminal_rsp_extra_param_type  dummy_extra_param    = {0};
  boolean                             more_tlv             = TRUE;

  /* Cannot do {0x00} in declaration because of compilation warning 
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  MSG_HIGH(" IN GSTK_SETUP_CALL_REQ ",0,0,0);

  if(command_ptr == NULL)
  {
    MSG_ERROR("command_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize request */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );
  if((gstk_status == GSTK_SUCCESS) && (cmd_data_ptr != NULL))
  {
    /* build the proactive cmd request into the gstk_exp.h _req structure */

    /* Message Header */
    request.hdr_cmd.command_id = GSTK_SETUP_CALL_REQ;
    request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

    /* command number */
    request.hdr_cmd.command_number =
        command_details_ref_table[cmd_details_ref_id].command_details.command_number;

    /* setup call requirement */
    switch(command_details_ref_table[cmd_details_ref_id].command_details.qualifier) {
    case 0x00:
    case 0x01:
      request.cmd.setup_call_pro_cmd_req.call_setup_requirement = GSTK_NO_OTHER_CALLS;
      MSG_MED(" .... no other calls", 0, 0, 0);
      break;
    case 0x02:
    case 0x03:
      request.cmd.setup_call_pro_cmd_req.call_setup_requirement = GSTK_HOLD_ACTIVE_CALLS;
      MSG_MED(" .... hold other calls", 0, 0, 0);
      break;
    case 0x04:
    case 0x05:
      request.cmd.setup_call_pro_cmd_req.call_setup_requirement = GSTK_DISCONNECT_ACTIVE_CALLS;
      MSG_MED(" .... disconnect other calls", 0, 0, 0);
      break;
    default:
      MSG_MED(" .... unknown setup call requirement", 0 , 0, 0);
      break;
    }

    /* redial required? */
    switch(command_details_ref_table[cmd_details_ref_id].command_details.qualifier) {
    case 0x00:
    case 0x02:
    case 0x04:
      request.cmd.setup_call_pro_cmd_req.redial.necessary = FALSE;
      MSG_MED(" .... NO Redial", 0, 0, 0);
      break;
    case 0x01:
    case 0x03:
    case 0x05:
      request.cmd.setup_call_pro_cmd_req.redial.necessary = TRUE;
      MSG_MED(" .... Redial ", 0, 0, 0);
      break;
    default:
      MSG_MED(" .... unknown redial requirement", 0 , 0, 0);
      break;
    }

    /* initialize all optional field to not present */
    request.cmd.setup_call_pro_cmd_req.capability_config.length = 0;
    request.cmd.setup_call_pro_cmd_req.destination_subaddress.length = 0;
    request.cmd.setup_call_pro_cmd_req.redial.timeout.present = FALSE;
    request.cmd.setup_call_pro_cmd_req.setup_call_display_alpha.length = 0;
    request.cmd.setup_call_pro_cmd_req.setup_call_display_icon.present = FALSE;
    request.cmd.setup_call_pro_cmd_req.user_cnf_alpha.length = 0;
    request.cmd.setup_call_pro_cmd_req.user_cnf_icon.present = FALSE;

    /* alpha user confirmation (O)
      address
      cap configuration (O)
      subaddress (O)
      duration (O)
      icon id user confirmation (O)
      alpha call setup phase (O)
      icon id call setup phase (O)
    */

    /* optional user confirmation alpha */
    gstk_status = gstk_parser_alpha_identifier_tlv(cmd_data_ptr,
                                              cmd_data_len,
                                              curr_offset,
                                              &(request.cmd.setup_call_pro_cmd_req.user_cnf_alpha),
                                              &new_offset,
                                              &more_tlv,
                                              OPTIONAL,
                                              &cmd_details_ref_id);

    if (gstk_status != GSTK_SUCCESS || !more_tlv) { /* since there is at least 1 more mandatory tlv */
      error_already_raised = TRUE;
    }
    else {
      curr_offset = new_offset;
    }
    
    /* mandatory address */
    if (!error_already_raised ) {
      gstk_status = gstk_parser_address_ss_string_tlv(
                                                GSTK_ADDRESS_TAG,
                                                cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &(request.cmd.setup_call_pro_cmd_req.address),
                                                &new_offset,
                                                &more_tlv,
                                                MANDATORY,
                                                &cmd_details_ref_id);

      if (gstk_status != GSTK_SUCCESS) {
        error_already_raised = TRUE;
      }
      else {
        if (more_tlv) {
          curr_offset = new_offset;
          /*optional cap config */
          gstk_status = gstk_parser_cap_config_tlv( cmd_data_ptr,
                                                    cmd_data_len,
                                                    curr_offset,
                                                    &(request.cmd.setup_call_pro_cmd_req.capability_config),
                                                    &new_offset,
                                                    &more_tlv,
                                                    OPTIONAL,
                                                    &cmd_details_ref_id);

          if (gstk_status != GSTK_SUCCESS) {
            error_already_raised = TRUE;
          }
          else {
            if (more_tlv){
              curr_offset = new_offset;
              /*optional subaddress */
              gstk_status = gstk_parser_subaddress_tlv(
                                        cmd_data_ptr,
                                        cmd_data_len,
                                        curr_offset,
                                        &(request.cmd.setup_call_pro_cmd_req.destination_subaddress),
                                        &new_offset,
                                        &more_tlv,
                                        OPTIONAL,
                                        &cmd_details_ref_id);

              if (gstk_status != GSTK_SUCCESS) {
                error_already_raised = TRUE;
              }
              else {
                if (more_tlv){
                  curr_offset = new_offset;
                  /*optional duration */
                  gstk_status = gstk_parser_duration_tlv(
                                            cmd_data_ptr,
                                            cmd_data_len,
                                            curr_offset,
                                            &(request.cmd.setup_call_pro_cmd_req.redial.timeout),
                                            &new_offset,
                                            &more_tlv,
                                            OPTIONAL,
                                            &cmd_details_ref_id);

                  if (gstk_status != GSTK_SUCCESS) {
                    error_already_raised = TRUE;
                  }
                  else {
                    if (more_tlv){
                      curr_offset = new_offset;
                      /*optional user confirm icon */
                      gstk_status = gstk_parser_icon_id_tlv(
                                                &(request.cmd.setup_call_pro_cmd_req.user_cnf_alpha),
                                                cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &(request.cmd.setup_call_pro_cmd_req.user_cnf_icon),
                                                &new_offset,
                                                &more_tlv,
                                                OPTIONAL,
                                                &cmd_details_ref_id);

                      if (gstk_status != GSTK_SUCCESS) {
                        error_already_raised = TRUE;
                      }
                      else {
                        if (more_tlv){
                          curr_offset = new_offset;
                          /*optional setup call alpha */
                          gstk_status = gstk_parser_alpha_identifier_tlv(
                                                    cmd_data_ptr,
                                                    cmd_data_len,
                                                    curr_offset,
                                                    &(request.cmd.setup_call_pro_cmd_req.setup_call_display_alpha),
                                                    &new_offset,
                                                    &more_tlv,
                                                    OPTIONAL,
                                                    &cmd_details_ref_id);

                          if (gstk_status != GSTK_SUCCESS) {
                            error_already_raised = TRUE;
                          }
                          else {
                            if (more_tlv){
                              curr_offset = new_offset;
                              /*optional setup call icon */
                              gstk_status = gstk_parser_icon_id_tlv(
                                                        &(request.cmd.setup_call_pro_cmd_req.setup_call_display_alpha),
                                                        cmd_data_ptr,
                                                        cmd_data_len,
                                                        curr_offset,
                                                        &(request.cmd.setup_call_pro_cmd_req.setup_call_display_icon),
                                                        &new_offset,
                                                        &more_tlv,
                                                        OPTIONAL,
                                                        &cmd_details_ref_id);

                              if (gstk_status != GSTK_SUCCESS || more_tlv) {
                                /* this is the last tlv, if the more_tlv is TRUE, => error */
                                error_already_raised = TRUE;
                              }
                            } /* more tlv, try call setup icon */
                          } /* no error after setup call display alpha or there is no display alpha */
                        } /* more tlv, try call setup alpha */
                      } /* no error after user confirm icon or there is no user confirm icon */
                    } /*  more tlv, try user confirmation icon */
                  } /* no error after duration parsing or there is no duration */
                } /* more tlv, try duration parsing next */
              } /* no error after subaddress parsing or there is no subaddress */
            } /* more tlv, try subaddress parsing next */
          } /* no error after cap config parsing or if there is no cap config present */
        } /*more tlv after address parsing*/
      } /* no error from address parsing */
    } /* no error from user confirmation alpha */
  } /* preprocess command success */
  else { /* preprocess_cmd returns errors */
      error_already_raised = TRUE;
  } /* GSTK_ERROR from proprocess_cmd */

  /* No error has been encountered */
  if(!error_already_raised) {
    /* Send command to clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if(gstk_status != GSTK_SUCCESS)
    {
      /* Send Terminal Response to Card */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      MSG_HIGH("SetUp call send client error", 0, 0, 0);

      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

      gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                  0,              /* user_data */
                                  cmd_details_ref_id, /* command_details_ref */
                                  command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                  GSTK_SETUP_CALL_CNF, /* command response */
                                  GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND, /* general_result */
                                  &additional_info, /* additional result */
                                  &dummy_extra_param /* extra param */ );
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
      }
    }
    else
    {
      /* Start Timer */
      gstk_util_start_client_response_timer(cmd_details_ref_id);
      /* reset gstk_status to SUCCESS since there has been no error*/
      /* This is needed since some of the TLVs are optional, which might
      ** cause the various parsing functions to return invalid_command */
      gstk_status = GSTK_SUCCESS;
    }
  }
  else { /* error, free alpha string if created already */
    ERR("setup call parsing err", 0, 0, 0);
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                0,              /* user_data */
                                cmd_details_ref_id, /* command_details_ref */
                                command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                GSTK_SETUP_CALL_CNF, /* command response */
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                                &additional_info, /* additional result */
                                &dummy_extra_param /* extra param */ );
    if(gstk_status != GSTK_SUCCESS) {
      MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
    }
  } /* if error_already_raised */

  /* release memory allocation for alpha string if not null */
  gstk_setup_mo_call_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /* gstk_setup_mo_call_req */

/*===========================================================================
FUNCTION gstk_setup_mo_call_cnf

DESCRIPTION
  This function packs the Set Up Call terminal response and send
  the command to UIM server.

PARAMETERS
  setup_call_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_setup_mo_call_cnf (
  gstk_setup_call_cnf_type* setup_call_cnf_ptr )
{

  gstk_status_enum_type                     gstk_status     = GSTK_SUCCESS;
  setup_call_send_ss_terminal_response_type STK_response    = {0};
  uim_cmd_type                              *uim_cmd_ptr    = NULL;
  uint8                                     header_len      = 0;
  boolean                                   has_ussd_string = FALSE;
  boolean                                   has_cc          = FALSE;
  int                                       offset          = 0;
  int                                       i               = 0;
  uint32                                    ref_id          = 0;

  MSG_HIGH("IN GSTK setup call cnf ",0,0,0);

  if(setup_call_cnf_ptr == NULL)
  {
    MSG_ERROR("setup_call_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(setup_call_send_ss_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      setup_call_cnf_ptr->cmd_details_ref_id,
      setup_call_cnf_ptr->message_header.command_id,
      setup_call_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, FALSE, FALSE);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, FALSE, FALSE);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      setup_call_cnf_ptr->command_result,
      setup_call_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, FALSE, FALSE);
    return gstk_status;
  }

  /* header_len:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  header_len = STK_response.header.command_details.length +
                          STK_response.header.device_id.device_tag_length +
                          STK_response.header.result.result_tag_length +
                          6;

  /* Pack extra param if presented */
  if (setup_call_cnf_ptr->extra_param.has_cc_type_modification) {
    /* extra param present due to call control type change */
    /* CC requested action */

    /* memalloc alpha and ussd string inside the packing function */
    gstk_status = gstk_packer_cc_req_action_tlv(
      &(setup_call_cnf_ptr->extra_param.cc_req_action),
      &STK_response.cc_req_action);

    if (gstk_status != GSTK_SUCCESS) {
      if (setup_call_cnf_ptr->extra_param.cc_req_action.call_type == GSTK_USSD) {
        gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, TRUE, TRUE);
      }
      else {
        gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, TRUE, FALSE);
      }
      return gstk_status;
    }

    /* result object 2 */
    gstk_status = gstk_packer_result_tlv(
      setup_call_cnf_ptr->extra_param.command_result2,
      setup_call_cnf_ptr->extra_param.additional_info2_ptr,
      &STK_response.result2_for_cc);

    if (gstk_status != GSTK_SUCCESS) {
      if (setup_call_cnf_ptr->extra_param.cc_req_action.call_type == GSTK_USSD) {
        gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, TRUE, TRUE);
      }
      else {
        gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, TRUE, FALSE);
      }
      return gstk_status;
    }

    /* length == header len +
                cc requested action + 2
                result object 2 + 2
                */
    STK_response.num_bytes = header_len +
                            STK_response.cc_req_action.length +
                            STK_response.result2_for_cc.result_tag_length + 4;
    has_cc = TRUE;

    if (setup_call_cnf_ptr->extra_param.cc_req_action.call_type == GSTK_USSD) {
      /* text string2 required if change to USSD */
      STK_response.cc_mod_text2.text = (uint8*)gstk_malloc(
                                       setup_call_cnf_ptr->extra_param.text_string2.length);
      
      if (STK_response.cc_mod_text2.text == NULL) {
        gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, has_cc, TRUE);
        return gstk_status;
      }

      gstk_status = gstk_packer_text_string_tlv(
          GSTK_TEXT_STRING_TAG,
          setup_call_cnf_ptr->extra_param.text_string2.dcs,
          setup_call_cnf_ptr->extra_param.text_string2.length,
          setup_call_cnf_ptr->extra_param.text_string2.text,
          &STK_response.cc_mod_text2);
      
      if (gstk_status != GSTK_SUCCESS) {
        gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, has_cc, TRUE);
        return gstk_status;
      }
      
      /* length == length from above +
                  text string for changing to USSD + 2 */
      STK_response.num_bytes += STK_response.cc_mod_text2.length + 2;
      has_ussd_string = TRUE;
    }
  }
  else { /* not extra param */
    STK_response.num_bytes = header_len;
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        ref_id = setup_call_cnf_ptr->cmd_details_ref_id;

        /* clear the timer for client response */
        (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(setup_call_cnf_ptr->cmd_details_ref_id);

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
                          (setup_call_cnf_ptr->result_additional_info.length * 
                           sizeof(byte)));
          offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
        }

        if (has_cc) {
          /* there are CC extra param */
          /* copy call control requested action tlv */

          /* copy in separate section */
          uim_cmd_ptr->terminal_response.data[header_len++] = STK_response.cc_req_action.tag;
          uim_cmd_ptr->terminal_response.data[header_len++] = STK_response.cc_req_action.length;
          
          if(!has_ussd_string) {
            if((tags_enum_type)STK_response.cc_req_action.gstk_address_string_tlv.ss_string.tag == GSTK_SS_STRING_TAG) {
              /* has ss_string */
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                &STK_response.cc_req_action.gstk_address_string_tlv.ss_string,
                header_len,
                (STK_response.cc_req_action.gstk_address_string_tlv.ss_string.length + 2),
                UIM_MAX_CHARS,
                sizeof(gstk_address_tag_tlv_type));
              header_len = header_len +
                STK_response.cc_req_action.gstk_address_string_tlv.ss_string.length + 2;
            }
          } /* ! ussd_string */
          else {
          if ((tags_enum_type)STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.tag == GSTK_USSD_STRING_TAG) {
            /* has ussd string */
            gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                               &STK_response.cc_req_action.gstk_address_string_tlv.ussd_string,
                               header_len,
                               3, /* tag, length and dcs */
                               UIM_MAX_CHARS,
                               sizeof(gstk_string_tlv_type));
            header_len += 3;
            if(STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.length > 1) {
              gstk_byte_offset_memcpy(&uim_cmd_ptr->terminal_response.data[header_len],
                                 STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.text,
                                 header_len,
                                 (STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.length - 1),
                                 UIM_MAX_CHARS,
                                 sizeof(gstk_string_tlv_type));
              header_len = header_len +
                STK_response.cc_req_action.gstk_address_string_tlv.ussd_string.length - 1;
            }
          }
          }
          if ((tags_enum_type)STK_response.cc_req_action.alpha.tag == GSTK_ALPHA_IDENTIFIER_TAG) {
            /* copy alpha tag, length */
            gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                               &STK_response.cc_req_action.alpha,
                               header_len,
                               2,
                               UIM_MAX_CHARS,
                               sizeof(gstk_alpha_tlv_type));
            header_len += 2;
            /* copy the alpha text */
            if(STK_response.cc_req_action.alpha.length > 0 ) {
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 STK_response.cc_req_action.alpha.alpha,
                                 header_len,
                                 (STK_response.cc_req_action.alpha.length),
                                 UIM_MAX_CHARS,
                                 setup_call_cnf_ptr->extra_param.cc_req_action.length);
              header_len += STK_response.cc_req_action.alpha.length;
            }
          }
          if ((tags_enum_type)STK_response.cc_req_action.bc_repeat_ind.tag == GSTK_BC_REPEAT_INDICATOR_TAG) {
            /* has bc repeat indicator */
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 &STK_response.cc_req_action.bc_repeat_ind,
                                 header_len,
                                 (STK_response.cc_req_action.bc_repeat_ind.length + 2),
                                 UIM_MAX_CHARS,
                                 sizeof(gstk_bc_repeat_ind_tlv_type));
              header_len = header_len +
                STK_response.cc_req_action.bc_repeat_ind.length + 2;
          }

          /* copy result object 2 tlv */
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                             &STK_response.result2_for_cc,
                             offset,
                             3,
                             UIM_MAX_CHARS,
                             sizeof(gstk_result_tag_tlv_type));
          header_len += 3;
          if(STK_response.result2_for_cc.result_tag_length > 1) {
            gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                               STK_response.result2_for_cc.additional_info,
                               offset,
                               (STK_response.result2_for_cc.result_tag_length - 1),
                               UIM_MAX_CHARS,
                               (setup_call_cnf_ptr->extra_param.additional_info2_ptr.length *
                               sizeof(byte)));  
           header_len = header_len + STK_response.result2_for_cc.result_tag_length - 1;
          }

          /* if there is text string 2 */
          if (has_ussd_string) {
            /* copy text string 2 */
            /* copy text tag, length */
            gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                               &STK_response.cc_mod_text2,
                               offset,
                               3,
                               UIM_MAX_CHARS,
                               sizeof(gstk_string_tlv_type));
            header_len += 3;
            /* copy the text */
            if(STK_response.cc_mod_text2.length > 1 ) {
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                 STK_response.cc_mod_text2.text,
                                 offset,
                                 (STK_response.cc_mod_text2.length - 1),
                                 UIM_MAX_CHARS,
                                 setup_call_cnf_ptr->extra_param.text_string2.length);
            }
          }
        }

        MSG_MED("Set Up Call: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
        for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) {
          MSG_MED("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
        }

        gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */

      }
      else {
        MSG_MED("Never here", 0, 0, 0);
      }
    }
  } /* uim_cmd_ptr != NULL */
  else {
    MSG_ERROR ("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_setup_mo_call_cnf_cleanup(setup_call_cnf_ptr, &STK_response, has_cc, has_ussd_string);
  return gstk_status;
} /* gstk_setup_mo_call_cnf */

