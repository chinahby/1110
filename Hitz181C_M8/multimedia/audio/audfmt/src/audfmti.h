#ifndef AUDFMTI_H
#define AUDFMTI_H
/*===========================================================================

                  A U D I O   F O R M A T S   I N T E R N A L
                          H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions for 
  the audio formats module.

Copyright (c) 2000 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audfmti.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/09    gs     For cmx5p0, modified code to send last channel pitch, pan,
                   vol during seek, to reduce the cmds sent to dsp
11/18/08   vsud    Added structure for CC10 Pan handling.
11/14/08   vsud    Handled SMAF content which have no EOS but NOP @ end of MTSQ.
10/02/08    vsud   Fixed CR# 157201 for SMAF file play back issues.
06/10/08    kd     Fixed CR# 137293 add support for playback of offset binary pcm data 
06/23/08    ry     Added SMAF support for LED/VIB event synchronization.
02/28/08   wfd     Integrate ARM changes for DLS CMX 5.0 support.
12/20/07    sk     Modified code to update the CMID_EXT_MAX_BUFFER to 0x40
	           CR 113690.
05/10/07    sk     Added support for SMAF Phrase L2; added support for SMAF HP
                   note velocity command.
03/30/07    sk     Added support for handling master volume command in 
                   SMAF files CR 111604
01/11/07    ss     Increased CMID_NOTE_OFF_MAX_COMMANDS to 144.
06/15/06    st     Added CMID_PROGRAM_ORIG() macro, CR95611.
06/09/06    st     Added support for sound effects bank.
06/02/06    st     Modified AUDFMT_CMD_BUF_MAX_WORDS from 100 to 128; added
                   support to process wave data chunk after wsmp chunk is
                   found in DLS parser, CR88398.
03/01/06    lg     Added support for SMAF meta data in AUDIO_SPEC callback
12/22/05  lg/sk    Modified code to ignore duplicate Jump Ids in MFI.(CR#85509)
11/18/05    st     Added peg_depth variables in audfmt_dls_state_type and
                   audfmt_inst_def_type structures.
10/18/05    st     Added AUDFMT_DLS_CHUNK_ART2 to audfmt_dls_chunk_enum_type.
07/22/05    lg     Added curent_offset to audfmt_track_type for SMAF. 
                   Added AUDFMT_MASK_SMAF_FM16 and AUDFMT_MASK_SMAF_DEFAULT_HQ
                   to audfmt_status_mask2_type
07/19/05    pl     Added AUDFMT_STATUS_DATA_ERR to audfmt_status_type
                   and error_status to audfmt_parse_type.
06/15/05    st     Added support for SVG commands in c-midi files.
05/31/05    st     Added failed_cmd_cnt to audfmt_parse_type.
05/25/05    st     Modified code to add support for start and end offsets for
                   SMAF MA3/MA5 files.
05/08/05    st     Increased CMID_NOTE_OFF_MAX_COMMANDS to 72.
04/06/05    st     Added MELO_DEFAULT_ADEC_VOL.
03/10/05    st     Modified code to manage ADEC processing using
                   audfmt_adec_status_type; added support for MFi CMX extension
                   commands: WAV commands, poly mode, device control.
02/22/05    st     Modified code to add FM drum DLS entries into
                   audfmt_parse_type.
02/05/05    st     Modified code to add support for HQ DLS and DLS file parsing;
                   removed FEATURE_SAF featurization; modified code to
                   support playback from a byte offset into actual file.
01/25/05    st     Added skip_buf_count to audfmt_track_type; removed
                   AUDFMT_TRACK_MASK_SKIP_BUF.
01/11/05    st     Added definitions for MFi support.
11/01/04    st     Moved channel_active parameter so that it is outside of
                   feature definitions; added support for SMAF MA2 pitch bend
                   commands.
09/13/04    st     Added SMAF_FILE_MODE_MA5_16; added support for SMAF FM DLS;
                   added MIDI_RP_RESET_PARM.
08/11/04    st     Added AUDFMT_MASK_GRAPH_CHUNK_ENABLED.
07/02/04    st     Added volume parameter to audfmt_wav_spec_type; added
                   macros for ADEC coding for note off command processing;
                   added flag to audfmt_param_def_type structure to indicate
                   if DLS waveform is MSBit escaped.
05/12/04    st     Modified code to support Phrase files; modified code to
                   support skipping initial silence during playback.
04/23/04    lg     Added macros for dynamic SAF Object buffers.
04/22/04    lg     Modified code to support Animated AAC Ringers.
04/16/04    st     Added definitions and support for animation control.
03/22/04    st     Modified code to perform delayed pause and volume commands;
                   rearranged status bitmasks.
                   Added AUDFMT_MASK_SEEK_PAUSED, AUDFMT_MASK_CB_STORE_DONE;
                   updated audfmt_parse_type with repeat_orig, pan_factor
                   parameters and removed tune_factor_pending.
03/04/04    st     Added AUDFMT_STATUS_CMD_FOUND and AUDFMT_TRACK_MASK_CMD_PEND.
01/21/04 lg/aw     Added support for SMAF LED/Vibrator event.
11/17/03    aw     Added support for SMAF file mode.
10/16/03  aw/st    Added support for SMAF and Multisequence.
04/30/03    st     Added new language support for WebAudio files.
11/14/02    st     Added CMID_MVOL_TO_DSP() macro.
09/26/02  aw/st    Added support for wave subchunks; added support to playback
                   ADPCM wave data in webaudio file.
09/19/02    st     Added AUDFMT_MIN_CMID_TRACKS, MIDI_ALL_CONTROLLERS_RESET;
                   added support for PCPI and CNTS subchunks; added fine
                   pitchbend support.
08/07/02    st     Added bank number values for older version CMX files;
                   modified lower end value for PB range command.
07/25/02    st     Added support for exsn, exsa, exsb and exsc subchunks; added
                   support for new pitch bend and pitch bend range command;
                   added support for JPEG in WebAudio files.
03/12/02    st     Obsoleted FEATURE_QSYNTH_ADRC going forward.
11/10/01    st     Modified code to support delayed VIBE commands.
10/01/01    st     Added pnew to audfmt_cb_store_type in order to handle
                   picture clear commands that occur at the same time as
                   picture new commands.
09/04/01    st     Added proper featurization.
09/04/01    st     Modified bank values and added bank value for drum channel.
09/03/01    st     Modified code to move all the audio decoder code to the
                   audfadec module.
08/03/01    st     Fixed featurization.
07/04/01    st     Modified audfmt_cb_store_type in order to handle storing 
                   multiple text append commands.
06/20/01    st     Increased number of supported MIDI tracks from 16 to 31.
04/04/01    st     Added CMID_MAX_WAV_DSP_VOLUME.
                   Added new WebAudio format defines; updated audio decoder
                   support; added support for SAF commands and removed
                   cue point support in audfmt_cb_store_type; separated
                   buffer management of audio decoder and normal MIDI/WebAudio
                   playback.
02/28/01    st     Modified audfmt_midi_track_type to add variables used
                   during preparsing of audio format file; added 
                   audfmt_get_data_store_type; modified audfmt_ff_store_type
                   to be audfmt_cb_store_type to be used as a general callback
                   buffer type.
01/26/01    st     Modified CMID_TO_VOL_DSP macro to use 0x100 as nominal
                   gain value.
12/10/00    st     Fixed calculation for CMID_VOL_TO_DSP macro.
12/04/00    st     Added CMX2.0 adec access functions and defaults.
11/27/00  st/sm    Moved much code to audmain*.  Featurized EFS/MIDI/CMX2.0.
08/23/00    st     Modified CMID_DEFAULT_VELOCITY to 0x7E.
07/26/00    st     Moved vibrator definitions to audfmt.h.
07/25/00    st     Added CMID_CUEP_BYTES.
07/22/00    st     Added CMID_CODE_HANGUL for korean text support; added 
                   AUDFMT_CMID_MIN_TEMPO and AUDFMT_ALL_NOTE_OFF_COMMAND.
07/15/00    st     Added support for user-controlled tempo and tuning; added
                   support for character set WebAudio message; added support
                   for cue point, jump, LED and vibrator WebAudio messages.
06/13/00    st     Modified audfmt_ff_store_type to handle text positioning
                   during fast-forward/rewind; added text positioning defines.
05/19/00    st     Featurized WebAudio specific code; removed WebAudio text
                   and picture commands that are no longer supported.
04/28/00    st     Added audfmt_ff_pause_mask; added ff_pause parameter to
                   audfmt_parse_ctl_type structure.
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

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_SMAF
/*To implement the Adec volume in host*/
short smafgaintable[128]={
        0x0000, 0x0002, 0x0008, 0x0012, 0x0021, 0x0033, 0x0049, 0x0064,
        0x0082, 0x00A5, 0x00CB, 0x00F6, 0x0125, 0x0157, 0x018E, 0x01C9,
        0x0208, 0x024B, 0x0292, 0x02DD, 0x032D, 0x0380, 0x03D7, 0x0433,
        0x0492, 0x04F6, 0x055D, 0x05C9, 0x0639, 0x06AD, 0x0724, 0x07A0,
        0x0820, 0x08A4, 0x092C, 0x09B9, 0x0A49, 0x0ADD, 0x0B76, 0x0C12,
        0x0CB2, 0x0D57, 0x0E00, 0x0EAC, 0x0F5D, 0x1012, 0x10CB, 0x1188,
        0x1249, 0x130E, 0x13D7, 0x14A4, 0x1575, 0x164B, 0x1724, 0x1801,
        0x18E3, 0x19C9, 0x1AB2, 0x1BA0, 0x1C92, 0x1D87, 0x1E81, 0x1F7F,
        0x2081, 0x2187, 0x2291, 0x23A0, 0x24B2, 0x25C8, 0x26E3, 0x2801,
        0x2924, 0x2A4A, 0x2B75, 0x2CA4, 0x2DD6, 0x2F0D, 0x3048, 0x3187,
        0x32CA, 0x3411, 0x355C, 0x36AB, 0x37FF, 0x3956, 0x3AB1, 0x3C11,
        0x3D74, 0x3EDC, 0x4048, 0x41B7, 0x432B, 0x44A3, 0x461F, 0x479F,
        0x4923, 0x4AAB, 0x4C37, 0x4DC7, 0x4F5C, 0x50F4, 0x5290, 0x5431,
        0x55D5, 0x577E, 0x592B, 0x5ADB, 0x5C90, 0x5E49, 0x6006, 0x61C7,
        0x638C, 0x6555, 0x6722, 0x68F3, 0x6AC9, 0x6CA2, 0x6E7F, 0x7061,
        0x7246, 0x7430, 0x761E, 0x780F, 0x7A05, 0x7BFF, 0x7DFD, 0x7FFF
};
#endif
/* This type is used to return status for various audio format functions. */
typedef enum {
  AUDFMT_STATUS_SUCCESS,          /* Command was successful        */
  AUDFMT_STATUS_BUFF_ERR,         /* Data underflow encountered    */
  AUDFMT_STATUS_JUMP,             /* Jump command encountered      */
  AUDFMT_STATUS_CMD_BUFF_ERR,     /* Command buffer overflow       */
  AUDFMT_STATUS_STORE_ERR,        /* Stored command buffer error   */
  AUDFMT_STATUS_DONE,             /* Command is finished           */
  AUDFMT_STATUS_STOP,             /* Stop playback immediately     */
  AUDFMT_STATUS_ERROR,            /* Command encountered an error  */
  AUDFMT_STATUS_DATA_ERR,         /* Data Error                    */
  AUDFMT_STATUS_CMD_FOUND,        /* Next valid command found      */
  AUDFMT_STATUS_PENDING,          /* Status is pending             */
  AUDFMT_STATUS_FAILURE           /* Command failed                */
} audfmt_status_type;

