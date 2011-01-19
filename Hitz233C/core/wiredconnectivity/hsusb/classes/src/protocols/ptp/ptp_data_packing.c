/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include <jos.h>
#include "ptp_data_packing.h"
#include "ptp_utils.h"

void pack_u8(void **buf, juint8_t u8)
{
    juint8_t **p = (juint8_t**)buf;
    **p = u8;
    (*p)++;
}

void pack_u16(void **buf, juint16_t u16)
{
    juint16_t **p = (juint16_t**)buf;
    PTP_H2T16(*p, u16);
    (*p)++;
}

void pack_u32(void **buf, juint32_t u32)
{
    juint32_t **p = (juint32_t**)buf;
    PTP_H2T32(*p, u32);
    (*p)++;
}

void pack_u64(void **buf, ptp_uint64_t *u64)
{
#ifdef PTP_TRANSPORT_BIG_ENDIAN
    pack_u32(buf, u64->high);
    pack_u32(buf, u64->low);
#else
    pack_u32(buf, u64->low);
    pack_u32(buf, u64->high);
#endif
}

void pack_u128(void **buf, ptp_uint128_t *u128)
{
#ifdef PTP_TRANSPORT_BIG_ENDIAN
    pack_u64(buf, &u128->high);
    pack_u64(buf, &u128->low);
#else
    pack_u64(buf, &u128->low);
    pack_u64(buf, &u128->high);
#endif
}

juint32_t pack_str(void **buf, juint32_t max_size /* In bytes */,
    wchar_t *in_str)
{
    void *s_buf;
    juint8_t num_chars = 0;

    /* Handle empty string */
    if (!in_str || !(*in_str))
    {
        pack_u8(buf, num_chars);
        return sizeof(juint8_t);
    }
    
    s_buf = *buf;    /* Save original pointer (for count) */
    *(juint8_t**)buf += 1;       /* Leave room for element count */

    for(; num_chars < PTP_MAX_STRING_LEN &&          /* Limit string size */
        PTP_STR_PACKED_SIZE(num_chars) < max_size && /* Dont overflow buffer */
        *in_str;                                     /* End of string? */
        in_str++, num_chars++)
    {
        pack_u16(buf, (juint16_t)*in_str);
    }

    /* Null terminate the string, if it exists */
    if (num_chars)
    {
        pack_u16(buf, 0);
        num_chars++;
    }

    pack_u8(&s_buf, num_chars);
    return PTP_STR_PACKED_SIZE(num_chars);
}

juint8_t unpack_u8(void **buf)
{
    juint8_t **p = (juint8_t**)buf;
    juint8_t u8 = **p;
    (*p)++;
    return u8;
}

juint16_t unpack_u16(void **buf)
{
    juint16_t **p = (juint16_t**)buf;
    juint16_t u16 = PTP_T2H16(*p);
    (*p)++;
    return u16;
}

juint32_t unpack_u32(void **buf)
{
    juint32_t **p = (juint32_t**)buf;
    juint32_t u32 = PTP_T2H32(*p);
    (*p)++;
    return u32;
}

ptp_uint64_t unpack_u64(void **buf)
{
    ptp_uint64_t u64;
    
#ifdef PTP_TRANSPORT_BIG_ENDIAN
    u64.high = unpack_u32(buf);
    u64.low = unpack_u32(buf);
#else
    u64.low = unpack_u32(buf);
    u64.high = unpack_u32(buf);
#endif

    return u64;
}

ptp_uint128_t unpack_u128(void **buf)
{
    ptp_uint128_t u128;
    
#ifdef PTP_TRANSPORT_BIG_ENDIAN
    u128.high = unpack_u64(buf);
    u128.low = unpack_u64(buf);
#else
    u128.low = unpack_u64(buf);
    u128.high = unpack_u64(buf);
#endif

    return u128;
}

