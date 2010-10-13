/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("AUDFMT Services: RealAudio")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             A U D I O   F O R M A T s   R E A L  A U D I O   S E R V I C E S 

GENERAL DESCRIPTION
  This module contains RealAudio decoder driver software.

EXTERNALIZED FUNCTIONS
  audra_format_play
    Use this function to play streaming RealAudio.
  audra_get_data_cb_req
    Callback function which handles the response to a Data request.
  audra_get_sample_rate
    Returns the sampling rate of the current ra song.
  audra_flush
    Flushes the buffers and restarts playback.
  audra_reset
    Resets internal buffers after a song is unloaded.
  audra_adec_adjust
    Adjusts adec buffers to reflect their newly used data.
  audra_parse
    Parses one event of the file.  Prepares buffers for data transfer.
  audra_age
    Checks to see if the buffers are ready for use.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audra.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/05    lg     Removed check for 0 length buffers, NULL buffers indicate error.
07/19/05    pl     Added the support for setting the error status at the point 
                   where the error is encountered. The error status would 
                   eventually be relayed back to the upper layer to provide
                   more information for the aborted playback.
11/23/03    sg     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"         /* Target specific definitions                */
#include "comdef.h"         /* Customer specific definitions              */
#include "customer.h"       /* Customer specific definitions              */
#ifdef FEATURE_REAL_AUDIO
#include "msg.h"            /* Message logging/reporting services         */
#include "err.h"            /* Error logging/reporting services           */
#include <memory.h>         /* Memory manipulation routines               */

#include "audmain.h"        /* Audio Format Data definitions and protos   */
#include "audra.h"          /* RealAudio typedefs and prototypes          */
#include "audrai.h"         /* RealAudio internal typedefs and prototypes */
#include "snd.h"            /* Sound layer typedefs                       */

/* <EJECT> */
/*---------------------------------------------------------------------------
** General Defines
**---------------------------------------------------------------------------
*/
#define AUDRA_MAX_FILE_LEN (uint32)(-1)

/* This structure contains all the information about the file
** that is being parsed.
*/
audra_parse_ctl_type audra_parse_ctl;

/* audra_info contains all relevant information about the file being played
** currently
*/
audra_info_type audra_info;

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudRA Internal Function definitions
**---------------------------------------------------------------------------
*/

/* <EJECT> */
/*===========================================================================

FUNCTION audra_supported_ra

DESCRIPTION
  This function checks the playback config. to verify whether it is supported.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the file is supported.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audra_supported_ra(void)
{
  return
  (
    (audra_info.num_channels == SND_RA_CHANNEL_MONO ||
     audra_info.num_channels == SND_RA_CHANNEL_DUAL) &&

    (audra_info.decoder_mode == SND_RA_DECODER_GECKO1 ||
     audra_info.decoder_mode == SND_RA_DECODER_GECKO2) &&

    (audra_info.sample_rate >= SND_SAMPLE_RATE_8000 &&
     audra_info.sample_rate <= SND_SAMPLE_RATE_48000) &&

    (audra_info.samples_per_frame == SND_RA_SAMPLES_PER_FRAME_256 ||
     audra_info.samples_per_frame == SND_RA_SAMPLES_PER_FRAME_512 ||
     audra_info.samples_per_frame == SND_RA_SAMPLES_PER_FRAME_1024)
  );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_get_data

DESCRIPTION
  Initiates an asynchronous data request for the indicated buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void audra_get_data(
  audra_buf_type   *buf
)
{
  if (buf != NULL) {
    if (buf->loading == FALSE && buf->length == 0) {
      if (buf->data != NULL) {
        audmain_do_play_cb(audra_parse_ctl.handle, SND_CMX_AF_FREE_BUF, 0,
                           (void *) buf->data);
        buf->data = NULL;
      }

      buf->start = audra_parse_ctl.file_location;
      if (audra_parse_ctl.file_location < audra_parse_ctl.file_len) {
        (void) audmain_get_data(audra_parse_ctl.handle,
                                audra_parse_ctl.file_location,
                                (void*) &buf->data);
        buf->loading = TRUE;
        audra_parse_ctl.file_location += audra_parse_ctl.track_length;
      }
    }
  }
}

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDRA External Function definitions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audra_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
boolean audra_get_data_cb_req(
  uint8  **data, 
  uint8  *buf, 
  uint32 length,
  uint32 pos
)
{
  audra_buf_type *buffer, *other_buf;

  *data = buf;

  if (buf == NULL) {
    MSG_ERROR("No data received", 0,0,0);
    audra_parse_ctl.state = AUDRA_DATA_STATE_ERROR;
    audra_parse_ctl.error_status = AUDMAIN_STATUS_DATA_ACCESS_ERR;
    return FALSE;
  }

  /* Dump the data if we are in an error state */
  if (audra_parse_ctl.state == AUDRA_DATA_STATE_ERROR) {
    audmain_do_play_cb(audra_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)buf);
    return FALSE;
  }

  if(buf == audra_parse_ctl.buf1.data) {
    buffer    = &audra_parse_ctl.buf1;
    other_buf = &audra_parse_ctl.buf2;
  }
  else if(buf == audra_parse_ctl.buf2.data) {
    buffer    = &audra_parse_ctl.buf2;
    other_buf = &audra_parse_ctl.buf1;
  }
  else {
    MSG_ERROR("Misplaced buffer!",0,0,0);
    audra_parse_ctl.state = AUDRA_DATA_STATE_ERROR;
    audra_parse_ctl.error_status = AUDMAIN_STATUS_ERROR;
    return FALSE;
  }

  if(audra_parse_ctl.state == AUDRA_DATA_STATE_FLUSH) {
    audmain_do_play_cb(audra_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)buffer->data);
    buffer->data    = NULL;
    buffer->start   = 0;
    buffer->length  = 0;
    buffer->loading = FALSE;

    if (!audra_parse_ctl.buf1.loading && !audra_parse_ctl.buf2.loading) {
      /* Get more data, now that all previous buffers have been flushed */
      audra_get_data(&audra_parse_ctl.buf1);
      audra_get_data(&audra_parse_ctl.buf2);
      audra_parse_ctl.state = AUDRA_DATA_STATE_LOADING;
    }
  }
  else {
    buffer->length  = length;
    buffer->loading = FALSE;
    audra_parse_ctl.state = AUDRA_DATA_STATE_READY;
  }

  return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_format_play

