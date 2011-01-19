#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/** @file

This file, along with synthesis-generated.c, contains the synthesis
filterbank routines. The operations performed correspond to the
operations described in A2DP Appendix B, Figure 12.3. Several
mathematical optimizations are performed, particularly for the
8-subband case.

One important optimization is to note that the "matrixing" operation
can be decomposed into the product of a type II discrete cosine kernel
and another, sparse matrix.

According to Fig 12.3, in the 8-subband case,
@code
    N[k][i] = cos((i+0.5)*(k+4)*pi/8), k = 0..15 and i = 0..7
@endcode

N can be factored as R * C2, where C2 is an 8-point type II discrete
cosine kernel given by
@code
    C2[k][i] = cos((i+0.5)*k*pi/8)), k = 0..7 and i = 0..7
@endcode

R turns out to be a sparse 16x8 matrix with the following non-zero
entries:
@code
    R[k][k+4]        =  1,   k = 0..3
    R[k][abs(12-k)]  = -1,   k = 5..15
@endcode

The spec describes computing V[0..15] as N * R.
@code
    V[0..15] = N * R = (R * C2) * R = R * (C2 * R)
@endcode

C2 * R corresponds to computing the discrete cosine transform of R, so
V[0..15] can be computed by taking the DCT of R followed by assignment
and selective negation of the DCT result into V.

        Although this was derived empirically using GNU Octave, it is
        formally demonstrated in, e.g., Liu, Chi-Min and Lee,
        Wen-Chieh. "A Unified Fast Algorithm for Cosine Modulated
        Filter Banks in Current Audio Coding Standards." Journal of
        the AES 47 (December 1999): 1061.

Given the shift operation performed prior to computing V[0..15], it is
clear that V[0..159] represents a rolling history of the 10 most
recent groups of blocks input to the synthesis operation. Interpreting
the matrix N in light of its factorization into C2 and R, R's
sparseness has implications for interpreting the values in V. In
particular, there is considerable redundancy in the values stored in
V. Furthermore, since R[4][0..7] are all zeros, one out of every 16
values in V will be zero regardless of the input data. Within each
block of 16 values in V, fully half of them are redundant or
irrelevant:

@code
    V[ 0] =  DCT[4]
    V[ 1] =  DCT[5]
    V[ 2] =  DCT[6]
    V[ 3] =  DCT[7]
    V[ 4] = 0
    V[ 5] = -DCT[7] = -V[3] (redundant)
    V[ 6] = -DCT[6] = -V[2] (redundant)
    V[ 7] = -DCT[5] = -V[1] (redundant)
    V[ 8] = -DCT[4] = -V[0] (redundant)
    V[ 9] = -DCT[3]
    V[10] = -DCT[2]
    V[11] = -DCT[1]
    V[12] = -DCT[0]
    V[13] = -DCT[1] = V[11] (redundant)
    V[14] = -DCT[2] = V[10] (redundant)
    V[15] = -DCT[3] = V[ 9] (redundant)
@endcode

Since the elements of V beyond 15 were originally computed the same
way during a previous run, what holds true for V[x] also holds true
for V[x+16]. Thus, so long as care is taken to maintain the mapping,
we need only actually store the unique values, which correspond to the
output of the DCT, in some cases inverted. In fact, instead of storing
V[0..159], we could store DCT[0..79] which would contain a history of
DCT results. More on this in a bit.

Going back to figure 12.3 in the spec, it should be clear that the
vector U need not actually be explicitly constructed, but that with
suitable indexing into V during the window operation, the same end can
be accomplished. In the same spirit of the pseudocode shown in the
figure, the following is the construction of W without using U:

@code
    for i=0 to 79 do
        W[i] = D[i]*VSIGN(i)*V[remap_V(i)] where remap_V(i) = 32*(int(i/16)) + (i % 16) + (i % 16 >= 8 ? 16 : 0)
                                             and VSIGN(i) maps i%16 into {1, 1, 1, 1, 0, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1 }
                                             These values correspond to the
                                             signs of the redundant values as
                                             shown in the explanation three
                                             paragraphs above.
@endcode

We saw above how V[4..8,13..15] (and by extension
V[(4..8,13..15)+16*n]) can be defined in terms of other elements
within the subblock of V. V[0..3,9..12] correspond to DCT elements.

@code
    for i=0 to 79 do
        W[i] = D[i]*DSIGN(i)*DCT[remap_DCT(i)]
@endcode

The DCT is calculated using the Arai-Agui-Nakajima factorization,
which saves some computation by producing output that needs to be
multiplied by scaling factors before being used.

@code
    for i=0 to 79 do
        W[i] = D[i]*SCALE[i%8]*AAN_DCT[remap_DCT(i)]
@endcode

D can be premultiplied with the DCT scaling factors to yield

