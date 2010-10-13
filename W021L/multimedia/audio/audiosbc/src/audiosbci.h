#ifndef AUDIOSBCI_H
#define AUDIOSBCI_H

/*===========================================================================
 B L U E  T O O T H  S B C  C O D E C  I N T E R N A L  H E A D E R  F I L E

DESCRIPTION
  This header file contains all the definitions internal to the Bluetooth
  SBC codec module.

REFERENCES

Copyright (c) 2005 - 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/codec/sbc/main/latest/src/audiosbci.h#2 $ $DateTime: 2009/10/14 07:34:24 $ $Author: c_ypaida $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/14/09    dp     CMI related changes
04/17/07    sp     Fix for CR 110376. Update QTV when AV Sync information from
                   the DSP comes after the frame is sent to BT.
11/26/06    ac     Featurization to support FEATURE_QDSP_RTOS
11/20/06    sp     Fixed some AV Sync related issues.
07/26/06    sp     Added support for A2DP AV Sync
05/29/06   anb	   Added audiosbc_set_bit_pool_rates(),audiosbc_calc_bit_rate()
                   to support reconfiguration of bitpool values   
11/29/05    sp     Added flow control for DSP based SBC. 
11/16/05    sp     Changes to support SBC encoding in lower priority task.
09/18/05    sp     Added support for DSP based SBC encoder
08/29/05    sp     Added P4 header, made changes so that SBC parameters could
                   be changed for testing purposes.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP 
                   headset. Changed the default mode to Joint stereo. Fixed a
                   bug that resulted in bit rate reduced to half for stereo
                   headsets.
1/10/05     sp     Initial version 

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "target.h"             /* Target specific definitions            */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "bt.h" 
#include "btpf.h"
#include "audiosbc.h"

#ifndef FEATURE_SBC_DSP_CODEC
#include "oi_codec_sbc.h"       /* Header file of the third party's codec */

#else

#define SBC_MAX_CHANNELS 2
#define SBC_MAX_BANDS 8
#define SBC_MAX_BLOCKS 16
#define SBC_MIN_BITPOOL 2   /**< Minimum size of the bit allocation pool used to encode the stream */
#define SBC_MAX_BITPOOL 250 /**< Maximum size of the bit allocation pool used to encode the stream */

#define SBC_HEADER_LEN 4
#define SBC_MAX_FRAME_LEN (SBC_HEADER_LEN + \
                             ((SBC_MAX_BANDS * SBC_MAX_CHANNELS / 2) + \
                              (SBC_MAX_BANDS + SBC_MAX_BLOCKS * SBC_MAX_BITPOOL + 7)/8))
#define SBC_MAX_SAMPLES_PER_FRAME   (SBC_MAX_BANDS * SBC_MAX_BLOCKS)

#define SBC_FREQ_44100 2 /**< The sampling frequency is 44.1 kHz: one possible value for the @c frequency parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_FREQ_48000 3 /**< The sampling frequency is 48 kHz: one possible value for the @c frequency parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_MONO 0         /**< The mode of the encoded channel is mono: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_DUAL_CHANNEL 1 /**< The mode of the encoded channel is dual-channel: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_STEREO 2       /**< The mode of the encoded channel is stereo: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_JOINT_STEREO 3 /**< The mode of the encoded channel is joint stereo: one possible value for the @c mode parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_SUBBANDS_8  1 /**< The encoded stream has 8 subbands: one possible value for the @c subbands parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_BLOCKS_4    0 /**< A block size of 4 blocks was used to encode the stream: one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_8    1 /**< A block size of 8 blocks was used to encode the stream is : one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_12   2 /**< A block size of 12 blocks was used to encode the stream: one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_BLOCKS_16   3 /**< A block size of 16 blocks was used to encode the stream: one possible value for the @c blocks parameter of OI_CODEC_SBC_EncoderConfigure() */

#define SBC_LOUDNESS 0    /**< The bit allocation method: one possible value for the @c loudness parameter of OI_CODEC_SBC_EncoderConfigure() */
#define SBC_SNR 1         /**< The bit allocation method: one possible value for the @c loudness parameter of OI_CODEC_SBC_EncoderConfigure() */

