#ifndef GSTK_EXP_H
#define GSTK_EXP_H


/*===========================================================================


                     G S T K _ E X P    H E A D E R

GENERAL DESCRIPTION
  This is the GSTK export header file for GSTK client.  It contains
  command prototype and function protocol that client can use to communicate
  with GSTK.


FUNCTIONS
  gstk_client_init
    GSTK client calls this function to initialize with GSTK module.

  gstk_client_reg
    Event registration and de-registration

  gstk_client_toolkit_cmd_reg
    Event registration and de-registration for raw format

  gstk_send_terminal_response
    Send terminal response library function

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

  gstk_slot_send_envelope_cc_command
    Send call control envelope command library function
    
  gstk_ascii_to_gstk_address_type
    This function converts ASCII digits to gstk_address_type
    
  gstk_address_to_ascii
    This function converts gstk_address_type to ASCII digits

  gstk_send_terminal_response_ext
    Send terminal response library function with additional info pointer type

  gstk_send_raw_terminal_response
    Send raw terminal response library function

  gstk_send_envelope_cell_broadcast_dl_command
    Send cell broadcast download envelope command library function

  gstk_send_raw_envelope_command
    Send envelope command in raw format
  
  gstk_util_decode_dcs_type
    This function determines if the dcs is 7 bit or 8 bit or ucs2 depending
    on whether it is a cell broadcast dcs or not

  gstk_util_unpack_7_bit_chars_ext
    This function unpacks the input data from SMS7 bit format into ASCII
    and returns the actual data length without the null terminated byte.

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


$Header: //source/qcom/qct/modem/uim/gstk/rel/07H1_2/inc/gstk_exp.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/09   xz      Support client id clean up of modem restart
04/07/09   gg      Added define to set bit 3 of 17th byte in Treminal Profile
03/18/09   xz      Fix RPC compilation error
03/11/09   xz      Fix meta comments
03/03/09   gg      moved GSTK_REG_PROVIDE_BATTERY_STATE_EVT from
                   GSTK_REG_ALL_EVT to GSTK_REG_PROACTIVE_CMD_EVT
03/02/09   xz      Provide SW1 and SW2 support for raw APDU responses
02/26/09   gg      Introduced Class B event registration
01/07/09   tml     Add apitrim to force the gstk_app_enum_type to be included
                   in the autogeneration process
11/26/08   nb      Removed Featurization Flags for Common Modem Interface.
10/13/08   yb      Removed Lint Errors
10/10/08   yb      Fixed documentation
09/14/08   sk      Added fix for lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/06/08   sk      Fixed meta comments for RPC
08/21/08   sk      Added support for enhanced GSTK registration that includes 
                   support for multiple clients to perform terminal profile 
                   download and support for clients to register for a subset of 
                   proactive command in GSTK or RAW format. 
08/08/08   xz      Added support of raw proactive command, raw terminal response
                   and raw envelope command
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
11/07/07   sk      Added gstk enum for card type
09/25/07   sk      Changed tp dl bytes to allow alpha and display of extn text
                   support indication to come from the app
08/29/07   sk      Added additional info enums
03/01/07   sk      Fixed RPC metacomments
01/11/07   tml     Added rec_num for icon ID
11/22/06   sk      Updated RPC Metacomments
10/08/06   sk      Added support for unknown unsupported proactive commands.
09/22/06   jk      Fix RPC Metacomments bug
09/20/06   jk      Update RPC Metacomments
08/18/06   sk      Made gstk_nw_search_mode_enum_type spec compliant
07/27/06   jk      Updated Metacomments for 7200
06/27/06   jk      Added RPC support for function gstk_client_reg_ext
06/06/06   sk      Added support for network search mod
06/02/06   tml     GPRS Call Control support
06/01/06   sk      Fixed warnings
06/01/06   sk      Added support for channel status and data available
                   registrations.
05/08/06   jk      RPC Metadata comments for dual-proc
05/03/06   tml     Lint
04/06/06   sk      Updated Terminal Profile values.
04/04/06   sk      Added support for immediate digit response and variable 
                   timeout object for get inkey cmd.
03/06/06   sk      Added support for duration object in display text cmd.
                   Added support for battery status.
09/01/05   sun     Fixed Enum Values
07/13/05   sk      Added Access Technology support
05/11/05   sst     Lint fixes
04/06/05   tml     Exposed utility functions for alpha GSM default unicode
                   conversion
01/10/05   sk      Added original dcs from SIM in ussd string type and add
                   number of channel in channel status response
12/06/04   sk      Added macro for GSTK_MAX_GPRS_CHANNELS
                   Changed gstk_get_ch_status_rsp_extra_param_type
                   struct type to support maximum GPRS channels.
12/04/04   tml     move dcs enum decode and unpack function to exp.h
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Added 8 bit to 7 bit decoding function
08/19/04   tml     Added cell broadcast support
07/22/04   tml     Added gstk_additional_info_ptr_type such that
                   greater than GSTK_MAX_ADDITIONAL_INFO can be passed down 
                   to the card
06/14/04   tml     Added gstk_end_proactive_req_type
06/09/04   tml     Added Terminal profile version support, added 
                   packed/unpacked enum for get input, added color coding 
                   scheme for icon
05/11/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection 
                   evt dl supports
04/16/04   tml     linted
03/01/04   tml     Merged from tip: added provide local lang info support
09/12/03   tml     Added ascii to gstk_address_type and vice versa conversaton
                   Changed mode_type to mode_enum_type and 
                   slot_id_type_to slot_id_enum_type 
09/12/03   tml     Increased the GSTK_MAX_ADDRESS_LEN
09/04/03   tml     Dual slot support
09/03/03   tml     Added Slot ID
05/16/03   tml     Linted
04/21/03   tml     Added Sim busy for envelope response enum type.  Added
                   supports to handle UTK
03/03/03   tml     Changed byte to uint8, update enum names
02/26/03   tml     Added user data in the hdr command type
                   Changed library function to take function pointer instead of
                   pointer to function
02/25/03   tml     changed ERROR to ERROR_TONE to avoid conflict with atcop
02/12/03   tml     Added gstk_map_proactive_req_to_cnf
                   Removed gstk_find_expected_proactive_cmd_rsp
02/07/03 tml/jar   Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
** Capabilities
** ------------
** (other internal capabilities also defined in gstk.h)
*/

#define GSTK_MAX_ADDRESS_LEN                   200
#define GSTK_MAX_SUBADDR_LEN                   20 /* CALLED_PARTY_SUBADDRESS_LENGTH */
                                                  /* CM maximum call party subaddress is 20 */
#define GSTK_MAX_CCP_LEN                       14 /* GSDI_CCP_LEN */
#define GSTK_MAX_PROVISIONING_FILE_REF         20  /*??? */
#define GSTK_MAX_ADDITIONAL_INFO               10

/* Duration units (For redial timeout & tones. See also 11.14) */
#define GSTK_DURATION_MINUTES                                  0x00
#define GSTK_DURATION_SECONDS                                  0x01
#define GSTK_DURATION_TENTHS_OF_SECONDS                        0x02

#define GSTK_LANG_CODE_LEN                     2

/* Terminal profile size */
#ifndef FEATURE_CAT_REL6
#define GSTK_TERMINAL_PROFILE_SIZE 20 /* bytes */
#else
#define GSTK_TERMINAL_PROFILE_SIZE 29 /* bytes */
#endif /* FEATURE_CAT_REL6 */

#define GSTK_MAX_GPRS_CHANNELS    0x07


#define GSTK_MAX_PDP_ACT_PARAM_SIZE 255

/* This Maximum is used for the output length of the gstk decode functions,
and will need to be expanded as larger capacity is needed */
#define GSTK_EXP_RPC_MAX_OUTPUT_DATA 500

/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/
/* Client reference data */
typedef uint32 gstk_client_ref_data_type;

/*--------------------------------------------------------------------------
                             Enum Definitions
---------------------------------------------------------------------------*/
typedef enum
{
   GSTK_BATTERY_ST_VERY_LOW = 0x00, /* Battery status - very low */
   GSTK_BATTERY_ST_LOW      = 0x01, /* Battery status - low */
   GSTK_BATTERY_ST_AVG      = 0x02, /* Battery status - average */
   GSTK_BATTERY_ST_GOOD     = 0x03, /* Battery status - good */
   GSTK_BATTERY_ST_FULL     = 0x04, /* Battery status - full */
   GSTK_BATTERY_ST_NA       = 0xFE, /* Battery status - status not applicable */
   GSTK_BATTERY_ST_UNKNOWN  = 0xFF  /* Battery status - not known Eg.battery charging */
}
gstk_battery_state_enum_type;

/* According to spec 102.223 */
typedef enum
{
  GSTK_NW_SEARCH_MODE_MANUAL    = 0x00,
  GSTK_NW_SEARCH_MODE_AUTOMATIC = 0x01,
  GSTK_NW_SEARCH_MODE_MAX       = 0xFF
}
gstk_nw_search_mode_enum_type;

/*===========================================================================
--  Enum: gstk_mode_enum_type 
     GSTK_GSM_MODE -> current system is GSM mode
     GSTK_CDMA_MODE -> current system is CDMA mode
===========================================================================*/
typedef enum {
  GSTK_GSM_MODE,
  GSTK_CDMA_MODE,
  GSTK_MODE_MAX       = 0xFFFF
} 
gstk_mode_enum_type;

/* ----------------------------- Client Registration information --------------------------- */

/*===========================================================================
--  Enum: gstk_client_type
     GSTK_DS_TYPE     -> Data related client
     GSTK_UI_TYPE     -> User interface related client
     GSTK_ESTK_TYPE   -> Enhanced toolkit related client
     GSTK_TEST_TYPE   -> Test interface related client
     GSTK_NO_TYPE     -> default value during power up
===========================================================================*/
typedef enum {
    GSTK_DS_TYPE,
    GSTK_UI_TYPE,
    GSTK_TEST_TYPE,
    GSTK_ESTK_TYPE,
    GSTK_NO_TYPE,
    GSTK_TYPE_MAX      = 0x7FFFFFFF
}
gstk_client_type;

/*===========================================================================
--  Enum: gstk_client_reg_cb_func_type
     GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT  -> Callback supporting GSTK format
	                                      only
     GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT -> Callback supporting multiple 
	                                      formats (RAW/GSTK)
===========================================================================*/
typedef enum {
    GSTK_CLI_CB_NONE                       = 0x00,
    GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT,
    GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT,
    GSTK_CLI_CB_MAX                        = 0x7FFFFFFF
}
gstk_client_reg_cb_func_format_enum_type;

/*===========================================================================
--  Enum: gstk_client_pro_cmd_data_format_enum_type
     GSTK_FORMAT_NONE    ->  default format value is NONE when not set.
                             Client **should** specify either GSTK_RAW_FORMAT
                             or GSTK_GSTK_FORMAT in its request. 
     GSTK_RAW_FORMAT     ->  proactive command in Raw Format
     GSTK_GSTK_FORMAT    ->  proactive command in GSTK format 
     GSTK_FORMAT_MAX     ->  Max value for format type
===========================================================================*/
typedef enum 
{
  GSTK_FORMAT_NONE  = 0x00,
  GSTK_RAW_FORMAT,                /* proactive cmd in raw bytes format */  
  GSTK_GSTK_FORMAT,               /* proactive cmd in gstk format */  
  GSTK_FORMAT_MAX = 0x7FFFFFFF 
} gstk_client_pro_cmd_data_format_enum_type;

/*===========================================================================
--  Enum: gstk_tk_functionality_enum_type
     GSTK_HANDLE_FUNC_NONE             ->  Default value for functionality.
                                           Client **should" specify only valid 
                                           values such as Display/UI or All.
     GSTK_HANDLE_DISPLAY_FUNC_ONLY     ->  Display Functionality only
     GSTK_HANDLE_MODEM_FUNC_ONLY       ->  Modem Functionality only 
     GSTK_HANDLE_ALL_FUNC              ->  All functionality
     GSTK_HANDLE_FUNC_MAX              ->  Max value for functionality
===========================================================================*/
typedef enum 
{
  GSTK_HANDLE_FUNC_NONE           = 0x00,
  /* handle display functionality only */
  GSTK_HANDLE_DISPLAY_FUNC_ONLY,
  /* handle modem functionality only */           
  GSTK_HANDLE_MODEM_FUNC_ONLY,
  /* handle complete functionality */
  GSTK_HANDLE_ALL_FUNC,
  /* Place holder */
  GSTK_HANDLE_FUNC_MAX            = 0x7FFFFFFF
} gstk_tk_functionality_enum_type;

/* Registration class bit mask that the client should use
** in order to specify the set/subset of proactive commands 
** belonging to that class that the client is registering 
** for using API gstk_client_toolkit_cmd_reg().
*/
#define GSTK_REG_UNSPECIFIED_CATEGORY_CMD       0xFFFFFFFF
#define GSTK_REG_CATEGORY_0_CMD                 0x00000000
#define GSTK_REG_CATEGORY_C_CMD                 0x00000001
#define GSTK_REG_CATEGORY_D_CMD                 0x00000002
#define GSTK_REG_CATEGORY_E_CMD                 0x00000003
#define GSTK_REG_CATEGORY_B_CMD                 0x00000004

/* Registration bitmask is used when client registers for gstk events
** This allows them to specify which information they want to receive
** event from gstk
*/

/* Register for individual Proactive Command event
** Commands have been listed in the class alphabetical order of
** Category 0 - denotes that the commands belong to a general category
             of toolkit functionality
** Category C - denotes browser related proactive and envelope commands.
** Category E - denotes BIP (GPRS) related proactive and envelope commands.
*/
/* NOTE: The order in which the class elements have been listed is
** 1. Category 0 2. Category "C" 3. Category "E"
** Any additions to the elements in the existing Category below should be 
** done after the last member of the corresponding Category.
** Category have also been listed in alphabetical order and any new
** addition of Category should follow the same rule
*/

/* Class "0" */
#define GSTK_REG_DISPLAY_TEXT_EVT           0x00000001
#define GSTK_REG_GET_INKEY_EVT              0x00000002
#define GSTK_REG_GET_INPUT_EVT              0x00000004
#define GSTK_REG_MORE_TIME_EVT              0x00000008    

#define GSTK_REG_PLAY_TONE_EVT              0x00000010
#define GSTK_REG_POLL_INTERVAL_EVT          0x00000020
#define GSTK_REG_PROVIDE_LOCAL_INFO_EVT     0x00000040
#define GSTK_REG_REFRESH_EVT                0x00000080   

#define GSTK_REG_SELECT_ITEM_EVT            0x00000100
#define GSTK_REG_SEND_SMS_EVT               0x00000200
#define GSTK_REG_SEND_SS_EVT                0x00000400
#define GSTK_REG_SEND_USSD_EVT              0x00000800   

#define GSTK_REG_SETUP_CALL_EVT             0x00001000
#define GSTK_REG_SETUP_EVENT_LIST_EVT       0x00002000
#define GSTK_REG_SETUP_MENU_EVT             0x00004000
#define GSTK_REG_SETUP_IDLE_TEXT_EVT        0x00008000    

#define GSTK_REG_POLLING_OFF_EVT            0x00010000
#define GSTK_REG_PROVIDE_LANG_INFO_EVT      0x00020000
#define GSTK_REG_LANG_NOTIFY_EVT            0x00040000
#define GSTK_REG_TIMER_MANAGEMENT_EVT       0x00080000    

#define GSTK_REG_SEND_DTMF_EVT              0x00100000
#define GSTK_REG_SETUP_EVT_USER_ACT_EVT     0x00200000
#define GSTK_REG_SETUP_EVT_IDLE_MENU_EVT    0x00400000
#define GSTK_REG_SETUP_EVT_LANG_SEL_EVT     0x00800000    

/* register for profile download rsp */
#define GSTK_REG_PROFILE_DL_RSP_EVT         0x01000000
/* register for Provide Battery Status Req */
#define GSTK_REG_PROVIDE_BATTERY_STATE_EVT  0x02000000
/* register for envelope response events */
#define GSTK_REG_EVT_DOWNLOAD_RSP_EVT       0x04000000
#define GSTK_REG_MENU_SEL_RSP_EVT           0x08000000  
                                                           
/* Class "C" commands */
#define GSTK_REG_LAUNCH_BROWSER_EVT         0x10000000
#define GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT 0x20000000

/* Class "E" commands */
#define GSTK_REG_DATA_EVT                       0x40000000
#define GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT   0x80000000  
/* register for Channel status event */
#define GSTK_REG_SETUP_EVT_CH_STATUS_EVT    0x0000000100000000LL

/* Class B command */
#define GSTK_REG_RUN_AT_CMDS_EVT            0x0000000200000000LL

/* register for all proactive command events */
#define GSTK_REG_PROACTIVE_CMD_EVT             \
  (GSTK_REG_DISPLAY_TEXT_EVT             |     \
  GSTK_REG_GET_INKEY_EVT                 |     \
  GSTK_REG_GET_INPUT_EVT                 |     \
  GSTK_REG_MORE_TIME_EVT                 |     \
  GSTK_REG_PLAY_TONE_EVT                 |     \
  GSTK_REG_POLL_INTERVAL_EVT             |     \
  GSTK_REG_PROVIDE_LOCAL_INFO_EVT        |     \
  GSTK_REG_REFRESH_EVT                   |     \
  GSTK_REG_SELECT_ITEM_EVT               |     \
  GSTK_REG_SEND_SMS_EVT                  |     \
  GSTK_REG_SEND_SS_EVT                   |     \
  GSTK_REG_SEND_USSD_EVT                 |     \
  GSTK_REG_SETUP_CALL_EVT                |     \
  GSTK_REG_SETUP_EVENT_LIST_EVT          |     \
  GSTK_REG_SETUP_MENU_EVT                |     \
  GSTK_REG_SETUP_IDLE_TEXT_EVT           |     \
  GSTK_REG_POLLING_OFF_EVT               |     \
  GSTK_REG_PROVIDE_LANG_INFO_EVT         |     \
  GSTK_REG_LANG_NOTIFY_EVT               |     \
  GSTK_REG_TIMER_MANAGEMENT_EVT          |     \
  GSTK_REG_SEND_DTMF_EVT                 |     \
  GSTK_REG_SETUP_EVT_USER_ACT_EVT        |     \
  GSTK_REG_SETUP_EVT_IDLE_MENU_EVT       |     \
  GSTK_REG_SETUP_EVT_LANG_SEL_EVT        |     \
  GSTK_REG_PROVIDE_BATTERY_STATE_EVT     |     \
  GSTK_REG_LAUNCH_BROWSER_EVT            |     \
  GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT    |     \
  GSTK_REG_DATA_EVT                      |     \
  GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT  |     \
  GSTK_REG_SETUP_EVT_CH_STATUS_EVT       |     \
  GSTK_REG_RUN_AT_CMDS_EVT)

/* register for all events including proactive command events*/
#define GSTK_REG_ALL_EVT                       \
  (GSTK_REG_PROACTIVE_CMD_EVT            |     \
  GSTK_REG_PROFILE_DL_RSP_EVT            |     \
  GSTK_REG_EVT_DOWNLOAD_RSP_EVT          |     \
  GSTK_REG_MENU_SEL_RSP_EVT)  


/* ----------------------------- Command Type information --------------------------- */

