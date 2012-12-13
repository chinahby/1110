/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("ADPCM Module")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            A D P C M    A U D I O   F O R M A T s   M O D U L E    

GENERAL DESCRIPTION
  These functions implement a ADPCM file format player.

EXTERNALIZED FUNCTIONS
  audadpcm_get_data_cb_req
    Handles the get/read data callback.
  audadpcm_parse_head
    Parses the head of a ADPCM file.
  audadpcm_age
    Ages the specified ADPCM file by a number of milliseconds.
  audadpcm_parse
    Parses one event of the indicated ADPCM file.
  audadpcm_adec_adjust
    This function adjust the buffer status.
  audadpcm_reset
    This function reset adpcm status.
  audadpcm_get_sample_rate
    This function returns the sampling rate of qsynth image.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2003 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audadpcm.c#10 $ $DateTime: 2010/09/07 05:27:19 $ $Author: rpampana $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/07/10    rp     fix to handle the corrupted adpcm fmt data chunck size . 
01/18/10   aim     Fixed the compiler warnings. 
10/23/09   sk/gs   Modified the code to return error if
                   audadpcm_parse_ctl.format.bytes_rate is zero.
10/14/09    bk     Changed the way of calculation in audadpcm_set_data_rate
                   for very low sample rate files. And made the minimum allowed
                   sampling rate for wav file to be 100 Hz.
10/01/09    bk     Supporting all non zero and odd native sample rates upto 48K
                          in wave parser for Linear PCM files.
09/22/09    bk     Modified the wave file parser such that if header contains
                   zero sample rate then parser should return error in order to
                   avoid further processing.
09/18/09    bk     Modified the parser such that if no data chunk is available
                   in wave file then the parser should return gracefully.
07/29/09    bk     Returning error for linear PCM files for build with defined 
                   FEATURE_STD_MIDI.
05/06/09    gs     Returning data-error for unsupported adpcm formats in
                   audadpcm_parse_head()
12/27/07   wfd     Add ADPCM file header parsing to audadpcm_is_supported().
07/10/07    st     Modified code to support an additional reserved data buffer. 
                   Modified code to double buffer size for high bitrate raw
                   PCM audio playback
04/12/07    ss     Fixed CR_116022 Modified audadpcm_parse_head() and 
                   audadpcm_format_play() to request at least 1000 byte worth
                   of buffer. 
01/29/07    sk     Added Odd sampling rate support for LinerPCM CR 101068. 
01/18/07    ss     Added support to play fractional sample rate wave bitstream.
08/08/06    st     Updated audadpcm_set_data_rate() to support a-law and
                   mu-law playback, CR98411.
06/26/06    pl     Modified various functions to adopt new ping pong buffer
                   data writing scheme. 
05/02/06    hz     Added fix for CR89264.
03/13/06    st     Modified call to audmain_adpcm_start() to support new
                   interface; modified calculation of frame size for ADPCM
                   to match DSP calculation.
02/21/06    hz     Fixed featurization in audadpcm_parse_head.
01/13/06   act     Fixed compile warning with low-tier featureset.
09/18/05    pl     Modified code to calculate ADPCM frame size for new audio
                   decoder API.
08/02/05    st     Modified audadpcm_parse_head() to properly handle large
                   initial buffers returned from client.
07/26/05    sm     Added format_play() support for wav a/ulaw files.
07/22/05    sm     Added WAV A/Ulaw support.
07/19/05    pl     Added the support for setting the error status at the point 
                   where the error is encountered. The error status would 
                   eventually be relayed back to the upper layer to provide
                   more information for the aborted playback.
07/08/05    pl     Modified audadpcm_do_file_spec() to set supported operation 
                   bit mask.
06/03/05    st     Modified code to support sending a 1 second tick callback
                   to client for PCM recording.
04/19/05    aw     Added support for 48Khz PCM playback.
01/25/05    st     Modified time calculation to account for 32bit overflow.
08/26/04    st     Modified code to calculate the elapsed time for rewind/
                   fast-forward command based on data rate.
08/11/04    st     Modified code to handle playback of misaligned buffers
                   properly.
03/22/04    st     Modified code to support PCM playback and recording;
                   modified code to not perform external operations if only
                   doing calc_time or get_specs function; modified code to
                   remove use of scratch buffer and use buffers from client;
                   modified code to do audadpcm_get_data() after doing adjust.
                   Removed compiler warning.
01/22/04    lg     Added get_time functionality.
12/02/03    sg     Added support for linear PCM playback.
11/20/03    aw     Modified code to set 32KHz output sampling rate qsynth 
                   hybrid mode.
10/16/03  aw/st    Modified code to pass ADPCM mode to audmain_adpcm_start().
03/10/03    st     Modified code to fail playback if zero data bytes is
                   received; removed use of AUDADPCM_MASK_BUF2_PENDING.
02/19/03    aw     Create file.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"
#include "msg.h"
#if defined(FEATURE_WAVE_PB) || defined(FEATURE_PCM_REC)
#include "audadpcm.h"           /* Audio Format definitions and prototypes */
#include "audadpcmi.h"          /* Internal Audio Format definitions       */
#include "audmain.h"            /* Audio Format definitions and prototypes */
#include <string.h>             /* String routines                         */
#endif /* FEATURE_WAVE_PB || FEATURE_PCM_REC */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data.  There is no global data.

===========================================================================*/

#ifdef FEATURE_WAVE_PB
/* This structure contains all the information about a file
** that is being parsed. This module currently only supports parsing
** one file at a time.
*/
audadpcm_parse_ctl_type audadpcm_parse_ctl;

/* Maximum unsigned 32-bit value; mainly used to indicate that length of
** PCM file is indeterminant.
*/
#define AUDADPCM_MAX_32BIT           ((unsigned) 0xFFFFFFFF)
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_PCM_REC
/* These structures contain all the information about a file
** that is being recorded.
*/
audpcm_rec_ctl_type audpcm_rec_reverse_ctl;
audpcm_rec_ctl_type audpcm_rec_forward_ctl;

/* Header for PCM files */
const uint8 audpcm_header[] = {
  'R', 'I', 'F', 'F',
  0x00, 0x00, 0x00, 0x00,   /* Offset 4: file length, to be filled in    */
  'W', 'A', 'V', 'E',
  'f', 'm', 't', ' ',
  0x10, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x01, 0x00,
  0x40, 0x1f, 0x00, 0x00,
  0x80, 0x3e, 0x00, 0x00,
  0x02, 0x00, 0x10, 0x00,
  'd', 'a', 't', 'a',
  0x00, 0x00, 0x00, 0x00
};
#endif /* FEATURE_PCM_REC */