juint32_t unpack_str(void **buf, wchar_t **out_str)
{
    juint8_t num_chars;
    wchar_t *str;

    num_chars = unpack_u8(buf);
    if (!num_chars)
    {
        /* Return NULL when given an empty PTP string */
        *out_str = NULL;
        return 0;
    }

    str = (wchar_t*)jmalloc(num_chars*sizeof(wchar_t), M_ZERO);
    if (!str)
    {
        *out_str = NULL;
        DBG_E(DPROTO_PTP, ("unpack_str: Error allocating memory"
            " for string\n"));
        return 0;
    }

    *out_str = str;
    while(num_chars--)
    {
        *str = unpack_u16(buf);
        str++;
    }

    /* Place a terminating NULL, if one does not exist */
    str--;
    if (*str != 0)
        *str = 0;

    return num_chars*sizeof(wchar_t);
}

void unpack_objinfo(void *buf, ptp_obj_info_t *objinfo)
{
    objinfo->storage_id = (ptp_storage_id_t) unpack_u32(&buf);
    objinfo->format = (ptp_obj_format_t) unpack_u16(&buf);
    objinfo->prot_status = (enum ptp_protection_status) unpack_u16(&buf);
    objinfo->compressed_size = unpack_u32(&buf);
    objinfo->thumb_format = (ptp_obj_format_t)unpack_u16(&buf);
    objinfo->thumb_compressed_size = unpack_u32(&buf);
    objinfo->thumb_width = unpack_u32(&buf);
    objinfo->thumb_height = unpack_u32(&buf);
    objinfo->width = unpack_u32(&buf);
    objinfo->height = unpack_u32(&buf);
    objinfo->bit_depth = unpack_u32(&buf);
    objinfo->parent_object = (ptp_obj_handle_t) unpack_u32(&buf);
    objinfo->association_type = (enum ptp_association) unpack_u16(&buf);
    objinfo->association_desc = unpack_u32(&buf);
    objinfo->sequence_number = unpack_u32(&buf);
    unpack_str(&buf, &objinfo->filename);
    unpack_str(&buf, &objinfo->capture_date);
    unpack_str(&buf, &objinfo->modification_date);
    unpack_str(&buf, &objinfo->keywords);
}

#define ARRAY_SIZE(elem_size, elem_count)      \
    (sizeof(juint32_t) + (elem_count) * (elem_size))

juint32_t calc_packed_size(ptp_type_t type, juint32_t count)
{
    juint32_t rc = 0;

    switch (type)
    {
    case PTP_TYPE_AINT8:
    case PTP_TYPE_AUINT8:
        rc = ARRAY_SIZE(sizeof(juint8_t), count);
        break;
        
    case PTP_TYPE_INT8:
    case PTP_TYPE_UINT8:
        rc = sizeof(juint8_t);
        break;
        
    case PTP_TYPE_AINT16:
    case PTP_TYPE_AUINT16:
        rc = ARRAY_SIZE(sizeof(juint16_t), count);
        break;
        
    case PTP_TYPE_INT16:
    case PTP_TYPE_UINT16:
        rc = sizeof(juint16_t);
        break;
        
    case PTP_TYPE_AINT32:
    case PTP_TYPE_AUINT32:
        rc = ARRAY_SIZE(sizeof(juint32_t), count);
        break;
        
    case PTP_TYPE_INT32:
    case PTP_TYPE_UINT32:
        rc = sizeof(juint32_t);
        break;
        
    case PTP_TYPE_AINT64:
    case PTP_TYPE_AUINT64:
        rc = ARRAY_SIZE(PTP_UINT64_PACKED_SIZE, count);
        break;
        
    case PTP_TYPE_INT64:
    case PTP_TYPE_UINT64:
        rc = PTP_UINT64_PACKED_SIZE;
        break;
        
    case PTP_TYPE_AINT128:
    case PTP_TYPE_AUINT128:
        rc = ARRAY_SIZE(PTP_UINT128_PACKED_SIZE, count);
        break;
        
    case PTP_TYPE_INT128:
    case PTP_TYPE_UINT128:
        rc = PTP_UINT128_PACKED_SIZE;
        break;
        
    case PTP_TYPE_STR:
        rc = PTP_STR_PACKED_SIZE(count);
        break;
        
    default: /* including PTP_TYPE_UNDEF */
        DBG_E(DPROTO_PTP, ("calc_packed_size: unsupported type (%x)\n",
            type));
        break;
    }

    return rc;
}

