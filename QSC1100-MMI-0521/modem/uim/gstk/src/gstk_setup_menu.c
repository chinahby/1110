/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E T U P   M E N U


GENERAL DESCRIPTION
    This source file contains functions to process setup menu
  proactive command


FUNCTIONS
  gstk_setup_menu_cmd_details
    This function will check the command details stored in the
    Global GSTK Command Details Reference Table.

  gstk_setup_menu_req
    This function parses the Setup menu Command from the card
    and sends it to the client.

  gstk_setup_menu_cnf
    This function packs the setup menu terminal response and sends
    the command to UIM server.

  gstk_setup_menu_cnf_cleanup
    This function cleans up any allocated pointer on the stack.


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2001-2008 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_setup_menu.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/14/08   sk      Fixed lint errors
09/04/08   sk      reset timer for client response when a TR is being sent to
                   the card.
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
06/09/06   sk      Lint fixes
05/03/06   tml     lint
11/15/05   sk      Lint fixes
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
08/13/04   tml     Centralize release memory condition in the request function
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Memory leak fix
05/26/04   tml     Ensure successfully send TR to card when parsing error 
                   occurs.  Added More debug messages 
05/16/03   tml     linted
04/29/03   tml     Properly initialize data struct
03/06/03   tml     Updated enum names
02/13/03   tml     Added/Changed DM messages
02/07/03   jar     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Task Definition Headers */
#include "tmc.h"

/* Memory Header Files */
#include "memheap.h"

/* GSTK Header Files   */
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"

/* Output & String Header Files */
#include "err.h"
#include "string.h"

/*===========================================================================
FUNCTION gstk_setup_menu_req_cleanup

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
static void gstk_setup_menu_req_cleanup(
  gstk_cmd_from_card_type *request_ptr, 
  uint8                   *cmd_data_ptr)
{
  gstk_util_release_card_cmd_memory(request_ptr);
  if (cmd_data_ptr != NULL) {
    gstk_free(cmd_data_ptr);
  }
} /* gstk_setup_menu_req_cleanup */

/*===========================================================================
FUNCTION gstk_setup_menu_cnf_cleanup

DESCRIPTION
  This function cleans up any allocated pointer on the stack.

PARAMETERS
  setup_menu_cnf_ptr: [Input] Pointer to message that is required to
                              be processed and sent to UIM
  STK_response_ptr  : [Input] Pointer to the STK intermediate message

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
static void gstk_setup_menu_cnf_cleanup(
  gstk_setup_menu_cnf_type                * setup_menu_cnf_ptr,
  setup_menu_terminal_response_type       * STK_response_ptr)
{
  if(STK_response_ptr == NULL)
  {
    MSG_ERROR("STK_response_ptr ERR:NULL",0,0,0);
    return;
  }
  /* free display setup menu tlv */
  gstk_util_release_upper_layer_cmd_memory(
    (gstk_cmd_type*)setup_menu_cnf_ptr);/*lint !e826 area too small */
  if (STK_response_ptr->data.result.additional_info != NULL) {
    gstk_free(STK_response_ptr->data.result.additional_info);
    STK_response_ptr->data.result.additional_info = NULL;
  }
} /* gstk_setup_menu_cnf_cleanup */

