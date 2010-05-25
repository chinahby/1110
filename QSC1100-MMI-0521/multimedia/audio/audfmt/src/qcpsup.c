/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("QCP Format Utilities")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Q C P   F O R M A T   U T I L I T I E S

GENERAL DESCRIPTION
  These functions support QCP format type utilities.

EXTERNALIZED FUNCTIONS
  qcpsup_validate_frame
    Validates a QCP frame header byte.
  qcpsup_do_head
    Parses the head of a QCP file.
  qcpsup_do_amr_head
    Validate AMR-NB header.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2001-2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/qcpsup.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/29/08    sj     Added .awb file format support
10/26/07    hs     Added EVW file format support
09/27/07    pl     Fixed merge error which caused EVB to break.
07/05/07    hs     Fixed CR 122000 checking invalid headers in qcp file format
04/12/07    hs     Checking for NULL buffer pointer
04/07/07    pl     Added AMR-NB header parser.
03/21/07    at     Added EVRC-B file format support.
06/01/06    sg     Fixed featurization so that G711 content can be played
                   when the build does not support QSYNTH.
01/24/06   act     Changed 4GV-NB QCP GUID to correct value.
11/07/05    sm     In qcpsup_validate_frame(), process ONSET, BAD, and SID_BAD.
09/18/05    st     Added qcpsup_validate_raw_frames() function.
07/22/05    sm     Added WAV mu/alaw support.
04/28/05   ymc     Added QCP file support for 4GV vocoder.
09/15/04    sm     Added QCP file support for EFR/FR/HR vocoders.
08/11/04    st     Modified code to support misaligned buffer access in
                   qcpsup_get_wave_fmt().
12/02/03    sg     Added support for linear PCM playback.
04/17/03    sm     Added support for AMR file format.
02/19/03    st     Refeaturized code to expose qcpsup_get_adpcm_fmt() under
                   FEATURE_ADPCM.
12/04/02    sg     Modified so that qcpsup_validate_frame() fails when the
                   AMR frame mode is not valid.
11/06/02    sm     Added QCP AMR support.
09/19/02    st     Modified code to update index parameter in qcpsup_do_head()
                   and qcpsup_adpcm_do_head() only if function is successful.
08/10/02  aw/st    Modified code to support ADPCM file format; adjusted
                   debug MSG levels.
03/18/02    st     Modified code to check for NULL pointer before using.
09/03/01    st     Modified code to support EVRC QCP files; added code for
                   QCP chunk parsing functions.
04/04/01    st     Created

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "task.h"               /* Task definitions and prototypes         */

#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP) || defined(FEATURE_WAVE_PB)

#include "qcpsup.h"             /* Audio Format definitions and prototypes */
#include "qcpsupi.h"            /* Internal Audio Format definitions       */
#include "msg.h"                /* Message logging/reporting services      */
#include <string.h>             /* String routines                         */

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_read_length

DESCRIPTION
  This function reads the length entries of a QCP file.

DEPENDENCIES
  None

RETURN VALUE
  Number of valid frames.

SIDE EFFECTS
  None

===========================================================================*/
void qcpsup_read_length (
  uint8   *buffer,
  uint32  *length
) {

  if(buffer != NULL) {
    *length  = (buffer[3] << 24) | (buffer[2] << 16) |
               (buffer[1] << 8)  | (buffer[0]);
  }

}

/* <EJECT> */
/*===========================================================================

FUNCTION qcpsup_skip_chunk

DESCRIPTION
  This function skips a chunk.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void qcpsup_skip_chunk (
  uint8   **buffer,
  uint32  *cur_index
) {
  uint32 num_bytes;

  qcpsup_read_length (&((*buffer)[QCPSUP_CHUNK_ID_BYTES]), &num_bytes);

  if(num_bytes & 1) {
    num_bytes++;
  }
  *buffer    += (num_bytes + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES);
  *cur_index += (num_bytes + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES);

}

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
qcpsup_status_type qcpsup_do_riff (
  uint8   *riff_id,
  uint8   *buffer,
  uint32  *index,
  uint32  *length
) {
  qcpsup_status_type ret_val = QCPSUP_STATUS_ERROR;
  uint32 total_length = 0;

  /* Checking NULL to prevent crashing */
  if( buffer != NULL )
  {
    buffer = &buffer[*index];
  
    if(strncmp(QCPSUP_RIFF_ID, (char *) buffer, QCPSUP_CHUNK_ID_BYTES) == 0) {
  
      buffer += QCPSUP_CHUNK_ID_BYTES;
      qcpsup_read_length (buffer, length);
      buffer += QCPSUP_LENGTH_BYTES;

      /* check integer overflow */
      total_length = *length + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES;
	  if( total_length < *length)
	  {
	    MSG_ERROR("Encountered a chunk length overflow error",0,0,0);
        return QCPSUP_STATUS_ERROR;
	  }

      if(strncmp((char *) riff_id, (char *) buffer, QCPSUP_CHUNK_ID_BYTES) == 0) {
        *index  += QCPSUP_LENGTH_BYTES + (2*QCPSUP_CHUNK_ID_BYTES);
        *length += QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES;
        ret_val  = QCPSUP_STATUS_SUCCESS;
      }
    }
  }

  return(ret_val);
}

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
qcpsup_status_type qcpsup_find_chunk (
  uint8  *chunk_id,
  uint8  *buffer,
  uint32 *index,
  uint32 buf_len,
  uint32 *length
) {
  qcpsup_status_type ret_val   = QCPSUP_STATUS_BUFF_ERROR;
  uint32             cur_index = 0;

  // Checking NULL to prevent crashing
  if( buffer != NULL )
  {
    buffer = &buffer[*index];
    while((cur_index + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES) <= buf_len) {
      if(strncmp((char *) chunk_id, (char *) buffer, QCPSUP_CHUNK_ID_BYTES) == 0)
      {
        buffer    += QCPSUP_CHUNK_ID_BYTES;
        cur_index += (QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES);
  
        qcpsup_read_length (buffer, length);
        ret_val = QCPSUP_STATUS_SUCCESS;
        break;
  
      } else {
        qcpsup_skip_chunk (&buffer, &cur_index);
      }
    }
  
    *index += cur_index;
  }

  return(ret_val);
}