/* STK command values   */
/* Type of Command Section 8.6 and 9.4 3GPP 31.111 v 3.3 */
/* This is used in the command details to indicate what
** proactive command is being issued from SIM/USIM to
** the mobile.
** Certain values are also being used in the
** gstk_next_item_ind_type based on standard specification
*/
#define GSTK_CMD_STK_REFRESH                0x01
#define GSTK_CMD_STK_MORE_TIME              0x02
#define GSTK_CMD_STK_POLL_INTERVAL          0x03
#define GSTK_CMD_STK_POLLING_OFF            0x04
#define GSTK_CMD_STK_SET_UP_EVENT_LIST      0x05
#define GSTK_CMD_STK_SET_UP_CALL            0x10
#define GSTK_CMD_STK_SEND_SS                0x11
#define GSTK_CMD_STK_USSD                   0x12
#define GSTK_CMD_STK_SEND_SHORT_MESSAGE     0x13
#define GSTK_CMD_STK_SEND_DTMF              0x14
#define GSTK_CMD_STK_LAUNCH_BROWSER         0x15
#define GSTK_CMD_STK_PLAY_TONE              0x20
#define GSTK_CMD_STK_DISPLAY_TEXT           0x21
#define GSTK_CMD_STK_GET_INKEY              0x22
#define GSTK_CMD_STK_GET_INPUT              0x23
#define GSTK_CMD_STK_SELECT_ITEM            0x24
#define GSTK_CMD_STK_SET_UP_MENU            0x25
#define GSTK_CMD_STK_PROVIDE_LOCAL_INFO     0x26
#define GSTK_CMD_STK_TIMER_MANAGEMENT       0x27
#define GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT  0x28
#define GSTK_CMD_STK_LANG_NOTIFICATION      0x35
#define GSTK_CMD_STK_END_OF_PROACTIVE_SES   0x81
#define GSTK_CMD_STK_OPEN_CHANNEL           0x40
#define GSTK_CMD_STK_CLOSE_CHANNEL          0x41
#define GSTK_CMD_STK_RECEIVE_DATA           0x42
#define GSTK_CMD_STK_SEND_DATA              0x43
#define GSTK_CMD_STK_GET_CHANNEL_STATUS     0x44

/* not supported for PHASE 1 development */
#define GSTK_CMD_STK_PERFORM_CARD_APDU      0x30
#define GSTK_CMD_STK_POWER_ON_CARD          0x31
#define GSTK_CMD_STK_POWER_OFF_CARD         0x32
#define GSTK_CMD_STK_GET_READER_STATUS      0x33
#define GSTK_CMD_STK_RUN_AT_COMMAND         0x34


/* ----------------------------- Profile Download releated information --------------------------- */

/* Profile Download bitmask

** divided in 3 groups, the first group includes information that the
** upper layer is "required" to update the information, the second
** group includes information that are required to be retrieved from
** NV.  The second group allows ease of switching the configuration with the
** built hardware/software. The third group is the set of terminal profile bytes
** that *should* be updated ONLY by gstk after the card type has been determined.

** The following defines is to be used in the gstk_profile_dl_support_type
** so that the client can indicate which profile download information
** it is going to modify

** the left most significant byte specifies offset shifting.  Used internally
** when GSTK build the terminal profile.  The next byte specifies the byte
** of this information in the profile download string, the right most
** byte specifies the location of this information within that byte
*/

/* ---------------- first group -------------------- */
/*first byte */
#define GSTK_SMS_PP_DL_SUPPORT                        0x0102 /* value = (1,0) */
#define GSTK_MENU_SEL_SUPPORT                         0x0108 /* value = (1,0) */
/* For SAT */
#define GSTK_USSD_DO_IN_CC_SUPPORT                    0x0140 /* value = (1,0) */

/* Second byte */
#define GSTK_CC_SUPPORT                               0x0202 /* value = (1,0) */
#define GSTK_MO_SMS_CONTROL_SUPPORT                   0x0208 /* value = (1,0) */
#define GSTK_SUPPORT_FOR_ALPHA_IN_CC                  0x0210  /* value = (1,0) */
#define GSTK_UCS2_ENTRY_SUPPORT                       0x0220 /* value = (1,0) */
#define GSTK_UCS2_DISPLAY_SUPPORT                     0x0240 /* value = (1,0) */
/* For USAT */
#define GSTK_USAT_DISPLAY_SUPPORT_BYTE_2_BIT_8        0x0280 /* value = (1,0) */
/* For SAT */
#define GSTK_DISPLAY_OF_EXTN_TEXT                     0x0280 /* value = (1,0) */

/* 3rd byte */
#define GSTK_DISPLAY_TEXT_SUPPORT                     0x0301 /* value = (1,0) */
#define GSTK_GET_INKEY_SUPPORT                        0x0302 /* value = (1,0) */
#define GSTK_GET_INPUT_SUPPORT                        0x0304 /* value = (1,0) */
#define GSTK_PLAY_TONE_SUPPORT                        0x0310 /* value = (1,0) */

/* 4th byte */
#define GSTK_SELECT_ITEM_SUPPORT                      0x0401 /* value = (1,0) */
#define GSTK_SEND_SHORT_MSG_SUPPORT                   0x0402 /* value = (1,0) */
#define GSTK_SEND_SS_SUPPORT                          0x0404 /* value = (1,0) */
#define GSTK_SEND_USSD_SUPPORT                        0x0408 /* value = (1,0) */
#define GSTK_SET_UP_CALL_SUPPORT                      0x0410 /* value = (1,0) */
#define GSTK_SET_UP_MENU_SUPPORT                      0x0420 /* value = (1,0) */

/* 5th byte */
#define GSTK_USER_ACT_EVT_SUPPORT                     0x0520 /* value = (1,0) */
#define GSTK_IDLE_SCRN_AVAIL_EVT_SUPPORT              0x0540 /* value = (1,0) */

/* 6th byte */
#define GSTK_LANG_SEL_EVT_SUPPORT                     0x0601 /* value = (1,0) */
#define GSTK_BROWSER_TERM_EVT_SUPPORT                 0x0602 /* value = (1,0) */

#define GSTK_DATA_AVAIL_EVT_SUPPORT                   0x0604 /* value = (1,0) */  /* default = 0 */
#define GSTK_CH_STATUS_EVT_SUPPORT                    0x0608 /* value = (1,0) */  /* default = 0 */


/* 8th byte */
#define GSTK_SET_UP_IDLE_MODE_TEXT_SUPPORT            0x0810 /* value = (1,0) */
#define GSTK_USAT_SETUP_CALL_SUPPORT_BYTE_8_BIT_7     0x0840 /* value = (1,0) */
#define GSTK_SECOND_ALPHA_IN_SETUP_CALL_SUPPORT       0x0840 /* value = (1,0) */
#define GSTK_SECOND_CAP_CONFIG_PARAM_SUPPORT          0x0880 /* value = (1,0) */

/* 9th byte */
#define GSTK_USAT_DISP_TEXT_SUPPORT_BYTE_9_BIT_1      0x0901 /* value = (1,0) */ 
#define GSTK_SUSTAINED_DISP_TEXT_SUPPORT              0x0901 /* value = (1,0) */ 
#define GSTK_SEND_DTMF_SUPPORT                        0x0902 /* value = (1,0) */ 
#define GSTK_PLI_NMR_BCCH                             0x0904 /* value = (1,0) */ 
#define GSTK_PROVIDE_LOCAL_INFO_LANG_SUPPORT          0x0908 /* value = (1,0) */ 
#define GSTK_LANGUAGE_NOTIFICATION_SUPPORT            0x0920 /* value = (1,0) */
#define GSTK_LAUNCH_BROWSER_SUPPORT                   0x0940 /* value = (1,0) */

/* 10th byte */
#define GSTK_SK_SUPPORT_IN_SEL_ITEM                   0x0A01 /* value = (1,0) */
#define GSTK_SK_SUPPORT_IN_SET_UP_MENU                0x0A02 /* value = (1,0) */

/* 11th byte */
#define GSTK_MAX_SK_SUPPORT                           0x0BFF /* value = 1 byte of info */

/* 12th byte */
#define GSTK_OPEN_CH_SUPPORT                          0x0C01 /* value = (1,0) */  
#define GSTK_CLOSE_CH_SUPPORT                         0x0C02 /* value = (1,0) */  
#define GSTK_RECEIVE_DATA_SUPPORT                     0x0C04 /* value = (1,0) */  
#define GSTK_SEND_DATA_SUPPORT                        0x0C08 /* value = (1,0) */  
#define GSTK_GET_CH_STATUS_SUPPORT                    0x0C10 /* value = (1,0) */  


/* 13th byte */
#define GSTK_CSD_SUPPORT                              0x0D01 /* value = (1,0) */
#define GSTK_GPRS_SUPPORT                             0x0D02 /* value = (1,0) */

/* 14th byte */
#define GSTK_NUM_CHAR_SUPPORT_DOWN_ME                 0x0E1F /* 5 bits of info
                                                             ** num of char support without scrolling */
#define GSTK_SCREEN_SIZE_SUPPORT                      0x0E80 /* value = (1,0)
                                                             ** if user can change the num of char across/down the display
                                                             ** or if ME can dynamically change the num of char supported
                                                             ** on the display, then value = 1*/

/* 15th byte */
#define GSTK_NUM_CHAR_SUPPORT_ACROSS_ME               0x0F7F /* 7 bits of info
                                                       ** num of char support without scrolling */
#define GSTK_VARIABLE_FONT_SIZE_SUPPORT               0x0F80 /* value = (1,0) */

/* 16th byte */
#define GSTK_DISPLAY_RESIZED_SUPPORT                  0x1001 /* value = (1,0) */
#define GSTK_TEXT_WRAPPING_SUPPORT                    0x1002 /* value = (1,0) */
#define GSTK_TEXT_SCROLLING_SUPPORT                   0x1004 /* value = (1,0) */
#define GSTK_WIDTH_REDUCE_IN_MENU_SUPPORT             0x10E0 /* value = 3 bits of info
                                                             ** num of char across the display due to DISPLAY TEXT
                                                             ** without scrolling minus num of char across display
                                                             ** due to SELECT ITEM without scrolling */

/* 17th byte */
#define GSTK_TCP_SUPPORT                              0x1101 /* value = (1,0) */
#define GSTK_UDP_SUPPORT                              0x1102 /* value = (1,0) */
#define GSTK_TCP_SERVER_MODE_SUPPORT                  0x1104 /* value = (1,0) */


/* 18th byte */
#define GSTK_DISPLAY_TXT_VARIABLE_TIMEOUT_SUPPORT     0x1201
#define GSTK_INKEY_HELP_FOR_VARIABLE_TIMEOUT_AND_IMMEDIATE_DIGIT_RSP     0x1202
#define GSTK_INKEY_VARIABLE_TIMEOUT_SUPPORT           0x1208


/* 19th byte */
#define GSTK_PROTOCOL_VERSION                         0x130F /* 4 bits of protocol version info */

/* 20th byte */
//#ifdef FEATURE_UIM_RUIM
#define GSTK_CDMA_SEND_SMS_SUPPORT                    0x1401
#define GSTK_CDMA_SMS_PP_SUPPORT                      0x1402

/* 22nd byte */
#define GSTK_PROVIDE_LOCAL_INFO_BATTERY_STATE_SUPPORT 0x1610

//#endif /* FEATURE_UIM_RUIM */


/* ------------------ second group -------------------------- */
/* The followings have default API support values already, should client want
** to change their information, they can only change those that have the default
** value set to 1
** If client set the value for those #defines that have a default value == 0,
** GSTK will ignore the client's setting, and send the default value (=0)
** for those #defines bitmask to the SIM/USIM,
*/
/* 1st byte */
#define GSTK_PROFILE_DL_SUPPORT                       0x0101 /* value = (1,0) */  /* default = 1 */
#define GSTK_CELL_BROADCAST_DATA_DL_SUPPORT           0x0104 /* value = (1,0) */  /* default = 0 */
#define GSTK_TIMER_EXPIRATION_SUPPORT                 0x0120 /* value = (1,0) */  /* default = 1 */

/* 3rd byte */
#define GSTK_MORE_TIME_SUPPORT                        0x0308 /* value = (1,0) */  /* default = 1 */
#define GSTK_POLL_INTERVAL_SUPPORT                    0x0320 /* value = (1,0) */  /* default = 1 */
#define GSTK_POLLING_OFF_SUPPORT                      0x0340 /* value = (1,0) */  /* default = 1 */
#define GSTK_REFRESH_SUPPORT                          0x0380 /* value = (1,0) */  /* default = 1 */

/* 4th byte */
#define GSTK_PROVIDE_LOCAL_INFO_LOCATION_INFO_SUPPORT 0x0440 /* value = (1,0) */  /* default = 1 */
#define GSTK_PROVIDE_LOCAL_INFO_NMR_SUPPORT           0x0480 /* value = (1,0) */  /* default = 1 */

/* 5th byte */
#define GSTK_SET_UP_EVT_LIST_SUPPORT                  0x0501 /* value = (1,0) */  /* default = 1 */
#define GSTK_MT_CALL_EVT_SUPPORT                      0x0502 /* value = (1,0) */  /* default = 1 */
#define GSTK_CALL_CONNECTED_EVT_SUPPORT               0x0504 /* value = (1,0) */  /* default = 1 */
#define GSTK_CALL_DISCONNECTED_EVT_SUPPORT            0x0508 /* value = (1,0) */  /* default = 1 */
#define GSTK_LOCATION_STATUS_EVT_SUPPORT              0x0510 /* value = (1,0) */  /* default = 1 */
#define GSTK_CARD_READER_ST_EVT_SUPPORT               0x0580 /* value = (1,0) */  /* default = 0 */

/* 7th byte */
#define GSTK_POWER_ON_CARD_SUPPORT                    0x0701 /* value = (1,0) */  /* default = 0 */
#define GSTK_POWER_OFF_CARD_SUPPORT                   0x0702 /* value = (1,0) */  /* default = 0 */
#define GSTK_PERFORM_CARD_APDU_SUPPORT                0x0704 /* value = (1,0) */  /* default = 0 */
#define GSTK_GET_READER_ST_SUPPORT                    0x0708 /* value = (1,0) */  /* default = 0 */
#define GSTK_GET_READER_ST_CARD_ID_SUPPORT            0x0710 /* value = (1,0) */  /* default = 0 */

/* 8th byte */
#define GSTK_TIMER_START_STOP_SUPPORT                 0x0801 /* value = (1,0) */  /* default = 1 */
#define GSTK_TIMER_GET_CURR_VALUE_SUPPORT             0x0802 /* value = (1,0) */  /* default = 1 */
#define GSTK_PROVIDE_LOCAL_INFO_TIME_SUPPORT          0x0804 /* value = (1,0) */  /* default = 1 */
#define GSTK_RUN_AT_CMD_SUPPORT                       0x0820 /* value = (1,0) */  /* default = 0 */

/* 9th byte */
#define GSTK_PROVIDE_LOCAL_INFO_TIME_ADV_SUPPORT      0x0910 /* value = (1,0) */  /* default = 1 */

/* 13th byte */
#define GSTK_NUM_CH_SUPPORT                           0x0DE0 /* 3 bits of info */ /* default = 0 */

/* 18th byte */
#define GSTK_PROVIDE_LOCAL_INFO_ESN_SUPPORT           0x1210
#define GSTK_PROVIDE_LOCAL_INFO_IMEISV_SUPPORT        0x1240
#define GSTK_PROVIDE_LOCAL_INFO_NETWORK_SEARCH_MODE_SUPPORT              0x1280

/* 23rd byte */
#define GSTK_PROVIDE_LOCAL_INFO_MEID_SUPPORT          0x1702

#define GSTK_MAX_PROFILE_DL_UPDATE_SUPPORT      76 /* maximum num of profile download info that
                                                      can be updated by other layers */

#define GSTK_MAX_MENU_ITEMS                     50

/* ------------------ third group -------------------------- */
/* The third group is the set of terminal profile bytes that 
** *should* be updated only by gstk after the card type has been determined.
*/

/* first byte */
/* For USAT */
#define GSTK_USAT_SMS_PP_DL_SUPPORT_BYTE1_BIT_5       0x0110 /* value = (1,0) */
/* for SAT */
#define GSTK_9EXX_RSP_CODE_SUPPORT                    0x0110 /* value = (1,0) */
/* For USAT */
#define GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_7             0x0140 /* value = (1,0) */
#define GSTK_CC_ALWAYS_SENT_DURING_REDIAL             0x0180 /* value = (1,0) */
#define GSTK_USAT_CC_SUPPORT_BYTE_1_BIT_8             0x0180 /* value = (1,0) */

/* second byte */
/* For USAT */
#define GSTK_USAT_CC_SUPPORT_BYTE_2_BIT_5             0x0210 /* value = (1,0) */

/* eighth byte */
#define GSTK_USAT_CC_SUPPORT_BYTE_8_BIT_8             0x0880 /* value = (1,0) */

/* ninth byte */
#define GSTK_PLI_NMR_UTRAN                            0x0904 /* value = (1,0) */



/* ----------------------------- GSTK_STATUS_ENUM_TYPE ------------------------------------- */

/*===========================================================================
--  Enum: gstk_status_enum_type
      gstk library functions and register call back function return type
      client can use this to check whether the function call into gstk
      is successful or not
===========================================================================*/
typedef enum {
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_DUPLICATE_REGISTRATION,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_NO_CLIENT_REG_FOUND, /* when no client has registered for a command */
    GSTK_INVALID_CLIENT_TYPE, /* Invalid client id type */
    GSTK_PROFILE_VALUE_NOT_ALLOWED, /* when client tried to set the value of a profile download bitmask
                                    ** which has a default value of 0 to 1 */
    GSTK_PROFILE_ALREADY_SENT,
    GSTK_MEMORY_ERROR,              /* use when no memory avaliable for further
                                    ** allocation */
    GSTK_CLIENT_SPACE_FULL,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_EXTRA_PARAM_MISSING,
    GSTK_EXTRA_PARAM_NOT_REQUIRED,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_INVALID_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_FILE_ACCESS_FAIL,  /* e.g., when accessing EF-IMG failed */
    GSTK_CONTRADICTING_TIMER_REQUEST,
    GSTK_NOT_INIT,
    GSTK_INVALID_DEVICE_COMMAND,
    GSTK_RPC_ERROR,
    GSTK_STATUS_MAX                    = 0x7FFFFFFF
}
gstk_status_enum_type;



/* ----------------------------- GSTK <-> upper layer command enum type information --------------------------- */

typedef uint32 gstk_client_id_type;


