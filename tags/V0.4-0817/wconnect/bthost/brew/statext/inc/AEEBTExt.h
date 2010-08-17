#ifndef _AEEBTEXT_H_
#define _AEEBTEXT_H_

/*===========================================================================

FILE:      AEEBTExt.h

SERVICES:  AEE Bluetooth Data Structures

DESCRIPTION: This is the header file for Bluetooth BREW Extension

PUBLIC CLASSES:  Not Applicable

        Copyright © 2004-2009 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/


#include "AEE.h"

#ifdef __cplusplus
extern "C" {
#endif

// Base Defines
#ifdef  AEEBT_MAX_DEVICENAME_LEN
#undef  AEEBT_MAX_DEVICENAME_LEN
#endif

#ifdef  AEEBT_MAX_SERVICENAME_LEN
#undef  AEEBT_MAX_SERVICENAME_LEN
#endif

#define AEEBT_MAX_PIN_CODE_LEN      16
#define AEEBT_MAX_NICKNAME_LEN      16
#define AEEBT_MAX_DEVICENAME_LEN    30
#define AEEBT_MAX_SERVICENAME_LEN   30
#define AEEBT_SSP_PASSKEY_LEN        7 // 6-digit + 1 NULL terminating char

#define AEEBT_MAX_MEM_DIAL_DIGITS    2
#define AEEBT_MAX_PHONE_NUM_DIGITS  64
#define AEEBT_MAX_PHONEBOOK_ENTRIES 99

#define AEEBT_APP_ID_NULL           0xFFFF // The NULL/invalid app ID.

/* Size of OBEX buffer (subtract max OBEX header size (11) )*/
/* Used for all OBEX profiles that use a dynamic buffer. */
  /* Currently (16384-11) 16K - max size of OBEX header (11 bytes) */
#define AEEBT_OBEX_BUFFER_SIZE      16373

/* Link key status */
#define AEEBT_RM_LK_COMBINATION         0
#define AEEBT_RM_LK_UNBONDED            1 /* unauthenticated unbonded */
#define AEEBT_RM_LK_AUTH_UNBONDED       2 /* authenticated unbonded */
#define AEEBT_RM_LK_NONE                3 /* link key not valid */
#define AEEBT_RM_LK_AUTH_DEDICATED      4 /* authenticated dedicated bonded */
#define AEEBT_RM_LK_AUTH_GENERAL        5 /* authenticated general bonded */
#define AEEBT_RM_LK_DEDICATED           6 /* unauthenticated dedicated bonded */
#define AEEBT_RM_LK_GENERAL             7 /* unauthenticated general bonded */
#define AEEBT_RM_LK_DEBUG               8
typedef uint8 AEEBTLinkKeyStatus;


/*========================================================================
 service class part of class of device returned from device discovery
======================================================================= */
enum
{
  AEEBT_COD_SC_UNKNOWN                   = 0x0000,
  AEEBT_COD_SC_LIMITED_DISCOVERABLE_MODE = 0x0001,
  AEEBT_COD_SC_NETWORKING                = 0x0002,
  AEEBT_COD_SC_RENDERING                 = 0x0004,
  AEEBT_COD_SC_CAPTURING                 = 0x0008,
  AEEBT_COD_SC_OBJECT_TRANSFER           = 0x0010,
  AEEBT_COD_SC_AUDIO                     = 0x0020,
  AEEBT_COD_SC_TELEPHONY                 = 0x0040,
  AEEBT_COD_SC_INFORMATION               = 0x0080,
  AEEBT_COD_SC_POSITIONING               = 0x0100,
  AEEBT_COD_SC_ALL                       = 0x00FF
};
typedef uint16    AEEBTServiceClass;

/*========================================================================
 Major device class (part of Class of Device)
======================================================================= */
enum
{
  AEEBT_COD_MAJ_DEV_CLS_MISC                 = 0x0000,
  AEEBT_COD_MAJ_DEV_CLS_COMPUTER             = 0x0100,
  AEEBT_COD_MAJ_DEV_CLS_PHONE                = 0x0200,
  AEEBT_COD_MAJ_DEV_CLS_LAN_ACCESS_POINT     = 0x0300,
  AEEBT_COD_MAJ_DEV_CLS_AUDIO                = 0x0400,
  AEEBT_COD_MAJ_DEV_CLS_PERIPHERAL           = 0x0500,
  AEEBT_COD_MAJ_DEV_CLS_IMAGING              = 0x0600,
  AEEBT_COD_MAJ_DEV_CLS_UNCLASSIFIED         = 0x1F00
};
typedef uint16    AEEBTMajorDeviceClass;

/*========================================================================
 Minor device class (part of Class of Device)
======================================================================= */
enum
{
  AEEBT_COD_MIN_DEV_CLS_UNCLASSIFIED         = 0x0000,

  AEEBT_COD_MIN_DEV_CLS_DESKTOP_WORKSTATION  = AEEBT_COD_MAJ_DEV_CLS_COMPUTER + 1,
  AEEBT_COD_MIN_DEV_CLS_SERVER_CLASS_COMPUTER,
  AEEBT_COD_MIN_DEV_CLS_LAPTOP,
  AEEBT_COD_MIN_DEV_CLS_HANDHELD_PC_OR_PDA,
  AEEBT_COD_MIN_DEV_CLS_PALM_SIZED_PC_OR_PDA,
  AEEBT_COD_MIN_DEV_CLS_WEARABLE_COMPUTER,

  AEEBT_COD_MIN_DEV_CLS_CELLULAR             = AEEBT_COD_MAJ_DEV_CLS_PHONE + 1,
  AEEBT_COD_MIN_DEV_CLS_CORDLESS,
  AEEBT_COD_MIN_DEV_CLS_SMART_PHONE,
  AEEBT_COD_MIN_DEV_CLS_WIRED_MODEM_OR_VOICE_GATEWAY,
  AEEBT_COD_MIN_DEV_CLS_COMMON_ISDN_ACCESS,
  AEEBT_COD_MIN_DEV_CLS_SIM_CARD_READER,

  AEEBT_COD_MIN_DEV_CLS_FULLY_AVAILABLE      = AEEBT_COD_MAJ_DEV_CLS_LAN_ACCESS_POINT,
  AEEBT_COD_MIN_DEV_CLS_1_TO_17_PERCENT_UTILIZED,
  AEEBT_COD_MIN_DEV_CLS_17_TO_33_PERCENT_UTILIZED,
  AEEBT_COD_MIN_DEV_CLS_33_TO_50_PERCENT_UTILIZED,
  AEEBT_COD_MIN_DEV_CLS_50_to_67_PERCENT_UTILIZED,
  AEEBT_COD_MIN_DEV_CLS_67_TO_83_PERCENT_UTILIZED,
  AEEBT_COD_MIN_DEV_CLS_83_TO_99_PERCENT_UTILIZED,
  AEEBT_COD_MIN_DEV_CLS_NO_SERVICE_AVAILABLE,

  AEEBT_COD_MIN_DEV_CLS_HEADSET_PROFILE      = AEEBT_COD_MAJ_DEV_CLS_AUDIO + 1,
  AEEBT_COD_MIN_DEV_CLS_HANDSFREE
};
typedef uint16    AEEBTMinorDeviceClass;

/*========================================================================
 Bluetooth Reason Codes
======================================================================= */
enum
{
  AEEBT_REASON_SUCCESS                                = 0x00,
  AEEBT_REASON_NO_CONNECTION,
  AEEBT_REASON_HW_FAILURE,
  AEEBT_REASON_AUTHENTICATION_FAILURE,
  AEEBT_REASON_KEY_MISSING,
  AEEBT_REASON_MEMORY_FULL,
  AEEBT_REASON_CONNECTION_TIMEOUT,
  AEEBT_REASON_MAX_CONNECTIONS,
  AEEBT_REASON_ACL_ALREADY_EXISTS,
  AEEBT_REASON_CMD_DISALLOWED,
  AEEBT_REASON_CONN_REJECTED_NO_RESOURCE,
  AEEBT_REASON_CONN_REJECTED_SECURITY_FAILURE,
  AEEBT_REASON_CONN_REJECTED_PERSONAL_DEVICE,
  AEEBT_REASON_HOST_TIMEOUT,
  AEEBT_REASON_USER_ENDED_CONNECTION,
  AEEBT_REASON_POWER_GOING_OFF,
  AEEBT_REASON_LOCAL_HOST_TERMINATED_CONNECTION,
  AEEBT_REASON_REPEATED_ATTEMPTS,
  AEEBT_REASON_PAIRING_NOT_ALLOWED,
  AEEBT_REASON_UNSPECIFIED_ERROR,
  AEEBT_REASON_SWITCH_NOT_ALLOWED,
  AEEBT_REASON_ENCRYPT_MODE_NOT_ACCEPTED,
  AEEBT_REASON_INSUFFICIENT_RESOURCES,
  AEEBT_REASON_LAST
};
typedef uint8   AEEBTReason;

