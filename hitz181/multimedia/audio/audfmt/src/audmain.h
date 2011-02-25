#ifndef AUDMAIN_H
#define AUDMAIN_H
/*===========================================================================

       A U D I O   F O R M A T S   M A I N   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the audio formats main module.

Copyright (c) 2000 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmain.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/16/09    kd     Added default volume level for SMAF.
10/02/08    vsud   Fixed CR# 157201 for SMAF file play back issues.
08/26/08   anb     Added support for DSP based WMAPRO
03/17/08   anb     Fixed compiler warning wrt audmain_reset_parms()
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
07/10/07    st     Updated audmain_adec_ppbuf_adjust() function prototype.
04/07/07    pl     Changed audmain_qcp_adec_start() to audmain_voc_adec_start()
                   to accommodate new AMR-NB decoder. 
02/07/07    at     Added AUDMAIN_AMR_WB_AUDIO_TYPE audmain_file_type for
                   3GP AWB file support.
09/04/06    ay     Added AMR-WB+ support.
08/15/06    sg     Added sync read API that allows the caller to get tbe
                   length of the data.
06/26/06    pl     Added new function definitions audmain_adec_ppbuf_adjust()
                   and audmain_adec_data_consumed().
06/02/06    st     Modified AUDMAIN_CMD_BUF_MAX_WORDS from 125 to 128.
04/14/06    lg     Added new API for getting the INIT_DONE flag(CR#89914)
03/13/06    st     Added support for delayed start and repeat mode for playback
                   of audio decoder files to MIDI device.
01/30/06    pl     Added support to provide a way to select hybrid mode 
                   for multisequencer mode and MIDI device.
09/18/05   pl/st   Added audmain_get_var_adec_data(), audmain_qcp_adec_start()
                   function prototypes.
08/11/05    hz     Added support for DTX and data requrest time for AMR 
                   recording.
07/25/05    lg     Modify code to do SMAF waveform unpacking in DSP.
07/22/05    sm     Added WAV A/Ulaw support.
07/20/05    sp     Added support for AAC LOAS.
07/19/05    pl     Extended audmain_status_type to include error-specific 
                   status.
05/24/05    hz     Added support for MultiMedia record.
04/28/05   ymc     Added 4GV NB support.
02/14/05    hz     Added AUDMAIN_WMA_TYPE in audmain_file_type for Windows
                   Media Audio decoding.
02/05/05    st     Added byte_offset parameter to audmain_play_param_type and
                   various supported functions; added support for HQ DLS;
                   added support for DLS file processing.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
08/11/04    st     Added audmain_audfmt_check_reset() and
                   audmain_audfmt_check_reset_req() function prototypes;
                   modified audmain_cmd_buf_type to separate single large
                   MIDI command buffer into four separate command buffers.
07/02/04    st     Added start_aging flag to audmain_cmd_buf_type.
04/14/04    lg     Modified code to support Animated AAC Ringers.
03/22/04    st     Added get_specs flag to audmain_play_param_type; added
                   support for PCM playback and recording; updated
                   audmain_audfmt_pause_seq() and audmain_audfmt_set_vol_seq()
                   and audmain_get_specs() function prototypes.
                   Added audmain_audfmt_pause_seq(), audmain_play_cb(),
                   audmain_audfmt_set_vol_seq(), audmain_get_final_cmd(),
                   audmain_midi_out_qcp_pause(),
                   audmain_audfmt_adec_chan_flush(),
                   audmain_check_midi_out_qcp_channel(), audmain_set_pan();
                   updated audmain_audfmt_adec_chan_pause(), audmain_set_tune(),
                   audmain_audfmt_adec_chan_pause(), audmain_set_ff_start(),
                   audmain_set_tempo(), audmain_reset_sequence();
                   removed audmain_get_wa_audfadec_status(),
                   audmain_get_note_off_cmd(), audmain_multi_callback(),
                   audmain_midi_out_qcp_flag().
03/04/04    st     Added prototype for audmain_reset_calc_time.
01/26/04    lg     Modified code to support multi_sequencer. Added new functions
                   for tune, tempo, pan and volume adjustments.
01/26/04    sg     Modified to send success callbacks for formats not supporting
                   tuning and tempo - status will indicate no change in
                   tuning/tempo.
            aw     Get support to get file specs.
01/21/04    aw     Added support for qsynth volume level mapping.
12/08/03    aw     Fixed the feature-ization for AUDMAIN_CMD_BUF_MAX_WORDS.
12/02/03    sg     Added support for linear PCM playback.
11/23/03    sg     Added support for RealAudio codec.
10/16/03  aw/st    Modified code to support SMAF and audfmt multi-sequence.
09/19/03    sg     Modified to support additional AAC parameters required by the
                   DSP. Added support for a 'psuedo-raw' mode.
03/13/03    st     Removed AUDMAIN_NO_AUDIO.
02/19/03   aw/st   Added support for ADPCM format; modified 
                   audmain_adpcm_start() function prototype; added
                   audmain_check_midi_out_qcp() function prototype.
01/25/03    aw     Featurized audmain_get_wa_audfadec_status() properly.
11/19/02    st     Added audmain_audfmt_synth_mvol() and
                   audmain_audfmt_adec_mvol() function prototypes.
11/06/02    sm     Added AMR QCP support.
11/05/02    sg     Added support for iMelody format.
10/28/02    aw     Added audmain_get_wa_audfadec_status function.
08/13/02    yl     Added the AUDMAIN_REC_AUTO_STOP in audmain_rec_status_type.
08/10/02   aw/st   Added support for ADPCM audio decoder.
07/25/02   sg/st   Added support AAC and AUDMAIN_NO_AUDIO.
                   Added audmain_format_play_init() and audmain_flush().
02/06/02    st     Added audmain_get_seek_overflow() prototype.
10/08/01    st     Added audmain_get_pause_flag() prototype.
10/01/01    st     Modified code to support multiple audio decoder channels:
                   audmain_midi_out_qcp_reset(), audmain_midi_out_qcp(), 
                   audmain_adec_adjust(), audmain_audfmt_adec_chan_pan(),
                   audmain_audfmt_adec_chan_vol(), and added
                   audmain_get_adec_data() and audmain_adec_reset_channel().
09/16/01    st     Added audmain_qcp_set_cnfg() function prototype.
09/03/01  st/aw    Modified code to support QCP record, playback and support
                   functions; modified code to use new audfadec module.
07/04/00    st     Added AUDMAIN_STATUS_STORE_ERR.
04/26/00    sm     Removed speed_factor from audmp3_song_config_type.
04/04/01    st     Added audmain_init_done() function prototype.
                   Added AUDMAIN_STATUS_JUMP to describe jump and cue point
                   commands; created audmain_adec_cmd_type; modified
                   functionality of audmain_set_reset_cmd(); added
                   audmain_get_note_off_cmd() prototype; added length parameter
                   to audmain_get_data_cb_req(); added support for playing QCP
                   files to the direct MIDI device output interface.
02/28/01    st     Added AUDMAIN_STATUS_PENDING and AUDMAIN_STATUS_BUFF_ERR_PEND
                   to audmain_status_type; added audmain_set_parse_done() and
                   audmain_do_preparse() function prototypes.
12/06/00    st     Added audmain_get_repeat().
12/04/00  st/sm    Added protos for adec_chan_pan(), adec_chan_vol(),
                   get_song_config().  audxxx_song_config_type added.
11/27/00    st     created file 

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#ifdef FEATURE_AUDIO_FORMAT
#include "snd.h"                /* Sound typedefs and prototypes          */
#include "voc.h"                /* Vocoder definitions and interface      */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* This type is used to return status for various audio format functions. */
typedef enum {
  AUDMAIN_STATUS_SUCCESS,          /* Command was successful        */
  AUDMAIN_STATUS_PENDING,          /* Status is pending             */
  AUDMAIN_STATUS_BUFF_ERR,         /* Data underflow encountered    */
  AUDMAIN_STATUS_DONE,             /* Command is finished           */
  AUDMAIN_STATUS_ERROR,          /* Encountered an error with unknown reason */
  AUDMAIN_STATUS_DATA_ERR,       /* Encountered invalid data error           */
  AUDMAIN_STATUS_DATA_ACCESS_ERR,/* Encountered data access error            */
  AUDMAIN_STATUS_RESOURCE_ERR,   /* Encountered resource error               */
  AUDMAIN_STATUS_FAILURE,        /* Command failed                           */
  AUDMAIN_STATUS_MAX         
} audmain_status_type;

