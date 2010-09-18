#ifndef AEEBTSDDEF_H
#define AEEBTSDDEF_H
/*=============================================================================
FILE:         AEEBTSDDef.h

SERVICES:     

DESCRIPTION:  Definitions for constants and types used by IBTServiceDiscovery.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBTDef.h"

/*=============================================================================
 Some upper limits - arbitrarily chosen
=============================================================================*/
#define AEEBTSD_MAX_SEARCH_UUID                         12
#define AEEBTSD_MAX_SEARCH_ATTR_ID                      12 
                                                       
#define AEEBTSD_MAX_ATTR_PER_REC                        12
#define AEEBTSD_MAX_PROTO_DESC                           4
#define AEEBTSD_MAX_PROTO_DESC_PARAMS                    1
#define AEEBTSD_MAX_HID_CLASS_DESC_STRING_LEN           55

/*=============================================================================
 Service attribute types
=============================================================================*/
#define AEEBTSD_ATTRIBUTE_TYPE_UNKNOWN                   0
#define AEEBTSD_ATTRIBUTE_TYPE_BOOL                      1
#define AEEBTSD_ATTRIBUTE_TYPE_UINT8                     2
#define AEEBTSD_ATTRIBUTE_TYPE_UINT16                    3
#define AEEBTSD_ATTRIBUTE_TYPE_UINT32                    4
#define AEEBTSD_ATTRIBUTE_TYPE_UINT64                    5
#define AEEBTSD_ATTRIBUTE_TYPE_UINT128                   6
#define AEEBTSD_ATTRIBUTE_TYPE_UUID                      7
#define AEEBTSD_ATTRIBUTE_TYPE_UUID128                   8
#define AEEBTSD_ATTRIBUTE_TYPE_STRING                    9
#define AEEBTSD_ATTRIBUTE_TYPE_UINT_LIST                10
#define AEEBTSD_ATTRIBUTE_TYPE_UUID_LIST                11
#define AEEBTSD_ATTRIBUTE_TYPE_PROTO_DESC_LIST          12
#define AEEBTSD_ATTRIBUTE_TYPE_LANG_BASE_ATTR_ID_LIST   13
#define AEEBTSD_ATTRIBUTE_TYPE_HID_DESC_LIST            15

typedef uint32 BTSvcAttribType;

/*=============================================================================
 Service attribute IDs
=============================================================================*/
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_RECORD_HANDLE                0x0000
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST                0x0001
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_RECORD_STATE                 0x0002
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_SERVICE_ID                   0x0003
#define AEEBTSD_ATTRIBUTE_ID_PROTOCOL_DESCRIPTOR_LIST             0x0004
#define AEEBTSD_ATTRIBUTE_ID_BROWSE_GROUP_LIST                    0x0005
#define AEEBTSD_ATTRIBUTE_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST      0x0006
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_INFO_TIME_TO_LIVE            0x0007
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_AVAILABILITY                 0x0008
#define AEEBTSD_ATTRIBUTE_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST    0x0009
#define AEEBTSD_ATTRIBUTE_ID_DOCUMENTATION_URL                    0x000A
#define AEEBTSD_ATTRIBUTE_ID_CLIENT_EXECUTABLE_URL                0x000B
#define AEEBTSD_ATTRIBUTE_ID_ICON_URL                             0x000C
#define AEEBTSD_ATTRIBUTE_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS 0x000D

#define AEEBTSD_ATTRIBUTE_ID_SERVICE_NAME                         0x0100
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_DESCRIPTION                  0x0101
#define AEEBTSD_ATTRIBUTE_ID_PROVIDER_NAME                        0x0102

#define AEEBTSD_ATTRIBUTE_ID_VERSION_NUMBER_LIST                  0x0200
#define AEEBTSD_ATTRIBUTE_ID_GROUP_ID                             0x0200
#define AEEBTSD_ATTRIBUTE_ID_SERVICE_DATABASE_STATE               0x0201
#define AEEBTSD_ATTRIBUTE_ID_HID_DESCRIPTOR_LIST                  0x0206

#define AEEBTSD_ATTRIBUTE_ID_SERVICE_VERSION                      0x0300

#define AEEBTSD_ATTRIBUTE_ID_NETWORK                              0x0301
#define AEEBTSD_ATTRIBUTE_ID_CTP_EXTERNAL_NETWORK                 0x0301
#define AEEBTSD_ATTRIBUTE_ID_SYNC_SUPPORTED_DATA_STORES_LIST      0x0301

#define AEEBTSD_ATTRIBUTE_ID_GAP_REMOTE_AUDIO_VOLUME_CONTROL      0x0302
#define AEEBTSD_ATTRIBUTE_ID_FAX_CLASS_1_SUPPORT                  0x0302

