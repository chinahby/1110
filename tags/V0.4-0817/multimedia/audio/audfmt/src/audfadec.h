#ifndef AUDFADEC_H
#define AUDFADEC_H
/*===========================================================================

             A U D I O   F O R M A T s   A U D I O   D E C O D E R
                           H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the audio formats audio decoder module.

Copyright (c) 2001 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audfadec.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/06    pl     Modified definition of audfadec_adjust(). 
04/18/06    st     Updated audfadec_wa_reset() to support a clear data request
                   pending flag, CR90915.
03/13/06    st     Added audfadec_start_channel() prototype; updated
                   audfadec_midi_out_qcp() prototype.
05/08/05    st     Added frame_size parameter to audfadec_wa_info_type.
03/10/05    st     Added mfi_flag and mfi_continue parameters to
                   audfadec_wa_info_type.
04/14/04    lg     Modified code to support Animated AAC Ringers.
03/22/04    st     Added audfadec_midi_out_qcp_channel_flag() and
                   audfadec_pause_midi_out_qcp() function prototypes.
10/16/03  aw/st    Added support for SMAF.
09/26/02  aw/st    Added support to select adec mode for webaudio.
11/10/01    st     Added audfadec_wa_activate() function prototype.
10/01/01    st     Modified audfadec_adjust() to support multiple channels;
                   added audfadec_reset_midi_out_qcp() prototype.
09/03/01    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
#include "snd.h"                /* Sound typedefs and prototypes          */
#include "audmaini.h"           /* Audio format typedefs and prototypes   */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define AUDFADEC_NUM_TRACKS AUDMAIN_ADEC_MAX_CHANNELS

#ifdef FEATURE_WEBAUDIO
/* This structure is used to indicate all the information required to parse
** the audio decoder data embedded winthin a WebAudio file.
*/
typedef struct {
  uint32 block_start;      /* Location of Audio decoder data + header */
  uint32 next_block;       /* Location of the next block of data      */
  uint32 length;           /* Length of current block of audio data   */
  uint32 location;         /* Location of current block of audio data */
  uint8  *buf1;            /* Buffer containing audio decoder data    */
  uint32 length_buf1;      /* Length in bytes of first buffer         */
  uint8  *buf2;            /* Buffer containing audio decoder data    */
  uint32 length_buf2;      /* Length in bytes of second buffer        */

  uint16 sample_rate_hz;   /* Sampling rate                           */
  uint16 bits_per_sample;  /* Bits per sample                         */
  uint16 channels;         /* Number of channels                      */
  uint16 frame_size;       /* Frame size                              */
  audmain_adec_mode_enum_type
         adec_mode;        /* Audio decoder type                      */
  audmain_adpcm_mode_enum_type
         yadpcm_mode;      /* Mode of audio decoder data              */

  boolean mfi_flag;        /* MFi audio decoder flag                  */
  boolean mfi_continue;    /* MFi block continue flag                 */
} audfadec_wa_info_type;
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDFADEC Public Function Prototypes
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION audfadec_get_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming data buffer.

DEPENDENCIES
  audfadec_buf_mgmt

RETURN VALUE
  None

SIDE EFFECTS
  audfadec_buf_mgmt

===========================================================================*/
extern boolean audfadec_get_data_cb_req(
  uint8 **data,
  uint8 *buf,
  uint32 length,
  uint32 pos
  /*lint -esym(715,pos) Tell Lint we know pos parameter is not used */
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_adjust

DESCRIPTION
  This function adjusts the adec buffer by the indicated number of bytes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_GASYNTH_INTERFACE
extern audmain_status_type audfadec_adjust (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   *buf1_len,
  uint32                   *buf2_len,
  uint8                    *buf_usage_mask
);
#else 
extern audmain_status_type audfadec_adjust (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   *num_used
);
#endif /* FEATURE_GASYNTH_INTERFACE */
/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_parse

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
extern audmain_status_type audfadec_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_adec_cmd_type   *cmd_buf
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_reset_channel

DESCRIPTION
  This function resets the indicated QCP file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfadec_reset_channel (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  channel
);

/* <EJECT> */
#ifdef FEATURE_WEBAUDIO
/*===========================================================================

FUNCTION audfadec_wa_reset

DESCRIPTION
  This function resets all WebAudio audio decoders.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfadec_wa_reset (
  uint32  channel,
  boolean clear_data_req
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_wa_activate

DESCRIPTION
  This function will activate any pending audio decoders.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void audfadec_wa_activate ( uint32 channel );

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_wa_out_qcp

DESCRIPTION
  This function will initiate the playback of a QCP file embedded in
  a WebAudio file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audfadec_wa_out_qcp (
  snd_af_access_type          *handle,
  uint32                      channel,
  audfadec_wa_info_type       *info,
  audmain_parse_ctl_type      *parse_ctl,
  boolean                     do_offset
);

/* <EJECT> */
#ifdef FEATURE_ANIMATED_AAC
/*===========================================================================

FUNCTION audfadec_get_song_config

DESCRIPTION
  Gets the song config .

DEPENDENCIES
  audfadec_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfadec_get_song_config(
  audmain_parse_ctl_type   *parse_ctl,
  audmpeg_song_config_type *song_config
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_get_sample_rate

DESCRIPTION
  Gets the sample_rate.

DEPENDENCIES
  audfadec_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audfadec_get_sample_rate(
  audmain_parse_ctl_type   *parse_ctl
);
#endif /* FEATURE_ANIMATED_AAC */
#endif /* FEATURE_WEBAUDIO */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
/*===========================================================================

FUNCTION audfadec_midi_out_qcp_flag

DESCRIPTION
  This function indicates whether the cuurent audio deocder parse
  is from a MIDI out command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfadec_midi_out_qcp_flag (
  audmain_parse_ctl_type      *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_midi_out_qcp_channel_flag

DESCRIPTION
  This function indicates whether the indicated channel is from a MIDI
  out command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audfadec_midi_out_qcp_channel_flag (
  audmain_parse_ctl_type *parse_ctl,
  uint32                 channel
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_reset_midi_out_qcp

DESCRIPTION
  This function resets QCP file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audfadec_reset_midi_out_qcp (
  audmain_parse_ctl_type  *parse_ctl
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_start_channel

DESCRIPTION
  This function starts the indicated adec file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfadec_start_channel (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  channel
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_pause_midi_out_qcp

DESCRIPTION
  This function pauses QCP file playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audfadec_pause_midi_out_qcp (
  audmain_parse_ctl_type  *parse_ctl,
  boolean                 pause_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfadec_midi_out_qcp

DESCRIPTION
  This function will initiate the playback of a QCP file with the QSynth
  DSP image.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern audmain_status_type audfadec_midi_out_qcp (
  snd_af_access_type               *handle,
  uint32                           channel,
  audmain_parse_ctl_type           *parse_ctl,
  audmain_midi_out_adec_param_type *param
);
#endif /* FEATURE_MIDI_OUT_QCP */

#endif /* defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) */
#endif /* AUDFADEC_H */
