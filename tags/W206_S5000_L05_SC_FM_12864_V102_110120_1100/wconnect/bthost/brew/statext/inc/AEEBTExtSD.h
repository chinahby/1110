#ifndef _AEE_BT_EXT_SD_H_
#define _AEE_BT_EXT_SD_H_

/*===========================================================================
FILE:      AEEBTExtSD.h

SERVICES:  BlueTooth Service Discovery Interface

GENERAL DESCRIPTION: BREW interface to the BT Service Discovery Server

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtSD

        Copyright © 2003, 2004, 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

// BREW generated ID
#define AEECLSID_BLUETOOTH_SD    	0x0101364c

// max number of responses expected for each device discovery
#define AEEBT_SD_MAX_RSPS                 10

// max number of UUIDs for each service search
#define AEEBT_SD_MAX_SEARCH_UUID          12

// max number of 128-bit UUIDs for each service search
#define AEEBT_SD_MAX_SEARCH_UUID128        1

// max number of attributes for each service search
#define AEEBT_SD_MAX_SEARCH_ATTR_ID       12 

// max number of attributes in each service record
#define AEEBT_SD_MAX_SVC_ATTR_ID          12

// max number of UUIDs in the UUID list in each attribute
#define AEEBT_SD_MAX_UUID                  4

// max number of 128-bit UUIDs in the UUID list in each attribute
#define AEEBT_SD_MAX_UUID128               1

// max number of protocol descriptor in the protocol descriptor list 
// in each attribute
#define AEEBT_SD_MAX_PROTO_DESC            4

// max number of protocol descriptor list in the additional protocol
// descriptor list in each attribute
#define AEEBT_SD_MAX_PROTO_DESC_LISTS      1

// max number of parameters in each protocol descriptor in each attribute
#define AEEBT_SD_MAX_PROTO_PARAM           1

// max number of languages in each language base list in each attribute
#define AEEBT_SD_MAX_LANG_BASE_REC         1

// max number of unsigned integers in each uint list in each attribute
#define AEEBT_SD_MAX_UINT                  8

#define AEEBT_SD_MAX_TEXT_STRING_LEN      32

#define AEEBT_SD_MAX_NUM_OF_HID_CLASS_DESC       1
#define AEEBT_SD_MAX_HID_CLASS_DESC_STRING_LEN  55

// Bluetooth assigned UUIDs for Protocols
#define AEEBT_SD_PROTOCOL_SDP                                0x0001
#define AEEBT_SD_PROTOCOL_UDP                                0x0002
#define AEEBT_SD_PROTOCOL_RFCOMM                             0x0003
#define AEEBT_SD_PROTOCOL_TCP                                0x0004
#define AEEBT_SD_PROTOCOL_TCS_BIN                            0x0005
#define AEEBT_SD_PROTOCOL_TCS_AT                             0x0006
#define AEEBT_SD_PROTOCOL_OBEX                               0x0008
#define AEEBT_SD_PROTOCOL_IP                                 0x0009
#define AEEBT_SD_PROTOCOL_FTP                                0x000A
#define AEEBT_SD_PROTOCOL_HTTP                               0x000C
#define AEEBT_SD_PROTOCOL_WSP                                0x000E
#define AEEBT_SD_PROTOCOL_BNEP                               0x000F
#define AEEBT_SD_PROTOCOL_UPNP                               0x0010
#define AEEBT_SD_PROTOCOL_HIDP                               0x0011
#define AEEBT_SD_PROTOCOL_HARDCOPY_CONTROL_CHANNEL           0x0012
#define AEEBT_SD_PROTOCOL_HARDCOPY_DATA_CHANNEL              0x0014
#define AEEBT_SD_PROTOCOL_HARDCOPY_NOTIFICATION              0x0016
#define AEEBT_SD_PROTOCOL_AVCTP                              0x0017
#define AEEBT_SD_PROTOCOL_AVDTP                              0x0019
#define AEEBT_SD_PROTOCOL_CMTP                               0x001B
#define AEEBT_SD_PROTOCOL_UDI_C_PLANE                        0x001D
#define AEEBT_SD_PROTOCOL_L2CAP                              0x0100

// Bluetooth assigned UUIDs for Service Classes
#define AEEBT_SD_SERVICE_CLASS_SERVICE_DISCOVERY_SERVER      0x1000
#define AEEBT_SD_SERVICE_CLASS_BROWSE_GROUP_DESCRIPTOR       0x1001
#define AEEBT_SD_SERVICE_CLASS_PUBLIC_BROWSE_GROUP           0x1002
#define AEEBT_SD_SERVICE_CLASS_SERIAL_PORT                   0x1101
#define AEEBT_SD_SERVICE_CLASS_LAN_ACCESS_PPP                0x1102
#define AEEBT_SD_SERVICE_CLASS_DIALUP_NETWORKING             0x1103
#define AEEBT_SD_SERVICE_CLASS_IR_MC_SYNC                    0x1104
#define AEEBT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH              0x1105
#define AEEBT_SD_SERVICE_CLASS_OBEX_FILE_TRANSFER            0x1106
#define AEEBT_SD_SERVICE_CLASS_IR_MC_SYNC_COMMAND            0x1107
#define AEEBT_SD_SERVICE_CLASS_HEADSET                       0x1108
#define AEEBT_SD_SERVICE_CLASS_CORDLESS_TELEPHONY            0x1109
#define AEEBT_SD_SERVICE_CLASS_AUDIO_SOURCE                  0x110A
#define AEEBT_SD_SERVICE_CLASS_AUDIO_SINK                    0x110B
#define AEEBT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET      0x110C
#define AEEBT_SD_SERVICE_CLASS_ADVANCED_AUDIO_DISTRIBUTION   0x110D
#define AEEBT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL             0x110E
#define AEEBT_SD_SERVICE_CLASS_VIDEO_CONFERENCING            0x110F
#define AEEBT_SD_SERVICE_CLASS_INTERCOM                      0x1110
#define AEEBT_SD_SERVICE_CLASS_FAX                           0x1111
#define AEEBT_SD_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY         0x1112
#define AEEBT_SD_SERVICE_CLASS_WAP                           0x1113
#define AEEBT_SD_SERVICE_CLASS_WAP_CLIENT                    0x1114
#define AEEBT_SD_SERVICE_CLASS_PANU                          0x1115
#define AEEBT_SD_SERVICE_CLASS_NAP                           0x1116
#define AEEBT_SD_SERVICE_CLASS_GN                            0x1117
#define AEEBT_SD_SERVICE_CLASS_DIRECT_PRINTING               0x1118
#define AEEBT_SD_SERVICE_CLASS_REFERENCE_PRINTING            0x1119
#define AEEBT_SD_SERVICE_CLASS_IMAGING                       0x111A
#define AEEBT_SD_SERVICE_CLASS_IMAGING_RESPONDER             0x111B
#define AEEBT_SD_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE     0x111C
#define AEEBT_SD_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS    0x111D
#define AEEBT_SD_SERVICE_CLASS_HANDSFREE                     0x111E
#define AEEBT_SD_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY       0x111F
#define AEEBT_SD_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE 0x1120
#define AEEBT_SD_SERVICE_CLASS_REFLECTED_UI                  0x1121
#define AEEBT_SD_SERVICE_CLASS_BASIC_PRINTING                0x1122
#define AEEBT_SD_SERVICE_CLASS_PRINTING_STATUS               0x1123
#define AEEBT_SD_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE_SERVICE 0x1124
#define AEEBT_SD_SERVICE_CLASS_HARDCOPY_CABLE_REPLACEMENT    0x1125
#define AEEBT_SD_SERVICE_CLASS_HCR_PRINT                     0x1126
#define AEEBT_SD_SERVICE_CLASS_HCR_SCAN                      0x1127
#define AEEBT_SD_SERVICE_CLASS_COMMON_ISDN_ACCESS            0x1128
#define AEEBT_SD_SERVICE_CLASS_VIDEO_CONFERENCING_GW         0x1129
#define AEEBT_SD_SERVICE_CLASS_UDI_MT                        0x112A
#define AEEBT_SD_SERVICE_CLASS_UDI_TA                        0x112B
#define AEEBT_SD_SERVICE_CLASS_AUDIO_VIDEO                   0x112C
#define AEEBT_SD_SERVICE_CLASS_SIM_ACCESS                    0x112D
#define AEEBT_SD_SERVICE_CLASS_PBAP_PCE                      0x112E
#define AEEBT_SD_SERVICE_CLASS_PBAP_PSE                      0x112F
#define AEEBT_SD_SERVICE_CLASS_PNP_INFORMATION               0x1200
#define AEEBT_SD_SERVICE_CLASS_GENERIC_NETWORKING            0x1201
#define AEEBT_SD_SERVICE_CLASS_GENERIC_FILE_TRANSFER         0x1202
#define AEEBT_SD_SERVICE_CLASS_GENERIC_AUDIO                 0x1203
#define AEEBT_SD_SERVICE_CLASS_GENERIC_TELEPHONY             0x1204
#define AEEBT_SD_SERVICE_CLASS_UPNP_SERVICE                  0x1205
#define AEEBT_SD_SERVICE_CLASS_UPNP_IP_SERVICE               0x1206
#define AEEBT_SD_SERVICE_CLASS_ESDP_UPNP_IP_PAN              0x1300
#define AEEBT_SD_SERVICE_CLASS_ESDP_UPNP_IP_LAP              0x1301
#define AEEBT_SD_SERVICE_CLASS_ESDP_UPNP_L2CAP               0x1302
#define AEEBT_SD_SERVICE_CLASS_VIDEO_SOURCE                  0x1303
#define AEEBT_SD_SERVICE_CLASS_VIDEO_SINK                    0x1304
#define AEEBT_SD_SERVICE_CLASS_VIDEO_DISTRIBUTION            0x1305

#define AEEBT_SD_SERVICE_CLASS_SYNCML_SERVER { \
              0, 0, 0, 0x01, 0, 0, 0x10, 0, 0x80, 0, 0, 0x02, 0xEE, 0, 0, 0x02 }
#define AEEBT_SD_SERVICE_CLASS_SYNCML_CLIENT { \
              0, 0, 0, 0x02, 0, 0, 0x10, 0, 0x80, 0, 0, 0x02, 0xEE, 0, 0, 0x02 }

// Bluetooth assigned numbers for Attribute ID
#define AEEBT_SD_ATTR_ID_SERVICE_RECORD_HANDLE               0x0000
#define AEEBT_SD_ATTR_ID_SERVICE_CLASS_ID_LIST               0x0001
#define AEEBT_SD_ATTR_ID_SERVICE_RECORD_STATE                0x0002
#define AEEBT_SD_ATTR_ID_SERVICE_SERVICE_ID                  0x0003
#define AEEBT_SD_ATTR_ID_PROTOCOL_DESCRIPTOR_LIST            0x0004
#define AEEBT_SD_ATTR_ID_BROWSE_GROUP_LIST                   0x0005
#define AEEBT_SD_ATTR_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST     0x0006
#define AEEBT_SD_ATTR_ID_SERVICE_INFO_TIME_TO_LIVE           0x0007
#define AEEBT_SD_ATTR_ID_SERVICE_AVAILABILITY                0x0008
#define AEEBT_SD_ATTR_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST   0x0009
#define AEEBT_SD_ATTR_ID_DOCUMENTATION_URL                   0x000A
#define AEEBT_SD_ATTR_ID_CLIENT_EXECUTABLE_URL               0x000B
#define AEEBT_SD_ATTR_ID_ICON_URL                            0x000C
#define AEEBT_SD_ATTR_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS 0x000D
#define AEEBT_SD_ATTR_ID_SERVICE_NAME                        0x0100
#define AEEBT_SD_ATTR_ID_SERVICE_DESCRIPTION                 0x0101
#define AEEBT_SD_ATTR_ID_PROVIDER_NAME                       0x0102
#define AEEBT_SD_ATTR_ID_VERSION_NUMBER_LIST                 0x0200
#define AEEBT_SD_ATTR_ID_GROUP_ID                            0x0200
#define AEEBT_SD_ATTR_ID_SERVICE_DATABASE_STATE              0x0201
#define AEEBT_SD_ATTR_ID_HID_DESC_LIST                       0x0206
#define AEEBT_SD_ATTR_ID_SERVICE_VERSION                     0x0300
#define AEEBT_SD_ATTR_ID_NETWORK                             0x0301

// Cordless Telephony Profile
#define AEEBT_SD_ATTR_ID_EXTERNAL_NETWORK                    0x0301

// Synchronization Profile
#define AEEBT_SD_ATTR_ID_SUPPORTED_DATA_STORES_LIST          0x0301

// Generic Access Profile
#define AEEBT_SD_ATTR_ID_REMOTE_AUDIO_VOLUME_CONTROL         0x0302

// Object Push Profile
#define AEEBT_SD_ATTR_ID_SUPPORTED_FORMATS_LIST              0x0303

// Fax Profile
#define AEEBT_SD_ATTR_ID_FAX_CLASS_1_SUPPORT                 0x0302
#define AEEBT_SD_ATTR_ID_FAX_CLASS_2_0_SUPPORT               0x0303
#define AEEBT_SD_ATTR_ID_FAX_CLASS_2_SUPPORT                 0x0304
#define AEEBT_SD_ATTR_ID_AUDIO_FEEDBACK_SUPPORT              0x0305

// BIP
#define AEEBT_SD_ATTR_ID_SUPPORTED_CAPABILITIES              0x0310

// BIP and HFP
#define AEEBT_SD_ATTR_ID_SUPPORTED_FEATURES                  0x0311

// BIP
#define AEEBT_SD_ATTR_ID_SUPPORTED_FUNCTIONS                 0x0312
#define AEEBT_SD_ATTR_ID_TOTAL_IMAGING_DATA_CAPACITY         0x0313

// BPP printer specific attributes
#define AEEBT_SD_ATTR_ID_DOCUMENT_FORMATS_SUPPORTED          0x0350
#define AEEBT_SD_ATTR_ID_CHARACTER_REPERTOIRES_SUPPORTED     0x0352
#define AEEBT_SD_ATTR_ID_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED 0x0354
#define AEEBT_SD_ATTR_ID_COLOR_SUPPORTED                     0x0356
#define AEEBT_SD_ATTR_ID_1284ID                              0x0358
#define AEEBT_SD_ATTR_ID_PRINTER_NAME                        0x035A
#define AEEBT_SD_ATTR_ID_PRINTER_LOCATION                    0x035C
#define AEEBT_SD_ATTR_ID_DUPLEX_SUPPORTED                    0x035E
#define AEEBT_SD_ATTR_ID_MEDIA_TYPES_SUPPORTED               0x0360
#define AEEBT_SD_ATTR_ID_MAX_MEDIA_WIDTH                     0x0362
#define AEEBT_SD_ATTR_ID_MAX_MEDIA_LENGTH                    0x0364
#define AEEBT_SD_ATTR_ID_ENHANCED_LAYOUT_SUPPORTED           0x0366
#define AEEBT_SD_ATTR_ID_RUI_FORMATS_SUPPORTED               0x0368
#define AEEBT_SD_ATTR_ID_REFERENCE_PRINTING_RUI_SUPPORTED    0x0370
#define AEEBT_SD_ATTR_ID_DIRECT_PRINTING_RUI_SUPPORTED       0x0372
#define AEEBT_SD_ATTR_ID_REFERENCE_PRINTING_TOP_URL          0x0374
#define AEEBT_SD_ATTR_ID_DIRECT_PRINTING_TOP_URL             0x0376
#define AEEBT_SD_ATTR_ID_DEVICE_NAME                         0x037A

// event types returned via HandleEvent()
enum
{
  AEEBT_SD_EVT_DEVICE_DISCOVERY_RESP,       // each time a device is discovered
  AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE,   // when device discovery completes
  AEEBT_SD_EVT_DEVICE_DISCOVERY_STARTED,    // response to DiscoverDevice()
  AEEBT_SD_EVT_DEVICE_DISCOVERY_STOPPED,    // response to StopDeviceDiscovery()
  AEEBT_SD_EVT_DISCOVERABLE_MODE_SET,       // response to SetDiscoverable() and
                                            //   SetLimitedDiscoverable()
  AEEBT_SD_EVT_BROWSE_RESP,                 // response to BrowseService()
  AEEBT_SD_EVT_BROWSE_FAILED,
  AEEBT_SD_EVT_SEARCH_RESP,                 // response to SearchService()
  AEEBT_SD_EVT_SEARCH_FAILED,
  AEEBT_SD_EVT_DEVICE_NAME_RESP,            // response to GetDeviceName()
  AEEBT_SD_EVT_DEVICE_NAME_FAILED,
  AEEBT_SD_EVT_SERVICE_REGISTERED,          // response to RegisterService() and
                                            //   RegisterCustomService()
  AEEBT_SD_EVT_SERVICE_UNREGISTERED,        // response to UnregisterService() and
                                            //   UnregisterCustomService()
  AEEBT_SD_EVT_DISCOVERABLE_MODE_RESP,      // response to GetDiscoverableMode()
  AEEBT_SD_EVT_SERVICE_RECORD_ADDED,        // response to AddServiceRecord()
  AEEBT_SD_EVT_BROWSE_SERVICE_CANCELED      // response to BrowseServiceCancel()
};

// error codes for AEEBT_SD_EVT_*
enum
{
  AEEBT_SD_ERR_NONE,
  AEEBT_SD_ERR_BAD_STATE,
  AEEBT_SD_ERR_DEVICE_DISCOVERY_FAILED,
  AEEBT_SD_ERR_DEVICE_DISCOVERY_ALREADY_IN_PROGRESS,
  AEEBT_SD_ERR_DEVICE_DISCOVERY_NOT_IN_PROGRESS,
  AEEBT_SD_ERR_GET_DEVICE_NAME_FAILED,
  AEEBT_SD_ERR_SERVICE_DB_FULL,
  AEEBT_SD_ERR_SERVICE_RECORD_NOT_FOUND,
  
  /* specific error for Browse failure or Search failure */
  AEEBT_SD_ERR_INVALID_RESPONSE_SYNTAX,
  AEEBT_SD_ERR_MAX_SDP_CONN,
  AEEBT_SD_ERR_REQUEST_PENDING,
  AEEBT_SD_ERR_CONN_FAILED,
  AEEBT_SD_ERR_SERVICE_NOT_FOUND,
  AEEBT_SD_ERR_CONN_TERMINATED,
  AEEBT_SD_ERR_TIMEOUT,
  AEEBT_SD_ERR_UNKNOWN,

  AEEBT_SD_ERR_GET_DISCOVERABLE_MODE_FAILED,
  AEEBT_SD_ERR_ADD_SERVICE_RECORD_FAILED
};

