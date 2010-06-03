#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_codec_sbc_private.h"

/** @file
 *
 * This file performs quantization of subband values according to the results of
 * the bit allocation algorithm.
 *
 * The quantized value is determined as follows. (The equations below should be
 * treated as floating point).
 *
 * q = floor( ((value / scalefactor) + 1.0) * levels / 2.0)
 *
 * where scalefactor = 2^(scale_factor + 1) and levels = (2^bits) - 1
 *
 * By the definition of scale_factor, abs(value) < scalefactor. Therefore:
 *
 *      -1.0 < (value / scalefactor) < 1.0
 *      0 < (value / scalefactor + 1.0) < 2.0
 *      0 < ((value / scalefactor + 1.0) / 2) < 1.0
 *      0 < ((value / scalefactor + 1.0) / 2) * levels < levels
 *      0 <= q <= levels - 1
 *
 *
 * Integerization proceeds as follows:
 *
 *
 * 1) Normalize the value:
 *
 * Taking the floating point subexpression:
 *
 *    (value / scalefactor + 1.0)
 *
 * And muliplying by scalefactor gives
 *
 *     scalefactor * (value / scalefactor + 1.0) = value + scalefactor
 *
 * Recalling that scalefactor 2^(scale_factor + 1) = (1 << (scalefactor + 1) we can rewwrite the above
 * using integer arithmetic as:
 *
 *     norm = value + (1 << (scale_factor + 1))
 *
 * 2) Multuply normalized value by (levels / 2.0) where
 *
 *     levels = (2^bits) - 1 = ((1 << bits) - 1)
 *
 * So the product
 *
 *     norm * levels = (norm << bits) - norm
 *
 * Therefore
 *
 *     norm * levels / 2 = ((norm << bits) - norm) >> 1
 *
 * However, norm was scaled up by scalefactor in 1) where scalefactor = 2^(scale_factor + 1) so
 * shifting the product right by (scale_factor + 1) undoes the scaling so the result is
 *
 *    (((norm << bits) - norm) >> 1) >> (scale_factor + 1)
 *
 * or
 *
 *    ((norm << bits) - norm) >> (scale_factor + 2)
 *
 * However, (scale_factor + bits) == 31 the left shift by bits can overflow a 32 bit value losing
 * the msb. The overflow case is easily detected by testing if shifting
 * norm lefy by (bits - 1) sets the msb. In this case norm shifted right one bit prior to
 * calculating the result:
 *
 *    norm = norm >> 1
 *    ((norm << bits) - norm) >> (scale_factor + 1)
 *
 * No precision is lost because the lsb would have been shifted out in any case.
 *
 * @ingroup esbc_lib
 */

///@addtogroup esbc_lib
///@{

#ifdef DEBUG_QUANTIZATION

#include <math.h>

INLINE OI_UINT16 quant_double(OI_INT value, long scale_factor, long bits) /* Only for testing purposes */
{
    OI_INT scalefactor;
    OI_INT levels;

    scalefactor = 1 << (scale_factor + 1);
    levels = (1 << bits) - 1;
    return (OI_UINT16)floor(((double)value/(double)scalefactor+1.0)*(double)levels/2.0);
}
#endif

INLINE OI_UINT16 quant_fixed(OI_INT value, long scale_factor, long bits) /* This should now be identical to quant_double */
{
    OI_UINT sf = scale_factor + 1;
    OI_UINT32 norm = value + (1 << sf);
    OI_UINT32 q;

    OI_ASSERT(bits > 0);
    OI_ASSERT(scale_factor >= 0);
    OI_ASSERT(scale_factor <= 15);

    /* Check for overflow */
    if ((norm << (bits - 1)) & 0x80000000) {
        norm >>= 1;
    } else {
        ++sf;
    }
    q = (((norm << bits) - norm) >> sf);

#ifdef DEBUG_QUANTIZATION
    OI_ASSERT(q >= OI_UINT16_MIN && q <= OI_UINT16_MAX);
    OI_ASSERT(q < ((1u<<bits)-1u));
    OI_ASSERT(q == quant_double(value, scale_factor, bits));
#endif

    return (OI_UINT16)q;
}


INLINE OI_UINT16 OI_SBC_Quant(OI_INT32 value, OI_UINT scale_factor, OI_UINT bits)
{
    return QUANT(value, scale_factor, bits);
}

///@}
#endif /* FEATURE_BT_EXTPF_AV */
