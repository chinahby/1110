/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Audmp3 Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             A U D I O   F O R M A T s   M P 3   S E R V I C E S

GENERAL DESCRIPTION
  This module contains AudMP3 services software.

EXTERNALIZED FUNCTIONS
  audmp3_format_play
    Plays back streaming MP3 data.
  audmp3_parse_head
    Parses the mp3 header for relevant infomation.
  audmp3_get_data_cb_req
    Callback function which handles the response to a Data request.
  audmp3_get_sample_rate
    Returns the sampling rate of the current mp3 song.
  audmp3_flush
    Flushes the buffers and restarts playback.
  audmp3_age
    Checks to see if the buffers are ready for use.
  audmp3_fforward
    Moves the MP3 playback forward ms milliseconds.
  audmp3_rewind
    Moves the MP3 playback backward ms milliseconds.
  audmp3_reset
    Resets internal MP3 buffers after a song is unloaded.
  audmp3_get_length
    Returns the total duration of the selected MP3 file.
  audmp3_get_song_config
    Returns song-specific information to optimize MP3 playback.
  audmp3_adec_adjust
    Adjusts adec buffers to reflect their newly used data.
  audmp3_parse
    Parses one event of the MP3 file.  Prepares buffers for data transfer.
  audmp3_age
    Checks to see if the buffers are ready for use.

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c) 2000 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmp3.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/09    KK     Fixed CR 168161 - When Rewind operation was done, Observed 
                   AbError after 1Sec for all the (MP3-8kbps) clips & 
                   FastForward not working 
11/21/08    kk     Added support for to play short MP3 files 
                   (< 1sec or file size, excluding ID3 tag is <2500)-CR 162679.
06/10/08    kk     Fixed CR 148311 - Incorrect song length (num_frames) 
                   calculation in "audmp3_calc_song_len_ms" func.
03/11/08    kk     Added support for sending mp3 configuration 
                   information for DSP. 
02/20/08    hs     Fixed crash because of NULL ptr, andding MSG_LOW
10/06/09    hs     Fixed memory leakage on gframes
08/30/07    hs     Added audmp3_unicode_to_ascii(), audmp3_copyframe_v2_to_v1
                   to copy v2 to v1 considering encoding type
08/06/07    kk     Merged the Fix CR 124244 - Full ID3V2.3 support, 
                   support for URL link (W)frames .
05/23/07    kk     Removed F3 message from the while loop for Mp3 files.
05/23/07    kk     Fixed  CR 113932 - CMX get spec problem due to APIC 
                   frame in MP3 ID3V2.3 tag.
05/14/07    hs     Fixed CR 118480 - ID3Tag V2 fixes
05/11/07    hs     Fixed CR 117907 - crash and memory leak
04/25/07    anb    Fixed a compilation issue in audmp3_rewind()
04/16/07    kk     Fixed :- CR 114511-VBR MP3 file without TOC seek problem. 
03/13/07    az     Fixed the CR: 112008 array off-by-one error.
03/05/07    az     Fixed the CR: 112175 memory leak when playing MP3(ID3v2) and
                   AAC files.
01/15/07    kk     Fixed issue while extracting the ID3V1 tag information.
11/07/06    kk     corrected the Fix for the wrong calculation of VBR MP3 files.
09/26/06    az     Fixed the issue regarding the crash during mp3 file playback.
08/17/06    hz     Fixed CR#95733 - MP3 file spec terminates fs handle. 
08/15/06    sg     Modified to use uint32 for find_sync API.
08/01/06    hz     Fixed a mis-merging bug which causes VBR FFWD and 
                   continous playback error.
07/28/06    hz     Added support for ID3V2.
07/28/06    az     Modified the code to resolve the CR 97439.
04/12/06    KK     Modified audmp3_calc_song_len_ms to fix the wrong calculation 
                   of playback time for VBR mp3 clips.
01/26/06    hz     Modified audmp3_calc_song_len_ms to fix CR86401.
12/28/05    hz     Modified to make MP3 playback time calculated more accurate.
                   Fixed CR80340.
10/20/05    ay     Fixed audmp3_calc_byte_offset() from returning 0ms position
                   when current position + seek amount = 1000ms.
07/19/05    pl     Modified audmp3_parse_head() to initialize error status to
                   AUDMAIN_STATUS_MAX meaning that no error has been found.
07/08/05    pl     Modified audmp3_do_file_spec() to set supported operation
                   bit mask.
07/07/05    hz     Added support for MP3 version 2.5.
07/06/05    hz     Applied mp3_frame_mask when do header fields extract.
05/24/05    sg     Clearified a build warning.
05/17/05    sg     Modified to not return elapsed time greater than file
                   duration if seek is attempted beyond the file (CR63283).
05/12/05    sp     Changed mp3_frame_mask so that DSP properly plays files with
                   the mode in frame header changing from frame to frame.
04/23/05    ac     Revised the audmp3_find_sync() to include a boolean argument
                   for the first buffer
08/11/04    sg     Modified to use macros when copying ID3 tag strings.
04/23/04    sg     Modified to fail sync if data is invalid or not supported.
                   Modified to reject fforward/rewind if song_len is unknown.
03/05/04    sg     Enhanced find_sync() to allow it skip garbage data at the
                   beginning of a file.
11/23/03    sg     Modified song length calculation to prevent overflow errors.
09/19/03    sg     Added support for streaming interface.
03/13/03    sg     Added support to play files which have a bunch of zeroes at
                   the beginning of the song.
07/25/02    sg     Reduced MSG level to HIGH for invalid file message.
06/17/02    sg     Modified to use audmpeg services whereever possible.
                   Added audmp3_flush.
03/21/02    st     Fixed access to mp3_info structure.
02/20/02    sg     Fixed problem where playing would continue beyond the end of
                   a song if the fast forward button was pressed rapidly.
10/01/01    st     Modified audmp3_adec_adjust() function to make bytes_used
                   passed by reference.
07/13/01    sm     Fixed problem that songs < 1600 bytes and odd length
                   wouldn't play.  Queues 2nd track in parse head.
06/13/01    sm     Replaced track_?_empty with track_?_length.  Ensure all data
                   is sent to DSP before terminating playback.
04/26/01    sm     Removed speed calculation from audmp3_get_song_config().
04/04/01    st     Added length parameter to audmp3_get_data_cb_req();
                   modified code to use new audmain_cmd_buf_type.
12/10/00    st     Added #include "comdef.h" to remove compiler warning when
                   FEATURE_MP3 is not defined.
12/04/00    sm     Added audmp3_supported_mp3(), _do_time(), _get_song_config(),
                   Check read_failed in parse.  Renamed ff/rew functions.
11/27/00    sm     Updated for audmain*.  Added file info support.
11/12/00    sm     Fixed read/read_cb for swapped buffers.  Improved ff/rew
11/03/00    sm     Added get_pause/set_pause/set_ff_pause().
10/30/00    sm     Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "comdef.h"           /* Customer specific definitions           */
#include "customer.h"           /* Customer specific definitions           */
#ifdef FEATURE_MP3
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include <memory.h>             /* Memory manipulation routines            */

#include "audmain.h"            /* Audio Format Data definitions and protos*/
#include "audmp3.h"             /* MP3 typedefs and prototypes             */
#include "audmp3i.h"            /* MP3 internal typedefs and prototypes    */
#include "audmpeg.h"            /* MPEG typedefs and prototypes            */
#include "snd.h"

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDMP3 General Defines
**---------------------------------------------------------------------------
*/
/* This structure contains all the information about a MP3 file
** that is being parsed.
*/
audmpeg_parse_ctl_type audmp3_parse_ctl;

/* mp3_info contains all relevant information about the currently playing MP3
** file, extracted from the header.
*/
mp3_info_type mp3_info;

/* Each MP3 frame contains sync data and unchanging header data which can be
** searched for to more accurately locate the next sync.
*/
const byte mp3_frame_mask[4] = {0xFF,0xFE,0x0C,0x00};
byte mp3_frame_data[4];
boolean mp3_frame_data_found = FALSE;

