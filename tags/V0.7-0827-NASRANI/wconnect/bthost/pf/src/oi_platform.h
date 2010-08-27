#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/**
 * @file
 *
 * Platform-neutral definitions for operations which may be usefully optimized
 * for specific platforms.
 *
 * @ingroup esbc_lib
 */

///@addtogroup esbc_lib
///@{

#ifndef _PLATFORM_H
#define _PLATFORM_H

/** If the compiler supports an intrinsic to pass an assertion to the optimizer,
 * this definition should be overridden to use it.
 */
#define OPT_HINT(x)

/** Used to inform the optimizer of the properties of the loop in the following
 * statement.
 */
#define OPT_LOOP_HINT(variable, min, max, mod) OPT_HINT(variable >= min); OPT_HINT(variable <= max); OPT_HINT(variable % mod == 0)

/**
 * Unrolled macro to copy 4 32-bit aligned 32-bit values backward in memory
 */
#define COPY4WORDS_BACK(_dest, _src)            \
    do {                                        \
            OI_INT32 _a, _b, _c, _d;            \
            _a = *--_src;                       \
            _b = *--_src;                       \
            _c = *--_src;                       \
            _d = *--_src;                       \
            *--_dest = _a;                      \
            *--_dest = _b;                      \
            *--_dest = _c;                      \
            *--_dest = _d;                      \
    } while (0)



/** This typically won't be overridden. However, it's useful when building the
 * library with one toolchain and the application with a different toolchain,
 * since the compilers' math libraries tend not to be compatible.
 */
#define DIVIDE(a, b) ((a) / (b))

/** Evaluates to the _bits least significant bits of _x
 */
#define LOWBITS(_x, _bits) ((_x) & ((1 << (_bits))-1))

/** Evaluates to the absolute value of _x */
#define ABS(_x) ((_x) < 0 ? -(_x) : (_x))

/** Evaluates to the larger of _x and _y */
#define MAX(_x, _y) ((_x) > (_y) ? (_x) : (_y))

/** Evaluates to 0 if _x is 0 or 1, otherwise to floor(log2(_x))
 * Alternatively, for _x > 0, returns k such that 2^k <= x < 2^(k+1)
 */
#define SCALEFAC(_x) scalefac(_x)

/** Scales x by y bits to the right, adding a rounding factor.
 */
#define SCALE(x, y) (((x) + (1 <<((y)-1))) >> (y))

/** Returns x, saturating to the valid range of a signed 16-bit integer */
#define CLIP_INT16(x) do { if (x > OI_INT16_MAX) { x = OI_INT16_MAX; } else if (x < OI_INT16_MIN) { x = OI_INT16_MIN; } } while (0)

#define COPY_BACKWARD_32BIT_ALIGNED_40_HALFWORDS(dest, src) do { shift_buffer(dest, src, 40); } while (0)
#define COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS(dest, src) do { shift_buffer(dest, src, 72); } while (0)
#define COPY_BACKWARD_32BIT_ALIGNED_104_HALFWORDS(dest, src) do { shift_buffer(dest, src, 104); } while (0)

#define QUANT(value, scale_factor, bits) quant_fixed(value, scale_factor,bits)

/**
 * Default C language implementation of a 16x32->32 multiply. This function may
 * be replaced by a platform-specific version for speed.
 *
 * @param u A signed 16-bit multiplicand
 * @param v A signed 32-bit multiplier

 * @return  A signed 32-bit value corresponding to the 32 most significant bits
 * of the 48-bit product of u and v.
 */
INLINE OI_INT32 default_mul_16s_32s_hi(OI_INT16 u, OI_INT32 v)
{
    OI_UINT16 v0;
    OI_INT16 v1;

    OI_INT32 w,x;

    v0 = (OI_UINT16)(v & 0xffff);
    v1 = (OI_INT16) (v >> 16);

    w = v1 * u;
    x = u * v0;

    return w + (x >> 16);
}

/**
 * Default C language implementation of a 32x32->32 multiply. This function may
 * be replaced by a platform-specific version for speed.
 *
 * @param u A signed 32-bit multiplicand
 * @param v A signed 32-bit multiplier

 * @return  A signed 32-bit value corresponding to the 32 most significant bits
 * of the 64-bit product of u and v.
 */
INLINE OI_INT32 default_mul_32s_32s_hi(OI_INT32 u, OI_INT32 v)
{
    OI_UINT32 u0, v0;
    OI_INT32 u1, v1, w1, w2, t;

    u0 = u & 0xFFFF; u1 = u >> 16;
    v0 = v & 0xFFFF; v1 = v >> 16;
    t = u1*v0;
    w1 = t & 0xFFFF;
    w2 = t >> 16;
    w1 = u0*v1 + w1;
    return u1*v1 + w2 + (w1 >> 16);
}

#define MUL_16S_16S(_x, _y) ((_x) * (_y))
#define MUL_16S_32S_HI(_x, _y) default_mul_16s_32s_hi(_x, _y)
#define MUL_32S_32S_HI(_x, _y) default_mul_32s_32s_hi(_x, _y)

#define LONG_MULT_DCT(K, sample) (MUL_16S_32S_HI(K, sample)<<2)

/* Expands to an efficient rotate, if one exists.
 * rotates b right by c bits.*/
#define ROR(b, c) ((b) >> (c)) | ((b) << (32-(c)))

/**
 * This macro invokes an 8-point type-II DCT. It may be redefined to invoke a
 * platform-optimized version if needed.
 */
#define DCT2_8(dst, src) dct2_8(dst, src)

//#define fast_gpio_manip_intsoff(a,b)

#if OI_CPU_TYPE == OI_CPU_ARM7_LEND
#include "platform_arm.h"
#else
#define fast_gpio_manip_intsoff(a,b)
#endif


#endif /* _PLATFORM_H */

///@}
#endif /* FEATURE_BT_EXTPF_AV */
