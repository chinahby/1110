#ifndef UIMTK_H
#define UIMTK_H
/*===========================================================================

 E X T E R N A L   U I M  T O O L K I T   L I B R A R Y   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the UIM toolkit interface and TLV 
  library.

Copyright (c) 2001 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uimtk.h_v   1.3   05 Apr 2002 17:26:44   tsummers  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimtk.h#1 $ $DateTime: 2008/12/15 12:00:08 $ $Author: nkukreja $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/03   wli     Added GSTK feature.
02/13/03   wli     Changed feature TOOKIT TO TOOKIT_UTK.
12/20/02   ts      Fixed Lint issues
04/03/02   ts      Added a parse return status to indicate the last simple TLV
                   in the BER TLV.  Added the reference to the Proactive 
                   polling duration.
12/13/01   ts      Aligned code and fixed comments.
10/05/01   ts      Created file to support the UIM toolkit and TLV objects.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "uimdrv.h"
#if defined FEATURE_UIM_TOOLKIT_UTK && !defined FEATURE_GSTK

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
                 TLV definitions for UIM toolkit interface
--------------------------------------------------------------------------*/

/* This type defines the tag values of all tags that are used in the interface
   with the UIM application toolkit */
typedef enum {
/* Tag values for the simple tags */                      
  UIM_TK_COMMAND_DETAILS_TAG       = 0x01,
  UIM_TK_DEVICE_ID_TAG             = 0x02,
  UIM_TK_RESULT_TAG                = 0x03,
  UIM_TK_DURATION_TAG              = 0x04,
  UIM_TK_ALPHA_ID_TAG              = 0x05,
  UIM_TK_ADDRESS_TAG               = 0x06,
  UIM_TK_CAP_CONFIG_TAG            = 0x07,
  UIM_TK_SUBADDRESS_TAG            = 0x08,
  UIM_TK_CDMA_SMS_TPDU_TAG         = 0x48,
  UIM_TK_TEXT_STRING_TAG           = 0x0D,
  UIM_TK_TONE_TAG                  = 0x0E,
  UIM_TK_ITEM_TAG                  = 0x0F,
  UIM_TK_ITEM_ID_TAG               = 0x10,
  UIM_TK_RSP_LEN_TAG               = 0x11,
  UIM_TK_FILE_LIST_TAG             = 0x12,
  UIM_TK_LOCN_INFO_TAG             = 0x13,
  UIM_TK_IMEI_TAG                  = 0x14,
  UIM_TK_HELP_REQ_TAG              = 0x15,
  UIM_TK_NW_MEAS_RESULTS_TAG       = 0x16,
  UIM_TK_DEFAULT_TEXT_TAG          = 0x17,
  UIM_TK_ITEMS_NEXT_ACTION_IND_TAG = 0x18,
  UIM_TK_ICON_ID_TAG               = 0x1E,
  UIM_TK_ICON_ID_LIST_TAG          = 0x1F,
  UIM_TK_IMMEDIATE_RSP_TAG         = 0x2B,
  UIM_TK_LANG_TAG                  = 0x2D,
 #ifdef CUST_EDITION   
  UIM_TK_DATE_TIME_AND_TIME_ZONE_TAG = 0x26, 
  UIM_TK_ACCESS_TECHNOLOGY_TAG     = 0x3F,  
#endif /*CUST_EDITION*/
/* Tag values for the BER tags */
  UIM_TK_PROACTIVE_SIM_COMMAND_TAG = 0xD0,
  UIM_TK_CDMA_SMS_PP_DOWNLOAD_TAG  = 0xD1, 
  UIM_TK_MENU_SELECTION_TAG        = 0xD3
} uim_tk_tag_enum_type;                  
                                         

/* Type of command coding for proactive commands */
typedef enum {
  UIM_TK_REFRESH               =   0x01,
  UIM_TK_MORE_TIME             =   0x02,
  UIM_TK_POLL_INTERVAL         =   0x03,
  UIM_TK_POLLING_OFF           =   0x04,
  UIM_TK_SETUP_CALL            =   0x10,
  UIM_TK_CDMA_SEND_SHORT_MSG   =   0x13,
  UIM_TK_PLAY_TONE             =   0x20,
  UIM_TK_DISPLAY_TEXT          =   0x21,
  UIM_TK_GET_INPUT             =   0x23,
  UIM_TK_SELECT_ITEM           =   0x24,
  UIM_TK_SETUP_MENU            =   0x25,
  UIM_TK_PROVIDE_LOCAL_INFO    =   0x26,
  UIM_TK_END_PROACTIVE_SESSION =   0x81
} uim_tk_proactive_cmd_enum_type;

