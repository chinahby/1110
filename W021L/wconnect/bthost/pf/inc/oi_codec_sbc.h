#ifdef FEATURE_BT_EXTPF_AV
/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#ifndef _OI_CODEC_SBC_H
#define _OI_CODEC_SBC_H

/**
@file
Declarations of codec functions, data types, and macros.

@ingroup esbc_lib
*/

///@addtogroup esbc_lib
///@{

#include "oi_stddefs.h"
#include "oi_status.h"

#define SBC_MAX_CHANNELS 2
#define SBC_MAX_BANDS 8
#define SBC_MAX_BLOCKS 16
#define SBC_MIN_BITPOOL 2   /**< Minimum size of the bit allocation pool used to encode the stream */
#define SBC_MAX_BITPOOL 250 /**< Maximum size of the bit allocation pool used to encode the stream */
#define SBC_MAX_ONE_CHANNEL_BPS 320000
#define SBC_MAX_TWO_CHANNEL_BPS 512000


#define SBC_HEADER_LEN 4
#define SBC_MAX_FRAME_LEN (SBC_HEADER_LEN + \
                             ((SBC_MAX_BANDS * SBC_MAX_CHANNELS / 2) + \
                              (SBC_MAX_BANDS + SBC_MAX_BLOCKS * SBC_MAX_BITPOOL + 7)/8))
#define SBC_MAX_SAMPLES_PER_FRAME   (SBC_MAX_BANDS * SBC_MAX_BLOCKS)

#define SBC_MAX_SCALEFACTOR_BYTES ((4*(SBC_MAX_CHANNELS * SBC_MAX_BANDS) + 7)/8)

#define OI_SBC_SYNCWORD 0x9c
#define OI_SBC_ENHANCED_SYNCWORD 0x9d

#define SBC_FREQ_16000 0 /**< The sampling frequency is 16 kHz: one possible value for the @c frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_FREQ_32000 1 /**< The sampling frequency is 32 kHz: one possible value for the @c frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_FREQ_44100 2 /**< The sampling frequency is 44.1 kHz: one possible value for the @c frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_FREQ_48000 3 /**< The sampling frequency is 48 kHz: one possible value for the @c frequency parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_MONO 0         /**< The mode of the encoded channel is mono: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_DUAL_CHANNEL 1 /**< The mode of the encoded channel is dual-channel: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_STEREO 2       /**< The mode of the encoded channel is stereo: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_JOINT_STEREO 3 /**< The mode of the encoded channel is joint stereo: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_SUBBANDS_4  0 /**< The encoded stream has 4 subbands: one possible value for the @c subbands parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_SUBBANDS_8  1 /**< The encoded stream has 8 subbands: one possible value for the @c subbands parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_BLOCKS_4    0 /**< A block size of 4 blocks was used to encode the stream: one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_8    1 /**< A block size of 8 blocks was used to encode the stream is : one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_12   2 /**< A block size of 12 blocks was used to encode the stream: one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_16   3 /**< A block size of 16 blocks was used to encode the stream: one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_LOUDNESS 0    /**< The bit allocation method: one possible value for the @c loudness parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_SNR 1         /**< The bit allocation method: one possible value for the @c loudness parameter of OI_CODEC_SBC_EncoderConfigure() */

/** Used internally. */
typedef struct {
    OI_UINT16 frequency;    /**< The sampling frequency. Input parameter. */
    OI_UINT8 freqIndex;

    OI_UINT8 nrof_blocks;   /**< The block size used to encode the stream. Input parameter. */
    OI_UINT8 blocks;


    OI_UINT8 nrof_subbands; /**< The number of subbands of the encoded stream. Input parameter. */
    OI_UINT8 subbands;

    OI_UINT8 mode;          /**< The mode of the encoded channel. Input parameter. */
    OI_UINT8 nrof_channels; /**< The number of channels of the encoded stream. */

    OI_UINT8 alloc;         /**< The bit allocation method. Input parameter. */
    OI_UINT8 bitpool;       /**< Size of the bit allocation pool used to encode the stream. Input parameter. */
    OI_UINT8 crc;           /**< Parity check byte used for error detection. */
    OI_UINT8 join;          /**< Whether joint stereo has been used. */
    OI_UINT8 enhanced;

    OI_UINT8 cachedInfo;    /**< Information about the previous frame */
} OI_CODEC_SBC_FRAME_INFO;

/** Used internally. */
typedef struct {
    OI_CODEC_SBC_FRAME_INFO frameInfo;
    OI_INT8 scale_factor[SBC_MAX_CHANNELS*SBC_MAX_BANDS];
    union {
        OI_UINT8 uint8[SBC_MAX_CHANNELS*SBC_MAX_BANDS];
        OI_UINT32 uint32[SBC_MAX_CHANNELS*SBC_MAX_BANDS/4];
    } bits;
    OI_INT32 subdata[SBC_MAX_BLOCKS*SBC_MAX_CHANNELS*SBC_MAX_BANDS];
    OI_BYTE formatByte;
} OI_CODEC_SBC_COMMON_CONTEXT;

