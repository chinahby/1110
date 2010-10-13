#ifndef VIDEOENGI_MACRO_H
#define VIDEOENGI_MACRO_H
/*===========================================================================

                  V I D E O   E N C O D E   E N G I N E
                 I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains internal definitions to calculate initialization
  parameters on the fly for the video encode engine module.

Copyright(c) 2003-2005 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/videoengi_macro.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/08   shiven  Max bit rate not working when EFS done
09/03/07   Yogi    Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
03/28/07   gopi    Fixed SKM brand recording problems with AMR audio format.
03/12/07   ali     changed the value of VIDEO_ENG_VIDEO_STREAM_BUF_SIZE_FACTOR
02/09/07   ali     Changes for dynamic RC based on feedback from EFS
10/30/06   dhaval  Removed usage of VIDEO_ENG_LIMIT_NEAR_THRESHOLD and
                   VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD.
04/05/06   jdas	   Removed lint error
03/08/06   Nagesh  Video Async Buffer size set based on the Video Bit Rate
01/04/06   ali     Changed default values to 8 instead of 0 in case of
                   audio stream is set to VIDEO_FMT_STREAM_AUDIO_NONE
                   or video stream is set to VIDEO_FMT_STREAM_VIDEO_NONE
07/18/05   jk      Changed input argument to macro functions.
06/28/05   jk      Changed VIDEO_ENG_MAX_CHUNKS as per videofmt that make
                   every I-frame in fragments the first frame of a chunk.
06/14/05   jk      Added support for the AAC UI.
05/31/05   jk      Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */

#ifdef FEATURE_VIDEO_ENCODE

#include "video_common.h"
#include "videoengi_cust.h"     /* Customer specific engine definitions    */
#include "videoengi.h"          /* engine internal typedefs and prototypes */

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifndef FEATURE_VIDEO_ENG_FIXED_PARAMETERS

/***************************************************************************/
/* First hand definitions                                                  */
/***************************************************************************/

/* This is a factor in [%] that determines the margine of allocated video
** stream buffer and how larger it will be than the actual video chunk size.
*/
#define VIDEO_ENG_VIDEO_STREAM_BUF_SIZE_FACTOR    2

/* This is a factor in [%] that determines the margine of allocated audio
** stream buffer and how larger it will be than the actual audio chunk size.
** This is set larger than video because audio data flows in in burst mode
** and depends on video for data output.
*/
#define VIDEO_ENG_AUDIO_STREAM_BUF_SIZE_FACTOR    300

/* This is a factor in [%] that determines the margine of allocated buffer to
** hold one fragment and how larger it will be than the actual fragment size.
*/
#define VIDEO_ENG_FRAGMENT_BUF_SIZE_FACTOR        120

/* This is a factor in [%] that determines the margine of the size of samples/
** chunks table to store meta-data for one fragment and how larger they will 
** be than the actual number of samples/chunks in a fragment.
*/
#define VIDEO_ENG_FRAGMENT_TABLE_SIZE_FACTOR      110

/* These are default samples/chunks table size.
*/
#define VIDEO_ENG_DEFAULT_SAMPLES_TABLE_SIZE      600
#define VIDEO_ENG_DEFAULT_CHUNKS_TABLE_SIZE       600

/* This tells if video stream and audio stream is interlaced and is TRUE if
** both video stream and audio stream are present and FALSE otherwise.
*/
#define VIDEO_ENG_VIDEO_AUDIO_INTERLACED(g_context)                 \
    (                                                               \
        (g_context->video_format != VIDEO_FMT_STREAM_VIDEO_NONE)    \
        && (g_context->audio_format != VIDEO_FMT_STREAM_AUDIO_NONE) \
        && (!g_context->no_interlace)                               \
    )

/* This tells video bitrate.
*/
#define VIDEO_ENG_GET_VIDEO_BITRATE(g_context)                    \
    (                                                             \
        (g_context->video_format == VIDEO_FMT_STREAM_VIDEO_NONE)  \
        ? (8)                                                     \
        : g_context->video_target_bitrate                         \
    )

/* This tells audio bitrate whether vocoder or audio.
*/
#define VIDEO_ENG_GET_AUDIO_BITRATE(g_context)                    \
    (                                                             \
        (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_NONE)  \
        ? (8)                                                     \
        : g_context->audio_target_bitrate                         \
    )

