#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
 ***********************************************************************************/

/**
@file

The functions in this file relate to the allocation of available bits to
subbands within the SBC/eSBC frame, along with support functions for computing
frame length and bitrate.

@ingroup esbc_lib
*/

///@addtogroup esbc_lib
///@{

#include "oi_codec_sbc_private.h"

OI_UINT32 OI_SBC_MaxBitpool(OI_CODEC_SBC_FRAME_INFO *frame)
{
    switch (frame->mode) {
        case SBC_MONO:
        case SBC_DUAL_CHANNEL:
            return 16 * frame->nrof_subbands;
        case SBC_STEREO:
        case SBC_JOINT_STEREO:
            return 32 * frame->nrof_subbands;
    }

    ERROR(("Invalid frame mode %d", frame->mode));
    OI_ASSERT(FALSE);
    return 0; /* Should never be reached */
}

OI_UINT32 OI_CODEC_SBC_CalculateBitrate(OI_CODEC_SBC_FRAME_INFO *frame)
{
    OI_UINT blocksbands;
    blocksbands = frame->nrof_subbands * frame->nrof_blocks;

    return DIVIDE(8 * OI_CODEC_SBC_CalculateFramelen(frame) * frame->frequency, blocksbands);
}


OI_UINT16 OI_CODEC_SBC_CalculateFramelen(OI_CODEC_SBC_FRAME_INFO *frame)
{
    OI_UINT16 nbits = frame->nrof_blocks * frame->bitpool;
    OI_UINT16 nrof_subbands = frame->nrof_subbands;
    OI_UINT16 result = nbits;

    if (frame->mode == SBC_JOINT_STEREO) {
        result += nrof_subbands + (8 * nrof_subbands);
    } else {
    if (frame->mode == SBC_DUAL_CHANNEL) { result += nbits; }
        if (frame->mode == SBC_MONO) { result += 4*nrof_subbands; } else { result += 8*nrof_subbands; }
    }
    return SBC_HEADER_LEN + (result + 7) / 8;
}


INLINE OI_UINT16 OI_SBC_CalculateFrameAndHeaderlen(OI_CODEC_SBC_FRAME_INFO *frame, OI_UINT *headerLen_)
{
    OI_UINT headerLen = SBC_HEADER_LEN + frame->nrof_subbands * frame->nrof_channels/2;

    if (frame->mode == SBC_JOINT_STEREO) { headerLen++; }

    *headerLen_ = headerLen;
    return OI_CODEC_SBC_CalculateFramelen(frame);
}


OI_UINT16 OI_CODEC_SBC_CalculatePcmBytes(OI_CODEC_SBC_FRAME_INFO *frame)
{
    return sizeof(OI_INT16) * frame->nrof_subbands * frame->nrof_channels * frame->nrof_blocks;
}


#define MIN(x, y)  ((x) < (y) ? (x) : (y))


/*
 * Computes the bit need for each sample and as also returns a counts of bit
 * needs that are greater than one.
 */
static OI_UINT computeBitneed(OI_CODEC_SBC_COMMON_CONTEXT *common,
                              OI_UINT8 *bits,
                              OI_UINT ch)
{
    static const OI_INT8 offset4[4][4] = {
        { -1, 0, 0, 0 },
        { -2, 0, 0, 1 },
        { -2, 0, 0, 1 },
        { -2, 0, 0, 1 }
    };

    static const OI_INT8 offset8[4][8] = {
        { -2, 0, 0, 0, 0, 0, 0, 1 },
        { -3, 0, 0, 0, 0, 0, 1, 2 },
        { -4, 0, 0, 0, 0, 0, 1, 2 },
        { -4, 0, 0, 0, 0, 0, 1, 2 }
    };

    OI_UINT bitcount = 0;
    OI_INT loudness;
    const OI_UINT nrof_subbands = common->frameInfo.nrof_subbands;
    OI_UINT sb;
    OI_INT8 *scale_factor = &common->scale_factor[ch ? nrof_subbands : 0];

    if (common->frameInfo.alloc == SBC_SNR) {
        OPT_LOOP_HINT(nrof_subbands, 4, 8, 4);
        for (sb = 0; sb < nrof_subbands; sb++) {
            loudness = scale_factor[sb];
            if ((bits[sb] = loudness) > 1) {
                bitcount += loudness;
            }
        }
    } else {
        const OI_INT8 *offset;
        if (nrof_subbands == 4) {
            offset = offset4[common->frameInfo.freqIndex];
        } else {
            offset = offset8[common->frameInfo.freqIndex];
        }
        OPT_LOOP_HINT(nrof_subbands, 4, 8, 4);
        for (sb = 0; sb < nrof_subbands; sb++) {
            loudness = scale_factor[sb];
            if (loudness) {
                loudness -= offset[sb];
                if (loudness > 0)  {
                    loudness /= 2;
                }
                loudness += 5;
            }
            if ((bits[sb] = loudness) > 1) {
                bitcount += loudness;
            }
        }
    }
    return bitcount;
}


