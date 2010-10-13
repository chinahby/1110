/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     B L U E T O O T H   P H O N E  B O O K   A C C E S S   P R O F I L E   
                                C O M M O N

GENERAL DESCRIPTION
  This module contains the Bluetooth Phone Book Access Profile data and code 
  common to client and server.

Copyright (c) 2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/pbap_private.c#1 $ 

$Log: $
*
*    #3        23 Oct 2006            GS
* Undoing change made in #2 so that OBEX continue response with no 
* body or end-of-body header will be handled correctly during
* GET operation.
*
*    #2        15 Jun 2006            GS
* Updated UnMarshallHeaderData() to default to 'final' packet if no
* End-of-Body header is present
*
*    #1        06 Feb 2006            GS
* Initial version
*

===========================================================================*/

#define __OI_MODULE__ OI_MODULE_PBAP_SRV

#include "pbap_spec.h"
#include "pbap_private.h"
#include "oi_obexcommon.h"   

#include "oi_memmgr.h"
#include "oi_assert.h"

#define DEFINE_PBAP_REQUEST_TYPE(name, str) \
static const OI_CHAR name##String[] = str; \
    const OI_OBEX_BYTESEQ OI_PBAP_##name = { OI_ARRAYSIZE(name##String), (OI_BYTE *)name##String }

DEFINE_PBAP_REQUEST_TYPE(PullPhoneBookType, PBAP_OBJ_TYPE_PHONE_BOOK);
DEFINE_PBAP_REQUEST_TYPE(PullvCardListingType, PBAP_OBJ_TYPE_VCARD_LISTING);
DEFINE_PBAP_REQUEST_TYPE(PullvCardEntryType, PBAP_OBJ_TYPE_VCARD_ENTRY);

void PBAP_InitAppParams(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len)
{
    ByteStream_Init(bk->bs, data, len);
    ByteStream_Open(bk->bs, BYTESTREAM_WRITE);
    bk->byteseq.data = data;
    bk->byteseq.len = 0;
}

void PBAP_AppendAppParamUINT8(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT8 value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(OI_UINT8));
    ByteStream_PutUINT8(bk->bs, value);
}

void PBAP_AppendAppParamUINT16(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT16 value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(OI_UINT16));
    ByteStream_PutUINT16(bk->bs, value, PBAP_BYTE_ORDER);
}

void PBAP_AppendAppParamUINT32(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UINT32 value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(OI_UINT32));
    ByteStream_PutUINT32(bk->bs, value, PBAP_BYTE_ORDER);
}

void PBAP_AppendAppParamUINT64(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, UINT64 value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(UINT64));
    ByteStream_PutUINT64(bk->bs, value, PBAP_BYTE_ORDER);
}

void PBAP_AppendAppParamUUID128(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_UUID128 *uuid)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, sizeof(uuid->ms32bits) + sizeof(uuid->base));
    ByteStream_PutUINT32(bk->bs, uuid->ms32bits, PBAP_BYTE_ORDER);
    ByteStream_PutBytes(bk->bs, uuid->base, sizeof(uuid->base));
}

void PBAP_AppendAppParamUTF8(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 tag, OI_OBEX_BYTESEQ *value)
{
    ByteStream_PutUINT8(bk->bs, tag);
    ByteStream_PutUINT8(bk->bs, value->len);
    ByteStream_PutBytes(bk->bs, value->data, value->len);
}

void PBAP_FinishAppParams(PBAP_APP_PARAM_BOOKKEEPING *bk)
{
    ByteStream_Close(bk->bs);
    bk->byteseq.len = ByteStream_GetSize(bk->bs);  
}

static OI_STATUS NextParam(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_UINT8 *tag, OI_UINT8 *len)
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

void PBAP_ResetAppParams(PBAP_APP_PARAM_BOOKKEEPING *bk, OI_BYTE *data, OI_UINT16 len)
{
    ByteStream_Init(bk->bs, data, len);
    ByteStream_Open(bk->bs, BYTESTREAM_READ);
    bk->nextTagPos = 0;
}

