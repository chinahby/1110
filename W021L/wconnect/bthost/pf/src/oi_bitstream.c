#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/**
@file
Functions for manipulating input and output bitstreams.

@ingroup esbc_lib
*/

///@addtogroup esbc_lib
///@{

#include "oi_stddefs.h"
#include "oi_bitstream.h"
#include "oi_assert.h"

PRIVATE void OI_BITSTREAM_ReadInit(OI_BITSTREAM *bs,
                                   OI_BYTE *buffer)
{
    bs->ptr = buffer;
    bs->value = ((OI_INT32)bs->ptr[0] << 16) | ((OI_INT32)bs->ptr[1] << 8) | (bs->ptr[2]);
    bs->ptr += 3;
    bs->bitPtr = 8;
}

PRIVATE void OI_BITSTREAM_WriteInit(OI_BITSTREAM *bs,
                                   OI_BYTE *buffer)
{
    bs->ptr = buffer;
    bs->value = 0;
    bs->bitPtr = 32;
}

#ifdef SBC_DECODER

PRIVATE OI_UINT32 OI_BITSTREAM_ReadUINT(OI_BITSTREAM *bs, OI_UINT bits)
{
    OI_UINT32 result;

    OI_BITSTREAM_READUINT(result, bits, bs->ptr, bs->value, bs->bitPtr);

    return result;
}

PRIVATE OI_UINT8 OI_BITSTREAM_ReadUINT4Aligned(OI_BITSTREAM *bs)
{
    OI_UINT32 result;

    OI_ASSERT(bs->bitPtr < 16);
    OI_ASSERT(bs->bitPtr % 4 == 0);

    if (bs->bitPtr == 8) {
        result = bs->value << 8;
        bs->bitPtr = 12;
    } else {
        result = bs->value << 12;
        bs->value = (bs->value << 8) | *bs->ptr++;
        bs->bitPtr = 8;
    }
    result >>= 28;
    OI_ASSERT(result < (1u << 4));
    return (OI_UINT8)result;
}

PRIVATE OI_UINT8 OI_BITSTREAM_ReadUINT8Aligned(OI_BITSTREAM *bs)
{
    OI_UINT32 result;
    OI_ASSERT(bs->bitPtr == 8);

    result = bs->value >> 16;
    bs->value = (bs->value << 8) | *bs->ptr++;

    return (OI_UINT8)result;
}

#endif

#ifdef SBC_ENCODER

void OI_BITSTREAM_WriteUINT8Aligned(OI_BITSTREAM *bs,
                                    OI_UINT8 datum)
{
    OI_ASSERT((bs->bitPtr & 7) == 0);
    *bs->ptr++ = datum;
}

void OI_BITSTREAM_Write2xUINT4Aligned(OI_BITSTREAM *bs,
                                      OI_UINT8 datum1,
                                      OI_UINT8 datum2)
{
    OI_ASSERT(datum1 < 16);
    OI_ASSERT(datum2 < 16);
    OI_ASSERT((bs->bitPtr & 7) == 0);

    *bs->ptr++ = ((datum1 << 4) | datum2);
}

void OI_BITSTREAM_WriteUINT(OI_BITSTREAM *bs,
                            OI_UINT16 datum,
                            OI_UINT bits)
{
    OI_BITSTREAM_WRITEUINT(bs->ptr, bs->value, bs->bitPtr, datum, bits);

/*    bs->value |= datum << (32 - bs->bitPtr - bits);
    bs->bitPtr += bits;

    while (bs->bitPtr >= 8) {
        bs->bitPtr -= 8;
        *bs->ptr++ = (OI_UINT8)(bs->value >> 24);
        bs->value <<= 8;
    }
    */
}

void OI_BITSTREAM_WriteFlush(OI_BITSTREAM *bs)
{
    OI_BITSTREAM_WRITEFLUSH(bs->ptr, bs->value, bs->bitPtr);
}

#endif

///@}
#endif /* FEATURE_BT_EXTPF_AV */
