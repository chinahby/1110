/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audio Formats Audio Decoder Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             A U D I O   F O R M A T s   A U D I O   D E C O D E R
                              S E R V I C E S

GENERAL DESCRIPTION
  These functions implement an audio decoder player.

EXTERNALIZED FUNCTIONS
  audfadec_get_data_cb_req
    This function responds accordingly to an incoming data buffer.
  audfadec_adjust
    This function adjusts the adec buffer by the indicated number of bytes.
  audfadec_parse
    This function will parse the indicated QCP file and prepare the command
    buffer with data for playback.
  audfadec_reset_channel
    This function resets the indicated QCP file playback.
  audfadec_wa_reset
    This function resets all WebAudio audio decoders.
  audfadec_wa_out_qcp
    This function will initiate the playback of a QCP file embedded in
    a WebAudio file.
  audfadec_midi_out_qcp_flag
    This function indicates whether the cuurent audio deocder parse
    is from a MIDI out command.
  audfadec_midi_out_qcp
    This function will initiate the playback of a QCP file with the QSynth
    DSP image.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2001 - 2008 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audfadec.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/08    kd     Fixed klockwork warnings.
06/31/08   vsud    Featurized the PCM binary offset code under webaudio.
06/10/08	kd     Fixed CR# 137293 add support for playback of offset binary pcm data
01/27/08  pl/sk    Change QCP bitstream stop condition.CR 125651.
07/10/07   sk      Reverted the compiler switch FEATURE_AMR_IF1   
04/20/07   anb     Added compiler switch FEATURE_AMR_IF1 for removing link error
                   in SC2x. 
04/07/07    pl     Added AMR-NB and QCP All-Rate playback support
01/18/07    ss     Added support to play fractional sample rate wave bitstream.
09/14/06    pl     Fixed choppy YADPCM playback due to incorrect buffer index.
09/13/06    sk     Fixed (adec_ptr->track_buf2 == NULL)check missing
                   in audfadec_age();
09/12/06    lg     Fixed a compilation issue in previous changes
09/08/06    lg     Fixed issues with PMD file playback
09/01/06    pl     Included fixes for deadlock prevention during repeat.
            lg     Fixed an issue with QCP within PMD ending soon.
08/15/06    pl     Fixed getting first chunk of audio data for continuous 
                   data format.
08/08/06    st     Modified code to reset adpcm_state parameter in
                   audfadec_reset(), CR98748.
06/26/06    pl     Modified various functions to adopt new ping pong buffer
                   data writing scheme. 
06/15/06    st     Modified code to use the correct FREE command to free
                   buffers, CR95886.
06/08/06    lg     Fixed featurization in audfadec_prepare_buffers
04/25/06    lg     Fixed CR#90454, changed the frame size calculation for nonGA
                   builds as per DSP requirements.
04/18/06    st     Updated audfadec_wa_reset() to support a clear data request
                   pending flag, CR90915; modified calculation of frame size
                   for ADPCM in audfadec_wa_activate().
03/27/06    lg     Fixed CR#74842, added check for last odd byte for AAC.
03/13/06    st     Added support for delayed start and repeat mode for playback
                   of audio files; modified calculation of frame size for ADPCM
                   to match DSP calculation.
01/13/06   act     Fixed compile warning with low-tier featureset.
12/06/05    lg     Fixed issue with buffer_index + buffer_length > AUDFADEC_BUF_LENGTH
11/23/05    lg     Fixed multiple issues with Animated AAC ringers: CR78752,
                   CR81313 and CR74842
10/21/05    st     Modified code to send back START status when INIT_DONE
                   is reached.
09/18/05   pl/st   Modified code to support new audio decoder API, including
                   changes to support multiple audio decoder types
                   simultaneously; modified code to handle playback using
                   variable data buffer sizes.
08/15/05    lg     Added check for NULL adec_ptr for CR# 58506
07/22/05    lg     Added support for Stereo YADPCM. 
07/25/05   ayu     Modified code to return error on bad handles when playing
                   QCP files.
07/19/05   ayu     Modified code to check for NULL pointers.
06/15/05    st     Modifed code to account for pending buffer during start
                   of PCM playback in audfadec_add_command().
05/08/05    st     Modifed code to accept frame size for MFi ADPCM playback.
03/21/05    st     Fixed featurization in audfadec_add_command().
03/10/05    st     Added support for ADPCM playback from MFi CMX extension
                   format; modified code to ensure there is at least one
                   frame's worth of YADPCM data before sending to be processed.
01/05/05    aw     Modified code to properly extract aac header info.
07/02/04    st     Modified code to avoid overflow in audfadec_decode_adpcm();
                   modified code to support various sampling rates for YADPCM
                   mode.
05/12/04    st     Modified code to add more supported sampling rates for
                   YADPCM; modified code to properly calculate energy of
                   sample in audfadec_decode_adpcm().
05/04/04    aw     Fixed YADPCM featurization.
04/22/04    lg     Modified code to support Animated AAC Ringers.
04/16/04    st     Added support to decode stereo YADPCM clips.
03/22/04    st     Modified code to reset PENDING flag in audfadec_wa_reset();
                   Modifed code to support PCM playback; modified code to select
                   closest supported sampling rate for YADPCM playback.
                   Modified code to flush and unpause ADEC channel at the end
                   of playback; added audfadec_midi_out_qcp_channel_flag() and
                   audfadec_pause_midi_out_qcp() functions.
12/02/03    sg     Modified to check if the file to be played is an ADPCM in
                   audfadec_add_command(), return failure if it is not.
10/16/03  aw/st    Modified code to support SMAF and multi-sequence.
02/19/03    st     Modified code to support new audmain_adpcm_start() function
                   prototype.
09/26/02  aw/st    Added support to playback ADPCM wave data in webaudio file;
                   Remove the ARM code to do byte swap for ADPCM data, byte
                   swapping will be done in the DSP side.
09/17/02    st     Modified code to handle short audio decoder files.
08/10/02  aw/st    Added Support for ADPCM audio decoder; added ABORT
                   callback if playback has failed.
11/11/01    st     Modified code to handle delayed audio decoder commands 
                   from WebAudio source.
10/01/01    st     Modified code to handle multiple adec channels; modified
                   audfadec_get_data_cb_req() to handle freeing unexpected
                   buffers.
09/03/01    st     Created

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "task.h"               /* Task definitions and prototypes         */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
#include "audfadec.h"           /* Audio Format definitions and prototypes */
#include "audfadeci.h"          /* Internal Audio Format definitions       */
#include "qcpsup.h"             /* QCP Format utilities definitions        */
#include "audmain.h"            /* Audio Format definitions and prototypes */
#include "audfmt.h"             /* Audio Format definitions and prototypes */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <string.h>             /* String routines                         */
#include <math.h>               /* Math routines                           */

#ifdef FEATURE_ANIMATED_AAC
#include "audaac.h"
#endif /* FEATURE_ANIMATED_AAC */

#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
#include "amrsup.h"
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data.  There is no global data.

===========================================================================*/

/* This structure contains all the information about a file
** that is being parsed. This module currently only supports parsing
** one file at a time.
*/
audfadec_parse_ctl_type  audfadec_parse_ctl;

/* This parameter is used to hold the callback function used for the last 
** data get command. This is used in the cases where playbacks have been
** stopped but not all data get commands have returned. This callback is
** then used to free the buffer.
*/
snd_af_data_cb_func_type audfadec_last_cb;

#if defined(FEATURE_WEBAUDIO) && defined(FEATURE_QSYNTH_ADPCM)
/* These definitions are used to figure out the closest sampling rate to
** use for YADPCM playback.
*/
#define AUDFADEC_YADPCM_SAMPLE_RATE_NUM 24
const uint32 audfadec_yadpcm_sr[] =
{ 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000,
  13000, 14000, 15000, 16000, 18000, 20000, 22000, 24000, 26000, 28000, 30000,
  32000
};

/* The following tables are used to decode YADPCM clips */
const sint31 audfadec_yadpcm_diff_lookup[16] = {
  1, 3, 5, 7, 9, 11, 13, 15, -1, -3, -5, -7, -9, -11, -13, -15
};

const sint31 audfadec_yadpcm_index_scale[16] = {
  0x0e6, 0x0e6, 0x0e6, 0x0e6, 0x133, 0x199, 0x200, 0x266,
  0x0e6, 0x0e6, 0x0e6, 0x0e6, 0x133, 0x199, 0x200, 0x266
};
#endif /* FEATURE_WEBAUDIO && FEATURE_QSYNTH_ADPCM */

/* This macro is used to verify whether the audio decoder mode is valid
** before starting a clip.
*/
#ifdef FEATURE_GASYNTH_INTERFACE
#define AUDFADEC_VERIFY_MODE(v) (audmain_get_adec_mode() != \
                                                   AUDMAIN_ADEC_MODE_AAC)
#else
#define AUDFADEC_VERIFY_MODE(v) (audmain_get_adec_mode() == v)
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_reset

DESCRIPTION
  This function resets the parameters in an audfadec_track_type structure
  to initial values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfadec_reset (
  uint32              channel,
  audfadec_track_type *adec_ptr
) {
  boolean enabled;
 
 MSG_MED("reset channel %d", channel, 0, 0);

#ifdef FEATURE_WEBAUDIO
  if((adec_ptr->status_flag & AUDFADEC_MASK_WA_PEND) == AUDFADEC_MASK_WA_PEND) {
    adec_ptr->wa_reserve.handle          = adec_ptr->handle;
    adec_ptr->wa_reserve.block_start     = adec_ptr->block_start;
    adec_ptr->wa_reserve.next_block      = adec_ptr->next_block;
    adec_ptr->wa_reserve.status_flag     = adec_ptr->status_flag;
    adec_ptr->wa_reserve.length_not_read = adec_ptr->length_not_read +
                                                      adec_ptr->buffer_orig;
    adec_ptr->wa_reserve.file_location   = adec_ptr->file_location - 
                                                      adec_ptr->buffer_orig;

    adec_ptr->wa_reserve.status_flag    &= ~(AUDFADEC_MASK_BUF_PENDING |
                                             AUDFADEC_MASK_SKIP_BUF);

    adec_ptr->wa_reserve.sample_rate_hz  = adec_ptr->sample_rate_hz;
    adec_ptr->wa_reserve.bits_per_sample = adec_ptr->bits_per_sample;
    adec_ptr->wa_reserve.channels        = adec_ptr->channels;
    adec_ptr->wa_reserve.frame_size      = adec_ptr->frame_size;
    adec_ptr->wa_reserve.yadpcm_mode     = adec_ptr->yadpcm_mode;
  }
#endif /* FEATURE_WEBAUDIO */

  enabled = (adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED);

  if(enabled) {
#ifdef FEATURE_GASYNTH_INTERFACE
    audmain_adpcm_stop(channel);
#else
    audmain_audfmt_adec_chan_flush(channel);
#ifdef FEATURE_QSYNTH_ADPCM
    if((adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) ||
       (adec_ptr->status_flag & AUDFADEC_MASK_PCM)) {
      audmain_adpcm_stop(channel);
    }
#endif /* FEATURE_QSYNTH_ADPCM */
#endif /* FEATURE_GASYNTH_INTERFACE */
  }

  if(adec_ptr->handle != NULL) {
    if(adec_ptr->track_buf != NULL) {
      if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
        audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           (void *) adec_ptr->track_buf);
      } else {
        audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                           (void *) adec_ptr->track_buf);
      }
    }
    if(adec_ptr->track_buf2 != NULL) {
      if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
        audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           (void *) adec_ptr->track_buf2);
      } else {
        audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                           (void *) adec_ptr->track_buf2);
      }
    }
  }

  if(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING) {
    if(adec_ptr->status_flag & AUDFADEC_MASK_SKIP_BUF) {
      MSG_ERROR("Already skipping buffer: %d",adec_ptr->skip_position,0,0);
    } 
    adec_ptr->status_flag   = AUDFADEC_MASK_SKIP_BUF;
    adec_ptr->skip_position = adec_ptr->file_location;
  } else if(adec_ptr->status_flag & AUDFADEC_MASK_SKIP_BUF) {
    adec_ptr->status_flag = AUDFADEC_MASK_SKIP_BUF ;
  } else {
    adec_ptr->status_flag = 0;
  }
#ifndef FEATURE_GASYNTH_INTERFACE
  adec_ptr->status_flag |= AUDFADEC_MASK_DSP_RESET;
#endif /* !FEATURE_GASYNTH_INTERFACE */

#ifdef FEATURE_MIDI_OUT_QCP
  if(enabled) {
    audmain_do_play_cb(adec_ptr->handle, SND_CMX_MIDI_OUT_QCP_RESET,
                       0, NULL);
  }
#endif /* FEATURE_MIDI_OUT_QCP */

  adec_ptr->handle          = NULL;
  adec_ptr->next_block      = 0;
  adec_ptr->block_start     = 0;
  adec_ptr->file_location   = 0;
  adec_ptr->length_not_read = 0;
  adec_ptr->buffer_pad      = 0;
  adec_ptr->buffer_orig     = 0;
  adec_ptr->buffer_length   = 0;
  adec_ptr->buffer_index    = AUDFADEC_INIT_OFFSET;
  adec_ptr->track_buf       = NULL;
  adec_ptr->track_index     = 0;
  adec_ptr->track_buf_size  = 0;
  adec_ptr->track_buf2      = NULL;
  adec_ptr->track_index2    = 0;
  adec_ptr->track_buf_size2 = 0;
  adec_ptr->frame_size      = 0;
  adec_ptr->intbuff_length  = 0;
#ifdef FEATURE_QSYNTH_ADPCM
  adec_ptr->adpcm_state     = AUDFADEC_ADPCM_STATE_NONE;
#endif /* FEATURE_QSYNTH_ADPCM */
#if defined(FEATURE_WEBAUDIO) && defined(FEATURE_QSYNTH_ADPCM)
  adec_ptr->sample_rate     = AUDFADEC_ADPCM_SAMPLE_RATE_UNKNOWN;
#endif /* FEATURE_WEBAUDIO && FEATURE_QSYNTH_ADPCM */
  adec_ptr->repeat_num      = 0;

#ifndef FEATURE_GASYNTH_INTERFACE
  if(enabled) {
    audmain_audfmt_adec_chan_pause(channel, FALSE);
  }
#endif /* FEATURE_GASYNTH_INTERFACE */
#ifdef FEATURE_ANIMATED_AAC
  adec_ptr->aac_sample_rate = SND_SAMPLE_RATE_48000;
  memset( (void*)&adec_ptr->aac_config, 0, sizeof(audmain_song_config_type));
#endif
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_reset_check_status

DESCRIPTION
  This function resets the audio decoder channel and sends status back to
  AUDFMT module, if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfadec_reset_check_status (
  uint32              channel,
  audfadec_track_type *adec_ptr
) {
#ifdef FEATURE_WEBAUDIO
  if((adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC) &&
     !(adec_ptr->wa_reserve.status_flag & AUDFADEC_MASK_WA_PEND)) {
    audfmt_adec_channel_done(channel);
  }
#endif /* FEATURE_WEBAUDIO */
  audfadec_reset(channel, adec_ptr);
}