/* <EJECT> */
#ifdef FEATURE_WAVE_PB
/*===========================================================================

FUNCTION audadpcm_get_data_cb_req

DESCRIPTION
  This function handles the get data callback.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -
  FALSE -

SIDE EFFECTS
  audadpcm_parse_ctl

===========================================================================*/
boolean audadpcm_get_data_cb_req (
  uint8 **data,
  uint8 *buf,
  uint32 length,
  uint32 pos
) {
  audadpcm_track_ctl_type *track_ptr, *track_ptr2;
  boolean                 ret_val = FALSE;
  uint32                  len, index;

  if((data == &audadpcm_parse_ctl.track1.buffer) ||
     (data == &audadpcm_parse_ctl.track2.buffer) ||
     (data == &audadpcm_parse_ctl.reserve.buffer)) {

    if (buf == NULL) {
      if (audadpcm_parse_ctl.data_length != AUDADPCM_MAX_32BIT) {
        audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_FAILURE;
        audadpcm_parse_ctl.error_status = AUDMAIN_STATUS_DATA_ACCESS_ERR;
      } else {
        audadpcm_parse_ctl.data_length = 0;
      }
    } else if (length == 0) {
  
      audmain_do_play_cb(audadpcm_parse_ctl.handle, SND_CMX_AF_FREE_VAR_BUF,
                         0, (void *) buf);

      /* Since the buffer is freed up, pending mark should be cleared */
      if (audadpcm_parse_ctl.status_flag & AUDADPCM_MASK_BUF_PENDING) {
        audadpcm_parse_ctl.status_flag &= ~AUDADPCM_MASK_BUF_PENDING;
      }
    } else {
      if(audadpcm_parse_ctl.status_flag & AUDADPCM_MASK_REPOSITION) {
        audmain_do_play_cb(audadpcm_parse_ctl.handle, SND_CMX_AF_FREE_VAR_BUF,
                           0, (void *) buf);

        audadpcm_parse_ctl.status_flag &= ~AUDADPCM_MASK_REPOSITION;

        if(audadpcm_parse_ctl.data_length != 0) {
          audmain_get_var_data(audadpcm_parse_ctl.handle,
                               audadpcm_parse_ctl.file_location,
                               audadpcm_parse_ctl.block_length,
                               audadpcm_parse_ctl.frame_size,
                               &(audadpcm_parse_ctl.track1.buffer));
        } else {
          audadpcm_parse_ctl.status_flag   &= ~AUDADPCM_MASK_BUF_PENDING;
        }
      } else {
        if(data == &audadpcm_parse_ctl.track1.buffer) {
          track_ptr = &audadpcm_parse_ctl.track1;
        } else if(data == &audadpcm_parse_ctl.track2.buffer) {
          track_ptr = &audadpcm_parse_ctl.track2;
        } else {
          track_ptr = &audadpcm_parse_ctl.reserve;
        }
        if(audadpcm_parse_ctl.data_length != AUDADPCM_MAX_32BIT) {
          len = MIN(audadpcm_parse_ctl.data_length, length);
          audadpcm_parse_ctl.data_length   -= len;
        } else {
          len = length;
        }
        if(audadpcm_parse_ctl.status_flag & AUDADPCM_MASK_COPY_PENDING) {
          if((audadpcm_parse_ctl.track1.length <= audadpcm_parse_ctl.frame_size)
             && (track_ptr == &audadpcm_parse_ctl.track2)) {
            track_ptr2 = &audadpcm_parse_ctl.track1;

            index = audadpcm_parse_ctl.frame_size - track_ptr2->length;
            memcpy(&buf[index], &track_ptr2->buffer[track_ptr2->index],
                   track_ptr2->length);

            track_ptr->buffer  = buf;
            track_ptr->index   = index;
            track_ptr->length  = len + audadpcm_parse_ctl.track1.length;

            audmain_do_play_cb(audadpcm_parse_ctl.handle,
                               SND_CMX_AF_FREE_VAR_BUF, 0,
                               (void *) track_ptr2->buffer);
            track_ptr2->buffer = NULL;
            track_ptr2->length = 0;

            audadpcm_parse_ctl.track_status  = AUDADPCM_STATUS_TRACK2;
            audadpcm_parse_ctl.status_flag  &= ~AUDADPCM_MASK_COPY_PENDING;
          } else {
            audmain_do_play_cb(audadpcm_parse_ctl.handle,
                               SND_CMX_AF_FREE_VAR_BUF, 0, (void *) buf);
            audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_FAILURE;
            audadpcm_parse_ctl.error_status = AUDMAIN_STATUS_RESOURCE_ERR;
          }
        } else {
          track_ptr->buffer  = buf;
          track_ptr->index   = audadpcm_parse_ctl.frame_size;
          track_ptr->length  = len;
        }
    
        audadpcm_parse_ctl.file_location += len;
        audadpcm_parse_ctl.status_flag   &= ~AUDADPCM_MASK_BUF_PENDING;
      }
    }
    ret_val = TRUE;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_get_data

DESCRIPTION
  This function tries to retrieve the next data buffer.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if things have been set up to get data

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audadpcm_get_data (
  audadpcm_parse_ctl_type   *adpcm_ctl
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

#ifdef FEATURE_GASYNTH_INTERFACE 
  if((adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM) ||
     (adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_LINEAR_PCM)) {
    if(adpcm_ctl->track1.length == 0) {
      if(adpcm_ctl->track1.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->track1.buffer);
        adpcm_ctl->track1.buffer = NULL;
        adpcm_ctl->track_inuse &= ~AUDADPCM_STATUS_TRACK1;
      }
      if(adpcm_ctl->track1.buffer == NULL) {
        if(adpcm_ctl->reserve.buffer != NULL) {
          adpcm_ctl->track1 = adpcm_ctl->reserve;
          adpcm_ctl->reserve.buffer = NULL;
          adpcm_ctl->reserve.length = 0;
          ret_val = AUDMAIN_STATUS_PENDING;
        } else if((adpcm_ctl->data_length != 0) &&
                  (!(adpcm_ctl->status_flag & AUDADPCM_MASK_BUF_PENDING))) {
          adpcm_ctl->status_flag |= AUDADPCM_MASK_BUF_PENDING;
          audmain_get_var_data(adpcm_ctl->handle, adpcm_ctl->file_location,
                               adpcm_ctl->block_length, adpcm_ctl->frame_size,
                               &(adpcm_ctl->track1.buffer));
          MSG_MED("Request track1 buffer", 0, 0, 0);
        }
      }
    }
    
    if(adpcm_ctl->track2.length == 0) {
      if(adpcm_ctl->track2.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->track2.buffer);
        adpcm_ctl->track2.buffer = NULL;
        adpcm_ctl->track_inuse &= ~AUDADPCM_STATUS_TRACK2;
      }
      if(adpcm_ctl->track2.buffer == NULL) {
        if(adpcm_ctl->reserve.buffer != NULL) {
          adpcm_ctl->track2 = adpcm_ctl->reserve;
          adpcm_ctl->reserve.buffer = NULL;
          adpcm_ctl->reserve.length = 0;
          ret_val = AUDMAIN_STATUS_PENDING;
        } else if((adpcm_ctl->data_length != 0) &&
                  (!(adpcm_ctl->status_flag & AUDADPCM_MASK_BUF_PENDING))) {
          adpcm_ctl->status_flag |= AUDADPCM_MASK_BUF_PENDING;
          audmain_get_var_data(adpcm_ctl->handle, adpcm_ctl->file_location,
                               adpcm_ctl->block_length, adpcm_ctl->frame_size,
                               &(adpcm_ctl->track2.buffer));
          MSG_MED("Request track2 buffer" ,0 ,0 ,0);
        }
      }
    }

    if((adpcm_ctl->status_flag & AUDADPCM_MASK_USE_RESERVE) &&
       (adpcm_ctl->reserve.buffer == NULL) &&
       (adpcm_ctl->data_length != 0) &&
       (!(adpcm_ctl->status_flag & AUDADPCM_MASK_BUF_PENDING))) {
      adpcm_ctl->status_flag |= AUDADPCM_MASK_BUF_PENDING;
      audmain_get_var_data(adpcm_ctl->handle, adpcm_ctl->file_location,
                           adpcm_ctl->block_length, adpcm_ctl->frame_size,
                           &(adpcm_ctl->reserve.buffer));
    }
  } else 
#endif /* FEATURE_GASYNTH_INTERFACE */
  if (adpcm_ctl->track1.length == 0) {
    if ((adpcm_ctl->track1.buffer != NULL) &&
        (adpcm_ctl->track_inuse != AUDADPCM_STATUS_TRACK1)) {
      audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                         adpcm_ctl->track1.buffer);
      adpcm_ctl->track1.buffer = NULL;
    }
    if ((adpcm_ctl->data_length != 0) &&
        (adpcm_ctl->track1.buffer == NULL) &&
        (!(adpcm_ctl->status_flag & AUDADPCM_MASK_BUF_PENDING))) {
      adpcm_ctl->status_flag |= AUDADPCM_MASK_BUF_PENDING;
      audmain_get_var_data(adpcm_ctl->handle, adpcm_ctl->file_location,
                           adpcm_ctl->block_length, adpcm_ctl->frame_size,
                           &(adpcm_ctl->track1.buffer));
    }
  }

  if (adpcm_ctl->track2.length == 0) {
    if ((adpcm_ctl->track2.buffer != NULL) &&
        (adpcm_ctl->track_inuse != AUDADPCM_STATUS_TRACK2)) {
      audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                         adpcm_ctl->track2.buffer);
      adpcm_ctl->track2.buffer = NULL;
    }
    if ((adpcm_ctl->data_length != 0) &&
        (adpcm_ctl->track2.buffer == NULL) &&
        (!(adpcm_ctl->status_flag & AUDADPCM_MASK_BUF_PENDING))) {
      adpcm_ctl->status_flag |= AUDADPCM_MASK_BUF_PENDING;
      audmain_get_var_data(adpcm_ctl->handle, adpcm_ctl->file_location,
                           adpcm_ctl->block_length, adpcm_ctl->frame_size,
                           &(adpcm_ctl->track2.buffer));
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_do_file_spec

DESCRIPTION
  This function builds a sound file specifications buffer and transmits the
  data to the CMX layer through the playback callback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audadpcm_do_file_spec (
  snd_af_access_type          *handle,
  audadpcm_parse_ctl_type     *adpcm_ctl
)
{
  snd_wave_spec_type snd_spec;

  switch(adpcm_ctl->format.wave_format) {
    case QCPSUP_WAVE_FORMAT_IMA_ADPCM:
      snd_spec.file_type      = SND_CMX_AF_FILE_ADPCM;
      snd_spec.supported_ops  = SND_SUPPORTED_OPS_MASK_GETTIME;

      break;
    case QCPSUP_WAVE_FORMAT_LINEAR_PCM:
      snd_spec.file_type      = SND_CMX_AF_FILE_PCM;
      snd_spec.supported_ops  = (snd_supported_ops_mask_type)(SND_SUPPORTED_OPS_MASK_SEEK |
                                 SND_SUPPORTED_OPS_MASK_GETTIME);
      break;
#ifdef FEATURE_VOC_G711
    case QCPSUP_WAVE_FORMAT_ALAW:
      snd_spec.file_type      = SND_CMX_AF_FILE_WAV_ALAW;
      snd_spec.supported_ops  = (snd_supported_ops_mask_type)(SND_SUPPORTED_OPS_MASK_SEEK |
                                 SND_SUPPORTED_OPS_MASK_GETTIME);

      break;
    case QCPSUP_WAVE_FORMAT_MULAW:
      snd_spec.file_type      = SND_CMX_AF_FILE_WAV_MULAW;
      snd_spec.supported_ops  = (snd_supported_ops_mask_type)(SND_SUPPORTED_OPS_MASK_SEEK |
                                 SND_SUPPORTED_OPS_MASK_GETTIME);
      break;
#endif /* FEATURE_VOC_G711 */
  }
  snd_spec.channels         = adpcm_ctl->format.channels;
  snd_spec.sample_rate      = adpcm_ctl->format.sample_rate;
  snd_spec.bytes_rate       = adpcm_ctl->format.bytes_rate;
  snd_spec.frame_size       = adpcm_ctl->format.frame_size;
  snd_spec.bits_per_sample  = adpcm_ctl->format.bits_per_sample;

  audmain_do_play_cb( handle, SND_CMX_AF_AUDIO_SPEC, 0, (void *)&snd_spec );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_set_data_rate

DESCRIPTION
  Sets parameters used in rewind/fast-forward.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audadpcm_set_data_rate (
  audadpcm_parse_ctl_type     *adpcm_ctl
) {
#if defined(FEATURE_PCM) || defined(FEATURE_VOC_G711)
  uint32 tlong;

  switch(adpcm_ctl->format.wave_format) {
#ifdef FEATURE_PCM
    case QCPSUP_WAVE_FORMAT_LINEAR_PCM:
#endif /* FEATURE_PCM */
#ifdef FEATURE_VOC_G711
    case QCPSUP_WAVE_FORMAT_ALAW:
    case QCPSUP_WAVE_FORMAT_MULAW:
#endif /* FEATURE_VOC_G711 */
      tlong = (adpcm_ctl->format.sample_rate * adpcm_ctl->format.channels *
               adpcm_ctl->format.bits_per_sample) >> 3;

      switch(tlong) {
        case 11025:
          adpcm_ctl->data_rate          = 441;
          adpcm_ctl->rate_offset        = 40;
          adpcm_ctl->overflow_offset    = (AUDADPCM_MAX_32BIT/441)*40;
          adpcm_ctl->overflow_rate      = AUDADPCM_MAX_32BIT/441;
          adpcm_ctl->overflow_offset_ms = AUDADPCM_MAX_32BIT/40;
          break;

        case 22050:
          adpcm_ctl->data_rate          = 441;
          adpcm_ctl->rate_offset        = 20;
          adpcm_ctl->overflow_offset    = (AUDADPCM_MAX_32BIT/441)*20;
          adpcm_ctl->overflow_rate      = AUDADPCM_MAX_32BIT/441;
          adpcm_ctl->overflow_offset_ms = AUDADPCM_MAX_32BIT/20;
          break;

        case 44100:
          adpcm_ctl->data_rate          = 441;
          adpcm_ctl->rate_offset        = 10;
          adpcm_ctl->overflow_offset    = (AUDADPCM_MAX_32BIT/441)*10;
          adpcm_ctl->overflow_rate      = AUDADPCM_MAX_32BIT/441;
          adpcm_ctl->overflow_offset_ms = AUDADPCM_MAX_32BIT/10;
          break;

        case 88200:
          adpcm_ctl->data_rate          = 441;
          adpcm_ctl->rate_offset        = 5;
          adpcm_ctl->overflow_offset    = (AUDADPCM_MAX_32BIT/441)*5;
          adpcm_ctl->overflow_rate      = AUDADPCM_MAX_32BIT/441;
          adpcm_ctl->overflow_offset_ms = AUDADPCM_MAX_32BIT/5;
          break;

        case 176400:
          adpcm_ctl->data_rate          = 882;
          adpcm_ctl->rate_offset        = 5;
          adpcm_ctl->overflow_offset    = (AUDADPCM_MAX_32BIT/882)*5;
          adpcm_ctl->overflow_rate      = AUDADPCM_MAX_32BIT/882;
          adpcm_ctl->overflow_offset_ms = AUDADPCM_MAX_32BIT/5;
          break;

        default:
         if(adpcm_ctl->format.sample_rate >= 1000){
          adpcm_ctl->data_rate       = tlong/1000;
	   adpcm_ctl->rate_offset     = 1;
	   adpcm_ctl->overflow_offset = AUDADPCM_MAX_32BIT/adpcm_ctl->data_rate;
	   adpcm_ctl->overflow_rate   = adpcm_ctl->overflow_offset;
	   adpcm_ctl->overflow_offset_ms = AUDADPCM_MAX_32BIT;
	  } else {
	   adpcm_ctl->data_rate           = tlong/10;
	   adpcm_ctl->rate_offset          = 100;
           adpcm_ctl->overflow_offset   = AUDADPCM_MAX_32BIT;
           adpcm_ctl->overflow_rate     = (AUDADPCM_MAX_32BIT/adpcm_ctl->data_rate);
	   adpcm_ctl->overflow_offset_ms = (AUDADPCM_MAX_32BIT/adpcm_ctl->rate_offset);
	  }
          break;
      }
      break;
  }
#endif /* FEATURE_PCM || FEATURE_VOC_G711 */
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_is_supported

DESCRIPTION
  Verifies if the file type is supported.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if supported; FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audadpcm_is_supported (
  qcpsup_adpcm_format_type *format
)
{
  boolean status = FALSE;

#ifdef FEATURE_STD_MIDI
/* supported adpcm file types are 8K,16K and 32K sampled, with 4bps or
   8bps with mono channel only
   Note that Linear PCM files are not supported under this feature*/
#ifdef FEATURE_ADPCM
  if (format->wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM) {
	/* 3-bits per sample not supported */
	if (((format->bits_per_sample == 4)||(format->bits_per_sample == 8))&& 
	    (format->channels == 1) &&     
	    ((format->sample_rate == 8000) || (format->sample_rate == 16000)||
	    (format->sample_rate == 32000) )  
	    ) 
            {
                status = TRUE;
	    }
       }
#endif   /* FEATURE_ADPCM */

#else   /*FEATURE_STD_MIDI*/

#ifdef FEATURE_ADPCM
  if (format->wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM) {
	/* 3-bits per sample not supported */
	if (( (format->bits_per_sample == 4)) &&
		((format->channels == 1) || (format->channels == 2)) &&
		((format->sample_rate >= 8000) && (format->sample_rate <= 48000))) {
      status = TRUE;
	}
  }
#endif /* FEATURE_ADPCM */

#ifdef FEATURE_PCM
  if (format->wave_format == QCPSUP_WAVE_FORMAT_LINEAR_PCM) {
#ifdef FEATURE_QSYNTH_PCM
    if (((format->bits_per_sample == 8) || (format->bits_per_sample == 16)) &&
        ((format->channels == 1) || (format->channels == 2)) && (
#ifndef FEATURE_GASYNTH_INTERFACE
        (format->sample_rate == 4000)  || (format->sample_rate == 8000)  ||
        (format->sample_rate == 11025) || (format->sample_rate == 12000) ||
        (format->sample_rate == 16000) || (format->sample_rate == 22050) ||
        (format->sample_rate == 24000) || (format->sample_rate == 32000) ||
        (format->sample_rate == 44100) || (format->sample_rate == 48000))) {
#else
        ((format->sample_rate >= 100) && (format->sample_rate <= 48000)))) {
#endif    /* FEATURE_GASYNTH_INTERFACE */    

      status = TRUE;
    }
#elif defined(FEATURE_QSYNTH_ADPCM)
    if (((format->bits_per_sample == 4) || (format->bits_per_sample == 8)) &&
        (format->channels == 1) &&
        ((format->sample_rate >=100)  || (format->sample_rate == 8000))) {
      status = TRUE;
    }
#endif /* FEATURE_QSYNTH_PCM */
  }
#endif /* FEATURE_PCM */

#ifdef FEATURE_VOC_G711
  if(format->wave_format == QCPSUP_WAVE_FORMAT_ALAW ||
     format->wave_format == QCPSUP_WAVE_FORMAT_MULAW) {
    if((format->bits_per_sample == 8) &&
       (format->channels == 1) &&
       (format->sample_rate == 8000)) {
      status = TRUE;
    }
  }
#endif /* FEATURE_VOC_G711 */

#endif  /*FEATURE_STD_MIDI*/

  return status;
}

/* <EJECT> */
#ifdef FEATURE_PCM
/*===========================================================================

FUNCTION audadpcm_format_play

DESCRIPTION
  This function sets up to play a PCM file with no header information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audadpcm_format_play (
  snd_cmx_format_spec_type *input_spec,
  uint8                    *buffer,
  snd_af_access_type       *handle,
  audmain_parse_ctl_type   *parse_ctl
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;
  uint32 tlong;

  if((input_spec != NULL) && (handle != NULL) && (parse_ctl != NULL) &&
     ((input_spec->wave_codec.file_type == SND_CMX_AF_FILE_PCM)
#ifdef FEATURE_VOC_G711
       || (input_spec->wave_codec.file_type == SND_CMX_AF_FILE_WAV_ALAW)
       || (input_spec->wave_codec.file_type == SND_CMX_AF_FILE_WAV_MULAW)
#endif /* FEATURE_VOC_G711 */
     )) {
    if(input_spec->wave_codec.sample_fmt == SND_WAVE_SAMPLE_FORMAT_SIGNED) {
      audadpcm_parse_ctl.signed_samples = TRUE;
    } else {
      audadpcm_parse_ctl.signed_samples = FALSE;
    }
    switch(input_spec->wave_codec.file_type) {
#ifdef FEATURE_VOC_G711
      case SND_CMX_AF_FILE_WAV_ALAW:
        audadpcm_parse_ctl.format.wave_format = QCPSUP_WAVE_FORMAT_ALAW;
        audadpcm_parse_ctl.format.frame_size = 1;
        /* Since we only support ALaw WAV at 8khz, we can fix the bytes_rate
        ** at 160 bytes/frame * 50 frames/sec = 8000 bytes/sec.
        */
        audadpcm_parse_ctl.format.bytes_rate = 8000;
        break;
      case SND_CMX_AF_FILE_WAV_MULAW:
        audadpcm_parse_ctl.format.wave_format = QCPSUP_WAVE_FORMAT_MULAW;
        audadpcm_parse_ctl.format.frame_size = 1;
        /* Since we only support ULaw WAV at 8khz, we can fix the bytes_rate
        ** at 160 bytes/frame * 50 frames/sec = 8000 bytes/sec.
        */
        audadpcm_parse_ctl.format.bytes_rate = 8000;
        break;
#endif /* FEATURE_VOC_G711 */
      case SND_CMX_AF_FILE_PCM:
      default:
        audadpcm_parse_ctl.format.wave_format = QCPSUP_WAVE_FORMAT_LINEAR_PCM;
        break;
    }
    audadpcm_parse_ctl.format.channels    = input_spec->wave_codec.channels;
    audadpcm_parse_ctl.format.sample_rate = input_spec->wave_codec.sample_rate;
    audadpcm_parse_ctl.format.bits_per_sample =
                                        input_spec->wave_codec.bits_per_sample;

    if (audadpcm_is_supported(&audadpcm_parse_ctl.format) == FALSE) {
      return(AUDMAIN_STATUS_ERROR);
    }

    tlong = ((audadpcm_parse_ctl.format.sample_rate *
              audadpcm_parse_ctl.format.bits_per_sample *
              audadpcm_parse_ctl.format.channels) >> 3) / 10;

    audadpcm_parse_ctl.frame_size = ((tlong / 20) + 2) << 1;

    tlong = ((tlong >> 2) + 1) << 1;
    if(tlong <= parse_ctl->buffer_size) {
      audadpcm_parse_ctl.block_length = parse_ctl->buffer_size;
    } else {
      audadpcm_parse_ctl.block_length  = tlong << 1;
      audadpcm_parse_ctl.status_flag  |= AUDADPCM_MASK_USE_RESERVE;
    }
    audadpcm_parse_ctl.block_length = MAX(1000, 
                                          audadpcm_parse_ctl.block_length);

    audadpcm_set_data_rate(&audadpcm_parse_ctl);

#ifdef FEATURE_QSYNTH_ADPCM
#ifdef FEATURE_QSYNTH_PCM
    if (audmain_set_adec_mode(AUDMAIN_ADEC_MODE_PCM) != AUDMAIN_STATUS_SUCCESS)
#else
    if (audmain_set_adec_mode(AUDMAIN_ADEC_MODE_YADPCM) !=
        AUDMAIN_STATUS_SUCCESS)
#endif
    {
      return (AUDMAIN_STATUS_ERROR);
    }
    audmain_set_hybrid_mode(AUDMAIN_AUDFMT_POLY_MODE_7);
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
    audmain_adec_reset_channel(parse_ctl, 0);
#endif
#endif /* FEATURE_QSYNTH_ADPCM */

    audadpcm_parse_ctl.track1.buffer  = buffer;
    audadpcm_parse_ctl.track2.buffer  = NULL;
    audadpcm_parse_ctl.reserve.buffer = NULL;
    audadpcm_parse_ctl.handle         = handle;

    audadpcm_parse_ctl.start_position = 0;
    audadpcm_parse_ctl.total_length   = AUDADPCM_MAX_32BIT;
    audadpcm_parse_ctl.data_length    = AUDADPCM_MAX_32BIT;

    audadpcm_parse_ctl.track_status   = AUDADPCM_STATUS_TRACK1;
    audadpcm_parse_ctl.track_inuse    = AUDADPCM_STATUS_NONE;
    parse_ctl->tracks = &audadpcm_parse_ctl;

    if(((uint32) audadpcm_parse_ctl.track1.buffer) & 0x1) {
      audadpcm_parse_ctl.file_location = 0;
      audadpcm_parse_ctl.track1.length = 0;

      audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0,
                         (void *) audadpcm_parse_ctl.track1.buffer);

      audadpcm_parse_ctl.track1.buffer = NULL;

      audmain_set_read_sync(FALSE);
      audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_BUF_PENDING |
                                        AUDADPCM_MASK_BUFF_ERR_SENT;

      audmain_get_var_data(handle, 0, audadpcm_parse_ctl.block_length,
                           audadpcm_parse_ctl.frame_size,
                           &(audadpcm_parse_ctl.track1.buffer));
    } else {
      audadpcm_parse_ctl.file_location  = parse_ctl->buffer_size;
      audadpcm_parse_ctl.track1.length  = parse_ctl->buffer_size;
      audadpcm_parse_ctl.track1.index   = 0;


      if(audadpcm_parse_ctl.track1.length <= audadpcm_parse_ctl.frame_size) {
        audmain_set_read_sync(FALSE);
        audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_BUF_PENDING |
                                          AUDADPCM_MASK_COPY_PENDING;
        audmain_get_var_data(handle, audadpcm_parse_ctl.file_location,
                             audadpcm_parse_ctl.block_length,
                             audadpcm_parse_ctl.frame_size,
                             &(audadpcm_parse_ctl.track2.buffer));
      }
    }

    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}
