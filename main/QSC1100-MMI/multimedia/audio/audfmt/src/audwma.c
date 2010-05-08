/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         A U D I O   F O R M A T 
          W I N D O W S   M E D I A   A U D I O  S E R V I C E S 

GENERAL DESCRIPTION
  This module contains WindowsMediaAudio decoder driver software.

EXTERNALIZED FUNCTIONS
  audwma_format_play
    Use this function to play streaming WindowsMediaAudio.
  audwma_get_data_cb_req
    Callback function which handles the response to a Data request.
  audwma_get_sample_rate
    Returns the sampling rate of the current ra song.
  audwma_flush
    Flushes the buffers and restarts playback.
  audwma_reset
    Resets internal buffers after a song is unloaded.
  audwma_adec_adjust
    Adjusts adec buffers to reflect their newly used data.
  audwma_parse
    Parses one event of the file.  Prepares buffers for data transfer.
  audwma_age
    Checks to see if the buffers are ready for use.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audwma.c#2 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/08   anb     Added support for DSP based WMAPRO
07/19/05    pl     Added the support for setting the error status at the point 
                   where the error is encountered. The error status would 
                   eventually be relayed back to the upper layer to provide
                   more information for the aborted playback.
04/13/05    hz     Skip the null data packet during flush state.
04/11/05    hz     Skip the zero length data packet.
02/14/05    hz     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"    /* Target specific definitions                        */
#include "comdef.h"    /* Customer specific definitions                      */
#include "customer.h"  /* Customer specific definitions                      */
#ifdef FEATURE_WMA                                                           
#include "msg.h"       /* Message logging/reporting services                 */
#include "err.h"       /* Error logging/reporting services                   */
#include <memory.h>    /* Memory manipulation routines                       */
                                                                             
#include "audmain.h"   /* Audio Format Data definitions and protos           */
#include "audwma.h"    /* WindowsMediaAudio typedefs and prototypes          */
#include "audwmai.h"   /* WindowsMediaAudio internal typedefs and prototypes */

/* <EJECT> */
/*---------------------------------------------------------------------------
** General Defines
**---------------------------------------------------------------------------
*/
#define AUDWMA_MAX_FILE_LEN (uint32)(-1)

/* This structure contains all the information about the file
** that is being parsed.
*/
audwma_parse_ctl_type audwma_parse_ctl;

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudWMA Internal Function definitions
**-------------------------------------------------------------------------*/

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_supported_wma

DESCRIPTION
  This function checks the playback config. to verify whether it is supported.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the file is supported.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audwma_supported_wma