/*===========================================================================
FUNCTION GSTK_SETUP_MENU_CMD_DETAILS

DESCRIPTION
  This function will check the command details stored in the
  Global GSTK Command Details Reference Table.

  From this information, it will be determined whether there is
  a preferred MENU Selection.  It will also be determined whether
  there is help available.

PARAMETERS
  request_ptr           : [Input] Pointer to request
  cmd_details_ref_id_ptr: [Input] Pointer to command details reference id

DEPENDENCIES
  command_details_ref_table

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_setup_menu_cmd_details(
  gstk_cmd_from_card_type *request_ptr,
  const uint32            *cmd_details_ref_id_ptr
)
{
  if(request_ptr == NULL)
  {
    MSG_ERROR("request_ptr ERR:NULL",0,0,0);
    return;
  }

  if(cmd_details_ref_id_ptr == NULL)
  {
    MSG_ERROR("cmd_details_ref_id_ptr ERR:NULL",0,0,0);
    return;
  }
  /* Need to Determine If there is a Preference       */
  /* BIT: 1   -> 0 No Selection Preference            */
  /*          -> 1 Selection using Soft Key Preferred */
  if ( command_details_ref_table[*cmd_details_ref_id_ptr].command_details.qualifier
       & GSTK_SETUP_MENU_SEL_PREF_BITMASK)
  {
    request_ptr->cmd.setup_menu_pro_cmd_req.softkey_selection = TRUE;
    MSG_HIGH("SELECTION USING SOFTKEY PREFERRED",0,0,0);
  }
  else
  {
    request_ptr->cmd.setup_menu_pro_cmd_req.softkey_selection = FALSE;
    MSG_HIGH("NO SELECTION PREFERENCE SPECIFIED",0,0,0);
  }

  /* Need to Determine If Help is available   */
  /* BIT: 8   -> 0 No Help Available          */
  /*          -> 1 Help Available             */
  if ( command_details_ref_table[*cmd_details_ref_id_ptr].command_details.qualifier
      & GSTK_SETUP_MENU_HELP_AVAIL_BITMASK )
  {
    request_ptr->cmd.setup_menu_pro_cmd_req.help_available  = TRUE;
    MSG_HIGH("HELP AVAILABLE",0,0,0);
  }
  else
  {
    request_ptr->cmd.setup_menu_pro_cmd_req.help_available  = FALSE;
    MSG_HIGH("HELP NOT AVAILABLE",0,0,0);
  }
} /* gstk_setup_menu_cmd_details */