/*===========================================================================
--  Enum: gstk_cmd_enum_type
      Command Message ID:  There are 4 different message ID groups:
       1 for proactive command, 1 for terminal response and
       1 for envelope command, 1 for envelope response
===========================================================================*/
typedef enum {
    GSTK_CMD_ENUM_NOT_USE          = 0x00,
    /* ---------------- Requests from SIM/USIM to ME ---------------- */
    /*
    ** Message ID's for proactive command messages sent to upper layers
    ** These are used in the command header message id portion of the
    ** structure
    */

    /* Proactive Command request */
    GSTK_DISPLAY_TEXT_REQ               = 0x01,
    GSTK_GET_INKEY_REQ                  = 0x02,
    GSTK_GET_INPUT_REQ                  = 0x03,
    GSTK_LAUNCH_BROWSER_REQ             = 0x04,
    GSTK_MORE_TIME_REQ                  = 0x05, /* internal use */
    GSTK_PLAY_TONE_REQ                  = 0x06,
    GSTK_POLL_INTERVAL_REQ              = 0x07, /* internal use */
    GSTK_PROVIDE_LOCAL_INFO_REQ         = 0x08, /* internal use */
    GSTK_REFRESH_REQ                    = 0x09, /* internal use */
    GSTK_RUN_AT_CMDS_REQ                = 0x0A,
    GSTK_SELECT_ITEM_REQ                = 0x0B,
    GSTK_SEND_SMS_REQ                   = 0x0C,
    GSTK_SEND_SS_REQ                    = 0x0D,
    GSTK_SEND_USSD_REQ                  = 0x0E,
    GSTK_SETUP_CALL_REQ                 = 0x0F,
    GSTK_SETUP_EVENT_LIST_REQ           = 0x10, /* internal use */
    GSTK_SETUP_MENU_REQ                 = 0x11,
    GSTK_SETUP_IDLE_TEXT_REQ            = 0x12,
    GSTK_POLLING_OFF_REQ                = 0x13, /* internal use */
    GSTK_END_PROACTIVE_CMD_REQ          = 0x14,
    GSTK_TOOLKIT_MODE_CHANGE_REQ        = 0x15,
    GSTK_PROVIDE_LANG_INFO_REQ          = 0x16,
    GSTK_SEND_DTMF_REQ                  = 0x17,
    GSTK_LANG_NOTIFICATION_REQ          = 0x18,
    GSTK_TIMER_MANAGEMENT_REQ           = 0x19, /* internal use */
    GSTK_OPEN_CH_REQ                    = 0x1A,
    GSTK_CLOSE_CH_REQ                   = 0x1B,
    GSTK_SEND_DATA_REQ                  = 0x1C,
    GSTK_RECEIVE_DATA_REQ               = 0x1D,
    GSTK_GET_CH_STATUS_REQ              = 0x1E,
    GSTK_PROVIDE_BATTERY_STATUS_REQ     = 0x1F,
    
    /*
    ** Message ID's for envelope command response messages from SIM/USIM
    ** in response to an earlier envelope command sent by the upper layers
    ** These are used in the command header message id portion of the
    ** structure
    */

    /* Envelope Indication Responses */
    GSTK_PROFILE_DL_IND_RSP              = 0x81,
    GSTK_MENU_IND_RSP                    = 0x82,
    GSTK_SMS_PP_DOWNLOAD_IND_RSP         = 0x83,
    GSTK_MT_CALL_EVT_IND_RSP             = 0x84,
    GSTK_CALL_CONNECTED_EVT_IND_RSP      = 0x85,
    GSTK_CALL_DISCONNECTED_EVT_IND_RSP   = 0x86,
    GSTK_IDLE_SCRN_EVT_IND_RSP           = 0x87,
    GSTK_LOCATION_STATUS_EVT_IND_RSP     = 0x88,
    GSTK_USER_ACTIVITY_EVT_IND_RSP       = 0x89,
    GSTK_MO_SMS_CTRL_IND_RSP             = 0x8A,
    GSTK_CC_IND_RSP                      = 0x8B,
    GSTK_TIMER_EXPIRE_IND_RSP            = 0x8C,
    GSTK_LANG_SELECTION_EVT_IND_RSP      = 0x8D,
    GSTK_BROWSER_TERMINATION_EVT_IND_RSP = 0x8E,
    GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP = 0x8F,
    GSTK_DATA_AVAIL_EVT_IND_RSP          = 0x90,
    GSTK_CH_STATUS_EVT_IND_RSP           = 0x91,
    GSTK_ACCESS_TECH_EVT_IND_RSP         = 0x92,
    GSTK_NW_SEARCH_MODE_EVT_IND_RSP      = 0x93,
    GSTK_RAW_ENVELOPE_RSP                = 0x94,
    /* ---------------- Requests from ME to SIM/USIM ---------------- */

    /*
    ** Message ID's for terminal response of proactive
    ** command messages sent from upper layers
    ** Upper layers use this in the library function
    ** gstk_lib_send_terminal_response as the
    ** indication of what terminal response it is
    ** going to request gstk to pack and send to uim driver
    */

    /* Proactive Command Conformations (Terminal Response) */
    GSTK_DISPLAY_TEXT_CNF              = 0x41,
    GSTK_GET_INKEY_CNF                 = 0x42,
    GSTK_GET_INPUT_CNF                 = 0x43,
    GSTK_LAUNCH_BROWSER_CNF            = 0x44,
    GSTK_MORE_TIME_CNF                 = 0x45, /* internal use */
    GSTK_PLAY_TONE_CNF                 = 0x46,
    GSTK_POLL_INTERVAL_CNF             = 0x47, /* internal use */
    GSTK_PROVIDE_LOCAL_INFO_CNF        = 0x48, /* internal use */
    GSTK_REFRESH_CNF                   = 0x49, /* internal use */
    GSTK_RUN_AT_CMDS_CNF               = 0x4A,
    GSTK_SELECT_ITEM_CNF               = 0x4B,
    GSTK_SEND_SMS_CNF                  = 0x4C,
    GSTK_SEND_SS_CNF                   = 0x4D,
    GSTK_SEND_USSD_CNF                 = 0x4E,
    GSTK_SETUP_CALL_CNF                = 0x4F,
    GSTK_SETUP_EVENT_LIST_CNF          = 0x50,
    GSTK_SETUP_MENU_CNF                = 0x51,
    GSTK_SETUP_IDLE_TEXT_CNF           = 0x52,
    GSTK_POLLING_OFF_CNF               = 0x53, /* internal use */
    GSTK_PROVIDE_LANG_INFO_CNF         = 0x54,
    GSTK_SEND_DTMF_CNF                 = 0x55,
    GSTK_LANG_NOTIFICATION_CNF         = 0x56,
    GSTK_TIMER_MANAGEMENT_CNF          = 0x57, /* internal use */
    GSTK_OPEN_CH_CNF                   = 0x58,
    GSTK_CLOSE_CH_CNF                  = 0x59,
    GSTK_SEND_DATA_CNF                 = 0x5A,
    GSTK_RECEIVE_DATA_CNF              = 0x5B,
    GSTK_GET_CH_STATUS_CNF             = 0x5C,
    GSTK_PROVIDE_ACCESS_TECH_CNF       = 0x5D,
    GSTK_PROVIDE_BATT_STATUS_CNF       = 0x5E,
    GSTK_PROVIDE_NW_SEARCH_MODE_CNF    = 0x5F,
    GSTK_RAW_TERMIMAL_RESPONSE         = 0x60,
    /*
    ** Message ID's for envelope command messages sent from upper layers
    ** Upper layers required to send this as the first parameter in the
    ** library function: gstk_lib_send_envelope_cmd() as an indication
    ** to gstk which type of envelope that it is requesting gstk to pack
    ** Send forward to uim driver
    */

    /* Envelope Command Indication request */
    GSTK_PROFILE_DOWNLOAD_IND          = 0x61,
    GSTK_MENU_SELECTION_IND            = 0x62,
    GSTK_SMS_PP_DOWNLOAD_IND           = 0x63,
    GSTK_MT_CALL_EVT_IND               = 0x64,
    GSTK_CALL_CONNECTED_EVT_IND        = 0x65,
    GSTK_CALL_DISCONNECTED_EVT_IND     = 0x66,
    GSTK_IDLE_SCRN_EVT_IND             = 0x67,
    GSTK_LOCATION_STATUS_EVT_IND       = 0x68,
    GSTK_USER_ACTIVITY_EVT_IND         = 0x69,
    GSTK_MO_SMS_CTRL_IND               = 0x6A,
    GSTK_CC_IND                        = 0x6B,
    GSTK_TIMER_EXPIRE_IND              = 0x6C,
    GSTK_LANG_SELECTION_EVT_IND        = 0x6D,
    GSTK_BROWSER_TERMINATION_EVT_IND   = 0x6E,
    GSTK_CELL_BROADCAST_DOWNLOAD_IND   = 0x6F,
    GSTK_DATA_AVAIL_EVT_IND            = 0x70,
    GSTK_CH_STATUS_EVT_IND             = 0x71,
    GSTK_ACCESS_TECH_EVT_IND           = 0x72,     
    GSTK_NW_SEARCH_MODE_EVT_IND        = 0x73,
    GSTK_RAW_ENVELOPE_IND              = 0x74,
    /*
    ** Message ID's for envelope command notification as a result of
    ** the setup event list
    */
    GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY  = 0x100,
    GSTK_SETUP_EVT_USER_ACT_NOTIFY     = 0x101,
    GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY = 0x102,
    GSTK_SETUP_EVT_LANG_SEL_NOTIFY     = 0x103,
    GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY   = 0x104,
    GSTK_SETUP_EVT_CH_STATUS_NOTIFY    = 0x105,

    GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF   = 0x200,

    GSTK_CLIENT_ID_REG_REQ             = 0x300,
    GSTK_CLIENT_PRO_CMD_REG_REQ        = 0x301,
    
    /* terminal response SW1 and SW2 report */
    GSTK_TERM_RSP_SW1_SW2_RPT          = 0x302,

    GSTK_CMD_ENUM_MAX                  = 0x7FFFFFFF
}
gstk_cmd_enum_type;




/* ----------------------------- Terminal Response Result information --------------------------- */


/*===========================================================================
--  Enum: gstk_general_result_enum_type
      Terminal Response result codes destined for the STK application
      (as defined in 11.14)
      Upper layer needs to specify this in the second parameter in the
      gstk_lib_send_terminal_response to indicate the status of the command
      being executed
===========================================================================*/
typedef enum {
    /* values based on Standard */
    GSTK_COMMAND_PERFORMED_SUCCESSFULLY                              = 0x00,
    GSTK_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION                = 0x01,
    GSTK_COMMAND_PERFORMED_WITH_MISSING_INFORMATION                  = 0x02,
    GSTK_PCMD_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ             = 0x03,
    GSTK_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED         = 0x04,
    GSTK_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM       = 0x05,

    GSTK_COMMAND_SUCCESSFUL_LIMITED_SERVICE                          = 0x06,
    GSTK_COMMAND_PERFORMED_WITH_MODIFICATION                         = 0x07,
    GSTK_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE                       = 0x08,

    GSTK_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER                    = 0x10,
    GSTK_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER                  = 0x11,
    GSTK_NO_RESPONSE_FROM_USER                                       = 0x12,
    GSTK_HELP_INFORMATION_REQUIRED_BY_THE_USER                       = 0x13,

    GSTK_USSD_SS_TERMINATED_BY_THE_USER                              = 0x14,

    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND                      = 0x20,
    GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND                 = 0x21,
    GSTK_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST                     = 0x22,
    GSTK_USER_CLEARED_DOWN_CALL_BEFORE_CONNECTION_OR_NETWORK_RELEASE = 0x23,
    GSTK_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE        = 0x24,
    GSTK_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM      = 0x25,

    GSTK_LAUNCH_BROWSER_GENERIC_ERROR                                = 0x26,

    GSTK_COMMAND_BEYOND_THE_ME_CAPABILITIES                          = 0x30,
    GSTK_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME                           = 0x31,
    GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME                           = 0x32,
    GSTK_COMMAND_NUMBER_NOT_KNOWN_BY_ME                              = 0x33,
    GSTK_SS_RETURN_ERROR                                             = 0x34,
    GSTK_SMS_RP_ERROR                                           = 0x35,
    GSTK_ERROR_REQUIRED_VALUES_ARE_MISSING                           = 0x36,
    GSTK_USSD_RETURN_ERROR                                           = 0x37,
    GSTK_MULTIPLE_CARD_COMMANDS_ERROR                                = 0x38,
    GSTK_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM  = 0x39,

    GSTK_BEARER_INDEPENDENT_PROTOCOL_ERROR                           = 0x3A,
    GSTK_ACCESS_TECH_UNABLE_TO_PROCESS_CMD                           = 0x3B,
    GSTK_GENERAL_RESULT_MAX                                          = 0xFFFF
}
gstk_general_result_enum_type;


/*===========================================================================
--  Enum: gstk_additional_info_code_enum_type
      Additional Info codes based on what the gstk_general_result_enum_type,
      the following additional_info_code might required in the
      additional_result parameter in the gstk_send_terminal_response()

      additional information for "GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND"
        GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN
        GSTK_SCREEN_IS_BUSY
        GSTK_ME_CURRENTLY_BUSY_ON_CALL
        GSTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION
        GSTK_NO_SERVICE_AVAILABLE
        GSTK_ACCESS_CONTROL_CLASS_BAR
        GSTK_RADIO_RESOURCE_NOT_GRANTED
        GSTK_NOT_IN_SPEECH_CALL
        GSTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION
        GSTK_ME_CURRENTLY_BUSY_ON_SEND_DTMF_COMMAND
        GSTK_NO_USIM_ACTIVE

      additional information for Successful Send SS command:
            first byte: SS Return Result operation code
            second byte:SS Return Result parameters

      additional information for
              "GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND"
          first bype: cause value of the Cause information element from network,
                       bit 8 = 1,
            0x00 => no specific cause

     additional information for "GSTK_SS_RETURN_ERROR"
          first byte: error value in the Facility information element returned
                       by network
          0x00 => no specific cause

     additional information for "GSTK_SMS_RP_ERROR"
        first byte: cause value in the RP-Cause element of the RP-ERROR
                    message from network, bit 8 = 0,
        0x00 -> no specific cause

     additional information for "GSTK_USSD_RETURN_ERROR"
        first byte: error value in the Facility information from network
        0x00 -> no specific cause

     additional information for "GSTK_INTERACTION_WITH_CALL_CONTROL_OR_MO_
                    SHORT_MESSAGE_CONTROL_BY_USIM_PERMANENT_PROBLEM"
          options:
            no specific cause   0x00 GSTK_(NO_SPECIFIC_CAUSE_CAN_BE_GIVEN)
            action not allowed  0x01 (GSTK_ACTION_NOT_ALLOWED)
              request changed     0x02 (GSTK_REQUEST_CHANGED)

     additional information for "GSTK_LAUNCH_BROWSER_GENERIC_ERROR_CODE"
          options
             no specific cause 0x00 (GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN)
             bearer unavailable 0x01  (GSTK_BEARER_UNAVAILABLE)
             browser unavailable 0x02  (GSTK_BROWSER_UNAVAILABLE)
               me unable to read provisioning data 0x03
                       (GSTK_ME_UNABLE_TO_READ_PROVISIONING_DATA)
===========================================================================*/
typedef enum {
    /* no further parsing or change in value should be performed by gstk
    ** for this case, values are based off of standard */
    GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN                             = 0x00,
    GSTK_SCREEN_IS_BUSY                                             = 0x01,
    GSTK_ACTION_NOT_ALLOWED                                         = 0x01,
    GSTK_BEARER_UNAVAILABLE                                         = 0x01,
    GSTK_CH_UNAVAILABLE                                             = 0x01,

    GSTK_REQUEST_CHANGED                                            = 0x02,
    GSTK_ME_CURRENTLY_BUSY_ON_CALL                                  = 0x02,
    GSTK_BROWSER_UNAVAILABLE                                        = 0x02,
    GSTK_CH_CLOSED                                                  = 0x02,

    GSTK_ME_UNABLE_TO_READ_PROVISIONING_DATA                        = 0x03,
    GSTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION                        = 0x03,
    GSTK_CH_ID_INVALID                                              = 0x03,

    GSTK_NO_SERVICE_AVAILABLE                                       = 0x04,
    GSTK_BUFFER_SIZE_UNAVAILABLE                                    = 0x04,

    GSTK_ACCESS_CONTROL_CLASS_BAR                                   = 0x05,
    GSTK_SECURITY_ERROR_UNSUCCESSFUL_AUTH                           = 0x05,

    GSTK_RADIO_RESOURCE_NOT_GRANTED                                 = 0x06,
    GSTK_SIM_ME_INTERFACE_TRANSPORT_LV_UNAVAILABLE                  = 0x06,

    GSTK_NOT_IN_SPEECH_CALL                                         = 0x07,

    GSTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION                      = 0x08,

    GSTK_ME_CURRENTLY_BUSY_ON_SEND_DTMF_COMMAND                     = 0x09,

    GSTK_NO_USIM_ACTIVE                                             = 0x0A,

    GSTK_ADDITIONAL_INFO_CODE_MAX                                   = 0xFFFF
}
gstk_additional_info_code_enum_type;

/* ----------------------------- Envelope response general return status ----------------------------------- */

/*===========================================================================
--  Enum: gstk_general_envelope_rsp_enum_type
      Used in menu selection envelope download response, event download
      envelope response and sms pp download envelope response when there is
      no additional data returned from the card

      GSTK_ENVELOPE_CMD_SUCCESS: map to status word 0x9000 or 0x91xx
      GSTK_ENVELOPE_CMD_CARD_BUSY: the card is busy, mobile can choose to
                                   try later, 0x9300
      GSTK_ENVELOPE_CMD_FAIL: other status word
===========================================================================*/
typedef enum {
    GSTK_ENVELOPE_CMD_SUCCESS,
    GSTK_ENVELOPE_CMD_CARD_BUSY,
    GSTK_ENVELOPE_CMD_FAIL,
    GSTK_ENVELOPE_CMD_RSP_MAX               = 0xFFFF
}
gstk_general_envelope_rsp_enum_type;




/*===========================================================================
--  Enum: gstk_terminal_profile_rsp_enum_type
      Used in terminal profile response

      GSTK_PROFILE_RSP_SUCCESS: map to status word 0x9000 or 0x91xx
      GSTK_PROFILE_RSP_FAIL: other status word
===========================================================================*/
typedef enum {
    GSTK_PROFILE_RSP_SUCCESS,
    GSTK_PROFILE_RSP_FAIL,
    GSTK_PROFILE_RSP_MAX                    = 0xFFFF
}
gstk_terminal_profile_rsp_enum_type;



/* ----------------------------- Various enum type used in Proactive Command --------------------------------------
-------------------------------- terminal response, envelope cmd and rsp information --------------------------- */


/*===========================================================================
--  Enum: gstk_input_format_enum_type
===========================================================================*/
typedef enum {
    GSTK_SMS_DEF_ALPHABET               = 0x00,
    GSTK_YES_NO                         = 0x01,
    GSTK_NUMERICAL_ONLY                 = 0x02,
    GSTK_UCS2_ALPHABET                  = 0x03,
    GSTK_IMMEDIATE_DIGIT_RSP            = 0x04,
    GSTK_YES_NO_AND_IMM_DGT_RSP         = 0x05,
    GSTK_INPUT_FORMAT_MAX               = 0xFFFF
}
gstk_input_format_enum_type;

/*===========================================================================
--  Enum: gstk_input_pack_format_enum_type
===========================================================================*/
typedef enum {
    GSTK_UNPACK_FORMAT              = 0x00,
    GSTK_PACK_FORMAT                = 0x01,
    GSTK_INPUT_PACK_FORMAT_MAX      = 0xFFFF
}
gstk_input_pack_format_enum_type;

/*===========================================================================
--  Enum: gstk_menu_present_enum_type
      Menu presentation format avaliabled for select item based on the
      command detail parsing in the select item proactive command
===========================================================================*/
typedef enum {
    GSTK_PRESENTATION_NOT_SPECIFIED     = 0x00,
    GSTK_DATA_VALUE_PRESENTATION        = 0x01,
    GSTK_NAVIGATION_PRESENTATION        = 0x02,
    GSTK_MENU_PRESENT_MAX               = 0xFFFF
}
gstk_menu_present_enum_type;


/*===========================================================================
--  Enum: gstk_tone_enum_type
      Various type of tone available for the play tone command
      as specified in the corresponding command detail tlv


      NOT_IN_USE: to indicate it is optional
===========================================================================*/
typedef enum {
    /* values based from Standard */
    GSTK_DIAL_TONE                       = 0x01,
    GSTK_CALLED_SUBSCRIBER_BUSY          = 0x02,
    GSTK_CONGESTION                      = 0x03,
    GSTK_RADIO_PATH_ACK                  = 0x04,
    GSTK_RADIO_PATH_NOT_AVAIL_CALL_DROP  = 0x05,
    GSTK_ERROR_TONE                      = 0x06,
    GSTK_CALL_WAITING_TONE               = 0x07,
    GSTK_RINGING_TONE                    = 0x08,
    GSTK_GENERAL_BEEP                    = 0x10,
    GSTK_POSITIVE_ACK_TONE               = 0x11,
    GSTK_NEGATIVE_ACK_TONE               = 0x12,
    /* UTK */
    GSTK_RINGING_TONE_SELECTED_BY_USER   = 0x13,
    GSTK_SMS_ALERT_TONE_SELECTED_BY_USER = 0x14,
    /* to indicate it is optional */
    GSTK_NOT_IN_USE                      = 0xAA,
    GSTK_TONE_MAX                        = 0xFFFF
}
gstk_tone_enum_type;



/*===========================================================================
--  Enum: gstk_call_setup_req_enum_type
      Various type of call setup parameters as specified in the command detail
===========================================================================*/
typedef enum {
    GSTK_NO_OTHER_CALLS                 = 0x00,
    GSTK_HOLD_ACTIVE_CALLS              = 0x01,
    GSTK_DISCONNECT_ACTIVE_CALLS        = 0x02,
    GSTK_CALL_SETUP_REQ_MAX             = 0xFFFF
}
gstk_call_setup_req_enum_type;


