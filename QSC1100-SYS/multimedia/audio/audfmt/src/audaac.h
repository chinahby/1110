
#ifndef AUDFAAC_H
#define AUDFAAC_H
/*===========================================================================

           A U D I O   F O R M A T   A A C   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface AAC
  files with the Audio Format modules.

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaac.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/06    hz     Fixed a mis-merging bug which causes VBR FFWD and 
                   continous playback error.
07/28/06    hz     Added support for AAC ADTS/VBR song length.
07/20/05    sp     Added support for AAC LOAS.
04/14/04    lg     Modified code to support Animated AAC Ringers.
06/17/02    sg     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */

#ifdef FEATURE_AAC
#include "audmaini.h"   /* Audio Format Data definitions and protos        */
#include "snd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/* <EJECT> */
/*---------------------------------------------------------------------------
** AudAAC Public Function Prototypes
**---------------------------------------------------------------------------
*/
/* <EJECT> */
/*===========================================================================

FUNCTION audaac_format_play

DESCRIPTION
  Use this function to play streaming PV AAC.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
audmain_status_type audaac_format_play (
  snd_aac_format_spec_type    *input_spec, /* Stream format info           */
  uint8                       *buffer,     /* CMX Input buffer             */
  snd_af_access_type          *handle,     /* CMX Data handle              */
  audmain_parse_ctl_type      *parse_ctl   /* data buffer info             */
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_parse_head

DESCRIPTION
  This function parses the AAC header for relevant infomation and initializes
  the necessary data structures.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audaac_parse_head (
  uint8                      *buffer,     /* CMX Input buffer             */
  snd_af_access_type         *handle,     /* CMX Data handle              */
  audmain_parse_ctl_type     *parse_ctl,  /* data buffer info             */
  audmain_play_param_type    *param       /* operation parameter          */ 
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern boolean audaac_get_data_cb_req (
  uint8  **data, 
  uint8  *buf, 
  uint32 length,
  uint32 pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current aac song.

DEPENDENCIES
  aac_info

RETURN VALUE
  sample rate of the last aac file parsed

SIDE EFFECTS
  None

===========================================================================*/
extern snd_sample_rate_type audaac_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES
  aac_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audaac_flush (
  audmain_parse_ctl_type   *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_fforward

DESCRIPTION
  This function moves the AAC playback forward ms milliseconds.

DEPENDENCIES
  aac_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds forward.

===========================================================================*/
extern audmain_status_type audaac_fforward (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_forward,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_rewind

DESCRIPTION
  This function moves the AAC playback backward ms milliseconds.

DEPENDENCIES
  aac_info must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds backward.

===========================================================================*/
extern audmain_status_type audaac_rewind (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_forward,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_reset

DESCRIPTION
  This function resets internal AAC buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  aac_info, aac_frame_data_found

===========================================================================*/
extern void audaac_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_length

DESCRIPTION
  This function returns the total duration of the selected AAC file.

DEPENDENCIES
  aac_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audaac_get_length(
  audmain_parse_ctl_type *parse_ptr,
  uint32                 *elapsed_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_song_config

DESCRIPTION
  Returns song-specific information which is used set up the DSP.

DEPENDENCIES
  aac_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audaac_get_song_config(
  audmain_parse_ctl_type   *parse_ctl,
  audmpeg_song_config_type *aac_config
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audaac_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_parse

DESCRIPTION
  This function parses one event of the AAC file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audaac_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audaac_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audaac_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
);

/* <EJECT> */
/*===========================================================================

FUNCTION audaac_extract_adts_header

DESCRIPTION
  This function extracts the ADTS header from the start of data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void audaac_extract_adts_header(
  byte            *data
);
/* <EJECT> */
/*===========================================================================

FUNCTION audaac_extract_loas_header

DESCRIPTION
  This function extracts the LOAS header from the start of data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void audaac_extract_loas_header(
  byte            *data
);
/* <EJECT> */
/*===========================================================================

FUNCTION audaac_get_animaac_config

DESCRIPTION
  Returns song-specific information which is used to configure Animated AAC 

DEPENDENCIES
  aac_info

RETURN VALUE
  TRUE - Format correct
  FALSE - Wrong format

SIDE EFFECTS
  None

===========================================================================*/
boolean audaac_get_animaac_config(
  audmain_parse_ctl_type   *parse_ctl,
  audmpeg_song_config_type *song_config,
  snd_sample_rate_type        *sample_rate,
  byte                        *data
);

/*===========================================================================

FUNCTION audaac_cleanup_vbr

DESCRIPTION
  If it is ADTS file playback, check and clean up the vbr_parse block.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audaac_cleanup_vbr (void);

/*===========================================================================

FUNCTION audaac_check_ops_in_progress

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
extern boolean audaac_check_ops_in_progress (void);

/*===========================================================================

FUNCTION audaac_abort_calc_time

DESCRIPTION
  This function abort the current get_time operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void audaac_abort_calc_time (void);
#endif /* FEATURE_AAC */
#endif /* AUDFAAC_H */

