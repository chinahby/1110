#ifndef VIDEOFMT_MP4W_H
#define VIDEOFMT_MP4W_H
/*===========================================================================

            V I D E O   F O R M A T S   -   M P 4   W R I T E R
                  I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the internal definitions to the MP4 file
  format writer portion of the video formats services.

Copyright(c) 2002-2003 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/mmservices/mpeg4fileformat/writer/main/latest/inc/videofmt_mp4w.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/10   prs	     Increasing meta data buffer size as it helps meta data writes for long time record stop scenarios.
07/08/08   shiven  Adding support for loci atom
08/28/07   rpw     Added EVRC-B and EVRC-WB support
04/28/08   sanal   Adding length filed, Merging changes for cprt atom.
11/05/07   dhaval  Added copyright atom support
07/03/07   jk      Added 'output_unit_size' for EFS throughput control.
02/19/06   vs      Merged code for support to enable/disable audio/video truncation 
                   from 6550/4.5 branch.
12/18/06   gopi    Added support to enable/disable audio/video truncation.
06/20/06   Gopi    Added support for dynamically updating titl,descp
                   auth atoms.
                   Code is featurized in videoeng,cameraservices under
                   FEATURE_VIDEO_SETATOMS_DYNAMIC.
                   In videofmt support has been added as an additional API.
                   Also videofmt is feature free file hence didn't featurize
                   code.
03/10/06   Gopi    Added support for MFRA atom
12/09/05   rpw     Added AMR-WB and AMR-WB+ support
11/14/05   Shahed New implementation of camcorder space status error/warning msgs.
                  The new messaging does dot change the API of the videofmt.
11/11/05   Shahed Undo SPACE error/warning changes.
10/08/05   Shahed  Added a new member videofmt_space_out_type in
                   video_fmt_mp4w_context_type struct to support SPACE
                   error/warning message details (i.e. sample,chunk space out etc.
06/28/05   jk      Made every I-frame in fragments the first frame of a chunk.
05/24/05   jk      Added support for a very long clip.
05/12/05   rpw     Added movie duration limit feature.
02/17/05   jk      Removed 'mvex' in 'moov' when no 'moof' follows.
                   Fixed space check for fragmented 3g2.
02/07/05   rpw     Added still image support.
                   Added audio only support.
                   Added NTT DoCoMo DRM atom support.
                   Added capability of disabling interlacing.
                   Added file size limit feature.
                   Fixed file limit reached logic.
                   Now collapsing all 'mdat' atoms together when recording.
                   Added mode selection to select bitrate for AMR recording.
                   Replaced all TASKLOCK/TASKFREE with REX critical sections.
01/13/05   jk      Re-implemented one sample delay to fix fragmented 3g2.
12/08/04   rpw     Added ability to read/write language code
                   from 'mdhd' atom.
                   Added ability to read width/height and origin
                   information from 'tkhd' atom.
11/18/04   jk      Made functions static. Deleted video_fmt_mp4w_stream_pull.
                   Combined video_fmt_mp4w_stream_flush into
                   video_fmt_mp4w_stream_output.
11/17/04   jk      Added video_fmt_mp4w_stream_regulate.
11/10/04   jk      Added video_fmt_mp4w_stream_reset to reset stream buffer.
11/04/04   jk      Added video_fmt_mp4w_unset_chunk to fix a bug that was hit
                   when a chunk was set and it turned out it was the last
                   chunk.
11/03/04   jk      Added video_fmt_mp4w_seek to expose pointer to stream
                   buffer with its size to the client,
                   video_fmt_mp4w_write_footer to write fixed stream footer.
                   Implemented one sample delay before writing chunk out
                   to append fixed footer to the last sample.
09/08/04   rpw     Added option to force 'stsz' table generation, even for
                   streams with fixed-size samples (SKM requirement).
07/15/04   rpw(hd) Added support for writing H.264 streams.
07/14/04   rpw     Added timed text encoding support.
04/21/04   rpw     Added ability to write 'udat' atoms through the
                   video_fmt_mp4w_write_uuid funtion, used to
                   write 'uuid' atoms.
12/02/03   rpw     Added support for 3GPP2 QCELP (QCELPSampleEntry).
09/05/03   enj     Removing FEATURE_VIDEOFMT (i.e. permanently enabling it)
06/25/03   rpw     Created file, based on functionality from original
                   videoengi.h file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "rex.h"                /* Real Time Executive                     */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* This is used in 'video_fmt_mp4w_stream_output' to flush the stream
** buffer.
*/
#define VIDEO_FMT_MP4W_FLUSH_ALL            0

/* This sets the minimum amount of memory allocated for a stream buffer.  A
** minimum is required because stream buffers may be reused to accomodate
** streaming out data with minimum buffer size requirements.
*/
#define VIDEO_FMT_MP4W_MINIMUM_BUFFER_SIZE  1024

/* This sets the amount of memory allocated for the meta data buffer.
*/
#define VIDEO_FMT_MP4W_META_DATA_BUFFER_SIZE  16384//4096//32768

/* This structure represents one stream buffer.  The buffer consists of a
** chunk of memory allocated for the buffer, along with its size and two
** memory pointers marking the bounds of the current valid data in the
** buffer.  Using the two pointers, the buffer is used as a circular buffer
** where the "head" pointer marks the first byte of valid data in the buffer,
** and the "tail" pointer marks the first unused byte in the buffer.
**
** With the exposure of the stream buffer to its client were the
** following elements "end" and "linear" added.
** "end" is used to mark the end of valid data in the buffer. I.e. "end"
** is set to "tail" when "tail" wraps around at the end of the buffer and
** becomes smaller than "head". "end" is set to "size" when "head" wraps
** around at the end of the buffer and becomes smaller than "tail".
** "linear" denotes the size of free linear space from tail. "linear" is
** "size" - "tail" if "head" < "tail" and "head" - "tail" if "head" > "tail".
*/
typedef struct {
  uint8   *data;   /* pointer to the actual stream data                    */
  uint32  head;    /* offset of next unconsumed byte in data stream        */
  uint32  tail;    /* one past offset of last unconsumed byte in stream    */
  uint32  end;     /* effective size of data buffer when head > tail       */
  uint32  linear;  /* size of free linear buffer from tail                 */
  uint32  size;    /* total size of data buffer (used and unused)          */
} video_fmt_mp4w_stream_type;

