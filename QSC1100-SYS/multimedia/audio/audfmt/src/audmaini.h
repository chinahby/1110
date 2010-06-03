#ifndef AUDMAINI_H
#define AUDMAINI_H
/*===========================================================================

              A U D I O   F O R M A T S   M A I N   I N T E R N A L
                          H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions for 
  the audio formats main module.

Copyright (c) 2000-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmaini.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/07    sk     Included the snd_af_access_type in audmain_parse_ctl.
                   CR 120253,124336,124337,120853. 
04/16/04    st     Added defines for default tune and tempo values.
10/16/03  aw/st    Modified code to support SMAF and multi-sequence.
02/19/03  aw/st    Added FEATURE_QSYNTH_ADPCM to audfadec_mode in 
                   audmain_parse_ctl_type structure.
08/10/02  aw/st    Added audfadec_mode to audmain_parse_ctl_type.
02/06/02    st     Added seek_overflow to audmain_parse_ctl_type.
09/03/01    st     Added parameters to audmain_parse_ctl_type in order to
                   keep track of QSynth audio decoder functions and QCP
                   support functions.
02/28/01    st     Added AUDMAIN_MASK_PARSED to audmain_status_mask_type.
12/04/00    sm     audmain_song_config_type added to audmain_parse_ctl.
11/27/00    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions               */
#include "comdef.h"             /* Definitions for byte, word, etc.          */

#ifdef FEATURE_AUDIO_FORMAT
#include "audmain.h"            /* Audio format main typedefs and prototypes */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Default values for tempo and tune commands */
#define AUDMAIN_TUNE_DEFAULT     0x40
#define AUDMAIN_TEMPO_DEFAULT    100

/* Bit values for fast-forward pauses
*/
typedef enum {
  AUDMAIN_MASK_INIT_DONE        = 0x01, /* Initialization done           */
  AUDMAIN_MASK_PAUSE            = 0x02, /* Pause flag                    */
  AUDMAIN_MASK_FF_PAUSE         = 0x04, /* Pause due to fast-forwarding  */
  AUDMAIN_MASK_FF_START         = 0x08  /* Pause in prep for fforwarding */
} audmain_status_mask_type;

/* This structure contains all the information on a midi/WebAudio file
** being parsed.
*/
typedef struct {
  audmain_file_type     type;          /* Type of file being parsed         */
  uint32                status_flag;   /* Flag indicating various statuses  */
  snd_af_access_type    *handle;        /* CMX handle ptr                    */
  uint32                buffer_size;   /* Size of buffers being returned    */
  void                  *tracks;       /* Tracks for this file              */
  audmain_cmd_buf_type  current_buf;   /* Command buffer for this file      */
  audmain_song_config_type song_cfg;   /* Optimal configuration for song    */
#if defined (FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
  boolean               audfadec_enabled; /* Audio decoder enabled flag     */
  void                  *audfadec_tracks; /* Audio decoder information      */
#endif /* defined (FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) */
#if defined (FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined (FEATURE_QSYNTH_ADPCM)
  audmain_adec_mode_enum_type
                        audfadec_mode;    /* Audio decoder mode             */
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */
#ifdef FEATURE_QCP
  boolean               qcp_func_enabled; /* QCP functions enabled          */
#endif /* FEATURE_QCP */
} audmain_parse_ctl_type;

#endif /* FEATURE_AUDIO_FORMAT */

#endif /* AUDMAINI_H */