#undef ARRAY_SIZE

juint32_t pack_prop_value(ptp_type_t type, void **buf, juint32_t buf_size,
    ptp_prop_value_t *value)
{
    juint32_t pack_bytes = 0;
    juint32_t i;
    DECLARE_FNAME("pack_prop_value");

    DBG_X(DPROTO_PTP, ("%s: entered , size=%d\n", fname, buf_size));

    /* Precalculate how many bytes need to be packed */
    pack_bytes = calc_packed_size(type, (type == PTP_TYPE_STR) ?
        ptp_string_len(value->data.string)+1 :
        value->num_elems);

    /* on start of partial transaction pack the elemenet count (array header).
     * in ongoing partial transaction calc out the header */
    if (PTP_IS_TYPE_ARRAY(type))
    { 
        if (value->elem_offset)
            pack_bytes -= sizeof(juint32_t);
        else
            pack_u32(buf, value->total_elems);
    }

    /* Verify we have enough room */
    if (buf_size < pack_bytes)
    {
        DBG_E(DPROTO_PTP, ("%s: buf_size too small (%d<%d)\n", fname,
            buf_size, pack_bytes));
        return 0;
    }

    switch (type)
    {
    case PTP_TYPE_INT8:
    case PTP_TYPE_UINT8:
        pack_u8(buf, value->data.uint8);
        break;
        
    case PTP_TYPE_INT16:
    case PTP_TYPE_UINT16:
        pack_u16(buf, value->data.uint16);
        break;
        
    case PTP_TYPE_INT32:
    case PTP_TYPE_UINT32:
        pack_u32(buf, value->data.uint32);
        break;
        
    case PTP_TYPE_AINT8:
    case PTP_TYPE_AUINT8:
        for (i = 0 ; i < value->num_elems ; i++)
            pack_u8(buf, value->data.a_uint8[i]);
        break;
        
    case PTP_TYPE_AINT16:
    case PTP_TYPE_AUINT16:
        for (i = 0 ; i < value->num_elems ; i++)
            pack_u16(buf, value->data.a_uint16[i]);
        break;
        
    case PTP_TYPE_AINT32:
    case PTP_TYPE_AUINT32:
        for (i = 0 ; i < value->num_elems ; i++)
            pack_u32(buf, value->data.a_uint32[i]);
        break;
        
    case PTP_TYPE_INT64:
    case PTP_TYPE_UINT64:
        pack_u64(buf, &value->data.uint64);
        break;
        
    case PTP_TYPE_AINT64:
    case PTP_TYPE_AUINT64:
        for (i = 0 ; i < value->num_elems ; i++)
            pack_u64(buf, &value->data.a_uint64[i]);
        break;
        
    case PTP_TYPE_INT128:
    case PTP_TYPE_UINT128:
        pack_u128(buf, &value->data.uint128);
        break;
        
    case PTP_TYPE_AINT128:
    case PTP_TYPE_AUINT128:
        for (i = 0 ; i < value->num_elems ; i++)
            pack_u128(buf, &value->data.a_uint128[i]);
        break;

    case PTP_TYPE_STR:
        pack_bytes = pack_str(buf, buf_size, value->data.string);
        break;

    default: /* including PTP_TYPE_UNDEF */
        DBG_E(DPROTO_PTP, ("%s: unsupported type (%x)\n",
            fname, type));
        break;
    }

    DBG_X(DPROTO_PTP, ("%s: packed %d bytes\n", fname, pack_bytes));
    return pack_bytes;
} 

