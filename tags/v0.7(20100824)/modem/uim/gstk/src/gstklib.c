/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y


GENERAL DESCRIPTION

  This source file contains the library functions that are exposed to GSTK
  client as well as internal GSTK clients.

FUNCTIONS
  gstk_client_init
    GSTK client calls this function to initialize with GSTK module.
    Client call to this function will be a synchronous call ie
    client id if available will be returned immediately.

  gstk_client_init_ext
    GSTK client calls this function to initialize with GSTK module 
    Client call to this function will be an asynchronous call ie
    a command will be queued into GSTK queue to process the client id 
    request. Upon client id being available, client callback will be
    invoked to report the client id.

  gstk_client_reg
    Event registration and de-registration

  gstk_client_toolkit_cmd_reg
    Event registration and de-registration for raw format OR for
    GSTK enhanced registration support

  gstk_send_terminal_response
    Send terminal response library function

  gstk_send_terminal_response_ext
    Send terminal response library function (Allow more than 
    GSTK_MAX_ADDITIONAL_INFO bytes of additional info to be sent)

  gstk_send_raw_terminal_response
    Send raw terminal response library function

  gstk_send_envelope_cc_command
    Send call control envelope command library function

  gstk_send_envelope_evt_download_command
    Send event download envelope command library function

  gstk_send_envelope_mo_sms_ctrl_command
    Send MO sms control envelope command library function

  gstk_send_envelope_menu_sel_command
    Send menu selection envelope command library function

  gstk_send_envelope_sms_pp_dl_command
    Send SMS PP download envelope command library function

  gstk_update_profile_download_value
    Send terminal profile command library function

  gstk_map_proactive_req_to_cnf
    This function mapped the passed in command req type to the corresponding
    response type
    
  gstk_parse_ton_npi
    This function parse the ton and npi byte into gstk enum types

  gstk_send_proactive_cmd
    Allows UIM Server to put proactive command onto GSTK command queue

  gstk_is_cc_required
    Utility function to check if call control is required for any MO calls

  gstk_is_mo_sms_ctrl_required
    Utility function to check if mo sms control is required for any MO calls

  gstk_is_sms_pp_supported
    Utility function to check if sms pp control is supported.

  gstk_is_ussd_object_supported
    Utility function to check if the USSD object in call control is supported
    by the SIM or not.

  gstk_slot_is_cc_required
    Utility function to check if call control is required for any MO calls
    based on slot information

  gstk_slot_send_envelope_cc_command
    Send call control envelope command library function based on slot info

  gstk_ascii_to_gstk_address_type
    This function converts ASCII digits to gstk_address_type
    
  gstk_address_to_ascii
    This function converts gstk_address_type to ASCII digits

  gstk_send_envelope_cell_broadcast_dl_command
    Send Cell Broadcast download envelope command library function

  gstk_is_cell_broadcast_dl_supported
    Utility function to check if cell broadcast download is supported.

  gstk_send_raw_envelope_command
    This function is used to send raw envelope command down to SIM/USIM/RUIM.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2009 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/src/gstklib.c#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/09   dd      Only allow one GSTK Menu Selection message at a time
05/14/09   kp      Added compiler directive for demand Paging Changes
05/11/09   kp      Demand Paging Changes
04/28/09   gg      Calling gsdi_lib_is_service_available() in 
                   gstk_is_sms_pp_supported() to check status of service
04/15/09   xz      Support client id clean up of modem restart
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id 
                   when parse client TP and simplify TP download procedure
03/02/09   xz      Add client_id when send raw terminal response
02/27/09   gg      Fixing SimToolkit Black screen problem
01/16/09   xz      1) Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
                   2) Fix issue of unable to send terminal response with addi info
11/13/08   xz      Fix issue of unable to de-reg
11/13/08   xz      Fix klockwork error
11/03/08   sk      Featurized ESTK based on external interface support
10/13/08   yb      Fixed documentation
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/16/08   sk      Fixed lint error
09/14/08   sk      Fixed lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/22/08   xz      Change malloc() to gstk_malloc()
08/21/08   sk      Added support for enhanced GSTK registration that includes 
                   support for multiple clients to perform terminal profile 
                   download and support for clients to register for a subset of 
                   proactive command in GSTK or RAW format. 
08/14/08   sk      Fixed warnings.
08/08/08   xz      Add support for raw format of registration, proactive 
                   command, terminal response and evelope command/response
04/07/08   sk      Fix for PLI UTRAN NMR - Limited Service
03/17/08   sk      Fixed klockwork errors
02/06/08   sk      Support for PLI UTRAN NMR
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was originally
                   handled by GSTK
06/18/07   sp      Back out changes in gstk_slot_is_cc_required()
05/25/07   sp      Update the check to see if service is available and also 
                   activated for call control.
04/27/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Reverted changes to return FALSE for gprs call control
01/31/07   sk      Fixed lint error
01/29/07   sk      Return FALSE for check for gprs call control
11/08/06   tml     Added function to return the current toolkit slot
10/23/06   sk      Fixed lint errors.
09/18/06   sk      Fixed lint errors.
08/21/06   sk      Added featurization for GPRS call control
05/23/06   tml     GPRS Call Control support
06/01/06   sk      Added support for channel status and data available 
                   registrations.
05/03/06   tml     lint
01/18/05   sk      Added check to avoid Terminal Response for a
                   REFRESH SIM Reset proactive command.
11/09/05   sk      Fixed Lint Errors
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
11/23/04  sk/tml   Fixed compilation warning
09/20/04   tml     Added 8 bit to 7 bit decoding function
08/19/04   tml     Added cell broadcast support
07/22/04   tml     Added gstk_send_terminal_response_ext API such that
                   greater than GSTK_MAX_ADDITIONAL_INFO can be passed down 
                   to the card
05/26/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection 
                   evt dl supports
04/16/04   tml     linted
03/01/04   tml     Added provide local info lang support
12/02/03   tml     call control and mo sms control fixes
11/05/03   tml     Fixed call control issue
09/26/03   tml     Put cmd back to free queue if send terminal response fails
09/19/03   att     Added return values
09/13/03   tml     Added ascii to gstk_address_type and vice versa conversaton
                   utility functions
                           Changed slot_id_type to slot_id_enum_type
09/04/03   tml     Dual slot support
09/03/03   tml     Fixed envelope command table free issue
07/08/03   tml     Allowed at least 1 address in the MO SMS control to be
                   of zero length
05/16/03   tml     linted
04/21/03   tml     Added is sms pp support library function and NPI support and
                   cdma sms support
03/06/03   tml     Updated enum names, added gstk_is_cc_required and
                   gstk_is_mo_sms_ctrl_required
02/26/03   tml     Changed functions to take gstk_evt_cb_funct_type instead of
                   pointer to gstk_evt_cb_funct_type
02/13/03   tml     Added/Changed DM messages
02/12/03   tml     Added gstk_map_proactive_req_to_cnf
02/11/03   tml     Ensure that GSTK is the only that is allowed to send
                   terminal response for More Time proactive command
02/07/03   tml     Initial Version


===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"
#include "customer.h"

#include "string.h"
#include "msg.h"
#include "err.h"
#include "gsdi_exp.h"
#include "tmc.h"
#ifdef FEATURE_ESTK
#error code not present
#endif /* FEATURE_ESTK */

/*===========================================================================
FUNCTION GSTK_CLIENT_INIT

DESCRIPTION
  All tasks wishing to receive GSTK events has to first call gstk_client_init
  before issuing the gstk_client_reg_cb.
  In order to receive events, the client has to finish the "registration"
  process through the gstk_client_reg_cb
  GSTK will fill in the client_id which passed in as the first param in the
  function, which the client can then use that as the indication
  in other registration or deregistraton processes
  The function also requires client_type which specifies whether the client
  is a CM, DS, UI etc
  When client deregisters all its events, the client id will no longer be
  valid, thus, if the client wants re-register later, it has to obtain a new
  client id by calling gstk_client_init() again

PARAMETERS
  client_id: [Input/Output] Pointer to client id which will be assigned by GSTK
                            upon the return of this function
  client_type: [Input] Indicates the client type of the calling client.
                       Valid values are:
                        GSTK_DS_TYPE
                        GSTK_UI_TYPE


DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_CLIENT_SPACE_FULL,
    GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_client_init (
        gstk_client_id_type *client_id,
        gstk_client_type    client_type)
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    int                     i=0;

    MSG_HIGH("GSTK client init", 0, 0, 0);

    if (client_id != NULL) {
      for (i = 1; i < GSTK_MAX_CLIENT; i++) {
        if (gstk_client_table[i].free) {
          gstk_client_table[i].free = FALSE;
          gstk_client_table[i].client_type = client_type;
          /* initialize the callback, user_data and evt_reg_bitmask to NULL and zero */
          gstk_client_table[i].evt_cb.client_cb = NULL;
          gstk_client_table[i].evt_cb_type      = GSTK_CLI_CB_NONE;
          gstk_client_table[i].evt_reg_bitmask = 0;
          gstk_client_table[i].user_data = 0;
          gstk_client_table[i].client_init_cmd_cb = NULL;
          /* assign client_id */
          *client_id = (gstk_client_id_type)(i+1);
          MSG_HIGH("GSTK Client ID 0x%x", *client_id, 0, 0);
          break;
        }
      }
      if (i == GSTK_MAX_CLIENT) { /* no more space available */
        *client_id = 0;
        MSG_ERROR("GSTK Client Table Full", 0, 0, 0);
        gstk_status = GSTK_CLIENT_SPACE_FULL;
      }
    }
    else {
      MSG_ERROR("Null Input Param", 0, 0, 0);
      gstk_status = GSTK_NULL_INPUT_PARAM;
    }
    return gstk_status;
} /* gstk_client_init */

/*===========================================================================
FUNCTION GSTK_CLIENT_INIT_EXT

DESCRIPTION
  This function is an enhancement to the gstk_client_init() function . Here 
  client request to GSTK for client-id is treated as an "asynchronous" call.
  Therefore the client **must** provide a valid callaback function pointer
  so GSTK can use it to return the client id. In functionality, this function 
  is the same as the gstk_client_init() function. 

PARAMETERS
  client_type: [Input]        Indicates the client type of the calling client.
                              Valid values are:
                                GSTK_DS_TYPE
                                GSTK_UI_TYPE
                                GSTK_ESTK_TYPE
                                GSTK_TEST_TYPE
  response_cb: [Input]        client callback that GSTK uses to return 
                              client id
  client_ref:  [Input/Output] client reference data


DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS             - if processing of client_init is successful
    GSTK_INVALID_CLIENT_TYPE - if client type is invalid
    GSTK_BAD_PARAM           - if client callback is NULL
    GSTK_MEMORY_ERROR        - if GSTK queue is FULL
    GSTK_UNSUPPORTED_COMMAND - if enhanced multiple client support is 
                               not turned on.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_client_init_ext (
  gstk_client_type               client_type,
  gstk_client_init_cb_func_type  response_cb,
  gstk_client_ref_data_type      client_ref
)
{
#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#else /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
  (void)response_cb;
  (void)client_type;
  (void)client_ref;
  MSG_ERROR("FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT not defined", 0, 0, 0);
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */

} /* gstk_client_init_ext */