/*========================================================================
 Bluetooth Error Codes
======================================================================= */
#define AEEBT_ERR_BASE          0
#define AEEBT_ERR_OBEX_BASE     0x1000
#define AEEBT_ERR_OBEX_RSP_BASE (AEEBT_ERR_OBEX_BASE | 0x0100)

enum
{
  AEEBT_ERR_NONE                         = AEEBT_ERR_BASE,
  AEEBT_ERR_BAD_PARAM,
  AEEBT_ERR_RESOURCE,
  AEEBT_ERR_SD_DB_FULL,
  AEEBT_ERR_SD_REC_EXISTS,
  AEEBT_ERR_SD_REC_NOT_FOUND,
  AEEBT_ERR_UNKNOWN,
  AEEBT_ERR_INVALID_HANDLE,
  AEEBT_ERR_OUT_OF_MEMORY,
  AEEBT_ERR_ALREADY_REGISTERED,
  AEEBT_ERR_NOT_REGISTERED,
  AEEBT_ERR_ALREADY_CONNECTED,
  AEEBT_ERR_NOT_CONNECTED,
  AEEBT_ERR_CHANNEL_NOT_FOUND,
  AEEBT_ERR_NOT_INITIALIZED,
  AEEBT_ERR_INITIALIZATION_FAILED,
  AEEBT_ERR_INVALID_OPERATION,
  AEEBT_ERR_OPERATION_IN_PROGRESS,
  AEEBT_ERR_NEGOTIATION_FAILURE,

  // the following errors are triggered by the response from the remote device
  AEEBT_ERR_OBEX_DISCONNECT_FAILED       = AEEBT_ERR_OBEX_BASE,
  AEEBT_ERR_OBEX_PUT_RSP,
  AEEBT_ERR_OBEX_GET_RSP,
  AEEBT_ERR_OBEX_CONNECT_FAILED,
  AEEBT_ERR_OBEX_UNKNOWN,
  AEEBT_ERR_OBEX_FOLDER_NOT_FOUND,
  AEEBT_ERR_OBEX_BROWSING_NOT_ALLOWED,

  // the following errors map directly to OBEX response codes received from
  //  remote device
  AEEBT_ERR_OBEX_PARTIAL_CONTENT         = AEEBT_ERR_OBEX_RSP_BASE,
  AEEBT_ERR_OBEX_BAD_REQUEST,
  AEEBT_ERR_OBEX_UNAUTHORIZED,
  AEEBT_ERR_OBEX_FORBIDDEN,
  AEEBT_ERR_OBEX_NOT_FOUND,
  AEEBT_ERR_OBEX_METHOD_NOT_ALLOWED,
  AEEBT_ERR_OBEX_NOT_ACCEPTABLE,
  AEEBT_ERR_OBEX_LENGTH_REQUIRED,
  AEEBT_ERR_OBEX_PRECONDITION_FAILED,
  AEEBT_ERR_OBEX_UNSUPPORTED_MEDIA_TYPE,
  AEEBT_ERR_OBEX_INTERNAL_SERVER_ERROR,
  AEEBT_ERR_OBEX_NOT_IMPLEMENTED,
  AEEBT_ERR_OBEX_SERVICE_UNAVAILABLE,
  AEEBT_ERR_OBEX_DATABASE_FULL,
  AEEBT_ERR_OBEX_DATABASE_LOCKED,
  AEEBT_ERR_OBEX_CLIENT_ABORTED_COMMAND,
  AEEBT_ERR_OBEX_CANNOT_PROCESS_COMMAND
};
typedef uint16 AEEBTError;

/*========================================================================
 Bluetooth Device Address
======================================================================= */
typedef PACKED struct tagAEEBTAddr
{
  uint8 uAddr[6];
} GCCPACKED AEEBTBDAddr;

#define AEEBT_BD_ADDRS_EQUAL( p1, p2 ) (                              \
          (MEMCMP( (uint8*)(p1), (uint8*)(p2),                        \
                    sizeof( AEEBTBDAddr) ) == 0) ? TRUE : FALSE )


/*========================================================================
  Notifications
======================================================================= */

// BREW generated ID
#define AEECLSID_BLUETOOTH_NOTIFIER     0x0101532d

// Notification Masks...
#define NMASK_BT_AG     0x0001
#define NMASK_BT_NA     0x0002
#define NMASK_BT_RM     0x0004
#define NMASK_BT_SD     0x0008
#define NMASK_BT_SPP    0x0010
#define NMASK_BT_OPP    0x0020
#define NMASK_BT_FTP    0x0040
#define NMASK_BT_BIP    0x0080
#define NMASK_BT_BPP    0x0100
/* This notification mask is shared with HID
 event range to use is 16K to 32K */
#define NMASK_BT_SML    0x0200
#define NMASK_BT_L2     0x0400
#define NMASK_BT_CTP    0x0800
#define NMASK_BT_ICP    0x0800
#define NMASK_BT_PBAP   0x1000
#define NMASK_BT_SAP    0x2000
#define NMASK_BT_A2DP   0x4000
#define NMASK_BT_AVRCP  0x8000
/*This notification mask is shared with SML
 event range to use is 0 to 16K */
#define NMASK_BT_HID    NMASK_BT_SML /* Reusing BIT Mask Sync ML */

#define AEEBT_ENMASK    (ERROR_USER + 1)
#define AEEBT_EPORT     (ERROR_USER + 2)
#define AEEBT_EDBFULL   (ERROR_USER + 3)
#define AEEBT_ERECFULL  (ERROR_USER + 4)
#define AEEBT_ENOREC    (ERROR_USER + 5)
#define AEEBT_ENOATTR   (ERROR_USER + 6)
/* End of the service record in the queue */
#define AEEBT_EENDOFREC (ERROR_USER + 7) 

typedef struct _IBTExtNotifier IBTExtNotifier;
typedef struct _AEEBTNotification AEEBTNotification;
typedef struct _AEEBTSppStatus AEEBTSppStatus;
typedef struct _AEEBTSppConfig AEEBTSppConfig;
typedef struct _AEEBTModemStatus AEEBTModemStatus;
typedef struct _AEEBTLineError AEEBTLineError;
typedef struct _AEEBTLinkStatus AEEBTLinkStatus;
typedef struct _AEEBTDeviceUpdateStatus AEEBTDeviceUpdateStatus;
typedef struct _AEEBTAuthorizeReq AEEBTAuthorizeReq;
typedef struct _AEEBTOOBData AEEBTOOBData;
typedef uint8 AEEBTKeypressType;

// Types returned in Notification events:
typedef struct _AEEBTPasskeyEvent
{
  AEEBTBDAddr            bdAddr;
  char                   passkey[ AEEBT_SSP_PASSKEY_LEN ]; // NULL terminated
} AEEBTPasskeyEvent;

typedef struct _AEEBTBondedEvent
{
  AEEBTBDAddr            bdAddr;
  uint16                 uError;
  AEEBTLinkKeyStatus     linkKeyStatus;
} AEEBTBondedEvent;


enum
{
  AEEBT_L2_QOS_NO_TRAFFIC,
  AEEBT_L2_QOS_BEST_EFFORT,
  AEEBT_L2_QOS_GUARANTEED
};
typedef uint8 AEEBTQoSType;

typedef struct _AEEBTL2ConfigInfo
{
  uint16 psm;
  uint16 cid;
  uint16 in_flush_to;
  uint16 in_mtu;
  uint16 out_mtu;
  AEEBTQoSType in_qos;
  uint32 token_rate;
  uint32 token_bucket_size;
  uint32 peak_bandwidth;
  uint32 latency;
  uint32 delay_variation;
} AEEBTL2ConfigInfo;

