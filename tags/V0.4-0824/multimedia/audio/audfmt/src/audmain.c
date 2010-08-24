/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audio Formats Main Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 A U D I O   F O R M A T s   S E R V I C E S
                            M A I N   F I L E

GENERAL DESCRIPTION
  These functions implement audio players.

EXTERNALIZED FUNCTIONS
  audmain_get_data
    Does a playback callback to get the requested data.
  audmain_do_play_cb
    Does a playback callback with the indicated status.
  audmain_reset
    Resets the various parser states.
  audmain_get_parse_type
    Returns the type of file being parsed (MIDI, QCP, MP3, AAC, ...).
  audmain_get_pause
    Checks if the playback is paused.
  audmain_valid_time
    Checks for conditions that indicate that sound should not age the
    indicated time.
  audmain_get_mute
    Checks if there is a mute condition.
  audmain_set_read_sync
    This function sets the data gets to be synchronous and/or asynchronous.
  audmain_set_init_done
    Sets a flag to indicate that the initialization is finished.
  audmain_set_pause
    Sets a flag that indicates that playback is paused.
  audmain_set_ff_start
    Sets a flag that indicates that playback is preparing to start seeking.
  audmain_set_repeat_timer
    Sets the repeat timer.
  audmain_set_tempo
    Sets the playback tempo to the indicated tempo (currently only valid 
    for MIDI type sounds).
  audmain_set_tune
    Sets the tune factor for the current playback.  (currently only 
    valid for MIDI type sounds).
  audmain_get_reset_cmd
    This function returns a command buffer with the MIDI reset command.
  audmain_get_data_cb_req
    Handles the get/read data callback.
  audmain_format_play_init
    Plays files based on specified format type.
  audmain_parse_head
    Parses the head of an audio file.
  audmain_age
    Ages the specified audio file by a number of milliseconds.
  audmain_parse
    Parses one event of the indicated audio file.
  audmain_flush
    Causes the buffers to be flushed, playing to be restarted.
  audmain_mm_record_start 
    Starts a MultiMedia recording session.
  audmain_mm_record_stop
    Stops a MultiMedia recording session.
  audmain_mm_record_pause
    Pauses/resumes a MultiMedia recording session.
  audmain_mm_rec_status 
    Sends the indicated status back to the client.
  audmain_mm_rec_send_status
    Sends the indicated status back to the client.
  audmain_mm_record_buf
    Accepts a buffer for MultiMedia recording.
  audmain_mm_record_get_buf
    This function get a buffer information for recording Multimedia data.
  audmain_mm_record_frame 
    This function records the multimedia encoded frame(s) into buffers.
  audmain_voc_adec_start
    Start audio decoder for QCP or AMR.

  audmain_adec_ppbuf_adjust
    This function adjusts the adec buffer for the ping pong buffer 
    architecture.

  audmain_adec_data_consumed
    This function checks if DSP has done consuming data except partial frame
    data.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2000 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmain.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/09   bk      Modified audmain_parse_head() such that if a unsupported
                   wav file bitstream contains MP3 sync word then its allowed 
                   to go through MP3 parser. This fix works in conjunction with 
                   the modified MP3 parser which searches for minimum two and 
                   maximum of four sync words.
06/18/09   kkm     Type casting (audmain_parse_ctl.current_buf) correctly
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
05/06/09    gs     Added code to skip other format parsing when adpcm
                   format is unsupported, in audmain_parse_head()
01/19/09   bk      Fixed a compilation bug.
08/26/08   anb     Added support for DSP based WMAPRO
07/25/08    ry     Added a check for first data request in audmain_parse_head,
                   if audmain_read_length is zero then fail the playback. CR #
                   146145.
04/11/08   skr     Fixed Thin UI build compilation issues on 6246
02/20/08    hs     Fixed no audio issue if we have big ID3V2 mp3 file from QTV
12/11/07    sk     Modified the changes in audmain_set_init_done()to set the 
                   init done flag AUDMAIN_MASK_INIT_DONE merged by mistake 
                   in previous version. 
11/05/07    sk     Udpated the changes in audmain_set_init_done() to send 
                   SND_CMX_AF_STARTED status to CMX layer. when the parser 
                   is not set to init done flag.
                   CR 120253,124336,124337,120853. 
10/26/07    hs     Added EVW file format support
07/10/07    st     Modified audmain_adec_ppbuf_adjust() to return
                   status information.
04/07/07    pl     Changed audmain_qcp_adec_start() to audmain_voc_adec_start()
                   to accommodate new AMR-NB decoder. 
03/21/07    at     Added EVRC-B file format support.
02/07/07    at     Added AUDMAIN_AMR_WB_AUDIO_TYPE case to various functions
                   for 3GP AWB file support.
09/04/06    ay     Added AMR-WB+ support.
08/15/06    sg     Added sync read API that allows the caller to get tbe
                   length of the data.
08/08/06    st     Added support for rewind and fast-forward for a-law and
                   mu-law playback, CR98411.
08/01/06    hz     Fixed a mis-merging bug which causes VBR FFWD and 
                   continous playback error.
07/28/06    hz     Added support for ID3V2 and AAC ADTS/VBR song length.
06/26/06    pl     Modified various functions to adopt new ping pong buffer
                   data writing scheme. 
06/05/06    sg     Fixed MIDI_OUT featurization.
04/14/06    lg     Added new API for getting the INIT_DONE flag(CR#89914)
03/13/06    st     Added support for delayed start and repeat mode for playback
                   of audio decoder files to MIDI device.
02/13/05    ay     Added support for iMelody pausing.
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
01/09/06    st     Modified code to reset audmain_read_sync properly in
                   audmain_record(), CR85387.
09/18/05   pl/st   Added audmain_get_var_adec_data(), audmain_qcp_adec_start()
                   functions; modified audmain_set_adec_mode() to support
                   multiple audio decoder types simultaneously.
07/25/05    lg     Modify code to do SMAF waveform unpacking in DSP.
07/22/05    sm     Added WAV A/Ulaw support.
07/08/05    pl     Modify audmain_format_play_init() to reset the parse 
                   control structure when failure or error status is 
                   encountered.
05/24/05    hz     Added support for Multimedia record.
04/28/05   ymc     Added 4GV NB support.
03/10/05    st     Modified code to send 4-byte MFi extension ID to audfmt
                   layer.
02/22/05    st     Modified code to add call to enable MFi processing.
02/14/05    hz     MOdified code to support Windows Media Audio decoding;
                   In audmain_reset(),  audmain_get_sample_rate() and
                   audmain_get_data_cb_req() added case handling for Windows
                   Media Audio encode processing.
02/05/05    st     Modified code to support functions from an offset into
                   file; modified code to support HQ DLS; added support for
                   DLS file processing.
11/01/04    st     Modified code to set MIDI playback mode to use high
                   polyphony (HP) synth if FEATURE_MIDI_USE_HP is defined.
08/11/04    st     Modified code to reset state parameters properly after a
                   calc_time() function.
07/02/04    sg     Moved audmp3_parse_head() to be the last function in
                   audmain_parse_head() to reduce the possibility of mistaking
                   AAC as MP3 data.
            st     Modified code to initialize start_aging flag in
                   audmain_parse_head().
04/22/04    lg     Modified code to support Animated AAC Ringers.
04/16/04    st     Modified code to set tune and tempo to default values for
                   unsupported formats.
03/22/04    st     Added support for PCM playback and recording; updated
                   audmain_audfmt_pause_seq() and audmain_audfmt_set_vol_seq()
                   functions to return proper status; updated
                   audmain_get_specs() function.
                   Added audmain_play_cb(), audmain_set_pan(),
                   audmain_get_final_cmd(), audmain_audfmt_pause_seq(),
                   audmain_audfmt_set_vol_seq(),
                   audmain_audfmt_adec_chan_flush(),
                   audmain_check_midi_out_qcp_channel() and
                   audmain_midi_out_qcp_pause() functions; removed
                   audmain_get_wa_audfadec_status(), audmain_get_note_off_cmd(),
                   audmain_multi_callback(); minor updates to
                   audmain_audfmt_adec_chan_pause(), audmain_ff_start(),
                   audmain_set_tempo(), audmain_set_tune(),
                   audmain_reset_sequence().
03/04/04    st     Modified code to support audfmt reset calc time.
01/26/04    lg     Modified code to support multi_sequencer. Added new functions
                   for tune, tempo, pan and volume adjustments.
01/26/04    sg     Modified to send success callbacks for formats not supporting
                   tuning and tempo - status will indicate no change in
                   tuning/tempo.
            aw     Get support to get file specs.
01/22/04    lg     Added get_time functionality for IMY,ADPCM,PCM & QCP formats.
01/21/04    aw     Added support for qsynth volume level mapping.
12/08/03    aw     Fixed audmain_check_midi_out_qcp() feature-ization.
12/02/03    sg     Added support for linear PCM playback.
11/23/03    sg     Added support for RealAudio codec.
10/16/03  aw/st    Modified code to support audfmt multi-sequences.
09/19/03    sg     Added streaming interface for MP3.
04/17/03    sm     Added .AMR file format support.
03/13/03    st     Removed support for AUDMAIN_NO_AUDIO.
03/10/03    st     Updated audmain_audfmt_synth_mvol() and
                   audmain_audfmt_adec_mvol() to check proper status.
02/19/03   aw/st   Added support for ADPCM format; modified
                   audmain_adpcm_start() and added audmain_check_midi_out_qcp()
                   to support simultaneous ADPCM and midi_out_qcp() commands.
01/25/03    aw     Featurized audmain_get_wa_audfadec_status() properly.
11/19/02    st     Added audmain_audfmt_synth_mvol() and
                   audmain_audfmt_adec_mvol() functions.
11/05/02    sg     Added support for iMelody format.
10/28/02    aw     Added audmain_get_wa_audfadec_status function.
09/19/02   ymc     Featurized AUDMAIN_QCP_TYPE under FEATURE_QCP.
08/10/02   aw/st   Added support for ADPCM audio decoder.
07/25/02   sg/st   Added support AAC and AUDMAIN_NO_AUDIO.
                   Added audmain_format_play_init() and audmain_flush().
02/06/02    st     Added audmain_get_seek_overflow() function.
10/08/01    st     Added audmain_get_pause_flag().
10/01/01    st     Modified code to support mulitple audio decoder channels.
09/16/01    st     Added support for setting cnfg chunk in a QCP file.
09/03/01  st/aw    Modified code to use new audfadec module for QSynth audio
                   decoder; modified code to check for NULL callbacks; modified
                   audmain_reset_parms() to only reset the audio parsing
                   portion of control structure; modified audmain_reset() to 
                   always reset type; added code to support QCP recording,
                   playback and supporting functions.
04/26/01    sm     Fixed MIDI featurization
04/04/01    st     Added audmain_init_done().
                   Modified functionality of audmain_set_reset_cmd(); added
                   audmain_get_note_off_cmd(); added length parameter
                   to audmain_get_data_cb_req() and audmain_get_data_cb();
                   added support for playing QCP files to the direct MIDI
                   device output interface; modified code to verify buffer
                   when doing a synchronous data get; lint updates.
02/28/01    st     Added audmain_set_preparse_done(), audmain_preparse() and
                   audmain_do_preparse() functions; modified code to issue
                   a preparse command at the end of audmain_parse_head();
                   modified call to audfmt_do_play_cb() to use the new
                   function interface; lint updates.
12/06/00    st     Added audmain_get_repeat().  audmain_get_mute() doesn't check
                   the repeat time anymore.
12/04/00  st/sm    Added audmain_get_song_config(), audmain_do_time() support
                   for MP3, renamed mp3 ff/rew calls.  Added adec_chan_pan() and
                   adec_chan_vol() for cmx2.0.  snd_audfmt_cmd() now returns
                   snd_status_type.
11/27/00    st     Created

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "task.h"               /* Task definitions and prototypes         */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#ifdef FEATURE_AUDIO_FORMAT
#include "audmain.h"            /* Audio Format definitions and prototypes */
#include "audmaini.h"           /* Audio Format definitions and prototypes */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <string.h>             /* String routines                         */

#ifdef FEATURE_MIDI
#include "audfmt.h"             /* Audio Format definitions and prototypes */
#endif

#ifdef FEATURE_QCP
#include "audqcp.h"             /* Audio Format definitions and prototypes */
#endif

#ifdef FEATURE_MP3
#include "audmp3.h"             /* MP3 definitions and prototypes          */
#endif

#if defined(FEATURE_AAC)|| defined(FEATURE_AAC_REC)
#include "audaac.h"             /* AAC definitions and prototypes          */
#ifdef FEATURE_AAC_REC
#include "audaac_rec.h"         /* AAC record definitions and prototypes   */
#endif /* FEATURE_AAC_REC */
#endif /* FEATURE_AAC || FEATURE_AAC_REC */

#ifdef FEATURE_IMELODY
#include "audimy.h"             /* IMY definitions and prototypes          */
#endif

#if defined(FEATURE_WAVE_PB) || defined(FEATURE_PCM_REC)
#include "audadpcm.h"           /* ADPCM,PCM definitions and prototypes    */
#endif

#ifdef FEATURE_REAL_AUDIO
#include "audra.h"              /* RealAudio definitions and prototypes    */
#endif

#ifdef FEATURE_WMA
#include "audwma.h"        /* Window Media Audio definitions and protoypes */
#endif

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#include "audamrwb.h"
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

#if defined(FEATURE_QCP)     || defined(FEATURE_ADEC) || \
    defined(FEATURE_PCM_REC) || defined(FEATURE_MM_REC)
#include "voc.h"                /* Vocoder typedefs                        */
#endif

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
#include "audfadec.h"           /* Audio Format audio decoder prototypes   */
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#include "sndi.h"               /* Sound internal typedefs                 */

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
audmain_parse_ctl_type audmain_parse_ctl = { AUDMAIN_INVALID_TYPE };

/* This flag indicates whether data reads are done asyncronously or
** synchronously. Data reads at the beginning of a new parse are done
** synchronously, i.e. we do a rex_wait() until our read request has been
** executed.
*/
boolean audmain_read_sync = FALSE;

/* This parameter is used to indicate the buffer that is pending on 
** synchronous data reads.
*/
void   *audmain_read_pending = NULL;
uint32 audmain_read_length;

#ifdef FEATURE_MM_REC
/* Content in av sync report    */
snd_av_report_type  audmain_mm_rec_av_report;
#endif /* FEATURE_MM_REC */

#ifdef FEATURE_MFI_ENABLE
#error code not present
#endif /* FEATURE_MFI_ENABLE */
/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_header_data_cb

DESCRIPTION
  This function is used as the callback to a read/get header data request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_get_header_data_cb (
  void    *audmain_data,
  uint8   *buf,
  uint32  length,
  uint32  position
) {
  snd_audfmt_header_data((uint8 **) audmain_data, buf, length, position);
} /* end audmain_get_header_data_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_header_data

DESCRIPTION
  This function does a read of header data.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_header_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint8                **data
) {
  snd_af_read_cb_type read_cb;
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if((handle != NULL) && (handle->data_func != NULL)) {
    read_cb.client_data = (void *) data;
    read_cb.cb_func     = (snd_af_read_cb_func_type) audmain_get_header_data_cb;

    handle->data_func(SND_CMX_AF_READ, handle->audfmt_ptr, num, &read_cb);

  } else {
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_dls_cb

DESCRIPTION
  This function is used as the callback to a read/get DLS waveform
  data request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_get_dls_cb (
  void   *client_data,
  uint8  *data,
  uint8  *buf,
  uint32 length
) {
  snd_audfmt_dls_data(client_data, data, buf, length);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_dls

DESCRIPTION
  This function issues a command to retrieve DLS waveform data.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_dls (
  snd_af_access_type   *handle,
  uint32               position,
  uint32               length,
  void                 *client_data
) {
  snd_af_get_dls_cb_type dls_cb;
  audmain_status_type    ret_val = AUDMAIN_STATUS_SUCCESS;

  if((handle != NULL) && (handle->data_func != NULL)) {
    dls_cb.position    = position;
    dls_cb.length      = length;
    dls_cb.client_data = client_data;
    dls_cb.cb_func     = (snd_af_get_dls_cb_func_type) audmain_get_dls_cb;

    handle->data_func(SND_CMX_AF_GET_DLS, handle->audfmt_ptr, 0, &dls_cb);

  } else {
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_data_cb

DESCRIPTION
  This function is used as the callback to a read/get data request.

DEPENDENCIES
  audmain_read_sync

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_get_data_cb (
  void    *audmain_data,
  uint8   *buf,
  uint32  length,
  uint32  position
  /*lint -esym(715,position) Suppress "unused" Message */
) {
  if((audmain_read_sync) && (audmain_read_pending == audmain_data)) {
    /* If this is a synchronous call, set the buffer and allow snd task
    ** to continue.
    */
    *((uint8 **) audmain_data) = buf;
    audmain_read_pending = NULL;
    audmain_read_length  = length;
    snd_audfmt_continue();
  } else {
    snd_audfmt_new_data((uint8 **) audmain_data, buf, length, position);
  }
} /* end audmain_get_data_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_async_data

DESCRIPTION
  This function does an asynchronous data read request.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_async_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint8                **data
) {
  snd_af_read_cb_type read_cb;
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if((handle != NULL) && (handle->data_func != NULL)) {
    read_cb.client_data = (void *) data;
    read_cb.cb_func     = (snd_af_read_cb_func_type) audmain_get_data_cb;

    handle->data_func(SND_CMX_AF_READ, handle->audfmt_ptr, num, &read_cb);

  } else {
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return(ret_val);
} /* end audmain_get_async_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_do_get_data

DESCRIPTION
  This function does a read and waits for a signal to continue if in
  synchonous mode, else it just returns.

DEPENDENCIES
  audmain_read_sync

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_do_get_data (
  snd_af_data_cmd_enum command,
  snd_af_access_type   *handle,
  uint32               num,
  uint8                **data
) {
  snd_af_read_cb_type read_cb;
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if((handle != NULL) && (handle->data_func != NULL)) {
    read_cb.client_data = (void *) data;
    read_cb.cb_func     = (snd_af_read_cb_func_type) audmain_get_data_cb;

    if(audmain_read_sync) {
      audmain_read_pending = (void *) data;
    }

    handle->data_func(command, handle->audfmt_ptr, num, &read_cb);

    if(audmain_read_sync) {
      /* If this is a synchronous call, wait for a callback before continuing */
      if(!snd_audfmt_wait()) {
        ret_val = AUDMAIN_STATUS_ERROR;
      }
    }
  } else {
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return(ret_val);
} /* end audmain_do_get_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_data

DESCRIPTION
  This function calls audmain_do_get_data() with SND_CMX_AF_READ.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_data (
  snd_af_access_type *handle,
  uint32             num,
  uint8              **data
) {
  return(audmain_do_get_data(SND_CMX_AF_READ, handle, num, data));
} /* end audmain_get_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_sync_data

DESCRIPTION
  This function calls audmain_do_get_data() with SND_CMX_AF_READ.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_sync_data (
  snd_af_access_type *handle,
  uint32             read_position, /* File position to read from */
  uint8              **data,        /* Data buffer pointer        */
  uint32             *data_length   /* Number of bytes read       */
) {
  if (audmain_read_sync) {
    if (audmain_do_get_data(SND_CMX_AF_READ, handle, read_position, data) ==
        AUDMAIN_STATUS_SUCCESS) {
      *data_length = audmain_read_length;
      return(AUDMAIN_STATUS_SUCCESS);
    }
  }
  *data_length = 0;
  return(AUDMAIN_STATUS_ERROR);
} /* end audmain_get_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_var_data

DESCRIPTION
  This function does a variable length read and waits for a signal to
  continue if in synchonous mode, else it just returns.

DEPENDENCIES
  audmain_read_sync

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_var_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint32               buf_size,
  uint32               offset,
  uint8                **data
) {
  snd_af_read_cb_type read_cb;
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  if((handle != NULL) && (handle->data_func != NULL)) {
    read_cb.client_data = (void *) data;
    read_cb.cb_func     = (snd_af_read_cb_func_type) audmain_get_data_cb;
    read_cb.buffer_size = buf_size;
    read_cb.offset      = offset;

    handle->data_func(SND_CMX_AF_READ_VAR, handle->audfmt_ptr, num, &read_cb);

    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audmain_get_var_data */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION audmain_get_var_adec_data

