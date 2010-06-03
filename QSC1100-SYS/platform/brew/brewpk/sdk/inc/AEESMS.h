#ifndef AEESMS_H
#define AEESMS_H
/*=====================================================================
FILE:  AEESMS.h

SERVICES:  AEE SMS Interfaces

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEE SMS

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
======================================================================*/
#include "AEEWebOpts.h"
#include "AEEModel.h"

//**********************************************************************
//
// ISMSMsg Interface
//
//**********************************************************************
typedef WebOpt SMSMsgOpt;
typedef struct ISMSMsg      ISMSMsg;

// Define priorities for message
#define AEESMS_PRIORITY_NORMAL         0
#define AEESMS_PRIORITY_INTERACTIVE    1
#define AEESMS_PRIORITY_URGENT         2
#define AEESMS_PRIORITY_EMERGENCY      3

// Define a data type to refer priorities
typedef uint32 AEEMSGPriority;

// Define privacy options
#define AEESMS_PRIVACY_NORMAL          0
#define AEESMS_PRIVACY_RESTRICTED      1
#define AEESMS_PRIVACY_CONFIDENTIAL    2
#define AEESMS_PRIVACY_SECRET          3

// Define a data type to refer privacy options
typedef uint32 AEEMSGPrivacy;

// Define message classes
#define AEESMS_CLASS_0                 0           // GW Class 0
#define AEESMS_CLASS_1                 1           // GW Class 1
#define AEESMS_CLASS_2                 2           // GW Class 2
#define AEESMS_CLASS_3                 3           // GW Class 3
#define AEESMS_CLASS_NONE              4           // None
#define AEESMS_CLASS_CDMA              5           // CDMA

// Define a data type to refer message class
typedef uint32 AEESMSClass;

// Define message tags
#define AEESMS_TAG_NONE                      0
#define AEESMS_TAG_MT_READ                   1
#define AEESMS_TAG_MT_NOT_READ               2
#define AEESMS_TAG_MO_SENT                   3
#define AEESMS_TAG_MO_NOT_SENT               4
#define AEESMS_TAG_MO_SENT_ST_NOT_RECEIVED   5
#define AEESMS_TAG_MO_SENT_ST_NOT_STORED     6
#define AEESMS_TAG_MO_SENT_ST_STORED         7
#define AEESMS_TAG_MO_TEMPLATE               8
#define AEESMS_TAG_STATUS_RPT                9

// Define a data type to refer to message tags
typedef uint32 AEESMSTag;

// Define Protocol ID for GSM / WCDMA Messages
#define AEESMS_PID_DEFAULT           0x00
#define AEESMS_PID_IMPLICIT          0x20
#define AEESMS_PID_TELEX             0x21
#define AEESMS_PID_G3_FAX            0x22
#define AEESMS_PID_G4_FAX            0x23
#define AEESMS_PID_VOICE_PHONE       0x24
#define AEESMS_PID_ERMES             0x25
#define AEESMS_PID_NAT_PAGING        0x26
#define AEESMS_PID_VIDEOTEX          0x27
#define AEESMS_PID_TELETEX_UNSPEC    0x28
#define AEESMS_PID_TELETEX_PSPDN     0x29
#define AEESMS_PID_TELETEX_CSPDN     0x2a
#define AEESMS_PID_TELETEX_PSTN      0x2b
#define AEESMS_PID_TELETEX_ISDN      0x2c
#define AEESMS_PID_UCI               0x2d
#define AEESMS_PID_RESERVED_0x1e     0x2e
#define AEESMS_PID_RESERVED_0x1f     0x2f
#define AEESMS_PID_MSG_HANDLING      0x30
#define AEESMS_PID_X400              0x31
#define AEESMS_PID_INTERNET_EMAIL    0x32
#define AEESMS_PID_RESERVED_0x33     0x33
#define AEESMS_PID_RESERVED_0x34     0x34
#define AEESMS_PID_RESERVED_0x35     0x35
#define AEESMS_PID_RESERVED_0x36     0x36
#define AEESMS_PID_RESERVED_0x37     0x37
#define AEESMS_PID_SC_SPECIFIC_1     0x38
#define AEESMS_PID_SC_SPECIFIC_2     0x39
#define AEESMS_PID_SC_SPECIFIC_3     0x3a
#define AEESMS_PID_SC_SPECIFIC_4     0x3b
#define AEESMS_PID_SC_SPECIFIC_5     0x3c
#define AEESMS_PID_SC_SPECIFIC_6     0x3d
#define AEESMS_PID_SC_SPECIFIC_7     0x3e
#define AEESMS_PID_GSM_UMTS          0x3f
#define AEESMS_PID_SM_TYPE_0         0x40
#define AEESMS_PID_REPLACE_SM_1      0x41
#define AEESMS_PID_REPLACE_SM_2      0x42
#define AEESMS_PID_REPLACE_SM_3      0x43
#define AEESMS_PID_REPLACE_SM_4      0x44
#define AEESMS_PID_REPLACE_SM_5      0x45
#define AEESMS_PID_REPLACE_SM_6      0x46
#define AEESMS_PID_REPLACE_SM_7      0x47
#define AEESMS_PID_EMS               0x5e
#define AEESMS_PID_RETURN_CALL       0x5f
#define AEESMS_PID_ANSI136_R_DATA    0x7c
#define AEESMS_PID_ME_DATA_DOWNLOAD  0x7d
#define AEESMS_PID_ME_DEPERSONALIZE  0x7e
#define AEESMS_PID_SIM_DATA_DOWNLOAD 0x7f

// Define a data type to refer to Protocol ID
typedef uint32 AEESMSPID;

#define AEESMS_NUMBER_UNKNOWN          0
#define AEESMS_NUMBER_INTERNATIONAL    1
#define AEESMS_NUMBER_NATIONAL         2
#define AEESMS_NUMBER_NETWORK          3
#define AEESMS_NUMBER_SUBSCRIBER       4
#define AEESMS_NUMBER_ALPHANUMERIC     5
#define AEESMS_NUMBER_ABBREVIATED      6

// Define a data type to refer to Number Type
typedef uint32 AEESMSNumberType;

#define AEESMS_NUMBER_PLAN_UNKNOWN     0
#define AEESMS_NUMBER_PLAN_TELEPHONY   1  /* CCITT E.164 and E.163, including ISDN plan */
#define AEESMS_NUMBER_PLAN_DATA        3  /* CCITT X.121 */
#define AEESMS_NUMBER_PLAN_TELEX       4  /* CCITT F.69 */
#define AEESMS_NUMBER_PLAN_PRIVATE     9

// Define a data type to refer to Number Plan
typedef uint32 AEESMSNumberPlan;

#define AEESMS_TP_STATUS_RECEIVED_OK                     0x00
#define AEESMS_TP_STATUS_UNABLE_TO_CONFIRM_DELIVERY      0x01
#define AEESMS_TP_STATUS_REPLACED                        0x02
#define AEESMS_TP_STATUS_TRYING_CONGESTION               0x20
#define AEESMS_TP_STATUS_TRYING_SME_BUSY                 0x21
#define AEESMS_TP_STATUS_TRYING_NO_RESPONSE_FROM_SME     0x22
#define AEESMS_TP_STATUS_TRYING_SERVICE_REJECTED         0x23
#define AEESMS_TP_STATUS_TRYING_QOS_NOT_AVAILABLE        0x24
#define AEESMS_TP_STATUS_TRYING_SME_ERROR                0x25
#define AEESMS_TP_STATUS_PERM_REMOTE_PROCEDURE_ERROR     0x40
#define AEESMS_TP_STATUS_PERM_INCOMPATIBLE_DEST          0x41
#define AEESMS_TP_STATUS_PERM_REJECTED_BY_SME            0x42
#define AEESMS_TP_STATUS_PERM_NOT_OBTAINABLE             0x43
#define AEESMS_TP_STATUS_PERM_QOS_NOT_AVAILABLE          0x44
#define AEESMS_TP_STATUS_PERM_NO_INTERWORKING            0x45
#define AEESMS_TP_STATUS_PERM_VP_EXPIRED                 0x46
#define AEESMS_TP_STATUS_PERM_DELETED_BY_ORIG_SME        0x47
#define AEESMS_TP_STATUS_PERM_DELETED_BY_SC_ADMIN        0x48
#define AEESMS_TP_STATUS_PERM_SM_NO_EXISTING             0x49
#define AEESMS_TP_STATUS_TEMP_CONGESTION                 0x60
#define AEESMS_TP_STATUS_TEMP_SME_BUSY                   0x61
#define AEESMS_TP_STATUS_TEMP_NO_RESPONSE_FROM_SME       0x62
#define AEESMS_TP_STATUS_TEMP_SERVICE_REJECTED           0x63
#define AEESMS_TP_STATUS_TEMP_QOS_NOT_AVAILABLE          0x64
#define AEESMS_TP_STATUS_TEMP_SME_ERROR                  0x65
#define AEESMS_TP_STATUS_LAST                            0xFF

// Define a data type to refer to Report Status
typedef uint32 AEESMSReportStatus;

// Define message types
#define AEESMS_TYPE_UNKNOWN                  0
#define AEESMS_TYPE_TEXT                     1
#define AEESMS_TYPE_PAGE                     2
#define AEESMS_TYPE_VOICE_MAIL_NOTIFICATION  3
#define AEESMS_TYPE_BROADCAST                4
#define AEESMS_TYPE_WAP                      5
#define AEESMS_TYPE_EMS                      6
#define AEESMS_TYPE_ANY                      0xffff

// Define a data type to refer to message type
typedef uint32 AEESMSType;

// Define Message Waiting Types
#define  AEESMS_MSGWAITINGTYPE_NONE          0
#define  AEESMS_MSGWAITINGTYPE_DISCARD       1
#define  AEESMS_MSGWAITINGTYPE_STORE         2
#define  AEESMS_MSGWAITINGTYPE_NONE_1111     3

// Define a data type to refer to Message Waiting Type
typedef uint8 AEESMSMsgWaitingType;



// Define Message Waiting Kind Types
#define  AEESMS_MSGWAITINGKIND_VOICEMAIL              0
#define  AEESMS_MSGWAITINGKIND_FAX                    1 
#define  AEESMS_MSGWAITINGKIND_EMAIL                  2
#define  AEESMS_MSGWAITINGKIND_OTHER                  3
#define  AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE     0x10
#define  AEESMS_MSGWAITINGKIND_FAX_INACTIVE           0x11 
#define  AEESMS_MSGWAITINGKIND_EMAIL_INACTIVE         0x12
#define  AEESMS_MSGWAITINGKIND_OTHER_INACTIVE         0x13
#define  AEESMS_MSGWAITINGKIND_NONE                   0xff

// Define a data type to refer to Message Waiting Kind Type
typedef uint8 AEESMSMsgWaitingKind;



// Define User Data Header Types
#define   AEESMS_UDH_CONCAT_8                0
#define   AEESMS_UDH_SPECIAL_SM              1
#define   AEESMS_UDH_PORT_8                  4
#define   AEESMS_UDH_PORT_16                 5 
#define   AEESMS_UDH_SMSC_CONTROL            6
#define   AEESMS_UDH_SOURCE                  7
#define   AEESMS_UDH_CONCAT_16               8
#define   AEESMS_UDH_WCMP                    9
#define   AEESMS_UDH_TEXT_FORMATING          10
#define   AEESMS_UDH_PRE_DEF_SOUND           11
#define   AEESMS_UDH_USER_DEF_SOUND          12
#define   AEESMS_UDH_PRE_DEF_ANIM            13
#define   AEESMS_UDH_LARGE_ANIM              14
#define   AEESMS_UDH_SMALL_ANIM              15
#define   AEESMS_UDH_LARGE_PICTURE           16
#define   AEESMS_UDH_SMALL_PICTURE           17
#define   AEESMS_UDH_VAR_PICTURE             18
#define   AEESMS_UDH_RFC822                  32

// Define a data type to refer UDH ID
typedef uint8 AEESMSUDHId;

#define  AEESMS_UDH_TEXT_COLOR_BLACK          0x0
#define  AEESMS_UDH_TEXT_COLOR_DARK_GREY      0x1
#define  AEESMS_UDH_TEXT_COLOR_DARK_RED       0x2
#define  AEESMS_UDH_TEXT_COLOR_DARK_YELLOW    0x3
#define  AEESMS_UDH_TEXT_COLOR_DARK_GREEN     0x4
#define  AEESMS_UDH_TEXT_COLOR_DARK_CYAN      0x5
#define  AEESMS_UDH_TEXT_COLOR_DARK_BLUE      0x6
#define  AEESMS_UDH_TEXT_COLOR_DARK_MAGENTA   0x7
#define  AEESMS_UDH_TEXT_COLOR_GREY           0x8
#define  AEESMS_UDH_TEXT_COLOR_WHITE          0x9
#define  AEESMS_UDH_TEXT_COLOR_BRIGHT_RED     0xA
#define  AEESMS_UDH_TEXT_COLOR_BRIGHT_YELLOW  0xB
#define  AEESMS_UDH_TEXT_COLOR_BRIGHT_GREEN   0xC
#define  AEESMS_UDH_TEXT_COLOR_BRIGHT_CYAN    0xD
#define  AEESMS_UDH_TEXT_COLOR_BRIGHT_BLUE    0xE
#define  AEESMS_UDH_TEXT_COLOR_BRIGHT_MAGENTA 0xF

// Define a data type to refer UDH Text Color ID
typedef uint8 AEESMSUDHTextColorId;

// Define message broadcast types
#define AEESMS_BC_UNKOWN                     0
#define AEESMS_BC_EMERGENCY                  1
#define AEESMS_BC_ADMIN                      2
#define AEESMS_BC_MAINTENANCE                3
#define AEESMS_BC_GEN_NEWS_LOC               4
#define AEESMS_BC_GEN_NEWS_REG               5
#define AEESMS_BC_GEN_NEWS_NAT               6
#define AEESMS_BC_GEN_NEWS_INT               7
#define AEESMS_BC_FIN_NEWS_LOC               8
#define AEESMS_BC_FIN_NEWS_REG               9
#define AEESMS_BC_FIN_NEWS_NAT               0xA
#define AEESMS_BC_FIN_NEWS_INT               0xB
#define AEESMS_BC_SPT_NEWS_LOC               0xC
#define AEESMS_BC_SPT_NEWS_REG               0xD
#define AEESMS_BC_SPT_NEWS_NAT               0xE
#define AEESMS_BC_SPT_NEWS_INT               0xF
#define AEESMS_BC_ENT_NEWS_LOC               0x10
#define AEESMS_BC_ENT_NEWS_REG               0x11
#define AEESMS_BC_ENT_NEWS_NAT               0x12
#define AEESMS_BC_ENT_NEWS_INT               0x13
#define AEESMS_BC_LOC_WEATHER                0x14
#define AEESMS_BC_AREA_TRAFFIC               0x15
#define AEESMS_BC_AIRPORT_SCHED              0x16
#define AEESMS_BC_RESTAURANTS                0x17
#define AEESMS_BC_LODGINGS                   0x18
#define AEESMS_BC_RETAILS                    0x19
#define AEESMS_BC_ADS                        0x1A
#define AEESMS_BC_STOCK_QUOTES               0x1B
#define AEESMS_BC_JOBS                       0x1C
#define AEESMS_BC_MEDICAL                    0x1D
#define AEESMS_BC_TECH_NEWS                  0x1E
#define AEESMS_BC_MULTI                      0x1F
#define AEESMS_BC_ALL                        0xffff

// Define a data type to refer to message broadcast types
typedef uint32 AEESMSBCType;

#define AEESMS_ENC_OCTET             0
#define AEESMS_ENC_IS91EP            1
#define AEESMS_ENC_ASCII             2
#define AEESMS_ENC_IA5               3
#define AEESMS_ENC_UNICODE           4
#define AEESMS_ENC_SHIFT_JIS         5
#define AEESMS_ENC_KOREAN            6
#define AEESMS_ENC_LATIN_HEBREW      7
#define AEESMS_ENC_LATIN             8
#define AEESMS_ENC_GSM_7_BIT_DEFAULT 9

// Define a data type to refer to MO SMS encoding types
typedef uint32 AEESMSEncType;

#define AEESMS_GEO_SCOPE_CELL_IMMEDIATE  0
#define AEESMS_GEO_SCOPE_PLMN            1
#define AEESMS_GEO_SCOPE_LOCATION_AREA   2
#define AEESMS_GEO_SCOPE_CELL_NORMAL     3

// Define a data type to refer to Geo Scope types
typedef uint32 AEESMSGeoScope;

#define AEESMS_GW_CB_UNKNOWN             0
#define AEESMS_GW_CB_NO_SERVICE          1
#define AEESMS_GW_CB_SAME_CELL           2
#define AEESMS_GW_CB_CELL_CHANGE         3
#define AEESMS_GW_CB_LAC_CHANGE          4
#define AEESMS_GW_CB_PLMN_CHANGE         5

// Define a data type to refer to Geo Scope types
typedef uint32 AEESMSCellChangeType;

typedef struct
{
   uint16   wYear;
   uint16   wMonth;
   uint16   wDay;
   uint16   wHour;
   uint16   wMinute;
   uint16   wSecond;
   uint16   wWeekDay;
   sint15   nTimeZone;
} AEESMSGWTime;

// Define GSM/UMTS commands supported by ISMS
#define AEESMS_GW_COMMAND_ENQUIRY                  0
#define AEESMS_GW_COMMAND_CANCEL_STATUS_REPORT     1
#define AEESMS_GW_COMMAND_DELETE_SM                2
#define AEESMS_GW_COMMAND_ENABLE_STATUS_REPORT     3

// Define a data type to refer to GSM/WCDMA commands
typedef uint32 AEESMSGWCommand;

//Define a data type for display modes
typedef uint32 AEESMSDisplayMode;

#define AEESMS_DISPLAY_MODE_IMMEDIATE    0
#define AEESMS_DISPLAY_MODE_DEFAULT      1
#define AEESMS_DISPLAY_MODE_USER_INVOKE  2
#define AEESMS_DISPLAY_MODE_RESERVED     3
#define AEESMS_DISPLAY_MODE_NONE         0xFFFFFFFF

// Defines to set various message options
#define MSGOPT_TO_DEVICE_SZ     (WEBOPT_SZ + 0)
#define MSGOPT_PAYLOAD_SZ       (WEBOPT_SZ + 1)
#define MSGOPT_BREW_HANDLER_SZ  (WEBOPT_SZ + 2)
#define MSGOPT_SC_ADDRESS_SZ    (WEBOPT_SZ + 3)
#define MSGOPT_FROM_DEVICE_SZ   (WEBOPT_SZ + 4)
#define MSGOPT_CALLBACK_SZ      (WEBOPT_SZ + 5)
#define MSGOPT_TO_EMAIL_SZ      (WEBOPT_SZ + 6)

#define MSGOPT_PAYLOAD_WSZ      (WEBOPT_WSZ + 0)

#define MSGOPT_VARIABLE_UNIQUE(id)     (WEBOPT_VARIABLE + (id)*WEBOPT_VARIABLE_INC)

#define MSGOPT_TIMESTAMP               (MSGOPT_VARIABLE_UNIQUE(0) + sizeof(JulianType))
#define MSGOPT_ABSOULTE_DEFERREDTIME   (MSGOPT_VARIABLE_UNIQUE(1) + sizeof(JulianType))
#define MSGOPT_ABSOLUTE_DEFERREDTIME   (MSGOPT_VARIABLE_UNIQUE(1) + sizeof(JulianType))
#define MSGOPT_GW_TIMESTAMP            (MSGOPT_VARIABLE_UNIQUE(2) + sizeof(AEESMSGWTime))
#define MSGOPT_DISCHARGETIME           (MSGOPT_VARIABLE_UNIQUE(3) + sizeof(AEESMSGWTime))
#define MSGOPT_ABSOLUTE_VALIDITY       (MSGOPT_VARIABLE_UNIQUE(4) + sizeof(AEESMSGWTime))

#define MSGOPT_BREW_CLASS              (WEBOPT_32BIT + 0)
#define MSGOPT_PRIORITY                (WEBOPT_32BIT + 1)
#define MSGOPT_PRIVACY                 (WEBOPT_32BIT + 2)
#define MSGOPT_PAYLOAD_ENCODING        (WEBOPT_32BIT + 3)
#define MSGOPT_LANG                    (WEBOPT_32BIT + 4)
#define MSGOPT_CLASS                   (WEBOPT_32BIT + 5)
#define MSGOPT_TAG                     (WEBOPT_32BIT + 6)
#define MSGOPT_MSGTYPE                 (WEBOPT_32BIT + 7)
#define MSGOPT_BCTYPE                  (WEBOPT_32BIT + 8)
#define MSGOPT_REQUEST_DL_ACK          (WEBOPT_32BIT + 9)
#define MSGOPT_NUM                     (WEBOPT_32BIT + 10)
#define MSGOPT_REQUEST_USER_ACK        (WEBOPT_32BIT + 11)
#define MSGOPT_MOSMS_ENCODING          (WEBOPT_32BIT + 12)
#define MSGOPT_MSG_ID                  (WEBOPT_32BIT + 13)
#define MSGOPT_REQUEST_READ_ACK        (WEBOPT_32BIT + 14)
#define MSGOPT_DL_ACK                  (WEBOPT_32BIT + 15)
#define MSGOPT_USER_ACK                (WEBOPT_32BIT + 16)
#define MSGOPT_READ_ACK                (WEBOPT_32BIT + 17)
#define MSGOPT_BC_SERVICE_ID           (WEBOPT_32BIT + 18)
#define MSGOPT_BC_PAGE_NUMBER          (WEBOPT_32BIT + 19)
#define MSGOPT_BC_TOTAL_PAGES          (WEBOPT_32BIT + 20)
#define MSGOPT_BC_GEO_SCOPE            (WEBOPT_32BIT + 21)
#define MSGOPT_BC_MESSAGE_CODE         (WEBOPT_32BIT + 22)
#define MSGOPT_BC_UPDATE_NUMBER        (WEBOPT_32BIT + 23)
#define MSGOPT_GW_PID                  (WEBOPT_32BIT + 24)
#define MSGOPT_REPLY_PATH_PRESENT      (WEBOPT_32BIT + 25)
#define MSGOPT_REPORT_STATUS           (WEBOPT_32BIT + 26)
#define MSGOPT_TO_NUMBER_TYPE          (WEBOPT_32BIT + 27)
#define MSGOPT_TO_NUMBER_PLAN          (WEBOPT_32BIT + 28)
#define MSGOPT_FROM_NUMBER_TYPE        (WEBOPT_32BIT + 29)
#define MSGOPT_FROM_NUMBER_PLAN        (WEBOPT_32BIT + 30)
#define MSGOPT_CALLBACK_NUMBER_TYPE    (WEBOPT_32BIT + 31)
#define MSGOPT_CALLBACK_NUMBER_PLAN    (WEBOPT_32BIT + 32)
#define MSGOPT_SC_ADDRESS_NUMBER_TYPE  (WEBOPT_32BIT + 33)
#define MSGOPT_SC_ADDRESS_NUMBER_PLAN  (WEBOPT_32BIT + 34)
#define MSGOPT_AUTO_STORAGE_TYPE       (WEBOPT_32BIT + 35)
#define MSGOPT_AUTO_STORAGE_INDEX      (WEBOPT_32BIT + 36)
#define MSGOPT_MSG_WAITING_TYPE        (WEBOPT_32BIT + 37)
#define MSGOPT_MSG_WAITING_KIND        (WEBOPT_32BIT + 38)
#define MSGOPT_COMPRESSED_MSG          (WEBOPT_32BIT + 39)
#define MSGOPT_MORE_MSG_TO_SEND        (WEBOPT_32BIT + 40)
#define MSGOPT_RAW_DCS_DATA            (WEBOPT_32BIT + 41)
#define MSGOPT_RELATIVE_DEFERREDTIME   (WEBOPT_32BIT + 42)
#define MSGOPT_RELATIVE_VALIDITYTIME   (WEBOPT_32BIT + 43)
#define MSGOPT_CANCELLATION_REQUEST    (WEBOPT_32BIT + 44)
#define MSGOPT_GW_COMMAND              (WEBOPT_32BIT + 45)
#define MSGOPT_GW_COMMAND_UDHI         (WEBOPT_32BIT + 46)
#define MSGOPT_GW_COMMAND_MN           (WEBOPT_32BIT + 47)
#define MSGOPT_DISPLAY_MODE            (WEBOPT_32BIT + 48)
#define MSGOPT_LOOPBACK_MSG            (WEBOPT_32BIT + 49)
#define MSGOPT_LOOPBACK_FROM_APPLET    (WEBOPT_32BIT + 50)