/* Compact Midi and Midi defines and macros */
#define AUDFMT_MAX_MIDI_TRACKS     31
#define AUDFMT_MIN_CMID_TRACKS     1
#define AUDFMT_MAX_CMID_TRACKS     4
#define AUDFMT_MAX_CHANNELS        16

#define AUDFMT_CHUNK_ID_BYTES      4

#define AUDFMT_MTHD_ID             "MThd"
#define AUDFMT_MTRK_ID             "MTrk"

#ifdef FEATURE_WEBAUDIO
#define AUDFMT_CMID_ID             "cmid"
#define AUDFMT_WASF_ID             "wasf"
#define AUDFMT_MELO_ID             "melo"
#define AUDFMT_TRAC_ID             "trac"
#define AUDFMT_CRPT_ID             "crpt"
#define AUDFMT_VERS_ID             "vers"
#define AUDFMT_CODE_ID             "code"
#define AUDFMT_TITL_ID             "titl"
#define AUDFMT_DATE_ID             "date"
#define AUDFMT_SORC_ID             "sorc"
#define AUDFMT_COPY_ID             "copy"
#define AUDFMT_NOTE_ID             "note"
#define AUDFMT_EXST_ID             "exst"
#define AUDFMT_EXSA_ID             "exsa"
#define AUDFMT_EXSB_ID             "exsb"
#define AUDFMT_EXSC_ID             "exsc"
#define AUDFMT_EXSN_ID             "exsn"
#define AUDFMT_CUEP_ID             "cuep"
#define AUDFMT_HIST_ID             "hist"
#define AUDFMT_PROT_ID             "prot"
#define AUDFMT_PCPI_ID             "pcpi"
#define AUDFMT_CNTS_ID             "cnts"
#define AUDFMT_WAVE_ID             "wave"
#ifdef FEATURE_SMAF
#define AUDFMT_MMMD_ID             "MMMD"
#define AUDFMT_CNTI_ID             "CNTI"
#define AUDFMT_OPDA_ID             "OPDA"
#define AUDFMT_MTR_ID              "MTR"
#define AUDFMT_MSPI_ID             "MspI"
#define AUDFMT_MTSU_ID             "Mtsu"
#define AUDFMT_MTSQ_ID             "Mtsq"
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

#define AUDFMT_MIDI_DEFAULT_TEMPO  0x7A000
#define AUDFMT_MS_PER_MINUTE       60000
#define AUDFMT_MAX_32BIT           ((unsigned) 0xFFFFFFFF)
#define AUDFMT_MAX_24BIT           ((unsigned) 0xFFFFFF)
#define AUDFMT_MAX_16BIT           ((unsigned) 0xFFFF)
#define AUDFMT_16_OVERFLOW_FACTOR  8
#define AUDFMT_24_OVERFLOW_FACTOR  16

#ifdef FEATURE_WEBAUDIO
#define AUDFMT_CMID_DEFAULT_TEMPO  6000
#define AUDFMT_CMID_MIN_TEMPO      20
#define AUDFMT_CMID_OF_FACTOR      715827
#endif

#define AUDFMT_DRUM_CHANNEL        9

#define AUDFMT_COMMAND_SIZE_4      4
#define AUDFMT_COMMAND_SIZE_3      3

#define MIDI_HEADER_BYTES          14
#define MIDI_HEADER_LENGTH         6
#define MIDI_TRACK_HEADER_BYTES    8
#define MIDI_TYPE_0                0
#define MIDI_TYPE_1                1

#define MIDI_VAR_LENGTH_CONT_MASK  0x80
#define MIDI_VAR_LENGTH_CONT(v)    ((v & MIDI_VAR_LENGTH_CONT_MASK) == \
                                    MIDI_VAR_LENGTH_CONT_MASK)

#define MIDI_VAR_LENGTH_MAX        4
#define MIDI_VAR_LENGTH_BITS_MASK  0x7F
#define MIDI_VAR_LENGTH_BITS_SHIFT 0x07
#define MIDI_VAR_LENGTH_BITS(v)    (v & MIDI_VAR_LENGTH_BITS_MASK)
#define MIDI_VAR_LENGTH_CALC(m,v)  ((m << MIDI_VAR_LENGTH_BITS_SHIFT) | \
                                    MIDI_VAR_LENGTH_BITS(v))

#define MIDI_STATUS_BIT            0x80
#define MIDI_STATUS_BYTE(v)        ((v & MIDI_STATUS_BIT) == MIDI_STATUS_BIT)

#define MIDI_STATUS_NIBBLE         0xF0
#define MIDI_CHANNEL_NIBBLE        0x0F
#define MIDI_NOTE_OFF              0x80
#define MIDI_NOTE_ON               0x90
#define MIDI_AFTER_TOUCH           0xA0
#define MIDI_CONTROL_CHANGE        0xB0
#define MIDI_PROGRAM_CHANGE        0xC0
#define MIDI_CHANNEL_PRESSURE      0xD0
#define MIDI_PITCH_WHEEL           0xE0
#define MIDI_SYSTEM_MESSAGE        0xF0
#define MIDI_RESET_COMMAND         0xFF

#define MIDI_ALL_CHANNELS          0x10
#define MIDI_BANK_SEL_COMMAND      0x00
#define MIDI_FINE_BANK_SEL_COMMAND 0x20
#define MIDI_MODULATION_COMMAND    0x01
#define MIDI_VOLUME_COMMAND        0x07
#define MIDI_PANPOT_COMMAND        0x0A
#define MIDI_EXPRESSION_COMMAND    0x0B
#define MIDI_RP_FINE_COMMAND       0x64
#define MIDI_RP_COARSE_COMMAND     0x65
#define MIDI_RP_RESET_PARM         0x00
#define MIDI_STOP_COMMAND          0x78
#define MIDI_ALL_CONTROLLERS_RESET 0x79
#define MIDI_ALL_NOTE_OFF_COMMAND  0x7B

#define MIDI_RP_TUNE_PARM1         0x02
#define MIDI_RP_TUNE_PARM2         0x00
#define MIDI_RP_TUNE_PARM3         0x06

#define MIDI_RP_PITCH_PARM1        0x00
#define MIDI_RP_PITCH_PARM2        0x00
#define MIDI_RP_PITCH_PARM3        0x06
#define MIDI_RP_PITCH_PARM4        0x26
#define MIDI_RP_PITCH_PARM5        0x00

#define MIDI_SYS_EXCLUSIVE         0xF0
#define MIDI_END_SYS_EXCLUSIVE     0xF7

#define MIDI_META_EVENT            0xFF
#define MIDI_END_OF_TRACK          0x2F
#define MIDI_NOP_EVENT             0x00
#define MIDI_TEMPO_EVENT           0x51
#define MIDI_TEMPO_LENGTH          0x03

#ifdef FEATURE_WEBAUDIO
#define CMID_HEADER_BYTES          8
#define CMID_TRACK_HEADER_BYTES    8
#define CMID_CODE_BYTES            1
#define CMID_DATE_BYTES            8
#define CMID_VERS_BYTES            4
#define CMID_SORC_BYTES1           1
#define CMID_SORC_BYTES2           2
#define CMID_NOTE_BYTES            2
#define CMID_EXST_BYTES            2
#define CMID_HIST_BYTES            2
#define CMID_CUEP_BYTES            4
#define CMID_PCPI_BYTES            1
#define CMID_POLY_BYTES            1
#define CMID_WAVE_BYTES            1
#define CMID_SAFO_BYTES            4

#define CMID_CODE_ANSI             0x00
#define CMID_CODE_ISO8859_1        0x01
#define CMID_CODE_ISO8859_2        0x02
#define CMID_CODE_ISO8859_3        0x03
#define CMID_CODE_ISO8859_4        0x04
#define CMID_CODE_ISO8859_5        0x05
#define CMID_CODE_ISO8859_6        0x06
#define CMID_CODE_ISO8859_7        0x07
#define CMID_CODE_ISO8859_8        0x08
#define CMID_CODE_ISO8859_9        0x09
#define CMID_CODE_ISO8859_10       0x0a
#define CMID_CODE_SHIFTJIS         0x80
#define CMID_CODE_HANGUL           0x81
#define CMID_CODE_CHINESE_SIMP     0x82
#define CMID_CODE_CHINESE_TRAD     0x83
#define CMID_CODE_HINDI            0x84

#define CMID_POLY_MAX_HP           0x00
#define CMID_POLY_MAX_HQ           0x07
#define CMID_POLY_MAX              CMID_POLY_MAX_HQ
#define CMID_POLY_DEFAULT          CMID_POLY_MAX_HP

#define CMID_WAVE_MODE_ADPCM       0x00
#define CMID_WAVE_MODE_QCELP       0x01
#define CMID_WAVE_MODE_AAC         0x02

#define CMID_EXTENSION_IND         0x3F
#define CMID_EXTENSION_BYTE(v)     ((v & CMID_EXTENSION_IND) == \
                                    CMID_EXTENSION_IND)

#define CMID_EXT_CLASS_A           0x3F
#define CMID_EXT_CLASS_B           0x7F
#define CMID_EXT_CLASS_C           0xBF
#define CMID_EXT_NORMAL            0xFF

#define CMID_STATUS_A_START        0x00
#define CMID_STATUS_A_END          0x7F
#define CMID_STATUS_A(v)           (v <= CMID_STATUS_A_END)

#define CMID_STATUS_B_START        0x80
#define CMID_STATUS_B_END          0xEF
#define CMID_STATUS_B(v)           ((v >= CMID_STATUS_B_START) && \
                                    (v <= CMID_STATUS_B_END))

#define CMID_EXT_INFO_START        0xF0
#define CMID_EXT_INFO_END          0xFF
#define CMID_EXT_INFO(v)           ((v >= CMID_EXT_INFO_START) && \
                                    (v <= CMID_EXT_INFO_END))

#define CMID_NOTE_OFFSET           45
#define CMID_NOTE_MASK             0x3F
#define CMID_NOTE(v)               ((v & CMID_NOTE_MASK) + CMID_NOTE_OFFSET)

#define CMID_CHANNEL_INDEX_MASK    0xC0
#define CMID_CHANNEL_INDEX_SHIFT   6
#define CMID_CHANNEL_INDEX(v)      ((v & CMID_CHANNEL_INDEX_MASK) >> \
                                    CMID_CHANNEL_INDEX_SHIFT)

#define CMID_OCTAVE_MASK           0x03
#define CMID_OCTAVE_SHIFT(v)       (v & CMID_OCTAVE_MASK)
#define CMID_OCTAVE_PLUS1          0x01
#define CMID_OCTAVE_MINUS2         0x02
#define CMID_OCTAVE_MINUS1         0x03
#define CMID_ONE_OCTAVE            12
#define CMID_TWO_OCTAVE            24