@code
    for i=0 to 79 do
        W[i] = DSCALED[i]*AAN_DCT[remap_DCT(i)] where DSCALED[i] = D[i]*SCALE[i%8]
@endcode

The output samples X[0..7] are defined as sums of W:

@code
        X[j] = sum{i=0..9}(W[j+8*i])
@endcode

@ingroup esbc_lib
*/

#include "oi_codec_sbc_private.h"

PRIVATE void SynthWindow80_generated(OI_INT16 *pcm, SBC_BUFFER_T const * RESTRICT buffer);
PRIVATE void SynthWindow112_generated(OI_INT16 *pcm, SBC_BUFFER_T const * RESTRICT buffer);

#ifdef OI_CPU_TMS320C54X
PRIVATE void SynthWindow40_generated_c54x(OI_INT16 * RESTRICT pcm, SBC_BUFFER_T const * RESTRICT buffer);
PRIVATE void SynthWindow80_generated_c54x(OI_INT16 * RESTRICT pcm, SBC_BUFFER_T const * RESTRICT buffer);
PRIVATE void SynthWindow112_generated_c54x(OI_INT16 * RESTRICT pcm, SBC_BUFFER_T const * RESTRICT buffer);
#define SYNTH80 SynthWindow80_generated_c54x
#define SYNTH112 SynthWindow112_generated_c54x
#else
#define SYNTH80 SynthWindow80_generated
#define SYNTH112 SynthWindow112_generated
#endif

void SynthWindow40_int32_int32_symmetry_with_sum_experiment(OI_INT16 pcm[4][2], SBC_BUFFER_T buffer[80], OI_UINT channel);

PRIVATE void dct2_8(SBC_BUFFER_T * RESTRICT out, OI_INT32 const * RESTRICT x);
PRIVATE void dct2_4(SBC_BUFFER_T * RESTRICT out, OI_INT32 const * RESTRICT x);


#ifdef MIDSIDESTEREO_CHECK
static OI_INT32 subdataCopy[SBC_MAX_BLOCKS*SBC_MAX_CHANNELS*SBC_MAX_BANDS];

static void MidSideStereo_check(OI_CODEC_SBC_DECODER_CONTEXT *context)
{
    OI_INT32 *s;
    OI_INT32 *check;
    OI_UINT block;
    OI_UINT nrof_bands;
    OI_UINT sb;

    check = context->common.subdata;
    nrof_bands = context->common.frameInfo.nrof_subbands;

    if (context->common.frameInfo.mode == SBC_JOINT_STEREO) {
        for (sb = 0; sb < nrof_bands-1; sb++) {
            s = &context->common.subdata[sb];
            check = &subdataCopy[sb];
            for (block = 0; block < context->common.frameInfo.nrof_blocks; ++block) {
                if (s[0] != check[0]) {
                    printf("Subdata check failed sb %d block %d (left)\n", sb, block);
                    return;
                }
                if (s[nrof_bands] != check[nrof_bands]) {
                    printf("Subdata check failed sb %d block %d (right)\n", sb, block);
                    return;
                }
                s += nrof_bands * 2;
                check += nrof_bands * 2;
            }
        }
    }
    //printf("Subdata check ok\n");
}

PRIVATE void MidSideStereo_old(OI_CODEC_SBC_DECODER_CONTEXT *context)
{
    OI_INT32 *s;
    OI_UINT nrof_blocks;
    OI_UINT nrof_bands;
    OI_UINT join;
    OI_UINT sb;
    OI_INT i;

    for (i = 0; i < SBC_MAX_BLOCKS*SBC_MAX_CHANNELS*SBC_MAX_BANDS; ++i) {
        subdataCopy[i] = context->common.subdata[i];
    }

    join = context->common.frameInfo.join;
    nrof_bands = context->common.frameInfo.nrof_subbands;

    if (context->common.frameInfo.mode == SBC_JOINT_STEREO) {
        OI_UINT joinmask;
        joinmask = 1 << nrof_bands;

        for (sb = 0; sb < nrof_bands-1; sb++) {
            joinmask >>= 1;
            if (join & joinmask) {
                s = &subdataCopy[sb];

                nrof_blocks = context->common.frameInfo.nrof_blocks-1;
                do {
                    OI_INT32 mid, side, left, right;

                    mid = s[0];
                    side = s[nrof_bands];

                    left = mid + side;
                    right = mid - side;

                    s[0] = left;
                    s[nrof_bands] = right;

                    s += nrof_bands * 2;
                } while (nrof_blocks--);
            }
        }
    }

}
#endif /* MIDSIDESTEREO_CHECK */