/* <EJECT> */
#ifdef FEATURE_ANIMATED_AAC
/*===========================================================================

FUNCTION audfadec_get_aac_config

DESCRIPTION
  This function extracts the ADTS header from the start of AAC frame.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL audmain_status_type audfadec_get_aac_config(
  audmain_parse_ctl_type      *parse_ctl,
  audmpeg_song_config_type    *aac_config,
  snd_sample_rate_type        *sample_rate,
  byte                        *data,
  uint32                      length
)
{
  if( length < 6){
    return AUDMAIN_STATUS_PENDING;
  }

  if( audaac_get_animaac_config(parse_ctl , aac_config, sample_rate, data)){
     return AUDMAIN_STATUS_SUCCESS;
  } else{
    return AUDMAIN_STATUS_FAILURE;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_set_song_config

DESCRIPTION
  Sets the new song config & sampling rate on the DSP & Codec clk.

DEPENDENCIES
  audfadec_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_set_song_config(
  audfadec_track_type *adec_ptr
){
  
   return (audmain_set_adec_song_config( (void*)audfadec_parse_ctl.main_parse,
                                        &adec_ptr->aac_config, 
                                        adec_ptr->aac_sample_rate));

}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_get_song_config

DESCRIPTION
  Gets the song config .

DEPENDENCIES
  audfadec_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_get_song_config(
  audmain_parse_ctl_type   *parse_ctl,
  audmpeg_song_config_type *song_config
){
   audfadec_parse_ctl_type  *adec_ptr;

   if((parse_ctl != NULL) && (parse_ctl->audfadec_tracks != NULL)) {
     adec_ptr = (audfadec_parse_ctl_type*)parse_ctl->audfadec_tracks;
     if( (adec_ptr != NULL) &&
         (adec_ptr->tracks[0].status_flag & AUDFADEC_MASK_AAC)
       ){
       memcpy( (void*)song_config, 
               (void*)&adec_ptr->tracks[0].aac_config.header_config,
               sizeof( audmpeg_song_config_type));
     }else{
       memset( (void*)song_config, 0, sizeof(audmpeg_song_config_type));
     }
     return(AUDMAIN_STATUS_SUCCESS);
   }
  return(AUDMAIN_STATUS_DONE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_get_sample_rate

DESCRIPTION
  Gets the sample_rate.

DEPENDENCIES
  audfadec_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audfadec_get_sample_rate(
  audmain_parse_ctl_type   *parse_ctl
){
   audfadec_parse_ctl_type  *adec_ptr;

   if((parse_ctl != NULL) && (parse_ctl->audfadec_tracks != NULL)) {
     adec_ptr = (audfadec_parse_ctl_type*)parse_ctl->audfadec_tracks;
     if( (adec_ptr != NULL) &&
         (adec_ptr->tracks[0].status_flag & AUDFADEC_MASK_AAC)
       ){
       return adec_ptr->tracks[0].aac_sample_rate;
     }
   }
   return( SND_SAMPLE_RATE_48000);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_copy_aac

DESCRIPTION
  This function copies AAC data into a buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_copy_aac (
  audfadec_track_type *adec_ptr,
  uint8               *src,
  uint32              *length
) {
  uint32 copy_length, buf_length, index, tlong, index2;

  index        = adec_ptr->buffer_index + adec_ptr->buffer_length;
  buf_length   = AUDFADEC_BUF_LENGTH - index;

  if(index >= AUDFADEC_BUF_LENGTH) {
    return(AUDMAIN_STATUS_FAILURE);
  } 

  index2 = index;
  copy_length = *length;

  if( copy_length == 0){
    return(AUDMAIN_STATUS_SUCCESS);
  }

  tlong = MIN(copy_length, buf_length);
  
  memcpy( &(adec_ptr->buffer[index]), src, tlong );
  index                    += tlong;
  copy_length              -= tlong;
  buf_length               -= tlong;

  adec_ptr->buffer_length += (index - index2);
  index                    = 0;
  
  *length -= tlong;
  return(AUDMAIN_STATUS_SUCCESS);
}

#endif /* FEATURE_ANIMATED_AAC */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION audfadec_decode_adpcm

DESCRIPTION
  This function decodes stereo YADPCM and mixes resulting PCM into one 8-bit
  channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfadec_decode_adpcm (
  audfadec_track_type *adec_ptr,
  uint8               *dest,
  uint8               *src,
  uint32              length
) {
#ifndef FEATURE_STEREO_YADPCM
  sint31 tlong;
  uint8  value, value_nibble;

  value = *src++;
  while(length--) {
    value_nibble = value & 0xf;

    adec_ptr->signal_left += (adec_ptr->step_left *
                              audfadec_yadpcm_diff_lookup[value_nibble]) / 8;
    if(adec_ptr->signal_left < -32768) {
      adec_ptr->signal_left = -32768;
    } else if(adec_ptr->signal_left > 32767) {
      adec_ptr->signal_left = 32767;
    }
    adec_ptr->step_left = (adec_ptr->step_left *
                           audfadec_yadpcm_index_scale[value_nibble]) >> 8;
    if(adec_ptr->step_left < 0x7f) {
      adec_ptr->step_left = 0x7f;
    } else if(adec_ptr->step_left > 0x6000) {
      adec_ptr->step_left = 0x6000;
    }

    value_nibble = (value >> 4) & 0xf;

    adec_ptr->signal_right += (adec_ptr->step_right *
                               audfadec_yadpcm_diff_lookup[value_nibble]) / 8;
    if(adec_ptr->signal_right < -32768) {
      adec_ptr->signal_right = -32768;
    } else if(adec_ptr->signal_right > 32767) {
      adec_ptr->signal_right = 32767;
    }
    adec_ptr->step_right = (adec_ptr->step_right *
                            audfadec_yadpcm_index_scale[value_nibble]) >> 8;
    if(adec_ptr->step_right < 0x7f) {
      adec_ptr->step_right = 0x7f;
    } else if(adec_ptr->step_right > 0x6000) {
      adec_ptr->step_right = 0x6000;
    }

    tlong   = (adec_ptr->signal_left + adec_ptr->signal_right) >> 1;
    tlong  *= 46340;
    tlong >>= 15;
    if(tlong > 0x7fff) {
      tlong = 0x7fff;
    } else if(tlong < ((sint31) 0xFFFF8000)) {
      tlong = ((sint31) 0xFFFF8000);
    }

    *dest++ = (tlong >> 8) & 0xff;
    value   = *src++;
  }
#endif /* !FEATURE_STEREO_YADPCM */
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_copy_adpcm

DESCRIPTION
  This function copies ADPCM data into a buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_copy_adpcm (
  audfadec_track_type *adec_ptr,
  uint8               *src,
  uint32              *length
) {
  uint32 copy_length;
  boolean wrote = FALSE;
  uint8  *buffer;

  if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER1) && 
      !(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_LOCKED1)  
     ) {
    copy_length  = MIN(AUDFADEC_INT_BUF_LENGTH -
                       AUDFADEC_ADPCM_FRAME_LENGTH_MAX -
                       adec_ptr->buffer_length, *length );

     if( (copy_length != 0) &&
        !((copy_length == 1) && (adec_ptr->buffer_length == 0)) && 
         (((adec_ptr->buffer_length + copy_length)&0x1) == 0x1) ){
       copy_length--;
     }
    if(copy_length != 0) {
      buffer = &(adec_ptr->buffer[adec_ptr->buffer_length +
                                  AUDFADEC_ADPCM_FRAME_LENGTH_MAX]);

#ifdef FEATURE_WEBAUDIO
      if(adec_ptr->status_flag & AUDFADEC_MASK_YADPCM_DEC) {
        audfadec_decode_adpcm(adec_ptr, buffer, src, copy_length);
      } else
#endif /* FEATURE_WEBAUDIO */
      {
        memcpy(buffer, src, copy_length);
        wrote = TRUE;
      }

      adec_ptr->buffer_length += copy_length;
      *length                 -= copy_length;
      src                     += copy_length;
    }
  }

  if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER2)) {
    copy_length  = MIN(AUDFADEC_INT_BUF_LENGTH -
                       AUDFADEC_ADPCM_FRAME_LENGTH_MAX - 
                       adec_ptr->buffer_index, *length);
    if( (copy_length != 0) && 
        !((copy_length == 1) && (adec_ptr->buffer_index == 0)) && 
        (((adec_ptr->buffer_index + copy_length)&0x1) == 0x1) ){
      copy_length--;
    }
 
    if(copy_length != 0) {
      buffer = &(adec_ptr->buffer[adec_ptr->buffer_index +
                                  AUDFADEC_ADPCM_FRAME_LENGTH_MAX +
                                  AUDFADEC_INT_BUF_LENGTH]);

      if( wrote == TRUE){
       adec_ptr->adpcm_state |= AUDFADEC_ADPCM_STATE_LOCKED1;
      }
#ifdef FEATURE_WEBAUDIO
      if(adec_ptr->status_flag & AUDFADEC_MASK_YADPCM_DEC) {
        audfadec_decode_adpcm(adec_ptr, buffer, src, copy_length);
      } else
#endif /* FEATURE_WEBAUDIO */
      {
        memcpy(buffer, src, copy_length);
      }

      adec_ptr->buffer_index += copy_length;
      *length                -= copy_length;
    }
  }

  return(AUDMAIN_STATUS_SUCCESS);
}
#endif /* FEATURE_QSYNTH_ADPCM */
/* <EJECT> */
#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
/*===========================================================================

FUNCTION audfadec_copy_midi_out_qcp

DESCRIPTION
  This function copies All-Rate qcp data into a buffer for MIDI_OUT playback.

DEPENDENCIES
  None

RETURN VALUE
  Return AUDMAIN_STATUS_ERROR upon encountering corrupted
  frame header. 
  Return AUDMAIN_STATUS_BUFF_ERR if the source buffer has less than one 
  complete frame left.
  Otherwise, return AUDMAIN_STATUS_SUCCESS

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_copy_midi_out_qcp (
  audfadec_track_type       *adec_ptr,
  uint8                     *src,
  uint32                    *length
) {
  uint32 index, buf_length, frame_size = 0, copy_length;
  uint8  *buffer;
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER1)) {
    index = adec_ptr->buffer_length;
    buf_length = AUDFADEC_INT_BUF_LENGTH - index;
    buffer = &(adec_ptr->buffer[index]);
    index = 0;

    MSG_MED("audfadec_copy_midi_out_qcp: Populate intbuf1", 0, 0, 0);

    while((buf_length >= (AUDFADEC_FRAME_LENGTH + 2)) && (*length != 0)) {
      if (qcpsup_validate_frame(QCPSUP_FORMAT_13K,
                                *src, &frame_size) == TRUE) {
        if (frame_size <= (*length - 1)) {
          /* Pad header */
          buffer[index++] = 0;
          buf_length--;
          adec_ptr->buffer_length++;
          copy_length = frame_size + 1;
          /* Copy frame */
          memcpy(&buffer[index], src, copy_length);
          src                     += copy_length;
          *length                 -= copy_length;
          /* Pad end of frame */
          if ((frame_size & 0x01) != 0) {
            buffer[index+copy_length] = 0;
            copy_length++;
          }
          index                   += copy_length;
          adec_ptr->buffer_length += copy_length;
          buf_length              -= copy_length;
          
        } else {
          ret_val = AUDMAIN_STATUS_BUFF_ERR;
          break; /* Only Transcode when there is a complete 
                    frame */
        }
      } else {
        MSG_ERROR("Invalid QCP frame header",0,0,0);
        return(AUDMAIN_STATUS_ERROR);
      }
    }
  }

  
  if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER2)) {

    index = adec_ptr->buffer_index;
    buf_length = AUDFADEC_INT_BUF_LENGTH - index;
    buffer = &(adec_ptr->buffer[AUDFADEC_INT_BUF_LENGTH + index]); 
    index = 0;

    MSG_MED("audfadec_copy_midi_out_qcp: Populate intbuf2", 0, 0, 0);

    while((buf_length >= (AUDFADEC_FRAME_LENGTH + 2)) && (*length != 0)) {
      if (qcpsup_validate_frame(QCPSUP_FORMAT_13K,
                                *src, &frame_size) == TRUE) {
        if (frame_size <= (*length - 1)) {
          /* Pad header */
          buffer[index++] = 0;
          buf_length--;
          adec_ptr->buffer_index++;
          copy_length = frame_size + 1;
          /* Copy QCP frame */
          memcpy(&buffer[index], src, copy_length);
          src                     += copy_length;
          *length                 -= copy_length;
          /* Pad end of frame */
          if ((frame_size & 0x01) != 0) {
            buffer[index+copy_length] = 0;
            copy_length++;
          }
          index                   += copy_length;
          adec_ptr->buffer_index  += copy_length;
          buf_length              -= copy_length;

        } else {
          ret_val = AUDMAIN_STATUS_BUFF_ERR;
          break; /* Only Transcode when there is a complete 
                    frame */
        }
      } else {
        MSG_ERROR("Invalid QCP frame header",0,0,0);
        return(AUDMAIN_STATUS_ERROR);
      }
    }
  }

  if (*length == 0) {
    ret_val = AUDMAIN_STATUS_BUFF_ERR;
  }

  return(ret_val);
}

/*===========================================================================

FUNCTION audfadec_transcode_amr_frame

DESCRIPTION
  This function transcodes one native amr-nb frame to Qsynth AMR-NB frame 
  format.

DEPENDENCIES
  Caller needs to make sure destination buffer has at least one full 12.2 kbit/s 
  frame worth of empty space left to hold the transcoded AMR frame. 
  First byte of source buffer must be the frame header containg the frame type and 
  quality bit information.

RETURN VALUE
  Returns unsigned 32-bit integer to indicate the length of transcoded AMR
  frame

SIDE EFFECTS
  None

===========================================================================*/
uint32 audfadec_transcode_amr_frame
(
  uint8        *dst,
  uint8        *src
) {
  amrsup_frame_type          amr_ftype;
  amrsup_mode_type           amr_rate;
  amrsup_if1_frame_info_type amr_frame_info;
  uint32                     trans_length = 0;

  if ((dst != NULL) && (src != NULL)) {
    /* Extract AMR frame header information */
    amr_frame_info.frame_type_index = ((*src & AUDFADEC_AMR_FRAME_TYPE_MASK) >>
                                               AUDFADEC_AMR_FRAME_TYPE_SHIFT);
    /* Set quality flag */
    if((*src & AUDFADEC_AMR_FRAME_QUALITY_MASK) == 
                                       AUDFADEC_AMR_FRAME_QUALITY_MASK) {
      amr_frame_info.fqi = TRUE;
    } else {
      amr_frame_info.fqi = FALSE;
    }

    amr_frame_info.amr_type = AMRSUP_CODEC_AMR_NB;

    trans_length = amrsup_if1_de_framing((dst+2),
                                         &amr_ftype,
                                         &amr_rate,
                                         (src+1),
                                         &amr_frame_info);

    *(dst++) = amr_ftype;
    *(dst++) = amr_rate;
    trans_length += 2;
    if ((trans_length & 0x1) != 0) {
      /* Pad the end of frame */
      dst[trans_length++] = 0;
    }
    MSG_MED("transcode_amr: amr_ftype=%d amr_rate=%d trans_length=%d", 
            amr_ftype, amr_rate, trans_length);
  } else {
    MSG_ERROR("Both dest buf and src buf cannot be NULL", 0, 0, 0);
  }

  return(trans_length);
}

/*===========================================================================

FUNCTION audfadec_copy_midi_out_amr

DESCRIPTION
  This function copies amr data into a buffer for MIDI_OUT playback.

DEPENDENCIES
  None

RETURN VALUE
  Return AUDMAIN_STATUS_ERROR upon encountering corrupted
  frame header. 
  Return AUDMAIN_STATUS_BUFF_ERR if the source buffer has less than one 
  complete frame left.
  Otherwise, return AUDMAIN_STATUS_SUCCESS

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_copy_midi_out_amr (
  audfadec_track_type       *adec_ptr,
  uint8                     *src,
  uint32                    *length
) {
  uint32 index, buf_length, frame_size = 0, copy_length;
  uint8  *buffer;
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER1)) {
    index = adec_ptr->buffer_length;
    buf_length = AUDFADEC_INT_BUF_LENGTH - index;
    buffer = &(adec_ptr->buffer[index]);
    index = 0;
    MSG_MED("Populate intbuf1", 0, 0, 0);
    while((buf_length >= (AUDFADEC_AMR_MAX_FRAME_LENGTH + 3)) && (*length != 0)) {
      if (qcpsup_validate_frame(QCPSUP_AMR_FILE_FORMAT,
                                *src, &frame_size) == TRUE) {
        if (frame_size <= (*length - 1)) {
          copy_length = audfadec_transcode_amr_frame(&buffer[index],
                                                     src);
          src                     += (frame_size+1);
          *length                 -= (frame_size+1);
          index                   += copy_length;
          adec_ptr->buffer_length += copy_length;
          buf_length              -= copy_length;
        } else {
          ret_val = AUDMAIN_STATUS_BUFF_ERR;
          break; /* Only Transcode when there is a complete 
                    frame */
        }
      } else {
        MSG_ERROR("Invalid amr frame header",0,0,0);
        return(AUDMAIN_STATUS_FAILURE);
      }
    }
  }

  
  if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER2)) {

    index = adec_ptr->buffer_index;
    buf_length = AUDFADEC_INT_BUF_LENGTH - index;
    buffer = &(adec_ptr->buffer[AUDFADEC_INT_BUF_LENGTH + index]); 
    index = 0;
    MSG_MED("Populate intbuf2", 0, 0, 0);
    while((buf_length >= (AUDFADEC_AMR_MAX_FRAME_LENGTH + 3)) && (*length != 0)) {
      if (qcpsup_validate_frame(QCPSUP_AMR_FILE_FORMAT,
                                *src, &frame_size) == TRUE) {
        if (frame_size <= (*length - 1)) {
          copy_length = audfadec_transcode_amr_frame(&buffer[index],
                                                   src);
          src                     += (frame_size+1);
          *length                 -= (frame_size+1);
          index                   += copy_length;
          adec_ptr->buffer_index += copy_length;
          buf_length              -= copy_length;
        } else {
          ret_val = AUDMAIN_STATUS_BUFF_ERR;
          break; /* Only Transcode when there is a complete 
                    frame */
        }        
      } else {
        MSG_ERROR("Invalid amr frame header",0,0,0);
        return(AUDMAIN_STATUS_FAILURE);
      }
    }
  }

  if (*length == 0) {
    ret_val = AUDMAIN_STATUS_BUFF_ERR;
  }

  return(ret_val);
}
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_copy_qcp

