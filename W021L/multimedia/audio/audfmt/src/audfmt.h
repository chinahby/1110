#ifndef AUDFMT_H
#define AUDFMT_H
/*===========================================================================

              A U D I O   F O R M A T S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the audio formats module.

Copyright (c) 2000 - 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audfmt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
03/10/05    st     Modified audfmt_enable_mfi_processing() prototype to
                   accept 4 byte extension ID.
02/22/05    st     Added audfmt_enable_mfi_processing() function prototype.
02/05/05    st     Added audfmt_dls_get_data_cb_req(), audfmt_load_dls() and
                   audfmt_unload_dls() function prototypes.
11/01/04    st     Added audfmt_midi_poly_mode_type and audfmt_set_poly_mode()
                   function prototype.
08/11/04    st     Added audfmt_check_reset() function prototype; updated 
                   audfmt_reset_calc_time() function prototype.
03/22/04    st     Added audfmt_set_pan(), audfmt_get_final_cmd(),
                   audfmt_play_cb(), audfmt_seek_pause(); updated
                   audfmt_set_tempo(), audfmt_set_tune(), audfmt_pause(),
                   audfmt_reset_sequence(); removed audfmt_get_note_off_cmd(),
                   audfmt_multi_callback().
03/04/04    st     Added prototype for reset calc time.
01/26/04    lg     Modified code to support multi_sequencer.
10/16/03  aw/st    Modified code to support multi-sequence.
09/03/01    st     Modified code to move all the audio decoder code to the
                   audfadec module.
07/04/01    st     Modified audfmt_do_play_cb() function prototype.
04/04/01    st     Modified functionality of audfmt_set_reset_cmd(); added
                   audfmt_get_note_off_cmd() prototype; added length parameter
                   to audfmt_get_data_cb_req(); added support for playing QCP
                   files to the direct MIDI device output interface.
02/28/01    st     Added audfmt_preparse() function prototype; modified
                   audfmt_do_play_cb() to add audfmt_cb_state_type as a 
                   parameter; added audfmt_cb_state_type.
11/27/00  st/sm    Moved much code to audmain*.  Featurized EFS/MIDI/CMX2.0.
07/26/00    st     Added vibrator definitions.
07/25/00    st     Modified audfmt_parse_head() function prototype.
07/22/00    st     Featurized vibrator support; modified audfmt_set_tune
                   function prototype.
07/15/00    st     Added support for user-controlled tempo and tuning; added
                   support for vibrator control; lint updates.
05/09/00    st     Modified audfmt_get_repeat_ms() to audfmt_get_mute().
04/28/00    st     Added audfmt_set_ff_pause() and audfmt_set_ff_start()
                   function prototypes.
04/13/00    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#ifdef FEATURE_MIDI
#include "snd.h"                /* Sound typedefs and prototypes          */
#include "audmaini.h"           /* Audio format typedefs and prototypes   */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_WEBAUDIO
/* This enumerated type is used to construct a pattern for vibrator control */
typedef enum {
  AUDFMT_VIB_ON,          /* Vibrator on state        */
  AUDFMT_VIB_OFF,         /* Vibrator off state       */
  AUDFMT_VIB_RPT,         /* Vibrator repeat sequence */
  AUDFMT_VIB_STOP         /* Vibrator stop            */
} audfmt_vib_ctl_enum_type;

/* This structure is used to indicate a vibrator state for certain duration */
typedef struct {
  audfmt_vib_ctl_enum_type vib_ctl;     /* Vibrator control     */
  uint32                   time_ms;     /* Time in milliseconds */
} audfmt_vib_ctl_type;
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDFMT Public Function Prototypes
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION audfmt_enable_mfi_processing