#define AEEBT_L2_MTU_MIN           48
#define AEEBT_L2_MTU_DEFAULT       672

#define AEEBT_L2_FLUSH_TO_NO_RETX  1
#define AEEBT_L2_FLUSH_TO_INFINITE 0xFFFF
#define AEEBT_L2_FLUSH_TO_DEFALUT  AEEBT_L2_FLUSH_TO_INFINITE

#define AEEBT_L2_QOS_DEFAULT_TYPE            AEEBT_L2_QOS_BEST_EFFORT
#define AEEBT_L2_QOS_DEFAULT_TOKEN_RATE      0
#define AEEBT_L2_QOS_DEFAULT_BUCKET_SIZE     0
#define AEEBT_L2_QOS_DEFAULT_PEAK_BANDWIDTH  0
#define AEEBT_L2_QOS_DEFAULT_LATENCY         0xFFFFFFFF
#define AEEBT_L2_QOS_DEFAULT_DELAY_VARIATION 0xFFFFFFFF
//  AEEBT_L2_EVT_CONNECTED
typedef struct _AEEBTL2Connected
{
  AEEBTBDAddr       bdAddr;
  AEEBTL2ConfigInfo info;
} AEEBTL2Connected;

//  AEEBT_L2_EVT_CONNECT_FAILED
typedef struct _AEEBTL2ConnectFailed
{
  AEEBTBDAddr       bdAddr;
  uint16            psm;
  uint16            uError;
} AEEBTL2ConnectFailed;

//  AEEBT_L2_EVT_DISCONNECTED
typedef struct _AEEBTL2Disconnected
{
  uint16            cid;
  uint16            uError;
} AEEBTL2Disconnected;

//  AEEBT_L2_EVT_RECONFIGURED
typedef struct _AEEBTL2Reconfigured
{
  AEEBTL2ConfigInfo info;
  uint16            uError;
} AEEBTL2Reconfigured;

//  AEEBT_L2_EVT_PING_RSP
// data should be freed by the app
typedef struct _AEEBTL2PingRsp
{
  AEEBTBDAddr bdAddr;
  uint8*      data;
  uint16      size;
  uint16      uError;
} AEEBTL2PingRsp;

// AEEBT_SD_EVT_DISCOVERABLE_MODE_RESP
enum
{
  AEEBT_DISCOVERABLE_MODE_NONE,
  AEEBT_DISCOVERABLE_MODE_GENERAL,
  AEEBT_DISCOVERABLE_MODE_LIMITED
};
typedef uint8   AEEBTDiscoverableMode;

enum
{
  AEEBT_LIAC_0    /* 0x9E8B00 */
  /* Add to this enumeration as other reserved LIACs get defined by the SIG */
};
typedef uint8   AEEBTLIAC;

typedef struct _AEEBTDevDiscoveryComplete
{
  uint16  uNumDevicesDiscovered;
  uint16  uError;
} AEEBTDevDiscoveryComplete;

typedef struct _AEEBTDiscoverableModeResp
{
  uint16                 uError;
  AEEBTDiscoverableMode  mode;
  AEEBTLIAC              liac;
} AEEBTDiscoverableModeResp;

#define AEEBT_MAX_FILE_NAME               80
#define AEEBT_MAX_MIME_TYPE_LEN           51
#define AEEBT_MAX_USERID_LEN              20
#define AEEBT_MAX_PASSWORD_LEN            16

typedef uint8   AEEBTObjectType;

// OPP object types
enum
{
  AEEBT_OPP_ICAL,
  AEEBT_OPP_VCAL,
  AEEBT_OPP_VCARD,
  AEEBT_OPP_VNOTE,
  AEEBT_OPP_VMESSAGE,
  AEEBT_OPP_JPEG,
  AEEBT_OPP_UNKNOWN_TYPE
};

typedef struct _AEEBTProgressInfo
{
  uint32  objSize;
  uint32  numBytes;
} AEEBTProgressInfo;

typedef struct _AEEBTOPPObject
{
  AECHAR*         pwName;
  AEEBTObjectType objType;
  uint32          objSize;

  // Name of object that was pulled
  char*           pszName;
} AEEBTOPPObject;

enum
{
  AEEBT_FTP_FILE,
  AEEBT_FTP_FOLDER
};
typedef uint8   AEEBTFTPObject;

enum
{
  AEEBT_FTP_SET_PATH_TO_FOLDER,
  AEEBT_FTP_SET_PATH_TO_PARENT,
  AEEBT_FTP_SET_PATH_TO_ROOT
};
typedef uint8   AEEBTFTPSetPathFlag;

typedef struct _AEEBTFTPConnRequest
{
  AEEBTBDAddr     bdAddr;
  boolean         bUnauthorized;
  AECHAR          wUserID[ AEEBT_MAX_USERID_LEN ];
} AEEBTFTPConnRequest;

typedef struct _AEEBTFTPAuthRequest
{
  AEEBTBDAddr     bdAddr;
  boolean         bUserIDRequired;
} AEEBTFTPAuthRequest;

typedef struct _AEEBTFTPSetPathRequest
{
  AECHAR              wName[ AEEBT_MAX_FILE_NAME ];
  boolean             bCreate;
  AEEBTFTPSetPathFlag flag;
} AEEBTFTPSetPathRequest;

typedef struct _AEEBTFTPAccessRequest
{
  // name of object to put/get/delete/browse
  AECHAR          wName[ AEEBT_MAX_FILE_NAME ];

  AEEBTFTPObject  objType;  // file or folder
  uint32          objSize;  // size of object in Put request
} AEEBTFTPAccessRequest;


#define AEEBT_BIP_IMAGE_HANDLE_LEN         7
#define AEEBT_BIP_MAX_IMAGE_DESC_LEN     500

/* Definition for a 128-bit UUID */
typedef struct
{
  uint8            AEEBTBIP_uuid_byte[ 16 ];
} AEEBTBIPSDUUID128type;

typedef struct
{
  uint16           AEEBTBIPdataLen;
  byte*            AEEBTBIPdata;
} AEEBTBIPbyteSeqType;

// object types
typedef struct _AEEBTBIPObject
{
  AECHAR*         pwName;
  AEEBTObjectType objType;
  uint32          objSize;

  // Whether the notification corresponds to Primary or Secondary service 
  boolean         bPriServer;

  // Whether the notification is for server or client (for AEE events that
  // are common)
  boolean         bServerEvt;

  // Image descriptor XML string buffer
  char            ImageDescriptor[ AEEBT_BIP_MAX_IMAGE_DESC_LEN + 1];

  // Image handle, image name and attachment file name
  AECHAR          wImageHandle[ AEEBT_BIP_IMAGE_HANDLE_LEN + 1 ];
  AECHAR          wImageName[ AEEBT_MAX_FILE_NAME + 1 ];
  AECHAR          wAttachmentName[ AEEBT_MAX_FILE_NAME + 1 ];

  // GetImageList parameters
  uint16          handleCount;
  uint16          startOffset;
  boolean         bLatest;

  // Monitoring image store flag
  boolean         bStoreFlag;

  // GetPartialImage parameters
  uint32          partialLength;
  uint32          partialOffset;

  // RemoteDisplay parameter
  uint8           displayCommand;

  // ServiceID of secondary OBEX server (archived objects, referenced objects)
  AEEBTBIPSDUUID128type serviceID;

} AEEBTBIPObject;

/* BPP Targets */
enum
{
  AEEBT_BPP_TARGET_DPS = 1,     // Direct Printing
  AEEBT_BPP_TARGET_PBR = 2,     // Print by Reference
  AEEBT_BPP_TARGET_STS = 4,     // Status
  AEEBT_BPP_TARGET_REF = 5      // Reference Objects
};
typedef uint8      AEEBTBPPTargetType;

// object types
typedef struct _AEEBTBPPObject
{
  AECHAR*         pwName;
  AEEBTObjectType objType;
  uint32          objSize;

  // Channel/Target type
  AEEBTBPPTargetType target;

  // Authentication related
  boolean         bUserIDRequired;

  // GetReferenceObject parameters
  uint32          offset;
  int32           count;
  AECHAR          wRefObjName[ AEEBT_MAX_FILE_NAME + 1];
} AEEBTBPPObject;