/* This tells stream bitrate including both video and audio.
*/
#define VIDEO_ENG_GET_STREAM_BITRATE(g_context)                 \
    (                                                           \
        VIDEO_ENG_GET_VIDEO_BITRATE (g_context)                 \
        + VIDEO_ENG_GET_AUDIO_BITRATE (g_context)               \
    )

/* This is the size of samples table that video format services will allocate
** on memory for video.  This should be able to accomodate
** VIDEO_ENG_LIMIT_NEAR_THRESHOLD.
*/
#define VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE(g_context)       \
    (                                                           \
        MAX (VIDEO_ENG_DEFAULT_SAMPLES_TABLE_SIZE,              \
            g_context->space_limit_threshold                    \
            * g_context->video_time_resolution                  \
            / g_context->video_time_increment + 1)              \
    )

/* This is the size of chunks table that video format services will allocate
** on memory for video.  This should be able to accomodate
** VIDEO_ENG_LIMIT_NEAR_THRESHOLD.
*/
#define VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE                   \
    (  /*lint -e{506} constant value boolean*/                  \
        MAX (VIDEO_ENG_DEFAULT_CHUNKS_TABLE_SIZE,               \
            (g_context->space_limit_threshold * 1000)           \
            / VIDEO_ENG_DESIRED_INTERLACE_RATE + 1)             \
    )

/* This is the size of samples table that video format services will allocate
** on memory for audio.  This should be able to accomodate
** VIDEO_ENG_LIMIT_NEAR_THRESHOLD.
*/
#define VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE(g_context)       \
    (                                                           \
        MAX (VIDEO_ENG_DEFAULT_SAMPLES_TABLE_SIZE,              \
            g_context->space_limit_threshold                    \
            * g_context->audio_channels                         \
            * g_context->audio_sampling_frequency               \
            / g_context->audio_samples_per_frame                \
            + 1)                                                \
    )

/* This is the size of chunks table that video format services will allocate
** on memory for audio.  This should be able to accomodate
** VIDEO_ENG_LIMIT_NEAR_THRESHOLD.
*/
#define VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE         \
    VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE

/* This tells the video chunk size in bytes.
*/
#define VIDEO_ENG_GET_VIDEO_CHUNK_SIZE(g_context)       \
    (                                                   \
        (VIDEO_ENG_GET_VIDEO_BITRATE (g_context) / 8)   \
        * VIDEO_ENG_OPTIMAL_CHUNK_LENGTH                \
    )

/* This tells the audio chunk size in bytes.
*/
#define VIDEO_ENG_GET_AUDIO_CHUNK_SIZE(g_context)       \
    (                                                   \
        (VIDEO_ENG_GET_AUDIO_BITRATE (g_context) / 8)   \
        * VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH          \
    )

/* This tells actual video chunk size in bytes.  If audio stream is not 
** interlaced to video stream, VIDEO_ENG_OPTIMAL_CHUNK_LENGTH will determine
** the actual video chunk size.  Otherwise VIDEO_ENG_OPTIMAL_CHUNK_LENGTH or
** interlace rate will determine the actual chunk size.
*/
#define VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE(g_context)      \
    (                                                         \
        VIDEO_ENG_VIDEO_AUDIO_INTERLACED (g_context)          \
        ? MIN (VIDEO_ENG_GET_VIDEO_CHUNK_SIZE (g_context),    \
                (VIDEO_ENG_GET_VIDEO_BITRATE (g_context) / 8) \
                * VIDEO_ENG_DESIRED_INTERLACE_RATE / 1000)    \
        : VIDEO_ENG_GET_VIDEO_CHUNK_SIZE (g_context)          \
    )

/* This tells actual audio chunk size in bytes.  If audio stream is not 
** interlaced to video stream, VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH will  
** determine the actual audio chunk size.  Otherwise 
** VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_LENGTH or interlace rate will determine
** the actual chunk size.
*/
#define VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE(g_context)      \
    (                                                         \
        VIDEO_ENG_VIDEO_AUDIO_INTERLACED (g_context)          \
        ? MIN (VIDEO_ENG_GET_AUDIO_CHUNK_SIZE (g_context),    \
                (VIDEO_ENG_GET_AUDIO_BITRATE (g_context) / 8) \
                * VIDEO_ENG_DESIRED_INTERLACE_RATE / 1000)    \
        : VIDEO_ENG_GET_AUDIO_CHUNK_SIZE (g_context)          \
    )