/* This structure records the file location and number of frames in a single
** chunk of movie data.
*/
typedef struct {
  uint32  pos;      /* file offset of start of frames                      */
  uint32  samples;  /* number of samples in this file chunk                */
  uint32  desc;     /* index of corresponding sample description           */
} video_fmt_mp4w_chunk_table_entry_type;

/* This structure records the size of a frame or sample of movie data, along
** with any other information about the frame needed to build the meta-data
** for the movie.
*/
typedef struct {
  uint32   size;    /* size of this frame, in bytes                        */
  uint32   delta;   /* delta time to next frame                            */
  boolean  iframe;  /* TRUE if this frame was an I-Frame, FALSE otherwise  */
} video_fmt_mp4w_sample_table_entry_type;

/* This enumeration represents table type of table blocks stored in a
** temporary meta data storage.
*/
typedef enum {
  VIDEO_FMT_MP4W_SAMPLE,
  VIDEO_FMT_MP4W_CHUNK
} video_fmt_mp4w_table_type;

/* This structure records the information of table blocks stored in a
** temporary meta data storage.
*/
typedef struct {
  uint32                      id;     /* ID of the stored table block client */
                                      /* sets when storing a table block.    */
  uint32                      size;   /* size of the stored table, in bytes  */
  uint32                      items;  /* number of table elements            */
  video_fmt_mp4w_table_type   type;   /* VIDEO_FMT_MP4W_SAMPLE or            */
                                      /* VIDEO_FMT_MP4W_CHUNK                */
} video_fmt_mp4w_table_store_entry_type;

/* This structure represents a node in a linked list of atoms. */
typedef struct video_fmt_mp4w_atom_list_type_t {
  struct video_fmt_mp4w_atom_list_type_t  *next;  /* heap alloc */
  uint32                                  size;
  uint8                                   *atom;  /* heap alloc */
} video_fmt_mp4w_atom_list_type;