#define CMID_DEFAULT_VELOCITY      0x7E
#define CMID_VELOCITY_MASK         0xFC
#define CMID_VELOCITY_SHIFT        1
#define CMID_VELOCITY(v)           ((v & CMID_VELOCITY_MASK) >> \
                                    CMID_VELOCITY_SHIFT)

#define CMID_FINE_PITCH_BEND_LSB(v)   (((v) << 1) & 0x7f)
#define CMID_FINE_PITCH_BEND_MSB(v,m) (((v & 0x1F) << 2) | ((m >> 6) & 0x3))

#define CMID_TEMPO_IND_MASK        0xF0
#define CMID_TEMPO_IND             0xC0
#define CMID_STATUSB_TEMPO(v)      ((v & CMID_TEMPO_IND_MASK) == CMID_TEMPO_IND)
#define CMID_TIMEBASE_NIBBLE       0x0F

#define CMID_MASTER_VOLUME         0xB0
#define CMID_MASTER_BALANCE        0xB1
#define CMID_MASTER_MUTE           0xB2
#define CMID_MASTER_TUNE           0xB3
#define CMID_AUDIO_EFFECT          0xB4
#define CMID_VISUAL_EFFECT         0xB5
#define CMID_PLAY_MODE             0xB6
#define CMID_PLAY_PART             0xB7
#define CMID_PLAY_MEDIA            0xB8
#define CMID_PART_CONFIG           0xB9
#define CMID_CHANNEL_CONFIG        0xBA
#define CMID_RELATIVE_TEMPO        0xBC
#define CMID_RELATIVE_VOL          0xBD
#define CMID_PAUSE                 0xBD
#define CMID_STOP                  0xBE
#define CMID_RESET                 0xBF
#define CMID_CUE_POINT             0xD0
#define CMID_CUE_POINT_START       0x00
#define CMID_CUE_POINT_END         0x01
#define CMID_JUMP                  0xD1
#define CMID_NOP2                  0xDC
#define CMID_MELO_JUMP             0xDD
#define CMID_NOP                   0xDE
#define CMID_END_OF_TRACK          0xDF
#define CMID_PROGRAM_CHANGE        0xE0
#define CMID_BANK_CHANGE           0xE1
#define CMID_CHANNEL_VOLUME        0xE2
#define CMID_CHANNEL_PANPOT        0xE3
#define CMID_CHANNEL_PITCH_BEND    0xE4
#define CMID_CHANNEL_ASSIGN        0xE5
#define CMID_CHANNEL_REL_VOL       0xE6
#define CMID_CHANNEL_PB_RANGE      0xE7
#define CMID_WAVE_CHANNEL_VOLUME   0xE8
#define CMID_WAVE_CHANNEL_BALANCE  0xE9
#define CMID_WAVE_DETUNE           0xEA
#define CMID_TEXT_CONTROL          0xEB
#define CMID_PICTURE_CONTROL       0xEC
#define CMID_LED_CONTROL           0xED
#define CMID_VIB_CONTROL           0xEE
#define CMID_ANIM_CONTROL          0xEF

#define AUDFMT_MAX_CMID_JUMP       4
#define CMID_JUMP_POINT_MASK       0x40
#define CMID_JUMP_POINT_DEST       0x00
#define CMID_JUMP_POINT_JUMP       0x40
#define CMID_JUMP_POINT(v)         ((v & CMID_JUMP_POINT_MASK) == \
                                    CMID_JUMP_POINT_JUMP)
#define CMID_JUMP_ID_MASK          0x30
#define CMID_JUMP_ID_SHIFT         0x04
#define CMID_JUMP_ID(v)            ((v & CMID_JUMP_ID_MASK) >> \
                                    CMID_JUMP_ID_SHIFT)
#define CMID_JUMP_COUNT_MASK       0x0F
#define CMID_JUMP_COUNT(v)         (v & CMID_JUMP_COUNT_MASK)

#define MELO_JUMP_POINT_MASK       0x03
#define MELO_JUMP_POINT_DEST       0x00
#define MELO_JUMP_POINT_JUMP       0x01
#define MELO_JUMP_POINT(v)         ((v & MELO_JUMP_POINT_MASK) == \
                                    MELO_JUMP_POINT_JUMP)
#define MELO_INVALID_JUMP_POINT(v) ((v & MELO_JUMP_POINT_MASK) > \
                                    MELO_JUMP_POINT_JUMP)
#define MELO_JUMP_ID_MASK          0xC0
#define MELO_JUMP_ID_SHIFT         0x06
#define MELO_JUMP_ID(v)            ((v & MELO_JUMP_ID_MASK) >> \
                                    MELO_JUMP_ID_SHIFT)
#define MELO_JUMP_COUNT_MASK       0x3C
#define MELO_JUMP_COUNT_SHIFT      0x02
#define MELO_JUMP_COUNT(v)         ((v & MELO_JUMP_COUNT_MASK) >> \
                                    MELO_JUMP_COUNT_SHIFT)
#define MELO_JUMP_INDEFINITE_VAL   0x00
#define MELO_JUMP_INDEFINITE(v)    (v == MELO_JUMP_INDEFINITE_VAL)

#define CMID_JUMP_INIT_VAL         0x10
#define CMID_JUMP_INIT(v)          (v == CMID_JUMP_INIT_VAL)
#define CMID_JUMP_INDEFINITE_VAL   0x0F
#define CMID_JUMP_INDEFINITE(v)    (v == CMID_JUMP_INDEFINITE_VAL)

#define CMID_MASTER_BIT_MASK       0x7F
#define CMID_PAYLOAD_MASK          0x3F
#define CMID_PROGRAM_MASK          0x0F
#define CMID_BANK_SHIFT            0x40
#define CMID_PROGRAM_ORIG(v)       (v & ~CMID_BANK_SHIFT)
#define CMID_BANK_0                0x7D
#define CMID_BANK_NORMAL           0x79
#define CMID_BANK_DRUM             0x78
#define CMID_BANK_SFX              0x14
#define CMID_BANK_NORMAL_HQ        0x75
#define CMID_BANK_DRUM_HQ          0x74
#define CMID_BANK_DRUM_DLS         0x76
#define CMID_BANK_NORMAL_DLS       0x77
#define CMID_BANK_0_OLD            0x7F
#define CMID_BANK_NORMAL_OLD       0x7B
#define CMID_BANK_DRUM_OLD         0x7A
#define CMID_PB_RANGE_MASK         0x1F
#define CMID_PB_RANGE_MAX          12
#define CMID_PB_RANGE_MIN          0
#define CMID_PB_RANGE(v)           (v & CMID_PB_RANGE_MASK)

#define CMID_MAX_WAV_CHAN          4
#define CMID_DEFAULT_WAV_VOLUME    50
#define MELO_DEFAULT_ADEC_VOL      0x3F
#define CMID_DEFAULT_WAV_PANPOT    32
#define CMID_MAX_WAV_DSP_VOLUME    0x3FFF
#define CMID_MVOL_TO_DSP(v)        (v << 6)
#define CMID_VOL_TO_DSP(v)         ((v << 12) / 25)
#define CMID_PAN_TO_DSP(v)         ((((sint15) (v - 32)) << 6) / 31)

#define CMID_TEXT_ENABLE           0x00
#define CMID_TEXT_DISABLE          0x01
#define CMID_TEXT_CLEAR            0x02
#define CMID_TEXT_ADVANCE_ONE      0x04
#define CMID_TEXT_ADVANCE_TWO      0x05

#define CMID_PICTURE_ENABLE        0x00
#define CMID_PICTURE_DISABLE       0x01
#define CMID_PICTURE_CLEAR_NORMAL  0x02

#define CMID_LED_ENABLE_MASK       0x40
#define CMID_LED_ENABLE_V          0x40
#define CMID_LED_DISABLE_V         0x00
#define CMID_LED_ENABLE(v)         ((v & CMID_LED_ENABLE_MASK) == \
                                    CMID_LED_ENABLE_V)

#define CMID_LED_CTL_MASK          0x3F
#define CMID_LED_CTL(v)            (v & CMID_LED_CTL_MASK)
#define CMID_LED_DARK_BLACK        0x00
#define CMID_LED_DARK_BLUE         0x01
#define CMID_LED_DARK_GREEN        0x02
#define CMID_LED_DARK_CYAN         0x03
#define CMID_LED_DARK_RED          0x04
#define CMID_LED_DARK_PURPLE       0x05
#define CMID_LED_DARK_YELLOW       0x06
#define CMID_LED_DARK_WHITE        0x07
#define CMID_LED_BRIGHT_BLACK      0x08
#define CMID_LED_BRIGHT_BLUE       0x09
#define CMID_LED_BRIGHT_GREEN      0x0A
#define CMID_LED_BRIGHT_CYAN       0x0B
#define CMID_LED_BRIGHT_RED        0x0C
#define CMID_LED_BRIGHT_PURPLE     0x0D
#define CMID_LED_BRIGHT_YELLOW     0x0E
#define CMID_LED_BRIGHT_WHITE      0x0F

#define CMID_VIB_ENABLE_MASK       0x40
#define CMID_VIB_ENABLE_V          0x40
#define CMID_VIB_DISABLE_V         0x00
#define CMID_VIB_ENABLE(v)         ((v & CMID_VIB_ENABLE_MASK) == \
                                    CMID_VIB_ENABLE_V)

#define CMID_VIB_CTL_MASK          0x3F
#define CMID_VIB_CTL(v)            (v & CMID_VIB_CTL_MASK)
#define CMID_VIB_PATTERN_1         0x00
#define CMID_VIB_PATTERN_2         0x01
#define CMID_VIB_PATTERN_3         0x02
#define CMID_VIB_PATTERN_4         0x03
#define CMID_VIB_PATTERN_5         0x04
#define CMID_VIB_PATTERN_6         0x05
#define CMID_VIB_PATTERN_7         0x06
#define CMID_VIB_PATTERN_8         0x07

#define CMID_ANIM_ENABLE           0x00
#define CMID_ANIM_DISABLE          0x01
#define CMID_ANIM_CLEAR            0x02

#define CMID_SAF_OBJBUF_SIZE       65535

#define CMID_EXT_WAVETABLE         0xF0
#define CMID_EXT_WAVE              0xF1
#define CMID_EXT_TEXT              0xF2
#define CMID_EXT_PICTURE           0xF3
#define CMID_EXT_SAF               0xF4

#define CMID_EXT_MAX_BUFFER        0x40
#define CMID_EXT_ID_MASK           0x3F
#define CMID_EXT_ID(v)             (v & CMID_EXT_ID_MASK)

#define CMID_EXT_MODE_MASK         0xC0
#define CMID_EXT_MODE_SHIFT        0x06
#define CMID_EXT_MODE(v)           ((v & CMID_EXT_MODE_MASK) >> \
                                    CMID_EXT_MODE_SHIFT)

#define CMID_EXT_MODE_STORE        0x00
#define CMID_EXT_MODE_SET          0x01
#define CMID_EXT_MODE_RECYCLE      0x02