#define AEEBTSD_ATTRIBUTE_ID_OPP_SUPPORTED_FORMATS_LIST           0x0303
#define AEEBTSD_ATTRIBUTE_ID_FAX_CLASS_2_0_SUPPORT                0x0303

#define AEEBTSD_ATTRIBUTE_ID_FAX_CLASS_2_SUPPORT                  0x0304
#define AEEBTSD_ATTRIBUTE_ID_AUDIO_FEEDBACK_SUPPORT               0x0305
#define AEEBTSD_ATTRIBUTE_ID_BIP_SUPPORTED_CAPABILITIES           0x0310
#define AEEBTSD_ATTRIBUTE_ID_SUPPORTED_FEATURES                   0x0311
#define AEEBTSD_ATTRIBUTE_ID_BIP_SUPPORTED_FUNCTIONS              0x0312
#define AEEBTSD_ATTRIBUTE_ID_BIP_TOTAL_IMAGING_DATA_CAPACITY      0x0313
#define AEEBTSD_ATTRIBUTE_ID_PBAP_SUPPORTED_REPOSITORIES          0x0314

/* BPP printer specific attributes */
#define AEEBTSD_ATTRIBUTE_ID_DOCUMENT_FORMATS_SUPPORTED           0x0350
#define AEEBTSD_ATTRIBUTE_ID_CHARACTER_REPERTOIRES_SUPPORTED      0x0352
#define AEEBTSD_ATTRIBUTE_ID_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED  0x0354
#define AEEBTSD_ATTRIBUTE_ID_COLOR_SUPPORTED                      0x0356
#define AEEBTSD_ATTRIBUTE_ID_1284ID                               0x0358
#define AEEBTSD_ATTRIBUTE_ID_PRINTER_NAME                         0x035A
#define AEEBTSD_ATTRIBUTE_ID_PRINTER_LOCATION                     0x035C
#define AEEBTSD_ATTRIBUTE_ID_DUPLEX_SUPPORTED                     0x035E
#define AEEBTSD_ATTRIBUTE_ID_MEDIA_TYPES_SUPPORTED                0x0360
#define AEEBTSD_ATTRIBUTE_ID_MAX_MEDIA_WIDTH                      0x0362
#define AEEBTSD_ATTRIBUTE_ID_MAX_MEDIA_LENGTH                     0x0364
#define AEEBTSD_ATTRIBUTE_ID_ENHANCED_LAYOUT_SUPPORTED            0x0366
#define AEEBTSD_ATTRIBUTE_ID_RUI_FORMATS_SUPPORTED                0x0368
#define AEEBTSD_ATTRIBUTE_ID_REFERENCE_PRINTING_RUI_SUPPORTED     0x0370
#define AEEBTSD_ATTRIBUTE_ID_DIRECT_PRINTING_RUI_SUPPORTED        0x0372
#define AEEBTSD_ATTRIBUTE_ID_REFERENCE_PRINTING_TOP_URL           0x0374
#define AEEBTSD_ATTRIBUTE_ID_DIRECT_PRINTING_TOP_URL              0x0376
#define AEEBTSD_ATTRIBUTE_ID_DEVICE_NAME                          0x037A

typedef uint32 BTSvcAttribID;

/*=============================================================================
 Service discoverable mode
=============================================================================*/
#define AEEBTSD_DISCOVERABLE_MODE_NONE       0
#define AEEBTSD_DISCOVERABLE_MODE_GENERAL    1
#define AEEBTSD_DISCOVERABLE_MODE_LIMITED    2

typedef uint32 BTSvcDiscoverableMode;

/*=============================================================================
 Structures
=============================================================================*/
typedef struct
{
   int         nNumUUID16s;
   BTUUID16    UUID16 [AEEBTSD_MAX_SEARCH_UUID];
   boolean     bUUID128Valid;
   BTUUID128   UUID128;
   int         nNumAttributes;
   uint32      attribValue [AEEBTSD_MAX_SEARCH_ATTR_ID];
}  BTSvcSearchPattern;

typedef struct
{
   BTSvcAttribID     ID;
   BTSvcAttribType   type;
   int               nSize;
}  BTServiceAttrib;

typedef struct
{
   uint32   uHandle;
   BTUUID16 profileUUID;
}  BTServiceRecordSummary;

/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTSvcAttribType

Description:
   The types of attribute in Bluetooth service record.