// types of Service Attribute
enum
{
  AEEBT_SD_ATTR_TYPE_UNKNOWN,
  AEEBT_SD_ATTR_TYPE_UUID_LIST,
  AEEBT_SD_ATTR_TYPE_PROTO_DESC_LIST,
  AEEBT_SD_ATTR_TYPE_LANG_BASE_ATTR_ID_LIST,
  AEEBT_SD_ATTR_TYPE_UINT_LIST,
  AEEBT_SD_ATTR_TYPE_STRING,
  AEEBT_SD_ATTR_TYPE_UINT8,
  AEEBT_SD_ATTR_TYPE_UINT16,
  AEEBT_SD_ATTR_TYPE_UINT32,
  AEEBT_SD_ATTR_TYPE_UINT64,
  AEEBT_SD_ATTR_TYPE_BOOL,
  AEEBT_SD_ATTR_TYPE_UUID,
  AEEBT_SD_ATTR_TYPE_UUID128,
  AEEBT_SD_ATTR_TYPE_ADDL_PROTO_DESC_LISTS,
  AEEBT_SD_ATTR_TYPE_UINT128,
  AEEBT_SD_ATTR_TYPE_HID_DESC_LIST
};
typedef uint8     AEEBTAttributeType;


// record for each device discovered by DiscoverDevice()
typedef struct _AEEBTDeviceRecord
{
  AEEBTBDAddr		        bdAddr;
  AEEBTServiceClass     serviceClass;
  AEEBTMajorDeviceClass majorDevClass;
  AEEBTMinorDeviceClass minorDevClass;
} AEEBTDeviceRecord;


