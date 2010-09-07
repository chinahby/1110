/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    G S T K   S E T U P   E V E N T   L I S T


GENERAL DESCRIPTION
  This source file contains functions to process setup event list
  proactive command


FUNCTIONS
  gstk_setup_evt_list_req
    This function parses the Setup event list Proactive Command from the card
    and sends it to the client.

  gstk_setup_evt_list_cnf
    This function packs the setup event list terminal response and sends
    the command to UIM server.

  gstk_setup_evt_list_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_setup_evt_list.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/07/09   yb      Added a condition to check invalid TLV while sending 
                   terminal response for Setup event list
03/18/09   yb      Fixed condition for checking length while sending terminal
                   response for Setup event list
01/16/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
10/24/08   xz      Fix an issue of not send SETUP EVT LIST command to client
                   in GSTK format when registered via new API.
09/14/08   sk      Fixed lint errors
09/08/08   sk      Fixed indentation
08/21/08   sk      Added GSTK enhanced registration support
08/08/08   xz      Add support for raw set up event list command
11/13/07   tml     pipe the setup event list commands to client and wait for
                   responses from client before sending TR
04/23/07   nk      fixed possible buffer overflow
04/18/07   sk      send notification to clients only if relevant bit in the 
                   set up evt list has changed from the last received command.
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/24/06   sk      Lint fixes
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mode
04/04/06   sk      Added support for 1X related to location information
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
09/20/05   tml     RRC callback registration and hanlding to allow proper
                   cell id caching and location status event when cell ID 
                   changed during normal service
07/13/05   sk      Added support for Access Technology
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Added SAT/USAT data support
08/13/04   tml     Centralize release memory condition in the request function
08/03/04   tml     Memory leak issue fix
05/26/04   tml     Added More debug messages, added language selection 
                   and browser termination, user activity and idle screen
                   user notification supports
04/16/04   tml     linted
05/16/03   tml     linted
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
#include "gstk_envelope_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

#include "err.h"
#include "string.h"
#include "cm.h"
#include "tmc.h"

/*===========================================================================

                     GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
FUNCTION gstk_setup_evt_list_req_cleanup

DESCRIPTION
  This function clean up any stack allocated pointer.

PARAMETERS
  request: [Input] Pointer to gstk_cmd_from_card_type
  cmd_data: [Input] Pointer to cmd_data

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
static void gstk_setup_evt_list_req_cleanup(
  gstk_cmd_from_card_type *request, 
  uint8                   *cmd_data)
{
  gstk_util_release_card_cmd_memory(request);
  if (cmd_data != NULL) {
    gstk_free(cmd_data);
  }
} /* gstk_setup_evt_list_req_cleanup */

/*===========================================================================
FUNCTION gstk_setup_evt_list_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  setup_evt_list_cnf: [Input] Pointer to message that is required to
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
static void gstk_setup_evt_list_cnf_cleanup(
  gstk_setup_evt_list_cnf_type                * setup_evt_list_cnf,
  setup_evt_list_terminal_response_type       * STK_response)
{
  if(STK_response == NULL)
  {
    MSG_ERROR(" STK_response ERR:NULL",0,0,0);
    return;
  }

  if(setup_evt_list_cnf == NULL)
  {
    MSG_ERROR(" setup_evt_list_cnf ERR:NULL",0,0,0);
    return;
  }
  /* free setup event list cnf and tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)setup_evt_list_cnf);/*lint !e826 area too small */
  if (STK_response->data.result.additional_info != NULL) {
    gstk_free(STK_response->data.result.additional_info);
    STK_response->data.result.additional_info = NULL;
  }

} /* gstk_setup_evt_list_cnf_cleanup */