/* Definitions for the Tone field of a Tone proactive command */
#define UIM_TK_DIAL_TONE                                0x01
#define UIM_TK_CALLED_SUBSCRIBER_BUSY                   0x02
#define UIM_TK_CONGESTION                               0x03
#define UIM_TK_RADIO_PATH_ACKNOWLEDGE                   0x04
#define UIM_TK_RADIO_PATH_NOT_AVAILABLE_OR_CALL_DROPPED 0x05
#define UIM_TK_ERROR_OR_SPECIAL_INFORMATION             0x06
#define UIM_TK_CALL_WAITING                             0x07
#define UIM_TK_RINGING                                  0x08
#define UIM_TK_GENERAL_BEEP                             0x10
#define UIM_TK_POSITIVE_ACKNOWLEDGEMENT                 0x11
#define UIM_TK_NEGATIVE_ACKNOWLEDGEMENT_OR_ERROR        0x12
#define UIM_TK_RINGING_TONE_SEL_BY_USER_FOR_IC_V_CALL   0x13
#define UIM_TK_ALERT_TONE_SEL_BY_USER_FOR_IC_SMS        0x14

/* Definitions for General Result of a Result proactive command*/
typedef enum {
 UIM_TK_CMD_PERFORMED_SUCCESSFULLY                         = 0x00,
 UIM_TK_CMD_PERFORMED_W_PARTIAL_COMPREHENSION              = 0x01,
 UIM_TK_CMD_PERFORMED_W_MISSING_INFO                       = 0x02,
 UIM_TK_REFRESH_PERFORMED_W_ADDNL_EFS_READ                 = 0x03,
 UIM_TK_CMD_PERFORMED_SUCCESSFULLY_BUT_REQ_ICON_NOT_DISP   = 0x04,
 UIM_TK_CMD_PERFORMED_BUT_MODIFIED_BY_CC_BY_SIM            = 0x05,
 UIM_TK_CMD_PERFORMED_SUCCESSFULLY_LIMITED_SVC             = 0x06,
 UIM_TK_CMD_PERFORMED_W_MODIFICATION                       = 0x07,
 UIM_TK_REFRESH_PERFORMED_BUT_INDICATED_NAA_WAS_NOT_ACTIVE = 0x08,
#ifdef CUST_EDITION  
 UIM_TK_CMD_PERFORMED_BUT_TONE_NOT_PLAYED                  = 0x09,
#endif /*CUST_EDITION*/ 
 UIM_TK_PROACTIVE_RUIM_SESSION_TERMINATED_BY_USER          = 0x10,
 UIM_TK_BWARD_MOVE_IN_PROACTIVE_RUIM_SESSION_REQ_BY_USER   = 0x11,
 UIM_TK_NO_RESPONSE_FROM_USER                              = 0x12,
 UIM_TK_HELP_INFO_REQD_BY_USER                             = 0x13,
 UIM_TK_USSD_OR_SS_TRANSACTION_TERMINATED_BY_USER          = 0x14,
 UIM_TK_TERMINAL_CURRENTLY_UNABLE_TO_PROCESS_COMMAND       = 0x20,
 UIM_TK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND        = 0x21,
 UIM_TK_USER_DID_NOT_ACCEPT_PROACTIVE_COMMAND              = 0x22,
 UIM_TK_USER_CLEARED_DOWN_CALL_BEFORE_CONN_OR_NW_REL       = 0x23,
 UIM_TK_ACTION_IN_CONTRADICTION_W_CURR_TIMER_STATE         = 0x24,
 UIM_TK_INTERACTION_W_CC_BY_SIM_TEMP_PBLM                  = 0x25,
 UIM_TK_LAUNCH_BROWSER_GENERIC_ERROR_CODE                  = 0x26,
 UIM_TK_COMMAND_BEYOND_TERMINALS_CAPABILITIES              = 0x30,
 UIM_TK_COMMAND_TYPE_NOT_UNDERSTOOD_BY_TERMINAL            = 0x31,
 UIM_TK_COMMAND_DATA_NOT_UNDERSTOOD_BY_TERMINAL            = 0x32,
 UIM_TK_COMMAND_NUMBER_NOT_KNOWN_BY_TERMINAL               = 0x33,
 UIM_TK_SS_RETURN_ERROR                                    = 0x34,
 UIM_TK_SMS_RP_ERROR                                       = 0x35,
 UIM_TK_ERROR_REQD_VALUES_ARE_MISSING                      = 0x36,
 UIM_TK_USSD_RETURN_ERROR                                  = 0x37,
 UIM_TK_INTERACTION_W_CC_OR_MO_SMS_BY_SIM_PERM_PBLM        = 0x39
} uim_tk_general_result_type;                              
                                                           
/* Additional info on Terminal Problem of a Result proactive command */
#define UIM_TK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN    0x00
#define UIM_TK_SCREEN_IS_BUSY                    0x01
#define UIM_TK_TERMINAL_CURRENTLY_BUSY_ON_CALL   0x02
#define UIM_TK_NO_SERVICE                        0x04
#define UIM_TK_ACCESS_CONTROL_CLASS_BAR          0x05
#define UIM_TK_RADIO_RESOURCE_NOT_GRANTED        0x06
#define UIM_TK_NOT_IN_SPEECH_CALL                0x07
#define UIM_TK_NO_NAA_ACTIVE                     0x0A

