#ifndef AEESTK_H
#define AEESTK_H
/*======================================================
FILE:  AEESTK.h

SERVICES:  AEESTK member functions

GENERAL DESCRIPTION:

   ISTK, ISTKNotifier and related functionality

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/inc/AEESTK.h#16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/06   sk      Added newline character at EOF to remove warning
09/07/06   sk      Support for Send SS - CFU
06/09/06   sk      Added support for dcs in get input rsp
                   Enhancement for Send Command Response to support
                   new rsp structs and 4 bytes length fields.
04/04/06    sk     Added support for variable timeout and immediate digit
                   response for get inkey cmd.
04/04/06   sk      Added support for battery status
                   Added support for duration object in
                   display text cmd.
02/16/06   sk      Removed extra spaces from macro to fix compilation issues.
02/16/06   sk      Added Language Selection support
07/22/05   sk      Added Idle Screen support
06/16/05   tml     Linuxify fixes
=============================================================================*/


#include "AEE.h"
#include "AEEComdef.h"
#include "AEECM.h"
#include "STK.BID"



#define AEESTK_DURATION_MINUTES               0x00
#define AEESTK_DURATION_SECONDS               0x01
#define AEESTK_DURATION_TENTHS_OF_SECONDS     0x02

#define AEESTK_MAX_MENU_ITEMS                 50
#define AEESTK_MAX_ADDRESS_LEN                200
#define AEESTK_MAX_SUBADDR_LEN                20
#define AEESTK_MAX_PROVISIONING_FILE_REF      20

/* Values reflected the spec values */
#define AEESTK_7_BIT_DCS 0x00
#define AEESTK_8_BIT_DCS 0x04
#define AEESTK_UCS2_DCS  0x08

// Maximum number of items passed as additional info.

#define AEESTK_MAX_ADDITIONAL_INFO            10
// Maximum CCP 
#define AEESTK_MAX_CCP_LEN                    14 

#define AEESTK_LANG_CODE_LEN                  2
// The event mask that is sent with the EVT_NOTIFY
// The application need to register to STK_NOTIFIER with these masks

//Proactive Commands
#define NMASK_STKNOTIFIER_PROACTIVE_CMD       0x01 
//Envelope Commands
#define NMASK_STKNOTIFIER_ENVELOPE_CMD        0x02 

typedef struct
{
	 uint32 dwTransactionID;
	 uint32 dwCmdDataSize;
} AEESTKNotifierData;


// The Proactive requests ISTK sends to the registered clients 
typedef enum
{
  AEESTK_DISPLAY_TEXT_REQ         = 0x01,
  AEESTK_GET_INKEY_REQ,
  AEESTK_GET_INPUT_REQ,
  AEESTK_LAUNCH_BROWSER_REQ,
  AEESTK_MORE_TIME_REQ,
  AEESTK_PLAY_TONE_REQ,
  AEESTK_SELECT_ITEM_REQ,
  AEESTK_SEND_SMS_REQ,
  AEESTK_SEND_SS_REQ,
  AEESTK_SEND_USSD_REQ,
  AEESTK_SEND_DTMF_REQ,
  AEESTK_SETUP_CALL_REQ,
  AEESTK_SETUP_MENU_REQ,
  AEESTK_UPDATE_IDLE_TEXT_REQ,
  AEESTK_PROVIDE_LANG_INFO_REQ,
  AEESTK_LANG_NOTIFICATION_REQ, 
  AEESTK_POLL_INTERVAL_REQ,
  AEESTK_PROVIDE_LOCAL_INFO_REQ,
  AEESTK_REFRESH_REQ, 
  AEESTK_RUN_AT_CMDS_REQ,
  AEESTK_SETUP_EVENT_LIST_REQ,
  AEESTK_SETUP_IDLE_TEXT_REQ,
  AEESTK_OPEN_CH_REQ,
  AEESTK_CLOSE_CH_REQ,
  AEESTK_SEND_DATA_REQ,
  AEESTK_RECEIVE_DATA_REQ,
  AEESTK_GET_CH_STATUS_REQ,
  AEESTK_PROVIDE_BATTERY_STATUS_REQ,

  AEESTK_END_PROACTIVE_CMD_SES,
  AEESTK_PROACTIVE_CMD_SES_WITHOUT_TR,

  //Unused.
  AEESTK_MAX_PROACTIVE_REQ = 0xFFFFFFFF

} AEESTKProactiveCmdType;


//The Envelope requests ISTK sends to the registered cleints
typedef enum
{
  AEESTK_IDLE_SCREEN_REQ = 0x01,
  AEESTK_USER_ACTIVITY_REQ,
  AEESTK_LANG_SEL_REQ,
  AEESTK_DATA_AVAIL_REQ,
  AEESTK_CH_STATUS_REQ, 
  AEESTK_BROWSER_TERM_REQ,

  //Unused.
  AEESTK_EVENT_MAX_ENVELOPE_REQ = 0xFFFFFFFF
}AEESTKEnvelopeCmdType;



#define AEESTK_CMD_STK_REFRESH                0x01
#define AEESTK_CMD_STK_MORE_TIME              0x02
#define AEESTK_CMD_STK_POLL_INTERVAL          0x03
#define AEESTK_CMD_STK_POLLING_OFF            0x04
#define AEESTK_CMD_STK_SET_UP_EVENT_LIST      0x05
#define AEESTK_CMD_STK_SET_UP_CALL            0x10
#define AEESTK_CMD_STK_SEND_SS                0x11
#define AEESTK_CMD_STK_USSD                   0x12
#define AEESTK_CMD_STK_SEND_SHORT_MESSAGE     0x13
#define AEESTK_CMD_STK_SEND_DTMF              0x14
#define AEESTK_CMD_STK_LAUNCH_BROWSER         0x15
#define AEESTK_CMD_STK_PLAY_TONE              0x20
#define AEESTK_CMD_STK_DISPLAY_TEXT           0x21
#define AEESTK_CMD_STK_GET_INKEY              0x22
#define AEESTK_CMD_STK_GET_INPUT              0x23
#define AEESTK_CMD_STK_SELECT_ITEM            0x24
#define AEESTK_CMD_STK_SET_UP_MENU            0x25
#define AEESTK_CMD_STK_PROVIDE_LOCAL_INFO     0x26
#define AEESTK_CMD_STK_SET_UP_IDLE_MODE_TEXT  0x28
#define AEESTK_CMD_STK_RUN_AT_COMMAND         0x34
#define AEESTK_CMD_STK_END_OF_PROACTIVE_SES   0x81
#define AEESTK_CMD_STK_LANG_NOTIFICATION      0x35
#define AEESTK_CMD_STK_OPEN_CHANNEL           0x40
#define AEESTK_CMD_STK_CLOSE_CHANNEL          0x41
#define AEESTK_CMD_STK_RECEIVE_DATA           0x42
#define AEESTK_CMD_STK_SEND_DATA              0x43
#define AEESTK_CMD_STK_GET_CHANNEL_STATUS     0x44
#define AEESTK_CMD_STK_TIMER_MANAGEMENT       0x27
/* To be supported in future*/
#define AEESTK_CMD_STK_PERFORM_CARD_APDU      0x30
#define AEESTK_CMD_STK_POWER_ON_CARD          0x31
#define AEESTK_CMD_STK_POWER_OFF_CARD         0x32
#define AEESTK_CMD_STK_GET_READER_STATUS      0x33



