#ifndef _QCPLAYER_H_
#define _QCPLAYER_H_
/* =======================================================================
                               qcplayer.h
DESCRIPTION
  Common definitions for Mpeg-4 player.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/qc/main/latest/inc/qcplayer.h#20 $
$DateTime: 2010/11/09 04:48:52 $
$Change: 1509879 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
//The target version won't build unless all the standard includes come
//before any zrex includes.

//Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>

//OSCL includes
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"
#include "oscl_string.h"
#include "oscl_media_data.h"
#include "oscl_file_io.h"

//OS includes.
#include "zrex.h"

//This is the most global player include.
#include "qcplayer_oscl_utils.h"

//Platform includes
extern "C" {
#include "msg.h"
} // extern "C"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/////////////////////////////////
//
// Player feature flags
//
/////////////////////////////////

/////////////////////////////////
//
// Debug build options...
//
/////////////////////////////////
// #define FEATURE_QTV_M4V

///////////////////////////
//
// Player configuration parameters
//
///////////////////////////

//Max number of layers the player handles.
//I think this is actually used as max media layers,
//max video layers, and max audio layers
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#elif defined(FEATURE_MP4_TEMPORAL_SCALABILITY)
# define COMMON_MAX_LAYERS 4
#else
# define COMMON_MAX_LAYERS 1
#endif /* FEATURE_MP4_TEMPORAL_SCALABILITY */

//Max display queue depth.
#define COMMON_MAX_DISPLAY_QUEUE_DEPTH 10

//Default display queue depth
#define COMMON_DEFAULT_DISPLAY_QUEUE_DEPTH 5

///////////////////////////
//
// Thread Priorities
//
///////////////////////////

//
// Define our thread priorities as offsets from the
// base priority.
//

// All our threads should be lower than the base priority so that
// API commands will interrupt playback.

//  The player thread should be highest so it can interrupt playback.
//  Any decode threads should be lowest since they can hog the CPU.
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#define PLAYER_PRIORITY_OFFSET   (6)
#define AUDIO_PRIORITY_OFFSET    (5)
#define STREAMER_PRIORITY_OFFSET (4)
#define DOWNLOAD_PRIORITY_OFFSET (3)
#else
#define PLAYER_PRIORITY_OFFSET   (5)
#define AUDIO_PRIORITY_OFFSET    (4)
#define STREAMER_PRIORITY_OFFSET (3)
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#define VIDEO_PRIORITY_OFFSET    (2)
#define CPVTEXT_PRIORITY_OFFSET  (1)
#define PARSER_PRIORITY_OFFSET   (0)

#else

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
   #define PLAYER_PRIORITY_OFFSET   (5)
   #define AUDIO_PRIORITY_OFFSET    (4)
   #define STREAMER_PRIORITY_OFFSET (3)
   #define DOWNLOAD_PRIORITY_OFFSET (2)
#else
   #define PLAYER_PRIORITY_OFFSET   (4)
   #define AUDIO_PRIORITY_OFFSET    (3)
   #define STREAMER_PRIORITY_OFFSET (2)

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#define VIDEO_PRIORITY_OFFSET    (1)
#define PARSER_PRIORITY_OFFSET   (0)

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */


/////////////////////////////////////////
//
// Thread stack sizes
//
/////////////////////////////////////////

#define COMMON_STACK_SIZE (8*1024)
#define COMMON_STREAMER_STACK_SIZE  (12*1024)

/////////////////////////////////////////
//
// Thread timeouts and polling intervals.
//
/////////////////////////////////////////

//
// General
//

//time to wait for a thread to startup.
#define COMMON_THREAD_START_TIMEOUT_MSEC  1000

//time for threads to wait for a command when idle.
#define COMMON_IDLE_WAIT_TIMEOUT_MSEC   (1000*3600)

//time for threads to wait for a command when waiting for next fragment.
#define COMMON_FRAGMENT_WAIT_TIMEOUT_MSEC  1000

//time for threads to wait for a command when waiting for more data
//while doing Progressive DNLD
#define COMMON_DATA_INSUFFICIENT_WAIT_TIMEOUT_MSEC  10

//
// Video
//