#define MSGOPT_PAYLOAD_BINARY       (WEBOPT_VARBUFFER + 0)
#define MSGOPT_UDH_CONCAT           (WEBOPT_VARBUFFER + 1)
#define MSGOPT_UDH_SPECIAL_SM       (WEBOPT_VARBUFFER + 2)
#define MSGOPT_UDH_PORT             (WEBOPT_VARBUFFER + 3)
#define MSGOPT_UDH_TEXT_FORMAT      (WEBOPT_VARBUFFER + 4)
#define MSGOPT_UDH_PREDEFINED       (WEBOPT_VARBUFFER + 5)
#define MSGOPT_UDH_USERDEFINED      (WEBOPT_VARBUFFER + 6)
#define MSGOPT_UDH_VAR_PICTURE      (WEBOPT_VARBUFFER + 7)
#define MSGOPT_UDH_RFC822           (WEBOPT_VARBUFFER + 8)
#define MSGOPT_UDH_OTHER            (WEBOPT_VARBUFFER + 9)
#define MSGOPT_TPDU_DELIVER         (WEBOPT_VARBUFFER + 10) 
#define MSGOPT_TPDU_STATUS_REPORT   (WEBOPT_VARBUFFER + 11)
#define MSGOPT_ALPHA_ID             (WEBOPT_VARBUFFER + 12)
#define MSGOPT_COMMAND_DATA         (WEBOPT_VARBUFFER + 13)

#define MSGOPT_PAYLOAD_SOURCE   (WEBOPT_INTERFACE + 0)

#define MSGOPT_END              WEBOPT_END
#define MSGOPT_ANY              WEBOPT_ANY
#define MSGOPT_COPYOPTS         WEBOPT_COPYOPTS

#define MSGOPT_SETUDHCONCAT8BUFFER(pbuf,seq,total,ref)      do {((unsigned char*)(pbuf))[0] = (unsigned char)  5; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_CONCAT_8;      ((unsigned char*)(pbuf))[3] = (unsigned char)(seq);    ((unsigned char*)(pbuf))[4] = (unsigned char)(total); ((unsigned char*)(pbuf))[5] = (unsigned char)(ref);   ((unsigned char*)(pbuf))[6] = (unsigned char)0;} while(0)
#define MSGOPT_SETUDHCONCAT16BUFFER(pbuf,seq,total,ref)     do {((unsigned char*)(pbuf))[0] = (unsigned char)  5; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_CONCAT_16;     ((unsigned char*)(pbuf))[3] = (unsigned char)(seq);    ((unsigned char*)(pbuf))[4] = (unsigned char)(total); ((unsigned char*)(pbuf))[5] = (unsigned char)((ref) & 0x00FF); ((unsigned char*)(pbuf))[6] = (unsigned char)((ref) >> 8);} while(0)

#define MSGOPT_SETUDHSPECIALSMBUFFER(pbuf,waiting,waiting_kind,count) \
                                                            do {((unsigned char*)(pbuf))[0] = (unsigned char)  4; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_SPECIAL_SM;    ((unsigned char*)(pbuf))[3] = (unsigned char)(waiting);((unsigned char*)(pbuf))[4] = (unsigned char)(waiting_kind); ((unsigned char*)(pbuf))[5] = (unsigned char)(count);} while(0)

#define MSGOPT_SETUDHPORT8BUFFER(pbuf,dest,orig)            do {((unsigned char*)(pbuf))[0] = (unsigned char)  5; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_PORT_8;        ((unsigned char*)(pbuf))[3] = (unsigned char)(dest);   ((unsigned char*)(pbuf))[4] = (unsigned char)(0);     ((unsigned char*)(pbuf))[5] = (unsigned char)(orig);  ((unsigned char*)(pbuf))[6] = (unsigned char)0;} while(0)
#define MSGOPT_SETUDHPORT16BUFFER(pbuf,dest,orig)           do {((unsigned char*)(pbuf))[0] = (unsigned char)  5; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_PORT_16;       ((unsigned char*)(pbuf))[3] = (unsigned char)((dest) & 0x00FF); ((unsigned char*)(pbuf))[4] = (unsigned char)((dest) >> 8); ((unsigned char*)(pbuf))[5] = (unsigned char)((orig) & 0x00FF); ((unsigned char*)(pbuf))[6] = (unsigned char)((orig) >> 8);} while(0)

#define MSGOPT_SETUDHTEXTFORMATBUFFER(pbuf,start,len,style) do {((unsigned char*)(pbuf))[0] = (unsigned char)  4; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_TEXT_FORMATING;((unsigned char*)(pbuf))[3] = (unsigned char)(start);  ((unsigned char*)(pbuf))[4] = (unsigned char)(len);   ((unsigned char*)(pbuf))[5] = (unsigned char)(style);} while(0)
#define MSGOPT_SETUDHTEXTWITHCOLORFORMATBUFFER(pbuf,start,len,style,color) \
                                                            do {((unsigned char*)(pbuf))[0] = (unsigned char)  5; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_TEXT_FORMATING;((unsigned char*)(pbuf))[3] = (unsigned char)(start);  ((unsigned char*)(pbuf))[4] = (unsigned char)(len);   ((unsigned char*)(pbuf))[5] = (unsigned char)(style); ((unsigned char*)(pbuf))[6] = (unsigned char)(color);} while(0)

#define MSGOPT_SETUDHPREDEFSOUNDBUFFER(pbuf,pos,num)        do {((unsigned char*)(pbuf))[0] = (unsigned char)  3; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_PRE_DEF_SOUND; ((unsigned char*)(pbuf))[3] = (unsigned char)(pos);    ((unsigned char*)(pbuf))[4] = (unsigned char)(num);} while(0)
#define MSGOPT_SETUDHPREDEFANIMBUFFER(pbuf,pos,num)         do {((unsigned char*)(pbuf))[0] = (unsigned char)  3; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_PRE_DEF_ANIM;  ((unsigned char*)(pbuf))[3] = (unsigned char)(pos);    ((unsigned char*)(pbuf))[4] = (unsigned char)(num);} while(0)

#define MSGOPT_SETUDHUSERDEFSOUNDBUFFER(pbuf,pos,len,pdata) do {((unsigned char*)(pbuf))[0] = (unsigned char)((3 + (len)) & 0x00ff); ((unsigned char*)(pbuf))[1] = (unsigned char)((3 + (len)) >> 8); ((unsigned char*)(pbuf))[2] = AEESMS_UDH_USER_DEF_SOUND; ((unsigned char*)(pbuf))[3] = (unsigned char)(pos); ((unsigned char*)(pbuf))[4] = (unsigned char)(len); MEMCPY(((unsigned char*)((pbuf) + 5)),(pdata),(len));} while(0)
#define MSGOPT_SETUDHLARGEANIMBUFFER(pbuf,pos,pdata)        do {((unsigned char*)(pbuf))[0] = (unsigned char)131; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_LARGE_ANIM;    ((unsigned char*)(pbuf))[3] = (unsigned char)(pos);    ((unsigned char*)(pbuf))[4] = (unsigned char)(128); MEMCPY(((unsigned char*)((pbuf) + 5)),(pdata),(128));} while(0)
#define MSGOPT_SETUDHSMALLANIMBUFFER(pbuf,pos,pdata)        do {((unsigned char*)(pbuf))[0] = (unsigned char) 35; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_SMALL_ANIM;    ((unsigned char*)(pbuf))[3] = (unsigned char)(pos);    ((unsigned char*)(pbuf))[4] = (unsigned char) (32); MEMCPY(((unsigned char*)((pbuf) + 5)),(pdata), (32));} while(0)
#define MSGOPT_SETUDHLARGEPICBUFFER(pbuf,pos,pdata)         do {((unsigned char*)(pbuf))[0] = (unsigned char)131; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_LARGE_PICTURE; ((unsigned char*)(pbuf))[3] = (unsigned char)(pos);    ((unsigned char*)(pbuf))[4] = (unsigned char)(128); MEMCPY(((unsigned char*)((pbuf) + 5)),(pdata),(128));} while(0)
#define MSGOPT_SETUDHSMALLPICBUFFER(pbuf,pos,pdata)         do {((unsigned char*)(pbuf))[0] = (unsigned char) 35; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_SMALL_PICTURE; ((unsigned char*)(pbuf))[3] = (unsigned char)(pos);    ((unsigned char*)(pbuf))[4] = (unsigned char) (32); MEMCPY(((unsigned char*)((pbuf) + 5)),(pdata), (32));} while(0)

#define MSGOPT_SETUDHVARPICBUFFER(pbuf,pos,width,height,pdata) \
                                                            do {((unsigned char*)(pbuf))[0] = (unsigned char)(((39 + (width)*(height)) >> 3) & 0x00ff); ((unsigned char*)(pbuf))[1] = (unsigned char)((39 + ((width)*(height))) >> 11); ((unsigned char*)(pbuf))[2] = AEESMS_UDH_VAR_PICTURE; ((unsigned char*)(pbuf))[3] = (unsigned char)(pos); ((unsigned char*)(pbuf))[4] = (unsigned char)(width); ((unsigned char*)(pbuf))[5] = (unsigned char)(height); MEMCPY(((unsigned char*)((pbuf) + 6)),(pdata),((((height)*(width))+7)/8));} while(0)

#define MSGOPT_SETRFC822BUFFER(pbuf,len)                    do {((unsigned char*)(pbuf))[0] = (unsigned char)  2; ((unsigned char*)(pbuf))[1] = (unsigned char)0; ((unsigned char*)(pbuf))[2] = AEESMS_UDH_RFC822; ((unsigned char*)(pbuf))[3] = (unsigned char)(len);} while(0)

#define MSGOPT_SETUDHOTHERBUFFER(pbuf,udhID,len,pdata)      do {((unsigned char*)(pbuf))[0] = (unsigned char)((3 + (len)) & 0x00ff); ((unsigned char*)(pbuf))[1] = (unsigned char)((3 + (len)) >> 8); ((unsigned char*)(pbuf))[2] = (unsigned char)(udhID); ((unsigned char*)(pbuf))[3] = (unsigned char)((len) & 0x00FF); ((unsigned char*)(pbuf))[4] = (unsigned char)((len) >> 8); MEMCPY(((unsigned char*)((pbuf) + 5)),(pdata),(len));} while(0)

#define MSGOPT_GETUDHBUFFERLENGTH(pbuf)                     WEBOPT_GETVARBUFFERLENGTH(pbuf)
#define MSGOPT_GETUDHBUFFERDATA(pbuf)                       WEBOPT_GETVARBUFFERDATA(pbuf)

// Define ISMSMsg interface methods
AEEINTERFACE(ISMSMsg)
{
   INHERIT_IWebOpts(ISMSMsg);
};

#define ISMSMSG_AddRef(p)                       AEEGETPVTBL((p),ISMSMsg)->AddRef((p))
#define ISMSMSG_Release(p)                      AEEGETPVTBL((p),ISMSMsg)->Release((p))
#define ISMSMSG_QueryInterface(p,clsid,pp)      AEEGETPVTBL((p),ISMSMsg)->QueryInterface((p),(clsid),(pp))
#define ISMSMSG_AddOpt(p,o)                     AEEGETPVTBL((p),ISMSMsg)->AddOpt((p),(o))
#define ISMSMSG_RemoveOpt(p,n)                  AEEGETPVTBL((p),ISMSMsg)->RemoveOpt((p),(n),(0))
#define ISMSMSG_GetOpt(p,n,o)                   AEEGETPVTBL((p),ISMSMsg)->GetOpt((p),(n),(0),(o))
#define ISMSMSG_GetOptWithIndex(p,n,i,o)        AEEGETPVTBL((p),ISMSMsg)->GetOpt((p),(n),(i),(o))
//*********************************************************************
//
// ISMS Interface
//
//*********************************************************************
// Define client status. Main client can specify its status to ISMS using
// these types
#define AEESMS_CS_OK                            0
#define AEESMS_CS_OUT_OF_RESOURCES              1
#define AEESMS_CS_BUSY                          2
#define AEESMS_CS_BLOCK                         3

// Define data type to refer to client status
typedef uint32 AEESMSClientStatus;

// Define error type
#define AEESMS_ERRORTYPE_NONE                   0
#define AEESMS_ERRORTYPE_UNKNOWN                1
#define AEESMS_ERRORTYPE_TEMP                   2
#define AEESMS_ERRORTYPE_PERM                   3

// Define error
#define AEESMS_ERROR_NONE                       0

#define AEESMS_ERROR_GENERAL_PROBLEM               0x0001
#define AEESMS_ERROR_BUSY                          0x0002
#define AEESMS_ERROR_NO_MEMORY                     0x0003
#define AEESMS_ERROR_INVALID_TRANSACTION_ID        0x0004
#define AEESMS_ERROR_PHONE_NOT_READY               0x0005
#define AEESMS_ERROR_UNSUPPORTED                   0x0006
#define AEESMS_ERROR_UNEXPECTED_PARM_SIZE          0x0007
#define AEESMS_ERROR_MISSING_EXPECTED_PARM         0x0008
#define AEESMS_ERROR_MISSING_MANDATORY_PARM        0x0009  
#define AEESMS_ERROR_UNRECOGNIZED_PARM_VALUE       0x000a
#define AEESMS_ERROR_UNEXPECTED_PARM_VALUE         0x000b
#define AEESMS_ERROR_USER_DATA_SIZE_ERROR          0x000c
#define AEESMS_ERROR_SUPPL_SERVICE_NOT_SUPPORTED   0x000d
#define AEESMS_ERROR_RESERVED                      0x000e
#define AEESMS_ERROR_NO_ACK                        0x000f

#define AEESMS_ERROR_NETWORK_PROBLEM                  0x0010
#define AEESMS_ERROR_ADDRESS_TRANSLATION_FAILURE      0x0011
#define AEESMS_ERROR_ADDRESS_VACANT                   0x0012
#define AEESMS_ERROR_NETWORK_NOT_READY                0x0013
#define AEESMS_ERROR_NETWORK_RESOURCE_SHORTAGE        0x0014
#define AEESMS_ERROR_NETWORK_FAILURE                  0x0015
#define AEESMS_ERROR_NETWORK_INVALID_TELESERVICE_ID   0x0016
#define AEESMS_ERROR_NOT_ALLOWED_IN_AMPS              0x0017           
#define AEESMS_ERROR_ACCESS_TOO_LARGE                 0x0018
#define AEESMS_ERROR_DTC_TOO_LARGE                    0x0019
#define AEESMS_ERROR_CANNOT_SEND_BROADCAST            0x001a
#define AEESMS_ERROR_RPT_ACCESS_BLOCK                 0x001b

#define AEESMS_ERROR_DESTINATION_BUSY                       0x0020
#define AEESMS_ERROR_DESTINATION_OUT_OF_SERVICE             0x0021
#define AEESMS_ERROR_DESTINATION_PROBLEM                    0x0022
#define AEESMS_ERROR_DESTINATION_NO_PAGE_RESPONSE           0x0023
#define AEESMS_ERROR_DESTINATION_RESOURCE_SHORTAGE          0x0024
#define AEESMS_ERROR_DESTINATION_NO_LONGER_AT_THIS_ADDRESS  0x0025

#define AEESMS_ERROR_RADIO_PROBLEM                 0x0030
#define AEESMS_ERROR_RADIO_IF_RESOURCE_SHORTAGE    0x0031
#define AEESMS_ERROR_RADIO_IF_INCOMPATIBLE         0x0032

#define AEESMS_ERROR_PID_PROBLEM                   0x0040
#define AEESMS_ERROR_UNSUPPORTED_TELEMATIC         0x0041

#define AEESMS_ERROR_DCS_PROBLEM                   0x0051
#define AEESMS_ERROR_ALPHABET_NOT_SUPPORTED        0x0052
#define AEESMS_ERROR_MSG_CLASS_NOT_SUPPORTED       0x0053

#define AEESMS_ERROR_COMMAND_PROBLEM               0x0060
#define AEESMS_ERROR_SC_BUSY                       0x0061
#define AEESMS_ERROR_NO_SC_SUBSCRIPTION            0x0062
#define AEESMS_ERROR_SC_FAILURE                    0x0063

#define AEESMS_ERROR_SMS_ORIGINATION_DENIED        0x0070
#define AEESMS_ERROR_SMS_NOT_SUPPORTED             0x0071
#define AEESMS_ERROR_SMS_TERMINATION_DENIED        0x0072
#define AEESMS_ERROR_SMS_DELIVERY_POSTPONED        0x0073

#define AEESMS_ERROR_LOWER_LAYER_FAILURE           0x0080
#define AEESMS_ERROR_MO_CONTROL_DISALLOW           0x0081
#define AEESMS_ERROR_MO_CONTROL_ERROR              0x0082
#define AEESMS_ERROR_MO_RETRY_PERIOD_EXPIRED       0x0083

#define AEESMS_ERROR_UNASSIGNED_NUMBER                    0x0090
#define AEESMS_ERROR_OPERATOR_DETERMINED_BARRING          0x0091
#define AEESMS_ERROR_CALL_BARRED                          0x0092
#define AEESMS_ERROR_SM_REJECTED                          0x0093
#define AEESMS_ERROR_DESTINATION_OUT_OF_ORDER             0x0094
#define AEESMS_ERROR_UNIDENTIFIED_SUBSCRIBER              0x0095
#define AEESMS_ERROR_FACILITY_REJECTED                    0x0096
#define AEESMS_ERROR_UNKNOWN_SUBSCRIBER                   0x0097
#define AEESMS_ERROR_NETWORK_OUT_OF_ORDER                 0x0098
#define AEESMS_ERROR_TEMPORARY_FAILURE                    0x0099
#define AEESMS_ERROR_CONGESTION                           0x009a
#define AEESMS_ERROR_RESOURCES_UNAVAILABLE_UNSPECIFIED    0x009b
#define AEESMS_ERROR_REQUESTED_FACILITY_NOT_SUBSCRIBED    0x009c
#define AEESMS_ERROR_REQUESTED_FACILITY_NOT_IMPLEMENTED   0x009d
#define AEESMS_ERROR_INVALID_SM_TRANSFER_REF_VALUE        0x009e
#define AEESMS_ERROR_SEMANTICALLY_INCORRECT_MESSAGE       0x009f
#define AEESMS_ERROR_INVALID_MANDATORY_INFORMATION        0x00a0
#define AEESMS_ERROR_MESSAGE_TYPE_NON_EXISTENT            0x00a1
#define AEESMS_ERROR_MESSAGE_TYPE_NOT_COMPATIBLE          0x00a2
#define AEESMS_ERROR_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED   0x00a3
#define AEESMS_ERROR_PROTOCOL_ERROR_UNSPECIFIED           0x00a4
#define AEESMS_ERROR_INTERWORKING_UNSPECIFIED             0x00a5

#define AEESMS_ERROR_OEM_DEFINED_FIRST             0x00d0
#define AEESMS_ERROR_OEM_DEFINED_LAST              0x00ff

#define AEESMS_GETERRORTYPE(n)                  (uint16)((n) >> 16)
#define AEESMS_GETERROR(n)                      (uint16)((n) & 0x0000ffff)
#define AEESMS_SETERROR(n,et,e)                 do {(n) = (((uint32)(et)) << 16); ((n) += (e));} while (0)

// Define identifiers for Domain Preference
#define AEESMS_GW_DOMAIN_PREF_CS_PREFERRED      0
#define AEESMS_GW_DOMAIN_PREF_PS_PREFERRED      1
#define AEESMS_GW_DOMAIN_PREF_CS_ONLY           2
#define AEESMS_GW_DOMAIN_PREF_PS_ONLY           3
#define AEESMS_GW_DOMAIN_PREF_LAST              4

// Define SMS events with associated event notify data
#define EVT_MDL_SMS_MSG_SENT                    (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_SMS_DUPLICATE_CB_PAGE           (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_SMS_CELL_CHANGE                 (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_SMS_CS_CHANGE                   (EVT_MDL_GENERIC_CHANGE + 4)

// Event data associated with EVT_MDL_SMS_CS_CHANGE
typedef struct
{
   AEESMSType         mt;
   AEESMSClientStatus oldCS;
   AEESMSClientStatus newCS;
} AEESMSCSChangeEvtData;

// Define ISMS interface
typedef struct ISMS         ISMS;

AEEINTERFACE(ISMS)
{
   INHERIT_IQueryInterface(ISMS);

   void (*SendMsg)(ISMS *po, ISMSMsg * pISMSMsg, AEECallback * pcb, uint32 *pnRv);
   int  (*ReceiveMsg)(ISMS *po, uint32 uMsgId, ISMSMsg **ppMsg);
   int  (*SetClientStatus)(ISMS *po, AEESMSClientStatus cs, AEESMSType mt); 
   int  (*GetBytesAvailableForPayload)(ISMS * po, ISMSMsg * pISMSMsg, int * pnBytes, int * pnPadding);
   int  (*GetEncodingsAvailableForMOSMS)(ISMS * po, uint32 * pBuffer, uint32 * pnSize);
};

#define ISMS_AddRef(p)                                   AEEGETPVTBL(p,ISMS)->AddRef((p))
#define ISMS_Release(p)                                  AEEGETPVTBL(p,ISMS)->Release((p))
#define ISMS_QueryInterface(p,i,o)                       AEEGETPVTBL(p,ISMS)->QueryInterface((p),(i),(o))
#define ISMS_SendMsg(p,pm,pc,pn)                         AEEGETPVTBL(p,ISMS)->SendMsg((p),(pm),(pc),(pn))
#define ISMS_ReceiveMsg(p,n,ppi)                         AEEGETPVTBL(p,ISMS)->ReceiveMsg((p),(n),(ppi))
#define ISMS_SetClientStatus(p,cs,mt)                    AEEGETPVTBL(p,ISMS)->SetClientStatus((p),(cs),(mt))
#define ISMS_GetBytesAvailableForPayload(p,pm,pnb,pnp)   AEEGETPVTBL(p,ISMS)->GetBytesAvailableForPayload((p),(pm),(pnb),(pnp))
#define ISMS_GetEncodingsAvailableForMOSMS(p,pb,pn)      AEEGETPVTBL(p,ISMS)->GetEncodingsAvailableForMOSMS((p),(pb),(pn))
//*********************************************************************
//
// ISMSNotifier Interface
//
//*********************************************************************
#define NMASK_SMS_TYPE           0x0001   // Arrival of SMS for specific message type
#define NMASK_SMS_BROADCAST      0x0002   // Arrival of broadcast SMS
#define NMASK_SMS_TYPE_LOOPBACK  0x0004   // Arrival of a loopback SMS 

