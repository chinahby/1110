#ifndef AUDFADECI_H
#define AUDFADECI_H
/*===========================================================================

             A U D I O   F O R M A T s   A U D I O   D E C O D E R
                   I N T E R N A L   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions for 
  the audio format audio decoder module.

Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audfadeci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/08   sk/pl   Removed AUDFADEC_MASK_DONE_PENDING as it's not need anymore.
	           (CR 125651) 
04/07/07    pl     Added support for AMR-NB & QCP all-rate playback.
01/18/07    ss     Added support to play fractional sample rate wave bitstream.
09/01/06    pl     Added support for deadlock preverntion during repeat.
            lg     Fixed an issue with QCP within PMD ending soon.
06/26/06    pl     Added new AUDFADEC_MASKs and redefined 
                   audfadec_adpcm_state_enum_type.
03/13/06    st     Added support for delayed start and repeat mode for playback
                   of audio files.
09/18/05    st     Added AUDFADEC_MASK_CONT_DATA to audfadec_status_mask_type.
05/08/05    st     Added frame_size parameter to audfadec_wa_reserve_type.
03/10/05    st     Added AUDFADEC_MASK_MFI and AUDFADEC_MASK_MFI_CONT.
05/12/04    st     Modified code to increase buffer size to accommodate 
                   higher supported sampling rates.
04/22/04    lg     Modified code to support Animated AAC Ringers.
04/16/04    st     Added support to decode stereo YADPCM clips.
03/22/04    st     Modifed code to support PCM playback; added
                   AUDFADEC_MASK_COPY_PEND.
10/16/03  aw/st    Added support for SMAF.
09/26/02  aw/st    Added support to playback ADPCM wave data in webaudio file.
08/10/02  aw/st    Added support for ADPCM audio decoder.
11/10/01    st     Modified code to support delayed audio decoder command
                   from WebAudio source.
10/01/01    st     Modified AUDFADEC_BUF_LENGTH from 1000 to 500 in order
                   to reduce RAM usage.
09/03/01    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#include "snd.h"

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP)

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* These defines are used for the audio decoder. They indicate the size
** of the local audio decoder buffer in bytes, the size of one frame of
** QCP data and the offset into the buffer where data should be stored
** initially. These numbers are dictated by ADPCM because QCP has fewer
** requirements. The ADPCM max frame length refers to the amount of space
** to leave free in each buffer in order to conform to DSP interface
** for ADPCM. NOTE: For ADPCM the AUDFADEC_BUF_LENGTH must be a multiple
** of 8 because data given to the DSP must be in a 32-bit aligned buffer.
*/
#ifdef FEATURE_GASYNTH_INTERFACE
#define AUDFADEC_BUF_LENGTH              4000
#define AUDFADEC_ADPCM_FRAME_LENGTH_MAX  0
#define AUDFADEC_AMR_MAX_FRAME_LENGTH    31
#define AUDFADEC_AMR_FRAME_TYPE_MASK     0x78
#define AUDFADEC_AMR_FRAME_TYPE_SHIFT    3
#define AUDFADEC_AMR_FRAME_QUALITY_MASK  0x04
#else 
#define AUDFADEC_BUF_LENGTH              2000
#ifdef FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE
#define AUDFADEC_ADPCM_FRAME_LENGTH_MAX  336
#else
#define AUDFADEC_ADPCM_FRAME_LENGTH_MAX  324
#endif /* FEATURE_QSYNTH_FRACTIONAL_SAMPLE_RATE */
#endif /* FEATURE_GASYNTH_INTERFACE */

#define AUDFADEC_FRAME_LENGTH            34
#define AUDFADEC_INIT_OFFSET             (AUDFADEC_ADPCM_FRAME_LENGTH_MAX * 2)
#define AUDFADEC_INT_BUF_LENGTH          (AUDFADEC_BUF_LENGTH >> 1)