/*===========================================================================
Enum: AEESTKResult

      Terminal Response result codes destined for the STK application
      (as defined in 11.14)
      Upper layer needs to specify this in the second parameter in the when 
      sending terminal response to indicate the status of the command
      being executed
===========================================================================*/
typedef enum {
    /* values based on Standard */
    AEESTK_RESULT_COMMAND_PERFORMED_SUCCESSFULLY                              = 0x00,
    AEESTK_RESULT_COMMAND_PERFORMED_WITH_PARTIAL_COMPREHENSION                = 0x01,
    AEESTK_RESULT_COMMAND_PERFORMED_WITH_MISSING_INFORMATION                  = 0x02,
    //AEESTK_RESULT_PCMD_REFRESH_PERFORMED_WITH_ADDITIONAL_EFS_READ           = 0x03,
    AEESTK_RESULT_COMMAND_SUCCESSFUL_BUT_REQUESTED_ICON_NOT_DISPLAYED         = 0x04,
    AEESTK_RESULT_COMMAND_PERFORMED_BUT_MODIFIED_BY_CALL_CONTROL_BY_SIM       = 0x05,

    AEESTK_RESULT_COMMAND_SUCCESSFUL_LIMITED_SERVICE                          = 0x06,
    AEESTK_RESULT_COMMAND_PERFORMED_WITH_MODIFICATION                         = 0x07,
    //AEESTK_RESULT_REFRESH_PERFORMED_BUT_USIM_NOT_ACTIVE                     = 0x08,

    AEESTK_RESULT_PROACTIVE_SIM_SESSION_TERMINATED_BY_USER                    = 0x10,
    AEESTK_RESULT_BACKWARD_MOVE_IN_SESSION_REQUESTED_BY_USER                  = 0x11,
    AEESTK_RESULT_NO_RESPONSE_FROM_USER                                       = 0x12,
    AEESTK_RESULT_HELP_INFORMATION_REQUIRED_BY_THE_USER                       = 0x13,

    AEESTK_RESULT_USSD_SS_TERMINATED_BY_THE_USER                              = 0x14,

    AEESTK_RESULT_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND                      = 0x20,
    AEESTK_RESULT_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND                 = 0x21,
    AEESTK_RESULT_USER_DID_NOT_ACCEPT_CALL_SET_UP_REQUEST                     = 0x22,
    AEESTK_RESULT_USER_CLEARED_DOWN_CALL_BEFORE_CONNECTION_OR_NETWORK_RELEASE = 0x23,
    AEESTK_RESULT_ACTION_IN_CONTRADICTION_WITH_THE_CURRENT_TIMER_STATE        = 0x24,
    AEESTK_RESULT_INTERACTION_WITH_CALL_CONTROL_BY_SIM_TEMPORARY_PROBLEM      = 0x25,

    AEESTK_RESULT_LAUNCH_BROWSER_GENERIC_ERROR                                = 0x26,

    AEESTK_RESULT_COMMAND_BEYOND_THE_ME_CAPABILITIES                          = 0x30,
    AEESTK_RESULT_COMMAND_TYPE_NOT_UNDERSTOOD_BY_ME                           = 0x31,
    AEESTK_RESULT_COMMAND_DATA_NOT_UNDERSTOOD_BY_ME                           = 0x32,
    AEESTK_RESULT_COMMAND_NUMBER_NOT_KNOWN_BY_ME                              = 0x33,
    AEESTK_RESULT_SS_RETURN_ERROR                                             = 0x34,
    AEESTK_RESULT_SMS_RP_ERROR                                                = 0x35,
    AEESTK_RESULT_ERROR_REQUIRED_VALUES_ARE_MISSING                           = 0x36,
    AEESTK_RESULT_USSD_RETURN_ERROR                                           = 0x37,
    AEESTK_RESULT_MULTIPLE_CARD_COMMANDS_ERROR                                = 0x38,
    AEESTK_RESULT_INTERACTION_WITH_CC_BY_SIM_OR_MO_SM_CONTROL_BY_SIM_PROBLEM  = 0x39,

    AEESTK_RESULT_BEARER_INDEPENDENT_PROTOCOL_ERROR                           = 0x3A, 
    
    AEESTK_RESULT_MAX                                                         = 0xFFFFFFFF
} AEESTKResult;

// The reason codes passes as additional information.
typedef enum {
    AEESTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN                             = 0x00,
    AEESTK_SCREEN_IS_BUSY                                             = 0x01,
    AEESTK_ME_CURRENTLY_BUSY_ON_CALL                                  = 0x02,
    AEESTK_ME_CURRENTLY_BUSY_ON_SS_TRANSACTION                        = 0x03,
    AEESTK_NO_SERVICE_AVAILABLE                                       = 0x04,
    AEESTK_ACCESS_CONTROL_CLASS_BAR                                   = 0x05,
    AEESTK_RADIO_RESOURCE_NOT_GRANTED                                 = 0x06,
    AEESTK_NOT_IN_SPEECH_CALL                                         = 0x07,
    AEESTK_ME_CURRENTLY_BUSY_ON_USSD_TRANSACTION                      = 0x08,
    AEESTK_ME_CURRENTLY_BUSY_ON_SEND_DTMF_COMMAND                     = 0x09,
    AEESTK_NO_USIM_ACTIVE                                             = 0x0A,
    AEESTK_ACTION_NOT_ALLOWED                                         = 0x11,
    AEESTK_REQUEST_CHANGED                                            = 0x12,
    AEESTK_BEARER_UNAVAILABLE                                         = 0x21,
    AEESTK_BROWSER_UNAVAILABLE                                        = 0x22,
    AEESTK_ME_UNABLE_TO_READ_PROVISIONING_DATA                        = 0x23,
    
    AEESTK_MAX_ADDITIONAL_INFO_CODE                                   = 0xFFFFFFFF
}
AEESTKAddionalInfoCode;

//Browser Launch modes.
typedef enum {
    /* values based on standard */
    AEESTK_LAUNCH_IF_NOT_ALREADY_LAUNCH   = 0x00,
    AEESTK_USE_EXISTING_BROWSER           = 0x02,
    AEESTK_CLOSE_EXISTING_BROWSER         = 0x03, 

    AEESTK_MAX_LAUNCH_BROWSER_MODE        = 0xFFFFFFFF
}
AEESTKLaunchModeType;

// Input format type 
typedef enum {
    AEESTK_SMS_DEF_ALPHABET               = 0x00,
    AEESTK_YES_NO                         = 0x01,
    AEESTK_NUMERICAL_ONLY                 = 0x02,
    AEESTK_UCS2_ALPHABET                  = 0x03,
    AEESTK_IMMEDIATE_DIGIT_RSP            = 0x04,
    AEESTK_YES_NO_AND_IMM_DGT_RSP         = 0x05,
    AEESTK_MAX_INPUT_FORMAT_MODE          = 0xFFFFFFFF
}
AEESTKInputFormatType;

// Tone types 
typedef enum {
    /* values based from Standard */
    AEESTK_DIAL_TONE                       = 0x01,
    AEESTK_CALLED_SUBSCRIBER_BUSY          = 0x02,
    AEESTK_CONGESTION                      = 0x03,
    AEESTK_RADIO_PATH_ACK                  = 0x04,
    AEESTK_RADIO_PATH_NOT_AVAIL_CALL_DROP  = 0x05,
    AEESTK_ERROR_TONE                      = 0x06,
    AEESTK_CALL_WAITING_TONE               = 0x07,
    AEESTK_RINGING_TONE                    = 0x08,
    AEESTK_GENERAL_BEEP                    = 0x10,
    AEESTK_POSITIVE_ACK_TONE               = 0x11,
    AEESTK_NEGATIVE_ACK_TONE               = 0x12,
    AEESTK_RINGING_TONE_SELECTED_BY_USER   = 0x13,
    AEESTK_SMS_ALERT_TONE_SELECTED_BY_USER = 0x14,
    /* to indicate it is optional */
    AEESTK_NOT_IN_USE                      = 0xAA,


    AEESTK_MAX_TONE_TYPE                   = 0xFFFFFFFF

}
AEESTKToneType;

