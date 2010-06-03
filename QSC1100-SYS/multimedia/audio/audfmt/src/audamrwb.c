/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("AUDFMT Services: AMR WB/WB+")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      A U D I O   F O R M A T s   A M R - W B / W B +   S E R V I C E S

GENERAL DESCRIPTION
  This module contains AMR-WB/WB+ decoder driver software.

EXTERNALIZED FUNCTIONS
  audamrwb_format_play
    Use this function to start streaming playback.
  audamrwb_parse_head
    Parses the file header for relevant infomation.
  audamrwb_get_data_cb_req
    Callback function which handles the response to a data request.
  audamrwb_get_sample_rate
    Returns the sample rate of the current ra song.
  audamrwb_flush
    Flushes the buffers and restarts playback.
  audamrwb_fforward
    Moves the file playback forward ms milliseconds.
  audamrwb_rewind
    Moves the file playback backward ms milliseconds.
  audamrwb_reset
    Resets internal buffers after a song is unloaded.
  audamrwb_get_length
    Returns the total duration of the selected file.
  audamrwb_get_song_config
    Returns song-specific information to configure the DSP for playback.
  audamrwb_adec_adjust
    Adjusts to the next set of ADEC buffers after data consumption.
  audamrwb_parse
    Parses one event of the file.  Prepares buffers for data transfer.
  audamrwb_age
    Checks to see if the buffers are ready for use.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audamrwb.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
6/11/07     at     Modified AMR-WB and AMR-WB+ stream parsing state
                   machines to account for the frame headers in the avsync
                   byte count reported to the DSP.
02/06/07    at     Added AMR-WB stream support.
09/08/06    ay     Fixed a bug in audamrwb_dsp_pkt_hdr_set_len() to allow
                   10-bit DSP packet lengths.
                   Removed the export of the internal frame length look-up
                   table.
09/04/06    ay     Initial version.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"         /* Target specific definitions                 */
#include "comdef.h"         /* Customer specific definitions               */
#include "customer.h"       /* Customer specific definitions               */

#if defined(FEATURE_AMR_WB_AUDIO_DEC) || defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
#include <stdlib.h>
#include <limits.h>
#include <memory.h>         /* Memory manipulation routines                */
#include "msg.h"            /* Message logging/reporting services          */
#include "err.h"            /* Error logging/reporting services            */

#include "audmain.h"        /* Audio Format definitions and prototypes     */
#include "audamrwb.h"       /* AMR WB/WB+ definitions and prototypes       */
#include "audamrwbi.h"      /* AMR WB/WB+ internal defs. and prototypes    */
#include "amrsup.h"         /* AMR support funtions                        */
#include "snd.h"            /* Sound layer typedefs                        */

/* <EJECT> */
/*---------------------------------------------------------------------------
** General Definitions
**---------------------------------------------------------------------------
*/

/* The track length is defined as a very large unsigned number. The file or
** stream should end once this limit is reached.
*/
#define AUDAMRWB_MAX_TRACK_LEN ((uint32) -1)

/* The minimum amount of time (ms) to wait between data requests in CMX buffer
** underrun conditions. This delay helps prevent the watchdog from triggering
** when it is anticipated that the client would send consecutively low number
** of bytes to the data callback. The default is 250ms.
*/
#define AUDAMRWB_CMX_REQ_DELAY_MS (250)

/* The minimum amount of data (bytes) that must be received to allow early
** CMX buffer parsing in the data callback handler. A value too low could
** cause the dog timer to trigger when the client supplies small amounts of
** data too fast. The default is one superframe of the maximum bitrate.
*/
#define AUDAMRWB_CMX_EARLY_PARSE_THRESH (AUDAMRWB_AWP_MAX_SFRM_LEN)

/* Enable A/V SYNC support for AMR-WB playback.
*/
#define AUDAMRWB_AMR_WB_AUDIO_DEC_AV_SYNC

/* Enable A/V SYNC support for AMR-WB+ playback.
*/
#define AUDAMRWB_AMR_WBPLUS_AUDIO_DEC_AV_SYNC

/* This is the global parse control structure for AMR-WB/WB+ */
audamrwb_parse_ctl_type audamrwb_parse_ctl;

/* The frame length look-up table */
STATIC uint8 audamrwb_frm_data_len_table[] = {
  /* FT=0, AMR-WB 6.60 kbit/s */
  17,
  /* FT=1, AMR-WB 8.85 kbit/s */
  23,
  /* FT=2, AMR-WB 12.65 kbit/s */
  32,
  /* FT=3, AMR-WB 14.25 kbit/s */
  36,
  /* FT=4, AMR-WB 15.85 kbit/s */
  40,
  /* FT=5, AMR-WB 18.25 kbit/s */
  46,
  /* FT=6, AMR-WB 19.85 kbit/s */
  50,
  /* FT=7, AMR-WB 23.05 kbit/s */
  58,
  /* FT=8, AMR-WB 23.85 kbit/s */
  60,
  /* FT=9, AMR-WB SID */
  5,
  /* FT=10, AMR-WB+ 13.6 kbit/s mono */
  34,
  /* FT=11, AMR-WB+ 18 kbit/s stereo */
  45,
  /* FT=12, AMR-WB+ 24 kbit/s mono */
  60,
  /* FT=13, AMR-WB+ 24 kbit/s stereo */
  60,
  /* FT=14, FRAME_ERASURE */
  0,
  /* FT=15, NO_DATA */
  0,
  /* FT=16, AMR-WB+ 10.4 kbit/s */
  26,
  /* FT=17, AMR-WB+ 12.0 kbit/s */
  30,
  /* FT=18, AMR-WB+ 13.6 kbit/s */
  34,
  /* FT=19, AMR-WB+ 15.2 kbit/s */
  38,
  /* FT=20, AMR-WB+ 16.8 kbit/s */
  42,
  /* FT=21, AMR-WB+ 19.2 kbit/s */
  48,
  /* FT=22, AMR-WB+ 20.8 kbit/s */
  52,
  /* FT=23, AMR-WB+ 24.0 kbit/s */
  60,
  /* FT=24, AMR-WB+ 12.4 kbit/s */
  31,
  /* FT=25, AMR-WB+ 12.8 kbit/s */
  32,
  /* FT=26, AMR-WB+ 14 kbit/s */
  35,
  /* FT=27, AMR-WB+ 14.4 kbit/s */
  36,
  /* FT=28, AMR-WB+ 15.2 kbit/s */
  38,
  /* FT=29, AMR-WB+ 16 kbit/s */
  40,
  /* FT=30, AMR-WB+ 16.4 kbit/s */
  41,
  /* FT=31, AMR-WB+ 17.2 kbit/s */
  43,
  /* FT=32, AMR-WB+ 18 kbit/s */
  45,
  /* FT=33, AMR-WB+ 18.4 kbit/s */
  46,
  /* FT=34, AMR-WB+ 19.2 kbit/s */
  48,
  /* FT=35, AMR-WB+ 20 kbit/s */
  50,
  /* FT=36, AMR-WB+ 20.4 kbit/s */
  51,
  /* FT=37, AMR-WB+ 21.2 kbit/s */
  53,
  /* FT=38, AMR-WB+ 22.4 kbit/s */
  56,
  /* FT=39, AMR-WB+ 23.2 kbit/s */
  58,
  /* FT=40, AMR-WB+ 24 kbit/s */
  60,
  /* FT=41, AMR-WB+ 25.6 kbit/s */
  64,
  /* FT=42, AMR-WB+ 26 kbit/s */
  65,
  /* FT=43, AMR-WB+ 26.8 kbit/s */
  67,
  /* FT=44, AMR-WB+ 28.8 kbit/s */
  72,
  /* FT=45, AMR-WB+ 29.6 kbit/s */
  74,
  /* FT=46, AMR-WB+ 30 kbit/s */
  75,
  /* FT=47, AMR-WB+ 32 kbit/s */
  80
};

#if defined(AUDAMRWB_DBG_CAPTURE_CMX_PKTS) && defined(FEATURE_EFS)
static char audamrwb_dbg_cmx_dumpfile[FS_FILENAME_MAX_LENGTH_P] =
              "/audamrwb_in_cmx.bin";
#endif /* AUDAMRWB_DBG_CAPTURE_CMX_PKTS && FEATURE_EFS */

#if defined(AUDAMRWB_DBG_CAPTURE_ADEC_PKTS) && defined(FEATURE_EFS)
static char audamrwb_dbg_adec_dumpfile[FS_FILENAME_MAX_LENGTH_P] =
              "/audamrwb_out_adec.bin";
#endif /* AUDAMRWB_DBG_CAPTURE_ADEC_PKTS && FEATURE_EFS */

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Helper Functions
**---------------------------------------------------------------------------
*/

#if ((defined(AUDAMRWB_DBG_CAPTURE_CMX_PKTS) ||   \
      defined(AUDAMRWB_DBG_CAPTURE_ADEC_PKTS)) && \
     defined(FEATURE_EFS))
/*===========================================================================

FUNCTION audamrwb_dbg_io_write

DESCRIPTION
  Creates/appends source data to a file for diagnostic purposes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audamrwb_dbg_io_write (
  char*  file_name,
  uint8* source,
  uint32 length
) {
  fs_rsp_msg_type rsp_open;
  fs_rsp_msg_type rsp_write;
  fs_rsp_msg_type rsp_close;

  /* Create the output file if it doesn't exist */
  fs_open(file_name, FS_OA_APPEND, NULL, NULL, &rsp_open);
  if (rsp_open.open.status != FS_OKAY_S) {
    fs_open(file_name, FS_OA_CREATE, NULL, NULL, &rsp_open);
  }

  if (rsp_open.open.status == FS_OKAY_S) {
    fs_write(rsp_open.open.handle, source, length, NULL, &rsp_write);
  } else {
    MSG_ERROR("dbg_io_write: couldn't write to file", 0, 0, 0);
  }

  fs_close(rsp_open.open.handle, NULL, &rsp_close);
} /* audamrwb_dbg_io_write */

/* <EJECT> */
#endif /* (AUDAMRWB_DBG_CAPTURE_CMX_PKTS || AUDAMRWB_DBG_CAPTURE_ADEC_PKTS) */
       /* && FEATURE_EFS                                                    */
/*===========================================================================

FUNCTION audamrwb_cmx_is_awb_num_frames_valid

DESCRIPTION
  Determines if the indicated number of frames value for AMR-WB streams in a
  CMX packet is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_is_awb_num_frames_valid (
  uint16 num_frames
) {
  return ((0 < num_frames) && (num_frames <= AUDAMRWB_3GP_AWB_MAX_FPS));
} /* audamrwb_cmx_is_awb_num_frames_valid */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_is_awp_num_frames_valid

DESCRIPTION
  Determines if the indicated number of frames value for AMR-WB+ streams in a
  CMX packet is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_is_awp_num_frames_valid (
  uint16 num_frames
) {
  return (num_frames == 0);
} /* audamrwb_cmx_is_awp_num_frames_valid */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wb_frame

DESCRIPTION
  Determines if the indicated frame type for an AMR-WB frame is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wb_frame (
  uint8 frame_type
) {
  return ((frame_type <= 9) || ((14 <= frame_type) && (frame_type <= 15)));
} /* audamrwb_amr_is_wb_frame */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_mixed_frame

DESCRIPTION
  Determines if the indicated frame type for an AMR-WB/WB+ mixed-mode frame is
  valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_mixed_frame (
  uint8 frame_type
) {
  return (frame_type <= 15);
} /* audamrwb_amr_is_mixed_frame */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wbp_frame

DESCRIPTION
  Determines if the indicated frame type for an AMR-WB+ frame is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wbp_frame (
  uint8 frame_type
) {
  return ((16 <= frame_type) && (frame_type <= 47));
} /* audamrwb_amr_is_wbp_frame */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_valid_frame

DESCRIPTION
  Determines if the indicated frame type is any of the valid frame types.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_valid_frame (
  uint8 frame_type
) {
  return (frame_type <= 47);
} /* audamrwb_amr_is_valid_frame */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wb_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for
  AMR-WB.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wb_isf (
  uint8 isf
) {
  return (isf == 0);
} /* audamrwb_amr_is_wb_isf */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_mixed_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for
  AMR-WB/WB+ mixed mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_mixed_isf (
  uint8 isf
) {
  return (isf == 0);
} /* audamrwb_amr_is_mixed_isf */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_wbp_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for
  AMR-WB+.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_wbp_isf (
  uint8 isf
) {
  return (isf <= 13);
} /* audamrwb_amr_is_wbp_isf */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_amr_is_valid_isf

DESCRIPTION
  Determines if the indicated internal sampling frequency is valid for any of
  the valid frame types.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If valid.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_amr_is_valid_isf (
  uint8 isf
) {
  return (isf <= 13);
} /* audamrwb_amr_is_valid_isf */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_calc_frame_data_len

DESCRIPTION
  Calculates the number of bytes of frame data given a frame type.

DEPENDENCIES
  None

RETURN VALUE
  uint16 - The frame length in bytes.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint16 audamrwb_calc_frame_data_len (
  uint8 frame_type
) {
  if (frame_type <= 47) {
    if (audamrwb_amr_is_wb_frame(frame_type)) {
      return audamrwb_frm_data_len_table[frame_type];
    } else {
      /* AMR-WB+ frames are grouped in superframe storage units */
      return (AUDAMRWB_AWP_FPS * audamrwb_frm_data_len_table[frame_type]);
    }
  } else {
    return 0;
  }
} /* audamrwb_calc_frame_data_len */

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal CMX Buffer Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_cmx_free_data

DESCRIPTION
  Frees the data pointer of a buffer. NULL pointers are acceptable.

DEPENDENCIES
  None

RETURN VALUE
  uint8* - The new pointer of the freed buffer.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint8* audamrwb_cmx_free_data (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   data_ptr
) {
  if (data_ptr != NULL) {
    audmain_do_play_cb(parse_ctl->cmx_ctl.handle, SND_CMX_AF_FREE_BUF, 0,
                       ((void*)data_ptr));
  }
  return NULL;
} /* audamrwb_cmx_free_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_free

DESCRIPTION
  Frees indicated buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The indicated buffer is initialized for the next data request.

===========================================================================*/
STATIC void audamrwb_cmx_free (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_cmx_buf_type   *buffer
) {
  buffer->loading   = FALSE;
  buffer->track_pos = 0;
  buffer->data_ptr  = audamrwb_cmx_free_data(parse_ctl, buffer->data_ptr);
  buffer->data_len  = 0;
  buffer->data_idx  = 0;
} /* audamrwb_cmx_free */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_is_free