//video thread command timeouts
// This needs to be at least as great as the VideoDecoderTimeout
// Time to decode one video frame. Increased this from 2000 to
// 5000 due to the premption occurring when random positioning
// The searcher and other related threads sometimes starve off
// the Video Player thread from running.
#define COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC    12000
#define COMMON_VIDEO_STOP_TIMEOUT_MSEC      60000

//
// Audio
//

//CMX response timeout.  audio commands are blocked during this time.
#define COMMON_CMX_RESPONSE_TIMEOUT_MSEC    1000

//audio thread command timeouts.  must be larger than the cmx response timeout.
#define COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC     5000
#define COMMON_AUDIO_STOP_TIMEOUT_MSEC         5000
#define COMMON_AUDIO_STARTUP_TIME_MSEC         100

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
//timed text thread command timeouts.
#define COMMON_CPVTEXT_RESPONSE_TIMEOUT_MSEC    5000
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

//
// Streaming
//

//polling interval while buffering data during streaming.
#define COMMON_BUFFERING_POLL_TIME_MSEC   50

//For streaming, this is the amount of data we buffer
//initially (preroll duration) and after rebuffering.
#define COMMON_STREAM_DEFAULT_BUFFER_DURATION  5000
#define COMMON_STREAM_DEFAULT_BCAST_BUFFER_DURATION  2000

// The max preroll cap which can be set by User/OEM through
// config item/ISetting
#define COMMON_STREAM_DEFAULT_MAX_PREROLL  25000

// This preroll used to decide for disabling the rate adaptaion
// above 15 sec of preoll and increase the max duration, so that
// QTV can allocate buffer based on new max duration
#define COMMON_STREAM_DEFAULT_DECISIVE_PREROLL  15000
#define COMMON_STREAM_DEFAULT_REBUFFER_PREROLL  5000
#define COMMON_STREAM_DEFAULT_BCAST_REBUFFER_PREROLL  2000

//For streaming, this is the amount of data that triggers rebuffering.
#define COMMON_STREAM_BUFFER_MIN_MSEC 500
#define COMMON_STREAM_BCAST_BUFFER_MIN_MSEC 100

//streamer thread command timeouts.
#define COMMON_STREAMER_RESPONSE_TIMEOUT_MSEC 2000

#define COMMON_STREAMER_CONNECT_TIMEOUT_MSEC  25000
#define COMMON_STREAMER_STOP_TIMEOUT_MSEC      5000
#define COMMON_STREAMER_CLOSE_TIMEOUT_MSEC     5000
#define COMMON_STREAMER_SHUTDOWN_TIMEOUT_MSEC  5000

//
// Download
//
#define COMMON_DOWNLOAD_RESPONSE_TIMEOUT_MSEC   2000
#define COMMON_DOWNLOAD_SHUTDOWN_TIMEOUT_MSEC   10000
#define COMMON_DOWNLOAD_STOP_TIMEOUT_MSEC       5000
//this one is actually used by the audio thread.
#define COMMON_DOWNLOAD_POLLING_INTERVAL_MSEC   100

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
CLASS
  Common

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
struct Common
{
public:

  // threads
  enum ThreadNameType
  {
    PLAYER_THREAD_NAME,
    VIDEO_THREAD_NAME,
    DISPLAY_THREAD_NAME,
    AUDIO_THREAD_NAME,
    STREAMER_THREAD_NAME,
    DOWNLOAD_THREAD_NAME,
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    CPVTEXT_THREAD_NAME,
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    PARSER_THREAD_NAME,
    RENDERER_THREAD_NAME

#ifdef FEATURE_QTV_DRM_DCF
  //QTV Player thread
  ,QTV_UTIL_THREAD_NAME
#endif
#ifdef FEATURE_QTV_STREAM_RECORD
    ,RECORDER_THREAD_NAME
#endif

  };

  // Input clip file type.
  enum FileType
  {
    INPUT_UNKNOWN_FILE
    , INPUT_MPEG4_FILE
    , INPUT_BITSTREAM_FILE
  } ;