/*===========================================================================
--  Enum: gstk_launch_mode_enum_type
      Various launching browser mode as specified in the command detail
===========================================================================*/
typedef enum {
    /* values based on standard */
    GSTK_LAUNCH_IF_NOT_ALREADY_LAUNCH   = 0x00,
    GSTK_USE_EXISTING_BROWSER           = 0x02,
    GSTK_CLOSE_EXISTING_BROWSER         = 0x03,
    GSTK_LAUNCH_MODE_MAX                = 0xFFFF
}
gstk_launch_mode_enum_type;

/*===========================================================================
--  Enum: gstk_bearer_code_enum_type
===========================================================================*/
typedef enum {
    /* based on standard values */
    GSTK_SMS                            = 0x00,
    GSTK_CSD                            = 0x01,
    GSTK_USSD_BEARER_CODE               = 0x02,
    GSTK_GPRS                           = 0x03,
    GSTK_BEARER_DEFAULT                 = 0x04,
    /* QUESTION:  ok to change from RFU to MAX? */
    GSTK_RFU                            = 0x00FF,
    GSTK_BEARER_CODE_MAX                = 0xFFFF /* 0x04 - 0xFF = RFU */
}
gstk_bearer_code_enum_type;



/*===========================================================================
--  Enum: gstk_call_enum_type
      This is used so that GSTK/Client can cast the correct union structure type
      according to whether it is a voice call, SS or USSD


      GSTK_NONE: used to indicate that there is no voice/ss/ussd related
                 structure type required => when the field is optional

      GSTK_VOICE_PTR, GSTK_SS_PTR and GSTK_USSD_PTR are used to indicate if 
      the information additional info for the call control extran param
      terminal response are in the pointer format or not
===========================================================================*/
typedef enum {
    GSTK_VOICE,
    GSTK_SS,
    GSTK_USSD,
    GSTK_VOICE_PTR,
    GSTK_SS_PTR,
    GSTK_USSD_PTR,
    GSTK_PDP_CONTEXT_ACT,
    GSTK_NONE,
    GSTK_CALL_MAX        = 0xFFFF
}gstk_call_enum_type;


/*===========================================================================
--  Enum: gstk_control_result_enum_type
===========================================================================*/
typedef enum {
    /* based on standard values */
    GSTK_ALLOWED_NO_MOD     = 0x00,
    GSTK_NOT_ALLOWED        = 0x01,
    GSTK_ALLOWED_BUT_MOD    = 0x02,
    GSTK_CONTROL_RESULT_MAX = 0xFFFF
}
gstk_control_result_enum_type;


/*===========================================================================
--  Enum: gstk_bc_repeat_ind_enum_type
===========================================================================*/
typedef enum {
    /* based on standard values */
    GSTK_ALTERNATE_MODE     = 0x01,
    GSTK_SEQUENTIAL_MODE    = 0x03,
    GSTK_BC_REPEAT_IND_MAX  = 0xFFFF
}
gstk_bc_repeat_ind_enum_type;


/*===========================================================================
--  Enum: gstk_ton_enum_type
===========================================================================*/
typedef enum {
   /* based on standard values */
   GSTK_UNKNOWN_TON          = 0x00,
   GSTK_INTERNATIONAL_NUM    = 0x01,
   GSTK_NATIONAL_NUM         = 0x02,
   GSTK_NETWORK_SPECIFIC_NUM = 0x03,
   // QUESTION
   GSTK_RFU_TON              = 0x00FF,
   GSTK_TON_MAX              = 0xFFFF
}
gstk_ton_enum_type;

/*===========================================================================
--  Enum: gstk_npi_enum_type
===========================================================================*/
typedef enum {
   /* based on standard values */
   GSTK_UNKNOWN_NPI          = 0x00,
   GSTK_ISDN_TELEPHONY_NPI   = 0x01,
   GSTK_DATA_NPI             = 0x03,
   GSTK_TELEX_NPI            = 0x04,
   GSTK_PRIVATE_NPI          = 0x09,
   GSTK_EXT_RESERVED_NPI     = 0x0F,
   // QUESTION
   GSTK_RFU_NPI              = 0x00FF,
   GSTK_NPI_MAX              = 0xFFFF
}
gstk_npi_enum_type;


/*===========================================================================
--  Enum: gstk_slot_id_enum_type 
      Intended for Dual Slot build only
      GSTK_SLOT_NONE should never be used in the functions that require slot
      information
===========================================================================*/
typedef enum {
    GSTK_SLOT_NONE           = 0x00,
    GSTK_SLOT_1              = 0x01,
    GSTK_SLOT_2              = 0x02,
    GSTK_SLOT_ID_MAX         = 0xFFFF
}
gstk_slot_id_enum_type;



/*===========================================================================
--  Enum: gstk_img_code_enum_type 
===========================================================================*/
typedef enum {
  GSTK_UNKNOWN_IMAGE_CODE_SCHEME,
  GSTK_BASIC_IMAGE_CODE_SCHEME,
  GSTK_COLOUR_IMAGE_CODE_SCHEME,
  GSTK_IMAGE_CODE_SCHEME_MAX        = 0xFFFF
}
gstk_img_code_enum_type;


typedef enum {
  GSTK_DCS_8_BIT,
  GSTK_DCS_UCS2_8_BIT,
  GSTK_DCS_UCS2_7_BIT,
  GSTK_DCS_7_BIT,
  GSTK_DCS_NONE,
  GSTK_DCS_MAX                      = 0xFFFF
}
gstk_dcs_enum_type;


/* apitrim -include */
typedef enum{
 GSTK_APP_NONE,
 GSTK_APP_SIM,
 GSTK_APP_RUIM,
 GSTK_APP_USIM,
 GSTK_APP_UNKNOWN                  = 0xFFFF
}gstk_app_enum_type;
/* apitrim -end-include */


/*--------------------------------------------------------------------------
                             Struct Definitions
---------------------------------------------------------------------------*/

/*===========================================================================
--  Struct: gstk_profile_dl_support_type
      This structure contains profile download info that is required to be
      filled in by the client during profile download
===========================================================================*/
typedef struct {
    uint8         value;
    uint32        profile_dl_bitmask;
}
gstk_profile_dl_support_type;


/*===========================================================================
--  Struct: gstk_profile_dl_support_ext_type
      This structure contains profile download info that is required to be
      filled in by the client during profile download. This structure 
      differs from "gstk_profile_dl_support_type" as it differentiates
      between sat and usat tp download.
===========================================================================*/
typedef struct {
    uint8         value;
    uint32        profile_dl_bitmask;
    uint8         card_app_type;
}
gstk_profile_dl_support_ext_type;


/*===========================================================================
--  Struct: gstk_exp_hdr_type
      This structure is the header which every commands between GSTK and
      clients of the gstk_exp.h should use


      cmd_detail_reference:  client should pass the same ref_id back to
                                       GSTK in terminal response
      command_number: only valid for proactive commands
===========================================================================*/
typedef struct {
    gstk_cmd_enum_type          command_id;
    uint32                      cmd_detail_reference;
    uint8                       command_number;
    uint32                      user_data;
}
gstk_exp_hdr_type;



/* ----------------------------------- Structures to be ----------------------------------------------------
---------------------------------- used in various gstk_xxxx_req_type, -------------------------------------
---------------------------------- gstk_xxxx_rsp_type, the terminal response function ----------------------
---------------------------------- parameter and envelope command function parameter ----------------------*/


/*---------------------------------------- General structures ------------------------------*/

/*===========================================================================
--  Struct: gstk_string_type
      This structure contains string info for various commands

      length:  length of text
===========================================================================*/
typedef struct {
    uint8                        dcs; /* 0x04 for GSM Default Char
                                         0x08 for UCS2 formatting */
    uint8                        length;
    uint8*                       text;
    /* Questions:
    1. DCS handled in GSTK during parsing, so always sent "standard" text
    2. include DCS in this structure so that upper layer can use it
    */
    /*~ FIELD gstk_string_type.text VARRAY LENGTH gstk_string_type.length */
}
gstk_string_type;


/*===========================================================================
--  Struct: gstk_cb_page_type
      This structure contains cell broadcast page message

      length:  length of cell broadcast page
===========================================================================*/
typedef struct
{
  uint8                               length;
  uint8                               *cb_page_data;
}
gstk_cb_page_type;


/*===========================================================================
--  Struct: gstk_icon_type
      This structure contains the icon info for various commands
      present: this type is presented or not
      qualifier:  0 = display without text string
                    1 = display together with text
      height, width:  get the size of the icon: width, height from the
                      EF_IMG file (4F20), the width and height is expressed
                      in raster image points
      size:  based on file ID from the EF_IMG, GSTK is required to
             read the Image Instance Data Files (binary file), the
             size of the image instance can be obtained from the
             record's image instance data descriptor
      data:  Pointer to image instance data from the Image
                            Instance Data Files
      rec_num: Record Number in EF_IMG

      Limitation: only 1 instance of the icon is
      obtained even if there are multiple icon instance
      residing in the same record in EF_IMG
===========================================================================*/
typedef struct {
    boolean                       present;
    uint8                         qualifier;
    uint8                         height;
    uint8                         width;
    uint32                        size;
    gstk_img_code_enum_type       image_coding_scheme;
    uint8                         *data;
    uint8                         rec_num;
    /*~ FIELD gstk_icon_type.data VARRAY LENGTH gstk_icon_type.size */
}
gstk_icon_type;



/*===========================================================================
--  Struct: gstk_item_object_type
      This structure contains the item info for Select Item and Set Up Menu

      length:  length of the text
===========================================================================*/
typedef struct {
    uint8                        item_id;
    uint8                        length;
    uint8                        *item_text;
  /*~ FIELD gstk_item_object_type.item_text VARRAY LENGTH gstk_item_object_type.length */
}
gstk_item_object_type;


/*===========================================================================
--  Struct: gstk_next_item_ind_type
      This structure contains the next item indication

      num_items: from length field
      next_action_list are (based on section 9,4):
         CMD_STK_SET_UP_CALL
         CMD_STK_SEND_SS
         CMD_STK_USSD
         CMD_STK_SEND_SHORT_MESSAGE
         CMD_STK_LAUNCH_BROWSER
         CMD_STK_PLAY_TONE
         CMD_STK_DISPLAY_TEXT
         CMD_STK_GET_INKEY
         CMD_STK_GET_INPUT
         CMD_STK_SELECT_ITEM
         CMD_STK_SET_UP_MENU
         CMD_STK_SET_UP_IDLE_MODE_TEXT
         CMD_STK_END_OF_PROACTIVE_SES

         (future)
         CMD_STK_PERFORM_CARD_APDU
         CMD_STK_POWER_ON_CARD
         CMD_STK_POWER_OFF_CARD
         CMD_STK_GET_READER_STATUS
         CMD_STK_OPEN_CHANNEL
         CMD_STK_CLOSE_CHANNEL
         CMD_STK_RECEIVE_DATA
         CMD_STK_SEND_DATA
         CMD_STK_GET_CHANNEL_STATUS
===========================================================================*/
typedef struct {
    uint8                        num_items;
    uint8                        *next_action_list;
  /*~ FIELD gstk_next_item_ind_type.next_action_list VARRAY LENGTH gstk_next_item_ind_type.num_items */
}
gstk_next_item_ind_type;


/*===========================================================================
--  Struct: gstk_icon_id_list_type
      This structure contains the icon list

      num_items:  from length field
      icon_list: the qualifier from the icon id list tlv will be populated
                 to each gstk_icon_type's qualifier element
===========================================================================*/
typedef struct {
    boolean                      display_icon_only;
    uint8                        num_items;
    gstk_icon_type             * icon_list[GSTK_MAX_MENU_ITEMS];
  /*~ FIELD gstk_icon_id_list_type.icon_list VARRAY GSTK_MAX_MENU_ITEMS LENGTH gstk_icon_id_list_type.num_items
      gstk_icon_id_list_type.icon_list[] POINTER */
}
gstk_icon_id_list_type;

/*===========================================================================
--  Struct: gstk_at_cmd_type
      This structure contains the at command
===========================================================================*/
typedef struct {
    uint8                        length;
    uint8                        *at_cmd;
  /*~ FIELD gstk_at_cmd_type.at_cmd VARRAY LENGTH gstk_at_cmd_type.length */
}
gstk_at_cmd_type;

/*===========================================================================
--  Struct: gstk_duration_type
      This structure contains the duration information

      present:  to indicate whether this struct
                is optional or not
      units:  GSTK_DURATION_MINUTES,
              GSTK_DURATION_SECONDS,
              GSTK_DURATION_TENTHS_OF_SECONDS
===========================================================================*/
typedef struct {
    boolean                      present;
    uint8                        units;
    uint8                        length; /* time interval : range 0-255 */
}
gstk_duration_type;


/*===========================================================================
--  Struct: gstk_address_type
      This structure contains the address information

      length:  length of the address field
===========================================================================*/
typedef struct {
    gstk_ton_enum_type           TON;
    gstk_npi_enum_type           NPI;
    uint8                        length;
    uint8                        address[GSTK_MAX_ADDRESS_LEN];
}
gstk_address_type;


/*===========================================================================
--  Struct: gstk_subaddress_type
      This structure contains the subaddress information
===========================================================================*/
typedef struct {
    uint8                        length;
    uint8                        subaddress[GSTK_MAX_SUBADDR_LEN];
}
gstk_subaddress_type;


/*===========================================================================
--  Struct: gstk_ussd_string_type
      This structure contains the ussd string

      length:  length of the text field
      dcs:  as coded in for cell broadcast
===========================================================================*/
typedef struct {
    uint8                        orig_dcs_from_sim;
    uint8                        dcs;
    uint8                        length;
    uint8                        *text;
  /*~ FIELD gstk_ussd_string_type.text VARRAY LENGTH gstk_ussd_string_type.length */ 
}
gstk_ussd_string_type;


/*===========================================================================
--  Struct: gstk_sms_tpdu_type
      This structure contains the SMS TPDU information

      tpdu:  tpdu includes TP-Message Reference.
             The TP-Message Reference provided
             by the card might not be valide, ME needs to check and
             modify this value accordingly
===========================================================================*/
typedef struct {
    uint8                        length;
    uint8                        *tpdu;
  /*~ FIELD gstk_sms_tpdu_type.tpdu VARRAY LENGTH gstk_sms_tpdu_type.length */
}
gstk_sms_tpdu_type;


/*===========================================================================
--  Struct: gstk_cap_config_type
      This structure contains the capability configuration information used
      by various commands

      ccp[MAX_CCP_LEN]:  ccp is coded as in EF-CCP
===========================================================================*/
typedef struct {
    uint8                        length;

    /* Capability Configuration Parameters - See 8.4 */
    uint8                        ccp[GSTK_MAX_CCP_LEN];
    /* Questionssss What kind of parsing is being done and by whom in the current EF-CCP read?
       which module is using this information?
       if the parsing function exists, can other layers use this api to parse?
       what kind of format is expected for ccp before it is being sent down to lower layer,
       who expects that? cm? nas????
       */
}
gstk_cap_config_type;


/*===========================================================================
--  Struct: gstk_redial_type
      This structure contains the redial information used by Set Up Call
      command

      necessary:  from command detail
      timeout:  from optional Duration TLV
===========================================================================*/
typedef struct {
    boolean                      necessary;    /* from command detail */
    /* Optional */
    gstk_duration_type           timeout;      /* from optional Duration TLV */
}
gstk_redial_type;

/*===========================================================================
--  Struct: gstk_url_type
      This structure contains the URL used by Launch Browser
      command
      If the length is zero, application is required to use default URL.
      The url is coded in SMS 7bit default alphabet with bit8 = 0
===========================================================================*/
typedef struct {
    uint8                        length;
    uint8                        *url;
  /*~ FIELD gstk_url_type.url VARRAY LENGTH gstk_url_type.length */
}
gstk_url_type;



/*===========================================================================
--  Struct: gstk_bearer_type
      This structure contains the bearer information used by Launch Browser
      command
===========================================================================*/
typedef struct {
    uint8                        length;
    gstk_bearer_code_enum_type   *bearer_list;
  /*~ FIELD gstk_bearer_type.bearer_list VARRAY LENGTH gstk_bearer_type.length */
}
gstk_bearer_type;


/*===========================================================================
--  Struct: gstk_file_list_type
      This structure contains the file list information used by Refresh or
      Launch Browser command
        Used in Refresh Cmd when refresh mode ==
               FCN
               USIM Initialization & FCN
               3G session reset
             for other mode, GSTK will set the length = 0
             to indicate that this structure is not required
             in the refresh cmd process
         Also used as optional provisioning file reference in
          Launch Browser Cmd

      length:  total len of valid data in file_paths
      file_paths:  file_paths is the full path name for all the files
                   specified from the refresh cmd
                   e.g. file_paths = 3F002FE23F007F102F00.....
===========================================================================*/
typedef struct {
    uint8                        length;
    uint8                        num_of_files;
    uint8                        *file_paths;
  /*~ FIELD gstk_file_list_type.file_paths VARRAY LENGTH gstk_file_list_type.length */
}
gstk_file_list_type;

/*===========================================================================
--  Struct: gstk_ch_status_additional_info_enum_type
      This enum indicates the channel status additional info
===========================================================================*/
typedef enum {
    GSTK_CH_STATUS_NO_FURTHER_INFO,
    GSTK_CH_STATUS_LINK_DROPPED,
    GSTK_CH_STATUS_ADDITIONAL_INFO_NOT_USED,
    GSTK_CH_STATUS_ADDITIONAL_INFO_RFU,
    GSTK_CH_STATUS_ADDITIONAL_INFO_MAX       = 0xFFFF
}
gstk_ch_status_additional_info_enum_type;

/*===========================================================================
--  Struct: gstk_ch_status_type
      This structure includes channel status info

      channel_id
      is_link_est:   TRUE if link is established or PDP context activated
      ch_status_additional_info:  additional channel status information
===========================================================================*/
typedef struct {
    int32                                     ch_id;
    boolean                                   is_link_est;
    gstk_ch_status_additional_info_enum_type  ch_status_additional_info;
}
gstk_ch_status_type;

/*===========================================================================
--  Struct: gstk_data_avail_evt_ind_type
      This structure includes info that the clients are required to supply
      in Data Available event download.

      ch_status
      ch_len:   Length of the data available
===========================================================================*/
typedef struct {
    gstk_ch_status_type                ch_status;
    int32                              ch_len;
}
gstk_data_avail_evt_ind_type;

/*===========================================================================
--  Struct: gstk_alphabet_type
      This structure contains bytes array with each element represented
      in UCSII 8 bit, the length field indicates the number of uint16 in the
      alphabet buffer
===========================================================================*/
typedef struct {
    boolean                      alphabet_given;
    int32                        length;
    uint16                       *alphabet;
  /*~ FIELD gstk_alphabet_type.alphabet VARRAY LENGTH gstk_alphabet_type.length */
}
gstk_alphabet_type;

/*===========================================================================
--  Enum: gstk_packet_data_protocol_enum_type
      packet data protocol:  GSTK_PKT_DATA_IP or GSTK_PKT_DATA_MAX
===========================================================================*/
typedef enum {
  GSTK_PKT_DATA_IP                     = 0x02,
  GSTK_PKT_DATA_PROTOCOL_MAX           = 0xFFFF
}
gstk_packet_data_protocol_enum_type;

