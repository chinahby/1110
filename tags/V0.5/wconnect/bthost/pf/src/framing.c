#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/**
@file
Checksum and header-related functions.

@ingroup esbc_lib
*/

///@addtogroup esbc_lib
///@{

#include "oi_codec_sbc_private.h"
#include "oi_assert.h"
#include "oi_debug.h"

/* #define PRINT_SAMPLES */
/* #define PRINT_SCALEFACTORS */
/* #define DEBUG_CRC */

#if defined(PRINT_SAMPLES) || defined(PRINT_SCALEFACTORS)
#include <stdio.h>
#endif

static const OI_UINT16 freq_values[] = { 16000, 32000, 44100, 48000 };
static const OI_UINT8 block_values[] = { 4, 8, 12, 16 };
static const OI_UINT8 channel_values[] = { 1, 2, 2, 2 };
static const OI_UINT8 band_values[] = { 4, 8 };

#ifdef USE_WIDE_CRC
INLINE OI_CHAR crc_iterate(OI_UINT8 oldcrc, OI_UINT8 next)
{
    OI_UINT crc;
    OI_UINT idx;
    idx = oldcrc^next;
    crc = crc8_wide[idx >> 1];
    if (idx%2) {
        crc &= 0xff;
    } else {
        crc >>= 8;
    }

    return crc;
}

#ifdef SBC_4SUBBANDS
INLINE OI_CHAR crc_iterate_top4(OI_UINT8 oldcrc, OI_UINT8 next)
{
    OI_UINT crc;
    OI_UINT idx;
    idx = (oldcrc ^ next) >> 4;
    crc = crc8_wide[idx>>1];
    if (idx%2) {
        crc &= 0xff;
    } else {
        crc >>= 8;
    }

    return (oldcrc << 4) ^ crc;
}
#endif // SBC_4SUBBANDS

#else // USE_WIDE_CRC

#ifdef SBC_4SUBBANDS
INLINE OI_UINT8 crc_iterate_top4(OI_UINT8 oldcrc, OI_UINT8 next)
{
    return (oldcrc << 4) ^ crc8_narrow[(oldcrc^next) >> 4];
}
#endif // SBC_4SUBBANDS

#ifdef USE_NIBBLEWISE_CRC
INLINE OI_UINT8 crc_iterate(OI_UINT8 crc, OI_UINT8 next)
{
    crc = (crc << 4) ^ crc8_narrow[(crc^next) >> 4];
    crc = (crc << 4) ^ crc8_narrow[((crc>>4)^next)&0xf];

    return crc;
}

#else   // USE_NIBBLEWISE_CRC
INLINE OI_UINT8 crc_iterate(OI_UINT8 crc, OI_UINT8 next)
{
  return crc8_narrow[crc^next];
}

#endif  // USE_NIBBLEWISE_CRC

#endif // USE_WIDE_CRC


PRIVATE OI_UINT8 OI_SBC_CalculateChecksum(OI_CODEC_SBC_FRAME_INFO *frame, OI_BYTE const *data)
{
    OI_UINT i;
    OI_UINT8 crc = 0x0f;
    /* Count is the number of whole bytes subject to CRC. Actually, it's one
     * more than this number, because data[3] is the CRC field itself, which is
     * explicitly skipped. Since crc_iterate (should be) inlined, it's cheaper
     * spacewise to include the check in the loop. This shouldn't be much of a
     * bottleneck routine in the first place. */
    OI_UINT count = (frame->nrof_subbands * frame->nrof_channels / 2u) + 4;

    if (frame->mode == SBC_JOINT_STEREO && frame->nrof_subbands == 8) {
        count++;
    }

    for (i = 1; i < count; i++) {
        if (i != 3) {
            crc = crc_iterate(crc,data[i]);
        }
    }

#ifdef SBC_4SUBBANDS
    if (frame->mode == SBC_JOINT_STEREO && frame->nrof_subbands == 4) {
        crc = crc_iterate_top4(crc, data[i]);
    }
#endif // SBC_4SUBBANDS

    return crc;
}

void OI_SBC_ExpandFrameFields(OI_CODEC_SBC_FRAME_INFO *frame)
{
    frame->nrof_blocks = block_values[frame->blocks];
    frame->nrof_subbands = band_values[frame->subbands];

    frame->frequency = freq_values[frame->freqIndex];
    frame->nrof_channels = channel_values[frame->mode];
}

#if defined(USE_PLATFORM_MEMMOVE) || defined(USE_PLATFORM_MEMCPY)
#include <string.h>
#endif
void shift_buffer(SBC_BUFFER_T *dest, SBC_BUFFER_T *src, OI_UINT wordCount)
{
#ifdef USE_PLATFORM_MEMMOVE
    memmove(dest, src, wordCount * sizeof(SBC_BUFFER_T));
#elif defined(USE_PLATFORM_MEMCPY)
    OI_ASSERT(((OI_CHAR *)(dest) - (OI_CHAR *)(src)) >= wordCount*sizeof(*dest));
    memcpy(dest, src, wordCount * sizeof(SBC_BUFFER_T));
#else
    OI_UINT n;
    OI_INT32 *d;
    OI_INT32 *s;
    n = wordCount / 4 / (sizeof(OI_INT32)/sizeof(*dest));
    OI_ASSERT((n * 4 * (sizeof(OI_INT32)/sizeof(*dest))) == wordCount);

    d = (void*)(dest + wordCount);
    s = (void*)(src + wordCount);

    do {
        COPY4WORDS_BACK(d, s);
    } while (--n);
#endif
}

///@}
#endif /* FEATURE_BT_EXTPF_AV */
