#ifndef AUDMP3I_H
#define AUDMP3I_H
/*===========================================================================

        A U D F M T   S E R V I C E S   M P 3   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface MP3
  files with AUDFMT Services.

Copyright (c) 2000-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmp3i.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
04/16/07    kk     Fixed :- CR 114511-VBR MP3 file without TOC seek problem. 
07/28/06    hz     Added support for ID3V2.
09/19/03    sg     Added support for straming data playback.
07/25/02    sg     Modified to use audmpeg services whereever possible.
07/13/01    sm     Added track_?_loading flags to show data is being loaded.
06/13/01    sm     Replaced track_?_empty with track_?_length.
11/27/00    sm     Updated for audmain*.  Added file info support, VBR.
11/03/00    sm     Changed ff_pause to boolean.
10/30/00    sm     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#ifdef FEATURE_MP3
#include "audmpeg.h"   /* Data common to all MPEG audio file types        */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* The following defines are used to extract all data from the MP3 header.
** Each is divided into
**  the byte offset into the header
**  the byte mask for the bits
**  the number of bits to right-shift to extract a 0-based value
*/
#define MP3HDR_VERSION_OFS 1
#define MP3HDR_VERSION_M 0x18
#define MP3HDR_VERSION_SHIFT 3

#define MP3HDR_LAYER_OFS 1
#define MP3HDR_LAYER_M 0x06
#define MP3HDR_LAYER_SHIFT 1

#define MP3HDR_CRC_OFS 1
#define MP3HDR_CRC_M 0x01
#define MP3HDR_CRC_SHIFT 0

#define MP3HDR_BITRATE_OFS 2
#define MP3HDR_BITRATE_M 0xF0
#define MP3HDR_BITRATE_SHIFT 4

#define MP3HDR_SAMPLERATE_OFS 2
#define MP3HDR_SAMPLERATE_M 0x0C
#define MP3HDR_SAMPLERATE_SHIFT 2

#define MP3HDR_PADDING_OFS 2
#define MP3HDR_PADDING_M 0x02
#define MP3HDR_PADDING_SHIFT 1

#define MP3HDR_PRIVATE_OFS 2
#define MP3HDR_PRIVATE_M 0x01
#define MP3HDR_PRIVATE_SHIFT 0

#define MP3HDR_CHANNEL_OFS 3
#define MP3HDR_CHANNEL_M 0xC0
#define MP3HDR_CHANNEL_SHIFT 6

#define MP3HDR_CHANNEL_EXT_OFS 3
#define MP3HDR_CHANNEL_EXT_M 0x30
#define MP3HDR_CHANNEL_EXT_SHIFT 4

#define MP3HDR_COPYRIGHT_OFS 3
#define MP3HDR_COPYRIGHT_M 0x08
#define MP3HDR_COPYRIGHT_SHIFT 3

#define MP3HDR_ORIGINAL_OFS 3
#define MP3HDR_ORIGINAL_M 0x06
#define MP3HDR_ORIGINAL_SHIFT 2

#define MP3HDR_EMPHASIS_OFS 3
#define MP3HDR_EMPHASIS_M 0x03
#define MP3HDR_EMPHASIS_SHIFT 0

/* Definitions for the number of samples contained within each frame of a MP3
** file, for all versions and compression schemes
*/
#define MPEG_1_FRAME_SIZE_LAYER_1 384
#define MPEG_1_FRAME_SIZE_LAYER_2_3 1152

#define MPEG_2_FRAME_SIZE_LAYER_1 (MPEG_1_FRAME_SIZE_LAYER_1 / 2)
#define MPEG_2_FRAME_SIZE_LAYER_2_3 (MPEG_1_FRAME_SIZE_LAYER_2_3 / 2)


/*===========================================================================

                       MP3 HEADER STRUCTURE DECLARATIONS

===========================================================================*/
/* MP3 header, as extracted from the first frame
*/
typedef struct {
  snd_mp3_ver_enum_type         version;
  snd_mpeg_layer_enum_type      layer;
  boolean                       crc_flag;
  snd_mp3_bitrate_enum_type     bitrate;
  snd_sample_rate_type          samplerate;
  boolean                       padding_flag;
  boolean                       private_flag;
  snd_mp3_channel_enum_type     channel;
  snd_mp3_ext_enum_type         extension;
  boolean                       copyright_flag;
  boolean                       original_flag;
  snd_mp3_emphasis_enum_type    emphasis;
} mp3_header_type;

/* VBR extraction info
*/
#define XING_FRAMES_FLAG    0x01
#define XING_BYTES_FLAG     0x02
#define XING_TOC_FLAG       0x04
#define XING_SCALE_FLAG     0x08

#define XING_TOC_LEN        100

typedef struct {
  uint32 flags;           /* VBR flags */
  int frames;             /* Number of frames in file */
  int bytes;              /* Number of bytes of data in file */
  int scale;              /* VBR scaling info */
  boolean toc_flag;       /* VBR TOC field is present */
  byte toc[XING_TOC_LEN]; /* VBR table of contents */
} vbr_header_type;

/* MP3 information structure, including the header, and all calculated data
*/
typedef struct {
  audmain_mp3_config_type config;     /* Details about the MP3 format   */
  mp3_header_type         hdr;
  vbr_header_type         vbr_hdr;
  audmpeg_id3_tag_type    id3_tag;
  uint32          frame_bytes;   /* Length, in bytes, of an MP3 frame         */
  uint32          num_frames;    /* Number of frames in the MP3 file          */
  uint32          frame_samples; /* Length, in PCM samples, of a decoded frame*/
  uint32          song_len_ms;   /* Length, in ms, of the MP3 song            */
  boolean         is_stream;
} mp3_info_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** Audmp3 Internal Function Prototypes
**---------------------------------------------------------------------------
*/
/*==========================================================================
 FUNCTION audmp3_copy_v2_to_v1

 DESCRPTION
  In case v2 is found but v1 is not existing, copy certain frames to v1 
  corresponding fields

 DEPENDENCIES
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None
===========================================================================*/
void audmp3_copy_v2_to_v1 (
  snd_mp3_spec_type      *snd_spec,
  audmpeg_id3v2_tag_type *v2_tag
);
#endif /* FEATURE_MP3 */
#endif /* AUDMP3I_H */