DESCRIPTION
  Determines if the indicated buffer is no longer in use.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - The indicated CMX buffer is no longer in use.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_is_free (
  audamrwb_cmx_buf_type *buffer
) {
  return (buffer->data_ptr == NULL);
} /* audamrwb_cmx_is_free */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_can_recycle

DESCRIPTION
  Determines if the indicated buffer can be recycled/freed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - The indicated CMX buffer can be recycled.
  FALSE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_cmx_can_recycle (
  audamrwb_cmx_buf_type *buffer
) {
  return ((buffer->loading == FALSE) && (buffer->data_len == 0));
} /* audamrwb_cmx_can_recycle */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_request

DESCRIPTION
  Initiates an asynchronous CMX data request for the indicated buffer.

  Only one CMX request is allowed at any time! The track_pos is correct when
  data is actually received by audamrwb_get_data_cb_req(). If two consecutive
  CMX requests were allowed, then both would incorrectly receive the same
  track position.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - Sent the CMX data request.
  AUDMAIN_STATUS_ERROR   - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
STATIC audmain_status_type audamrwb_cmx_request (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_cmx_buf_type   *buffer
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  /* Load the next chunk of data if the given buffer can be reclaimed */
  if ((buffer != NULL) && audamrwb_cmx_is_free(buffer)) {
    /* Read in the next chunk of data if not past the end of track */
    if (parse_ctl->cmx_ctl.track_pos < parse_ctl->cmx_ctl.track_len) {
      buffer->loading   = TRUE;
      buffer->track_pos = 0;

      /* Update the CMX buffering state */
      if (parse_ctl->cmx_ctl.cmx_state != AUDAMRWB_CMX_BUF_STATE_UNDERRUN) {
        parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_LOADING;
      }

      /* The state of the given buffer at this point before
      ** audmain_get_data() must be:
      **
      **   buffer->loading   = TRUE
      **   buffer->track_pos = 0
      **   buffer->data_ptr  = NULL
      **   buffer->data_len  = 0
      **   buffer->data_idx  = 0
      */
      if (audmain_get_data(parse_ctl->cmx_ctl.handle,
                           parse_ctl->cmx_ctl.track_pos,
                           &buffer->data_ptr) != AUDMAIN_STATUS_SUCCESS) {
        MSG_ERROR("cmx_request: Get data request failed", 0, 0, 0);
      } else {
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
    }
  }

  return ret_val;
} /* audamrwb_cmx_request */

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Circular Packet Buffer Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_dsp_pkt_hdr_set_len

DESCRIPTION
  Sets the length field of the DSP packet header.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
INLINE void audamrwb_dsp_pkt_hdr_set_len (
  audamrwb_dsp_pkt_hdr_type* dsp_pkt_hdr,
  uint32                     length
) {
  /* Set the length aligned to bytes (8 bits) */
  dsp_pkt_hdr->length_low  = ((uint8)length);
  dsp_pkt_hdr->length_high = ((length >> 8) & 0x3);
} /* audamrwb_dsp_pkt_hdr_set_len */

/*===========================================================================

FUNCTION audamrwb_dsp_pkt_hdr_get_len

DESCRIPTION
  Gets the length field of the DSP packet header.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The length of the packet payload.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_dsp_pkt_hdr_get_len (
  audamrwb_dsp_pkt_hdr_type* dsp_pkt_hdr
) {
  /* The return length aligned to bytes (8 bits) */
  return ((dsp_pkt_hdr->length_high << 8) | (dsp_pkt_hdr->length_low));
} /* audamrwb_dsp_pkt_hdr_get_len */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_calc_base_offset

DESCRIPTION
  Calculates the distance between the tail of the indicated <pointer, length>
  pair and the base of the circular packet buffer.

  This function is primarily used in buffer wrap-around conditions for read
  and write operations.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The distance in bytes.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_calc_base_offset (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint8*                 cur_ptr,
  uint32                 length
) {
  return ((uint32)(cur_ptr - pkt_ctl->buf_ptr) + length);
} /* audamrwb_pkt_calc_base_offset */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_calc_offset_ptr

DESCRIPTION
  Calculates the pointer, within the circular packet buffer, that is relative
  to the beginning of the current starting position.

  The offset value is the number of bytes relative to the current starting
  position of the circular packet buffer. The offset value must be equal or
  less than the circular packet buffer size.

DEPENDENCIES
  None

RETURN VALUE
  uint8* - The calculated pointer.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint8* audamrwb_pkt_calc_offset_ptr (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset
) {
  uint32 base_offset;

  /* Calculate the overshoot length */
  base_offset = audamrwb_pkt_calc_base_offset(pkt_ctl, pkt_ctl->ready_ptr,
                                              offset);

  /* Wraparound the overflowed address space to the start position */
  if (base_offset < pkt_ctl->buf_size) {
    return (pkt_ctl->ready_ptr + offset);
  } else {
    /* Approximate the modulus operation */
    return (pkt_ctl->buf_ptr + (base_offset - pkt_ctl->buf_size));
  }
} /* audamrwb_pkt_calc_offset_ptr */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_calc_linear_len

DESCRIPTION
  Calculates the maximum linear length accessible without causing or
  requiring address wrap-around.

  The offset value is the number of bytes relative to the current starting
  position of the circular packet buffer. The offset value must be equal or
  less than the circular packet buffer size.

  This function is primarily used to determine the length of the head of the
  current buffer. The remaining length is part of the tail which begins from
  the base of the circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The length of the head of the current buffer.

SIDE EFFECTS
  None

===========================================================================*/
STATIC uint32 audamrwb_pkt_calc_linear_len (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset
) {
  uint32 base_offset;
  uint32 linear_len;

  base_offset = audamrwb_pkt_calc_base_offset(pkt_ctl, pkt_ctl->ready_ptr,
                                              offset);

  if (base_offset < pkt_ctl->buf_size) {
    linear_len = offset;
  } else {
    /* The offset must not exceed the buffer size */
    linear_len = (pkt_ctl->buf_size -
                  ((uint32)(pkt_ctl->ready_ptr - pkt_ctl->buf_ptr)));
  }

  return linear_len;
} /* audamrwb_pkt_calc_linear_len */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_init

DESCRIPTION
  Creates an instance of the packet control structure.

  The buf_size is the amount in bytes to dynamically allocate for the internal
  circular packet buffer.

DEPENDENCIES
  pkt_ctl - The provided structure must be zero-filled.

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_init (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 buf_size
) {
  audamrwb_status_type status;

  /* Setup the circular packet buffer */
  pkt_ctl->buf_size = buf_size;
  pkt_ctl->buf_ptr  = malloc(pkt_ctl->buf_size);

  if (pkt_ctl->buf_ptr != NULL) {
    pkt_ctl->buf_free    = pkt_ctl->buf_size;

    pkt_ctl->ready_ptr   = pkt_ctl->buf_ptr;
    pkt_ctl->ready_len   = 0;

    pkt_ctl->pkt_if      = AUDAMRWB_PKT_IF_NONE;
    pkt_ctl->pkt_max_len = 0;
    pkt_ctl->pkt_align   = AUDAMRWB_PKT_ALIGN_SIZE;

    status = AUDAMRWB_STATUS_SUCCESS;
  } else {
    MSG_ERROR("could not allocate memory", 0, 0, 0);
    status = AUDAMRWB_STATUS_ERR_NOMEM;
  }

  return status;
} /* audamrwb_pkt_init */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_deinit

DESCRIPTION
  Destroys an instance of the packet control structure.

DEPENDENCIES
  pkt_ctl - The provided structure must initialized.

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_deinit (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  audamrwb_status_type status = AUDAMRWB_STATUS_FAILURE;

  /* Free the circular packet buffer */
  if (pkt_ctl->buf_ptr != NULL) {
    free(pkt_ctl->buf_ptr);

    pkt_ctl->buf_ptr     = NULL;
    pkt_ctl->buf_size    = 0;
    pkt_ctl->buf_free    = 0;

    pkt_ctl->ready_ptr   = NULL;
    pkt_ctl->ready_len   = 0;

    pkt_ctl->pkt_if      = AUDAMRWB_PKT_IF_NONE;
    pkt_ctl->pkt_max_len = 0;
    pkt_ctl->pkt_align   = 0;

    status = AUDAMRWB_STATUS_SUCCESS;
  } else {
    MSG_ERROR("could not deallocate memory; base_ptr=NULL", 0, 0, 0);
  }

  return status;
} /* audamrwb_pkt_deinit */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_is_opened

DESCRIPTION
  Returns the status of the current working packet.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if a packet is currently opened for processing.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_pkt_is_opened (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  return ((pkt_ctl->pkt_if > AUDAMRWB_PKT_IF_NONE) &&
          (pkt_ctl->pkt_if < AUDAMRWB_PKT_IF_MAX));
} /* audamrwb_pkt_is_opened */

/*===========================================================================

FUNCTION audamrwb_pkt_is_full

DESCRIPTION
  Determines if the circular packet buffer is full.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if buffer is full.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_pkt_is_full (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  return (pkt_ctl->buf_free == 0);
} /* audamrwb_pkt_is_full */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_is_ready

DESCRIPTION
  Determines if the circular packet buffer's ready queue is not empty.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if data is available for processing.

SIDE EFFECTS
  None

===========================================================================*/
INLINE boolean audamrwb_pkt_is_ready (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  return (pkt_ctl->ready_len > 0);
} /* audamrwb_pkt_is_ready */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_get_free

DESCRIPTION
  Returns the free length of the circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Number of bytes free.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_get_free (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  return pkt_ctl->buf_free;
} /* audamrwb_pkt_get_free */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_write

DESCRIPTION
  Writes data to the circular packet buffer.

  The offset is relative to the beginning of the current starting position of
  the circular packet buffer.

  A NULL source_ptr writes nothing to the circular packet buffer.

  The *written_len returns to the caller the amount of data written in bytes.

  When the is_atomic is TRUE, the write operation must complete entirely.

  The sum of the offset and source_len must not exceed the circular packet
  buffer size. The size of the current working packet grows larger if the sum
  of the offset and source_len exceeds causes the working packet size to
  increase.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_write (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset,
  uint8*                 source_ptr,
  uint32                 source_len,
  uint32                *written_len,
  boolean                is_atomic
) {
  audamrwb_status_type status = AUDAMRWB_STATUS_FAILURE;
  uint32               adjusted_len = 0;
  uint32               base_offset;
  uint32               head_len;
  uint32               tail_len;
  uint32               overshoot_len;
  uint32               delta_need;
  uint32               delta_in_use;

  do {
    /* Readjust the number of bytes that can actually be written */
    if (offset >= pkt_ctl->buf_size) {
      MSG_ERROR("offset is not within PKT buffer", 0, 0, 0);
      status = AUDAMRWB_STATUS_ERR_BOUNDS;
      break;
    } else if ((offset + source_len) > pkt_ctl->buf_size) {
      if (is_atomic) {
        MSG_ERROR("write length causes PKT overflow", 0, 0, 0);
        status = AUDAMRWB_STATUS_ERR_OVERFLOW;
        break;
      } else {
        adjusted_len = pkt_ctl->buf_size - offset;
      }
    } else {
      adjusted_len = source_len;
    }

    if (adjusted_len > 0) {
      /* Copy the source to the circular packet buffer */
      if (source_ptr != NULL) {
        /* Calculate the relative position to the circular buffer's base
        ** address to see if we need to partition the write into two phases
        */
        base_offset = audamrwb_pkt_calc_base_offset(
                        pkt_ctl, pkt_ctl->ready_ptr, offset);

        /* Phase 1: Write the first segment, starts within the buffer */
        if (base_offset < pkt_ctl->buf_size) {
          head_len = MIN(adjusted_len, (pkt_ctl->buf_size - base_offset));
          tail_len = adjusted_len - head_len;
          overshoot_len = 0;

          if (head_len == 1) {
            *(pkt_ctl->buf_ptr + base_offset) = *source_ptr;
          } else {
            memcpy((pkt_ctl->buf_ptr + base_offset), source_ptr, head_len);
          }
        } else {
          head_len = 0;
          tail_len = adjusted_len;
          overshoot_len = base_offset - pkt_ctl->buf_size;
        }

        /* Phase 2: Write the second segment, wrapped around to the buffer
        ** base
        */
        if (tail_len > 0) {
          if (tail_len == 1) {
            *(pkt_ctl->buf_ptr + overshoot_len) = *(source_ptr + head_len);
          } else {
            memcpy((pkt_ctl->buf_ptr + overshoot_len), (source_ptr + head_len),
                   tail_len);
          }
        }
      }

      /* Update the free resource length */
      delta_need   = offset + adjusted_len;
      delta_in_use = pkt_ctl->buf_size - pkt_ctl->buf_free;

      if (delta_need > delta_in_use) {
        pkt_ctl->buf_free -= delta_need - delta_in_use;
      }
    }

    status = AUDAMRWB_STATUS_SUCCESS;
  } while (FALSE);

  /* Report the actual length written */
  if (written_len != NULL) {
    *written_len = adjusted_len;
  }

  return status;
} /* audamrwb_pkt_write */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_clear

DESCRIPTION
  Clears the entire circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_clear (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  (void) audamrwb_pkt_ready_clear(pkt_ctl);
  (void) audamrwb_pkt_packet_clear(pkt_ctl);

  return AUDAMRWB_STATUS_SUCCESS;
} /* audamrwb_pkt_clear */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_get_len

DESCRIPTION
  Returns the current packet length.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - The current packet length in number of bytes.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_packet_get_len (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  return (pkt_ctl->buf_size - pkt_ctl->buf_free - pkt_ctl->ready_len);
} /* audamrwb_pkt_packet_get_len */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_get_free

DESCRIPTION
  Returns the total amount of free space left in the current packet.

  This function only accounts for the virtual amount of free space available.
  Use audamrwb_pkt_get_buf_free() to determine the actual amount of free space
  left in the circular packet buffer.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Space remaining, in bytes, of the current packet.

SIDE EFFECTS
  None

===========================================================================*/
INLINE uint32 audamrwb_pkt_packet_get_free (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  return (pkt_ctl->pkt_max_len - audamrwb_pkt_packet_get_len(pkt_ctl));
} /* audamrwb_pkt_packet_get_free */