#ifdef SBC_4SUBBANDS
PRIVATE void MidSideStereo4(OI_CODEC_SBC_DECODER_CONTEXT *context)
{
    OI_INT32 *s;
    OI_INT block;
    register OI_UINT join;

    if (context->common.frameInfo.mode != SBC_JOINT_STEREO) {
        return;
    }

#ifdef MIDSIDESTEREO_CHECK
    MidSideStereo_old(context);
#endif

    s = context->common.subdata;
    join = context->common.frameInfo.join;
    for (block = context->common.frameInfo.nrof_blocks - 1; block >= 0; --block) {
        OI_INT32 mid0 = s[0];
        OI_INT32 mid1 = s[1];
        OI_INT32 mid2 = s[2];
        OI_INT32 side0 = s[4];
        OI_INT32 side1 = s[5];
        OI_INT32 side2 = s[6];
        OI_INT32 left0 = (join & (1 << 3)) ? mid0 + side0 : mid0;
        OI_INT32 left1 = (join & (1 << 2)) ? mid1 + side1 : mid1;
        OI_INT32 left2 = (join & (1 << 1)) ? mid2 + side2 : mid2;
        OI_INT32 right0 =(join & (1 << 3)) ? mid0 - side0 : side0;
        OI_INT32 right1 =(join & (1 << 2)) ? mid1 - side1 : side1;
        OI_INT32 right2 =(join & (1 << 1)) ? mid2 - side2 : side2;
        s[0] = left0;
        s[1] = left1;
        s[2] = left2;
        s[5] = right1;
        s[4] = right0;
        s[6] = right2;
        s += 4 * 2;
    }

#ifdef MIDSIDESTEREO_CHECK
    MidSideStereo_check(context);
#endif
}
#endif /* SBC_4SUBBANDS */

PRIVATE void MidSideStereo8(OI_CODEC_SBC_DECODER_CONTEXT *context)
{
    OI_INT32 *s;
    OI_INT block;
    OI_UINT join;

    if (context->common.frameInfo.mode != SBC_JOINT_STEREO) {
        return;
    }

#ifdef MIDSIDESTEREO_CHECK
    MidSideStereo_old(context);
#endif

    for (block = context->common.frameInfo.nrof_blocks - 1; block >= 0; --block) {
        s = &context->common.subdata[block * 2 * 8];
        join = context->common.frameInfo.join;
        {
            OI_INT32 mid = s[0];
            OI_INT32 side = s[8];
            OI_INT32 left = (join & (1 << 7)) ? mid + side : mid;
            OI_INT32 right =(join & (1 << 7)) ? mid - side : side;
            s[0] = left;
            s[8] = right;
        }
        while (join & 0x7E) {
            OI_INT32 mid0 = s[1];
            OI_INT32 mid1 = s[2];
            OI_INT32 mid2 = s[3];
            OI_INT32 side0 = s[9];
            OI_INT32 side1 = s[10];
            OI_INT32 side2 = s[11];
            OI_INT32 left0 = (join & (1 << 6)) ? mid0 + side0 : mid0;
            OI_INT32 left1 = (join & (1 << 5)) ? mid1 + side1 : mid1;
            OI_INT32 left2 = (join & (1 << 4)) ? mid2 + side2 : mid2;
            OI_INT32 right0 =(join & (1 << 6)) ? mid0 - side0 : side0;
            OI_INT32 right1 =(join & (1 << 5)) ? mid1 - side1 : side1;
            OI_INT32 right2 =(join & (1 << 4)) ? mid2 - side2 : side2;
            s[1] = left0;
            s[2] = left1;
            s[3] = left2;
            s[9] = right0;
            s[10] = right1;
            s[11] = right2;
            join <<= 3;
            s += 3;
        }
    }
#ifdef MIDSIDESTEREO_CHECK
    MidSideStereo_check(context);
#endif
}

PRIVATE void SynthFrame80_2ch(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 pcm[RESTRICT][2])
{
    OI_INT32 *s;
    OI_UINT nrof_blocks;
    OI_UINT ch;
    OI_UINT blk;
    OI_UINT offset;

    nrof_blocks = context->common.frameInfo.nrof_blocks;
    s = context->common.subdata;
    offset = context->synthBufferOffset;

    OPT_LOOP_HINT(nrof_blocks, 4, SBC_MAX_BLOCKS, 4);
    for (blk = 0; blk < nrof_blocks; blk++) {
        if (offset == 0) {
            shift_buffer(context->synthBuffer[0] + OI_ARRAYSIZE(context->synthBuffer[0]) - 72,context->synthBuffer[0], 72);
            shift_buffer(context->synthBuffer[1] + OI_ARRAYSIZE(context->synthBuffer[1]) - 72,context->synthBuffer[1], 72);
            offset = OI_ARRAYSIZE(context->synthBuffer[0]) - 80;
        } else {
            offset -= 1*8;
        }

        for (ch = 0; ch < 2; ch++) {
            DCT2_8(context->synthBuffer[ch] + offset, s);
            SYNTH80(((OI_INT16*)(pcm + blk*8))+ch, context->synthBuffer[ch] + offset);
            s += 8;
        }
    }
    context->synthBufferOffset = offset;
}