typedef enum {
  GSTK_CSD_BEARER_SPEED_AUTO             = 0,   
  GSTK_CSD_BEARER_SPEED_300_V21          = 1,
  GSTK_CSD_BEARER_SPEED_1200_V22         = 2,
  GSTK_CSD_BEARER_SPEED_1200_75_V23      = 3,
  GSTK_CSD_BEARER_SPEED_2400_V22         = 4,
  GSTK_CSD_BEARER_SPEED_2400_V26         = 5,
  GSTK_CSD_BEARER_SPEED_4800_V32         = 6,
  GSTK_CSD_BEARER_SPEED_9600_V32         = 7,
  GSTK_CSD_BEARER_SPEED_9600_V34         = 12,
  GSTK_CSD_BEARER_SPEED_14400_V34        = 14,
  GSTK_CSD_BEARER_SPEED_19200_V34        = 15,
  GSTK_CSD_BEARER_SPEED_28800_V34        = 16,
  GSTK_CSD_BEARER_SPEED_33600_V34        = 17,
  GSTK_CSD_BEARER_SPEED_1200_V120        = 34,
  GSTK_CSD_BEARER_SPEED_2400_V120        = 36,
  GSTK_CSD_BEARER_SPEED_4800_V120        = 38,
  GSTK_CSD_BEARER_SPEED_9600_V120        = 39,
  GSTK_CSD_BEARER_SPEED_14400_V120       = 43,
  GSTK_CSD_BEARER_SPEED_19200_V120       = 47,
  GSTK_CSD_BEARER_SPEED_28800_V120       = 48,
  GSTK_CSD_BEARER_SPEED_38400_V120       = 49,
  GSTK_CSD_BEARER_SPEED_48000_V120       = 50,
  GSTK_CSD_BEARER_SPEED_56000_V120       = 51,
  GSTK_CSD_BEARER_SPEED_300_V110         = 65,
  GSTK_CSD_BEARER_SPEED_1200_V110        = 66,
  GSTK_CSD_BEARER_SPEED_2400_V110_X31    = 68,
  GSTK_CSD_BEARER_SPEED_4800_V110_X31    = 70,
  GSTK_CSD_BEARER_SPEED_9600_V110_X31    = 71,
  GSTK_CSD_BEARER_SPEED_14400_V110_X31   = 75,
  GSTK_CSD_BEARER_SPEED_19200_V110_X31   = 79,
  GSTK_CSD_BEARER_SPEED_28800_V110_X31   = 80,
  GSTK_CSD_BEARER_SPEED_38400_V110_X31   = 81,
  GSTK_CSD_BEARER_SPEED_48000_V110_X31   = 82,
  GSTK_CSD_BEARER_SPEED_56000_V110_X31   = 83,
  GSTK_CSD_BEARER_SPEED_64000_V110_X31   = 84,
  GSTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT   = 115,
  GSTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT   = 116,
  GSTK_CSD_BEARER_SPEED_32000_PIAFS32K   = 120,
  GSTK_CSD_BEARER_SPEED_64000_PIAFS32K   = 121,
  GSTK_CSD_BEARER_SPEED_28800_MULTIMEDIA        = 130,
  GSTK_CSD_BEARER_SPEED_32000_MULTIMEDIA        = 131,
  GSTK_CSD_BEARER_SPEED_33600_MULTIMEDIA        = 132,
  GSTK_CSD_BEARER_SPEED_56000_MULTIMEDIA        = 133,
  GSTK_CSD_BEARER_SPEED_64000_MULTIMEDIA        = 134,
  GSTK_CSD_BEARER_SPEED_MAX                     = 0xFFFF
}
gstk_csd_bearer_speed_enum_type;

typedef enum
{
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ = 0,
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ  = 1,
  GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI           = 2,
  GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI        = 3,
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI        = 4,
  GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI         = 5,
  GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI           = 6,
  GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI         = 7,
  GSTK_CSD_BEARER_NAME_MAX                            = 0xFFFF
}
gstk_csd_bearer_name_enum_type;

typedef enum
{
  GSTK_CSD_BEARER_CE_TRANSPARENT                    = 0,
  GSTK_CSD_BEARER_CE_NON_TRANSPARENT                = 1,
  GSTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF          = 2,
  GSTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF      = 3,
  GSTK_CSD_BEARER_CE_MAX                            = 0xFFFF
}
gstk_csd_bearer_conn_element_enum_type;

/*===========================================================================
--  Struct: gstk_csd_bearer_param_type
      This structure contains the parameter information for CSD
      speed:              "speed" subparameter in 27.007
      name:               "name" subparameter in 27.007
      connection_element: "ce" subparameter in 27.007
===========================================================================*/
typedef struct {
  gstk_csd_bearer_speed_enum_type        speed;
  gstk_csd_bearer_name_enum_type         name;
  gstk_csd_bearer_conn_element_enum_type connection_element;  
}
gstk_csd_bearer_param_type;


/*===========================================================================
--  Struct: gstk_gprs_bearer_param_type
      This structure contains the parameter information for GPRS     
      precedence_cls:     value corresponding to "precedence" subparameter in 27.007
      delay_cls:          value corresponding to "delay" subparameter in 27.007
      reliability_cls:    value corresponding to "reliability" subparameter in 27.007
      peak_throughput:    value corresponding to "peak" subparameter in 27.007
      mean_throughput:    value corresponding to "mean" subparameter in 27.007
      pkt_data_protocol:  GSTK_PKT_DATA_IP or GSTK_PKT_DATA_RFU
===========================================================================*/
typedef struct {
  int32                                precedence_cls;
  int32                                delay_cls;
  int32                                reliability_cls; 
  int32                                peak_throughput; 
  int32                                mean_throughput; 
  gstk_packet_data_protocol_enum_type  pkt_data_protocol; 
}
gstk_gprs_bearer_param_type;

/*===========================================================================
--  Struct: gstk_bearer_description_type
      This structure contains the bearer type and bearer parameters
      bearer_type:  GSTK_CSD, GSTK_GPRS or GSTK_DEFAULT 
        GSTK_CSD: bearer_params.csd_bearer should be looked at in establishing the 
                  bearer for the transport protocol
        GSTK_GPRS: bearer_params.gprs_bearer should be looked at in establishing the 
                   bearer for the transport protocol
        GSTK_DEFAULT: mobile should use default bearer for thet transport 
                      protocol specified.  There is no member in the union that
                      mobile needs to look at in related to the bearer
===========================================================================*/
typedef struct {
  gstk_bearer_code_enum_type     bearer_type;
  union {
    gstk_csd_bearer_param_type   csd_bearer;
    gstk_gprs_bearer_param_type  gprs_bearer;
  /*~ CASE GSTK_CSD  gstk_bearer_description_type.bearer_params.csd_bearer */
  /*~ CASE GSTK_GPRS gstk_bearer_description_type.bearer_params.gprs_bearer */
  /*~ DEFAULT        gstk_bearer_description_type.bearer_params.void */
  } bearer_params;
  /*~ FIELD gstk_bearer_description_type.bearer_params DISC gstk_bearer_description_type.bearer_type */
}
gstk_bearer_description_type;
/*===========================================================================
--  Struct: gstk_pkt_data_address_enum_type
      Enum indicating the data IP address format
===========================================================================*/
typedef enum {
  GSTK_PKT_DATA_NO_ADDRESS_GIVEN,
  GSTK_PKT_DATA_DYNAMIC_ADDRESS,
  GSTK_PKT_DATA_IPV4_ADDRESS,
  GSTK_PKT_DATA_IPV6_ADDRESS,
  GSTK_PKT_DATA_ADDRESS_MAX                     = 0xFFFF
}
gstk_pkt_data_address_enum_type;

/*===========================================================================
--  Struct: gstk_data_address_type
      This structure contains the packet data address
      length:  indicates the length of the address buffer
      address:  when address type == GSTK_PKT_DATA_IPV4_ADDRESS, 4 bytes IP4
                address will be given with Bit 8 of first byte the most 
                significant bit of the IP address
                when address type == GSTK_PKT_DATA_IPV6_ADDRESS, 16 bytes IP6
                address will be given with Bit 8 of first byte the most
                significant bit of the IP address
===========================================================================*/
typedef struct {
  int32                        length;
  uint8                        *address;
}
  /*~ FIELD gstk_data_address_type.address VARRAY LENGTH gstk_data_address_type.length */
gstk_data_address_type;

/*===========================================================================
--  Struct: gstk_pkt_data_address_type
      This structure contains the packet data address information
      address_type:  GSTK_PKT_DATA_DYNAMIC_ADDRESS (no info for pkt_address)
                     GSTK_PKT_DATA_IPV4_ADDRESS, or GSTK_PKT_DATA_IPV6_ADDRESS,
                     GSTK_PKT_DATA_NO_ADDRESS_GIVEN
      pkt_address:  bearer parameter for GSTK_CSD
===========================================================================*/
typedef struct {
  gstk_pkt_data_address_enum_type     address_type;
  gstk_data_address_type              pkt_address;  
}
gstk_pkt_data_address_type;

/*===========================================================================
--  Struct: gstk_pkt_data_transport_level_enum_type
      Indicate whether UDP or TCP transport protocol should be used
      When the SAT/USAT application provides value that falls in the RFU, GSTK
      will reject the command and send the terminal response with 
      GSTK_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME
===========================================================================*/
typedef enum {
  GSTK_PKT_DATA_NOT_PRESENT,
  GSTK_PKT_DATA_UDP,
  GSTK_PKT_DATA_TCP,
  GSTK_PKT_DATA_TRANSPORT_LEVEL_MAX              = 0xFFFF
}
gstk_pkt_data_transport_level_enum_type;

/*===========================================================================
--  Struct: gstk_pkt_data_transport_level_type
      This structure contains the transport protocol and the port number
      transport_protocol:  GSTK_PKT_DATA_UDP or GSTK_PKT_DATA_TCP,
                           GSTK_PKT_DATA_NOT_PRESENT, 
                           GSTK_PKT_DATA_MAX (port number will be invalid in
                           this case)
      port_number
===========================================================================*/
typedef struct {
  gstk_pkt_data_transport_level_enum_type   transport_protocol;
  uint32                                    port_number;  
}
gstk_pkt_data_transport_level_type;


/*===========================================================================
--  Struct: gstk_ch_data_type
      This structure contains the channel data
      data_len:  length of the data pointer
      data
===========================================================================*/
typedef struct
{
    int32                data_len;
    uint8               *data;
  /*~ FIELD gstk_ch_data_type.data VARRAY LENGTH gstk_ch_data_type.data_len */
}
gstk_ch_data_type;


/*---------------------------------------- Terminal Response related structures ------------------------------*/

/* structures used when communication goes from ME -> SIM/USIM */


/*===========================================================================
--  Struct: gstk_additional_info_type
      This structure contains the additional information used by all
      terminal response.
      If there is no additional info, the length should be set to 0

      additional_info[GSTK_MAX_ADDITIONAL_INFO]: Refer to
               gstk_additional_info_code_enum_type for values:
               the value can either be directly from that enum type
               or some cause/result/error values from the network
===========================================================================*/
typedef struct {
    uint8                      length;
    uint8                      additional_info[GSTK_MAX_ADDITIONAL_INFO];
}
gstk_additional_info_type;
/*~ FIELD gstk_additional_info_type.additional_info VARRAY GSTK_MAX_ADDITIONAL_INFO LENGTH gstk_additional_info_type.length */


/*===========================================================================
--  Struct: gstk_additional_info_ptr_type
      This structure contains the additional information used by all
      terminal response.
      If there is no additional info, the length should be set to 0

      additional_info_ptr: Refer to gstk_additional_info_code_enum_type 
               for values: the value can either be directly from that enum type
               or some cause/result/error values from the network
===========================================================================*/
typedef struct {
    uint8                      length;
    uint8                      *additional_info_ptr;
}
gstk_additional_info_ptr_type;
/*~ FIELD gstk_additional_info_ptr_type.additional_info_ptr VARRAY LENGTH gstk_additional_info_ptr_type.length */

/*===========================================================================
--  Struct: gstk_at_rsp_type (future)
      This structure contains the at response information.
      It is used in terminal response for Run AT Command
===========================================================================*/
/*
typedef struct
{
   uint8                        length;
   uint8                        *at_rsp_string;
}
gstk_at_rsp_type;
*/


/*===========================================================================
--  Struct: gstk_cc_voice_type
      This structure contains voice call control information.
      It is used in terminal response and envelope response
===========================================================================*/
typedef struct {
    gstk_address_type            address;
    /* Optional */
    gstk_subaddress_type         subaddress;
    gstk_cap_config_type         ccp1;
    gstk_cap_config_type         ccp2;
}
gstk_cc_voice_type;



/*===========================================================================
--  Struct: gstk_pdp_context_act_param_type
      length - indicates the length of the PDP Context Activation Parameter field
      pdp_act_param - indicates the PDP Context Activation information
===========================================================================*/
typedef struct {
  int32    length;
  uint8          pdp_act_param[GSTK_MAX_PDP_ACT_PARAM_SIZE];  
}
gstk_pdp_context_act_param_type;



/*===========================================================================
--  Struct: gstk_cc_req_action_type
      This structure contains the call control request action information.
      It is used in terminal response and envelope response

      call_type:  if call_type == GSTK_NONE, this implies there is no optional
      gstk_address_string => no change in the call type or address
      or ss string or ussd string
      union {
        voice_info:  set up call
        ss_string:  send ss
        ussd_string:  send ussd
      }
      bc_repeat_ind:  avaliable only if ccp2 is presented
===========================================================================*/
typedef struct {
    uint8                        length;

    /* Optional */
    gstk_call_enum_type          call_type;
    union {
        gstk_cc_voice_type                voice_info;
        gstk_address_type                 ss_string;
        gstk_ussd_string_type             ussd_string;
        gstk_pdp_context_act_param_type   pdp_context_param;
  /*~ IF ( _DISC_ == GSTK_VOICE || _DISC_ == GSTK_VOICE_PTR ) gstk_cc_req_action_type.gstk_address_string.voice_info*/
  /*~ IF ( _DISC_ == GSTK_SS    || _DISC_ == GSTK_SS_PTR    ) gstk_cc_req_action_type.gstk_address_string.ss_string */
  /*~ IF ( _DISC_ == GSTK_USSD  || _DISC_ == GSTK_USSD_PTR  ) gstk_cc_req_action_type.gstk_address_string.ussd_string */
  /*~ CASE GSTK_NONE            gstk_cc_req_action_type.gstk_address_string.void */
  /*~ CASE GSTK_PDP_CONTEXT_ACT gstk_cc_req_action_type.gstk_address_string.pdp_context_param */
  /*~ DEFAULT                   gstk_cc_req_action_type.gstk_address_string.void */
    }gstk_address_string;
  /*~ FIELD gstk_cc_req_action_type.gstk_address_string DISC gstk_cc_req_action_type.call_type */
    gstk_string_type             alpha;
    gstk_bc_repeat_ind_enum_type bc_repeat_ind;
    /* section 7.3.1.6 response data */
}
gstk_cc_req_action_type;



/*---------------------- envelop Response related structures, from Card to ME --------------------------*/

/*===========================================================================
--  Struct: gstk_transaction_id_type
      This structure contains the transaction id.  It is used in MT Call,
      Call Connected and Call Disconnected event download command

      transaction_list:  - each transaction id shall not
                           appear more than once in the list
                         - bits 1-4: RFU
                           bits 5-7: TI value
                           bits 8  : TI flag
                         - TI value and flag are coded as defined in
                           3G 24.007
===========================================================================*/
typedef struct{
    uint8                        length;
    uint8                        *transaction_list;
/*~ FIELD gstk_transaction_id_type.transaction_list VARRAY LENGTH gstk_transaction_id_type.length */
}
gstk_transaction_id_type;


/*===========================================================================
--  Struct: gstk_cause_type
      This structure contains the cause information.  It is used in MT event
      download command

      cause:  coded according to Cause call control information element
              in 3G 24.008,
              starting at octet 3, i.e., remove the  length and
              IEI information from 3G 24.008
              - radio link timeout is indicated by the cause data object
                having a value part of 0 length
===========================================================================*/
typedef struct {
    boolean                      present;
    uint8                        length;
    uint8                        *cause;
/*~ FIELD gstk_cause_type.cause VARRAY LENGTH gstk_cause_type.length */

}
gstk_cause_type;



/*---------------------------------------------------------------
            Upper Layers <-> GSTK Interface Definitions
----------------------------------------------------------------*/

/*---------------------------------------- Terminal Response related structures ------------------------------------
------------------------------------------ used in the gsdi_send_terminal_response()------------------------------*/


/*===========================================================================
--  Struct: gstk_call_ss_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a Set Up Call or Send SS
      proactive command request

      has_cc_type_modification:  specified whether this struct is
                                 required or not in the terminal
                                 response, only required if the type of
                                 call has been changed
      command_result2, additional_info2:  results of the new call type
                                          response from the network
      text_string2:  needed only if SIM/UICC converts setup call to send ussd
===========================================================================*/
typedef struct {
    boolean                             has_cc_type_modification;
    gstk_cc_req_action_type             cc_req_action;
    gstk_general_result_enum_type       command_result2;
    gstk_additional_info_type           additional_info2;
    gstk_additional_info_ptr_type       additional_info2_ptr;
    gstk_string_type                    text_string2;
}
gstk_call_ss_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_get_inkey_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a Get Inkey
      proactive command request

      user_get_inkey_input: user input
      duration_of_exec   :  duration of execution of the cmd
===========================================================================*/
typedef struct
{
    gstk_string_type   user_get_inkey_input;
    gstk_duration_type duration_of_exec;
}
gstk_get_inkey_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_ussd_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a Send USSD proactive
      command request

      has_cc_type_modification:  specified whether this struct is required
                                 or not in the terminal response, only
                                 required if the type of call has been changed
      data:  user inputs
      cc_req_action:  Required only if call type has been changed
      command_result2, additional_info2:  results of the new call type
                                          response from the network
===========================================================================*/
typedef struct {
    boolean                             has_cc_type_modification;
    gstk_string_type                    data;
    gstk_cc_req_action_type             cc_req_action;
    gstk_general_result_enum_type       command_result2;
    gstk_additional_info_type           additional_info2;
    gstk_additional_info_ptr_type       additional_info2_ptr;
}
gstk_ussd_rsp_extra_param_type;


/*===========================================================================
--  Struct: gstk_provide_lang_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a provid local information
      for language proactive command request

      lang_code:  specified the language code as defined in spec
===========================================================================*/
typedef struct {
   uint8                                lang_code[2];
}
gstk_provide_lang_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_open_ch_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a open channel proactive 
      command request
===========================================================================*/
typedef struct {
  gstk_ch_status_type                     ch_status;
  gstk_bearer_description_type            bearer_description;
  int32                                   buffer_size;
}
gstk_open_ch_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_receive_data_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a receive data proactive 
      command request
===========================================================================*/
typedef struct {
   gstk_ch_data_type                       ch_data;
   int32                                   ch_data_remaining_len;                 
}
gstk_receive_data_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_send_data_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a send data proactive 
      command request
===========================================================================*/
typedef struct {
   int32                                   ch_data_remain_buf_len;  
}
gstk_send_data_rsp_extra_param_type;

/*===========================================================================
--  Struct: gstk_get_ch_status_rsp_extra_param_type
      This structure contains information that are required as the extra
      parameter in sending a terminal response to a get channel status
      proactive command request
===========================================================================*/
typedef struct {
   int32                                  cnt;
   gstk_ch_status_type                    ch_status[GSTK_MAX_GPRS_CHANNELS];
}
gstk_get_ch_status_rsp_extra_param_type;