/* This structure contains all the state information for a stream being
** written.
*/
typedef struct video_fmt_mp4w_stream_context_type {
  /*-------------------------------------------------------------------------
  ** Media format information - these store the identifiers of the codecs
  ** used, if any.
  **-------------------------------------------------------------------------
  */
  video_fmt_stream_type          type;      /* video, audio, etc.          */
  video_fmt_stream_subinfo_type  subinfo;   /* type-specific media info    */

  /*-------------------------------------------------------------------------
  ** Stream timescale - this defines the number of units per second for this
  ** stream.
  **-------------------------------------------------------------------------
  */
  uint32  timescale;

  /*-------------------------------------------------------------------------
  ** Stream duration - this caches the total length of the stream, in units
  ** of the stream timescale, since the beginning of the current fragment.
  **-------------------------------------------------------------------------
  */
  uint32  duration;

  /*-------------------------------------------------------------------------
  ** Total stream duration - this caches the total length of the stream, in
  ** units of the stream timescale, since the very beginning of the current
  ** stream.
  **-------------------------------------------------------------------------
  */
  uint32  total_duration;

  /*-------------------------------------------------------------------------
  ** Stream interlacing
  **
  ** The "interlace_other" field is used to link a stream to another stream,
  ** for the purpose of interlacing their data.  Setting this to the index of
  ** the stream itself ensures no link is made.  Setting this to any other
  ** value indicates that the stream is interlaced with the stream having this
  ** value as the index into the movie file's list of streams.  Furthermore,
  ** the data for that other stream will not be written out to the movie file
  ** when data for that stream is written by the client.  Instead, when data
  ** for this stream is written out, data for that other stream will be
  ** written out just after it, forcing the two streams to interlace at the
  ** rate of this stream being written out.
  **
  ** The "interlaced" field indicates whether or not another track is
  ** referencing this track for interlacing.  If TRUE, data for this track is
  ** never written out to the movie file as it arrives from the client.
  ** Instead, it is written out just after data for the interlacing track is
  ** written out.
  **
  ** The "interlace_rate" field indicates the rate, in units of the stream
  ** timescale, at which chunks should be generated.
  **
  ** The "chunk_start_time" field indicates the duration of the stream the
  ** last time a chunk was generated.
  **-------------------------------------------------------------------------
  */
  uint32                    interlace_other;
  boolean                   interlaced;
  uint32                    interlace_rate;
  uint32                    chunk_start_time;

  /*-------------------------------------------------------------------------
  ** Meta-data - these define various parameters which are not needed for
  ** storing the stream data itself, but are required when filling out the
  ** meta-data for the stream's track, later when the file is closed.
  **-------------------------------------------------------------------------
  */
  video_fmt_ref_track_type  ref_track;    /* typically used for OD track */
  uint8                     priority;     /* relative stream priority */
  const char                *handler;     /* name for handler reference */
  uint8                     mode_change_restriction;  /* AMRSampleEntry */
  uint8                     frames_per_sample;  /* AMR/EVRCSampleEntry */
  int16                     tkhd_layer;   /* visual stacking order */
  uint32                    tkhd_width;   /* visual display size */
  uint32                    tkhd_height;  /* visual display size */
  uint32                    tkhd_origin_x;  /* offset from video track */
  uint32                    tkhd_origin_y;  /* offset from video track */
  uint16                    language;   /* packed ISO-639-2/T language code */

  /*-------------------------------------------------------------------------
  ** Stream characteristics - these parameters affect how samples and chunks
  ** of stream data are written out, as well as the part of the sample table
  ** which is constructed differently depending on the organization of the
  ** stream data.
  **-------------------------------------------------------------------------
  */
  uint32                    optimal_chunk_size;
  uint32                    fixed_sample_size;
  uint32                    fixed_sample_delta;
  boolean                   has_inter_frames;
  uint32                    stsc_reset_rate;

  /*-------------------------------------------------------------------------
  ** Stream size limitations - these parameters are set by the client and
  ** govern how long the stream can be, as the meta-data for the stream is
  ** kept in memory until the movie file is completely written out.
  ** 'max_samples' and 'max_chunks' are the size of samples table and chunks
  ** table on memory respectively.  'max_table_stores' governs how many times
  ** these tables on memory can be stored in a temporary meta data storage
  ** combined.
  **-------------------------------------------------------------------------
  */
  uint32                    max_header;
  uint32                    max_footer;
  uint32                    max_samples;
  uint32                    max_chunks;
  uint32                    max_samples_rate;
  uint32                    max_table_stores;

  /*-------------------------------------------------------------------------
  ** Sample/Chunk table low thresholds - these define thresholds for the
  ** number of entries free in the sample and chunk tables, under which the
  ** movie file writer sends an indication to the client that it is about to
  ** run out of those resources.
  **-------------------------------------------------------------------------
  */
  uint32                    samples_out_near_threshold;
  uint32                    chunks_out_near_threshold;
  uint32                    samples_out_imminent_threshold;
  uint32                    chunks_out_imminent_threshold;

  /*-------------------------------------------------------------------------
  ** Stream Buffer - this is a circular buffer which the client pushes data
  ** into, and the movie writer reads from when writing out stream data to the
  ** output file.
  **-------------------------------------------------------------------------
  */
  video_fmt_mp4w_stream_type  buffer;

  /*-------------------------------------------------------------------------
  ** Chunk and sample tables - these record the information needed to
  ** construct the stream meta-data once the movie recording is finished and
  ** the output movie file is being generated.
  **-------------------------------------------------------------------------
  */
  video_fmt_mp4w_sample_table_entry_type  *samples;  /* heap alloc */
  uint32                                  samples_size;
  uint32                                  samples_chunk_start;
  uint32                                  samples_num_iframes;
  uint32                                  samples_largest;
  uint32                                  last_sample_size_increase;
  uint32                                  last_sample_delta_extend;
  int32                                   samples_index;
  video_fmt_mp4w_chunk_table_entry_type   *chunks;  /* heap alloc */
  uint32                                  chunks_size;
  uint32                                  chunk_bytes;
  uint32                                  stts_size;
  uint32                                  stsc_size;
  int32                                   chunks_index;
  video_fmt_mp4w_table_store_entry_type   *table_stores;  /* heap alloc */
  uint32                                  table_stores_size;
  uint32                                  stored_samples_size;
  uint32                                  stored_samples_table;
  uint32                                  stored_chunks_size;
  uint32                                  stored_chunks_table;
  boolean                                 reset_stsc_algo;

  /*-------------------------------------------------------------------------
  ** Decoder Specific Information - this buffer is used to hold the
  ** header portion of the bitstream.  This first part of the bitstream is not
  ** stored with the rest of the bitstream.  Rather, it is encoded into the
  ** elementary stream descriptor atom in the movie meta-data, and used by the
  ** decoder for information and initialization before the bitstream is
  ** delivered to the decoder.
  **
  ** NOTE: This is not used for timed text streams.  Refer to "text" field.
  **-------------------------------------------------------------------------
  */
  uint8    *header;  /* heap alloc */
  uint32   header_size;

  /*-------------------------------------------------------------------------
  ** Bitstream specific footer - this buffer is used to hold the fixed
  ** footer of the bitstream. This footer of the bitstream is later
  ** appended to the last sample of the bitstream. When it is appended,
  ** the sample size of the last sample and the corresponding chunk size
  ** are appropriately updated.
  **-------------------------------------------------------------------------
  */
  uint8    *footer;  /* heap alloc */
  uint32   footer_size;

  /*-------------------------------------------------------------------------
  ** Timed text sample descriptions - this is a linked list of sample entry
  ** atoms, used when the stream is timed text.  This list, and not the
  ** "header" buffer, is updated when the client writes header information for
  ** a timed text stream.
  **-------------------------------------------------------------------------
  */
  uint32                         num_text;
  video_fmt_mp4w_atom_list_type  *text;  /* heap alloc */
  video_fmt_mp4w_atom_list_type  *text_end;  /* points to end of "text" */

  /*-------------------------------------------------------------------------
  ** Bitrate calculation - these record statistics about the bitstream,
  ** for use in calculating the average bitrate over the whole stream, and
  ** the maximum bitrate over a one second window of time.
  **-------------------------------------------------------------------------
  */
  uint32  bitrate_window_start; /* along w/samples_size, defines window   */
  uint32  bitrate_window_bits;  /* total bits in whole stream             */
  uint32  bitrate_total_bits;   /* total bits in current window           */
  uint32  bitrate_window_time;  /* total time in current window           */
  uint32  bitrate_max_bits;     /* window max bits, over time             */

  /*-------------------------------------------------------------------------
  ** Mode set used - this is a bitmask of AMR modes encountered, and is only
  ** used if the stream is an AMR stream.  It is needed when writing out the
  ** AMRSampleEntry atom in the stream meta-data.  To help lighten the
  ** client's burden, the file writer builds this bitmask for it.
  **-------------------------------------------------------------------------
  */
  uint16  amr_modes_used;

  /*-------------------------------------------------------------------------
  ** Match duration - if enabled, this is used to link a stream to another
  ** stream, for the purpose of matching stream duration.  If this stream ends
  ** up being shorter than the linked stream, its last sample's duration is
  ** extended to make the stream have the same duration.  Otherwise, if the
  ** stream ends up being longer than the linked stream, the last samples of
  ** the stream are not accounted for in the meta data for the stream, in
  ** order to make the stream shorter to match the duration of the linked
  ** stream.
  **-------------------------------------------------------------------------
  */
  boolean  match_duration;
  uint32   match_duration_with;
} video_fmt_mp4w_stream_context_type;

/*MFRA Structure for storing statiscal data*/
typedef __packed struct video_fmt_mpw_mfrastruct_type
{
  uint32 time;
  uint32 moof_offset;
  uint8  traf_number;
  uint8  trun_number;
  uint8  SampleNumber;
}
video_fmt_mpw_mfrastruct_type;

/*MFRA memory allocation optimization value*/
#define MEMORYSIZE (1000 * sizeof(video_fmt_mpw_mfrastruct_type))

/*MFRA Data Structure*/
typedef struct video_fmt_mpw_mfradatastruct_type
{
   uint32 TrakId;
   uint32 currentIndex;
   uint8  *pMfraData;
   struct video_fmt_mpw_mfradatastruct_type *pNext;
}
video_fmt_mpw_mfradatastruct_type;

/* This structure contains all the state information for a movie being
** written.
*/
/*shahed - typedef enum
{
   VIDEO_FMT_SPACE_MSG_INFO_UNAVAILABLE,
   VIDEO_FMT_SPACE_MSG_SAMPLE_TABLE,
   VIDEO_FMT_SPACE_MSG_CHUNK_TABLE,
   VIDEO_FMT_SPACE_MSG_STORAGE,
   VIDEO_FMT_SPACE_MSG_MOVIE_DURATION
} video_fmt_space_out_type;*/