/*===========================================================================
FUNCTION GSTK_SETUP_MENU_REQ

DESCRIPTION
  This function parses the Set Up Menu Command from the card
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
gstk_status_enum_type gstk_setup_menu_req (
  gstk_proactive_cmd_type* command_ptr
)
{
  gstk_status_enum_type               gstk_status          = GSTK_SUCCESS;
  uint8                              *cmd_data_ptr         = NULL;
  int32                               cmd_data_len         = 0;
  uint32                              cmd_details_ref_id   = 0;
  uint8                               num_menu_items       = 0;
  uint8                               menu_item_index      = 0;
  int32                               offset               = 0;
  int32                               next_tlv_offset      = 0;
  boolean                             more_tlvs            = FALSE;
  boolean                             error_already_raised = FALSE;
  gstk_additional_info_type           additional_info      = {0};
  gstk_terminal_rsp_extra_param_type  dummy_extra_param    = {0};

  /* Cannot do {0x00} in declaration because of compilation warning 
     on certain target: "enumerated type mixed with another type" */
  gstk_cmd_from_card_type             request;

  MSG_HIGH(" IN GSTK_SETUP_MENU_REQ",0,0,0);

  if(command_ptr == NULL)
  {
    MSG_ERROR("command ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  MSG_LOW(" REQ Len: %x Req B1 %x Req B2 %x",
          command_ptr->length,
          command_ptr->data[0],
          command_ptr->data[1]);

  /* Initialize request to 0  */
  memset(&request, 0x00, sizeof(gstk_cmd_from_card_type));
  
  /* First Validate Setup Menu Command is */
  /* Valid                                */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );

  if ((gstk_status != GSTK_SUCCESS) || (cmd_data_ptr == NULL)) {
    error_already_raised = TRUE;
  }
  else {
    /* Build Command Header  */
    request.hdr_cmd.command_id = GSTK_SETUP_MENU_REQ;
    request.hdr_cmd.cmd_detail_reference = cmd_details_ref_id;

    /* Command Number - Assigned from Command    */
    /* Details Reference Table maintained by     */
    /* GSTK.  Set by gstk_preprocess_proactive_  */
    /* cmd and returned in cmd_details_ref_id    */
    request.hdr_cmd.command_number =
              command_details_ref_table[cmd_details_ref_id].
              command_details.command_number;

    /* Determine Command Details for Setup Menu  */
    gstk_setup_menu_cmd_details(&request,
                      &cmd_details_ref_id);

    /* Get the Number of Menu Items */
    gstk_status = gstk_parser_menu_num_items(
                      cmd_data_ptr,
                      &cmd_data_len,
                    &num_menu_items);

    if ( gstk_status == GSTK_SUCCESS )
    {
      /* Set the Number of Items */
      request.cmd.setup_menu_pro_cmd_req.num_items = num_menu_items;

      /* Mandatory Alpha */
      gstk_status = gstk_parser_alpha_identifier_tlv (
                          cmd_data_ptr,
                          cmd_data_len,
                          offset,
                          &(request.cmd.setup_menu_pro_cmd_req.title),
                          &next_tlv_offset,
                          &more_tlvs,
                          MANDATORY,
                          &cmd_details_ref_id
                          );
      if (gstk_status != GSTK_SUCCESS) {
        error_already_raised = TRUE;
      }
      else {
        offset = next_tlv_offset;
      }
    }
  }

  if (gstk_status != GSTK_SUCCESS) {
    error_already_raised = TRUE;
  }

  if ( ( gstk_status == GSTK_SUCCESS ) &&
    ( more_tlvs   == TRUE ) )
  {
    /* Alpha Identifier / Title was properly */
    /* pouplated                             */

    /* Start Parsing and Populating the      */
    /* Individual Menu Items                 */
    /* Must have at least 1 item */
    menu_item_index = 0;
    gstk_status = gstk_parser_item_tlv(
            cmd_data_ptr,
            cmd_data_len,
            offset,
            &request.cmd.setup_menu_pro_cmd_req.items[menu_item_index],
            &next_tlv_offset,
            &more_tlvs,
            MANDATORY,
            &cmd_details_ref_id);

    if ( gstk_status == GSTK_SUCCESS )
    {
      /* Change offset to point to offset */
      /* returned by the parsing function */
      offset = next_tlv_offset;

      /* Increment the menu_item_index    */
      menu_item_index++;
      while ( menu_item_index < request.cmd.setup_menu_pro_cmd_req.num_items )
      {
        gstk_status = gstk_parser_item_tlv(
              cmd_data_ptr,
              cmd_data_len,
              offset,
              &request.cmd.setup_menu_pro_cmd_req.items[menu_item_index],
              &next_tlv_offset,
              &more_tlvs,
              OPTIONAL,
              &cmd_details_ref_id);

        if ( gstk_status == GSTK_SUCCESS )
        {
          /* Change offset to point to offset */
          /* returned by the parsing function */
          offset = next_tlv_offset;

          /* Increment the menu_item_index    */
          menu_item_index++;
        }
        else
        {
          MSG_HIGH("PARSING ERROR %x",gstk_status,0,0);
          break;
        }
      }
    }
    else
    {
      MSG_HIGH("PARSING ERROR %x",gstk_status,0,0);
    }
  }

  if ( ( gstk_status == GSTK_SUCCESS ) &&
    ( more_tlvs   == TRUE ) )
  {
    /* Continue Parsing...need to look for */
    /* Next Action Indicator               */
    /* Current Offset is set to point to   */
    /* the next Action Indicator if it is  */
    /* Available                           */

    more_tlvs = FALSE;

    gstk_status = gstk_parser_next_action_indicator_tlv(
        cmd_data_ptr,
        cmd_data_len,
        offset,
        &request.cmd.setup_menu_pro_cmd_req.next_action_indicator,
        &next_tlv_offset,
        &more_tlvs,
        OPTIONAL,
        &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      error_already_raised = TRUE;
    }
    else
    {
      offset = next_tlv_offset;
    }
  }

  if ( ( gstk_status == GSTK_SUCCESS ) &&
       ( more_tlvs   == TRUE ) )
  {
    /* Continue Parsing...need to look for */
    /* Optional Icon Identifier            */
    /* Current Offset is set to point to   */
    /* the Icon Identifer Tag              */

    /* Warning...this parsing gets memory  */
    /* from heap to store icon data which  */
    /* must be freed                       */
    
    gstk_status = gstk_parser_icon_id_tlv(
              &(request.cmd.setup_menu_pro_cmd_req.title),
              cmd_data_ptr,
              cmd_data_len,
              offset,
              &(request.cmd.setup_menu_pro_cmd_req.icon),
              &next_tlv_offset,
              &more_tlvs,
              OPTIONAL,
              &cmd_details_ref_id);

    if(gstk_status != GSTK_SUCCESS)
    {
      error_already_raised = TRUE;
    }
    else
    {
      offset = next_tlv_offset;
    }
  }

  if ( ( gstk_status == GSTK_SUCCESS ) &&
    ( more_tlvs   == TRUE ) )
  {
    /* Continue Parsing...need to look for */
    /* Optional Icon Identifier            */
    /* Current Offset is set to point to   */
    /* the Icon Identifer Tag              */

    gstk_status = gstk_parser_item_icon_identifier_list_tlv(
              cmd_data_ptr,
              cmd_data_len,
              offset,
              &request.cmd.setup_menu_pro_cmd_req.icon_id_list,
              &next_tlv_offset,
              &more_tlvs,
              OPTIONAL,
              &cmd_details_ref_id);
    if (gstk_status != GSTK_SUCCESS)
    {
      error_already_raised = TRUE;
    }
    else
    {
      offset = next_tlv_offset;
    }
  }

  if ( !error_already_raised )
  {
    /* Send command to clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if ( gstk_status != GSTK_SUCCESS )
    {
      /* Send Terminal Response to Card         */
      /* ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND */
      MSG_HIGH("Could not send Message", 0, 0, 0);

      /* Send Terminal Response to Card */
      gstk_set_additional_info(&additional_info, GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN);
      dummy_extra_param.present            = FALSE;

      gstk_status = gstk_send_terminal_response(
                GSTK_CLIENT_ID,                            /* client_id          */
            0,                                     /* user_data          */
            cmd_details_ref_id,                        /* command_details_ref*/
            command_details_ref_table[cmd_details_ref_id].command_details.command_number,
            GSTK_SETUP_MENU_CNF,                   /* command response   */
            GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,   /* general_result     */
            &additional_info,                          /* additional result  */
            &dummy_extra_param                     /* extra param */
            );
      if(gstk_status != GSTK_SUCCESS) {
        MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
      }
    }
    else
    {
      /* Start Timer */
      gstk_util_start_client_response_timer(cmd_details_ref_id);

      /* reset gstk_status to SUCCESS           */
      /* since there has been no error          */
      /* This is needed since some of           */
      /* the TLVs are optional, which           */
      /* might cause the various parsing        */
      /* functions to return invalid_command    */
      gstk_status = GSTK_SUCCESS;
    }
  }
  else {
    /* error already raised */
    ERR("Setup menu TLV parsing err", 0, 0, 0);
    /* Send Terminal Response to Card */
    /* COMMAND_DATA_NOT_UNDERSTOOD_BY_ME */
    gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID, /* client_id */
                               0,              /* user_data */
                               cmd_details_ref_id, /* command_details_ref */
                               command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                               GSTK_SETUP_MENU_CNF, /* command response */
                               GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME, /* general_result */
                               &additional_info, /* additional result */
                               &dummy_extra_param /* extra param */ );
    if(gstk_status != GSTK_SUCCESS) {
      MSG_ERROR("TP fail, %d", gstk_status, 0, 0);
    }
  }

  /* release memory allocation for       */
  /* setup menu item request if not null */
  gstk_setup_menu_req_cleanup(&request, cmd_data_ptr);

  return gstk_status;
}

