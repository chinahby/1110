/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Video Formats Services MP4 Reader")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               V I D E O   F O R M A T S   -   M P 4   R E A D E R

GENERAL DESCRIPTION
  This module contains functions which parse and read files in the MP4
  file format.

REFERENCES
  ISO/IEC 14496, Part 1: Systems (MPEG-4)
  ISO/IEC 14496, Part 2: Visual (MPEG-4)
  ISO/IEC 14496, Part 3: Audio (MPEG-4)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2002-2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/mmservices/mpeg4fileformat/parser/main/latest/src/videofmt_mp4r_read.c#12 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/03   rpw     Added parsing of MP3 header for sampling rate and number
                   of channels.
09/05/03   enj     Removing FEATURE_VIDEOFMT (i.e. permanently enabling it)
08/18/03   rpw     Previous FFWD fix not complete - applied complete fix and
                   expanded out and commented parts of the logic to make it
                   more readable.
08/12/03    ny     Added support for the case when it is required to forward
                   reposition a stream that only has one I-frame. Basically
                   no repositioning will be attempted in this case.
08/12/03    ny     Fixed FFWD functionality.
07/31/03   rpw     Added "sampling_frequency" parsing for AAC audio.
07/29/03   rpw     Added VIDEO_FMT_STREAM_AUDIO_EVRC_PV to distinguish
                   between EVRC streams in 3GPP2 and PacketVideo's variant
                   which uses AudioSampleEntry with a special OTI value.
07/29/03   rpw     Fixed bug where reading samples by units of frames from a
                   stream with fixed sample sizes would try to read
                   non-existant 'stsz' atom.
07/28/03    ny     Initialization of default values for some file-level fields:
                   file_level_data.audio_only_allowed = TRUE;
                   file_level_data.video_only_allowed = TRUE;
                   file_level_data.no_rand_access = FALSE;
07/11/03   rpw     Added FEATURE_AAC_ONLY_EXTRA_RESYNC_MARKERS code to insert
                   resync markers at the beginning of each AAC frame, for
                   AAC bitstreams with audio_object_type between 17 and 27,
                   inclusive.
07/11/03   rpw     Added workaround to resolve CR 32732, where files with a
                   zero "sample_count" field in the 'stsz' atom caused
                   problems.  In this case, the code now recovers the frame
                   count by summing all the "sample_count" fields in the table
                   entries of the 'stts' atom.
07/10/03   rpw     Bug fix described on 07/01/03 was not done properly.
                   Fixed the bug the right way this time.
07/07/03   rpw     Fixed bug where code which parses H.263 bitstream to
                   determine frame size was looking at first sample in 5th
                   chunk, not first sample in 1st chunk.
07/07/03   rpw     Added missing code to fail upon parsing bad H.263
                   source_format field.
07/01/03   rpw     Fixed bug where parsing of audio stream would generate a
                   failure when parsing the very end.
06/30/03   rpw     Fixed bug where audio bitstream parsing would run out of
                   syntax before running out of bitstream.
06/25/03   rpw     Created file, from original videofmt_mp4.c file.  Renamed
                   all symbols from video_fmt_mp4r_* to video_fmt_mp4r_*.
06/23/03   rpw     Replaced FEATURE_MP4_DECODER with FEATURE_VIDEOFMT.
06/18/03   rpw     Fixed bug where reading a stream and spanning more than
                   one chunk in the file would cause the wrong number of
                   bytes to be read from the movie file.
06/11/03   rpw     Added parsing of MPEG-4 AAC audio bitstream headers.
06/03/03   rpw     Added code to recognize MPEG-4 video short header format
                   as H.263 content.  Added type for MPEG-4 AAC audio.
05/28/03   rpw     Fixed bug where data overrun by client would cause an
                   abrupt and erroneous state change to idle.
05/23/03   rpw     Added timed text atom minimal support.
05/22/03   rpw     Added interface for finding nearest sync sample at or
                   after any given sample in a stream.
                   Added stream byte offset to sample information.
05/22/03   rpw     Fixed typo in 3GPP2 EVRC sample entry atom names.
03/28/03   rpw     Fixed bug in assuming length of udta child atoms.
03/18/03   rpw     Merged in changes from MPEG4_FILE_FORMAT branch.
03/17/03   rpw     Fixed stream reading bugs when read unit is set
                   to VIDEO_FMT_DATA_UNIT_FRAME.
03/10/03   rpw     Fixed bug in getting size of first frame - header was not
                   included in the length.
03/05/03   rpw     Fixed bug when calling stream read function to obtain
                   read amount without actually reading.
02/27/03   rpw     Fixed bug where sample delta count was not reset for
                   each 'stts' entry.
02/26/03   rpw     Added code to handle case where 'stss' table is not
                   present in a file.  Also fixed handling of 'stsz' table
                   when samples are fixed length.
02/25/03   rpw     Added method of getting sample information (timestamps,
                   sample sizes, sync samples, etc.)
02/24/03   rpw     Replaced reading and caching of entire 'stco', 'stsc', and
                   'stsz' chunks with partial reading and caching, and only
                   when needed to read stream data.  This effectively lifts
                   the maximum file size limit and reduces the initial file
                   parsing delay.  Also the initial file parsing delay
                   is now the same for all files, big or small.
02/18/03   rpw     Added AMR track and sample entry atom support
11/04/02   rpw     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Common definitions                      */

#include "videofmt_mp4.h"       /* MP4 format typedefs and prototypes      */
#include "videofmt_mp4r.h"      /* Internal MP4 reader definitions         */
#include "msg.h"                /* Message logging/reporting services      */
#include <string.h>             /* Memory Manipulation routines            */
#include <stdio.h>              /* Standard I/O routines                   */

#define NEED_CACHE(table)                                                  \
(                                                                          \
  (table.current_table_pos < table.cache_start) ||                         \
  (table.current_table_pos >= (table.cache_start + table.cache_size))      \
)

#define NEED_CACHEP(table_ptr)                                                       \
(                                                                                    \
  (table_ptr->current_table_pos < table_ptr->cache_start) ||                         \
  (table_ptr->current_table_pos >= (table_ptr->cache_start + table_ptr->cache_size)) \
)
//#define REPOSTN_DEBUG

/* <EJECT> */
/*===========================================================================

FUNCTION  set_cache

DESCRIPTION
  This function runs the MP4 format stream state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void set_cache (video_fmt_mp4r_stream_type*   stream,
                uint32                        start_pos,
                video_fmt_mp4r_table_type     table)
{
  uint8*  dest_cache_ptr = NULL;
  uint32  src_data_start = 0;
  uint32  data_size = 0;

  switch (table)
  {
    case VIDEO_FMT_MP4R_STSZ_TABLE:
      stream->stsz.cache_start = start_pos;
      stream->stsz.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX(stream->stsz.table_size - start_pos, 1) );
      dest_cache_ptr           = (uint8 *) &stream->stsz_cache [0];
      src_data_start           = stream->stsz.file_offset +
                                 sizeof (stream->stsz_cache [0]) * stream->stsz.cache_start;
      data_size                = sizeof (stream->stsz_cache [0]) * stream->stsz.cache_size;
      break;

    case VIDEO_FMT_MP4R_STCO_TABLE:
      stream->stco.cache_start = start_pos;
      /* if co64 atom is present then multipy the stco table size by 2
         because each offset entry will be of 64 bits.
      */
      if(stream->co64_present)
      {
        stream->stco.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX((stream->stco.table_size * 2)-start_pos, 1) );
      }
      else
      {
        stream->stco.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX(stream->stco.table_size-start_pos, 1) );
      }
      dest_cache_ptr           = (uint8 *) &stream->stco_cache [0];
      src_data_start           = stream->stco.file_offset +
                                 sizeof (stream->stco_cache [0]) * stream->stco.cache_start;
      data_size                = sizeof (stream->stco_cache [0]) * stream->stco.cache_size;
      break;

    case VIDEO_FMT_MP4R_STSS_TABLE:
      stream->stss.cache_start = start_pos;
      stream->stss.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX(stream->stss.table_size-start_pos, 1) );
      dest_cache_ptr           = (uint8 *) &stream->stss_cache [0];
      src_data_start           = stream->stss.file_offset +
                                 sizeof (stream->stss_cache [0]) * stream->stss.cache_start;
      data_size                = sizeof (stream->stss_cache [0]) * stream->stss.cache_size;
      break;

    case VIDEO_FMT_MP4R_STSC_TABLE:
      stream->stsc.cache_start = start_pos;
      stream->stsc.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX(stream->stsc.table_size-start_pos, 1) );
      dest_cache_ptr           = (uint8 *) &stream->stsc_cache [0];
      src_data_start           = stream->stsc.file_offset +
                                 sizeof (stream->stsc_cache [0]) * stream->stsc.cache_start;
      data_size                = sizeof (stream->stsc_cache [0]) * stream->stsc.cache_size;
      break;

    case VIDEO_FMT_MP4R_STSC_INFO_TABLE:
      stream->stsc_info.cache_start = start_pos;
      stream->stsc_info.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                            MAX(stream->stsc.table_size-start_pos, 1) );
      dest_cache_ptr           = (uint8 *) &stream->stsc_info_cache [0];
      src_data_start           = stream->stsc.file_offset +
                                 sizeof (stream->stsc_info_cache [0]) * stream->stsc_info.cache_start;
      data_size                = sizeof (stream->stsc_info_cache [0]) * stream->stsc_info.cache_size;
      break;

    case VIDEO_FMT_MP4R_STTS_TABLE:
      stream->stts.cache_start = start_pos;
      stream->stts.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX(stream->stts.table_size-start_pos, 1) );
      dest_cache_ptr           = (uint8 *) &stream->stts_cache [0];
      src_data_start           = stream->stts.file_offset +
                                 sizeof (stream->stts_cache [0]) * stream->stts.cache_start;
      data_size                = sizeof (stream->stts_cache [0]) * stream->stts.cache_size;
      break;

    case VIDEO_FMT_MP4R_CTTS_TABLE:
      stream->ctts.cache_start = start_pos;
      stream->ctts.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX(stream->ctts.table_size-start_pos, 1) );
      dest_cache_ptr           = (uint8 *) &stream->ctts_cache [0];
      src_data_start           = stream->ctts.file_offset +
                                 sizeof (stream->ctts_cache [0]) * stream->ctts.cache_start;
      data_size                = sizeof (stream->ctts_cache [0]) * stream->ctts.cache_size;
      break;


    case VIDEO_FMT_MP4R_TFRA_TABLE:
      stream->tfra.cache_start = start_pos;
      stream->tfra.cache_size  = MIN ( VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
                                       MAX(stream->tfra.table_size-start_pos, 1) );
      dest_cache_ptr           = (uint8 *) &stream->tfra_cache [0];
      src_data_start           = stream->tfra.file_offset +
                                 sizeof (stream->tfra_cache [0]) * stream->tfra.cache_start;
      data_size                = sizeof (stream->tfra_cache [0]) * stream->tfra.cache_size;
      break;


    default:
      MSG_ERROR ("set_cache: invalid table type", 0, 0, 0);
      break;
  }

  stream->get_data_dst = dest_cache_ptr;
  stream->get_data_src = src_data_start;
  stream->get_data_size = data_size;
  stream->get_data_needed = data_size;
  stream->get_data_read = 0;
  stream->expect_eof = FALSE;
  stream->state_next [1] = stream->state_next [0];
  stream->state_next [0] = stream->state;

}
/*===========================================================================

FUNCTION  set_cache_trun

DESCRIPTION
  This function runs the MP4 format stream state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void set_cache_trun (video_fmt_mp4r_stream_type*   stream,
                     video_fmt_mp4r_sample_table_type* trun)
{
  uint8*  dest_cache_ptr = NULL;
  uint32  src_data_start = 0;
  uint32  data_size = 0;
  uint32  current_trun = stream->current_trun;
  boolean              getData = FALSE;

  switch(trun->numOptTrunFields)
  {
    case 0:
      getData = FALSE;
      stream->fill_trun_cache = FALSE;
      /* update the cache_trun_loaded variable even though
       * no data is actually loaded so that the sample info
       * and samples are retrieved correctly
      */
      stream->trun_sample_cache_trun_loaded = current_trun;
      break;

    case 1:
      dest_cache_ptr  = (uint8 *)stream->trun_one_entry_cache;
      src_data_start  = stream->trun[current_trun].file_offset +
                        sizeof (video_fmt_mp4r_trun_one_entry_type) * stream->trun[current_trun].cache_start;
      data_size       = sizeof (video_fmt_mp4r_trun_one_entry_type) * stream->trun[current_trun].cache_size;
      stream->fill_trun_cache = TRUE;
      getData = TRUE;
      break;

    case 2:
      dest_cache_ptr  = (uint8 *)stream->trun_two_entry_cache;
      src_data_start  = stream->trun[current_trun].file_offset +
                        sizeof (video_fmt_mp4r_trun_two_entry_type) * stream->trun[current_trun].cache_start;
      data_size       = sizeof (video_fmt_mp4r_trun_two_entry_type) * stream->trun[current_trun].cache_size;
      stream->fill_trun_cache = TRUE;
      getData = TRUE;
      break;

    case 3:
      dest_cache_ptr  = (uint8 *)stream->trun_three_entry_cache;
      src_data_start  = stream->trun[current_trun].file_offset +
                        sizeof (video_fmt_mp4r_trun_three_entry_type) * stream->trun[current_trun].cache_start;
      data_size       = sizeof (video_fmt_mp4r_trun_three_entry_type) * stream->trun[current_trun].cache_size;
      stream->fill_trun_cache = TRUE;
      getData = TRUE;
      break;

    case 4:
      dest_cache_ptr  = (uint8 *)stream->trun_four_entry_cache;
      src_data_start  = stream->trun[current_trun].file_offset +
                        sizeof (video_fmt_mp4r_trun_four_entry_type) * stream->trun[current_trun].cache_start;
      data_size       = sizeof (video_fmt_mp4r_trun_four_entry_type) * stream->trun[current_trun].cache_size;
      stream->fill_trun_cache = TRUE;
      getData = TRUE;
      break;

    default:
      getData = FALSE;
      stream->fill_trun_cache = FALSE;
      MSG_ERROR ("set_trun_cache: invalid number of entries ", 0, 0, 0);
      break;
  }

  if(getData)
  {
    stream->trun_sample_cache_trun_loaded = current_trun;
    stream->get_data_dst = dest_cache_ptr;
    stream->get_data_src = src_data_start;
    stream->get_data_size = data_size;
    stream->get_data_needed = data_size;
    stream->get_data_read = 0;
    stream->expect_eof = FALSE;
    stream->state_next [1] = stream->state_next [0];
    stream->state_next [0] = stream->state;
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
  }
}