typedef OI_INT16 SBC_BUFFER_T;

/** Opaque parameter to decoding functions; maintains context. */
typedef struct {
    OI_CODEC_SBC_COMMON_CONTEXT common;
    OI_UINT8 limit;
    OI_UINT8 limitSubbands;
    OI_UINT8 enhancedEnabled;
    OI_UINT  synthBufferOffset;
    SBC_BUFFER_T synthBuffer[SBC_MAX_CHANNELS][27*SBC_MAX_BANDS];
} OI_CODEC_SBC_DECODER_CONTEXT;

/** Opaque parameter to encoding functions; maintains context. */
typedef struct {
    OI_CODEC_SBC_COMMON_CONTEXT common;
    SBC_BUFFER_T analysisBuffer[SBC_MAX_CHANNELS][27*SBC_MAX_BANDS];
    OI_UINT analysisBufferOffset;
} OI_CODEC_SBC_ENCODER_CONTEXT;


/**
 * This function resets the decoder. The context must be reset when
 * changing streams, or if the following stream parameters change:
 * number of subbands, stereo mode, or frequency.
 *
 * @param context   Pointer to the decoder context structure to be reset.
 *
 * @param enhanced  If true, enhanced SBC operation is enabled. If enabled,
 *                  the codec will recognize the alternative syncword for
 *                  decoding an enhanced SBC stream. Enhancements should not
 *                  be enabled unless the stream is known to be generated
 *                  by an enhanced encoder, or there is a small possibility
 *                  for decoding glitches if synchronization were to be lost.
 */
OI_STATUS OI_CODEC_SBC_DecoderReset(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                    OI_BOOL                       enhanced);

/**
 * This function limits the kind of SBC frames that the Decoder will
 * process.  Its use is optional.  If used, it must be called after
 * calling OI_CODEC_SBC_DecoderReset(). After it is called, any calls
 * to OI_CODEC_SBC_DecodeFrame() with SBC frames that do not conform
 * to the Subband and Enhanced SBC setting will be rejected with an
 * OI_STATUS_INVALID_PARAMETERS return.
 *
 * @param context   Pointer to the decoder context structure to be limited.
 *
 * @param enhanced  If true, all frames passed to the decoder must be
 *                  Enhanced SBC frames. If false, all frames must be
 *                  standard SBC frames.
 *
 * @param subbands  May be set to SBC_SUBBANDS_4 or SBC_SUBBANDS_8. All
 *                  frames passed to the decoder must be encoded with
 *                  the requested number of subbands.
 *
 * */
OI_STATUS OI_CODEC_SBC_DecoderLimit(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                    OI_BOOL                       enhanced,
                                    OI_UINT8                      subbands);


/**
 * Decode one SBC frame.
 *
 * @param context       Pointer to a decoder context structure. The same context
 *                      must be used each time when decoding from the same stream.
 *
 * @param frameData     Address of a pointer to the SBC data to decode. This
 *                      value will be updated to point to the next frame after
 *                      successful decoding.
 *
 * @param frameBytes    Pointer to a UINT32 containing the number of available
 *                      bytes of frame data. This value will be updated to reflect
 *                      the number of bytes remaining after a decoding operation.
 *
 * @param pcmData       Address of an array of OI_INT16 pairs, which will be
 *                      populated with the decoded audio data. This address
 *                      is not updated.
 *
 * @param pcmBytes      Pointer to a UINT32 in/out parameter. On input, it
 *                      should contain the number of bytes available for pcm
 *                      data. On output, it will contain the number of bytes
 *                      written. Note that this differs from the semantics of
 *                      frameBytes.
 * */
OI_STATUS OI_CODEC_SBC_DecodeFrame(OI_CODEC_SBC_DECODER_CONTEXT *context,
                                   OI_BYTE **frameData,
                                   OI_UINT32 *frameBytes,
                                   OI_INT16 pcmData[][2],
                                   OI_UINT32 *pcmBytes);

/**
 * This function resets the encoder. The context must be reset when
 * changing streams, or if the following stream parameters change:
 * number of subbands, stereo mode.
 */
OI_STATUS OI_CODEC_SBC_EncoderReset(OI_CODEC_SBC_ENCODER_CONTEXT *context);