DESCRIPTION
  This function does a variable length read for ADEC mode. 

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_var_adec_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint32               buf_size,
  uint32               offset,
  uint8                **data
) {
  snd_af_read_cb_type read_cb;
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  if((handle != NULL) && (handle->data_func != NULL)) {
    read_cb.client_data = (void *) data;
    read_cb.cb_func     = (snd_af_read_cb_func_type) audmain_get_data_cb;
    read_cb.buffer_size = buf_size;
    read_cb.offset      = offset;

    handle->data_func(SND_CMX_ADEC_READ_VAR, handle->audfmt_ptr, num,
                      &read_cb);

    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
} /* end audmain_get_var_adec_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_adec_data

DESCRIPTION
  This function calls audmain_do_get_data() with SND_CMX_ADEC_READ.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_adec_data (
  snd_af_access_type *handle,
  uint32             num,
  uint8              **data
) {
  return(audmain_do_get_data(SND_CMX_ADEC_READ, handle, num, data));
} /* end audmain_get_adec_data */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_adec_chan_start

DESCRIPTION
  This function starts the Audio decoder channel.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if command fails
  AUDMAIN_STATUS_SUCCESS - if command succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_audfmt_adec_chan_start (
  uint32  channel
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  if(snd_audfmt_adec_chan_start(channel) == SND_SUCCESS) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_adec_chan_pause

DESCRIPTION
  This function sets the Audio decoder channel pause flag.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if command fails
  AUDMAIN_STATUS_SUCCESS - if command succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_audfmt_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  if(snd_audfmt_adec_chan_pause(channel, pause_flag) == SND_SUCCESS) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_adec_chan_flush

DESCRIPTION
  This function sends a flush command to the Audio decoder channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_audfmt_adec_chan_flush (
  uint32  channel
) {
  snd_audfmt_adec_chan_flush(channel);
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION audmain_audfmt_adec_mvol

DESCRIPTION
  This function sets the Audio decoder master volume.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if command fails
  AUDMAIN_STATUS_SUCCESS - if command succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_audfmt_adec_mvol (
  uint16 vol
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  if(snd_audfmt_adec_mvol(vol) == SND_SUCCESS) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_adec_chan_vol

DESCRIPTION
  This function sets the Audio decoder channel volume.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if command fails
  AUDMAIN_STATUS_SUCCESS - if command succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_audfmt_adec_chan_vol (
  uint32 channel,
  uint16 vol
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  if(snd_audfmt_adec_chan_vol(channel, vol) == SND_SUCCESS) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_adec_chan_pan

DESCRIPTION
  This function sets the Audio decoder channel panning.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if command fails
  AUDMAIN_STATUS_SUCCESS - if command succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_audfmt_adec_chan_pan (
  uint32 channel,
  sint15 pan
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_ERROR;

  if(snd_audfmt_adec_chan_pan(channel, pan) == SND_SUCCESS) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_do_play_cb

DESCRIPTION
  This function performs a playback callback.

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_do_play_cb (
  snd_af_access_type    *handle,
  snd_af_data_cmd_enum  command,
  uint32                num,
  void                  *data
) {
  if((handle != NULL) && (handle->data_func != NULL)) {
    handle->data_func(command, handle->audfmt_ptr, num, data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_play_cb

DESCRIPTION
  This function performs a playback callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_play_cb (
  void                  *state_type,
  snd_af_access_type    *handle,
  snd_af_data_cmd_enum  command,
  uint32                num,
  void                  *data
) {
#ifdef FEATURE_MIDI
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    if(parse_ptr->type == AUDMAIN_MIDI_TYPE) {
      audfmt_play_cb(parse_ptr, command, num, data);
      return;
    }
  }
#endif /* FEATURE_MIDI */

  if((handle != NULL) && (handle->data_func != NULL)) {
    handle->data_func(command, handle->audfmt_ptr, num, data);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_reset_parms

DESCRIPTION
  This function resets all the audmain module parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audmain_parse_ctl

===========================================================================*/
void audmain_reset_parms()
{
  MSG_LOW("audmain_reset_parms",0,0,0);
  audmain_parse_ctl.type          = AUDMAIN_INVALID_TYPE;
  audmain_parse_ctl.status_flag   = 0;
  audmain_parse_ctl.buffer_size   = 0;
  audmain_parse_ctl.tracks        = NULL;

  memset(&audmain_parse_ctl.current_buf, 0, sizeof(audmain_cmd_buf_type));
  memset(&audmain_parse_ctl.song_cfg, 0, sizeof(audmain_song_config_type));
} /* end audmain_reset_parms */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_reset

DESCRIPTION
  This function indicates that playback has ended and the state should 
  be reset.

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_reset ( void )
{
  boolean reset_all = TRUE;

  MSG_LOW("audmain_reset type=%d",audmain_parse_ctl.type,0,0);
  switch (audmain_parse_ctl.type) {
#ifdef FEATURE_MP3
    case AUDMAIN_MP3_TYPE:
      audmp3_reset();
      break;
#endif /* FEATURE_MP3 */

#ifdef FEATURE_AAC
    case AUDMAIN_AAC_TYPE:
      audaac_reset();
      break;
#endif /* FEATURE_AAC */

#ifdef FEATURE_MIDI
    case AUDMAIN_MIDI_TYPE:
      reset_all = audfmt_reset();
      break;
#endif /* FEATURE_MIDI */

#ifdef FEATURE_IMELODY
    case AUDMAIN_IMY_TYPE:
      audimy_reset();
      break;
#endif /* FEATURE_IMELODY */

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
    case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
    case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
    case AUDMAIN_WAV_ALAW_TYPE:
    case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
      audadpcm_reset();
      break;
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_REAL_AUDIO
    case AUDMAIN_RA_TYPE:
      audra_reset();
      break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
    case AUDMAIN_WMA_TYPE:
    case AUDMAIN_WMA_PRO_TYPE:
      audwma_reset();
      break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    case AUDMAIN_AMR_WB_TYPE:
    case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
    case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
      audamrwb_reset();
      break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

    default:
      break;
  }

  if(reset_all) {
    audmain_parse_ctl.type = AUDMAIN_INVALID_TYPE;
    audmain_parse_ctl.status_flag =0x0; 
  }
#if defined(FEATURE_MIDI) || defined(FEATURE_QSYNTH_ADPCM)
  audmain_set_adec_mode(AUDMAIN_ADEC_MODE_NONE);
#endif /* FEATURE_MIDI || FEATURE_QSYNTH_ADPCM */
} /* end audmain_reset */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_reset_calc_time

DESCRIPTION
  This function resets all calc time operations.

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_reset_calc_time ( void ) {
#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    if(audfmt_reset_calc_time()) {
      audmain_parse_ctl.type = AUDMAIN_INVALID_TYPE;
    }
  }
#endif /* FEATURE_MIDI */

#ifdef FEATURE_MP3
  if ((audmain_parse_ctl.type == AUDMAIN_MP3_TYPE) &&
      (audmp3_check_ops_in_progress())) {
    audmp3_abort_calc_time();
  }
#endif /* FEATURE_MP3 */

#ifdef FEATURE_AAC
  if ((audmain_parse_ctl.type == AUDMAIN_AAC_TYPE) &&
      (audaac_check_ops_in_progress())) {
    audaac_abort_calc_time();
  }
#endif /* FEATURE_AAC */
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_parse_type

DESCRIPTION
  This function returns the type of file being parsed.

DEPENDENCIES
  None

RETURN VALUE
  audmain_file_type - Type of file being parsed (MIDI, QCP, MP3, AAC, ...).

SIDE EFFECTS
  None

===========================================================================*/
audmain_file_type audmain_get_parse_type (
  void   *state_type
) {
  audmain_file_type      ret_val = AUDMAIN_INVALID_TYPE;
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    ret_val = parse_ptr->type;
  }

  return(ret_val);
} /* end audmain_get_parse_type */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_pause_flag

DESCRIPTION
  This function returns the state of the pause flag for the indicated
  parse.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if playback is paused
  FALSE - if playback is not paused

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_get_pause_flag (
  void   *state_type
) {
  audmain_parse_ctl_type *parse_ptr;
  boolean                ret_val = FALSE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch (parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_get_pause_flag(parse_ptr);
        break;
#endif /* FEATURE_MIDI */
      default:
        ret_val = ((parse_ptr->status_flag & AUDMAIN_MASK_PAUSE) != 0);
        break;
    }
  }

  return(ret_val);
} /* end audmain_get_pause_flag */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_pause

DESCRIPTION
  This function returns the state of the pause flag for the indicated
  parse.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if playback is paused
  FALSE - if playback is not paused

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_get_pause (
  void   *state_type
) {
  audmain_parse_ctl_type *parse_ptr;
  boolean                ret_val = FALSE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch (parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_get_pause(parse_ptr);
        break;
#endif /* FEATURE_MIDI */
      default:
        ret_val = ((parse_ptr->status_flag & (AUDMAIN_MASK_PAUSE    |
                                              AUDMAIN_MASK_FF_PAUSE |
                                              AUDMAIN_MASK_FF_START)) != 0);
        break;
    }
  }

  return(ret_val);
} /* end audmain_get_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_valid_time

DESCRIPTION
  This function returns 

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if playback is paused
  FALSE - if playback is not paused

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_get_valid_time (
  void   *state_type
) {
  audmain_parse_ctl_type *parse_ptr;
  boolean                ret_val = FALSE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch (parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_get_valid_time(parse_ptr);
        break;
#endif /* FEATURE_MIDI */
      default:
        ret_val = ((parse_ptr->status_flag & (AUDMAIN_MASK_PAUSE    |
                                              AUDMAIN_MASK_FF_PAUSE |
                                              AUDMAIN_MASK_FF_START)) == 0);
        break;
    }
  }

  return(ret_val);
} /* end audmain_get_valid_time */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_sample_rate

DESCRIPTION
  This function returns the sample rate of the current song.

DEPENDENCIES
  None

RETURN VALUE
  Sample rate of the last file parsed

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audmain_get_sample_rate (
  void                     *state_type
) {
  audmain_parse_ctl_type *parse_ptr;
  snd_sample_rate_type   sample_rate = SND_SAMPLE_RATE_NONE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
#ifdef FEATURE_ANIMATED_AAC
        if( parse_ptr->audfadec_mode == AUDMAIN_ADEC_MODE_AAC){
          sample_rate = audfadec_get_sample_rate( parse_ptr);
        }
        else
#endif /* FEATURE_ANIMATED_AAC */
        {
          sample_rate = audfmt_get_sample_rate(parse_ptr);
        }
        break;
#endif

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        sample_rate = SND_SAMPLE_RATE_8000;
        break;
#endif

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        sample_rate = audmp3_get_sample_rate(parse_ptr);
        break;
#endif

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        sample_rate = audaac_get_sample_rate(parse_ptr);
        break;
#endif

#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
        sample_rate = audimy_get_sample_rate(parse_ptr);
        break;
#endif

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
      case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        sample_rate = audadpcm_get_sample_rate(parse_ptr);
        break;
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
        sample_rate = audra_get_sample_rate(parse_ptr);
        break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
        sample_rate = audwma_get_sample_rate(parse_ptr);
        break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        sample_rate = audamrwb_get_sample_rate(parse_ptr);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }

  return(sample_rate);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_read_sync

DESCRIPTION
  This function sets the data requests to synchronous or asynchronous
  mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  audmain_read_sync

===========================================================================*/
void audmain_set_read_sync (
  boolean sync
) {
  audmain_read_sync = sync;
} /* end audmain_set_read_sync */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_init_done

DESCRIPTION
  This function sets the initialization flag to TRUE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_set_init_done (
  void *state_type
) {
  audmain_parse_ctl_type *parse_ptr;
    
  if(state_type != NULL) {
    parse_ptr               = (audmain_parse_ctl_type *) state_type;
    
    if(
#ifdef FEATURE_MIDI
		(parse_ptr->type != AUDMAIN_MIDI_TYPE) &&
#endif 
        !(parse_ptr->status_flag & AUDMAIN_MASK_INIT_DONE)) {
         audmain_do_play_cb(parse_ptr->handle, SND_CMX_AF_STARTED, 0, NULL);  
       }
     parse_ptr->status_flag |= AUDMAIN_MASK_INIT_DONE;
       
  }
} /* end audmain_set_init_done */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_init_done

DESCRIPTION
  This function gets the initialization flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_get_init_done (
  void *state_type
) {
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr               = (audmain_parse_ctl_type *) state_type;
    return ( (parse_ptr->status_flag & AUDMAIN_MASK_INIT_DONE) != 0 );
  }
  return (FALSE);
} /* end audmain_get_init_done */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_pause

DESCRIPTION
  This function sets the pause flag for the indicated parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_set_pause (
  void        *state_type,
  boolean     pause
) {
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

#ifdef FEATURE_MIDI
    if(parse_ptr->type == AUDMAIN_MIDI_TYPE) {
      audfmt_pause(parse_ptr, pause);
      return;
    }
#endif /* FEATURE_MIDI */

#ifdef FEATURE_IMELODY
    if (parse_ptr->type == AUDMAIN_IMY_TYPE) {
      audimy_pause(parse_ptr, pause);
    }
#endif /* FEATURE_IMELODY */

    if(pause) {
      parse_ptr->status_flag |= AUDMAIN_MASK_PAUSE;
    } else {
      parse_ptr->status_flag &= ~AUDMAIN_MASK_PAUSE;
    }
// add pause command for parses; ADPCM and MIDI should be only ones who care
// unset pause flag for MIDI
  }
} /* end audmain_set_pause */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_ff_start

DESCRIPTION
  This function sets the fast-forward preparation pause flag for the
  indicated parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_set_ff_start (
  void *state_type
) {
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

#ifdef FEATURE_MIDI
    if(parse_ptr->type == AUDMAIN_MIDI_TYPE) {
      audfmt_seek_pause(parse_ptr);
      return;
    }
#endif

    parse_ptr->status_flag |= AUDMAIN_MASK_FF_START;
  }
} /* end audmain_set_ff_start */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_tempo

