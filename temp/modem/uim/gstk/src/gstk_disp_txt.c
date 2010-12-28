/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   D I S P L A Y   T E X T


GENERAL DESCRIPTION
  This source file contains functions to handle display text proactive
  command


FUNCTIONS
  gstk_display_text_req
    This function parses the Display Text Proactive Command from the card
    and sends it to the client.

  gstk_display_text_cnf
    This function packs the Display Text terminal response and send
    the command to UIM server.

  gstk_display_text_cnf_cleanup
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

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_disp_txt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
05/03/06   tml     lint
04/04/06   sk      Added support for duration object in Display Txt command.
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Memory leak fix
05/26/04   tml     Ensure successfully send TR to card when parsing error 
                   occurs.  Added More debug messages 
09/03/03   tml     Change source device ID to ME_DEVICE
05/16/03   tml     linted
04/29/03   tml     Properly initialize data struct
04/17/03   tml     lint and Immediate Response fix
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/11/03   tml     Fixed current offset calculation in display text request
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
FUNCTION gstk_display_text_req_cleanup

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
static void gstk_display_text_req_cleanup(
  gstk_cmd_from_card_type *request_ptr,
  uint8                   *cmd_data_ptr)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_display_text_req_cleanup */

/*===========================================================================
FUNCTION gstk_display_text_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  display_text_cnf_ptr: [Input] Pointer to message that is required to
                                be processed and sent to UIM
  STK_response_ptr    : [Input] Pointer to the STK intermediate message

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
static void gstk_display_text_cnf_cleanup(
  gstk_display_text_cnf_type          *display_text_cnf_ptr,
  display_text_terminal_response_type *STK_response_ptr)
{
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK Response ERR:NULL",0,0,0);
    return;
  }

  if(display_text_cnf_ptr == NULL)
  {
    MSG_ERROR("display_text_cnf_ptr ERR:NULL",0,0,0);
    return;
  }

  /* free display text cnf and display tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)((void*)display_text_cnf_ptr));

  if (STK_response_ptr->data.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->data.result.additional_info);
    STK_response_ptr->data.result.additional_info = NULL;
  }
} /* gstk_display_text_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_display_text_parsing_err_term_rsp

DESCRIPTION
  This function receive terminal response as a result of parsing error.

PARAMETERS
  cmd_details_ref_id  : [Input] command details ref id   
  result              : [Input] GSTK result
  additional_info_ptr : [Input] additional info pointer

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
static void gstk_display_text_parsing_err_term_rsp(
  uint32                              cmd_details_ref_id,
  gstk_general_result_enum_type       result,
  const gstk_additional_info_type    *additional_info_ptr)
{
  gstk_status_enum_type               gstk_status       = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param = {0};

  MSG_ERROR("Receive Data TLV parsing error", 0, 0, 0);

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* Send Terminal Response to Card */
  /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
  gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID,     /* client_id */
                                            0,                  /* user_data */
                                            cmd_details_ref_id, /* command_details_ref */
                                            command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                            GSTK_DISPLAY_TEXT_CNF,   /* command response */
                                            result, /* general_result */
                                            additional_info_ptr,   /* additional result */
                                            &dummy_extra_param  /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    MSG_ERROR("TR fail, %d",gstk_status,0,0);
  }    
} /* gstk_display_text_parsing_err_term_rsp */