/* Bit values for audio decoder status
*/
typedef enum {
  AUDFADEC_MASK_ADEC_ENABLED   = 0x000001,/* Audio decoder processing enabled */
  AUDFADEC_MASK_INIT_DONE      = 0x000002,/* Initialization done              */
  AUDFADEC_MASK_NEXT_FLAG      = 0x000004,/* More decoder info to follow      */
  AUDFADEC_MASK_NEW_BLOCK      = 0x000008,/* Processing of new block required */
  AUDFADEC_MASK_DO_OFFSET      = 0x000010,/* Processing of offset required    */
  AUDFADEC_MASK_DSP_RESET      = 0x000020,/* Do DSP reset                     */
  AUDFADEC_MASK_WA_ADEC        = 0x000040,/* WebAudio audio decoder           */
  AUDFADEC_MASK_BUF_PENDING    = 0x000080,/* Receive buffer pending           */
  AUDFADEC_MASK_SKIP_BUF       = 0x000100,/* Skip receive buffer              */
  AUDFADEC_MASK_COPY_PEND      = 0x000200,/* Skip receive buffer              */
  AUDFADEC_MASK_WA_PEND        = 0x000400,/* WebAudio ADEC pending            */
  AUDFADEC_MASK_ADPCM          = 0x000800,/* ADPCM audio decoder              */
  AUDFADEC_MASK_YADPCM         = 0x001000,/* Y-ADPCM audio decoder            */
  AUDFADEC_MASK_PCM            = 0x002000,/* PCM audio decoder                */
  AUDFADEC_MASK_AAC            = 0x004000,/* AAC audio decoder                */
  AUDFADEC_MASK_YADPCM_DEC     = 0x008000,/* Decoding stereo YADPCM           */
  AUDFADEC_MASK_MFI            = 0x010000,/* MFi audio decoder flag           */
  AUDFADEC_MASK_MFI_CONT       = 0x020000,/* MFi continue flag                */
  AUDFADEC_MASK_CONT_DATA      = 0x040000,/* Continuous data flag             */
  AUDFADEC_MASK_DELAYED_START  = 0x080000,/* Delayed start flag               */
  AUDFADEC_MASK_REPEAT_ENABLED = 0x100000, /* Repeat enabled flag             */
  AUDFADEC_MASK_QCP            = 0x200000, /* QCP audio decoder               */
  AUDFADEC_MASK_PPBUF_INT      = 0x400000, /* Ping Pong buffer Interface on DSP */
  AUDFADEC_MASK_NEW_BLOCK2     = 0x800000,/* new block in buf2 */
  AUDFADEC_MASK_AMR            = 0x1000000, /* AMR audio decoder */
  AUDFADEC_MASK_CHECK_AUDFMT   = 0x2000000  /* Mask to allow checking for audio format 
                                               of the bistream */
} audfadec_status_mask_type;

#ifdef FEATURE_QSYNTH_ADPCM
/* This type is used to keep track of the ADPCM data buffer management
** state
*/
/* In game audio build, this enum type is used differently. It is used as
   mask to indicate buffers that are in used */
typedef enum {
  AUDFADEC_ADPCM_STATE_NONE = 0x0,       /* Initial state, no ADPCM buffers */
  AUDFADEC_ADPCM_STATE_BUFFER1 = 0x1,   /* Currently using buffer 1        */
  AUDFADEC_ADPCM_STATE_BUFFER2 = 0x2,    /* Currently using buffer 2        */
  AUDFADEC_ADPCM_STATE_LOCKED1 = 0x04 
} audfadec_adpcm_state_enum_type;
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_WEBAUDIO
#ifdef FEATURE_QSYNTH_ADPCM
/* Supported ADPCM's sample rate in webaudio file
*/
typedef enum {
  AUDFADEC_ADPCM_SAMPLE_RATE_8000    = 0x0,
  AUDFADEC_ADPCM_SAMPLE_RATE_16000   = 0x1,
  AUDFADEC_ADPCM_SAMPLE_RATE_32000   = 0x2,
  AUDFADEC_ADPCM_SAMPLE_RATE_UNKNOWN = 0x3
} audfadec_wa_adpcm_sample_rate_type;

/* This macro will extract the sample rate and block size from the adpcm 
** header in the webaudio file
*/
#define WA_ADPCM_RATE(v) (((v) & 0xc0) >> 6)
#define WA_ADPCM_BLK(v,u)  ((((v) & 0x0F) << 8) | (u))

/* This macro will convert the ADPCM sample rate from webaudio format to 
** decimal value
*/
#define ADPCM_SAMPLE_RATE(v) \
  ((v == AUDFADEC_ADPCM_SAMPLE_RATE_8000)?8000: \
   ((v == AUDFADEC_ADPCM_SAMPLE_RATE_16000)?16000: \
    ((v == AUDFADEC_ADPCM_SAMPLE_RATE_32000)?32000:0)))
#endif /* FEATURE_QSYNTH_ADPCM */