/* This tells the fragment size in bytes.
*/
#define VIDEO_ENG_GET_FRAGMENT_BUF_SIZE(g_context)        \
    (                                                     \
        (VIDEO_ENG_GET_STREAM_BITRATE(g_context) / 8)     \
        * VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE                \
        * VIDEO_ENG_FRAGMENT_BUF_SIZE_FACTOR / 100        \
    )

/* This tells the maximum number of video frames in the stream.
*/
#define VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM(g_context)    \
    (                                                           \
        VIDEO_ENG_MAX_STREAM_LENGTH                             \
        * g_context->video_time_resolution                      \
        / g_context->video_time_increment                       \
    )

/* This tells the maximum video frames in a fragment.
*/
#define VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT(g_context)  \
    (                                                           \
        ((( VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE                    \
          + g_context->space_limit_threshold )                  \
        * g_context->video_time_resolution)                     \
        / g_context->video_time_increment)                      \
        * VIDEO_ENG_FRAGMENT_TABLE_SIZE_FACTOR / 100            \
    )

/* This tells how many video samples table blocks at least should be stored
** to record VIDEO_ENG_MAX_STREAM_LENGTH seconds of video.
*/
#define VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES(g_context)       \
    (                                                           \
        VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM (g_context)   \
        / VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE (g_context)    \
    )

/* This tells the maximum number of video chunks in the stream.
*/
#define VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM(g_context)     \
    (                                                           \
        VIDEO_ENG_MAX_STREAM_LENGTH                             \
        * (VIDEO_ENG_GET_VIDEO_BITRATE (g_context) / 8)         \
        / VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE (g_context)     \
    )

/* This tells the video chunks table size that can store the maximum number 
** of video chunks in a fragment and won't meet the chunks table limit near
** condition.  The chunks table in memory should be large enough to store
** all the chunks of a fragment and still not to meet the chunks table limit
** near condition when fragmentation is enabled.
** Video Format Services starts a new chunk whenever it meets with an I-frame
** in fragments except for the first fragment.  The size of the chunk table
** Video Format Services will need for a fragment varies as below.
**
** (1) I-frame period in second < interlace rate in second
**      "number of chunks in a fragment" 
**          = ( "fragment length in time" / "I-frame period" + 1 )
**          + ( VIDEO_ENG_LIMIT_NEAR_THRESHOLD * "interlace rate in second" )
** 
** (2) I-frame period in second > interlace rate in second
**      "number of chunks in a fragment" 
**          = ( "fragment length in time" * "byte rate" 
**              / "average chunk size in bytes" )
**          + ( "fragment length in time" / "I-frame period" + 1 )
**          + ( VIDEO_ENG_LIMIT_NEAR_THRESHOLD * "interlace rate in second" )
**
** This was simplified with the assumption that "I-frame period" was not less
** than 1 second to:
*/
#define VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT(g_context)   \
    (                                                           \
      MAX                                                       \
      (                                                         \
        ( VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE                      \
          + g_context->space_limit_threshold )                  \
        ,                                                       \
        (( VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE                     \
          + g_context->space_limit_threshold )                  \
        * (VIDEO_ENG_GET_VIDEO_BITRATE (g_context) / 8)         \
        / VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE (g_context))    \
        * 2                                                     \
      )                                                         \
      * VIDEO_ENG_FRAGMENT_TABLE_SIZE_FACTOR / 100              \
    )

/* This tells how many video chunks table blocks at least should be stored
** to record VIDEO_ENG_MAX_STREAM_LENGTH seconds of video.
*/
#define VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES(g_context)        \
    (                                                           \
        VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM (g_context)    \
        / VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE                 \
    )

/* This tells the maximum number of audio frames in the stream. */
#define VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM(g_context)    \
    (                                                           \
        VIDEO_ENG_MAX_STREAM_LENGTH                             \
        * g_context->audio_channels                             \
        * g_context->audio_sampling_frequency                   \
        / g_context->audio_samples_per_frame                    \
    )

