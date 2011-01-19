#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/**
 * @file
 *
 * This file drives SBC encoding.
 *
 * @ingroup esbc_lib
 */

///@addtogroup esbc_lib
///@{

#include "oi_codec_sbc_private.h"
#include "oi_bitstream.h"



OI_STATUS OI_CODEC_SBC_EncoderReset(OI_CODEC_SBC_ENCODER_CONTEXT *context)
{
    OI_UINT i;

    for (i = 0; i < sizeof(*context); i++) {
        ((char *)context)[i] = 0;
    }


    return OI_OK;
}

static void calculateBitpool(OI_CODEC_SBC_ENCODER_CONTEXT *context, OI_UINT32 *bitrate)
{
    OI_UINT32 x;
    OI_UINT32 rate;
    OI_UINT32 nrof_subbands;
    OI_UINT32 nrof_channels;
    OI_UINT32 nrof_blocks;
    OI_UINT32 joined;
    OI_UINT stereo;
    OI_UINT32 frequency;

    rate = *bitrate;
    nrof_subbands = context->common.frameInfo.nrof_subbands;
    nrof_channels = context->common.frameInfo.nrof_channels;
    nrof_blocks = context->common.frameInfo.nrof_blocks;
    stereo = (context->common.frameInfo.mode >= SBC_STEREO ? 1 : 0);
    joined = (context->common.frameInfo.mode == SBC_JOINT_STEREO ? 1 : 0);
    frequency = context->common.frameInfo.frequency;


    if (nrof_channels == 1 && rate > SBC_MAX_ONE_CHANNEL_BPS) {
        rate = SBC_MAX_ONE_CHANNEL_BPS;
    } else if (nrof_channels == 2 && rate > SBC_MAX_TWO_CHANNEL_BPS) {
        rate = SBC_MAX_TWO_CHANNEL_BPS;
    }

    x = nrof_subbands * rate * nrof_blocks - frequency * (32 + nrof_subbands * (4 * nrof_channels + joined));
    x = DIVIDE(x, nrof_blocks * DIVIDE(frequency,100)); /* All frequency values are divisible by 100. This gives us some extra precision later. */
    if (context->common.frameInfo.mode == SBC_DUAL_CHANNEL) {
        x >>= 1;
    }
    x = DIVIDE(x, 100);

    if (stereo && x > 32 * nrof_subbands) {
        x = 32 * nrof_subbands;
    } else if (!stereo && x > 16 * nrof_subbands) {
        x = 16 * nrof_subbands;
    }
    if (x < 2) {
        x = 2;
    } else if (x > 250) {
        x = 250;
    }


    context->common.frameInfo.bitpool = (OI_UINT8)x;
    *bitrate = OI_CODEC_SBC_CalculateBitrate(&context->common.frameInfo);
}

OI_STATUS OI_CODEC_SBC_EncoderReconfigure(OI_CODEC_SBC_ENCODER_CONTEXT *context,
                                          OI_UINT32 *bitrate,
                                          OI_UINT16 *framelen)
{
    calculateBitpool(context, bitrate);
    *framelen = OI_CODEC_SBC_CalculateFramelen(&context->common.frameInfo);

    return OI_OK;
}