/*===========================================================================
--  Struct: gstk_terminal_rsp_extra_param_type
      This structure consists of a boolean to indicate whether there are any
      extra parameter required and a union of all the possible extra parameters that
      will be needed for the various proactive command terminal response that
      GSTK currently supports

      rsp_type: indicate which _CNF that the union should be looked at.  Valid
                values are:
                GSTK_GET_INKEY_CNF
                GSTK_GET_INPUT_CNF
                GSTK_POLL_INTERVAL_CNF            
                GSTK_SELECT_ITEM_CNF     
                GSTK_SEND_SS_CNF               
                GSTK_SEND_USSD_CNF            
                GSTK_SETUP_CALL_CNF               
                GSTK_PROVIDE_LANG_INFO_CNF              
                GSTK_OPEN_CH_CNF                   
                GSTK_SEND_DATA_CNF                 
                GSTK_RECEIVE_DATA_CNF              
                GSTK_GET_CH_STATUS_CNF            

      only the following proactive command requires extra param in the terminal
      response according standard:
        poll interval  - the actual polling interval that ME can use
        get inkey  - user input
        get input  - user input
        select item  - user selection
        provide local information  - local information based on what specific
                                     information is requested
        run at command - run AT response
        set up call  - only required if CC has changed the set up call to
                       ss or ussd
        send SS  - only required if CC has changed the ss to set up call
                   or ussd
        send USSD  - only required if CC has changed the ussd to set up
                     call or ss

      But since GSTK processes some of the commands "internally", clients
      will only need to provide extra params for
        get inkey,
        get input,
        select item,
        run at command (future)
        setup up call
        send SS
        send USSD

      local information is being handled in GSTK internally
      only 1 kind of local information can be requested from the SIM/USIM at
      a time since the information is being specified in the command detail,
      which is an exclusive OR for the various information

      run_at_response; (future )
      setup_call_extra_param, send_ss_extra_param, send_ussd_extra_param:
       the following extra param is needed when cc response changed the call
       type except in the gstk_ussd_rsp_extra_param_type, which include a
       mandatory ussd string type for the regular send USSD command
       most of the information in these structures are passed in from
       the GSTK upon CC responses, upon the client sends the new call
       request to the network and after receiving the response, it will
       require to send both this information plus the new call type's
       network response back to GSTK
===========================================================================*/
typedef struct {
    boolean                                  present;
    gstk_cmd_enum_type                       rsp_type;
    union {
      gstk_duration_type                            poll_duration;
      gstk_string_type                              user_get_input;
      gstk_get_inkey_rsp_extra_param_type           get_inkey_extra_param; /* get in key extension */
      uint8                                         select_item_chosen_item;
      gstk_call_ss_rsp_extra_param_type             setup_call_extra_param;
      gstk_call_ss_rsp_extra_param_type             send_ss_extra_param;
      gstk_ussd_rsp_extra_param_type                send_ussd_extra_param;
      gstk_provide_lang_rsp_extra_param_type        provide_lang_info_extra_param;
      gstk_battery_state_enum_type                  battery_status_info_extra_param;
      gstk_open_ch_rsp_extra_param_type             open_ch_extra_param;
      gstk_receive_data_rsp_extra_param_type        receive_data_extra_param;
      gstk_send_data_rsp_extra_param_type           send_data_extra_param;
      gstk_get_ch_status_rsp_extra_param_type       get_ch_status_extram_param;
  /*~ IF ( _DISC_ == GSTK_GET_INKEY_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.get_inkey_extra_param */
  /*~ IF ( _DISC_ == GSTK_GET_INPUT_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.user_get_input */
  /*~ IF ( _DISC_ == GSTK_POLL_INTERVAL_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.poll_duration */
  /*~ IF ( _DISC_ == GSTK_SELECT_ITEM_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.select_item_chosen_item */
  /*~ IF ( _DISC_ == GSTK_SEND_SS_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.send_ss_extra_param */
  /*~ IF ( _DISC_ == GSTK_SEND_USSD_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.send_ussd_extra_param */
  /*~ IF ( _DISC_ == GSTK_SETUP_CALL_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.setup_call_extra_param */
  /*~ IF ( _DISC_ == GSTK_PROVIDE_LANG_INFO_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.provide_lang_info_extra_param */
  /*~ IF ( _DISC_ == GSTK_PROVIDE_BATT_STATUS_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.battery_status_info_extra_param */
  /*~ IF ( _DISC_ == GSTK_OPEN_CH_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.open_ch_extra_param */
  /*~ IF ( _DISC_ == GSTK_SEND_DATA_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.send_data_extra_param */
  /*~ IF ( _DISC_ == GSTK_RECEIVE_DATA_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.receive_data_extra_param */
  /*~ IF ( _DISC_ == GSTK_GET_CH_STATUS_CNF ) gstk_terminal_rsp_extra_param_type.extra_param.get_ch_status_extram_param */
  /*~ DEFAULT                                gstk_terminal_rsp_extra_param_type.extra_param.void */
    }extra_param;
  /*~ FIELD gstk_terminal_rsp_extra_param_type.extra_param DISC gstk_terminal_rsp_extra_param_type.rsp_type */
}
gstk_terminal_rsp_extra_param_type;



/*---------------------------------------- Proactive Command related structures ------------------------------*/

/*
** Structure of proactive commands sent from SIM/USIM to the upper layers.
** _req_type is proactive cmds populated in the GSTK and sent to upper layers
*/

/*===========================================================================
--  Struct: gstk_display_text_req_type
      GSTK will parse the display text proactive command into this structure.

      user_control:  if TRUE, allow user to clear screen                           
===========================================================================*/
typedef struct {
    gstk_string_type                   text_string;
    boolean                            high_priority;
    boolean                            user_control;
    /* Optional */
    gstk_icon_type                     icon;
    boolean                            immediate_response_required;
    gstk_duration_type                 duration;
}
gstk_display_text_req_type;



/*===========================================================================
--  Struct: gstk_get_inkey_req_type
      GSTK will parse the get inkey proactive command into this structure.

      response_format: options are:
        YES_NO
        NUMERICAL_ONLY
        SMS_DEF_ALPHABET_UNPACKED
                 - doesn't matter whether it is packed or unpacked
                   in get inkey since there will only be 1 character
                   response
        UCS2_ALPHABET
===========================================================================*/
typedef struct {
    gstk_string_type                   text_string;
    gstk_input_format_enum_type        response_format;
    boolean                            help_available;
    /* Optional */
    gstk_icon_type                     icon;
    gstk_duration_type                 variable_timeout;
}
gstk_get_inkey_req_type;


/*===========================================================================
--  Struct: gstk_get_input_req_type
      GSTK will parse the get input proactive command into this structure.

      response_format:  options are:
              NUMERICAL_ONLY
              SMS_DEF_ALPHABET_UNPACKED
              UCS2_ALPHABET
      show_user_input:  If FALSE, ME can display '*' chars
===========================================================================*/
typedef struct {
    gstk_string_type                   text_string;
    gstk_input_format_enum_type        response_format;
    gstk_input_pack_format_enum_type   respone_packing_format;
    uint8                              maximum_user_input;
    uint8                              minimum_user_input;
    boolean                            help_available;
    boolean                            show_user_input;
    /* Optional */
    gstk_string_type                   default_text;
    gstk_icon_type                     icon;
}
gstk_get_input_req_type;




/*===========================================================================
--  Struct: gstk_launch_browser_req_type
      GSTK will parse the launch browser proactive command into this
      structure.

      url: If the length is zero, application is required to use default URL.
           The url is coded in SMS 7bit default alphabet with bit8 = 0
      browser_id:  0x00 = default browser, the only value supported in
                          standard now
===========================================================================*/
typedef struct {
    gstk_launch_mode_enum_type         launch_mode;
    gstk_url_type                      url;
    /* Optional */
    uint8                              browser_id;
    gstk_bearer_type                   bearer;
    uint32                             num_provisioning_files;
    gstk_file_list_type                provisioning_file_ref[GSTK_MAX_PROVISIONING_FILE_REF];
    gstk_string_type                   gateway_proxy_text_string;
    gstk_string_type                   user_confirm_alpha;
    gstk_icon_type                     user_confirm_icon;
}
gstk_launch_browser_req_type;





/*===========================================================================
--  Struct: gstk_play_tone_req_type
      GSTK will parse the Play tone proactive command into this structure.
===========================================================================*/
typedef struct {
    /* Optional */
    gstk_string_type                   alpha;
    gstk_tone_enum_type                tone;
    gstk_duration_type                 duration;
    gstk_icon_type                     icon;
}
gstk_play_tone_req_type;








/*===========================================================================
--  Struct: gstk_run_at_cmd_req_type
      GSTK will parse the run AT command proactive command into this structure.
===========================================================================*/
typedef struct {
    gstk_at_cmd_type                   at_cmd;
    /* Optional */
    gstk_string_type                   alpha;
    gstk_icon_type                     icon;
}
gstk_run_at_cmd_req_type;




/*===========================================================================
--  Struct: gstk_select_item_req_type
      GSTK will parse the Select item proactive command into this structure.
===========================================================================*/
typedef struct {
    boolean                            help_available;
    gstk_menu_present_enum_type        presentation;
    boolean                            softkey_selection;
    uint8                              num_items;
    gstk_item_object_type              items[GSTK_MAX_MENU_ITEMS];
    /* Optional */
    gstk_string_type                   title;
    uint8                              default_item;  /* optional if value set to 0xFF -> okay???? */
    gstk_next_item_ind_type            next_action_indicator;
    gstk_icon_type                     icon;
    gstk_icon_id_list_type             icon_id_list;
}
gstk_select_item_req_type;



/*===========================================================================
--  Struct: gstk_setup_menu_req_type
      GSTK will parse the Set up menu proactive command into this structure.
===========================================================================*/
typedef struct {
    boolean                            help_available;
    boolean                            softkey_selection;
    uint8                              num_items;
    gstk_item_object_type              items[GSTK_MAX_MENU_ITEMS];
    /* Optional */
    gstk_string_type                   title;
    uint8                              default_item;  /* optional if value set to 0xFF -> okay???? */
    gstk_next_item_ind_type            next_action_indicator;
    gstk_icon_type                     icon;
    gstk_icon_id_list_type             icon_id_list;
}
gstk_setup_menu_req_type;



/*===========================================================================
--  Struct: gstk_setup_call_req_type
      GSTK will parse the Set up call proactive command into this structure.

      call_setup_requirement:  hold_active_calls,
                               terminate_active_calls,
                               no_other_calls are exclusive
      redial:  redial required or not is mandatory redial duration
               is optional
===========================================================================*/
typedef struct {
    gstk_call_setup_req_enum_type      call_setup_requirement;
    gstk_address_type                  address;
    gstk_redial_type                   redial;
    /* Optional */
    gstk_string_type                   user_cnf_alpha;
    gstk_string_type                   setup_call_display_alpha;
    gstk_icon_type                     user_cnf_icon;
    gstk_icon_type                     setup_call_display_icon;
    gstk_subaddress_type               destination_subaddress;
    gstk_cap_config_type               capability_config;
}
gstk_setup_call_req_type;

/*===========================================================================
--  Struct: gstk_setup_idle_mode_text_req_type
      GSTK will parse the Set up idle mode text proactive command into
      this structure.
===========================================================================*/
typedef struct {
    gstk_string_type                   text_string;
    /* Optional */
    gstk_icon_type                     icon;
}
gstk_setup_idle_mode_text_req_type;

/*===========================================================================
--  Struct: gstk_send_ss_req_type
      GSTK will parse the Send SS proactive command into this structure.
===========================================================================*/
typedef struct {
    gstk_address_type                  ss_string;
    /* Optional */
    gstk_string_type                   alpha;
    gstk_icon_type                     icon;
}
gstk_send_ss_req_type;


/*===========================================================================
--  Struct: gstk_send_ussd_req_type
      GSTK will parse the Send USSD proactive command into this structure.
===========================================================================*/
typedef struct {
    gstk_ussd_string_type              ussd_string;
    /* Optional */
    gstk_string_type                   alpha;
    gstk_icon_type                     icon;
}
gstk_send_ussd_req_type;


/*===========================================================================
--  Struct: gstk_send_sms_req_type
      GSTK will parse the Send SMS proactive command into this structure.
===========================================================================*/
typedef struct {
    boolean                            packing_required;
    gstk_sms_tpdu_type                 sms_tpdu;
    boolean                            is_cdma_sms;
    /* Optional */
    gstk_address_type                  address;
    gstk_string_type                   alpha;
    gstk_icon_type                     icon;
}
gstk_send_sms_req_type;



typedef enum
{
  GSTK_BROWSER_USER_TERMINATED    = 0x00,
  GSTK_BROWSER_ERROR_TERMINATED   = 0x01,
  GSTK_BROWSER_TERM_CAUSE_MAX     = 0xFFFF
}
gstk_browser_term_cause_enum_type;


typedef struct 
{
    uint8                              lang_code[GSTK_LANG_CODE_LEN];
}
gstk_lang_type;

/*===========================================================================
--  Struct: gstk_dtmf_type
      length - indicates the length of the dtmf field
      dtmf - indicates the DTMF string in BCD format
===========================================================================*/
typedef struct {
    uint8                              length;
    uint8                              *dtmf;
  /*~ FIELD gstk_dtmf_type.dtmf VARRAY LENGTH gstk_dtmf_type.length */
}
gstk_dtmf_type;

/*===========================================================================
--  Struct: gstk_send_dtmf_req_type
      GSTK will parse the Send DTMF proactive command into this structure.
      gstk_dtmf_type defines in BCD format
===========================================================================*/
typedef struct {
    gstk_dtmf_type                     dtmf; /* in BCD format */ 
    gstk_string_type                   alpha;
    gstk_icon_type                     icon;
}
gstk_send_dtmf_req_type;

/*===========================================================================
--  Struct: gstk_lang_notification_req_type
      GSTK will parse the Language Notification proactive command into 
      this structure.
      Mobile should still send COMMAND_SUCCESSFUL even if the lang_code 
      is not understood by the mobile
===========================================================================*/
typedef struct {
    boolean                            specific_lang_notification;
    gstk_lang_type                     lang;
}
gstk_lang_notification_req_type;

typedef struct {
    boolean                            require_notification;
}
gstk_evt_notify_type;



/*===========================================================================
--  Struct: gstk_open_ch_req_type
      GSTK will parse the Open Channel proactive command into 
      this structure.
===========================================================================*/
typedef struct {
    boolean                            on_demand_link_est;
    gstk_alphabet_type                 alpha;
    gstk_icon_type                     icon;
    gstk_bearer_description_type       bearer;
    int32                              buffer_size;
    gstk_alphabet_type                 access_point_name;
    gstk_pkt_data_address_type         local_address;
    gstk_alphabet_type                 text_string_user_login;
    gstk_alphabet_type                 text_string_user_passwd;
    gstk_pkt_data_transport_level_type transport_level;
    gstk_pkt_data_address_type         data_dest_address;
}
gstk_open_ch_req_type;

/*===========================================================================
--  Struct: gstk_close_ch_req_type
      GSTK will parse the Close Channel proactive command into 
      this structure.
===========================================================================*/
typedef struct {
    int32                              ch_id;
    gstk_alphabet_type                 alpha;
    gstk_icon_type                     icon;
}
gstk_close_ch_req_type;

/*===========================================================================
--  Struct: gstk_receive_data_req_type
      GSTK will parse the Receive Data proactive command into 
      this structure.
===========================================================================*/
typedef struct {
    int32                              ch_id;
    int32                              ch_data_length;
    gstk_alphabet_type                 alpha;
    gstk_icon_type                     icon;
}
gstk_receive_data_req_type;

/*===========================================================================
--  Struct: gstk_send_data_req_type
      GSTK will parse the Send Data proactive command into 
      this structure.
===========================================================================*/
typedef struct {
    int32                              ch_id;
    boolean                            send_data_immediately;
    gstk_ch_data_type                  ch_data;
    gstk_alphabet_type                 alpha;
    gstk_icon_type                     icon;
}
gstk_send_data_req_type;


typedef struct {
    int32                              ch_id;
}
gstk_get_ch_status_req_type;



/* ----------------------------------- Envelope command type structures, from ME to CARD ------------------------------*/

/* Used in the various send envelope functions */


/*===========================================================================
--  Struct: gstk_mt_call_evt_ind_type
      This structure includes info that the clients are required to supply
      in MT Call event download.

      transaction_id:  1 transaction id in the SETUP message from
                       the network
      address:  include when Calling Party BCD number is received
                by ME in the SETUP message
      subaddress:  include when Calling Party Subaddress is received by
                   ME in the SETUP message
===========================================================================*/
typedef struct {
    gstk_transaction_id_type           transaction_id;
    /* C */
    gstk_address_type                  address;
    gstk_subaddress_type               subaddress;
}
gstk_mt_call_evt_ind_type;


/*===========================================================================
--  Struct: gstk_call_connected_evt_ind_type
      This structure includes info that the clients are required to supply
      in Call Connected event download.

      transaction_id:  1 transaction id in the CONNECT message
===========================================================================*/
typedef struct {
    gstk_transaction_id_type           transaction_id;
}
gstk_call_connected_evt_ind_type;


/*===========================================================================
--  Struct: gstk_call_disconnected_evt_ind_type
      This structure includes info that the clients are required to supply
      in Call Disconnected event download.

      transaction_id:  list of transaction ids for all the calls being
                       disconnected
      cause:   - reflect the CC-Cause information elemenet sent or received
                 in the DISCONNECT, RELEASE or RELEASE COMPLETE message
               - in case of radio link timeout, the cause data object shall
                 be included, with zero length value
===========================================================================*/
typedef struct {
    gstk_transaction_id_type           transaction_id;
    /* Optional */
    gstk_cause_type                    cause;
}
gstk_call_disconnected_evt_ind_type;



/*===========================================================================
--  Struct: gstk_evt_dl_ind_type
      This structure includes info that the clients are required to supply
      in Event Download Envelope Command.
      Valid evt_type values for this structure are:
       GSTK_MT_CALL_EVT_IND            (evt_ind.mt_call_evt_ind)
       GSTK_CALL_CONNECTED_EVT_IND     (evt_ind.call_connected_evt_ind)
       GSTK_CALL_DISCONNECTED_EVT_IND  (evt_ind.call_disconnected_evt_ind)
       GSTK_IDLE_SCRN_EVT_IND          
       GSTK_USER_ACTIVITY_EVT_IND      
       GSTK_LANG_SELECTION_EVT_IND     (evt_ind.lang_sel_evt_ind)
       GSTK_BROWSER_TERMINATE_EVT_IND  (evt_ind.browser_terminate_evt_ind)
       GSTK_DATA_AVAIL_EVT_IND         (evt_ind.data_avail_evt_ind)
       GSTK_CH_STATUS_EVT_IND          (evt_ind.ch_status_evt_ind)

       GSTK_LOCATION_STATUS_EVT_IND is internal to GSTK
       there is no specific structures that the clients need to fill in case
       of idle screen and user activity evt indications, GSTK can base
       on the gstk_cmd_enum_type to determine what event is being sent
       to the card
===========================================================================*/
typedef struct {
    gstk_cmd_enum_type                        evt_type;
    union {
        gstk_mt_call_evt_ind_type               mt_call_evt_ind;
        gstk_call_connected_evt_ind_type        call_connected_evt_ind;
        gstk_call_disconnected_evt_ind_type     call_disconnected_evt_ind;
        gstk_lang_type                          lang_sel_evt_ind;
        gstk_browser_term_cause_enum_type       browser_terminate_evt_ind;
        gstk_data_avail_evt_ind_type            data_avail_evt_ind;
        gstk_ch_status_type                     ch_status_evt_ind;
        /*~ PARTIAL evt_ind */
        /*~ IF ( _DISC_ == GSTK_MT_CALL_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.mt_call_evt_ind */
        /*~ IF ( _DISC_ == GSTK_CALL_CONNECTED_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.call_connected_evt_ind */
        /*~ IF ( _DISC_ == GSTK_CALL_DISCONNECTED_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.call_disconnected_evt_ind */
        /*~ IF ( _DISC_ == GSTK_LANG_SELECTION_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.lang_sel_evt_ind */
        /*~ IF ( _DISC_ == GSTK_BROWSER_TERMINATION_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.browser_terminate_evt_ind */
        /*~ IF ( _DISC_ == GSTK_DATA_AVAIL_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.data_avail_evt_ind */
        /*~ IF ( _DISC_ == GSTK_CH_STATUS_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.ch_status_evt_ind */
        /*~ IF ( _DISC_ == GSTK_USER_ACTIVITY_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.void */
        /*~ IF ( _DISC_ == GSTK_IDLE_SCRN_EVT_IND ) gstk_evt_dl_ind_type.evt_ind.void */
        /*~ DEFAULT                                 gstk_evt_dl_ind_type.evt_ind.void */
        /* location info indication is being handled within GSTK                  */
    }evt_ind;
/*~ FIELD gstk_evt_dl_ind_type.evt_ind DISC gstk_evt_dl_ind_type.evt_type */
}
gstk_evt_dl_ind_type;