typedef struct _AEEBTUUID128
{
  uint8     aUUIDByte[ 16 ];
} AEEBTUUID128;

typedef struct _AEEBTUuidList
{
  uint16        uNumUUIDs;  // how many elements in aUUID are valid
  uint16        aUUID[ AEEBT_SD_MAX_UUID ];
  
  uint16        uNumUUID128s;
  AEEBTUUID128  aUUID128[ AEEBT_SD_MAX_UUID128 ];
} AEEBTUuidList;

typedef struct _AEEBTSDProtoDesc
{
  uint16    uUUID;      // identifying the protocol
  uint16    uNumParam;  // how many elements in aParam are valid
  struct
  {
    uint32  uValue;
    uint8   uSize;
  } aParam[ AEEBT_SD_MAX_PROTO_PARAM ];   // Protocol-specific parameters

  boolean       bUUID128;
  AEEBTUUID128  aUUID128;
} AEEBTSDProtoDesc;

typedef struct _AEEBTSDProtoDescList
{
  uint16            uNumProtoDesc;
  AEEBTSDProtoDesc  aProtoDesc[ AEEBT_SD_MAX_PROTO_DESC ];
} AEEBTSDProtoDescList;

typedef struct _AEEBTSDAddlProtoDescLists
{
  uint16                uNumProtoDescList;
  AEEBTSDProtoDescList  aProtoDescList[ AEEBT_SD_MAX_PROTO_DESC_LISTS ];
} AEEBTSDAddlProtoDescLists;