#define CMID_PCPI_PIC_PIXEL_MASK   0x01
#define CMID_PCPI_PIC_PIXEL(v)     ((v & CMID_PCPI_PIC_PIXEL_MASK) == \
                                    CMID_PCPI_PIC_PIXEL_MASK)

#define CMID_EXT_PIC_FORMAT_MASK   0x3F
#define CMID_EXT_PIC_FORMAT(v)     (v & CMID_EXT_PIC_FORMAT_MASK)

#define CMID_EXT_PIC_FORMAT_BMP    0x01
#define CMID_EXT_PIC_FORMAT_JPEG   0x02
#define CMID_EXT_PIC_FORMAT_PNG    0x03

#define CMID_EXT_PIC_DRAW_NORMAL   0x00
#define CMID_EXT_PIC_DRAW_SLOW     0x01
#define CMID_EXT_PIC_DRAW_MIDDLE   0x02
#define CMID_EXT_PIC_DRAW_FAST     0x03

#define CMID_EXT_PIC_OFFSET_PC_MAX 100
#define CMID_EXT_PIC_OFFSET_PC(v)  (v <= CMID_EXT_PIC_OFFSET_PC_MAX)

#define CMID_EXT_PIC_OFFSET_LT     101
#define CMID_EXT_PIC_OFFSET_CENTER 102
#define CMID_EXT_PIC_OFFSET_RB     103

#define CMID_EXT_TEXT_CMD_MASK     0x40
#define CMID_EXT_TEXT_SET          0x00
#define CMID_EXT_TEXT_APPEND       0x40

#define CMID_EXT_TEXT_Y_POS_MASK   0x07
#define CMID_EXT_TEXT_Y_POS(v)     (v & CMID_EXT_TEXT_Y_POS_MASK)
#define CMID_EXT_TEXT_Y_POS_TOP    0x02
#define CMID_EXT_TEXT_Y_POS_CENTER 0x01
#define CMID_EXT_TEXT_Y_POS_BOTTOM 0x00
#define CMID_EXT_TEXT_X_POS_MASK   0x38
#define CMID_EXT_TEXT_X_POS(v)     (v & CMID_EXT_TEXT_X_POS_MASK)
#define CMID_EXT_TEXT_X_POS_LEFT   0x00
#define CMID_EXT_TEXT_X_POS_CENTER 0x08
#define CMID_EXT_TEXT_X_POS_RIGHT  0x10

#define CMID_EXT_WAVE_FORMAT_MASK  0x3f
#define CMID_EXT_WAVE_FORMAT(v)    (v & CMID_EXT_WAVE_FORMAT_MASK)
#define CMID_EXT_WAVE_WAV_FORMAT   0x00
#define CMID_EXT_WAVE_MP1_FORMAT   0x01
#define CMID_EXT_WAVE_MP2_FORMAT   0x02
#define CMID_EXT_WAVE_MP3_FORMAT   0x03
#define CMID_EXT_WAVE_QCP_FORMAT   0x04
#define CMID_EXT_WAVE_ADPCM_FORMAT 0x05
#define CMID_EXT_WAVE_AAC_FORMAT   0x06

#define CMID_EXT_WAVE_PREV_MASK    0x01
#define CMID_EXT_WAVE_PREV(v)      (v & CMID_EXT_WAVE_PREV_MASK)

/* This macro return the audio decoder mode in audmain_adec_mode_enum_type
** format.
*/
#define CMID_ADEC_MODE(v) (CMID_EXT_WAVE_FORMAT(v) - 4 + AUDMAIN_ADEC_MODE_QCP)

#define CMID_EXT_SAF_FORMAT_MASK   0xE0
#define CMID_EXT_SAF_FORMAT_SHIFT  0x05
#define CMID_EXT_SAF_FORMAT(v)     ((v & CMID_EXT_SAF_FORMAT_MASK) >> \
                                    CMID_EXT_SAF_FORMAT_SHIFT)

#define CMID_EXT_SAF_FORMAT_SAF    0x0
#define CMID_EXT_SAF_FORMAT_SVG    0x3

#define CMID_EXT_SAF_CONT_MASK     0x01
#define CMID_EXT_SAF_CONT(v)       (v & CMID_EXT_SAF_CONT_MASK)

#define CMID_EXT_SAF_CMD_MASK      0x1F
#define CMID_EXT_SAF_CMD(v)        (v & CMID_EXT_SAF_CMD_MASK)

#define CMID_EXT_SAF_CMD_DATA          0x00
#define CMID_EXT_SAF_CMD_FRAME_ID      0x01
#define CMID_EXT_SAF_CMD_FRAME         0x02
#define CMID_EXT_ANIM_CHUNK_CMD_FRAME  0x10

/*Defines the Wav content types in cmid file */
typedef enum {
  AUDFMT_CMID_WAVE_ADPCM,
  AUDFMT_CMID_WAVE_QCELP,
  AUDFMT_CMID_WAVE_AAC   
}audfmt_wave_cnts_enum;

#define ADEC_VOL_TO_DSP(v)           (1 << (v+9))
#define ADEC_CONTENT_VOL(v, m)       ((v*m) >> 13)

#ifdef FEATURE_SMAF
#define SMAF_FILE_MODE_MA3_16        0x32
#define SMAF_FILE_MODE_MA5_16        0x34
#define SMAF_FILE_MODE_32            0x33

#define SMAF_WAVE_FORMAT_MASK        0x70
#define SMAF_WAVE_FORMAT(v)          (v & SMAF_WAVE_FORMAT_MASK)
#define SMAF_WAVE_BASE_BIT_MASK      0x0F
#define SMAF_WAVE_BASE_BIT(v)        (v & SMAF_WAVE_BASE_BIT_MASK)
#define SMAF_WAVE_BASE_BIT_4         0x00
#define SMAF_WAVE_BASE_BIT_8         0x01
#define SMAF_WAVE_BASE_BIT_12        0x02
#define SMAF_WAVE_BASE_BIT_16        0x03
#define SMAF_WAVE_CHANNELS_MASK      0x80
#define SMAF_WAVE_CHANNELS_1         0x00
#define SMAF_WAVE_CHANNELS(v)        (((v & SMAF_WAVE_CHANNELS_MASK) == \
                                       SMAF_WAVE_CHANNELS_1) ? 1:2)

#define SMAF_HP_EXCLUSIVE_MESSAGE    0xFF
#define SMAF_HP_EXCLUSIVE_CMD        0xF0
#define SMAF_HP_NOP_CMD              0x00

#define SMAF_HP_CONTROL_MESSAGE      0x00
#define SMAF_HP_TRACK_END_CMD        0x00
#define SMAF_HP_CHANNEL_SHIFT        0x06
#define SMAF_HP_CHANNEL_NUM(v,m)     ((v >> SMAF_HP_CHANNEL_SHIFT) + (m << 2))
#define SMAF_HP_NORMAL_CMD_MASK      0x30
#define SMAF_HP_NORMAL_CMD(v)        ((v & SMAF_HP_NORMAL_CMD_MASK) == \
                                      SMAF_HP_NORMAL_CMD_MASK)
#define SMAF_HP_NORMAL_CMD_TYPE_MASK 0x0F
#define SMAF_HP_NORMAL_CMD_TYPE(v)   (v & SMAF_HP_NORMAL_CMD_TYPE_MASK)
#define SMAF_HP_PROGRAM_CHANGE_CMD   0x00
#define SMAF_HP_BANK_SELECT_CMD      0x01
#define SMAF_HP_OCTAVE_SHIFT_CMD     0x02
#define SMAF_HP_MODULATION_CMD       0x03
#define SMAF_HP_PITCH_BEND_CMD       0x04
#define SMAF_HP_VELOCITY_CMD         0x06
#define SMAF_HP_VOLUME_CMD           0x07
#define SMAF_HP_PAN_CMD              0x0A
#define SMAF_HP_EXPRESSION_CMD       0x0B
#define SMAF_HP_SHORT_CMD_VALUE_MASK 0x0F
#define SMAF_HP_SHORT_CMD_VALUE(v)   (v & SMAF_HP_SHORT_CMD_VALUE_MASK)
#define SMAF_HP_SHORT_CMD_TYPE_MASK  0x30
#define SMAF_HP_SHORT_CMD_TYPE(v)    (v & SMAF_HP_SHORT_CMD_TYPE_MASK)
#define SMAF_HP_SHORT_EXPRESSION_CMD 0x00
#define SMAF_HP_SHORT_PITCH_BEND_CMD 0x10
#define SMAF_HP_SHORT_MODULATION_CMD 0x20
#define SMAF_MA5_PITCH_BEND_CMD      0x0F

#define SMAF_HP_NOTE_CHANNEL_SHIFT   0x06
#define SMAF_HP_NOTE_CHANNEL(v)      (v >> SMAF_HP_NOTE_CHANNEL_SHIFT)
#define SMAF_HP_NOTE_OCTAVE_SHIFT    0x04
#define SMAF_HP_NOTE_OCTAVE_MASK     0x30
#define SMAF_HP_NOTE_OCTAVE(v)       ((v & SMAF_HP_NOTE_OCTAVE_MASK) >> \
                                      SMAF_HP_NOTE_OCTAVE_SHIFT)
#define SMAF_HP_NOTE_NUMBER_MASK     0x0F
#define SMAF_HP_NOTE_NUMBER(v)       (v & SMAF_HP_NOTE_NUMBER_MASK)
#define SMAF_HP_NOTE_OFFSET          36
#define SMAF_HP_NOTE_DEFAULT_VEL     0x7F

#define SMAF_HP_WAVE_NUMBER_MASK     0x3F
#define SMAF_HP_WAVE_NUMBER(v)       (v & SMAF_HP_WAVE_NUMBER_MASK)
#define SMAF_HP_WAVE_FORMAT_PCM      0x00
#define SMAF_HP_WAVE_FORMAT_YADPCM   0x10

#define SMAF_HP_VIB_MASK1            (0x40)
#define SMAF_HP_VIB_MASK2            (0x04)
#define SMAF_MOBILE_VIB_MASK         (0x20)
#define SMAF_MOBILE_LED_MASK         (0x10)

#define SMAF_MOBILE_DRUM_BANK_VALUE    0x7D
#define SMAF_MOBILE_DRUM_BANK(v)       (v == SMAF_MOBILE_DRUM_BANK_VALUE)
#define SMAF_MOBILE_WAVE_FORMAT_PCM    0x00
/*SMAF_MOBILE_WAVE_FORMAT_OBPCM  is offset binary pcm data in smaf
*/
#define SMAF_MOBILE_WAVE_FORMAT_OBPCM  0x10
#define SMAF_MOBILE_WAVE_FORMAT_YADPCM 0x20
#define SMAF_MOBILE_SHORT_NOTE_CMD     0x80
#define SMAF_CHANNEL_VOL_EXP_CONSTANT  0x64

#define SMAF_GET_SPEC_FROM_OPDA(v)    ( (v&0x30) || (v&0x40) || (v&0x50))
#endif /* FEATURE_SMAF */