OI_STATUS OI_CODEC_SBC_EncoderConfigure(OI_CODEC_SBC_ENCODER_CONTEXT *context,
                                        OI_BOOL enhanced,
                                        OI_UINT8 frequency,
                                        OI_UINT8 mode,
                                        OI_UINT8 subbands,
                                        OI_UINT8 blocks,
                                        OI_UINT8 alloc,
                                        OI_UINT32 *bitrate,
                                        OI_UINT16 *framelen)
{
    if (frequency > SBC_FREQ_48000) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (enhanced) {
#ifdef SBC_ENHANCED
        if (subbands != SBC_SUBBANDS_8) {
            return OI_STATUS_INVALID_PARAMETERS;
        }
#else
        return OI_STATUS_INVALID_PARAMETERS;
#endif // SBC_ENHANCED
    }

    if (mode > SBC_JOINT_STEREO) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

#ifndef SBC_4SUBBANDS
    if (subbands == SBC_SUBBANDS_4) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
#endif

#ifndef SBC_8SUBBANDS
    if (subbands == SBC_SUBBANDS_8) {
        return OI_STATUS_INVALID_PARAMETERS;
    }
#endif

    if (subbands > SBC_SUBBANDS_8) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (blocks > SBC_BLOCKS_16) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (alloc > SBC_SNR) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

    if (bitrate == NULL || framelen == NULL) {
        return OI_STATUS_INVALID_PARAMETERS;
    }

#ifdef SBC_ENHANCED
    context->common.frameInfo.enhanced = enhanced;
#else
    context->common.frameInfo.enhanced = FALSE;
#endif
    context->common.frameInfo.freqIndex = frequency;
    context->common.frameInfo.mode = mode;
    context->common.frameInfo.subbands = subbands;
    context->common.frameInfo.blocks = blocks;
    context->common.frameInfo.alloc = alloc;

    OI_SBC_ExpandFrameFields(&context->common.frameInfo);
    calculateBitpool(context, bitrate);
    *framelen = OI_CODEC_SBC_CalculateFramelen(&context->common.frameInfo);
    return OI_OK;
}


OI_STATUS OI_CODEC_SBC_EncodeFrame(OI_CODEC_SBC_ENCODER_CONTEXT *context,
                                   OI_INT16 pcmData[][2], OI_UINT32 *pcmBytes,
                                   OI_BYTE **frameData, OI_UINT32 *frameBytes)
{
    OI_BITSTREAM bs;
    OI_UINT framelen;
    OI_UINT frameSamples;
    OI_BYTE *frameStart;

    if (*frameBytes < SBC_HEADER_LEN) {
        return OI_CODEC_SBC_NOT_ENOUGH_HEADER_DATA ;
    }

    frameStart = *frameData;

    OI_SBC_WriteHeader(&context->common, *frameData);
    *frameData += SBC_HEADER_LEN;
    *frameBytes -= SBC_HEADER_LEN;

    framelen = OI_CODEC_SBC_CalculateFramelen(&context->common.frameInfo) - SBC_HEADER_LEN;
    frameSamples = context->common.frameInfo.nrof_blocks * context->common.frameInfo.nrof_subbands;

    if (*frameBytes < framelen) {
        return OI_CODEC_SBC_NOT_ENOUGH_BODY_DATA ;
    }

    if (*pcmBytes < (frameSamples * sizeof (OI_UINT16[2]))) {
        return OI_CODEC_SBC_NOT_ENOUGH_AUDIO_DATA;
    }

    *pcmBytes = frameSamples * sizeof(OI_UINT16[2]);
    OI_BITSTREAM_WriteInit(&bs, *frameData);

    OI_SBC_AnalyzeFrame(context, pcmData);
    if (context->common.frameInfo.mode == SBC_JOINT_STEREO) {
        OI_SBC_ComputeJointScalefactorsAndDoMidSide(&context->common);
    } else {
        OI_SBC_ComputeScalefactors(&context->common);
    }
    OI_SBC_WriteScalefactors(&context->common, &bs);
    OI_SBC_ComputeBitAllocation(&context->common);
    if (context->common.frameInfo.mode == SBC_JOINT_STEREO) {
        OI_SBC_WriteSamples(&context->common, &bs);
    } else {
        OI_SBC_WriteSamples(&context->common, &bs);
    }

    frameStart[3] = OI_SBC_CalculateChecksum(&context->common.frameInfo, frameStart);

    *frameData += framelen;
    *frameBytes -= framelen;

    return OI_OK;
}

void OI_SBC_WriteHeader(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BYTE *data)
{
    OI_CODEC_SBC_FRAME_INFO *frame;

    frame = &common->frameInfo;

#ifdef SBC_ENHANCED
    *data++ = frame->enhanced ? OI_SBC_ENHANCED_SYNCWORD : OI_SBC_SYNCWORD;
#else
    *data++ = OI_SBC_SYNCWORD;
#endif
    *data++ = ((frame->freqIndex & 0x3) << 6)
        | ((frame->blocks & 0x3) << 4)
        | ((frame->mode & 0x3) << 2)
        | ((frame->alloc & 0x1) << 1)
        | (frame->subbands & 0x1);

    *data++ = frame->bitpool;
    *data++ = 0;    /* This gets refilled by OI_CODEC_SBC_EncodeFrame(). */
}