typedef struct ISMSNotifier ISMSNotifier;

//*********************************************************************
//
// ISMSStorage Interface
//
//*********************************************************************
// Define identifiers for storage type

// Storage for point to point SMS Messages
#define AEESMS_CARD_RUIM      0
#define AEESMS_CARD_SIM       1
#define AEESMS_NV_CDMA        2
#define AEESMS_NV_GW          3

// Retained because this was defined once. Serves no purpose other than
// providing a define for 4.
#define AEESMS_STORAGE_LAST        4

// Storage for SMS Templates
#define AEESMS_CARD_RUIM_TEMPLATE  4
#define AEESMS_CARD_SIM_TEMPLATE   5
#define AEESMS_NV_CDMA_TEMPLATE    6
#define AEESMS_NV_GW_TEMPLATE      7

// Storage for SMS Status Reports
#define AEESMS_CARD_SIM_STATUS_REPORT  8

#define AEESMS_STORAGE_NONE 0xffffffff

// Define data type to refer to storage types
typedef uint32 AEESMSStorageType;

// Define storage events with associated event notify data
#define EVT_MDL_SMSSTORAGE_STORE        (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_SMSSTORAGE_UPDATE       (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_SMSSTORAGE_DELETE       (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_SMSSTORAGE_DELETE_TAG   (EVT_MDL_GENERIC_CHANGE + 4)
#define EVT_MDL_SMSSTORAGE_DELETE_ALL   (EVT_MDL_GENERIC_CHANGE + 5)
#define EVT_MDL_SMSSTORAGE_READY        (EVT_MDL_GENERIC_CHANGE + 6)
#define EVT_MDL_SMSSTORAGE_FULL         (EVT_MDL_GENERIC_CHANGE + 7)
#define EVT_MDL_SMSSTORAGE_CLASS_0_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 8)
#define EVT_MDL_SMSSTORAGE_CLASS_1_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 9)
#define EVT_MDL_SMSSTORAGE_CLASS_2_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 10)
#define EVT_MDL_SMSSTORAGE_CLASS_3_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 11)
#define EVT_MDL_SMSSTORAGE_CLASS_NONE_AUTOSTORE    (EVT_MDL_GENERIC_CHANGE + 12)
#define EVT_MDL_SMSSTORAGE_CLASS_CDMA_AUTOSTORE    (EVT_MDL_GENERIC_CHANGE + 13)

typedef struct
{
   AEESMSStorageType st;
   uint32 nIndex;
   AEESMSTag mt;
} AEESMSStorageEvtData;

typedef struct
{
   uint32                     nSize;
   uint32                     nMaxSlots;
   uint32                     nFreeSlots;
   uint32                     nUsedTagSlots;
} AEESMSStorageStatus;

#define INHERIT_ISMSStorage(iname)\
   INHERIT_IQueryInterface(iname); \
   void (*GetStorageStatus)(iname * po, AEESMSStorageType st, AEESMSTag mt, \
                            AEECallback *pcb, AEESMSStorageStatus * pss, uint32 *pnErr); \
   void (*StoreMsg)(iname * po, AEESMSStorageType st, ISMSMsg * pi, \
                    AEECallback *pcb, uint32 * pnIndex, uint32 *pnErr); \
   void (*ReadMsg)(iname * po, AEESMSStorageType st, uint32 nIndex, \
                   AEECallback *pcb, ISMSMsg ** ppi, uint32 *pnErr); \
   void (*Delete)(iname * po, AEESMSStorageType st, AEESMSTag mt, \
                  uint32 nIndex, AEECallback *pcb, uint32 *pnErr); \
   void (*EnumMsgInit)(iname * po, AEESMSStorageType st, AEESMSTag mt, \
                       AEECallback *pcb, uint32 *pnErr); \
   void (*EnumNextMsg)(iname * po, AEESMSStorageType st, AEECallback *pcb, \
                       uint32 * pnIndex, ISMSMsg ** ppi, uint32 *pnErr); \
   void (*UpdateMsg)(iname * po, AEESMSStorageType st, uint32 nIndex, \
                     ISMSMsg * pi, AEECallback *pcb, uint32 *pnErr)

AEEINTERFACE_DEFINE(ISMSStorage);

#define ISMSSTORAGE_AddRef(p)                               AEEGETPVTBL(p,ISMSStorage)->AddRef((p))
#define ISMSSTORAGE_Release(p)                              AEEGETPVTBL(p,ISMSStorage)->Release((p))
#define ISMSSTORAGE_QueryInterface(p,i,o)                   AEEGETPVTBL(p,ISMSStorage)->QueryInterface((p),(i),(o))
#define ISMSSTORAGE_GetStorageStatus(p,st,mt,pcb,pss,pe)    AEEGETPVTBL(p,ISMSStorage)->GetStorageStatus((p),(st),(mt),(pcb),(pss),(pe))
#define ISMSSTORAGE_StoreMsg(p,st,pi,pcb,pni,pe)            AEEGETPVTBL(p,ISMSStorage)->StoreMsg((p),(st),(pi),(pcb),(pni),(pe))
#define ISMSSTORAGE_ReadMsg(p,st,ni,pcb,ppi,pe)             AEEGETPVTBL(p,ISMSStorage)->ReadMsg((p),(st),(ni),(pcb),(ppi),(pe))
#define ISMSSTORAGE_DeleteAll(p,st,pcb,pe)                  AEEGETPVTBL(p,ISMSStorage)->Delete((p),(st),(AEESMS_TAG_NONE),(0xffffffff),(pcb),(pe))
#define ISMSSTORAGE_DeleteMsgTag(p,st,mt,pcb,pe)            AEEGETPVTBL(p,ISMSStorage)->Delete((p),(st),(mt),(0xffffffff),(pcb),(pe))
#define ISMSSTORAGE_DeleteMsg(p,st,ni,pcb,pe)               AEEGETPVTBL(p,ISMSStorage)->Delete((p),(st),(AEESMS_TAG_NONE),(ni),(pcb),(pe))
#define ISMSSTORAGE_EnumMsgInit(p,st,mt,pcb,pe)             AEEGETPVTBL(p,ISMSStorage)->EnumMsgInit((p),(st),(mt),(pcb),(pe))
#define ISMSSTORAGE_EnumNextMsg(p,st,pcb,pni,ppi,pe)        AEEGETPVTBL(p,ISMSStorage)->EnumNextMsg((p),(st),(pcb),(pni),(ppi),(pe))
#define ISMSSTORAGE_UpdateMsg(p,st,ni,pi,pcb,pe)            AEEGETPVTBL(p,ISMSStorage)->UpdateMsg((p),(st),(ni),(pi),(pcb),(pe))

#define AEEIID_SMSSTORAGE2    	  0x01038cc8

#define INHERIT_ISMSStorage2(iname)\
   INHERIT_ISMSStorage(iname); \
   void (*AutoStoreMsg)(iname * po, AEESMSClass mc, AEESMSStorageType st, \
                        AEECallback *pcb, uint32 *pnErr)

AEEINTERFACE_DEFINE(ISMSStorage2);

#define ISMSSTORAGE2_AddRef(p)                               AEEGETPVTBL(p,ISMSStorage2)->AddRef((p))
#define ISMSSTORAGE2_Release(p)                              AEEGETPVTBL(p,ISMSStorage2)->Release((p))
#define ISMSSTORAGE2_QueryInterface(p,i,o)                   AEEGETPVTBL(p,ISMSStorage2)->QueryInterface((p),(i),(o))
#define ISMSSTORAGE2_GetStorageStatus(p,st,mt,pcb,pss,pe)    AEEGETPVTBL(p,ISMSStorage2)->GetStorageStatus((p),(st),(mt),(pcb),(pss),(pe))
#define ISMSSTORAGE2_StoreMsg(p,st,pi,pcb,pni,pe)            AEEGETPVTBL(p,ISMSStorage2)->StoreMsg((p),(st),(pi),(pcb),(pni),(pe))
#define ISMSSTORAGE2_ReadMsg(p,st,ni,pcb,ppi,pe)             AEEGETPVTBL(p,ISMSStorage2)->ReadMsg((p),(st),(ni),(pcb),(ppi),(pe))
#define ISMSSTORAGE2_DeleteAll(p,st,pcb,pe)                  AEEGETPVTBL(p,ISMSStorage2)->Delete((p),(st),(AEESMS_TAG_NONE),(0xffffffff),(pcb),(pe))
#define ISMSSTORAGE2_DeleteMsgTag(p,st,mt,pcb,pe)            AEEGETPVTBL(p,ISMSStorage2)->Delete((p),(st),(mt),(0xffffffff),(pcb),(pe))
#define ISMSSTORAGE2_DeleteMsg(p,st,ni,pcb,pe)               AEEGETPVTBL(p,ISMSStorage2)->Delete((p),(st),(AEESMS_TAG_NONE),(ni),(pcb),(pe))
#define ISMSSTORAGE2_EnumMsgInit(p,st,mt,pcb,pe)             AEEGETPVTBL(p,ISMSStorage2)->EnumMsgInit((p),(st),(mt),(pcb),(pe))
#define ISMSSTORAGE2_EnumNextMsg(p,st,pcb,pni,ppi,pe)        AEEGETPVTBL(p,ISMSStorage2)->EnumNextMsg((p),(st),(pcb),(pni),(ppi),(pe))
#define ISMSSTORAGE2_UpdateMsg(p,st,ni,pi,pcb,pe)            AEEGETPVTBL(p,ISMSStorage2)->UpdateMsg((p),(st),(ni),(pi),(pcb),(pe))
#define ISMSSTORAGE2_AutoStoreMsg(p,mc,st,pcb,pe)            AEEGETPVTBL(p,ISMSStorage2)->AutoStoreMsg((p),(mc),(st),(pcb),(pe))

static __inline int ISMSSTORAGE_AutoStoreMsg(ISMSStorage * po, AEESMSClass mc, AEESMSStorageType st, AEECallback *pcb, uint32 *pnErr)
{
   int nErr;
   ISMSStorage2 * pi = NULL;

   nErr = ISMSSTORAGE_QueryInterface(po, AEEIID_SMSSTORAGE2, (void**)&pi);
   if (AEE_SUCCESS == nErr) {
      ISMSSTORAGE2_AutoStoreMsg(pi, mc, st, pcb, pnErr);
      ISMSSTORAGE2_Release(pi);
   }
   return nErr;
}

//**********************************************************************
//
// ISMSBCSrvOpts
//
//**********************************************************************
typedef WebOpt SMSBCSrvOpts;
typedef struct ISMSBCSrvOpts      ISMSBCSrvOpts;

// Define broadcast alert type
#define AEESMS_BC_ALERT_NONE                    0
#define AEESMS_BC_ALERT_DEFAULT                 1
#define AEESMS_BC_ALERT_VIBRATE_ONCE            2
#define AEESMS_BC_ALERT_VIBRATE_REPEAT          3
#define AEESMS_BC_ALERT_VISUAL_ONCE             4
#define AEESMS_BC_ALERT_VISUAL_REPEAT           5
#define AEESMS_BC_ALERT_LOW_PRIORITY_ONCE       6
#define AEESMS_BC_ALERT_LOW_PRIORITY_REPEAT     7
#define AEESMS_BC_ALERT_MEDIUM_PRIORITY_ONCE    8
#define AEESMS_BC_ALERT_MEDIUM_PRIORITY_REPEAT  9
#define AEESMS_BC_ALERT_HIGH_PRIORITY_ONCE      10
#define AEESMS_BC_ALERT_HIGH_PRIORITY_REPEAT    11

// Define data type to refer to broadcast alert type
typedef uint32 AEESMSBCAlertType;

// Define ServiceID structure
typedef struct
{
   AEESMSBCType srvType;
   uint32       dwLang;
   uint16       fromService;
   uint16       toService;
} AEESMSBCSrvID;

// Reserve ISMSBCSrvOpts chunk of the option ID space
#define SMSBCSRVOPT_LABEL_SZ         (WEBOPT_SZ + 0)

#define SMSBCSRVOPT_LABEL_WSZ        (WEBOPT_WSZ + 0)

#define SMSBCSRVOPT_SELECTED         (WEBOPT_32BIT + 0)
#define SMSBCSRVOPT_PRIORITY         (WEBOPT_32BIT + 1)
#define SMSBCSRVOPT_LABEL_ENCODING   (WEBOPT_32BIT + 2)
#define SMSBCSRVOPT_ALERT            (WEBOPT_32BIT + 3)
#define SMSBCSRVOPT_MAX_MSGS         (WEBOPT_32BIT + 4)

#define SMSBCSRVOPT_SERVICEID        (WEBOPT_VARIABLE + sizeof(AEESMSBCSrvID))

#define SMSBCSRVOPT_END              WEBOPT_END
#define SMSBCSRVOPT_ANY              WEBOPT_ANY
#define SMSBCSRVOPT_COPYOPTS         WEBOPT_COPYOPTS

AEEINTERFACE(ISMSBCSrvOpts)
{
   INHERIT_IWebOpts(ISMSBCSrvOpts);
};

#define ISMSBCSRVOPTS_AddRef(p)                       AEEGETPVTBL((p),ISMSBCSrvOpts)->AddRef((p))
#define ISMSBCSRVOPTS_Release(p)                      AEEGETPVTBL((p),ISMSBCSrvOpts)->Release((p))
#define ISMSBCSRVOPTS_QueryInterface(p,clsid,pp)      AEEGETPVTBL((p),ISMSBCSrvOpts)->QueryInterface((p),(clsid),(pp))
#define ISMSBCSRVOPTS_AddOpt(p,o)                     AEEGETPVTBL((p),ISMSBCSrvOpts)->AddOpt((p),(o))
#define ISMSBCSRVOPTS_RemoveOpt(p,n)                  AEEGETPVTBL((p),ISMSBCSrvOpts)->RemoveOpt((p),(n),(0))
#define ISMSBCSRVOPTS_GetOpt(p,n,o)                   AEEGETPVTBL((p),ISMSBCSrvOpts)->GetOpt((p),(n),(0),(o))

//*********************************************************************
//
// ISMSBCConfig Interface
//
//*********************************************************************
// Define broadcast configurations
#define AEESMS_BC_CONFIG_DISALLOW         0
#define AEESMS_BC_CONFIG_ALLOW_TABLE      1
#define AEESMS_BC_CONFIG_ALLOW_ALL        2

// Define data type to refer to broadcast configurations
typedef uint32 AEESMSBCConfig;

// Define broadcast preferences
#define AEESMS_BC_PREF_DISALLOW           0
#define AEESMS_BC_PREF_ALLOW_TABLE        1
#define AEESMS_BC_PREF_ALLOW_ALL          2

// Define data type to refer to broadcast preferences
typedef uint32 AEESMSBCPref;

// Define broadcast message mode
#define AEESMS_MODE_CDMA                  0
#define AEESMS_MODE_GW                    1

// Define data type to refer to broadcast message mode
typedef uint32 AEESMSMode;

typedef struct
{
   AEESMSMode             mode;        // Service Mode 
   AEESMSBCSrvID          sSrvID;      // Service ID  
} AEESMSBCConfigEvtData;

// Define SMSBCConfig events with associated event notify data
#define EVT_MDL_SMSBCCONFIG_CONFIG                    (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_SMSBCCONFIG_PREF                      (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_SMSBCCONFIG_ADDED                     (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_SMSBCCONFIG_UPDATED                   (EVT_MDL_GENERIC_CHANGE + 4)
#define EVT_MDL_SMSBCCONFIG_DELETED                   (EVT_MDL_GENERIC_CHANGE + 5)
#define EVT_MDL_SMSBCCONFIG_SELECT_ALL_SRVS           (EVT_MDL_GENERIC_CHANGE + 6)
#define EVT_MDL_SMSBCCONFIG_SET_PRIORITY_ALL_SRVS     (EVT_MDL_GENERIC_CHANGE + 7)
#define EVT_MDL_SMSBCCONFIG_DELETE_ALL_SRVS           (EVT_MDL_GENERIC_CHANGE + 8)

typedef struct ISMSBCConfig         ISMSBCConfig;

AEEINTERFACE(ISMSBCConfig)
{
   INHERIT_IQueryInterface(ISMSBCConfig);
   void     (*GetBCConfig)(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr);
   void     (*GetBCPref)(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr);
   void     (*SetBCPref)(ISMSBCConfig * po, AEESMSMode mode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr);
   void     (*GetServiceIDs)(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, AEESMSBCSrvID ** ppsid, uint32 * pnErr);
   void     (*GetServiceOpts)(ISMSBCConfig * po, AEESMSMode mode, AEESMSBCSrvID srvID, AEECallback *pcb, ISMSBCSrvOpts ** ppi, uint32 * pnErr);
   void     (*SetServiceOpts)(ISMSBCConfig * po, AEESMSMode mode, ISMSBCSrvOpts * pi, AEECallback *pcb, uint32 * pnErr);
   void     (*GetAllServiceOpts)(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, ISMSBCSrvOpts *** pppi, uint32 * pnErr);
   void     (*DeleteService)(ISMSBCConfig * po, AEESMSMode mode, AEESMSBCSrvID srvID, AEECallback *pcb, uint32 * pnErr);
};

#define ISMSBCCONFIG_AddRef(p)                              AEEGETPVTBL(p,ISMSBCConfig)->AddRef((p))
#define ISMSBCCONFIG_Release(p)                             AEEGETPVTBL(p,ISMSBCConfig)->Release((p))
#define ISMSBCCONFIG_QueryInterface(p,i,o)                  AEEGETPVTBL(p,ISMSBCConfig)->QueryInterface((p),(i),(o))
#define ISMSBCCONFIG_GetBCConfig(p,m,pcb,pc,pe)             AEEGETPVTBL(p,ISMSBCConfig)->GetBCConfig((p),(m),(pcb),(pc),(pe))
#define ISMSBCCONFIG_GetBCPref(p,m,pcb,pp,pe)               AEEGETPVTBL(p,ISMSBCConfig)->GetBCPref((p),(m),(pcb),(pp),(pe))
#define ISMSBCCONFIG_SetBCPref(p,m,np,pcb,pe)               AEEGETPVTBL(p,ISMSBCConfig)->SetBCPref((p),(m),(np),(pcb),(pe))
#define ISMSBCCONFIG_GetServiceIDs(p,m,pcb,pnc,ppsid,pe)    AEEGETPVTBL(p,ISMSBCConfig)->GetServiceIDs((p),(m),(pcb),(pnc),(ppsid),(pe))
#define ISMSBCCONFIG_GetServiceOpts(p,m,sid,pcb,ppi,pe)     AEEGETPVTBL(p,ISMSBCConfig)->GetServiceOpts((p),(m),(sid),(pcb),(ppi),(pe))
#define ISMSBCCONFIG_SetServiceOpts(p,m,pi,pcb,pe)          AEEGETPVTBL(p,ISMSBCConfig)->SetServiceOpts((p),(m),(pi),(pcb),(pe))
#define ISMSBCCONFIG_GetAllServiceOpts(p,m,pcb,pnc,pppi,pe) AEEGETPVTBL(p,ISMSBCConfig)->GetAllServiceOpts((p),(m),(pcb),(pnc),(pppi),(pe))
#define ISMSBCCONFIG_DeleteService(p,m,sid,pcb,pe)          AEEGETPVTBL(p,ISMSBCConfig)->DeleteService((p),(m),(sid),(pcb),(pe))



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEMSGPriority

Description:
   Specifies the priority of the message.

Definition:
enum {
   AEESMS_PRIORITY_NORMAL,
   AEESMS_PRIORITY_INTERACTIVE,
   AEESMS_PRIORITY_URGENT,
   AEESMS_PRIORITY_EMERGENCY
};
typedef uint32 AEEMSGPriority;

Members:
   AEESMS_PRIORITY_NORMAL:
   AEESMS_PRIORITY_INTERACTIVE:
   AEESMS_PRIORITY_URGENT:
   AEESMS_PRIORITY_EMERGENCY:

Comments:
   None

See Also:
   None

=======================================================================
AEEMSGPrivacy

Description:
   Specifies the Privacy of the message.

Definition:

enum {
   AEESMS_PRIVACY_NORMAL,
   AEESMS_PRIVACY_RESTRICTED,
   AEESMS_PRIVACY_CONFIDENTIAL,
   AEESMS_PRIVACY_SECRET
};
typedef uint32 AEEMSGPrivacy;

Members:

   AEESMS_PRIVACY_NORMAL:
   AEESMS_PRIVACY_RESTRICTED:
   AEESMS_PRIVACY_CONFIDENTIAL:
   AEESMS_PRIVACY_SECRET:

Comments:
   None

See Also:
   None

=======================================================================
AEESMSClass

Description:
   Specifies the class of the message which can be used by application to
decide storage option for the message.

Definition:

enum
{
   AEESMS_CLASS_0,
   AEESMS_CLASS_1,
   AEESMS_CLASS_2,
   AEESMS_CLASS_3,
   AEESMS_CLASS_NONE,
   AEESMS_CLASS_CDMA
};
typedef uint32 AEESMSClass;

Members:

   AEESMS_CLASS_0    : GSM/UMTS Class 0 message
   AEESMS_CLASS_1    : GSM/UMTS Class 1 message
   AEESMS_CLASS_2    : GSM/UMTS Class 2 message
   AEESMS_CLASS_3    : GSM/UMTS Class 3 message
   AEESMS_CLASS_NONE : No message class
   AEESMS_CLASS_CDMA : CDMA message

Comments:
   None

See Also:
   None

=======================================================================
AEESMSTag

Description:
   Specifies the message tag.

Definition:

enum
{
   AEESMS_TAG_NONE,
   AEESMS_TAG_MT_READ,
   AEESMS_TAG_MT_NOT_READ,
   AEESMS_TAG_MO_SENT,
   AEESMS_TAG_MO_NOT_SENT,
   AEESMS_TAG_MO_SENT_ST_NOT_RECEIVED,
   AEESMS_TAG_MO_SENT_ST_NOT_STORED,
   AEESMS_TAG_MO_SENT_ST_STORED,
   AEESMS_TAG_MO_TEMPLATE,
   AEESMS_TAG_STATUS_RPT
};
typedef uint32 AEESMSTag;