#endif /* FEATURE_PCM */

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_parse_head

DESCRIPTION
  This function parses and validates the head of a ADPCM file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if recognized as ADPCM file
  AUDMAIN_STATUS_ERROR   - if not recognized as ADPCM file or invalid

SIDE EFFECTS
  audadpcm_parse_ctl

===========================================================================*/
audmain_status_type audadpcm_parse_head (
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_play_param_type     *param,
  audmain_parse_ctl_type      *parse_ctl
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;
  uint64 tqword;
  uint32 tlong, index = 0, buffer_size;
  uint32 file_size, buffer_start = 0;
  uint8  riff_id[]  = {'W', 'A', 'V', 'E'};
  uint8  chunk_id[] = {'d', 'a', 't', 'a'};
  qcpsup_status_type status;
#if defined(FEATURE_QSYNTH_ADPCM) || defined(FEATURE_QSYNTH_PCM)
  audmain_adec_mode_enum_type adec_mode = AUDMAIN_ADEC_MODE_NONE;
#endif /* FEATURE_QSYNTH_ADPCM || FEATURE_QSYNTH_PCM */

  if(buffer != NULL) {
    if(qcpsup_do_riff(riff_id, buffer, &index, &file_size) ==
       QCPSUP_STATUS_SUCCESS) {
      while ((status = qcpsup_get_wave_fmt(buffer, &index,
                                            parse_ctl->buffer_size,
                                            &audadpcm_parse_ctl.format)
             ) == QCPSUP_STATUS_BUFF_ERROR) {
        if((index + buffer_start) >= file_size) {
          return(AUDMAIN_STATUS_ERROR);
        }
        audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);

        if(audmain_get_data(handle, buffer_start + index, &buffer) !=
           AUDMAIN_STATUS_SUCCESS) {
          return(AUDMAIN_STATUS_ERROR);
        } else {
          parse_ctl->buffer_size = audmain_get_data_buffer_size();
          buffer_start  += index;
          index          = 0;
        }
      }

      /*Check the index, if audfmt format chunck size is corrupted 
        index can have undesirable value */
      if((index + buffer_start) > file_size) {
         MSG_ERROR("format chunk data size corrupted" 
                   "index:%x buffer_start:%x  file_size:%x ",
                   index, buffer_start, file_size);
        return(AUDMAIN_STATUS_ERROR);
      }
   
      /*Check the index size, if required get the new data buffer */
      if(index >= parse_ctl->buffer_size )
      {
        audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);
        if(audmain_get_data(handle, buffer_start + index, &buffer) !=
           AUDMAIN_STATUS_SUCCESS) {
          return(AUDMAIN_STATUS_ERROR);
        } else {
          parse_ctl->buffer_size = audmain_get_data_buffer_size();
          buffer_start  += index;
          index          = 0;
        }
      }

      if(status == QCPSUP_STATUS_SUCCESS) {
        if (audadpcm_is_supported(&audadpcm_parse_ctl.format) == FALSE) {
          return(AUDMAIN_STATUS_ERROR);
        }

        while((status = qcpsup_find_chunk(chunk_id, buffer, &index,
                                          parse_ctl->buffer_size,
                                          &audadpcm_parse_ctl.total_length)) ==
              QCPSUP_STATUS_BUFF_ERROR)
        {
          if((index + buffer_start) >= file_size) {
            return(AUDMAIN_STATUS_ERROR);
          }
          audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);

          if(audmain_get_data(handle, buffer_start + index, &buffer) !=
             AUDMAIN_STATUS_SUCCESS) {
            return(AUDMAIN_STATUS_ERROR);
          } else {
            parse_ctl->buffer_size = audmain_get_data_buffer_size();
            buffer_start  += index;
            index          = 0;
          }
        }
        /*Check for the valid data length */
        if(audadpcm_parse_ctl.total_length >= file_size )
        {
          MSG_ERROR("data chunk size corrupted" 
                    "data total_length:%x file_size:%x ",
                    audadpcm_parse_ctl.total_length, file_size, 0);
         return(AUDMAIN_STATUS_ERROR);

        }

      }

      if (!status == QCPSUP_STATUS_SUCCESS) {
        return(AUDMAIN_STATUS_DATA_ERR);
      }
    } else {
      return(AUDMAIN_STATUS_ERROR);
    }

    /* Calculate total time to play file */
    /* Check If bytes_rate is zero before calculating the totaltime
       to prevent divied by Zero error */
    if(!audadpcm_parse_ctl.format.bytes_rate)
    {
      MSG_ERROR("audadpcm_parse_head :Received Zero Bytes_rate=%d",
                audadpcm_parse_ctl.format.bytes_rate,0,0);
      return (AUDMAIN_STATUS_ERROR);
    }
    else
    {
      if(audadpcm_parse_ctl.total_length < 4294967) {
        audadpcm_parse_ctl.time_ms = (audadpcm_parse_ctl.total_length * 1000) / 
                                           (audadpcm_parse_ctl.format.bytes_rate);
      } else {
        tqword = (uint64) audadpcm_parse_ctl.total_length * 1000;
        audadpcm_parse_ctl.time_ms = tqword /
                                         audadpcm_parse_ctl.format.bytes_rate;
      }
    }

    audadpcm_parse_ctl.track1.buffer  = buffer;
    audadpcm_parse_ctl.track2.buffer  = NULL;
    audadpcm_parse_ctl.reserve.buffer = NULL;
    audadpcm_parse_ctl.handle         = handle;
    audadpcm_parse_ctl.error_status   = AUDMAIN_STATUS_MAX;
    parse_ctl->type = AUDMAIN_INVALID_TYPE;