typedef enum {
    AEESTK_PRESENTATION_NOT_SPECIFIED     = 0x00,
    AEESTK_DATA_VALUE_PRESENTATION        = 0x01,
    AEESTK_NAVIGATION_PRESENTATION        = 0x02,
    
    AEESTK_MAX_MENUP_PRESENT_TYPE         = 0xFFFFFFFF
}
AEESTKMenuPresentType;

// Call Setup request type
typedef enum {
    AEESTK_NO_OTHER_CALLS                 = 0x00,
    AEESTK_HOLD_ACTIVE_CALLS              = 0x01,
    AEESTK_DISCONNECT_ACTIVE_CALLS        = 0x02,
    
    AEESTK_MAX_CALL_SETUP_REQ_TYPE        = 0xFFFFFFFF
}
AEESTKCallSetupReqType;
// Call Type
typedef enum {
    AEESTK_VOICE                          = 0x00,
    AEESTK_SS                             = 0x01,
    AEESTK_USSD                           = 0x02,
    AEESTK_NONE                           = 0x03,


    AEESTK_MAX_nCallType                  = 0xFFFFFFFF
}
AEESTKCallType;


typedef enum {
    /* based on standard values */
    AEESTK_BCREPEAT_MODE_ALTERNATE     = 0x01,
    AEESTK_BCREPEAT_MODE_SEQUENTIAL    = 0x03,

    AEESTK_MAX_BC_REPEAST_TYPE         = 0xFFFFFFFF
}
AEESTKBCRepeatIndType;

typedef enum {
    /* based on standard values */
    AEESTK_BEARER_SMS     = 0x00,
    AEESTK_BEARER_CSD     = 0x01,
    AEESTK_BEARER_USSD    = 0x02,
    AEESTK_BEARER_GPRS    = 0x03,
    AEESTK_BEARER_DEFAULT = 0x04,
    AEESTK_BEARER_RFU     = 0xFF, /* 0x05 - 0xFF = RFU */

    AEESTK_MAX_BEARER_CODE_TYPE = 0xFFFFFFFF
}
AEESTKBearerCodeType;

typedef enum {
    AEESTK_CH_STATUS_NO_FURTHER_INFO           = 0x00,
    AEESTK_CH_STATUS_LINK_DROPPED,
    AEESTK_CH_STATUS_ADDITIONAL_INFO_NOT_USED,
    AEESTK_CH_STATUS_ADDITIONAL_INFO_RFU,
    AEESTK_CH_STATUS_ADDITIONAL_INFO_MAX       = 0xFFFF
}
AEESTKChStAdditionalInfoType;

typedef enum {
  AEESTK_CSD_BEARER_SPEED_AUTO             = 0,	
  AEESTK_CSD_BEARER_SPEED_300_V21,
  AEESTK_CSD_BEARER_SPEED_1200_V22,
  AEESTK_CSD_BEARER_SPEED_1200_75_V23,
  AEESTK_CSD_BEARER_SPEED_2400_V22,
  AEESTK_CSD_BEARER_SPEED_2400_V26,
  AEESTK_CSD_BEARER_SPEED_4800_V32,
  AEESTK_CSD_BEARER_SPEED_9600_V32,
  AEESTK_CSD_BEARER_SPEED_9600_V34,
  AEESTK_CSD_BEARER_SPEED_14400_V34,
  AEESTK_CSD_BEARER_SPEED_19200_V34,
  AEESTK_CSD_BEARER_SPEED_28800_V34,
  AEESTK_CSD_BEARER_SPEED_33600_V34,
  AEESTK_CSD_BEARER_SPEED_1200_V120,
  AEESTK_CSD_BEARER_SPEED_2400_V120,
  AEESTK_CSD_BEARER_SPEED_4800_V120,
  AEESTK_CSD_BEARER_SPEED_9600_V120,
  AEESTK_CSD_BEARER_SPEED_14400_V120,
  AEESTK_CSD_BEARER_SPEED_19200_V120,
  AEESTK_CSD_BEARER_SPEED_28800_V120,
  AEESTK_CSD_BEARER_SPEED_38400_V120,
  AEESTK_CSD_BEARER_SPEED_48000_V120,
  AEESTK_CSD_BEARER_SPEED_56000_V120,
  AEESTK_CSD_BEARER_SPEED_300_V110,
  AEESTK_CSD_BEARER_SPEED_1200_V110,
  AEESTK_CSD_BEARER_SPEED_2400_V110_X31,
  AEESTK_CSD_BEARER_SPEED_4800_V110_X31,
  AEESTK_CSD_BEARER_SPEED_9600_V110_X31,
  AEESTK_CSD_BEARER_SPEED_14400_V110_X31,
  AEESTK_CSD_BEARER_SPEED_19200_V110_X31,
  AEESTK_CSD_BEARER_SPEED_28800_V110_X31,
  AEESTK_CSD_BEARER_SPEED_38400_V110_X31,
  AEESTK_CSD_BEARER_SPEED_48000_V110_X31,
  AEESTK_CSD_BEARER_SPEED_56000_V110_X31,
  AEESTK_CSD_BEARER_SPEED_64000_V110_X31,
  AEESTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT,
  AEESTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT,
  AEESTK_CSD_BEARER_SPEED_32000_PIAFS32K,
  AEESTK_CSD_BEARER_SPEED_64000_PIAFS32K,
  AEESTK_CSD_BEARER_SPEED_28800_MULTIMEDIA,
  AEESTK_CSD_BEARER_SPEED_32000_MULTIMEDIA,
  AEESTK_CSD_BEARER_SPEED_33600_MULTIMEDIA,
  AEESTK_CSD_BEARER_SPEED_56000_MULTIMEDIA,
  AEESTK_CSD_BEARER_SPEED_64000_MULTIMEDIA,
  AEESTK_CSD_BEARER_SPEED_MAX                     = 0xFFFF,
}
AEESTKBearerCSDSpeedType;

typedef enum
{
  AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ = 0,
  AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ,
  AEESTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI,
  AEESTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI,
  AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI,
  AEESTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI,
  AEESTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI,
  AEESTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI,
  AEESTK_CSD_BEARER_NAME_MAX                            = 0xFFFF
}
AEESTKBearerCSDNameType;

typedef enum
{
  AEESTK_CSD_BEARER_CE_TRANSPARENT                    = 0,
  AEESTK_CSD_BEARER_CE_NON_TRANSPARENT,
  AEESTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF,
  AEESTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF,
  AEESTK_CSD_BEARER_CE_MAX                            = 0xFFFF,
}
AEESTKBearerCSDConnectionElementType;

typedef enum {
  AEESTK_PKT_DATA_IP                              = 0x00,
  AEESTK_PKT_DATA_PROTOCOL_MAX                    = 0xFFFF
}
AEESTKPacketDataProtocolType;

typedef enum {
  AEESTK_PKT_DATA_NO_ADDRESS_GIVEN,
  AEESTK_PKT_DATA_DYNAMIC_ADDRESS,
  AEESTK_PKT_DATA_IPV4_ADDRESS,
  AEESTK_PKT_DATA_IPV6_ADDRESS,
  AEESTK_PKT_DATA_ADDRESS_MAX                     = 0xFFFF
}
AEESTKPacketDataAddressCategoryType;