Members:

   AEESMS_TAG_NONE                     : No status tag. This is treated as don't care for the cases
                                         where tag sets a qualification criteria for specified action
   AEESMS_TAG_MT_READ                  : Message is read
   AEESMS_TAG_MT_NOT_READ              : Message is not read
   AEESMS_TAG_MO_SENT                  : Message is sent
   AEESMS_TAG_MO_NOT_SENT              : Message is not sent
   AEESMS_TAG_MO_SENT_ST_NOT_RECEIVED  : Message sent but status not received. GSM/UMTS specific.
   AEESMS_TAG_MO_SENT_ST_NOT_STORED    : Message sent, status received but not stored. GSM/UMTS specific.
   AEESMS_TAG_MO_SENT_ST_STORED        : Message sent, status received and stored. GSM/UMTS specific.
   AEESMS_TAG_MO_TEMPLATE              : Message is template for mobile originated messages.
   AEESMS_TAG_STATUS_RPT               : Message is status report

Comments:
   None

See Also:
   None

=======================================================================
AEESMSPID

Description:
   Specifies the Protocol Identifier in a GSM / WCDMA Message

Definition:

enum
{
  AEESMS_PID_DEFAULT           = 0x00,
  AEESMS_PID_IMPLICIT          = 0x20,
  AEESMS_PID_TELEX             = 0x21,
  AEESMS_PID_G3_FAX            = 0x22,
  AEESMS_PID_G4_FAX            = 0x23,
  AEESMS_PID_VOICE_PHONE       = 0x24,
  AEESMS_PID_ERMES             = 0x25,
  AEESMS_PID_NAT_PAGING        = 0x26,
  AEESMS_PID_VIDEOTEX          = 0x27,
  AEESMS_PID_TELETEX_UNSPEC    = 0x28,
  AEESMS_PID_TELETEX_PSPDN     = 0x29,
  AEESMS_PID_TELETEX_CSPDN     = 0x2a,
  AEESMS_PID_TELETEX_PSTN      = 0x2b,
  AEESMS_PID_TELETEX_ISDN      = 0x2c,
  AEESMS_PID_UCI               = 0x2d,
  AEESMS_PID_RESERVED_0x1e     = 0x2e,
  AEESMS_PID_RESERVED_0x1f     = 0x2f,
  AEESMS_PID_MSG_HANDLING      = 0x30,
  AEESMS_PID_X400              = 0x31,
  AEESMS_PID_INTERNET_EMAIL    = 0x32,
  AEESMS_PID_RESERVED_0x33     = 0x33,
  AEESMS_PID_RESERVED_0x34     = 0x34,
  AEESMS_PID_RESERVED_0x35     = 0x35,
  AEESMS_PID_RESERVED_0x36     = 0x36,
  AEESMS_PID_RESERVED_0x37     = 0x37,
  AEESMS_PID_SC_SPECIFIC_1     = 0x38,
  AEESMS_PID_SC_SPECIFIC_2     = 0x39,
  AEESMS_PID_SC_SPECIFIC_3     = 0x3a,
  AEESMS_PID_SC_SPECIFIC_4     = 0x3b,
  AEESMS_PID_SC_SPECIFIC_5     = 0x3c,
  AEESMS_PID_SC_SPECIFIC_6     = 0x3d,
  AEESMS_PID_SC_SPECIFIC_7     = 0x3e,
  AEESMS_PID_GSM_UMTS          = 0x3f,
  AEESMS_PID_SM_TYPE_0         = 0x40,
  AEESMS_PID_REPLACE_SM_1      = 0x41,
  AEESMS_PID_REPLACE_SM_2      = 0x42,
  AEESMS_PID_REPLACE_SM_3      = 0x43,
  AEESMS_PID_REPLACE_SM_4      = 0x44,
  AEESMS_PID_REPLACE_SM_5      = 0x45,
  AEESMS_PID_REPLACE_SM_6      = 0x46,
  AEESMS_PID_REPLACE_SM_7      = 0x47,
  AEESMS_PID_EMS               = 0x5e,
  AEESMS_PID_RETURN_CALL       = 0x5f,
  AEESMS_PID_ANSI136_R_DATA    = 0x7c,
  AEESMS_PID_ME_DATA_DOWNLOAD  = 0x7d,
  AEESMS_PID_ME_DEPERSONALIZE  = 0x7e,
  AEESMS_PID_SIM_DATA_DOWNLOAD = 0x7f
};
typedef uint32 AEESMSPID;

Members:
   As defined in GSM / WCDMA Messages - ref 3GPP TS 23.040, section 9.2.3.9

Comments:
   None

See Also:
   None
=======================================================================
AEESMSNumberType

Description:
   Specifies the number type

Definition:
   #define AEESMS_NUMBER_UNKNOWN          0
   #define AEESMS_NUMBER_INTERNATIONAL    1
   #define AEESMS_NUMBER_NATIONAL         2
   #define AEESMS_NUMBER_NETWORK          3
   #define AEESMS_NUMBER_SUBSCRIBER       4
   #define AEESMS_NUMBER_ALPHANUMERIC     5
   #define AEESMS_NUMBER_ABBREVIATED      6

   // Define a data type to refer to Number Type
   typedef uint32 AEESMSNumberType

Members:
   Number Types

Comments:
   None

See Also:
   None

=======================================================================
AEESMSNumberPlan

Description:
   Specifies the Number Plan

Definition:
   #define AEESMS_NUMBER_PLAN_UNKNOWN     0
   #define AEESMS_NUMBER_PLAN_TELEPHONY   1  // CCITT E.164 and E.163, including ISDN plan
   #define AEESMS_NUMBER_PLAN_DATA        3  // CCITT X.121
   #define AEESMS_NUMBER_PLAN_TELEX       4  // CCITT F.69
   #define AEESMS_NUMBER_PLAN_PRIVATE     9

   // Define a data type to refer to Number Plan
   typedef uint32 AEESMSNumberPlan

Members:
   Number Plan Types

Comments:
   None

See Also:
   None

=======================================================================
AEESMSReportStatus

Description:
   Specifies the status for status report

Definition:

enum
{
  AEESMS_TP_STATUS_RECEIVED_OK                   = 0x00,
  AEESMS_TP_STATUS_UNABLE_TO_CONFIRM_DELIVERY    = 0x01,
  AEESMS_TP_STATUS_REPLACED                      = 0x02,
  AEESMS_TP_STATUS_TRYING_CONGESTION             = 0x20,
  AEESMS_TP_STATUS_TRYING_SME_BUSY               = 0x21,
  AEESMS_TP_STATUS_TRYING_NO_RESPONSE_FROM_SME   = 0x22,
  AEESMS_TP_STATUS_TRYING_SERVICE_REJECTED       = 0x23,
  AEESMS_TP_STATUS_TRYING_QOS_NOT_AVAILABLE      = 0x24,
  AEESMS_TP_STATUS_TRYING_SME_ERROR              = 0x25,
  AEESMS_TP_STATUS_PERM_REMOTE_PROCEDURE_ERROR   = 0x40,
  AEESMS_TP_STATUS_PERM_INCOMPATIBLE_DEST        = 0x41,
  AEESMS_TP_STATUS_PERM_REJECTED_BY_SME          = 0x42,
  AEESMS_TP_STATUS_PERM_NOT_OBTAINABLE           = 0x43,
  AEESMS_TP_STATUS_PERM_QOS_NOT_AVAILABLE        = 0x44,
  AEESMS_TP_STATUS_PERM_NO_INTERWORKING          = 0x45,
  AEESMS_TP_STATUS_PERM_VP_EXPIRED               = 0x46,
  AEESMS_TP_STATUS_PERM_DELETED_BY_ORIG_SME      = 0x47,
  AEESMS_TP_STATUS_PERM_DELETED_BY_SC_ADMIN      = 0x48,
  AEESMS_TP_STATUS_PERM_SM_NO_EXISTING           = 0x49,
  AEESMS_TP_STATUS_TEMP_CONGESTION               = 0x60,
  AEESMS_TP_STATUS_TEMP_SME_BUSY                 = 0x61,
  AEESMS_TP_STATUS_TEMP_NO_RESPONSE_FROM_SME     = 0x62,
  AEESMS_TP_STATUS_TEMP_SERVICE_REJECTED         = 0x63,
  AEESMS_TP_STATUS_TEMP_QOS_NOT_AVAILABLE        = 0x64,
  AEESMS_TP_STATUS_TEMP_SME_ERROR                = 0x65,
  AEESMS_TP_STATUS_LAST                          = 0xFF
};
typedef uint32 AEESMSReportStatus;

Members:
   TP-Status - ref. 3GPP TS 23.040 section 9.2.3.15

Comments:
   None

See Also:
   None

=======================================================================

AEESMSType

Description:
   Specifies the message type.

Definition:

enum
{
   AEESMS_TYPE_UNKNOWN = 0,
   AEESMS_TYPE_TEXT,
   AEESMS_TYPE_PAGE,
   AEESMS_TYPE_VOICE_MAIL_NOTIFICATION,
   AEESMS_TYPE_BROADCAST,
   AEESMS_TYPE_WAP,
   AEESMS_TYPE_EMS,
   AEESMS_TYPE_ANY = 0xffff
};
typedef uint32 AEESMSType;

Members:

   AEESMS_TYPE_UNKNOWN: Incoming SMS Message has a unknown or unrecognised type.

   AEESMS_TYPE_TEXT: Incoming message is CDMA Teleservice CMT_95 Message, or 
                     IS91_SHORT_MESSAGE or GSM SMS Text Message.

   AEESMS_TYPE_PAGE: Incoming message is CDMA Teleservice CPT_95 or IS91 CLI Message

   AEESMS_TYPE_VOICE_MAIL_NOTIFICATION: Incoming message is CDMA Teleservice VMN_95
                                       or IS91 Voice Mail Notification or analog MWI
                                       or GSM SMS Message with Message Waiting Indication.

   AEESMS_TYPE_BROADCAST: Incoming message is broadcast message

   AEESMS_TYPE_WAP: Incoming message is a WAP push message

   AEESMS_TYPE_EMS: Incoming message is an EMS message

   AEESMS_TYPE_ANY: Useful only with ISMS_SetClientStatus().

Comments:
   None

See Also:
   None

=======================================================================
AEESMSMsgWaitingType

Description:
   Specifies the message waiting.

Definition:

enum 
{
   AEESMS_MSGWAITINGTYPE_NONE = 0,
   AEESMS_MSGWAITINGTYPE_DISCARD,
   AEESMS_MSGWAITINGTYPE_STORE,
   AEESMS_MSGWAITINGTYPE_NONE_1111
};
typedef uint8 AEESMSMsgWaitingType;

Members:

   AEESMS_MSGWAITINGTYPE_NONE: No Message Waiting

   AEESMS_MSGWAITINGTYPE_DISCARD: Do not store the Message Waiting

   AEESMS_MSGWAITINGTYPE_STORE: Store the Message Waiting

   AEESMS_MSGWAITINGTYPE_NONE_1111: No Message Waiting (1111)

Comments:
   None

See Also:
   None

======================================================================= 
AEESMSMsgWaitingKind

Description:
   Specifies the message waiting kind.

Definition:

#define  AEESMS_MSGWAITINGKIND_VOICEMAIL              0
#define  AEESMS_MSGWAITINGKIND_FAX                    1
#define  AEESMS_MSGWAITINGKIND_EMAIL                  2
#define  AEESMS_MSGWAITINGKIND_OTHER                  3
#define  AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE     0x10
#define  AEESMS_MSGWAITINGKIND_FAX_INACTIVE           0x11 
#define  AEESMS_MSGWAITINGKIND_EMAIL_INACTIVE         0x12
#define  AEESMS_MSGWAITINGKIND_OTHER_INACTIVE         0x13
#define  AEESMS_MSGWAITINGKIND_NONE                   0xff

Members:

   AEESMS_MSGWAITINGKIND_VOICEMAIL: Voicemail Message Waiting

   AEESMS_MSGWAITINGKIND_FAX: Fax Message Waiting

   AEESMS_MSGWAITINGKIND_EMAIL: Email Message Waiting

   AEESMS_MSGWAITINGKIND_OTHER: Other Message Waiting

   AEESMS_MSGWAITINGKIND_VOICEMAIL_INACTIVE: Inactive Voicemail Message Waiting

   AEESMS_MSGWAITINGKIND_FAX_INACTIVE: Inactive Fax Message Waiting

   AEESMS_MSGWAITINGKIND_EMAIL_INACTIVE: Inactive Email Message Waiting

   AEESMS_MSGWAITINGKIND_OTHER_INACTIVE: Inactive Other Message Waiting

   AEESMS_MSGWAITINGKIND_NONE: Used as default when message waiting kind is not specified

Comments:
   None

See Also:
   None

======================================================================= 
AEESMSUDHId

Description:
   Specifies the message user data header ID.

Definition:

enum 
{
   AEESMS_UDH_CONCAT_8 = 0,
   AEESMS_UDH_SPECIAL_SM = 1,
   AEESMS_UDH_PORT_8 = 4,
   AEESMS_UDH_PORT_16 = 5, 
   AEESMS_UDH_SMSC_CONTROL = 6,
   AEESMS_UDH_SOURCE = 7,
   AEESMS_UDH_CONCAT_16 = 8,
   AEESMS_UDH_WCMP = 9,
   AEESMS_UDH_TEXT_FORMATING = 10,
   AEESMS_UDH_PRE_DEF_SOUND = 11,
   AEESMS_UDH_USER_DEF_SOUND = 12,
   AEESMS_UDH_PRE_DEF_ANIM = 13,
   AEESMS_UDH_LARGE_ANIM = 14,
   AEESMS_UDH_SMALL_ANIM = 15,
   AEESMS_UDH_LARGE_PICTURE = 16,
   AEESMS_UDH_SMALL_PICTURE = 17,
   AEESMS_UDH_VAR_PICTURE = 18,
   AEESMS_UDH_RFC822 = 32,
};
typedef uint8 AEESMSUDHId;

Members:
   AEESMS_UDH_CONCAT_8:
   AEESMS_UDH_SPECIAL_SM:
   AEESMS_UDH_PORT_8:
   AEESMS_UDH_PORT_16: 
   AEESMS_UDH_SMSC_CONTROL:
   AEESMS_UDH_SOURCE:
   AEESMS_UDH_CONCAT_16:
   AEESMS_UDH_WCMP:
   AEESMS_UDH_TEXT_FORMATING:
   AEESMS_UDH_PRE_DEF_SOUND:
   AEESMS_UDH_USER_DEF_SOUND:
   AEESMS_UDH_PRE_DEF_ANIM:
   AEESMS_UDH_LARGE_ANIM:
   AEESMS_UDH_SMALL_ANIM:
   AEESMS_UDH_LARGE_PICTURE:
   AEESMS_UDH_SMALL_PICTURE:
   AEESMS_UDH_VAR_PICTURE:
   AEESMS_UDH_RFC822:

Comments:
   None

See Also:
   None

======================================================================= 
AEESMSUDHTextColorId

Description:
   Specifies the text color information for AEESMS_UDH_TEXT_FORMATING.

Definition:

   #define  AEESMS_UDH_TEXT_COLOR_BLACK          0x0
   #define  AEESMS_UDH_TEXT_COLOR_DARK_GREY      0x1
   #define  AEESMS_UDH_TEXT_COLOR_DARK_RED       0x2
   #define  AEESMS_UDH_TEXT_COLOR_DARK_YELLOW    0x3
   #define  AEESMS_UDH_TEXT_COLOR_DARK_GREEN     0x4
   #define  AEESMS_UDH_TEXT_COLOR_DARK_CYAN      0x5
   #define  AEESMS_UDH_TEXT_COLOR_DARK_BLUE      0x6
   #define  AEESMS_UDH_TEXT_COLOR_DARK_MAGENTA   0x7
   #define  AEESMS_UDH_TEXT_COLOR_GREY           0x8
   #define  AEESMS_UDH_TEXT_COLOR_WHITE          0x9
   #define  AEESMS_UDH_TEXT_COLOR_BRIGHT_RED     0xA
   #define  AEESMS_UDH_TEXT_COLOR_BRIGHT_YELLOW  0xB
   #define  AEESMS_UDH_TEXT_COLOR_BRIGHT_GREEN   0xC
   #define  AEESMS_UDH_TEXT_COLOR_BRIGHT_CYAN    0xD
   #define  AEESMS_UDH_TEXT_COLOR_BRIGHT_BLUE    0xE
   #define  AEESMS_UDH_TEXT_COLOR_BRIGHT_MAGENTA 0xF

   // Define a data type to refer UDH Text Color ID
   typedef uint8 AEESMSUDHTextColorId;

Members:
   None

Comments:
   None

See Also:
   None

=======================================================================
AEESMSBCType

Description:
   Specifies the message broadcast type for CDMA messages

Definition:

enum
{
   AEESMS_BC_UNKOWN          = 0,
   AEESMS_BC_EMERGENCY,      // 1
   AEESMS_BC_ADMIN,          // 2
   AEESMS_BC_MAINTENANCE,    // 3
   AEESMS_BC_GEN_NEWS_LOC,   // 4
   AEESMS_BC_GEN_NEWS_REG,   // 5
   AEESMS_BC_GEN_NEWS_NAT,   // 6
   AEESMS_BC_GEN_NEWS_INT,   // 7
   AEESMS_BC_FIN_NEWS_LOC,   // 8
   AEESMS_BC_FIN_NEWS_REG,   // 9
   AEESMS_BC_FIN_NEWS_NAT,   // 0xA
   AEESMS_BC_FIN_NEWS_INT,   // 0xB
   AEESMS_BC_SPT_NEWS_LOC,   // 0xC
   AEESMS_BC_SPT_NEWS_REG,   // 0xD
   AEESMS_BC_SPT_NEWS_NAT,   // 0xE
   AEESMS_BC_SPT_NEWS_INT,   // 0xF
   AEESMS_BC_ENT_NEWS_LOC,   // 0x10
   AEESMS_BC_ENT_NEWS_REG,   // 0x11
   AEESMS_BC_ENT_NEWS_NAT,   // 0x12
   AEESMS_BC_ENT_NEWS_INT,   // 0x13
   AEESMS_BC_LOC_WEATHER,    // 0x14
   AEESMS_BC_AREA_TRAFFIC,   // 0x15
   AEESMS_BC_AIRPORT_SCHED,  // 0x16
   AEESMS_BC_RESTAURANTS,    // 0x17
   AEESMS_BC_LODGINGS,       // 0x18
   AEESMS_BC_RETAILS,        // 0x19
   AEESMS_BC_ADS,            // 0x1A
   AEESMS_BC_STOCK_QUOTES,   // 0x1B
   AEESMS_BC_JOBS,           // 0x1C
   AEESMS_BC_MEDICAL,        // 0x1D
   AEESMS_BC_TECH_NEWS,      // 0x1E
   AEESMS_BC_MULTI,          // 0x1F
   AEESMS_BC_MAX             = 0xffff
};
typedef uint32 AEESMSBCType;

Members:
   AEESMS_BC_UNKOWN:
   AEESMS_BC_EMERGENCY:
   AEESMS_BC_ADMIN:
   AEESMS_BC_MAINTENANCE:
   AEESMS_BC_GEN_NEWS_LOC:
   AEESMS_BC_GEN_NEWS_REG:
   AEESMS_BC_GEN_NEWS_NAT:
   AEESMS_BC_GEN_NEWS_INT:
   AEESMS_BC_FIN_NEWS_LOC:
   AEESMS_BC_FIN_NEWS_REG:
   AEESMS_BC_FIN_NEWS_NAT:
   AEESMS_BC_FIN_NEWS_INT:
   AEESMS_BC_SPT_NEWS_LOC:
   AEESMS_BC_SPT_NEWS_REG:
   AEESMS_BC_SPT_NEWS_NAT:
   AEESMS_BC_SPT_NEWS_INT:
   AEESMS_BC_ENT_NEWS_LOC:
   AEESMS_BC_ENT_NEWS_REG:
   AEESMS_BC_ENT_NEWS_NAT:
   AEESMS_BC_ENT_NEWS_INT:
   AEESMS_BC_LOC_WEATHER:
   AEESMS_BC_AREA_TRAFFIC:   
   AEESMS_BC_AIRPORT_SCHED:
   AEESMS_BC_RESTAURANTS:
   AEESMS_BC_LODGINGS:
   AEESMS_BC_RETAILS:
   AEESMS_BC_ADS:
   AEESMS_BC_STOCK_QUOTES:
   AEESMS_BC_JOBS:
   AEESMS_BC_MEDICAL:
   AEESMS_BC_TECH_NEWS:
   AEESMS_BC_MULTI:

Comments:
   None

See Also:
   None

=======================================================================
AEESMSGeoScope

Description:
   Specifies the Geographic Scope for a GW Broadcast message

Definition:

enum
{
   AEESMS_GEO_SCOPE_CELL_IMMEDIATE  = 0,
   AEESMS_GEO_SCOPE_PLMN,            // 1
   AEESMS_GEO_SCOPE_LOCATION_AREA,   // 2
   AEESMS_GEO_SCOPE_CELL_NORMAL,     // 3
};
typedef uint32 AEESMSGeoScope;

Members:
   AEESMS_GEO_SCOPE_CELL_IMMEDIATE:
   AEESMS_GEO_SCOPE_PLMN:
   AEESMS_GEO_SCOPE_LOCATION_AREA:
   AEESMS_GEO_SCOPE_CELL_NORMAL: 

Comments:
   None

See Also:
   None

=======================================================================
AEESMSCellChangeType

Description:
   Specifies the cell change type with EVT_MDL_SMS_CELL_CHANGE

Definition:

   #define AEESMS_GW_CB_UNKNOWN             0
   #define AEESMS_GW_CB_NO_SERVICE          1
   #define AEESMS_GW_CB_SAME_CELL           2
   #define AEESMS_GW_CB_CELL_CHANGE         3
   #define AEESMS_GW_CB_LAC_CHANGE          4
   #define AEESMS_GW_CB_PLMN_CHANGE         5

   // Define a data type to refer to Geo Scope types
   typedef uint32 AEESMSCellChangeType;

Members:
   AEESMS_GW_CB_UNKNOWN:
   AEESMS_GW_CB_NO_SERVICE:
   AEESMS_GW_CB_SAME_CELL:
   AEESMS_GW_CB_CELL_CHANGE:
   AEESMS_GW_CB_LAC_CHANGE:
   AEESMS_GW_CB_PLMN_CHANGE: 

Comments:
   None

See Also:
   None

=======================================================================
AEESMSDisplayMode

Description:
   Message Display Mode is used to indicate to the mobile station when to
   display the received message. Following are possible display modes:

   - Immediate Display: Display the received message as soon as possible.

   - Mobile default setting: Display the received message based on a 
   pre-defined mode on the device

   - User Invoke: Display the received message based on the mode selected 
   by the end user.

Definition:

   #define AEESMS_DISPLAY_MODE_IMMEDIATE    0
   #define AEESMS_DISPLAY_MODE_DEFAULT      1
   #define AEESMS_DISPLAY_MODE_USER_INVOKE  2
   #define AEESMS_DISPLAY_MODE_RESERVED     3
   #define AEESMS_DISPLAY_MODE_NONE         0xFFFFFFFF

   //Define a data type for display modes
   typedef uint32 AEESMSDisplayMode;

Members:
   AEESMS_DISPLAY_MODE_IMMEDIATE
   AEESMS_DISPLAY_MODE_DEFAULT
   AEESMS_DISPLAY_MODE_USER_INVOKE
   AEESMS_DISPLAY_MODE_RESERVED
   AEESMS_DISPLAY_MODE_NONE

Comments:
   None

See Also:
   None

