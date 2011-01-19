/*===========================================================================


            G S T K   P R O A C T I V E   C M D



GENERAL DESCRIPTION
  This source file contains functions to process proactive command.
  It contains functions that will dispatch the proactive command processing
  to various function based on the type of command being sent from the card


FUNCTIONS
  gstk_find_expected_proactive_cmd_rsp
    This function mapped the passed in command type to the corresponding
    response type

  gstk_preprocess_proactive_cmd
    This function parsed the command into:
    proactive command tag, total tlv length, command detail tlv
    and device identities tlv and performs early error detection on these fields

  gstk_process_proactive_command
    This function switches on the command id and call the corresponding
   proactive command to further parse/process the command

  gstk_end_proactive_session
    This function notifies all the gstk client regarding the end of a
    proactive session


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


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstk_proactive_cmd.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/09   xz      Fix issue of unable to send error TR
01/16/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
11/13/08   xz      Fix issue of cmd_ref_id leak
11/13/08   xz      Fix klockwork errors
09/14/08   sk      Added fix for lint errors
09/08/08   sk      Added support to save the raw apdu of the proactive command
                   from the card for ESTK use.
09/08/08   sk      Fixed warning
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/27/08   sk      Fixed warning
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session 
                   Added support to differentiate the different types of
                   Provide Local Info Proactive command requests
08/21/08   sk      Added GSTK enhanced registration support.
08/19/08   sk      Fixed compilation error
08/15/08   sk      Fixed compilation error
08/14/08   sk      Fixed warnings
08/08/08   xz      Add support for raw proactive command
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was 
                   originally handled by GSTK
08/08/07   sk      Ignore command when BER TLV is unknown
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
10/23/06   sk      Fixed lint errors.
10/08/06   sk      Added support for unknown unsupported proactive commands.
06/24/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
04/04/06   sk      Added pli - battery status support
11/09/05   sk      Fixed Lint Errors
07/26/05   sk      Changed Response Type for Access Technology
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Added SAT/USAT data support
06/14/04   tml     Added support to differentiate GSTK auto generated END or
                   End from the card
05/11/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection 
                   evt dl supports
04/16/04   tml     linted
03/01/04   tml     Added language provide local info support
07/07/03   tml     Change priority of messages
05/16/03   tml     linted
04/17/03   tml     lint
03/06/03   tml     Updated enum names
02/28/03   jar     Uncommented Refreh Req Handling
02/13/03   tml     Added/Changed DM messages
02/07/03   tml     Initial Version


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "intconv.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"

#include "err.h"
#include "string.h"

/* Task Definition Headers */
#include "tmc.h"