/* The following structure is used to manage ADEC channels
*/
typedef struct {
  uint8 enable:1;   /* ADEC channel is enabled                     */
  uint8 reset:1;    /* Reset pending on this ADEC channel          */
  uint8 volume:1;   /* Volume command pending on this ADEC channel */
  uint8 panning:1;  /* Volume command pending on this ADEC channel */
  uint8 channel:2;  /* WA/MFi/SMAF virtual ADEC command            */
  uint8 index:2;    /* MFi index of order of ADEC channels         */
  uint8 wav_id;     /* ID of WAV clip                              */
} audfmt_adec_status_type;
  
/* Audio decoder off commands are entered into the note off command
** processing. The MSBit is used to indicate the note off is for an audio
** decoder and not a normal note off command.
*/
#define AUDFMT_NO_ADEC_CHAN_OFFSET      0x80
#define AUDFMT_NO_ADEC_CHAN(v)          (v + AUDFMT_NO_ADEC_CHAN_OFFSET)
#define AUDFMT_IS_ADEC_NO(v)            ((v >= AUDFMT_NO_ADEC_CHAN_OFFSET) && \
                                         (v < AUDFMT_NO_ADEC_CHAN_OFFSET + 4))
#define AUDFMT_NO_GET_ADEC_CHAN(v)      (v - AUDFMT_NO_ADEC_CHAN_OFFSET)

typedef struct {
  boolean              used;
  uint8                vib_pend;    /* Pending VIBE pattern command        */
  audfmt_vib_ctl_type  *vib_ctl;    /* Pattern to vibrate                  */
  uint32               index;       /* Index of current state in vib_ctl   */
  uint32               time_ms;     /* Time remaining until next vib state */
} audfmt_vib_type;

/* Structure to save the size and location of data found embedded
** in WebAudio files.
*/
typedef struct {
  uint32  location;      /* Location in file where data is found */
  uint32  num_bytes;     /* Number of bytes of the data          */
} audfmt_ext_store_type;

#define AUDFMT_TEXT_STORE_NUM 5

typedef struct {
  snd_cmx_af_text_type    af_text;     /* Text status command               */
  uint32                  num;         /* Text append number parameter      */
  uint32                  length;      /* Length of text                    */
} audfmt_text_store_type;

#ifdef FEATURE_SMAF 
/* Structure to save the led/vibrator note on/off event in SMAF file
*/
#define AUDFMT_SMAF_CB_STORE_NUM 5
typedef struct {
  snd_af_data_cmd_enum              smaf_cmd;  /* LED or Vibrator command   */
  snd_cmx_smaf_led_vib_ctl_type     smaf_ctl;  /* LED or VIB ctl parameter  */
} audfmt_smaf_cb_store_type;
#endif /* FEATURE_SMAF */

/* This structure is used to save the client callback commands that are 
** supposed to be buffered. Only the latest commands are saved.
*/
typedef struct {
  boolean                   used;
  snd_af_data_cmd_enum      pclear;       /* Picture clear command            */
  snd_af_data_cmd_enum      pdisable;     /* Picture disable command          */
  snd_af_data_cmd_enum      pnew;         /* New picture command              */
  snd_cmx_af_pic_new_type   pic_status;   /* New Picture status command       */
  uint32                    pic_num;      /* Picture number parameter         */
  snd_cmx_af_buf_rtn_type   pic_buf;      /* Buffers used for picture data    */
  snd_cmx_af_text_type      text_new;     /* Text status command              */
  uint32                    text_new_num; /* Text new number parameter        */
  snd_cmx_af_buf_rtn_type   text_new_buf; /* Buffers used for new text data   */
  audfmt_text_store_type    text_app[AUDFMT_TEXT_STORE_NUM];
  uint32                    text_app_num; /* Number of text appends           */
  snd_cmx_af_buf_rtn_type   text_app_buf; /* Buffers used for append text data*/
  snd_af_data_cmd_enum      tclear;       /* Text clear command               */
  snd_af_data_cmd_enum      tdisable;     /* Text disable command             */
  uint32                    trew;         /* Text rewind commands             */
  uint32                    tadv;         /* Text advance commands            */
  snd_af_data_cmd_enum      led_cmd;      /* LED command                      */
  snd_cmx_af_led_ctl_type   led_ctl;      /* LED command control              */
  snd_af_data_cmd_enum      saf_clear;    /* SAF clear command                */
  snd_af_data_cmd_enum      saf_disable;  /* SAF disable command              */
  snd_af_data_cmd_enum      saf_cmd;      /* SAF command                      */
  snd_cmx_af_saf_type       saf_status;   /* SAF status command               */
  uint32                    saf_num;      /* SAF number parameter             */
  snd_cmx_af_buf_rtn_type   saf_buf;      /* Buffers used for SAF data        */
  snd_af_data_cmd_enum      svg_cmd;      /* SVG command                      */
  snd_cmx_af_svg_frame_type svg_status;   /* SVG status command               */
  uint32                    svg_num;      /* SVG number parameter             */
  audfmt_ext_store_type     pic_store[CMID_EXT_MAX_BUFFER];
  audfmt_ext_store_type     saf_store[CMID_EXT_MAX_BUFFER];
#ifdef FEATURE_SMAF
  audfmt_smaf_cb_store_type smaf_cb[AUDFMT_SMAF_CB_STORE_NUM];
                                        /* LED/Vibrator control data         */
  uint32                    smaf_cb_num;/* Number of LED/Vibrator callback   */
#endif /* FEATURE_SMAF */
} audfmt_cb_store_type;

#define CMID_NOTE_OFF_MAX_COMMANDS     144

#define CMID_NOTE_OFF_INDEX_CORR(v)  \
      (((v) >= CMID_NOTE_OFF_MAX_COMMANDS) ? \
                    ((v) - CMID_NOTE_OFF_MAX_COMMANDS) : (v))

#define CMID_NOTE_OFF_NEXT_COMMAND(v)  \
  (((v + 1) == (CMID_NOTE_OFF_MAX_COMMANDS)) ? 0 : (v + 1))

#define CMID_NOTE_OFF_PREV_COMMAND(v) \
  ((v == 0) ? (CMID_NOTE_OFF_MAX_COMMANDS - 1) : (v - 1))

typedef struct {
  uint32 gate;
  uint8  channel;
  uint8  note;
  uint8  device_mask;
} audfmt_note_off_type;

typedef struct {
  uint32               num;
  uint32               index;
  audfmt_note_off_type commands[CMID_NOTE_OFF_MAX_COMMANDS];
} audfmt_note_off_buffer_type;

/* This structure is used to keep the jumping state information */
typedef struct {
  uint32  location[AUDFMT_MAX_CMID_TRACKS];    /* Location info */
  uint8   delta_tick[AUDFMT_MAX_CMID_TRACKS];
  boolean do_delta_tick[AUDFMT_MAX_CMID_TRACKS];
  uint16  repeat_num;                          /* Number of times to jump */
  boolean same_time;
  boolean active;
} audfmt_jump_struct_type;

typedef struct {
  uint8   format;
  boolean mode_3d;
  uint16  sampling_rate;
} audfmt_wav_spec_type;

#ifdef FEATURE_SMAF
typedef enum {
  AUDFMT_SMAF_HP_TYPE,
  AUDFMT_SMAF_COMPRESSED_TYPE,
  AUDFMT_SMAF_MOBILE_TYPE,
  AUDFMT_SMAF_PHRASE_TYPE,
  AUDFMT_SMAF_INVALID_TYPE
} audfmt_smaf_type;

typedef enum {
  AUDFMT_SMAF_PHRASE_L1,
  AUDFMT_SMAF_PHRASE_L2,
  AUDFMT_SMAF_PHRASE_UNKNOWN
} audfmt_smaf_phrase_type;

typedef struct {
  uint32 codeword;
  uint8  code_length;
  uint8  value;
} audfmt_huffman_entry_type;

#define AUDFMT_SMAF_DLS_ENTRY_MAX 32
typedef struct {
  uint8 bank_msb;
  uint8 bank_lsb;
  uint8 program;
  uint8 instrument;
} audfmt_smaf_dls_entry_type;

typedef struct {
  uint8  note;
  uint8  range;
  uint8  c_value;
  uint16 b_value;
} audfmt_smaf_ma2_pitch_type;
#endif /* FEATURE_SMAF */
#endif /* FEATURE_WEBAUDIO */

typedef enum {
  AUDFMT_BANK_LEGACY,
  AUDFMT_BANK_NORMAL,
  AUDFMT_BANK_DRUM,
  AUDFMT_BANK_NORMAL_DLS,
  AUDFMT_BANK_DRUM_DLS,
  AUDFMT_BANK_SFX
} audfmt_bank_status_type;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

typedef enum {
  AUDFMT_MIDI_MIDI_TYPE,
  AUDFMT_MIDI_CMID_TYPE,
  AUDFMT_MIDI_WASF_TYPE,
  AUDFMT_MIDI_MELO_TYPE,
  AUDFMT_MIDI_SMAF_TYPE,
  AUDFMT_MIDI_INVALID_TYPE
} audfmt_midi_type;

typedef enum {
  AUDFMT_TRACK_MASK_DO_TIME      = 0x0001,
  AUDFMT_TRACK_MASK_CMD_PEND     = 0x0002,
  AUDFMT_TRACK_MASK_PENDING_BUF  = 0x0004,
  AUDFMT_TRACK_MASK_BUFF_DELAY   = 0x0008,
  AUDFMT_TRACK_MASK_OFFSET_SKIP  = 0x0010,
  AUDFMT_TRACK_MASK_CUEP_DONE    = 0x0020,
  AUDFMT_TRACK_MASK_INIT_DONE    = 0x0040,
  AUDFMT_TRACK_MASK_PCM_TRACK    = 0x0080,
  AUDFMT_TRACK_MASK_DO_MTSP      = 0x0100,
  AUDFMT_TRACK_MASK_MTSP_DONE    = 0x0200,
  AUDFMT_TRACK_MASK_DO_HUFFMAN   = 0x0400,
  AUDFMT_TRACK_MASK_DO_MTSU      = 0x0800,
  AUDFMT_TRACK_MASK_DO_MTSU_CONT = 0x1000,
  AUDFMT_TRACK_MASK_DO_VOIC      = 0x2000,
  AUDFMT_TRACK_MASK_DO_VOIC_CONT = 0x4000,
  AUDFMT_TRACK_MASK_DO_MSPI      = 0x8000
} audfmt_track_mask_type;

/* This structure contains all the information on a particular track
** that is being parsed.
*/
typedef struct {
  snd_af_access_type
           *audfmt_handle;      /* Info to callback client                   */
  uint32   track_status;        /* Status mask for track                     */
  uint32   index;               /* Index into buffer being processed         */
  uint8    *track_buf;          /* Current data buffer being used            */
  uint32   buf_size;            /* Size of buffer in track_buf               */
  uint8    *track_buf_reserve;  /* Reserve data buffer                       */
  uint32   buf_reserve_size;    /* Size of buffer in track_buf_reserve       */
  uint32   track_end;           /* Position of end of track                  */
  uint32   file_location;       /* Location in file being processed          */
  uint32   current_location;    /* Last point in file read for this track    */
  uint32   delta_tick;          /* Time, in ticks, until next track event    */
  uint8    running_status;      /* Running status of this track              */
  uint8    compressed_bits;     /* Compressed bits, for compressed SMAF      */
  uint8    num_compressed_bits; /* Number bits remaining in compressed_bits  */
  uint32   skip_buf_count;      /* Count of data buffers to skip             */
#ifdef FEATURE_SMAF
  uint32   start_offset;        /* Offset into track to start playback       */
  uint32   stop_offset;         /* Offset into track to stop playback        */
  uint32   current_offset;      /* Current offset into track                 */
#endif /* FEATURE_SMAF */
} audfmt_track_type;