/*===========================================================================
FUNCTION gstk_send_setup_evt_list_to_client

DESCRIPTION
  This function sends a event in setup event list proactive either in GSTK
  format or raw format based on the type of the client. Also, if it sends
  raw setup event list proactive command to a client, the corresponding
  bit in the flag pointed by cli_rvcd_raw_cmd_flg_ptr will be set.

PARAMETERS
  pro_cmd_ptr:              [Input]        Pointer to setup event list proactive
                                           command
  gstk_msg_ptr:             [Input]        Pointer to gstk event message
  cmd_details_ref_id:       [Input]        The reference id of command details 
                                           reference table
  cli_rvcd_raw_cmd_flg_ptr: [Input/Output] Pointer to the flag where the bit will
                                           be set if its corresponding client 
                                           received raw proactive command

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
static gstk_status_enum_type gstk_send_setup_evt_list_to_client(
  gstk_proactive_cmd_type *pro_cmd_ptr,
  gstk_cmd_from_card_type *gstk_msg_ptr,
  uint32                  cmd_details_ref_id,
  uint32                  *cli_rcvd_raw_cmd_flg_ptr
)
{
  gstk_status_enum_type                     gstk_status = GSTK_ERROR;
  gstk_client_id_type                       client_id   = 0;
  gstk_client_pro_cmd_data_format_enum_type data_format = GSTK_FORMAT_NONE;
  gstk_client_pro_cmd_reg_data_type         pro_cmd_msg_data;
  gstk_raw_cmd_from_card_type               raw_cmd_fmt;
  uint32                                    cli_bitmask = 1;

  if ( (pro_cmd_ptr == NULL)
    || (gstk_msg_ptr == NULL)
    || (cli_rcvd_raw_cmd_flg_ptr == NULL))
  {
    MSG_ERROR("null pointer passed in: 0x%x 0x%x 0x%x!",
      pro_cmd_ptr,
      gstk_msg_ptr,
      cli_rcvd_raw_cmd_flg_ptr);
    return GSTK_NULL_INPUT_PARAM;
  }

  memset(&pro_cmd_msg_data, 0, sizeof(gstk_client_pro_cmd_reg_data_type));
  memset(&raw_cmd_fmt, 0, sizeof(gstk_raw_cmd_from_card_type));

  /* find the client who has registered for this event */
  gstk_status = gstk_util_find_client_with_registered_event(
                   &client_id,
                   gstk_msg_ptr->hdr_cmd.command_id,
                   GSTK_GSTK_FORMAT,
                   GSTK_HANDLE_ALL_FUNC);

  if (gstk_status == GSTK_NO_CLIENT_REG_FOUND)
  {
#if defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT)
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_ STREAMING_APDU_SUPPORT) */
      MSG_HIGH("no client registered for event", 0, 0, 0);
      return GSTK_CLIENT_NOT_REGISTERED;
#if defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT)
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT) */
  }
  else
  {
    data_format = GSTK_GSTK_FORMAT;
  }

