/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   G E T   I N P U T


GENERAL DESCRIPTION
  This source file contains functions to process get input proactive command


FUNCTIONS
  gstk_get_input_req
    This function parses the Get Input Proactive Command from the card
    and sends it to the client.

  gstk_get_input_cnf
    This function packs the get input terminal response and send
    the command to UIM server.

  gstk_get_input_cnf_cleanup
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

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_get_input.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/09   yb      Fixed response data for UCS2 DCS for Get Input command
09/05/08   yb      Fixed lint error
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
08/20/08   yb      fixed possible buffer overflow
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Added Null ptr checks
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
04/04/06   sk      Changed name for get input terminal response type
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
02/09/05   sk      Added support for conversion from ascii text to sms default
                   in gstk_get_input_cnf
08/13/04   tml     Centralize release memory condition in the request function
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Memory leak fix
06/10/04   tml     Added pack or unpacked user input indication
05/26/04   tml     Ensure successfully send TR to card when parsing error 
                   occurs.  Added More debug messages 
09/03/03   tml     Added packing functionality
05/16/03   tml     linted
04/29/03   tml     Properly initialized data structure
04/17/03   tml     lint
03/20/03   tml     Set the offset correctly when sending the information
                   to uim driver
03/19/03   tml     Set extra param presence to FALSE when GSTK encountered
                   error in parsing the proactive command
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/11/03   tml     Fixed current offset calculation in get input request
02/07/03   tml     Initial Version

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

/*===========================================================================
FUNCTION gstk_get_input_req_cleanup

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
static void gstk_get_input_req_cleanup(
  gstk_cmd_from_card_type *request_ptr, 
  uint8                   *cmd_data_ptr)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_get_input_req_cleanup */

/*===========================================================================
FUNCTION gstk_get_input_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  get_input_cnf_ptr: [Input] Pointer to message that is required to
                             be processed and sent to UIM
  STK_response_ptr : [Input] Pointer to the STK intermediate message
  raw_text_ptr     : [Input] Pointer to the raw text temporary buffer
  sms_text_ptr     : [Input] Pointer to sms text

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
static void gstk_get_input_cnf_cleanup(
  gstk_get_input_cnf_type                *get_input_cnf_ptr,
  gstk_get_input_terminal_response_type  *STK_response_ptr,
  uint8                                  *raw_text_ptr,
  uint8                                  *sms_text_ptr)
{
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK_response_ptr ERR:NULL",0,0,0);
    return;
  }
  if(get_input_cnf_ptr == NULL)
  {
    MSG_ERROR("get_input_cnf_ptr ERR:NULL",0,0,0);
    return;
  }
  /* free the text string byte pointer from STK_response_ptr and get_input_cnf_ptr */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)get_input_cnf_ptr);/*lint !e826 area too small */
  if (raw_text_ptr != NULL) {
    gstk_free(raw_text_ptr);
  }
  if (sms_text_ptr != NULL) {
    gstk_free(sms_text_ptr);
  }
  if (STK_response_ptr->text_string.text != NULL) {
    gstk_free(STK_response_ptr->text_string.text);
    STK_response_ptr->text_string.text = NULL;
  }
  if (STK_response_ptr->header.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->header.result.additional_info);
    STK_response_ptr->header.result.additional_info = NULL;
  }
} /* gstk_get_input_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_get_input_parsing_err_term_rsp

DESCRIPTION
  This function receive terminal response as a result of parsing error.

PARAMETERS
  cmd_details_ref_id: [Input]   

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
static void gstk_get_input_parsing_err_term_rsp( 
  uint32                            cmd_details_ref_id,
  gstk_general_result_enum_type     result,
  const gstk_additional_info_type   *additional_info_ptr)
{
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param;

  ERR("Receive Data TLV parsing error", 0, 0, 0);

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* Send Terminal Response to Card */
  gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID,       /* client_id */
                                            0,                    /* user_data */
                                            cmd_details_ref_id,   /* command_details_ref */
                                            command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                            GSTK_GET_INPUT_CNF,   /* command response */
                                            result,               /* general_result */
                                            additional_info_ptr,   /* additional result */
                                            &dummy_extra_param     /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    MSG_ERROR("TR fail, %d",gstk_status,0,0);
  }    
} /* gstk_get_input_parsing_err_term_rsp */