/* Type indicating the format of file that is being parsed. */
typedef enum {
#ifdef FEATURE_MIDI
  AUDMAIN_MIDI_TYPE,      /* Midi type format       */
#endif
#ifdef FEATURE_MP3
  AUDMAIN_MP3_TYPE,       /* MP3 file format        */
#endif
#ifdef FEATURE_AAC
  AUDMAIN_AAC_TYPE,       /* AAC file format        */
#endif
#ifdef FEATURE_QCP
  AUDMAIN_QCP_TYPE,       /* QCP file format        */
#endif
#ifdef FEATURE_IMELODY
  AUDMAIN_IMY_TYPE,       /* iMelody file format    */
#endif
#ifdef FEATURE_ADPCM
  AUDMAIN_ADPCM_TYPE,     /* ADPCM file format      */
#endif
#ifdef FEATURE_PCM
  AUDMAIN_PCM_TYPE,       /* PCM file format        */
#endif
#ifdef FEATURE_REAL_AUDIO
  AUDMAIN_RA_TYPE,        /* RealAudio file format  */
#endif
#ifdef FEATURE_WMA
  AUDMAIN_WMA_TYPE,       /* Window Media Audio file format */
  AUDMAIN_WMA_PRO_TYPE,   /* Window Media Pro Audio file format */
#endif
#ifdef FEATURE_AMR_WB_AUDIO_DEC
  AUDMAIN_AMR_WB_TYPE,    /* AMR-WB file format     */
  AUDMAIN_AMR_WB_AUDIO_TYPE, /* 3GP AMR-WB file format */
#endif /* FEATURE_AMR_WB_AUDIO_DEC */
#ifdef FEATURE_AMR_WBPLUS_AUDIO_DEC
  AUDMAIN_AMR_WBPLUS_TYPE, /* AMR-WB+ file format   */
#endif /* FEATURE_AMR_WBPLUS_AUDIO_DEC */
#ifdef FEATURE_VOC_G711
  AUDMAIN_WAV_MULAW_TYPE, /* Wav file, Mu-Law format*/
  AUDMAIN_WAV_ALAW_TYPE,  /* Wav file, A-Law format */
#endif /* FEATURE_VOC_G711 */
  AUDMAIN_INVALID_TYPE    /* Unknown/Invalid format         */

} audmain_file_type;

/* This structure is used to indicate playback parameters to audio format
** layer.
*/
typedef struct {
  uint32                      byte_offset;    /* Offset to start of file  */
  boolean                     calc_time;      /* Calc time flag           */
  boolean                     get_specs;      /* Get specifications flag  */
  boolean                     multiseq_flag;  /* Mulitsequencer flag      */
  snd_audfmt_start_enum_type  start_position; /* Start mode               */
  boolean                     repeat_enable;  /* Repeat flag              */
  boolean                     cuep_enable;    /* Cue point flag           */
  boolean                     packed_dls_wf_mode; /* Packed DLS waveform  */
  uint8                       repeat_cnt;     /* Repeat count             */
  uint16                      repeat_ms;      /* Repeat silence duration  */
} audmain_play_param_type;

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
#ifdef FEATURE_MULTIPLE_ADEC_CHANNELS
#define AUDMAIN_ADEC_MAX_CHANNELS 4
#else
#define AUDMAIN_ADEC_MAX_CHANNELS 1
#endif /* FEATURE_MULTIPLE_ADEC_CHANNELS */
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
/* This structure is used to indicate Audio Decoder data */
typedef struct {
  uint8   *adec_buf1;       /* First buffer for Audio Decoder  */
  uint32  adec_num_buf1;    /* Size of first buffer            */
  uint8   *adec_buf2;       /* Second buffer for Audio Decoder */
  uint32  adec_num_buf2;    /* Size of second buffer           */
  boolean adec_reset;       /* Reset audio decoder on DSP      */
#ifdef FEATURE_GASYNTH_INTERFACE
  uint8   buf_usage_mask;   /* Serves as mean for the driver to
                               inform the parser that buffers
                               are in used */
#endif /* FEATURE_GASYNTH_INTERFACE */
} audmain_adec_cmd_type;
#endif /* FEATURE_ADEC || FEATURE_WAVE_PB */