(
  snd_wma_format_spec_type *spec
)
{
  if ((spec->sample_rate < SND_SAMPLE_RATE_8000)  ||
      (spec->sample_rate > SND_SAMPLE_RATE_48000) ||
      (spec->channels < 1)                        ||
      (spec->channels > 2)                        ||
      (spec->version > 3)) {
    return (FALSE);
  }
    
  return (TRUE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_get_data

DESCRIPTION
  Initiates an asynchronous data request for the indicated buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void audwma_get_data(
  audwma_buf_type   *buf
)
{
  if (buf != NULL) {
    if (buf->loading == FALSE && buf->length == 0) {
      if (buf->data != NULL) {
        audmain_do_play_cb(audwma_parse_ctl.handle, SND_CMX_AF_FREE_BUF, 0,
                           (void *) buf->data);
        buf->data = NULL;
      }

      buf->start = audwma_parse_ctl.file_location;
      if (audwma_parse_ctl.file_location < audwma_parse_ctl.file_len) {
        (void) audmain_get_data(audwma_parse_ctl.handle,
                                audwma_parse_ctl.file_location,
                                (void*) &buf->data);
        buf->loading = TRUE;
        audwma_parse_ctl.file_location += audwma_parse_ctl.track_length;
      }
    }
  }
}

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDWMA External Function definitions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audwma_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
boolean audwma_get_data_cb_req(
  uint8  **data, 
  uint8  *buf, 
  uint32 length,
  uint32 pos
)
{
  audwma_buf_type *buffer, *other_buf;

  *data = buf;

  if((audwma_parse_ctl.state & AUDWMA_DATA_STATE_FLUSH) == 0) {

    if ((buf == NULL) || length == 0) {
      MSG_ERROR("No data received", 0,0,0);
      audwma_parse_ctl.state = AUDWMA_DATA_STATE_ERROR;
      audwma_parse_ctl.error_status = AUDMAIN_STATUS_DATA_ACCESS_ERR;
      return FALSE;
    }
  }

  /* Dump the data if we are in an error state */
  if (audwma_parse_ctl.state == AUDWMA_DATA_STATE_ERROR) {
    audmain_do_play_cb(audwma_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)buf);
    return FALSE;
  }

  if(buf == audwma_parse_ctl.buf1.data) {
    buffer    = &audwma_parse_ctl.buf1;
    other_buf = &audwma_parse_ctl.buf2;
  }
  else if(buf == audwma_parse_ctl.buf2.data) {
    buffer    = &audwma_parse_ctl.buf2;
    other_buf = &audwma_parse_ctl.buf1;
  }
  else {
    MSG_ERROR("Misplaced buffer!",0,0,0);
    audwma_parse_ctl.state = AUDWMA_DATA_STATE_ERROR;
    audwma_parse_ctl.error_status = AUDMAIN_STATUS_ERROR;
    return FALSE;
  }

  if(audwma_parse_ctl.state == AUDWMA_DATA_STATE_FLUSH) {
    audmain_do_play_cb(audwma_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)buffer->data);
    buffer->data    = NULL;
    buffer->start   = 0;
    buffer->length  = 0;
    buffer->loading = FALSE;

    if (!audwma_parse_ctl.buf1.loading && !audwma_parse_ctl.buf2.loading) {
      /* Get more data, now that all previous buffers have been flushed */
      audwma_get_data(&audwma_parse_ctl.buf1);
      audwma_get_data(&audwma_parse_ctl.buf2);
      audwma_parse_ctl.state = AUDWMA_DATA_STATE_LOADING;
    }
  }
  else {
    buffer->length  = length;
    buffer->loading = FALSE;
    audwma_parse_ctl.state = AUDWMA_DATA_STATE_READY;
  }

  return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_format_play

DESCRIPTION
  Use this function to play streaming WindowsMediaAudio.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
audmain_status_type audwma_format_play (
  snd_wma_format_spec_type    *input_spec,
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *parse_ctl
)
{

  if(!audwma_supported_wma(input_spec)) {
    MSG_ERROR("Unsupported features used in WMA stream",0,0,0);
    return(AUDMAIN_STATUS_ERROR);
  }

  /* Reset internal parameters */
  audwma_reset();

  /* Setup the ping-pong buffers and playback info */
  audwma_parse_ctl.spec          = *input_spec;
  audwma_parse_ctl.file_len      = AUDWMA_MAX_FILE_LEN;
  audwma_parse_ctl.file_location = parse_ctl->buffer_size;
  audwma_parse_ctl.handle        = handle;
  audwma_parse_ctl.state         = AUDWMA_DATA_STATE_READY;
  audwma_parse_ctl.track_length  = parse_ctl->buffer_size;
  audwma_parse_ctl.curr_buf      = &audwma_parse_ctl.buf1;
  audwma_parse_ctl.buf1.data     = buffer;
  audwma_parse_ctl.buf1.start    = 0;
  audwma_parse_ctl.buf1.length   = parse_ctl->buffer_size;
  audwma_parse_ctl.buf1.loading  = 0;
  audwma_parse_ctl.error_status  = AUDMAIN_STATUS_MAX;
  parse_ctl->tracks              = &audwma_parse_ctl;

  audmain_set_read_sync(FALSE);
  audwma_get_data(&audwma_parse_ctl.buf2);

  MSG_MED("Streaming WMA ready to go", 0, 0, 0);

  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current ra song.

DEPENDENCIES
  ra_info

RETURN VALUE
  sample rate of the file being played.

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audwma_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
)
{
  if ((parse_ctl != NULL) && (parse_ctl->tracks != NULL)) {
    return(((audwma_parse_ctl_type *)parse_ctl->tracks)->spec.sample_rate);
  }
  return(SND_SAMPLE_RATE_UNKNOWN);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES
  ra_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audwma_flush (
  audmain_parse_ctl_type   *parse_ctl
)
{
  audmain_cmd_buf_type *cmd_buf;
  audmain_status_type  ret_val = AUDMAIN_STATUS_ERROR;
  
  if(parse_ctl != NULL) {
    cmd_buf = &parse_ctl->current_buf;
    if (audwma_parse_ctl.state != AUDWMA_DATA_STATE_FLUSH) {

      cmd_buf->adec_cmd.adec_num_buf1 = cmd_buf->adec_cmd.adec_num_buf2 = 0;
      audwma_parse_ctl.buf1.length = 0;
      audwma_parse_ctl.buf2.length = 0;
   
      if (audwma_parse_ctl.buf1.loading || audwma_parse_ctl.buf2.loading) {
        audwma_parse_ctl.state = AUDWMA_DATA_STATE_FLUSH;
      }
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_reset

DESCRIPTION
  This function resets internal RA buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ra_info

===========================================================================*/
void audwma_reset ()
{
  if (audwma_parse_ctl.buf1.data != NULL) {
    audmain_do_play_cb(audwma_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)audwma_parse_ctl.buf1.data);
  }

  if (audwma_parse_ctl.buf2.data != NULL) {
    audmain_do_play_cb(audwma_parse_ctl.handle, SND_CMX_AF_FREE_BUF,
                       0, (void *)audwma_parse_ctl.buf2.data);
  }

  (void) memset(&audwma_parse_ctl, 0, sizeof(audwma_parse_ctl));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audwma_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
)
{
  uint32                  num_bytes;
  audwma_buf_type          *buf_low, *buf_high;
  uint32                  dummy;
  audmain_adec_cmd_type   *adec_cmd;
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;

  if (parse_ctl != NULL && audwma_parse_ctl.state != AUDWMA_DATA_STATE_ERROR) {
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

      if (audwma_parse_ctl.buf1.start < audwma_parse_ctl.buf2.start) {
        buf_low = &audwma_parse_ctl.buf1;
        buf_high = &audwma_parse_ctl.buf2;
      }
      else {
        buf_low = &audwma_parse_ctl.buf2;
        buf_high = &audwma_parse_ctl.buf1;
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
        audwma_age(parse_ctl, &dummy, &dummy);
      }
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audwma_parse

DESCRIPTION
  This function parses one event of the RA file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audwma_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audwma_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
)
{
  audmain_status_type    ret_val = AUDMAIN_STATUS_SUCCESS;
  audwma_buf_type         *buf_low, *buf_high;

  if (parse_ctl != NULL && cmd_buf != NULL) {
    switch (audwma_parse_ctl.state) {
      case AUDWMA_DATA_STATE_LOADING:
        /* Both ping-pong buffers are loading */
        break;

      case AUDWMA_DATA_STATE_READY:
        if (audwma_parse_ctl.file_location >= audwma_parse_ctl.file_len &&
            audwma_parse_ctl.buf1.data == NULL &&
            audwma_parse_ctl.buf2.data == NULL) {
          ret_val = AUDMAIN_STATUS_DONE;
        }
        else {
          if (audwma_parse_ctl.buf1.start < audwma_parse_ctl.buf2.start) {
            buf_low = &audwma_parse_ctl.buf1;
            buf_high = &audwma_parse_ctl.buf2;
          }
          else {
            buf_low = &audwma_parse_ctl.buf2;
            buf_high = &audwma_parse_ctl.buf1;
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
            if (audwma_parse_ctl.file_location >= audwma_parse_ctl.file_len) {
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

      case AUDWMA_DATA_STATE_FLUSH:
        /* Waiting for flush operation to complete */
        break;

      case AUDWMA_DATA_STATE_ERROR:
        ret_val = audwma_parse_ctl.error_status;
        break;

      default:
        MSG_FATAL("Illegal internal state", 0,0,0);
        audwma_parse_ctl.state = AUDWMA_DATA_STATE_ERROR;
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

FUNCTION audwma_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audwma_age (
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

  switch (audwma_parse_ctl.state) {
    case AUDWMA_DATA_STATE_LOADING:
      /* Both ping-pong buffers are loading */
      break;

    case AUDWMA_DATA_STATE_READY:
      /* At least one buffer is ready; try to load the other if necessary */
      audwma_get_data(&audwma_parse_ctl.buf1);
      audwma_get_data(&audwma_parse_ctl.buf2);
      if (audwma_parse_ctl.buf1.loading && audwma_parse_ctl.buf2.loading) {
        audwma_parse_ctl.state = AUDWMA_DATA_STATE_LOADING;
      }
      break;

    case AUDWMA_DATA_STATE_FLUSH:
      /* Waiting for flush operation to complete */
      break;

    case AUDWMA_DATA_STATE_ERROR:
      break;

    default:
      MSG_FATAL("Illegal internal state", 0,0,0);
      audwma_parse_ctl.state = AUDWMA_DATA_STATE_ERROR;
      audwma_parse_ctl.error_status = AUDMAIN_STATUS_ERROR;
      break;
  }
}

#endif /* FEATURE_WMA */
