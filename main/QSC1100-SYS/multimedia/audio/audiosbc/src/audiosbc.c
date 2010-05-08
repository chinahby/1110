/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("APIs for the Blue Tooth SBC Codec")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  B L U E  T O O T H  S B C  C O D E C  I N T E R F A C E  F U N C T I O N S  

GENERAL DESCRIPTION
  These routines are the interface to the Bluetooth SBC Codec.
  
REFERENCES
  None

EXTERNALIZED FUNCTIONS
  audiosbc_configure
  audiosbc_encode

Copyright(c) 2005 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/codec/sbc/main/latest/src/audiosbc.c#3 $ $DateTime: 2009/02/02 00:50:12 $ $Author: digishp $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/09    dp     Fixed CR 145197 incorrect logic in audioabc.c
11/18/08    dp     Ported fix for CR 146748
11/23/07   vsud    Moved the header file inclusion to sndtest.h
11/22/07   anb     Removed banned-api sprintf and used std_strlprintf instead
10/18/07   wfd     Wrap use of audiosbc_context.streaming_started with 
                   #ifdef FEATURE_AVS_A2DP_AVSYNC
10/15/07   anb     Fixed CR's 126265,112002,128224
10/02/07   wfd     Fixed CR 125926 - Reduce the maximum SCB bitrate by 1/8 to avoid
                   thoroughput issue with headsets with restricted buffer pools.
09/10/07   anb     Fixed CR 127583 - Lost of audio in starting WMA A2DP playback.
08/16/07    ac     Fixed compilation issue for 7k
07/05/07   anb     Removed the compiler error issue(removed the debug code)
07/03/07   anb     Fix for CR119961(SR 935533)
04/17/07    sp     Fix for CR 110376. Update QTV when AV Sync information from
                   the DSP comes after the frame is sent to BT.
01/15/07    sp     Fix for CR 107416. Video freezes after changing the device
                   to BT A2DP HS from wired headset.
11/26/06    ac     Featurization to support FEATURE_QDSP_RTOS
11/20/06    sp     Fixed some AV Sync related issues.
07/26/06    sp     Added support for A2DP AV Sync
05/29/06   anb	   Added audiosbc_set_bit_pool_rates(),audiosbc_calc_bit_rate()
                   to support reconfiguration of bitpool values   
05/19/06    sp     Fixed a bug in setting the bitrate supported by headset.
05/02/06    sg     Set default audiosbc_block_len to 16 since that seems to be
                   most commonly used (80597).
03/16/06   anb     Added support for sbc re-configuration given the encoded
                   params CR 78581, 87334.
11/29/05    sp     Added flow control for DSP based SBC. 
11/17/05    hz     Fixed compilation warning.
11/16/05    sp     Changes to support SBC encoding in lower priority task.
                   Added preliminary support for the streaming of SBC frames 
                   from EFS. FEATURE_AVS_SBC_EFS_TEST_STREAM has to be defined
                   to accomplish this.
11/09/05    sp/mr  Fixed Lint errors.
09/18/05    sp     Added support for DSP based SBC encoder
09/16/05    sp     Fixed a compiler error.
09/15/05    sk     Changed the name of the function that frees the qdsp buffer
08/29/05    sp     Cleanedup code. Made fixes so that A2DP suspend happens
                   in vocfunc.c alone, made changes so that SBC parameters could
                   be changed for testing pruposes.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP 
                   headset. Changed the default mode to Joint stereo. Fixed a
                   bug that resulted in bit rate reduced to half for stereo
                   headsets.
5/27/05     sp     Initial version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"           /* Customer Specific Definitions           */
#include "target.h"
#include "msg.h"

#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "audiosbci.h"        /* Module's internal typedefs                */
#ifdef FEATURE_QDSP_RTOS
/* Support added for new interface of QDSP RTOS*/
#include "audbtstub.h"
#endif /*FEATURE_QDSP_RTOS */ 
#ifdef SND_TEST_CODE
#include "fs.h"
#include "clk.h"
#include <stdio.h>
#include <stdlib.h>
#include "sndtest.h"
#endif /* SND_TEST_CODE */

#ifdef FEATURE_AVS_SBC_EFS_TEST_STREAM

#define         AUDIOSBC_EFS_STREAM_TEST_BUF_SIZE   500
#define         AUDIOSBC_EFS_STREAM_TEST_READ_SIZE  460
#define         AUDIOSBC_EFS_STREAM_TEST_FRAME_SIZE  46
bt_app_id_type  audiosbc_efs_streaming_bt_handle    = 0;  /* client id */
uint8           audiosbc_sbc_efs_start_streaming    = 0;
fs_handle_type  audiosbc_efs_stream_test_fs_handle  = NULL;
uint8           audiosbc_efs_stream_test_buf[AUDIOSBC_EFS_STREAM_TEST_BUF_SIZE];

typedef enum {
  AUDIOSBC_SBC_EFS_STREAMING_STOPPED,
  AUDIOSBC_SBC_EFS_STREAMING_CONNECT_PENDING,
  AUDIOSBC_SBC_EFS_STREAMING_STREAMING
}audiosbc_sbc_efs_streaming_state_type;

audiosbc_sbc_efs_streaming_state_type audiosbc_sbc_efs_streaming_state =
                                      AUDIOSBC_SBC_EFS_STREAMING_STOPPED;

dword audiosbc_efs_stream_timer_value = 55;
rex_timer_type audiosbc_efs_stream_timer;  /* Timer for sending frames */

#endif /* FEATURE_AVS_SBC_EFS_TEST_STREAM  */

#ifdef FEATURE_AVS_SBCENC_TASK 

#include "rex.h"                /* definition of REX data types           */
#include "task.h"               /* Task definitions and prototypes        */
#include "mc.h"                 /* Need for MC signals and prototypes     */
#include "dog.h"

/* SBC Encoding happens in a lower priority task */

rex_timer_type audiosbc_rpt_timer;    /* Watch Dog Timer               */

rex_timer_type audiosbc_codec_timer;  /* Timer for encoding and sending frames */

#define AUDIOSBC_NUM_PACKETS 5             /* # of Free Packets Allocated  */
q_type                audiosbc_cmd_q;      /* Request Packets Queue        */
q_type                audiosbc_free_q;     /* Available Packets Queue      */
audiosbc_packets_type audiosbc_free_packet[AUDIOSBC_NUM_PACKETS];

#define AUDIOSBC_CODEC_TIMER 10

#endif

uint16 audiosbc_num_frames_per_run  = 7;
/******************************************************************************
The SBC codec expects the PCM  data to consist of 2 strides each of the size
of a word (16 bits). i.e. the PCM data should be of the format

L R L R L R 

where L stands for Left channel sample and R stands for Right channel sample
and each sample is 16 bits long. 

This is valid even for Mono data. But for Mono data the R channel sample
could even be 0. DSP will send us the PCM data in this format.

Here we define sample length in bytes as 
4 = 2 (for two channels) * 2 (bytes per word)
*******************************************************************************/
uint8 audiosbc_sample_length = 4;

audiosbc_conv_type audiosbc_subband_conv[AUDIOSBC_NUM_SUBBANDS_MAX] = 
                {
#ifndef FEATURE_SBC_DSP_CODEC
                  {4, SBC_SUBBANDS_4,   AUDIOSBCENC_SUBBANDS_4},
#endif
                  {8, SBC_SUBBANDS_8,   AUDIOSBCENC_SUBBANDS_8},
                };
audiosbc_conv_type audiosbc_block_conv[AUDIOSBC_BLOCK_LENGTH_MAX] =
                {
                  {4,  SBC_BLOCKS_4,    AUDIOSBCENC_BLOCK_LENGTH_4},
                  {8,  SBC_BLOCKS_8,    AUDIOSBCENC_BLOCK_LENGTH_8},
                  {12, SBC_BLOCKS_12,   AUDIOSBCENC_BLOCK_LENGTH_12},
                  {16, SBC_BLOCKS_16,   AUDIOSBCENC_BLOCK_LENGTH_16},
                };

audiosbc_conv_type audiosbc_mode_conv[AUDIOSBC_CHANNEL_MODE_MAX] = 
                {
                  {0, 0,                0}, /* Invalid */
                  {0, SBC_MONO,         AUDIOSBCENC_CHANNEL_MODE_MONO},
                  {0, SBC_DUAL_CHANNEL, AUDIOSBCENC_CHANNEL_MODE_DUAL_CHANNEL},
                  {0, SBC_STEREO,       AUDIOSBCENC_CHANNEL_MODE_STEREO},
                  {0, SBC_JOINT_STEREO, AUDIOSBCENC_CHANNEL_MODE_JOINT_STEREO},
                };

audiosbc_conv_type audiosbc_sample_rate_conv[AUDIOSBC_SAMPLE_RATE_MAX] = 
               {
                 {0,  0,                0                                   },
                 {44100,SBC_FREQ_44100, AUDIOSBCENC_SAMPLING_FREQUENCY_44100}, 
                 {48000,SBC_FREQ_48000, AUDIOSBCENC_SAMPLING_FREQUENCY_48000},
               };

audiosbc_conv_type audiosbc_alloc_mode_conv[AUDIOSBC_ALLOC_MODE_MAX] = 
               {
                 {0, SBC_SNR,           AUDIOSBCENC_ALLOCATION_METHOD_SNR},
                 {0, SBC_LOUDNESS,      AUDIOSBCENC_ALLOCATION_METHOD_LOUDNESS},
               };

audiosbc_num_subbands_type audiosbc_num_subbands = AUDIOSBC_NUM_SUBBANDS_8;
audiosbc_block_length_type audiosbc_block_len    = AUDIOSBC_BLOCK_LENGTH_16;
audiosbc_alloc_mode_type   audiosbc_alloc        = AUDIOSBC_ALLOC_MODE_SNR;           

audiosbc_sample_rate_type  audiosbc_sample_rate  = 
                                           AUDIOSBC_SAMPLE_RATE_INVALID;
audiosbc_channel_mode_type audiosbc_channel_mode = 
                                           AUDIOSBC_CHANNEL_MODE_JOINT_STEREO;
boolean audiosbc_blocks_bands_changed = FALSE;

/* Ptr to BT cb func */
snd_sbc_frame_cb_func_ptr_type audiosbc_bt_cb = NULL;

/* The SBC encoder context */
audiosbc_context_type audiosbc_context;

#ifdef SND_TEST_CODE

#define AUDIOSBC_SBC_DEBUG_SIZE       (3*1024*1024)
uint32 asbc_sbc_debug_count = 0;
uint8* asbc_sbc_debug_ptr = NULL;
extern uint32 asbc_pcm_debug_count ;

fs_handle_type            audiosbc_pcm_handle = NULL;
fs_rsp_msg_type           audiosbc_snd_fs_rsp;
fs_open_xparms_type       audiosbc_open_parms;
char                      audiosbc_file_name[100];
uint16                    audiosbc_test_out = 0;
extern snd_test_block_type snd_test_block;

#endif /* SND_TEST_CODE */

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

typedef enum {
  AUDIOSBC_AVSYNC_SLOT_STATE_AVAIL,
  AUDIOSBC_AVSYNC_SLOT_STATE_PEND_INFORM,
  AUDIOSBC_AVSYNC_SLOT_STATE_INFORMED
} audiosbc_avsync_slot_state_type;

/* This is the unit of AV Sync information contained in each slot of the
avsync table */

typedef struct {
  /* timestampe, num_of_samples and num_of_bytes are exactly those that we
  received from the DSP */
  qword                           timestamp;
  qword                           num_of_samples;
  qword                           num_of_bytes;
  /* Sequence number is calculated based on the num_of_samples and the
  num of samples that go into a frame */
  qword                           seq_number;
  audiosbc_avsync_slot_state_type slot_state;
}audiosbc_avsync_info_type;

#define AUDIOSBC_AVSYNC_MAX_SLOT 100

audiosbc_avsync_info_type audiosbc_avsync_tbl[AUDIOSBC_AVSYNC_MAX_SLOT];