  // Error context for MP4 error checks
  enum Mp4ErrorContext
  {
    READMEDIAFILE=0,
    GETFILETYPE,
    GETTITLE,
    GETSCALABILITY,
    GETMAXVIDEO,
    GETVIDEO,
    GETVIDEOTIME,
    GETMAXAUDIO,
    GETAUDIOTIMESCALE,
    GETAUDIO,
    GETAUDIOTIME,
    GETINFO,
    GETNUMBEROFTRACKS,
    SELECTVIDEOTRACK,
    DESCRIBE
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    ,GETTIMEDTEXT
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  };

  // Exception context
  enum ExceptionContext
  {
    PLAYER_THREAD,
    VIDEO_THREAD,
    AUDIO_THREAD,
    AUDIO_CALLBACK,
    STREAMER_THREAD,
    PV_PLAYER_API,
    MEDIA_PLAYER,
    DISPLAY_THREAD,
    DOWNLOAD_THREAD,
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    CPVTEXT_THREAD,
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    PARSER_THREAD
  };

  //Structure used for getting the AV sync stats.
  struct LastFrame
  {
    bool bValid;
    long nFrame;
    long nTimestamp;
    long nTime;
  };

  // Status codes from download thread
  enum DownloadStatusCode
  {
    DOWNLOAD_CONNECTED,
    DOWNLOAD_COMPLETE,
    DOWNLOAD_DATA_AVAIL,
    DOWNLOAD_STOP,
    DOWNLOAD_ERROR_ABORT,
    DOWNLOAD_EXCEPTION,
    DOWNLOAD_MEMALLOC_FAIL
  };

  // Status codes from streamer thread
  enum StreamerStatusCode
  {
    STREAM_OPEN_FAIL,
    STREAM_OPEN_COMPLETE,
    STREAM_CONNECTING,
    STREAM_CONNECTED,
    STREAM_START,
    STREAM_DONE,
    STREAM_ERROR_ABORT,
    STREAM_RATE_ADAPTATION_SUPPORTED,
    STREAM_EXCEPTION,
    STREAM_AUTH_REQUIRED,
    STREAM_REDIRECT,
    STREAM_TRACK_SELECTION_CHANGED,
    STREAM_RELEASE_QOS_COMPLETE,
    STREAM_SERVER_CLOSED_EXCEPTION,
    STREAM_SERVER_NOT_ENOUGH_BW,
    STREAM_SERVER_SIDE_PLAYLIST_SKIP_SUCCESS,
    STREAM_SERVER_SIDE_PLAYLIST_SKIP_FAILED,
    STREAM_SERVER_SIDE_PLAYLIST_TRANSITION,
    STREAM_INVALID
    ,STREAM_TRACK_PRE_SELECTION_COMPLETE

#ifdef FEATURE_QTV_FCS
#error code not present
#endif
    ,STREAM_DATA_INACTIVITY_TIMEOUT
    ,STREAM_TRACKLIST_UNKNOWN_CODEC
    ,STREAM_NETWORK_ERROR
    ,STREAM_RECONNECT_SUCCESS
    ,STREAM_RECONNECT_FAIL
    ,STREAM_RECONNECT_PREPARE_SUCCESS
    ,STREAM_RECONNECT_PREPARE_FAIL
    ,STREAM_RECONNECT_IN_PROGRESS
    ,STREAM_FCS_SWITCH_SUPPORTED
    ,STREAM_BLOCKED_BY_FIREWALL
    ,STREAM_TEARDOWN_FOR_RECONNECT
  };


#ifdef FEATURE_QTV_GENERIC_BCAST

  enum GenericBcastMediaStatusCode
  {
    GENERIC_BCAST_STATUS_OPEN_COMPLETE,
    GENERIC_BCAST_STATUS_OPEN_FAILED,
    GENERIC_BCAST_STATUS_CLOSE_COMPLETE,
    GENERIC_BCAST_STATUS_REPOSITION_COMPLETE,
    GENERIC_BCAST_STATUS_REPOSITION_FAILED,
    GENERIC_BCAST_STATUS_SEEK_SYNC_COMPLETE,
    GENERIC_BCAST_STATUS_SEEK_SYNC_FAILED,
    GENERIC_BCAST_STATUS_TRACK_LIST_UPDATE,
    GENERIC_BCAST_STATUS_PCR_UPDATE,
    GENERIC_BCAST_STATUS_USER_DATA_UPDATE,
    GENERIC_BCAST_STATUS_ABORT,
    GENERIC_BCAST_STATUS_END_OF_TSB,
    GENERIC_BCAST_STATUS_TSB_OVERFLOW,
    GENERIC_BCAST_STATUS_TSB_DATA_LOSS,
    GENERIC_BCAST_STATUS_MAX
  };