Definition:
   #define AEEBTSD_ATTRIBUTE_TYPE_UNKNOWN                   0
   #define AEEBTSD_ATTRIBUTE_TYPE_BOOL                      1
   #define AEEBTSD_ATTRIBUTE_TYPE_UINT8                     2
   #define AEEBTSD_ATTRIBUTE_TYPE_UINT16                    3
   #define AEEBTSD_ATTRIBUTE_TYPE_UINT32                    4
   #define AEEBTSD_ATTRIBUTE_TYPE_UINT64                    5
   #define AEEBTSD_ATTRIBUTE_TYPE_UINT128                   6
   #define AEEBTSD_ATTRIBUTE_TYPE_UUID                      7
   #define AEEBTSD_ATTRIBUTE_TYPE_UUID128                   8
   #define AEEBTSD_ATTRIBUTE_TYPE_STRING                    9
   #define AEEBTSD_ATTRIBUTE_TYPE_UINT_LIST                10
   #define AEEBTSD_ATTRIBUTE_TYPE_UUID_LIST                11
   #define AEEBTSD_ATTRIBUTE_TYPE_PROTO_DESC_LIST          12
   #define AEEBTSD_ATTRIBUTE_TYPE_LANG_BASE_ATTR_ID_LIST   13
   #define AEEBTSD_ATTRIBUTE_TYPE_HID_DESC_LIST            15
   typedef uint32 BTSvcAttribType;

===============================================================================
BTSvcAttribID

Description:
   Attribute IDs assigned by Bluetooth SIG.

Definition:
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_RECORD_HANDLE                0x0000
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST                0x0001
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_RECORD_STATE                 0x0002
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_SERVICE_ID                   0x0003
   #define AEEBTSD_ATTRIBUTE_ID_PROTOCOL_DESCRIPTOR_LIST             0x0004
   #define AEEBTSD_ATTRIBUTE_ID_BROWSE_GROUP_LIST                    0x0005
   #define AEEBTSD_ATTRIBUTE_ID_LANGUAGE_BASE_ATTRIBUTE_ID_LIST      0x0006
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_INFO_TIME_TO_LIVE            0x0007
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_AVAILABILITY                 0x0008
   #define AEEBTSD_ATTRIBUTE_ID_BLUETOOTH_PROFILE_DESCRIPTOR_LIST    0x0009
   #define AEEBTSD_ATTRIBUTE_ID_DOCUMENTATION_URL                    0x000A
   #define AEEBTSD_ATTRIBUTE_ID_CLIENT_EXECUTABLE_URL                0x000B
   #define AEEBTSD_ATTRIBUTE_ID_ICON_URL                             0x000C
   #define AEEBTSD_ATTRIBUTE_ID_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS 0x000D
   
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_NAME                         0x0100
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_DESCRIPTION                  0x0101
   #define AEEBTSD_ATTRIBUTE_ID_PROVIDER_NAME                        0x0102
   
   #define AEEBTSD_ATTRIBUTE_ID_VERSION_NUMBER_LIST                  0x0200
   #define AEEBTSD_ATTRIBUTE_ID_GROUP_ID                             0x0200
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_DATABASE_STATE               0x0201
   #define AEEBTSD_ATTRIBUTE_ID_HID_DESCRIPTOR_LIST                  0x0206
   
   #define AEEBTSD_ATTRIBUTE_ID_SERVICE_VERSION                      0x0300
   
   #define AEEBTSD_ATTRIBUTE_ID_NETWORK                              0x0301
   #define AEEBTSD_ATTRIBUTE_ID_CTP_EXTERNAL_NETWORK                 0x0301
   #define AEEBTSD_ATTRIBUTE_ID_SYNC_SUPPORTED_DATA_STORES_LIST      0x0301
   
   #define AEEBTSD_ATTRIBUTE_ID_GAP_REMOTE_AUDIO_VOLUME_CONTROL      0x0302
   #define AEEBTSD_ATTRIBUTE_ID_FAX_CLASS_1_SUPPORT                  0x0302
   
   #define AEEBTSD_ATTRIBUTE_ID_OPP_SUPPORTED_FORMATS_LIST           0x0303
   #define AEEBTSD_ATTRIBUTE_ID_FAX_CLASS_2_0_SUPPORT                0x0303
   
   #define AEEBTSD_ATTRIBUTE_ID_FAX_CLASS_2_SUPPORT                  0x0304
   #define AEEBTSD_ATTRIBUTE_ID_AUDIO_FEEDBACK_SUPPORT               0x0305
   #define AEEBTSD_ATTRIBUTE_ID_BIP_SUPPORTED_CAPABILITIES           0x0310
   #define AEEBTSD_ATTRIBUTE_ID_SUPPORTED_FEATURES                   0x0311
   #define AEEBTSD_ATTRIBUTE_ID_BIP_SUPPORTED_FUNCTIONS              0x0312
   #define AEEBTSD_ATTRIBUTE_ID_BIP_TOTAL_IMAGING_DATA_CAPACITY      0x0313
   #define AEEBTSD_ATTRIBUTE_ID_PBAP_SUPPORTED_REPOSITORIES          0x0314
   
   #define AEEBTSD_ATTRIBUTE_ID_DOCUMENT_FORMATS_SUPPORTED           0x0350
   #define AEEBTSD_ATTRIBUTE_ID_CHARACTER_REPERTOIRES_SUPPORTED      0x0352
   #define AEEBTSD_ATTRIBUTE_ID_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED  0x0354
   #define AEEBTSD_ATTRIBUTE_ID_COLOR_SUPPORTED                      0x0356
   #define AEEBTSD_ATTRIBUTE_ID_1284ID                               0x0358
   #define AEEBTSD_ATTRIBUTE_ID_PRINTER_NAME                         0x035A
   #define AEEBTSD_ATTRIBUTE_ID_PRINTER_LOCATION                     0x035C
   #define AEEBTSD_ATTRIBUTE_ID_DUPLEX_SUPPORTED                     0x035E
   #define AEEBTSD_ATTRIBUTE_ID_MEDIA_TYPES_SUPPORTED                0x0360
   #define AEEBTSD_ATTRIBUTE_ID_MAX_MEDIA_WIDTH                      0x0362
   #define AEEBTSD_ATTRIBUTE_ID_MAX_MEDIA_LENGTH                     0x0364
   #define AEEBTSD_ATTRIBUTE_ID_ENHANCED_LAYOUT_SUPPORTED            0x0366
   #define AEEBTSD_ATTRIBUTE_ID_RUI_FORMATS_SUPPORTED                0x0368
   #define AEEBTSD_ATTRIBUTE_ID_REFERENCE_PRINTING_RUI_SUPPORTED     0x0370
   #define AEEBTSD_ATTRIBUTE_ID_DIRECT_PRINTING_RUI_SUPPORTED        0x0372
   #define AEEBTSD_ATTRIBUTE_ID_REFERENCE_PRINTING_TOP_URL           0x0374
   #define AEEBTSD_ATTRIBUTE_ID_DIRECT_PRINTING_TOP_URL              0x0376
   #define AEEBTSD_ATTRIBUTE_ID_DEVICE_NAME                          0x037A
   
   typedef uint32 BTSvcAttribID;