#if defined(FEATURE_MIDI) || defined(FEATURE_IMELODY)
#define AUDMAIN_CMD_BUF_MAX_WORDS  128

#ifdef FEATURE_AUDFMT_MULTISEQUENCER
#define AUDMAIN_CMD_BUF_NUM_SEQ    4
#else
#define AUDMAIN_CMD_BUF_NUM_SEQ    1
#endif /* FEATURE_AUDFMT_MULTISEQUENCER */

/* This type is a buffer for commands to give to the synthesizer. */
typedef struct {
  uint16 num_words;          /* Number of total words in command buffer  */
  uint16 num_commands;       /* Number of commands in command buffer     */
  uint16 command_buf[AUDMAIN_CMD_BUF_MAX_WORDS];
                             /* Buffer of commands                       */
} audmain_midi_cmd_buf_type;
#endif /*  FEATURE_MIDI || FEATURE_IMELODY */

/* This type is a buffer for commands to give to the synthesizer. */
typedef struct {
#if defined(FEATURE_MIDI) || defined(FEATURE_IMELODY)
  audmain_midi_cmd_buf_type midi_cmd[AUDMAIN_CMD_BUF_NUM_SEQ];
  boolean start_aging;       /* Flag to indicate commands are processing */
#endif /* FEATURE_MIDI || FEATURE_IMELODY */

#if defined(FEATURE_IMELODY) || defined(FEATURE_WEBAUDIO)
  boolean vib;               /* vibrator control                         */
#endif /* FEATURE_IMELODY || FEATURE_WEBAUDIO */

#ifdef FEATURE_IMELODY
  byte  imy_volume;          /* iMelody volume                           */
#endif

#if defined(FEATURE_ADEC) || defined(FEATURE_WAVE_PB)
  audmain_adec_cmd_type adec_cmd; /* Audio Decoder command               */
#endif

#ifdef FEATURE_QCP
  voc_pb_header_type   *prompt1;  /* Pointers to prompts to be played    */
  voc_pb_header_type   *prompt2;
  voc_pb_header_type   *prompt3;
  voc_pb_header_type   *prompt4;
#endif
} audmain_cmd_buf_type;

#ifdef FEATURE_MPEG_AUDIO

#ifdef FEATURE_AAC

/* AAC Data formats
*/
typedef enum {
  AUDMAIN_AAC_DATA_FORMAT_ADIF,       /* ADIF                   */
  AUDMAIN_AAC_DATA_FORMAT_ADTS,       /* ADTS                   */
  AUDMAIN_AAC_DATA_FORMAT_RAW,        /* Raw (PV)               */
  AUDMAIN_AAC_DATA_FORMAT_PSUEDO_RAW, /* Raw with frame markers */
  AUDMAIN_AAC_DATA_FORMAT_LOAS        /* LOAS                   */
} audmain_aac_data_format_enum_type;

/* Info that will be used to configure the DSP
*/
typedef struct {
  uint16                              buffer_factor;
  audmain_aac_data_format_enum_type   aac_format;

  /* See ISO/IEC 14496-3:2001(E) for details on the parameters below */
  byte                                sample_freq_index;
  byte                                audio_object;
  snd_aac_channel_enum_type           channel_configuration;
  byte                                ep_config;
  byte                                aac_section_data_resilience_flag;
  byte                                aac_scalefactor_data_resilience_flag;
  byte                                aac_spectral_data_resilience_flag;
} audmain_aac_config_type;
#endif /* FEATURE_AAC */

#ifdef FEATURE_MP3
/* Info that will be used to configure the DSP
*/
typedef struct {
  uint32             sample_freq;
} audmain_mp3_config_type;
#endif /* FEATURE_MP3 */

/* The following structure contains decoder specific information about the
** currently loaded MPEG audio song.  It is used to configure the DSP.
*/
typedef struct {
  uint8                   mask[4];    /* Mask for sync word */
  uint8                   header[4];  /* Sync word          */
  uint8                   mask_size;  /* size in bytes      */
#ifdef FEATURE_AAC
  audmain_aac_config_type aac_config; /* DSP config info    */
#endif
#ifdef FEATURE_MP3
  audmain_mp3_config_type mp3_config; /* DSP config info    */
#endif
} audmpeg_song_config_type;
#endif /* FEATURE_MPEG_AUDIO */

#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
/* Info that will be used to configure the DSP
*/
typedef struct {
  uint16 sample_rate;   /* Sampling rate in Hz  */
  uint16 dec_chan_mode; /* Decoder channel mode */
} audmain_amr_wb_config_type;

/* The following structure contains decoder specific information about the
** currently loaded AMR-WB/WB+ audio playback to configure the DSP.
*/
typedef struct {
  audmain_amr_wb_config_type amr_wb_config; /* DSP config info */
} audmain_amr_wb_song_config_type;
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */

/* The following union is used when requesting a decoder configuration based
** on the currently selected song.
*/
typedef union {
#ifdef FEATURE_MPEG_AUDIO
  audmpeg_song_config_type header_config;
#endif /* FEATURE_MPEG_AUDIO */
#ifdef FEATURE_AMR_WB_AUDIO_DEC_COMMON
  audmain_amr_wb_song_config_type amr_wb_config; /* AMR-WB config */
#endif /* FEATURE_AMR_WB_AUDIO_DEC_COMMON */
  uint8 dummy;
} audmain_song_config_type;

#ifdef FEATURE_QCP
/* The following enumerated type is used to indicate the status of 
** recording callbacks from the vocoder driver.
*/
typedef enum {
  AUDMAIN_REC_RX_BUFFER,       /* Rx (forward link) data buffer  */
  AUDMAIN_REC_LAST_RX_BUFFER,  /* Final Rx data buffer           */
  AUDMAIN_REC_TX_BUFFER,       /* Tx (reverse link) data buffer  */
  AUDMAIN_REC_LAST_TX_BUFFER,  /* Final Tx data buffer           */
  AUDMAIN_REC_INT_TIME,        /* Time interval callback         */
  AUDMAIN_REC_INFORM_13K,      /* Service option changed to 13K  */
  AUDMAIN_REC_INFORM_EVRC,     /* Service option changed to EVRC */
  AUDMAIN_REC_INFORM_4GV_NB,   /* Service option changed to 4GV  */
  AUDMAIN_REC_INFORM_AMR,      /* Service option changed to AMR  */
  AUDMAIN_REC_INFORM_EFR,      /* Service option changed to EFR  */
  AUDMAIN_REC_INFORM_FR,       /* Service option changed to FR   */
  AUDMAIN_REC_INFORM_HR,       /* Service option changed to HR   */
  AUDMAIN_REC_CHANGE_CONFIG,   /* Configuration changed          */
  AUDMAIN_REC_PAUSED,          /* Recording paused               */
  AUDMAIN_REC_RESUMED,         /* Recording resumed              */
  AUDMAIN_REC_ERROR,           /* Recording error                */
  AUDMAIN_REC_BUFF_ERROR,      /* Recording buffer error         */
  AUDMAIN_REC_AUTO_STOP,       /* Recording stopped by silence   */
  AUDMAIN_REC_DONE,            /* Recording done                 */
  AUDMAIN_REC_INVALID
} audmain_rec_status_type;
#endif /* FEATURE_QCP */

#if defined(FEATURE_MIDI) || defined(FEATURE_MIDI_OUT_QCP) || \
    defined(FEATURE_QSYNTH_ADPCM)
/* Type specifications for QSynth audio decoder - the order should not be changed
*/
typedef enum {
  AUDMAIN_ADEC_MODE_NONE,              /* QSynth ADEC is not in used       */
  AUDMAIN_ADEC_MODE_YADPCM,            /* QSynth Y-ADPCM audio decoder     */
  AUDMAIN_ADEC_MODE_QCP,               /* QSynth QCELP audio decoder       */
  AUDMAIN_ADEC_MODE_ADPCM,             /* QSynth ADPCM audio decoder       */
  AUDMAIN_ADEC_MODE_AAC,               /* AAC decoder                      */
  AUDMAIN_ADEC_MODE_PCM,               /* QSynth PCM audio decoder       */
  AUDMAIN_ADEC_MODE_DEFAULT            /* QSynth default audio decoder     */
} audmain_adec_mode_enum_type;
#endif /* FEATURE_MIDI || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/* This enumerated type indicates if the APDMC file has been started with the
** play command or from the audfadec module.
*/
typedef enum {
  AUDMAIN_ADPCM_PLAY_FILE,     /* ADPCM file started with audadpcm module */
  AUDMAIN_ADPCM_PLAY_AUDFADEC  /* ADPCM file started with audfadec module */
} audmain_adpcm_play_enum_type;

/* This type is used to indicate the type of audio decoder data
*/
typedef enum {
  AUDMAIN_ADPCM_ADPCM_MODE,          /* ADPCM format          */
  AUDMAIN_ADPCM_YADPCM_MODE,         /* Y-ADPCM format        */
  AUDMAIN_ADPCM_SIGNED_PCM_MODE,     /* Signed PCM format     */
  AUDMAIN_ADPCM_UNSIGNED_PCM_MODE,   /* Unsigned PCM format   */
  AUDMAIN_ADPCM_WAV_ALAW_MODE,       /* Wav, ALaw format      */
  AUDMAIN_ADPCM_WAV_MULAW_MODE,      /* Wav, ULaw format      */
  AUDMAIN_ADPCM_INVALID_MODE
} audmain_adpcm_mode_enum_type;

/* Type to describe the adpcm format
*/
typedef struct {
  uint16 channels;           /* Number of channels                         */
  uint32 sample_rate;        /* Samplers per second                        */
  uint32 bytes_rate;         /* Average bytes per second                   */
  uint16 frame_size;         /* Block Align, num of bytes for n-channels   */
  uint16 bits_per_sample;    /* Number of bits used for each sample        */
} audmain_adpcm_format_type;
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WAVE_PB) || \
    defined(FEATURE_QSYNTH_ADPCM)
/* This type is used to indicate the start mode for ADEC playback.
*/
typedef enum {
  AUDMAIN_ADEC_START_MODE_NORMAL,  /* Normal start - playback immediately  */
  AUDMAIN_ADEC_START_MODE_DELAYED, /* Delayed start - load file and wait   */
  AUDMAIN_ADEC_START_MODE_INVALID
} audmain_adec_start_mode_enum_type;
#endif /* FEATURE_MIDI_OUT_QCP || FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_MIDI_OUT_QCP
/* This type is used to indicate the repeat mode for ADEC playback.
*/
typedef enum {
  AUDMAIN_ADEC_REPEAT_MODE_DISABLED, /* Repeat mode disabled - play file once */
  AUDMAIN_ADEC_REPEAT_MODE_ENABLED,  /* Repeat mode enabled - repeat file the */
                                     /* indicated number of times             */
  AUDMAIN_ADEC_REPEAT_MODE_INVALID
} audmain_adec_repeat_mode_enum_type;

