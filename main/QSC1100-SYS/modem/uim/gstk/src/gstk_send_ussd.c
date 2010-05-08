/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E N D   U S S D


GENERAL DESCRIPTION
  This source file contains functions to process send USSD
  proactive command


FUNCTIONS
  gstk_send_ussd_req
    This function parses the Send USSD Proactive Command from the card
    and sends it to the client.

  gstk_send_ussd_cnf
    This function packs the send ussd terminal response and sends
    the command to UIM server.

  gstk_send_ussd_cnf_cleanup
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

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_send_ussd.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/08   yb      Removed extra checks for tr length in gstk_send_ussd_cnf
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
08/20/08   yb      Added support for longer text string in terminal response
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
01/10/05  sk/tml   Change parsing function call from text string to ussd string
08/13/04   tml     Centralize release memory condition in the request function
08/05/04   tml     Fixed send ussd result error text == NULL issue
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Memory leak fix
05/26/04   tml     Ensure successfully send TR to card when parsing error 
                   occurs.  Added More debug messages 
09/12/03   tml     Changed Source ID from Network to ME
05/16/03   tml     linted
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
FUNCTION gstk_send_ussd_req_cleanup

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
static void gstk_send_ussd_req_cleanup(
  gstk_cmd_from_card_type *request_ptr, 
  uint8                   *cmd_data_ptr)
{
  if(request_ptr == NULL)
  {
    MSG_ERROR("request_ptr ERR:NULL",0,0,0);
    return;
  }
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_send_ussd_req_cleanup */

/*===========================================================================
FUNCTION gstk_send_ussd_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  send_ussd_cnf_ptr: [Input] Pointer to message that is required to
                             be processed and sent to UIM
  STK_response_ptr : [Input] Pointer to the STK intermediate message

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
static void gstk_send_ussd_cnf_cleanup(
  gstk_send_ussd_cnf_type                * send_ussd_cnf_ptr,
  send_ussd_terminal_response_type       * STK_response_ptr,
  boolean                                  has_cc)
{
  if(send_ussd_cnf_ptr == NULL)
  {
    MSG_ERROR("send_ussd_cnf_ptr ERR:NULL",0,0,0);
    return;
  }
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK_response_ptr ERR:NULL",0,0,0);
    return;
  }
  /* free the text string byte pointer from STK_response_ptr and send_ussd_cnf_ptr */
  /* STK_response_ptr:
        text string
        cc_req_action: ussd string, alpha
        result 2's additional info */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)send_ussd_cnf_ptr);/*lint !e826 area too small */

  /* text string */
  if (STK_response_ptr->text_string.text != NULL) {
    gstk_free(STK_response_ptr->text_string.text);
    STK_response_ptr->text_string.text = NULL;
  }
  if (has_cc) {
    if (STK_response_ptr->cc_req_action.alpha.alpha != NULL) {
      gstk_free(STK_response_ptr->cc_req_action.alpha.alpha);
      STK_response_ptr->cc_req_action.alpha.alpha = NULL;
    }
    if (STK_response_ptr->result2_for_cc.additional_info != NULL) {
      gstk_free(STK_response_ptr->result2_for_cc.additional_info);
      STK_response_ptr->result2_for_cc.additional_info = NULL;
    }
  }
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info = NULL;
  }

} /* gstk_send_ussd_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_send_ussd_req

DESCRIPTION
  This function parses the Send ussd Proactive Command from the card
  and sends it to the client.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  command_ptr: [Input] Pointer to message parsed and sent to clients

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
gstk_status_enum_type gstk_send_ussd_req  (
  gstk_proactive_cmd_type* command_ptr)
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

  MSG_HIGH(" IN GSTK_SEND_ussd_REQ ",0,0,0);

  if(command_ptr == NULL)
  {
    MSG_ERROR("command ERR:NULL",0,0,0);
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
    request.hdr_cmd.command_id = GSTK_SEND_USSD_REQ;
    request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

    /* command number */
    request.hdr_cmd.command_number =
        command_details_ref_table[cmd_details_ref_id].command_details.command_number;


    /* initialize all optional field to not present */
    request.cmd.send_ussd_pro_cmd_req.alpha.length = 0;
    request.cmd.send_ussd_pro_cmd_req.icon.present = FALSE;

    /* alpha (O)
      ussd string
      icon id (O)
    */

    /* optional alpha */
    gstk_status = gstk_parser_alpha_identifier_tlv(cmd_data_ptr,
                                              cmd_data_len,
                                              curr_offset,
                                              &(request.cmd.send_ussd_pro_cmd_req.alpha),
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
    
    /* mandatory ussd string */
    if (!error_already_raised ) {
      
      gstk_status = gstk_parser_ussd_string_tlv(
        cmd_data_ptr,
        cmd_data_len,
        curr_offset,
        &(request.cmd.send_ussd_pro_cmd_req.ussd_string),
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
          /*optional icon */
          gstk_status = gstk_parser_icon_id_tlv(
                                    &(request.cmd.send_ussd_pro_cmd_req.alpha),
                                    cmd_data_ptr,
                                    cmd_data_len,
                                    curr_offset,
                                    &(request.cmd.send_ussd_pro_cmd_req.icon),
                                    &new_offset,
                                    &more_tlv,
                                    OPTIONAL,
                                    &cmd_details_ref_id);

          if (gstk_status != GSTK_SUCCESS ) {
            /* this is the last tlv, if the more_tlv is TRUE, => error */
            error_already_raised = TRUE;
          }
        } /*more tlv after ussd string parsing*/
      } /* no error from ussd string parsing */
    } /* no error from alpha */
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
      MSG_HIGH("send ussd send client err", 0, 0, 0);

      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

      gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                  0,              /* user_data */
                                  cmd_details_ref_id, /* command_details_ref */
                                  command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                  GSTK_SEND_USSD_CNF, /* command response */
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

    ERR("send ussd parse err", 0, 0, 0);
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                                0,              /* user_data */
                                cmd_details_ref_id, /* command_details_ref */
                                command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                GSTK_SEND_USSD_CNF, /* command response */
                                GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                                &additional_info, /* additional result */
                                &dummy_extra_param /* extra param */ );
    if(gstk_status != GSTK_SUCCESS) {
      MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
    }
  } /* if error_already_raised */

  /* release memory allocation for alpha string if not null */
  gstk_send_ussd_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /* gstk_send_ussd_req */