typedef enum {
  AEESTK_PKT_DATA_NOT_PRESENT,
  AEESTK_PKT_DATA_UDP,
  AEESTK_PKT_DATA_TCP,
  AEESTK_PKT_DATA_TRANSPORT_LEVEL_MAX              = 0xFFFF
}
AEESTKPacketDataTransportLevelCategoryType;

/* As per spec 102.223 */
typedef enum
{
   AEESTK_BATTERY_LEVEL_0       = 0x00, /* Battery status - very low */
   AEESTK_BATTERY_LEVEL_1       = 0x01, /* Battery status - low */
   AEESTK_BATTERY_LEVEL_2       = 0x02, /* Battery status - average */
   AEESTK_BATTERY_LEVEL_3       = 0x03, /* Battery status - good */
   AEESTK_BATTERY_LEVEL_4       = 0x04, /* Battery status - full */
   AEESTK_BATTERY_LEVEL_NA      = 0xFE, /* Battery status - status not   
                                           applicable */
   AEESTK_BATTERY_LEVEL_UNKNOWN = 0xFF  /* Battery status - not known  
                                           Eg.battery charging */
}
AEESTKBatteryStatusType;

/*===========================================================================
 AEESTKChStatusType: Uses to pass channel status information
===========================================================================*/
typedef struct {
    int32                                nChID;
    boolean                              bIsLinkEst;
    AEESTKChStAdditionalInfoType         ChStatusAdditionalInfo;
}
AEESTKChStatusType;


/*===========================================================================
 AEESTKBearerCSDParamType: bearer parameter information for CSD
===========================================================================*/
typedef struct {
  AEESTKBearerCSDSpeedType             Speed;
  AEESTKBearerCSDNameType              Name;
  AEESTKBearerCSDConnectionElementType ConnectionElement;  
}
AEESTKBearerCSDParamType;


/*===========================================================================
 AEESTKBearerGPRSParamType: Bearer parameter information for GPRS     
===========================================================================*/
typedef struct {
  int32                                nPrecedenceCls;
  int32                                nDelayCls;
  int32                                nReliabilityCls; 
  int32                                nPeakThroughput; 
  int32                                nMeanThroughput; 
  AEESTKPacketDataProtocolType         PktDataProtocol; 
}
AEESTKBearerGPRSParamType;

/*===========================================================================
 AEESTKBearerDescriptionType: Bearer type and bearer parameters
      BearerType:  AEESTK_CSD, AEESTK_GPRS or AEESTK_BEARER_DEFAULT 
        AEESTK_CSD: BearerParams.BearerCSD should be looked at in establishing the 
                  bearer for the transport protocol
        AEESTK_GPRS: BearerParams.BearerGPRS should be looked at in establishing the 
                   bearer for the transport protocol
        AEESTK_BEARER_DEFAULT: mobile should use default bearer for thet transport 
                      protocol specified.  There is no member in the union that
                      mobile needs to look at in related to the bearer
===========================================================================*/
typedef struct {
  AEESTKBearerCodeType     BearerType;
  union {
    AEESTKBearerCSDParamType   BearerCSD;
    AEESTKBearerGPRSParamType  BearerGPRS;
  } BearerParams;
}
AEESTKBearerDescriptionType;


/*===========================================================================
 AEESTKDataBufferType: Generic data buffer that contains int32 for Length and
                       byte pointer for the data
===========================================================================*/
typedef struct {
  int32                        nLength;
  uint8                        *pData;
}
AEESTKDataBufferType;

/*===========================================================================
 AEESTKPacketDataAddressType: Packet data address information
      AddressType:  AEESTK_PKT_DATA_DYNAMIC_ADDRESS 
                      => (no info for sPktAddress)
                    AEESTK_PKT_DATA_IPV4_ADDRESS,
                      => (sPktAddress: 4 bytes IP4 address will be given with 
                          Bit 8 of first byte the most significant bit of the 
                          IP address)
                    AEESTK_PKT_DATA_IPV6_ADDRESS
                      => (sPktAddress: 16 bytes IP6 address will be given with 
                          Bit 8 of first byte the most significant bit of the 
                          IP address)
                    AEESTK_PKT_DATA_NO_ADDRESS_GIVEN
===========================================================================*/
typedef struct {
  AEESTKPacketDataAddressCategoryType PktAddressType;
  int32                               nPktAddressLength;
  uint8                              *pPktAddress; 
}
AEESTKPacketDataAddressType;



/*===========================================================================
 AEESTKPacketDataTransportLevelType: Transport protocol and port number
      TransportProtocol: AEESTK_PKT_DATA_UDP or AEESTK_PKT_DATA_TCP,
                         AEESTK_PKT_DATA_NOT_PRESENT, 
                         AEESTK_PKT_DATA_MAX (dwPortNumber will be invalid in
                         this case)
===========================================================================*/
typedef struct {
  AEESTKPacketDataTransportLevelCategoryType   TransportProtocol;
  uint32                                       dwPortNumber;  
}
AEESTKPacketDataTransportLevelType;


/*============================================================================
  structute definitions
=============================================================================*/

/*============================================================================
 AEESTKDuration : used with Redial and Play Tone requests.
=============================================================================*/

typedef struct {
    boolean                      bPresent;
    uint8                        nUnits;
    uint8                        nLength;
}
AEESTKDuration;

/*============================================================================
 AEESTKString: Used to pass strings to registerd clients e.g. alpha, SS, USSD,
               Display string requests. (This is now a common structure which
	       also accounts for the previously defined AEESTKItemObject
               struct.
=============================================================================*/
typedef struct {
    uint32			 nLength;
    AECHAR*                      pwszText;
}
AEESTKString;

/*===========================================================================
 AEESTKIcon: Used to pass icon information registerd clients 
=============================================================================*/
typedef struct {
    boolean                       bPresent;
    uint8                         nQualifier;
    uint8                         nHeight;
    uint8                         nWidth;
    uint32                        nSize;
    uint8                         *pData;
}
AEESTKIcon;

/*===========================================================================
 AEESTKUrl: Passed with Launch Browser Request.
=============================================================================*/
typedef struct {
    uint8                        nLength;
    uint8                        *pURL;
}
AEESTKUrl;

/*============================================================================
 AEESTKBearer: 
=============================================================================*/

typedef struct {
    uint8                       nLength;
    AEESTKBearerCodeType        *pBearerList;
}
AEESTKBearer;

/*============================================================================
 AEESTKFileList: Passed with Launch Browser request. 
=============================================================================*/
typedef struct {
    uint8                        nLength;
    uint8                        nNumFiles;
    uint8                        *pFilePaths;
}
AEESTKFileList;

/*============================================================================
 AEESTKNextItemInd: Passed with Menu related requests. 
=============================================================================*/
typedef struct {
    uint8                        nNumItems;
    uint8                        *pNextActionList;
}
AEESTKNextItemInd;

/*============================================================================
 AEESTKIconIdList: Passed with Menu related requests. 
=============================================================================*/
typedef struct {
    boolean                     bDisplayIconOnly;
    uint8                       nNumItems;
    AEESTKIcon                  *pIconList[AEESTK_MAX_MENU_ITEMS];
}
AEESTKIconIdList;

/*===========================================================================
 AEESTKSMSTPDU: Passed with Send SMSrequest. 
=============================================================================*/
typedef struct {
    uint8                        nLength;
    uint8                        *pTPDU;
}
AEESTKSMSTPDU;