#ifdef SBC_4SUBBANDS
static void OI_SBC_WriteScalefactors_4SB(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *bs)
{
    OI_INT8 *sf = common->scale_factor;
    OI_UINT count = common->frameInfo.nrof_channels * 4;

    OI_ASSERT(common->frameInfo.nrof_subbands == 4);
    if (common->frameInfo.mode == SBC_JOINT_STEREO) {
        OI_BITSTREAM_WriteUINT(bs, common->frameInfo.join, 4);
    }
    do {
        OI_BITSTREAM_WriteUINT(bs, (OI_UINT16)*sf, 4);
        ++sf;
    } while (--count);
}
#endif


#ifdef SBC_8SUBBANDS
static void OI_SBC_WriteScalefactors_8SB(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *bs)
{
    OI_INT8 *sf = common->scale_factor;
    OI_UINT count = (common->frameInfo.nrof_channels * 8) / 2;

    OI_ASSERT(common->frameInfo.nrof_subbands == 8);
    if (common->frameInfo.mode == SBC_JOINT_STEREO) {
        OI_BITSTREAM_WriteUINT8Aligned(bs, common->frameInfo.join);
    }
    do {
        OI_BITSTREAM_Write2xUINT4Aligned(bs, sf[0], sf[1]);
        sf += 2;
    } while (--count);
}
#endif


void OI_SBC_WriteScalefactors(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *bs)
{
#ifdef SBC_4SUBBANDS
    if (common->frameInfo.nrof_subbands == 4) {
        OI_SBC_WriteScalefactors_4SB(common, bs);
        return;
    }
#endif
#ifdef SBC_8SUBBANDS
    OI_SBC_WriteScalefactors_8SB(common, bs);
#endif
}



#ifdef DEBUG_ANALYSIS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static FILE *sub[8];
static FILE *subq[8];
static OI_BOOL first = TRUE;

#define INIT_DEBUG_ANALYSIS     \
if (first) {                \
    first = FALSE;          \
        atexit(close_files);    \
        open_files();           \
}


static void close_files(void)
{
    OI_UINT i;
    for (i=0;i<8;i++) {
        fclose(sub[i]);
        fclose(subq[i]);
    }
}

static void open_files(void)
{
    OI_UINT i;
    for (i = 0; i<8; i++) {
        char subname[] = "sub_.raw";
        char subqname[] =   "subq_.raw";

        subname[3] = '1'+i;
        subqname[4] = '1'+i;
        sub[i] = fopen(subname, "wb");
        if (sub[i] == NULL) {
            fprintf(stderr, "Failed to open debugging file %s\n", subname);
        }
        subq[i] = fopen(subqname,"wb");
        if (subq[i] == NULL) {
            fprintf(stderr, "Failed to open debugging file %s\n", subqname);
        }
    }
}

#else
#define INIT_DEBUG_ANALYSIS
#endif


