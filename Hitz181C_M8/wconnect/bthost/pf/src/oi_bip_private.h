/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bip_private.h#1 $ 

$Log: $
*
*    #4        18 Oct 2006            PR
* Merged OI Code v3.0.14rev1revQ
*
*    #3        23 Jul 2004            JH
* Moved to version 3.0.12 revision 1
*
===========================================================================*/

/** 
@file 
@internal
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/

/** \addtogroup BIP_Internal */
/**@{*/

#ifndef _OI_BIP_PRIVATE_H
#define _OI_BIP_PRIVATE_H

#include "oi_bytestream.h"
#include "oi_obexspec.h"
#include "oi_bip_spec.h"

/** application parameter manipulation functions */

typedef struct _OI_BIP_APP_PARAM_BOOKKEEPING {
    OI_OBEX_BYTESEQ byteseq;
    OI_INT16 nextTagPos;
    OI_BYTE_STREAM bs;
} OI_BIP_APP_PARAM_BOOKKEEPING;

typedef void *OI_BIP_GENERIC_PARAMS;

/** Resets the parameter bookkeeping data structure in preperation for building
 * an application parameters header.
 * @param bk    Pointer to the bookkeeping structure
 * @param data  Pointer to the raw data buffer to contain the finished header
 * @param len   Length of the raw data buffer
 */
void OI_BIP_InitAppParams(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len);


/** Appends an application parameter entry in UINT8 format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void OI_BIP_AppendAppParamUINT8(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT8 value);


/** Appends an application parameter entry in UINT16 (big endian) format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void OI_BIP_AppendAppParamUINT16(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT16 value);


/** Appends an application parameter entry in UINT32 (big endian) format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param value The value of the entry
 */
void OI_BIP_AppendAppParamUINT32(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT32 value);


/** Appends an application parameter entry in UUID128 format
 * @param bk    Pointer to the bookkeeping structure
 * @param tag   Application parameter tag for this entry
 * @param uuid  Pointer to the UUID. This data is copied by the function.
 */
void OI_BIP_AppendAppParamUUID128(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UUID128 *uuid);


/** Finalizes the bookkeeping data structure. After this call bk is passed
 * to BIP_GetRequest or BIP_SetRequest via the "appParam" function parameter.
 * @param bk    Pointer to the bookkeeping structure
 */
void OI_BIP_FinishAppParams(OI_BIP_APP_PARAM_BOOKKEEPING *bk);


void OI_BIP_ResetAppParams(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len);
OI_STATUS OI_BIP_FindAppParamUINT8(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT8 *value);
OI_STATUS OI_BIP_FindAppParamUINT16(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT16 *value);
OI_STATUS OI_BIP_FindAppParamUINT32(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT32 *value);
OI_STATUS OI_BIP_FindAppParamUUID128(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UUID128 *uuid);


extern const OI_OBEX_BYTESEQ OI_BIP_CapabilitiesType;
extern const OI_OBEX_BYTESEQ OI_BIP_ImageListType;
extern const OI_OBEX_BYTESEQ OI_BIP_ImageType;
extern const OI_OBEX_BYTESEQ OI_BIP_ThumbnailType;
extern const OI_OBEX_BYTESEQ OI_BIP_AttachmentType;
extern const OI_OBEX_BYTESEQ OI_BIP_PartialType;
extern const OI_OBEX_BYTESEQ OI_BIP_MonitoringType;
extern const OI_OBEX_BYTESEQ OI_BIP_StatusType;
extern const OI_OBEX_BYTESEQ OI_BIP_DisplayType;
extern const OI_OBEX_BYTESEQ OI_BIP_PrintType;
extern const OI_OBEX_BYTESEQ OI_BIP_ArchiveType;
extern const OI_OBEX_BYTESEQ OI_BIP_PropertiesType;