#if (GSTK_MAX_CLIENT > 32)
#error "The max num of GSTK client is more than 32. Need to extend cli_bitmask!"
#endif /* GSTK_MAX_CLIENT > 32 */

  cli_bitmask <<= (client_id - 1);
  
  if (gstk_client_table[client_id-1].evt_cb.new_client_cb != NULL)
  {
    if (gstk_client_table[client_id-1].evt_cb_type == GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT)
    {
#if defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT)
      if (data_format == GSTK_RAW_FORMAT)
      {
        /* check if raw cmd is already sent */
        if (((*cli_rcvd_raw_cmd_flg_ptr) & cli_bitmask) == 0)
        {
          /* construct raw format message */        
          raw_cmd_fmt.raw_hdr.cmd_ref_id = cmd_details_ref_id;
          raw_cmd_fmt.raw_hdr.user_data  = pro_cmd_ptr->message_header.user_data;
          raw_cmd_fmt.payload.data_buffer_ptr = pro_cmd_ptr->data;
          raw_cmd_fmt.payload.data_len        = uint32toint32(pro_cmd_ptr->length);
          pro_cmd_msg_data.data_format_type        = GSTK_RAW_FORMAT;
          pro_cmd_msg_data.pro_cmd_reg_data.raw_req_rsp_ptr = &raw_cmd_fmt;

          /* send to client and set the flag */
          gstk_client_table[client_id-1].evt_cb.new_client_cb(&pro_cmd_msg_data);
          *cli_rcvd_raw_cmd_flg_ptr |= cli_bitmask;
        }
      }
      else if (data_format == GSTK_GSTK_FORMAT)
      {
        pro_cmd_msg_data.data_format_type = GSTK_GSTK_FORMAT;
        pro_cmd_msg_data.pro_cmd_reg_data.gstk_req_rsp_ptr = gstk_msg_ptr;
        /* send to client and set the flag */
        gstk_client_table[client_id-1].evt_cb.new_client_cb(&pro_cmd_msg_data);
      }
      else
      {
        MSG_ERROR("Failed to dispatch evt 0x%x! Invalid data format type 0x%x!",
                  gstk_msg_ptr->hdr_cmd.command_id,
                  data_format,
                  0);
       return GSTK_ERROR;
      }

#if !defined(FEATURE_APPS_IMAGE_WINMOB) && defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT)
#error code not present
#endif /* !defined(FEATURE_APPS_IMAGE_WINMOB) && defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) */

#else /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_ STREAMING_APDU_SUPPORT) */
      (void)cli_bitmask;
      (void)data_format;
      MSG_ERROR("Enhanced Reg is not supported!", 0, 0, 0);
      return GSTK_ERROR;
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_ STREAMING_APDU_SUPPORT) */
    }
    else
    {
      gstk_util_call_client_evt_cb(client_id, gstk_msg_ptr);
#if defined(FEATURE_APPS_IMAGE_WINMOB)
      /* increment the number of client responses needed before sending TR */
      command_details_ref_table[cmd_details_ref_id].pending_responses++;
#else /* FEATURE_APPS_IMAGE_WINMOB */
      (void)cmd_details_ref_id;
#endif /* FEATURE_APPS_IMAGE_WINMOB */
    }
  }
  else
  {
    MSG_ERROR("evt cb ptr is null for cli 0x%x", client_id, 0, 0);
    return GSTK_ERROR;
  }
  
  return GSTK_SUCCESS;
} /* gstk_send_setup_evt_list_to_client */

/*===========================================================================
FUNCTION gstk_setup_evt_list_req

DESCRIPTION
  This function parses the Set Up Event List Proactive Command from the card
  and update the static event list variable and register with the correct
  modules for the event indicated.
  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

PARAMETERS
  cmd_ptr: [Input] Pointer to message parsed and sent to clients

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
gstk_status_enum_type gstk_setup_evt_list_req (gstk_proactive_cmd_type* cmd_ptr)
{
  uint8                              *cmd_data_ptr        = NULL;
  int32                               cmd_data_len        = 0;
  uint32                              cmd_details_ref_id  = GSTK_MAX_CLIENT;
  int32                               curr_offset         = 0;
  gstk_status_enum_type               gstk_status         = GSTK_SUCCESS;
  boolean                             error_already_raised= TRUE;
  int32                               new_offset          = 0;
  gstk_additional_info_type           additional_info;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param;
  boolean                             more_tlv            = FALSE;
  uint8                               new_event_list_mask[GSTK_MAX_EVENT_LIST_LEN];
  gstk_general_result_enum_type       general_result      = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
  cm_client_status_e_type             cm_reg_return_status= CM_CLIENT_ERR_NONE;
  gstk_cmd_from_card_type             request;
#if (GSTK_MAX_CLIENT <= 32)
  /* if the raw setup event list proactive command is sent to client,
   * the corresponding bit will be set
   */
  uint32                              cli_rcvd_raw_cmd_flg = 0;
#endif /* GSTK_MAX_CLIENT <= 32 */

  MSG_HIGH(" IN GSTK_SETUP_EVT_LIST_REQ ",0,0,0);

  memset(&additional_info, 0, sizeof(gstk_additional_info_type));
  memset(&dummy_extra_param, 0, sizeof(gstk_terminal_rsp_extra_param_type));