#ifdef FEATURE_ADPCM
    if(audadpcm_parse_ctl.format.wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM)
    {
      parse_ctl->type = AUDMAIN_ADPCM_TYPE;
#ifdef FEATURE_QSYNTH_ADPCM
      adec_mode = AUDMAIN_ADEC_MODE_ADPCM;
#endif
    }
#endif /* FEATURE_ADPCM */

#ifdef FEATURE_PCM
    if(audadpcm_parse_ctl.format.wave_format == QCPSUP_WAVE_FORMAT_LINEAR_PCM)
    {
      if(audadpcm_parse_ctl.format.bits_per_sample == 8) {
        audadpcm_parse_ctl.signed_samples = FALSE;
      } else {
        audadpcm_parse_ctl.signed_samples = TRUE;
      }
      parse_ctl->type = AUDMAIN_PCM_TYPE;
#ifdef FEATURE_QSYNTH_PCM
      adec_mode = AUDMAIN_ADEC_MODE_PCM;
#elif defined(FEATURE_QSYNTH_ADPCM)
      adec_mode = AUDMAIN_ADEC_MODE_YADPCM;
#endif
    }
#endif /* FEATURE_PCM */

#ifdef FEATURE_VOC_G711
    if(audadpcm_parse_ctl.format.wave_format == QCPSUP_WAVE_FORMAT_ALAW) {
      parse_ctl->type = AUDMAIN_WAV_ALAW_TYPE;
    }
    if(audadpcm_parse_ctl.format.wave_format == QCPSUP_WAVE_FORMAT_MULAW) {
      parse_ctl->type = AUDMAIN_WAV_MULAW_TYPE;
    }