/*===========================================================================
FUNCTION gstk_find_expected_proactive_cmd_rsp

DESCRIPTION
  This function mapped the passed in command type to the corresponding
  response type

PARAMETERS
  request_cmd_type: [Input] Original proactive command type to be mapped
  expected_cmd_rsp: [Input/Output] Pointer to response command type to be
                                   populated

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_UNSUPPORTED_COMMAND
      GSTK_INVALID_COMMAND

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_find_expected_proactive_cmd_rsp(
                      uint32              request_cmd_type,
                      gstk_cmd_enum_type *expected_cmd_rsp)
{
    if(expected_cmd_rsp == NULL)
    {
      MSG_ERROR("expected_cmd_rsp ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }
    switch(request_cmd_type) {
    case GSTK_CMD_STK_REFRESH:
        *expected_cmd_rsp = GSTK_REFRESH_CNF;
         break;
    case GSTK_CMD_STK_MORE_TIME:
        *expected_cmd_rsp = GSTK_MORE_TIME_CNF;
        break;
    case GSTK_CMD_STK_POLL_INTERVAL:
        *expected_cmd_rsp = GSTK_POLL_INTERVAL_CNF;
        break;
    case GSTK_CMD_STK_POLLING_OFF:
        *expected_cmd_rsp = GSTK_POLLING_OFF_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_EVENT_LIST:
        *expected_cmd_rsp = GSTK_SETUP_EVENT_LIST_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_CALL:
        *expected_cmd_rsp = GSTK_SETUP_CALL_CNF;
        break;
    case GSTK_CMD_STK_SEND_SS:
        *expected_cmd_rsp = GSTK_SEND_SS_CNF;
        break;
    case GSTK_CMD_STK_USSD:
        *expected_cmd_rsp = GSTK_SEND_USSD_CNF;
        break;
    case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
        *expected_cmd_rsp = GSTK_SEND_SMS_CNF;
        break;
    case GSTK_CMD_STK_LAUNCH_BROWSER:
        *expected_cmd_rsp = GSTK_LAUNCH_BROWSER_CNF;
        break;
    case GSTK_CMD_STK_PLAY_TONE:
        *expected_cmd_rsp = GSTK_PLAY_TONE_CNF;
        break;
    case GSTK_CMD_STK_DISPLAY_TEXT:
        *expected_cmd_rsp = GSTK_DISPLAY_TEXT_CNF;
        break;
    case GSTK_CMD_STK_GET_INKEY:
        *expected_cmd_rsp = GSTK_GET_INKEY_CNF;
        break;
    case GSTK_CMD_STK_GET_INPUT:
        *expected_cmd_rsp = GSTK_GET_INPUT_CNF;
        break;
    case GSTK_CMD_STK_SELECT_ITEM:
        *expected_cmd_rsp = GSTK_SELECT_ITEM_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_MENU:
        *expected_cmd_rsp = GSTK_SETUP_MENU_CNF;
        break;
    case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
        /* Language provide local info cnf type will be assigned in the
           preprocess proactive cmd function */
        *expected_cmd_rsp = GSTK_PROVIDE_LOCAL_INFO_CNF;
        break;
    case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
        *expected_cmd_rsp = GSTK_SETUP_IDLE_TEXT_CNF;
        break;
    case GSTK_CMD_STK_RUN_AT_COMMAND:
        *expected_cmd_rsp = GSTK_RUN_AT_CMDS_CNF;
        break;
    case GSTK_CMD_STK_SEND_DTMF:
        *expected_cmd_rsp = GSTK_SEND_DTMF_CNF;
        break;
    case GSTK_CMD_STK_TIMER_MANAGEMENT:
        *expected_cmd_rsp = GSTK_TIMER_MANAGEMENT_CNF;
        break;
    case GSTK_CMD_STK_LANG_NOTIFICATION:
        *expected_cmd_rsp = GSTK_LANG_NOTIFICATION_CNF;
        break;
    case GSTK_CMD_STK_OPEN_CHANNEL:
        *expected_cmd_rsp = GSTK_OPEN_CH_CNF;
        break;
    case GSTK_CMD_STK_CLOSE_CHANNEL:
        *expected_cmd_rsp = GSTK_CLOSE_CH_CNF;
        break;
    case GSTK_CMD_STK_RECEIVE_DATA:
        *expected_cmd_rsp = GSTK_RECEIVE_DATA_CNF;
        break;
    case GSTK_CMD_STK_SEND_DATA:
        *expected_cmd_rsp = GSTK_SEND_DATA_CNF;
        break;
    case GSTK_CMD_STK_GET_CHANNEL_STATUS:
        *expected_cmd_rsp = GSTK_GET_CH_STATUS_CNF;
        break;

    /* not supported for PHASE 1 development */
    case GSTK_CMD_STK_PERFORM_CARD_APDU:
    case GSTK_CMD_STK_POWER_ON_CARD:
    case GSTK_CMD_STK_POWER_OFF_CARD:
    case GSTK_CMD_STK_GET_READER_STATUS:
    default:
        *expected_cmd_rsp = GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF;
        break;
    }

    // how about CMD_STK_END_PROACTIVE_CMD_SEC?
    return GSTK_SUCCESS;
} /* gstk_find_expected_proactive_cmd_rsp*/

