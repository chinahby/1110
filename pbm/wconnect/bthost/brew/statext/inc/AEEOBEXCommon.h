#ifndef _AEE_OBEX_COMMON_H
#define _AEE_OBEX_COMMON_H


/*=============================================================================

FILE: AEEOBEXCommon.h

SERVICES: OBEX BREW common data structures

GENERAL DESCRIPTION: This header file contains all definitions shared by IOBEX

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None

(c) COPYRIGHT 2004,2005 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/inc/AEEOBEXCommon.h#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $
  $Change: 815296 $

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
3/21/07    MH      Realm string type added to realm structure.
2/28/07    MH      Added support for server disconnect response functionality
1/29/07    MH      Added support for 128Bit UUID and service name registration
11/28/06   MH      IOBEX Interface documentation update.
10/26/06   MH      Connect request indication supports all header types.
9/19/06    MH      Modified auth conn request to support server authentication
8/25/06    bh      compile errors fixed
8/14/06    bh      BT app changes after obex client auth tests.
7/28/06    bh      modified for obex client authentication
6/23/06    jh      added AEEOBEXConnectCfm and updated AEEOBEXSetPathRequest and
                   AEEOBEXConnRequest to support JSR82
3/24/04    sp      corrected featurization so that header file is not featurized

=============================================================================*/


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
#define AEE_OBEX_MAX_USERID_LEN              20
//see services/efs/fs_sys_types.h - FS_PATH_MAX (==128) and FS_NAME_MAX (==80)
#define AEE_OBEX_MAX_FILE_NAME               208

#define AEEOBEX_NO_CONN_ID                   0xFFFF

#define AEE_OBEX_MAX_REALM_LEN               255
    
/* SD service name maximum length */
#define AEE_OBEXSERVER_MAX_SERVICENAME_LEN   32

//model notification queue size per OEMOBEX object
#define OBEX_MAX_PENDING_EVENTS             3
#define AEEOBEX_MIMETYPE_MAX_STRLEN         128

/* MIME types used by OBEX */
#define AEE_OBEX_FOLDER_LISTING_TYPE             "x-obex/folder-listing" /* "x-obex/folder-listing" */
#define AEE_OBEX_ICALENDAR_TYPE                  "text/calendar"         /* "text/calendar */
#define AEE_OBEX_VCALENDAR_TYPE                  "text/x-vcalendar"      /* text/x-vcalendar */
#define AEE_OBEX_VCARD_TYPE                      "text/x-vcard"          /* text/x-vcard */
#define AEE_OBEX_VNOTE_TYPE                      "text/x-vnote"          /* text/x-vnote */
#define AEE_OBEX_VMESSAGE_TYPE                   "text/x-vmsg"           /* text/x-vmsg */
#define AEE_OBEX_UPF_TYPE                        "image/x-UPF"            /* image/x-UPF */
#define AEE_OBEX_JPEG_TYPE                       "image/jpeg"           /* image/jpeg */
#define AEE_OBEX_TEXT_TYPE                       "text/plain"           /* text/plain */

//Transport related stuff

//All the supported transport types
enum _AEEOBEXTransportType
{
    AEE_OBEX_BLUETOOTH_TRANSPORT = 0x01,
    AEE_OBEX_IRDA_TRANSPORT      = 0x02,
    AEE_OBEX_USB_TRANSPORT       = 0x04,
    AEE_OBEX_RS232_TRANSPORT     = 0x08
} ;

typedef uint8 AEEOBEXTransportType;

//a bit mask of transport types typedef uint8 AEEOBEXTransportTypes;

//BT Device Address
typedef PACKED struct
{
    uint8  bd_addr_bytes[ 6 ];  /*  48 bits  */
} AEEOBEX_BT_BD_AddrType;


//contains a bitmap of all supported types - used by an app to register
//with OBEX profile.

typedef uint8 AEEOBEXTransportConfigType;