/* This structure is used to indicate the playback parameters for ADEC
*/
typedef struct {
  audmain_adec_start_mode_enum_type  start_mode;  /* Start mode for playback  */
  audmain_adec_repeat_mode_enum_type repeat_mode; /* Repeat mode for playback */
  uint32                             repeat_num;  /* Number of iterations for */
                                              /* repeat: 0 = rpt indefinitely */
} audmain_midi_out_adec_param_type;
#endif /* FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_MM_REC
#ifdef FEATURE_AAC_REC
/* Structure for each recorded AAC frame information */
typedef struct {
  uint32 frame_cnt;                         /* Frame number encoded           */
  uint32 data_length;                       /* Data length in this frame      */
} audmain_aac_record_frame_type;

/* Data holder to get data from DSP for AAC record */
typedef struct {
  audmain_aac_record_frame_type buf1_frame_info[VOC_AAC_ENC_MAX_FRAME]; 
                                 /* Holder for each frame info in buffer1    */
  uint16 buf1_frame_num;         /* Number of frames recorded in buffer1     */
  uint16 lost_frame1;            /* lost frame number in buffer1 recorded    */
  boolean buf1_flush;            /* Wether need to send buffer to cmx        */
  boolean buf1_avsync;           /* A/V sync report required                 */
  uint32  samples_rpt1;          /* A/V sync report samples for buffer1      */
  uint32  sync_frame1;           /* Frame number need to report              */

  audmain_aac_record_frame_type buf2_frame_info[VOC_AAC_ENC_MAX_FRAME]; 
                                 /* Holder for each frame info in buffer2    */
  uint16 buf2_frame_num;         /* Number of frames recorded in buffer2     */
  uint16 lost_frame2;            /* lost frame number in buffer2 recorded    */
  boolean buf2_flush;            /* Wether need to send buffer to cmx        */
  boolean buf2_avsync;           /* A/V sync report required                 */
  uint32  samples_rpt2;          /* A/V sync report samples for buffer2      */
  uint32  sync_frame2;           /* Frame number need to report              */
} audmain_aac_record_data_type;

/* Available Buffer information for AAC data record */
typedef struct {
  uint8  *buf1;                             /* Buffer1 starting address       */
  uint32 buf1_space;                        /* Available space of buffer1     */

  uint8  *buf2;                             /* Buffer2 starting address       */
  uint32 buf2_space;                        /* Available space of buffer2     */

  uint16 hdr_offset;                        /* Format header offset           */
  uint32 sync_samples;                      /* A/V sync required sync samples */
  uint32 sync_frames;                       /* Reported A/V sync frame number */
} audmain_aac_record_buf_type;
#endif /* FEATURE_AAC_REC */

/* Data holder to get data from DSP during MultiMedia recording */
typedef union {
  uint16 dummy_buf;
#ifdef FEATURE_AAC_REC
  audmain_aac_record_data_type aac_buf; /* Recorded AAC data              */
#endif /* FEATURE_AAC_REC */
} audmain_mm_record_data_type;

/* Buffer  holder to get data from DSP during MultiMedia recording */
typedef union {
  uint8 *dummy_buf;
#ifdef FEATURE_AAC_REC
  audmain_aac_record_buf_type aac_buf;  /* Recorded AAC data              */
#endif /* FEATURE_AAC_REC */
} audmain_mm_record_buf_type;
#endif /* FEATURE_MM_REC */

typedef enum {
  AUDMAIN_AUDFMT_POLY_MODE_0,    /* All HP-mode, 72 HP voices  */
  AUDMAIN_AUDFMT_POLY_MODE_1,    /* 56 HP voices, 4 HQ voices */
  AUDMAIN_AUDFMT_POLY_MODE_2,    /* 46 HP voices, 8 HQ voices  */
  AUDMAIN_AUDFMT_POLY_MODE_3,    /* 38 HP voices, 12 HQ voices */
  AUDMAIN_AUDFMT_POLY_MODE_4,    /* 30 HP voices, 16 HQ voices */
  AUDMAIN_AUDFMT_POLY_MODE_5,    /* 20 HP voices, 20 HQ voices */
  AUDMAIN_AUDFMT_POLY_MODE_6,    /* 12 HP voices, 24 HQ voices */
  AUDMAIN_AUDFMT_POLY_MODE_7     /* All HQ-mode, 32 HQ voices  */
} audmain_audfmt_poly_mode_type ;