#endif /* FEATURE_VOC_G711 */

    if(param->calc_time) {
      audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_AUX_FUNC;
    } else if(param->get_specs) {
      audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_AUX_FUNC;
      audadpcm_do_file_spec (handle, &audadpcm_parse_ctl);
    } else {
#ifdef FEATURE_QSYNTH_ADPCM
      if (audmain_set_adec_mode(adec_mode) != AUDMAIN_STATUS_SUCCESS) {
        return (AUDMAIN_STATUS_ERROR);
      }
      audmain_set_hybrid_mode(AUDMAIN_AUDFMT_POLY_MODE_7);
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
      audmain_adec_reset_channel(parse_ctl, 0);
#endif
#endif /* FEATURE_QSYNTH_ADPCM */

      tlong = ((audadpcm_parse_ctl.format.sample_rate *
                audadpcm_parse_ctl.format.bits_per_sample *
                audadpcm_parse_ctl.format.channels) >> 3) / 10;
#ifdef FEATURE_ADPCM
      if (audadpcm_parse_ctl.format.wave_format ==
          QCPSUP_WAVE_FORMAT_IMA_ADPCM) {
#ifdef FEATURE_GASYNTH_INTERFACE
        audadpcm_parse_ctl.frame_size = 
          ((((audadpcm_parse_ctl.format.sample_rate / 400) + 3) * 
          audadpcm_parse_ctl.format.channels) + 2) * 2;
#else
        audadpcm_parse_ctl.frame_size = AUDADPCM_FRAME_LENGTH_MAX;
#endif /* FEATURE_GASYNTH_INTERFACE */
      } else
#endif /* FEATURE_ADPCM */
      {
        audadpcm_parse_ctl.frame_size = ((tlong / 20) + 2) << 1;
      }

      tlong = ((tlong >> 2) + 1) << 1;
      if((tlong <= parse_ctl->buffer_size) && (parse_ctl->buffer_size < 5000)) {
        audadpcm_parse_ctl.block_length = parse_ctl->buffer_size;
      } else {
        audadpcm_parse_ctl.block_length = tlong;
      }
      audadpcm_parse_ctl.block_length = MAX(1000, 
                                          audadpcm_parse_ctl.block_length);
      audadpcm_set_data_rate(&audadpcm_parse_ctl);

      audadpcm_parse_ctl.track_status   = AUDADPCM_STATUS_TRACK1;
      audadpcm_parse_ctl.track_inuse    = AUDADPCM_STATUS_NONE;
      parse_ctl->tracks = &audadpcm_parse_ctl;
      audadpcm_do_file_spec (handle, &audadpcm_parse_ctl);

      audadpcm_parse_ctl.start_position = buffer_start + index;

      if(((uint32) audadpcm_parse_ctl.track1.buffer) & 0x1) {
        audadpcm_parse_ctl.file_location = buffer_start + index;
        audadpcm_parse_ctl.track1.length = 0;
        audadpcm_parse_ctl.data_length   = audadpcm_parse_ctl.total_length;

        audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0,
                           (void *) audadpcm_parse_ctl.track1.buffer);

        audadpcm_parse_ctl.track1.buffer = NULL;
        
        audmain_set_read_sync(FALSE);
        audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_BUF_PENDING |
                                          AUDADPCM_MASK_BUFF_ERR_SENT;

        audmain_get_var_data(handle, audadpcm_parse_ctl.file_location,
                             audadpcm_parse_ctl.block_length,
                             audadpcm_parse_ctl.frame_size,
                             &(audadpcm_parse_ctl.track1.buffer));

      } else {
        buffer_size = MIN(parse_ctl->buffer_size, 0xfff0);

        audadpcm_parse_ctl.file_location = buffer_start + buffer_size;

        tlong = buffer_size - index;
        if(audadpcm_parse_ctl.total_length > tlong) {
          audadpcm_parse_ctl.track1.length = tlong;
          audadpcm_parse_ctl.data_length   = audadpcm_parse_ctl.total_length -
                                             tlong;
        } else {
          audadpcm_parse_ctl.track1.length = audadpcm_parse_ctl.total_length;
          audadpcm_parse_ctl.data_length   = 0;
        }

        audadpcm_parse_ctl.track1.index   = index;

        if((audadpcm_parse_ctl.track1.length <= audadpcm_parse_ctl.frame_size)
          && (audadpcm_parse_ctl.data_length != 0)) {
          audmain_set_read_sync(FALSE);
          audadpcm_parse_ctl.status_flag |= AUDADPCM_MASK_BUF_PENDING |
                                            AUDADPCM_MASK_COPY_PENDING;
          audmain_get_var_data(handle, audadpcm_parse_ctl.file_location,
                               audadpcm_parse_ctl.block_length,
                               audadpcm_parse_ctl.frame_size,
                               &(audadpcm_parse_ctl.track2.buffer));
        }
      }
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_age

