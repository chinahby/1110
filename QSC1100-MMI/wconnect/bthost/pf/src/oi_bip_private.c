/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bip_private.c#1 $ 

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

  Basic Imaging Profile internals
  */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_BIP_SRV

#include "oi_bip_spec.h"
#include "oi_bip_private.h"
#include "oi_obexcommon.h"  

#include "oi_memmgr.h"
#include "oi_assert.h"


#define DEFINE_TARGET_SEQ(name, bytes) \
static const OI_BYTE name##Bytes[] = bytes; \
    const OI_OBEX_BYTESEQ OI_BIP_##name = { OI_ARRAYSIZE(name##Bytes), (OI_BYTE *)name##Bytes }

/* *************** Primary target UUIDs *******************/

DEFINE_TARGET_SEQ(ImagePushObexTargetUUID, OI_BIP_IMAGE_PUSH_OBEX_TARGET_UUID);
DEFINE_TARGET_SEQ(ImagePullObexTargetUUID, OI_BIP_IMAGE_PULL_OBEX_TARGET_UUID);
DEFINE_TARGET_SEQ(AdvancedImagePrintingObexTargetUUID, OI_BIP_ADVANCED_IMAGE_PRINTING_OBEX_TARGET_UUID);
DEFINE_TARGET_SEQ(AutomaticArchiveObexTargetUUID, OI_BIP_AUTOMATIC_ARCHIVE_OBEX_TARGET_UUID);
DEFINE_TARGET_SEQ(RemoteCameraObexTargetUUID, OI_BIP_REMOTE_CAMERA_OBEX_TARGET_UUID);
DEFINE_TARGET_SEQ(RemoteDisplayObexTargetUUID, OI_BIP_REMOTE_DISPLAY_OBEX_TARGET_UUID);

/* *************** Secondary target UUIDs *******************/

DEFINE_TARGET_SEQ(ReferencedObjectsObexTargetUUID, OI_BIP_REFERENCED_OBJECTS_OBEX_TARGET_UUID);
DEFINE_TARGET_SEQ(ArchivedObjectsObexTargetUUID, OI_BIP_ARCHIVED_OBJECTS_OBEX_TARGET_UUID);

/* **********************************************************/


#define DEFINE_REQUEST_TYPE(name, str) \
static const OI_CHAR name##String[] = str; \
    const OI_OBEX_BYTESEQ OI_BIP_##name = { OI_ARRAYSIZE(name##String), (OI_BYTE *)name##String }

DEFINE_REQUEST_TYPE(CapabilitiesType, "x-bt/img-capabilities");
DEFINE_REQUEST_TYPE(ImageListType, "x-bt/img-listing");
DEFINE_REQUEST_TYPE(ImageType, "x-bt/img-img");
DEFINE_REQUEST_TYPE(ThumbnailType, "x-bt/img-thm");
DEFINE_REQUEST_TYPE(AttachmentType, "x-bt/img-attachment");
DEFINE_REQUEST_TYPE(PartialType, "x-bt/img-partial");
DEFINE_REQUEST_TYPE(MonitoringType, "x-bt/img-monitoring");
DEFINE_REQUEST_TYPE(StatusType, "x-bt/img-status");
DEFINE_REQUEST_TYPE(DisplayType, "x-bt/img-display");
DEFINE_REQUEST_TYPE(PrintType, "x-bt/img-print");
DEFINE_REQUEST_TYPE(ArchiveType, "x-bt/img-archive");
DEFINE_REQUEST_TYPE(PropertiesType, "x-bt/img-properties");


void OI_BIP_InitAppParams(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len)
{
    ByteStream_Init(bk->bs, data, len);
    ByteStream_Open(bk->bs, BYTESTREAM_WRITE);
    bk->byteseq.data = data;
    bk->byteseq.len = 0;
}

void OI_BIP_AppendAppParamUINT8(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT8 value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(OI_UINT8));
    ByteStream_PutUINT8(bk->bs, value);
}

void OI_BIP_AppendAppParamUINT16(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT16 value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(OI_UINT16));
    ByteStream_PutUINT16(bk->bs, value, OI_BIP_BYTE_ORDER);
}

