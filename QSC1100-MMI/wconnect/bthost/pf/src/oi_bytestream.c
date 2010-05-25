/**
 * @file
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#define __OI_MODULE__ OI_MODULE_MEMMGR

#include "oi_core_common.h"
#include "oi_bytestream.h"

OI_BYTE_STREAM *ByteStream_Alloc(OI_UINT16 len)
{
    OI_BYTE *buf;
    OI_BYTE_STREAM *bs;

    buf = OI_Malloc(len);
    if (NULL == buf) return NULL;

    bs = OI_Malloc(sizeof(OI_BYTE_STREAM));
    if (bs != NULL) {
        ByteStream_Init(*bs, buf, len);
    }
    else {
        OI_Free(buf);
    }
    return bs;
}

void ByteStream_Free(OI_BYTE_STREAM *bs) 
{
    OI_ASSERT(bs);

    OI_Free(ByteStream_GetDataPointer(*bs));
    OI_Free(bs);
}

#include "oi_core_common.h"

#ifdef OI_DEBUG  //ByteStream_Print only used by internal test programs
#error code not present
#endif

OI_STATUS ByteStream_Parse(OI_BYTE_STREAM *bs,
                           OI_INT byteorder,
                           const OI_CHAR format[],
                           OI_UINT formatLen,
                           ...)
{
    va_list list;
    OI_UINT i;
    OI_BYTE *data;
    OI_BD_ADDR *addr;
    OI_STATUS result = OI_OK;
    OI_UINT8 *u8;
    OI_UINT16 *u16;
    OI_UINT32 *u32;
    OI_UUID128 *uuid128;

    OI_ASSERT(bs);
    OI_ASSERT(ByteStream_GetDataPointer(*bs));

    va_start(list, formatLen);
    for (i = 0; i < formatLen; i++) {
        switch (format[i]) {
            case TYPE_INT8:
            case TYPE_UINT8:
                u8 = va_arg(list, OI_UINT8 *);
                ByteStream_GetUINT8_Checked(*bs, *u8);
                break;
            case TYPE_INT16:
            case TYPE_UINT16:
            case TYPE_UUID16:
                u16 = va_arg(list, OI_UINT16 *);
                ByteStream_GetUINT16_Checked(*bs, *u16, byteorder);
                break;
            case TYPE_INT24:
            case TYPE_UINT24:
                u32 = va_arg(list, OI_UINT32 *);
                ByteStream_GetUINT24_Checked(*bs, *u32, byteorder);
                break;
            case TYPE_INT32:
            case TYPE_UINT32:
            case TYPE_UUID32:
                u32 = va_arg(list, OI_UINT32 *);
                ByteStream_GetUINT32_Checked(*bs, *u32, byteorder);
                break;
            case TYPE_DATA:
                //This only works if the length field has already been assigned
                data = va_arg(list, OI_BYTE *);
                u16 = va_arg(list, OI_UINT16 *);
                ByteStream_GetBytes_Checked(*bs, data, (OI_UINT16)*u16);
                break;
            case TYPE_BD_ADDR:
                addr = va_arg(list, OI_BD_ADDR *);
                ByteStream_GetBDADDR_Checked(*bs, *addr, byteorder);
                break;
            case TYPE_UUID128:
                uuid128 = va_arg(list, OI_UUID128 *);
                ByteStream_GetUINT32_Checked(*bs, uuid128->ms32bits, OI_BIG_ENDIAN_BYTE_ORDER);
                ByteStream_GetBytes_Checked(*bs, uuid128->base, OI_BT_UUID128_SIZE - sizeof(OI_UINT32));
                break;
            case TYPE_IPV6_ADDR:
                {
                    void *p = va_arg(list, void *);
                    ByteStream_GetBytes_Checked(*bs, p, OI_BT_IPV6ADDR_SIZE);
                }
                break;
            default:
                OI_ASSERT(FALSE);
                break;
        }
    }
    va_end(list);

    if (ByteStream_Error(*bs)) {
        result = OI_FAIL;
    }
    return result;
}