PRIVATE void SynthFrame80_1ch(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 pcm[RESTRICT][2])
{
    OI_INT32 *s;
    OI_UINT nrof_blocks;
    OI_UINT blk;

    nrof_blocks = context->common.frameInfo.nrof_blocks;
    s = context->common.subdata;

    OPT_LOOP_HINT(nrof_blocks, 4, SBC_MAX_BLOCKS, 4);
    for (blk = 0; blk < nrof_blocks; blk++) {
        if (context->synthBufferOffset == 0) {
            shift_buffer(context->synthBuffer[0] + OI_ARRAYSIZE(context->synthBuffer[0]) - 72,context->synthBuffer[0], 72);
            context->synthBufferOffset = OI_ARRAYSIZE(context->synthBuffer[0]) - 80;
        } else {
            context->synthBufferOffset -= 1*8;
        }

        DCT2_8(context->synthBuffer[0] + context->synthBufferOffset, s);
        SYNTH80(((OI_INT16*)(pcm + blk*8))+0, context->synthBuffer[0] + context->synthBufferOffset);
        s += 8;
    }
}


void OI_SBC_SynthFrame(OI_CODEC_SBC_DECODER_CONTEXT *context, OI_INT16 pcm[RESTRICT][2])
{
#if defined(SBC_4SUBBANDS) || defined(SBC_ENHANCED) || defined(OI_CODE)
    OI_UINT blk;
    OI_UINT ch;
#endif /*defined(SBC_4SUBBANDS) || defined(SBC_ENHANCED) || defined(OI_CODE)*/

    OI_UINT nrof_subbands;
    OI_UINT nrof_channels;

#if defined(SBC_4SUBBANDS) || defined(SBC_ENHANCED)   
    OI_UINT nrof_blocks;
    OI_INT32 *s;
#endif /*defined(SBC_4SUBBANDS) || defined(SBC_ENHANCED)*/

    nrof_subbands = context->common.frameInfo.nrof_subbands;
    nrof_channels = context->common.frameInfo.nrof_channels;

#if defined(SBC_4SUBBANDS) || defined(SBC_ENHANCED)       
    nrof_blocks = context->common.frameInfo.nrof_blocks;
    s = context->common.subdata;
#endif /*defined(SBC_4SUBBANDS) || defined(SBC_ENHANCED)*/

    OI_ASSERT(nrof_subbands == 4 || nrof_subbands == 8);
    if (nrof_subbands == 4) {
#ifdef SBC_4SUBBANDS
        MidSideStereo4(context);
        OPT_LOOP_HINT(nrof_blocks, 4, SBC_MAX_BLOCKS, 4);
        for (blk = 0; blk < nrof_blocks; blk++) {
            if (context->synthBufferOffset == 0) {
                shift_buffer(context->synthBuffer[0] + OI_ARRAYSIZE(context->synthBuffer[0]) - 2*36,
                             context->synthBuffer[0],
                             2*36);
                if (nrof_channels == 2) {
                    shift_buffer(context->synthBuffer[1] + OI_ARRAYSIZE(context->synthBuffer[0]) - 2*36,
                                 context->synthBuffer[1],
                                 2*36);
                }
                context->synthBufferOffset = OI_ARRAYSIZE(context->synthBuffer[0]) - 2*40;
            } else {
                context->synthBufferOffset -= 2*4;
            }
            OPT_LOOP_HINT(nrof_channels, 1, 2, 1);
            for (ch = 0; ch < nrof_channels; ch++) {
                cosineModulateSynth4(context->synthBuffer[ch] + context->synthBufferOffset, s);
                SynthWindow40_int32_int32_symmetry_with_sum(pcm + blk*4,
                                                            context->synthBuffer[ch] + context->synthBufferOffset,
                                                            ch);
                s += 4;
            }
        }
#endif

#ifdef SBC_ENHANCED
    } else if (context->common.frameInfo.enhanced) {
        MidSideStereo8(context);
        OPT_LOOP_HINT(nrof_blocks, 4, SBC_MAX_BLOCKS, 4);
        for (blk = 0; blk < nrof_blocks; blk++) {
            if (context->synthBufferOffset == 0) {
                shift_buffer(context->synthBuffer[0] + OI_ARRAYSIZE(context->synthBuffer[0]) - (112 - 8),
                             context->synthBuffer[0],
                             (112 - 8));
                if (nrof_channels == 2) {
                    shift_buffer(context->synthBuffer[1] + OI_ARRAYSIZE(context->synthBuffer[0]) - (112 - 8),
                                 context->synthBuffer[1],
                                 (112 - 8));
                }
                context->synthBufferOffset = OI_ARRAYSIZE(context->synthBuffer[0]) - 112;
            } else {
                context->synthBufferOffset -= 8;
            }

            OPT_LOOP_HINT(nrof_channels, 1, 2, 1);
            for (ch = 0; ch < nrof_channels; ch++) {
                DCT2_8(context->synthBuffer[ch] + context->synthBufferOffset, s);
                SYNTH112(((OI_INT16*)(pcm + blk*8))+ch, context->synthBuffer[ch] + context->synthBufferOffset);
                s += 8;
            }
        }
#endif // SBC_ENHANCED
    } else {
#if defined(SBC_4SUBBANDS) && !defined (OI_CODE)
        if (nrof_subbands == 4) {
            MidSideStereo4(context);
        } else
#endif /* SBC_4SUBBANDS */
	{
            MidSideStereo8(context);
        }
        if (nrof_channels == 2) {
            SynthFrame80_2ch(context, pcm);
        } else {
            SynthFrame80_1ch(context, pcm);
        }
    }
}