/*
 * Explanation of the adjustToFitBitpool inner loop.
 *
 * The inner loop computes the effect of adjusting the bit allocation up or
 * down. Allocations must be 0 or in the range 2..16. This is accomplished by
 * the following code:
 *
 *           for (s = samples - 1; s >= 0; --s) {
 *              OI_INT bits = bitadjust + bitneeds[s];
 *              bits = bits < 2 ? 0 : bits;
 *              bits = bits > 16 ? 16 : bits;
 *              count += bits;
 *          }
 *
 * This loop can be optimized to perform 4 operations at a time as follows:
 *
 * Adjustment is computed as a 7 bit signed value and added to the bitneed.
 *
 * Negative allocations are zeroed by masking. (n & 0x40) >> 6 puts the
 * sign bit into bit 0, adding this to 0x7F give us a mask of 0x80
 * for -ve values and 0x7F for +ve values.
 *
 * n &= 0x7F + (n & 0x40) >> 6)
 *
 * Allocations greeate than 16 are truncated to 16. Adjusted allocations are in
 * the range 0..31 so we know that bit 4 indicates values >= 16. We use this bit
 * to create a mask that zeroes bits 0 .. 3 if bit 4 is set.
 *
 * n &= (15 + (n >> 4))
 *
 * Allocations of 1 are disallowed. Add and shift creates a mask that
 * eliminates the illegal value
 *
 * n &= ((n + 14) >> 4) | 0x1E
 *
 * These operations can be performed in 8 bits without overflowing so we can
 * operate on 4 values at once.
 */


/** Encoder/Decoder
 *
 * Computes adjustment +/- of bitneeds to fill bitpool and returns overall
 * adjustment and excess bits.
 */
static OI_INT adjustToFitBitpool(const OI_UINT bitpool,
                                 OI_UINT32 *bitneeds,
                                 const OI_UINT samples,
                                 OI_UINT bitcount,
                                 OI_UINT *excess)
{
    OI_INT maxBitadjust = 0;

    if (bitcount != bitpool) {
        OI_INT bitadjust = (bitcount > bitpool) ? -8 : 8;
        OI_INT chop = 8;
        while (chop) {
            OI_UINT32 total = 0;
            OI_UINT count;
            OI_UINT32 adjust4;
            OI_INT i;

            adjust4 = bitadjust & 0x7F;
            adjust4 |= (adjust4 << 8);
            adjust4 |= (adjust4 << 16);

            for (i = (samples / 4 - 1); i >= 0; --i) {
                OI_UINT32 mask;
                OI_UINT32 n = bitneeds[i] + adjust4;
                mask = 0x7F7F7F7F + ((n & 0x40404040) >> 6);
                n &= mask;
                mask = 0x0F0F0F0F + ((n & 0x10101010) >> 4);
                n &= mask;
                mask = (((n + 0x0E0E0E0E) >> 4) | 0x1E1E1E1E);
                n &= mask;
                total += n;
            }

            count = (total & 0xFFFF) + (total >> 16);
            count = (count & 0xFF) + (count >> 8);

            chop >>= 1;
            if (count > bitpool) {
                bitadjust -= chop;
            } else {
                maxBitadjust = bitadjust;
                bitcount = count;
                bitadjust += chop;
            }
        }
    }

    *excess = bitpool - bitcount;
    return maxBitadjust;
}

/* See adjustToFitBitpool for an explanation of the bit manipulation. The
 * operating being performed is essentially the same
 */
INLINE void allocateAdjustedBits(OI_UINT32 *common_bits, const OI_UINT32 *bitneeds, OI_UINT bands_times_channels, OI_INT bitadjust)
{
    OI_UINT i = bands_times_channels / 4;
    OI_UINT32 adjust_vector = bitadjust & 0x7f;

    adjust_vector |= adjust_vector << 8;
    adjust_vector |= adjust_vector << 16;

    OI_ASSERT(((long)common_bits)%4 == 0);
    OI_ASSERT(((long)bitneeds)%4 == 0);

    do {
        OI_UINT32 mask;
        OI_UINT32 bits = *bitneeds++ + adjust_vector;
        mask = 0x7f7f7f7f + ((bits & 0x40404040) >> 6);
        bits &= mask;
        mask = 0x0f0f0f0f + ((bits & 0x10101010) >> 4);
        bits &= mask;
        mask = (((bits + 0x0e0e0e0e) >> 4) | 0x1e1e1e1e);
        bits &= mask;
        *common_bits++ = bits;
    } while (--i);
}