void OI_BIP_AppendAppParamUINT32(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT32 value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(OI_UINT32));
    ByteStream_PutUINT32(bk->bs, value, OI_BIP_BYTE_ORDER);
}

void OI_BIP_AppendAppParamUUID128(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UUID128 *uuid)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(uuid->ms32bits) + sizeof(uuid->base));
    ByteStream_PutUINT32(bk->bs, uuid->ms32bits, OI_BIP_BYTE_ORDER);
    ByteStream_PutBytes(bk->bs, uuid->base, sizeof(uuid->base));
}

void OI_BIP_FinishAppParams(OI_BIP_APP_PARAM_BOOKKEEPING *bk)
{
    ByteStream_Close(bk->bs);
    bk->byteseq.len = ByteStream_GetSize(bk->bs);
    OI_DBGPRINT(("AppParams: %@", bk->byteseq.data, bk->byteseq.len)); 
}

static OI_STATUS NextParam(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 *tag, OI_UINT8 *len)
{
    ByteStream_SetPos(bk->bs, bk->nextTagPos);
    
    if (ByteStream_NumReadBytesAvail(bk->bs) == 0) {
        return OI_STATUS_NOT_FOUND;
    } else if (ByteStream_Error(bk->bs)) {
        return OI_STATUS_INVALID_PACKET;
    }

    ByteStream_GetUINT8_Checked(bk->bs, *tag);         /* Parameter tag */
    ByteStream_GetUINT8_Checked(bk->bs, *len);         /* Parameter length */
    if (ByteStream_Error(bk->bs) || ByteStream_NumReadBytesAvail(bk->bs) < *len) {
        return OI_STATUS_INVALID_PACKET;
    }

    bk->nextTagPos = ByteStream_GetPos(bk->bs);
    bk->nextTagPos += *len;
    return OI_OK;
}

void OI_BIP_ResetAppParams(OI_BIP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len)
{
    ByteStream_Init(bk->bs, data, len);
    ByteStream_Open(bk->bs, BYTESTREAM_READ);
    bk->nextTagPos = 0;
}