/*===========================================================================

FUNCTION  process_fill_trun_cache

DESCRIPTION
  This function runs the MP4 format stream state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_fill_trun_cache (video_fmt_mp4r_stream_type*   stream,
                              video_fmt_mp4r_sample_table_type* trun)
{
    uint32 i;

    switch (trun->numOptTrunFields)
    {
      case 1:
        for(i = 0; i < trun->cache_size; i++)
        {
          switch(trun->trun_sample_combination)
          {
            case VIDEO_FMT_MP4R_TRUN_SAMPLE_DURATION_PRESENT:
                stream->trun_cache[i].sample_duration
                    = stream->trun_one_entry_cache[i].entry_one;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_SIZE_PRESENT:
                stream->trun_cache[i].sample_size
                    = stream->trun_one_entry_cache[i].entry_one;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_FLAGS_PRESENT:
                stream->trun_cache[i].sample_flags
                    = stream->trun_one_entry_cache[i].entry_one;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_COMP_TIME_OFFSET_PRESENT:
                stream->trun_cache[i].sample_composition_time_offset
                    = stream->trun_one_entry_cache[i].entry_one;
                break;

            default:
                break;
          }
        }
        break;

      case 2:
        for(i = 0; i < trun->cache_size; i++)
        {
          switch(trun->trun_sample_combination)
          {
            case VIDEO_FMT_MP4R_TRUN_SAMPLE_DURATION_AND_SIZE_PRESENT:
                stream->trun_cache[i].sample_duration
                    = stream->trun_two_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_size
                    = stream->trun_two_entry_cache[i].entry_two;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_DURATION_AND_FLAGS_PRESENT:
                stream->trun_cache[i].sample_duration
                    = stream->trun_two_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_flags
                    = stream->trun_two_entry_cache[i].entry_two;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_DURATION_AND_COMP_TIME_OFFSET_PRESENT:
                stream->trun_cache[i].sample_duration
                    = stream->trun_two_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_composition_time_offset
                    = stream->trun_two_entry_cache[i].entry_two;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_SIZE_AND_FLAGS_PRESENT:
                stream->trun_cache[i].sample_size
                    = stream->trun_two_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_flags
                    = stream->trun_two_entry_cache[i].entry_two;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_SIZE_AND_COMP_TIME_OFFSET_PRESENT:
                stream->trun_cache[i].sample_size
                    = stream->trun_two_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_composition_time_offset
                    = stream->trun_two_entry_cache[i].entry_two;
                break;

            case  VIDEO_FMT_MP4R_TRUN_SAMPLE_FLAGS_AND_COMP_TIME_OFFSET_PRESENT:
                stream->trun_cache[i].sample_flags
                    = stream->trun_two_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_composition_time_offset
                    = stream->trun_two_entry_cache[i].entry_two;
                break;

            default:
                break;

          }
        }
        break;

      case 3:
        for(i = 0; i < trun->cache_size; i++)
        {
          switch(trun->trun_sample_combination)
          {
            case VIDEO_FMT_MP4R_TRUN_SAMPLE_DURATION_SIZE_AND_FLAGS_PRESENT:
                stream->trun_cache[i].sample_duration
                    = stream->trun_three_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_size
                    = stream->trun_three_entry_cache[i].entry_two;
                stream->trun_cache[i].sample_flags
                    = stream->trun_three_entry_cache[i].entry_three;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_DURATION_FLAGS_AND_COMP_TIME_OFFSET_PRESENT:
                stream->trun_cache[i].sample_duration
                    = stream->trun_three_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_flags
                    = stream->trun_three_entry_cache[i].entry_two;
                stream->trun_cache[i].sample_composition_time_offset
                    = stream->trun_three_entry_cache[i].entry_three;
                break;

            case VIDEO_FMT_MP4R_TRUN_SAMPLE_SIZE_FLAGS_AND_COMP_TIME_OFFSET_PRESENT:
                stream->trun_cache[i].sample_size
                    = stream->trun_three_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_flags
                    = stream->trun_three_entry_cache[i].entry_two;
                stream->trun_cache[i].sample_composition_time_offset
                    = stream->trun_three_entry_cache[i].entry_three;
                break;

            default:
                break;
          }
        }
        break;

      case 4:
        for(i = 0; i < trun->cache_size; i++)
        {
          switch(trun->trun_sample_combination)
          {
            case VIDEO_FMT_MP4R_TRUN_SAMPLE_ALL_PRESENT:
                stream->trun_cache[i].sample_duration
                    = stream->trun_four_entry_cache[i].entry_one;
                stream->trun_cache[i].sample_size
                    = stream->trun_four_entry_cache[i].entry_two;
                stream->trun_cache[i].sample_flags
                    = stream->trun_four_entry_cache[i].entry_three;
                stream->trun_cache[i].sample_composition_time_offset
                    = stream->trun_four_entry_cache[i].entry_four;
                break;

            default:
                break;
          }
        }

        default:
          break;
    }
    stream->fill_trun_cache = FALSE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION  process_get_data_state

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_get_data_state (void *server_data)
{
  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;

  /* Request more data from the user. */
  stream->cb_info.get_data.buffer       = stream->get_data_dst;
  stream->cb_info.get_data.offset       = stream->get_data_src;
  stream->cb_info.get_data.num_bytes    = stream->get_data_size;
  stream->cb_info.get_data.callback_ptr = video_fmt_mp4r_stream_process;
  stream->cb_info.get_data.server_data  = stream;


  /*Make sure that we dont read past the current write buffer offset*/
  if (stream->wBufferOffset && ((stream->get_data_src + stream->get_data_size-1)
      >= stream->wBufferOffset))
  {
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_DATA_INCOMPLETE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    return;
  }


  stream->callback_ptr (VIDEO_FMT_GET_DATA,
                        stream->client_data,
                        &stream->cb_info,
                        video_fmt_mp4r_end);

  /* Verify the user gave us a legal number of bytes.  If not,
  ** register an error message, and accept only the bytes we wanted.
  */
  if (stream->cb_info.get_data.num_bytes > stream->get_data_size)
  {
    MSG_ERROR ("stream_process: get_data overrun!", 0, 0, 0);
    stream->cb_info.get_data.num_bytes = stream->get_data_size;
  }

  /* Advance the internal variables tracking the reading. */
  stream->get_data_dst += stream->cb_info.get_data.num_bytes;
  stream->get_data_src += stream->cb_info.get_data.num_bytes;
  stream->get_data_size -= stream->cb_info.get_data.num_bytes;
  stream->get_data_read += stream->cb_info.get_data.num_bytes;
  stream->get_data_needed -= MIN (stream->get_data_needed, stream->cb_info.get_data.num_bytes);

  /* Move to the next state if no bytes were given, or no more bytes
  ** are needed.
  */
  if(!stream->cb_info.get_data.num_bytes)
  {
      //Read Error
      MSG_ERROR ("Read File Error!", 0, 0, 0);
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
      stream->callback_ptr (VIDEO_FMT_FAILURE, stream->client_data, NULL, video_fmt_mp4r_end);
      return;
  }
  if (!stream->get_data_needed)
  {
    stream->state = stream->state_next [0];
    stream->state_next [0] = stream->state_next [1];
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_stsz_count_state

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_STATE_STSZ_COUNT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_stsz_count_state (void *server_data)
{
  uint32  i;

  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;

  /* check for the need to set up the stsz table cache */
  if (( (int32)stream->count_sample < stream->stsz.cache_start) ||
      ( stream->count_sample >= (int32)stream->stsz.cache_start + stream->stsz.cache_size))
  {
    /* Set up to read table into memory. */
    if((stream->state_next [0] == VIDEO_FMT_MP4R_STREAM_STATE_READ) &&
      ((stream->read_unit == VIDEO_FMT_DATA_UNIT_BYTE) || (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME))&&
       (stream->chunk_adj_start_byte))
    {
        //rewind processing: chache in both directions.
        set_cache (stream, stream->count_sample -
                    MIN(stream->count_sample, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE>>2), VIDEO_FMT_MP4R_STSZ_TABLE);
    }
    else
    {
        //ff processing: cache only in forward direction.
        set_cache (stream, stream->count_sample, VIDEO_FMT_MP4R_STSZ_TABLE);
    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* Count as many samples as we have in cache. */
  for (i = stream->count_sample;
       i < MIN (stream->count_sample_end, (stream->stsz.cache_start + stream->stsz.cache_size));
       ++i)
  {
    *stream->count_bytes
    += N2H (stream->stsz_cache [i - stream->stsz.cache_start]);
    ++stream->count_sample;
  }

  /* If all samples are counted, return to previous state. */
  if (stream->count_sample == stream->count_sample_end)
  {
    stream->state = stream->state_next [0];
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_trun_count_state

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_STATE_TRUN_COUNT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_trun_count_state (void *server_data)
{
  uint32  i;
  video_fmt_mp4r_sample_table_type        *trun;

  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;
  trun = &stream->trun[stream->current_trun];

  /* check for the need to set up the trun sample table cache */
  if ((stream->count_sample < trun->cache_start) ||
      (stream->count_sample >= trun->cache_start + trun->cache_size) ||
      (stream->current_trun != stream->trun_sample_cache_trun_loaded))
  {

    trun->cache_start = stream->count_sample;
    trun->cache_size  = MIN (VIDEO_FMT_MP4R_TABLE_CACHE_SIZE, stream->count_sample_end - stream->count_sample);

    /* Update cache information. */

    set_cache_trun ( stream, trun);
    return;
  }

  if(stream->fill_trun_cache)
      process_fill_trun_cache(stream, trun);

  /* Count as many samples as we have in cache. */
  for (i = stream->count_sample; i < MIN (stream->count_sample_end, trun->cache_start + trun->cache_size); ++i)
  {
    if(trun->tr_flag_sample_size_present)
    {
      *stream->count_bytes += N2H (stream->trun_cache[i - trun->cache_start].sample_size);
    }
    else
    {
      *stream->count_bytes += trun->default_sample_size;
    }
    ++stream->count_sample;
  }

  /* If all samples are counted, return to previous state. */
  if (stream->count_sample == stream->count_sample_end)
  {
    stream->state = stream->state_next [0];
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_read_state

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_STATE_READ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_read_state (void *server_data, boolean *exit_loop)
{
  uint32  bytes;
  uint32  first_sample_to_read;
  uint32  current_chunk;
  boolean cache_needed;
  boolean reverse;

  video_fmt_mp4r_stco_entry_type     *stco_entry;
  video_fmt_mp4r_stsc_entry_type     *stsc_entry;
  video_fmt_mp4r_sample_table_type        *trun;


  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;
  trun = &stream->trun[stream->current_trun];

  if (!stream->read_size)
  {
    stream->cb_info.io_done.bytes = stream->read_total_bytes;
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_IO_DONE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  if (stream->fragment_processing)
  {
    if (stream->chunk_start_sample < stream->main_fragment_frames + stream->last_fragment_frames)
    {
      stream->current_trun = 0;
      trun = &stream->trun[stream->current_trun];
      stream->chunk_start_sample = stream->main_fragment_frames + stream->last_fragment_frames;
      stream->chunk_start_byte = stream->main_fragment_bytes + stream->last_fragment_bytes - stream->header;
      stream->chunk_count_bytes_valid = FALSE;
      stream->last_read_sample = stream->chunk_start_sample;
      stream->last_read_offset = stream->chunk_start_byte;
    }

    if (!stream->chunk_count_bytes_valid)
    {
      /* Get the chunk count bytes from the TRUN table
      */
      stream->chunk_count_bytes_valid = TRUE;
      stream->chunk_count_bytes = 0;
      stream->count_sample = 0;
      stream->count_sample_end = trun->table_size;
      stream->count_bytes = &stream->chunk_count_bytes;
      stream->state_next [0] = stream->state;
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_TRUN_COUNT;
      return;
    }

    /* Advance to next chunk if first sample to read is not in
    ** this chunk.
    */
    first_sample_to_read = (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
                           ? stream->chunk_start_sample + trun->table_size
                           : stream->chunk_start_byte + stream->chunk_count_bytes;
    if (stream->read_offset >= first_sample_to_read)
    {
      /* Advance sample and byte position to start of next chunk,
      ** and record chunk start sample and byte.
      */
      stream->chunk_start_sample = stream->chunk_start_sample + trun->table_size;
      stream->chunk_start_byte   = stream->chunk_start_byte + stream->chunk_count_bytes;

      /* Advance to next chunk and initialize chunk byte count if it has not already been done
      ** while getting the sample info . */
      if (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
      {
        if (stream->last_read_sample + 1 != stream->read_offset)
          ++stream->current_trun;
      }
      else
      {
        if (stream->last_read_offset != stream->read_offset)
          ++stream->current_trun;
      }
      trun = &stream->trun[stream->current_trun];
      stream->chunk_count_bytes_valid = FALSE;
      return;
    }

    /* Branch according to read unit. */
    if (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
    {
      /* Determine the number of samples to read from this chunk. */
      stream->read_amt_samples = MIN (stream->read_size,
                                      trun->table_size + stream->chunk_start_sample - stream->read_offset);

      /* If the read amount is not yet known, count the samples. */
      if (!stream->read_amt_bytes_set)
      {
        stream->read_amt_bytes_set = TRUE;
        stream->read_amt_bytes = 0;
        stream->count_sample = stream->read_offset
            - stream->chunk_start_sample;
        stream->count_sample_end = stream->read_offset
            - stream->chunk_start_sample + stream->read_amt_samples;
        stream->count_bytes = &stream->read_amt_bytes;
        stream->state_next [0] = stream->state;
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_TRUN_COUNT;
        return;
      }

      /* If reading the first sample in the chunk, start reading
      ** from the first byte in the chunk.
      */
      if (stream->read_offset == stream->chunk_start_sample)
      {
        stream->last_read_sample = stream->read_offset;
        stream->last_read_offset = stream->chunk_start_byte;
      }

      /* Otherwise, if the offset into the chunk is not known, count
      ** the samples before the first.
      */
      else if (stream->last_read_sample != stream->read_offset)
      {
        stream->last_read_sample = stream->read_offset;
        stream->last_read_offset = stream->chunk_start_byte;
        stream->count_sample = 0;
        stream->count_sample_end
            = stream->read_offset - stream->chunk_start_sample;
        stream->count_bytes = &stream->last_read_offset;
        stream->state_next [0] = stream->state;
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_TRUN_COUNT;
        return;
      }
    }
    else
    {
      /* Determine the number of bytes to read from this chunk. */
      stream->read_amt_bytes_set = TRUE;
      stream->read_amt_bytes = MIN (stream->read_size,
                                    stream->chunk_count_bytes + stream->chunk_start_byte - stream->read_offset);

      /* We don't yet know how many samples this covers, so zero out
      ** the sample amount.
      */
      stream->read_amt_samples = 0;
    }
    /* Determine now many bytes to read, and the source - either
    ** the stream header or the stream data.
    */
    if (stream->read_header_offset < stream->header)
    {
      bytes = MIN ((stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME) ? stream->header : stream->read_size,
                   stream->header - stream->read_header_offset);
      stream->get_data_src = stream->header_location + stream->read_header_offset;
    }
    else
    {
      bytes = stream->read_amt_bytes;
      stream->get_data_src = (trun->base_data_offset) +
                             (trun->data_offset) + ((stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
                                                    ? stream->last_read_offset : stream->read_offset)
                             - stream->chunk_start_byte;
    }
    if (stream->read_buffer)
    {
      /* Set other get_data parameters. */
      stream->get_data_dst = stream->read_buffer;
      stream->get_data_size = bytes;
      stream->get_data_needed = stream->get_data_size;
      stream->get_data_read = 0;
      stream->expect_eof = FALSE;

      /* Set up to return to this state, and move to the get_data
      ** state to perform the actual read from the file.
      */
      if (stream->get_data_needed > 0)
      {
        stream->state_next [0] = stream->state;
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
      }

      /* Advance read buffer pointer. */
      stream->read_buffer += bytes;
    }
    /* Advance stream reading state. */
    if (stream->read_header_offset < stream->header)
    {
      stream->read_header_offset += bytes;
      if (stream->read_unit == VIDEO_FMT_DATA_UNIT_BYTE)
      {
        stream->read_size -= bytes;
      }
    }
    else
    {
      /* Update client state variables. */
      if (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
      {
        stream->read_offset += stream->read_amt_samples;
        stream->read_size -= stream->read_amt_samples;
        stream->last_read_sample += stream->read_amt_samples;
        stream->last_read_offset += stream->read_amt_bytes;
      }
      else
      {
        stream->read_offset += stream->read_amt_bytes;
        stream->read_size -= stream->read_amt_bytes;
        stream->last_read_offset = stream->read_offset;
      }
    }

    /* Remember total bytes read, and reset chunk read size. */
    stream->read_total_bytes += bytes;
    stream->read_amt_bytes_set = FALSE;
    return;
  } /*end of if(fragment_processing)*/


  /* if stream->chunk_adj_start_byte is set and we have already counted the bytes
     in the current chunk, means we are rewinding, so update chunk start byte */
  if(stream->chunk_adj_start_byte && stream->chunk_count_bytes_valid)
  {
    stream->chunk_start_byte = stream->chunk_start_byte - stream->chunk_count_bytes;
    stream->chunk_adj_start_byte = 0;
  }

  /* If read position is before current chunk, or there are
  ** no cached chunks, reset to first chunk in file.
  */
  current_chunk = (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
                  ? stream->chunk_start_sample
                  : stream->chunk_start_byte;

  if (!stream->stco.cache_size)
  {
    stream->stco.current_table_pos = 0;
    stream->stsc.current_table_pos = 0;
    stream->chunk_start_sample = 0;
    stream->chunk_start_byte = 0;
    stream->chunk_count_bytes_valid = FALSE;
    stream->last_read_sample = 0;
    stream->last_read_offset = 0;
    stream->chunk_adj_start_byte = FALSE;
    reverse = FALSE;
  }
  else
  {
    reverse = stream->read_offset < current_chunk;
  }

  /* if chunk_adj_start_byte is set means we need to count chunk bytes for rewind,
     then no need to update the cache, just go and count the bytes. */
  if(!stream->chunk_adj_start_byte)
  {
    if (reverse)
    {
      /* update STCO table cache, if needed */
      cache_needed = FALSE;
      /* Check if current position is at or before first sample in
      ** the cached region.
      */
      if (stream->stco.current_table_pos <= stream->stco.cache_start)
      {
        /* Exception: if current position is on first sample,
        ** do not recache.
        */
        if (stream->stco.current_table_pos != 0)
        {
          cache_needed = TRUE;
        }
      }
      /* Check if current position is beyond one past the last
      ** sample in the cached region. */
      else if ( stream->stco.current_table_pos
                > stream->stco.cache_start + (int32)stream->stco.cache_size )
      {
        cache_needed = TRUE;
      }
      if(cache_needed)
      {
        /* Set up to read table into memory. */
        set_cache ( stream,
                    stream->stco.current_table_pos -
                    MIN(stream->stco.current_table_pos, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE),
                    VIDEO_FMT_MP4R_STCO_TABLE);
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
        return;
      }

      /* update STSC table cache, if needed */
      cache_needed = FALSE;
      /* Check if current position is at or before first sample in
      ** the cached region.
      */
      if (stream->stsc.current_table_pos <= stream->stsc.cache_start)
      {
        /* Exception: if current position is on first sample,
        ** do not recache.
        */
        if (stream->stsc.current_table_pos != 0)
        {
          cache_needed = TRUE;
        }
      }
      /* Check if current position is beyond one past the last
      ** sample in the cached region. */
      else if ( stream->stsc.current_table_pos
                > stream->stsc.cache_start + (int32)stream->stsc.cache_size )
      {
        cache_needed = TRUE;
      }
      if (cache_needed)
      {
        /* Set up to read table into memory. */
        set_cache ( stream,
                    (stream->stsc.current_table_pos + 1
                     - MIN (stream->stsc.current_table_pos + 1, (int32)stream->stsc.cache_size)),
                     VIDEO_FMT_MP4R_STSC_TABLE);
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
        return;
      }
    }
    else
    {
      /* Cache 'stco' table entries. */
      if NEED_CACHE(stream->stco) /*lint!e574 */
      {
        /* Set up to read table into memory. */
        set_cache (stream, stream->stco.current_table_pos, VIDEO_FMT_MP4R_STCO_TABLE);
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
        return;
      }

      /* Cache 'stsc' table entries. */
      if (NEED_CACHE(stream->stsc) ||
          ((stream->stsc.current_table_pos + 1 >= stream->stsc.cache_start + (int32)stream->stsc.cache_size) &&
          (stream->stsc.cache_start + (int32)stream->stsc.cache_size < (int32)stream->stsc.table_size)))
      {
        /* Set up to read table into memory. */
        set_cache (stream, stream->stsc.current_table_pos, VIDEO_FMT_MP4R_STSC_TABLE);
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
        return;
      }
    }
  }
  /* if co64 atom is present then advance the stco table by 2
       because each offset entry will be of 64 bits.
  */
  if(stream->co64_present)
  {
    /* Count the bytes in this chunk, if not yet counted. */
    stco_entry = (stream->stco_cache + stream->stco.current_table_pos + 1) - stream->stco.cache_start;
  }
  else
  {
  /* Count the bytes in this chunk, if not yet counted. */
  stco_entry = stream->stco_cache + stream->stco.current_table_pos - stream->stco.cache_start;
  }
  stsc_entry = stream->stsc_cache + stream->stsc.current_table_pos - stream->stsc.cache_start;
  if (!stream->chunk_count_bytes_valid)
  {
    /* If the samples are all the same size, the number of bytes
    ** is simply the number of samples multipled by the sample
    ** size.
    */
    stream->chunk_count_bytes_valid = TRUE;
    if (stream->sample_size)
    {
      stream->chunk_count_bytes = N2H (stsc_entry->samples_per_chunk) * stream->sample_size;
    }

    /* Otherwise, the 'stsz' table must be consulted to count the
    ** bytes in the samples.
    */
    else
    {
      stream->chunk_count_bytes = 0;
      stream->count_sample = stream->chunk_start_sample;
      stream->count_sample_end = stream->chunk_start_sample + N2H (stsc_entry->samples_per_chunk);
      stream->count_bytes = &stream->chunk_count_bytes;
      stream->state_next [0] = stream->state;
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_STSZ_COUNT;
    }
    return;
  }

  /* Advance to next chunk if first sample to read is not in
  ** this chunk.
  */
  if (stream->read_offset >= ((stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
                              ? stream->chunk_start_sample + N2H (stsc_entry->samples_per_chunk)
                              : stream->chunk_start_byte + stream->chunk_count_bytes))
  {
    /* Advance sample and byte position to start of next chunk,
    ** and record chunk start sample and byte.
    */
    stream->chunk_start_sample = stream->chunk_start_sample + N2H (stsc_entry->samples_per_chunk);
    stream->chunk_start_byte   = stream->chunk_start_byte + stream->chunk_count_bytes;

    /* if co64 atom is present then advance the stco table by 2
       because each offset entry will be of 64 bits.
    */
    if(stream->co64_present)
    {
      /* Advance to next chunk and recalculate chunk byte count. */
      stream->stco.current_table_pos = stream->stco.current_table_pos + 2;
    }
    else
    {
    /* Advance to next chunk and recalculate chunk byte count. */
    ++stream->stco.current_table_pos;
    }
    stream->chunk_count_bytes_valid = FALSE;

    /* Advance to next 'stsc' entry if it starts with the next
    ** chunk.
    **
    ** GRB 1/6/04: KDDI Annex I allows for chunks in stsc
    ** to start counting at 0...we should offset our math
    ** by the index of the first chunk in the first stsc entry.
    */
    if (!stream->first_entry_offset_set)
    {
      stream->first_entry_offset = N2H(stream->stsc_cache[0].first_chunk);
      stream->first_entry_offset_set = TRUE;
    }

    /* If there is a duplicate entry in the STSC table ignore the second entry and increment the
    ** pointers accordingly. Obviously this will not work if the entry is repeated more than
    ** twice or the entries are out of order.
    */
    if((stream->stsc.current_table_pos + 1 < stream->stsc.cache_start + (int32)stream->stsc.cache_size) &&
       (N2H((stsc_entry+1)->first_chunk) <= N2H((stsc_entry)->first_chunk)))
  {
        MSG_ERROR ("process_read_state: Duplicate entry found in STSC at position %d for track ID %d",
                   stream->stsc.current_table_pos , N2H((stsc_entry)->first_chunk), 0);
    ++stream->stsc.current_table_pos;
    ++stsc_entry;
  }

    /* In case of co64 atom every two entries will constitue one entry. So we are divinding by 2*/
    if ((stream->stsc.current_table_pos + 1 < stream->stsc.cache_start + stream->stsc.cache_size) &&
    ((stream->co64_present == 0 && 
      N2H ((stsc_entry+1)->first_chunk) == stream->stco.current_table_pos  + stream->first_entry_offset) ||
     (stream->co64_present == 1 && 
      N2H ((stsc_entry+1)->first_chunk) == stream->stco.current_table_pos / 2  + stream->first_entry_offset)))
    {
      ++stream->stsc.current_table_pos;
    }

    return;
  }
  else if (stream->read_offset < ((stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME) ?
                                  stream->chunk_start_sample : stream->chunk_start_byte) )

  {
    /* back sample and byte position to end of prev chunk,
    ** and record chunk start sample and byte.
    */
    /* if co64 atom is present then decrement the stco table by 2
       because each offset entry will be of 64 bits.
    */
    if(stream->co64_present)
    {
      /* Advance to next chunk and initialize chunk byte count. */
      stream->stco.current_table_pos = stream->stco.current_table_pos - 2;
    }
    else
    {
    /* Advance to next chunk and initialize chunk byte count. */
    --stream->stco.current_table_pos;
    }

    /* back to prev 'stsc' entry if it starts with the prev
    ** chunk.
    **
    ** GRB 1/6/04: KDDI Annex I allows for chunks in stsc
    ** to start counting at 0...we should offset our math
    ** by the index of the first chunk in the first stsc entry.
    */
    if (!stream->first_entry_offset_set)
    {
      stream->first_entry_offset = N2H(stream->stsc_cache[0].first_chunk);
      stream->first_entry_offset_set = TRUE;
    }

    if( (stream->stsc.current_table_pos <= (stream->stsc.cache_start+stream->stsc.cache_size))
        && (stream->stsc.current_table_pos > stream->stsc.cache_start)
        && (N2H((stsc_entry-1)->first_chunk) >= N2H((stsc_entry)->first_chunk)))
    {
        MSG_ERROR ("process_read_state: Duplicate entry found in STSC at position %d for track ID %d",
                   stream->stsc.current_table_pos , N2H((stsc_entry)->first_chunk), 0);
      --stream->stsc.current_table_pos;
      --stsc_entry;
    }
    /* Back up to previous 'stsc' table entry if we've reached the first chunk of that entry. */
    if( (stream->stsc.current_table_pos <= (stream->stsc.cache_start+(int32)stream->stsc.cache_size))
        && (stream->stsc.current_table_pos > stream->stsc.cache_start)
        && ((stream->co64_present == 0 && 
             (stream->stco.current_table_pos + stream->first_entry_offset) < N2H ((stsc_entry)->first_chunk)) ||
            (stream->co64_present == 1 && 
             (stream->stco.current_table_pos / 2 + stream->first_entry_offset) < N2H ((stsc_entry)->first_chunk))))
    {
      --stream->stsc.current_table_pos;
      stsc_entry--;
    }

    stream->chunk_start_sample = stream->chunk_start_sample - N2H ((stsc_entry)->samples_per_chunk);
    /* here we set the flag. Next loop will count the bytes in this chunk.
       Then using this boolean flag and substracting counted byte in this chunk,
       we will get the actual chunk start byte of just rewinded chunk. */
    stream->chunk_adj_start_byte = TRUE;
    stream->chunk_count_bytes_valid = FALSE;

    return;
  }

  /* Branch according to read unit. */
  if (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
  {
    /* Determine the number of samples to read from this chunk. */
    stream->read_amt_samples = MIN (stream->read_size,
                                    N2H (stsc_entry->samples_per_chunk) + stream->chunk_start_sample - stream->read_offset);

    /* If the read amount is not yet known, count the samples. */
    if (!stream->read_amt_bytes_set)
    {
      stream->read_amt_bytes_set = TRUE;
      if (stream->sample_size)
      {
        stream->read_amt_bytes = stream->sample_size * stream->read_amt_samples;
      }
      else
      {
        stream->read_amt_bytes = 0;
        stream->count_sample = stream->read_offset;
        stream->count_sample_end = stream->read_offset + stream->read_amt_samples;
        stream->count_bytes = &stream->read_amt_bytes;
        stream->state_next [0] = stream->state;
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_STSZ_COUNT;
        return;
      }
    }

    /* If reading the first sample in the chunk, start reading
    ** from the first byte in the chunk.
    */
    if (stream->read_offset == stream->chunk_start_sample)
    {
      stream->last_read_sample = stream->read_offset;
      stream->last_read_offset = 0;
    }

    /* Otherwise, if the offset into the chunk is not known, count
    ** the samples before the first.
    */
    else if (stream->last_read_sample != stream->read_offset)
    {
      if (stream->sample_size)
      {
          stream->last_read_offset = stream->sample_size
              * (stream->read_offset - stream->chunk_start_sample);
          stream->last_read_sample = stream->read_offset;
      }
      else
      {
        stream->last_read_sample = stream->read_offset;
        stream->last_read_offset = 0;
        stream->count_sample = stream->chunk_start_sample;
        stream->count_sample_end = stream->read_offset;
        stream->count_bytes = &stream->last_read_offset;
        stream->state_next [0] = stream->state;
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_STSZ_COUNT;
        return;
      }
    }
  }
  else
  {
      /* Determine the number of bytes to read from this chunk. */
      stream->read_amt_bytes = MIN (stream->read_size,
                                    stream->chunk_count_bytes +
                                    stream->chunk_start_byte -
                                    stream->read_offset);

    /* We don't yet know how many samples this covers, so zero out
    ** the sample amount.
    */
    stream->read_amt_samples = 0;
  }

  /* Determine now many bytes to read, and the source - either
  ** the stream header or the stream data.
  */
  if (stream->read_header_offset < stream->header)
  {
    bytes = MIN ((stream->read_unit
                  == VIDEO_FMT_DATA_UNIT_FRAME)
                 ? stream->header : stream->read_size,
                 stream->header
                 - stream->read_header_offset);
    stream->get_data_src = stream->header_location
                           + stream->read_header_offset;
  }
  else
  {
    bytes = stream->read_amt_bytes;
    stream->get_data_src = N2H (stco_entry->chunk_offset)
                           + ((stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
                              ? stream->last_read_offset
                              : stream->read_offset - stream->chunk_start_byte);
  }

  /*Make sure that we dont read past the current write buffer offset*/
  if (stream->wBufferOffset && ((stream->get_data_src + bytes-1) >= stream->wBufferOffset))
  {
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_DATA_INCOMPLETE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  /* If a valid buffer pointer was given, set up to read from the
  ** stream directly to the client's buffer.
  */
  if (stream->read_buffer)
  {
    /* Set other get_data parameters. */
    stream->get_data_dst = stream->read_buffer;
    stream->get_data_size = bytes;
    stream->get_data_needed = stream->get_data_size;
    stream->get_data_read = 0;
    stream->expect_eof = FALSE;

    /* Set up to return to this state, and move to the get_data
    ** state to perform the actual read from the file.
    */
    if (stream->get_data_needed > 0)
    {
      stream->state_next [0] = stream->state;
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    }

    /* Advance read buffer pointer. */
    stream->read_buffer += bytes;
  }

  /* Advance stream reading state. */
  if (stream->read_header_offset < stream->header)
  {
    stream->read_header_offset += bytes;
    if (stream->read_unit == VIDEO_FMT_DATA_UNIT_BYTE)
    {
      stream->read_size -= bytes;
    }
  }
  else
  {
    /* Update client state variables. */
    if (stream->read_unit == VIDEO_FMT_DATA_UNIT_FRAME)
    {
      stream->read_offset += stream->read_amt_samples;
      stream->read_size -= stream->read_amt_samples;
      stream->last_read_sample += stream->read_amt_samples;
      stream->last_read_offset += stream->read_amt_bytes;
    }
    else
    {
      stream->read_offset += stream->read_amt_bytes;
      stream->read_size -= stream->read_amt_bytes;
    }
  }

  /* Remember total bytes read, and reset chunk read size. */
  stream->read_total_bytes += bytes;
  stream->read_amt_bytes_set = FALSE;

}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_find_abs_file_Offset

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_STATE_FIND_ABS_OFFSET.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_find_abs_file_Offset (void *server_data, boolean *exit_loop)
{
  uint32  bytes;
  /* uint32  current_chunk; */

  video_fmt_mp4r_stco_entry_type     *stco_entry;
  video_fmt_mp4r_stsc_entry_type     *stsc_entry;

  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;

  /* If read position is before current chunk, or there are
  ** no cached chunks, reset to first chunk in file.
  */
  /* current_chunk = stream->chunk_start_byte; */
  if (!stream->stco.cache_size)
  {
    stream->stco.current_table_pos = 0;
    stream->stsc.current_table_pos = 0;
    stream->chunk_start_sample = 0;
    stream->chunk_start_byte = 0;
    stream->chunk_count_bytes_valid = FALSE;
    stream->last_read_sample = 0;
    stream->last_read_offset = 0;
  }

  /* Cache 'stco' table entries. */
  if NEED_CACHE(stream->stco)/*lint!e574 */
  {
    /* Set up to read table into memory. */
    set_cache (stream, stream->stco.current_table_pos, VIDEO_FMT_MP4R_STCO_TABLE);
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* Cache 'stsc' table entries. */
  if (NEED_CACHE(stream->stsc) || /*lint!e574 */
      ((stream->stsc.current_table_pos + 1 >= stream->stsc.cache_start + (int32)stream->stsc.cache_size) &&
      (stream->stsc.cache_start + (int32)stream->stsc.cache_size < stream->stsc.table_size)))
  {
    /* Set up to read table into memory. */
    set_cache (stream, stream->stsc.current_table_pos, VIDEO_FMT_MP4R_STSC_TABLE);
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* if co64 atom is present then advance the stco table by 2 
       because each offset entry will be of 64 bits. 
  */
  if(stream->co64_present)
  {
    /* Count the bytes in this chunk, if not yet counted. */
    stco_entry = (stream->stco_cache + stream->stco.current_table_pos + 1) - stream->stco.cache_start;
  }
  else
  {
  /* Count the bytes in this chunk, if not yet counted. */
  stco_entry = stream->stco_cache + stream->stco.current_table_pos - stream->stco.cache_start;
  }
  stsc_entry = stream->stsc_cache + stream->stsc.current_table_pos - stream->stsc.cache_start;
  if (!stream->chunk_count_bytes_valid)
  {
    /* If the samples are all the same size, the number of bytes
    ** is simply the number of samples multipled by the sample
    ** size.
    */
    stream->chunk_count_bytes_valid = TRUE;
    if (stream->sample_size)
    {
      stream->chunk_count_bytes = N2H (stsc_entry->samples_per_chunk) * stream->sample_size;
    }

    /* Otherwise, the 'stsz' table must be consulted to count the
    ** bytes in the samples.
    */
    else
    {
      stream->chunk_count_bytes = 0;
      stream->count_sample = stream->chunk_start_sample;
      stream->count_sample_end = stream->chunk_start_sample + N2H (stsc_entry->samples_per_chunk);
      stream->count_bytes = &stream->chunk_count_bytes;
      stream->state_next [0] = stream->state;
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_STSZ_COUNT;
      return;
    }
  }

  /* Advance to next chunk if first sample to read is not in
  ** this chunk.
  */
  if (stream->read_offset >= (stream->chunk_start_byte + stream->chunk_count_bytes))
  {
    /* Advance sample and byte position to start of next chunk,
    ** and record chunk start sample and byte.
    */
    stream->chunk_start_sample = stream->chunk_start_sample + N2H (stsc_entry->samples_per_chunk);
    stream->chunk_start_byte   = stream->chunk_start_byte + stream->chunk_count_bytes;
    /* if co64 atom is present then advance the stco table by 2
       because each offset entry will be of 64 bits.
    */
    if(stream->co64_present)
    {
      /* Advance to next chunk and recalculate chunk byte count. */
      stream->stco.current_table_pos = stream->stco.current_table_pos + 2;
    }
    else
    {
    /* Advance to next chunk and recalculate chunk byte count. */
    ++stream->stco.current_table_pos;
    }
    stream->chunk_count_bytes_valid = FALSE;

    /* Advance to next 'stsc' entry if it starts with the next
    ** chunk.
    **
    ** GRB 1/6/04: KDDI Annex I allows for chunks in stsc
    ** to start counting at 0...we should offset our math
    ** by the index of the first chunk in the first stsc entry.
    */
    if (!stream->first_entry_offset_set)
    {
      stream->first_entry_offset = N2H(stream->stsc_cache[0].first_chunk);
      stream->first_entry_offset_set = TRUE;
    }

    /* In case of co64 atom every two entries will constitue one entry. So we are divinding by 2*/
    if ((stream->stsc.current_table_pos + 1 < stream->stsc.cache_start + (int32)stream->stsc.cache_size) &&
        ((stream->co64_present == 0 && 
          N2H ((stsc_entry+1)->first_chunk) == stream->stco.current_table_pos  + stream->first_entry_offset) ||
         (stream->co64_present == 1 && 
          N2H ((stsc_entry+1)->first_chunk) == stream->stco.current_table_pos / 2  + stream->first_entry_offset)))
    {
      ++stream->stsc.current_table_pos;
    }

    return;
  }


  /* Determine the number of bytes to read from this chunk. */
  bytes = MIN (stream->read_size,
               stream->chunk_count_bytes +
               stream->chunk_start_byte -
               stream->read_offset);

  stream->get_data_src = N2H (stco_entry->chunk_offset)
                         + (stream->read_offset - stream->chunk_start_byte);

  /*Return the abs_file_offset to the client*/
  stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
  stream->cb_info.info.abs_file_offset = stream->get_data_src + bytes-1;
  stream->callback_ptr (VIDEO_FMT_ABS_FILE_OFFSET,
                        stream->client_data,
                        &stream->cb_info,
                        video_fmt_mp4r_end);
  *exit_loop = TRUE;
  return;
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_get_sample_info_state

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_STATE_GET_SAMPLE_INFO.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_get_sample_info_state (void *server_data, boolean *exit_loop)
{
  boolean cache_needed;
  boolean reverse;

  video_fmt_mp4r_stsz_entry_type     *stsz_entry;
  video_fmt_mp4r_stss_entry_type     *stss_entry;
  video_fmt_mp4r_stts_entry_type     *stts_entry;
  video_fmt_mp4r_ctts_entry_type     *ctts_entry;
  video_fmt_mp4r_stsc_entry_type     *stsc_info_entry;

  uint32                                  offset;
  video_fmt_mp4r_sample_table_type        *trun;
  video_fmt_mp4r_trun_entry_type          *trun_entry = 0;


  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;
  trun = &stream->trun[stream->current_trun];

  if (!stream->get_sample_info_size)
  {
    stream->cb_info.io_done.bytes
    = stream->get_sample_info_total_samples
      * sizeof (video_fmt_sample_info_type);
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_IO_DONE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  /* the sample is in the fragment */
  if (stream->fragment_processing)
  {
    uint32 i;

    if(stream->fragment_repositioned)
    {
        trun->cache_start = 0;
        trun->current_table_pos = 0;

      /* Update cache information. */
      trun->cache_size
      = MIN (VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
             trun->table_size);

      set_cache_trun (stream, trun);

      stream->fragment_repositioned = FALSE;
      return;
    }


    offset = stream->main_fragment_frames + stream->last_fragment_frames;

    for (i = 0; i < stream->current_trun; i++)
    {
      offset += stream->trun[i].table_size;
    }

    /* Reset sample position if the next sample to collect information
    ** is before the current sample.
    */
    if (stream->get_sample_info_offset < offset + trun->current_table_pos)
    {
      stream->current_trun = 0;
      trun = &stream->trun[stream->current_trun];
      trun->current_table_pos = 0;
      stream->chunk_start_sample = 0;
      offset = stream->main_fragment_frames + stream->last_fragment_frames;
      stream->sample_byte_offset = stream->main_fragment_bytes + stream->last_fragment_bytes;
      stream->sample_timestamp = stream->main_fragment_timestamp + stream->last_fragment_timestamp;
      //stream->get_sample_info_offset = offset;
      stream->get_sample_info_buffer->offset = stream->sample_byte_offset;
      stream->get_sample_info_buffer->time = stream->sample_timestamp;
      stream->get_sample_info_buffer->decode_time = stream->get_sample_info_buffer->time;
    }

    /* return if the sample is in the main fragment */
    if (stream->get_sample_info_offset < stream->main_fragment_frames)
    {
      return;
    }

    /*advance to next trun if done with the current trun */
    if ((trun->current_table_pos)
        && (trun->current_table_pos
            >= trun->cache_start
            + trun->cache_size)
        &&  (trun->table_size
             <= trun->current_table_pos))
    {
      ++stream->current_trun;
      trun = &stream->trun[stream->current_trun];
      trun->current_table_pos = 0;
      trun->cache_start = 0;
      trun->cache_size = 0;
    }

    if (stream->trun_entry_count
        && (NEED_CACHEP(trun)
            || (stream->current_trun != stream->trun_sample_cache_trun_loaded)))
    {
      trun->cache_start = trun->current_table_pos;
      trun->cache_size
      = MIN (VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
             trun->table_size
             - trun->current_table_pos);


      set_cache_trun (stream, trun);
      return;
    }

    if(stream->fill_trun_cache)
      process_fill_trun_cache(stream, trun);

    trun_entry = stream->trun_cache
                    + trun->current_table_pos
                    - trun->cache_start;

    if (stream->get_sample_info_offset == offset + trun->current_table_pos)
    {
      /* If the buffer pointer is not NULL, collect and store
      ** information about the sample.
      */
      if (stream->get_sample_info_buffer)
      {
        stream->get_sample_info_buffer->sample
        = offset + trun->current_table_pos;
        if (!stream->sample_timestamp)
        {
          if(stream->sample_duration)
          {
             stream->get_sample_info_buffer->time = stream->sample_duration *
                  (stream->last_fragment_frames + stream->main_fragment_frames);
             stream->get_sample_info_buffer->decode_time = stream->get_sample_info_buffer->time;
          }
           else
          {
             stream->get_sample_info_buffer->time +=
                stream->get_sample_info_buffer->time;
            stream->get_sample_info_buffer->decode_time = stream->get_sample_info_buffer->time;
          }

          stream->sample_timestamp = stream->get_sample_info_buffer->time;
        }
        else
        {
          stream->get_sample_info_buffer->time = stream->sample_timestamp;
          stream->get_sample_info_buffer->decode_time = stream->get_sample_info_buffer->time;
        }

        if (!stream->sample_byte_offset)
        {
          if(stream->sample_size)
             stream->get_sample_info_buffer->offset = stream->sample_size *
                  (stream->last_fragment_frames + stream->main_fragment_frames);
          else
            stream->get_sample_info_buffer->offset +=
                    stream->get_sample_info_buffer->size;

          stream->sample_byte_offset = stream->get_sample_info_buffer->offset;
        }
        else
          stream->get_sample_info_buffer->offset
          = stream->sample_byte_offset;

        /*if sample_duration is present in the trun get the value of the duration*/
        if(trun->tr_flag_sample_duration_present)
        {
          stream->get_sample_info_buffer->delta
                    = N2H (trun_entry->sample_duration);
        }
        else
        {
          /*
          * We store sample duration to be used for 'TRUN' in 'TRUN' itself.
          * This is becasue since there can be multiple TRAF in a given fragment, there can be
          * multiple TFHD.
          * So, while parsing 'TRUN', if it does not have sample duration information,
          * we update value from corresponding TFHD.
          *
          */

          stream->get_sample_info_buffer->delta = trun->default_sample_duration;
        }

        /*if sample_size is present in the trun get the size*/
        if(trun->tr_flag_sample_size_present)
        {
          stream->get_sample_info_buffer->size
                    = N2H (trun_entry->sample_size);
        }
        else
        {
          /*
          * We store sample size to be used for 'TRUN' in 'TRUN' itself.
          * This is becasue since there can be multiple TRAF in a given fragment, there can be
          * multiple TFHD.
          *
          * So, while parsing 'TRUN', if it does not have sample size information,
          * we update value from corresponding TFHD.
          *
          */

          stream->get_sample_info_buffer->size = trun->default_sample_size;
        }

    stream->get_sample_info_buffer->sync = 0;
        /* set Sync Sample flag if first_sample_flag is present. */
        if (trun->tr_flag_first_sample_flag_present)
        {
          if ((trun->first_sample_flags == FLAG_I_VOP) && (trun->current_table_pos == 0))
            stream->get_sample_info_buffer->sync = 1;
        }
        /* set Sync Sample flag if flag_sample_flags is present. */
        else if(trun->tr_flag_sample_flags_present)
        {
           if (N2H (trun_entry->sample_flags) == FLAG_I_VOP)
             stream->get_sample_info_buffer->sync = 1;
        }
    /* set Sync Sample flag to default value (trex atom) if first_sample_flag and
            flag_sample_flag are not present. */
        else
    {
      if (stream->default_sample_flags == FLAG_I_VOP)
          stream->get_sample_info_buffer->sync = 1;
    }

        stream->get_sample_info_buffer->sample_desc_index =
            trun->sample_description_index;

        ++stream->get_sample_info_buffer;
      }

      /* Count sample covered. */
      stream->last_sample_offset = stream->get_sample_info_offset;
      ++stream->get_sample_info_offset;
      ++stream->get_sample_info_total_samples;
      --stream->get_sample_info_size;
    }

    ++trun->current_table_pos;
    ++stream->sample_delta_count;

    /*if sample_duration is present in the trun add value of duration to sample_timestamp*/
    if(trun->tr_flag_sample_duration_present)
    {
      stream->sample_timestamp += N2H (trun_entry->sample_duration);
    }
    else
    {
       /*
       * We store sample duration to be used for 'TRUN' in 'TRUN' itself.
       * This is becasue since there can be multiple TRAF in a given fragment, there can be
       * multiple TFHD.
       *
       * So, while parsing 'TRUN', if it does not have sample duration information,
       * we update value from corresponding TFHD.
       *
       */

       stream->sample_timestamp += trun->default_sample_duration;
    }

    /*if sample_size is present in the trun get the value of the duration*/
    if(trun->tr_flag_sample_size_present)
    {
      stream->sample_byte_offset += N2H (trun_entry->sample_size);
    }
    else
    {
      stream->sample_byte_offset += trun->default_sample_size;
    }
    return;
  }
  /* Reset sample position if the next sample to collect information
  ** is before the current sample.
  */
  /* Reset sample position if the next sample to collect information
  ** is before the current sample.
  */
  if (stream->stts.cache_size == 0)
  {
    stream->stsz.current_table_pos = 0;
    stream->stts.current_table_pos = 0;
    stream->ctts.current_table_pos = 0;
    stream->stss.current_table_pos = 0;
    stream->sample_timestamp = 0; /* TODO: start>0 allowed? */
    stream->sample_byte_offset = 0;
    stream->sample_delta_count = 0;
    stream->sample_ctts_offset_count = 0;
    stream->stsc_info.current_table_pos = 0;
    reverse = FALSE;
  }
  else
  {
    reverse = ( stream->stsz.current_table_pos > (int32)stream->get_sample_info_offset);
  }

  /* Cache 'stsz' table entries if samples are variable-length and
  ** the current sample is not in the 'stsz' cache.
  */
  cache_needed = !stream->sample_size;
  if (cache_needed && reverse)
  {
    cache_needed = FALSE;

    /* Check if current position is at or before first sample in
    ** the cached region.
    */
    if (stream->stsz.current_table_pos <= stream->stsz.cache_start)
    {
      /* Exception: if current position is on first sample,
      ** do not recache.
      */
      if (stream->stsz.current_table_pos != 0)
      {
        cache_needed = TRUE;
      }
    }

    /* Check if current position is beyond one past the last
    ** sample in the cached region. */
    else if (stream->stsz.current_table_pos
             > stream->stsz.cache_start
             + (int32)stream->stsz.cache_size)
    {
      cache_needed = TRUE;
    }
  }
  else if (cache_needed && !reverse)
  {
    cache_needed = FALSE;

    /* Check if current position is before first sample in
    ** cached region.
    */
    if (stream->stsz.current_table_pos < stream->stsz.cache_start)
    {
      cache_needed = TRUE;
    }

    /* Check if current position is at or beyond one past the last
    ** sample in the cached region.
    */
    else if (stream->stsz.current_table_pos
             >= stream->stsz.cache_start
             + (int32)stream->stsz.cache_size)
    {
      cache_needed = TRUE;
    }
  }
  if (cache_needed)
  {
    /* Update cache information. */
    if (reverse)
    {
      set_cache (stream,
                 (stream->stsz.current_table_pos + 1
                  - MIN (stream->stsz.current_table_pos + 1, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                 VIDEO_FMT_MP4R_STSZ_TABLE);
    }
    else
    {
      set_cache (stream, stream->stsz.current_table_pos, VIDEO_FMT_MP4R_STSZ_TABLE);
    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* Cache 'stts' table entries. */
  cache_needed = FALSE;
  if (reverse)
  {
    /* Check if current position is at or before first sample in
    ** the cached region.
    */
    if (stream->stts.current_table_pos <= stream->stts.cache_start)
    {
      /* Exception: if current position is on first sample,
      ** do not recache.
      */
      if (stream->stts.current_table_pos != 0)
      {
        cache_needed = TRUE;
      }
    }

    /* Check if current position is beyond one past the last
    ** sample in the cached region. */
    else if (stream->stts.current_table_pos
             > stream->stts.cache_start
             +(int32) stream->stts.cache_size)
    {
      cache_needed = TRUE;
    }
  }
  else
  {
    /* Check if current position is before first sample in
    ** cached region.
    */
    if (stream->stts.current_table_pos < stream->stts.cache_start)
    {
      cache_needed = TRUE;
    }

    /* Check if current position is at or beyond one past the last
    ** sample in the cached region.
    */
    else if (stream->stts.current_table_pos
             >= stream->stts.cache_start
             +(int32) stream->stts.cache_size)
    {
      cache_needed = TRUE;
      if(stream->stts.current_table_pos >= (int32)stream->stts.table_size)
          cache_needed = FALSE;
    }
  }
  if (cache_needed)
  {
    /* Update cache information. */
    if (reverse)
    {
      set_cache (stream,
                 (stream->stts.current_table_pos + 1
                  - MIN (stream->stts.current_table_pos + 1, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                 VIDEO_FMT_MP4R_STTS_TABLE);

    }
    else
    {
      set_cache (stream, stream->stts.current_table_pos, VIDEO_FMT_MP4R_STTS_TABLE);

    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }
  else
  {
    uint32 stts_table_pos = stream->stts.current_table_pos;
    /* some clips have more samples than entries in STTS table (SKT-MOD clips). So we make sure that
        we don't go beyond entries in STTS table and keep using last entry values for all extra samples */
    if(stts_table_pos >= stream->stts.table_size)
      stts_table_pos = stream->stts.table_size-1;

    /* some SKTT clips have zero sample count in some STTS entries, so we need to add
       their time delta and goto next entry and update cache if needed */
    stts_entry = stream->stts_cache + stts_table_pos
                - stream->stts.cache_start;
    if( N2H(stts_entry->count) == 0 )
    {
      /* Count delta time of sample. */
      if (reverse && (stts_table_pos>0) )
      {
        stream->sample_timestamp -= N2H (stts_entry->delta);
        stream->stts.current_table_pos = stts_table_pos-1;
        return;
      }
      else if(stts_table_pos<(stream->stts.table_size-1))
      {
        stream->sample_timestamp += N2H (stts_entry->delta);
        stream->stts.current_table_pos = stts_table_pos+1;
        return;
      }
    }
  }

  /* if there is any CTTS atom present */
  if(stream->ctts.table_size)
  {
    /* Cache 'ctts' table entries. */
    cache_needed = FALSE;
    if (reverse)
    {
      /* Check if current position is at or before first sample in
      ** the cached region.
      */
      if (stream->ctts.current_table_pos <= stream->ctts.cache_start)
      {
        /* Exception: if current position is on first sample,
        ** do not recache.
        */
        if (stream->ctts.current_table_pos != 0)
        {
          cache_needed = TRUE;
        }
      }

      /* Check if current position is beyond one past the last
      ** sample in the cached region. */
      else if (stream->ctts.current_table_pos
               > stream->ctts.cache_start
               +(int32) stream->ctts.cache_size)
      {
        cache_needed = TRUE;
      }
    }
    else
    {
      /* Check if current position is before first sample in
      ** cached region.
      */
      if (stream->ctts.current_table_pos < stream->ctts.cache_start)
      {
        cache_needed = TRUE;
      }

      /* Check if current position is at or beyond one past the last
      ** sample in the cached region.
      */
      else if (stream->ctts.current_table_pos
               >= stream->ctts.cache_start
               +(int32) stream->ctts.cache_size)
      {
        cache_needed = TRUE;
        if(stream->ctts.current_table_pos >= stream->ctts.table_size)
            cache_needed = FALSE;
      }
    }
    if (cache_needed)
    {
      /* Update cache information. */
      if (reverse)
      {
        set_cache (stream,
                   (stream->ctts.current_table_pos + 1
                    - MIN (stream->ctts.current_table_pos + 1, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                   VIDEO_FMT_MP4R_CTTS_TABLE);

      }
      else
      {
        set_cache (stream, stream->ctts.current_table_pos, VIDEO_FMT_MP4R_CTTS_TABLE);

      }
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
      return;
    }
    else
    {
      uint32 ctts_table_pos = stream->ctts.current_table_pos;
      /* some clips have more samples than entries in CTTS table (SKT-MOD clips). So we make sure that
         we don't go beyond entries in CTTS table and keep using last entry values for all extra samples */
      if(ctts_table_pos >= stream->ctts.table_size)
        ctts_table_pos = stream->ctts.table_size-1;
    }
  }

  /* Cache 'stss' table entries, if 'stss' table is present and the
  ** current position is before the cache start, after the cache
  ** end, or there is a next entry in the table and the current
  ** entry is the last in the cache.
  */
  if (stream->stss.file_offset
      && stream->stss.table_size
      && ((stream->stss.current_table_pos
           < stream->stss.cache_start)

          /* Current position before cache start? */
          || ((stream->stss.current_table_pos
               == stream->stss.cache_start)
              && (stream->stss.current_table_pos > 0)
              && reverse)

          /* After the cache end? */
          || (stream->stss.current_table_pos
              >= stream->stss.cache_start + (int32)stream->stss.cache_size)

          /* Current entry is last in the cache and there is a next
          ** entry in the table?
          */
          || ((stream->stss.current_table_pos + 1
               >= (int32)stream->stss.cache_start
               + (int32)stream->stss.cache_size)
              && (stream->stss.cache_start + stream->stss.cache_size
                  < stream->stss.table_size))))
  {
    /* Update cache information. */
    if (reverse)
    {
      set_cache (stream,
                 (stream->stss.current_table_pos + 2
                  - MIN (stream->stss.current_table_pos + 2, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                 VIDEO_FMT_MP4R_STSS_TABLE);
    }
    else
    {
      set_cache (stream, stream->stss.current_table_pos, VIDEO_FMT_MP4R_STSS_TABLE);
    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* Cache 'stsc info' table entries. */
  if (reverse)
  {

    /* Cache 'stsc_info' table entries. */
    cache_needed = FALSE;
    /* Check if current position is at or before first sample in
    ** the cached region.
    */
    if (stream->stsc_info.current_table_pos <= stream->stsc_info.cache_start)
    {
      /* Exception: if current position is on first sample,
      ** do not recache.
      */
      if (stream->stsc_info.current_table_pos != 0)
      {
        cache_needed = TRUE;
      }
    }
    /* Check if current position is beyond one past the last
    ** sample in the cached region. */
    else if (stream->stsc_info.current_table_pos
            > stream->stsc_info.cache_start
            + (int32)stream->stsc_info.cache_size)
    {
      cache_needed = TRUE;
    }

    if (cache_needed)
    {
      /* Set up to read table into memory. */
      set_cache (stream,
                 (stream->stsc_info.current_table_pos + 1
                  - MIN (stream->stsc_info.current_table_pos + 1, (int32)stream->stsc_info.cache_size)),
                 VIDEO_FMT_MP4R_STSC_INFO_TABLE);
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
      return;
    }
  }
  else
  {
    if (NEED_CACHE(stream->stsc_info)/*lint!e574 */
        || ((stream->stsc_info.current_table_pos + 1 >= stream->stsc_info.cache_start
            + (int32)stream->stsc_info.cache_size)
            && (stream->stsc_info.cache_start + stream->stsc_info.cache_size
                < stream->stsc.table_size)))
    {
      /* Set up to read table into memory. */
      set_cache (stream, stream->stsc_info.current_table_pos, VIDEO_FMT_MP4R_STSC_INFO_TABLE);
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
      return;
    }
  }

  /* If the current sample is the next sample of interest, collect
  ** (and store, if the buffer pointer is not NULL) information
  ** about the current sample being iterated.
  */
  stsz_entry = stream->stsz_cache + stream->stsz.current_table_pos
               - stream->stsz.cache_start;
  stts_entry = stream->stts_cache + stream->stts.current_table_pos
               - stream->stts.cache_start;
  ctts_entry = stream->ctts_cache + stream->ctts.current_table_pos
               - stream->ctts.cache_start;
  stss_entry = stream->stss_cache + stream->stss.current_table_pos
               - stream->stss.cache_start;
  stsc_info_entry = stream->stsc_info_cache + stream->stsc_info.current_table_pos
                    - stream->stsc_info.cache_start;

  if (stream->get_sample_info_offset == stream->stsz.current_table_pos)
  {
    /* If the buffer pointer is not NULL, collect and store
    ** information about the sample.
    */
    if (stream->get_sample_info_buffer)
    {
      stream->get_sample_info_buffer->sample
      = stream->stsz.current_table_pos;
      stream->get_sample_info_buffer->size
      = (stream->sample_size ? stream->sample_size
         : N2H (stsz_entry->sample_size));
      if (!stream->stsz.current_table_pos)
      {
        if(stream->get_sample_info_buffer->size > (stream->get_sample_info_buffer->size+stream->header) )
        {
          MSG_ERROR ("get_sample_info: sample size overflow detected.", 0, 0, 0);
          stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
          stream->callback_ptr (VIDEO_FMT_DATA_CORRUPT,
                                stream->client_data,
                                NULL, NULL);
          return;
        }
        stream->get_sample_info_buffer->size
        += stream->header;
      }
      stream->get_sample_info_buffer->offset
      = stream->sample_byte_offset;
      stream->get_sample_info_buffer->time
      = stream->sample_timestamp;
      stream->get_sample_info_buffer->decode_time = stream->get_sample_info_buffer->time;
      stream->get_sample_info_buffer->delta = N2H (stts_entry->delta);
      if(stream->ctts.table_size)
      {
        /* Count delta time of sample. */
        stream->get_sample_info_buffer->time = stream->get_sample_info_buffer->decode_time + N2H (ctts_entry->offset);
      }

      /* If 'stss' table exists, frame might not be sync
      ** frame.
      */
      if (stream->stss.file_offset)
      {
        /* Check 'stss' table only if not empty. */
        if (stream->stss.cache_size)
        {
          stream->get_sample_info_buffer->sync
          = (stream->get_sample_info_offset + 1
             == N2H (stss_entry->sync_sample));
        }

        /* Otherwise, no all samples are sync samples. */
        else
        {
          stream->get_sample_info_buffer->sync = 0;
        }
      }

      /* Otherwise, all samples are sync samples. */
      else
      {
        stream->get_sample_info_buffer->sync = 1;
      }

      if (stream->stsc_info.cache_size)
      {
        stream->get_sample_info_buffer->sample_desc_index
        = N2H(stsc_info_entry->sample_desc_index);
      }

      ++stream->get_sample_info_buffer;
    }

    /* Count sample covered. */
    ++stream->get_sample_info_offset;
    ++stream->get_sample_info_total_samples;
    --stream->get_sample_info_size;
  }


  /* Move one sample forwards or backwards in sample table. */
  if (reverse)
  {
    /* Back up to previous 'stss' entry if there is an 'stss'
    ** table, there is a previous entry, and the current entry
    ** matches the current sample.
    */
    if (stream->stss.file_offset
        && (stream->stss.current_table_pos > 0)
        && (N2H (stss_entry->sync_sample)
            == stream->stsz.current_table_pos + 1))
    {
      --stream->stss.current_table_pos;
    }

    /* Back up to previous sample. */
    --stream->stsz.current_table_pos;
    --stsz_entry;

    /* Back up to previous 'stts' entry if all deltas in this
    ** table entry are accounted for, but back only if current STTS position
    ** is greater than zero.
    */
    if ((stream->sample_delta_count == 0) && (stream->stts.current_table_pos >0))
    {
      --stream->stts.current_table_pos;
      --stts_entry;
      stream->sample_delta_count = N2H (stts_entry->count);
    }

    if(stream->sample_delta_count>0)
      --stream->sample_delta_count;

    /* Count delta time of previous sample. */
    stream->sample_timestamp -= N2H (stts_entry->delta);

    /* Back up to previous 'ctts' entry if all deltas in this
    ** table entry are accounted for, but back only if current CTTS position
    ** is greater than zero.
    */
    if ((stream->sample_ctts_offset_count == 0) && (stream->ctts.current_table_pos >0))
    {
      --stream->ctts.current_table_pos;
      --ctts_entry;
      stream->sample_ctts_offset_count = N2H (ctts_entry->count);
    }

    if(stream->sample_ctts_offset_count>0)
      --stream->sample_ctts_offset_count;

    /* Count size of sample. */
    stream->sample_byte_offset
    -= (stream->sample_size ? stream->sample_size
        : N2H (stsz_entry->sample_size));
    if (!stream->stsz.current_table_pos)
    {
      stream->sample_byte_offset -= stream->header;
    }
    /* Back up to previous 'stsc' table entry if we've reached the first chunk of that entry. */
    if( (stream->stsc_info.current_table_pos+1 < (stream->stsc_info.cache_start+(int32)stream->stsc_info.cache_size))
        && (stream->stsc_info.current_table_pos > stream->stsc_info.cache_start))
    {
      if ( (++stream->stsc_sample_countdown >
                        ( (N2H((stsc_info_entry+1)->first_chunk) - N2H((stsc_info_entry)->first_chunk))
                          * N2H((stsc_info_entry)->samples_per_chunk)) ) )
      {
        --stream->stsc_info.current_table_pos;
        stream->stsc_sample_countdown = 1;
      }
    }
    else if(stream->stsc_info.current_table_pos > stream->stsc_info.cache_start)
    {
      if (++stream->stsc_sample_countdown >  (N2H((stsc_info_entry)->samples_per_chunk)
          * (stream->stco.table_size - N2H((stsc_info_entry)->first_chunk))))
      {
        --stream->stsc_info.current_table_pos;
        stream->stsc_sample_countdown = 1;
      }
    }
  }
  else
  {
    /* some clips have more samples than entries in STTS table (SKT-MOD clips). So we make sure that
        we don't go beyond entries in STTS table and keep using last entry values for all extra samples */
    if(stream->stts.current_table_pos >= (int32)stream->stts.table_size)
    {
      stream->stts.current_table_pos = stream->stts.table_size-1;
      stts_entry = stream->stts_cache + stream->stts.current_table_pos
                  - stream->stts.cache_start;
    }

    /* Count delta time of sample. */
    stream->sample_timestamp += N2H (stts_entry->delta);
    ++stream->sample_delta_count;

    /* Count size of sample. */
    stream->sample_byte_offset
    += (stream->sample_size ? stream->sample_size
        : N2H (stsz_entry->sample_size));
    if (!stream->stsz.current_table_pos)
    {
      stream->sample_byte_offset += stream->header;
    }

    /* If we're on the first sample, initialize the stsc_sample_countdown. */
    if ((0 == stream->stsz.current_table_pos)
        && (stream->stsc_info.cache_size > 1))
    {
      stream->stsc_sample_countdown =
      (N2H((stsc_info_entry+1)->first_chunk) - N2H(stsc_info_entry->first_chunk))
      * N2H(stsc_info_entry->samples_per_chunk);
    }

    /* Advance to next sample. */
    ++stream->stsz.current_table_pos;

    /* Advance to next 'stss' entry if there is an 'stss' table,
    ** there is a next entry, and the next entry matches the next
    ** sample.
    */
    if (stream->stss.file_offset
        && (stream->stss.current_table_pos + 1 < stream->stss.cache_start
            + (int32)stream->stss.cache_size)
        && (N2H ((stss_entry+1)->sync_sample)
            == stream->stsz.current_table_pos + 1))
    {
      ++stream->stss.current_table_pos;
    }

    /* Advance to next 'stts' entry if all deltas in this table
    ** entry are accounted for.
    */
    if (stream->sample_delta_count == N2H (stts_entry->count))
    {
      stream->stts.current_table_pos++;
      stream->sample_delta_count = 0;
    }

    ++stream->sample_ctts_offset_count;

    /* Advance to next 'ctts' entry if all deltas in this table
    ** entry are accounted for.
    */
    if (stream->sample_ctts_offset_count == N2H (ctts_entry->count))
    {
      stream->ctts.current_table_pos++;
      stream->sample_ctts_offset_count = 0;
    }

    /* Advance to next 'stsc' table entry if we've reached the first chunk of that entry. */
    if ((0 == --stream->stsc_sample_countdown)
        && (stream->stsc_info.current_table_pos + 1 < stream->stsc_info.cache_start
            + (int32)stream->stsc_info.cache_size))
    {
      ++stream->stsc_info.current_table_pos;
      if ((stream->stsc_info.current_table_pos + 1 < stream->stsc_info.cache_start
          + (int32)stream->stsc_info.cache_size))
      {
        stream->stsc_sample_countdown =
        (N2H((stsc_info_entry+2)->first_chunk) - N2H((stsc_info_entry+1)->first_chunk))
        * N2H((stsc_info_entry+1)->samples_per_chunk);
      }
      else
      {
        stream->stsc_sample_countdown = N2H((stsc_info_entry+1)->samples_per_chunk)
          * (stream->stco.table_size - N2H((stsc_info_entry+1)->first_chunk));
      }
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_find_sync_sample_state

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_STATE_FIND_SYNC_SAMPLE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_find_sync_sample_state (void *server_data, boolean *exit_loop)
{
  boolean cache_needed;
  boolean reverse;

  video_fmt_mp4r_stsz_entry_type     *stsz_entry;
  video_fmt_mp4r_stss_entry_type     *stss_entry;
  video_fmt_mp4r_stts_entry_type     *stts_entry;
  video_fmt_mp4r_ctts_entry_type     *ctts_entry;

  video_fmt_mp4r_trun_entry_type     *trun_entry = 0;
  uint32                             offset;
  video_fmt_mp4r_sample_table_type   *trun;


  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;
  trun = &stream->trun[stream->current_trun];


  if (stream->fragment_processing)
  {
    uint32  i;
    if(stream->fragment_repositioned)
    {
        trun->cache_start = 0;
        trun->current_table_pos = 0;

      /* Update cache information. */
      trun->cache_size
      = MIN (VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
             trun->table_size);

      set_cache_trun (stream, trun);

      stream->fragment_repositioned = FALSE;
      return;
    }


    if(stream->fill_trun_cache)
      process_fill_trun_cache(stream, trun);

    //Process the Fragment
    offset = stream->main_fragment_frames + stream->last_fragment_frames;

    for (i = 0; i < stream->current_trun; i++)
    {
      offset += stream->trun[i].table_size;
    }
#ifdef REPOSTN_DEBUG
  MSG_ERROR("offset:%d stream->current_trun:%d",offset, stream->current_trun, 0);
#endif

  /*Rewind case */
    if(stream->seek_reverse)
    {
    /* Check if current position is beyond one past the last*/
      if (!stream->seek_start_found && (trun->current_table_pos + offset == stream->get_sample_info_offset))
      {
        stream->seek_start_found = TRUE;
#ifdef REPOSTN_DEBUG
        MSG_ERROR("offset:%d stream->current_trun:%d",offset, stream->current_trun, 0);
    MSG_ERROR("trun->current_table_pos:%d stream->get_sample_info_offset:%d",trun->current_table_pos, stream->get_sample_info_offset, 0);
        MSG_ERROR("Setting stream->seek_start_found = TRUE",0,0,0);
#endif
    /*If trun->table_size == trun->current_table_pos then we need to update the current trun to
      process the last sample in the current trun*/
        if(trun->table_size == trun->current_table_pos)
        {
#ifdef REPOSTN_DEBUG
      MSG_ERROR("trun->table_size == trun->current_table_pos", 0,0,0);
      MSG_ERROR("trun->table_size:%d trun->current_table_pos:%d", trun->table_size,trun->current_table_pos,0);
#endif
          if(stream->current_trun < stream->trun_entry_count)
          {
#ifdef REPOSTN_DEBUG
        MSG_ERROR("stream->current_trun < stream->trun_entry_count", 0,0,0);
            MSG_ERROR("stream->current_trun:%d stream->trun_entry_count:%d", stream->current_trun,stream->trun_entry_count,0);
#endif
            ++stream->current_trun;
            trun = &stream->trun[stream->current_trun];
      trun->cache_start = 0;
      trun->current_table_pos = 0;

      /* Update cache information. */
      trun->cache_size = MIN (VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,trun->table_size);
      set_cache_trun (stream, trun);
      return;
      }
    }
    }
  }
    else
    {
    /* Check if current position is beyond one past the last*/
      if (!stream->seek_start_found
        && (trun->current_table_pos + offset
            == stream->get_sample_info_offset))
      {
        stream->seek_start_found = TRUE;
      }
    }


    /* Determine whether we're currently moving forwards or reverse
    ** through the sample table.
    */
    if (stream->seek_start_found)
    {
      reverse = stream->seek_reverse;
    }
    else
    {
      reverse = (offset + trun->current_table_pos
                 > stream->get_sample_info_offset);
    }

    /* Cache 'trun' table entries if samples are variable-length and
    ** the current sample is not in the 'stsz' cache.
    */
    cache_needed = FALSE;
    if (reverse)
    {
      /* Check if current position is at or before first sample in
      ** the cached region.
      */
      if (trun->current_table_pos < trun->cache_start)
      {
        /* Exception: if current position is on first sample,
        ** do not recache.
        */
        if (stream->current_trun != 0)
        {
          --stream->current_trun;
          cache_needed = TRUE;
          if (stream->current_trun < 0)
          {
            stream->current_trun = 0;
            cache_needed = FALSE;
          }
          trun = &stream->trun[stream->current_trun];
        }
      }
      /* Check if current position is beyond one past the last
      ** sample in the cached region. */
      else if (trun->current_table_pos
               > trun->cache_start
               + trun->cache_size)
      {
        ++stream->current_trun;
        trun = &stream->trun[stream->current_trun];
        if (stream->current_trun >= stream->trun_entry_count)
          stream->current_trun = stream->trun_entry_count - 1;
        cache_needed = TRUE;
      }
    }
    else
    {
      cache_needed = FALSE;

      /* Check if current position is before first sample in
      ** cached region.
      */
      if (trun->current_table_pos < trun->cache_start)
      {
        --stream->current_trun;
        trun = &stream->trun[stream->current_trun];
        cache_needed = TRUE;
        if (stream->current_trun < 0)
        {
          stream->current_trun = 0;
          cache_needed = FALSE;
        }

      }

      /* Check if current position is at or beyond one past the last
      ** sample in the cached region.
      */
      else if (trun->current_table_pos
               >= trun->cache_start
               + trun->cache_size)
      {
        ++stream->current_trun;
        trun = &stream->trun[stream->current_trun];
        if (stream->current_trun > stream->trun_entry_count)
          stream->current_trun = stream->trun_entry_count;
        cache_needed = TRUE;
      }
    }

    if (cache_needed)
    {
      /* Update cache information. */
      if (reverse)
      {
        trun->cache_start = trun->current_table_pos + 1
                            - MIN (trun->current_table_pos + 1,
                                   VIDEO_FMT_MP4R_TABLE_CACHE_SIZE);
        trun->current_table_pos = trun->table_size - 1;
      }
      else
      {
        trun->cache_start = 0;
        trun->current_table_pos = 0;
      }
      /* Update cache information. */
      trun->cache_size
      = MIN (VIDEO_FMT_MP4R_TABLE_CACHE_SIZE,
             trun->table_size);

      set_cache_trun (stream, trun);
      return;
    }

    if(stream->fill_trun_cache)
      process_fill_trun_cache(stream, trun);

    /* If the current sample is the next sample of interest, collect
    ** and store information about the current sample being iterated.
    */
    trun_entry = stream->trun_cache
                   + trun->current_table_pos
                   - trun->cache_start;

  stream->get_sample_info_buffer->sync = 0;

  /* If the first sample flag is present and set
       and we are at the start of the trun then
       return the first sample as sync frame.
       if the first sample flag is not present,
       the sample flags are set and the current sample
       is the I-Frame then retun that sample as sync frame.
    */
    if (stream->seek_start_found
       /* first sample flag is present */
       && ((trun->tr_flag_first_sample_flag_present
       /* current position in the current trun is zero means first position */
        && (trun->current_table_pos == 0)
       /* first sample flags should be zero for an I-frame*/
        && (trun->first_sample_flags == FLAG_I_VOP))

       /* first sample flags are not present */
       || ((!trun->tr_flag_first_sample_flag_present)
       /* sample flags are set */
       && (trun->tr_flag_sample_flags_present)
       /* current position should not be more than trun size - 1 */
       && (trun->current_table_pos <=  trun->table_size - 2 )
       /* the sample flags should be zero for an I-Frame */
       && (N2H (trun_entry->sample_flags)== FLAG_I_VOP ))

     || ((!trun->tr_flag_first_sample_flag_present)&& (!trun->tr_flag_sample_flags_present)
     && (stream->default_sample_flags == FLAG_I_VOP))))
    {
      stream->get_sample_info_buffer->sample
      = offset + trun->current_table_pos;

      /*if sample_duration is present in the trun get the value of the duration*/
      if(trun->tr_flag_sample_duration_present)
      {
        stream->get_sample_info_buffer->delta
                  = N2H (trun_entry->sample_duration);
      }
      else
      {
        stream->get_sample_info_buffer->delta = trun->default_sample_duration;
      }

      /*if sample_size is present in the trun get the value of the duration*/
      if(trun->tr_flag_sample_size_present)
      {
        stream->get_sample_info_buffer->size
                  = N2H (trun_entry->sample_size);
      }
      else
      {
        stream->get_sample_info_buffer->size = trun->default_sample_size;
      }

      if (offset + trun->current_table_pos == stream->last_sample_offset)
      {
        --stream->last_sample_offset;
        /*if sample_duration is present in the trun get the value of the duration*/
        if(trun->tr_flag_sample_duration_present)
        {
          stream->sample_timestamp -= N2H (trun_entry->sample_duration);
        }
        else
        {
          stream->sample_timestamp -= trun->default_sample_duration;
        }

        /*if sample_size is present in the trun get the value of the duration*/
        if(trun->tr_flag_sample_size_present)
        {
          stream->sample_byte_offset -= N2H(trun_entry->sample_size);
        }
        else
        {
          stream->sample_byte_offset -= trun->default_sample_size;
        }
      }
      stream->get_sample_info_buffer->time = stream->sample_timestamp;
      stream->get_sample_info_buffer->decode_time = stream->get_sample_info_buffer->time;
      stream->get_sample_info_buffer->offset = stream->sample_byte_offset;

      /* set Sync Sample flag */

      stream->get_sample_info_buffer->sync = 1;

      ++stream->get_sample_info_buffer;
      stream->chunk_start_sample = 0;

      /* Issue an I/O done callback to the client to indicate all the
      ** information has been generated, and return to the "ready"
      ** state.
      */
      stream->cb_info.io_done.bytes
      = sizeof (video_fmt_sample_info_type);
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
      stream->callback_ptr (VIDEO_FMT_IO_DONE,
                            stream->client_data,
                            &stream->cb_info,
                            video_fmt_mp4r_end);
      *exit_loop = TRUE;
      return;
    }

    /* If we are at the first sample, if seeking backward, or the last
    ** sample, if seeking forward, but have not found a sync sample,
    ** issue an I/O done callback to the client, but set bytes read to
    ** zero to indicate that the previous sync sample was not found,
    ** and return to the "ready" state.
    */
    if (((stream->current_trun == 0)
         && (trun->current_table_pos == 0) && reverse)
        || ((stream->current_trun + 1 >= stream->trun_entry_count)
            && (trun->current_table_pos + 1 >= trun->table_size)
            && !reverse))
    {
      stream->cb_info.io_done.bytes = 0;
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
      stream->callback_ptr (VIDEO_FMT_IO_DONE,
                            stream->client_data,
                            &stream->cb_info,
                            video_fmt_mp4r_end);
      *exit_loop = TRUE;
      return;
    }

    /* Move one sample forwards or backwards in trun. */
    if (reverse)
    {
      if (offset + trun->current_table_pos <= stream->last_sample_offset)
      {

        /* Count delta time of previous sample. */
        if(trun->tr_flag_sample_duration_present)
        {
          stream->sample_timestamp -= N2H (trun_entry->sample_duration);
        }
        else
        {
          stream->sample_timestamp -= trun->default_sample_duration;
        }

        /*if sample_size is present in the trun get the value of the duration*/
        if(trun->tr_flag_sample_size_present)
        {
          stream->sample_byte_offset -= N2H(trun_entry->sample_size);
        }
        else
        {
          stream->sample_byte_offset -= trun->default_sample_size;
        }
        --stream->last_sample_offset;
      }
      --trun->current_table_pos;
      --stream->sample_delta_count;
    }
    else
    {
      if (offset + trun->current_table_pos > stream->last_sample_offset)
      {
        /* Count delta time of sample. */
        if(trun->tr_flag_sample_duration_present)
        {
          stream->sample_timestamp += N2H (trun_entry->sample_duration);
        }
        else
        {
           stream->sample_timestamp += trun->default_sample_duration;
        }

        /*if sample_size is present in the trun get the value of the duration*/
        if(trun->tr_flag_sample_size_present)
        {
          stream->sample_byte_offset += N2H(trun_entry->sample_size);
        }
        else
        {
          stream->sample_byte_offset += trun->default_sample_size;
        }
        ++stream->last_sample_offset;
      }
      ++trun->current_table_pos;
      ++stream->sample_delta_count;
    }
    return;
  }

  /* If we know the 'stss' table is empty, and then all the samples are sync samples in the main fragment*/
  if(0 == stream->stss.table_size)
  {
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_SAMPLE_INFO;
    stream->get_sample_info_size = 1;
    *exit_loop = FALSE;
    return;
  }

  /* Check if we have reached the seek start point. */
  if (!stream->seek_start_found
      && (stream->stsz.current_table_pos
          == stream->get_sample_info_offset))
  {
    stream->seek_start_found = TRUE;
  }

  /* Determine whether we're currently moving forwards or reverse
  ** through the sample table.
  */
  if (stream->seek_start_found)
  {
    reverse = stream->seek_reverse;
  }
  else
  {
    reverse = (stream->stsz.current_table_pos
               > (int32)stream->get_sample_info_offset);
  }

  /* Cache 'stsz' table entries if samples are variable-length and
  ** the current sample is not in the 'stsz' cache.
  */
  cache_needed = !stream->sample_size;
  if (cache_needed && reverse)
  {
    cache_needed = FALSE;

    /* Check if current position is at or before first sample in
    ** the cached region.
    */
    if (stream->stsz.current_table_pos <= stream->stsz.cache_start)
    {
      /* Exception: if current position is on first sample,
      ** do not recache.
      */
      if (stream->stsz.current_table_pos != 0)
      {
        cache_needed = TRUE;
      }
    }

    /* Check if current position is beyond one past the last
    ** sample in the cached region. */
    else if (stream->stsz.current_table_pos
             > stream->stsz.cache_start
             + (int32)stream->stsz.cache_size)
    {
      cache_needed = TRUE;
    }
  }
  else if (cache_needed && !reverse)
  {
    cache_needed = FALSE;

    /* Check if current position is before first sample in
    ** cached region.
    */
    if (stream->stsz.current_table_pos < stream->stsz.cache_start)
    {
      cache_needed = TRUE;
    }

    /* Check if current position is at or beyond one past the last
    ** sample in the cached region.
    */
    else if (stream->stsz.current_table_pos
             >= stream->stsz.cache_start
             + (int32)stream->stsz.cache_size)
    {
      cache_needed = TRUE;
    }
  }
  if (cache_needed)
  {
    /* Update cache information. */
    if (reverse)
    {
      set_cache (stream,
                 (stream->stsz.current_table_pos + 1
                  - MIN (stream->stsz.current_table_pos + 1, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                 VIDEO_FMT_MP4R_STSZ_TABLE);
    }
    else
    {
      set_cache (stream, stream->stsz.current_table_pos, VIDEO_FMT_MP4R_STSZ_TABLE);
    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* Cache 'stts' table entries. */
  cache_needed = FALSE;
  if (reverse)
  {
    /* Check if current position is at or before first sample in
    ** the cached region.
    */
    if (stream->stts.current_table_pos <= stream->stts.cache_start)
    {
      /* Exception: if current position is on first sample,
      ** do not recache.
      */
      if (stream->stts.current_table_pos != 0)
      {
        cache_needed = TRUE;
      }
    }

    /* Check if current position is beyond one past the last
    ** sample in the cached region. */
    else if (stream->stts.current_table_pos
             > stream->stts.cache_start
             + (int32)stream->stts.cache_size)
    {
      cache_needed = TRUE;
    }
  }
  else
  {
    /* Check if current position is before first sample in
    ** cached region.
    */
    if (stream->stts.current_table_pos < stream->stts.cache_start)
    {
      cache_needed = TRUE;
    }

    /* Check if current position is at or beyond one past the last
    ** sample in the cached region.
    */
    else if (stream->stts.current_table_pos
             >= stream->stts.cache_start
             + (int32)stream->stts.cache_size)
    {
      cache_needed = TRUE;
      if(stream->stts.current_table_pos >= (int32)stream->stts.table_size)
          cache_needed = FALSE;
    }
  }
  if (cache_needed)
  {
    /* Update cache information. */
    if (reverse)
    {
      set_cache (stream,
                 (stream->stts.current_table_pos + 1
                  - MIN (stream->stts.current_table_pos + 1, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                 VIDEO_FMT_MP4R_STTS_TABLE);

    }
    else
    {
      set_cache (stream, stream->stts.current_table_pos, VIDEO_FMT_MP4R_STTS_TABLE);

    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }
  else
  {
    uint32 stts_table_pos = stream->stts.current_table_pos;
    /* some clips have more samples than entries in STTS table (SKT-MOD clips). So we make sure that
        we don't go beyond entries in STTS table and keep using last entry values for all extra samples */
    if(stts_table_pos >= stream->stts.table_size)
      stts_table_pos = stream->stts.table_size-1;

    /* some SKTT clips have zero sample count in some STTS entries, so we need to add
       their time delta and goto next entry and update cache if needed */
    stts_entry = stream->stts_cache + stts_table_pos
                - stream->stts.cache_start;
    if( N2H(stts_entry->count) == 0 )
    {
      /* Count delta time of sample. */
      if (reverse && (stts_table_pos>0) )
      {
        stream->sample_timestamp -= N2H (stts_entry->delta);
        stream->stts.current_table_pos = stts_table_pos-1;
        return;
      }
      else if(stts_table_pos<(stream->stts.table_size-1))
      {
        stream->sample_timestamp += N2H (stts_entry->delta);
        stream->stts.current_table_pos = stts_table_pos+1;
        return;
      }
    }
  }

  /* if there is any CTTS atom present */
  if(stream->ctts.table_size)
  {
    /* Cache 'ctts' table entries. */
    cache_needed = FALSE;
    if (reverse)
    {
      /* Check if current position is at or before first sample in
      ** the cached region.
      */
      if (stream->ctts.current_table_pos <= stream->ctts.cache_start)
      {
        /* Exception: if current position is on first sample,
        ** do not recache.
        */
        if (stream->ctts.current_table_pos != 0)
        {
          cache_needed = TRUE;
        }
      }

      /* Check if current position is beyond one past the last
      ** sample in the cached region. */
      else if (stream->ctts.current_table_pos
               > stream->ctts.cache_start
               + (int32)stream->ctts.cache_size)
      {
        cache_needed = TRUE;
      }
    }
    else
    {
      /* Check if current position is before first sample in
      ** cached region.
      */
      if (stream->ctts.current_table_pos < stream->ctts.cache_start)
      {
        cache_needed = TRUE;
      }

      /* Check if current position is at or beyond one past the last
      ** sample in the cached region.
      */
      else if (stream->ctts.current_table_pos
               >= stream->ctts.cache_start
               + (int32)stream->ctts.cache_size)
      {
        cache_needed = TRUE;
        if(stream->ctts.current_table_pos >= stream->ctts.table_size)
            cache_needed = FALSE;
      }
    }
    if (cache_needed)
    {
      /* Update cache information. */
      if (reverse)
      {
        set_cache (stream,
                   (stream->ctts.current_table_pos + 1
                    - MIN (stream->ctts.current_table_pos + 1, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                   VIDEO_FMT_MP4R_CTTS_TABLE);

      }
      else
      {
        set_cache (stream, stream->ctts.current_table_pos, VIDEO_FMT_MP4R_CTTS_TABLE);

      }
      stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
      return;
    }
    else
    {
      uint32 ctts_table_pos = stream->ctts.current_table_pos;
      /* some clips have more samples than entries in CTTS table (SKT-MOD clips). So we make sure that
         we don't go beyond entries in CTTS table and keep using last entry values for all extra samples */
      if(ctts_table_pos >= stream->ctts.table_size)
        ctts_table_pos = stream->ctts.table_size-1;

    /* some SKTT clips have zero sample count in some ctts entries, so we need to add
       their time delta and goto next entry and update cache if needed */
    ctts_entry = stream->ctts_cache + ctts_table_pos
                - stream->ctts.cache_start;
    if( N2H(ctts_entry->count) == 0 )
    {
      /* Count delta time of sample. */
      if (reverse && (ctts_table_pos>0) )
      {       
        stream->ctts.current_table_pos = ctts_table_pos-1;
        return;
      }
      else if(ctts_table_pos<(stream->ctts.table_size-1))
      {       
        stream->ctts.current_table_pos = ctts_table_pos+1;
        return;
      }
    }
    }
  }

  /* Cache 'stss' table entries, if 'stss' table is present and the
  ** current position is before the cache start, after the cache
  ** end, or there is a next entry in the table and the current
  ** entry is the last in the cache.
  */
  if (stream->stss.file_offset
      && stream->stss.table_size
      && ((stream->stss.current_table_pos
           < stream->stss.cache_start)

          /* Current position before cache start? */
          || ((stream->stss.current_table_pos
               == stream->stss.cache_start)
              && (stream->stss.current_table_pos > 0)
              && reverse)

          /* After the cache end? */
          || (stream->stss.current_table_pos
              >= stream->stss.cache_start + (int32)stream->stss.cache_size)

          /* Current entry is last in the cache and there is a next
          ** entry in the table?
          */
          || ((stream->stss.current_table_pos + 1
               >= stream->stss.cache_start
               + (int32)stream->stss.cache_size)
              && (stream->stss.cache_start + stream->stss.cache_size
                  < stream->stss.table_size))))
  {
    /* Update cache information. */
    if (reverse)
    {
      set_cache (stream,
                 (stream->stss.current_table_pos + 2
                  - MIN (stream->stss.current_table_pos + 2, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                 VIDEO_FMT_MP4R_STSS_TABLE);
    }
    else
    {
      set_cache (stream, stream->stss.current_table_pos, VIDEO_FMT_MP4R_STSS_TABLE);
    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* If the current sample is the next sample of interest, collect
  ** and store information about the current sample being iterated.
  */
  stsz_entry = stream->stsz_cache + stream->stsz.current_table_pos
               - stream->stsz.cache_start;
  stts_entry = stream->stts_cache + stream->stts.current_table_pos
               - stream->stts.cache_start;
  ctts_entry = stream->ctts_cache + stream->ctts.current_table_pos
               - stream->ctts.cache_start;
  stss_entry = stream->stss_cache + stream->stss.current_table_pos
               - stream->stss.cache_start;
  if (stream->seek_start_found
      && (stream->stsz.current_table_pos + 1 == N2H (stss_entry->sync_sample)))
  {
    /* Collect and store information about the sample. */
    stream->get_sample_info_buffer->sample
    = stream->stsz.current_table_pos;
    stream->get_sample_info_buffer->size
    = (stream->sample_size ? stream->sample_size
       : N2H (stsz_entry->sample_size));
    if (!stream->stsz.current_table_pos)
    {
      if(stream->get_sample_info_buffer->size > (stream->get_sample_info_buffer->size+stream->header) )
      {
        MSG_ERROR ("get_sample_info: sample size overflow detected.", 0, 0, 0);
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
        stream->callback_ptr (VIDEO_FMT_DATA_CORRUPT,
                              stream->client_data,
                              NULL, NULL);
        return;
      }
      stream->get_sample_info_buffer->size
      += stream->header;
    }
    stream->get_sample_info_buffer->offset
    = stream->sample_byte_offset;
    stream->get_sample_info_buffer->time
    = stream->sample_timestamp;
    stream->get_sample_info_buffer->decode_time = stream->get_sample_info_buffer->time;
    stream->get_sample_info_buffer->delta = N2H (stts_entry->delta);
    if(stream->ctts.table_size)
    {
      stream->get_sample_info_buffer->time = stream->get_sample_info_buffer->decode_time + N2H (ctts_entry->offset);      
    }
    stream->get_sample_info_buffer->sync
    = (stream->stss.file_offset
       ? (stream->stsz.current_table_pos + 1
          == N2H (stss_entry->sync_sample)) : 1);
    ++stream->get_sample_info_buffer;

    /* Issue an I/O done callback to the client to indicate all the
    ** information has been generated, and return to the "ready"
    ** state.
    */
    stream->cb_info.io_done.bytes
    = sizeof (video_fmt_sample_info_type);
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_IO_DONE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  /* If we are at the first sample, if seeking backward, or the last
  ** sample, if seeking forward, but have not found a sync sample,
  ** issue an I/O done callback to the client, but set bytes read to
  ** zero to indicate that the previous sync sample was not found,
  ** and return to the "ready" state.
  */
  if (((stream->stsz.current_table_pos == 0) && reverse)
      || ((stream->stsz.current_table_pos + 1 >= (int32)stream->stsz.table_size)
          && !reverse))
  {
    stream->cb_info.io_done.bytes = 0;
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_IO_DONE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  /* Move one sample forwards or backwards in sample table. */
  if (reverse)
  {
    /* Back up to previous 'stss' entry if there is an 'stss'
    ** table, there is a previous entry, and the current entry
    ** matches the current sample.
    */
    if (stream->stss.file_offset
        && (stream->stss.current_table_pos > 0)
        && (N2H (stss_entry->sync_sample)
            == stream->stsz.current_table_pos + 1))
    {
      --stream->stss.current_table_pos;
    }

    /* Back up to previous sample. */
    --stream->stsz.current_table_pos;
    --stsz_entry;

    /* Back up to previous 'stts' entry if all deltas in this
    ** table entry are accounted for, but back only if current STTS position
    ** is greater than zero.
    */
    if ((stream->sample_delta_count == 0) && (stream->stts.current_table_pos >0))
    {
      --stream->stts.current_table_pos;
      --stts_entry;
      stream->sample_delta_count = N2H (stts_entry->count);
    }

    if(stream->sample_delta_count>0)
      --stream->sample_delta_count;

    /* Count delta time of previous sample. */
    stream->sample_timestamp -= N2H (stts_entry->delta);
    /* Back up to previous 'ctts' entry if all deltas in this
    ** table entry are accounted for, but back only if current CTTS position
    ** is greater than zero.
    */
    if ((stream->sample_ctts_offset_count == 0) && (stream->ctts.current_table_pos >0))
    {
      --stream->ctts.current_table_pos;
      --ctts_entry;
      stream->sample_ctts_offset_count = N2H (ctts_entry->count);
    }

    if(stream->sample_ctts_offset_count>0)
      --stream->sample_ctts_offset_count;

    /* Count size of sample. */
    stream->sample_byte_offset
    -= (stream->sample_size ? stream->sample_size
        : N2H (stsz_entry->sample_size));
    if (!stream->stsz.current_table_pos)
    {
      stream->sample_byte_offset -= stream->header;
    }
  }
  else
  {
    /* some clips have more samples than entries in STTS table (SKT-MOD clips). So we make sure that
        we don't go beyond entries in STTS table and keep using last entry values for all extra samples */
    if(stream->stts.current_table_pos >= (int32)stream->stts.table_size)
    {
      stream->stts.current_table_pos = stream->stts.table_size-1;
      stts_entry = stream->stts_cache + stream->stts.current_table_pos
                  - stream->stts.cache_start;
    }

    /* Count delta time of sample. */
    stream->sample_timestamp += N2H (stts_entry->delta);
    ++stream->sample_delta_count;

    /* Count size of sample. */
    stream->sample_byte_offset
    += (stream->sample_size ? stream->sample_size
        : N2H (stsz_entry->sample_size));
    if (!stream->stsz.current_table_pos)
    {
      stream->sample_byte_offset += stream->header;
    }

    /* Advance to next sample. */
    ++stream->stsz.current_table_pos;

    /* Advance to next 'stss' entry if there is an 'stss' table,
    ** there is a next entry, and the next entry matches the next
    ** sample.
    */
    if (stream->stss.file_offset
        && (stream->stss.current_table_pos + 1 < stream->stss.cache_start
            + (int32)stream->stss.cache_size)
        && (N2H ((stss_entry+1)->sync_sample)
            == stream->stsz.current_table_pos + 1))
    {
      ++stream->stss.current_table_pos;
      if ( (stream->stss.current_table_pos + 1) == stream->stss.table_size )
      {
        /* Last I-frame, get it! */
        stream->get_sample_info_offset = stream->stsz.current_table_pos;
      }
    }

    /* Advance to next 'stts' entry if all deltas in this table
    ** entry are accounted for.
    */
    if (stream->sample_delta_count == N2H (stts_entry->count))
    {
      stream->stts.current_table_pos++;
      stream->sample_delta_count = 0;
    }

    /* some clips have more samples than entries in CTTS table (SKT-MOD clips). So we make sure that
       we don't go beyond entries in CTTS table and keep using last entry values for all extra samples */
    if(stream->ctts.current_table_pos >= stream->ctts.table_size)
    {
      stream->ctts.current_table_pos = stream->ctts.table_size-1;
      ctts_entry = stream->ctts_cache + stream->ctts.current_table_pos
                  - stream->ctts.cache_start;
    }

    ++stream->sample_ctts_offset_count;   
   
    /* Advance to next 'ctts' entry if all deltas in this table
    ** entry are accounted for.
    */
    if (stream->sample_ctts_offset_count == N2H (ctts_entry->count))
    {
      stream->ctts.current_table_pos++;
      stream->sample_ctts_offset_count = 0;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  process_get_access_point_state

DESCRIPTION
  This function processes the state VIDEO_FMT_MP4R_STREAM_GET_ACCESS_POINT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_get_access_point_state (void *server_data, boolean *exit_loop)
{
  boolean reverse = FALSE;

  video_fmt_tfra_entry_type     *tfra_entry = NULL;

  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;

  /* If we know the 'tfra' table is empty, and there are no access
  ** points, set the bytes read to zero to indicate that the next
  ** sync sample was not found, and return to the "ready" state.
  */
  if (0 == stream->tfra.table_size)
  {
    stream->cb_info.io_done.bytes = 0;
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_IO_DONE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  if(!NEED_CACHE(stream->tfra))
  {
    /* If the current sample is the next sample of interest, collect
    ** and store information about the current sample being iterated.
    */
    tfra_entry = stream->tfra_cache + stream->tfra.current_table_pos
                - stream->tfra.cache_start;
    /* Check if we have reached the seek start point. */
    if (!stream->seek_start_found
        && ((N2H (tfra_entry->access_point_time)
                >= stream->get_access_point_timestamp) ||
            (stream->tfra.current_table_pos + 1 >= stream->tfra.table_size)
           )
       )
    {
        stream->seek_start_found = TRUE;
    }

    /* Determine whether we're currently moving forwards or reverse
    ** through the sample table.
    */
    if (stream->seek_start_found)
    {
        reverse = stream->seek_access_point_reverse;
    }
    else
    {
        reverse = (N2H(tfra_entry->access_point_time)
                > stream->get_access_point_timestamp);
    }
  }
  else
  {
      if(!stream->seek_start_found)
          reverse = FALSE;
  }

  /* Cache 'tfra' table entries, if 'tfra' table is present and the
  ** current position is before the cache start, after the cache
  ** end, or there is a next entry in the table and the current
  ** entry is the last in the cache.
  */
  if (stream->tfra.file_offset
      && NEED_CACHE(stream->tfra))
  {
    /* Update cache information. */
    if (reverse)
    {
      set_cache (stream,
                 (stream->tfra.current_table_pos + 1
                  - MIN (stream->tfra.current_table_pos + 1, VIDEO_FMT_MP4R_TABLE_CACHE_SIZE)),
                 VIDEO_FMT_MP4R_TFRA_TABLE);
    }
    else
    {
      set_cache (stream, stream->tfra.current_table_pos, VIDEO_FMT_MP4R_TFRA_TABLE);
    }
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA;
    return;
  }

  /* Check if we have reached the seek start point. */
  if(stream->seek_start_found)
  {
      if(reverse)
      {
          if(N2H (tfra_entry->access_point_time)
            < stream->get_access_point_timestamp)
            --stream->tfra_skip;
      }
      else
      {
          if(N2H (tfra_entry->access_point_time)
            > stream->get_access_point_timestamp)
          ++stream->tfra_skip;
      }
  }

  if ((stream->seek_start_found) &&
      (stream->get_access_point_skipnumber
          == stream->tfra_skip))
  {
    /* Collect and store information about the sample. */
    stream->get_access_point_buffer->access_point_time
        = N2H (tfra_entry->access_point_time);
    stream->get_access_point_buffer->moof_offset
        = N2H (tfra_entry->moof_offset);
    stream->get_access_point_buffer->sample_number
        = tfra_entry->sample_number;
    stream->get_access_point_buffer->traf_number
        = tfra_entry->traf_number;
    stream->get_access_point_buffer->trun_number
        = tfra_entry->trun_number;
    ++stream->get_access_point_buffer;

    /* Issue an I/O done callback to the client to indicate all the
    ** information has been generated, and return to the "ready"
    ** state.
    */
    stream->cb_info.io_done.bytes
    = sizeof (video_fmt_tfra_entry_type);
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_IO_DONE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  /* If we are at the first sample, if seeking backward, or the last
  ** sample, if seeking forward, but have not found a sync sample,
  ** issue an I/O done callback to the client, but set bytes read to
  ** zero to indicate that the previous sync sample was not found,
  ** and return to the "ready" state.
  */
  if (((stream->tfra.current_table_pos == 0) && reverse)
      || ((stream->tfra.current_table_pos + 1 >= stream->tfra.table_size)
          && !reverse))
  {
    stream->cb_info.io_done.bytes = 0;
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
    stream->callback_ptr (VIDEO_FMT_IO_DONE,
                          stream->client_data,
                          &stream->cb_info,
                          video_fmt_mp4r_end);
    *exit_loop = TRUE;
    return;
  }

  /* Move one sample forwards or backwards in sample table. */
  if (reverse)
  {
    --stream->tfra.current_table_pos;
  }
  else
  {
    ++stream->tfra.current_table_pos;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4r_stream_process

DESCRIPTION
  This function runs the MP4 format stream state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4r_stream_process (void *server_data)
{

  boolean                      exit_loop = FALSE;
  video_fmt_mp4r_stream_type   *stream = (video_fmt_mp4r_stream_type *) server_data;

  while (!exit_loop)
  {
    switch (stream->state)
    {
      case VIDEO_FMT_MP4R_STREAM_STATE_GET_DATA:
        /* No matter what, exit the state machine loop each time this
        ** state is visited.  This is necessary in order to limit the
        ** amount of processing done each state machine iteration.
        */
        exit_loop = TRUE;
        process_get_data_state (server_data);
        break;

      case VIDEO_FMT_MP4R_STREAM_STATE_READY:
        /* Idle state - nothing to do. */
        exit_loop = TRUE;
        break;

      case VIDEO_FMT_MP4R_STREAM_STATE_STSZ_COUNT:
        /* If sample to count is outside range of cached region of stsz
        ** table, update cache.
        */
        process_stsz_count_state (server_data);
        break;


      case VIDEO_FMT_MP4R_STREAM_STATE_TRUN_COUNT:
        /* If sample to count is outside range of cached region of TRUN
        ** table, update cache.
        */
        process_trun_count_state (server_data);
        break;

      case VIDEO_FMT_MP4R_STREAM_STATE_FIND_ABS_OFFSET:
          process_find_abs_file_Offset(server_data,&exit_loop);
          break;

      case VIDEO_FMT_MP4R_STREAM_STATE_READ:
        /* If no more bytes are needed, issue an I/O done callback to the
        ** client to indicate all the bytes have been read, and return to
        ** the "ready" state.
        */
        process_read_state (server_data, &exit_loop);
        break;

      case VIDEO_FMT_MP4R_STREAM_STATE_GET_SAMPLE_INFO:
        /* If no more sample information entries are needed, issue an I/O
        ** done callback to the client to indicate all the information has
        ** been generated, and return to the "ready" state.
        */
        process_get_sample_info_state (server_data, &exit_loop);
        break;

      case VIDEO_FMT_MP4R_STREAM_STATE_FIND_SYNC_SAMPLE:
        process_find_sync_sample_state (server_data, &exit_loop);
        break;

      case VIDEO_FMT_MP4R_STREAM_STATE_GET_ACCESS_POINT:
        process_get_access_point_state (server_data, &exit_loop);
        break;

      case VIDEO_FMT_MP4R_STREAM_STATE_INVALID:
      default:
        /* We should hopefully never reach here.  However, if we do,
        ** return the stream state to ready and return.  This should abort
        ** any stream access in progress.
        */
        MSG_ERROR ("stream_process: Invalid state", 0, 0, 0);
        stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READY;
        stream->callback_ptr (VIDEO_FMT_FAILURE,
                              stream->client_data,
                              NULL, video_fmt_mp4r_end);
        return;
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4r_read_stream

DESCRIPTION
  This function is given to the client as a callback.  It is called in order
  to request data to be read from a stream in the media file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4r_read_stream
(
  uint32                        stream_number,
  video_fmt_data_unit_type      unit,
  uint32                        offset,        /* in units */
  uint32                        size,          /* in units */
  uint8                         *buffer,
  void                          *server_data,
  video_fmt_status_cb_func_type callback_ptr,
  void                          *client_data
)
{
    video_fmt_mp4r_context_type *context;
    video_fmt_mp4r_stream_type  *stream;
    video_fmt_stream_info_type  *stream_info;

    /* Return an error if the format services is not currently waiting in the
    ** proper state.
    */
    context = (video_fmt_mp4r_context_type *) server_data;
    if (context->state != VIDEO_FMT_MP4R_STATE_READY)
    {
        MSG_ERROR ("read_stream: format services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }

    /* Verify that the given stream number is valid. */
    if (stream_number >= context->num_streams)
    {
        MSG_ERROR ("read_stream: invalid stream number given!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, video_fmt_mp4r_end);
        return;
    }

    /* Return an error if the stream services is not currently waiting in the
    ** proper state.
    */
    stream = &context->stream_state [stream_number];
    if (stream->state != VIDEO_FMT_MP4R_STREAM_STATE_READY)
    {
        MSG_ERROR ("read_stream: stream services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }
    stream_info = &context->stream_info [stream_number];

    /* Verify that the given sample number and read size are valid, and
    ** determine whether or not read includes part of header.
    */
    switch (unit)
    {
    case VIDEO_FMT_DATA_UNIT_FRAME:
        /* Put given offset and size into range. */
        offset = MIN (offset, stream_info->frames);
        size = MIN (size, stream_info->frames - offset);

        /* If reading the first frame, include the header. */
        if (offset == 0)
        {
            stream->read_header_offset = 0;
        }
        else
        {
            stream->read_header_offset = stream_info->header;
        }
        stream->read_offset = offset;
        if( (offset >= stream->main_fragment_frames) && context->fragment_present )
        {
            /* getting samples from the fragments */
            stream->fragment_processing = TRUE;
        }
        else
            stream->fragment_processing = FALSE;
        break;

    case VIDEO_FMT_DATA_UNIT_BYTE:
        /* Count the bitstream header. */
        stream->read_header_offset = MIN (offset, stream_info->header);
        stream->read_offset
                = offset - MIN (offset, stream->read_header_offset);

        if(stream->main_fragment_bytes && (offset >= stream->main_fragment_bytes)
           && context->fragment_present )
        {
            /* getting samples from the fragments */
            stream->fragment_processing = TRUE;
        }
        else
            stream->fragment_processing = FALSE;
        break;

    default:
        MSG_ERROR ("read_stream: invalid read unit given!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, video_fmt_mp4r_end);
        return;
    }

    /* Store other read parameters. */
    stream->callback_ptr = callback_ptr;
    stream->client_data = client_data;
    stream->read_buffer = buffer;
    stream->read_size = size;
    stream->read_unit = unit;

    /* Begin the stream read operation. */
    stream->read_total_bytes = 0;
    stream->read_amt_samples = 0;
    stream->read_amt_bytes_set = FALSE;
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_READ;
    video_fmt_mp4r_stream_process (stream);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4r_abs_file_offset

DESCRIPTION
  This function is given to the client as a callback.  It is called in order
  to request absolute file offset of a stream sample in the media file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4r_abs_file_offset
(
  uint32                         stream_number,
  uint32                         sampleOffset,        /* in units              */
  uint32                         sampleSize,          /* in units              */
  void                           *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4r_context_type *context;
    video_fmt_mp4r_stream_type  *stream;

    /* Return an error if the format services is not currently waiting in the
    ** proper state.
    */
    context = (video_fmt_mp4r_context_type *) server_data;
    if (context->state != VIDEO_FMT_MP4R_STATE_READY)
    {
        MSG_ERROR ("read_stream: format services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }

    /* Verify that the given stream number is valid. */
    if (stream_number >= context->num_streams)
    {
        MSG_ERROR ("read_stream: invalid stream number given!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, video_fmt_mp4r_end);
        return;
    }

    /* Return an error if the stream services is not currently waiting in the
    ** proper state.
    */
    stream = &context->stream_state [stream_number];
    if (stream->state != VIDEO_FMT_MP4R_STREAM_STATE_READY)
    {
        MSG_ERROR ("read_stream: stream services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }

    /* Store other read parameters. */
    stream->callback_ptr = callback_ptr;
    stream->client_data = client_data;
    stream->read_offset = sampleOffset;
    stream->read_size = sampleSize;

    /* Begin the stream read operation. */
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_FIND_ABS_OFFSET;
    video_fmt_mp4r_stream_process (stream);

}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4r_get_sample_info

DESCRIPTION
  This function is given to the client as a callback.  It is called in order
  to request information about one or more samples in a stream in the media
  file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4r_get_sample_info
(
  uint32                      stream_number,
  uint32                      offset,
  uint32                      size,
  video_fmt_sample_info_type  *buffer,
  void                        *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4r_context_type  *context;
    video_fmt_mp4r_stream_type   *stream;
    video_fmt_stream_info_type  *stream_info;

    /* Return an error if the format services is not currently waiting in the
    ** proper state.
    */
    context = (video_fmt_mp4r_context_type *) server_data;
    if (context->state != VIDEO_FMT_MP4R_STATE_READY)
    {
        MSG_ERROR ("get_sample_info: format services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }

    /* Verify that the given stream number is valid. */
    if (stream_number >= context->num_streams)
    {
        MSG_ERROR ("get_sample_info: invalid stream number given!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, video_fmt_mp4r_end);
        return;
    }

    /* Return an error if the stream services is not currently waiting in the
    ** proper state.
    */
    stream = &context->stream_state [stream_number];
    if (stream->state != VIDEO_FMT_MP4R_STREAM_STATE_READY)
    {
        MSG_ERROR ("get_sample_info: stream services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }
    stream_info = &context->stream_info [stream_number];

    /* Put given offset and size into range. */
    offset = MIN (offset, stream_info->frames);

    if(offset < stream_info->track_frag_info.first_frame)
        offset = stream_info->track_frag_info.first_frame;
    size = MIN (size, stream_info->frames - offset);


    if( (offset >= stream->main_fragment_frames) && context->fragment_present )
    {
        /* getting samples from the fragments */
        stream->fragment_processing = TRUE;
    }
    else
        stream->fragment_processing = FALSE;


    /* Store get sample info parameters. */
    stream->callback_ptr = callback_ptr;
    stream->client_data = client_data;
    stream->get_sample_info_stream = stream_number;
    stream->get_sample_info_size = size;
    stream->get_sample_info_offset = offset;
    stream->get_sample_info_buffer = buffer;

    /* Begin the sample information gather operation. */
    stream->get_sample_info_total_samples = 0;
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_SAMPLE_INFO;
    video_fmt_mp4r_stream_process (stream);
}

/* <EJECT> */
/*===========================================================================

FUNCTION  video_fmt_mp4r_find_sync_sample

DESCRIPTION
  This function is given to the client as a callback.  It is called in order
  to request a lookup of the nearest sync sample at or before/after a given
  sample in a stream in the media file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4r_find_sync_sample (
  uint32                         stream_number,
  uint32                         offset,
  boolean                        reverse,
  video_fmt_sample_info_type     *buffer,
  void                           *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4r_context_type  *context;
    video_fmt_mp4r_stream_type   *stream;
    video_fmt_stream_info_type  *stream_info;

    /* Return an error if the format services is not currently waiting in the
    ** proper state.
    */
    context = (video_fmt_mp4r_context_type *) server_data;
    if (context->state != VIDEO_FMT_MP4R_STATE_READY)
    {
        MSG_ERROR ("find_sync_sample: format services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }

    /* Verify that the given stream number is valid. */
    if (stream_number >= context->num_streams)
    {
        MSG_ERROR ("find_sync_sample: invalid stream number given!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, video_fmt_mp4r_end);
        return;
    }

    /* Return an error if the stream services is not currently waiting in the
    ** proper state.
    */
    stream = &context->stream_state [stream_number];
    if (stream->state != VIDEO_FMT_MP4R_STREAM_STATE_READY)
    {
        MSG_ERROR ("find_sync_sample: stream services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }
    stream_info = &context->stream_info [stream_number];

    /* Put given offset into range. */
    offset = MIN (offset, stream_info->frames);


    if( (offset >= stream->main_fragment_frames) && context->fragment_present )
    {
        /* getting samples from the fragments */
        stream->fragment_processing = TRUE;
    }
    else
        stream->fragment_processing = FALSE;


    /* Store get sample info parameters. */
    stream->callback_ptr = callback_ptr;
    stream->client_data = client_data;
    stream->get_sample_info_stream = stream_number;
    stream->get_sample_info_offset = offset;
    stream->get_sample_info_buffer = buffer;
    stream->seek_reverse = reverse;
    stream->seek_start_found = FALSE;

    /* Begin the find sync sample operation. */
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_FIND_SYNC_SAMPLE;
    video_fmt_mp4r_stream_process (stream);
}

/*===========================================================================

FUNCTION  video_fmt_mp4r_find_access_point

DESCRIPTION
  This function is given to the client as a callback.  It is called in order
  to request a lookup of the nearest access point at or before/after a given
  sample in a stream in the media file.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void video_fmt_mp4r_find_access_point (
  uint32                         stream_number,
  uint32                         sample_timestamp,
  int32                          skipNumber,
  boolean                        reverse,
  video_fmt_tfra_entry_type      *buffer,
  void                           *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
)
{
    video_fmt_mp4r_context_type  *context;
    video_fmt_mp4r_stream_type   *stream;
    /* Return an error if the format services is not currently waiting in the
    ** proper state.
    */
    context = (video_fmt_mp4r_context_type *) server_data;
    if (context->state != VIDEO_FMT_MP4R_STATE_READY)
    {
        MSG_ERROR ("find_sync_sample: format services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }

    /* Verify that the given stream number is valid. */
    if (stream_number >= context->num_streams)
    {
        MSG_ERROR ("find_access_point: invalid stream number given!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_FAILURE, client_data, NULL, video_fmt_mp4r_end);
        return;
    }

    /* Return an error if the stream services is not currently waiting in the
    ** proper state.
    */
    stream = &context->stream_state [stream_number];
    if (stream->state != VIDEO_FMT_MP4R_STREAM_STATE_READY)
    {
        MSG_ERROR ("find_sync_sample: stream services not ready!", 0, 0, 0);
        callback_ptr (VIDEO_FMT_BUSY, client_data, NULL, NULL);
        return;
    }
    /* Store get sample info parameters. */
    stream->callback_ptr = callback_ptr;
    stream->get_access_point_timestamp = sample_timestamp;
    stream->get_access_point_skipnumber = skipNumber;
    stream->get_access_point_buffer = buffer;
    stream->seek_access_point_reverse = reverse;
    stream->seek_start_found = FALSE;
    stream->tfra_skip = 0;
    if(stream->tfra.table_size != context->tfra.table_size)
      stream->tfra = context->tfra;

    /* Begin the find sync sample operation. */
    stream->state = VIDEO_FMT_MP4R_STREAM_STATE_GET_ACCESS_POINT;
    video_fmt_mp4r_stream_process (stream);
}