static OI_STATUS CopyParamData(OI_OBEX_BYTESEQ *paramData, PBAP_REQUEST_HEADERS *req)
{
    OI_STATUS status;
    PBAP_APP_PARAM_BOOKKEEPING bk;
    OI_UINT8 tag;
    OI_UINT8 len;

    PBAP_ResetAppParams(&bk, paramData->data, paramData->len);

    while (ByteStream_NumReadBytesAvail(bk.bs) > 0) {
        status = NextParam(&bk, &tag, &len);
        if (status == OI_STATUS_NOT_FOUND) {
            return OI_OK;
        }
        if (!OI_SUCCESS(status)) {
            return status;
        }
        switch (tag) {
            case PBAP_OBEX_APP_PARAM_ORDER:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->order);
                req->flags |= PBAP_PARAM_ORDER;
                break;
                
            case PBAP_OBEX_APP_PARAM_SEARCH_VALUE:
                /* haven't figured out a better way to do it, yet */
                req->value.data = (OI_BYTE *) ((bk.bs).__data + (bk.bs).__pos);
                req->value.len  = len;
                (bk.bs).__pos += len; 

                req->flags |= PBAP_PARAM_VALUE;
                break;
                
            case PBAP_OBEX_APP_PARAM_SEARCH_ATTRIBUTE:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->attribute);
                req->flags |= PBAP_PARAM_ATTRIBUTE;
                break;
                
            case PBAP_OBEX_APP_PARAM_MAX_LIST_COUNT:
                if (len != sizeof(OI_UINT16)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT16(bk.bs, req->listCount, 
                                     OI_BIG_ENDIAN_BYTE_ORDER);
                req->flags |= PBAP_PARAM_MAX_LIST_COUNT;
                break;
                
            case PBAP_OBEX_APP_PARAM_LIST_START_OFFSET:
                if (len != sizeof(OI_UINT16)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT16(bk.bs, req->startOffset, 
                                     OI_BIG_ENDIAN_BYTE_ORDER);
                req->flags |= PBAP_PARAM_LIST_START_OFFSET;
                break;
                
            case PBAP_OBEX_APP_PARAM_FILTER:
                if (len != sizeof(UINT64)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT64(bk.bs, req->filter, 
                                     OI_BIG_ENDIAN_BYTE_ORDER);
                req->flags |= PBAP_PARAM_FILTER;
                break;

            case PBAP_OBEX_APP_PARAM_FORMAT:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->format);
                req->flags |= PBAP_PARAM_FORMAT;
                break;

            case PBAP_OBEX_APP_PARAM_PHONE_BOOK_SIZE:
                if (len != sizeof(OI_UINT16)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT16(bk.bs, req->phoneBookSize, 
                                     OI_BIG_ENDIAN_BYTE_ORDER);
                req->flags |= PBAP_PARAM_PHONE_BOOK_SIZE;
                break;

            case PBAP_OBEX_APP_PARAM_NEW_MISSED_CALLS:
                if (len != sizeof(OI_UINT8)) {
                    return OI_STATUS_INVALID_PACKET;
                }
                ByteStream_GetUINT8(bk.bs, req->missedCalls);
                req->flags |= PBAP_PARAM_NEW_MISSED_CALLS;
                break;
        }

    }

    return OI_OK;
}

OI_STATUS PBAP_UnmarshallHeaderData(PBAP_REQUEST_HEADERS *reqHeaders,
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
                    if (!(reqHeaders->flags & PBAP_PARAM_TYPE)) {
                        reqHeaders->type = &hdr->val.type;
                        reqHeaders->flags |= PBAP_PARAM_TYPE;
                    }
                    break;

                case OI_OBEX_HDR_NAME:
                    if (!(reqHeaders->flags & PBAP_PARAM_NAME)) {
                        reqHeaders->name = &hdr->val.unicode;
                        reqHeaders->flags |= PBAP_PARAM_NAME;
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
                    reqHeaders->length = hdr->val.length;
                    reqHeaders->flags |= PBAP_PARAM_LENGTH;
                    break;
                case OI_OBEX_HDR_BODY:
                    reqHeaders->body = &hdr->val.byteseq;
                    break;
                case OI_OBEX_HDR_END_OF_BODY:
                    reqHeaders->body = &hdr->val.byteseq;
                    break;
            }
        }
    }
    return OI_OK;
}