/*===========================================================================
--  Struct: gstk_mo_sms_ctrl_ind_type
      This structure includes info that the clients are required to supply
      in SMS MO Control Command.

      rp_dest_address:  RP_Destination_Address of service center where
                        ME send the message to
      tp_dest_address:  TP_Destination_Address where ME supposed to
                        send the message to
      Mandatory location information will be populated in GSTK
===========================================================================*/
typedef struct {
    gstk_address_type                  rp_dest_address;
    gstk_address_type                  tp_dest_address;
}
gstk_mo_sms_ctrl_ind_type;


/*===========================================================================
--  Struct: gstk_cc_ind_type
      This structure includes info that the clients are required to supply
      in Call Control Command.

      call_enum_type:  GSTK_VOICE, GSTK_SS, GSTK_USSD
      union {
        voice:  set up call
        ss_string:  send ss
        ussd_string:  send ussd
      }
      Mandatory location information will be populated in GSTK
===========================================================================*/
typedef struct {
    gstk_call_enum_type                call_enum_type;
    union {
        gstk_cc_voice_type               voice;
        gstk_address_type                ss_string;
        gstk_ussd_string_type            ussd_string;
        gstk_pdp_context_act_param_type  pdp_context;
  /*~ IF ( _DISC_ == GSTK_VOICE || _DISC_ == GSTK_VOICE_PTR ) gstk_cc_ind_type.gstk_address_string.voice */
  /*~ IF ( _DISC_ == GSTK_SS    || _DISC_ == GSTK_SS_PTR    ) gstk_cc_ind_type.gstk_address_string.ss_string */
  /*~ IF ( _DISC_ == GSTK_USSD  || _DISC_ == GSTK_USSD_PTR  ) gstk_cc_ind_type.gstk_address_string.ussd_string */
  /*~ IF ( _DISC_ == GSTK_PDP_CONTEXT_ACT )                   gstk_cc_ind_type.gstk_address_string.pdp_context */
  /*~ DEFAULT                                                 gstk_cc_ind_type.gstk_address_string.void */
    }gstk_address_string;
  /*~ FIELD gstk_cc_ind_type.gstk_address_string DISC gstk_cc_ind_type.call_enum_type */
}
gstk_cc_ind_type;




/* ----------------------------------- Envelope command responses type structures ------------------------------*/

/* _rsp_type is envelope response from the card to a previously ME issued envelope cmd
*/




/*===========================================================================
--  Struct: gstk_sms_pp_download_ack_type
      This structure includes information returned from Card in response to
      SMS PP Download.
      This struct is used only when Card response acknowledge the SMS PP
      download with certain ACK data.

      ack:  Based on Standard, max of 128
===========================================================================*/
typedef struct {
    uint8                              length;
    uint8                              *ack;
  /*~ FIELD gstk_sms_pp_download_ack_type.ack VARRAY LENGTH gstk_sms_pp_download_ack_type.length */
}
gstk_sms_pp_download_ack_type;




/*===========================================================================
--  Struct: gstk_sms_pp_download_rsp_type
      This structure includes information returned from Card upon receiving
      SMS PP download command.
      the response of the sms PP download envelope command might not
      result in data response from SIM/USIM;
      if SIM/USIM do return a data response, the data info will be presented
      in the form of gstk_sms_pp_download_ack_type
      else if there is no data response, GSTK will indicate the success/failure
      SMS PP Download command response as gstk_general_envelope_rsp_enum_type

      general_resp: indicates the SUCCESS/FAILURE of the sms pp download rsp
      response_data_present:  TRUE if card return rsp data to the sms
                              pp envelope cmd

      when response_data_present == TRUE, client needs to look at the
      gstk_sms_pp_download_ack_type,
===========================================================================*/
typedef struct {
    gstk_general_envelope_rsp_enum_type general_resp;
    boolean                             response_data_present;
    gstk_sms_pp_download_ack_type       ack;
}
gstk_sms_pp_download_rsp_type;

/*===========================================================================
--  Struct: gstk_raw_envelope_rsp_type
      This structure includes information returned from Card upon receiving
      raw envelope command.
      The response of the raw envelope command might not
      result in data response from SIM/USIM.
      If SIM/USIM do return a data response, the data info will be presented
      in rsp_data_ptr.
      There won't be payload returned from the envelope response from the 
      card if it is a response for one of the "event download" envelopes.
      
      general_resp: indicates the SUCCESS/FAILURE of the raw envelope response
      length;       length of rsp_data_ptr
      rsp_data_ptr: pointer to the response data
      sw1         : value of SW1 of the response
                    refer to uim_sw1_type in uim.h for all valid values
      sw2         : value of SW2 of the response
                    refer to uim_sw2_type in uim.h for all valid values
===========================================================================*/
typedef struct {
    gstk_general_envelope_rsp_enum_type general_resp;
    uint32                              length;
    uint8                               *rsp_data_ptr;
    uint8                               sw1;
    uint8                               sw2;
  /*~ FIELD gstk_raw_envelope_rsp_type.rsp_data_ptr VARRAY LENGTH gstk_raw_envelope_rsp_type.length */
} gstk_raw_envelope_rsp_type;

/*===========================================================================
--  Struct: gstk_term_rsp_sw1_sw2_rpt_type
      This structure SW1 and SW2 information of terminal response

    sw1: The value of SW1 (refer to uim_sw1_type in uim.h for all valid values)
    sw2: The value of SW2 (refer to uim_sw1_type in uim.h for all valid values)
===========================================================================*/
typedef struct {
  uint8 sw1;
  uint8 sw2;
} gstk_term_rsp_sw1_sw2_rpt_type;

/*===========================================================================
--  Struct: gstk_mo_sms_ctrl_rsp_type
      This structure includes information returned from Card upon receiving
      MO SMS Control request.

      general_resp - Indicates whether the envelope command is success or failure

      rp_dest_address:  RP_Destination_Address of Service Center
                         if not present => no modification

      tp_dest_address:  TP_Destination_Address
                          if not present => no modification
===========================================================================*/
typedef struct {
    gstk_general_envelope_rsp_enum_type general_resp; /* ENVELOPE SUCCESS or FAIL */
    gstk_control_result_enum_type       mo_sms_ctrl_result;
    /* Optional */
    gstk_address_type                   rp_dest_address;
    gstk_address_type                   tp_dest_address;
    gstk_string_type                    alpha;
}
gstk_mo_sms_ctrl_rsp_type;


/*===========================================================================
--  Struct: gstk_cc_rsp_type
      This structure includes information returned from Card upon receiving
      Call Control request.

      general_resp - Indicates whether the envelope command is success or failure
      call_ctrl_result:  based on which information is available, ME is
                         required to update those information accordingly
                         when the call_ctrl_result == GSTK_ALLOWED_BUT_MOD
===========================================================================*/
typedef struct {
    gstk_general_envelope_rsp_enum_type general_resp; /* ENVELOPE SUCCESS or FAIL */
    gstk_control_result_enum_type       call_ctrl_result;
    /* Optional */
    gstk_cc_req_action_type             cc_req_action;
}
gstk_cc_rsp_type;


/*===========================================================================
--  Struct: gstk_end_proactive_req_type
      This structure indicates if the End Proactive Session is generated by
      GSTK because there is no more Fetch command or if it is from the card
===========================================================================*/
typedef struct {
    boolean                            end_from_card; /* Proactive Session Ended
                                                         due to card */
}
gstk_end_proactive_req_type;


/*===========================================================================
--  Struct: gstk_cmd_from_card_type
      This structure includes union of all the possible commands to be
      received from the card through GSTK.
      hdr_cmd contains information that will allow the correct casting of the
      union member.
      polling interval, refresh, set up event list are handled in GSTK
      internally

      envelope_general_rsp: used for setup evt download (except
                   location status event)
                   and menu selection envelope rsp
===========================================================================*/
typedef struct {
    gstk_exp_hdr_type                             hdr_cmd;
    union {
        /*
        ** GSTK_PROACTIVE_CMD_REQ message
        */
        gstk_display_text_req_type              display_text_pro_cmd_req;
        gstk_get_inkey_req_type                 get_inkey_pro_cmd_req;
        gstk_get_input_req_type                 get_input_pro_cmd_req;
        gstk_launch_browser_req_type            launch_browser_pro_cmd_req;
        /* more time has only hdr info */
        gstk_play_tone_req_type                 play_tone_pro_cmd_req;
        gstk_run_at_cmd_req_type                run_at_cmd_pro_cmd_req;
        gstk_select_item_req_type               select_item_pro_cmd_req;
        gstk_send_sms_req_type                  send_sms_pro_cmd_req;
        gstk_send_ss_req_type                   send_ss_pro_cmd_req;
        gstk_send_ussd_req_type                 send_ussd_pro_cmd_req;
        gstk_setup_call_req_type                setup_call_pro_cmd_req;
        gstk_setup_idle_mode_text_req_type      setup_idle_mode_txt_pro_cmd_req;
        gstk_setup_menu_req_type                setup_menu_pro_cmd_req;
        gstk_send_dtmf_req_type                 send_dtmf_pro_cmd_req;
        gstk_lang_notification_req_type         lang_notification_pro_cmd_req;
        gstk_end_proactive_req_type             end_pro_cmd_req;
        gstk_open_ch_req_type                   open_ch_pro_cmd_req;
        gstk_close_ch_req_type                  close_ch_pro_cmd_req;
        gstk_receive_data_req_type              receive_data_pro_cmd_req;
        gstk_send_data_req_type                 send_data_pro_cmd_req;
        gstk_get_ch_status_req_type             get_ch_status_pro_cmd_req;

        /* polling interval, polling off, refresh,
           set up event list are handled in GSTK internally */

        /*
        ** As a result of setup event list
        ** Idle Screen, User Activity, Browser Termination and Language Select
        ** will use this structure to notify the client for upcoming envelope
        ** command event download procedures
        */
        gstk_evt_notify_type                    evt_notify_req;
        /*
        ** GSTK_ENVELOPE_RSP message
        */
        gstk_sms_pp_download_rsp_type           sms_pp_download_rsp;
        gstk_mo_sms_ctrl_rsp_type               mo_sms_cc_rsp;
        gstk_cc_rsp_type                        cc_rsp;
        gstk_general_envelope_rsp_enum_type     envelope_general_rsp;  /* used for setup evt download (except
                                                                        * location status event)
                                                                        * and menu selection envelope rsp */
        gstk_raw_envelope_rsp_type              raw_envelope_rsp;  

        /* GSTK Terminal Profile Response message
        */
        gstk_terminal_profile_rsp_enum_type     terminal_profile_rsp;

        /* GSTK terminal response SW1/SW2 notification message */
        gstk_term_rsp_sw1_sw2_rpt_type          term_rsp_sw1_sw2_rpt;

  /*~ CASE GSTK_DISPLAY_TEXT_REQ       gstk_cmd_from_card_type.cmd.display_text_pro_cmd_req */
  /*~ CASE GSTK_GET_INKEY_REQ          gstk_cmd_from_card_type.cmd.get_inkey_pro_cmd_req    */
  /*~ CASE GSTK_GET_INPUT_REQ          gstk_cmd_from_card_type.cmd.get_input_pro_cmd_req    */
  /*~ CASE GSTK_LAUNCH_BROWSER_REQ     gstk_cmd_from_card_type.cmd.launch_browser_pro_cmd_req */
  /*~ CASE GSTK_MORE_TIME_REQ          gstk_cmd_from_card_type.cmd.void                     */
  /*~ CASE GSTK_PLAY_TONE_REQ          gstk_cmd_from_card_type.cmd.play_tone_pro_cmd_req    */
  /*~ CASE GSTK_POLL_INTERVAL_REQ      gstk_cmd_from_card_type.cmd.void                    */
  /*~ CASE GSTK_PROVIDE_LOCAL_INFO_REQ gstk_cmd_from_card_type.cmd.void               */
  /*~ CASE GSTK_REFRESH_REQ            gstk_cmd_from_card_type.cmd.void                     */
  /*~ CASE GSTK_RUN_AT_CMDS_REQ        gstk_cmd_from_card_type.cmd.run_at_cmd_pro_cmd_req   */
  /*~ CASE GSTK_SELECT_ITEM_REQ        gstk_cmd_from_card_type.cmd.select_item_pro_cmd_req  */
  /*~ CASE GSTK_SEND_SMS_REQ           gstk_cmd_from_card_type.cmd.send_sms_pro_cmd_req     */
  /*~ CASE GSTK_SEND_SS_REQ            gstk_cmd_from_card_type.cmd.send_ss_pro_cmd_req      */
  /*~ CASE GSTK_SEND_USSD_REQ          gstk_cmd_from_card_type.cmd.send_ussd_pro_cmd_req    */
  /*~ CASE GSTK_SETUP_CALL_REQ         gstk_cmd_from_card_type.cmd.setup_call_pro_cmd_req   */
  /*~ CASE GSTK_SETUP_EVENT_LIST_REQ   gstk_cmd_from_card_type.cmd.void                 */
  /*~ CASE GSTK_SETUP_IDLE_TEXT_REQ    gstk_cmd_from_card_type.cmd.setup_idle_mode_txt_pro_cmd_req */
  /*~ CASE GSTK_POLLING_OFF_REQ        gstk_cmd_from_card_type.cmd.void                     */
  /*~ CASE GSTK_SETUP_MENU_REQ         gstk_cmd_from_card_type.cmd.setup_menu_pro_cmd_req   */
  /*~ CASE GSTK_SEND_DTMF_REQ          gstk_cmd_from_card_type.cmd.send_dtmf_pro_cmd_req    */
  /*~ CASE GSTK_LANG_NOTIFICATION_REQ  gstk_cmd_from_card_type.cmd.lang_notification_pro_cmd_req */
  /*~ CASE GSTK_TIMER_MANAGEMENT_REQ   gstk_cmd_from_card_type.cmd.void                 */
  /*~ CASE GSTK_END_PROACTIVE_CMD_REQ  gstk_cmd_from_card_type.cmd.end_pro_cmd_req     */
  /*~ CASE GSTK_TOOLKIT_MODE_CHANGE_REQ gstk_cmd_from_card_type.cmd.void              */
  /*~ CASE GSTK_PROVIDE_LANG_INFO_REQ  gstk_cmd_from_card_type.cmd.void                */
  /*~ CASE GSTK_OPEN_CH_REQ            gstk_cmd_from_card_type.cmd.open_ch_pro_cmd_req      */
  /*~ CASE GSTK_CLOSE_CH_REQ           gstk_cmd_from_card_type.cmd.close_ch_pro_cmd_req     */
  /*~ CASE GSTK_RECEIVE_DATA_REQ       gstk_cmd_from_card_type.cmd.receive_data_pro_cmd_req */
  /*~ CASE GSTK_SEND_DATA_REQ          gstk_cmd_from_card_type.cmd.send_data_pro_cmd_req    */
  /*~ CASE GSTK_GET_CH_STATUS_REQ      gstk_cmd_from_card_type.cmd.get_ch_status_pro_cmd_req */

  /*~ CASE GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY  gstk_cmd_from_card_type.cmd.evt_notify_req */
  /*~ CASE GSTK_SETUP_EVT_USER_ACT_NOTIFY     gstk_cmd_from_card_type.cmd.evt_notify_req */
  /*~ CASE GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY gstk_cmd_from_card_type.cmd.evt_notify_req */
  /*~ CASE GSTK_SETUP_EVT_LANG_SEL_NOTIFY     gstk_cmd_from_card_type.cmd.evt_notify_req */
  /*~ CASE GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY   gstk_cmd_from_card_type.cmd.evt_notify_req */
  /*~ CASE GSTK_SETUP_EVT_CH_STATUS_NOTIFY    gstk_cmd_from_card_type.cmd.evt_notify_req */

  /*~ CASE GSTK_SMS_PP_DOWNLOAD_IND_RSP           gstk_cmd_from_card_type.cmd.sms_pp_download_rsp */
  /*~ CASE GSTK_MO_SMS_CTRL_IND_RSP               gstk_cmd_from_card_type.cmd.mo_sms_cc_rsp       */
  /*~ CASE GSTK_CC_IND_RSP                        gstk_cmd_from_card_type.cmd.cc_rsp              */
  /*~ CASE GSTK_PROFILE_DL_IND_RSP                gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_MENU_IND_RSP                      gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_MT_CALL_EVT_IND_RSP               gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_CALL_CONNECTED_EVT_IND_RSP        gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_CALL_DISCONNECTED_EVT_IND_RSp     gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_IDLE_SCRN_EVT_IND_RSP             gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_LOCATION_STATUS_EVT_IND_RSP       gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_USER_ACTIVITY_EVT_IND_RSP         gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_TIMER_EXPIRE_IND_RSP              gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_LANG_SELECTION_EVT_IND_RSP        gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_BROWSER_TERMINATION_EVT_IND_RSP   gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP   gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_DATA_AVAIL_EVT_IND_RSP            gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_CH_STATUS_EVT_IND_RSP             gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_ACCESS_TECH_EVT_IND_RSP           gstk_cmd_from_card_type.cmd.envelope_general_rsp*/
  /*~ CASE GSTK_RAW_ENVELOPE_RSP                  gstk_cmd_from_card_type.cmd.raw_envelope_rsp*/
  /*~ CASE GSTK_TERM_RSP_SW1_SW2_RPT              gstk_cmd_from_card_type.cmd.term_rsp_sw1_sw2_rpt*/
  /*~ DEFAULT  gstk_cmd_from_card_type.cmd.terminal_profile_rsp */
    }cmd;
/*~ FIELD gstk_cmd_from_card_type.cmd DISC (_OBJ_.hdr_cmd.command_id) */
}
gstk_cmd_from_card_type;

/*===========================================================================
-- Struct for GSTK Generic Data type.
===========================================================================*/
typedef struct
{
    uint8  *data_buffer_ptr;
    int32   data_len;
    /*~ FIELD gstk_generic_data_type.data_buffer_ptr VARRAY LENGTH gstk_generic_data_type.data_len */
}
gstk_generic_data_type;

/*===========================================================================
-- Struct for a defining a set of commands belonging to a class.
===========================================================================*/
typedef struct 
{
  /* BIT MASK identifying a class of proactive commands.
     Valid values are
     GSTK_REG_CATEGORY_0_CMD
     GSTK_REG_CATEGORY_B_CMD
     GSTK_REG_CATEGORY_C_CMD
     GSTK_REG_CATEGORY_D_CMD
     GSTK_REG_CATEGORY_E_CMD */
  uint32                               class_bit_mask;
  /* BIT MASK identifying a single proactive command belonging to the 
     above mentioned class -- Refer spec 11.14, 31.111, 102 223 Annex A */
  uint64                               cmd_reg_bit_mask;
}gstk_toolkit_class_cmd_reg_info_type;

/*===========================================================================
-- Struct for use in client registration. This struct will help client specify
   what functionality the client is interested in processing.
===========================================================================*/
typedef struct 
{
  /* BIT MASK identifying a set of proactive command belonging to 
  a specific class*/
  gstk_toolkit_class_cmd_reg_info_type reg_set;
  /* UI/MODEM/ALL */
  gstk_tk_functionality_enum_type      client_func_type;
}gstk_toolkit_cmd_reg_info_type;

/*===========================================================================
-- Struct for raw bytes header .
   This structure denotes the header for the raw proactive commands that a 
   client has registered for
===========================================================================*/
typedef struct
{
  uint32                    cmd_ref_id;
  gstk_client_ref_data_type user_data;
}gstk_raw_bytes_hdr_type;