/*===========================================================================
FUNCTION gstk_preprocess_proactive_cmd

DESCRIPTION
  This function parsed the command into:
  proactive command tag, total tlv length, command detail tlv
  and device identities tlv and performs early error detection on these fields
  The rest of the data is put into the cmd_data_ptr (passed in parameter)
  cmd_data_len_ptr indicates the size of the remaining tlv datas after parsing
  the command details and device ID
  cmd_details_ref_id is the index location in the global command details ref
  table.  If there is still free slot in the command details ref table,
  the command details thus parsed will be assigned to the index location (as
  indicated by cmd_details_ref_id) in the global ref table

PARAMETERS
  cmd_ptr:           [Input]        Pointer to the original proactive command
                                    data to be processed
  cmd_data_ptr:      [Input/Output] Pointer to be populated for the rest of the
                                    proactive cmd_ptr data after command details 
                                    and device id has been processed. If NULL 
                                    pointer is passed in, this function will not 
                                    allocate command data buffer.
  cmd_data_len_ptr:  [Input/Output] Length for valid data in the cmd_data_ptr that 
                                    is being returned to the user. If NULL pointer
                                    is passed in, this function will not allocate 
                                    command data buffer.
  cmd_ref_id_ptr:    [Input/Output] Pointer to command details reference index 
                                    location allocated for this proactive command 
                                    in the command_details_ref_table

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
gstk_status_enum_type gstk_preprocess_proactive_cmd(
         gstk_proactive_cmd_type*  cmd_ptr,
         uint8**                   cmd_data_ptr,
         int32*                    cmd_data_len_ptr,
         uint32*                   cmd_ref_id_ptr)
{
  gstk_status_enum_type    gstk_status                = GSTK_SUCCESS;
  int32                    length_of_TLV_length_field = 1;
  int32                    length_of_all_tlvs         = 0; /* cmd_ptr->data[1] */
  int32                    count                      = 0; /* incremented along the cmd_ptr as we parse
                                                            ** and verify each index location */
  gstk_cmd_enum_type       expected_cmd_rsp;

  MSG_LOW("IN PREPROCESS_COMMAND_EXT",0,0,0);

  /* input param checkings */
  if (cmd_ptr == NULL) {
    MSG_ERROR("cmd_ptr is null!", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }

#ifndef FEATURE_GSTK_STREAMING_APDU_SUPPORT
  if (cmd_data_ptr == NULL) {
    MSG_ERROR("cmd_data_ptr is null!", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }

  if (cmd_data_len_ptr == NULL) {
    MSG_ERROR("cmd_data_len_ptr is null!", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

  if (cmd_ref_id_ptr == NULL) {
    MSG_ERROR("cmd_details_ref_id is null!", 0, 0, 0);
    return GSTK_BAD_PARAM;
  }

  MSG_LOW(" == CURRENT COUNT: 0x%x",count,0,0);

  /*
  ** All STK commands have the following initial format,
  ** --------------------------------------------------
  **
  ** 1  | 1 byte  - Proactive SIM command tag (0xD0)
  ** 2  | 2 bytes - Length (either 1 or 2 bytes long - see 11.14)
  ** 3  | 1 byte  - Command Details tag (0x81) (Also encompasses next 4 bytes)
  ** 4  | 1 byte  - Length (0x03) 3 more bytes coming
  ** 5  | 1 byte  - Command number
  ** 6  | 2 bytes - Command specfic operation instructions
  ** 7  | 1 byte  - Device identites tag (0x82)
  ** 8  | 1 byte  - Length (0x02) 2 more coming
  ** 9  | 1 byte  - Source device code
  ** 10 | 1 byte  - Dest. device code
  */

  /*
  ** BER-TLV LENGTH
  ** This length field can either be 0x00-0x7F or
  ** 0x81 if the 2nd byte is used
  */

  /* Check for length of the length value has already been done in the 
     calling function */

  count = GSTK_PROACTIVE_CMD_TAG_INDEX + 1; /* total BER-TLV length field */
  length_of_TLV_length_field = gstk_find_length_of_length_value(&(cmd_ptr->data[count]));

  length_of_all_tlvs = cmd_ptr->data[count+length_of_TLV_length_field-1];
  count = count + length_of_TLV_length_field;

  MSG_MED(" TLV_length = %x",length_of_all_tlvs,0,0);
  MSG_LOW(" CURRENT COUNT: %x",count,0,0);

  /*
  ** COMMAND DETAILS
  ** Parse and extract the 5 bytes of command_details
  */

  if(!gstk_valid_tag(cmd_ptr->data[count],GSTK_COMMAND_DETAILS_TAG))
  {
    ERR_FATAL("Invalid Cmd Tag 0x%x", cmd_ptr->data[count], 0, 0);
  }

  *cmd_ref_id_ptr = gstk_util_cmd_details_ref_table_get_next_free_slot();

  if ((*cmd_ref_id_ptr == GSTK_CMD_DETAILS_REF_TABLE_FULL) || 
      (*cmd_ref_id_ptr > (GSTK_MAX_PROACTIVE_COMMAND-1)))
  {
    ERR_FATAL("CMD Detail Table FULL", 0, 0, 0);
  }
#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
  /* Set pending responses to 1 because at least we need 1 responses to build the TR */
  command_details_ref_table[*cmd_ref_id_ptr].pending_responses = 1;
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */

  /* Command Detail Tag */
  command_details_ref_table[*cmd_ref_id_ptr].command_details.tag =
    cmd_ptr->data[count++];

  /* Command Detail TLV length */
  command_details_ref_table[*cmd_ref_id_ptr].command_details.length =
    cmd_ptr->data[count++];

  if (command_details_ref_table[*cmd_ref_id_ptr].command_details.length !=
      GSTK_COMMAND_DETAILS_LEN)
  {
    /* Mark the cmd_details_ref_id location as free slot again */
    gstk_util_cmd_details_ref_table_free_slot(*cmd_ref_id_ptr);
    ERR_FATAL("CMD Detail Leng ERROR", 0, 0, 0);
  }

  /* Command Number */
  command_details_ref_table[*cmd_ref_id_ptr].command_details.command_number =
  cmd_ptr->data[count++];

  /* Type of Command */
  command_details_ref_table[*cmd_ref_id_ptr].command_details.type_of_command =
    cmd_ptr->data[count++];

  /* Command Qualifier */
  command_details_ref_table[*cmd_ref_id_ptr].command_details.qualifier =
    cmd_ptr->data[count++];

  /* Map the passed in command type to the corresponding response type */
  gstk_status = gstk_find_expected_proactive_cmd_rsp(
                    command_details_ref_table[*cmd_ref_id_ptr].command_details.type_of_command,
                    &expected_cmd_rsp);
  if(gstk_status != GSTK_SUCCESS) {
    ERR_FATAL("Unable to find expected proactive cmd_ptr rsp 0x%x", 
      command_details_ref_table[*cmd_ref_id_ptr].command_details.type_of_command,
      0, 
      0);
  }

  if (expected_cmd_rsp == GSTK_PROVIDE_LOCAL_INFO_CNF) 
  {
    switch (command_details_ref_table[*cmd_ref_id_ptr].command_details.qualifier)
    {
    case GSTK_LANGUAGE_SETTING:
      /* Change expected rsp enum type for Language provide local info */
         expected_cmd_rsp = GSTK_PROVIDE_LANG_INFO_CNF;
         break;
    case GSTK_ACCESS_TECHNOLOGY:
      /* Change expected rsp enum type for Access Technology */
         expected_cmd_rsp = GSTK_PROVIDE_ACCESS_TECH_CNF;
         break;
    case GSTK_BATTERY_STATUS:
#ifdef FEATURE_CAT_REL6
      /* Change expected rsp enum type for Battery Status */
         expected_cmd_rsp = GSTK_PROVIDE_BATT_STATUS_CNF;
#else /* #ifdef FEATURE_REL6 */
      MSG_HIGH("GSTK_BATTERY_STATUS: FEATURE_CAT_REL6 not supported",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    case GSTK_NETWORK_SEARCH_MODE:
#ifdef FEATURE_CAT_REL6
      /* Change expected rsp enum type for Network Search Mode */
         expected_cmd_rsp = GSTK_PROVIDE_NW_SEARCH_MODE_CNF;
#else /* #ifdef FEATURE_REL6 */
      MSG_HIGH("GSTK_NETWORK_SEARCH_MODE: FEATURE_CAT_REL6 not supported",0,0,0);
#endif /* #ifdef FEATURE_CAT_REL6 */
      break;
    default:
      break;  
    }
  }

  command_details_ref_table[*cmd_ref_id_ptr].expected_cmd_rsp_type = expected_cmd_rsp;

  MSG_LOW(" COMMAND DETAILS TAG: %x",
          command_details_ref_table[*cmd_ref_id_ptr].command_details.tag,0,0);
  MSG_LOW(" COMMAND DETAILS TAG LEN: %x",
          command_details_ref_table[*cmd_ref_id_ptr].command_details.length,0,0);
  MSG_MED(" COMMAND DETAILS CMD NUM: %x",
          command_details_ref_table[*cmd_ref_id_ptr].command_details.command_number,0,0);
  MSG_MED(" COMMAND DETAILS TYPE OF CMD: %x",
          command_details_ref_table[*cmd_ref_id_ptr].command_details.type_of_command,0,0);
  MSG_MED(" COMMAND DETAILS QUALIFIER: %x",
          command_details_ref_table[*cmd_ref_id_ptr].command_details.qualifier,0,0);

  /*
  ** DEVICE IDENTITIES
  ** Parse and extract the 4 bytes of device_identities
  */

  /* If device identities field exists, sanity check on the device identity info */

  if(gstk_valid_tag(cmd_ptr->data[count],GSTK_DEVICE_IDENTITY_TAG))
  {
    count++;
    if (cmd_ptr->data[count] == GSTK_DEVICE_IDENTITY_LEN) {
      count++;
      if (cmd_ptr->data[count] == (int)GSTK_UICC_SIM_DEVICE) { /* from card */
        MSG_LOW(" DEVICE IDs TRG DEV  : %x",cmd_ptr->data[count],0,0);
        count++; /* increment for target device */
        switch(cmd_ptr->data[count])
        {
        case GSTK_KEYPAD_DEVICE:
        case GSTK_DISPLAY_DEVICE:
        case GSTK_EARPIECE_DEVICE:
        case GSTK_UICC_SIM_DEVICE:
        case GSTK_ME_DEVICE:
        case GSTK_NETWORK_DEVICE:
        case GSTK_CHANNEL_1_DEVICE:
        case GSTK_CHANNEL_2_DEVICE:
        case GSTK_CHANNEL_3_DEVICE:
        case GSTK_CHANNEL_4_DEVICE:
        case GSTK_CHANNEL_5_DEVICE:
        case GSTK_CHANNEL_6_DEVICE:
        case GSTK_CHANNEL_7_DEVICE:
          command_details_ref_table[*cmd_ref_id_ptr].destination_dev_id = (device_identity_e)cmd_ptr->data[count];
          count++;/* increment for next TLV index */
          break;
        default:
          /* save it such that it can be used later on */
          command_details_ref_table[*cmd_ref_id_ptr].destination_dev_id = (device_identity_e)cmd_ptr->data[count];
          MSG_HIGH(" Dest Dev ID invalid %x", cmd_ptr->data[count], 0, 0);
           return GSTK_INVALID_DEVICE_COMMAND;
        }
      }
      else {
        MSG_HIGH(" Source Dev ID != UICC_SIM_DEVICE, %x", cmd_ptr->data[count], 0, 0);
        /* Mark the cmd_details_ref_id location as free slot again */
        return GSTK_INVALID_COMMAND;
      }
    }
    else {
      MSG_HIGH(" Dev ID Len != 2, %x", cmd_ptr->data[count], 0, 0);
      /* Mark the cmd_details_ref_id location as free slot again */
      return GSTK_INVALID_COMMAND;
    }
  }
  else {
    MSG_HIGH(" Dev ID Tag != Device_id_tag, %x", cmd_ptr->data[count], 0, 0);
    /* Mark the cmd_details_ref_id location as free slot again */
    return GSTK_INVALID_COMMAND;
  }
#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
  /*
  ** Store remaining bytes into command specific array
  */
  *cmd_data_len_ptr = length_of_all_tlvs + 
                      length_of_TLV_length_field + 
                      1 - 
                      count;

  if(*cmd_data_len_ptr > 0)
  {
    MSG_MED(" REMAIN CMD LEN: 0x%x, offset: 0x%x",*cmd_data_len_ptr, count,0);

    if (count < GSTK_MAX_DATA_SIZE && count > 0)
    {
      *cmd_data_ptr = (uint8*)gstk_malloc(int32touint32(*cmd_data_len_ptr));
      if (*cmd_data_ptr == NULL) {
        MSG_ERROR("No Memory for cmd_ptr data", 0, 0, 0);
        return GSTK_BAD_PARAM;
      }

      /* Set Command Specific Data */
      memcpy(*cmd_data_ptr,
             &(cmd_ptr->data[count]),
             int32touint32(*cmd_data_len_ptr));
    }
    else
    {
      MSG_ERROR("The index of cmd_ptr->data is out of bound!", 0, 0, 0);
      return GSTK_ERROR;
    }
  }

  return GSTK_SUCCESS;
}/*lint !e818 *cmd_ptr suppression for externalized function */
/* gstk_preprocess_proactive_cmd */

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

/*===========================================================================
FUNCTION gstk_process_proactive_command

DESCRIPTION

  This function switches on the command id and call the corresponding
  proactive command to further parse/process the command

PARAMETERS
  cmd_ptr: [Input] Pointer to the proactive command to be processed

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
gstk_status_enum_type gstk_process_proactive_command(
  gstk_proactive_cmd_type *cmd_ptr )
{
  gstk_status_enum_type              gstk_status                 = GSTK_SUCCESS;
  uint8                              type_of_command             = 0x00;
  uint8                              length_of_TLV_length_field  = 0x01;
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
  /* Skip TLV command details */
  /* Ignore Proactive Command Tag = 0xD0 and look at the length field value
  ** to determine length of the length field */

  /*
  ** BER-TLV LENGTH
  ** This length field can either be 0x00-0x7F or
  ** 0x81 if the 2nd byte is used
  */

  MSG_LOW(" process proactive command", 0, 0, 0);

  if(cmd_ptr == NULL)
  {
    MSG_ERROR("cmd_ptr ERR:NULL",0,0,0);
    return GSTK_BAD_PARAM;
  }

  /* Check if we have a valid proactive SIM command tag (0xD0) */
  if(!gstk_valid_tag(cmd_ptr->data[GSTK_PROACTIVE_CMD_TAG_INDEX],
                     (tags_enum_type)GSTK_PROACTIVE_SIM_COMMAND_TAG)) 
  {
    /* Not a proactive command tag, ignore the command */
    MSG_HIGH("not a valid proactive command 0x%x",cmd_ptr->data[GSTK_PROACTIVE_CMD_TAG_INDEX],0,0);
    return GSTK_INVALID_COMMAND;
  }

  /*
  ** BER-TLV LENGTH
  ** This length field can either be 0x00-0x7F or
  ** 0x81 if the 2nd byte is used
  */

  length_of_TLV_length_field = int32touint8(gstk_find_length_of_length_value(&cmd_ptr->data[1]));

  MSG_LOW(" Length of TLV: %x",length_of_TLV_length_field,0,0);

  if(length_of_TLV_length_field != 1 &&
     length_of_TLV_length_field != 2)
  {
    /* Invalid BER TLV len , ignore the command */
    MSG_HIGH(" ERROR:  Length of TLV Invalid: 0x%x", length_of_TLV_length_field,0,0);
    return GSTK_INVALID_LENGTH;
  }
  /* 4th location after total length field is the type of command
  ** 0   Proactive SIM Command Tag
  ** 1-2 total length
  ** +1  Command Details tag
  ** +1  Command details len = 3
  ** +1  Command number
  ** +1  Command Type
  ** +1  Command Qualifier
  */
  type_of_command = cmd_ptr->data[length_of_TLV_length_field + 4];

#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#endif /*FEATURE_GSTK_STREAMING_APDU_SUPPORT */
   /*
   ** WARNING
   ** If count position is modified, be sure to
   ** correct device debug statements after switch
   */

  switch( type_of_command )
  {
  case GSTK_CMD_STK_POLL_INTERVAL:
  case GSTK_CMD_STK_POLLING_OFF:
    gstk_status = gstk_polling_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SET_UP_EVENT_LIST:
    gstk_status = gstk_setup_evt_list_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SET_UP_CALL:
    gstk_status = gstk_setup_mo_call_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SEND_SS:
    gstk_status = gstk_send_ss_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_USSD:
    gstk_status = gstk_send_ussd_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_LAUNCH_BROWSER:
    gstk_status = gstk_launch_browser_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
    gstk_status = gstk_send_sms_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_PLAY_TONE:
    gstk_status = gstk_play_tone_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_DISPLAY_TEXT:
    gstk_status = gstk_display_text_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_GET_INKEY:
    gstk_status = gstk_get_inkey_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_GET_INPUT:
    gstk_status = gstk_get_input_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SELECT_ITEM:
    gstk_status = gstk_select_item_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SET_UP_MENU:
    gstk_status = gstk_setup_menu_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
    gstk_status = gstk_provide_local_info_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
    gstk_status = gstk_setup_idle_mode_text_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_END_OF_PROACTIVE_SES:
    gstk_status = gstk_end_proactive_session(cmd_ptr);
    break;

  case GSTK_CMD_STK_REFRESH:
    gstk_status = gstk_refresh_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_MORE_TIME:
    gstk_status = gstk_more_time_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_TIMER_MANAGEMENT:
    gstk_status = gstk_timer_management_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_LANG_NOTIFICATION:
    gstk_status = gstk_lang_notification_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SEND_DTMF:
    gstk_status = gstk_send_dtmf_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_OPEN_CHANNEL:
    gstk_status = gstk_open_ch_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_CLOSE_CHANNEL:
    gstk_status = gstk_close_ch_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_RECEIVE_DATA:
    gstk_status = gstk_receive_data_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_SEND_DATA:
    gstk_status = gstk_send_data_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_GET_CHANNEL_STATUS:
    gstk_status = gstk_get_ch_status_req(cmd_ptr);
    break;

  case GSTK_CMD_STK_RUN_AT_COMMAND:
  case GSTK_CMD_STK_PERFORM_CARD_APDU:
  case GSTK_CMD_STK_POWER_ON_CARD:
  case GSTK_CMD_STK_POWER_OFF_CARD:
  case GSTK_CMD_STK_GET_READER_STATUS:
    gstk_status = 
      gstk_unknown_unsupported_proactive_cmd_req(cmd_ptr,
                                                 GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES);
    break;
  default:
    gstk_status = 
      gstk_unknown_unsupported_proactive_cmd_req(cmd_ptr,
                                                 GSTK_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME);
      break;
  }
   return(gstk_status);
} /* gstk_process_proactive_command */

/*===========================================================================
FUNCTION gstk_end_proactive_session

DESCRIPTION

  This function notifies all the gstk client regarding the end of a
  proactive session

PARAMETERS
  None

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
gstk_status_enum_type gstk_end_proactive_session( 
    gstk_proactive_cmd_type* cmd)
{
    gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
    gstk_cmd_from_card_type             request;

    MSG_HIGH("END Proactive session ",0,0,0);

    if(cmd == NULL)
    {
      MSG_ERROR("cmd ERR:NULL",0,0,0);
      return GSTK_BAD_PARAM;
    }

    /* initialize request to 0 */
    memset(&request, 0, sizeof(gstk_end_proactive_req_type)+sizeof(gstk_exp_hdr_type));

    /* build the end proactive cmd request into the gstk_exp.h _req structure */

    /* Message Header */
    request.hdr_cmd.command_id = GSTK_END_PROACTIVE_CMD_REQ;
    request.hdr_cmd.cmd_detail_reference = 0;

    /* Check if it is gstk generated end session */
    if (cmd->data[GSTK_GENERATED_END_SES_INDEX] != GSTK_GENERATED_END_SES) {
      request.cmd.end_pro_cmd_req.end_from_card = TRUE;
    }

    /* Send command to all clients */
    gstk_status = gstk_util_send_message_to_clients(&request);

    if (gstk_status != GSTK_SUCCESS) {
      MSG_HIGH("Can't SEND End proactive session to clients", 0, 0, 0);
    }
    return gstk_status;

}/*lint !e818 *cmd suppression for externalized function */
/* gstk_end_proactive_session */

/*===========================================================================
FUNCTION gstk_build_unknown_unsupported_term_rsp

DESCRIPTION
  This function send terminal response for an unknown or unsupported
  proactive command.

PARAMETERS
  cmd_details_ref_id: [Input] 
  result            : [Input] 
  cnf_type          : [Input] 

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
void gstk_build_unknown_unsupported_term_rsp(
    uint32                              cmd_details_ref_id,
    gstk_general_result_enum_type       result,
    gstk_cmd_enum_type                  cnf_type)
{
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;
  gstk_terminal_rsp_extra_param_type  dummy_extra_param;

  MSG_HIGH("Build Unknown / Unsupported Proactive command", 0, 0, 0);

  /* initialize dummy_extra_param */
  dummy_extra_param.present = FALSE;

  /* Send Terminal Response to Card */
  gstk_status = gstk_send_terminal_response(GSTK_CLIENT_ID,     /* client_id */
                                            0,                  /* user_data */
                                            cmd_details_ref_id, /* command_details_ref */
                                            command_details_ref_table[cmd_details_ref_id].command_details.command_number,  /* command_num */
                                            cnf_type,           /* command response */
                                            result,             /* general_result */
                                            NULL,               /* additional result */
                                            &dummy_extra_param  /* extra param */ );
  if (gstk_status != GSTK_SUCCESS) {
    MSG_ERROR("TR fail, %d",gstk_status,0,0);
  }    
} /* gstk_build_unknown_unsupported_term_rsp */

/*===========================================================================
FUNCTION gstk_unknown_unsupported_proactive_cmd_req

DESCRIPTION
  This function parses unknown or unsupported proactive command and populates 
  result as GSTK_BEYOND_ME_CAPABILITIES or GSTK_COMMAND_TYPE_UNKNOWN respectively.

PARAMETERS
  command:     [Input] Pointer to command info from card
  result_type: [Input] The result that needs to be sent back to the card.

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
gstk_status_enum_type gstk_unknown_unsupported_proactive_cmd_req (
    gstk_proactive_cmd_type        * command_ptr, 
    gstk_general_result_enum_type  result_type
)
{

  gstk_status_enum_type               gstk_status         = GSTK_SUCCESS;
  uint8                               *cmd_data_ptr       = NULL;
  int32                               cmd_data_len        = 0;
  uint32                              cmd_details_ref_id  = 0;

  MSG_HIGH(" IN gstk_unknown_unsupported_proactive_cmd_req",0,0,0);

  /* Check and populate command details into the cmd_details_ref_table
  ** Check Device ID validity
  ** put the rest of the command into the cmd_data, and specifies the length of
  ** the cmd_data in the cmd_data_len
  */
  gstk_status = gstk_preprocess_proactive_cmd((gstk_proactive_cmd_type*)command_ptr,
                                              &cmd_data_ptr,
                                              &cmd_data_len,
                                              &cmd_details_ref_id );
  /* There exists mandatory tlv, so cmd_data should not be null */
  if (gstk_status != GSTK_SUCCESS) 
  {
    result_type = GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME;
  }
  /* release cmd_data as this is not needed anymore */
  if(cmd_data_ptr != NULL)
  {
    gstk_free(cmd_data_ptr);
    cmd_data_ptr = NULL;
  }
  /* cmd_details_ref_id will be valid even if gstk_preprocess_proactive_cmd() does not
     return a SUCCESS
  */
  gstk_build_unknown_unsupported_term_rsp(cmd_details_ref_id,
                                          result_type,
                                          command_details_ref_table[cmd_details_ref_id].expected_cmd_rsp_type);
  return(gstk_status);

}/* gstk_unknown_unsupported_proactive_cmd_req */