typedef struct
{
  uint16           AEEBTSMLdataLen;
  byte*            AEEBTSMLdata;
} AEEBTSMLbyteSeqType;

typedef struct
{
  uint16           len;
  uint16*          str;
} AEEBTSMLunicodeStrType;

typedef struct
{
  uint8                    id;
  union
  {
    // generic header types
    uint8                  uInt8;
    uint32                 uInt32;
    AEEBTSMLbyteSeqType    byteseq;
    AEEBTSMLunicodeStrType unicode;

    // specific header types
    uint32                 count;
    uint32                 length;
    uint32                 connectionId;
    AEEBTSMLbyteSeqType    type;
    AEEBTSMLbyteSeqType    time;
    AEEBTSMLbyteSeqType    target;
    AEEBTSMLbyteSeqType    http;
    AEEBTSMLbyteSeqType    body;
    AEEBTSMLbyteSeqType    endOfBody;
    AEEBTSMLbyteSeqType    who;
    AEEBTSMLbyteSeqType    applicationParams;
    AEEBTSMLbyteSeqType    authChallenge;
    AEEBTSMLbyteSeqType    authResponse;
    AEEBTSMLbyteSeqType    objectClass;
    AEEBTSMLunicodeStrType name;
    AEEBTSMLunicodeStrType description;
  } val;
} AEEBTSMLOBEXhdrType;

typedef struct _AEEBTSMLObject
{
  AECHAR*         pwName;
  AEEBTObjectType objType;
  uint32          objSize;

  uint8                OBEXheadersCount;
  AEEBTSMLOBEXhdrType* pOBEXheaders;

  // For authentication
  byte            userID[ AEEBT_MAX_USERID_LEN ];
  uint8           userIDlen;
} AEEBTSMLObject;

typedef struct _AEEBTHIDObject
{ 
  uint8           uResult;
  uint16          hidHandle; 
  AEEBTBDAddr     bdAddr;

}AEEBTHIDObject;

// CTP/ICP Call Clear cause types
enum
{
  AEEBT_TCS_CAUSE_NONE                               =   0,
  AEEBT_TCS_CAUSE_UNALLOCATED_NUMBER                 =   1,
  AEEBT_TCS_CAUSE_NO_ROUTE_TO_DESTINATION            =   3,
  AEEBT_TCS_CAUSE_NORMAL_CALL_CLEARING               =  16,
  AEEBT_TCS_CAUSE_USER_BUSY                          =  17,
  AEEBT_TCS_CAUSE_NO_USER_RESPONDING                 =  18,
  AEEBT_TCS_CAUSE_NO_ANSWER_FROM_USER                =  19,
  AEEBT_TCS_CAUSE_CALL_REJECTED                      =  21,
  AEEBT_TCS_CAUSE_NUMBER_CHANGED                     =  22,
  AEEBT_TCS_CAUSE_NONSELECTED_USER_CLEARING          =  26,
  AEEBT_TCS_CAUSE_INVALID_NUMBER_FORMAT              =  28,
  AEEBT_TCS_CAUSE_FACILITY_REJECTED                  =  29,
  AEEBT_TCS_CAUSE_NO_CIRCUIT_AVAILABLE               =  34,
  AEEBT_TCS_CAUSE_TEMPORARY_FAILURE                  =  41,
  AEEBT_TCS_CAUSE_REQUESTED_CIRCUIT_NOT_AVAILABLE    =  44,
  AEEBT_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE      =  58,
  AEEBT_TCS_CAUSE_BEARER_CAPABILITY_NOT_IMPLEMENTED  =  65,
  AEEBT_TCS_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED =  69,
  AEEBT_TCS_CAUSE_MANDATORY_INFORMATION_ELEM_MISSING =  96,
  AEEBT_TCS_CAUSE_INVALID_INFORMATION_ELEM_CONTENTS  = 100,
  AEEBT_TCS_CAUSE_RECOVERY_ON_TIMER_EXPIRY           = 102
};
typedef uint16 AEEBTTCSCauseType;

// CTP/ICP Call Status type
enum
{
  AEEBT_TCS_CALL_STATUS_OVERLAP_RECEIVING            = 0,
  AEEBT_TCS_CALL_STATUS_PROCEEDING                   = 1,
  AEEBT_TCS_CALL_STATUS_ALERTING                     = 2
};
typedef uint8 AEEBTTCSCallStatusType;

// CTP/ICP number type
enum
{
  AEEBT_TCS_NUMBER_TYPE_UNKNOWN                    = 0x0000,
  AEEBT_TCS_NUMBER_TYPE_INTERNATIONAL              = 0x0010,
  AEEBT_TCS_NUMBER_TYPE_NATIONAL                   = 0x0020,
  AEEBT_TCS_NUMBER_TYPE_NETWORK                    = 0x0010 | 0x0020,
  AEEBT_TCS_NUMBER_TYPE_SUBSCRIBER                 = 0x0040,
  AEEBT_TCS_NUMBER_TYPE_ABBREVIATED                = 0x0040 | 0x0020,
  AEEBT_TCS_NUMBER_TYPE_RESERVED_FOR_EXTENSION     = 0x0040 | 0x0020 | 0x0010
};
typedef uint16 AEEBTTCSNumberTypeType;

// CTP/ICP number plan type
enum
{
  AEEBT_TCS_NUMBER_PLAN_UNKNOWN                      = 0x0000,
  AEEBT_TCS_NUMBER_PLAN_ISDN                         = 0x0001,
  AEEBT_TCS_NUMBER_PLAN_X121                         = 0x0001 | 0x0002,
  AEEBT_TCS_NUMBER_PLAN_RESERVED                     = 0x0004,
  AEEBT_TCS_NUMBER_PLAN_NATIONAL                     = 0x0008,
  AEEBT_TCS_NUMBER_PLAN_PRIVATE                      = 0x0008 | 0x0001
};
typedef uint16 AEEBTTCSNumberPlanType;

// CTP/ICP presentation type
enum
{
  AEEBT_TCS_PRESENTATION_ALLOWED                     = 0x0000,
  AEEBT_TCS_PRESENTATION_RESTRICTED                  = 0x0020,
  AEEBT_TCS_PRESENTATION_NUM_NOT_AVAILABLE           = 0x0040,
  AEEBT_TCS_PRESENTATION_RESERVED                    = 0x0020 | 0x0040
};
typedef uint16 AEEBTTCSPresentationType;

// CTP/ICP screening type
enum
{
  AEEBT_TCS_SCREENING_USER_NOT_SCREENED              = 0x0000,
  AEEBT_TCS_SCREENING_USER_PASSED                    = 0x0001,
  AEEBT_TCS_SCREENING_USER_FAILED                    = 0x0002,
  AEEBT_TCS_SCREENING_NETWORK                        = 0x0001 | 0x0002
};
typedef uint16 AEEBTTCSScreeningType;

// CTP/ICP audio control type
enum
{
  AEEBT_TCS_AUDIO_CONTROL_VOLUME_INCREASE            = 0,
  AEEBT_TCS_AUDIO_CONTROL_VOLUME_DECREASE            = 1,
  AEEBT_TCS_AUDIO_CONTROL_MIC_GAIN_INCREASE          = 2,
  AEEBT_TCS_AUDIO_CONTROL_MIC_GAIN_DECREASE          = 3,
  AEEBT_TCS_AUDIO_CONTROL_NONE                       = 4
};
typedef uint8 AEEBTTCSAudioControlType;

#define AEEBT_TCS_MAX_DIGITS                          20

// CTP/ICP Called/Calling number type
typedef PACKED struct
{
  boolean                     bCalling;
  AEEBTTCSNumberTypeType      type;
  AEEBTTCSNumberPlanType      plan;
  AEEBTTCSPresentationType    presentation;
  AEEBTTCSScreeningType       screening;
  char                        digits[AEEBT_TCS_MAX_DIGITS];
  char                        numDigits;
} GCCPACKED AEEBTTCSNumberType;

typedef AEEBTTCSCauseType              AEEBTCTPCauseType;
typedef AEEBTTCSCallStatusType         AEEBTCTPCallStatusType;
typedef AEEBTTCSNumberType             AEEBTCTPNumberType;
typedef AEEBTTCSAudioControlType       AEEBTCTPAudioControlType;