/* The bitrate conversion table
** mp3_bitrate_table[mapped_version][layer+1][rate]
*/
const snd_mp3_bitrate_enum_type mp3_bitrate_table[2][3][16] = {
  /* MP3 Version 1.0 */
  {
    /* Layer 1 */
    {
      SND_MP3_BITRATE_FREE, SND_MP3_BITRATE_32K,
        SND_MP3_BITRATE_64K,  SND_MP3_BITRATE_96K,

      SND_MP3_BITRATE_128K, SND_MP3_BITRATE_160K,
        SND_MP3_BITRATE_192K, SND_MP3_BITRATE_224K,

      SND_MP3_BITRATE_256K, SND_MP3_BITRATE_288K,
        SND_MP3_BITRATE_320K, SND_MP3_BITRATE_352K,

      SND_MP3_BITRATE_384K, SND_MP3_BITRATE_416K,
        SND_MP3_BITRATE_448K, SND_MP3_BITRATE_UNK
    },
    /* Layer 2 */
    {
      SND_MP3_BITRATE_FREE, SND_MP3_BITRATE_32K,
        SND_MP3_BITRATE_48K,  SND_MP3_BITRATE_56K,

      SND_MP3_BITRATE_64K,  SND_MP3_BITRATE_80K,
        SND_MP3_BITRATE_96K,  SND_MP3_BITRATE_112K,

      SND_MP3_BITRATE_128K, SND_MP3_BITRATE_160K,
        SND_MP3_BITRATE_192K, SND_MP3_BITRATE_224K,

      SND_MP3_BITRATE_256K, SND_MP3_BITRATE_320K,
        SND_MP3_BITRATE_384K, SND_MP3_BITRATE_UNK
    },
    /* Layer 3 */
    {
      SND_MP3_BITRATE_FREE, SND_MP3_BITRATE_32K,
        SND_MP3_BITRATE_40K,  SND_MP3_BITRATE_48K,

      SND_MP3_BITRATE_56K,  SND_MP3_BITRATE_64K,
        SND_MP3_BITRATE_80K,  SND_MP3_BITRATE_96K,

      SND_MP3_BITRATE_112K, SND_MP3_BITRATE_128K,
        SND_MP3_BITRATE_160K, SND_MP3_BITRATE_192K,

      SND_MP3_BITRATE_224K, SND_MP3_BITRATE_256K,
        SND_MP3_BITRATE_320K, SND_MP3_BITRATE_UNK
    }
  },
  /* MP3 Version 2.5, RESERVED, 2.0 */
  {
    /* Layer 1 */
    {
      SND_MP3_BITRATE_FREE, SND_MP3_BITRATE_32K,
        SND_MP3_BITRATE_48K,  SND_MP3_BITRATE_56K,

      SND_MP3_BITRATE_64K,  SND_MP3_BITRATE_80K,
        SND_MP3_BITRATE_96K,  SND_MP3_BITRATE_112K,

      SND_MP3_BITRATE_128K, SND_MP3_BITRATE_144K,
        SND_MP3_BITRATE_160K, SND_MP3_BITRATE_176K,

      SND_MP3_BITRATE_192K, SND_MP3_BITRATE_224K,
        SND_MP3_BITRATE_256K, SND_MP3_BITRATE_UNK
    },
    /* Layer 2 */
    {
      SND_MP3_BITRATE_FREE, SND_MP3_BITRATE_8K,
        SND_MP3_BITRATE_16K,  SND_MP3_BITRATE_24K,

      SND_MP3_BITRATE_32K,  SND_MP3_BITRATE_40K,
        SND_MP3_BITRATE_48K,  SND_MP3_BITRATE_56K,

      SND_MP3_BITRATE_64K,  SND_MP3_BITRATE_80K,
        SND_MP3_BITRATE_96K,  SND_MP3_BITRATE_112K,

      SND_MP3_BITRATE_128K, SND_MP3_BITRATE_144K,
        SND_MP3_BITRATE_160K, SND_MP3_BITRATE_UNK
    },
    /* Layer 3 */
    {
      SND_MP3_BITRATE_FREE, SND_MP3_BITRATE_8K,
        SND_MP3_BITRATE_16K,  SND_MP3_BITRATE_24K,

      SND_MP3_BITRATE_32K,  SND_MP3_BITRATE_40K,
        SND_MP3_BITRATE_48K,  SND_MP3_BITRATE_56K,

      SND_MP3_BITRATE_64K,  SND_MP3_BITRATE_80K,
        SND_MP3_BITRATE_96K,  SND_MP3_BITRATE_112K,

      SND_MP3_BITRATE_128K, SND_MP3_BITRATE_144K,
        SND_MP3_BITRATE_160K, SND_MP3_BITRATE_UNK
    }
  }
};

/* samplerate_table[version][rate] */
const snd_sample_rate_type mp3_samplerate_table[4][4] = {
  /* MP3 Version 2.5 */
  { SND_SAMPLE_RATE_11025, SND_SAMPLE_RATE_12000,
      SND_SAMPLE_RATE_8000, SND_SAMPLE_RATE_UNKNOWN },
  /* MP3 Version RESERVED */
  { SND_SAMPLE_RATE_UNKNOWN, SND_SAMPLE_RATE_UNKNOWN,
      SND_SAMPLE_RATE_UNKNOWN, SND_SAMPLE_RATE_UNKNOWN },
  /* MP3 Version 2.0 */
  { SND_SAMPLE_RATE_22050, SND_SAMPLE_RATE_24000,
      SND_SAMPLE_RATE_16000, SND_SAMPLE_RATE_UNKNOWN },
  /* MP3 Version 1.0 */
  { SND_SAMPLE_RATE_44100, SND_SAMPLE_RATE_48000,
      SND_SAMPLE_RATE_32000, SND_SAMPLE_RATE_UNKNOWN }
};

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudMP3 Macro definitions
**---------------------------------------------------------------------------
*/
#define IS_SYNC(x) \
  (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xE0) == 0xE0)

#define IS_VALID_FRAME(x) \
  ((((x)[0] & mp3_frame_mask[0]) == mp3_frame_data[0]) && \
   (((x)[1] & mp3_frame_mask[1]) == mp3_frame_data[1]) && \
   (((x)[2] & mp3_frame_mask[2]) == mp3_frame_data[2]) && \
   (((x)[3] & mp3_frame_mask[3]) == mp3_frame_data[3]))

#define EXTRACT_I4(x) \
  ((uint32)(x)[0] << 24 | (uint32)(x)[1] << 16 | (uint32)(x)[2] << 8 | \
                                                                 (uint32)(x)[3])
#define IS_VBR (mp3_info.vbr_hdr.flags != 0)

  #define EOF_PADDING_BUFFER_SIZE 1500

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudMP3 Internal Function definitions
**---------------------------------------------------------------------------
*/
/*==========================================================================
 FUNCTION audmp3_unicode_to_ascii

 DESCRPTION
  convert unicode string to plain text within size. For now make it static.

 DEPENDENCIES
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None
===========================================================================*/
LOCAL void audmp3_unicode_to_ascii(char *pDest, uint16 *pSrc , uint32 size)
{
  for( ; size != 0; size--, pDest++,  pSrc++)
  {
    *pDest = *((char *)pSrc);
  }
}

/*==========================================================================
 FUNCTION audmp3_copyframe_v2_to_v1

 DESCRPTION
  copy v2 frame to v1 frame considering encoding type. This function is
  introduced to avoid reduce repeatation of same code. For now make it static.

 DEPENDENCIES
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None
===========================================================================*/
LOCAL void audmp3_copyframe_v2_to_v1(char *pDest, 
                        audmpeg_id3v2_frame_type *pframe, const uint32 max_size)
{
  uint32 frame_len;

  if( *(pframe->frame_content) == (uint8)0x01 ) {
    /* Unicode */
    uint16 *pload16;

    /* count plain text length only after remove header */
    frame_len = (pframe->frame_size-3)/2;
    frame_len = (frame_len > max_size ? max_size: frame_len);
    pload16 = (uint16 *)(pframe->frame_content+3);  /* remove 3 byte header */
    audmp3_unicode_to_ascii( pDest, pload16, frame_len );
  }
  else if( *(pframe->frame_content) == (uint8)0x00 ) {
    /* Plain text */
    uint8* pload8 = pframe->frame_content + 1;
    frame_len = pframe->frame_size -1;
    frame_len = (frame_len > max_size ? max_size: frame_len);
    memcpy(pDest, pload8, frame_len);
  }
  else {
    MSG_ERROR("Not supported encoding type",0,0,0);
  }

}