DESCRIPTION
  This function copies PMD qcp data into a buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_copy_qcp (
  audfadec_track_type *adec_ptr,
  uint8               *src,
  uint32              *length
) {
  uint32 index, buf_length, frame_size, copy_length;
  boolean wrote = FALSE;
  uint8  *buffer;

   if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER1) && 
      !(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_LOCKED1)  
     ){
    index = adec_ptr->buffer_length + AUDFADEC_ADPCM_FRAME_LENGTH_MAX;
    buf_length = AUDFADEC_INT_BUF_LENGTH - index;
    buffer = &(adec_ptr->buffer[index]);
    index = 0;

    while( ((buf_length >= AUDFADEC_FRAME_LENGTH+2) || (buf_length >= *length)) 
           && ( *length != 0)
         ) {

        if( adec_ptr->buffer_pad != 0){
          if( buf_length < adec_ptr->buffer_pad){
            break; /* Continue with buffer2 */
          }
          copy_length = adec_ptr->buffer_pad;
          adec_ptr->buffer_pad = 0;
        }else if( qcpsup_validate_frame(QCPSUP_FORMAT_13K, *src, &frame_size) &&
                 (frame_size == AUDFADEC_FRAME_LENGTH)) {

          copy_length = MIN( AUDFADEC_FRAME_LENGTH+1, *length);
          adec_ptr->buffer_pad = AUDFADEC_FRAME_LENGTH + 1 - copy_length;

          /* Pad with 0x00 before each frame */
          buffer[index++] = 0;
          buf_length--;
          adec_ptr->buffer_length ++;

        } else {
          MSG_ERROR("Invalid frame boundary.",0,0,0);
          return(AUDMAIN_STATUS_FAILURE);
        }

      memcpy( &buffer[index], src, copy_length );
      index       += copy_length;
      src         += copy_length;
      buf_length  -= copy_length;
      *length     -= copy_length;
      adec_ptr->buffer_length += copy_length;
      wrote       = TRUE;
      MSG_MED( "Copied %ld bytes to buf1", copy_length, 0, 0);
    }
  }

  
  if (!(adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER2)) {

    index = adec_ptr->buffer_index + AUDFADEC_ADPCM_FRAME_LENGTH_MAX;
    buf_length = AUDFADEC_INT_BUF_LENGTH - index;
    buffer = &(adec_ptr->buffer[AUDFADEC_INT_BUF_LENGTH + index]); 
    index = 0;

    while( ((buf_length >= AUDFADEC_FRAME_LENGTH+2) || (buf_length >= *length)) 
           && ( *length != 0)
         ) {

        if( adec_ptr->buffer_pad != 0){
          if( buf_length < adec_ptr->buffer_pad){
            break; /* Continue with buffer2 */
          }
          copy_length = adec_ptr->buffer_pad;
          adec_ptr->buffer_pad = 0;
        }else if( qcpsup_validate_frame(QCPSUP_FORMAT_13K, *src, &frame_size) &&
                 (frame_size == AUDFADEC_FRAME_LENGTH)) {

          copy_length = MIN( AUDFADEC_FRAME_LENGTH+1, *length);
          adec_ptr->buffer_pad = AUDFADEC_FRAME_LENGTH + 1 - copy_length;

          /* Pad with 0x00 before each frame */
          buffer[index++] = 0;
          buf_length--;
          adec_ptr->buffer_index ++;

        } else {
          MSG_ERROR("Invalid frame boundary.",0,0,0);
          return(AUDMAIN_STATUS_FAILURE);
        }

        if( wrote == TRUE){
          adec_ptr->adpcm_state |= AUDFADEC_ADPCM_STATE_LOCKED1;
          wrote = FALSE;
        }

        memcpy( &buffer[index], src, copy_length );
        index       += copy_length;
        src         += copy_length;
        buf_length  -= copy_length;
        *length     -= copy_length;
        adec_ptr->buffer_index += copy_length;
        MSG_MED( "Copied %ld bytes to buf2", copy_length, 0, 0);
      }
  }

  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_copy

DESCRIPTION
  This function copies one buffer of QCP data to a circular buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_copy (
  audfadec_track_type *adec_ptr,
  uint8               *src,
  uint32              *length
) {
  uint32  index, index2, frame_size = 0, buf_length, tlong;
  uint32  copy_length = 0, copy_length2, counter;
  boolean do_copy = FALSE;

  if (!(adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC)) {
    if ((adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE)) {
#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
      if (adec_ptr->status_flag & AUDFADEC_MASK_QCP) {
        return(audfadec_copy_midi_out_qcp(adec_ptr, src,length));
      } else if (adec_ptr->status_flag & AUDFADEC_MASK_AMR) {
        return(audfadec_copy_midi_out_amr(adec_ptr,src,length));
      } else 
#endif /*  FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */
      if (adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
        return(AUDMAIN_STATUS_SUCCESS);
      }
    } else {
      return (AUDMAIN_STATUS_SUCCESS);
    }
  }

#ifdef FEATURE_QSYNTH_ADPCM
  if ( ((adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) != 0) &&
       ((adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) != 0) 
     ) { 
    return (audfadec_copy_adpcm(adec_ptr, src, length));
  }
#endif /* FEATURE_QSYNTH_ADPCM */

   if ( ((adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT) != 0) &&
       (audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_QCP) ){
     if ((adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) != 0) { 
       return (audfadec_copy_qcp( adec_ptr, src, length));
     } else{
       return (AUDMAIN_STATUS_SUCCESS);
     }
   } 


#ifdef FEATURE_ANIMATED_AAC
  if (((adec_ptr->status_flag & AUDFADEC_MASK_AAC) == AUDFADEC_MASK_AAC) &&
      (adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE)) {
    return (audfadec_copy_aac(adec_ptr, src, length)); 
  }
#endif /* FEATURE_ANIMATED_AAC */

  tlong = AUDFADEC_BUF_LENGTH - adec_ptr->buffer_length;
  if(tlong > *length) {
    copy_length = *length;
    do_copy     = TRUE;
  } else if(tlong > 200) {
    copy_length = tlong;
    do_copy     = TRUE;
  }

  if(do_copy) {
    copy_length2 = copy_length;
    index        = adec_ptr->buffer_index + adec_ptr->buffer_length;
    buf_length   = AUDFADEC_BUF_LENGTH - index;

    if(!(adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE)) {
      if(index < AUDFADEC_BUF_LENGTH) {
        buf_length = MIN(AUDFADEC_BUF_LENGTH - index, copy_length);
        /*lint -e669 Tell Lint we know that this will not overrun buffers. */
        memcpy( &(adec_ptr->buffer[index]), src, buf_length );
        copy_length             -= buf_length;
        adec_ptr->buffer_length += buf_length;
      }
    } else {
      for(counter = 2; counter != 0; counter--) {
        if(index < AUDFADEC_BUF_LENGTH) {
          index2 = index;

          tlong = MIN(adec_ptr->buffer_pad, copy_length);
          tlong = MIN(tlong, buf_length);
          memcpy( &(adec_ptr->buffer[index]), src, tlong );
          index                += tlong;
          src                  += tlong;
          copy_length          -= tlong;
          buf_length           -= tlong;
          adec_ptr->buffer_pad -= tlong;

          tlong = 0;
          while((buf_length != 0) && (copy_length != 0)) {
            adec_ptr->buffer[index++] = 0;
            buf_length--;
            if(qcpsup_validate_frame(QCPSUP_FORMAT_13K, *src, &frame_size) &&
               (frame_size == AUDFADEC_FRAME_LENGTH)) {
              tlong = MIN(frame_size + 1, copy_length);
              tlong = MIN(tlong, buf_length);
              memcpy( &(adec_ptr->buffer[index]), src, tlong );
              index       += tlong;
              src         += tlong;
              copy_length -= tlong;
              buf_length  -= tlong;
            } else {
              MSG_ERROR("Invalid frame boundary.",0,0,0);
              return(AUDMAIN_STATUS_ERROR);
            }
          }

          if(tlong != 0) {
            adec_ptr->buffer_pad = (frame_size + 1) - tlong;
          }

          adec_ptr->buffer_length += (index - index2);
          index                    = 0;
        } else {
          index -= AUDFADEC_BUF_LENGTH;
        }

        buf_length = AUDFADEC_BUF_LENGTH - adec_ptr->buffer_length;
      }
    }

    *length -= (copy_length2 - copy_length);

    return(AUDMAIN_STATUS_SUCCESS);
  }

  return(AUDMAIN_STATUS_BUFF_ERR);
}

/* <EJECT> */
#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
/*===========================================================================

FUNCTION audfadec_age_midi_out_vocoder

DESCRIPTION
  This function reads in the next buffer of vocoder-type decoder data, 
  if necessary.

DEPENDENCIES
  None

RETURN VALUE
  Return AUDMAIN_STATUS_DONE to indicate the playback is done.
  Return AUDMAIN_STATUS_FAILURE to indicate error encounter during
  parse the the content from client buffer.
  Otherwise, return AUDMAIN_STATUS_SUCCESS.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_age_midi_out_vocoder (
  audfadec_track_type *adec_ptr
) {
  audmain_status_type status = AUDMAIN_STATUS_FAILURE;
  uint32              org_length = 0;

  if((adec_ptr != NULL) &&
     (adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED)) {
    /* Check for stop condition first 
       Since we guarantee that internal buffers would always contain complete 
       frames, there would be no partial frame given to DSP. 
       We should always get all internal buffers back */
    if ((adec_ptr->length_not_read == 0) &&
        (adec_ptr->buffer_length == 0) &&
        (adec_ptr->buffer_index == 0)) {
      MSG_MED("MIDI_OUT VOC: Done playback", 0, 0, 0);
      return (AUDMAIN_STATUS_DONE);
    }

    /* Arrange the buffers */
    org_length = adec_ptr->track_buf_size;

    status = audfadec_copy(adec_ptr,
                           &adec_ptr->track_buf[
                           adec_ptr->track_index], 
                           &adec_ptr->track_buf_size);
    MSG_MED("Copied %d bytes into internal buff", 
            (org_length - adec_ptr->track_buf_size), 0, 0);
    adec_ptr->track_index += (org_length - adec_ptr->track_buf_size);

    /* Data request if not enough data left */
    if (status == AUDMAIN_STATUS_BUFF_ERR) {
      if (!(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING)) {
        if (adec_ptr->length_not_read != 0) {
          audmain_get_var_adec_data(adec_ptr->handle, 
                                    adec_ptr->file_location,
                                    500,
                                    adec_ptr->track_buf_size,
                                    &adec_ptr->track_buf2);
          adec_ptr->status_flag |= (AUDFADEC_MASK_BUF_PENDING |
                                    AUDFADEC_MASK_COPY_PEND);
          MSG_MED("MIDI_OUT VOC:Request more data with track_buf2", 0, 0, 0);
        } 
      }
      status = AUDMAIN_STATUS_SUCCESS;
    }
  }

  return(status);
}
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */
#ifdef FEATURE_QSYNTH_ADPCM
/*===========================================================================

FUNCTION audfadec_age_cont

DESCRIPTION
  This function reads in the next buffer of audio decoder data, if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GASYNTH_INTERFACE
audmain_status_type audfadec_age_cont (
  audfadec_track_type *adec_ptr
) {
  if((adec_ptr != NULL) &&
     (adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) &&
     (adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA)) {

    if((adec_ptr->track_buf != NULL) &&
       (adec_ptr->track_buf_size == 0)) {
      audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                         (void *) adec_ptr->track_buf);
      adec_ptr->track_buf = NULL;
      adec_ptr->adpcm_state &= ~AUDFADEC_ADPCM_STATE_BUFFER1;
    }
    if((adec_ptr->track_buf2 != NULL) &&
       (adec_ptr->track_buf_size2 == 0)) {
      audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                         (void *) adec_ptr->track_buf2);
      adec_ptr->track_buf2 = NULL;
      adec_ptr->adpcm_state &= ~AUDFADEC_ADPCM_STATE_BUFFER2;
    }

    if(adec_ptr->length_not_read != 0) {
      if( (adec_ptr->track_buf == NULL) &&
          !(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING)){
        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;
          
        audfadec_last_cb = adec_ptr->handle->data_func;
        audmain_get_var_adec_data(adec_ptr->handle, adec_ptr->file_location,
                                  adec_ptr->buffer_length,
                                  adec_ptr->buffer_index,
                                  &adec_ptr->track_buf);
        MSG_MED("age_cont: request data for buf1", 0, 0, 0);
      } else if((adec_ptr->track_buf2 == NULL) &&
               !(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING)){
        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;
          
        audfadec_last_cb = adec_ptr->handle->data_func;
        audmain_get_var_adec_data(adec_ptr->handle, adec_ptr->file_location,
                                  adec_ptr->buffer_length,
                                  adec_ptr->buffer_index,
                                  &adec_ptr->track_buf2);
        MSG_MED("age_cont: request data for buf2", 0, 0, 0);
      }
    } else if(( adec_ptr->track_buf == NULL || 
                  adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER1) &&
                 ( adec_ptr->track_buf2 == NULL || 
                   adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER2)) {      
      return (AUDMAIN_STATUS_DONE);
    }
  }
  return(AUDMAIN_STATUS_SUCCESS);
}
#else /* FEATURE_GASYNTH_INTERFACE */
audmain_status_type audfadec_age_cont (
  audfadec_track_type *adec_ptr
) {
  if((adec_ptr != NULL) &&
     (adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) &&
     (adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA)) {
    if((adec_ptr->length_not_read == 0) &&
       (((adec_ptr->track_buf == NULL) &&
         (adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER2)) ||
        ((adec_ptr->track_buf2 == NULL) &&
         (adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER1)))) {
      return (AUDMAIN_STATUS_DONE);
    }

    if((adec_ptr->track_buf != NULL) &&
       (adec_ptr->track_buf_size == 0) &&
       (adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER2)) {
      audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                         (void *) adec_ptr->track_buf);
      adec_ptr->track_buf = NULL;
    }
    if((adec_ptr->track_buf2 != NULL) &&
       (adec_ptr->track_buf_size2 == 0) && 
       (adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER1)
       ) {
      audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                         (void *) adec_ptr->track_buf2);
      adec_ptr->track_buf2 = NULL;
    }

    if((!(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING)) &&
       (adec_ptr->length_not_read != 0)) {
      if(adec_ptr->track_buf == NULL) {
        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;

        audfadec_last_cb = adec_ptr->handle->data_func;
        audmain_get_var_adec_data(adec_ptr->handle, adec_ptr->file_location,
                                  adec_ptr->buffer_length,
                                  adec_ptr->buffer_index,
                                  &adec_ptr->track_buf);        
      } else if(adec_ptr->track_buf2 == NULL) {
        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;

        audfadec_last_cb = adec_ptr->handle->data_func;
        audmain_get_var_adec_data(adec_ptr->handle, adec_ptr->file_location,
                                  adec_ptr->buffer_length,
                                  adec_ptr->buffer_index,
                                  &adec_ptr->track_buf2);
      }
    }
  }
  return(AUDMAIN_STATUS_SUCCESS);
}
#endif /* FEATURE_GASYNTH_INTERFACE */
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_age