#endif /* !FEATURE_SBC_DSP_CODEC */

#ifndef FEATURE_QDSP_RTOS
#include "sndihw.h"
#endif 

#define AUDIOSBC_MAX_SAMPLE_WORDS_PER_FRAME  (SBC_MAX_SAMPLES_PER_FRAME * 2)
#define AUDIOSBC_MAX_SAMPLE_BYTES_PER_FRAME  (SBC_MAX_SAMPLES_PER_FRAME * 2 * 2)

#define AUDIOSBC_BITRATE_MONO                 192000
#define AUDIOSBC_BITRATE_STEREO               (AUDIOSBC_BITRATE_MONO * 2)

#define AUDIOSBC_MAX_BACKLOG                  4
#define AUDIOSBC_BACKLOG_SLOT_FULL            0xFFFF

#ifdef FEATURE_AVS_SBCENC_TASK 
/* This signal is set when the diag_rpt_timer expires
*/
#define AUDIOSBC_RPT_TIMER_SIG       0x0001
#define AUDIOSBC_CMD_Q_SIG           0x0002
#define AUDIOSBC_CODEC_TIMER_SIG     0x0010

#ifdef FEATURE_AVS_SBC_EFS_TEST_STREAM
#define AUDIOSBC_EFS_STREAM_TIMER_SIG     0x0020
#endif

typedef enum   {
 AUDIOSBC_OP_INVALID,
 AUDIOSBC_OP_PROCESS_PCM,                       /* PCM for SBC encoding       */
 AUDIOSBC_OP_CHANGE_BITRATE,                    /* Request for Bitrate change */
}audiosbc_op_type;

typedef struct {
  q_link_type                link;       /* Command queue link                */
  audiosbc_op_type           cmd;        /* Audiosbc command to perform       */
} audiosbc_hdr_packet_type;

typedef struct {
  audiosbc_hdr_packet_type   hdr;
  snd_pp_buffer_type         pp_buffer;  /* Buffer received from DSP          */
}audiosbc_pcm_packet_type;

typedef struct {
  audiosbc_hdr_packet_type   hdr;
  snd_sbc_bitrate_type       bitrate_hint; /* Suggested bitrate enum          */
  uint32                     bitrate; /* Actual bitrate if the enum is CUSTOM */
}audiosbc_bitrate_packet_type;


typedef union {
  audiosbc_hdr_packet_type       hdr;
  audiosbc_pcm_packet_type       pcm;  
  audiosbc_bitrate_packet_type   bitrate_info;
}audiosbc_packets_type;

#endif

typedef enum {
  AUDIOSBC_BACKLOG_SLOT_NAVAIL,       /* A backlog slot is not available */
  AUDIOSBC_BACKLOG_SLOT_AVAIL         /* A backlog slot is available     */
}audiosbc_backlog_slot_state_type;

typedef struct {
  snd_pp_buffer_type                pp_buf;   /* PP buf that we are holding */
  uint8                             *pcm_ptr; /* Offset into the PP buf     */
  uint16                            buf_size; /* Size of pp buf remaining   */
  audiosbc_backlog_slot_state_type  state;    /* State of this index        */
#ifdef FEATURE_SBC_DSP_CODEC
  uint16                            num_frames;/* Number of frames remaining */
  uint16                            frame_size;/* Size of the SBC frame      */
#endif
}audiosbc_pcm_backlog_info_type;