static void unpack_value(juint32_t size, void *dstbuf, void **srcbuf)
{
    switch (size)
    {
    case sizeof(juint8_t):
        *(juint8_t *)dstbuf = unpack_u8(srcbuf);
        break;
    case sizeof(juint16_t):
        *(juint16_t *)dstbuf = unpack_u16(srcbuf);
        break;
    case sizeof(juint32_t):
        *(juint32_t *)dstbuf = unpack_u32(srcbuf);
        break;
    case PTP_UINT64_PACKED_SIZE:
        *(ptp_uint64_t *)dstbuf = unpack_u64(srcbuf);
        break;
    case PTP_UINT128_PACKED_SIZE:
        *(ptp_uint128_t *)dstbuf = unpack_u128(srcbuf);
        break;
    default:
        DBG_E(DPROTO_PTP, ("unpack_value: error - invalid size (%u)\n",
            size));
        break;
    }
}

static juint32_t unpack_prop_array(juint32_t elem_size, void *buf,
    juint32_t buf_size, ptp_prop_value_t *value, void *res_buf, juint32_t *res)
{
    void **vbuf = (void **)&buf;
    juint8_t *dst = value->data.a_uint8;
    juint32_t count = 0;
    juint_t elem_count = 0;
    DECLARE_FNAME("unpack_prop_array");

    DBG_X(DPROTO_PTP, ("%s: entered , size[%d]\n", fname, elem_size));

    /* if no total_elems yet filled (from the calling function) 
     * unpack the size */
    if (value->total_elems == 0)
    {
        value->total_elems = unpack_u32(&buf);
        buf_size -= sizeof(juint32_t);
    }

    /* handle the residue situation */
    if (res && *res)
    {
        juint_t remainder = elem_size - *res;
        if (!value->total_elems)
        {
            DBG_E(DPROTO_PTP, ("%s: residue exist but this is the first chunk "
                "of array value \n", fname));
            goto Error;
        }
        
        /* copy to res_buf the remainder of the element which received in 
         * previos transaction and then unpack it to the array */
        j_memcpy(res_buf, buf, remainder);
        *(juint8_t**)vbuf += remainder;
        buf_size -= remainder;

        unpack_value(elem_size, dst, &res_buf);
        
        count++;
    }

    DBG_V(DPROTO_PTP, ("%s: buf_size[%d]\n", fname, buf_size));

    elem_count = buf_size / elem_size;
    for ( ; count < elem_count ; count++)
        unpack_value(elem_size, &dst[(count * elem_size)], &buf);

    if (res)
    {
        *res = buf_size % elem_size;
        if (*res)
            j_memcpy(buf, res_buf, *res);
    }

    return count;

Error:
    return 0;

}

juint32_t unpack_prop_value(ptp_type_t type, void *buf, juint32_t buf_size,
    ptp_prop_value_t *value, void *out_buf, void *res_buf, juint32_t *res)
{
    DECLARE_FNAME("unpack_prop_value");

    value->data_type = type;

    if (type == PTP_TYPE_STR)
    {
        value->data.string = out_buf;
        return unpack_str(&buf, &value->data.string);
    }

    if (PTP_IS_TYPE_ARRAY(type))
    {
        value->data.a_uint8 = out_buf;
        return unpack_prop_array(PTP_GET_TYPE_SIZE(type), buf, buf_size, value,
            res_buf, res);
    }

    switch (type)
    {
    case PTP_TYPE_INT8:
    case PTP_TYPE_UINT8:  
        value->data.uint8 = unpack_u8(&buf);
        return sizeof(juint8_t);
    case PTP_TYPE_INT16:
    case PTP_TYPE_UINT16:
        value->data.uint16 = unpack_u16(&buf);
        return sizeof(juint16_t);
    case PTP_TYPE_INT32:
    case PTP_TYPE_UINT32:
        value->data.uint32 = unpack_u32(&buf);
        return sizeof(juint32_t);
    case PTP_TYPE_INT64:
    case PTP_TYPE_UINT64:
        value->data.uint64 = unpack_u64(&buf);
        return PTP_UINT64_PACKED_SIZE;
    case PTP_TYPE_INT128:
    case PTP_TYPE_UINT128:
        value->data.uint128 = unpack_u128(&buf);
        return PTP_UINT128_PACKED_SIZE;
    default: /* including PTP_TYPE_UNDEF */
        DBG_E(DPROTO_PTP, ("%s: unsupported type (%x)\n", fname, type));
        break;
    }

    return 0;
}