/*===========================================================================

FUNCTION audmp3_extract_vbr_header

DESCRIPTION
  This function extracts the VBR header from the first frame of MP3 data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audmp3_extract_vbr_header(
  byte *data,
  mp3_header_type *hdr_ptr,
  vbr_header_type *vbr_ptr
)
{
  int counter = XING_TOC_LEN;
  byte *toc_ptr;

  /* Clear VBR flags register */
  vbr_ptr->flags = 0;

  /* VBR header is located in different areas based on MPEG version and
  ** channel.  Skip over unused data area.
  */
  if(hdr_ptr->version == SND_MP3_VER_1) {
    if(hdr_ptr->channel == SND_MP3_CHANNEL_SINGLE) {
      data += 21;
    } else {
      data += 36;
    }
  }
  if(hdr_ptr->version == SND_MP3_VER_2 ||
     hdr_ptr->version == SND_MP3_VER_25) {
    if(hdr_ptr->channel == SND_MP3_CHANNEL_SINGLE) {
      data += 13;
    } else {
      data += 21;
    }
  }

  /* Search for "Xing" header */
  if(data[0] != 0x58 || data[1] != 0x69 || data[2] != 0x6e || data[3] != 0x67) {
    /* No header found, flags = 0 == no VBR */
    return;
  }
  data += 4;
  /* Store flags register */
  vbr_ptr->flags = EXTRACT_I4(data);
  data += 4;
  if(vbr_ptr->flags & XING_FRAMES_FLAG) {
    /* If frames flag set, store frames register */
    vbr_ptr->frames = EXTRACT_I4(data);
    data += 4;
  }
  if(vbr_ptr->flags & XING_BYTES_FLAG) {
    /* If bytes flag set, store bytes register */
    vbr_ptr->bytes = EXTRACT_I4(data);
    data += 4;
  }
  if(vbr_ptr->flags & XING_TOC_FLAG) {
    /* If TOC flag set, store TOC */
    vbr_ptr->toc_flag = TRUE;
    toc_ptr = vbr_ptr->toc;
    do {
      *toc_ptr++ = *data++;
    } while(--counter);
  }
  if(vbr_ptr->flags & XING_SCALE_FLAG) {
    /* If scale flag set, store scale register */
    vbr_ptr->scale = EXTRACT_I4(data);
  } else {
    vbr_ptr->scale = -1;
  }
}

/*===========================================================================

FUNCTION audmp3_extract_mp3_header

DESCRIPTION
  This function extracts the MP3 header from the start of data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audmp3_extract_mp3_header(
  byte            *data,
  mp3_info_type   *mp3_info
) {

  mp3_info->hdr.version = (snd_mp3_ver_enum_type)
    ((data[MP3HDR_VERSION_OFS] & MP3HDR_VERSION_M) >> MP3HDR_VERSION_SHIFT);
  mp3_info->hdr.layer = (snd_mpeg_layer_enum_type)
    ((data[MP3HDR_LAYER_OFS] & MP3HDR_LAYER_M) >> MP3HDR_LAYER_SHIFT);
  mp3_info->hdr.crc_flag = (boolean)
    ((data[MP3HDR_CRC_OFS] & MP3HDR_CRC_M) >> MP3HDR_CRC_SHIFT);
  mp3_info->hdr.bitrate = (snd_mp3_bitrate_enum_type)
    mp3_bitrate_table[(mp3_info->hdr.version == SND_MP3_VER_1 ? 0 : 1)]
                     [mp3_info->hdr.layer+1]
                     [((data[MP3HDR_BITRATE_OFS] &
                      MP3HDR_BITRATE_M) >> MP3HDR_BITRATE_SHIFT)];
  mp3_info->hdr.samplerate = (snd_sample_rate_type)
  mp3_samplerate_table[mp3_info->hdr.version]
                        [((data[MP3HDR_SAMPLERATE_OFS] &
                         MP3HDR_SAMPLERATE_M) >> MP3HDR_SAMPLERATE_SHIFT)];
  mp3_info->hdr.padding_flag = (boolean)
    ((data[MP3HDR_PADDING_OFS] & MP3HDR_PADDING_M) >> MP3HDR_PADDING_SHIFT);
  mp3_info->hdr.private_flag = (boolean)
    ((data[MP3HDR_PRIVATE_OFS] & MP3HDR_PRIVATE_M) >> MP3HDR_PRIVATE_SHIFT);
  mp3_info->hdr.channel = (snd_mp3_channel_enum_type)
    ((data[MP3HDR_CHANNEL_OFS] & MP3HDR_CHANNEL_M) >> MP3HDR_CHANNEL_SHIFT);
  mp3_info->hdr.extension = (snd_mp3_ext_enum_type)
    ((data[MP3HDR_CHANNEL_EXT_OFS] & MP3HDR_CHANNEL_EXT_M) >> MP3HDR_CHANNEL_EXT_SHIFT);
  mp3_info->hdr.copyright_flag = (boolean)
    ((data[MP3HDR_COPYRIGHT_OFS] & MP3HDR_COPYRIGHT_M) >> MP3HDR_COPYRIGHT_SHIFT);
  mp3_info->hdr.original_flag = (boolean)
    ((data[MP3HDR_ORIGINAL_OFS] & MP3HDR_ORIGINAL_M) >> MP3HDR_ORIGINAL_SHIFT);
  mp3_info->hdr.emphasis = (snd_mp3_emphasis_enum_type)
    ((data[MP3HDR_EMPHASIS_OFS] & MP3HDR_EMPHASIS_M) >> MP3HDR_EMPHASIS_SHIFT);

  /* mp3_frame_data holds the unchanging area of the mp3 frame header that
  ** can be scanned for to find the next valid frame.  The sync is included.
  */
  mp3_frame_data[0] = (0xFF) & (mp3_frame_mask[0]);
  mp3_frame_data[1] = (0xE0 | (data[MP3HDR_VERSION_OFS] & MP3HDR_VERSION_M)
                            | (data[MP3HDR_LAYER_OFS] & MP3HDR_LAYER_M)) &
                      (mp3_frame_mask[1]);
  mp3_frame_data[2] = (data[MP3HDR_SAMPLERATE_OFS] & MP3HDR_SAMPLERATE_M) &
                      (mp3_frame_mask[2]);
  mp3_frame_data[3] = (data[MP3HDR_CHANNEL_OFS] & MP3HDR_CHANNEL_M) &
                      (mp3_frame_mask[3]);
  /* this is required for DSP configuration*/
  MSG_MED("samplerate index = %d",mp3_info->hdr.samplerate,0,0);
  switch(mp3_info->hdr.samplerate)
  {
    case SND_SAMPLE_RATE_NONE:
      mp3_info->config.sample_freq = 0000;
      break;
    case SND_SAMPLE_RATE_8000:
      mp3_info->config.sample_freq = 8000;
      break;
    case SND_SAMPLE_RATE_11025:
      mp3_info->config.sample_freq = 11025;
      break;
    case SND_SAMPLE_RATE_12000:
      mp3_info->config.sample_freq = 12000;
      break;
    case SND_SAMPLE_RATE_16000:
      mp3_info->config.sample_freq = 16000;
      break;
    case SND_SAMPLE_RATE_22050:
      mp3_info->config.sample_freq = 22050;
      break;
    case SND_SAMPLE_RATE_24000:
      mp3_info->config.sample_freq = 24000;
      break;
    case SND_SAMPLE_RATE_32000:
      mp3_info->config.sample_freq = 32000;
      break;
    case SND_SAMPLE_RATE_44100:
      mp3_info->config.sample_freq = 44100;
      break;
    case SND_SAMPLE_RATE_48000:
      mp3_info->config.sample_freq = 48000;
      break;    
    default:
      mp3_info->config.sample_freq = 0;
      break;
  }
  MSG_MED("samplefreq = %d",mp3_info->config.sample_freq,0,0);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_is_valid_mp3