/* Refresh command details qualifiers */
#define UIM_TK_NAA_INITIALIZATION_AND_FULL_FILE_CHANGE_NOTIFICATION 0x00
#define UIM_TK_FILE_CHANGE_NOTIFICATION                             0x01
#define UIM_TK_NAA_INITIALISATION_AND_FILE_CHANGE_NOTIFICATION      0x02
#define UIM_TK_NAA_INITIALISATION                                   0x03
#define UIM_TK_NAA_RESET                                            0x04

/* Set up Call command details qualifiers */
#define UIM_TK_SETUP_CALL_BUT_ONLY_IF_NOT_BUSY_ON_ANOTHER_CALL           0x00
#define UIM_TK_SETUP_CALL_BUT_ONLY_IF_NOT_BUSY_ON_ANOTHER_CALL_W_REDIAL  0x01
#define UIM_TK_SETUP_CALL_PUTTING_ALL_OTHER_CALLS_ON_HOLD                0x02
#define UIM_TK_SETUP_CALL_PUTTING_ALL_OTHER_CALLS_ON_HOLD_W_REDIAL       0x03
#define UIM_TK_SETUP_CALL_DISCONNECTING_ALL_OTHER_CALLS                  0x04
#define UIM_TK_SETUP_CALL_DISCONNECTING_ALL_OTHER_CALLS_W_REDIAL         0x05

typedef enum {
/* Source and Destination ID of the Device Identities command*/
 UIM_TK_KEYPAD   =  0x01, 
 UIM_TK_DISPLAY  =  0x02,
 UIM_TK_EARPIECE =  0x03,
 UIM_TK_UIM      =  0x81,
 UIM_TK_TERMINAL =  0x82,
 UIM_TK_NETWORK  =  0x83
} uim_tk_dev_id_type;

/* Time unit coding of Duration command */
#define UIM_TK_MINUTES      0x00 
#define UIM_TK_SECONDS      0x01
#define UIM_TK_TENTHS_SECS  0x02

#ifdef CUST_EDITION  
/* Get input command details qualifiers */
#define UIM_TK_GETINPUT_NUM_ALPHA    0x01
#define UIM_TK_GETINPUT_GSM_UNICODE  0x02
#define UIM_TK_GETINPUT_DISPLAY_TEXT 0x04
#define UIM_TK_GETINPUT_COMPRESS     0x08
#define UIM_TK_GETINPUT_ALPHA_UNICODE 0x03

/* Local Information command details qualifiers */
#define UIM_TK_LOCAL_INFO_ACCORD      0x00 
#define UIM_TK_LOCAL_INFO_DATE_TIME   0x03 
#define UIM_TK_LOCAL_INFO_LANGUAGE    0x04 
#define UIM_TK_LOCAL_INFO_ACCESS_TECH 0x06 

#define UIM_TK_ACCESS_TECH_TIA_EIA_IS2000 0x06 
#endif
/* Coding for PROFILE COMMAND      */
/* Byte 1 of TERMINAL PROFILE data */
#define UIM_TK_B1_PROFILE_DOWNLOAD             0x01
#define UIM_TK_B1_CDMA_SMS_PP_DOWNLOAD         0x02
#define UIM_TK_B1_CELL_BROADCAST_DOWNLOAD      0x04
#define UIM_TK_B1_MENU_SELECTION               0x08

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B1_CDMA_SMS_PP_DOWNLOAD2        0x10
#elif defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_B1_SUPPORT_9EXX_ERROR_CODE      0x10
#endif

#define UIM_TK_B1_TIMER_EXPIRATION             0x20
#define UIM_TK_B1_RFU1                         0x40
#define UIM_TK_B1_CALL_CTRL_BY_NAA             0x80

/* Byte 2 of TERMINAL PROFILE data */
#define UIM_TK_B2_COMMAND_RESULT               0x01
#define UIM_TK_B2_CALL_CTRL_BY_NAA             0x02

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B2_CALL_CTRL_BY_NAA2            0x04
#elif  defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_CELL_ID_INCL_IN_CC_BY_NAA       0x04
#endif

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B2_RFU1                         0x08
#elif defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_MO_SMS_CONTROL_BY_NAA           0x08
#endif

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B2_RFU2                         0x10
#elif defined(FEATURE_UIM_TOOLKIT_STK)
#define UIM_TK_ALPHA_ID_HANDLING               0x10
#endif

#define UIM_TK_B2_UNICODE_ENTRY                0x20
#define UIM_TK_B2_UNICODE_DISPLAY              0x40
#define UIM_TK_B2_DISPAY_EXTENSION_TEXT        0x80