/* <EJECT> */
#if defined(FEATURE_WAVE_PB) || defined(FEATURE_QSYNTH_ADPCM)
/*===========================================================================

FUNCTION qcpsup_get_wave_fmt

DESCRIPTION
  This function parses and finds the fmt chunk in a .wav file.

DEPENDENCIES
  None

RETURN VALUE
  WAV format information is returned.

SIDE EFFECTS
  None

===========================================================================*/
qcpsup_status_type qcpsup_get_wave_fmt (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  qcpsup_adpcm_format_type *format
) {
  qcpsup_status_type ret_val   = QCPSUP_STATUS_BUFF_ERROR;
  uint32             cur_index = 0;
  uint16             wav_format;

  buffer = &buffer[*index];
  while((cur_index + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES) <= buf_len) {
    if(strncmp(QCPSUP_FMT_ID, (char *) buffer, QCPSUP_CHUNK_ID_BYTES) == 0) {
      if((cur_index + QCPSUP_CHUNK_ID_BYTES + 20) <= buf_len) {

        memcpy(&wav_format, buffer+8, sizeof(uint16));
        if(wav_format != QCPSUP_WAVE_FORMAT_LINEAR_PCM &&
#ifdef FEATURE_VOC_G711
           wav_format != QCPSUP_WAVE_FORMAT_ALAW &&
           wav_format != QCPSUP_WAVE_FORMAT_MULAW &&
#endif /* FEATURE_VOC_G711 */
           wav_format != QCPSUP_WAVE_FORMAT_IMA_ADPCM) {
          ret_val = QCPSUP_STATUS_ERROR;
        } else {
          format->wave_format     = wav_format;
          memcpy(&format->channels,        buffer+10, sizeof(uint16));
          memcpy(&format->sample_rate,     buffer+12, sizeof(uint32));
          memcpy(&format->bytes_rate,      buffer+16, sizeof(uint32));
          memcpy(&format->frame_size,      buffer+20, sizeof(uint16));
          memcpy(&format->bits_per_sample, buffer+22, sizeof(uint16));

          qcpsup_skip_chunk (&buffer, &cur_index);

          ret_val = QCPSUP_STATUS_SUCCESS;
        }
        break;
      }
    } else {
      qcpsup_skip_chunk (&buffer, &cur_index);
    }
  }
  *index += cur_index;
  return(ret_val);
}
#endif /* FEATURE_WAVE_PB || FEATURE_QSYNTH_ADPCM */

/* <EJECT> */
#if defined(FEATURE_QCP) || defined(FEATURE_WEBAUDIO) || \
    defined(FEATURE_MIDI_OUT_QCP)
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

There are definitions for LOCAL data.  There is no global data.

===========================================================================*/
/* The following data arrays are used to determine the format of QCP
** files. They are the unique IDs used in 13k, EVRC, 4GV and AMR QCP files.
*/
const uint8 qcpsup_13k_guid1[] = {
  0x41,0x6D,0x7F,0x5E,
  0x15,0xB1,
  0xD0,0x11,
  0xBA,0x91,0x00,0x80,0x5F,0xB4,0xB9,0x7E
};

const uint8 qcpsup_13k_guid2[] = {
  0x42,0x6D,0x7F,0x5E,
  0x15,0xB1,
  0xD0,0x11,
  0xBA,0x91,0x00,0x80,0x5F,0xB4,0xB9,0x7E
};