DESCRIPTION
  This function ages the file by the indicated number of milliseconds.
  No aging is necessary for ADPCM files because the timing is driven by the
  vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audadpcm_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
    /*lint -esym(715,elapsed_time) Tell Lint we know elapsed_time 
      parameter is not used */
  uint32                  *adjusted_time
    /*lint -esym(715,adjusted_time) Tell Lint we know adjusted_time 
      parameter is not used */
) {
  audmain_status_type              status;
  audadpcm_parse_ctl_type          *adpcm_ctl;
  audmain_adpcm_mode_enum_type     pcm_mode = AUDMAIN_ADPCM_INVALID_MODE;
  qcpsup_adpcm_format_type         *adpcm_format;
  static audmain_adpcm_format_type format; /* Allocate on heap, not stack */
  adpcm_ctl = (audadpcm_parse_ctl_type *)parse_ctl->tracks;

  if (adpcm_ctl != NULL) {
    adpcm_format = &adpcm_ctl->format;
    if ((adpcm_ctl->status_flag & AUDADPCM_MASK_INIT_DONE) == 0) {
      switch(adpcm_format->wave_format) {
        case QCPSUP_WAVE_FORMAT_IMA_ADPCM:
          pcm_mode = AUDMAIN_ADPCM_ADPCM_MODE;
          break;
        case QCPSUP_WAVE_FORMAT_LINEAR_PCM:
          if(adpcm_ctl->signed_samples) {
            pcm_mode = AUDMAIN_ADPCM_SIGNED_PCM_MODE;
          } else {
            pcm_mode = AUDMAIN_ADPCM_UNSIGNED_PCM_MODE;
          }
          break;
#ifdef FEATURE_VOC_G711
        case QCPSUP_WAVE_FORMAT_ALAW:
          pcm_mode = AUDMAIN_ADPCM_WAV_ALAW_MODE;
          break;
        case QCPSUP_WAVE_FORMAT_MULAW:
          pcm_mode = AUDMAIN_ADPCM_WAV_MULAW_MODE;
          break;
#endif /* FEATURE_VOC_G711 */
      }
      format.channels        = adpcm_format->channels;
      format.sample_rate     = adpcm_format->sample_rate;
      format.bytes_rate      = adpcm_format->bytes_rate;
      format.frame_size      = adpcm_format->frame_size;
      format.bits_per_sample = adpcm_format->bits_per_sample;

      status = audmain_adpcm_start ( 0, AUDMAIN_ADPCM_PLAY_FILE,
                                     &format, pcm_mode,
                                     AUDMAIN_ADEC_START_MODE_NORMAL );
      if(status == AUDMAIN_STATUS_SUCCESS) {
        adpcm_ctl->status_flag |= AUDADPCM_MASK_INIT_DONE;
      } else if(status == AUDMAIN_STATUS_FAILURE) {
        adpcm_ctl->status_flag |= AUDADPCM_MASK_FAILURE;
        adpcm_ctl->error_status = AUDMAIN_STATUS_RESOURCE_ERR;
      }
      *elapsed_time  = 0;
      *adjusted_time = 0;
    } else { 
      *adjusted_time = *elapsed_time;
    }

    (void) audadpcm_get_data(adpcm_ctl);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_parse

DESCRIPTION
  This function parses one event of the ADPCM file.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if things have been set up to get data
  AUDMAIN_STATUS_DONE    - if already at the end of file
  AUDMAIN_STATUS_FAILURE - if error occurs

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audadpcm_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
    /*lint -esym(715,time_remain) Tell Lint we know time_remain 
      parameter is not used */
) {
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;
  audadpcm_parse_ctl_type *adpcm_ctl;

  if((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    adpcm_ctl = (audadpcm_parse_ctl_type *) (parse_ctl->tracks);
    if (!(adpcm_ctl->status_flag & AUDADPCM_MASK_FAILURE)) {
      cmd_buf->adec_cmd.adec_buf1     = 0;
      cmd_buf->adec_cmd.adec_num_buf1 = 0;
      cmd_buf->adec_cmd.adec_buf2     = 0;
      cmd_buf->adec_cmd.adec_num_buf2 = 0;
      cmd_buf->adec_cmd.adec_reset    = 0;
#ifdef FEATURE_GASYNTH_INTERFACE
      if ((adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM) ||
          (adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_LINEAR_PCM)) {
        if ((adpcm_ctl->data_length == 0) &&
            (adpcm_ctl->reserve.buffer == NULL) &&
            !((adpcm_ctl->track1.buffer != NULL) &&
              !(adpcm_ctl->track_inuse & AUDADPCM_STATUS_TRACK1)) &&
            !((adpcm_ctl->track2.buffer != NULL) &&
              !(adpcm_ctl->track_inuse & AUDADPCM_STATUS_TRACK2))) {
          adpcm_ctl->status_flag |= AUDADPCM_MASK_DONE_PENDING;
          return (AUDMAIN_STATUS_DONE);
        }
      } else 
#endif /* FEATURE_GASYNTH_INTERFACE */
      if((adpcm_ctl->data_length == 0) && 
         (adpcm_ctl->track1.length == 0) &&
         (adpcm_ctl->track2.length == 0)) {
        return (AUDMAIN_STATUS_DONE);
      }

      if (adpcm_ctl->status_flag & AUDADPCM_MASK_INIT_DONE) {
#ifdef FEATURE_GASYNTH_INTERFACE 
        if ((adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM) ||
            (adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_LINEAR_PCM)) {
          if ((adpcm_ctl->track2.length == 0) &&
              (adpcm_ctl->track1.length == 0) &&
              (adpcm_ctl->reserve.buffer == NULL) &&
              !(adpcm_ctl->status_flag & AUDADPCM_MASK_BUFF_ERR_SENT)) {
            adpcm_ctl->status_flag |= AUDADPCM_MASK_BUFF_ERR_SENT;
            return (AUDMAIN_STATUS_BUFF_ERR);
          }
          if (adpcm_ctl->track1.buffer != NULL) {
            cmd_buf->adec_cmd.adec_buf1     = &adpcm_ctl->track1.buffer
                                                  [adpcm_ctl->track1.index];
            cmd_buf->adec_cmd.adec_num_buf1 = adpcm_ctl->track1.length;
          } else {
            cmd_buf->adec_cmd.adec_buf1     = NULL;
            cmd_buf->adec_cmd.adec_num_buf1 = 0;
          }
          if (adpcm_ctl->track2.buffer != NULL) {
            cmd_buf->adec_cmd.adec_buf2     = &adpcm_ctl->track2.buffer
                                                   [adpcm_ctl->track2.index];
            cmd_buf->adec_cmd.adec_num_buf2 = adpcm_ctl->track2.length;
          } else {
            cmd_buf->adec_cmd.adec_buf2 = NULL;
            cmd_buf->adec_cmd.adec_num_buf2 = 0;
          }
          cmd_buf->adec_cmd.buf_usage_mask = adpcm_ctl->track_inuse;
        } else 
#endif /* FEATURE_GASYNTH_INTERFACE */
        if (adpcm_ctl->track_status != AUDADPCM_STATUS_TRACK2) {
          if(adpcm_ctl->track1.length != 0) {
            adpcm_ctl->status_flag &= ~AUDADPCM_MASK_BUFF_ERR_SENT;
            cmd_buf->adec_cmd.adec_buf1     = adpcm_ctl->track1.buffer;
            cmd_buf->adec_cmd.adec_buf2     = &adpcm_ctl->track1.buffer
                                                      [adpcm_ctl->track1.index];
            cmd_buf->adec_cmd.adec_num_buf1 = adpcm_ctl->track1.length;
            cmd_buf->adec_cmd.adec_num_buf2 = adpcm_ctl->track1.index;
          } else if(!(adpcm_ctl->status_flag & AUDADPCM_MASK_BUFF_ERR_SENT)) {
            /* Data underflow encountered */
            adpcm_ctl->status_flag |= AUDADPCM_MASK_BUFF_ERR_SENT;
            return (AUDMAIN_STATUS_BUFF_ERR);
          }
        } else {
          if(adpcm_ctl->track2.length != 0) {
            adpcm_ctl->status_flag &= ~AUDADPCM_MASK_BUFF_ERR_SENT;
            cmd_buf->adec_cmd.adec_buf1     = adpcm_ctl->track2.buffer;
            cmd_buf->adec_cmd.adec_buf2     = &adpcm_ctl->track2.buffer
                                                      [adpcm_ctl->track2.index];
            cmd_buf->adec_cmd.adec_num_buf1 = adpcm_ctl->track2.length;
            cmd_buf->adec_cmd.adec_num_buf2 = adpcm_ctl->track2.index;
          } else if(!(adpcm_ctl->status_flag & AUDADPCM_MASK_BUFF_ERR_SENT)) {
            /* Data underflow encountered */
            adpcm_ctl->status_flag |= AUDADPCM_MASK_BUFF_ERR_SENT;
            return (AUDMAIN_STATUS_BUFF_ERR);
          }
        }
      }  
      ret_val = AUDMAIN_STATUS_SUCCESS;
    } else {
      ret_val = adpcm_ctl->error_status;
    }
  } else {
    MSG_ERROR("Parse control structure is not set properly.", 0,0,0);
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_adec_adjust

DESCRIPTION
  This function adjust the buffer status.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if no errors in the prompt were found
  AUDMAIN_STATUS_ERROR   - if there is an invalid frame in the prompt

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GASYNTH_INTERFACE 
audmain_status_type audadpcm_adec_adjust ( 
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *buf1_used,
  uint32                  *buf2_used,
  uint8                   *buf_usage_mask
) {
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;
  audadpcm_parse_ctl_type *adpcm_ctl;
  uint32                  num_used;

  if ((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    ret_val   = AUDMAIN_STATUS_SUCCESS;
    num_used  = *buf1_used + *buf2_used;
    adpcm_ctl = (audadpcm_parse_ctl_type *)parse_ctl->tracks;
    if ((adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_IMA_ADPCM) ||
        (adpcm_ctl->format.wave_format == QCPSUP_WAVE_FORMAT_LINEAR_PCM)) {
      if (!(adpcm_ctl->status_flag & AUDADPCM_MASK_DONE_PENDING)) {
        adpcm_ctl->track_inuse = (audadpcm_track_status_enum_type)*buf_usage_mask;
      }
      if(num_used != 0) {
        adpcm_ctl->track1.length -= *buf1_used;
        adpcm_ctl->track2.length -= *buf2_used;
        ret_val = audadpcm_get_data(adpcm_ctl);
      }
    } else {
      if(num_used != 0) {
        if (adpcm_ctl->track_status != AUDADPCM_STATUS_TRACK2) {
          adpcm_ctl->track1.length -= num_used;
          adpcm_ctl->track1.index  += num_used;
          if (adpcm_ctl->track1.length == 0) {
            adpcm_ctl->track_status = AUDADPCM_STATUS_TRACK2;
          }
          adpcm_ctl->track_inuse = AUDADPCM_STATUS_TRACK1;
        } else {
          adpcm_ctl->track2.length -= num_used;
          adpcm_ctl->track2.index  += num_used;
          if (adpcm_ctl->track2.length == 0) {
            adpcm_ctl->track_status = AUDADPCM_STATUS_TRACK1;
          }
          adpcm_ctl->track_inuse = AUDADPCM_STATUS_TRACK2;
        }
        (void) audadpcm_get_data(adpcm_ctl);
      }
    }
  }
  return (ret_val);
} 
#else
audmain_status_type audadpcm_adec_adjust ( 
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *num_used
) {
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;
  audadpcm_parse_ctl_type *adpcm_ctl;

  if ((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    if(*num_used != 0) {
      adpcm_ctl = (audadpcm_parse_ctl_type *)parse_ctl->tracks;
      if (adpcm_ctl->track_status != AUDADPCM_STATUS_TRACK2) {
        adpcm_ctl->track1.length -= *num_used;
        adpcm_ctl->track1.index  += *num_used;
        if (adpcm_ctl->track1.length == 0) {
          adpcm_ctl->track_status = AUDADPCM_STATUS_TRACK2;
        }
        adpcm_ctl->track_inuse = AUDADPCM_STATUS_TRACK1;
      } else {
        adpcm_ctl->track2.length -= *num_used;
        adpcm_ctl->track2.index  += *num_used;
        if (adpcm_ctl->track2.length == 0) {
          adpcm_ctl->track_status = AUDADPCM_STATUS_TRACK1;
        }
        adpcm_ctl->track_inuse = AUDADPCM_STATUS_TRACK2;
      }
      (void) audadpcm_get_data(adpcm_ctl);
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return (ret_val);
}
#endif /* FEATURE_GASYNTH_INTERFACE */
/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_reset

DESCRIPTION
  This function indicates that playback has ended and audadpcm should reset
  its state.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  audadpcm_parse_ctl.

===========================================================================*/
void audadpcm_reset ( void )
{
#ifdef FEATURE_QSYNTH_ADPCM
  if(!(audadpcm_parse_ctl.status_flag & AUDADPCM_MASK_AUX_FUNC)) {
    audmain_adpcm_stop( 0 );
  }
#endif /* FEATURE_QSYNTH_ADPCM */

  if(audadpcm_parse_ctl.track1.buffer != NULL) {
    audmain_do_play_cb(audadpcm_parse_ctl.handle, SND_CMX_AF_FREE_VAR_BUF, 0, 
                       (void *) audadpcm_parse_ctl.track1.buffer);
  }
  if(audadpcm_parse_ctl.track2.buffer != NULL) {
    audmain_do_play_cb(audadpcm_parse_ctl.handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                       (void *) audadpcm_parse_ctl.track2.buffer);
  }
  if(audadpcm_parse_ctl.reserve.buffer != NULL) {
    audmain_do_play_cb(audadpcm_parse_ctl.handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                       (void *) audadpcm_parse_ctl.reserve.buffer);
  }
  memset( &audadpcm_parse_ctl, 0, sizeof(audadpcm_parse_ctl_type));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of G711 voc or qsynth.
  Which is independent of the file.

DEPENDENCIES
  None.

RETURN VALUE
  snd_sample_rate_type.

SIDE EFFECTS
  None.

===========================================================================*/
snd_sample_rate_type audadpcm_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
) {
#ifdef FEATURE_VOC_G711
  if(parse_ctl->type == AUDMAIN_WAV_ALAW_TYPE ||
     parse_ctl->type == AUDMAIN_WAV_MULAW_TYPE) {
    /* The file has already been parsed and validated against this sample rate*/
    return(SND_SAMPLE_RATE_8000);
  } else
#endif /* FEATURE_VOC_G711 */
  {
    return SND_SAMPLE_RATE_32000;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_get_length

DESCRIPTION
  This function returns the length of the file in ms.

DEPENDENCIES
  audadpcm_parse_ctl.

RETURN VALUE
  AUDMAIN_STATUS_DONE.

SIDE EFFECTS
  None.

===========================================================================*/
audmain_status_type audadpcm_get_length (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *time_ms
) {
  *time_ms = audadpcm_parse_ctl.time_ms;
  return AUDMAIN_STATUS_DONE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_reposition

DESCRIPTION
  This function repositions the data pointer to the indicated position.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
audmain_status_type audadpcm_reposition (
  audadpcm_parse_ctl_type *adpcm_ctl,
  uint32                  position
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

  if((adpcm_ctl != NULL) && (adpcm_ctl->handle != NULL)) {
    if((position >= adpcm_ctl->start_position) &&
       ((adpcm_ctl->total_length == AUDADPCM_MAX_32BIT) ||
        (position < (adpcm_ctl->start_position + adpcm_ctl->total_length)))) {
      if(adpcm_ctl->track1.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->track1.buffer);
      }
      if(adpcm_ctl->track2.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->track2.buffer);
      }
      if(adpcm_ctl->reserve.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->reserve.buffer);
      }
      adpcm_ctl->track1.buffer  = NULL;
      adpcm_ctl->track1.index   = 0;
      adpcm_ctl->track1.length  = 0;
      adpcm_ctl->track2.buffer  = NULL;
      adpcm_ctl->track2.index   = 0;
      adpcm_ctl->track2.length  = 0;
      adpcm_ctl->reserve.buffer = NULL;
      adpcm_ctl->reserve.index  = 0;
      adpcm_ctl->reserve.length = 0;

      adpcm_ctl->track_status  = AUDADPCM_STATUS_TRACK1;
      adpcm_ctl->track_inuse   = AUDADPCM_STATUS_NONE;
      if(adpcm_ctl->total_length != AUDADPCM_MAX_32BIT) {
        adpcm_ctl->data_length = adpcm_ctl->total_length -
                                 (position - adpcm_ctl->start_position);
      }
      adpcm_ctl->file_location = position;

      if(adpcm_ctl->status_flag & AUDADPCM_MASK_BUF_PENDING) {
        adpcm_ctl->status_flag &= ~AUDADPCM_MASK_COPY_PENDING;
        adpcm_ctl->status_flag |= AUDADPCM_MASK_REPOSITION;
      } else {
        adpcm_ctl->status_flag |= AUDADPCM_MASK_BUF_PENDING;
        audmain_get_var_data(adpcm_ctl->handle, adpcm_ctl->file_location,
                             adpcm_ctl->block_length, adpcm_ctl->frame_size,
                             &(adpcm_ctl->track1.buffer));
      }

      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_flush

DESCRIPTION
  This function flushes data buffers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
audmain_status_type audadpcm_flush (
  audmain_parse_ctl_type  *parse_ctl
) {
  audmain_status_type     ret_val = AUDMAIN_STATUS_FAILURE;
  audadpcm_parse_ctl_type *adpcm_ctl;

  if((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    adpcm_ctl = (audadpcm_parse_ctl_type *) (parse_ctl->tracks);

    ret_val = audadpcm_reposition(adpcm_ctl, adpcm_ctl->start_position);
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_fforward

DESCRIPTION
  This function moves the playback forward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audadpcm_fforward (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  audmain_status_type     ret_val = AUDMAIN_STATUS_FAILURE;
  audadpcm_parse_ctl_type *adpcm_ctl;
  uint32                  adjust_bytes, buf_offset, current_location;

  if((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    adpcm_ctl = (audadpcm_parse_ctl_type *) (parse_ctl->tracks);

    if(ms_forward < adpcm_ctl->overflow_rate) {
      if(adpcm_ctl->rate_offset == 1) {
        adjust_bytes = ms_forward * adpcm_ctl->data_rate;
      } else {
        adjust_bytes = (ms_forward * adpcm_ctl->data_rate) /
                       adpcm_ctl->rate_offset;
      }
    } else if(ms_forward < adpcm_ctl->overflow_offset) {
      adjust_bytes = (ms_forward / adpcm_ctl->rate_offset) *
                     adpcm_ctl->data_rate;
    } else {
      adjust_bytes = AUDADPCM_MAX_32BIT;
    }

    if((adpcm_ctl->track1.buffer != NULL) &&
       (adpcm_ctl->track2 .buffer!= NULL)) {
      buf_offset = (adpcm_ctl->track1.length + adpcm_ctl->track2.length) >> 1;
    } else if((adpcm_ctl->track1.buffer != NULL) ||
              (adpcm_ctl->track2.buffer != NULL)) {
      buf_offset = adpcm_ctl->block_length >> 1;
    } else {
      buf_offset = 0;
    }

    if(((adpcm_ctl->data_length == AUDADPCM_MAX_32BIT) ||
        ((adpcm_ctl->data_length + buf_offset) > adjust_bytes)) &&
       (adjust_bytes != AUDADPCM_MAX_32BIT)) {
      current_location = adpcm_ctl->file_location - buf_offset + adjust_bytes;
      current_location = (current_location & ~0x3) |
                         (adpcm_ctl->start_position & 0x3);
      ret_val          = audadpcm_reposition(adpcm_ctl, current_location);

      if(current_location <= adpcm_ctl->start_position) {
        *elapsed_ms  = 0;
        *adjusted_ms = 0;
      } else {
        current_location = current_location - adpcm_ctl->start_position;
        if(current_location > adpcm_ctl->overflow_offset_ms) {
          *elapsed_ms = (current_location / adpcm_ctl->data_rate) *
                        adpcm_ctl->rate_offset;
        } else {
          *elapsed_ms = (current_location * adpcm_ctl->rate_offset) /
                        adpcm_ctl->data_rate;
        }
        *adjusted_ms = *elapsed_ms;
      }
    } else {
      adpcm_ctl->data_length = 0;
      ret_val                = AUDMAIN_STATUS_SUCCESS;

      if(adpcm_ctl->track1.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->track1.buffer);
      }
      if(adpcm_ctl->track2.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->track2.buffer);
      }
      if(adpcm_ctl->reserve.buffer != NULL) {
        audmain_do_play_cb(adpcm_ctl->handle, SND_CMX_AF_FREE_VAR_BUF, 0,
                           adpcm_ctl->reserve.buffer);
      }
      adpcm_ctl->track1.buffer  = NULL;
      adpcm_ctl->track1.index   = 0;
      adpcm_ctl->track1.length  = 0;
      adpcm_ctl->track2.buffer  = NULL;
      adpcm_ctl->track2.index   = 0;
      adpcm_ctl->track2.length  = 0;
      adpcm_ctl->reserve.buffer = NULL;
      adpcm_ctl->reserve.index  = 0;
      adpcm_ctl->reserve.length = 0;

      if(adpcm_ctl->status_flag & AUDADPCM_MASK_BUF_PENDING) {
        adpcm_ctl->status_flag |= AUDADPCM_MASK_REPOSITION;
      }

      *elapsed_ms   = adpcm_ctl->time_ms;
      *adjusted_ms  = adpcm_ctl->time_ms;
    }
  }
  return (ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_rewind

DESCRIPTION
  This function moves the playback back ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audadpcm_rewind (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  audmain_status_type     ret_val = AUDMAIN_STATUS_FAILURE;
  audadpcm_parse_ctl_type *adpcm_ctl;
  uint32                  adjust_bytes, buf_offset, current_location;

  if((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    adpcm_ctl = (audadpcm_parse_ctl_type *) (parse_ctl->tracks);

    if(ms_rewind < adpcm_ctl->overflow_rate) {
      if(adpcm_ctl->rate_offset == 1) {
        adjust_bytes = ms_rewind * adpcm_ctl->data_rate;
      } else {
        adjust_bytes = (ms_rewind * adpcm_ctl->data_rate) /
                       adpcm_ctl->rate_offset;
      }
    } else if(ms_rewind < adpcm_ctl->overflow_offset) {
      adjust_bytes = (ms_rewind / adpcm_ctl->rate_offset) *
                     adpcm_ctl->data_rate;
    } else {
      adjust_bytes = AUDADPCM_MAX_32BIT;
    }

    if((adpcm_ctl->track1.buffer != NULL) &&
       (adpcm_ctl->track2 .buffer!= NULL)) {
      buf_offset = (adpcm_ctl->track1.length + adpcm_ctl->track2.length) >> 1;
    } else if((adpcm_ctl->track1.buffer != NULL) ||
              (adpcm_ctl->track2.buffer != NULL)) {
      buf_offset = adpcm_ctl->block_length >> 1;
    } else {
      buf_offset = 0;
    }

    if(((adpcm_ctl->data_length == AUDADPCM_MAX_32BIT) ||
        ((adpcm_ctl->file_location - buf_offset - adpcm_ctl->start_position) >
         adjust_bytes)) && (adjust_bytes != AUDADPCM_MAX_32BIT)) {
      current_location = adpcm_ctl->file_location - buf_offset - adjust_bytes;
      current_location = (current_location & ~0x3) |
                         (adpcm_ctl->start_position & 0x3);
      ret_val          = audadpcm_reposition(adpcm_ctl, current_location);

      if(current_location <= adpcm_ctl->start_position) {
        *elapsed_ms  = 0;
        *adjusted_ms = 0;
      } else {
        current_location = current_location - adpcm_ctl->start_position;
        if(current_location > adpcm_ctl->overflow_offset_ms) {
          *elapsed_ms = (current_location / adpcm_ctl->data_rate) *
                        adpcm_ctl->rate_offset;
        } else {
          *elapsed_ms = (current_location * adpcm_ctl->rate_offset) /
                        adpcm_ctl->data_rate;
        }
        *adjusted_ms = *elapsed_ms;
      }
    } else {
      ret_val = audadpcm_reposition(adpcm_ctl, adpcm_ctl->start_position);

      *elapsed_ms  = 0;
      *adjusted_ms = 0;
    }
  }
  return (ret_val);  
}
#endif /* FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION audpcm_record_frame

DESCRIPTION
  This function records a frame of data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audpcm_record_frame (
  audpcm_rec_ctl_type *ctl_ptr,
  const uint16        *pcm_data
) {
  snd_pcm_rec_data_struct_type data;

  if(ctl_ptr != NULL) {
    if((ctl_ptr->status & AUDPCM_MASK_ENABLED) &&
       !(ctl_ptr->status & AUDPCM_MASK_PAUSED)) {
      if(ctl_ptr->current_buf != NULL) {
        if(ctl_ptr->num_frames == 49) {
          audmain_pcm_rec_status(SND_CMX_AF_TIME_TICK, NULL,
                                 ctl_ptr->cb_func, ctl_ptr->client_data);
          ctl_ptr->num_frames = 0;
        } else {
          ctl_ptr->num_frames++;
        }
        memcpy(&ctl_ptr->current_buf[ctl_ptr->index_current_buf],
               (uint8 *) pcm_data, 2*VOC_PCM_FRAME_LENGTH);
        ctl_ptr->index_current_buf   += 2*VOC_PCM_FRAME_LENGTH;
        if(ctl_ptr->status & AUDPCM_MASK_FRAME_DROPPED) {
          audmain_pcm_rec_status(SND_CMX_PCM_REC_DROP_RESUME, NULL,
                                 ctl_ptr->cb_func, ctl_ptr->client_data);
          ctl_ptr->status &= ~AUDPCM_MASK_FRAME_DROPPED;
        }

        if((ctl_ptr->index_current_buf + 2*VOC_PCM_FRAME_LENGTH) >
           (ctl_ptr->size_current_buf)) {
          data.rec_data_buf  = ctl_ptr->current_buf;
          data.rec_data_num  = ctl_ptr->index_current_buf;
          data.rec_data_pos  = ctl_ptr->running_index;
          data.client_data   = (const void*) ctl_ptr;
          audmain_pcm_rec_status(SND_CMX_PCM_REC_DATA, &data, ctl_ptr->cb_func,
                                 ctl_ptr->client_data);

          ctl_ptr->running_index     += ctl_ptr->index_current_buf;
          ctl_ptr->index_current_buf  = 0;
          ctl_ptr->current_buf        = ctl_ptr->reserve_buf;
          ctl_ptr->size_current_buf   = ctl_ptr->size_reserve_buf;
          ctl_ptr->reserve_buf        = NULL;
          ctl_ptr->size_reserve_buf   = 0;
        }
      } else if(!(ctl_ptr->status & AUDPCM_MASK_FRAME_DROPPED)) {
        audmain_pcm_rec_status(SND_CMX_PCM_REC_DROP_FRAMES, NULL,
                               ctl_ptr->cb_func, ctl_ptr->client_data);
        ctl_ptr->status |= AUDPCM_MASK_FRAME_DROPPED;
      }
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audpcm_record_reverse_cb

DESCRIPTION
  This function is used as CB func for recording reverse link.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audpcm_record_reverse_cb (
  const uint16 *pcm_data
) {
  audpcm_record_frame(&audpcm_rec_reverse_ctl, pcm_data);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audpcm_record_forward_cb

DESCRIPTION
  This function is used as CB func for recording forward link.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audpcm_record_forward_cb (
  const uint16 *pcm_data
) {
  audpcm_record_frame(&audpcm_rec_forward_ctl, pcm_data);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audpcm_record

DESCRIPTION
  This function starts a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_pcm_client_output_fn_type audpcm_record (
  snd_pcm_rec_dir_type         link,
  uint8                        *buf1,
  uint32                       num_buf1,
  uint8                        *buf2,
  uint32                       num_buf2,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  snd_pcm_rec_data_struct_type  data;
  voc_pcm_client_output_fn_type ret_val  = NULL;
  audpcm_rec_ctl_type           *ctl_ptr = NULL;

  if(link == SND_PCM_REC_DIR_REVERSE) {
    ctl_ptr = &audpcm_rec_reverse_ctl;
    ret_val = audpcm_record_reverse_cb;
  } else if(link == SND_PCM_REC_DIR_FORWARD) {
    ctl_ptr = &audpcm_rec_forward_ctl;
    ret_val = audpcm_record_forward_cb;
  }

  if((ctl_ptr != NULL) && !(ctl_ptr->status & AUDPCM_MASK_ENABLED)) {
    data.rec_data_buf  = (uint8 *) audpcm_header;
    data.rec_data_num  = sizeof(audpcm_header);
    data.rec_data_pos  = 0;
    data.buf_cb_func   = NULL;
    data.client_data   = NULL;
    if(cb_func != NULL) {
      cb_func(SND_CMX_PCM_REC_HEADER_DATA, &data, client_data);
    }

    INTLOCK();
    ctl_ptr->status             = AUDPCM_MASK_ENABLED;
    ctl_ptr->num_frames         = 0;
    ctl_ptr->running_index      = sizeof(audpcm_header);
    ctl_ptr->current_buf        = buf1;
    ctl_ptr->size_current_buf   = num_buf1;
    ctl_ptr->index_current_buf  = 0;
    ctl_ptr->reserve_buf        = buf2;
    ctl_ptr->size_reserve_buf   = num_buf2;
    ctl_ptr->cb_func            = cb_func;
    ctl_ptr->client_data        = client_data;
    INTFREE();
    return(ret_val);
  }
  return(NULL);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audpcm_record_stop

DESCRIPTION
  This function stops a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audpcm_record_stop (
  snd_pcm_rec_dir_type  link
) {
  snd_pcm_rec_data_struct_type rec_data;
  audmain_status_type          ret_val  = AUDMAIN_STATUS_FAILURE;
  audpcm_rec_ctl_type          *ctl_ptr = NULL;

  if(link == SND_PCM_REC_DIR_REVERSE) {
    ctl_ptr = &audpcm_rec_reverse_ctl;
  } else if(link == SND_PCM_REC_DIR_FORWARD) {
    ctl_ptr = &audpcm_rec_forward_ctl;
  }

  if((ctl_ptr != NULL) && (ctl_ptr->status & AUDPCM_MASK_ENABLED)) {
    INTLOCK();
    ctl_ptr->status &= ~AUDPCM_MASK_ENABLED;
    INTFREE();
    if(ctl_ptr->cb_func != NULL) {
      rec_data.buf_cb_func = NULL;
      rec_data.client_data = NULL;
      if(ctl_ptr->index_current_buf != 0) {
        rec_data.rec_data_buf   = ctl_ptr->current_buf;
        rec_data.rec_data_num   = ctl_ptr->index_current_buf;
        rec_data.rec_data_pos   = ctl_ptr->running_index;
        ctl_ptr->cb_func(SND_CMX_PCM_REC_DATA, &rec_data, ctl_ptr->client_data);

        ctl_ptr->running_index += ctl_ptr->index_current_buf;
      }
      ctl_ptr->file_size    = ctl_ptr->running_index - 8;
      rec_data.rec_data_buf = (uint8 *) &ctl_ptr->file_size;
      rec_data.rec_data_num = 4;
      rec_data.rec_data_pos = 4;
      ctl_ptr->cb_func(SND_CMX_PCM_REC_HEADER_DATA, &rec_data,
                       ctl_ptr->client_data);

      ctl_ptr->running_index -= sizeof(audpcm_header);
      rec_data.rec_data_buf   = (uint8 *) &ctl_ptr->running_index;
      rec_data.rec_data_num   = 4;
      rec_data.rec_data_pos   = 40;
      ctl_ptr->cb_func(SND_CMX_PCM_REC_HEADER_DATA, &rec_data,
                       ctl_ptr->client_data);

      ctl_ptr->cb_func(SND_CMX_PCM_REC_DONE, NULL, ctl_ptr->client_data);
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audpcm_record_pause

DESCRIPTION
  This function pauses/resumes a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audpcm_record_pause (
  boolean               pause,
  snd_pcm_rec_dir_type  link
) {
  audmain_status_type ret_val  = AUDMAIN_STATUS_FAILURE;
  audpcm_rec_ctl_type *ctl_ptr = NULL;

  if(link == SND_PCM_REC_DIR_REVERSE) {
    ctl_ptr = &audpcm_rec_reverse_ctl;
  } else if(link == SND_PCM_REC_DIR_FORWARD) {
    ctl_ptr = &audpcm_rec_forward_ctl;
  }

  if((ctl_ptr != NULL) && (ctl_ptr->status & AUDPCM_MASK_ENABLED)) {
    if(ctl_ptr->cb_func != NULL) {
      if(pause) {
        if(!(ctl_ptr->status & AUDPCM_MASK_PAUSED)) {
          ctl_ptr->cb_func(SND_CMX_PCM_REC_PAUSED, NULL, ctl_ptr->client_data);
        }
      } else {
        if(ctl_ptr->status & AUDPCM_MASK_PAUSED) {
          ctl_ptr->cb_func(SND_CMX_PCM_REC_RESUMED, NULL, ctl_ptr->client_data);
        }
      }
    }
    INTLOCK();
    if(pause) {
      ctl_ptr->status |= AUDPCM_MASK_PAUSED;
    } else {
      ctl_ptr->status &= ~AUDPCM_MASK_PAUSED;
    }
    INTFREE();
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audpcm_rec_send_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audpcm_rec_send_status (
  snd_af_data_cmd_enum status
) {
  switch (status) {
    case SND_CMX_PCM_REC_INTERRUPTED:
      if((audpcm_rec_reverse_ctl.status & AUDPCM_MASK_ENABLED) &&
         !(audpcm_rec_reverse_ctl.status & AUDPCM_MASK_INTERRUPTED) &&
         (audpcm_rec_reverse_ctl.cb_func != NULL)) {
        audpcm_rec_reverse_ctl.status |= AUDPCM_MASK_INTERRUPTED;
        audpcm_rec_reverse_ctl.cb_func(status, NULL,
                                       audpcm_rec_reverse_ctl.client_data);
      }

      if((audpcm_rec_forward_ctl.status & AUDPCM_MASK_ENABLED) &&
         !(audpcm_rec_forward_ctl.status & AUDPCM_MASK_INTERRUPTED) &&
         (audpcm_rec_forward_ctl.cb_func != NULL)) {
        audpcm_rec_forward_ctl.status |= AUDPCM_MASK_INTERRUPTED;
        audpcm_rec_forward_ctl.cb_func(status, NULL,
                                       audpcm_rec_forward_ctl.client_data);
      }
      break;

    case SND_CMX_PCM_REC_INT_RESUME:
      if((audpcm_rec_reverse_ctl.status & AUDPCM_MASK_ENABLED) &&
         (audpcm_rec_reverse_ctl.status & AUDPCM_MASK_INTERRUPTED) &&
         (audpcm_rec_reverse_ctl.cb_func != NULL)) {
        audpcm_rec_reverse_ctl.status &= ~AUDPCM_MASK_INTERRUPTED;
        audpcm_rec_reverse_ctl.cb_func(status, NULL,
                                       audpcm_rec_reverse_ctl.client_data);
      }

      if((audpcm_rec_forward_ctl.status & AUDPCM_MASK_ENABLED) &&
         (audpcm_rec_forward_ctl.status & AUDPCM_MASK_INTERRUPTED) &&
         (audpcm_rec_forward_ctl.cb_func != NULL)) {
        audpcm_rec_forward_ctl.status &= ~AUDPCM_MASK_INTERRUPTED;
        audpcm_rec_forward_ctl.cb_func(status, NULL,
                                       audpcm_rec_forward_ctl.client_data);
      }
      break;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audpcm_record_buf

DESCRIPTION
  This function accepts a buffer for PCM recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audpcm_record_buf (
  uint8       *buffer,
  uint32      num_bytes,
  const void  *client_data
) {
  audpcm_rec_ctl_type *ctl_ptr = (audpcm_rec_ctl_type *) client_data;

  if((ctl_ptr != NULL) && (ctl_ptr->status & AUDPCM_MASK_ENABLED)) {
    INTLOCK();
    if(ctl_ptr->current_buf == NULL) {
      ctl_ptr->current_buf       = buffer;
      ctl_ptr->size_current_buf  = num_bytes;
      ctl_ptr->index_current_buf = 0;
    } else if(ctl_ptr->reserve_buf == NULL) {
      ctl_ptr->reserve_buf       = buffer;
      ctl_ptr->size_reserve_buf  = num_bytes;
    }
    INTFREE();
  }
}
#endif /* FEATURE_PCM_REC */