/*===========================================================================
FUNCTION GSTK_CLIENT_REG

DESCRIPTION
  All tasks wishing to receive GSTK events can use this function to register
  their callback functions with GSTK.
  gstk_evt_cb_funct_type specifies the cb function to be called.
  Based on the evt_reg_bitmask, gstk will only send event/information to
  the client who has registered for that event.  The cb function should follow
  the definition as state above in the gstk_evt_cb_funct_type and be aware
  of its total processing time, since the cb will be running in gstk context.
  Most of the proactive commands allow only one client registration, this is
  to prevent multiple terminal responses sent by various clients for the same
  single proactive command.  Exception to this is MORE TIME and END OF
  PROACTIVE COMMAND commands, where even without client registration for these
  specific events, GSTK will broadcast them to all its clients.

  When evt_reg_bitmask = 0 => Deregistration

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  gstk_callback_function_to_register: [Input] The callback function pointer
                                        that the client wants GSTK to
                                        call for the registered event
                                        notifications
  evt_reg_bitmask: [Input] Indicates the event that user want to
                           register the list of events are:
                           for individual proactive command =>
                             GSTK_REG_DISPLAY_TEXT_EVT
                             GSTK_REG_GET_INKEY_EVT
                             GSTK_REG_GET_INPUT_EVT
                             GSTK_REG_LAUNCH_BROWSER_EVT
                             GSTK_REG_MORE_TIME_EVT
                             GSTK_REG_PLAY_TONE_EVT
                             GSTK_REG_POLL_INTERVAL_EVT
                             GSTK_REG_POLLING_OFF_EVT
                             GSTK_REG_PROVIDE_LOCAL_INFO_EVT
                             GSTK_REG_PROVIDE_LANG_INFO_EVT
                             GSTK_REG_REFRESH_EVT
                             GSTK_REG_RUN_AT_CMDS_EVT
                             GSTK_REG_SELECT_ITEM_EVT
                             GSTK_REG_SEND_SMS_EVT
                             GSTK_REG_SEND_SS_EVT
                             GSTK_REG_SEND_USSD_EVT
                             GSTK_REG_SETUP_CALL_EVT
                             GSTK_REG_SETUP_EVENT_LIST_EVT  
                             GSTK_REG_SETUP_MENU_EVT
                             GSTK_REG_SETUP_IDLE_TEXT_EVT

                           for all proactive commands =>
                             GSTK_REG_PROACTIVE_CMD_EVT

                           for envelope response =>
                             GSTK_REG_EVT_DOWNLOAD_RSP_EVT
                             GSTK_REG_MENU_SEL_RSP_EVT

                           for all possible commands =>
                             GSTK_REG_ALL_EVT

                           To deregister => 0

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_DUPLICATE_REGISTRATION,

COMMENTS
  This function should only be called after gstk_client_init has been called
  After client deregisters, if it wants to re-register, it needs to get a new
  client id by calling gstk_client_init.  Since GSTK will release the client
  ID and erase the callback function once a client deregisters.

SIDE EFFECTS
  When evt_reg_bitmask == 0, the client_id becomes invalid.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_client_reg (
    gstk_client_id_type     client_id,
    uint32                  user_data,
    gstk_evt_cb_funct_type  gstk_callback_function_to_register,
    uint32                  evt_reg_bitmask )
{
    return gstk_client_reg_ext(client_id, 
                               user_data, 
                               gstk_callback_function_to_register,
                               (uint64)evt_reg_bitmask);
} /* gstk_client_reg */


/*===========================================================================
FUNCTION GSTK_CLIENT_REG_EXT

DESCRIPTION
  This is an extension of gstk_client_reg() function and has been introduced to
  support evt reg bit masks of size uint64. The functionality is the same as of
  gstk_client_reg().

  When evt_reg_bitmask = 0 => Deregistration

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  gstk_callback_function_to_register: [Input] The callback function pointer
                                        that the client wants GSTK to
                                        call for the registered event
                                        notifications
  evt_reg_bitmask: [Input] Indicates the event that user want to
                           register the list of events are:
                           for individual proactive command =>
                             GSTK_REG_DISPLAY_TEXT_EVT
                             GSTK_REG_GET_INKEY_EVT
                             GSTK_REG_GET_INPUT_EVT
                             GSTK_REG_LAUNCH_BROWSER_EVT
                             GSTK_REG_MORE_TIME_EVT
                             GSTK_REG_PLAY_TONE_EVT
                             GSTK_REG_POLL_INTERVAL_EVT
                             GSTK_REG_POLLING_OFF_EVT
                             GSTK_REG_PROVIDE_LOCAL_INFO_EVT
                             GSTK_REG_PROVIDE_LANG_INFO_EVT
                             GSTK_REG_REFRESH_EVT
                             GSTK_REG_RUN_AT_CMDS_EVT
                             GSTK_REG_SELECT_ITEM_EVT
                             GSTK_REG_SEND_SMS_EVT
                             GSTK_REG_SEND_SS_EVT
                             GSTK_REG_SEND_USSD_EVT
                             GSTK_REG_SETUP_CALL_EVT
                             GSTK_REG_SETUP_EVENT_LIST_EVT
                             GSTK_REG_SETUP_MENU_EVT
                             GSTK_REG_SETUP_IDLE_TEXT_EVT
                             GSTK_REG_SETUP_DATA_AVAILABLE_EVT
                             GSTK_REG_SETUP_CH_STATUS_EVT

                           for all proactive commands =>
                             GSTK_REG_PROACTIVE_CMD_EVT

                           for envelope response =>
                             GSTK_REG_EVT_DOWNLOAD_RSP_EVT
                             GSTK_REG_MENU_SEL_RSP_EVT

                           for all possible commands =>
                             GSTK_REG_ALL_EVT

                           To deregister => 0

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_DUPLICATE_REGISTRATION

COMMENTS
  This function should only be called after gstk_client_init has been called
  After client deregisters, if it wants to re-register, it needs to get a new
  client id by calling gstk_client_init.  Since GSTK will release the client
  ID and erase the callback function once a client deregisters.

SIDE EFFECTS
  When evt_reg_bitmask == 0, the client_id becomes invalid.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_client_reg_ext (
    gstk_client_id_type     client_id,
    uint32                  user_data,
    gstk_evt_cb_funct_type  gstk_callback_function_to_register,
    uint64                  evt_reg_bitmask )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;

    MSG_HIGH("GSTK client ext 0x%x reg 0x%x", client_id, evt_reg_bitmask, 0);

    if (!gstk_util_is_valid_client_id(client_id)) { /* not a valid client id */
      return GSTK_CLIENT_NOT_REGISTERED;
    }

    /* valid client ID, proceed with other checking */
    if (gstk_callback_function_to_register == NULL) {
      if (evt_reg_bitmask == 0) {/* client wants to deregister */
        gstk_util_free_client_id(client_id);
      }
      else {
        MSG_ERROR("Null Input Param", 0, 0, 0);
        gstk_status = GSTK_NULL_INPUT_PARAM;
      }
    }
    else { /* cb function is not null */
#ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#else
      gstk_client_table[client_id-1].evt_reg_bitmask = evt_reg_bitmask;
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
      gstk_client_table[client_id-1].evt_cb_type      = 
        GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT;
      gstk_client_table[client_id-1].evt_cb.client_cb = 
        (gstk_evt_cb_funct_type)gstk_callback_function_to_register;
      gstk_client_table[client_id-1].user_data = user_data;
    }

    return gstk_status;

} /* gstk_client_reg_ext */

/*===========================================================================
FUNCTION GSTK_CLIENT_TOOLKIT_CMD_REG

DESCRIPTION
  This is a new registration API that clients can use to register with GSTK for
  specific proactive commands. This API differs from "gstk_client_reg_ext" in
  the following aspects. 
  a. the client has the option of specifying hich part (UI, MODEM or ALL) of the 
     proactive command it would like to process. 
  b. The client can also specify if it wishes to recive the proactive command notification
  in the GSTK format or in the RAW APDU format.
  c. Client can provide a command callback that GSTK would use to report the status
     of the registration even before the proactive commands come in from the card.
  NOTE: A single client should register with the same callback for any proactive
        command registration irrespective of the format and the functionality 
        type.

  The following functionalities are similar as in the API "gstk_client_reg_ext". 
  a. the client should provide a event callback that GSTK would use to send 
     notification of the proactive commands as and when they occur.
  b. Based on the evt_reg_bitmask and the class bitmask, gstk will only send 
     event/information to the client who has registered for that event.  
  c. The cb function should be aware of its total processing time, since the cb 
     will be running in gstk context.
  d. Most of the proactive commands allow only one client registration, this is
  to prevent multiple terminal responses sent by various clients for the same
  single proactive command.  Exception to this is MORE TIME and END OF
  PROACTIVE COMMAND commands, where even without client registration for these
  specific events, GSTK will broadcast them to all its clients.
  NOTE: New Caller(s) that would be calling in for registering for proactive 
        commands are recommended to use the this API which is backward compatible
        in functionality to the API "gstk_client_reg_ext".

PARAMETERS

  client_id         [Input] - Client id 
  user_data         [Input] - Client user data
  data_format_type  [Input] - Data format
  num               [Input] - Number of commands being registered for
  reg_list_ptr      [Input] - pointer to registration list
  client_cmd_cb     [Input] - Immediate callback invoked to reflect status of registration                                
  client_evt_cb     [Input] - Callback to receive command in the raw / gstk format

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_CLIENT_NOT_REGISTERED - if the client id is not recognized
    GSTK_BAD_PARAM             - bad input parameters provided
    GSTK_NULL_INPUT_PARAM      - null input pointer provided
    GSTK_SUCCESS               - command was queued successfully to GSTK

COMMENTS
  This function should only be called after gstk_client_init_ext has been 
  called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_client_toolkit_cmd_reg(
  gstk_client_id_type                         client_id,
  gstk_client_ref_data_type                   client_data,
  gstk_client_pro_cmd_data_format_enum_type   data_format_type,
  uint32                                      num,                                      
  const gstk_toolkit_cmd_reg_info_type        *reg_list_ptr,
  gstk_reg_rsp_cmd_cb_func_type               client_cmd_cb,
  gstk_toolkit_evt_cb_func_type               client_evt_cb
)
{
 #ifdef FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT
#error code not present
#else
  (void)client_id;
  (void)client_data;
  (void)data_format_type;
  (void)num;                                      
  (void)reg_list_ptr;
  (void)client_cmd_cb;
  (void)client_evt_cb;
  MSG_ERROR("gstk_client_toolkit_cmd_reg() is not supported!", 0, 0, 0);
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT */
} /* gstk_client_toolkit_cmd_reg */