typedef struct _AEEBTLangBaseAttribIdList
{
  uint16    uNumLangBase;
  struct
  {
    uint16  uLangId;
    uint16  uCharEncId;
    uint16  uBaseAttrId;
  } aLangBase[ AEEBT_SD_MAX_LANG_BASE_REC ];

} AEEBTLangBaseAttribIdList;

typedef struct
{
  uint8   uNumVal;  // how many elements in uVal are valid
  uint64  uVal[ AEEBT_SD_MAX_UINT ];
} AEEBTUintList;

typedef struct
{
  uint8   uVal;
  uint8   uLen;
  byte    str[ AEEBT_SD_MAX_HID_CLASS_DESC_STRING_LEN ];
} AEEBTHIDClassDescType;

typedef struct
{
  uint16  uNumHIDClassDesc;
  AEEBTHIDClassDescType
          aHIDClassDesc[ AEEBT_SD_MAX_NUM_OF_HID_CLASS_DESC ];
} AEEBTHIDDescListType;

// Definition for a SDP Service Attribute
typedef struct _AEEBTServiceAttribute
{
  uint16                      uAttrID;
  AEEBTAttributeType          attrType;
  union
  {
    AEEBTUuidList             uuidList;
    AEEBTSDProtoDescList      protoDescList;
    AEEBTSDAddlProtoDescLists addlProtoDescLists;
    AEEBTLangBaseAttribIdList langBaseAttrIdList;
    AEEBTUintList             uintList;
    AECHAR                    wStr[ AEEBT_SD_MAX_TEXT_STRING_LEN ];
    uint64                    uPrimitiveValue;
    boolean                   bFlag;
    AEEBTHIDDescListType      hidDescList;
    AEEBTUUID128              uuid128;
  } value;

} AEEBTServiceAttribute;


// Definition for a SDP Service Record
typedef struct
{
  boolean               bCustomSvc; /* TRUE if the service record was
                                       created using 128-bit UUID */
  uint16                uUUID;
  AEEBTUUID128          uUUID128;
  uint16                uNumAttr;
  AEEBTServiceAttribute svcAttr[ AEEBT_SD_MAX_SVC_ATTR_ID ];
} AEEBTServiceRecord;

// to be use when browsing for services
typedef struct
{
  uint16    uSvcClsId;
  uint8     uSrvChanNum;
  AECHAR    wSvcName[ AEEBT_SD_MAX_TEXT_STRING_LEN ];
} AEEBTBrowseRecord;

typedef struct _AEEBTSearchPattern
{
  uint16        uNumUUIDs;
  uint16        aUUID[ AEEBT_SD_MAX_SEARCH_UUID ];
  uint16        uNumAttrIDs;
  struct
  {
    boolean     bIsRange;
    uint32      uValue;
  } aAttrID[ AEEBT_SD_MAX_SEARCH_ATTR_ID ];

  uint16        uNumUUID128s;
  AEEBTUUID128  aUUID128[ AEEBT_SD_MAX_SEARCH_UUID128 ];

} AEEBTSearchPattern;

typedef struct _IBTExtSD IBTExtSD;