#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
  gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type *)&gstk_priv_data.setup_evt_list_cnf_cache);
  memset(&gstk_priv_data.setup_evt_list_cnf_cache, 
         0x00,
         sizeof(gstk_priv_data.setup_evt_list_cnf_cache));
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */

  /* initialize additional_info's length to 0 */
  additional_info.length = 0;

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* initialize request */
  memset(&request, 0, sizeof(gstk_evt_notify_type) + sizeof(gstk_exp_hdr_type));

  /* initialize the new event list to 0 */
  memset(new_event_list_mask, 0, GSTK_MAX_EVENT_LIST_LEN);

  /* reset error_already_raise to FALSE since gstk_validate_proactive_SIM_command returns success */
  error_already_raised = FALSE;
  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data_ptr, and specifies the length of
  ** the cmd_data_ptr in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)cmd_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  
  if((gstk_status == GSTK_SUCCESS) && (cmd_data_ptr != NULL))
  {

    /* parse the event list tlv */
    gstk_status = gstk_parser_evt_list_tlv( cmd_data_ptr,
                                            cmd_data_len,
                                            curr_offset,
                                            new_event_list_mask, /* pointer to the evt_list array */
                                            &new_offset,
                                            &more_tlv,
                                            MANDATORY,
                                            &cmd_details_ref_id);
    if (more_tlv || gstk_status != GSTK_SUCCESS ) { /* should not have any more tlvs */
      error_already_raised = TRUE;
    } /* more tlv */
  } /* GSTK_SUCCESS from preprocess_command */
  else {
    error_already_raised = TRUE;
  }

  /* No error has been encountered */
  if(!error_already_raised) {
    /* update the static event list variable */

    /* new_event_list_mask will contain the current set up evt list received from the card */
    /* gstk_evt_list will contain the last set up evt list received from the card for which
       the ME had sent a TR of success */
    /* (de)register with the corresponding modules */
    if ((new_event_list_mask[GSTK_MT_CALL] & 0x01) == 0x01) { /* mt call */
      MSG_MED("... Register MT", 0, 0, 0);
      /* register for MT call event */
      cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_REG,        /* event registration */
                          CM_CALL_EVENT_SETUP_IND,    /* from this call event */
                          CM_CALL_EVENT_SETUP_IND,    /* to this call event */
                          NULL );                     /* pointer to a cb function for errors */
      if (cm_reg_return_status != CM_CLIENT_OK) {
        gstk_status = GSTK_ERROR;
      }
    }
    else {
      MSG_MED("... Deregister MT", 0, 0, 0);
      /* deregister for MT call event */
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
      cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_DEREG,      /* event deregistration */
                          CM_CALL_EVENT_SETUP_IND,    /* from this call event */
                          CM_CALL_EVENT_SETUP_IND,    /* to this call event */
                          NULL );                     /* pointer to a cb function for errors */
      if (cm_reg_return_status != CM_CLIENT_OK) {
        gstk_status = GSTK_ERROR;
      }