/*===========================================================================
FUNCTION GSTK_SEND_TERMINAL_RESPONSE_EXT

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a terminal response to
  a proactive command.
  In this function, GSTK, will copy all the information in the parameter
  list into gstk_cmd type, and assign the correct length, cmd_detail_reference
  to the gstk_exp_hdr_type, and signal GSTK_TERMINAL_RESPONSE, so that when GSTK
  starts processing this command from its command queue, it knows what
  command it is, the corresponding command details to be used based on the
  ref_id.
  This function differs from gstk_Send_terminal_response in that it takes
  gstk_additional_info_ptr_type instead of gstk_additional_info_type.
  i.e., when client is required to pass in more than GSTK_MAX_ADDITIONAL_INFO
  byte of additional information, they can use this utility function

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                             value returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  command_number: [Input] This is the same value as that was being passed to
                          the client in the corresponding proactive command
                          request
  terminal_response_enum: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result: [Input] Specifies the result of the proactive command
                          operation
  additional_result: [Input] Pointer specifies the optional additional result
                             for the proactive command, the length field in
                             the gstk_additonal_info_ptr_type specifies number of
                             additional_info_ptr in the structure
  other_info: [Input] A union pointer of various extra parameters for the
                      various proactive command response, please refer to
                      gstk_terminal_rsp_param_type for details

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
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
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_terminal_response_ext (
        gstk_client_id_type                     client_id,
        uint32                                  user_data,
        uint32                                  cmd_detail_ref_id,
        uint8                                   command_number,
        gstk_cmd_enum_type                      terminal_response_enum,
        gstk_general_result_enum_type           general_result,
        gstk_additional_info_ptr_type          *additional_result,
        gstk_terminal_rsp_extra_param_type     *other_info )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type      * task_cmd;

    MSG_HIGH("Client send terminal rsp, ref_id:0x%x, command #:0x%x, enum:0x%x", 
              cmd_detail_ref_id, command_number, terminal_response_enum);

    /* validate client id */
    if (gstk_client_table[(client_id-1)].free) {
      if (client_id != GSTK_CLIENT_ID) {/* it is not gstk calling this function
                                        either */
        MSG_ERROR("Invalid Client ID 0x%x", client_id, 0, 0);
        return GSTK_CLIENT_NOT_REGISTERED;
      }
    }

    /* validate command details/command num and command resp type */
    if (command_details_ref_table[cmd_detail_ref_id].free_slot) {
      if ((command_details_ref_table[cmd_detail_ref_id].expected_cmd_rsp_type
             == GSTK_MORE_TIME_CNF) ||
          (terminal_response_enum == GSTK_MORE_TIME_CNF)) {
        /* no need to send terminal response down since already done so */
        return GSTK_SUCCESS;
      }
      else {
        MSG_ERROR("A Free slot for=> Terminal rsp enum: 0x%x, expected cmd rsp: 0x%x", 
                  terminal_response_enum, 
                  command_details_ref_table[cmd_detail_ref_id].expected_cmd_rsp_type, 
                  0);
        return GSTK_BAD_PARAM;
      }
    }
    else {
      if (command_details_ref_table[cmd_detail_ref_id].expected_cmd_rsp_type == GSTK_MORE_TIME_CNF) {
        if (client_id != GSTK_CLIENT_ID) {
         /* no need to send terminal response down since gstk will do so */
         return GSTK_SUCCESS;
        }
      }
      if ((command_details_ref_table[cmd_detail_ref_id].expected_cmd_rsp_type ==
           terminal_response_enum) &&
          (command_details_ref_table[cmd_detail_ref_id].command_details.command_number ==
          command_number)) { /* command details info okay */

#if defined(FEATURE_APPS_IMAGE_WINMOB) || (defined(FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT) && defined(FEATURE_GSTK_STREAMING_APDU_SUPPORT))
        command_details_ref_table[cmd_detail_ref_id].pending_responses--;
#endif /* FEATURE_APPS_IMAGE_WINMOB || (FEATURE_GSTK_ENH_MULTI_CLIENT_SUPPORT && FEATURE_GSTK_STREAMING_APDU_SUPPORT) */

        /* Check for general result */
        switch(general_result) {
        case GSTK_COMMAND_PERFORMED_SUCCESSFULLY:
        case GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED:
          if ((command_details_ref_table[cmd_detail_ref_id].command_details.type_of_command ==
              GSTK_CMD_STK_REFRESH) && 
              (command_details_ref_table[cmd_detail_ref_id].command_details.qualifier ==
               GSTK_SIM_REFRESH_RESET))
          {
              /* No need to send Terminal Response for a successful Refresh - SIM Reset */
              gstk_util_cmd_details_ref_table_free_slot(cmd_detail_ref_id);
              return GSTK_SUCCESS;
          }
          if (command_details_ref_table[cmd_detail_ref_id].partial_comprehension) {
            general_result = GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION;
          }
          break;
        case GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION:
        case GSTK_PCMD_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ:
        case GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM:
        case GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE:
        case GSTK_COMMAND_PERFORMED_WITH_MODIFICATION:
        case GSTK_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE:
        default:
          break;
        }

        /* get command buf */
        task_cmd = gstk_task_get_cmd_buf();
        if ( task_cmd != NULL ) { /* successfully obtained command buffer */
            /* build the terminal response */
            if(other_info != NULL)
            {
            MSG_HIGH("Other_info is %d",other_info->present,0,0);
            }
            else
            {
              MSG_HIGH("Other_info is NULL",0,0,0);
            }
            gstk_status = gstk_populate_terminal_response_cnf(
                                            task_cmd,
                                            terminal_response_enum,
                                            general_result,
                                            additional_result,
                                            other_info,
                                            command_number,
                                            cmd_detail_ref_id,
                                            user_data);
            if (gstk_status == GSTK_SUCCESS) {
              /* set GSTK_CMD_Q_SIG and put on GSTK queue */
              gstk_task_put_cmd_buf(task_cmd);
              /* stop the terminal response timer */
             // gstk_util_stop_client_response_timer();
            }
            else {
              /* put the queue back to the free queue */
              (void)q_link(task_cmd, &task_cmd->cmd_hdr.link);
              q_put(task_cmd->cmd_hdr.done_q_ptr, &(task_cmd->cmd_hdr.link));
              MSG_ERROR("Populate Terminal Response Error, 0x%x", gstk_status, 0, 0);
            }
        }
        else {
            // build error message to GSTK?????????????
            ERR("No more gstk buf", 0, 0, 0);
            gstk_status = GSTK_MEMORY_ERROR;
        }
      }
      else { /* command details info not okay */
        MSG_ERROR("Command Details info Error, exp command #: 0x%x, exp command type: 0x%x", 
            command_details_ref_table[cmd_detail_ref_id].command_details.command_number, 
            command_details_ref_table[cmd_detail_ref_id].expected_cmd_rsp_type, 0);
        MSG_ERROR("Command Details info recv: command #: 0x%x, term rsp type: 0x%x", 
            command_number, terminal_response_enum, 0);
        return GSTK_BAD_PARAM;
      }
    }
    return gstk_status;
}/*lint !e818 *other_info *additional_info suppression for externalized function */
/*gstk_send_terminal_response_ext*/

/*===========================================================================
FUNCTION GSTK_SEND_TERMINAL_RESPONSE

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a terminal response to
  a proactive command.
  In this function, GSTK, will copy all the information in the parameter
  list into gstk_cmd type, and assign the correct length, cmd_detail_reference
  to the gstk_exp_hdr_type, and signal GSTK_TERMINAL_RESPONSE, so that when GSTK
  starts processing this command from its command queue, it knows what
  command it is, the corresponding command details to be used based on the
  ref_id.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                             value returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
  command_number: [Input] This is the same value as that was being passed to
                          the client in the corresponding proactive command
                          request
  terminal_response_enum: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result: [Input] Specifies the result of the proactive command
                          operation
  additional_result: [Input] Pointer specifies the optional additional result
                             for the proactive command, the length field in
                             the gstk_additonal_info_type specifies number of
                             additional_info in the structure
  other_info: [Input] A union pointer of various extra parameters for the
                      various proactive command response, please refer to
                      gstk_terminal_rsp_param_type for details

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
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
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_terminal_response (
        gstk_client_id_type                     client_id,
        uint32                                  user_data,
        uint32                                  cmd_detail_ref_id,
        uint8                                   command_number,
        gstk_cmd_enum_type                      terminal_response_enum,
        gstk_general_result_enum_type           general_result,
        const gstk_additional_info_type         *additional_result,
        gstk_terminal_rsp_extra_param_type      *other_info )
{
    gstk_status_enum_type          gstk_status = GSTK_SUCCESS;
    gstk_additional_info_ptr_type  additional_ptr_result;

    /* initialize additional into ptr data */
    additional_ptr_result.length = 0;
    additional_ptr_result.additional_info_ptr = NULL;
    if ((additional_result != NULL) && (additional_result->length > 0)) {
      if (additional_result->length <= GSTK_MAX_ADDITIONAL_INFO)
      {
        additional_ptr_result.additional_info_ptr = gstk_malloc(
                                                    additional_result->length);
        if (additional_ptr_result.additional_info_ptr == NULL) {
          return GSTK_MEMORY_ERROR;
        }
        additional_ptr_result.length = additional_result->length;
        memcpy(additional_ptr_result.additional_info_ptr,
               additional_result->additional_info,
               additional_result->length);
      }
      else
      {
        MSG_ERROR("additional_result->length (0x%x) is too long (>= 0x%x)!",
                  additional_result->length, GSTK_MAX_ADDITIONAL_INFO, 0);
        return GSTK_BAD_PARAM;
      }
    }

    if(other_info != NULL)
    {
      MSG_HIGH("Other_info is %d",other_info->present,0,0);
    }
    else
    {
      MSG_HIGH("Other_info is NULL",0,0,0);
    }
    gstk_status = gstk_send_terminal_response_ext(client_id, 
                                    user_data, 
                                    cmd_detail_ref_id,
                                    command_number,
                                    terminal_response_enum,
                                    general_result,
                                    &additional_ptr_result,
                                    other_info);

    /* free the info ptr if not null */
    if (additional_ptr_result.additional_info_ptr != NULL) {
      gstk_free(additional_ptr_result.additional_info_ptr);
      additional_ptr_result.additional_info_ptr = NULL;
    }
    
    return gstk_status;

} /*gstk_send_terminal_response*/

/*===========================================================================
FUNCTION gstk_send_raw_terminal_response

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a raw terminal response to
  a proactive command.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
 term_rsp: [Input] The raw terminal response that the client wants to send

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,               - Terminal response is sent successfully
    GSTK_CLIENT_NOT_REGISTERED, - The client ID passed in is invalid
    GSTK_BAD_PARAM,             - One of parameters passed in is invalid
    GSTK_MEMORY_ERROR           - Can't allocate memory

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_terminal_response (
  gstk_client_id_type                     client_id,
  uint32                                  user_data,
  uint32                                  cmd_detail_ref_id,
  gstk_generic_data_type                  term_resp)
{
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#else
  (void)client_id;
  (void)user_data;
  (void)cmd_detail_ref_id;
  (void)term_resp;
  MSG_ERROR("gstk_send_raw_terminal_response() is not supported!", 0, 0, 0);
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */

} /*gstk_send_raw_terminal_response*/

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CC_COMMAND

DESCRIPTION

  This function send the Call Control envelope command to SIM/USIM.
  Clients need to supply the gstk_cc_ind_type as the parameter.
  within the gstk_cc_ind_type,
  Also, the gstk_address_string is required based on whether the call is
  voice call, SS or USSD
  there are also 3 optional data fields: subaddress, ccp1 and ccp2

PARAMETERS
  user_data:    [Input] User data that client wants GSTK to keep track of
  cc_ind_param: [Input] Pointer to the call control envelope command info
  gstk_cc_cb:   [Input] Function pointer to which GSTK will send
                        the card response data corresponding to the
                        Call Control command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_cc_command (
        uint32                    user_data,
        gstk_cc_ind_type          *cc_ind_param,
        gstk_evt_cb_funct_type    gstk_cc_cb )
{
    return gstk_slot_send_envelope_cc_command(GSTK_SLOT_1,
                                              user_data,
                                              cc_ind_param,
                                              gstk_cc_cb);
}/*lint !e818 *cc_ind_param suppression for externalized function */
/* gstk_send_envelope_cc_command*/

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_EVT_DOWNLOAD_COMMAND

DESCRIPTION

  This function sends the event download envelope command to the SIM/USIM
  Client is required to send the evt_list as the parameter to this function
  The gstk_evt_dl_ind_type is a struct where the first element is an enum
  type to indicate what events are being sent to the SIM/USIM in this
  command.
  Based on the enum value, client is required to fill in the specific
  information related to that event indication.
  This function does not require client ID because it is intended to be called
  by internal modules.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  evt_list: [Input] Pointer to the event download info

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_evt_download_command (
        gstk_client_id_type         client_id,
        uint32                      user_data,
        gstk_evt_dl_ind_type        *evt_list )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type      * task_cmd;
    uint32                  env_ref_id;

    MSG_HIGH("GSTK client send evt dl", 0, 0, 0);

    /* validate client id */
    if (gstk_client_table[client_id-1].free) {
      if (client_id != GSTK_CLIENT_ID) {/* it is not gstk calling this function
                                        either */
        MSG_ERROR("Client 0x%x Not Registered", client_id, 0, 0);
        return GSTK_CLIENT_NOT_REGISTERED;
      }
    }

    if(evt_list == NULL)
    {
      return GSTK_ERROR;
    }

    /* get the envelope refernce id */
    gstk_status =  gstk_util_save_env_cmd_info(
                              client_id,
                              NULL,
                              evt_list->evt_type,
                              user_data,
                              &env_ref_id);
    if(gstk_status == GSTK_SUCCESS) {

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
          /* build the evt download envelope command */
          gstk_status = gstk_util_build_evt_dl_ind(task_cmd, env_ref_id, evt_list);
          if(gstk_status == GSTK_SUCCESS) {
            /* set GSTK_CMD_Q_SIG and put on GSTK queue */
            gstk_task_put_cmd_buf(task_cmd);
          }
          else { /* release the env_ref_id */
            MSG_ERROR("Unable to build EVT DL for 0x%x, status=0x%x", evt_list->evt_type, gstk_status, 0);
            gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
            /* put the queue back to the free queue */
            (void)q_link(task_cmd, &task_cmd->cmd_hdr.link);
            q_put(task_cmd->cmd_hdr.done_q_ptr, &(task_cmd->cmd_hdr.link));
          }
      }
      else {
          ERR("No more gstk buf", 0, 0, 0);
          /* free the env slot */
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
          gstk_status = GSTK_MEMORY_ERROR;
      }
    }
    return gstk_status;
} /*lint !e818 *evt_list suppression for externalized function */
/* gstk_send_envelope_evt_download_command */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MO_SMS_CTRL_COMMAND