void OI_SBC_WriteSamples(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *bs)
{
    OI_INT32 *s = common->subdata;
    OI_UINT nrof_loops = common->frameInfo.nrof_channels * common->frameInfo.nrof_subbands;
    OI_UINT blks = common->frameInfo.nrof_blocks;

    OI_UINT8 *bs_ptr = bs->ptr;
    OI_UINT32 bs_value = bs->value;
    OI_UINT bs_bitPtr = bs->bitPtr;

    INIT_DEBUG_ANALYSIS

        do {
            OI_UINT8 *bits_array = common->bits.uint8;
            OI_INT8 *scale_factor_array = common->scale_factor;
            OI_UINT loops = nrof_loops;

            do {
                OI_UINT quant;
                OI_UINT bits;


                bits = *bits_array++;
                if (bits) {
                    quant = OI_SBC_Quant(*s, *scale_factor_array, bits);
                    OI_ASSERT(quant < ((1u << bits)-1u));
                    OI_BITSTREAM_WRITEUINT(bs_ptr, bs_value, bs_bitPtr, quant, bits);
                }

#ifdef DEBUG_ANALYSIS
                {
                    static int sb = 0;
                    OI_INT16 value;

                    if (!bits) {
                        quant = 0;
                    }

                    value = (OI_INT16)*s;
                    fwrite(&value, 1, sizeof(value), sub[sb]);

                    if (bits) {
                        value = (OI_INT16)OI_SBC_Dequant(quant, *scale_factor_array, bits);
                    } else {
                        value = 0;
                    }
                    fwrite(&value, 1, sizeof(value), subq[sb]);

                    sb++;
                    if (sb >= common->frameInfo.nrof_subbands) {
                        sb = 0;
                    }


                }
#endif
                scale_factor_array++;
                s++;
            } while (--loops);
        } while (--blks);

    OI_BITSTREAM_WRITEFLUSH(bs_ptr, bs_value, bs_bitPtr);
}


#define SCALEFAC_BY_LOOKUP

#ifdef SCALEFAC_BY_LOOKUP

#undef SCALEFAC

static const OI_UINT8 scalefacTable[] = {
    0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8
};


OI_UINT SCALEFAC(OI_UINT32 x)
{
    if (x > 256) {
        return 8 + scalefacTable[x >> 8];
    } else {
        return scalefacTable[x];
    }
}

#else

OI_UINT scalefac(OI_UINT32 x)
{
    OI_UINT val = 0;

    OI_ASSERT(x >= 0);
    while (x >>= 1) { val++; }
    return val;
}

#endif


/** Encoder **/
PRIVATE void OI_SBC_ComputeScalefactors(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_UINT ch;
    OI_UINT sb;
    OI_UINT blk;
    OI_UINT nrof_channels;
    OI_UINT nrof_subbands;
    OI_UINT nrof_blocks;
    OI_INT32 *s;
    OI_UINT blockStride;

    nrof_blocks = common->frameInfo.nrof_blocks;
    nrof_channels = common->frameInfo.nrof_channels;
    nrof_subbands = common->frameInfo.nrof_subbands;

    blockStride = nrof_channels * nrof_subbands;

    for (ch = 0; ch < nrof_channels; ch++) {
        OI_UINT channelStride = ch * nrof_subbands;
        OI_INT32 *subdata = &common->subdata[channelStride];
        for (sb = 0; sb < nrof_subbands; sb++) {
            OI_UINT32 max = 0;
            s = &subdata[sb];
            /*
             * Scale factors are computed for the same subband in each block.
             */
            for (blk = 0; blk < nrof_blocks; blk++) {
                max |= ABS(*s);
                s += blockStride;
            }
            common->scale_factor[channelStride + sb] = SCALEFAC(max);
        }
    }
}

#ifdef SPECIALIZE_COMPUTE_JOINT
PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide4(OI_CODEC_SBC_COMMON_CONTEXT *common)
#define NROF_SUBBANDS 4
#include "computejoint.inc"
#undef NROF_SUBBANDS

PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide8(OI_CODEC_SBC_COMMON_CONTEXT *common)
#define NROF_SUBBANDS 8
#include "computejoint.inc"
#undef NROF_SUBBANDS

PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    if (common->frameInfo.nrof_subbands == 8) {
        OI_SBC_ComputeJointScalefactorsAndDoMidSide8(common);
    } else {
        OI_ASSERT(common->frameInfo.nrof_subbands == 4);
        OI_SBC_ComputeJointScalefactorsAndDoMidSide4(common);
    }
}

#else
PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide(OI_CODEC_SBC_COMMON_CONTEXT *common)
{
    OI_UINT nrof_subbands = common->frameInfo.nrof_subbands;

#define NROF_SUBBANDS nrof_subbands
#include "computejoint.inc"
#undef NROF_SUBBANDS
}
#endif

///@}
#endif /* FEATURE_BT_EXTPF_AV */