/* Byte 3 of TERMINAL PROFILE data */
#define UIM_TK_B3_DISPLAY_TEXT                 0x01
#define UIM_TK_B3_GET_INKEY                    0x02
#define UIM_TK_B3_GET_INPUT                    0x04
#define UIM_TK_B3_MORE_TIME                    0x08
#define UIM_TK_B3_PLAY_TOME                    0x10
#define UIM_TK_B3_POLL_INTERVAL                0x20
#define UIM_TK_B3_POLLING_OFF                  0x40
#define UIM_TK_B3_REFRESH                      0x80

/* Byte 4 of TERMINAL PROFILE data */
#define UIM_TK_B4_SELECT_ITEM                  0x01
#define UIM_TK_B4_SEND_SHORT_MSG               0x02
#define UIM_TK_B4_SEND_SS                      0x04
#define UIM_TK_B4_SEND_USSD                    0x08
#define UIM_TK_B4_SETUP_CALL                   0x10
#define UIM_TK_B4_SETUP_MENU                   0x20
#define UIM_TK_B4_PROVIDE_LOCAL_INFO_MCC       0x40
#define UIM_TK_B4_PROVIDE_LOCAL_INFO_NMR       0x80

/* Byte 5 of TERMINAL PROFILE data */
#define UIM_TK_B5_SETUP_EVENT_LIST             0x01
#define UIM_TK_B5_MT_CALL                      0x02
#define UIM_TK_B5_CALL_CONNECTED               0x04
#define UIM_TK_B5_CALL_DISCONNECTED            0x08
#define UIM_TK_B5_LOCATION_STATUS              0x10
#define UIM_TK_B5_USER_ACTIVITY                0x20
#define UIM_TK_B5_IDLE_SCREEN_AVAILABLE        0x40
#define UIM_TK_B5_CARD_READER_STATUS           0x80

/* Byte 6 of TERMINAL PROFILE data */
#define UIM_TK_B6_LANGUAGE_SELECTION           0x01
#define UIM_TK_B6_BROWSER_TERMINATION          0x02
#define UIM_TK_B6_DATA_AVAILABLE               0x04
#define UIM_TK_B6_CHAN_STATUS                  0x08

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B6_ACCESS_TECH_CHANGE           0x10
#define UIM_TK_B6_DISPLAY_PARAMS_CHANGE        0x20
#define UIM_TK_B6_LOCAL_CONNECTION             0x40
#define UIM_TK_B6_RFU                          0x80
#else
#define UIM_TK_B6_RFU1                         0x10
#define UIM_TK_B6_RFU2                         0x20
#define UIM_TK_B6_RFU3                         0x40
#define UIM_TK_B6_RFU4                         0x80
#endif 

/* Byte 7 of TERMINAL PROFILE data */
#define UIM_TK_B7_POWER_ON_CARD                0x01
#define UIM_TK_B7_POWER_OFF_CARD               0x02
#define UIM_TK_B7_PERFORM_CARD_APDU            0x04
#define UIM_TK_B7_GET_CARD_READER_STATUS       0x08
#define UIM_TK_B7_GET_CARD_READER_ID           0x10
#define UIM_TK_B7_RFU1                         0x20
#define UIM_TK_B7_RFU2                         0x40
#define UIM_TK_B7_RFU3                         0x80

/* Byte 8 of TERMINAL PROFILE data */
#define UIM_TK_B8_TIMER_MGMT_START_STOP        0x01
#define UIM_TK_B8_TIMER_MGMT_CURR_VALUE        0x02
#define UIM_TK_B8_PROVIDE_LOCAL_INFO_DATE      0x04

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B8_GET_INKEY                    0x08
#else
#define UIM_TK_B8_BINARY_CHOIVE_IN_GET_INKEY   0x08
#endif

#define UIM_TK_B8_SETUP_IDLE_MODE_TEXT         0x10
#define UIM_TK_B8_RUN_AT_COMMAND               0x20
#define UIM_TK_B8_SEC_ALPHA_ID_IN_SETUP_CALL   0x40

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B8_CALL_CTRL_BY_NAA             0x80
#else
#define UIM_TK_B8_SEC_CAP_CFG_PARAM            0x80
#endif

/* Byte 9 of TERMINAL PROFILE data */
#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B9_DISPLAY_TEXT                 0x01
#else
#define UIM_TK_B9_SUSTAINED_DIDPLAY_TEXT       0x01
#endif

#define UIM_TK_B9_SEND_TDMF                    0x02

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_NMR       0x04
#else
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_BCCH      0x04
#endif

#define UIM_TK_B9_PROVIDE_LOCAL_INFO_LANG      0x08
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_TIMING    0x10
#define UIM_TK_B9_LANGUAGE_NOTIFICATION        0x20
#define UIM_TK_B9_LAUNCH_BROWSER               0x40

