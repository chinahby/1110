#ifndef AUDAMRWB_H
#define AUDAMRWB_H
/*===========================================================================

           A M R - W B / W B +   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  AMR-WB/WB+ streams with the Audio Format modules.

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audamrwb.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/06    ay     Initial version.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#if defined(FEATURE_AMR_WB_AUDIO_DEC) || defined(FEATURE_AMR_WBPLUS_AUDIO_DEC)
#include "audmaini.h"   /* Audio Format Data definitions and protos        */
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*---------------------------------------------------------------------------
** Export Functions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audamrwb_format_play

DESCRIPTION
  Use this function to start streaming playback.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS - If successful.
  AUDMAIN_STATUS_ERROR   - Otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audamrwb_format_play (
  snd_ext_speech_format_spec_type *input_spec, /* Stream type      */
  uint8*                           buffer,     /* CMX Input buffer */
  snd_af_access_type              *handle,     /* CMX Data handle  */
  audmain_parse_ctl_type          *main_ctl    /* Data buffer info */
);

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
extern audmain_status_type audamrwb_parse_head (
  uint8*                  buffer,  /* CMX Input buffer */
  snd_af_access_type     *handle,  /* CMX Data handle  */
  audmain_parse_ctl_type *main_ctl /* Data buffer info */
);

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
extern boolean audamrwb_get_data_cb_req (
  uint8* *dest_data_ref,
  uint8*  recv_ptr,
  uint32  recv_len,
  uint32  track_pos
);

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
extern snd_sample_rate_type audamrwb_get_sample_rate (
  audmain_parse_ctl_type *main_ctl
);

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
extern audmain_status_type audamrwb_flush (
  audmain_parse_ctl_type *main_ctl
);

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
extern audmain_status_type audamrwb_fforward (
  audmain_parse_ctl_type *main_ctl,
  uint32                  ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

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
extern audmain_status_type audamrwb_rewind (
  audmain_parse_ctl_type *main_ctl,
  uint32                  ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

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
extern void audamrwb_reset (void);

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
extern audmain_status_type audamrwb_get_length (
  audmain_parse_ctl_type *parse_ptr,
  uint32                 *elapsed_ms
);

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
extern audmain_status_type audamrwb_get_song_config (
  audmain_parse_ctl_type          *main_ctl,
  audmain_amr_wb_song_config_type *song_config
);

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
extern audmain_status_type audamrwb_adec_adjust (
  audmain_parse_ctl_type *main_ctl,
  uint32                 *bytes_used
);

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
extern audmain_status_type audamrwb_parse (
  audmain_parse_ctl_type *main_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *time_remain
);

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
extern void audamrwb_age (
  audmain_parse_ctl_type *main_ctl,
  uint32                 *elapsed_time,
  uint32                 *adjusted_time
);
#endif /* FEATURE_AMR_WB_AUDIO_DEC || FEATURE_AMR_WBPLUS_AUDIO_DEC */
#endif /* AUDAMRWB_H */