=======================================================================
SMSMsgOpt

Description:
   Holds an id, value pair for ISMSMsg.

Definition:
   typedef WebOpt SMSMsgOpt;

Members:
   int32  nId: option id, chosen from one of the message options

   void *pVal: option value, depends on nId

Comments:
   SMSMsgOpt structures indicate various parameters of an SMS message. The structures,
   when passed to AddOpt, are not themselves copied, the contents are copied out

See Also:
   None
=======================================================================
AEESMSGWTime

Description:
This structure contains Julian date and time zone information for GSM/UMTS
messages.

Definition:
typedef struct
{
   uint16 wYear;
   uint16 wMonth;
   uint16 wDay;
   uint16 wHour;
   uint16 wMinute;
   uint16 wSecond;
   uint16 wWeekDay;
   sint15 nTimeZone;
} AEESMSGWTime;

Members:

wYear : 4-digit year
wMonth  :  Month 1-12(January=1, December=12)
wDay : Day 1-31
wHour : Hour 0-23
wMinute : Minute 0-59
wSecond : Seconds 0-59
wWeekDay : Day of the week 0-6 (0=Monday, 6=Sunday)
nTimeZone: Time Zone information is specified as [-48 .. +48] number of 15 minutes

Comments:
None

See Also:
None

=======================================================================
AEESMSGWCommand

Description:
   Specifies a GSM/WCDMA command

Definition:

   #define AEESMS_GW_COMMAND_ENQUIRY                  0
   #define AEESMS_GW_COMMAND_CANCEL_STATUS_REPORT     1
   #define AEESMS_GW_COMMAND_DELETE_SM                2
   #define AEESMS_GW_COMMAND_ENABLE_STATUS_REPORT     3

   // Define a data type to refer to GSM/WCDMA commands
   typedef uint32 AEESMSGWCommand;

Members:
   AEESMS_GW_COMMAND_ENQUIRY: Enquiry relating to previously submitted short message

   AEESMS_GW_COMMAND_CANCEL_STATUS_REPORT:Cancel Status Report Request relating to 
                                          previously submitted short message

   AEESMS_GW_COMMAND_DELETE_SM:Delete previously submitted Short Message

   AEESMS_GW_COMMAND_ENABLE_STATUS_REPORT:Enable Status Report Request relating 
                                          to previously submitted short message

Comments:
   None

See Also:
   None

=======================================================================

MSGOPT_

The prefix MSGOPT_ is used for SMS message options.
=======================================================================

message options

Description:
   Message option specifies message parameter to be set.

Definition:
   #define MSGOPT_TO_DEVICE_SZ     (WEBOPT_SZ + 0)
   #define MSGOPT_PAYLOAD_SZ       (WEBOPT_SZ + 1)
   #define MSGOPT_BREW_HANDLER_SZ  (WEBOPT_SZ + 2)
   #define MSGOPT_SC_ADDRESS_SZ    (WEBOPT_SZ + 3)
   #define MSGOPT_FROM_DEVICE_SZ   (WEBOPT_SZ + 4)
   #define MSGOPT_CALLBACK_SZ      (WEBOPT_SZ + 5)
   #define MSGOPT_TO_EMAIL_SZ      (WEBOPT_SZ + 6)

   #define MSGOPT_PAYLOAD_WSZ      (WEBOPT_WSZ + 0)

   #define MSGOPT_TIMESTAMP        (WEBOPT_VARIABLE + sizeof(JulianType))
   #define MSGOPT_ABSOLUTE_DEFERREDTIME   (WEBOPT_VARIABLE + WEBOPT_VARIABLE_INC + sizeof(JulianType))

   #define MSGOPT_GW_TIMESTAMP     (WEBOPT_VARIABLE + sizeof(AEESMSGWTime))
   #define MSGOPT_DISCHARGETIME    (WEBOPT_VARIABLE + WEBOPT_VARIABLE_INC + sizeof(AEESMSGWTime))
   #define MSGOPT_ABSOLUTE_VALIDITY    (WEBOPT_VARIABLE + 2*WEBOPT_VARIABLE_INC + sizeof(AEESMSGWTime))

   #define MSGOPT_BREW_CLASS       (WEBOPT_32BIT + 0)
   #define MSGOPT_PRIORITY         (WEBOPT_32BIT + 1)
   #define MSGOPT_PRIVACY          (WEBOPT_32BIT + 2)
   #define MSGOPT_PAYLOAD_ENCODING (WEBOPT_32BIT + 3)
   #define MSGOPT_LANG             (WEBOPT_32BIT + 4)
   #define MSGOPT_CLASS            (WEBOPT_32BIT + 5)
   #define MSGOPT_TAG              (WEBOPT_32BIT + 6)
   #define MSGOPT_MSGTYPE          (WEBOPT_32BIT + 7)
   #define MSGOPT_BCTYPE           (WEBOPT_32BIT + 8)
   #define MSGOPT_REQUEST_DL_ACK   (WEBOPT_32BIT + 9)
   #define MSGOPT_NUM              (WEBOPT_32BIT + 10)
   #define MSGOPT_REQUEST_USER_ACK (WEBOPT_32BIT + 11)
   #define MSGOPT_MOSMS_ENCODING   (WEBOPT_32BIT + 12)
   #define MSGOPT_MSG_ID           (WEBOPT_32BIT + 13)
   #define MSGOPT_REQUEST_READ_ACK (WEBOPT_32BIT + 14)
   #define MSGOPT_DL_ACK           (WEBOPT_32BIT + 15)
   #define MSGOPT_USER_ACK         (WEBOPT_32BIT + 16)
   #define MSGOPT_READ_ACK         (WEBOPT_32BIT + 17)
   #define MSGOPT_BC_SERVICE_ID    (WEBOPT_32BIT + 18)
   #define MSGOPT_BC_PAGE_NUMBER   (WEBOPT_32BIT + 19)
   #define MSGOPT_BC_TOTAL_PAGES   (WEBOPT_32BIT + 20)
   #define MSGOPT_BC_GEO_SCOPE     (WEBOPT_32BIT + 21)
   #define MSGOPT_BC_MESSAGE_CODE  (WEBOPT_32BIT + 22)
   #define MSGOPT_BC_UPDATE_NUMBER (WEBOPT_32BIT + 23)
   #define MSGOPT_GW_PID           (WEBOPT_32BIT + 24)
   #define MSGOPT_REPLY_PATH_PRESENT   (WEBOPT_32BIT + 25)
   #define MSGOPT_REPORT_STATUS    (WEBOPT_32BIT + 26)
   #define MSGOPT_TO_NUMBER_TYPE          (WEBOPT_32BIT + 27)
   #define MSGOPT_TO_NUMBER_PLAN          (WEBOPT_32BIT + 28)
   #define MSGOPT_FROM_NUMBER_TYPE        (WEBOPT_32BIT + 29)
   #define MSGOPT_FROM_NUMBER_PLAN        (WEBOPT_32BIT + 30)
   #define MSGOPT_CALLBACK_NUMBER_TYPE    (WEBOPT_32BIT + 31)
   #define MSGOPT_CALLBACK_NUMBER_PLAN    (WEBOPT_32BIT + 32)
   #define MSGOPT_SC_ADDRESS_NUMBER_TYPE  (WEBOPT_32BIT + 33)
   #define MSGOPT_SC_ADDRESS_NUMBER_PLAN  (WEBOPT_32BIT + 34)
   #define MSGOPT_AUTO_STORAGE_TYPE       (WEBOPT_32BIT + 35)
   #define MSGOPT_AUTO_STORAGE_INDEX      (WEBOPT_32BIT + 36)
   #define MSGOPT_MSG_WAITING_TYPE        (WEBOPT_32BIT + 37)
   #define MSGOPT_MSG_WAITING_KIND        (WEBOPT_32BIT + 38)
   #define MSGOPT_COMPRESSED_MSG          (WEBOPT_32BIT + 39)
   #define MSGOPT_MORE_MSG_TO_SEND        (WEBOPT_32BIT + 40)
   #define MSGOPT_RAW_DCS_DATA            (WEBOPT_32BIT + 41)
   #define MSGOPT_RELATIVE_DEFERREDTIME   (WEBOPT_32BIT + 42)
   #define MSGOPT_RELATIVE_VALIDITYTIME   (WEBOPT_32BIT + 43)
   #define MSGOPT_CANCELLATION_REQUEST    (WEBOPT_32BIT + 44)
   #define MSGOPT_GW_COMMAND              (WEBOPT_32BIT + 45)
   #define MSGOPT_GW_COMMAND_UDHI         (WEBOPT_32BIT + 46)
   #define MSGOPT_GW_COMMAND_MN           (WEBOPT_32BIT + 47)
   #define MSGOPT_DISPLAY_MODE            (WEBOPT_32BIT + 48)
   #define MSGOPT_LOOPBACK_MSG            (WEBOPT_32BIT + 49)
   #define MSGOPT_LOOPBACK_FROM_APPLET    (WEBOPT_32BIT + 50)

   #define MSGOPT_PAYLOAD_BINARY   (WEBOPT_VARBUFFER + 0)
   #define MSGOPT_UDH_CONCAT       (WEBOPT_VARBUFFER + 1)
   #define MSGOPT_UDH_SPECIAL_SM   (WEBOPT_VARBUFFER + 2)
   #define MSGOPT_UDH_PORT         (WEBOPT_VARBUFFER + 3)
   #define MSGOPT_UDH_TEXT_FORMAT  (WEBOPT_VARBUFFER + 4)
   #define MSGOPT_UDH_PREDEFINED   (WEBOPT_VARBUFFER + 5)
   #define MSGOPT_UDH_USERDEFINED  (WEBOPT_VARBUFFER + 6)
   #define MSGOPT_UDH_VAR_PICTURE  (WEBOPT_VARBUFFER + 7)
   #define MSGOPT_UDH_RFC822       (WEBOPT_VARBUFFER + 8)
   #define MSGOPT_UDH_OTHER        (WEBOPT_VARBUFFER + 9)
   #define MSGOPT_TPDU_DELIVER     (WEBOPT_VARBUFFER + 10) 
   #define MSGOPT_TPDU_STATUS_REPORT   (WEBOPT_VARBUFFER + 11)
   #define MSGOPT_ALPHA_ID             (WEBOPT_VARBUFFER + 12)
   #define MSGOPT_COMMAND_DATA         (WEBOPT_VARBUFFER + 13)

   #define MSGOPT_PAYLOAD_SOURCE   (WEBOPT_INTERFACE + 0)

   #define MSGOPT_END              WEBOPT_END
   #define MSGOPT_ANY              WEBOPT_ANY
   #define MSGOPT_COPYOPTS         WEBOPT_COPYOPTS

Members:
   MSGOPT_TO_DEVICE_SZ     : NULL terminated string providing destination device number. '+'
                             as first character signifies international number.

   MSGOPT_PAYLOAD_SZ       : Null terminated string payload. Good for single-byte formats with
                             encoding where NULL isn't valid character.

   MSGOPT_BREW_HANDLER_SZ  : Null terminated string identifying MIME type of BREW handler for
                             the message.

   MSGOPT_SC_ADDRESS_SZ    : Null terminated string identifying service center address.
                             Specific only for GSM messages.

   MSGOPT_FROM_DEVICE_SZ   : NULL terminated string providing address of the device sending
                             message. '+' as first character signifies international number.

   MSGOPT_CALLBACK_SZ      : NULL terminated string providing callabck number for page

   MSGOPT_TO_EMAIL_SZ      : NULL terminated string providing destination email address.

   MSGOPT_PAYLOAD_WSZ      : Null terminated wide string payload. Good for double-byte formats with
                             encoding where 0x0000 isn't valid character.

   MSGOPT_TIMESTAMP        : JulianType message timestamp for received messages. Doesn't contain time
                             zone information and thus not suitable for GSM/UMTS messages.
   

   MSGOPT_ABSOLUTE_DEFERREDTIME: JulianType absolute deferred delivery time. Valid only for CDMA
                                 messages.

   MSGOPT_GW_TIMESTAMP     : AEESMSGWTime message timestamp for received messages. Contains time zone
                             information.

   MSGOPT_DISCHARGETIME    : AEESMSGWTime message discharge time provided with status report.

   MSGOPT_ABSOLUTE_VALIDITY: AEESMSGWTime message absolute validity.

   MSGOPT_BREW_CLASS       : AEECLSID Class ID of the destination applet.

   MSGOPT_PRIORITY         : AEESMSPriority message priority

   MSGOPT_PRIVACY          : AEESMSPrivacy message privacy

   MSGOPT_PAYLOAD_ENCODING : BREW encoding of payload buffer. Specified as AEE_ENC_XXX. If this
                             is not specified, payload buffer is assumed to be in default device
                             encoding.

   MSGOPT_LANG             : Message language. Specified as LNG_XXX.If this is not specified,
                             payload buffer is assumed to be in default device language.

   MSGOPT_CLASS            : AEESMSClass message class. Can be used by SMS app to decide
                             storage option for the message.

   MSGOPT_TAG              : AEESMSTag message status tag

   MSGOPT_MSGTYPE          : AEESMSType message type

   MSGOPT_BCTYPE           : AEESMSBCType message broadcast type for CDMA broadcast messages

   MSGOPT_REQUEST_DL_ACK   : Set TRUE if delivery acknowledgement or status report is requested. 
                             FALSE otherwise.

   MSGOPT_NUM              : Number of messages for voice mail indication

   MSGOPT_REQUEST_USER_ACK : Set TRUE if user acknowledgement is requested. FALSE otherwise.

   MSGOPT_MOSMS_ENCODING   : AEESMSEncType mobile originated SMS encoding type. Specifies
                             encoding to use for sending message over the air. If not specified,
                             ISMS will use default mobile originated SMS encoding specified by
                             the device.

   MSGOPT_MSG_ID           : Message reference number. Applications can use this with MSGOPT_USER_ACK 
                             and MSGOPT_READ_ACK to send user and read acknowledgement respectively on
                             CDMA network. Applications can use this with MSGOPT_CANCELLATION_REQUEST 
                             to send message cancellation request for a previously submitted message
                             on CDMA network.

   MSGOPT_REQUEST_READ_ACK : Set TRUE if read acknowledgement is requested. FALSE otherwise.

   MSGOPT_DL_ACK           : Set TRUE if message is delivery acknowledgement or status report. FALSE otherwise. 
                             This is applicable only for MT SMS and is ignored, if specified, for MO SMS.

   MSGOPT_USER_ACK         : Set TRUE if message is user acknowledgement. FALSE otherwise. Valid only for
                             CDMA messages.

   MSGOPT_READ_ACK         : Set TRUE if message is read acknowledgement. FALSE otherwise. Valid only for
                             CDMA messages.
   
   MSGOPT_BC_SERVICE_ID    : Service ID of a GSM / WCDMA Broadcast Message

   MSGOPT_BC_PAGE_NUMBER   : Page Number of a GSM / WCDMA Broadcast Message

   MSGOPT_BC_TOTAL_PAGES   : Total Pages of a GSM / WCDMA Broadcast Message

   MSGOPT_BC_GEO_SCOPE     : AEESMSGeoScope Type indicating Geographic Scope of a GSM / WCDMA Broadcast Message

   MSGOPT_BC_MESSAGE_CODE  : Message Code of a GSM / WCDMA Broadcast Message

   MSGOPT_BC_UPDATE_NUMBER : Update Number of a GSM / WCDMA Broadcast Message

   MSGOPT_GW_PID           : AEESMSPID Type - Protocol Identifier of a GSM / WCDMA Message

   MSGOPT_REPLY_PATH_PRESENT : Set TRUE if message reply path is present for a GSM / WCDMA message. 
                               FALSE otherwise. Absence of this option means message reply path is
                               not present.

   MSGOPT_REPORT_STATUS    : AEESMSReportStatus Type - Specifies status of the status report.
   
   MSGOPT_TO_NUMBER_TYPE   : AEESMSNumberType - Specifies number type for MSGOPT_TO_DEVICE_SZ

   MSGOPT_TO_NUMBER_PLAN   : AEESMSNumberPlan - Specifies number plan for MSGOPT_TO_DEVICE_SZ

   MSGOPT_FROM_NUMBER_TYPE : AEESMSNumberType - Specifies number type for MSGOPT_FROM_DEVICE_SZ

   MSGOPT_FROM_NUMBER_PLAN : AEESMSNumberPlan - Specifies number plan for MSGOPT_FROM_DEVICE_SZ

   MSGOPT_CALLBACK_NUMBER_TYPE   : AEESMSNumberType - Specifies number type for MSGOPT_CALLBACK_SZ

   MSGOPT_CALLBACK_NUMBER_PLAN   : AEESMSNumberPlan - Specifies number plan for MSGOPT_CALLBACK_SZ

   MSGOPT_SC_ADDRESS_NUMBER_TYPE : AEESMSNumberType - Specifies number type for MSGOPT_SC_ADDRESS_SZ

   MSGOPT_SC_ADDRESS_NUMBER_PLAN : AEESMSNumberPlan - Specifies number plan for MSGOPT_SC_ADDRESS_SZ
   
   MSGOPT_AUTO_STORAGE_TYPE      : Specifies AEESMSStorageType for automatically stored mobile 
                                   terminated messages

   MSGOPT_AUTO_STORAGE_INDEX     : Specifies index of automatically stored mobile terminated messages

   MSGOPT_MSG_WAITING_TYPE       : AEESMSMsgWaitingType - Specifies type of waiting message

   MSGOPT_MSG_WAITING_KIND       : AEESMSMsgWaitingKind - Specifies kind of waiting message

   MSGOPT_COMPRESSED_MSG         : boolean - Specifies TP - DCS message compressed parameter
   
   MSGOPT_MORE_MSG_TO_SEND       : boolean - Specifies TP - MMS

   MSGOPT_RAW_DCS_DATA           : Raw DCS DATA. Applicable only for GSM/UMTS messages.
   
   MSGOPT_RELATIVE_DEFERREDTIME  : Relative deferred delivery time in seconds. Valid only for
                                   CDMA messages.

   MSGOPT_RELATIVE_VALIDITYTIME  : Relative validity time in seconds.

   MSGOPT_CANCELLATION_REQUEST   : Set TRUE if message is a cancellation request for a previously
                                   submitted message. FALSE otherwise. Valid only for MO CDMA messages.

   MSGOPT_GW_COMMAND             : AEESMSGWCommand command. Applicable only for GSM/UMTS messages.

   MSGOPT_GW_COMMAND_UDHI        : Specifies if command data has user data header present indication set TRUE.
   
   MSGOPT_GW_COMMAND_MN          : Applications can use this with MSGOPT_GW_COMMAND to specify the reference number of the message
                                   to which the specified command is to be applied.

   MSGOPT_LOOPBACK_MSG           : boolean - Is the message a loopback message? i.e. a message that 
                                   is not sent out to the network but delivered back to the sender again.
   
   MSGOPT_LOOPBACK_FROM_APPLET   : AEECLSID Class ID of the origination applet for a loopback message
                                   (applicable to MT messages only).
   
   MSGOPT_PAYLOAD_BINARY   : Binary payload.
   
                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Applications can use convenient MACROs 
                             WEBOPT_GETVARBUFFERLENGTH() and WEBOPT_GETVARBUFFERDATA() to get data 
                             length and data respectively. Likewise, applications can use 
                             WEBOPT_SETVARBUFFER() to set binary payload.

   MSGOPT_UDH_CONCAT       : Concatenation User Data Header. Used with AEESMS_UDH_CONCAT_8 and 
                             AEESMS_UDH_CONCAT_16 IDs.
                             
                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format: 
~							 
                                    1 byte  : (AEESMSUDHId)
                                    1 byte  : (uint8) Sequence Number
                                    1 byte  : (uint8) Total Messages 
                                    2 bytes : (uint16) Message Reference Number                                  
*
   
                          Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             Likewise, applications can use MSGOPT_SETUDHCONCAT8BUFFER() and
                             MSGOPT_SETUDHCONCAT16BUFFER() to set user data header buffer.

   MSGOPT_UDH_SPECIAL_SM   : Special SM User Data Header. Used with AEESMS_UDH_SPECIAL_SM ID.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format:
~
                                    1 byte  : (AEESMSUDHId)
                                    1 byte  : (AEESMSMsgWaitingType)
                                    1 byte  : (AEESMSMsgWaitingKind)
                                    1 byte  : (uint8) Message Count
*

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             Likewise, applications can use MSGOPT_SETUDHSPECIALSMBUFFER() to set user 
                             data header buffer.

   MSGOPT_UDH_PORT         : Port User Data Header. Used with AEESMS_UDH_PORT_8 and AEESMS_UDH_PORT_16 IDs.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first.  Next bytes have data in following
                             format: 
~							 
                                    1 byte  : (AEESMSUDHId)
                                    2 bytes : (uint16) Destination Port
                                    2 bytes : (uint16) Origination Port
*

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             Likewise, applications can use MSGOPT_SETUDHPORT8BUFFER() and 
                             MSGOPT_SETUDHPORT16BUFFER() to set user data header buffer.

   MSGOPT_UDH_TEXT_FORMAT  : Text Formatting User Data Header. Used with AEESMS_UDH_TEXT_FORMATING ID.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format: 
~							 
                                    1 byte  : (AEESMSUDHId)
                                    1 byte  : (uint8) Starting Position
                                    1 byte  : (uint8) Text Formatting Length 
                                    1 byte  : Text Formatting Style
~
                                              Bit 1 and Bit 0:
~											  
															   00  Left Alignment
                                                               01  Center Alignment
                                                               10  Right Alignment
                                                               11  Default Alignment
*
                                              Bit 3 and Bit 2: 
~											                   
															   00  Normal Font Size
                                                               01  Large Font Size
                                                               10  Small Font Size
*                                                              11  Reserved Font Size

                                              Bit 4:  1  Bold
                                              Bit 5:  1  Italic

                                              Bit 6:  1  Underlined
                                              Bit 7:  1  Strikethrough

                                    1 byte  : Text Color
                                              Bits 3 - 0: Foreground Text Color specified as AEESMSUDHTextColorId
                                              Bits 7 - 4: Background Text Color specified as AEESMSUDHTextColorId

*
*

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             
                             For MSGOPT_UDH_TEXT_FORMAT, presence of the text color information is optional.

                             For mobile originated messages, applications can use MSGOPT_SETUDHTEXTFORMATBUFFER() 
                             to set user data header buffer for text without color information and 
                             MSGOPT_SETUDHTEXTWITHCOLORFORMATBUFFER to set user data header buffer for 
                             text with color information.

                             For mobile terminated messages, applications can use MSGOPT_GETUDHBUFFERLENGTH()
                             to determine if text color information is present.

   MSGOPT_UDH_PREDEFINED   : Predefined Animation / Sound User Data Header. Used with AEESMS_UDH_PRE_DEF_SOUND 
                             and AEESMS_UDH_PRE_DEF_ANIM IDs.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format:
~
                                    1 byte  : (AEESMSUDHId)
                                    1 byte  : (uint8) Position
                                    1 byte  : (uint8) Animation / Sound Number