/*===========================================================================
 AEESTKAddress: Passed with Call/SMS related requests. 
=============================================================================*/
typedef struct {
    AEECMNumberType             TON;
    AEECMNumberPlan             NPI;
    uint8                       nLength;
    uint8                       address[AEESTK_MAX_ADDRESS_LEN];
}
AEESTKAddress;

/*===========================================================================
 AEESTKRedial: Passed with setup call reuqest
=============================================================================*/
typedef struct {
    boolean                     bNecessary;    /* from command detail */
    /* Optional */
    AEESTKDuration              timeout;      /* from optional Duration TLV */
}
AEESTKRedial;

/*===========================================================================
 AEESTKSubAddress: Passed with setup call/ CC reuqests.
=============================================================================*/
typedef struct {
    uint8                        nLength;
    uint8                        subaddress[AEESTK_MAX_SUBADDR_LEN];
}
AEESTKSubAddress;

/*============================================================================
 AEESTKCapConfig: 
=============================================================================*/
typedef struct {
    uint8                        nLength;
    uint8                        ccp[AEESTK_MAX_CCP_LEN];
}
AEESTKCapConfig;


/*============================================================================
 AEESTKAdditionalInfo: 
=============================================================================*/
typedef struct {
    uint8                       nLength;
    AEESTKAddionalInfoCode      additionalInfo[AEESTK_MAX_ADDITIONAL_INFO];
    uint8                      *pAdditionalInfoData;
}
AEESTKAdditionalInfo;



/*============================================================================
 AEESTKProactiveCmdHeader: 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdType      nCommandID;
    uint8                       nCommandNumber;
}
AEESTKProactiveCmdHeader;

/*============================================================================
 AEESTKCCVoice: 
=============================================================================*/
typedef struct {
    AEESTKAddress               address;
    /* Optional */
    AEESTKSubAddress            subaddress;
    AEESTKCapConfig             ccp1;
    AEESTKCapConfig             ccp2;
}
AEESTKCCVoice;


/*============================================================================
 AEESTKCCReqAction: 
=============================================================================*/
typedef struct {
    uint8                       nLength;

    /* Optional */
    AEESTKCallType              nCallType;
    union {
        AEESTKCCVoice           voiceInfo;
        AEESTKAddress           ssString;
        AEESTKString            ussdString;
    }addressString;
    AEESTKString                alpha;
    AEESTKBCRepeatIndType       bcRepeatInd;
    /* section 7.3.1.6 response data */
}
AEESTKCCReqAction;


/*============================================================================
 AEESTKCallSSResponse: 
=============================================================================*/
typedef struct {
    boolean                     bCCModification;
    AEESTKCCReqAction           ccReqAction;
    AEESTKResult                nCommandResult2;
    AEESTKAdditionalInfo        additionalInfo2;
    AEESTKString                textString2;
}
AEESTKCallSSResponse;

/*============================================================================
 AEESTKUSSDResponse: 
=============================================================================*/
typedef struct {
    boolean                     bCCModification;
    uint8                       nDCSFromNW;
    AEESTKString                data;
    AEESTKCCReqAction           ccReqAction;
    AEESTKResult                nCommandResult2;
    AEESTKAdditionalInfo        additionalInfo2;
}
AEESTKUSSDResponse;

typedef enum
{
  AEESTK_INKEY_INPUT_IS_SINGLE_CHAR,
  AEESTK_INKEY_INPUT_IS_YES,
  AEESTK_INKEY_INPUT_IS_NO
}
AEESTKInkeyResponseType;

/*===========================================================================
 AEESTKInkeyResponse
===========================================================================*/
typedef struct {
     AEESTKInkeyResponseType    ResponseType;
     AECHAR                     *pszText;
     AEESTKDuration             TotalDisplayDuration; 
}
AEESTKInkeyResponse;

/*===========================================================================
 AEESTKGetInputResponse
===========================================================================*/
typedef struct
{  
    AEESTKString           pString; /* user input text string and length  */
    AEESTKInputFormatType  dcs;     /* input rsp format from card Eg.UCS2 */
}
AEESTKGetInputResponse;

/*===========================================================================
 AEESTKOpenChResponse
===========================================================================*/
typedef struct {
  int32                                nChID;
  boolean                              bIsLinkEst;
  AEESTKChStAdditionalInfoType         ChStatusAdditionalInfo;
  AEESTKBearerDescriptionType          BearerDescription;
  int32                                nBufferSize;
}
AEESTKOpenChResponse;

/*===========================================================================
 AEESTKReceiveDataResponse
 ===========================================================================*/
typedef struct {
  int32                         nLength;
  uint8                        *pData;
  int32                         nChDataRemainingLen;                 
}
AEESTKReceiveDataResponse;


/*===========================================================================
 Send Data Response: int32 Channel Data Remaining Empty Bytes
===========================================================================*/



/*===========================================================================
 AEESTKGetChStatusResponse
===========================================================================*/
typedef struct {
  int32                                nChID;
  boolean                              bIsLinkEst;
  AEESTKChStAdditionalInfoType         ChStatusAdditionalInfo;
}
AEESTKGetChStatusResponse;

/*===========================================================================
 AEESTKLangSelResponse
===========================================================================*/
typedef struct {
  int32                                len;
  uint8                                *sLangCode ;
}
AEESTKLangSelectionResponse;

typedef struct {
  int32                                nChID;
  boolean                              bIsLinkEst;
  AEESTKChStAdditionalInfoType         ChStatusAdditionalInfo;
  int32                                nChLen;
}
AEESTKDataAvailNotify;

typedef enum {
  AEESTK_BROWSER_TERM_USER_TERMINATED    = 0x00,
  AEESTK_BROWSER_TERM_ERROR_TERMINATED   = 0x01,
  AEESTK_BROWSER_TERM_CAUSE_MAX          = 0xFFFF
}
AEESTKBrowserTermEnumType;



/*============================================================================
 AEESTKDisplayTextReq: 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    AEESTKString                textString;
    boolean                     bHighPriority;
    boolean                     bUserControl;
    /* Optional */
    AEESTKIcon                  icon;
    boolean                     bImmediateRespReq;
    AEESTKDuration              bDurationOfDisplay;
}
AEESTKDisplayTextReq;


/*============================================================================
 AEESTKGetInkeyReq: 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    AEESTKString                textString;
    AEESTKInputFormatType       responseFormat;
    boolean                     bHelpAvaliable;
    /* Optional */
    AEESTKIcon                  icon;
    AEESTKDuration              variableTimeout;
}
AEESTKGetInkeyReq;



/*============================================================================
 AEESTKGetInputReq: 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    AEESTKString                textString;
    AEESTKInputFormatType       responseFormat;
    uint8                       nMaxUserInput;
    uint8                       nMinUserInput;
    boolean                     bHelpAvaliable;
    boolean                     bShowUserInput;
    /* Optional */
    AEESTKString                defaultText;
    AEESTKIcon                  icon;
}
AEESTKGetInputReq;




/*============================================================================
 AEESTKLaunchBrowserReq: 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    AEESTKLaunchModeType        nLaunchMode;
    AEESTKUrl                   url;
    /* Optional */
    uint8                       nBrowserID;
    AEESTKBearer                bearer;
    uint32                      nNumProvisioningFiles;
    AEESTKFileList              provisioningFileRef[AEESTK_MAX_PROVISIONING_FILE_REF];
    AEESTKString                gatewayProxyTextString;
    AEESTKString                UserConfirmAlpha;
    AEESTKIcon                  UserConfirmIcon;
}
AEESTKLaunchBrowserReq;