/*===========================================================================
FUNCTION gstk_display_text_req

DESCRIPTION
  This function parses the Display Text Proactive Command from the card
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
gstk_status_enum_type gstk_display_text_req  (
  gstk_proactive_cmd_type* command_ptr)
{

  uint8                              *cmd_data_ptr         = NULL;
  int32                               cmd_data_len         = 0;
  uint32                              cmd_details_ref_id   = 0;
  int32                               curr_offset          = 0;
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  int32                               new_offset           = 0;
  gstk_additional_info_type           additional_info      = {0};
  boolean                             more_tlv             = FALSE;

  /* Cannot do {0x00} in declaration because of compilation warning 
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  MSG_HIGH("IN GSTK_DISPLAY_TEXT_REQ ",0,0,0);

  if(!command_ptr)
  {
    MSG_ERROR("command_ptr is NULL",0,0,0);
    return GSTK_NULL_INPUT_PARAM;
  }

  /* initialize request to 0 */
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

  /* There exists mandatory tlv, so cmd_data_ptr should not be null */
  if ((gstk_status != GSTK_SUCCESS) || (cmd_data_ptr == NULL)) {
    gstk_display_text_parsing_err_term_rsp(cmd_details_ref_id, 
                                           GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                           &additional_info);
    /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
    gstk_display_text_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
  
  /* build the proactive cmd request into the gstk_exp.h _req structure */

  /* Message Header */
  request.hdr_cmd.command_id = GSTK_DISPLAY_TEXT_REQ;
  request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

  /* command number */
  request.hdr_cmd.command_number =
      command_details_ref_table[cmd_details_ref_id].command_details.command_number;

  /* Priority */
  /* Command details, qualifier tells us the priority */
  if(command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x01)
  {
    request.cmd.display_text_pro_cmd_req.high_priority = TRUE;
    MSG_MED(" .... Priority HIGH", 0, 0, 0);
  }
  else
  {
    request.cmd.display_text_pro_cmd_req.high_priority  = FALSE;
    MSG_MED(" .... Priority NORMAL", 0, 0, 0);
  }

  /* User control */
  /* Command details, qualifier tells us if the user is to clear the screen */
  if(command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x80)
  {
    request.cmd.display_text_pro_cmd_req.user_control = TRUE;
    MSG_MED(" .... User Clears", 0, 0, 0);
  }
  else
  {
    request.cmd.display_text_pro_cmd_req.user_control  = FALSE;
    MSG_MED(" .... Timer Clears", 0, 0, 0);
  }

  /* Text String: DCS, length and text */
  gstk_status = gstk_parser_text_string_tlv(cmd_data_ptr,
                                            cmd_data_len,
                                            curr_offset,
                                            GSTK_TEXT_STRING_TAG,
                                            &(request.cmd.display_text_pro_cmd_req.text_string),
                                            &new_offset,
                                            &more_tlv,
                                            MANDATORY,
                                            &cmd_details_ref_id);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_display_text_parsing_err_term_rsp(cmd_details_ref_id, 
                                           GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                           &additional_info);
    /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
    gstk_display_text_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
    
  /* Initialize the icon and immediate_response to FALSE */
  request.cmd.display_text_pro_cmd_req.icon.present = FALSE;
  request.cmd.display_text_pro_cmd_req.immediate_response_required = FALSE;

  if (more_tlv) 
  { /* there are more tlv */
    curr_offset = new_offset;

    /* Icon ID */
    gstk_status = gstk_parser_icon_id_tlv(
        &(request.cmd.display_text_pro_cmd_req.text_string),
        cmd_data_ptr,
        cmd_data_len,
        curr_offset,
        &(request.cmd.display_text_pro_cmd_req.icon),
        &new_offset,
        &more_tlv,
        OPTIONAL,
        &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS) {
      gstk_display_text_parsing_err_term_rsp(cmd_details_ref_id, 
                                           GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                           &additional_info);
      /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
      gstk_display_text_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
      
    curr_offset = new_offset;

    /* if tag value doesn't match Icon ID tag, continue since there are more optional
    ** tag to be tested */

    /* Optional Immediate Response TLV */
    if (more_tlv) 
    {
      if(gstk_valid_tag(cmd_data_ptr[curr_offset], GSTK_IMMEDIATE_RESPONSE_TAG)) 
      {
        curr_offset++; /* more to the length field */
        /* there is immediate tlv */
        request.cmd.display_text_pro_cmd_req.immediate_response_required = TRUE;
        MSG_MED(" .... Immediate Response specified", 0, 0, 0);

        curr_offset = curr_offset + cmd_data_ptr[curr_offset] + 1;
        if (curr_offset == cmd_data_len) {
          more_tlv = FALSE;
        }

      } /* optional Immediate Response */

      /* if tag value doesn't match Duration object tag, continue since there are more optional
      ** tag to be tested */

#ifdef FEATURE_CAT_REL6
      /* Optional Duration object TLV */
      if (more_tlv) 
      {
        /* optional duration tlv */
        gstk_status = gstk_parser_duration_tlv( cmd_data_ptr,
                                                cmd_data_len,
                                                curr_offset,
                                                &(request.cmd.display_text_pro_cmd_req.duration),
                                                &new_offset,
                                                &more_tlv,
                                                OPTIONAL,
                                                &cmd_details_ref_id);
        if(gstk_status != GSTK_SUCCESS) 
        {
          gstk_display_text_parsing_err_term_rsp(cmd_details_ref_id, 
                                                 GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                                 &additional_info);
          /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
          gstk_display_text_req_cleanup(&request, cmd_data_ptr);
          return gstk_status;
        }
        curr_offset = new_offset;
      } /* Optional Duration object TLV */
#endif /* #ifdef FEATURE_CAT_REL6 */
    } /* no more TLV after duration object TLV */
  }
 
  /* Send command to clients */
  gstk_status = gstk_util_send_message_to_clients(&request);

  if(gstk_status != GSTK_SUCCESS)
  {
    /* Send Terminal Response to Card */
    /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
    MSG_HIGH("Display Text not able to send to client", 0, 0, 0);

    /* Send Terminal Response to Card */
    gstk_set_additional_info(
      &additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);

    gstk_display_text_parsing_err_term_rsp(
      cmd_details_ref_id, 
      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
      &additional_info);
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

  /* release memory allocation for text string if not null */
  gstk_display_text_req_cleanup(&request, cmd_data_ptr);
  return(gstk_status);

} /* gstk_display_text_req*/

/*===========================================================================
FUNCTION gstk_display_text_cnf

DESCRIPTION
  This function packs the Display Text terminal response and send
  the command to UIM server.

PARAMETERS
  display_text_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_display_text_cnf (
  gstk_display_text_cnf_type* display_text_cnf_ptr )
{

  gstk_status_enum_type                     gstk_status  = GSTK_SUCCESS;
  display_text_terminal_response_type       STK_response = {0};
  uim_cmd_type                             *uim_cmd_ptr  = NULL;
  int                                       offset       = 0;
  int                                       i            = 0;
  uint32                                    ref_id       = 0;

  MSG_HIGH("IN GSTK_DISPLAY_TEXT_CNF ",0,0,0);

  if(display_text_cnf_ptr == NULL)
  {
    MSG_ERROR("Display Text Cnf ERR: NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(display_text_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      display_text_cnf_ptr->cmd_details_ref_id,
      display_text_cnf_ptr->message_header.command_id,
      display_text_cnf_ptr->command_number,
      &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_display_text_cnf_cleanup(display_text_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_display_text_cnf_cleanup(display_text_cnf_ptr, &STK_response);
    return gstk_status;
  }


  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      display_text_cnf_ptr->command_result,
      display_text_cnf_ptr->result_additional_info,
      &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_display_text_cnf_cleanup(display_text_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
  */
  STK_response.num_bytes = STK_response.data.command_details.length +
                           STK_response.data.device_id.device_tag_length +
                           STK_response.data.result.result_tag_length +
                           6;


  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      if (uim_cmd_ptr != NULL) {
        ref_id = display_text_cnf_ptr->cmd_details_ref_id;

        /* clear the timer for client response */
        (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(display_text_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = 0;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                        &STK_response.data.command_details,
                        offset,
                        (STK_response.data.command_details.length + 2),
                        UIM_MAX_CHARS,
                        sizeof(gstk_command_details_tlv_type));
        offset = (int)(offset + STK_response.data.command_details.length + 2);
        /* copy device */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                        &STK_response.data.device_id,
                        offset,
                        (STK_response.data.device_id.device_tag_length + 2),
                        UIM_MAX_CHARS,
                        sizeof(gstk_device_identities_tag_tlv_type));
        offset = (int)(offset + STK_response.data.device_id.device_tag_length + 2);
        /* copy result */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                        &STK_response.data.result,
                        offset,
                        3,/* result tag +  length + general result */
                        UIM_MAX_CHARS,
                        sizeof(gstk_result_tag_tlv_type));
        offset = offset + 3;
        if(STK_response.data.result.result_tag_length > 1 ) { /* has additional info */
          gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                        STK_response.data.result.additional_info,
                        offset,
                        (STK_response.data.result.result_tag_length - 1),
                        UIM_MAX_CHARS,
                        (display_text_cnf_ptr->result_additional_info.length * 
                        sizeof(byte)));
          offset = (int)(offset + STK_response.data.result.result_tag_length - 1);
        }

        MSG_MED("Display Text: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
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
  
  gstk_display_text_cnf_cleanup(display_text_cnf_ptr, &STK_response);
  return gstk_status;
} /* gstk_display_text_cnf */



