/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("AudMPEG Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             A U D I O   F O R M A T s   M P E G   S E R V I C E S 

GENERAL DESCRIPTION
  This module contains definitions common to both MP3 and AAC file formats

EXTERNALIZED FUNCTIONS
  audmpeg_get_data_cb_req
    Callback function which handles the response to a Data request.
  audmpeg_load_id3v1_tag
    This function loads the id3v1 tag from the MPEG Audio file into a
    temporary buffer.
  audmpeg_find_first_frame
    Processes any ID3v2 tags and finds the first frame.
  audmpeg_setup_track2
    Sets up the track2 buffer.
  audmpeg_adec_adjust
    Adjusts adec buffers to reflect their newly used data.
  audmpeg_parse
    Parses one event of the MPEG file.  Prepares buffers for data transfer.
  audmpeg_age
    Checks to see if the buffers are ready for use.
  audmpeg_reset
    Resets internal buffers after a song is unloaded.
  audmpeg_setup_streaming
    This function sets up the buffers to handle streaming audio.

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c) 2002 - 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/audfmt/main/latest/src/audmpeg.c#9 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/09  kkm      Fixed to not send end of file if there are any pending requests.
                   Also padding is done if there are no pending requests
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and 
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions
06/03/09    kk     fixed CR 183914 - Incorrect ID3 tag makes infinite loop.
04/15/09    KK     Fixed CR 168161 - When Rewind operation was done, Observed 
                   AbError after 1Sec for all the (MP3-8kbps) clips & 
                   FastForward not working 
02/20/08    kk     Fixed CR 172460 - Enter endless loop when parsing mp3 with 
                   false ID3 tag of length 0. 
11/21/08    kk     Added support for to play short MP3 files 
                   (< 1sec or file size, excluding ID3 tag is <2500)-CR 162679.
11/04/08    kk     CR159736-Added support to play Multiple Id3tag Mp3 files.
08/13/08   skr     Fixed a memory leak issue in audmpeg_id3v2_parse_async()
02/28/08   vsud    Fixed CR 132084- applicable to all targets irrespective of 
                   CMX buffer size.
10/06/09    hs     Fixed memory leakage on gframes
09/16/07    hs     Added support for "TOFN" tag
08/30/07    hs     Fixed crash issue when we have APIC frame and added
                   checking invalid frame len
08/17/07    kk     Fixed CR 125886 - tag information still can't be got by UI.
08/06/07    kk     Merged the Fix CR 124244 - Full ID3V2.3 support, 
                   support for URL link (W)frames .
05/25/07    kk     Fixed compilation error.
05/23/07    kk     Fixed  CR 113932 - CMX get spec problem due to APIC 
                   frame in MP3 ID3V2.3 tag.
05/11/07    hs     Fixed CR 117907 - crash and memory leak
04/19/07    hs     Fixed crash on corrupted mp3 file
01/23/07    kk     Fixed issues related to adding 44k sample rate.
11/29/06    kk     fixed CR 106011-Not getting playback time when audfmt_get_time()
                   is called.
10/19/06    az     Fixed the CR: 103936 unable to play MP3 Playback file 
08/15/06    sg     Modified to use uint32 for find_sync API. Fixed 99003 to
                   account for files which are smaller than the CMX buffers
                   when doing sync reads.
07/28/06    hz     Added support for ID3V2 and AAC ADTS/VBR song length.
06/06/06    sg     Fixed Klocwork warning.
04/13/06    KK     Fixed NULL pointer memcpy in audmpeg_load_id3v1_tag() while 
                   an error is returned from File system during extracting id3v1
                   tag for MP3/AAC as requested from OEM.
04/04/06    KK     Modified audmpeg_find_first_frame() to support proper playing
                   of MP3/AAC files with file length <= to the Buffer size.
08/12/05    sg     Modified so that we no longer wait for both buffers before
                   resuming playback in RESYNC_LOAD state.
07/19/05    pl     Added the support for setting the error status at the point 
                   where the error is encountered. The error status would 
                   eventually be relayed back to the upper layer to provide
                   more information for the aborted playback.
05/17/05    ac     Added support for padding header-less streams with zeroes to
                   ensure that the DSP decodes all the frames at the end of a
                   clip.
04/25/05    lg     Modified audmpeg_parse to return AUDMAIN_STATUS_ERROR if CMX
                   failed to get more data from Source.
04/23/05    ac     Modified code to pass boolean argument for the first
                   buffer when looking for sync information
08/27/04    sg     Restore old ID3 extraction behaviour.
08/11/04    sg     Changed so that SND determines when the next aging should be
                   invoked. Modified to use the correct ID3 tag length.
07/30/04    st     Modified to restore primary buffer if sync word is not
                   found by find_first_sync()
04/23/04    sg     Added check to not load data if location is greater than
                   file length.
03/05/04    sg     Modified find_first_frame() to not skip zeros, since
                   mp3_find_sync() has been enhanced to skip garbage.
                   Modified to fake an extra byte if there's only one byte
                   remaining to be written to the DSP.
01/26/04    sg     Reduced MSG_LVL for some diagnostic messages.
11/23/03    sg     Modified to clear elapsed time while audmain is not ready.
09/19/03    sg     Modified to return the correct status from
                   audmpeg_get_data_cb_req().
03/13/03    sg     Added support to play files which have a bunch of zeroes at
                   the beginning of the song.
02/07/03    sg     Modified to break the loop between audmpeg_age()
                   audmpeg_resync() when the fast-forward button is pushed
                   repeatedly (CR27652).
02/04/03    sg     Modified audmpeg_get_data_cb_req() so that it validates the
                   length parameter. Also guarantees that the last byte of a
                   file with an odd file size will be written to the DSP.
08/15/02    sg     Modified so that audmpeg_age() is also called immediately
                   after data is moved to the adec, if any buffers are empty.
                   This will allows us to request the client for data earlier,
                   and help reduce client side delays.
08/07/02    st     Modified debug message.
07/25/02    sg     In audmpeg_resync() use the find_sync_func pointer to
                   determine if the data is streaming or not.
07/18/02    sg     In audmpeg_parse(), if there is one byte remaining and the
                   end of a file is reached, terminate playing, since we'll
                   never be able write the single byte to the DSP. In streaming
                   mode, return a buffer underflow error.
06/26/02    sg     Removed buffer re-alignment in audmpeg_adec_adjust()
                   when odd number of bytes are used, since
                   qdsp_cmd_adec_write() already handles this scenario.
06/17/02    sg     Initial version
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Customer specific definitions           */
#include "customer.h"           /* Customer specific definitions           */

#ifdef FEATURE_MPEG_AUDIO
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
#include "audmain.h"
#include "audmaini.h"
#include "audmpeg.h"
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* Dynamic memory allocation routines      */

/* Seek frame ID for id3v2 parsing */
const char seek_frame_id[5] = {"SEEK"};

/* APIC frame ID for id3v2 frame parsing */
const char apic_frame_id[5] = {"APIC"};

/* Current supported frame IDs */
const char id3v2_frames_supported[][5] = {

  "TIT2",   /* Title      "TIT2"    */ 
  "TIT3",   /* Subtitle   "TIT3"    */ 
  "TCOP",   /* Copyright  "TCOP"    */
  "TPRO",   /* Produced   "TPRO"    */
  "TCOM",   /* Composer   "TCOM"    */ 
  "TPE1",   /* Artist     "TPE1"    */
  "TPE2",   /* Orchestra  "TPE2"    */ 
  "TPE3",   /* Conductor  "TPE3"    */ 
  "TEXT",   /* Lyricist   "TEXT"    */ 
  "TALB",   /* Album      "TALB"    */ 
  "TRCK",   /* Track      "TRCK"    */ 
  "TDRC",   /* Year       "TDRC"    */ 
  "TPUB",   /* Publisher  "TPUB"    */ 
  "TCON",   /* Genre      "TCON"    */ 
  "TRSN",   /* Station    "TRSN"    */ 
  "TENC",   /* Encoder    "TENC"    */  
  "TLEN"    /* Length     "TLEN"    */
};

/*   support additional TFrames, find in gframes if available */
char id3v2_frames_supported_added[][5] = {
  "TIT1",   /* Group      "TIT1"     */
  "TFLT",   /* Filetype   "TFLT"     */
  "TBPM",   /* BPM        "TBPM"     */
  "TSIZ",   /* Size       "TSIZ"     */
  "TDAT",   /* Date       "TDAT"     */
  "TOFN"    /* Original file name "TOFN"     */
};

/* Samplerate conversion table for calculating the song length
*/
extern const uint32 audmpeg_samplerate_values_table[] = {
  0,
  8000, 11025, 12000,
  16000, 22050, 24000,
  32000, 
  44100, 48000,
#ifdef FEATURE_AAC
  64000, 88200, 96000,
#endif
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
  44000,
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */
  0
};

/* Counter to keep track of malloc - will help with tracking any memory
 * leak issues */
LOCAL uint32 audmpeg_mem_alloc_cnt = 0;

/* <EJECT> */
/*---------------------------------------------------------------------------
** AudMPEG Internal Function definitions
**---------------------------------------------------------------------------
*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
/*===========================================================================

FUNCTION audmpeg_extract_id3v1_tag

DESCRIPTION
  This function extracts the id3v1 tag from the MPEG Audio file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void audmpeg_extract_id3v1_tag(
  byte *data,
  audmpeg_id3v1_tag_type *tag
)
{
  memcpy((byte *)tag, data, AUDMPEG_ID3V1_TAG_LEN);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

#ifdef FEATURE_MPEG_ID3V2
#error code not present
#endif /* FEATURE_MPEG_ID3V2 */

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_get_id3v2_tag_size

DESCRIPTION
  This function extracts the id3v2 tag size from the MPEG Audio file.
  Allows us to skip over unnecessary data quickly.

DEPENDENCIES
  None.

RETURN VALUE
  0 if no tag is present or error;
  Tag length in bytes if present.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