DESCRIPTION
  This function checks the header of the MP3 file to verify it is valid.
  Note that not all valid MP3 files are supported.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the file is supported.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audmp3_is_valid_mp3(
  mp3_header_type *hdr_ptr
)
{
  mp3_frame_data_found = TRUE;
  if (hdr_ptr->version == SND_MP3_VER_RESERVED ||
      hdr_ptr->version >= SND_MP3_VER_UNKNOWN  ||

      hdr_ptr->layer == SND_MP3_LAYER_RESERVED ||
      hdr_ptr->layer >= SND_MP3_LAYER_UNKNOWN  ||

      hdr_ptr->bitrate == SND_MP3_BITRATE_FREE ||
      hdr_ptr->bitrate >= SND_MP3_BITRATE_UNK  ||

      hdr_ptr->samplerate == SND_SAMPLE_RATE_NONE ||
      hdr_ptr->samplerate > SND_SAMPLE_RATE_48000) {
    mp3_frame_data_found = FALSE;
  }
  return mp3_frame_data_found;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_supported_mp3

DESCRIPTION
  This function checks the header of the MP3 file to verify it is supported.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the file is supported.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean audmp3_supported_mp3(
  mp3_header_type *hdr_ptr
)
{
  mp3_frame_data_found = FALSE;
  if( ( (hdr_ptr->version == SND_MP3_VER_2) ||
        (hdr_ptr->version == SND_MP3_VER_1) ||
        (hdr_ptr->version == SND_MP3_VER_25)
      ) &&

      (hdr_ptr->layer == SND_MP3_LAYER_3) &&

      (hdr_ptr->bitrate != SND_MP3_BITRATE_FREE) &&
      (hdr_ptr->bitrate != SND_MP3_BITRATE_UNK) &&

      (hdr_ptr->samplerate > SND_SAMPLE_RATE_NONE) &&
      (hdr_ptr->samplerate <= SND_SAMPLE_RATE_48000)
    ) {
    mp3_frame_data_found = TRUE;
  }
  return mp3_frame_data_found;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_calc_frame_bytes

DESCRIPTION
  This function calculates the length of an MP3 frame based on information
  stored in the MP3's header.

DEPENDENCIES
  None.

RETURN VALUE
  the frame length in bytes

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint16 audmp3_calc_frame_bytes(
  mp3_header_type *hdr_ptr,
  vbr_header_type *vbr_ptr
) {
  uint32 frame_bytes;
  uint32 temp_bytes;
  
    if(IS_VBR) {
      if(mp3_info.vbr_hdr.bytes == 0) {            
        frame_bytes = 0;
      } else {
        frame_bytes = mp3_info.vbr_hdr.frames / mp3_info.vbr_hdr.bytes;
      }
	} else {
    /* Try not to introduce much error from division.
    ** We don't take the padding bit into account for the frame calc to ensure
    ** we generate a minimum frame length to skip over.
    */
    frame_bytes = (((uint32)144 * hdr_ptr->bitrate * (uint32)1000) * 10 /
                  (uint32)audmpeg_samplerate_values_table[hdr_ptr->samplerate]);

    temp_bytes  = (uint32)(frame_bytes / 10);

    if ((frame_bytes - temp_bytes * 10) > 5) {
      frame_bytes = (frame_bytes / 10) + 1;
    } else {
      frame_bytes = (frame_bytes) / 10;
    }

    /* MP3 Layer 1 frames are one-third the size Layer 2 & 3 frames. */
    if(hdr_ptr->layer == SND_MP3_LAYER_1) {
      frame_bytes /= 3;
    }

    /* MP3 2.0 frames are one-half the size of 1.0 frames */
    if(hdr_ptr->version == SND_MP3_VER_2 ||
       hdr_ptr->version == SND_MP3_VER_25) {
      frame_bytes /= 2;
    }
  }
  return((uint16)frame_bytes);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_calc_frame_samples

DESCRIPTION
  This function calculates the length, in samples, of a frame.

DEPENDENCIES
  None.

RETURN VALUE
  the frame length in samples

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint32 audmp3_calc_frame_samples(
  mp3_header_type *hdr_ptr
)
{
  uint32 size;

  /* Frame samples are based on version and layer */
  if(hdr_ptr->version == SND_MP3_VER_1) {
    if(hdr_ptr->layer == SND_MP3_LAYER_1) {
      size = MPEG_1_FRAME_SIZE_LAYER_1;
    } else {
      size = MPEG_1_FRAME_SIZE_LAYER_2_3;
    }
  } else {
    if(hdr_ptr->layer == SND_MP3_LAYER_1) {
      size = MPEG_2_FRAME_SIZE_LAYER_1;
    } else {
      size = MPEG_2_FRAME_SIZE_LAYER_2_3;
    }
  }

  return(size);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_calc_song_len_ms

DESCRIPTION
  This function calculates the length of the song in milliseconds.

DEPENDENCIES
  None.

RETURN VALUE
  length of the song in milliseconds

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint32 audmp3_calc_song_len_ms(
  mp3_header_type  *hdr_ptr,
  vbr_header_type  *vbr_ptr,
  uint32           frame_bytes,
  uint32           song_len_bytes,
  uint32           frame_samples
) {
  uint32 song_len_ms;
  uint64 num_frames;
  uint64 temp_num_frames;
  uint64 temp_len_ms;

  uint32 end_tag_size = 0;

  /* when we calculated the song length in milisecond, only consider the tag
   * at the end of the file. Becuae fisrt_sync has already excluded the 
   * the tag size at beginning of the file.
   */
  if ((mp3_info.id3_tag.v2_tag_found == TRUE) &&
      ((mp3_info.id3_tag.v2_tag.tag_locate == 
        ID3V2_TAG_LOCATE_PREPEND_POSTPEND) ||
             (mp3_info.id3_tag.v2_tag.tag_locate == 
        ID3V2_TAG_LOCATE_POSTPEND_ONLY))) {

    end_tag_size = mp3_info.id3_tag.v2_tag.postpend_size;
  }

  if (mp3_info.id3_tag.v1_tag_found == TRUE) {
    
    end_tag_size += AUDMPEG_ID3V1_TAG_LEN; 
  }

  /* Calculate the number of frames */
  if(IS_VBR) {
    num_frames = vbr_ptr->frames;
    song_len_ms = (num_frames * frame_samples) / 
         (audmpeg_samplerate_values_table[hdr_ptr->samplerate]/1000);

  } else {
    num_frames = ((uint64)(song_len_bytes - 
      (audmp3_parse_ctl.first_sync + end_tag_size)) * 10) / frame_bytes;
    
    temp_num_frames = num_frames / 10;

    if ((num_frames - temp_num_frames * 10) > 5) {
      num_frames = (num_frames / 10) + 1;
    } else {
      num_frames = num_frames / 10;
    }

    /* This arrangement of math operations takes care of overflow errors,
     * to make more accruate calculation */
    temp_len_ms = num_frames * 1000;

    if (temp_len_ms < 
        (uint64)(audmpeg_samplerate_values_table[hdr_ptr->samplerate])) {
  
      temp_len_ms = (temp_len_ms * frame_samples * 10) / 
        (audmpeg_samplerate_values_table[hdr_ptr->samplerate]);

      temp_len_ms = (temp_len_ms / 10);
    } else {

      temp_len_ms = ((temp_len_ms * frame_samples) / 
          (audmpeg_samplerate_values_table[hdr_ptr->samplerate]));

    }

    song_len_ms = (uint32)temp_len_ms;

  }
  return (song_len_ms);
}
 
/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_do_file_spec

DESCRIPTION
  This function builds a sound file specifications buffer and transmits the
  data to the CMX layer through the playback callback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audmp3_do_file_spec (
  snd_af_access_type          *handle
)
{
  snd_mp3_spec_type snd_spec;
  int cnt = 0;
  memset(&snd_spec, 0, sizeof(snd_mp3_spec_type)); 

  MSG_LOW("audmp3_do_file_spec",0,0,0);
  snd_spec.version      = mp3_info.hdr.version;
  snd_spec.layer        = mp3_info.hdr.layer;
  snd_spec.crc_f        = mp3_info.hdr.crc_flag;
  snd_spec.bitrate      = mp3_info.hdr.bitrate;
  snd_spec.sample_rate  = mp3_info.hdr.samplerate;
  snd_spec.channel      = mp3_info.hdr.channel;
  snd_spec.extension    = mp3_info.hdr.extension;
  snd_spec.copyright_f  = mp3_info.hdr.copyright_flag;
  snd_spec.original_f   = mp3_info.hdr.original_flag;
  snd_spec.emphasis     = mp3_info.hdr.emphasis;

  if (mp3_info.id3_tag.v2_tag_found) {
 
    audmpeg_id3v2_tag_type *v2_ptr = &(mp3_info.id3_tag.v2_tag);

    snd_id3v2_tag_frame_type *snd_frame_ptr = &(snd_spec.id3_tag.v2_tag.title);      
    snd_spec.id3_tag.v2_tag.tag_ver  = v2_ptr->version;
    snd_spec.id3_tag.v2_tag.tag_flag = v2_ptr->hdr_flag;
    snd_spec.id3_tag.v2_tag_found    = TRUE;

    for (cnt = 0; cnt < AUDMPEG_ID3V2_FRAME_MAX; cnt++, snd_frame_ptr++) {

      if ((v2_ptr->frames[cnt].found) &&
          (v2_ptr->frames[cnt].frame_content)) {

        snd_frame_ptr->content = v2_ptr->frames[cnt].frame_content;
        snd_frame_ptr->flag    = v2_ptr->frames[cnt].flag;
        snd_frame_ptr->size    = v2_ptr->frames[cnt].frame_size;

      } else {

        snd_frame_ptr->content = NULL;
        snd_frame_ptr->flag    = 0;
        snd_frame_ptr->size    = 0;
      }
    } /* for */

    /* copy the APIC frame Information */
    if (v2_ptr->apic.found) {
      snd_spec.id3_tag.v2_tag.apic.found = v2_ptr->apic.found;
      snd_spec.id3_tag.v2_tag.apic.flags = v2_ptr->apic.flags;
      snd_spec.id3_tag.v2_tag.apic.file_position = 
                                     v2_ptr->apic.file_position;
      snd_spec.id3_tag.v2_tag.apic.size = v2_ptr->apic.size;
    }
    /* copy the Generic frames information */
    snd_spec.id3_tag.v2_tag.gframes = 
              (snd_id3v2_generic_frame_type*)v2_ptr->gframes;
    snd_spec.id3_tag.v2_tag.gframes_cnt = v2_ptr->gframes_cnt; 
  }
	
  if (mp3_info.id3_tag.v1_tag_found) {
    audmpeg_id3v1_tag_type *id3_ptr = &(mp3_info.id3_tag.v1_tag);
    snd_spec.id3_tag.v1_tag_found    = TRUE;

    memcpy(snd_spec.id3_tag.v1_tag.title, 
           (const char * PACKED)id3_ptr->title,
           AUDMPEG_ID3_TITLE_LENGTH);

    memcpy(snd_spec.id3_tag.v1_tag.artist, 
           (const char * PACKED)id3_ptr->artist,
           AUDMPEG_ID3_ARTIST_LENGTH);

    memcpy(snd_spec.id3_tag.v1_tag.album, 
           (const char * PACKED)id3_ptr->album,
           AUDMPEG_ID3_ALBUM_LENGTH);

    memcpy(snd_spec.id3_tag.v1_tag.year, 
           (const char * PACKED)id3_ptr->year,
           AUDMPEG_ID3_YEAR_LENGTH);

    memcpy(snd_spec.id3_tag.v1_tag.comment, 
           (char * PACKED)id3_ptr->comment,
           AUDMPEG_ID3_COMMENT_LENGTH);

    snd_spec.id3_tag.v1_tag.genre = id3_ptr->genre;

  }
	
  if ((mp3_info.id3_tag.v2_tag_found) && 
      !(mp3_info.id3_tag.v1_tag_found)) {

    MSG_LOW("V2 only, copy v2 to v1",0,0,0);
    audmp3_copy_v2_to_v1(&snd_spec, &(mp3_info.id3_tag.v2_tag));
    mp3_info.id3_tag.v1_tag_found = TRUE;

  }

  if (mp3_info.song_len_ms > 0) {
    if(IS_VBR) {
      /* Check if TOC for VBR is present then only support seek */
      if (mp3_info.vbr_hdr.toc_flag) {
        snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_SEEK |
                                SND_SUPPORTED_OPS_MASK_GETTIME;
      } else {
        snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_GETTIME;
      }
    } else {
      snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_SEEK |
	                          SND_SUPPORTED_OPS_MASK_GETTIME;
    }
  } else {
    snd_spec.supported_ops = SND_SUPPORTED_OPS_MASK_NONE;
  }

  snd_spec.file_type = SND_CMX_AF_FILE_MP3;
  audmain_do_play_cb(handle, SND_CMX_AF_AUDIO_SPEC, 0, (void *)&snd_spec);

  /* Free up id3v2 since it is just sent to CMX
   */
  /* Since we copy same structure to snd layer, we don't need to free again.
  but we need to mark on mp3_info not to free same buffer next time*/
  mp3_info.id3_tag.v2_tag.gframes = NULL;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_calc_byte_offset_vbr

DESCRIPTION
  This function calculates the byte offset within an MP3 VBR file given the
MP3 VBR header information, the number of milliseconds to move in the file,
and the current position of the file in milliseconds.

DEPENDENCIES
  mp3_info must be initialized.

RETURN VALUE
  A byte offset into the file is returned, which can be used for forward or
backward movement.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint32 audmp3_calc_byte_offset_vbr(
  int ms,                 /* Milliseconds to calculate for byte movement   */
  int song_pos_ms         /* Current position in MS of song                */
)
{
  int new_pos_ms;  /* Destination position in song in MS                      */
  int percent_x10; /* percentage into file to move, factored by 10 (000-999)  */
  int percent_a;   /* percentage into file to move, 0-99 (fraction truncated) */
  int factor_a;    /* lower factor based on 0-99 percent from TOC             */
  int factor_b;    /* upper factor based on 0-99 percent from TOC             */
  int factor_x;    /* estimated factor between factor_a and factor_b          */

  /* Figure out where in the song we will move to.
  */
  new_pos_ms = song_pos_ms + ms;
  if(new_pos_ms < 0) {
    /* Start at the beginning */
    new_pos_ms = 0;
  }
  if(new_pos_ms > mp3_info.song_len_ms) {
    /* Move to the end */
    new_pos_ms = mp3_info.song_len_ms;
  }

  /* Calculate percentage into song, factored by 10 */
  percent_x10 = (new_pos_ms * 1000) / (int)mp3_info.song_len_ms;
  if(percent_x10 >= 990) {
    /* Set percent_a and factor_b to max values */
    percent_a = 99;
    factor_b = 256;
  } else {
    /* Calculate percent_a and retrieve factor_b */
    percent_a = percent_x10 / 10;
    factor_b = mp3_info.vbr_hdr.toc[(percent_a)+1];
  }
  factor_a = mp3_info.vbr_hdr.toc[percent_a];

  /* Factor_x is an approximate gradient between factor_a and factor_b based on
  ** percentage differences of factor_a and factor_b.  Factor_x is scaled by
  ** 10 for greater precision.  The value is between 0 and 2560.
  */
  factor_x = factor_a * 10 + (factor_b - factor_a) *
                                                 (percent_x10 - percent_a * 10);
  /* return approximate destination position into file */
  return(audmp3_parse_ctl.first_sync +
         (factor_x * (audmp3_parse_ctl.file_len / 2560)));
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_calc_byte_offset

DESCRIPTION
  This function calculates the byte offset within an MP3 file given the MP3
header information and the number of milliseconds to move in the file.

DEPENDENCIES
  mp3_info must be initialized.

RETURN VALUE
  A byte offset into the file is returned, which can be used for forward or
backward movement.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uint32 audmp3_calc_byte_offset(
  int ms,                 /* Milliseconds to calculate for byte movement   */
  int song_pos_ms         /* Current position in MS of song                */
)
{
  int new_pos_ms;  /* Destination position in song in MS                      */
  uint32 num_frames = 0;
  uint32 offset;

  /* Figure out where in the song we will move to.
  */
  new_pos_ms = song_pos_ms + ms;
  /* this is required for small files to work fine*/
  if (!audmp3_parse_ctl.file_pad_req) {
    /* reduce the file_len to the actual size */
    audmp3_parse_ctl.file_len -= audmp3_parse_ctl.file_pad_length;
    /* set for EOF padding */
    audmp3_parse_ctl.file_pad_req = TRUE;
  }

  if(new_pos_ms < 0) {
    /* Start at the beginning */
    new_pos_ms = 0;
  }
  if(new_pos_ms > mp3_info.song_len_ms) {
    /* Move to the end */
    new_pos_ms = mp3_info.song_len_ms;
    return(audmp3_parse_ctl.file_len); 
  }

  /* Calculate MS in 2 steps, one for seconds, and one for fractions of
  ** seconds.  Avoid loss of precision by rearranging calculations.
  */
  if(new_pos_ms >= 1000) {
    num_frames = (new_pos_ms / 1000) *
     (uint32)audmpeg_samplerate_values_table[mp3_info.hdr.samplerate] /
                                                       mp3_info.frame_samples;
  }

  /* Fraction of second movement. */
  num_frames += ((new_pos_ms % 1000) *
                 (uint32)audmpeg_samplerate_values_table[mp3_info.hdr.samplerate]) /
                                              (mp3_info.frame_samples * 1000);

  offset = audmp3_parse_ctl.first_sync + (mp3_info.frame_bytes * num_frames);
  return(offset);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_find_sync

DESCRIPTION
  This function calculates the byte offset of the sync bits within an MP3
file given the MP3 buffer and the number of bytes contained within the
buffer.

DEPENDENCIES
  The buffer must be at least 2 bytes long.

RETURN VALUE
  A byte offset into the buffer is returned, or an offset equal to the
length of the buffer is returned if the sync is not found.

SIDE EFFECTS
  None.

===========================================================================*/
int audmp3_find_sync(
  byte    **buffer_ptr_ptr,   /* Buffer to search through */
  uint32  buf_len,
  boolean first_buffer
)
{

  uint32  min_len = 4;
  uint32  frame_len = 0;
  uint32  next_fr_off = 0;     /* Offset to the start of the next frame */
  uint32  next_fr_off_end = 0;
  uint32  index = 0;
  byte    *buffer = *buffer_ptr_ptr;
  boolean found_sync = FALSE;

  /* Need a minimum number of bytes in the buffer to have a sync
  */
  if(buf_len < min_len) {
    return(buf_len);
  }

  /* Look through the buffer to find the sync bytes.
  */
  while (index <= (buf_len - min_len)) {
    if(IS_SYNC(buffer)) {
      if(mp3_frame_data_found) {
        /* We are trying to seek to a new location - reuse the sync info we
         * already have */
        if(IS_VALID_FRAME(buffer)) {
          found_sync = TRUE;
          break;
        }
      } else {
        audmp3_extract_mp3_header(buffer, &mp3_info);
        audmp3_extract_vbr_header(buffer, &mp3_info.hdr, &mp3_info.vbr_hdr);

        if(IS_VBR) {
          if(audmp3_is_valid_mp3(&mp3_info.hdr)) {
            found_sync = TRUE;
            break;
          }
        }
        else {
          if(audmp3_is_valid_mp3(&mp3_info.hdr)) {
            /* Found a valid header, but let's verify that it is not a false
             * positive by checking if there exists another frame with the
             * same sync info, next to this frame */
            frame_len = audmp3_calc_frame_bytes(&mp3_info.hdr,
                                                &mp3_info.vbr_hdr);
            if (frame_len) {
              /* Account for loss of precision in the frame length calculation*/
              frame_len--;

              /* Check if the remaining buffer size is large enough to
               * look for another sync */
              if ((index + frame_len) < (buf_len - (min_len - 1))) {
                next_fr_off = frame_len;
                next_fr_off_end = next_fr_off +
                            MIN(6, buf_len - (index+frame_len) - (min_len - 1));

                /* Search the next few bytes for the next sync */
                while (next_fr_off < next_fr_off_end) {
                  if (IS_SYNC(buffer+next_fr_off)) {
                    if(IS_VALID_FRAME(buffer+next_fr_off)) {
                      found_sync = TRUE;
                      break;
                    }
                  }
                  next_fr_off++;
                }
                if (found_sync == TRUE) {
                  break;
                }
              }
              else {
                /* Assume that the first sync is valid, since there is not
                 * enough data in the buffer to look for the next sync */
                found_sync = TRUE;
                break;
              }
            }
          }
        }
        /* Clear info, since we hit a false positive */
        mp3_frame_data_found = FALSE;
      }
    }
    buffer++;
    index++;
  }

  if(found_sync == TRUE) {
    if(audmp3_supported_mp3(&mp3_info.hdr)) {
      MSG_MED("MP3 ver %d,layer %d,bitrate  %d",4-mp3_info.hdr.version,
                               4-mp3_info.hdr.layer,mp3_info.hdr.bitrate);
      MSG_MED("MP3 samplerate %d", 
            audmpeg_samplerate_values_table[mp3_info.hdr.samplerate], 0, 0);
      return(index);
    }
  }
  return(buf_len);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_data_cb_req

DESCRIPTION
  This function responds accordingly to an incoming data buffer.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
boolean audmp3_get_data_cb_req(
  uint8  **data,
  uint8  *buf,
  uint32 length,
  uint32 pos
)
{
  *data = buf;

  MSG_LOW("audmp3_get_data_cb_req len=%d pos=%d ops=%x", length,pos,audmp3_parse_ctl.ops);
  /* Data used for tag processing. */
  if (((audmp3_parse_ctl.ops & 
         AUDMPEG_PROC_OPS_PROCESSING_GETTIME) ||
       (audmp3_parse_ctl.ops & 
         AUDMPEG_PROC_OPS_PROCESSING_GETSPEC)) &&
      (audmp3_parse_ctl.ops & 
       AUDMPEG_PROC_OPS_PROCESSING_GETTAG)) {

    if (audmpeg_id3v2_parse_async(&audmp3_parse_ctl, 
                                  buf, 
                                  length,
                                  &(mp3_info.id3_tag.v2_tag))) {

      /* tags process is done */
      audmp3_parse_ctl.ops &= ~AUDMPEG_PROC_OPS_PROCESSING_GETTAG;

      if (audmp3_parse_ctl.ops & 
          AUDMPEG_PROC_OPS_PROCESSING_GETSPEC) {

        audmp3_do_file_spec(audmp3_parse_ctl.audmpeg_handle);

      } else if (audmp3_parse_ctl.ops & 
                 AUDMPEG_PROC_OPS_PROCESSING_GETTIME) {

        /* Calculate the song length, if a file size is known */
        mp3_info.song_len_ms = audmp3_calc_song_len_ms(
                         &mp3_info.hdr, &mp3_info.vbr_hdr, mp3_info.frame_bytes,
                         audmp3_parse_ctl.file_len,
                         mp3_info.frame_samples);
    
        MSG_MED("Song length is %d ms",mp3_info.song_len_ms,0,0);

        audmain_audfmt_do_time();
      }

      audmp3_parse_ctl.ops = AUDMPEG_PROC_OPS_NONE; 

      audmain_do_play_cb(audmp3_parse_ctl.audmpeg_handle, 
                         SND_CMX_AF_FREE_BUF, 
                         0, 
                         (void *)audmp3_parse_ctl.track_buf_2);
    } else {

      audmain_do_play_cb(audmp3_parse_ctl.audmpeg_handle, 
                         SND_CMX_AF_FREE_BUF, 
                         0, 
                         (void *)audmp3_parse_ctl.track_buf_2);

      audmp3_parse_ctl.track_2_start = 
        mp3_info.id3_tag.v2_tag.next_file_pos; 
      
      audmain_get_data(audmp3_parse_ctl.audmpeg_handle, 
                       audmp3_parse_ctl.track_2_start,
                       &(audmp3_parse_ctl.track_buf_2));
    }  
       
    return (TRUE);
  }

  return audmpeg_get_data_cb_req(&audmp3_parse_ctl, buf, length);
}

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDMP3 External Function definitions
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION audmp3_format_play

DESCRIPTION
  Use this function to play streaming MP3.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
audmain_status_type audmp3_format_play (
  snd_cmx_format_spec_type    *input_spec,
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *parse_ctl
) {
   if(input_spec->codec_type.file_type != SND_CMX_AF_FILE_MP3) {
     MSG_LOW("Not an MP3 stream",0,0,0);
     return(AUDMAIN_STATUS_ERROR);
   }

   /* Reset internal parameters */
   audmp3_reset();

   mp3_info.is_stream = TRUE;

   /* Extract all relevant MP3 information */
   audmp3_extract_mp3_header(buffer, &mp3_info);
   audmp3_extract_vbr_header(buffer, &mp3_info.hdr, &mp3_info.vbr_hdr);

   /* Internal checks for MP3 support */
   if(!audmp3_supported_mp3(&mp3_info.hdr)) {
     MSG_HIGH("Unsupported MP3 file",0,0,0);
     return(AUDMAIN_STATUS_ERROR);
   }

   /* Clear the resync func, so that any attempt to resync will fail */
   audmp3_parse_ctl.find_sync_func = NULL;

   /* Setup the buffers */
   audmpeg_setup_streaming(buffer, handle, parse_ctl, &audmp3_parse_ctl);

   MSG_MED("Streaming MP3 ready to go", 0, 0, 0);
   return(AUDMAIN_STATUS_SUCCESS);
} /* end audmp3_format_play */

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_parse_head

DESCRIPTION
  This function parses the mp3 header for relevant infomation.

DEPENDENCIES
  None

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS

===========================================================================*/
audmain_status_type audmp3_parse_head (
  uint8                      *buffer,
  snd_af_access_type         *handle,
  audmain_parse_ctl_type     *parse_ctl,
  audmain_play_param_type    *param
) {
	 
  /* Reset internal parameters */
  audmp3_reset();

  mp3_info.is_stream = FALSE;

  if (param->get_specs) {
    audmp3_parse_ctl.ops = AUDMPEG_PROC_OPS_PROCESSING_GETSPEC;
  } else if (param->calc_time) {
    audmp3_parse_ctl.ops = AUDMPEG_PROC_OPS_PROCESSING_GETTIME;
  } else {
    audmp3_parse_ctl.ops = AUDMPEG_PROC_OPS_PROCESSING_PLAYBACK;
  }

  if (!audmpeg_find_first_frame(buffer, 
                                handle, 
                                parse_ctl,
                                &audmp3_parse_ctl, 
                                &mp3_info.id3_tag)) {
      MSG_LOW("Not an MP3 file",0,0,0);
      return(AUDMAIN_STATUS_ERROR);
  }
  
  mp3_info.frame_bytes = audmp3_calc_frame_bytes(&mp3_info.hdr,
                                                 &mp3_info.vbr_hdr);

  mp3_info.frame_samples = audmp3_calc_frame_samples(&mp3_info.hdr);

  if (!(param->calc_time) &&
      !(param->get_specs)) {
    /* Sync word has been found, frame head is processed, 
     * keep the orginal implementation for playback processing */

    /* Locate the ID3V1 tag and extract it, if available */
    audmpeg_load_id3v1_tag(handle, &audmp3_parse_ctl, &mp3_info.id3_tag);
  
    /* Calculate the song length, if a file size is known */
    if (audmp3_parse_ctl.file_len != 0) {
      mp3_info.song_len_ms = audmp3_calc_song_len_ms(
                       &mp3_info.hdr, &mp3_info.vbr_hdr, mp3_info.frame_bytes,
                       audmp3_parse_ctl.file_len,
                       mp3_info.frame_samples);
  
      MSG_MED("Song length is %d ms",mp3_info.song_len_ms,0,0);
    } else {
      mp3_info.song_len_ms = 0;
      MSG_ERROR("Unable to calculate song length.  FF/REW Disabled.",0,0,0);
    }
  
    audmpeg_setup_track2(handle, parse_ctl, &audmp3_parse_ctl);

    /* Create the SND/CMX file spec table, and return the data to the 
     * CMX layer */
    audmp3_do_file_spec(handle);
  
    audmp3_parse_ctl.error_status = AUDMAIN_STATUS_MAX;
    /* Mark for Padding 1500 bytes at the end of the file: This is required to
     * ensure that we force the DSP to decode any remaining portion of mp3 data
     * and provide a delay to check for EOF
     */
    audmp3_parse_ctl.file_pad_req = TRUE;
    audmp3_parse_ctl.file_pad_length = EOF_PADDING_BUFFER_SIZE; /* 1500 */
  
    MSG_MED("MP3 ready to go", 0, 0, 0);
    return (AUDMAIN_STATUS_SUCCESS);

  } else if ((param->calc_time) ||
             (param->get_specs)) {

    if (audmp3_parse_ctl.file_len > 0) {

      memset(&(mp3_info.id3_tag), 0, sizeof(audmpeg_id3_tag_type));
    
      audmpeg_id3tag_process(handle, 
                             &audmp3_parse_ctl, 
                             &mp3_info.id3_tag);
      
      if (audmp3_parse_ctl.ops & 
          AUDMPEG_PROC_OPS_PROCESSING_GETTAG) {
    
        /* Oops, let's deal with those famous tags, seigh...... 
         * switch to ASYNC data request */
        audmain_set_read_sync(FALSE);
        
        audmp3_parse_ctl.track_2_start = 
          mp3_info.id3_tag.v2_tag.next_file_pos;
        
        audmain_get_data(handle, 
                         audmp3_parse_ctl.track_2_start,
                         &(audmp3_parse_ctl.track_buf_2));
  
        return (AUDMAIN_STATUS_SUCCESS);

      } else if (!(audmp3_parse_ctl.ops & 
                   AUDMPEG_PROC_OPS_PROCESSING_GETTAG)) {
    
        if (param->get_specs) {
    
          /* Create the SND/CMX file spec table, and return the data to the 
           * CMX layer */
          audmp3_do_file_spec(handle);
        } 
    
        if (param->calc_time) {
    
          mp3_info.song_len_ms = 
            audmp3_calc_song_len_ms(&mp3_info.hdr, 
                                    &mp3_info.vbr_hdr, 
                                    mp3_info.frame_bytes,
                                    audmp3_parse_ctl.file_len,
                                    mp3_info.frame_samples);
    
          MSG_MED("Song length is %d ms for get_time",
                  mp3_info.song_len_ms,0,0);
          /* reset id3v2 structure here, get spec/playback will be released 
          by CMX */
          audmpeg_id3v2_reset_tag(&(mp3_info.id3_tag.v2_tag), TRUE);
        } /* param->calc_time */
      }
    
      return (AUDMAIN_STATUS_SUCCESS);
    }       
  }

  return (AUDMAIN_STATUS_ERROR);
} /* end audmp3_parse_head */

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_sample_rate

DESCRIPTION
  This function returns the sampling rate of the current mp3 song.

DEPENDENCIES
  mp3_info

RETURN VALUE
  sample rate of the last mp3 file parsed

SIDE EFFECTS
  None

===========================================================================*/
snd_sample_rate_type audmp3_get_sample_rate (
  audmain_parse_ctl_type  *parse_ctl
)
{
  return(mp3_info.hdr.samplerate);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_flush

DESCRIPTION
  This function flushes the buffers and restarts playback.

DEPENDENCIES
  mp3_info must be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmp3_flush (
  audmain_parse_ctl_type   *parse_ctl
)
{
  audmpeg_parse_ctl_type *mp3_ptr;
  audmain_status_type    ret_val = AUDMAIN_STATUS_ERROR;
  uint32 dummy1 = 0;
  uint32 dummy2 = 0;

  if(parse_ctl != NULL) {
    mp3_ptr = (audmpeg_parse_ctl_type*)parse_ctl->tracks;
    MSG_LOW("audmp3_flush mp3_ptr=%p",mp3_ptr,0,0);
    if( mp3_ptr != NULL) {
      if (mp3_info.is_stream == TRUE) {
        mp3_ptr->file_location = audmp3_parse_ctl.first_sync;
      }
      mp3_ptr->resync_state  = AUDMPEG_RESYNC_START;
    }
    audmpeg_age(parse_ctl, &dummy1, &dummy2);
    ret_val = AUDMAIN_STATUS_SUCCESS;
  }
  return(ret_val);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_fforward

DESCRIPTION
  This function moves the MP3 playback forward ms milliseconds.

DEPENDENCIES
  mp3_info must be initialized

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds forward.

===========================================================================*/
audmain_status_type audmp3_fforward (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_forward,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
)
{
  audmpeg_parse_ctl_type *mp3_ptr;

  if(parse_ctl != NULL && ms_forward > 0) {
    mp3_ptr = (audmpeg_parse_ctl_type *)parse_ctl->tracks;
    if (mp3_info.is_stream == TRUE ||
        mp3_info.song_len_ms == 0) {
      /* streaming data - let the higher layers handle this */
      return AUDMAIN_STATUS_FAILURE;
    } else {
      if(IS_VBR) {
        /* Check if TOC for VBR is present then only do the FF 
         * else don't do any thing */
        if (mp3_info.vbr_hdr.toc_flag) {
          mp3_ptr->file_location =
                          audmp3_calc_byte_offset_vbr(ms_forward, *elapsed_ms);
        }
      } else {
        mp3_ptr->file_location =
                               audmp3_calc_byte_offset(ms_forward, *elapsed_ms);
      }

      mp3_ptr->resync_state = AUDMPEG_RESYNC_START;
      if(mp3_ptr->file_location >= audmp3_parse_ctl.file_len) {
        *adjusted_ms = *elapsed_ms = mp3_info.song_len_ms;
      } else {
        if(IS_VBR) {
          /* Adjust the elapsed time only if TOC is present */
          if (mp3_info.vbr_hdr.toc_flag) {
            *adjusted_ms = *elapsed_ms = *elapsed_ms + ms_forward;
          }
        } else {
          *adjusted_ms = *elapsed_ms = *elapsed_ms + ms_forward;
        }
      }
    }
  }
  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_rewind

DESCRIPTION
  This function moves the MP3 playback backward ms milliseconds.

DEPENDENCIES
  mp3_info must be initialized

RETURN VALUE
  AUDMAIN_STATUS_SUCCESS if successful

SIDE EFFECTS
  The buffer pointers are moved ms milliseconds backward.

===========================================================================*/
audmain_status_type audmp3_rewind (
  audmain_parse_ctl_type   *parse_ctl,
  uint32                   ms_rewind,
  uint32                   *elapsed_ms,
  uint32                   *adjusted_ms
)
{
  audmpeg_parse_ctl_type *mp3_ptr;

  if(parse_ctl != NULL && ms_rewind > 0) {
    if (mp3_info.is_stream == TRUE || mp3_info.song_len_ms == 0) {
      /* streaming data - let the higher layers handle this */
      return AUDMAIN_STATUS_FAILURE;
    } else {
      mp3_ptr = (audmpeg_parse_ctl_type *) parse_ctl->tracks;
      if(IS_VBR) {
        /* Check if TOC for VBR is present then only do the RW 
         * else don't do any thing */
        if (mp3_info.vbr_hdr.toc_flag) {
        mp3_ptr->file_location =
                           audmp3_calc_byte_offset_vbr(-ms_rewind, *elapsed_ms);
        }
      } else {
        mp3_ptr->file_location =
                               audmp3_calc_byte_offset(-ms_rewind, *elapsed_ms);
      }

      mp3_ptr->resync_state = AUDMPEG_RESYNC_START;
      if(ms_rewind >= *elapsed_ms) {
        if(IS_VBR) {
          /* Adjust the elapsed time only if TOC is present */
          if (mp3_info.vbr_hdr.toc_flag) {
            *adjusted_ms = *elapsed_ms = 0;
	  }
        } else {
        *adjusted_ms = *elapsed_ms = 0;
        }
      } else {
        if(IS_VBR) {
          /* Adjust the elapsed time only if TOC is present */
          if (mp3_info.vbr_hdr.toc_flag) {
            *adjusted_ms = *elapsed_ms = *elapsed_ms - ms_rewind;
          }
      } else {
        *adjusted_ms = *elapsed_ms = *elapsed_ms - ms_rewind;
        }
      }
    }
  }
  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_reset

DESCRIPTION
  This function resets internal MP3 buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mp3_info, audmp3_parse_ctl

===========================================================================*/
void audmp3_reset ()
{
  MSG_LOW("audmp3_reset",0,0,0);
  if(mp3_info.id3_tag.v1_tag_found || mp3_info.id3_tag.v2_tag_found) {
    audmpeg_id3v2_reset_tag(&(mp3_info.id3_tag.v2_tag), TRUE);
  }
  audmpeg_reset(&audmp3_parse_ctl);

  (void) memset(&mp3_info, 0, sizeof(mp3_info));

  audmp3_parse_ctl.find_sync_func = audmp3_find_sync;

  mp3_frame_data_found = FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_length

DESCRIPTION
  This function returns the total duration of the selected MP3 file.

DEPENDENCIES
  mp3_info

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmp3_get_length(
  audmain_parse_ctl_type *parse_ptr,
  uint32 *elapsed_ms
)
{
  if (audmp3_parse_ctl.ops & 
      AUDMPEG_PROC_OPS_PROCESSING_GETTAG) {

    return (AUDMAIN_STATUS_PENDING);
  }

  *elapsed_ms = mp3_info.song_len_ms;
  MSG_LOW("audmp3_get_length length=%d",mp3_info.song_len_ms,0,0);
  return(AUDMAIN_STATUS_DONE);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_get_song_config

DESCRIPTION
  Returns song-specific information to optimize MP3 playback.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmp3_get_song_config(
  audmain_parse_ctl_type *parse_ctl,
  audmpeg_song_config_type *song_config
)
{
  mp3_header_type             *mp3_hdr_ptr   = &mp3_info.hdr;
  audmain_mp3_config_type     *config        = &mp3_info.config;
  MSG_HIGH("audmp3_get_song_config",0,0,0);
  if(parse_ctl != NULL) {
    song_config->mp3_config = *config;

    if( mp3_hdr_ptr->samplerate == SND_SAMPLE_RATE_NONE ||
        mp3_hdr_ptr->samplerate == SND_SAMPLE_RATE_MAX){
      return(AUDMAIN_STATUS_ERROR);
    }
    if(mp3_frame_data_found) {
      /* If we found the header, copy the non-changing header info and mask */
      song_config->mask_size = sizeof(mp3_frame_mask);
      memcpy(song_config->mask, mp3_frame_mask, song_config->mask_size);
      memcpy(song_config->header, mp3_frame_data, song_config->mask_size);
    } else {
      /* Clear out the mask and header info */
      song_config->mask_size = 0;
      memset(song_config->mask, 0, sizeof(song_config->mask));
      memset(song_config->header, 0, sizeof(song_config->header));
    }

  }
  return(AUDMAIN_STATUS_SUCCESS);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmp3_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
)
{
  return audmpeg_adec_adjust(parse_ctl, bytes_used);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_parse

DESCRIPTION
  This function parses one event of the MP3 file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audmp3_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmp3_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
)
{
  return audmpeg_parse(parse_ctl, cmd_buf, time_remain);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmp3_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void audmp3_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
)
{
  audmpeg_age(parse_ctl, elapsed_time, adjusted_time);
}

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
)
{

  audmpeg_id3v2_frame_type *v2_frame;
  snd_id3v1_tag_type       *v1_ptr;

  v1_ptr = &(snd_spec->id3_tag.v1_tag); 
 
  v2_frame = &(v2_tag->frames[AUDMPEG_ID3V2_FRAME_TITLE]);
  if (v2_frame->frame_content) {
  	audmp3_copyframe_v2_to_v1(v1_ptr->title, v2_frame,
                              AUDMPEG_ID3_TITLE_LENGTH);
  }

  v2_frame = &(v2_tag->frames[AUDMPEG_ID3V2_FRAME_ARTIST]);
  if (v2_frame->frame_content) {
  	audmp3_copyframe_v2_to_v1(v1_ptr->artist, v2_frame,
                              AUDMPEG_ID3_ARTIST_LENGTH);
  }

  v2_frame = &(v2_tag->frames[AUDMPEG_ID3V2_FRAME_ALBUM]);
  if (v2_frame->frame_content) {
  	audmp3_copyframe_v2_to_v1(v1_ptr->album, v2_frame,
                              AUDMPEG_ID3_ALBUM_LENGTH);
  }

  v2_frame = &(v2_tag->frames[AUDMPEG_ID3V2_FRAME_YEAR]);
  if (v2_frame->frame_content) {
  	audmp3_copyframe_v2_to_v1(v1_ptr->year, v2_frame,
                              AUDMPEG_ID3_YEAR_LENGTH);
  }

  v2_frame = &(v2_tag->frames[AUDMPEG_ID3V2_FRAME_LYRICIST]);
  if (v2_frame->frame_content) {
  	audmp3_copyframe_v2_to_v1(v1_ptr->comment, v2_frame,
                              AUDMPEG_ID3_COMMENT_LENGTH);
  }

  v2_frame = &(v2_tag->frames[AUDMPEG_ID3V2_FRAME_GENRE]);
  if (v2_frame->frame_content) {
    memcpy(&(v1_ptr->genre), v2_frame->frame_content, 1);
  }
}

/*===========================================================================

FUNCTION audmp3_check_ops_in_progress

DESCRIPTION
  Check whether there is any operation in-progress when do parse_head.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean audmp3_check_ops_in_progress (void)
{
  return (audmp3_parse_ctl.ops != AUDMPEG_PROC_OPS_NONE);
}

/*===========================================================================

FUNCTION audmp3_abort_calc_time

DESCRIPTION
  This function abort the current get_time operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void audmp3_abort_calc_time (void)
{
  audmp3_parse_ctl.ops = AUDMPEG_PROC_OPS_NONE;

  audmain_do_play_cb(audmp3_parse_ctl.audmpeg_handle, 
                     SND_CMX_AF_FREE_BUF, 
                     0, 
                     (void *)audmp3_parse_ctl.track_buf_2);
}

#endif /* FEATURE_MP3 */