//The Connection structures are filled in by an OBEX  client where the client
//specifies the transport to use as well as any transport specific parameters that are
//needed to initiate the connection

#define AEEOBEX_INVALID_16BitUUID  0xFFFF

/* Definition for a 16-bit UUID */
typedef uint16 AEEOBEX_BT_SD_UUIDType;

/* Definition for a 128-bit UUID */
typedef struct
{
  uint8            uuid_byte[ 16 ];
} AEEOBEX_BT_SD_UUID128Type;

//BT security flags
enum _AEEOBEXBTSecurityType
{
    AEEOBEX_BT_SEC_NONE,
    AEEOBEX_BT_SEC_AUTHENTICATE,
    AEEOBEX_BT_SEC_AUTHENTICATE_AND_ENCRYPT
} ;
typedef uint8 AEEOBEXBTSecurityType;

//BT connection data to be supplied by client
typedef struct
{
    AEEOBEX_BT_BD_AddrType   *bd_addr_ptr;
    uint8                     channel;
    AEEOBEX_BT_SD_UUIDType    uuid;
    AEEOBEXBTSecurityType     security;
    /* etc */
} AEEOBEXBTConnStructType;

//see bt.hbt_spp_scn_enum_type (BT_SPP_SCN_UNSPECIFIED)
//for client initiated connections, the channel needs to be specified
//using this value
#define BT_CHANNEL_SPP_SCN_UNSPECIFIED  0

//IrDA connection data to be supplied by client
typedef struct
{
    uint32      irda_device_addr;
    const       uint8 *ias_query;
    uint8       ias_query_len;
    /* etc */
} AEEOBEXIrDAConnStructType;

//these are the supported transports the client may initiate a connection on
//TODO: more transports may need to be added (USB, RS232 etc)

typedef union
{
    AEEOBEXBTConnStructType    bt_conn;
    AEEOBEXIrDAConnStructType  irda_conn;
} AEEOBEXTransportConnUnionType;

//This is the struct that the client app supplies while requesting
//a connection. This data type is also used in a few other places.
typedef struct
{
    AEEOBEXTransportType          transport;
    AEEOBEXTransportConnUnionType transport_conn;
} AEEOBEXDeviceAddressType;

//This is the struct that the client app supplies while requesting
//the authentication of the server. It has the information for the
// realm field of the challenge header.
typedef struct
{
    AECHAR realmStr[AEE_OBEX_MAX_REALM_LEN];
    uint8 len;
    uint8 realmStrType ;
}AEEOBEXRealm;


/*
// object types

enum
{
  AEE_OBEX_OPP_ICAL,
  AEE_OBEX_OPP_VCAL,
  AEE_OBEX_OPP_VCARD,
  AEE_OBEX_OPP_VNOTE,
  AEE_OBEX_OPP_VMESSAGE,
  AEE_OBEX_OPP_UNKNOWN_TYPE
};

typedef uint8 AEEOBEXOPPObjectType;
*/

//Forward declaration of Header type structute
typedef struct _AEEOBEXHdrType AEEOBEXHdrType;

typedef struct _AEEOBEXOPPObject
{
  AECHAR              *pwName;
  char                *objType;
  uint32               objSize;
} AEEOBEXOPPObject;

typedef struct _AEEOBEXOPPStatusObject {
   uint32               bytesTransferred;
   uint32               objSize;  //total size of obj being transferred
} AEEOBEXOPPStatusObject;

//data associated with a connection request from a remote client
typedef struct _AEEOBEXConnRequest
{
    uint8               srvChNum;
    uint8               targetId; //target id associated with the connect request
    boolean             bUnauthorized;
    AEEOBEXHdrType     *headerPtr; //this is the actual header
    uint8               headerCount; //the number of headers contained in the headerPtr
    AECHAR              wUserID[ AEE_OBEX_MAX_USERID_LEN ];
} AEEOBEXConnRequestType;