DESCRIPTION
  Use this function to play streaming RealAudio.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
audmain_status_type audra_format_play (
  snd_ra_format_spec_type     *input_spec,
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *parse_ctl
)
{
  if(input_spec->codec_type.file_type != SND_CMX_AF_FILE_RA) {
    MSG_MED("Not RealAudio data",0,0,0);
    return(AUDMAIN_STATUS_ERROR);
  }

  /* Reset internal parameters */
  audra_reset();

  audra_info.decoder_mode      = input_spec->decoder_mode;
  audra_info.sample_rate       = input_spec->sample_rate;
  audra_info.num_channels      = input_spec->num_channels;
  audra_info.samples_per_frame = input_spec->samples_per_frame;
  audra_info.num_regions       = input_spec->num_regions;
  audra_info.bits_per_frame    = input_spec->bits_per_frame;
  audra_info.coupling_start    = input_spec->coupling_start;
  audra_info.coupling_quant    = input_spec->coupling_quant;
  audra_info.frame_erasure     = input_spec->frame_erasure;

  if(!audra_supported_ra()) {
    MSG_ERROR("Unsupported features used in RealAudio stream",0,0,0);
    return(AUDMAIN_STATUS_ERROR);
  }

  /* Setup the ping-pong buffers */
  audra_parse_ctl.file_len      = AUDRA_MAX_FILE_LEN;
  audra_parse_ctl.file_location = parse_ctl->buffer_size;
  audra_parse_ctl.handle        = handle;
  audra_parse_ctl.state         = AUDRA_DATA_STATE_READY;
  audra_parse_ctl.track_length  = parse_ctl->buffer_size;
  audra_parse_ctl.curr_buf      = &audra_parse_ctl.buf1;
  audra_parse_ctl.buf1.data     = buffer;
  audra_parse_ctl.buf1.start    = 0;
  audra_parse_ctl.buf1.length   = parse_ctl->buffer_size;
  audra_parse_ctl.buf1.loading  = 0;
  audra_parse_ctl.error_status  = AUDMAIN_STATUS_MAX;
  audmain_set_read_sync(FALSE);
  audra_get_data(&audra_parse_ctl.buf2);

  MSG_HIGH("Streaming RealAudio ready to go", 0, 0, 0);
  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current ra song.

DEPENDENCIES
  ra_info

RETURN VALUE
  sample rate of the file being played.

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audra_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
)
{
  return(audra_info.sample_rate);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES
  ra_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audra_flush (
  audmain_parse_ctl_type   *parse_ctl
)
{
  audmain_cmd_buf_type *cmd_buf;
  audmain_status_type  ret_val = AUDMAIN_STATUS_ERROR;
  
  if(parse_ctl != NULL) {
    cmd_buf = &parse_ctl->current_buf;
    if (audra_parse_ctl.state != AUDRA_DATA_STATE_FLUSH) {

      cmd_buf->adec_cmd.adec_num_buf1 = cmd_buf->adec_cmd.adec_num_buf2 = 0;
      audra_parse_ctl.buf1.length = 0;
      audra_parse_ctl.buf2.length = 0;
   
      if (audra_parse_ctl.buf1.loading || audra_parse_ctl.buf2.loading) {
        audra_parse_ctl.state = AUDRA_DATA_STATE_FLUSH;
      }
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_reset

DESCRIPTION
  This function resets internal RA buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ra_info

===========================================================================*/
void audra_reset ()
{
  if (audra_parse_ctl.buf1.data != NULL) {
    audmain_do_play_cb(audra_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)audra_parse_ctl.buf1.data);
  }

  if (audra_parse_ctl.buf2.data != NULL) {
    audmain_do_play_cb(audra_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)audra_parse_ctl.buf2.data);
  }

  (void) memset(&audra_parse_ctl, 0, sizeof(audra_parse_ctl));
  (void) memset(&audra_info, 0, sizeof(audra_info));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audra_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
)
{
  uint32                  num_bytes;
  audra_buf_type          *buf_low, *buf_high;
  uint32                  dummy;
  audmain_adec_cmd_type   *adec_cmd;
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;

  if (parse_ctl != NULL && audra_parse_ctl.state != AUDRA_DATA_STATE_ERROR) {
    if (*bytes_used != 0) {

      num_bytes = *bytes_used;

      adec_cmd = &parse_ctl->current_buf.adec_cmd;

      if (num_bytes > adec_cmd->adec_num_buf1) {
        num_bytes -= adec_cmd->adec_num_buf1;
        adec_cmd->adec_buf1     += adec_cmd->adec_num_buf1;
        adec_cmd->adec_num_buf1 = 0;
        adec_cmd->adec_buf2     += num_bytes;
        adec_cmd->adec_num_buf2 -= num_bytes;
      }
      else {
        adec_cmd->adec_buf1 += num_bytes;
        adec_cmd->adec_num_buf1 -= num_bytes;
      }

      if (audra_parse_ctl.buf1.start < audra_parse_ctl.buf2.start) {
        buf_low = &audra_parse_ctl.buf1;
        buf_high = &audra_parse_ctl.buf2;
      }
      else {
        buf_low = &audra_parse_ctl.buf2;
        buf_high = &audra_parse_ctl.buf1;
      }

      if (adec_cmd->adec_num_buf1 == 0) {
        buf_low->length = 0;
      }
      if (adec_cmd->adec_num_buf2 == 0) {
        buf_high->length = 0;
      }

      /* Try to reload the buffers if they are empty. Otherwise we'll have to
       * wait until the next age event to trigger a reload.
       * This can help alleviate client side data delays.
       */
      if (!buf_low->length || !buf_high->length) {
        audra_age(parse_ctl, &dummy, &dummy);
      }
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_parse

DESCRIPTION
  This function parses one event of the RA file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audra_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audra_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
)
{
  audmain_status_type    ret_val = AUDMAIN_STATUS_SUCCESS;
  audra_buf_type         *buf_low, *buf_high;

  if (parse_ctl != NULL && cmd_buf != NULL) {
    switch (audra_parse_ctl.state) {
      case AUDRA_DATA_STATE_LOADING:
        /* Both ping-pong buffers are loading */
        break;

      case AUDRA_DATA_STATE_READY:
        if (audra_parse_ctl.file_location >= audra_parse_ctl.file_len &&
            audra_parse_ctl.buf1.data == NULL &&
            audra_parse_ctl.buf2.data == NULL) {
          ret_val = AUDMAIN_STATUS_DONE;
        }
        else {
          if (audra_parse_ctl.buf1.start < audra_parse_ctl.buf2.start) {
            buf_low = &audra_parse_ctl.buf1;
            buf_high = &audra_parse_ctl.buf2;
          }
          else {
            buf_low = &audra_parse_ctl.buf2;
            buf_high = &audra_parse_ctl.buf1;
          }

          if (cmd_buf->adec_cmd.adec_num_buf1 == 0) {
            /* First buffer used up, refresh with new data */
            if (cmd_buf->adec_cmd.adec_num_buf2 == 0) {
              /* Second buffer used, move new data from tracks */
              cmd_buf->adec_cmd.adec_buf1 = buf_low->data;
              cmd_buf->adec_cmd.adec_num_buf1 = buf_low->length;

              if (cmd_buf->adec_cmd.adec_buf1 == NULL) {
                cmd_buf->adec_cmd.adec_num_buf1 = 0;
                *time_remain = 250;
                MSG_HIGH("Buffer underflow.", 0,0,0);
                ret_val = AUDMAIN_STATUS_BUFF_ERR;
              }
            }
            else {
              /* Second buffer partially used, move new data from 2nd buffer */
              cmd_buf->adec_cmd.adec_num_buf1 = cmd_buf->adec_cmd.adec_num_buf2;
              cmd_buf->adec_cmd.adec_buf1 = cmd_buf->adec_cmd.adec_buf2;
              /* Kill second buffer */
              cmd_buf->adec_cmd.adec_num_buf2 = 0;
              cmd_buf->adec_cmd.adec_buf2 = NULL;
            }
          }

          if (cmd_buf->adec_cmd.adec_num_buf2 == 0) {
            /* Second buffer used, setup buffer */
            cmd_buf->adec_cmd.adec_buf2 = buf_high->data;
            cmd_buf->adec_cmd.adec_num_buf2 = buf_high->length;
            if (cmd_buf->adec_cmd.adec_buf2 == NULL) {
              cmd_buf->adec_cmd.adec_num_buf2 = 0;
            }
          }

          if (cmd_buf->adec_cmd.adec_num_buf1 +
             cmd_buf->adec_cmd.adec_num_buf2 == 1) {
            if (audra_parse_ctl.file_location >= audra_parse_ctl.file_len) {
              /* We'll never be able to write this one odd byte, since the DSP
               * can only handle words.
               */
              cmd_buf->adec_cmd.adec_num_buf1 = 0;
              cmd_buf->adec_cmd.adec_num_buf2 = 0;
              ret_val = AUDMAIN_STATUS_DONE;
            }
            else {
              MSG_HIGH("Buffer underflow.", 0,0,0);
              ret_val = AUDMAIN_STATUS_BUFF_ERR;
            }
          }
        }
        break;

      case AUDRA_DATA_STATE_FLUSH:
        /* Waiting for flush operation to complete */
        break;

      case AUDRA_DATA_STATE_ERROR:
        ret_val = audra_parse_ctl.error_status;
        break;

      default:
        MSG_FATAL("Illegal internal state", 0,0,0);
        audra_parse_ctl.state = AUDRA_DATA_STATE_ERROR;
        ret_val = AUDMAIN_STATUS_ERROR;
        break;
    }
  }
  else {
    MSG_FATAL("NULL buffer pointer", 0,0,0);
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return ret_val;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audra_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audra_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
) 
{
  if (parse_ctl->status_flag & AUDMAIN_MASK_INIT_DONE) {
    *adjusted_time = *elapsed_time;
  }
  else {
    *elapsed_time  = 0;
    *adjusted_time = 0;
  }

  switch (audra_parse_ctl.state) {
    case AUDRA_DATA_STATE_LOADING:
      /* Both ping-pong buffers are loading */
      break;

    case AUDRA_DATA_STATE_READY:
      /* At least one buffer is ready; try to load the other if necessary */
      audra_get_data(&audra_parse_ctl.buf1);
      audra_get_data(&audra_parse_ctl.buf2);
      if (audra_parse_ctl.buf1.loading && audra_parse_ctl.buf2.loading) {
        audra_parse_ctl.state = AUDRA_DATA_STATE_LOADING;
      }
      break;

    case AUDRA_DATA_STATE_FLUSH:
      /* Waiting for flush operation to complete */
      break;

    case AUDRA_DATA_STATE_ERROR:
      break;

    default:
      MSG_FATAL("Illegal internal state", 0,0,0);
      audra_parse_ctl.state = AUDRA_DATA_STATE_ERROR;
      audra_parse_ctl.error_status = AUDMAIN_STATUS_ERROR;
      break;
  }
}

#endif /* FEATURE_REAL_AUDIO */