/** Encoder/Decoder */
void computeStereoBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_INT i;
    const OI_UINT nrof_subbands = common->frameInfo.nrof_subbands;
    OI_UINT excess;
    OI_UINT ex;
    OI_UINT sb;
    OI_UINT ch;
    OI_UINT bitcount;
    union {
        OI_UINT8 uint8[2 * SBC_MAX_BANDS];
        OI_UINT32 uint32[2 * SBC_MAX_BANDS / 4];
    } bitneeds;
    OI_INT bitadjust;
    OI_INT bits;

    bitcount = computeBitneed(common, &bitneeds.uint8[0], 0);
    bitcount += computeBitneed(common, &bitneeds.uint8[nrof_subbands], 1);

    bitadjust = adjustToFitBitpool(common->frameInfo.bitpool, bitneeds.uint32, 2 * nrof_subbands, bitcount, &ex);
    /* ensure excess can go into a register */
    excess = ex;

    allocateAdjustedBits(common->bits.uint32, bitneeds.uint32, 2 * nrof_subbands, bitadjust);
   /*
     * Allocate excess bits starting with channel 0, subband 0
     */
    sb = 0;
    ch = 0;
    while (excess && (sb < nrof_subbands)) {
        i = sb + (ch ? nrof_subbands : 0);
        bits = common->bits.uint8[i];
        if (bits < 16) {
            if (bits) {
                common->bits.uint8[i] = bits + 1;
                --excess;
            } else if ((excess > 1) && (bitneeds.uint8[i] == (1 - bitadjust))) {
                common->bits.uint8[i] = 2;
                excess -= 2;
            }
        }
        sb += ch;
        ch ^= 1;
    }
    sb = 0;
    ch = 0;
    OI_ASSERT(excess <= (OI_INT)nrof_subbands);
    while (excess) {
        i = sb + (ch ? nrof_subbands : 0);
        bits = common->bits.uint8[i];
        if (bits < 16) {
            common->bits.uint8[i] = bits + 1;
            --excess;
        }
        sb += ch;
        ch ^= 1;
    }
}


/** Encoder/Decoder */
static void computeSeparateBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common,
                                         OI_UINT ch)
{
    const OI_UINT8 nrof_subbands = common->frameInfo.nrof_subbands;
    OI_UINT excess;
    OI_UINT ex;
    OI_UINT sb;
    OI_UINT bitcount;
    union {
        OI_UINT8 uint8[SBC_MAX_BANDS];
        OI_UINT32 uint32[SBC_MAX_BANDS / 4];
    } bitneeds;
    OI_INT bitadjust;
    OI_UINT8 * RESTRICT allocBits;
    OI_INT bits;

    bitcount = computeBitneed(common, bitneeds.uint8, ch);

    bitadjust = adjustToFitBitpool(common->frameInfo.bitpool, bitneeds.uint32, nrof_subbands, bitcount, &ex);
    /* ensure excess can go into a register */
    excess = ex;

    /*
     * Allocate adjusted bits
     */
    allocBits = &common->bits.uint8[ch ? nrof_subbands : 0];

    allocateAdjustedBits((OI_UINT32*)allocBits, bitneeds.uint32, nrof_subbands, bitadjust);
    /*
     * Allocate excess bits starting with subband 0
     */
    sb = 0;
    while (excess && (sb < nrof_subbands)) {
        bits = allocBits[sb];
        if (bits < 16) {
            if (bits) {
                allocBits[sb] = bits + 1;
                --excess;
            } else if ((excess > 1) && (bitneeds.uint8[sb] == (1 - bitadjust))) {
                allocBits[sb] += 2;
                excess -= 2;
            }
        }
        ++sb;
    }
    sb = 0;
    while (excess && (sb < nrof_subbands)) {
        bits = allocBits[sb];
        if (bits < 16) {
            allocBits[sb] = bits + 1;
            --excess;
        }
        ++sb;
    }
}


/** Encoder/Decoder */
PRIVATE void OI_SBC_ComputeBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_ASSERT(common->frameInfo.bitpool <= OI_SBC_MaxBitpool(&common->frameInfo));
    switch (common->frameInfo.mode) {
        case SBC_DUAL_CHANNEL:
            computeSeparateBitAllocation(common, 1);
            /* fall through */
        case SBC_MONO:
            computeSeparateBitAllocation(common, 0);
            break;
        case SBC_STEREO:
        case SBC_JOINT_STEREO:
            computeStereoBitAllocation(common);
            break;
    }
}

///@}
#endif /* FEATURE_BT_EXTPF_AV */