#ifdef SBC_4SUBBANDS
void SynthWindow40_int32_int32_symmetry_with_sum(OI_INT16 pcm[4][2], SBC_BUFFER_T buffer[80], OI_UINT channel)
{
    OI_INT32 pa;
    OI_INT32 pb;

    /* These values should be zero, since out[2] of the 4-band cosine modulation
     * is always zero. */
    OI_ASSERT(buffer[ 2] == 0);
    OI_ASSERT(buffer[10] == 0);
    OI_ASSERT(buffer[18] == 0);
    OI_ASSERT(buffer[26] == 0);
    OI_ASSERT(buffer[34] == 0);
    OI_ASSERT(buffer[42] == 0);
    OI_ASSERT(buffer[50] == 0);
    OI_ASSERT(buffer[58] == 0);
    OI_ASSERT(buffer[66] == 0);
    OI_ASSERT(buffer[74] == 0);


    pa  = window_4[ 4] * (buffer[12] + buffer[76]);
    pa += window_4[ 8] * (buffer[16] - buffer[64]);
    pa += window_4[12] * (buffer[28] + buffer[60]);
    pa += window_4[16] * (buffer[32] - buffer[48]);
    pa += window_4[20] *  buffer[44];
    pa = SCALE(-pa, 15);
    CLIP_INT16(pa);
    pcm[0][channel] = (OI_INT16)pa;


    pa  = window_4[ 1] * buffer[ 1]; pb  = window_4[ 1] * buffer[79];
    pb += window_4[ 3] * buffer[ 3]; pa += window_4[ 3] * buffer[77];
    pa += window_4[ 5] * buffer[13]; pb += window_4[ 5] * buffer[67];
    pb += window_4[ 7] * buffer[15]; pa += window_4[ 7] * buffer[65];
    pa += window_4[ 9] * buffer[17]; pb += window_4[ 9] * buffer[63];
    pb += window_4[11] * buffer[19]; pa += window_4[11] * buffer[61];
    pa += window_4[13] * buffer[29]; pb += window_4[13] * buffer[51];
    pb += window_4[15] * buffer[31]; pa += window_4[15] * buffer[49];
    pa += window_4[17] * buffer[33]; pb += window_4[17] * buffer[47];
    pb += window_4[19] * buffer[35]; pa += window_4[19] * buffer[45];
    pa = SCALE(-pa, 15);
    CLIP_INT16(pa);
    pcm[1][channel] = (OI_INT16)(pa);
    pb = SCALE(-pb, 15);
    CLIP_INT16(pb);
    pcm[3][channel] = (OI_INT16)(pb);


    pa  = window_4[2] * (/*buffer[ 2] + */ buffer[78]);  /* buffer[ 2] is always zero */
    pa += window_4[6] * (buffer[14] /* + buffer[66]*/);  /* buffer[66] is always zero */
    pa += window_4[10] * (/*buffer[18] + */ buffer[62]);  /* buffer[18] is always zero */
    pa += window_4[14] * (buffer[30] /* + buffer[50]*/);  /* buffer[50] is always zero */
    pa += window_4[18] * (/*buffer[34] + */ buffer[46]);  /* buffer[34] is always zero */
    pa = SCALE(-pa, 15);
    CLIP_INT16(pa);
    pcm[2][channel] = (OI_INT16)(pa);
}
#endif /* SBC_4SUBBANDS */