//data associated with a authentication request from a remote client
typedef struct _AEEOBEXAuthConnRequest
{
    uint8           srvChNum;
    uint8           targetId; //target id associated with the connect request
    boolean         bUidReqd;
    boolean         bFullAccess;
    AEEOBEXRealm    realmInfo ;
} AEEOBEXAuthConnRequestType;


typedef struct _OBEXNotificationType
{

    //the sender fills in the class ID of the recipient of the notification
    //The app checks this field to figure out if it should process the notification
    //an EVT_NOTIFY is sent using the mask NMASK_OBEX_GOEP
    //Note that a notification is sent only for CON_REQ to a server app.
    AEECLSID                         classID;
    //this is used to initialize the new IOBEXServer object via IOBEXServer_Initialize()
    uint32                           obexHandle;

    AEEOBEXConnRequestType           ConnReq;
    // New Notfiication event to indicate JSR82 Server APP to indicate client sends the challenge
    // and pass Realm info.
    AEEOBEXAuthConnRequestType       AuthReq;
     //maybe empty  if its not a BT connection
    AEEOBEX_BT_BD_AddrType           bdAddr;
    //either BT or IRDA....when known. Only used by IOBEXOPP
    AEEOBEXTransportConfigType       transportType;
} OBEXNotificationDataType;

//Data sent via EVT_NOTIFY for a CONNECT request
typedef struct _AEEOBEXNotification AEEOBEXNotificationType;

//data associated with an OBEX header
typedef struct
{
    byte*            data;
    uint16           len;

} AEEOBEXByteSeqType;


struct _AEEOBEXNotification
{
    AEEOBEXNotificationType*  pNext;
    //upper 16 bits contain the EVT ID and lower 16 bits contain the mask
    uint32                    uID;
    OBEXNotificationDataType  data;
};

typedef struct
{
    uint16           len;
    uint16*          str;
} AEEOBEXunicodeStrType;

/* Header Types */
enum _HeaderIDType
{
    AEE_OBEX_HDR_COUNT       = 0xC0,   /**< number of objects (used by Connect) */
    AEE_OBEX_HDR_NAME        = 0x01,   /**< name of the object (often a file name) */
    AEE_OBEX_HDR_TYPE        = 0x42,    /**< type of object (e.g., text, HTML, binary, manufacturer-specific) */
    AEE_OBEX_HDR_LENGTH      = 0xC3,  /**< the length of the object in bytes */
    AEE_OBEX_HDR_TIME        = 0x44,    /**< date/time stamp (ISO 8601) */
    AEE_OBEX_HDR_DESCRIPTION = 0x05, /**< text description of the object */
    AEE_OBEX_HDR_TARGET      = 0x46,     /**< name of service to which operation is targeted */
    AEE_OBEX_HDR_HTTP        = 0x47,       /**< HTTP 1.x header */
    AEE_OBEX_HDR_BODY        = 0x48,       /**< a chunk of the object body. */
    AEE_OBEX_HDR_END_OF_BODY = 0x49, /**< the final chunk of the object body */
    AEE_OBEX_HDR_WHO         = 0x4A,         /**< identifies the OBEX application; used to tell if talking to a peer */
    AEE_OBEX_HDR_CONNECTION_ID            = 0xCB, /**< an identifier used for OBEX connection multiplexing */
    AEE_OBEX_HDR_APPLICATION_PARAMS       = 0x4C, /**< extended application request and response information */
    AEE_OBEX_HDR_AUTHENTICATION_CHALLENGE = 0x4D, /**< authentication digest-challenge */
    AEE_OBEX_HDR_AUTHENTICATION_RESPONSE  = 0x4E, /**< authentication digest-response */
    AEE_OBEX_HDR_CREATOR_ID               = 0xCF,    /**< identifies the creator of object */
    AEE_OBEX_HDR_OBJECT_CLASS             = 0x51   /**< OBEX object class of object */
};
typedef uint8 HeaderIDType;