DESCRIPTION
  This function enables MFi processing and must be called to enable MFi
  parser code every time an MFi file is to be processed.
  This function also sets the 4-byte MFi extension ID to be used. Please
  note that this ID can only be set once, the mfi_id parameter will be
  ignored after the first call to this function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_enable_mfi_processing (
  uint8 *mfi_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset

DESCRIPTION
  This function indicates that playback has ended and audfmt should reset
  its state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audfmt_skipping

===========================================================================*/
extern boolean audfmt_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset_calc_time

DESCRIPTION
  This function stops all sequences that are doing a CALC_TIME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfmt_reset_calc_time ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_check_reset

DESCRIPTION
  This function checks if there are any active sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfmt_check_reset ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tempo

DESCRIPTION
  This function sets the playback tempo factor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audfmt_tempo_mult_pending

===========================================================================*/
extern audmain_status_type audfmt_set_tempo (
  uint32 *tempo_factor
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tune

DESCRIPTION
  This function sets the playback tune factor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_set_tune (
  uint8 *tune_factor
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_pan

DESCRIPTION
  This function sets the playback pan factor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_set_pan (
  uint8 *pan_factor
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_final_cmd

DESCRIPTION
  This function returns the final commands in the global command buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_get_final_cmd (
  audmain_cmd_buf_type *cmd_buf
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming data buffer.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  None

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern boolean audfmt_get_data_cb_req(
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current song.

DEPENDENCIES
  None

RETURN VALUE
  sample rate of the file

SIDE EFFECTS
  None

===========================================================================*/
extern snd_sample_rate_type audfmt_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_play_cb

DESCRIPTION
  This function performs a playback callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_play_cb (
  audmain_parse_ctl_type *parse_ctl,
  snd_af_data_cmd_enum   command,
  uint32                 num,
  void                   *data
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_dls_data_cb_req

DESCRIPTION
  This function is the callback function for DLS waveform data requests.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_dls_data_cb_req (
  uint8  *data,
  uint8  *buf,
  uint32 length,
  void   *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_header_data_cb_req

DESCRIPTION
  This function is the callback function for data read commands while
  parsing file headers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_header_data_cb_req (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_parse_head

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audfmt_parse_ctl
  audfmt_read_sync
  audfmt_buf_size

===========================================================================*/
extern audmain_status_type audfmt_parse_head (
  uint8                   *buffer,
  snd_af_access_type      *handle,
  audmain_play_param_type *param,
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_age

DESCRIPTION
  This function ages the file by the indicated number of milliseconds.

DEPENDENCIES
  midi_tempo
  old_midi_tempo
  cmid_tempo
  old_cmid_tempo
  audfmt_overflow_factor

RETURN VALUE
  None

SIDE EFFECTS
  audfmt_elapsed_ms
  audfmt_elapsed_tick
  old_cmid_tempo
  old_midi_tempo

===========================================================================*/
extern void audfmt_age ( 
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time 
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_parse

DESCRIPTION
  This function parses one time event of a the specified file type.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  audmain_status_type
  time_remain - time, in ms, until next event.

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
extern audmain_status_type audfmt_parse (
  audmain_parse_ctl_type *parse_ctl,
  audmain_cmd_buf_type   *cmd_buf,
  uint32                 *time_remain
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_preparse

DESCRIPTION
  This function preparses one time event of a the specified file type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfmt_preparse (
  audmain_parse_ctl_type   *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_pause_flag

DESCRIPTION
  This function gets pause status of MIDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfmt_get_pause_flag (
  audmain_parse_ctl_type *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_pause

DESCRIPTION
  This function gets pause status of MIDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfmt_get_pause (
  audmain_parse_ctl_type *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_valid_time

DESCRIPTION
  This function gets valid_time status of MIDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfmt_get_valid_time (
  audmain_parse_ctl_type *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_pause

DESCRIPTION
  This function pauses all files being played.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_pause (
  audmain_parse_ctl_type *parse_ctl,
  boolean                pause_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_seek_pause

DESCRIPTION
  This function pauses all files being played in preparation for seek command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_seek_pause (
  audmain_parse_ctl_type *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_fforward

DESCRIPTION
  This function moves the playback forward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_fforward (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_forward,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_rewind

DESCRIPTION
  This function moves the playback back ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_rewind (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 ms_rewind,
  uint32                 *elapsed_ms,
  uint32                 *adjusted_ms
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_reset_sequence

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_reset_sequence (
  void     *sequence
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_pause_sequence

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_pause_sequence (
  void     *sequence,
  boolean  pause_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_get_sequence_num

DESCRIPTION
  This function gets the sequence num of given sequence

DEPENDENCIES
  None

RETURN VALUE
  seq num

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_get_sequence_num (
  void     *sequence,
  uint16 *seq_num
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_fforward_sequence

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_fforward_sequence (
  void     *sequence,
  uint32   ms_forward
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_rewind_sequence

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_rewind_sequence (
  void     *sequence,
  uint32   ms_rewind
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tempo_sequence

DESCRIPTION
  This function sets the playback tempo factor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_set_tempo_sequence (
  void     *sequence,
  uint32   tempo_factor
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_tune_sequence

DESCRIPTION
  This function sets the playback tune factor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_set_tune_sequence (
  void     *sequence,
  uint32   tune_factor
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_pan_sequence

DESCRIPTION
  This function sets the playback pan factor of the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_set_pan_sequence (
  void     *sequence,
  uint16   pan_factor
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_set_vol_sequence

DESCRIPTION
  This function sets the volume of the indicated sequence if it has Adec data.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS
  AUDMAIN_STATUS_FAILURE : sequence not active

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_set_vol_sequence (
  void     *sequence,
  uint16   vol_factor
);

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/* This enum type is used to select the mode that MIDI files will be played
** in. This mode setting will have no effect in multisequencer mode.
*/
typedef enum {
  AUDFMT_MIDI_POLY_MODE_HQ,    /* Use high-quality mode for MIDI playback   */
  AUDFMT_MIDI_POLY_MODE_HP     /* Use high-polyphony mode for MIDI playback */
} audfmt_midi_poly_mode_type;

/*===========================================================================

FUNCTION audfmt_set_poly_mode

DESCRIPTION
  This function sets the playback mode for MIDI files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_set_poly_mode (
  audfmt_midi_poly_mode_type mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_adec_channel_done

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_adec_channel_done (
  uint32 channel
);
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION audfmt_dls_get_data_cb_req

DESCRIPTION
  This function handles data callbacks for the DLS processing unit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfmt_dls_get_data_cb_req (
  void      *client_data,
  uint8     *buf,
  uint32    length,
  uint32    position
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_load_dls

DESCRIPTION
  This function loads a DLS file for all sequences to use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_load_dls (
  snd_af_access_type *handle,
  uint32             byte_offset
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_unload_dls

DESCRIPTION
  This function unloads DLS resources reserved for all sequences to use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfmt_unload_dls (
  snd_af_access_type *handle
);
#endif /* FEATURE_DLS */

/*==========================================================================
 FUNCTION audfmt_multi_set_poly_mode

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   Status to indicate if the command is carried out correctly

 SIDE EFFECTS
   None
 ==========================================================================*/
audmain_status_type audfmt_multi_set_poly_mode (
  audmain_audfmt_poly_mode_type mode
);

#endif /* FEATURE_MIDI */
#endif /* AUDFMT_H */