DESCRIPTION
  This function reads in the next buffer of audio decoder data, if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_age (
  audfadec_track_type *adec_ptr
) {
  audmain_status_type status;
  uint32 length;
#ifdef FEATURE_WEBAUDIO
  uint8  *buffer;
#ifdef FEATURE_QSYNTH_ADPCM
  audfadec_wa_adpcm_sample_rate_type  sample_rate;
  uint16 frame_size;
#endif /* FEATURE_QSYNTH_ADPCM */
#endif /* FEATURE_WEBAUDIO */

  if(adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) {
#ifdef FEATURE_QSYNTH_ADPCM
    if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
      MSG_MED("Call age_cont", 0, 0, 0);
      return(audfadec_age_cont(adec_ptr));
    }
#endif /* FEATURE_QSYNTH_ADPCM */
#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
      if ((adec_ptr->status_flag & AUDFADEC_MASK_AMR) ||
          (adec_ptr->status_flag & AUDFADEC_MASK_QCP)) {
        MSG_MED("Call age_midi_out_vocoder", 0, 0, 0);
        return(audfadec_age_midi_out_vocoder(adec_ptr));
      } 
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */
    
    if( adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT){
      if( adec_ptr->track_buf == NULL &&
          adec_ptr->track_buf_size == 0 && 
         !(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING) &&
          adec_ptr->track_buf2 != NULL && 
          adec_ptr->track_buf_size2 != 0){
        adec_ptr->track_buf = adec_ptr->track_buf2;
        adec_ptr->track_buf_size = adec_ptr->track_buf_size2;
        adec_ptr->track_buf2 = 0;
        adec_ptr->track_buf_size2 = 0;
        adec_ptr->track_index = 0;
        if( adec_ptr->status_flag & AUDFADEC_MASK_NEW_BLOCK2){
          adec_ptr->status_flag |= AUDFADEC_MASK_NEW_BLOCK;
          adec_ptr->status_flag &= ~AUDFADEC_MASK_NEW_BLOCK2;
        }
      }
    }

#ifdef FEATURE_WEBAUDIO
    if(adec_ptr->status_flag & AUDFADEC_MASK_NEW_BLOCK) {
      if(adec_ptr->track_buf != NULL) {
        if(adec_ptr->status_flag & AUDFADEC_MASK_MFI) {
          buffer = adec_ptr->track_buf;
          length = buffer[3] << 8 | buffer[4];
          if(buffer[12] == 0) {
            adec_ptr->status_flag &= ~AUDFADEC_MASK_MFI_CONT;
          }
          if(length > adec_ptr->track_buf_size - 5) {
            adec_ptr->length_not_read = length - (adec_ptr->track_buf_size - 5);
          } else {
            adec_ptr->length_not_read = 0;
            adec_ptr->track_buf_size  = length + 5;
          }
          adec_ptr->file_location  += adec_ptr->track_buf_size;

          adec_ptr->track_buf_size -= 13;
          adec_ptr->track_index     = 13;

        } else {
          buffer = adec_ptr->track_buf;

          length = buffer[2] << 8 | buffer[3];
          if(length > adec_ptr->track_buf_size - 4) {
            adec_ptr->length_not_read = length - (adec_ptr->track_buf_size - 4);
          } else {
            adec_ptr->length_not_read = 0;
            adec_ptr->track_buf_size  = length + 4;
          }
          adec_ptr->file_location += adec_ptr->track_buf_size;

          adec_ptr->next_block = buffer[6] << 24 | buffer[7] << 16 |
                                 buffer[8] << 8  | buffer[9];

          if(adec_ptr->next_block != 0) {
            adec_ptr->next_block  += adec_ptr->block_start;
            adec_ptr->status_flag |= AUDFADEC_MASK_NEXT_FLAG;
          } else {
            adec_ptr->status_flag &= ~AUDFADEC_MASK_NEXT_FLAG;
          }

          adec_ptr->track_buf_size -= 11;
          adec_ptr->track_index     = 11;

#ifdef FEATURE_QSYNTH_ADPCM
          if(((adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) ==
              AUDFADEC_MASK_ADPCM) &&
             ((adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) ==
              AUDFADEC_MASK_INIT_DONE)) {
            /* Check ADPCM sample rate and block size, return ERROR
            ** if they are different.
            */
            sample_rate = WA_ADPCM_RATE(buffer[11]);
            frame_size  = WA_ADPCM_BLK(buffer[11], buffer[12]);
            adec_ptr->track_buf_size -= 2;
            adec_ptr->track_index    += 2;
            if ((sample_rate != adec_ptr->sample_rate) || 
                (frame_size != adec_ptr->frame_size)) {
              return (AUDMAIN_STATUS_ERROR);
            }
          }
#endif /* FEATURE_QSYNTH_ADPCM */
        }

        adec_ptr->status_flag &= ~AUDFADEC_MASK_NEW_BLOCK;
      }
    }
#endif /* FEATURE_WEBAUDIO */

    if((adec_ptr->length_not_read == 0) && 
       (adec_ptr->track_buf == NULL) &&
       (!(adec_ptr->status_flag & AUDFADEC_MASK_NEW_BLOCK))) {
      if (adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT) {
        if ((adec_ptr->track_buf2 == NULL) &&
	        !( adec_ptr->status_flag & AUDFADEC_MASK_NEXT_FLAG) &&
            ((adec_ptr->buffer_length == 0) || 
             (adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER1)) &&
            ((adec_ptr->buffer_index == 0) ||
             (adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER2)) &&
            (adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE)
           ) {
          MSG_MED( "Done writing all PPBUF data to DSP", 0, 0,0);
          return (AUDMAIN_STATUS_DONE);
        }
      } else
#ifdef FEATURE_QSYNTH_ADPCM
      if (adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) {
        if (((adec_ptr->buffer_length == 0) && 
             (adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER2)) ||
            ((adec_ptr->buffer_index == 0) &&
             (adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER1))) {
          return (AUDMAIN_STATUS_DONE);
        }
      } else 
#endif /* FEATURE_QSYNTH_ADPCM */
      if (adec_ptr->buffer_length == 0) {
        return(AUDMAIN_STATUS_DONE);
      }
    }

    if(adec_ptr->track_buf != NULL) {
      length = adec_ptr->track_buf_size;
      status = audfadec_copy(adec_ptr, 
                             &(adec_ptr->track_buf[adec_ptr->track_index]),
                             &adec_ptr->track_buf_size);

      if(status == AUDMAIN_STATUS_SUCCESS) {
        if(adec_ptr->track_buf_size == 0) {
          audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                             (void *) adec_ptr->track_buf);

          adec_ptr->track_buf = NULL;
        } else {
          adec_ptr->track_index += (length - adec_ptr->track_buf_size);
        }
      } else if(status == AUDMAIN_STATUS_ERROR) {
        return(AUDMAIN_STATUS_ERROR);
      }
    }

    if((adec_ptr->track_buf == NULL) && 
       !(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING) && 
       ( adec_ptr->track_buf2 == NULL) &&
       (adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE)) {
      if(adec_ptr->length_not_read != 0) {
        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;

        audfadec_last_cb = adec_ptr->handle->data_func;
        (void) audmain_get_adec_data(adec_ptr->handle,
                                     adec_ptr->file_location,
                                     &(adec_ptr->track_buf));
      }
#ifdef FEATURE_WEBAUDIO
      else if(adec_ptr->status_flag & AUDFADEC_MASK_MFI) {
        if(adec_ptr->status_flag & AUDFADEC_MASK_MFI_CONT) {
          adec_ptr->status_flag |= (AUDFADEC_MASK_NEW_BLOCK |
                                    AUDFADEC_MASK_BUF_PENDING);

          audfadec_last_cb = adec_ptr->handle->data_func;
          (void) audmain_get_adec_data(adec_ptr->handle,
                                       adec_ptr->file_location,
                                       &(adec_ptr->track_buf));
        }
      } else if(adec_ptr->next_block != 0) {
        adec_ptr->block_start    = adec_ptr->next_block;
        adec_ptr->file_location  = adec_ptr->next_block;
        adec_ptr->status_flag   |= AUDFADEC_MASK_NEW_BLOCK;

        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;

        audfadec_last_cb = adec_ptr->handle->data_func;
        (void) audmain_get_adec_data(adec_ptr->handle,
                                     adec_ptr->file_location,
                                     &(adec_ptr->track_buf));
      }

#endif /* FEATURE_WEBAUDIO */
    } else if((adec_ptr->track_buf2== NULL) && 
       (adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT) &&
       !(adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING) &&
       (adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE)) {
      if(adec_ptr->length_not_read != 0) {
        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;

        audfadec_last_cb = adec_ptr->handle->data_func;
        (void) audmain_get_adec_data(adec_ptr->handle,
                                     adec_ptr->file_location,
                                     &(adec_ptr->track_buf2));
      }
#ifdef FEATURE_WEBAUDIO
      else if(adec_ptr->next_block != 0) {
        adec_ptr->block_start    = adec_ptr->next_block;
        adec_ptr->file_location  = adec_ptr->next_block;
        adec_ptr->status_flag   |= AUDFADEC_MASK_NEW_BLOCK2;

        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;

        audfadec_last_cb = adec_ptr->handle->data_func;
        (void) audmain_get_adec_data(adec_ptr->handle,
                                     adec_ptr->file_location,
                                     &(adec_ptr->track_buf2));
      }
#endif /* FEATURE_WEBAUDIO */
    }
  }

  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_process_repeat

DESCRIPTION
  This function does repeat processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfadec_process_repeat (
  audfadec_track_type *adec_ptr
) {
  if(adec_ptr->status_flag & AUDFADEC_MASK_REPEAT_ENABLED) {
    if(adec_ptr->repeat_num == 0) {
      adec_ptr->file_location   = adec_ptr->orig_file_location;
      adec_ptr->length_not_read = adec_ptr->orig_length_not_read;
    } else {
      adec_ptr->repeat_num--;
      if(adec_ptr->repeat_num > 0) {
        adec_ptr->file_location   = adec_ptr->orig_file_location;
        adec_ptr->length_not_read = adec_ptr->orig_length_not_read;
      } else {
        adec_ptr->length_not_read = 0;
      }
    }
  } else {
    adec_ptr->length_not_read = 0;
  }
}

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM
/*===========================================================================

FUNCTION audfadec_prepare_buffers

DESCRIPTION
  This function prepares initial data buffers for PCM and ADPCM files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_prepare_buffers (
  audfadec_track_type       *adec_ptr,
  uint32                    adec_mask,
  uint32                    data_length,
  uint32                    index,
  audmain_adpcm_format_type *format
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;
  uint32  tlong;

  MSG_MED("prepare adpcm buffer", 0, 0, 0);
  if((adec_ptr != NULL) && (format != NULL)) {
    adec_ptr->length_not_read = data_length; 
    index = index - adec_ptr->track_index;
    adec_ptr->track_buf_size      -= index;
    adec_ptr->orig_file_location   = index;
    adec_ptr->orig_length_not_read = data_length;
    adec_ptr->track_index          += index;

    tlong = ((format->sample_rate * format->bits_per_sample *
              format->channels) >> 3) / 10;
    
    if(adec_mask == AUDFADEC_MASK_ADPCM) {
      adec_ptr->buffer_index  = AUDFADEC_ADPCM_FRAME_LENGTH_MAX;
      /* keep track of frame size for deadlock prevention purpose */
      adec_ptr->frame_size    = ((((format->sample_rate / 400) + 3) * 
				  format->channels) + 2) * 2;
#ifdef FEATURE_QSYNTH_PCM
    } else if(adec_mask == AUDFADEC_MASK_PCM) {
      adec_ptr->buffer_index  = AUDFADEC_ADPCM_FRAME_LENGTH_MAX;
      /* keep track of frame size for deadlock prevention purpose */
      adec_ptr->frame_size    = ((tlong / 20) + 2) << 1;
#endif /* FEATURE_QSYNTH_PCM */
    } else {
      return(AUDMAIN_STATUS_ERROR);
    }

    adec_ptr->buffer_length   = MAX((((tlong >> 2) + 1) << 1),
                                    1000);
    
    adec_ptr->adpcm_state    = AUDFADEC_ADPCM_STATE_NONE;
    adec_ptr->status_flag   |= adec_mask;

    if (adec_ptr->track_buf_size < adec_ptr->length_not_read) {
      adec_ptr->length_not_read -= adec_ptr->track_buf_size;
    } else {
      adec_ptr->track_buf_size = adec_ptr->length_not_read;
      audfadec_process_repeat(adec_ptr);
    }

    if ((adec_ptr->track_buf_size < adec_ptr->frame_size) &&
        (adec_ptr->length_not_read != 0)) {
      memcpy(&adec_ptr->buffer[adec_ptr->intbuff_length],
             &adec_ptr->track_buf[adec_ptr->track_index], 
             adec_ptr->track_buf_size);
      adec_ptr->intbuff_length += adec_ptr->track_buf_size;
      adec_ptr->buffer_index   += adec_ptr->track_buf_size;
      audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                                 (void *) adec_ptr->track_buf);

      adec_ptr->status_flag   |= (AUDFADEC_MASK_INIT_DONE   |
                                  AUDFADEC_MASK_BUF_PENDING |
                                  AUDFADEC_MASK_CONT_DATA);
      
      adec_ptr->track_buf      = NULL;
      adec_ptr->track_index    = 0;
      adec_ptr->track_buf_size = 0;
      if(audmain_get_var_adec_data(adec_ptr->handle,
                                   adec_ptr->file_location,
                                   adec_ptr->buffer_length,
                                   adec_ptr->buffer_index,
                                   &adec_ptr->track_buf) ==
         AUDMAIN_STATUS_SUCCESS) {
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
    } else {
      adec_ptr->status_flag    |= (AUDFADEC_MASK_INIT_DONE |
                                     AUDFADEC_MASK_CONT_DATA);
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
  }
  return(ret_val);
}
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_add_command