===============================================================================
BTSvcDiscoverableMode

Description:
   The different modes of discoverability a in which Bluetooth radio can be.

Definition:
   #define AEEBTSD_DISCOVERABLE_MODE_NONE       0
   #define AEEBTSD_DISCOVERABLE_MODE_GENERAL    1
   #define AEEBTSD_DISCOVERABLE_MODE_LIMITED    2
   
   typedef uint32 BTSvcDiscoverableMode;

===============================================================================
BTSvcSearchPattern

Description:
   This data type represents the search data to be matched.

Definition:
   typedef struct
   {
      int         nNumUUID16s;
      BTUUID16    UUID16 [AEEBTSD_MAX_SEARCH_UUID];
      boolean     bUUID128Valid;
      BTUUID128   UUID128;
      int         nNumAttributes;
      uint32      attribValue [AEEBTSD_MAX_SEARCH_ATTR_ID];
   }  BTSvcSearchPattern;

Members:
   nNumUUID16s    : number of valid items in UUID16 list
   UUID16         : array of UUID16s
   bUUID128Valid  : TRUE if UUID128 is valid
   UUID128        : 128-bit UUID
   nNumAttributes : number of valid items in attribValue list
   attribValue    : array of attributes for which to search; to specify a range
                     of attributes, set the beginning attribute UUID in the
                     high order 16 bits and the ending attribute UUID in the
                     low order 16 bits.

===============================================================================
BTServiceAttrib

Description:
   This data type represents the basic information for an attribute.

Definition:
   typedef struct
   {
      BTSvcAttribID     ID;
      BTSvcAttribType   type;
      int               nSize;
   }  BTServiceAttrib;

Members:
   ID             : the attribute ID assigned by Bluetooth SID
   type           : the type of attribute
   nSize          : memory size required to hold the attribute

===============================================================================
BTServiceRecordSummary

Description:
   This data type represents the most essential information of a service record.

Definition:
   typedef struct
   {
      uint32   uHandle;
      BTUUID16 profileUUID;
   }  BTServiceRecordSummary;

Members:
   uHandle        : the handle for this record, assigned by underlying 
                      Bluetooth driver
   profileUUID    : the profile supported by the service represented by this
                      service record

=============================================================================*/

#endif // AEEBTSDDEF_H