uint16 audiosbc_avsync_rindex = 0;
uint16 audiosbc_avsync_windex = 0;
uint16 audiosbc_avsync_iindex = 0;
uint8  audiosbc_avsync_log    = 0;

#endif
/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_init_context

DESCRIPTION
  This function initializes the BT SBC codec context. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_init_context
(
  audiosbc_context_type *context
)
{
  uint16 i;

  if (context == NULL)
  {
    MSG_ERROR("SBC Encoder:Invalid audiosbc context\n", 0,0,0);
    return;
  }
  /* Encoder will be enabled only when it is configured */
  audiosbc_disable_encoder();

  context->alloc = (OI_UINT8) audiosbc_alloc_mode_conv[audiosbc_alloc].oi_value;
  context->mode  = (OI_UINT8)audiosbc_mode_conv[audiosbc_channel_mode].oi_value;

  context->enhanced          = FALSE;
  context->framelen          = 0;
  context->hold_buf_len      = 0;
  context->hold_buffer_valid = FALSE;

  if ((audiosbc_channel_mode == AUDIOSBC_CHANNEL_MODE_STEREO) ||
      (audiosbc_channel_mode == AUDIOSBC_CHANNEL_MODE_JOINT_STEREO))
  {
    context->bitrate = (OI_UINT32)AUDIOSBC_BITRATE_HIGH * 2;
  }
  else
  {
    context->bitrate    = (OI_UINT32)AUDIOSBC_BITRATE_HIGH;
  }
  context->num_blocks   =(OI_UINT8)
                               audiosbc_block_conv[audiosbc_block_len].oi_value;
  context->num_subbands =(OI_UINT8) 
                          audiosbc_subband_conv[audiosbc_num_subbands].oi_value;

  context->sample_rate  = 
             (OI_UINT8)audiosbc_sample_rate_conv[audiosbc_sample_rate].oi_value;

  context->sequence_number = 0;
  context->time_stamp      = audiosbc_block_conv[audiosbc_block_len].actual *
                            audiosbc_subband_conv[audiosbc_num_subbands].actual;

  /* PCM bytes per frame is 
  2 (number of bytes per word) *
  2 (two strides) *
  8 (number of subbands) *
  16 (number of blocks) = 512, so the power of 2 is 9 */

  context->power_of_two          = 8;
  context->sbc_frame_buffer_size = SBC_MAX_FRAME_LEN;

  context->pcm_backlog = FALSE;
  context->audio_done  = FALSE;
  for (i = 0; i < AUDIOSBC_MAX_BACKLOG; i++)
  {
    context->backlog[i].buf_size               = 0;
    context->backlog[i].pcm_ptr                = NULL;
    context->backlog[i].pp_buf.pp_buf_length   = 0;
    context->backlog[i].pp_buf.pp_data         = NULL;
    context->backlog[i].pp_buf.pp_id           = 0;
    context->backlog[i].pp_buf.pp_num_channels = 0;
    context->backlog[i].state                  = AUDIOSBC_BACKLOG_SLOT_AVAIL;
  }
  context->backlog_read_slot  = 0;
  context->backlog_write_slot = 0;

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

 for (i = 0; i < AUDIOSBC_AVSYNC_MAX_SLOT; i++)
 {
   audiosbc_avsync_tbl[i].slot_state = AUDIOSBC_AVSYNC_SLOT_STATE_AVAIL;
   qw_set(audiosbc_avsync_tbl[i].seq_number     , 0,0);
   qw_set(audiosbc_avsync_tbl[i].num_of_bytes   , 0,0);
   qw_set(audiosbc_avsync_tbl[i].num_of_samples , 0,0);
   qw_set(audiosbc_avsync_tbl[i].timestamp      , 0,0);
 }
   context->streaming_started        = FALSE;

   audiosbc_avsync_rindex = 0;
   audiosbc_avsync_windex = 0;
   audiosbc_avsync_iindex = 0;

   qw_set(context->last_frame, 0,0);
   qw_set(context->last_ack_frame, 0,0);
   qw_set(context->cumulative_samples, 0,0);
   qw_set(context->last_samples, 0,0);

#endif /* FEATURE_SBC_DSP_CODEC && FEATURE_AVS_A2DP_AVSYNC */
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_configure

DESCRIPTION
  This function configures the BT SBC codec. This function must be called
  before we receive any PCM data for encoding. The only information that is
  required now is whether the BT headset is a stereo or mono device.

DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_configure( 
  audiosbcenc_data_type           *remote_params,
  bt_app_id_type                  bt_app_id,
  audiosbc_sample_rate_type       sample_rate
) {
#ifndef FEATURE_SBC_DSP_CODEC
  OI_STATUS status;
#endif
  uint32 blocks = 0;
  uint32 bands  = 0;


  if (remote_params == NULL)
  {
    MSG_ERROR("SBC Encoder:Invalid remote params \n", 0,0,0);
    return;

  }

  if ((sample_rate == AUDIOSBC_SAMPLE_RATE_INVALID) || 
      (sample_rate >= AUDIOSBC_SAMPLE_RATE_MAX))
  {
    MSG_ERROR("SBC Encoder: Invalid Sample rate \n", 0,0,0);
  }
  else
  {
    audiosbc_context.bt_app_id = bt_app_id;
    blocks = audiosbc_block_conv[audiosbc_block_len].actual;
    bands  = audiosbc_subband_conv[audiosbc_num_subbands].actual;

    audiosbc_sample_rate = sample_rate;
    MSG_MED("SBC encoder sample rate = %d, blocks = %d bands = %d\n", \
            audiosbc_sample_rate_conv[sample_rate].actual, blocks,bands);

    /* Set audiosbc_channel_mode based on remote_parameter channel mode */
    if ( remote_params->octet0.channel_mode & 
         AUDIOSBCENC_CHANNEL_MODE_JOINT_STEREO)
    {
      audiosbc_channel_mode = AUDIOSBC_CHANNEL_MODE_JOINT_STEREO;
    }
    else if ( remote_params->octet0.channel_mode & 
              AUDIOSBCENC_CHANNEL_MODE_STEREO)
    {
      audiosbc_channel_mode = AUDIOSBC_CHANNEL_MODE_STEREO;
    }
    else 
    {
      audiosbc_channel_mode = AUDIOSBC_CHANNEL_MODE_MONO;
    }

    audiosbc_init_context(&audiosbc_context);

    audiosbc_context.min_bitpool = remote_params->min_bitpool;
    audiosbc_context.max_bitpool = remote_params->max_bitpool;

    audiosbc_calc_bit_rate();

    MSG_HIGH("Audiosbc min bitpool = %d max bitpool = %d \n", 
              audiosbc_context.min_bitpool, audiosbc_context.max_bitpool, 0);
    MSG_HIGH("Audiosbc min bitrate = %d Maxbitrate = %d \n", 
              audiosbc_context.min_bitrate, audiosbc_context.max_bitrate, 0);

#ifdef FEATURE_SBC_DSP_CODEC
    audiosbc_context.sbc_active              = TRUE;
#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)
    audiosbc_context.streaming_started       = FALSE;
#endif /* FEATURE_SBC_DSP_CODEC && FEATURE_AVS_A2DP_AVSYNC */
    sndhw_sbc_configure(
      audiosbc_block_len, audiosbc_channel_mode, audiosbc_alloc);
    sndhw_sbc_set_bitrate(audiosbc_context.bitrate);
    audiosbc_context.num_sample_lengths = 
      audiosbc_block_conv[audiosbc_block_len].actual *
      audiosbc_subband_conv[audiosbc_num_subbands].actual;
                                        
#else /* FEATURE_SBC_DSP_CODEC */
    status = OI_CODEC_SBC_EncoderReset(&audiosbc_context.oi_context);
    if (status != OI_OK)
    {
      MSG_ERROR("SBC Encoder:Error in resetting the SBC codec %d\n", status,0,0);
    }
    else
    {
      status = OI_CODEC_SBC_EncoderConfigure(&audiosbc_context.oi_context,
                                             audiosbc_context.enhanced,
                                             audiosbc_context.sample_rate,
                                             audiosbc_context.mode,
                                             audiosbc_context.num_subbands,
                                             audiosbc_context.num_blocks,
                                             audiosbc_context.alloc,
                                             &audiosbc_context.bitrate,
                                             &audiosbc_context.framelen);
      if (status != OI_OK)
      {
        MSG_ERROR("SBC Encoder:Error in configuring the SBC codec %d\n", status,0,0);
      }
      else
      {
        audiosbc_context.sbc_active = TRUE;
        audiosbc_context.num_sample_lengths = 
                    audiosbc_context.oi_context.common.frameInfo.nrof_subbands * 
                    audiosbc_context.oi_context.common.frameInfo.nrof_blocks;

        audiosbc_context.num_pcm_sampleb_in_frame = 
        audiosbc_context.num_sample_lengths * audiosbc_sample_length;

        audiosbc_context.bitpool = 
          audiosbc_context.oi_context.common.frameInfo.bitpool ;
        MSG_HIGH("SBC actual bitpool = %d bitrate = %d \n", 
                  audiosbc_context.bitpool, audiosbc_context.bitrate, 0);

        audiosbc_context.pcm_bytes = 
        OI_CODEC_SBC_CalculatePcmBytes(
          &audiosbc_context.oi_context.common.frameInfo);
      }
    }
#endif /* FEATURE_SBC_DSP_CODEC */
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_get_backlog_slot()

DESCRIPTION
  THis function returns a backlog slot that is free
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint16 audiosbc_get_backlog_slot()
{
  uint16 retval = AUDIOSBC_BACKLOG_SLOT_FULL;
  uint16 index;

  if (audiosbc_context.backlog[audiosbc_context.backlog_write_slot].state ==
      AUDIOSBC_BACKLOG_SLOT_NAVAIL) {

    /* We are severly backlogged, lets drop the buffer at this location */

    index = 
     audiosbc_context.backlog[audiosbc_context.backlog_write_slot].pp_buf.pp_id;

    MSG_HIGH("Dropping DSP frames due to backlog\n",0,0,0);

    sndhw_free_wb_pcm_rx_interface_buffer(index);
  }
  audiosbc_context.backlog[audiosbc_context.backlog_write_slot].state =
                                                AUDIOSBC_BACKLOG_SLOT_NAVAIL;

  retval = audiosbc_context.backlog_write_slot; 
  if(audiosbc_context.pcm_backlog == FALSE)
  {
    audiosbc_context.pcm_backlog = TRUE;
    audiosbc_context.backlog_read_slot = retval;
  }

  /* Advance the write slot index */
  audiosbc_context.backlog_write_slot = 
        (audiosbc_context.backlog_write_slot + 1) % AUDIOSBC_MAX_BACKLOG;

  return retval;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_free_backlog_slot()

DESCRIPTION
  THis function the current read backlog slot and advaces the read pointer to
  the next slot.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_free_backlog_slot()
{
  uint16 next_slot;
  audiosbc_context.backlog[audiosbc_context.backlog_read_slot].state =
                                                   AUDIOSBC_BACKLOG_SLOT_AVAIL ;
  next_slot = (audiosbc_context.backlog_read_slot + 1) % AUDIOSBC_MAX_BACKLOG;
  if (audiosbc_context.backlog[next_slot].state == AUDIOSBC_BACKLOG_SLOT_NAVAIL)
  {
    audiosbc_context.backlog_read_slot = next_slot;
  }
  else
  {
    audiosbc_context.pcm_backlog = FALSE;
    audiosbc_context.backlog_read_slot = 0;
    audiosbc_context.backlog_write_slot = 0;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_encode

DESCRIPTION
 This function receives the PCM data from DSP and calls the SBC codec
 encoder function to encode the PCM data. This function sends the 
 encoded frames to the BT module through the callback provided by
 BT module.
          
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint16 audiosbc_encode(
  uint8** pcm_buffer, 
  uint16 *num_frames_req,
  sint31 dsp_bytes 
) {

  boolean encode = TRUE;
  uint16 need_bytes = 0;  /* Number of bytes needed to make a full frame */
  uint16 buffer_bytes = 0;  /* Number of bytes used from new buffer(pcm_buffer)
                               in a run */
  uint16 total_bytes_used = 0; /* Total num. of bytes used from new buffer 
                                (pcm_buffer) in run */
  uint16 total_bytes = 0; /* Total number of bytes = held_bytes + dsp_bytes */

  uint16 num_frames = 0;  /* Number of SBC frames reqd. to encode total_bytes */
  uint16 frame_count = 0;
  uint32 frame_buf_len = 0;

  uint8* audiosbc_frame_buffer = NULL;

  OI_INT16 (*pcm_data)[2]  = NULL;/* points pcm data for SBC frame will be taken */
  OI_STATUS encoder_status = OI_OK;

  if (pcm_buffer == NULL)       
  {
    MSG_ERROR("SBC Encoder:Invalid pcmbuffer to SBC encoder\n", 0,0,0);
    return 0;
  }
  if (num_frames_req == NULL)       
  {
    MSG_ERROR("SBC Encoder:Invalid num of frames req: NULL\n", 0,0,0);
    return 0;
  }

  #ifdef SND_TEST_CODE
  if ((asbc_sbc_debug_ptr == NULL)&&(snd_test_block.audiosbc_write_frame == 1))
  {
    asbc_sbc_debug_ptr = (uint8*)malloc(AUDIOSBC_SBC_DEBUG_SIZE);
    if (asbc_sbc_debug_ptr == NULL)
    {
      MSG_ERROR("SBC Encoder:debug memory alloc failure\n", 0,0,0);
    }
  }

  if (snd_test_block.audiosbc_write_pcm == 1)
  {
    if (audiosbc_pcm_handle == NULL)  
    {
      audiosbc_open_parms.create.cleanup_option   = FS_OC_CLOSE;
      audiosbc_open_parms.create.buffering_option = FS_OB_PROHIBIT;
      audiosbc_open_parms.create.attribute_mask   = FS_FA_UNRESTRICTED;

      memset(audiosbc_file_name, '\0', sizeof(audiosbc_file_name));
      std_strlprintf(audiosbc_file_name,sizeof(audiosbc_file_name),
                     "%s%ld","AUDIOSBC_pcm", clk_read_secs());

      fs_open(audiosbc_file_name, FS_OA_CREATE, &audiosbc_open_parms, NULL, 
              &audiosbc_snd_fs_rsp);
    }
    if (audiosbc_snd_fs_rsp.open.handle != NULL && 
          audiosbc_snd_fs_rsp.any.status == FS_OKAY_S) {

        audiosbc_pcm_handle = audiosbc_snd_fs_rsp.open.handle;
        fs_write(audiosbc_pcm_handle, *pcm_buffer, dsp_bytes , NULL, 
                 &audiosbc_snd_fs_rsp);
    }
  }

  #endif /* SND_TEST_CODE */

  if ((audiosbc_context.hold_buffer_valid) || 
      ((uint32)dsp_bytes < audiosbc_context.num_pcm_sampleb_in_frame))
  {
    /* Either the hold buffer is valid or the number of PCM bytes that we
    received from DSP is not sufficient to fill a frame */

    if (!audiosbc_context.hold_buffer_valid)
    {
      /* We are here because of insufficient bytes and not because our
      hold buffer is valid */
      audiosbc_context.hold_buf_len = 0;
    }
    /* Number of bytes, in addition to the bytes in the hold buffer, needed
    to fill a frame */
    need_bytes = (uint16)audiosbc_context.num_pcm_sampleb_in_frame - 
                 audiosbc_context.hold_buf_len;

    if (need_bytes <= dsp_bytes)
    {
      /* We have sufficient bytes to fill a frame now */
      buffer_bytes = need_bytes;
      total_bytes = (uint16)(audiosbc_context.hold_buf_len + dsp_bytes);

      pcm_data = (OI_INT16 (*)[2])audiosbc_context.hold_buffer;
      audiosbc_context.hold_buffer_valid = FALSE;
    }
    else
    {
      /* We still donot have sufficient bytes to fill a frame */
      buffer_bytes = (uint16)dsp_bytes;
      audiosbc_context.hold_buffer_valid = TRUE;
      encode = FALSE; /* We donot want to do any encoding now */
      /* we tell the caller that we donot have any SBC frames so
      no flush should happen */ 
      *num_frames_req = 0;
    }
    if((AUDIOSBC_MAX_SAMPLE_BYTES_PER_FRAME - audiosbc_context.hold_buf_len) >=
         buffer_bytes)
    {
      memcpy (((uint8*)(audiosbc_context.hold_buffer) + 
             audiosbc_context.hold_buf_len), *pcm_buffer, buffer_bytes);
      audiosbc_context.hold_buf_len += buffer_bytes;
    }
    else
    {
      MSG_ERROR("SBC encoder:Hold Buffer Full %d\n",0,0,0);
    }
    if(encode == FALSE)
    {
      /* We tell the caller that this DSP buffer can be freed */
      *pcm_buffer = NULL;
    }
  }
  else
  {
    /* Hold buffer is not valid and this buffer from DSP has enough bytes
    to fill in a frame */
    pcm_data = (OI_INT16 (*)[2])*pcm_buffer;
    buffer_bytes = (uint16)audiosbc_context.num_pcm_sampleb_in_frame;
    total_bytes = (uint16)dsp_bytes;
  }

  if (encode)
  {
    /* The number of frames that we can encode with the data available */
    num_frames = total_bytes / audiosbc_context.num_pcm_sampleb_in_frame ; 
    if (num_frames > *num_frames_req)
    {
      /* The number of frames that can be encoded using this DSP
      buffer is greater than the number of frames that we have been
      asked to encode */
      num_frames = *num_frames_req;
    }
    else
    {
      /* we could encode only num_frames but we have been asked to
      encode more */
      *num_frames_req = num_frames;
    }
    frame_count = num_frames;
    while (frame_count)
    {
      audiosbc_frame_buffer = audiosbc_context.sbc_frame_buffer;

      *pcm_buffer += buffer_bytes;

      /* This keeps count of the total number of bytes remaining */
      dsp_bytes -= buffer_bytes;

      /* This keeps count of the total number of bytes used from the
      pp buffer that we received */
      total_bytes_used += buffer_bytes;

      frame_buf_len = SBC_MAX_FRAME_LEN;

      encoder_status = 
        OI_CODEC_SBC_EncodeFrame(&audiosbc_context.oi_context, pcm_data, 
        &audiosbc_context.num_pcm_sampleb_in_frame, &audiosbc_frame_buffer, 
        &frame_buf_len);

      if (encoder_status != OI_OK)
      {
        MSG_ERROR("SBC Encoder:Error in SBC encoder %d\n", encoder_status,0,0);
        break;
      }
      pcm_data = (OI_INT16 (*)[2])*pcm_buffer;
      buffer_bytes = (uint16)audiosbc_context.num_pcm_sampleb_in_frame;
      frame_count--;
      if(audiosbc_bt_cb != NULL)
      {
        /* Send the data to BT */
        audiosbc_bt_cb(audiosbc_context.sbc_frame_buffer,
                       audiosbc_context.sequence_number,
                       audiosbc_context.time_stamp,
                       (frame_count ? FALSE : TRUE),
                       audiosbc_context.framelen
                       );
      }
#ifdef SND_TEST_CODE
      if ((asbc_sbc_debug_ptr != NULL) && 
          ((AUDIOSBC_SBC_DEBUG_SIZE - asbc_sbc_debug_count) > 
           audiosbc_context.framelen))
      {
        memcpy ((asbc_sbc_debug_ptr + asbc_sbc_debug_count), 
                (uint8*)audiosbc_context.sbc_frame_buffer, 
                audiosbc_context.framelen
                );
        asbc_sbc_debug_count += audiosbc_context.framelen;
      }
#endif /* SND_TEST_CODE */
    }
    audiosbc_context.sequence_number += num_frames;
    if ((dsp_bytes > 0) && 
        ((uint32)dsp_bytes < audiosbc_context.num_pcm_sampleb_in_frame))
    {
      if((AUDIOSBC_MAX_SAMPLE_BYTES_PER_FRAME - audiosbc_context.hold_buf_len) >=
           dsp_bytes)
      {

       /* We have less than one frame worth of PCM left in the DSP buffer, copy
       it to hold buffer and tell the caller that the DSP buffer can be freed */
       memcpy((uint8*)(audiosbc_context.hold_buffer), (uint8*)*pcm_buffer, 
             (uint32)dsp_bytes);

       audiosbc_context.hold_buf_len =(uint16) dsp_bytes;
       audiosbc_context.hold_buffer_valid = TRUE;
       /* This DSP buffer can be freed */
       *pcm_buffer = 0;
      } 
    }
    else if (dsp_bytes == 0) 
    {
      /* We have no bytes left on the DSP buffer, so this buffer can be freed */
      *pcm_buffer = 0;
    }
  }
  return total_bytes_used;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_audio_flush

DESCRIPTION
  This function will be called by snd to let us know that the audio playback
  is done. We will use this information to reset the sequence number and to
  send any remaining frames to BT.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_audio_flush(boolean send_held_frame)
{
#ifdef FEATURE_SBC_DSP_CODEC
  audiosbc_context.sequence_number = 0;
#else
  uint16 num_sampleb_left = 0;
  OI_STATUS encoder_status = OI_OK;
  uint32  audiosbc_frame_buffer_len = 0;
  uint8* audiosbc_frame_buffer = NULL;
  OI_INT16 (*pcmData)[2]  = NULL;

  if ((audiosbc_context.sbc_active) && (audiosbc_context.pcm_backlog == FALSE)&&
      (audiosbc_context.hold_buffer_valid) && send_held_frame)
  {
    num_sampleb_left = (uint16)(audiosbc_context.num_pcm_sampleb_in_frame - 
      audiosbc_context.hold_buf_len);

    audiosbc_frame_buffer = audiosbc_context.sbc_frame_buffer;
    /* lets zero pad the hold buffer */
    memset ((void*) ((uint8*)(audiosbc_context.hold_buffer)+
                              audiosbc_context.hold_buf_len),0, 
                              num_sampleb_left);
    pcmData = (OI_INT16 (*)[2])audiosbc_context.hold_buffer;

    audiosbc_frame_buffer_len = audiosbc_context.framelen;

    encoder_status = 
      OI_CODEC_SBC_EncodeFrame(&audiosbc_context.oi_context, pcmData, 
       &audiosbc_context.num_pcm_sampleb_in_frame, &audiosbc_frame_buffer,
       &audiosbc_frame_buffer_len);

    if (encoder_status != OI_OK)
    {
      MSG_ERROR("SBC Encoder:Error in SBC encoder %d\n", encoder_status,0,0);
      return;
    }
    else
    {
      if(audiosbc_bt_cb != NULL)
      {
        /* Send the data to BT */
        audiosbc_bt_cb(audiosbc_context.sbc_frame_buffer,
                       audiosbc_context.sequence_number,
                       audiosbc_context.time_stamp,
                       TRUE,
                       audiosbc_context.framelen
                       );
      }
#ifdef SND_TEST_CODE
      if ((asbc_sbc_debug_ptr != NULL) && 
          ((AUDIOSBC_SBC_DEBUG_SIZE - asbc_sbc_debug_count) > 
           audiosbc_context.framelen))
      {
        memcpy ((asbc_sbc_debug_ptr + asbc_sbc_debug_count), 
                (uint8*)audiosbc_context.sbc_frame_buffer, 
                audiosbc_context.framelen
                );
        asbc_sbc_debug_count += audiosbc_context.framelen;
      }
#endif /* SND_TEST_CODE */
    }
  }

  if ((audiosbc_context.pcm_backlog == FALSE ) || (send_held_frame == FALSE))
  {
    audiosbc_context.sequence_number = 0;
    audiosbc_context.hold_buffer_valid = FALSE;
    audiosbc_context.hold_buf_len = 0;
    memset((void*)audiosbc_context.hold_buffer, 0,
                        AUDIOSBC_MAX_SAMPLE_BYTES_PER_FRAME);

    audiosbc_disable_encoder();
    sndhw_bt_a2dp_suspend();

#ifdef SND_TEST_CODE
    if (audiosbc_pcm_handle != NULL)
    {
      fs_close(audiosbc_pcm_handle, NULL, &audiosbc_snd_fs_rsp);
      audiosbc_pcm_handle = NULL;
    }
#endif /* SND_TEST_CODE */
  }
  else
  {
    audiosbc_context.audio_done = TRUE;
  }

#endif
}

#ifdef FEATURE_AVS_SBCENC_TASK
/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_start_timer

DESCRIPTION
  This function sets the SBC timer. This function is called from
  SBC module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void audiosbc_start_timer(void)
{
  rex_set_timer( &audiosbc_codec_timer, AUDIOSBC_CODEC_TIMER);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_get_packet

DESCRIPTION
  Get a Audiosbc Command Packet.  
  
DEPENDENCIES

RETURN VALUE
  A pointer to an available snd_packets_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  audiosbc_free_q (and the packet fetched off audiosbc_free_q)

===========================================================================*/
LOCAL audiosbc_packets_type *audiosbc_get_packet( void )
{
  audiosbc_packets_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (audiosbc_packets_type *) q_get( &audiosbc_free_q );
  if( packet == NULL ) {
    ERR( "Ran Out of Audiosbc Packets!", 0, 0, 0 );
  } else {
#ifdef SND_DEBUG
    /* Fill the entire sound packet with invalid values to 
    ** catch any un-initialized fields.
    */
    (void) memset(packet, 0xff, sizeof(audiosbc_packets_type));
#else
    /* Fill the entire sound packet with 0's to get
    ** default behavior on un-initialized fields.
    */
    (void) memset(packet, 0, sizeof(audiosbc_packets_type));
#endif /* SND_DEBUG */
    packet->hdr.cmd        = AUDIOSBC_OP_INVALID;    /* Invalid Command Value */
  }

  return( packet );

} /* end of audiosbc_get_packet */
/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_cmd

DESCRIPTION
  Queue a command for processing by the Sbc encoder Task.

DEPENDENCIES
  This routine must not be called until the Sbc encoder Task has been started.

RETURN VALUE
  None

SIDE EFFECTS
  snd_cmd_q
  snd_tcb

===========================================================================*/
LOCAL void audiosbc_cmd
(
  audiosbc_packets_type *audiosbc_cmd_ptr
    /* pointer to sound command */
)
{
  /* Init link field */
  (void) q_link(audiosbc_cmd_ptr, &audiosbc_cmd_ptr->hdr.link );  

  /* Put on command queue  */
  q_put( &audiosbc_cmd_q, &audiosbc_cmd_ptr->hdr.link );    

  /* Signal a queue event  */
  (void) rex_set_sigs( &sbcenc_tcb, AUDIOSBC_CMD_Q_SIG ); 

} /* end of audiosbc_cmd */
/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_queue_pcm

DESCRIPTION
 This is the callback function called by SND task to send the PCM from
 DSP to SBC module. We will queue the buffer to SBC encoder task's queue.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_queue_pcm (const snd_pp_buffer_type pp_buffer)
{
  audiosbc_packets_type *packet;
    /* Pointer to Request Packet from Packet Pool */

  /* Get a Sound Packet and queue the Request.
  */
  packet = audiosbc_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> pcm.hdr.cmd          = AUDIOSBC_OP_PROCESS_PCM;
    packet -> pcm.pp_buffer        = pp_buffer; /* Do a copy */
    audiosbc_cmd(packet);
  }
  else
  {
    MSG_ERROR("New buffer received from DSP when no Audiosbc Packets available\
               \n", 0,0,0);
  }
}
#endif

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_frame_requires_ack

DESCRIPTION
  This is an internal function called to determine if this frame needs an
  ack from BT.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean audiosbc_frame_requires_ack()
{
  boolean ret_val = FALSE;

  qword      seq_num;
  sint7      seq_num_diff   = 0;
  uint16     i              = 0;

  qw_set(seq_num, 0, audiosbc_context.sequence_number);


  for (i = audiosbc_avsync_rindex; i < audiosbc_avsync_windex;)
  {
    seq_num_diff = qw_cmp(seq_num, audiosbc_avsync_tbl[i].seq_number); 

    if ((seq_num_diff == 0) && 
        (audiosbc_avsync_tbl[i].slot_state ==
          AUDIOSBC_AVSYNC_SLOT_STATE_PEND_INFORM))
    {
      ret_val = TRUE;
      if(audiosbc_avsync_log)
      {
        MSG_MED("Current asbc seq num = %d slot seq = %d slot state = %d", \
                  audiosbc_context.sequence_number, \
                  qw_lo( audiosbc_avsync_tbl[i].seq_number),\
                  audiosbc_avsync_tbl[i].slot_state);
      }
      audiosbc_avsync_tbl[i].slot_state = AUDIOSBC_AVSYNC_SLOT_STATE_INFORMED;
      break;
    }
    else if (seq_num_diff == -1)
    {
      /* We are not yet sending this frame */
      break;
    }
    else if (audiosbc_avsync_tbl[i].slot_state ==
               AUDIOSBC_AVSYNC_SLOT_STATE_PEND_INFORM)
    {
      ret_val = TRUE;
      if(audiosbc_avsync_log)
      {
        MSG_MED("Current asbc seq num = %d slot seq = %d slot state = %d", \
                  audiosbc_context.sequence_number, \
                  qw_lo( audiosbc_avsync_tbl[i].seq_number),\
                  audiosbc_avsync_tbl[i].slot_state);
      }
      audiosbc_avsync_tbl[i].slot_state = AUDIOSBC_AVSYNC_SLOT_STATE_INFORMED;
    }
    i++;
    if (i >= AUDIOSBC_AVSYNC_MAX_SLOT)
    {
      i -= AUDIOSBC_AVSYNC_MAX_SLOT;
    }
  }
  if (ret_val)
  {
    MSG_MED("AV Sync ack req for seq num = %d sent", \
              audiosbc_context.sequence_number, 0,0);
  }
  return ret_val;
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_pcm_cb_func

DESCRIPTION
 This is the callback function called by SND task to send the PCM from
 DSP to SBC module
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_pcm_cb_func (const snd_pp_buffer_type pp_buffer)
{

#ifdef FEATURE_SBC_DSP_CODEC
  uint16      *pp_ptr        = NULL;
  uint8       *sbc_ptr       = NULL;

  uint16      sbc_frame_size = 0;
  uint16      sbc_num_frames = 0;
/* Byte swapping isn't needed by new RTOS SVC  */
#ifndef FEATURE_QDSP_RTOS
  uint8       *sbc_temp_ptr  = NULL;
  sint31      sbc_num_bytes  = 0;
  uint8       temp_byte      = 0;
#endif /*FEATURE_QDSP_RTOS*/

  uint16      frames_used    = 0;

  /* Does this frame need an ack */
  boolean     ack_req        = FALSE; 

  /* Did we ever request an ack in this set */
  boolean     ack_requested  = FALSE; 
  uint16      backlog_index  = 0;

  pp_ptr = pp_buffer.pp_data;
  if (pp_ptr)
  {
    if (audiosbc_context.sbc_active == TRUE)
    {
      if (audiosbc_context.pcm_backlog == FALSE)
      {
        /* frame size is the first word */
        sbc_frame_size = *pp_ptr++; 
        /* Number of frames is in the second word */
        sbc_num_frames = *pp_ptr++;

        sbc_ptr  = (uint8*)pp_ptr;
/* Byte swapping isn't needed by new RTOS SVC  */
#ifndef FEATURE_QDSP_RTOS

        sbc_temp_ptr = sbc_ptr;

        sbc_num_bytes = sbc_frame_size * sbc_num_frames;

        /* Since we access the DSP buffer as bytes we need to do 
           byte swapping */
        while (sbc_num_bytes > 0)
        {
          temp_byte = *sbc_temp_ptr;
          *sbc_temp_ptr = *(sbc_temp_ptr + 1);
          *(sbc_temp_ptr + 1) = temp_byte;
          sbc_num_bytes -= 2;
          sbc_temp_ptr += 2;
        }
#endif /* FEATURE_QDSP_RTOS*/

        if(audiosbc_bt_cb != NULL)
        {

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

          if (!audiosbc_context.streaming_started)
          {
            audiosbc_context.streaming_started = TRUE;
          }
#endif

          while (sbc_num_frames && (frames_used < audiosbc_num_frames_per_run))
          {
            sbc_num_frames--;
            frames_used++;

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

            ack_req       = audiosbc_frame_requires_ack();
            ack_requested = ack_req;

            if(!sbc_num_frames && !ack_requested)
            {
              /* This is the last frame of this set and we have never requested
                 an ack for any frame in this set, so request an ack for this */
              ack_req = TRUE;
              MSG_MED("AV Sync ack req for seq num = %d sent", \
                        audiosbc_context.sequence_number, 0,0);
            } 
#endif

            /* Send the data to BT */
            audiosbc_bt_cb((uint8*)sbc_ptr,
                           audiosbc_context.sequence_number,
                           audiosbc_context.time_stamp,
                           ack_req, sbc_frame_size 
                           );

            audiosbc_context.sequence_number++;

            ack_req = FALSE;

            sbc_ptr += sbc_frame_size;
#ifdef FEATURE_QDSP_RTOS
            if(sbc_frame_size %2 != 0) {
              sbc_ptr++;
            }
#endif /* FEATURE_QDSP_RTOS*/

          }
        }

        if (sbc_num_frames == 0)
        {
          sndhw_free_wb_pcm_rx_interface_buffer(pp_buffer.pp_id);
        }
        else
        {
          backlog_index = audiosbc_get_backlog_slot();
          audiosbc_context.backlog[backlog_index].pp_buf     = pp_buffer;
          audiosbc_context.backlog[backlog_index].pcm_ptr    = sbc_ptr;
          audiosbc_context.backlog[backlog_index].num_frames = sbc_num_frames;
          audiosbc_context.backlog[backlog_index].frame_size = sbc_frame_size;
          /* We started off with no backlog and now we have a backlog, so
            lets start a timer */
          snd_sbc_start_timer();
         }
      }
      else
      {
        /* There is a backlog, so lets store this buffer */
        backlog_index = audiosbc_get_backlog_slot();
        MSG_MED("New buffer while backlogged: Slot = %d buf = %x\n", \
                backlog_index, pp_buffer.pp_data,0);

        sbc_frame_size                                     = *pp_ptr++; 
        sbc_num_frames                                     = *pp_ptr++;
        sbc_ptr                                            = (uint8*)pp_ptr;

/* Byte swapping isn't needed by new RTOS SVC  */
#ifndef FEATURE_QDSP_RTOS
        sbc_temp_ptr = sbc_ptr;
        sbc_num_bytes = sbc_frame_size * sbc_num_frames;

        /* Since we access the DSP buffer as bytes we need to do 
           byte swapping */
        while (sbc_num_bytes > 0)
        {
          temp_byte = *sbc_temp_ptr;
          *sbc_temp_ptr = *(sbc_temp_ptr + 1);
          *(sbc_temp_ptr + 1) = temp_byte;
          sbc_num_bytes -= 2;
          sbc_temp_ptr += 2;
        }
#endif /* FEATURE_QDSP_RTOS*/

        audiosbc_context.backlog[backlog_index].pp_buf     = pp_buffer;
        audiosbc_context.backlog[backlog_index].frame_size = sbc_frame_size;
        audiosbc_context.backlog[backlog_index].pcm_ptr    = sbc_ptr;
        audiosbc_context.backlog[backlog_index].num_frames = sbc_num_frames;

        /* Since we already know that there is a backlog no need to start a
        timer */
      }
    }
    else 
    {
      sndhw_free_wb_pcm_rx_interface_buffer(pp_buffer.pp_id);
    }
  }
  else
  {
    sndhw_free_wb_pcm_rx_interface_buffer(pp_buffer.pp_id);
  }
#else
  uint8* pcm_buffer = NULL;
  sint31 dsp_bytes = 0;
  uint16 num_frames_req = 0;
  uint16 dsp_bytes_remaining = 0;
  uint16 backlog_index = 0;
  uint16 bytes_used = 0;

  if (audiosbc_context.sbc_active == TRUE)
  {
    if (audiosbc_context.pcm_backlog == FALSE)
    {
      /* There is no backlog we can go ahead and encode this buffer */
      pcm_buffer = (uint8*) pp_buffer.pp_data;
      dsp_bytes = (sint31)(pp_buffer.pp_buf_length * 2);
      num_frames_req = audiosbc_num_frames_per_run;

      bytes_used = audiosbc_encode(&pcm_buffer, &num_frames_req, dsp_bytes);

      if (pcm_buffer == NULL)
      {
        sndhw_free_wb_pcm_rx_interface_buffer(pp_buffer.pp_id);
      }
      else
      {
        dsp_bytes_remaining = (uint16)
                                   ((pp_buffer.pp_buf_length * 2) - bytes_used);
        backlog_index = audiosbc_get_backlog_slot();
        audiosbc_context.backlog[backlog_index].pp_buf   = pp_buffer;
        audiosbc_context.backlog[backlog_index].buf_size = dsp_bytes_remaining;
        audiosbc_context.backlog[backlog_index].pcm_ptr = pcm_buffer;
        /* We started off with no backlog and now we have a backlog, so
        lets start a timer */
#ifdef FEATURE_AVS_SBCENC_TASK 
        audiosbc_start_timer();
#else
        snd_sbc_start_timer();
#endif
      }
    }
    else
    {
      /* There is a backlog, so lets store this buffer */
      backlog_index = audiosbc_get_backlog_slot();
      MSG_MED("New buffer while backlogged: Slot = %d buf = %x\n", \
              backlog_index, pp_buffer.pp_data,0);
      audiosbc_context.backlog[backlog_index].pp_buf   = pp_buffer;

      audiosbc_context.backlog[backlog_index].buf_size = 
        (uint16)(pp_buffer.pp_buf_length * 2);

      audiosbc_context.backlog[backlog_index].pcm_ptr = 
        (uint8*)pp_buffer.pp_data;

      /* Since we already know that there is a backlog no need to start a
      timer */
    }
  }
  else 
  {
    sndhw_free_wb_pcm_rx_interface_buffer(pp_buffer.pp_id);
  }
#endif /* FEATURE_SBC_DSP_CODEC */
}
/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_clear_backlog

DESCRIPTION
  This function is called periodically to clear the backlog. 
  This function is called from snd module.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void audiosbc_clear_backlog()
{
#ifdef FEATURE_SBC_DSP_CODEC
  uint16     backlog_index        = 0;
  uint16     num_frames           = 0; /* NUmber of frames encoded so far */
  uint16     backlog_frames       = 0;
  uint8*     sbc_ptr              = NULL;
  uint16     sbc_frame_size       = 0;
  boolean    ack_req              = FALSE;

  if (audiosbc_context.sbc_active == TRUE)
  {
    backlog_index = audiosbc_context.backlog_read_slot;

    sbc_ptr         = audiosbc_context.backlog[backlog_index].pcm_ptr;
    backlog_frames  = audiosbc_context.backlog[backlog_index].num_frames;
    sbc_frame_size  = audiosbc_context.backlog[backlog_index].frame_size;

    while ((audiosbc_context.pcm_backlog == TRUE) && backlog_frames &&
           (num_frames != audiosbc_num_frames_per_run))
    {
      num_frames++;
      backlog_frames--;

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)

      if(!backlog_frames)
      {
        ack_req = TRUE;
        MSG_MED("AV Sync ack req for seq num = %d sent", \
                  audiosbc_context.sequence_number, 0,0);
      } else {
        ack_req = FALSE;
      }
#endif

      /* Send the data to BT */
      audiosbc_bt_cb((uint8*)sbc_ptr,
                     audiosbc_context.sequence_number,
                     audiosbc_context.time_stamp,
                     ack_req, sbc_frame_size 
                     );

      sbc_ptr += sbc_frame_size;

#ifdef FEATURE_QDSP_RTOS      
      if(sbc_frame_size %2 != 0) {
         sbc_ptr++;
      }
#endif /* FEATURE_QDSP_RTOS */

      audiosbc_context.sequence_number++;
      ack_req = FALSE;
    }

    if (backlog_frames == 0)
    {
      /* we have used all the frames in this buffer */
      sndhw_free_wb_pcm_rx_interface_buffer(
        audiosbc_context.backlog[backlog_index].pp_buf.pp_id);
      audiosbc_free_backlog_slot();
    }
    else
    {
      audiosbc_context.backlog[backlog_index].num_frames    = backlog_frames;
      audiosbc_context.backlog[backlog_index].pcm_ptr       = sbc_ptr;
    }
  }
#else
  uint8* pcm_buffer = NULL;
  sint31 dsp_bytes = 0;
  uint16 num_frames_req = 0;
  uint16 num_frames     = 0; /* NUmber of frames encoded so far */
  uint16 dsp_bytes_remaining = 0;
  uint16 backlog_index = 0;
  uint16 bytes_used;

  while ((audiosbc_context.pcm_backlog == TRUE) &&
         (num_frames != audiosbc_num_frames_per_run))
  {
    if (audiosbc_context.sbc_active == TRUE)
    {
      backlog_index = audiosbc_context.backlog_read_slot;

      pcm_buffer = audiosbc_context.backlog[backlog_index].pcm_ptr;
      dsp_bytes = (sint31)audiosbc_context.backlog[backlog_index].buf_size;

      num_frames_req = audiosbc_num_frames_per_run - num_frames;

      bytes_used = audiosbc_encode(&pcm_buffer, &num_frames_req, dsp_bytes);
      /* Update the total number of frames encoded with the number of
      frames encoded in the current loop */
      num_frames += num_frames_req;
      dsp_bytes_remaining = audiosbc_context.backlog[backlog_index].buf_size -
                                                                   bytes_used; 

      if ((pcm_buffer == NULL) || (dsp_bytes_remaining == 0))
      {
        /* We are done with this buffer */
        sndhw_free_wb_pcm_rx_interface_buffer(
          audiosbc_context.backlog[backlog_index].pp_buf.pp_id);
        audiosbc_free_backlog_slot();
      }
      else
      {
        audiosbc_context.backlog[backlog_index].buf_size = dsp_bytes_remaining;
        audiosbc_context.backlog[backlog_index].pcm_ptr = pcm_buffer;
      }
    }
    else
    {
      /* This case can happen if while we are back logged sbc encoder is
      inactivated */
      sndhw_free_wb_pcm_rx_interface_buffer(
        audiosbc_context.backlog[backlog_index].pp_buf.pp_id);
      audiosbc_free_backlog_slot();
    }
  }
#endif

  if(audiosbc_context.pcm_backlog == TRUE)
  {
    /* There is still a backlog, we need to start the timer */
#ifdef FEATURE_AVS_SBCENC_TASK
    audiosbc_start_timer();
#else
    snd_sbc_start_timer();
#endif
  }
  else if (audiosbc_context.audio_done == TRUE)
  {
    /* We have eliminated all the backlog, snd has told us that audio
    is done, so lets flush out any remaining pcm in hold buffer */
    audiosbc_audio_flush(TRUE);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_init

DESCRIPTION
  This function initializes the BT SBC codec. Currently this function just
  registers the callback function with SND to receive the PCM data from
  DSP.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_init(snd_sbc_frame_cb_func_ptr_type sbc_cb)
{
  audiosbc_bt_cb = sbc_cb;
#ifdef FEATURE_AVS_SBCENC_TASK 
  snd_sbc_set_cb(audiosbc_queue_pcm, NULL, NULL);
#else
  snd_sbc_set_cb(audiosbc_pcm_cb_func, NULL, NULL);
#endif
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_set_bitrate_req

DESCRIPTION
  This functions sets the SBC codec's bitrate.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The size of the encoded frame will change and consequently the DSMs
  requested will be of a different size.
===========================================================================*/
boolean audiosbc_set_bitrate_req(snd_sbc_bitrate_type bitrate_hint, uint32 bps)
{
#ifndef FEATURE_SBC_DSP_CODEC
  OI_STATUS status;
#endif
  boolean   retval = TRUE;        /* SUccess */
  uint32 bitrate;

  switch (bitrate_hint) {
    case SND_SBC_BITRATE_CUSTOM:
      bitrate = bps; 
      break;

    case SND_SBC_BITRATE_LOWEST:
      bitrate = (uint32)AUDIOSBC_BITRATE_LOWEST;
      break;

    case SND_SBC_BITRATE_LOW:
      bitrate = (uint32)AUDIOSBC_BITRATE_LOW;
      break;

    case SND_SBC_BITRATE_MEDIUM: 
      bitrate = (uint32)AUDIOSBC_BITRATE_MEDIUM;
      break;

    case SND_SBC_BITRATE_HIGH:
      bitrate = (uint32)AUDIOSBC_BITRATE_HIGH;
      break;

    case SND_SBC_BITRATE_HIGHEST:
      bitrate = (uint32)AUDIOSBC_BITRATE_MAX;
      break;

    default:
      bitrate = (uint32)AUDIOSBC_BITRATE_HIGH;
      break;
  }
  if ((audiosbc_context.mode != SBC_MONO) && 
      (bitrate_hint != SND_SBC_BITRATE_CUSTOM)) {
    bitrate *= 2;
  }

  if ((bitrate >= audiosbc_context.min_bitrate) &&
      (bitrate <= audiosbc_context.max_bitrate)) {
    audiosbc_context.bitrate = bitrate;
    retval = TRUE;
  } else if (bitrate < audiosbc_context.min_bitrate) {
    audiosbc_context.bitrate = audiosbc_context.min_bitrate;
    retval = FALSE;
  } else if (bitrate > audiosbc_context.max_bitrate) {
    audiosbc_context.bitrate = audiosbc_context.max_bitrate;
    retval = FALSE;
  }
  MSG_MED("SBC encoder bitrate set to %d \n", audiosbc_context.bitrate, 0,0);

#ifdef FEATURE_SBC_DSP_CODEC
    sndhw_sbc_set_bitrate(audiosbc_context.bitrate);
#else

  status = OI_CODEC_SBC_EncoderReset(&audiosbc_context.oi_context);
  if (status != OI_OK)
  {
    MSG_ERROR("SBC Encoder:Error in reseting the SBC codec %d\n", status,0,0);
    retval = FALSE;
  }
  else
  {
    status = OI_CODEC_SBC_EncoderConfigure(&audiosbc_context.oi_context,
                                           audiosbc_context.enhanced,
                                           audiosbc_context.sample_rate,
                                           audiosbc_context.mode,
                                           audiosbc_context.num_subbands,
                                           audiosbc_context.num_blocks,
                                           audiosbc_context.alloc,
                                           &audiosbc_context.bitrate,
                                           &audiosbc_context.framelen);
    if (status != OI_OK)
    {
      MSG_ERROR("SBC Encoder:Error in configuring the SBC codec %d\n", status,0,0);
      retval = FALSE;
    }
    else
    {
      audiosbc_context.sbc_active = TRUE;
      audiosbc_context.num_sample_lengths = 
                    audiosbc_context.oi_context.common.frameInfo.nrof_subbands * 
                    audiosbc_context.oi_context.common.frameInfo.nrof_blocks;

      audiosbc_context.num_pcm_sampleb_in_frame = 
      audiosbc_context.num_sample_lengths * audiosbc_sample_length;
      audiosbc_context.pcm_bytes = 
      OI_CODEC_SBC_CalculatePcmBytes(
        &audiosbc_context.oi_context.common.frameInfo);

    }
  }
#endif
  return retval;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_get_config

DESCRIPTION
  This function is called by vocoder driver to get the configuration 
  information with which the encoder has been configured. 
  This information is to be passed on to the BT drivers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_get_config(audiosbc_config_type *config)
{
  if (config == NULL)
  {
    MSG_ERROR("SBC Encoder:invalid configuration pointer for sbc codec\n", 0,0,0);
    return;
  }  

  config->alloc        = 
    (uint8)audiosbc_alloc_mode_conv[audiosbc_alloc].standards_value;

  config->num_subbands = 
    (uint8)audiosbc_subband_conv[audiosbc_num_subbands].standards_value;

  config->block_length = 
    (uint8)audiosbc_block_conv[audiosbc_block_len].standards_value;

  config->sample_rate  = 
    (uint8)audiosbc_sample_rate_conv[audiosbc_sample_rate].standards_value;

  config->mode         = 
    (uint8)audiosbc_mode_conv[audiosbc_channel_mode].standards_value;

  config->min_bitpool = audiosbc_context.min_bitpool;
  config->max_bitpool = audiosbc_context.max_bitpool;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_set_params

DESCRIPTION
  This function sets the block length, channel mode and allocation method
  for the encoder
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_set_params(uint32 blocks, uint32 mode, uint32 alloc, uint32 bands)
{
  audiosbc_block_len    = (audiosbc_block_length_type)blocks;
  audiosbc_channel_mode = (audiosbc_channel_mode_type)mode;
  audiosbc_alloc        = (audiosbc_alloc_mode_type)alloc;
  audiosbc_num_subbands = (audiosbc_num_subbands_type)bands;
}
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
void audiosbc_calc_bit_rate(void)
{
  uint8 join = 0;
  uint8 channelnum = 1;
  uint32 frame_length_min;
  uint32 frame_length_max;
  uint32 tempvar_min = 0;
  uint32 tempvar_max = 0;
  uint32 tempvar = 0;
  uint32 blocks = 0;
  uint32 bands  = 0;

  /*calculate blocks & bands based on config SBC values..*/
  blocks = audiosbc_block_conv[audiosbc_block_len].actual;
  bands  = audiosbc_subband_conv[audiosbc_num_subbands].actual;

  /*calculate blocks & bands based on re-config SBC values..*/
  MSG_HIGH("audiosbc_channel_mode = %d\n", audiosbc_channel_mode, 0, 0);
  
  switch (audiosbc_channel_mode)
  {
    case AUDIOSBC_CHANNEL_MODE_MONO:
      join                  = 0;
      channelnum            = 1;
      break;
    
    case AUDIOSBC_CHANNEL_MODE_DUAL:
    case AUDIOSBC_CHANNEL_MODE_STEREO:
      join                  = 0;
      channelnum            = 2;
      break;
      
    case AUDIOSBC_CHANNEL_MODE_JOINT_STEREO:
      join                  = 1;
      channelnum            = 2;
      break;

    default:
      MSG_ERROR("audio: invalid audio sbc channel mode\n", 0, 0, 0);
      join                  = 0;
      channelnum            = 1;
      break;
  }


  /* Calculate the min and max bitrate from the min and max bitpool received
    from the headset. The formula is given is A2DP spec */
    if ((audiosbc_channel_mode == AUDIOSBC_CHANNEL_MODE_STEREO) ||
        (audiosbc_channel_mode == AUDIOSBC_CHANNEL_MODE_JOINT_STEREO))
    {
      tempvar_min = (join * bands) + (blocks * audiosbc_context.min_bitpool);
      tempvar_max = (join * bands) + (blocks * audiosbc_context.max_bitpool);
    }
    else
    {
      tempvar_min = channelnum * blocks * audiosbc_context.min_bitpool;
      tempvar_max = channelnum * blocks * audiosbc_context.max_bitpool;
    }
    /* Determine how many bytes it would take to hold each of the temp variable
     lengths by rounding up to next multiple of 8. */
    tempvar_min = (tempvar_min + 0x07)>>3;
    tempvar_max = (tempvar_max + 0x07)>>3;

   /* calculate frame length in bytes */
    frame_length_min = 4 + ((4 * bands * channelnum)>>3) + tempvar_min;
    frame_length_max = 4 + ((4 * bands * channelnum)>>3) + tempvar_max;

    tempvar = 
      audiosbc_sample_rate_conv[audiosbc_sample_rate].actual /(bands * blocks);

    /* convert from bytes per second to bits */
    audiosbc_context.min_bitrate = (frame_length_min * tempvar)<<3;
    audiosbc_context.max_bitrate = (frame_length_max * tempvar)<<3;

    /* ensure that max value is greater than min value */
    if (audiosbc_context.min_bitrate > audiosbc_context.max_bitrate) {
      /* swap min and max values */
      tempvar = audiosbc_context.min_bitrate;
      audiosbc_context.min_bitrate = audiosbc_context.max_bitrate;
      audiosbc_context.max_bitrate = tempvar;
    }
    if (audiosbc_context.min_bitrate != audiosbc_context.max_bitrate) {
      /* reduce the maximum bit rate by 1/8 to account for actual */
      /* variations in thoroughput, if min and max not the same. */
      audiosbc_context.max_bitrate -= (audiosbc_context.max_bitrate>>3);
      if (audiosbc_context.max_bitrate < audiosbc_context.min_bitrate) {
        /* clamp adjusted max value to be >= min value */
        audiosbc_context.max_bitrate = audiosbc_context.min_bitrate;
      }
    }
    
    if (audiosbc_context.bitrate >= audiosbc_context.max_bitrate ||
      audiosbc_context.bitrate <= audiosbc_context.min_bitrate) {
      /* defaulting it to Max Bit rate as per the requirement... */
      audiosbc_context.bitrate = audiosbc_context.max_bitrate ;
    }

  MSG_HIGH("Audiosbc calculated bitrate = %d \n", audiosbc_context.bitrate, 0, 0);
}
/*=====================================================================
FUNCTION  audiosbc_set_bit_pool_rate

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
void audiosbc_set_bit_pool_rates(uint8 min_bitpool,uint8 max_bitpool)
{
#ifndef FEATURE_SBC_DSP_CODEC
  OI_STATUS status;
#endif

  /*check if current bitpool values are modified or not....*/
   if((audiosbc_context.min_bitpool == min_bitpool) &&
      (audiosbc_context.max_bitpool == max_bitpool))
    {
    /*there is no change in the bit pool values too so return..*/
    return;
    }
  
    /*Set the new bit pool values in the context..*/
    audiosbc_context.min_bitpool = min_bitpool;
    audiosbc_context.max_bitpool = max_bitpool;

   /*Calculate the new max/min bitrates based on the new bit pool values..*/
    audiosbc_calc_bit_rate();

  /*re-config to new bit rate..*/
  /*Check if the sbc_encoder is active or not */
  if (audiosbc_context.sbc_active == TRUE)
  {
#ifdef FEATURE_SBC_DSP_CODEC
    sndhw_sbc_set_bitrate(audiosbc_context.bitrate);
#else
    status = OI_CODEC_SBC_EncoderReconfigure(&audiosbc_context.oi_context,
                  &audiosbc_context.bitrate,
                  &audiosbc_context.framelen);
    if (status != OI_OK)
    {
      MSG_ERROR("SBC Encoder:Error in re-configuring with new bitrate %d\n",
                status,0,0);
    }
    else
    {
        audiosbc_context.num_sample_lengths = 
                    audiosbc_context.oi_context.common.frameInfo.nrof_subbands * 
                    audiosbc_context.oi_context.common.frameInfo.nrof_blocks;

        audiosbc_context.num_pcm_sampleb_in_frame = 
        audiosbc_context.num_sample_lengths * audiosbc_sample_length;

        audiosbc_context.bitpool = 
          audiosbc_context.oi_context.common.frameInfo.bitpool ;

        audiosbc_context.pcm_bytes = 
        OI_CODEC_SBC_CalculatePcmBytes(
          &audiosbc_context.oi_context.common.frameInfo);
    }
#endif
  }
  else
  {
   MSG_MED("Sbc_encoder not active to set the new bit rate value..",0,0,0);
  }
}

/*===========================================================================

FUNCTION  audiosbc_set_enc_params

DESCRIPTION
  This function sets the actual audio sbc parameters block length, channel mode 
  and allocation method values from the audiosbcenc_data_type. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void audiosbc_set_enc_params(audiosbcenc_data_type *sbc_enc_data_ptr)
{
  uint8 sub_band_val;
  uint8 channel_val;
  uint8 block_val; 
  uint8 alloc_val; 

  /*Check for the validty of the encoder*/
  if (sbc_enc_data_ptr == NULL)
  {
    MSG_ERROR("SBC Encoder:Invalid sbc_enc_data_ptr to re-configure \n", 0,0,0);
    return;
  }

  sub_band_val = audiosbc_get_sub_bands(
                                  sbc_enc_data_ptr->octet1.subbands);
  channel_val = audiosbc_get_channel_mode(
                              sbc_enc_data_ptr->octet0.channel_mode);
  block_val = audiosbc_get_block_length(
                              sbc_enc_data_ptr->octet1.block_length);
  alloc_val = audiosbc_get_alloc_method(
                          sbc_enc_data_ptr->octet1.allocation_method);

  
   /*check if bands & blocks are changed..*/
   if((audiosbc_num_subbands != sub_band_val)|| 
      (audiosbc_block_len != block_val))
   {
   /*change in blocks & bands*/
    audiosbc_blocks_bands_changed = TRUE;
   }
   else
   {
    /*no change in bands & blocks*/
    audiosbc_blocks_bands_changed = FALSE;
   }
  
  /*check if correct values are set for sbc parameters 
   lest retain the default values.*/
  if((sub_band_val == AUDIOSBC_NUM_SUBBANDS_MAX) ||
     (channel_val == AUDIOSBC_CHANNEL_MODE_MAX ) ||
     (block_val == AUDIOSBC_BLOCK_LENGTH_MAX ) ||
     (alloc_val == AUDIOSBC_ALLOC_MODE_MAX))
  {
    MSG_ERROR("Invalid audiosbc_config Retaining original config values..",
       0,0,0);
    return;
  }
  else
  {
   audiosbc_num_subbands = (audiosbc_num_subbands_type)sub_band_val;
   audiosbc_channel_mode = (audiosbc_channel_mode_type)channel_val;
   audiosbc_block_len = (audiosbc_block_length_type)block_val;
   audiosbc_alloc = (audiosbc_alloc_mode_type)alloc_val;
  }
 
  /*Check if sbc encoder is active or not..*/
  if (audiosbc_context.sbc_active == TRUE)
  {
    /*re-configure the bitpool values if blocks - bands are not changed*/
    if(audiosbc_blocks_bands_changed == FALSE)
    {
      audiosbc_set_bit_pool_rates(sbc_enc_data_ptr->min_bitpool,
           sbc_enc_data_ptr->max_bitpool);
    }
   else
   {
      MSG_MED("blocks|bands got changed,cudnt set bitpool rates..",0,0,0);
   }
  }
}
/*===========================================================================

FUNCTION  audiosbc_get_block_length

DESCRIPTION
  This function returns the audiosbc value for block length given encoded
  block length value.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None

===========================================================================*/

uint8 audiosbc_get_block_length(uint8 enc_param_block_length)
{
   uint8 enc_block_len = AUDIOSBC_BLOCK_LENGTH_MAX;

   if ( enc_param_block_length & 
         AUDIOSBCENC_BLOCK_LENGTH_4)
   {
      enc_block_len = AUDIOSBC_BLOCK_LENGTH_4;
   }
   else if ( enc_param_block_length & 
               AUDIOSBCENC_BLOCK_LENGTH_8)
   {
      enc_block_len = AUDIOSBC_BLOCK_LENGTH_8;
   }
   else if ( enc_param_block_length & 
               AUDIOSBCENC_BLOCK_LENGTH_12)
   {
      enc_block_len = AUDIOSBC_BLOCK_LENGTH_12;
   }
   else if ( enc_param_block_length & 
               AUDIOSBCENC_BLOCK_LENGTH_16)
   {
      enc_block_len = AUDIOSBC_BLOCK_LENGTH_16;
   }   
   
   return enc_block_len;

}
/*===========================================================================

FUNCTION  audiosbc_get_channel_mode

DESCRIPTION
  This function returns the audiosbc value for channel mode given encoded
  channel mode value.
  
DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None

===========================================================================*/

uint8 audiosbc_get_channel_mode(uint8 enc_param_channel_mod)
{
   uint8 enc_channel_mod = AUDIOSBC_CHANNEL_MODE_MAX;

   if ( enc_param_channel_mod & 
         AUDIOSBCENC_CHANNEL_MODE_JOINT_STEREO)
   {
      enc_channel_mod = AUDIOSBC_CHANNEL_MODE_JOINT_STEREO;
   }
   else if ( enc_param_channel_mod & 
              AUDIOSBCENC_CHANNEL_MODE_STEREO)
   {
      enc_channel_mod = AUDIOSBC_CHANNEL_MODE_STEREO;
   }
   else if( enc_param_channel_mod & 
              AUDIOSBCENC_CHANNEL_MODE_MONO)
   {
      enc_channel_mod = AUDIOSBC_CHANNEL_MODE_MONO;
   }
   else if( enc_param_channel_mod & 
              AUDIOSBCENC_CHANNEL_MODE_DUAL_CHANNEL)
   {
      enc_channel_mod = AUDIOSBC_CHANNEL_MODE_DUAL;
   }
   
   return enc_channel_mod;
}
/*===========================================================================

FUNCTION  audiosbc_get_alloc_method

DESCRIPTION
  This function returns the audiosbc value for alloc method given encoded
  the alloc method value. 
  
DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None

===========================================================================*/

uint8 audiosbc_get_alloc_method(uint8 enc_param_alloc_method)
{
   uint8 enc_alloc_method = AUDIOSBC_ALLOC_MODE_MAX;

   if ( enc_param_alloc_method & 
                AUDIOSBCENC_ALLOCATION_METHOD_LOUDNESS)
   {
      enc_alloc_method = AUDIOSBC_ALLOC_MODE_LOUDNESS;
   }
   else if ( enc_param_alloc_method & 
             AUDIOSBCENC_ALLOCATION_METHOD_SNR)
   {
      enc_alloc_method = AUDIOSBC_ALLOC_MODE_SNR;
   } 

   return enc_alloc_method;
}
/*===========================================================================

FUNCTION  audiosbc_get_sub_bands

DESCRIPTION
  This function returns the audiosbc value for sub band given encoded
  the sub band. 
  
DEPENDENCIES
  None

RETURN VALUE
  uint8

SIDE EFFECTS
  None

===========================================================================*/
uint8 audiosbc_get_sub_bands(uint8 enc_param_sub_band)
{
   uint8 enc_sub_bands = AUDIOSBC_NUM_SUBBANDS_MAX;

  #ifndef FEATURE_SBC_DSP_CODEC
   if ( enc_param_sub_band & AUDIOSBCENC_SUBBANDS_4)
   {
      enc_sub_bands = AUDIOSBC_NUM_SUBBANDS_4;
   }
  #endif
   if ( enc_param_sub_band & AUDIOSBCENC_SUBBANDS_8)
   {
      enc_sub_bands = AUDIOSBC_NUM_SUBBANDS_8;
   } 
   return enc_sub_bands;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_disable_encoder

DESCRIPTION
  This function disables the encoder. Any more buffers received by the encoder
  will not be sent to BT
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_disable_encoder(void)
{
  MSG_MED("Disabling SBC encoder\n", 0,0,0);
  audiosbc_context.sbc_active = FALSE;
  audiosbc_blocks_bands_changed = FALSE;

}

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_set_bitrate

DESCRIPTION
  This functions sets the SBC codec's bitrate
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean audiosbc_set_bitrate(snd_sbc_bitrate_type bitrate_hint, uint32 bps)
{
  boolean retval = TRUE;

#ifdef FEATURE_AVS_SBCENC_TASK
  audiosbc_packets_type *packet;

  packet = audiosbc_get_packet();    /* Already Logs Error if Fails */
  if (packet != NULL) {
    packet -> bitrate_info.hdr.cmd          = AUDIOSBC_OP_CHANGE_BITRATE;
    packet -> bitrate_info.bitrate_hint     = bitrate_hint; 
    packet -> bitrate_info.bitrate          = bps;
    audiosbc_cmd(packet);
  }
  else
  {
    MSG_ERROR("Bitrate req received when no Audiosbc Packets available \n",
               0,0,0);
    retval = FALSE;
  }
#else
  retval = audiosbc_set_bitrate_req(bitrate_hint, bps);
#endif
  return retval;
}

#ifdef FEATURE_AVS_SBCENC_TASK 

/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the SBC encoder task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void audiosbc_shutdown( void )
{

  /* Acknowledge the stop signal
  */
#ifdef FEATURE_MULTIMODE_ARCH
  task_stop();
#else
  (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /*FEATURE_MULTIMODE_ARCH*/

  /* Clear the stop signal, and return
  */
  (void) rex_clr_sigs( &sbcenc_tcb, TASK_STOP_SIG );

} /* end of audiosbc_shutdown */

/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the audiosbc task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void audiosbc_offline( void )
{

  /* Acknowledge the offline signal
  */
#ifdef FEATURE_MULTIMODE_ARCH
  task_offline();
#else
  (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /*FEATURE_MULTIMODE_ARCH*/

  /* Clear the offline signal, and return
  */
  (void) rex_clr_sigs( &sbcenc_tcb, TASK_OFFLINE_SIG );

} /* end of snd_offline */


/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_req

DESCRIPTION
  This procedure processes requests received by audiosbc_cmd().  
  See audiosbci.h for a description of the actual packets.

DEPENDENCIES
  audiosbc_init() must have been called to init the command queue.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void audiosbc_req( void )
{
  audiosbc_packets_type *cmd_ptr;

  while ((cmd_ptr = (audiosbc_packets_type *) q_get(&audiosbc_cmd_q)) != NULL ){

    /* Process each command type
    */
    switch (cmd_ptr->hdr.cmd ) {
      case AUDIOSBC_OP_PROCESS_PCM:
        audiosbc_pcm_cb_func(cmd_ptr->pcm.pp_buffer);
      break;

      case AUDIOSBC_OP_CHANGE_BITRATE:
        audiosbc_set_bitrate_req(cmd_ptr->bitrate_info.bitrate_hint, 
                             cmd_ptr->bitrate_info.bitrate);
      break;
    }
    q_put( &audiosbc_free_q, (q_link_type *) cmd_ptr );
  }
}

#ifdef FEATURE_AVS_SBC_EFS_TEST_STREAM
/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_setup_efs_test_streaming

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_setup_efs_test_streaming()
{
  fs_rsp_msg_type           fs_rsp;
  audiosbcenc_data_type     sbc_data;
  const char               filename[] = "brew/mod/mediaplayer/media/test.sbc";

  if (audiosbc_sbc_efs_streaming_state == AUDIOSBC_SBC_EFS_STREAMING_STOPPED)
  {
    fs_open(filename, FS_OA_READONLY, 
                                                           NULL, NULL, &fs_rsp);
    if (fs_rsp.open.handle != NULL && fs_rsp.any.status == FS_OKAY_S)
    {
      audiosbc_efs_stream_test_fs_handle = fs_rsp.open.handle;
      sbc_data.octet0.sampling_frequency = AUDIOSBCENC_SAMPLING_FREQUENCY_44100; 
      sbc_data.octet0.channel_mode |= AUDIOSBCENC_CHANNEL_MODE_MONO; 
      sbc_data.octet1.block_length  =  AUDIOSBCENC_BLOCK_LENGTH_16;
      sbc_data.octet1.subbands     |= AUDIOSBCENC_SUBBANDS_8 ;
      sbc_data.octet1.allocation_method |=  
                                         AUDIOSBCENC_ALLOCATION_METHOD_LOUDNESS;
      sbc_data.min_bitpool = 10; 
      sbc_data.max_bitpool = 140; 
      (void) bt_cmd_pf_a2dp_start(audiosbc_efs_streaming_bt_handle, &sbc_data);
      audiosbc_sbc_efs_streaming_state = 
                                     AUDIOSBC_SBC_EFS_STREAMING_CONNECT_PENDING;
    }
    else
    {
      MSG_ERROR("Unable to open the test SBC file", 0,0,0);
      audiosbc_sbc_efs_start_streaming = 0;
    }
  } 
}

/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_do_efs_test_streaming

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_do_efs_streaming_test()
{
  fs_rsp_msg_type           fs_rsp;
  uint32                    buf_count = 0;
  static                    seq_num   = 0;

  if (audiosbc_sbc_efs_streaming_state == AUDIOSBC_SBC_EFS_STREAMING_STREAMING)
  {
    fs_read(audiosbc_efs_stream_test_fs_handle, audiosbc_efs_stream_test_buf,
            AUDIOSBC_EFS_STREAM_TEST_READ_SIZE, NULL, &fs_rsp);

    if ((fs_rsp.read.count != AUDIOSBC_EFS_STREAM_TEST_READ_SIZE) ||
        (fs_rsp.read.status == FS_EOF_S))
    {
      buf_count = fs_rsp.read.count / AUDIOSBC_EFS_STREAM_TEST_FRAME_SIZE;
      fs_close(audiosbc_efs_stream_test_fs_handle, NULL, &fs_rsp);
      audiosbc_efs_stream_test_fs_handle = NULL;
      audiosbc_sbc_efs_streaming_state = 
                                     AUDIOSBC_SBC_EFS_STREAMING_STOPPED;

      bt_cmd_pf_a2dp_suspend(audiosbc_efs_streaming_bt_handle);
      seq_num = 0;
    }
    else if (fs_rsp.read.count == AUDIOSBC_EFS_STREAM_TEST_READ_SIZE)
    {
      buf_count = fs_rsp.read.count / AUDIOSBC_EFS_STREAM_TEST_FRAME_SIZE;
      rex_set_timer(&audiosbc_efs_stream_timer, audiosbc_efs_stream_timer_value);
    }
    while(buf_count)
    {
      buf_count--;
      if(audiosbc_bt_cb != NULL)
      {
        /* Send the data to BT */
        MSG_ERROR("Sending sequence number %d", seq_num, 0,0);
        audiosbc_bt_cb(audiosbc_efs_stream_test_buf,
                       seq_num, 128, (buf_count ? FALSE : TRUE),
                      AUDIOSBC_EFS_STREAM_TEST_FRAME_SIZE );
        seq_num++;
      }
    }
  }
}
/*===========================================================================

FUNCTION audiosbc_efs_streaming_bt_ev_cb_func

DESCRIPTION
  Function to receive responses issued through app_id registration.

DEPENDENCIES
  The Bluetooth task must have been initialized first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_efs_streaming_bt_ev_cb_func (
  struct bt_ev_msg_struct* bt_ev_msg_ptr
) {
  switch(bt_ev_msg_ptr->ev_hdr.ev_type) {
    case BT_EV_PF_A2DP_START:
    MSG_HIGH("Audiosbc efs streaming START event received", 0,0,0);
      audiosbc_sbc_efs_streaming_state = AUDIOSBC_SBC_EFS_STREAMING_STREAMING;
      rex_set_timer(&audiosbc_efs_stream_timer, audiosbc_efs_stream_timer_value);
      break;
    default:
      break;
  }
}

#endif /* FEATURE_AVS_SBC_EFS_TEST_STREAM */


/* <EJECT> */
/*===========================================================================

FUNCTION audiosbcenc_task

DESCRIPTION
  This procedure is the entrance procedure for the SBC encoder Task.  It
  contains the main processing loop for the SBC encoder Task which controls
  queue processing and sbc encoding.

DEPENDENCIES

RETURN VALUE
  Does not return

SIDE EFFECTS
  sbcenc_tcb

===========================================================================*/
void audiosbcenc_task
(
  dword ignored
  /* Parameter received from Main Control task - ignored */
  /*lint -esym(715,ignored)
  ** Have lint not complain about the ignored parameter 'ignored' which is
  ** specified to make this routine match the template for rex_def_task().
  */
)
{
  rex_sigs_type   sigs;
  uint8           i = 0;

  rex_def_timer(&audiosbc_rpt_timer, &sbcenc_tcb, AUDIOSBC_RPT_TIMER_SIG);
  rex_def_timer(&audiosbc_codec_timer, &sbcenc_tcb, AUDIOSBC_CODEC_TIMER_SIG);

#ifdef FEATURE_MULTIMODE_ARCH
  task_start( AUDIOSBC_RPT_TIMER_SIG, DOG_SBCENC_RPT, &audiosbc_rpt_timer );
#else
  mc_task_start( AUDIOSBC_RPT_TIMER_SIG, DOG_SBCENC_RPT, &audiosbc_rpt_timer );
#endif /*FEATURE_MULTIMODE_ARCH*/

  /* Clear start signal
  */
  (void) rex_clr_sigs( &sbcenc_tcb, TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs( &sbcenc_tcb, AUDIOSBC_RPT_TIMER_SIG );

  (void) q_init( &audiosbc_cmd_q );          /* Initialize the command queue  */
  (void) q_init( &audiosbc_free_q );         /* Initialize the free queue     */

  for(i=0; i < AUDIOSBC_NUM_PACKETS; i++) {  /* Add items to snd_free_q       */
    q_link_type *link_item;
    link_item = q_link( &audiosbc_free_packet[i], 
                        &audiosbc_free_packet[i].hdr.link );
    q_put( &audiosbc_free_q, link_item );
  }

#ifdef FEATURE_AVS_SBC_EFS_TEST_STREAM
  audiosbc_efs_streaming_bt_handle = bt_cmd_ec_get_application_id();
  bt_cmd_ec_reg_event_set_any_app_id(audiosbc_efs_streaming_bt_handle, 
                                     audiosbc_efs_streaming_bt_ev_cb_func, 
                                    BT_EC_ES_CUSTOM, BT_EV_PF_A2DP_CON, 
                                    BT_EV_PF_A2DP_SUSPEND);

  rex_def_timer(&audiosbc_efs_stream_timer, &sbcenc_tcb, 
                                                 AUDIOSBC_EFS_STREAM_TIMER_SIG);
#endif
  for(;;)
  {
    sigs = rex_wait( AUDIOSBC_RPT_TIMER_SIG   |  /* Watchdog report timer */
                     AUDIOSBC_CMD_Q_SIG       |  /* Command Queue signal  */
                     TASK_STOP_SIG            |  /* Task stop from MC     */
                     TASK_OFFLINE_SIG         |  /* Task offline from MC  */
#ifdef FEATURE_AVS_SBC_EFS_TEST_STREAM
                     AUDIOSBC_EFS_STREAM_TIMER_SIG |
#endif
                     AUDIOSBC_CODEC_TIMER_SIG    /* Backlog reduction     */
                   );


    /* If watchdog report timer expired, report and restart the timer
    */
    if ( (sigs & AUDIOSBC_RPT_TIMER_SIG) != 0 ) {
      dog_report(DOG_SBCENC_RPT);
      (void) rex_set_timer( &audiosbc_rpt_timer, DOG_SBCENC_RPT_TIME );
    }

    if ( (sigs & TASK_STOP_SIG) != 0 ) {
      audiosbc_shutdown();
    }

    if ( (sigs & TASK_OFFLINE_SIG) != 0 ) {
      audiosbc_offline();
    }
    if( (sigs & AUDIOSBC_CODEC_TIMER_SIG) != 0) {
      (void) rex_clr_sigs( &sbcenc_tcb, AUDIOSBC_CODEC_TIMER_SIG);
      audiosbc_clear_backlog();
    }

    if ( (sigs & AUDIOSBC_CMD_Q_SIG) != 0 ) {
      (void) rex_clr_sigs( &sbcenc_tcb, AUDIOSBC_CMD_Q_SIG);
      audiosbc_req();
    }

#ifdef FEATURE_AVS_SBC_EFS_TEST_STREAM
    if( (sigs & AUDIOSBC_EFS_STREAM_TIMER_SIG) != 0) {
      (void) rex_clr_sigs( &sbcenc_tcb, AUDIOSBC_EFS_STREAM_TIMER_SIG);
      audiosbc_do_efs_streaming_test();
    }

    if (audiosbc_sbc_efs_start_streaming)
    {
      audiosbc_setup_efs_test_streaming();
      audiosbc_sbc_efs_start_streaming = 0;
    }
#endif

  }
}
#endif

#if defined (FEATURE_SBC_DSP_CODEC) && defined (FEATURE_AVS_A2DP_AVSYNC)
/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_process_avsync

DESCRIPTION
  This is the function called by SND task to send AV Sync information that 
  was received from the ADSP

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void audiosbc_process_avsync 
(
  qword timestamp,         /* Timestamp for the callback in ms            */    
  qword num_samples,       /* Sample counter. Use only lower 48 bits      */
  qword num_bytes          /* Sample counter. Use only lower 48 bits      */
)
{
  qword                num_frames;
  qword                temp_samples;
  static uint8         temp_int = 0;
  
  if (!audiosbc_context.sbc_active || !audiosbc_context.streaming_started)
  {
    snd_sbc_avsync_final(num_bytes, num_samples, timestamp);

    qw_equ(audiosbc_context.avsync_offset, num_samples);

    temp_int = 0;
    return;
  }

  if(audiosbc_avsync_log)
  {
    MSG_MED("Current samples = %d %d", 
              qw_hi(num_samples), qw_lo(num_samples), 0);
    MSG_MED("last samples = %d %d", 
              qw_hi(audiosbc_context.last_samples), 
              qw_lo(audiosbc_context.last_samples), 0);
  }

  if (qw_cmp(num_samples, audiosbc_context.last_samples)== 1)
  {
    qw_sub(temp_samples, num_samples, audiosbc_context.last_samples);
    qw_add(audiosbc_context.cumulative_samples, 
           audiosbc_context.cumulative_samples, temp_samples);
    qw_equ(audiosbc_context.last_samples, num_samples);
    if(audiosbc_avsync_log)
    {
      MSG_MED("Cumulative samples = %d %d",  
                qw_hi(audiosbc_context.cumulative_samples), 
                qw_lo(audiosbc_context.cumulative_samples), 0);
    }
  }
  else
  {
    qw_equ(audiosbc_context.last_samples, num_samples);
    snd_sbc_avsync_final(num_bytes, num_samples, timestamp);
    return;
  }

  if (!temp_int)
  {
    MSG_HIGH("AV Sync offset = %d %d", qw_hi(audiosbc_context.avsync_offset), \
               qw_lo(audiosbc_context.avsync_offset), 0);
    temp_int = 1;
  }

  qw_equ(temp_samples, audiosbc_context.cumulative_samples);

  /*check for the validity of avsync_offset
    when there is a device change during the playback...*/
  if (qw_cmp(audiosbc_context.cumulative_samples,
       audiosbc_context.avsync_offset) == -1)
  {
    //Reset the avsync offset value...
    qw_set(audiosbc_context.avsync_offset, 0,0);
  }

  qw_sub(temp_samples, temp_samples, audiosbc_context.avsync_offset);
  qw_div(num_frames, temp_samples, audiosbc_context.num_sample_lengths);

  if (qw_cmp(audiosbc_context.last_ack_frame, num_frames) >= 0)
  {
    if(audiosbc_avsync_log)
    {
      MSG_MED("LACK frame = %d curr frame = %d", 
                qw_lo(audiosbc_context.last_ack_frame), qw_lo(num_frames), 0);
    }
    /* This frame has already been sent to BT and has been acknowledged by BT
     so acknowledge to QTV right away */

    snd_sbc_avsync_final(num_bytes, num_samples, timestamp);
  }
  else if (audiosbc_avsync_tbl[audiosbc_avsync_windex].slot_state ==
           AUDIOSBC_AVSYNC_SLOT_STATE_AVAIL)
  {
    qw_equ(
      audiosbc_avsync_tbl[audiosbc_avsync_windex].num_of_bytes,num_bytes);
    qw_equ(
      audiosbc_avsync_tbl[audiosbc_avsync_windex].num_of_samples,num_samples);
    qw_equ(
      audiosbc_avsync_tbl[audiosbc_avsync_windex].timestamp , timestamp);

    qw_equ(audiosbc_context.last_frame, num_frames);
    qw_equ(audiosbc_avsync_tbl[audiosbc_avsync_windex].seq_number, num_frames);

    if (qw_lo(num_frames) < audiosbc_context.sequence_number ) 
    {
      /* This frame has been sent to BT but not yet acknowledged by BT */
      audiosbc_avsync_tbl[audiosbc_avsync_windex].slot_state = 
        AUDIOSBC_AVSYNC_SLOT_STATE_INFORMED;
    } 
    else 
    {
      /* This frame has not yet been sent to BT */
      audiosbc_avsync_tbl[audiosbc_avsync_windex].slot_state = 
        AUDIOSBC_AVSYNC_SLOT_STATE_PEND_INFORM;
    }

    MSG_MED("AV Sync info rcvd slot = %d frames = %d %d",  
             audiosbc_avsync_windex, qw_hi(num_frames), 
              qw_lo(num_frames));

    audiosbc_avsync_windex++;

    if (audiosbc_avsync_windex >= AUDIOSBC_AVSYNC_MAX_SLOT)
    {
      audiosbc_avsync_windex -= AUDIOSBC_AVSYNC_MAX_SLOT;
    }
  }
  else 
  {
    MSG_ERROR ("BT AV SYNC no slots available", 0,0,0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audiosbc_avsync

DESCRIPTION
 This is the function called by SND when BT calls with the SBC frame number
 that has been transmitted sucessfully

DEPENDENCIES

RETURN VALUE
  Does not return

SIDE EFFECTS

===========================================================================*/
void audiosbc_avsync (qword frames)
{
  uint16 i = 0;
  boolean slot_changed = FALSE;
  uint16 last_slot = audiosbc_avsync_rindex;

  MSG_MED("BT AV SYNC rcvd %d %d frames ack from BT", 
          qw_hi(frames), qw_lo(frames),0);

  qw_equ(audiosbc_context.last_ack_frame, frames);

  for (i = audiosbc_avsync_rindex; i != audiosbc_avsync_windex;)
  {
    if (qw_cmp(frames, audiosbc_avsync_tbl[i].seq_number) >= 0)
    {
      audiosbc_avsync_tbl[i].slot_state = AUDIOSBC_AVSYNC_SLOT_STATE_AVAIL;
      audiosbc_avsync_rindex++;

      if (audiosbc_avsync_rindex >= AUDIOSBC_AVSYNC_MAX_SLOT)
      {
        audiosbc_avsync_rindex -= AUDIOSBC_AVSYNC_MAX_SLOT;
      }
      slot_changed = TRUE;
      last_slot = i;
    }
    i++;
    if (i >= AUDIOSBC_AVSYNC_MAX_SLOT)
    {
      i -= AUDIOSBC_AVSYNC_MAX_SLOT;
    }
  }
  if (slot_changed)
  {
    snd_sbc_avsync_final(audiosbc_avsync_tbl[last_slot].num_of_bytes,
                         audiosbc_avsync_tbl[last_slot].num_of_samples,
                         audiosbc_avsync_tbl[last_slot].timestamp);
  }
}
#endif
#endif /* FEATURE_SBC_CODEC */