// object types
typedef struct _AEEBTCTPObject
{
  AECHAR*         pwName;
  AEEBTObjectType objType;
  uint32          objSize;

  // Call info related
  boolean                  bSendingCmplt;
  char                     keypadFacility;
  AEEBTCTPNumberType       callingNumber;
  AEEBTCTPNumberType       calledNumber;
  AEEBTCTPAudioControlType audioControl;

  // Call release info
  AEEBTCTPCauseType        cause;

  // Outgoing Call status
  AEEBTCTPCallStatusType   status;

  // DTMF tone
  char                     tone;

} AEEBTCTPObject;

// ICP
typedef AEEBTTCSCauseType              AEEBTICPCauseType;
typedef AEEBTTCSCallStatusType         AEEBTICPCallStatusType;
typedef AEEBTTCSNumberType             AEEBTICPNumberType;

// object types
typedef struct _AEEBTICPObject
{
  AECHAR*         pwName;
  AEEBTObjectType objType;
  uint32          objSize;

  // Call release info
  AEEBTICPCauseType        cause;

  // Outgoing Call status
  AEEBTICPCallStatusType   status;
} AEEBTICPObject;

typedef struct
{
  AEEBTBDAddr     bdAddr;
  AEEBTError      error;
  uint16          uMaxMsgSize;
} AEEBTSAPConnStatus;

typedef struct
{
  uint16          uResult;
  uint8           uStatus;
} AEEBTSAPCardReaderStatus;

// AVRCP
/* See AVRCP 1.0, section 4.7.5 to see which commands are
 * required to be supported by each category */
#define AEEBT_AVRCP_CATEGORY1 0x01 /* Player / Recorder */
#define AEEBT_AVRCP_CATEGORY2 0x02 /* Monitor / Amplifier */
#define AEEBT_AVRCP_CATEGORY3 0x04 /* Tuner */
#define AEEBT_AVRCP_CATEGORY4 0x08 /* Menu */
typedef uint8 AEEBTAVRCPCatType;

#define AEEBT_AVRCP_MAX_OPERANDS 16

#define AEEBT_AVRCP_RESPONSE_NOT_IMPLEMENTED    0x8
#define AEEBT_AVRCP_RESPONSE_ACCEPTED           0x9
#define AEEBT_AVRCP_RESPONSE_REJECTED           0xa
#define AEEBT_AVRCP_RESPONSE_IN_TRANSITION      0xb
#define AEEBT_AVRCP_RESPONSE_IMPLEMENTED        0xc
#define AEEBT_AVRCP_RESPONSE_STABLE             0xc
#define AEEBT_AVRCP_RESPONSE_CHANGED            0xd
typedef uint8 AEEBTAVRCPRspType;

#define AEEBT_AVRCP_OP_PRESSED  0
#define AEEBT_AVRCP_OP_RELEASED 1
typedef uint8 AEEBTAVRCPOpStatusType;

typedef PACKED struct {
  AEEBTAVRCPOpStatusType opStatus;
  uint8                  txnId;
} GCCPACKED AEEBTAVRCPOpType;

typedef PACKED struct {
  uint8                      txn_id;
  uint8                      cmd;
  uint8                      subunit_type;
  uint8                      subunit_id;
  uint8                      opcode;
  uint8                      operand_count;
} GCCPACKED AEEBTAVRCPGenericOpType;

// AVRCP event types
enum {
  AEEBT_AVRCP_EVT_CONNECTED,
  AEEBT_AVRCP_EVT_CON_FAILED,
  AEEBT_AVRCP_EVT_DISCONNECTED,
  AEEBT_AVRCP_EVT_TARGET_COMMAND,
  AEEBT_AVRCP_EVT_OP_SELECT,
  AEEBT_AVRCP_EVT_OP_UP,
  AEEBT_AVRCP_EVT_OP_DOWN,
  AEEBT_AVRCP_EVT_OP_LEFT,
  AEEBT_AVRCP_EVT_OP_RIGHT,
  AEEBT_AVRCP_EVT_OP_RIGHT_UP,
  AEEBT_AVRCP_EVT_OP_RIGHT_DOWN,
  AEEBT_AVRCP_EVT_OP_LEFT_UP,
  AEEBT_AVRCP_EVT_OP_LEFT_DOWN,
  AEEBT_AVRCP_EVT_OP_ROOT_MENU,
  AEEBT_AVRCP_EVT_OP_SETUP_MENU,
  AEEBT_AVRCP_EVT_OP_CONTENTS_MENU,
  AEEBT_AVRCP_EVT_OP_FAVORITE_MENU,
  AEEBT_AVRCP_EVT_OP_EXIT,
  AEEBT_AVRCP_EVT_OP_0,
  AEEBT_AVRCP_EVT_OP_1,
  AEEBT_AVRCP_EVT_OP_2,
  AEEBT_AVRCP_EVT_OP_3,
  AEEBT_AVRCP_EVT_OP_4,
  AEEBT_AVRCP_EVT_OP_5,
  AEEBT_AVRCP_EVT_OP_6,
  AEEBT_AVRCP_EVT_OP_7,
  AEEBT_AVRCP_EVT_OP_8,
  AEEBT_AVRCP_EVT_OP_9,
  AEEBT_AVRCP_EVT_OP_DOT,
  AEEBT_AVRCP_EVT_OP_ENTER,
  AEEBT_AVRCP_EVT_OP_CLEAR,
  AEEBT_AVRCP_EVT_OP_CHANNEL_UP,
  AEEBT_AVRCP_EVT_OP_CHANNEL_DOWN,
  AEEBT_AVRCP_EVT_OP_PREVIOUS_CHANNEL,
  AEEBT_AVRCP_EVT_OP_SOUND_SELECT,
  AEEBT_AVRCP_EVT_OP_INPUT_SELECT,
  AEEBT_AVRCP_EVT_OP_DISPLAY_INFORMATION,
  AEEBT_AVRCP_EVT_OP_HELP,
  AEEBT_AVRCP_EVT_OP_PAGE_UP,
  AEEBT_AVRCP_EVT_OP_PAGE_DOWN,
  AEEBT_AVRCP_EVT_OP_POWER,
  AEEBT_AVRCP_EVT_OP_VOLUME_UP,
  AEEBT_AVRCP_EVT_OP_VOLUME_DOWN,
  AEEBT_AVRCP_EVT_OP_MUTE,
  AEEBT_AVRCP_EVT_OP_PLAY,
  AEEBT_AVRCP_EVT_OP_STOP,
  AEEBT_AVRCP_EVT_OP_PAUSE,
  AEEBT_AVRCP_EVT_OP_RECORD,
  AEEBT_AVRCP_EVT_OP_REWIND,
  AEEBT_AVRCP_EVT_OP_FAST_FORWARD,
  AEEBT_AVRCP_EVT_OP_EJECT,
  AEEBT_AVRCP_EVT_OP_FORWARD,
  AEEBT_AVRCP_EVT_OP_BACKWARD,
  AEEBT_AVRCP_EVT_OP_ANGLE,
  AEEBT_AVRCP_EVT_OP_SUBPICTURE,
  AEEBT_AVRCP_EVT_OP_F1,
  AEEBT_AVRCP_EVT_OP_F2,
  AEEBT_AVRCP_EVT_OP_F3,
  AEEBT_AVRCP_EVT_OP_F4,
  AEEBT_AVRCP_EVT_OP_F5,
  AEEBT_AVRCP_EVT_OP_GENERIC
};
typedef uint8 AEEBTAVRCPEventType;

// PBAP

enum
{
  AEEBT_PBAP_AUTH_NONE,
  AEEBT_PBAP_AUTH_PIN,
  AEEBT_PBAP_AUTH_USERID_AND_PIN
};
typedef uint8 AEEBTPBAPAuthentication;

// PCE/PSE Supported Features
enum
{
  AEEBT_PBAP_FEATURE_DOWNLOADING              = 0x01,
  AEEBT_PBAP_FEATURE_BROWSING                 = 0x02,
  AEEBT_PBAP_FEATURE_DOWNLOADING_AND_BROWSING = 0x03
};
typedef uint8  AEEBTPBAPFeatures;