/* <EJECT> */
#if 0
/*===========================================================================

FUNCTION audamrwb_pkt_packet_resize

DESCRIPTION
  Resizes the current working packet to the indicated length. The actual
  resize mechanism is indicated by the packet interface type of the current
  working packet.

  The requested_len must be equal or less than the maximum size allowed per
  packet according to the packet interface type.

  The *adjusted_len returns to the caller the amount of data resized in
  bytes.

  When the is_atomic is TRUE, the resize operation must complete entirely.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_resize (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 requested_len,
  uint32                *adjusted_len,
  boolean                is_atomic
) {
  audamrwb_status_type       status = AUDAMRWB_STATUS_FAILURE;
  audamrwb_dsp_pkt_hdr_type *dsp_pkt_hdr;
  uint32                     new_len = 0;

  switch (pkt_ctl->pkt_if) {
    case AUDAMRWB_PKT_IF_NONE:
      MSG_ERROR("PKT IF is not configured", 0, 0, 0);
      break;

    case AUDAMRWB_PKT_IF_DSP_RAW:
    case AUDAMRWB_PKT_IF_DSP_PSEUDORAW:
      /* Ensure the current packet does not exceed its maximum length */
      if (requested_len > pkt_ctl->pkt_max_len) {
        if (is_atomic) {
          MSG_ERROR("PKT overflow is not allowed", 0, 0, 0);
          status = AUDAMRWB_STATUS_ERR_OVERFLOW;
          break;
        } else {
          new_len = pkt_ctl->pkt_max_len;
        }
      }

      /* Ensure there is enough free space to perform the write */
      if (new_len > (pkt_ctl->buf_size - pkt_ctl->ready_len)) {
        if (is_atomic) {
          MSG_ERROR("insufficient PKT free space", 0, 0, 0);
          status = AUDAMRWB_STATUS_ERR_NOSPACE;
          break;
        } else {
          new_len = pkt_ctl->buf_size - pkt_ctl->ready_len;
        }
      }

      /* Resize the current packet to its new length */
      pkt_ctl->buf_free = pkt_ctl->buf_size - (pkt_ctl->ready_len + new_len);

      status = AUDAMRWB_STATUS_SUCCESS;
      break;

    case AUDAMRWB_PKT_IF_DSP_VIRTUAL:
      dsp_pkt_hdr = &pkt_ctl->pkt_if_info.dsp_virtual.hdr;

      /* Ensure the current packet does not exceed its maximum length */
      if (requested_len > AUDAMRWB_DSP_PKT_MAX_DATA_LEN) {
        if (is_atomic) {
          MSG_ERROR("PKT overflow is not allowed", 0, 0, 0);
          status = AUDAMRWB_STATUS_ERR_OVERFLOW;
          break;
        } else {
          new_len = AUDAMRWB_DSP_PKT_MAX_DATA_LEN;
        }
      } else {
        new_len = requested_len;
      }

      audamrwb_dsp_pkt_hdr_set_len(dsp_pkt_hdr, new_len);
      break;

    default:
      MSG_ERROR("invalid PKT IF 0x%08X", pkt_ctl->pkt_if, 0, 0);
      status = AUDAMRWB_STATUS_ERR_UNSUPPORTED;
      break;
  }

  /* Report the actual length allocated */
  if (adjusted_len != NULL) {
    *adjusted_len = new_len;
  }

  return status;
} /* audamrwb_pkt_packet_resize */
#endif /* 0 */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_write

DESCRIPTION
  Writes data to the current working packet. The actual write mechanism is
  indicated by the packet interface type of the current working packet.

  The offset is relative to the beginning of the current working packet.

  A NULL source_ptr writes nothing to the circular packet buffer.

  The *written_len returns to the caller the amount of data written in bytes.

  When the is_atomic is TRUE, the write operation must complete entirely.

  The sum of the offset and source_len must not exceed the circular packet
  buffer size. The size of the current working packet grows larger if the sum
  of the offset and source_len exceeds causes the working packet size to
  increase.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_write (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 offset,
  uint8*                 source_ptr,
  uint32                 source_len,
  uint32                *written_len,
  boolean                is_atomic
) {
  audamrwb_status_type       status = AUDAMRWB_STATUS_FAILURE;
  audamrwb_dsp_pkt_hdr_type *dsp_pkt_hdr;
  uint32                     write_len = 0;

  switch (pkt_ctl->pkt_if) {
    case AUDAMRWB_PKT_IF_NONE:
      MSG_ERROR("PKT IF is not configured", 0, 0, 0);
      break;

    case AUDAMRWB_PKT_IF_DSP_RAW:
    case AUDAMRWB_PKT_IF_DSP_PSEUDORAW:
      /* Ensure the current packet does not exceed its maximum length */
      if (is_atomic && ((offset + source_len) > pkt_ctl->pkt_max_len)) {
        MSG_ERROR("PKT overflow is not allowed", 0, 0, 0);
        status = AUDAMRWB_STATUS_ERR_OVERFLOW;
        break;
      }

      /* Write to the current packet's data payload */
      status = audamrwb_pkt_write(
                 pkt_ctl, (pkt_ctl->ready_len + offset), source_ptr,
                 source_len, &write_len, is_atomic);
      if (AUDAMRWB_IS_BAD(status)) {
        MSG_ERROR("PKT write failed", 0, 0, 0);
      }
      break;

    case AUDAMRWB_PKT_IF_DSP_VIRTUAL:
      dsp_pkt_hdr = &pkt_ctl->pkt_if_info.dsp_virtual.hdr;

      /* Ensure the current packet does not exceed its maximum length */
      if ((offset + source_len) > AUDAMRWB_DSP_PKT_MAX_DATA_LEN) {
        if (is_atomic) {
          MSG_ERROR("PKT overflow is not allowed", 0, 0, 0);
          status = AUDAMRWB_STATUS_ERR_OVERFLOW;
          break;
        } else {
          write_len = AUDAMRWB_DSP_PKT_MAX_DATA_LEN;
        }
      } else {
        write_len = offset + source_len;
      }

      /* Write to the current packet's virtual data payload */
      if (write_len > audamrwb_dsp_pkt_hdr_get_len(dsp_pkt_hdr)) {
        audamrwb_dsp_pkt_hdr_set_len(dsp_pkt_hdr, write_len);
      }

      status = AUDAMRWB_STATUS_SUCCESS;
      break;

    default:
      MSG_ERROR("invalid PKT IF 0x%08X", pkt_ctl->pkt_if, 0, 0);
      status = AUDAMRWB_STATUS_ERR_UNSUPPORTED;
      break;
  }

  /* Report the actual length written */
  if (written_len != NULL) {
    *written_len = write_len;
  }

  return status;
} /* audamrwb_pkt_packet_write */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_sync

DESCRIPTION
  Synchronizes the current working packet to the ready queue.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_sync (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  pkt_ctl->ready_len = pkt_ctl->buf_size - pkt_ctl->buf_free;

  /* Clear the packet structure */
  pkt_ctl->pkt_if      = AUDAMRWB_PKT_IF_NONE;
  pkt_ctl->pkt_max_len = 0;

  return AUDAMRWB_STATUS_SUCCESS;
} /* audamrwb_pkt_packet_sync */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_clear

DESCRIPTION
  Clears the current working packet.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_clear (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  pkt_ctl->buf_free = pkt_ctl->buf_size - pkt_ctl->ready_len;

  /* Clear the packet structure */
  pkt_ctl->pkt_if      = AUDAMRWB_PKT_IF_NONE;
  pkt_ctl->pkt_max_len = 0;

  return AUDAMRWB_STATUS_SUCCESS;
} /* audamrwb_pkt_packet_clear */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_create

DESCRIPTION
  Creates/reopens a new current working packet.

  The *required_len returns the number of bytes needed or allocated by the
  current working packet for the indicated packet interface type.

  The *pkt_if_info should be non-NULL if the indicated packet interface type
  is AUDAMRWB_PKT_IF_DSP_PSEUDORAW.

  The circular packet buffer begins with the ready queue followed by the
  current working packet. The ready queue can contain zero or more packets,
  inserted in FIFO order, to be consumed by the client. The current working
  packet is appended to the end of the ready queue when it becomes closed--
  closed packets should not be modified. At most one working packet exists at
  any time.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_create (
  audamrwb_pkt_ctl_type     *pkt_ctl,
  audamrwb_pkt_if_type       pkt_if,
  audamrwb_pkt_if_info_type *pkt_if_info,
  uint32                    *required_len
) {
  audamrwb_status_type       status = AUDAMRWB_STATUS_FAILURE;
  audamrwb_dsp_pkt_hdr_type* dsp_hdr_ptr;
  uint32                     needed_len = 0;

  if (audamrwb_pkt_is_opened(pkt_ctl)) {
    if (pkt_if != pkt_ctl->pkt_if) {
      status = AUDAMRWB_STATUS_ERR_MISMATCH;
    } else {
      status = AUDAMRWB_STATUS_OK_OPEN;
    }
  } else {
    switch (pkt_if) {
      case AUDAMRWB_PKT_IF_NONE:
        MSG_ERROR("PKT IF is not configured", 0, 0, 0);
        break;

      case AUDAMRWB_PKT_IF_DSP_RAW:
        /* Setup the packet structure */
        pkt_ctl->pkt_if      = pkt_if;
        pkt_ctl->pkt_max_len = AUDAMRWB_DSP_PKT_MAX_PKT_LEN;

        status = AUDAMRWB_STATUS_SUCCESS;
        break;

      case AUDAMRWB_PKT_IF_DSP_PSEUDORAW:
      case AUDAMRWB_PKT_IF_DSP_VIRTUAL:
        /* Set the free space requirement */
        needed_len = sizeof(*dsp_hdr_ptr);

        if (pkt_ctl->buf_free < needed_len) {
          MSG_ERROR("not enough space to create PKT", 0, 0, 0);
          status = AUDAMRWB_STATUS_ERR_NOSPACE;
          break;
        }

        /* Setup the packet structure */
        pkt_ctl->pkt_if      = pkt_if;
        pkt_ctl->pkt_max_len = AUDAMRWB_DSP_PKT_MAX_FRM_LEN;

        if (pkt_if == AUDAMRWB_PKT_IF_DSP_PSEUDORAW) {
          /* Initialize the DSP pseudo-raw packet header */
          if (pkt_if_info == NULL) {
            MSG_ERROR("PKT IF INFO is missing", 0, 0, 0);
            status = AUDAMRWB_STATUS_ERR_BADPARAM;
            break;
          }

          dsp_hdr_ptr = &pkt_ctl->pkt_if_info.dsp_pseudoraw.hdr;

          dsp_hdr_ptr->sync_word   = 0xFFFF;
          dsp_hdr_ptr->bfi1        = 0;
          dsp_hdr_ptr->bfi2        = 0;
          dsp_hdr_ptr->bfi3        = 0;
          dsp_hdr_ptr->bfi4        = 0;
          dsp_hdr_ptr->reserved    = 0;
          dsp_hdr_ptr->vpi         = 0;
          audamrwb_dsp_pkt_hdr_set_len(dsp_hdr_ptr, 0);
          dsp_hdr_ptr->frame_type  = pkt_if_info->dsp_pseudoraw.hdr.frame_type;
          dsp_hdr_ptr->zero_1      = 0;
          dsp_hdr_ptr->isf         = pkt_if_info->dsp_pseudoraw.hdr.isf;
          dsp_hdr_ptr->zero_2      = 0;
          dsp_hdr_ptr->zero_3      = 0;
          dsp_hdr_ptr->zero_4      = 0;
        } else {
          /* Initialize the DSP virtual packet header */
          dsp_hdr_ptr = &pkt_ctl->pkt_if_info.dsp_virtual.hdr;

          dsp_hdr_ptr->sync_word   = 0xFFFF;
          dsp_hdr_ptr->bfi1        = 0;
          dsp_hdr_ptr->bfi2        = 0;
          dsp_hdr_ptr->bfi3        = 0;
          dsp_hdr_ptr->bfi4        = 0;
          dsp_hdr_ptr->reserved    = 0;
          dsp_hdr_ptr->vpi         = 1;
          audamrwb_dsp_pkt_hdr_set_len(dsp_hdr_ptr, 0);
          dsp_hdr_ptr->frame_type  = 0;
          dsp_hdr_ptr->zero_1      = 0;
          dsp_hdr_ptr->isf         = 0;
          dsp_hdr_ptr->zero_2      = 0;
          dsp_hdr_ptr->zero_3      = 0;
          dsp_hdr_ptr->zero_4      = 0;
        }

        /* The actual DSP packet header will be written at packet closure */
        status = audamrwb_pkt_write(pkt_ctl, pkt_ctl->ready_len, NULL,
                                    sizeof(*dsp_hdr_ptr), NULL, TRUE);
        if (AUDAMRWB_IS_BAD(status)) {
          MSG_ERROR("unable to create the new packet", 0, 0, 0);

          /* Clear the packet structure */
          pkt_ctl->pkt_if      = AUDAMRWB_PKT_IF_NONE;
          pkt_ctl->pkt_max_len = 0;
        }
        break;

      default:
        MSG_ERROR("invalid PKT IF 0x%08X", pkt_if, 0, 0);
        status = AUDAMRWB_STATUS_ERR_UNSUPPORTED;
        break;
    }
  }

  /* Report the amount of space needed/allocated to create the packet */
  if (required_len != NULL) {
    *required_len = needed_len;
  }

  return status;
} /* audamrwb_pkt_packet_create */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_packet_close