DESCRIPTION
  This function formats the command for the audio decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_add_command (
  audmain_adec_cmd_type  *cmd_buf,
  audfadec_track_type    *adec_ptr,
  uint32                 channel
) {
#if defined(FEATURE_ANIMATED_AAC) || defined(FEATURE_GASYNTH_INTERFACE) || \
    defined(FEATURE_QSYNTH_ADPCM)
  audmain_status_type status;
#endif
  uint32 index = 0, length = 0, tlong = 0;
#ifdef FEATURE_WEBAUDIO
  uint8  *buffer;
#ifdef FEATURE_QSYNTH_ADPCM
  audfadec_wa_adpcm_sample_rate_type sample_rate;
  audmain_adpcm_mode_enum_type       adec_mode;
#endif /* FEATURE_QSYNTH_ADPCM */
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_QSYNTH_ADPCM
  audmain_adpcm_format_type format;
  qcpsup_adpcm_format_type  qcp_format;
#endif /* FEATURE_QSYNTH_ADPCM */
#if defined(FEATURE_QSYNTH_ADPCM) || defined(FEATURE_GASYNTH_INTERFACE)
  audmain_adec_start_mode_enum_type start_mode = AUDMAIN_ADEC_START_MODE_NORMAL;
#endif /* FEATURE_QSYNTH_ADPCM || FEATURE_GASYNTH_INTERFACE */

  cmd_buf->adec_num_buf1 = 0;
  cmd_buf->adec_buf1     = NULL;
  cmd_buf->adec_num_buf2 = 0;
  cmd_buf->adec_buf2     = NULL;

  if(adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) {

#ifdef FEATURE_WEBAUDIO
    if(adec_ptr->status_flag & AUDFADEC_MASK_DO_OFFSET) {
      if( ((adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT) != 0) &&
          (audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_QCP) ) {
        if( adec_ptr->track_buf_size >= 5){
          buffer = &adec_ptr->track_buf[adec_ptr->track_index];
         
          adec_ptr->next_block = buffer[0] << 24 | buffer[1] << 16 |
                                 buffer[2] << 8  | buffer[3];

          if(adec_ptr->next_block != 0) {
            adec_ptr->next_block  += adec_ptr->block_start;
            adec_ptr->status_flag |= AUDFADEC_MASK_NEXT_FLAG;
          } else {
            adec_ptr->status_flag &= ~AUDFADEC_MASK_NEXT_FLAG;
          }

          adec_ptr->track_buf_size -= 5;
          adec_ptr->track_index    += 5;
          adec_ptr->status_flag &= ~AUDFADEC_MASK_DO_OFFSET;
        }
      } else if(adec_ptr->buffer_length >= 5) {
        buffer = &adec_ptr->buffer[adec_ptr->buffer_index];
        adec_ptr->next_block = buffer[0] << 24 | buffer[1] << 16 |
                               buffer[2] << 8  | buffer[3];

        if(adec_ptr->next_block != 0) {
          adec_ptr->next_block  += adec_ptr->block_start;
          adec_ptr->status_flag |= AUDFADEC_MASK_NEXT_FLAG;
        } else {
          adec_ptr->status_flag &= ~AUDFADEC_MASK_NEXT_FLAG;
        }

        adec_ptr->buffer_length -= 5;
        adec_ptr->buffer_index  += 5;
        adec_ptr->status_flag &= ~AUDFADEC_MASK_DO_OFFSET;
      }
    }
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_ANIMATED_AAC
    if((adec_ptr->status_flag & AUDFADEC_MASK_AAC) &&
       !(adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) ){
      status = audfadec_get_aac_config(audfadec_parse_ctl.main_parse,
                                       &adec_ptr->aac_config.header_config,
                                       &adec_ptr->aac_sample_rate,
                                       &adec_ptr->buffer[adec_ptr->buffer_index],
                                       adec_ptr->buffer_length);
      if ( status == AUDMAIN_STATUS_SUCCESS) {
        if(audfadec_set_song_config(adec_ptr) == AUDMAIN_STATUS_SUCCESS) {
          adec_ptr->status_flag |= AUDFADEC_MASK_INIT_DONE;
        }
      } else if( status == AUDMAIN_STATUS_PENDING){
        return(AUDMAIN_STATUS_SUCCESS);
      } else {
        return (AUDMAIN_STATUS_FAILURE);
      }
    }
#endif /* FEATURE_ANIMATED_AAC */

    if((adec_ptr->status_flag & AUDFADEC_MASK_CHECK_AUDFMT) &&
       !(adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) &&
       !(adec_ptr->status_flag & AUDFADEC_MASK_DO_OFFSET)) {
      index = adec_ptr->buffer_index;
#ifdef FEATURE_GASYNTH_INTERFACE
      if (!(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) &&
          (qcpsup_do_amr_head(&adec_ptr->track_buf[adec_ptr->track_index],
                              &tlong,adec_ptr->track_buf_size) == 
           QCPSUP_STATUS_SUCCESS)) {
        MSG_MED("AMR header found", 0, 0, 0);
        adec_ptr->status_flag |= AUDFADEC_MASK_AMR;
        
        if(adec_ptr->status_flag & AUDFADEC_MASK_DELAYED_START) {
          start_mode = AUDMAIN_ADEC_START_MODE_DELAYED;
        }
        status = audmain_voc_adec_start(channel, 
					AUDMAIN_VOC_ADEC_AMR, 
					start_mode);
        if (status == AUDMAIN_STATUS_SUCCESS) 
        {
          adec_ptr->track_index         += tlong;
          adec_ptr->track_buf_size      -= tlong;
          
          adec_ptr->orig_file_location   = tlong;
          /* .amr file does not contain data chunk size */
          adec_ptr->orig_length_not_read = (adec_ptr->length_not_read + 
                                            adec_ptr->track_buf_size);
          if(adec_ptr->length_not_read == 0) {
            audfadec_process_repeat(adec_ptr);
          }

          adec_ptr->buffer_length   = 0;
          adec_ptr->buffer_index    = 0;
          adec_ptr->status_flag    |= AUDFADEC_MASK_INIT_DONE;
          tlong = adec_ptr->track_buf_size;
          (void) audfadec_copy(adec_ptr, 
                               &(adec_ptr->track_buf[
                                 adec_ptr->track_index]), 
                               &adec_ptr->track_buf_size);
          adec_ptr->track_index += (tlong - adec_ptr->track_buf_size);
          
        } else if (status == AUDMAIN_STATUS_PENDING) {
          return (AUDMAIN_STATUS_SUCCESS);
        } else {
          return (AUDMAIN_STATUS_FAILURE);
        }
      } else 
#endif /* FEATURE_GASYNTH_INTERFACE */
      if(!(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) &&
         qcpsup_do_head( &adec_ptr->track_buf[adec_ptr->track_index],
                         &tlong,
                         adec_ptr->track_buf_size,
                         &length, NULL ) == QCPSUP_STATUS_SUCCESS) {
        if(AUDFADEC_VERIFY_MODE(AUDMAIN_ADEC_MODE_QCP)) {
#ifdef FEATURE_GASYNTH_INTERFACE 
          adec_ptr->status_flag |= AUDFADEC_MASK_QCP;
          if(adec_ptr->status_flag & AUDFADEC_MASK_DELAYED_START) {
            start_mode = AUDMAIN_ADEC_START_MODE_DELAYED;
          }
          status = audmain_voc_adec_start(channel, 
                                          AUDMAIN_VOC_ADEC_QCP, 
                                          start_mode);
          if (status == AUDMAIN_STATUS_SUCCESS) 
#endif /* FEATURE_GASYNTH_INTERFACE */
          {
            adec_ptr->track_index         += tlong; 
            adec_ptr->track_buf_size      -= tlong;
            adec_ptr->orig_file_location   = tlong;
            adec_ptr->orig_length_not_read = length;
            if (length <= adec_ptr->track_buf_size) {
              audfadec_process_repeat(adec_ptr);
              /* Set the track_buf_size equal to 
                 length indicated in the header */
              adec_ptr->track_buf_size = length;
            } else {
              adec_ptr->length_not_read = length - adec_ptr->track_buf_size;
            }
            adec_ptr->buffer_length   = 0;
            adec_ptr->buffer_index    = 0;
            adec_ptr->status_flag    |= AUDFADEC_MASK_INIT_DONE;
            tlong = adec_ptr->track_buf_size;
            (void) audfadec_copy(adec_ptr, 
                                 &(adec_ptr->track_buf[
                                   adec_ptr->track_index]), 
                                 &adec_ptr->track_buf_size);
            adec_ptr->track_index += (tlong - adec_ptr->track_buf_size);
          } 
#ifdef FEATURE_GASYNTH_INTERFACE
          else if (status == AUDMAIN_STATUS_PENDING) {
            return (AUDMAIN_STATUS_SUCCESS);
          } else {
            return (AUDMAIN_STATUS_FAILURE);
          }
#endif /* FEATURE_GASYNTH_INTERFACE */
        } else {
          return(AUDMAIN_STATUS_FAILURE);
        }
      } else if(!(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) &&
                 ( adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT) &&
                qcpsup_validate_fixed_full(&adec_ptr->track_buf[adec_ptr->track_index],
                                           adec_ptr->track_buf_size)) {
        if(AUDFADEC_VERIFY_MODE(AUDMAIN_ADEC_MODE_QCP)) {
#ifdef FEATURE_GASYNTH_INTERFACE
          status = audmain_voc_adec_start(channel, 
                                          AUDMAIN_VOC_ADEC_QCP,
                                          start_mode);
          if (status == AUDMAIN_STATUS_SUCCESS) 
#endif /* FEATURE_GASYNTH_INTERFACE */
          {
            adec_ptr->orig_file_location   = 0;
            adec_ptr->orig_length_not_read = adec_ptr->buffer_length +
                                             adec_ptr->track_buf_size +
                                             adec_ptr->length_not_read;

            adec_ptr->buffer_length = 0;
            adec_ptr->buffer_index  = 0;
            adec_ptr->status_flag  |= AUDFADEC_MASK_INIT_DONE;
            (void) audfadec_copy(adec_ptr, &adec_ptr->track_buf[adec_ptr->track_index], &adec_ptr->track_buf_size);
          } 
        }
      } else if(!(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) &&
                qcpsup_validate_fixed_full(&(adec_ptr->buffer[index]),
                                           adec_ptr->buffer_length)) {
        if(AUDFADEC_VERIFY_MODE(AUDMAIN_ADEC_MODE_QCP)) {
#ifdef FEATURE_GASYNTH_INTERFACE
          if(adec_ptr->status_flag & AUDFADEC_MASK_DELAYED_START) {
            start_mode = AUDMAIN_ADEC_START_MODE_DELAYED;
          }
          status = audmain_voc_adec_start(channel, 
					  AUDMAIN_VOC_ADEC_QCP,start_mode);
          if (status == AUDMAIN_STATUS_SUCCESS) 
#endif /* FEATURE_GASYNTH_INTERFACE */
          {
            adec_ptr->orig_file_location   = 0;
            adec_ptr->orig_length_not_read = adec_ptr->buffer_length +
                                             adec_ptr->track_buf_size +
                                             adec_ptr->length_not_read;

            if(adec_ptr->length_not_read == 0) {
              audfadec_process_repeat(adec_ptr);
            }

            length = adec_ptr->buffer_length;

            adec_ptr->buffer_length = 0;
            adec_ptr->buffer_index  = 0;
            adec_ptr->status_flag  |= AUDFADEC_MASK_INIT_DONE;
            (void) audfadec_copy(adec_ptr, &(adec_ptr->buffer[index]), &length);
          } 
#ifdef FEATURE_GASYNTH_INTERFACE
          else if (status == AUDMAIN_STATUS_PENDING) {
            return (AUDMAIN_STATUS_SUCCESS);
          } else {
            return (AUDMAIN_STATUS_FAILURE);
          }
#endif /* FEATURE_GASYNTH_INTERFACE */
        } else {
          return(AUDMAIN_STATUS_FAILURE);
        }
#ifdef FEATURE_ANIMATED_AAC
      } else if(((adec_ptr->status_flag & AUDFADEC_MASK_AAC) ==
                 AUDFADEC_MASK_AAC) && 
                (adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC)) {
        if(audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC) {
          length =  adec_ptr->track_buf_size;
          if( audfadec_copy(adec_ptr, 
                              &(adec_ptr->track_buf[adec_ptr->track_index]),
                              &length) == AUDMAIN_STATUS_SUCCESS){
            adec_ptr->track_buf_size  -= length;
            if( adec_ptr->track_buf_size == 0){
              adec_ptr->track_index     = 0;
            }
          }
          
        } else {
          return(AUDMAIN_STATUS_FAILURE);
        }
#endif /* FEATURE_ANIMATED_AAC */

#ifdef FEATURE_QSYNTH_ADPCM
#ifdef FEATURE_WEBAUDIO
      } else if (((adec_ptr->status_flag & AUDFADEC_MASK_YADPCM) ==
                   AUDFADEC_MASK_YADPCM) &&
                 (adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC)) {
        if(AUDFADEC_VERIFY_MODE(AUDMAIN_ADEC_MODE_YADPCM)) {
#ifndef FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE
          format.sample_rate = (adec_ptr->sample_rate_hz + 50);
#else
	  format.sample_rate = adec_ptr->sample_rate_hz;
#endif /* FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE */

#ifndef FEATURE_STEREO_YADPCM
          if((adec_ptr->channels == 2) &&
             (adec_ptr->yadpcm_mode == AUDMAIN_ADPCM_YADPCM_MODE)) {
            format.channels         = 1;
            format.bits_per_sample  = 8;
            adec_mode               = AUDMAIN_ADPCM_SIGNED_PCM_MODE;   
            adec_ptr->status_flag  |= AUDFADEC_MASK_YADPCM_DEC;
            adec_ptr->signal_left   = 0;
            adec_ptr->step_left     = 0x7f;
            adec_ptr->signal_right  = 0;
            adec_ptr->step_right    = 0x7f;
          } else 
#endif /* !FEATURE_STEREO_YADPCM */
          {
            format.channels         = adec_ptr->channels;
            format.bits_per_sample  = adec_ptr->bits_per_sample;
			if(adec_ptr->yadpcm_mode == AUDMAIN_ADPCM_UNSIGNED_PCM_MODE ) {
             adec_mode = AUDMAIN_ADPCM_SIGNED_PCM_MODE;
            } else{
            adec_mode = adec_ptr->yadpcm_mode;
			}
          }
          format.bytes_rate = (format.sample_rate * format.channels) >> 1;

          status = audmain_adpcm_start(channel, AUDMAIN_ADPCM_PLAY_AUDFADEC,
                                       &format, adec_mode,
                                       AUDMAIN_ADEC_START_MODE_NORMAL);
          if(status == AUDMAIN_STATUS_SUCCESS) {
            adec_ptr->adpcm_state     = AUDFADEC_ADPCM_STATE_NONE;
            adec_ptr->status_flag    |= AUDFADEC_MASK_INIT_DONE;
            adec_ptr->frame_size      = adec_ptr->sample_rate_hz / 200;

            if(!(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA)) {
              index                     = adec_ptr->buffer_index;
              length                    = adec_ptr->buffer_length;
              adec_ptr->buffer_length   = 0;
              adec_ptr->buffer_index    = 0;
              (void) audfadec_copy(adec_ptr, &(adec_ptr->buffer[index]),
                                   &length);
              if(length != 0) {
                MSG_ERROR("Length should be zero: %d", length,0,0);
                return(AUDMAIN_STATUS_FAILURE);
              }
            }
          } else if(status == AUDMAIN_STATUS_PENDING) {
            return(AUDMAIN_STATUS_SUCCESS);
          } else {
            return(AUDMAIN_STATUS_FAILURE);
          }
        }
      } else if (((adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) ==
                   AUDFADEC_MASK_ADPCM) && 
                 (adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC)) {
        if(AUDFADEC_VERIFY_MODE(AUDMAIN_ADEC_MODE_ADPCM)) {
          if(adec_ptr->status_flag & AUDFADEC_MASK_MFI) {
            format.channels        = adec_ptr->channels;
            format.bits_per_sample = adec_ptr->bits_per_sample;
            format.sample_rate     = adec_ptr->sample_rate_hz;
            format.frame_size      = adec_ptr->frame_size;
            format.bytes_rate  = (format.sample_rate * format.channels) >> 1;

            /* Retry adpcm start, if qsynth image is not ready */
            status = audmain_adpcm_start(channel, AUDMAIN_ADPCM_PLAY_AUDFADEC,
                                         &format, AUDMAIN_ADPCM_ADPCM_MODE,
                                         AUDMAIN_ADEC_START_MODE_NORMAL);
            if(status == AUDMAIN_STATUS_SUCCESS) {
              adec_ptr->adpcm_state     = AUDFADEC_ADPCM_STATE_NONE;
              adec_ptr->status_flag    |= AUDFADEC_MASK_INIT_DONE;

              if(!(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA)) {
                length  = adec_ptr->buffer_length;
                index   = adec_ptr->buffer_index;
                adec_ptr->buffer_length   = 0;
                adec_ptr->buffer_index    = 0;
                (void) audfadec_copy(adec_ptr, &(adec_ptr->buffer[index]),
                                     &length);
                if(length != 0) {
                  MSG_ERROR("Length should be zero: %d", length,0,0);
                  return(AUDMAIN_STATUS_FAILURE);
                }
              }
            } else if(status == AUDMAIN_STATUS_PENDING) {
              return(AUDMAIN_STATUS_SUCCESS);
            } else {
              return(AUDMAIN_STATUS_FAILURE);
            }
          } else if(adec_ptr->buffer_length > 2) {
            index = adec_ptr->buffer_index;
            buffer = &(adec_ptr->buffer[index]);
            format.channels    = 1;
            sample_rate        = WA_ADPCM_RATE(buffer[0]);
            format.sample_rate = ADPCM_SAMPLE_RATE(sample_rate);
            format.bytes_rate  = (format.sample_rate * format.channels) >> 1; 
            format.frame_size  = WA_ADPCM_BLK(buffer[0], buffer[1]);
            format.bits_per_sample = 4;

            /* Retry adpcm start, if qsynth image is not ready */
            status = audmain_adpcm_start(channel, AUDMAIN_ADPCM_PLAY_AUDFADEC,
                                         &format, AUDMAIN_ADPCM_ADPCM_MODE,
                                         AUDMAIN_ADEC_START_MODE_NORMAL);
            if (status == AUDMAIN_STATUS_SUCCESS) {
              length                    = adec_ptr->buffer_length - 2;
              adec_ptr->buffer_length   = 0;
              adec_ptr->buffer_index    = 0;
              adec_ptr->adpcm_state     = AUDFADEC_ADPCM_STATE_NONE;
              adec_ptr->status_flag    |= AUDFADEC_MASK_INIT_DONE;
              adec_ptr->sample_rate     = sample_rate;
              adec_ptr->frame_size      = format.frame_size;
              (void) audfadec_copy(adec_ptr, &(adec_ptr->buffer[index+2]),
                                   &length);
              if(length != 0) {
                MSG_ERROR("Length should be zero: %d", length,0,0);
                return(AUDMAIN_STATUS_FAILURE);
              }
            } else if(status == AUDMAIN_STATUS_PENDING) {
              return(AUDMAIN_STATUS_SUCCESS);
            } else {
              return(AUDMAIN_STATUS_FAILURE);
            }
          }
        } else {
          return(AUDMAIN_STATUS_FAILURE);
        }
#endif /* FEATURE_WEBAUDIO */
      } else {
        index = adec_ptr->track_index;

        if(qcpsup_adpcm_do_head( &adec_ptr->track_buf[adec_ptr->track_index], 
                                 &index,
                                 adec_ptr->track_buf_size, &tlong, 
                                 &qcp_format ) == QCPSUP_STATUS_SUCCESS) {
          if (qcp_format.wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM) {
            if(AUDFADEC_VERIFY_MODE(AUDMAIN_ADEC_MODE_ADPCM)) {
              format.channels        = qcp_format.channels;
              format.sample_rate     = qcp_format.sample_rate;
              format.bytes_rate      = qcp_format.bytes_rate;
              format.frame_size      = qcp_format.frame_size;
              format.bits_per_sample = qcp_format.bits_per_sample;

              if(adec_ptr->status_flag & AUDFADEC_MASK_DELAYED_START) {
                start_mode = AUDMAIN_ADEC_START_MODE_DELAYED;
              }

              status = audmain_adpcm_start(channel, AUDMAIN_ADPCM_PLAY_AUDFADEC,
                                           &format, AUDMAIN_ADPCM_ADPCM_MODE,
                                           start_mode);
              if(status == AUDMAIN_STATUS_SUCCESS) {
                if(audfadec_prepare_buffers(adec_ptr, AUDFADEC_MASK_ADPCM,
                                            tlong, index, &format) !=
                   AUDMAIN_STATUS_SUCCESS) {
                  return(AUDMAIN_STATUS_ERROR);
                }
              } else if(status == AUDMAIN_STATUS_PENDING) {
                return(AUDMAIN_STATUS_SUCCESS);
              } else {
                return(AUDMAIN_STATUS_FAILURE);
              }
            } else {
              return(AUDMAIN_STATUS_FAILURE);
            }
#ifdef FEATURE_QSYNTH_PCM
          } else if (qcp_format.wave_format == QCPSUP_WAVE_FORMAT_LINEAR_PCM) {
            if(AUDFADEC_VERIFY_MODE(AUDMAIN_ADEC_MODE_PCM)) {
              format.channels        = qcp_format.channels;
              format.sample_rate     = qcp_format.sample_rate;
              format.bytes_rate      = qcp_format.bytes_rate;
              format.frame_size      = qcp_format.frame_size;
              format.bits_per_sample = qcp_format.bits_per_sample;

              if(adec_ptr->status_flag & AUDFADEC_MASK_DELAYED_START) {
                start_mode = AUDMAIN_ADEC_START_MODE_DELAYED;
              }

              if(qcp_format.bits_per_sample == 8) {
                status = audmain_adpcm_start(channel,
                                             AUDMAIN_ADPCM_PLAY_AUDFADEC,
                                             &format,
                                             AUDMAIN_ADPCM_UNSIGNED_PCM_MODE,
                                             start_mode);
              } else {
                status = audmain_adpcm_start(channel,
                                             AUDMAIN_ADPCM_PLAY_AUDFADEC,
                                             &format,
                                             AUDMAIN_ADPCM_SIGNED_PCM_MODE,
                                             start_mode);
              }
              if(status == AUDMAIN_STATUS_SUCCESS) {
                if(audfadec_prepare_buffers(adec_ptr, AUDFADEC_MASK_PCM,
                                            tlong, index, &format) !=
                   AUDMAIN_STATUS_SUCCESS) {
                  return(AUDMAIN_STATUS_ERROR);
                }
              } else if(status == AUDMAIN_STATUS_PENDING) {
                return(AUDMAIN_STATUS_SUCCESS);
              } else {
                return(AUDMAIN_STATUS_FAILURE);
              }
            } else {
              return(AUDMAIN_STATUS_FAILURE);
            }
#endif /* FEATURE_QSYNTH_PCM */
          } else {
            return(AUDMAIN_STATUS_FAILURE);
          }
        } else {
          return(AUDMAIN_STATUS_FAILURE);
        }
#endif /* FEATURE_QSYNTH_ADPCM */
      }

#ifdef FEATURE_MIDI_OUT_QCP
      if(!(adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC) &&
         (adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE)) {
        if((adec_ptr->handle != NULL) &&
           (adec_ptr->handle->data_func != NULL)) {
          adec_ptr->handle->data_func(SND_CMX_MIDI_OUT_QCP_START,
                                      adec_ptr->handle->audfmt_ptr, 0, NULL);
        }
      }
#endif /* FEATURE_MIDI_OUT_QCP */
    }

    if(adec_ptr->status_flag & AUDFADEC_MASK_DSP_RESET) {
#ifdef FEATURE_QSYNTH_ADPCM
      if((audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_ADPCM) &&
#ifdef FEATURE_QSYNTH_PCM
         (audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_PCM)   &&
#endif /* FEATURE_QSYNTH_PCM */
         (audmain_get_adec_mode() != AUDMAIN_ADEC_MODE_YADPCM))
#endif /* FEATURE_QSYNTH_ADPCM */
      {
        cmd_buf->adec_reset = TRUE;
      }
      adec_ptr->status_flag &= ~AUDFADEC_MASK_DSP_RESET;
    }

    if(adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) {
#ifdef FEATURE_QSYNTH_ADPCM
      if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
#ifdef FEATURE_GASYNTH_INTERFACE
        cmd_buf->adec_buf1           = &adec_ptr->track_buf[
                                       adec_ptr->track_index];
        cmd_buf->adec_num_buf1       = adec_ptr->track_buf_size;
        cmd_buf->adec_buf2           = &adec_ptr->track_buf2[
                                       adec_ptr->track_index2];
        cmd_buf->adec_num_buf2       = adec_ptr->track_buf_size2;        
        MSG_MED("add_command: filling data to lower layer", 0, 0,0);
#else
        if(!(adec_ptr->status_flag & AUDFADEC_MASK_COPY_PEND)) {
          if(adec_ptr->adpcm_state != AUDFADEC_ADPCM_STATE_BUFFER1) {
            if(adec_ptr->track_buf_size != 0) {
              cmd_buf->adec_buf1     = adec_ptr->track_buf;
              cmd_buf->adec_buf2     = &adec_ptr->track_buf
                                             [adec_ptr->track_index];
              cmd_buf->adec_num_buf1 = adec_ptr->track_buf_size;
              cmd_buf->adec_num_buf2 = adec_ptr->track_index;
            }
          } else {
            if(adec_ptr->track_buf_size2 != 0) {
              cmd_buf->adec_buf1     = adec_ptr->track_buf2;
              cmd_buf->adec_buf2     = &adec_ptr->track_buf2
                                             [adec_ptr->track_index2];
              cmd_buf->adec_num_buf1 = adec_ptr->track_buf_size2;
              cmd_buf->adec_num_buf2 = adec_ptr->track_index2;
            }
          }
        }
#endif /* FEATURE_GASYNTH_INTERFACE */
      } else if( (adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT) != 0) {
#ifndef FEATURE_GASYNTH_INTERFACE
        if(adec_ptr->adpcm_state != AUDFADEC_ADPCM_STATE_BUFFER1) 
#endif
        {
          if(adec_ptr->buffer_length != 0 && !(adec_ptr->buffer_length & 0x1)) {
            cmd_buf->adec_buf1     = adec_ptr->buffer;
            cmd_buf->adec_num_buf1 = adec_ptr->buffer_length;
#ifndef FEATURE_GASYNTH_INTERFACE
            cmd_buf->adec_buf2     = &adec_ptr->buffer
                                           [AUDFADEC_ADPCM_FRAME_LENGTH_MAX];
            cmd_buf->adec_num_buf2 = AUDFADEC_ADPCM_FRAME_LENGTH_MAX;
#endif
           
          }
        } 
#ifndef FEATURE_GASYNTH_INTERFACE
        else if( adec_ptr->adpcm_state != AUDFADEC_ADPCM_STATE_BUFFER2)
#endif
        {
          if(adec_ptr->buffer_index != 0 && !(adec_ptr->buffer_index & 0x1)) {
#ifndef FEATURE_GASYNTH_INTERFACE
            cmd_buf->adec_buf1  = &adec_ptr->buffer[AUDFADEC_INT_BUF_LENGTH];
            cmd_buf->adec_num_buf1 = adec_ptr->buffer_index;
            cmd_buf->adec_buf2  = &adec_ptr->buffer[AUDFADEC_INT_BUF_LENGTH +
                                               AUDFADEC_ADPCM_FRAME_LENGTH_MAX];
            cmd_buf->adec_num_buf2 = AUDFADEC_ADPCM_FRAME_LENGTH_MAX;
#else            
            cmd_buf->adec_buf2  = &adec_ptr->buffer[AUDFADEC_INT_BUF_LENGTH];
            cmd_buf->adec_num_buf2 = adec_ptr->buffer_index;
#endif
            
          }
        }
        
#ifdef FEATURE_WEBAUDIO
        if(((adec_ptr->status_flag & AUDFADEC_MASK_YADPCM) ==
            AUDFADEC_MASK_YADPCM) &&
           (cmd_buf->adec_num_buf1 < adec_ptr->frame_size)) {
          cmd_buf->adec_num_buf1 = 0;
          cmd_buf->adec_buf1     = NULL;
          cmd_buf->adec_num_buf2 = 0;
          cmd_buf->adec_buf2     = NULL;
        }
#endif /* FEATURE_WEBAUDIO */
      } else
#endif /* FEATURE_QSYNTH_ADPCM */
#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
      if ((adec_ptr->status_flag & AUDFADEC_MASK_AMR) ||
          (adec_ptr->status_flag & AUDFADEC_MASK_QCP)) {
        if (adec_ptr->buffer_length > 0) {
          MSG_MED("VOC:Write intbuf1 length=%d",adec_ptr->buffer_length, 0, 0);
          cmd_buf->adec_buf1     = adec_ptr->buffer;
          cmd_buf->adec_num_buf1 = adec_ptr->buffer_length;
          /* Lock up internal buffer1 */                    
        }

        if (adec_ptr->buffer_index > 0) {
          MSG_MED("VOC:Write intbuf2 length=%d",adec_ptr->buffer_index, 0, 0);
          /* Lock up internal buffer2 */          
          cmd_buf->adec_buf2  = &adec_ptr->buffer[AUDFADEC_INT_BUF_LENGTH];
          cmd_buf->adec_num_buf2 = adec_ptr->buffer_index;
        }
      } else 
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */
      {
        cmd_buf->adec_num_buf1 = MIN(adec_ptr->buffer_length,
                                 AUDFADEC_BUF_LENGTH - adec_ptr->buffer_index);
        cmd_buf->adec_buf1     = &(adec_ptr->buffer[adec_ptr->buffer_index]);
        cmd_buf->adec_num_buf2 = adec_ptr->buffer_length -
                                                       cmd_buf->adec_num_buf1;
        cmd_buf->adec_buf2     = &(adec_ptr->buffer[0]);
      }
    }

    return(AUDMAIN_STATUS_SUCCESS);
  }

  if(adec_ptr->handle != NULL) {
    audfadec_reset_check_status(channel, adec_ptr);
  }

  return(AUDMAIN_STATUS_DONE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_adjust

DESCRIPTION
  This function adjusts the adec buffer by the indicated number of bytes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GASYNTH_INTERFACE
audmain_status_type audfadec_adjust (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   *buf1_used,
  uint32                   *buf2_used,
  uint8                    *buf_usage_mask
) {
  audfadec_parse_ctl_type *adec_parse;
  audfadec_track_type     *adec_ptr;
  audmain_status_type     ret_val = AUDMAIN_STATUS_SUCCESS;
  uint32                  counter, num_used;

  adec_parse = (audfadec_parse_ctl_type *) (parse_ctl->audfadec_tracks);

  for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter ++) {
    adec_ptr   = &(adec_parse->tracks[counter]);

    if(adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) {
      num_used = buf1_used[counter] + buf2_used[counter];
#ifdef FEATURE_QSYNTH_ADPCM
      if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
          adec_ptr->adpcm_state = buf_usage_mask[counter];
        
        MSG_MED("ch%d state=%d", counter, adec_ptr->adpcm_state, 0);
        if (num_used > 0) {
          adec_ptr->track_buf_size -= buf1_used[counter];
          adec_ptr->track_buf_size2 -= buf2_used[counter];
          (void) audfadec_age(adec_ptr);
        }
      } else if((adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) ||
#ifdef FEATURE_MIDI_OUT_QCP
                (adec_ptr->status_flag & AUDFADEC_MASK_AMR)   ||
                (adec_ptr->status_flag & AUDFADEC_MASK_QCP)   ||
#endif /* FEATURE_MIDI_OUT_QCP */
                (adec_ptr->status_flag & AUDFADEC_MASK_PPBUF_INT) ) {
        adec_ptr->adpcm_state = buf_usage_mask[counter];

        if (adec_ptr->adpcm_state & AUDFADEC_ADPCM_STATE_BUFFER1) {
          adec_ptr->adpcm_state  &= ~AUDFADEC_ADPCM_STATE_LOCKED1;
        }

        if(num_used != 0) {
          adec_ptr->buffer_length -= buf1_used[counter];
          adec_ptr->buffer_index  -= buf2_used[counter];
          if( adec_ptr->buffer_length == 0){
             MSG_MED( "Freeing buffer1 on ch%ld", counter, 0, 0);
              adec_ptr->adpcm_state &= ~AUDFADEC_ADPCM_STATE_BUFFER1;
          }
          if( adec_ptr->buffer_index == 0){
             MSG_MED( "Freeing buffer2 on ch%ld", counter, 0, 0);
              adec_ptr->adpcm_state &= ~AUDFADEC_ADPCM_STATE_BUFFER2;
          }
          (void) audfadec_age(adec_ptr);
        }
      } else
#endif /* FEATURE_QSYNTH_ADPCM */
      {
        if(adec_ptr->buffer_length < num_used) {
          MSG_ERROR("Invalid length!!! %d, %d", adec_ptr->buffer_length, 
                                                num_used, 0);
          audfadec_reset_check_status(counter, adec_ptr);
          ret_val = AUDMAIN_STATUS_FAILURE;
        } else if(adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) {
          adec_ptr->buffer_length -= num_used;
          adec_ptr->buffer_index  += num_used;
          if(adec_ptr->buffer_index > AUDFADEC_BUF_LENGTH) {
            adec_ptr->buffer_index -= AUDFADEC_BUF_LENGTH;
          }else if( (adec_ptr->buffer_length < adec_ptr->buffer_index) &&
                    (adec_ptr->status_flag & AUDFADEC_MASK_AAC) &&
                    ( adec_ptr->buffer_index + adec_ptr->buffer_length < AUDFADEC_BUF_LENGTH)
          ){
            memcpy( (void*)adec_ptr->buffer, (void*) &adec_ptr->buffer[adec_ptr->buffer_index],
                    adec_ptr->buffer_length);
            adec_ptr->buffer_index = 0;
          }

          if( (adec_ptr->status_flag & AUDFADEC_MASK_AAC) &&
              adec_ptr->buffer_length == 0 &&
              adec_ptr->buffer_index == AUDFADEC_BUF_LENGTH
            ){
            adec_ptr->buffer_index = 0;
          }
        }
      }
    }
  }

  return(ret_val);
}
#else /* FEATURE_GASYNTH_INTERFACE */
audmain_status_type audfadec_adjust (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   *num_used
) {
  audfadec_parse_ctl_type *adec_parse;
  audfadec_track_type     *adec_ptr;
  audmain_status_type     ret_val = AUDMAIN_STATUS_SUCCESS;
  uint32                  counter;

  adec_parse = (audfadec_parse_ctl_type *) (parse_ctl->audfadec_tracks);

  for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter ++) {
    adec_ptr   = &(adec_parse->tracks[counter]);

    if(adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) {
#ifdef FEATURE_QSYNTH_ADPCM
      if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
        if(num_used[counter] != 0) {
          if(adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER2) {
            adec_ptr->adpcm_state     = AUDFADEC_ADPCM_STATE_BUFFER1;
            adec_ptr->track_buf_size2 = 0;
          } else if(adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER1) {
            adec_ptr->adpcm_state     = AUDFADEC_ADPCM_STATE_BUFFER2;
            adec_ptr->track_buf_size  = 0;
          } else {
            adec_ptr->adpcm_state     = AUDFADEC_ADPCM_STATE_BUFFER1;
          }
          (void) audfadec_age(adec_ptr);
        }
      } else if((adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) ==
                AUDFADEC_MASK_ADPCM) {
        if(num_used[counter] != 0) {
          if(adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER2) {
            adec_ptr->adpcm_state  = AUDFADEC_ADPCM_STATE_BUFFER1;
            adec_ptr->buffer_index = 0;
          } else if(adec_ptr->adpcm_state == AUDFADEC_ADPCM_STATE_BUFFER1) {
            adec_ptr->adpcm_state   = AUDFADEC_ADPCM_STATE_BUFFER2;
            adec_ptr->buffer_length = 0;
          } else {
            adec_ptr->adpcm_state = AUDFADEC_ADPCM_STATE_BUFFER1;
          }
          (void) audfadec_age(adec_ptr);
        }
      } else
#endif /* FEATURE_QSYNTH_ADPCM */
      {
        if(adec_ptr->buffer_length < num_used[counter]) {
          MSG_ERROR("Invalid length!!! %d, %d", adec_ptr->buffer_length, 
                                                num_used[counter], 0);
          audfadec_reset_check_status(counter, adec_ptr);
          ret_val = AUDMAIN_STATUS_FAILURE;
        } else if(adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) {
          adec_ptr->buffer_length -= num_used[counter];
          adec_ptr->buffer_index  += num_used[counter];
          if(adec_ptr->buffer_index > AUDFADEC_BUF_LENGTH) {
            adec_ptr->buffer_index -= AUDFADEC_BUF_LENGTH;
          }else if( (adec_ptr->buffer_length < adec_ptr->buffer_index) &&
                    (adec_ptr->status_flag & AUDFADEC_MASK_AAC) &&
                    ( adec_ptr->buffer_index + adec_ptr->buffer_length <= AUDFADEC_BUF_LENGTH)
          ){
            memcpy( (void*)adec_ptr->buffer, (void*) &adec_ptr->buffer[adec_ptr->buffer_index],
                    adec_ptr->buffer_length);
            adec_ptr->buffer_index = 0;
          }

          if( (adec_ptr->status_flag & AUDFADEC_MASK_AAC) &&
              adec_ptr->buffer_length == 0 &&
              adec_ptr->buffer_index == AUDFADEC_BUF_LENGTH
            ){
            adec_ptr->buffer_index = 0;
          }
        }
      }
    }
  }

  return(ret_val);

}
#endif /* FEATURE_GASYNTH_INTERFACE */
/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_get_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming data buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfadec_get_data_cb_req (
  uint8 **data,
  uint8 *buf,
  uint32 length,
  uint32 pos
  /*lint -esym(715,pos) Tell Lint we know pos parameter is not used */
) {
  audfadec_track_type *adec_ptr;
  boolean             ret_val = FALSE;
  uint32              tlong, offset,index;
  int                 counter;

  for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter ++) {
    adec_ptr = &audfadec_parse_ctl.tracks[counter];
    if((&(adec_ptr->track_buf) == data) || (&(adec_ptr->track_buf2) == data)) {
      if(adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) {
        if(buf == NULL) {
          adec_ptr->status_flag    &= ~AUDFADEC_MASK_BUF_PENDING;
          audfadec_reset(counter, adec_ptr);
          MSG_HIGH("Got NULL buffer back reset", 0, 0, 0);
        } else if((adec_ptr->status_flag & AUDFADEC_MASK_SKIP_BUF) && 
                  (adec_ptr->skip_position == pos)) {
          MSG_MED("Dropping buffer", 0,0,0);
          adec_ptr->status_flag &= ~AUDFADEC_MASK_SKIP_BUF;
          if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
            audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                               (void *) buf);
          } else {
            audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                               (void *) buf);
          }
        } else if((adec_ptr->status_flag & AUDFADEC_MASK_BUF_PENDING)) {
          if (((uint32) buf) & 0x1) {
            MSG_ERROR("odd byte address", 0, 0, 0);
          }

          /* Set the flag so parser knows to check for the audio format
            */
          adec_ptr->status_flag |= AUDFADEC_MASK_CHECK_AUDFMT;

          *data = buf;
          if(( adec_ptr->status_flag & AUDFADEC_MASK_NEW_BLOCK) || 
             ( adec_ptr->status_flag & AUDFADEC_MASK_NEW_BLOCK2)) {
            tlong = length;
            MSG_MED( "New bistream block", 0, 0, 0);
          } else {
            tlong = MIN( adec_ptr->length_not_read, length );
            adec_ptr->file_location   += tlong;
            adec_ptr->length_not_read -= tlong;

            if(adec_ptr->status_flag & AUDFADEC_MASK_INIT_DONE) {
              if(adec_ptr->length_not_read == 0) {
                audfadec_process_repeat(adec_ptr);
              }
            }
          }
          if(adec_ptr->status_flag & AUDFADEC_MASK_CONT_DATA) {
            if ((adec_ptr->length_not_read != 0) && 
                (adec_ptr->intbuff_length + tlong) < 
                adec_ptr->frame_size) {
              /* Not enough to give it to the driver, copy to internal buffer 
              first */
              memcpy(&adec_ptr->buffer[adec_ptr->intbuff_length],
                     &buf[adec_ptr->buffer_index], 
                     tlong);
              adec_ptr->intbuff_length += tlong;
              adec_ptr->buffer_index += tlong;
              tlong = 0;
              *data = NULL;
              audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                                 (void *) buf);
              MSG_MED("Deadlock prevention: copy to internal buffer",0,0,0); 
            } else if (adec_ptr->intbuff_length > 0) {
                  /* Assume there is a empty buffer before start of actual audio
              data with the length equal to adec_ptr->buffer_length */
			  if (adec_ptr->buffer_index >= adec_ptr->intbuff_length ) {
                adec_ptr->buffer_index -= adec_ptr->intbuff_length;
                memcpy(&buf[adec_ptr->buffer_index], 
                       adec_ptr->buffer, 
                       adec_ptr->intbuff_length);
              } else {
				MSG_ERROR("Parser bug: aborting playback", 0, 0, 0);
                audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                                   (void *) buf);
                adec_ptr->status_flag    &= ~AUDFADEC_MASK_BUF_PENDING;
				audfadec_reset(counter, adec_ptr);
			  }
              MSG_MED("Move data from internal buffer back to CMX buffer"
                      ,0,0,0);
              tlong += adec_ptr->intbuff_length;
              adec_ptr->intbuff_length = 0;                
            } 

           #ifdef FEATURE_WEBAUDIO
			/*for offset binary adpcm in SMAF*/
			if ((buf != NULL) && (adec_ptr->status_flag & AUDFADEC_MASK_YADPCM)
				 && (adec_ptr->yadpcm_mode == AUDMAIN_ADPCM_UNSIGNED_PCM_MODE)){
				for (index=0;index < tlong ;index++) {
					buf[index]=buf[index]-0x80;
				}
            } 
           #endif
            offset = adec_ptr->buffer_index;
          } else if ((adec_ptr->status_flag & AUDFADEC_MASK_AMR) ||
                     (adec_ptr->status_flag & AUDFADEC_MASK_QCP)) {
              if ((adec_ptr->status_flag & AUDFADEC_MASK_COPY_PEND) &&
                  (&(adec_ptr->track_buf2) == data) &&
                  (adec_ptr->track_buf != NULL)) {  
                MSG_MED("AMR: Combine two client buffers", 0, 0, 0);
                memcpy(buf, 
                       &adec_ptr->track_buf[adec_ptr->track_index],
                       adec_ptr->track_buf_size);
                audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                                   (void *) adec_ptr->track_buf);
                adec_ptr->track_buf       = buf;
                adec_ptr->track_buf_size += tlong;
                adec_ptr->track_index     = 0;
                adec_ptr->track_buf2      = NULL;
                adec_ptr->status_flag    &= ~(AUDFADEC_MASK_BUF_PENDING | 
                                              AUDFADEC_MASK_COPY_PEND);
              } else {
                MSG_ERROR("Parser bug: abort playback", 0, 0, 0);
                audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                                   (void *) buf);
                adec_ptr->status_flag    &= ~AUDFADEC_MASK_BUF_PENDING;
                audfadec_reset(counter, adec_ptr);
              }
              return(TRUE);
          } else {
            offset = 0;
          }

          if(&(adec_ptr->track_buf) == data) {
            adec_ptr->track_buf_size  = tlong;
            adec_ptr->track_index     = offset;
            MSG_MED("Get buf1 back", 0, 0, 0);
           } else if( &(adec_ptr->track_buf2) == data) {
            adec_ptr->track_buf_size2 = tlong;
            adec_ptr->track_index2    = offset;
            MSG_MED("Get buf2 back", 0, 0, 0);
          }

          adec_ptr->status_flag    &= ~AUDFADEC_MASK_BUF_PENDING;

         } else {
          MSG_ERROR("Received unrequested buffer: %d",pos,0,0);
          /* Received unexpected buffer, free it. */
          audmain_do_play_cb(adec_ptr->handle, SND_CMX_AF_FREE_BUF, 0,
                             (void *) buf);
        }
      } else {
        if(adec_ptr->status_flag & AUDFADEC_MASK_SKIP_BUF) {
          adec_ptr->status_flag &= ~AUDFADEC_MASK_SKIP_BUF;
        }
        audfadec_last_cb(SND_CMX_AF_FREE_BUF, NULL, NULL, buf);
      }
      ret_val = TRUE;
      break;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_parse