#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B9_PROVIDE_LOCAL_INFO_ACC_TECH  0x80
#else
#define UIM_TK_B9_RFU                          0x80
#endif

/* Byte 10 of TERMINAL PROFILE data */
#define UIM_TK_B10_SOFT_KEYS_SELECT_ITEM       0x01
#define UIM_TK_B10_SOFT_KEYS_SETUP_MENU        0x02
#define UIM_TK_B10_RFU1                        0x04
#define UIM_TK_B10_RFU2                        0x08
#define UIM_TK_B10_RFU3                        0x10
#define UIM_TK_B10_RFU4                        0x20
#define UIM_TK_B10_RFU5                        0x40
#define UIM_TK_B10_RFU6                        0x80

/* Byte 11 of TERMINAL PROFILE data */
/* Indicates the max number of soft keys */

/* Byte 12 of TERMINAL PROFILE data */
#define UIM_TK_B12_OPEN_CHAN                   0x01
#define UIM_TK_B12_CLOSE_CHAN                  0x02
#define UIM_TK_B12_RECEIVE_DATA                0x04
#define UIM_TK_B12_SEND_DATA                   0x08
#define UIM_TK_B12_GET_CHANNEL_STATUS          0x10
#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B12_SERVICE_SRCH                0x20
#define UIM_TK_B12_GET_SERVICE_INFO            0x40
#define UIM_TK_B12_DECLARE_SERVICE             0x80
#else
#define UIM_TK_B12_RFU1                        0x20
#define UIM_TK_B12_RFU2                        0x40
#define UIM_TK_B12_RFU3                        0x80
#endif

/* Byte 13 of TERMINAL PROFILE data */
#define UIM_TK_B13_CSD_SUP_BY_ME               0x01
#define UIM_TK_B13_GPRS_SUP_BY_ME              0x02
#ifdef FEATURE_UIM_TOOLKIT_UTK
#define UIM_TK_B13_BT_SUP_BY_ME                0x04
#define UIM_TK_B13_IRDA_SUP_BY_ME              0x08
#define UIM_TK_B13_RS232_SUP_BY_ME             0x10
#endif
/* bit6-bit8 are number of channel supported */

/* Byte 14 of TERMINAL PROFILE data */         
/* bit1-bit5 are number of characters supported */
#define UIM_TK_B14_RFU1                        0x20
#define UIM_TK_B14_RFU2                        0x40
#define UIM_TK_B14_SCREEN_SIZE_PARAMS          0x80

/* Byte 15 of TERMINAL PROFILE data */
/* bit1-bit7 are number of characters supported */
#define UIM_TK_B15_VAR_SIZE_FONT               0x10

/* Byte 16 of TERMINAL PROFILE data */
#define UIM_TK_B16_DISP_RESIZE                 0x01
#define UIM_TK_B16_TEXT_WRAP                   0x02
#define UIM_TK_B16_TEXT_SCROLL                 0x04
#define UIM_TK_B16_RFU1                        0x08
#define UIM_TK_B16_RFU2                        0x10
/* bit6-bit8 are width reduction */             
                                                
/* Byte 17 of TERMINAL PROFILE data */          
#define UIM_TK_B17_TCP                         0x01
#define UIM_TK_B17_UDP                         0x02
#define UIM_TK_B17_RFU1                        0x04
#define UIM_TK_B17_RFU2                        0x08
#define UIM_TK_B17_RFU3                        0x10
#define UIM_TK_B17_RFU4                        0x20
#define UIM_TK_B17_RFU5                        0x40
#define UIM_TK_B17_RFU6                        0x80

#ifdef FEATURE_UIM_TOOLKIT_UTK
/* Byte 18 of TERMINAL PROFILE data */
#define UIM_TK_B18_DISP_TEXT_VAR_TIME_OUT      0x01
#define UIM_TK_B18_GET_INKEY_W_HELP            0x02
#define UIM_TK_B18_USB                         0x04
#define UIM_TK_B18_GET_INKEY_VAR_TIME_OUT      0x08
#define UIM_TK_B18_PROVIDE_LOCAL_INFO_ESN      0x10
#define UIM_TK_B18_RFU1                        0x20
#define UIM_TK_B18_RFU2                        0x40
#define UIM_TK_B18_RFU3                        0x80

#define UIM_TK_B20_CDMA_SEND_SMS 			         0x01
#define UIM_TK_B20_CDMA_SMS_PP_DOWNLOAD 	     0x02

#endif /* FEATURE_UIM_TOOLKIT_UTK */

#ifdef FEATURE_UIM_TOOLKIT_UTK
  /* Length of PROFILE command data (UTK spec) */
  #define UIM_TK_TERMINAL_PROFILE_DATA_LENGTH    20
