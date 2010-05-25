#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #3 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _OI_CODEC_SBC_PRIVATE_H
#define _OI_CODEC_SBC_PRIVATE_H

/**
@file
Function prototypes and macro definitions used internally by the codec.

@ingroup esbc_lib
*/

///@addtogroup esbc_lib
///@{
#ifdef OI_CODE
#error code not present
#else /* OI_CODE */
#  ifdef RESTRICT
#    undef  RESTRICT
#  endif
#  define RESTRICT
#endif /* OI_CODE */

#ifdef CODEC_DEBUG
#include <stdio.h>
#define ERROR(x) do { printf x; printf("\n"); } while (0)
#else
#define ERROR(x)
#endif

#ifdef TRACE_EXECUTION
#define TRACE(x) do { printf x; printf("\n"); } while (0)
#else
#define TRACE(x)
#endif

#ifndef PRIVATE
#define PRIVATE
#endif

#ifndef INLINE
#define INLINE
#endif

#include "oi_codec_sbc.h"
#include "oi_platform.h"
#include "oi_sbc_tables-generated.h"
#include "oi_assert.h"

/** Used internally. */
typedef struct {
    OI_UINT8 *ptr;
    OI_UINT32 value;
    OI_UINT bitPtr;
} OI_BITSTREAM;


#define VALID_INT16(x) (((x) >= OI_INT16_MIN) && ((x) <= OI_INT16_MAX))
#define VALID_INT32(x) (((x) >= OI_INT32_MIN) && ((x) <= OI_INT32_MAX))

#define DCTII_8_SHIFT_IN 0
#define DCTII_8_SHIFT_OUT 16-DCTII_8_SHIFT_IN

#define DCTII_8_SHIFT_0 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_1 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_2 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_3 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_4 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_5 (DCTII_8_SHIFT_OUT)
#define DCTII_8_SHIFT_6 (DCTII_8_SHIFT_OUT-1)
#define DCTII_8_SHIFT_7 (DCTII_8_SHIFT_OUT-2)

#define DCT_SHIFT 15

#define DCTIII_4_SHIFT_IN 2
#define DCTIII_4_SHIFT_OUT 15

#define DCTIII_8_SHIFT_IN 3
#define DCTIII_8_SHIFT_OUT 14

PRIVATE OI_UINT32 OI_SBC_MaxBitpool(OI_CODEC_SBC_FRAME_INFO *frame);

PRIVATE void OI_SBC_ComputeBitAllocation(OI_CODEC_SBC_COMMON_CONTEXT *frame);
PRIVATE OI_UINT8 OI_SBC_CalculateChecksum(OI_CODEC_SBC_FRAME_INFO *frame, OI_BYTE const *data);

/* Transform functions */
PRIVATE void shift_buffer(SBC_BUFFER_T *dest, SBC_BUFFER_T *src, OI_UINT wordCount);
PRIVATE void cosineModulateSynth4(SBC_BUFFER_T * RESTRICT out, OI_INT32 const * RESTRICT in);
PRIVATE void SynthWindow40_int32_int32_symmetry_with_sum(OI_INT16 pcm[4][2], SBC_BUFFER_T buffer[80], OI_UINT channel);

#ifdef SBC_4SUBBANDS
INLINE void dct3_4(OI_INT32 * RESTRICT out, OI_INT32 const * RESTRICT in);
PRIVATE void analyze4_generated(SBC_BUFFER_T analysisBuffer[RESTRICT 40],
                                OI_INT16 pcm[4][2],
                                OI_UINT channel,
                                OI_INT32 subband[4]);
#endif // SBC_4SUBBANDS

#if defined(SBC_ENHANCED) || defined(SBC_8SUBBANDS)
INLINE void dct3_8(OI_INT32 * RESTRICT out, OI_INT32 const * RESTRICT in);
#endif

#ifdef SBC_8SUBBANDS
PRIVATE void analyze8_generated(SBC_BUFFER_T analysisBuffer[RESTRICT 80],
                                OI_INT16 pcm[8][2],
                                OI_UINT channel,
                                OI_INT32 subband[8]);
#endif

#ifdef SBC_ENHANCED
PRIVATE void analyze8_enhanced_generated(SBC_BUFFER_T analysisBuffer[RESTRICT 112],
                                         OI_INT16 pcm[8][2],
                                         OI_UINT channel,
                                         OI_INT32 subband[8]);
#endif

/* Decoder functions */

#ifdef SBC_DECODER
INLINE OI_STATUS OI_SBC_ReadHeader(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BYTE *data);
PRIVATE OI_STATUS OI_SBC_ValidateHeader(OI_CODEC_SBC_COMMON_CONTEXT *common);
PRIVATE void OI_SBC_ReadScalefactors(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *ob);
PRIVATE void OI_SBC_ReadSamples(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *ob);
PRIVATE void OI_SBC_SynthFrame(OI_CODEC_SBC_DECODER_CONTEXT *common, OI_INT16 pcm[][2]);
INLINE OI_INT32 OI_SBC_Dequant(OI_UINT32 raw, OI_UINT scale_factor, OI_UINT bits);
#endif // SBC_DECODER

/* Encoder functions */
#ifdef SBC_ENCODER
PRIVATE void OI_SBC_AnalyzeFrame(OI_CODEC_SBC_ENCODER_CONTEXT *context, OI_INT16 pcm[][2]);
PRIVATE void OI_SBC_ComputeScalefactors(OI_CODEC_SBC_COMMON_CONTEXT *common);
PRIVATE void OI_SBC_ComputeJointScalefactorsAndDoMidSide(OI_CODEC_SBC_COMMON_CONTEXT *common);
PRIVATE void OI_SBC_WriteHeader(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BYTE *data);
PRIVATE void OI_SBC_WriteScalefactors(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *ob);
PRIVATE void OI_SBC_WriteSamples(OI_CODEC_SBC_COMMON_CONTEXT *common, OI_BITSTREAM *ob);
INLINE OI_UINT16 OI_SBC_Quant(OI_INT32 value, OI_UINT scale_factor, OI_UINT bits);
#endif // SBC_ENCODER

PRIVATE void OI_SBC_ExpandFrameFields(OI_CODEC_SBC_FRAME_INFO *frame);

///@}

#endif /* _OI_CODEC_SBC_PRIVATE_H */

#endif /* FEATURE_BT_EXTPF_AV */