AEEINTERFACE(IBTExtSD)
{
  INHERIT_IQueryInterface(IBTExtSD);
  int (*DiscoverDevices)(IBTExtSD* po, AEEBTServiceClass svcCls, 
                          const AEEBTBDAddr*, AEEBTDeviceRecord* pDevRec, 
                          uint8 uMaxNumRec);
  int (*StopDeviceDiscovery)(IBTExtSD* po);
  int (*SetDiscoverable)(IBTExtSD* po, boolean discoverable);
  int (*GetDeviceName)(IBTExtSD* po, const AEEBTBDAddr* pBDAddr,
                          AECHAR* pDevName, uint8 uMaxLen);
  int (*BrowseService)(IBTExtSD* po, const AEEBTBDAddr* pBDAddr, 
                          AEEBTBrowseRecord* pBrowseRec, uint8 numRecs);
  int (*SearchService)(IBTExtSD* po, const AEEBTBDAddr* pBDAddr, 
                          const AEEBTSearchPattern* pSrchPttrn,
                          AEEBTServiceRecord* pSvcRec, uint8 numRecs);
  int (*RegisterService)(IBTExtSD* po, uint16 uuid, uint16 version, 
                          uint8 scn, const AECHAR* svcName);
  int (*UnregisterService)(IBTExtSD* po, uint16 uuid);
  int (*SetLimitedDiscoverable)(IBTExtSD* po, AEEBTLIAC uLIAC);
  int (*GetDiscoverableMode)(IBTExtSD* po);
  
  int (*AddServiceRecord)(IBTExtSD* po, uint16 uuid,
                          const AEEBTServiceRecord* pSvcRec);
  int (*AddServiceAttribute)(IBTExtSD* po, uint32 uSvcRecHandle,
                          const AEEBTServiceAttribute* pSvcAttr);
  int (*UpdateServiceAttribute)(IBTExtSD* po, uint32 uSvcRecHandle,
                          const AEEBTServiceAttribute* pSvcAttr);
  int (*ServiceDBEnumInit)(IBTExtSD* po);
  int (*ServiceDBEnumNext)(IBTExtSD* po, AEEBTServiceRecord* pSvcRec);
  int (*RegisterCustomService)(IBTExtSD* po, const AEEBTUuidList* pCustomSvc,
                          uint16 version, uint8 scn, uint16 psm,
                          const AECHAR* svcName);
  int (*UnregisterCustomService)(IBTExtSD* po,
                          const AEEBTUUID128* pCustomUUID);
  int (*GetDeviceNameCancel)(IBTExtSD* po, const AEEBTBDAddr* pBDAddr );
  int (*BrowseCancel)(IBTExtSD* po, const AEEBTBDAddr* pBDAddr );
  
};


#define IBTEXTSD_AddRef(p)                      \
        AEEGETPVTBL((p),IBTExtSD)->AddRef((p))
        
#define IBTEXTSD_Release(p)                     \
        AEEGETPVTBL((p),IBTExtSD)->Release((p))
        
#define IBTEXTSD_QueryInterface(p,i,ppo)        \
        AEEGETPVTBL((p),IBTExtSD)->QueryInterface((p),(i),(ppo))
        
#define IBTEXTSD_DiscoverDevices(p,sc,pa,pr,n)  \
        AEEGETPVTBL((p),IBTExtSD)->DiscoverDevices((p),(sc),(pa),(pr),(n))
        
#define IBTEXTSD_StopDeviceDiscovery(p)         \
        AEEGETPVTBL((p),IBTExtSD)->StopDeviceDiscovery((p))
        
#define IBTEXTSD_SetDiscoverable(p,b)           \
        AEEGETPVTBL((p),IBTExtSD)->SetDiscoverable((p),(b))
        
#define IBTEXTSD_GetDeviceName(p,b,pn,n)        \
        AEEGETPVTBL((p),IBTExtSD)->GetDeviceName((p),(b),(pn),(n))
        
#define IBTEXTSD_GetDeviceNameCancel(p,b)        \
        AEEGETPVTBL((p),IBTExtSD)->GetDeviceNameCancel((p),(b))
        
#define IBTEXTSD_BrowseService(p,pa,pr,n)       \
        AEEGETPVTBL((p),IBTExtSD)->BrowseService((p),(pa),(pr),(n))
        
#define IBTEXTSD_BrowseServiceCancel(p,pa)       \
        AEEGETPVTBL((p),IBTExtSD)->BrowseCancel((p),(pa))
        
#define IBTEXTSD_SearchService(p,pa,ps,pr,n)    \
        AEEGETPVTBL((p),IBTExtSD)->SearchService((p),(pa),(ps),(pr),(n))
        
#define IBTEXTSD_RegisterService(p,u,v,s,pn)    \
        AEEGETPVTBL((p),IBTExtSD)->RegisterService((p),(u),(v),(s),(pn))
        
#define IBTEXTSD_RegisterCustomService(p,pa,v,s,m,pn) \
        AEEGETPVTBL((p),IBTExtSD)->RegisterCustomService((p),(pa),(v),(s),(m),(pn))
        
#define IBTEXTSD_UnregisterService(p,u)         \
        AEEGETPVTBL((p),IBTExtSD)->UnregisterService((p),(u))
        
#define IBTEXTSD_UnregisterCustomService(p,pu)  \
        AEEGETPVTBL((p),IBTExtSD)->UnregisterCustomService((p),(pu))
        
#define IBTEXTSD_SetLimitedDiscoverable(p,u)    \
        AEEGETPVTBL((p),IBTExtSD)->SetLimitedDiscoverable((p),(u))

#define IBTEXTSD_GetDiscoverableMode(p)         \
        AEEGETPVTBL((p),IBTExtSD)->GetDiscoverableMode((p))

#define IBTEXTSD_AddServiceRecord(p,u,pa)       \
        AEEGETPVTBL((p),IBTExtSD)->AddServiceRecord((p),(u),(pa))

#define IBTEXTSD_AddServiceAttribute(p,u,pa)    \
        AEEGETPVTBL((p),IBTExtSD)->AddServiceAttribute((p),(u),(pa))
 
#define IBTEXTSD_UpdateServiceAttribute(p,u,pa) \
        AEEGETPVTBL((p),IBTExtSD)->UpdateServiceAttribute((p),(u),(pa))

#define IBTEXTSD_ServiceDBEnumInit(p)           \
        AEEGETPVTBL((p),IBTExtSD)->ServiceDBEnumInit((p))

#define IBTEXTSD_ServiceDBEnumNext(p,pa)        \
        AEEGETPVTBL((p),IBTExtSD)->ServiceDBEnumNext((p),(pa))

/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================


=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

=======================================================================

Interface Name:  IBTExtSD

Description: BlueTooth Service Discovery

  The use of IBTExtSD consists of sending commands and receiving
  events.  Before issuing a command, first use IBTEXTSD_Register() 
  to register an event handler.  If a command returns SUCCESS, then an 
  event is guaranteed to be generated in response at a later time.

  However, some commands.  Some events are asyncronously 
  generated by the BT audio device, typically in response to a user action, 
  such a pressing the send button or adjusting the speaker volume.
  
  In the event handler, the evt parameter will be equal to one of the
  AEEBT_SD_EVT_* events.  The wParam should be interpreted as a boolean.
  If wParam is equal to FALSE, then the associated command has failed and 
  and lParam contains an AEEBT_SD_ERR_* error code.  If wParam is equal to 
  TRUE, in most cases lParam is unused, but in some cases it contains an 
  integer value as follows:
  
  Event                                     lParam (when wParam is TRUE)
  -----                                     ----------------------------
  AEEBT_SD_EVT_DEVICE_DISCOVERY_RESP        num dev discovered to date
  AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE    sDevDiscComplete
  AEEBT_SD_EVT_BROWSE_RESP                  total num records found
  AEEBT_SD_EVT_SEARCH_RESP                  total num records found