typedef struct {
  snd_af_access_type
               *handle;          /* Info to callback client                  */
  uint32       status_flag;      /* Flag indicates status of ADEC processing */
  uint32       next_block;       /* Location of next block of ADEC data      */
  uint32       block_start;      /* Location of the start of current block   */
  uint32       file_location;    /* Current location in the ADEC file        */
  uint32       length_not_read;  /* Number of bytes still left to read       */

  uint16       sample_rate_hz;   /* Sampling rate                            */
  uint16       bits_per_sample;  /* Bits per sample                          */
  uint16       channels;         /* Number of channels                       */
  uint16       frame_size;       /* Frame size                               */
  audmain_adpcm_mode_enum_type
               yadpcm_mode;      /* Mode of audio decoder data               */
#ifdef FEATURE_ANIMATED_AAC
  audmain_song_config_type
               aac_config;       /* AAC config info                          */
  snd_sample_rate_type 
               aac_sample_rate;  /* AAC sample rate                          */
#endif /* FEATURE_ANIMATED_AAC */
} audfadec_wa_reserve_type;
#endif /* FEATURE_WEBAUDIO */

/* This structure is used in processing audio decoder data
*/
typedef struct {
  snd_af_access_type
               *handle;          /* Info to callback client                  */
  uint32       status_flag;      /* Flag indicates status of ADEC processing */
  uint32       skip_position;    /* Position of buffer to skip               */
  uint32       next_block;       /* Location of next block of ADEC data      */
  uint32       block_start;      /* Location of the start of current block   */
  uint32       file_location;    /* Current location in the ADEC file        */
  uint32       length_not_read;  /* Number of bytes still left to read       */

  uint32       orig_file_location;   /* Location of start of data in file    */
  uint32       orig_length_not_read; /* Total length of audio data           */
  uint32       repeat_num;           /* Number of times to repeat file       */

  uint32       buffer_orig;      /* Num of bytes from original WA buffers    */
  uint32       buffer_pad;       /* Num of bytes before padding is required  */
  uint32       buffer_length;    /* Number of bytes in data buffer           */
  uint32       buffer_index;     /* Index of head of data buffer             */
  uint8        buffer[AUDFADEC_BUF_LENGTH]; /* Data buffer                   */
  uint8        *track_buf;       /* Buffer containing more data              */
  uint32       track_index;      /* Index into track_buf                     */
  uint32       track_buf_size;   /* Number of bytes in buffer track_buf      */
  uint8        *track_buf2;      /* Buffer containing more data              */
  uint32       track_index2;     /* Index into track_buf2                    */
  uint32       track_buf_size2;  /* Number of bytes in buffer track_buf2     */
  uint16       frame_size;       /* WebAudio ADPCM decoder or continous data
                                    frame size                               */
  uint32       intbuff_length;
#ifdef FEATURE_QSYNTH_ADPCM
  audfadec_adpcm_state_enum_type
               adpcm_state;      /* ADPCM data ping-pong buffer state        */
#endif /* FEATURE_QSYNTH_ADPCM */

#ifdef FEATURE_WEBAUDIO
  audfadec_wa_reserve_type
               wa_reserve;       /* WebAudio Audio decoder reserve structure */
#ifdef FEATURE_QSYNTH_ADPCM
  audfadec_wa_adpcm_sample_rate_type
               sample_rate;      /* WebAudio ADPCM decoder sample rate       */
  
  uint16       sample_rate_hz;   /* Sampling rate                            */
  uint16       bits_per_sample;  /* Bits per sample                          */
  uint16       channels;         /* Number of channels                       */
  audmain_adpcm_mode_enum_type
               yadpcm_mode;      /* Mode of audio decoder data               */

  sint31       signal_left;      /* Used to decode left channel of YADPCM    */
  sint31       step_left;        /* Used to decode left channel of YADPCM    */
  sint31       signal_right;     /* Used to decode right channel of YADPCM   */
  sint31       step_right;       /* Used to decode right channel of YADPCM   */
#endif /* FEATURE_QSYNTH_ADPCM */
#endif /* FEATURE_WEBAUDIO */
#ifdef FEATURE_ANIMATED_AAC
  audmain_song_config_type  aac_config;  /* AAC Config info                  */
  snd_sample_rate_type      aac_sample_rate; /* sample rate                  */
#endif /*FEATURE_ANIMATED_AAC */
} audfadec_track_type;

/* Structure containing the number of supported tracks of audio
** decoder parses.
*/
typedef struct {
  audfadec_track_type    tracks[AUDFADEC_NUM_TRACKS]; /* ADEC tracks          */
#ifdef FEATURE_WEBAUDIO
  audmain_parse_ctl_type *main_parse;                 /* Main parse structure */
#endif /* FEATURE_WEBAUDIO */
} audfadec_parse_ctl_type;

#endif /* defined(FEATURE_WEBAUDIO) || defined(FEATURE_MIDI_OUT_QCP) */
#endif /* AUDFADECI_H */