const uint8 qcpsup_evrc_guid[] = {
  0x8D,0xD4,0x89,0xE6,
  0x76,0x90,
  0xB5,0x46,
  0x91,0xEF,0x73,0x6A,0x51,0x00,0xCE,0xB4
};

/* 4GV-NB GUID {CA29FD3C-53F6-F54E-90E9-F4236D599B61} */
const uint8 qcpsup_4gv_nb_guid[] = {
  0xCA,0x29,0xFD,0x3C,
  0x53,0xF6,
  0xF5,0x4E,
  0x90,0xE9,
  0xF4,0x23,0x6D,0x59,0x9B,0x61
};

/* 4GV-WB GUID {84001D0B-4CDD-594C-9E2D-CD1FB7261BA1} */
const uint8 qcpsup_4gv_wb_guid[] = {
  0x84,0x00,0x1D,0x0B,
  0x4C,0xDD,
  0x59,0x4C,
  0x9E,0x2D,
  0xCD,0x1F,0xB7,0x26,0x1B,0xA1
};

/* AMR GUID {53E0A86A-4F47-46bd-8AFA-ACF2328273BD} */
const uint8 qcpsup_amr_guid[] = {
  0x53,0xE0,0xA8,0x6A,
  0x4F,0x47,
  0x46,0xBD,
  0x8A,0xFA,
  0xAC,0xF2,0x32,0x82,0x73,0xBD
};

/* AMR-WB GUID {AFAE660D-F87B-5B49-B70F-73E63FFAF064} */
const uint8 qcpsup_amr_wb_guid[] = {
  0xAF,0xAE,0x66,0x0D,
  0xF8,0x7B,
  0x5B,0x49,
  0xB7,0x0F, 
  0x73,0xE6,0x3F,0xFA,0xF0,0x64
};

/* AMR-WB+ GUID {65E9E091-23D8-4947-976C-1284B2336A88} */
const uint8 qcpsup_amr_wb_plus_guid[] = {
  0x65,0xE9,0xE0,0x91,
  0x23,0xD8,
  0x49,0x47,
  0x97,0x6C,
  0x12,0x84,0xB2,0x33,0x6A,0x88
};

/* EFR GUID {08900075-89E9-4b6b-931F-5C0E67AD6D5D} */
const uint8 qcpsup_efr_guid[] = {
  0x08,0x90,0x00,0x75,
  0x89,0xe9,
  0x4b,0x6b,
  0x93,0x1f,
  0x5c,0x0e,0x67,0xad,0x6d,0x5d
};

/* FR GUID {8C3D5CE8-8BF0-48a5-8546-1887EB2B4D05} */
const uint8 qcpsup_fr_guid[] = {
  0x8c,0x3d,0x5c,0xe8,
  0x8b,0xf0,
  0x48,0xa5,
  0x85,0x46,
  0x18,0x87,0xeb,0x2b,0x4d,0x05
};

/* HR GUID {1036661E-DB73-4204-8697-F914D3D86D8C} */
const uint8 qcpsup_hr_guid[] = {
  0x10,0x36,0x66,0x1e,
  0xdb,0x73,
  0x42,0x04,
  0x86,0x97,
  0xf9,0x14,0xd3,0xd8,0x6d,0x8c
};

/* <EJECT> */
#if defined(FEATURE_AUDFMT_AMR) || (defined(FEATURE_MIDI_OUT_QCP) \
    && defined(FEATURE_GASYNTH_INTERFACE))
/*===========================================================================

FUNCTION qcpsup_validate_amr_frame

DESCRIPTION
  This function validates an AMR frame header byte and returns the size of the 
  packet.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if the frame byte is a valid frame byte.
  FALSE - if the frame byte is not a valid frame byte.

SIDE EFFECTS
  None

===========================================================================*/
boolean qcpsup_validate_amr_frame (
  uint8  frame_byte,
  uint32 *frame_size
) {
  qcpsup_amr_frame_type_enum amr_type;
  boolean                    ret_val = TRUE;

  /* Extract AMR frame type from header byte */
  amr_type = (qcpsup_amr_frame_type_enum)
               (frame_byte & AUDAMR_FRAME_TYPE_MASK) >> AUDAMR_FRAME_TYPE_SHIFT;

  /* Choose size based on amr_type */
  switch(amr_type) {
    case QCPSUP_AMR_TYPE_0475:
      *frame_size = QCPSUP_RATE_AMR_475_SIZE;
      break;

    case QCPSUP_AMR_TYPE_0515:
      *frame_size = QCPSUP_RATE_AMR_515_SIZE;
      break;

    case QCPSUP_AMR_TYPE_0590:
      *frame_size = QCPSUP_RATE_AMR_590_SIZE;
      break;

    case QCPSUP_AMR_TYPE_0670:
      *frame_size = QCPSUP_RATE_AMR_670_SIZE;
      break;

    case QCPSUP_AMR_TYPE_0740:
      *frame_size = QCPSUP_RATE_AMR_740_SIZE;
      break;

    case QCPSUP_AMR_TYPE_0795:
      *frame_size = QCPSUP_RATE_AMR_795_SIZE;
      break;

    case QCPSUP_AMR_TYPE_1020:
      *frame_size = QCPSUP_RATE_AMR_102_SIZE;
      break;

    case QCPSUP_AMR_TYPE_1220:
      *frame_size = QCPSUP_RATE_AMR_122_SIZE;
      break;

    case QCPSUP_AMR_TYPE_AMR_SID:
      *frame_size = QCPSUP_RATE_AMR_SID_SIZE;
      break;

    case QCPSUP_AMR_TYPE_NO_DATA:
      *frame_size = QCPSUP_RATE_AMR_BLANK_SIZE;
      break;

    default:
      ret_val = FALSE;
      break;
  }
  return(ret_val);
}
#endif /* FEATURE_AUDFMT_AMR || 
         (FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE) */