/**
 * This function sets the encoder parameters.
 *
 * @param context        Encoder context structure. The same context must be
 *                       used each time a frame is encoded into a given stream
 *
 * @param enhanced       Set to TRUE to enable Open Interface proprietary
 *                       quality enhancements. Streams generated with this
 *                       flag set will NOT operate correctly with a standard
 *                       decoder. An external mechanism, such as AVDTP
 *                       capabilties, should be used to determine whether
 *                       setting this flag is appropriate. Setting enhanced
 *                       mode to TRUE restricts subbands to SBC_SUBBANDS_8.
 *
 * @param frequency      One of SBC_FREQ_16000, SBC_FREQ_32000, SBC_FREQ_44100,
 *                       SBC_FREQ_48000
 *
 * @param mode           One of SBC_MONO, SBC_DUAL_CHANNEL, SBC_STEREO,
 *                       SBC_JOINT_STEREO
 *
 * @param subbands       One of SBC_SUBBANDS_4, SBC_SUBBANDS_8
 *
 * @param blocks         One of SBC_BLOCKS_4, SBC_BLOCKS_8, SBC_BLOCKS_12,
 *                       SBC_BLOCKS_16
 *
 * @param loudness       One of SBC_LOUDNESS, SBC_SNR
 *
 * @param bitrate        Pointer to a UINT32 in/out parameter. On input, a
 *                       requested bitrate specified in bits per second. On
 *                       output, the actual bitrate selected.
 *
 * @param framelen       Pointer to a UINT16 parameter which will contain the
 *                       size in bytes of a single SBC frame which uses the
 *                       parameters requested.
 */
OI_STATUS OI_CODEC_SBC_EncoderConfigure(OI_CODEC_SBC_ENCODER_CONTEXT *context,
                                        OI_BOOL enhanced,
                                        OI_UINT8 frequency,
                                        OI_UINT8 mode,
                                        OI_UINT8 subbands,
                                        OI_UINT8 blocks,
                                        OI_UINT8 loudness,
                                        OI_UINT32 *bitrate,
                                        OI_UINT16 *framelen);

/*
 * After the codec has been configured, it is permissable to change the bitrate
 * on the fly. This function allows the caller to do so.
 *
 * @param context        Encoder context structure. The same context must be
 *                       used each time a frame is encoded into a given stream
 *
 * @param bitrate        Pointer to a UINT32 in/out parameter. On input, a
 *                       requested bitrate specified in bits per second. On
 *                       output, the actual bitrate selected.
 *
 * @param framelen       Pointer to a UINT16 parameter which will contain the
 *                       size in bytes of a single SBC frame which uses the
 *                       parameters requested.
 */
OI_STATUS OI_CODEC_SBC_EncoderReconfigure(OI_CODEC_SBC_ENCODER_CONTEXT *context,
                                          OI_UINT32 *bitrate,
                                          OI_UINT16 *framelen);

/**
 * Encode a single SBC frame
 *
 * @param context      Encoder context structure. The same context must be
 *                     used each time a frame is encoded into a given stream.
 *
 * @param pcmData      Address of an array of OI_INT16 pairs, which will be
 *                     encoded into the SBC frame. This address is not updated.
 *
 * @param pcmBytes     Pointer to a UINT32 in/out parameter. On input, it
 *                     should contain the number of bytes of available pcm
 *                     data. On output, it will contain the number of bytes
 *                     used. Note that this differs from the semantics of
 *                     frameBytes.
 *
 * @param frameData    Address of a pointer to the buffer to contain the
 *                     encoded SBC frame. This address will be updated to
 *                     point to the first byte after the encoded data.
 *
 * @param frameBytes   Pointer to a UINT32 containing the number of available
 *                     bytes of space for encoded data. This value will be
 *                     updated to reflect the number of bytes remaining after
 *                     the encoding operation.
 */
OI_STATUS OI_CODEC_SBC_EncodeFrame(OI_CODEC_SBC_ENCODER_CONTEXT *context,
                                   OI_INT16 pcmData[][2],
                                   OI_UINT32 *pcmBytes,
                                   OI_BYTE **frameData,
                                   OI_UINT32 *frameBytes);

/* Common functions */

/**
   Calculate the frame length.

   @param frame The frame whose length to calculate

   @return the length of an individual encoded frame in
   bytes
 */
OI_UINT16 OI_CODEC_SBC_CalculateFramelen(OI_CODEC_SBC_FRAME_INFO *frame);

/**
   Calculate the bit rate.

   @param frame The frame whose bit rate to calculate

   @return the approximate bit rate in bits per second,
   assuming that stream parameters are constant
 */
OI_UINT32 OI_CODEC_SBC_CalculateBitrate(OI_CODEC_SBC_FRAME_INFO *frame);

/**
   Calculate decoded audio data length for one frame.

   @param frame The frame whose audio data length to calculate

   @return length of decoded audio data for a
   single frame, in bytes
 */
OI_UINT16 OI_CODEC_SBC_CalculatePcmBytes(OI_CODEC_SBC_FRAME_INFO *frame);

/**
 * Get the codec version text.
 *
 * @return  pointer to text string containing codec version text
 *
 */
OI_CHAR *OI_CODEC_Version(void);

///@}

#endif /* _OI_CODEC_SBC_H */


#endif /* FEATURE_BT_EXTPF_AV */