PRIVATE void dct2_4(SBC_BUFFER_T * RESTRICT out, OI_INT32 const *RESTRICT in)
{
    OI_INT32 f0, f1, f2, f3, f4, f7, f8, f9, f10;

    f0 = (in[0] - in[3]);
    f1 = (in[0] + in[3]);
    f2 = (in[1] - in[2]);
    f3 = (in[1] + in[2]);

    f4 = f1 - f3;

    out[0] = (SBC_BUFFER_T)-SCALE(f1 + f3, DCT_SHIFT);
    out[2] = (SBC_BUFFER_T)-SCALE(LONG_MULT_DCT(DCTII_4_K06_FIX, f4), DCT_SHIFT);
    f7 = f0 + f2;
    f8 = LONG_MULT_DCT(DCTII_4_K08_FIX, f0);
    f9 = LONG_MULT_DCT(DCTII_4_K09_FIX, f7);
    f10 = LONG_MULT_DCT(DCTII_4_K10_FIX, f2);
    out[3] = (SBC_BUFFER_T)-SCALE(f8 + f9, DCT_SHIFT);
    out[1] = (SBC_BUFFER_T)-SCALE(f10 - f9, DCT_SHIFT);
}

/**
  This routine implements the cosine modulation matrix for 4-subband
  synthesis. This is called "matrixing" in the SBC specification. This
  matrix, M4,  can be factored into an 8-point Type II Discrete Cosine
  Transform, DCTII_4 and a matrix S4, given here:

  @code
        __               __
       |   0   0   1   0   |
       |   0   0   0   1   |
       |   0   0   0   0   |
       |   0   0   0  -1   |
  S4 = |   0   0  -1   0   |
       |   0  -1   0   0   |
       |  -1   0   0   0   |
       |__ 0  -1   0   0 __|

  M4 * in = S4 * (DCTII_4 * in)
  @endcode

  (DCTII_4 * in) is computed using a Fast Cosine Transform. The algorithm
  here is based on an implementation computed by the SPIRAL computer
  algebra system, manually converted to fixed-point arithmetic. S4 can be
  implemented using only assignment and negation.
  */
PRIVATE void cosineModulateSynth4(SBC_BUFFER_T * RESTRICT out, OI_INT32 const * RESTRICT in)
{
    OI_INT32 f0, f1, f2, f3, f4, f7, f8, f9, f10;
    OI_INT32 y0, y1, y2, y3;

    f0 = (in[0] - in[3]);
    f1 = (in[0] + in[3]);
    f2 = (in[1] - in[2]);
    f3 = (in[1] + in[2]);

    f4 = f1 - f3;

    y0 = -SCALE(f1 + f3, DCT_SHIFT);
    y2 = -SCALE(LONG_MULT_DCT(DCTII_4_K06_FIX, f4), DCT_SHIFT);
    f7 = f0 + f2;
    f8 = LONG_MULT_DCT(DCTII_4_K08_FIX, f0);
    f9 = LONG_MULT_DCT(DCTII_4_K09_FIX, f7);
    f10 = LONG_MULT_DCT(DCTII_4_K10_FIX, f2);
    y3 = -SCALE(f8 + f9, DCT_SHIFT);
    y1 = -SCALE(f10 - f9, DCT_SHIFT);

    out[0] = (OI_INT16)-y2;
    out[1] = (OI_INT16)-y3;
    out[2] = (OI_INT16)0;
    out[3] = (OI_INT16)y3;
    out[4] = (OI_INT16)y2;
    out[5] = (OI_INT16)y1;
    out[6] = (OI_INT16)y0;
    out[7] = (OI_INT16)y1;
}


/** Performs an 8-point Type-II scaled DCT using the Arai-Agui-Nakajima
 * factorization. The scaling factors are folded into the windowing
 * constants. 29 adds and 5 16x32 multiplies per 8 samples.
 */

