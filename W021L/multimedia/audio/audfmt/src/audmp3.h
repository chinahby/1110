#ifndef AUDFMP3_H
#define AUDFMP3_H
/*===========================================================================

           A U D I O   F O R M A T   M P 3   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface MP3
  files with the Audio Format modules.

Copyright (c) 2000-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmp3.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/06    hz     Fixed a mis-merging bug which causes VBR FFWD and 
                   continous playback error.
07/28/06    hz     Added support for ID3V2.
09/19/03    sg     Added interface to playback streaming data.
07/25/02    sg     Modified to use audmpeg services whereever possible.
                   Added audmp3_flush.
10/01/01    st     Modified audmp3_adec_adjust() prototype to make bytes_used
                   passed by reference.
04/04/01    st     Updated audmp3_get_data_cb_req() function prototype.
12/04/00    sm     Added protos for _do_time(), _get_song_config().  Ren ff/rew.
11/27/00    sm     Updated for audmain*.  Added file info support.
11/09/00    sm     Corrected featurization.
11/03/00    sm     Added get_pause/set_pause/set_ff_pause()
10/30/00    sm     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_MP3
#include "audmain.h"    /* Audio Format Data definitions and protos        */
#include "audmaini.h"
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*---------------------------------------------------------------------------
** AudMP3 Public Function Prototypes
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audmp3_format_play

DESCRIPTION
  Use this function to play streaming MP3.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audmp3_format_play (
  snd_cmx_format_spec_type    *input_spec,
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_parse_head

DESCRIPTION
  This function parses the mp3 header for relevant infomation.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audmp3_parse_head (
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *parse_ctl,
  audmain_play_param_type     *param
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming data buffer.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern boolean audmp3_get_data_cb_req (
  uint8  **data, 
  uint8  *buf, 
  uint32 length,
  uint32 pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current mp3 song.

DEPENDENCIES
  mp3_info

RETURN VALUE
  sample rate of the last mp3 file parsed

SIDE EFFECTS
  None

===========================================================================*/
extern snd_sample_rate_type audmp3_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES
  mp3_info must be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmp3_flush (
  audmain_parse_ctl_type   *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_fforward

DESCRIPTION
  This function moves the MP3 playback forward ms milliseconds.

DEPENDENCIES
  mp3_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds forward.

===========================================================================*/
extern audmain_status_type audmp3_fforward (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_forward,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_rewind

DESCRIPTION
  This function moves the MP3 playback backward ms milliseconds.

DEPENDENCIES
  mp3_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds backward.

===========================================================================*/
extern audmain_status_type audmp3_rewind (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_forward,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_reset

DESCRIPTION
  This function resets internal MP3 buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mp3_info, mp3_frame_data_found

===========================================================================*/
extern void audmp3_reset (
  void
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_length

DESCRIPTION
  This function returns the total duration of the selected MP3 file.

DEPENDENCIES
  mp3_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmp3_get_length(
  audmain_parse_ctl_type *parse_ptr,
  uint32 *elapsed_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_song_config

DESCRIPTION
  Returns song-specific information to optimize MP3 playback.

DEPENDENCIES
  mp3_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmp3_get_song_config(
  audmain_parse_ctl_type  *parse_ctl,
  audmpeg_song_config_type *mp3_config
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmp3_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_parse

DESCRIPTION
  This function parses one event of the MP3 file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audmp3_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmp3_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmp3_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
);

/*===========================================================================

FUNCTION audmp3_check_ops_in_progress

DESCRIPTION
  Check whether there is any operation in-progress when do parse_head.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audmp3_check_ops_in_progress (void);

/*===========================================================================

FUNCTION audmp3_abort_calc_time

DESCRIPTION
  This function abort the current get_time operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void audmp3_abort_calc_time (void);
#endif /* FEATURE_MP3 */
#endif /* AUDFMP3_H */