/*===========================================================================

FUNCTION qcpsup_validate_amr_wb_frame

DESCRIPTION
  This function validates an AMR frame header byte and returns the size of the 
  packet.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if the frame byte is a valid frame byte.
  FALSE - if the frame byte is not a valid frame byte.

SIDE EFFECTS
  None

===========================================================================*/
boolean qcpsup_validate_amr_wb_frame (
  uint8  frame_byte,
  uint32 *frame_size
) {
  qcpsup_amr_wb_frame_type_enum amr_wb_ftype;
  boolean                    ret_val = TRUE;

  /* Extract AMR frame type from header byte */
  amr_wb_ftype = (qcpsup_amr_wb_frame_type_enum)
               (frame_byte & AUDAWB_FRAME_TYPE_MASK) >> AUDAWB_FRAME_TYPE_SHIFT;

  /* Choose size based on amr_type */
  switch(amr_wb_ftype) {
    case QCPSUP_AWB_TYPE_0660:
      *frame_size = QCPSUP_AWB_RATE_0660_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_0885:
      *frame_size = QCPSUP_AWB_RATE_0885_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_1265:
      *frame_size = QCPSUP_AWB_RATE_1265_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_1425:
      *frame_size = QCPSUP_AWB_RATE_1425_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_1585:
      *frame_size = QCPSUP_AWB_RATE_1585_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_1825:
      *frame_size = QCPSUP_AWB_RATE_1825_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_1985:
      *frame_size = QCPSUP_AWB_RATE_1985_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_2305:
      *frame_size = QCPSUP_AWB_RATE_2305_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_2385:
      *frame_size = QCPSUP_AWB_RATE_2385_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_SID:
      *frame_size = QCPSUP_AWB_SID_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_SPEECH_LOST:
      *frame_size = QCPSUP_AWB_SPEECH_LOST_FRAME_SIZE;
      break;

    case QCPSUP_AWB_TYPE_NO_DATA:
      *frame_size = QCPSUP_AWB_NO_DATA_FRMAE_SIZE;
      break;

    default:
      MSG_MED("Invalid AMR-WB frame header %d", frame_byte,0,0);
      ret_val = FALSE;
      break;
  }
  return(ret_val);
}

/* <EJECT> */
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
boolean qcpsup_validate_frame (
  qcpsup_format_enum_type  format,
  uint8                    frame_byte,
  uint32                   *frame_size
) {
  boolean ret_val = TRUE;

#if defined(FEATURE_AUDFMT_AMR) || (defined(FEATURE_MIDI_OUT_QCP) \
    && defined(FEATURE_GASYNTH_INTERFACE))
  if(format == QCPSUP_AMR_FILE_FORMAT) {
    return(qcpsup_validate_amr_frame(frame_byte, frame_size));
  }
#endif /* FEATURE_AUDFMT_AMR || 
         (FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE) */

  if(format == QCPSUP_AWB_FILE_FORMAT) {
    return(qcpsup_validate_amr_wb_frame(frame_byte, frame_size));
  }