/* This tells the maximum audio frames in a fragment.
*/
#define VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT(g_context)    \
    (                                                             \
        (( VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE                       \
          + g_context->space_limit_threshold )                    \
        * g_context->audio_channels                               \
        * g_context->audio_sampling_frequency                     \
        / g_context->audio_samples_per_frame)                     \
        * VIDEO_ENG_FRAGMENT_TABLE_SIZE_FACTOR / 100              \
    )

/* This tells how many audio samples table blocks at least should be stored
** to record VIDEO_ENG_MAX_STREAM_LENGTH seconds of audio.
*/
#ifdef FEATURE_CAMCORDER_AAC_REC
#define VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES(g_context)             \
    (                                                                 \
        (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC  \
         || g_context->audio_samples_per_frame > 1)                   \
        ? ( VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM (g_context)     \
            / VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE (g_context) )    \
        : 0                                                           \
    )
#else /* FEATURE_CAMCORDER_AAC_REC */
#define VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES(g_context)             \
    (                                                                 \
        (g_context->audio_samples_per_frame > 1)                      \
        ? ( VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM (g_context)     \
            / VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE (g_context) )    \
        : 0                                                           \
    )

#endif /* ! FEATURE_CAMCORDER_AAC_REC */

/* This tells how many audio chunks table blocks at least should be stored
** to record VIDEO_ENG_MAX_STREAM_LENGTH seconds of audio.
*/
#define VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES(g_context)  \
    VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES (g_context)

/* This converts 'n' to 'n*1024'.
*/
#define VIDEO_ENG_KBYTES(n)                       ((n) * 1024)


/***************************************************************************/
/* Secondary definitions                                                   */
/***************************************************************************/

/* This is the desired minimum size of each chunk of media data to write out
** to the output at once.
*/
#define VIDEO_ENG_OPTIMAL_CHUNK_SIZE(g_context)     \
    VIDEO_ENG_GET_VIDEO_CHUNK_SIZE (g_context)

/* This is the desired minimum size of each chunk of audio data to write out
** to the output at once, if not interlacing with video.
*/
#define VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE(g_context)    \
    VIDEO_ENG_GET_AUDIO_CHUNK_SIZE (g_context)

/* This is the number of bytes to allocate for the video stream buffer.  It
** should be at least VIDEO_ENG_OPTIMAL_CHUNK_SIZE plus double the largest 
** video frame size, otherwise it will overflow before it is flushed out
** by the video format services.  If the processing latency in the engine is
** substantial (more than a frame or two) then this number should be even
** greater to account for that latency.
*/
#define VIDEO_ENG_VIDEO_STREAM_BUF_SIZE(g_context)          \
    (                                                       \
        2* VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE (g_context)   \
        * VIDEO_ENG_VIDEO_STREAM_BUF_SIZE_FACTOR       \
    )

/* This is the number of bytes to allocate for the audio stream buffer.  Be
** careful here to ensure that this is at least equal to
** VIDEO_ENG_VIDEO_STREAM_BUF_SIZE times the ratio of the audio bitrate over
** the video bitrate.  Otherwise, the buffer may overflow before enough data
** accumulates in the video stream to cause both streams to be flushed.
*/
#define VIDEO_ENG_AUDIO_STREAM_BUF_SIZE(g_context)          \
    (                                                       \
        VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE (g_context)   \
        * VIDEO_ENG_AUDIO_STREAM_BUF_SIZE_FACTOR / 100      \
    )

/* This is the number of bytes to allocate for the memory file buffer used to
** hold fragment received from video format services before a whole fragment
** is reordered and written to EFS.  Buffer of this size is allocated only if
** VIDEO_ENG_TEMPORARY_FILE_ON_MEMORY is set to TRUE.
*/
#define VIDEO_ENG_FRAGMENT_BUF_SIZE(g_context)   \
    VIDEO_ENG_GET_FRAGMENT_BUF_SIZE (g_context)