//The actual OBEX hdr
typedef struct _AEEOBEXHdrType
{
    HeaderIDType              id;
    union
    {
        // generic header types
        uint8                  uInt8;
        uint32                 uInt32;
        AEEOBEXByteSeqType    byteseq;
        AEEOBEXunicodeStrType unicode;

        // specific header types
        uint32                 count;
        uint32                 length;
        uint32                 connectionId;
        AEEOBEXByteSeqType    type;
        AEEOBEXByteSeqType    time;
        AEEOBEXByteSeqType    target;
        AEEOBEXByteSeqType    http;
        AEEOBEXByteSeqType    body;
        AEEOBEXByteSeqType    endOfBody;
        AEEOBEXByteSeqType    who;
        AEEOBEXByteSeqType    applicationParams;
        AEEOBEXByteSeqType    authChallenge;
        AEEOBEXByteSeqType    authResponse;
        AEEOBEXByteSeqType    objectClass;
        AEEOBEXunicodeStrType name;
        AEEOBEXunicodeStrType description;
    } val;
} AEEOBEXHdrType;

//Model Notification Data and related stuff


//data associated with a authorization request from a remote host
typedef struct _AEEOBEXAuthRequest
{
    boolean                  bUserIDRequired;
    boolean                  bFullAccess;
} AEEOBEXAuthRequestType;

//data associated with a authorization response from a remote host
typedef struct _AEEOBEXAuthResponse
{
    AECHAR          wUserID[ AEE_OBEX_MAX_USERID_LEN ];
} AEEOBEXAuthResponseType;


//data assoicated with a SETPATH request from a remote client
typedef struct _AEEOBEXSetPathRequest
{
    AECHAR              folderName[ AEE_OBEX_MAX_FILE_NAME ];
    boolean             bCreate;
    boolean             upLevel; //cd up or down
    //this is the actual header
    AEEOBEXHdrType     *headerPtr;
    //the number of headers contained in the headerPtr
    uint8               headerCount;
} AEEOBEXSetPathRequestType;


//data associated with a GET or PUT
typedef struct _AEEOBEXAccessRequest
{
    //this is the actual header
    AEEOBEXHdrType     *headerPtr;
    //the number of headers contained in the headerPtr
    uint8               headerCount;
} AEEOBEXAccessRequestType;

//data associated with Disconnect Request
typedef struct _AEEOBEXDisconnectRequestType
{
    //this is the actual header
    AEEOBEXHdrType     *headerPtr;
    //the number of headers contained in the headerPtr
    uint8               headerCount;
} AEEOBEXDisconnectRequestType;

typedef struct _AEEOBEXConnectCfm
{
    boolean             readOnly;
    //this is the actual header
    AEEOBEXHdrType     *headerPtr;
    //the number of headers contained in the headerPtr
    uint8               headerCount;
} AEEOBEXConnectCfmType;

//This is the data that is sent via the IMODEL_Notify().
//It contains one of the fields from the union.
typedef struct _OBEXModelNotificationData
{
    ModelEvent          base;  //evCode contains the Event Id for the notification. dwParam contains the status code.
    //the event ID indicates the member of the union that is populated
    union
    {
       /*IOBEX (for GOEP) related Data */
        AEEOBEXByteSeqType            targetId;
        AEEOBEXAuthRequestType        authReq;
        AEEOBEXAuthResponseType       authRes;
        AEEOBEXSetPathRequestType     setPathReq;
        AEEOBEXAccessRequestType      accessReq;
        AEEOBEXDisconnectRequestType  discReq;
        AEEOBEXConnectCfmType         connectCfm;

        /* IOBEXOPP (OPP) related Data */
        AEEOBEXOPPObject          OppObject;
        AEEOBEXOPPStatusObject    oppStatusObject;
        //maybe empty  if its not a BT connection
        AEEOBEX_BT_BD_AddrType    bdAddr;

    }u;
} AEEOBEXModelNotificationDataType;


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/




#endif // _AEE_OBEX_COMMON_H