typedef struct {
#ifndef FEATURE_SBC_DSP_CODEC
  /* Encoder's current context information */
  OI_CODEC_SBC_ENCODER_CONTEXT oi_context;
#endif /* !FEATURE_SBC_DSP_CODEC */

  /* Default Bitrate is set to 192000 per channel */
  uint32 bitrate;

  /* The following two are calculated based on the min and max bitpool 
  received from headset */
  uint32 min_bitrate;

  uint32 max_bitrate;

  /* Default mode is Mono */
  uint8 mode;

  /* Default number of subbands */
  uint8 num_subbands;

  /* Default number of blocks */
  uint8 num_blocks;

  /* Default alloc mode */
  uint8 alloc;

  /* Enhanced mode ? */
  boolean enhanced;

  /* Sample rate */
  uint8 sample_rate;

  /* Lenght of encoder frame */
  uint16 framelen;

  /* Number of PCM bytes */
  uint16 pcm_bytes;

  /* If the encoder is currently active */
  boolean sbc_active;

  uint16 num_sample_lengths ; 
                                
  /* Number of pcm samples, in bytes, per frame */
  uint32 num_pcm_sampleb_in_frame; 

  /* Buffer to hold data that was not sufficient to fit in a frame */
  uint8 hold_buffer[AUDIOSBC_MAX_SAMPLE_BYTES_PER_FRAME];

  /* Flag that says if the hold buffer is valid or not */
  boolean hold_buffer_valid;

  /* length of the hold buffer, if the buffer is valid */
  uint16 hold_buf_len;

  /* Sequence number of the sbc frames */
  uint32 sequence_number;

  uint32 time_stamp;

  /* Total number of bytes in a frame is a power of two, we save what power
  of two is the frame length here. This is used to calculate the number of
  frames required to encode the bytes received from DSP without doing a 
  division */
  uint8  power_of_two;

  /* Bit pool value returned by encoder config */
  uint32 min_bitpool;

  uint32 max_bitpool;

  uint32 bitpool;

  /* Temp. buffer that we use to store the encoded frame */
  uint8  sbc_frame_buffer[SBC_MAX_FRAME_LEN];

  /* Size of temp. buffer that we use to store the encoded frame */
  uint16 sbc_frame_buffer_size;

  /* If there is a backlog */
  boolean pcm_backlog;

  /* Data structure to store backlog information */
  audiosbc_pcm_backlog_info_type backlog[AUDIOSBC_MAX_BACKLOG];

  /* Backlog slot from where we should do the read the buffer to be sent */
  uint16 backlog_read_slot;
  
  /* Backlog slot where the new buffer arriving will be placed */
  uint16 backlog_write_slot;

  /* If audio playback has ended */
  boolean audio_done;

  bt_app_id_type bt_app_id;

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

  boolean        streaming_started;

  qword          avsync_offset;

  qword          cumulative_samples;

  qword          last_samples;

  qword          last_frame;

  qword          last_ack_frame;

#endif

}audiosbc_context_type;

typedef enum {
#ifndef FEATURE_SBC_DSP_CODEC
  AUDIOSBC_NUM_SUBBANDS_4,
#endif
  AUDIOSBC_NUM_SUBBANDS_8,
  AUDIOSBC_NUM_SUBBANDS_MAX
}audiosbc_num_subbands_type;

typedef enum {
  AUDIOSBC_BLOCK_LENGTH_4,
  AUDIOSBC_BLOCK_LENGTH_8,
  AUDIOSBC_BLOCK_LENGTH_12,
  AUDIOSBC_BLOCK_LENGTH_16,
  AUDIOSBC_BLOCK_LENGTH_MAX
}audiosbc_block_length_type;

typedef enum {
  AUDIOSBC_ALLOC_MODE_SNR,
  AUDIOSBC_ALLOC_MODE_LOUDNESS,
  AUDIOSBC_ALLOC_MODE_MAX
}audiosbc_alloc_mode_type;

/* Structure to match the OI's # defs with the actual values, this will be
in an array indexed by an enum */
typedef struct {
  uint32 actual;
  uint32 oi_value;
  uint32 standards_value;
}audiosbc_conv_type;

/*=======================================================================
FUNCTION  audiosbc_set_bit_pool_rates

DESCRIPTION
  This function configures the bit pool/rate values so that audiosbc
  can be re-configured with the new values that are being set with.
DEPENDENCIES
  audiosbc_sample_rate must be set before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_set_bit_pool_rates(uint8 min_bitpool,uint8 max_bitpool);
/* <EJECT> */
/*=========================================================================*/

/*=======================================================================
FUNCTION  audiosbc_calc_bit_rate

DESCRIPTION
  This function calculates the new sbc bitrate based on the bitpool values.
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_calc_bit_rate(void);
/* <EJECT> */
/*=========================================================================*/


#endif /* FEATURE_SBC_CODEC */
#endif /* AUDIOSBCI_H */