typedef enum {
  AUDFMT_MASK_ENABLED           = 0x00000001,
  AUDFMT_MASK_CALC_TIME         = 0x00000002,
  AUDFMT_MASK_SEEK_ACTIVE       = 0x00000004,
  AUDFMT_MASK_ERROR             = 0x00000008,
  AUDFMT_MASK_INIT_DONE         = 0x00000010,
  AUDFMT_MASK_STARTED_AGE       = 0x00000020,
  AUDFMT_MASK_PAUSED            = 0x00000040,
  AUDFMT_MASK_SEEK_PAUSED       = 0x00000080,
  AUDFMT_MASK_PARSING_HEADER    = 0x00000100,
  AUDFMT_MASK_PARSE_PENDING     = 0x00000200,
  AUDFMT_MASK_PARSE_DONE        = 0x00000400,
  AUDFMT_MASK_PARSE_DELAYED     = 0x00000800,
  AUDFMT_MASK_PENDING_BUF       = 0x00001000,
  AUDFMT_MASK_DONE_PENDING      = 0x00002000,
  AUDFMT_MASK_GET_SPEC          = 0x00004000,
  AUDFMT_MASK_SILENCE_SKIP      = 0x00008000,
  AUDFMT_MASK_FIND_POLY_CMD     = 0x00010000,
  AUDFMT_MASK_REPEAT_PENDING    = 0x00020000,
  AUDFMT_MASK_FIND_CUEP_END     = 0x00040000,
  AUDFMT_MASK_SAF_OBJECTS_DONE  = 0x00080000,
  AUDFMT_MASK_PRO_FOUND         = 0x00100000,
  AUDFMT_MASK_INITIAL_SKIP      = 0x00200000,
  AUDFMT_MASK_WAIT_RESUME       = 0x00400000,
  AUDFMT_MASK_RESUME_PENDING    = 0x00800000,
  AUDFMT_MASK_REPEAT_ENABLE     = 0x01000000,
  AUDFMT_MASK_RESTARTING        = 0x02000000,
  AUDFMT_MASK_MULTISEQ_MODE     = 0x04000000,
  AUDFMT_MASK_CB_STORE_DONE     = 0x08000000,
  AUDFMT_MASK_CUEP_PLAY         = 0x10000000,
  AUDFMT_MASK_OFFSET_SKIP       = 0x20000000,
  AUDFMT_MASK_MSPI_FOUND        = 0x40000000,
  AUDFMT_MASK_DLS_ENABLED       = 0x80000000
} audfmt_status_mask_type;

typedef enum {
  AUDFMT_MASK_PAUSE_CMD_PENDING   = 0x00000001,
  AUDFMT_MASK_VOL_CMD_PENDING     = 0x00000002,
  AUDFMT_MASK_JUMP_PENDING        = 0x00000004,
  AUDFMT_MASK_CUEP_PENDING        = 0x00000008,
  AUDFMT_MASK_PAUSE_PENDING       = 0x00000010,
  AUDFMT_MASK_NEXT_BUFFER         = 0x00000020,
  AUDFMT_MASK_PCPI_ENABLED        = 0x00000080,
  AUDFMT_MASK_GRAPH_CHUNK_ENABLED = 0x00000100,
  AUDFMT_MASK_SMAF_MA2            = 0x00000200,
  AUDFMT_MASK_RESET_WAV_INIT      = 0x00000400,
  AUDFMT_MASK_PARSING_OPDA        = 0x00000800,
  AUDFMT_MASK_OPDA_DONE           = 0x00001000,
  AUDFMT_MASK_VIB_EN_PENDING      = 0x00002000,
  AUDFMT_MASK_VIB_DIS_PENDING     = 0x00004000,
  AUDFMT_MASK_SMAF_FM16           = 0x00008000,
  AUDFMT_MASK_SMAF_DEFAULT_HQ     = 0x00010000,
  AUDFMT_MASK_SMAF_PACKED_DLS_WF  = 0x00020000,
  AUDFMT_MASK_PARSING_CNTI        = 0x00040000,
  AUDFMT_MASK_CNTI_DONE           = 0x00080000,
  AUDFMT_MASK_PARSING_DCH         = 0x00100000,
  AUDFMT_MASK_PARSING_SUBCHUNKS   = 0x20000000,
  AUDFMT_MASK_PARSING_CHUNKS      = 0x40000000
} audfmt_status_mask2_type;

#ifdef FEATURE_SMAF
typedef enum {
  AUDFMT_MASK_SMAF_MA3         = 0x00000001,
  AUDFMT_MASK_SMAF_MA5         = 0x00000002,
  AUDFMT_MASK_SMAF_ADPCM_PAIR  = 0x00000008,
 }audfmt_status_mask3_type;



struct smaf_channel_pair{
  uint8   wavid1;    
  uint8   wavid2; 
  boolean pair_status;
  struct smaf_channel_pair *next_channel_pair;  
};

typedef struct smaf_channel_pair audfmt_smaf_channel_pair;

 struct smaf_channel_pan{
  uint8  wavid;
  sint15 pan_val;
  uint32 seq;
  struct smaf_channel_pan *next_channel_pan;
};
 
typedef struct smaf_channel_pan audfmt_smaf_channel_pan;


 struct smaf_cc10_channel_pan{
  uint8   midi_channel;
  sint15  pan_val;
  uint32  seq;
  struct smaf_cc10_channel_pan *next_cc10_channel_pan;
};
 
typedef struct smaf_cc10_channel_pan audfmt_smaf_channel_cc10_pan;


#endif

#define AUDFMT_CMD_BUF_MAX_WORDS 128

typedef struct {
  uint16 num_words;          /* Number of total words in command buffer */
  uint16 num_commands;       /* Number of commands in command buffer    */
  uint16 command_buf[AUDFMT_CMD_BUF_MAX_WORDS];
                             /* Buffer of commands                      */
} audfmt_cmd_buf_type;

typedef struct{
  uint8  code_type;      /* Character encoding type */
  char  title[30];     /* Song title */
  char  artist[30];    /* Artist Name */
  char  copyright[30]; /* Copyright */
  uint8 flag;           /* Indicates if any meta data is read */
} audfmt_smaf_audio_spec_type;

/* This structure contains all the information on all the tracks of the
** current file being parsed
*/
typedef struct {
  snd_status_type         error_status; /* Indicate the parse error */
  audfmt_midi_type        type;
  uint8                   sequence;
  uint8                   hybrid_mode;
  uint8                   drum_program;
  snd_af_access_type      *handle;   /* Info to callback client     */
  uint32                  midi_status;
  uint32                  midi_status2;
  #ifdef FEATURE_SMAF
  uint32                  midi_status3;
  #endif
  uint32                  num_tracks;
  uint32                  failed_cmd_cnt;
  uint32                  file_offset;
  uint32                  num_read;
  uint32                  current_header_track;
  uint32                  ppqn_10;
  uint32                  tempo;
  uint32                  tempo_pending;
  uint32                  tempo_factor;
  uint32                  tempo_factor_pending;
  uint32                  calc_factor;
  uint32                  calc_ms_of;
  uint32                  calc_tick_of;
  uint32                  elapsed_ms;
  uint32                  elapsed_tick;
  uint32                  accumulated_ms;
  uint32                  play_time_ms;
  uint32                  play_tick;
  uint32                  seek_ms;
  uint32                  seek_tick;
  uint32                  seek_elapsed_tick;
  uint32                  delta_tick;
  uint32                  delta_time;
  uint32                  repeat_ms;
  uint32                  interval_ms;
  uint32                  repeat_orig;
  uint32                  repeat_count;
  boolean                 channel_active[AUDFMT_MAX_CHANNELS];
  audfmt_track_type       tracks[AUDFMT_MAX_MIDI_TRACKS];
  audfmt_cmd_buf_type     cmd_buf;

  uint8                   tune_factor;
  uint8                   pan_factor;
  uint16                  vol_factor;

  audfmt_bank_status_type bank_status[AUDFMT_MAX_CHANNELS];
  uint8                   bank_msb[AUDFMT_MAX_CHANNELS];
  uint8                   bank_lsb[AUDFMT_MAX_CHANNELS];

#ifdef FEATURE_WEBAUDIO
  uint16                       version;
  uint16                       note_size;
  uint16                       exsa_size;
  uint16                       exsb_size;
  uint16                       exsc_size;
  uint16                       exsn_size;
  uint32                       track_start[AUDFMT_MAX_CMID_TRACKS + 1];
  uint32                       cuep_offset[AUDFMT_MAX_CMID_TRACKS];
  boolean                      cuep_do_tick[AUDFMT_MAX_CMID_TRACKS];
  uint32                       cuep_tick[AUDFMT_MAX_CMID_TRACKS];
  uint32                       cuep_repeat;
  uint32                       cuep_repeat_orig;
  uint32                       contents;
  uint32                       file_end;
  uint8                        channels[AUDFMT_MAX_CHANNELS];
  boolean                      channel_index_active[AUDFMT_MAX_CHANNELS];
  uint8                        chan_vol[AUDFMT_MAX_CHANNELS];
  uint8                        program[AUDFMT_MAX_CHANNELS];
  uint8                        code;
  uint8                        cmid_tempo;
  uint8                        cmid_timebase;
  uint8                        cmid_master_vol;

  audfmt_note_off_buffer_type  note_off_buffer;
  audfmt_jump_struct_type      jump_info[AUDFMT_MAX_CMID_JUMP];

  audfmt_adec_status_type      adec_status[4];
  audfmt_ext_store_type        wav_store[CMID_EXT_MAX_BUFFER];
  audfmt_wav_spec_type         wav_spec[CMID_EXT_MAX_BUFFER];
  uint16                       wav_chan_vol[CMID_MAX_WAV_CHAN];
  sint15                       wav_chan_pan[CMID_MAX_WAV_CHAN];
  audfmt_cb_store_type         *cb_store;
  audfmt_vib_type              *vib_ctl;

  uint16                       adec_chan_vol[CMID_MAX_WAV_CHAN];
  audfmt_wave_cnts_enum        cmid_wave_type;
  uint32                       saf_objbuf_size;

  uint8                        anim_format_type;
  uint8                        anim_current_id;

#ifdef FEATURE_MFI
#error code not present
#endif /* FEATURE_MFI */

#ifdef FEATURE_SMAF
// optimize so that webaudio and smaf variables are union'ed
  uint32                       smaf_start_tick;
  uint32                       smaf_end_tick;
  uint32                       smaf_timebase_d;
  uint32                       smaf_timebase_g;

  audfmt_smaf_type             smaf_type;
  audfmt_smaf_phrase_type      smaf_phrase_type;
  uint8                        smaf_content_type;
  uint32                       smaf_dch_size;
  audfmt_smaf_audio_spec_type  smaf_audio_spec;
  audfmt_smaf_channel_pair     *channel_pair_ptr;
  audfmt_smaf_channel_pan      *channel_pan_ptr;
  audfmt_smaf_channel_cc10_pan *cc10_channel_pan_ptr;
  /* Only for mobile format */
  uint8                        smaf_start_num_comp_bits;
  uint8                        smaf_start_comp_bits;

  /* Only for HP format */
  sint7                        smaf_octave_shift[AUDFMT_MAX_CHANNELS];
  uint8                        smaf_program_change[16];
  uint8                        smaf_velocity[16];

  /* Only for compressed mobile format */
  audfmt_huffman_entry_type    *smaf_huffman_table_entry;
  uint32                       smaf_current_code;
  uint32                       smaf_code_length;
  audfmt_huffman_entry_type    smaf_huffman_table[256];

  /* LED and Vibrator controls  */
  uint16                       smaf_vib_ctl;
  uint16                       smaf_led_ctl;

  /* Required for LED/VIB event synchronization */
  uint32                       smaf_vib_count;
  uint32                       smaf_led_count;

  /* SMAF FM DLS instrument mapping */
  audfmt_smaf_dls_entry_type   smaf_fm_dls[AUDFMT_SMAF_DLS_ENTRY_MAX];
  uint32                       smaf_fm_dls_index;
  audfmt_smaf_dls_entry_type   smaf_fm_drum[AUDFMT_SMAF_DLS_ENTRY_MAX];
  uint32                       smaf_fm_drum_index;

  audfmt_smaf_ma2_pitch_type   smaf_ma2_pitch[AUDFMT_MAX_CHANNELS];
#endif /* FEATURE_SMAF */

#ifdef FEATURE_DLS
  snd_af_access_type           dls_handle;
#endif /* FEATURE_DLS */
#endif /* FEATURE_WEBAUDIO */

#ifdef FEATURE_AUDIO_CMX_5P0
  boolean midi_pitch_is_changed[AUDFMT_MAX_CHANNELS];
  uint16 midi_ch_pitch_param[AUDFMT_MAX_CHANNELS];

  boolean midi_vol_is_changed[AUDFMT_MAX_CHANNELS];
  uint16 midi_ch_vol_param[AUDFMT_MAX_CHANNELS];

  boolean midi_pan_is_changed[AUDFMT_MAX_CHANNELS];
  uint16 midi_ch_pan_param[AUDFMT_MAX_CHANNELS];
#endif /* FEATURE_AUDIO_CMX_5P0 */ 
} audfmt_parse_type;