*

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             Likewise, applications can use MSGOPT_SETUDHPREDEFSOUNDBUFFER() and 
                             MSGOPT_SETUDHPREDEFANIMBUFFER() to set user data header buffer.

   MSGOPT_UDH_USERDEFINED  : Userdefined Picture / Animation / Sound User Data Header. Used with 
                             AEESMS_UDH_USER_DEF_SOUND, AEESMS_UDH_LARGE_ANIM, AEESMS_UDH_SMALL_ANIM, 
                             AEESMS_UDH_LARGE_PIC and AEESMS_UDH_SMALL_PIC IDs.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format:
~							 
                                    1 byte  : (AEESMSUDHId)
                                    1 byte  : (uint8) Positions
                                    1 byte  : (uint8) Data Length
*
                        <Data length> bytes : Data 
~
                                              Data Length for AEESMS_UDH_LARGE_ANIM =  128
                                              Data Length for AEESMS_UDH_SMALL_ANIM =   32
                                              Data Length for AEESMS_UDH_LARGE_PIC =   128
                                              Data Length for AEESMS_UDH_SMALL_PIC =    32
*

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. ~
                             Likewise, applications can use ~
							 MSGOPT_SETUDHUSERDEFSOUNDBUFFER(),~
                             MSGOPT_SETUDHLARGEANIMBUFFER(), ~
							 MSGOPT_SETUDHSMALLANIMBUFFER(), ~
                             MSGOPT_SETUDHLARGEPICBUFFER() ~
							 and MSGOPT_SETUDHSMALLPICBUFFER()~ 
                             to set user data header buffer.

   MSGOPT_UDH_VAR_PICTURE  : Variable Picture User Data Header. Used with AEESMS_UDH_VAR_PICTURE ID.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format: 
~							 
                                    1 byte  : (AEESMSUDHId)
                                    1 byte  : (uint8) Positions
                                    1 byte  : (uint8) Width of Picture
                                    1 byte  : (uint8) Height of Picture
*
===pre>
           <((Width*Height) + 7) / 8> bytes : Data
===/pre>

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             Likewise, applications can use MSGOPT_SETUDHVARPICBUFFER() to set user 
                             data header buffer.

   MSGOPT_UDH_RFC822       : RFC 822 (Email) User Data Header. Used with AEESMS_UDH_RFC822 ID.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format:
~							 
                                    1 byte  : (AEESMSUDHId)
                                    1 byte  : (uint8) Header Length
*

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             Likewise, applications can use MSGOPT_SETRFC822BUFFER() to set user 
                             data header buffer.

   MSGOPT_UDH_OTHER        : Other User Data Header. Used with the remaining AEESMSUDHId.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Next bytes have data in following
                             format: 
~							 
                                    1 byte  : (AEESMSUDHId)
                                    2 bytes : (uint16) Header Length
===pre>
                      <Header Length> bytes : Data
===/pre>
*

                             Applications can use convenient MACROs MSGOPT_GETUDHBUFFERLENGTH() and 
                             MSGOPT_GETUDHBUFFERDATA() to get data length and data respectively. 
                             Likewise, applications can use MSGOPT_SETUDHOTHERBUFFER() to set user 
                             data header buffer.

   MSGOPT_TPDU_DELIVER     : UMTS Deliver TPDU. This option is specified only for mobile terminated
                             messages when ISMS_ReceiveMsg() is called by an application with PL_SYSTEM
                             privilege.
   
                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Applications can use convenient MACROs 
                             WEBOPT_GETVARBUFFERLENGTH() and WEBOPT_GETVARBUFFERDATA() to get data 
                             length and data respectively.

   MSGOPT_TPDU_STATUS_REPORT: UMTS status report TPDU. This option is specified only for mobile terminated
                             messages when ISMS_ReceiveMsg() is called by an application with PL_SYSTEM
                             privilege.
   
                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Applications can use convenient MACROs 
                             WEBOPT_GETVARBUFFERLENGTH() and WEBOPT_GETVARBUFFERDATA() to get data 
                             length and data respectively.

   MSGOPT_ALPHA_ID         : Alpha ID. Applicable only to GW template and ISMSMsg specified with
                             EVT_MDL_SMS_MSG_SENT event for message sent on GW network.

                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Applications can use convenient MACROs 
                             WEBOPT_GETVARBUFFERLENGTH() and WEBOPT_GETVARBUFFERDATA() to get data 
                             length and data respectively.

   MSGOPT_COMMAND_DATA     : AEESMSGWCommand Data.
   
                             The first two bytes give the length of the following data. The length 
                             is little-endian, low byte first. Applications can use convenient MACROs 
                             WEBOPT_GETVARBUFFERLENGTH() and WEBOPT_GETVARBUFFERDATA() to get data 
                             length and data respectively. Likewise, applications can use 
                             WEBOPT_SETVARBUFFER() to set binary payload.

                             Applications must specify MSGOPT_COMMAND_DATA only when MSGOPT_GW_COMMAND 
                             is specified.

   MSGOPT_PAYLOAD_SOURCE   : IAStream message stream

   MSGOPT_END              : No pVal, ignore rest, used for terminating a vector of SMSMsgOpts

   MSGOPT_ANY              : No pVal, matches any opt, used for enumeration

   MSGOPT_COPYOPTS         : pVal boolean, if set to TRUE, causes the ISMSMsg to copy current
                             and future SMSMsgOpt

Comments:
   1. Applications must specify only one of MSGOPT_BREW_CLASS and 
      MSGOPT_BREW_HANDLER_SZ options. Specifying both will result in one of
      the option being ignored. Implementation doesn't publish the order in which
      these options are checked.

   2. Applications must specify only one of MSGOPT_PAYLOAD_SZ, MSGOPT_PAYLOAD_WSZ 
      and MSGOPT_PAYLOAD_BINARY payload options. Specifying multiple options will 
      result in extra payload options being ignored. Implementation doesn't publish 
      the order in which these options are checked.

See Also:
   None

=======================================================================
AEESMSStorageType

Description:
   Specifies the message storage type.

Definition:

typedef uint32 AEESMSStorageType;

Members:

   AEESMS_CARD_RUIM : RUIM Card
   AEESMS_CARD_SIM : SIM Card
   AEESMS_NV_CDMA : CDMA NV
   AEESMS_NV_GW : GW NV
   AEESMS_CARD_RUIM_TEMPLATE: RUIM Card Template
   AEESMS_CARD_SIM_TEMPLATE: SIM Card Template
   AEESMS_NV_CDMA_TEMPLATE: CDMA NV Template
   AEESMS_NV_GW_TEMPLATE: GW NV Template
   AEESMS_CARD_SIM_STATUS_REPORT: SIM Card Status Report
   AEESMS_STORAGE_NONE: Used with ISMSSTORAGE2_AutoStoreMsg() to disable auto storage for
                        messages of the specified message class

Comments:
   None

See Also:
   None

=======================================================================
AEESMSStorageStatus

Description:
   Specifies the SMS storage status

Definition:

typedef struct
{
   uint32                     nSize;
   uint32                     nMaxSlots;
   uint32                     nFreeSlots;
   uint32                     nUsedTagSlots;
} AEESMSStorageStatus;


Members:
   nSize: struct size
   nMaxSlots: Maximum number of slots
   nFreeSlots: Number of free slots
   nUsedTagSlots: Used slot for specified tag type

Comments:
   None

See Also:
   None

=======================================================================
AEESMSBCAlertType

Description:
   Specifies the alert type for a broadcast message.

Definition:

enum
{
   AEESMS_BC_ALERT_NONE  = 0,
   AEESMS_BC_ALERT_DEFAULT,
   AEESMS_BC_ALERT_VIBRATE_ONCE,
   AEESMS_BC_ALERT_VIBRATE_REPEAT,
   AEESMS_BC_ALERT_VISUAL_ONCE,
   AEESMS_BC_ALERT_VISUAL_REPEAT,
   AEESMS_BC_ALERT_LOW_PRIORITY_ONCE,
   AEESMS_BC_ALERT_LOW_PRIORITY_REPEAT,
   AEESMS_BC_ALERT_MEDIUM_PRIORITY_ONCE,
   AEESMS_BC_ALERT_MEDIUM_PRIORITY_REPEAT,
   AEESMS_BC_ALERT_HIGH_PRIORITY_ONCE,
   AEESMS_BC_ALERT_HIGH_PRIORITY_REPEAT
};
typedef unit32 AEESMSBCAlertType;

Members:

   AEESMS_BC_ALERT_NONE	~						
   AEESMS_BC_ALERT_DEFAULT ~
   AEESMS_BC_ALERT_VIBRATE_ONCE ~
   AEESMS_BC_ALERT_VIBRATE_REPEAT ~
   AEESMS_BC_ALERT_VISUAL_ONCE ~
   AEESMS_BC_ALERT_VISUAL_REPEAT ~
   AEESMS_BC_ALERT_LOW_PRIORITY_ONCE ~
   AEESMS_BC_ALERT_LOW_PRIORITY_REPEAT ~
   AEESMS_BC_ALERT_MEDIUM_PRIORITY_ONCE ~
   AEESMS_BC_ALERT_MEDIUM_PRIORITY_REPEAT ~
   AEESMS_BC_ALERT_HIGH_PRIORITY_ONCE ~
   AEESMS_BC_ALERT_HIGH_PRIORITY_REPEAT ~

Comments:
   None

See Also:
   None

=======================================================================

SMSBCSrvOpts

Description:
   Holds an id, value pair for ISMSBCSrvOpts.

Definition:
   typedef WebOpt SMSBCSrvOpts;

Members:
   int32  nId: option id, chosen from one of the SMS broadcast service options

   void *pVal: option value, depends on nId

Comments:
   SMSBCSrvOpts structures indicate various parameters of SMS broadcast service.
   The structures, when passed to AddOpt, are not themselves copied, the contents
   are copied out.

See Also:
   None

=======================================================================
SMSBCSRVOPT_


The prefix SMSBCSRVOPT_ is used for SMS broadcast service options.
=======================================================================

SMS broadcast service options

Description:
   A SMS broadcast service option specifies message parameter to be set.

Definition:

   #define SMSBCSRVOPT_LABEL_SZ         (WEBOPT_SZ + 0)

   #define SMSBCSRVOPT_LABEL_WSZ        (WEBOPT_WSZ + 0)

   #define SMSBCSRVOPT_SELECTED         (WEBOPT_32BIT + 0)
   #define SMSBCSRVOPT_PRIORITY         (WEBOPT_32BIT + 1)
   #define SMSBCSRVOPT_LABEL_ENCODING   (WEBOPT_32BIT + 2)
   #define SMSBCSRVOPT_ALERT            (WEBOPT_32BIT + 3)
   #define SMSBCSRVOPT_MAX_MSGS         (WEBOPT_32BIT + 4)

   #define SMSBCSRVOPT_SERVICEID        (WEBOPT_VARIABLE + sizeof(AEESMSBCSrvID))

   #define SMSBCSRVOPT_END              WEBOPT_END
   #define SMSBCSRVOPT_ANY              WEBOPT_ANY
   #define SMSBCSRVOPT_COPYOPTS         WEBOPT_COPYOPTS

Members:
   SMSBCSRVOPT_LABEL_SZ       : Null terminated string identifying service label in the table.
                                Specific only to CDMA.

   SMSBCSRVOPT_LABEL_WSZ      : Null terminated wide string identifying service label in the table.
                                Specific only to CDMA.

   SMSBCSRVOPT_SELECTED       : Set TRUE if service is selected. FALSE otherwise.

   SMSBCSRVOPT_PRIORITY       : Priority threshold for the service

   SMSBCSRVOPT_LABEL_ENCODING : BREW encoding of service label. Specified as AEE_ENC_XXX.If this
                                is not specified, label is assumed to be in default device encoding.

   SMSBCSRVOPT_ALERT          : AEESMSBCAlertType alert type for the service

   SMSBCSRVOPT_MAX_MSGS       : Limit on maximum messages which can be stored for the service

   SMSBCSRVOPT_SERVICEID      : AEESMSBCSrvID Service ID

   SMSBCSRVOPT_END            : No pVal, ignore rest, used for terminating a vector of SMSBCSrvOpts

   SMSBCSRVOPT_ANY            : No pVal, matches any opt, used for enumeration

   SMSBCSRVOPT_COPYOPTS       : pVal boolean, if set to TRUE, causes the ISMSBCSrvOpts to copy current
                                and future SMSBCSrvOpts

Comments:
   None

See Also:
   None

=======================================================================
AEESMSBCConfig

Description:
   Specifies carriers broadcast configuration.

Definition:

enum
{
   AEESMS_BC_CONFIG_DISALLOW,
   AEESMS_BC_CONFIG_ALLOW_TABLE,
   AEESMS_BC_CONFIG_ALLOW_ALL

};
typedef uint32 AEESMSBCConfig;

Members:

   AEESMS_BC_CONFIG_DISALLOW     : No broadcast to devices allowed
   AEESMS_BC_CONFIG_ALLOW_TABLE  : Allow broadcast of services specified
                                   in device service table only
   AEESMS_BC_CONFIG_ALLOW_ALL    : Allow broadcast of all services

Comments:
   None

See Also:
   None

=======================================================================
AEESMSBCPref

Description:
   Specifies clients broadcast preference.

Definition:

enum
{
   AEESMS_BC_PREF_DISALLOW,
   AEESMS_BC_PREF_ALLOW_TABLE,
   AEESMS_BC_PREF_ALLOW_ALL

};
typedef uint32 AEESMSBCPref;


Members:

   AEESMS_BC_PREF_DISALLOW     : Client doesn't want any broadcast message
   AEESMS_BC_PREF_ALLOW_TABLE  : Client wants broadcast messages of services specified in
                                 device service table only
   AEESMS_BC_PREF_ALLOW_ALL    : Client wants all broadcast messages

Comments:
   None

See Also:
   None

=======================================================================
AEESMSMode

Description:
   Specifies clients broadcast message mode.

Definition:

enum
{
   AEESMS_MODE_CDMA,
   AEESMS_MODE_GW
};
typedef uint32 AEESMSMode;


Members:

   AEESMS_MODE_CDMA            : CDMA Message Mode
   AEESMS_MODE_GW              : GSM / WCDMA Message Mode

Comments:
   None

See Also:
   None

======================================================================= 
AEESMSBCSrvID

Description:
   Broadcast service ID. Serves as identifier for a broadcast service.

Definition:

typedef struct
{
   AEESMSBCType srvType;
   uint32 dwLang;
   uint16 fromService;
   uint16 toService;
} AEESMSBCSrvID;

Members:
   srvType: Service Type
   dwLang: Language ID
   fromService: From Service ID
   toService  : To Service ID

Comments:
   srvType and dwLang are valid only for message mode AEESMS_MODE_CDMA.
   fromService and toService are valid only for message mode AEESMS_MODE_GW.

See Also:
   AEESMSBCType

=======================================================================
AEESMSClientStatus

Description:
   Specifies the status of main client. This status is used to decide
decide how ISMS should acknowledge the incoming message.

Definition:

{
   AEESMS_CS_OK,
   AEESMS_CS_OUT_OF_RESOURCES,
   AEESMS_CS_BUSY,
   AEESMS_CS_BLOCK
};
typedef uint32 AEESMSClientStatus;

Members:

   AEESMS_CS_OK : OK status
   AEESMS_CS_OUT_OF_RESOURCES : Out of resources
   AEESMS_CS_BUSY : Busy with something that precludes use of SMS 
                    or probably just congested. 
   AEESMS_CS_BLOCK : The device is not authorized to 
                     receive the SMS message

Comments:
   None

See Also:
   ISMS_SetClientStatus

=======================================================================
AEESMS Domain Preference

Description:
   Specify domain preference for GW (GSM, WCDMA) messages. Domain
preference can be set and get using ICONFIG_SetItem and ICONFIG_GetItem
respectively with CFGI_SMS_GW_DOMAIN_PREF.

Definition:

#define AEESMS_GW_DOMAIN_PREF_CS_PREFERRED      0
#define AEESMS_GW_DOMAIN_PREF_PS_PREFERRED      1
#define AEESMS_GW_DOMAIN_PREF_CS_ONLY           2
#define AEESMS_GW_DOMAIN_PREF_PS_ONLY           3

Members:

AEESMS_GW_DOMAIN_PREF_CS_PREFERRED: Circuit Switch Preferred
AEESMS_GW_DOMAIN_PREF_PS_PREFERRED: Packet Switch Preferred
AEESMS_GW_DOMAIN_PREF_CS_ONLY: Circuit Switch Only
AEESMS_GW_DOMAIN_PREF_PS_ONLY: Packet Switch Only

Comments:
   Return code for ICONFIG_SetItem(CFGI_SMS_GW_DOMAIN_PREF) only indicates
if request for domain preference change is accepted or not. Clients are advised 
to use ICONFIG_GetItem(CFGI_SMS_GW_DOMAIN_PREF), asynchronously, to check if 
request for domain preference change is satisfied.

See Also:
   ICONFIG_SetItem(), ICONFIG_GetItem(), CFGI_SMS_GW_DOMAIN_PREF

=======================================================================
ISMS model events

Description:
   Events provided to listeners registered with ISMS IModel interface.

Definition:

#define EVT_MDL_SMS_MSG_SENT            (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_SMS_DUPLICATE_CB_PAGE   (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_SMS_CELL_CHANGE         (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_SMS_CS_CHANGE           (EVT_MDL_GENERIC_CHANGE + 4)

Members:

EVT_MDL_SMS_MSG_SENT       : Event sent when a message is sent. dwParam of ModelEvent 
                             struct points to ISMSMsg interface. ISMS calls ISMSMSG_Release() 
                             after sending the model event. Clients needing to hold onto 
                             ISMSMsg interface must call ISMSMSG_AddRef().

EVT_MDL_SMS_DUPLICATE_CB_PAGE : Event sent for a duplicate broadcast message. dwParam 
                                of the ModelEvent struct points to ISMSMsg interface
                                containing following message options:
                                    MSGOPT_BC_SERVICE_ID
                                    MSGOPT_BC_PAGE_NUMBER
                                    MSGOPT_BC_TOTAL_PAGES
                                    MSGOPT_BC_GEO_SCOPE
                                    MSGOPT_BC_MESSAGE_CODE
                                    MSGOPT_BC_UPDATE_NUMBER
                                    MSGOPT_LANG
                                    MSGOPT_PAYLOAD_ENCODING
                             Clients needing to hold onto ISMSMsg interface must call 
                             ISMSMSG_AddRef().

EVT_MDL_SMS_CELL_CHANGE    : Event sent on cell change. dwParam of the ModelEvent struct
                             specifies AEESMSCellChangeType

EVT_MDL_SMS_CS_CHANGE : Event sent when Client Status is changed.
                        dwParam of the ModelEvent structure
                        points to AEESMSCSChangeEvtData.


Comments:

See Also:
   ISMSMsg, AEESMSCSChangeEvtData

=======================================================================
AEESMSCSChangeEvtData

Description:
   Contains event data for events notified by IModel interface 
   supported by ISMS for EVT_MDL_SMS_CS_CHANGE.

Definition:

typedef struct
{
   AEESMSType         mt;
   AEESMSClientStatus oldCS;
   AEESMSClientStatus newCS;
} AEESMSCSChangeEvtData;

Members:
   mt    : SMS Message Type value
   oldCS : Old Client Status value
   newCS : New Client Status value

Comments:
   None

See Also:
   ISMS model event EVT_MDL_SMS_CS_CHANGE

=======================================================================
ISMSStorage model events

Description:
   Events provided to listeners registered with ISMSStorage IModel
   interface.

Definition:

#define EVT_MDL_SMSSTORAGE_STORE        (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_SMSSTORAGE_UPDATE       (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_SMSSTORAGE_DELETE       (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_SMSSTORAGE_DELETE_TAG   (EVT_MDL_GENERIC_CHANGE + 4)
#define EVT_MDL_SMSSTORAGE_DELETE_ALL   (EVT_MDL_GENERIC_CHANGE + 5)
#define EVT_MDL_SMSSTORAGE_READY        (EVT_MDL_GENERIC_CHANGE + 6)
#define EVT_MDL_SMSSTORAGE_FULL         (EVT_MDL_GENERIC_CHANGE + 7)

Members:

EVT_MDL_SMSSTORAGE_STORE      : Event sent when a message is stored on specified storage type
EVT_MDL_SMSSTORAGE_UPDATE     : Event sent when a message is updated on specified storage type
EVT_MDL_SMSSTORAGE_DELETE     : Event sent when a message is deleted from specified storage type
EVT_MDL_SMSSTORAGE_DELETE_TAG : Event sent when messages of specified tag are deleted from specified storage type
EVT_MDL_SMSSTORAGE_DELETE_ALL : Event sent when all messages are deleted from specified storage type
EVT_MDL_SMSSTORAGE_READY      : Event sent when specified storage type is ready for use
EVT_MDL_SMSSTORAGE_FULL       : Event sent when specified storage type is full

Comments:
   dwParam of ModelEvent struct points to AEESMSStorageEvtData structure
   containing event specific data.

See Also:
   AEESMSStorageEvtData

=======================================================================
ISMSStorage2 model events

Description:
   Events provided to listeners registered with ISMSStorage2 IModel
   interface.

Definition:

#define EVT_MDL_SMSSTORAGE_STORE        (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_SMSSTORAGE_UPDATE       (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_SMSSTORAGE_DELETE       (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_SMSSTORAGE_DELETE_TAG   (EVT_MDL_GENERIC_CHANGE + 4)
#define EVT_MDL_SMSSTORAGE_DELETE_ALL   (EVT_MDL_GENERIC_CHANGE + 5)
#define EVT_MDL_SMSSTORAGE_READY        (EVT_MDL_GENERIC_CHANGE + 6)
#define EVT_MDL_SMSSTORAGE_FULL         (EVT_MDL_GENERIC_CHANGE + 7)
#define EVT_MDL_SMSSTORAGE_CLASS_0_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 8)
#define EVT_MDL_SMSSTORAGE_CLASS_1_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 9)
#define EVT_MDL_SMSSTORAGE_CLASS_2_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 10)
#define EVT_MDL_SMSSTORAGE_CLASS_3_AUTOSTORE       (EVT_MDL_GENERIC_CHANGE + 11)
#define EVT_MDL_SMSSTORAGE_CLASS_NONE_AUTOSTORE    (EVT_MDL_GENERIC_CHANGE + 12)
#define EVT_MDL_SMSSTORAGE_CLASS_CDMA_AUTOSTORE    (EVT_MDL_GENERIC_CHANGE + 13)

Members:

EVT_MDL_SMSSTORAGE_STORE      : Event sent when a message is stored on specified storage type
EVT_MDL_SMSSTORAGE_UPDATE     : Event sent when a message is updated on specified storage type
EVT_MDL_SMSSTORAGE_DELETE     : Event sent when a message is deleted from specified storage type
EVT_MDL_SMSSTORAGE_DELETE_TAG : Event sent when messages of specified tag are deleted from specified storage type
EVT_MDL_SMSSTORAGE_DELETE_ALL : Event sent when all messages are deleted from specified storage type
EVT_MDL_SMSSTORAGE_READY      : Event sent when specified storage type is ready for use
EVT_MDL_SMSSTORAGE_FULL       : Event sent when specified storage type is full
EVT_MDL_SMSSTORAGE_CLASS_0_AUTOSTORE      : Event sent when auto store is changed for class 0 messages
EVT_MDL_SMSSTORAGE_CLASS_1_AUTOSTORE      : Event sent when auto store is changed for class 1 messages
EVT_MDL_SMSSTORAGE_CLASS_2_AUTOSTORE      : Event sent when auto store is changed for class 2 messages
EVT_MDL_SMSSTORAGE_CLASS_3_AUTOSTORE      : Event sent when auto store is changed for class 3 messages
EVT_MDL_SMSSTORAGE_CLASS_NONE_AUTOSTORE   : Event sent when auto store is changed for class none messages
EVT_MDL_SMSSTORAGE_CLASS_CDMA_AUTOSTORE   : Event sent when auto store is changed for CDMA messages