typedef struct video_fmt_mp4w_context_type {
  /*-------------------------------------------------------------------------
  ** Movie file critical section - must be held when accessing the parts
  ** of this structure common to all streams.
  **-------------------------------------------------------------------------
  */
  rex_crit_sect_type  cs;

  /*-------------------------------------------------------------------------
  ** Fragment size - this is set when the file is opened, and if non-zero
  ** indicates that the output file should be fragmented according to the
  ** specification "Movie File Format Specification for cdma2000 1x/1xEV-DO
  ** Version 1.1.0" by KDDI.  The value specifies the number of seconds to
  ** make each movie fragment.
  **-------------------------------------------------------------------------
  */
  uint32  fragment_size;

  /*-------------------------------------------------------------------------
  ** Fragment sequence number - if generating a fragmented file, this tracks
  ** the number of fragments written, and is the number written out for the
  ** next fragment sequence_number field in the 'mfhd' atom.
  **-------------------------------------------------------------------------
  */
  uint32  fragment_sequence_number;

  /*-------------------------------------------------------------------------
  ** Endianness flag - this flag is set to TRUE on little-endian targets.
  **-------------------------------------------------------------------------
  */
  boolean  byte_swap_needed;

  /*-------------------------------------------------------------------------
  ** Callback information given by client
  **-------------------------------------------------------------------------
  */
  video_fmt_status_cb_func_type  callback_ptr;
  void                           *client_data;

  /*-------------------------------------------------------------------------
  ** Stream contexts - these contain the information and current context data
  ** for each stream being written to the movie file.
  **-------------------------------------------------------------------------
  */
  uint32                              num_streams;
  video_fmt_mp4w_stream_context_type  *streams;  /* heap alloc */

  /*-------------------------------------------------------------------------
  ** Output offset - this is the current offset into the movie file being
  ** written.
  **-------------------------------------------------------------------------
  */
  uint32  output_offset;

  /*-------------------------------------------------------------------------
  ** Movie timescale - this defines the number of units per second for this
  ** movie overall.  It is used in the 'mvhd' atom.
  **-------------------------------------------------------------------------
  */
  uint32  timescale;

  /*-------------------------------------------------------------------------
  ** Movie duration - this defines the overall length of the movie, in units
  ** of the movie timescale.  It is returned to the client when finished
  ** writing the movie.
  **-------------------------------------------------------------------------
  */
  uint32  duration;

  /*-------------------------------------------------------------------------
  ** Fragment duration - this defines the overall length of the current
  ** fragment, in units of the movie timescale.  It is used in the 'mvhd'
  ** atom.
  **-------------------------------------------------------------------------
  */
  uint32  fragment_duration;

  /*-------------------------------------------------------------------------
  ** Movie size thresholds - these are the number of seconds, before the movie
  ** is estimated to fill up all available space on the storage device, when
  ** the VIDEO_FMT_LIMIT_NEAR and VIDEO_FMT_LIMIT_NEAR warnings should be
  ** given.
  **-------------------------------------------------------------------------
  */
  uint32  movie_size_warning_near_threshold;
  uint32  movie_size_warning_imminent_threshold;

  /*-------------------------------------------------------------------------
  ** Movie size limit - if non-zero, this is the maximum file size to record.
  **-------------------------------------------------------------------------
  */
  uint32  movie_size_limit;

  /*-------------------------------------------------------------------------
  ** Movie duration limit - if non-zero, this is the maximum file duration to
  ** record.
  **-------------------------------------------------------------------------
  */
  uint32  movie_duration_limit;

  /*-------------------------------------------------------------------------
  ** Space low flags - these are set once the chunk processing determines that
  ** one of the limited resources used in caching meta data for the movie as
  ** it is being written has fallen below the thresholds given by the client
  ** at initialization time.
  **-------------------------------------------------------------------------
  */
  boolean  space_out_near;
  boolean  space_out_imminent;

  /*-------------------------------------------------------------------------
  ** Space out flag - this is set if the engine runs out of space in one of
  ** the limited resources used in caching meta data for the movie.
  **-------------------------------------------------------------------------
  */
  boolean  space_out;

  video_fmt_space_out_type space_out_status;
  /*-------------------------------------------------------------------------
  ** Timed text samples - this is a linked list of outstanding text samples
  ** that the client has allocated.  We need to keep track of them, because if
  ** we bail out on any error condition, we need to free the samples
  ** automatically, since the client will be unable to call us back to free
  ** them.
  **-------------------------------------------------------------------------
  */
  struct video_fmt_mp4w_text_sample_type_t  *text_samples;

  /*-------------------------------------------------------------------------
  ** Force 'stsz' table flag - if set to TRUE, the 'stsz' atoms of all
  ** streams will all contain tables of sample sizes, even for streams which
  ** have fixed-size samples, for which the writer would ordinarily
  ** abbreviate the 'stsz' table.  This is to conform with formats
  ** such as SKM which have this requirement.
  **-------------------------------------------------------------------------
  */
  boolean  force_stsz_table;

  /*-------------------------------------------------------------------------
  ** Buffer for meta-data - this is a buffer which the videofmt writes meta
  ** data into, and then the movie writer reads from when writing out
  ** a fragmentation or a file to the output file.
  **-------------------------------------------------------------------------
  */
  video_fmt_mp4w_stream_type  meta_data_buffer;

  /*-------------------------------------------------------------------------
  ** Media data atom information - once the first media chunk is written, the
  ** offset in the file of the atom header is stored here, and the flag that
  ** indicates the offset is valid is set.  This offset is used so the writer
  ** can go back and update the media data atom size when recording is done.
  ** Buffer for meta-data - this is a buffer which the videofmt writes meta
  ** data into, and then the movie writer reads from when writing out
  ** a fragmentation or a file to the output file.
  **-------------------------------------------------------------------------
  */
  boolean  mdat_written;
  uint32   mdat_offset;

  /*-------------------------------------------------------------------------
  ** NTT DoCoMo Digital Rights Management (DRM) - setting "include_drm" will
  ** cause 'drm ' and 'dcmd' atoms to be generated in the meta-data.  The
  ** 'dcmd' atom will contain the given distribution rights flags.
  **-------------------------------------------------------------------------
  */
  boolean  include_drm;
  uint16   drm_distribution_rights;  /* see MobileMP4 spec */

  /*-------------------------------------------------------------------------
  ** User data - setting "include_user_data" will cause a 'udta' atom to be
  ** generated in the meta-data, just after the last 'trak' atom.  The 'udta'
  ** atom will contain subatoms, each containing a different string of user
  ** data, such as the movie title, author, description and copyright.
  **-------------------------------------------------------------------------
  */
  boolean                   include_user_data;
  video_fmt_user_data_type  user_data;
  
  /*-------------------------------------------------------------------------
  ** MFRA structure related elements.
  ** pHead - points to head of linked list.
  ** pCurrent - points to current node of linked list.
  ** MfraAllocSuccess - If there is no enough space in RAM then we don't record
  ** MFRA stats.
  ** MfraEntryCount - Keeps track of the number of entries in the MFRA atom.
  **-------------------------------------------------------------------------
  */
  video_fmt_mpw_mfradatastruct_type       *pHead;
  video_fmt_mpw_mfradatastruct_type       *pCurrent;
  boolean                                 MfraAllocSuccess;
  uint32                                  MfraEntryCount;
  boolean                                 context_deleted;  
  uint32                                  last_chunk_size;
  boolean                                 update_file_data;

  /*-------------------------------------------------------------------------
  **  enable_fixupdata - If set to true with truncate final video clip based
  **  on minimum duration of audio or video stream.
  **-------------------------------------------------------------------------
  */
   boolean                                 enable_fixupdata;

  /*-------------------------------------------------------------------------
  **  output_unit_size - If non-zero, indicates the maximum data size in bytes
  **  that videoFMT outputs per VIDEO_FMT_PUT_DATA callback and updates stream
  **  buffer pointers. This parameter affects the EFS write throughput.
  **-------------------------------------------------------------------------
  */
  uint32                                    output_unit_size;
} video_fmt_mp4w_context_type;