/*============================================================================
 AEESTKPlayToneReq: 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    /* Optional */
    AEESTKString                alpha;
    AEESTKToneType              nTone;
    AEESTKDuration              duration;
    AEESTKIcon                  icon;
}
AEESTKPlayToneReq;



/*============================================================================
 AEESTKSelectItemReq: 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    boolean                     bHelpAvaliable;
    AEESTKMenuPresentType       presentation;
    boolean                     bSoftkeySelection;
    uint8                       nNumItems;
    AEESTKString                items[AEESTK_MAX_MENU_ITEMS];
    /* Optional */
    AEESTKString                title;
    uint8                       nDefaultItem;  /* optional if value set to 0xFF -> okay???? */
    AEESTKNextItemInd           nextActionIndicator;
    AEESTKIcon                  icon;
    AEESTKIconIdList            iconIdList;
}
AEESTKSelectItemReq;



/*===========================================================================
--  Struct: AEESTKSendSMSReq
===========================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    boolean                     bPackingRequired;
    AEESTKSMSTPDU               smsTPDU;
    boolean                     bIsCDMASMS;
    /* Optional */
    AEESTKAddress               address;
    AEESTKString                alpha;
    AEESTKIcon                  icon;
}
AEESTKSendSMSReq;



/*===========================================================================
 AEESTKSendSSReq
===========================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    AEESTKAddress               ssString;
    /* Optional */
    AEESTKString                alpha;
    AEESTKIcon                  icon;
}
AEESTKSendSSReq;


/*===========================================================================
 AEESTKSendUSSDReq
===========================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    uint8                       nDCSFromSIM;
    AEESTKString                ussdString;
    /* Optional */
    AEESTKString                alpha;
    AEESTKIcon                  icon;
}
AEESTKSendUSSDReq;



/*===========================================================================
 AEESTKSetupCallReq

      callSetupRequirement:  hold_active_calls,
                               terminate_active_calls,
                               no_other_calls are exclusive
      redial:  redial required or not is mandatory redial duration
               is optional
===========================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    AEESTKCallSetupReqType      callSetupRequirement;
    AEESTKAddress               address;
    AEESTKRedial                redial;
    /* Optional */
    AEESTKString                userCnfAlpha;
    AEESTKString                setupCallDisplayAlpha;
    AEESTKIcon                  userCnfIcon;
    AEESTKIcon                  setupCallDisplayIcon;
    AEESTKSubAddress            destinationSubAddress;
    AEESTKCapConfig             capabilityConfig;
}
AEESTKSetupCallReq;

/*===========================================================================
  AEESTKSetupIdleModeTextReq
===========================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    AEESTKString                string;
    /* Optional */
    AEESTKIcon                  icon;
}
AEESTKSetupIdleModeTextReq;

/*===========================================================================
 AEESTKMoreTimeReq
===========================================================================*/
typedef struct
{
  AEESTKProactiveCmdHeader    cmdHdr;
}
AEESTKMoreTimeReq;

/*===========================================================================
 AEESTKEndProactiveCmdReq
===========================================================================*/
typedef struct
{
  AEESTKProactiveCmdHeader    cmdHdr;
  boolean                     bEndFromCard;
}
AEESTKEndProactiveCmdReq;

/*===========================================================================
 AEESTKProvideLangReq
===========================================================================*/
typedef struct
{
  AEESTKProactiveCmdHeader    cmdHdr;
}
AEESTKProvideLangReq;

/*===========================================================================
 AEESTKLangInfoReq
===========================================================================*/
typedef struct
{
  AEESTKProactiveCmdHeader    cmdHdr;
  boolean                     bSpecLangRequest;
  uint8                       nLangCode[AEESTK_LANG_CODE_LEN];
}
AEESTKLangInfoReq;

/*===========================================================================
 AEESTKSetupMenuReq
===========================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
    boolean                     bHelp;
    boolean                     bSoftkeySelection;
    uint8                       nItems;
    AEESTKString                items[AEESTK_MAX_MENU_ITEMS];
    /* Optional */
    AEESTKString                title;
    uint8                       nDefaultItem;
    AEESTKNextItemInd           nextActionIndicator;
    AEESTKIcon                  icon;
    AEESTKIconIdList            iconIdList;
}
AEESTKSetupMenuReq;

/*============================================================================
 AEESTKSendDTMFReq 
=============================================================================*/
typedef struct {
  AEESTKProactiveCmdHeader    cmdHdr;
  AEESTKString alpha;
  AEESTKIcon  icon;
  uint8       nLength;
  uint8       *pDTMFString;
} AEESTKSendDTMFReq;

/*============================================================================
 AEESTKOpenChReq
=============================================================================*/
typedef struct {
  AEESTKProactiveCmdHeader           cmdHdr;  
  boolean                            bOnDemandLinkEst;
  AEESTKString                       sAlpha;
  AEESTKIcon                         sIcon;
  AEESTKBearerDescriptionType        sBearer;
  int32                              nBufferSize;
  AEESTKString                       sAccessPointName;
  AEESTKPacketDataAddressType        sLocalAddress;
  AEESTKPacketDataTransportLevelType sTransportLevel;
  AEESTKPacketDataAddressType        sDataDestAddress;
  AEESTKString                       sUserLogin;
  AEESTKString                       sUserPassword;
} AEESTKOpenChReq;

/*============================================================================
 AEESTKCloseChReq
=============================================================================*/
typedef struct {
  AEESTKProactiveCmdHeader           cmdHdr;  
  int32                              nChID;
  AEESTKString                       sAlpha;
  AEESTKIcon                         sIcon;
} AEESTKCloseChReq;

/*============================================================================
 AEESTKReceiveDataReq 
=============================================================================*/
typedef struct {
  AEESTKProactiveCmdHeader           cmdHdr;  
  int32                              nChID;
  int32                              nChDataLength;
  AEESTKString                       sAlpha;
  AEESTKIcon                         sIcon;
} AEESTKReceiveDataReq;

/*============================================================================
 AEESTKSendDataReq 
=============================================================================*/
typedef struct {
  AEESTKProactiveCmdHeader           cmdHdr;  
  int32                              nChID;
  boolean                            bSendDataImmediately;
  AEESTKDataBufferType               sChData;
  AEESTKString                       sAlpha;
  AEESTKIcon                         sIcon;
} AEESTKSendDataReq;

/*============================================================================
 AEESTKGetChStatusReq 
=============================================================================*/
typedef struct {
  AEESTKProactiveCmdHeader           cmdHdr;  
} AEESTKGetChStatusReq;

/*============================================================================
 AEESTKProvideBatteryStatusReq 
=============================================================================*/
typedef struct {
    AEESTKProactiveCmdHeader    cmdHdr;
}
AEESTKProvideBatteryStatusReq;

/*============================================================================
 AEESTKEventDLInd: 
=============================================================================*/

typedef struct {
    AEESTKEnvelopeCmdType       commandID;
}
AEESTKEventDLInd;

/*============================================================================
 AEESTKEnvelopeCmd: 
=============================================================================*/
typedef struct {
  AEESTKEnvelopeCmdType         commandID;
 }
AEESTKEnvelopeNotif;

/*============================================================================
 AEESTKEnvelopeRsp
=============================================================================*/
typedef struct {
  AEESTKEnvelopeCmdType         commandID;
  void                          *pResponse;
} 
AEESTKEnvelopeRsp;


typedef struct ISTK ISTK;    
typedef struct ISTKNotifier ISTKNotifier;