Comments:
   dwParam of ModelEvent struct points to AEESMSStorageEvtData structure
   containing event specific data.

See Also:
   AEESMSStorageEvtData

======================================================================= 
AEESMSStorageEvtData

Description:
   Contains event specific data for events notified by IModel interface 
   supported by ISMSStorage.

Definition:

typedef struct
{
   AEESMSStorageType st;
   uint32 nIndex;
   AEESMSTag mt;
} AEESMSStorageEvtData;

Members:
   st : Storage Type
   nIndex : Message Index
   mt : Message Tag

Comments:
   None

See Also:
   ISMSStorage model events

=======================================================================
ISMSBCConfig model events

Description:
   Events provided to listeners registered with ISMSBCConfig IModel
   interface.

Definition:

#define EVT_MDL_SMSBCCONFIG_CONFIG                    (EVT_MDL_GENERIC_CHANGE + 1)
#define EVT_MDL_SMSBCCONFIG_PREF                      (EVT_MDL_GENERIC_CHANGE + 2)
#define EVT_MDL_SMSBCCONFIG_ADDED                     (EVT_MDL_GENERIC_CHANGE + 3)
#define EVT_MDL_SMSBCCONFIG_UPDATED                   (EVT_MDL_GENERIC_CHANGE + 4)
#define EVT_MDL_SMSBCCONFIG_DELETED                   (EVT_MDL_GENERIC_CHANGE + 5)
#define EVT_MDL_SMSBCCONFIG_SELECT_ALL_SRVS           (EVT_MDL_GENERIC_CHANGE + 6)
#define EVT_MDL_SMSBCCONFIG_SET_PRIORITY_ALL_SRVS     (EVT_MDL_GENERIC_CHANGE + 7)
#define EVT_MDL_SMSBCCONFIG_DELETE_ALL_SRVS           (EVT_MDL_GENERIC_CHANGE + 8)

Members:

EVT_MDL_SMSBCCONFIG_CONFIG                   : Event sent when broadcast configuration changes.
                                               dwParam of model event specifies the mode.

EVT_MDL_SMSBCCONFIG_PREF                     : Event sent when broadcast preference changes.
                                               dwParam of model event specifies the mode.

EVT_MDL_SMSBCCONFIG_ADDED                    : Event sent when a new broadcast service is added
                                               to the broadcast service table.dwParam of model 
                                               event specifies the mode.

EVT_MDL_SMSBCCONFIG_UPDATED                  : Event sent when a new broadcast service is updated.
                                               dwParam of model points to AEESMSBCConfigEvtData
                                               structure.

EVT_MDL_SMSBCCONFIG_DELETED                  : Event sent when a broadcast service is deleted
                                               from the broadcast service table. dwParam of model 
                                               event specifies the mode.

EVT_MDL_SMSBCCONFIG_SELECT_ALL_SRVS          : Event sent when all services are selected. dwParam 
                                               of model event specifies the mode.

EVT_MDL_SMSBCCONFIG_SET_PRIORITY_ALL_SRVS    : Event sent when proority for all services is set.
                                               dwParam of model event specifies the mode.

EVT_MDL_SMSBCCONFIG_DELETE_ALL_SRVS          : Event sent when all services are deleted.
                                               dwParam of model event specifies the mode.

Comments:
   None

See Also:
   AEESMSBCConfigEvtData

======================================================================= 
AEESMSBCConfigEvtData

Description:
   Contains event specific data for events notified by IModel interface 
   supported by ISMSBCConfig.

Definition:

typedef struct
{
   AEESMSMode             mode;        // Service Mode   
   AEESMSBCSrvID          sSrvID;      // Service ID
} AEESMSBCConfigEvtData;

Members:
   mode : Service Mode
   sSrvID : Service ID

Comments:
   None

See Also:
   ISMSBCConfig model events

=======================================================================
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISMSMsg

Description:
   ISMSMsg interface encapsulates message that can be sent/received by an
application using ISMS interface. It inherits from IWebOpts to let
application specify/access various parameters associated with the message.

For sending message, the ISMSMsg interface instance is obtained via
ISHELL_CreateInstance() mechanism. While receiving message, message ID
is provided in ((AEENotify *)dwParam)->pData of the EVT_NOTIFY. Applications 
can then receive message corresponding to the message ID using ISMS_ReceiveMsg()

ISMSMsg exhibits LIFO queue behavior. This means application interested in certain 
order of options would need to take into account that ISMSMsg is LIFO queue and act 
accordingly as shown below.

MO:

If application wants p1, p2 and p3 to go in order of p1, p2, p3 on network, application 
would need to do:

ISMSMSG_AddOpt(p3);
ISMSMSG_AddOpt(p2);
ISMSMSG_AddOpt(p1);

MT:

If application wants options of certain ID (e.g. MSGOPT_UDH_PREDEFINED) to be retrieved 
in the order in which these are received, application would need to do:

{
   nIndex = 0;
   while (ISMSMSG_GetOptWithIndex(pISMSMsg, MSGOPT_UDH_PREDEFINED, nIndex++, &smo) == SUCCESS); 
   while (ISMSMSG_GetOptWithIndex(pISMSMsg, MSGOPT_UDH_PREDEFINED, nIndex--, &smo) == SUCCESS)
   {
      // Process smo
      …
   }
}

=============================================================================

ISMSMSG_AddRef()

Description:
   This method increments the reference count of the ISMSMsg Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSMSG_AddRef(ISMSMsg * po)

Parameters:

   po [in]: Pointer to the ISMSMsg Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   ISMSMSG_Release()

=============================================================================

ISMSMSG_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSMSG_Release(ISMSMsg * po)

Parameters:

   po [in]: Pointer to the ISMSMsg Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   ISMSMSG_AddRef()

==============================================================================

Function: ISMSMSG_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int ISMSMSG_QueryInterface(ISMSMsg *po, AEECLSID id, void **p)

Parameters:

   po [in]: Pointer to ISMSMsg interface

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:
   SUCCESS: on success,
   ECLASSNOTSUPPORT: if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMSMSG_AddOpt()

Description:
   Append the list of SMSMsgOpts to the current message option set

Prototype:

  int ISMSMSG_AddOpt(ISMSMsg *po, SMSMsgOpt *apwoz)

Parameters:

  po: the interface pointer

  apwoz: array of SMSMsgOpt structs, terminated by a SMSMsgOpt with
         nId set to MSGOPT_END

Return Value:
   SUCCESS: if the options could be added successfully
   ENOMEMORY if enough memory couldn't be allocated

Comments:
   Even though multiple SMSMsgOpts corresponding to an option ID
   can be added, ISMSMsg uses only the most recently added
   SMSMsgOpt.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMSMSG_RemoveOpt()

Description:
   Removes the most recently added SMSMsgOpt matching nOptId

Prototype:

   int ISMSMSG_RemoveOpt(ISMSMsg *po, int nOptId)

Parameters:

   po:  the interface pointer

   nOptId: id of the option to remove from the options list

Return Value:
   SUCCESS: if the options could be added successfully
   EFAILED: if the specified SMSMsgOpt couldn't be found

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMSMSG_GetOpt()

Description:

   Find most recently added SMSMsgOpt, in the list of SMSMsgOpts in po,
   matching nOptId. pwo is filled in with a the matching SMSMsgOpt contents.

Prototype:

   int ISMSMSG_GetOpt(ISMSMsg *po, int32 nOptId, SMSMsgOpt *pwo)

Parameters:
   po :  the interface pointer

   nOptId: id of the option to find in the options list

   pwo: where to stick the found SMSMsgOpt

Return Value:
   SUCCESS: if the options could be added successfully
   EFAILED: if the specified SMSMsgOpt couldn't be found

Comments:
   None

Side Effects:
   None

See Also:
   None

=============================================================================

Function: ISMSMSG_GetOptWithIndex()

Description: 
    
   Find SMSMsgOpt, in the list of SMSMsgOpts in po, matching nOptId and index. 
   pwo is filled in with a the matching SMSMsgOpt contents.

Prototype:

   int ISMSMSG_GetOptWithIndex(ISMSMsg *po, int32 nOptId, int32 index, SMSMsgOpt *pwo)

Parameters:
   po :  the interface pointer

   nOptId: id of the option to find in the options list

   index: Option Index.

   pwo: where to stick the found SMSMsgOpt

Return Value:  
   SUCCESS: if the options could be added successfully
   EFAILED: if the specified SMSMsgOpt couldn't be found
    
Comments:
   None

Side Effects:
   None

See Also:
   None

=============================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISMS

Description:
   ISMS interface lets application send/receive messages encapsulated by
ISMSMsg interface.

   The ISMS interface instance is obtained via ISHELL_CreateInstance()
mechanism.

   ISMS also supports IModel interfcae. Client can access IModel interface by 
calling ISMS_QueryInterface() with AEEIID_MODEL and register listeners using 
IMODEL_AddListener.

=============================================================================

ISMS_AddRef()

Description:
   This method increments the reference count of the ISMS Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 ISMS_AddRef(ISMS * po)

Parameters:

   po [in]: Pointer to the ISMS Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   ISMS_Release()

=============================================================================

ISMS_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 ISMS_Release(ISMS * po)

Parameters:

   po [in]: Pointer to the ISMS Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   ISMS_AddRef()

==============================================================================

Function: ISMS_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int ISMS_QueryInterface(ISMS *po, AEECLSID id, void **p)

Parameters:

   po [in]: Pointer to ISMS interface.

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:
   SUCCESS: on success,
   ECLASSNOTSUPPORT: if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

See Also:
  None
=======================================================================

Function: ISMS_SendMsg()

Description:
   This method is used to send messages from the handset.This method can be used to
   send messages to either a specific BREW application on another handset or a general
   text message to the handset. On successful message send, the client sending message,
   if registered with IModel interface supported by ISMS, is sent EVT_MDL_SMS_MSG_SENT.

Prototype:
   void ISMS_SendMsg(ISMS *po, const ISMSMsg * pISMSMsg, AEECallback * pcb, uint32 *pnRv)

Parameters:
   po [in]: Pointer to ISMS interface

   pISMSMsg [in]: Pointer to ISMSMsg interface pointing to the message to be sent

   pcb [in]: Pointer to callback to inform the status of the message sent

   pnRv [out]: Place holder for return value

Return Value:

   Higher 16 bits specify error type specified as AEESMS_ERRORTYPE_XXX where as lower
   16 bits specify error specified as AEESMS_ERROR_XXX. Applications can use 
   AEESMS_GETERRORTYPE() to access error type and AEESMS_GETERROR() to access error.

Comments:

   Clients calling ISMS_SendMsg can specify payload encoding as well as MO SMS encoding 
   by setting MSGOPT_PAYLOAD_ENCODING and MSGOPT_MOSMS_ENCODING options of ISMSMsg
   respectively.

   Payload encoding types are defined as AEE_ENC_XXX whereas MO SMS encoding types are
   defined as AEESMSEncType.

   Valid combinations for Payload and MO SMS encoding types are as following. All other 
   combinations except the ones listed below will result in an erroneous result.

   Please note that specified MO SMS encoding must be one of the MO SMS encoding types 
   supported by the device. ISMS_GetEncodingsAvailableForMOSMS() provides the list of 
   MO SMS encoding types supported by the device.

<pre>
   Payload Encoding           MO SMS Encoding            Comments
 
   <any encoding>             AEESMS_ENC_OCTET
 
   AEE_ENC_ISOLATIN1          AEESMS_ENC_ASCII           Will work only if each byte of payload
                                                         can be packed in 7 bits
 
   AEE_ENC_UNICODE            AEESMS_ENC_ASCII           Will work only if each uint16 of payload
                                                         can be packed in 7 bits
 
   AEE_ENC_EUC_CN             AEESMS_ENC_ASCII           Will work only if each uint16 of payload
                                                         can be packed in 7 bits
 
   AEE_ENC_ISOLATIN1          AEESMS_ENC_LATIN           
 
   AEE_ENC_UNICODE            AEESMS_ENC_LATIN           Will work only if each uint16 of payload
                                                         can be packed in 8 bits
 
   AEE_ENC_EUC_CN             AEESMS_ENC_LATIN           Will work only if each uint16 of payload
                                                         can be packed in 8 bits
 
   AEE_ENC_ISOLATIN1          AEESMS_ENC_UNICODE
 
   AEE_ENC_UNICODE            AEESMS_ENC_UNICODE
 
   AEE_ENC_EUC_CN             AEESMS_ENC_UNICODE
 
   AEE_ENC_ISOLATIN1          AEESMS_ENC_IS91EP
 
   AEE_ENC_ISOLATIN1          AEESMS_ENC_IA5             Will work only if each byte of payload
                                                         can be packed in 7 bits
 
   AEE_ENC_S_JIS              AEESMS_ENC_SHIFT_JIS
 
   AEE_ENC_KSC5601            AEESMS_ENC_KOREAN
 
   AEE_ENC_LATIN_HEBREW       AEESMS_ENC_LATIN_HEBREW
 
   AEE_ENC_GSM_7_BIT_DEFAULT  AEESMS_ENC_GSM_7_BIT_DEFAULT
</pre>

Side Effects:
   None

See Also:
   ISMS model events

=======================================================================

Function: ISMS_ReceiveMsg()

Description:
   Applications are notified of messages by EVT_NOTIFY where 
   ((AEENotify *)dwParam)->pData contains message ID. Using this method, 
   applications can receive the message.

Prototype:
   int ISMS_ReceiveMsg(ISMS *po, uint32 uMsgId, ISMSMsg **ppMsg)

Parameters:
   po [in]: Pointer to ISMS interface

   uMsgId [in]: Message ID

   ppMsg [out]: Place holder for ISMSMsg interface pointing to the message

Return Value:
   SUCCESS: if successful.

Comments:
   Message payload of encoding AEE_ENC_UNICODE is specified using MSGOPT_PAYLOAD_WSZ.~
   Message payload of encoding AEE_ENC_ISOLATIN1 is specified using MSGOPT_PAYLOAD_SZ.~
   Message payload of other encoding types are specified using MSGOPT_PAYLOAD_BINARY.

   Apart from messages directed at the native application, BREW also supports the
   following special SMS messages

   BREW Directed SMS
   * Directed to a specific class id. 
   * Sender needs to know the recipients class ID. 
   * Delivered by BREW to the specific application by querying class associated with that class id. 
   * If an instance of the application is not present, BREW creates one and delivers it using its HandleEvent() function. 
   * Sent as //BREW:<ClassID>:<Message>
   * Application is loaded with EVT_APP_MESSAGE and is delivered <Message>. Application can start itself on this event.
   * EVT_APP_MESSAGE Behavior is retained for Backward Compatibility
   * EVT_APP_MESSAGE_EX added to provide 32 bit message ID to destination application
   * Destination application can get full message by using ISMS_ReceiveMsg() with message ID
 

   BREW Handler Directed SMS
   * Directed at a specific mime handler (mime handler needs to be registered with BREW Registry). 
   * Sender does not need to know the recipient’s class ID. 
   * This is just like any other URI handling mechanism, except that it is done via an SMS 
     so can be trigged by entities external to the phone. 
   * Brew loads and starts the application with the given payload and the application gets control of the display. 
   * Message format is //<SMS_Prefix>:<Message_Text>
   * Applet must register as handler for <SMS_Prefix>
   * Application is started and is delivered EVT_APP_BROWSE_URL.  
   * In addition, application is also delivered EVT_APP_MESSAGE_EX to differentiate between invocation from other apps and SMS.
   * dwParam on EVT_APP_BROWSE_URL contains pointer to //<SMS_Prefix>:<Message_Text>

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMS_SetClientStatus()

Description:
   Use of this API will expect that caller has AEECLSID_SMSMAINCLIENT privilege.

Using this API, main SMS client can specify client memory status for a message type. 
This status is used to decide how ISMS should acknowledge the incoming message for 
the specified message type.

Prototype:
   int ISMS_SetClientStatus(ISMS *po, AEESMSClientStatus cs, AEESMSType mt); 

Parameters:
   po [in]: Pointer to ISMS interface

   cs [in]: Client Status

   mt [in]: Message Type

            On device operating in CDMA mode, acceptable values of this parameter are 
            AEESMS_TYPE_TEXT, AEESMS_TYPE_PAGE, AEESMS_TYPE_VOICE_MAIL_NOTIFICATION,
            AEESMS_TYPE_WAP, AEESMS_TYPE_EMS and AEESMS_TYPE_ANY.
            
            On device operating in GSM mode, only acceptable value for this parameter is 
            AEESMS_TYPE_ANY.

Return Value:
   SUCCESS:  If successful
   EFAILED:  If failed
   EBADPARM: Invalid Parameter
   EPRIVLEVEL:  App doesn't have AEECLSID_SMSMAINCLIENT privilege

Comments:
   AEECLSID_SMSMAINCLIENT class ID is used solely for privilege to do set client
   status. This class ID only goes into the calling apps's extension list to get
   signed for permission to be able to set main client status.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMS_GetBytesAvailableForPayload()

Description:
   Checks various ISMSMsg parameters and finds that how many more bytes and
   bits are available for payload. If pISMSMsg is NULL pointer, it returns
   maximum number of bytes allowed for payload.

Prototype:
   int ISMS_GetBytesAvailableForPayload(ISMS * po, ISMSMsg * pISMSMsg, int * pnBytes, int * pnPadding)

Parameters:
   po [in]: Pointer to ISMS interface

   pISMSMsg [in]: Pointer to ISMSMsg interface

   pnBytes [out]: Place holder for bytes available for payload

   pnPadding [out]: Place holder for bits padded. useful only if MSGOPT_MOSMS_ENCODING
                    is AEESMS_ENC_ASCII or AEESMS_ENC_GSM_7_BIT_DEFAULT

Return Value:
   SUCCESS:  If successful
   EFAILED:  Otherwise

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMS_GetEncodingsAvailableForMOSMS()

Description:
   Provides the list of encodings supported by the device for mobile
   originated messages.

Prototype:
   int ISMS_GetEncodingsAvailableForMOSMS(ISMS * po, uint32 * pBuffer, uint32 * pnSize)

Parameters:
   po [in]: Pointer to ISMS interface

   pBuffer [out]: Place holder for available MO SMS encoding options

   pSize [out]: Place holder for size of the memory referred by pBuffer

Return Value:
   SUCCESS:  If successful
   EFAILED:  Otherwise

Comments:
   Applications can pass NULL pBuffer to query memory size required for holding
   MO SMS encoding options.

Side Effects:
   None

See Also:
   None

=======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISMSNotifier

Description:
  ISMSNotifier is the notifier class with which applications can register
  for SMS notifications in following ways:

  1. Using application MIF
  2. Using ISHELL_RegisterNotify()

  Applications are not required and cann't create an instance of ISMSNotifier.

Following are the notification masks supported by ISMSNotifier:

#define NMASK_SMS_TYPE         0x0001   Arrival of SMS for specific AEESMSType
#define NMASK_SMS_BROADCAST    0x0002   Arrival of broadcast SMS
#define NMASK_SMS_TYPE_LOOPBACK  0x0004 Arrival of a loopback SMS 

=======================================================================

NMASK_SMS_TYPE

Description:

   This is a notification mask that allows applications to register for SMS message
of a specific message type. To construct the actual 32-bit mask to be used in the
MIF for the registration, the upper 16 bits of the mask must contain the message
type and the lower 16 bits must contain NMASK_SMS_TYPE.
For example: To register for voice mail notifications, a message with type
AEESMS_TYPE_VOICE_MAIL_NOTIFICATION, the 32-mask should be:
((AEESMS_TYPE_VOICE_MAIL_NOTIFICATION << 16) | NMASK_SMS_TYPE).
When the SMS message of specified message typeis received by the system, the application
is notified through the EVT_NOTIFY event. The dwParam of this event is of type AEENotify.
The pData member in this AEENotify structure will contain message ID. Apps can use
ISMS_ReceiveMsg() to receive message corresponding to the message ID.

Definition:
   #define NMASK_SMS_TYPE     0x0001

Members:
   None

Comments:
   None


See Also:
   None

=======================================================================

NMASK_SMS_BROADCAST

Description:

   This is a notification mask that allows applications to register for SMS
broadcast messages of a specific type. To construct the actual 32-bit mask to
be used in the MIF for the registration, the upper 16 bits of the mask must
contain the message broadcast service ID and the lower 16 bits must contain 
NMASK_SMS_BROADCAST.

For example: 

   To register for emergency broadcast messages on CDMA network, a message with 
type AEESMS_BC_EMERGENCY, the 32-mask should be:
   ((AEESMS_BC_EMERGENCY << 16) | NMASK_SMS_BROADCAST).

   To register for service ID 10 messages on GW network, the 32-mask should be:
   ((10 << 16) | NMASK_SMS_BROADCAST).

When the broadcast SMS message of specified message type is received by the system,
the application is notified through the EVT_NOTIFY event. The dwParam of this event
is of type AEENotify. The pData member in this AEENotify structure will contain
message ID. Apps can use ISMS_ReceiveMsg() to receive message corresponding
to the message ID.

Definition:
   #define NMASK_SMS_BROADCAST     0x0002

Members:
   None

Comments:
   None


See Also:
   None

=======================================================================

NMASK_SMS_TYPE_LOOPBACK

Description:

   This is similar to NMASK_SMS_TYPE except that it is specific to 
loopback messages (i.e. a message that is not sent out to the network 
but delivered back to the sender again.)

Definition:
   #define NMASK_SMS_TYPE_LOOPBACK     0x0004

Members:
   None

Comments:
   None


See Also:
   NMASK_SMS_TYPE 
   MSGOPT_LOOPBACK_MSG 
   MSGOPT_LOOPBACK_FROM_APPLET

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISMSStorage

Description:
   ISMSStorage interface encapsulates SMS storage (RUIM/SIM). Use of this 
   interface will expect that caller has AEECLSID_SMSMAINCLIENT privilege.

   ISMSStorage also supports IModel interfcae. Client can access IModel
   interface by calling ISMSSTORAGE_QueryInterface() with AEEIID_MODEL and 
   register listeners using IMODEL_AddListener.

=============================================================================

ISMSSTORAGE_AddRef()

Description:
   This method increments the reference count of the ISMSStorage Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSSTORAGE_AddRef(ISMSStorage * po)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   ISMSSTORAGE_Release()

=============================================================================

ISMSSTORAGE_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSSTORAGE_Release(ISMSStorage * po)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   ISMSSTORAGE_AddRef()

==============================================================================

Function: ISMSSTORAGE_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int ISMSSTORAGE_QueryInterface(ISMSStorage *po, AEECLSID id, void **p)

Parameters:

   po [in]: Pointer to ISMSStorage interface

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:
   SUCCESS: on success,
   ECLASSNOTSUPPORT: if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