DESCRIPTION
  Closes and appends the current working packet to the end of the ready
  queue.

  The *required_len returns the number of bytes needed or allocated to close
  the current working packet.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_packet_close (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                *required_len
) {
  audamrwb_status_type       status = AUDAMRWB_STATUS_FAILURE;
  audamrwb_dsp_pkt_hdr_type *dsp_pkt_hdr;
  uint32                     pkt_len;
  uint32                     aligned_len;
  uint32                     pad_len;
#ifdef AUDAMRWB_DBG_CAPTURE_ADEC_PKTS
  uint32                     offset;
  uint8                      fill_byte;
#endif /* AUDAMRWB_DBG_CAPTURE_ADEC_PKTS */
  uint32                     needed_len = 0;

  switch (pkt_ctl->pkt_if) {
    case AUDAMRWB_PKT_IF_NONE:
      MSG_ERROR("PKT IF is not configured", 0, 0, 0);
      break;

    case AUDAMRWB_PKT_IF_DSP_PSEUDORAW:
    case AUDAMRWB_PKT_IF_DSP_VIRTUAL:
      if (pkt_ctl->pkt_if == AUDAMRWB_PKT_IF_DSP_PSEUDORAW) {
        dsp_pkt_hdr = &pkt_ctl->pkt_if_info.dsp_pseudoraw.hdr;
      } else {
        dsp_pkt_hdr = &pkt_ctl->pkt_if_info.dsp_virtual.hdr;
      }

      /* Add padding to the current packet if it is not aligned */
      pkt_len     = audamrwb_pkt_packet_get_len(pkt_ctl);
      aligned_len = AUDAMRWB_PKT_ALIGN(pkt_len, pkt_ctl->pkt_align);

      if (aligned_len > pkt_len) {
        pad_len = aligned_len - pkt_len;

        if (pad_len > pkt_ctl->buf_free) {
          MSG_ERROR("not enough buffer space to align PKT size", 0, 0, 0);
          needed_len = pad_len;
          status = AUDAMRWB_STATUS_ERR_NOSPACE;
          break;
        } else {
#ifdef AUDAMRWB_DBG_CAPTURE_ADEC_PKTS
          /* Pad the alignment with a constant to make binary diff'ing easier
          ** for offline analysis--optimization is not required
          */
          fill_byte = 'A';
          for (offset = 0; offset < pad_len; offset++) {
            (void) audamrwb_pkt_write(
                     pkt_ctl, (pkt_ctl->ready_len + pkt_len + offset),
                     &fill_byte, sizeof(fill_byte), NULL, TRUE);
          }
#else
          pkt_ctl->buf_free -= pad_len;
#endif /* AUDAMRWB_DBG_CAPTURE_ADEC_PKTS */
        }
      }

      /* Update the DSP packet length field */
      if (pkt_ctl->pkt_if == AUDAMRWB_PKT_IF_DSP_PSEUDORAW) {
        /* Set the pseudo-raw packet super frame length */
        audamrwb_dsp_pkt_hdr_set_len(dsp_pkt_hdr,
                                     (pkt_len - sizeof(*dsp_pkt_hdr)));
      } else {
        /* The virtual payload length is always up-to-date */
      }

      /* Write the DSP packet header */
      status = audamrwb_pkt_write(pkt_ctl, pkt_ctl->ready_len,
                                  ((uint8*)dsp_pkt_hdr), sizeof(*dsp_pkt_hdr),
                                  NULL, TRUE);

      /* Finalize the current packet */
      if (AUDAMRWB_IS_BAD(status)) {
        status = audamrwb_pkt_packet_clear(pkt_ctl);
      } else {
        status = audamrwb_pkt_packet_sync(pkt_ctl);
      }
      break;

    case AUDAMRWB_PKT_IF_DSP_RAW:
      /* Add padding to the current packet if it is not aligned */
      pkt_len     = audamrwb_pkt_packet_get_len(pkt_ctl);
      aligned_len = AUDAMRWB_PKT_ALIGN(pkt_len, pkt_ctl->pkt_align);

      if (aligned_len > pkt_len) {
        pad_len = aligned_len - pkt_len;

        if (pad_len > pkt_ctl->buf_free) {
          MSG_ERROR("not enough buffer space to align PKT size", 0, 0, 0);
          needed_len = pad_len;
          status = AUDAMRWB_STATUS_ERR_NOSPACE;
          break;
        } else {
#ifdef AUDAMRWB_DBG_CAPTURE_ADEC_PKTS
          /* Pad the alignment with a constant to make binary diff'ing easier
          ** for offline analysis--optimization is not required
          */
          fill_byte = 'A';
          for (offset = 0; offset < pad_len; offset++) {
            (void) audamrwb_pkt_write(
                     pkt_ctl, (pkt_ctl->ready_len + pkt_len + offset),
                     &fill_byte, sizeof(fill_byte), NULL, TRUE);
          }
#else
          pkt_ctl->buf_free -= pad_len;
#endif /* AUDAMRWB_DBG_CAPTURE_ADEC_PKTS */
        }
      }

      /* Finalize the current packet */
      status = audamrwb_pkt_packet_sync(pkt_ctl);
      break;

    default:
      MSG_ERROR("invalid PKT IF 0x%08X", pkt_ctl->pkt_if, 0, 0);
      status = AUDAMRWB_STATUS_ERR_UNSUPPORTED;
      break;
  }

  /* Report the amount of space needed/allocated to create the packet */
  if (required_len != NULL) {
    *required_len = needed_len;
  }

  return status;
} /* audamrwb_pkt_packet_close */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_ready_read

DESCRIPTION
  Retrieves the head and tail buffers to the ready queue.

  This function is used by the client to retrieve data ready for processing.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_ready_read (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint8*                *head_ptr,
  uint32                *head_len,
  uint8*                *tail_ptr,
  uint32                *tail_len
) {
  if (head_ptr != NULL) {
    *head_ptr = pkt_ctl->ready_ptr;
  }
  if (head_len != NULL) {
    *head_len = audamrwb_pkt_calc_linear_len(pkt_ctl, pkt_ctl->ready_len);
  }
  if (tail_ptr != NULL) {
    *tail_ptr = pkt_ctl->buf_ptr;
  }
  if (tail_len != NULL) {
    *tail_len = pkt_ctl->ready_len - *head_len;
  }

  return AUDAMRWB_STATUS_SUCCESS;
} /* audamrwb_pkt_ready_read */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_ready_sync

DESCRIPTION
  Synchronizes the current starting position of the circular packet buffer to
  reflect the amount of data read from the ready queue.

  This function is used by the client to notify the amount of data consumed
  from the ready queue.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_ready_sync (
  audamrwb_pkt_ctl_type *pkt_ctl,
  uint32                 consumed_len
) {
  audamrwb_status_type status = AUDAMRWB_STATUS_FAILURE;

  if (consumed_len > pkt_ctl->ready_len) {
    /* Over-consumption of packets should not occur */
    MSG_ERROR("over-consumption is not allowed", 0, 0, 0);
  } else {
    /* Shift the current position to the next read position */
    pkt_ctl->ready_ptr = audamrwb_pkt_calc_offset_ptr(pkt_ctl, consumed_len);
    /* Free the consumed data back to the free memory pool */
    pkt_ctl->ready_len -= consumed_len;
    pkt_ctl->buf_free  += consumed_len;

    status = AUDAMRWB_STATUS_SUCCESS;
  }

  return status;
} /* audamrwb_pkt_ready_sync */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_pkt_ready_clear

DESCRIPTION
  Clears the ready queue.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_pkt_ready_clear (
  audamrwb_pkt_ctl_type *pkt_ctl
) {
  pkt_ctl->buf_free  += pkt_ctl->ready_len;
  pkt_ctl->ready_ptr  = audamrwb_pkt_calc_offset_ptr(pkt_ctl,
                                                     pkt_ctl->ready_len);
  pkt_ctl->ready_len  = 0;

  return AUDAMRWB_STATUS_SUCCESS;
} /* audamrwb_pkt_ready_clear */

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Parser Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_parse_init

DESCRIPTION
  Initializes the internal resources and behavior of the parser.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_parse_init (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_mode_type       mode
) {
  audamrwb_status_type status = AUDAMRWB_STATUS_FAILURE;

  do {
    /* Initialize the general parser status */
    parse_ctl->status = AUDAMRWB_STATUS_SUCCESS;

    /* Initialize the need sample rate flag */
    parse_ctl->need_rate = TRUE;

    /* Setup the circular packet buffer */
    status = audamrwb_pkt_init(&parse_ctl->pkt_ctl, AUDAMRWB_PKT_BUF_SIZE);
    if (AUDAMRWB_IS_BAD(status)) {
      MSG_ERROR("init_parser: could not allocate PKT buffer: 0x%08X", status,
                0, 0);
      break;
    }

    /* Setup the operation mode */
    parse_ctl->operation = mode;

    /* Setup the first parsing state */
    status = audamrwb_parse_reset(parse_ctl);
    if (AUDAMRWB_IS_BAD(status)) {
      MSG_ERROR("init_parser: bad operation mode: 0x%08X", status, 0, 0);
      break;
    }
  } while (FALSE);

  return status;
} /* audamrwb_parse_init */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_parse_reset

DESCRIPTION
  Resets the parser state to starting values.

DEPENDENCIES
  None

RETURN VALUE
  audamrwb_status_type

SIDE EFFECTS
  None

===========================================================================*/
STATIC audamrwb_status_type audamrwb_parse_reset (
  audamrwb_parse_ctl_type *parse_ctl
) {
  audamrwb_status_type status = AUDAMRWB_STATUS_SUCCESS;

  /* Setup the first parsing state */
  switch (parse_ctl->operation) {
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case AUDAMRWB_MODE_DEC_AWB_FILE:
      parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_1;
      break;

    case AUDAMRWB_MODE_DEC_AWB_STREAM:
      parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1;
      break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case AUDAMRWB_MODE_DEC_AWP_STREAM:
      parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1;
      break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

    default:
      MSG_ERROR("cmx_parse_reset: invalid option: 0x%08X",
                parse_ctl->operation, 0, 0);
      status = AUDAMRWB_STATUS_FAILURE;
  }

  parse_ctl->run_sm = FALSE;

  return status;
} /* audamrwb_parse_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_cmx_buf_consume

DESCRIPTION
  Consumes and stores the indicated number of bytes of data from the source
  buffer to a destination buffer.

  This function is primarily used to consume CMX buffers during parsing. The
  caller must ensure that there is enough data remaining to consume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_cmx_buf_consume (
  uint8*  *cmx_buf,
  uint32* *cmx_buf_len,
  uint8*   dest_ptr,
  uint32   length
) {
  if (length > 0) {
    if (dest_ptr != NULL) {
      if (length == 1) {
        /* Move 1 byte from CMX buffer to the destination */
        *dest_ptr = **cmx_buf;
      } else {
        /* Move multiple bytes from CMX buffer to the destination */
        memcpy(dest_ptr, *cmx_buf, length);
      }
    }

    *cmx_buf      += length;
    **cmx_buf_len -= length;
  }
} /* audamrwb_cmx_buf_consume */

/* <EJECT> */
#ifdef FEATURE_AMR_WB_AUDIO_DEC
/*===========================================================================

FUNCTION audamrwb_transcode_awb_storage

DESCRIPTION
  AMR-WB storage transcoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_awb_storage (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   cmx_buf,
  uint32                  *cmx_buf_len
) {
  /* Unsupported */
  MSG_ERROR("AMR-WB file playback is not supported", 0, 0, 0);
} /* audamrwb_transcode_awb_storage */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_transcode_awb_stream

