#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_codec_sbc_private.h"

#if !defined(SBC_ENHANCED) && !defined(SBC_4SUBBANDS) && !defined(SBC_8SUBBANDS)
#error At least one of SBC_ENHANCED, SBC_4SUBBANDS or SBC_8SUBBANDS must be defined
#endif
/** @file

This file, along with analysis-generated.c, contains the analysis filterbank
routines. The operations performed correspond to the operations described by
Figure 12.5 of the Bluetooth Audio Video Working Group document "Advanced Audio
Distribution Profile Specification, Adopted version 1.0."

As is the case with the synthesis filterbank, one important
optimization involves decomposing the "matrixing" operation. In this
case, it can be represented as the product of a type III discrete
cosine kernel and a retangular sparse matrix.

For figure 12.5, 8-subband case:
@code
    M[i][k] = cos((i+0.5)*(k-4)*pi/8), k = 0..15 and i = 0..7
@endcode

M can be factored into C3 * T, where C3 is an 8-point type III discrete
cosine kernel given by
@code
    C3[k][i] = cos(i*(k+0.5)*pi/8)), k = 0..7 and i = 0..7
@endcode

Note that C3 is the transposition of the matrix C2 as defined in the
explanation in synthesis.c.  T is a sparse 8x16 matrix with the
following non-zero entries:
@code
    T[abs(4-k)][k]      =  1,    k = 0..11

    T[20-k][k]          = -1,    k = 13..15
@endcode

See the reference to Liu's paper in synthesis.c for a formal
derivation of this relationship. T is superficially similar to the
transposition of R (R is defined in synthesis.c), but they are not
identical.

Looking back to Figure 12.5, we have output samples S[0..7] defined in
terms of the "Partial Calculation" vector Y[0..15] as follows:
@code
    S = M * Y
@endcode

Rewriting, taking into account that M = C3 * T yields
@code
    S = (C3 * T) * Y = C3 * (T * Y)
@endcode

S is therefore the type III discrete cosine transform of (T *Y).
Every element of T is either 1, 0, or -1, and at most two elements
per row are non-zero. It therefore can be implemented by summing or
taking differences of elements of Y.  Y itself is simply composed of
sums of subsequences of Z, which is itself just the input history
buffer multiplied by the windowing function. Thus, rather than
computing Y by summing elements in Z, we compute T * Y in place by
adding or subtracting the elements of Z which combine under T to form
the contribution to (T * Y). Z need not be explicitly computed, but
may be computed as needed.

As with synthesis.c, shifing the history buffer X is done lazily.

@ingroup esbc_lib
*/

///@addtogroup esbc_lib
///@{

void OI_SBC_AnalyzeFrame(OI_CODEC_SBC_ENCODER_CONTEXT *context,
                         OI_INT16 pcm[][2])
{
    OI_UINT ch;
    OI_UINT nrof_blocks = context->common.frameInfo.nrof_blocks;
    OI_INT32 *s = context->common.subdata;
    OI_UINT offset = context->analysisBufferOffset;


#ifdef SBC_4SUBBANDS
    if (context->common.frameInfo.nrof_subbands == 4) {
        OI_UINT nrof_channels = context->common.frameInfo.nrof_channels;
        do {
            if (offset == 0) {
            for (ch = 0; ch < nrof_channels; ch++) {
                    COPY_BACKWARD_32BIT_ALIGNED_40_HALFWORDS(context->analysisBuffer[ch] + OI_ARRAYSIZE(context->analysisBuffer[0]) - 40,
                                                              context->analysisBuffer[ch]);
                }
                offset = OI_ARRAYSIZE(context->analysisBuffer[0]) - 40;
                }
            offset -= 4;
            for (ch = 0; ch < nrof_channels; ch++) {
                analyze4_generated(context->analysisBuffer[ch] + offset, pcm, ch, s);
                s += 4;
            }
            pcm += 4;
        } while (--nrof_blocks);
    } else
#endif //SBC_4SUBBANDS
    {
        OI_ASSERT(context->common.frameInfo.nrof_subbands == 8);
#ifdef SBC_ENHANCED
        if (context->common.frameInfo.enhanced) {
            do {
                if (offset == 0) {
                    for (ch = 0; ch < 2; ch++) {
                        COPY_BACKWARD_32BIT_ALIGNED_104_HALFWORDS(context->analysisBuffer[ch] + OI_ARRAYSIZE(context->analysisBuffer[0]) - 104,
                                                                  context->analysisBuffer[ch]);
                    }
                    offset = OI_ARRAYSIZE(context->analysisBuffer[0]) - 104;
                }
                offset -= 8;
#if 0
                for (ch = 0; ch < 2; ch++) {
                    analyze8_enhanced_generated(context->analysisBuffer[0] + offset, pcm, ch, s);
                    s += 8;
                }
#else
                analyze8_enhanced_generated(context->analysisBuffer[0] + offset, pcm, 0, s);
                s += 8;
                analyze8_enhanced_generated(context->analysisBuffer[1] + offset, pcm, 1, s);
                s += 8;
#endif
                pcm += 8;
            } while (--nrof_blocks);
        } else
#endif //SBC_ENHANCED
#ifdef SBC_8SUBBANDS
        {
            OI_UINT nrof_channels = context->common.frameInfo.nrof_channels;
            do {
                if (offset == 0) {
                for (ch = 0; ch < nrof_channels; ch++) {
                        COPY_BACKWARD_32BIT_ALIGNED_72_HALFWORDS(context->analysisBuffer[ch] + OI_ARRAYSIZE(context->analysisBuffer[0]) - 72,
                                                                 context->analysisBuffer[ch]);
                    }
                    offset = OI_ARRAYSIZE(context->analysisBuffer[0]) - 72;
                }
                offset -= 8;
                for (ch = 0; ch < nrof_channels; ch++) {
                    analyze8_generated(context->analysisBuffer[ch] + offset, pcm, ch, s);
                    s += 8;
                }
                pcm += 8;
            } while (--nrof_blocks);
        }
#else // SBC_8SUBBANDS
        {;}
#endif // SBC_8SUBBANDS
    }
    context->analysisBufferOffset = offset;
}


