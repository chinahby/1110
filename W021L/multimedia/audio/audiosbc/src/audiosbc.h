#ifndef AUDIOSBC_H
#define AUDIOSBC_H

/*===========================================================================
 B L U E  T O O T H  S B C  C O D E C  H E A D E R  F I L E

DESCRIPTION
  This header file contains all the definitions for the interface exposed
  by SBC module to  snd/vocoder modules 

REFERENCES

Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/codec/sbc/main/latest/src/audiosbc.h#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/07    ck     Fixed compiler warnings
11/20/06    sp     Fixed some issues in Av Sync.
07/26/06    sp     Added support for A2DP AV Sync
03/16/06   anb     Added support for sbc re-configuration given the encoded params
                   CR 78581,87334.
11/16/05    sp     Changes to support SBC encoding in lower priority task.
09/18/05    sp     Added support for DSP based SBC encoder
08/29/05    sp     Made changes so that A2DP suspend happens only in vocfunc.c
                   Added P4 header.
07/13/05    sp     Added code to honor bitpool values preferred by BT A2DP 
                   headset. Changed the default mode to Joint stereo. Fixed a
                   bug that resulted in bit rate reduced to half for stereo
                   headsets.
06/01/05    sp      Added function prototypes to fix compiler warnings and 
                    fixed an error in copyright message.
05/27/05    sp      Initial version 

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "target.h"             /* Target specific definitions            */
#if defined (FEATURE_SBC_CODEC) || defined (FEATURE_SBC_DSP_CODEC)
#include "audiosbcenc.h"
#include "snd.h"                /* Included for Post proc buffer definitions */
#include "qw.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef enum {
  AUDIOSBC_SUCCESS,
  AUDIOSBC_FAILURE
}audiosbc_status_type;

typedef enum {
  AUDIOSBC_CHANNEL_MODE_INVALID,
  AUDIOSBC_CHANNEL_MODE_MONO,
  AUDIOSBC_CHANNEL_MODE_DUAL,
  AUDIOSBC_CHANNEL_MODE_STEREO,
  AUDIOSBC_CHANNEL_MODE_JOINT_STEREO,
  AUDIOSBC_CHANNEL_MODE_MAX
}audiosbc_channel_mode_type;

typedef enum {
  AUDIOSBC_SAMPLE_RATE_INVALID,
  AUDIOSBC_SAMPLE_RATE_44100,
  AUDIOSBC_SAMPLE_RATE_48000,
  AUDIOSBC_SAMPLE_RATE_MAX
}audiosbc_sample_rate_type;

typedef struct {
  uint32 min_bitpool;
  uint32 max_bitpool;
  uint8 block_length;
  uint8 num_subbands;
  uint8 alloc;
  uint8 sample_rate;
  uint8 mode;
}audiosbc_config_type;

typedef enum {
  AUDIOSBC_BITRATE_LOWEST   = 64000,
  AUDIOSBC_BITRATE_LOW      = 96000,
  AUDIOSBC_BITRATE_MEDIUM   = 128000,
  AUDIOSBC_BITRATE_HIGH     = 192000,
  AUDIOSBC_BITRATE_MAX      = 256000
}audiosbc_bitrate_type ;


/* <EJECT> */
/*===========================================================================

                             EXTERNALIZED FUNCTIONS

===========================================================================*/

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
void audiosbc_init(snd_sbc_frame_cb_func_ptr_type sbc_cb);

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
  bt_app_id_type                  app_id,
  audiosbc_sample_rate_type       sbc_sample_rate
);

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_audio_flush

DESCRIPTION
  This function will be called by snd to let us know that the audio playback
  is done. We will reset the sequence number.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_audio_flush(boolean send_held_frame);

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_set_sample_rate

DESCRIPTION
  Sets the sample rate for the SBC codec.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_set_sample_rate(audiosbc_sample_rate_type sample_rate);

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_get_config

DESCRIPTION
  This function is called by vocoder driver to get the configuration 
  information. This information is to be passed on to the BT drivers.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_get_config(audiosbc_config_type *config);

/* <EJECT> */
/*===========================================================================

FUNCTION  audiosbc_set_bitrate

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
boolean audiosbc_set_bitrate(snd_sbc_bitrate_type bitrate_hint, uint32 bps);

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
void audiosbc_set_params(uint32 blocks, uint32 mode, uint32 alloc, uint32 bands);
/*===========================================================================

FUNCTION  audiosbc_set_enc_params

DESCRIPTION
  This function sets the block length, channel mode and allocation method values
  from the audiosbcenc_data_type.
  layers.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audiosbc_set_enc_params(audiosbcenc_data_type *sbc_enc_data_ptr);
/* <EJECT> */
/*===========================================================================*/
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
uint8 audiosbc_get_block_length(uint8 enc_block_length);
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
uint8 audiosbc_get_channel_mode(uint8 enc_channel_mod);
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
uint8 audiosbc_get_alloc_method(uint8 enc_alloc_method);
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
uint8 audiosbc_get_sub_bands(uint8 enc_sub_band);
/*===========================================================================*/

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
void audiosbc_clear_backlog(void);

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
void audiosbc_disable_encoder(void);

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
void audiosbc_pcm_cb_func (const snd_pp_buffer_type pp_buffer);

#ifdef FEATURE_AVS_SBCENC_TASK 
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
);
#endif

#endif /* FEATURE_SBC_CODEC */
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
);
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
void audiosbc_avsync 
(
  qword frames
);
#endif
#endif /* AUDIOSBC_H */