#ifdef DEBUG_DCT
PRIVATE void float_dct2_8(float * RESTRICT out, OI_INT32 const *RESTRICT in)
{
#define FIX(x,bits) (((int)floor(0.5f+((x)*((float)(1<<bits)))))/((float)(1<<bits)))
#define FLOAT_BUTTERFLY(x,y) x += y; y = x - (y*2); OI_ASSERT(VALID_INT32(x)); OI_ASSERT(VALID_INT32(y));
#define FLOAT_MULT_DCT(K, sample) (FIX(K,20) * sample)
#define FLOAT_SCALE(x, y) (((x) / (double)(1 << (y))))

    double L00,L01,L02,L03,L04,L05,L06,L07;
    double L25;

    double in0,in1,in2,in3;
    double in4,in5,in6,in7;

    in0 = FLOAT_SCALE(in[0], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in0));
    in1 = FLOAT_SCALE(in[1], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in1));
    in2 = FLOAT_SCALE(in[2], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in2));
    in3 = FLOAT_SCALE(in[3], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in3));
    in4 = FLOAT_SCALE(in[4], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in4));
    in5 = FLOAT_SCALE(in[5], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in5));
    in6 = FLOAT_SCALE(in[6], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in6));
    in7 = FLOAT_SCALE(in[7], DCTII_8_SHIFT_IN); OI_ASSERT(VALID_INT32(in7));

    L00 = (in0 + in7); OI_ASSERT(VALID_INT32(L00));
    L01 = (in1 + in6); OI_ASSERT(VALID_INT32(L01));
    L02 = (in2 + in5); OI_ASSERT(VALID_INT32(L02));
    L03 = (in3 + in4); OI_ASSERT(VALID_INT32(L03));

    L04 = (in3 - in4); OI_ASSERT(VALID_INT32(L04));
    L05 = (in2 - in5); OI_ASSERT(VALID_INT32(L05));
    L06 = (in1 - in6); OI_ASSERT(VALID_INT32(L06));
    L07 = (in0 - in7); OI_ASSERT(VALID_INT32(L07));

    FLOAT_BUTTERFLY(L00, L03);
    FLOAT_BUTTERFLY(L01, L02);

    L02 += L03; OI_ASSERT(VALID_INT32(L02));

    L02 = FLOAT_MULT_DCT(AAN_C4_FLOAT, L02); OI_ASSERT(VALID_INT32(L02));

    FLOAT_BUTTERFLY(L00, L01);

    out[0] = (float)FLOAT_SCALE(L00, DCTII_8_SHIFT_0); OI_ASSERT(VALID_INT16(out[0]));
    out[4] = (float)FLOAT_SCALE(L01, DCTII_8_SHIFT_4); OI_ASSERT(VALID_INT16(out[4]));

    FLOAT_BUTTERFLY(L03, L02);
    out[6] = (float)FLOAT_SCALE(L02, DCTII_8_SHIFT_6); OI_ASSERT(VALID_INT16(out[6]));
    out[2] = (float)FLOAT_SCALE(L03, DCTII_8_SHIFT_2); OI_ASSERT(VALID_INT16(out[2]));

    L04 += L05; OI_ASSERT(VALID_INT32(L04));
    L05 += L06; OI_ASSERT(VALID_INT32(L05));
    L06 += L07; OI_ASSERT(VALID_INT32(L06));

    L04/=2;
    L05/=2;
    L06/=2;
    L07/=2;

    L05 = FLOAT_MULT_DCT(AAN_C4_FLOAT, L05); OI_ASSERT(VALID_INT32(L05));

    L25 = L06 - L04; OI_ASSERT(VALID_INT32(L25));
    L25 = FLOAT_MULT_DCT(AAN_C6_FLOAT, L25); OI_ASSERT(VALID_INT32(L25));

    L04 = FLOAT_MULT_DCT(AAN_Q0_FLOAT, L04); OI_ASSERT(VALID_INT32(L04));
    L04 -= L25; OI_ASSERT(VALID_INT32(L04));

    L06 = FLOAT_MULT_DCT(AAN_Q1_FLOAT, L06); OI_ASSERT(VALID_INT32(L06));
    L06 -= L25; OI_ASSERT(VALID_INT32(L25));

    FLOAT_BUTTERFLY(L07, L05);

    FLOAT_BUTTERFLY(L05, L04);
    out[3] = (float)(FLOAT_SCALE(L04, DCTII_8_SHIFT_3-1)); OI_ASSERT(VALID_INT16(out[3]));
    out[5] = (float)(FLOAT_SCALE(L05, DCTII_8_SHIFT_5-1)); OI_ASSERT(VALID_INT16(out[5]));

    FLOAT_BUTTERFLY(L07, L06);
    out[7] = (float)(FLOAT_SCALE(L06, DCTII_8_SHIFT_7-1)); OI_ASSERT(VALID_INT16(out[7]));
    out[1] = (float)(FLOAT_SCALE(L07, DCTII_8_SHIFT_1-1)); OI_ASSERT(VALID_INT16(out[1]));
}
#undef BUTTERFLY
#endif