#ifdef SBC_4SUBBANDS
/** 4-point type III DCT.
 * Called from analyze4_generated.
 * */

INLINE void dct3_4(OI_INT32 * RESTRICT out, OI_INT32 const * RESTRICT in)
{
    OI_INT32 f0, f1, f2, f3, f4, f5, f6, f7, f8;

    f0 = LONG_MULT_DCT(DCTIII_4_K00_FIX, in[2]);
    f1 = in[0] - f0;
    f2 = in[0] + f0;
    f3 = in[1] + in[3];
    f4 = LONG_MULT_DCT(DCTIII_4_K04_FIX, in[1]);
    f5 = LONG_MULT_DCT(DCTIII_4_K05_FIX, f3);
    f6 = LONG_MULT_DCT(DCTIII_4_K06_FIX, in[3]);
    f7 = f4 + f5;
    f8 = f6 - f5;
    out[3] = SCALE(f2 - f8, DCTIII_4_SHIFT_OUT);
    out[0] = SCALE(f2 + f8, DCTIII_4_SHIFT_OUT);
    out[2] = SCALE(f1 - f7, DCTIII_4_SHIFT_OUT);
    out[1] = SCALE(f1 + f7, DCTIII_4_SHIFT_OUT);

}
#endif // SBC_4SUBBANDS

#if defined(SBC_8SUBBANDS) || defined(SBC_ENHANCED)
/** 8-point type III DCT.
 * Called from analyze8_generated and analyze8_enhanced_generated.
 * */

INLINE void dct3_8(OI_INT32 * RESTRICT out, OI_INT32 const * RESTRICT in)
{
    OI_INT32 f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10;
    OI_INT32 f11, f12, f13, f14, f15, f16, f17, f18, f19;
    OI_INT32 f20, f21, f22, f23, f24, f25, f26, f27, f28;
    OI_INT32 f29, f30, f31, f32;


    f0 = LONG_MULT_DCT(DCTIII_8_K00_FIX, in[4]);
    f1 = in[0] - f0;
    f2 = in[0] + f0;
    f3 = in[2] + in[6];
    f4 = LONG_MULT_DCT(DCTIII_8_K04_FIX, in[2]);
    f5 = LONG_MULT_DCT(DCTIII_8_K05_FIX, f3);
    f6 = LONG_MULT_DCT(DCTIII_8_K06_FIX, in[6]);
    f7 = f4 + f5;
    f8 = f6 - f5;
    f9 = f2 - f8;
    f10 = f2 + f8;
    f11 = f1 - f7;
    f12 = f1 + f7;
    f13 = in[3] - in[5];
    f14 = in[3] + in[5];
    f15 = LONG_MULT_DCT(DCTIII_8_K15_FIX, f14);
    f16 = in[1] - f15;
    f17 = in[1] + f15;
    f18 = LONG_MULT_DCT(DCTIII_8_K18_FIX, f13);
    f19 = f18 - in[7];
    f20 = f18 + in[7];
    f21 = f20 + f17;
    f22 = LONG_MULT_DCT(DCTIII_8_K22_FIX, f20);
    f23 = LONG_MULT_DCT(DCTIII_8_K23_FIX, f21);
    f24 = LONG_MULT_DCT(DCTIII_8_K24_FIX, f17);
    f25 = f22 + f23;
    f26 = f24 - f23;
    f27 = f19 + f16;
    f28 = LONG_MULT_DCT(DCTIII_8_K28_FIX, f19);
    f29 = LONG_MULT_DCT(DCTIII_8_K29_FIX, f27);
    f30 = LONG_MULT_DCT(DCTIII_8_K30_FIX, f16);
    f31 = f28 + f29;
    f32 = f30 - f29;
    out[7] = SCALE(f10 - f25, DCTIII_8_SHIFT_OUT);
    out[0] = SCALE(f10 + f25, DCTIII_8_SHIFT_OUT);
    out[6] = SCALE(f12 - f31, DCTIII_8_SHIFT_OUT);
    out[1] = SCALE(f12 + f31, DCTIII_8_SHIFT_OUT);
    out[5] = SCALE(f11 - f32, DCTIII_8_SHIFT_OUT);
    out[2] = SCALE(f11 + f32, DCTIII_8_SHIFT_OUT);
    out[4] = SCALE(f9 - f26, DCTIII_8_SHIFT_OUT);
    out[3] = SCALE(f9 + f26, DCTIII_8_SHIFT_OUT);
}
#endif //defined(SBC_4SUBBANDS) || defined(SBC_ENHANCED)

///@}
#endif /* FEATURE_BT_EXTPF_AV */