AEEINTERFACE(ISTK)
{
  INHERIT_IQueryInterface(ISTK);

  int (*GetCmdData)
                (
                  ISTK                      *pISTK, 
                  uint32                    dwTransactionID, 
                  void                      *pCmdBuf, 
                  uint32                    *pBufLen
                );
  
  int (*SendCommandResponse)
                (
                  ISTK                      *pISTK,
                  AEESTKProactiveCmdHeader  cmdHdr,
                  AEESTKResult              result,						  
                  AEESTKAdditionalInfo      *additionalResult,						  
                  void                      *extra_param,
		  uint8			    extra_param_length
                );

  /* Enhanced SendCommandResponse to support new response structures and 4 bytes 
     extra param length 
  */

  int (*SendCommandResponseEx)
                (
                  ISTK                      *pISTK,
                  AEESTKProactiveCmdHeader  cmdHdr,
                  AEESTKResult              result,						  
                  AEESTKAdditionalInfo      *additionalResult,						  
                  void                      *extra_param
                );

  int (*SendEnvelopeEvtDownloadCommand)
                (
                  ISTK                      *pISTK,	
	                uint16                    *pwTransactionID,
	                uint32                    dwSize,
                  AEESTKEventDLInd          *evtList
                );
  int (*SendEnvelopeMenuSelCommand)
                (
                  ISTK		                  *pISTK,
	                uint16                    *pwTransactionID,
                  byte                      nItemChosen,                          
                  boolean                   bHelp);
  int (*UserActivity)
                (
                  ISTK                      *pISTK);
  int (*ChannelStatusChange)
                (
                  ISTK                        *pISTK,
                  AEESTKGetChStatusResponse    sChStatus);
  int (*DataAvailable)
                (
                  ISTK                      *pISTK,
                  AEESTKDataAvailNotify      sDataAvail);
  int (*BrowserTermination)
                (
                  ISTK                      *pISTK,
                  AEESTKBrowserTermEnumType sBrowserTerm);
  int(*IdleScreen)(ISTK *pISTK);
  int(*LangSelection)
                (
                   ISTK                        *pISTK,
                   AEESTKLangSelectionResponse *sLangSelection);

};

AEEINTERFACE(ISTKNotifier)
{
  INHERIT_INotifier(ISTKNotifier);
};


#define ISTK_AddRef(p)                 AEEGETPVTBL(p, ISTK)->AddRef(p)

#define ISTK_Release(pISTK)            AEEGETPVTBL(pISTK, ISTK)->Release(pISTK)

#define ISTK_QueryInterface(pISTK, clsid, pp)   \
          AEEGETPVTBL(pISTK, ISTK)->QueryInterface(pISTK, clsid, pp)

#define ISTK_GetCmdData(pISTK, nCmdNum, pCmdBuf, nBufLen) \
          AEEGETPVTBL(pISTK, ISTK)->GetCmdData(pISTK, nCmdNum, pCmdBuf, nBufLen)

#define ISTK_SendCommandResponse(pISTK, cmdHdr, result, additionalResult, other, extra_param_length) \
          AEEGETPVTBL(pISTK, ISTK)->SendCommandResponse(pISTK, cmdHdr, result, additionalResult, other, extra_param_length)

#define ISTK_SendCommandResponseEx(pISTK, cmdHdr, result, additionalResult, other) \
          AEEGETPVTBL(pISTK, ISTK)->SendCommandResponseEx(pISTK, cmdHdr, result, additionalResult, other)
          
#define ISTK_SendEnvelopeEvtDownloadCommand(pISTK, pwTransactionID, dwSize, evtList) \
          AEEGETPVTBL(pISTK, ISTK)->SendEnvelopeEvtDownloadCommand(pISTK, pwTransactionID, dwSize, evtList)

#define ISTK_SendEnvelopeMenuSelCommand(pISTK, pwTransactionID, nItemChosen, bHelp)\
          AEEGETPVTBL(pISTK, ISTK)->SendEnvelopeMenuSelCommand(pISTK, pwTransactionID, nItemChosen, bHelp)
#define ISTK_UserActivity(pISTK)\
          AEEGETPVTBL(pISTK, ISTK)->UserActivity(pISTK)
#define ISTK_IdleScreen(pISTK)\
          AEEGETPVTBL(pISTK, ISTK)->IdleScreen(pISTK)
#define ISTK_LangSelection(pISTK, sLangSelection)\
          AEEGETPVTBL(pISTK, ISTK)->LangSelection(pISTK,sLangSelection)

#define ISTK_DataAvailable(pISTK,sDataAvail)\
          AEEGETPVTBL(pISTK, ISTK)->DataAvailable(pISTK,sDataAvail)
          
#define ISTK_ChannelStatusChange(pISTK,sChStatus)\
          AEEGETPVTBL(pISTK, ISTK)->ChannelStatusChange(pISTK,sChStatus)

#define ISTK_BrowserTermination(pISTK,sBrowserTerm)\
          AEEGETPVTBL(pISTK, ISTK)->BrowserTermination(pISTK,sBrowserTerm)