LOCAL int audmpeg_get_id3v2_tag_size(byte *data)
{
  uint32 taglen = 0;

  /* Check if tag data is valid */
  if(data[3] == 0xFF || data[4] == 0xFF ||
     data[6] >= 0x80 || data[7] >= 0x80 || data[8] >= 0x80 || data[9] >= 0x80) {
    /* Not a valid tag. */
    MSG_HIGH("Invalid ID3V2 tag!",0,0,0);
    return(0);
  }

  /* Check if tag is a supported version */
  if(data[3] > 0x04) {
    MSG_HIGH("ID3V2 tag version is unsupported",data[3],0,0);
    return(0);
  }
  /* The first 10 bytes contain the id3v2 header */
  taglen = 10;

  /* The length of the header is then calculated */
  taglen += (uint32)data[6] << 21 | (uint32)data[7] << 14 |
            (uint32)data[8] << 7  | (uint32)data[9];

  MSG_LOW("ID3v2 tag is %d bytes long.\n",taglen,0,0);
  
  /* if(data[5] & 0x40) then we have an extended header, but since the taglen
  ** already includes the size of the extended header, we ignore it.
  */
  return(taglen);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_resync

DESCRIPTION
  This function attempts to move the MPEG buffers to point to the next sync
  start.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The buffer pointers are moved to start at the next sync.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
LOCAL audmain_status_type audmpeg_resync(
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf
)
{
  audmpeg_parse_ctl_type      *mpeg_ptr;
  audmpeg_find_sync_func_type find_sync_func;
  uint32                      next_sync;
  uint32                      total_length;

  if(parse_ctl != NULL) {

    mpeg_ptr       = (audmpeg_parse_ctl_type *) parse_ctl->tracks;
    find_sync_func = mpeg_ptr->find_sync_func;


    if (find_sync_func == NULL) {
      /* Don't try to find sync since ADIF/streaming data has no headers */
      next_sync = 0;
      audmpeg_adec_adjust(parse_ctl, (uint32 *) &next_sync);
    }
    else {
      next_sync = total_length = cmd_buf->adec_cmd.adec_num_buf1 +
                                 cmd_buf->adec_cmd.adec_num_buf2;

      /* Try to find the next sync in the first buffer */
      if(cmd_buf->adec_cmd.adec_buf1 != NULL) {
        next_sync = find_sync_func(&(cmd_buf->adec_cmd.adec_buf1),
                                   cmd_buf->adec_cmd.adec_num_buf1, TRUE);

        if(next_sync == cmd_buf->adec_cmd.adec_num_buf1) {
          /* Didn't find the sync.  Search the next buffer */
          if(cmd_buf->adec_cmd.adec_buf2 != NULL) {
            next_sync = cmd_buf->adec_cmd.adec_num_buf1 +
                       find_sync_func(&(cmd_buf->adec_cmd.adec_buf2),
                                      cmd_buf->adec_cmd.adec_num_buf2, TRUE);
          }
        }
      }

      if(next_sync == total_length) {
        /* Error! No sync found! */
        cmd_buf->adec_cmd.adec_num_buf1 = cmd_buf->adec_cmd.adec_num_buf2 = 0;
        return(AUDMAIN_STATUS_FAILURE);
      }

      if(next_sync != 0) {
        audmpeg_adec_adjust(parse_ctl, (uint32 *) &next_sync);
      }
    }
  }
  return(AUDMAIN_STATUS_SUCCESS);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*---------------------------------------------------------------------------
** AUDMPEG External Function definitions
**---------------------------------------------------------------------------
*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
/*===========================================================================

FUNCTION audmpeg_get_data_cb_req

DESCRIPTION
  Callback function which handles the response to a Data request.

DEPENDENCIES
  audfmt_buf_mgmt

RETURN VALUE
  TRUE if the data was successfully obtained

SIDE EFFECTS
  audfmt_buf_mgmt

===========================================================================*/
boolean audmpeg_get_data_cb_req(
  audmpeg_parse_ctl_type *parse_ctl,
  uint8                  *buf, 
  uint32                 length
)
{
  if(buf == NULL) {
    MSG_ERROR("No data received",0,0,0);
    parse_ctl->read_failed = TRUE;
    parse_ctl->error_status = AUDMAIN_STATUS_DATA_ACCESS_ERR;
  }
  else
  {
    /* If the file ends on a odd byte boundary, make it even, so that the song
    ** will be played to completion in the adec transport function
    */
    if((parse_ctl->file_location > parse_ctl->file_len) && (length & 1)) {
      length++;
      if (parse_ctl->file_len & 1) {
        parse_ctl->file_len++;
      }
    }

    /* Store received length for the track processed. */
    if(buf == parse_ctl->track_buf_1 &&
       parse_ctl->track_1_start <= parse_ctl->file_len) {

      if(parse_ctl->track_1_start+length > parse_ctl->file_len) {
        length = parse_ctl->file_len - parse_ctl->track_1_start;
      }
      parse_ctl->track_1_length  = length;
      parse_ctl->track_1_loading = FALSE;
    }
    else if(buf == parse_ctl->track_buf_2 &&
       parse_ctl->track_2_start <= parse_ctl->file_len) {

      if(parse_ctl->track_2_start+length > parse_ctl->file_len) {
        length = parse_ctl->file_len - parse_ctl->track_2_start;
      }
      parse_ctl->track_2_length  = length;
      parse_ctl->track_2_loading = FALSE;
    }
    else {
      MSG_FATAL("Misplaced buffer!",0,0,0);
      parse_ctl->read_failed = TRUE;
      parse_ctl->error_status = AUDMAIN_STATUS_ERROR;
    }
  }

  return(!parse_ctl->read_failed);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_load_id3v1_tag

DESCRIPTION
  This function loads the id3v1 tag from the MPEG Audio file into a
  temporary buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_load_id3v1_tag(
  snd_af_access_type     *handle,
  audmpeg_parse_ctl_type *parse_ctl,
  audmpeg_id3_tag_type   *id3_tag
)
{
  if(parse_ctl->file_len == 0) {
    MSG_ERROR("Unable to locate EOF for ID3V1 tag extraction",0,0,0);
    return;
  }
  /* Load in the second buffer with the ID3V1 tag offset */
  parse_ctl->track_2_start = parse_ctl->file_len - parse_ctl->track_length;
  audmain_get_data(handle, parse_ctl->track_2_start,
                   &(parse_ctl->track_buf_2));

  if(((parse_ctl->track_buf_2 + 
      (parse_ctl->track_length - AUDMPEG_ID3V1_TAG_LEN)) != NULL) && 
      (parse_ctl->track_buf_2 != NULL)) { 
    audmpeg_extract_id3v1_tag(parse_ctl->track_buf_2 + 
                              (parse_ctl->track_length - AUDMPEG_ID3V1_TAG_LEN), 
                              &id3_tag->v1_tag); 
    audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, 
                       (void *) parse_ctl->track_buf_2); 
  } else { 
    MSG_ERROR("parse_ctl->track_buf_2 is NULL Value",0,0,0); 
    return;
  }

  /* Search for "TAG" at start of ID3V1 info */
  if((id3_tag->v1_tag.tag_id[0] == 0x54) &&
     (id3_tag->v1_tag.tag_id[1] == 0x41) &&
     (id3_tag->v1_tag.tag_id[2] == 0x47)) {
    id3_tag->v1_tag_found = TRUE;
  } else {
    id3_tag->v1_tag_found = FALSE;
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_find_first_frame

DESCRIPTION
  Processes any ID3v2 tags and finds the first frame

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a frame was found

SIDE EFFECTS

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_find_first_frame(
  uint8                       *buffer,
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *main_parse_ctl,
  audmpeg_parse_ctl_type      *mpeg_parse_ctl,
  audmpeg_id3_tag_type        *id3_tag
)
{
  int    sync_start = 0;
  uint32 id3v2_tag_length = 0;
  uint32 total_id3v2_tags_length = 0;
  uint32  data_len = 0;
  /* Get the file length */
  audmain_do_play_cb(handle, SND_CMX_AF_GET_FILE_LEN, 0,
                                            (void *)&mpeg_parse_ctl->file_len);
  MSG_MED("File length = %d",mpeg_parse_ctl->file_len,0,0);
  /* Setup our parse control structure */
  mpeg_parse_ctl->track_buf_1       = buffer;
  mpeg_parse_ctl->track_buf_2       = NULL;
  mpeg_parse_ctl->audmpeg_handle    = handle;
  mpeg_parse_ctl->track_1_start     = 0;
  mpeg_parse_ctl->track_2_start     = 0;
  mpeg_parse_ctl->track_1_length    = main_parse_ctl->buffer_size;
  mpeg_parse_ctl->track_length      = main_parse_ctl->buffer_size;
  mpeg_parse_ctl->file_location     = main_parse_ctl->buffer_size;

  /* Skip the id3v2 tag for now. */
  while (IS_ID3V2_TAG(buffer)) {

  /* copy the start of ID3 tag */
    mpeg_parse_ctl->id3v2_tag_start = total_id3v2_tags_length;
    MSG_LOW("mpeg_parse_ctl->id3v2_tag_start = %d",mpeg_parse_ctl->id3v2_tag_start,0,0);
    id3v2_tag_length = audmpeg_get_id3v2_tag_size(buffer);

    if(id3v2_tag_length == 0) {
      MSG_ERROR("Corrupt ID3V2 tag/taglength",0,0,0);
      break;
    }

    total_id3v2_tags_length += id3v2_tag_length;
    id3_tag->v2_tag_length = total_id3v2_tags_length;
    MSG_LOW("total_id3v2_tags_length = %d",total_id3v2_tags_length,0,0);

    if(total_id3v2_tags_length) {
      if (total_id3v2_tags_length < mpeg_parse_ctl->file_len) {
        /* This data request depends on it being a synchronous request */
        if(mpeg_parse_ctl->id3v2_tag_start != 0) {
          audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, buffer);
          MSG_LOW("Freed buffer",0,0,0);
        }

        if (audmain_get_sync_data(handle, total_id3v2_tags_length, &buffer, &data_len)
            != AUDMAIN_STATUS_SUCCESS) {
          return(FALSE);
        }
        
        if (data_len == 0) {
          MSG_ERROR("got Zero Length buffer - Unable to play the file",0,0,0);
          if( (mpeg_parse_ctl->track_buf_1 != buffer) && (buffer != NULL) ) {
            audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, buffer);
            MSG_LOW("Freed buffer",0,0,0);
          }
          return(FALSE);
        }
        MSG_LOW("Got buffer, position = %d",total_id3v2_tags_length,0,0);
      
      } else {
        MSG_ERROR("ID3V2 tag > file_len - Unable to play the file ",0,0,0);
        if( (mpeg_parse_ctl->track_buf_1 != buffer) && (buffer != NULL) ) {
            audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, buffer);
            MSG_LOW("Freed buffer",0,0,0);
        }
        return(FALSE);
      }
    }
  }
  
  if (total_id3v2_tags_length) {
    sync_start = mpeg_parse_ctl->find_sync_func(&buffer, data_len, FALSE);
  }
  else {
    data_len = mpeg_parse_ctl->track_1_length;
    sync_start = mpeg_parse_ctl->find_sync_func(&buffer, data_len, TRUE);
  } 
  
  /* Depending on the fact the sync_start should at least be less than
     data_len - sync_word_length */
  if(sync_start == data_len) {
    MSG_MED("Can't find sync!",0,0,0);
    if(mpeg_parse_ctl->track_buf_1 != buffer) {
      audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, buffer);
    }
    return(FALSE);
  }

  /* Advance buffer to point to the start of the sync */
  mpeg_parse_ctl->first_sync = total_id3v2_tags_length + sync_start;
  mpeg_parse_ctl->track_1_start = mpeg_parse_ctl->first_sync;
  if(sync_start != 0) {
    if( (mpeg_parse_ctl->track_buf_1 != buffer) && (buffer != NULL) ) {
      audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, buffer);
    }

    audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0, 
                                         (void *) mpeg_parse_ctl->track_buf_1);
    MSG_LOW("free for data in find first frame",0,0,0);

    if(audmain_get_data(handle, mpeg_parse_ctl->track_1_start,
                   &(mpeg_parse_ctl->track_buf_1)) != AUDMAIN_STATUS_SUCCESS) {
      MSG_LOW("req for data in find first frame",0,0,0);
      return(FALSE);
    }
    
    if(((mpeg_parse_ctl->track_1_start + data_len)- sync_start) >=
                                         mpeg_parse_ctl->file_len) {
      mpeg_parse_ctl->track_1_length = data_len - sync_start;   
    }
    mpeg_parse_ctl->file_location = mpeg_parse_ctl->track_1_start +
                                                mpeg_parse_ctl->track_1_length;
  } else if(mpeg_parse_ctl->track_buf_1 != buffer) {
    audmain_do_play_cb(handle, SND_CMX_AF_FREE_BUF, 0,
                                         (void *) mpeg_parse_ctl->track_buf_1);

    mpeg_parse_ctl->track_buf_1    = buffer;
    mpeg_parse_ctl->file_location  = mpeg_parse_ctl->track_1_start + data_len;
    mpeg_parse_ctl->track_1_length = data_len;
  }

  /* Setup adec buffers */
  main_parse_ctl->current_buf.adec_cmd.adec_buf1 = mpeg_parse_ctl->track_buf_1;
  main_parse_ctl->current_buf.adec_cmd.adec_num_buf1=
                                                mpeg_parse_ctl->track_1_length;

  /* Return our parse control structure as a return handle */
  main_parse_ctl->tracks = mpeg_parse_ctl;

  return(TRUE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_setup_track2

DESCRIPTION
  Sets up the track2 buffer.

DEPENDENCIES
  Should be called after track1 has been setup using audmpeg_find_first_frame()

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_setup_track2(
  snd_af_access_type          *handle,
  audmain_parse_ctl_type      *main_parse_ctl,
  audmpeg_parse_ctl_type      *mpeg_parse_ctl
)
{
  /* Load in the second buffer */
  audmain_set_read_sync(FALSE);
  mpeg_parse_ctl->track_2_start = mpeg_parse_ctl->file_location;
  if(mpeg_parse_ctl->file_location < mpeg_parse_ctl->file_len) {
    audmain_get_data(handle, mpeg_parse_ctl->track_2_start,
                     &(mpeg_parse_ctl->track_buf_2));
    mpeg_parse_ctl->track_2_loading = TRUE;
    mpeg_parse_ctl->file_location += mpeg_parse_ctl->track_length;
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_adec_adjust

DESCRIPTION
  This function adjusts adec buffers to reflect their newly used data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
audmain_status_type audmpeg_adec_adjust (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *bytes_used
) 
{
  audmpeg_parse_ctl_type  *mpeg_ptr;
  audmain_cmd_buf_type    *cmd_buf;
  uint32                  num_bytes;
  uint32                  dummy;

  if(parse_ctl != NULL) {

    num_bytes = *bytes_used;
    MSG_LOW("audmpeg_adec_adjust- num bytes used = %d",num_bytes ,0,0);
    mpeg_ptr = (audmpeg_parse_ctl_type *) parse_ctl->tracks;
    cmd_buf = &parse_ctl->current_buf;

   MSG_LOW("audmpeg_adec_adjust1- cmd_buf->adec_cmd.adec_num_buf1 = %d",cmd_buf->adec_cmd.adec_num_buf1 ,0,0);
   MSG_LOW("audmpeg_adec_adjust1- cmd_buf->adec_cmd.adec_num_buf2 = %d",cmd_buf->adec_cmd.adec_num_buf2 ,0,0);
    if(num_bytes > cmd_buf->adec_cmd.adec_num_buf1) {
      num_bytes -= cmd_buf->adec_cmd.adec_num_buf1;
      cmd_buf->adec_cmd.adec_buf1 += cmd_buf->adec_cmd.adec_num_buf1;
      cmd_buf->adec_cmd.adec_num_buf1 = 0;
      cmd_buf->adec_cmd.adec_buf2 += num_bytes;
      cmd_buf->adec_cmd.adec_num_buf2 -= num_bytes;
    } else {
      cmd_buf->adec_cmd.adec_buf1 += num_bytes;
      cmd_buf->adec_cmd.adec_num_buf1 -= num_bytes;
    }
   MSG_LOW("audmpeg_adec_adjust2- cmd_buf->adec_cmd.adec_num_buf1 = %d",cmd_buf->adec_cmd.adec_num_buf1 ,0,0);
   MSG_LOW("audmpeg_adec_adjust2- cmd_buf->adec_cmd.adec_num_buf2 = %d",cmd_buf->adec_cmd.adec_num_buf2 ,0,0);

    if(mpeg_ptr->track_1_start < mpeg_ptr->track_2_start) {
      if((cmd_buf->adec_cmd.adec_num_buf1 == 0) &&
         (mpeg_ptr->track_buf_1 != NULL)) {
        mpeg_ptr->track_1_length = 0;
      }
      if((cmd_buf->adec_cmd.adec_num_buf2 == 0) &&
         (mpeg_ptr->track_buf_2 != NULL)) {
        mpeg_ptr->track_2_length = 0;
      }
    } else {
      if((cmd_buf->adec_cmd.adec_num_buf1 == 0) &&
         (mpeg_ptr->track_buf_2 != NULL)) {
        mpeg_ptr->track_2_length = 0;
      }
      if((cmd_buf->adec_cmd.adec_num_buf2 == 0) &&
         (mpeg_ptr->track_buf_1 != NULL)) {
        mpeg_ptr->track_1_length = 0;
      }
    }

    /* Try to reload the buffers if they are empty. Otherwise we'll have to
     * until the next DSP interrupt to trigger a reload.
     * This can help alleviate client side data delays.
     */
    if(mpeg_ptr->track_1_length == 0 || mpeg_ptr->track_2_length == 0) {
    MSG_LOW("audmpeg_adec_adjust- mpeg_ptr->track_1_length = %d",mpeg_ptr->track_1_length ,0,0);
    MSG_LOW("audmpeg_adec_adjust- mpeg_ptr->track_2_length = %d",mpeg_ptr->track_2_length ,0,0);
      audmpeg_age(parse_ctl, &dummy, &dummy);
    }
  }
  return(AUDMAIN_STATUS_SUCCESS);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_parse

DESCRIPTION
  This function parses one event of the MPEG file.  Prepares buffers for data
  transfer.

DEPENDENCIES
  Must call audmpeg_age() before calling this function!

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
audmain_status_type audmpeg_parse (
  audmain_parse_ctl_type  *parse_ctl,
  audmain_cmd_buf_type    *cmd_buf,
  uint32                  *time_remain
) {
  audmpeg_parse_ctl_type *mpeg_ptr;
  audmain_status_type    ret_code = AUDMAIN_STATUS_FAILURE;

  if(parse_ctl != NULL) {

    mpeg_ptr     = (audmpeg_parse_ctl_type *) parse_ctl->tracks;

    /* End of playback? */
    if(((mpeg_ptr->file_location >= mpeg_ptr->file_len) &&
        (mpeg_ptr->track_buf_1 == NULL) &&
        (mpeg_ptr->track_buf_2 == NULL) &&
        (!mpeg_ptr->track_1_loading)    &&
        (!mpeg_ptr->track_2_loading) ) ){
      ret_code = AUDMAIN_STATUS_DONE;
    } else if (mpeg_ptr->read_failed == TRUE) {
      ret_code = mpeg_ptr->error_status;

    } else {
      ret_code = AUDMAIN_STATUS_SUCCESS;

      if(mpeg_ptr->resync_state != AUDMPEG_RESYNC_DONE) {
        /* Wait for any pending RESYNC request to complete */
        return(ret_code);
      }

      MSG_LOW("audmpeg_parse-cmd_buf->adec_cmd.adec_num_buf1= %d ", cmd_buf->adec_cmd.adec_num_buf1, 0, 0);
      MSG_LOW("audmpeg_parse-cmd_buf->adec_cmd.adec_num_buf2= %d ", cmd_buf->adec_cmd.adec_num_buf2, 0, 0);
      if(cmd_buf->adec_cmd.adec_num_buf1 == 0) {
        /* First buffer used up, refresh with new data */
        if(cmd_buf->adec_cmd.adec_num_buf2 == 0) {
          /* Second buffer used, move new data from tracks */
          if(mpeg_ptr->track_1_start < mpeg_ptr->track_2_start) {
            cmd_buf->adec_cmd.adec_buf1 = mpeg_ptr->track_buf_1;
            cmd_buf->adec_cmd.adec_num_buf1 = mpeg_ptr->track_1_length;
         MSG_LOW("audmpeg_parse-(copy track_buf1)cmd_buf->adec_cmd.adec_num_buf1= %d ", cmd_buf->adec_cmd.adec_num_buf1, 0, 0);
          } else {
            cmd_buf->adec_cmd.adec_buf1 = mpeg_ptr->track_buf_2;
            cmd_buf->adec_cmd.adec_num_buf1 = mpeg_ptr->track_2_length;
         MSG_LOW("audmpeg_parse-(copy track_buf2)cmd_buf->adec_cmd.adec_num_buf1= %d ", cmd_buf->adec_cmd.adec_num_buf1, 0, 0);
          }
          if(cmd_buf->adec_cmd.adec_buf1 == NULL) {
            cmd_buf->adec_cmd.adec_num_buf1 = 0;
            *time_remain = 250;
            MSG_MED("Buffer underflow.", 0,0,0);
            ret_code = AUDMAIN_STATUS_BUFF_ERR;
          }
        } else {
          /* Second buffer partially used, move new data from 2nd buffer */
          MSG_LOW("Second buffer partially used, move new data from 2nd buffer ", 0, 0, 0);
          
          MSG_LOW("audmpeg_parse-(copy adec_buf2)cmd_buf->adec_cmd.adec_num_buf1= %d ", cmd_buf->adec_cmd.adec_num_buf1, 0, 0);
          cmd_buf->adec_cmd.adec_num_buf1 = cmd_buf->adec_cmd.adec_num_buf2;
          cmd_buf->adec_cmd.adec_buf1 = cmd_buf->adec_cmd.adec_buf2;
          MSG_LOW("audmpeg_parse-(copied adec_buf2)cmd_buf->adec_cmd.adec_num_buf1= %d ", cmd_buf->adec_cmd.adec_num_buf1, 0, 0);
          /* Kill second buffer */
          cmd_buf->adec_cmd.adec_num_buf2 = 0;
          cmd_buf->adec_cmd.adec_buf2 = NULL;
       MSG_LOW("Kill second buffer- cmd_buf->adec_cmd.adec_num_buf2 %d ", cmd_buf->adec_cmd.adec_num_buf2, 0, 0);
        }
      }

      if(cmd_buf->adec_cmd.adec_num_buf2 == 0) {
        /* Second buffer used, setup buffer */
        if(mpeg_ptr->track_1_start < mpeg_ptr->track_2_start) {
          cmd_buf->adec_cmd.adec_buf2 = mpeg_ptr->track_buf_2;
          cmd_buf->adec_cmd.adec_num_buf2 = mpeg_ptr->track_2_length;
          MSG_LOW("audmpeg_parse-(copy track_buf2)cmd_buf->adec_cmd.adec_num_buf2= %d ", cmd_buf->adec_cmd.adec_num_buf2, 0, 0);
        } else {
          cmd_buf->adec_cmd.adec_buf2 = mpeg_ptr->track_buf_1;
          cmd_buf->adec_cmd.adec_num_buf2 = mpeg_ptr->track_1_length;
          MSG_LOW("audmpeg_parse-(copy track_buf1)cmd_buf->adec_cmd.adec_num_buf2= %d ", cmd_buf->adec_cmd.adec_num_buf2, 0, 0);
        }
        if(cmd_buf->adec_cmd.adec_buf2 == NULL) {
          cmd_buf->adec_cmd.adec_num_buf2 = 0;
        }
      }

      if(cmd_buf->adec_cmd.adec_num_buf1 +
         cmd_buf->adec_cmd.adec_num_buf2 == 1) {
        if(mpeg_ptr->file_location >= mpeg_ptr->file_len) {
          /* We'll never be able to write this one odd byte, since the DSP
           * can only handle words.
           */
          if (cmd_buf->adec_cmd.adec_num_buf1 == 1) {
            cmd_buf->adec_cmd.adec_num_buf1++;
          }
          else if (cmd_buf->adec_cmd.adec_num_buf2 == 1) {
            cmd_buf->adec_cmd.adec_num_buf2++;
          }
        }
        else {
          MSG_MED("Buffer underflow.", 0,0,0);
          ret_code = AUDMAIN_STATUS_BUFF_ERR;
        }
      }
    }
  }
  return(ret_code);
}

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_pad_file

DESCRIPTION
  Appends a zero-filled"pad" buffer at the end of the file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_pad_file (
  audmpeg_parse_ctl_type  *mpeg_ptr,
  uint8                   **buf_ptr,
  uint32                  *track_len_ptr
)
{
  if (mpeg_ptr->file_pad_req) {
    

    mpeg_ptr->track_buf_dynamic = *buf_ptr =
      (uint8 *) malloc(mpeg_ptr->file_pad_length);  

    if (*buf_ptr != NULL) {
      audmpeg_mem_alloc_cnt++;

      /* Increment file length by pad length */
      mpeg_ptr->file_len += mpeg_ptr->file_pad_length; 
      /* Increment the file location by pad length */ 
      mpeg_ptr->file_location += mpeg_ptr->file_pad_length;
      *track_len_ptr = mpeg_ptr->file_pad_length;     

      /* Initialize to 0 */     
      memset(*buf_ptr, 0, mpeg_ptr->file_pad_length);

     /* Clear flag so that we pad only one time per file. */
     mpeg_ptr->file_pad_req = FALSE;

    } 
  } 
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_age

DESCRIPTION
  This function checks to see if the buffers are ready for use.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_age (
  audmain_parse_ctl_type  *parse_ctl,
  uint32                  *elapsed_time,
  uint32                  *adjusted_time
) 
{
  audmpeg_parse_ctl_type  *mpeg_ptr;
  audmain_cmd_buf_type    *cmd_buf;

  if(parse_ctl == NULL) {
    MSG_ERROR("Invalid pointer", 0, 0, 0);
    return;
  }

  if(!(parse_ctl->status_flag & AUDMAIN_MASK_INIT_DONE)) {
    *elapsed_time  = 0;
  }
  *adjusted_time = *elapsed_time;

  mpeg_ptr = (audmpeg_parse_ctl_type *) parse_ctl->tracks;
  cmd_buf = &parse_ctl->current_buf;
  MSG_LOW("SR_debug mpeg_ptr->resync_state %d ", mpeg_ptr->resync_state, 0, 0);
  switch (mpeg_ptr->resync_state) {
    case AUDMPEG_RESYNC_START:
      cmd_buf->adec_cmd.adec_num_buf1 = cmd_buf->adec_cmd.adec_num_buf2 = 0;
      mpeg_ptr->track_1_length = mpeg_ptr->track_2_length = 0;

      /* Wait for pending reads to complete, if any */
      MSG_LOW("mpeg_ptr->track_1_loading = %d ", mpeg_ptr->track_1_loading, 0, 0);
      MSG_LOW("mpeg_ptr->track_2_loading = %d ", mpeg_ptr->track_2_loading, 0, 0);
      if (mpeg_ptr->track_1_loading || mpeg_ptr->track_2_loading) {
        mpeg_ptr->resync_state = AUDMPEG_RESYNC_DUMP;
        return;
      }
      else {
        mpeg_ptr->resync_state = AUDMPEG_RESYNC_LOAD;
      }
      break;

    case AUDMPEG_RESYNC_DUMP:
      if(mpeg_ptr->track_1_loading || mpeg_ptr->track_2_loading) {
        return;
      }
      else {
        /* Dump the data from the pending reads */
        mpeg_ptr->track_1_length = mpeg_ptr->track_2_length = 0;

        /* Now wait for the data loads which will actually be used */
        mpeg_ptr->resync_state = AUDMPEG_RESYNC_LOAD;
      }
      break;

    case AUDMPEG_RESYNC_LOAD:
      if(!mpeg_ptr->track_1_loading || !mpeg_ptr->track_2_loading) {
        if (mpeg_ptr->track_1_length) {
          cmd_buf->adec_cmd.adec_buf1      = mpeg_ptr->track_buf_1;
          cmd_buf->adec_cmd.adec_num_buf1  = mpeg_ptr->track_1_length;
        }
        if (mpeg_ptr->track_2_length) {
          cmd_buf->adec_cmd.adec_buf2      = mpeg_ptr->track_buf_2;
          cmd_buf->adec_cmd.adec_num_buf2  = mpeg_ptr->track_2_length;
        }
        mpeg_ptr->resync_state = AUDMPEG_RESYNC_DONE;
        audmpeg_resync(parse_ctl, cmd_buf);
        return;
      }
      break;

    case AUDMPEG_RESYNC_DONE:
      break;
  }
  MSG_LOW("SR_debug mpeg_ptr->track_1_length %d ", mpeg_ptr->track_1_length, 0, 0);
  MSG_LOW("SR_debug mpeg_ptr->track_1_loading %d ", mpeg_ptr->track_1_loading, 0, 0);
  if((mpeg_ptr->track_1_length == 0) && (!mpeg_ptr->track_1_loading)) {
    /* If buf 1 is empty and unused, free and load */
    mpeg_ptr->track_1_start = mpeg_ptr->file_location;
    if (mpeg_ptr->track_buf_1 != NULL) {
      if (mpeg_ptr->track_buf_dynamic != mpeg_ptr->track_buf_1) {
        audmain_do_play_cb(mpeg_ptr->audmpeg_handle, SND_CMX_AF_FREE_BUF, 0,
                           (void *) mpeg_ptr->track_buf_1);
          MSG_LOW(" free in age for buf1",0,0,0);
      }
      else {
        mpeg_ptr->track_buf_dynamic = NULL;
        audmpeg_mem_alloc_cnt--;
        free(mpeg_ptr->track_buf_1); 
        MSG_LOW(" free in age else case for buf1",0,0,0);
      }    
      mpeg_ptr->track_buf_1 = NULL;
    }
    if(mpeg_ptr->file_location < mpeg_ptr->file_len) {
    (void) audmain_get_data(mpeg_ptr->audmpeg_handle,
                            mpeg_ptr->file_location,
                            &(mpeg_ptr->track_buf_1));
      MSG_LOW(" Req in age for buf1",0,0,0);
      mpeg_ptr->track_1_loading = TRUE;
      mpeg_ptr->file_location += mpeg_ptr->track_length;
      MSG_LOW("SR_debug Request for data 1", 0, 0, 0);
    } else if (mpeg_ptr->file_pad_req) {
      MSG_LOW("request for padding 1500 bytes of zeros - buf1",0,0,0);
      audmpeg_pad_file(mpeg_ptr, &mpeg_ptr->track_buf_1,
                       &mpeg_ptr->track_1_length);
    } 
  } 

  MSG_LOW("SR_debug mpeg_ptr->track_2_length %d ", mpeg_ptr->track_2_length, 0, 0);
  MSG_LOW("SR_debug mpeg_ptr->track_2_loading %d ", mpeg_ptr->track_2_loading, 0, 0);
  if((mpeg_ptr->track_2_length == 0) && (!mpeg_ptr->track_2_loading)) {
    /* If buf 2 is empty and unused, free and load */
    mpeg_ptr->track_2_start = mpeg_ptr->file_location;
    if (mpeg_ptr->track_buf_2 != NULL) { 
      if (mpeg_ptr->track_buf_dynamic !=  mpeg_ptr->track_buf_2) {
        audmain_do_play_cb(mpeg_ptr->audmpeg_handle, SND_CMX_AF_FREE_BUF, 0,
                           (void *) mpeg_ptr->track_buf_2);
        MSG_LOW(" free in age for buf2",0,0,0);
      } else {
        mpeg_ptr->track_buf_dynamic = NULL;
        audmpeg_mem_alloc_cnt--;
        free(mpeg_ptr->track_buf_2); 
        MSG_LOW(" free in age else case for buf2",0,0,0);
      }    
      mpeg_ptr->track_buf_2 = NULL;
    }
    if(mpeg_ptr->file_location < mpeg_ptr->file_len) {
      (void) audmain_get_data(mpeg_ptr->audmpeg_handle,
                              mpeg_ptr->file_location,
                              &(mpeg_ptr->track_buf_2));
      MSG_LOW(" Req in age for buf2",0,0,0);
      mpeg_ptr->track_2_loading = TRUE;
      mpeg_ptr->file_location += mpeg_ptr->track_length;
      MSG_LOW("SR_debug Request for data 2", 0, 0, 0);
    } else if ((mpeg_ptr->file_pad_req) && (mpeg_ptr->track_1_loading == FALSE)) {
      MSG_LOW("request for padding 1500 bytes of zeros buf2",0,0,0);
      audmpeg_pad_file(mpeg_ptr, &mpeg_ptr->track_buf_2,
                       &mpeg_ptr->track_2_length);
    }
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_reset

DESCRIPTION
  This function resets internal MPEG buffers after a song is unloaded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_reset (
  audmpeg_parse_ctl_type  *parse_ctl
)
{
  if (parse_ctl != NULL) {
    if (parse_ctl->track_buf_dynamic != NULL) {
      audmpeg_mem_alloc_cnt--;
      free(parse_ctl->track_buf_dynamic); 
      parse_ctl->track_buf_dynamic = NULL;
    }

    (void) memset(parse_ctl, 0, sizeof(audmpeg_parse_ctl_type));

    parse_ctl->first_sync       = 0;
    parse_ctl->read_failed      = FALSE;
    parse_ctl->resync_state     = AUDMPEG_RESYNC_DONE;
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/* <EJECT> */
/*===========================================================================

FUNCTION audmpeg_setup_streaming

DESCRIPTION
  This function sets up the buffers to handle streaming audio.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_setup_streaming (
   uint8                       *buffer,
   snd_af_access_type          *handle,
   audmain_parse_ctl_type      *main_parse_ctl,
   audmpeg_parse_ctl_type      *mpeg_parse_ctl
)
{
   /* Setup our parse control structure */

   /* Set file length to a very large number */
   mpeg_parse_ctl->file_len          = AUDMPEG_MAX_FILE_LEN;

   mpeg_parse_ctl->track_buf_1       = buffer;
   mpeg_parse_ctl->track_buf_2       = NULL;
   mpeg_parse_ctl->audmpeg_handle    = handle;
   mpeg_parse_ctl->track_1_start     = 0;
   mpeg_parse_ctl->track_2_start     = 0;
   mpeg_parse_ctl->track_1_length    = main_parse_ctl->buffer_size;
   mpeg_parse_ctl->track_length      = main_parse_ctl->buffer_size;
   mpeg_parse_ctl->file_location     = main_parse_ctl->buffer_size;
   mpeg_parse_ctl->first_sync        = 0;

   /* Setup adec buffers */
   main_parse_ctl->current_buf.adec_cmd.adec_buf1 = mpeg_parse_ctl->track_buf_1;
   main_parse_ctl->current_buf.adec_cmd.adec_num_buf1=
                                                 mpeg_parse_ctl->track_1_length;

   /* Return our parse control structure as a return handle */
   main_parse_ctl->tracks = mpeg_parse_ctl;

   audmpeg_setup_track2(handle, main_parse_ctl, mpeg_parse_ctl);
   return(TRUE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3tag_process

DESCRIPTION
  This function start processing the ID3 tag from the beginning of the clip.

DEPENDENCIES
  None

RETURN VALUE
  TURE  - Success
  FALSE - Failed

SIDE EFFECTS
  If valid ID3 tag has been found, corresponding id3_tag data structure is
  updated.
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_id3tag_process (
  snd_af_access_type     *handle,
  audmpeg_parse_ctl_type *parse_ctl,
  audmpeg_id3_tag_type   *id3_tag
)
{
  int8    flag;
  uint16  tag_version;  
  uint32  tag_size;
  uint32  v1_tag_size, ext_hdr = 0;
  audmpeg_id3v2_tag_type *v2_tag;
  boolean support;

  audmpeg_id3_tag_version_type  tag_type = AUDMPEG_TAGTYPE_NONE;

  /* reset for SYNC read data from the beginning of the file */
  parse_ctl->track_2_start = parse_ctl->id3v2_tag_start;

  audmain_get_data(handle, 
                   parse_ctl->track_2_start,
                   &(parse_ctl->track_buf_2));
  if(parse_ctl->track_buf_2 == NULL)
  {
    MSG_ERROR("Audio data reading error",0,0,0);
    return;
  }

  v2_tag = &(id3_tag->v2_tag);

  /* First check if there is any ID3V2 header existing */
  if ((tag_type = audmpeg_find_tag_type(parse_ctl->track_buf_2)) ==
      AUDMPEG_TAGTYPE_ID3V2) {

    id3_tag->v2_tag_found = TRUE;
    id3_tag->tag_type  |= AUDMPEG_TAGTYPE_ID3V2;
    v2_tag->tag_locate  = ID3V2_TAG_LOCATE_PREPEND_ONLY; 
 
    audmpeg_id3v2_parse_tag(parse_ctl,
                            parse_ctl->track_buf_2,
                            parse_ctl->track_length,
                            v2_tag);

    if (v2_tag->next_file_pos > 0) {
      parse_ctl->ops |= AUDMPEG_PROC_OPS_PROCESSING_GETTAG; 
    } else {
      parse_ctl->ops = AUDMPEG_PROC_OPS_NONE;
    }
  }

  /* Tag may be existing at the end of file, start find frame sync from the 
   * beginnig of file
   */

  /* since ID3V2 could not be found at the beginning of the file, then start
   * searching from the end to see whether it is a ID3V1 or ID3V2 footer
   * is existing
   */

  /* SYNC read - Load in the second buffer with the ID3V1 tag offset */
  audmain_do_play_cb(handle, 
                     SND_CMX_AF_FREE_BUF, 
                     0, 
                     (void *)parse_ctl->track_buf_2); 

  parse_ctl->track_2_start = parse_ctl->file_len - parse_ctl->track_length;
  audmain_get_data(handle, 
                   parse_ctl->track_2_start,
                   &(parse_ctl->track_buf_2));
  if(parse_ctl->track_buf_2 == NULL)
  {
    MSG_ERROR("Audio data reading error",0,0,0);
    return;
  }

  if (audmpeg_find_tag_type(parse_ctl->track_buf_2 +
                            (parse_ctl->track_length - 
                             AUDMPEG_ID3V1_TAG_LEN)) ==
      AUDMPEG_TAGTYPE_ID3V1) {

    /* This is ID3V1 tag
     */
    id3_tag->v1_tag_found = TRUE;

    audmpeg_extract_id3v1_tag (parse_ctl->track_buf_2 +
                               (parse_ctl->track_length - 
                                AUDMPEG_ID3V1_TAG_LEN),
                               &(id3_tag->v1_tag));

    id3_tag->tag_type |= AUDMPEG_TAGTYPE_ID3V1;

    if (!(id3_tag->tag_type & AUDMPEG_TAGTYPE_ID3V2)) {

      parse_ctl->ops = AUDMPEG_PROC_OPS_NONE; 
    }
  }

  /* Check whether there is any ID3V2 tag at the end of file */
  if (id3_tag->tag_type & AUDMPEG_TAGTYPE_ID3V1) { 
    v1_tag_size = AUDMPEG_ID3V1_TAG_LEN;
  } else {
    v1_tag_size = 0;
  }

  if (audmpeg_find_tag_type(parse_ctl->track_buf_2     +
                            (parse_ctl->track_length -
                             v1_tag_size             - 
                             AUDMPEG_ID3V2_HEADER_FOOTER_LEN)) ==
      AUDMPEG_TAGTYPE_ID3V2_FOOTER) {
  
    /* This is ID3V2 tag but starts from the end of file
     */
    if (v1_tag_size > 0) {
      id3_tag->tag_type |= AUDMPEG_TAGTYPE_ID3V2;
    } else {
      id3_tag->tag_type = AUDMPEG_TAGTYPE_ID3V2;
    }

    v2_tag->tag_locate = ID3V2_TAG_LOCATE_POSTPEND_ONLY; 
  
    support = 
      audmpeg_id3v2_parse_tag_header((parse_ctl->track_buf_2 +
                                       (parse_ctl->track_length - 
                                        AUDMPEG_ID3V1_TAG_LEN - 
                                        AUDMPEG_ID3V2_HEADER_FOOTER_LEN)),
                                      &tag_version,
                                      &flag,
                                      &tag_size);
    if (support) {
      v2_tag->next_file_pos = 
        (parse_ctl->file_len   - 
         v1_tag_size           -
         tag_size              +
         AUDMPEG_ID3V2_HEADER_FOOTER_LEN);

      ext_hdr = audmpeg_id3v2_check_ext_hdr((parse_ctl->track_buf_2 +
                                             (parse_ctl->track_length - 
                                              AUDMPEG_ID3V1_TAG_LEN - 
                                              AUDMPEG_ID3V2_HEADER_FOOTER_LEN)), 
                                            v2_tag);
      v2_tag->next_file_pos           += ext_hdr; 
      v2_tag->already_parsed_tag_size  = ext_hdr;
    } else {
      v2_tag->next_file_pos = 0;
    }
     
    v2_tag->version   = tag_version;
    v2_tag->hdr_flag  = flag;
    v2_tag->tag_size  = tag_size;

    v2_tag->postpend_size += tag_size + ext_hdr + v1_tag_size +
                              (v2_tag->hdr_flag & 
                               AUDMPEG_ID3V2_TAG_FLAG_FOOTERPRESENT) ?
                               (AUDMPEG_ID3V2_HEADER_FOOTER_LEN * 2) :
                               (AUDMPEG_ID3V2_HEADER_FOOTER_LEN);

    if (v2_tag->next_file_pos > 0) {
      parse_ctl->ops |= AUDMPEG_PROC_OPS_PROCESSING_GETTAG; 
    } else {
      parse_ctl->ops = AUDMPEG_PROC_OPS_NONE;
    }
  }

  audmain_do_play_cb(handle, 
                     SND_CMX_AF_FREE_BUF, 
                     0, 
                     (void *) parse_ctl->track_buf_2);

  return;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_find_tag_type

DESCRIPTION
  Findes the tag type/version - v1 or v2, based on the key word.

DEPENDENCIES
  None

RETURN VALUE
  audmpeg_id3_tag_version_type

SIDE EFFECTS
 
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
audmpeg_id3_tag_version_type audmpeg_find_tag_type(
  const uint8 *buf
)
{
  audmpeg_id3_tag_version_type ret_val = AUDMPEG_TAGTYPE_NONE;

  if ((buf[0] == 'T') &&
      (buf[1] == 'A') && 
      (buf[2] == 'G')) {

    ret_val = AUDMPEG_TAGTYPE_ID3V1; 

  } else if (((buf[0] == 'I' && buf[1] == 'D' && buf[2] == '3') ||
              (buf[0] == '3' && buf[1] == 'D' && buf[2] == 'I')) &&
             (buf[3] < 0xff && buf[4] < 0xff &&
              buf[6] < 0x80 && buf[7] < 0x80 
              && buf[8] < 0x80 && buf[9] < 0x80)) {

    ret_val = 
      (buf[0] == 'I' ? AUDMPEG_TAGTYPE_ID3V2 : AUDMPEG_TAGTYPE_ID3V2_FOOTER);
  }
  MSG_MED("audmpeg_find_tag_type tag_type = %d",ret_val,0,0);
  return (ret_val);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_process_tag_frame

DESCRIPTION
  Parse the whole content into id3_tag structure based on the required fields.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -- Parsing is successful
  FALSE -- Parsing is failed

SIDE EFFECTS
 
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_id3v2_process_tag_frame (
  audmpeg_parse_ctl_type        *parse_ctl,
  audmpeg_id3v2_tag_type        *v2_tag
)
{
  uint32  parsed_size = 0 ;

  if (v2_tag->holder.buf == NULL) {
    v2_tag->next_file_pos = 0;
    return (FALSE);
  }

  parsed_size = 
    audmpeg_id3v2_parse_frame(v2_tag);

  MSG_LOW("audmpeg_id3v2_parse_frame done, parsed_size=%d next_file_pos=%d",
                                           parsed_size,v2_tag->next_file_pos,0);
  if (parsed_size == 0) {
    v2_tag->next_file_pos = 0;
    return (FALSE); 
  }

 /* Update the file position with the parsed size and the ID3 header length */
  v2_tag->next_file_pos = parsed_size + AUDMPEG_ID3V2_HEADER_FOOTER_LEN;

  if (v2_tag->tag_size <= v2_tag->next_file_pos) {

    if ((AUDMPEG_ID3V2_TAG_VERSION_MAJOR(v2_tag->version) == 4) &&
        (v2_tag->holder.seek_frame.found == TRUE)) {

      v2_tag->first_seek_buf = TRUE;

      v2_tag->next_file_pos +=
        (((v2_tag->hdr_flag & AUDMPEG_ID3V2_TAG_FLAG_FOOTERPRESENT) ?
          AUDMPEG_ID3V2_HEADER_FOOTER_LEN : 0) + 
         v2_tag->holder.seek_frame.offset);

    } else {
      v2_tag->next_file_pos = 0;
    }
  }

  return (TRUE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_tag_header

DESCRIPTION
  Parse the ID3V2 header to get tag version, flags and tag size.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -- Parsing is successful
  FALSE -- Parsing is failed

SIDE EFFECTS
 
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_id3v2_parse_tag_header (
  uint8  *buf,
  uint16 *version,
  int8   *flag,
  uint32 *size
)
{
  uint8   *buf_ptr = NULL;

  buf_ptr = buf;

  /* skip 3 bytes tag id */
  buf_ptr += 3;

  *version = audmpeg_id3v2_parse_uint(&buf_ptr, 2);     /* version: 2 bytes */

  *flag    = audmpeg_id3v2_parse_uint(&buf_ptr, 1);     /* flags:   1 bytes */

  /* syncsafe encoded tag header size applies to both ID3V2.3 and id3V2.4 */
  *size    = audmpeg_id3v2_parse_syncsafe(&buf_ptr, 4);

  MSG_MED("audmpeg_id3v2_parse_tag_header - tag size = %d", size, 0, 0);

  if ((AUDMPEG_ID3V2_TAG_VERSION_MAJOR(*version) < 3) || 
      (AUDMPEG_ID3V2_TAG_VERSION_MAJOR(*version) > 4)) {

    MSG_ERROR("ID3V2 version %d is not supported!",*version,0,0);
    return (FALSE);
  }

  return (TRUE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_uint

DESCRIPTION
  Parse the byte(s) to get the corresponding integer value.

DEPENDENCIES
  None

RETURN VALUE
  integer value of the byte(s).

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint32 audmpeg_id3v2_parse_uint (
  uint8  **ptr,
  uint8  bytes
)
{
  uint32 value = 0;

  if (bytes < 1 || bytes > 4) {
    return 0;
  }

  switch (bytes) {
    case 4: value = (value << 8) | *(*ptr)++;
    case 3: value = (value << 8) | *(*ptr)++;
    case 2: value = (value << 8) | *(*ptr)++;
    case 1: value = (value << 8) | *(*ptr)++;
  }

  return value;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_syncsafe

DESCRIPTION
  Parse the syncsafe byte2 to get the corresponding integer value.

DEPENDENCIES
  None

RETURN VALUE
  integer value of the byte(s).

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint32 audmpeg_id3v2_parse_syncsafe (
  uint8  **ptr,
  uint8  bytes
)
{
  uint32 value = 0;

  switch (bytes) {
  case 5: value = (value << 4) | (*(*ptr)++ & 0x0f);
  case 4: value = (value << 7) | (*(*ptr)++ & 0x7f);
          value = (value << 7) | (*(*ptr)++ & 0x7f);
          value = (value << 7) | (*(*ptr)++ & 0x7f);
          value = (value << 7) | (*(*ptr)++ & 0x7f);
      break;
    default:
      value = 0;
      break;
  }
  return value;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_frame

DESCRIPTION
  Parse the syncsafe byte2 to get the corresponding integer value.

DEPENDENCIES
  None

RETURN VALUE
  integer value of the byte(s) been parsed

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint32 audmpeg_id3v2_parse_frame (
  audmpeg_id3v2_tag_type *v2_tag
)
{
  uint32  cnt;
  uint8   *buf_ptr, *temp_buf_ptr; 
  uint8   major_version;
  uint32  frame_len;
  boolean foot_present = FALSE;
  uint32  already_parsed_tag_size;

  major_version = AUDMPEG_ID3V2_TAG_VERSION_MAJOR(v2_tag->version);
  /* copy the already parsed tag size */
  already_parsed_tag_size = v2_tag->already_parsed_tag_size;
  if (v2_tag->holder.buf_len < AUDMPEG_ID3V2_FRAME_HEADER_LEN) { 
    MSG_ERROR("buf length = %d is too short",v2_tag->holder.buf_len,0,0);
    return (0);
  }

  foot_present = (v2_tag->hdr_flag) & AUDMPEG_ID3V2_TAG_FLAG_FOOTERPRESENT;

  temp_buf_ptr = buf_ptr = v2_tag->holder.buf;

  do {

    if (!foot_present) {
      /* check if this is padding data, if true, must 
       * be at the end of tag now */
      if (audmpeg_id3v2_check_padding(temp_buf_ptr, v2_tag)) {

        /* It is already padding, stop parsing frame */
        return (v2_tag->already_parsed_tag_size);

      } /* check padding */
    } /* if (!foot_present) */

    /* CHECK for an APIC frame */
    if (audmpeg_id3v2_findframe(v2_tag->apic.id, temp_buf_ptr,
                                   AUDMPEG_ID3V2_FRAME_ID_LEN)) {
      MSG_HIGH("APIC ID3v2 frame found",0,0,0);
      v2_tag->apic.found  = TRUE;
      temp_buf_ptr += AUDMPEG_ID3V2_FRAME_ID_LEN; 
      frame_len = ((major_version == 3) ? 
                 audmpeg_id3v2_parse_uint(&temp_buf_ptr, 4) :   
                 audmpeg_id3v2_parse_syncsafe(&temp_buf_ptr, 4));
      v2_tag->apic.size  = frame_len;
      v2_tag->apic.flags  = (uint16)audmpeg_id3v2_parse_uint(&temp_buf_ptr, 2);
      v2_tag->apic.file_position = v2_tag->already_parsed_tag_size +
                                         AUDMPEG_ID3V2_HEADER_FOOTER_LEN;
      if (((temp_buf_ptr - v2_tag->holder.buf) + frame_len) >
           v2_tag->holder.buf_len) {
        /* check if the frame length is bigger than buffer len 
         * skip the frame, cannot support as of now */
    
        if( frame_len >= v2_tag->holder.buf_len) { 
            v2_tag->already_parsed_tag_size = already_parsed_tag_size +
                          (temp_buf_ptr - v2_tag->holder.buf) + frame_len;
        } else {
          v2_tag->already_parsed_tag_size = already_parsed_tag_size +
                                (temp_buf_ptr - v2_tag->holder.buf) - 
                                         AUDMPEG_ID3V2_HEADER_FOOTER_LEN ;
        }      
        return(v2_tag->already_parsed_tag_size);
      }
      temp_buf_ptr += frame_len;  /* Increment the buffer */
    } else if (IS_ID3V2_WFRAME(temp_buf_ptr)) { /* CHECK for Wframes*/
      /* Increment the gframe count */
      v2_tag->gframes_cnt ++;      
      if ((v2_tag->gframes_cnt / AUDMPEG_GFRAME_PRE_ALLOC_MEM) ==
              v2_tag->gframes_prealloc_index) {
        v2_tag->gframes_prealloc_index += 1;
        v2_tag->gframes = 
              realloc( v2_tag->gframes, 
                      sizeof(audmpeg_id3v2_generic_frame_type) * 
             (AUDMPEG_GFRAME_PRE_ALLOC_MEM * 
              v2_tag->gframes_prealloc_index));
        if(v2_tag->gframes == NULL) {
          MSG_ERROR("Realloc failure, no memory available!",0,0,0);
          return (0);
        }
      }
      /* copy the tag ID */
      memcpy(&(v2_tag->gframes[v2_tag->gframes_cnt - 1].id[0]), 
           temp_buf_ptr,AUDMPEG_ID3V2_FRAME_ID_LEN);
      /* reposition buffer */
      temp_buf_ptr += AUDMPEG_ID3V2_FRAME_ID_LEN; 
      frame_len = ((major_version == 3) ? 
                audmpeg_id3v2_parse_uint(&temp_buf_ptr, 4) :   
                 audmpeg_id3v2_parse_syncsafe(&temp_buf_ptr, 4));
      v2_tag->gframes[v2_tag->gframes_cnt - 1].frame_size = frame_len; 
      v2_tag->gframes[v2_tag->gframes_cnt - 1].flags =  
                  (uint16)audmpeg_id3v2_parse_uint(&temp_buf_ptr, 2);

      if (((temp_buf_ptr - v2_tag->holder.buf) + frame_len) >
        v2_tag->holder.buf_len) {
        /* check if the frame length is bigger than buffer len 
         * skip the frame, cannot support as of now */

        if( frame_len >= v2_tag->holder.buf_len) { 
           v2_tag->already_parsed_tag_size = already_parsed_tag_size +
              (temp_buf_ptr - v2_tag->holder.buf) + frame_len;
        }
        /* check if this is an invalid/garbage frame_len */
        if( v2_tag->already_parsed_tag_size > v2_tag->tag_size) {
          MSG_ERROR ("corrupt ID3V2 tag encountered, stop parsing ",0,0,0); 
          v2_tag->already_parsed_tag_size = v2_tag->tag_size; 
          return 0;
        }
        return(v2_tag->already_parsed_tag_size);
      }

      /* note the tag content information */
      v2_tag->gframes[v2_tag->gframes_cnt - 1].info_type = 
                                     AUDMPEG_ID3V2_TAG_INFO_BY_CONTENT;           
      v2_tag->gframes[v2_tag->gframes_cnt - 1].frame_data.
                                   frame_content = malloc(frame_len);
      if ( v2_tag->gframes[v2_tag->gframes_cnt - 1].
                                   frame_data.frame_content == NULL) {
        MSG_ERROR("frame content memory allocate failed!",0,0,0);
        return (0);
      }      
      memcpy(v2_tag->gframes[v2_tag->gframes_cnt - 1].frame_data.frame_content, 
                                             temp_buf_ptr, frame_len);
      temp_buf_ptr += frame_len;
    } else if (IS_ID3V2_TFRAME(temp_buf_ptr)) { /* CHECK for Tframes*/

      /* check for additinal T frames */

      if( audmpeg_id3v2_findframe(id3v2_frames_supported_added[0],temp_buf_ptr,
                                  AUDMPEG_ID3V2_FRAME_ID_LEN) ||
          audmpeg_id3v2_findframe(id3v2_frames_supported_added[1],temp_buf_ptr,
                                  AUDMPEG_ID3V2_FRAME_ID_LEN) ||
          audmpeg_id3v2_findframe(id3v2_frames_supported_added[2],temp_buf_ptr,
                                  AUDMPEG_ID3V2_FRAME_ID_LEN) ||
          audmpeg_id3v2_findframe(id3v2_frames_supported_added[3],temp_buf_ptr,
                                  AUDMPEG_ID3V2_FRAME_ID_LEN) ||
          audmpeg_id3v2_findframe(id3v2_frames_supported_added[4],temp_buf_ptr,
                                  AUDMPEG_ID3V2_FRAME_ID_LEN) ||                                  
          audmpeg_id3v2_findframe(id3v2_frames_supported_added[5],temp_buf_ptr,
                                  AUDMPEG_ID3V2_FRAME_ID_LEN) ) {

            /* Increment the gframe count */
            v2_tag->gframes_cnt ++;      
            if ((v2_tag->gframes_cnt / AUDMPEG_GFRAME_PRE_ALLOC_MEM) ==
                    v2_tag->gframes_prealloc_index) {
              v2_tag->gframes_prealloc_index += 1;
              v2_tag->gframes = 
                    realloc( v2_tag->gframes, 
                            sizeof(audmpeg_id3v2_generic_frame_type) * 
                   (AUDMPEG_GFRAME_PRE_ALLOC_MEM * 
                    v2_tag->gframes_prealloc_index));
              if(v2_tag->gframes == NULL) {
                MSG_ERROR("Realloc failure, no memory available!",0,0,0);
                return (0);
              }
            }
            /* copy the tag ID */
            memcpy(&(v2_tag->gframes[v2_tag->gframes_cnt - 1].id[0]), 
                 temp_buf_ptr,AUDMPEG_ID3V2_FRAME_ID_LEN);
            /* reposition buffer */
            temp_buf_ptr += AUDMPEG_ID3V2_FRAME_ID_LEN; 
            frame_len = ((major_version == 3) ? 
                      audmpeg_id3v2_parse_uint(&temp_buf_ptr, 4) :   
                       audmpeg_id3v2_parse_syncsafe(&temp_buf_ptr, 4));
            v2_tag->gframes[v2_tag->gframes_cnt - 1].frame_size = frame_len; 
            v2_tag->gframes[v2_tag->gframes_cnt - 1].flags =  
                        (uint16)audmpeg_id3v2_parse_uint(&temp_buf_ptr, 2);

            if (((temp_buf_ptr - v2_tag->holder.buf) + frame_len) >
                                              v2_tag->holder.buf_len) {
              /* check if the frame length is bigger than buffer len 
               * skip the frame, cannot support as of now */
      
              if( frame_len >= v2_tag->holder.buf_len) { 
                v2_tag->already_parsed_tag_size = already_parsed_tag_size +
                  (temp_buf_ptr - v2_tag->holder.buf) + frame_len;
              }
              /* check if this is an invalid/garbage frame_len */
              if( v2_tag->already_parsed_tag_size > v2_tag->tag_size) {
                MSG_ERROR ("corrupt ID3V2 tag encountered, stop parsing ",0,0,0); 
                v2_tag->already_parsed_tag_size = v2_tag->tag_size;
                return 0;
              }
              return(v2_tag->already_parsed_tag_size);
            }
            
            /* note the tag content information */
            v2_tag->gframes[v2_tag->gframes_cnt - 1].info_type = 
                                           AUDMPEG_ID3V2_TAG_INFO_BY_CONTENT;           
            v2_tag->gframes[v2_tag->gframes_cnt - 1].frame_data.
                                         frame_content = malloc(frame_len);
            if ( v2_tag->gframes[v2_tag->gframes_cnt - 1].
                                         frame_data.frame_content == NULL) {
              MSG_ERROR("frame content memory allocate failed!",0,0,0);
              return (0);
            }      
            memcpy(v2_tag->gframes[v2_tag->gframes_cnt - 1].frame_data.frame_content, 
                                                   temp_buf_ptr, frame_len);
            temp_buf_ptr += frame_len;

      } else {
        /* CHECK for the other supported frames */
        for (cnt = 0; cnt < AUDMPEG_ID3V2_FRAME_MAX; cnt++) {
          if (!(v2_tag->frames[cnt].found)) {

            if (audmpeg_id3v2_findframe(&(v2_tag->frames[cnt].id[0]),
                              temp_buf_ptr,AUDMPEG_ID3V2_FRAME_ID_LEN)) {
              /* reposition buffer */
              temp_buf_ptr += AUDMPEG_ID3V2_FRAME_ID_LEN; 
              frame_len = ((major_version == 3) ? 
                    audmpeg_id3v2_parse_uint(&temp_buf_ptr, 4) :   
                     audmpeg_id3v2_parse_syncsafe(&temp_buf_ptr, 4));
              v2_tag->frames[cnt].frame_size = frame_len;  
              v2_tag->frames[cnt].flag  = 
                        (uint16)audmpeg_id3v2_parse_uint(&temp_buf_ptr, 2);

              if (((temp_buf_ptr - v2_tag->holder.buf) + frame_len) >
                                              v2_tag->holder.buf_len) {
                /* check if the frame length is bigger than buffer len 
                 * skip the frame, cannot support as of now */

                if( frame_len >= v2_tag->holder.buf_len) { 
                    v2_tag->already_parsed_tag_size = already_parsed_tag_size +
                      (temp_buf_ptr - v2_tag->holder.buf) + frame_len;
                  }
                  /* check if this is an invalid/garbage frame_len */
                if( v2_tag->already_parsed_tag_size > v2_tag->tag_size) {
                  MSG_ERROR ("corrupt ID3V2 tag encountered, stop parsing ",0,0,0); 
                    v2_tag->already_parsed_tag_size = v2_tag->tag_size; 
                  return 0;
                  }
                  return(v2_tag->already_parsed_tag_size);
                }
            
              v2_tag->frames[cnt].found = TRUE;
              v2_tag->frames[cnt].frame_content = malloc(frame_len);
              if (v2_tag->frames[cnt].frame_content == NULL) {
                MSG_ERROR("frame content memory allocate failed!",0,0,0);
                return (0);
              }
              memcpy(v2_tag->frames[cnt].frame_content, temp_buf_ptr, frame_len);
              temp_buf_ptr += frame_len;
              break; /* break for */
            }
          }
        } /* for */
      } /* else if */
    }
  
    /* did not find the frame from our list, check "SEEK" frame */
    if (temp_buf_ptr == buf_ptr) {

      if (major_version == 4 && 
          audmpeg_id3v2_findframe(v2_tag->holder.seek_frame.id, 
                                  temp_buf_ptr, 
                                  AUDMPEG_ID3V2_FRAME_ID_LEN)) {
  
        v2_tag->holder.seek_frame.found  = TRUE;
  
        temp_buf_ptr += (AUDMPEG_ID3V2_FRAME_ID_LEN   + 
                         AUDMPEG_ID3V2_FRAME_SIZE_LEN +
                         AUDMPEG_ID3V2_FRAME_FLAG_LEN);
  
        v2_tag->holder.seek_frame.offset = 
          audmpeg_id3v2_parse_uint(&temp_buf_ptr, 4); 
  
      } else { /* Frames not supported has to be skipped */
        /* reposition buffer */
        temp_buf_ptr += AUDMPEG_ID3V2_FRAME_ID_LEN; 
        frame_len = ((major_version == 3) ? 
                audmpeg_id3v2_parse_uint(&temp_buf_ptr, 4) :   
               audmpeg_id3v2_parse_syncsafe(&temp_buf_ptr, 4));
        if( frame_len > v2_tag->tag_size ) {
          MSG_ERROR("frame len is invalid, corrupted file or wrong alignment",0,0,0);
          return 0;
        }
        
        /* update temp_buf_ptr to starting of next frame */
        temp_buf_ptr += AUDMPEG_ID3V2_FRAME_FLAG_LEN + frame_len;
      }
    }

    buf_ptr = temp_buf_ptr;

    v2_tag->already_parsed_tag_size = already_parsed_tag_size +
                                    (buf_ptr - v2_tag->holder.buf);

  } while ((v2_tag->holder.buf_len - (buf_ptr - v2_tag->holder.buf)) >
           AUDMPEG_ID3V2_FRAME_HEADER_LEN); 

  return ((uint32)(buf_ptr - v2_tag->holder.buf));
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_findframe

DESCRIPTION
  Find the frame ID.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - 

SIDE EFFECTS
  ptr is updated
 
===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_id3v2_findframe (
  char   *data,
  uint8  *buf,
  uint32 len
)
{
  uint32 cnt;

  for (cnt = 0; cnt < len; cnt++) {

    if (data[cnt] != buf[cnt]) {
      return (FALSE);
    }
  }

  return (TRUE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_tag

DESCRIPTION
  Process the ID3V2 tag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_id3v2_parse_tag (
  audmpeg_parse_ctl_type *parse_ctl,
  uint8                  *tag_buf,
  uint32                 tag_buf_len,
  audmpeg_id3v2_tag_type *v2_tag
)
{
  boolean supported;
  uint16  tag_version;
  int8    flag;
  uint32  tag_size;
  uint16  frame_cnt = 0;

  supported = audmpeg_id3v2_parse_tag_header(tag_buf,
                                             &tag_version,
                                             &flag,
                                             &tag_size);

  v2_tag->version   = tag_version;
  v2_tag->hdr_flag  = flag;
  v2_tag->tag_size  = tag_size;

  v2_tag->next_file_pos = 0;

  if (!supported) {
    MSG_HIGH("audmpeg_id3v2_parse_tag - Not supported = %d", tag_size, 0, 0);
    return;
  }

  /* check whether there is an extended tag header */
  if (audmpeg_id3v2_check_ext_hdr(tag_buf, v2_tag) == FALSE) {
    return;
  }

  /* Initialize for SEEK frame */
  memcpy(v2_tag->holder.seek_frame.id,
         seek_frame_id,
         AUDMPEG_ID3V2_FRAME_ID_LEN); 

  v2_tag->holder.seek_frame.found  = FALSE;
  v2_tag->holder.seek_frame.offset = 0;
  v2_tag->holder.seek_frame.parse  = FALSE;

  v2_tag->first_seek_buf           = FALSE;

  /* Initialize for APIC frame parsing */
  memcpy(v2_tag->apic.id, apic_frame_id,
                 AUDMPEG_ID3V2_FRAME_ID_LEN); 
  v2_tag->apic.found  = FALSE;
  v2_tag->apic.file_position = 0;
  v2_tag->apic.flags  = 0;
  v2_tag->apic.size  = 0;


  v2_tag->already_parsed_tag_size  = v2_tag->ext_hdr_size;

  v2_tag->next_file_pos = AUDMPEG_ID3V2_HEADER_FOOTER_LEN +
                          v2_tag->ext_hdr_size;

  /* in case extended buffer existing, need to deduct its size */
  v2_tag->holder.buf      = tag_buf + 
                              v2_tag->next_file_pos;

  v2_tag->holder.buf_len  = tag_buf_len - 
                              v2_tag->next_file_pos;

  /* Frames currently we support for OEM
   */
  memset(&(v2_tag->frames[0]), 
         0, 
         sizeof(audmpeg_id3v2_frame_type) * (AUDMPEG_ID3V2_FRAME_MAX - 1)); 

  for (frame_cnt = 0; frame_cnt < AUDMPEG_ID3V2_FRAME_MAX; frame_cnt++) {

    memcpy(&(v2_tag->frames[frame_cnt].id[0]), 
           id3v2_frames_supported[frame_cnt],
           AUDMPEG_ID3V2_FRAME_ID_LEN);
  }
  /* Allocate Memory for the Generic frames */
  v2_tag->gframes_prealloc_index = 1;
  v2_tag->gframes_cnt = 0;
  v2_tag->gframes = malloc(sizeof(audmpeg_id3v2_generic_frame_type) * 
             (AUDMPEG_GFRAME_PRE_ALLOC_MEM * 
              v2_tag->gframes_prealloc_index));

  if (v2_tag->gframes == NULL) {
    MSG_ERROR("ID3V2, gframes malloc failed",0,0,0);
    return;
  }

  audmpeg_id3v2_process_tag_frame(parse_ctl,
                                  v2_tag);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_check_buf_2

DESCRIPTION
  If buffer 2 is not free, free it up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
void audmpeg_id3v2_check_buf_2 (
  audmpeg_parse_ctl_type *parse_ctl
)
{
  if (parse_ctl->track_2_start > 0) {
    audmain_do_play_cb(parse_ctl->audmpeg_handle, 
                       SND_CMX_AF_FREE_BUF, 
                       0, 
                       (void *)parse_ctl->track_buf_2);
   
    parse_ctl->track_2_start = 0;
  }
}

/*===========================================================================

FUNCTION audmpeg_id3v2_reset_tag

DESCRIPTION
  Reset whole tags structure or content fields only.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void audmpeg_id3v2_reset_tag ( 
  audmpeg_id3v2_tag_type *v2_tag,
  boolean                reset_all
)
{
  int cnt;

  for (cnt = 0; cnt < AUDMPEG_ID3V2_FRAME_MAX; cnt++) {
    if (v2_tag->frames[cnt].frame_content != NULL) {
      free(v2_tag->frames[cnt].frame_content); 
    }
    memset(&(v2_tag->frames[cnt]), 0, sizeof(audmpeg_id3v2_frame_type));
  }

  /* free all gereral frames */
  if (v2_tag->gframes != NULL) {
    for (cnt = 0; cnt < v2_tag->gframes_cnt; cnt++) {
      if (v2_tag->gframes[cnt].info_type == AUDMPEG_ID3V2_TAG_INFO_BY_CONTENT) {
        free(v2_tag->gframes[cnt].frame_data.frame_content);
        v2_tag->gframes[cnt].frame_data.frame_content = NULL;
      }
    }
    free(v2_tag->gframes);
    v2_tag->gframes = NULL;
    v2_tag->gframes_cnt = 0;
    v2_tag->gframes_prealloc_index =0;
  }

  /* free ext header if available*/
  if (v2_tag->ext_hdr != NULL) {
    free(v2_tag->ext_hdr);
    v2_tag->ext_hdr = NULL;
  }
    
  if (reset_all) {
    memset(v2_tag, 0, sizeof(audmpeg_id3v2_tag_type));
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_check_ext_hdr

DESCRIPTION
  Check whether the extended header exists

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_id3v2_check_ext_hdr (
  uint8                  *tag_buf,
  audmpeg_id3v2_tag_type *v2_tag
)
{
  uint8  major_version;
  uint8  *ext_hdr_ptr;

  major_version = AUDMPEG_ID3V2_TAG_VERSION_MAJOR(v2_tag->version);
  v2_tag->ext_hdr_size = 0;

  ext_hdr_ptr   = (tag_buf + AUDMPEG_ID3V2_HEADER_FOOTER_LEN);

  if ((major_version == 3) &&
      (v2_tag->hdr_flag & AUDMPEG_ID3V2_TAG_FLAG_EXTENDEDHEADER)) {

    v2_tag->ext_hdr_size =
        audmpeg_id3v2_parse_uint(&ext_hdr_ptr, 4);

  } else if ((major_version == 4) &&
             (v2_tag->hdr_flag & AUDMPEG_ID3V2_TAG_FLAG_EXTENDEDHEADER)) {

    v2_tag->ext_hdr_size = 
      audmpeg_id3v2_parse_syncsafe(&ext_hdr_ptr, 4);

  } 

  if (v2_tag->ext_hdr_size) { 

    v2_tag->ext_hdr = malloc(v2_tag->ext_hdr_size); 

    if (v2_tag->ext_hdr == NULL) {

      MSG_ERROR("ID3V2, ext_hdr malloc failed",0,0,0);
      return (FALSE);
    }

    memcpy(v2_tag->ext_hdr, ext_hdr_ptr, v2_tag->ext_hdr_size); 
  }

  return (TRUE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_check_padding

DESCRIPTION
  Check whether this is padding data

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If it is padding
  FALSE - If it is not padding

SIDE EFFECTS
  None 

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_id3v2_check_padding (
  uint8 *buf,
  audmpeg_id3v2_tag_type *v2_tag
)
{
  uint8 i;  

 for (i = 0; i < (v2_tag->tag_size - v2_tag->already_parsed_tag_size); i++, buf += 1) { 
     if (*buf != 0) {
      /* Check, if reached end of the frames in the ID3tag */
      if (i >= AUDMPEG_ID3V2_FRAME_ID_LEN) {
        MSG_HIGH("NO MORE FRAMES/FOUND PADDING",0,0,0);
        v2_tag->already_parsed_tag_size = v2_tag->tag_size;
        return (TRUE);
      }
       return (FALSE);
     }
  }
  MSG_HIGH("FOUND PADDING/REACHED THE END OF TAG",0,0,0);
  v2_tag->already_parsed_tag_size = v2_tag->tag_size;
  return (TRUE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION audmpeg_id3v2_parse_async

DESCRIPTION
  Process the leftover ID3V2 tag through ASYNC read.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean audmpeg_id3v2_parse_async (
  audmpeg_parse_ctl_type *parse_ctl,
  uint8                  *buf, 
  uint32                 buf_len,
  audmpeg_id3v2_tag_type *v2_tag
)
{
  uint16 tag_version;   
  int8   hdr_flag;
  uint32 tag_size;
  uint32 ext_hdr;

  if (v2_tag->holder.seek_frame.found &&
      v2_tag->first_seek_buf) {

    ext_hdr = 0;
    audmpeg_id3v2_parse_tag_header(buf,
                                   &tag_version,
                                   &hdr_flag,
                                   &tag_size);

    v2_tag->version  = tag_version;
    v2_tag->hdr_flag = hdr_flag;
    v2_tag->tag_size = tag_size;

    ext_hdr = audmpeg_id3v2_check_ext_hdr(buf, v2_tag);

    if (v2_tag->tag_locate == ID3V2_TAG_LOCATE_PREPEND_ONLY) {
      v2_tag->tag_locate = ID3V2_TAG_LOCATE_PREPEND_POSTPEND;

      v2_tag->postpend_size = tag_size + ext_hdr +
                               (hdr_flag & 
                                AUDMPEG_ID3V2_TAG_FLAG_FOOTERPRESENT) ?
                                (AUDMPEG_ID3V2_HEADER_FOOTER_LEN * 2) :
                                (AUDMPEG_ID3V2_HEADER_FOOTER_LEN);
    } else {

      v2_tag->postpend_size += tag_size + ext_hdr +
                                (hdr_flag & 
                                 AUDMPEG_ID3V2_TAG_FLAG_FOOTERPRESENT) ?
                                 (AUDMPEG_ID3V2_HEADER_FOOTER_LEN * 2) :
                                 (AUDMPEG_ID3V2_HEADER_FOOTER_LEN);
    }

    v2_tag->holder.seek_frame.found   = FALSE;
    v2_tag->first_seek_buf            = FALSE;
  
    v2_tag->tag_size                  = tag_size;
    v2_tag->already_parsed_tag_size   += ext_hdr;
  
    /* check whether there is an extended tag header */
    v2_tag->next_file_pos += AUDMPEG_ID3V2_HEADER_FOOTER_LEN + ext_hdr;
  
    /* in case extended buffer existing, need to deduct the size of it */
    v2_tag->holder.buf     = buf + 
                             AUDMPEG_ID3V2_HEADER_FOOTER_LEN + 
                             ext_hdr;
  
    v2_tag->holder.buf_len = buf_len - 
                             (AUDMPEG_ID3V2_HEADER_FOOTER_LEN + 
                             ext_hdr); 
  } else {

    /* in case extended buffer existing, need to deduct the size of it */
    v2_tag->holder.buf     = buf;
  
    v2_tag->holder.buf_len = buf_len;
  }

  audmpeg_id3v2_process_tag_frame(parse_ctl,
                                  v2_tag);

  if (v2_tag->next_file_pos == 0) {
    return (TRUE);
  }

  return (FALSE);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_MPEG_AUDIO */