  enum GenericBcastMediaSourceID
  {
    GENERIC_BCAST_MEDIA_INVALID, /* Invalid Status */
    GENERIC_BCAST_MEDIA_ISDB,    /* ISDB Media */
    GENERIC_BCAST_MEDIA_FLO,     /* FLO Media  */
    GENERIC_BCAST_MEDIA_TDMB,     /* TDMB Media */
    GENERIC_BCAST_MEDIA_MASK,     /* Media mask source */
    GENERIC_BCAST_SIMULATED_SOURCE, /* Media mask log source */
#ifdef FEATURE_QTV_GENERIC_BCAST_CMMB
#error code not present
#endif
    GENERIC_BCAST_MEDIA_MAX
  };

  enum ClockSyncMethodType
  {
    CLOCK_SYNC_METHOD_NONE,
    CLOCK_SYNC_METHOD_ISDBT_PCR
  };
#endif /* FEATURE_QTV_GENERIC_BCAST */

  // Audio Track State
  enum AudioTrackState
  {
    AUDIO_TRACK_STATE_INVALID,  /* Invalid state */
    AUDIO_TRACK_STATE_SELECTED, /* Selected State */
    AUDIO_TRACK_STATE_READY,    /* READY state (6k Targets) */
    AUDIO_TRACK_STATE_DISABLED /* DISABLED state */
  };

  // Audio Channel Configuration Type
  enum ChannelConfigType
  {
    AUDIO_CHANNEL_UNKNOWN = 0,
    AUDIO_CHANNEL_MONO,         /* Single channel */
    AUDIO_CHANNEL_DUAL,         /* Stereo */
    AUDIO_CHANNEL_TRIPLE,       /* 3 channels (UNSUPPORTED) */
    AUDIO_CHANNEL_QUAD,         /* 4 channels (UNSUPPORTED) */
    AUDIO_CHANNEL_QUINTUPLE,    /* 5 channels (UNSUPPORTED */
    AUDIO_CHANNEL_SEXTUPLE,     /* 5+1 channels (UNSUPPORTED */
    AUDIO_CHANNEL_OCTUPLE,      /* 7+1 channels (UNSUPPORTED) */
    AUDIO_CHANNEL_DUAL_MONO,    /* Dual mono */
    AUDIO_CHANNEL_UNSUPPORTED
  };

  /* Playback speed settings */
  enum PlaybackSpeedType
  {
    PLAYBACK_SPEED_NO_CHANGE /* Use existing configuration */
    ,PLAYBACK_SPEED_NORMAL   /* Normal speed (default) */
    ,PLAYBACK_SPEED_1P3X     /* 1.3x speed */
  };

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  /* Dual mono output settings */
  enum DualMonoOutputType
  {
    DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R = 0, /* main channel to left and right */
    DUAL_MONO_OUTPUT_SUB_TO_L_AND_R,      /* sub channel to left and right */
    DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R,  /* main channel to left, sub channel to right */
    DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R   /* sub channel to left, main channel to right */
  };
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

  /* Status codes from parser thread */
  enum ParserStatusCode
  {
    PARSER_IDLE,
    PARSER_READY,
    PARSER_PAUSE,
    PARSER_RESUME,
    PARSER_FAILED
  };


  // Status codes from audio thread.
  enum AudioStatusCode
  {
    AUDIO_BUFFERING,
    AUDIO_PLAYING,
    AUDIO_DONE,
    AUDIO_ERROR_ABORT,
    AUDIO_EXCEPTION,
    AUDIO_PAUSED,
    AUDIO_SUSPENDED,
    AUDIO_STOPPED,
    AUDIO_DOWNLOAD_POLLING_START,
    AUDIO_DOWNLOAD_POLLING_END,
    AUDIO_RESTARTING_TRACK,
    AUDIO_ELAPSE_TIME,
    AUDIO_MEDIA_END,
    AUDIO_RESTART,
    AUDIO_BUFFERING_END,
    AUDIO_PAUSED_DISABLED,
    AUDIO_TRACK_STATE_CHANGE_NOTICE
  };