Interfaces:

   IBTExtSD provides several additional interfaces; please see the
   corresponding header file for complete documentation.

   IBASE                        AEE.h
      IBTEXTSD_AddRef()
      IBTEXTSD_Release()
   IQUERYINTERFACE              AEE.h
      IBTEXTSD_QueryInterface()

=======================================================================

Function:  IBTEXTSD_DiscoverDevices()

Description: Discover nearby Bluetooth devices.  To find any BT device
             (no filtering), set svcCls to AEEBT_COD_SC_ALL.  pBDAddr
             is only meaningful when svcCls is set to AEEBT_COD_SC_UNKNOWN,
             and even then, currently only the least significant byte of
             the BD address is used to filter the discovered devices.

Prototype:

  int IBTEXTSD_DiscoverDevices( IBTExtSD*          po,
                                AEEBTServiceClass  svcCls,
                                const AEEBTBDAddr* pBDAddr,
                                AEEBTDeviceRecord* pDevRec,
                                uint8 uMaxNumRec);

Parameters:

  po           - pointer to the IBTExtSD object
  svcCls       - service class used in filtering result
  pBDAddr      - pointer to BD address used in filtering result
  pDevRec      - pointer to memory location to store search result
  uMaxNumRec   - max number of records expected

Return value:

  SUCCESS indicates that the device discovery request was queued and that
  AEEBT_SD_EVT_DEVICE_DISCOVERY_STARTED, AEEBT_SD_EVT_DEVICE_DISCOVERY_RESP 
  and/or AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE events will be sent to the 
  registered event handler at a later time.  All other values indicate 
  failure, and will not generate an event.  In this context, the 
  following error codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  ENOMEMORY - out of memory
  EBADPARM  - NULL input pointers

Events:
  AEEBT_SD_EVT_DEVICE_DISCOVERY_STARTED can be accompanied by any of the 
  following error codes:
  
  AEEBT_SD_ERR_NONE                     - no error
  AEEBT_SD_ERR_DEVICE_DISCOVERY_FAILED  - failed, no error specified
  AEEBT_SD_ERR_DEVICE_DISCOVERY_ALREADY_IN_PROGRESS
                                        - device discovery already started

  If device discovery is successfully started, and at least one device is
  discovered, AEEBT_SD_EVT_DEVICE_DISCOVERY_RESP will be generated and 
  accompanied by an integer representing the number of devices discovered
  up to that point.
  
  If device discovery is successfully started, regardless of the reason
  for device discovery to stop, AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE 
  will be generated and accompanied by an integer representing the total
  number of devices discovered and an error code indicating whether any
  problem was encountered.
  
=======================================================================

Function:  IBTEXTSD_StopDeviceDiscovery()

Description: Stop the on-going device discovery.

Prototype:

  int IBTEXTSD_StopDeviceDiscovery( IBTExtSD* po );

Parameters:

  po           - pointer to the IBTExtSD object

Return value:

  SUCCESS indicates that the request to stop device discovery was queued and
  that AEEBT_SD_EVT_DEVICE_DISCOVERY_STOPPED and
  AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE events will be sent to the 
  registered event handler at a later time.  All other values indicate 
  failure, and will not generate an event.  In this context, the following 
  error codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  ENOMEMORY - out of memory

Events:
  AEEBT_SD_EVT_DEVICE_DISCOVERY_STOPPED can be accompanied by any of the 
  following error codes:
  
  AEEBT_SD_ERR_NONE                     - no error
  AEEBT_SD_ERR_DEVICE_DISCOVERY_NOT_IN_PROGRESS
                                        - device discovery was not in progress

  If device discovery was stopped, AEEBT_SD_EVT_DEVICE_DISCOVERY_COMPLETE 
  will be generated and accompanied by an integer representing the total
  number of devices discovered.
  
=======================================================================

Function:  IBTEXTSD_SetDiscoverable()

Description: Enable/disable discovery of the local device by remote devices.

Prototype:

  int IBTEXTSD_SetDiscoverable( IBTExtSD* po, boolean discoverable );

Parameters:

  po           - pointer to the IBTExtSD object
  discoverable - TRUE to enable, FALSE to disable

Return value:

  SUCCESS indicates that the request to enable/disable discovery was queued 
  and that a AEEBT_SD_EVT_DISCOVERABLE_MODE_SET event will be sent to the 
  registered event handler at a later time.  All other values indicate failure, 
  and will not generate an event.  In this context, the following error
  codes have special meaning:

  EBADSTATE - IBTExtSD object is not registered
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_DISCOVERABLE_MODE_SET can be accompanied by any of the 
  following error codes:
  
  AEEBT_SD_ERR_NONE                     - no error
  
=======================================================================

Function:  IBTEXTSD_SetLimitedDiscoverable()

Description: Enable limited discovery mode.

Prototype:

  int IBTEXTSD_SetLimitedDiscoverable( IBTExtSD* po, AEEBTLIAC uLIAC );

Parameters:

  po           - pointer to the IBTExtSD object
  uLIAC        - Limited Inquiry Access Code

Return value:

  SUCCESS indicates that the request to enable limited discovery mode was queued 
  and that a AEEBT_SD_EVT_DISCOVERABLE_MODE_SET event will be sent to the 
  registered event handler at a later time.  All other values indicate failure, 
  and will not generate an event.  In this context, the following error
  codes have special meaning:

  EBADSTATE - IBTExtSD object is not registered
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_DISCOVERABLE_MODE_SET can be accompanied by any of the 
  following error codes:
  
  AEEBT_SD_ERR_NONE                     - no error
  
=======================================================================

Function:  IBTEXTSD_GetDiscoverableMode()

Description: Query local device's discoverable mode

Prototype:

  int IBTEXTSD_GetDiscoverableMode( IBTExtSD* po );

Parameters:

  po           - pointer to the IBTExtSD object

Return value:

  SUCCESS indicates that the request to query discovery mode was queued 
  and that a AEEBT_SD_EVT_DISCOVERABLE_MODE_RESP event will be sent to the 
  registered event handler at a later time.  All other values indicate failure, 
  and will not generate an event.  In this context, the following error
  codes have special meaning:

  EBADSTATE - IBTExtSD object is not registered

Event:
  AEEBT_SD_EVT_DISCOVERABLE_MODE_RESP can be accompanied by any of the 
  following error codes:
  
  AEEBT_SD_ERR_NONE                         - no error
  AEEBT_SD_ERR_GET_DISCOVERABLE_MODE_FAILED - failed, no reason specified
  