/* This is the number of bytes to allocate for the async writer buffer used
** to hold data written in efs_fwrite before it can be written to EFS.
** This number is used when fragmentation is enabled. If fragment is enabled,
** each fragment will be written to this buffer in a big burst, therefore 
** buffer larger than one fragment is needed.
** CAUTION: ASYNC BUFFER SIZE MUST BE A POWER OF 2.
*/
#define VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE(g_context)   \
    (                                                         \
        (VIDEO_ENG_GET_FRAGMENT_BUF_SIZE (g_context)          \
              <= VIDEO_ENG_KBYTES (64))                       \
        ? VIDEO_ENG_KBYTES(64)                                \
        : (VIDEO_ENG_GET_FRAGMENT_BUF_SIZE (g_context)        \
              <= VIDEO_ENG_KBYTES (128))                      \
        ? VIDEO_ENG_KBYTES(128)                               \
        : (VIDEO_ENG_GET_FRAGMENT_BUF_SIZE (g_context)        \
              <= VIDEO_ENG_KBYTES (256))                      \
        ? VIDEO_ENG_KBYTES (256)                              \
        : (VIDEO_ENG_GET_FRAGMENT_BUF_SIZE (g_context)        \
              <= VIDEO_ENG_KBYTES (512))                      \
        ? VIDEO_ENG_KBYTES (512)                              \
        : (VIDEO_ENG_GET_FRAGMENT_BUF_SIZE (g_context)        \
              <= VIDEO_ENG_KBYTES (1024))                     \
        ? VIDEO_ENG_KBYTES (1024)                             \
        : VIDEO_ENG_KBYTES (2048)                             \
    )

/* This is the size of the video samples table that is allocated on memory.
** The maximum number of video frames that the engine supports recording
** in determined by this and VIDEO_ENG_MAX_VIDEO_TABLE_STORES.
*/
#define VIDEO_ENG_MAX_VIDEO_FRAMES(g_context)                       \
    (                                                               \
        (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2)         \
        ? VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT (g_context)   \
        : VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE (g_context)        \
    )

/* This is the size of the video chunks table that is allocated on memory.
** The maximum number of media data chunks that the engine supports
** writing to the output is determined by this and 
** VIDEO_ENG_MAX_VIDEO_TABLE_STORES.
*/
#define VIDEO_ENG_MAX_CHUNKS(g_context)                             \
    (                                                               \
        (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2)         \
        ? VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT (g_context)    \
        : VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE                     \
    )

/* This is the size of the audio samples table that is allocated on memory.
** The maximum number of audio frames that the engine supports recording
** in determined by this and VIDEO_ENG_MAX_AUDIO_TABLE_STORES.
*/
#define VIDEO_ENG_MAX_AUDIO_FRAMES(g_context)                       \
    (                                                               \
        (g_context->file_brand == VIDEO_FMT_BRAND_FRAG_3G2)         \
        ? VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT (g_context)   \
        : VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE (g_context)        \
    )

/* This is the maximum video frames per second + 1. This is only used to 
** calculate the maximum bit rate when VIDEO_ENG_MAX_VIDEO_TABLE_STORES
** is greater than 0.
*/
#define VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND(g_context)     \
    (                                                         \
        g_context->video_time_resolution                      \
        / g_context->video_time_increment + 1                 \
    )

/* This is the maximum audio frames per second + 1. This is only used to 
** calculate the maximum bit rate when VIDEO_ENG_MAX_AUDIO_TABLE_STORES
** is greater than 0.
*/
#define VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND(g_context)     \
    (                                                         \
        g_context->audio_channels                             \
        * g_context->audio_sampling_frequency                 \
        / g_context->audio_samples_per_frame                  \
        + 1                                                   \
    )

/* This is the sum of the maximum number of video samples table stores and
** the maximum number of video chunks table stores.
*/
#define VIDEO_ENG_MAX_VIDEO_TABLE_STORES(g_context)           \
    (                                                         \
        VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES (g_context)     \
        + VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES (g_context)  \
    )

/* This is the sum of the maximum number of audio samples table stores and
** the maximum number of audio chunks table stores.
*/
#define VIDEO_ENG_MAX_AUDIO_TABLE_STORES(g_context)           \
    (                                                         \
      VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES (g_context)       \
      + VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES (g_context)    \
    )

 