See Also:
   None

=============================================================================

ISMSSTORAGE_GetStorageStatus()

Description:
   This method returns storage status corresponding to the specified storage type.
   This can also be used to count messages of a particular tag stored on the specified
   storage type.

Prototype:
   void ISMSSTORAGE_GetStorageStatus(ISMSStorage * po, AEESMSStorageType st, AEESMSTag mt, AEECallback *pcb, AEESMSStorageStatus * pss, uint32 *pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   mt [in]: Message Tag
   pcb [in]: Callback
   pss [out]: Place holder for storage status
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pss and pnErr must remain valid during entire transaction.These are filled by
   ISMSSTORAGE and can be examined in the callback function.

Side Effects:
   None

See Also:
   None

=============================================================================

ISMSSTORAGE_StoreMsg()

Description:
   This method stores a message on the specified storage type.

Prototype:
   void ISMSSTORAGE_StoreMsg(ISMSStorage * po, AEESMSStorageType st, ISMSMsg * pi, AEECallback *pcb, uint32 * pnIndex, uint32 *pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   pi [in]: Pointer to ISMSMsg interface
   pcb [in]: Callback
   pnIndex [out]: Place holder for message index in SMS storage
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnIndex and pnErr must remain valid during entire transaction. These are filled by
   ISMSSTORAGE and can be examined in the callback function.

   On successful message store, clients registered with ISMSStorage interface objects,
   using IModel interface supported by ISMSStorage, are sent EVT_MDL_SMSSTORAGE_STORE.

   An ISMSMsg with Tag as Status Report will only be stored if the Mobile Originated
   (MO) SMS for which the Status Report is being stored is also stored in the same 
   Storage Types (SIM). 
   i.e. the Status Report will be stored to AEESMS_CARD_SIM_STATUS_REPORT if and only
   if the corresponding MO SMS Messsage is stored to AEESMS_CARD_SIM.

Side Effects:
   None

See Also:
   ISMSStorage model events

=============================================================================

ISMSSTORAGE_ReadMsg()

Description:
   This method reads a message from the specified storage type.

Prototype:
   void ISMSSTORAGE_ReadMsg(ISMSStorage * po, AEESMSStorageType st, uint32 nIndex, AEECallback *pcb, ISMSMsg ** ppi, uint32 *pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   nIndex [in] : Message index in SMS storage
   pcb [in]: Callback
   ppi [out]: Place holder for pointer to ISMSMsg interface. Must be released by application.
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   ppi and pnErr must remain valid during entire transaction. These are filled by
   ISMSSTORAGE and can be examined in the callback function.

Side Effects:
   None

See Also:
   None

=============================================================================

ISMSSTORAGE_DeleteAll()

Description:
   This method deletes all the messages from the specified storage type.

Prototype:
   void ISMSSTORAGE_DeleteAll(ISMSStorage * po, AEESMSStorageType st, AEECallback *pcb, uint32 *pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   pcb [in]: Callback
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnErr must remain valid during entire transaction. This is filled by
   ISMSSTORAGE and can be examined in the callback function.

   On successful deletion of all of the messages from the specified storage type, 
   clients registered with ISMSStorage interface objects, using IModel interface 
   supported by ISMSStorage, are sent EVT_MDL_SMSSTORAGE_DELETE_ALL.

Side Effects:
   None

See Also:
   ISMSStorage model events

=============================================================================

ISMSSTORAGE_DeleteMsgTag()

Description:
   This method deletes all the messages of specified tag from the specified
   storage type.

Prototype:
   void ISMSSTORAGE_DeleteMsgTag(ISMSStorage * po, AEESMSStorageType st, AEESMSTag mt, AEECallback *pcb, uint32 *pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   mt [in]: Message Tag
   pcb [in]: Callback
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnErr must remain valid during entire transaction. This is filled by
   ISMSSTORAGE and can be examined in the callback function.

   On successful deletion of all of the messages of the specified tag from 
   the specified storage type, clients registered with ISMSStorage interface 
   objects, using IModel interface supported by ISMSStorage, are sent 
   EVT_MDL_SMSSTORAGE_DELETE_TAG.

Side Effects:
   None

See Also:
   ISMSStorage model events

=============================================================================

ISMSSTORAGE_DeleteMsg()

Description:
   This method deletes message of specified index from the specified
   storage type.

Prototype:
   void ISMSSTORAGE_DeleteMsg(ISMSStorage * po, AEESMSStorageType st, uint32 nIndex, AEECallback *pcb, uint32 *pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   nIndex [in]: Message Index
   pcb [in]: Callback
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnErr must remain valid during entire transaction. This is filled by
   ISMSSTORAGE and can be examined in the callback function.

   On successful deletion of the message, clients registered with ISMSStorage 
   interface objects, using IModel interface supported by ISMSStorage, are sent 
   EVT_MDL_SMSSTORAGE_DELETE.

   On Deletion of an Mobile Originated (MO) SMS Message with a corresponding
   Status Report, the Status Report will also be Deleted.

Side Effects:
   None

See Also:
   ISMSStorage model events

=============================================================================

ISMSSTORAGE_EnumMsgInit()

Description:
   This method initializes the specified storage type for enumeration of the
messages of the specified tag. If storage device is already being enumerated,
calling ISMSSTORAGE_EnumMsgInit() resets the enumeration criteria to new
criteria and prepares storage device for enumeration as per new criteria.

Prototype:
   void ISMSSTORAGE_EnumMsgInit(ISMSStorage * po, AEESMSStorageType st, AEESMSTag mt, AEECallback *pcb, uint32 *pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   mt [in]: Message Tag
   pcb [in]: Callback
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnErr must remain valid during entire transaction. This is filled by
   ISMSSTORAGE and can be examined in the callback function.

Side Effects:
   None

See Also:
   None

=============================================================================

ISMSSTORAGE_EnumNextMsg()

Description:
   This method retrieves information about next message on specified storage type
that meets enumeration criteria specified in ISMSSTORAGE_EnumMsgInit()

Prototype:
   void ISMSSTORAGE_EnumNextMsg(ISMSStorage * po, AEESMSStorageType st, AEECallback *pcb, uint32 * pnIndex, ISMSMsg ** ppi, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   pcb [in]: Callback
   pnIndex [out]: Place holder for message index in SMS storage
   ppi [out]: Place holder for pointer to ISMSMsg interface
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   Ensure that ISMSSTORAGE_EnumMsgInit() has been called before using this method.
   Client can specify pnIndex to get index of next message or ppi to get next message
   or both to get index as well as message.

   pnIndex, ppi and pnErr must remain valid during entire transaction. These are filled by
   ISMSSTORAGE and can be examined in the callback function.

   Callback is invoked with error if error occurs during enumeration.
   
   Callback is invoked with SUCCESS with ppi, if specified, set to NULL and pnIndex,
   if specified, set to 0xffffffff, if there is no item left to return as per the criteria 
   specified in ISMSSTORAGE_EnumMsgInit.

Side Effects:
   None

See Also:
   None

=======================================================================

ISMSSTORAGE_UpdateMsg()
 
Description:
   This method updates a message in the specified storage type and index.
 
Prototype:
   void ISMSSTORAGE_UpdateMsg(ISMSStorage * po, AEESMSStorageType st, uint32 nIndex, ISMSMsg * pi, AEECallback *pcb, uint32 *pnErr)
 
Parameters:
 
   po [in]: Pointer to the ISMSStorage Interface object
   st [in]: Storage Type
   nIndex [in]: Message index in SMS storage
   pi [in]: Pointer to ISMSMsg interface
   pcb [in]: Callback
   pnErr [out]: Place holder for error
 
Return Value:
   None
 
Comments:
   pnErr must remain valid during entire transaction. These are filled by
   ISMSSTORAGE and can be examined in the callback function.				

   On successful message store, clients registered with ISMSStorage interface objects,
   using IModel interface supported by ISMSStorage, are sent EVT_MDL_SMSSTORAGE_UPDATE.

Side Effects:
   None

See Also:
   ISMSStorage model events

=======================================================================

ISMSSTORAGE_AutoStoreMsg()
 
Description:
   This method is a static inline that queries the passed interface pointer for 
   AEEIID_SMSSTORAGE2 and invokes ISMSSTORAGE2_AutoStoreMsg() on successful
   ISMSSTORAGE_QueryInterface(AEEIID_SMSSTORAGE2). Refer ISMSSTORAGE2_AutoStoreMsg()
   documentation for more details.
 
Prototype:
   int ISMSSTORAGE_AutoStoreMsg(ISMSStorage * po, AEESMSClass mc, AEESMSStorageType st, AEECallback *pcb, uint32 *pnErr)
 
Parameters:
 
   po [in]: Pointer to the ISMSStorage Interface object
   mc [in]: Message Class
   st [in]: Storage Type
   pcb [in]: Callback
   pnErr [out]: Place holder for error
 
Return Value:
   AEE_SUCCESS: If successful to invoke ISMSSTORAGE2_AutoStoreMsg()
   Error Code returned by ISMSSTORAGE_QueryInterface(AEEIID_SMSSTORAGE2) otherwise
 
Comments:
   Please note that "auto store message" might be default platform configuration 
   for a message class resulting into the messages with that class being automatically
   stored. Applications can override the default platform configuration by calling 
   this API for a message classe with AEESMS_STORAGE_NONE.

Side Effects:
   None

See Also:
   ISMSSTORAGE2_AutoStoreMsg()
=======================================================================
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISMSStorage2

Description:
   ISMSStorage2 interface inherits from ISMSStorage interface and adds
   a new method ISMSSTORAGE2_AutoStoreMsg(). Pointer to ISMSStorage2 can be 
   obtained by querying AEECLSID_SMSSTORAGE instance with AEEIID_SMSSTORAGE2.

=============================================================================

ISMSSTORAGE2_AddRef()

Description:
   This function is inherited from ISMSTORAGE_AddRef().

=============================================================================

ISMSSTORAGE2_Release()

Description:
   This function is inherited from ISMSTORAGE_Release().

==============================================================================

Function: ISMSSTORAGE2_QueryInterface()

Description:

   This function is inherited from ISMSTORAGE_QueryInterface().

=============================================================================

ISMSSTORAGE2_GetStorageStatus()

Description:
   This function is inherited from ISMSTORAGE_GetStorageStatus().

=============================================================================

ISMSSTORAGE2_StoreMsg()

Description:
   This function is inherited from ISMSTORAGE_StoreMsg().

=============================================================================

ISMSSTORAGE2_ReadMsg()

Description:
   This function is inherited from ISMSTORAGE_ReadMsg().

=============================================================================

ISMSSTORAGE2_DeleteAll()

Description:
   This function is inherited from ISMSTORAGE_DeleteAll().

=============================================================================

ISMSSTORAGE2_DeleteMsgTag()

Description:
   This function is inherited from ISMSTORAGE_DeleteMsgTag().

=============================================================================

ISMSSTORAGE2_DeleteMsg()

Description:
   This function is inherited from ISMSTORAGE_DeleteMsg().

=============================================================================

ISMSSTORAGE2_EnumMsgInit()

Description:
   This function is inherited from ISMSTORAGE_EnumMsgInit().

=============================================================================

ISMSSTORAGE2_EnumNextMsg()

Description:
   This function is inherited from ISMSTORAGE_EnumNextMsg().

=======================================================================

ISMSSTORAGE2_UpdateMsg()
 
Description:
   This function is inherited from ISMSTORAGE_UpdateMsg().

=======================================================================

ISMSSTORAGE2_AutoStoreMsg()
 
Description:
   This method enables or disables auto storage status of the messages of the specified 
   message class. The storage type and storage index information about automatically 
   stored messages is specified with MSGOPT_AUTO_STORAGE_TYPE and MSGOPT_AUTO_STORAGE_INDEX 
   respectively. An application can disable auto storage on messages of a class by calling
   this method with the desired message class and storage type AEESMS_STORAGE_NONE.
 
Prototype:
   void ISMSSTORAGE2_AutoStoreMsg(ISMSStorage2 * po, AEESMSClass mc, AEESMSStorageType st, AEECallback *pcb, uint32 *pnErr)
 
Parameters:
 
   po [in]: Pointer to the ISMSStorage2 Interface object
   mc [in]: Message Class
   st [in]: Storage Type
   pcb [in]: Callback
   pnErr [out]: Place holder for error
 
Return Value:
   None
 
Comments:
   pnErr must remain valid during entire transaction. These are filled by
   ISMSStorage2 implementation and can be examined in the callback function.				

   On successful change to auto storage status, clients registered with ISMSStorage2 
   interface objects, using IModel interface supported by ISMSStorage2, are sent 
   EVT_MDL_SMSSTORAGE_CLASS_XXX_AUTOSTORE events.

   Please note that "auto store message" might be default platform configuration 
   for a message class resulting into the messages with that class being automatically
   stored. Applications can override the default platform configuration by calling 
   this API for a message classe with AEESMS_STORAGE_NONE.
   
Side Effects:
   None

See Also:
   ISMSStorage2 model events
=======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISMSBCSrvOpts

Description:
   ISMSBCSrvOpts interface encapsulates various parameters of SMS broadcast
service identified by AEESMSBCSrvID.

The ISMSBCSrvOpts interface instance is obtained via ISHELL_CreateInstance()
mechanism.Use of this interface will expect that caller has AEECLSID_SMSMAINCLIENT 
privilege.

=============================================================================

ISMSBCSRVOPTS_AddRef()

Description:
   This method increments the reference count of the ISMSBCSrvOpts Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSBCSRVOPTS_AddRef(ISMSBCSrvOpts * po)

Parameters:

   po [in]: Pointer to the ISMSBCSrvOpts Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   ISMSBCSRVOPTS_Release()

=============================================================================

ISMSBCSRVOPTS_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSBCSRVOPTS_Release(ISMSBCSrvOpts * po)

Parameters:

   po [in]: Pointer to the ISMSBCSrvOpts Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   ISMSBCSRVOPTS_AddRef()

==============================================================================

Function: ISMSBCSRVOPTS_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int ISMSBCSRVOPTS_QueryInterface(ISMSBCSrvOpts *po, AEECLSID id, void **p)

Parameters:

   po [in]: Pointer to ISMSBCSrvOpts interface

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:
   SUCCESS: on success,
   ECLASSNOTSUPPORT: if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMSBCSRVOPTS_AddOpt()

Description:
   Append the list of SMSBCSrvOpts to the current message option set

Prototype:

  int ISMSBCSRVOPTS_AddOpt(ISMSBCSrvOpts *po, SMSBCSrvOpts *apwoz)

Parameters:

  po: the interface pointer

  apwoz: array of SMSBCSrvOpts structs, terminated by a SMSBCSrvOpts with
         nId set to SMSBCSRVOPT_END

Return Value:
   SUCCESS: if the options could be added successfully
   ENOMEMORY if enough memory couldn't be allocated

Comments:
   Even though multiple SMSBCSrvOpts corresponding to an option ID
   can be added, ISMSBCSrvOpts uses only the most recently added
   SMSBCSrvOpts.

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMSBCSRVOPTS_RemoveOpt()

Description:
   Removes the most recently added SMSBCSrvOpts matching nOptId

Prototype:

   int ISMSBCSRVOPTS_RemoveOpt(ISMSBCSrvOpts *po, int nOptId)

Parameters:

   po:  the interface pointer

   nOptId: id of the option to remove from the options list

Return Value:
   SUCCESS: if the options could be added successfully
   EFAILED: if the specified SMSBCSrvOpts couldn't be found

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

Function: ISMSBCSRVOPTS_GetOpt()

Description:

   Find most recently added SMSBCSrvOpts, in the list of SMSBCSrvOpts in po,
   matching nOptId. pwo is filled in with a the matching SMSBCSrvOpts contents.

Prototype:

   int ISMSBCSRVOPTS_GetOpt(ISMSBCSrvOpts *po, int32 nOptId, SMSBCSrvOpts *pwo)

Parameters:
   po :  the interface pointer

   nOptId: id of the option to find in the options list

   pwo: where to stick the found SMSBCSrvOpts

Return Value:
   SUCCESS: if the options could be added successfully
   EFAILED: if the specified SMSBCSrvOpts couldn't be found

Comments:
   None

Side Effects:
   None

See Also:
   None

============================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: ISMSBCConfig

Description:
   ISMSBCConfig interface lets applications configure SMS broadcast service.
Use of this interface will expect that caller has AEECLSID_SMSMAINCLIENT 
privilege.

=============================================================================

ISMSBCCONFIG_AddRef()

Description:
   This method increments the reference count of the ISMSBCConfig Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSBCCONFIG_AddRef(ISMSBCConfig * po)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   ISMSBCCONFIG_Release()

=============================================================================

ISMSBCCONFIG_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 ISMSBCCONFIG_Release(ISMSBCConfig * po)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   ISMSBCCONFIG_AddRef()

==============================================================================

Function: ISMSBCCONFIG_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int ISMSBCCONFIG_QueryInterface(ISMSBCConfig *po, AEECLSID id, void **p)

Parameters:

   po [in]: Pointer to ISMSBCConfig interface

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:
   SUCCESS: on success,
   ECLASSNOTSUPPORT: if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

See Also:
   None

=============================================================================

ISMSBCCONFIG_GetBCConfig()

Description:
   This method lets application access carriers broadcast configuration.

Prototype:
   void ISMSBCCONFIG_GetBCConfig(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, AEESMSBCConfig * pConfig, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   pcb [in]: Callback
   pConfig [out]: Place holder for carrier's broadcast configuration
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pConfig and pnErr must remain valid during entire transaction.These are filled by
   ISMSBCConfig and can be examined in the callback function.

Side Effects:
   None

See Also:
   AEESMSBCConfig

=============================================================================

ISMSBCCONFIG_GetBCPref()

Description:
   This method lets application access its preference for broadcast messages.

Prototype:
   void ISMSBCCONFIG_GetBCPref(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, AEESMSBCPref * pPref, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   pcb [in]: Callback
   pPref [out]: Place holder for application's broadcast preference
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pPref and pnErr must remain valid during entire transaction.These are filled by
   ISMSBCConfig and can be examined in the callback function.

Side Effects:
   None

See Also:
   AEESMSBCPref

=============================================================================

ISMSBCCONFIG_SetBCPref()

Description:
   This method lets application set its preference for broadcast messages.

Prototype:
   void ISMSBCCONFIG_SetBCPref(ISMSBCConfig * po, AEESMSMode mode, AEESMSBCPref nPref, AEECallback *pcb, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   nPref [in]: application's broadcast preference
   pcb [in]: Callback
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnErr must remain valid during entire transaction.This is filled by
   ISMSBCConfig and can be examined in the callback function.

Side Effects:
   None

See Also:
   AEESMSBCPref

=============================================================================

ISMSBCCONFIG_GetServiceIDs()

Description:
   This method lets application access service IDs from the broadcast SMS service
table on the device.

Prototype:
   void ISMSBCCONFIG_GetServiceIDs(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, AEESMSBCSrvID ** ppsid, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   pcb [in]: Callback
   pnCount [out]: Number of entries in service ID array returned in ppsid
   ppsid [out]: Place holder for service ID array. Need to be freed by application.
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnCount, ppsid and pnErr must remain valid during entire transaction. These are filled by
   ISMSBCConfig and can be examined in the callback function.

Side Effects:
   None

See Also:
   AEESMSBCSrvID

=============================================================================

ISMSBCCONFIG_GetServiceOpts()

Description:
   This method lets application access ISMSBCSrvOpts service option interface,
corresponding to the specified service ID, which can be used to access service
options.

Prototype:
   void ISMSBCCONFIG_GetServiceOpts(ISMSBCConfig * po, AEESMSMode mode, AEESMSBCSrvID srvID, AEECallback *pcb, ISMSBCSrvOpts ** ppi, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   srvID [in]: Service ID
   pcb [in]: Callback
   ppi [out]: Place holder for ISMSBCSrvOpts interface corresponding to the
              specified service ID. Application must release interface by calling
              ISMSBCSRVOPTS_Release()
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   ppi and pnErr must remain valid during entire transaction. These are filled by
   ISMSBCConfig and can be examined in the callback function.

Side Effects:
   None

See Also:
   AEESMSBCSrvID, ISMSBCConfig, ISMSBCCONFIG_SetServiceOpts()

=============================================================================

ISMSBCCONFIG_SetServiceOpts()

Description:
   This method lets application set or update service options corresponding to
the service ID specified in ISMSBCSrvOpts. If service is not there in device 
service table, this methods adds the service in device service table.

Prototype:
   void ISMSBCCONFIG_SetServiceOpts(ISMSBCConfig * po, AEESMSMode mode, ISMSBCSrvOpts * pi, AEECallback *pcb, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   pi [in]: ISMSBCSrvOpts interface
   pcb [in]: Callback
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnErr must remain valid during entire transaction. This is filled by
   ISMSBCConfig and can be examined in the callback function.

Side Effects:
   None

See Also:
   ISMSBCConfig, ISMSBCCONFIG_GetServiceOpts()

=============================================================================

ISMSBCCONFIG_GetAllServiceOpts()

Description: 
   This method lets application access ISMSBCSrvOpts service option interfaces
corresponding to all the service options present for the given Message Mode - CDMA or GW.

Prototype:
   void ISMSBCCONFIG_GetAllServiceOpts(ISMSBCConfig * po, AEESMSMode mode, AEECallback *pcb, uint32 * pnCount, ISMSBCSrvOpts *** pppi, uint32 * pnErr)

Parameters:
 
   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   pcb [in]: Callback
   pnCount [out]: Number of entries in list of ISMSBCSrvOpts interfaces returned in ppi
   pppi [out]: Place holder for array of ISMSBCSrvOpts interfaces corresponding to all
              service options present for the specified mode. Application must 
              release each interface by calling ISMSBCSRVOPTS_Release() on it. Application
              also need to free the memory containing interface pointers.
   pnErr [out]: Place holder for error

Return Value: 
   None 

Comments: 
   pppi and pnErr must remain valid during entire transaction. These are filled by 
   ISMSBCConfig and can be examined in the callback function.
   
   // Following is an example of how application should release interface pointers
   // and memory containing interface pointers

   ISMSBCSrvOpts ** ppISMSBCSrvOpts;
   pppi == &ppISMSBCSrvOpts;

   for (dwIndex = 0; dwIndex < *pnCount; dwIndex++)
   {
      ISMSBCSRVOPTS_Release(*(ppISMSBCSrvOpts + dwIndex));
   }
   FREE(ppISMSBCSrvOpts);

Side Effects: 
   None

=============================================================================

ISMSBCCONFIG_DeleteService()

Description:
   This method deletes specified service from the device service table.

Prototype:
   void ISMSBCCONFIG_DeleteService(ISMSBCConfig * po, AEESMSMode mode, AEESMSBCSrvID srvID, AEECallback *pcb, uint32 * pnErr)

Parameters:

   po [in]: Pointer to the ISMSBCConfig Interface object
   mode[in]: Broadcast Message Mode
   srvID [in]: Service ID
   pcb [in]: Callback
   pnErr [out]: Place holder for error

Return Value:
   None

Comments:
   pnErr must remain valid during entire transaction. This is filled by
   ISMSBCConfig and can be examined in the callback function.

Side Effects:
   None

See Also:
   AEESMSBCSrvID

=======================================================================*/

#endif   // AEESMS.h