// PSE Supported Repositories
enum
{
  AEEBT_PBAP_REPOSITORY_LOCAL                 = 0x01,
  AEEBT_PBAP_REPOSITORY_SIM_CARD              = 0x02,
  AEEBT_PBAP_REPOSITORY_LOCAL_AND_SIM_CARD    = 0x03,
  AEEBT_PBAP_REPOSITORY_INVALID               = 0x04
};
typedef uint8  AEEBTPBAPRepositories;

// vCard Attribute Mask
enum
{
  AEEBT_PBAP_ATTRIBUTE_VERSION            = 0x00000001,
  AEEBT_PBAP_ATTRIBUTE_FN                 = 0x00000002,
  AEEBT_PBAP_ATTRIBUTE_N                  = 0x00000004,
  AEEBT_PBAP_ATTRIBUTE_PHOTO              = 0x00000008,
  AEEBT_PBAP_ATTRIBUTE_BDAY               = 0x00000010,
  AEEBT_PBAP_ATTRIBUTE_ADR                = 0x00000020,
  AEEBT_PBAP_ATTRIBUTE_LABEL              = 0x00000040,
  AEEBT_PBAP_ATTRIBUTE_TEL                = 0x00000080,
  AEEBT_PBAP_ATTRIBUTE_EMAIL              = 0x00000100,
  AEEBT_PBAP_ATTRIBUTE_MAILER             = 0x00000200,
  AEEBT_PBAP_ATTRIBUTE_TZ                 = 0x00000400,
  AEEBT_PBAP_ATTRIBUTE_GEO                = 0x00000800,
  AEEBT_PBAP_ATTRIBUTE_TITLE              = 0x00001000,
  AEEBT_PBAP_ATTRIBUTE_ROLE               = 0x00002000,
  AEEBT_PBAP_ATTRIBUTE_LOGO               = 0x00004000,
  AEEBT_PBAP_ATTRIBUTE_AGENT              = 0x00008000,
  AEEBT_PBAP_ATTRIBUTE_ORG                = 0x00010000,
  AEEBT_PBAP_ATTRIBUTE_NOTE               = 0x00020000,

  AEEBT_PBAP_ATTRIBUTE_REV                = 0x00040000,
  AEEBT_PBAP_ATTRIBUTE_SOUND              = 0x00080000,
  AEEBT_PBAP_ATTRIBUTE_URL                = 0x00100000,
  AEEBT_PBAP_ATTRIBUTE_UID                = 0x00200000,
  AEEBT_PBAP_ATTRIBUTE_KEY                = 0x00400000,
  AEEBT_PBAP_ATTRIBUTE_NICKNAME           = 0x00800000,
  AEEBT_PBAP_ATTRIBUTE_CATEGORIES         = 0x01000000,
  AEEBT_PBAP_ATTRIBUTE_PROID              = 0x02000000,
  AEEBT_PBAP_ATTRIBUTE_CLASS              = 0x04000000,
  AEEBT_PBAP_ATTRIBUTE_SORT_STRING        = 0x08000000,
  AEEBT_PBAP_ATTRIBUTE_TIMESTAMP          = 0x10000000
};
typedef uint64 AEEBTPBAPAttribMask;

// vCard Formats
enum
{
  AEEBT_PBAP_FORMAT_VCARD_2_1             = 0x00,
  AEEBT_PBAP_FORMAT_VCARD_3_0             = 0x01
};
typedef uint8 AEEBTPBAPFormat;

// Search Order
enum
{
  AEEBT_PBAP_SORT_ORDER_INDEXED           = 0x00,
  AEEBT_PBAP_SORT_ORDER_ALPHANUMERIC      = 0x01,
  AEEBT_PBAP_SORT_ORDER_PHONETIC          = 0x02
};
typedef uint8 AEEBTPBAPOrder;

enum
{
  AEEBT_PBAP_SET_PATH_TO_FOLDER,
  AEEBT_PBAP_SET_PATH_TO_PARENT,
  AEEBT_PBAP_SET_PATH_TO_ROOT
};
typedef uint8 AEEBTPBAPSetPathFlag;

// Object Types
#define AEEBT_PBAP_OBJECT_NAME_LOCAL_PB       "telecom/pb.vcf"
#define AEEBT_PBAP_OBJECT_NAME_LOCAL_ICH      "telecom/ich.vcf"
#define AEEBT_PBAP_OBJECT_NAME_LOCAL_OCH      "telecom/och.vcf"
#define AEEBT_PBAP_OBJECT_NAME_LOCAL_MCH      "telecom/mch.vcf"
#define AEEBT_PBAP_OBJECT_NAME_LOCAL_CCH      "telecom/cch.vcf"

#define AEEBT_PBAP_OBJECT_NAME_SIM_PB         "SIM1/telecom/pb.vcf"
#define AEEBT_PBAP_OBJECT_NAME_SIM_ICH        "SIM1/telecom/ich.vcf"
#define AEEBT_PBAP_OBJECT_NAME_SIM_OCH        "SIM1/telecom/och.vcf"
#define AEEBT_PBAP_OBJECT_NAME_SIM_MCH        "SIM1/telecom/mch.vcf"
#define AEEBT_PBAP_OBJECT_NAME_SIM_CCH        "SIM1/telecom/cch.vcf"

// Folder Types
#define AEEBT_PBAP_FOLDER_NAME_ROOT           "root"
#define AEEBT_PBAP_FOLDER_NAME_SIM1           "SIM1"
#define AEEBT_PBAP_FOLDER_NAME_TELECOM        "telecom"
#define AEEBT_PBAP_FOLDER_NAME_PB             "pb"
#define AEEBT_PBAP_FOLDER_NAME_ICH            "ich"
#define AEEBT_PBAP_FOLDER_NAME_OCH            "och"
#define AEEBT_PBAP_FOLDER_NAME_MCH            "mch"
#define AEEBT_PBAP_FOLDER_NAME_CCH            "cch"

// Name of vcard-listings object xml file
#define AEEBT_PBAP_VCARD_LISTING_XML_FILE     "vcard.xml"

// Search Attribute Types
enum {
  AEEBT_PBAP_SEARCH_ATTRIBUTE_NAME,
  AEEBT_PBAP_SEARCH_ATTRIBUTE_NUMBER,
  AEEBT_PBAP_SEARCH_ATTRIBUTE_SOUND
};
typedef uint8 AEEBTPBAPSearchAttrib;

typedef struct
{
  uint16           AEEBTPBAPdataLen;
  byte*            AEEBTPBAPdata;
} AEEBTPBAPbyteSeqType;

typedef struct _AEEBTPBAPObject
{
  // Name of phonebook object to pull (can be relative or
  // absolute depending upon whether it is Download or
  // Browse request)
  AECHAR                    pwObjectName[AEEBT_MAX_FILE_NAME + 1];

  // Requested attributes in the vCard object
  uint64                    filter;

  AEEBTPBAPFormat           format;
  uint16                    maxListCount;
  uint16                    listStartOffset;

  // Phonebook folder name
  AECHAR*                   pwFolderName;

  AEEBTPBAPOrder            searchOrder;
  AECHAR                    searchValue[AEEBT_MAX_MIME_TYPE_LEN];
  AEEBTPBAPSearchAttrib     searchAttrib;

  // Vcard or XML string
  char*                     pObject;

  uint16                    phoneBookSize;
  uint8                     newMissedCalls;

  // SetPhoneBook params
  AECHAR                    wName[ AEEBT_MAX_FILE_NAME + 1 ];
  AEEBTPBAPSetPathFlag      flag;

  // Authentication related
  AEEBTBDAddr               bdAddr;
  boolean                   bUserIDRequired;
  AECHAR                    wUserID[AEEBT_MAX_USERID_LEN];
} AEEBTPBAPObject;

/* Sound AVS is about to play */
#define AEEBT_SND_NONE            0x00  /* no audio                   */
#define AEEBT_SND_OTHER           0x01  /* non BT support audio       */
#define AEEBT_SND_BT_SCO          0x02  /* BT SCO supported audio     */
#define AEEBT_SND_BT_OTHER        0x04  /* BT non-SCO supported audio */
#define AEEBT_SND_DTMF_START      0x08  /* started playing DTMF tones */
#define AEEBT_SND_DTMF_STOP       0x10  /* stopped playing DTMF tones */
#define AEEBT_SND_OTHER_STOP      0x20  /* non BT support audio       */
#define AEEBT_SND_DTMF_RING_START 0x40  /* DTMF Ringer Start          */
#define AEEBT_SND_DTMF_RING_STOP  0x80  /* DTMF Ringer Stop           */