static OI_STATUS CopyParamData(OI_OBEX_BYTESEQ *paramData, BIP_REQUEST_HEADERS *req)
{
    OI_STATUS status;
    OI_BIP_APP_PARAM_BOOKKEEPING bk;
    OI_UINT8 tag;
    OI_UINT8 len;

    OI_BIP_ResetAppParams(&bk, paramData->data, paramData->len);
    
    while (ByteStream_NumReadBytesAvail(bk.bs) > 0) {
        status = NextParam(&bk, &tag, &len);
        if (status == OI_STATUS_NOT_FOUND) {
            return OI_OK;
        }
        if (!OI_SUCCESS(status)) {
            return status;
        }
        switch (tag) {
            case OI_BIP_OBEX_APP_PARAM_NB_RETURNED_HANDLES:
                if (len != sizeof(OI_UINT16)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT16(bk.bs, req->nbReturnedHandles, OI_BIP_BYTE_ORDER);
                req->flags |= BIP_PARAM_NB_RETURNED_HANDLES;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_LIST_START_OFFSET:
                if (len != sizeof(OI_UINT16)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT16(bk.bs, req->listStartOffset, OI_BIP_BYTE_ORDER);
                req->flags |= BIP_PARAM_LIST_START_OFFSET;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_LATEST_CAPTURED_IMAGES:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->latestCapturedImages);
                req->flags |= BIP_PARAM_LATEST_CAPTURED_IMAGES;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_PARTIAL_FILE_LENGTH:
                if (len != sizeof(OI_UINT32)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT32(bk.bs, req->partialFileLength, OI_BIP_BYTE_ORDER);
                req->flags |= BIP_PARAM_PARTIAL_FILE_LENGTH;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_PARTIAL_FILE_START_OFFSET:
                if (len != sizeof(OI_UINT32)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT32(bk.bs, req->partialFileStartOffset, OI_BIP_BYTE_ORDER);
                req->flags |= BIP_PARAM_PARTIAL_FILE_START_OFFSET;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_TOTAL_FILE_SIZE:
                if (len != sizeof(OI_UINT32)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT32(bk.bs, req->totalFileSize, OI_BIP_BYTE_ORDER);
                req->flags |= BIP_PARAM_TOTAL_FILE_SIZE;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_END_FLAG:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->endFlag);
                req->flags |= BIP_PARAM_END_FLAG;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_REMOTE_DISPLAY:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->remoteDisplay);
                req->flags |= BIP_PARAM_REMOTE_DISPLAY;
                break;
                
            case OI_BIP_OBEX_APP_PARAM_SERVICE_ID:
                if (len != sizeof(OI_UUID128)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT32(bk.bs, req->serviceId.ms32bits, OI_BIP_BYTE_ORDER);
                ByteStream_GetBytes(bk.bs, req->serviceId.base, sizeof(req->serviceId.base));
                req->flags |= BIP_PARAM_SERVICE_ID;
                break;

            case OI_BIP_OBEX_APP_PARAM_STORE_FLAG:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->storeFlag);
                req->flags |= BIP_PARAM_STORE_FLAG;
                break;
        }

    }

    return OI_OK;
}

OI_STATUS OI_BIP_UnmarshallHeaderData(BIP_REQUEST_HEADERS *reqHeaders,
                                      OI_OBEX_HEADER_LIST *obexHeaders)
{
    OI_UINT i;
    OI_OBEX_HEADER *hdr;

    OI_MemZero(reqHeaders, sizeof(*reqHeaders));
    if (obexHeaders) {
        for (i = 0; i < obexHeaders->count; i++) {
            hdr = obexHeaders->list + i;

            switch (hdr->id) {
                case OI_OBEX_HDR_TYPE:
                    if (!(reqHeaders->flags & BIP_PARAM_TYPE)) {
                        OI_DBGPRINT2(("Type: %s", hdr->val.type.data));
                        reqHeaders->type = &hdr->val.type;
                        reqHeaders->flags |= BIP_PARAM_TYPE;
                    }
                    break;

                case OI_OBEX_HDR_NAME:
                    if (!(reqHeaders->flags & BIP_PARAM_NAME)) {
                        OI_DBGPRINT2(("Name: %S", hdr->val.unicode.str));
                        reqHeaders->name = &hdr->val.unicode;
                        reqHeaders->flags |= BIP_PARAM_NAME;
                    }
                    break;

                case OI_BIP_OBEX_HDR_IMG_DESCRIPTOR:
                    if (!(reqHeaders->flags & BIP_PARAM_DESCRIPTOR)) {
                        OI_DBGPRINT2(("Descriptor: %s", hdr->val.byteseq.data));
                        reqHeaders->descriptor = &hdr->val.byteseq;
                        reqHeaders->flags |= BIP_PARAM_DESCRIPTOR;
                    }
                    break;

                case OI_BIP_OBEX_HDR_IMG_HANDLE:
                    if (!(reqHeaders->flags & BIP_PARAM_HANDLE)) {
                        OI_DBGPRINT2(("Handle: %S", hdr->val.unicode.str));
                        reqHeaders->handle = &hdr->val.unicode;
                        reqHeaders->flags |= BIP_PARAM_HANDLE;
                    }
                    break;

                case OI_OBEX_HDR_APPLICATION_PARAMS:
                    {
                        OI_STATUS status;
                        status = CopyParamData(&hdr->val.applicationParams, reqHeaders);

                        if (!OI_SUCCESS(status)) {
                            return status;
                        }
                    }
                    break;

                case OI_OBEX_HDR_LENGTH:
                    OI_DBGPRINT2(("Length: %d", hdr->val.length));
                    reqHeaders->length = hdr->val.length;
                    reqHeaders->flags |= BIP_PARAM_LENGTH;
                    break;
                case OI_OBEX_HDR_BODY:
                    OI_DBGPRINT2(("Body (...)"));
                    reqHeaders->body = &hdr->val.byteseq;
                    reqHeaders->final = FALSE;
                    break;
                case OI_OBEX_HDR_END_OF_BODY:
                    OI_DBGPRINT2(("EndOfBody (...)"));
                    reqHeaders->body = &hdr->val.byteseq;
                    reqHeaders->final = TRUE;
                    break;
            }
        }
    }
    return OI_OK;
}

#ifdef OI_DEBUG
#error code not present
#endif