DESCRIPTION
  This function sets the playback tempo factor. ONLY VALID FOR MIDI TYPE
  SOUNDS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_tempo (
  void      *state_type,
  uint32    *tempo_factor
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_set_tempo(tempo_factor);
        break;
#endif /* FEATURE_MIDI */

      case AUDMAIN_INVALID_TYPE:
        break;

      default:
        *tempo_factor = AUDMAIN_TEMPO_DEFAULT;
        ret_val       = AUDMAIN_STATUS_SUCCESS;
        break;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_tune

DESCRIPTION
  This function sets the playback tune factor.  ONLY VALID FOR MIDI TYPE
  SOUNDS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_tune (
  void                  *state_type,
  uint8                 *tune_factor
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch (parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_set_tune(tune_factor);
        break;
#endif /* FEATURE_MIDI */

      case AUDMAIN_INVALID_TYPE:
        break;

      default:
        *tune_factor = AUDMAIN_TUNE_DEFAULT;
        ret_val      = AUDMAIN_STATUS_SUCCESS;
        break;
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_pan

DESCRIPTION
  This function sets the playback pan factor.  ONLY VALID FOR MIDI TYPE
  SOUNDS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_pan (
  void                  *state_type,
  uint8                 *pan_factor
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

#ifdef FEATURE_MIDI
    if(parse_ptr->type == AUDMAIN_MIDI_TYPE) {
      ret_val = audfmt_set_pan(pan_factor);
    }
#endif
  }
  return(ret_val);
}

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION audmain_get_final_cmd

DESCRIPTION
  This function returns the final MIDI commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_get_final_cmd (
  audmain_cmd_buf_type *cmd_buf
) {
  audfmt_get_final_cmd(cmd_buf);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_check_reset_req

DESCRIPTION
  This function checks if parse should be reset.

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_audfmt_check_reset_req ()
{
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    if(audfmt_check_reset()) {
      audmain_parse_ctl.type = AUDMAIN_INVALID_TYPE;
    }
  }
}
#endif /* FEATURE_MIDI */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming data buffer.

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  TRUE  - if a valid buffer, which can be processed immediately, was received
  FALSE - if currently no processing needs to be done on the received buffer

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_get_data_cb_req( 
  uint8  **data, 
  uint8  *buf,
  uint32 length,
  uint32 pos
) {
  boolean ret_val = FALSE;

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
  ret_val = audfadec_get_data_cb_req(data, buf, length, pos);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_QCP
  if(audmain_parse_ctl.qcp_func_enabled) {
    ret_val = audqcp_func_get_data_cb_req(data, buf, length, pos);
  }
#endif /* FEATURE_QCP */

  if(!ret_val) 
  {
    MSG_LOW("audmain_get_data_cb_req type=%d",audmain_parse_ctl.type,0,0);
    switch (audmain_parse_ctl.type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_get_data_cb_req(data, buf, length, pos);
        break;
#endif

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        ret_val = audqcp_get_data_cb_req(data, buf, length, pos);
        break;
#endif

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_get_data_cb_req(data, buf, length, pos);
        break;
#endif

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_get_data_cb_req(data, buf, length, pos);
        break;
#endif

#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
        ret_val = audimy_get_data_cb_req(data, buf, length, pos);
        break;
#endif

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
      case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        ret_val = audadpcm_get_data_cb_req(data, buf, length, pos);
        break;
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
        ret_val = audra_get_data_cb_req(data, buf, length, pos);
        break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
        ret_val = audwma_get_data_cb_req(data, buf, length, pos);
        break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        ret_val = audamrwb_get_data_cb_req(data, buf, length, pos);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }

  return(ret_val);
} /* end audmain_get_data_cb_req */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_header_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming header data buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_header_data_cb_req (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
) {
#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    audfmt_header_data_cb_req(data, buf, length, pos);
  }
#endif /* FEATURE_MIDI */
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_dls_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming DLS data buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_dls_data_cb_req (
  uint8  *data,
  uint8  *buf,
  uint32 length,
  void   *client_data
) {
#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    audfmt_dls_data_cb_req(data, buf, length, client_data);
  }
#endif /* FEATURE_MIDI */
}

/* <EJECT> */
#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/*===========================================================================

FUNCTION audmain_adec_adjust

DESCRIPTION
  This function adjusts the adec buffer.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS -  if
  AUDMAIN_STATUS_FAILURE -  if

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_adec_adjust (
  void                     *state_type,
  uint32                   *num_used
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

#ifndef FEATURE_GASYNTH_INTERFACE
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
    if(parse_ptr->audfadec_enabled) {
      ret_val = audfadec_adjust(parse_ptr, num_used);
    }
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_GASYNTH_INTERFACE */
    switch(parse_ptr->type) {
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_adec_adjust(parse_ptr, num_used);
        break;
#endif

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_adec_adjust(parse_ptr, num_used);
        break;
#endif

#ifndef FEATURE_GASYNTH_INTERFACE
#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
      case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        ret_val = audadpcm_adec_adjust(parse_ptr, num_used);
        break;
#endif /* FEATURE_WAVE_PB */
#endif /* FEATURE_GASYNTH_INTERFACE */
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
        ret_val = audra_adec_adjust(parse_ptr, num_used);
        break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
        ret_val = audwma_adec_adjust(parse_ptr, num_used);
        break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        ret_val = audamrwb_adec_adjust(parse_ptr, num_used);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
#endif /* FEATURE_MIDI */
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
#endif /* FEATURE_QCP */
      case AUDMAIN_INVALID_TYPE:
      default:
        ret_val = AUDMAIN_STATUS_SUCCESS;
        break;
    }
  }

  return(ret_val);
}
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_format_play_init

DESCRIPTION
  This function sets up the appropriate audio play, based on the format
  specified.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS -  if playing was successfully initiated.
  AUDMAIN_STATUS_FAILURE -  if playing could not be initiated.

SIDE EFFECTS
  audmain_parse_ctl
  audmain_read_sync

===========================================================================*/
audmain_status_type audmain_format_play_init (
  snd_af_access_type       *handle,        /* Data handle                */
  snd_cmx_format_spec_type *input_spec,    /* Input format specification */
  void                     *state_ptr_ptr, /* AUDFMT Control buffer      */
  audmain_cmd_buf_type     **cmd_buf       /* Command buffer             */
) {
  audmain_status_type      ret_val;
  snd_cmx_af_spec_type     audfmt_spec;
  uint8                    *buffer;

  ret_val           = AUDMAIN_STATUS_FAILURE;
  audmain_read_sync = TRUE;

  audmain_reset_parms();

  audmain_do_play_cb(handle, SND_CMX_AF_SET_TRACKS, 0,
                     (void *) &audmain_parse_ctl.buffer_size);

  if(audmain_get_data(handle, 0, &buffer) != AUDMAIN_STATUS_SUCCESS) {
    return(ret_val);
  }

  audmain_parse_ctl.buffer_size = audmain_read_length;

  if(buffer == NULL) {
    MSG_FATAL("Null buffer",0,0,0);
    audmain_parse_ctl.type = AUDMAIN_INVALID_TYPE;
  } else {

    MSG_LOW("audmain_format_play_init file_type",input_spec->codec_type.file_type,0,0);
    switch (input_spec->codec_type.file_type) {
#ifdef FEATURE_MP3
      case SND_CMX_AF_FILE_MP3:
        ret_val = audmp3_format_play(input_spec, buffer, handle,
                                     &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_MP3_TYPE;
        break;
#endif /* FEATURE_MP3 */

#ifdef FEATURE_AAC
      case SND_CMX_AF_FILE_AAC:
        ret_val = audaac_format_play((snd_aac_format_spec_type *) input_spec,
                                     buffer, handle, &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_AAC_TYPE;
        break;
#endif /* FEATURE_AAC */

#ifdef FEATURE_QCP
      case SND_CMX_AF_FILE_QCP_13K:
      case SND_CMX_AF_FILE_QCP_EVRC:
      case SND_CMX_AF_FILE_QCP_4GV_NB:
      case SND_CMX_AF_FILE_QCP_AMR:
#ifdef FEATURE_AUDFMT_AMR
      case SND_CMX_AF_FILE_AMR:
#endif /* FEATURE_AUDFMT_AMR */
#ifdef FEATURE_AUDFMT_EVB
      case SND_CMX_AF_FILE_EVB:
#endif /* FEATURE_AUDFMT_EVB */
#ifdef FEATURE_AUDFMT_EVW
      case SND_CMX_AF_FILE_EVW:
#endif /* FEATURE_AUDFMT_EVW */

        ret_val = audqcp_format_play(input_spec, buffer, handle,
                                     &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_QCP_TYPE;
        break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_REAL_AUDIO
      case SND_CMX_AF_FILE_RA:
        ret_val = audra_format_play((snd_ra_format_spec_type *) input_spec,
                                     buffer, handle, &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_RA_TYPE;
        break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_PCM
      case SND_CMX_AF_FILE_PCM:
        ret_val = audadpcm_format_play(input_spec, buffer, handle,
                                       &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_PCM_TYPE;
        break;
#endif /* FEATURE_PCM */

#ifdef FEATURE_VOC_G711
      case SND_CMX_AF_FILE_WAV_ALAW:
        ret_val = audadpcm_format_play(input_spec, buffer, handle,
                                       &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_WAV_ALAW_TYPE;
        break;
      case SND_CMX_AF_FILE_WAV_MULAW:
        ret_val = audadpcm_format_play(input_spec, buffer, handle,
                                       &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_WAV_MULAW_TYPE;
        break;
#endif /* FEATURE_VOC_G711 */

#ifdef FEATURE_WMA
      case SND_CMX_AF_FILE_WMA:
        ret_val = audwma_format_play((snd_wma_format_spec_type *) input_spec,
                                     buffer, handle, &audmain_parse_ctl);
        audmain_parse_ctl.type = AUDMAIN_WMA_TYPE;
        break;
      case SND_CMX_AF_FILE_WMA_PRO:
        ret_val = audwma_format_play((snd_wma_format_spec_type *) input_spec,
                                     buffer, handle, &audmain_parse_ctl);
        audmain_parse_ctl.type = AUDMAIN_WMA_PRO_TYPE;
        break;

#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case SND_CMX_AF_FILE_AWB:
        ret_val = audamrwb_format_play(
                               (snd_ext_speech_format_spec_type *) input_spec,
                                          buffer, handle, &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_AMR_WB_AUDIO_TYPE;
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case SND_CMX_AF_FILE_AMR_WBPLUS:
        ret_val = audamrwb_format_play(
                               (snd_ext_speech_format_spec_type *) input_spec,
                                          buffer, handle, &audmain_parse_ctl);

        audmain_parse_ctl.type = AUDMAIN_AMR_WBPLUS_TYPE;
        break;
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */

      default:
        MSG_ERROR("Unknown audio format %d",
                  input_spec->codec_type.file_type, 0, 0);
    }
  }

  if(ret_val == AUDMAIN_STATUS_FAILURE || ret_val == AUDMAIN_STATUS_ERROR) {
    audmain_reset_parms();

    if (ret_val == AUDMAIN_STATUS_FAILURE) {
      audfmt_spec.file_type = SND_CMX_AF_FILE_UNKNOWN;
      audmain_do_play_cb ( handle, SND_CMX_AF_AUDIO_SPEC, 0, &audfmt_spec );
    }

    audmain_do_play_cb ( handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);
  } else if(ret_val == AUDMAIN_STATUS_SUCCESS) {
    audmain_parse_ctl.status_flag = 0;
    audmain_parse_ctl.handle = handle;
    if(cmd_buf != NULL) {
      *(audmain_cmd_buf_type**)cmd_buf = (audmain_cmd_buf_type*)&(audmain_parse_ctl.current_buf);
    }

    if(state_ptr_ptr != NULL) {
      *(void **) state_ptr_ptr = (void *) &(audmain_parse_ctl);
    }
  } else {
    MSG_FATAL("Invalid return status", 0, 0, 0);
    audmain_reset_parms();
  }

  audmain_read_sync = FALSE;

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_parse_head

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  If a new module is added in this function and which doesn't support
  SND_AUDIO_SPEC callback function. Then audmain_get_specs function need
  to return status AUDMAIN_STATUS_ERROR for this module. 

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS -  if the file's header was parsed and recognized 
                            successfully
  AUDMAIN_STATUS_FAILURE -  if the the file was not parsed or recognized 
                            successfully

SIDE EFFECTS
  audmain_parse_ctl
  audmain_read_sync

===========================================================================*/
audmain_status_type audmain_parse_head (
  snd_af_access_type      *handle,
  void                    *state_ptr_ptr,
  audmain_cmd_buf_type    **cmd_buf,
  audmain_play_param_type *param
) {
  audmain_status_type    ret_val;
  snd_cmx_af_spec_type   audfmt_spec;
  uint8                  *buffer;
#ifdef FEATURE_MIDI
  audmain_play_param_type local_param;
#endif /* FEATURE_MIDI */
  audmain_status_type audmain_adpcm_status = AUDMAIN_STATUS_FAILURE;

  if((param->calc_time) && (audmain_parse_ctl.type != AUDMAIN_INVALID_TYPE)
#ifdef FEATURE_MIDI
     && (audmain_parse_ctl.type != AUDMAIN_MIDI_TYPE)
#endif /* FEATURE_MIDI */
    ) {
    return(AUDMAIN_STATUS_FAILURE);
  }

  audmain_read_sync = TRUE;

  audmain_do_play_cb(handle, SND_CMX_AF_SET_TRACKS, 0,
                                     (void *) &audmain_parse_ctl.buffer_size);

  if(audmain_get_data(handle, param->byte_offset, &buffer) !=
     AUDMAIN_STATUS_SUCCESS) {
    audmain_read_sync = FALSE;
    return(AUDMAIN_STATUS_FAILURE);
  }

  audmain_parse_ctl.buffer_size = audmain_read_length;

  if((buffer != NULL) && (audmain_read_length != 0)) {
#ifdef FEATURE_MIDI
#ifdef FEATURE_MFI_ENABLE
#error code not present
#endif /* FEATURE_MFI_ENABLE */

    local_param = *param;
#ifdef FEATURE_SMAF_DSP_WAVEFORM_UNPACKING
    local_param.packed_dls_wf_mode = TRUE;
#else
    local_param.packed_dls_wf_mode = FALSE;
#endif /* FEATURE_SMAF_DSP_WAVEFORM_UNPACKING */
    ret_val = audfmt_parse_head(buffer, handle, &local_param,
                                                       &audmain_parse_ctl);

    if(ret_val == AUDMAIN_STATUS_SUCCESS) {
#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_MIDI_USE_HP
      audfmt_set_poly_mode(AUDFMT_MIDI_POLY_MODE_HP);
#endif /* FEATURE_MIDI_USE_HP */
#endif /* FEATURE_WEBAUDIO */
      audmain_parse_ctl.type = AUDMAIN_MIDI_TYPE;
      if(cmd_buf != NULL) {
        audmain_parse_ctl.current_buf.start_aging = FALSE;
        *(audmain_cmd_buf_type**)cmd_buf = (audmain_cmd_buf_type*)&(audmain_parse_ctl.current_buf);
      }

      if(state_ptr_ptr != NULL) {
        *(void **) state_ptr_ptr = (void *) &(audmain_parse_ctl);
      }

      audmain_read_sync = FALSE;
      return(AUDMAIN_STATUS_SUCCESS);
    }
#endif /* FEATURE_MIDI */

    if((param->multiseq_flag)
#ifdef FEATURE_MIDI
       || ((param->calc_time) &&
            (audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE))
#endif /* FEATURE_MIDI */
      ) {
      audmain_do_play_cb(handle, SND_CMX_AF_FAILURE, 0, NULL);
      audmain_read_sync = FALSE;
      return(AUDMAIN_STATUS_FAILURE);
    }

    audmain_reset_parms();
    audmain_parse_ctl.buffer_size = audmain_read_length;

#ifdef FEATURE_QCP
    if(audqcp_parse_head(buffer, handle, param->start_position,
                         &audmain_parse_ctl) == AUDMAIN_STATUS_SUCCESS) {
      audmain_parse_ctl.type = AUDMAIN_QCP_TYPE;
    } else
#endif
#ifdef FEATURE_IMELODY
    if(audimy_parse_head(buffer, handle, &audmain_parse_ctl) ==
       AUDMAIN_STATUS_SUCCESS) {
      audmain_parse_ctl.type = AUDMAIN_IMY_TYPE;
    } else
#endif
#ifdef FEATURE_WAVE_PB
    if((audmain_adpcm_status = audadpcm_parse_head(buffer, handle, param, &audmain_parse_ctl)) ==
       AUDMAIN_STATUS_SUCCESS) {
      /* type is set in audadpcm_parse_head() */
    }
    else
#endif /* FEATURE_WAVE_PB */
#ifdef FEATURE_AAC
    if(audaac_parse_head(buffer, handle, &audmain_parse_ctl, param) ==
       AUDMAIN_STATUS_SUCCESS) {
      audmain_parse_ctl.type = AUDMAIN_AAC_TYPE;
    } else
#endif
#ifdef FEATURE_MP3
    if (audmp3_parse_head(buffer, handle, &audmain_parse_ctl,param) ==
       AUDMAIN_STATUS_SUCCESS) {
      audmain_parse_ctl.type = AUDMAIN_MP3_TYPE;
    } else
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
    if(audamrwb_parse_head(buffer, handle, &audmain_parse_ctl) ==
       AUDMAIN_STATUS_SUCCESS) {
      audmain_parse_ctl.type = AUDMAIN_AMR_WB_TYPE;
    } else
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
    {
      MSG_ERROR("Unrecognized audio type", 0,0,0);
    }

    if(audmain_parse_ctl.type == AUDMAIN_INVALID_TYPE) {
      audfmt_spec.file_type = SND_CMX_AF_FILE_UNKNOWN;
      audmain_do_play_cb ( handle, SND_CMX_AF_AUDIO_SPEC, 0, &audfmt_spec );

      audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, (void *) buffer);
      ret_val = AUDMAIN_STATUS_FAILURE;
    } else {
      audmain_parse_ctl.status_flag = 0;
      audmain_parse_ctl.handle = handle; 

      if(cmd_buf != NULL) {
        *(audmain_cmd_buf_type**)cmd_buf = (audmain_cmd_buf_type*)&(audmain_parse_ctl.current_buf);
      }

      if(state_ptr_ptr != NULL) {
        *(void **) state_ptr_ptr = (void *) &(audmain_parse_ctl);
      }

      ret_val = AUDMAIN_STATUS_SUCCESS;
    }

  } else {
    audmain_do_play_cb(handle, SND_CMX_AF_FAILURE, 0, NULL);
    ret_val = AUDMAIN_STATUS_FAILURE;
  }

  audmain_read_sync = FALSE;

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_age

DESCRIPTION
  This function ages the file by the indicated number of milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_age (
  void          *state_type,
  uint32        *elapsed_time,
  uint32        *adjusted_time
) {
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        audfmt_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        audqcp_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        audmp3_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        audaac_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif

#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
        audimy_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
      case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        audadpcm_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
        audra_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
        audwma_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        audamrwb_age(parse_ptr, elapsed_time, adjusted_time);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_parse

DESCRIPTION
  This function parses one time event of the specified parse.

DEPENDENCIES
  None

RETURN VALUE
  audmain_status_type
  time_remain - time, in ms, until next event.

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_parse (
  void                     *state_type,
  audmain_cmd_buf_type     *cmd_buf,
  uint32                   *time_remain
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        ret_val = audqcp_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif

#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
        ret_val = audimy_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
      case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        ret_val = audadpcm_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
        ret_val = audra_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
        ret_val = audwma_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        ret_val = audamrwb_parse(parse_ptr, cmd_buf, time_remain);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_preparse

DESCRIPTION
  This function preparses one time event of the specified parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_preparse (
  void *state_type
) {
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        if(audfmt_preparse(parse_ptr)) {
          snd_audfmt_preparse_done(state_type);
        }
        break;
#endif

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
#endif
#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_do_preparse

DESCRIPTION
  This function issues a preparse command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_do_preparse (
  snd_af_access_type    *handle
) {
  snd_af_preparse_type preparse_buf;

  preparse_buf.client_data   = &audmain_parse_ctl;
  preparse_buf.preparse_func = (snd_af_preparse_func_type) audmain_preparse;

  audmain_do_play_cb(handle, SND_CMX_AF_PREPARSE, 0, &preparse_buf);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_flush

DESCRIPTION
  This function flushes the audio buffers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_flush (
  void     *state_type
)
{
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_SUCCESS;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        ret_val = audqcp_flush(parse_ptr);
        break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_flush(parse_ptr);
        break;
#endif /* FEATURE_MP3 */

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_flush(parse_ptr);
        break;
#endif /* FEATURE_AAC */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
        ret_val = audra_flush(parse_ptr);
        break;
#endif /* FEATURE_REAL_AUDIO */

#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
        ret_val = audadpcm_flush(parse_ptr);
        break;
#endif /* FEATURE_PCM */

#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
        ret_val = audwma_flush(parse_ptr);
        break;
#endif /* FEATURE_WMA */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        ret_val = audamrwb_flush(parse_ptr);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif /* FEATURE_IMELODY */
      default:
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_fforward

DESCRIPTION
  This function moves the playback forward ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_fforward (
  void     *state_type,
  uint32   ms_forward,
  uint32   *elapsed_ms,
  uint32   *adjusted_ms
)
{
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_SUCCESS;

  if(state_type != NULL) {
    parse_ptr               = (audmain_parse_ctl_type *) state_type;
    parse_ptr->status_flag &= ~AUDMAIN_MASK_FF_START;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_fforward(parse_ptr, ms_forward, 
                                  elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_MIDI */

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_fforward(parse_ptr, ms_forward,
                                  elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_MP3 */

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_fforward(parse_ptr, ms_forward,
                                  elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_AAC */

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        ret_val = audqcp_fforward(parse_ptr, ms_forward,
                                  elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
        ret_val = audadpcm_fforward(parse_ptr, ms_forward,
                                    elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_PCM */

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_VOC_G711
    case AUDMAIN_WAV_ALAW_TYPE:
    case AUDMAIN_WAV_MULAW_TYPE:
        ret_val = audadpcm_fforward(parse_ptr, ms_forward,
                                    elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_VOC_G711 */
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
        ret_val = audamrwb_fforward(parse_ptr, ms_forward,
                                    elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif /* FEATURE_IMELODY */
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
      case AUDMAIN_INVALID_TYPE:
      default:
        ret_val = AUDMAIN_STATUS_FAILURE;
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_rewind

DESCRIPTION
  This function moves the playback back ms milliseconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_rewind (
  void     *state_type,
  uint32   ms_rewind,
  uint32   *elapsed_ms,
  uint32   *adjusted_ms
)
{
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_SUCCESS;

  if(state_type != NULL) {
    parse_ptr               = (audmain_parse_ctl_type *) state_type;
    parse_ptr->status_flag &= ~AUDMAIN_MASK_FF_START;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = audfmt_rewind(parse_ptr, ms_rewind, elapsed_ms, adjusted_ms);
        break;
#endif

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_rewind(parse_ptr, ms_rewind, elapsed_ms, adjusted_ms);
        break;
#endif

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_rewind(parse_ptr, ms_rewind, elapsed_ms, adjusted_ms);
        break;
#endif

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        ret_val = audqcp_rewind(parse_ptr, ms_rewind, elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
        ret_val = audadpcm_rewind(parse_ptr, ms_rewind, elapsed_ms,
                                                               adjusted_ms);
        break;
#endif

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_VOC_G711
    case AUDMAIN_WAV_ALAW_TYPE:
    case AUDMAIN_WAV_MULAW_TYPE:
        ret_val = audadpcm_rewind(parse_ptr, ms_rewind, elapsed_ms,
                                                               adjusted_ms);
        break;
#endif /* FEATURE_VOC_G711 */
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
        ret_val = audamrwb_rewind(parse_ptr, ms_rewind, elapsed_ms,
                                  adjusted_ms);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif /* FEATURE_REAL_AUDIO */
#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif /* FEATURE_IMELODY */
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
      case AUDMAIN_INVALID_TYPE:
      default:
        ret_val = AUDMAIN_STATUS_FAILURE;
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_do_seek

DESCRIPTION
  This function moves the playback to the proper position.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_do_seek (
  void     *state_type,
  uint32   *elapsed_ms,
  uint32   *adjusted_ms
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_DONE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = AUDMAIN_STATUS_PENDING;
        break;
#endif

#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        ret_val = audqcp_do_seek(parse_ptr, &(parse_ptr->current_buf),
                                 elapsed_ms, adjusted_ms);
        break;
#endif /* FEATURE_QCP */

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        if (parse_ptr->status_flag & AUDMAIN_MASK_FF_PAUSE) { 

          ret_val = AUDMAIN_STATUS_PENDING; 
        }
        break;
#endif
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
#endif /* FEATURE_MP3 */
#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_VOC_G711
    case AUDMAIN_WAV_ALAW_TYPE:
    case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
#endif /* FEATURE_WAVE_PB */
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_time

DESCRIPTION
  This function calculates the playback time of the specified file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_time (
  snd_af_access_type  *handle,
  uint32              byte_offset,
  void                *state_ptr_ptr
) {
  audmain_play_param_type param;
  audmain_status_type     ret_val = AUDMAIN_STATUS_SUCCESS;

  MSG_LOW("audmain_get_time",0,0,0);
  if(handle != NULL) {
    param.calc_time      = TRUE;
    param.get_specs      = FALSE;
    param.start_position = SND_AUDFMT_START_NORMAL;
    param.multiseq_flag  = FALSE;
    param.repeat_enable  = FALSE;
    param.cuep_enable    = FALSE;
    param.repeat_cnt     = 0;
    param.repeat_ms      = 0;
    param.byte_offset    = byte_offset;

    ret_val = audmain_parse_head (handle, state_ptr_ptr, NULL, &param);
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_do_time

DESCRIPTION
  This function calculates the time of the specified file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_do_time (
  void     *state_type,
  uint32   *elapsed_ms
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_DONE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
        ret_val = AUDMAIN_STATUS_PENDING;
        break;
#endif

#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_get_length(parse_ptr, elapsed_ms);
        break;
#endif

#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_get_length(parse_ptr, elapsed_ms);
        break;
#endif

#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
        ret_val = audimy_do_time( parse_ptr, elapsed_ms); 
        break;
#endif
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
        ret_val = audqcp_do_time( parse_ptr, elapsed_ms);
        break;
#endif

#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
      case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        ret_val = audadpcm_get_length( parse_ptr, elapsed_ms);
        break;
#endif /* FEATURE_WAVE_PB */

#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
        ret_val = audamrwb_get_length(parse_ptr, elapsed_ms);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC */

#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_specs

DESCRIPTION
  This function get the audio format type of the specified file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmain_get_specs (
  snd_af_access_type   *handle,
  uint32               byte_offset
) {
  audmain_play_param_type param;
  audmain_status_type     ret_val = AUDMAIN_STATUS_ERROR;

  if(handle != NULL) {
    param.calc_time      = FALSE;
    param.get_specs      = TRUE;
    param.start_position = SND_AUDFMT_START_NORMAL;
    param.multiseq_flag  = FALSE;
    param.repeat_enable  = FALSE;
    param.cuep_enable    = FALSE;
    param.repeat_cnt     = 0;
    param.repeat_ms      = 0;
    param.byte_offset    = byte_offset;
 
    ret_val = audmain_parse_head (handle, NULL, NULL, &param);

    if (ret_val == AUDMAIN_STATUS_SUCCESS) {
      MSG_LOW("audmain_get_specs type=%d", audmain_parse_ctl.type,0,0);
      switch(audmain_parse_ctl.type) {
#ifdef FEATURE_MIDI
        case AUDMAIN_MIDI_TYPE: 
          break;
#endif
#ifdef FEATURE_REAL_AUDIO
        case AUDMAIN_RA_TYPE:
          ret_val = AUDMAIN_STATUS_ERROR;
          audmain_reset();
          break;
#endif

#ifdef FEATURE_WMA
        case AUDMAIN_WMA_TYPE:
        case AUDMAIN_WMA_PRO_TYPE:
          ret_val = AUDMAIN_STATUS_ERROR;
          audmain_reset();
          break;
#endif /* FEATURE_WMA */

/* in case of ID3V2 tag is big, we need to switch async way to parse it.
Fix for no response after renaming mp3(with apic frame) to mp4
*/
#ifdef FEATURE_MP3
        case AUDMAIN_MP3_TYPE:
#endif /* FEATURE_MP3 */
#ifdef FEATURE_AAC
        case AUDMAIN_AAC_TYPE:
#endif /* FEATURE_MP3 */
          break;
        case AUDMAIN_INVALID_TYPE:
        default:
          audmain_reset();
          break;
      }
    }
  }
  return (ret_val);
}


/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_song_config

DESCRIPTION
  This function retrieves decoder-specific song information used to optimize
  playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_song_config (
  void                     *state_type,
  audmain_song_config_type **config
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_DONE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

    switch(parse_ptr->type) {
#ifdef FEATURE_MP3
      case AUDMAIN_MP3_TYPE:
        ret_val = audmp3_get_song_config(parse_ptr,
                                         &parse_ptr->song_cfg.header_config);
        break;
#endif
#ifdef FEATURE_AAC
      case AUDMAIN_AAC_TYPE:
        ret_val = audaac_get_song_config(parse_ptr,
                                         &parse_ptr->song_cfg.header_config);
        break;
#endif
#ifdef FEATURE_MIDI
      case AUDMAIN_MIDI_TYPE:
#ifdef FEATURE_ANIMATED_AAC
        if( parse_ptr->audfadec_mode == AUDMAIN_ADEC_MODE_AAC){
          ret_val = audfadec_get_song_config( parse_ptr,  
                                              &parse_ptr->song_cfg.header_config);
        }
#endif /* FEATURE_ANIMATED_AAC */
        break;
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
#ifdef FEATURE_AMR_WB_AUDIO_DEC
      case AUDMAIN_AMR_WB_TYPE:
      case AUDMAIN_AMR_WB_AUDIO_TYPE:
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
      case AUDMAIN_AMR_WBPLUS_TYPE:
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
        ret_val = audamrwb_get_song_config(parse_ptr,
                                           &parse_ptr->song_cfg.amr_wb_config);
        break;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */
#ifdef FEATURE_REAL_AUDIO
      case AUDMAIN_RA_TYPE:
#endif
#ifdef FEATURE_IMELODY
      case AUDMAIN_IMY_TYPE:
#endif
#ifdef FEATURE_QCP
      case AUDMAIN_QCP_TYPE:
#endif
#ifdef FEATURE_WMA
      case AUDMAIN_WMA_TYPE:
      case AUDMAIN_WMA_PRO_TYPE:
#endif
      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }

    /* Return the song configuration */
    if(config != NULL) {
      *config = &parse_ptr->song_cfg;
    }

  }

  return(ret_val);
}

/* <EJECT> */
#ifdef FEATURE_ANIMATED_AAC
/*===========================================================================

FUNCTION audmain_set_adec_song_config

DESCRIPTION
   This function sets the AAC config to DSP for Animated-AAC and the Codec clks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_adec_song_config (
  void                     *state_type,
  audmain_song_config_type *config,
  snd_sample_rate_type     sample_rate
) {
    if( snd_set_adec_song_config( config, sample_rate) == TRUE){
      return( AUDMAIN_STATUS_SUCCESS);
    }

    return(AUDMAIN_STATUS_ERROR);
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_reset_sequence

DESCRIPTION
  This function resets the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_reset_sequence (
  void    *sequence
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_reset_sequence(sequence);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_pause_sequence

DESCRIPTION
  This function pauses the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_pause_sequence (
  void    *sequence,
  boolean pause_flag
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_pause_sequence(sequence, pause_flag);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_sequence_num

DESCRIPTION
  This function gets the number of the indicated sequence.

DEPENDENCIES
  Sequence must be started

RETURN VALUE
  Sequence Number

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_sequence_num (
  void    *sequence,
  uint16  *seq_num
) {

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    if( audfmt_get_sequence_num(sequence, seq_num) != AUDMAIN_STATUS_SUCCESS){
    return AUDMAIN_STATUS_FAILURE;
  }
  }
#endif /* FEATURE_MIDI */

  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_fforward_sequence

DESCRIPTION
  This function fast-forwards the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_fforward_sequence (
  void    *sequence,
  uint32  ms_forward
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_fforward_sequence(sequence, ms_forward);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_rewind_sequence

DESCRIPTION
  This function rewinds the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_rewind_sequence (
  void    *sequence,
  uint32  ms_rewind
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_rewind_sequence(sequence, ms_rewind);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_tempo_sequence

DESCRIPTION
  This function sets the playback tempo factor for the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_tempo_sequence (
  void    *sequence,
  uint32  tempo_factor
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_set_tempo_sequence(sequence, tempo_factor);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_tune_sequence

DESCRIPTION
  This function sets the playback tune factor for the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_tune_sequence (
  void    *sequence,
  uint32  tune_factor
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_set_tune_sequence(sequence, tune_factor);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_pan_sequence

DESCRIPTION
  This function sets the playback pan factor for the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_pan_sequence (
  void    *sequence,
  uint16  pan_factor
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_set_pan_sequence(sequence, pan_factor);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_vol_sequence

DESCRIPTION
  This function sets the playback vol factor for the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_set_vol_sequence (
  void    *sequence,
  uint16  vol_factor
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#ifdef FEATURE_MIDI
  if(audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) {
    ret_val = audfmt_set_vol_sequence(sequence, vol_factor);
  }
#endif /* FEATURE_MIDI */

  return(ret_val);
}

/* <EJECT> */
#ifdef FEATURE_QCP
/*===========================================================================

FUNCTION audmain_reset_qcp_func

DESCRIPTION
  This function resets the QCP function flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_reset_qcp_func ( void )
{
  audmain_parse_ctl.qcp_func_enabled = FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_get_data_size

DESCRIPTION
  This function finds the size of the data chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_get_data_size (
  snd_af_access_type *handle
) {
  audqcp_get_func(AUDQCP_FUNC_GET_DATA, handle, &audmain_parse_ctl);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_get_frames

DESCRIPTION
  This function finds the number of frames in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_get_frames (
  snd_af_access_type *handle
) {
  audqcp_get_func(AUDQCP_FUNC_GET_VRAT, handle, &audmain_parse_ctl);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_get_cnfg

DESCRIPTION
  This function finds the cnfg chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_get_cnfg (
  snd_af_access_type *handle
) {
  audqcp_get_func(AUDQCP_FUNC_CNFG, handle, &audmain_parse_ctl);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_set_cnfg

DESCRIPTION
  This function sets the cnfg chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_set_cnfg (
  uint16                         cnfg,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
) {
  static uint16 cnfg_buffer;

  cnfg_buffer = cnfg;
  audqcp_set_chunk_data( AUDQCP_FUNC_CNFG, &audmain_parse_ctl, 
                         (uint8 *) &cnfg_buffer,
                         2, handle, cb_func, client_data );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_get_labl

DESCRIPTION
  This function finds the labl chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_get_labl (
  snd_af_access_type *handle
) {
  audqcp_get_chunk_data(AUDQCP_FUNC_LABL, handle, &audmain_parse_ctl);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_set_labl

DESCRIPTION
  This function sets the labl chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_set_labl (
  uint8                          *buffer,
  uint32                         num_bytes,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
) {
  audqcp_set_chunk_data( AUDQCP_FUNC_LABL, &audmain_parse_ctl, buffer,
                         num_bytes, handle, cb_func, client_data );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_get_text

DESCRIPTION
  This function finds the text chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_get_text (
  snd_af_access_type *handle
) {
  audqcp_get_chunk_data(AUDQCP_FUNC_TEXT, handle, &audmain_parse_ctl);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_qcp_set_text

DESCRIPTION
  This function sets the text chunk in a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_qcp_set_text (
  uint8                          *buffer,
  uint32                         num_bytes,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
) {
  audqcp_set_chunk_data( AUDQCP_FUNC_TEXT, &audmain_parse_ctl, buffer,
                         num_bytes, handle, cb_func, client_data );
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_record

DESCRIPTION
  This function starts a QCP recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_record (
  snd_qcp_format_type          format,
  snd_qcp_dir_type             link,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  audmain_status_type status = AUDMAIN_STATUS_FAILURE;

  if(audmain_parse_ctl.type == AUDMAIN_INVALID_TYPE) {
    audmain_read_sync = TRUE;
    status = audqcp_record(format, link, cb_func, client_data);

    if (status != AUDMAIN_STATUS_SUCCESS) {
      audmain_read_sync = FALSE;
    }
  }

  return(status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_record_stop

DESCRIPTION
  This function stops a QCP recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_record_stop (
  audmain_rec_status_type status
) {
  return(audqcp_record_stop(status));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_rec_voc_cb

DESCRIPTION
  This function handles recording callbacks from the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_rec_voc_cb (
  audmain_rec_status_type status,
  uint8                   *buffer,
  uint32                  num_bytes,
  uint32                  num_frames
) {
  audqcp_rec_voc_cb(status, buffer, num_bytes, num_frames);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_buffer_cb

DESCRIPTION
  This function is used as the callback to a get recording buffer request.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_get_buffer_cb (
  uint8   *buf,
  uint32  num_bytes
) {
  if(audmain_read_sync) {
    snd_qcp_rec_new_buffer_req(buf, num_bytes);
    snd_audfmt_continue();
  } else {
    snd_qcp_rec_new_buffer(buf, num_bytes);
  }
} /* end audmain_get_buffer_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_buffer

DESCRIPTION
  This function sends a command to the client to get a recording buffer.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_get_buffer (
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if(cb_func != NULL) {
    cb_func(SND_CMX_QCP_REC_GET_BUF, (void *) audmain_get_buffer_cb,
            client_data);

    if(audmain_read_sync) {
      /* If this is a synchronous call, wait for a callback before continuing */
      if(!snd_audfmt_wait()) {
        ret_val = AUDMAIN_STATUS_ERROR;
      }
    }
  } else {
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return(ret_val);
} /* end audmain_get_buffer */
#endif /* FEATURE_QCP */

/* <EJECT> */
#ifdef FEATURE_PCM_REC
/*===========================================================================

FUNCTION audmain_pcm_record

DESCRIPTION
  This function starts a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
voc_pcm_client_output_fn_type audmain_pcm_record (
  snd_pcm_rec_dir_type         link,
  uint8                        *buf1,
  uint32                       num_buf1,
  uint8                        *buf2,
  uint32                       num_buf2,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  return(audpcm_record(link, buf1, num_buf1, buf2, num_buf2,
                       cb_func, client_data));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_pcm_record_stop

DESCRIPTION
  This function stops a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_pcm_record_stop (
  snd_pcm_rec_dir_type  link
) {
  return(audpcm_record_stop(link));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_pcm_record_pause

DESCRIPTION
  This function pauses/resumes a PCM recording session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_pcm_record_pause (
  boolean               pause,
  snd_pcm_rec_dir_type  link
) {
  return(audpcm_record_pause(pause, link));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_pcm_rec_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_pcm_rec_status (
  snd_af_data_cmd_enum         status,
  snd_pcm_rec_data_struct_type *data,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
) {
  snd_pcm_rec_status(status, data, cb_func, client_data);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_pcm_rec_send_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_pcm_rec_send_status (
  snd_af_data_cmd_enum status
) {
  audpcm_rec_send_status(status);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_pcm_record_buf

DESCRIPTION
  This function accepts a buffer for PCM recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_pcm_record_buf (
  uint8       *buffer,
  uint32      num_bytes,
  const void  *client_data
) {
  audpcm_record_buf(buffer, num_bytes, client_data);
}
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================

FUNCTION audmain_mm_record_start

DESCRIPTION
  This function starts a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_mm_record_start (
  snd_mm_rec_aud_type         *rec_param,   /* Record parameters              */
  snd_mm_rec_cmd_struct_type  *buf,         /* MM record buffer info          */
  snd_mm_rec_cb_func_ptr_type cb_func,      /* Callback function to CMX       */
  const void                  *client_data  /* Client data pointer            */
)
{

  switch (rec_param->format) {
#ifdef FEATURE_AAC_REC
  case SND_MM_FORMAT_AAC:

      return (audaac_rec_start((snd_mm_rec_aac_para_type *)rec_param, 
                               buf,
                               cb_func, 
                               client_data));
#endif /* FEATURE_AAC_REC */

  default:
      return (FALSE);

  } /* switch (format) */
} /* audmain_mm_record_start */

/*===========================================================================

FUNCTION audmain_mm_record_stop

DESCRIPTION
  This function stops a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_mm_record_stop 
(
  snd_mm_rec_format_type format
) 
{
#ifdef FEATURE_AAC_REC
  if (format == SND_MM_FORMAT_AAC) {

    return (audaac_rec_stop());

  } else 
#endif /* FEATURE_AAC_REC */
  {
    return AUDMAIN_STATUS_ERROR;
  }
}

/*===========================================================================

FUNCTION audmain_mm_record_pause

DESCRIPTION
  This function pauses/resumes a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_mm_record_pause (
  boolean                pause,
  snd_mm_rec_format_type format
) 
{
#ifdef FEATURE_AAC_REC
  if (format == SND_MM_FORMAT_AAC) {

    return (audaac_rec_pause(pause));

  } else 
#endif /* FEATURE_AAC_REC */
  {
    return AUDMAIN_STATUS_ERROR;
  }
}

/*===========================================================================

FUNCTION audmain_mm_rec_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_mm_rec_status (
  snd_af_data_cmd_enum        status,
  snd_mm_rec_data_struct_type *data,
  snd_mm_rec_cb_func_ptr_type cb_func,
  const void                  *client_data
) 
{
  snd_mm_rec_status(status, data, cb_func, client_data);
}

/*===========================================================================

FUNCTION audmain_mm_rec_send_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_mm_rec_send_status (
  snd_mm_rec_format_type format,
  snd_af_data_cmd_enum   status
) 
{
#ifdef FEATURE_AAC_REC
  if (format == SND_MM_FORMAT_AAC) {
    audaac_rec_send_status(status);
  }
#endif /* FEATURE_AAC_REC */
}

/*===========================================================================

FUNCTION audmain_mm_record_buf

DESCRIPTION
  This function accepts a buffer for MultiMedia recording.

DEPENDENCIES
  FEATURE_MM_REC

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_mm_record_buf (
  uint8                  *buffer,
  uint32                 num_bytes,
  const void             *client_data,
  snd_mm_rec_format_type format
)
{
  switch (format) {

#ifdef FEATURE_AAC_REC
  case SND_MM_FORMAT_AAC:
      audaac_rec_buf(buffer, num_bytes, client_data);
      break;
#endif /* FEATURE_AAC_REC */

  default:
      return;
  } /* switch (format) */
}

/*===========================================================================

FUNCTION audmain_mm_record_get_buf

DESCRIPTION
  This function get a buffer information for recording Multimedia data.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_mm_record_get_buf (
  snd_mm_rec_format_type     mm_type,
  audmain_mm_record_buf_type *buf_type
)
{
  boolean ret_val;

  ret_val = FALSE;

  switch (mm_type) {
#ifdef FEATURE_AAC_REC
    case SND_MM_FORMAT_AAC:
      ret_val = audaac_rec_get_buf(
                          (voc_aac_record_buf_type *)&(buf_type->aac_buf));
      break;
#endif /* FEATURE_AAC_REC */

    default:
      break;
  }

  return (ret_val);
}

/*===========================================================================

FUNCTION audmain_mm_record_frame

DESCRIPTION
  This function records the multimedia encoded frame(s) into buffers.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_mm_record_frame (
  snd_mm_rec_format_type      mm_type,
  audmain_mm_record_data_type *frame_info
)
{
  switch (mm_type) {
#ifdef FEATURE_AAC_REC
    case SND_MM_FORMAT_AAC:
      audaac_rec_frame((voc_aac_record_data_type *)frame_info);
      break;
#endif /* FEATURE_AAC_REC */

    default:
      break;
  }
}
#endif /* FEATURE_MM_REC */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM
/*===========================================================================

FUNCTION audmain_adpcm_stop

DESCRIPTION
  This function stops ADPCM playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_adpcm_stop (
  uint32 channel
) {
  snd_adec_adpcm_stop(channel);
}
#endif /* FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_GASYNTH_INTERFACE
/*===========================================================================

FUNCTION audmain_voc_adec_start

DESCRIPTION
  This function starts vocoder playback.

DEPENDENCIES
  None

RETURN VALUE
  A value of audmain_status_type to indiciate whether DSP is properly
  configured to accept 13K audio data. 

SIDE EFFECTS
  None
===========================================================================*/
audmain_status_type audmain_voc_adec_start (
  uint32                            channel,
  audmain_voc_adec_enum_type        voc_type,
  audmain_adec_start_mode_enum_type start_mode
) {
  snd_status_type               adec_result;
  audmain_status_type           retval   = AUDMAIN_STATUS_FAILURE;
  snd_adec_start_mode_enum_type snd_mode = SND_ADEC_START_MODE_NORMAL;
  snd_voc_adec_enum_type        snd_voc_type = SND_VOC_ADEC_QCP;

  if(start_mode == AUDMAIN_ADEC_START_MODE_DELAYED) {
    snd_mode = SND_ADEC_START_MODE_DELAYED;
  }

  if (voc_type == AUDMAIN_VOC_ADEC_AMR) {
    snd_voc_type = SND_VOC_ADEC_AMR;
  }

  adec_result = snd_voc_adec_start(channel, snd_voc_type, snd_mode);
  if (adec_result == SND_SUCCESS) {
    retval = AUDMAIN_STATUS_SUCCESS;
  } else if (adec_result == SND_PROMPT_BUSY) {
    retval = AUDMAIN_STATUS_PENDING;
  }

  return (retval);
}

/*===========================================================================

FUNCTION audmain_adec_ppbuf_adjust

DESCRIPTION
  This function adjusts the adec buffer for the ping pong buffer 
  architecture.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_adec_ppbuf_adjust (
  void   *state_type, 
  uint32 *buf1_used,
  uint32 *buf2_used,
  uint8  *buf_usage_mask
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if (state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
    if(parse_ptr->audfadec_enabled) {
      ret_val = audfadec_adjust(parse_ptr, buf1_used, 
                                buf2_used, buf_usage_mask);
    }
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */  

    switch(parse_ptr->type) {
#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif    
#ifdef FEATURE_VOC_G711
      case AUDMAIN_WAV_ALAW_TYPE:
      case AUDMAIN_WAV_MULAW_TYPE:
#endif /* FEATURE_VOC_G711 */
        ret_val = audadpcm_adec_adjust(parse_ptr, buf1_used, buf2_used, 
                                       buf_usage_mask);
        break;
#endif /* FEATURE_WAVE_PB */
      case AUDMAIN_INVALID_TYPE:
      default:
        break;
    }
  }
  return(ret_val);
}

/*===========================================================================

FUNCTION audmain_adec_data_consumed

DESCRIPTION
  This function checks if DSP has done consuming data except partial frame
  data

DEPENDENCIES
  None

RETURN VALUE
  True if DSP is done consuming data
  False if DSP has not done consuming data

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmain_adec_data_consumed(
  uint8 channel
) {
  return(snd_adec_data_consumed(channel));
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION audmain_adpcm_start

DESCRIPTION
  This function starts ADPCM playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_adpcm_start (
  uint32                            channel,
  audmain_adpcm_play_enum_type      play_type,
  audmain_adpcm_format_type         *format,
  audmain_adpcm_mode_enum_type      mode,
  audmain_adec_start_mode_enum_type start_mode
) {
  audmain_status_type           ret_val = AUDMAIN_STATUS_FAILURE;
  snd_status_type               snd_status;
  snd_adec_adpcm_format_type    snd_format;
  snd_adec_start_mode_enum_type snd_mode = SND_ADEC_START_MODE_NORMAL;

  if(start_mode == AUDMAIN_ADEC_START_MODE_DELAYED) {
    snd_mode = SND_ADEC_START_MODE_DELAYED;
  }

  snd_format.channels        = format->channels;
  snd_format.sample_rate     = format->sample_rate;
  snd_format.bytes_rate      = format->bytes_rate;
  snd_format.frame_size      = format->frame_size;
  snd_format.bits_per_sample = format->bits_per_sample;
  snd_format.mode            = (snd_adpcm_mode_enum_type) mode;

  snd_status = snd_adec_adpcm_start(channel,
                                    (snd_adec_adpcm_play_enum_type) play_type,
                                    &snd_format, snd_mode);

  if(snd_status == SND_SUCCESS) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  } else if(snd_status == SND_PROMPT_BUSY) {
    ret_val = AUDMAIN_STATUS_PENDING;
  }
  return(ret_val);
}
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION audmain_set_adec_mode

DESCRIPTION
  This function sets the indicated audio decoder type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GASYNTH_INTERFACE
audmain_status_type audmain_set_adec_mode (
  audmain_adec_mode_enum_type mode
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
  switch(mode) {
    case AUDMAIN_ADEC_MODE_NONE:
      /* Release the adec mode, check any client using it
      ** 1, playing a MIDI file
      ** 2, midi out is still enabled
      */
      if (((audmain_parse_ctl.type == AUDMAIN_INVALID_TYPE)
#ifdef FEATURE_VOC_G711
        || (audmain_parse_ctl.type == AUDMAIN_WAV_MULAW_TYPE)
        || (audmain_parse_ctl.type == AUDMAIN_WAV_ALAW_TYPE)
#endif /* FEATURE_VOC_G711 */
          )
#ifdef FEATURE_MIDI_OUT_QCP
         && (!snd_get_midi_out_enabled_flag())
#endif /* FEATURE_MIDI_OUT_QCP */
         ) {
        ret_val = AUDMAIN_STATUS_SUCCESS;
        audmain_parse_ctl.audfadec_mode = AUDMAIN_ADEC_MODE_NONE;
      }
      break;

    case AUDMAIN_ADEC_MODE_DEFAULT:
      mode = AUDMAIN_ADEC_MODE_QCP;
      /* Fall thru */

    case AUDMAIN_ADEC_MODE_YADPCM:
    case AUDMAIN_ADEC_MODE_QCP:
    case AUDMAIN_ADEC_MODE_ADPCM:
    case AUDMAIN_ADEC_MODE_PCM:
      if(audmain_parse_ctl.audfadec_mode != AUDMAIN_ADEC_MODE_AAC) {
        audmain_parse_ctl.audfadec_mode = mode;
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
      break;

#ifdef FEATURE_ANIMATED_AAC
    case AUDMAIN_ADEC_MODE_AAC:
      if((audmain_parse_ctl.audfadec_mode == AUDMAIN_ADEC_MODE_NONE) ||
         (audmain_parse_ctl.audfadec_mode == AUDMAIN_ADEC_MODE_AAC)) {
        audmain_parse_ctl.audfadec_mode = mode;
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
      break;
#endif
  }
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */
  return (ret_val);
}
#else
audmain_status_type audmain_set_adec_mode (
  audmain_adec_mode_enum_type mode  
) {
  audmain_status_type         ret_val = AUDMAIN_STATUS_FAILURE;

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
  if (mode == AUDMAIN_ADEC_MODE_NONE) { 
    /* Release the adec mode, check any client using it
    ** 1, playing a MIDI file
    ** 2, midi out is still enabled
    */
    if (((audmain_parse_ctl.type == AUDMAIN_INVALID_TYPE)
#ifdef FEATURE_VOC_G711
      || (audmain_parse_ctl.type == AUDMAIN_WAV_MULAW_TYPE)
      || (audmain_parse_ctl.type == AUDMAIN_WAV_ALAW_TYPE)
#endif /* FEATURE_VOC_G711 */
#ifdef FEATURE_MIDI_OUT_QCP
       ) && (!snd_get_midi_out_enabled_flag()
#endif /* FEATURE_MIDI_OUT_QCP */
       )) {
      ret_val = AUDMAIN_STATUS_SUCCESS;
      audmain_parse_ctl.audfadec_mode = AUDMAIN_ADEC_MODE_NONE;
    }
  } else if (mode == AUDMAIN_ADEC_MODE_DEFAULT) {
    /* If requesting default mode set to QCP if mode is not already
    ** configured.
    */
    if (audmain_parse_ctl.audfadec_mode == AUDMAIN_ADEC_MODE_NONE) {
      audmain_parse_ctl.audfadec_mode = AUDMAIN_ADEC_MODE_QCP;
      snd_set_adec_mode(SND_SET_ADEC_MODE_QCP);
    }
    ret_val = AUDMAIN_STATUS_SUCCESS;
  } else if (audmain_parse_ctl.audfadec_mode == AUDMAIN_ADEC_MODE_NONE) {
    /* If not client is using adec */
#ifdef FEATURE_QSYNTH_ADPCM 
    if (mode == AUDMAIN_ADEC_MODE_ADPCM)
    {
      audmain_parse_ctl.audfadec_mode = mode;
      snd_set_adec_mode(SND_SET_ADEC_MODE_ADPCM);
      ret_val = AUDMAIN_STATUS_SUCCESS;
    } else if (mode == AUDMAIN_ADEC_MODE_YADPCM)
    {
      audmain_parse_ctl.audfadec_mode = mode;
      snd_set_adec_mode(SND_SET_ADEC_MODE_YADPCM);
      ret_val = AUDMAIN_STATUS_SUCCESS;
#ifdef FEATURE_QSYNTH_PCM
    } else if (mode == AUDMAIN_ADEC_MODE_PCM)
    {
      audmain_parse_ctl.audfadec_mode = mode;
      snd_set_adec_mode(SND_SET_ADEC_MODE_PCM);
      ret_val = AUDMAIN_STATUS_SUCCESS;
#endif /* FEATURE_QSYNTH_PCM */
    } else
#endif /* FEATURE_QSYNTH_ADPCM */
    if (mode == AUDMAIN_ADEC_MODE_QCP)
    {
      audmain_parse_ctl.audfadec_mode = mode;
      snd_set_adec_mode(SND_SET_ADEC_MODE_QCP);
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
#ifdef FEATURE_ANIMATED_AAC
    else if( mode == AUDMAIN_ADEC_MODE_AAC){
      audmain_parse_ctl.audfadec_mode = mode;
      ret_val = AUDMAIN_STATUS_SUCCESS;
    }
#endif
  } else if (audmain_parse_ctl.audfadec_mode == mode) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */
  return (ret_val);
}
#endif /* FEATURE_GASYNTH_INTERFACE */
#endif /* FEATURE_MIDI || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_MIDI
/*===========================================================================

FUNCTION audmain_set_dls_config

DESCRIPTION
  This function sets the DLS configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_set_dls_config (
  audmain_dls_config_type *dls_config
) {
  snd_set_dls_config((snd_dls_config_type *) dls_config);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_qsynth_vol_mode

DESCRIPTION
  This function sets the qsynth volume level mapping.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_set_qsynth_vol_mode(
  audmain_qsynth_vol_mode_enum_type mode
) {
  snd_set_qsynth_vol_mode((snd_qsynth_vol_mode_enum_type) mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_hybrid_mode

DESCRIPTION
  This function sets the hybrid mode configuration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_set_hybrid_mode (
  audmain_audfmt_poly_mode_type hybrid_mode
) {
  snd_audfmt_poly_mode_type trans_poly_mode;
  boolean set_poly_mode = TRUE;

  switch(hybrid_mode) {
    case AUDMAIN_AUDFMT_POLY_MODE_0:
        trans_poly_mode = SND_AUDFMT_POLY_MODE_0;
        break;
    case AUDMAIN_AUDFMT_POLY_MODE_1:
        trans_poly_mode = SND_AUDFMT_POLY_MODE_1;
        break;
    case AUDMAIN_AUDFMT_POLY_MODE_2:
        trans_poly_mode = SND_AUDFMT_POLY_MODE_2;
        break;
    case AUDMAIN_AUDFMT_POLY_MODE_3:
        trans_poly_mode = SND_AUDFMT_POLY_MODE_3;
        break;
    case AUDMAIN_AUDFMT_POLY_MODE_4:
        trans_poly_mode = SND_AUDFMT_POLY_MODE_4;
        break;
    case AUDMAIN_AUDFMT_POLY_MODE_5:
        trans_poly_mode = SND_AUDFMT_POLY_MODE_5;
        break;
    case AUDMAIN_AUDFMT_POLY_MODE_6:
        trans_poly_mode = SND_AUDFMT_POLY_MODE_6;
        break;
    case AUDMAIN_AUDFMT_POLY_MODE_7:
      trans_poly_mode = SND_AUDFMT_POLY_MODE_7;
      break;
    default:
      set_poly_mode = FALSE;
      break;
  }

  if (set_poly_mode == TRUE) {
    snd_set_hybrid_mode(hybrid_mode);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_pause_seq

DESCRIPTION
  This function issues a command to pause sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_audfmt_pause_seq (
  uint32  sequence,
  boolean pause_flag
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

  if(snd_audfmt_pause_seq(sequence, pause_flag)) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_set_vol_seq

DESCRIPTION
  This function issues a command to set sequence volume.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_audfmt_set_vol_seq (
  uint32  sequence,
  uint16  vol
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

  if(snd_audfmt_set_vol_seq(sequence, vol)) {
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_audfmt_check_reset

DESCRIPTION
  This function checks if parse needs to be reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_audfmt_check_reset ()
{
  snd_audfmt_check_reset();
}

/* <EJECT> */
#ifdef FEATURE_DLS
/*===========================================================================

FUNCTION audmain_dls_get_data_cb_req

DESCRIPTION
  This function processes the callback of a read/get data request for DLS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_dls_get_data_cb_req (
  void    *data,
  uint8   *buf,
  uint32  length,
  uint32  position
) {
  audfmt_dls_get_data_cb_req(data, buf, length, position);
} /* end audmain_dls_get_data_cb_req */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_dls_get_data_cb

DESCRIPTION
  This function is used as the callback to a read/get data request for
  DLS processing.

DEPENDENCIES
  audmain_read_sync

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_dls_get_data_cb (
  void    *audmain_data,
  uint8   *buf,
  uint32  length,
  uint32  position
) {
  snd_audfmt_new_dls_data(audmain_data, buf, length, position);
} /* end audmain_dls_get_data_cb */

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_dls_get_data

DESCRIPTION
  This function issues a get/read command for DLS data.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_ERROR   - if data get fails
  AUDMAIN_STATUS_SUCCESS - if data get succeeds

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_dls_get_data (
  snd_af_data_cmd_enum command,
  snd_af_access_type   *handle,
  uint32               num,
  snd_af_read_cb_type  *parm
) {
  snd_af_read_cb_type read_cb;
  audmain_status_type ret_val = AUDMAIN_STATUS_SUCCESS;

  if((handle != NULL) && (handle->data_func != NULL)) {
    read_cb         = *parm;
    read_cb.cb_func = (snd_af_read_cb_func_type) audmain_dls_get_data_cb;

    handle->data_func(command, handle->audfmt_ptr, num, &read_cb);
  } else {
    ret_val = AUDMAIN_STATUS_ERROR;
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_dls_load

DESCRIPTION
  This function loads a DLS file to be used by all sequences.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_dls_load (
  snd_af_access_type *handle,
  uint32             byte_offset
) {
  audfmt_load_dls(handle, byte_offset);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_dls_unload

DESCRIPTION
  This function unloads all DLS resources that have been loaded via
  audmain_dls_load().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_dls_unload (
  snd_af_access_type *handle
) {
  audfmt_unload_dls(handle);
}
#endif /* FEATURE_DLS */

#ifdef FEATURE_AUDFMT_HYBRID_MODE
/*==========================================================================
 FUNCTION audmain_multi_set_poly_mode

 DESCRPTION
   This function sets hybrid mode associated with multisequencer

 DEPENDENCIES
   None

 RETURN VALUE
   Status to indicate if the command is carried out correctly

 SIDE EFFECTS
   None
 ==========================================================================*/
audmain_status_type audmain_multi_set_poly_mode (
  audmain_audfmt_poly_mode_type mode
) {
  return audfmt_multi_set_poly_mode(mode);
}
#endif /* FEATURE_AUDFMT_HYBRID_MODE */
#endif /* FEATURE_MIDI */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION audmain_get_adec_mode

DESCRIPTION
  This function gets the currently configured audio decoder type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_adec_mode_enum_type audmain_get_adec_mode (void)
{
  return (audmain_parse_ctl.audfadec_mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_adec_reset_channel

DESCRIPTION
  This function resets the indicated QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_adec_reset_channel (
  void   *state_type,
  uint32 channel
) {
  audmain_status_type    ret_val = AUDMAIN_STATUS_ERROR;
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    if(parse_ptr->audfadec_enabled) {
      if(audfadec_reset_channel(parse_ptr, channel) == AUDMAIN_STATUS_DONE) {
        parse_ptr->audfadec_enabled = FALSE;
        parse_ptr->audfadec_tracks  = NULL;
        ret_val = AUDMAIN_STATUS_DONE;
      } else {
        ret_val = AUDMAIN_STATUS_SUCCESS;
      }
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_adec_parse

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
audmain_status_type audmain_adec_parse (
  void                    *state_type,
  audmain_adec_cmd_type   *cmd_buf
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    if(parse_ptr->audfadec_enabled) {
      ret_val = audfadec_parse(parse_ptr, cmd_buf);
    }

    if(ret_val != AUDMAIN_STATUS_SUCCESS) {
      parse_ptr->audfadec_enabled = FALSE;
      parse_ptr->audfadec_tracks  = NULL;
    }
  }

  return(ret_val);
}
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION audmain_midi_out_qcp_flag

DESCRIPTION
  This function will determine if the indicated adec is initiated from
  MIDI out command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_midi_out_qcp_flag (
  void *state_type
) {
  audmain_parse_ctl_type *parse_ptr;
  boolean                ret_val = FALSE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    if(parse_ptr->audfadec_enabled) {
      ret_val = audfadec_midi_out_qcp_flag(parse_ptr);
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_check_midi_out_qcp

DESCRIPTION
  This function will determine if the indicated adec is initiated from
  MIDI out command and there is currently no ADPCM playback active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_check_midi_out_qcp (
  void *state_type
) {
  boolean ret_val = FALSE;

  if(state_type != NULL) {
    switch(audmain_parse_ctl.type) {
#ifdef FEATURE_WAVE_PB
#ifdef FEATURE_ADPCM
      case AUDMAIN_ADPCM_TYPE:
#endif
#ifdef FEATURE_PCM
      case AUDMAIN_PCM_TYPE:
#endif
        break;
#endif /* FEATURE_WAVE_PB */

      default:
      ret_val = audmain_midi_out_qcp_flag(state_type);
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_check_midi_out_qcp_channel

DESCRIPTION
  This function will determine if the indicated adec channel is initiated from
  MIDI out command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean audmain_check_midi_out_qcp_channel (
  void   *state_type,
  uint32 channel
) {
  audmain_parse_ctl_type *parse_ptr;
  boolean                ret_val = FALSE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    if(parse_ptr->audfadec_enabled) {
      ret_val = audfadec_midi_out_qcp_channel_flag(parse_ptr, channel);
    }
  }

  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_midi_out_qcp

DESCRIPTION
  This function will initiate the playback of a QCP file with the QSynth
  DSP image.

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  audmain_parse_ctl

===========================================================================*/
audmain_status_type audmain_midi_out_qcp (
  void                             *state_ptr_ptr,
  uint32                           channel,
  snd_af_access_type               *handle,
  audmain_midi_out_adec_param_type *param
) {
  audmain_status_type ret_val = AUDMAIN_STATUS_FAILURE;

  if(
#ifdef FEATURE_MIDI
     (audmain_parse_ctl.type == AUDMAIN_MIDI_TYPE) ||
#endif /* FEATURE_MIDI */
#ifdef FEATURE_ADPCM
     (audmain_parse_ctl.type == AUDMAIN_ADPCM_TYPE) ||
#endif /* FEATURE_ADPCM */
#ifdef FEATURE_PCM
     (audmain_parse_ctl.type == AUDMAIN_PCM_TYPE) ||
#endif /* FEATURE_PCM */
     (audmain_parse_ctl.type == AUDMAIN_INVALID_TYPE)) {
    ret_val = audfadec_midi_out_qcp(handle, channel, &audmain_parse_ctl, param);
    if(ret_val == AUDMAIN_STATUS_SUCCESS) {
      *(void **) state_ptr_ptr = (void *) &(audmain_parse_ctl);
    }
  } else {
    audmain_do_play_cb(handle, SND_CMX_MIDI_OUT_QCP_RESET, 0, NULL);
  }

  return (ret_val); 
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_midi_out_adec_start_channel

DESCRIPTION
  This function starts the indicated adec file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_midi_out_adec_start_channel (
  void   *state_type,
  uint32 channel
) {
  audmain_status_type    ret_val = AUDMAIN_STATUS_ERROR;
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    if(parse_ptr->audfadec_enabled) {
      ret_val = audfadec_start_channel(parse_ptr, channel);
    }
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_midi_out_qcp_reset

DESCRIPTION
  This function resets the indicated QCP file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_midi_out_qcp_reset (
  void *state_type
) {
  audmain_parse_ctl_type *parse_ptr;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    if(parse_ptr->audfadec_enabled) {
      audfadec_reset_midi_out_qcp(parse_ptr);
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_midi_out_qcp_pause

DESCRIPTION
  This function pauses the MIDI OUT QCP files.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmain_midi_out_qcp_pause (
  void    *state_type,
  boolean pause_flag
) {
  audmain_parse_ctl_type *parse_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_FAILURE;

  if(state_type != NULL) {
    parse_ptr = (audmain_parse_ctl_type *) state_type;
    if(parse_ptr->audfadec_enabled) {
      ret_val = audfadec_pause_midi_out_qcp(parse_ptr, pause_flag);
    }
  }
  return(ret_val);
}
#endif /* FEATURE_MIDI_OUT_QCP */

/*===========================================================================

FUNCTION audmain_audfmt_do_time

DESCRIPTION
  This function is the intemediator and essentially invoke snd_audfmt_do_time 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_audfmt_do_time (void)
{
  snd_audfmt_do_time(); 
}

/*===========================================================================

FUNCTION audmain_vbr_check_ffwd

DESCRIPTION
  This function re-check the VBR fforward when the target frame is found

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_vbr_check_ffwd (
  void   *handle,
  void   *state_type,
  uint8  *buf,
  uint32 adjusted_ms,
  uint32 elapsed_ms,
  uint32 ms_forward
)
{
  audmain_parse_ctl_type   *parse_ptr;

  if (state_type != NULL) {
    parse_ptr  = (audmain_parse_ctl_type *) state_type;

    if (parse_ptr->status_flag |= AUDMAIN_MASK_FF_PAUSE) { 
      parse_ptr->status_flag &= ~AUDMAIN_MASK_FF_PAUSE;
    }

    snd_vbr_check_ffwd(adjusted_ms, elapsed_ms, ms_forward);
  }
}

/*===========================================================================

FUNCTION audmain_cleanup_vbr

DESCRIPTION
  Check the corresponding format to clean up the vbr parse structure.

DEPENDENCIES
  File type being closed. 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmain_cleanup_vbr (audmain_file_type file_type)
{
  switch (file_type) {
#ifdef FEATURE_AAC
    case AUDMAIN_AAC_TYPE:
      audaac_cleanup_vbr();
      break;
#endif

    default:
      break;
  }
}
#endif /* FEATURE_AUDIO_FORMAT */
