#ifndef QCPSUP_H
#define QCPSUP_H
/*===========================================================================

         Q C P   A U D I O   F O R M A T S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for other
  tasks to interface with the QCP audio formats module.

Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/qcpsup.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/08    sj     Added .awb file format support
10/26/07    hs     Added EVW file format support
04/07/07    pl     Added qcpsup_do_amr_head() function definition.
03/21/07    at     Added EVRC-B file format support.
06/01/06    sg     Fixed featurization so that G711 content can be played
                   when the build does not support QSYNTH.
09/18/05    st     Added qcpsup_validate_raw_frames() function prototype.
07/22/05    sm     Added WAV mu/alaw support.
04/28/05   ymc     Added 4GV NB support.
           ymc     Updated Copyright.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
12/02/03    sg     Added support for linear PCM playback.
04/17/03    sm     Added AMR file format support.
02/19/03    st     Refeaturized qcpsup_adpcm_format_type to expose under
                   FEATURE_ADPCM; added qcpsup_get_adpcm_fmt() function
                   prototype.
11/06/02    sm     Added QCP AMR support.
08/10/02  aw/st    Modified code to support ADPCM file format.
09/03/01    st     Modified code to support EVRC QCP files; added code for
                   QCP chunk parsing functions.
04/04/01    st     created file

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/* The following is a structure describing the format of an ADPCM file.
*/
typedef enum {
  QCPSUP_WAVE_FORMAT_LINEAR_PCM = 0x01,
#ifdef FEATURE_VOC_G711
  QCPSUP_WAVE_FORMAT_ALAW       = 0x06,
  QCPSUP_WAVE_FORMAT_MULAW      = 0x07,
#endif /* FEATURE_VOC_G711 */
  QCPSUP_WAVE_FORMAT_IMA_ADPCM  = 0x11
} qcpsup_wave_format_enum_type;

typedef struct {
  qcpsup_wave_format_enum_type wave_format; /* Type of PCM encoding        */
  uint16 channels;           /* Number of channels                         */
  uint32 sample_rate;        /* Samplers per second                        */
  uint32 bytes_rate;         /* Average bytes per second                   */
  uint16 frame_size;         /* Block Align, num of bytes for n-channels   */
  uint16 bits_per_sample;    /* Number of bits used for each sample        */
} qcpsup_adpcm_format_type;
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* The following is used to indicate the return status of functions.
*/
#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WAVE_PB)
typedef enum {
  QCPSUP_STATUS_SUCCESS,   /* Status success        */
  QCPSUP_STATUS_ERROR,     /* Status error          */
  QCPSUP_STATUS_BUFF_ERROR /* Status buffer error   */
} qcpsup_status_type;
#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP ||
          FEATURE_WAVE_PB */

#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP)
/* The following is a list of formats supported by this module.
*/
typedef enum {
  QCPSUP_FORMAT_13K,     /* 13k QCP format  */
  QCPSUP_FORMAT_EVRC,    /* EVRC QCP format */
  QCPSUP_FORMAT_4GV_NB,  /* 4GV QCP format  */
  QCPSUP_FORMAT_AMR,     /* AMR QCP format  */
  QCPSUP_FORMAT_EFR,     /* EFR QCP format  */
  QCPSUP_FORMAT_FR,      /* FR QCP format   */
  QCPSUP_FORMAT_HR,      /* HR QCP format   */
  QCPSUP_AMR_FILE_FORMAT,/* .AMR file format*/
  QCPSUP_AWB_FILE_FORMAT,/* .AWB file format*/
  QCPSUP_EVB_FILE_FORMAT,/* .EVB file format*/
  QCPSUP_EVW_FILE_FORMAT,/* .EVW file format*/
  QCPSUP_FORMAT_INVALID
} qcpsup_format_enum_type;
#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

/* <EJECT> */
/*---------------------------------------------------------------------------
** QCPSUP Public Function Prototypes
**---------------------------------------------------------------------------
*/
#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WAVE_PB)
/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_do_riff

DESCRIPTION
  This function validates a RIFF header.

DEPENDENCIES
  None

RETURN VALUE
  This function returns the location of the end of the RIFF ID.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_do_riff (
  uint8   *riff_id,
  uint8   *buffer,
  uint32  *index,
  uint32  *length
);

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_find_chunk

DESCRIPTION
  This function parses and locates the indicated chunk.