  // Status codes from video thread.
  enum VideoStatusCode
  {
    VIDEO_BUFFERING,
    VIDEO_PLAYING,
    VIDEO_DONE,
    VIDEO_ERROR_ABORT,
    VIDEO_EXCEPTION,
    VIDEO_PAUSED,
    VIDEO_STOPPED,
    VIDEO_SUSPENDED,
    VIDEO_RENDERING
    ,VIDEO_NEXT_FRAME_DONE
    ,VIDEO_NEXT_FRAME_FAILED
    ,VIDEO_PREV_FRAME_DONE
    ,VIDEO_PREV_FRAME_FAILED
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
    ,VIDEO_ROTATION_DONE
    ,VIDEO_ROTATION_FAILED
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
    ,VIDEO_SCALING_DONE
    ,VIDEO_SCALING_FAILED
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO*/
    ,VIDEO_RESTARTING_TRACK
    ,VIDEO_ELAPSE_TIME
    ,VIDEO_MEDIA_END
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    ,VIDEO_RELEASE_DONE
    ,VIDEO_RELEASE_FAILED
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
    ,VIDEO_DISABLE_DONE
    ,VIDEO_DISABLE_FAILED
    ,VIDEO_ENABLE_DONE
    ,VIDEO_ENABLE_FAILED
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */
  };

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  // Status codes form the TimedText thread.
  enum TimedTextStatusCode
  {
    CPVTEXT_BUFFERING,
    CPVTEXT_PLAYING,
    CPVTEXT_DONE,
    CPVTEXT_ERROR_ABORT,
    CPVTEXT_EXCEPTION,
    CPVTEXT_PAUSED,
    CPVTEXT_STOPPED,
    CPVTEXT_SUSPENDED,
    CPVTEXT_RENDERING
   ,CPVTEXT_RESTARTING_TRACK
   ,CPVTEXT_ELAPSE_TIME
   ,CPVTEXT_MEDIA_END
  };
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  // Audio data buffer.
  struct AudioDataBuffer
  {
    //data buffer
    unsigned char * pData;
    int nData;    //current number of bytes
    int nMaxData; //max number of bytes

    int nPendingData;
    int nPendingOffset;
    unsigned char * pPendingData;

    //frame size buffer, one byte per frame.
    unsigned char * pSize;
    int nSize;    //current number of samples
    int nMaxSize; //max number of samples
    int nPartialFrame;
    int nPartialSamples; // Number of samples of the partial frame
    long nPartialTimestamp; // Timestamp for the partial frame

    //timestamp and frame number of data in buffer.
    long nFirstTimestamp,nLastTimestamp;
    unsigned long nFirstFrame,nLastFrame;

#ifdef FEATURE_QTV_AFE_CLK_BASED_AV_SYNC
#error code not present
#endif // FEATURE_QTV_AFE_CLK_BASED_AV_SYNC
  };

#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
  struct AVSyncAudioDataBuffer
  {
    uint32 nByteOffSet;
    uint32 nTimeStamp;
    uint32 nFrame; // Added for Fast Audio Playback
    uint32 nSampleDelta;
  };
#endif


  // Notify function type used by audio manager.
  enum AudioNotifyType
  {
    NOTIFY_TIMING,
    NOTIFY_DONE,
    NOTIFY_DELAY,
    NOTIFY_ERROR,
    NOTIFY_EXCEPTION
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
   ,NOTIFY_SPEC
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
    ,NOTIFY_FEEDBACK
    ,NOTIFY_TRACK_STATE_CHANGE
    /*
    * Use to initiate immediate data delivery by waking audio thread rather than
    * waiting for timer to expire.
    */
    ,NOTIFY_INITIATE_IMMEDIATE_DATA_DELIVERY
    ,NOTIFY_CLOCK_DRIFT_ADJUST
    ,NOTIFY_IMAGE_CHANGE
  };
  typedef void (*AudioNotifyFuncT)(AudioNotifyType notify,
                                   void* pClientData,
                                   void* pData);
};

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
//these need to come after the other common defs
#include "QCUtils.h"


#endif //qcplayerh