/*===========================================================================
FUNCTION gstk_setup_menu_cnf

DESCRIPTION
  This function packs the Setup Menu terminal response and sends
  the command to UIM server.

PARAMETERS
  setup_menu_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_setup_menu_cnf (
  gstk_setup_menu_cnf_type* setup_menu_cnf_ptr
)
{
  gstk_status_enum_type                   gstk_status   = GSTK_SUCCESS;
  setup_menu_terminal_response_type       STK_response  = {0};
  uim_cmd_type                            *uim_cmd_ptr  = NULL;
  int                                     offset        = 0;
  int                                     i             = 0;
  uint32                                  ref_id        = 0;
  
  MSG_HIGH("IN GSTK setup menu cnf ",0,0,0);

  if(setup_menu_cnf_ptr == NULL)
  {
    MSG_ERROR("setup_menu_cnf_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_response */
  memset(&STK_response, 0x00, sizeof(setup_menu_terminal_response_type));

  /* Pack various TLVs */

  /* Command details */
  /* from the cached table */
  gstk_status = gstk_packer_cmd_details_tlv(
      setup_menu_cnf_ptr->cmd_details_ref_id,
      setup_menu_cnf_ptr->message_header.command_id,
      setup_menu_cnf_ptr->command_number,
      &STK_response.data.command_details );

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_menu_cnf_cleanup(setup_menu_cnf_ptr, &STK_response);
    return gstk_status;
  }

  /* device ID */
  gstk_status = gstk_packer_dev_id_tlv(
      GSTK_ME_DEVICE,
      GSTK_UICC_SIM_DEVICE,
      &STK_response.data.device_id );


  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_menu_cnf_cleanup(setup_menu_cnf_ptr, &STK_response);
    return gstk_status;
  }


  /* result tag */
  gstk_status = gstk_packer_result_tlv(
      setup_menu_cnf_ptr->command_result,
      setup_menu_cnf_ptr->result_additional_info,
      &STK_response.data.result);

  if (gstk_status != GSTK_SUCCESS) {
    gstk_setup_menu_cnf_cleanup(setup_menu_cnf_ptr, &STK_response);
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
        ref_id = setup_menu_cnf_ptr->cmd_details_ref_id;

        /* clear the timer for client response */
        (void) rex_clr_timer( &(command_details_ref_table[ref_id].tp_rpt_time));

        /* clear the command detail table reference slot before since we are about to reply to
          the card */
        gstk_util_cmd_details_ref_table_free_slot(setup_menu_cnf_ptr->cmd_details_ref_id);

        /* set user_data */
        uim_cmd_ptr->hdr.user_data               = 0;
        /* populate the terminal_response info */
        uim_cmd_ptr->terminal_response.num_bytes = STK_response.num_bytes;

        /* copy command details */
        memcpy(uim_cmd_ptr->terminal_response.data,
              &STK_response.data.command_details,
              (int)(STK_response.data.command_details.length + 2));
        offset = (int)(offset + STK_response.data.command_details.length + 2);
        /* copy device */
        memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
              &STK_response.data.device_id,
              (int)(STK_response.data.device_id.device_tag_length + 2));
        offset = (int)(offset + STK_response.data.device_id.device_tag_length + 2);
        /* copy result */
        memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
              &STK_response.data.result,
              3); /* result tag +  length + general result */
        offset = offset + 3;
        memcpy(&(uim_cmd_ptr->terminal_response.data[offset]),
              STK_response.data.result.additional_info,
              (int)(STK_response.data.result.result_tag_length - 1));
        offset = (int)(offset + STK_response.data.result.result_tag_length - 1);

        MSG_MED("Setup Menu: TR 0x%x bytes", uim_cmd_ptr->terminal_response.num_bytes, 0, 0);
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

  gstk_setup_menu_cnf_cleanup(setup_menu_cnf_ptr, &STK_response);
  return gstk_status;

} /* gstk_setup_menu_cnf */