DESCRIPTION

  This function is used to send MO SMS control to the SIM/USIM
  within the gstk_mo_sms_ctrl_ind_type structure, user needs to
  populate the RP_Destination_Address, TP_Destination_Address

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  mo_sms_control: [Input] The MO SMS control info
  gstk_mo_sms_ctrl_cb: [Input] Function pointer to which GSTK will send
                               the card response data corresponding to the
                               SMS MO Control command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_mo_sms_ctrl_command (
    uint32                                user_data,
    const gstk_mo_sms_ctrl_ind_type      *mo_sms_control,
    gstk_evt_cb_funct_type                gstk_mo_sms_ctrl_cb )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type    * task_cmd;
    uint32                  env_ref_id;

    MSG_HIGH("GSTK client send mo sms ctrl", 0, 0, 0);

    /* validate call back function */
    if (gstk_mo_sms_ctrl_cb == NULL || mo_sms_control == NULL) {
      return GSTK_NULL_INPUT_PARAM;
    }
    /* check for mandatory data */
    if((mo_sms_control->rp_dest_address.length == 0) &&
      (mo_sms_control->tp_dest_address.length == 0)) {
      return GSTK_BAD_PARAM;
    }

    /* get the envelope refernce id */
    gstk_status =  gstk_util_save_env_cmd_info(
                              0,
                              gstk_mo_sms_ctrl_cb,
                              GSTK_MO_SMS_CTRL_IND,
                              user_data,
                              &env_ref_id);
    if(gstk_status == GSTK_SUCCESS) {

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
          /* build the envelope command */
          task_cmd->cmd.mo_sms_ctrl_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
          task_cmd->cmd.mo_sms_ctrl_envelope_ind.message_header.command_id = (int)GSTK_MO_SMS_CTRL_IND;
          /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
          task_cmd->cmd.mo_sms_ctrl_envelope_ind.message_header.user_data = env_ref_id;

          /* NOTE: RP address is a structure with all static memebers ie no pointers
             The stucture copy needs to be changed to memcpy with appropriate memory allocations
             if any memeber of the structure is dynamically allocated. 
          */
          
          /* RP address - length, TON, NPI and address */
          /* RP address source and destination address buffers are static buffers */ 
          task_cmd->cmd.mo_sms_ctrl_envelope_ind.mo_sms_ctrl_data.rp_dest_address = 
            mo_sms_control->rp_dest_address; 

          /* NOTE: TP address is a structure with all static memebers ie no pointers
             The stucture copy needs to be changed to memcpy with appropriate memory allocations
             if any memeber of the structure is dynamically allocated. 
          */

          /* TP address - length, TON, NPI and address */
          /* TP address source and destination address buffers are static buffers */ 
          task_cmd->cmd.mo_sms_ctrl_envelope_ind.mo_sms_ctrl_data.tp_dest_address =
            mo_sms_control->tp_dest_address;

          /* set GSTK_CMD_Q_SIG and put on GSTK queue */
          gstk_task_put_cmd_buf(task_cmd);
      }
      else {
          ERR("No more gstk buf", 0, 0, 0);
          /* free the env slot */
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
          return GSTK_MEMORY_ERROR;
      }

    }

    return GSTK_SUCCESS;

} /* gstk_send_envelope_mo_sms_ctrl_command */



/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND

DESCRIPTION

  This function sends the menu selection envelope command down to SIM/USIM.
  Clients need to provide the item identifier for the user selected item.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  item_chosen: [Input] User selected item.
                       Valid value [0x01, 0xFF],
                       0x00 is used to indicate a null item identifier
  help_requested: [Input] Optional data field in the envelope command:
                          TRUE -> help is requested
                          FALSE -> help is not requested and should not
                                   be sent down to the SIM/USIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_client_init has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_menu_sel_command (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type      * task_cmd;
    uint32                  env_ref_id;
    uint32                  i = 0;

    MSG_HIGH("GSTK client send menu selection, 0x%x", item_chosen, 0, 0);

    /* validate client id */
    if (gstk_client_table[client_id-1].free) {
      if (client_id != GSTK_CLIENT_ID) {/* it is not gstk calling this function
                                        either */
        MSG_ERROR("Client 0x%x not registered", client_id, 0, 0);
        return GSTK_CLIENT_NOT_REGISTERED;
      }
    }

    /* check if another menu selection message is already in progress */
    for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++)
    {
      if ((envelope_cmd_ref_table[i].free == FALSE) &&
          (envelope_cmd_ref_table[i].expected_cmd_rsp_type == GSTK_MENU_IND_RSP))
      {
        MSG_ERROR("GSTK Menu Selection message already in progress. Client ID: 0x%x", client_id, 0, 0);
        return GSTK_ERROR;
      }
    }

    /* get the envelope refernce id */
    gstk_status =  gstk_util_save_env_cmd_info(
                              client_id,
                              NULL,
                              GSTK_MENU_SELECTION_IND,
                              user_data,
                              &env_ref_id);
    if(gstk_status == GSTK_SUCCESS) {

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
          /* build the envelope command */
          task_cmd->cmd.menu_selection_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
          task_cmd->cmd.menu_selection_envelope_ind.message_header.command_id = (int)GSTK_MENU_SELECTION_IND;
          /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
          task_cmd->cmd.menu_selection_envelope_ind.message_header.user_data = env_ref_id;
          task_cmd->cmd.menu_selection_envelope_ind.chosen_item_id = item_chosen;
          task_cmd->cmd.menu_selection_envelope_ind.request_help = help_requested;

          /* set GSTK_CMD_Q_SIG and put on GSTK queue */
          gstk_task_put_cmd_buf(task_cmd);
      }
      else {
          ERR("No more gstk buf", 0, 0, 0);
          /* free the env slot */
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
          gstk_status = GSTK_MEMORY_ERROR;
      }

    }

    return gstk_status;
} /* gstk_send_envelope_menu_sel_command */

/*===========================================================================
FUNCTION GSTK_SEND_RAW_ENVELOPE_COMMAND

DESCRIPTION

  This function is used to send raw envelope command down to SIM/USIM/RUIM.
  
PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  env_cmd:   [Input] The envelope command data to be sent to SIM/USIM
  env_cmd_cb:[Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,               - The envelopse command is sent successfully
    GSTK_MEMORY_ERROR,          - Can't allocate memory or envelope command information buffer
    GSTK_BAD_PARAM,             - The parameter passed in is invalid
    GSTK_CLIENT_NOT_REGISTERED  - The client is not registered

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_raw_envelope_rsp_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_envelope_command (
  gstk_client_id_type          client_id,
  uint32                       user_data,
  gstk_generic_data_type       env_cmd,
  gstk_evt_cb_funct_type       gstk_raw_env_cb)
{
#ifdef FEATURE_GSTK_STREAMING_APDU_SUPPORT
#error code not present
#else
  (void)client_id;
  (void)user_data;
  (void)env_cmd;
  (void)gstk_raw_env_cb;
  MSG_ERROR("gstk_send_raw_envelope_command() is not supported!", 0, 0, 0);
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GSTK_STREAMING_APDU_SUPPORT */
} /* gstk_send_raw_envelope_command */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_SMS_PP_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a sms pp download command to the
  SIM/USIM.

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  is_cdma_sms_pp: [Input] Indicates whether it is CDMA or non CDMA SMS
                          PP download
  gstk_address_type: [Input] Optional according to standard specification.
                             user can use the length field to indicate
                             whether there is an address or not
                             (length = 0 => GSTK should not look at the
                             address parameter in constructing the envelope
                             command).
                             When the length != 0, the address field should
                             hold the RP_Originating_Address of the Service
                             Center
  sms_tpdu: [Input] Pointer indicating the SMS TPDU for the PP download
                    command
  gstk_sms_pp_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             SMS PP download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_sms_pp_dl_command (
    uint32                         user_data,
    boolean                        is_cdma_sms_pp,
    const gstk_address_type       *address,
    const gstk_sms_tpdu_type      *sms_tpdu,
    gstk_evt_cb_funct_type         gstk_sms_pp_dl_cb )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type      * task_cmd;
    uint32                  env_ref_id;

    MSG_HIGH("GSTK client sms pp dl", 0, 0, 0);

    /* validate call back function */
    if (gstk_sms_pp_dl_cb == NULL || sms_tpdu == NULL) {
      MSG_ERROR("Null Input Param", 0, 0, 0);
      return GSTK_NULL_INPUT_PARAM;
    }
    if(sms_tpdu->length == 0) {
      /* mandatory param */
      MSG_ERROR("SMS TPDU len = 0", 0, 0, 0);
      return GSTK_BAD_PARAM;
    }

    /* get the envelope refernce id */
    gstk_status =  gstk_util_save_env_cmd_info(
                              0,
                              gstk_sms_pp_dl_cb,
                              GSTK_SMS_PP_DOWNLOAD_IND,
                              user_data,
                              &env_ref_id);
    if(gstk_status == GSTK_SUCCESS) {

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
          /* build the envelope command */
          task_cmd->cmd.sms_pp_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
          task_cmd->cmd.sms_pp_envelope_ind.message_header.command_id = (int)GSTK_SMS_PP_DOWNLOAD_IND;
          /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
          task_cmd->cmd.sms_pp_envelope_ind.message_header.user_data = env_ref_id;
          /* type of technology */
          task_cmd->cmd.sms_pp_envelope_ind.is_cdma_sms_pp = is_cdma_sms_pp;
          /* TPDU */
          task_cmd->cmd.sms_pp_envelope_ind.tpdu.length = sms_tpdu->length;
          task_cmd->cmd.sms_pp_envelope_ind.tpdu.tpdu = 
            gstk_malloc(sms_tpdu->length);
          if(task_cmd->cmd.sms_pp_envelope_ind.tpdu.tpdu == NULL) {
            /* memory_error */
            /* put the queue back to the free queue */
            (void)q_link(task_cmd, &task_cmd->cmd_hdr.link);
            q_put(task_cmd->cmd_hdr.done_q_ptr, &(task_cmd->cmd_hdr.link));
            /* free the env slot */
            gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
            MSG_ERROR("Not Able to Allocate TPDU memory", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(task_cmd->cmd.sms_pp_envelope_ind.tpdu.tpdu, sms_tpdu->tpdu, sms_tpdu->length) ;

          /* optional address */
          task_cmd->cmd.sms_pp_envelope_ind.address.length = 0;

          if(address != NULL) {
            if(address->length != 0) {
              /* has address */
              task_cmd->cmd.sms_pp_envelope_ind.address.length = address->length;
              task_cmd->cmd.sms_pp_envelope_ind.address.TON = address->TON;
              task_cmd->cmd.sms_pp_envelope_ind.address.NPI = address->NPI;
              memcpy(task_cmd->cmd.sms_pp_envelope_ind.address.address,
                     address->address, address->length);
            }
          }

          /* set GSTK_CMD_Q_SIG and put on GSTK queue */
          gstk_task_put_cmd_buf(task_cmd);
      }
      else {
          ERR("No more gstk buf", 0, 0, 0);
          /* free the env slot */
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
          return GSTK_MEMORY_ERROR;
      }

    }

    return GSTK_SUCCESS;
} /*gstk_send_envelope_sms_pp_dl_command*/


/*===========================================================================
FUNCTION GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE

DESCRIPTION

  This function allows clients to update the terminal profile before gstk
  sends it down to gsdi and uim server.
  GSTK will send the profile download events to the card after it receives
  the first profile download update value from one of its client.
  GSTK will ignore the subsequence profile download updates.

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  num_updates: [Input] Specifies the number of gstk_profile_dl_support_type
                       that the clients are specifying in this function
  value_list: [Input] Pointer to list of new updates

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_client_init has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_PROFILE_VALUE_NOT_ALLOWED,
    GSTK_PROFILE_ALREADY_SENT,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_FILE_ACCESS_FAIL,
    GSTK_NOT_INIT

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_update_profile_download_value (
    gstk_client_id_type                    client_id,
    uint32                                 user_data,
    uint8                                  num_updates,
    const gstk_profile_dl_support_type    *value_list )
{
    gstk_status_enum_type            gstk_status = GSTK_SUCCESS;
    int                              i           = 0;
    gstk_profile_dl_support_ext_type *tp_ext_ptr = NULL;

    MSG_HIGH("GSTK client profile download", 0, 0, 0);

    if (value_list == NULL) {
        MSG_ERROR("Null input param", 0, 0, 0);
        gstk_status = GSTK_NULL_INPUT_PARAM;
        return gstk_status;
    }

    tp_ext_ptr = gstk_malloc(
                   num_updates * sizeof(gstk_profile_dl_support_ext_type));
    if (tp_ext_ptr)
    {
      for (i = 0; i < num_updates; ++ i)
      {
        tp_ext_ptr->card_app_type = (uint8)GSTK_APP_NONE;
        tp_ext_ptr->profile_dl_bitmask = value_list[i].profile_dl_bitmask;
        tp_ext_ptr->value = value_list[i].value;
      }
      gstk_status = gstk_update_profile_download_value_ext(
                      client_id,
                      user_data,
                      num_updates,
                      tp_ext_ptr);
      gstk_free(tp_ext_ptr);
    }
    else
    {
      MSG_ERROR("Failed to allocate tp_ext_ptr!", 0, 0, 0);
      gstk_status = GSTK_MEMORY_ERROR;
    }

    return gstk_status;
} /*gstk_update_profile_download_value */