/* This enumerated type identifies the different kinds of sample entry atom
** which may be used in the meta data for a stream in an MP4 file.
*/
typedef enum sample_entry_type_t {
  SAMPLE_ENTRY_MP4V,
  SAMPLE_ENTRY_MP4A,
  SAMPLE_ENTRY_MP4S,
  SAMPLE_ENTRY_SAMR,
  SAMPLE_ENTRY_SAWB,
  SAMPLE_ENTRY_SAWP,
  SAMPLE_ENTRY_SEVC,
  SAMPLE_ENTRY_SECB,
  SAMPLE_ENTRY_SECW,
  SAMPLE_ENTRY_SQCP,
  SAMPLE_ENTRY_S263,
  SAMPLE_ENTRY_AVC1,
  SAMPLE_ENTRY_TX3G
} sample_entry_type_t;

/* This structure represents a timed text sample, consisting of a string with
** a linked list of modifier atoms.  Instances of this structure are created
** on the heap in the video_fmt_mp4w_alloc_text function, and freed in the
** video_fmt_mp4w_write_stream function.
*/
typedef struct video_fmt_mp4w_text_sample_type_t {
  struct video_fmt_mp4w_text_sample_type_t  *next;
  video_fmt_mp4w_context_type    *context;
  char                           *text_string;    /* heap alloc */
  uint16                         text_length;
  video_fmt_mp4w_atom_list_type  *modifiers;      /* heap alloc */
  video_fmt_mp4w_atom_list_type  *modifiers_end;  /* points to end of list */
} video_fmt_mp4w_text_sample_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
** VIDEOFMT_MP4W Private Function Prototypes
**---------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION video_fmt_mp4w_write_stream