DESCRIPTION
  This function will parse the indicated QCP file and prepare the command
  buffer with data for playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_adec_cmd_type   *cmd_buf
) {
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;
  audmain_status_type     status;
  audfadec_parse_ctl_type *adec_parse;
  audfadec_track_type     *adec_ptr;
  uint32                  counter;

  if((parse_ctl != NULL) && (parse_ctl->audfadec_tracks != NULL)) {
    adec_parse = (audfadec_parse_ctl_type *) (parse_ctl->audfadec_tracks);
    for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter ++) {
      adec_ptr = &(adec_parse->tracks[counter]);

      if(adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) {
        status = audfadec_age(adec_ptr);

        if(status == AUDMAIN_STATUS_SUCCESS) {
          status = audfadec_add_command(&(cmd_buf[counter]), adec_ptr, counter);
        }

#ifdef FEATURE_GASYNTH_INTERFACE
        cmd_buf[counter].buf_usage_mask = adec_ptr->adpcm_state;
#endif /* FEATURE_GASYNTH_INTERFACE */

        if(status != AUDMAIN_STATUS_SUCCESS) {
#ifdef FEATURE_MIDI_OUT_QCP
          if ((status == AUDMAIN_STATUS_FAILURE) 
#ifdef FEATURE_WEBAUDIO
              && !(adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC)
#endif /* FEATURE_WEBAUDIO */
             ) {
            audmain_do_play_cb(adec_ptr->handle, SND_CMX_MIDI_OUT_QCP_ABORT,
                               0, NULL);
          }
#endif /* FEATURE_MIDI_OUT_QCP */
#ifdef FEATURE_ANIMATED_AAC
          if((adec_ptr->status_flag & AUDFADEC_MASK_AAC) != AUDFADEC_MASK_AAC)
#endif
          {
#ifdef FEATURE_GASYNTH_INTERFACE
            if ((status == AUDMAIN_STATUS_DONE) && 
                (audmain_adec_data_consumed(counter) == FALSE)) {
              MSG_MED("Wait until decoder finish consuming data.", 
                       0, 0, 0);
              ret_val = AUDMAIN_STATUS_SUCCESS;
              cmd_buf[counter].adec_buf1 = NULL;
              cmd_buf[counter].adec_buf2 = NULL;
              cmd_buf[counter].adec_num_buf1 = 0;
              cmd_buf[counter].adec_num_buf2 = 0;
            } else 
#endif /* FEATURE_GASYNTH_INTERFACE */
            {
              audfadec_reset_check_status(counter, adec_ptr);
            }
          }
        } else {
          ret_val = AUDMAIN_STATUS_SUCCESS;
        }
      }
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_reset_channel

DESCRIPTION
  This function resets the indicated QCP file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_reset_channel (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  channel
) {
  audfadec_parse_ctl_type *adec_parse;
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;
  int                     counter;

  if((parse_ctl != NULL) && (parse_ctl->audfadec_tracks != NULL) &&
     (channel < AUDFADEC_NUM_TRACKS)) {
    adec_parse = (audfadec_parse_ctl_type *) (parse_ctl->audfadec_tracks);
    audfadec_reset_check_status(channel, &(adec_parse->tracks[channel]) );

    ret_val = AUDMAIN_STATUS_DONE;

    for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter++) {
      if(adec_parse->tracks[counter].status_flag &
                                   AUDFADEC_MASK_ADEC_ENABLED) {
        ret_val = AUDMAIN_STATUS_SUCCESS;
        break;
      }
    }
  }

  return(ret_val);
}

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION audfadec_wa_reset

DESCRIPTION
  This function resets all WebAudio audio decoders.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfadec_wa_reset (
  uint32  channel,
  boolean clear_data_req
) {
  uint32  status;

  if(channel < AUDFADEC_NUM_TRACKS) {
    status = audfadec_parse_ctl.tracks[channel].status_flag;

    if(status & AUDFADEC_MASK_WA_ADEC) {
      audfadec_reset(channel, &audfadec_parse_ctl.tracks[channel]);
      if(clear_data_req) {
        audfadec_parse_ctl.tracks[channel].status_flag &=
                                                    ~AUDFADEC_MASK_SKIP_BUF;
      }

    }
    audfadec_parse_ctl.tracks[channel].status_flag &= ~AUDFADEC_MASK_WA_PEND;
    audfadec_parse_ctl.tracks[channel].wa_reserve.status_flag &= 
                                                       ~AUDFADEC_MASK_WA_PEND;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_wa_activate

DESCRIPTION
  This function will activate any pending audio decoders.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void audfadec_wa_activate ( uint32 channel )
{
  audfadec_track_type *adec_ptr;
  boolean             activate;

#ifdef FEATURE_QSYNTH_ADPCM
  uint32 tlong;
#endif /* FEATURE_QSYNTH_ADPCM */

  if(channel < AUDFADEC_NUM_TRACKS) {
    activate = FALSE;
    adec_ptr = &audfadec_parse_ctl.tracks[channel];

    if((adec_ptr->status_flag & AUDFADEC_MASK_WA_PEND) ==
       AUDFADEC_MASK_WA_PEND) {
      adec_ptr->status_flag &= ~AUDFADEC_MASK_WA_PEND;
      activate               = TRUE;
    } else if((adec_ptr->wa_reserve.status_flag & AUDFADEC_MASK_WA_PEND) ==
              AUDFADEC_MASK_WA_PEND) {
      audfadec_reset(channel, adec_ptr );

      if( adec_ptr->status_flag & AUDFADEC_MASK_AAC){
        adec_ptr->buffer_index = 0;
      }                            

      adec_ptr->wa_reserve.status_flag &= ~AUDFADEC_MASK_WA_PEND;
      adec_ptr->status_flag            |= adec_ptr->wa_reserve.status_flag;

      adec_ptr->handle          = adec_ptr->wa_reserve.handle;
      adec_ptr->block_start     = adec_ptr->wa_reserve.block_start;
      adec_ptr->next_block      = adec_ptr->wa_reserve.next_block;
      adec_ptr->length_not_read = adec_ptr->wa_reserve.length_not_read;
      adec_ptr->file_location   = adec_ptr->wa_reserve.file_location;

      adec_ptr->sample_rate_hz  = adec_ptr->wa_reserve.sample_rate_hz;
      adec_ptr->bits_per_sample = adec_ptr->wa_reserve.bits_per_sample;
      adec_ptr->channels        = adec_ptr->wa_reserve.channels;
      adec_ptr->frame_size      = adec_ptr->wa_reserve.frame_size;
      adec_ptr->yadpcm_mode     = adec_ptr->wa_reserve.yadpcm_mode;

#ifdef FEATURE_ANIMATED_AAC
      adec_ptr->aac_sample_rate = adec_ptr->wa_reserve.aac_sample_rate;
      memcpy( (void*)&adec_ptr->aac_config, 
              (void*)&adec_ptr->wa_reserve.aac_config,
              sizeof( audmain_song_config_type));
#endif

      activate = TRUE;
    }

    if(activate) {
      audfadec_parse_ctl.main_parse->audfadec_tracks  = &audfadec_parse_ctl; 
      audfadec_parse_ctl.main_parse->audfadec_enabled = TRUE;

      if(adec_ptr->status_flag & AUDFADEC_MASK_AAC) {
        adec_ptr->buffer_index = 0;
      }
#ifdef FEATURE_GASYNTH_INTERFACE
      else{
        adec_ptr->status_flag  |= AUDFADEC_MASK_PPBUF_INT ;
      }
#endif

      adec_ptr->status_flag |= AUDFADEC_MASK_ADEC_ENABLED |
                               AUDFADEC_MASK_WA_ADEC;

#ifdef FEATURE_QSYNTH_ADPCM
      if((adec_ptr->status_flag & AUDFADEC_MASK_ADPCM) &&
         (((adec_ptr->status_flag & AUDFADEC_MASK_YADPCM)
#ifndef FEATURE_STEREO_YADPCM
           && (adec_ptr->channels == 1)
#endif /* !FEATURE_STEREO_YADPCM */
          ) ||
          ((adec_ptr->status_flag & AUDFADEC_MASK_MFI) &&
           !(adec_ptr->status_flag & AUDFADEC_MASK_MFI_CONT))) &&
         ! (adec_ptr->status_flag & AUDFADEC_MASK_DO_OFFSET) &&
         (adec_ptr->next_block == 0)) {
        tlong = ((adec_ptr->sample_rate_hz * adec_ptr->bits_per_sample *
                  adec_ptr->channels) >> 3) / 10;

        adec_ptr->buffer_index    = AUDFADEC_ADPCM_FRAME_LENGTH_MAX;
        adec_ptr->buffer_length   = MAX((((tlong >> 2) + 1) << 1),
                                        1000);
        adec_ptr->status_flag |= (AUDFADEC_MASK_BUF_PENDING |
                                  AUDFADEC_MASK_CONT_DATA);

        audmain_get_var_adec_data(adec_ptr->handle,
                                  adec_ptr->file_location,
                                  adec_ptr->buffer_length,
                                  adec_ptr->buffer_index,
                                  &adec_ptr->track_buf);
      } else 
#endif /* FEATURE_QSYNTH_ADPCM */
      if(adec_ptr->length_not_read != 0) {
        adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;
        audfadec_last_cb       = adec_ptr->handle->data_func;
        audmain_get_adec_data(adec_ptr->handle, adec_ptr->file_location,
                              &(adec_ptr->track_buf));
      }
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_wa_out_qcp

DESCRIPTION
  This function will initiate the playback of a QCP file embedded in
  a WebAudio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
audmain_status_type audfadec_wa_out_qcp (
  snd_af_access_type          *handle,
  uint32                      channel,
  audfadec_wa_info_type       *info,
  audmain_parse_ctl_type      *parse_ctl,
  boolean                     do_offset
) {
  uint32 length = 0, tlong;
  audfadec_track_type *adec_ptr;

  if((channel < AUDFADEC_NUM_TRACKS) && 
#ifdef FEATURE_GASYNTH_INTERFACE
     ((info->adec_mode != AUDMAIN_ADEC_MODE_AAC) ||
      (audmain_get_adec_mode() == AUDMAIN_ADEC_MODE_AAC))
#else
     (audmain_get_adec_mode() == info->adec_mode)
#endif /* FEATURE_GASYNTH_INTERFACE */
#ifndef FEATURE_QSYNTH_ADPCM
     && (info->adec_mode != AUDMAIN_ADEC_MODE_ADPCM) 
     && (info->adec_mode != AUDMAIN_ADEC_MODE_YADPCM) 
#endif 
    ) {
    audfadec_parse_ctl.main_parse = parse_ctl;
    adec_ptr                      = &audfadec_parse_ctl.tracks[channel];

    if(adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) {
      adec_ptr->wa_reserve.handle          = handle;
      adec_ptr->wa_reserve.block_start     = info->block_start;
      adec_ptr->wa_reserve.length_not_read = info->length;
      adec_ptr->wa_reserve.file_location   = info->location;
      adec_ptr->wa_reserve.status_flag     = AUDFADEC_MASK_WA_PEND;

#ifdef FEATURE_QSYNTH_ADPCM
      if (info->adec_mode == AUDMAIN_ADEC_MODE_ADPCM) {
        adec_ptr->wa_reserve.status_flag     |= AUDFADEC_MASK_ADPCM;
        if(info->mfi_flag) {
          adec_ptr->wa_reserve.status_flag   |= AUDFADEC_MASK_MFI;
          if(info->mfi_continue) {
            adec_ptr->wa_reserve.status_flag |= AUDFADEC_MASK_MFI_CONT;
          }
          adec_ptr->wa_reserve.sample_rate_hz  = info->sample_rate_hz;
          adec_ptr->wa_reserve.bits_per_sample = info->bits_per_sample;
          adec_ptr->wa_reserve.channels        = info->channels;
          adec_ptr->wa_reserve.frame_size      = info->frame_size;
        }
      } else if (info->adec_mode == AUDMAIN_ADEC_MODE_YADPCM) {
        adec_ptr->wa_reserve.status_flag    |= AUDFADEC_MASK_YADPCM |
                                               AUDFADEC_MASK_ADPCM;
        adec_ptr->wa_reserve.sample_rate_hz  = info->sample_rate_hz;
        adec_ptr->wa_reserve.bits_per_sample = info->bits_per_sample;
        adec_ptr->wa_reserve.channels        = info->channels;
        adec_ptr->wa_reserve.yadpcm_mode     = info->yadpcm_mode;
      }
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_ANIMATED_AAC
      if(info->adec_mode == AUDMAIN_ADEC_MODE_AAC) {
        adec_ptr->wa_reserve.status_flag |= AUDFADEC_MASK_AAC; 
      }
#endif

      if(!do_offset) {
        adec_ptr->wa_reserve.next_block     = info->next_block;
        if(info->next_block != 0) {
          adec_ptr->wa_reserve.status_flag |= AUDFADEC_MASK_NEXT_FLAG;
        }
      } else {
        adec_ptr->wa_reserve.status_flag |= AUDFADEC_MASK_DO_OFFSET;
        adec_ptr->wa_reserve.next_block   = 0;
      }
    } else {
      adec_ptr->status_flag            &= ~AUDFADEC_MASK_WA_PEND;
      adec_ptr->wa_reserve.status_flag &= ~AUDFADEC_MASK_WA_PEND;
      audfadec_reset(channel, adec_ptr );

      adec_ptr->handle      = handle;
      adec_ptr->block_start = info->block_start;

#ifdef FEATURE_QSYNTH_ADPCM
      if (info->adec_mode == AUDMAIN_ADEC_MODE_ADPCM) {
        adec_ptr->status_flag    |= AUDFADEC_MASK_ADPCM;
        if(info->mfi_flag) {
          adec_ptr->status_flag   |= AUDFADEC_MASK_MFI;
          if(info->mfi_continue) {
            adec_ptr->status_flag |= AUDFADEC_MASK_MFI_CONT;
          }
          adec_ptr->sample_rate_hz  = info->sample_rate_hz;
          adec_ptr->bits_per_sample = info->bits_per_sample;
          adec_ptr->channels        = info->channels;
          adec_ptr->frame_size      = info->frame_size;
        }
      } else if (info->adec_mode == AUDMAIN_ADEC_MODE_YADPCM) {
        adec_ptr->status_flag    |= AUDFADEC_MASK_YADPCM |
                                    AUDFADEC_MASK_ADPCM;
        adec_ptr->sample_rate_hz  = info->sample_rate_hz;
        adec_ptr->bits_per_sample = info->bits_per_sample;
        adec_ptr->channels        = info->channels;
        adec_ptr->yadpcm_mode     = info->yadpcm_mode;
      }
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_ANIMATED_AAC
      if(info->adec_mode == AUDMAIN_ADEC_MODE_AAC) {
        adec_ptr->status_flag |= AUDFADEC_MASK_AAC;
      }
#endif

      if(!do_offset) {
        if(info->next_block != 0) {
          adec_ptr->next_block   = info->next_block;
          adec_ptr->status_flag |= AUDFADEC_MASK_NEXT_FLAG;
        }

        if((info->buf1 != NULL) && (info->length_buf1 != 0)) {
          tlong   = info->length_buf1;
          (void) audfadec_copy(adec_ptr, info->buf1, &tlong);
          length  = info->length_buf1 - tlong;
        }

        if((info->buf2 != NULL) && (info->length_buf2 != 0)) {
          tlong   = info->length_buf2;
          (void) audfadec_copy(adec_ptr, info->buf2, &tlong);
          length += info->length_buf2 - tlong;
        }

        adec_ptr->length_not_read = info->length - length;
        adec_ptr->file_location   = info->location + length;
        adec_ptr->buffer_orig     = length;

      } else {
        adec_ptr->status_flag     |= AUDFADEC_MASK_DO_OFFSET;
        adec_ptr->length_not_read  = info->length;
        adec_ptr->file_location    = info->location;
        adec_ptr->buffer_orig      = 0;
      }

      adec_ptr->status_flag |= AUDFADEC_MASK_WA_PEND | AUDFADEC_MASK_WA_ADEC;
    }
    return (AUDMAIN_STATUS_SUCCESS);
  }

  return(AUDMAIN_STATUS_ERROR);
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION audfadec_midi_out_qcp_flag

DESCRIPTION
  This function indicates whether any of the current audio decoder parses
  is from a MIDI out command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfadec_midi_out_qcp_flag (
  audmain_parse_ctl_type      *parse_ctl
) {
  boolean ret_val = FALSE;
  uint32 counter;

  audfadec_parse_ctl_type *adec_parse;
  audfadec_track_type     *adec_ptr;

  if((parse_ctl != NULL) && (parse_ctl->audfadec_tracks != NULL)) {
    adec_parse = (audfadec_parse_ctl_type *) (parse_ctl->audfadec_tracks);
    for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter++) {
      adec_ptr   = &(adec_parse->tracks[counter]);

      if((adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) &&
         !(adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC)) {
        ret_val = TRUE;
        break;
      }
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_midi_out_qcp_channel_flag

DESCRIPTION
  This function indicates whether the indicated channel is from a MIDI
  out command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audfadec_midi_out_qcp_channel_flag (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 channel
) {
  boolean ret_val = FALSE;

  audfadec_parse_ctl_type *adec_parse;
  audfadec_track_type     *adec_ptr;

  if((channel < AUDFADEC_NUM_TRACKS) && (parse_ctl != NULL) &&
     (parse_ctl->audfadec_tracks != NULL)) {
    adec_parse = (audfadec_parse_ctl_type *) (parse_ctl->audfadec_tracks);
    adec_ptr   = &(adec_parse->tracks[channel]);

    ret_val = ((adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) &&
               !(adec_ptr->status_flag & AUDFADEC_MASK_WA_ADEC));
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_reset_midi_out_qcp

DESCRIPTION
  This function resets QCP file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfadec_reset_midi_out_qcp (
  audmain_parse_ctl_type  *parse_ctl
) {
  boolean adec_done = TRUE;
  uint32  status;
  int     counter;

  for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter++) {
    status = audfadec_parse_ctl.tracks[counter].status_flag;

    if(status & AUDFADEC_MASK_ADEC_ENABLED) {
      if(!(status & AUDFADEC_MASK_WA_ADEC)) {
        audfadec_reset(counter, &audfadec_parse_ctl.tracks[counter] );
        MSG_MED("midi_out_qcp reset", 0, 0, 0);
      } else {
        adec_done = FALSE;
      }
    }
  }
  if(adec_done) {
    parse_ctl->audfadec_tracks  = NULL;
    parse_ctl->audfadec_enabled = FALSE;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_pause_midi_out_qcp

DESCRIPTION
  This function pauses QCP file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_pause_midi_out_qcp (
  audmain_parse_ctl_type  *parse_ctl,
  boolean                 pause_flag
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;
  uint32              status;
  int                 counter;

  for(counter = 0; counter < AUDFADEC_NUM_TRACKS; counter++) {
    status = audfadec_parse_ctl.tracks[counter].status_flag;

    if(status & AUDFADEC_MASK_ADEC_ENABLED) {
      if(!(status & AUDFADEC_MASK_WA_ADEC)) {
        audmain_audfmt_adec_chan_pause(counter, pause_flag);
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_start_channel

DESCRIPTION
  This function starts the indicated adec file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_start_channel (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  channel
) {
  audfadec_parse_ctl_type *adec_parse;
  audfadec_track_type     *adec_ptr;
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;

  if((parse_ctl != NULL) && (parse_ctl->audfadec_tracks != NULL) &&
     (channel < AUDFADEC_NUM_TRACKS)) {
    adec_parse = (audfadec_parse_ctl_type *) (parse_ctl->audfadec_tracks);
    adec_ptr   = &(adec_parse->tracks[channel]);

    if((adec_ptr->status_flag & AUDFADEC_MASK_ADEC_ENABLED) &&
       (adec_ptr->status_flag & AUDFADEC_MASK_DELAYED_START)) {
      ret_val = audmain_audfmt_adec_chan_start(channel);
      if(ret_val == AUDMAIN_STATUS_SUCCESS) {
        adec_ptr->status_flag &= ~AUDFADEC_MASK_DELAYED_START;
        if((adec_ptr->handle != NULL) &&
           (adec_ptr->handle->data_func != NULL)) {
          adec_ptr->handle->data_func(SND_CMX_MIDI_OUT_ADEC_START_CMD,
                                      adec_ptr->handle->audfmt_ptr, 0, NULL);
        }
      }
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_midi_out_qcp

DESCRIPTION
  This function will initiate the playback of a QCP file with the QSynth
  DSP image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
audmain_status_type audfadec_midi_out_qcp (
  snd_af_access_type               *handle,
  uint32                           channel,
  audmain_parse_ctl_type           *parse_ctl,
  audmain_midi_out_adec_param_type *param
) {
  audfadec_track_type *adec_ptr;

  if((handle != NULL) && (channel < AUDFADEC_NUM_TRACKS) &&
     (parse_ctl != NULL)) {
    adec_ptr = &audfadec_parse_ctl.tracks[channel];

    parse_ctl->audfadec_tracks  = &audfadec_parse_ctl;
    parse_ctl->audfadec_enabled = TRUE;

    audfadec_reset_check_status(channel, adec_ptr );

    adec_ptr->handle        = handle;
    adec_ptr->file_location = 0;

    if(param->start_mode == AUDMAIN_ADEC_START_MODE_DELAYED) {
      adec_ptr->status_flag |= AUDFADEC_MASK_DELAYED_START;
    }

    if(param->repeat_mode == AUDMAIN_ADEC_REPEAT_MODE_ENABLED) {
      adec_ptr->status_flag |= AUDFADEC_MASK_REPEAT_ENABLED;
      adec_ptr->repeat_num   = param->repeat_num;
    }

    if(handle->data_func != NULL) {
      handle->data_func(SND_CMX_AF_GET_FILE_LEN, handle->audfmt_ptr, 0,
                        (void *) &adec_ptr->length_not_read);
    }

    adec_ptr->status_flag |= AUDFADEC_MASK_BUF_PENDING;

    audfadec_last_cb = handle->data_func;
    (void) audmain_get_adec_data(adec_ptr->handle, 0, &(adec_ptr->track_buf));

    adec_ptr->status_flag |= AUDFADEC_MASK_ADEC_ENABLED;

    return (AUDMAIN_STATUS_SUCCESS);
  }
  return(AUDMAIN_STATUS_ERROR);
}
#endif /* FEATURE_MIDI_OUT_QCP */

#endif /* defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) */