/*===========================================================================
FUNCTION gstk_send_ussd_cnf

DESCRIPTION
  This function packs the Send USSD terminal response and send
  the command to UIM server.

PARAMETERS
  send_ussd_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_send_ussd_cnf (
  gstk_send_ussd_cnf_type* send_ussd_cnf_ptr )
{

  gstk_status_enum_type                 gstk_status            = GSTK_SUCCESS;
  send_ussd_terminal_response_type      STK_response           = {0};
  uim_cmd_type                          *uim_cmd_ptr           = NULL;
  boolean                               has_cc                 = FALSE;
  int                                   offset                 = 0;
  int                                   i                      = 0;
  uint32                                ref_id                 = 0;

  MSG_HIGH("IN GSTK send ussd cnf ",0,0,0);

  if(send_ussd_cnf_ptr == NULL)
  {
    MSG_ERROR("send_ussd_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(send_ussd_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      send_ussd_cnf_ptr->cmd_details_ref_id,
      send_ussd_cnf_ptr->message_header.command_id,
      send_ussd_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, FALSE);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, FALSE);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      send_ussd_cnf_ptr->command_result,
      send_ussd_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, FALSE);
    return gstk_status;
  }

  /* STK_response.num_bytes:  command details data + 2   (2 is for the tag and length field)
                             + device id data + 2
                             + result data + 2
  */
  STK_response.num_bytes = STK_response.header.command_details.length +
                           STK_response.header.device_id.device_tag_length +
                           STK_response.header.result.result_tag_length +
                           6;

  /* text string presented */
  if (send_ussd_cnf_ptr->ussd_extra_param.data.length > 0){
    STK_response.text_string.text = (uint8*)gstk_malloc(
                                    send_ussd_cnf_ptr->ussd_extra_param.data.length);
    if (STK_response.text_string.text == NULL) {
      gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, FALSE);
      return gstk_status;
    }

    gstk_status = gstk_packer_text_string_tlv(
          GSTK_TEXT_STRING_TAG,
          send_ussd_cnf_ptr->ussd_extra_param.data.dcs,
          send_ussd_cnf_ptr->ussd_extra_param.data.length,
          send_ussd_cnf_ptr->ussd_extra_param.data.text,
          &STK_response.text_string);
  
    if (gstk_status != GSTK_SUCCESS) {
      gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, FALSE);
      return gstk_status;
    }

    /* length == length from above +
                  text string for USSD + 2 */
    STK_response.num_bytes += (STK_response.text_string.length + GSTK_TAG_LENGTH_LEN);
  }

  /* Pack extra param if presented */
  if (send_ussd_cnf_ptr->ussd_extra_param.has_cc_type_modification) {
    /* extra param present due to call control type change */
    /* CC requested action */

    /* memalloc alpha and ussd string inside the packing function */
    gstk_status = gstk_packer_cc_req_action_tlv(
      &(send_ussd_cnf_ptr->ussd_extra_param.cc_req_action),
      &STK_response.cc_req_action);

    if (gstk_status != GSTK_SUCCESS) {
      gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, TRUE);
      return gstk_status;
    }

    /* result object 2 */
    gstk_status = gstk_packer_result_tlv(
      send_ussd_cnf_ptr->ussd_extra_param.command_result2,
      send_ussd_cnf_ptr->ussd_extra_param.additional_info2_ptr,
      &STK_response.result2_for_cc);

    if (gstk_status != GSTK_SUCCESS) {
      gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, TRUE);
      return gstk_status;
    }

    /* length == cc requested action + 2
                 result object 2 + 2
                */
    STK_response.num_bytes += STK_response.cc_req_action.length +
                              STK_response.result2_for_cc.result_tag_length + 4;
    has_cc = TRUE;
  }

  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        ref_id = send_ussd_cnf_ptr->cmd_details_ref_id;

        /* clear the timer */
        (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(send_ussd_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = 0;
        /* populate the terminal_response info */

        if (STK_response.text_string.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
          STK_response.num_bytes++;
        }
        if(STK_response.num_bytes > (GSTK_MAX_DATA_SIZE-1))
        {
          gstk_status = gstk_fill_error_response(uim_cmd_ptr,
                                                 STK_response.header,
                                                 offset);
        }
        else
        {
          uim_cmd_ptr->terminal_response.num_bytes = (byte)STK_response.num_bytes;            
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
                                  (GSTK_TAG_LENGTH_LEN + GSTK_GEN_RESULT_LEN), /* result tag +  length + general result */
                                  UIM_MAX_CHARS,
                                  sizeof(gstk_result_tag_tlv_type));
          offset = offset + 3;
          if(STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
            gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                    STK_response.header.result.additional_info,
                                    offset,
                                    (STK_response.header.result.result_tag_length - 1), 
                                    UIM_MAX_CHARS,
                                    (send_ussd_cnf_ptr->result_additional_info.length * 
                                    sizeof(byte)));
            offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
          }

          /* copy text string */
          if(STK_response.text_string.length > 0) {
            /* copy the text string tag */
              uim_cmd_ptr->terminal_response.data[offset++] = STK_response.text_string.tag;

            /* Encoding the length on two bytes if length of string is greater than 0x7F */
            /* checking if the length of string is greater than 0x7F */
            if(STK_response.text_string.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
              uim_cmd_ptr->terminal_response.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
            }
            /* copy the length of string and data coding scheme tag */
            uim_cmd_ptr->terminal_response.data[offset++] = STK_response.text_string.length;
            uim_cmd_ptr->terminal_response.data[offset++] = STK_response.text_string.data_coding_scheme;
            /* copy the text */
            if(STK_response.text_string.length > 1) {
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      STK_response.text_string.text,
                                      offset,
                                      (STK_response.text_string.length - 1),
                                      UIM_MAX_CHARS,
                                      send_ussd_cnf_ptr->ussd_extra_param.data.length);
              /* update offset */
              offset = (int)(offset + STK_response.text_string.length -1);
            }
          }

          if (has_cc) {
            /* there are CC extra param */
            /* copy call control requested action tlv */

            /* copy in separate section */
            uim_cmd_ptr->terminal_response.data[offset++] = STK_response.cc_req_action.tag;
            uim_cmd_ptr->terminal_response.data[offset++] = STK_response.cc_req_action.length;

            if(send_ussd_cnf_ptr->ussd_extra_param.cc_req_action.call_type == GSTK_VOICE) {
              if((tags_enum_type)STK_response.cc_req_action.gstk_address_string_tlv.address.tag == GSTK_ADDRESS_TAG) {
                /* has address */
                gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                  &STK_response.cc_req_action.gstk_address_string_tlv.address,
                  offset,
                  (STK_response.cc_req_action.gstk_address_string_tlv.address.length + 2),
                  UIM_MAX_CHARS,
                  sizeof(gstk_address_tag_tlv_type));
                offset = (int)(offset + 
                          STK_response.cc_req_action.gstk_address_string_tlv.address.length + 2);
              }
              if((tags_enum_type)STK_response.cc_req_action.subaddress.tag == GSTK_CALLED_PARTY_SUB_ADDRESS_TAG) {
                /* has subaddr */
                gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.cc_req_action.subaddress,
                                        offset,
                                        (STK_response.cc_req_action.subaddress.length + 2),
                                        UIM_MAX_CHARS,
                                        sizeof(gstk_subaddress_tlv_type));
                offset = (int)(offset + 
                  STK_response.cc_req_action.subaddress.length + 2);
              }
              if((tags_enum_type)STK_response.cc_req_action.ccp1.tag == GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
                /* has ccp1 */
                gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.cc_req_action.ccp1,
                                        offset,
                                        (STK_response.cc_req_action.ccp1.length + 2),
                                        UIM_MAX_CHARS,
                                        sizeof(gstk_cap_config_tlv_type));
                offset = (int)(offset + 
                  STK_response.cc_req_action.ccp1.length + 2);

              }
              if((tags_enum_type)STK_response.cc_req_action.ccp2.tag == GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG) {
                /* has ccp2 */
                gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                        &STK_response.cc_req_action.ccp2,
                                        offset,
                                        (STK_response.cc_req_action.ccp2.length + 2),
                                        UIM_MAX_CHARS,
                                        sizeof(gstk_cap_config_tlv_type));
                offset = (int)(offset + 
                  STK_response.cc_req_action.ccp2.length + 2);
              }
            } /* voice */
            else { /* SS */
            if((tags_enum_type)STK_response.cc_req_action.gstk_address_string_tlv.ss_string.tag == GSTK_SS_STRING_TAG) {
                /* has ss_string */
                gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                  &STK_response.cc_req_action.gstk_address_string_tlv.ss_string,
                  offset,
                  (STK_response.cc_req_action.gstk_address_string_tlv.ss_string.length + 2),
                  UIM_MAX_CHARS,
                  sizeof(gstk_address_tag_tlv_type));
                offset = (int)(offset + 
                  STK_response.cc_req_action.gstk_address_string_tlv.ss_string.length + 2);
              }
            }
            if ((tags_enum_type)STK_response.cc_req_action.alpha.tag == GSTK_ALPHA_IDENTIFIER_TAG) {
              /* copy alpha tag, length */
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                      &STK_response.cc_req_action.alpha,
                                      offset,
                                      2,
                                      UIM_MAX_CHARS,
                                      sizeof(gstk_alpha_tlv_type));
              offset += 2;
              /* copy the alpha text */
              if(STK_response.cc_req_action.alpha.length > 0 ){
                gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                         STK_response.cc_req_action.alpha.alpha,
                         offset,
                         (STK_response.cc_req_action.alpha.length),
                         UIM_MAX_CHARS,
                         send_ussd_cnf_ptr->ussd_extra_param.cc_req_action.length);
                offset = (int)(offset + STK_response.cc_req_action.alpha.length);
              }
            }
            if ((tags_enum_type)STK_response.cc_req_action.bc_repeat_ind.tag == GSTK_BC_REPEAT_INDICATOR_TAG) {
              /* has bc repeat indicator */
                gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                         &STK_response.cc_req_action.bc_repeat_ind,
                         offset,
                         (STK_response.cc_req_action.bc_repeat_ind.length + 2),
                         UIM_MAX_CHARS,
                         sizeof(gstk_bc_repeat_ind_tlv_type));
                offset = (int)(offset + 
                  STK_response.cc_req_action.bc_repeat_ind.length + 2);
            }

            /* copy result object 2 tlv */
            gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                    &STK_response.result2_for_cc,
                                    offset,
                                    3,
                                    UIM_MAX_CHARS,
                                    sizeof(gstk_result_tag_tlv_type));
            offset += 3;
            if(STK_response.result2_for_cc.result_tag_length > 1) {
              gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                 STK_response.result2_for_cc.additional_info,
                 offset,
                 (STK_response.result2_for_cc.result_tag_length - 1),
                 UIM_MAX_CHARS,
                 (send_ussd_cnf_ptr->ussd_extra_param.additional_info2_ptr.length *
                  sizeof(byte)));
              offset = (int)(offset + STK_response.result2_for_cc.result_tag_length - 1);
            }
          }
        }

        MSG_MED("USSD: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
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

  gstk_send_ussd_cnf_cleanup(send_ussd_cnf_ptr, &STK_response, has_cc);
  return gstk_status;
} /* gstk_send_ussd_cnf */