typedef enum {
    BIP_CMD_NONE = 0,
    BIP_CMD_GET_CAPABILITIES,
    
    /* Put and Delete must be dispatched specially since they share a type
     * header */
    BIP_CMD_PUT_OR_DELETE_IMAGE,
    BIP_CMD_PUT_IMAGE,
    BIP_CMD_DELETE_IMAGE,
    
    BIP_CMD_PUT_LINKED_ATTACHMENT,
    BIP_CMD_PUT_LINKED_THUMBNAIL,
    BIP_CMD_REMOTE_DISPLAY,
    BIP_CMD_GET_IMAGES_LIST,
    BIP_CMD_GET_IMAGE_PROPERTIES,
    BIP_CMD_GET_IMAGE,
    BIP_CMD_GET_LINKED_THUMBNAIL,
    BIP_CMD_GET_LINKED_ATTACHMENT,
    BIP_CMD_START_PRINT,
    BIP_CMD_GET_PARTIAL_IMAGE,
    BIP_CMD_START_ARCHIVE,
    BIP_CMD_GET_MONITORING_IMAGE,
    BIP_CMD_GET_STATUS
} BIP_CMD;

typedef enum {
    BIP_PARAM_NONE = 0,
    BIP_PARAM_NAME = BIT0,
    BIP_PARAM_DESCRIPTOR = BIT1,
    BIP_PARAM_HANDLE = BIT2,
    BIP_PARAM_NB_RETURNED_HANDLES = BIT3,
    BIP_PARAM_LIST_START_OFFSET = BIT4,
    BIP_PARAM_LATEST_CAPTURED_IMAGES = BIT5,
    BIP_PARAM_PARTIAL_FILE_LENGTH = BIT6,
    BIP_PARAM_PARTIAL_FILE_START_OFFSET = BIT7,
    BIP_PARAM_TOTAL_FILE_SIZE = BIT8,
    BIP_PARAM_END_FLAG = BIT9,
    BIP_PARAM_REMOTE_DISPLAY = BIT10,
    BIP_PARAM_SERVICE_ID = BIT11,
    BIP_PARAM_STORE_FLAG = BIT12,
    BIP_PARAM_DATA = BIT13,
    BIP_PARAM_LENGTH = BIT14,

    BIP_PARAM_TYPE = BIT15
} BIP_PARAM_FLAGS;

typedef struct {
    BIP_PARAM_FLAGS flags;
    
    OI_OBEX_BYTESEQ *type;
    OI_OBEX_UNICODE *name;
    OI_OBEX_BYTESEQ *descriptor;
    OI_OBEX_UNICODE *handle;
    OI_OBEX_BYTESEQ *params;
    OI_UINT32 length;
    OI_OBEX_BYTESEQ *body;
    OI_BOOL final;
    
    OI_UINT16 nbReturnedHandles;
    OI_UINT16 listStartOffset;
    OI_UINT8 latestCapturedImages;
    OI_UINT32 partialFileLength;
    OI_UINT32 partialFileStartOffset;
    OI_UINT32 totalFileSize;
    OI_UINT8 endFlag;
    OI_UINT8 remoteDisplay;
    OI_UUID128 serviceId;
    OI_UINT8 storeFlag;
} BIP_REQUEST_HEADERS;

void OI_BIP_FreeHeaders(BIP_REQUEST_HEADERS *reqHeaders);

OI_STATUS OI_BIP_UnmarshallHeaderData(BIP_REQUEST_HEADERS *reqHeaders,
                                      OI_OBEX_HEADER_LIST *obexHeaders);

typedef union {
    OI_BIP_GET_CAPABILITIES_PARAMS getCapabilities;
    OI_BIP_GET_MONITORING_IMAGE_PARAMS getMonitoringImage;
    OI_BIP_PUT_IMAGE_PARAMS putImage;
    OI_BIP_PUT_LINKED_THUMBNAIL_PARAMS putLinkedThumbnail;
    OI_BIP_PUT_LINKED_ATTACHMENT_PARAMS putLinkedAttachment;
    OI_BIP_GET_IMAGES_LIST_PARAMS getImagesList;
    OI_BIP_GET_IMAGE_PARAMS getImage;
    OI_BIP_GET_LINKED_THUMBNAIL_PARAMS getLinkedThumbnail;
    OI_BIP_GET_LINKED_ATTACHMENT_PARAMS getLinkedAttachment;
    OI_BIP_START_PRINT_PARAMS startPrint;
    OI_BIP_START_ARCHIVE_PARAMS startArchive;
    OI_BIP_DELETE_IMAGE_PARAMS deleteImage;
    OI_BIP_GET_PARTIAL_IMAGE_PARAMS getPartialImage;
    OI_BIP_GET_IMAGE_PROPERTIES_PARAMS getImageProperties;
    OI_BIP_REMOTE_DISPLAY_PARAMS remoteDisplay;
} BIP_PARAM_UNION;