#if defined(FEATURE_AUDFMT_EVB) || defined(FEATURE_AUDFMT_EVW)
  if ((format == QCPSUP_EVW_FILE_FORMAT)||(format == QCPSUP_EVB_FILE_FORMAT)) {
    switch ( frame_byte ) {
      case QCPSUP_EVB_RATE_BLANK:
      case QCPSUP_EVB_RATE_ERASURE:
        *frame_size = QCPSUP_RATE_4GV_NB_BLANK_SIZE;
        break;

      case QCPSUP_EVB_RATE_EIGHTH:
        *frame_size = QCPSUP_RATE_4GV_NB_EIGHTH_SIZE;
        break;

      case QCPSUP_EVB_RATE_QUARTER:
        *frame_size = QCPSUP_RATE_4GV_NB_QUARTER_SIZE;
        break;

      case QCPSUP_EVB_RATE_HALF:
        *frame_size = QCPSUP_RATE_4GV_NB_HALF_SIZE;
        break;

      case QCPSUP_EVB_RATE_FULL:
        *frame_size = QCPSUP_RATE_4GV_NB_FULL_SIZE;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }
#endif /* FEATURE_AUDFMT_EVB or FEATURE_AUDFMT_EVW */

  if(format == QCPSUP_FORMAT_13K) {
    switch(frame_byte) {
      case QCPSUP_RATE_ERASURE:
      case QCPSUP_RATE_BLANK:
        *frame_size = QCPSUP_RATE_13K_BLANK_SIZE;
        break;

      case QCPSUP_RATE_EIGHTH:
        *frame_size = QCPSUP_RATE_13K_EIGHTH_SIZE;
        break;

      case QCPSUP_RATE_QUARTER:
        *frame_size = QCPSUP_RATE_13K_QUARTER_SIZE;
        break;

      case QCPSUP_RATE_HALF:
        *frame_size = QCPSUP_RATE_13K_HALF_SIZE;
        break;

      case QCPSUP_RATE_FULL:
        *frame_size = QCPSUP_RATE_13K_FULL_SIZE;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }
  if(format == QCPSUP_FORMAT_EVRC) {
    switch(frame_byte) {
      case QCPSUP_RATE_ERASURE:
      case QCPSUP_RATE_BLANK:
        *frame_size = QCPSUP_RATE_EVRC_BLANK_SIZE;
        break;

      case QCPSUP_RATE_EIGHTH:
        *frame_size = QCPSUP_RATE_EVRC_EIGHTH_SIZE;
        break;

      case QCPSUP_RATE_QUARTER:
        *frame_size = QCPSUP_RATE_EVRC_QUARTER_SIZE;
        break;

      case QCPSUP_RATE_HALF:
        *frame_size = QCPSUP_RATE_EVRC_HALF_SIZE;
        break;

      case QCPSUP_RATE_FULL:
        *frame_size = QCPSUP_RATE_EVRC_FULL_SIZE;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }

  if(format == QCPSUP_FORMAT_4GV_NB) {
    switch(frame_byte) {
      case QCPSUP_RATE_ERASURE:
      case QCPSUP_RATE_BLANK:
        *frame_size = QCPSUP_RATE_4GV_NB_BLANK_SIZE;
        break;

      case QCPSUP_RATE_EIGHTH:
        *frame_size = QCPSUP_RATE_4GV_NB_EIGHTH_SIZE;
        break;

      case QCPSUP_RATE_QUARTER:
        *frame_size = QCPSUP_RATE_4GV_NB_QUARTER_SIZE;
        break;

      case QCPSUP_RATE_HALF:
        *frame_size = QCPSUP_RATE_4GV_NB_HALF_SIZE;
        break;

      case QCPSUP_RATE_FULL:
        *frame_size = QCPSUP_RATE_4GV_NB_FULL_SIZE;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }

  if(format == QCPSUP_FORMAT_AMR) {
    switch(QCPSUP_EXTRACT_AMR_FRAME_MODE(frame_byte)) {
      case QCPSUP_AMR_SPEECH_GOOD:
      case QCPSUP_AMR_SPEECH_DEGRADED:
      case QCPSUP_AMR_ONSET:
      case QCPSUP_AMR_SPEECH_BAD:
        switch(QCPSUP_EXTRACT_AMR_RATE(frame_byte)) {

          case QCPSUP_RATE_BLANK:
            *frame_size = QCPSUP_RATE_AMR_BLANK_SIZE;
            break;
          case QCPSUP_RATE_4_75_AMR:
            *frame_size = QCPSUP_RATE_AMR_475_SIZE;
            break;
          case QCPSUP_RATE_5_15_AMR:
            *frame_size = QCPSUP_RATE_AMR_515_SIZE;
            break;
          case QCPSUP_RATE_5_9_AMR:
            *frame_size = QCPSUP_RATE_AMR_590_SIZE;
            break;
          case QCPSUP_RATE_6_7_AMR:
            *frame_size = QCPSUP_RATE_AMR_670_SIZE;
            break;
          case QCPSUP_RATE_7_4_AMR:
            *frame_size = QCPSUP_RATE_AMR_740_SIZE;
            break;
          case QCPSUP_RATE_7_95_AMR:
            *frame_size = QCPSUP_RATE_AMR_795_SIZE;
            break;
          case QCPSUP_RATE_10_2_AMR:
            *frame_size = QCPSUP_RATE_AMR_102_SIZE;
            break;
          case QCPSUP_RATE_12_2_AMR:
            *frame_size = QCPSUP_RATE_AMR_122_SIZE;
            break;

          default:
            MSG_ERROR("Invalid frame header %d", frame_byte,0,0);
            ret_val = FALSE;
            break;
        }
        break;

      case QCPSUP_AMR_SID_FIRST:
      case QCPSUP_AMR_SID_UPDATE:
      case QCPSUP_AMR_SID_BAD:
        *frame_size = QCPSUP_RATE_AMR_SID_SIZE;
        break;

      case QCPSUP_AMR_NO_DATA:
        *frame_size = 0;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }

  if(format == QCPSUP_FORMAT_EFR) {
    switch(QCPSUP_EXTRACT_GSM_FRAME_MODE(frame_byte)) {
      case QCPSUP_GSM_SPEECH_GOOD:
        if(QCPSUP_EXTRACT_GSM_RATE(frame_byte) == QCPSUP_RATE_FULL_EFR) {
          *frame_size = QCPSUP_RATE_GSM_EFR_SIZE;
        } else {
          MSG_ERROR("Invalid frame header %d", frame_byte,0,0);
          ret_val = FALSE;
        }
        break;

      case QCPSUP_GSM_SID:
        *frame_size = QCPSUP_RATE_GSM_EFR_SID_SIZE;
        break;

      case QCPSUP_GSM_BFI:
      case QCPSUP_GSM_INVALID_SID:
      case QCPSUP_GSM_NO_DATA:
        *frame_size = 0;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }

  if(format == QCPSUP_FORMAT_FR) {
    switch(QCPSUP_EXTRACT_GSM_FRAME_MODE(frame_byte)) {
      case QCPSUP_GSM_SPEECH_GOOD:
        if(QCPSUP_EXTRACT_GSM_RATE(frame_byte) == QCPSUP_RATE_FULL_FR) {
          *frame_size = QCPSUP_RATE_GSM_FR_SIZE;
        } else {
          MSG_ERROR("Invalid frame header %d", frame_byte,0,0);
          ret_val = FALSE;
        }
        break;

      case QCPSUP_GSM_SID:
        *frame_size = QCPSUP_RATE_GSM_FR_SID_SIZE;
        break;

      case QCPSUP_GSM_BFI:
      case QCPSUP_GSM_INVALID_SID:
      case QCPSUP_GSM_NO_DATA:
        *frame_size = 0;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }

  if(format == QCPSUP_FORMAT_HR) {
    switch(QCPSUP_EXTRACT_GSM_FRAME_MODE(frame_byte)) {
      case QCPSUP_GSM_SPEECH_GOOD:
        if(QCPSUP_EXTRACT_GSM_RATE(frame_byte) == QCPSUP_RATE_FULL_HR) {
          *frame_size = QCPSUP_RATE_GSM_HR_SIZE;
        } else {
          MSG_ERROR("Invalid frame header %d", frame_byte,0,0);
          ret_val = FALSE;
        }
        break;

      case QCPSUP_GSM_SID:
        *frame_size = QCPSUP_RATE_GSM_HR_SID_SIZE;
        break;

      case QCPSUP_GSM_BFI:
      case QCPSUP_GSM_INVALID_SID:
      case QCPSUP_GSM_NO_DATA:
        *frame_size = 0;
        break;

      default:
        MSG_MED("Invalid frame header %d", frame_byte,0,0);
        ret_val = FALSE;
        break;
    }
  }

  if(format >= QCPSUP_FORMAT_INVALID) {
    MSG_ERROR("Invalid frame format %d", format, 0, 0);
    ret_val = FALSE;
  }

  return( ret_val );
}

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
qcpsup_format_enum_type qcpsup_validate_raw_frames (
  uint8  *frame_buf,
  uint32 length
) {
  qcpsup_format_enum_type ret_val;
  uint32                  counter, tlen, index, blank_frames;

  index        = 0;
  blank_frames = 0;
  ret_val      = QCPSUP_FORMAT_13K;
  for(counter = 0; counter < 5; counter++) {
    if((index <= length) && (blank_frames < 3) &&
       (qcpsup_validate_frame(QCPSUP_FORMAT_13K, frame_buf[index], &tlen))) {
      if(tlen == 0) {
        blank_frames++;
      }
      index += tlen + 1;
    } else {
      ret_val = QCPSUP_FORMAT_INVALID;
      break;
    }
  }

  if((ret_val == QCPSUP_FORMAT_INVALID) || (blank_frames >= 3)) {
    index        = 0;
    blank_frames = 0;
    ret_val      = QCPSUP_FORMAT_EVRC;
    for(counter = 0; counter < 5; counter++) {
      if((index <= length) && (blank_frames < 3) &&
         (qcpsup_validate_frame(QCPSUP_FORMAT_EVRC, frame_buf[index], &tlen))) {
        if(tlen == 0) {
          blank_frames++;
        }
        index += tlen + 1;
      } else {
        ret_val = QCPSUP_FORMAT_INVALID;
        break;
      }
    }

    if((ret_val == QCPSUP_FORMAT_INVALID) || (blank_frames >= 3)) {
      index        = 0;
      blank_frames = 0;
      ret_val      = QCPSUP_FORMAT_4GV_NB;
      for(counter = 0; counter < 5; counter++) {
        if((index <= length) && (blank_frames < 3) &&
           (qcpsup_validate_frame(QCPSUP_FORMAT_4GV_NB,
                                  frame_buf[index], &tlen))) {
          if(tlen == 0) {
            blank_frames++;
          }
          index += tlen + 1;
        } else {
          ret_val = QCPSUP_FORMAT_INVALID;
          break;
        }
      }

      if(blank_frames >= 3) {
        ret_val = QCPSUP_FORMAT_INVALID;
      }
    }
  }

  return(ret_val);
}

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
boolean qcpsup_validate_fixed_full (
  uint8  *frame_buf,
  uint32 length
) {
  boolean ret_val = FALSE;
  uint32  counter, tlen, index = 0;

  // Checking NULL to prevent crashing
  if( frame_buf != NULL )
  {
    for(counter = 0; counter < 3; counter++) {
      if((index <= length) && 
         (qcpsup_validate_frame(QCPSUP_FORMAT_13K, frame_buf[index], &tlen)) &&
         (tlen == QCPSUP_RATE_13K_FULL_SIZE)) 
      {
        index += QCPSUP_RATE_13K_FULL_SIZE + 1;
      } else {
        break;
      }
    }
      ret_val = ((counter == 3) && (index <= length));
  }

  return(ret_val);
}

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
qcpsup_status_type qcpsup_get_fmt (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  qcpsup_format_enum_type  *format
) {
  qcpsup_status_type ret_val   = QCPSUP_STATUS_BUFF_ERROR;
  uint32             cur_index = 0;

  // Checking NULL to prevent crashing
  if( buffer != NULL )
  {
    buffer = &buffer[*index];
  
    /* Initialize format to INVALID */
    if(format != NULL) {
      *format = QCPSUP_FORMAT_INVALID;
    }
  
    while((cur_index + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES) <= buf_len) {
      if(strncmp(QCPSUP_FMT_ID, (char *) buffer, QCPSUP_CHUNK_ID_BYTES) == 0) {
        if((cur_index + QCPSUP_CHUNK_ID_BYTES + 6 + 16) <= buf_len) {
          if((memcmp(&buffer[10], qcpsup_13k_guid1, 16) == 0) ||
             (memcmp(&buffer[10], qcpsup_13k_guid2, 16) == 0)) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_13K;
            }
          }
          if(memcmp(&buffer[10], qcpsup_evrc_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_EVRC;
            }
          }
          if(memcmp(&buffer[10], qcpsup_4gv_nb_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_4GV_NB;
            }
          }
          if(memcmp(&buffer[10], qcpsup_4gv_wb_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_INVALID;
                     /* no such file format */
            }
          }
          if(memcmp(&buffer[10], qcpsup_amr_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_AMR;
            }
          }
          if(memcmp(&buffer[10], qcpsup_amr_wb_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_INVALID;
                     /* no such file format */
            }
          }
          if(memcmp(&buffer[10], qcpsup_amr_wb_plus_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_INVALID;
                     /* no such file format */
            }
          }
          if(memcmp(&buffer[10], qcpsup_efr_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_EFR;
            }
          }
          if(memcmp(&buffer[10], qcpsup_fr_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_FR;
            }
          }
          if(memcmp(&buffer[10], qcpsup_hr_guid, 16) == 0) {
            if(format != NULL) {
              *format = QCPSUP_FORMAT_HR;
            }
          }
          if((format != NULL) && (*format >= QCPSUP_FORMAT_INVALID)) {
            ret_val = QCPSUP_STATUS_ERROR;
            break;
          }
        } else {
          break;
        }
  
        qcpsup_skip_chunk (&buffer, &cur_index);
  
        ret_val = QCPSUP_STATUS_SUCCESS;
        break;
  
      } else {
        qcpsup_skip_chunk (&buffer, &cur_index);
      }
    }
  
    *index += cur_index;
  }

  return(ret_val);
}

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
qcpsup_status_type qcpsup_get_vrat (
  uint8   *buffer,
  uint32  *index,
  uint32  buf_len,
  uint32  *num_frames
) {
  qcpsup_status_type ret_val   = QCPSUP_STATUS_BUFF_ERROR;
  uint32             cur_index = 0;

  buffer = &buffer[*index];
  while((cur_index + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES) <= buf_len) {
    if(strncmp(QCPSUP_VRAT_ID, (char *) buffer, QCPSUP_CHUNK_ID_BYTES) == 0) {

      if((cur_index + QCPSUP_CHUNK_ID_BYTES + 4 + 8) <= buf_len) {
        qcpsup_read_length (&buffer[12], num_frames);
      } else {
        break;
      }

      ret_val = QCPSUP_STATUS_SUCCESS;
      break;

    } else {
      qcpsup_skip_chunk (&buffer, &cur_index);
    }
  }

  *index += cur_index;
  return(ret_val);
}

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
qcpsup_status_type qcpsup_get_cnfg (
  uint8   *buffer,
  uint32  *index,
  uint32  buf_len,
  uint32  *cnfg
) {
  qcpsup_status_type ret_val   = QCPSUP_STATUS_BUFF_ERROR;
  uint32             cur_index = 0;

  buffer = &buffer[*index];
  while((cur_index + QCPSUP_CHUNK_ID_BYTES + QCPSUP_LENGTH_BYTES) <= buf_len) {
    if(strncmp(QCPSUP_CNFG_ID, (char *) buffer, QCPSUP_CHUNK_ID_BYTES) == 0) {

      if((cur_index + QCPSUP_CHUNK_ID_BYTES + 4 + 2) <= buf_len) {
        *cnfg = (buffer[9] << 8) | buffer[8];
      } else {
        break;
      }

      ret_val = QCPSUP_STATUS_SUCCESS;
      break;

    } else {
      qcpsup_skip_chunk (&buffer, &cur_index);
    }
  }

  *index += cur_index;
  return(ret_val);
}

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
qcpsup_status_type qcpsup_do_head (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  uint32                   *length,
  qcpsup_format_enum_type  *format
) {
  uint32 file_size, local_index;
  uint8  riff_id[]  = {'Q', 'L', 'C', 'M'};
  uint8  chunk_id[] = {'d', 'a', 't', 'a'};

  // Checking NULL to prevent crashing
  if( buffer == NULL) {
    return(QCPSUP_STATUS_ERROR);
  }

  local_index = *index;
  if((qcpsup_do_riff(riff_id, buffer, &local_index, &file_size) ==
                                                   QCPSUP_STATUS_SUCCESS) &&
     (qcpsup_get_fmt(buffer, &local_index, buf_len, format) == 
                                                   QCPSUP_STATUS_SUCCESS) &&
     (qcpsup_find_chunk(chunk_id, buffer, &local_index, buf_len, length) == 
                                                   QCPSUP_STATUS_SUCCESS)) {
    *index = local_index;
    return(QCPSUP_STATUS_SUCCESS);
  } else {
    return(QCPSUP_STATUS_ERROR);
  }
}

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
qcpsup_status_type qcpsup_adpcm_do_head (
  uint8                    *buffer,
  uint32                   *index,
  uint32                   buf_len,
  uint32                   *length,
  qcpsup_adpcm_format_type *format
) {
  uint32 file_size, local_index;
  uint8  riff_id[]  = {'W', 'A', 'V', 'E'};
  uint8  chunk_id[] = {'d', 'a', 't', 'a'};

  // Checking NULL to prevent crashing
  if( buffer == NULL) {
    return(QCPSUP_STATUS_ERROR);
  }

  local_index = *index;
  if((qcpsup_do_riff(riff_id, buffer, &local_index, &file_size) ==
                                                   QCPSUP_STATUS_SUCCESS) &&
     (qcpsup_get_wave_fmt(buffer, &local_index, buf_len, format) ==
                                                   QCPSUP_STATUS_SUCCESS) &&
     (qcpsup_find_chunk(chunk_id, buffer, &local_index, buf_len, length) ==
                                                   QCPSUP_STATUS_SUCCESS)) {
    *index = local_index;
    return(QCPSUP_STATUS_SUCCESS);
  } else {
    return(QCPSUP_STATUS_ERROR);
  }
}
#endif /* FEATURE_QSYNTH_ADPCM */
#if defined(FEATURE_MIDI_OUT_QCP) && defined(FEATURE_GASYNTH_INTERFACE)
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
qcpsup_status_type qcpsup_do_amr_head
(
  uint8    *buffer,
  uint32   *index,
  uint32   buf_len
) {
  qcpsup_status_type  ret_val = QCPSUP_STATUS_ERROR;
  uint8               amr_header_id[] = {'#','!','A','M','R','\n'};  
  uint32              amr_header_len = sizeof(amr_header_id);

  if ((buffer != NULL) && (index != NULL) &&
      (buf_len >= amr_header_len)) {
    if (memcmp(amr_header_id, 
               buffer, 
               amr_header_len) == 0) {
      *index += amr_header_len;
      ret_val = QCPSUP_STATUS_SUCCESS;
    }
  } else {
    MSG_ERROR("NULL buffer ptr or index ptr", 0, 0, 0);
  }
 
  return (ret_val);
}
#endif /* FEATURE_MIDI_OUT_QCP && FEATURE_GASYNTH_INTERFACE */
#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP */
#endif /* FEATURE_QCP || FEATURE_WEBAUDIO || FEATURE_MIDI_OUT_QCP ||
          FEATURE_WAVE_PB */
