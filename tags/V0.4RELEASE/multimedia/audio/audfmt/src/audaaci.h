#ifndef AUDAACI_H
#define AUDAACI_H
/*===========================================================================

        A U D F M T   S E R V I C E S   A A C   H E A D E R   F I L E

DESCRIPTION
  This header file contains all internal definitions necessary to interface
  MPEG-2 AAC files with AUDFMT Services.

Copyright (c) 2002 - 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audaaci.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/06    hz     Added support for AAC ADTS/VBR song length.
05/23/05    hz     Modified to adapt AAC recording change.
08/27/04    sg     Added macro for max frame size.
09/19/03    sg     Modified to support additional parameters required by the
                   DSP. Added support for a 'psuedo-raw' mode.
07/25/02    sg     Added support for ADIF format.
06/17/02    sg     Initial Version.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#ifdef FEATURE_AAC
#include "audmpeg.h"   /* Data common to all MPEG audio file types        */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* The following defines are used to extract all data from the AAC header.
** Each is divided into
**  the byte offset into the header
**  the byte mask for the bits
**  the number of bits to right-shift to extract a 0-based value
*/
#define AACHDR_VERSION_OFS            1
#define AACHDR_VERSION_M              0x08
#define AACHDR_VERSION_SHIFT          3

#define AACHDR_LAYER_OFS              1
#define AACHDR_LAYER_M                0x06
#define AACHDR_LAYER_SHIFT            1
#define AACHDR_LAYER_SIZE             2

#define AACHDR_CRC_OFS                1
#define AACHDR_CRC_M                  0x01
#define AACHDR_CRC_SHIFT              0
#define AACHDR_CRC_SIZE               1

#define AACHDR_PROFILE_OFS            2
#define AACHDR_PROFILE_M              0xC0
#define AACHDR_PROFILE_SHIFT          6
#define AAC_PROFILE_SIZE              2

#define AACHDR_SAMPLERATE_OFS         2
#define AACHDR_SAMPLERATE_M           0x3C
#define AACHDR_SAMPLERATE_SHIFT       2
#define AAC_SAMPLING_FREQ_INDEX_SIZE  4

#define AACHDR_PRIVATE_OFS            2
#define AACHDR_PRIVATE_M              0x02
#define AACHDR_PRIVATE_SHIFT          1

#define AACHDR_ORIGINAL_OFS           3
#define AACHDR_ORIGINAL_M             0x20
#define AACHDR_ORIGINAL_SHIFT         5
#define AAC_ORIGINAL_COPY_SIZE        1

#define AACHDR_HOME_OFS               3
#define AACHDR_HOME_M                 0x10
#define AACHDR_HOME_SHIFT             4
#define AAC_HOME_SIZE                 1


#define AACHDR_COPYRIGHT_BIT_OFS      3
#define AACHDR_COPYRIGHT_BIT_M        0x08
#define AACHDR_COPYRIGHT_BIT_SHIFT    3
#define AAC_COPYRIGHT_PRESENT_SIZE    1

#define AACHDR_COPYRIGHT_START_OFS    3
#define AACHDR_COPYRIGHT_START_M      0x04
#define AACHDR_COPYRIGHT_START_SHIFT  2

#define AACHDR_NUM_DATA_BLOCKS_OFS    6
#define AACHDR_NUM_DATA_BLOCKS_M      0x03
#define AACHDR_NUM_DATA_BLOCKS_SHIFT  0

#define AAC_BITSTREAM_TYPE_SIZE               1
#define AAC_BITRATE_SIZE                     23
#define AAC_NUM_PFE_SIZE                      4
#define AAC_BUFFER_FULLNESS_SIZE             20
#define AAC_ELEMENT_INSTANCE_TAG_SIZE         4
#define AAC_NUM_FRONT_CHANNEL_ELEMENTS_SIZE   4
#define AAC_NUM_SIDE_CHANNEL_ELEMENTS_SIZE    4
#define AAC_NUM_BACK_CHANNEL_ELEMENTS_SIZE    4
#define AAC_NUM_LFE_CHANNEL_ELEMENTS_SIZE     2
#define AAC_NUM_ASSOC_DATA_ELEMENTS_SIZE      3
#define AAC_NUM_VALID_CC_ELEMENTS_SIZE        4
#define AAC_MONO_MIXDOWN_PRESENT_SIZE         1
#define AAC_MONO_MIXDOWN_ELEMENT_SIZE         4
#define AAC_STEREO_MIXDOWN_PRESENT_SIZE       1
#define AAC_STEREO_MIXDOWN_ELEMENT_SIZE       4
#define AAC_MATRIX_MIXDOWN_PRESENT_SIZE       1
#define AAC_MATRIX_MIXDOWN_SIZE               3
#define AAC_FCE_SIZE                          5
#define AAC_SCE_SIZE                          5
#define AAC_BCE_SIZE                          5
#define AAC_LFE_SIZE                          4
#define AAC_ADE_SIZE                          4
#define AAC_VCE_SIZE                          5
#define AAC_COMMENT_FIELD_BYTES_SIZE          8
#define AAC_COMMENT_FIELD_DATA_SIZE           8


/* The number of samples contained within each frame of a AAC file.
*/
#define AAC_SAMPLES_PER_DATA_BLOCK 1024
#define AAC_MAX_FRAME_SIZE 768*4 /* Includes worst case for aacPlus */

/* Minimum ADTS frame header size - 7 bytes, excluding CRC byte 
 * if there is any.
 */
#define AUDAAC_ADTS_MIN_HEADER_LEN 7

/*===========================================================================

                       AAC HEADER STRUCTURE DECLARATIONS

===========================================================================*/

/* AAC header, as extracted from the first frame
*/
typedef struct {
  /* Fixed header - doesn't change from frame to frame */
  /* snd_aac_id_enum_type           version; */
  snd_mpeg_layer_enum_type          layer;
  boolean                           crc_flag;
  snd_sample_rate_type              samplerate;
  boolean                           private_flag;
  boolean                           original_flag;
  boolean                           home_flag;

  /* Variable header - changes from frame to frame */
  /* UNUSED
  byte                         copyright_bit;
  byte                         copyright_start;
  uint16                       frame_length;
  byte                         num_raw_data_blocks;
  */

  uint32                      bit_rate;
  boolean                     variable_bitrate; /* TRUE = variable bit rate */

} aac_header_type;

/* AAC information structure, including the header, and all calculated data
*/
typedef struct {
  audmain_aac_config_type config;         /* Details about the AAC format   */
  aac_header_type         hdr;            /* Header Info                    */
  audmpeg_id3_tag_type    id3_tag;        /* ID3 TAG - only if not PV stream*/
  uint32                  song_len_ms;    /* Length, in ms, of the AAC song */
} aac_info_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** Audaac Internal Function Prototypes
**---------------------------------------------------------------------------
*/
/*===========================================================================

FUNCTION audaac_adts_parse_frame

DESCRIPTION
  Parsed a buffer to get the number of ADTS frame number.


DEPENDENCIES
  None.

RETURN VALUE
  Length of the buffer parsed.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 audaac_adts_parse_frame (
  uint8   *buf,
  uint32  buf_len,
  boolean play_back
);
#endif /* FEATURE_AAC */
#endif /* AUDAACI_H */