/*===========================================================================
FUNCTION GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE_EXT

DESCRIPTION
  This function allows clients to update the terminal profile before gstk
  sends it down to gsdi and uim server.
  GSTK will send the profile download events to the card after it receives
  the first profile download update value from one of its client.
  GSTK will ignore the subsequence profile download updates.
  This function differs from function GSTK_UPDATE_PROFILE_DOWNLOAD_VALUE() 
  as the former supports differentiating sat Vs usat terminal profile while the
  latter does not.

PARAMETERS
  client_id:     [Input] Client id of the calling client.  This should be the
                         value returned from the gstk_client_init()
  user_data:     [Input] User data that client wants GSTK to keep track of
  num_updates:   [Input] Specifies the number of gstk_profile_dl_support_type
                         that the clients are specifying in this function
  value_list_ptr:[Input] Pointer to list of new updates

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  This function should only be called after gstk_client_init has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_PROFILE_VALUE_NOT_ALLOWED,
    GSTK_PROFILE_ALREADY_SENT,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_FILE_ACCESS_FAIL,
    GSTK_NOT_INIT

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_update_profile_download_value_ext (
    gstk_client_id_type                        client_id,
    uint32                                     user_data,
    uint8                                      num_updates,
    const gstk_profile_dl_support_ext_type    *value_list_ptr )
{
    gstk_status_enum_type   gstk_status   = GSTK_SUCCESS;
    gstk_task_cmd_type      *task_cmd_ptr = NULL;
    int32                   i             = 0;
    int32                   j             = 0;

    MSG_HIGH("GSTK client profile download", 0, 0, 0);

    if (value_list_ptr == NULL) {
        MSG_ERROR("Null input param", 0, 0, 0);
        gstk_status = GSTK_NULL_INPUT_PARAM;
        return gstk_status;
    }

    /* for each terminal profile data, we will copy 4 bytes to
       gstk_task_cmd_type.terminal_profile_cmd.data buffer */
    i = num_updates * 4;
    if (i > GSTK_MAX_TP_DATA_SIZE)
    {
      MSG_ERROR("TP data is too long (0x%x)!", i, 0, 0);
      return GSTK_INVALID_LENGTH;
    }

    if (gstk_util_is_valid_client_id(client_id)) {
        /* get command buf */
        task_cmd_ptr = gstk_task_get_cmd_buf();
        if ( task_cmd_ptr != NULL ) { /* successfully obtained command buffer */
             /* build the terminal profile command */
             task_cmd_ptr->cmd.terminal_profile_cmd.message_header.command_group = GSTK_TERMINAL_PROFILE_CMD;
             task_cmd_ptr->cmd.terminal_profile_cmd.message_header.command_id = 0; /* doesn't matter */
             task_cmd_ptr->cmd.terminal_profile_cmd.message_header.user_data = user_data;

             for (i = 0; i < num_updates; i++) {
               task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] = 
                   (uint8)value_list_ptr[i].card_app_type;
               task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] = 
                   (uint8)((value_list_ptr[i].profile_dl_bitmask & 0xFF00) >> 0x0008);
               task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] = 
                   (uint8)(value_list_ptr[i].profile_dl_bitmask & 0x00FF);
               task_cmd_ptr->cmd.terminal_profile_cmd.data[j++] = 
                   value_list_ptr[i].value;
             }
             task_cmd_ptr->cmd.terminal_profile_cmd.length = int32touint32(j);
             /* put command back to command queue */
             gstk_task_put_cmd_buf(task_cmd_ptr);
        }
        else {
             gstk_status = GSTK_MEMORY_ERROR;
        }
    }
    else {
        MSG_ERROR("Client 0x%x not registered", client_id, 0, 0);
        gstk_status = GSTK_CLIENT_NOT_REGISTERED;
    }
    return gstk_status;
} /*gstk_update_profile_download_value_ext */


/*===========================================================================
FUNCTION gstk_map_proactive_req_to_cnf

DESCRIPTION
  This function mapped the passed in command req type to the corresponding
  response type

PARAMETERS
  request_cmd: [Input] Original proactive command REQ to be mapped
  expected_cmd_rsp: [Input/Output] Pointer to response command type (CNF)
                                   to be populated

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
gstk_status_enum_type gstk_map_proactive_req_to_cnf(
                      gstk_cmd_enum_type  request_cmd,
                      gstk_cmd_enum_type *expected_cmd_rsp)
{

  MSG_HIGH("client map req to cnf", 0, 0, 0);

  if(expected_cmd_rsp == NULL) {
    MSG_ERROR("Null Input Param", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(request_cmd)
  {
    case GSTK_DISPLAY_TEXT_REQ:
        *expected_cmd_rsp = GSTK_DISPLAY_TEXT_CNF;
        break;
    case GSTK_GET_INKEY_REQ:
        *expected_cmd_rsp = GSTK_GET_INKEY_CNF;
        break;
    case GSTK_GET_INPUT_REQ:
        *expected_cmd_rsp = GSTK_GET_INPUT_CNF;
        break;
    case GSTK_LAUNCH_BROWSER_REQ:
        *expected_cmd_rsp = GSTK_LAUNCH_BROWSER_CNF;
        break;
    case GSTK_MORE_TIME_REQ:
        *expected_cmd_rsp = GSTK_MORE_TIME_CNF;
        break;
    case GSTK_PLAY_TONE_REQ:
        *expected_cmd_rsp = GSTK_PLAY_TONE_CNF;
        break;
    case GSTK_POLL_INTERVAL_REQ:
        *expected_cmd_rsp = GSTK_POLL_INTERVAL_CNF;
        break;
    case GSTK_PROVIDE_LOCAL_INFO_REQ:
        *expected_cmd_rsp = GSTK_PROVIDE_LOCAL_INFO_CNF;
        break;
    case GSTK_PROVIDE_LANG_INFO_REQ:
        *expected_cmd_rsp = GSTK_PROVIDE_LANG_INFO_CNF;
        break;
    case GSTK_REFRESH_REQ:
        *expected_cmd_rsp = GSTK_REFRESH_CNF;
        break;
    case GSTK_RUN_AT_CMDS_REQ:
        *expected_cmd_rsp = GSTK_RUN_AT_CMDS_CNF;
        break;
    case GSTK_SELECT_ITEM_REQ:
        *expected_cmd_rsp = GSTK_SELECT_ITEM_CNF;
        break;
    case GSTK_SEND_SMS_REQ:
        *expected_cmd_rsp = GSTK_SEND_SMS_CNF;
        break;
    case GSTK_SEND_SS_REQ:
        *expected_cmd_rsp = GSTK_SEND_SS_CNF;
        break;
    case GSTK_SEND_USSD_REQ:
        *expected_cmd_rsp = GSTK_SEND_USSD_CNF;
        break;
    case GSTK_SETUP_CALL_REQ:
        *expected_cmd_rsp = GSTK_SETUP_CALL_CNF;
        break;
    case GSTK_SETUP_EVENT_LIST_REQ:
        *expected_cmd_rsp = GSTK_SETUP_EVENT_LIST_CNF;
        break;
    case GSTK_SETUP_MENU_REQ:
        *expected_cmd_rsp = GSTK_SETUP_MENU_CNF;
        break;
    case GSTK_SETUP_IDLE_TEXT_REQ:
        *expected_cmd_rsp = GSTK_SETUP_IDLE_TEXT_CNF;
        break;
    case GSTK_SEND_DTMF_REQ:
        *expected_cmd_rsp = GSTK_SEND_DTMF_CNF;
        break;
    case GSTK_LANG_NOTIFICATION_REQ:
        *expected_cmd_rsp = GSTK_LANG_NOTIFICATION_CNF;
        break;
    default:
        MSG_ERROR("Unknown GSTK REQ Type 0x%x", request_cmd, 0, 0);
        *expected_cmd_rsp = GSTK_END_PROACTIVE_CMD_REQ;
        return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_map_proactive_cmd_cnf_rsp*/


/*===========================================================================
FUNCTION gstk_parse_ton_npi

DESCRIPTION
  This function parse the ton and npi byte into gstk enum types

PARAMETERS
  TON_NPI: [Input] byte that contains TON and NPI info
  TON: [Input/Output] Pointer to TON
  NPI: [Input/Output] Pointer to NPI


DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parse_ton_npi(
                                        uint8 TON_NPI,
                                        gstk_ton_enum_type *TON,
                                        gstk_npi_enum_type *NPI)
{

  if (TON == NULL || NPI == NULL) {
    MSG_ERROR("Null Input Para", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  
  *TON = (gstk_ton_enum_type)((TON_NPI & GSTK_TON_MASK_VALUE) >> GSTK_TON_SHIFT_VALUE);
  switch(*TON) {
  case GSTK_UNKNOWN_TON:
    *TON = GSTK_UNKNOWN_TON;
    MSG_MED("Unknown TON: %x", *TON, 0, 0);
    break;
  case GSTK_INTERNATIONAL_NUM:
    *TON = GSTK_INTERNATIONAL_NUM;
    MSG_MED("International TON: %x", *TON, 0, 0);
    break;
  case GSTK_NATIONAL_NUM:
    *TON = GSTK_NATIONAL_NUM;
    MSG_MED("National TON: %x", *TON, 0, 0);
    break;
  case GSTK_NETWORK_SPECIFIC_NUM:
    *TON = GSTK_NETWORK_SPECIFIC_NUM;
    MSG_MED("Specific TON: %x", *TON, 0, 0);
    break;
  default:
    *TON = GSTK_RFU_TON;
    MSG_MED("RFU TON: %x", *TON, 0, 0);
    break;
  }

  *NPI = (gstk_npi_enum_type)(TON_NPI & GSTK_NPI_MASK_VALUE);
  switch(*NPI) {
  case GSTK_UNKNOWN_NPI:
   *NPI = GSTK_UNKNOWN_NPI;
   MSG_MED("Unknown NPI: %x", *NPI, 0, 0);
   break;
  case GSTK_ISDN_TELEPHONY_NPI:
   *NPI = GSTK_ISDN_TELEPHONY_NPI;
   MSG_MED("ISDN Telephony NPI: %x", *NPI, 0, 0);
   break;
  case GSTK_DATA_NPI:
   *NPI = GSTK_DATA_NPI;
   MSG_MED("DATA NPI: %x", *NPI, 0, 0);
   break;
  case GSTK_TELEX_NPI:
   *NPI = GSTK_TELEX_NPI;
   MSG_MED("Telex NPI: %x", *NPI, 0, 0);
   break;
  case GSTK_PRIVATE_NPI:
   *NPI = GSTK_PRIVATE_NPI;
   MSG_MED("Private NPI: %x", *NPI, 0, 0);
   break;
  case GSTK_EXT_RESERVED_NPI:
   *NPI = GSTK_EXT_RESERVED_NPI;
   MSG_MED("ext reserved NPI: %x", *NPI, 0, 0);
   break;
  case GSTK_RFU_NPI:
  default:
   *NPI = GSTK_RFU_NPI;
   MSG_MED("RFU NPI: %x", *NPI, 0, 0);
   break;
  }
  return GSTK_SUCCESS;
}


/*              for internal module use                   */


/*===========================================================================
FUNCTION gstk_send_proactive_cmd

DESCRIPTION

  This function allows clients to put a proactive cmd request before parsing
  onto GSTK command queue
  queue.

PARAMETERS
  data_length: [Input] Length of the data
  data: [Input] Pointer to the data buffer that contains the Proactive cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */
gstk_status_enum_type gstk_send_proactive_cmd(
  uint32        data_length,
  const uint8  *data )
{
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  gstk_task_cmd_type      * task_cmd;

  if (data != NULL) {
    if (data_length > GSTK_MAX_DATA_SIZE) {
      MSG_ERROR("Data Len 0x%x > GSTK_MAX_DATA_SIZE", data_length, 0, 0);
      gstk_status = GSTK_PARAM_EXCEED_BOUNDARY;
    }
    else {
      /* get the command from buffer */
      task_cmd = gstk_task_get_cmd_buf();
      if (task_cmd != NULL) {
      /* Build the gstk_cmd */

        task_cmd->cmd.proactive_cmd.message_header.command_group = GSTK_PROACTIVE_CMD;
        task_cmd->cmd.proactive_cmd.message_header.command_id = 0; /* doesn't matter */
        task_cmd->cmd.proactive_cmd.message_header.user_data = 0;  /* doesn't matter */

        task_cmd->cmd.proactive_cmd.length = data_length;
        memcpy(task_cmd->cmd.proactive_cmd.data, data, data_length);

        /* put command back to command queue */
        gstk_task_put_cmd_buf(task_cmd);
      }
      else {
        MSG_ERROR("No more GSTK cmd buf", 0, 0, 0);
        gstk_status = GSTK_ERROR;
      }
    }
  }
  else {
    MSG_ERROR("Null Input Param", 0, 0, 0);
    gstk_status = GSTK_NULL_INPUT_PARAM;
  }
  return gstk_status;
} /* gstk_send_proactive_cmd */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_NON_DEMAND_PAGED_FUNCTION */

/*===========================================================================
FUNCTION gstk_is_cc_required

DESCRIPTION
  Utility function to check if call control is required for any MO calls

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_cc_required(void)
{
  return gstk_slot_is_cc_required(GSTK_SLOT_1);

} /* gstk_is_cc_required */

/*lint -e715 slot not being used */
/*===========================================================================
FUNCTION gstk_is_gprs_cc_required

DESCRIPTION
  Utility function to check if call control for PDP Context Activation
  is required for any GPRS Data calls

PARAMETER
  slot ID

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_gprs_cc_required(gstk_slot_id_enum_type slot)
{
#ifdef FEATURE_GPRS_CALLCONTROL
#error code not present
#else /* #ifdef FEATURE_GPRS_CALLCONTROL*/
  MSG_HIGH("Feature GPRS Call Control not supported",0,0,0);
  return FALSE;
#endif /* #ifdef FEATURE_GPRS_CALLCONTROL*/
}
/*lint +e715 slot not being used */
/*===========================================================================
FUNCTION gstk_is_mo_sms_ctrl_required

DESCRIPTION
  Utility function to check if mo sms control is required for any MO calls

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_mo_sms_ctrl_required(void)
{
  uint8 me_support = 0;
  gsdi_svr_rsp_type service;
  gsdi_card_apps_T  card_app;

  me_support =
  (gstk_terminal_profile_cache[GSTK_MO_SMS_CTRL_TERM_PROFILE_INDEX])&GSTK_MO_SMS_CTRL_TERM_PROFILE_MASK;

  gsdi_get_apps_available(&card_app);
  if(me_support == GSTK_MO_SMS_CTRL_TERM_PROFILE_MASK) {
    if ((card_app.apps_enabled &  GSDI_USIM_APP_MASK) == GSDI_USIM_APP_MASK) {
      /* Check service table bit */
      service = gsdi_lib_is_service_available(GSDI_MOSMS_CONTROL);
    }
    else {
      /* Check service table bit */
      service = gsdi_lib_is_service_available(GSDI_MO_SMS_BY_SIM);
    }
    MSG_HIGH("MO SMS Ctrl Service 0x%x", service.svr_bitmap, 0, 0);
    if((((service.svr_bitmap)&0x10) == 0x10) || (((service.svr_bitmap)&0x01) == 0x01)) {
       return TRUE;
    }
  }
  return FALSE;

} /* gstk_is_mo_sms_ctrl_required */


/*===========================================================================
FUNCTION gstk_is_sms_pp_supported

DESCRIPTION
  Utility function to check if SMS PP Download is allowed/supported

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_sms_pp_supported(void)
{
  gsdi_svr_rsp_type                srv_available = {0};
    
  /* Check if terminal profile included SMS-PP service */
  if(gstk_terminal_profile_cache[GSTK_SMS_PP_TERM_PROFILE_INDEX] &
     GSTK_SMS_PP_TERM_PROFILE_MASK)
  {
    /* Check if SIM Service Table included SMS-PP service */
    srv_available = gsdi_lib_is_service_available(GSDI_DATA_DL_SMSPP);

    if (GSDI_SUCCESS == srv_available.gsdi_status &&
        0 != srv_available.svr_bitmap)
    {
      return TRUE;
    }
  }

  return FALSE;
} /* gstk_is_sms_pp_supported */

/*===========================================================================
FUNCTION gstk_is_cell_broadcast_dl_supported

DESCRIPTION
  Utility function to check if Cell Broadcast Download is allowed/supported

PARAMETER
  slot [Input]: Indicates which slot the query is intended for

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_cell_broadcast_dl_supported(gstk_slot_id_enum_type slot)
{
  uint8 me_support = 0;
  gsdi_svr_rsp_type service;

  me_support = (gstk_terminal_profile_cache[GSTK_CELL_BROADCAST_TERM_PROFILE_INDEX])&GSTK_CELL_BROADCAST_TERM_PROFILE_MASK;

  if(me_support == GSTK_CELL_BROADCAST_TERM_PROFILE_MASK) {
    switch (slot)
    {
    case GSTK_SLOT_1:
      /* Check service table bit */
      service = gsdi_lib_is_service_available(GSDI_DATA_DL_SMSCB);
      break;

#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSTK_SLOT_2:      
      /* Check service table bit */
      service = gsdi2_lib_is_service_available(GSDI_SLOT_2, GSDI_DATA_DL_SMSCB);
      break;
#endif /*FEATURE_GSTK_DUAL_SLOTS */
    
    default:
      ERR("Invalid Slot ID", 0, 0, 0);
      return FALSE;
    }
    MSG_HIGH("Cell Broadcast Service 0x%x", service.svr_bitmap, 0, 0);
    if((((service.svr_bitmap)&0x10) == 0x10) || (((service.svr_bitmap)&0x01) == 0x01)) {
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    return FALSE;
  }

} /* gstk_is_cellbroad_dl_supported */


/*===========================================================================
FUNCTION GSTK_SLOT_SEND_ENVELOPE_CC_COMMAND

DESCRIPTION

  This function sends the Call Control envelope command to SIM/USIM based on
  the slot information provided
  Clients need to supply the gstk_cc_ind_type as the parameter.
  within the gstk_cc_ind_type,
  Also, the gstk_address_string is required based on whether the call is
  voice call, SS or USSD
  there are also 3 optional data fields: subaddress, ccp1 and ccp2 for voice
  call related Call Control envelope command
  This function does not require client ID because it is intended to be called
  by internal modules.

PARAMETERS
  slot: [Input] Indicates if the user wants to go to Slot 1 or Slot 2 for 
                the Call control request
  user_data: [Input] User data that client wants GSTK to keep track of
  cc_ind_param: [Input] Pointer to the call control envelope command info
  gstk_cc_cb: [Input] Function pointer to which GSTK will send
                      the card response data corresponding to the
                      Call Control command

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_cc_command (
    gstk_slot_id_enum_type    slot,
    uint32                    user_data,
    const gstk_cc_ind_type   *cc_ind_param,
    gstk_evt_cb_funct_type    gstk_cc_cb )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type    * task_cmd;
    uint32                  env_ref_id;

    /* check for slot first */
    switch (slot)
    {
    case GSTK_SLOT_1:
#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSTK_SLOT_2:
#endif /*FEATURE_GSTK_DUAL_SLOTS */
      break;
    default:
      ERR("Unknown slot", 0, 0, 0);
      return GSTK_BAD_PARAM;
    }

    MSG_HIGH("GSTK client send cc", 0, 0, 0);
    /* validate call back function */
    if (gstk_cc_cb == NULL || cc_ind_param == NULL) {
      MSG_ERROR("Null Input Param", 0, 0, 0);
      return GSTK_NULL_INPUT_PARAM;
    }
    /* check for mandatory data */
    switch(cc_ind_param->call_enum_type) {
    case GSTK_VOICE:
      if(cc_ind_param->gstk_address_string.voice.address.length == 0) {
        MSG_ERROR("address len == 0", 0, 0, 0);
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_USSD:
      if(cc_ind_param->gstk_address_string.ussd_string.length == 0) {
        MSG_ERROR("ussd len == 0", 0, 0, 0);
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_SS:
      if(cc_ind_param->gstk_address_string.ss_string.length == 0) {
        MSG_ERROR("ss len == 0", 0, 0, 0);
        return GSTK_BAD_PARAM;
      }
      break;
    case GSTK_PDP_CONTEXT_ACT:
      if(cc_ind_param->gstk_address_string.pdp_context.length == 0) {
        MSG_ERROR("pdp context len == 0", 0, 0, 0);
        return GSTK_BAD_PARAM;
      }
      break;
    default:
      MSG_ERROR("Unknown call enum type 0x%x", cc_ind_param->call_enum_type, 0, 0);
      return GSTK_BAD_PARAM;
    }

    /* get the envelope refernce id */
    gstk_status =  gstk_util_save_env_cmd_info(
                              0,
                              gstk_cc_cb,
                              GSTK_CC_IND,
                              user_data,
                              &env_ref_id);
    if(gstk_status == GSTK_SUCCESS) {

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
          /* build the envelope command */
          task_cmd->cmd.cc_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
          task_cmd->cmd.cc_envelope_ind.message_header.command_id = (int)GSTK_CC_IND;
          /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
          task_cmd->cmd.cc_envelope_ind.message_header.user_data = env_ref_id;
          /* address/ss/ussd */
          task_cmd->cmd.cc_envelope_ind.cc_data.call_enum_type =
            cc_ind_param->call_enum_type;
          switch(cc_ind_param->call_enum_type) {
          case GSTK_VOICE:
            memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.address,
                   cc_ind_param->gstk_address_string.voice.address.address,
                   cc_ind_param->gstk_address_string.voice.address.length);
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.TON =
              cc_ind_param->gstk_address_string.voice.address.TON;
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.NPI =
              cc_ind_param->gstk_address_string.voice.address.NPI;
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.address.length =
              cc_ind_param->gstk_address_string.voice.address.length;
            /* initialize optional ccp1/ccp2 and subaddress */
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp1.length = 0;
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp2.length = 0;
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.subaddress.length = 0;
            /*optional ccp1 */
            if(cc_ind_param->gstk_address_string.voice.ccp1.length > 0) {
              task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp1.length =
                cc_ind_param->gstk_address_string.voice.ccp1.length;
              gstk_memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp1.ccp,
                          cc_ind_param->gstk_address_string.voice.ccp1.ccp,
                          cc_ind_param->gstk_address_string.voice.ccp1.length, 
                          GSTK_MAX_CCP_LEN,
                          GSTK_MAX_CCP_LEN);
            }
            /*optional ccp2 */
            if(cc_ind_param->gstk_address_string.voice.ccp2.length > 0) {
              task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp2.length =
                cc_ind_param->gstk_address_string.voice.ccp2.length;
              gstk_memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.ccp2.ccp,
                          cc_ind_param->gstk_address_string.voice.ccp2.ccp,
                          cc_ind_param->gstk_address_string.voice.ccp2.length, 
                          GSTK_MAX_CCP_LEN,
                          GSTK_MAX_CCP_LEN);
            }
            /*optional subaddress */
            if(cc_ind_param->gstk_address_string.voice.subaddress.length > 0) {
              task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.subaddress.length =
                cc_ind_param->gstk_address_string.voice.subaddress.length;
              gstk_memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.voice.subaddress.subaddress,
                          cc_ind_param->gstk_address_string.voice.subaddress.subaddress,
                          cc_ind_param->gstk_address_string.voice.subaddress.length, 
                          GSTK_MAX_SUBADDR_LEN,
                          GSTK_MAX_SUBADDR_LEN);
            }
            break;
          case GSTK_USSD:
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text =
                   gstk_malloc(cc_ind_param->gstk_address_string.ussd_string.length);
            if(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text == NULL) {
              /* memory_error */
              /* put the queue back to the free queue */
              (void)q_link(task_cmd, &task_cmd->cmd_hdr.link);
              q_put(task_cmd->cmd_hdr.done_q_ptr, &(task_cmd->cmd_hdr.link));
              /* free the env slot */
              gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
              MSG_ERROR("Can't allocate ussd string memory", 0, 0, 0);
              return GSTK_MEMORY_ERROR;
            }
            memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text,
                   cc_ind_param->gstk_address_string.ussd_string.text,
                   cc_ind_param->gstk_address_string.ussd_string.length);
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.dcs =
              cc_ind_param->gstk_address_string.ussd_string.dcs;
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ussd_string.length =
              cc_ind_param->gstk_address_string.ussd_string.length;
            break;
          case GSTK_SS:
            memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.address,
                   cc_ind_param->gstk_address_string.ss_string.address,
                   cc_ind_param->gstk_address_string.ss_string.length);
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.TON =
              cc_ind_param->gstk_address_string.ss_string.TON;
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.NPI =
              cc_ind_param->gstk_address_string.ss_string.NPI;
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.ss_string.length =
              cc_ind_param->gstk_address_string.ss_string.length;
            break;
          case GSTK_PDP_CONTEXT_ACT:
            memcpy(task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.pdp_context.pdp_act_param,
                   cc_ind_param->gstk_address_string.pdp_context.pdp_act_param,
                   int32touint32(cc_ind_param->gstk_address_string.pdp_context.length));
            task_cmd->cmd.cc_envelope_ind.cc_data.gstk_address_string.pdp_context.length =
              cc_ind_param->gstk_address_string.pdp_context.length;
            break;
          default:
            break;
          }

          /* set the slot */
          task_cmd->cmd.cc_envelope_ind.slot = slot;
          task_cmd->cmd.cc_envelope_ind.slot_info_present = TRUE;

          /* set GSTK_CMD_Q_SIG and put on GSTK queue */
          gstk_task_put_cmd_buf(task_cmd);
      }
      else {
          ERR("No more gstk buf", 0, 0, 0);
          /* free the env slot */
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
          return GSTK_MEMORY_ERROR;
      }
    }
    return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_slot_is_cc_required