=======================================================================

Function:  IBTEXTSD_GetDeviceName()

Description: Gets the Bluetooth name of the remote device.

Prototype:

  int IBTEXTSD_GetDeviceName( IBTExtSD* po, const AEEBTBDAddr* pBDAddr,
                              AECHAR* pDevName, uint8 uMaxLen );

Parameters:

  po          - pointer to the IBTExtSD object
  pBDAddr     - pointer to the BD Address of remote device
  pDevName    - pointer to memory location to store remote device name
  uMaxLen     - max num of characters expected

Return value:

  SUCCESS indicates that the request to get device name was queued and that a
  AEEBT_SD_EVT_DEVICE_NAME_RESP event will be sent to the registered event
  handler at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error
  codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  EBADPARM  - an input pointer is invalid
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_DEVICE_NAME_RESP can be accompanied by any of the 
  following error codes:
  
  AEEBT_SD_ERR_NONE                   - no error
  AEEBT_SD_ERR_GET_DEVICE_NAME_FAILED - failed, one of the reason 
                                        is that the cancel initiated
                                        by IBTEXTSD_GetDeviceNameCancel()
                                        was successful
  
=======================================================================

Function:  IBTEXTSD_GetDeviceNameCancel()

Description: Cancels the device name request that was initiated 
             by IBTEXTSD_GetDeviceName().

Prototype:

  int IBTEXTSD_GetDeviceNameCancel( IBTExtSD* po, const AEEBTBDAddr* pBDAddr);

Parameters:

  po          - pointer to the IBTExtSD object
  pBDAddr     - pointer to the BD Address of remote device
  
Return value:

  SUCCESS indicates that the request to cancel the device name request 
  was queued. All other values indicate failure. In this context, 
  the following error codes have special meaning: 
    
  EBADSTATE - IBTExtSD object is not registered
  EBADPARM  - an input pointer is invalid
  ENOMEMORY - out of memory
  
  If the cancel succeeds, AEEBT_SD_EVT_DEVICE_NAME_RESP that is sent in 
  response to IBTEXTSD_GetDeviceName() is accompanied with error code
  AEEBT_SD_ERR_GET_DEVICE_NAME_FAILED.

Event:
  None
  
=======================================================================

Function:  IBTEXTSD_BrowseService()

Description: Browse a remote BT device for available services.

Prototype:

  int IBTEXTSD_BrowseService( IBTExtSD* po, const AEEBTBDAddr* pBDAddr,
                              AEEBTBrowseRecord* pBrowseRec,
                              uint8 maxNumRecs );

Parameters:

  po          - pointer to the IBTEXTSD object
  pBDAddr     - pointer to the BD Address of remote device
  pBrowseRec  - pointer to memory location to store browse result
  maxNumRecs  - max number of records expected

Return value:

  SUCCESS indicates that the request to perform service search was queued and
  that AEEBT_SD_EVT_BROWSE_RESP event will be sent to the registered event
  handler at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error
  codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  EBADPARM  - an input pointer is NULL
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_BROWSE_RESP can be accompanied by total number of records found
  if wParam==TRUE, or the error code AEEBT_SD_ERR_BROWSE_SERVICE_FAILED if 
  wParam==FALSE.
  
=======================================================================

Function:  IBTEXTSD_BrowseServiceCancel()

Description: Cancels a browse operation that was initiated by a call 
             to IBTEXTSD_BrowseService().

Prototype:

  int IBTEXTSD_BrowseServiceCancel( IBTExtSD* po, const AEEBTBDAddr* pBDAddr );
                             
Parameters:

  po          - pointer to the IBTEXTSD object
  pBDAddr     - pointer to the BD Address of remote device
  
Return value:

  SUCCESS indicates that the request to perform browse cancel was queued and
  that AEEBT_SD_EVT_BROWSE_SERVICE_CANCELED event will be sent to
  the registered event  handler at a later time. 
  All other values indicate failure, and  will not generate an event.
  In this context, the following error codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered or browse currently not in 
              progress. 
  EBADPARM  - an input pointer is NULL
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_BROWSE_SERVICE_CANCELED is sent to the app to indicate that 
  browsing was cancelled.Note that in this case AEEBT_SD_EVT_BROWSE_RESP
  event will not be sent to the application.
  
=======================================================================

Function:  IBTEXTSD_SearchService()

Description: Searches a remote BT device for some particular services 
             and the characteristics specific to each requested service.

Prototype:

  int IBTEXTSD_SearchService( IBTExtSD* po, const AEEBTBDAddr* pBDAddr,
                              const AEEBTSearchPattern* pSrchPttrn,
                              AEEBTServiceRecord* pSvcRec,
                              uint8 maxNumRecs );

Parameters:

  po          - pointer to the IBTEXTSD object
  pBDAddr     - pointer to the BD Address of remote device
  pSrchPttrn  - pointer to the search pattern containing the search criteria;
                if 'uNumUUIDs' is set to 0, the 'uuid128' field must be set
                for service search using 128-bit UUID.  Also, only one
                attribute ID is supported
  pSvcRec     - pointer to memory location to store search result
  maxNumRecs  - max number of records expected

Return value:

  SUCCESS indicates that the request to perform service search was queued and
  that AEEBT_SD_EVT_SEARCH_RESP event will be sent to the registered event
  handler at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error
  codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  EBADPARM  - an input pointer is NULL
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_SEARCH_RESP can be accompanied by total number of records found
  if wParam==TRUE, or the error code AEEBT_SD_ERR_SEARCH_SERVICE_FAILED if 
  wParam==FALSE.
  
=======================================================================

Function:  IBTEXTSD_RegisterService() and IBTEXTSD_RegisterCustomService

Description: Registers a service with Bluetooth driver.  This causes a service
             record to be created with the information provided in these APIs.
             
             IBTEXTSD_RegisterService is used for service registration using
             16-bit UUID.
               
             IBTEXTSD_RegisterCustomService is used for custom service
             registration using 128-bit UUID.

Prototype:

  int IBTEXTSD_RegisterService( IBTExtSD* po, uint16 uuid, uint16 version,
                                uint8 scn, const AECHAR* pwSvcName );

  int IBTEXTSD_RegisterCustomService( IBTExtSD* po,
                                const AEEBTUuidList* pCustomSvc,
                                uint16 version, uint8 scn, uint16 psm,
                                const AECHAR* pwSvcName );