/* This is the number of bytes to allocate for the async writer buffer used
** to hold data written in efs_fwrite before it can be written to EFS.
** This sets the Async Buffer size with the Next best power of 2 depending on the Video Bitrate
** CAUTION: ASYNC BUFFER SIZE MUST BE A POWER OF 2.
*/
#define VIDEO_ENG_ASYNC_WRITER_BUF_SIZE(g_context)                             \
(                                                                              \
  (VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE(g_context) > VIDEO_ENG_KBYTES (512))  \
  ? VIDEO_ENG_KBYTES (1024)                                                    \
  :(VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE(g_context) > VIDEO_ENG_KBYTES (256)) \
  ? VIDEO_ENG_KBYTES (512)                                                     \
  :(VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE(g_context) > VIDEO_ENG_KBYTES (128)) \
  ? VIDEO_ENG_KBYTES (256)                                                     \
  :(VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE(g_context) > VIDEO_ENG_KBYTES (64))  \
  ? VIDEO_ENG_KBYTES (128)                                                     \
  : VIDEO_ENG_KBYTES (64)                                                      \
)  


/*===========================================================================

                V A L U E S   O F   M A C R O S   U N D E R   
                      V A R I O U S   S E T T I N G S

-----------------------------------------------------------------------------
The macro values are based on the definitions in videoengi_cust.h such as;

      VIDEO_ENG_DESIRED_INTERLACE_RATE :      3000   [ms]
      VIDEO_ENG_OPTIMAL_CHUNK_LENGTH :        3      [s]
      VIDEO_ENG_LIMIT_NEAR_THRESHOLD :        10     [s]
      VIDEO_ENG_FRAG_3G2_FRAGMENT_SIZE :      15     [s]
      VIDEO_ENG_MAX_STREAM_LENGTH :           3600   [s]

-----------------------------------------------------------------------------
QVGA 384 [Kbps] 15 [fps] with QCELP full

VIDEO_ENG_VIDEO_AUDIO_INTERLACED =              1
VIDEO_ENG_GET_VIDEO_BITRATE =                   384000
VIDEO_ENG_GET_AUDIO_BITRATE =                   14000
VIDEO_ENG_GET_STREAM_BITRATE =                  398000
VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM =     54000
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT =   412
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES =        90
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM =      1200
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT =    27
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES =         2
VIDEO_ENG_GET_VIDEO_CHUNK_SIZE =                144000
VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE =         144000
VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM =     180000
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT =   1375
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES =        0
VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_AUDIO_CHUNK_SIZE =                5250
VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE =         5250
VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES =         2
---
VIDEO_ENG_OPTIMAL_CHUNK_SIZE =                  144000
VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE =            5250
VIDEO_ENG_VIDEO_STREAM_BUF_SIZE =               288000
VIDEO_ENG_AUDIO_STREAM_BUF_SIZE =               15750
VIDEO_ENG_FRAGMENT_BUF_SIZE =                   895500
VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE =      1048576
VIDEO_ENG_MAX_VIDEO_FRAMES =                    600
VIDEO_ENG_MAX_CHUNKS =                          600
VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND =        16
VIDEO_ENG_MAX_VIDEO_TABLE_STORES =              92
VIDEO_ENG_MAX_AUDIO_FRAMES =                    600
VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND =        51
VIDEO_ENG_MAX_AUDIO_TABLE_STORES =              2
VIDEO_ENG_ASYNC_WRITER_BUF_SIZE =               262144

-----------------------------------------------------------------------------
QVGA 384 [Kbps] 15 [fps] with QCELP half

VIDEO_ENG_VIDEO_AUDIO_INTERLACED =              1
VIDEO_ENG_GET_VIDEO_BITRATE =                   384000
VIDEO_ENG_GET_AUDIO_BITRATE =                   6800
VIDEO_ENG_GET_STREAM_BITRATE =                  390800
VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM =     54000
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT =   412
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES =        90
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM =      1200
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT =    27
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES =         2
VIDEO_ENG_GET_VIDEO_CHUNK_SIZE =                144000
VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE =         144000
VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM =     180000
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT =   1375
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES =        0
VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_AUDIO_CHUNK_SIZE =                2550
VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE =         2550
VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES =         2
---
VIDEO_ENG_OPTIMAL_CHUNK_SIZE =                  144000
VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE =            2550
VIDEO_ENG_VIDEO_STREAM_BUF_SIZE =               288000
VIDEO_ENG_AUDIO_STREAM_BUF_SIZE =               7650
VIDEO_ENG_FRAGMENT_BUF_SIZE =                   879300
VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE =      1048576
VIDEO_ENG_MAX_VIDEO_FRAMES =                    600
VIDEO_ENG_MAX_CHUNKS =                          600
VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND =        16
VIDEO_ENG_MAX_VIDEO_TABLE_STORES =              92
VIDEO_ENG_MAX_AUDIO_FRAMES =                    600
VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND =        51
VIDEO_ENG_MAX_AUDIO_TABLE_STORES =              2
VIDEO_ENG_ASYNC_WRITER_BUF_SIZE =               262144

-----------------------------------------------------------------------------
QVGA 384 [Kbps] 15 [fps] with EVRC

VIDEO_ENG_VIDEO_AUDIO_INTERLACED =              1
VIDEO_ENG_GET_VIDEO_BITRATE =                   384000
VIDEO_ENG_GET_AUDIO_BITRATE =                   9200
VIDEO_ENG_GET_STREAM_BITRATE =                  393200
VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM =     54000
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT =   412
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES =        90
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM =      1200
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT =    27
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES =         2
VIDEO_ENG_GET_VIDEO_CHUNK_SIZE =                144000
VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE =         144000
VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM =     180000
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT =   1375
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES =        0
VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_AUDIO_CHUNK_SIZE =                3450
VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE =         3450
VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES =         2
---
VIDEO_ENG_OPTIMAL_CHUNK_SIZE =                  144000
VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE =            3450
VIDEO_ENG_VIDEO_STREAM_BUF_SIZE =               288000
VIDEO_ENG_AUDIO_STREAM_BUF_SIZE =               10350
VIDEO_ENG_FRAGMENT_BUF_SIZE =                   884700
VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE =      1048576
VIDEO_ENG_MAX_VIDEO_FRAMES =                    600
VIDEO_ENG_MAX_CHUNKS =                          600
VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND =        16
VIDEO_ENG_MAX_VIDEO_TABLE_STORES =              92
VIDEO_ENG_MAX_AUDIO_FRAMES =                    600
VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND =        51
VIDEO_ENG_MAX_AUDIO_TABLE_STORES =              2
VIDEO_ENG_ASYNC_WRITER_BUF_SIZE =               262144

-----------------------------------------------------------------------------
QVGA 384 [Kbps] 15 [fps] with AMR

VIDEO_ENG_VIDEO_AUDIO_INTERLACED =              1
VIDEO_ENG_GET_VIDEO_BITRATE =                   384000
VIDEO_ENG_GET_AUDIO_BITRATE =                   12800
VIDEO_ENG_GET_STREAM_BITRATE =                  396800
VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM =     54000
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT =   412
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES =        90
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM =      1200
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT =    27
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES =         2
VIDEO_ENG_GET_VIDEO_CHUNK_SIZE =                144000
VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE =         144000
VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM =     180000
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT =   1375
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES =        0
VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_AUDIO_CHUNK_SIZE =                4800
VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE =         4800
VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES =         2
---
VIDEO_ENG_OPTIMAL_CHUNK_SIZE =                  144000
VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE =            4800
VIDEO_ENG_VIDEO_STREAM_BUF_SIZE =               288000
VIDEO_ENG_AUDIO_STREAM_BUF_SIZE =               14400
VIDEO_ENG_FRAGMENT_BUF_SIZE =                   892800
VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE =      1048576
VIDEO_ENG_MAX_VIDEO_FRAMES =                    600
VIDEO_ENG_MAX_CHUNKS =                          600
VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND =        16
VIDEO_ENG_MAX_VIDEO_TABLE_STORES =              92
VIDEO_ENG_MAX_AUDIO_FRAMES =                    600
VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND =        51
VIDEO_ENG_MAX_AUDIO_TABLE_STORES =              2
VIDEO_ENG_ASYNC_WRITER_BUF_SIZE =               262144

-----------------------------------------------------------------------------
QVGA 384 [Kbps] 15 [fps] without audio

VIDEO_ENG_VIDEO_AUDIO_INTERLACED =              0
VIDEO_ENG_GET_VIDEO_BITRATE =                   384000
VIDEO_ENG_GET_AUDIO_BITRATE =                   0
VIDEO_ENG_GET_STREAM_BITRATE =                  384000
VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM =     54000
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT =   412
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES =        90
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM =      1200
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT =    27
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES =         2
VIDEO_ENG_GET_VIDEO_CHUNK_SIZE =                144000
VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE =         144000
VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM =     180000
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT =   1375
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES =        0
VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_AUDIO_CHUNK_SIZE =                0
VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE =         0
VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES =         2
---
VIDEO_ENG_OPTIMAL_CHUNK_SIZE =                  144000
VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE =            0
VIDEO_ENG_VIDEO_STREAM_BUF_SIZE =               288000
VIDEO_ENG_AUDIO_STREAM_BUF_SIZE =               0
VIDEO_ENG_FRAGMENT_BUF_SIZE =                   864000
VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE =      1048576
VIDEO_ENG_MAX_VIDEO_FRAMES =                    600
VIDEO_ENG_MAX_CHUNKS =                          600
VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND =        16
VIDEO_ENG_MAX_VIDEO_TABLE_STORES =              92
VIDEO_ENG_MAX_AUDIO_FRAMES =                    600
VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND =        51
VIDEO_ENG_MAX_AUDIO_TABLE_STORES =              2
VIDEO_ENG_ASYNC_WRITER_BUF_SIZE =               262144

===========================================================================*/