/* This function calculates the AAN DCT. Its inputs are in S16.15 format, as
 * returned by OI_SBC_Dequant. In practice, abs(in[x]) < 52429.0 / 1.38
 * (1244918057 integer). The function it computes is an approximation to the array defined
 * by:
 *
 * diag(aan_s) * AAN= C2
 *   or
 *
 * AAN = diag(1/aan_s) * C2
 *
 * where C2 is as it is defined in the comment at the head of this file, and
 *
 * aan_s[i] = aan_s = 1/(2*cos(i*pi/16)) with i = 1..7, aan_s[0] = 1;
 *
 * aan_s[i] = [ 1.000  0.510  0.541  0.601  0.707  0.900  1.307  2.563 ]
 *
 * The output ranges are shown as follows:
 *
 * Let Y[0..7] = AAN * X[0..7]
 *
 * Without loss of generality, assume the input vector X consists of elements
 * between -1 and 1. The maximum possible value of a given output element occurs
 * with some particular combination of input vector elements each of which is -1
 * or 1. Consider the computation of Y[i]. Y[i] = sum t=0..7 of AAN[t,i]*X[i]. Y is
 * maximized if the sign of X[i] matches the sign of AAN[t,i], ensuring a
 * positive contribution to the sum. Equivalently, one may simply sum
 * abs(AAN)[t,i] over t to get the maximum possible value of Y[i].
 *
 * This yields approximately [8.00  10.05   9.66   8.52   8.00   5.70   4.00   2.00]
 *
 * Given the maximum magnitude sensible input value of +/-37992, this yields the
 * following vector of maximum output magnitudes:
 *
 * [ 303936  381820  367003  323692  303936  216555  151968   75984 ]
 *
 * Ultimately, these values must fit into 16 bit signed integers, so they must
 * be scaled. A non-uniform scaling helps maximize the kept precision. The
 * relative number of extra bits of precision maintainable with respect to the
 * largest value is given here:
 *
 * [ 0  0  0  0  0  0  1  2 ]
 *
 */
PRIVATE void dct2_8(SBC_BUFFER_T * RESTRICT out, OI_INT32 const *RESTRICT in)
{
#define BUTTERFLY(x,y) x += y; y = x - (y<<1);
#define FIX_MULT_DCT(K, x) (MUL_32S_32S_HI(K,x)<<2)

    OI_INT32 L00,L01,L02,L03,L04,L05,L06,L07;
    OI_INT32 L25;

    OI_INT32 in0,in1,in2,in3;
    OI_INT32 in4,in5,in6,in7;

#if DCTII_8_SHIFT_IN != 0
    in0 = SCALE(in[0], DCTII_8_SHIFT_IN);
    in1 = SCALE(in[1], DCTII_8_SHIFT_IN);
    in2 = SCALE(in[2], DCTII_8_SHIFT_IN);
    in3 = SCALE(in[3], DCTII_8_SHIFT_IN);
    in4 = SCALE(in[4], DCTII_8_SHIFT_IN);
    in5 = SCALE(in[5], DCTII_8_SHIFT_IN);
    in6 = SCALE(in[6], DCTII_8_SHIFT_IN);
    in7 = SCALE(in[7], DCTII_8_SHIFT_IN);
#else
    in0 = in[0];
    in1 = in[1];
    in2 = in[2];
    in3 = in[3];
    in4 = in[4];
    in5 = in[5];
    in6 = in[6];
    in7 = in[7];
#endif

    L00 = in0 + in7;
    L01 = in1 + in6;
    L02 = in2 + in5;
    L03 = in3 + in4;

    L04 = in3 - in4;
    L05 = in2 - in5;
    L06 = in1 - in6;
    L07 = in0 - in7;

    BUTTERFLY(L00, L03);
    BUTTERFLY(L01, L02);

    L02 += L03;

    L02 = FIX_MULT_DCT(AAN_C4_FIX, L02);

    BUTTERFLY(L00, L01);

    out[0] = (OI_INT16)SCALE(L00, DCTII_8_SHIFT_0);
    out[4] = (OI_INT16)SCALE(L01, DCTII_8_SHIFT_4);

    BUTTERFLY(L03, L02);
    out[6] = (OI_INT16)SCALE(L02, DCTII_8_SHIFT_6);
    out[2] = (OI_INT16)SCALE(L03, DCTII_8_SHIFT_2);

    L04 += L05;
    L05 += L06;
    L06 += L07;

    L04/=2;
    L05/=2;
    L06/=2;
    L07/=2;

    L05 = FIX_MULT_DCT(AAN_C4_FIX, L05);

    L25 = L06 - L04;
    L25 = FIX_MULT_DCT(AAN_C6_FIX, L25);

    L04 = FIX_MULT_DCT(AAN_Q0_FIX, L04);
    L04 -= L25;

    L06 = FIX_MULT_DCT(AAN_Q1_FIX, L06);
    L06 -= L25;

    BUTTERFLY(L07, L05);

    BUTTERFLY(L05, L04);
    out[3] = (OI_INT16)SCALE(L04, DCTII_8_SHIFT_3-1);
    out[5] = (OI_INT16)SCALE(L05, DCTII_8_SHIFT_5-1);

    BUTTERFLY(L07, L06);
    out[7] = (OI_INT16)SCALE(L06, DCTII_8_SHIFT_7-1);
    out[1] = (OI_INT16)SCALE(L07, DCTII_8_SHIFT_1-1);
#undef BUTTERFLY

#ifdef DEBUG_DCT
    {
        float float_out[8];
        float_dct2_8(float_out, in);
    }
#endif
}

///@}
#endif /* FEATURE_BT_EXTPF_AV */