#ifdef FEATURE_GASYNTH_INTERFACE
typedef enum {
  AUDMAIN_VOC_ADEC_QCP,      /* QCP audio decoder */
  AUDMAIN_VOC_ADEC_AMR       /* AMR audio decoder */
} audmain_voc_adec_enum_type;
#endif /* FEATURE_GASYNTH_INTERFACE */

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDMAIN Public Function Prototypes
**---------------------------------------------------------------------------
*/
#ifdef FEATURE_GASYNTH_INTERFACE
/*===========================================================================

FUNCTION audmain_voc_adec_start

DESCRIPTION
  This function starts QCP or AMR-NB playback.

DEPENDENCIES
  None

RETURN VALUE
  A value of audmain_status_type to indiciate whether DSP is properly
  configured to accept 13K audio data. 

SIDE EFFECTS
  None
===========================================================================*/
extern audmain_status_type audmain_voc_adec_start (
  uint32                            channel,
  audmain_voc_adec_enum_type        voc_type,
  audmain_adec_start_mode_enum_type start_mode
);

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
extern audmain_status_type audmain_adec_ppbuf_adjust (
  void *state_type, 
  uint32 *buf1_used,
  uint32 *buf2_used,
  uint8  *buf_usage_mask
);

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
extern boolean audmain_adec_data_consumed(
  uint8 channel
);
#endif /* FEATURE_GASYNTH_INTERFACE */

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
extern audmain_status_type audmain_get_header_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint8                **data
);

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
extern audmain_status_type audmain_get_dls (
  snd_af_access_type   *handle,
  uint32               position,
  uint32               length,
  void                 *client_data
);

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
extern audmain_status_type audmain_get_async_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint8                **data
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_data

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
extern audmain_status_type audmain_get_data (
  snd_af_access_type *handle,
  uint32             num,
  uint8              **data
);

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
);

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
extern audmain_status_type audmain_get_var_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint32               buf_size,
  uint32               offset,
  uint8                **data
);

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
extern audmain_status_type audmain_set_adec_mode (
  audmain_adec_mode_enum_type mode
);
#endif /* FEATURE_MIDI || FEATURE_MIDI_OUT_QCP || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#ifdef FEATURE_MIDI
/* This type used to specified the qsynth volume level
*/
typedef enum {
  AUDMAIN_QSYNTH_DEFAULT_VOL_LVL = 0,
                                /* Default volume level for MIDI/WEBAUDIO */
  AUDMAIN_QSYNTH_SMAF_VOL_LVL = 1 ,
                                /* Default volume level for SMAF */
  AUDMAIN_QSYNTH_SMAF_VOL_LVL_MA2=AUDMAIN_QSYNTH_SMAF_VOL_LVL,
                                /* Volume level for SMAF MA-2 */
  AUDMAIN_QSYNTH_SMAF_VOL_LVL_MA3=AUDMAIN_QSYNTH_SMAF_VOL_LVL_MA2,
                                /* Volume level for SMAF MA-3 */  
  AUDMAIN_QSYNTH_SMAF_VOL_LVL_MA5=2 
                                /* Volume level for SMAF MA-5 */
} audmain_qsynth_vol_mode_enum_type;


/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_qsynth_vol_mode

DESCRIPTION
  This function sets the qsynth volume level.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_set_qsynth_vol_mode (
  audmain_qsynth_vol_mode_enum_type mode
);

/* <EJECT> */
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
/* This structure is used to initialize tables for DLS resources */ 
typedef struct {
  uint32 hp_inst_def;   /* Table for HP intrument definitions */
  uint32 hp_drum_def;   /* Table for HP drum definitions      */
  uint32 hp_wave_info;  /* Table for HP waveform definitions  */
  uint32 hq_inst_def;   /* Table for HQ intrument definitions */
  uint32 hq_drum_def;   /* Table for HQ drum definitions      */
  uint32 hq_wave_info;  /* Table for HQ wave definitions      */
  uint32 hq_inst_num;   /* Number of HQ instruments           */
  uint32 hq_drum_num;   /* Number of HQ drums                 */
} audmain_dls_config_type;

extern void audmain_set_dls_config (
  audmain_dls_config_type *dls_config
);

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
extern void audmain_set_hybrid_mode (
  audmain_audfmt_poly_mode_type hybrid_mode
);

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
extern audmain_status_type audmain_audfmt_pause_seq (
  uint32  sequence,
  boolean pause_flag
);

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
extern audmain_status_type audmain_audfmt_set_vol_seq (
  uint32  sequence,
  uint16  vol
);

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
extern void audmain_audfmt_check_reset (void);

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
extern void audmain_dls_get_data_cb_req (
  void    *data,
  uint8   *buf,
  uint32  length,
  uint32  position
);

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
extern audmain_status_type audmain_dls_get_data (
  snd_af_data_cmd_enum command,
  snd_af_access_type   *handle,
  uint32               num,
  snd_af_read_cb_type  *parm
);

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
extern void audmain_dls_load (
  snd_af_access_type *handle,
  uint32             byte_offset
);

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
extern void audmain_dls_unload (
  snd_af_access_type *handle
);
#endif /* FEATURE_DLS */
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
extern audmain_adec_mode_enum_type audmain_get_adec_mode (void);