/*===========================================================================
-- Struct for callback function parameter.
   This structure is used by GSTK to send the information regarding the 
   registered proactive commands bytes in the RAW format as the client had 
   requested.
===========================================================================*/
typedef struct
{
  gstk_raw_bytes_hdr_type   raw_hdr;
  gstk_generic_data_type    payload;
}gstk_raw_cmd_from_card_type;

/*===========================================================================
-- Struct for callback function parameter.
   This structure is used by GSTK to send the information regarding the 
   registered proactive commands bytes in the format the client had requested.
===========================================================================*/
typedef struct 
{
  /* RAW/GSTK format */
  gstk_client_pro_cmd_data_format_enum_type data_format_type; 
  union
  {
    /* Callback parameter to receive command in the raw format */
    gstk_raw_cmd_from_card_type  *raw_req_rsp_ptr;
    /* Callback parameter to receive command in the gstk format */
    gstk_cmd_from_card_type      *gstk_req_rsp_ptr;
/*~ CASE GSTK_RAW_FORMAT        gstk_client_pro_cmd_reg_data_type.pro_cmd_reg_data.raw_req_rsp_ptr */
/*~ CASE GSTK_GSTK_FORMAT       gstk_client_pro_cmd_reg_data_type.pro_cmd_reg_data.gstk_req_rsp_ptr */
  }pro_cmd_reg_data;
/*~ FIELD gstk_client_pro_cmd_reg_data_type.pro_cmd_reg_data DISC (_OBJ_.data_format_type) */
/*~ FIELD gstk_client_pro_cmd_reg_data_type.pro_cmd_reg_data.raw_req_rsp_ptr POINTER       */
/*~ FIELD gstk_client_pro_cmd_reg_data_type.pro_cmd_reg_data.gstk_req_rsp_ptr POINTER      */
} gstk_client_pro_cmd_reg_data_type;

/*===========================================================================
--  Definition for callback function.
      Upon receiving the events, the client is required to copy the command
      into its buffer/queue ASAP
      The length information is encoded in the gstk_exp_hdr_type
      and the cb function should take up relatively short execution time
      since it is still running in GSTK context
      In processing the gstk_cmd_from_card_type eventually,
      client can first based on the gstk_cmd_enum_type inside the
      gstk_cmd_from_card_type, gstk_exp_hdr_type structure to cast the command
      into the corresponding structure for further processing.
      There is no return value for this callback function protocol.
===========================================================================*/
typedef void (*gstk_evt_cb_funct_type) (
        gstk_cmd_from_card_type *         req_rsp );
/*~ PARAM IN req_rsp POINTER */


/*===========================================================================
--  Definition for client callback function for proactive cmd registration
      This callback can be used by GSTK to send proactive commands to the
      registered client in raw format or GSTK format. This 
      callback differs from "gstk_evt_cb_funct_type" in the sense that 
      this GSTK uses this to notify clients of concerned proactive commands
      in the format that the client wants. The old callback has support for
      only the GSTK format whereas this callback has support for the GSTK 
      format and the raw format. 
      When this callback is invoked in the GSTK context, the client is 
      required to copy the command into its buffer/queue ASAP
      For GSTK format - client should follow same parsing instructions
      as for "gstk_evt_cb_funct_type" 
      For Raw Format - client will get the proactive command information in
      the raw byte format. 
      With either of these formats, client should copy the information
      and let the callback return immediately since the callback is called
      in the GSTK context. 
===========================================================================*/
typedef void (*gstk_toolkit_evt_cb_func_type)( 
	gstk_client_pro_cmd_reg_data_type *req_rsp_ptr);
/*~ PARAM IN req_rsp_ptr POINTER */

/*===========================================================================
-- Definition for register callback function.
   This client callback is invoked when GSTK wants to report the status of 
   registration to the client. The registration status indicated will be 
   applicable to the whole set of commands. If a particular command in the 
   client set failed to register, GSTK would not perform registration
   for any of the commands in the request and will return failure to the 
   client.
===========================================================================*/
typedef void (*gstk_reg_rsp_cmd_cb_func_type)(
        gstk_status_enum_type             gstk_reg_status,
        gstk_client_ref_data_type         user_data);

/*===========================================================================
-- Definition for client init callback function.
   This client callback is invoked when GSTK wants to report the status of 
   registration to the client. 
===========================================================================*/
typedef void (*gstk_client_init_cb_func_type)(
        gstk_status_enum_type             gstk_reg_status,
        gstk_client_id_type               client_id,
        gstk_client_ref_data_type         user_data);

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
    GSTK_NULL_INPUT_PARAM,

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_client_init (
        gstk_client_id_type *client_id,
        gstk_client_type    client_type);
/*~ FUNCTION gstk_client_init */
/*~ PARAM OUT client_id POINTER */

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
extern gstk_status_enum_type gstk_client_init_ext (
  gstk_client_type               client_type,
  gstk_client_init_cb_func_type  response_cb,
  gstk_client_ref_data_type      client_ref);
/*~ FUNCTION gstk_client_init_ext RELEASE_FUNC gstk_client_id_force_cleanup(response_cb) */
/*~ ONERROR return GSTK_RPC_ERROR */

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

                           for profile download response =>
                             GSTK_REG_PROFILE_DL_RSP_EVT

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
extern gstk_status_enum_type gstk_client_reg (
        gstk_client_id_type     client_id,
        uint32                  user_data,
        gstk_evt_cb_funct_type  gstk_callback_function_to_register,
        uint32                  evt_reg_bitmask );
/*~ FUNCTION gstk_client_reg */
/*~ ONERROR return GSTK_RPC_ERROR */

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
                             GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT
                             GSTK_REG_SETUP_EVT_CH_STATUS_EVT 

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
extern gstk_status_enum_type gstk_client_reg_ext (
        gstk_client_id_type     client_id,
        uint32                  user_data,
        gstk_evt_cb_funct_type  gstk_callback_function_to_register,
        uint64                  evt_reg_bitmask );
/*~ FUNCTION gstk_client_reg_ext */
/*~ ONERROR return GSTK_RPC_ERROR */

/*===========================================================================
FUNCTION GSTK_CLIENT_TOOLKIT_CMD_REG

DESCRIPTION
  This is a new registration API that clients can use to ask GSTK to send
  proactive commands of their All tasks wishing to receive GSTK events can use this function to register
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
  This function should only be called after gstk_client_init has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_client_toolkit_cmd_reg(
  /* Client id */
  gstk_client_id_type                         client_id,
  /* Client user data */
  gstk_client_ref_data_type                   client_user_data,
  /* Data format */
  gstk_client_pro_cmd_data_format_enum_type   data_format_type,
  /* Number of commands being registered for */
  uint32                                      num,                                      
  const gstk_toolkit_cmd_reg_info_type        *reg_list_ptr,
  /* Immediate callback invoked to reflect status of registration */
  gstk_reg_rsp_cmd_cb_func_type               client_cmd_cb,
  /* Callback to receive command in the raw / gstk format */
  gstk_toolkit_evt_cb_func_type               client_evt_cb
);
/*~ FUNCTION gstk_client_toolkit_cmd_reg */
/*~ PARAM IN reg_list_ptr VARRAY LENGTH num */
/*~ ONERROR return GSTK_RPC_ERROR */

/*===========================================================================
FUNCTION GSTK_SEND_TERMINAL_RESPONSE

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a terminal response to
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
extern gstk_status_enum_type gstk_send_terminal_response (
        gstk_client_id_type                     client_id,
        uint32                                  user_data,
        uint32                                  cmd_detail_ref_id,
        uint8                                   command_number,
        gstk_cmd_enum_type                      terminal_response_enum,
        gstk_general_result_enum_type           general_result,
        const gstk_additional_info_type         *additional_result,
        gstk_terminal_rsp_extra_param_type      *other_info );
/*~ FUNCTION gstk_send_terminal_response */
/*~ PARAM IN additional_result POINTER */
/*~ PARAM IN other_info POINTER */



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
extern gstk_status_enum_type gstk_send_terminal_response_ext (
        gstk_client_id_type                     client_id,
        uint32                                  user_data,
        uint32                                  cmd_detail_ref_id,
        uint8                                   command_number,
        gstk_cmd_enum_type                      terminal_response_enum,
        gstk_general_result_enum_type           general_result,
        gstk_additional_info_ptr_type          *additional_result,
        gstk_terminal_rsp_extra_param_type     *other_info );
/*~ FUNCTION gstk_send_terminal_response_ext */
/*~ PARAM IN additional_result POINTER */
/*~ PARAM IN other_info POINTER */

/*===========================================================================
FUNCTION GSTK_SEND_RAW_TERMINAL_RESPONSE

DESCRIPTION
  This function is served as a utility function to the gstk client, so that
  the client can call this function in sending a raw terminal response to
  a proactive command. The applicable proactive commands are:
    1) DISPLAY TEXT
    2) GET INKEY
    3) GET INPUT
    4) PLAY TONE
    5) REFRESH
    6) SET UP MENU
    7) SELECT ITEM
    8) SET UP IDLE MODE TEXT
    9) SEND DTMF
    10)LAUNCH BROWSER

PARAMETERS
  client_id:         [Input] Client id of the calling client.  This should be the
                             value returned from the gstk_client_init()
  user_data:         [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_raw_bytes_hdr_type
 term_rsp:           [Input] The raw terminal response that the client wants to send

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
extern gstk_status_enum_type gstk_send_raw_terminal_response (
        gstk_client_id_type                     client_id,
        gstk_client_ref_data_type               user_data,
        uint32                                  cmd_detail_ref_id,
        gstk_generic_data_type                  term_resp);
/*~ FUNCTION gstk_send_raw_terminal_response */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CC_COMMAND

DESCRIPTION

  This function send the Call Control envelope command to SIM/USIM.
  Clients need to supply the gstk_cc_ind_type as the parameter.
  within the gstk_cc_ind_type,
  Also, the gstk_address_string is required based on whether the call is
  voice call, SS or USSD
  there are also 3 optional data fields: subaddress, ccp1 and ccp2 for voice
  call related Call Control envelope command
  This function does not require client ID because it is intended to be called
  by internal modules.

PARAMETERS
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
extern gstk_status_enum_type gstk_send_envelope_cc_command (
        uint32                    user_data,
        gstk_cc_ind_type          *cc_ind_param,
        gstk_evt_cb_funct_type    gstk_cc_cb );
/*~ FUNCTION gstk_send_envelope_cc_command */
/*~ PARAM IN cc_ind_param POINTER */
/*~ ONERROR return GSTK_RPC_ERROR */

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
extern boolean gstk_is_cc_required(void);
/*~ FUNCTION gstk_is_cc_required */

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
extern gstk_status_enum_type gstk_send_envelope_evt_download_command (
        gstk_client_id_type         client_id,
        uint32                      user_data,
        gstk_evt_dl_ind_type        *evt_list );
/*~ FUNCTION gstk_send_envelope_evt_download_command */
/*~ PARAM IN evt_list POINTER */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MO_SMS_CTRL_COMMAND

DESCRIPTION

  This function is used to send MO SMS control to the SIM/USIM
  within the gstk_mo_sms_ctrl_ind_type structure, user needs to
  populate the RP_Destination_Address, TP_Destination_Address
  This function does not require client ID because it is intended to be called
  by internal modules.

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
extern gstk_status_enum_type gstk_send_envelope_mo_sms_ctrl_command (
    uint32                                user_data,
    const gstk_mo_sms_ctrl_ind_type      *mo_sms_control,
    gstk_evt_cb_funct_type                gstk_mo_sms_ctrl_cb );



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
extern gstk_status_enum_type gstk_send_envelope_menu_sel_command (
        gstk_client_id_type          client_id,
        uint32                       user_data,
        uint8                        item_chosen,
        boolean                      help_requested );
/*~ FUNCTION gstk_send_envelope_menu_sel_command */

/*===========================================================================
FUNCTION GSTK_SEND_RAW_ENVELOPE_COMMAND

DESCRIPTION

  This function is used to send raw envelope command down to SIM/USIM/RUIM.
  
PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_client_init()
  user_data: [Input] User data that client wants GSTK to keep track of
  env_cmd:   [Input] The envelope command data to be sent to SIM/USIM/RUIM
  env_cmd_cb:[Input] Callback function

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
  gstk_raw_envelope_rsp_type in its callback function.

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_send_raw_envelope_command (
  gstk_client_id_type          client_id,
  uint32                       user_data,
  gstk_generic_data_type       env_cmd,
  gstk_evt_cb_funct_type       gstk_raw_env_cb);
/*~ FUNCTION gstk_send_raw_envelope_command */
/*~ ONERROR return GSTK_RPC_ERROR */

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
extern gstk_status_enum_type gstk_send_envelope_sms_pp_dl_command (
    uint32                         user_data,
    boolean                        is_cdma_sms_pp,
    const gstk_address_type       *address,
    const gstk_sms_tpdu_type      *sms_tpdu,
    gstk_evt_cb_funct_type         gstk_sms_pp_dl_cb );



/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION

  Clients can call this function to send a cell broadcast download command to the
  SIM/USIM.

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command.  The length field should be set to 
                    GSTK_CELL_BROADCAST_PAGE_LEN
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

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
extern gstk_status_enum_type gstk_send_envelope_cell_broadcast_dl_command (
  uint32                     user_data,
  const gstk_cb_page_type   *cb_page_ptr,
  gstk_evt_cb_funct_type     gstk_cb_dl_cb );


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
extern gstk_status_enum_type gstk_update_profile_download_value (
    gstk_client_id_type                    client_id,
    uint32                                 user_data,
    uint8                                  num_updates,
    const gstk_profile_dl_support_type    *value_list );
/*~ FUNCTION gstk_update_profile_download_value */
/*~ PARAM IN value_list VARRAY LENGTH num_updates */

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
extern gstk_status_enum_type gstk_update_profile_download_value_ext (
    gstk_client_id_type                        client_id,
    uint32                                     user_data,
    uint8                                      num_updates,
    const gstk_profile_dl_support_ext_type    *value_list_ptr );
/*~ FUNCTION gstk_update_profile_download_value_ext */
/*~ PARAM IN value_list_ptr VARRAY LENGTH num_updates */

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
extern gstk_status_enum_type gstk_map_proactive_req_to_cnf(
                      gstk_cmd_enum_type  request_cmd,
                      gstk_cmd_enum_type *expected_cmd_rsp)  ;
/*~ FUNCTION gstk_map_proactive_req_to_cnf */
/*~ PARAM INOUT expected_cmd_rsp POINTER */


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
extern gstk_status_enum_type gstk_parse_ton_npi(
                                        uint8 TON_NPI,
                                        gstk_ton_enum_type *TON,
                                        gstk_npi_enum_type *NPI);

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
extern gstk_status_enum_type gstk_slot_send_envelope_cc_command (
    gstk_slot_id_enum_type    slot,
    uint32                    user_data,
    const gstk_cc_ind_type   *cc_ind_param,
    gstk_evt_cb_funct_type    gstk_cc_cb );

/*===========================================================================
FUNCTION gstk_address_to_ascii

DESCRIPTION
  This function converts the gstk address foramt to ASCII value

PARAMETERS
  gstk_addr: [Input] Pointer to input address
  ascii_addr: [Input/Output] Pointer to output address
  ascii_len; [Input] Length of the ascii_addr buffer, to ensure the size of
                     the ascii_addr will not exceed the information that is
                     being converted.


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
extern gstk_status_enum_type gstk_address_to_ascii(
  const gstk_address_type *gstk_addr,
  uint8                   *ascii_addr,
  uint8                    ascii_len);

/*===========================================================================
FUNCTION gstk_ascii_to_gstk_address_type

DESCRIPTION
  This function converts the ASCII value to gstk address format

PARAMETERS
  ascii_addr: [Input] Pointer to input address
  ascii_len: [Input] input address length
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
extern gstk_status_enum_type gstk_ascii_to_gstk_address_type(
  const uint8       *ascii_addr,
  uint8              ascii_len,
  gstk_address_type *gstk_addr);


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
extern gstk_status_enum_type gstk_unpack_7_bit_chars (const uint8    *in,
                                                      uint32          in_len,
                                                      uint8          *out,
                                                      uint32         *out_len );


/*===========================================================================
FUNCTION gstk_util_decode_dcs_type

DESCRIPTION
  This function determines if the dcs is 7 bit or 8 bit or ucs2 depending
  on whether it is a cell broadcast dcs or not

PARAMETERS
  is_cb_dcs - This flag is used for differentiating if the dcs is for 
              ussd or not
  cs        - data coding scheme

DEPENDENCIES
  None

RETURN VALUE
  gstk_dcs_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_dcs_enum_type gstk_util_decode_dcs_type(boolean is_cb_dcs, uint8 dcs);
/*~ FUNCTION gstk_util_decode_dcs_type */

/*===========================================================================
FUNCTION gstk_util_unpack_7_bit_chars_ext

DESCRIPTION
  This function unpacks the input data from SMS7 bit format into ASCII
  and returns the actual data length without the null terminated byte.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  int32: length of output data length
  If this function returns negative value, it should be considered as 
  an error.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern int32 gstk_util_unpack_7_bit_chars_ext
(
  const uint8       * in,
  int32             in_len,
  uint16            * out
);
/*~ FUNCTION gstk_util_unpack_7_bit_chars_ext */
/*~ PARAM IN in VARRAY LENGTH in_len */
/*~ PARAM OUT out VARRAY GSTK_EXP_RPC_MAX_OUTPUT_DATA LENGTH _RESULT_ */


/*===========================================================================
FUNCTION gstk_decode_h80_usc2_alphabet

DESCRIPTION
  Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer
    
PARAMETERS
  int32           in_len          - length of the input buffer
  uint8          *ucs2_default_in - ucs2 buffer
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32 : Length of NULL terminated output buffer.
  If this function returns negative value, it should be considered as 
  an error.

===========================================================================*/
int32 gstk_decode_h80_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in, 
                                    uint16             *alphabet_out);
/*~ FUNCTION gstk_decode_h80_usc2_alphabet */
/*~ PARAM IN ucs2_default_in VARRAY LENGTH in_len */
/*~ PARAM OUT alphabet_out VARRAY GSTK_EXP_RPC_MAX_OUTPUT_DATA LENGTH _RESULT_ */
/*===========================================================================
FUNCTION gstk_decode_h81_usc2_alphabet

DESCRIPTION
  Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer
    
PARAMETERS
  int32           in_len          - length of the input buffer
  uint8          *ucs2_default_in - ucs2 buffer
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32 : Length of NULL terminated output buffer.
  If this function returns negative value, it should be considered as 
  an error.

===========================================================================*/
int32 gstk_decode_h81_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in, 
                                    uint16             *alphabet_out);
/*~ FUNCTION gstk_decode_h81_usc2_alphabet */
/*~ PARAM IN ucs2_default_in VARRAY LENGTH in_len */
/*~ PARAM OUT alphabet_out VARRAY GSTK_EXP_RPC_MAX_OUTPUT_DATA LENGTH _RESULT_ */

/*===========================================================================
FUNCTION gstk_decode_h82_usc2_alphabet

DESCRIPTION
  Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer
    
PARAMETERS
  int32           in_len          - length of the input buffer
  uint8          *ucs2_default_in - ucs2 buffer
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32 : Length of NULL terminated output buffer.
  If this function returns negative value, it should be considered as 
  an error.

===========================================================================*/
int32 gstk_decode_h82_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in, 
                                    uint16             *alphabet_out);
/*~ FUNCTION gstk_decode_h82_usc2_alphabet */
/*~ PARAM IN ucs2_default_in VARRAY LENGTH in_len */
/*~ PARAM OUT alphabet_out VARRAY GSTK_EXP_RPC_MAX_OUTPUT_DATA LENGTH _RESULT_ */

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
  uint32               cmd_ref_id,
  boolean              yes_no_rsp
);
/*~ FUNCTION gstk_send_setup_call_user_cnf_alpha_rsp */

/*============================================================================
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
);
/*~ FUNCTION gstk_client_id_force_cleanup */

#endif  /* GSTK_EXP_H */