#endif
    }
    if (gstk_status == GSTK_SUCCESS ) {
      /* Call Connected */
      if ((new_event_list_mask[GSTK_CALL_CONNECTED] & 0x01) == 0x01) { /* call connected */
        MSG_HIGH("... Call Connected event needed", 0, 0, 0);    
      }
      else {
        MSG_HIGH("... Call Connected event not needed", 0, 0, 0);
      }

      /* Call Disconnected */
      if ((new_event_list_mask[GSTK_CALL_DISCONNECTED] & 0x01) == 0x01) { /* call disconnected */
        MSG_HIGH("... Call Disconnected event needed", 0, 0, 0);    
      }
      else {
        MSG_HIGH("... Call Disconnected event not needed", 0, 0, 0);
      }
   
      /* Location status */
      if ((new_event_list_mask[GSTK_LOCATION_STATUS] & 0x01) == 0x01) 
      { 
        /* location status */
        MSG_HIGH("... Location status event needed", 0, 0, 0);
      }
      else {
        MSG_HIGH("... Location status event not needed", 0, 0, 0);
      }

      /* User activity */
      /* build Message to clients */
      request.hdr_cmd.command_id = GSTK_SETUP_EVT_USER_ACT_NOTIFY;
#ifdef FEATURE_APPS_IMAGE_WINMOB
      request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;
      request.hdr_cmd.command_number = 
        command_details_ref_table[cmd_details_ref_id].command_details.command_number;
#else
      /*cmd_detail_reference - not used */
      request.hdr_cmd.cmd_detail_reference = 0xFF;
      /* command number - not used */
      request.hdr_cmd.command_number = 0xFF;
#endif /* FEATURE_APPS_IMAGE_WINMOB */

      if ((new_event_list_mask[GSTK_USER_ACTIVITY] & 0x01) == 0x01){ /* user act */
        /* nothing */
        request.cmd.evt_notify_req.require_notification = TRUE;
        MSG_MED("... Register User act", 0, 0, 0);
      }
      else {
        request.cmd.evt_notify_req.require_notification = FALSE;
        MSG_MED("... DeRegister user act", 0, 0, 0);
        /* nothing */
      }
      /* Send command to clients only if any updates were received for this bit */
      if(new_event_list_mask[GSTK_USER_ACTIVITY] != gstk_evt_list[GSTK_USER_ACTIVITY])
      {
        gstk_status = gstk_send_setup_evt_list_to_client(
                        cmd_ptr,
                        &request,
                        cmd_details_ref_id,
                        &cli_rcvd_raw_cmd_flg);
      }

      if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
        /* reset gstk_status */
        gstk_status = GSTK_SUCCESS;
        /*idle screen */
        /* build Message to clients */
        request.hdr_cmd.command_id = GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY;
      
        if ((new_event_list_mask[GSTK_IDLE_SCRN_AVAILABLE] & 0x01) == 0x01) { /* idle scrn */
          request.cmd.evt_notify_req.require_notification = TRUE;        
          MSG_MED("... Register idle", 0, 0, 0);               
        }
        else {
          request.cmd.evt_notify_req.require_notification = FALSE;      
          MSG_MED("... deRegister idle", 0, 0, 0);
        }
        /* Send command to clients only if any updates were received for this bit */
        if(new_event_list_mask[GSTK_IDLE_SCRN_AVAILABLE] != gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE])
        {
          gstk_status = gstk_send_setup_evt_list_to_client(
                          cmd_ptr,
                          &request,
                          cmd_details_ref_id,
                          &cli_rcvd_raw_cmd_flg);
        }

        if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
          /* reset gstk_status */
          gstk_status = GSTK_SUCCESS;
          /* language selection */
          /* build Message to clients */
          request.hdr_cmd.command_id = GSTK_SETUP_EVT_LANG_SEL_NOTIFY;
      
          if ((new_event_list_mask[GSTK_LANGUAGE_SELECTION] & 0x01) == 0x01) { /* lang sel */
            request.cmd.evt_notify_req.require_notification = TRUE;
            MSG_MED("... Register Language Selection", 0, 0, 0);
          }
          else {
            request.cmd.evt_notify_req.require_notification = FALSE;
            MSG_MED("... DeRegister Language Selection", 0, 0, 0);
          }
          /* Send command to clients only if any updates were received for this bit */
          if(new_event_list_mask[GSTK_LANGUAGE_SELECTION] != gstk_evt_list[GSTK_LANGUAGE_SELECTION])
          {
            gstk_status = gstk_send_setup_evt_list_to_client(
                            cmd_ptr,
                            &request,
                            cmd_details_ref_id,
                            &cli_rcvd_raw_cmd_flg);
          }

          if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
            /* reset gstk_status */
            gstk_status = GSTK_SUCCESS;
            /* browser termination */
            /* build Message to clients */
            request.hdr_cmd.command_id = GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY;
            if ((new_event_list_mask[GSTK_BROWSER_TERMINATION] & 0x01) == 0x01) { /* browser term */
              request.cmd.evt_notify_req.require_notification = TRUE;
              MSG_MED("... Register Browser Termination", 0, 0, 0);
            }
            else {
              request.cmd.evt_notify_req.require_notification = FALSE;
              MSG_MED("... DeRegister Browser Termination", 0, 0, 0);
            }
            /* Send command to clients only if any updates were received for this bit */
            if(new_event_list_mask[GSTK_BROWSER_TERMINATION] != gstk_evt_list[GSTK_BROWSER_TERMINATION])
            {
              gstk_status = gstk_send_setup_evt_list_to_client(
                              cmd_ptr,
                              &request,
                              cmd_details_ref_id,
                              &cli_rcvd_raw_cmd_flg);
            }

            if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
              /* reset gstk_status */
              gstk_status = GSTK_SUCCESS;

              /* future */
              if ((new_event_list_mask[GSTK_CARD_READER_STATUS] & 0x01) == 0x01) { /* card reader */
              }
              else {
              }
              
              /* data available */
              /* build Message to clients */
              request.hdr_cmd.command_id = GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY;
              if ((new_event_list_mask[GSTK_DATA_AVAILABLE] & 0x01) == 0x01) { /* data avail */
                request.cmd.evt_notify_req.require_notification = TRUE;
                MSG_MED("... Register Data Available", 0, 0, 0);
              }
              else {
                request.cmd.evt_notify_req.require_notification = FALSE;
                MSG_MED("... DeRegister Data Available", 0, 0, 0);
              }
              /* Send command to clients only if any updates were received for this bit */
              if(new_event_list_mask[GSTK_DATA_AVAILABLE] != gstk_evt_list[GSTK_DATA_AVAILABLE])
              {
                gstk_status = gstk_util_send_message_to_clients(&request);
#ifdef FEATURE_APPS_IMAGE_WINMOB
                if (gstk_status == GSTK_SUCCESS)
                {
                  /* increment the number of client responses needed before sending TR */
                  command_details_ref_table[cmd_details_ref_id].pending_responses++;
                }
#endif /* FEATURE_APPS_IMAGE_WINMOB */
              }

              if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) {
                /* reset gstk_status */
                gstk_status = GSTK_SUCCESS;
              
                /* channel status */
                /* build Message to clients */
                request.hdr_cmd.command_id = GSTK_SETUP_EVT_CH_STATUS_NOTIFY;
                if ((new_event_list_mask[GSTK_CHANNEL_STATUS] & 0x01) == 0x01) { /* data avail */
                  request.cmd.evt_notify_req.require_notification = TRUE;
                  MSG_MED("... Register Channel Status", 0, 0, 0);
                }
                else {
                  request.cmd.evt_notify_req.require_notification = FALSE;
                  MSG_MED("... DeRegister Channel Status", 0, 0, 0);
                }
                /* Send command to clients only if any updates were received for this bit */
                if(new_event_list_mask[GSTK_CHANNEL_STATUS] != gstk_evt_list[GSTK_CHANNEL_STATUS])
                {
                  gstk_status = gstk_util_send_message_to_clients(&request);
#ifdef FEATURE_APPS_IMAGE_WINMOB
                  if (gstk_status == GSTK_SUCCESS)
                  {
                    /* increment the number of client responses needed before sending TR */
                    command_details_ref_table[cmd_details_ref_id].pending_responses++;
                  }
#endif /* FEATURE_APPS_IMAGE_WINMOB */
                }
#ifdef FEATURE_CAT_REL6
                if ((gstk_status == GSTK_SUCCESS) || (gstk_status == GSTK_CLIENT_NOT_REGISTERED)) 
                {
                  int evt_flg = 0;
                  /* reset gstk_status */
                  gstk_status = GSTK_SUCCESS;

                  evt_flg = new_event_list_mask[GSTK_NW_SEARCH_MODE] & 0x01;
#if !defined (FEATURE_GSTK_FOR_NON_LTK_BUILD)
                  gstk_status = gstk_cm_ph_reg_dereg(evt_flg,
                                                     CM_PH_EVENT_SYS_SEL_PREF,
                                                     CM_PH_EVENT_SYS_SEL_PREF);
#endif
                } /* channel status */                      
#endif /* FEATURE_CAT_REL6 */
              } /* data available */
            } /* gstk_succses_from browser term (de)registration */
          } /* gstk_success from lang sel (de)registration */
        } /* gstk_success from idle screen (de)registration */
      } /* gstk_success from user activity (de)registration */
    } /* gstk_success from MT (de)registration */

    if(gstk_status != GSTK_SUCCESS)
    {
      /* Send Terminal Response to Card */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      MSG_HIGH("SETUP EVT LIST TLV parsing", 0, 0, 0);

      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      general_result = GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    }
    else
    {
      MSG_MED("Setup Evt List Success", 0, 0, 0);
      /* Update gstk_evt_list with the newly received set up evt list from the card */
      memcpy(gstk_evt_list, new_event_list_mask, GSTK_MAX_EVENT_LIST_LEN);
      /* Send good Terminal Response to the Card */
      general_result = GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
    }
  }
  else { /* parsing error */

    ERR("SETUP EVT LIST TLV parsing err", 0, 0, 0);
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    general_result = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  } /* if error_already_raised */

  /* send the Terminal Response */
  gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                              0,              /* user_data */
                              cmd_details_ref_id, /* command_details_ref */
                              command_details_ref_table[cmd_details_ref_id].command_details.command_number,
                              /* command_num */
                              GSTK_SETUP_EVENT_LIST_CNF, /* command response */
                              general_result, /* general_result */
                              &additional_info, /* additional result */
                              &dummy_extra_param /* extra param */ );
  /* no need to release memory allocation, but still call the function */
  gstk_setup_evt_list_req_cleanup(&request, cmd_data_ptr);

  if(gstk_status != GSTK_SUCCESS) {
    MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
  }
  return gstk_status;
} /* gstk_setup_evt_list_req */