#ifdef FEATURE_CAMCORDER_AAC_REC
/*===========================================================================
In addition, the macro values are based on the following settings.

        AAC channel mode :            mono
        AAC sampling rate :           16000 [Hz]
        AAC bit rate :                28000 [bps]

-----------------------------------------------------------------------------
QVGA 384 [Kbps] 15 [fps] with AAC

VIDEO_ENG_VIDEO_AUDIO_INTERLACED =              1
VIDEO_ENG_GET_VIDEO_BITRATE =                   384000
VIDEO_ENG_GET_AUDIO_BITRATE =                   28000
VIDEO_ENG_GET_STREAM_BITRATE =                  412000
VIDEO_ENG_GET_VIDEO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_VIDEO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_STREAM =     54000
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_IN_FRAGMENT =   412
VIDEO_ENG_GET_MAX_VIDEO_SAMPLES_STORES =        90
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_STREAM =      1200
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_IN_FRAGMENT =    27
VIDEO_ENG_GET_MAX_VIDEO_CHUNKS_STORES =         2
VIDEO_ENG_GET_VIDEO_CHUNK_SIZE =                144000
VIDEO_ENG_GET_ACTUAL_VIDEO_CHUNK_SIZE =         144000
VIDEO_ENG_GET_AUDIO_SAMPLES_TABLE_SIZE =        600
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_STREAM =     56250
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_IN_FRAGMENT =   429
VIDEO_ENG_GET_MAX_AUDIO_SAMPLES_STORES =        93
VIDEO_ENG_GET_AUDIO_CHUNKS_TABLE_SIZE =         600
VIDEO_ENG_GET_AUDIO_CHUNK_SIZE =                10500
VIDEO_ENG_GET_ACTUAL_AUDIO_CHUNK_SIZE =         10500
VIDEO_ENG_GET_MAX_AUDIO_CHUNKS_STORES =         2
---
VIDEO_ENG_OPTIMAL_CHUNK_SIZE =                  144000
VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE =            10500
VIDEO_ENG_VIDEO_STREAM_BUF_SIZE =               288000
VIDEO_ENG_AUDIO_STREAM_BUF_SIZE =               31500
VIDEO_ENG_FRAGMENT_BUF_SIZE =                   927000
VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE =      1048576
VIDEO_ENG_MAX_VIDEO_FRAMES =                    600
VIDEO_ENG_MAX_CHUNKS =                          600
VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND =        16
VIDEO_ENG_MAX_VIDEO_TABLE_STORES =              92
VIDEO_ENG_MAX_AUDIO_FRAMES =                    600
VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND =        16
VIDEO_ENG_MAX_AUDIO_TABLE_STORES =              95
VIDEO_ENG_ASYNC_WRITER_BUF_SIZE =               262144
===========================================================================*/
#endif /* FEATURE_CAMCORDER_AAC_REC */

#endif /* #ifndef FEATURE_VIDEO_ENG_FIXED_PARAMETERS */
#endif /* #ifdef FEATURE_VIDEO_ENCODE */
#endif /* #ifndef VIDEOENGI_MACRO_H */