/*=============================================================================
FUNCTION: ISTK_GetCmdData

DESCRIPTION:  
  Get the new command in the buffer provided

PROTOTYPE:
      ISTK_GetCmdData
        (
          ISTK                      *pISTK, 
          uint32                    dwTransactionID, 
          void                      *pCmdBuf, 
          uint32                    *pBufLen
        );
PARAMETERS:
  pISTK             [in]:  ISTK interface pointer.
  dwTransactionID   [in]:  The transaction ID received with the NMASK_STKNOTIFIER_xxx event
  *pCmdBuf          [out]: Pointer to the buffer where the command data will be filled in. 
  *pBufLen          [in / out]: The length of the buffer provided/ filled in by ISTK. 

RETURN VALUE: 
  Returns AEE return values.

  AEE_SUCCESS:  If the command was successfully filled in the buffer provided.
  EBADPARM:     If pBufLen is NULL, or transaction ID is invalid or the buffer provided
                is insufficient.
  EFAILED:      Otherwise.

COMMENTS:
  The clients for the ISTKNotifer receive notifications with one of the following masks
  NMASK_STKNOTIFIER_PROACTIVE_CMD or
  NMASK_STKNOTIFIER_ENVELOPE_CMD

  whenever there is a new command available. 

  The dwParam which is a pointer to AEENotify structure has a pData pointer which is actually
  a 32 bit transaction id, namely dwTransactionID. The lower 16 bits of this transaction id 
  contain the command type (AEESTKProactiveCmdType/ AEESTKEnvelopeCmdType). If the application 
  is interested to know the details for that command, it will have to allocate the buffer for 
  that command and then get this buffer filled in by the interface. 
  
  If the third parameter, pCmdBuf is NULL, the interface just fills in the size of the buffer
  required in the fourth parameter (pBufLen). So, to get the size of the buffer, call
  ISTK_GetCmdBuf(pISTK, dwTransactionID, NULL, &nLen). Now application can allocate a buffer and
  call the same function again to get the  buffer filled in by the interface.

  The following types of commands are supported for   NMASK_STKNOTIFIER_PROACTIVE_CMD.
 
   Command ID (dwTransactionID & 0x0000FFFF)   Type of structure returned.
   ====================================================================================
  (1) AEESTK_DISPLAY_TEXT_REQ                   AEESTKDisplayTextReq      
  (2) AEESTK_GET_INKEY_REQ                      AEESTKGetInkeyReq
  (3) AEESTK_GET_INPUT_REQ                      AEESTKGetInputReq     
  (4) AEESTK_LAUNCH_BROWSER_REQ                 AEESTKLaunchBrowserReq  
  (5) AEESTK_MORE_TIME_REQ                      AEESTKMoreTimeReq
  (6) AEESTK_PLAY_TONE_REQ                      AEESTKPlayToneReq
  (7) AEESTK_SELECT_ITEM_REQ                    AEESTKSelecteItemReq 
  (8) AEESTK_SEND_SMS_REQ                       AEESTKSendSMSReq  
  (9) AEESTK_SEND_SS_REQ                        AEESTKSendSSReq
 (10) AEESTK_SEND_USSD_REQ                      AEESTKSendUSSDReq  
 (11) AEESTK_SETUP_CALL_REQ                     AEESTKSetupCallReq
 (12) AEESTK_SETUP_MENU_REQ                     AEESTKSetupMenuReq
 (13) AEESTK_UPDATE_IDLE_TEXT_REQ               AEESTKUpdateIdleTextReq
 (14) AEESTK_END_PROACTIVE_CMD_SES              AEESTKEndProactiveCmdReq
 (15) AEESTK_PROACTIVE_CMD_SES_WITHOUT_TR       AEESTKProactiveCmdHeader
 (16) AEESTK_PROVIDE_LANG_INFO_REQ              AEESTKProvideLangReq
 (17) AEESTK_LANG_NOTIFICATION_REQ              AEESTKLangInfoReq
 (17) AEESTK_OPEN_CH_REQ                        AEESTKOpenChReq
 (18) AEESTK_CLOSE_CH_REQ                       AEESTKCloseChReq
 (19) AEESTK_SEND_DATA_REQ                      AEESTKSendDataReq
 (20) AEESTK_RECEIVE_DATA_REQ                   AEESTKReceiveDataReq
 (21) AEESTK_GET_CH_STATUS_REQ                  AEESTKGetChStatusReq


 None of the envelope commands are supported at present.


SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*=============================================================================
FUNCTION: ISTK_SendCommandResponse

DESCRIPTION:  
  Send terminal response for a proactive command.

PROTOTYPE:
  int ISTK_SendCommandResponse
                (
                  ISTK                      *pISTK,
                  AEESTKProactiveCmdHeader  cmdHdr,
                  AEESTKResult              result,						  
                  AEESTKAdditionalInfo      *additionalResult,						  
                  void                      *extra_param,
		              uint8			                 extra_param_length
                );

PARAMETERS:
  pISTK                [in]: ISTK interface pointer.
  cmdHdr               [in]: Header of the command for which response is being sent.
  result               [in]: Result for the request
  *additionalResult    [in]: Additional result for the request
  *extra_param         [in]: Any additional information along with the result.
  extra_param_length   [in]: Length of any additional information

RETURN VALUE: 
  Returns AEE return values.

  AEE_SUCCESS:  If procedure response was sent successfully.
  EBADPARM:     If cmdHdr specifies a non-existent command / already responded command
  EFAILED:      Otherwise.

COMMENTS:

  After the application has processed the proactive command, it needs to send a response for 
  the command. The response contains the command for which the response is being sent, the 
  result for the request, additional information and any extra information that needs to be sent.

  The result usually contains whether the proactive request was successfully performed or not.
  The Additional info extends the result, giving a specific reason why command faile, if so.
  extra_param contains command-specific information. 
  Look at the AEESTKResult for the various results that can be given.
  Look at the AEESTKAddionalInfoCode for the various additional information types.

  Following is the list of the command-ids for which application can send extra_param 

  Command ID                Type of Extra paramter   Information in Extra paramter
  ==================================================================================-====
  AEESTK_GET_INKEY_REQ      AEESTKString              The key entered by user
  AEESTK_GET_INPUT_REQ      AEESTKString              The string entered by user
  AEESTK_SELECT_ITEM_REQ    uint8                     The id of the item selected
  AEESTK_SETUP_CALL_REQ     AEESTKCallSSResponse      See the structure definition for the details
  AEESTK_SEND_SS_REQ        AEESTKCallSSResponse      See the structure definition for the details 
  AEESTK_SEND_USSD_REQ      AEESTKUSSDResponse        See the structure definition for the details
  AEESTK_OPEN_CH_REQ        AEESTKOpenChResponse      See the structure definition for the details
  AEESTK_SEND_DATA_REQ      int32                     Data Length being sent
  AEESTK_RECEIVE_DATA_REQ   AEESTKReceiveDataResponse See the structure definition for the details
  AEESTK_GET_CH_STATUS_REQ  AEESTKGetChStatusResponse See the structure definition for the details


  For all other proactive requests, this parameter should be set to NULL.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*=============================================================================
FUNCTION: ISTK_SendEnvelopeEvtDownloadCommand

DESCRIPTION:  
  Send an Envelope Event Download command

PROTOTYPE:
  int ISTK_SendEnvelopeEvtDownloadCommand
                (
                  ISTK                      *pISTK,	
	                uint16                    *pwTransactionID,
                  uint32                    dwSize,
                  AEESTKEventDLInd          *evtList
	        );

PARAMETERS:
  pISTK             [in]:  ISTK interface pointer.
  pwTransactionID   [out]: TransactionId, filled in by ISTK. 
  evtList           [out]: The NULL terminated event download list.

RETURN VALUE: 
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If pwTransactionID/evtList is/are NULL.
  EFAILED:      Otherwise.

COMMENTS:
  
  When the application sends an Event Download command, it recievs a Transaction Id.
  This transaction id can be used later by application when it recieves an EVT_NOTIFY 
  with NMASK_STKNOTIFIER_ENVELOPE_CMD. In this case, the upper 16 bits of the data 
  received with the event is same as the Transaction Id application has received when it 
  sent the Envelope event download command while the lower 16 bits specify the command id.
  This way, from the notification, the application can determine if the response received
  from the card is for the command it generated or not. (Since there can be multiple clients 
  for the ISTK, some other client could have started the envelope command).

  *Currently this functionality is not supported as none of the envelope commands 
  are supported.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*=============================================================================
FUNCTION: ISTK_SendEnvelopeMenuSelCommand

DESCRIPTION:  
  Send Menu Selection Envelope Command.

PROTOTYPE:
  int ISTK_SendEnvelopeMenuSelCommand
                (
                  ISTK		                  *pISTK,
	                uint16                    *pwTransactionID,
                  byte                      nItemChosen,                          
                  boolean                   bHelp
                );

PARAMETERS:
  pISTK             [in]:  ISTK interface pointer.
  pwTransactionID   [out]: TransactionId, filled in by ISTK. 
  nItemChosen       [in] : Id of the item selected
  bHelp             [in]:  TRUE if help required by user, FALSE otherwise

RETURN VALUE: 
  Returns AEE return values.

  AEE_SUCCESS:  If procedure was started correctly.
  EBADPARM:     If pwTransactionID is NULL.
  EFAILED:      Otherwise.

COMMENTS:
  
  For the main menu that application displays, whenever an item is selected, the 
  application needs to call ISTK_SendEnvelopeMenuSelCommand to let the STK know 
  about the item selected so that it can respond with the next request to be performed.

  Again, application gets back a transaction id for this transaction which it can keep to
  determine later, if any response received from the STK for the envelope commands was
  for the application itself. The nItemChosen is the nItemID field of the MenuItem that was
  received by the application as a part of the AEESTK_SETUP_MENU_REQUEST.

  The application has to implement a way for the user to request help. (e.g. pressing '#'
  key on menu-screen means the user needs help). If the user requsted help, set the fourth 
  parameter (bHelp) to TRUE.

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

#endif //AEESTK_H