typedef uint32 AEEBTSoundType;

typedef union
{
  uint16                    uError;
  uint8                     uResult;
  uint8                     uStatus;

  AEEBTReason               reason;
  AEEBTBDAddr               bdAddr;

  /* AG specific */
  uint16                    uVolumeGain;
  uint8                     uDTMFChar;
  AEEBTSoundType            sSoundType;
  
  /* RM specific */
  AEEBTLinkStatus*          pLinkStatus;
  AEEBTDeviceUpdateStatus*  pDevUpdateStatus;
  AEEBTAuthorizeReq*        pAuthorizeReq;
  AEEBTKeypressType         keypress;
  AEEBTPasskeyEvent         sPasskey;
  AEEBTBondedEvent          sBonded;

  /* SDP specific */
  uint16                    uNumDevicesDiscovered;
  uint16                    uNumSvcRecsFound;
  AECHAR*                   pDevName;
  AEEBTDevDiscoveryComplete sDevDiscComplete;
  AEEBTDiscoverableModeResp sDiscoverableModeResp;
  uint32                    uSvcRecHandle;

  /* SPP specific */
  uint16                    uID;
  AEEBTSppStatus*           pSppStatus;
  AEEBTSppConfig*           pSppConfig;
  AEEBTModemStatus*         pModemStatus;
  AEEBTLineError*           pLineError;

  /* L2 specific */
  AEEBTL2Connected            sL2Connected;
  AEEBTL2ConnectFailed        sL2ConnectFailed;
  AEEBTL2Disconnected         sL2Disconnected;
  AEEBTL2Reconfigured         sL2Reconfigured;
  AEEBTL2PingRsp              sL2PingRsp;

  /* specific to OBEX profiles */
  AEEBTProgressInfo         ProgressInfo;

  /* OPP specific */
  AEEBTOPPObject            OppObject;

  /* FTP specific */
  AEEBTFTPConnRequest       ConnReq;
  AEEBTFTPAuthRequest       AuthReq;
  AEEBTFTPSetPathRequest    SetPathReq;
  AEEBTFTPAccessRequest     AccessReq;
  boolean                   bReadOnly;

  /* BIP specific */
  AEEBTBIPObject            BipObject;

  /* BPP specific */
  AEEBTBPPObject            BppObject;

  /* SML specific */
  AEEBTSMLObject            SmlObject;

  /* CTP specific */
  AEEBTCTPObject            CtpObject;

  /* ICP specific */
  AEEBTICPObject            IcpObject;

  /* SAP specific */
  AEEBTSAPConnStatus        SapConnStatus;
  AEEBTSAPCardReaderStatus  CardReaderStatus;
  boolean                   bDiscImmediately;

  /* AV specific */
  uint8                     A2DPStartInit;
  uint8                     A2DPSuspendInit;
  uint8                     A2DPOpenInit;
  uint8                     A2DPCloseInit;
  uint8                     A2DPBitrate;
  AEEBTAVRCPOpType          AvrcpOp;
  AEEBTAVRCPGenericOpType   AvrcpGenericOp;

  /* PBAP specific */
  AEEBTPBAPObject           PBAPObject;

  /* HID specific */
  AEEBTHIDObject            HIDObject;

} NotificationData;

struct _AEEBTNotification
{
  AEEBTNotification*  pNext;
  uint32              uID;
  NotificationData    data;
};

#ifdef __cplusplus
}
#endif
/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEBTBDAddr

Description:
   AEEBTBDAddr defines the Bluetooth address of the device.

Definition:
   typedef PACKED struct tagAEEBTBDAddr
   {
     char      caddr[6];
   } GCCPACKED AEEBTBDAddr;

Members:

   caddr - Bluetooth address array


Comments:
   None.


See Also:
   None

=======================================================================

AEEBTCTPNumberType

Description:
   This enumeration defines the the type of call.

Definition:
   enum
   {
     AEEBT_TCS_NUMBER_TYPE_UNKNOWN                    = 0x0000,
     AEEBT_TCS_NUMBER_TYPE_INTERNATIONAL              = 0x0010,
     AEEBT_TCS_NUMBER_TYPE_NATIONAL                   = 0x0020,
     AEEBT_TCS_NUMBER_TYPE_NETWORK                    = 0x0010 | 0x0020,
     AEEBT_TCS_NUMBER_TYPE_SUBSCRIBER                 = 0x0040,
     AEEBT_TCS_NUMBER_TYPE_ABBREVIATED                = 0x0040 | 0x0020,
     AEEBT_TCS_NUMBER_TYPE_RESERVED_FOR_EXTENSION     = 0x0040 | 0x0020 | 0x0010
   };
   typedef uint16 AEEBTTCSNumberTypeType;

Members:

Comments:
   None.

See Also:
   None

=======================================================================

AEEBTCTPAudioControlType

Description:
   This enumeration defines the type of control command.

Definition:
   enum
   {
     AEEBT_TCS_AUDIO_CONTROL_VOLUME_INCREASE            = 0,
     AEEBT_TCS_AUDIO_CONTROL_VOLUME_DECREASE            = 1,
     AEEBT_TCS_AUDIO_CONTROL_MIC_GAIN_INCREASE          = 2,
     AEEBT_TCS_AUDIO_CONTROL_MIC_GAIN_DECREASE          = 3,
     AEEBT_TCS_AUDIO_CONTROL_NONE                       = 4
   };
   typedef uint8 AEEBTTCSAudioControlType;

Members:

Comments:
   None.

See Also:
   None

=======================================================================

AEEBTICPCauseType

Description:
   This enumeration describes the cause (or reason) for clearing the call.

Definition:
   enum
   {
     AEEBT_TCS_CAUSE_NONE                               =   0,
     AEEBT_TCS_CAUSE_UNALLOCATED_NUMBER                 =   1,
     AEEBT_TCS_CAUSE_NO_ROUTE_TO_DESTINATION            =   3,
     AEEBT_TCS_CAUSE_NORMAL_CALL_CLEARING               =  16,
     AEEBT_TCS_CAUSE_USER_BUSY                          =  17,
     AEEBT_TCS_CAUSE_NO_USER_RESPONDING                 =  18,
     AEEBT_TCS_CAUSE_NO_ANSWER_FROM_USER                =  19,
     AEEBT_TCS_CAUSE_CALL_REJECTED                      =  21,
     AEEBT_TCS_CAUSE_NUMBER_CHANGED                     =  22,
     AEEBT_TCS_CAUSE_NONSELECTED_USER_CLEARING          =  26,
     AEEBT_TCS_CAUSE_INVALID_NUMBER_FORMAT              =  28,
     AEEBT_TCS_CAUSE_FACILITY_REJECTED                  =  29,
     AEEBT_TCS_CAUSE_NO_CIRCUIT_AVAILABLE               =  34,
     AEEBT_TCS_CAUSE_TEMPORARY_FAILURE                  =  41,
     AEEBT_TCS_CAUSE_REQUESTED_CIRCUIT_NOT_AVAILABLE    =  44,
     AEEBT_TCS_CAUSE_BEARER_CAPABILITY_UNAVAILABLE      =  58,
     AEEBT_TCS_CAUSE_BEARER_CAPABILITY_NOT_IMPLEMENTED  =  65,
     AEEBT_TCS_CAUSE_REQUESTED_FACILITY_NOT_IMPLEMENTED =  69,
     AEEBT_TCS_CAUSE_MANDATORY_INFORMATION_ELEM_MISSING =  96,
     AEEBT_TCS_CAUSE_INVALID_INFORMATION_ELEM_CONTENTS  = 100,
     AEEBT_TCS_CAUSE_RECOVERY_ON_TIMER_EXPIRY           = 102
   };
   typedef uint16 AEEBTTCSCauseType;

Members:


Comments:
   None.


See Also:
   None

=======================================================================

AEEBTAVRCPCatType

Description:
   This describes the categories used by AVRCP. Please see the BT AVRCP spec
   for an enumeration of which Pass Through commands must be supported by
   category.