DESCRIPTION
  Utility function to check if call control is required for any MO calls

PARAMETER
  slot [Input]: Indicates which slot the query is intended for

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_slot_is_cc_required(gstk_slot_id_enum_type slot)
{
  uint8 me_support = 0;
  gsdi_svr_rsp_type service;

  me_support = (gstk_terminal_profile_cache[GSTK_CC_TERM_PROFILE_INDEX])&GSTK_CC_TERM_PROFILE_MASK;

  if(me_support == GSTK_CC_TERM_PROFILE_MASK) {

    switch (slot)
    {
    case GSTK_SLOT_1:
      /* Check service table bit */
      service = gsdi_lib_is_service_available(GSDI_CALL_CONTROL);
      break;

#ifdef FEATURE_GSTK_DUAL_SLOTS
    case GSTK_SLOT_2:      
      /* Check service table bit */
      service = gsdi2_lib_is_service_available(GSDI_SLOT_2, GSDI_CALL_CONTROL);
      break;
#endif /*FEATURE_GSTK_DUAL_SLOTS */
    
    default:
      ERR("Invalid Slot ID", 0, 0, 0);
      return FALSE;
    }

    MSG_HIGH("Call Ctrl Service 0x%x", service.svr_bitmap, 0, 0);
    /* 0x10 is for USIM(UST) and 0x01 is for SIM (SST) */
    if((((service.svr_bitmap)&0x10) == 0x10) || (((service.svr_bitmap)&0x01) == 0x01)) {
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION gstk_address_to_ascii

DESCRIPTION
  This function converts the gstk address foramt to ASCII value

PARAMETERS
  gstk_addr: [Input] Pointer to input address
  ascii_addr: [Input/Output] Pointer to output address
  ascii_len; [Input] Length of the ascii_addr buffer, to ensure the size of
                     the ascii_addr will not exceed the information that is
                     being convertedh of the ascii_addr buffer;


DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_address_to_ascii(
  const gstk_address_type *gstk_addr,
  uint8                   *ascii_addr,
  uint8                    ascii_len)
{
  int i = 0;
  uint8 addr_len = 0;

  if (gstk_addr == NULL || ascii_addr == NULL) 
  {
    MSG_ERROR("Null Input Param", 0, 0, 0);
    return GSTK_NULL_INPUT_PARAM;
  }
  if (ascii_len < gstk_addr->length)
  {
    MSG_ERROR("ascii len 0x%x < gstk addr len 0x%x", ascii_len, gstk_addr->length, 0);
    return GSTK_BAD_PARAM;
  }

  memset(ascii_addr, 0x00, ascii_len);
  if (gstk_addr->length > GSTK_MAX_ADDRESS_LEN) {
    MSG_ERROR("gstk_addr->length exceeds GSTK_MAX_ADDRESS_LEN",0,0,0);
    addr_len = GSTK_MAX_ADDRESS_LEN;
  }
  else {
    addr_len = gstk_addr->length;
  }
  for (i=0; i< addr_len; i++) {
    switch (gstk_addr->address[i])
    {
      case 0x0A:
        ascii_addr[i] = 0x2A;
        break;
      case 0x0B:
        ascii_addr[i] = 0x23;
        break;
      default:
        ascii_addr[i] = gstk_addr->address[i] + 0x30;
        break;
    }
  }
  return GSTK_SUCCESS;
} /* gstk_address_to_ascii */

/*===========================================================================
FUNCTION gstk_ascii_to_gstk_address_type

DESCRIPTION
  This function converts the ASCII value to gstk address format

PARAMETERS
  ascii_addr: [Input] Pointer to input address
  ascii_len: [Input] Input address length
  gstk_addr: [Input/Output] Pointer to output address


DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type:
      GSTK_SUCCESS
      GSTK_NULL_INPUT_PARAM

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_ascii_to_gstk_address_type(
  const uint8       *ascii_addr,
  uint8              ascii_len,
  gstk_address_type *gstk_addr)
{
  int i = 0;
  if (gstk_addr == NULL || ascii_addr == NULL) 
  {
    return GSTK_NULL_INPUT_PARAM;
  }
     
  if (ascii_len > GSTK_MAX_ADDRESS_LEN) {
    MSG_ERROR("ascii len 0x%x > GSTK_MAX_ADDRESS_LEN", ascii_len, 0, 0);
    return GSTK_BAD_PARAM;
  }
  memset(gstk_addr->address, 0x00, GSTK_MAX_ADDRESS_LEN);

  for (i = 0; i < ascii_len; i++)
  {
    switch (ascii_addr[i]) {
    case 0x2A:
      gstk_addr->address[i] = 0x0A;
      break;
    case 0x23:
      gstk_addr->address[i] = 0x0B;
      break;
    default:
      gstk_addr->address[i] = ascii_addr[i] - 0x30;
      break;
    }
  }
  gstk_addr->length = ascii_len;
  return GSTK_SUCCESS;

} /* gstk_ascii_to_gstk_address_type */

/*===========================================================================
FUNCTION gstk_is_ussd_object_supported

DESCRIPTION
  Utility function to check if the USSD object in call control is supported
  by the SIM or not.

PARAMETER
  slot [Input]: Indicates which slot the query is intended for

DEPENDENCIES
  Client is required to check gstk_slot_is_cc_required or gstk_is_cc_required
  together with this function in order to determine if USSD call control is
  required before sending down the envelope command for USSD call control

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_ussd_object_supported(gstk_slot_id_enum_type slot)
{
  gsdi_svr_rsp_type service;
  gsdi_card_apps_T  card_apps;

  gsdi_get_apps_available( &card_apps );

  switch (slot)
  {
  case GSTK_SLOT_1:
    /* Check service table bit */
    service = gsdi_lib_is_service_available(GSDI_CALL_CONTROL);
    if (card_apps.apps_enabled & GSDI_USIM_APP_MASK) {
      service.svr_bitmap |= 0x10;
      service.gsdi_status = GSDI_SUCCESS;
    }
    break;

#ifdef FEATURE_GSTK_DUAL_SLOTS
  case GSTK_SLOT_2:      
    /* Check service table bit */
    service = gsdi2_lib_is_service_available(GSDI_SLOT_2, GSDI_CALL_CONTROL);
    if (card_apps.apps2_enabled & GSDI_USIM_APP_MASK) {
      service.svr_bitmap |= 0x10;
      service.gsdi_status = GSDI_SUCCESS;
    }
    break;
#endif /*FEATURE_GSTK_DUAL_SLOTS */
  
  default:
    ERR("Invalid Slot ID", 0, 0, 0);
    return FALSE;
  }

  MSG_HIGH("USSD Obj Service 0x%x", service.svr_bitmap, 0, 0);
  if((((service.svr_bitmap)&0x10) == 0x10) || (((service.svr_bitmap)&0x01) == 0x01)) {
    return TRUE;
  }
  return FALSE;  
}

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a cell broadcast download command to the
  SIM/USIM.

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  This function should only be called after gstk_client_init has been called.  
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_cell_broadcast_dl_command (
  uint32                     user_data,
  const gstk_cb_page_type   *cb_page_ptr,
  gstk_evt_cb_funct_type     gstk_cb_dl_cb )
{
    gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
    gstk_task_cmd_type      * task_cmd;
    uint32                  env_ref_id;

    MSG_HIGH("GSTK client cell broadcast dl", 0, 0, 0);

    /* validate call back function */
    if (gstk_cb_dl_cb == NULL || cb_page_ptr == NULL) {
      MSG_ERROR("Null Input Param", 0, 0, 0);
      return GSTK_NULL_INPUT_PARAM;
    }

    /* If padding is required, it is required to be done by
       the caller */
    if(cb_page_ptr->length != GSTK_CELL_BROADCAST_PAGE_LEN) {
      /* mandatory param */
      MSG_ERROR("Cell Broadcast len = 0x%x", cb_page_ptr->length, 0, 0);
      return GSTK_BAD_PARAM;
    }

    if(cb_page_ptr->cb_page_data == NULL) {
      /* null page data */
      MSG_ERROR("Cell Broadcast data NULL", 0, 0, 0);
      return GSTK_BAD_PARAM;
    }

    /* get the envelope refernce id */
    gstk_status =  gstk_util_save_env_cmd_info(
                              0,
                              gstk_cb_dl_cb,
                              GSTK_CELL_BROADCAST_DOWNLOAD_IND,
                              user_data,
                              &env_ref_id);
    if(gstk_status == GSTK_SUCCESS) {

      /* get command buf */
      task_cmd = gstk_task_get_cmd_buf();
      if ( task_cmd != NULL ) { /* successfully obtained command buffer */
          /* build the envelope command */
          task_cmd->cmd.cell_broadcast_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
          task_cmd->cmd.cell_broadcast_envelope_ind.message_header.command_id = (int)GSTK_CELL_BROADCAST_DOWNLOAD_IND;
          /* already save the user data in the envelope_cmd_ref_table, set the user_data as the env_ref_id */
          task_cmd->cmd.cell_broadcast_envelope_ind.message_header.user_data = env_ref_id;
          /* Cell Broadcast Page */
          task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.length = cb_page_ptr->length;
          task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data =
            gstk_malloc(cb_page_ptr->length);
          if(task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data == NULL) {
            /* memory_error */
            /* put the queue back to the free queue */
            (void)q_link(task_cmd, &task_cmd->cmd_hdr.link);
            q_put(task_cmd->cmd_hdr.done_q_ptr, &(task_cmd->cmd_hdr.link));
            /* free the env slot */
            gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
            MSG_ERROR("Not Able to Allocate TPDU memory", 0, 0, 0);
            return GSTK_MEMORY_ERROR;
          }
          memcpy(task_cmd->cmd.cell_broadcast_envelope_ind.cb_page.cb_page_data, 
                 cb_page_ptr->cb_page_data, 
                 cb_page_ptr->length) ;

          /* set GSTK_CMD_Q_SIG and put on GSTK queue */
          gstk_task_put_cmd_buf(task_cmd);
      }
      else {
          ERR("No more gstk buf", 0, 0, 0);
          /* free the env slot */
          gstk_util_env_cmd_ref_table_free_slot(env_ref_id);
          return GSTK_MEMORY_ERROR;
      }
    }

    return gstk_status;
} /*gstk_send_envelope_cell_broadcast_dl_command*/


/*===========================================================================
FUNCTION gstk_unpack_7_bit_chars

DESCRIPTION
  This function unpacks the input data from SMS7 bit format into ASCII.
  If out is NULL, the function will return the expected out_len without 
  copying data to the out

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data
  out_len: [Input/Output] Pointer to indicate the output length

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
gstk_status_enum_type gstk_unpack_7_bit_chars ( const uint8    *in,
                                                uint32          in_len,
                                                uint8          *out,
                                                uint32         *out_len )
{
  uint32   i=0;
  uint32   pos=0;
  int      shift = 0;

  if ((in == NULL) || (out_len == NULL)) {
    return GSTK_ERROR;
  }

  if (out == NULL) {
    *out_len = in_len + in_len/7;
    return GSTK_SUCCESS;
  }
  
  for( i=i; pos < in_len; i++, pos++ ) {
    out[i] = ( in[pos] << shift ) & 0x7F;
    if( pos != 0 ) {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }
   
    shift ++;
    if( shift == 7 ) {
      shift = 0;
      /* a possible extra complete character is available */
      i ++;
      out[i] = in[pos] >> 1;
    }
  }
  *out_len = i;
  return GSTK_SUCCESS;
} /* gstk_util_unpack_7_bit_chars() */