/*===========================================================================
FUNCTION gstk_send_setup_evt_list_cnf

DESCRIPTION
  This function packs the setup event list terminal response and send
  the command to UIM server.

PARAMETERS
  cnf_ptr: [Input] Pointer to message that is required to be sent to UIM

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
static gstk_status_enum_type gstk_send_setup_evt_list_cnf(
  gstk_setup_evt_list_cnf_type* cnf_ptr)
{
  gstk_status_enum_type                 gstk_status        = GSTK_SUCCESS;
  setup_evt_list_terminal_response_type STK_response;
  uim_cmd_type                          *uim_cmd_ptr       = NULL;
  int                                   offset             = 0;
  int                                   i                  = 0;
  
  /* initialize STK_response */
  memset(&STK_response, 0, sizeof(setup_evt_list_terminal_response_type));

  MSG_HIGH("Send setup evt list cnf.",0,0,0);

  if(cnf_ptr == NULL)
  {
    MSG_ERROR("cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }
  
  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      cnf_ptr->cmd_details_ref_id,
      cnf_ptr->message_header.command_id,
      cnf_ptr->command_number,
      &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      cnf_ptr->command_result,
      cnf_ptr->result_additional_info,
      &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
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
        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(
          cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = 0;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                                &STK_response.data.command_details,
                                offset,
                                (STK_response.data.command_details.length + 
                                 GSTK_TAG_LENGTH_LEN ),
                                UIM_MAX_CHARS,
                                sizeof(gstk_command_details_tlv_type));
        offset = (int)(offset + STK_response.data.command_details.length + 
                       GSTK_TAG_LENGTH_LEN );
        /* copy device */
        gstk_byte_offset_memcpy(uim_cmd_ptr->terminal_response.data,
                             &STK_response.data.device_id,
                             offset,
                             (STK_response.data.device_id.device_tag_length + 
                              GSTK_TAG_LENGTH_LEN ),
                             UIM_MAX_CHARS,
                             sizeof(gstk_device_identities_tag_tlv_type));
        offset = (int)(offset + STK_response.data.device_id.device_tag_length + 
                       GSTK_TAG_LENGTH_LEN );
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
                            (cnf_ptr->result_additional_info.length * 
                             sizeof(byte)));
          offset = (int)(offset + STK_response.data.result.result_tag_length - 1);
        }

        MSG_MED("Setup Evt List: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
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
    ERR ("no UIM cmd buf", 0, 0, 0);
    gstk_status = GSTK_MEMORY_ERROR;
  }

  gstk_setup_evt_list_cnf_cleanup(cnf_ptr, &STK_response);
  return gstk_status;
} /*gstk_send_setup_evt_list_cnf*/