DESCRIPTION
  This procedure is used to provide the movie file writer with more stream
  data for the movie being written.  Depending on various parameters and the
  current state of the stream, data may or may not be written out to the movie
  file from this stream and possibly other streams with which it may be
  interlaced.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void video_fmt_mp4w_write_stream (
  uint32                            stream_number,
  uint32                            num_samples,
  const video_fmt_sample_info_type  *sample_info,
  const uint8                       *sample_data,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_write_text

DESCRIPTION
  This procedure is used to provide the movie file writer with information for
  constructing and writing a text sample to write to a text stream.  Depending
  on various parameters and the current state of the stream, data may or may
  not be written out to the movie file from this stream and possibly other
  streams with which it may be interlaced.

  The given text object is released by calling this function and should not be
  referenced again.  To release the text object without writing it, call
  video_fmt_mp4w_free_text instead.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  The text sample structure and any linked modifier atoms are automatically
  deallocated after they are written to the stream.

===========================================================================*/
static void video_fmt_mp4w_write_text (
  uint32                            stream_number,
  void                              *handle,    /* allocated sample handle  */
  uint32                            delta,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_write_header

DESCRIPTION
  This procedure is used to provide the movie file writer with more stream
  header data for the movie being written.  The data is held in a
  heap-allocated buffer until the meta-data for the stream is written out.
  Note that only streams having the capability of storing decoder specific
  information in the meta-data (currently only streams having 'esds' atoms)
  may have header information.

  For text streams, the header data given should be the entire contents of a
  'tx3g' atom.  To generate a 'tx3g' atom from a video_fmt_text_type
  structure, do not use this function.  Use video_fmt_mp4w_write_header_text
  instead.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  If the header is for a timed text track, any samples written to
  the stream after this point are associated with the new sample entry.

===========================================================================*/
static void video_fmt_mp4w_write_header (
  uint32                            stream_number,
  uint32                            num_bytes,
  const uint8                       *header_data,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_write_header_text

DESCRIPTION
  This procedure is used to generate a new text sample entry ('tx3g') atom for
  a text stream.  The 'tx3g' atom is constructed based on the given header
  data structure, and held in a heap-allocated buffer until the meta-data for
  the stream is written out.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A 'tx3g' sample entry atom is constructed on the heap and linked to the
  stream.  Any samples written to the stream after this point are associated
  with the new sample entry.

===========================================================================*/
static void video_fmt_mp4w_write_header_text (
  uint32                              stream_number,
  const struct video_fmt_text_type_t  *text,
  void                                *server_data,
  video_fmt_status_cb_func_type       callback_ptr,
  void                                *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_write_footer

DESCRIPTION
  This procedure is used to provide the movie file writer with more stream
  footer data for the movie being written.  The data is held in a
  heap-allocated buffer until the last sample for the stream is written out.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void video_fmt_mp4w_write_footer (
  uint32                            stream_number,
  uint32                            num_bytes,
  const uint8                       *footer_data,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_write_uuid

DESCRIPTION
  This procedure is used to pass along user data atom information to be
  written to the movie file.  The data is written in the form of a 'uuid' atom
  with the UUID given to this function, or a 'udat' atom if no UUID is given
  (uuid = NULL).

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void video_fmt_mp4w_write_uuid (
  const uint8                    *uuid,  /* for 'uuid', 16-byte array      */
                                         /* for 'udat', set to NULL        */
  const void                     *data,  /* data for atom contents         */
  uint32                         size,   /* size of atom contents          */
  void                           *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_seek

DESCRIPTION
  This procedure is used to provide the the client with the buffer where the
  client can directly puts data.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  Pointer where more data can be put.
  NULL in error condition

SIDE EFFECTS

===========================================================================*/
static uint8* video_fmt_mp4w_seek (
  uint32                            stream_number,
  uint32                            offset,
  boolean                           bLarge,
  uint32                            *size,
  void                              *server_data,
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_alloc_text

DESCRIPTION
  This procedure is used to construct a new sample for a timed
  text stream.  It assembles a sample containing just the given text string,
  with no modifiers.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  A handle to the newly created text sample is returned.  Pass this handle to
  the video_fmt_mp4w_modify_*_text functions to apply modifiers to the
  sample.  Pass the handle to video_fmt_mp4w_write_text to write the sample
  to the track and deallocate the sample.  Otherwise, pass the handle to
  video_fmt_mp4w_free_text to dispose of the sample without writing it.

SIDE EFFECTS
  None

===========================================================================*/
static void *video_fmt_mp4w_alloc_text (
  const char                     *data,  /* string to put into sample    */
  uint16                         size,   /* size of data string in bytes */
  void                           *server_data,
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_free_text

DESCRIPTION
  This procedure is used to release a timed text stream sample without writing
  it to the stream.  If the sample is written to the
  video_fmt_mp4w_write_text function, do not pass it to this function, as
  the video_fmt_mp4w_write_text function automatically releases it after it
  is written.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  Any text modifiers allocated and linked to the text sample are deallocated.

===========================================================================*/
static void video_fmt_mp4w_free_text (
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_style_text

DESCRIPTION
  This procedure is used to apply a style modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_style_text (
  uint16                           num_entries,
  const video_fmt_text_style_type  *entries,
  void                             *handle,    /* allocated sample handle   */
  video_fmt_status_cb_func_type    callback_ptr,
  void                             *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_hl_text

DESCRIPTION
  This procedure is used to apply a highlight modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_hl_text (
  uint16                         start_char, /* starting offset in string   */
  uint16                         end_char,   /* ending offset in string     */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_hl_color_text

DESCRIPTION
  This procedure is used to apply a highlight color modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_hl_color_text (
  uint32                         color,      /* 0xrrggbbaa where rr=red,    */
                                             /* gg=green, bb=blue, aa=alpha */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_karaoke_text

DESCRIPTION
  This procedure is used to apply a dynamic highlighting (karaoke) modifier to
  a text sample constructed with the video_fmt_mp4w_alloc_text function,
  before writing it to the track with the video_fmt_mp4w_write_text
  function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_karaoke_text (
  uint32                            start_time,
  uint16                            num_entries,
  const video_fmt_karaoke_box_type  *entries,
  void                              *handle,    /* allocated sample handle  */
  video_fmt_status_cb_func_type     callback_ptr,
  void                              *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_scroll_text

DESCRIPTION
  This procedure is used to apply a scroll delay modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_scroll_text (
  uint32                         scroll_delay,
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_link_text

DESCRIPTION
  This procedure is used to apply a hyperlink modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_link_text (
  uint16                         start_char, /* starting offset in string   */
  uint16                         end_char,   /* ending offset in string     */
  const char                     *url,       /* URL for hyperlink           */
  uint8                          url_length, /* length of URL in bytes      */
  const char                     *alt,       /* alternate text for link     */
  uint8                          alt_length, /* length of alt text in bytes */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_box_text

DESCRIPTION
  This procedure is used to apply a text box modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_box_text (
  int16                          left,
  int16                          top,
  int16                          right,
  int16                          bottom,
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_blink_text

DESCRIPTION
  This procedure is used to apply a blink modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_blink_text (
  uint16                         start_char, /* starting offset in string   */
  uint16                         end_char,   /* ending offset in string     */
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_wrap_text

DESCRIPTION
  This procedure is used to apply a text wrap modifier to a text sample
  constructed with the video_fmt_mp4w_alloc_text function, before writing it
  to the track with the video_fmt_mp4w_write_text function.

DEPENDENCIES
  video_fmt_mp4w_alloc_text must be called first.

RETURN VALUE
  None

SIDE EFFECTS
  A modifier atom is allocated on the heap and linked to the text sample.

===========================================================================*/
static void video_fmt_mp4w_modify_wrap_text (
  boolean                        automatic,
  void                           *handle,    /* allocated sample handle     */
  video_fmt_status_cb_func_type  callback_ptr,
  void                           *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_modify_user_atoms

DESCRIPTION
 Client can dynamically set auth, titl, decsp and cprt atoms during 
 recording session. 
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  updates the titl,auth,descp,cprt atoms

===========================================================================*/

static void video_fmt_mp4w_modify_user_atoms(
  video_fmt_user_data_type             *puser_data,
  void                                 *handle,
  video_fmt_status_cb_func_type        callback_ptr,
  void                                 *client_data
);

/*===========================================================================

FUNCTION  video_fmt_mp4w_end

DESCRIPTION
  This function terminates the current MP4 file format session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void video_fmt_mp4w_end
(
  void *server_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_update_meta_data

DESCRIPTION
  This procedure is called after data is written to a stream, in order to
  update the stream's meta data structures in memory.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to store meta data table.

SIDE EFFECTS
  None

===========================================================================*/
static boolean video_fmt_mp4w_update_meta_data (
  video_fmt_mp4w_context_type       *context,
  uint32                            stream_number,
  const video_fmt_sample_info_type  *sample_info
);

/*===========================================================================

FUNCTION video_fmt_mp4w_sample_processing

DESCRIPTION
  This procedure is called after data is written to a stream, and before
  the stream's meta data structures in memory are updated.

DEPENDENCIES
  video_fmt_mp4w_create must be called first.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None

===========================================================================*/
static boolean video_fmt_mp4w_sample_processing (
  video_fmt_mp4w_context_type       *context,
  uint32                            stream_number,
  boolean                           write_chunk,
  boolean                           limit_reached
);

/*===========================================================================

FUNCTION video_fmt_mp4w_new_text_modifier_atom

DESCRIPTION
  This function allocates memory to hold a new timed text sample modifier
  atom, and links the new atom to the list of modifier atoms attached to the
  given text sample.

DEPENDENCIES
  None.

RETURN VALUE
  If NULL is returned, memory allocation was unsuccessful.  Otherwise, the
  return value points to the memory allocated for the atom's contents.

SIDE EFFECTS
  The newly allocated text sample modifier atom is added to the end of the
  linked list of modifier atoms for the given text sample.

===========================================================================*/
static uint8 *video_fmt_mp4w_new_text_modifier_atom (
  video_fmt_mp4w_context_type      *context,
  video_fmt_mp4w_text_sample_type  *sample,
  const char                       *atom_name,
  uint32                           atom_size
);

/*===========================================================================

FUNCTION video_fmt_mp4w_chunk_processing

DESCRIPTION
  This function determines whether or not the engine should output the next
  chunk of data, and if so, outputs the next chunk.

DEPENDENCIES
  None.

RETURN VALUE
  The value returned indicates the result of the processing:
  VIDEO_FMT_DONE - chunk processed successfully
  VIDEO_FMT_FAILURE - indicates critical failure in writing the output file
  VIDEO_FMT_LIMIT_NEAR - chunk processed successfully, but resource limit near
  VIDEO_FMT_LIMIT_REACHED - failed to process chunk, but file is salvageable

SIDE EFFECTS
  None.

===========================================================================*/
static video_fmt_status_type video_fmt_mp4w_chunk_processing (
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number,
  boolean                      write_chunk,
  boolean                      last_chunk
);

/*===========================================================================

FUNCTION video_fmt_mp4w_write_chunk

DESCRIPTION
  This function outputs the current chunk of data for a stream, and sets up to
  accept more incoming data for the next chunk of the stream.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_write_chunk (
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number
);

/*===========================================================================

FUNCTION video_fmt_mp4w_append_footer

DESCRIPTION
  This function appends footer to the last sample and fixes its sample size.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_append_footer (
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number
);

/*===========================================================================

FUNCTION video_fmt_mp4w_generate_meta_data

DESCRIPTION
  This function finishes writing the main fragment of the MP4 output file by
  generating and writing all the meta-data streams and atoms that go in the
  main fragment.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_generate_meta_data
(
  video_fmt_mp4w_context_type  *context,
  boolean                      more_fragments
);

/*===========================================================================

FUNCTION video_fmt_mp4w_generate_fragment

DESCRIPTION
  This function finishes writing the next fragment (not the main fragment) of
  the MP4 output file by generating and writing all the meta-data streams and
  atoms that go in the fragment.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to write data to the
  output destination.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_generate_fragment
(
  video_fmt_mp4w_context_type  *context
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_alloc

DESCRIPTION
  This function allocates memory for a stream buffer and initializes the
  stream.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the stream was allocated successfully.  Otherwise, FALSE
  is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_stream_alloc (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream,
  uint32                       size
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_free

DESCRIPTION
  This function frees the memory for a stream buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_stream_free (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_amt_full

DESCRIPTION
  This function calculates the number of unconsumed bytes in the stream.

DEPENDENCIES
  None.

RETURN VALUE
  The number of unconsumed bytes of data in the stream is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 video_fmt_mp4w_stream_amt_full (
  video_fmt_mp4w_stream_type  *stream
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_amt_left

DESCRIPTION
  This function calculates the number of bytes more data can fit in the
  stream.

DEPENDENCIES
  None.

RETURN VALUE
  The number of bytes of usable free space in the stream is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 video_fmt_mp4w_stream_amt_left (
  video_fmt_mp4w_stream_type  *stream
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_seek

DESCRIPTION
  This function advances stream->tail by offset and returns stream->tail
  with the number of bytes more data can linearly fit in the stream without
  wrapping around.
  Input argument bWrap set to TRUE lets stream->tail wrap around.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer where more data can be put.
  NULL in error condition

SIDE EFFECTS
  None.

===========================================================================*/
static uint8* video_fmt_mp4w_stream_seek (
  video_fmt_mp4w_stream_type  *stream,
  uint32                      offset,
  boolean                     bWrap,
  uint32                      *size
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_push

DESCRIPTION
  This function pushes data into a stream buffer.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was pushed successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_stream_push (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream,
  const uint8                  *input,
  uint32                       len
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_push_reverse

DESCRIPTION
  This function pushes data into a stream buffer, but in reverse order.  In
  other words, the data is put in front of any previous data in the stream.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_stream_push_reverse (
  video_fmt_mp4w_stream_type  *stream,
  const uint8                 *input,
  uint32                      len
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_truncate

DESCRIPTION
  This function changes "stream->tail" and forces the data amount in the
  stream buffer to be equal to "data_size"

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_stream_truncate (
  video_fmt_mp4w_stream_type   *stream,
  uint32                       data_size
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_reset

DESCRIPTION
  This function resets the given output buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_stream_reset (
  video_fmt_mp4w_stream_type   *stream
);

/*===========================================================================

FUNCTION video_fmt_mp4w_stream_output

DESCRIPTION
  This function outputs given number of bytes in the given output buffer.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was output successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_stream_output (
  video_fmt_mp4w_context_type  *context,
  video_fmt_mp4w_stream_type   *stream,
  uint32                       len
);

/*===========================================================================

FUNCTION  video_fmt_mp4w_write_movie

DESCRIPTION
  This function is called to write a block of data out to the movie file.

DEPENDENCIES
  None

RETURN VALUE
  The number of bytes actually written out is returned.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 video_fmt_mp4w_write_movie
(
  video_fmt_mp4w_context_type  *context,
  const uint8                  *buffer,
  uint32                       size
);

/*===========================================================================

FUNCTION  video_fmt_mp4w_do_limit_reached_callback

DESCRIPTION
  This function sends a VIDEO_FMT_LIMIT_REACHED callback to the client.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void video_fmt_mp4w_do_limit_reached_callback
(
  video_fmt_mp4w_context_type  *context
);

/*===========================================================================

FUNCTION  video_fmt_mp4w_failure

DESCRIPTION
  This function is called whenever there is a critical failure in the video
  format services, in order to free any resources that the services had
  allocated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void video_fmt_mp4w_failure
(
  video_fmt_mp4w_context_type  *context,
  boolean                      discard_output
);

/*===========================================================================

FUNCTION  video_fmt_mp4w_release

DESCRIPTION
  This function frees all memory used for the state structures and context
  information of the video format services movie writer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void video_fmt_mp4w_release
(
  video_fmt_mp4w_context_type  *context
);

/*===========================================================================

FUNCTION video_fmt_mp4w_close

DESCRIPTION
  This function finishes up and closes the movie file being written.

DEPENDENCIES
  None.

RETURN VALUE
  VIDEO_FMT_SUCCESS is returned if the last bit of the file was written
  successfully.  Otherwise, VIDEO_FMT_FAILURE is returned, indicating that the
  file is probably corrupted.

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
static video_fmt_status_type video_fmt_mp4w_close
(
  video_fmt_mp4w_context_type  *context
);

/*===========================================================================

FUNCTION video_fmt_mp4w_convert_time_units

DESCRIPTION
  This function converts the given time units from one timescale to another.

DEPENDENCIES
  None.

RETURN VALUE
  The given time units are converted from one timescale to another, and
  returned.

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 video_fmt_mp4w_convert_time_units
(
  uint32 units,
  uint32 from,
  uint32 to
);

/*===========================================================================

FUNCTION video_fmt_mp4w_copy_language_coded_string

DESCRIPTION
  This function makes a copy of a language coded string.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the string was copied successfully.
  Otherwise, FALSE is returned.

SIDE EFFECTS
  The target language coded string is constructed.

===========================================================================*/
static boolean video_fmt_mp4w_copy_language_coded_string
(
  video_fmt_language_coded_string_type        *target,
  const video_fmt_language_coded_string_type  *source,
  video_fmt_status_cb_func_type               callback_ptr,
  void                                        *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_copy_location

DESCRIPTION
  This function makes a copy of a location

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the location was copied successfully.
  Otherwise, FALSE is returned.

SIDE EFFECTS
  The target location is constructed.

===========================================================================*/
static boolean video_fmt_mp4w_copy_location
(
  video_fmt_location_type                     *target,
  const video_fmt_location_type               *source,
  video_fmt_status_cb_func_type               callback_ptr,
  void                                        *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_free_language_coded_string

DESCRIPTION
  This function frees memory used in a language coded string.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The language coded string is freed.

===========================================================================*/
static void video_fmt_mp4w_free_language_coded_string
(
  video_fmt_language_coded_string_type        *target,
  video_fmt_status_cb_func_type               callback_ptr,
  void                                        *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_free_loci

DESCRIPTION
  This function frees memory used in loci atom

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The location atom is freed.

===========================================================================*/

static void video_fmt_mp4w_free_loci
(
  video_fmt_location_type                     *target,
  video_fmt_status_cb_func_type               callback_ptr,
  void                                        *client_data
);

/*===========================================================================

FUNCTION video_fmt_mp4w_encode_language_coded_string_atom

DESCRIPTION
  This function encodes and writes out an atom to contain the given language
  coded string.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is returned if the string was copied successfully.
  Otherwise, FALSE is returned.

SIDE EFFECTS
  The language coded string is written out to the given stream buffer.

===========================================================================*/
static boolean video_fmt_mp4w_encode_language_coded_string_atom
(
  video_fmt_mp4w_context_type                 *context,
  video_fmt_mp4w_stream_type                  *buffer,
  const video_fmt_language_coded_string_type  *source,
  const char                                  *atom
);

/*===========================================================================

FUNCTION video_fmt_mp4w_encode_loci_atom 

DESCRIPTION
  This function encodes & writes out loci atom 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The location atom is freed.

===========================================================================*/
static boolean video_fmt_mp4w_encode_loci_atom
(
  video_fmt_mp4w_context_type                 *context,
  video_fmt_mp4w_stream_type                  *buffer,
  const video_fmt_location_type               *source,
  const char                                  *atom
);

/*===========================================================================

FUNCTION video_fmt_mp4w_wcscpy

DESCRIPTION
  This function makes a copy of a NULL-terminated wide string.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The destination string receives a copy of the source string.

===========================================================================*/
static void video_fmt_mp4w_wcscpy
(
  short *dst,
  const short *src,
  const uint32 length
);

/*===========================================================================

FUNCTION video_fmt_mp4w_put_meta_table

DESCRIPTION
  This function stores meta data table (chunks table or samples table).

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to store meta data
  table.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_put_meta_table
(
  video_fmt_mp4w_context_type  *context,
  uint32                       stream_number,
  video_fmt_mp4w_table_type    type
);

/*===========================================================================

FUNCTION video_fmt_mp4w_get_meta_table

DESCRIPTION
  This function reads in meta data table (chunks table or samples table).

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned normally to indicate successful processing.  If
  the value FALSE is returned, it indicates a failure to read in meta data
  table.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean video_fmt_mp4w_get_meta_table
(
  video_fmt_mp4w_context_type           *context,
  uint32                                stream_number,
  video_fmt_mp4w_table_store_entry_type *stored
);


/*===========================================================================

FUNCTION video_fmt_mp4w_addmfra_node

DESCRIPTION
  This function copies the new MFRA values into memory.
  Later these values are retrieved and written.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_addmfra_node
(
    video_fmt_mpw_mfrastruct_type *pValues,
    video_fmt_mp4w_context_type  *context
);
/*===========================================================================

FUNCTION video_fmt_mp4w_writemfra_tofile

DESCRIPTION
  This function copies the MFRA values from Memory into the temparory file.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_writemfra_tofile
(
    video_fmt_mp4w_context_type  *context
);

/*===========================================================================

FUNCTION video_fmt_mp4w_freemfra_datastruct

DESCRIPTION
  This function frees the datastruct of MFRA.

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.

===========================================================================*/
static void video_fmt_mp4w_freemfra_datastruct
(
    video_fmt_mp4w_context_type  *context
);

#endif /* VIDEOFMT_MP4W_H */