#ifdef FEATURE_GSTK_DUAL_SLOTS
/*===========================================================================
FUNCTION gstk_get_curr_tk_slot

DESCRIPTION
  Utility to return the current GSTK toolkit Slot.

PARAMETER
  None

DEPENDENCIES
  Applicable to Dual Slot functionality only

RETURN VALUE
  gstk_slot_id_enum_type
  GSTK_SLOT_1
  GSTK_SLOT_2
  GSTK_SLOT_NONE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_slot_id_enum_type gstk_get_curr_tk_slot(void)
{
  MSG_HIGH("Current GSTK TK Slot is 0x%x", gstk_curr_tk_slot, 0, 0);
  return gstk_curr_tk_slot;
} /* gstk_get_curr_tk_slot */
#endif /* FEATURE_GSTK_DUAL_SLOTS */

#ifdef FEATURE_GSTK_NMR_UTRAN_SUPPORT
/*===========================================================================
FUNCTION   gstk_queue_rrc_utran_nmr_rsp

DESCRIPTION 
  Library function called by RRC to post an NMR response to GSTK for network 
  measurement results for utran.

PARAMETERS
  status:       result status from RRC
  length:       Length of data returned
  nmr_info_ptr: UTRAN NMR info returned from RRC
  data_ptr:     GSTK reference data returned back by RRC

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  Max length of bytes sent by RRC should <= 0xFF

SIDE EFFECTS
  Will queue a UTRAN NMR response to GSTK

SEE ALSO
  None
===========================================================================*/
void gstk_queue_rrc_utran_nmr_rsp(
  gstk_utran_status_enum_type status,
  uint32                      length, 
  const uint8               * nmr_info_ptr,
  const void                * data_ptr)
{
  gstk_task_cmd_type      * task_cmd_ptr = NULL;

  MSG_HIGH(" IN gstk_queue_rrc_utran_nmr_rsp ",0,0,0);

  /* do not check for nmr_info_ptr to be NULL, as RRC may return a NULL ptr for
     error case . In such a case GSTK needs to return an appropriate Terminal 
     Response **Unable to process command**
  */

  if (data_ptr == NULL)
  {
    ERR("Null Data ptr nmr_info_ptr", 0, 0, 0);
    return;
  }

  if(length > 0xFF)
  {
    MSG_HIGH("RRC returned NMR byte length greater than 0xFF",length,0,0);
  }

  /* get the command from buffer */
  task_cmd_ptr = gstk_task_get_cmd_buf(); /* memset is done in this function */
  if(task_cmd_ptr == NULL)
  {
    ERR("GSTK_MEMORY_ERROR", 0, 0, 0);
    return;
  }

  switch(status)
  {
  case GSTK_UTRAN_NMR_RESULT_SUCCESS:
    if( length == 0 && nmr_info_ptr == NULL )
    {
      MSG_HIGH("RRC returned Success for NMR with no data",0,0,0);
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
        GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 1;
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = 
        gstk_malloc(sizeof(uint8));
      if(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr
         == NULL)
      {
        MSG_ERROR("NULL PTR in gstk_queue_rrc_utran_nmr_rsp",0,0,0);
        /* put the queue back to the free queue */
        (void)q_link(task_cmd_ptr, &task_cmd_ptr->cmd_hdr.link);
        q_put(task_cmd_ptr->cmd_hdr.done_q_ptr, &(task_cmd_ptr->cmd_hdr.link));
        return; 
      }
      *(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr) = 
        (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    }
    else
    {
      if(gstk_next_curr_location_status == GSTK_LIMITED_SERVICE)
      {
        task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result = 
          GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE;
      }
      else
      {
        task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
          GSTK_COMMAND_PERFORMED_SUCCESSFULLY;
      }
    }
    break;

  case GSTK_UTRAN_NMR_RESULT_FAIL_UE_ON_GERAN:
  case GSTK_UTRAN_NMR_RESULT_FAIL_UE_OOS:
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 1;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = 
      gstk_malloc(sizeof(uint8));
    if(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr
       == NULL)
    {
      MSG_ERROR("NULL PTR in gstk_queue_rrc_utran_nmr_rsp",0,0,0);
      /* put the queue back to the free queue */
      (void)q_link(task_cmd_ptr, &task_cmd_ptr->cmd_hdr.link);
      q_put(task_cmd_ptr->cmd_hdr.done_q_ptr, &(task_cmd_ptr->cmd_hdr.link));
      return; 
    }
    *(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr) = 
    (uint8)GSTK_NO_SERVICE_AVAILABLE;
    break;

  case GSTK_UTRAN_NMR_RESULT_FAIL_OTHER:
  default:
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_result =
      GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 1;
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = 
      gstk_malloc(sizeof(uint8));
    if(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr
       == NULL)
    {
      MSG_ERROR("NULL PTR in gstk_queue_rrc_utran_nmr_rsp",0,0,0);
      /* put the queue back to the free queue */
      (void)q_link(task_cmd_ptr, &task_cmd_ptr->cmd_hdr.link);
      q_put(task_cmd_ptr->cmd_hdr.done_q_ptr, &(task_cmd_ptr->cmd_hdr.link));
      return; 
    }
    *(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr) = 
    (uint8)GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    break;
  }

  MSG_HIGH("Building terminal response for pli utran nmr...",0,0,0);
  /* Build the gstk_cmd */
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = 
    GSTK_TERMINAL_RSP;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.command_id    = 
    (uint32) GSTK_PROVIDE_LOCAL_INFO_CNF;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length = 
    length;
  if( length > 0 && nmr_info_ptr!=NULL ) 
  {
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length = 
       GSTK_MIN(length, 0xFF);
    task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr = 
      gstk_malloc((sizeof(uint8)) * 
      task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length);
      if(!task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr)
      {
        ERR("GSTK_MEMORY_ERROR", 0, 0, 0);
        /* free the info ptr if not null */
        if (task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) 
        {
          gstk_free(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr);
          task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
        }
        /* put the queue back to the free queue */
        (void)q_link(task_cmd_ptr, &task_cmd_ptr->cmd_hdr.link);
        q_put(task_cmd_ptr->cmd_hdr.done_q_ptr, &(task_cmd_ptr->cmd_hdr.link));
        return; 
      }
      memset(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr,
             0x00,
             task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length);
      memcpy(task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr,
             nmr_info_ptr,
             task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.length);
  }
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type     = 
    GSTK_NMR_UTRAN_INFO;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.message_header.user_data     = 0;
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id           =
    *((uint32*)data_ptr);
  task_cmd_ptr->cmd.provide_local_info_term_rsp_cnf.command_number               =
    command_details_ref_table[*((uint32*)data_ptr)].command_details.command_number;

  /* put command back to command queue */
  gstk_task_put_cmd_buf(task_cmd_ptr);

}/* gstk_queue_rrc_utran_nmr_rsp */
#endif /* FEATURE_GSTK_NMR_UTRAN_SUPPORT */

/*===========================================================================
FUNCTION   gstk_send_user_cnf_alpha_rsp

DESCRIPTION 
  Library function called by client to vote for YES or NO as a response for the
  user confirmation alpha sent by ESTK for Set up Call command. 

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback 
               reporting the set up call (display func). The client needs 
               to send back this reference id when sending the user response
               as to whether the call should be set up or not.
  yes_no_rsp : yes / no response for setting up the call

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_setup_call_user_cnf_alpha_rsp(
  gstk_client_id_type  client_id,
  uint32               gstk_user_data,
  boolean              yes_no_rsp
)
{
#ifdef FEATURE_ESTK
#error code not present
#else /* FEATURE_ESTK */
  (void)client_id;
  (void)gstk_user_data;
  (void)yes_no_rsp;
  MSG_ERROR("FEATURE_ESTK not defined",0,0,0);
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_ESTK */
}/* gstk_send_user_cnf_alpha_rsp */

/*===========================================================================
FUNCTION   gstk_uim_fetch_allow

DESCRIPTION 
  Library function called by UIM to check if UIM should do fetch or not
  depending on the following two condtions:
  1. whether the next command in gstk queue BIP Envelope or not
  2. whether there is a BIP Envelope that has been sent to UIM from gstk 
     for which gstk has not received a response yet.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_uim_fetch_allow(
)
{
  /* Entering gstk critical section */
  boolean ret_flag = FALSE;
  rex_enter_crit_sect(&gstk_crit_sect);
  if(!gstk_is_first_cmd_bip_env())
  {
    if(gstk_bip_env_in_progress)
    {
      MSG_HIGH("Not allowing Fetch as BIP ENV is in progress",0,0,0);
      gstk_uim_fetch_pending = TRUE;
    }
    else
    {
      MSG_HIGH("Allowing Fetch",0,0,0);
      gstk_uim_fetch_pending = FALSE;
      gstk_proactive_cmd_in_progress = TRUE;
      ret_flag = TRUE;
    }
  }
  else
  {
    MSG_HIGH("Not allowing Fetch as First command in gstk queue is BIP ENV",0,0,0);
    gstk_uim_fetch_pending = TRUE;
  }
  /* Leaving gstk critical section */
  rex_leave_crit_sect(&gstk_crit_sect);
  return ret_flag;
} /* gstk_uim_fetch_allow */

/* ============================================================================
  FUNCTION:      GSTK_CLIENT_ID_FORCE_CLEANUP

  DESCRIPTION:
    This function performs cleanup for client id when RPC has indicated that
    the application processor client(s) has/have been terminated.

  PARAMETERS:
    response_cb: Input  Callback that maps to the client id which enables
                        GSTK to perform corresponding client id 
                        deregistration on the client's behalf.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    gstk_status_enum_type:
      GSTK_SUCCESS             - if processing of client_init is successful
      GSTK_INVALID_CLIENT_TYPE - if client type is invalid
      GSTK_BAD_PARAM           - if client callback is NULL
      GSTK_MEMORY_ERROR        - if GSTK queue is FULL
      GSTK_UNSUPPORTED_COMMAND - if enhanced multiple client support is 
                                 not turned on.


  SIDE EFFECTS:
    Will result in the client id(s) that were linked to the corresponding
    response_cb during the gstk_client_init_ext() to be deregistered.
========================================================================== */
void gstk_client_id_force_cleanup(
  gstk_client_init_cb_func_type response_cb_ptr
)
{
  if (!response_cb_ptr)
  {
    MSG_ERROR("response_cb_ptr:NULL!", 0, 0, 0);
    return;
  }
  
  if (gstk_client_init_ext(GSTK_NO_TYPE, response_cb_ptr, 0) != GSTK_SUCCESS)
  {
    MSG_ERROR("Failed to dereg client id of cb 0x%x",
              response_cb_ptr, 0, 0);
  }
} /* gstk_client_id_force_cleanup */