#ifndef FEATURE_AUDIO_CMX_5P0
typedef struct {
  uint8 max_key;
  uint8 waveform_id;
  uint8 type;
  uint8 course;
  uint8 tvf_fc;
  uint8 flw_key;
  uint8 feg_attack_time;
  uint8 tvf_resonance;
  uint8 feg_release_time;
  uint8 feg_decay_time;
  uint8 feg_sustain_level;
  uint8 feg_initial_level;
  uint8 feg_depth;
  uint8 feg_release_level;
  uint8 aeg_decay_time;
  uint8 aeg_attack_time;
  uint8 aeg_initial_level;
  uint8 aeg_release_time;
  uint8 aeg_depth;
  uint8 aeg_sustain_level;
  uint8 peg_decay_time;
  uint8 peg_attack_time;
  uint8 peg_initial_level;
  uint8 peg_release_time;
  uint8 peg_release_level;
  uint8 peg_sustain_level;
  uint8 lfo_freq;
  uint8 peg_depth;
  uint8 lfo_feg_depth;
  uint8 lfo_peg_depth;
  uint8 lfo_delay;
  uint8 lfo_aeg_depth;
} audfmt_prog_def_struct_type;
#endif /* not FEATURE_AUDIO_CMX_5P0 */

typedef enum {
  AUDFMT_PROG_DEF_FREE,
  AUDFMT_PROG_DEF_RESERVED_SEQ1,
  AUDFMT_PROG_DEF_IN_USE_SEQ1,
  AUDFMT_PROG_DEF_RESERVED_SEQ2,
  AUDFMT_PROG_DEF_IN_USE_SEQ2,
  AUDFMT_PROG_DEF_RESERVED_SEQ3,
  AUDFMT_PROG_DEF_IN_USE_SEQ3,
  AUDFMT_PROG_DEF_RESERVED_SEQ4,
  AUDFMT_PROG_DEF_IN_USE_SEQ4,
  AUDFMT_PROG_DEF_RESERVED_ALL,
  AUDFMT_PROG_DEF_IN_USE_ALL
} audfmt_prog_def_in_use_type;

typedef struct {
  audfmt_prog_def_in_use_type  in_use_flag;
  uint8                        bank_msb;
  uint8                        bank_lsb;
  uint8                        program;
  uint8                        wave_id;
  uint8                        wave_type;
#ifdef FEATURE_AUDIO_CMX_5P0
  uint8                        total_level;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  uint16                       sampling_freq;
  uint16                       loop_start;
  uint16                       loop_end;
} audfmt_param_def_type;

typedef struct {
  audfmt_prog_def_in_use_type in_use_flag;
  uint8                       wave_entry;
  uint8                       inst_entry;
  uint8                       wave_id;
  boolean                     wave_encoded;
  boolean                     drum_tune;
  uint32                      location;
  uint32                      size;
  uint8                       *encoded_waveform;
  uint8                       *decoded_waveform;

  uint16                       sampling_freq;
  uint16                       loop_start;
  uint16                       loop_end;
} audfmt_waveform_param_type;

#ifdef FEATURE_AUDIO_CMX_5P0
typedef struct {
  uint8  *wave_addr;
  uint32 gain;
  sint15 fine_tune;
  uint8  loop_type_wave_depth;
  uint8  unity_note;
  uint16 loop_length;
  uint16 loop_start;
  uint16 wave_length;
  uint16 sampling_rate;
} audfmt_hq_waveform_info_type;

typedef struct {
  uint16 min_key;
  uint16 max_key;
  audfmt_hq_waveform_info_type *wf_info_ptr;
  uint32 mod_lfo_freq;
  uint32 mod_lfo_delay;
  uint32 vib_lfo_freq;
  uint32 vib_lfo_delay;
  uint32 aeg_delay_time;
  uint32 aeg_attack_time;
  uint32 aeg_hold_time;
  uint32 aeg_decay_time;
  uint32 aeg_sustain_level;
  uint32 aeg_release_time;
  uint32 aeg_shutdown_time;
  uint32 aeg_velocity_to_attack;
  uint32 aeg_key_to_decay;
  uint32 aeg_key_to_hold;
  uint32 peg_delay_time;
  uint32 peg_attack_time;
  uint32 peg_hold_time;
  uint32 peg_decay_time;
  uint32 peg_sustain_level;
  uint32 peg_release_time;
  uint32 peg_velocity_to_attack;
  uint32 peg_key_to_decay;
  uint32 peg_key_to_hold;
  uint32 note_to_key;
  uint32 rpn2_to_key;
  uint32 tvf_fc;
  uint32 tvf_resonance;
  uint32 mod_lfo_to_fc;
  uint32 mod_lfo_cc1_to_fc;
  uint32 mod_lfo_ch_pressure_to_fc;
  uint32 peg_to_fc;
  uint32 velocity_to_fc;
  uint32 key_to_fc;
  uint32 mod_lfo_to_gain;
  uint32 mod_lfo_cc1_to_gain;
  uint32 mod_lfo_ch_pressure_to_gain;
  uint32 velocity_to_gain;
  uint32 cc7_to_gain;
  uint32 cc11_to_gain;
  uint32 fine_tune;
  uint32 pitch_wheel_rpn0_to_pitch;
  uint32 key_to_pitch;
  uint32 rpn1_to_pitch;
  uint32 vib_lfo_to_pitch;
  uint32 vib_lfo_cc1_to_pitch;
  uint32 vib_lfo_ch_pressure_to_pitch;
  uint32 mod_lfo_to_pitch;
  uint32 mod_lfo_cc1_to_pitch;
  uint32 mod_lfo_ch_pressure_to_pitch;
  uint32 peg_to_pitch;
  uint32 pan;
  uint32 cc10_to_pan;
  uint32 cc91_to_reverb_send;
  uint32 reverb_send;
  uint32 cc93_to_chorus_send;
  uint32 chorus_send;
} audfmt_hq_prog_def_struct_type;
typedef struct {
  uint32                         num_regions;
  audfmt_hq_prog_def_struct_type *addr;
} audfmt_inst_rec_type;

typedef struct {
  audfmt_param_def_type          param_def[128];
  audfmt_inst_rec_type           prog_rec[128];
  audfmt_hq_prog_def_struct_type prog_def[128];

  audfmt_waveform_param_type     wave_param[128];
  audfmt_hq_waveform_info_type   wave_info[128];
} audfmt_dls_struct_type;

#else /* FEATURE_AUDIO_CMX_5P0 */

typedef struct {
  uint16 wave_loc;
  uint16 reserved;
  uint16 wave_addr_lsw;
  uint16 wave_addr_msw;
  uint16 wave_type;
  uint16 loop_start;
  uint16 wave_length;
  uint16 loop_length;
  uint16 wave_tune_msw;
  uint16 wave_tune_lsw;
} audfmt_waveform_info_type;

typedef struct {
  audfmt_param_def_type       param_def[128];
  audfmt_prog_def_struct_type prog_def[128];

  audfmt_waveform_param_type  wave_param[128];
  audfmt_waveform_info_type   wave_info[128];
} audfmt_dls_struct_type;
#endif /* FEATURE_AUDIO_CMX_5P0 */

#ifdef FEATURE_DLS
typedef enum {
  AUDFMT_PROG_DEF_WAVE_DONE_FLAG  = 0x01,
  AUDFMT_PROG_DEF_ART_DONE_FLAG   = 0x02,
  AUDFMT_PROG_DEF_WSMP_DONE_FLAG  = 0x04,
  AUDFMT_PROG_DEF_WLNK_DONE_FLAG  = 0x08,
  AUDFMT_PROG_DEF_RGNH_DONE_FLAG  = 0x10
} audfmt_prog_def_mask_type;

typedef struct {
  uint32 prog_mask;
  uint16 unity_note;
  sint15 fine_tune;
  uint16 loop_start;
  uint16 loop_length;
#ifdef FEATURE_AUDIO_CMX_5P0
  uint32 loop_type;
  uint32 gain;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  uint32 wave_id;
} audfmt_prog_def_ctl_type;