Parameters:

  po          - pointer to the IBTEXTSD object
  uuid        - the 16-bit UUID representing the service
  pCustomSvc  - pointer to the UUID list representing the custom service, which
                must contain at least one 128-bit UUID.  The 16-bit UUIDs are
                optional.  If multiple 128-bit or 16-bit UUIDs are specified,
                the 128-bit and 16-bit UUID lists must be specified in the
                order from the most specific UUID to the most generic, with
                all the UUIDs in the 128-bit UUID list being more specific
                that those in the 16-bit UUID list.  To unregister the custom
                service, only the most specific 128-bit UUID needs to be
                specified in the corresponding unregister API
  version     - version number of the service
  scn         - server channel number associated with the service
  psm         - PSM associated with the service
  pwSvcName   - pointer to the name of the service

Return value:

  SUCCESS indicates that the request to register the service was queued and
  that AEEBT_SD_EVT_SERVICE_REGISTERED event will be sent to the registered 
  event handler at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error
  codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  EBADPARM  - an input pointer is NULL
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_SERVICE_REGISTERED can be accompanied by any of the following
  error codes:
  
  AEEBT_SD_ERR_NONE             - no error
  AEEBT_SD_ERR_SERVICE_DB_FULL  - no free service record available
  
=======================================================================

Function:  IBTEXTSD_UnregisterService() and IBTEXTSD_UnregisterCustomService()

Description: Unregisters a service with Bluetooth driver.  This causes the 
             service record for the given service to be purged from Bluetooth
             driver's database.

             IBTEXTSD_UnregisterService is used for service unregistration
             using 16-bit UUID.
               
             IBTEXTSD_UnregisterCustomService is used for service
             unregistration using 128-bit UUID.

Prototype:

  int IBTEXTSD_UnregisterService( IBTExtSD* po, uint16 uuid );
  
  int IBTEXTSD_UnregisterCustomService( IBTExtSD* po,
                                        const AEEBTUUID128* pCustomUUID );

Parameters:

  po          - pointer to the IBTEXTSD object
  uuid        - the UUID representing the service to be unregistered
  pCustomUUID - pointer to the 128-bit UUID representing the custom service
                to be unregistered

Return value:

  SUCCESS indicates that the request to register the service was queued and
  that AEEBT_SD_EVT_SERVICE_UNREGISTERED event will be sent to the registered 
  event handler at a later time.  All other values indicate failure, and
  will not generate an event.  In this context, the following error
  codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  ENOMEMORY - out of memory

Event:
  AEEBT_SD_EVT_SERVICE_UNREGISTERED can be accompanied by any of the
  following error codes:
  
  AEEBT_SD_ERR_NONE                     - no error
  AEEBT_SD_ERR_SERVICE_RECORD_NOT_FOUND - record not found in service DB

=======================================================================

Function:  IBTEXTSD_AddServiceRecord()

Description: Add a service record to the local SDP database.

Prototype:

  int IBTExtSD_AddServiceRecord( IBTExtSD*                  po,
                                 uint16                     uuid,
                                 const AEEBTServiceRecord*  pSvcRec );

Parameters:

  po            - pointer to the IBTEXTSD object
  uuid          - UUID representing the service to be updated
  pSvcRec       - pointer to a fully or partially populated service record.
                  For populated service attributes, the service attribute
                  ID field must be specified

Return value:

  SUCCESS indicates that the request to add the service record was queued
  and that AEEBT_SD_EVT_SERVICE_RECORD_ADDED event will be sent to the
  registered event handler at a later time.  All other values indicate
  failure, and will not generate an event.  In this context, the following
  error codes have special meaning:
  
  EBADSTATE - IBTExtSD object is not registered
  EBADPARM  - an input pointer is NULL
  ENOMEMORY - out of memory
  
Event:
  AEEBT_SD_EVT_SERVICE_RECORD_ADDED can be accompanied by any of the
  following error codes:

  uSvcRecHandle                 - no error if uSvcRecHandle != 0xFFFFFFFF
  AEEBT_SD_ERR_SERVICE_DB_FULL  - no free service record available
  
=======================================================================

Function:  IBTEXTSD_AddServiceAttribute()

Description: Add a service attribute to a service record.

Prototype:

  int IBTExtSD_AddServiceAttribute( IBTExtSD*  po,
                                    uint32     uSvcRecHandle,
                                    const AEEBTServiceAttribute*  pSvcAttrx );

Parameters:

  po            - pointer to the IBTEXTSD object
  uSvcRecHandle - handle of the service record to which the attribute is added
  pSvcAttr      - pointer to the service attribute to be added.  The service
                  attribute ID must be specified

Return value:

  SUCCESS indicates that the specified service attribute has been added to
  the service record.  No events will be generated for this command.
  In this context, the following error codes have special meaning:

    EBADPARAM         - Bad input parameter
    AEEBT_ENOREC      - Service record does not exist
    AEEBT_ERECFULL    - Service record already full
  
=======================================================================

Function:  IBTEXTSD_UpdateServiceAttribute()

Description: Update an existing service attribute.

Prototype:

  int IBTExtSD_UpdateServiceAttribute( IBTExtSD*  po,
                                       uint32     uSvcRecHandle,
                                       const AEEBTServiceAttribute*  pSvcAttr );

Parameters:

  po            - pointer to the IBTEXTSD object
  uSvcRecHandle - handle of the service record to be updated
  pSvcAttr      - pointer to the service attribute to be updated.  The service
                  attribute ID must be specified

Return value:

  SUCCESS indicates that the specified service attribute has been updated.
  No events will be generated for this command.  In this context, the
  following error codes have special meaning:

    EBADPARAM         - Bad input parameter
    AEEBT_ENOREC      - Service record does not exist
    AEEBT_ENOATTR     - Service attribute does not exist
    
=======================================================================

Function:  IBTEXTSD_ServiceDBEnumInit()

Description: Initialize/reset the enumeration in the SDP service database.

Prototype:

  int IBTExtSD_ServiceDBEnumInit( IBTExtSD*  po );

Parameters:

  po            - pointer to the IBTEXTSD object

Return value:

=======================================================================

Function:  IBTEXTSD_ServiceDBEnumNext()

Description: Retrieve the next service record in the SDP service database. 

Prototype:

  int IBTExtSD_ServiceDBEnumNext( IBTExtSD*  po,
                                  AEEBTServiceRecord*  pSvcRec );

Parameters:

  po            - pointer to the IBTEXTSD object
  pSvcRec       - pointer to memory location to store service record info

Return value:

   SUCCESS indicates that the next Service Record Handle is successfully
   retrieved.

    EBADPARAM         - Bad input parameter
	AEEBT_EENDOFREC   - End of the Service Records in the database
=======================================================================*/


#endif /* _AEE_BT_EXT_SD_H_ */
