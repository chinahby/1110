#ifndef AUDADPCM_H
#define AUDADPCM_H
/*===========================================================================

      A D P C M    A U D I O   F O R M A T S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the ADPCM audio formats module.

Copyright (c) 2003-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audadpcm.h#1 $ $DateTime: 2008/04/24 13:56:20 $ $Author: yuanz $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/06    pl     Modified audadpcm_adec_adjust() definition
                   for FEATURE_GASYNTH_INTERFACE.
03/22/04    st     Added support for PCM playback and recording; corrected
                   audadpcm_get_length() function prototype.
01/22/04    lg     Added get_time functionality
12/02/03    sg     Added support for linear PCM playback.
02/19/03    aw     Create file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#if defined(FEATURE_WAVE_PB) || defined(FEATURE_PCM_REC)
#include "snd.h"                /* Sound typedefs and prototypes          */
#include "voc.h"                /* Vocoder definitions and interface      */
#include "audmaini.h"           /* Audio format typedefs and prototypes   */
#endif /* FEATURE_WAVE_PB || FEATURE_PCM_REC */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDADPCM Public Function Prototypes
**---------------------------------------------------------------------------
*/
#ifdef FEATURE_WAVE_PB
/*===========================================================================

FUNCTION audadpcm_get_data_cb_req

DESCRIPTION
  This function handles the get data callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audadpcm_parse_ctl
  audadpcm_buf_size

===========================================================================*/
extern boolean audadpcm_get_data_cb_req (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
);

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
extern audmain_status_type audadpcm_format_play (
  snd_cmx_format_spec_type *input_spec,
  uint8                    *buffer,
  snd_af_access_type       *handle,
  audmain_parse_ctl_type   *parse_ctl
);
#endif /* FEATURE_PCM */

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_parse_head

DESCRIPTION
  This function parses and validates the head of a ADPCM file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audadpcm_parse_ctl
  audadpcm_buf_size

===========================================================================*/
extern audmain_status_type audadpcm_parse_head (
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_play_param_type     *param,
  audmain_parse_ctl_type      *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_age

DESCRIPTION
  This function ages the file by the indicated number of milliseconds.
  No aging is necessary for ADPCM files because the timing is driven by the
  decoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audadpcm_age ( 
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time 
);

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_parse

DESCRIPTION
  This function parses the ADPCM file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audadpcm_parse (
  audmain_parse_ctl_type *parse_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_adec_adjust

DESCRIPTION
  This function adjust the buffer status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GASYNTH_INTERFACE
extern audmain_status_type audadpcm_adec_adjust(
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *buf1_used,
  uint32                  *buf2_used,
  uint8                   *buf_usage_mask
);
#else
extern audmain_status_type audadpcm_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *num_used
);
#endif /* FEATURE_GASYNTH_INTERFACE */
/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_reset

DESCRIPTION
  This function indicates that playback has ended and audfmt should reset
  its state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void audadpcm_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audadpcm_get_sample_rate

DESCRIPTION
  This function returns the sampling rate used for qsynth ADPCM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern snd_sample_rate_type audadpcm_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
);

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
extern audmain_status_type audadpcm_get_length (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *time_ms
);

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
extern audmain_status_type audadpcm_flush (
  audmain_parse_ctl_type  *parse_ctl
);

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
extern audmain_status_type audadpcm_fforward (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

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
extern audmain_status_type audadpcm_rewind (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);
#endif /* FEATURE_WAVE_PB */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
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
extern voc_pcm_client_output_fn_type audpcm_record (
  snd_pcm_rec_dir_type         link,
  uint8                        *buf1,
  uint32                       num_buf1,
  uint8                        *buf2,
  uint32                       num_buf2,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

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
extern audmain_status_type audpcm_record_stop (
  snd_pcm_rec_dir_type  link
);

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
extern audmain_status_type audpcm_record_pause (
  boolean               pause,
  snd_pcm_rec_dir_type  link
);

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
extern void audpcm_rec_send_status (
  snd_af_data_cmd_enum status
);

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
extern void audpcm_record_buf (
  uint8       *buffer,
  uint32      num_bytes,
  const void  *client_data
);
#endif /* FEATURE_PCM_REC */
#endif /* AUDADPCM_H */