/*===========================================================================
FUNCTION gstk_consolidate_setup_evt_list_cnf

DESCRIPTION
  This function waits for all clients to send terminal responses of setup 
  event list command. After receiving all responses, it sends the final 
  response to the card
  
  If FEATURE_APPS_IMAGE_WINMOB is defined or both 
  FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT and FEATURE_GSTK_STREAMING_APDU_SUPPORT 
  are define, this function will wait for all clients to send SETUP EVT LIST 
  terminal response, consolidate all responses and send the final response to 
  the card. However, if any client sends error terminal response, this function 
  will send this error to the card without further waiting. If none of above 
  three features are defined, the function will simply send the terminal 
  response right away to the card.

PARAMETERS
  setup_evt_list_cnf: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_consolidate_setup_evt_list_cnf(
  gstk_setup_evt_list_cnf_type* setup_evt_list_cnf)
{
#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
  static uint32 cache_size = sizeof(gstk_priv_data.setup_evt_list_cnf_cache);
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */

  MSG_HIGH("IN GSTK gstk_consolidate_setup_evt_list_cnf ",0,0,0);

  if(setup_evt_list_cnf == NULL)
  {
    MSG_ERROR("setup_evt_list_cnf ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
  if (gstk_priv_data.setup_evt_list_cnf_cache.message_header.command_id
      == GSTK_CMD_ENUM_NOT_USE)
  {
    /* cache is not initialized, initialize header */
    gstk_memcpy(&gstk_priv_data.setup_evt_list_cnf_cache.message_header, 
                &setup_evt_list_cnf->message_header, 
                cache_size, 
                cache_size, 
                cache_size); 
  }    

  switch(setup_evt_list_cnf->command_result)
  {
  case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
  case GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION:
  case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
    gstk_priv_data.setup_evt_list_cnf_cache.command_result = 
      setup_evt_list_cnf->command_result;
    break;
  case GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND:
  case GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES:
  case GSTK_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME:
  case GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME:
  case GSTK_COMMAND_NUMBER_NOT_KNOWN_BY_ME:
  case GSTK_ERROR_REQUIRED_VALUES_ARE_MISSING:
    /* send error response right away */
    command_details_ref_table[
    setup_evt_list_cnf->cmd_details_ref_id].pending_responses = 0;
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type *)&gstk_priv_data.setup_evt_list_cnf_cache);
    gstk_memcpy(&gstk_priv_data.setup_evt_list_cnf_cache,
                setup_evt_list_cnf,
                cache_size,
                cache_size,
                cache_size);
    if (setup_evt_list_cnf->result_additional_info.length > 0)
    {
      gstk_priv_data.setup_evt_list_cnf_cache.result_additional_info.additional_info_ptr = 
        gstk_malloc(setup_evt_list_cnf->result_additional_info.length);
      if (gstk_priv_data.setup_evt_list_cnf_cache.result_additional_info.additional_info_ptr
          == NULL)
      {
        MSG_ERROR("can't allocate mem for addi info in setup evt list tr!",
                  0, 0, 0);
      }
      else
      {
        gstk_memcpy(
          gstk_priv_data.setup_evt_list_cnf_cache.result_additional_info.additional_info_ptr,
          setup_evt_list_cnf->result_additional_info.additional_info_ptr,
          setup_evt_list_cnf->result_additional_info.length,
          setup_evt_list_cnf->result_additional_info.length,
          setup_evt_list_cnf->result_additional_info.length);
        gstk_priv_data.setup_evt_list_cnf_cache.result_additional_info.length = 
          setup_evt_list_cnf->result_additional_info.length;
      }
    }
    break;
  default:
    MSG_ERROR("unexpected setup evt list result received 0x%x",
              setup_evt_list_cnf->command_result, 0, 0);
    gstk_util_release_upper_layer_cmd_memory(
      (gstk_cmd_type *)setup_evt_list_cnf);
    return GSTK_ERROR;
  }

  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type *)setup_evt_list_cnf);

  if (command_details_ref_table[
       gstk_priv_data.setup_evt_list_cnf_cache.cmd_details_ref_id].pending_responses != 0)
  {
    MSG_HIGH("wait for 0x%x more term rsp!", 
             command_details_ref_table[
               setup_evt_list_cnf->cmd_details_ref_id].pending_responses,
             0,
             0);
    return GSTK_SUCCESS;
  }
  return gstk_send_setup_evt_list_cnf(&gstk_priv_data.setup_evt_list_cnf_cache);
#else
  return gstk_send_setup_evt_list_cnf(setup_evt_list_cnf);
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */
} /* gstk_consolidate_setup_evt_list_cnf */

#if defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT)
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT */
