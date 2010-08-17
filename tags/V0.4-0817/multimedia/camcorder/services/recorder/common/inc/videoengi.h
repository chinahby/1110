#ifndef VIDEOENGI_H
#define VIDEOENGI_H
/*===========================================================================

                  V I D E O   E N C O D E   E N G I N E
                 I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the internal definitions to the video encode
  engine module.

Copyright(c) 2003-2007 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/videoengi.h#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20/01/08   sanal   Handle fast pause resume
12/17/08   sanal   Added return types for functions required to enable 
                   synchronous usage of videoeng APIs
12/23/08   girip  Featurise audio recording for FTM camcorder
11/24/08   ps      Removed Superfluouscompiler  warning
11/14/08   sanal   Store the temp files at the save location of movie clip
09/23/08   ps      Done Changes for more than 4GB card support
07/08/08   shiven  Adding support for loci atom
04/04/08   sanal   Fix for last frame dropped in post editing
02/11/08   sanal   Addign Idiag support for frames dropped in a recorded clip
11/05/07   dhaval  Added copyright atom support
09/03/07   Yogi    Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
06/19/07   kadhir  Added FTL and USB as storage media
03/12/07   ali     Added max_bitrate flag
02/27/07   jdas    Fix up 'ftyp' for only 1 fragment in fragmented format.
02/09/07   ali     Changes for dynamic RC based on feedback from EFS
11/30/06   Pavan   Support added for Movieedit Interface
11/30/06   Shahed  Move video_eng_cmx_sync_cb_func() outside feature
                   flag FEATURE_AAC_REC due to its usage in non-AAC cases.
10/30/06   dhaval  Setting space_limit_threshold and space_imminent_threshold
                   value according to user's settings from UI.
09/04/06   dhaval  Added code to support VENC API for transcoder
08/10/06   Nag     Error recovery added incase of error while Reordering atoms
                   in last fragment
07/14/06   jdas    Added featurized clipinfo stored in a recorded clip
07/11/06   nag     Added init parameter to reorder of moov mdat atoms according to KDDI specification
06/06/06   jdas    Added support of fragment_duration in moov-mvex-mehd when
                   output set to VIDEO_METHOD_CLIENT
06/22/06   dw      Prevent error recovery due to invalid DSP timer expiration
06/20/06   Gopi    Added support for dynamically updating titl,descp
                   auth atoms.
05/17/06   jdas    Added H.264 profile/level related macros
05/04/06   Shahed  Changes for new A/V sync implementation for AAC with
                   CMX MM API sync signal.
05/04/06   cl      Changes to switch stream order.
04/17/06   Shahed  Added a new error handling case in video engine to recover
           /Wyh    from QDSP failure/dead in the middle of video encoding.
04/10/06   jdas    Removed lint error
04/07/06   Shahed  Added a data structure used in audio/video sync.
03/27/06   dw      Allow video_encode_set/get_config through video engine
03/08/06   Nagesh  Video Async Buffer size set based on the Video Bit Rate
02/09/06   wyh     New levels for MPEG4 simple profile.
02/03/06   Gopi    Added message to Switch context of g_frame->clk callback
                   to graph.
12/23/05   jk      Write temporary files to the same location as the target
                   file with updated space check.
11/23/05   dw      Added recovery error support VS packet shortage
11/14/05   Shahed  New implementation of camcorder space status error/warning msgs.
11/11/05   Shahed  Undo SPACE error/warning changes.
11/08/05   Shahed  Added an enum for details of SPACE error/warning type for
                   use in g_context.
09/23/05   dw/aa   Enhancements for video engine buffer overflow handling
08/31/05   wyh     Implement Code review changes for video_eng_frame
07/18/05   jk      Merged from videoengi.h#33 in msmshared.
                   Added video_eng_get_config and video_eng_set_config.
                   Changed input argument to macro functions.
                   Simplified audio stream truncation.
07/05/05   jk      Added 'aac_dsi_written' to video_eng_context_type.
06/28/05   jk      Changed VIDEO_ENG_MAX_CHUNKS as per videofmt that make
                   every I-frame in fragments the first frame of a chunk.
06/14/05   jk      Added support for the AAC UI.
06/08/05   jk      Fixed video_eng_write_uuid.
05/31/05   jk      Added support for the AAC encoder.
                   Added support for the simplified customization.
                   Moved video_fmt_create from init to start.
                   Added support to record very long clips, writing meta
                   data to a temporary storage.
                   Added space check for MMC in addition to flash.
05/12/05   jk      Changed interlace rate unit from second to ms.
04/12/05   jk      Extended chunk/sample tables to 1 hour.
03/16/05   jk      Fixed 'stts' atom related error in 3gp brand where rounding
                   error in delta accumulated.
03/16/05   wyh     New QDSP API requires 8 uint16's per frame done for stats.
03/07/05   jk      Included videoeng_fio.h.
03/01/05   jk      Fixed for fragmented 3g2 brand at 384Kbps.
                   Implemented video engine file object. Changed:
                   (1) to use video engine file object.
                   (2) to use temporary fragment either on EFS or on memory.
                   Fixed EFS free space calculation when temporary file is
                   on EFS.
02/22/05   jk      Fixed MPEG4 Profile@Level indication.
02/22/05   wyh     Raw Encoder API changes.
02/16/05   jk      Merged in changes from mainline #29. Major changes are:
                   - Added file size limit feature.
                   - Fixed file limit reached logic.
                   - Now collapsing all 'mdat' atoms together when recording.
                   - Replaced all TASKLOCK/TASKFREE with REX critical sections.
02/10/05   jk      Merged in timed text support from mainline #28.
01/25/05   wh      Cosmetic changes from code review of video_eng_frame api
01/21/05   wh      Eliminate all malloc()'s from the mp4 library.
01/15/05   wh      Added video_eng_frame() API.
12/29/04   wyh     Heap tracking support through videoeng_malloc();
12/07/04   jk      Updated audio stream truncation after code walk.
11/29/04   drl     Increased VIDEO_ENG_AUDIO_STREAM_BUF_SIZE to hold 7 seconds
                   of audio.  This supports N-Frame Delayed encoding.
11/24/04   jk      Added some audio variables to video_eng_context_type to
                   truncate audio stream upon pause/stop.
11/18/04   jk      Updated as per the code walk.
                   Deleted all video_eng_stream_XXX functions.
11/17/04   jk      Added buffer_out and efs_free_space to
                   video_eng_context_type.
                   Increased VIDEO_ENG_NUM_PACKETS to 100.
11/03/04   jk      Buffer sharing among video encoder, video engine, and
                   videofmt.
                   Merged some codes in from videoengi.h#27 in MSMSHARED
10/01/04   jk      Renamed enc_frame_ptr to enc_pkt_ptr and updated for new video
                   apis.
09/06/04   pp/wyh  VIDEO_ENG_VED_BUF_SIZE -> 32768 to match BITSTREAM_BUFSIZE
                   VIEO_ENG_MB_BUF_SIZE is 779 per MSM6550 interface spec.
01/26/04    gr     Added support for buffered file I/O.
10/27/03   rpw     Added support for KDDI fragmented file version of 3G2.
09/08/03   sj      Added media_timescale to video_eng_context_type
09/08/03   sj      Added support of variable frame rate.
08/04/03   rpw     Revamp of code to separate out movie file writer and
                   integrate it into videofmt.
07/23/03   rpw     Doubled the max header size from 32 to 64 to accomodate
                   larger bitstream headers.
06/30/03   rpw     Fixed bug where H.263 video had VOS/VO headers.
05/13/03   rpw     Changed fs_handle_open to output_open and moved outside
                   EFS featurization.
05/07/03   rpw     Added engine status codes VIDEO_LIMIT_NEAR and
                   VIDEO_LIMIT_REACHED.  The former is sent when the engine
                   determines it is about to run out of memory or flash.  The
                   latter is sent if the engine actually does run out of
                   memory or flash, in which case a video_eng_stop is
                   performed automatically.
                   Added code to save what the engine can of the file being
                   recorded, if a failure occurs.  If the file cannot be
                   saved, and is in the EFS, it is deleted before the engine
                   exits.
                   Added code to relax the constraint that every video chunk
                   past the first needed to have at most the number of
                   samples as in the previous chunk.
                   Tuned optimal chunk size and video stream buffer size.
04/07/03   rpw     Added EVRCSampleEntry generation (when QTV interop
                   not enabled).
                   Added visual_object_sequence_end_code generation.
                   Replaced OEM_Malloc/OEM_Free with malloc/free.
                   Optimized writes of bitstream, combining 8-byte 'mdat'
                   header with video chunk.
                   Added calculations of average and maximum bitrates.
03/27/03   rpw     Fixed bug in AMR header skipping, and code used to specify
                   AMR in cmx_qcp_record_reverse.
03/24/03   rpw     Changed frame rate to time increment and resolution.
03/24/03   rpw     Fixed bugs in writing audio data in offline mode.
03/10/03   rpw     New enhanced feature set added for "offline"
                   movie writing.
                   Added synchronous call capability for "offline" mode.
                   Added API for writing UUID atoms.
02/21/03   rpw     Renamed vdd_buf_ptr* to enc_frame_ptr* to reflect
                   actual usage
02/18/03   rpw     Fixed AMR packet size
02/17/03   rpw     Added audio support
11/22/02   rpw     Created file.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer specific definitions                   */

#ifdef FEATURE_VIDEO_ENCODE

#include "videoeng.h"   /* video engine typedefs and prototypes            */
#include "videofmt.h"   /* video format services                           */
#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "queue.h"      /* Definitions for queue services                  */
#include "cmx.h"        /* Compact Media Extension (CMX) services          */
#include "rex.h"        /* Real Time Executive                             */
#include "video_enc.h"  /* Video Encode Interface                          */

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
#include "video_enc_dsp_if.h"
#include "video_enc_special_effects.h"
#endif

#ifdef FEATURE_EFS2
#include "fs_public.h"          /* File system typedefs and prototypes     */
#include "fs_stdio.h"           /* File system buffered I/O header         */
#include "videoeng_fio.h"       /* Video engine file I/O header            */
#include "videoeng_amc.h"
#endif /* FEATURE_EFS2 */

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
#include "clk.h"
#endif

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*****************************************************************************/
/* Flexible constants - the following are constants that can be adjusted to  */
/* tune the engine in various ways.  These are described in more detail in   */
/* the Qcamcorder application note.                                          */
/*****************************************************************************/

/* Video Encode Engine Command Queue Available Packets.
*/
#define VIDEO_ENG_NUM_PACKETS                   100

/* This is the number of command queue packets to reserve for
** VIDEO_ENG_QUEUE_RESERVE_URGENT priority request.
*/
#define VIDEO_ENG_QUEUE_RESERVE_URGENT          30

/* This is the number of command queue packets to reserve for
** VIDEO_ENG_QUEUE_RESERVE_CRITICAL priority request.
*/
#define VIDEO_ENG_QUEUE_RESERVE_CRITICAL        2

/*
** This is the size of the DSP buffer per MB in words
**  11 + sizeof(uint16)*6*64
*/
#define VIDEO_ENG_MB_BUF_SIZE                   779

/* This is the desired minimum size of each chunk of media data to write out
** to the output at once.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_OPTIMAL_CHUNK_SIZE(x)         49152  /* 1 sec at 384Kbps */
#endif

/* This is the desired minimum size of each chunk of audio data to write out
** to the output at once, if not interlacing with video.
*/
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_OPTIMAL_AUDIO_CHUNK_SIZE(x)   5250  /* 3 sec of QCELP */
#endif
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/* This is the number of bytes to allocate for the video stream buffer.  It
** should be at least VIDEO_ENG_OPTIMAL_CHUNK_SIZE plus double the largest
** video frame size, otherwise it will overflow before it is flushed out
** by the video format services.  If the processing latency in the engine is
** substantial (more than a frame or two) then this number should be even
** greater to account for that latency.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_VIDEO_STREAM_BUF_SIZE(x)      96750
                                                /* 6 sec @ 128Kb + 1000 */
#endif

/* This is the number of bytes to allocate for the audio stream buffer.  Be
** careful here to ensure that this is at least equal to
** VIDEO_ENG_VIDEO_STREAM_BUF_SIZE times the ratio of the audio bitrate over
** the video bitrate.  Otherwise, the buffer may overflow before enough data
** accumulates in the video stream to cause both streams to be flushed.
*/
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_AUDIO_STREAM_BUF_SIZE(x)      13250
                                                /* 7 sec of QCELP + 1000 */
#endif
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/* This is the number of bytes to allocate for the timed text stream buffer. */
#define VIDEO_ENG_TEXT_STREAM_BUF_SIZE          4096

/* This is the size of the video samples table that is allocated on memory.
** The maximum number of video frames that the engine supports recording
** in determined by this and VIDEO_ENG_MAX_VIDEO_TABLE_STORES.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_MAX_VIDEO_FRAMES(x)           600   /* 40 sec. at 15fps */
#endif

/* This is the size of the audio samples table that is allocated on memory.
** The maximum number of audio frames that the engine supports recording
** in determined by this and VIDEO_ENG_MAX_AUDIO_TABLE_STORES.
*/
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_MAX_AUDIO_FRAMES(x)           180000
                                                /* 60 minutes at 50fps */
#endif
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/* This is the size of the video chunks table that is allocated on memory.
** The maximum number of media data chunks that the engine supports
** writing to the output is determined by this and
** VIDEO_ENG_MAX_VIDEO_TABLE_STORES.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_MAX_CHUNKS(x)                 600
                                                /* 30 min at 3 sec/chunk */
#endif

/* This is the maximum video frames per second + 1. This is only used to
** calculate the maximum bit rate when VIDEO_ENG_MAX_VIDEO_TABLE_STORES
** is greater than 0.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_MAX_VIDEO_SAMPLES_PER_SECOND(x)       16
#endif

/* This is the maximum audio frames per second + 1. This is only used to
** calculate the maximum bit rate when VIDEO_ENG_MAX_AUDIO_TABLE_STORES
** is greater than 0.
*/
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_MAX_AUDIO_SAMPLES_PER_SECOND(x)       16  /* mono, 16KHz */
#endif
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/* This is the sum of the maximum number of video samples table stores and
** the maximum number of video chunks table stores.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_MAX_VIDEO_TABLE_STORES(x)             92
                    /* 90 samples table + 2 chunks table for 1 hour */
#endif

/* NOTE:
** VIDEO_ENG_MAX_VIDEO_FRAMES set to 600 can take 600 frames for 40 seconds at
** 15 fps.  VIDEO_ENG_MAX_CHUNKS set to 600 can take 600 chunks for 30 minutes
** at 3 seconds/chunk.  The number 600 was taken considering the meta data
** size in bytes per write to flash.  If this is too small, it is inefficient
** to write small data too often.  If this is too large, it takes long to
** write large data and may block other operation during that period.
** VIDEO_ENG_MAX_VIDEO_TABLE_STORES set to 92 can store 90 video samples table
** for 1 hour (90 x 40 seconds) and 2 video chunks table for 1 hour (2 x 30
** minutes.
*/

/* This is the sum of the maximum number of audio samples table stores and
** the maximum number of audio chunks table stores.
*/
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_MAX_AUDIO_TABLE_STORES(x)             95
                    /* 93 samples table + 2 chunks table for 1 hour */
#endif
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/* This is the maximum number of bytes to allocate for the decoder specific
** information for the video bitstream.
*/
#define VIDEO_ENG_VSTREAM_HEADER_MAX                    64

/* This is the rate, in seconds, at which to reset the sample-to-chunk mapping
** table run-length encoding algorithm, which tries to maintain a constant
** number of samples per chunk.  Resetting the algorithm allows more samples
** to be combined into chunks, at the expense of adding at least one 'stsc'
** table entry every time a reset is done.
*/
#define VIDEO_ENG_STSC_ALGO_RESET_RATE                  5

/* This is to decide whether to store temporary file fragment before reordering
** on memory or on efs.  Set it to TRUE to store the temporary fragment on
** memory for better performance.
*/
#define VIDEO_ENG_TEMPORARY_FRAGMENT_ON_MEMORY          TRUE

/* This is the number of bytes to allocate for the memory file buffer used to
** hold fragment received from video format services before a whole fragment
** is reordered and written to EFS.  Buffer of this size is allocated only if
** VIDEO_ENG_TEMPORARY_FILE_ON_MEMORY is set to TRUE.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_FRAGMENT_BUF_SIZE(x)                  (1024 * 1024)
#endif

/* This is the number of bytes to allocate for the async writer buffer used
** to hold data written in efs_fwrite before it can be written to EFS.
** This number is used when fragmentation is disabled. If fragment is disabled,
** data will be written to this buffer chunk by chunk. Buffer larger than one
** chunk is needed.
** CAUTION: ASYNC BUFFER SIZE MUST BE A POWER OF 2.
*/
#define VIDEO_ENG_ASYNC_WRITER_DEFAULT_BUF_SIZE         65536


/* This is the number of bytes to allocate for the async writer buffer used
** to hold data written in efs_fwrite before it can be written to EFS.
** This is used to store 3 sec of data in case of QVGA and video bitrate of 384Kbps
** CAUTION: ASYNC BUFFER SIZE MUST BE A POWER OF 2.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_ASYNC_WRITER_BUF_SIZE(x)     (262144)
#endif


/* This is the number of bytes to allocate for the async writer buffer used
** to hold data written in efs_fwrite before it can be written to EFS.
** This number is used when fragmentation is enabled. If fragment is enabled,
** each fragment will be written to this buffer in a big burst, therefore
** buffer larger than one fragment is needed.
** CAUTION: ASYNC BUFFER SIZE MUST BE A POWER OF 2.
*/
#ifdef FEATURE_VIDEO_ENG_FIXED_PARAMETERS
#define VIDEO_ENG_ASYNC_WRITER_FRAGMENT_BUF_SIZE(x)     (1024 * 1024)
#endif

/* This is the minimum number of bytes to keep free on the filesystem where
** the movie is stored - if the free space falls below this number, the movie
** recording should be stopped automatically.
*/
#define VIDEO_ENG_EFS_MARGIN                            65536

/* This is the minimum number of bytes to keep free in the memory buffer where
** the movie is stored, if writing a movie to a memory buffer - if the free
** space falls below this number, the movie recording should be stopped
** automatically.
*/
#define VIDEO_ENG_MEM_MARGIN                            65536

/* These are names of temporary files to use if necessary when converting
** movie files.
*/
#ifdef T_WINNT
#error code not present
#else
#define VIDEO_ENG_TEMP_FILE_1                "/temp1.mp4"
#define VIDEO_ENG_TEMP_FILE_2                "/temp2.mp4"
#define VIDEO_ENG_TEMP_FILE_3                "/temp3.bin"
#endif

/* This is the height to make the box for a timed text stream. */
#define VIDEO_ENG_TEXT_BOX_HEIGHT            30

/* This is the size to make telop text when encoding in a timed text stream. */
#define VIDEO_ENG_TELOP_FONT_SIZE            12

/* This is the maximum number of samples that the engine supports writing to
** the timed text stream for a movie.
*/
#define VIDEO_ENG_MAX_TEXT_SAMPLES           100

/* This is the maximum number of audio samples to write to the video format
** services in one shot.  If more than this number of samples are generated by
** the CMX layer, they are split among several writes to pass them along to
** the movie file.
*/
#define MAX_AUDIO_SAMPLES_PER_WRITE          50

/*****************************************************************************/
/* Fixed constants - DO NOT TOUCH - the following are fixed constants that   */
/* are not meant to be changed, as they are either universal constants or    */
/* would have a high probability of breaking the implementation if changed.  */
/*****************************************************************************/

/* This signal is set when an item is put on the video_eng_cmd_q.
*/
#define VS_VIDEO_ENG_CMD_Q_SIG               0x0080

/* Used to get notification of an asynchronous EFS write completion.
*/
#define VS_VIDEO_ASYNC_WRITE_SIG             0x10000

/* Used to wait for async task completion of all requests */
#define VS_VIDEO_ASYNC_REQ_COMPLETE          0x20000

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/* This is the sampling rate of narrow-band vocoders. */
#define VOCODER_SAMPLING_RATE                8000

/* This is the number of vocoder packets generated per second. */
#define VOCODER_PACKET_RATE                  50

/* This is the number of samples per vocoder packet. */
#define VOCODER_SAMPLES_PER_PACKET           \
    (VOCODER_SAMPLING_RATE / VOCODER_PACKET_RATE)
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* These are the sizes of packets, in bytes, for different vocoder types. */
#define VOCODER_PACKET_SIZE_13K_FULL         35
#define VOCODER_PACKET_SIZE_13K_HALF         17
#define VOCODER_PACKET_SIZE_EVRC             23
#define VOCODER_PACKET_SIZE_AMR_1220         32  /* 12.2 kbps mode */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#define VOCODER_PACKET_SIZE_AMR_1020         27  /* 10.2 kbps mode */
#define VOCODER_PACKET_SIZE_AMR_0795         21  /* 7.95 kbps mode */
#define VOCODER_PACKET_SIZE_AMR_0740         20  /* 7.4 kbps mode */
#define VOCODER_PACKET_SIZE_AMR_0670         18  /* 6.7 kbps mode */
#define VOCODER_PACKET_SIZE_AMR_0590         16  /* 5.9 kbps mode */
#define VOCODER_PACKET_SIZE_AMR_0515         14  /* 5.15 kbps mode */
#define VOCODER_PACKET_SIZE_AMR_0475         13  /* 4.75 kbps mode */

/* These are the bit rates of the different vocoder types, in bits per
** second.
*/
#define VOCODER_BITRATE_13K_FULL             14000
#define VOCODER_BITRATE_13K_HALF             6800
#define VOCODER_BITRATE_EVRC                 9200
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/* This is the size of the "QCELP File Header", as defined by KDDI's spec,
** "CDMA Motion Picture Terminal Audio/Video File Format Specification for
** Downloading/Streaming Short Audio/Video Clips, version 1.1.2", figure 4 in
** section 2.5.1.2.
*/
#define QCP_FILE_HEADER_SIZE                 162

#ifdef FEATURE_VIDEO_ENG_QTV_INTEROP
/* This is the size of the audio decoder specific information structure
** expected by QTV for EVRC audio.
*/
#define PV_EVRC_STREAM_HEADER_SIZE           8
#endif /* FEATURE_VIDEO_ENG_QTV_INTEROP */

/* This is the size of an AMR file header, as defined by RFC 3267. */
#define AMR_FILE_HEADER_SIZE                 6

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/* This is the size of an actual QCP file header, as generated by CMX. */
#define CMX_QCP_HEADER_SIZE                  194

/* This tells vocoder audio bitrate which is maximum for each audio format. */
#define VIDEO_ENG_GET_VOCODER_BITRATE(g_context)                            \
    (                                                                       \
        (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL)   \
        ? (VOCODER_BITRATE_13K_FULL)                                        \
        : (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF) \
        ? (VOCODER_BITRATE_13K_HALF)                                        \
        : (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_EVRC)          \
        ? (VOCODER_BITRATE_EVRC)                                            \
        : (g_context->audio_format == VIDEO_FMT_STREAM_AUDIO_AMR)           \
        ? (                                                                 \
              (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_1220)    \
              ? VOCODER_PACKET_SIZE_AMR_1220 * VOCODER_PACKET_RATE * 8      \
              : (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_1020)  \
              ? VOCODER_PACKET_SIZE_AMR_1020 * VOCODER_PACKET_RATE * 8      \
              : (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_0795)  \
              ? VOCODER_PACKET_SIZE_AMR_0795 * VOCODER_PACKET_RATE * 8      \
              : (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_0740)  \
              ? VOCODER_PACKET_SIZE_AMR_0740 * VOCODER_PACKET_RATE * 8      \
              : (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_0670)  \
              ? VOCODER_PACKET_SIZE_AMR_0670 * VOCODER_PACKET_RATE * 8      \
              : (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_0590)  \
              ? VOCODER_PACKET_SIZE_AMR_0590 * VOCODER_PACKET_RATE * 8      \
              : (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_0515)  \
              ? VOCODER_PACKET_SIZE_AMR_0515 * VOCODER_PACKET_RATE * 8      \
              : (g_context->audio_rate_set == VIDEO_FMT_RATE_SET_AMR_0475)  \
              ? VOCODER_PACKET_SIZE_AMR_0475 * VOCODER_PACKET_RATE * 8      \
              : (0)                                                         \
          )                                                                 \
        : (0)                                                               \
    )

#ifdef FEATURE_CAMCORDER_AAC_REC
/* This is the number of samples per AAC frame. */
#define AAC_SAMPLES_PER_FRAME                1024

/* This is the AAC bits per sample times 4096 for high bit rate. */
#define AAC_BITS_PER_SAMPLE_HIGH             0x1C00

/* This is the AAC bits per sample times 4096 for medium bit rate. */
#define AAC_BITS_PER_SAMPLE_MED              0x1800

/* This is the AAC bits per sample times 4096 for low bit rate. */
#define AAC_BITS_PER_SAMPLE_LOW              0x1000

/* This is the AAC bits per sample factor. */
#define AAC_BITS_PER_SAMPLE_DIVISOR          4096
#endif /* FEATURE_CAMCORDER_AAC_REC */

/* This is the sync signal callback frequency expected from CMX in msec */
#define AV_SYNC_CMX_SIGNALING_TIME  200
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* This is the size of QCIF */
#define QCIF_DX 176
#define QCIF_DY 144

/* This is the size of CIF */
#define CIF_DX 352
#define CIF_DY 288

/* This is the size of VGA */
#define VGA_DX 640
#define VGA_DY 480

/* This is the size of PAL */
#define PAL_DX 720
#define PAL_DY 576

/* This is the bit rate limits for each profiles@levels */
#define SIMPLE_L0_MAX_BIT_RATE    64000
#define SIMPLE_L1_MAX_BIT_RATE    64000
#define SIMPLE_L2_MAX_BIT_RATE   128000
#define SIMPLE_L3_MAX_BIT_RATE   384000
#define CORE_L2_MAX_BIT_RATE    2000000

/* This is the profile and level indications */
#define SIMPLE_PROFILE_LEVEL1     0x01
#define SIMPLE_PROFILE_LEVEL2     0x02
#define SIMPLE_PROFILE_LEVEL3     0x03
#define CORE_PROFILE_LEVEL2       0x22
#define SIMPLE_PROFILE_LEVEL0     0x08

/* Simple profile leves 4a and 5 was added in 14496-2 3rd Ed Amd 2
   2005-10-01 AMENDMENT 2: New Levels for Simple Profile
*/
#define SIMPLE_L4a_MAX_BIT_RATE 4000000
#define SIMPLE_L5_MAX_BIT_RATE  8000000
#define SIMPLE_PROFILE_LEVEL4a    0x04
#define SIMPLE_PROFILE_LEVEL5     0x05

/* use this as the default if there is no matching simple profile
** level
*/
#define PROFILE_MAIN_L4           0x34

/* This is the offset from the visual_object_sequence_start_code where
** the profile and level indication is located.
*/
#define PROFILE_LEVEL_OFFSET    4




#define H264_BASE_LINE_PROFILE    66

#define H264_DEFAULT_PROFILE_COM  0x80
#define H264_L1b_PROFILE_COM      0x90

/* Number of pixels present in one
** macroblock in H.264
*/
#define H264_PIXELS_PER_MB   256

/* The following macros are defined for H.264
** from H.264 specs Annex A Table A-1 */

/* Max video Bitrate for each level */
#define H264_L1_MAX_BIT_RATE    64000
#define H264_L1b_MAX_BIT_RATE   128000
#define H264_L11_MAX_BIT_RATE   192000
#define H264_L12_MAX_BIT_RATE   384000
#define H264_L13_MAX_BIT_RATE   768000
#define H264_L2_MAX_BIT_RATE    2000000
#define H264_L21_MAX_BIT_RATE   4000000
#define H264_L22_MAX_BIT_RATE   4000000

/* Different H.264 level values */
#define H264_LEVEL1       10
#define H264_LEVEL1b      11
#define H264_LEVEL11      11
#define H264_LEVEL12      12
#define H264_LEVEL13      13
#define H264_LEVEL2       20
#define H264_LEVEL21      21
#define H264_LEVEL22      22

/* List of frame sizes in MB for diff levels */
#define H264_MB_PER_FRAME_L1   99
#define H264_MB_PER_FRAME_L1b  99
#define H264_MB_PER_FRAME_L11  396
#define H264_MB_PER_FRAME_L12  396
#define H264_MB_PER_FRAME_L13  396
#define H264_MB_PER_FRAME_L2   396
#define H264_MB_PER_FRAME_L21  792
#define H264_MB_PER_FRAME_L22  1620


/* List of Max MB processing rate for diff levels */
#define H264_MB_PER_SEC_L1    1485
#define H264_MB_PER_SEC_L1b   1485
#define H264_MB_PER_SEC_L11   3000
#define H264_MB_PER_SEC_L12   6000
#define H264_MB_PER_SEC_L13   11880
#define H264_MB_PER_SEC_L2    11880
#define H264_MB_PER_SEC_L21   19800
#define H264_MB_PER_SEC_L22   20250

/* List of SPACE LIMIT THRESHOLD value's options */
#define CAMCORDER_SPACE_LIMIT_DEFAULT 0
#define CAMCORDER_SPACE_LIMIT_LOW     1
#define CAMCORDER_SPACE_LIMIT_MED     2
#define CAMCORDER_SPACE_LIMIT_HIGH    3

/* <EJECT> */
/*===========================================================================

                        DATA TYPE DECLARATIONS

===========================================================================*/

/* This enumerated type lists the different video encode engine command
** codes.
*/
typedef enum {
    VIDEO_ENG_CMD_INIT,
    VIDEO_ENG_CMD_START,
    VIDEO_ENG_CMD_GET_CONFIG,
    VIDEO_ENG_CMD_SET_CONFIG,
    VIDEO_ENG_CMD_WRITE_UUID,
    VIDEO_ENG_CMD_PAUSE,
    VIDEO_ENG_CMD_RESUME,
    VIDEO_ENG_CMD_STOP,
    VIDEO_ENG_CMD_EXIT,
    VIDEO_ENG_CMD_FAILURE,
    VIDEO_ENG_CMD_ENC_STATUS,
    VIDEO_ENG_CMD_CMX_STATUS,
    VIDEO_ENG_CMD_PROCESS_VIDEO,
    VIDEO_ENG_CMD_WRITE_VIDEO_STREAM,
    VIDEO_ENG_CMD_WRITE_AUDIO_STREAM,
    VIDEO_ENG_CMD_WRITE_TEXT_STREAM,
    VIDEO_ENG_CMD_WRITE_SEGMENT_HEADER,
    VIDEO_ENG_CMD_INVALID   /* (Leave Last) Invalid Command - Causes Error */
} video_eng_cmd_type;

/* Header for video encode engine commands
*/
typedef struct {
  q_link_type         link;            /* Command queue link               */
  video_eng_cmd_type  cmd;             /* CMX command to perform           */
} video_eng_hdr_type;

/* Base packet type for video encode engine commands that use standard CB
** function and have no parameters.
*/
typedef struct {
  video_eng_hdr_type       hdr;              /* Command Header             */
  video_cb_func_ptr_type   cb_func;          /* Callback function pointer  */
  void                     *client_data;     /* Client data                */
} video_eng_base_packet_type;

/* Structure used for VIDEO_ENG_CMD_INIT to initialize engine. */
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header             */
  video_init_type              args;         /* Arguments to initialize    */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                         *client_data; /* Client data                */
} video_eng_init_packet_type;

/* Structure used for VIDEO_ENG_CMD_GET_CONFIG to get config data. */
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header             */
  video_config_type            *p_args;      /* Pointer to config data     */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                         *client_data; /* Client data                */
} video_eng_get_config_packet_type;

/* Structure used for VIDEO_ENG_CMD_SET_CONFIG to set config data. */
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header             */
  video_config_type            args;         /* Arguments for config       */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                         *client_data; /* Client data                */
} video_eng_set_config_packet_type;

/* This structure is used to form linked lists of user data. */
typedef struct video_eng_udat_list_type_t {
  struct video_eng_udat_list_type_t *next;
  uint8                             uuid [16];  /* UUID of user data atom   */
  uint8                             *data;      /* buffer of user data      */
  uint32                            size;       /* size of user data        */
} video_eng_udat_list_type;

/* Structure used for VIDEO_ENG_CMD_WRITE_UUID to write a user data atom to
** the movie file about to be recorded.
*/
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header             */
  video_eng_udat_list_type     *udat;        /* User data                  */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                         *client_data; /* Client data                */
} video_eng_write_uuid_packet_type;

/* Structure used for VIDEO_ENG_CMD_ENC_STATUS to indicate a status change
** from the video encoder interface.
*/
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header             */
  video_status_type            status;       /* Video Encoder status       */
} video_eng_enc_status_packet_type;

/* Structure used for VIDEO_ENG_CMD_CMX_STATUS to indicate a status change
** from the CMX interface.
*/
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header             */
  cmx_status_type              status;       /* CMX status                 */
} video_eng_cmx_status_packet_type;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* Structure used for VIDEO_ENG_CMD_PROCESS_VIDEO and
** VIDEO_ENG_CMD_PROCESS_AUDIO to indicate more of the video or audio
** bitstream has been received from the video encoder or CMX layer.
*/
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header                          */
  uint32                       len;          /* Length of encoded data                  */
  boolean                      sync;         /* TRUE = I-frame                          */
  int32                        delta;        /* time difference between t-1 and t frame */
} video_eng_process_packet_type;

/* Structure used for VIDEO_ENG_CMD_FAILURE to indicate a failure in video
 ** engine.
 */
typedef struct {
  video_eng_hdr_type           hdr;          /* Command Header             */
  boolean                      stop_audio;       /* Video Encoder status       */
} video_eng_failure_packet_type;

/* Structure used for VIDEO_ENG_CMD_WRITE_VIDEO_STREAM to write video stream
** to the movie file in post editing recording.
*/
typedef struct video_eng_write_vstream_packet_struct
{
  video_eng_hdr_type           hdr;          /* Command Header             */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                        *client_data;  /* Client data                */
} video_eng_write_vstream_packet_type;


/* Structure used for VIDEO_ENG_CMD_WRITE_AUDIO_STREAM to write audio stream to
** the movie file in post editing recording.
*/
typedef struct video_eng_write_astream_packet_struct
{
  video_eng_hdr_type           hdr;          /* Command Header             */
  uint32                       len;          /* Length of encoded data     */
  uint8                       *data;         /* time diff btw t-1 & t frame */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                        *client_data;  /* Client data                */
} video_eng_write_astream_packet_type;

/* Structure used for VIDEO_ENG_CMD_WRITE_TEXT_STREAM to write text stream to
** the movie file in post editing recording.
*/
typedef struct video_eng_write_tstream_packet_struct
{
  video_eng_hdr_type           hdr;          /* Command Header             */
  uint32                       num;          /* number of samples          */
  video_fmt_sample_info_type  *info;         /* sample info                */
  uint8                       *data;         /* sample data                */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                        *client_data;  /* Client data                */
} video_eng_write_tstream_packet_type;

/* Structure used for VIDEO_ENG_CMD_WRITE_SEGMENT_HEADER to write text stream
** segment header to the movie file in post editing recording.
*/
typedef struct video_eng_write_segment_header_packet_struct
{
  video_eng_hdr_type           hdr;          /* Command Header             */
  video_fmt_stream_type        type;         /* stream type                */
  uint32                       size;         /* size of data               */
  uint8                       *data;         /* segment header data        */
  video_cb_func_ptr_type       cb_func;      /* Callback function pointer  */
  void                        *client_data;  /* Client data                */
} video_eng_write_segment_header_packet_type;

/* This union combines together the different types of video engine
** packets.
*/
typedef union {
  video_eng_hdr_type                hdr;       /* Command Header            */
  video_eng_base_packet_type        base;      /* Base packet type          */
  video_eng_init_packet_type        init;      /* Used to initialize engine */
  video_eng_failure_packet_type     failure;   /* Used to send the failure  */
  video_eng_get_config_packet_type  get_config;/* Used to get config data   */
  video_eng_set_config_packet_type  set_config;/* Used to set config data   */
  video_eng_write_uuid_packet_type  write_uuid;/* For writing UUID atoms    */
  video_eng_enc_status_packet_type  enc_stat;  /* Used to update enc status */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
  video_eng_cmx_status_packet_type  cmx_stat;  /* Used to update CMX status */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
  video_eng_process_packet_type     process;   /* Used to process data      */
                                               /* For writing video stream  */
  video_eng_write_vstream_packet_type  write_vstream;
                                               /* For writing audio stream  */
  video_eng_write_astream_packet_type  write_astream;
                                               /* For writing text stream  */
  video_eng_write_tstream_packet_type  write_tstream;
                                               /* For writing text stream
                                                  segment header */
  video_eng_write_segment_header_packet_type  write_segment_header;
} video_eng_packet_type;

/* This enumerated type lists all the different states of the video encode
** engine.
*/
typedef enum {
  VIDEO_ENG_STATE_INIT,       /* In the process of initializing            */
  VIDEO_ENG_STATE_IDLE,       /* Ready to begin recording                  */
  VIDEO_ENG_STATE_STARTING,   /* In the process of starting recording      */
  VIDEO_ENG_STATE_RECORDING,  /* Currently recording video/audio           */
  VIDEO_ENG_STATE_PAUSING,    /* In the process of pausing recording       */
  VIDEO_ENG_STATE_PAUSED,     /* Currently recording but paused            */
  VIDEO_ENG_STATE_RESUMING,   /* In the process of resuming recording      */
  VIDEO_ENG_STATE_STOPPING,   /* In the process of stopping recording      */
  VIDEO_ENG_STATE_ABORTING,   /* In the process of aborting recording      */
  VIDEO_ENG_STATE_EXITING,    /* In the process of cleaning up             */
  VIDEO_ENG_STATE_INVALID     /* last - invalid state, indicates error     */
} video_eng_state_type;

/* This enumerated type lists the different priorities clients of
** video_eng_cmd_get_pkt can specify when requesting a command queue packet.
*/
typedef enum {
    VIDEO_ENG_QUEUE_PRI_NORMAL,
    VIDEO_ENG_QUEUE_PRI_URGENT,
      VIDEO_ENG_QUEUE_PRI_CRITICAL,
    VIDEO_ENG_QUEUE_PRI_INVALID   /* (Leave Last) Invalid - Causes Error   */
} video_eng_queue_priority_type;

/* This structure is used to form linked lists of timed text sample entries. */
typedef struct video_eng_text_sample_entry_type_t {
  struct video_eng_text_sample_entry_type_t  *next;
  video_fmt_text_type                        entry;
} video_eng_text_sample_entry_type;

/* This structure is used to form linked lists of timed text samples. */
typedef struct video_eng_text_sample_type_t {
  struct video_eng_text_sample_type_t  *next;
  void                                 *handle;
  uint32                               start;
  uint32                               end;
  int                                  sample_entry;  /* one-based */
} video_eng_text_sample_type;

/* This structure stores variable needed in video_eng_write_video_stream */
typedef struct video_eng_write_vstream_type_t
{
  uint32                      curr_sample;
  uint32                      total_sample;
  uint32                      offset_base;
  video_fmt_sample_info_type *sample_info;
  uint8                      *sample_data;
  uint8                      *buf_ptr;
  uint32                      buf_size;
} video_eng_write_vstream_type;

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
#ifdef FEATURE_CAMCORDER_AAC_REC
/* This is the AAC sampling rate index. */
typedef enum {
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_96000 = 0,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_88200 = 1,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_64000 = 2,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_48000 = 3,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_44100 = 4,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_32000 = 5,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_24000 = 6,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_22050 = 7,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_16000 = 8,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_12000 = 9,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_11025 = 10,
  VIDEO_ENG_AAC_SAMPLE_RATE_INDEX_8000  = 11
} video_enc_aac_sample_rate_index_type;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/*
**  backend:   IDLE  --->  BUSY ---> DONE ---> IDLE
**
**             IDLE ---> BUSY        video_eng_frame with a raw input
**             BUSY ---> DONE        frame done without error
**             BUSY ---> FAILED      frame done with error
**             DONE ---> IDLE        at Handshake
**             FAILED /
**
**  encoder:   IDLE ---> READY ---> WAITING --> BUSY --> DONE ---> READY ---->
**
**            IDLE ---> READY       Caused by DecodeDone filling PingPong Buffer.
**                                  Happens at Handshake upon successful be_state
**                                     transition to DONE
**            READY --> WAITING     Start of cycle, waiting for encoder to start
**            WAITING --> BUSY      Start of video_enc_frame()
**            BUSY --> DONE         Return from video_enc_frame()
**            DONE --> READY        Before handshake, BE success.
**                  \  IDLE         If BE failed before handshake.
*/
typedef enum
{
  VIDOE_ENG_RAW_ENCODE_INVALID = 0x00,
  VIDEO_ENG_RAW_ENCODE_IDLE    = 0x01,
  VIDEO_ENG_RAW_ENCODE_READY   = 0x02,
  VIDEO_ENG_RAW_ENCODE_WAITING = 0x04,
  VIDEO_ENG_RAW_ENCODE_DONE    = 0x08,
  VIDEO_ENG_RAW_ENCODE_BUSY    = 0x10,
  VIDEO_ENG_RAW_ENCODE_FAILED  = 0x20
} video_eng_raw_encode_state_type;

/* This type specifies the storage medium used for movie file
*/
typedef enum {
  VIDEO_ENG_DEST_FLASH,            /* Save location Flash memory */
  VIDEO_ENG_DEST_MMC,              /* Save location MMC */
  VIDEO_ENG_DEST_FTL,              /* Save location FTL drive*/
  VIDEO_ENG_DEST_USB              /* Save location USB MSD */
}video_eng_save_location_type;


typedef struct
{
  uint16                   encode_buffer[8];
  video_cb_func_ptr_type   client_cb;
  void *                   client_data;
  uint32                   msec;
}video_eng_enc_pingpong_type;

typedef struct
{
  uint8        first_audio_cb_received;
  uint32       timelog_vfeConfig_cmd;
  uint32       vfe_config_delay;
  uint32       skip_audio_pkts;
  uint32       skip_audio_sum_ms;
  uint32       skip_audio_sum_ms_prev;
  uint32       last_skip_ms;
  uint32       last_skip_num_fr;
  uint32       audio_sync_error_adjust;
}avSync_type;

/* This structure tracks the frame drops and the delta caused by frame
drop */
typedef struct
{
  uint32 prev_encode_frame_time;         /*Time stamp of prev encoded frame*/
  uint32 frame_drop_duration;            /*Total time elapsed in ms from   
                                            previous encoded frame         */
  uint32 frame_drop_duration_before_pause;/*Total duration of frame drop   
                                            just before pausing            */
  uint32 first_frame_default_delta;      /*Typically video_time_increment  */
  boolean first_frame_encode_pending;    /*Waiting to encode first frame   */
  boolean first_souce_frame_dropped;     /*First frame while resume dropped*/  
}video_eng_drop_frame_time_info_type;


/* This structure contains all the variables for the video encode engine.
*/
typedef struct {
  /*-------------------------------------------------------------------------
  ** Engine Critical Section - should be held in parts of the engine that
  ** are not inherently thread-safe.
  **-------------------------------------------------------------------------
  */
  rex_crit_sect_type    cs;

  /*-------------------------------------------------------------------------
  ** Engine Command Queue, Free Queue and Available Packets
  **-------------------------------------------------------------------------
  */
  q_type                cmd_q;    /* Request Packets Queue   */
  q_type                free_q;   /* Available Packets Queue */
  video_eng_packet_type free_packet [VIDEO_ENG_NUM_PACKETS];


  /*-------------------------------------------------------------------------
  ** Encoder Specific Internal Memoryj --- as specified by the encoder
  **-------------------------------------------------------------------------
  */
  void *encoder_internal_memory;   /* encoder specific required memory */

  /*-------------------------------------------------------------------------
  ** Engine State
  **-------------------------------------------------------------------------
  */
  video_eng_state_type  state;

  /*-------------------------------------------------------------------------
  ** Video frame width and height
  **-------------------------------------------------------------------------
  */
  uint16  frame_width;      /* frame width          */
  uint16  frame_height;     /* frame height         */

  /*-------------------------------------------------------------------------
  ** Buffers used by Video Encoder
  **-------------------------------------------------------------------------
  */
  uint8  *enc_pkt_ptr1;        /* encoded frame buf1    */
  uint8  *enc_pkt_ptr2;        /* encoded frame buf2    */
  uint8  *vop_recon_buf_ptr1;    /* recon frame buf1      */
  uint8  *vop_recon_buf_ptr2;    /* recon frame buf2      */
  uint8  *ved_buf_ptr;           /* video enc output data */
  uint32 enc_pkt_size;         /* Size of the video pkt */
#ifdef FEATURE_VIDEO_ENCODE_DIAG
#error code not present
#endif /* FEATURE_VIDEO_ENCODE_DIAG */

  /*-------------------------------------------------------------------------
  ** Callback information given by client
  **-------------------------------------------------------------------------
  */
  video_cb_func_ptr_type  callback_ptr;
  void                    *client_data;
  video_eng_size_cb_func_ptr_type  size_cb;      /* indicates size file  */
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
  video_eng_cmx_data1_cb_func_ptr_type audio_data1_cb; /* First data cb indication from CMX */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
#ifdef FEATURE_VIDEO_ENCODE_DIAG
#error code not present
#endif /* FEATURE_VIDEO_ENCODE_DIAG */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  /*-------------------------------------------------------------------------
  ** Engine Terminate Flag - when set, the engine will deallocate its context
  ** variables and stop running until video_eng_init is called once again.
  **-------------------------------------------------------------------------
  */
  boolean  terminate;

  /*-------------------------------------------------------------------------
  ** File format - this stores the file format, i.e. MP4, AVI, ...
  **-------------------------------------------------------------------------
  */
  video_fmt_type        file_format;

  /*-------------------------------------------------------------------------
  ** Brand indicator - this stores the particular brand of MP4 file chosen by
  ** the client.
  **-------------------------------------------------------------------------
  */
  video_fmt_brand_type  file_brand;

  /*-------------------------------------------------------------------------
  ** Format indicators - these store the identifiers of the codecs
  ** used, if any.
  **-------------------------------------------------------------------------
  */
  video_fmt_stream_audio_type  audio_format;
  uint16                       audio_rate_set;
  video_fmt_stream_video_type  video_format;
  boolean                      still_image;
  boolean                      no_interlace;

  /*-------------------------------------------------------------------------
  ** Movie size limit - this stores the maximum movie size and maximum movie
  ** duration.  This is ignored if this == 0.
  **-------------------------------------------------------------------------
  */
  uint32  movie_size_limit;
  uint32  movie_duration_limit;

  /*-------------------------------------------------------------------------
  ** Video frame time increment, resolution and timescale - these store the
  ** rate at which the video frames will be generated.
  **
  **-------------------------------------------------------------------------
  */
  uint32  video_time_increment;
  uint32  video_time_resolution;
  uint32  media_timescale;

  /*-------------------------------------------------------------------------
  ** Audio sample size - this is the fixed size of each packet of audio data
  ** recorded by CMX.
  **------------------------------------------------------------------------
  */
  uint32  audio_sample_size;

  /*-------------------------------------------------------------------------
  ** Audio time resolution - this is typically audio sampling rate.
  **------------------------------------------------------------------------
  */
  uint32  audio_time_resolution;

  /*-------------------------------------------------------------------------
  ** Audio sample delta - this is the fixed distance between sequential
  ** audio packets.
  **------------------------------------------------------------------------
  */
  uint32  audio_sample_delta;

  /*-------------------------------------------------------------------------
  ** Video sample delta - this is the fixed distance between sequential video
  ** frames.
  **------------------------------------------------------------------------
  */
  uint32  video_sample_delta;

  /*-------------------------------------------------------------------------
  ** Repeat video header - this flag, when set, causes the video decoder
  ** specific information to be repeated in the first video frame in the
  ** bitstream.
  **------------------------------------------------------------------------
  */
  boolean  video_repeat_header;

  /*-------------------------------------------------------------------------
  ** Failure on Exit - if set, the engine will return a VIDEO_FAILURE status
  ** once it is finished exiting.  Otherwise, a normal exit will return a
  ** VIDEO_SUCCESS status.
  **-------------------------------------------------------------------------
  */
  boolean  failure_on_exit;

  /*-------------------------------------------------------------------------
  ** Delete on Exit - if set, the engine will remove the file it was recording
  ** once it is finished exiting (as long as "save_on_exit" is not also set.)
  **-------------------------------------------------------------------------
  */
  boolean  delete_on_exit;

  /*-------------------------------------------------------------------------
  ** Save on Exit - if set, the engine will attempt to save what it has of the
  ** movie it was recording, before exiting.  Otherwise, whatever has been
  ** saved so far will be abandoned unless the engine goes through the normal
  ** stop procedure.
  **-------------------------------------------------------------------------
  */
  boolean  save_on_exit;

  /*-------------------------------------------------------------------------
  ** Space low flags - these are set once the chunk processing determines that
  ** there is probably less than VIDEO_ENG_LIMIT_NEAR_THRESHOLD or
  ** VIDEO_ENG_LIMIT_IMMINENT_THRESHOLD seconds of recording time left before
  ** either the sample table memory or the filesystem space is exhausted.
  **-------------------------------------------------------------------------
  */
  boolean  space_out_near;
  boolean  space_out_imminent;

  /*-------------------------------------------------------------------------
  ** Space out flag - this is set if the engine runs out of space in the chunk
  ** or sample table, or it determines that there is probably less than
  ** VIDEO_ENG_LIMIT_REACHED_THRESHOLD seconds of recording time left, and has
  ** to stop recording on its own.
  **-------------------------------------------------------------------------
  */
  boolean  space_out;
  video_fmt_space_out_type space_out_status;

  /*-------------------------------------------------------------------------
  ** stream buffer out flag - this is set if the engine runs out of space in
  ** one of the videofmt stream buffers.
  **-------------------------------------------------------------------------
  */
  boolean  buffer_out;

  /*-------------------------------------------------------------------------
  ** VS packet out flag - this is set if there is a shortage of VS packets
  ** during video recording.
  **-------------------------------------------------------------------------
  */
  boolean  packet_out;

  /*-------------------------------------------------------------------------
  ** First audio packet dropped flag - this is set once the first audio packet
  ** is dropped.  We drop the first audio packet as it may be corrupted.
  **-------------------------------------------------------------------------
  */
  boolean  first_audio_packet_dropped;

  /*-------------------------------------------------------------------------
  ** Output specifications - these variables determine how to output the
  ** encoded movie, and any pertinent state information.
  **-------------------------------------------------------------------------
  */
  video_handle_type  output;
  uint32             output_offset;
  uint32             output_size;
#ifdef FEATURE_EFS2
  VIDEO_ENG_FILE     *fs_handle;
#endif /* FEATURE_EFS2 */
  boolean            output_open;
  boolean            use_temp_file_1;
  boolean            use_temp_file_2;
  boolean            use_temp_file_3;

  /*-------------------------------------------------------------------------
  ** Fragment variables - these track important fragment-specific information
  ** when writing a fragmented movie.
  **-------------------------------------------------------------------------
  */
  uint32             frag_in_offset;
  uint32             frag_out_offset;
  uint32             frag_total_duration;

#ifdef FEATURE_EFS2
  /*-------------------------------------------------------------------------
  ** Temporary output files - if a file fixup is needed after recording the
  ** movie file, this is the temporary file used to record the initial movie.
  ** This is used for AMC files and also used for fragmented 3G2 files.  For
  ** AMC files, the whole file is stored first and then reordered.  For
  ** fragmented 3G2 files, the temporary file is used only to reorder one
  ** fragment at a time.  The second temporary file is used only for
  ** fragmented 3G2 files when the final destionation is not a file.
  **-------------------------------------------------------------------------
  */
  VIDEO_ENG_FILE     *temp_handle_1;
  VIDEO_ENG_FILE     *temp_handle_2;
  VIDEO_ENG_FILE     *temp_handle_3;

  /*-------------------------------------------------------------------------
  ** Temporary output file names - these are used to hold pointers to the
  ** temporary file name strings either on EFS or on MMC.
  **-------------------------------------------------------------------------
  */
  char               temp_file_name_1[FS_FILENAME_MAX_LENGTH_P];
  char               temp_file_name_2[FS_FILENAME_MAX_LENGTH_P];
  char               temp_file_name_3[FS_FILENAME_MAX_LENGTH_P];

  /*-------------------------------------------------------------------------
  ** Temporary output file sizes - these are used to hold size of temporary
  ** files on EFS to keep track of free space on EFS at any given moment.
  **-------------------------------------------------------------------------
  */
  uint32             temp_file_size;
  uint32             temp_file_size_3;

  /*-------------------------------------------------------------------------
  ** Temporary output file size factor - each of these is used to calculate
  ** the space for temp_file_1 and temp_file_2 on flash and on mmc
  ** respectively.
  ** E.g.,
  ** temp_file_size_factor =
  **    2 if both temp_file_1 and temp_file_2 are on flash.
  **    1 if temp_file_1 is on flash and temp_file_2 is on mmc or not needed.
  **    1 if temp_file_1 is on memory and temp_file_2 is on flash.
  **    0 if temp_file_1 is on memory and temp_file_2 is on mmc or not needed.
  ** mmc_temp_file_size_factor =
  **    2 if both temp_file_1 and temp_file_2 are on mmc.
  **    1 if temp_file_1 is on mmc and temp_file_2 is not needed.
  **    1 if temp_file_1 is on memory or flash and temp_file_2 is on mmc.
  **    0 if temp_file_1 is on memory or flash and temp_file_2 is not needed.
  **-------------------------------------------------------------------------
  */
  uint32             temp_file_size_factor;
  /*-------------------------------------------------------------------------
  ** Temporary output file 3 size factor - each of these is used to calculate
  ** the space for temp_file_3 on flash and on mmc respectively.
  ** E.g.,
  **    temp_file_size_factor_3 = 1
  **    mmc_temp_file_size_factor_3 = 0
  **    if temp_file_3 is on flash.
  **-------------------------------------------------------------------------
  */
  uint32             temp_file_size_factor_3;
  /*-------------------------------------------------------------------------
  ** Free space in EFS - this represents the initial free space in flash when
  ** video recording gets started.
  **-------------------------------------------------------------------------
  */
  uint64           free_space;

  /*-------------------------------------------------------------------------
  ** Current save location for recorded clip
  **-------------------------------------------------------------------------
  */
  video_eng_save_location_type    save_location;
#endif /* FEATURE_EFS2 */

  /*-------------------------------------------------------------------------
  ** Video prev frame size and sync - this saves the size of the prev
  ** frame received from the encoder.  The last frame is not written out until
  ** the movie file is closed, so that an end stream sequence can be written
  ** to it.
  **-------------------------------------------------------------------------
  */
  uint32   video_prev_frame_size;
  uint32   video_prev_frame_delta;
  boolean  video_prev_frame_sync;

  /*-------------------------------------------------------------------------
  ** Audio/Video synchronization - these flags are used to record when the
  ** audio and video parts of a command have been completed, and whether or
  ** not there were error(s) on either side.
  **-------------------------------------------------------------------------
  */
  boolean  video_done;
  boolean  video_error;
  boolean  audio_done;
  boolean  audio_error;

  /*-------------------------------------------------------------------------
  ** Video format services - these variables contain state information
  ** relating to the usage of the video format services.
  **-------------------------------------------------------------------------
  */
  video_fmt_status_type                videofmt_status;
  void                                 *videofmt_server_data;
  video_fmt_continue_cb_func_type      videofmt_continue;
  video_fmt_end_cb_func_type           videofmt_end;
  video_fmt_write_cb_func_type         videofmt_write_stream;
  video_fmt_write_text_cb_func_type    videofmt_write_text;
  video_fmt_write_header_cb_func_type  videofmt_write_header;
  video_fmt_write_footer_cb_func_type  videofmt_write_footer;
  video_fmt_write_header_text_cb_func_type  videofmt_write_header_text;
  video_fmt_write_uuid_cb_func_type    videofmt_write_uuid;
  video_fmt_seek_cb_func_type          videofmt_seek;
  video_fmt_alloc_text_cb_func_type    videofmt_alloc_text;
  video_fmt_free_text_cb_func_type     videofmt_free_text;
  video_fmt_modify_style_text_cb_func_type  videofmt_modify_style_text;
  video_fmt_modify_hl_text_cb_func_type     videofmt_modify_hl_text;
  video_fmt_modify_link_text_cb_func_type   videofmt_modify_link_text;
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
  video_fmt_modify_user_atoms_cb_func_type  videofmt_modify_user_atoms;
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/
  boolean                              videofmt_fragment_done;

  /*-------------------------------------------------------------------------
  ** Timed Text Encoder - these variables are used to encode a timed text
  ** stream in the movie, if enabled.
  **-------------------------------------------------------------------------
  */
  char                                 *text;
  uint32                               text_size;
  video_eng_text_input_type            text_type;
  int16                                text_box_width;
  int16                                text_box_height;
  video_eng_text_sample_type           *text_samples;
  video_eng_text_sample_type           *text_samples_end;
  video_eng_text_sample_entry_type     *text_headers;
  video_eng_text_sample_entry_type     *text_headers_end;
  uint32                               text_stream_num;
  int                                  text_current_header;

  /*-------------------------------------------------------------------------
  ** Video Track Duration - this keeps track of the total duration of the
  ** video track, as it is written to the movie file.  The duration is in
  ** units of the video track time resolution.
  **-------------------------------------------------------------------------
  */
  uint32                               video_duration;

  /*-------------------------------------------------------------------------
  ** Video Track Duration - this keeps track of the total duration of the
  ** video track, as it is written to the movie file.  The duration is in
  ** units of the media_timescale resolution.
  **-------------------------------------------------------------------------
  */
  uint32                               video_media_duration;

  /*-------------------------------------------------------------------------
  ** Track Duration for A/V Sync - this keeps track of the total duration of
  ** the video track, as each encode frame is passed to the video engine.
  ** The duration is in units of the video track time resolution.
  **-------------------------------------------------------------------------
  */
  uint32                               av_sync_duration;

  /*-------------------------------------------------------------------------
  ** Video Frame Count - this keeps track of the total number of video frames
  ** received from the encoder, since encoding began.
  **-------------------------------------------------------------------------
  */
  uint32                               video_frames_received;

  /*-------------------------------------------------------------------------
  ** Audio Frames per Sample - for some MP4 brands, audio samples may contain
  ** more than one frame.  This variable is set to the number of frames to put
  ** in each audio sample.
  **-------------------------------------------------------------------------
  */
  uint32                               audio_frames_per_sample;

  /*-------------------------------------------------------------------------
  ** Audio buffer - this buffer stores left-over audio frames from CMX which
  ** did not fit into a whole number of samples when writing to the movie
  ** file.
  **-------------------------------------------------------------------------
  */
  uint8                                *audio_leftover;
  uint32                               audio_leftover_bytes;

  /*-------------------------------------------------------------------------
  ** Audio sample structures - these are used if writing audio samples that
  ** are not fixed size.  For example, when writing more than one frame per
  ** sample, where last sample may have fewer frames than the ones before it.
  **-------------------------------------------------------------------------
  */
  video_fmt_sample_info_type  audio_sample_info [MAX_AUDIO_SAMPLES_PER_WRITE];

  /*-------------------------------------------------------------------------
  ** Audio stream number - this tracks which stream contains audio, since it
  ** depends on whether or not the movie includes video.
  **-------------------------------------------------------------------------
  */
  uint32                               audio_stream_num;

  /*-------------------------------------------------------------------------
  ** Audio number of packets - this keeps track of the number of audio
  ** packets,  as audio data is written to video format services in cmx
  ** callback.
  **-------------------------------------------------------------------------
  */
  uint32                               audio_num_packets;

  /*-------------------------------------------------------------------------
  ** User data - this maintains a linked list of the user data. User data
  ** is passed to the video FMT after its creation.
  **-------------------------------------------------------------------------
  */
  video_eng_udat_list_type             *udat_head;
  video_eng_udat_list_type             *udat_tail;

#ifdef FEATURE_CAMCORDER_AAC_REC
  /*-------------------------------------------------------------------------
  ** AAC audio - these are used to set the AAC encoder.
  **-------------------------------------------------------------------------
  */
  uint32                               aac_channel_mode;
  uint32                               aac_sampling_rate_index;
  uint16                               aac_bits_per_sample;

  /*-------------------------------------------------------------------------
  ** AAC Decoder Specific Information - this flag is set once DSI is written.
  **-------------------------------------------------------------------------
  */
  boolean                              aac_dsi_written;
#endif /* FEATURE_CAMCORDER_AAC_REC */

  /*-------------------------------------------------------------------------
  ** Include "drm " atom.
  **-------------------------------------------------------------------------
  */
  boolean                      include_drm;
  uint16                       drm_distribution_rights;

  /*-----------------------------------------------------------------------
  ** For  user data atom 'titl', 'auth', 'dscp', 'loci' atom.
  **-------------------------------------------------------------------------
  */
  boolean 						 include_user_atom; /* To include the atom or not */
  video_eng_user_data_atom_type  useratom_data;  /* Actual data in user atom */

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
  video_eng_user_atoms_client_cb useratom_cbptr; /* Call back ptr for user atom */
#endif
  /*-------------------------------------------------------------------------------
  ** For reording moov and mdata atoms in bitstream according to KDDI specification
  **-------------------------------------------------------------------------------
  */

   boolean                      reorder_atom;   /* moov data atom reorder*/

  /*-------------------------------------------------------------------------
  ** Video encoder target bitrate.
  **-------------------------------------------------------------------------
  */
  uint32                       video_target_bitrate;

  /*-------------------------------------------------------------------------
  ** Audio settings - these are audio target bit rate, number of audio
  ** channels, sampling frequency, and the number of PCM samples per audio
  ** frame.
  ** 'audio_samples_per_frame' is not related to 'audio_frames_per_sample'.
  **-------------------------------------------------------------------------
  */
  uint32                       audio_target_bitrate;
  uint32                       audio_channels;
  uint32                       audio_sampling_frequency;
  uint32                       audio_samples_per_frame;
  avSync_type                  avSync_info;


  /*------------------------------------------------------------------------
  ** This tracks the number of frames being dropped and the total time
  ** delta caused by frame drops.
  **------------------------------------------------------------------------
  */
  video_eng_drop_frame_time_info_type frame_drop_info;


  /*-------------------------------------------------------------------------
  ** Video stream number - this tracks which stream contains video.
  **-------------------------------------------------------------------------
  */
  int                          video_stream_num;

#ifdef FEATURE_VIDEO_CLIP_INFO
  /*-------------------------------------------------------------------------
  ** Qcom Platform specific data - this maintains the information of various
  ** data which is passed to the video FMT at the end of recording.
  **-------------------------------------------------------------------------
  */
  video_fmt_clipinfo_type          clipinfo;
#endif //FEATURE_VIDEO_CLIP_INFO

   uint32 video_start_time;              /* video start time */
   
   boolean dynamic_bitrate_enable;       /* whether external bitrate control*/
                                         /* mode should be enabled          */
   uint32 dynamic_bitrate;               /* user_bitrate                    */

   boolean max_bitrate_enable;

  vencoder_frame_done_type     *pEncFrame;
  vencoder_frame_cb_fn         framecb_ptr;
  uint8                        *client_to_enc_buffer;
  uint32                       enc_buffer_size;
  /*-------------------------------------------------------------------------
  ** No coding - this keeps track whether it's a real-time recording or
  ** post editing recording.
  **-------------------------------------------------------------------------
  */
  boolean                              no_coding;

  boolean                              adjust_es_time;

  video_eng_write_vstream_type         wvs;
  /* Space limit threshold and imminent threshold value. *
   * Both variables will be set according to UI inputs   */
  uint8                        space_limit_threshold;
  uint8                        space_imminent_threshold;

/*-------------------------------------------------------------------------
  ** Fragment sequence number for fragmented format.
  **-------------------------------------------------------------------------
  */
  uint32                       fragment_number;
} video_eng_context_type;


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API

/* return messages for video_eng_frame related routines */
typedef enum {
  VIDENG_FRAME_SUCCESS,

  /* attempt to init w/o first a video_eng_init */
  VIDENG_FRAME_INIT_BAD_CONTEXT,

  /* attmpt to init when video_eng_frame is already initialized */
  VIDENG_FRAME_SECOND_INIT,

  /* malloc failed to initailize context structure */
  VIDENG_FRAME_MALLOC_CONTEXT,

  /* video_eng_frame_terminate called when its not initialized */
  VIDENG_FRAME_ALREADY_TERMINATED,

  /* video_eng_frame_terminate cannot complete until processes complete*/
  VIDENG_FRAME_TERMINATE_REQUESTED,

  /* another failure after the clock started */
  VIDENG_FRAME_CLOCKED_TERMINATE_ALREADY_SET,

  VIDENG_FRAME_MAX
} video_eng_frame_stat_type;


/* With video_eng_frame() we introduce GRAPH task commands in video_eng.
** This is a list of commands executed by the video_eng in the GRAPH
** task.
*/

typedef enum {
  VIDENG_GRAPH_FRAME_CMD,              /* process video_encode_frame */
  VIDENG_GRAPH_FRAME_DONE,             /* frame doen */
  VIDENG_GRAPH_HANDSHAKE,              /* send handshake to client */
  VIDENG_GRAPH_BE_FAILURE,             /* frame done w/ error state */
  VIDENG_GRAPH_DSP_ERROR_AHB_OVERFLOW, /* Error from BE */
  VIDENG_GRAPH_DSP_ERROR_ILLEGAL_CMD,  /* Error from BE */
  VIDENG_GRAPH_INIT,                   /* unused */
  VIDENG_GRAPH_INIT_ERROR,             /* unused */
  VIDENG_GRAPH_ERROR,                  /* error condition */
  VIDENG_GRAPH_SELECTION_DONE,         /* unused */
  VIDENG_GRAPH_SELECTION_ERROR,        /* unused */
  VIDENG_GRAPH_SLEEP_DONE,             /* unused */
  VIDENG_GRAPH_FRAME_FAIL,             /* unused */
  VIDENG_GRAPH_CANNOT_START_FRAME,     /* unable to start video_eng_frame*/
  VIDENG_GRAPH_RAW_FRAME_FAILED,       /* VideoEncDSP_VideoEncRawFrame failed*/
  VIDENG_GRAPH_FRAME_CALLED_BEFORE_HANDSHAKE,
                                       /* called video_eng_frame before handshake */
  VIDENG_GRAPH_FRAME_ISR_DURING_REX_LOCK,
                                       /* video_eng_frame() called from ISR */
                                       /*   during critical section lock    */
  VIDENG_GRAPH_TIMER_CB,
  VIDENG_GRAPH_MAX
} graph_video_eng_msg_enum;

/* This contains all the global variables for video_eng_frame
*/
typedef struct {
  rex_crit_sect_type                cs;

  /* cache the 3 parameter supplied to video_eng_frame.
  ** can be updated in the ISR context.
  ** the 3 parameters are callback function, client_data, and
  ** frame time in msec.
  */
  video_cb_func_ptr_type            raw_encode_client_cb;
  void                             *raw_encode_client_data;
  uint32                            raw_encode_msec;

  /* backend state. please refer to state diagram */
  video_eng_raw_encode_state_type   be_state;

  /* encoder state. please refer to state diagram */
  video_eng_raw_encode_state_type   enc_state;

  /* The ping pong buffer store the output DSP (lossy) encode from the previous
  ** frame (time T-1) and use that as input to the ARM (lossless) encode frame
  ** (time T).  Simultaneously it provides output for the DSP processing of
  ** current input frame at time T.
  */
  video_eng_enc_pingpong_type       enc_pingpong_bufs[2];

  /* points to which pingpong buffer is to be used to store the output of the
  ** current DSP processing (time T).  The other pingpong location (location
  ** 1-next_enc_pingpong_location) is the time T-1 frame.
  */
  uint16                            next_enc_pingpong_location;

  /* if tree it means a video_eng_frame_terminate attempted a terminate but
  ** failed.  The handshake should check this flag to make another terminate
  ** attempt.
  */
  boolean                           terminate;

  /* termination timer.   If termination failed Because backend or encoder is
  ** busy then set a time. If timer returns and we still didn't finish then
  ** terminate anyway
  */
  clk_cb_type                       clk;

  /* give the back end a time limit to finish encoding a frame */
  clk_cb_type                       backend_timeout_clk;

  /* Flag to indicate lossless encoding done so we can cancel timer */
  boolean                           be_timer_blocked;

} video_eng_frame_context_type;

#endif  // #ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API


/* <EJECT> */
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
/*===========================================================================

FUNCTION graph_video_eng_queue_cmd

DESCRIPTION
  This function sends a command to the graph task requesting that it
  apply a pending video engine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void graph_video_eng_queue_cmd( graph_video_eng_msg_enum msg,
                                       void *response);
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_graph_cmd_parser

DESCRIPTION
  Graph-task command parser to process commands sent from the DSP interface
  layer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_graph_cmd_parser(graph_video_eng_msg_enum msg,
                                const void *response
                               );
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmd_parser

DESCRIPTION
  This procedure processes requests received by video_eng_cmd().  See
  videoengi.h for a description of the actual Video Engine packets.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  video_eng_init() must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  cmd_q

===========================================================================*/
extern void video_eng_cmd_parser (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmd

DESCRIPTION
  Queue a command for processing by the Video Engine.

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  video_eng_init() needs to have initialized the queues.

RETURN VALUE
  None

SIDE EFFECTS
  video_eng_cmd_q
  vs_tcb

===========================================================================*/
extern void video_eng_cmd
(
  video_eng_packet_type *video_eng_cmd_ptr
    /* pointer to VS command */
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmd_get_pkt

DESCRIPTION
  Get an Engine Command Packet.  Use by callers of video_eng_cmd().

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  video_eng_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available video_eng_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  video_eng_free_q (and the packet fetched off video_eng_free_q)

===========================================================================*/
extern video_eng_packet_type *video_eng_cmd_get_pkt (
  video_eng_queue_priority_type  priority
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_init_processing

DESCRIPTION
  This function initializes the video and audio encoders in preparation for
  recording a video file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_init_processing (
  const video_init_type  *args
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_start_processing

DESCRIPTION
  This function starts recording a video file, with parameters given during
  initialization time.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_start_processing (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_get_config_processing

DESCRIPTION
  This function gets the video engine config data.

DEPENDENCIES
  video_eng_init_processing() should be called first.
  video_eng_start_processing() must not have been called since the last time
  video_eng_init_processing() was called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_get_config_processing (
  video_config_type  *args
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_set_config_processing

DESCRIPTION
  This function sets the video engine config data.

DEPENDENCIES
  video_eng_init_processing() should be called first.
  video_eng_start_processing() must not have been called since the last time
  video_eng_init_processing() was called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_set_config_processing (
  video_config_type  *args
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_write_uuid_processing

DESCRIPTION
  This function writes a user data atom to the video file.

DEPENDENCIES
  video_eng_init_processing() should be called first.
  video_eng_start_processing() must not have been called since the last time
  video_eng_init_processing() was called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_write_uuid_processing (
  video_eng_udat_list_type     *udat
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_pause_processing

DESCRIPTION
  This function pauses recording the current video file being recorded.

DEPENDENCIES
  video_eng_start_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_pause_processing (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_resume_processing

DESCRIPTION
  This function resumes recording the current video file that was recording
  but was paused.

DEPENDENCIES
  video_eng_start_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_resume_processing (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_stop_processing

DESCRIPTION
  This function stops recording a video file, writes the file meta-data, and
  closes the recorded video file.

DEPENDENCIES
  video_eng_start_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_stop_processing (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_exit_processing

DESCRIPTION
  This function cleans up the video and audio encoders after recording a video
  file.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_exit_processing (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_status_processing

DESCRIPTION
  This function processes a status change from the video encoder.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_enc_status_processing (
  video_status_type  status
);


/*===========================================================================

FUNCTION video_eng_write_vstream_processing

DESCRIPTION
  This function writes video stream in a video file.
  This is used in post editing recording

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_write_vstream_processing (video_status_type status);


/*===========================================================================

FUNCTION video_eng_write_astream_processing

DESCRIPTION
  This function writes audio stream in a video file.
  This is used in post editing recording

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_write_astream_processing (uint32 len, const uint8 *data);

/*===========================================================================

FUNCTION video_eng_write_tstream_processing

DESCRIPTION
  This function writes text stream in a video file.
  This is used in post editing recording

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_write_tstream_processing (
  uint32 num,
  const video_fmt_sample_info_type *info,
  const uint8 *data);

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_status_processing

DESCRIPTION
  This function processes a status change from CMX.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_cmx_status_processing (
  cmx_status_type  status
);
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_audio_video_synchronize

DESCRIPTION
  This function moves the state machine according to the current operation in
  progress and the current state of the audio/video synchronization
  variables.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_audio_video_synchronize (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_video_processing

DESCRIPTION
  This function processes the video bitstream after it has been updated.

DEPENDENCIES
  video_eng_init_processing() should be called first.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern video_status_type video_eng_video_processing (
  uint32   len,
  boolean  sync,
  int32    delta
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_normal_cb

DESCRIPTION
  This function handles normal status callbacks from the video encoder
  interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_enc_normal_cb (
  video_status_type  status,
  const void         *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_req_cb

DESCRIPTION
  This function handles data request callbacks from the video encoder
  interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_enc_req_cb (
  video_status_type                   status,
  const video_enc_enc_to_client_type  *enc_to_client_data,
  video_enc_client_to_enc_type        *client_to_enc_data,
  const void                          *client_data
);

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_normal_cb

DESCRIPTION
  This function handles status callbacks from the CMX interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_cmx_normal_cb (
  cmx_status_type  status,
  const void       *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_data_cb

DESCRIPTION
  This function handles data request callbacks from the CMX interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_cmx_data_cb (
  uint32                           position,
  uint32                           max_len,
  cmx_client_req_cb_func_ptr_type  data_req_cb,
  const void                       *server_client_data,
  const void                       *client_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_var_data_cb

DESCRIPTION
  This function handles data request callbacks from the CMX interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_cmx_var_data_cb (
  uint32                           position,
  uint32                           max_len,
  cmx_client_req_cb_func_ptr_type  data_req_cb,
  const void                       *server_client_data,
  const void                       *client_data
);

/*===========================================================================

FUNCTION video_eng_cmx_sync_cb_func

DESCRIPTION
  This function handles sync callbacks from the CMX interface for
  A/V Sync purpose.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_sync_cb_func(
  cmx_av_report_type *av_report,
  const void         *client_data
);

#ifdef FEATURE_CAMCORDER_AAC_REC
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_cmx_aac_data_cb

DESCRIPTION
  This function handles data request callbacks from the CMX interface for
  the AAC encoding.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_cmx_aac_data_cb (
  uint32                           position,
  uint32                           max_len,
  cmx_client_req_cb_func_ptr_type  data_req_cb,
  const void                       *server_client_data,
  const void                       *client_data
);

#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_videofmt_callback

DESCRIPTION
  This function handles callbacks from the video format services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_videofmt_callback (
  video_fmt_status_type                  status,
  void                                   *client_data,
  union video_fmt_status_cb_info_type_t  *info,
  video_fmt_end_cb_func_type             end
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_enc_failure

DESCRIPTION
  This function exits the engine with a failure status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_enc_failure (
  boolean  stop_audio
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_free_enc_buffers

DESCRIPTION
  This function frees any video encoder buffers that are still allocated.
  stream.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_free_enc_buffers (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_output

DESCRIPTION
  This function writes the given data to the output destination set up
  for the engine.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was output successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean video_eng_output (
  const uint8  *data,
  uint32       offset,
  uint32       len
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_write_meta_data

DESCRIPTION
  This function writes the given data to the temporary destination set up
  for the engine.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was written successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean video_eng_write_meta_data (
  const uint8  *data,
  uint32       len
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_read_meta_data

DESCRIPTION
  This function reads data from the temporary destination set up
  for the engine.

DEPENDENCIES
  None.

RETURN VALUE
  The value TRUE is returned if the data was read successfully.  Otherwise,
  the value FALSE is returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean video_eng_read_meta_data (
  const uint8  *data,
  uint32       offset,
  uint32       len
);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_handle_exit

DESCRIPTION
  This function finishes an exit operation after the video/audio systems
  have finished shutting down.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  g_context->terminated is set, and a callback (either VIDEO_SUCCESS or
  VIDEO_FAILURE) is sent to the client, to indicate either that a file was
  saved or not.

===========================================================================*/
extern void video_eng_handle_exit (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_handle_terminate

DESCRIPTION
  This function shuts down the video engine.  It is called when the engine is
  in the midst of terminating.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  All video engine variables, including g_context, are freed.

===========================================================================*/
extern void video_eng_handle_terminate (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_handle_close

DESCRIPTION
  This function finishes up and closes the movie file being written.  It is
  called once the video encoder has finished stopping, or the movie file is
  closed for any other reason.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
extern void video_eng_handle_close (boolean suppress_callback);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_close_writer

DESCRIPTION
  This function releases all video format services writer resources.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
extern void video_eng_close_writer (void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_reorder_fragment

DESCRIPTION
  This function reorders the fragment just written out by videofmt and passes
  the data along to its final destination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The movie file meta data is written and the file is closed.

===========================================================================*/
extern boolean video_eng_reorder_fragment (boolean);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_update_mehd_atom

DESCRIPTION
  This function locates the 'mehd' atom in the output file, and writes the
  given value in for the "fragment_duration" field of that atom.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_update_mehd_atom (uint32 total_duration);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_find_save_location

DESCRIPTION
  Determines the save location

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_find_save_location(const char *path);
/*===========================================================================
FUNCTION video_eng_write_video_stream

DESCRIPTION
  This procedure queues a command which writes a video
  stream to an output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_video_stream (
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
);

/*===========================================================================
FUNCTION video_eng_audio_stream

DESCRIPTION
  This procedure queues a command which writes an audio
  stream to an output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_audio_stream (
  uint32                           len,
  uint8                           *sample_data,
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
);

/*===========================================================================
FUNCTION video_eng_text_stream

DESCRIPTION
  This procedure queues a command which writes a text
  stream to an output file.
  It is used in post editing recording.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  cmd_q
  free_q
  free_packet

===========================================================================*/
video_status_type video_eng_write_text_stream (
  uint32                           num_samples,
  video_fmt_sample_info_type      *sample_info,
  uint8                           *sample_data,
  video_cb_func_ptr_type           callback_ptr,
  void                            *client_data
);

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_process_frame

DESCRIPTION
  VS-task process to pull a VFE frame from the VSE queue, run special, and
  send it to the backend.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_process_frame(void);

/*===========================================================================

FUNCTION video_eng_set_frame_done_buffer

DESCRIPTION
  The uint16 frame_done_buf[4] that is used to pass DSP status to the loss-
  less encoder needs to be placed in the packet of the current frame being
  processed.  This routine is called from the DSP-interface layer to
  make sure this data is not lost.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_set_frame_done_buffer_isr(uint16 *buf);

/*===========================================================================

FUNCTION video_eng_rawEncodeConfigCompleteErrorCb_isr

DESCRIPTION
  ConfigDone callback returned an error.

  This routine runs in the ISR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_rawEncodeConfigCompleteErrorCb_isr(void);

/* <EJECT> */
/*===========================================================================

FUNCTION video_eng_rawEncodeFrameDoneCb_isr

DESCRIPTION
  Decode Done callback for BackEnd initiated from video_encode_frame().

  This routine runs in the ISR context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void video_eng_rawEncodeFrameDoneCb_isr(
  CAMQDSP_ReturnCodeType result,
  CAMQDSP_ResponseType *response
);

/*===========================================================================

FUNCTION video_eng_venc_req_cb

DESCRIPTION
  This function handles data request callbacks from the video encoder
  interface.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void video_eng_venc_req_cb (
  video_status_type                   status,
  const video_enc_enc_to_client_type  *enc_to_client_data,
  video_enc_client_to_enc_type        *client_to_enc_data,
  const void                          *client_data
);

#endif /* ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

#endif /* FEATURE_VIDEO_ENCODE */

#endif /* VIDEOENGI_H */