/* <EJECT> */
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
extern audmain_status_type audmain_get_var_adec_data (
  snd_af_access_type   *handle,
  uint32               num,
  uint32               buf_size,
  uint32               offset,
  uint8                **data
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_adec_data

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
extern audmain_status_type audmain_get_adec_data (
  snd_af_access_type *handle,
  uint32             num,
  uint8              **data
);

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
extern audmain_status_type audmain_audfmt_adec_chan_start (
  uint32  channel
);

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
extern audmain_status_type audmain_audfmt_adec_chan_pause (
  uint32  channel,
  boolean pause_flag
);

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
extern void audmain_audfmt_adec_chan_flush (
  uint32  channel
);
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
extern audmain_status_type audmain_audfmt_adec_mvol (
  uint16 vol
);

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
extern audmain_status_type audmain_audfmt_adec_chan_vol (
  uint32 channel,
  uint16 vol
);

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
extern audmain_status_type audmain_audfmt_adec_chan_pan (
  uint32 channel,
  sint15 pan
);
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
extern void audmain_do_play_cb (
  snd_af_access_type   *handle,
  snd_af_data_cmd_enum command,
  uint32               num,
  void                 *data
);

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
extern void audmain_play_cb (
  void                  *state_type,
  snd_af_access_type    *handle,
  snd_af_data_cmd_enum  command,
  uint32                num,
  void                  *data
);

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
extern void audmain_reset_parms(void);


/* <EJECT> */
/*===========================================================================

FUNCTION audmain_reset

DESCRIPTION
  This function indicates that playback has ended and audmain should reset
  its state.

DEPENDENCIES
  audmain_parse_ctl

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_reset ( void );

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
extern void audmain_reset_calc_time ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_parse_type

DESCRIPTION
  This function returns the type of file being parsed.

DEPENDENCIES
  None

RETURN VALUE
  audmain_file_type - Type of file being parsed (MIDI, QCP, MP3, ...).

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_file_type audmain_get_parse_type (
  void   *state_type
);

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
extern boolean audmain_get_pause_flag (
  void   *state_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_get_pause

DESCRIPTION
  This function returns the state of the pause flag for the indicated
  midi/WebAudio parse.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if playback is paused
  FALSE - if playback is not paused

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audmain_get_pause (
  void *state_type
);

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
extern boolean audmain_get_valid_time (
  void   *state_type
);

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
extern snd_sample_rate_type audmain_get_sample_rate (
  void                     *state_type
);

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
  audmain_parse_ctl

===========================================================================*/
extern void audmain_set_read_sync (
  boolean sync
);

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
extern void audmain_set_init_done (
  void *state_type
);

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
);

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
extern void audmain_set_pause (
  void       *state_type,
  boolean    pause
);

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
extern void audmain_set_ff_start (
  void *state_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_set_parse_done

DESCRIPTION
  This function sets the parse done flag for the indicated parse.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_set_parse_done (
  void *state_type
);

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
extern audmain_status_type audmain_set_tempo (
  void    *state_type,
  uint32  *tempo_factor
);

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
extern audmain_status_type audmain_set_tune (
  void  *state_type,
  uint8 *tune_factor
);

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
extern audmain_status_type audmain_set_pan (
  void  *state_type,
  uint8 *pan_factor
);

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
extern void audmain_get_final_cmd (
  audmain_cmd_buf_type *cmd_buf
);

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
extern void audmain_audfmt_check_reset_req ( void );
#endif /* FEATURE_MIDI */

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
extern void audmain_header_data_cb_req (
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
);

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
extern void audmain_dls_data_cb_req (
  uint8  *data,
  uint8  *buf,
  uint32 length,
  void   *client_data
);

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
extern boolean audmain_get_data_cb_req(
  uint8 **data,
  uint8 *buf,
  uint32 length,
  uint32 pos
);

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
extern audmain_status_type audmain_adec_adjust (
  void                     *state_type,
  uint32                   *num_used
);
#endif

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
extern audmain_status_type audmain_format_play_init (
  snd_af_access_type       *handle,
  snd_cmx_format_spec_type *input_spec,
  void                     *state_ptr_ptr,
  audmain_cmd_buf_type     **cmd_buf
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_parse_head

DESCRIPTION
  This function parses the head of a file to determine what type of file it
  is. It then calls the appropriate setup function.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS -  if the file's header was parsed and recognized
                            successfully
  AUDMAIN_STATUS_FAILURE -  if the the file was not parsed or recognized
                            successfully

SIDE EFFECTS
  audmain_parse_ctl
  audmain_read_sync

===========================================================================*/
extern audmain_status_type audmain_parse_head (
  snd_af_access_type      *handle,
  void                    *state,
  audmain_cmd_buf_type    **cmd_buf,
  audmain_play_param_type *param
);

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
extern void audmain_age ( 
  void          *state_type,
  uint32        *elapsed_time,
  uint32        *adjusted_time 
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_parse

DESCRIPTION
  This function parses one time event of a the specified parse.

DEPENDENCIES
  audmain_buf_mgmt

RETURN VALUE
  audmain_status_type
  time_remain - time, in ms, until next event.

SIDE EFFECTS
  audmain_buf_mgmt

===========================================================================*/
extern audmain_status_type audmain_parse (
  void                    *state_type,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
);

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
extern void audmain_do_preparse (
  snd_af_access_type    *handle
);

/* <EJECT> */
/*===========================================================================

FUNCTION audmain_flush

DESCRIPTION
  This function flushes audio buffers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmain_flush (
  void     *state_type
);

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
extern audmain_status_type audmain_fforward (
  void     *state_type,
  uint32   ms_forward,
  uint32   *elapsed_ms,
  uint32   *adjusted_ms
);

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
extern audmain_status_type audmain_rewind (
  void     *state_type,
  uint32   ms_rewind,
  uint32   *elapsed_ms,
  uint32   *adjusted_ms
);

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
extern audmain_status_type audmain_do_seek (
  void     *state_type,
  uint32   *elapsed_ms,
  uint32   *adjusted_ms
);

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
extern audmain_status_type audmain_get_time (
  snd_af_access_type  *handle,
  uint32              byte_offset,
  void                *state_ptr_ptr
);

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
);

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
);

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
extern audmain_status_type audmain_get_song_config (
  void                     *state_type,
  audmain_song_config_type **config
);

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
);
#endif /*FEATURE_ANIMATED_AAC */

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
extern audmain_status_type audmain_reset_sequence (
  void    *sequence
);

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
extern audmain_status_type audmain_pause_sequence (
  void    *sequence,
  boolean pause_flag
);
                
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
  uint16 *seq_num
);

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
extern audmain_status_type audmain_fforward_sequence (
  void    *sequence,
  uint32  ms_forward
);

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
extern audmain_status_type audmain_rewind_sequence (
  void    *sequence,
  uint32  ms_rewind
);

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
extern audmain_status_type audmain_set_tempo_sequence (
  void    *sequence,
  uint32  tempo_factor
);

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
extern audmain_status_type audmain_set_tune_sequence (
  void    *sequence,
  uint32  tune_factor
);

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
extern audmain_status_type audmain_set_pan_sequence (
  void    *sequence,
  uint16  pan_factor
);

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
);

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
extern void audmain_reset_qcp_func ( void );

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
extern void audmain_qcp_get_data_size (
  snd_af_access_type *handle
);

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
extern void audmain_qcp_get_frames (
  snd_af_access_type *handle
);

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
extern void audmain_qcp_get_cnfg (
  snd_af_access_type *handle
);

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
extern void audmain_qcp_set_cnfg (
  uint16                         cnfg,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
);

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
extern void audmain_qcp_get_labl (
  snd_af_access_type *handle
);

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
extern void audmain_qcp_set_labl (
  uint8                          *buffer,
  uint32                         num_bytes,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
);

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
extern void audmain_qcp_get_text (
  snd_af_access_type *handle
);

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
extern void audmain_qcp_set_text (
  uint8                          *buffer,
  uint32                         num_bytes,
  snd_af_access_type             *handle,
  snd_qcp_rec_cb_func_ptr_type   cb_func,
  const void                     *client_data
);

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
extern audmain_status_type audmain_record (
  snd_qcp_format_type          format,
  snd_qcp_dir_type             link,
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

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
extern audmain_status_type audmain_record_stop ( 
  audmain_rec_status_type status
);

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
extern void audmain_rec_voc_cb (
  audmain_rec_status_type status,
  uint8                   *buffer,
  uint32                  num_bytes,
  uint32                  num_frames
);

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
extern audmain_status_type audmain_get_buffer (
  snd_qcp_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);
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
extern voc_pcm_client_output_fn_type audmain_pcm_record (
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
extern audmain_status_type audmain_pcm_record_stop (
  snd_pcm_rec_dir_type  link
);

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
extern audmain_status_type audmain_pcm_record_pause (
  boolean               pause,
  snd_pcm_rec_dir_type  link
);

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
extern void audmain_pcm_rec_status (
  snd_af_data_cmd_enum         status,
  snd_pcm_rec_data_struct_type *data,
  snd_pcm_rec_cb_func_ptr_type cb_func,
  const void                   *client_data
);

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
extern void audmain_pcm_rec_send_status (
  snd_af_data_cmd_enum status
);

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
extern void audmain_pcm_record_buf (
  uint8       *buffer,
  uint32      num_bytes,
  const void  *client_data
);
#endif /* FEATURE_PCM_REC */

#ifdef FEATURE_MM_REC
/*===========================================================================
FUNCTION audmain_mm_record_start

DESCRIPTION
  This function starts a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean audmain_mm_record_start (
  snd_mm_rec_aud_type         *rec_param,   /* Record parameters              */
  snd_mm_rec_cmd_struct_type  *buf,         /* MM record buffer info          */
  snd_mm_rec_cb_func_ptr_type cb_func,      /* Callback function to CMX       */
  const void                  *client_data  /* Client data pointer            */
);

/*===========================================================================

FUNCTION audmain_mm_record_stop

DESCRIPTION
  This function stops a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmain_mm_record_stop (
  snd_mm_rec_format_type format
);

/*===========================================================================

FUNCTION audmain_mm_record_pause

DESCRIPTION
  This function pauses/resumes a MultiMedia recording session.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern audmain_status_type audmain_mm_record_pause (
  boolean                pause,
  snd_mm_rec_format_type format
);

/*===========================================================================

FUNCTION audmain_mm_rec_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_mm_rec_status (
  snd_af_data_cmd_enum        status,
  snd_mm_rec_data_struct_type *data,
  snd_mm_rec_cb_func_ptr_type cb_func,
  const void                  *client_data
);

/*===========================================================================

FUNCTION audmain_mm_rec_send_status

DESCRIPTION
  This function sends the indicated status back to the client.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_mm_rec_send_status (
  snd_mm_rec_format_type format,
  snd_af_data_cmd_enum   status
);

/*===========================================================================

FUNCTION audmain_mm_record_buf

DESCRIPTION
  This function accepts a buffer for MultiMedia recording.

DEPENDENCIES
  FEATURE_MM_REC is defined.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_mm_record_buf (
  uint8                  *buffer,
  uint32                 num_bytes,
  const void             *client_data,
  snd_mm_rec_format_type format
);

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
extern void audmain_mm_record_frame (
  snd_mm_rec_format_type      mm_type,
  audmain_mm_record_data_type *frame_info
);

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
extern boolean audmain_mm_record_get_buf (
  snd_mm_rec_format_type     mm_type,
  audmain_mm_record_buf_type *buf_type
);
#endif /* FEATURE_MM_REC */

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM 
/*===========================================================================

FUNCTION audmain_adpcm_stop

DESCRIPTION
  This function stop ADPCM playback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void audmain_adpcm_stop ( uint32 channel );
#endif /* FEATURE_QSYNTH_ADPCM */

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
extern audmain_status_type audmain_adpcm_start (
  uint32                            channel,
  audmain_adpcm_play_enum_type      play_type,
  audmain_adpcm_format_type         *format,
  audmain_adpcm_mode_enum_type      mode,
  audmain_adec_start_mode_enum_type start_mode
);
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)
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
extern audmain_status_type audmain_adec_reset_channel (
  void   *state_type,
  uint32 channel
);

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
extern audmain_status_type audmain_adec_parse (
  void                    *state_type,
  audmain_adec_cmd_type   *cmd_buf
);
#endif /* FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
#ifdef FEATURE_MIDI_OUT_QCP
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
extern boolean audmain_check_midi_out_qcp (
  void *state_type
);

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
extern boolean audmain_check_midi_out_qcp_channel (
  void   *state_type,
  uint32 channel
);

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
extern audmain_status_type audmain_midi_out_qcp (
  void                             *state_ptr_ptr,
  uint32                           channel,
  snd_af_access_type               *handle,
  audmain_midi_out_adec_param_type *param
);

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
extern audmain_status_type audmain_midi_out_adec_start_channel (
  void   *state_type,
  uint32 channel
);

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
extern void audmain_midi_out_qcp_reset (
  void *state_type
);

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
extern audmain_status_type audmain_midi_out_qcp_pause (
  void    *state_type,
  boolean pause_flag
);
#endif /* FEATURE_MIDI_OUT_QCP */

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
);

#endif /* FEATURE_AUDFMT_HYBRID_MODE */

/*==========================================================================
 FUNCTION audmain_get_read_size

 DESCRPTION
   This function returns the size of SYNC data read through audmain_get_data.

 DEPENDENCIES
   audmain_read_sync must be true.

 RETURN VALUE
   SYNC read size in bytes.

 SIDE EFFECTS
   None
 ==========================================================================*/
uint32 audmain_get_read_size (void);

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
void audmain_audfmt_do_time (void);

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
);	

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
extern void audmain_cleanup_vbr (
  audmain_file_type file_type
);

#endif /* FEATURE_AUDIO_FORMAT */
#endif /* AUDMAIN_H */