Definition:
   #define AEEBT_AVRCP_CATEGORY1 0x01 // Player / Recorder
   #define AEEBT_AVRCP_CATEGORY2 0x02 // Monitor / Amplifier
   #define AEEBT_AVRCP_CATEGORY3 0x04 // Tuner
   #define AEEBT_AVRCP_CATEGORY4 0x08 // Menu
   typedef uint8 AEEBTAVRCPCatType;

Members:

Comments:
   None.

See Also:
   See AVRCP 1.0, section 4.7.5.

=======================================================================

AEEBTAVRCPGenericOpType

Description:
   This type is used to send and receive AVRCP Vendor Specific Pass Through
   commands, as well as other AVCTP Vendor Unique data.

Definition:
   typedef PACKED struct {
     uint8                      txn_id;
     uint8                      cmd;
     uint8                      subunit_type;
     uint8                      subunit_id;
     uint8                      opcode;
     uint8                      operand_count;
   } GCCPACKED AEEBTAVRCPGenericOpType;

Members:

   txn_id - The AV/C transaction ID. A response should match the command.
   cmd - The AV/C and AVCTP command value. For a response, this should be of the AEEBTAVRCPRspType type.
   subunit_type - The addressed subunit type defined in the AV/C spec.
   subunit_id - The addressed subunit ID.
   opcode - The first byte of AVRCP data.
   operand_count - The number of bytes in the AVRCP payload.
   

Comments:
   None.


See Also:
   IBTEXTAVRCP_GetGenericOp
   IBTEXTAVRCP_SendGenericReply

=======================================================================

AEEBTAVRCPOpStatusType


Description:
   This describes the status of a passthrough command -- either Pressed or Released.


Definition:
   #define AEEBT_AVRCP_OP_PRESSED  0
   #define AEEBT_AVRCP_OP_RELEASED 1
   typedef uint8 AEEBTAVRCPOpStatusType;


Members:


Comments:
   None.


See Also:
   None

=======================================================================

AEEBTAVRCPEventType


Description:
   This is the AVRCP Passthough event type.


Definition:
   enum {
     AEEBT_AVRCP_EVT_CONNECTED,
     AEEBT_AVRCP_EVT_CON_FAILED,
     AEEBT_AVRCP_EVT_DISCONNECTED,
     AEEBT_AVRCP_EVT_TARGET_COMMAND,
     AEEBT_AVRCP_EVT_OP_SELECT,
     AEEBT_AVRCP_EVT_OP_UP,
     AEEBT_AVRCP_EVT_OP_DOWN,
     AEEBT_AVRCP_EVT_OP_LEFT,
     AEEBT_AVRCP_EVT_OP_RIGHT,
     AEEBT_AVRCP_EVT_OP_RIGHT_UP,
     AEEBT_AVRCP_EVT_OP_RIGHT_DOWN,
     AEEBT_AVRCP_EVT_OP_LEFT_UP,
     AEEBT_AVRCP_EVT_OP_LEFT_DOWN,
     AEEBT_AVRCP_EVT_OP_ROOT_MENU,
     AEEBT_AVRCP_EVT_OP_SETUP_MENU,
     AEEBT_AVRCP_EVT_OP_CONTENTS_MENU,
     AEEBT_AVRCP_EVT_OP_FAVORITE_MENU,
     AEEBT_AVRCP_EVT_OP_EXIT,
     AEEBT_AVRCP_EVT_OP_0,
     AEEBT_AVRCP_EVT_OP_1,
     AEEBT_AVRCP_EVT_OP_2,
     AEEBT_AVRCP_EVT_OP_3,
     AEEBT_AVRCP_EVT_OP_4,
     AEEBT_AVRCP_EVT_OP_5,
     AEEBT_AVRCP_EVT_OP_6,
     AEEBT_AVRCP_EVT_OP_7,
     AEEBT_AVRCP_EVT_OP_8,
     AEEBT_AVRCP_EVT_OP_9,
     AEEBT_AVRCP_EVT_OP_DOT,
     AEEBT_AVRCP_EVT_OP_ENTER,
     AEEBT_AVRCP_EVT_OP_CLEAR,
     AEEBT_AVRCP_EVT_OP_CHANNEL_UP,
     AEEBT_AVRCP_EVT_OP_CHANNEL_DOWN,
     AEEBT_AVRCP_EVT_OP_PREVIOUS_CHANNEL,
     AEEBT_AVRCP_EVT_OP_SOUND_SELECT,
     AEEBT_AVRCP_EVT_OP_INPUT_SELECT,
     AEEBT_AVRCP_EVT_OP_DISPLAY_INFORMATION,
     AEEBT_AVRCP_EVT_OP_HELP,
     AEEBT_AVRCP_EVT_OP_PAGE_UP,
     AEEBT_AVRCP_EVT_OP_PAGE_DOWN,
     AEEBT_AVRCP_EVT_OP_POWER,
     AEEBT_AVRCP_EVT_OP_VOLUME_UP,
     AEEBT_AVRCP_EVT_OP_VOLUME_DOWN,
     AEEBT_AVRCP_EVT_OP_MUTE,
     AEEBT_AVRCP_EVT_OP_PLAY,
     AEEBT_AVRCP_EVT_OP_STOP,
     AEEBT_AVRCP_EVT_OP_PAUSE,
     AEEBT_AVRCP_EVT_OP_RECORD,
     AEEBT_AVRCP_EVT_OP_REWIND,
     AEEBT_AVRCP_EVT_OP_FAST_FORWARD,
     AEEBT_AVRCP_EVT_OP_EJECT,
     AEEBT_AVRCP_EVT_OP_FORWARD,
     AEEBT_AVRCP_EVT_OP_BACKWARD,
     AEEBT_AVRCP_EVT_OP_ANGLE,
     AEEBT_AVRCP_EVT_OP_SUBPICTURE,
     AEEBT_AVRCP_EVT_OP_F1,
     AEEBT_AVRCP_EVT_OP_F2,
     AEEBT_AVRCP_EVT_OP_F3,
     AEEBT_AVRCP_EVT_OP_F4,
     AEEBT_AVRCP_EVT_OP_F5,
     AEEBT_AVRCP_EVT_OP_GENERIC
   };
   typedef uint8 AEEBTAVRCPEventType;


Members:


Comments:
   None.


See Also:
   None

=======================================================================

AEEBTAVRCPRspType


Description:
   This is the response code sent to the CT for a passthrough command.


Definition:
   #define AEEBT_AVRCP_RESPONSE_NOT_IMPLEMENTED    0x8
   #define AEEBT_AVRCP_RESPONSE_ACCEPTED           0x9
   #define AEEBT_AVRCP_RESPONSE_REJECTED           0xa
   #define AEEBT_AVRCP_RESPONSE_IN_TRANSITION      0xb
   #define AEEBT_AVRCP_RESPONSE_IMPLEMENTED        0xc
   #define AEEBT_AVRCP_RESPONSE_STABLE             0xc
   #define AEEBT_AVRCP_RESPONSE_CHANGED            0xd
   typedef uint8 AEEBTAVRCPRspType;


Members:


Comments:
   None.


See Also:
   None

======================================================================= */

/*=====================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name:  IBTExtNotifier

Description: BlueTooth Notifier

  IBTExtNotifier allows applications to register for notifications
  of events occurring in Bluetooth driver.

  Users can register for this notification via ISHELL_RegisterNotify().

  The defined notification masks are:
  NMASK_BT_AG       notification of AG events
  NMASK_BT_NA       notification of NA events
  NMASK_BT_RM       notification of RM events
  NMASK_BT_SD       notification of SD events
  NMASK_BT_SPP      notification of SPP events
  NMASK_BT_L2       notification of L2 events

  For example, to register to receive all AG and NA events:

  ISHELL_RegisterNotify( pShell, myClsID, AEECLSID_BLUETOOTH_NOTIFIER,
                         NMASK_BT_AG | NMASK_BT_AG | 0xFFFF0000 );

  When handling EVT_NOTIFY, dwMask field of the AEENotify contains the
  notification mask and the event.  The mask can be extracted by
  GET_NOTIFIER_MASK( dwMask ), and the event by GET_NOTIFIER_VAL( dwMask ).
  The pData field of the AEENotify is a pointer to NotificationData.

======================================================================= */

#endif  //_AEEBTEXT_H_