#else /* !FEATURE_UIM_TOOLKIT_UTK */
  #define UIM_TK_TERMINAL_PROFILE_DATA_LENGTH    19
#endif /* FEATURE_UIM_TOOLKIT_UTK */

/*--------------------------------------------------------------------------
                          TLV definitions
  These types define the parsed data of the VALUE field of a simple TLV.                          
--------------------------------------------------------------------------*/
typedef struct {
  boolean                comprehension_required;
  uim_tk_tag_enum_type   tag;
} uim_tk_hdr_type;

/* Command Details - parsed Value field */
typedef struct {
  uim_tk_hdr_type                hdr;
  byte                           cmd_num;
  uim_tk_proactive_cmd_enum_type cmd_type;
  byte                           cmd_qual;
} uim_tk_command_details_tlv_type;

/* Device Identities - parsed Value field */
typedef struct {
  uim_tk_hdr_type    hdr;
  uim_tk_dev_id_type source_dev_id;
  uim_tk_dev_id_type dest_dev_id;
} uim_tk_device_id_tlv_type;

/* Result - parsed Value field */
typedef struct {
  uim_tk_hdr_type            hdr;
  uim_tk_general_result_type result;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_result_tlv_type;

/* Duration - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte time_unit;
  byte time_interval;
} uim_tk_duration_tlv_type;

/* Alpha Identifier - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_alpha_id_tlv_type;

/* Address - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte ton_npi;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_address_tlv_type;

/* Capability configuration parameters - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_cap_config_params_tlv_type;

/* Subaddress - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_subaddress_tlv_type;

/* CDMA SMS TPDU - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_cdma_sms_tpdu_tlv_type;

/* Text String - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte dcs;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_text_string_tlv_type;

/* Tone - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte tone;
} uim_tk_tone_tlv_type;

/* Item - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte item_id;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_item_tlv_type;

/* Item Identifier - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte id;
} uim_tk_item_id_tlv_type;

/* Response Length - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte min_len_rsp;
  byte max_len_rsp;
} uim_tk_rsp_len_tlv_type;

/* File List - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte num_files;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_file_list_tlv_type;

/* Location Information - parsed Value field */
typedef struct {
#ifndef CUST_EDITION  
  uim_tk_hdr_type hdr;
  dword mcc_mnc;
  word lac;
  word cell_id;
#else
  uim_tk_hdr_type hdr;   
  word   mcc;
  word   imsi_11_12;
  word   sid;
  word   nid;
  word   base_id; 
  dword  base_lat;
  dword  base_long; 
#endif  //#if 0
} uim_tk_locn_info_tlv_type;  

/* IMEI - parsed Value field */
# define UIM_TK_IMEI_SIZE 8
typedef struct {
  uim_tk_hdr_type hdr;
  byte imei[UIM_TK_IMEI_SIZE];
} uim_tk_imei_tlv_type;


/* Network Measurement Results - parsed Value field */
#define UIM_TK_NW_MEAS_RESULTS_DATA_SIZE  16
typedef struct {
  uim_tk_hdr_type hdr;
  byte nw_meas_data[UIM_TK_NW_MEAS_RESULTS_DATA_SIZE];
} uim_tk_nw_meas_results_tlv_type;

/* Default Text - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte dcs;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_default_text_tlv_type;

/* Items Next Action Indicator - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_items_next_action_ind_tlv_type;

/* Icon Identifier - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte icon_qual;
  byte icon_id;
} uim_tk_icon_id_tlv_type;

/* Item Icon Identifier List - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte icon_list_qual;
  byte num_bytes;
  byte data[UIM_MAX_CHARS];
} uim_tk_icon_id_list_tlv_type;   

/* Language - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  word lang;
} uim_tk_lang_tlv_type;

#ifdef CUST_EDITION  
/* access_technology - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  byte access_technology;
} uim_tk_access_technology_tlv_type;

/* date_time - parsed Value field */
typedef struct {
  uim_tk_hdr_type hdr;
  word  year;
  word  month;
  word  day;
  word  hour;
  word  minute;
  word  second;
  word  zone;
} uim_tk_date_time_tlv_type;
#endif

/* This type defines a parsed simple tlv.  A client task can use this type
   to process simple tlv data objects. */
typedef union {
  uim_tk_hdr_type                    hdr;
  uim_tk_command_details_tlv_type    cmd_details;
  uim_tk_device_id_tlv_type          device_id;
  uim_tk_result_tlv_type             result;
  uim_tk_duration_tlv_type           duration;
  uim_tk_alpha_id_tlv_type           alpha_id;
  uim_tk_address_tlv_type            addr;
  uim_tk_cap_config_params_tlv_type  cap_cfg;   
  uim_tk_subaddress_tlv_type         subaddr;
  uim_tk_cdma_sms_tpdu_tlv_type      cdma_sms;
  uim_tk_text_string_tlv_type        txt;
  uim_tk_tone_tlv_type               tone;
  uim_tk_item_tlv_type               item;
  uim_tk_item_id_tlv_type            item_id;
  uim_tk_rsp_len_tlv_type            rsp_len;
  uim_tk_file_list_tlv_type          file_list;
  uim_tk_locn_info_tlv_type          locn_info;
  uim_tk_imei_tlv_type               iemi;
  uim_tk_nw_meas_results_tlv_type    nw_meas_results;
  uim_tk_default_text_tlv_type       default_txt;
  uim_tk_items_next_action_ind_tlv_type next_action;
  uim_tk_icon_id_tlv_type            icon_id;
  uim_tk_icon_id_list_tlv_type       icon_id_list;
  uim_tk_lang_tlv_type               lang;
#ifdef CUST_EDITION  
  uim_tk_access_technology_tlv_type  access_technology;
  uim_tk_date_time_tlv_type          date_time;
#endif
} uim_tk_tlv_type;

/*--------------------------------------------------------------------------
                          TLV conversion library
  These types define the interface to the tlv library conversion routines.
--------------------------------------------------------------------------*/

/* This type defines the result of a tlv conversion */
typedef enum {
  UIM_TK_MESSAGE_IS_VALID,
  UIM_TK_MSG_VALID_AND_LAST,
  UIM_TK_UNEXPECTED_TAG,
  UIM_TK_UNKNOWN_TAG,
  UIM_TK_OFFSET_OUTSIDE_BUFFER,
  UIM_TL_LENGTH_ERROR,
  UIM_TK_MESSAGE_TOO_SHORT,
  UIM_TK_MESSAGE_TOO_LONG
} uim_tk_tlv_proc_status_type;

/*--------------------------------------------------------------------------
                 BER TLV Conversion routines
--------------------------------------------------------------------------*/

/* This type defines the return type for parsing a BER TLV */
typedef struct {
  uim_tk_tlv_proc_status_type status; /* Result of packing operation */
  boolean              comprehension_required;
  uim_tk_tag_enum_type tag;  /* BER-TLV tag */
} uim_tk_parse_ber_tlv_return_type;

/* This type defines the return type for packing a BER TLV */
typedef struct {
  uim_tk_tlv_proc_status_type status; /* Result of packing operation */
  byte  tlv_offset;            /* Offset into buffer where BER-TLV starts 
                                  This is needed since the length field 
                                  is variable.  */
  byte  total_size;            /* This is the size of the entire BER-TLV */
} uim_tk_pack_ber_tlv_return_type;
#ifdef CUST_EDITION  
// integer to BCD
#define UTK_INT2BCD(x)  ((x/10)|((x%10) << 4))
// BCD to integer
#define UTK_BCD2INT(x)  (((((x) & 0xf0) >> 4) * 10) + ((x) & 0x0f))
#endif
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PARSE_BER_TLV

DESCRIPTION
  This procedure parses a BER-TLV.  The function compares the amount of
  data in the buffer to the length in the BER-TLV.

DEPENDENCIES
  The function needs the size of the raw data to determine if the length 
  in the TLV matches the amount of data received.

RETURN VALUE
  This function returns the status of the parse and the tag value
  of the BER_TLV.

SIDE EFFECTS
  None

===========================================================================*/
extern uim_tk_parse_ber_tlv_return_type uim_tk_parse_ber_tlv
(
  byte  *tlv_buff,
  word  total_size
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PACK_BER_TLV

DESCRIPTION
  This procedure formats a BER-TLV.  The function adds a tag and length 
  for a BER-TLV that has already had the value field filled out.  Since the
  length field cannot be determined beforehand, this function returns an
  offset that indicates the number of bytes (from the beginning of the 
  buffer) the BER-TLV actually starts.  

DEPENDENCIES
  The function needs the size of the Value field to determine the length
  of the TLV.  Also, the Value field should already been filled out with
  simple TLVs.
  
  This function assumes the BER-TLV is 255 bytes (total) in size.
  
RETURN VALUE
  This function returns the status of the pack and the tag value
  of the BER_TLV.

SIDE EFFECTS
  None

===========================================================================*/
extern uim_tk_pack_ber_tlv_return_type uim_tk_pack_ber_tlv
(
  byte                 *tlv_buff,  /* BER-TLV raw data buffer */
  uim_tk_tag_enum_type tag,        /* BER-TLV tag */
  byte                 value_size  /* Size of the Value field */
);

/* <EJECT> */
/* This type defines the return type for parsing or packing a BER TLV */
typedef struct {
  uim_tk_tlv_proc_status_type status;
  byte                 tlv_size;
} uim_tk_proc_simple_tlv_return_type;

/*===========================================================================

FUNCTION UIM_TK_PARSE_SIMPLE_TLV_FROM_BER_TLV

DESCRIPTION
  This procedure parses a simple TLV from a BER-TLV.  The function looks for 
  valid tag and length values.

DEPENDENCIES
  The function needs the address of the BER-TLV and the byte offset (in the
  Value field) of the start of the simple TLV.  
  The offset is used to determine where to parse a simple TLV from the Value
  field of the BER TLV.  
                            -----
                            | T |  BER TLV buffer
                            | L |
                            |   |
                            |TLV|  Simple TLV within Value field (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            -----
                                

RETURN VALUE
  This function returns the status of the parse and the length of the simple
  TLV.

SIDE EFFECTS
  This function builds a parsed simple TLV in the buffer pointed to by 
  parsed_tlv_buff.

===========================================================================*/
extern uim_tk_proc_simple_tlv_return_type uim_tk_parse_simple_tlv_from_ber_tlv
(
  byte*            tlv_buff,       /* BER_TLV buffer pointer */
  byte             offset,         /* byte offset of simple tlv */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed tlv buffer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PACK_SIMPLE_TLV_IN_BER_TLV

DESCRIPTION
  This procedure packs a simple TLV into a BER-TLV Value field.  The function 
  checks for valid tags and lengths of the simple TLV.  It also ensures the 
  simple TLV does not overwrite the BER-TLV (assumed to be 255 bytes).
  length values.

DEPENDENCIES
  The function needs the address of the BER-TLV and the byte offset (in the
  Value field) of the start of the simple TLV.  
  The offset is used to determine where to pack a simple TLV into the Value
  field of the BER TLV.  
                            -----
                            | T |  BER TLV buffer
                            | L |
                            |   |
                            |TLV|  Simple TLV within Value field (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            -----

RETURN VALUE
  This function returns the status of the pack and the length of the simple
  TLV.

SIDE EFFECTS
  This function fills in the simple tlv into the Value field of a BER TLV.
  The simple TLV is placed at the byte offset of the Value field of the BER
  TLV.

===========================================================================*/
extern uim_tk_proc_simple_tlv_return_type uim_tk_pack_simple_tlv_in_ber_tlv
(
  byte*            tlv_buff,       /* BER_TLV buffer pointer */
  byte             offset,         /* byte offset of simple tlv */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed tlv buffer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PARSE_SIMPLE_TLV

DESCRIPTION
  This procedure parses a simple TLV.  The function looks for valid tag and
  length values.

DEPENDENCIES
  The function needs the address of the TLV buffer and the byte offset of the 
  start of the simple TLV.  
  The offset is used to determine where to parse a simple TLV from a buffer of
  a list of simple TLVs.
                            -----
                            |TLV|  Simple TLV within buffer (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            -----

RETURN VALUE
  This function returns the status of the parse and the length of the simple
  TLV.

SIDE EFFECTS
  This function builds a parsed simple TLV in the buffer pointed to by 
  parsed_tlv_buff.

===========================================================================*/
extern uim_tk_proc_simple_tlv_return_type uim_tk_parse_simple_tlv
(
  byte*            tlv_buff,       /* Simple TLV list buffer pointer */
  byte             offset,         /* byte offset of simple tlv within buffer */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed simple tlv buffer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PACK_SIMPLE_TLV

DESCRIPTION
  This procedure packs a simple TLV into a simple buffer.  The function 
  checks for valid tags and lengths of the simple TLV.  It also ensures the 
  simple TLV does not overwrite the simple buffer (assumed to be 255 bytes).

DEPENDENCIES
  The function needs the address of the simple buffer and the byte offset 
  of the start of the simple TLV.  
  The offset is used to determine where to pack a simple TLV into a buffer
  of a list of simple TLVs.
                            -----
                            |TLV|  Simple TLV within Value field (offset=0)
                            |   |
                            |   |
                    offset->|TLV|  Simple TLV within Value field
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            |   |
                            -----

RETURN VALUE
  This function returns the status of the pack and the length of the simple
  TLV.

SIDE EFFECTS
  This function fills in the simple tlv into the Value field of a BER TLV.
  The simple TLV is placed at the byte offset of the Value field of the BER
  TLV.

===========================================================================*/
extern uim_tk_proc_simple_tlv_return_type uim_tk_pack_simple_tlv
(
  byte*            tlv_buff,       /* Simple TLV list buffer pointer */
  byte             offset,         /* byte offset of simple tlv within buffer */
  uim_tk_tlv_type* parsed_tlv_buff /* parsed simple tlv buffer */
);

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_TK_PROCESS_PROACTIVE_COMMAND

DESCRIPTION
  This procedure processes a proactive command received from the UIM card.

DEPENDENCIES
  This function needs an interface from the UI to process proactive commands 
  that apply to the UI.

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
extern void uim_tk_process_proactive_command
(
  uim_rsp_buf_type *rsp_ptr
);
#endif /* FEATURE_UIM_TOOLKIT_UTK */
#endif /* UIMTK_H */