DESCRIPTION
  AMR-WB stream transcoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_awb_stream (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   cmx_buf,
  uint32                  *cmx_buf_len
) {
  audamrwb_status_type        status;
  boolean                     exit_flag = FALSE;
  audamrwb_cmx_pkt_proc_type *cur_cmx_pkt_ptr = &parse_ctl->cur_cmx_pkt;
  audamrwb_frm_proc_type     *cur_frm_ptr     = &parse_ctl->cur_frame;
  audamrwb_pkt_ctl_type      *pkt_ctl         = &parse_ctl->pkt_ctl;
  audamrwb_dsp_pkt_hdr_type  *dsp_pkt_hdr;
  audamrwb_pkt_if_info_type   pkt_if_info;
  uint8                       cur_byte;
  uint32                      read_len;
  uint32                      remaining_len;
#ifndef FEATURE_AMR_WB_3GP_RAW_FRAMES
  amrsup_if1_frame_info_type  if1_frame_info;
  int                         num_bytes_decoded;
#endif /* FEATURE_AMR_WB_3GP_RAW_FRAMES */

  /* Process the incoming buffer */
  while ((exit_flag == FALSE) &&
         ((*cmx_buf_len > 0) || (parse_ctl->run_sm == TRUE))) {

    /* Parse the buffer according to the current parse state */
    switch (parse_ctl->parse_state) {

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_SYNC_1
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1:
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */

        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* Look for the first byte of the sync word */
        if (cur_byte == 0xFF) {

          /* Now go find the second sync byte */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_2;
        } else {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#else
          MSG_ERROR("sync_1 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_SYNC_2
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_2:
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* Verify that we have the second sync byte of the sync word */
        if (cur_byte == 0xFF) {
          /* Now go find the first byte of the frames per sample word */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_1;
        } else {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#else
          MSG_ERROR("sync_2 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_FRAMES_1
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_1:
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* Store the first byte of the number of frames per sample. 
        ** The first byte is the LSB of the little-endian word.
        */
        cur_cmx_pkt_ptr->hdr.num_samples = (uint16)cur_byte;

        /* Move on to finding second byte of the frames per sample word */
        parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_2;
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_FRAMES_2
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_2:
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* Store the second byte of the number of frames per sample
        ** The second byte is the MSB of the little endian word.
        */
        cur_cmx_pkt_ptr->hdr.num_samples |= (((uint16)cur_byte) << 8) & 0xFF00;

        /* For AMR-WB we must have 0 < num_samples <= 15 */
        if ( (0 < cur_cmx_pkt_ptr->hdr.num_samples) &&
             (cur_cmx_pkt_ptr->hdr.num_samples <= 15) ) {
          /* Go get the CMX packet data length */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_LENGTH;
        }
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#else
        else {
          MSG_ERROR("frames_2 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
        }
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_LENGTH
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_LENGTH:
#ifdef AUDAMRWB_AMR_WB_AUDIO_DEC_AV_SYNC
        /* Check if requirements are met before processing this state */
        if (audamrwb_pkt_get_free(pkt_ctl) < (1 * AUDAMRWB_DSP_PKT_HDR_LEN)) {
          exit_flag = TRUE;
          break;
        }
#endif /* AUDAMRWB_AMR_WB_AUDIO_DEC_AV_SYNC */

        /* Consume up to 4 bytes when possible */
        read_len = AUDAMRWB_CMX_PKT_HDR_LEN - cur_cmx_pkt_ptr->header_read;
        read_len = MIN(read_len, *cmx_buf_len);

        if (read_len > 0) {
          audamrwb_cmx_buf_consume(
            &cmx_buf, &cmx_buf_len,
            (((uint8*)&cur_cmx_pkt_ptr->hdr) + cur_cmx_pkt_ptr->header_read),
            read_len);
          cur_cmx_pkt_ptr->header_read += (uint16)read_len;
        }

        /* Wait until we have collected four bytes to form one CMX packet
        ** data length field
        */
        if (cur_cmx_pkt_ptr->header_read >= AUDAMRWB_CMX_PKT_HDR_LEN) {
          MSG_LOW("info: CMX packet header found %+d byte(s)",
                  cur_cmx_pkt_ptr->hdr.num_bytes, 0, 0);

#ifdef AUDAMRWB_AMR_WB_AUDIO_DEC_AV_SYNC
          /* Discard (8) bytes: We're done reading the CMX packet header:
          ** Inform the DSP to account for the metadata read to the A/V sync
          ** bytes consumed counter
          */
          status = audamrwb_pkt_packet_create(
                     pkt_ctl, AUDAMRWB_PKT_IF_DSP_VIRTUAL, NULL, NULL);
          if (AUDAMRWB_IS_OK(status)) {
            status = audamrwb_pkt_packet_write(
                       pkt_ctl, 0, NULL, 8, NULL, TRUE);
            if (AUDAMRWB_IS_OK(status)) {
              status = audamrwb_pkt_packet_close(pkt_ctl, NULL);
            }
          }
          if (AUDAMRWB_IS_BAD(status)) {
            MSG_ERROR("could not write a VPKT 0:0x%08X", status, 0, 0);
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
            break;
          }
#endif /* AUDAMRWB_AMR_WB_AUDIO_DEC_AV_SYNC */
          cur_cmx_pkt_ptr->header_read = 0;

          /* CMX packet data parsing is done if it's 0-lengthed */
          if (cur_cmx_pkt_ptr->hdr.num_bytes == 0) {
            /* Start looking for the next CMX packet: The current packet is
            ** empty
            */
            MSG_MED("warning: empty CMX packet received, find sync_1", 0, 0,
                    0);
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1;
            break;
          }

          /* Reset the current CMX packet data processing fields */
          cur_cmx_pkt_ptr->data_read = 0;
          cur_cmx_pkt_ptr->frame_count = 0;

          /* Reset the current frame processing fields */
          cur_frm_ptr->header_read = 0;
          cur_frm_ptr->frame_count = 0;

          /* Now that we have all the CMX packet header, we are ready to
          ** parse for some frames */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_HDR_1;
        } else {
          /* Stay in this state until we have the data length field */
          /* parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_LENGTH; */
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_FRM_HDR_1
      **---------------------------------------------------------------------
      */
    case AUDAMRWB_PARSE_STATE_FRM_HDR_1:
        /* Check if requirements are met before processing this state */
        if (audamrwb_pkt_get_free(pkt_ctl) < (2 * AUDAMRWB_DSP_PKT_HDR_LEN)) {
          exit_flag = TRUE;
          break;
        }
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->data_read += 1;
        cur_frm_ptr->header_read += 1;

        /* Store the current byte to the frame header */
        ((uint8*)&(cur_frm_ptr->hdr.awb_hdr))[0] = cur_byte;

        /* Setup the decoder output sample rate */
        if (parse_ctl->need_rate == TRUE) {
          if (audamrwb_amr_is_wb_frame(cur_frm_ptr->hdr.awb_hdr.frame_type)) {
            /* Sampling rate for AMR-WB is 16kHz */
            parse_ctl->config.sample_rate = SND_SAMPLE_RATE_16000;
            parse_ctl->need_rate = FALSE;
          }
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY
#error Basic AMR-WB playback error recovery feature is not available.
#else
          else {
            MSG_ERROR("hdr_1 state: detected data corruption", 0, 0, 0);
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
            break;
          }
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY */
        }

        /* Calculate the frame length */
        cur_frm_ptr->data_len = audamrwb_calc_frame_data_len(
                                  cur_frm_ptr->hdr.awb_hdr.frame_type);

        /* Verify that we have enough bytes left to form one storage unit */
        remaining_len = (cur_cmx_pkt_ptr->hdr.num_bytes -
                         cur_cmx_pkt_ptr->data_read);

        if (remaining_len < cur_frm_ptr->data_len) {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY
#error Basic AMR-WB playback error recovery feature is not available.
#else
          MSG_ERROR("hdr_1 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY */
          break;
        }

#ifdef AUDAMRWB_AMR_WB_AUDIO_DEC_AV_SYNC
        /* Discard (1) byte: inform the DSP to account for the frame header to the A/V sync
        ** bytes consumed counter.
        */
        status = audamrwb_pkt_packet_create(
                   pkt_ctl, AUDAMRWB_PKT_IF_DSP_VIRTUAL, NULL, NULL);
        if (AUDAMRWB_IS_OK(status)) {
            status = audamrwb_pkt_packet_write(
                       pkt_ctl, 0, NULL, 1, NULL, TRUE);
          if (AUDAMRWB_IS_OK(status)) {
            status = audamrwb_pkt_packet_close(pkt_ctl, NULL);
          }
        }
        if (AUDAMRWB_IS_BAD(status)) {
          MSG_ERROR("could not write a VPKT 0:0x%08X", status, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
          break;
        }
#endif /* AUDAMRWB_AMR_WB_AUDIO_DEC_AV_SYNC */

        cur_frm_ptr->header_read = 0;

        /* Increment the frames found counters */
        cur_cmx_pkt_ptr->frame_count += 1;
        cur_frm_ptr->frame_count += 1;

        /* Reset the current frame's data fields */
        cur_frm_ptr->data_read = 0;
        memset(cur_frm_ptr->frame_data, 0, AUDAMRWB_AWB_MAX_FRM_DATA_LEN);

       /* Create a new pseudoraw packet */
        dsp_pkt_hdr = &pkt_if_info.dsp_pseudoraw.hdr;
        dsp_pkt_hdr->frame_type = cur_frm_ptr->hdr.awb_hdr.frame_type;
        dsp_pkt_hdr->isf = 0; /* ISF is zero for AMR-WB */

        status = audamrwb_pkt_packet_create(
                   pkt_ctl, AUDAMRWB_PKT_IF_DSP_PSEUDORAW, &pkt_if_info, NULL);
        if (AUDAMRWB_IS_BAD(status)) {
          MSG_ERROR("could not create a new PKT", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
          break;
        }

        /* Now go get the frame's data */
        parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_DATA;
        break;

      /*---------------------------------------------------------------------
      ** STATE_FRM_DATA
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_FRM_DATA:
        /* Check if requirements are met before processing this state */
        if (audamrwb_pkt_get_free(pkt_ctl) == 0) {
          exit_flag = TRUE;
          break;
        }

        /* Consume the remaining frame data */
        read_len = cur_frm_ptr->data_len - cur_frm_ptr->data_read;
        read_len = MIN(read_len, (cur_cmx_pkt_ptr->hdr.num_bytes -
                                  cur_cmx_pkt_ptr->data_read));
        read_len = MIN(read_len, *cmx_buf_len);


        if ( read_len > 0 ) {
          /* Consume the data to a temporary buffer. (We store data to
          ** temp buffer until we have the whole frame. Then we transcode
          ** it (IF1 de-frame) and after that we write it to the DSP packet.
          */
          audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len,
               (cur_frm_ptr->frame_data + cur_frm_ptr->data_read), read_len);
          cur_cmx_pkt_ptr->data_read += ((uint16)read_len);
          cur_frm_ptr->data_read += ((uint16)read_len);
        }

        /* Check if we are done reading the frame data */
        if (cur_frm_ptr->data_read >= cur_frm_ptr->data_len) {
#ifdef FEATURE_AMR_WB_3GP_RAW_FRAMES
          /* Move on to writing the frame data to the circular DSP packet
          ** buffer
          */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_DATA_WRITE;
          /* Force the transcoder to run, in case the CMX buffer is empty,
          ** to get to the next state
          */
          parse_ctl->run_sm = TRUE;
#else
          /* Now that we have the whole frame, we transcode it: IF1 to 
          ** raw DSP format
          */
          amrsup_frame_type frame_type; /* dummy frame_type */
          amrsup_mode_type amr_mode;    /* dummy amr_mode */

          if1_frame_info.frame_type_index = cur_frm_ptr->hdr.awb_hdr.frame_type;
          if1_frame_info.fqi = cur_frm_ptr->hdr.awb_hdr.fqi;
          if1_frame_info.amr_type = AMRSUP_CODEC_AMR_WB;

          num_bytes_decoded = amrsup_if1_de_framing(cur_frm_ptr->transcoded_frame_data,
                                                    &frame_type,
                                                    &amr_mode,
                                                    cur_frm_ptr->frame_data,
                                                    &if1_frame_info);

          if ( num_bytes_decoded == cur_frm_ptr->data_len ) {
            /* IF1 de-framing went ok. Now move on to writing the frame
            ** data to the circular DSP buffer.
            */
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_DATA_WRITE;

            /* Force the transcoder to run, in case the CMX buffer is empty, to
            ** to get to the next state
            */
            parse_ctl->run_sm = TRUE;
          } else {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY
#error Basic AMR-WB playback error recovery feature is not available.
#else
            MSG_ERROR("frm_data state: IF1 decoding failed", 0, 0, 0);
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY */
          }
#endif /* FEATURE_AMR_WB_3GP_RAW_FRAMES */
        } else {
          /* Stay in this state until we are done reading the frame data */
          /* parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_DATA; */
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_FRM_DATA_WRITE
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_FRM_DATA_WRITE:
        /* Do we have enough room in the circular DSP packet buffer
        ** to copy over the frame data?
        */
        if ( audamrwb_pkt_get_free(pkt_ctl) >= cur_frm_ptr->data_len ) {
#ifdef FEATURE_AMR_WB_3GP_RAW_FRAMES
          /* Yes. Append the raw frame data (as obtained directly from the
          ** CMX buffer) to the current DSP packet.
          */
          (void) audamrwb_pkt_packet_write(
                 pkt_ctl, audamrwb_pkt_packet_get_len(pkt_ctl),
                 cur_frm_ptr->frame_data, cur_frm_ptr->data_len, NULL, TRUE);
#else
          /* Yes. Append the IF1 de-framed frame data to the current DSP 
          ** packet.
          */
          (void) audamrwb_pkt_packet_write(
                 pkt_ctl, audamrwb_pkt_packet_get_len(pkt_ctl),
                 cur_frm_ptr->transcoded_frame_data, cur_frm_ptr->data_len,
                 NULL, TRUE);
#endif /* FEATURE_AMR_WB_3GP_RAW_FRAMES */

          /* Now move on to closing the packet */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_END;
        } else {
          /* No. Stay in this state until enough of the circular DSP buffer
          ** is consumed that there is room for our frame data.
          */
          exit_flag = TRUE;
        }
      break;

      /*---------------------------------------------------------------------
      ** STATE_FRM_END
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_FRM_END:
        /* Close the current packet (aligned as needed) */
        status = audamrwb_pkt_packet_close(pkt_ctl, NULL);
        if (AUDAMRWB_IS_BAD(status)) {
          if (status == AUDAMRWB_STATUS_ERR_NOSPACE) {
            /* Wait until the circular packet buffer has more free space */
            MSG_MED("stalling packet close; not enough space", 0, 0, 0);
            parse_ctl->run_sm = TRUE;
            exit_flag = TRUE;
            break;
          } else {
            MSG_ERROR("could not close packet", 0, 0, 0);
            parse_ctl->run_sm = FALSE;
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
            break;
          }
        } else {
          parse_ctl->run_sm = FALSE;
        }

        /* Check if there are more frames to parse in the current CMX packet
        */
        if (cur_cmx_pkt_ptr->data_read < cur_cmx_pkt_ptr->hdr.num_bytes) {
          /* Still more frames to parse from the current CMX packet */
          MSG_LOW("info: parsed one storage unit, continue frm_hdr_1", 0, 0,
                  0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_HDR_1;
        } else {
          /* No more frames to parse from the current CMX packet */
          MSG_LOW("info: parsed final storage unit, find sync_1", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1;
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_FAILURE
      **---------------------------------------------------------------------
      */
      default:
      case AUDAMRWB_PARSE_STATE_FAILURE:
        parse_ctl->status = AUDAMRWB_STATUS_ERR_BADSTATE;
        parse_ctl->run_sm = FALSE;
        exit_flag = TRUE;
        break;

    } /* switch (parse_ctl->parse_state) */
  } /* while ((exit_flag == FALSE) */

} /* audamrwb_transcode_awb_stream */
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

/* <EJECT> */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
/*===========================================================================

FUNCTION audamrwb_transcode_awp_stream

DESCRIPTION
  AMR-WB+ stream transcoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_awp_stream (
  audamrwb_parse_ctl_type *parse_ctl,
  uint8*                   cmx_buf,
  uint32                  *cmx_buf_len
) {
  audamrwb_status_type        status;
  boolean                     exit_flag = FALSE;
  audamrwb_cmx_pkt_proc_type *cur_cmx_pkt_ptr = &parse_ctl->cur_cmx_pkt;
  audamrwb_frm_proc_type     *cur_frm_ptr     = &parse_ctl->cur_frame;
  audamrwb_pkt_ctl_type      *pkt_ctl         = &parse_ctl->pkt_ctl;
  audamrwb_dsp_pkt_hdr_type  *dsp_pkt_hdr;
  audamrwb_pkt_if_info_type   pkt_if_info;
  uint8                       cur_byte;
  uint32                      read_len;
  uint32                      remaining_len;

  /* Process the incoming buffer */
  while ((exit_flag == FALSE) &&
         ((*cmx_buf_len > 0) || (parse_ctl->run_sm == TRUE))) {
    /* Parse the buffer according to the current parse state */
    switch (parse_ctl->parse_state) {
      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_SYNC_1
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1:
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */

        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* Look for the first byte of the sync word */
        if (cur_byte == 0xFF) {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */

          /* Now go find the second sync byte */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_2;
        } else {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#else
          MSG_ERROR("sync_1 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_SYNC_2
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_2:
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* Verify that we have the second sync byte of the sync word */
        if (cur_byte == 0xFF) {
          /* Now go find the first byte of the frames per sample word */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_1;
        } else {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#else
          MSG_ERROR("sync_2 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_FRAMES_1
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_1:
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* The first byte of the number of frames per sample must be 0x00 for
        ** AMR-WB+: The first byte is the LSB of the little-endian word
        */
        if (cur_byte == 0x00) {
          /* Now go find the second byte of the frames per sample word */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_2;
        }
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#else
        else {
          MSG_ERROR("frames_1 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
        }
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_FRAMES_2
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_FRAMES_2:
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->header_read += 1;

        /* The second byte of the number of frames per sample must be 0x00
        ** for AMR-WB+: The second byte is the MSB of the little-endian word
        */
        if (cur_byte == 0x00) {
          /* Now go get the CMX packet data length */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_LENGTH;
        }
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#else
        else {
          MSG_ERROR("frames_2 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
        }
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */
        break;

      /*---------------------------------------------------------------------
      ** STATE_CMX_HDR_LENGTH
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_CMX_HDR_LENGTH:
#ifdef AUDAMRWB_AMR_WBPLUS_AUDIO_DEC_AV_SYNC
        /* Check if requirements are met before processing this state */
        if (audamrwb_pkt_get_free(pkt_ctl) < (1 * AUDAMRWB_DSP_PKT_HDR_LEN)) {
          exit_flag = TRUE;
          break;
        }
#endif /* AUDAMRWB_AMR_WBPLUS_AUDIO_DEC_AV_SYNC */

        /* Consume up to 4 bytes when possible */
        read_len = AUDAMRWB_CMX_PKT_HDR_LEN - cur_cmx_pkt_ptr->header_read;
        read_len = MIN(read_len, *cmx_buf_len);

        if (read_len > 0) {
          audamrwb_cmx_buf_consume(
            &cmx_buf, &cmx_buf_len,
            (((uint8*)&cur_cmx_pkt_ptr->hdr) + cur_cmx_pkt_ptr->header_read),
            read_len);
          cur_cmx_pkt_ptr->header_read += (uint16)read_len;
        }

        /* Wait until we have collected four bytes to form one CMX packet
        ** data length field
        */
        if (cur_cmx_pkt_ptr->header_read >= AUDAMRWB_CMX_PKT_HDR_LEN) {
          MSG_LOW("info: CMX packet header found %+d byte(s)",
                  cur_cmx_pkt_ptr->hdr.num_bytes, 0, 0);

#ifdef AUDAMRWB_AMR_WBPLUS_AUDIO_DEC_AV_SYNC
          /* Discard (8) bytes: We're done reading the CMX packet header:
          ** Inform the DSP to account for the metadata read to the A/V sync
          ** bytes consumed counter
          */
          status = audamrwb_pkt_packet_create(
                     pkt_ctl, AUDAMRWB_PKT_IF_DSP_VIRTUAL, NULL, NULL);
          if (AUDAMRWB_IS_OK(status)) {
            status = audamrwb_pkt_packet_write(
                       pkt_ctl, 0, NULL, 8, NULL, TRUE);
            if (AUDAMRWB_IS_OK(status)) {
              status = audamrwb_pkt_packet_close(pkt_ctl, NULL);
            }
          }
          if (AUDAMRWB_IS_BAD(status)) {
            MSG_ERROR("could not write a VPKT 0:0x%08X", status, 0, 0);
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
            break;
          }
#endif /* AUDAMRWB_AMR_WBPLUS_AUDIO_DEC_AV_SYNC */
          cur_cmx_pkt_ptr->header_read = 0;

          /* CMX packet data parsing is done if it's 0-lengthed */
          if (cur_cmx_pkt_ptr->hdr.num_bytes == 0) {
            /* Start looking for the next CMX packet: The current packet is
            ** empty
            */
            MSG_MED("warning: empty CMX packet received, find sync_1", 0, 0,
                    0);
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1;
            break;
          }

          /* Reset the current CMX packet data processing fields */
          cur_cmx_pkt_ptr->data_read = 0;
          cur_cmx_pkt_ptr->frame_count = 0;

          /* Reset the current frame processing fields */
          cur_frm_ptr->header_read = 0;
          cur_frm_ptr->frame_count = 0;

          /* Now that we have all the CMX packet header, we are ready to
          ** parse for some frames */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_HDR_1;
        } else {
          /* Stay in this state until we have the data length field */
          /* parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_LENGTH; */
        }
        break;

#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY
#error CMX packet error recovery feature is not available.
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_CMX_PKT_RECOVERY */

      /*---------------------------------------------------------------------
      ** STATE_FRM_HDR_1
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_FRM_HDR_1:
        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->data_read += 1;
        cur_frm_ptr->header_read += 1;

        /* Store the current byte to the first byte of the frame header */
        ((uint8*)&(cur_frm_ptr->hdr.awp_hdr))[0] = cur_byte;

        /* Setup the decoder output sample rate based on the first frame
        ** parsed in the current playback session
        */
        if (parse_ctl->need_rate == TRUE) {
          if (audamrwb_amr_is_mixed_frame(cur_frm_ptr->hdr.awp_hdr.frame_type)) {
            /* Force the sample rate to 16kHz for AMR-WB and AMR-WB/WB+
            ** mixed-mode content
            */
            parse_ctl->config.sample_rate = SND_SAMPLE_RATE_16000;
          }
          parse_ctl->need_rate = FALSE;
        }

        /* Calculate the frame length */
        cur_frm_ptr->data_len = audamrwb_calc_frame_data_len(
                                  cur_frm_ptr->hdr.awp_hdr.frame_type);

        /* Verify that we have enough bytes left to form one storage unit:
        ** second byte of the frame header and the frame data
        */
        remaining_len = (cur_cmx_pkt_ptr->hdr.num_bytes -
                         cur_cmx_pkt_ptr->data_read);

        if (remaining_len < (1 + cur_frm_ptr->data_len)) {
#ifdef FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY
#error Basic AMR-WB+ playback error recovery feature is not available.
#else
          MSG_ERROR("hdr_1 state: detected data corruption", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
#endif /* FEATURE_AUDAMRWB_AUDIO_DEC_BASIC_RECOVERY */
          break;
        }

        /* Now go read the second byte of the frame header */
        parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_HDR_2;
        break;

      /*---------------------------------------------------------------------
      ** STATE_FRM_HDR_2
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_FRM_HDR_2:
        /* Check if requirements are met before processing this state */
        if (audamrwb_pkt_get_free(pkt_ctl) < (2 * AUDAMRWB_DSP_PKT_HDR_LEN)) {
          exit_flag = TRUE;
          break;
        }

        /* Consume 1 byte */
        audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, &cur_byte, 1);
        cur_cmx_pkt_ptr->data_read += 1;
        cur_frm_ptr->header_read += 1;

        /* Store the current byte to the second byte of the frame header */
        ((uint8*)&(cur_frm_ptr->hdr.awp_hdr))[1] = cur_byte;

#ifdef AUDAMRWB_AMR_WBPLUS_AUDIO_DEC_AV_SYNC
        /* Discard (2) bytes: inform the DSP to account for the frame header to the A/V sync
        ** bytes consumed counter.
        */
        status = audamrwb_pkt_packet_create(
                   pkt_ctl, AUDAMRWB_PKT_IF_DSP_VIRTUAL, NULL, NULL);
        if (AUDAMRWB_IS_OK(status)) {
            status = audamrwb_pkt_packet_write(
                       pkt_ctl, 0, NULL, 2, NULL, TRUE);
          if (AUDAMRWB_IS_OK(status)) {
            status = audamrwb_pkt_packet_close(pkt_ctl, NULL);
          }
        }
        if (AUDAMRWB_IS_BAD(status)) {
          MSG_ERROR("could not write a VPKT 0:0x%08X", status, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
          break;
        }
#endif /* AAUDAMRWB_AMR_WBPLUS_AUDIO_DEC_AV_SYNC */

        cur_frm_ptr->header_read = 0;

        /* Check for inconsistent frame type and ISF: Bad ISF does not affect
        ** our parsing or the DSP's ability to recover from this
        */
        if (((audamrwb_amr_is_wbp_frame(cur_frm_ptr->hdr.awp_hdr.frame_type) &&
              audamrwb_amr_is_wbp_isf(cur_frm_ptr->hdr.awp_hdr.isf)) ||
             (audamrwb_amr_is_mixed_frame(cur_frm_ptr->hdr.awp_hdr.frame_type) &&
              audamrwb_amr_is_mixed_isf(cur_frm_ptr->hdr.awp_hdr.isf))) == FALSE) {
          MSG_ERROR("warning: ISF inconsistent with frame type", 0, 0, 0);
        }

        /* Increment the frames found counters */
        cur_cmx_pkt_ptr->frame_count += 1;
        cur_frm_ptr->frame_count += 1;

        /* Reset the current frame's data read field */
        cur_frm_ptr->data_read = 0;

        /* Create a new pseudoraw packet */
        dsp_pkt_hdr = &pkt_if_info.dsp_pseudoraw.hdr;
        dsp_pkt_hdr->frame_type = cur_frm_ptr->hdr.awp_hdr.frame_type;
        dsp_pkt_hdr->isf = cur_frm_ptr->hdr.awp_hdr.isf;

        status = audamrwb_pkt_packet_create(
                   pkt_ctl, AUDAMRWB_PKT_IF_DSP_PSEUDORAW, &pkt_if_info, NULL);
        if (AUDAMRWB_IS_BAD(status)) {
          MSG_ERROR("could not create a new PKT", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
          break;
        }

        /* Now go get the frame's data */
        parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_DATA;
        break;

      /*---------------------------------------------------------------------
      ** STATE_FRM_DATA
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_FRM_DATA:
        /* Check if requirements are met before processing this state */
        if (audamrwb_pkt_get_free(pkt_ctl) == 0) {
          exit_flag = TRUE;
          break;
        }

        /* Consume the remaining frame data */
        read_len = audamrwb_pkt_get_free(pkt_ctl);
        read_len = MIN(read_len, (cur_frm_ptr->data_len -
                                  cur_frm_ptr->data_read));
        read_len = MIN(read_len, (cur_cmx_pkt_ptr->hdr.num_bytes -
                                  cur_cmx_pkt_ptr->data_read));
        read_len = MIN(read_len, *cmx_buf_len);

        if (read_len > 0) {
          /* Append read data to the current packet's data payload */
          (void) audamrwb_pkt_packet_write(
                   pkt_ctl, audamrwb_pkt_packet_get_len(pkt_ctl),
                   cmx_buf, read_len, NULL, TRUE);

          /* Consume the data */
          audamrwb_cmx_buf_consume(&cmx_buf, &cmx_buf_len, NULL, read_len);
          cur_cmx_pkt_ptr->data_read += ((uint16)read_len);
          cur_frm_ptr->data_read += ((uint16)read_len);
        }

        /* Check if we are done reading the frame data */
        if (cur_frm_ptr->data_read >= cur_frm_ptr->data_len) {
          /* Force the transcoder to run, in case the CMX buffer is empty, to
          ** get to the next state
          */
          parse_ctl->run_sm = TRUE;

          /* Data payload is complete; close the packet */
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_END;
        } else {
          /* Stay in this state until we are done reading the frame data */
          /* parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_DATA; */
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_FRM_END
      **---------------------------------------------------------------------
      */
      case AUDAMRWB_PARSE_STATE_FRM_END:
        /* Close the current packet (aligned as needed) */
        status = audamrwb_pkt_packet_close(pkt_ctl, NULL);
        if (AUDAMRWB_IS_BAD(status)) {
          if (status == AUDAMRWB_STATUS_ERR_NOSPACE) {
            /* Wait until the circular packet buffer has more free space */
            MSG_MED("stalling packet close; not enough space", 0, 0, 0);
            parse_ctl->run_sm = TRUE;
            exit_flag = TRUE;
            break;
          } else {
            MSG_ERROR("could not close packet", 0, 0, 0);
            parse_ctl->run_sm = FALSE;
            parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FAILURE;
            break;
          }
        } else {
          parse_ctl->run_sm = FALSE;
        }

        /* Check if there are more frames to parse in the current CMX packet
        */
        if (cur_cmx_pkt_ptr->data_read < cur_cmx_pkt_ptr->hdr.num_bytes) {
          /* Still more frames to parse from the current CMX packet */
          MSG_LOW("info: parsed one storage unit, continue frm_hdr_1", 0, 0,
                  0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_FRM_HDR_1;
        } else {
          /* No more frames to parse from the current CMX packet */
          MSG_LOW("info: parsed final storage unit, find sync_1", 0, 0, 0);
          parse_ctl->parse_state = AUDAMRWB_PARSE_STATE_CMX_HDR_SYNC_1;
        }
        break;

      /*---------------------------------------------------------------------
      ** STATE_FAILURE
      **---------------------------------------------------------------------
      */
      default:
      case AUDAMRWB_PARSE_STATE_FAILURE:
        parse_ctl->status = AUDAMRWB_STATUS_ERR_BADSTATE;
        parse_ctl->run_sm = FALSE;
        exit_flag = TRUE;
        break;
    }
  }
} /* audamrwb_transcode_awp_stream */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_transcode_stream

DESCRIPTION
  Parses and converts CMX interface packets, from the indicated CMX buffer, to
  DSP interface packets.

  The parse_ctl->run_sm will be true when the last DSP interface packet is
  still completing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC void audamrwb_transcode_stream (
  audamrwb_parse_ctl_type *parse_ctl,
  audamrwb_cmx_buf_type   *buffer
) {
  uint32 bytes_total = buffer->data_len;
  uint32 bytes_used;

  /* Parse data from the CMX buffer to the circular packet buffer */
  switch (parse_ctl->operation) {
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case AUDAMRWB_MODE_DEC_AWB_FILE:
      audamrwb_transcode_awb_storage(parse_ctl,
                                      &buffer->data_ptr[buffer->data_idx],
                                      &buffer->data_len);
      break;

    case AUDAMRWB_MODE_DEC_AWB_STREAM:
      audamrwb_transcode_awb_stream(parse_ctl,
                                    &buffer->data_ptr[buffer->data_idx],
                                    &buffer->data_len);
      break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case AUDAMRWB_MODE_DEC_AWP_STREAM:
      audamrwb_transcode_awp_stream(parse_ctl,
                                    &buffer->data_ptr[buffer->data_idx],
                                    &buffer->data_len);
      break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

    default:
      MSG_ERROR("cmx_parse: operation not supported 0:0x%08X",
                parse_ctl->operation, 0, 0);
  }

  /* Calculate the number of bytes consumed from parsing */
  bytes_used = bytes_total - buffer->data_len;

#if defined(AUDAMRWB_DBG_CAPTURE_CMX_PKTS) && defined(FEATURE_EFS)
  /* Dump the CMX buffer consumed to file */
  audamrwb_dbg_io_write(audamrwb_dbg_cmx_dumpfile,
                        &buffer->data_ptr[buffer->data_idx], bytes_used);
#endif /* AUDAMRWB_DBG_CAPTURE_CMX_PKTS && FEATURE_EFS */

  /* Update the data indexer */
  buffer->data_idx += bytes_used;
} /* audamrwb_transcode_stream */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_transfer_cmx2adec

DESCRIPTION
  Transcodes the received CMX buffers to the currently configured stream type.
  Requests for more data from CMX as needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
STATIC audmain_status_type audamrwb_transfer_cmx2adec (
  audamrwb_parse_ctl_type *parse_ctl,
  uint32                  *time_remain_ptr
) {
  audmain_status_type    ret_val = AUDMAIN_STATUS_SUCCESS;
  audamrwb_cmx_buf_type *cur_buffer = &parse_ctl->cmx_ctl.cmx_buf;
  audmain_cmd_buf_type  *cmd_buf = parse_ctl->cmd_buf;

  /* Parse the CMX buffer to the circular packet buffer */
  audamrwb_transcode_stream(parse_ctl, cur_buffer);

  /* Adjust the adec_cmd structure */
  (void) audamrwb_pkt_ready_read(&parse_ctl->pkt_ctl,
                                 &cmd_buf->adec_cmd.adec_buf1,
                                 &cmd_buf->adec_cmd.adec_num_buf1,
                                 &cmd_buf->adec_cmd.adec_buf2,
                                 &cmd_buf->adec_cmd.adec_num_buf2);

  /* Request for more CMX data */
  if (cur_buffer->data_len == 0) {
    /* CMX buffer underrun: Both CMX buffers are empty, now determine whether
    ** we need to request for more data
    */
    if (parse_ctl->cmx_ctl.track_pos >= parse_ctl->cmx_ctl.track_len) {
      /* Playback ending conditions */
      if ((cur_buffer->loading == FALSE) &&
          (audamrwb_pkt_is_ready(&parse_ctl->pkt_ctl))) {
        /* We're done when we're past the end of the track and there's no
        ** more data to parse
        */
        MSG_HIGH("cmx2adec: waiting for the ADEC to complete", 0, 0, 0);
        ret_val = AUDMAIN_STATUS_DONE;
      } else {
        /* Wait for the ADEC buffers to finish up playback before exiting */
        MSG_HIGH("cmx2adec: waiting to send the remaining ADEC content", 0, 0,
                 0);
        *time_remain_ptr = AUDAMRWB_CMX_REQ_DELAY_MS;
      }
    } else {
      /* Playback is not done yet */
      if ((parse_ctl->cmx_ctl.cmx_state != AUDAMRWB_CMX_BUF_STATE_UNDERRUN) ||
          (parse_ctl->elapsed_ms >= AUDAMRWB_CMX_REQ_DELAY_MS)) {

        MSG_HIGH("cmx2adec: CMX buffer underrun, ready, 0: %d, 1: %d",
                 parse_ctl->elapsed_ms, AUDAMRWB_CMX_REQ_DELAY_MS, 0);

        /* Perform a data request upon entering the buffer underrun state or
        ** throttle one data request no more than once per delay period
        */
        if (audamrwb_cmx_can_recycle(cur_buffer)) {
          audamrwb_cmx_free(parse_ctl, cur_buffer);
          (void) audamrwb_cmx_request(parse_ctl, cur_buffer);
          MSG_MED("cmx2adec: cur_buf requesting for track_pos=0x%08X, len=%d",
            parse_ctl->cmx_ctl.track_pos, parse_ctl->cmx_ctl.max_read_len, 0);
        }

        parse_ctl->elapsed_ms = 0;
        parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_UNDERRUN;

        *time_remain_ptr = AUDAMRWB_CMX_REQ_DELAY_MS;
      } else {
        /* Calculate the time remaining to the next data request */
        *time_remain_ptr = AUDAMRWB_CMX_REQ_DELAY_MS - parse_ctl->elapsed_ms;

        MSG_HIGH("cmx2adec: CMX buffer underrun, delay, 0: %d, 1: %d",
                 parse_ctl->elapsed_ms, AUDAMRWB_CMX_REQ_DELAY_MS, 0);
      }

      ret_val = AUDMAIN_STATUS_BUFF_ERR;
    }
  } else {
    /* Request for more data when the buffer becomes empty */
    if (audamrwb_cmx_can_recycle(cur_buffer)) {
      audamrwb_cmx_free(parse_ctl, cur_buffer);
      (void) audamrwb_cmx_request(parse_ctl, cur_buffer);
    }
  }

  return ret_val;
} /* audamrwb_transfer_cmx2adec */

/* <EJECT> */
/*---------------------------------------------------------------------------
** Internal Support Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_save_playback_config

DESCRIPTION
  This function saves and verifies that the playback configuration is
  supported.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the playback configuration is supported.
  FALSE - Otherwise.

SIDE EFFECTS
  audamrwb_info is setup.

===========================================================================*/
STATIC boolean audamrwb_save_playback_config (
  audamrwb_parse_ctl_type         *parse_ctl,
  snd_ext_speech_format_spec_type *input_spec
) {
  boolean is_valid;

  switch (input_spec->codec_type.file_type) {
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case SND_CMX_AF_FILE_AWB:
      is_valid = (/* validate the channel type supported */
                  (input_spec->channel_mode == SND_CHANNEL_MONO) &&
                  /* validate the sample rate supported */
                  (input_spec->sample_rate == SND_SAMPLE_RATE_16000));
      break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case SND_CMX_AF_FILE_AMR_WBPLUS:
      is_valid = (/* validate the channel type supported */
                  ((input_spec->channel_mode == SND_CHANNEL_MONO) ||
                   (input_spec->channel_mode == SND_CHANNEL_DUAL)) &&
                  /* validate the sample rate supported */
                  ((input_spec->sample_rate == SND_SAMPLE_RATE_8000) ||
                   (input_spec->sample_rate == SND_SAMPLE_RATE_11025) ||
                   (input_spec->sample_rate == SND_SAMPLE_RATE_16000) ||
                   (input_spec->sample_rate == SND_SAMPLE_RATE_22050) ||
                   (input_spec->sample_rate == SND_SAMPLE_RATE_24000) ||
                   (input_spec->sample_rate == SND_SAMPLE_RATE_32000) ||
                   (input_spec->sample_rate == SND_SAMPLE_RATE_44100) ||
                   (input_spec->sample_rate == SND_SAMPLE_RATE_48000)));

      break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
    default:
      is_valid = FALSE;
      break;
  }

  if (is_valid == TRUE) {
    parse_ctl->config.file_type    = input_spec->codec_type.file_type;
    parse_ctl->config.num_channels = input_spec->channel_mode;
    parse_ctl->config.sample_rate  = input_spec->sample_rate;
  }

  MSG_HIGH("pb_config: ftype=%d, fs_enum=%d, channels_enum=%d",
           input_spec->codec_type.file_type, input_spec->sample_rate,
           input_spec->channel_mode);

  return is_valid;
} /* audamrwb_save_playback_config */

/* <EJECT> */
/*---------------------------------------------------------------------------
** External Function Definitions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_format_play

DESCRIPTION
  Use this function to start streaming playback.

DEPENDENCIES
  audamrwb_info
  audamrwb_parse_ctl

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - If successful.
  AUDMAIN_STATUS_ERROR   - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audamrwb_format_play (
  snd_ext_speech_format_spec_type *input_spec, /* Stream type      */
  uint8*                           buffer,     /* CMX Input buffer */
  snd_af_access_type              *handle,     /* CMX Data handle  */
  audmain_parse_ctl_type          *main_ctl    /* Data buffer info */
) {
  audamrwb_status_type     status = AUDAMRWB_STATUS_FAILURE;
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;
  uint32                   dummy_time_remain;

  /* Validate the input stream type */
  switch (input_spec->codec_type.file_type) {
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case SND_CMX_AF_FILE_AWB:
      MSG_HIGH("play_codec: initiating an AMR-WB stream", 0, 0, 0);
      break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case SND_CMX_AF_FILE_AMR_WBPLUS:
      MSG_HIGH("play_codec: initiating an AMR-WB+ stream", 0, 0, 0);
      break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
    default:
      MSG_ERROR("play_codec: unsupported stream", 0, 0, 0);
      return AUDMAIN_STATUS_FAILURE;
  }

  /* Reset the internal resources */
  audamrwb_reset();

  /* Validate and save the specified playback configuration */
  if (audamrwb_save_playback_config(parse_ctl, input_spec) == FALSE) {
    MSG_ERROR("play_codec: unsupported features used on stream", 0, 0, 0);
    return AUDMAIN_STATUS_FAILURE;
  }

  /* Store a reference to the external decoder buffer structure */
  parse_ctl->cmd_buf = &main_ctl->current_buf;

  /* Initialize the CMX packet buffer management */
  parse_ctl->cmx_ctl.handle = handle;

  parse_ctl->cmx_ctl.track_len    = AUDAMRWB_MAX_TRACK_LEN;
  parse_ctl->cmx_ctl.track_pos    = main_ctl->buffer_size;
  parse_ctl->cmx_ctl.max_read_len = AUDAMRWB_PKT_BUF_SIZE;

  parse_ctl->cmx_ctl.cmx_state         = AUDAMRWB_CMX_BUF_STATE_READY;
  parse_ctl->cmx_ctl.cmx_buf.loading   = FALSE;
  parse_ctl->cmx_ctl.cmx_buf.track_pos = 0;
  parse_ctl->cmx_ctl.cmx_buf.data_ptr  = buffer;
  parse_ctl->cmx_ctl.cmx_buf.data_len  = main_ctl->buffer_size;

  /* Initialize the parser internals */
  switch (input_spec->codec_type.file_type) {
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case SND_CMX_AF_FILE_AWB:
      status = audamrwb_parse_init(parse_ctl, AUDAMRWB_MODE_DEC_AWB_STREAM);
      break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case SND_CMX_AF_FILE_AMR_WBPLUS:
      status = audamrwb_parse_init(parse_ctl, AUDAMRWB_MODE_DEC_AWP_STREAM);
      break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
    default:
      status = AUDAMRWB_STATUS_ERR_UNSUPPORTED;
      break;
  }

  if (AUDAMRWB_IS_BAD(status)) {
    MSG_ERROR("play_codec: could not initialize the parser: 0x%08X", status,
              0, 0);
    return AUDMAIN_STATUS_FAILURE;
  }

  /* Use asynchronous buffering from now on */
  audmain_set_read_sync(FALSE);

  /* Begin parsing the synchronous data received primarily for determining
  ** the decoder output sample rate
  */
  dummy_time_remain = 0;
  (void) audamrwb_transfer_cmx2adec(parse_ctl, &dummy_time_remain);

  if (parse_ctl->need_rate == TRUE) {
    MSG_ERROR("play_codec: could not determine the sample rate", 0, 0, 0);

    /* Setup the state to cause asynchronous buffers to free up in
    ** audamrwb_get_data_cb_req()
    */
    parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_ERROR;
    parse_ctl->status            = AUDAMRWB_STATUS_FAILURE;
    return AUDMAIN_STATUS_FAILURE;
  }

  /* Setup complete */
  MSG_HIGH("play_codec: stream is ready", 0, 0, 0);

  return AUDMAIN_STATUS_SUCCESS;
} /* audamrwb_format_play */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_parse_head

DESCRIPTION
  This function parses the file header for relevant infomation and
  initializes the necessary data structures.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - if successful.
  AUDMAIN_STATUS_FAILURE - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audamrwb_parse_head (
  uint8*                  buffer,  /* CMX Input buffer */
  snd_af_access_type     *handle,  /* CMX Data handle  */
  audmain_parse_ctl_type *main_ctl /* Data buffer info */
) {
  return AUDMAIN_STATUS_FAILURE;
} /* audamrwb_parse_head */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a data request.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  None

===========================================================================*/
boolean audamrwb_get_data_cb_req (
  uint8* *dest_data_ref,
  uint8*  recv_ptr,
  uint32  recv_len,
  uint32  track_pos
) {
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;
  uint8*                   dummy_data_ptr;
  uint32                   dummy_time_remain;
  audamrwb_cmx_buf_type   *cur_buffer;

  /* Store the recieved data to the client specified data pointer */
  if (dest_data_ref != NULL) {
    *dest_data_ref = recv_ptr;
  } else {
    /* Use a dummy data_ptr if the client did not provide one to us */
    MSG_ERROR("data_cb: destination is NULL", 0, 0, 0);
    dest_data_ref  = &dummy_data_ptr;
    *dest_data_ref = recv_ptr;
  }

  /* Determine if the incoming buffer is expected */
  if (dest_data_ref == &parse_ctl->cmx_ctl.cmx_buf.data_ptr) {
    cur_buffer  = &parse_ctl->cmx_ctl.cmx_buf;
  } else {
    /* Jettison the data received because it is not intended for our buffers
    */
    MSG_ERROR("data_cb: misplaced buffer 0:0x%08X 1:0x%08X", dest_data_ref,
              recv_ptr, 0);
    *dest_data_ref = audamrwb_cmx_free_data(parse_ctl, recv_ptr);
    parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_ERROR;
    parse_ctl->status            = AUDAMRWB_STATUS_ERR_MISMATCH;
    return FALSE;
  }

  /* Perform data validation */
  if (recv_ptr == NULL) {
    MSG_ERROR("data_cb: source is NULL: no data received", 0, 0, 0);
    parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_ERROR;
    parse_ctl->status            = AUDAMRWB_STATUS_ERR_NULL;
    /* Fall through: Clean up the current buffer in the buffer management */
  }

  /* Perform buffer management on the received data */
  switch (parse_ctl->cmx_ctl.cmx_state) {
    case AUDAMRWB_CMX_BUF_STATE_LOADING:
    case AUDAMRWB_CMX_BUF_STATE_READY:
    case AUDAMRWB_CMX_BUF_STATE_UNDERRUN:
      /* Update the current buffer in response to receiving data
      */

      cur_buffer->loading   = FALSE;
      cur_buffer->track_pos = parse_ctl->cmx_ctl.track_pos;
      cur_buffer->data_ptr  = recv_ptr;

      /* Total data received mustn't exceed the track length */
      if ((cur_buffer->track_pos + recv_len) < parse_ctl->cmx_ctl.track_len) {
        cur_buffer->data_len = recv_len;
      } else {
        cur_buffer->data_len = (parse_ctl->cmx_ctl.track_len -
                                cur_buffer->track_pos);
      }

      /* Update the next track read position */
      parse_ctl->cmx_ctl.track_pos += cur_buffer->data_len;

      /* Update the CMX buffering state: The underrun state doesn't change
      ** until we actually receive data
      */
      if ((parse_ctl->cmx_ctl.cmx_state != AUDAMRWB_CMX_BUF_STATE_UNDERRUN) ||
          (cur_buffer->data_len > 0)) {
        parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_READY;
      }

      MSG_MED("data_cb: track_pos=0x%08X, recv_len=%d, data_len=%d",
              cur_buffer->track_pos, recv_len, cur_buffer->data_len);

      /* Optimization: Start parsing the received data now since we are in a
      ** less critical area of code. Do not parse the received data here when
      ** we receive too little data to avoid dog time-out from immediate
      ** subsequent data requests.
      */
      if (recv_len > AUDAMRWB_CMX_EARLY_PARSE_THRESH) {
        dummy_time_remain = 0;
        (void) audamrwb_transfer_cmx2adec(parse_ctl, &dummy_time_remain);
      }
      break;

    case AUDAMRWB_CMX_BUF_STATE_FLUSH:
      /* Jettison all data until all buffers are cleared in the flush state
      */

      /* Flush the current buffer */
      audamrwb_cmx_free(parse_ctl, cur_buffer);

      /* Optimization: Immediately begin buffering up data when the flush has
      ** completed
      */
      (void) audamrwb_cmx_request(parse_ctl, cur_buffer);
      break;

    default:
      /* We should never encounter an unhandled state
      */

      MSG_FATAL("data_cb: unexpected state: 0x%08X",
                parse_ctl->cmx_ctl.cmx_state, 0, 0);
      parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_ERROR;
      parse_ctl->status            = AUDAMRWB_STATUS_ERR_BADSTATE;
      /*lint -fallthrough */

    case AUDAMRWB_CMX_BUF_STATE_ERROR:
      /* Jettison the received data in the error state
      */

      MSG_ERROR("data_cb: error state: track_pos: 0x%08X "
                "cur_buffer->track_pos: 0x%08X", track_pos,
                cur_buffer->track_pos, 0);

      /* Flush the current buffer */
      audamrwb_cmx_free(parse_ctl, cur_buffer);
      return FALSE;
  }

  return TRUE;
} /* audamrwb_get_data_cb_req */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_get_sample_rate

DESCRIPTION
  This function returns the sample rate of the current song.

DEPENDENCIES
  None

RETURN VALUE
  snd_sample_rate_type - The sample rate of the file being played.

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audamrwb_get_sample_rate (
  audmain_parse_ctl_type *main_ctl
) {
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;

  return parse_ctl->config.sample_rate;
} /* audamrwb_get_sample_rate */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audamrwb_flush (
  audmain_parse_ctl_type *main_ctl
) {
  audmain_status_type      ret_val = AUDMAIN_STATUS_FAILURE;
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;
  audmain_cmd_buf_type*    cmd_buf;

  if (main_ctl != NULL) {
    if (AUDAMRWB_IS_OK(parse_ctl->status) &&
        (parse_ctl->cmx_ctl.cmx_state != AUDAMRWB_CMX_BUF_STATE_FLUSH)) {
      /* Flush internal buffers */
      switch (parse_ctl->operation) {
#ifdef FEATURE_AMR_WB_AUDIO_DEC
        case AUDAMRWB_MODE_DEC_AWB_STREAM:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
        case AUDAMRWB_MODE_DEC_AWP_STREAM:
          /* Cause the CMX buffer to flush */
          if (parse_ctl->cmx_ctl.cmx_buf.loading == TRUE) {
            parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_FLUSH;
          } else {
            audamrwb_cmx_free(parse_ctl, &parse_ctl->cmx_ctl.cmx_buf);
          }

          /* Flush the circular packet buffer */
          (void) audamrwb_parse_reset(parse_ctl);
          (void) audamrwb_pkt_clear(&parse_ctl->pkt_ctl);
          cmd_buf = parse_ctl->cmd_buf;
          (void) audamrwb_pkt_ready_read(&parse_ctl->pkt_ctl,
                                         &cmd_buf->adec_cmd.adec_buf1,
                                         &cmd_buf->adec_cmd.adec_num_buf1,
                                         &cmd_buf->adec_cmd.adec_buf2,
                                         &cmd_buf->adec_cmd.adec_num_buf2);

          /* Readjust the track position */
          parse_ctl->cmx_ctl.track_pos = 0;

          ret_val = AUDMAIN_STATUS_SUCCESS;
          break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

        default:
          MSG_ERROR("flush: invalid option: 0x%08X", parse_ctl->operation, 0,
                    0);
      }
    }
  } else {
    MSG_ERROR("flush: bad parameter 0:0x%08X", main_ctl, 0, 0);
  }

  return ret_val;
} /* audamrwb_flush */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_fforward

DESCRIPTION
  This function moves the file playback forward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds forward.

===========================================================================*/
audmain_status_type audamrwb_fforward (
  audmain_parse_ctl_type *main_ctl,
  uint32                  ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  return AUDMAIN_STATUS_FAILURE;
} /* audamrwb_fforward */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_rewind

DESCRIPTION
  This function moves the file playback backward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds backward.

===========================================================================*/
audmain_status_type audamrwb_rewind (
  audmain_parse_ctl_type *main_ctl,
  uint32                  ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
) {
  return AUDMAIN_STATUS_FAILURE;
} /* audamrwb_rewind */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_reset

DESCRIPTION
  This function resets internal resources after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audamrwb_parse_ctl is cleared.

===========================================================================*/
void audamrwb_reset (void)
{
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;

  /* Free any active CMX buffer */
  audamrwb_cmx_free(parse_ctl, &parse_ctl->cmx_ctl.cmx_buf);

  /* Free the circular packet buffer */
  (void) audamrwb_pkt_deinit(&parse_ctl->pkt_ctl);

  /* Clear out internal structures */
  memset(parse_ctl, 0, sizeof(*parse_ctl));
} /* audamrwb_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_get_length

DESCRIPTION
  This function returns the total duration of the selected file.

DEPENDENCIES
  aac_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audamrwb_get_length (
  audmain_parse_ctl_type *parse_ptr,
  uint32                 *elapsed_ms
) {
  return AUDMAIN_STATUS_FAILURE;
} /* audamrwb_get_length */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_get_song_config

DESCRIPTION
  Returns song-specific playback configuration for the DSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audamrwb_get_song_config (
  audmain_parse_ctl_type          *main_ctl,
  audmain_amr_wb_song_config_type *song_config
) {
  audmain_status_type      ret_val = AUDMAIN_STATUS_SUCCESS;
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;

  /* Map snd_sample_rate_type to sample rate in Hz */
  switch (parse_ctl->config.sample_rate) {
    case SND_SAMPLE_RATE_8000:
      song_config->amr_wb_config.sample_rate = 8000;
      break;

    case SND_SAMPLE_RATE_11025:
      song_config->amr_wb_config.sample_rate = 11025;
      break;

    case SND_SAMPLE_RATE_12000:
      song_config->amr_wb_config.sample_rate = 12000;
      break;

    case SND_SAMPLE_RATE_16000:
      song_config->amr_wb_config.sample_rate = 16000;
      break;

    case SND_SAMPLE_RATE_22050:
      song_config->amr_wb_config.sample_rate = 22050;
      break;

    case SND_SAMPLE_RATE_24000:
      song_config->amr_wb_config.sample_rate = 24000;
      break;

    case SND_SAMPLE_RATE_32000:
      song_config->amr_wb_config.sample_rate = 32000;
      break;

    case SND_SAMPLE_RATE_44100:
      song_config->amr_wb_config.sample_rate = 44100;
      break;

    case SND_SAMPLE_RATE_48000:
      song_config->amr_wb_config.sample_rate = 48000;
      break;

    default:
      MSG_ERROR("song_config: unexpected sample rate 0:0x%08X",
                parse_ctl->config.sample_rate, 0, 0);

      song_config->amr_wb_config.sample_rate = 0;
      ret_val = AUDMAIN_STATUS_FAILURE;
      break;
  }

  /* Map snd_channel_enum_type to the DSP mono or stereo mode */
  if (ret_val == AUDMAIN_STATUS_SUCCESS) {
    switch (parse_ctl->config.num_channels) {
      case SND_CHANNEL_MONO:
        song_config->amr_wb_config.dec_chan_mode = (uint16)(-1);
        break;

      case SND_CHANNEL_DUAL:
        song_config->amr_wb_config.dec_chan_mode = 0;
        break;

      default:
        MSG_ERROR("song_config: unexpected sample channel numbers 0:0x%08X",
                  parse_ctl->config.num_channels, 0, 0);

        song_config->amr_wb_config.dec_chan_mode = 0;
        ret_val = AUDMAIN_STATUS_FAILURE;
        break;
    }
  }

  MSG_HIGH("song_config: fs=%dHz, dec_chan_mode=0x%04X",
           song_config->amr_wb_config.sample_rate,
           song_config->amr_wb_config.dec_chan_mode, 0);

  return ret_val;
} /* audamrwb_get_song_config */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_adec_adjust

DESCRIPTION
  Adjusts to the next set of ADEC buffers after data consumption.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audamrwb_adec_adjust (
  audmain_parse_ctl_type *main_ctl,
  uint32                 *bytes_used
) {
  audmain_status_type      ret_val = AUDMAIN_STATUS_FAILURE;
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;
  audmain_cmd_buf_type    *cmd_buf;
  uint32                   dummy_time_remain;

  if ((main_ctl != NULL) && (bytes_used != NULL)) {
    if ((*bytes_used > 0) && AUDAMRWB_IS_OK(parse_ctl->status)) {
      cmd_buf = parse_ctl->cmd_buf;

#if defined(AUDAMRWB_DBG_CAPTURE_ADEC_PKTS) && defined(FEATURE_EFS)
      if (cmd_buf->adec_cmd.adec_num_buf1 < *bytes_used) {
        audamrwb_dbg_io_write(audamrwb_dbg_adec_dumpfile,
                              cmd_buf->adec_cmd.adec_buf1,
                              cmd_buf->adec_cmd.adec_num_buf1);
        audamrwb_dbg_io_write(audamrwb_dbg_adec_dumpfile,
                              cmd_buf->adec_cmd.adec_buf2,
                              (*bytes_used - cmd_buf->adec_cmd.adec_num_buf1));
      } else {
        audamrwb_dbg_io_write(audamrwb_dbg_adec_dumpfile,
                              cmd_buf->adec_cmd.adec_buf1, *bytes_used);
      }
#endif /* AUDAMRWB_DBG_CAPTURE_ADEC_PKTS && FEATURE_EFS */

      /* Adjust to the next circular packet buffer read positions */
      (void) audamrwb_pkt_ready_sync(&parse_ctl->pkt_ctl, *bytes_used);
      (void) audamrwb_pkt_ready_read(&parse_ctl->pkt_ctl,
                                     &cmd_buf->adec_cmd.adec_buf1,
                                     &cmd_buf->adec_cmd.adec_num_buf1,
                                     &cmd_buf->adec_cmd.adec_buf2,
                                     &cmd_buf->adec_cmd.adec_num_buf2);
      MSG_MED("adec_adjust: consumed %+d byte(s), ready %+d byte(s), free "
              "%+d byte(s)", *bytes_used, parse_ctl->pkt_ctl.ready_len,
              parse_ctl->pkt_ctl.buf_free);

      /* Optimization: Start parsing data from any available CMX buffer to
      ** keep the internal circular packet buffer full
      */
      dummy_time_remain = 0;
      ret_val = audamrwb_transfer_cmx2adec(parse_ctl, &dummy_time_remain);
    }
  } else {
    MSG_ERROR("adec_adjust: bad parameters 0:0x%08X 1:0x%08X", main_ctl,
              bytes_used, 0);
  }

  return ret_val;
} /* audamrwb_adec_adjust */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_parse

DESCRIPTION
  This function parses the CMX buffers for frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audamrwb_parse (
  audmain_parse_ctl_type *main_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *time_remain
) {
  audmain_status_type      ret_val = AUDMAIN_STATUS_SUCCESS;
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;

  if ((main_ctl != NULL) && (cmd_buf != NULL) && (time_remain != NULL)) {
    if (AUDAMRWB_IS_OK(parse_ctl->status)) {
      /* Parse the CMX buffer */
      switch (parse_ctl->cmx_ctl.cmx_state) {
        case AUDAMRWB_CMX_BUF_STATE_LOADING:
          /* Nothing to parse: The no CMX buffer is waiting to receive data */
          break;

        case AUDAMRWB_CMX_BUF_STATE_READY:
        case AUDAMRWB_CMX_BUF_STATE_UNDERRUN:
          /* Perform format conversion from CMX to ADEC packets */
          ret_val = audamrwb_transfer_cmx2adec(parse_ctl, time_remain);
          break;

        case AUDAMRWB_CMX_BUF_STATE_FLUSH:
          /* Nothing to parse: The CMX buffer is waiting to get flushed */
          break;

        case AUDAMRWB_CMX_BUF_STATE_ERROR:
          /* Report an error: No error recovery support for data receive
          ** errors
          */
          ret_val = AUDMAIN_STATUS_ERROR;
          break;

        default:
          /* We should never encounter an unhandled state */
          MSG_FATAL("parse: unexpected state 0:0x%08X",
                    parse_ctl->cmx_ctl.cmx_state, 0, 0);
          parse_ctl->cmx_ctl.cmx_state = AUDAMRWB_CMX_BUF_STATE_ERROR;
          ret_val                      = AUDMAIN_STATUS_ERROR;
          break;
      }
    } else {
      ret_val = AUDMAIN_STATUS_ERROR;
    }
  } else {
    MSG_FATAL("parse: NULL pointer 0:0x%08X 1:0x%08X 2:0x%08X", main_ctl,
              &parse_ctl->cmd_buf->adec_cmd, time_remain);
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return ret_val;
} /* audamrwb_parse */

/* <EJECT> */
/*===========================================================================

FUNCTION audamrwb_age

DESCRIPTION
  This function reports the elapsed and adjusted times.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audamrwb_age (
  audmain_parse_ctl_type *main_ctl,
  uint32                 *elapsed_time,
  uint32                 *adjusted_time
) {
  audamrwb_parse_ctl_type *parse_ctl = &audamrwb_parse_ctl;

  if ((main_ctl != NULL) && (elapsed_time != NULL) &&
      (adjusted_time != NULL)) {
    /* Update the elapsed time when playback starts */
    if ((main_ctl->status_flag & AUDMAIN_MASK_INIT_DONE) != 0) {
      *adjusted_time = *elapsed_time;

      if (parse_ctl->cmx_ctl.cmx_state == AUDAMRWB_CMX_BUF_STATE_UNDERRUN) {
        parse_ctl->elapsed_ms += *elapsed_time;
      }
    } else {
      *elapsed_time  = 0;
      *adjusted_time = 0;
    }
  } else {
    MSG_FATAL("age: NULL pointer 0:0x%08X 1:0x%08X 2:0x%08X", main_ctl,
              elapsed_time, adjusted_time);
  }
} /* audamrwb_age */
#endif /* FEATURE_AMR_WB_AUDIO_DEC || FEATURE_AMR_WBPLUS_AUDIO_DEC */