DEPENDENCIES
  None

RETURN VALUE
  The size and location of the chunk is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_find_chunk (
  uint8  *chunk_id,
  uint8  *buffer,
  uint32 *index,
  uint32 buf_len,
  uint32 *length
);
#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP ||
          FEATURE_WAVE_PB */

#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

FUNCTION qcpsup_validate_frame

DESCRIPTION
  This function validates a QCP frame header byte and returns the size of the
  packet based on the rate.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if the frame byte is a valid frame byte.
  FALSE - if the frame byte is not a valid frame byte.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qcpsup_validate_frame (
  qcpsup_format_enum_type  format,
  uint8                    frame_byte,
  uint32                   *frame_size
);

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_validate_raw_frames

DESCRIPTION
  This function validates five consecutive 13K, EVRC or 4GV_NB frames.
  At least three of the five frames must be non-blank.

DEPENDENCIES
  None

RETURN VALUE
  Format of frames, if recognized.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_format_enum_type qcpsup_validate_raw_frames (
  uint8  *frame_buf,
  uint32 length
);

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_validate_fixed_full

DESCRIPTION
  This function validates three consecutive 13K fixed full rate QCP frames.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if there are three consecutive packets of fixed full rate frames.
  FALSE - if not valid QCP frames.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean qcpsup_validate_fixed_full (
  uint8  *frame_buf,
  uint32 length
);

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_get_fmt

DESCRIPTION
  This function parses and finds the fmt chunk.

DEPENDENCIES
  None

RETURN VALUE
  The format of the QCP file is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_get_fmt (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  qcpsup_format_enum_type  *format
);

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_get_vrat

DESCRIPTION
  This function parses and finds the vrat chunk.

DEPENDENCIES
  None

RETURN VALUE
  The number of frames in the QCP file is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_get_vrat (
  uint8   *buffer,
  uint32  *index,
  uint32  buf_len,
  uint32  *num_frames
);

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_get_cnfg

DESCRIPTION
  This function parses and finds the cnfg chunk.

DEPENDENCIES
  None

RETURN VALUE
  The cnfg chunk value of the QCP file is returned.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_get_cnfg (
  uint8   *buffer,
  uint32  *index,
  uint32  buf_len,
  uint32  *cnfg
);

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_do_head

DESCRIPTION
  This function parses and validates the QCP head until the DATA chunk
  is found.

DEPENDENCIES
  None

RETURN VALUE
  This funtion returns the format of the QCP file and the size and
  location of the data chunk.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_do_head (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  uint32                   *length,
  qcpsup_format_enum_type  *format
);

/* <EJECT> */
#ifdef FEATURE_QSYNTH_ADPCM
/*===========================================================================

FUNCTION qcpsup_adpcm_do_head

DESCRIPTION
  This function parses and validates the ADPCM head until the DATA chunk
  is found.

DEPENDENCIES
  None

RETURN VALUE
  This funtion returns the format of the ADPCM file and the size and
  location of the data chunk.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_adpcm_do_head (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  uint32                   *length,
  qcpsup_adpcm_format_type *format
);
#endif /* FEATURE_QSYNTH_ADPCM */
#if defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_GASYNTH_INTERFACE)
/*===========================================================================

FUNCTION qcpsup_do_amr_head

DESCRIPTION
  This function verifies whether the content of buffer starts with amr-nb 
  header.

DEPENDENCIES
  None

RETURN VALUE
  Return TRUE if content of buffer starts with amr-nb header; Otherwise, return
  FALSE. Index is also incremented by length of AMR header.

SIDE EFFECTS
  None

===========================================================================*/
extern qcpsup_status_type qcpsup_do_amr_head
(
  uint8    *buffer,
  uint32   *index,
  uint32   buf_len
);
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */
#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */

#ifdef FEATURE_WAVE_PB
/*===========================================================================

FUNCTION qcpsup_get_wave_fmt

DESCRIPTION
  This function parses and finds the fmt chunk in an ADPCM file.

DEPENDENCIES
  None

RETURN VALUE
  ADPCM format information is returned.

SIDE EFFECTS
  None

===========================================================================*/
qcpsup_status_type qcpsup_get_wave_fmt (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  qcpsup_adpcm_format_type *format
);
#endif /* FEATURE_WAVE_PB */

#endif /* QCPSUP_H */