#ifndef FEATURE_AUDIO_CMX_5P0
typedef struct {
  uint8 max_key;
  uint8 waveform_id;
  uint8 detune;
  uint8 course;
  uint8 flw_key;
  uint8 vel_offset;
  uint8 lfo_mod_sw;
  uint8 key_flw_type;
  uint8 lfo1_freq;
  uint8 lfo1_type;
  uint8 lfo1_feg_depth;
  uint8 lfo1_peg_depth;
  uint8 lfo1_delay;
  uint8 lfo1_aeg_depth;
  uint8 lfo2_freq;
  uint8 lfo2_type;
  uint8 lfo2_feg_depth;
  uint8 lfo2_peg_depth;
  uint8 lfo2_delay;
  uint8 lfo2_aeg_depth;
  uint8 tvf_fc;
  uint8 tvf_type;
  uint8 tvf_flw_key;
  uint8 tvf_resonance;
  uint8 peg_attack1_time;
  uint8 tvf_key_flw_type;
  uint8 peg_release_time;
  uint8 peg_decay2_time;
  uint8 peg_attack1_level;
  uint8 peg_initial_level;
  uint8 peg_release_level;
  uint8 peg_sustain_level;
  uint8 feg_attack1_time;
  uint8 peg_depth;
  uint8 feg_release_time;
  uint8 feg_decay2_time;
  uint8 feg_attack1_level;
  uint8 feg_initial_level;
  uint8 feg_release_level;
  uint8 feg_sustain_level;
  uint8 aeg_attack1_time;
  uint8 feg_depth;
  uint8 aeg_release_time;
  uint8 aeg_decay2_time;
  uint8 aeg_attack1_level;
  uint8 aeg_initial_level;
  uint8 aeg_depth;
  uint8 aeg_sustain_level;
} audfmt_hq_prog_def_struct_type;

typedef struct {
  uint16 wave_addr_lsw;
  uint16 wave_addr_msw;
  uint16 loop_length1;
  uint16 loop_start1;
  uint16 loop_length2;
  uint16 loop_start2;
  uint16 reserved;
  uint16 wave_length;
  uint16 wave_tune_msw;
  uint16 wave_tune_lsw;
} audfmt_hq_waveform_info_type;

#endif /* not FEATURE_AUDIO_CMX_5P0 */

typedef struct {
  audfmt_prog_def_in_use_type     in_use_flag;
  boolean                         drum;
  uint8                           bank_msb;
  uint8                           bank_lsb;
  uint8                           program;
  uint8                           drum_note;
  uint8                           num_regions;
  uint8                           current_region;
#ifndef FEATURE_AUDIO_CMX_5P0
  boolean                         peg_depth_art_flag;
  uint32                          peg_depth_art_value;
#endif /* not FEATURE_AUDIO_CMX_5P0 */
  uint8                           *buffer;
  audfmt_prog_def_ctl_type        *prog_ctl;
  audfmt_hq_prog_def_struct_type  *prog_def;
  void                            *next_inst_def;
} audfmt_inst_def_type;

/* This mask is used to indicate status information regarding wave
** list parsing.
*/
typedef enum {
  AUDFMT_WAVE_CTL_WSMP_DONE_FLAG  = 0x01, /* wsmp chunk has been processed  */
  AUDFMT_WAVE_CTL_FMT_DONE_FLAG   = 0x02, /* fmt chunk has been processed   */
  AUDFMT_WAVE_CTL_DATA_PEND_FLAG  = 0x04, /* data chunk has been found      */
  AUDFMT_WAVE_CTL_DATA_DONE_FLAG  = 0x08, /* data chunk has been processed  */
  AUDFMT_WAVE_CTL_DRUM_WAVEFORM   = 0x10  /* Indicates waveform is for drum */
} audfmt_wave_ctl_mask_type;

typedef struct {
  uint16                       wave_mask;
  audfmt_prog_def_in_use_type  in_use_flag;
#ifdef FEATURE_AUDIO_CMX_5P0
  uint32                       gain;
  uint32                       loop_type;
#else
  uint8                        aeg_depth;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  uint16                       unity_note;
  sint15                       fine_tune;
  uint32                       loop_start;
  uint32                       loop_length;
  uint32                       sampling_rate;
  uint16                       bits_per_sample;
} audfmt_wave_ctl_type;

#define AUDFMT_NUM_INST_DEF 128
#define AUDFMT_NUM_WAVE_DEF 128

#ifdef FEATURE_AUDIO_CMX_5P0
typedef struct {
  audfmt_inst_def_type         inst_def[AUDFMT_NUM_INST_DEF];
  audfmt_inst_rec_type         inst_rec[AUDFMT_NUM_INST_DEF];
  audfmt_inst_def_type         drum_def[AUDFMT_NUM_INST_DEF];
  audfmt_inst_rec_type         drum_rec[AUDFMT_NUM_INST_DEF];
  audfmt_wave_ctl_type         wave_ctl[AUDFMT_NUM_WAVE_DEF];
  audfmt_hq_waveform_info_type wave_info[AUDFMT_NUM_WAVE_DEF];
} audfmt_hq_dls_struct_type;
#else
typedef struct {
  audfmt_inst_def_type         inst_def[AUDFMT_NUM_INST_DEF];
  uint32                       inst_addr[AUDFMT_NUM_INST_DEF];
  audfmt_inst_def_type         drum_def[AUDFMT_NUM_INST_DEF];
  uint32                       drum_addr[AUDFMT_NUM_INST_DEF];
  audfmt_wave_ctl_type         wave_ctl[AUDFMT_NUM_WAVE_DEF];
  audfmt_hq_waveform_info_type wave_info[AUDFMT_NUM_WAVE_DEF];
} audfmt_hq_dls_struct_type;
#endif /* FEATURE_AUDIO_CMX_5P0 */

typedef enum {
  AUDFMT_DLS_LIST_LINS,
  AUDFMT_DLS_LIST_WVPL,
  AUDFMT_DLS_LIST_INS,
  AUDFMT_DLS_LIST_LRGN,
  AUDFMT_DLS_LIST_LART,
  AUDFMT_DLS_LIST_RGN,
  AUDFMT_DLS_LIST_UNRECOGNIZED,
  AUDFMT_DLS_CHUNK_PTBL,
  AUDFMT_DLS_CHUNK_INSH,
  AUDFMT_DLS_CHUNK_RGNH,
  AUDFMT_DLS_CHUNK_WSMP,
  AUDFMT_DLS_CHUNK_WLNK,
  AUDFMT_DLS_CHUNK_ART1,
  AUDFMT_DLS_CHUNK_ART2,
  AUDFMT_DLS_CHUNK_FMT,
  AUDFMT_DLS_CHUNK_DATA,
  AUDFMT_DLS_CHUNK_UNRECOGNIZED,
  AUDFMT_DLS_INSUFFICIENT_DATA
} audfmt_dls_chunk_enum_type;

typedef enum {
  AUDFMT_DLS_STATE_INIT,
  AUDFMT_DLS_STATE_RIFF,
  AUDFMT_DLS_STATE_LIST_LINS,
  AUDFMT_DLS_STATE_LIST_INS,
  AUDFMT_DLS_STATE_LRGN_INIT,
  AUDFMT_DLS_STATE_LIST_LRGN,
  AUDFMT_DLS_STATE_LIST_LART,
  AUDFMT_DLS_STATE_LIST_RGN,
  AUDFMT_DLS_STATE_WAVE_INIT,
  AUDFMT_DLS_STATE_LIST_WAVE,
  AUDFMT_DLS_STATE_WAVE_DATA,
  AUDFMT_DLS_STATE_PTBL,
  AUDFMT_DLS_STATE_ERROR,
  AUDFMT_DLS_STATE_DONE,
  AUDFMT_DLS_STATE_RELEASE,
  AUDFMT_DLS_STATE_INVALID
} audfmt_dls_state_enum_type;

typedef struct {
  audfmt_dls_state_enum_type state;
  audfmt_dls_state_enum_type prev_state;
  audfmt_dls_chunk_enum_type data_req;
  uint8                      sequence;
  boolean                    wave_param_enabled;
  snd_af_access_type         *handle;
  uint32                     riff_length;
  uint32                     lins_length;
  uint32                     ins_length;
  uint32                     lrgn_length;
  uint32                     lart_length;
  uint32                     rgn_length;
  uint32                     wave_length;

  audfmt_hq_prog_def_struct_type inst_prog_def;
#ifndef FEATURE_AUDIO_CMX_5P0
  boolean                        peg_depth_art_flag;
  uint32                         peg_depth_art_value;
#endif /* not  FEATURE_AUDIO_CMX_5P0 */
  audfmt_inst_def_type           *start_inst;
  audfmt_inst_def_type           *inst_def_ptr;

  uint32  wvpl_position;
  uint32  ptbl_position;
  uint32  ptbl_length;

  uint32  wave_index;
  uint32  wave_position;
  uint32  wave_data_position;
  uint32  wave_data_length;
#ifdef FEATURE_AUDIO_CMX_5P0
  uint32  gain;
#endif /* FEATURE_AUDIO_CMX_5P0 */
  uint16  unity_note;
  sint15  fine_tune;
  uint32  loop_start;
  uint32  loop_length;
#ifdef FEATURE_AUDIO_CMX_5P0
  uint32  loop_type;
#endif /* FEATURE_AUDIO_CMX_5P0 */

  uint32  file_offset;
  uint32  file_position;
  uint32  current_position;
  uint32  index;
  uint8   *buf1;
  uint32  length_buf1;
  uint8   *buf2;
  uint32  length_buf2;
} audfmt_dls_state_type;

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_process_dls

DESCRIPTION
  This function is the main processing state machine for DLS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audfmt_process_dls (audfmt_dls_state_type *state);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_load_offset_dls

DESCRIPTION
  This function loads a DLS file from an offset position within the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_load_offset_dls (
  snd_af_access_type *handle,
  uint32             start_position,
  uint32             sequence
);

/* <EJECT> */
/*===========================================================================

FUNCTION audfmt_unload_offset_dls

DESCRIPTION
  This function unloads the DLS resources for the indicated sequence.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audfmt_unload_offset_dls (
  snd_af_access_type *handle,
  uint32             sequence
);
#endif /* FEATURE_DLS */

#ifdef FEATURE_SMAF
/*===========================================================================

FUNCTION audfmt_calculate_smaf_adec_chn_volume

DESCRIPTION
  This function calculates the ADEC Channel volume for SMAF File.

INPUTS
  smafgaintable/master volume in file/key velocity

RETURN VALUE
	Adec Channel volume is returned.

SIDE EFFECTS
  None

===========================================================================*/
uint16 audfmt_set_smaf_chn_volume(audfmt_parse_type* midi_ctl, uint16 keyvelocity);
/*===========================================================================

FUNCTION smaf_clean_heap

DESCRIPTION
  This function clears global variables.

INPUTS
audfmt_parse_type

RETURN VALUE
none

SIDE EFFECTS
  None

===========================================================================*/
void smaf_clean_heap(audfmt_parse_type* midi_ctl);
/*===========================================================================

FUNCTION audfmt_get_smaf_chn_pan

DESCRIPTION
  This function returns the Pan value
INPUTS

RETURN VALUE
pan value for ADEC channel

SIDE EFFECTS
  None

===========================================================================*/
sint15 audfmt_get_smaf_chn_pan(audfmt_parse_type *midi_ctl,uint8 noteno,uint8 midi_channel);

/* <EJECT> */
/*===========================================================================

FUNCTION smaf_do_cnti

DESCRIPTION
  This function processes CNTI chunk in a SMAF file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates audio spec

===========================================================================*/
static audfmt_status_type smaf_do_cnti (
  audfmt_parse_type *midi_ctl
);
#endif /*FEATURE_SMAF*/
#endif /* FEATURE_MIDI */
#endif /* AUDFMTI_H */