/*===========================================================================
FUNCTION gstk_get_input_req

DESCRIPTION
  This function parses the Get Input Proactive Command from the card
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
gstk_status_enum_type gstk_get_input_req (
  gstk_proactive_cmd_type* command_ptr)
{
  uint8                              *cmd_data_ptr       = NULL;
  int32                               cmd_data_len       = 0;
  uint32                              cmd_details_ref_id = 0;
  int32                               curr_offset        = 0;
  gstk_status_enum_type               gstk_status        = GSTK_SUCCESS;
  int32                               new_offset         = 0;
  gstk_additional_info_type           additional_info    = {0};
  boolean                             more_tlv           = FALSE;

  /* Cannot do {0x00} in declaration because of compilation warning 
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  MSG_HIGH(" IN GSTK_GET_INPUT_REQ ",0,0,0);

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
  ** the cmd_data in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  /* There exists mandatory tlv, so cmd_data_ptr should not be null */
  if ((gstk_status != GSTK_SUCCESS) || (cmd_data_ptr == NULL)) {
    gstk_get_input_parsing_err_term_rsp(cmd_details_ref_id, 
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
    gstk_get_input_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
  
  /* build the proactive cmd request into the gstk_exp.h _req structure */

  /* Message Header */
  request.hdr_cmd.command_id = GSTK_GET_INPUT_REQ;
  request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

  /* command number */
  request.hdr_cmd.command_number =
      command_details_ref_table[cmd_details_ref_id].command_details.command_number;

  /* Masked the user input ?  from command details */
  if ((command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x04) == 0x04) {
    request.cmd.get_input_pro_cmd_req.show_user_input = FALSE;
    MSG_MED(" .... Hidden input", 0, 0, 0);
  }
  else {
    request.cmd.get_input_pro_cmd_req.show_user_input = TRUE;
    MSG_MED(" .... Show User input", 0, 0, 0);
  }

  /* help information available */
  request.cmd.get_input_pro_cmd_req.help_available =
    ((command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x80) == 0x80);

  /* user response format:
    SMS alphabet, numerical, UCS2 */
  if ((command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x01) == 0x01) {
    /* alpha */
    if ((command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x02) == 0x02) {
      /* UCS2 */
      request.cmd.get_input_pro_cmd_req.response_format = GSTK_UCS2_ALPHABET;
      MSG_MED(" .... UCS2", 0, 0, 0);
    }
    else {
      /* SMS default */
      request.cmd.get_input_pro_cmd_req.response_format = GSTK_SMS_DEF_ALPHABET;
      MSG_MED(" .... SMS default", 0, 0, 0);
    }
  }
  else {
    /* digit */
    request.cmd.get_input_pro_cmd_req.response_format = GSTK_NUMERICAL_ONLY;
    MSG_MED(" .... Numerical Only", 0, 0, 0);
  }

  /* user input packed or unpacked */
  if ((command_details_ref_table[cmd_details_ref_id].command_details.qualifier & 0x08) == 0x08) {
    request.cmd.get_input_pro_cmd_req.respone_packing_format = GSTK_PACK_FORMAT;
  }
  else {
    request.cmd.get_input_pro_cmd_req.respone_packing_format = GSTK_UNPACK_FORMAT;
  }

  /* verify if non numerical input format + masked input => return FALSE */
  if ((request.cmd.get_input_pro_cmd_req.response_format != GSTK_NUMERICAL_ONLY) &&
      (!request.cmd.get_input_pro_cmd_req.show_user_input)) {
    MSG_ERROR(" ........ Num + Hidden => Wrong", 0,0, 0);
    gstk_get_input_parsing_err_term_rsp(cmd_details_ref_id, 
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
    gstk_get_input_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }

  /* Text String: DCS, length and text */
  gstk_status = gstk_parser_text_string_tlv(  cmd_data_ptr,
                                              cmd_data_len,
                                              curr_offset,
                                              GSTK_TEXT_STRING_TAG,
                                              &(request.cmd.get_input_pro_cmd_req.text_string),
                                              &new_offset,
                                              &more_tlv,
                                              MANDATORY,
                                              &cmd_details_ref_id);

  if (gstk_status != GSTK_SUCCESS || !more_tlv) {
    gstk_get_input_parsing_err_term_rsp(cmd_details_ref_id, 
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
    gstk_get_input_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
      
  /* Response length */
  curr_offset = new_offset;
  gstk_status = gstk_parser_response_len_tlv(
              cmd_data_ptr,
              cmd_data_len,
              curr_offset,
              &(request.cmd.get_input_pro_cmd_req.maximum_user_input),
              &(request.cmd.get_input_pro_cmd_req.minimum_user_input),
              &new_offset,
              &more_tlv,
              MANDATORY,
              &cmd_details_ref_id);
  if (gstk_status != GSTK_SUCCESS) {
    gstk_get_input_parsing_err_term_rsp(cmd_details_ref_id, 
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
    /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
    gstk_get_input_req_cleanup(&request, cmd_data_ptr);
    return gstk_status;
  }
   
  /* initialize the default text and the icon to not present */
  request.cmd.get_input_pro_cmd_req.default_text.length = 0;
  request.cmd.get_input_pro_cmd_req.icon.present = FALSE;

  if (more_tlv) {
    curr_offset = new_offset;

    /* optional default text TLV */
    /* default text */
    gstk_status = gstk_parser_text_string_tlv(cmd_data_ptr,
                                              cmd_data_len,
                                              curr_offset,
                                              GSTK_DEFAULT_TEXT_TAG,
                                              &(request.cmd.get_input_pro_cmd_req.default_text),
                                              &new_offset,
                                              &more_tlv,
                                              OPTIONAL,
                                              &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS) {
      gstk_get_input_parsing_err_term_rsp(cmd_details_ref_id, 
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
      /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
      gstk_get_input_req_cleanup(&request, cmd_data_ptr);
      return gstk_status;
    }
    
    if (more_tlv) {
      curr_offset = new_offset;
      /* Icon ID */
      gstk_status = gstk_parser_icon_id_tlv(
                    &(request.cmd.get_input_pro_cmd_req.text_string),
                    cmd_data_ptr,
                    cmd_data_len,
                    curr_offset,
                    &(request.cmd.get_input_pro_cmd_req.icon),
                    &new_offset,
                    &more_tlv,
                    OPTIONAL,
                    &cmd_details_ref_id);

      if (gstk_status != GSTK_SUCCESS ) {
        gstk_get_input_parsing_err_term_rsp(cmd_details_ref_id, 
                                        GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME,
                                        &additional_info);
        /* release memory allocation for receive data request info data and cmd_data_ptr accordingly */
        gstk_get_input_req_cleanup(&request, cmd_data_ptr);
        return gstk_status;
      }
    } /* optional Icon */
  } /* no more TLV after response len TLV */

  /* Send command to clients */
  gstk_status = gstk_util_send_message_to_clients(&request);

  if(gstk_status != GSTK_SUCCESS)
  {
    /* Send Terminal Response to Card */
    /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
    MSG_HIGH("Get Input Sending to Client Err", 0, 0, 0);

    /* Send Terminal Response to Card */
    gstk_set_additional_info(
      &additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
    /*
     * extra_param.present = FALSE;
     * extra_param.extra_param.user_get_inkey_input.dcs = 0;
     * extra_param.extra_param.user_get_inkey_input.length = 0;
     */ 
    gstk_get_input_parsing_err_term_rsp(cmd_details_ref_id, 
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
  
  /* release memory allocation for text string/default text if not null */
  gstk_get_input_req_cleanup(&request, cmd_data_ptr);
  return gstk_status;
} /* gstk_get_input_req */

/*===========================================================================
FUNCTION gstk_get_input_cnf

DESCRIPTION
  This function packs the get input terminal response and send
  the command to UIM server.

PARAMETERS
  get_input_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_get_input_cnf(
  gstk_get_input_cnf_type* get_input_cnf_ptr)
{

  gstk_status_enum_type                   gstk_status      = GSTK_SUCCESS;
  gstk_get_input_terminal_response_type   STK_response     = {0};
  uim_cmd_type                            *uim_cmd_ptr     = NULL;
  uint8                                   header_bytes     = 0;
  uint8                                   dcs              = 0;
  int                                     offset           = 0;
  int                                     i                = 0;
  uint8                                   *raw_text_ptr    = NULL;
  uint8                                   *dcs_text        = NULL;
  uint8                                   *sms_format_text = NULL;
  uint32                                   ref_id          = 0;

  MSG_HIGH("IN GSTK Get input cnf ",0,0,0);

  if(get_input_cnf_ptr == NULL)
  {
    MSG_ERROR("get_input_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(gstk_get_input_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      get_input_cnf_ptr->cmd_details_ref_id,
      get_input_cnf_ptr->message_header.command_id,
      get_input_cnf_ptr->command_number,
      &STK_response.header.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_get_input_cnf_cleanup(get_input_cnf_ptr, &STK_response, raw_text_ptr,sms_format_text);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.header.device_id );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_get_input_cnf_cleanup(get_input_cnf_ptr, &STK_response, raw_text_ptr,sms_format_text);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      get_input_cnf_ptr->command_result,
      get_input_cnf_ptr->result_additional_info,
      &STK_response.header.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_get_input_cnf_cleanup(get_input_cnf_ptr, &STK_response, raw_text_ptr,sms_format_text);
    return gstk_status;
  }

  /* Returned User Input */
  if(get_input_cnf_ptr->data_present) {
#ifdef FEATURE_GSTK_UCS2_TO_ASCII
    dcs = gstk_util_translate_qualifier_to_dcs(get_input_cnf_ptr->data_coding_scheme);
#else /* FEATURE_GSTK_UCS2_TO_ASCII */
    /* translate the qualifier into dcs */
    dcs = gstk_util_translate_qualifier_to_dcs(
          command_details_ref_table[get_input_cnf_ptr->cmd_details_ref_id].command_details.qualifier);
#endif /* FEATURE_GSTK_UCS2_TO_ASCII */

    if (get_input_cnf_ptr->returned_data_len > 0) {
      raw_text_ptr = (uint8*)gstk_malloc(get_input_cnf_ptr->returned_data_len);
      if (raw_text_ptr == NULL)
      {
        MSG_ERROR("No more memory", 0, 0, 0);
        gstk_get_input_cnf_cleanup(get_input_cnf_ptr, &STK_response, raw_text_ptr,sms_format_text);
        return GSTK_ERROR;
      }
      
      raw_text_ptr = (uint8*) memcpy(raw_text_ptr, get_input_cnf_ptr->data, get_input_cnf_ptr->returned_data_len);
      dcs_text = raw_text_ptr;

      /* Check if the format required is UCS2 or SMS 
         UCS2 - 0x08
         SMS  - 0x00 OR 0x04
      */
      if((dcs == 0x00) || (dcs == 0x04))
      {
        sms_format_text = (uint8*)gstk_malloc(
                          get_input_cnf_ptr->returned_data_len);
        if (sms_format_text == NULL)
        {
          ERR("No more memory", 0, 0, 0);
          gstk_get_input_cnf_cleanup(get_input_cnf_ptr, &STK_response, raw_text_ptr,sms_format_text);
          return GSTK_ERROR;
        }
      
        ASCIIText_ConvertTo_GSMDefault (get_input_cnf_ptr->data,
                                        get_input_cnf_ptr->returned_data_len,
                                        sms_format_text);
        dcs_text = sms_format_text;

      }/* if((dcs == 0x00) || (dcs == 0x04)) */
      
      if (command_details_ref_table[get_input_cnf_ptr->cmd_details_ref_id].command_details.qualifier & 0x08)
      { /* packing is required */
        get_input_cnf_ptr->returned_data_len =
          uint16touint8(
            gstk_util_pack_7_bit_chars(
              get_input_cnf_ptr->data, 
              get_input_cnf_ptr->returned_data_len, 
              dcs_text));
      }
      STK_response.text_string.text = (uint8*)gstk_malloc(
                                              get_input_cnf_ptr->returned_data_len);
      if (STK_response.text_string.text == NULL)
      {
        ERR("No more memory", 0, 0, 0);
        gstk_get_input_cnf_cleanup(get_input_cnf_ptr, &STK_response,raw_text_ptr,sms_format_text);
        return GSTK_ERROR;
      }
    }

    /* pack the user input into text string tlv */
    gstk_status = gstk_packer_text_string_tlv(
      GSTK_TEXT_STRING_TAG,
      dcs,
      get_input_cnf_ptr->returned_data_len,
      dcs_text,
      &STK_response.text_string);
  }

  /* total data length:  command details data + 2   (2 is for the tag and length field)
                      + device id data + 2
                      + result data + 2
                      + text data + 2
  */
  header_bytes =           STK_response.header.command_details.length +
                          STK_response.header.device_id.device_tag_length +
                          STK_response.header.result.result_tag_length +
                          6;
  if(get_input_cnf_ptr->data_present) {
    STK_response.num_bytes = header_bytes +
                        STK_response.text_string.length +
                        GSTK_TAG_LENGTH_LEN;
  }
  else {
    STK_response.num_bytes = header_bytes;
  }


  /* Send response to STK application on SIM */
  /* Send Message to UIM */
  /* get a buffer for the request */
  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr != NULL ) {
    gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_TERMINAL_RESPONSE_F, &uim_cmd_ptr);
    if (gstk_status == GSTK_SUCCESS) {
      ref_id = get_input_cnf_ptr->cmd_details_ref_id;

      /* clear the timer for client response */
      (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

      /* clear the command detail table reference slot before since we are about to reply to
        the card */
      gstk_util_cmd_details_ref_table_free_slot(get_input_cnf_ptr->cmd_details_ref_id);

      /* set user_data */
      uim_cmd_ptr->hdr.user_data               = 0;
      /* populate the terminal_response info */
      uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;
      if (STK_response.text_string.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
        uim_cmd_ptr->terminal_response.num_bytes++;
      }

      /* copy command details */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                       &STK_response.header.command_details,
                       offset,
                       (STK_response.header.command_details.length + GSTK_TAG_LENGTH_LEN),
                       UIM_MAX_CHARS,
                       sizeof(gstk_command_details_tlv_type));
      offset = (int)(offset + STK_response.header.command_details.length
                      + GSTK_TAG_LENGTH_LEN);
      /* copy device */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                       &STK_response.header.device_id,
                       offset,
                       (STK_response.header.device_id.device_tag_length + GSTK_TAG_LENGTH_LEN),
                       UIM_MAX_CHARS,
                       sizeof(gstk_device_identities_tag_tlv_type));
      offset = (int)(offset + STK_response.header.device_id.device_tag_length
              + GSTK_TAG_LENGTH_LEN);
      /* copy result */
      gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                       &STK_response.header.result,
                       offset,
                       GSTK_TAG_LENGTH_LEN + 1, /* result tag +  length + general result */
                       UIM_MAX_CHARS,
                       sizeof(gstk_result_tag_tlv_type));
      offset = offset + GSTK_TAG_LENGTH_LEN + 1;
      if(STK_response.header.result.result_tag_length > 1 ) { /* has additional info */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                         STK_response.header.result.additional_info,
                         offset,
                         (STK_response.header.result.result_tag_length - 1),
                         UIM_MAX_CHARS,
                         (get_input_cnf_ptr->result_additional_info.length * 
                         sizeof(byte)));
        offset = (int)(offset + STK_response.header.result.result_tag_length - 1);
      }

      /* copy user input */
      if (get_input_cnf_ptr->data_present) { /* has text */
        if (offset < UIM_MAX_CHARS) {
          uim_cmd_ptr->terminal_response.data[offset++] = STK_response.text_string.tag;
        }
        else {
          MSG_ERROR("Trying to access invalid index: %d, max index allowed: %d",
                    offset, UIM_MAX_CHARS, 0);
        }
        
        if(STK_response.text_string.length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
          if (offset < UIM_MAX_CHARS) {
            uim_cmd_ptr->terminal_response.data[offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
          }
          else {
            MSG_ERROR("Trying to access invalid index: %d, max index allowed: %d",
                      offset, UIM_MAX_CHARS, 0);
          }
        }
        if ((offset + 1) < UIM_MAX_CHARS) {
          uim_cmd_ptr->terminal_response.data[offset++] = STK_response.text_string.length;
          uim_cmd_ptr->terminal_response.data[offset++] = STK_response.text_string.data_coding_scheme;
        }
        else {
          MSG_ERROR("Trying to access invalid index: %d, max index allowed: %d",
                    offset, UIM_MAX_CHARS, 0);
        }

        if (get_input_cnf_ptr->returned_data_len != 0) {
           gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                            STK_response.text_string.text,
                            offset,
                            get_input_cnf_ptr->returned_data_len,
                            UIM_MAX_CHARS,
                            get_input_cnf_ptr->returned_data_len);
                                                               
          offset = (int)(offset + get_input_cnf_ptr->returned_data_len);
        }
      }

      MSG_MED("Get Input: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
      for(i = 0; i< uim_cmd_ptr->terminal_response.num_bytes; i++) {
        MSG_MED("uim_cmd_ptr[%d] = 0x%x", i, uim_cmd_ptr->terminal_response.data[i], 0);
      }
      gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
    }
  } /* uim_cmd_ptr != NULL */
  else {
    MSG_ERROR ("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }
  
  gstk_get_input_cnf_cleanup(get_input_cnf_ptr, &STK_response,raw_text_ptr,sms_format_text);
  return gstk_status;

} /* gstk_get_input_cnf*/