typedef enum {
    BIPSRV_IMAGING_RESPONDER,
    BIPSRV_REFERENCED_OBJECTS,
    BIPSRV_ARCHIVED_OBJECTS
} BIPSRV_SERVER_TYPE;


/**
 * These constnats are used to map BIP function structure parameters to
 * their underlying OBEX header representations.
 */
enum {
    OI_BIP_OBEX_HDR_IMG_HANDLE = 0x30,
    OI_BIP_OBEX_HDR_IMG_DESCRIPTOR = 0x71,

    OI_BIP_OBEX_APP_PARAM_NB_RETURNED_HANDLES = 0x01,           /* UINT16 */
    OI_BIP_OBEX_APP_PARAM_LIST_START_OFFSET = 0x02,             /* UINT16 */
    OI_BIP_OBEX_APP_PARAM_LATEST_CAPTURED_IMAGES = 0x03,        /* UINT8 */
    OI_BIP_OBEX_APP_PARAM_PARTIAL_FILE_LENGTH = 0x04,           /* UINT32 */
    OI_BIP_OBEX_APP_PARAM_PARTIAL_FILE_START_OFFSET = 0x05,     /* UINT32 */
    OI_BIP_OBEX_APP_PARAM_TOTAL_FILE_SIZE = 0x06,               /* UINT32 */
    OI_BIP_OBEX_APP_PARAM_END_FLAG = 0x07,                      /* UINT8 */
    OI_BIP_OBEX_APP_PARAM_REMOTE_DISPLAY = 0x08,                /* UINT8 */
    OI_BIP_OBEX_APP_PARAM_SERVICE_ID = 0x09,                    /* UUID128 */
    OI_BIP_OBEX_APP_PARAM_STORE_FLAG = 0x0a                     /* UINT8 */
};

#define OI_BIP_BYTE_ORDER OI_BIG_ENDIAN_BYTE_ORDER

#define OI_BIP_IMAGE_PUSH_OBEX_TARGET_UUID \
    { 0xE3, 0x3D, 0x95, 0x45, 0x83, 0x74, 0x4A, 0xD7, 0x9E, 0xC5, 0xC1, 0x6B, 0xE3, 0x1E, 0xDE, 0x8E }

#define OI_BIP_IMAGE_PULL_OBEX_TARGET_UUID \
    { 0x8E, 0xE9, 0xB3, 0xD0, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_ADVANCED_IMAGE_PRINTING_OBEX_TARGET_UUID \
    { 0x92, 0x35, 0x33, 0x50, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_AUTOMATIC_ARCHIVE_OBEX_TARGET_UUID \
    { 0x94, 0x01, 0x26, 0xC0, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_REMOTE_CAMERA_OBEX_TARGET_UUID \
    { 0x94, 0x7E, 0x74, 0x20, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

#define OI_BIP_REMOTE_DISPLAY_OBEX_TARGET_UUID \
    { 0x94, 0xC7, 0xCD, 0x20, 0x46, 0x08, 0x11, 0xD5, 0x84, 0x1A, 0x00, 0x02, 0xA5, 0x32, 0x5B, 0x4E }

    
#define OI_BIP_REFERENCED_OBJECTS_OBEX_TARGET_UUID \
    { 0x8E, 0x61, 0xF9, 0x5D, 0x1A, 0x79, 0x11, 0xD4, 0x8E, 0xA4, 0x00, 0x80, 0x5F, 0x9B, 0x98, 0x34 }

#define OI_BIP_ARCHIVED_OBJECTS_OBEX_TARGET_UUID \
    { 0x8E, 0x61, 0xF9, 0x5E, 0x1A, 0x79, 0x11, 0xD4, 0x8E, 0xA4, 0x00, 0x80, 0x5F, 0x9B, 0x98, 0x34 }



#ifdef OI_DEBUG
#error code not present
#else
#define OI_BIP_CommandString(cmd)   ("")
#endif

#endif /* _OI_BIP_PRIVATE_H */

/**@}*/
