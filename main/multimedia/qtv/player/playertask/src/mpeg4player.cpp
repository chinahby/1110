/* =======================================================================
                               mpeg4player.cpp
DESCRIPTION
  This module contains the declaration for the Mpeg4Player class.
  The class is the high-level interface to player functionality.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playertask/main/latest/src/mpeg4player.cpp#154 $
$DateTime: 2010/11/26 01:07:04 $
$Change: 1532594 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "Events.h"
#include "QtvPlayer.h"
#include "ReferenceCountedPointer.h"
#include "fileMedia.h"
#include "streammedia.h"
#include "qtv_perf.h"

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   #include "timedtext.h"
   #include "kddiuserdataatom.h"
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
#include "Streamer_v2.h"
#else
#include "streamer.h"
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */

//core lib includes
   #include "filebase.h"
   #include "atomdefs.h"
   #include "qtvstreambase.h"
   #include "QtvConfig.h"
   #include "ITrackList.h"
   #include "DefaultTrackSelectionPolicy.h"

#include "Mpeg4Player.h"

#ifdef FEATURE_QTV_DRM_DCF
  #include "IxStreamFactory.h"
  #include "IxStream.h"
  #include "zrex.h"
  #include "rex.h"
  #include "ixipc.h"
#endif


#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#include "pvdl.h"
#endif

#include "qtv_conc_mgr.h"

extern "C" {
#include "assert.h"
#include "event.h"
#if defined (FEATURE_QTV_MFDRM) && defined (FEATURE_SEC)
#error code not present
#endif
#include "clkregim.h"
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif

#ifdef FEATURE_CMI_MM
#error code not present
#endif
}

#ifndef FEATURE_WINCE
#include "disp.h"
#endif   /*    FEATURE_WINCE   */
#ifdef FEATURE_WINCE
#error code not present
#endif   /*    FEATURE_WINCE   */
#include "zrex.h"
#include "vdecoder_types.h"


#ifndef FEATURE_WINCE
#include "fs_public.h"
#endif   /*    FEATURE_WINCE   */


#ifdef FEATURE_QTV_GENERIC_BCAST
#include "GenericBcastMedia.h"
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
#include "StreamMedia_v2.h"
#define STREAMMEDIA CStreamMedia_v2
#else
#define STREAMMEDIA StreamMedia
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */

#ifdef FEATURE_QTV_BCAST_FLO
#error code not present
#endif  /* FEATURE_QTV_BCAST_FLO */

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//Set test switches here (undef for normal mode)
// #define SHOW_PARSE_INFO
// #define SHOW_AUDIO_TICK

//AV tick, used for debug only.
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
   #define ENABLE_AV_TICK FALSE
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#define QTV_HTTP_PULL_DATA_INTERVAL 500
#endif //defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

//Event priorities.

//API events are lowest priority.
static const int API_EVENT_PRI = 1;
//Internal status events are medium.
static const int INTERNAL_EVENT_PRI = API_EVENT_PRI+1;
//Some internal events are urgent.
static const int INTERNAL_EVENT_PRI_HIGH = INTERNAL_EVENT_PRI+1;

//Psos only has normal and urgent priority.  Urgent messages
//will arrive in LIFO order so only audio timing messages are
//urgent.
static const int EVENT_PRI_URGENT = INTERNAL_EVENT_PRI_HIGH;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
//Temp files for download.
static const char DownloadDatFilename[] = "PVDL.DAT";
static const char DownloadConfigFilename[] = "PVDLCONF.DAT";

//Download constants.
//static const int PSEUDO_STREAM_BUFFER_SIZE_MSEC = 2000;

static const int DOWNLOAD_TIMEOUT_SECONDS  =30 ;

//static const int DOWNLOAD_FILESIZE_TIMEOUT_MSEC = 20000 * 10;

//static const int DOWNLOAD_FILESIZE_POLL_INTERVAL_MSEC = 100;


#endif

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
static const int MAX_TEMPFILENAME=20;
#endif


#ifdef FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE
extern char mp4WriteClipStatsFileName[AEE_MAX_FILE_NAME];
#endif


//static int AUDIO_DELAY_DURATION = 250;
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

//message receive block.
QCMessageReceive Mpeg4Player::playerER;

QCThreadControl Mpeg4Player::playerTC;

bool Mpeg4Player::m_bIsTaskDone = false;

//thread startup sync flag
bool Mpeg4Player::m_bThreadStartSync = false;

uint32 Mpeg4Player::guMpeg4PlayerRefCnt = 0;

QTV_BroadcastTransmitterTask* Mpeg4Player::m_bcastXmitTask = NULL;

/*
  This list of error strings MUST match the definition
 of the associated enum type enum ErrorCode in mpeg4player.h,
 including any conditional defines.
*/
const char* const Mpeg4Player::ErrorStrings[] = {
   "OPEN_CLIP_ERROR",
   "PLAY_CLIP_ERROR",
   "STOP_ERROR",
   "START_PSEUDO_STREAM_ERROR",
   "START_PLAYING_ERROR",
   "RESUME_PLAY_ERROR",
   "QTV_PSEUDO_RESUME_ERROR",
   "QTV_PSEUDO_PARSER_FAILURE",
   "RESTART_ACTIVE_CLIP_ERROR",
   "PREP_AV_ERROR",
   "STREAM_START_ERROR",
   "STREAM_CONNECT_ERROR",
   "QTV_PARSER_INIT_ERROR",
   "QTV_SKIP_CLIP_ERROR",
   "DOWNLOAD_TRACK_SELECT_ERROR",
   "START_HTTP_STREAM_ERROR",
   "QTV_HTTP_RESUME_ERROR",
};

/* How long to wait for the player's to comeout of its event processing loop */
#define PLAYER_EVENT_LOOP_EXIT_TIMEOUT 10000 /* in ms */

/* How often to poll the player while its exiting its eventloop */
#define PLAYER_POLL_EVENT_LOOP_EXIT_TIMEOUT          250 /* in ms */

////////////////////////////////////////////////////////////////////
//
// Event sink table
//
////////////////////////////////////////////////////////////////////

// Event Sink
EVENT_TABLE( playerEventTable, NULL )

//from API

EVENT_SINK( PV_OPEN_URN, Mpeg4Player::PV_OPEN_URN_handler,
           API_EVENT_PRI, TRUE ) /*lint !e641 */

EVENT_SINK( PV_OPEN_BUFFER_URN, Mpeg4Player::PV_OPEN_BUFFER_URN_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */

#ifdef FEATURE_QTV_DRM_DCF
/*
 * Sink/Handler for event posted by QtvPlayer.This event contains
 * IxStream* for audio/video/text
 */

EVENT_SINK( PV_OPEN_INPUTSTREAM_URN, Mpeg4Player::PV_OPEN_INPUTSTREAM_URN_handler,
            API_EVENT_PRI, TRUE )
#endif

EVENT_SINK( QTV_MUTE,
            Mpeg4Player::QTV_MUTE_handler,
            API_EVENT_PRI,
            TRUE ) /*lint !e641 */
#ifdef FEATURE_QTV_GENERIC_BCAST
EVENT_SINK( QTV_SELECT_PB_TRACK,
            Mpeg4Player::QTV_SELECT_PB_TRACK_handler,
            API_EVENT_PRI,
            TRUE ) /*lint !e641 */
EVENT_SINK( QTV_REPOSITION,
            Mpeg4Player::QTV_REPOSITION_handler,
            API_EVENT_PRI,
            TRUE ) /*lint !e641 */
EVENT_SINK( QTV_SEEK_SYNC,
            Mpeg4Player::QTV_SEEK_SYNC_handler,
            API_EVENT_PRI,
            TRUE ) /*lint !e641 */
EVENT_SINK( QTV_PLAY_SYNC,
            Mpeg4Player::QTV_PLAY_SYNC_handler,
            API_EVENT_PRI,
            TRUE ) /*lint !e641 */
#endif /* FEATURE_QTV_GENERIC_BCAST */

EVENT_SINK( PV_PLAY_CLIP, Mpeg4Player::PV_PLAY_CLIP_handler,
           API_EVENT_PRI, TRUE ) /*lint !e641 */
#ifdef FEATURE_QTV_STREAM_RECORD
EVENT_SINK( PV_RECORD_CLIP, Mpeg4Player::QTV_RECORD_CLIP_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( PV_RECORD_STOP, Mpeg4Player::QTV_STOP_RECORDING_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
#endif

EVENT_SINK( PV_REGISTER_FOR_CALLBACK, Mpeg4Player::PV_REGISTER_FOR_CALLBACK_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( PV_REGISTER_FOR_CALLBACK_EX, Mpeg4Player::PV_REGISTER_FOR_CALLBACK_EX_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif
EVENT_SINK( PV_PAUSE, Mpeg4Player::PV_PAUSE_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
EVENT_SINK( QTV_PAUSE_AUDIO, Mpeg4Player::QTV_PAUSE_AUDIO_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( QTV_PAUSE_VIDEO, Mpeg4Player::QTV_PAUSE_VIDEO_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( QTV_PAUSE_TEXT, Mpeg4Player::QTV_PAUSE_TEXT_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( QTV_RESUME_AUDIO, Mpeg4Player::QTV_RESUME_AUDIO_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( QTV_RESUME_VIDEO, Mpeg4Player::QTV_RESUME_VIDEO_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( QTV_RESUME_TEXT, Mpeg4Player::QTV_RESUME_TEXT_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
#endif /*FEATURE_FILE_FRAGMENTATION || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

EVENT_SINK( PV_STOP, Mpeg4Player::PV_STOP_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */

EVENT_SINK( PV_NEXT_FRAME, Mpeg4Player::PV_NEXT_FRAME_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( PV_PREV_FRAME, Mpeg4Player::PV_PREV_FRAME_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
EVENT_SINK( PV_VIDEO_ROTATE, Mpeg4Player::PV_VIDEO_ROTATE_handler,
            INTERNAL_EVENT_PRI, TRUE )
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
EVENT_SINK( PV_VIDEO_SCALE, Mpeg4Player::PV_VIDEO_SCALE_handler,
            INTERNAL_EVENT_PRI, TRUE )
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

EVENT_SINK( PV_SET_AUDIOOVER, Mpeg4Player::PV_SET_AUDIOOVER_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */

//from audio thread
EVENT_SINK( AUDIO_STATUS, Mpeg4Player::AUDIO_STATUS_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( AUDIO_TIMING, Mpeg4Player::AUDIO_TIMING_handler,
           INTERNAL_EVENT_PRI_HIGH, TRUE )/*lint !e641 */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
EVENT_SINK( AUDIO_SPEC, Mpeg4Player::AUDIO_SPEC_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

#ifndef FEATURE_WINCE
//from video thread.
EVENT_SINK( VIDEO_STATUS, Mpeg4Player::VIDEO_STATUS_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */
#endif   /*    FEATURE_WINCE   */

//from streamer thread
EVENT_SINK( STREAMER_STATUS, Mpeg4Player::STREAMER_STATUS_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */


#ifdef FEATURE_QTV_GENERIC_BCAST
EVENT_SINK( GENERIC_BCAST_STATUS,
            Mpeg4Player::GENERIC_BCAST_STATUS_handler,
            INTERNAL_EVENT_PRI,
            TRUE) /*lint !e641 */
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
              //from download thread
EVENT_SINK( DOWNLOAD_STATUS, Mpeg4Player::DOWNLOAD_STATUS_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( PV_DELETE_DOWNLOAD_SESSION, Mpeg4Player::PV_DELETE_DOWNLOAD_SESSION_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( PV_DOWNLOAD_CLIP, Mpeg4Player::PV_DOWNLOAD_CLIP_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( PV_RESUME_DOWNLOAD, Mpeg4Player::PV_RESUME_DOWNLOAD_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( PV_STOP_DOWNLOAD, Mpeg4Player::PV_STOP_DOWNLOAD_handler,
           API_EVENT_PRI, TRUE )/*lint !e641 */
EVENT_SINK( DOWNLOAD_FILE_SIZE_STATUS, Mpeg4Player::DOWNLOAD_FILE_SIZE_STATUS_handler,
           INTERNAL_EVENT_PRI, TRUE )

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
//from TimedText thread.
EVENT_SINK( CPVTEXT_STATUS, Mpeg4Player::CPVTEXT_STATUS_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_QTV_PSEUDO_STREAM

EVENT_SINK( QTV_PS_PARSER_STATUS_EVENT, Mpeg4Player::QTV_PS_PARSER_STATUS_EVENT_handler,
            INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */

EVENT_SINK( QTV_PS_PAUSE_EVENT, Mpeg4Player::QTV_PS_PAUSE_EVENT_handler,
            INTERNAL_EVENT_PRI, TRUE)/*lint !e641 */

EVENT_SINK( QTV_PS_PROCESS_PSEUDO_STREAM, Mpeg4Player::QTV_PS_PROCESS_PSEUDO_STREAM_handler,
            API_EVENT_PRI, TRUE )/*lint !e641 */


EVENT_SINK( QTV_PS_STREAM_OPEN_EVENT,
        Mpeg4Player::QTV_PS_STREAM_OPEN_EVENT_handler,
        API_EVENT_PRI, TRUE)/*lint !e641 */

EVENT_SINK( QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT,
        Mpeg4Player::QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_handler,
           API_EVENT_PRI, TRUE)/*lint !e641 */

#endif/*FEATURE_QTV_PSEUDO_STREAM*/


#ifdef FEATURE_FILE_FRAGMENTATION
EVENT_SINK( QTV_SKIP_CLIP, Mpeg4Player::QTV_SKIP_CLIP_handler,
            API_EVENT_PRI, TRUE )/*lint !e641 */
#endif /*FEATURE_FILE_FRAGMENTATION*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

EVENT_SINK( QTV_HTTP_STREAM_OPEN_EVENT,
        Mpeg4Player::QTV_HTTP_STREAM_OPEN_EVENT_handler,
           API_EVENT_PRI, TRUE)/*lint !e641 */

EVENT_SINK( QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT,
        Mpeg4Player::QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_handler,
           API_EVENT_PRI, TRUE)/*lint !e641 */

EVENT_SINK( QTV_HTTP_PARSER_STATUS_EVENT, Mpeg4Player::QTV_HTTP_PARSER_STATUS_EVENT_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */

EVENT_SINK( QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT, Mpeg4Player::QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */

EVENT_SINK( QTV_PROCESS_HTTP_STREAM, Mpeg4Player::QTV_PROCESS_HTTP_STREAM_handler,
           INTERNAL_EVENT_PRI, TRUE )/*lint !e641 */
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
EVENT_SINK( QTV_HTTP_BUFFER_UPDATE, Mpeg4Player::QTV_HTTP_BUFFER_UPDATE_handler,
  API_EVENT_PRI, TRUE )

EVENT_SINK( QTV_HTTP_EVENT, Mpeg4Player::QTV_HTTP_EVENT_handler,
  API_EVENT_PRI, TRUE )
#endif



EVENT_SINK( QTV_SUSPEND, Mpeg4Player::QTV_SUSPEND_handler,
            API_EVENT_PRI, TRUE )/*lint !e641 */

EVENT_SINK( QTV_RESUME, Mpeg4Player::QTV_RESUME_handler,
            API_EVENT_PRI, TRUE )

#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */

#ifdef FEATURE_QTV_WM_DRM_API
EVENT_SINK( QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT, Mpeg4Player::QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_handler,
            API_EVENT_PRI, TRUE )/*lint !e641 */
#endif /* FEATURE_QTV_WM_DRM_API */

#ifdef FEATURE_QTV_PLAYLIST
EVENT_SINK( QTV_SKIP, Mpeg4Player::QTV_SKIP_handler,
            API_EVENT_PRI, TRUE )
#endif /* FEATURE_QTV_PLAYLIST */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
EVENT_SINK( QTV_COPY_BUFFER_TO_FILE, Mpeg4Player::QTV_COPY_BUFFER_TO_FILE_handler,
            API_EVENT_PRI, TRUE )
#endif

EVENT_SINK( QTV_AUDIO_RESTARTING_TRACK, Mpeg4Player::QTV_AUDIO_RESTARTING_TRACK_handler,
            API_EVENT_PRI, TRUE )

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
EVENT_SINK( QTV_HTTP_REGISTER_CALLBACK_EVENT, Mpeg4Player::QTV_HTTP_REGISTER_CALLBACK_EVENT_handler,
            API_EVENT_PRI, TRUE )

EVENT_SINK( HTTP_DOWNLOAD_STATUS, Mpeg4Player::HTTP_DOWNLOAD_STATUS_handler,
           INTERNAL_EVENT_PRI, TRUE )
#endif
#ifdef FEATURE_QTV_AVI
EVENT_SINK( QTV_EXCESSIVE_FRAME_DROPS,
            Mpeg4Player::QTV_EXCESSIVE_FRAME_DROPS_handler,
            INTERNAL_EVENT_PRI,
            TRUE)
#endif
EVENT_TABLE_END

#ifdef FEATURE_QTV_AVI
EVENT_SOURCE(QTV_EXCESSIVE_FRAME_DROPS, dummy);
#endif
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_LTK
extern "C" void mmu_invalidate_data_cache_lines (uint32*  start_ptr,
                                                 uint32   byte_cnt);
#endif /*FEATURE_LTK*/

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/* This global variable should be set to true by OEM if it wants Qtv to
** release the DSP with the subsequent call to Qtv pause.
*/
extern "C" {
boolean qtv_cfg_enable_dsp_release_oem = FALSE;

extern boolean qtv_cfg_enable_dsp_release;
/* Variable which holds the player state i.e whether it was playing
   or not  before the incoming voice/VT call interrupted the player */
extern boolean playingBeforeVoiceCall;
}
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

#ifdef FEATURE_QTV_DIVX_DRM
  bool IS_DRM_ERROR(int32 error_code)
  {
    if((error_code == DRM_AUTHORIZATION_ERROR)        ||
       (error_code == DRM_ERROR_DEVICE_NOT_REGISTERED)||
       (error_code == DRM_RENTAL_COUNT_EXPIRED)       ||
       (error_code == DRM_OUT_OF_MEMORY)              ||
       (error_code == DRM_PLAYBACK_GENERAL_ERROR))
    {
      return true;
    }
    return false;
  }
#endif
/* =======================================================================
**                            Function Definitions
** ======================================================================= */
////////////////////////////////////////////////////////////////////
//
// Player Control Routines
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION
  Mpeg4Player::CreatePlayer

DESCRIPTION
//
//  Create an instance of the player.
//  Note: the reason we need a "create" instead of just doing all
//  this in the constructor is that zrex calls crash inside
//  constructors.
//  Return true if no error.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::CreatePlayer
(
  QtvPlayer::MallocOutputBufferT MallocOutputBuffer,
  QtvPlayer::FreeOutputBufferT   FreeOutputBuffer
)
{
   //static const char pCreateError[] = "Mpeg4Player::CreatePlayer failed";

   //prevent multiple invocation on CreatePlayer
   if (bInstance)
   {
      return false;
   }

   bInstance=true;

   InitDefaults();

   //
   //Init the system utilities. Done only once.
   //
   if(1 == guMpeg4PlayerRefCnt)
   {
     ZUtils::Init();

     if (!QCUtils::Init())
     {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::CreatePlayer failed" );
        return false;
     }
   }

   if (!m_mediaSync.Init(&AVSync_CS, &qtvConfigObject))
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::CreatePlayer failed" );
      return false;
   }


   // Store the OEM buffer allocation function.
   // Will be used when media wants to send data to the application layer.
   OEMMallocBuffer = MallocOutputBuffer;
   OEMFreeBuffer =   FreeOutputBuffer;
   //
   // Note:  the reason to do all the thread creation here
   // is that there's a zrex bug which causes constructors
   // to execute multiple times.  it appears to occur during
   // thread creation.  once it's fixed it might be nice
   // to make the threads create themselves automatically
   // as needed.
   //

   //create the embedded objects
#ifndef FEATURE_WINCE
  //Initialize the AudioPlayerIf
  AudioPlayerIf::Init();
  if (!AudioPlayerIf::CreateAudioPlayer(activeAudioPlayerId, EventPostCallback, (void *)this))
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::CreatePlayer failed" );
      return false;
   }
  else
  {
    AudioPlayerIf::SetAVSyncObj(activeAudioPlayerId, &m_mediaSync);
  }

  if (pActiveVideoPlayer && !(pActiveVideoPlayer->Create((void *)this, MallocOutputBuffer, FreeOutputBuffer, &m_mediaSync,&qtvConfigObject)))
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::CreatePlayer failed" );
     return false;
  }
  if(1 == guMpeg4PlayerRefCnt)
  {
    #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if (!textPlayer.Create((void *)this, &m_mediaSync))
    {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::CreatePlayer failed" );
       return false;
    }
    #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  }

  ResetPlaybackData();
#endif   /*    FEATURE_WINCE   */
  #ifndef FEATURE_QTV_MAPI_STREAM_MEDIA
  if (pQtvStream)
  {
    QTV_Delete( pQtvStream ); /*lint !e449 */
    pQtvStream = NULL;
  }
  #endif /* !FEATURE_QTV_MAPI_STREAM_MEDIA */

  //create the player thread.
  if (!StartThread())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::CreatePlayer failed" );
    return false;
  }

  ShowFlags();

  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::EventPostCallback

DESCRIPTION
  This is the function callback registered by Mpeg4Player with Adio/Video
  subsystems. Used to post an event to Mpeg4Player.

PARAMETERS:
  QCMessageType * const   pEvent
    Event to be posted to the Mpeg4Player via zrex event dispatching mechanism.

  void * const pCbData
    Callback data. (this pointer)

RETURN VALUE
  None

SIDE EFFECTS
  None.

========================================================================== */
void Mpeg4Player::EventPostCallback
(
  QCMessageType * const   pEvent,
  void * const            pCbData
)
{
  Mpeg4Player *pThis = ( Mpeg4Player * )pCbData;
  ASSERT( pThis );

  //Now post the event.
  pThis->PostMessage( pEvent );
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetPlayerStateExternal

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
QtvPlayer::ReturnT Mpeg4Player::GetPlayerStateExternal(QtvPlayer::PlayerStateRecordT &state)
{
   if (bInstance)
   {
      GetPlayerState(state);
      return QtvPlayer::QTV_RETURN_OK;
   }
   return QtvPlayer::QTV_RETURN_ERROR;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
/* ======================================================================
FUNCTION
  Mpeg4Player::GetDownloadTime

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
QtvPlayer::ReturnT Mpeg4Player::GetDownloadTime(QtvPlayer::PlayerStateRecordT &state)
{
   if (bInstance)
   {
       state.downloadTime=0;
       if (clip.bDownload)
       {
           QTV_MSG(MSG_LEGACY_LOW,"GetDownloadTime: Call GetPlaybackTime ");

           if (pPVDL)
           {
               int n;
               if (pPVDL->GetPlaybackTime(&n)==PVDL_SUCCESS)
               {
                   state.downloadTime=n;
                   return QtvPlayer::QTV_RETURN_OK;
               }
               else
               {
                   return QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
               }

           }
       }

   }

   return QtvPlayer::QTV_RETURN_ERROR;
   }

#endif



#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
/* ======================================================================
FUNCTION
Mpeg4Player::getDownloadClipType

DESCRIPTION
returns the playback mode for a FastTrack clip

DEPENDENCIES
List any dependencies for this function, global variables, state,
resource availability, etc.

RETURN VALUE
Enumerate possible return values

SIDE EFFECTS
Detail any side effects.

========================================================================== */
QtvPlayer::DownloadPlaybackControlT Mpeg4Player::getDownloadClipType(void)
{
  QtvPlayer::DownloadPlaybackControlT nReturn = QtvPlayer::NONE;
  if (bInstance)
  {
    if (clip.bDownload && appData.bValid )
    {
      nReturn = appData.data.DownloadPlaybackControl;
    }
    else
    {
      if(clip.bQtvHttpStreaming)
      {
       nReturn = QtvPlayer::PLAYBACK_ASAP;
      }
    }

  }

  return nReturn;

}
#endif
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

/* ======================================================================
FUNCTION
  Mpeg4Player::SetPvxSaveOption

DESCRIPTION
  Sets a flag that determines whether to save downloaded pvx file to buffer of EFS.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::SetPvxSaveOption(char *filePath)
{

   if (filePath)
   {
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                "Mpeg4Player::SetPvxSaveOption PDS2: File Path %s ",
                filePath);
       FileSavedToBuffer = 0;
       int len = strlen(filePath);
       if (len > 3 * 1024)
       {
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Directory name too long = % d",
                  len);
          Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT );
          return;
       }
       nPvxFilePath = (char*)QTV_Malloc(len+1);
       if (nPvxFilePath != NULL)
       {
         (void)std_strlcpy(nPvxFilePath,filePath, len+1);
         nPvxFilePath[len]='\0';
       }
       else
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Malloc failed");
         Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT );
         return;
       }
   }
   else
   {
       FileSavedToBuffer = 1;
   }

}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetPvxPreroll

DESCRIPTION
  Sets the preroll for a  pvx clip.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::SetPvxPreroll(int pvxPreroll)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                "Mpeg4Player::SetPvxPreroll PDS2: pvxPreroll %d ",
                pvxPreroll);


  nPvxPreroll= pvxPreroll ;
  //Use nPvxPreroll in SetBufferDuration()

}
#endif
/* ======================================================================
FUNCTION
  Mpeg4Player::GetAudioVideoStatistics

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
QtvPlayer::ReturnT Mpeg4Player::GetAudioVideoStatistics(QtvPlayer::AudioVideoStatisticsT &statistics)
{
  /* Clear out the statistics structure */
  memset( (void*)&statistics, 0, sizeof(statistics) );

  if (bInstance)
  {
    bool bOK=true;

     if((playerState == CONNECTING) || (playerState == IDLE))
     {
       return QtvPlayer::QTV_RETURN_ERROR;
     }

    Common::LastFrame audioLastFrame,videoLastFrame;

    // Initialize the A/V Sync to "not present"
    statistics.AVSync.avSyncValid = FALSE;

    // Get video stats or zero
    bool bVideoOK=false;
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (clip.bHasVideo)
#else
    if (clip.bHasVideo || IsVideoMuted())
#endif
    {
      if (
#ifndef FEATURE_WINCE
            pActiveVideoPlayer
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
)
      {
#ifndef FEATURE_WINCE
         bVideoOK = pActiveVideoPlayer->GetStatistics(statistics,videoLastFrame);
#endif   /*    FEATURE_WINCE   */
         if (!bVideoOK)
         {
           bOK=false;
         }
      }
    }

    //get audio stats
    bool bAudioOK=false;
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (clip.bHasAudio)
#else
    if (clip.bHasAudio || IsAudioMuted())
#endif
    {
#ifndef FEATURE_WINCE
      bAudioOK = AudioPlayerIf::GetStatistics(activeAudioPlayerId,statistics,audioLastFrame);
#endif
      if (!bAudioOK)
      {
        bOK=false;
      }
    }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#endif /* FEATURE_MP4_UNUSED_CODE */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    if (bOK)
    {
      QTV_MSG( QTVDIAG_STATISTICS,  "Statistics");
      if( bVideoOK )
      {
        QTV_MSG( QTVDIAG_STATISTICS,  " Video:");
        QTV_MSG1( QTVDIAG_STATISTICS, "   format %d", statistics.Video.format );
        QTV_MSG1( QTVDIAG_STATISTICS, "   frames %d", statistics.Video.frames );
        QTV_MSG1( QTVDIAG_STATISTICS, "   I-Frames %d", statistics.Video.nIFrameTally );
        QTV_MSG1( QTVDIAG_STATISTICS, "   P-Frames %d", statistics.Video.nPFrameTally );
        QTV_MSG1( QTVDIAG_STATISTICS, "   B-Frames %d", statistics.Video.nBFrameTally );
        QTV_MSG1( QTVDIAG_STATISTICS, "   framesInterpolated %d", statistics.Video.framesInterpolated );
        QTV_MSG1( QTVDIAG_STATISTICS, "   skippedDecode %d", statistics.Video.skippedDecode );
        QTV_MSG1( QTVDIAG_STATISTICS, "   skippedDisplay %d", statistics.Video.skippedDisplay );
        QTV_MSG1( QTVDIAG_STATISTICS, "   avSyncDrops %d", statistics.Video.avSyncDrops );
        QTV_MSG1( QTVDIAG_STATISTICS, "   Drops for Macroblock Errors %d", statistics.Video.skippedDisplayDueToErrs );
        QTV_MSG1( QTVDIAG_STATISTICS, "   failedDecode %d", statistics.Video.failedDecode );
        QTV_MSG1( QTVDIAG_STATISTICS, "   maxDrop %d", statistics.Video.maxDrop );
        QTV_MSG1( QTVDIAG_STATISTICS, "   encodedFrameRate %d", statistics.Video.encodedFrameRate );
        QTV_MSG1( QTVDIAG_STATISTICS, "   encodedBitrate %d", statistics.Video.encodedBitrate );
        QTV_MSG1( QTVDIAG_STATISTICS, "   decodedFrameRate %d", statistics.Video.decodedFrameRate );
        QTV_MSG1( QTVDIAG_STATISTICS, "   displayedFrameRate %d", statistics.Video.displayedFrameRate );
        QTV_MSG1( QTVDIAG_STATISTICS, "   maxDataLead %d", statistics.Video.maxDataLead);
        QTV_MSG1( QTVDIAG_STATISTICS, "   minDataLead %d", statistics.Video.minDataLead);
      }
      if( bAudioOK )
      {
        QTV_MSG( QTVDIAG_STATISTICS,  " Audio:");
        QTV_MSG1( QTVDIAG_STATISTICS, "   format %d", statistics.Audio.format );
        QTV_MSG1( QTVDIAG_STATISTICS, "   encodedBitrate %d", statistics.Audio.encodedBitrate );
        QTV_MSG1( QTVDIAG_STATISTICS, "   frames %d", statistics.Audio.frames );
        QTV_MSG1( QTVDIAG_STATISTICS, "   nullSamples %d", statistics.Audio.nullSamples );
        QTV_MSG1( QTVDIAG_STATISTICS, "   maxDrop %d", statistics.Audio.maxDrop );
        QTV_MSG1( QTVDIAG_STATISTICS, "   maxAudioDataLead %d", statistics.Audio.maxAudioDataLead );
        QTV_MSG1( QTVDIAG_STATISTICS, "   minAudioDataLead %d", statistics.Audio.minAudioDataLead );
      }
      if( statistics.AVSync.avSyncValid )
      {
        QTV_MSG( QTVDIAG_STATISTICS,  " A/V Sync");
        QTV_MSG1( QTVDIAG_STATISTICS, "   videoAheadMsec %d", statistics.AVSync.videoAheadMsec );
        QTV_MSG1( QTVDIAG_STATISTICS, "   videoBehindMsec %d", statistics.AVSync.videoBehindMsec );
      }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#endif /* FEATURE_MP4_UNUSED_CODE */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

      return QtvPlayer::QTV_RETURN_OK;
    }
  }

  return QtvPlayer::QTV_RETURN_ERROR;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetRTPStatistics

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
QtvPlayer::ReturnT Mpeg4Player::GetRTPStatistics(QtvPlayer::RTPStatisticsT &statistics)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::GetRTPStatistics" );

   if (bInstance)
   {
      if (streamer && streamer->GetRTPStatistics(statistics))
      {
         return QtvPlayer::QTV_RETURN_OK;
      }
      return QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
   }
   return QtvPlayer::QTV_RETURN_ERROR;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::GetURNType

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
QtvPlayer::ReturnT Mpeg4Player::GetURNType(QtvPlayer::URNTypeT &URNType, bool bVideoURN)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::GetURNType" );

   QtvPlayer::ReturnT nReturn=QtvPlayer::QTV_RETURN_ERROR;
   if (bInstance)
   {
      nReturn=QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
      //Grab the global URN info.
      QCUtils::EnterCritSect(&URN_CS);

      if (bVideoURN)
      {
         if (videoURN.bValid)
         {
           URNType=videoURN.type;
           nReturn=QtvPlayer::QTV_RETURN_OK;
         }
      } else
      {
         if (audioURN.bValid)
         {
           URNType=audioURN.type;
           nReturn=QtvPlayer::QTV_RETURN_OK;
         }
         else if (textURN.bValid)
         {
           URNType=textURN.type;
           nReturn=QtvPlayer::QTV_RETURN_OK;
         }
      }

      QCUtils::LeaveCritSect(&URN_CS);
   }
   return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetURLType

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
QtvPlayer::ReturnT Mpeg4Player::GetURLType(QtvPlayer::URLTypeT &URLType)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::GetURLType" );

   QtvPlayer::ReturnT nReturn=QtvPlayer::QTV_RETURN_ERROR;
   if (bInstance)
   {
      nReturn=QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
      //Grab the global URL info.
      QCUtils::EnterCritSect(&URL_CS);
      if (url.bValid)
      {
         //we don't report local file URLs, there's no appropriate
         //type to report.
         if (!url.bLocalFileFromPVX)
         {
            URLType=url.type;
            nReturn=QtvPlayer::QTV_RETURN_OK;
         }
      }
      QCUtils::LeaveCritSect(&URL_CS);
   }
   return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetFrameInfo

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  A boolean indicating if the frame info was properly set.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::SetFrameInfo(void *pRGBFrame, void *pYUVFrame,
                               uint16 SrcHeight, uint16 SrcWidth,
                               uint32 Timestamp, uint32 NumConcealedMB,
                               uint8 fVOP, void *pPostFilterMbInfo,
                               unsigned long NumIntraMbs,
                               void *pMetaData,
                               uint16 wCropWinX2, uint16 wCropWinY2)
{
   QCUtils::EnterCritSect(&frameInfo_CS);

   if ( !IsVideoRenderingOkInState( playerState ) )
   {
     QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "SetFrameInfo: rendering not allowed in state %d",
                    playerState );
     QCUtils::LeaveCritSect(&frameInfo_CS);
     return false;
   }

   /* Frame information should not be valid.  If it is, then we've lost a frame */
   if ( frameInfo.bValid )
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Set/Get FrameInfo lost a frame" );
#ifndef FEATURE_WINCE
      if (pActiveVideoPlayer)
      {
         pActiveVideoPlayer->UpdateStats(VideoDec::DROP_BEFORE_DISPLAY);
      }
#endif   /*    FEATURE_WINCE   */
      QCUtils::LeaveCritSect(&frameInfo_CS);
      QTV_PERF(QTV_PERF_MDP_DROP);
      return false;
   }
   QTV_PERF_FRAME_UPDATE(Timestamp, QTV_PERF_RENDER_START);

   /* if MDP is defined, we dont do the bliting to the screen MDP does that so
   ** we dont have to worry about our cache being stale when the frame
   ** is sent to display
   */
#ifndef FEATURE_QTV_MDP
   /* Invalidate cache.  This prevents artifacts, in the form of small
   ** horizontal lines, from displaying due to stale data in the cache.
   */
   int nBytes;
  /* for IPL based scaling DSP does not need bigger buffer */

#if defined(FEATURE_MP4_FRAME_TRANSFORMATIONS) && !defined(FEATURE_QTV_IPL_SCALING)
  /* allocate 4 times more memory to support 2X Up Scaling */
  nBytes = 4 * SrcHeight * SrcWidth * MP4_RGB_BITS_PER_PIXEL / 8;
#else
  nBytes = SrcHeight * SrcWidth * MP4_RGB_BITS_PER_PIXEL / 8;
#endif

#ifdef FEATURE_LTK
   mmu_invalidate_data_cache_lines((uint32 *) pRGBFrame, nBytes);
#endif /*FEATURE_LTK*/

   nBytes = SrcHeight * SrcWidth * MP4_YUV_BITS_PER_PIXEL / 8;

#ifdef FEATURE_LTK
   mmu_invalidate_data_cache_lines((uint32 *) pYUVFrame, nBytes);
#endif /*FEATURE_LTK*/
#endif

   /*keep track of number of bufffers sent to upper layers */
   numBuffersSent++;

   /* Copy the frame information into our local storage */
   frameInfo.bValid = TRUE;
   frameInfo.info.RGBBuffer      = pRGBFrame;
   frameInfo.info.YUVBuffer      = pYUVFrame;
   frameInfo.info.Height         = SrcHeight;
   frameInfo.info.Width          = SrcWidth;

   frameInfo.info.CropWindow.x2      = wCropWinX2;
   frameInfo.info.CropWindow.y2      = wCropWinY2;

   frameInfo.info.CropWindow.x1        = 0;
   frameInfo.info.CropWindow.y1        = 0;

   frameInfo.info.Timestamp      = Timestamp;
   frameInfo.info.NumConcealedMB = NumConcealedMB;
   frameInfo.info.bI_VOP         = fVOP;
   frameInfo.info.pPostFilterMbInfo = (VDEC_POST_FILTER_MB_INFO *)pPostFilterMbInfo;
   frameInfo.info.NumIntraMBBuffer = NumIntraMbs;
   frameInfo.pMetaData             = pMetaData;

   frameInfo.bInfoSetSinceLastPrep = true;

   QCUtils::LeaveCritSect(&frameInfo_CS);

   if( (Media::MPEG4_CODEC  == GetVideoCodecType()) ||
       (Media::H263_CODEC  == GetVideoCodecType()) ||
       (Media::H264_CODEC  == GetVideoCodecType()) ||
       (Media::WMV1_CODEC  == GetVideoCodecType()) ||
       (Media::WMV2_CODEC  == GetVideoCodecType()) ||
       (Media::WMV3_CODEC  == GetVideoCodecType()))
   {
       //update clipinfo dimensions here
       SetImageDimensions(frameInfo.info.Width,frameInfo.info.Height);
   }
   //Set the time stamp to renderer
   if (pActiveVideoPlayer)
   {
   pActiveVideoPlayer->set_last_frame_rendered_((uint64)Timestamp);
   }

   QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
    "Set FrameInfo TS=%d,numBuffersSent=%d",Timestamp,numBuffersSent );

   /* Notify the application layer of the decoded frame */
   Notify(QtvPlayer::QTV_PLAYER_STATUS_DECODE_FRAME);

   return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetFrameInfo

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
QtvPlayer::ReturnT Mpeg4Player::GetFrameInfo
(
  QtvPlayer::FrameInfoT &info,
  void ** ppExtFrmInfo
)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::GetFrameInfo" );

   Media* pVM = GetVideoMedia();

   QtvPlayer::ReturnT nReturn=QtvPlayer::QTV_RETURN_ERROR;

   if ( bInstance )
   {
      nReturn=QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;

      QCUtils::EnterCritSect(&frameInfo_CS);


      // MFLO gets a waiver to the 'bValid' check - for them, we always return
      // information about the last frame we had, even if it's technically no
      // longer valid.  If there was no previous frame, we should
      // still return a failure code, even to MFLO.

      if
      (
        frameInfo.bValid ||
        (
          pVM != NULL &&
          pVM->GetSource() == Media::BCAST_FLO_SOURCE &&
          frameInfo.bInfoSetSinceLastPrep == true
        )
      )
      {
         nReturn=QtvPlayer::QTV_RETURN_OK;
         /* Copy our data into the user buffer */
         info = frameInfo.info;
         /* Copy the extended frame info if a valid ptr is provided */
         if (ppExtFrmInfo)
         {
           *ppExtFrmInfo = frameInfo.pMetaData;
         }
         frameInfo.bValid = FALSE;
      }
      QCUtils::LeaveCritSect(&frameInfo_CS);
   }

   return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetClipInfo

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
QtvPlayer::ReturnT Mpeg4Player::GetClipInfo(QtvPlayer::ClipInfoT &info)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::GetClipInfo" );

  QtvPlayer::ReturnT nReturn=QtvPlayer::QTV_RETURN_ERROR;
  if (bInstance)
  {
    nReturn=QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
 #ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    //Get download filesize if it wasn't available yet.
    bool bNeedSize=false;

    QCUtils::EnterCritSect(&clipInfo_CS);
    if (clip.bDownload && clipInfo.bValid && clipInfo.info.Size==0)
    {
      bNeedSize=true;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
              "Mpeg4Player::GetClipInfo: PDS2: File size is needed") ;
    }

    QCUtils::LeaveCritSect(&clipInfo_CS);

    if (bNeedSize)
    {
      unsigned long size=GetDownloadFilesize();
      if (size>0)
      {
        QCUtils::EnterCritSect(&clipInfo_CS);
        clipInfo.info.Size= (int) size;
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Mpeg4Player::GetClipInfo PDS2: File size %d ",
                 size) ;
        QCUtils::LeaveCritSect(&clipInfo_CS);
      }
    }

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

    //Grab clip info.
    QCUtils::EnterCritSect(&clipInfo_CS);
    if (clipInfo.bValid)
    {
      info=clipInfo.info;
      nReturn=QtvPlayer::QTV_RETURN_OK;
    }
    QCUtils::LeaveCritSect(&clipInfo_CS);
  }

  return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetServerChallenge

DESCRIPTION
  Function : Mpeg4Player::GetServerChallenge()
  Date      : 09/16/2003
  Purpose  : Method to obtain the value field of WWW-Authenticate header
  In/out    : Pointer to write the challenge rcvd from server

  Return    : QTV_RETURN_OK/QTV_RETURN_ERROR
  Note     : By default,canAppAuthenticate is set to FALSE

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT Mpeg4Player::GetServerChallenge(char *challenge)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::GetServerChallenge" );

   QtvPlayer::ReturnT nReturn=QtvPlayer::QTV_RETURN_ERROR;
   if (bInstance)
   {
     (void)std_strlcpy(challenge, serverChallenge, QTV_MAX_CHALLENGE_BYTES + 1);
     nReturn=QtvPlayer::QTV_RETURN_OK;
   }
   return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::AppCanAuthenticate

DESCRIPTION
  Function : Mpeg4Player::AppCanAuthenticate()
  Date      : 09/16/2003
  Purpose  : Method to record the ability of the QtvApp to provide
            Authorization information, if required
  In/out    : TRUE, if Qtv APP can provide authorization info
            FALSE, if Qtv App cannot provide authorization info

  Return    : NONE
  Note     : By default,canAppAuthenticate is set to FALSE

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void
Mpeg4Player::AppCanAuthenticate(bool auth)
{
   canAppAuthenticate = auth;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::CanAppAuthenticate

DESCRIPTION
  Function : Mpeg4Player::CanAppAuthenticate()
  Date      : 09/16/2003
  Purpose  : Method to retrieve the ability of the QtvApp to provide
           Authorization information, if required
      In/out   : NONE
  Return    : TRUE, if Qtv App has informed it's ability to provide
            Authorization information
            FALSE, if Qtv App has not informed anything or informed it's
            inability to handle user authorization

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::CanAppAuthenticate()
{
   return canAppAuthenticate;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::RetrySessionWithQtvUserAuthInfo

DESCRIPTION
  Function : Mpeg4Player::retryWithQtvUserAuthInfo()
  date      : 09/16/2003
  purpose  : retry session establishment with qtv authorization info
 In/out    : Authorization info
            This information is passed as is in the value field of the
            outgoing RTSP message header "Authorize : "
  Return    : QTV_RETURN_ERROR if it fails

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT Mpeg4Player::RetrySessionWithQtvUserAuthInfo(char *authInfo)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::RetrySessionWithQtvUserAuthInfo" );

   QtvPlayer::ReturnT nReturn=QtvPlayer::QTV_RETURN_ERROR;

   if (bInstance)
   {
      if ( NULL == authInfo || FALSE == challenged )
      {   /* User pressed CANCEL or provide no Auth info or the client
            was never challenged for this session */
      (void)streamer->RetrySessionWithAuthorization(NULL);
         return nReturn;
      }

    (void)std_strlcpy(userAuthInfo, authInfo, QTV_MAX_CHALLENGE_BYTES + 1);
    QTV_MSG_SPRINTF_1(QTVDIAG_MP4_PLAYER,"Authorization value rcvd"
                      "by Mpeg4 player:%s",userAuthInfo);

      challenged = false;
    (void)streamer->RetrySessionWithAuthorization(userAuthInfo);
      nReturn = QtvPlayer::QTV_RETURN_OK;

   }
   return nReturn;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::SetUserAgentParameters

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
QtvPlayer::ReturnT Mpeg4Player::SetUserAgentParameters(const char *man,
                                                       const char *ctn)
{
   QtvPlayer::ReturnT nReturn = QtvPlayer::QTV_RETURN_ERROR;

  (void)std_strlcpy(userAgentMAN, man, QTV_MAX_USER_AGENT_MAN_BYTES);
  (void)std_strlcpy(userAgentCTN, ctn, QTV_MAX_USER_AGENT_CTN_BYTES);
   if (strlen(man) < QTV_MAX_USER_AGENT_MAN_BYTES)
   {
      nReturn = QtvPlayer::QTV_RETURN_OK;
   }
   if (strlen(ctn) < QTV_MAX_USER_AGENT_CTN_BYTES)
   {
      nReturn = QtvPlayer::QTV_RETURN_OK;
   }

   userAgentParamsType = QTV_LGT_STREAMING_EXTENSIONS;
   return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetUserAgent

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
QtvPlayer::ReturnT Mpeg4Player::SetUserAgent(const char * userAgentName)
{
   QtvPlayer::ReturnT nReturn = QtvPlayer::QTV_RETURN_ERROR;

  (void)std_strlcpy(userAgent, userAgentName, QTV_MAX_USER_AGENT_BYTES);
   if (strlen(userAgentName) < QTV_MAX_USER_AGENT_BYTES)
   {
      nReturn=QtvPlayer::QTV_RETURN_OK;
   }
   return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetUserAgentParameters

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
QtvPlayer::ReturnT Mpeg4Player::SetUserAgentParameters(const char* params)
{
   /* copy parameters */
   userAgentParams = params;
   userAgentParamsType = QTV_SKT_STREAMING_EXTENSIONS;

   return QtvPlayer::QTV_RETURN_OK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Destroy

DESCRIPTION
//
// Force an immediate shutdown and cleanup.  Return true if no error.
//  Note: the reason we need a "destroy" routine instead of
//  just doing this in the destructor is that zrex calls will
//  crash inside the destructor.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
bool Mpeg4Player::Destroy()
{
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::Destroy this=0x%x",this );

   if (!bInstance)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::Destroy failed !bInstance");
      return false;
   }
   //it's not safe to destroy unless player is idle.
   else if (playerState!=IDLE)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::Destroy failed playerState!=IDLE");
      return false;
   } else
   {
      //reset instance counter
      bInstance=false;
#ifndef FEATURE_WINCE
      //destroy embedded objects.
      AudioPlayerIf::Destroy(activeAudioPlayerId);
      activeAudioPlayerId = 0;
      //destroy the video player object
      if (pActiveVideoPlayer)
      {
         pActiveVideoPlayer->Destroy();
         PlayerBaseIF::DestroyVideoPlayerInstance(pActiveVideoPlayer);
         pActiveVideoPlayer = NULL;
      }
#endif   /*    FEATURE_WINCE   */

      if(1 == guMpeg4PlayerRefCnt)
      {
#ifndef FEATURE_WINCE
  #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        textPlayer.Destroy();
  #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif
        if (streamer != NULL)
        {
          streamer->Destroy();
        }
      }

      if (!m_useMAPILogic && pQtvStream)
      {
         QTV_Delete( pQtvStream );
         pQtvStream = NULL;
      }

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

      /* clear previous media info, if any */
      if(mediaInfo)
      {
        QTV_Free(mediaInfo);
        mediaInfo = NULL;
      }
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
      if (dataInterface)
      {
        QTV_Delete(dataInterface);
        dataInterface = NULL;
      }
      if(httpHeaderManager)
      {
        QTV_Delete(httpHeaderManager);
        httpHeaderManager = NULL;
      }
#endif

      if(1 == guMpeg4PlayerRefCnt)
      {
        // Wait for the player thread to exit its processing loop then kill it.
        unsigned long sleepTotal = 0;
        while ( !m_bIsTaskDone && sleepTotal < PLAYER_EVENT_LOOP_EXIT_TIMEOUT )
        {
          sleepTotal += PLAYER_POLL_EVENT_LOOP_EXIT_TIMEOUT;
          rex_sleep( PLAYER_POLL_EVENT_LOOP_EXIT_TIMEOUT );
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                        "Mpeg4Player::Destroy %d ms spend waiting to finish..!", sleepTotal);
        }
        QCUtils::KillThread(&playerTC);
        m_bThreadStartSync = false;

        //Utilities terminate.
        ZUtils::Terminate();
        QCUtils::Terminate();
      }

      //cleanup
      CleanupPlayback();
      DestroyMedia();

      m_mediaSync.Cleanup();

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_HTTP_DOWNLOAD)
      if (!FileNeedstobeSaved && !FileSavedToBuffer)
      {
        (void)OSCL_FileDelete(downloadFileName);
      }
      if (downloadFileName)
      {
        QTV_Free(downloadFileName);
        downloadFileName = NULL;
      }
#endif

      return true;
   }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* ======================================================================
FUNCTION
  Mpeg4Player::GetBitfile

DESCRIPTION
//
// Routines to access global data.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
OSCL_FILE * Mpeg4Player::GetBitfile(bool bVideoBitFile)
{
   if (bVideoBitFile)
      return clip.pVideoBitfile;
   else
      return clip.pAudioBitfile;
}

////////////////////////////////////////////////////////////////////
//
// private utilities
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION
  Mpeg4Player::IsThreadActive

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
bool Mpeg4Player::IsThreadActive()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::IsThreadActive" );

   return QCUtils::IsThreadActive(&playerTC);
}

/* ======================================================================
FUNCTION
  Mpeg4Player::StartThread

DESCRIPTION
//
//  Create the player thread
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::StartThread()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::StartThread" );

   //only one can be active at a time.
   if (!QCUtils::IsThreadActive(&playerTC))
   {
      //start the thread.

      playerState = IDLE;
      lastPlayerState = IDLE;

      playerAudioConcState = AUDIO_NOT_CONCURRENT;
      audioSource = QtvPlayer::AUDIO_SOURCE_AUTO;
      prevAudioConcState   = AUDIO_NOT_CONCURRENT;

      playerTC.priority = QTVPlayerPriority;
      m_bThreadStartSync = false;

      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Creating Mpeg4Player thread" );
      if (!QCUtils::CreateThread(&playerTC,(unsigned long)this, false))
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::StartThread failed");
         return false;
      }

      unsigned long sleepTotal = 0;
      static const unsigned long EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL = 10;

      //don't return until the thread has set its event dispatch table.
      while ( !m_bThreadStartSync && sleepTotal <= COMMON_THREAD_START_TIMEOUT_MSEC)
      {
        sleepTotal += EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL;
        rex_sleep( EVENT_DISPATCH_TABLE_CREATION_POLL_INTERVAL );
      }

      if(sleepTotal > COMMON_THREAD_START_TIMEOUT_MSEC)
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::StartThread failed start thread timeout");
         return false;
      }
      else
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                      "Mpeg4Player: %d ms spent starting thread and creating event dispatch table", sleepTotal);
      }
   }
   if (!QCUtils::IsThreadActive(&playerTC))
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::StartThread failed thread not active");
      return false;
   }
   else
   {
     threadIdle = false;
   }

   return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ShowFlags

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
void Mpeg4Player::ShowFlags()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::ShowFlags" );

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   PV OS ZREX" );
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   FEATURE_QTV_STREAM" );
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   FEATURE_QTV_LOCAL_FILE" );
#ifdef FEATURE_QTV_SDP
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   FEATURE_QTV_SDP" );
#endif
#ifdef FEATURE_QTV_M4V
#error code not present
#endif
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   FEATURE_QTV_REPOSITIONING" );
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   FEATURE_MP4_3GPP_TIMED_TEXT" );
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

}

/* ======================================================================
FUNCTION
  Mpeg4Player::ShowStatus

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
void Mpeg4Player::ShowStatus(
  QtvPlayer::PlayerStatusT status,
#ifdef SHOW_FRAMES
  QtvPlayer::PlayerStateRecordT ps
#else
  QtvPlayer::PlayerStateRecordT
#endif /* SHOW_FRAMES */
 ,Mpeg4Player::CallbackData cb
  )
{

   const int MAX_DESCRIPTION_LENGTH = 200;
   char        description[MAX_DESCRIPTION_LENGTH];
   const char *string = QtvPlayer::MapPlayerStatusCodetoString(status);
   const char *formatString = "Qtv engine code %d: %s";

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ShowStatus" );

   /*-------------------------------------------------------------------------
     Build a description string
   -------------------------------------------------------------------------*/
   description[0] = 0;
   if (QtvPlayer::IsPlayerStatusErrorCode(status))
   {
     (void)std_strlcpy(description, "ERROR - ", MAX_DESCRIPTION_LENGTH);
   }

   if ( (strlen(string)+strlen(description) ) < MAX_DESCRIPTION_LENGTH)
   {
     (void)std_strlcat(description, string, MAX_DESCRIPTION_LENGTH);
   } else
   {
     (void)std_strlcat(description, "<<Too long>>", MAX_DESCRIPTION_LENGTH);
   }

   /*-------------------------------------------------------------------------
     Print a message according to importance
   -------------------------------------------------------------------------*/
   if (!QtvPlayer::IsPlayerStatusTrivialCode(status))
   {
      QTV_MSG_SPRINTF_2(QTVDIAG_MP4_PLAYER, formatString, status, description);
   }

   /*-------------------------------------------------------------------------
     Call the callback if it is registered and pass the code and string
   -------------------------------------------------------------------------*/

     if(cb.instanceFunc)
   {
      QtvPlayer::PlayerStatusInstanceCallbackT PlayerStatusCallback =
              QtvPlayer::GetQTVErrorCallback(cb.handle);
      if (PlayerStatusCallback != 0)
      {
         PlayerStatusCallback(status, string, cb.handle, cb.pUserData);
      }
   }
   else if (cb.func)
   {
      QtvPlayer::PlayerStatusCallbackT PlayerStatusCallback =
         QtvPlayer::GetQTVErrorCallback();
      if (PlayerStatusCallback != 0)
      {
         PlayerStatusCallback(status, string);
      }
   }

#ifdef SHOW_FRAMES
   QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "<<...V %d A %d P %d",ps.videoFrames,ps.audioFrames,ps.playbackMsec);
#endif /* SHOW_FRAMES */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetPlayerState

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
void Mpeg4Player::GetPlayerState(QtvPlayer::PlayerStateRecordT &ps)
{
  ps.playbackMsec=0;
  ps.videoPlaybackMsec=0;
  ps.audioPlaybackMsec=0;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  ps.textPlaybackMsec=0;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#ifndef FEATURE_WINCE
  //video frames.
  ps.videoFrames=0;
  if (clip.bHasVideo)
  {
    if (pActiveVideoPlayer)
    {
       ps.videoPlaybackMsec = (unsigned long)pActiveVideoPlayer->GetElapsedTime();
       m_nVideoPlaybackTime = ps.videoPlaybackMsec;
       (void)pActiveVideoPlayer->GetPosition(ps.videoFrames);
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"Video playback time in Msec:  %ld", ps.videoPlaybackMsec);
    }
  }
  //audio frames.
  ps.audioFrames=0;
  if (clip.bHasAudio)
  {
    ps.audioPlaybackMsec = (unsigned long)AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
    m_nAudioPlaybackTime = ps.audioPlaybackMsec;
    (void)AudioPlayerIf::GetPosition(activeAudioPlayerId, ps.audioFrames);
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"Audio playback time in Msec:  %ld", ps.audioPlaybackMsec);
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.bHasText)
  {
    ps.textPlaybackMsec = (uint32)textPlayer.GetElapsedTime();
    m_nTextPlaybackTime = ps.textPlaybackMsec;
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"Text playback time in Msec:  %ld",ps.textPlaybackMsec);
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */
#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
  // Adjust playback times for PV server side playlists. Since some times may be
  // before and after clip transitions, we convert maximum raw time and set the
  // time for all media types to the result, so we don't jump back and forth.

  if (clip.bStreaming && (playerState != IDLE))
  {
    int64 audioPlaybackTime = clip.bHasAudio ? (int64) ps.audioPlaybackMsec : (int64) -1;
    int64 videoPlaybackTime = clip.bHasVideo ? (int64) ps.videoPlaybackMsec : (int64) -1;
    int64 textPlaybackTime =
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
       clip.bHasText ? (int64) ps.videoPlaybackMsec :
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
       (int64) -1;

    if (streamer->AdjustPlaybackTimesForServerSidePlaylist(audioPlaybackTime,
         videoPlaybackTime, textPlaybackTime))
    {
      ps.audioPlaybackMsec = (uint32) audioPlaybackTime;
      ps.videoPlaybackMsec = (uint32) videoPlaybackTime;
      ps.textPlaybackMsec  = (uint32) textPlaybackTime;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
        "Playback timestamps not adjusted for SSP");
    }
  } // if (clip.bStreaming)
#endif /* FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST */

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  //download bytes.
  ps.downloadProgress=0;
  if (clip.bDownload)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
            "GetPlayerState: Call GetDownloadProgress");
    ps.downloadProgress=GetDownloadProgress();
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
             "PDS2: DownloadProgress %d",
             ps.downloadProgress);

    if (pPVDL)
    {
      (void)pPVDL->GetPlaybackTime((int*)&ps.downloadTime);
    }

    //This is done because the mediaplayer calculates the download percentage complete
    //by comparing the  the downloaded time from the http header and duration from downloaded file
    //for some clips the downloaded time in last http header is less than duration of downloaded file
    // hence this workaround
    if( bDownloadDone &&
        (ps.downloadTime < clipInfo.info.Duration ) )
        ps.downloadTime = clipInfo.info.Duration ;

    if(playerState == PLAYING)
    {

       if(clip.pVideoMpeg4Playback)
       {
         if(clip.pAudioMpeg4Playback && clip.bHasAudio)
         {
           clip.pVideoMpeg4Playback->setVideoPos(ps.audioPlaybackMsec);
           QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"GetPlayerState: setVideoPos ==audioPos : %u ",
                 ps.audioPlaybackMsec);
         }
         else
         {
           QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"GetPlayerState: setVideoPos %u ",
                 ps.videoPlaybackMsec );
           clip.pVideoMpeg4Playback->setVideoPos(ps.videoPlaybackMsec);
         }
       }

       if(clip.pAudioMpeg4Playback)
       {
         clip.pAudioMpeg4Playback->setAudioPos(ps.audioPlaybackMsec);
         QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"GetPlayerState: setAudioPos %u",
               ps.audioPlaybackMsec);

         if(clip.pVideoMpeg4Playback)
         {
           clip.pVideoMpeg4Playback->setAudioPos(ps.audioPlaybackMsec);
         }
       }
     }

  }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  if (clip.bQtvHttpStreaming)
  {
     ps.downloadProgress= 0;
     if(playerState != IDLE)
     {
       if( !bDownloadDone)
       {
         ps.downloadProgress=httpdownload.getDownloadProgress();
       }
       else
       {
         ps.downloadProgress=100;
       }

     }

  }
#endif /* FEATURE_QTV_HTTP_DOWNLOAD*/

  /*
  * It is possible that for non audio only clip, playback time might increase in a paused state, such as
  * doing SingleStep operation using ShowNextFrame or ShowPrevFrame.If we return audio playback time,
  * it won't correspond to the frame being displayed under this scenario. So, We should take MAX TS across all media streams
  * when player is in a PAUSED state. Otherwise, return audio playback time if audio is not done etc.
  */
  if(playerState == PAUSED || playerState == SUSPENDED ||
    ( playerState == REPOSITIONING && (lastPlayerState == PAUSED || lastPlayerState == SUSPENDED) ))
  {
      ps.playbackMsec = MAX(ps.audioPlaybackMsec, ps.videoPlaybackMsec);
      #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        ps.playbackMsec = MAX(ps.playbackMsec, ps.textPlaybackMsec);
      #endif
      QTV_MSG1(MSG_LEGACY_HIGH,"GetPlayerState:(playerState == PAUSED):returning MAX(audio,video,text)%d",ps.playbackMsec);
  }
  else
  {
#ifndef FEATURE_WINCE
      /* audio always has higher priority, so while repositioning
         audio will be updated before video.
         If we have audio and audio track is still not done, we should
         return its time, else video */
      if( clip.bHasAudio && !AudioPlayerIf::IsDone(activeAudioPlayerId)
      #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
      #endif
      #ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
      #endif
        )
      {
        ps.playbackMsec = ps.audioPlaybackMsec;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"GetPlayerState: Returning Audio playback time");

      }
      else if( clip.bHasVideo && pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone()))
      {
        ps.playbackMsec = MAX(ps.audioPlaybackMsec, ps.videoPlaybackMsec);
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"GetPlayerState: Max of audio(%ld) and video(%ld)",
                                      ps.audioPlaybackMsec,
                                      ps.videoPlaybackMsec  );
      }
      else
      {
    #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        ps.playbackMsec = MAX(ps.audioPlaybackMsec, ps.videoPlaybackMsec);
        ps.playbackMsec = MAX(ps.playbackMsec, ps.textPlaybackMsec);
        QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"GetPlayerState: Max of audio(%ld) and video(%ld) and text(%ld)",
                                      ps.audioPlaybackMsec,
                                      ps.videoPlaybackMsec,
                                      ps.textPlaybackMsec);
    #else
        ps.playbackMsec = MAX(ps.audioPlaybackMsec, ps.videoPlaybackMsec);
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"GetPlayerState: Max of audio(%ld) and video(%ld)"
                                      ps.audioPlaybackMsec,
                                      ps.videoPlaybackMsec  );
    #endif
      }
#endif   /*    FEATURE_WINCE   */
  }//end of else if(playerState == PAUSED)

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif
  //Player elapsed time should never exceed the clip duraiotn,if available.
  if(clipInfo.bValid && clipInfo.info.Duration)
  {
    ps.playbackMsec = MIN(ps.playbackMsec, clipInfo.info.Duration);
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetCallback

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
void Mpeg4Player::GetCallback(Mpeg4Player::CallbackData &cb, QtvPlayer::PlayerStateRecordT &ps)
{ // get current callback function and data.

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::GetCallback" );

   cb=appControl.callback;
   if (cb.func || cb.instanceFunc)
   {
      GetPlayerState(ps);
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Callback

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
void Mpeg4Player::Callback(
                          QtvPlayer::PlayerStatusT status, /* Qtv player status */
                          Mpeg4Player::CallbackData cb,    /* mp4engine_cb Call back */
                          QtvPlayer::PlayerStateRecordT ps /* mpeg4player state */
                          )
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::Callback" );
   ShowStatus(status,ps,cb);
   if (cb.func)
   {
      (cb.func)(status, cb.pClientData, ps);   /* PVMPEG4EngineCB in OEMMPEG42PV.cpp  */
   }
   else if (cb.instanceFunc)
   {
      (cb.instanceFunc)(status, cb.pClientData, ps, cb.handle, cb.pUserData);   /* PVMPEG4EngineCB in OEMMPEG42PV.cpp  */
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Notify

DESCRIPTION
//
// Notify caller of status updates without doing any state change

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::Notify(QtvPlayer::PlayerStatusT status)
{
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::Notify, status=%d", status );

   Mpeg4Player::CallbackData cb ;
   QtvPlayer::PlayerStateRecordT ps;
   GetCallback(cb,ps);

#ifdef FEATURE_QTV_PSEUDO_STREAM
   if(status == QtvPlayer::QTV_PS_PLAYER_BUFFERING)
   {
     cb.pClientData = &appControl.nPseudoStreamDownloadRate;
   }
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   if(status == QtvPlayer::QTV_HTTP_PLAYER_BUFFERING)
   {
     cb.pClientData = &appControl.nHTTPStreamDownloadRate;
   }
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION */

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
   // Copy the clip index and reasons for failure into the user data.
   QtvPlayer::PlaylistClipTransitionInfo transitionInfo;

   if ((status == QtvPlayer::QTV_PLAYER_STATUS_CLIP_TRANSITION) ||
       (status == QtvPlayer::QTV_PLAYER_STATUS_PLAYLIST_PLAY_COMPLETE))
   {
     transitionInfo.clipIndex                    = appControl.cbData1;
     transitionInfo.clipTransitionErrorReasons   = appControl.cbData2;
     transitionInfo.transitionTime               = appControl.cbData3;

     ZUtils::StrcpyN(transitionInfo.playlistName, appControl.cbString1.get_cstr(),
                     sizeof(transitionInfo.playlistName));

     cb.pClientData = &transitionInfo;
   }
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

#ifdef FEATURE_QTV_GENERIC_BCAST
   // Copy the Transaction identifier and timestamp values
   QtvPlayer::MapiCallbackInfo mapiInfo;

   if (status == QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_DONE ||
       status == QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_FAILED ||
       status == QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_DONE ||
       status == QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED ||
       status == QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE ||
       status == QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED)
   {
     mapiInfo.uTransID   = appControl.cbData1;
     mapiInfo.uTimestamp = appControl.cbData2;
     cb.pClientData      = &mapiInfo;
   }

   QtvPlayer::TrackListT trackListInfo;
   if (status == QtvPlayer::QTV_PLAYER_STATUS_TRACK_STATE_CHANGE)
   {
     trackListInfo = m_audioTrackListInfo;
     cb.pClientData = &trackListInfo;
   }
#endif

   if (status == QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED ||
       status == QtvPlayer::QTV_PLAYER_STATUS_PAUSED)
   {
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
     qtv_conc_mgr::suspend_state_type suspend_state =
       qtv_conc_mgr::get_suspend_state(this, QTV_PLAYER_MIME_TYPE);

     if (suspend_state == qtv_conc_mgr::STATE_SUSPENDED)
     {
       status = QtvPlayer::QTV_PLAYER_STATUS_PAUSED_SUSPENDED;
     }
     else if(suspend_state == qtv_conc_mgr::STATE_INT_SUSPENDED)
     {
       status = QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED;
     }
     else
     {
       status = QtvPlayer::QTV_PLAYER_STATUS_PAUSED;
     }
#endif /*FEATURE_QTV_DISABLE_CONC_MGR*/
   }

   /*
     During a streaming session if we get a redirect request from server,
     we filter out all notifications to QTV
   */
   if( !(clip.bStreaming && redirectFlag) )
   {
      Callback(status,cb,ps);
   }
   return;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::LogClipStatistics

DESCRIPTION
  Logs the statistics to the QXDM for this clip.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

========================================================================== */
#ifndef PLATFORM_LTK
void Mpeg4Player::LogClipStatistics( QtvPlayer::AudioVideoStatisticsT &stats)
{
  log_mpeg4_clip_stats_type *pBuf = NULL;

  /* Check if logging is turned on */
  if ( !log_status( LOG_MPEG4_CLIP_STATS_VER3_C ) )
  {
    return;
  }

  /* Allocate a buffer */
  pBuf = (log_mpeg4_clip_stats_type*) log_alloc( LOG_MPEG4_CLIP_STATS_VER3_C, sizeof(*pBuf) );
  if ( pBuf == NULL )
  {
    /* No memory for logging right now, so continue */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"No memory for clip stats logging");
    return;
  }

  /* Fill in the statistics */
  pBuf->DecodedFrameRate                 = (stats.Video.decodedFrameRate << 16); /* Convert to 16.16 fixed point */
  pBuf->DisplayedFrameRate               = (stats.Video.displayedFrameRate << 16); /* Convert to 16.16 fixed point */
  pBuf->NumberFrames                     = stats.Video.frames;
  pBuf->nIFrameTally                     = stats.Video.nIFrameTally;
  pBuf->nPFrameTally                     = stats.Video.nPFrameTally;
  pBuf->nBFrameTally                     = stats.Video.nBFrameTally;
  pBuf->VideoFramesInterpolated          = stats.Video.framesInterpolated;
  pBuf->NumberFramesDroppedUnrecoverable = stats.Video.failedDecode;
  pBuf->NumberFramesDroppedDecodeTime    = stats.Video.skippedDecode;
  pBuf->NumberFramesDroppedRenderTime    = stats.Video.skippedDisplay;
  pBuf->videoAVSyncDrops                 = stats.Video.avSyncDrops;
  pBuf->FramesDroppedofMacroBlockErrors  = stats.Video.skippedDisplayDueToErrs;
  pBuf->MaxConsecVideoDrops              = stats.Video.maxDrop;
  pBuf->VideoEncodedFrameRate            = (stats.Video.encodedFrameRate << 16); /* Convert to 16.16 fixed point */
  pBuf->VideoEncodedBitrate              = stats.Video.encodedBitrate;
  pBuf->VideoFormat                      = stats.Video.format;
  pBuf->maxVideoLead                     = stats.Video.maxDataLead;
  pBuf->minVideoLead                     = stats.Video.minDataLead;
  pBuf->AudioFormat                      = stats.Audio.format;
  pBuf->AudioEncodedBitrate              = stats.Audio.encodedBitrate;
  pBuf->minAudioDataLead                 = stats.Audio.minAudioDataLead;
  pBuf->maxAudioDataLead                 = stats.Audio.maxAudioDataLead;
  pBuf->maxAudioDrop                     = stats.Audio.maxDrop;
  pBuf->numAudioFrames                   = stats.Audio.frames;
  pBuf->nullAudioSamples                 = stats.Audio.nullSamples;

  if( stats.AVSync.avSyncValid && (clip.bHasAudio && clip.bHasVideo))
  {
    pBuf->MaxMsecVideoAhead      = stats.AVSync.videoAheadMsec;
    pBuf->MaxMsecVideoBehind     = stats.AVSync.videoBehindMsec;
  }

  /* Log the buffer */
  log_commit( pBuf );

#ifdef FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE
  {
    char str[256];
    if (mp4WriteClipStatsFileName[0] != '\0')
    {
      char *endpos;
      char *pos = str;
      OSCL_FILE* fp = NULL;
      (void)std_strlcpy(str, "dbg/", strlen("dbg/") + 1);
      strcat(str, mp4WriteClipStatsFileName);

      /* Make sure the complete directory path exists */
      endpos = strrchr(str, '/');
      if (endpos != NULL)
      {
         *endpos = '\0';
         while(pos)
         {
            pos = strchr(pos, '/');
            if (pos)
            {
               *pos = '\0';
            }
            efs_mkdir(str, S_IREAD | S_IWRITE);
            if (pos)
            {
               *pos = '/';
               pos++;
            }
         }
         *endpos = '/';
      }

      /* add the Clip STats extension, and open the file */
      strcat(str, ".cst");
      fp = OSCL_FileOpen((OSCL_CHAR)str, (OSCL_TCHAR *) _T("wb"));
      if ( fp!= NULL && pEngineStats != NULL )
      {
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (clip.bHasVideo)
#else
    if (clip.bHasVideo || IsVideoMuted())
#endif
    {
         /* Write the clip stats to file */
        sprintf(str, "Decoded Frame Rate:             %8lu\r\n", pBuf->DecodedFrameRate);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Displayed Frame Rate:           %8lu\r\n", pBuf->DisplayedFrameRate);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Number Frames:                  %8lu\r\n", pBuf->NumberFrames);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Number of I-Frames:             %8lu\r\n", pBuf->nIFrameTally);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Number of P-Frames:             %8lu\r\n", pBuf->nPFrameTally);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Number of B-Frames:             %8lu\r\n", pBuf->nBFrameTally);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
  sprintf(str, "Video Frames Interpolated:          %8lu\r\n", pBuf->VideoFramesInterpolated);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Frames Dropped - Unrecoverable: %8lu\r\n", pBuf->NumberFramesDroppedUnrecoverable);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Frames Dropped - Decode Time:   %8lu\r\n", pBuf->NumberFramesDroppedDecodeTime);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Frames Dropped - Render Time:   %8lu\r\n", pBuf->NumberFramesDroppedRenderTime);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
  sprintf(str, "Frames Dropped due to AV sync:   %8lu\r\n", pBuf->videoAVSyncDrops);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Total MacroBlocks Concealed:   %8lu\r\n",pBuf->FramesDroppedofMacroBlockErrors);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Max consecutive frame drops:   %8lu\r\n",pBuf->MaxConsecVideoDrops);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Max Msec Video Ahead:           %8lu\r\n", pBuf->MaxMsecVideoAhead);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Max Msec Video Behind:          %8lu\r\n", pBuf->MaxMsecVideoBehind);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "maxVideoDataLead:          %8l\r\n", pBuf->maxVideoLead);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "minVideoDataLead:          %8l\r\n", pBuf->minVideoLead);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Video Encoded Frame Rate:       %8lu\r\n", pBuf->VideoEncodedFrameRate);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Video Format:                   %8lu\r\n", pBuf->VideoFormat);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Video Encoded Bitrate:          %8lu\r\n", pBuf->VideoEncodedBitrate);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
    }
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (clip.bHasAudio)
#else
    if (clip.bHasAudio || IsAudioMuted())
#endif
    {
        sprintf(str, "Audio Format:                   %8lu\r\n", pBuf->AudioFormat);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Audio Encoded Bitrate:          %8lu\r\n", pBuf->AudioEncodedBitrate);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "maxAudioDataLead:          %8l\r\n", pBuf->maxAudioDataLead);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "minAudioDataLead:          %8l\r\n", pBuf->minAudioDataLead);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "max consecutive AudioDrop:          %8lu\r\n", pBuf->maxAudioDrop);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Number Audio Frames:          %8lu\r\n", pBuf->numAudioFrames);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );
        sprintf(str, "Null Audio Samples:          %8lu\r\n", pBuf->nullAudioSamples);
        (void)OSCL_FileWrite ((OSCL_CHAR)str, strlen(str), 1, fp );

        (void)OSCL_FileClose(fp);
        }
        fp = NULL;
      }
    }
  }
#endif /* FEATURE_MP4_WRITE_CLIP_STATS_TO_FILE */
}
#endif /* !PLATFORM_LTK */

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool Mpeg4Player::StartDownloader()
{
  bool bError;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW," Mpeg4Player::StartDownloader ");

  download.StartDownload(bError, /*&playerTC*/ NULL, clip.pVideoLocalFilename);
  {
    if (!bError)
    {
      unsigned long size=GetPVDLFilesize();

      if (size>0)
      {
        SetPVDLFilesize(size);
        return true;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "PDS2: Error can't get filesize in StartDownloader");
        return false;
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR," Error in Mpeg4Player::StartDownloader ");
      DownloadComplete(ABORT);
    }
  }
  return false;
}

void Mpeg4Player::StopDownloader(bool &bError)
//stop downloader
{
  bError=false;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "PDS2: Stopping download");
  download.Stop(bError,&playerTC);
}

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
/* ======================================================================
FUNCTION
  Mpeg4Player::StartHttpDownloader

DESCRIPTION

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::StartHttpDownloader()
{
  bool bError;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW," Mpeg4Player::StartHttpDownloader ");
  if (httpdownload.StartDownload(bError))
  {
    if (!bError)
    {
      return true;
    }
    else
    {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR," Error in Mpeg4Player::StartHttpDownloader ");
    }
  }
  return false;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::StopHttpDownloader

DESCRIPTION

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::StopHttpDownloader(bool &bError)
{
  bError=false;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Stopping HTTP download");
  httpdownload.Stop(bError);
}
#endif
/* ======================================================================
FUNCTION
  Mpeg4Player::StopPlayer

DESCRIPTION
//Stop AV and streamer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::StopPlayer(bool &bError, bool bCloseStreamer)
{
  bError=false;
  bool bErr=false;

  nPrerollDurationFilled =  0;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    bool err = true;
    if (clip.bDownload && pPVDL)
    {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::Stop Downloader" );
       StopDownloader(err);
    }
#endif

// deregister CPU resources if not localfile playback
#ifdef FEATURE_QTV_DYNAMIC_CPU_SWITCHING
  if (clip.bStreaming || clip.bBcastISDB
      #ifdef FEATURE_QTV_BCAST_FLO
#error code not present
      #endif
      #ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      || clip.bDownload
      #endif
      #ifdef FEATURE_QTV_PSEUDO_STREAM
      || clip.bPseudoStreaming
      #endif
      #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      || clip.bHTTPStreaming
      #endif
      #ifdef FEATURE_QTV_HTTP_DOWNLOAD
      || clip.bQtvHttpStreaming
      #endif
      )
  {
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QTV_PROTOCOLS_HIGH);
  }
#elif defined(FEATURE_QTV_STATIC_CPU_HIGH)
  clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QTV_PROTOCOLS_HIGH);
#endif

  if (playerState==OPENING)
  {
   //stop audio polling.
#ifndef FEATURE_WINCE
    bErr = AudioPlayerIf::Stop(activeAudioPlayerId);
#endif
    if (bErr) bError=true;
  }
  else
  {
    if((playerState != CONNECTING) && (playerState != IDLE))
    {

#ifndef PLATFORM_LTK
      /* No need to collect stats, if player is still in 'connecting' state */
      /* (Threre are no stats to collect)*/

      /* No need to stop video,audio and text threads, if player never issued
       * 'start' command to these threads
       */

      // Log clip statistics before stopping the players because:
      //   - videoPlayer is at a higher priority than mpeg4player
      //   - statistics are cleared when stop is called
      /* Get the audio/video statistics before checking if logging is turned on */
      QtvPlayer::AudioVideoStatisticsT stats;
      (void)GetAudioVideoStatistics(stats);
      LogClipStatistics(stats);

#ifndef FEATURE_WINCE
      uint32 writeClipStatsToFile = 0;
      writeClipStatsToFile = qtvConfigObject.GetQTVConfigItem(
                                   QtvConfig::QTVCONFIG_WRITE_STATS_TO_FILE);

      if (writeClipStatsToFile)
      {
          int nreturn = 0;
          nreturn = WriteStatisticsToEFS(stats);
          if (nreturn)
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Successfully statistics are written to a text file in EFS");
          else
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to write statistics to a text file in EFS");
      }
#endif   /*    FEATURE_WINCE   */

#endif

      // Start to teardown and close network resources --
      // this is asynchronous.
      if (clip.bStreaming && bCloseStreamer && streamer )
      {
        (void)streamer->InitiateStop();
      }

      #ifdef FEATURE_QTV_BCAST_FLO
#error code not present
      #endif /* FEATURE_QTV_BCAST_FLO */

      QTV_MSG( QTVDIAG_MP4_PLAYER, "Stop Audio First, followed by Video " );
#ifndef FEATURE_WINCE
      //  if (clip.bHasAudio)
      {
        bErr = AudioPlayerIf::Stop(activeAudioPlayerId);
        if (bErr) bError=true;
      }

#ifndef FEATURE_QTV_GENERIC_BCAST
      if (clip.bHasVideo)
#else
      if (clip.bHasVideo || IsVideoMuted())
#endif
      {
        if (pActiveVideoPlayer)
        {
          if (playerState == SUSPENDED)
          {
              /* this function should be called only in suspended instance */
              pActiveVideoPlayer->StopVideoPlayerSync();
          }
          else
          {
             pActiveVideoPlayer->Stop(bErr);
             if (bErr) bError=true;
          }
        }
      }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifndef FEATURE_QTV_GENERIC_BCAST
      if (clip.bHasText)
#else
      if(clip.bHasText || IsTextMuted())
#endif
      {
        (void)textPlayer.Stop(bErr);
        if (bErr) bError=true;
      }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

      #ifdef FEATURE_QTV_GENERIC_BCAST
      if (clip.bBcastGeneric)
      {
          GENERIC_BCAST_MEDIA_STOP(clip.pVideoMpeg4Playback,bErr);
          if (bErr) bError=true;
      }

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

      #endif /* FEATURE_QTV_GENERIC_BCAST */
#endif   /*    FEATURE_WINCE   */

      // Wait for streamer to close network resources, destroy streamer
      // objects and stop scheduler.
      if (clip.bStreaming && bCloseStreamer && streamer )
      {
        // This will call Streamer::Stop() and Streamer::DestroySession()
        streamer->Destroy();
      }
#ifdef FEATURE_QTV_STREAM_RECORD
      if (bRecording)
      {
         bRecording = FALSE;
         if(clip.pVideoMpeg4Playback)
         {
            (void)clip.pVideoMpeg4Playback->qtvio_stop_file_writer();
         }
      }
#endif
    }
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
  }

  appControl.bHasMediaStarted = false;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  if (clip.bDownload && pPVDL)
  {
     if (err)
     {
       //error's stopping, go ahead and go idle
       //but report an abort instead of a stopped code.
       DownloadComplete(ABORT);
     }
     else
     {
       DownloadComplete(STOPPED);
     }
  }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  if (clip.bQtvHttpStreaming)
  {
     bool err = true;
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::Stop Downloader" );
     StopHttpDownloader(err);
  }
#endif
  QTV_MSG( QTVDIAG_MP4_PLAYER,  "=========================================" );
  QTV_MSG1( QTVDIAG_MP4_PLAYER, " Qtv total bytes allocated: %d", QTV_GetTotalBytesAllocated() );
  QTV_MSG( QTVDIAG_MP4_PLAYER,  "=========================================" );
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PlaybackErrorAbort

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
void Mpeg4Player::PlaybackErrorAbort()
{
   bool bErr;
   StopPlayer(bErr);
   PlaybackComplete(ABORT);
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void Mpeg4Player::DownloadErrorAbort()
{
  bool bErr;
  StopDownloader(bErr);
  DownloadComplete(ABORT);
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


/* ======================================================================
FUNCTION
  Mpeg4Player::ErrorAbort

DESCRIPTION
// this is for doing error abort with an internal error code

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::ErrorAbort(ErrorCode code)
{
  if( /*code >= MPEG4PLAYER_MIN_ERROR &&*/ code <= MPEG4PLAYER_MAX_ERROR )
  {
    /* Print out diagnostic message */
    QTV_MSG_SPRINTF_2( QTVDIAG_MP4_PLAYER, "ErrorAbort: %d=%s",
                       code, ErrorStrings[code] );

    /* Process the abort */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    if (clip.bDownload)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "PDS2: Mpeg4Player::ErrorAbort:downloadErrorAbort");
      DownloadErrorAbort();
    }
    else
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
      PlaybackErrorAbort();
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unknown error code: %d", code );
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Notify

DESCRIPTION
//
// Notify caller of status updates, and change state if necessary.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::Notify(State newState)
{
  static uint32 tempPrerollDuration = 0;
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::Notify, newState=%d", newState );

  //save current status before doing the state change.
  Mpeg4Player::CallbackData cb ;
  QtvPlayer::PlayerStateRecordT ps;
  bool bIgnoreNotify = false;  
  GetCallback(cb,ps);

#ifdef FEATURE_QTV_GENERIC_BCAST
  if(clip.bBcastGeneric && !clip.bLiveBcast)
  {
    if ( playerState != newState && ( newState == PAUSED || newState == SUSPENDED ) )
    {
      bool bRet;
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
      bRet = GENERIC_BCAST_MEDIA_PAUSE(clip.pVideoMpeg4Playback);
      if(!bRet)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "Generic Bcast Media Pause failed");
      }
    }
  }
#endif
  //do state change
  SetState(newState);

  //map state to notification.
  QtvPlayer::PlayerStatusT s = QtvPlayer::QTV_PLAYER_STATUS_IDLE;
  switch (newState)
  {
    case PLAYING:
    {
      nPrerollDurationFilled =  tempPrerollDuration = 0;
#if (defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || \
     defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO))
      if (m_playbackSpeed == Common::PLAYBACK_SPEED_1P3X)
      {
        s=QtvPlayer::QTV_PLAYER_STATUS_PLAYING_1P3X;
      }
      else
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY ||
          FEATURE_QTV_FAST_PLAYBACK_AUDIO */
      {
      s=QtvPlayer::QTV_PLAYER_STATUS_PLAYING;
      }
      break;
    }

    case BUFFERING:
      if(nPrerollDurationFilled > tempPrerollDuration)
      {
        tempPrerollDuration = nPrerollDurationFilled;
      }
      else
      {
        nPrerollDurationFilled = tempPrerollDuration;
      }

      {
        Media * pMedia = NULL;
        if( clip.bStreaming || (clip.bHasVideo && clip.pVideoMpeg4Playback) )
        {
          pMedia = clip.pVideoMpeg4Playback;
        }
        else if ( clip.bHasAudio && clip.pAudioMpeg4Playback)
        {
          pMedia = clip.pAudioMpeg4Playback;
        }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        else if ( clip.bHasText && clip.pTextMpeg4Playback)
        {
          pMedia = clip.pTextMpeg4Playback;
        }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
        if( pMedia && ((int32)nPrerollDurationFilled <= pMedia->GetBufferDuration()) )
        {
          if(pMedia->GetBufferDuration() == 0)
            nPrerollPercentageFilled = 0;
          else
          nPrerollPercentageFilled = ((int)nPrerollDurationFilled*100) /
                                                  pMedia->GetBufferDuration();
        }
        else
        {
          nPrerollPercentageFilled = 100;
        }

        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "Reporting Buffering %% %ld",
                nPrerollPercentageFilled);
      }
      //insures that the PrerollPercentageFilled is not greater than 100%
      if(nPrerollPercentageFilled > 100)
      {
         nPrerollPercentageFilled = 100;
      }
      if (nPrerollPercentageFilled != 100)
      {
         cb.pClientData = &nPrerollPercentageFilled;
         s=QtvPlayer::QTV_PLAYER_STATUS_BUFFERING;
	 bIgnoreNotify = false;		 
      }
      else
      {
          // if 100% buffering done then ignore STATUS_BUFFERING CB to OEM 	     
          bIgnoreNotify = true;
      }
      break;

    case OPENING:
      s=QtvPlayer::QTV_PLAYER_STATUS_OPENING;
#ifdef FEATURE_QTV_PSEUDO_STREAM
      if(clip.bPseudoStreaming)
      {
        cb.pClientData = &appControl.nPseudoStreamDownloadRate;
      }
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      if(clip.bHTTPStreaming)
      {
        cb.pClientData = &appControl.nHTTPStreamDownloadRate;
      }
#endif
      break;

    case PAUSED:
    case SUSPENDED:
    {
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
      qtv_conc_mgr::suspend_state_type suspend_state =
        qtv_conc_mgr::get_suspend_state(this, QTV_PLAYER_MIME_TYPE);

      if (suspend_state == qtv_conc_mgr::STATE_SUSPENDED)
      {
        s = QtvPlayer::QTV_PLAYER_STATUS_PAUSED_SUSPENDED;
      }
      else if (suspend_state == qtv_conc_mgr::STATE_INT_SUSPENDED)
      {
        s = QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED;
      }
      else
      {
        s=QtvPlayer::QTV_PLAYER_STATUS_PAUSED;
      }
#else
      s=QtvPlayer::QTV_PLAYER_STATUS_PAUSED;
#endif /*FEATURE_QTV_DISABLE_CONC_MGR*/
      break;
    }

    case CONNECTING:
      s=QtvPlayer::QTV_PLAYER_STATUS_CONNECTING;
      nPrerollDurationFilled = tempPrerollDuration = 0;
      break;

    case PB_READY:
      s=QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY;
      if( (clip.bHasVideo && clip.pVideoMpeg4Playback) || (clip.bHasAudio && clip.pAudioMpeg4Playback))
      {
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
         // update incall concurrency state for H264
         if ( qtv_conc_mgr::can_process_command(this, QTV_PLAYER_MIME_TYPE) )
         {
           UpdateAudioConcState();
         }
         else
         {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                       "In Call Concurrency not supported");
         }
#endif
       }
#ifdef FEATURE_H264_DECODER
      //Parse the sequence and picture parameter headers for H.264
      if(clip.bHasVideo && clip.pVideoMpeg4Playback
#ifndef FEATURE_WINCE
         && pActiveVideoPlayer
#endif   /*    FEATURE_WINCE   */
)
      {

#ifndef FEATURE_WINCE
         bool nRet = false;
         if (pActiveVideoPlayer)
         {
           nRet = pActiveVideoPlayer->PreParseHeader(clip.pVideoMpeg4Playback, reposControl.bRestart, GetAudioCodecType());
           if (pActiveVideoPlayer->GetLastVDECError() == VDEC_ERR_UNSUPPORTED_PROFILE)
           {
               Notify(QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_PROFILE);
           }
           if (pActiveVideoPlayer->GetLastVDECError() == VDEC_ERR_UNSUPPORTED_DIMENSIONS)
           {
               Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED);
           }
         }

       if ( (!clip.bHasAudio) && (!clip.bHasText) && (!nRet))
       {
                 s=QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT;
       }
#endif   /*    FEATURE_WINCE   */
      }
#endif /* FEATURE_H264_DECODER */
      nPrerollDurationFilled = tempPrerollDuration = 0;
      break;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    case DL_READY:
      s=QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_READY;
      nPrerollDurationFilled = tempPrerollDuration = 0;
      break;

    case DOWNLOADING:
      s=QtvPlayer::QTV_PLAYER_STATUS_DOWNLOADING;
      break;

    case DOWNLOADED_DATA_AVAIL:
      s=QtvPlayer::QTV_PLAYER_STATUS_DOWNLOADED_DATA_AVAIL;
      break;
#endif

    case REPOSITIONING:
      nPrerollDurationFilled = tempPrerollDuration = 0;
      tempPrerollDuration = 0;
      s=QtvPlayer::QTV_PLAYER_STATUS_REPOSITIONING;
      break;

    case IDLE:
      s=QtvPlayer::QTV_PLAYER_STATUS_IDLE;
      nPrerollDurationFilled = tempPrerollDuration = 0;
      break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
    case PLAYER_PSEUDO_PAUSE:
      cb.pClientData = &appControl.nPseudoStreamDownloadRate;
      s=QtvPlayer::QTV_PLAYER_STATUS_PSEUDO_PAUSE;
      break;
#endif /* FEATURE_QTV_PSEUDO_STREAM */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    case PLAYER_HTTP_PAUSE:
      cb.pClientData = &appControl.nHTTPStreamDownloadRate;
      s=QtvPlayer::QTV_PLAYER_STATUS_HTTP_PAUSE;
      break;
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

    default:
      s=QtvPlayer::QTV_PLAYER_STATUS_IDLE;
      break;
  }

  /*
    During a streaming session if we get a redirect request from server,
    we filter out all notifications to QTV
  */
  if( !(clip.bStreaming && redirectFlag) &&
      /* We only need to do a state change for SUSPENDED state, and not send
      ** a status notification. The status notification will be sent after the
      ** video player is done releasing the DSP.
      */
  (newState != SUSPENDED) && (!bIgnoreNotify))
  {
    //Notify caller.
    Callback(s,cb,ps);
  }
}


/* ======================================================================
FUNCTION
  Mpeg4Player::SetState

DESCRIPTION
//
//  Do a player state transition
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::SetState(State newState)
{
   long curr_time ;
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetState, newState=%d", newState );

   if (newState != playerState)
   {
      /* Perform the new state specific processing before the state is
       * changed
       */

      switch (newState)
      {
         case IDLE:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = IDLE %d ",ZUtils::Clock());

#ifdef SHOW_INFO
#ifndef FEATURE_WINCE
            //show the stream info.
            if (clip.bHasAudio) AudioPlayerIf::ShowInfo(activeAudioPlayerId);
            if (clip.bHasVideo && pActiveVideoPlayer)
       {
         pActiveVideoPlayer->ShowInfo();
            }
#endif   /*    FEATURE_WINCE   */
#endif /* SHOW_INFO */

            //cleanup stream.
            if (clip.bStreaming && streamer
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
                )
            {
               bool bError;
               streamer->Stop(bError);
            }
#if defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined (FEATURE_QTV_PSEUDO_STREAM)
            appControl.mediaCounter =0;
#endif


            #ifdef FEATURE_QTV_BCAST_FLO
#error code not present
            #endif /* FEATURE_QTV_BCAST_FLO */

            #ifdef FEATURE_QTV_GENERIC_BCAST
            if (clip.bBcastGeneric)
            {
              bool bError;
              GENERIC_BCAST_MEDIA_STOP(clip.pVideoMpeg4Playback,bError);
            }
            #endif /* FEATURE_QTV_GENERIC_BCAST */

            //show the clip info
#ifdef SHOW_INFO
            ShowClipInfo();
#endif /* SHOW_INFO */
            break;

         case PAUSED:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = PAUSED %d ",ZUtils::Clock());
            break;

         case REPOSITIONING:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = REPOSITIONING %d ",ZUtils::Clock());
            break;

         case CONNECTING:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = CONNECTING %d ",ZUtils::Clock());
            break;

         case BUFFERING:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = BUFFERING %d ",ZUtils::Clock());
            if(clip.bStreaming &&
               (playerState == PLAYING) &&
               (newState == BUFFERING) )
            {
               // This is now rebuffering
               // passing a 'true' here sets all rebuffering parameters
               streamer->SetStreamBufferingParameters(true);
            }
            break;

         case OPENING:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = OPENING %d ",ZUtils::Clock());
            break;

         case PB_READY:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = PB_READY %d ",ZUtils::Clock());
            break;

        case PLAYING:
#ifndef FEATURE_WINCE
            curr_time = ZUtils::Clock();
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = PLAYING %d",curr_time);
            // Transition from BUFFERING to PLAYING
            /*
            * We want to update A/V Offset here only when reposition has been done.
            * Executing in normal buffering/playing scenario will cause video freeze.
            * After successful reposition,
            * (playerState == BUFFERING) AND
            * (lastPlayerState == PB_READY)//(when starting for the first time) OR
            * (lastPlayerState == REPOSITIONING) (normal reposition)
            */
            if( (playerState == BUFFERING) &&
                ( (lastPlayerState == PB_READY)||(lastPlayerState == REPOSITIONING) ) )
            {
                /*
                *
                * Now we should get current playback time from
                * v/a/t player rather than from Mpeg4Player::GetPlayTimes
                * for the reason mentioned below.
                *
                * Mpeg4Player::GetPlayTimes only knows about the TS being asked via PlayClip,
                * which might be significantly different in case of video only clip.
                * This will cause video freeze.
                * If there is an audio, even though we end up updating A/V offset
                * from wrong timestamp here, we may not encounter video freeze as
                * audio will correct the A/V offset when it starts PLAYING.
                */
                long currentPlayTime = 0;

                /*
                * If we have video, we would have repositioned video first before doing audio, so
                * use video elapsed time to set the offset.
                */
                if(clip.bHasVideo)
                {
                  if (
#ifndef FEATURE_WINCE
                      pActiveVideoPlayer
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
)
                  {
#ifndef FEATURE_WINCE
                    currentPlayTime = pActiveVideoPlayer->GetElapsedTime();
#endif   /*    FEATURE_WINCE   */
                    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                                  QTVDIAG_PRIO_MED,"currentPlayTime updated from Video TS %d",currentPlayTime);
                  }
                }
                else if(clip.bHasAudio)
                {
                  //In case there is no video, we would have repositioned audio first before doing it for text
                  //so, use audio elapsed time.
#ifndef FEATURE_WINCE
                  currentPlayTime = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
                   QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                                QTVDIAG_PRIO_MED,"currentPlayTime updated from Audio TS %d",currentPlayTime);

                }
                else if(clip.bHasText)
                {
                  //Text only clip, use elapsed time from text player
#ifndef FEATURE_WINCE
                  currentPlayTime = textPlayer.GetElapsedTime();
#endif
                   QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                                QTVDIAG_PRIO_MED,"currentPlayTime updated from Text TS %d",currentPlayTime);
                }
                else
                {
                  /*
                  * This should never happen, otherwise there is something WRONG!!
                  * Just print a FATAL message.
                  */
                  QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                                QTVDIAG_PRIO_FATAL,"No a/v/t in clip Info currentPlayTime %d",currentPlayTime);
                }

                //Now set the A/V offset based on current playback time(only when it is positive)
                if (currentPlayTime > 0)
                {
                   QTV_MSG_PRIO2(QTVDIAG_GENERAL,QTVDIAG_PRIO_MED,"AV sync set at curr_time %d currentPlayTime %d",curr_time,currentPlayTime);
                   m_mediaSync.SetPlaybackOffset(AVSync::AudioAV,curr_time, currentPlayTime);
                   m_mediaSync.SetPlaybackOffset(AVSync::TextAV,curr_time, currentPlayTime);
                   m_mediaSync.SetPlaybackOffset(AVSync::VideoAV,curr_time, currentPlayTime);
                }
        }
            break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
         case PLAYER_PSEUDO_PAUSE:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                      "Player state = PLAYER_PSEUDO_PAUSE %d",
                      ZUtils::Clock());
            break;
#endif /* FEATURE_QTV_PSEUDO_STREAM */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
         case PLAYER_HTTP_PAUSE:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                      "Player state = PLAYER_HTTP_PAUSE %d",
                      ZUtils::Clock());
            break;
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

        case SUSPENDED:
           QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                         "Player state = SUSPENDED %d ",ZUtils::Clock());
           break;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
        case DL_READY:
           QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                         "Player state = DL_READY %d ",ZUtils::Clock());
           break;

        case DOWNLOADING:
           QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                         "Player state = DOWNLOADING %d ",ZUtils::Clock());
           break;

        case DOWNLOADED_DATA_AVAIL:
           QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                         "Player state = DOWNLOADED_DATA_AVAIL %d ",ZUtils::Clock());
           break;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

         default:
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Player state = ????? %d ", newState);
            break;
      }

      /* change state */
      lastPlayerState = playerState;
      playerState = newState;
   }

   if(playerState != PAUSED && playerState != SUSPENDED)
   {
     /* Reset the resume pending flag */
     appControl.bResumePending = false;
   }

#if 0
   switch (newState)
   {
      //Always call cleanup when Idle even if we did not leave Idle,
      //in case stuff was partially done.
      case IDLE:
         CleanupPlayback();
         break;
   }
#endif
}

/* ======================================================================
FUNCTION
  Mpeg4Player::InitDefaults

DESCRIPTION
//init data for each time player is created.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::InitDefaults()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::InitDefaults" );

   pQtvStream = NULL;

#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
   streamer = NULL;
   clip.pVideoMpeg4Playback = NULL;
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */

   playbackID = 0;
   streamID = 0;
   m_useMAPILogic = false;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
   pPVDL=NULL;
   bDownloading = true;
   bDownloadDone = false;
   appControl.bResumeDownload=false;
   appControl.nStorageSize = 0;
   appControl.bStartDownloading = true;
   appControl.bPvxStart  = false;

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   downloadID = 0;
   FileSavedToBuffer = 1;
   FileNeedstobeSaved = false;
   nPvxFilePath =NULL ;
#endif
   selectedAudioTrackIdx = -1;
   selectedVideoTrackIdx = -1;
   numReconnectAttemptsMade = 0;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   selectedTextTrackIdx = -1;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

   serverChallenge[0] = '\0';
   challenged         = false;
   canAppAuthenticate = false;

#ifdef FEATURE_QTV_PSEUDO_STREAM
   player_pp_entry_time = 0;
   httpPullData.bTimerSet = false;
   httpPullData.dnldDataSize = 0;
   httpPullData.FetchBufferedDataSize = 0;
   httpPullData.m_QtvInstanceHandle= NULL;
   httpPullData.pHttpPullTimer = NULL;
   httpPullData.pUpdateWptrOffsetPS = 0;
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   player_http_pause_entry_time = 0;
   http_startupTime = HTTP_DEFAULT_STARTUP_TIME;
   http_bufferingResumeTime = HTTP_DEFAULT_REBUFFERING_TIME;
   httpPullData.bTimerSet = false;
   httpPullData.dnldDataSize = 0;
   httpPullData.FetchBufferedDataSize = 0;
   httpPullData.pHttpPullTimer = NULL;
   httpPullData.m_QtvInstanceHandle= NULL;
   httpPullData.pUpdateWptrOffsetPD = 0;
   http_bContentPurge = false;
   http_ContentSizeFromSDP = -1;
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

   sdpIF.URLType           = QtvPlayer::URL_NONE;
   sdpIF.pKey              = NULL;
   sdpIF.bValid            = false;
   sdpIF.nPlaylistLength   = 1;
   sdpIF.nPlaylistIndex    = 0;
   sdpIF.bPlaylistComplete = false;
   sdpIF.pSDPInfo          = sdpIF.sdpFile.GetSDPInfo();

   playerState = IDLE ;
   lastPlayerState = IDLE;
   nPrerollDurationFilled = 0;
   nPrerollPercentageFilled = 0;
   appControl.callback.func = NULL;
   appControl.callback.instanceFunc = NULL;
   appControl.callback.pClientData = 0;
   appControl.callback.pUserData = 0;
   appControl.bDelayAudio=false;
   appControl.bDelayAudioOutput = false;
   appControl.bDoneAudioBuffering = false;
   appControl.bReceivedFirstFrame = false;
   appControl.bStepAVSyncNeeded = false;
   appControl.callback.handle = NULL;

   bLoopTrackFlag = false;

  m_nVideoPlaybackTime = 0;
  m_nAudioPlaybackTime = 0;
  acceleratedDuration = -1;
  speed = -1;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  m_nTextPlaybackTime = 0;
#endif


#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

   memset(&mediaInfo, 0, sizeof(mediaInfo));
#ifdef FEATURE_QTV_GENERIC_BCAST
  mediaInfoSize = 0;
#endif

#ifndef FEATURE_WINCE
  activeAudioPlayerId = 0;
  pActiveVideoPlayer = NULL;
#endif   /*    FEATURE_WINCE   */

   InitURN();

   frameInfo.bValid = FALSE;
   numBuffersSent = 0;
   frameToRelease.info.RGBBuffer = NULL;
   frameToRelease.info.YUVBuffer = NULL;
   frameInfo.bInfoSetSinceLastPrep = false;

   //user agent params initialization
   userAgentMAN[0] = '\0';
   userAgentCTN[0] = '\0';
   userAgentVCT[0] = '\0';
   userAgentACT[0] = '\0';
   userAgentSCT[0] = '\0';
   userAgentCLR[0] = '\0';
   userAgentLCD[0] = '\0';
   userAgentVER[0] = '\0';
   userAgent[0] = '\0';

   userAgentParamsType = QTV_SKT_STREAMING_EXTENSIONS;
#ifdef FEATURE_QTV_LGT_STREAMING_EXTENSIONS
   userAgentParamsType = QTV_LGT_STREAMING_EXTENSIONS;
#endif

   (void)std_strlcpy(userAgent, "QtvPlayer", QTV_MAX_USER_AGENT_BYTES);
   InitUserAgentParams();

   bAbort = false;

#ifdef FEATURE_QTV_NET_POLICY
   netPolicyInfo.isSet = false;
#endif /* FEATURE_QTV_NET_POLICY */
#ifdef FEATURE_QTV_STREAM_RECORD
  (void)std_strlcpy(recFileName, "", sizeof(recFileName));
   bRecFileOverWrite = FALSE;
   recDuration = 0;
   recMode = 0xFF;
   bRecPending = FALSE ;
   bRecording = FALSE;
   recorded_clip_duration = 0;
#endif

   redirectFlag = false;
   redirectTime = 0;
   if (NULL != redirectURL)
   {
     redirectURL->Reset();
   }
   bStop = false ;

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */

   release_qos_for_qdsp_release = true;
   bNotifyUnknownCodec = false;
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
   fileOpened = false;
   dataInterface = NULL;
   httpHeaderManager = NULL;
   isMimeTypeSupported = NULL;
   DefaultFetchBufferedDataSize = NULL;
   DefaultFetchBufferedData = NULL;
#endif
#ifdef FEATURE_QTV_GENERIC_BCAST
   InitCmdSpace();
#endif
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_HTTP_DOWNLOAD)
   downloadFileName = NULL;
#endif

   m_playbackSpeed = Common::PLAYBACK_SPEED_NORMAL;

   memset (&qtv_event_clip_started_payload, 0x00, sizeof(qtv_event_clip_started_type));
   m_bMediaAbort = false;
   bAllowVidToSetOffSetOnAudUnderrun = false;
   userAuthInfo[0] = '\0';
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   parser_pause_entry_time = 0;
#endif
   frameInfo.info.RGBBuffer = NULL;
   frameInfo.info.YUVBuffer = NULL;
   frameToRelease.bValid = FALSE;
   frameToRelease.bInfoSetSinceLastPrep = false;
   OEMMallocBuffer = NULL;
   OEMFreeBuffer   = NULL;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
   downloadInfo.bValid = false;
#endif
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
   rootpath[0] = '\0';
   ResetDownloadData();
#endif
   bSelectAudio = false;
   bSelectVideo = false;

   pvxIF.nPlaylistLength = 0;
   pvxIF.nPlaylistIndex = (-1);
   pvxIF.bPlaylistComplete=false;

   playerAudioConcState = AUDIO_NOT_CONCURRENT;
   prevAudioConcState  = AUDIO_NOT_CONCURRENT;
   audioSource = QtvPlayer::AUDIO_SOURCE_AUTO;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

   nPvxPreroll = 0;
#endif
   reposControl.nStart=0;
   reposControl.nStop = -1;
   reposControl.nRestart = -1;
   reposControl.bTimesValid = true;
   reposControl.reposType = 0;
   reposControl.nStreamStart=(-1);
   reposControl.nStreamStop=(-1);

#ifdef FEATURE_FILE_FRAGMENTATION
  ProcessAudioFragmentData.lastFragmentBytes = 0;
  ProcessAudioFragmentData.lastFragmentFrames = 0;

  ProcessVideoFragmentData.lastFragmentBytes = 0;
  ProcessVideoFragmentData.lastFragmentFrames = 0;

#endif


#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   lastAudioPosition = 0;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
   audioPositionWhenSuspended = 0;
#ifdef FEATURE_QTV_PSEUDO_STREAM
   ps_last_ts =0;
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   http_last_ts =0;
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
#ifdef FEATURE_QTV_GENERIC_BCAST
  m_uMuteClock = 0;
  m_uMuteTimestamp = 0;
  m_ulMuteDuration = 0;
  /* Timestamp at which roll-over occurs */
  m_uMaxTimestamp = 0;
#endif

}

/* ======================================================================
FUNCTION
  Mpeg4Player::InitUserAgentParams

DESCRIPTION
//init user agent parameters data

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::InitUserAgentParams(const char* min )
{
  if( QTV_SKT_STREAMING_EXTENSIONS == userAgentParamsType )
  {
    /* set default value for user agent parameters */
    /* retrieve display information */
    disp_info_type  dispInfo;
    unsigned int    displayWidth, displayHeight;

    /* NOTE - extend: support extra color space */
    dispInfo = disp_get_info();

    /* color support */
    char col[2];
    col[1]='\0';
    switch (dispInfo.bpp)
    {
      case DISP_8BPP:
        col[0]='1';
        break;
      case DISP_16BPP:
        col[0]='2';
        break;
      case DISP_18BPP:
        col[0]='3';
        break;
      default:
        /* default to 16 color LCD */
        col[0]='0';
        break;
    }

    /* resolution support */
    /* NOTE - extend: support extra LCD size */
    char lcd[2];
    lcd[1]='\0';
    displayWidth = dispInfo.disp_width;
    displayHeight =  dispInfo.disp_height;
    if (displayWidth <= MP4_SQCIF_WIDTH ||
        displayHeight <= MP4_SQCIF_HEIGHT)
    {
      lcd[0]='0';
    }
    else if (displayWidth <= MP4_QCIF_WIDTH ||
             displayHeight <= MP4_QCIF_HEIGHT)
    {
      lcd[0]='1';
    }
    else if (displayWidth <= MP4_CIF_WIDTH ||
             displayHeight <= MP4_CIF_HEIGHT)
    {
      lcd[0]='2';
    }
    else
    {
      /* default to CIF */
      lcd[0]='2';
    }

    // get min
    static char mintable[10] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
    // set default MIN for SKT streaming
    char minBuf[QTV_MAX_USER_AGENT_MIN_BYTES] = "6191234567";
#ifndef FEATURE_WINCE
    nv_item_type nv1, nv2;
    // To deal with the case when NV_MIN1_I/NV_MIN2_I are internal NV Items,
    // Set up correct index here. Anyway, this is a weird NV access requirement
    *((byte*)&nv1) = 0;  // index 0
    *((byte*)&nv2) = 0;  // index 0
    if( ZUtils::readNVItem( NV_MIN1_I, &nv1 ) &&
       ZUtils::readNVItem( NV_MIN2_I, &nv2 ) )
    {
      // the following implementation is copied from uistate.c
      // ui_min2_ascii() & ui_min1_ascii()
      // since "ui" interface is going to be obsolete, we can
      // not rely on them to provide service anymore.
      #define CDMAMIN 1
      // convert MIN1
      dword minc = nv2.min2.min2[CDMAMIN];
      minBuf[0] = mintable[ minc/100 ];
      minc %= 100;
      minBuf[1] = mintable[ minc/10 ];
      minBuf[2] = mintable[ minc%10 ];
      // convert MIN2
      minc = nv1.min1.min1[CDMAMIN];
      word temp, tempIndex;
      temp = (word) (minc>>14 );
      minBuf[3] = mintable[ temp/100 ];
      temp %= 100;
      minBuf[4] = mintable[ temp/10 ];
      minBuf[5] = mintable[ temp%10 ];
      minc &= 0x3FFFL;                /* get bottom 14 bits */
      /* next digit is top 4 bits */
      temp = (word) (( minc >> 10 ) & 0xF );
      minBuf[6] = (char) ( ( ( temp == 10 ) ? 0 : temp ) + '0' );
      temp = (word) ( minc & 0x3FF ); /* get bottom 10 bits */
      tempIndex = temp / 100;
      if(tempIndex < 10)
      {
         minBuf[7] = mintable[ tempIndex ];
      }
      temp %= 100;
      minBuf[8] = mintable[ temp/10 ];
      minBuf[9] = mintable[ temp%10 ];

      minBuf[10] = '\0';
    }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

    if( min == NULL )
    {
      min = minBuf;
    }

    // construct params string
    char tmpBuffer[QTV_MAX_USER_AGENT_STR_BYTES];
    int len = std_strlprintf( tmpBuffer, QTV_MAX_USER_AGENT_STR_BYTES,
          "org=SS; model=13; terversion=11; VodSwVer=00; min=%s; lcd=%s; cl=%s; vo=1; HwClass=1; "
          "version=0x0200; visualpro=0x040003; audiopro=0x000001; rtpprofile=0x8008; MIDIType=0",
          min, lcd, col);
    if( len > 0 )
    {
      tmpBuffer[len] = '\0';
    }
    else
    {
      tmpBuffer[0] = '\0';
    }

    userAgentParams = tmpBuffer;

  }
  else if ( QTV_LGT_STREAMING_EXTENSIONS == userAgentParamsType )
  {
    std_strlcpy(userAgentMAN, "LG", QTV_MAX_USER_AGENT_MAN_BYTES);
    std_strlcpy(userAgentCTN, "0000000000000", QTV_MAX_USER_AGENT_CTN_BYTES);
    std_strlcpy(userAgentVCT, "03", QTV_MAX_USER_AGENT_VCT_BYTES);
    std_strlcpy(userAgentACT, "01", QTV_MAX_USER_AGENT_ACT_BYTES);
    std_strlcpy(userAgentSCT, "01", QTV_MAX_USER_AGENT_SCT_BYTES);
    {
      disp_info_type  dispInfo;
      unsigned int    displayWidth, displayHeight;

      dispInfo = disp_get_info();
      displayWidth = dispInfo.disp_width;
      displayHeight =  dispInfo.disp_height;

      switch (dispInfo.bpp)
      {
         case DISP_8BPP:
            std_strlcpy(userAgentCLR, "08", QTV_MAX_USER_AGENT_CLR_BYTES);
            break;
         case DISP_16BPP:
            std_strlcpy(userAgentCLR, "16", QTV_MAX_USER_AGENT_CLR_BYTES);
            break;
         case DISP_18BPP:
            std_strlcpy(userAgentCLR, "18", QTV_MAX_USER_AGENT_CLR_BYTES);
            break;
         default:
            break;
      }

      if (displayWidth <= MP4_SQCIF_WIDTH ||
          displayHeight <= MP4_SQCIF_HEIGHT)
      {
         std_strlcpy(userAgentLCD, "1", QTV_MAX_USER_AGENT_LCD_BYTES);
      } else if (displayWidth <= MP4_QCIF_WIDTH ||
                 displayHeight <= MP4_QCIF_HEIGHT)
      {
         std_strlcpy(userAgentLCD, "2", QTV_MAX_USER_AGENT_LCD_BYTES);
      } else if (displayWidth <= MP4_CIF_WIDTH ||
                 displayHeight <= MP4_CIF_HEIGHT)
      {
         std_strlcpy(userAgentLCD, "3", QTV_MAX_USER_AGENT_LCD_BYTES);
      } else
      {
         std_strlcpy(userAgentLCD, "4", QTV_MAX_USER_AGENT_LCD_BYTES);
      }
    }
    std_strlcpy(userAgentVER, "0100", QTV_MAX_USER_AGENT_VER_BYTES);
    min = NULL;
  } /*if ( QTV_SKT_STREAMING_EXTENSIONS = userAgentParamsType )*/
}


/* ======================================================================
FUNCTION
  Mpeg4Player::CleanupPlayback

DESCRIPTION
//
// player cleanup
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::CleanupPlayback()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::CleanupPlayback" );

   //close file
   if (clip.pAudioBitfile)
   {
    (void)OSCL_FileClose(clip.pAudioBitfile);
      clip.pAudioBitfile = NULL;
   }
   if (clip.pVideoBitfile)
   {
    (void)OSCL_FileClose(clip.pVideoBitfile);
      clip.pVideoBitfile = NULL;
   }


#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   qtv_cfg_enable_dsp_release = FALSE;
   lastAudioPosition = 0;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

   audioPositionWhenSuspended = 0;

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   clip.mediaCounter = 0;
#endif /* FEATURE_QTV_PSEUDO_STREAM ||FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

   /* After CleanupPlayback is called we don't want to send any frames to the
   ** OEM, so clear the flag
   */
   frameInfo.bValid = FALSE;
   numBuffersSent = 0;
   frameToRelease.info.RGBBuffer = NULL;
   frameToRelease.info.YUVBuffer = NULL;
   redirectFlag = false ;

   numReconnectAttemptsMade = 0;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  if (clip.pMpeg4Download)
  {
    QTV_Delete( clip.pMpeg4Download );
    clip.pMpeg4Download = NULL;
  }
#endif

  /*
     When Playback is done, reset playback times for audio, video and text.
     This is more clean/neat rather than checking player state in Mpeg4Player::Notify
     (Returning 0 for playback times if player state is IDLE||OPENING||CONNECTING.)

     This fix will take care of the changes introduced in
     Qtv/common/Mpeg4Player.cpp#442
     QTV.05.02/common/Mpeg4Player.cpp#174.
  */

  m_nVideoPlaybackTime = 0;
  m_nAudioPlaybackTime = 0;

  #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    m_nTextPlaybackTime = 0;
  #endif

  #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   if(httpPullData.pHttpPullTimer)
   {
       //When playback ends, de-activate the timer, otherwise,
       //QTV keeps reporting PS/PD download status untill Stop command is issued to Mpeg4Player.
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::CleanupPlayback stopping the Http PULL Timer!!!" );
       HttpPullTimerStop();
   }
  #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::DestroyMedia()
{
  if ( (clip.pVideoMpeg4Playback == clip.pAudioMpeg4Playback) &&
    (clip.pVideoMpeg4Playback == clip.pTextMpeg4Playback) )
  {
    if (clip.pVideoMpeg4Playback)
    {
      QTV_Delete( clip.pVideoMpeg4Playback );
      clip.pVideoMpeg4Playback = NULL;
      clip.pAudioMpeg4Playback = NULL;
      clip.pTextMpeg4Playback = NULL;
    }
  }
  else
  {
    if (clip.pVideoMpeg4Playback)
    {
      QTV_Delete( clip.pVideoMpeg4Playback );
      clip.pVideoMpeg4Playback = NULL;
    }
    if (clip.pAudioMpeg4Playback)
    {
      QTV_Delete( clip.pAudioMpeg4Playback );
      clip.pAudioMpeg4Playback = NULL;
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if (clip.pTextMpeg4Playback)
    {
      QTV_Delete( clip.pTextMpeg4Playback );
      clip.pTextMpeg4Playback = NULL;
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* ======================================================================
FUNCTION
  Mpeg4Player::PostMessage

DESCRIPTION
//
// post an event

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::PostMessage(QCMessageType *pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PostEvent" );

   QCUtils::PostMessage(pEvent, EVENT_PRI_URGENT, &playerER);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

////////////////////////////////////////////////////////////////////
//
// Message handlers.
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_OPEN_URN_handler

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
void Mpeg4Player::PV_OPEN_URN_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_OPEN_URN_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_OPEN_URN_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      const PV_OPEN_URN_type * pEvent = (PV_OPEN_URN_type *)handle;
      bool bOK = pThis->OpenURN(pEvent);

      if (bOK)
      {
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_OPEN_URN_COMPLETE);
      }
      }
      else
      {
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED);
      }
   }
 }
}


#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PS_PROCESS_PSEUDO_STREAM_handler

DESCRIPTION
  Public static method used as message handler to process the PseudoStream fragment

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::QTV_PS_PROCESS_PSEUDO_STREAM_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                "Mpeg4Player::QTV_PS_PROCESS_PSEUDO_STREAM_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PS_PROCESS_PSEUDO_STREAM_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->ProcessPseudoStream((QTV_PS_PROCESS_PSEUDO_STREAM_type *)handle))
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::QTV_PS_PROCESS_PSEUDO_STREAM successful" );
      }
   }
}
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_OPEN_BUFFER_URN_handler

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
void Mpeg4Player::PV_OPEN_BUFFER_URN_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_OPEN_BUFFER_URN_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_OPEN_BUFFER_URN_handler: Input param handle is NULL");
      return;
   }
   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->OpenURN((PV_OPEN_BUFFER_URN_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_OPEN_URN_COMPLETE);
      }
      else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED);
      }
   }
}

#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PS_STREAM_OPEN_EVENT_handler

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
void Mpeg4Player::QTV_PS_STREAM_OPEN_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::QTV_PS_STREAM_OPEN_EVENT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PS_STREAM_OPEN_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->OpenPseudoStream((QTV_PS_STREAM_OPEN_EVENT_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_PS_COMMAND_OPEN_STREAM_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PS_PAUSE_EVENT_handler

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
void Mpeg4Player::QTV_PS_PAUSE_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER,
          "Mpeg4Player::QTV_PS_PAUSE_EVENT_handler");

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PS_PAUSE_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      pThis->PseudoPauseHandler((QTV_PS_PAUSE_EVENT_type *)handle);
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_handler

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
void Mpeg4Player::QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->updatePseudoStreamBufferOffset((QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_UPDATE_PSEUDO_STREAM_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_UPDATE_PSEUDO_STREAM_FAILED);
      }
   }
}
#endif /* FEATURE_QTV_PSEUDO_STREAM */


#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_handler

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
void Mpeg4Player::QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->updateHTTPStreamBufferOffset((QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_UPDATE_HTTP_STREAM_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_UPDATE_HTTP_STREAM_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_HTTP_STREAM_OPEN_EVENT_handler

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
void Mpeg4Player::QTV_HTTP_STREAM_OPEN_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::QTV_HTTP_STREAM_OPEN_EVENT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_HTTP_STREAM_OPEN_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->OpenHTTPStream((QTV_HTTP_STREAM_OPEN_EVENT_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_HTTP_COMMAND_OPEN_STREAM_FAILED);
      }
   }
}


/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_HTTP_PARSER_STATUS_EVENT_handler

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
void Mpeg4Player::QTV_HTTP_PARSER_STATUS_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::QTV_HTTP_PARSER_STATUS_EVENT_handler");

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_HTTP_PARSER_STATUS_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      pThis->HTTPParserStatus((QTV_HTTP_PARSER_STATUS_EVENT_type *)handle);
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PROCESS_HTTP_STREAM_handler

DESCRIPTION
  Public static method used as message handler to process the PseudoStream fragment

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::QTV_PROCESS_HTTP_STREAM_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::QTV_PROCESS_HTTP_STREAM_handler");

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PROCESS_HTTP_STREAM_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->ProcessHTTPStream((QTV_PROCESS_HTTP_STREAM_type *)handle))
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::QTV_PROCESS_HTTP_STREAM successful" );
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_handler

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
void Mpeg4Player::QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER,
          "Mpeg4Player::QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_handler");

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
    (void)pThis->HTTPPause((QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_type *)handle);
   }
}
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_PLAY_CLIP_handler

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
void Mpeg4Player::PV_PLAY_CLIP_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_PLAY_CLIP_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::PV_PLAY_CLIP_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->PlayClip((PV_PLAY_CLIP_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_PLAY_CLIP_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_PLAY_CLIP_FAILED);
    }
  }
}

#ifdef FEATURE_QTV_STREAM_RECORD
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_RECORD_CLIP_handler

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

void Mpeg4Player::QTV_RECORD_CLIP_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_RECORD_CLIP_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_RECORD_CLIP_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->RecordClip((PV_RECORD_CLIP_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_RECORD_CLIP_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_RECORD_CLIP_FAILED);
    }
  }
}

/* ======================================================================
FUNCTION
 Mpeg4Player::QTV_STOP_RECORDING_handler

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

void Mpeg4Player::QTV_STOP_RECORDING_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "QTV_STOP_RECORDING_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_STOP_RECORDING_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->stopStreamRecording((PV_RECORD_STOP_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_RECORD_STOP_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_RECORD_STOP_FAILED);
    }
  }
}

#endif /* FEATURE_QTV_STREAM_RECORD */


/* ======================================================================
FUNCTION
  Mpeg4Player::PV_REGISTER_FOR_CALLBACK_handler

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
void Mpeg4Player::PV_REGISTER_FOR_CALLBACK_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_REGISTER_FOR_CALLBACK_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_REGISTER_FOR_CALLBACK_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->RegisterForCallback((PV_REGISTER_FOR_CALLBACK_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_REGISTER_FOR_CALLBACK_EX_handler

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
void Mpeg4Player::PV_REGISTER_FOR_CALLBACK_EX_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_REGISTER_FOR_CALLBACK_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_REGISTER_FOR_CALLBACK_EX_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->RegisterForCallback((PV_REGISTER_FOR_CALLBACK_EX_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_REGISTER_FOR_CALLBACK_FAILED);
      }
   }
}
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_PAUSE_handler

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
void Mpeg4Player::PV_PAUSE_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_PAUSE_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_PAUSE_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->Pause((PV_PAUSE_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_FAILED);
      }
   }
}

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PAUSE_AUDIO_handler

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
void Mpeg4Player::QTV_PAUSE_AUDIO_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_PAUSE_AUDIO_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PAUSE_AUDIO_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->PauseAudio((QTV_PAUSE_AUDIO_type *)handle))
      {
          pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_AUDIO_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_AUDIO_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PAUSE_VIDEO_handler

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
void Mpeg4Player::QTV_PAUSE_VIDEO_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_PAUSE_VIDEO_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PAUSE_VIDEO_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->PauseVideo((QTV_PAUSE_VIDEO_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_VIDEO_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_VIDEO_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PAUSE_TEXT_handler

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
void Mpeg4Player::QTV_PAUSE_TEXT_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_PAUSE_TEXT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PAUSE_TEXT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->PauseText((QTV_PAUSE_TEXT_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_TEXT_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_TEXT_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_RESUME_AUDIO_handler

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
void Mpeg4Player::QTV_RESUME_AUDIO_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_RESUME_AUDIO_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_RESUME_AUDIO_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->ResumeAudio((QTV_RESUME_AUDIO_type *)handle))
      {
          pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_AUDIO_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_AUDIO_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_RESUME_VIDEO_handler

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
void Mpeg4Player::QTV_RESUME_VIDEO_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_RESUME_VIDEO_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_RESUME_VIDEO_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->ResumeVideo((QTV_RESUME_VIDEO_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_VIDEO_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_VIDEO_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_RESUME_TEXT_handler

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
void Mpeg4Player::QTV_RESUME_TEXT_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_RESUME_TEXT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_RESUME_TEXT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->ResumeText((QTV_RESUME_TEXT_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_TEXT_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_TEXT_FAILED);
      }
   }
}
#endif /* FEATURE_FILE_FRAGMENTATION || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PS_PARSER_STATUS_EVENT_handler

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
void Mpeg4Player::QTV_PS_PARSER_STATUS_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG( QTVDIAG_MP4_PLAYER,
          "Mpeg4Player::QTV_PS_PARSER_STATUS_EVENT_handler");

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_PS_PARSER_STATUS_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      pThis->ParserStatus((QTV_PS_PARSER_STATUS_EVENT_type *)handle);
   }
}

#endif /*FEATURE_FILE_FRAGMENTATION*/
/* ======================================================================
FUNCTION
  Mpeg4Player::PV_STOP_handler

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
void Mpeg4Player::PV_STOP_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_STOP_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::PV_STOP_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->Stop((PV_STOP_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_STOP_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_STOP_FAILED);
    }
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_NEXT_FRAME_handler

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
void Mpeg4Player::PV_NEXT_FRAME_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_NEXT_FRAME_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_NEXT_FRAME_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->PlayNextFrame((PV_NEXT_FRAME_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_NEXT_FRAME_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_NEXT_FRAME_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_PREV_FRAME_handler

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
void Mpeg4Player::PV_PREV_FRAME_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_PREV_FRAME_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_PREV_FRAME_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->PlayPrevFrame((PV_PREV_FRAME_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PREV_FRAME_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_PREV_FRAME_FAILED);
      }
   }
}

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
/* ======================================================================
FUNCTION
  Mpeg4Player::PV_VIDEO_ROTATE_handler

DESCRIPTION
  Simply call RotateVideo().

  This function tries to follow the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

  Note! If the pThis variable is not set, no callback is notified!

DEPENDENCIES
  pThis must be defined so that RotateVideo can be invoked, and a callback
  notified.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void Mpeg4Player::PV_VIDEO_ROTATE_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_VIDEO_ROTATE_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_VIDEO_ROTATE_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (NULL == pThis) {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::PV_VIDEO_ROTATE_handler: no video player -- no callback!" );
   }
   else
   {
      if (!pThis->RotateVideo((PV_VIDEO_ROTATE_type *)handle))
      {
          // We need to do the notification since RotateVideo returned an
          // error, but we can't.
          pThis->Notify(QtvPlayer::QTV_PLAYER_STATUS_ROTATION_FAILED);
      }

   }
}
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
/* ======================================================================
FUNCTION
  Mpeg4Player::PV_VIDEO_SCALE_handler

DESCRIPTION
  Simply call ScaleVideo().

  This function tries to follow the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

  Note! If the pThis variable is not set, no callback is notified!

DEPENDENCIES
  pThis must be defined so that ScaleVideo can be invoked, and a callback
  notified.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void Mpeg4Player::PV_VIDEO_SCALE_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_VIDEO_SCALE_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_VIDEO_SCALE_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (NULL == pThis)  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::PV_VIDEO_SCALE_handler: no video player -- no callback!" );
   }
   else
   {
      if (!pThis->ScaleVideo((PV_VIDEO_SCALE_type *)handle))
      {
          // We need to do the notification since ScaleVideo returned an
          // error, but we can't.
          pThis->Notify(QtvPlayer::QTV_PLAYER_STATUS_SCALING_FAILED);
      }
   }
}
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_SET_AUDIOOVER_handler

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
void Mpeg4Player::PV_SET_AUDIOOVER_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_SET_AUDIOOVER_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::PV_SET_AUDIOOVER_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    (void)pThis->SetAudioOverride((PV_SET_AUDIOOVER_type *)handle);
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_DELETE_DOWNLOAD_SESSION_handler

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
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void Mpeg4Player::PV_DELETE_DOWNLOAD_SESSION_handler(QCMessageHandleType handle)
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
             "Mpeg4Player::PV_DELETE_DOWNLOAD_SESSION_handler");

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::PV_DELETE_DOWNLOAD_SESSION_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->DeleteDownloadSession((PV_DELETE_DOWNLOAD_SESSION_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_DELETE_DOWNLOAD_SESSION_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_DELETE_DOWNLOAD_SESSION_FAILED);
    }
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_DOWNLOAD_CLIP_handler

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
void Mpeg4Player::PV_DOWNLOAD_CLIP_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
             "Mpeg4Player::PV_DOWNLOAD_CLIP_handler");

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::PV_DOWNLOAD_CLIP_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->DownloadClip((PV_DOWNLOAD_CLIP_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_DOWNLOAD_CLIP_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_DOWNLOAD_CLIP_FAILED);
    }
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_RESUME_DOWNLOAD_handler

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
void Mpeg4Player::PV_RESUME_DOWNLOAD_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
             "Mpeg4Player::PV_RESUME_DOWNLOAD_handler");

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::PV_RESUME_DOWNLOAD_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->ResumeDownload((PV_RESUME_DOWNLOAD_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_DOWNLOAD_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_RESUME_DOWNLOAD_FAILED);
    }
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_STOP_DOWNLOAD_handler

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
void Mpeg4Player::PV_STOP_DOWNLOAD_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
             "Mpeg4Player::PV_STOP_DOWNLOAD_handler");

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::PV_STOP_DOWNLOAD_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    if (pThis->StopDownload((PV_STOP_DOWNLOAD_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_STOP_DOWNLOAD_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_STOP_DOWNLOAD_FAILED);
    }
  }
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
/* ======================================================================
FUNCTION
  Mpeg4Player::VIDEO_STATUS_handler

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
void Mpeg4Player::VIDEO_STATUS_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::VIDEO_STATUS_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::VIDEO_STATUS_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      pThis->VideoStatus((VIDEO_STATUS_type *)handle);
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::AUDIO_STATUS_handler

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
void Mpeg4Player::AUDIO_STATUS_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::AUDIO_STATUS_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::AUDIO_STATUS_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

   if (pThis)
   {
      pThis->AudioStatus((AUDIO_STATUS_type *)handle);
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::AUDIO_TIMING_handler

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
void Mpeg4Player::AUDIO_TIMING_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::AUDIO_TIMING_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::AUDIO_TIMING_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      pThis->AudioTiming((AUDIO_TIMING_type *)handle);
   }
}
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION
  Mpeg4Player::AUDIO_SPEC_handler

DESCRIPTION
  Handles audio status from the audio player

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::AUDIO_SPEC_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::AUDIO_SPEC_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::AUDIO_SPEC_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      pThis->AudioSpec((AUDIO_SPEC_type *)handle);
   }
}
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

/* ======================================================================
FUNCTION
  Mpeg4Player::STREAMER_STATUS_handler

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
void Mpeg4Player::STREAMER_STATUS_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::STREAMER_STATUS_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::STREAMER_STATUS_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

   if (pThis)
   {
      pThis->StreamerStatus((STREAMER_STATUS_type *)handle);
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_MUTE_handler

DESCRIPTION
 This method is invoked to process asynchronous mute command from the
 Qtv Player.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

========================================================================== */
void Mpeg4Player::QTV_MUTE_handler(QCMessageHandleType handle)
{
  QtvPlayer::ReturnT       retVal;
  uint32   bmSucceeded;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                "Mpeg4Player::QTV_MUTE_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_MUTE_handler: Input param handle is NULL");
      return;
   }

  Mpeg4Player *pThis =
    (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    QTV_MUTE_type *muteEv = (QTV_MUTE_type *) handle;

    /* Make sure that the mute combination is valid */
    if(muteEv->eAction == QTV_TRACK_MUTE &&
       pThis->CheckForAllMute(muteEv->bmTrackSelected))
    {
      pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_MUTE_ALL_FAILED);
      return;
    }

    retVal = pThis->MutePlaybackTracks(muteEv->bmTrackSelected,
                                      muteEv->eAction,
                                      &bmSucceeded);
    if(muteEv->eAction == QTV_TRACK_MUTE)
    {
      if (retVal == QtvPlayer::QTV_RETURN_OK)
      {
        pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_MUTE_COMPLETE);
      }
      else
      {
        pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_MUTE_FAILED);
      }
    }
    else /* UnMute Otherwise */
    {
      if (retVal == QtvPlayer::QTV_RETURN_OK)
      {
        pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_COMPLETE);
      }
      else
      {
        pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_UNMUTE_FAILED);
      }
    }
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::CheckForAllMute

DESCRIPTION
  This method is used to check if the mute will result into pause. mute can not
  switch off all the tracks. If we are playing video only clip then mute can not
  be applied. For live media , you can mute all the tracks since pause is not
  applicable there.

DEPENDENCIES
 None.

RETURN VALUE
 TRUE/FALSE.

SIDE EFFECTS
 None.

========================================================================== */
bool Mpeg4Player::CheckForAllMute(uint32  bmTrackSelected)
{
  if(!clip.bHasAudio || (bmTrackSelected & QTV_AUDIO_TRACK_SELECTED))
  {
    if(!clip.bHasVideo || (bmTrackSelected & QTV_VIDEO_TRACK_SELECTED))
    {
      if(!clip.bHasText || (bmTrackSelected & QTV_TEXT_TRACK_SELECTED))
      {
        // Confirming the mute all
        // Check for Live media
        if( !is_live_broadcast( clipInfo.info.streamtype ) )
        {
          /* For recorded or file like media streams no mute all support */
          return true;
        }
      }
    }
  }
  return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::MapMediaCodecToQtvPlayerCodec

DESCRIPTION
//
//  This function returns the codec type which understandable by player application
//

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None

========================================================================== */

QtvPlayer::TrackTypeT Mpeg4Player::MapMediaCodecToQtvPlayerCodec(Media::CodecType codec)
{
  switch(codec)
  {
    case Media::EVRC_CODEC:
      return QtvPlayer::TRACK_AUDIO_EVRC;

    case Media::QCELP_CODEC:
      return QtvPlayer::TRACK_AUDIO_QCELP;

    case Media::AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
      return  QtvPlayer::TRACK_AUDIO_MP4A_LATM;

    case Media::GSM_AMR_CODEC:
      return  QtvPlayer::TRACK_AUDIO_AMR;

#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */

#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */

#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */

#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */

    case Media::MPEG4_CODEC:
      return QtvPlayer::TRACK_VIDEO_MP4V_ES;

    case Media::H263_CODEC:
      return QtvPlayer::TRACK_VIDEO_H263_2000;

#ifdef FEATURE_H264_DECODER
    case Media::H264_CODEC:
      return  QtvPlayer::TRACK_VIDEO_H264;

#endif /* FEATURE_H264_DECODER */
    case Media::STILL_IMAGE_CODEC:
	case Media::STILL_IMAGE_H263_CODEC:	
      return QtvPlayer::TRACK_UNKNOWN;

#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case Media::TIMED_TEXT_CODEC:
      return QtvPlayer::TRACK_3GPP_TIMED_TEXT;
#endif

#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case Media::WMA_CODEC:
      return QtvPlayer::TRACK_WM_AUDIO;


    case Media::WMV1_CODEC:
      return QtvPlayer::TRACK_WM1_VIDEO;

    case Media::WMV2_CODEC:
      return QtvPlayer::TRACK_WM2_VIDEO;

    case Media::WMV3_CODEC:
      return QtvPlayer::TRACK_WM3_VIDEO;

#endif /* FEATURE_H264_DECODER */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    case Media::WMA_PRO_CODEC:
       return QtvPlayer::TRACK_WM_PRO_AUDIO;

    case Media::WMA_PRO_PLUS_CODEC:
       return QtvPlayer::TRACK_WM_PRO_PLUS_AUDIO;
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */

#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */

    case Media::UNKNOWN_CODEC:
      /* fall thru */
    default:
      return QtvPlayer::TRACK_UNKNOWN;
  } /* end of switch(codec) */
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION
  Mpeg4Player::GENERIC_BCAST_STATUS_handler

DESCRIPTION
 This method is invoked to process the asynchronous callback status
 values posted by the generic broadcast media source. When media source
 indicates the callback status generic broadcast object posts the
 equivalent event back to the mpeg4Player object. There is a break
 in media source thread due to this.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

========================================================================== */
void Mpeg4Player::GENERIC_BCAST_STATUS_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                "Mpeg4Player::GENERIC_BCAST_STATUS_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::GENERIC_BCAST_STATUS_handler: Input param handle is NULL");
      return;
   }

  Mpeg4Player *pThis =
    (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    pThis->GenericBcastMediaStatusHandler((GENERIC_BCAST_STATUS_type *) handle);
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_SELECT_PB_TRACK_handler

DESCRIPTION
 This method is invoked to process asynchronous select_pb_track command from the
 Qtv Player.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

========================================================================== */
void Mpeg4Player::QTV_SELECT_PB_TRACK_handler(QCMessageHandleType handle)
{
  QtvPlayer::ReturnT       retVal;
  uint32   bmSucceeded;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                "Mpeg4Player::QTV_SELECT_PB_TRACK_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_SELECT_PB_TRACK_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis =
    (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    QTV_SELECT_PB_TRACK_type *selectEv = (QTV_SELECT_PB_TRACK_type *) handle;
    retVal = pThis->SelectPlaybackTracks(selectEv->bmTrackSelected,
                                         selectEv->nAudioTrackID,
                                         selectEv->nVideoTrackID,
                                         selectEv->nTextTrackID,
                                         bmSucceeded);
    if (retVal != QtvPlayer::QTV_RETURN_OK)
    {
      pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_SELECT_PB_TRACK_FAILED);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_SELECT_PB_TRACK_COMPLETE);
    }
  }
}
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_REPOSITION_handler

DESCRIPTION
 This method is invoked to process asynchronous reposition command from the
 Qtv Player.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

========================================================================== */
void Mpeg4Player::QTV_REPOSITION_handler(QCMessageHandleType handle)
{
  QtvPlayer::ReturnT       retVal;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::QTV_REPOSITION_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_REPOSITION_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis =
    (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    QTV_REPOSITION_type *reposEv = (QTV_REPOSITION_type *) handle;
    retVal = pThis->Reposition(reposEv->nTimestamp,
                               true,
                               reposEv->uTransID);
    if (retVal != QtvPlayer::QTV_RETURN_OK)
    {
      pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_FAILED);
    }
  }
}
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_SEEK_SYNC_handler

DESCRIPTION
 This method is invoked to process asynchronous seek_sync command from the
 Qtv Player.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

========================================================================== */
void Mpeg4Player::QTV_SEEK_SYNC_handler(QCMessageHandleType handle)
{
  QtvPlayer::ReturnT       retVal;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::QTV_SEEK_SYNC_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_SEEK_SYNC_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis =
    (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    QTV_SEEK_SYNC_type *seekEv = (QTV_SEEK_SYNC_type *) handle;
    retVal = pThis->SeekToSync(seekEv->nSyncOffset,
                               true,
                               seekEv->uTransID);
    if (retVal != QtvPlayer::QTV_RETURN_OK)
    {
      pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED);
    }
  }
}
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_PLAY_SYNC_handler

DESCRIPTION
 This method is invoked to process asynchronous play_sync command from the
 Qtv Player.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

========================================================================== */
void Mpeg4Player::QTV_PLAY_SYNC_handler(QCMessageHandleType handle)
{
  QtvPlayer::ReturnT       retVal;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::QTV_PLAY_SYNC_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_PLAY_SYNC_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis =
    (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    QTV_PLAY_SYNC_type *playSyncEv = (QTV_PLAY_SYNC_type *) handle;
    retVal = pThis->PlaySyncFrame(playSyncEv->nSyncOffset,
                                  playSyncEv->uTransID);
    if (retVal != QtvPlayer::QTV_RETURN_OK)
    {
      pThis->Notify(QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED);
    }
  }
}
#endif /* FEATURE_QTV_GENERIC_BCAST */

/* ======================================================================
FUNCTION
  Mpeg4Player::DOWNLOAD_STATUS_handler

DESCRIPTION
 This method is invoked to process asynchronous play_sync command from the
 Qtv Player.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

========================================================================== */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void Mpeg4Player::DOWNLOAD_STATUS_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::DOWNLOAD_STATUS_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::DOWNLOAD_STATUS_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    pThis->DownloadStatus((DOWNLOAD_STATUS_type *)handle);
  }
}
/* ======================================================================
FUNCTION
  Mpeg4Player::DOWNLOAD_FILE_SIZE_STATUS_handler

DESCRIPTION
  This method is invoked to process asynchronous event from Downloader.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::DOWNLOAD_FILE_SIZE_STATUS_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::DOWNLOAD_FILE_SIZE_STATUS_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::DOWNLOAD_FILE_SIZE_STATUS_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    pThis->DownloadFileSizeStatus((DOWNLOAD_FILE_SIZE_STATUS_type *)handle);
  }

}

/* ======================================================================
FUNCTION
  Mpeg4Player::DownloadFileSizeStatus

DESCRIPTION
  This method will check the event status from downloader and based on that
  notify the player.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::DownloadFileSizeStatus(const DOWNLOAD_FILE_SIZE_STATUS_type *pEvent )
{
  unsigned long size=0;

  if(pEvent)
  {
    size=GetPVDLFilesize();
    if (size>0)
    {
      SetPVDLFilesize(size);
      Notify(DOWNLOADING);
      bDownloading = true;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "PDS2:Downloader not started");
      DownloadComplete(ABORT);
    }

  }
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION
  Mpeg4Player::CPVTEXT_STATUS_handler

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
void Mpeg4Player::CPVTEXT_STATUS_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::CPVTEXT_STATUS_handler" );

   if(handle == NULL)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::CPVTEXT_STATUS_handler: Input param handle is NULL");
     return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

   if (pThis)
   {
      pThis->TimedTextStatus((CPVTEXT_STATUS_type *)handle);
   }
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_SKIP_CLIP_handler

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
void Mpeg4Player::QTV_SKIP_CLIP_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_SKIP_CLIP_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_SKIP_CLIP_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    if (pThis->SkipClip((QTV_SKIP_CLIP_type *)handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_SKIP_CLIP_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_SKIP_CLIP_FAILED);
    }
  }
}
#endif /*FEATURE_FILE_FRAGMENTATION*/

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_SUSPEND_handler

DESCRIPTION
  Handles QTV_SUSPEND event.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::QTV_SUSPEND_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                "Mpeg4Player::QTV_SUSPEND_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_SUSPEND_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

   if (pThis)
   {
      if (!pThis->Suspend((QTV_SUSPEND_type *)handle))
      {
        if(((QTV_SUSPEND_type*)handle)->bSuspendIsExternal == true)
        { /* This is the case when QTV_COnfig::Pause_Is_Suspend is true.
           In this case, external player is expecting PAUSE_FAILED. */
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
              "Mpeg4Player::QTV_SUSPEND returns PAUSE_FAILED" );
           pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_FAILED);
        }
        else
        {
           pThis->Notify(QtvPlayer::QTV_PLAYER_STATUS_SUSPEND_FAILED);
        }
       }
       else
       {
          if(((QTV_SUSPEND_type*)handle)->bSuspendIsExternal == true)
          {
            pThis->Notify(QtvPlayer::QTV_COMMAND_PAUSE_COMPLETE);
          }
       }
    }

}

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_RESUME_handler

DESCRIPTION
  Handles QTV_RESUME event.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

========================================================================== */
void Mpeg4Player::QTV_RESUME_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::QTV_RESUME_handler" );

  Mpeg4Player *pThis =
    (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    if ( !(pThis->Resume((QTV_RESUME_type *)handle)) )
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_PLAY_CLIP_FAILED);
    }
  }
}

#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */

#ifdef FEATURE_QTV_WM_DRM_API
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_handler

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
void Mpeg4Player::QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

   if (pThis)
   {
      if (pThis->RegisterDRMDecryptMethod((QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_COMPLETE);
      } else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_REGISTER_DRM_DECRYPT_METHOD_FAILED);
      }
   }
}
#endif /* FEATURE_QTV_WM_DRM_API */

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_SKIP_handler

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
#ifdef FEATURE_QTV_PLAYLIST
void Mpeg4Player::QTV_SKIP_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_SKIP_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_SKIP_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    if (pThis->Skip((QTV_SKIP_type *) handle))
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_COMPLETE);
    }
    else
    {
      pThis->Notify(QtvPlayer::QTV_COMMAND_PLAYLIST_PLAY_FAILED);
    }
  }
}
#endif /* FEATURE_QTV_PLAYLIST */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_COPY_BUFFER_TO_FILE_handler

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
void Mpeg4Player::QTV_COPY_BUFFER_TO_FILE_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_COPY_BUFFER_TO_FILE_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_COPY_BUFFER_TO_FILE_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;

  if (pThis)
  {
    if (pThis->CopyBufferToFile((QTV_COPY_BUFFER_TO_FILE_type *)handle))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::CopyBufferToFile Success" );
      pThis->Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_COMPLETE);
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::CopyBufferToFile Failed" );
      pThis->Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED);
    }
    if (((QTV_COPY_BUFFER_TO_FILE_type *)handle)->fullpathtoFilename)
    {
       QTV_Free(((QTV_COPY_BUFFER_TO_FILE_type *)handle)->fullpathtoFilename);
    }
  }
}
#endif

/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_AUDIO_RESTARTING_TRACK_handler

DESCRIPTION
  Event handler for event QTV_AUDIO_RESTARTING_TRACK.

PARAMETERS
  QCMessageHandleType
    Not handled by the function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void Mpeg4Player::QTV_AUDIO_RESTARTING_TRACK_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_AUDIO_RESTARTING_TRACK_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::QTV_AUDIO_RESTARTING_TRACK_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    pThis->Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_RESTARTING_TRACK);
  }
}

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_HTTP_REGISTER_CALLBACK_EVENT_handler

DESCRIPTION
  Event handler for register callback event posted by OEM

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::QTV_HTTP_REGISTER_CALLBACK_EVENT_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::QTV_HTTP_REGISTER_CALLBACK_EVENT_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_HTTP_REGISTER_CALLBACK_EVENT_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
     pThis->HTTPRegisterCallback((QTV_HTTP_REGISTER_CALLBACK_EVENT_type *)handle);
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::HTTP_DOWNLOAD_STATUS_handler

DESCRIPTION
  Event handling for differnt status handling

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::HTTP_DOWNLOAD_STATUS_handler(QCMessageHandleType handle)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::DOWNLOAD_STATUS_handler" );

  if(handle == NULL)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
       "Mpeg4Player::HTTP_DOWNLOAD_STATUS_handler: Input param handle is NULL");
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
    pThis->HttpDownloadStatus((HTTP_DOWNLOAD_STATUS_type *)handle);
  }
}
#endif

////////////////////////////////////////////////////////////////////
//
// player thread routines
//
////////////////////////////////////////////////////////////////////

#ifdef SHOW_INFO
/* ======================================================================
FUNCTION
  Mpeg4Player::ShowClip

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
void Mpeg4Player::ShowClip()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ShowClip" );
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "****** Clip :" );
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Audio: %d",(int)clip.bHasAudio, 0, 0 );
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Video: %d",(int)clip.bHasVideo, 0, 0 );
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Text:  %d",(int)clip.bHasText, 0, 0 );
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Still: %d",(int)clip.bStillImage, 0, 0 );
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Streaming: %d",(int)clip.bStreaming, 0, 0 );

#ifdef FEATURE_QTV_GENERIC_BCAST_ISDB
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Broadcast ISDB: %d",(int)clip.bBcastISDB,0,0);
#endif /* FEATURE_QTV_GENERIC_BCAST_ISDB */

#ifdef FEATURE_QTV_GENERIC_BCAST
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                 "Generic Bcast: %s",
                 ConvertGenericMediaSourceToStr(clib.eGenericBcastMediaID));
#endif /* FEATURE_QTV_GENERIC_BCAST */

   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Broadcast FLO: %d",(int)clip.bBcastFLO,0,0);

   ShowClipInfo();
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ShowClipInfo

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
void Mpeg4Player::ShowClipInfo()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ShowClipInfo" );

   if (clipInfo.bValid)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "");
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "****** Clip Info:");
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "  Valid %d",clipInfo.bValid);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "  Stream Type: %d",clipInfo.info.streamtype);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "  Repos %d",clipInfo.info.RepositioningAllowed);
      switch (clipInfo.info.MediaType)
      {
         case QtvPlayer::MEDIA_AUDIO_ONLY:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_AUDIO_ONLY");
            break;
         case QtvPlayer::MEDIA_VIDEO_ONLY:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_VIDEO_ONLY");
            break;
         case QtvPlayer::MEDIA_AUDIO_VIDEO:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_AUDIO_VIDEO");
            break;
         case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_VIDEO_STILL_IMAGE");
            break;
         case QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_AUDIO_VIDEO_STILL_IMAGE");
            break;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
         case QtvPlayer::MEDIA_VIDEO_TEXT:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_VIDEO_TEXT");
            break;
         case QtvPlayer::MEDIA_AUDIO_TEXT:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_AUDIO_TEXT");
            break;
         case QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_AUDIO_VIDEO_TEXT");
            break;
         case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_VIDEO_STILL_IMAGE_TEXT");
            break;
         case QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT");
            break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
         default:
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   MEDIA_???");
            break;
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Width %d",clipInfo.info.Width);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Height %d",clipInfo.info.Height);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Duration %d",clipInfo.info.Duration);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Size %d",clipInfo.info.Size);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Audio Sampling Frequecy %d",clipInfo.info.AudioSamplingFreq);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Number Of Audio Channels %d",clipInfo.info.NumAudioChannels);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Video Frame Rate %d",(uint32)clipInfo.info.VideoFrameRate);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Video Encoded Bit Rate %d",clipInfo.info.VideoEncodedBitRate);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Audio Encoded Bit Rate %d",clipInfo.info.AudioEncodedBitRate);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Title ::%s::",clipInfo.info.ExtInfo.StdInfo.Title);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Author ::%s::",clipInfo.info.ExtInfo.StdInfo.Author);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Copyright ::%s::",clipInfo.info.ExtInfo.StdInfo.Copyright);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Rating ::%s::",clipInfo.info.ExtInfo.StdInfo.Rating);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Description ::%s::",clipInfo.info.ExtInfo.StdInfo.Description);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Performance ::%s::",clipInfo.info.ExtInfo.StdInfo.Performance);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Genre ::%s::",clipInfo.info.ExtInfo.StdInfo.Genre);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Classification ::%s::",clipInfo.info.ExtInfo.StdInfo.Classification);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Keyword ::%s::",clipInfo.info.ExtInfo.StdInfo.Keyword);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Location ::%s::",clipInfo.info.ExtInfo.StdInfo.Location);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Album ::%s::",clipInfo.info.ExtInfo.StdInfo.Album);
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Track Count ::%d::",clipInfo.info.TrackCount);
      for (i = 0; i < clipInfo.info.TrackCount; i++)
      {
         QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "   Track %d MIME Type ::%s::",clipInfo.info.TrackMIMEType[i]);
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "");
   } else
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "No valid Clip Info to report");

}
#endif

/* ======================================================================
FUNCTION
  Mpeg4Player::SetImageDimensions

DESCRIPTION
//video player calls this when dimensions are available.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::SetImageDimensions(int width,int height)
{
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetImageDimensions %d %d",width,height);
  if(width && height)
  {
    QCUtils::EnterCritSect(&clipInfo_CS);
    clipInfo.info.Width=width;
    clipInfo.info.Height=height;
    QCUtils::LeaveCritSect(&clipInfo_CS);
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetClipExtInfo

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
QtvPlayer::ClipExtInfoKindT Mpeg4Player::GetClipExtInfo(Media * pMedia, ClipInfo &clipInfo)
{ /*lint !e578 */
   // Make static to avoid stack overflow
   static QtvPlayer::ClipExtInfoT tempExtInfo;
   QtvPlayer::ClipExtInfoKindT extInfoKind = QtvPlayer::CLIP_EXT_INFO_NONE;
   OSCL_String<char> oTemp;
   int i;

   memset(&tempExtInfo, 0, sizeof(tempExtInfo));

#ifdef FEATURE_MP4_KDDI_META_DATA
   if ( pMedia  && pMedia->HasClipMetaData() )
   {
      /* only file media can has meta data */
      FileMedia * pfm = (FileMedia *)pMedia;
      QtvPlayer::ClipInfoKDDIExt *pKDDIExtInfo = &(tempExtInfo.KDDIInfo);

      pKDDIExtInfo->CopyProhibitionFlag=pfm->GetCopyProhibitionFlag();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->ValidityEffectiveDate=pfm->GetValidityEffectiveDate();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->ValidityPeriod=pfm->GetValidityPeriod();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->NumberOfAllowedPlayBacks=pfm->GetNumberOfAllowedPlayBacks();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->AuthorDLLVersion=pfm->GetAuthorDLLVersion();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->EditFlags=pfm->GetEditFlags();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->RecordingMode=pfm->GetRecordingMode();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->RecordingDate=pfm->GetRecordingDate();
      (void)pfm->CheckError(Common::GETINFO);

      if ( pfm->GetContentPropertyTitle(oTemp) )
      {
         (void)pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->ContentPropertyTitle,oTemp.c_str(),QTV_MAX_CONTENT_PROPERTY_TITLE_BYTES);
      }

      if ( pfm->GetContentPropertyCopyRight(oTemp) )
      {
         (void) pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->ContentPropertyCopyRight,oTemp.c_str(),QTV_MAX_CONTENT_PROPERTY_COPYRIGHT_BYTES);
      }

      if ( pfm->GetContentPropertyAuthor(oTemp) )
      {
         (void)pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->ContentPropertyAuthor,oTemp.c_str(),QTV_MAX_CONTENT_PROPERTY_AUTHOR_BYTES);
      }

      if ( pfm->GetContentPropertyMemo(oTemp) )
      {
         (void)pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->ContentPropertyMemo,oTemp.c_str(),QTV_MAX_CONTENT_PROPERTY_MEMO_BYTES);
      }

      unsigned char *bytePtr = NULL;

      pfm->GetDeviceName(&bytePtr);
      (void)pfm->CheckError(Common::GETINFO);
      if (bytePtr)
      {
      for ( i=0; i<QTV_MAX_DEVICE_NAME_BYTES; i++)
         {
            pKDDIExtInfo->DeviceName[i] = *(bytePtr+i);
         }
      }

      pfm->GetModelName(&bytePtr);
      (void)pfm->CheckError(Common::GETINFO);
      if (bytePtr)
      {
         for (int j=0; j<QTV_MAX_MODEL_NAME_BYTES; j++)
         {
            pKDDIExtInfo->ModelName[j] = *(bytePtr+j);
         }
      }

      pfm->GetEncoderInformation(&bytePtr);
      (void)pfm->CheckError(Common::GETINFO);
      if (bytePtr)
      {
         for (int k=0; k<QTV_MAX_ENCODER_INFO_BYTES; k++)
         {
            pKDDIExtInfo->EncoderInformation[k] = *(bytePtr+k);
         }
      }

      pfm->GetMuxInformation(&bytePtr);
      (void)pfm->CheckError(Common::GETINFO);
      if (bytePtr)
      {
         for (int m=0; m<QTV_MAX_MUX_INFO_BYTES; m++)
         {
            pKDDIExtInfo->MuxInformation[m] = *(bytePtr+m);
         }
      }

      // The remainder are for the GPS metadata atoms
      pKDDIExtInfo->GPSByteOrder=pfm->GetGPSByteOrder();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->GPSVersionID = pfm->getGPSVersionID();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->GPSLatitudeRef = pfm->getGPSLatitudeRef();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->GPSLongitudeRef = pfm->getGPSLongitudeRef();
      (void)pfm->CheckError(Common::GETINFO);

      pKDDIExtInfo->GPSAltitudeRef = pfm->getGPSAltitudeRef();
      (void)pfm->CheckError(Common::GETINFO);

      uint64 *GPSLongDataPtr;

      GPSLongDataPtr = pfm->GetGPSLatitudeArray();
      (void)pfm->CheckError(Common::GETINFO);
      if (GPSLongDataPtr)
      {
         for (i=0; i<QTV_MAX_GPS_LATITUDE; i++)
         {
            pKDDIExtInfo->GPSLatitudeArray[i] = *(GPSLongDataPtr+i);
         }
      }

      GPSLongDataPtr = pfm->GetGPSLongitudeArray();
      (void)pfm->CheckError(Common::GETINFO);
      if (GPSLongDataPtr)
      {
         for (i=0; i<QTV_MAX_GPS_LONGITUDE; i++)
         {
            pKDDIExtInfo->GPSLongitudeArray[i] = *(GPSLongDataPtr+i);
         }
      }

      pKDDIExtInfo->GPSAltitude = pfm->GetGPSAltitude();
      (void)pfm->CheckError(Common::GETINFO);

      GPSLongDataPtr = pfm->GetGPSTimeArray();
      (void)pfm->CheckError(Common::GETINFO);
      if (GPSLongDataPtr)
      {
         for (i=0; i<QTV_MAX_GPS_TIME; i++)
         {
            pKDDIExtInfo->GPSTimeArray[i] = *(GPSLongDataPtr+i);
         }
      }

      if ( pfm->GetGPSSurveyData(oTemp) )
      {
         (void)pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->GPSSurveyData,oTemp.c_str(),QTV_MAX_GPS_SURVEY_DATA_BYTES);
      }

      if ( pfm->GetPositioningMethod(oTemp) )
      {
         (void) pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->GPSPositioningMethod,oTemp.c_str(),QTV_MAX_GPS_POSITIONING_METHOD_BYTES);
      }

      if ( pfm->GetPositioningName(oTemp) )
      {
         (void)pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->GPSPositioningName,oTemp.c_str(),QTV_MAX_GPS_POSITIONING_NAME_BYTES);
      }

      if ( pfm->GetGPSDate(oTemp) )
      {
         (void)pfm->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pKDDIExtInfo->GPSDate,oTemp.c_str(),QTV_MAX_GPS_DATE_BYTES);
      }

      pKDDIExtInfo->GPSExtensionMapScaleInfo=pfm->GetGPSExtensionMapScaleInfo();
      (void)pfm->CheckError(Common::GETINFO);

      //if any of the KDDI metadata atoms are present, then it is KDDI file
      if ( (pKDDIExtInfo->ContentPropertyTitle[0] != '\0') ||
           (pKDDIExtInfo->ContentPropertyCopyRight[0] != '\0') ||
           (pKDDIExtInfo->ContentPropertyAuthor[0] != '\0') ||
           (pKDDIExtInfo->ContentPropertyMemo[0] != '\0') ||
           (pKDDIExtInfo->DeviceName[0] != '\0') ||
           (pKDDIExtInfo->ModelName[0] != '\0') ||
           (pKDDIExtInfo->EncoderInformation[0] != '\0') ||
           (pKDDIExtInfo->MuxInformation[0] != '\0') ||
           (pKDDIExtInfo->CopyProhibitionFlag != 0) ||
           (pKDDIExtInfo->ValidityEffectiveDate != 0) ||
           (pKDDIExtInfo->ValidityPeriod != 0) ||
           (pKDDIExtInfo->NumberOfAllowedPlayBacks != 0) ||
           (pKDDIExtInfo->AuthorDLLVersion != 0) ||
           (pKDDIExtInfo->EditFlags != 0) ||
           (pKDDIExtInfo->RecordingMode != 0) ||
           (pKDDIExtInfo->RecordingDate != 0) ||
           /* only one mandoty GPS field is sufficient */
           (pKDDIExtInfo->GPSLatitudeRef != 0)
         )
      {
         extInfoKind = QtvPlayer::CLIP_EXT_INFO_KDDI;
      }
   } /* end of if(clip has meta data) */
#endif /* FEATURE_MP4_KDDI_META_DATA */
#ifdef FEATURE_QTV_DIVX_DRM
   if ( pMedia  && pMedia->isAviFileInstance() && pMedia->IsDRMProtection() )
   {
     /*
     * Clip is DRM protected, populate clip specific
     * DRM informtaion.OEM can retrieve extended clip info to get details.
     */
     FileMedia * pfm = (FileMedia *)pMedia;
     QtvPlayer::ClipDrmInfoT *pClipDivxDrmInfo = &(tempExtInfo.DrmInfo);
     pMedia->GetClipDrmInfo((void*)pClipDivxDrmInfo);
     extInfoKind = QtvPlayer::CLIP_EXT_DIVX_DRM_INFO;
     QTV_USE_ARG1(pfm);
   }
#endif

   /* if no carrier specific meta data is present, get standard extended info */
   if (extInfoKind == QtvPlayer::CLIP_EXT_INFO_NONE)
   {
      QtvPlayer::ClipInfoStandardExt *pStdExtInfo;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      if(clip.bDownload && appData.bValid )
      {
          pStdExtInfo                           = &(tempExtInfo.PvxInfo.StdInfo);
          tempExtInfo.PvxInfo.AllowUserPlayback = appData.data.UserPlaybackAllowed;
          tempExtInfo.PvxInfo.DownloadPlaybackControl  = appData.data.DownloadPlaybackControl;
          if (pvxIF.appFile.PurgeDownloadFile(pvxIF.nPlaylistIndex))
          {
                    tempExtInfo.PvxInfo.PurgeDowloadedFile = true ;
          }
          extInfoKind                           = QtvPlayer::CLIP_EXT_INFO_PVX;
      }
      else
#endif /*FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2*/
#if 0 /*will be removed once the support is added for the new mpeg4spec in layers above*/
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      if (clip.bQtvHttpStreaming)
      {
          pStdExtInfo                           = &(tempExtInfo.PDInfo.StdInfo);
          tempExtInfo.PDInfo.PurgeDowloadedFile = http_bContentPurge;
          extInfoKind                           = QtvPlayer::CLIP_EXT_INFO_PD;
      }
      else
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
#endif /*if 0*/
      {
          pStdExtInfo = &(tempExtInfo.StdInfo);
          extInfoKind = QtvPlayer::CLIP_EXT_INFO_STANDARD;
      }

      if ( pMedia && pMedia->GetTitle(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Title,oTemp.c_str(),QTV_MAX_TITLE_BYTES);
      }

      if ( pMedia && pMedia->GetAuthor(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Author,oTemp.c_str(),QTV_MAX_AUTHOR_BYTES);
      }

      if ( pMedia && pMedia->GetCopyright(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Copyright,oTemp.c_str(),QTV_MAX_COPYRIGHT_BYTES);
      }

      if ( pMedia && pMedia->GetRating(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Rating,oTemp.c_str(),QTV_MAX_RATING_BYTES);
      }

      if ( pMedia && pMedia->GetDescription(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Description,oTemp.c_str(),QTV_MAX_DESCRIPTION_BYTES);
      }

      if ( pMedia && pMedia->GetOwner(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Owner,oTemp.c_str(),QTV_MAX_OWNER_BYTES);
      }

      if ( pMedia && pMedia->GetVersion(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Version,oTemp.c_str(),QTV_MAX_VERSION_BYTES);
      }

      if ( pMedia && pMedia->GetCreationDate(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->CreationDate,oTemp.c_str(),QTV_MAX_CREATIONDATE_BYTES);
      }

      if ( pMedia && pMedia->GetArtist(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Artist,oTemp.c_str(),QTV_MAX_AUTHOR_BYTES);
      }

      if ( pMedia && pMedia->GetSTitle(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->sTitle,oTemp.c_str(),QTV_MAX_TITLE_BYTES);
      }

      if ( pMedia && pMedia->GetAlbum(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Album,oTemp.c_str(),QTV_MAX_ALBUM_BYTES);
      }

      if ( pMedia && pMedia->GetInfo(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Info,oTemp.c_str(),QTV_MAX_INFO_BYTES);
      }
      if ( pMedia && pMedia->GetPerf(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Performance,oTemp.c_str(),QTV_MAX_PERF_BYTES);
      }
      if ( pMedia && pMedia->GetGenre(oTemp) )
      {
        (void)pMedia->CheckError(Common::GETINFO);
        ZUtils::StrcpyN(pStdExtInfo->Genre,oTemp.c_str(),QTV_MAX_GENRE_BYTES);
      }
      if ( pMedia && pMedia->GetClsf(oTemp) )
      {
        (void)pMedia->CheckError(Common::GETINFO);
        ZUtils::StrcpyN(pStdExtInfo->Classification,oTemp.c_str(),QTV_MAX_CLSF_BYTES);
      }
      if ( pMedia && pMedia->GetKywd(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Keyword,oTemp.c_str(),QTV_MAX_KYWD_BYTES);
      }
      if ( pMedia && pMedia->GetLoci(oTemp) )
      {
        (void)pMedia->CheckError(Common::GETINFO);
        ZUtils::StrcpyN(pStdExtInfo->Location,oTemp.c_str(),QTV_MAX_LOCI_BYTES);
      }
      if ( pMedia && pMedia->GetYrrc(oTemp) )
      {
         (void)pMedia->CheckError(Common::GETINFO);
         ZUtils::StrcpyN(pStdExtInfo->Yrrc,oTemp.c_str(),QTV_MAX_YRRC_BYTES);
      }

      if (clip.bStreaming)
      {
        if (pMedia->GetLaunchURL(oTemp))
        {
          (void)pMedia->CheckError(Common::GETINFO);
          ZUtils::StrcpyN(pStdExtInfo->LaunchURL,oTemp.c_str(),QTV_MAX_URN_BYTES);
        }
        pStdExtInfo->EndofClipAction = pMedia->GetEndOfClipAction();
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
        STREAMMEDIA *streamMedia = (STREAMMEDIA *) pMedia;
        pStdExtInfo->NumClipsInServerSidePlaylist =
          streamMedia->GetNumClipsInServerSidePlaylist();
        pStdExtInfo->IsPrevClipInServerSidePlaylistAvail =
          streamMedia->IsPreviousClipInServerSidePlaylistAvailable();
        pStdExtInfo->IsNextClipInServerSidePlaylistAvail =
          streamMedia->IsNextClipInServerSidePlaylistAvailable();
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
      }
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      else if(clip.bDownload && clip.pMpeg4Download)
      {
          if (clip.pMpeg4Download->GetLaunchURL(oTemp))
          {
            (void)clip.pMpeg4Download->CheckError(Common::GETINFO);
            ZUtils::StrcpyN(pStdExtInfo->LaunchURL,oTemp.c_str(),QTV_MAX_URN_BYTES);
          }
          pStdExtInfo->EndofClipAction = clip.pMpeg4Download->GetEndOfClipAction();
      }
#endif
      else
      {
          pStdExtInfo->NumClipsInServerSidePlaylist = 0;
          pStdExtInfo->IsPrevClipInServerSidePlaylistAvail = 0;
          pStdExtInfo->IsNextClipInServerSidePlaylistAvail = 0;
      }
   }

   // Save clip info
   clipInfo.info.ExtInfoType = extInfoKind;
   clipInfo.info.ExtInfo = tempExtInfo;

   return extInfoKind;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetClipInfo

DESCRIPTION
//
//  This sets the clip info
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::SetClipInfo(Media * pVideo, int VideoFileSize, Media * pAudio, int AudioFileSize, Media * pText, int TextFileSize)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetClipInfo" );

  // Make static to avoid stack overflow
  ClipInfo tempInfo;

  memset(&tempInfo, 0, sizeof(tempInfo));
  tempInfo.bValid=false;

  if (pVideo || pAudio || pText)
  {
    Media *pMedia = NULL;
    Media::MediaFileType eVideoFileType = Media::CONTENT_UNKNOWN;
    Media::MediaFileType eAudioFileType = Media::CONTENT_UNKNOWN;
    Media::MediaFileType eTextFileType = Media::CONTENT_UNKNOWN;
    int     filesize;

    filesize = MAX(VideoFileSize, AudioFileSize);
    filesize = MAX(filesize, TextFileSize);

    if (pVideo)
    {
      pMedia = pVideo;
    }
    else if(pAudio)
    {
      pMedia = pAudio;
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else if ( pText)
    {
      pMedia = pText;
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifndef FEATURE_QTV_GENERIC_BCAST
    if (clip.bBcastFLO || clip.bBcastISDB)
    {
      /*---------------------------------------------------------------------
        Broadcast is always live.  Truly, we're not streaming here,
        but the important part of this flag is the "live" part.
      ---------------------------------------------------------------------*/
      tempInfo.info.streamtype = QtvPlayer::MEDIA_LIVE_STREAMING;
    }
#endif

    //width and height get set later so zeroize for now.
    tempInfo.info.Width=0;
    tempInfo.info.Height=0;

    if (clip.bStreaming) /* If it's a streaming clip then,*/
    {
      /* check if it is a live stream */
      if (pMedia->IsLive())
      {
        /* Set the clip stream type to Live */
        tempInfo.info.streamtype = QtvPlayer::MEDIA_LIVE_STREAMING;
      }
      else
      {
        /* Not a live stream. Se the clip type to non-live stream */
        tempInfo.info.streamtype = QtvPlayer::MEDIA_STREAMING;

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
        STREAMMEDIA *streamMedia = (STREAMMEDIA *) pMedia;
        ServerSidePlaylistType playlistType =
          streamMedia->GetServerSidePlaylistType();

        switch (playlistType)
        {
#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
          case SERVER_SIDE_PLAYLIST_TYPE_PV:
          tempInfo.info.streamtype = QtvPlayer::MEDIA_PV_SERVER_SIDE_PLAYLIST;
          break;
#endif /* FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST */

#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
          case SERVER_SIDE_PLAYLIST_TYPE_WM:
          tempInfo.info.streamtype = QtvPlayer::MEDIA_WM_SERVER_SIDE_PLAYLIST;
          break;
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */

          case SERVER_SIDE_PLAYLIST_TYPE_NONE:
          default:
          // leave as QtvPlayer::MEDIA_STREAMING
          break;
        }
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
      }
    }
    else    /* Not stream media */
    {
      if (pVideo)
      {
        eVideoFileType = pVideo->GetFileType();
      }

      if (pAudio)
      {
        eAudioFileType = pAudio->GetFileType();
      }

      if (pText)
      {
        eTextFileType = pText->GetFileType();
      }

      if(eVideoFileType != Media::CONTENT_UNKNOWN)
      {
        pMedia = pVideo;
      }
      else if(eAudioFileType != Media::CONTENT_UNKNOWN)
      {
        pMedia = pAudio;
      }
      else if(eTextFileType != Media::CONTENT_UNKNOWN)
      {
        pMedia = pText;
      }
#ifndef FEATURE_QTV_GENERIC_BCAST
      /* Local clip. Set the clip type to Non streaming  */
      tempInfo.info.streamtype = QtvPlayer::MEDIA_NON_STREAMING;
#else
      /*---------------------------------------------------------------------
        Broadcast is not always live.
      ---------------------------------------------------------------------*/
      if (clip.bBcastStream && pMedia && pMedia->IsLive())
      {
        if ( pMedia->IsRepositioningAllowed() )
        {
          tempInfo.info.streamtype = QtvPlayer::MEDIA_BUFFERED_LIVE_STREAMING;
        }
        else
        {
          tempInfo.info.streamtype = QtvPlayer::MEDIA_LIVE_STREAMING;
        }
        clip.bLiveBcast = true;
      }
      else
      {
        /* Local clip. Set the clip type to Non streaming  */
        tempInfo.info.streamtype = QtvPlayer::MEDIA_NON_STREAMING;
      }
#endif
    } // if (clip.bStreaming)

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
    if (clip.bDownload || clip.bHTTPStreaming || clip.bQtvHttpStreaming)
    {
      tempInfo.info.streamtype = QtvPlayer::MEDIA_STREAMING;
    }
#endif /* FEATURE_QTV_HTTP_DOWNLOAD */

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Clip Stream Type: %d",
                  tempInfo.info.streamtype);

    Media::MediaFileType eMp4Filetype = Media::CONTENT_UNKNOWN;
    bool bError = false;

    /* Below if condition is added to fix klocwork warnings */
    if (pMedia != NULL)
    {
      (void)pMedia->CheckError(Common::GETINFO);

      tempInfo.info.RepositioningAllowed=pMedia->IsRepositioningAllowed();
      (void)pMedia->CheckError(Common::GETINFO);
#ifdef FEATURE_QTV_PDCF
    tempInfo.info.encryptionType=pMedia->getEncryptionType();
    (void)pMedia->CheckError(Common::GETINFO);
#endif /* FEATURE_QTV_PDCF */

      tempInfo.info.VideoOnlyPlaybackAllowed=pMedia->IsVideoOnlyPlaybackAllowed();
      (void)pMedia->CheckError(Common::GETINFO);

      tempInfo.info.AudioOnlyPlaybackAllowed=pMedia->IsAudioOnlyPlaybackAllowed();
      (void)pMedia->CheckError(Common::GETINFO);

      tempInfo.info.Size=filesize;

      tempInfo.info.Duration=pMedia->GetDurationMsec();
      (void)pMedia->CheckError(Common::GETINFO);
      // put the movie duration in the paylaod
      qtv_event_clip_started_payload.duration = tempInfo.info.Duration;

      //Get media type
      eMp4Filetype = pMedia->GetFileType();
      bError = pMedia->CheckError(Common::GETFILETYPE);
    }
    if (bError)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Mpeg4Player::SetClipInfo can't get filetype");
    }
    tempInfo.info.MediaType = GetPVPlayerMediaType(eMp4Filetype);

    // Even if a clip has no supported tracks, the clip info is still valid.
    tempInfo.bValid = true;

    /*if (tempInfo.info.MediaType != QtvPlayer::MEDIA_INVALID)
    {
      tempInfo.bValid=true;
    } */

    // Get MIME Types
    ReferenceCountedPointer<ITrackList> trackList;

    if ((pMedia != NULL && !pMedia->GetTrackList(MakeOutPointer(trackList))) ||
        (trackList == NULL))
    {
      return;
    }

    int32 numTracks = trackList->GetNumTracksAvailable();

    if (numTracks > QTV_MAX_MEDIA_TRACKS)
    {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Track count (%d) exceeds max tracks (%d)", numTracks,
        QTV_MAX_MEDIA_TRACKS);
      numTracks = QTV_MAX_MEDIA_TRACKS;
    }

    tempInfo.info.TrackCount = numTracks;

    for (uint32 i = 0; i < numTracks; i++)
    {
      switch (trackList->GetCodecType(i))
      {
        case Media::EVRC_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_AUDIO_EVRC;
          break;
        case Media::QCELP_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_AUDIO_QCELP;
          break;
        case Media::AAC_CODEC:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_AUDIO_MP4A_LATM;
          break;
        case Media::GSM_AMR_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_AUDIO_AMR;
          break;
#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */
#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */
#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */
#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */

        case Media::MPEG4_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_VIDEO_MP4V_ES;
          break;
        case Media::H263_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_VIDEO_H263_2000;
          break;
#ifdef FEATURE_H264_DECODER
        case Media::H264_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_VIDEO_H264;
          break;
#endif /* FEATURE_H264_DECODER */
#ifdef FEATURE_DIVX_311_ENABLE
        case Media::DIVX311_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_VIDEO_DIVX311;
          break;
#endif
        case Media::STILL_IMAGE_CODEC:
		case Media::STILL_IMAGE_H263_CODEC:		
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_UNKNOWN;
          break;
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        case Media::TIMED_TEXT_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_3GPP_TIMED_TEXT;
          break;
#endif
#ifdef FEATURE_QTV_WINDOWS_MEDIA
        case Media::WMA_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_WM_AUDIO;
          break;

        case Media::WMV1_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_WM1_VIDEO;
          break;

        case Media::WMV2_CODEC:
             tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_WM2_VIDEO;
             break;

         case Media::WMV3_CODEC:
             tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_WM3_VIDEO;
          break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
        case Media::WMA_PRO_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_WM_PRO_AUDIO;
          break;
        case Media::WMA_PRO_PLUS_CODEC:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_WM_PRO_PLUS_AUDIO;
          break;
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */

        case Media::UNKNOWN_CODEC:
        default:
          tempInfo.info.TrackTypes[i] = QtvPlayer::TRACK_UNKNOWN;
          break;
      } /* end of switch(codec) */
    } /* end of for(numTracks) */

    /* get extended clip info */
    (void)GetClipExtInfo(pMedia, tempInfo);

    /* now over-ride some audio video info for local file playback */
    if (  (videoURN.type==QtvPlayer::URN_LOCAL_FILE) || (audioURN.type==QtvPlayer::URN_LOCAL_FILE) ||
          (videoURN.type==QtvPlayer::URN_LOCAL_BUFFER) || (audioURN.type==QtvPlayer::URN_LOCAL_BUFFER) ||
          (textURN.type==QtvPlayer::URN_LOCAL_FILE) || (textURN.type==QtvPlayer::URN_LOCAL_BUFFER)
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
          ||((videoURN.type==QtvPlayer::URN_PVX_FILE)&&clip.bDownload)
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#ifdef FEATURE_QTV_PSEUDO_STREAM
           || (videoURN.type==QtvPlayer::URN_PSEUDO_STREAM) || (audioURN.type==QtvPlayer::URN_PSEUDO_STREAM)
           || (textURN.type==QtvPlayer::URN_PSEUDO_STREAM)
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
           || (videoURN.type==QtvPlayer::URN_HTTP_STREAM) || (audioURN.type==QtvPlayer::URN_HTTP_STREAM)
           || (textURN.type==QtvPlayer::URN_HTTP_STREAM)
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_DRM_DCF
           || (videoURN.type == QtvPlayer::URN_INPUTSTREAM)
           || (audioURN.type == QtvPlayer::URN_INPUTSTREAM)
           || (textURN.type  == QtvPlayer::URN_INPUTSTREAM)
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
           || (videoURN.type== QtvPlayer::URN_QTV_HTTP_STREAM)
#endif
          )
    {
      /* duration should be larger of two files */
      int VideoDuration=0;
      int AudioDuration=0;
      int TextDuration=0;

      if (pVideo != NULL)
      {
        VideoDuration = pVideo->GetDurationMsec();
      }

      if (pAudio != NULL)
      {
        AudioDuration = pAudio->GetDurationMsec();
      }

      if (pText != NULL)
      {
        TextDuration = pText->GetDurationMsec();
      }

      /* take maximum duration */
      tempInfo.info.Duration = MAX(VideoDuration, AudioDuration);
      tempInfo.info.Duration = MAX(tempInfo.info.Duration, TextDuration);

      // put the movie duration in the paylaod
      qtv_event_clip_started_payload.duration = tempInfo.info.Duration;

      tempInfo.bValid=true;

      /* set correct file type based on two files */
      switch(eVideoFileType)
      {
        case Media::CONTENT_VIDEO:
        case Media::CONTENT_AUDIO_VIDEO:
        case Media::CONTENT_VIDEO_TEXT:
        case Media::CONTENT_AUDIO_VIDEO_TEXT:
          eVideoFileType = Media::CONTENT_VIDEO;
          break;

        case Media::CONTENT_VIDEO_STILL_IMAGE:
        case Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE:
        case Media::CONTENT_VIDEO_STILL_IMAGE_TEXT:
        case Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE_TEXT:
          eVideoFileType = Media::CONTENT_VIDEO_STILL_IMAGE;
          break;

        default:
          eVideoFileType = Media::CONTENT_UNKNOWN;
          break;
      }

      switch(eAudioFileType)
      {
        case Media::CONTENT_AUDIO:
        case Media::CONTENT_AUDIO_VIDEO:
        case Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE:
        case Media::CONTENT_AUDIO_TEXT:
        case Media::CONTENT_AUDIO_VIDEO_TEXT:
        case Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE_TEXT:
          eAudioFileType = Media::CONTENT_AUDIO;
          break;

        default:
          eAudioFileType = Media::CONTENT_UNKNOWN;
          break;
      }

      switch(eTextFileType)
      {
        case Media::CONTENT_TEXT:
        case Media::CONTENT_VIDEO_TEXT:
        case Media::CONTENT_AUDIO_TEXT:
        case Media::CONTENT_AUDIO_VIDEO_TEXT:
        case Media::CONTENT_VIDEO_STILL_IMAGE_TEXT:
        case Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE_TEXT:
          eTextFileType = Media::CONTENT_TEXT;
          break;

        default:
          eTextFileType = Media::CONTENT_UNKNOWN;
          break;
      }

      /* now combine audio/video/text component into one clip type */
      if(eVideoFileType == Media::CONTENT_VIDEO)
      {
        if(eAudioFileType == Media::CONTENT_AUDIO)
        {
          if(eTextFileType==Media::CONTENT_TEXT)
          {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT;
          }
          else
          {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_AUDIO_VIDEO;
          }
        }
        else
        {
          if(eTextFileType==Media::CONTENT_TEXT)
          {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_VIDEO_TEXT;
          }
          else
          {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_VIDEO_ONLY;
          }
        }
      }
      else if(eVideoFileType == Media::CONTENT_VIDEO_STILL_IMAGE)
      {
        if(eAudioFileType == Media::CONTENT_AUDIO)
        {
          if(eTextFileType==Media::CONTENT_TEXT)
          {
              tempInfo.info.MediaType = QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT;
          }
          else
          {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE;
          }
        }
        else
        {
          if(eTextFileType==Media::CONTENT_TEXT)
          {
              tempInfo.info.MediaType = QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT;
          }
          else
          {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_VIDEO_STILL_IMAGE;
          }
        }
      }
      else if(eAudioFileType==Media::CONTENT_AUDIO)
      {
        if(eTextFileType==Media::CONTENT_TEXT)
        {
              tempInfo.info.MediaType = QtvPlayer::MEDIA_AUDIO_TEXT;
        }
        else
        {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_AUDIO_ONLY;
        }
      }
      else if(eTextFileType==Media::CONTENT_TEXT)
      {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_TEXT;
      }
      else
      {
            tempInfo.info.MediaType = QtvPlayer::MEDIA_INVALID;
      }
    } /* end of if(local file || local buffer || inputstream) */
    else if ( clip.bStreaming )
    {
      // Get video clip dimensions from SDP file.
      if ( NULL != pMedia )
      {
         uint32 width = 0;
         uint32 height = 0;

         if (pMedia->GetVideoDimensions(&width, &height, 0) &&
             (width > 0) && (height > 0))
         {
            tempInfo.info.Width = width;
            tempInfo.info.Height = height;
            if( (width > MP4_MAX_DECODE_WIDTH) ||
                (height > MP4_MAX_DECODE_WIDTH) )
            {
              Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED);
              QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_RESOLUTION_NOT_SUPPORTED H=%ld, W=%ld", tempInfo.info.Height, tempInfo.info.Width);
            }
         }
         tempInfo.info.AudioSamplingFreq = ((StreamMedia *)pMedia)->GetAudioSampleRate();
         tempInfo.info.NumAudioChannels = pMedia->GetNumAudioChannels(0);
         tempInfo.info.AudioEncodedBitRate = pMedia->GetAudioBitRate(0);
         tempInfo.info.VideoEncodedBitRate = pMedia->GetVideoBitRate(0);
      }
    } /* end of else if ( clip.bStreaming ) */
  }

  //
  // Save clip info in the global data.
  //
  QCUtils::EnterCritSect(&clipInfo_CS);
  if (tempInfo.bValid)
  {
    clipInfo=tempInfo;
  }
  QCUtils::LeaveCritSect(&clipInfo_CS);

}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetPVPlayerMediaType

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
QtvPlayer::MediaTypeT Mpeg4Player::GetPVPlayerMediaType(Media::MediaFileType eMp4Filetype)
{
   QtvPlayer::MediaTypeT pvType;
   switch (eMp4Filetype)
   {
      case Media::CONTENT_AUDIO:
         pvType = QtvPlayer::MEDIA_AUDIO_ONLY;
         break;

      case Media::CONTENT_VIDEO:
         pvType = QtvPlayer::MEDIA_VIDEO_ONLY;
         break;

      case Media::CONTENT_AUDIO_VIDEO:
         pvType = QtvPlayer::MEDIA_AUDIO_VIDEO;
         break;

      case Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE:
         pvType = QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE;
         break;

      case Media::CONTENT_VIDEO_STILL_IMAGE:
         pvType = QtvPlayer::MEDIA_VIDEO_STILL_IMAGE;
         break;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      case Media::CONTENT_VIDEO_TEXT:
         pvType = QtvPlayer::MEDIA_VIDEO_TEXT;
         break;

      case Media::CONTENT_AUDIO_TEXT:
         pvType = QtvPlayer::MEDIA_AUDIO_TEXT;
         break;

      case Media::CONTENT_AUDIO_VIDEO_TEXT:
         pvType = QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT;
         break;

      case Media::CONTENT_VIDEO_STILL_IMAGE_TEXT:
         pvType = QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT;
         break;

      case Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE_TEXT:
         pvType = QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT;
         break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

      default:
         pvType = QtvPlayer::MEDIA_INVALID;
         break;
   }
   return pvType;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetClipComponents

DESCRIPTION
//
// Set clip component flags.  return true if no error.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::SetClipComponents(bool bAudioSelected,bool bVideoSelected, bool bTextSelected)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetClipComponents" );

  clip.bHasAudio=false;
  clip.bHasVideo=false;
  clip.bStillImage=false;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  clip.bHasText=false;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  if (clip.bM4vFile)
  {
    clip.bHasVideo=true;
  }
  else
  {
    //we need the media type previously set in the clip info.
    if (clipInfo.bValid)
    {
      switch (clipInfo.info.MediaType)
      {
        case QtvPlayer::MEDIA_AUDIO_ONLY:
          clip.bHasAudio=true;
          break;
        case QtvPlayer::MEDIA_VIDEO_ONLY:
          clip.bHasVideo=true;
          break;
        case QtvPlayer::MEDIA_AUDIO_VIDEO:
          clip.bHasAudio=true;
          clip.bHasVideo=true;
          break;
        case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE:
          clip.bHasVideo=true;
          clip.bStillImage=true;
          break;
        case QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE:
          clip.bHasAudio=true;
          clip.bHasVideo=true;
          clip.bStillImage=true;
          break;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        case QtvPlayer::MEDIA_VIDEO_TEXT:
          clip.bHasVideo=true;
          clip.bHasText=true;
          break;
        case QtvPlayer::MEDIA_AUDIO_TEXT:
          clip.bHasAudio=true;
          clip.bHasText=true;
          break;
        case QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT:
          clip.bHasAudio=true;
          clip.bHasVideo=true;
          clip.bHasText=true;
          break;
        case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT:
          clip.bHasVideo=true;
          clip.bStillImage=true;
          clip.bHasText=true;
          break;
        case QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT:
          clip.bHasAudio=true;
          clip.bHasVideo=true;
          clip.bStillImage=true;
          clip.bHasText=true;
          break;
        case QtvPlayer::MEDIA_TEXT:
          clip.bHasText=true;
          break;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
      }
      //
      //report errors if we couldn't select the components.
      //

      /* If an AUDIO TRACK is present in the clip but if it's CODEC is not supported
       * then we won't report QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED as
       * clip.bHasAudio will be marked as FALSE. Thus, connsult respective media file to
       * see if audio track is present in the clip. This is done to avoid reporting
       * QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED for video only or video+text clips.
       */
      if (!bAudioSelected &&
            (clip.bHasAudio ||
            ((clip.pAudioMpeg4Playback != NULL) &&
               clip.pAudioMpeg4Playback->IsAudioPresentInClip())))
      {
        //could not select any audio track.
        Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR_FORMAT_NOT_SUPPORTED);
        clip.bHasAudio=false;
        //see if we need to disallow video-only playback.
        if (clip.bHasVideo)
        {
          if (!clipInfo.info.VideoOnlyPlaybackAllowed)
          {
            Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ONLY_PLAYBACK_NOT_ALLOWED);
            clip.bHasVideo=false;
            clip.bHasEnhancedVideo = false;
          }
        }
      }

     if (!bVideoSelected &&
         (clip.bHasVideo ||
         ((clip.pVideoMpeg4Playback != NULL) &&
          clip.pVideoMpeg4Playback->IsVideoPresentInClip())))
      {
        //could not select any video track.
        Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR_FORMAT_NOT_SUPPORTED);
        clip.bHasVideo=false;
        clip.bHasEnhancedVideo = false;
        //see if we need to disallow audio-only playback.
        if (clip.bHasAudio)
        {
          if (!clipInfo.info.AudioOnlyPlaybackAllowed)
          {
            Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ONLY_PLAYBACK_NOT_ALLOWED);
            clip.bHasAudio=false;
          }
        }
      }
      if (clip.bHasText && !bTextSelected)
      {
        //could not select any text track.
        //Notify(QtvPlayer::QTV_PLAYER_STATUS_TEXT_ERROR_FORMAT_NOT_SUPPORTED);
        clip.bHasText=false;
      }
    }
  }

  //Start or restart streaming.
#ifdef FEATURE_QTV_STREAM_RECORD
  if (clip.bStreaming && clip.pVideoMpeg4Playback && playerState < PB_READY )
  {
    #ifdef FEATURE_QTV_TEXT_RECORD
    clip.pVideoMpeg4Playback->setRecordingStreamNo(clip.bHasAudio, clip.bHasVideo,
                                                   clip.bHasEnhancedVideo, clip.bHasText);
    #else
    clip.pVideoMpeg4Playback->setRecordingStreamNo(clip.bHasAudio, clip.bHasVideo,
                                                   clip.bHasEnhancedVideo);
    #endif
  }
#endif


  /* Store the genuine "has audio" state then override it if audio
  ** is concurrent.
  */
  clip.bHasAudioWhenNotSuspended = clip.bHasAudio;

#ifdef FEATURE_QTV_GENERIC_BCAST
  // Check the mute configuration and clear the flags if needed
  if(clip.bHasAudio && IsAudioMuted())
  {
    clip.bHasAudio = false;
  }
  if(clip.bHasVideo && IsVideoMuted())
  {
    clip.bHasVideo = false;
  }
  if(clip.bHasText && IsTextMuted())
  {
    clip.bHasText = false;
  }
#endif

  if(clip.bHasVideo)
  {
#ifndef FEATURE_WINCE
    if (NULL == pActiveVideoPlayer)
    {
      pActiveVideoPlayer = (PlayerBaseIF*)PlayerBaseIF::CreateVideoPlayerInstance();
      if (pActiveVideoPlayer)
      {
        if (!pActiveVideoPlayer->Create((void *)this, OEMMallocBuffer, OEMFreeBuffer, &m_mediaSync,&qtvConfigObject))
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error in Mpeg4Player::SetClipComponents");
          return false;
        }
      }
      else
      {
        PlayerBaseIF::DestroyVideoPlayerInstance(pActiveVideoPlayer);
        pActiveVideoPlayer = NULL;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video Player Creation Failed");
      }
    }
#endif

  }

  //fail now if clip has no playable components
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  bool bOK = clip.bHasAudio || clip.bHasVideo || clip.bHasText;
#else
  bool bOK = clip.bHasAudio || clip.bHasVideo;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  if (!bOK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::SetClipComponents: no playable components");
  }

  return bOK;
}
#ifdef FEATURE_QTV_AVI
/* ======================================================================
FUNCTION
  Mpeg4Player::QTV_EXCESSIVE_FRAME_DROPS_handler

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

void
Mpeg4Player::QTV_EXCESSIVE_FRAME_DROPS_handler
(
  QCMessageHandleType handle_in
)
{
  Media* pMedia = NULL;

   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::QTV_EXCESSIVE_FRAME_DROPS_handler");

   if(handle_in == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::QTV_EXCESSIVE_FRAME_DROPS_handler: Input param handle is NULL");
      QTV_USE_ARG1(pMedia);
      return;
   }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle_in)->user_data;

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::ExcessiveFrameDropsHandler in State %d",
                              pThis->playerState );

  if(pThis->playerState == PLAYING)
  {
    if (pThis->clip.bHasVideo && pThis->pActiveVideoPlayer)
    {
       if(pThis->pActiveVideoPlayer->notifyExcessFrameDrop() != true)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                       "Mpeg4Player::notifyExcessFrameDrop failed" );
       }
    }
  }
QTV_USE_ARG1(pMedia);
}

#endif
/* ======================================================================
FUNCTION
  Mpeg4Player::DescribeAndSelectTracks

DESCRIPTION
//
// Do track selection.  Return TRUE if ok.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::DescribeAndSelectTracks(Media *p,
                                          bool &bAudioSelected,
                                          bool &bVideoSelected,
                                          bool &bTextSelected)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
    "Mpeg4Player::DescribeAndSelectTracks" );

  bAudioSelected = false;
  bVideoSelected = false;
  bTextSelected = false;

  if (clip.bM4vFile)
  {
    return true;
    }

  QTV_NULL_PTR_CHECK(p, false);

  //Get track information.
  ReferenceCountedPointer<ITrackList> trackList;

  if (!p->GetSelectedTrackList(MakeOutPointer(trackList)) ||
      (trackList == NULL))
      {
    return false;
      }

  bool bError = p->CheckError(Common::DESCRIBE);

  QTV_MSG1(QTVDIAG_GENERAL, "Mpeg4Player::Describe tracklist != NULL, bError = %d", bError);

  if (bError)
  {
    return false;
  }

  HandleTrackListChange(MakeInPointer(trackList), p,
                        bAudioSelected, bVideoSelected, bTextSelected);

  NotifyTrackListChange(MakeInPointer(trackList));

  /* Succeed even if no media is selected, so that in a playlist,
   * the user can go to the next clip.
   */
  if (!bAudioSelected && !bVideoSelected && !bTextSelected)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
      "Mpeg4Player::DescribeAndSelectTracks: No Track Error");
  }

  return true;
}

void Mpeg4Player::HandleTrackListChange(ITrackList *trackList /* in */,
                                        Media *media,
                                        bool &bAudioSelected,
                                        bool &bVideoSelected,
                                        bool &bTextSelected)
{
  QTV_NULL_PTR_CHECK(trackList, RETURN_VOID);

  int numTracks = (int) trackList->GetNumTracksAvailable();

  for (int i = 0; i < numTracks; i++)
  {
    if (!trackList->IsTrackSelected(i))
    {
      continue;
    }

    Media::CodecType codecType = trackList->GetCodecType(i);

    if (Media::IsAudioCodec(codecType))
    {
      if (selectedAudioTrackIdx < 0)
  {
        selectedAudioTrackIdx = i;
  }
    } // if (isAudioCodec)

    if (Media::IsVideoCodec(codecType))
  {
      if (selectedVideoTrackIdx < 0)
  {
        selectedVideoTrackIdx = i;
  }
    } // if (isVideoCodec)

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    /* Incase of text only tracks we must set
       selectedTextTrackIdx to valid track id
       so that player will allow it to play */

    if ( codecType == Media::TIMED_TEXT_CODEC )
  {
       if( selectedTextTrackIdx < 0 )
    {
         selectedTextTrackIdx = i;
       }
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    if ((codecType == Media::MPEG4_CODEC) &&
        (trackList->GetDependsOnID(i) != -1))
      {
      clip.bHasEnhancedVideo = true;
        }
  } // for (int i = 0; i < numTracks; i++)

  bAudioSelected = (selectedAudioTrackIdx >= 0);
  bVideoSelected = (selectedVideoTrackIdx >= 0);
  bTextSelected  = (selectedTextTrackIdx >= 0);

#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
  /* special case for telop text which is not a track */
  if ( !bTextSelected && media->IsTelopPresent() )
        {
    bTextSelected = true;
        }
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

  if (selectedAudioTrackIdx >= 0)
      {
    // get audio codec for the qtv_event_clip_started_type payload
    qtv_event_clip_started_payload.audio_codec =
      (uint8) trackList->GetCodecType(selectedAudioTrackIdx);
  }

  if (selectedVideoTrackIdx >= 0)
        {
    qtv_event_clip_started_payload.video_codec =
      (uint8) trackList->GetCodecType(selectedVideoTrackIdx);
  }
        }

void Mpeg4Player::NotifyTrackListChange(ITrackList *trackList /* in */)
        {
  QTV_NULL_PTR_CHECK(trackList, RETURN_VOID);

  int32 numTracks = trackList->GetNumTracksAvailable();

  for (int32 i = 0; i < numTracks; i++)
      {
    if (bNotifyUnknownCodec && !trackList->IsTrackSelected(i) &&
        !trackList->IsPlayable(i) &&
        (trackList->GetCodecType(i) != Media::AAC_CODEC)
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
       )
         {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_UNSUPPORTED_CODEC);
         }
  } // for all tracks

  if (clip.bHasEnhancedVideo)
      {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
      "Clip has enhanced video layer.");
    Notify(QtvPlayer::QTV_PLAYER_TS_LAYER_PRESENT);
      }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::InitURN

DESCRIPTION
//
// init data for a new URN.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::InitURN()
{
   DestroyMedia();
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::InitURN" );

   // URN
   QCUtils::EnterCritSect(&URN_CS);

   (void)videoURN.Init();
   (void)audioURN.Init();
   (void)textURN.Init();

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
   URN.Init();
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

   QCUtils::LeaveCritSect(&URN_CS);

   pvxIF.bValid=false;
   sdpIF.bValid=false;
   // A new URN means a new URL.
   InitURL();
}

/* ======================================================================
FUNCTION
  Mpeg4Player::InitURL

DESCRIPTION
//
// init data for a new URL.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::InitURL()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::InitURL" );

   QCUtils::EnterCritSect(&URL_CS);
   (void)url.Init();
   QCUtils::LeaveCritSect(&URL_CS);

   // A new URL means a new clip.
   InitClip();
}

/* ======================================================================
FUNCTION
  Mpeg4Player::InitClip

DESCRIPTION
//
// init data for a new clip.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::InitClip()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::InitClip" );

   QCUtils::EnterCritSect(&clipInfo_CS);
   clipInfo.bValid=false;
   QCUtils::LeaveCritSect(&clipInfo_CS);

   (void)clip.Init();

#ifdef FEATURE_QTV_DRM_DCF
   clip.videoURN              = &videoURN;
   clip.audioURN              = &audioURN;
   clip.textURN               = &textURN;
   clip.bValid                = false;
#endif

   appControl.bStartPlaying      = false;
   appControl.bPureLocalFile     = false;
   appControl.bResumePending     = false;

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   appControl.mediaCounter       = 0;
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

   appControl.bHasMediaStarted = false;
   appControl.bPauseMediaAfterStartPlaying = false;
   appControl.nStreamStartsToIgnore = 0;

   appControl.nVideoAbortsToIgnore = 0;
   appControl.nAudioAbortsToIgnore = 0;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   appControl.nTextAbortsToIgnore = 0;
#endif /*  FEATURE_MP4_3GPP_TIMED_TEXT */

   //default play times.  these will be used when repositioning is disabled
   //or when clip plays for some reason other than a user command.
   reposControl.nStart=0;
   reposControl.nStop = -1;
   reposControl.nRestart = -1;
   reposControl.bTimesValid = true;
   reposControl.reposType = 0;

}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetURN

DESCRIPTION
//
// set urn from urn name.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::SetURN(const URL &fullURN, URNData &destURN)
{
  bool bUrnFile=false;
  char * strippedURN = NULL;
  bool bSDPFileDownload = false;
  bool bPVXFileDownload = false;

  if (fullURN == NULL) //check if invalid data is passed
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetURN fullURN is NULL");
    return;
  }
  if (fullURN.IsEmpty() || (fullURN.GetUrlLength() > URL::MaxUrlSize))
  {
    return;
  }
  //QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetURN %s", fullURN);

  strippedURN = (char *) QTV_Malloc(sizeof(char) * (URL::MaxUrlSize+1));
  if (NULL == strippedURN)
  {
    return;
  }
  (void)std_strlcpy( strippedURN, (const char *)fullURN, URL::MaxUrlSize+1 );

  URNData urn;
  if (!urn.Init())
  {
  	QTV_Free(strippedURN);// Gemsea Fix MEM
    return;
  }
  urn.bValid=false;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  urn.bHttpURN=false;
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  urn.bHttpPDURN = false;
#endif
  urn.pBufURN = NULL;
  urn.bufSize = 0;

  //save the urn.
  *urn.sURN = fullURN;
  if (URL::URL_OK != urn.sURN->GetErrorCode())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::SetURN failed while copying urls");
    return;
  }

  //QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Input URN ::%s:",fullURN);

  // A .URN file is a kludge to get a URN from a file so it doesn't
  // have to come from the media player. The URN read from the file
  // will replace the input URN.
  if ( urn.sURN->EndsWith(".urn") || urn.sURN->EndsWith(".url"))
  {
    bUrnFile=true;

    //read the urn from the file.
    OSCL_FILE * fp = OSCL_FileOpen((const char *)(*urn.sURN), "r");
    char c;
    int i;

    //Initialize the name.
    memset(strippedURN, 0x00, URL::MaxUrlSize+1);

    //skip leading blanks
    for (i=0; OSCL_FileRead(&c,1,1,fp)==1;)
    {
      if (c>' ')
      {
        strippedURN[i++] = c; break;
      }
    }
    //get chars
#ifndef FEATURE_WINCE
    while (OSCL_FileRead(&c,1,1,fp)==1 && i <= URL::MaxUrlSize)
    {
      strippedURN[i++] = c;
    }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
    (void)OSCL_FileClose(fp);

    if (strippedURN[URL::MaxUrlSize] != '\0' )
    {
      /* The URN Length exceeds urlsize. Skipping further parsing;
      this should never happen*/
      QTV_Free(strippedURN);// Gemsea Fix MEM
      return;
    }

    //remove trailing blanks
    i--;
    while (0<=i && strippedURN[i]<=' ') strippedURN[i--] = '\0';
    //terminate
    i=ZMIN(i+1,URL::MaxUrlSize);
    strippedURN[i]='\0';

    // dprintf uses buffer size of 256 bytes. The following dprintf
    // will corrupt the stack if the length of urn.sURN exceeds 231 bytes
    //QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "URL name: ::%s::", urn.sURN);
  }

  //set url
  urn.sURN->SetUrl(strippedURN);
  if (URL::URL_OK != urn.sURN->GetErrorCode())
  {
  	QTV_Free(strippedURN);// Gemsea Fix MEM
    return;
  }

  // Strip the query portion of the URN if it exists
  // This is the application session id passed to the URL by the
  // web module for SDP URNs
  char *pQuery = (char *)zrex_strstri(strippedURN, "?");
  if (pQuery != NULL)
  {
    strippedURN[pQuery - strippedURN] = '\0';

    if ( ZUtils::FindR(strippedURN,".sdp") != ZUtils::npos )
    {
      bSDPFileDownload = true;
    }

    if ( ZUtils::FindR(strippedURN,".pvx") != ZUtils::npos )
    {
      bPVXFileDownload = true;
    }
  }

  //Let sdp interface know the config object
  sdpIF.SetConfigObject(&qtvConfigObject);

  //
  // Get the URN type.
  //

  if (urn.sURN->StartsWith("rtsp://") || urn.sURN->StartsWith("rtspt://"))
  {
    // Workaround for HTML being inserted into URL by faulty browsers.
    if ((ZUtils::Find((const char *)(*urn.sURN), "<") != ZUtils::npos) ||
        (ZUtils::Find((const char *)(*urn.sURN), ">") != ZUtils::npos))
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                    QTVDIAG_PRIO_ERROR,
                    "Mpeg4Player::SetURN: found bad URL: %s",
                    (const char *)(*urn.sURN));
    }
    else
    {
      //RTSP stream.
      urn.type = QtvPlayer::URN_RTSP;
      urn.bValid = true;
    }
  }

  else if (urn.sURN->EndsWith(".flo"))
  {
    /* Forget the URN.  Its name merely indicates that we should use FLO.
       Parms came from the BREW layer in the form of SetMediaInfo */
    urn.type = QtvPlayer::URN_BCAST_FLO;
    urn.bValid = true;
  }
#ifdef FEATURE_QTV_MAPI_MEDIA_MASK
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_REPLAY
#error code not present
#endif
// Generic Broadcast object is a composite object.
// All broadcast types such as FLO, ISDB and TDMB belong to
// generic broadcast class. Hence it doesnt have unique URN on
// on its own. eGenericBcastMediaID is used to differentiate across
// various broadcast objects supported.
#ifdef FEATURE_QTV_GENERIC_BCAST_ISDB
  else if (urn.sURN->StartsWith("isdb://"))
  {
    // Get rid of isdb://
    // If there's nothing afterward, the URL will be NULL
    urn.sURN->SkipCharsAtStart(strlen("isdb://"));
    urn.type = QtvPlayer::URN_BCAST_ISDB;
    urn.bValid = true;
  }
#endif

#ifdef FEATURE_QTV_GENERIC_BCAST_CMMB
#error code not present
#endif

#ifdef FEATURE_QTV_GENERIC_BCAST_TDMB
#error code not present
#endif /* FEATURE_QTV_BCAST_TDMB */

  else if (urn.sURN->StartsWith("sdp://"))
  {
    urn.type = QtvPlayer::URN_SDP_BUFFER;
    urn.bValid=true;

    sdpIF.bValid=false;

    sdpIF.pSDPInfo = QTV_New(SDPInfo);
    if(sdpIF.pSDPInfo != NULL)
    {
      //freeing the memory will be taken care by SessionInfo
      sdpIF.pSDPInfo->isSDPInfoLocallyAllocated = true;
    }

    // Get rid of sdp://
    urn.sURN->SkipCharsAtStart(strlen("sdp://"));

    // Parse SDP buffer and get sdp info;
    if (sdpIF.sdpParser.parseSDPandHandleErrors((const char *)(*urn.sURN),
                                                urn.sURN->GetUrlLength(),
                                                sdpIF.pSDPInfo) != SDP_SUCCESS)
    {
      QTV_MSG( MSG_LEGACY_ERROR,
               "Mpeg4Player::SetURN, SDP Buffer parse failed");
    }
    else
    {
      if(sdpIF.pSDPInfo != NULL)
      {
        // Get Control URL
        sdpIF.url = sdpIF.pSDPInfo->getSessionDescription()->getControlURL();
      }
      sdpIF.URLType = QtvPlayer::URL_STREAM;
      sdpIF.pKey = NULL;
      sdpIF.bValid = true;
      //get playlist length and select first clip.
      sdpIF.nPlaylistLength = 1; // pvxIF.appFile.GetTotalClips();
      sdpIF.bPlaylistComplete = false;
      //init to (-1) instead of zero since we inc the index on each url.
      sdpIF.nPlaylistIndex=(-1);
    }
  }
  else if ((urn.sURN->EndsWith(".sdp")) || (bSDPFileDownload))
  {
    //SDP filename.
    urn.type = QtvPlayer::URN_SDP_FILE;
    urn.bValid=true;

    //parse the pvx file now.
    sdpIF.bValid=false;
    if (sdpIF.sdpFile.ParseApplicationFile(*urn.sURN) == PV_NO_ERROR)
    {
      sdpIF.url = sdpIF.sdpFile.GetSDPURL();
      sdpIF.URLType = sdpIF.sdpFile.GetSDPURLType();

      sdpIF.pKey = sdpIF.sdpFile.GetSDPKey();
      sdpIF.pSDPInfo = sdpIF.sdpFile.GetSDPInfo();
      sdpIF.bValid=true;
      //get playlist length and select first clip.
      sdpIF.nPlaylistLength = 1; // pvxIF.appFile.GetTotalClips();
      sdpIF.bPlaylistComplete = false;
      //init to (-1) instead of zero since we inc the index on each url.
      sdpIF.nPlaylistIndex=(-1);
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      if (NULL != URN.sURN)
      {
        *URN.sURN = sdpIF.url;
      }
      if ((sdpIF.pSDPInfo != NULL) && (sdpIF.pSDPInfo->getSessionDescription() != NULL))
      {
         http_bContentPurge = sdpIF.pSDPInfo->getSessionDescription()->getPurgeStatus();
         http_ContentSizeFromSDP = sdpIF.pSDPInfo->getSessionDescription()->getFileSize();
      }
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
    }
    else
    {
      //in this case OpenURN will eventually fail.
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SetURN, SDP parse failed");
    }
#ifndef FEATURE_WINCE
    event_report(EVENT_QTV_SDP_SELECTED);
#endif   /*    FEATURE_WINCE   */
  }
  else if (  urn.sURN->EndsWith(".pvx") || (bPVXFileDownload) )
  {

#ifdef FEATURE_PDS2_NV_CONFIG

    // Always allow LTK to stream Progressive download 2
    // content for testing.
#if !defined (PLATFORM_LTK) && !defined (FEATURE_WINCE)
    nv_item_type  nv_item;
    /* Read FastTrack enable item from NV */
    if (!ZUtils::readNVItem( NV_ENABLE_FASTTRACK_I , &nv_item) ||
        (nv_item.enable_fasttrack == 0))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PDS2 enable flag from NV is not set");
      QTV_Free(strippedURN);
      return;
    }
#endif   /* IF-ENDIF   <=>   #if !defined (PLATFORM_LTK) && !defined (FEATURE_WINCE) */
#endif /* End of FEATURE_PDS2_NV_CONFIG */

    //PVX filename.
    urn.type = QtvPlayer::URN_PVX_FILE;
    urn.bValid=true;

    //parse the pvx file now.
    pvxIF.bValid=false;
    if (pvxIF.appFile.ParseApplicationFile(urn.sURN->GetUrlBuffer())==PV_NO_ERROR)
    {
      pvxIF.bValid=true;
      //get playlist length and select first clip.
      pvxIF.nPlaylistLength=pvxIF.appFile.GetTotalClips();
      pvxIF.bPlaylistComplete=false;
      //init to (-1) instead of zero since we inc the index on each url.
      pvxIF.nPlaylistIndex=(-1);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      // Check if a .txt file with the same name as .pvx file exist
      // if yes then the download is broken
      char DownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
      if (NULL != URN.sURN)
      {
        *URN.sURN = *urn.sURN;
      }
      getDownloadTxtFileName(DownloadTxtFile, sizeof(DownloadTxtFile));
      if (OSCL_FileExists (DownloadTxtFile))
      {
        appControl.bResumeDownload = true;
      }
      else
      {
        appControl.bResumeDownload = false;
      }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

    } else
    {
      //in this case OpenURN will eventually fail.
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SetURN, PVX parse failed");
    }
  }
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  else if (urn.sURN->StartsWith("http://"))
  {
    urn.bHttpPDURN = true;
    urn.type = QtvPlayer::URN_QTV_HTTP_STREAM;
    urn.bValid=true;
    if (NULL != URN.sURN)
    {
       *URN.sURN = *urn.sURN;
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                 "SetURN: URN typ: URN__QTV_HTTP_STREAM");
  }
#endif /* FEATURE_QTV_HTTP_DOWNLOAD */
  else if (!urn.sURN->IsEmpty())      //check for blank URL
  {
    //assume it's a local file. .mp4 and .3gp extensions
    // Strip off any file:// specifier
    // Strip "file://" from URN if necessary
    if (urn.sURN->StartsWith("file://"))
    {
      // Save the original URN and not the
      // uppercase version
      urn.sURN->SkipCharsAtStart(strlen("FILE://"));
    }

    urn.type = QtvPlayer::URN_LOCAL_FILE;
    urn.bValid = true;
  }

  //Copy new urn into global data.
  QCUtils::EnterCritSect(&URN_CS);
  if (urn.bValid)
  {
    destURN = urn;
  }
  QCUtils::LeaveCritSect(&URN_CS);
  QTV_Free(strippedURN);
  bUrnFile = bUrnFile; /* To fix lint */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetURN

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
void Mpeg4Player::SetURN(unsigned char *pBufURN, uint32 bufSize, URNData &destURN)
{
   if (!pBufURN)
      return;

   URNData urn;
   if (!urn.Init())
   {
     return;
   }
   urn.pBufURN = pBufURN;
   urn.bufSize = bufSize;
   urn.type = QtvPlayer::URN_LOCAL_BUFFER;
   urn.bValid = true;

   //Copy new urn into global data.
   QCUtils::EnterCritSect(&URN_CS);
   if (urn.bValid)
   {
      destURN=urn;
   }
   QCUtils::LeaveCritSect(&URN_CS);

   // Get clip source and file type for this url.
  (void)SetClipType();

}

#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::SetPseudoStream

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

void Mpeg4Player::setPseudoStream(unsigned char *pBufURN,
                  uint32 bufSize,
                  QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize,
                  QtvPlayer::FetchBufferedDataT FetchBufferedData,
                  QtvPlayer::InstanceHandleT m_QtvInstanceHandle,
                  URNData &destURN)
{

   URNData urn;
   if (!urn.Init())
   {
     return;
   }
   urn.pBufURN = pBufURN;
   urn.bufSize = bufSize;
   urn.type = QtvPlayer::URN_PSEUDO_STREAM;
   urn.bValid = true;

   //for pull interface FetchBufferedDataSize & FetchBufferedData are
   //valid [not null]
   urn.FetchBufferedDataSize = FetchBufferedDataSize;
   urn.FetchBufferedData = FetchBufferedData;
   urn.m_QtvInstanceHandle = m_QtvInstanceHandle;
   //Copy new urn into global data.
   QCUtils::EnterCritSect(&URN_CS);
   if (urn.bValid)
   {
      destURN=urn;
   }
   QCUtils::LeaveCritSect(&URN_CS);

   // Get clip source and file type for this url.
   (void)SetClipType();

}
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* ======================================================================
FUNCTION
  Mpeg4Player::SetHTTPStream

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

void Mpeg4Player::setHTTPStream(unsigned char *pBufURN,
                  uint32 bufSize,
                  QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize,
                  QtvPlayer::FetchBufferedDataT FetchBufferedData,
                  QtvPlayer::InstanceHandleT m_QtvInstanceHandle,
                  URNData &destURN)
{
   URNData urn;
   if (!urn.Init())
   {
     return;
   }

   //for push interface pBufURN and bufSize are valid [not null/0]
   urn.pBufURN = pBufURN;
   urn.bufSize = bufSize;
   urn.type = QtvPlayer::URN_HTTP_STREAM;
   urn.bValid = true;

   //for pull interface FetchBufferedDataSize & FetchBufferedData are
   //valid [not null]
   urn.FetchBufferedDataSize = FetchBufferedDataSize;
   urn.FetchBufferedData = FetchBufferedData;
   urn.m_QtvInstanceHandle = m_QtvInstanceHandle;

   //Copy new urn into global data.
   QCUtils::EnterCritSect(&URN_CS);
   if (urn.bValid)
   {
      destURN=urn;
   }
   QCUtils::LeaveCritSect(&URN_CS);

   // Get clip source and file type for this url.
  (void)SetClipType();

}
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

/* ======================================================================
FUNCTION
  Mpeg4Player::SetURL

DESCRIPTION
//
// set url from urn.  return true if the url was updated.
// for pvx, calling this repeatedely will get urls from the
// playlist.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::SetURL()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::SetURL" );

  bool bUpdate=false;

  URLData url;
  if (!url.Init())
  {
    return bUpdate;
  }
  url.bValid=false;
  url.bLocalFileFromPVX=false;

  if (pvxIF.bValid)
  {
    //Get the next URL from the playlist.
    if (!pvxIF.bPlaylistComplete)
    {
      //advance playlist index.
      pvxIF.nPlaylistIndex++;

      if (pvxIF.nPlaylistIndex<pvxIF.nPlaylistLength)
      {
        //Select clip from playlist
        pvxIF.appFile.SetCurrentClip(pvxIF.nPlaylistIndex);

        //Clear old URL data.
        InitURL();

        //Set URL info from the PVX
        switch (pvxIF.appFile.GetClipSource(pvxIF.nPlaylistIndex))
        {
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
          case PV_DOWNLOAD:
            url.bValid=true;
            url.type = QtvPlayer::URL_DOWNLOAD;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: SetURL : url type:PV_DOWNLOAD:URL_DOWNLOAD");
            break;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

          case PV_REMOTE:
            url.type = QtvPlayer::URL_STREAM;
            url.bValid=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: SetURL : url type:PV_REMOTE:URL_STREAM");
            break;

          case PV_LOCAL:
            url.type = QtvPlayer::URL_STREAM;
            url.bLocalFileFromPVX=true;
            url.bValid=true;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                         "PDS2: SetURL : url type:PV_LOCAL:URL_STREAM");
            break;

          default:
            break;
        }
        if (url.bValid && (NULL != url.sURL))
        {
          *url.sURL = pvxIF.appFile.GetClipName(pvxIF.nPlaylistIndex);
          if (URL::URL_OK != url.sURL->GetErrorCode())
          {
            url.bValid = false;
          }
        }
      }
      else
      {
        //reached the end of the playlist.
        pvxIF.bPlaylistComplete=true;
      }
    } /*if (!pvxIF.bPlaylistComplete)*/
    //error check.
    if (!url.bValid)
    {
      if (!pvxIF.bPlaylistComplete)
      {
        //we're not at the end of the playlist, yet we couldn't
        //get another URL.
        //set the playlist complete to avoid repeated errors.
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SetURL playlist error" );
        pvxIF.bPlaylistComplete=true;
      }
    }
    //only update the URL if we got an entry from the playlist.
    //that way we won't clobber the last entry when reaching
    //the end of the list.
    bUpdate=url.bValid;
  }
  else if (sdpIF.bValid)
  {
    //Clear old URL data.
    InitURL();
    url.type = sdpIF.URLType;
    url.bValid = true;
    if (url.bValid)
    {
      /*
      sessionDescription * pSD = sdpIF.pSDPInfo->getSessionDescription();
      if ( pSD )
      {
        *url.sURL = (char *)pSD->getSessionName();
      }
      else
      */
      {
        // This is the input filename as opposed to the URL
        *url.sURL = sdpIF.url;
        if (URL::URL_OK != url.sURL->GetErrorCode())
        {
          url.bValid = false;
        }
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SetURL Play Error" );
      sdpIF.bPlaylistComplete=true;
    }
    bUpdate = url.bValid;
  }
  else
  {
    //not a pvx file, so get the URL from the URN.
    /* since we support only one URL, take that from video URN */
    if (videoURN.bValid || audioURN.bValid || textURN.bValid)
    {
      //Always overwrite the previous URL.
      bUpdate=true;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_HTTP_DOWNLOAD)
      if (videoURN.type==QtvPlayer::URN_QTV_HTTP_STREAM ||
          videoURN.type==QtvPlayer::URN_PVX_FILE)
      {
        //Download URN (this is for test support only, normally the download
        //URL comes from the PVX file).
        url.type = QtvPlayer::URL_DOWNLOAD;
        url.bValid = true;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                     "PDS2: URL type set to URL_DOWNLOAD");

      }
      else
#endif /*  FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
      if (videoURN.type==QtvPlayer::URN_RTSP)
      {
        //Stream URN.
        url.type = QtvPlayer::URL_STREAM;
        url.bValid = true;
      }
      if (url.bValid)
      {
        /* since we support only one URL, take that from video URN */
        *url.sURL = *videoURN.sURN;
        if (URL::URL_OK != url.sURL->GetErrorCode())
        {
          url.bValid = false;
        }
      }
    } /*if (videoURN.bValid || audioURN.bValid || textURN.bValid)*/
  }

  if (bUpdate)
  {
    //update the global url data.
    QCUtils::EnterCritSect(&URL_CS);
    if (url.bValid)
    {
      this->url = url;
    }
    QCUtils::LeaveCritSect(&URL_CS);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    SetAppData();
#endif /*  FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
    // Get clip source and file type for this url.
    (void)SetClipType();
  }

  return bUpdate;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void Mpeg4Player::SetAppData()
//
// set app data assuming we previously parsed the app file.
//
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetAppData\n");

  if (pvxIF.bValid)
  {
    AppData app;
    app.bValid=true;

    // Get download playback action.
    switch (pvxIF.appFile.GetDownloadPlayback(pvxIF.nPlaylistIndex))
    {
    case NOT_AUTO_START:
      app.data.DownloadPlaybackControl = QtvPlayer::NO_PLAYBACK;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "PDS2: DL PBack action: NO_PLAYBACK");
      break;
    case AUTO_START:
      app.data.DownloadPlaybackControl = QtvPlayer::PLAYBACK_AFTER_DOWNLOAD;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "PDS2: DL PBack action: PLAYBACK_AFTER_DOWNLOAD");
      break;
    case START_ASAP:
      app.data.DownloadPlaybackControl = QtvPlayer::PLAYBACK_ASAP;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "PDS2: DL PBack action: PLAYBACK_ASAP");
      break;
    default:
      app.data.DownloadPlaybackControl = QtvPlayer::NO_PLAYBACK;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "PDS2: DL PBack action: NO_PLAYBACK");
      break;
    }

#ifdef QTV_FEATURE_SET_PVX_ATTRIBUTE
    //workaround for PVX Playclip issue
    app.data.DownloadPlaybackControl = QtvPlayer::NO_PLAYBACK;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PDS2: DL PBack action changed to: NO_PLAYBACK");
#endif

    // Get next URL and end of clip action.
    char *pNext = pvxIF.appFile.LaunchURLAfterPlayback();
    if ((pNext != NULL) && (NULL != app.data.NextURL))
    {
      *app.data.NextURL = pNext;
      app.data.EndOfClipAction=END_OF_CLIP_PLAY_NEXT_URL;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                    "PDS2: End of clip PBack action: END_OF_CLIP_PLAY_NEXT_URL");

    }
    else if(pvxIF.appFile.ExitAfterPlayback())
    {
      app.data.EndOfClipAction=END_OF_CLIP_CLOSE_APP;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                    "PDS2: End of clip PBack action: END_OF_CLIP_CLOSE_APP");
    }
    else
    {
      app.data.EndOfClipAction = NO_END_OF_CLIP_ACTION;
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                   "PDS2: End of clip PBack action: NO_END_OF_CLIP_ACTION");

    }

    //Repositioning allowed?
    app.data.RepositioningAllowed = pvxIF.appFile.AllowRepositioning(pvxIF.nPlaylistIndex);
        if(app.data.RepositioningAllowed)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," PDS2: Repositioning allowed");
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," PDS2: Repositioning is NOT allowed");
        }

    //Secured?
    app.data.Secured = pvxIF.appFile.IsSecured();
        if(app.data.Secured )
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," PDS2: Secured");
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," PDS2: DLoad is NOT secured");
        }


       //User playback during download allowed?
       app.data.UserPlaybackAllowed = !pvxIF.appFile.DisableUserPlayback(pvxIF.nPlaylistIndex);

        if(app.data.UserPlaybackAllowed)
        {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," PDS2: User playback is allowed");
        }
        else
        {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED," PDS2: User playback is NOT allowed");

           if( (app.data.DownloadPlaybackControl == QtvPlayer::NO_PLAYBACK) ||
               (app.data.DownloadPlaybackControl == QtvPlayer::PLAYBACK_AFTER_DOWNLOAD) )
           {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," PDS2: App Data conflict,changing to PLAYBACK_ASAP");
              app.data.DownloadPlaybackControl = QtvPlayer::PLAYBACK_ASAP;
           }

        }


    //Set the global app data.
//    QCUtils::EnterCritSect(&appData_CS);
    if (app.bValid)
    {
      appData=app;
    }
//    QCUtils::LeaveCritSect(&appData_CS);

    //set other fields that are only used internally.
    appData.flags=pvxIF.appFile.GetFlags(pvxIF.nPlaylistIndex);
    unsigned long start,stop;
    (void)pvxIF.appFile.GetClipTimes(&start,&stop,pvxIF.nPlaylistIndex);
    appData.startTime=(long)start;
    appData.stopTime=(long)stop;
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "PDS2: clip start time %d, stop time %d",
                appData.startTime,appData.stopTime);
  }
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2  */



/* ======================================================================
FUNCTION
  Mpeg4Player::SetClipType

DESCRIPTION
//
// set clip type fields.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::SetClipType()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetClipType" );

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
   if (URN.bHttpURN)
   {
     clip.bDownload = true;
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "PDS2: Clip download is set to TRUE" );
   }
   else
#endif

   if (videoURN.bValid)
   {
     switch (videoURN.type)
     {
     case QtvPlayer::URN_LOCAL_FILE:
       clip.pVideoLocalFilename = videoURN.sURN->GetUrlBuffer();
#ifdef FEATURE_QTV_M4V
#error code not present
#endif
       break;

     case QtvPlayer::URN_LOCAL_BUFFER:
       clip.pVideoLocalBuffer = videoURN.pBufURN;
       clip.videoBufSize = videoURN.bufSize;
       break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
     case QtvPlayer::URN_PSEUDO_STREAM:
       clip.pVideoLocalBuffer = videoURN.pBufURN;

       clip.videoBufSize = videoURN.bufSize;
       clip.bPseudoStreaming = true;
       clip.FetchBufferedDataSize = videoURN.FetchBufferedDataSize;
       clip.FetchBufferedData = videoURN.FetchBufferedData;
       clip.m_QtvInstanceHandle = videoURN.m_QtvInstanceHandle;
       break;
#endif  /* FEATURE_QTV_PSEUDO_STREAM */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
     case QtvPlayer::URN_HTTP_STREAM:
       clip.pVideoLocalBuffer = videoURN.pBufURN;

       clip.videoBufSize = videoURN.bufSize;
       clip.bHTTPStreaming = true;
       clip.FetchBufferedDataSize = videoURN.FetchBufferedDataSize;
       clip.FetchBufferedData = videoURN.FetchBufferedData;
       clip.m_QtvInstanceHandle = videoURN.m_QtvInstanceHandle;
       break;
#endif  /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  #ifdef FEATURE_QTV_DRM_DCF
     case QtvPlayer::URN_INPUTSTREAM:
         {
           //We are playing from IxStream, check if the underlying stream is Progressive?
           bool bIsProgressive = false;
           #ifdef FEATURE_QTV_PSEUDO_STREAM
             IxStream* pStream = (IxStream* )videoURN.inputStream;
             if(pStream && pStream->IsProgressive())
             {
               clip.bPseudoStreaming = true;
               bIsProgressive = true;
             }
           #endif
           #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
             IxStream* pixStream = (IxStream* )videoURN.inputStream;
             if(pixStream && pixStream->IsProgressive())
             {
               clip.bHTTPStreaming = true;
               bIsProgressive = true;
             }
           #endif
           if(bIsProgressive)
           {
             clip.FetchBufferedDataSize = videoURN.FetchBufferedDataSize;
             clip.FetchBufferedData = videoURN.FetchBufferedData;
             clip.m_QtvInstanceHandle = videoURN.m_QtvInstanceHandle;

           }
           break;
         }
  #endif
#endif//#endif #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

     case QtvPlayer::URN_SDP_FILE:
     case QtvPlayer::URN_PVX_FILE:
     case QtvPlayer::URN_RTSP:
     case QtvPlayer::URN_SDP_BUFFER:
       if (url.bValid)
       {
         //check for local file
         if (url.bLocalFileFromPVX)
         {
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
           if (!FileSavedToBuffer)
           {
              clip.pVideoLocalFilename = url.sURL->GetUrlBuffer();
           }
#endif
         }
         else
         {
          //either rtsp or download.
           switch (url.type)
           {
           case QtvPlayer::URL_STREAM:
             clip.bStreaming = true;
             break;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_HTTP_DOWNLOAD)
          case QtvPlayer::URL_DOWNLOAD:
            if (videoURN.type == QtvPlayer::URN_SDP_FILE)
             {
                clip.bQtvHttpStreaming = true;
             }
             else
             {
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
                clip.bDownload = true;
                if (!FileSavedToBuffer)
                {
                  char DownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
                  getDownloadTxtFileName(DownloadTxtFile, sizeof(DownloadTxtFile));
                  if (appControl.bResumeDownload)
                  {
                    OSCL_FILE *fp;
                    char c, field[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
                    fp = OSCL_FileOpen(DownloadTxtFile, "r");
                    uint32 len;
                    //skip leading blanks
                    for (len=0; OSCL_FileRead(&c,1,1,fp)==1 && len < sizeof(field);)
                    {
                      if (c>' ')
                      {
                        field[len++]=c; break;
                      }
                    }
                    //read file name
                    while (OSCL_FileRead(&c,1,1,fp)==1 && c!='\0' && len < sizeof(field))
                    {
                      field[len++]=c;
                    }
                    if( len < sizeof(field))
                    {
                      field[len++]='\0';
                    }
                    if (len == sizeof(field))
                    {
                      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SetClipType Filename too long" );
                      OSCL_FileClose(fp);
                      return false;
                    }
                    char* savedFileName = (char*) QTV_Malloc(len);
                    if(savedFileName)
                    {
                      (void)ZUtils::StrcpyN(savedFileName,field, len);
                    }
                    else
                    {
                      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SetClipType Malloc failed" );
                      OSCL_FileClose(fp);
                      return false;
                    }
                    OSCL_FileClose(fp);
                    clip.pAudioLocalFilename = clip.pVideoLocalFilename = savedFileName;
                  }
                  else
                  {
                    int32 len=strlen(pvxIF.appFile.GetClipName(0));
                    char *pUrl = (char*)QTV_Malloc(len+1);
                    if(NULL == pUrl)
                    {
                      return false;
                    }
                    (void)std_strlcpy(pUrl,pvxIF.appFile.GetClipName(0),len+1);

                    setDownloadFileName(pUrl);
                    if (downloadFileName)
                    {
                      clip.pAudioLocalFilename = clip.pVideoLocalFilename = downloadFileName;
                    }
                    else
                    {
#ifndef FEATURE_WINCE
                      clip.pAudioLocalFilename = clip.pVideoLocalFilename = (char *)"/brew/mod/mediaplayer/media/ftest.mp4";
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
                    }

                    QTV_Free(pUrl);

                  }
                }
#endif
             }
             break;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
           }
         }
       }
       break;
// If the URN belong to BCAST_ISDB, clip object members are initialized
// differently based on the feature flag. If the generic
// broadcast feature defined then set its own flag. In addition
// Mark the eGenericBcastMediaID appropriately.
#ifdef FEATURE_QTV_GENERIC_BCAST_ISDB
     case QtvPlayer::URN_BCAST_ISDB:
       clip.bBcastGeneric        = true;
       clip.eGenericBcastMediaID = Common::GENERIC_BCAST_MEDIA_ISDB;
       clip.bBcastStream = true;
       clip.bBcastISDB   = true;
       break;
#endif

#ifdef FEATURE_QTV_GENERIC_BCAST_CMMB
#error code not present
#endif

#ifdef FEATURE_QTV_GENERIC_BCAST_TDMB
#error code not present
#endif


// Media Mask section
#ifdef FEATURE_QTV_MAPI_MEDIA_MASK
#error code not present
#endif /* FEATURE_QTV_MAPI_MEDIA_MASK */

#ifdef FEATURE_QTV_MAPI_REPLAY
#error code not present
#endif

// If the URN belong to BCAST_FLO clip object members initialized
// differently based on the feature flag. If the generic
// broadcast feature defined then set its own flag. In addition
// Mark the eGenericBcastMediaID appropriately.
     case QtvPlayer::URN_BCAST_FLO:
#ifdef FEATURE_QTV_GENERIC_BCAST_FLO
#error code not present
#else
       /* Broadcast FLO */
       clip.bBcastFLO = true;
#endif
#ifdef FEATURE_QTV_GENERIC_BCAST
      clip.bBcastStream = true;
#endif
       break;

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
     case QtvPlayer::URN_QTV_HTTP_STREAM:
       // Progresssive Download with QTV HTTP stack
       clip.bQtvHttpStreaming = true;
       break;
#endif

     }
   }
   if (audioURN.bValid)
   {
     #ifdef FEATURE_QTV_DRM_DCF
       bool bipstreamprog = false;
       if(audioURN.type == QtvPlayer::URN_INPUTSTREAM)
       {
         IxStream* pStream = (IxStream* )audioURN.inputStream;
         if(pStream)
         {
           bipstreamprog = ((pStream->IsProgressive())!=0);
         }
       }
     #endif
      if (audioURN.type == QtvPlayer::URN_LOCAL_FILE)
      {
        clip.pAudioLocalFilename = audioURN.sURN->GetUrlBuffer();
      }
      else if (audioURN.type == QtvPlayer::URN_LOCAL_BUFFER

#ifdef FEATURE_QTV_PSEUDO_STREAM
               || audioURN.type == QtvPlayer::URN_PSEUDO_STREAM
              #ifdef FEATURE_QTV_DRM_DCF
              || bipstreamprog == true
              #endif
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
              || audioURN.type == QtvPlayer::URN_HTTP_STREAM
              #ifdef FEATURE_QTV_DRM_DCF
              || bipstreamprog == true
              #endif
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
              )
      {
         clip.pAudioLocalBuffer = audioURN.pBufURN;
         clip.audioBufSize = audioURN.bufSize;
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
         if( (audioURN.type == QtvPlayer::URN_HTTP_STREAM)
             #ifdef FEATURE_QTV_DRM_DCF
              || bipstreamprog == true
             #endif
            )
         {
             clip.bHTTPStreaming = true;
             clip.FetchBufferedDataSize = audioURN.FetchBufferedDataSize;
             clip.FetchBufferedData = audioURN.FetchBufferedData;
             clip.m_QtvInstanceHandle = audioURN.m_QtvInstanceHandle;

         }
#endif   /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_PSEUDO_STREAM
          if( (audioURN.type == QtvPlayer::URN_PSEUDO_STREAM)
             #ifdef FEATURE_QTV_DRM_DCF
              || bipstreamprog == true
             #endif
            )
         {
           clip.bPseudoStreaming = true;
           clip.FetchBufferedDataSize = audioURN.FetchBufferedDataSize;
           clip.FetchBufferedData = audioURN.FetchBufferedData;
           clip.m_QtvInstanceHandle = audioURN.m_QtvInstanceHandle;

         }
#endif   /* FEATURE_QTV_PSEUDO_STREAM */
      }
   }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   if(textURN.bValid)
   {
     #ifdef FEATURE_QTV_DRM_DCF
        bool bipstreamprog = false;
        if(textURN.type == QtvPlayer::URN_INPUTSTREAM)
        {
          IxStream* pStream = (IxStream* )textURN.inputStream;
          if(pStream)
          {
            bipstreamprog = ((pStream->IsProgressive())!=0);
          }
        }
     #endif
      if (textURN.type == QtvPlayer::URN_LOCAL_FILE)
      {
#ifndef FEATURE_WINCE
        clip.pTextLocalFilename = textURN.sURN->GetUrlBuffer();
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
      }
      else if (textURN.type == QtvPlayer::URN_LOCAL_BUFFER

#ifdef FEATURE_QTV_PSEUDO_STREAM
              || textURN.type == QtvPlayer::URN_PSEUDO_STREAM
              #ifdef FEATURE_QTV_DRM_DCF
              || bipstreamprog == true
              #endif
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
              || textURN.type == QtvPlayer::URN_HTTP_STREAM
              #ifdef FEATURE_QTV_DRM_DCF
              || textURN.type == QtvPlayer::URN_INPUTSTREAM
              #endif
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
              )
      {
         clip.pTextLocalBuffer = textURN.pBufURN;
         clip.textBufSize = textURN.bufSize;
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
         if( (textURN.type == QtvPlayer::URN_HTTP_STREAM)
             #ifdef FEATURE_QTV_DRM_DCF
              || bipstreamprog == true
             #endif
           )
         {
             clip.bHTTPStreaming = true;
             clip.FetchBufferedDataSize = textURN.FetchBufferedDataSize;
             clip.FetchBufferedData = textURN.FetchBufferedData;
             clip.m_QtvInstanceHandle = textURN.m_QtvInstanceHandle;
         }
#endif   /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_PSEUDO_STREAM
         if( (textURN.type == QtvPlayer::URN_PSEUDO_STREAM)
             #ifdef FEATURE_QTV_DRM_DCF
              || (bipstreamprog == true)
             #endif
           )
         {
           clip.bPseudoStreaming = true;
           clip.FetchBufferedDataSize = audioURN.FetchBufferedDataSize;
           clip.FetchBufferedData = audioURN.FetchBufferedData;
           clip.m_QtvInstanceHandle = audioURN.m_QtvInstanceHandle;
         }
#endif   /* FEATURE_QTV_PSEUDO_STREAM */
      }
   }
#endif //FEATURE_MP4_3GPP_TIMED_TEXT

#ifdef FEATURE_QTV_DRM_DCF
   clip.bValid = (clip.videoURN->bValid || clip.audioURN->bValid || clip.textURN->bValid);
#endif

   if (  clip.bStreaming ||
#ifndef FEATURE_QTV_GENERIC_BCAST
         clip.bBcastISDB ||
         clip.bBcastFLO ||
#else
         clip.bBcastStream     ||
#endif
         clip.pVideoLocalBuffer ||
         clip.pAudioLocalBuffer ||
         clip.pAudioLocalFilename ||
         clip.pVideoLocalFilename
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
        || clip.bDownload
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        || clip.pTextLocalBuffer
        || clip.pTextLocalFilename
#endif //FEATURE_MP4_3GPP_TIMED_TEXT
#ifdef FEATURE_QTV_DRM_DCF
        || clip.bValid
#endif
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        || clip.FetchBufferedDataSize
#endif   /* #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
        || clip.bQtvHttpStreaming
#endif
         )
   {
      return true;
   }

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SetClipType failed ");
   return false;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
OSCL_FILE * Mpeg4Player::OpenDownloadConfigFile(char *mode)
{
  char temp[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
  (void)std_strlcpy(temp,rootpath,sizeof(temp));
  uint temp_len = 0;  temp_len = strlen(temp);
  if((temp_len > 0) && (temp_len < (QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2)))
  {
    if (temp[temp_len - 1] != '/') (void)std_strlcat(temp,"/",sizeof(temp));
  }
  (void)std_strlcat(temp,DownloadConfigFilename,sizeof(temp));
    QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
            "PDS2: OpenDownloadConfigFile: temp file name %s",
            temp);
  return OSCL_FileOpen(temp,mode);
}
OSCL_FILE * Mpeg4Player::OpenDownloadDatFile(char *mode)
{
  char temp[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
  (void)std_strlcpy(temp,rootpath,sizeof(temp));
  uint temp_len = 0;  temp_len = strlen(temp);
  if((temp_len > 0) && (temp_len < (QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2)))
  {
    if (temp[temp_len -1]!='/') (void)std_strlcat(temp,"/",sizeof(temp));
  }
  (void)std_strlcat(temp,DownloadDatFilename,sizeof(temp));
    QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
            "PDS2: OpenDownloadDatFile: temp file name %s",
            temp);
  return OSCL_FileOpen(temp,mode);
}

/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
bool Mpeg4Player::DeleteDownloadDatFile(void)
{
    char temp[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
  (void)std_strlcpy(temp,rootpath,sizeof(temp));
  uint temp_len = 0;  temp_len = strlen(temp);
  if((temp_len > 0) && (temp_len < (QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2)))
  {
    if (temp[temp_len -1]!='/') (void)std_strlcat(temp,"/",sizeof(temp));
  }
  (void)std_strlcat(temp,DownloadDatFilename,sizeof(temp));
    QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
            "FT: OpenDownloadDatFile: temp file name %s",
            temp);
  return OSCL_FileDelete(temp);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


/* ======================================================================
FUNCTION
  Mpeg4Player::OpenURN

DESCRIPTION
//
//  Select a file
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::OpenURN(const PV_OPEN_URN_type * pEvent)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN" );

  bool bOK = false;

  if(guMpeg4PlayerRefCnt > 1)
  {
    //only audio spported from second inst onwards.
    pEvent->textURN->Reset();
  }

  // An OpenURN command will automatically stop the player.
  (void)Stop(NULL);
  // Clear any old URN info.
  InitURN();

  // Clear the zrex count of memory allocated
  (void)QTV_ClearBytesAllocated();

  //Set the rootpath first before doing anything else
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

  (void)std_strlcpy(rootpath,
                    (char*)(qtvConfigObject.GetQTVConfigItem(QtvConfig::QTVCONFIG_MEDIAPLAYER_ROOT_PATH)),
                     QTV_MAX_FILENAME_BYTES);
#endif

  //Set the URN
  if (pEvent)
  {
    //Set URN from the user command.
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::Calling Set VideoURN" );
    SetURN(*pEvent->videoURN, videoURN);
#ifdef FEATURE_QTV_WM_NO_STREAMING
    if ((videoURN.sURN->StartsWith("rtsp://") ||
         videoURN.sURN->StartsWith("rtspt://"))&&
        (videoURN.sURN->EndsWith(".wma") ||
         videoURN.sURN->EndsWith(".wmv")))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   "Windows Media Streaming not supported");
      return false;
    }
#endif
    if (pEvent->videoURN->EndsWith(".url") ||
        pEvent->videoURN->EndsWith(".sdp"))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_HIGH,
                   "Input is URL/SDP file. No need to set AudioURN" );
    }
    else if(pEvent->videoURN->StartsWith("sdp//"))
    {
      QTV_MSG( MSG_LEGACY_LOW,
               "Input is SDP buffer. No need to set AudioURN" );
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_HIGH,
                   "Input is Non URL/SDP file. Set AudioURN..." );
      SetURN(*pEvent->audioURN, audioURN);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Trying to Set textURN..." );
      SetURN(*pEvent->textURN, textURN);
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "No URN specified in OpenURN()" );
  }

  // Set the first URL
  if (!SetURL())
  {
    bOK = 0;
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::OpenURN SetURL failed");
  }
  else
  {

  memset (sUrnStatsName,0x00,QTV_STATS_LOCAL_BUFFER_SIZE);
  if (pEvent->videoURN->GetUrlBuffer()) {
    std_strlcpy(sUrnStatsName,pEvent->videoURN->GetUrlBuffer(),strlen(pEvent->videoURN->GetUrlBuffer())+1);
  }
  else if(pEvent->audioURN->GetUrlBuffer())
  {
    std_strlcpy(sUrnStatsName,pEvent->audioURN->GetUrlBuffer(),strlen(pEvent->audioURN->GetUrlBuffer())+1);
  }
  else if(pEvent->textURN->GetUrlBuffer())
  {
    std_strlcpy(sUrnStatsName,pEvent->textURN->GetUrlBuffer(),strlen(pEvent->textURN->GetUrlBuffer())+1);
  }

    //Open the clip
    bOK = OpenClip();
    if (!bOK)
    {
      ErrorAbort(OPEN_CLIP_ERROR);
    }
  }

  if (!bOK)
  {
    //Clear URN to prevent trying to download or play from it.
    InitURN();

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::OpenURN failed");
  }

  /*We have allocated space in event structure for video, audio URNs
  now free them here as they no longer required (handles both normal
  and url redirection case)*/
  if ((NULL != pEvent) && (NULL != pEvent->videoURN))
  {
    QTV_Delete(pEvent->videoURN);
  }
  if ((NULL != pEvent) && (NULL != pEvent->audioURN))
  {
    QTV_Delete(pEvent->audioURN);
  }
  if ((NULL != pEvent) && (NULL != pEvent->textURN))
  {
    QTV_Delete(pEvent->textURN);
  }

  return bOK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::OpenURN

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
bool Mpeg4Player::OpenURN(const PV_OPEN_BUFFER_URN_type * pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN for buffer" );

   bool bOK = false;

   // An OpenURN command will automatically stop the player.
  (void)Stop(NULL);
   // Clear any old URN info.
   InitURN();

   // Clear the zrex count of memory allocated
  (void)QTV_ClearBytesAllocated();

   //Set the URN
   if (pEvent)
   {
      //Set URN from the user command.

     if(guMpeg4PlayerRefCnt <= 1)
     {
       //only audio spported from second inst onwards.
       SetURN(pEvent->textBuffer, pEvent->textSize, textURN);
     }
     SetURN(pEvent->audioBuffer, pEvent->audioSize, audioURN);
     SetURN(pEvent->videoBuffer, pEvent->videoSize, videoURN);
   }

   //Buffer playback is only from file, so it can not be a URL.
   // Open the clip
   bOK = OpenClip();
   if (!bOK)
   {
      ErrorAbort(OPEN_CLIP_ERROR);
   }

   if (!bOK)
   {
      //Clear URN to prevent trying to download or play from it.
      InitURN();

      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::OpenURN failed");
   }
   return bOK;
}

#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::openPseudoStream

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
bool Mpeg4Player::OpenPseudoStream(const QTV_PS_STREAM_OPEN_EVENT_type * pEvent)
{
   QTV_MSG(QTVDIAG_MP4_PLAYER,"Mpeg4Player::OpenPseudoStream for pseudo streaming");
   bool bOK = false;
   // An OpenURN command will automatically stop the player.
   (void)Stop(NULL);
   // Clear any old URN info.
   InitURN();

   // Clear the zrex count of memory allocated
   (void)QTV_ClearBytesAllocated();

   //Set the URN
   if (pEvent)
   {
      qtv_event_ps_dl_start_type  qtv_event_ps_dl_start;
      /* Set URN from the user command. */

      if(guMpeg4PlayerRefCnt <= 1)
      {
        if(pEvent->dwTrackSelected & QTV_VIDEO_TRACK_SELECTED)
        {
          setPseudoStream(pEvent->pPSBuffer, pEvent->bufferSize,pEvent->FetchBufferedDataSize, pEvent->FetchBufferedData, pEvent->handle, videoURN);
        }
        if(pEvent->dwTrackSelected & QTV_TEXT_TRACK_SELECTED)
        {
          setPseudoStream(pEvent->pPSBuffer, pEvent->bufferSize,pEvent->FetchBufferedDataSize, pEvent->FetchBufferedData, pEvent->handle, textURN);
        }
      }

      if(pEvent->dwTrackSelected & QTV_AUDIO_TRACK_SELECTED)
      {
        setPseudoStream(pEvent->pPSBuffer, pEvent->bufferSize,pEvent->FetchBufferedDataSize, pEvent->FetchBufferedData, pEvent->handle, audioURN);
      }

    ///* Update the current write ptr offset.*/
    //if (pEvent->writePtrOffset)
    //   updatePseudoStreamBufferOffset(pEvent);
#ifndef FEATURE_WINCE
    ps_last_ts = ZUtils::Clock();
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

      qtv_event_ps_dl_start.totalBufSize =  pEvent->bufferSize;
      qtv_event_ps_dl_start.unfilledBufSize =
                               (pEvent->bufferSize - pEvent->writePtrOffset);
#ifndef FEATURE_WINCE
      event_report_payload(EVENT_QTV_PS_DOWNLOAD_STARTED,
                          sizeof(qtv_event_ps_dl_start_type),
                          &qtv_event_ps_dl_start);
#endif   /*    FEATURE_WINCE   */

   }

   /* Attempt to open the clip. When OpenClip returns, it is not guaranteed
      that the call results in an opened clip. Mpeg4player will stay in
    OPENING state until parser finishes parsing the main fragment
   */
   bOK = OpenClip();
   if (!bOK)
   {
      ErrorAbort(OPEN_CLIP_ERROR);
   }

   if (bOK)
   {
      QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::OpenPseudoStream OPENING ");
    appControl.nPseudoStreamDownloadRate = 0;
    Notify(QtvPlayer::QTV_PS_PLAYER_BUFFERING);
   }
   else
   {
    //Clear URN to prevent trying to download or play from it.
      InitURN();
      QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::OpenPseudoStream failed");
   }
   return bOK;
}

/*------------------------------------------------------------------------
   Method to update current pseudo stream buffer write pointer offset.
   Application should call this method and update Qtv after every write
   operation to pseudo stream buffer.

   Arguments:

   wPtrOffset: Write pointer offset from the beginning of pseudo stream buffer
           This value is expressed in bytes.

   wPtrOffset MUST always increment. Application should never pass an offset
   value which is less than the value issued in past updates. Such an update
   will be ignored.

  -------------------------------------------------------------------------*/
boolean Mpeg4Player::updatePseudoStreamBufferOffset( const QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_type * pEvent)
{
 static unsigned int prev_wPtrOffset = 0,cnt = 0;
 uint32 wPtrOffset = 0;

 if(pEvent)
 {
   //Pull interface
   if(httpPullData.FetchBufferedDataSize)
   {
     boolean bEndOfData = false;
     uint32 userData = 0;
      #ifdef FEATURE_QTV_DRM_DCF
        userData = (uint32)videoURN.inputStream;
      #endif
     (*httpPullData.FetchBufferedDataSize)(userData, &wPtrOffset,&bEndOfData,httpPullData.m_QtvInstanceHandle);
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "HttpPullDataInterface: DnldDataSize=%d"
                   ,wPtrOffset );
   }
   else
   {
     //Push interface
    wPtrOffset = pEvent->writePtrOffset;
 }
 }
 else
 {
    return FALSE;
 }

 /* download rate is expressed in Bits Per Second (BPS) */
 unsigned int download_rate = 0, duration = 0;


 QTV_MSG(QTVDIAG_MP4_PLAYER,
         "Mpeg4Player::updatePseudoStreamBufferOffset called");

 /* Code to compute download rate */
 ++cnt;
 if ((cnt % 3) == 0)
 {
   if (wPtrOffset < prev_wPtrOffset)
   {
       prev_wPtrOffset = 0;
   }
#ifndef FEATURE_WINCE
   duration = ZUtils::Clock() - ps_last_ts;
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
   if(duration)
       download_rate = (((wPtrOffset - prev_wPtrOffset) * 1000)/duration) * 8;

   appControl.nPseudoStreamDownloadRate = download_rate/1000;
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Mpeg4Player::Download rate %u BPS",download_rate);

   prev_wPtrOffset = wPtrOffset;
#ifndef FEATURE_WINCE
   ps_last_ts =ZUtils::Clock();
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
 }
 /* End of code to compute download rate */

 clip.pseudoStreamWritePtrOffset = wPtrOffset;

 //Generate DIAG event to report amount of data downloaded so far..
#ifndef FEATURE_WINCE
 event_report_payload(EVENT_QTV_DOWNLOAD_DATA_REPORT,
                           sizeof(wPtrOffset),
                           &wPtrOffset);
#endif   /*    FEATURE_WINCE   */

 if ( playerState == OPENING || playerState == PLAYER_PSEUDO_PAUSE)
 {
    Notify(QtvPlayer::QTV_PS_PLAYER_BUFFERING);
 }

 if(clip.pAudioMpeg4Playback)
 {
   clip.pAudioMpeg4Playback->UpdatePSBufferOffset(wPtrOffset);
 }
 if(clip.pVideoMpeg4Playback)
 {
   clip.pVideoMpeg4Playback->UpdatePSBufferOffset(wPtrOffset);
 }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
 if(clip.pTextMpeg4Playback)
 {
   clip.pTextMpeg4Playback->UpdatePSBufferOffset(wPtrOffset);
 }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

 return TRUE;
}
#endif /* FEATURE_QTV_PSEUDO_STREAM */


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
void Mpeg4Player::QTV_HTTP_BUFFER_UPDATE_handler(QCMessageHandleType handle)
{
  if(handle == NULL)
  {
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
     pThis->ProcessHTTPBufferUpdate((QTV_HTTP_BUFFER_UPDATE_type *)handle);
  }
}

void Mpeg4Player::QTV_HTTP_EVENT_handler(QCMessageHandleType handle)
{
  if(handle == NULL)
  {
     return;
  }

  Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
  if (pThis)
  {
     pThis->ProcessHTTPStateEvent((QTV_HTTP_EVENT_type*)handle);
  }
}

void Mpeg4Player::ProcessHTTPBufferUpdate(const QTV_HTTP_BUFFER_UPDATE_type *pEvent)
{
#ifdef FEATURE_QTV_PSEUDO_STREAM
  if(clip.bPseudoStreaming)
  {
    QTV_PS_PROCESS_PSEUDO_STREAM_type *pEvent2 = QCCreateMessage(QTV_PS_PROCESS_PSEUDO_STREAM,this);

    if(pEvent2)
    {
      pEvent2->bPSHasAudio = (bool) pEvent->bHasAudio;
      pEvent2->bPSHasVideo = (bool) pEvent->bHasVideo;
      pEvent2->bPSHasText = (bool) pEvent->bHasText;
      PostMessage(pEvent2);
    }
    if(clip.bHasVideo)
    {
      QTV_RESUME_VIDEO_type *video_pEvent = QCCreateMessage(QTV_RESUME_VIDEO, this);/*lint !e641 */
      if (video_pEvent)
      {
        PostMessage(video_pEvent);
      }
    }
    if(clip.bHasAudio)
    {
      QTV_RESUME_AUDIO_type *audio_pEvent = QCCreateMessage(QTV_RESUME_AUDIO, this);/*lint !e641 */
      if (audio_pEvent)
      {
        PostMessage(audio_pEvent);
      }
    }

    if(clip.bHasText)
    {
      QTV_RESUME_TEXT_type *text_pEvent = QCCreateMessage(QTV_RESUME_TEXT, this);/*lint !e641 */
      if (text_pEvent)
      {
        PostMessage(text_pEvent);
      }
    }
  }
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if(clip.bHTTPStreaming)
  {
    QTV_PROCESS_HTTP_STREAM_type *pEvent2 = QCCreateMessage(QTV_PROCESS_HTTP_STREAM,this);

    if (pEvent2)
    {
      pEvent2->bHasAudio = (bool) pEvent->bHasAudio;
      pEvent2->bHasVideo = (bool) pEvent->bHasVideo;
      pEvent2->bHasText = (bool) pEvent->bHasText;
      PostMessage(pEvent2);
    }
  }
#endif
}

void Mpeg4Player::ProcessHTTPStateEvent(const QTV_HTTP_EVENT_type *pEvent)
{
#ifdef FEATURE_QTV_PSEUDO_STREAM
  if(clip.bPseudoStreaming)
  {
    QTV_PS_PARSER_STATUS_EVENT_type *pEvent2 = QCCreateMessage(QTV_PS_PARSER_STATUS_EVENT, this);
    if (pEvent2)
    {
      pEvent2->status = pEvent->status;
      pEvent2->bHasVideo = pEvent->bHasVideo;
      pEvent2->bHasAudio = pEvent->bHasAudio;
      pEvent2->bHasText = pEvent->bHasText;
      PostMessage(pEvent2);
      if(pEvent->status == Common::PARSER_PAUSE)
      {
        if(clip.bHasVideo)
        {
          QTV_PAUSE_VIDEO_type *video_pEvent = QCCreateMessage(QTV_PAUSE_VIDEO, this);/*lint !e641 */
          if (video_pEvent)
          {
            PostMessage(video_pEvent);
          }
        }

        if(clip.bHasAudio)
        {
          QTV_PAUSE_AUDIO_type *audio_pEvent = QCCreateMessage(QTV_PAUSE_AUDIO, this);/*lint !e641 */
          if (audio_pEvent)
          {
            PostMessage(audio_pEvent);
          }
        }

        if(clip.bHasText)
        {
          QTV_PAUSE_TEXT_type *text_pEvent = QCCreateMessage(QTV_PAUSE_TEXT, this);/*lint !e641 */
          if (text_pEvent)
          {
            PostMessage(text_pEvent);
          }
        }
      }
    }
  }
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if(clip.bHTTPStreaming)
  {
    QTV_HTTP_PARSER_STATUS_EVENT_type *pEvent2 = QCCreateMessage(QTV_HTTP_PARSER_STATUS_EVENT, this);
    if (pEvent2)
    {
      pEvent2->status = pEvent->status;
      pEvent2->bHasVideo = pEvent->bHasVideo;
      pEvent2->bHasAudio = pEvent->bHasAudio;
      pEvent2->bHasText = pEvent->bHasText;
      PostMessage(pEvent2);
    }
    if(pEvent->status == Common::PARSER_PAUSE)
    {
      QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_type *pEventUnderrun = QCCreateMessage(QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT, this);

      if(pEventUnderrun)
      {
        pEventUnderrun->bAudio = (bool) pEvent->bHasAudio;
        pEventUnderrun->bVideo = (bool) pEvent->bHasVideo;
        pEventUnderrun->bText = (bool) pEvent->bHasText;
        PostMessage(pEventUnderrun);
      }
    }
  }
#endif
}

#endif



#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* ======================================================================
FUNCTION
  Mpeg4Player::openHTTPStream

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
bool Mpeg4Player::OpenHTTPStream(const QTV_HTTP_STREAM_OPEN_EVENT_type * pEvent)
{
   QTV_MSG(QTVDIAG_MP4_PLAYER,"Mpeg4Player::OpenHTTPStream for Progressive Download");
   bool bOK = false;
#ifndef FEATURE_WINCE
   // An OpenURN command will automatically stop the player.
   if (!clip.bQtvHttpStreaming)
   {
  (void)Stop(NULL);
   // Clear any old URN info.
   InitURN();

   // Clear the zrex count of memory allocated
  (void)QTV_ClearBytesAllocated();
   }
#endif   /*    FEATURE_WINCE   */
   //Set the URN
   if (pEvent)
   {
        //qtv_event_http_dl_start_type  qtv_event_http_dl_start;
        /* Set URN from the user command. */

      if(guMpeg4PlayerRefCnt <= 1)
      {
        if(pEvent->dwTrackSelected & QTV_VIDEO_TRACK_SELECTED)
        {
          setHTTPStream(pEvent->pPSBuffer, pEvent->bufferSize,
                        pEvent->FetchBufferedDataSize, pEvent->FetchBufferedData,pEvent->handle, videoURN);
        }
        if(pEvent->dwTrackSelected & QTV_TEXT_TRACK_SELECTED)
        {
          setHTTPStream(pEvent->pPSBuffer, pEvent->bufferSize,
                        pEvent->FetchBufferedDataSize, pEvent->FetchBufferedData,pEvent->handle, textURN);
        }
      }

      if(pEvent->dwTrackSelected & QTV_AUDIO_TRACK_SELECTED)
      {
          setHTTPStream(pEvent->pPSBuffer, pEvent->bufferSize,
                        pEvent->FetchBufferedDataSize, pEvent->FetchBufferedData,pEvent->handle, audioURN);
      }
#ifndef FEATURE_WINCE
        http_last_ts = ZUtils::Clock();
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

        /* Update the current write ptr offset.*/
        if(pEvent->bufferSize)
        {
            //push interface
            ASSERT(pEvent->pPSBuffer);
            clip.HTTPStreamWritePtrOffset = pEvent->writePtrOffset;
        }
        else
        {
            //pull interface
            uint32 userData = 0;
            #ifdef FEATURE_QTV_DRM_DCF
              userData = (uint32)videoURN.inputStream;
            #endif
            ASSERT(pEvent->FetchBufferedDataSize);
            ASSERT(pEvent->FetchBufferedData);
            boolean bEndOfData = false;
            (* pEvent->FetchBufferedDataSize)(userData,&httpPullData.dnldDataSize,&bEndOfData, pEvent->handle);
            clip.HTTPStreamWritePtrOffset = httpPullData.dnldDataSize;
        }

        /* Attempt to open the clip. When OpenClip returns, it is not guaranteed
        that the call results in an opened clip. Mpeg4player will stay in
        OPENING state until parser finishes parsing the main fragment
        */
        bOK = OpenClip();
        if (!bOK)
        {
            ErrorAbort(OPEN_CLIP_ERROR);
        }
   }

   if (bOK)
   {
     QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::OpenHTTPStream OPENING ");
     appControl.nHTTPStreamDownloadRate = 0;
     Notify(QtvPlayer::QTV_HTTP_PLAYER_BUFFERING);
   }
   else
   {
    //Clear URN to prevent trying to download or play from it.
      InitURN();
      QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::OpenHTTPStream failed");
   }
   return bOK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::updateHTTPStreamBufferOffset

DESCRIPTION
   Method to update current HTTP stream buffer write pointer offset.
   Application should call this method and update Qtv after every write
   operation to HTTP stream buffer.

   Arguments:

   wPtrOffset: Write pointer offset from the beginning of pseudo stream buffer
           This value is expressed in bytes.

   wPtrOffset MUST always increment. Application should never pass an offset
   value which is less than the value issued in past updates. Such an update
   will be ignored.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  void

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::updateHTTPStreamBufferOffset(const QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_type* pEvent)
{
 uint32 wPtrOffset = 0;

 if(pEvent)
 {
   //Pull interface
   if(httpPullData.FetchBufferedDataSize)
   {
      boolean bEndOfData = false;
      uint32 userData = 0;
      #ifdef FEATURE_QTV_DRM_DCF
        userData = (uint32)videoURN.inputStream;
      #endif
     (*httpPullData.FetchBufferedDataSize)(userData,&wPtrOffset,&bEndOfData, httpPullData.m_QtvInstanceHandle);
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "HttpPullDataInterface: DnldDataSize=%d"
                   ,wPtrOffset );
   }
   else
   {
     //Push interface
    wPtrOffset = pEvent->writePtrOffset;
 }
 }
 else
 {
     return FALSE;
 }

 /* download rate is expressed in Bits Per Second (BPS) */
 unsigned int download_rate = 0, duration = 0;
 uint32 totalPBTime = 0;

 QTV_MSG(QTVDIAG_MP4_PLAYER,
         "Mpeg4Player::updateHTTPStreamBufferOffset called");

 /* Code to compute download rate */
#ifndef FEATURE_WINCE
 duration = ZUtils::Clock() - http_last_ts;
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

 if(duration && clip.HTTPStreamWritePtrOffset && (wPtrOffset > clip.HTTPStreamWritePtrOffset))
 {
   download_rate = (((wPtrOffset - clip.HTTPStreamWritePtrOffset) * 1000 * 8)/duration);
   appControl.nHTTPStreamDownloadRate = download_rate/1024;
#ifndef FEATURE_WINCE
   http_last_ts = ZUtils::Clock();
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

   QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Mpeg4Player::Download rate %u BPS, CurrentOffSet %u,PreviousOffSet %u",download_rate,wPtrOffset,clip.HTTPStreamWritePtrOffset);
 }
 /* End of code to compute download rate */

 clip.HTTPStreamWritePtrOffset = wPtrOffset;

 //Generate DIAG event to report amount of data downloaded so far..
#ifndef FEATURE_WINCE
 event_report_payload(EVENT_QTV_DOWNLOAD_DATA_REPORT,
                           sizeof(wPtrOffset),
                           &wPtrOffset);
#endif   /*    FEATURE_WINCE   */

 if ( playerState == OPENING || playerState == PLAYER_HTTP_PAUSE)
 {
    Notify(QtvPlayer::QTV_HTTP_PLAYER_BUFFERING);
 }

   if(clip.pAudioMpeg4Playback)
   {
     clip.pAudioMpeg4Playback->UpdateHTTPBufferOffset(wPtrOffset);
   }
   if(clip.pVideoMpeg4Playback)
   {
     clip.pVideoMpeg4Playback->UpdateHTTPBufferOffset(wPtrOffset);
   }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(clip.pTextMpeg4Playback)
  {
    clip.pTextMpeg4Playback->UpdateHTTPBufferOffset(wPtrOffset);
 }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

 switch (playerState)
 {
   case PLAYER_HTTP_PAUSE:
       QtvPlayer::PlayerStateRecordT ps;

       GetPlayerState(ps);
       totalPBTime = ps.playbackMsec + http_bufferingResumeTime;

       if(HTTPCanPlayTracks(totalPBTime))
       {
         //Stop timer for pull interface
         if(clip.FetchBufferedDataSize)
         {
             HttpPullTimerStop();
         }

      PV_PLAY_CLIP_type *pPlayClipEvent = QCCreateMessage(PV_PLAY_CLIP, this);/*lint !e641 */
         if (pPlayClipEvent)
         {
           //resume
           pPlayClipEvent->startTime = -1;
           pPlayClipEvent->stopTime = -1;
           PostMessage(pPlayClipEvent);
           Notify(QtvPlayer::QTV_PLAYER_STATUS_HTTP_RESUME);
         }
       }
       break;

   default:
       break;
 }
 return TRUE;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::setStartAndBufferingTime

DESCRIPTION
  Sets the optimised start time and buffering time for HTTP Streaming.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  void

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::setStartAndBufferingTime(uint32 startupTime, uint32 bufferingResumeTime)
{
    if(startupTime < HTTP_DEFAULT_STARTUP_TIME)
    {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player=> startupTime: %d < HTTP_DEFAULT_STARTUP_TIME: %d",
            startupTime,HTTP_DEFAULT_STARTUP_TIME);
    }
    else
    {
        http_startupTime = startupTime;
    }

    if(bufferingResumeTime < HTTP_DEFAULT_REBUFFERING_TIME)
    {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player=> bufferingResumeTime: %d < HTTP_DEFAULT_REBUFFERING_TIME: %d",
            bufferingResumeTime,HTTP_DEFAULT_REBUFFERING_TIME);
    }
    else
    {
        http_bufferingResumeTime = bufferingResumeTime;
    }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ProcessHTTPStream

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
bool Mpeg4Player::ProcessHTTPStream(const QTV_PROCESS_HTTP_STREAM_type * pEvent )
{
  bool retVal = false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ProcessHTTPStream" );

    /* call both ProcessAudio() and PrpcessVideo() */
    /* Also check for pAudioMpeg4Playback and pVideoMpeg4Playback
       because CleanPlayback may have been callend before this event */

    if ((pEvent->bHasAudio) && (clip.pAudioMpeg4Playback))
    { /* Got audio */
       retVal = clip.pAudioMpeg4Playback->ParseHTTPStream();
    }

    if ((pEvent->bHasVideo) && (clip.pVideoMpeg4Playback))
  { /* Got video, parse it */
        retVal = clip.pVideoMpeg4Playback->ParseHTTPStream();
     }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if ((pEvent->bHasText) && (clip.pTextMpeg4Playback))
  { /* Got text, parse it */
    retVal = clip.pTextMpeg4Playback->ParseHTTPStream();
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  pEvent = 0;
  return retVal;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::HTTPPause

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
bool Mpeg4Player::HTTPPause(const QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_type*)
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT");
    //event_report(EVENT_QTV_PLAYER_STATE_HTTP_PAUSE);
    switch(playerState)
    {
     case PLAYING:
         if(clip.bHasVideo)
         {
      QTV_PAUSE_VIDEO_type *video_pEvent = QCCreateMessage(QTV_PAUSE_VIDEO, this);/*lint !e641 */
           if (video_pEvent)
           {
             PostMessage(video_pEvent);
           }
         }

         if(clip.bHasAudio)
         {
      QTV_PAUSE_AUDIO_type *audio_pEvent = QCCreateMessage(QTV_PAUSE_AUDIO, this);/*lint !e641 */
           if (audio_pEvent)
           {
             PostMessage(audio_pEvent);
           }
         }

         if(clip.bHasText)
         {
      QTV_PAUSE_TEXT_type *text_pEvent = QCCreateMessage(QTV_PAUSE_TEXT, this);/*lint !e641 */
           if (text_pEvent)
           {
             PostMessage(text_pEvent);
           }
         }
         Notify(PLAYER_HTTP_PAUSE);
         Notify(QtvPlayer::QTV_PLAYER_STATUS_HTTP_PAUSE);

         //Start timer for pull interface
         if(clip.FetchBufferedDataSize)
         {
            HttpPullTimerStart();
         }

         break;

     default:
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Notify: Player already in state PLAYER_HTTP_PAUSE");
         Notify(PLAYER_HTTP_PAUSE);
         break;
    }

    return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::HTTPCanPlayTracks

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
bool Mpeg4Player::HTTPCanPlayTracks(uint32 pbTime)
{
    if ((clip.bHasVideo) && (clip.pVideoMpeg4Playback))
    {
      if (!clip.pVideoMpeg4Playback->HTTPCanPlayTracks(pbTime))
          return false;
    }
    if((clip.bHasAudio) && (clip.pAudioMpeg4Playback))
    {
      if (!clip.pAudioMpeg4Playback->HTTPCanPlayTracks(pbTime))
          return false;
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if((clip.bHasText) && (clip.pTextMpeg4Playback))
    {
    if (!clip.pTextMpeg4Playback->HTTPCanPlayTracks(pbTime))
          return false;
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  return true;
}

#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::ProcessPseudoStream

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
bool Mpeg4Player::ProcessPseudoStream(const QTV_PS_PROCESS_PSEUDO_STREAM_type * pEvent )
{
  bool retVal = false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ProcessPseudoStream" );
  //Following macro returns false if pointer, (pEvent in this case) is NULL.
  QTV_NULL_PTR_CHECK(pEvent, false);

    /* call both ParseAudio() and ParseVideo() */
    /* Also check for pAudioMpeg4Playback and pVideoMpeg4Playback
       because CleanPlayback may have been callend before this event */

    if ((pEvent->bPSHasAudio) && (clip.pAudioMpeg4Playback))
    { /* Got audio, parse it */
       retVal = clip.pAudioMpeg4Playback->ParsePseudoStream();
    }

    if ((pEvent->bPSHasVideo) && (clip.pVideoMpeg4Playback))
    { /* Got video, parse it */
        retVal = clip.pVideoMpeg4Playback->ParsePseudoStream();
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if ((pEvent->bPSHasText) && (clip.pTextMpeg4Playback))
    { /* Got text, parse it */
      retVal = clip.pTextMpeg4Playback->ParsePseudoStream();
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  pEvent = 0;
  return retVal;
}
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

/* ======================================================================
FUNCTION
  Mpeg4Player::SetLocalFileSize

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
uint32 Mpeg4Player::SetLocalFileSize(const char *pFileName)
{
   uint32 size=0;
   if (pFileName)
   {
      size = OSCL_FileSize( pFileName );
   }
   return size;
}

#ifdef FEATURE_QTV_DRM_DCF

/* ======================================================================
FUNCTION
  Mpeg4Player::PV_OPEN_INPUTSTREAM_URN_handler

DESCRIPTION
  Handler for PV_OPEN_INPUTSTREAM_URN posted by QtvPlayer

DEPENDENCIES
  None

INPUT PARAMETERS:
->PV_OPEN_INPUTSTREAM_URN_type:An event containing IxStream*
  for audio/video/text instances.

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::PV_OPEN_INPUTSTREAM_URN_handler(QCMessageHandleType handle)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PV_OPEN_INPUTSTREAM_URN_handler" );

   if(handle == NULL)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::PV_OPEN_INPUTSTREAM_URN_handler: Input param handle is NULL");
      return;
   }

   Mpeg4Player *pThis = (Mpeg4Player *)((zrex_event_header_type *)handle)->user_data;
   if (pThis)
   {
      if (pThis->OpenURN((PV_OPEN_INPUTSTREAM_URN_type *)handle))
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_OPEN_URN_COMPLETE);
      }
      else
      {
         pThis->Notify(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED);
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetURN

DESCRIPTION
  Initialized the URN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::SetURN(dcf_ixstream_type inputStream, URNData& destURN)
{
   if (!inputStream)
      return;

   URNData urn;
   if (!urn.Init())
   {
     return;
   }
   urn.inputStream = inputStream;
   urn.type        = QtvPlayer::URN_INPUTSTREAM;
   urn.bValid      = true;
   #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   //Get from IxStream C interface
   urn.FetchBufferedDataSize = (QtvPlayer::FetchBufferedDataSizeT) ixstream_length;
   #endif

   //Copy new urn into global data.
   QCUtils::EnterCritSect(&URN_CS);
   destURN = urn;
   QCUtils::LeaveCritSect(&URN_CS);

   // Get clip source and file type for this url.
   (void)SetClipType();
}
/* ======================================================================
FUNCTION
  Mpeg4Player::OpenURN

DESCRIPTION
  None

DEPENDENCIES
  None

RETURN VALUE
  True or False

SIDE EFFECTS
  None

========================================================================== */
bool Mpeg4Player::OpenURN(PV_OPEN_INPUTSTREAM_URN_type * pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN for inputstream" );

   bool bOK = false;
   if(!pEvent)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Mpeg4Player::OpenURN NULL pEvent" );
     return false;
   }

   videoURN.inputStream = pEvent->videoStream;
   audioURN.inputStream = pEvent->audioStream;
   textURN.inputStream = pEvent->textStream;

   // An OpenURN command will automatically stop the player.
   (void)Stop(NULL);
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN after STOP" );

   // Clear any old URN info.
   InitURN();
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN after InitURN" );

   // Clear the zrex count of memory allocated
   (void)QTV_ClearBytesAllocated();

   //Set the URN
   if (pEvent)
   {
     //Set URN from the user command.
     SetURN(pEvent->videoStream, videoURN);
     SetURN(pEvent->audioStream, audioURN);
     SetURN(pEvent->textStream, textURN);
   }
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN after setURN" );

   // Open the clip
   bOK = OpenClip();
   if (!bOK)
   {
     ErrorAbort(OPEN_CLIP_ERROR);
   }
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN after OpenClip" );
   if (!bOK)
   {
     //Clear URN to prevent trying to download or play from it.
     InitURN();
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::OpenURN failed");
   }
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::OpenURN returning" );
   return bOK;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::SetLocalFileSize

DESCRIPTION
  Returns the size for local file. Also, applicable when media is DCF DRM media.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
int Mpeg4Player::SetLocalFileSize(URNData& urn)
{
   int size=0;
   if (urn.bValid)
   {
      if (urn.type == QtvPlayer::URN_INPUTSTREAM)
      {
             #ifdef FEATURE_QTV_DRM_DCF_TEST
           OSCL_FILE *fp;
           fp = OSCL_FileOpen(urn.inputStream);
           if (fp)
           {
             int s = OSCL_FileSeek(fp, 0, SEEK_SET);
             if (s == 0)
             {
               int nSet,nEnd;
               nSet = OSCL_FileTell(fp);
               s = OSCL_FileSeek(fp,0,SEEK_END);
               if (s == 0)
               {
                  nEnd=OSCL_FileTell(fp);
                  size=(nEnd-nSet+1);
               }
             }
             (void)OSCL_FileClose(fp);
           }
         #elif defined(FEATURE_QTV_DRM_DCF)
          if(urn.inputStream)
          {
                  IxErrnoType errorCode = E_FAILURE;
            IxStream* pStream = (IxStream* )urn.inputStream;
            if(pStream)
            {
              if(pStream->IsProgressive())
              {
                size = 0x7FFFFFFF;
              }
              else
              {
                errorCode = pStream->Size((uint32*)&size);
              }
              if(errorCode != E_SUCCESS)
              {
                        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"SetLocalFileSize(URNData& urn):errorCode %d",errorCode);
                    }
              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"SetLocalFileSize(URNData& urn):size %d",size);
            }
              }
         #endif
      }
   }
   return size;
}
#endif
/* ======================================================================
FUNCTION
  Mpeg4Player::IsAudioVideoTextFromSeparateFiles

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
bool Mpeg4Player::IsAudioVideoTextFromSeparateFiles()
{

   if ( (videoURN.type==QtvPlayer::URN_LOCAL_FILE) || (audioURN.type==QtvPlayer::URN_LOCAL_FILE) || (textURN.type==QtvPlayer::URN_LOCAL_FILE) ||
        (videoURN.type==QtvPlayer::URN_LOCAL_BUFFER) || (audioURN.type==QtvPlayer::URN_LOCAL_BUFFER) || (textURN.type==QtvPlayer::URN_LOCAL_BUFFER)
#ifdef FEATURE_QTV_PSEUDO_STREAM
        || (videoURN.type==QtvPlayer::URN_PSEUDO_STREAM) || (audioURN.type==QtvPlayer::URN_PSEUDO_STREAM) || (textURN.type==QtvPlayer::URN_PSEUDO_STREAM)
#endif /* FEATURE_QTV_PSEUDO_STREAM */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
        ||(videoURN.type==QtvPlayer::URN_PVX_FILE && TRUE == clip.bDownload)
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
        || (videoURN.type==QtvPlayer::URN_HTTP_STREAM) || (audioURN.type==QtvPlayer::URN_HTTP_STREAM) || (textURN.type==QtvPlayer::URN_HTTP_STREAM)
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_DRM_DCF
    || (videoURN.type == QtvPlayer::URN_INPUTSTREAM)
    || (audioURN.type == QtvPlayer::URN_INPUTSTREAM)
    || (textURN.type == QtvPlayer::URN_INPUTSTREAM)
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
    || (videoURN.type==QtvPlayer::URN_QTV_HTTP_STREAM)
#endif
      )
   {
      if ( (clip.pVideoMpeg4Playback != clip.pAudioMpeg4Playback) ||
            (clip.pVideoMpeg4Playback != clip.pTextMpeg4Playback) )
      {
         return true;
      }
   }
   return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetAudioMedia

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
Media * Mpeg4Player::GetAudioMedia()
{
  if(clip.bHasAudio
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
    )
  {
    if(IsAudioVideoTextFromSeparateFiles())
    {
      return clip.pAudioMpeg4Playback;
    }
    /* if audio/video are being played with same media, then only
    clip.pVideoMpeg4Playback is valid and clip.pAudioMpeg4Playback is NULL */
    return clip.pVideoMpeg4Playback;
  }
  return NULL;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetVideoMedia

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
Media * Mpeg4Player::GetVideoMedia()
{
  if( clip.bHasVideo && clip.pVideoMpeg4Playback )
    return clip.pVideoMpeg4Playback;
  return NULL;
}

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION
  Mpeg4Player::GetTextMedia

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
Media * Mpeg4Player::GetTextMedia()
{
  if(clip.bHasText)
  {
    if(IsAudioVideoTextFromSeparateFiles())
    {
      return clip.pTextMpeg4Playback;;
    }
    return clip.pVideoMpeg4Playback;
  }
  return NULL;
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

/* ======================================================================
FUNCTION
  Mpeg4Player::HttpPullDataInterface

DESCRIPTION
//
// APC used to check the amt of data available from OEM.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::HttpPullDataInterface(unsigned long mySelf)
{
    Mpeg4Player* pPlayer = (Mpeg4Player*)(mySelf);
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    if(pPlayer->clip.bHTTPStreaming && pPlayer->httpPullData.bTimerSet && pPlayer->httpPullData.pUpdateWptrOffsetPD)
    {
        //Use the static event to update the amt of data available
      //Data Size will be pulled from OEM by the event handler
      (pPlayer->httpPullData.pUpdateWptrOffsetPD)->writePtrOffset = 0;
        QCUtils::PostMessage(pPlayer->httpPullData.pUpdateWptrOffsetPD, 0, NULL);

       //Reset the timer
    (void)rex_set_timer(pPlayer->httpPullData.pHttpPullTimer,QTV_HTTP_PULL_DATA_INTERVAL);
    }
#endif

#ifdef FEATURE_QTV_PSEUDO_STREAM
    if(pPlayer->clip.bPseudoStreaming && pPlayer->httpPullData.bTimerSet && pPlayer->httpPullData.pUpdateWptrOffsetPS)
    {
        //Use the static event to update the amt of data available
      //Data Size will be pulled from OEM by the event handler
      (pPlayer->httpPullData.pUpdateWptrOffsetPS)->writePtrOffset = 0;
        QCUtils::PostMessage(pPlayer->httpPullData.pUpdateWptrOffsetPS, 0, NULL);

       //Reset the timer
    (void)rex_set_timer(pPlayer->httpPullData.pHttpPullTimer,QTV_HTTP_PULL_DATA_INTERVAL);
    }
#endif

    return;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif // defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

#ifdef FEATURE_QTV_GENERIC_BCAST
/*===========================================================================
FUNCTION
  Mpeg4Player::OpenGenericBcastMediaSource

DESCRIPTION
  Initializes the Generic broadcast media, and move the source
  filter to ready state. Playback will begin upon receiving the
  user command. QTV will start pulling data from the source.

RETURN VALUE
  true if successful, else false.

SIDE EFFECTS
  None.

===========================================================================*/
bool Mpeg4Player::OpenGenericBcastMediaSource()
{
  //error check
  if (playerState!=IDLE)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                  QTVDIAG_PRIO_ERROR,
                  "Weird state %d in OpenGenericBcastMediaSource",
                  playerState);
    return false;
  }
  if(!IsAudioVideoTextFromSeparateFiles())
  {
    if (clip.pVideoMpeg4Playback)
    {
      QTV_Delete(clip.pVideoMpeg4Playback);
      clip.pVideoMpeg4Playback=NULL;
      clip.pAudioMpeg4Playback=NULL;
      clip.pTextMpeg4Playback =NULL;
    }
  }
  else
  {
  if (clip.pVideoMpeg4Playback)
  {
    QTV_Delete(clip.pVideoMpeg4Playback);
    clip.pVideoMpeg4Playback=NULL;
  }

  if (clip.pAudioMpeg4Playback)
  {
    QTV_Delete(clip.pAudioMpeg4Playback);
    clip.pAudioMpeg4Playback=NULL;
  }
    if (clip.pTextMpeg4Playback)
    {
      QTV_Delete(clip.pTextMpeg4Playback);
      clip.pTextMpeg4Playback=NULL;
    }
  }


  /*-------------------------------------------------------------------------
    Construct the generic broadcast media object. It takes the mpeg4Player
    object pointer and the media source identifier.
  -------------------------------------------------------------------------*/
  clip.pTextMpeg4Playback  =
  clip.pAudioMpeg4Playback =
  clip.pVideoMpeg4Playback = Media::ConstructGenericBcastMedia((void *) this,
                                               clip.eGenericBcastMediaID);
  if ( !clip.pVideoMpeg4Playback )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "OpenGenericBcastMediaSource: Media object construction failed");
    return false;
  }

  /*-------------------------------------------------------------------------
    Initialize the bcast media and transition to OPENING state. The response
    to Open() command is received asynchronously and is processed by
    GenericBcastMediaStatusHandler().
  -------------------------------------------------------------------------*/
  if (!GENERIC_BCAST_MEDIA_OPEN(clip.pVideoMpeg4Playback,
                                videoURN.sURN->GetUrlBuffer()))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Failed to queue the Bcast Media Open Request");
    PlaybackErrorAbort();
    return false;
  }

  /* Transition the player to opening state */
  Notify(OPENING);

  return true;
}

const char* aGenericBcastMediaStr[] =
{
   "GENERIC_BCAST_MEDIA_INVALID", /* Invalid Status */
   "GENERIC_BCAST_MEDIA_ISDB",    /* ISDB Media */
   "GENERIC_BCAST_MEDIA_FLO",     /* FLO Media  */
   "GENERIC_BCAST_MEDIA_TDMB"     /* TDMB Media */
};

/*===========================================================================
FUNCTION
  Mpeg4Player::ConvertGenericBcastSourceToStr

DESCRIPTION
  Converts the source to its equivalent debug string understandable
  by troubleshooters.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
const char* Mpeg4Player::ConvertGenericBcastSourceToStr
(
  Common::GenericBcastMediaSourceID eID  /* source */
)
{
  if(eID >= ARR_SIZE(aGenericBcastMediaStr))
  {
    return "Invalid Media";
  }
  return aGenericBcastMediaStr[eID];
}


const char* aGenericBcastStatusStr[] =
{
   "GENERIC_BCAST_STATUS_OPEN_COMPLETE",
   "GENERIC_BCAST_STATUS_OPEN_FAILED",
   "GENERIC_BCAST_STATUS_CLOSE_COMPLETE",
   "GENERIC_BCAST_STATUS_REPOSITION_COMPLETE",
   "GENERIC_BCAST_STATUS_REPOSITION_FAILED",
   "GENERIC_BCAST_STATUS_SEEK_SYNC_COMPLETE",
   "GENERIC_BCAST_STATUS_SEEK_SYNC_FAILED",
   "GENERIC_BCAST_STATUS_TRACK_LIST_UPDATE",
   "GENERIC_BCAST_STATUS_PCR_UPDATE",
   "GENERIC_BCAST_STATUS_USER_DATA_UPDATE",
   "GENERIC_BCAST_STATUS_ABORT",
   "GENERIC_BCAST_STATUS_END_OF_TSB",
   "GENERIC_BCAST_STATUS_TSB_OVERFLOW",
   "GENERIC_BCAST_STATUS_TSB_DATA_LOSS"
};

/*===========================================================================
FUNCTION
  Mpeg4Player::GenericBcastConvertStatusToString

DESCRIPTION
  Converts the status to its equivalen debug string understandable
  by troubleshooters.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
const char* Mpeg4Player::GenericBcastConvertStatusToString
(
  Common::GenericBcastMediaStatusCode eStatus  /* status */
)
{
  if(eStatus >= Common::GENERIC_BCAST_STATUS_MAX)
  {
    return "Invalid Status";
  }
  return aGenericBcastStatusStr[eStatus];
}
/*===========================================================================
FUNCTION
  Mpeg4Player::ActOnUserTrackSelection

DESCRIPTION
  Acts on the ttrack selection trigger from the UI.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
bool Mpeg4Player::ActOnUserTrackSelection(int nAudID,
                                          int nVidID,
                                          int nTextID)
{
  // Obtain the secondary track list
  // Analyze the primary and secondary tracks list
  // Take action to update the secondary to primary in the gbm object.
  unsigned int  nResult = 0;
  bool bError = false;

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if(IsLocalMediaPLayback() || clip.bHTTPStreaming)
#else
  if(IsLocalMediaPLayback())
#endif
  {
    if((clip.pAudioMpeg4Playback) && (clip.bHasAudio) && (playerState== PB_READY))
    {
      bError = false;
      nResult = clip.pAudioMpeg4Playback->UserCompare(bError, nAudID,-1,-1);
    }
    else
    {
        bError = true;
    }
    if((clip.pVideoMpeg4Playback) && (clip.bHasVideo) && (playerState== PB_READY))
    {
      bError = false;
      nResult = clip.pVideoMpeg4Playback->UserCompare(bError, -1,nVidID,-1);
    }
    else
    {
        bError = true;
    }
    if((clip.pTextMpeg4Playback) && (clip.bHasText) && (playerState== PB_READY))
    {
      bError = false;
      nResult = clip.pTextMpeg4Playback->UserCompare(bError, -1,-1,nTextID);
    }
    else
    {
        bError = true;
    }

    if (bError)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Localfileplayback ActOnUserTrackSelection failed: Returning false");
      return false;
    }
  }
  else
  {
  nResult = GENERIC_BCAST_MEDIA_USER_COMPARE(clip.pVideoMpeg4Playback,
                                             nAudID,nVidID,nTextID,bError);
  }
  if(bError || (nResult & QTV_MEDIA_TRACK_LIST_ABORT))
  {
    // Raise Error message and abort
    PlaybackErrorAbort();
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Generic Bcast Media User Compare failed: Raising abort");
    return false;
  }
  if(IsLocalMediaPLayback())
  {
    if(!LocalMediaPLaybackUpgradeTracks(nResult))
    {
      PlaybackErrorAbort();
      return false;
    }
    else
    {
      return true;
    }
  }
  if(!GenericBcastUpgradeTracks(nResult))
  {
    PlaybackErrorAbort();
    return false;
  }
  return true;
}
/*===========================================================================
FUNCTION
  Mpeg4Player::ActOnNetworkTrackListUpdate

DESCRIPTION
  Acts on the track list update trigger from the network or media source side.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
bool Mpeg4Player::ActOnNetworkTrackListUpdate(void)
{
  // Obtain the secondary track list
  // Analyze the primary and secondary tracks list
  // Take action to update the secondary to primary in the gbm object.
  unsigned int  nResult;
  bool bError = false;

  nResult = GENERIC_BCAST_MEDIA_NETWORK_COMPARE(clip.pVideoMpeg4Playback,bError);
  if(bError || (nResult & QTV_MEDIA_TRACK_LIST_ABORT))
  {
    // Raise Error message and abort
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Generic Bcast Media Network Compare failed: Raising abort");
    return false;
  }
  return GenericBcastUpgradeTracks(nResult);
}
/*===========================================================================
FUNCTION
  Mpeg4Player::GenericBcastUpgradeTracks

DESCRIPTION
  Player is updated with new track list parameters selected.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
bool Mpeg4Player::GenericBcastUpgradeTracks(unsigned int nResult)
{
  bool bRet=true;
  unsigned long vidstart=0;
  // ABORT case is handled before invoking this function
  // hence if the result is non-trivial then dsp reset is performed
  // This includes the track addition scenarios also.
  if(nResult & (~QTV_MEDIA_TRACK_LIST_IGNORE))
  {
    QtvPlayer::PlayerStateRecordT ps;
    // Freeze the player; While freezing playback position is not important.
    bRet = StateBasedFreezeOrActivate(true,nResult,0);
    GetPlayerState(ps);
    vidstart = ps.playbackMsec;
  }

  // Update the state information in the Mpeg4Player class.
  if(bRet && (nResult != QTV_MEDIA_TRACK_LIST_IDENTICAL))
  {
    UpdateClipInfo(clip.pVideoMpeg4Playback,nResult);
  }

  if(bRet && (nResult & (~QTV_MEDIA_TRACK_LIST_IGNORE)))
  {
    // Wakeup the player with newly instantiated tracks
    // Since freezing is done within same check vidstart will be initialized.
    bRet = StateBasedFreezeOrActivate(false,nResult,vidstart);
  }



  // If the new and old tracks lists remain same result should have only bit
  // Otherwise we need to send the track list update.
  // This includes the case when the track identifiers alone modified, even though
  // all the other parameters remains the same.
  if(bRet && (nResult != QTV_MEDIA_TRACK_LIST_IDENTICAL))
  {
    // Issue Track List Update when the track IDs are modified also.
    Notify(QtvPlayer::QTV_PLAYER_STATUS_TRACK_LIST_UPDATE);
  }
  return bRet;
}

/*===========================================================================
FUNCTION
  Mpeg4Player::GenericBcastMediaStatusHandler

DESCRIPTION
  This routine acts on the asynchronous status received from the
  media source. At present only open complete is processed.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void Mpeg4Player::GenericBcastMediaStatusHandler
(
  const GENERIC_BCAST_STATUS_type* pEvent  /* Event Object */
)
{
  bool bError = false;
  CmdType         cmd;
  uint32        token;

  if (!pEvent)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "pEvent == NULL");
    return;
  }

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
            "GenericBcastMediaStatusHandler: status=%s",
            GenericBcastConvertStatusToString(pEvent->eStatus));


  // Process the event.
  switch (pEvent->eStatus)
  {
    case Common::GENERIC_BCAST_STATUS_OPEN_FAILED:
    {
      Notify(QtvPlayer::QTV_COMMAND_OPEN_URN_FAILED);
      break;
    }
    case Common::GENERIC_BCAST_STATUS_OPEN_COMPLETE:
    {
      switch (playerState)
      {
        case OPENING:
        {
          /* Update the timestamp rollover point */
          UpdateMaxTimestamp();

          /* Update the renderer's queuing mode */
          UpdateRendererQueuingMode();

          GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback, true);

          // Set the media Information if available
          if(mediaInfo && mediaInfoSize)
          {
            QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
                          "Sending the stored media Information to source(size = %d)",mediaInfoSize);
            GENERIC_BCAST_MEDIA_SET_MEDIA_INFO(clip.pVideoMpeg4Playback,mediaInfo,mediaInfoSize);
          }

          if(!GenericBcastDescribeClip(clip.pVideoMpeg4Playback))
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL,
                         QTVDIAG_PRIO_ERROR,
                         "GenericBcastMediaStatusHandler: Describe clip Failed");
            return;
          }

          Common::ClockSyncMethodType uClockSyncMethod;
          bool bResult =
            GENERIC_BCAST_MEDIA_GET_CLOCK_SYNC_METHOD(clip.pVideoMpeg4Playback,
                                                      uClockSyncMethod);
          if (bResult &&
              uClockSyncMethod == Common::CLOCK_SYNC_METHOD_ISDBT_PCR)
          {

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#else
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                         "PCR synchronization not enabled");
          }
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

          Notify(PB_READY);

          qtv_event_parser_ready_type qtv_event_parser_ready;
          memset (&qtv_event_parser_ready,0x00,sizeof(qtv_event_parser_ready_type));
#ifndef FEATURE_WINCE
          event_report_payload(EVENT_QTV_PARSER_STATE_READY, sizeof(qtv_event_parser_ready_type), &qtv_event_parser_ready);
#endif   /*    FEATURE_WINCE   */
          if(appControl.bStartPlaying)
          {
            // Pause to Play Transition. Indicate PlayModePlay to Media source
            if(clip.bBcastGeneric )
            {
              #ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
              #endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
              bool bRet;
              bRet = GENERIC_BCAST_MEDIA_PLAY( clip.pVideoMpeg4Playback, UID_QTV_MEDIA_PARAM_PLAY_MODE_PLAY );
              if(!bRet)
              {
                QTV_MSG_PRIO(QTVDIAG_GENERAL,
                             QTVDIAG_PRIO_ERROR,
                             "Generic Bcast Media Play failed");
              }
            }

            if (!StartPlaying())
            {
              bError = true;
              QTV_MSG_PRIO(QTVDIAG_GENERAL,
                           QTVDIAG_PRIO_ERROR,
                           "GBMStatusHandler: Start Playing Error");
            }
            else
            {
#ifndef FEATURE_WINCE
              event_report_payload(EVENT_QTV_CLIP_STARTED,
                                   sizeof(qtv_event_clip_started_type),
                                   &qtv_event_clip_started_payload);
#endif   /*    FEATURE_WINCE   */
            }
          }
          break;
        }

        default:
          bError = true;
          break;
      }
      break;
    }
    case Common::GENERIC_BCAST_STATUS_TRACK_LIST_UPDATE:
    {
        if(clip.pVideoMpeg4Playback)
        {

          // No Indication provided when the track list is same or player is aborted.
          bError = !ActOnNetworkTrackListUpdate();
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL,
                       QTVDIAG_PRIO_ERROR,
                       "GBMStatusHandler: TLU ignored since Media is not playing ");
        }

      break;
    }
    case Common::GENERIC_BCAST_STATUS_REPOSITION_COMPLETE:
    {
      if(CmdFree(pEvent->uTransID,token,cmd) &&
         (cmd == EXTERNAL_REPOSITION_CMD))
      {
        appControl.cbData1 = token;
        appControl.cbData2 = pEvent->uTimestamp;
        Notify(QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_DONE);
        break;
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                    QTVDIAG_PRIO_HIGH,
                    "recd internal reposition cmd ack for %d",
                    pEvent->uTransID);
      break;
    }
    case Common::GENERIC_BCAST_STATUS_SEEK_SYNC_COMPLETE:
    {
      if(CmdFree(pEvent->uTransID,token,cmd))
      {
        appControl.cbData1 = token;
        appControl.cbData2 = pEvent->uTimestamp;
        if(cmd == EXTERNAL_PLAY_SYNC_CMD)
        {
          // Flush the frames before invoking the play next frame.
          // So that the player can fetch new frames.
          if (pActiveVideoPlayer)
          {
            pActiveVideoPlayer->Flush();
          }
          if(PlayNextFrame(NULL))
          {
            Notify(QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE);
          }
          else
          {
            Notify(QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED);
          }
          break;
        }
        if(cmd == EXTERNAL_SEEK_SYNC_CMD)
        {
          Notify(QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_DONE);
          break;
        }
        else if ((appControl.cbData1 == 0) && (cmd == INTERNAL_SEEK_SYNC_CMD))
        {
          /* Play was requested from a predetermined point in the TSB during
          ** startup
          */
          bError = StartPlayingFromSeekedPoint(appControl.cbData2);
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                       "Error!! Unknown seek sync command");
        }
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                    QTVDIAG_PRIO_ERROR,
                    "Error: recd unknown transaction = %d",
                    pEvent->uTransID);
      break;
    }
    case Common::GENERIC_BCAST_STATUS_REPOSITION_FAILED:
    {
      if(CmdFree(pEvent->uTransID,token,cmd) &&
         (cmd == EXTERNAL_REPOSITION_CMD))
      {
        appControl.cbData1 = token;
        appControl.cbData2 = pEvent->uTimestamp;
        Notify(QtvPlayer::QTV_PLAYER_COMMAND_REPOSITION_FAILED);
        break;
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                    QTVDIAG_PRIO_HIGH,
                    "recd internal reposition cmd fail = %d",
                    pEvent->uTransID);
      break;
    }
    case Common::GENERIC_BCAST_STATUS_SEEK_SYNC_FAILED:
    {
      if(CmdFree(pEvent->uTransID,token,cmd))
      {
        appControl.cbData1 = token;
        appControl.cbData2 = pEvent->uTimestamp;
        if(cmd == EXTERNAL_PLAY_SYNC_CMD)
        {
          Notify(QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED);
          break;
        }
        else if(cmd == EXTERNAL_SEEK_SYNC_CMD)
        {
          Notify(QtvPlayer::QTV_PLAYER_COMMAND_SEEK_SYNC_FAILED);
          break;
        }
        else if ((appControl.cbData1 == 0) && (cmd == INTERNAL_SEEK_SYNC_CMD))
        {
          /* Play was requested from a predetermined point in the TSB during
          ** startup, but we failed to find it. So start playing from the
          ** point that TSB returned.
          */
          bError = StartPlayingFromSeekedPoint(appControl.cbData2);
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                       "Error!! Unknown seek sync command");
        }
      }
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                    QTVDIAG_PRIO_ERROR,
                    "Error: recd unknown transaction = %d",
                    pEvent->uTransID);
      break;
    }
    case Common::GENERIC_BCAST_STATUS_CLOSE_COMPLETE:
    {
      /* Do nothing . Need to think of some indication here */
      break;
    }
    case Common::GENERIC_BCAST_STATUS_ABORT:
    {
      GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback,false);
      bError = true;
      break;
    }
    case Common::GENERIC_BCAST_STATUS_PCR_UPDATE:
    {
      QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "PCR Update:  uPcr = %lu, uTts = %lu, bReset = %d",
                    pEvent->uPcr, pEvent->uTts, pEvent->bReset);

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#else
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "PCR synchronization not enabled");
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

      break;
    }
    case Common::GENERIC_BCAST_STATUS_END_OF_TSB:
    {
      QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "END_OF_TSB received" );

      #if defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO)
      /* End of TSB reached, set playback speed to normal, so that we don't
      ** fall out of the live end of of the buffer
      */
      SetPlaybackSpeed(Common::PLAYBACK_SPEED_NORMAL);
      #endif

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
      Notify(QtvPlayer::QTV_PLAYER_STATUS_END_OF_TSB);
      break;
    }
    case Common::GENERIC_BCAST_STATUS_TSB_OVERFLOW:
    {
      QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "TSB_OVERFLOW received" );
      Notify(QtvPlayer::QTV_PLAYER_STATUS_TSB_OVERFLOW);
      break;
    }
    case Common::GENERIC_BCAST_STATUS_TSB_DATA_LOSS:
    {
      QTV_MSG_PRIO( QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "TSB_DATA_LOSS received" );
      Notify(QtvPlayer::QTV_PLAYER_STATUS_TSB_DATA_LOSS);
      break;
    }
    default:
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
               "Error: recd unknown status = %d",
               pEvent->eStatus);
      bError = true;
      ASSERT(0);
      break;
    }
  }

  if (bError)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "GenericBcastMediaStatusHandler: Aborting playback");
    PlaybackErrorAbort();
  }

  return;
} /* GenericBcastMediaStatusHandler() */
#endif /* FEATURE_QTV_GENERIC_BCAST */


/*===========================================================================
FUNCTION
  Mpeg4Player::OpenBcastFLO

DESCRIPTION
  Initializes the broadcast FLO media, and proceed to the Ready state.

RETURN VALUE
  true if successful, else false.

SIDE EFFECTS
  Detail any side effects.

===========================================================================*/
bool Mpeg4Player::OpenBcastFLO()
{
#ifdef FEATURE_QTV_BCAST_FLO
#error code not present
#else
  return false;
#endif /* FEATURE_QTV_BCAST_FLO */

} /* OpenBcastFLO() */




/* ======================================================================
FUNCTION
  Mpeg4Player::OpenLocalFile

DESCRIPTION
//Open the local file and go to Ready state.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::OpenLocalFile()
{
  //error check
  if (playerState!=IDLE && playerState!=OPENING
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
     && playerState!=DOWNLOADING
#endif
     )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in OpenLocalFile");
    return false;
  }

  bool bVideoOK = false;
  bool bAudioOK = false;
  clip.bHasVideo     = false;
  clip.bHasAudio     = false;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  bool bTextOK  = false;
  clip.bHasText      = false;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

//the oscl stuff crashes with a null filename, so check here.
  if (!clip.pVideoLocalFilename && !clip.pAudioLocalFilename
      && !clip.pVideoLocalBuffer && !clip.pAudioLocalBuffer
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      && !clip.FetchBufferedData && !clip.FetchBufferedDataSize
#endif // defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
       && !clip.pTextLocalFilename  && !clip.pTextLocalBuffer
      #endif //FEATURE_MP4_3GPP_TIMED_TEXT
      #ifdef FEATURE_QTV_DRM_DCF
      && !clip.bValid
      #endif
      )
  {
    return false;
  }

  if(clip.pVideoMpeg4Playback == clip.pAudioMpeg4Playback)
  {
    if(clip.pVideoMpeg4Playback)
    {
      QTV_Delete( clip.pVideoMpeg4Playback );
    }
    clip.pVideoMpeg4Playback=NULL;
    clip.pAudioMpeg4Playback=NULL;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    clip.pTextMpeg4Playback=NULL;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  }
  else
  {
  if (clip.pVideoMpeg4Playback)
  {
    QTV_Delete( clip.pVideoMpeg4Playback );
    clip.pVideoMpeg4Playback=NULL;
  }
  if (clip.pAudioMpeg4Playback)
  {
    QTV_Delete( clip.pAudioMpeg4Playback );
    clip.pAudioMpeg4Playback=NULL;
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.pTextMpeg4Playback)
  {
      QTV_Delete( clip.pTextMpeg4Playback );
      clip.pTextMpeg4Playback=NULL;
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  }


  //Parsing takes a while, so go to opening state.
  if (playerState!=OPENING)
  {
    Notify(OPENING);
  }

  //Set local file size
  uint32 nVideoFileSize = 0, nAudioFileSize = 0, nTextFileSize = 0;

  if(clip.pVideoLocalFilename)
  {
    nVideoFileSize = SetLocalFileSize(clip.pVideoLocalFilename);
  }
  else if (clip.pVideoLocalBuffer != NULL)
  {
    nVideoFileSize = clip.videoBufSize;
  }
#ifdef FEATURE_QTV_DRM_DCF
  else if (clip.videoURN->bValid)
  {
    nVideoFileSize = SetLocalFileSize((*clip.videoURN));
  }
#endif

  if(clip.pAudioLocalFilename)
  {
    nAudioFileSize = SetLocalFileSize(clip.pAudioLocalFilename);
  }
  else if (clip.pAudioLocalBuffer != NULL)
  {
    nAudioFileSize = clip.audioBufSize;
  }
#ifdef FEATURE_QTV_DRM_DCF
  else if (clip.audioURN->bValid)
  {
    nAudioFileSize = SetLocalFileSize((*clip.audioURN));
  }
#endif

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(clip.pTextLocalFilename)
  {
    nTextFileSize = SetLocalFileSize(clip.pTextLocalFilename);
  }
  else if (clip.pTextLocalBuffer != NULL)
  {
    nTextFileSize = clip.textBufSize;
  }
#ifdef FEATURE_QTV_DRM_DCF
  else if (clip.textURN->bValid)
  {
    nTextFileSize = SetLocalFileSize((*clip.textURN));
  }
#endif
#endif //FEATURE_MP4_3GPP_TIMED_TEXT

  if (((nVideoFileSize == 0) && (nAudioFileSize == 0) && (nTextFileSize == 0))
      #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      && !clip.FetchBufferedData && !clip.FetchBufferedDataSize
      #endif //defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OpenLocalFile: Video and audio and text file sizes are 0");
    return false;
  }

  // Open and parse the file.
  // Create the stored file media interface.
#ifdef SHOW_PARSE_INFO
  unsigned long t;
#ifndef FEATURE_WINCE
  t = ZUtils::Clock();
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
#endif
#ifdef FEATURE_QTV_DIVX_DRM
  bool b_DrmError = false;
#endif
  /*if(bDownloading)
  {
  }
  else */
  //open local file, or downloaded file that is not currently downloading.
  if(clip.pVideoLocalFilename) /* If this clip has video */
  {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
        clip.pVideoMpeg4Playback = Media::OpenFile( clip.pVideoLocalFilename,
                                                    this,
                                                    true, /* Play video */
                                                    false, /* Do not Play audio */
                                                    false /*Do not play text*/);
#endif /* FEATURE_QTV_MFDRM */

        if( clip.pVideoMpeg4Playback )
        {
          if ( !isVideoParsedSuccessfully(clip.pVideoMpeg4Playback))
          {
            bVideoOK = false;
            #ifdef FEATURE_QTV_DIVX_DRM
             if( (clip.pVideoMpeg4Playback)                                        &&
                 (clip.pVideoMpeg4Playback->GetFile())                             &&
                 (IS_DRM_ERROR(clip.pVideoMpeg4Playback->GetFile()->GetFileError())) )
             {
               b_DrmError = true;
             }
            #endif

            QTV_Delete(clip.pVideoMpeg4Playback);
            clip.pVideoMpeg4Playback = NULL;
          }
          else
          {
            bVideoOK = true;
          }
        }
  }

  if(clip.pAudioLocalFilename) /* If this clip has audio */
  {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
    clip.pAudioMpeg4Playback = Media::OpenFile( clip.pAudioLocalFilename,
                                                    this,
                                                    false,/* Do not play video*/
                                                    true, /* Play audio */
                                                    false /*Do not play text*/);
#endif /* FEATURE_QTV_MFDRM */

    if( clip.pAudioMpeg4Playback )
    {
      /*
      * In case of DivX DRM playback, DRM context is created only in video instance.
      * b_DrmError is set to TRUE video instance returns DRM error code.
      * In such case, we want to error out the playback.
      */
      if ( !isAudioParsedSuccessfully(clip.pAudioMpeg4Playback)
           #ifdef FEATURE_QTV_DIVX_DRM
           || (b_DrmError)
           #endif
         )
      {
        bAudioOK = false;
        QTV_Delete(clip.pAudioMpeg4Playback);
        clip.pAudioMpeg4Playback = NULL;
      }
      else
      {
        bAudioOK = true;
      }
    }
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(clip.pTextLocalFilename) /* If this clip has audio */
  {
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
    clip.pTextMpeg4Playback = Media::OpenFile( clip.pTextLocalFilename,
                                                this,
                                                false,/* Do not play video*/
                                                false, /* Do not play audio */
                                                true  /* Play text */ );
#endif /* FEATURE_QTV_MFDRM */

    if ( clip.pTextMpeg4Playback )
    {
       /*
       * In case of DivX DRM playback, DRM context is created only in video instance.
       * b_DrmError is set to TRUE video instance returns DRM error code.
       * In such case, we want to error out the playback.
       */
       #ifdef FEATURE_QTV_DIVX_DRM
       if(b_DrmError)
       {
         bTextOK = false;
         QTV_Delete(clip.pTextMpeg4Playback);
         clip.pTextMpeg4Playback = NULL;
       }
       else
       #endif
       {
         bTextOK = true;
       }
    }
  }
#endif //FEATURE_MP4_3GPP_TIMED_TEXT

  //open buffer for playback.
  if(clip.pVideoLocalBuffer)
      {
#ifdef FEATURE_QTV_PSEUDO_STREAM
          /*If feature Pseudo Stream is defined
          feature File Fragmentation is also defined*/
          clip.pVideoMpeg4Playback = Media::OpenFile( clip.pVideoLocalBuffer,
                                                      clip.videoBufSize,
                                                      this,
                                                      true,  /*Play video */
                                                      false, /* Do Not Play audio */
                                                      false, /*Do not play text*/
                                                      clip.bPseudoStreaming,
                                                      clip.pseudoStreamWritePtrOffset );
#elif defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || \
      defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)
      if(clip.bDownload)
      {
      #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      clip.pTextMpeg4Playback =
      #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
          clip.pAudioMpeg4Playback =
          clip.pVideoMpeg4Playback = Media::OpenFile( clip.pVideoLocalBuffer,
                   /* Assuming that videoBufSize and  audioBufSize are same
                   or videoBufSize is greater than audioBufSize */
                                                      clip.videoBufSize,
                                                      this,
                                                      true,    /* Play video */
                                                      true,     /* Play audio */
                                                      true     /* Play text */ );
      }
      else
      {
          clip.pVideoMpeg4Playback = Media::OpenFile( clip.pVideoLocalBuffer,
                                                      clip.videoBufSize,
                                                      this,
                                                      true,  /*Play video */
                                                      false, /* Do Not Play audio */
                                                      false, /*Do not play text*/
                                                      clip.bHTTPStreaming,
                                                      clip.HTTPStreamWritePtrOffset,
                                                      http_startupTime );
      }
#else

          clip.pVideoMpeg4Playback = Media::OpenFile( clip.pVideoLocalBuffer,
                   /* Assuming that videoBufSize and  audioBufSize are same
                   or videoBufSize is greater than audioBufSize */
                                                      clip.videoBufSize,
                                                      this,
                                                      true,    /* Play video */
                                                      false, /* Do Not Play audio */
                                                      false /*Do not play text*/ );
#endif /* FEATURE_QTV_PSEUDO_STREAM/FEATURE_QTV_3GPP_PROGRESSIVE_DNLD/FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

    if ( clip.pVideoMpeg4Playback )
    {
       if ( !isVideoParsedSuccessfully(clip.pVideoMpeg4Playback))
       {
          bVideoOK = false;
          QTV_Delete(clip.pVideoMpeg4Playback);
          clip.pVideoMpeg4Playback = NULL;
       }
       else
       {
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
          clip.mediaCounter++;
# endif
          bVideoOK = true;
       }
    }
  }
 /* If audio buffer was provided */
  if(clip.pAudioLocalBuffer)
  {
#ifdef FEATURE_QTV_PSEUDO_STREAM
    /*If feature Pseudo Stream is defined
    feature File Fragmentation is also defined*/

          clip.pAudioMpeg4Playback = Media::OpenFile
                  (clip.pAudioLocalBuffer,
                   clip.audioBufSize,
                   this,
                   false, /* Don't play video */
                   true, /* Play audio */
                   false, /*Do not play text*/
                   clip.bPseudoStreaming,
                   clip.pseudoStreamWritePtrOffset
                  );

#elif defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)|| \
      defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)
      if(clip.bDownload)
      {
        if(clip.pVideoMpeg4Playback==NULL)
        {
        #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
          clip.pTextMpeg4Playback =
        #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
        clip.pAudioMpeg4Playback =
        clip.pVideoMpeg4Playback = Media::OpenFile
                (clip.pVideoLocalBuffer,
                  /* Assuming that videoBufSize and  audioBufSize are same
                  or videoBufSize is greater than audioBufSize */
                  clip.videoBufSize,
                  this,
                  true,    /* Play video */
                  true,     /* Play audio */
                  true /* Play audio */
                 );
        }
      }
      else
      {
          clip.pAudioMpeg4Playback = Media::OpenFile
                  (clip.pAudioLocalBuffer,
                   clip.audioBufSize,
                   this,
                   false, /* Don't play video */
                   true, /* Play audio */
                   false, /*Do not play text*/
                   clip.bHTTPStreaming,
                   clip.HTTPStreamWritePtrOffset,
                   http_startupTime
                  );
      }
#else
      clip.pAudioMpeg4Playback = Media::OpenFile
            (clip.pAudioLocalBuffer,
             clip.audioBufSize,
             this,
             false,    /* Do not play video */
             true, /* Play audio */
             false     /* Do not play audio */
            );
#endif /* FEATURE_QTV_PSEUDO_STREAM/FEATURE_QTV_3GPP_PROGRESSIVE_DNLD/FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

    if ( clip.pAudioMpeg4Playback )
    {
      if (!isAudioParsedSuccessfully(clip.pAudioMpeg4Playback))
      {
        bAudioOK = false;
        QTV_Delete(clip.pAudioMpeg4Playback);
        clip.pAudioMpeg4Playback = NULL;
      }
      else
      {
        bAudioOK = true;
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        clip.mediaCounter++;
#endif
      }
    }
  }

      #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  /* If text buffer was provided */
  if(clip.pTextLocalBuffer)
  {
#ifdef FEATURE_QTV_PSEUDO_STREAM
    /*If feature Pseudo Stream is defined
    feature File Fragmentation is also defined*/
          clip.pTextMpeg4Playback = Media::OpenFile
              (clip.pTextLocalBuffer,
               clip.textBufSize,
               this,
               false, /* Don't play video */
               false, /* Don't play audio */
               true, /* Play text */
               clip.bPseudoStreaming,
               clip.pseudoStreamWritePtrOffset
              );

#elif defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || \
      defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2)
      if(clip.bDownload)
      {
        if(clip.pVideoMpeg4Playback==NULL && clip.pAudioMpeg4Playback==NULL)
        {
        #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
          clip.pTextMpeg4Playback =
        #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
        clip.pAudioMpeg4Playback =
        clip.pVideoMpeg4Playback = Media::OpenFile
                (clip.pTextLocalBuffer,
                 /* Assuming that videoBufSize and  audioBufSize are same
                 or videoBufSize is greater than audioBufSize */
                 clip.textBufSize,
                 this,
                 true,    /* Play video */
                 true,     /* Play audio */
                 true     /* Play text */
                 );
         }
      }
      else
      {
            clip.pTextMpeg4Playback = Media::OpenFile
                  (clip.pTextLocalBuffer,
                   clip.textBufSize,
                   this,
                   false, /* Don't play video */
                   false, /* Don't play audio */
                   true, /* Play audio */
                   clip.bHTTPStreaming,
                   clip.HTTPStreamWritePtrOffset,
                   http_startupTime
                 );

      }
#else
          clip.pTextMpeg4Playback = Media::OpenFile
              (clip.pTextLocalBuffer,
                clip.textBufSize,
                this,
                false,    /* Do not play video */
                false,     /* Do not play audio */
                true     /* Play text */
                  );
#endif /* FEATURE_QTV_PSEUDO_STREAM/FEATURE_QTV_3GPP_PROGRESSIVE_DNLD/FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    if ( clip.pTextMpeg4Playback )
    {
      clip.mediaCounter++;
      bTextOK = true;
    }
#else
    if ( clip.pTextMpeg4Playback )
    {
      bTextOK = true;
    }
#endif
    }
#endif //FEATURE_MP4_3GPP_TIMED_TEXT

#ifdef FEATURE_QTV_DRM_DCF
  /*
  * Open/create Video media instance for supporting
  * playback of video from DCF media.
  *
  */
  if (clip.videoURN->bValid) /* If this clip has video */
  {
    if (clip.videoURN->type == QtvPlayer::URN_INPUTSTREAM && !clip.pVideoMpeg4Playback)
    {

      clip.pVideoMpeg4Playback = Media::OpenFile( clip.videoURN->inputStream,
                                                   clip.videoURN->type,
                                                   this,
                                                   true, /* Play video */
                                                   false, /* Do not Play audio */
                                                   false /*Do not play text*/);
    if( clip.pVideoMpeg4Playback )
    {
      bVideoOK = true;
#if defined (FEATURE_QTV_PSEUDO_STREAM) && defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        if(clip.bPseudoStreaming || clip.bHTTPStreaming)
#elif defined FEATURE_QTV_PSEUDO_STREAM
           if(clip.bPseudoStreaming)
#elif defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
           if( clip.bHTTPStreaming)
        #endif
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
           {
             clip.mediaCounter++;
           }
      #endif
    }
  }
  }

  /*
   * Open/create audio media instance for supporting
   * playback of audio from DCF media.
   *
   */
  if(clip.audioURN->bValid) /* If this clip has audio */
  {
    if (clip.audioURN->type == QtvPlayer::URN_INPUTSTREAM && !clip.pAudioMpeg4Playback)
    {
      clip.pAudioMpeg4Playback = Media::OpenFile( clip.audioURN->inputStream,
                                                  clip.audioURN->type,
                                                  this,
                                                  false, /* Do not Play video */
                                                  true, /* Play audio */
                                                  false /*Do not play text*/);

    if( clip.pAudioMpeg4Playback )
    {
      bAudioOK = true;
#if defined (FEATURE_QTV_PSEUDO_STREAM) && defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
           if(clip.bPseudoStreaming || clip.bHTTPStreaming)
#elif defined FEATURE_QTV_PSEUDO_STREAM
          if(clip.bPseudoStreaming)
#elif defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
          if( clip.bHTTPStreaming)
        #endif
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
          {
            clip.mediaCounter++;
          }
      #endif
    }
  }
  }

  /*
   * Open/create text media instance for supporting
   * playback of text from DCF media.
   *
   */
  #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if (clip.textURN->bValid) /* If this clip has text */
    {
      if (clip.textURN->type == QtvPlayer::URN_INPUTSTREAM && !clip.pTextMpeg4Playback)
      {
        clip.pTextMpeg4Playback = Media::OpenFile( clip.textURN->inputStream,
                                                  clip.textURN->type,
                                                  this,
                                                  false, /* Do not Play video */
                                                  false, /* Do not Play audio */
                                                  true /*Play text*/);

      if( clip.pTextMpeg4Playback )
      {
        bTextOK = true;
#if defined (FEATURE_QTV_PSEUDO_STREAM) && defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
           if(clip.bPseudoStreaming || clip.bHTTPStreaming)
#elif defined FEATURE_QTV_PSEUDO_STREAM
            if(clip.bPseudoStreaming)
#elif defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
            if( clip.bHTTPStreaming)
          #endif
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
            {
              clip.mediaCounter++;
            }
        #endif
      }
    }
    }
  #endif
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/* If it is 3gpp progressive download with pull interface*/
 if(clip.FetchBufferedDataSize)
 {
     httpPullData.bTimerSet = false;
     //Define/Create the timer..
     if(!httpPullData.pHttpPullTimer)
     {
         httpPullData.pHttpPullTimer = rex_create_timer_ex( Mpeg4Player::HttpPullDataInterface,
                                                            (unsigned long)(this));
     }

     /*Create static event QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT to update the write buffer offset.
       We need to create static event because the above timer httpPullData.pHttpPullTimer will end up calling APC
       Mpeg4Player::HttpPullDataInterface. Within this APC we cannot malloc as it can potentially interfere with
       some other APC triggered before Mpeg4Player::HttpPullDataInterface finishes; potentially corrupting the heap.
     */

#if defined FEATURE_QTV_PSEUDO_STREAM
     if(clip.bPseudoStreaming)
     {
      if(!httpPullData.pUpdateWptrOffsetPS)
      {

        httpPullData.pUpdateWptrOffsetPS =
        QCCreateStaticMessage(QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT, this);/*lint!e641 */
        if(!httpPullData.pHttpPullTimer || !httpPullData.pUpdateWptrOffsetPS)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OpenLocalFile: Pull Interface: Can't create timer/static event.");
          return false;
     }
      }
    }
#endif
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    if(clip.bHTTPStreaming)
    {
      if(!httpPullData.pUpdateWptrOffsetPD)
      {
        httpPullData.pUpdateWptrOffsetPD =
          QCCreateStaticMessage(QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT, this);/*lint!e641 */
        if(!httpPullData.pHttpPullTimer || !httpPullData.pUpdateWptrOffsetPD)
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OpenLocalFile: Pull Interface: Can't create timer/static event.");
       return false;
     }
      }
    }
#endif

     httpPullData.FetchBufferedDataSize = clip.FetchBufferedDataSize;
     httpPullData.m_QtvInstanceHandle = clip.m_QtvInstanceHandle;

     if(
        #ifdef FEATURE_QTV_DRM_DCF
     clip.videoURN->bValid &&
     #endif
     !clip.pVideoMpeg4Playback)
     {

#if defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) && defined(FEATURE_QTV_PSEUDO_STREAM)
      if(clip.bHTTPStreaming)
      {
       clip.pVideoMpeg4Playback = Media::OpenFile
                                 (clip.pVideoLocalBuffer,
                                 clip.videoBufSize,
                                 this,
                                 true,  /*Play video */
                                 false, /* Do Not Play audio */
                                 false, /*Do not play text*/
                                 clip.bHTTPStreaming,
                                 clip.HTTPStreamWritePtrOffset,
                                 http_startupTime,
                                 clip.FetchBufferedDataSize,
                                 clip.FetchBufferedData,
                                 clip.m_QtvInstanceHandle);
      }
      else
      {
       clip.pVideoMpeg4Playback = Media::OpenFile
                                 (clip.pVideoLocalBuffer,
                                 clip.videoBufSize,
                                 this,
                                 true,  /*Play video */
                                 false, /* Do Not Play audio */
                                 false, /*Do not play text*/
                                 clip.bPseudoStreaming,
                                 clip.pseudoStreamWritePtrOffset,
                                 http_startupTime,
                                 clip.FetchBufferedDataSize,
                                 clip.FetchBufferedData,
                                 clip.m_QtvInstanceHandle
                                 );
      }
#else
       clip.pVideoMpeg4Playback = Media::OpenFile
                                 (clip.pVideoLocalBuffer,
                                 clip.videoBufSize,
                                 this,
                                 true,  /*Play video */
                                 false, /* Do Not Play audio */
                                 false, /*Do not play text*/
       #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
                                 clip.bHTTPStreaming,
                                 clip.HTTPStreamWritePtrOffset,
                                 http_startupTime,
       #elif defined FEATURE_QTV_PSEUDO_STREAM
                                 clip.bPseudoStreaming,
                                 clip.pseudoStreamWritePtrOffset,
                                 http_startupTime,
       #endif
       #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                                 clip.FetchBufferedDataSize,
                                 clip.FetchBufferedData,
                                 clip.m_QtvInstanceHandle
       #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
                                 );
#endif

        if (clip.pVideoMpeg4Playback)
        {
          if ( !isVideoParsedSuccessfully(clip.pVideoMpeg4Playback))
          {
            bVideoOK = false;
            QTV_Delete(clip.pVideoMpeg4Playback);
            clip.pVideoMpeg4Playback = NULL;
          }
          else
          {
            clip.mediaCounter++;
            bVideoOK = true;
          }
        }
     }
     if(
     #ifdef FEATURE_QTV_DRM_DCF
     clip.audioURN->bValid &&
     #endif
     !clip.pAudioMpeg4Playback)
     {
       clip.pAudioMpeg4Playback = Media::OpenFile
                                 (clip.pAudioLocalBuffer,
                                 clip.audioBufSize,
                                 this,
                                 false, /* Don't play video */
                                 true, /* Play audio */
                                 false, /*Do not play text*/
       #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
                                 clip.bHTTPStreaming,
                                 clip.HTTPStreamWritePtrOffset,
                                 http_startupTime,
       #elif defined FEATURE_QTV_PSEUDO_STREAM
                                 clip.bPseudoStreaming,
                                 clip.pseudoStreamWritePtrOffset,
                                 http_startupTime,
       #endif
       #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                                 clip.FetchBufferedDataSize,
                                 clip.FetchBufferedData,
              clip.m_QtvInstanceHandle
       #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
                                 );

       if (clip.pAudioMpeg4Playback)
       {
         if(!isAudioParsedSuccessfully(clip.pAudioMpeg4Playback))
         {
           bAudioOK = false;
           QTV_Delete(clip.pAudioMpeg4Playback);
           clip.pAudioMpeg4Playback = NULL;
         }
         else
         {
           bAudioOK = true;
           clip.mediaCounter++;
         }
       }
     }
     if(
     #ifdef FEATURE_QTV_DRM_DCF
     clip.textURN->bValid &&
     #endif
     !clip.pTextMpeg4Playback)
     {
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD && defined FEATURE_QTV_PSEUDO_STREAM
       if(clip.bHTTPStreaming)
       {
        clip.pTextMpeg4Playback = Media::OpenFile
                                 (clip.pTextLocalBuffer,
                                 clip.textBufSize,
                                 this,
                                 false, /* Don't play video */
                                 false, /* Don't play audio */
                                 true, /* Play audio */
                                 clip.bHTTPStreaming,
                                 clip.HTTPStreamWritePtrOffset,
                                 http_startupTime,
                                 clip.FetchBufferedDataSize,
                                 clip.FetchBufferedData,
                                 clip.m_QtvInstanceHandle
                                 );
      }
      else
      {
       clip.pTextMpeg4Playback = Media::OpenFile
                                 (clip.pTextLocalBuffer,
                                 clip.textBufSize,
                                 this,
                                 false, /* Don't play video */
                                 false, /* Don't play audio */
                                 true, /* Play audio */
                                 clip.bPseudoStreaming,
                                 clip.pseudoStreamWritePtrOffset,
                                 http_startupTime,
                                 clip.FetchBufferedDataSize,
                                 clip.FetchBufferedData,
                                 clip.m_QtvInstanceHandle
                                 );
      }
#else

       clip.pTextMpeg4Playback = Media::OpenFile
                                 (clip.pTextLocalBuffer,
                                 clip.textBufSize,
                                 this,
                                 false, /* Don't play video */
                                 false, /* Don't play audio */
                                 true, /* Play audio */
       #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
                                 clip.bHTTPStreaming,
                                 clip.HTTPStreamWritePtrOffset,
                                 http_startupTime,
       #elif defined FEATURE_QTV_PSEUDO_STREAM
                                 clip.bPseudoStreaming,
                                 clip.pseudoStreamWritePtrOffset,
                                 http_startupTime,
       #endif
       #if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                                 clip.FetchBufferedDataSize,
                                 clip.FetchBufferedData,
                                 clip.m_QtvInstanceHandle
       #endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
                                 );
#endif
       if ( !clip.pTextMpeg4Playback )
         return false;
       clip.mediaCounter++;
     }
 }

#endif
 if(clip.pAudioMpeg4Playback)
 {
   if(clip.pAudioMpeg4Playback->isGenericAudioFileInstance())
   {
     //create generic AudioMger
#ifndef FEATURE_WINCE
     AudioPlayerIf::initializeAudioplayer(activeAudioPlayerId, AUDIO_ONLY_MIME_TYPE);
#endif   /*    FEATURE_WINCE   */
   }
   else
   {
     //create AudioMger
#ifndef FEATURE_WINCE
     AudioPlayerIf::initializeAudioplayer(activeAudioPlayerId, QTV_PLAYER_MIME_TYPE);
#endif   /*    FEATURE_WINCE   */
   }
 }
 #ifdef FEATURE_QTV_DIVX_DRM
   if(clip.pVideoMpeg4Playback && clip.pVideoMpeg4Playback->isAviFileInstance())
   {
     /*
     * For DRM playback, we only create DRM context in video instance.
     * Since we have separate media instance for audio/video/text,
     * need to setup DRM context in audio/text instances.
     */
     if(clip.pAudioMpeg4Playback)
     {
       clip.pAudioMpeg4Playback->CopyDRMContextInfo((void*)clip.pVideoMpeg4Playback->GetFile());
     }
     if(clip.pTextMpeg4Playback)
     {
       clip.pTextMpeg4Playback->CopyDRMContextInfo((void*)clip.pVideoMpeg4Playback->GetFile());
     }
 }
 #endif

  /* inform media, if they have to loop tracks */
  if(clip.pVideoMpeg4Playback) clip.pVideoMpeg4Playback->SetLoopTrackFlag(bLoopTrackFlag);
  if(clip.pAudioMpeg4Playback) clip.pAudioMpeg4Playback->SetLoopTrackFlag(bLoopTrackFlag);
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(clip.pTextMpeg4Playback) clip.pTextMpeg4Playback->SetLoopTrackFlag(bLoopTrackFlag);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  m_mediaSync.ProhibitAVSync(bLoopTrackFlag);

#ifdef FEATURE_QTV_PSEUDO_STREAM
 if(clip.bPseudoStreaming)
 {
   return true;
 }

#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
 if(clip.bHTTPStreaming)     /* If it is 3gpp progressive download */
 {
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
   /*Here setting the file size because we don't know the file size in case of
   progressive download and generic audio medai instancwe.
   because we are not parsing the file */
   if(clip.pAudioMpeg4Playback)
   {
     if(clip.pAudioMpeg4Playback->isGenericAudioFileInstance())
     {
       clip.pAudioMpeg4Playback->SetFileSize(clipInfo.info.Size);
     }
   }
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
   return true;
 }

#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
 appControl.bPureLocalFile=true;

  /* bAudioError = bVideoError = true; commented no where used */
  if (clip.pVideoMpeg4Playback && bVideoOK)
  {

    if( clip.pVideoMpeg4Playback->CheckError(Common::READMEDIAFILE) )
    {
      // bVideoError = false; not used any where, to solve lint
      bVideoOK = false;
    }

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    if (clip.bDownload)
    {
      (void)clip.pVideoMpeg4Playback->setPDS2DloadObj(pPVDL);
      clip.pVideoMpeg4Playback->SetBufferDuration(DEFAULT_PREROLL_FASTTRACK);

       if(downloadInfo.info.DownloadComplete)
       {
          ((FileMedia *)clip.pVideoMpeg4Playback)->DownloadDone();
       }

    }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


  }
  if (clip.pAudioMpeg4Playback && bAudioOK)
  {
    if( clip.pAudioMpeg4Playback->CheckError(Common::READMEDIAFILE) )
    {
      // bAudioError = false; commented out to solve lint error, no wher else used
      bAudioOK = false;
    }

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    if (clip.bDownload)
    {
      (void)clip.pAudioMpeg4Playback->setPDS2DloadObj(pPVDL);
      clip.pAudioMpeg4Playback->SetBufferDuration(DEFAULT_PREROLL_FASTTRACK);
      if(downloadInfo.info.DownloadComplete)
      {
        ((FileMedia *)clip.pAudioMpeg4Playback)->DownloadDone();
      }
    }
#endif /*  FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.pTextMpeg4Playback && bTextOK)
  {
    if( clip.pTextMpeg4Playback->CheckError(Common::READMEDIAFILE) )
    {
      // bTextError = false; to solve lint error no where else used
      bTextOK = false;
    }
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef SHOW_PARSE_INFO
#ifndef FEATURE_WINCE
  t = ZUtils::Clock()-t;
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Time to parse file %d",t);
#endif

/* If all the three media failed to create then only throw an error */
  if (!bVideoOK  && !bAudioOK
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      && !bTextOK
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
     )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OpenLocalFile: Can't open file");
    return false;
  }

  //Describe
  if (!DescribeClip(  clip.pVideoMpeg4Playback, nVideoFileSize,
                      clip.pAudioMpeg4Playback, nAudioFileSize,
                      clip.pTextMpeg4Playback, nTextFileSize) ||
      !(clip.bHasVideo || clip.bHasAudio
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        || clip.bHasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
       ))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OpenLocalFile: Can't describe clip");
    return false;
  }

  //Select
  if ( clip.bHasVideo && clip.pVideoMpeg4Playback )
  {
    //get width and height
    clipInfo.info.Width = 0;
    clipInfo.info.Height = 0;
    (void)clip.pVideoMpeg4Playback->GetVideoDimensions((uint32*)&clipInfo.info.Width, (uint32*)&clipInfo.info.Height);
    if( (clipInfo.info.Width > MP4_MAX_DECODE_WIDTH) ||
      (clipInfo.info.Height > MP4_MAX_DECODE_WIDTH ) )
    {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED);
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_RESOLUTION_NOT_SUPPORTED H=%ld, W=%ld", clipInfo.info.Height, clipInfo.info.Width);
    }
    clipInfo.info.VideoFrameRate=clip.pVideoMpeg4Playback->GetVideoFrameRate();
    clipInfo.info.VideoEncodedBitRate=clip.pVideoMpeg4Playback->GetVideoBitRate();
  }
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#else
  if ( clip.bHasAudio && clip.pAudioMpeg4Playback )
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
  {
    clipInfo.info.AudioSamplingFreq=clip.pAudioMpeg4Playback->GetAudioSamplingFreq(0);
    clipInfo.info.NumAudioChannels=clip.pAudioMpeg4Playback->GetNumAudioChannels(0);
    clipInfo.info.AudioEncodedBitRate=clip.pAudioMpeg4Playback->GetAudioBitRate(0);
  }

  /* if we have not got movie duration, get in now */
  if(!clipInfo.info.Duration)
  {
    if(clip.bHasVideo && clip.pVideoMpeg4Playback)
      clipInfo.info.Duration = clip.pVideoMpeg4Playback->GetDurationMsec();
    else if(clip.bHasAudio && clip.pAudioMpeg4Playback)
      clipInfo.info.Duration = clip.pAudioMpeg4Playback->GetDurationMsec();
    if(clip.bHasText && clip.pTextMpeg4Playback)
      clipInfo.info.Duration = clip.pTextMpeg4Playback->GetDurationMsec();
  }

  Notify(PB_READY);
#ifdef FEATURE_QTV_AVI
  if(clip.pVideoMpeg4Playback && clip.pAudioMpeg4Playback && clip.pVideoMpeg4Playback->isAviFileInstance())
  {
    clip.pAudioMpeg4Playback->SetIDX1Cache(clip.pVideoMpeg4Playback->GetIDX1Cache());
  }
#endif
#ifdef FEATURE_QTV_DIVX_DRM
  if( clipInfo.info.ExtInfo.DrmInfo.isRental && clipInfo.info.ExtInfoType == QtvPlayer::CLIP_EXT_DIVX_DRM_INFO)
  {
    /*
    * Clip is rental.
    * OEM needs to retrieve extended Clip Info to find out
    * current view count, view limit and any external signal protection, if any.
    * Calling PlayClip once OEM gets following notification means that
    * QTV is authorized to consume one of the views.
    *
    */
    m_bCallCommitPlayback = true;
    Notify(QtvPlayer::QTV_PLAYER_CONSUME_RENTAL_VIEW_CONFIRMATION );
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Reporting QTV_PLAYER_CONSUME_RENTAL_VIEW_CONFIRMATION");
  }
#endif
#ifdef FEATURE_QTV_WM_DRM_API
  if(storedpDRMDecryptFunction)
  {
    clip.pVideoMpeg4Playback->RegisterDRMDecryptMethod(storedpDRMDecryptFunction,storedpClientData);
    clip.pAudioMpeg4Playback->RegisterDRMDecryptMethod(storedpDRMDecryptFunction,storedpClientData);
    clip.pTextMpeg4Playback->RegisterDRMDecryptMethod(storedpDRMDecryptFunction,storedpClientData);
  }
#endif
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::isVideoParsedSuccessfully

DESCRIPTION
  Checks if video instance encountered any errors. If yes, generates
  appropriate notification.

DEPENDENCIES
  none

RETURN VALUE
  True if there are no erros else returns false.

SIDE EFFECTS
  none
========================================================================== */
bool Mpeg4Player::isVideoParsedSuccessfully(Media * mediaPtr)
{
  if(mediaPtr)
  {
    if(mediaPtr->GetFile()!= NULL)
    {
      if( mediaPtr->GetFile()->GetFileError() == TRACK_VIDEO_UNSUPPORTED_BITRATE)
      {
        //Report UNSUPPORTED_AUIO_BITRATE
        Notify(QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_BIT_RATE );
        return false;
      }
      else if( mediaPtr->GetFile()->GetFileError() == TRACK_VIDEO_UNSUPPORTED_PROFILE)
      {
        //Report UNSUPPORTED_PROFILE
        Notify(QtvPlayer::QTV_PLAYER_VIDEO_UNSUPPORTED_PROFILE );
        return false;
      }
      #ifdef FEATURE_QTV_DIVX_DRM
      else if( mediaPtr->GetFile()->GetFileError() == DRM_AUTHORIZATION_ERROR)
      {
        //Report AUTHORIZATION ERROR
        Notify(QtvPlayer::QTV_PLAYER_DRM_AUTHORIZATION_ERROR );
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "QTV_PLAYER_DRM_AUTHORIZATION_ERROR");
        return false;
      }
      else if( mediaPtr->GetFile()->GetFileError() == DRM_ERROR_DEVICE_NOT_REGISTERED)
      {
        //Report DEVICE REGISTRATION ERROR
        Notify(QtvPlayer::QTV_PLAYER_DRM_DEVICE_REGISTRATION_ERROR );
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Reporting QTV_PLAYER_DRM_DEVICE_REGISTRATION_ERROR");
        return false;
      }
      else if( mediaPtr->GetFile()->GetFileError() == DRM_RENTAL_COUNT_EXPIRED)
      {
        //Report RENTAL EXPIRED
        Notify(QtvPlayer::QTV_PLAYER_DRM_RENTAL_COUNT_EXPIRED );
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Reporting QTV_PLAYER_DRM_RENTAL_COUNT_EXPIRED");
        return false;
      }
      else if( ( mediaPtr->GetFile()->GetFileError() == DRM_OUT_OF_MEMORY)||
               ( mediaPtr->GetFile()->GetFileError() == DRM_PLAYBACK_GENERAL_ERROR) )

      {
        //Report GENERAL DRM ERROR
        Notify(QtvPlayer::QTV_PLAYER_DRM_PLAYBACK_GENERAL_ERROR );
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "QTV_PLAYER_DRM_PLAYBACK_GENERAL_ERROR");
        return false;
      }
      #endif//#ifdef FEATURE_QTV_DIVX_DRM
      else if( !mediaPtr->GetFile()->FileSuccess())
      {
        return false;
      }
      return true;
    }
  }
  return false;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::isAudioParsedSuccessfully

DESCRIPTION
  Checks if audio instance encountered any errors. If yes, generates
  appropriate notification.

DEPENDENCIES
  none

RETURN VALUE
  True if there are no erros else returns false.

SIDE EFFECTS
  none
========================================================================== */
bool Mpeg4Player::isAudioParsedSuccessfully(Media * mediaPtr)
{
  if(mediaPtr)
  {
    if(mediaPtr->GetFile()!= NULL)
    {
      if( mediaPtr->GetFile()->GetFileError() == TRACK_AUDIO_UNSUPPORTED_BITRATE)
      {
        //Report UNSUPPORTED_AUIO_BITRATE
        Notify(QtvPlayer::QTV_PLAYER_AUDIO_UNSUPPORTED_BIT_RATE );
        return false;
      }
      else if( !mediaPtr->GetFile()->FileSuccess())
      {
        return false;
      }
      return true;
    }
  }
  return false;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::OpenBitstream

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
bool Mpeg4Player::OpenBitstream()
{
  /*
   int nAudioFilesize, nVideoFilesize, nTextFileSize;
   nAudioFilesize = nVideoFilesize = nTextFileSize = 0;
  not accessed any where, to solve lint */
   if (clip.pVideoBitfile)
   {
    (void)OSCL_FileClose(clip.pVideoBitfile);
      clip.pVideoBitfile = NULL;
   }

   if (clip.pAudioBitfile)
   {
    (void)OSCL_FileClose(clip.pAudioBitfile);
      clip.pAudioBitfile = NULL;
   }
   //Set local file size
 /*
   if (clip.pVideoLocalFilename)
      nVideoFilesize = SetLocalFileSize(clip.pVideoLocalFilename);
   if (clip.pAudioLocalFilename)
      nAudioFilesize = SetLocalFileSize(clip.pAudioLocalFilename);
   nVideoFilesize,nAudioFilesize not used any where, to solve lint */

   if (clip.pVideoLocalFilename)
      clip.pVideoBitfile = OSCL_FileOpen( clip.pVideoLocalFilename, "rb");
   if (clip.pAudioLocalFilename)
      clip.pAudioBitfile = OSCL_FileOpen( clip.pAudioLocalFilename, "rb");

   if (!clip.pVideoBitfile && !clip.pAudioBitfile)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"OpenClip:: Can't open Audio and Video bitfiles");
      return false;
   }

   Notify(PB_READY);
   return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ResetPlaybackData

DESCRIPTION
//clear all playback state data that can get reported to the API.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::ResetPlaybackData()
{
#ifndef FEATURE_WINCE
  AudioPlayerIf::ResetData(activeAudioPlayerId);
  if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->ResetData();
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   textPlayer.ResetData();
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */
}

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
void Mpeg4Player::ResetDownloadData()
//clear all download state data that can get reported to the API.
{
  //clear state data
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: Reset DownloadData");
  downloadState.bytesRemaining=0;
  downloadState.playbackTime=(-1);
  downloadState.fileSize=0;
}
#endif
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
void Mpeg4Player::OpenDownloadSession()
{
  ResetDownloadData();
  DestroyMedia();
  //compute new download ID used for tracking status messages.
  downloadID = (downloadID+1) % 1000;
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
             "PDS2: OpenDownloadSession: downloadID=%d ",
             downloadID);
}

bool Mpeg4Player::SetSessionStart(SESSION_START *p,KEY_DEF *pKey)
{
  if (p)
  {
    memset(p,0,sizeof(SESSION_START));

    p->restart=appControl.bResumeDownload;
    if(p->restart)
    {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   " PDS2: SetSessionStart: resumedownload");
    }
    else
    {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    " PDS2: SetSessionStart: start fresh download");

    }

    p->host[0] = '\0';
    p->port[0] = '\0';

    if (strlen(rootpath)+strlen(DownloadDatFilename)+2>sizeof(p->datfilename))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Filename too long in SetSessionStart");
      return false;
    }
#ifndef FEATURE_WINCE
    (void)std_strlcpy(p->datfilename,rootpath,sizeof(p->datfilename));
    if (p->datfilename[strlen(p->datfilename)-1] != '/')
    {
      (void)std_strlcat(p->datfilename,"/",sizeof(p->datfilename));
    }
#else // #ifndef FEATURE_WINCE
#error code not present
#endif   /*    FEATURE_WINCE   */
#ifndef FEATURE_WINCE
    (void)std_strlcat(p->datfilename,DownloadDatFilename, sizeof(p->datfilename));
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
    QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                           "PDS2: datafilename %s",
                           p->datfilename);

    p->mpegfilename[0] = '\0';
    p->playerVersion[0] = '\0';
    ZUtils::StrcpyN(p->user_agent,"PVPlayer 3.0 PVPlayer/3.1",sizeof(p->user_agent));

    ZUtils::StrcpyN(p->user_network,"Qualcomm",sizeof(p->user_network));

    ZUtils::StrcpyN(p->deviceInfo,"MSM6X00",sizeof(p->deviceInfo));


    if (pKey)
    {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: pKey is valid");



      {
                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: pKey has valid data");
        // Replaced strcpy instances to ZUtils::StrcpyN which is same as strlcpy
        ZUtils::StrcpyN(p->userID,(char *)&pKey->UserID[0],sizeof(p->userID));
        ZUtils::StrcpyN(p->userAuth,(char *)&pKey->SessionID[0],sizeof(p->userAuth));
        ZUtils::StrcpyN(p->expiration,(char *)&pKey->ExpiryDate[0],sizeof(p->expiration));
        ZUtils::StrcpyN(p->appString,(char *)&pKey->ApplicationInfo[0],sizeof(p->appString));
        ZUtils::StrcpyN(p->filler,(char *)&pKey->Filler[0],sizeof(p->filler));
        ZUtils::StrcpyN(p->sign,(char *)&pKey->Signature[0],sizeof(p->sign));
      }
         }

    p->http_timeout=DOWNLOAD_TIMEOUT_SECONDS;
    p->startTime=0;

    p->storageSize= appControl.nStorageSize;
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: storageSize %d",p->storageSize);
    unsigned int flags=appData.bValid ? appData.flags : 0;
    p->dlFlags=(unsigned long)flags;
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: Dload flags %d",p->dlFlags);
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: SetSessionStart returning TRUE");

    char DownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
    getDownloadTxtFileName(DownloadTxtFile, sizeof(DownloadTxtFile));
    if ((char *)(*URN.sURN).IsNull())
    {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
           " SetSessionStart fails, returning..");
        return false;
    }
    if (OSCL_FileExists(DownloadTxtFile))
    {
      OSCL_FILE *fp;
      char c, filename[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
      fp = OSCL_FileOpen(DownloadTxtFile, "r");

      int len;
      //skip leading blanks
      for (len=0; OSCL_FileRead(&c,1,1,fp)==1;)
      {
         if (c>' ')
         {
            filename[len++]=c; break;
         }
      }
      //get chars
      while (OSCL_FileRead(&c,1,1,fp)==1 && c!='\0')
      {
        filename[len++]=c;
      }
      filename[len++]='\0';
      p->IncompleteDownloadFile = (char*) QTV_Malloc(len + 1);
      (void)OSCL_FileClose(fp);
      if(p->IncompleteDownloadFile)
      memcpy(p->IncompleteDownloadFile,filename, (strlen(filename) + 1));
    }
    p->FileSavedtoBuffer = FileSavedToBuffer;
    p->pvxDirPath = nPvxFilePath;
    return true;
  }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"PDS2: SetSessionStart failed, returning FALSE");
  return false;
}

/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::PurgeDownloadConfig()
//purge the download config file if it exists.
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: PurgeDownloadConfig called");
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

bool Mpeg4Player::OpenDownload(KEY_DEF * pKey)
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: OpenDownload called");


  if (clip.pMpeg4Download)
  {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: delete clip.pMpeg4Download");
    QTV_Delete( clip.pMpeg4Download );
    clip.pMpeg4Download=NULL;
  }

    if (pPVDL)
  {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: delete pPVDL");
    QTV_Delete( pPVDL );/*lint !e449 */
    pPVDL = NULL;
  }

  pPVDL = QTV_New( PVDL );
  if(pPVDL)
  {
    pPVDL->setRootPath(rootpath);
  }

  if (!download.Create((void *)this, pPVDL))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::OpenDownload failed");
    return false;
  }

  clip.pMpeg4Download = DownloadMedia::OpenDownload(pPVDL, &qtvConfigObject);

  bool bOK=false;
  if (clip.pMpeg4Download)
  {
    if (!clip.pMpeg4Download->CheckError(Common::READMEDIAFILE))
    {
      bOK=true;
    }
  }
  if (bOK)
  {
    if (NULL != url.sURL)
    {
       if(!download.setPvdlUrl(url.sURL->GetUrlBuffer()))
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "setPvdlUrl Failed");
         return false;
       }
    }

    //Put PVX info into session start.
    if (SetSessionStart(&(clip.pMpeg4Download->sessionStart),pKey))
    {
      //Connect.
      bool bError;
      if (download.StartConnecting(downloadID,bError,&playerTC,clip.pMpeg4Download))
      {
        if (!bError)
        {
          Notify(CONNECTING);
          //Remain in CONNECTING state and wait for download status messages
          return true;
        }
      }
    }
  }

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"OpenDownload returning FALSE");
    return false;
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::StartPlayingOrNotifyReady

DESCRIPTION:
  Thorough, meaningful description of what this function does.

INPUT/OUTPUT PARAMETERS:
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE:
  Enumerate possible return values

SIDE EFFECTS:
  None.
======================================================================*/
bool Mpeg4Player::StartPlayingOrNotifyReady(void)
{
    bool bOK=false;
    if(clip.bDownload)
    {
        if(appData.bValid && (appData.data.DownloadPlaybackControl == QtvPlayer::PLAYBACK_ASAP           ||
                              appData.data.DownloadPlaybackControl == QtvPlayer::PLAYBACK_AFTER_DOWNLOAD ||
                              appControl.bStartPlaying))
        {
          /* Check if it is ok to send PlayClip */
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
          if (qtv_conc_mgr::can_process_command(this, QTV_PLAYER_MIME_TYPE))
#else
       if(true)
#endif
          {
            if (OpenLocalFile())
            {
              SetPlayTimes(appData.startTime,appData.stopTime,false);

              PV_PLAY_CLIP_type *pEvent = QCCreateMessage(PV_PLAY_CLIP, this);

              if (pEvent)
              {
                pEvent->startTime=appData.startTime;
                pEvent->stopTime=appData.stopTime;
                pEvent->switchURN=NULL;
                PostMessage(pEvent);
              }
              else
              {
                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "pEvent creation failed.");
              }

              bOK=true;
            }
          }
          else
          {
            /* Call in progress, cannot issue playclip now, fake success */
            bOK = true;
          }
        }
        else
        {
            if (OpenLocalFile())
            {

              bOK=true;
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "PDS2: Open Local File");
            }
        }

    }

    if (!bOK)
    {
       return false;
    }
    return true;
}

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

/* ======================================================================
FUNCTION
  Mpeg4Player::OpenPlaybackSession

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
void Mpeg4Player::OpenPlaybackSession()
{
   ResetPlaybackData();

   //clear the repositioning history.
   reposControl.nStreamStart=(-1);
   reposControl.nStreamStop=(-1);
   reposControl.bRestart=false;

   //clear local file mode history
   appControl.bPureLocalFile=false;

   //compute new playback ID used for tracking status messages.
   playbackID = (playbackID+1) % 1000;

   if (clip.bStreaming)
   {
      //compute new stream ID used for tracking status messages
      streamID = (streamID+1) % 1000;
   }
}
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
/* ======================================================================
FUNCTION
  Mpeg4Player::OpenHTTPDownloadSession

DESCRIPTION
  Start of Download Session

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::OpenHTTPDownloadSession()
{
  ResetDownloadData();

  //compute new download ID used for tracking status messages.
  downloadID = (downloadID+1) % 1000;
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
             "OpenHTTPDownloadSession: downloadID=%d ",
             downloadID);
}
/* ======================================================================
FUNCTION
  Mpeg4Player::OpenHTTPDownload

DESCRIPTION
  Crates the downloader thread

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::OpenHTTPDownload()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"OpenHTTPDownload called");


  if (!httpdownload.Create((void *)this, httpHeaderManager))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::OpenHTTPDownload failed");
    return false;
  }
  void *storageReference = httpdownload.getStoragereference();
  if (!dataInterface)
  {
     dataInterface = (HttpDataInterface*) QTV_New(HttpDataInterface);
  }
  if (storageReference)
  {
    dataInterface->setStorageReference(storageReference);
  }
  else
  {
    return false;
  }

  //Connect.
  bool bError;
  uint32 downloadOffset = 0;
  char* fileName = NULL;
  // Check for broken Download
  fileName = checkforHttpBrokenDownload(downloadOffset);
  dataInterface->setDownloadOffset(fileName, downloadOffset);
  if (fileName)
  {
    QTV_Free(fileName);
    fileName = NULL;
  }

  int isPDPathSet = std_strlcpy(pdrootpath,
                    (char*)(qtvConfigObject.GetQTVConfigItem(QtvConfig::QTVCONFIG_HTTP_FILE_SAVE_TO_EFS_PATH)),
                     QTV_MAX_FILENAME_BYTES);

  if(isPDPathSet > 0)
  {
    SetPvxSaveOption(pdrootpath);
  } 
  else
  {
// Dont call the SetPvxSaveOption() twice if its already set by MediaPlayer/QTVPlayer.
// Checking if the SetPvxSaveOption() already called by MediaPlayer/QTvPlayer based on the flag
// FileSavedToBuffer. If its 0 means it is set by upper layer already,so we are not calling again.
//default value of FileSavedToBuffer flag is 1 means Heap.

    if(FileSavedToBuffer !=0)
    	{
          SetPvxSaveOption(0);
    	}
  }


  if (nPvxFilePath)
  {
    int32 len=URN.sURN->GetUrlLength()+1;
    char *pUrl = (char*)QTV_Malloc(len);
    if(NULL == pUrl)
    {
      return false;
    }

    (void)std_strlcpy(pUrl,URN.sURN->GetUrlBuffer(),len);
    setDownloadFileName(pUrl);
    if (downloadFileName)
    {
      dataInterface->setDataSaveOption(downloadFileName);
    }
    QTV_Free(pUrl);
  }

  /* Works only for unencrypted content. If the content is encrypted
     and content size check needs to done prior to connecting, OEM
     should support the check */
  if (http_ContentSizeFromSDP != -1)
  {
     dataInterface->setContentSize((uint32) http_ContentSizeFromSDP);
  }
  else if (videoURN.type == QtvPlayer::URN_SDP_FILE)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
    "OpenHTTPDownload: X-filesize attribute missing or unsupported-Proceeding with data call");
  }

  if (httpdownload.StartConnecting(downloadID,bError, (const char*)*url.sURL, downloadOffset))
  {
    if (!bError)
    {
      Notify(CONNECTING);
      //Remain in CONNECTING state and wait for download status messages
      return true;
    }
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"OpenDownload returning FALSE");
  return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::checkforHttpBrokenDownload

DESCRIPTION
  Checks if the current download is broken

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
char* Mpeg4Player::checkforHttpBrokenDownload(uint32 &offset)
{
   char DownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
   getDownloadTxtFileName(DownloadTxtFile, sizeof(DownloadTxtFile));
   if (OSCL_FileExists(DownloadTxtFile))
   {
      OSCL_FILE *fp;
      char c, field[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
      fp = OSCL_FileOpen(DownloadTxtFile, "r");

      uint32 len;
      //skip leading blanks
      for (len=0; OSCL_FileRead(&c,1,1,fp)==1 && len < sizeof(field); )
      {
         if (c>' ')
         {
            field[len++]=c; break;
         }
      }
      //read file name
      while (OSCL_FileRead(&c,1,1,fp)==1 && c!='\0' && len < sizeof(field))
      {
         field[len++]=c;
      }
      if (len == sizeof(field))
      {
         field[len-1]='\0';
   offset = 0;
         return NULL;
      }
      field[len]='\0';
      char* fileName = (char*) QTV_Malloc(len);
      if(fileName)
      {
         memcpy(fileName, field, (strlen(field) + 1));
      }
      else
      {
         (void)OSCL_FileClose(fp);
         QTV_Free(fileName);
         fileName = NULL;
         return 0;
      }
      len = 0;
      while (OSCL_FileRead(&c,1,1,fp)==1 && c!='\0')
      {
         field[len++]=c;
      }
      field[len++]='\0';
      char *endPtr;
      if (!OSCL_FileExists(fileName))
      {
         offset = 0;
   QTV_Free(fileName);
         fileName = NULL;
      }
      else
      {
        offset = (uint32) strtol(field, &endPtr, 10);
      }
      (void)OSCL_FileClose(fp);
      return fileName;
   }
   else
   {
      return NULL;
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::AddOemHttpHeaders

DESCRIPTION
  Function updates the table of HTTP header lines to be
  populated in the GET request.

DEPENDENCIES
  None

RETURN VALUE
  Result of the editing request

SIDE EFFECTS
========================================================================== */
QtvPlayer::OemHttpHeaderResult Mpeg4Player::AddOemHttpHeaders(
       const char *headerName,
       const char *headerValue
)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::AddOemHttpHeaders");

   if (!httpHeaderManager)
   {
      httpHeaderManager = (OemHeaders*) QTV_New(OemHeaders);
      if (!httpHeaderManager)
      {
         return QtvPlayer::OEM_HTTP_RESULT_NO_MEMORY;
      }
   }
   return httpHeaderManager->AddOemHeaders(
         headerName, headerValue);
}
#endif
/* ======================================================================
FUNCTION
  Mpeg4Player::OpenClip

DESCRIPTION
//
// Open the clip, optionally start playing, do state changes, and return true if OK.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::OpenClip()
{
  bool bOK = false;

  /* save loop track value and restore back for local file only */
  bool bLocalLoopTrackFlag = bLoopTrackFlag;
  bLoopTrackFlag = false;
  m_mediaSync.ProhibitAVSync(bLoopTrackFlag);

  KEY_DEF * pKey=NULL;

  /* if any one is going to add any new media stream types then
  they have to take care of the audio player initialization like
  the other initializations that we are doing in OpenLocalFile(),
  OpenBcastISDB, OpenStream etc.. */

  if (clip.bM4vFile)
  {
    // start bitstream file.

    OpenPlaybackSession();

    bOK = OpenBitstream();

    if (bOK && appControl.bStartPlaying)
    {
      bOK=StartPlaying();
    }
  }
  else
  {
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    //Start Mpeg4 file, stream, or download.
    //Get key for secured media.
  if (appData.bValid)
  {
    if (appData.data.Secured)
    {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: Secure download");
      if (pvxIF.bValid)
      {
        //set digital rights for this clip then get the key.
        pvxIF.appFile.SetDigitalRights(pvxIF.nPlaylistIndex);

            }
    }
        else
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2: Non secure download");
        }

        pKey=pvxIF.appFile.GetKey(pvxIF.nPlaylistIndex);
    }
    else
    {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"PDS2: appdata is invalid");
    }

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2  */

    if (clip.bStreaming)
    {
      //Start streaming
      OpenPlaybackSession();
      bOK = OpenStream(pKey);
      //(wait for STREAM_STATUS)
    }

    else if (clip.bBcastFLO)
    {
      OpenPlaybackSession();
      /*---------------------------------------------------------------------
        In contrast to streaming in which case RTSP communication takes
        place before streaming data is received, we need not do any initial
        set-up here.  In this way, we're similar to a local file playback.
      ---------------------------------------------------------------------*/
      bOK = OpenBcastFLO();
      if (bOK && appControl.bStartPlaying)
      {
        bOK = StartPlaying();
      }
    }
#ifdef FEATURE_QTV_GENERIC_BCAST
    else if (clip.bBcastGeneric)
    {
      OpenPlaybackSession();

      /*---------------------------------------------------------------------
        Open the generic broadcast media source
      ---------------------------------------------------------------------*/
      bOK = OpenGenericBcastMediaSource();
      /* since open is async true result will be known through callbacks */
    }
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    else if (clip.bDownload)
  {
     //Start download
      OpenDownloadSession();
    bOK = OpenDownload(pKey);
    //(wait for DOWNLOAD_STATUS)
  }
#endif

#ifdef FEATURE_QTV_PSEUDO_STREAM
    else if (clip.bPseudoStreaming)
    {
      OpenPlaybackSession();
      bOK = OpenLocalFile();
    }
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    else if (clip.bHTTPStreaming)
    {
      OpenPlaybackSession();
      bOK = OpenLocalFile();
    }
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
    else if(clip.bQtvHttpStreaming)
    {
       OpenHTTPDownloadSession();
       bOK = OpenHTTPDownload();
    }
#endif
    // InputStream playback is the same as local file playback
    else
    {
      /* restore back for local file only */
      bLoopTrackFlag = bLocalLoopTrackFlag;
      //Start local file
      OpenPlaybackSession();
      bOK = OpenLocalFile();
      if (bOK && appControl.bStartPlaying)
      {
        bOK = StartPlaying();
      }
    }
  }

  /* Intitialize the function pointer in media for processing Media Layer
     request */
  if(bOK && clip.pVideoMpeg4Playback)
  {
    clip.pVideoMpeg4Playback->SetMediaPlayerCb(&Mpeg4Player::MediaNotifyCB);
  }

  /* if both are not equal, means it is not local file playback.
     notify user that we can only loop for local media playback */
  if( bOK && (bLocalLoopTrackFlag != bLoopTrackFlag) )
  {
    Notify(QtvPlayer::QTV_PLAYER_LOOP_TRACK_NOT_SUPPORTED);
  }

  return bOK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::OpenStream

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
bool Mpeg4Player::OpenStream(KEY_DEF * pKey)
{
  bool bOK = false;
  bool bMp4Error=false;
  bool status =false;
  /* other than local file playback, all other playbacks are through video file only */
  if (clip.pVideoMpeg4Playback)
  {
    QTV_Delete( clip.pVideoMpeg4Playback );
    clip.pVideoMpeg4Playback = NULL;
  }

  //create AudioMgr
#ifndef FEATURE_WINCE
  AudioPlayerIf::initializeAudioplayer(activeAudioPlayerId, QTV_PLAYER_MIME_TYPE);
#endif   /*    FEATURE_WINCE   */

#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
  m_useMAPILogic = false;

#ifdef FEATURE_QTV_MCAST
  if ((sdpIF.bValid) &&
      (sdpIF.pSDPInfo != NULL) &&
      (sdpIF.pSDPInfo->getSessionDescription() != NULL) &&
      (sdpIF.pSDPInfo->getSessionDescription()->isMcast()))
  {
    // use MAPI path if Multicast
    m_useMAPILogic = true;
  }
#endif /* FEATURE_QTV_MCAST */

  // create Streamer/CStreamer_v2 based on Unicast/Multicast respectively
  if (!streamer)
  {
    if (m_useMAPILogic)
    {
      streamer = QTV_New(CStreamer_v2);
    }
    else
    {
      streamer = QTV_New_Args(Streamer,(NULL, true));
    }
  }

  bOK = (streamer != NULL);
  if (m_useMAPILogic && bOK)
  {
    // create StreamMedia_v2
    clip.pAudioMpeg4Playback = NULL;
    clip.pTextMpeg4Playback = NULL;
    clip.pVideoMpeg4Playback = QTV_New_Args(CStreamMedia_v2, ((void*)this));
    bOK &= (clip.pVideoMpeg4Playback != NULL);
    if (bOK)
    {
      (void)((CStreamer_v2*)streamer)->SetMediaSource(
                                       (CStreamMedia_v2*)clip.pVideoMpeg4Playback);

#ifdef FEATURE_QTV_NET_POLICY
    /* For StreamMAPI The call in Mpeg4Player::SetNetPolicy() fails at that point
     * as source media wont be initalized,Set it right here after
     * stream media initialization */
    if (netPolicyInfo.isSet)
    {
      (void)((CStreamMedia_v2*)clip.pVideoMpeg4Playback)->SetNetPolicyInfo(&netPolicyInfo);
    }
#endif /* FEATURE_QTV_NET_POLICY */

       // Set userAgentInfo
       UserAgentInfo userAgentInfo;
       userAgentInfo.userAgentName = userAgent;
       if (userAgentParamsType == QTV_SKT_STREAMING_EXTENSIONS)
       {
         userAgentInfo.userAgentParams = (char*) userAgentParams.get_cstr();
       }
       else if (userAgentParamsType == QTV_LGT_STREAMING_EXTENSIONS)
       {
         // construct params string for LGT extension
         char tmpBuffer[QTV_MAX_USER_AGENT_STR_BYTES];
         int len = std_strlprintf(tmpBuffer, QTV_MAX_USER_AGENT_STR_BYTES,
                                  "MAN=%s,CTN=%s,VCT=%s,ACT=%s,SCT=%s,CLR=%s,LCD=%s,VER=%s",
                                  userAgentMAN,userAgentCTN,userAgentVCT,userAgentACT,
                                  userAgentSCT,userAgentCLR,userAgentLCD,userAgentVER);
         if (len <= 0)
         {
           tmpBuffer[0] = '\0';
         }
         userAgentInfo.userAgentParams = tmpBuffer;
       }
       (void)((CStreamMedia_v2*)clip.pVideoMpeg4Playback)->setUserAgentInfo(userAgentInfo);

       // Prepend "file://" if the URN is not a RTSP URL and it ends with ".sdp".
       OSCL_STRING urnBuffer = videoURN.sURN->GetUrlBuffer();

       if ((!videoURN.sURN->StartsWith("rtsp://") ||
              !videoURN.sURN->StartsWith("rtspt://"))&&
           videoURN.sURN->EndsWith(".sdp"))
       {
         urnBuffer = "file://";
         urnBuffer += videoURN.sURN->GetUrlBuffer();
       }

       // Open stream media
       if (!GENERIC_BCAST_MEDIA_OPEN(clip.pVideoMpeg4Playback,
                                    (char *) urnBuffer.get_cstr()))
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL,
                      QTVDIAG_PRIO_ERROR,
                      "Failed to queue the Stream Media Open Request");
         bOK = false;
       }
    } //bOK
  } //m_useMAPILogic && bOK
  else if (bOK)
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */
  {
     // We need to have called SetURN( ) before we can initialize the
     // Streaming Library in the case of SDP support, since we require
     // the SDPInfo (either from the file or server)

     // checking for pQtvStream = NULL added to fix qcom memory leak
     // added bNeedStreamer so that we know if we need to create the streaming
     // interface or not.
     // added checks for successful creation of streaming lib cuz it's a little
     // more robust.  Have never seen a problem though. - jg
     if ( sdpIF.bValid )
     {
        if ( pQtvStream == NULL )
        {
           if ((pQtvStream = QTV_New_Args( QtvStream, ( &qtvConfigObject, &status, NULL,
                                                        sdpIF.pSDPInfo, false) )) == NULL)
           {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "QtvStream constructor failed" );
             return false;
           }
           // checking for successful creation of other object like pnb,pmb,pni
           if (!status)
           {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for QtvStream objects failed" );
             return false;
           }
        }
     }
     else
     {
        if ( pQtvStream == NULL )
        {
           if ((pQtvStream = QTV_New_Args( QtvStream,(&qtvConfigObject,&status ))) == NULL )
           {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "QtvStream constructor failed" );
             return false;
           }
           // checking for successful creation of other object like pnb,pmb,pni
           if (!status)
           {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for QtvStream objects failed" );
             return false;
           }
        }
     }

     if( streamer )
     {
        if ( !streamer->Create((void *)this, pQtvStream, &qtvConfigObject))
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "streamer.Create failed");
           return false;
        }
     }
     else
     {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "streamer.object NULL");
        return false;
     } /* if( streamer ) */

     // Create the streaming media interface
     if (pQtvStream)
     {
        // other than local file playback, all other playbacks are through video file only
        clip.pVideoMpeg4Playback = StreamMedia::OpenStream((void*) this, pQtvStream, &qtvConfigObject);
        if (clip.pVideoMpeg4Playback)
        {
           if (clip.pVideoMpeg4Playback->CheckError(Common::READMEDIAFILE)) bMp4Error=true;
           bOK = (!bMp4Error);
        }
     }
     if (!bOK)
     {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OpenClip:: Can't open stream");
     }
     else if (url.bValid)
     {
        bOK=false;

        if (url.type == QtvPlayer::URL_STREAM)
        {
           //For a streaming clip, start connecting and then wait for connection.
           bool bError = true;
           if (streamer->StartConnecting(streamID,
                                        (StreamMedia *) clip.pVideoMpeg4Playback,
                                         *url.sURL,
                                          pKey,
                                          bError))
           {
              if (bError)
              {
                 QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from StartConnecting");
                 StreamError();
              }
              else
              {
                 bOK=true;
                 Notify(CONNECTING);
                 //Remain in CONNECTING state and wait for stream status messages.
              }
           }
        }
     } /* else if (url.bValid) */
  } /* else if (bOK) */

  if (!bOK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::OpenStream failed");
  }

  return bOK;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::DescribeClip

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
bool Mpeg4Player::DescribeClip( Media *pVideo, int VideoFileSize,
                                Media *pAudio, int AudioFileSize,
                                Media *pText, int TextFileSize )
{
  // init the codec type field if the paylaod
  qtv_event_clip_started_payload.audio_codec = (uint8) Media::UNKNOWN_CODEC;
  qtv_event_clip_started_payload.video_codec = (uint8) Media::UNKNOWN_CODEC;

  selectedAudioTrackIdx = -1;
  selectedVideoTrackIdx = -1;
  selectedTextTrackIdx = -1;
  clip.bHasEnhancedVideo = false;

  // Describe and do track selection algorithm.
  bool bVideoMediaAudioSelected = false;
  bool bVideoMediaVideoSelected = false;
  bool bVideoMediaTextSelected  = false;

  bool bAudioMediaAudioSelected = false;
  bool bAudioMediaVideoSelected = false;
  bool bAudioMediaTextSelected  = false;

  bool bTextMediaAudioSelected = false;
  bool bTextMediaVideoSelected = false;
  bool bTextMediaTextSelected  = false;

  bool bVideoSelected = false;
  bool bAudioSelected = false;
  bool bTextSelected  = false;

  bool bVideoErr      = true;
  bool bAudioErr      = true;
  bool bTextErr       = true;

  /*If there is any unknown codec we have to notify
   that. So this flag is set to false initially*/
  bNotifyUnknownCodec = false;

  if (pVideo)
  {
    bVideoErr = !DescribeAndSelectTracks(pVideo, bVideoMediaAudioSelected, bVideoMediaVideoSelected, bVideoMediaTextSelected);
  }

  /*Since function DescribeAndSelectTracks we loop for
    all the tracks so we dont need any duplication of
    notification during further calls to this function*/
  bNotifyUnknownCodec = true;

  if (pAudio)
  {
    bAudioErr = !DescribeAndSelectTracks(pAudio, bAudioMediaAudioSelected, bAudioMediaVideoSelected, bAudioMediaTextSelected);
  }

  if (pText)
  {
    bTextErr = !DescribeAndSelectTracks(pText, bTextMediaAudioSelected, bTextMediaVideoSelected, bTextMediaTextSelected);
  }

  if (!bVideoErr || !bAudioErr || !bTextErr)
  {
    // If possible, set clip info based on Video clip
    SetClipInfo(pVideo, VideoFileSize, pAudio, AudioFileSize, pText, TextFileSize);

    if (  (videoURN.type==QtvPlayer::URN_LOCAL_FILE) || (audioURN.type==QtvPlayer::URN_LOCAL_FILE) ||
          (videoURN.type==QtvPlayer::URN_LOCAL_BUFFER) || (audioURN.type==QtvPlayer::URN_LOCAL_BUFFER) ||
          (textURN.type==QtvPlayer::URN_LOCAL_FILE) || (textURN.type==QtvPlayer::URN_LOCAL_BUFFER)
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
          || ((videoURN.type==QtvPlayer::URN_PVX_FILE) && clip.bDownload)
#endif

#ifdef FEATURE_QTV_PSEUDO_STREAM
          || (videoURN.type==QtvPlayer::URN_PSEUDO_STREAM) || (audioURN.type==QtvPlayer::URN_PSEUDO_STREAM)
          || (textURN.type==QtvPlayer::URN_PSEUDO_STREAM)
#endif /*FEATURE_QTV_PSEUDO_STREAM */
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
          || (videoURN.type==QtvPlayer::URN_HTTP_STREAM) || (audioURN.type==QtvPlayer::URN_HTTP_STREAM)
          || (textURN.type==QtvPlayer::URN_HTTP_STREAM)
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_DRM_DCF
      || (videoURN.type == QtvPlayer::URN_INPUTSTREAM)
      || (audioURN.type == QtvPlayer::URN_INPUTSTREAM)
      || (textURN.type == QtvPlayer::URN_INPUTSTREAM)
#endif
       )
    {
      bVideoSelected = bVideoMediaVideoSelected;
      bAudioSelected = bAudioMediaAudioSelected;
      bTextSelected  = bTextMediaTextSelected;
    }
    else
    {
      bVideoSelected = bVideoMediaVideoSelected;
      bAudioSelected = bVideoMediaAudioSelected;
      bTextSelected  = bVideoMediaTextSelected;
    }

    // Disable audio if it's WMA on LTK, since the LTK doesn't support it yet.
//#if defined(PLATFORM_LTK) && defined(FEATURE_QTV_WINDOWS_MEDIA)
#if 0
    for (int i = 0; i < clipInfo.info.TrackCount; i++)
    {
       if (QtvPlayer::TRACK_WM_AUDIO == clipInfo.info.TrackTypes[i])
       {
          bAudioSelected = false;
       }
    }
#endif /* PLATFORM_LTK && FEATURE_QTV_WINDOWS_MEDIA */

    // Select AV components.
    SetClipComponents(bAudioSelected, bVideoSelected, bTextSelected);

#ifdef SHOW_INFO
    ShowClip();
#endif /* SHOW_INFO */

#ifdef FEATURE_QTV_SKT_MOD_MIDI
    if( (pVideo && pVideo->IsMidiDataPresent()) ||
        (pAudio && pAudio->IsMidiDataPresent()) ||
        (pText && pText->IsMidiDataPresent()))
    {
      Notify(QtvPlayer::QTV_MIDI_DATA_PRESENT);
    }
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

    return true;
  }

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
    "Mpeg4Player::DescribeClip failed");

  return false;
}

/*===========================================================================
FUNCTION
  Mpeg4Player::LocalMediaPLaybackUpgradeTracks

DESCRIPTION
  Player is updated with new track list parameters selected.
  This function is for local file playback.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
bool Mpeg4Player::LocalMediaPLaybackUpgradeTracks(unsigned int nResult)
{

  // Update the state information in the Mpeg4Player class.
  if(nResult != QTV_LOCAL_MEDIA_TRACK_LIST_IDENTICAL)
  {
    UpdateLocalClipInfo(clip.pAudioMpeg4Playback,nResult);
    // Issue Track List Update when the track IDs are modified also.
    Notify(QtvPlayer::QTV_PLAYER_STATUS_TRACK_LIST_UPDATE);
  }
  return true;
}



/* ======================================================================
FUNCTION
  Mpeg4Player::UpdateLocalClipInfo

DESCRIPTION
 This updates  the clip info in case local file playback

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::UpdateLocalClipInfo(Media * pMedia,unsigned int nResult)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::UpdateLocalClipInfo" );

  uint32 numTracks=0;                  /* Number of Tracks          */
  int i;                               /* For loop index            */
  QTV_USE_ARG1(nResult);

  ReferenceCountedPointer<ITrackList> trackList;

  if ((pMedia && !pMedia->GetTrackList(MakeOutPointer(trackList))) ||
     (trackList == NULL))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Invalid Track List ; Quitting ");
    ASSERT(0);
    return;
  }

  numTracks = trackList->GetNumTracksAvailable();

  for (int i = 0; i < numTracks; i++)
  {
    if (!trackList->IsTrackSelected(i))
    {
      continue;
    }
    Media::CodecType codecType = trackList->GetCodecType(i);
    if (Media::IsAudioCodec(codecType))
    {
      if (selectedAudioTrackIdx < 0)
      {
        selectedAudioTrackIdx = i;
      }
    } // if (isAudioCodec)
    if (Media::IsVideoCodec(codecType))
    {
      if (selectedVideoTrackIdx < 0)
      {
        selectedVideoTrackIdx = i;
      }
    }
    if (Media::IsTextCodec(codecType))
    {
      if (selectedTextTrackIdx < 0)
      {
        selectedTextTrackIdx = i;
      }
    }

  } // for (int i = 0; i < numTracks; i++)


  QCUtils::EnterCritSect(&clipInfo_CS);
  clipInfo.info.TrackCount = numTracks;
  for(i = 0; i < numTracks; i++)
  {
    clipInfo.info.TrackTypes[i] = MapMediaCodecToQtvPlayerCodec(
    trackList->GetCodecType(i));
  }
  if (selectedAudioTrackIdx >= 0)
  {
    // get audio codec for the qtv_event_clip_started_type payload
    qtv_event_clip_started_payload.audio_codec =
      (uint8) trackList->GetCodecType(selectedAudioTrackIdx);
  }
  clipInfo.info.AudioSamplingFreq = pMedia->GetAudioSamplingFreq();
  clipInfo.info.NumAudioChannels  = pMedia->GetNumAudioChannels();
  QCUtils::LeaveCritSect(&clipInfo_CS);

#ifdef SHOW_INFO
  ShowClip();
#endif /* SHOW_INFO */
  return;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION
  Mpeg4Player::GenericBcastDescribeTracks

DESCRIPTION
  QTV usually select the tracks once the media described it. But for
  generic broadcast media tracks come selected. Hence the mpeg4 player
  invoke the describe and pick the selected track indices from the list.

PARAMETERS
  pVideo(I) - Broadcast Media object pointer

RETURN VALUE
  Returns TRUE/FALSE depending on how the function executed.

SIDE EFFECTS
  None.
========================================================================== */
bool Mpeg4Player::GenericBcastDescribeTracks(Media  *pVideo)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL,
               QTVDIAG_PRIO_LOW,
               "Mpeg4Player::GenericBcastDescribeTracks" );

  bool bAudioSelected=false;           /* Audio Selection boolean   */
  bool bVideoSelected=false;           /* Video Selection boolean   */
  bool bTextSelected=false;            /* Text Selection boolean    */
  ReferenceCountedPointer<ITrackList> trackList; /* Track List       */
  int32 i;                               /* For loop index            */

  // init the codec type field of the paylaod
  qtv_event_clip_started_payload.audio_codec =
  qtv_event_clip_started_payload.video_codec =
    (uint8)Media::UNKNOWN_CODEC;

  //Get track information.
  if (!pVideo->GetTrackList(MakeOutPointer(trackList)) ||
      (trackList == NULL))
  {
    QTV_MSG(QTVDIAG_GENERAL,
            "Mpeg4Player::Describe tracklist = NULL");

    return false;
  }

  // Loop over the tracks and initialize player attributes
  for (i = 0; i < trackList->GetNumTracksAvailable(); i++)
  {
    Media::CodecType codecType = trackList->GetCodecType(i);

    //Check whether player can play this track.
    if (!trackList->IsPlayable(i))
    {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_UNSUPPORTED_CODEC);
      continue;
    }
    //Check if the track is not chosen
    if(!trackList->IsTrackSelected(i))
    {
      continue;
    }
    // If the track is chosen then check for media type
    // update the relevant player attributes.
    if(Media::IsAudioCodec(codecType))
        {
          if(!bAudioSelected)
          {
            // get audio codec for the qtv_event_clip_started_type payload
        qtv_event_clip_started_payload.audio_codec = (uint8) codecType;
            selectedAudioTrackIdx = i;
            bAudioSelected = true;

      }
      else if (trackList->GetDependsOnID(i) == trackList->GetTrackID(i))
      {
        // This is the case when base layer appears later
        selectedAudioTrackIdx = i;
          }
        }
    else if (Media::IsVideoCodec(codecType))
        {
          if(!bVideoSelected)
          {
            // get video codec for the qtv_event_clip_started_type payload
        qtv_event_clip_started_payload.video_codec = (uint8) codecType;
            selectedVideoTrackIdx = i;
            bVideoSelected = true;
          }
      else if (trackList->GetDependsOnID(i) == trackList->GetTrackID(i))
      {
        // This is the case when base layer appears later
        selectedVideoTrackIdx = i;
      }
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
      if( (trackList->GetDependsOnID(i) != trackList->GetTrackID(i)) &&
          (codecType == qtv_event_clip_started_payload.video_codec))
           {
             //Select enhancement track.
             clip.bHasEnhancedVideo = true;
             QTV_MSG_PRIO(QTVDIAG_GENERAL,
                          QTVDIAG_PRIO_HIGH,
                          "Clip has enhanced video layer.");
             Notify(QtvPlayer::QTV_PLAYER_TS_LAYER_PRESENT);
           }
#endif
        }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    else if(GenericBcastMedia::IsTextCodec(codecType))
        {
          // Always select timed text track if present
          if (!bTextSelected)
          {
            bTextSelected = true;
            selectedTextTrackIdx = i;
          }
      else if (trackList->GetDependsOnID(i) == trackList->GetTrackID(i))
      {
        // This is the case when base layer appears later
        selectedTextTrackIdx = i;
      }
        }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  }

  // If possible, set clip info based on Video clip
  SetClipInfo(pVideo, 0, NULL, 0, NULL, 0);

  // Select AV components.
  SetClipComponents(bAudioSelected, bVideoSelected, bTextSelected);

#ifdef SHOW_INFO
  ShowClip();
#endif /* SHOW_INFO */

  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GenericBcastDescribeClip

DESCRIPTION
  Generic Broadcast media source selects the tracks by default. Hence
  this routine written to avoid default track selection logic present
  within the QTV player.

PARAMETERS
  pVideo(I) - generic broadcast media object pointer

RETURN VALUE
  Returns TRUE/FALSE depending on the result.

SIDE EFFECTS
  None.
========================================================================== */
bool Mpeg4Player::GenericBcastDescribeClip( Media *pVideo )
{
  ReferenceCountedPointer<ITrackList> trackList;

  // Validate the input parameters first
  if (!pVideo)
  {
    QTV_MSG(QTVDIAG_GENERAL,
            "Mpeg4Player::GenericBcastDescribeClip pMedia = NULL");
    return false;
  }

  //Let the media source describe or enumerate the track list.

  if (!pVideo->GetSelectedTrackList(MakeOutPointer(trackList)) ||
      (trackList == NULL))
  {
    QTV_MSG(QTVDIAG_GENERAL,
            "Mpeg4Player::GenericBcastDescribeClip tracklist = NULL");
    return false;
  }

  // Describe the chosen tracks and initialize/update the clip info
  if(!GenericBcastDescribeTracks(pVideo))
  {
    QTV_MSG(QTVDIAG_GENERAL,
            "Mpeg4Player::GenericBcastDescribeClip Describe Tracks failed");
    return false;
  }

  /*-----------------------------------------------------------------------
    Get video width and height
  -----------------------------------------------------------------------*/
  if (clip.bHasVideo)
  {
    clipInfo.info.Width = 0;
    clipInfo.info.Height = 0;
    clip.pVideoMpeg4Playback->GetVideoDimensions( (uint32*)&clipInfo.info.Width,
                                                  (uint32*)&clipInfo.info.Height);
    if( (clipInfo.info.Width > MP4_MAX_DECODE_WIDTH) ||
        (clipInfo.info.Height > MP4_MAX_DECODE_WIDTH ) )
    {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED);
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "VIDEO_RESOLUTION_NOT_SUPPORTED H=%ld, W=%ld",
                    clipInfo.info.Height, clipInfo.info.Width);
    }
  }


  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::UpdateClipInfo

DESCRIPTION
//
//  This updates  the clip info
//

DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::UpdateClipInfo(Media * pMedia,unsigned int nResult)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::SetClipInfo" );

  bool bAudioSelected=false;           /* Audio Selection boolean   */
  bool bVideoSelected=false;           /* Video Selection boolean   */
  bool bTextSelected=false;            /* Text Selection boolean    */
  uint32 numTracks=0;                  /* Number of Tracks          */
  int i;                               /* For loop index            */
  int numDepVideoLayers = 0;  /* Number of dependent video layers */

  ReferenceCountedPointer<ITrackList> trackList;

  if (!pMedia->GetTrackList(MakeOutPointer(trackList)) ||
     (trackList == NULL))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Invalid Track List ; Quitting ");
    ASSERT(0);
    return;
  }

  numTracks = trackList->GetNumTracksAvailable();

  // init the codec type field of the paylaod
  qtv_event_clip_started_payload.audio_codec =
  qtv_event_clip_started_payload.video_codec = (uint8)Media::UNKNOWN_CODEC;

  // Update the track indices any way
  GENERIC_BCAST_MEDIA_GET_SELECTED_TRACKS(pMedia, selectedAudioTrackIdx, selectedVideoTrackIdx,
                                          selectedTextTrackIdx, numDepVideoLayers);

  if(selectedAudioTrackIdx >= 0)
  {
    if((nResult & QTV_MEDIA_TRACK_LIST_AUDIO_ADDED) &&
       (!trackList->IsPlayable(selectedAudioTrackIdx)))
    {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_UNSUPPORTED_CODEC);
      selectedAudioTrackIdx = -1;
    }
    else
    {
      // get audio codec for the qtv_event_clip_started_type payload
      qtv_event_clip_started_payload.audio_codec =
      (uint8)trackList->GetCodecType(selectedAudioTrackIdx);
      bAudioSelected = true;
    }
  }
  if(selectedVideoTrackIdx >= 0)
  {
    if((nResult & QTV_MEDIA_TRACK_LIST_VIDEO_ADDED) &&
       (!trackList->IsPlayable(selectedVideoTrackIdx)))
    {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_UNSUPPORTED_CODEC);
      selectedVideoTrackIdx = -1;
    }
    else
    {
      // get audio codec for the qtv_event_clip_started_type payload
      qtv_event_clip_started_payload.video_codec =
      (uint8)trackList->GetCodecType(selectedVideoTrackIdx);
      bVideoSelected = true;
#ifdef FEATURE_MP4_TEMPORAL_SCALABILITY
      if(numDepVideoLayers > 1)
      {
        //Select enhancement track.
        clip.bHasEnhancedVideo = true;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                     "Clip has enhanced video layer.");
        Notify(QtvPlayer::QTV_PLAYER_TS_LAYER_PRESENT);
      }
#endif
    }
  }
  if(selectedTextTrackIdx >= 0)
  {
    if((nResult & QTV_MEDIA_TRACK_LIST_TEXT_ADDED) &&
       (!trackList->IsPlayable(selectedTextTrackIdx)))
    {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_UNSUPPORTED_CODEC);
      selectedTextTrackIdx = -1;
    }
    else
    {

      bTextSelected = true;
    }
  }


  QCUtils::EnterCritSect(&clipInfo_CS);

  clipInfo.info.TrackCount = numTracks;
  for(i = 0; i < numTracks; i++)
  {
    clipInfo.info.TrackTypes[i] = MapMediaCodecToQtvPlayerCodec(
      trackList->GetCodecType(i));

  }
  clipInfo.info.MediaType = UpgradeQtvMediaType(pMedia, nResult, clipInfo.info.MediaType);
  clipInfo.info.AudioSamplingFreq = pMedia->GetAudioSamplingFreq();
  clipInfo.info.NumAudioChannels  = pMedia->GetNumAudioChannels();
  pMedia->GetVideoDimensions((uint32 *)&clipInfo.info.Width,(uint32 *)&clipInfo.info.Height);

  QCUtils::LeaveCritSect(&clipInfo_CS);

  // Select AV components.
  SetClipComponents(bAudioSelected, bVideoSelected, bTextSelected);

  // Raise the clip started event after track list updates.
  QtvPlayer::PlayerStateRecordT ps;
  (void)QtvPlayer::GetPlayerState(ps);
  qtv_event_clip_started_payload.start_time = ps.playbackMsec;
  qtv_event_clip_started_payload.num_layers = numDepVideoLayers;
#ifndef FEATURE_WINCE

  event_report_payload(EVENT_QTV_CLIP_STARTED,
                       sizeof(qtv_event_clip_started_type),
                       &qtv_event_clip_started_payload);
#endif   /*    FEATURE_WINCE   */

#ifdef SHOW_INFO
  ShowClip();
#endif /* SHOW_INFO */
  return;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::UpgradeQtvMediaType

DESCRIPTION
Updates the media type as per addition of tracks due to track list update

DEPENDENCIES
  None.

RETURN VALUE
  Returns the updated media Type.

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::MediaTypeT Mpeg4Player::UpgradeQtvMediaType(Media *pMedia,
                                                       unsigned int nResult,
                                                       QtvPlayer::MediaTypeT eType)
{

  if(nResult & QTV_MEDIA_TRACK_LIST_AUDIO_ADDED)
  {
    eType = UpgradeMediaTypeForAudio(eType);
  }
  if(nResult & QTV_MEDIA_TRACK_LIST_VIDEO_ADDED)
  {
    eType = UpgradeMediaTypeForVideo(pMedia,eType);
  }
  if(nResult & QTV_MEDIA_TRACK_LIST_TEXT_ADDED)
  {
    eType = UpgradeMediaTypeForText(eType);
  }
  return eType;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::UpgradeMediaTypeForVideo

DESCRIPTION
Update the media type for added video track

DEPENDENCIES
  None.

RETURN VALUE
 Returns the updated media Type.

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::MediaTypeT Mpeg4Player::UpgradeMediaTypeForVideo(Media *pMedia, QtvPlayer::MediaTypeT eType)
{
  Media::MediaFileType eMedType = pMedia->GetFileType();
  bool                              bStillImage = false;

  if(eMedType == Media::CONTENT_VIDEO_STILL_IMAGE ||
     eMedType == Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE ||
     eMedType == Media::CONTENT_AUDIO_VIDEO_STILL_IMAGE_TEXT ||
     eMedType == Media::CONTENT_VIDEO_STILL_IMAGE_TEXT)
  {
    bStillImage = true;
  }
  switch(eType)
  {
    case QtvPlayer::MEDIA_AUDIO_ONLY:
      {
        if(bStillImage)
        {
          return QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE;
        }
        else
        {
          return QtvPlayer::MEDIA_AUDIO_VIDEO;
        }
      }

    case QtvPlayer::MEDIA_AUDIO_TEXT:
      {
        if(bStillImage)
        {
          return QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT;
        }
        else
        {
          return QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT;
        }
      }
    case QtvPlayer::MEDIA_TEXT:
      {
        if(bStillImage)
        {
          return QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT;
        }
        else
        {
          return QtvPlayer::MEDIA_VIDEO_TEXT;
        }
      }
    default:
      {
        return eType;
      }
  }
}
/* ======================================================================
FUNCTION
  Mpeg4Player::UpgradeMediaTypeForAudio

DESCRIPTION
Update the media type for added audio track

DEPENDENCIES
  None.

RETURN VALUE
 Returns the updated media Type.

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::MediaTypeT Mpeg4Player::UpgradeMediaTypeForAudio(QtvPlayer::MediaTypeT eType)
{

  switch(eType)
  {

    case QtvPlayer::MEDIA_VIDEO_ONLY:
      return QtvPlayer::MEDIA_AUDIO_VIDEO;

    case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE:
      return QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE;

    case QtvPlayer::MEDIA_VIDEO_TEXT:
      return QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT;


    case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT:
      return QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT;

    case QtvPlayer::MEDIA_TEXT:
      return QtvPlayer::MEDIA_AUDIO_TEXT;

    default:
      return eType;

  }
}
/* ======================================================================
FUNCTION
  Mpeg4Player::UpgradeMediaTypeForText

DESCRIPTION
Update the media type for added text track

DEPENDENCIES
  None.

RETURN VALUE
 Returns the updated media Type.

SIDE EFFECTS
  None

========================================================================== */
QtvPlayer::MediaTypeT
Mpeg4Player::UpgradeMediaTypeForText( QtvPlayer::MediaTypeT eType)
{

  switch(eType)
  {
    case QtvPlayer::MEDIA_AUDIO_ONLY:
      return QtvPlayer::MEDIA_AUDIO_TEXT;

    case QtvPlayer::MEDIA_VIDEO_ONLY:
      return QtvPlayer::MEDIA_VIDEO_TEXT;

    case QtvPlayer::MEDIA_AUDIO_VIDEO:
      return QtvPlayer::MEDIA_AUDIO_VIDEO_TEXT;

    case QtvPlayer::MEDIA_VIDEO_STILL_IMAGE:
      return QtvPlayer::MEDIA_VIDEO_STILL_IMAGE_TEXT;

    case QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE:
      return QtvPlayer::MEDIA_AUDIO_VIDEO_STILL_IMAGE_TEXT;

    default:
      return eType;
  }
}

#endif
/* ======================================================================
FUNCTION
  Mpeg4Player::GetPlayTimes

DESCRIPTION
//get the desired start & stop times.
//return False if times are bad.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::GetPlayTimes(long &play,long &stop)
{
   if (!reposControl.bTimesValid)
   {
      //try to re-run the filter on the previous times
      SetPlayTimes(reposControl.bRestart ? reposControl.nRestart : reposControl.nStart, reposControl.nStop, reposControl.bRestart);
   }

   if (reposControl.bTimesValid)
   {
      //Get filtered times.
      play = reposControl.bRestart ? reposControl.nRestart : reposControl.nStart;
      stop = reposControl.nStop;
   } else
   {
      //when times are not yet filtered, just return default values so we won't trigger any
      //errors from using times that may get filtered out later anyway.
      play = reposControl.bRestart ? -1 : 0;
      stop = -1;
   }

   //Do error checks and final filters.
   Media * pMP4;

   if(IsAudioVideoTextFromSeparateFiles())
   {
      if ( clip.bHasVideo && clip.pVideoMpeg4Playback)
         pMP4 = clip.pVideoMpeg4Playback;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      else if (clip.bHasAudio && clip.pAudioMpeg4Playback)
         pMP4 = clip.pAudioMpeg4Playback;
      else
         pMP4 = clip.pTextMpeg4Playback;
#else
      else
         pMP4 = clip.pAudioMpeg4Playback;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
   }
   else
   {
      pMP4 = clip.pVideoMpeg4Playback;
   }
   if (pMP4)
   {
      long nDuration = pMP4->GetDurationMsec();
      if (nDuration>0)
      {
         //Cap the stop time to the end of clip.  The streaming
         //library won't accept a stop time beyond the end of clip.
         stop = ZMIN(nDuration,stop);
         //Cap the start time to the end of clip.  The streaming
         //library won't accept a stop time beyond the end of clip.
         play = ZMIN(nDuration, play);
      }
   }

   return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetPlayTimes

DESCRIPTION
//
// set start, resume, & stop positions in the app control data.
// input values are from the PlayClip command.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  None.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::SetPlayTimes(long start,long stop,bool bRestart)
{
  //save the input values
  if (bRestart)
  {
    reposControl.nRestart=start;
  }
  else
  {
    reposControl.nStart=start;
  }
  reposControl.nStop=stop;
  reposControl.bRestart=bRestart;

  //Apply the filters.
  if (clipInfo.bValid)
  {
    bool bClipDisallow =
      (clipInfo.info.streamtype == QtvPlayer::MEDIA_LIVE_STREAMING) ||
      (bLoopTrackFlag) ||
      (!clipInfo.info.RepositioningAllowed);

    //times are valid once filters have been applied.
    reposControl.bTimesValid=true;

    Media * pMP4;
    if(IsAudioVideoTextFromSeparateFiles())
    {
      if ( clip.bHasVideo && clip.pVideoMpeg4Playback)
        pMP4 = clip.pVideoMpeg4Playback;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        else if (clip.bHasAudio && clip.pAudioMpeg4Playback)
          pMP4 = clip.pAudioMpeg4Playback;
        else
          pMP4 = clip.pTextMpeg4Playback;
#else
      else
        pMP4 = clip.pAudioMpeg4Playback;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
    }
    else
    {
      pMP4 = clip.pVideoMpeg4Playback;
    }
    if (!pMP4)
    {
      //Special case, the clip info came from the download media interface,
      //which doesn't have the "allow repositioning" flag.
      //Go ahead and apply the pvx filter now, but mark the data "invalid"
      //so it will get re-filtered later when clip info is available from
      //the playback interface (file format lib).
      reposControl.bTimesValid=false;
    }

    if (pMP4 && clipInfo.info.streamtype == QtvPlayer::MEDIA_LIVE_STREAMING
             && start == 0)
    {
       bClipDisallow = false;
    }
    //Choose the times to use.
    if (bClipDisallow)
    {
      //repos is prohibited for this clip.  always play entire clip or resume
      //from current position.
      reposControl.nStart = 0;
      reposControl.nRestart=(-1);
      reposControl.nStop=(-1);
      if (start!=reposControl.nStart || stop!=reposControl.nStop)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Ignoring PlayClip start times due to clip meta-data or Live stream");
      }
    }
  }

  //limit times to valid range.
  reposControl.nStart=ZMAX(reposControl.nStart,0);
  reposControl.nRestart=ZMAX(reposControl.nRestart,(-1));
  reposControl.nStop=ZMAX(reposControl.nStop,(-1));
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PrepAudioVideo

DESCRIPTION
//
// Prep audio and video
// Return True if all is well.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PrepAudioVideo(bool isReposSuccessful)
{
  bool bVideoOK = true;
  bool bAudioOK = true;
  bool bTextOK = true;
  bool bOK=true;
  bool bOkToPrep = true;

  /*
  *  PREP for Video is needed only when clip has Video that has not ended.
  *  The only exception to this is if player is in PB_READY state as user can play the same clip
  *  with out exiting the QTV.
  */


  bOkToPrep = (clip.bHasVideo && clip.pVideoMpeg4Playback
#ifndef FEATURE_WINCE
&& pActiveVideoPlayer && (!pActiveVideoPlayer->TrackHasEnded())
#endif   /*    FEATURE_WINCE   */
);

  if((clip.bHasVideo) && (playerState== PB_READY))
  {
    bOkToPrep = true;
  }

  if (bOkToPrep)
  {
    bool bError=false;
    /* If restarting when SUSPENDED the video player must be enabled
    ** before prepping.  When enabling video if the playerAudioConcState
    ** is AUDIO_CONCURRENT, GetAudioCodecType() returns CONC_CODEC so
    ** that video only is enabled.
    */
    if (reposControl.bRestart)
    {
      if (playerState == REPOSITIONING && lastPlayerState == SUSPENDED)
      {
        /* Video is currently disabled; need to enable before Prep.
        ** If Enable fails Prep will too, so defer error handling to Prep.
        */
#ifndef FEATURE_WINCE
        if (pActiveVideoPlayer)
        {
          pActiveVideoPlayer->Enable(bError, GetAudioCodecType());
        }
#endif   /*    FEATURE_WINCE   */
        if (bError)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                       "Enable video failed");
        }
        else
        {
          bVideoOK = true;
        }
      }
    }

    //Prep video
#ifndef FEATURE_WINCE
    if(pActiveVideoPlayer && pActiveVideoPlayer->Prep(playbackID, clip.pVideoMpeg4Playback, bError,
                         reposControl.bRestart, GetAudioCodecType()) )
    {
      if (bError)
      {
        bVideoOK = false;
        /*
         * bError == TRUE indicates that video player Prep failed,
         * so there won't be any video.
         * Set clip.bHasVideo to FALSE to allow reposition and other operations
         * on audio,text if any.
         */
        clip.bHasVideo = false;
        VideoError();
      }
    }
    else
    {
      //When video PREP fails, set bError to TRUE;
      bError = true;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video prep failed");
    }
#endif   /*    FEATURE_WINCE   */
    frameInfo.bInfoSetSinceLastPrep = false;
  }

 /*
  * We should do audioPlayer.Prep for Audio if reposition is successful or when
  * qtv_cfg_enable_dsp_release is TRUE. Failure to do this
  * will result in not able to start the audio when repostion fails. This is because
  * when qtv_cfg_enable_dsp_release is TRUE, Mpeg4player::Pause will actually tear down
  * the audio/CMX interfaceby invoking audioMgr::Stop, audioCMX::Stop etc.
  * This scenario can happen as some OEMs have this variable set to TRUE all the time.
  * When qtv_cfg_enable_dsp_release is true, reposControl.bRestart is FALSE.
  *
  * Also,if the audio interface is being created,make sure we do Prep at least once.
  * This makes sure that we will grab the media and can later on report error codes/status back to audio player.
  * For an example,consider following scenario:
  * After getting PB_READY, if we we take out the MMC card, video reposition will fail(as read will fail)
  * and we won't do audio player PREP as reposition failed. So, audioDataState will always remain DATA_OK in audio player
  * and audio player state will be IDLE but audioPlayer::TrackHasEnded will never return TRUE and playback will hang.
  * Grabbing the media helps in posting AUDIO_MEDIA_END back to audio player in addition to other error codes.
  */
  /*
  *  PREP for audio is needed only when clip has audio and track has not ended.
  *  The only exception to this is if player is in PB_READY state or PLAYER_HTTP_PAUSE state (for HTTP Streaming only)
  *  as user can play the same clip with out exiting the QTV.
  */
  bOkToPrep =(clip.bHasAudio
#ifndef FEATURE_WINCE
           && (!AudioPlayerIf::TrackHasEnded(activeAudioPlayerId))
#endif   /*    FEATURE_WINCE   */
  );
  if((clip.bHasAudio) && ((playerState== PB_READY)
      #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
       || ( playerState== PLAYER_HTTP_PAUSE)
      #endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
     ))
  {
    bOkToPrep = true;
  }

#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */

  if( bOkToPrep &&
      ( ( !reposControl.bRestart ) ||
        ( ( isReposSuccessful || ( playerState == REPOSITIONING && lastPlayerState == SUSPENDED ) ) )
      )
    )
  {
#ifndef FEATURE_WINCE
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    if ( (qtv_cfg_enable_dsp_release == TRUE ) && AudioPlayerIf::IsIdle(activeAudioPlayerId))
    {
      /* Audio interface must be reinitialized, because it was stopped by
      ** the incoming call.
      */
      bAudioOK = AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,false,
                                GetAudioMedia(),clip.bHasVideo,GetAudioCodecType());
    }
    else
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

    /* If restarting when SUSPENDED and audio is not concurrent the
    ** audio player must be prepped because it was stopped in the
    ** preceding state. Note that we will not get here when audio
    ** is concurrent because clip.bHasAudio is false in that state.
    */
    if (reposControl.bRestart &&
        (playerState == SUSPENDED ||
         (playerState == REPOSITIONING && lastPlayerState == SUSPENDED)) &&
        (playerAudioConcState == AUDIO_NOT_CONCURRENT))
    {
       bAudioOK = AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,false,
                                GetAudioMedia(),clip.bHasVideo,GetAudioCodecType());
    }
    else
    {
       bAudioOK = AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,reposControl.bRestart,
                                  GetAudioMedia(),clip.bHasVideo,GetAudioCodecType());
    }
#endif   /*    FEATURE_WINCE   */

    if (!bAudioOK)
    {
      AudioError();
      clip.bHasAudio = false;
    }

#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */

  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.bHasText)
  {
#ifndef FEATURE_WINCE
    if (!textPlayer.Prep(playbackID,reposControl.bRestart, GetTextMedia()))
    {
      TimedTextError();
      bTextOK = false;
      clip.bHasText = false;
    }
#endif   /*    FEATURE_WINCE   */
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  if (!clip.bHasVideo)
  {
     bVideoOK = false;
  }
  if (!clip.bHasAudio)
  {
     bAudioOK = false;
  }
  if (!clip.bHasText)
  {
     bTextOK = false;
  }

  if (!bVideoOK && !bAudioOK && !bTextOK)
  {
     bOK = false;
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PrepAudioVideo Failed");
  }

  return bOK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PrepVideo

DESCRIPTION
//
// Prep video
// Return True if all is well.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PrepVideo()
{
   bool bOK=false;
   if (clip.bHasVideo && clip.pVideoMpeg4Playback)
   {
      bool bError;

      //Prep video
#ifndef FEATURE_WINCE
      if (pActiveVideoPlayer && pActiveVideoPlayer->Prep( playbackID, clip.pVideoMpeg4Playback, bError,
                            reposControl.bRestart, GetAudioCodecType() ))
      {
         if (!bError)
         {
            bOK=true;
         }
      }
#endif   /*    FEATURE_WINCE   */
   }
   if (!bOK) QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PrepVideo Failed");
   return bOK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PrepAudio

DESCRIPTION
//
// Prep audio
// Return True if all is well.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PrepAudio()
{
  bool bOK=false;

  if (clip.bHasAudio)
  {
#ifndef FEATURE_WINCE
    if (!AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,reposControl.bRestart,
                                  GetAudioMedia(),clip.bHasVideo,GetAudioCodecType()))
    {
      AudioError();
    }
    else
    {
      bOK=true;
    }
#endif   /*    FEATURE_WINCE   */
  }
  if (!bOK)
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PrepAudio Failed");

  return bOK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PrepText

DESCRIPTION
//
// Prep Text
// Return True if all is well.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PrepText()
{
  bool bOK=false;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.bHasText)
  {
#ifndef FEATURE_WINCE
    if (!textPlayer.Prep(playbackID,reposControl.bRestart,GetTextMedia()))
    {
      TimedTextError();
    }
    else
    {
      bOK=true;
    }
#endif   /*    FEATURE_WINCE   */
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  if (!bOK)
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PrepText Failed");

  return bOK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::AudioError

DESCRIPTION
//some audio control error occurred.  rather than
//leave audio in an indeterminate state, try to stop it.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::AudioError()
{
   Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR);
#ifndef FEATURE_WINCE
   AudioPlayerIf::Stop(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::VideoError

DESCRIPTION
//some video control error occurred.  rather than
//leave video in an indeterminate state, try to stop it.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::VideoError()
{
   bool bError;
   if(
#ifndef FEATURE_WINCE
   pActiveVideoPlayer && ((VideoPlayer*)pActiveVideoPlayer)->GetLastVDECError() == VDEC_ERR_UNSUPPORTED_DIMENSIONS
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
)
   {
     Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED);
   }
   else
   {
     Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR);
   }
#ifndef FEATURE_WINCE
   if(pActiveVideoPlayer)
   {
     pActiveVideoPlayer->Stop(bError);
   }
#endif   /*    FEATURE_WINCE   */
}

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION
  Mpeg4Player::TimedTextError

DESCRIPTION
//some timed text control error occurred.  rather than
//leave timed text in an indeterminate state, try to stop it.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::TimedTextError()
{
   bool bError;
   Notify(QtvPlayer::QTV_PLAYER_STATUS_TEXT_ERROR);
#ifndef FEATURE_WINCE
  (void)textPlayer.Stop(bError);
#endif   /*    FEATURE_WINCE   */
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

/* ======================================================================
FUNCTION
  Mpeg4Player::StreamError

DESCRIPTION
//some streamer control error occurred.  rather than
//leave streamer in an indeterminate state, try to stop it.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::StreamError()
{
   bool bError;
   Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
   if( streamer )
   {
      streamer->Stop(bError);
   }
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool Mpeg4Player::WaitForDownloadFileHeader()
//see whether to wait file header before pseudo-streaming.
//this is the polling function called by the audio thread.
{
  //Keep waiting as long as header is not available and download
  //is still active.

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"PDS2: WaitForDownloadFileHeader");
  UpdateDownloadState();
  bool bWait = (downloadState.playbackTime <= 5000 && bDownloading);

#ifndef FEATURE_WINCE
  return bWait;
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
}


bool Mpeg4Player::StartPseudoStream(bool bPvxStart)
//Start pseudo-stream.
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"PDS2: StartPseudoStream");
  if (playerState!=IDLE
    && playerState!=DOWNLOADING)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                "Weird state in startpseudostream");
    return false;
  }

//#ifndef FEATURE_PV_PSEUDOSTREAM
#if 0
  if (playerState==DOWNLOADING)
  {
    //shouldn't get here.
    ErrorMsg( LOG_LVL_ERROR, "Error, pseudo-streaming is not allowed\n",0,0,0);
    return false;
  }
#endif

  //This is a new playback.
  OpenPlaybackSession();

  //save the "pvx start" flag, we'll need this when opening the
  //local file.
  appControl.bPvxStart=bPvxStart;

    //parse the file & start playing now.

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
       if (!FileSavedToBuffer)
       {
          clip.pVideoLocalBuffer = clip.pAudioLocalBuffer = NULL;
       }
       else
       {
       clip.pAudioLocalFilename = clip.pVideoLocalFilename = NULL;

       clip.pVideoLocalBuffer =
       clip.pAudioLocalBuffer   = (unsigned char*)pPVDL->getPDS2BufPtr();

       clip.audioBufSize =
       clip.videoBufSize = pPVDL->getPDS2BufSize();
       }
#endif /*  FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2   */

       if(StartPlayingOrNotifyReady())
       {
         return true;
       }
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PDS2: StartPlayingOrNotifyReady failed");
    return false;
}
#endif  /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

/* ======================================================================
FUNCTION
  Mpeg4Player::StartPlaying

DESCRIPTION
//Start or restart
//This is used for file, stream or pseudo-stream.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::StartPlaying()
{
   bool bError = false;
   bool bStarted=false;
   bool reposition=true;
   QtvPlayer::PlayerStateRecordT ps;
   bool bReposReadError= false;
   bool bOkToRepos = true;
   long audout = 0;

   GetPlayerState(ps);
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, " StartPlaying");

   if ((playerState != PB_READY) && (playerState != REPOSITIONING))
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in StartPlaying");
      return false;
   }

   /* In the case of streaming, once a PlayClip is issued, we wait for
   ** STREAM_START before we can actually start playing. During this time
   ** it's possible to receive an incoming call, so check with concurrency
   ** manager if it is ok to play.
   */
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
   if (!qtv_conc_mgr::can_process_command(this, QTV_PLAYER_MIME_TYPE))
   {
     /* If there is a call in progress, just complain and return true.
     ** After the call has ended, another PlayClip will be sent which will
     ** cause StartPlaying to be called again.
     */
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "StartPlaying issued while in a call.");
     return true;
   }
#endif
   //Retrieve the start and stop times.
   long start,stop;
   if (!GetPlayTimes(start,stop))
   {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
      return false;
   }

   /* Intitialize the function pointer in media for processing Media Layer
     request */
   if(clip.pVideoMpeg4Playback)
   {
     clip.pVideoMpeg4Playback->SetMediaPlayerCb(&Mpeg4Player::MediaNotifyCB);
   }


   #ifdef FEATURE_QTV_BCAST_FLO
#error code not present
   #endif /* FEATURE_QTV_BCAST_FLO */


#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   if(clip.bHTTPStreaming && playerState == PB_READY)
   {
       /* In the Initial play insure that there is enough data
        * to play for minimum start amount of buffering time
        */
       if(!HTTPCanPlayTracks(http_startupTime))
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Not enough Data Buffered to begin PlayBack HTTP Streaming");
         return false;
       }
       else
       {
         Notify(PLAYER_HTTP_PAUSE);
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Enough Data Buffered Start PlayBack for HTTP Streaming");
       }
   }

#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

   //start audio & video
   bStarted=false;

   /*
   *  Reposition is not allowed, if audio/video track has ended. Ignore text track, since its simple to restart.
   *  The only exception to this is if player is in PB_READY state as user can play the same clip
   *  with out exiting the QTV.
   */
   if(
       (playerState != PB_READY)
#ifndef FEATURE_WINCE
        &&
       ((clip.bHasVideo && pActiveVideoPlayer && (pActiveVideoPlayer->TrackHasEnded()) && !clip.bStillImage) ||
        (clip.bHasAudio && AudioPlayerIf::TrackHasEnded(activeAudioPlayerId)))
#else
#error code not present
#endif
          )
   {
     bOkToRepos = false;
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Audio/Video track has ended, reposition failed...");
     reposition = false;
     Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
   }
#ifdef FEATURE_QTV_DIVX_DRM
  if( m_bCallCommitPlayback )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "m_bCallCommitPlayback is TRUE");
    if(clip.pVideoMpeg4Playback)
    {
      if(clip.pVideoMpeg4Playback->CommitDivXPlayback())
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                     "clip.pVideoMpeg4Playback->CommitDivXPlayback returned TRUE..");
        m_bCallCommitPlayback = false;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                     "clip.pVideoMpeg4Playback->CommitDivXPlayback returned FALSE..");
        if( clip.pVideoMpeg4Playback->GetFile()->GetFileError() == DRM_AUTHORIZATION_ERROR)
        {
          //Report AUTHORIZATION ERROR
          Notify(QtvPlayer::QTV_PLAYER_DRM_AUTHORIZATION_ERROR );
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "QTV_PLAYER_DRM_AUTHORIZATION_ERROR");
      }
        bOkToRepos = false;
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "clip.pVideoMpeg4Playback is NULL");
      bOkToRepos = false;
    }
  }
#endif

   if (clip.bHasVideo && bOkToRepos)
   {
      long vidout = 0;
      //special case for still image, can't set the position beyond zero
      //or you won't be able to get the image.
      long vidstart = clip.bStillImage ? 0 : start;
      if (vidstart>=0)
      {
         if (clip.pVideoMpeg4Playback)
         {
#ifdef FEATURE_QTV_GENERIC_BCAST
           if (clip.bBcastGeneric && clip.bLiveBcast)
           {
             /* Don't set video position in the case of Bcast Live, the media
             ** has already been told to play from the live point.
             */
             bError = false;
           }
           else
#endif /* FEATURE_QTV_GENERIC_BCAST */
           {
             /* All other cases, a reposition was done, set the new position. */
             vidout = clip.pVideoMpeg4Playback->SetVideoPosition(vidstart,
                                                                 bError,
                                                                 ps.playbackMsec);
           }
            /* check if reposition is successful */
            if(bError)
            {
               Notify(QtvPlayer::QTV_PLAYER_STATUS_NO_SYNC_FRAME_ERROR);
               reposition = false;
               if(
                   (clip.pVideoMpeg4Playback)           &&
                   (clip.pVideoMpeg4Playback->GetFile())&&
                   (clip.pVideoMpeg4Playback->GetFile()->GetFileError()==READ_FAILED)
                 )
               {
                 bReposReadError = true;
               }
            }
            else
            {
              /* repositioning is successful, so we can call suspend to
                 flush any buffers which may have old frames */
#ifndef FEATURE_WINCE
              if (pActiveVideoPlayer)
        {
          (void)pActiveVideoPlayer->Suspend(bError);
              }

              if(clip.bHasAudio
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
                )
          {
            bError = AudioPlayerIf::Suspend(activeAudioPlayerId);
          }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
              if (clip.bHasText)
              (void)textPlayer.Suspend(bError);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */
              reposition = true;

              /* The VideoPlayer is suspendded, We're in the REPOSITIONING state,
              ** and we're about to clear the playback offset, which will cause
              ** VideoPlayer and VideoDec to 'relearn' the playback offset on the next
              ** frame which plays. Make sure there aren't any decoded frames queued
              ** which might screw up that process!
              */
#ifndef FEATURE_WINCE
              if (pActiveVideoPlayer)
        {
      pActiveVideoPlayer->Flush();
              }
              if(!clip.bStillImage)
                start = vidout;
              if (pActiveVideoPlayer)
        {
      pActiveVideoPlayer->SetPlayTimes(start,stop);
              }
#endif   /*    FEATURE_WINCE   */
            }
         }
      }
      else
      {
        // if vidstart < 0
#ifndef FEATURE_WINCE
        if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->SetPlayTimes(start,stop);
  }
#endif   /*    FEATURE_WINCE   */
      }
   } // bHasVideo

   if ( clip.bHasAudio && bOkToRepos)
   {
     audout = start;

     if (start>=0)
     {
       Media * pMP4 = GetAudioMedia();
       if (pMP4)
       {
         if(reposition)
         {
#ifdef FEATURE_QTV_GENERIC_BCAST
           if (clip.bBcastGeneric && clip.bLiveBcast)
           {
             /* Don't set audio position in the case of Bcast live, the media
             ** has already been told to play from the live point.
             */
             bError = false;
           }
           else
#endif /* FEATURE_QTV_GENERIC_BCAST */
           {
             /* All other cases, a reposition was done, set the new position. */
           audout = pMP4->SetAudioPosition(start, bError, ps.playbackMsec);
           }
           if(!bError)
           {
             reposition = true;
             start = audout;
#ifndef FEATURE_WINCE
             AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, start, stop);
#endif   /*    FEATURE_WINCE   */
             bReposReadError = false;
           }
           else
           {
             reposition = false;
             if(
                   (clip.pAudioMpeg4Playback)           &&
                   (clip.pAudioMpeg4Playback->GetFile())&&
                   (clip.pAudioMpeg4Playback->GetFile()->GetFileError()==READ_FAILED)
               )
             {
                 bReposReadError = true;
             }
           }
#ifndef FEATURE_WINCE
     if((!clip.bHasVideo) && (!bError))
           {
             /* in case it does not have video, here we call suspend to
                flush any buffers which may have old frames */
            bError = AudioPlayerIf::Suspend(activeAudioPlayerId);
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
             if (clip.bHasText)
               (void)textPlayer.Suspend(bError);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
           }
#endif   /*    FEATURE_WINCE   */
         }
       }
     }
     else
     {
       start = audout;
#ifndef FEATURE_WINCE
      AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, start, stop);
#endif   /*    FEATURE_WINCE   */
     }
   } // bHasAudio && !clip.bStillImage

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   if (clip.bHasText && bOkToRepos)
   {
     if (start>=0)
     {
       if(reposition)
       {
         Media *txtMedia = GetTextMedia();
         if (NULL != txtMedia)
         {
#ifdef FEATURE_QTV_GENERIC_BCAST
           if (clip.bBcastGeneric && clip.bLiveBcast)
           {
             /* Don't set text position in the case of Bcast live, the media
             ** has already been told to play from the live point.
             */
             bError = false;
           }
           else
#endif /* FEATURE_QTV_GENERIC_BCAST */
           {
             /* All other cases, a reposition was done, set the new position. */
             txtMedia->SetTextPosition(start,bError, ps.playbackMsec);
           }
         }

         /* in case it does not have video/audio, here we call
            suspend to flush any buffers which may have old frames */
#ifndef FEATURE_WINCE
         if( !clip.bHasVideo && !clip.bHasAudio )
           (void)textPlayer.Suspend(bError);

         textPlayer.SetPlayTimes(start,stop);
#endif   /*    FEATURE_WINCE   */
         if(
             (bError)                            &&
             (clip.pTextMpeg4Playback)           &&
             (clip.pTextMpeg4Playback->GetFile())&&
             (clip.pTextMpeg4Playback->GetFile()->GetFileError()==READ_FAILED)
           )
         {
           bReposReadError = true;
         }
       }
     }
     else
     {
#ifndef FEATURE_WINCE
       textPlayer.SetPlayTimes(start,stop);
#endif   /*    FEATURE_WINCE   */
     }
   } // bHasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

   if(bReposReadError)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "bReposReadError is SET,returning FALSE..");
     return false;
   }

   //Prep audio & video.  The reason to have a separate step for this
   //is that there are some delays in this processing, so by getting
   //it out of the way we get better AV sync at startup.

   if (!PrepAudioVideo(reposition))
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio video prep failed");
      return false;
   }

   if (playerState == REPOSITIONING)
   {
      qtv_event_clip_reposition_resume_type  qtv_event_clip_reposition_resume;
      // for PAYLOAD use the "start"
      qtv_event_clip_reposition_resume.resume_time = start;
      // for PAYLOAD use the "start"
#ifndef FEATURE_WINCE
      event_report_payload(EVENT_QTV_CLIP_REPOSITION_RESUME, // for PLAYBACK only
                           sizeof(qtv_event_clip_reposition_resume_type),
                           &qtv_event_clip_reposition_resume);
#endif   /*    FEATURE_WINCE   */
   }

   /*
    * Invalidate AV SYNC offset irrespective of reposition.
    * When audio,video,text starts, offset will be set correctly.
    * Without this, if user does PAUSE followed by some REPOSITIONING and RESUME/UNPAUSE on video only clip and if
    * reposition is not successful, then lot of frames will be dropped and video will look like as if playing fast.
    */

    //Re-init the playback timing
    m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
    m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
    m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

   if (reposition)
   {
     // First set the new AV sync offset.
     // If bDelayAudioOutput is true, we are definitely going to start video
     // first. The first video timestamp may be quite different from the
     // start time (either from what the user requested, or from the PLAY
     // response). Therefore allow video to set the playback offset on its own
     // when it first reads it -- don't set the offset here.
     if (!appControl.bDelayAudioOutput)
     {
#ifndef FEATURE_WINCE
       m_mediaSync.SetPlaybackOffset(AVSync::AudioAV, (int) ZUtils::Clock(), start);
       m_mediaSync.SetPlaybackOffset(AVSync::VideoAV, (int) ZUtils::Clock(), start);
       m_mediaSync.SetPlaybackOffset(AVSync::TextAV, (int) ZUtils::Clock(), start);
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
     }
   }
#ifndef FEATURE_WINCE
   else
   {
      if( (lastPlayerState == PAUSED) || (lastPlayerState == SUSPENDED) )
      {
         /* Special case, under following scenario: PlayClip(0,-1) -> Pause() -> after few seconds-> PlayClip(x,-1) */
         if(clip.bHasAudio && (!AudioPlayerIf::IsDone(activeAudioPlayerId)))
         {
           m_mediaSync.SetPlaybackOffset(AVSync::AudioAV, (int) ZUtils::Clock(), AudioPlayerIf::GetElapsedTime(activeAudioPlayerId));
         }
         else if(clip.bHasVideo && pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone()))
         {
           m_mediaSync.SetPlaybackOffset(AVSync::VideoAV, (int) ZUtils::Clock(), pActiveVideoPlayer->GetElapsedTime());
         }
         else if(clip.bHasText && (!textPlayer.IsDone()) )
         {
           m_mediaSync.SetPlaybackOffset(AVSync::TextAV, (int) ZUtils::Clock(), textPlayer.GetElapsedTime());
         }
      }
   }
#endif   /*    FEATURE_WINCE   */

//The order of starting video followed by audio followed by text changed because
//of change in renderer task priority. With out this fix, if text is available, video won't be displayed
//when playing from EFS as we end up repoprting PLAYING status late.

//start the text, if available and track has not ended
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifndef FEATURE_WINCE
   if (clip.bHasText)
   {
      if (textPlayer.Start(bError, reposControl.bRestart))
      {
         if (bError)
         {
            TimedTextError();
         } else
         {
            bStarted = true;
         }
      }
   }
#endif   /*    FEATURE_WINCE   */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
/*
   * For some clips, there is some delay when starting the actual audio playback. Begining of audio playback is indicated
   * when first AV SYNC feedback arrives from DSP.
   * However, this delay might cause momentary video freeze as video would have played
   * frames worth more than 250 milliseconds(our lip-sync video size).
   * Thus give some time for audio before starting video.
   */

   //if ((playerState == PB_READY)&& clip.bHasAudio && clip.bHasVideo)
   //{
   //  zrex_sleep(AUDIO_DELAY_DURATION);
   //}

   //Start the video,if available and track has not ended
   if (clip.bHasVideo
#ifndef FEATURE_WINCE
&& pActiveVideoPlayer && !(pActiveVideoPlayer->TrackHasEnded())
#endif   /*    FEATURE_WINCE   */
)
   {
       appControl.bReceivedFirstFrame = false;
#ifndef FEATURE_WINCE
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
      if (pActiveVideoPlayer && pActiveVideoPlayer->Start(bError, reposControl.bRestart))
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
#endif   /*    FEATURE_WINCE   */
      {
         if (bError)
         {
            VideoError();
         } else
         {
            bStarted = true;
         }
      }
   }

   //start the audio,if available and track has not ended
   if (clip.bHasAudio
#ifndef FEATURE_WINCE
        && (!AudioPlayerIf::TrackHasEnded(activeAudioPlayerId))
#endif   /*    FEATURE_WINCE   */
        )
   {
      if (StartAudio())
      {
#ifndef FEATURE_WINCE
          if (reposition)
          {
             AudioPlayerIf::SetTSfromBytes(activeAudioPlayerId, audout);
          }
#endif   /*    FEATURE_WINCE   */
          bStarted = true;
      }
   }


#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   /*
    * When reposition is attempted with qtv_cfg_enable_dsp_release set to TRUE and
    * If we don't reset qtv_cfg_enable_dsp_release after successful "Prep" for Audio & Video,
    * subsequent reposition during BUFFERING state would result in "AbError". This is because,
    * audioPlayer::Prep would be called with (bRestart == FALSE) and audio state would be (SUSPENDED)
    */
   qtv_cfg_enable_dsp_release = FALSE;
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, ">>Reset qtv_cfg_enable_dsp_release in StartPlaying after successful Mpeg4player::PrepAudioVideo()");
#endif

   appControl.bHasMediaStarted = bStarted;

   //change state
   if (bStarted)
   {
      //The AV players start out in buffering.  For a pure local file do not
      //transition to Playing at this point.  The app can issue a PlayClip
      //from a reposition and the clip would not resume because the video
      //data state is not yet at DATA_OK.
#ifdef FEATURE_QTV_PSEUDO_STREAM
      if (clip.bPseudoStreaming)
      {
        Notify(PLAYING);
      }
      else
#endif /*FEATURE_QTV_PSEUDO_STREAM*/
      {
         Notify(BUFFERING);
      }
      #ifdef FEATURE_QTV_GENERIC_BCAST
      // Execute the Queued up Mute Commands
      if(clip.bmQueuedMute)
      {
        if(!CheckForAllMute(clip.bmQueuedMute))
        {
          uint32             bmSucceeded = 0;
          QtvPlayer::ReturnT eRet        = QtvPlayer::QTV_RETURN_ERROR;
          eRet = MutePlaybackTracks(clip.bmQueuedMute,QTV_TRACK_MUTE,&bmSucceeded);
          QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                        "Queued up mute command(%d) returned %d,%d",
                        clip.bmQueuedMute,bmSucceeded,eRet);
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
          "Couldnt execute the queued up commands; All mute check failed");
        }
        clip.bmQueuedMute = 0;
      }
      #endif
      return true;
   }

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::StartPlay failed");
   return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::RestartActiveClip

DESCRIPTION
// restart the currently active clip with new times.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::RestartActiveClip()
{
  bool bAudioPaused = false;
  bool bVideoPaused = false;
  bool bTextPaused = false;

   if (playerState != BUFFERING &&
       playerState != PAUSED &&
       playerState != PLAYING &&
       playerState != REPOSITIONING
#ifdef FEATURE_QTV_PSEUDO_STREAM
       && playerState != PLAYER_PSEUDO_PAUSE
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
       && playerState != PLAYER_HTTP_PAUSE
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
       && playerState != SUSPENDED
       )
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in RestartActiveClip");
      return false;
   }

   /* don't do repositioning if any of the tracks has ended */
   //Get the new play times.
   long start,stop;
   if (!GetPlayTimes(start,stop))
   {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
      return false;
   }

   if (appControl.bStepAVSyncNeeded)
   {
     appControl.bStepAVSyncNeeded = false;
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                  "Seek performed after single step, resetting bStepAVSyncNeeded");
   }

   //Suspend audio & video if needed.
   switch (playerState)
   {
      case PLAYING:
      case BUFFERING:
      case PAUSED:
         bool bError;
         bool bSuspended;
         bSuspended=false;
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
         /* In generic audio formats after CMX start we need to get the
            audio spec from the CMX after parsing the data that we are giving.
            So we shouldn't allow repositioning until we get the audio spec
            from CMX otherwise it will leads to target freeze */
         if(clip.pAudioMpeg4Playback)
         {
           if(clip.pAudioMpeg4Playback->isGenericAudioFileInstance())
           {
             if(!clip.pAudioMpeg4Playback->IsGenericAudioFileMediaInitialized())
             {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "GenericAudioFileMedia is not initialized need to get data from CMX after parsing, so till that not allowing repositioning");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
               return true;
             }
           }
         }
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
         if (clip.bHasAudio)
         {
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
            if (qtv_cfg_enable_dsp_release)
            {
               /* Audio interface was stopped, so don't do a pause */
               bSuspended=true;
            }
            else
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
#ifndef FEATURE_WINCE
            if (AudioPlayerIf::Pause(activeAudioPlayerId))
            {
               bAudioPaused=true;
            }
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
#else //#ifndef FEATURE_WINCE
#error code not present
#endif   /*    FEATURE_WINCE   */
         }
         if (clip.bHasVideo)
         {
#ifndef FEATURE_WINCE
            if (pActiveVideoPlayer && pActiveVideoPlayer->Pause(bError))
            {
               if (bError)
               {
                  VideoError();
               }
               else
               {
                 bVideoPaused=true;
               }
            }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
         }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
         if (clip.bHasText)
         {
#ifndef FEATURE_WINCE
            if (textPlayer.Pause(bError))
            {
               if (bError)
               {
                  TimedTextError();
               }
               else
               {
               bTextPaused=true;
               }
            }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
         }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
        /* If any one of the audio or video failed to pause then resume and return */
        if((clip.bHasAudio ^ bAudioPaused) ||
           (clip.bHasVideo ^ (bVideoPaused || clip.bStillImage)))
        {
          if(clip.bHasAudio && bAudioPaused)
          {
#if defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined (FEATURE_QTV_PSEUDO_STREAM)
            if(!ResumeAudio(NULL))
            {
              AudioError();
            }
#endif
          }
          if(clip.bHasVideo && bVideoPaused)
          {
#if defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined (FEATURE_QTV_PSEUDO_STREAM)
            if(!ResumeVideo(NULL))
            {
              VideoError();
            }
#endif
          }
          if(clip.bHasText && bTextPaused)
          {
#if defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined (FEATURE_QTV_PSEUDO_STREAM)
            if(!ResumeText(NULL))
            {
              TimedTextError();
            }
#endif
          }
          QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "One of the player unable to pause. Repositioning failed. bAudioPaused=%d  bVideoPaused=%d  bTextPaused=%d", bAudioPaused, bVideoPaused, bTextPaused);
          Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
          return true;
        }
        else
        {
          bSuspended=true;
        }
         if (!bSuspended)
         {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::Player has failed to SUSPEND");
            return false;
         }
         zrex_sleep(50); /* give some time to suspend */
         break;

      case SUSPENDED:
      {
        /* Audio and video are disabled, so don't do a pause */
        bSuspended = true;
        break;
      }
   }

   //Notify that playback is suspended for repositioning.
   if (playerState != REPOSITIONING)
   {
      Notify(REPOSITIONING);
   }

   appControl.bDoneAudioBuffering = false;
   appControl.bReceivedFirstFrame = false;

   //Restart streaming.
   if (clip.bStreaming)
   {
      if (StartStreaming())
      {
         //remain in REPOSITIONING state and
         //wait on STREAM_START message
         return true;
      }
   }
   //Restart pseudo-stream or file.
   else
   {
      /* Since Audio task has higher priority, but for repositioning even
         audio has to be reposition at available video sync frame time (so
         for repositioning audio task depends on video task ).
         "bDelayAudio" will give video some head start.
      */
      //appControl.bDelayAudio = TRUE;

      if (StartPlaying())
      {
         //(we're now in BUFFERING or PLAYING state)
         return true;
      }
   }

   return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ResumePlay

DESCRIPTION
//
// resume play, return true if anything resumed.
// don't transition state yet, we'll do that when we
// get messages from audio & video.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::ResumePlay()
{
  QtvPlayer::PlayerStateRecordT ps;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ResumePlay" );
  GetPlayerState(ps);

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  /* If resume is called after the voice call ends,skip the IsDone check.
  For audio only clips this check returns TRUE and hence the clip doesn't continue
  after the voice call ends */
  if (!qtv_cfg_enable_dsp_release)
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
  {
#ifndef FEATURE_WINCE
     if(IsDone())
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::ResumePlay called when media is done.." );
       DonePlaying(false,false);
       return true;
     }
#endif   /*    FEATURE_WINCE   */
  }

  if( playerState!=PAUSED && playerState!=REPOSITIONING
#ifdef FEATURE_QTV_PSEUDO_STREAM
      && playerState != PLAYER_PSEUDO_PAUSE
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      && playerState != PLAYER_HTTP_PAUSE
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
      && playerState != SUSPENDED
    )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in ResumePlay");
    return false;
  }

  long start,stop;
  if (!GetPlayTimes(start,stop))
  {
    Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
    return false;
  }

  bool bError=false;

  //resume streamer.
  if (clip.bStreaming && streamer)
  {
    if (streamer->Resume(bError))
    {
      //report an error.
      if (bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Resume");
        StreamError();
      }
    }
    //if it's not streaming just resume anyway, the AV will
    //run out of data soon enough.
  }

 bool bResumed = false;


  if(appControl.bStepAVSyncNeeded)
  {
    /* now synchronize audio and text with video */
    long PlayTime = 0;
     long AudioPlayTime = 0;
    /* now synchronize audio and text with video */

#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
      PlayTime = pActiveVideoPlayer->GetElapsedTime();
    }
    AudioPlayTime = PlayTime;
#endif   /*    FEATURE_WINCE   */

    /*
     * Make sure audio track has not ended otherwise, doing audioPlayer::Prep will throw an
     * AUDIO_ERROR which will abort the playback.
     */

    if(clip.bHasAudio
#ifndef FEATURE_WINCE
        && (!AudioPlayerIf::IsDone(activeAudioPlayerId))
#endif   /*    FEATURE_WINCE   */
        )
    {
      if (clip.pAudioMpeg4Playback )
      {
         AudioPlayTime = clip.pAudioMpeg4Playback->SetAudioPosition(PlayTime, bError, ps.audioPlaybackMsec);
      }
      else if(clip.pVideoMpeg4Playback)
      {
         AudioPlayTime = clip.pVideoMpeg4Playback->SetAudioPosition(PlayTime, bError, ps.audioPlaybackMsec);
      }

      if(!bError)
      {
#ifndef FEATURE_WINCE
        AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, AudioPlayTime, stop);
        /* this will flush audio buffered in CMX */
        bError = AudioPlayerIf::Suspend(activeAudioPlayerId);
        /* update audio TS value to current position when resuming */
        AudioPlayerIf::SetTSfromBytes(activeAudioPlayerId,AudioPlayTime);
#endif   /*    FEATURE_WINCE   */
      }
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if(clip.bHasText)
    {
      bool lBError = false;

      Media *txtMedia = GetTextMedia();
      if (NULL != txtMedia)
      {
        (void)txtMedia->SetTextPosition(PlayTime, lBError,ps.textPlaybackMsec);
      }
#ifndef FEATURE_WINCE
      textPlayer.SetPlayTimes(PlayTime, stop);
      (void)textPlayer.Suspend(lBError);
#endif   /*    FEATURE_WINCE   */
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    /*
     * Should not do Prep for audio if the track has already ended, otherwise,
     * AUDIO_ERROR will be reported which will abort the playback.
     */
     if(clip.bHasAudio
#ifndef FEATURE_WINCE
          && (AudioPlayerIf::IsDone(activeAudioPlayerId))
#endif
          )
     {
       //Do prep for video and text,if available,individually as audio track has ended.
       if (clip.bHasVideo)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::ResumePlay, detected audio_done,calling PrepVideo");
         (void)PrepVideo();
       }
       #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
       if (clip.bHasText)
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::ResumePlay, detected audio_done,calling PrepText" );
         (void)PrepText();
       }
       #endif
     }
     else
     {
       (void)PrepAudioVideo(true);
     }
  }

   //reset the audio/video sync point
  m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

  //resume video followed by audio.
  //resuming audio before video causes loss of AV SYNC when playing WMV from MMC.
#ifndef FEATURE_WINCE
  if (clip.bHasVideo && !clip.bStillImage) //If video has ended and audio still exists, enable video so that we can pause/resume
  {
    if (playerState == SUSPENDED)
    {
      /* Video is currently disabled; need to enable before resume.
       * If Enable fails Resume will too, so defer error handling
       * to Resume. */
      if (pActiveVideoPlayer)
      {
        pActiveVideoPlayer->Enable(bError, GetAudioCodecType());
      }
      if (bError)
      {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Enable video failed");
      }
    }
    if (pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone()))
    {
      #ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
      if (qtv_cfg_enable_dsp_release && clip.bHasAudio)
      {
        AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, lastAudioPosition, stop);
        m_mediaSync.SetPlaybackOffset(AVSync::AudioAV, (int)ZUtils::Clock(), lastAudioPosition);

      }
      else
      #endif

      {
        if(clip.bHasAudio && (!AudioPlayerIf::IsDone(activeAudioPlayerId)))
        {
          m_mediaSync.SetPlaybackOffset(AVSync::AudioAV, (int)ZUtils::Clock(),
                                    AudioPlayerIf::GetElapsedTime(activeAudioPlayerId));
        }
      }

      if(!clip.bHasAudio
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif
        )
      {
        m_mediaSync.SetPlaybackOffset(AVSync::VideoAV, (int)ZUtils::Clock(), m_nVideoPlaybackTime);
        QTV_MSG1(MSG_LEGACY_HIGH,"Video Player to be resumed but updated the OFFSET for TS %ld",m_nVideoPlaybackTime);
      }

      if (pActiveVideoPlayer && pActiveVideoPlayer->Resume(bError,stop))
      {
        if (bError)
        {
          VideoError();
        }
        else
        {
          bResumed=true;
        }
        #ifdef FEATURE_FILE_FRAGMENTATION
        if( clip.bHasVideo && clip.pVideoMpeg4Playback && bResumed)
        {
          clip.pVideoMpeg4Playback->setPausedVideo(FALSE);
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Resumed Video From App");
        }
        #endif  /* FEATURE_FILE_FRAGMENTATION */
      }
      else
      {
       /* videoPlayer.Resume returns false.  This means that Resume cannot be
       * performed due to Resume being issued while the audioPlayer is playing.
       * This can happen if the Media Player and Qtv states get out of sync.
       * This is not a serious error so just indicate that Qtv resumed which has
       * the effect of ignoring the Resume.
       */
        bResumed=true;
      }
    }
  }

 /*
  * If audio track has ended, do not prep/start the audio as it will throw AUDIO_ERROR which will
  * abort the playback.
  */
  if(clip.bHasAudio && (!AudioPlayerIf::IsDone(activeAudioPlayerId)))
  {
    if(appControl.bStepAVSyncNeeded)
    {
      appControl.bStepAVSyncNeeded = false;
      bResumed = StartAudio();
    }
    else
    {
      #ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
        if (qtv_cfg_enable_dsp_release)
        {
          if ( !AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,false,
                                GetAudioMedia(),clip.bHasVideo,GetAudioCodecType()) )
          {
            AudioError();
          }
          else
          {
            /* We are not resuming, so set bRestart to false before calling
            ** StartAudio()
            */
            reposControl.bRestart=false;
            bResumed = StartAudio();

            // Currently we are in the PAUSED state.
            // Go to BUFFERING state so that when AudioBufferingDone() is
            // called, it will restart audio output.
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
              "ResumePlay: moving to BUFFERING state after restarting audio");

            Notify(BUFFERING);
          }
        }
        else
      #endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

       if ((playerState == SUSPENDED)
           && playerAudioConcState == AUDIO_NOT_CONCURRENT)
       {
         /* Audio is currently DISABLED; need to restore then resume. */
         bResumed = RestoreResumeAudio(stop);
       }
       else

       if ( AudioPlayerIf::Resume(activeAudioPlayerId, stop) )
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "ResumePlay: Resuming Audio successful..!!");
         bResumed=true;
       }
       else
       {
        /* audioPlayer.Resume returns false.  This means that Resume cannot be
         * performed due to Resume being issued while the audioPlayer is playing.
         * This can happen if the Media Player and Qtv states get out of sync.
         * This is not a serious error so just indicate that Qtv resumed which
         * has the effect of ignoring the Resume.
         */
        bResumed=true;
       }
    }
    #ifdef FEATURE_FILE_FRAGMENTATION
    if( clip.bHasAudio && clip.pAudioMpeg4Playback && bResumed)
    {
      clip.pAudioMpeg4Playback->setPausedAudio(FALSE);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Resumed Audio From App");
    }
    #endif  /* FEATURE_FILE_FRAGMENTATION */

    /* Reposition video if its a still image*/
    if (clip.bHasVideo && clip.pVideoMpeg4Playback && clip.bStillImage)
    {
        m_mediaSync.SetPlaybackOffset(AVSync::AudioAV, (int)ZUtils::Clock(),
        AudioPlayerIf::GetElapsedTime(activeAudioPlayerId));

      (void)clip.pVideoMpeg4Playback->SetVideoPosition(0, bError, 0);
      if (playerState == SUSPENDED)
      {
        /* Video is currently disabled; need to enable before resume.
         * If Enable fails Resume will too, so defer error handling
         * to Resume. */
        if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->Enable(bError, GetAudioCodecType());
        }
        if (bError)
        {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Enable video failed");
        }
      }

      bool lBError;
        //Prep video
      if (pActiveVideoPlayer && pActiveVideoPlayer->Prep(playbackID, clip.pVideoMpeg4Playback, lBError,
                            true, GetAudioCodecType()))
        {
        if (lBError)
            {
              VideoError();
            }
            else
            {
              /* Start Video */
              if (pActiveVideoPlayer->Start(bError, true))
              {
                if (lBError)
                {
                  VideoError();
                }
                else
                {
                    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "ResumePlay: StillImage: Restarting Video successful..!!");
                }
              }
            }
        }
    }
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.bHasText && (!textPlayer.IsDone()))
  {
    if(appControl.bStepAVSyncNeeded)
    {
      appControl.bStepAVSyncNeeded = false;
      bResumed = textPlayer.Start(bError,false);
    }
    else
    {
      if( (!clip.bHasAudio) && (!clip.bHasVideo) )
      {
        m_mediaSync.SetPlaybackOffset(AVSync::TextAV, (int)ZUtils::Clock(), m_nTextPlaybackTime);
        QTV_MSG1(MSG_LEGACY_HIGH,"Text Player to be resumed but updated the OFFSET for TS %ld",m_nTextPlaybackTime);
      }

      if (textPlayer.Resume(bError,stop))
      {
        if (bError)
        {
          TimedTextError();
        }
        else
        {
          bResumed=true;
        }
#ifdef FEATURE_FILE_FRAGMENTATION
        if( clip.bHasText && GetTextMedia() && bResumed)
        {
          GetTextMedia()->setPausedText(FALSE);
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Resumed Text From App");
        }
#endif  /* FEATURE_FILE_FRAGMENTATION */
      }
    }
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  /* State of Qtv and its DSP has been restored */
  if (qtv_cfg_enable_dsp_release)
  {
    qtv_cfg_enable_dsp_release = FALSE;
  }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

  if (bResumed)
  {
    /* Set the resume pending flag */
    appControl.bResumePending = true;
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::ResumeClip failed");
  }
  prevAudioConcState = AUDIO_NOT_CONCURRENT;
  return bResumed;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::UpdateStopTime

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
void Mpeg4Player::UpdateStopTime(long stop)
{
   if (clip.bHasAudio)
   {
#ifndef FEATURE_WINCE
    AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, -1, stop);
#endif   /*    FEATURE_WINCE   */
   }
   if (clip.bHasVideo
#ifndef FEATURE_WINCE
       && pActiveVideoPlayer
#endif   /*    FEATURE_WINCE   */
)
   {
#ifndef FEATURE_WINCE
      pActiveVideoPlayer->SetPlayTimes(-1,stop);
#endif   /*    FEATURE_WINCE   */
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PlayClip

DESCRIPTION
//
//  Start or Resume playing.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PlayClip(const PV_PLAY_CLIP_type * pEvent)
{
   ASSERT( pEvent );

  bool bOK = false;
  bOK = PlayClip(pEvent->startTime, pEvent->stopTime,pEvent->switchURN, pEvent->pbSpeed);

  if (pEvent->switchURN != NULL)
  {
    QTV_Delete(pEvent->switchURN);
  }
  return bOK;
}

#ifdef FEATURE_FILE_FRAGMENTATION
/* ======================================================================
FUNCTION
  Mpeg4Player::SkipClip

DESCRIPTION
//
//  Skip across the Random Access points.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::SkipClip(const QTV_SKIP_CLIP_type *pEvent)
{
   ASSERT( pEvent );

   #ifdef FEATURE_QTV_WINDOWS_MEDIA
     if( (Media::WMA_CODEC  == GetAudioCodecType()) ||
         (Media::WMV1_CODEC == GetVideoCodecType()) ||
         (Media::WMV2_CODEC == GetVideoCodecType()) ||
         (Media::WMV3_CODEC == GetVideoCodecType()) ||
         (Media::WMA_PRO_CODEC == GetVideoCodecType()) ||
         (Media::WMA_PRO_PLUS_CODEC == GetVideoCodecType()))
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::SkipClip, Returning false for WM..");
       return false;
     }
   #endif

   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::SkipClip %d", pEvent->skipNumber);

   //make sure there's a valid urn
   if (!videoURN.bValid && !audioURN.bValid && !textURN.bValid)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error: SkipClip with no URN");
     return false;
   }

   // Skip clip can result in playing of audio or video, so query the
   // concurrency manager.
#ifndef FEATURE_QTV_DISABLE_CONC_MGR
   if (qtv_conc_mgr::can_process_command(this, QTV_PLAYER_MIME_TYPE))
   {
     UpdateAudioConcState();
     if (playerAudioConcState == AUDIO_CONCURRENCY_NOT_SUPPORTED)
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "PlayClip not supported");
       Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED);
       return true;
     }
   }
   else
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "PlayClip not supported");
     Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_NOT_SUPPORTED);
     return false;
   }
#endif //#ifndef FEATURE_QTV_DISABLE_CONC_MGR
   //Take actions.
   long start = -1;
   long stop = -1;

   if ( clipInfo.info.RepositioningAllowed && !bLoopTrackFlag )
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Replaying Same Clip");
   }
   else
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Repositioning NOT ALLOWED" );
      Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
     return false;
   }

   //Cannot skip access points in stream.
   if (clip.bStreaming)
   {
     return false;
   }

   //Cannot skip access points in stream.
#ifndef FEATURE_QTV_GENERIC_BCAST
   if (clip.bBcastFLO || clip.bBcastISDB)
#else
   // Skip is not supported for Bcast streams at present
   if (clip.bBcastStream)
#endif
   {
     return false;
   }

   //Cannot restart a bitstream
   if (clip.bM4vFile)
   {
     return false;
   }

   qtv_event_clip_repositioning_type qtv_event_clip_repositioning_payload;
   qtv_event_clip_repositioning_payload.current_time = 0;
   //get elapsed time.
   unsigned long mediaElapsedTime =0;
   if (clip.bHasVideo
#ifndef FEATURE_WINCE
       && pActiveVideoPlayer
#endif   /*    FEATURE_WINCE   */
)
   {
#ifndef FEATURE_WINCE
     mediaElapsedTime = pActiveVideoPlayer->GetElapsedTime();
#endif   /*    FEATURE_WINCE   */
     qtv_event_clip_repositioning_payload.current_time = ZMAX(qtv_event_clip_repositioning_payload.current_time,mediaElapsedTime);
   }

   if (clip.bHasAudio)
   {
#ifndef FEATURE_WINCE
     mediaElapsedTime =  AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
     qtv_event_clip_repositioning_payload.current_time = ZMAX(qtv_event_clip_repositioning_payload.current_time,mediaElapsedTime);
   }
#ifndef FEATURE_WINCE
   event_report_payload(EVENT_QTV_CLIP_REPOSITIONING,
                        sizeof(qtv_event_clip_repositioning_type),
                        &qtv_event_clip_repositioning_payload);
#endif   /*    FEATURE_WINCE   */

   //Set restart time.
#ifdef FEATURE_QTV_PSEUDO_STREAM
   if (pEvent)
   {
     if(clip.bPseudoStreaming)
     {
       if(pEvent->skipNumber > 0)
       {/* This is a FF operation */
         if (PLAYER_PSEUDO_PAUSE == playerState)
         { /* FF, when pseudo stream is in pseudo pause state */
           QTV_MSG( QTVDIAG_MP4_PLAYER,
            "PlayClip: FF, when Pseudo Stream is in pseudo pause state."
            "Remain in state PLAYER_PSEUDO_PAUSE");
           Notify(PLAYER_PSEUDO_PAUSE);
           return false;
         }
       }
     }
   }
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

   if (!GetPlayTimes(start,stop))
   {
     Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
     return false;
   }

   //Restart active clip.
   if (pEvent->skipNumber==0 &&
       (playerState==PLAYING || playerState==BUFFERING))
   {
     //Special case for local file or pseudo-stream stop time update,
     //there's no real need to restart.
     UpdateStopTime(stop);
     return true;
   }

   //Normal restart.
   if (!RestartActiveClip(pEvent->skipNumber))
   {
     ErrorAbort(RESTART_ACTIVE_CLIP_ERROR);
     return false;
   }
   else
     return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::RestartActiveClip

DESCRIPTION
// restart the currently active clip with new times.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::RestartActiveClip(int32 skipNumber)
{
   if (playerState != BUFFERING &&
       playerState != PAUSED &&
       playerState != PLAYING &&
       playerState != REPOSITIONING
#ifdef FEATURE_QTV_PSEUDO_STREAM
       && playerState != PLAYER_PSEUDO_PAUSE
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
       && playerState != PLAYER_HTTP_PAUSE
#endif
       && playerState != SUSPENDED
       )
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in RestartActiveClip");
      return false;
   }
#ifndef FEATURE_WINCE
   /* don't do repositioning if any of the tracks has ended */
   if( (clip.bHasAudio && AudioPlayerIf::TrackHasEnded(activeAudioPlayerId)) ||
       (clip.bHasVideo && pActiveVideoPlayer && (pActiveVideoPlayer->TrackHasEnded() && !clip.bStillImage)) )
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Track ended. Repositioning failed.");
      Notify(QtvPlayer::QTV_PLAYER_STATUS_SEEK_FAILED);
      return true;
   }
#endif   /*    FEATURE_WINCE   */
   //Get the new play times.
   long start,stop;
   if (!GetPlayTimes(start,stop))
   {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
      return false;
   }

   //Suspend audio & video if needed.
   switch (playerState)
   {
      case PLAYING:
      case BUFFERING:
      case PAUSED:
         bool bError;
         bool bSuspended;
         bSuspended=false;
         if (clip.bHasAudio)
         {
#ifndef FEATURE_WINCE
            if (AudioPlayerIf::Pause(activeAudioPlayerId))
               {
                  bSuspended=true;
               }
#endif   /*    FEATURE_WINCE   */
            }
         if (clip.bHasVideo)
         {
#ifndef FEATURE_WINCE
            if (pActiveVideoPlayer && pActiveVideoPlayer->Pause(bError))
            {
               if (bError)
               {
                  VideoError();
               } else
               {
                  bSuspended=true;
               }
            }
#endif   /*    FEATURE_WINCE   */
         }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
         if (clip.bHasText)
         {
#ifndef FEATURE_WINCE
            if (textPlayer.Pause(bError))
            {
               if (bError)
               {
                  TimedTextError();
               } else
               {
                  bSuspended=true;
               }
            }
#endif   /*    FEATURE_WINCE   */
         }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

         if (!bSuspended)
         {
            return false;
         }
         zrex_sleep(50); /* give some time to suspend */
         break;

      case SUSPENDED:
      {
        /* Audio and video are disabled, so don't do a pause */
        bSuspended = true;
        break;
      }
   }

   //Notify that playback is suspended for repositioning.
   if (playerState != REPOSITIONING)
   {
      Notify(REPOSITIONING);
   }

   //Reposition pseudo-stream or file.

   /* Since Audio task has higher priority, but for repositioning even
      audio has to be reposition at available video sync frame time (so
      for repositioning audio task depends on video task ).
      "bDelayAudio" will give video some head start.
   */
   appControl.bDelayAudio = TRUE;

   if (RepositionAccessPoint(skipNumber))
   {
     //(we're now in BUFFERING or PLAYING state)
     return true;
   }
   return false;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::RepositionAccessPoint

DESCRIPTION
//Start or restart
//This is used for file, stream or pseudo-stream.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::RepositionAccessPoint(int32 skipNumber)
{
   bool bError = false;
   bool bStarted=false;
   bool reposition=false;

   if (playerState != REPOSITIONING)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in RepositionAccessPoint");
      return false;
   }

   QtvPlayer::PlayerStateRecordT ps;
   GetPlayerState(ps);

   //Retrieve the start and stop times.
   long start,stop;
   if (!GetPlayTimes(start,stop))
   {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
      return false;
   }

   //repositon and start audio & video
   bStarted=false;

   if (clip.bHasVideo)
   {
      long vidout = 0;
      if (clip.pVideoMpeg4Playback)
      {
        vidout = clip.pVideoMpeg4Playback->RepositionVideoAccessPoint(skipNumber, bError,ps.playbackMsec);
        if (bError )
        {
          Notify(QtvPlayer::QTV_PLAYER_STATUS_NO_RANDOM_ACCESS_POINT_ERROR);
          reposition = false;
        }
        else
        {
          /* repositioning is successful, so we can call suspend to
             flush any buffers which may have old frames */
#ifndef FEATURE_WINCE
          if (pActiveVideoPlayer)
          {
            (void)pActiveVideoPlayer->Suspend(bError);
          }

          if(clip.bHasAudio)
            bError = AudioPlayerIf::Suspend(activeAudioPlayerId);

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
          if(clip.bHasText)
            (void)textPlayer.Suspend(bError);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */
          reposition = true;
          /* The VideoPlayer is suspendded, We're in the REPOSITIONING state,
          ** and we're about to clear the playback offset, which will cause
          ** VideoPlayer and VideoDec to 'relearn' the playback offset on the next
          ** frame which plays. Make sure there aren't any decoded frames queued
          ** which might screw up that process!
          */
#ifndef FEATURE_WINCE
          if (pActiveVideoPlayer)
          {
            pActiveVideoPlayer->Flush();
          }
          start = vidout;
          SetPlayTimes(start,stop,true);
          if (pActiveVideoPlayer)
    {
       pActiveVideoPlayer->SetPlayTimes(start,stop);
          }
#endif   /*    FEATURE_WINCE   */
        }
      }
   } // bHasVideo

   if ( clip.bHasAudio && !clip.bStillImage )
   {
      long audout = start;
      Media * pMP4 = GetAudioMedia();
      if (pMP4)
      {
        if(((clip.bHasVideo) && (reposition)) || (!clip.bHasVideo))
        {
          if(!clip.bHasVideo)
          {
            audout = pMP4->RepositionAudioAccessPoint(skipNumber, bError,ps.playbackMsec);
            if(!bError)
              reposition = true;
          }
          else
            audout = pMP4->SetAudioPosition(start, bError, ps.playbackMsec);

          if((!clip.bHasVideo) && (!bError))
          {
            /* in case it does not have video, here we call suspend to
               flush any buffers which may have old frames */
#ifndef FEATURE_WINCE
            bError = AudioPlayerIf::Suspend(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifndef FEATURE_WINCE
            if(clip.bHasText)
              (void)textPlayer.Suspend(bError);
#endif   /*    FEATURE_WINCE   */
          }
          if(!bError)
          {
            start = audout;
            if(!clip.bHasVideo)
              SetPlayTimes(start,stop,true);
#ifndef FEATURE_WINCE
            AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, start, stop);
#endif   /*    FEATURE_WINCE   */
          }
          else
          {
            reposition = false;
          }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
        }
      }
   } // bHasAudio && !clip.bStillImage

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   if (clip.bHasText)
   {
      long textout = start;
      if (reposition)
      {
        bool lBError = false;

        Media *txtMedia = GetTextMedia();
        if (NULL != txtMedia)
        {
          textout = txtMedia->SetTextPosition(start, lBError, ps.playbackMsec);
        }

        /* in case it does not have video/audio, here we call
           suspend to flush any buffers which may have old frames */
#ifndef FEATURE_WINCE
        if( !clip.bHasVideo && !clip.bHasAudio )
           (void)textPlayer.Suspend(lBError);

        textPlayer.SetPlayTimes(textout,stop);
#endif   /*    FEATURE_WINCE   */
      }
   } // bHasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

   if(!reposition)
   {
     SetPlayTimes(start,stop,true);
   }
   //Prep audio & video.  The reason to have a separate step for this
   //is that there are some delays in this processing, so by getting
   //it out of the way we get better AV sync at startup.
   if (!PrepAudioVideo(reposition))
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio video prep failed");
      return false;
   }

   if (playerState == REPOSITIONING)
   {
      qtv_event_clip_reposition_resume_type  qtv_event_clip_reposition_resume;
      // for PAYLOAD use the "start"
      qtv_event_clip_reposition_resume.resume_time = start;
      // for PAYLOAD use the "start"
#ifndef FEATURE_WINCE
      event_report_payload(EVENT_QTV_CLIP_REPOSITION_RESUME, // for PLAYBACK only
                           sizeof(qtv_event_clip_reposition_resume_type),
                           &qtv_event_clip_reposition_resume);
#endif   /*    FEATURE_WINCE   */
   }

   if(reposition)
   {
     //Re-init the playback timing
     m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
     m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
     m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

   //First set the new AV sync offset.
#ifndef FEATURE_WINCE
   m_mediaSync.SetPlaybackOffset(AVSync::AudioAV, (int)ZUtils::Clock(), start);
   m_mediaSync.SetPlaybackOffset(AVSync::VideoAV, (int)ZUtils::Clock(), start);
   m_mediaSync.SetPlaybackOffset(AVSync::TextAV, (int)ZUtils::Clock(), start);
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
   }



#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   if (clip.bHasText)
   {
#ifndef FEATURE_WINCE
      if (textPlayer.Start(bError, reposControl.bRestart))
      {
         if (bError)
         {
            TimedTextError();
         } else
         {
            bStarted = true;
         }
      }
#endif   /*    FEATURE_WINCE   */
   }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

   //start the audio and video
   appControl.bReceivedFirstFrame = false;

   if (clip.bHasVideo && pActiveVideoPlayer)
   {
#ifndef FEATURE_WINCE
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
      if (pActiveVideoPlayer->Start(bError, reposControl.bRestart))
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
#endif   /*    FEATURE_WINCE   */
      {
         if (bError)
         {
            VideoError();
         }
         else
         {
            bStarted = true;
         }
      }
   }

   if (clip.bHasAudio)
   {
     if (StartAudio())
     {
        bStarted = true;
     }
   }

   //change state
   if (bStarted)
   {
      //The AV players start out in buffering, but for a pure
      //local file the transition to Playing happens immediately,
      //so just start in Playing.
#ifdef FEATURE_QTV_PSEUDO_STREAM
      if (clip.bPseudoStreaming)
      {
         Notify(PLAYING);
      }
      else
#endif /*FEATURE_QTV_PSEUDO_STREAM*/
      {
         Notify(BUFFERING);
      }
      return true;
   }

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::RepositionAccessPoint failed");
   return false;
}

#endif /*FEATURE_FILE_FRAGMENTATION*/

#ifdef FEATURE_QTV_PLAYLIST
bool Mpeg4Player::Skip(const QTV_SKIP_type *pEvent)
{
  QTV_NULL_PTR_CHECK(pEvent, false);
  return Skip(pEvent->playlistName,
    pEvent->clipIndex, (pEvent->isClipIndexRelative != 0),
    pEvent->time, pEvent->when);
}

// This method makes it easy to simulate Skip() requests within our code.
bool Mpeg4Player::Skip(const char *playlistName, int32 clipIndex,
                       bool isClipIndexRelative, int32 offset, int32 when)
{
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
  State stateBeforeSkip = playerState;

#ifndef FEATURE_WINCE
  QTV_MSG_PRIO5(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::Skip, state = %d, "
                "clipIndex = %d, rel = %d, offset = %d, when = %d",
                stateBeforeSkip, clipIndex, isClipIndexRelative ? 1 : 0, offset, when);
#endif   /*    FEATURE_WINCE   */

  reposControl.reposType = 1;

  // See if we are going to play during skip. It is only supported for PV
  // Server-Side Playlists. We only play during skip for a inter-clip skip or
  // if the request is not for now.
  bool shouldPlayDuringSkip = false;

#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
  shouldPlayDuringSkip = (qtvConfigObject.GetQTVConfigItem(
    QtvConfig::QTVCONFIG_PLAY_DURING_SKIP) != 0);

  if (shouldPlayDuringSkip)
  {
    shouldPlayDuringSkip =
      (clipInfo.info.streamtype == QtvPlayer::MEDIA_PV_SERVER_SIDE_PLAYLIST);
  }

  if (shouldPlayDuringSkip)
  {
    // Turn off play during skip if this is an intra-clip seek.
    if (isClipIndexRelative)
    {
      shouldPlayDuringSkip = (clipIndex != 0);
    }
    else
    {
      shouldPlayDuringSkip =
        (clipIndex != streamer->GetPlayingServerSidePlaylistClipIndex());
    }

    // Always play during skip if the request is not for now.
    if (when != -1)
    {
      shouldPlayDuringSkip = true;
    }
  }
#endif /* FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST */

  switch (playerState)
  {
    case IDLE:
    case CONNECTING:
    case PB_READY:
    {
      if (PlayClip(-1, -1))
      {
        // When we get the PLAY response from the server, we will get the
        // STREAM_START notication; ignore it. Wait for SKIP_SUCCESS instead.
        appControl.nStreamStartsToIgnore++;
        Notify(PLAYING);
      }
      else
      {
        return false;
      }
    }
    break;

    case PAUSED:
    case SUSPENDED:
    {
      if (shouldPlayDuringSkip)
      {
        if (PlayClip(-1, -1))
        {
          Notify(PLAYING);
        }
        else
        {
          return false;
        }
      }
    }
    break;

    case REPOSITIONING:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "Mpeg4Player::Skip: illegal state  %d", playerState);
      return false;
  }

  // So either restart or start stop have their offsets set.
  SetPlayTimes(offset, -1, true);
  reposControl.nStart = reposControl.nRestart;

  // [Jeff] This flag indicates whether to leave the server streaming data and to
  // continue playback on we have issued the skip command to the server.
  // I prefer:  (stateBeforeSkip == PLAYING) || (stateBeforeSkip == BUFFERING);
  // My compromise was: ((when == -1) || (stateBeforeSkip == PLAYING) ||
  //                     (stateBeforeSkip == BUFFERING));
  // But we have agreed to always restart playback once
  // QtvPlayer::PlaylistPlay() is called.
  // Sigh, I never win ...
  bool keepStreaming = true;

#ifdef FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST
  if ((playlistName != NULL) && (strlen(playlistName) == 0))
  {
    playlistName = NULL;
  }
#endif /* FEATURE_QTV_PV_SERVER_SIDE_PLAYLIST */

  // Try to queue the request first so we don't have to unpause after pausing
  // in case or error, and so the request goes out while we wait for pausing
  // to finish.
  if (!streamer->SkipToClipInServerSidePlaylist(playlistName,
       clipIndex, isClipIndexRelative, offset, when, keepStreaming,
       shouldPlayDuringSkip))
  {
    return false;
  }

  if (!keepStreaming)
  {
    appControl.bPauseMediaAfterStartPlaying = true;
  }

  // Pause media players if we're playing already unless we're supposed to
  // play during skip.
  if (!shouldPlayDuringSkip)
  {
    if (playerState == PLAYING)
    {
      if (!PauseMedia())
      {
        return false;
      }
    } // playerState == PLAYING

    Notify(REPOSITIONING);
  }

  // Wait for STREAM_SERVER_SIDE_PLAYLIST_SKIP_SUCCESS or FAILED
  return true;
#else
  QTV_USE_ARG1(playlistName);
  QTV_USE_ARG1(clipIndex);
  QTV_USE_ARG1(isClipIndexRelative);
  QTV_USE_ARG1(offset);
  QTV_USE_ARG1(when);

  return false;
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
}
#endif /* FEATURE_QTV_PLAYLIST */

#ifdef FEATURE_QTV_STREAM_RECORD
/* ======================================================================
FUNCTION
  Mpeg4Player::RecordClip

DESCRIPTION
//
//  Start or Resume playing.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::RecordClip(const PV_RECORD_CLIP_type * pEvent)
{
   if (FALSE == clip.bStreaming)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Record operation is not allowed.");
     return FALSE;
   }

   if(pEvent)
   {
     ZUtils::StrcpyN(recFileName, pEvent->fileName, QTV_MAX_URN_BYTES+1);
     QTV_MSG_SPRINTF_1(QTVDIAG_MP4_PLAYER,"Record to file name %s",recFileName );

     bRecFileOverWrite = pEvent->overwrite;
     if(bRecFileOverWrite)
     {
       QTV_MSG(QTVDIAG_MP4_PLAYER,"File overwrite is TRUE" );
     }
     recDuration = pEvent->duration;
     recMode = pEvent->mode;

     if(playerState < PB_READY)
     {
       bRecPending = TRUE;
     }
     else
     {
       if (TRUE == initStreamRecording(recFileName))
       { /* Disallow repositioning during stream recording */
         clipInfo.info.RepositioningAllowed =  FALSE;
       }
       else
       {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "initStreamRecording() returned FALSE");
         return FALSE;
       }
     }

   }

   return TRUE;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::stopStreamRecording

DESCRIPTION
//
//  Start or Resume playing.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */

bool Mpeg4Player::stopStreamRecording(const PV_RECORD_STOP_type*)
{
   bool status = FALSE;

   bRecPending = FALSE;
   if (bRecording)
   {
      bRecording = FALSE;
      if(clip.pVideoMpeg4Playback)
      {
        status = clip.pVideoMpeg4Playback->qtvio_stop_file_writer();
        if(FALSE == status)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error while stopping Qtv stream recorder");
        }
        clipInfo.info.RepositioningAllowed =
                        clip.pVideoMpeg4Playback->IsRepositioningAllowed();
        QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                       "Clip repositioning is set to: %s",
                       (clipInfo.info.RepositioningAllowed?"TRUE":"FALSE"));
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream does not exist");
      }
   }
   else
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "No RECORDING in Progress");

   }
   return status;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::initStreamRecording

DESCRIPTION
//
//  Start or Resume playing.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */

bool Mpeg4Player::initStreamRecording(char *filePath)
{
  bool status = FALSE;

  bRecPending = FALSE;


  if (FALSE == clip.bStreaming)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
            "Record operation is not allowed during Local Playback");

    return FALSE;
  }

  if (bRecording)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
            "Record operation is already in progress");

    return FALSE;
  }
  if(clip.pVideoMpeg4Playback)
  {
    if((clip.bHasAudio))
    { /* Fill videoFMT Structures */

      /* Fix the following. It fails at this point of time */
      /*pQtvStream->getTimeScale(sessionID, trackId);*/
      uint32 rtpaudiotimescale = 8000; /* Not used */
      QTV_MSG(QTVDIAG_MP4_PLAYER,"Setup audio recording");
      clip.pVideoMpeg4Playback->initAudioTrackRecording(clipInfo.info,
                                                      rtpaudiotimescale);
    }

    if((clip.bHasVideo))
    {
      uint32 rtpvideotimescale = clip.pVideoMpeg4Playback->GetVideoTimescale(0);
      QTV_MSG(QTVDIAG_MP4_PLAYER,"Setup video recording");
      clip.pVideoMpeg4Playback->initVideoTrackRecording(clipInfo.info,
                                                      rtpvideotimescale);
    }

    if (clip.bHasEnhancedVideo)
    {
      uint32 rtpvideotimescale = clip.pVideoMpeg4Playback->GetVideoTimescale(1);
      QTV_MSG(QTVDIAG_MP4_PLAYER,"Setup enhanced video recording");
      clip.pVideoMpeg4Playback->initEnhancedVideoTrackRecording(clipInfo.info,
                                                      rtpvideotimescale);
    }

    #ifdef FEATURE_QTV_TEXT_RECORD
    if((clip.bHasText))
    {
      uint32 rtptexttimescale = clip.pVideoMpeg4Playback->GetTextTimescale();
      QTV_MSG(QTVDIAG_MP4_PLAYER,"Setup text recording");
      clip.pVideoMpeg4Playback->initTextTrackRecording
                                      (clipInfo.info, rtptexttimescale);
    }
    #endif

    status = clip.pVideoMpeg4Playback->qtvio_start_file_writer(filePath,
                                                               bRecFileOverWrite,
                                                               recorderCallback,
                                                               this);
  }
  bRecording = status;

  if(!bRecording)
  {
  Notify(QtvPlayer::QTV_RECORDER_FAILED);
  }
  return status;
}

bool Mpeg4Player::SetStrRecEFSReservedSpace(uint32 resSpaceinBytes)
{
   if(clip.pVideoMpeg4Playback)
   {
      clip.pVideoMpeg4Playback->SetStrRecEFSReservedSpace(resSpaceinBytes);
      return true;
   }
   return false;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::recorderCallback

DESCRIPTION
Status callback from stream recorder

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
========================================================================== */
void Mpeg4Player::recorderCallback(/*RecorderStatusCode*/ int status,
                                   void* client_data)
{
  Mpeg4Player *player = (Mpeg4Player *)client_data;
  //Following macro returns void if pointers, (pEvent and (player->clip).pVideoMpeg4Playback in this case) are NULL.
  QTV_NULL_PTR_CHECK(player, RETURN_VOID);
  QTV_NULL_PTR_CHECK( ((player->clip).pVideoMpeg4Playback), RETURN_VOID);

  switch(status)
  {

  case FILE_SYS_LIMIT_APPROACHING:  /* File system Limit is approaching */

    player->Notify(QtvPlayer::QTV_REC_FILE_SYS_LIMIT_APPROACHING);
    break;

    case FILE_SYS_LIMIT_REACHED:  /* File system limit aprroached */

    player->Notify(QtvPlayer::QTV_REC_FILE_SYS_FULL);
    (player->clipInfo).info.RepositioningAllowed =
                    (player->clip).pVideoMpeg4Playback->IsRepositioningAllowed();

    break;

    case FILE_ALREADY_EXISTS: /* File name already exists, cannot overwrite */

    player->Notify(QtvPlayer::QTV_RECORDER_FILE_ALREADY_EXISTS);
    (player->clipInfo).info.RepositioningAllowed =
                     (player->clip).pVideoMpeg4Playback->IsRepositioningAllowed();

      break;

    case FILE_OW_FAILURE: /* Failed to overwrite a file name */

    player->Notify(QtvPlayer::QTV_RECORDER_FILE_OW_FAILURE);
      (player->clipInfo).info.RepositioningAllowed =
                    (player->clip).pVideoMpeg4Playback->IsRepositioningAllowed();

    break;

    case FILE_SYS_WRITE_ERR: /* File system failure */

    player->Notify(QtvPlayer::QTV_RECORDER_FILE_WRITE_ERROR);
     (player->clipInfo).info.RepositioningAllowed =
                   (player->clip).pVideoMpeg4Playback->IsRepositioningAllowed();

    break;

    case GENERIC_ERR:
      player->Notify(QtvPlayer::QTV_RECORDER_FAILED);
      (player->clipInfo).info.RepositioningAllowed =
                    (player->clip).pVideoMpeg4Playback->IsRepositioningAllowed();

      break;

    case RECORDING_COMPLETED:
      player->Notify(QtvPlayer::QTV_PLAYER_RECORDING_COMPLETED);
       (player->clipInfo).info.RepositioningAllowed =
                    (player->clip).pVideoMpeg4Playback->IsRepositioningAllowed();

      break;

    default:  /* unknown status code */

      player->Notify(QtvPlayer::QTV_RECORDER_FAILED);
      (player->clipInfo).info.RepositioningAllowed =
                (player->clip).pVideoMpeg4Playback->IsRepositioningAllowed();
    break;
  }

}

uint32 Mpeg4Player::getRecordedClipDuration()
{
  if(clip.pVideoMpeg4Playback)
  {
     return clip.pVideoMpeg4Playback->getRecordedClipDuration();
  }
  return 0;
}

#endif /* FEATURE_QTV_STREAM_RECORD */
/* ======================================================================
FUNCTION
  Mpeg4Player::RegisterForCallback

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
bool Mpeg4Player::RegisterForCallback(const PV_REGISTER_FOR_CALLBACK_type * pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::RegisterForCallback" );

   if (pEvent)
   {
      appControl.callback.func=pEvent->callback;
      appControl.callback.pClientData=pEvent->pClientData;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      //Also register this callback function with the TimedText thread
      if(1 == guMpeg4PlayerRefCnt)
      {
        if (!TimedText::RegisterForCallback(appControl.callback.func, appControl.callback.pClientData))
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::RegisterForCallback failed");
           return false;
        }
      }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

      return true;
   }
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::RegisterForCallback failed");
   return false;
}

bool Mpeg4Player::RegisterForCallback(const PV_REGISTER_FOR_CALLBACK_EX_type * pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::RegisterForCallback" );

   if (pEvent)
   {
      appControl.callback.instanceFunc=pEvent->callback;
      appControl.callback.pClientData=pEvent->pClientData;
      appControl.callback.pUserData=pEvent->pUserData;
      appControl.callback.handle=pEvent->handle;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      //Also register this callback function with the TimedText thread
      if(1 == guMpeg4PlayerRefCnt)
      {
        if (!TimedText::RegisterForCallback(appControl.callback.instanceFunc, appControl.callback.pClientData,
           appControl.callback.pUserData, appControl.callback.handle))
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::RegisterForCallback failed");
           return false;
        }
      }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

      return true;
   }
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::RegisterForCallback failed");
   return false;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::Stop

DESCRIPTION
//
//  Stop playback.  Return False if error occurred.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::Stop(PV_STOP_type *pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::Stop" );

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   if(httpPullData.pHttpPullTimer)
   {
       //Stop the timer and also remove it from the rex timer list if its pending.
       HttpPullTimerStop();
    (void)rex_clr_timer( httpPullData.pHttpPullTimer );
       //Note: The static event will be removed from the event list for Mpeg4Player
       // when the player thread is killed inside zrex_kill_thread()
   }
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

   //See whether playback is underway.
   bool bActive=false;
   nPrerollDurationFilled  = 0;
   switch (playerState)
   {
      case IDLE:
         //ignore
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
         break;
      case CONNECTING:
         //could be either download or streaming connect.
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
         bActive=clip.bStreaming || clip.bDownload || clip.bQtvHttpStreaming;
#else
         bActive=clip.bStreaming || clip.bDownload;
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
         break;
      default:
         bActive=true;
         break;
   }
   if (bActive)
   {
#ifndef FEATURE_WINCE
      if (HasVideo() && pActiveVideoPlayer)
      {
        pActiveVideoPlayer->StopVideoOutput();
      }
#endif   /*    FEATURE_WINCE   */

      bool bError;
      StopPlayer(bError);
/* Instead of deleting the Media Object through StopPlayer, Initialize the Playback data */
      ResetPlaybackData();
#ifdef FEATURE_QTV_STREAM_RECORD
      if (bRecording)
      {
        bRecording = FALSE;
        if(clip.pVideoMpeg4Playback)
        {
          (void)clip.pVideoMpeg4Playback->qtvio_stop_file_writer();
        }
      }
#endif

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
      playingBeforeVoiceCall = FALSE;
      if (qtv_cfg_enable_dsp_release)
      {
        qtv_cfg_enable_dsp_release = FALSE;
      }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

      if (bError)
      {
         //errors stopping.  it's hard to know what
         //to do here.  just goto idle to prevent
         //a situation where we can't start up
         //again.  report an error termination
         //instead of a stop though.
         PlaybackComplete(ABORT);
      } else
      {
         //stop was successful.
         PlaybackComplete(STOPPED);
      }
   }

   QTV_USE_ARG1(pEvent);

   //Make sure stop flag is reset after completion of Stop
   if (bStop)
   {
     bStop = false;
   }
   return true;
}

#ifdef  FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */

bool Mpeg4Player::PauseMedia()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::PauseMedia");

  bool bError = false;

  if (clip.bHasAudio)
  {
#ifndef FEATURE_WINCE
    if (!AudioPlayerIf::Pause(activeAudioPlayerId))
    {
      bError = true;
      AudioError();
    }
#endif   /*    FEATURE_WINCE   */
  }
  if (clip.bHasVideo)
  {
    if (
#ifndef FEATURE_WINCE
        pActiveVideoPlayer && pActiveVideoPlayer->Pause(bError)
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
    )
    {
      if (bError)
      {
        VideoError();
      }
    }
  } /* if (clip.bHasVideo) */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.bHasText)
  {
#ifndef FEATURE_WINCE
    if (textPlayer.Pause(bError))
    {
      if (bError)
      {
        TimedTextError();
      }
    }
#endif   /*    FEATURE_WINCE   */
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  return !bError;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Pause

DESCRIPTION
//
//  Pause
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::Pause(PV_PAUSE_type *)
{
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::Pause,State %d",playerState );

   bool bPaused=false;

   bool bStreamPaused = false;
   bool bError = false;

   if (clip.bStreaming && streamer && streamer->isReconnectionInProgress())
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                  "Pause issued while reconnection in progress");
   }

   // If we are in repositioning, that means were were waiting for
   // STREAM_START before calling StartPlaying(). Do that now so
   // that afterwards when we resume from the pause, it is just a normal
   // one. When STREAM_START arrives we should be in the PAUSED state so
   // nothing will happen. It's possible that we might be repositioning again
   // in which case STREAM_START will call StartPlaying() too early, but very
   // unlikely. If that ever happens, the next STREAM_START might transition
   // the player state incorrectly. As a workaround we keep a count of
   // the number of STREAM_START notifications to ignore.
   if (clip.bStreaming && (playerState == REPOSITIONING))
   {
     // If the player has transitioned to REPOSITIONING state due to a Skip,then
     //set the Play time to whatever it was it when the skip was performed
     //since we are resuming now
     if(((clipInfo.info.streamtype == QtvPlayer::MEDIA_PV_SERVER_SIDE_PLAYLIST) ||
         (clipInfo.info.streamtype == QtvPlayer::MEDIA_WM_SERVER_SIDE_PLAYLIST))
        && (reposControl.reposType==1))
     {
         SetPlayTimes(-1,-1,true);
     }
     if (StartPlaying())
     {
       // If we normally trigger off STREAM_START to StartPlaying(),
       // ignore one additional STREAM_START.
       if (reposControl.reposType == 0)
       {
         appControl.nStreamStartsToIgnore++;
       }
     }
     else
     {
       ErrorAbort(START_PLAYING_ERROR);
       return false;
     }
   }

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   if(qtv_cfg_enable_dsp_release_oem == TRUE)
   {
      qtv_cfg_enable_dsp_release = TRUE;
      qtv_cfg_enable_dsp_release_oem = FALSE;
   }

   if( qtv_cfg_enable_dsp_release )
   {
#if (defined (FEATURE_QTV_QOS_SELECTION) && defined ( FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 ))
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION && FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
    if( (playerState==PAUSED) && IsPlaying() )
    {
       /* This means, one of the video/audio/text players moved to playing state and
       mpeg4player is waiting to move to "playing" state, so force to "playing" state */

       Playing();
    }
      /* The pause could be either user initiated or could have been initiated
         by either  OEMCM or QVP due to an incoming voice/VT call. If it is
         due a to an incoming voice/VT  call  then store the state of the player
         i.e. whether it was playing or not, so that it can restored to the original
         state when the Resume is invoked by either OEMCM or VT after the voice/VT
         call ends. This flag will also be used by the mediaplayer to store the
         state of the player when it receives a suspend event from Brew due to an
         incoming call alert
      */
      if( ( playerState ==  BUFFERING ) ||
          ( playerState == PLAYING  )   ||
          ( playerState == REPOSITIONING )

#ifdef FEATURE_QTV_PSEUDO_STREAM
         ||( playerState == PLAYER_PSEUDO_PAUSE )
#endif //FEATURE_QTV_PSEUDO_STREAM
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
         ||( playerState == PLAYER_HTTP_PAUSE )
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

        )
      {
         playingBeforeVoiceCall = TRUE;
      }
      else
      {
         playingBeforeVoiceCall = FALSE;
      }
   }

#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

   switch (playerState)
   {

#if (defined (FEATURE_QTV_QOS_SELECTION) && defined ( FEATURE_QTV_QDSP_RELEASE_RESTORE ))
#error code not present
#endif  /* (defined (FEATURE_QTV_QOS_SELECTION) && defined ( FEATURE_QTV_QDSP_RELEASE_RESTORE )) */

      case PAUSED:
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
        if (qtv_cfg_enable_dsp_release)
        {
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /*  FEATURE_QTV_QOS_SELECTION */
          if (clip.bHasAudio)
          {
            /* Get the current audio timestamp and store it in lastAudioPosition
            ** so that we can resume audio at the correct position.
            */
#ifndef FEATURE_WINCE
            lastAudioPosition = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
            bError = AudioPlayerIf::Stop(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
          }
          if (clip.bHasVideo)
          {
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /*  FEATURE_QTV_QOS_SELECTION */
#ifndef FEATURE_WINCE
            if (pActiveVideoPlayer)
            {
              pActiveVideoPlayer->Release(bError);
            }
            if (bError)
            {
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /*  FEATURE_QTV_QOS_SELECTION */
               VideoError();
            }
#endif   /*    FEATURE_WINCE   */
          }
          else
          {
            /* Video is not present, so notify app that the release is done */
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Video not present, release done");
#ifndef FEATURE_QTV_QOS_SELECTION
            Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE);
#else
#error code not present
#endif /*  FEATURE_QTV_QOS_SELECTION */
        }

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
        } /* if (qtv_cfg_enable_dsp_release) */
        bPaused=true;
#else /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

        //ignore command
        bPaused=true;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
        //change state if either audio or video paused.
        if (bPaused)
        {
          Notify(PAUSED);
        }
      break;;
      case SUSPENDED:
        bPaused=true;
        Notify(QtvPlayer::QTV_PLAYER_STATUS_PAUSED);

        break;

      case BUFFERING:
      case PLAYING:
      case REPOSITIONING:
#ifdef FEATURE_QTV_PSEUDO_STREAM
      case PLAYER_PSEUDO_PAUSE:
#endif //FEATURE_QTV_PSEUDO_STREAM
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      case PLAYER_HTTP_PAUSE:
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /*  FEATURE_QTV_QDSP_RELEASE_RESTORE && FEATURE_QTV_QOS_SELECTION */
         //
         //  Pause playing the clip
         //
#ifndef FEATURE_QTV_GENERIC_BCAST
         if (clip.bBcastFLO || clip.bBcastISDB)
         {
           return false;
         }
#else
         if (clip.bBcastGeneric)
         {
           if (clipInfo.bValid && clipInfo.info.RepositioningAllowed)
           {
             /* A repositionable Bcast stream is being suspended, it is either
             ** recorded media or is live media with TSB. Set bLiveBcast to
             ** false, since we can either resume from the paused point in the
             ** TSB (in which case we're no longer playing live) or resume at
             ** the live end of the buffer (in which case bLiveBcast is set to
             ** true once again.
             */
             clip.bLiveBcast = false;
           }
           else
         {
             /* If the stream is cannot be repositioned it is the pure live case,
             ** there is no TSB. Do not allow pause.
             */
           return false;
         }
         }
#endif /* FEATURE_QTV_GENERIC_BCAST */

         //pause streamer
         bStreamPaused=false;
         if (clip.bStreaming && streamer)
         {
            if (streamer->Pause(bError))
            {
               if (bError)
               {
                  //not sure whether streamer is paused or not
                  //so stop it.
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Pause");
                  StreamError();
               }
               //go ahead with the AV pause even if stream was stopped.
               bStreamPaused=true;
            }
         }

         //pause audio and video
         if (bStreamPaused || (!clip.bStreaming))
         {
            if (clip.bHasAudio)
            {
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
               if (qtv_cfg_enable_dsp_release)
         {
#ifndef FEATURE_WINCE
                 lastAudioPosition = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
                 bError = AudioPlayerIf::Stop(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
         bPaused=true;
         }
         else
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
#ifndef FEATURE_WINCE
              if (AudioPlayerIf::Pause(activeAudioPlayerId))
                  {
                     bPaused=true;
                  }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
               }
            if (clip.bHasVideo)
            {
#ifndef FEATURE_WINCE
               if (pActiveVideoPlayer && pActiveVideoPlayer->Pause(bError))
               {
                  if (bError)
                  {
                     VideoError();
                  } else
                  {
                     bPaused=true;
                  }
               }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
               if (qtv_cfg_enable_dsp_release)
               {
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /*  FEATURE_QTV_QOS_SELECTION */
#ifndef FEATURE_WINCE
                  if (pActiveVideoPlayer)
                  {
                    pActiveVideoPlayer->Release(bError);
                  }
                  if (bError)
                  {
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
                     VideoError();
                  }
#endif   /*    FEATURE_WINCE   */
               }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
            } /* if (clip.bHasVideo) */
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
            else
            {
              if (qtv_cfg_enable_dsp_release)
              {
                /* Video is not present, so notify app that the release is done */
                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Video not present, release done");
#ifndef FEATURE_QTV_QOS_SELECTION
                Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE);
#else
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
              }
            }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
         }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
         if (clip.bHasText)
         {
#ifndef FEATURE_WINCE
            if (textPlayer.Pause(bError))
            {
               if (bError)
               {
                  TimedTextError();
               } else
               {
                  bPaused=true;
               }
            }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
         }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

        //change state if either audio or video paused.
        if (bPaused)
        {
          Notify(PAUSED);
        }
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif
        break;
      default:
         //can't pause now
         break;
   }
   if (!bPaused)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error, Mpeg4Player::Pause failed");
   }


   if (bPaused)
   {
      qtv_event_clip_pause_type qtv_event_clip_pause_payload;
      qtv_event_clip_pause_payload.current_time = 0;

      //get elapsed time since beginning of clip.
    unsigned long temp_var=0;
      if (clip.bHasVideo
#ifndef FEATURE_WINCE
          && pActiveVideoPlayer
#endif   /*    FEATURE_WINCE   */
)
      {
#ifndef FEATURE_WINCE
         temp_var=(unsigned long)pActiveVideoPlayer->GetElapsedTime();
#endif   /*    FEATURE_WINCE   */
         qtv_event_clip_pause_payload.current_time =
         ZMAX(qtv_event_clip_pause_payload.current_time, temp_var);
      }

      if (clip.bHasAudio)
      {
#ifndef FEATURE_WINCE
      temp_var=(unsigned long)AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
         qtv_event_clip_pause_payload.current_time =
         ZMAX(qtv_event_clip_pause_payload.current_time,
        temp_var);
      }
#ifndef FEATURE_WINCE
      event_report_payload(EVENT_QTV_CLIP_PAUSE,
                           sizeof(qtv_event_clip_pause_type),
                           &qtv_event_clip_pause_payload);
#endif   /*    FEATURE_WINCE   */
      /* Reset the resume pending flag, this is to handle the case when mpeg4player about to resume and
      "PAUSE" from app is recieved */
      appControl.bResumePending = FALSE;
   }// if bPaused

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   if( (!qtv_cfg_enable_dsp_release) && bPaused)
   {
      playingBeforeVoiceCall = FALSE;
   }
#endif /*  FEATURE_QTV_QDSP_RELEASE_RESTORE */
   return bPaused;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Suspend

DESCRIPTION
  Pauses play and releases the DSP. Causes playerState transition to
  SUSPENDED.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Returns true if all suspend actions were completed, false otherwise.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::Suspend(QTV_SUSPEND_type *pEvent)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "Mpeg4Player::Suspend, State %d", playerState);

  bool bSuspended = false;
  bool bIgnore = false;
  bool bVideoDisabled = false;
  bool bAudioDisabled = false;
  bool bTextPaused = false;
  bool bStreamPaused;
  bool bError;

#ifdef FEATURE_WINCE
#error code not present
#endif   /*    FEATURE_WINCE   */

  if (clip.bStreaming && streamer && streamer->isReconnectionInProgress())
  {
    if (!pEvent->bSuspendIsExternal)
    {
      //we have reconnection in progress, we need to stop player instead of suspnd
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                  "Stopping player for suspend(internal due to incoming call),reconnection in progress");
      return Stop(NULL);
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                  "suspend(external) issued while reconnection in progress");
    }
  }

  switch (playerState)
  {
    /* Ignore suspend while idle, connecting or playback ready. */
    case IDLE :
    case CONNECTING :
    case OPENING:
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    case DL_READY:
    case DOWNLOADING:
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
              "Mpeg4Player::Suspend, ignored in state=%d", playerState);
      /* If we're streaming we need to stop the streamer and move it to IDLE state,
         so that when the app resumes we reconnect to the server and resume
         playing. If we don't stop the server, the streamer will continue to
         receive data and will start playing, even if playback is not supported. */
      if (clip.bStreaming && streamer)
      {
        streamer->Stop(bError);
      }

      /* Nothing to do. */
      bIgnore = true;
      break;
    } /* end case IDLE || CONNECTING */

    case PB_READY :
    {
      /* During streaming there is a long duration between someone hitting play
         in PB_READY state and the player actually going to BUFFERING. In this
         duration, the streamer is active and has requested the server for data
         and since no one asked it to pause it will go its merry way and start
         playing the downloaded data. */
      if (clip.bStreaming && streamer)
      {
        /* Attempt to pause the stream. */
        if (streamer->Pause(bError))
        {
          /* If unsuccessful, attempt to stop the stream. */
          if (bError)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Pause");
            StreamError();
          }
        }
      }
      /* There is no need for a state change, so no need to Notify, so ignore */
      bIgnore = true;
      break;
    } /* end case PB_READY */

    case SUSPENDED :
    {
      /* Valid state: Suspend while already suspended. Nothing to do. */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
              "Mpeg4Player::Suspend, ignored when already suspended");
      bIgnore = true;
      break;
    } /* end case SUSPENDED */

    /* Suspend while paused. */
    case PAUSED :
#ifdef FEATURE_QTV_PSEUDO_STREAM
    case PLAYER_PSEUDO_PAUSE :
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    case PLAYER_HTTP_PAUSE :
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
    {
      /* If clip has audio, store elapsed time and disable audio. */
      if (clip.bHasAudio)
      {
#ifndef FEATURE_WINCE
        audioPositionWhenSuspended = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);

        if (AudioPlayerIf::Disable(activeAudioPlayerId))
        {
          bAudioDisabled = true;
        }
#endif   /*    FEATURE_WINCE   */
      }
      else
      {
        bAudioDisabled = true;
      } /* end if clip.bHasAudio */

      /* If clip has video, disable video (it is already paused). */
      if (clip.bHasVideo
#ifndef FEATURE_WINCE
          && pActiveVideoPlayer
#endif   /*    FEATURE_WINCE   */
      )
      {
#ifndef FEATURE_WINCE
        pActiveVideoPlayer->Disable(bError);
#endif   /*    FEATURE_WINCE   */
        if (bError)
        {
          VideoError();
        }
        else
        {
          bVideoDisabled = true;
        }
      }
      else
      {
        /* No video. */
        bVideoDisabled = true;
      } /* end if (clip.bHasVideo) */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
      /*
      * If clip has text and text track has not done playing,
      * pause the text track
      */
      if( (clip.bHasText)
#ifndef FEATURE_WINCE
            &&(!textPlayer.IsDone())
#endif   /*    FEATURE_WINCE   */
            )
      {
#ifndef FEATURE_WINCE
        if (textPlayer.Pause(bError))
        {
          if (bError)
          {
            TimedTextError();
          }
          else
          {
            bTextPaused = true;
          }
        }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
      }
      else
      {
        //If clip does not have text or text track has ended/done playing,
        //set bTextPaused to true.
        bTextPaused = true;
      }  /* end if (clip.bHasText) */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

      bSuspended = bVideoDisabled && bAudioDisabled && bTextPaused;
      break;
    } /* end case PAUSED */

    /* Suspend while playing or buffering. */
    case BUFFERING :
    case PLAYING :
    case REPOSITIONING:
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    case DOWNLOADED_DATA_AVAIL:
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
    {
      /* Determine whether it is necessary to pause the stream.
         If clip is streaming or broadcast ... */
      bStreamPaused = false;
#ifndef FEATURE_QTV_GENERIC_BCAST
      if (clip.bBcastISDB || clip.bBcastFLO)
      {
        return false;
      }
#else
      if (clip.bBcastGeneric)
      {
        if (clipInfo.bValid && clipInfo.info.RepositioningAllowed)
        {
          /* A repositionable Bcast stream is being suspended, it is either
          ** recorded media or is live media with TSB. Set bLiveBcast to
          ** false, since we can either resume from the paused point in the
          ** TSB (in which case we're no longer playing live) or resume at
          ** the live end of the buffer (in which case bLiveBcast is set to
          ** true once again.
          */
          clip.bLiveBcast = false;

          /* There is no need to take any pause action. When Qtv is suspended
             it will stop pulling data from the media layer; live data while
             in that state will get dropped. */
          bStreamPaused = true;
        }
        else
        {
          /* If the stream is cannot be repositioned it is the pure live case,
          ** there is no TSB. Do not allow pause.
          */
          return false;
        }
      }
#endif /* FEATURE_QTV_GENERIC_BCAST */

      /* If clip is streaming and stream not already paused ... */
      if (clip.bStreaming && streamer )
      {
        /* Attempt to pause the stream. */
        if (streamer->Pause(bError))
        {
          /* If unsuccessful, attempt to stop the stream. */
          if (bError)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                         "Stream error from Pause");
            StreamError();
          }
          /* Assume that the stream was paused (or stopped). */
          bStreamPaused = true;
        }
      }

      /* If stream is paused or clip is not streaming ... */
      if (bStreamPaused || (!clip.bStreaming))
      {
        /* If clip has audio, store elapsed time and disable audio. */
        if (clip.bHasAudio)
        {
#ifndef FEATURE_WINCE
          audioPositionWhenSuspended = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);

          if (AudioPlayerIf::Disable(activeAudioPlayerId))
          {
            bAudioDisabled = true;
          }
#else
#error code not present
#endif /*    FEATURE_WINCE   */
        }
        else
        {
          bAudioDisabled = true;
        } /* end if clip.bHasAudio */


        /* If clip has video, pause then disable video. */
        if (clip.bHasVideo )
        {
#ifndef FEATURE_WINCE
       if (pActiveVideoPlayer)
       {
               pActiveVideoPlayer->Disable(bError);
               if (bError)
               {
                 VideoError();
               }
               else
               {
                 bVideoDisabled = true;
               }
          }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
        }

        else
        {
          /* No video. */
          bVideoDisabled = true;
        } /* end if (clip.bHasVideo) */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        /*
        * If clip has text and text track has not done playing,
        * pause the text track
        */
        if( (clip.bHasText)
#ifndef FEATURE_WINCE
               &&(!textPlayer.IsDone())
#endif   /*    FEATURE_WINCE   */
               )
        {
#ifndef FEATURE_WINCE
          if (textPlayer.Pause(bError))
          {
            if (bError)
            {
              TimedTextError();
            }
            else
            {
              bTextPaused = true;
            }
          }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
        }
        else
        {
          //If clip does not have text or text track has ended/done playing,
          //set bTextPaused to true.
          bTextPaused = true;
        }  /* end if (clip.bHasText) */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
      }

      bSuspended = bVideoDisabled && bAudioDisabled && bTextPaused;
      break;
    } /* end case PLAYING, BUFFERING */

    default:
    {
      /* Attempt to suspend in invalid state. */
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "Mpeg4Player::Suspend, invalid player state %d",
               playerState);
      break;
    } /* end default */
  }  /* end switch */

  /* If suspend is not ignored */
  if (!bIgnore)
  {
    if (bSuspended)
    {
      Notify(SUSPENDED);

      /* Generate a suspend event. */
      GenerateSuspendEvent();
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::Suspend failed");
    }
  }
  else // If suspend is ignored
  {
    if((playerState == IDLE)&&(pEvent->bSuspendIsExternal == true))
    {
      bIgnore = false ;
      /* The external pause has come in the wrong state
         Suspend must fail in this case. */
    }
    else
    {
       /* If suspend is ignored, notify status suspended. This is needed for
          Mediaplayer to change states properly. */
       Notify(QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED);
    }
  }

  return (bSuspended || bIgnore);
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Resume

DESCRIPTION
  Resumes playback only if the player was previously suspended while playing.
  Else notifies paused or idle depending on player state.
DEPENDENCIES
  None.

RETURN VALUE
  Returns true if resume resulted in no errors, false otherwise.

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::Resume(QTV_RESUME_type *pEvent)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "Mpeg4Player::Resume, playerState = %d", playerState);

  bool bResumed = false;

  if (playerState == SUSPENDED)
  {
    if (lastPlayerState == PLAYING ||
    lastPlayerState == BUFFERING ||
    lastPlayerState == REPOSITIONING)
    {
      /* Issue a play only if the player transitioned from active playing to
      ** suspended state, i.e. from PLAYING/BUFFERING/REPOSITIONING to
      ** SUSPENDED state.
      */
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   "Resume: App not suspended, issue resume");

      bResumed = PlayClip(-1, -1);
    }
    else
    {
      /* If the playerState is SUSPENDED and the lastPlayerState was not an
      ** active playing state implies that the player was paused/suspended by
      ** the application. Notify pause, so that the app knows it is in the
      ** enabled state.
      */
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "Resume: App not suspended, can't resume, lastPlayerState = %d",
                    lastPlayerState);

      Notify(QtvPlayer::QTV_PLAYER_STATUS_PAUSED);
    bResumed = true;
  }
  } /*If IDLE send IDLE Status notification to application*/
  else if (playerState == IDLE)
  {
    /* Notify idle, so that app knows Qtv is no longer suspended */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Resume: App not suspended, player is IDLE");

    Notify(QtvPlayer::QTV_PLAYER_STATUS_IDLE);
    bResumed = true;
  }
  else if (playerState == PB_READY)
  {
    /* Notify PB READY, so that app knows Qtv is no longer suspended */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Resume: App not suspended, player is in PB_READY");

    Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_READY);
    bResumed = true;
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                  "Resume: App not suspended, nothing to do, lastPlayerState = %d",
                  lastPlayerState);

    bResumed = true;
  }

  QTV_USE_ARG1(pEvent);
  return bResumed;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION
  Mpeg4Player::FreezePlayer

DESCRIPTION
  Momentarily releases the DSP. .

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::FreezePlayer(unsigned int nResult)
{
#ifndef FEATURE_WINCE
  if(nResult & QTV_MEDIA_TRACK_LIST_AUDIO_RESET)
  {
    // Audio DSP is put to sleep independent of audio mute
    if((clip.bHasAudio || IsAudioMuted()) &&
       !AudioPlayerIf::IsDone(activeAudioPlayerId))
    {
      // For audio Reset we need to flush the audio pipeline too.
      // This is to make sure that we push the new data after update
      if(!AudioPlayerIf::Suspend(activeAudioPlayerId))
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "FreezePlayer : Audio suspend failed");
        return false;
      }

      if(!AudioPlayerIf::Disable(activeAudioPlayerId))
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "FreezePlayer : Audio disable failed");
        return false;
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_HIGH,
                   "FreezePlayer : Audio disable success");

    }
  }

  if(nResult & QTV_MEDIA_TRACK_LIST_VIDEO_RESET)
  {
    /* If clip has video, disable video (it is already paused). */
    // Video DSP is put to sleep independent of video mute
    if((clip.bHasVideo || IsVideoMuted()) &&
       (pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone())))
    {
      bool bError;
      pActiveVideoPlayer->Disable(bError);
      if(bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "FreezePlayer : Video disable failed");
        return false;
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_HIGH,
                   "FreezePlayer : Video disable success");
    }
  }

#endif   /*    FEATURE_WINCE   */
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::AudioMediaReposition

DESCRIPTION
  Resumes audio play after state based freeze logic is applied. Uses the comparison result figure out the needed
  actions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::AudioMediaReposition(unsigned int  nResult,
                                       unsigned long   start,
                                       long             stop)
{
  bool bError = false;

  if((nResult & QTV_MEDIA_TRACK_LIST_AUDIO_ADDED) ||
     (nResult & QTV_MEDIA_TRACK_LIST_AUDIO_RESET))
  {
    // When the audio track is added we need to set the position correctly
    if(nResult & QTV_MEDIA_TRACK_LIST_AUDIO_ADDED)
    {
      (void)clip.pVideoMpeg4Playback->SetAudioPosition(start, bError, start);
      if(bError)
      {
        //When video reposition fails return false
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "ActivateAudioDSP:Reposition failed");
        return false;
      }
    }
#ifndef FEATURE_WINCE
    AudioPlayerIf::SetPlayTimes(activeAudioPlayerId,start,stop);
#endif   /*    FEATURE_WINCE   */

    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateAudioDSP:Reposition Success");

    // Just to avoid the audio mute scenario we check the audio component flag here.
    if(clip.bHasAudio)
    {
#ifndef FEATURE_WINCE
      m_mediaSync.SetPlaybackOffset(AVSync::AudioAV, (int)ZUtils::Clock(), start);
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                    "Audio Player to be resumed but updated the OFFSET for TS %ld",
                    start);
    }
  }
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::AudioMediaPrep

DESCRIPTION
  Resumes Audio play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::AudioMediaPrep(unsigned int nResult)
{
  bool bRet=true;

  if((nResult & QTV_MEDIA_TRACK_LIST_AUDIO_ADDED) ||
     (nResult & QTV_MEDIA_TRACK_LIST_AUDIO_RESET))
  {
    //Prep the audio player.
#ifndef FEATURE_WINCE
    bRet = AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,false,
                               GetAudioMedia(),clip.bHasVideo,GetAudioCodecType());
    if(!bRet)
    {
      //When audio PREP fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateAudioDSP:Prep failed");
      return false;
    }

    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateAudioDSP:Prep Successful");

  }
#endif   /*    FEATURE_WINCE   */
  return true;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::AudioMediaStart

DESCRIPTION
  Resumes audio play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::AudioMediaStart(unsigned int nResult,long stop)
{
  bool bRet=true;


#ifndef FEATURE_WINCE
  if((nResult & QTV_MEDIA_TRACK_LIST_AUDIO_ADDED) ||
     (nResult & QTV_MEDIA_TRACK_LIST_AUDIO_RESET))
  {
    //Start is always performed immaterial of audio track addition or DSP Reset
    bRet = AudioPlayerIf::Start(activeAudioPlayerId, false);
    if(!bRet)
    {
      //When audio START  fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateAudioDSP: Start failed");
      return false;
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateAudioDSP:Start Successful");
    appControl.bDoneAudioBuffering = false;
    // If the mute flag is set then definitely clip has audio track selected , not vice versa.
    if(IsAudioMuted())
    {
      // If audio is muted , pause the audio immediately. This should not have any severe effect
      //Start is always performed immaterial of audio track addition or DSP Reset
      bRet = AudioPlayerIf::Pause(activeAudioPlayerId);
      if(!bRet)
      {
        //When audio START  fails, return false
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "ActivateAudioDSP: Pause failed");
        return false;
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_HIGH,
                   "ActivateAudioDSP:Pause Successful");

    }
  }
  else if(clip.bHasAudio && !AudioPlayerIf::IsDone(activeAudioPlayerId))
  {
    //Resume is performed if the audio player is only paused. i.e No Reset or addition case.
    bRet = AudioPlayerIf::Resume(activeAudioPlayerId, stop);
    if(!bRet)
    {
      //When audio START  fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateAudioDSP: Resume failed");
      return false;
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateAudioDSP:Resume Successful");

  }
#endif   /*    FEATURE_WINCE   */
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::TextMediaReposition

DESCRIPTION
  Resumes Text play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::TextMediaReposition(unsigned int  nResult,
                                      unsigned long   start,
                                      long             stop)
{
  bool bError=false;
  if(nResult & QTV_MEDIA_TRACK_LIST_TEXT_ADDED ||
     nResult & QTV_MEDIA_TRACK_LIST_TEXT_RESET)
  {
    // Add the text track if the comparison result says so
    if(nResult & QTV_MEDIA_TRACK_LIST_TEXT_ADDED)
    {
      (void)clip.pVideoMpeg4Playback->SetTextPosition(start, bError, start);
      if(bError)
      {
        //When text reposition fails return false
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "ActivateTextDSP:Reposition failed");
        return false;
      }
#ifndef FEATURE_WINCE
      textPlayer.SetPlayTimes(start,stop);
#endif   /*    FEATURE_WINCE   */
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_HIGH,
                   "ActivateTextDSP:Reposition Success");

    }
    // Set the Playback offset if needed.
    if(!clip.bHasAudio && !clip.bHasVideo && clip.bHasText)
    {
#ifndef FEATURE_WINCE
      m_mediaSync.SetPlaybackOffset(AVSync::TextAV, (int)ZUtils::Clock(), start);
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                    "Text Player to be resumed but updated the OFFSET for TS %ld",
                    start);
    }
  }
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::TextMediaPrep

DESCRIPTION
  Resumes Text play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::TextMediaPrep(unsigned int nResult)
{
  bool bRet=true;

  // Add the text track if the comparison result says so
  if(nResult & QTV_MEDIA_TRACK_LIST_TEXT_ADDED)
  {

    //Prep text
#ifndef FEATURE_WINCE
    bRet = textPlayer.Prep(playbackID, false,clip.pVideoMpeg4Playback);
#endif   /*    FEATURE_WINCE   */
    if(!bRet)
    {
      //When text PREP fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateTextDSP:Prep failed");
      return false;
    }

    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateTextDSP:Prep Successful");

  }

  return true;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::TextMediaStartOrResume

DESCRIPTION
  Resumes text play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::TextMediaStartOrResume(unsigned int nResult,
                                         long            stop)
{
  bool bRet  =true;
  bool bError=false;

  // Add the text track if the comparison result says so
  if(nResult & QTV_MEDIA_TRACK_LIST_TEXT_ADDED)
  {
#ifndef FEATURE_WINCE
    bRet = textPlayer.Start(bError, false);
    if(!bRet || bError)
    {
      //When text START  fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateTextDSP: Text Start failed");
      return false;
    }
#endif   /*    FEATURE_WINCE   */
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateTextDSP:Text Start Successful");

  }
  else if(clip.bHasText && !textPlayer.IsDone())
  {
#ifndef FEATURE_WINCE
    bRet = textPlayer.Resume(bError,stop);
    if(!bRet || bError)
    {
      //When video Resume  fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateTextDSP:  Resume failed");
      return false;
    }
#endif   /*    FEATURE_WINCE   */
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "ActivateTextDSP: Resume Successful");
  }
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ActivateVideoDSPReposition

DESCRIPTION
  Resumes video play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::VideoMediaReposition(unsigned int  nResult,
                                       unsigned long  &start,
                                       long             stop)
{
  bool bError=false;
  if(nResult & QTV_MEDIA_TRACK_LIST_VIDEO_ADDED ||
     nResult & QTV_MEDIA_TRACK_LIST_VIDEO_RESET )
  {
    // Add the video track if the comparison result says so
    if(nResult & QTV_MEDIA_TRACK_LIST_VIDEO_ADDED)
    {
      long vidstart;
      // Reposition and set play times
      vidstart = clip.bStillImage ? 0 : start;
      start = clip.pVideoMpeg4Playback->SetVideoPosition(vidstart, bError, vidstart);
      if(bError)
      {
        //When video reposition fails return false
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "ActivateVideoDSP:Reposition failed");
        return false;
      }
#ifndef FEATURE_WINCE
      if (pActiveVideoPlayer)
      {
        pActiveVideoPlayer->SetPlayTimes(vidstart,stop);
      }
#endif   /*    FEATURE_WINCE   */

      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_HIGH,
                   "ActivateVideoDSP:Reposition Success");

    }
    if(!clip.bHasAudio && clip.bHasVideo && !clip.bStillImage)
    {
#ifndef FEATURE_WINCE
      m_mediaSync.SetPlaybackOffset(AVSync::VideoAV, (int)ZUtils::Clock(), start);
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
      QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                    "Video Player to be resumed but updated the OFFSET for TS %ld",
                    start);
    }
  }
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::VideoMediaPrepOrEnable

DESCRIPTION
  Resumes video play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::VideoMediaPrepOrEnable(unsigned int nResult)
{
  bool bRet   = true;
  bool bError = false;

  // Add the video track if the comparison result says so
  if(nResult & QTV_MEDIA_TRACK_LIST_VIDEO_ADDED)
  {
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
    // Preparse before prep. Otherwise prep is failing.
      (void)pActiveVideoPlayer->PreParseHeader(clip.pVideoMpeg4Playback, false, GetAudioCodecType());
    //Prep video
       bRet = pActiveVideoPlayer->Prep(playbackID, clip.pVideoMpeg4Playback, bError,
                            false, GetAudioCodecType());
    }
    if(!bRet || bError)
    {
      //When video PREP fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateVideoDSP:Prep failed");
      return false;
    }
#endif   /*    FEATURE_WINCE   */
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateVideoDSP:Prep Successful");

  }
  else if(nResult & QTV_MEDIA_TRACK_LIST_VIDEO_RESET)
  {
    // Video is currently disabled; need to enable before resume.
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
      pActiveVideoPlayer->Enable(bError, GetAudioCodecType());
    }
    if(bError)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateVideoDSP: Enable video failed");
      return false;
    }
#endif   /*    FEATURE_WINCE   */
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateVideoDSP: Enable Successful");
  }

  return true;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::VideoMediaStartOrResume

DESCRIPTION
  Resumes video play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */

bool Mpeg4Player::VideoMediaStartOrResume(unsigned int nResult,
                                          long            stop)
{
  bool bRet   = true;
  bool bError = false;

  // Add the video track if the comparison result says so
  if(nResult & QTV_MEDIA_TRACK_LIST_VIDEO_ADDED)
  {
#ifndef FEATURE_WINCE
     if (pActiveVideoPlayer)
     {
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
       bRet = pActiveVideoPlayer->Start(bError, false);
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
     }
    if(!bRet || bError)
    {
      //When video START  fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateVideoDSP: Video Start failed");
      return false;
    }
#endif   /*    FEATURE_WINCE   */
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_HIGH,
                 "ActivateVideoDSP:Video Start Successful");

    appControl.bReceivedFirstFrame = false;
  }

  else if(clip.bHasVideo
#ifndef FEATURE_WINCE
&& pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone())
#endif   /*    FEATURE_WINCE   */
)
  {
#ifndef FEATURE_WINCE
    bRet = pActiveVideoPlayer->Resume(bError,stop);
    if(!bRet || bError)
    {
      //When video Resume  fails, return false
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "ActivateVideoDSP: Video Resume failed");
      return false;
    }
#endif   /*    FEATURE_WINCE   */
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "ActivateVideoDSP:Video Resume Successful");
  }
  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ActivatePlayer

DESCRIPTION
  Resumes play after state based freeze logic is applied.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::ActivatePlayer(unsigned int nResult,unsigned long vidstart)
{
  bool bRet   = true;
  long start=0;
  long stop =0;


  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ActivatePlayer" );

  if(!GetPlayTimes(start,stop))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Get Play Times failed");
    return false;
  }



  //reset the audio/video sync point
  m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

  // Reposition all applicable players
  // Logical AND does not invoke the function if the first operand is false.
  start=vidstart;
  // In live media repositioning is not supported.
  if(clip.bBcastGeneric && !clip.bLiveBcast)
  {
     bRet = VideoMediaReposition(nResult,vidstart,stop);
     if(clip.bStillImage)
     {
       // For still images vidstart is modified to zero. Henc update it back with the original for other tracks.
       vidstart = start;
     }
     bRet = (bRet) ? AudioMediaReposition(nResult,vidstart,stop):false;
     bRet = (bRet) ?  TextMediaReposition(nResult,vidstart,stop):false;
  }

  // Prep all the applicable Players
  bRet = (bRet) ? VideoMediaPrepOrEnable(nResult):false;
  bRet = (bRet) ? AudioMediaPrep(nResult):false;
  bRet = (bRet) ?  TextMediaPrep(nResult):false;

  // Start all the applicable players
  bRet = (bRet) ?  TextMediaStartOrResume(nResult,stop):false;
  bRet = (bRet) ?      AudioMediaStart(nResult,stop):false;
  bRet = (bRet) ? VideoMediaStartOrResume(nResult,stop):false;

  return bRet;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::FreezeOrActivate

DESCRIPTION
  Momentarily releases the DSP. Causes playerState transition to
  SUSPENDED.

DEPENDENCIES
  This action is performed as part of the player thread. This might have some consequences to the terminate.
  The waiting time for the player thread to reach idle might have to be increased.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::StateBasedFreezeOrActivate(bool bFreeze, unsigned int nResult,unsigned long start)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "Mpeg4Player::StateBasedFreezeOrActivate, State %d", playerState);

  bool bRet = true;
  if(IsPlayerPaused())
  {
    // In paused state dsp activation deferred after dynamic track list update
    // Hence there is no action performed to activate the dsp while the player is paused
    if(bFreeze)
    {
      // Disable all audio/video dsp's and move to suspended state.
      bRet = FreezePlayer(QTV_MEDIA_TRACK_LIST_RESET);
      // Move the state to SUSPENDED
      Notify(SUSPENDED);
    }
  }
  else if(IsPlayerActive())
  {
    if(bFreeze)
    {
      /* Pause all the applicable decoders before switching the DSP */
      bRet = PauseMedia();
      if(bRet)
      {
        bRet = FreezePlayer(nResult);
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "StateBasedFreezeOrActivate, Pause media failed ");
      }
    }
    else
    {
      bRet = ActivatePlayer(nResult,start);
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,
                  QTVDIAG_PRIO_HIGH,
                  "StateBasedFreezeOrActivate, ignored in state=%d",
                  playerState);
  }
  return bRet;
}

#endif

/* ======================================================================
FUNCTION
  Mpeg4Player::GenerateSuspendEvent

DESCRIPTION
  Generates a suspend event.
  Currently generates pause event instead because suspend event
  is unavailable.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::GenerateSuspendEvent()
{
  qtv_event_clip_pause_type qtv_event_clip_pause_payload;
  qtv_event_clip_pause_payload.current_time = 0;

  unsigned long mediaElapsedTime =0;
  #ifndef FEATURE_WINCE
  if (clip.bHasVideo && pActiveVideoPlayer)
  {
    mediaElapsedTime = pActiveVideoPlayer->GetElapsedTime();
#endif   /*    FEATURE_WINCE   */
    qtv_event_clip_pause_payload.current_time = ZMAX( qtv_event_clip_pause_payload.current_time, mediaElapsedTime );
 #ifndef FEATURE_WINCE
  }
#endif   /*    FEATURE_WINCE   */

  if (clip.bHasAudio)
  {
#ifndef FEATURE_WINCE
    mediaElapsedTime = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
    qtv_event_clip_pause_payload.current_time = ZMAX( qtv_event_clip_pause_payload.current_time, mediaElapsedTime );
  }
#ifndef FEATURE_WINCE
  event_report_payload(EVENT_QTV_CLIP_PAUSE,
                       sizeof(qtv_event_clip_pause_type),
                       &qtv_event_clip_pause_payload);
#endif   /*    FEATURE_WINCE   */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::RestoreResumeAudio

DESCRIPTION
  Restores and resumes audio.  Used when the playerState is
  SUSPENDED and audio is to be resumed.
  On entry, audio is disabled.  It is started with play position set
  to the last video play position if video is present or the the last
  audio position if not.  Setting the play position in this way makes
  stop/start equivalent to pause/resume.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  bool - true if successful, false otherwise

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::RestoreResumeAudio(long stop)
{
  bool bResumed = false;
  bool bRestart = false;
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

   QtvPlayer::PlayerStateRecordT ps;
   unsigned long nPlayTimel;
   bool bSetAudError=false;
   GetPlayerState(ps);

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "RestoreResumeAudio");

  /* This is a fresh start for audio, we want it to buffer, so set
     bDoneAudioBuffering to false */
  appControl.bDoneAudioBuffering = false;

  /* Prep the audio player. Audio is currently PAUSED_DISABLED, so this is a
     fresh start not a restart. */
#ifndef FEATURE_WINCE
  bResumed = AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,bRestart,
                                 GetAudioMedia(),clip.bHasVideo,GetAudioCodecType());
#endif   /*    FEATURE_WINCE   */

  /* If unsuccessful ... */
  if (!bResumed)
  {
    /* Generate error. */
     AudioError();
  }

  /* Otherwise (audio prepped) ... */
  else
  {
    long audioPlayTime = ps.playbackMsec;

    /* If clip has video and previous concurent state is AUDIO CONCURENT then we
       have to update the audio playback times with the Video playback time so that
       once the call is disconnected audio can catch up with the video*/
#ifndef FEATURE_WINCE
    if(clip.bHasVideo && (prevAudioConcState == AUDIO_CONCURRENT))
    {
            /* During incall video we should move the audio media pointers to video inorder
             to audio to catch up with the video*/
        if (clip.pAudioMpeg4Playback )
        {
           nPlayTimel = pActiveVideoPlayer->GetElapsedTime();
           audioPlayTime = clip.pAudioMpeg4Playback->SetAudioPosition(ps.playbackMsec, bSetAudError, nPlayTimel);
         }
    }
    AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, audioPlayTime, stop);
    /* Now start audio. */
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
    bResumed = AudioPlayerIf::Start(activeAudioPlayerId, bRestart);
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
#endif   /*    FEATURE_WINCE   */
  }

  return bResumed;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::UpdateAudioConcState

DESCRIPTION
  Updates audio concurrency state when a player state transition to
  playing occurs.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::UpdateAudioConcState()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Mpeg4Player::UpdateAudioConcState");

#ifndef FEATURE_QTV_DISABLE_CONC_MGR

  switch (audioSource)
  {
    case QtvPlayer::AUDIO_SOURCE_AUTO:
    {
      if (qtv_conc_mgr::get_play_mode(this, QTV_PLAYER_MIME_TYPE)
          == qtv_conc_mgr::PLAY_NORMAL)
      {
        if (playerAudioConcState == AUDIO_CONCURRENT)
        {
            prevAudioConcState = AUDIO_CONCURRENT;
        }
  playerAudioConcState = AUDIO_NOT_CONCURRENT;
      }
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
      else if (qtv_conc_mgr::get_play_mode(this, QTV_PLAYER_MIME_TYPE)
               == qtv_conc_mgr::PLAY_NOT_SUPPORTED)
      {
        playerAudioConcState = AUDIO_CONCURRENCY_NOT_SUPPORTED;
        prevAudioConcState = AUDIO_CONCURRENCY_NOT_SUPPORTED;
      }
      break;

    }

#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */

    default:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "Unknown audio source = %d", audioSource);
  } /* switch(AudioSource) */

  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                "Audio concurrency state = %d", playerAudioConcState);
#endif
}

#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */

#if defined (FEATURE_FILE_FRAGMENTATION) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/* ======================================================================
FUNCTION
  Mpeg4Player::PauseAudio

DESCRIPTION
//
//  Pause
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PauseAudio(QTV_PAUSE_AUDIO_type *)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::PauseAudio" );

   bool bPaused=false;

   switch (playerState)
   {
      case PAUSED:
      case SUSPENDED:
         //ignore command
         bPaused=true;
         break;

      case BUFFERING:
      case PLAYING:
#ifdef FEATURE_QTV_PSEUDO_STREAM
      case PLAYER_PSEUDO_PAUSE:
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      case PLAYER_HTTP_PAUSE:
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
         //
         //  Pause playing the clip
         //
      {
#ifndef FEATURE_QTV_GENERIC_BCAST
         if (clip.bBcastFLO || clip.bBcastISDB)
#else
         if (clip.bLiveBcast)
#endif
         {
           return false;
         }

         bool bError = false;

         //pause streamer
         bool bStreamPaused = false;
         if (clip.bStreaming && streamer )
         {
            if (streamer->Pause(bError))
            {
               if (bError)
               {
                  //not sure whether streamer is paused or not
                  //so stop it.
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Pause");
                  StreamError();
               }
               //go ahead with the AV pause even if stream was stopped.
               bStreamPaused=true;
            }
         }

         //pause audio and video
         if (bStreamPaused || (!clip.bStreaming))
         {
            if (clip.bHasAudio)
            {
#ifndef FEATURE_WINCE
              if ( AudioPlayerIf::Pause(activeAudioPlayerId) )
                  {
                     bPaused=true;
                  }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
               }
            }
        //change state if either audio or video paused.
        if (bPaused)
        {
          /*Set PauseAudio in FileBase*/
          if( clip.bHasAudio && clip.pAudioMpeg4Playback)
          {
#ifdef FEATURE_FILE_FRAGMENTATION
            clip.pAudioMpeg4Playback->setPausedAudio(TRUE);
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
            // Set state to HTTP_PAUSE state
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "set PLAYER_STATE_HTTP_PAUSE in PauseAudio");
           // event_report(EVENT_QTV_PLAYER_STATE_HTTP_PAUSE);
            player_http_pause_entry_time = pvGetTickCount();
            Notify(PLAYER_HTTP_PAUSE);
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Paused Audio From App");
          }

#ifdef FEATURE_QTV_PSEUDO_STREAM
        if(!IsVideoPlaying() || (clip.pVideoMpeg4Playback && clip.pVideoMpeg4Playback->getPausedVideo()))
          {
            if(!IsTextPlaying() || (clip.pTextMpeg4Playback && clip.pTextMpeg4Playback->getPausedText()))
            {
              // Set state to PSEUDO_PAUSE state
        if(playerState != PLAYER_PSEUDO_PAUSE)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "set PLAYER_STATE_PSEUDO_PAUSE");
#ifndef FEATURE_WINCE
          event_report(EVENT_QTV_PLAYER_STATE_PSEUDO_PAUSE);
#endif   /*    FEATURE_WINCE   */
          player_pp_entry_time = pvGetTickCount();
          Notify(PLAYER_PSEUDO_PAUSE);
        }
            }
          }
#endif /* FEATURE_QTV_PSEUDO_STREAM */
        }
      }
        break;

      default:
         //can't pause now
         break;
   }
   if (!bPaused)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error, Mpeg4Player::PauseAudio failed");
   }

   return bPaused;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PauseVideo

DESCRIPTION
//
//  Pause
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PauseVideo(QTV_PAUSE_VIDEO_type *)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::PauseVideo" );

   bool bPaused=false;

   switch (playerState)
   {
      case PAUSED:
      case SUSPENDED:
         //ignore command
         bPaused=true;
         break;

      case BUFFERING:
      case PLAYING:
#ifdef FEATURE_QTV_PSEUDO_STREAM
      case PLAYER_PSEUDO_PAUSE:
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      case PLAYER_HTTP_PAUSE:
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
      {
         //
         //  Pause playing the clip
         //
#ifndef FEATURE_QTV_GENERIC_BCAST
         if (clip.bBcastFLO || clip.bBcastISDB)
#else
         if (clip.bLiveBcast)
#endif
         {
           return false;
         }


         bool bError = false;

         //pause streamer
         bool bStreamPaused = false;
         if (clip.bStreaming && streamer )
         {
            if (streamer->Pause(bError))
            {
               if (bError)
               {
                  //not sure whether streamer is paused or not
                  //so stop it.
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Pause");
                  StreamError();
               }
               //go ahead with the AV pause even if stream was stopped.
               bStreamPaused=true;
            }
         }

         //pause audio and video
         if (bStreamPaused || (!clip.bStreaming))
         {
            if (clip.bHasVideo)
            {
#ifndef FEATURE_WINCE
               if (pActiveVideoPlayer && pActiveVideoPlayer->Pause(bError))
               {
                  if (bError)
                  {
                     VideoError();
                  }
                  else
                  {
                     bPaused=true;
                  }
               }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
            }
         }
        //change state if either audio or video paused.
        if (bPaused)
        {
          /*Set PauseVideo in FileBase*/
          if( clip.bHasVideo && clip.pVideoMpeg4Playback)
          {
#ifdef FEATURE_FILE_FRAGMENTATION
            clip.pVideoMpeg4Playback->setPausedVideo(TRUE);
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
            // Set state to HTTP_PAUSE state
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "set PLAYER_STATE_HTTP_PAUSE in PauseVideo");
            //event_report(EVENT_QTV_PLAYER_STATE_HTTP_PAUSE);
            player_http_pause_entry_time = pvGetTickCount();
            Notify(PLAYER_HTTP_PAUSE);
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Paused Video From App");
          }

#ifdef FEATURE_QTV_PSEUDO_STREAM
          if(!IsAudioPlaying() || (clip.pAudioMpeg4Playback && clip.pAudioMpeg4Playback->getPausedAudio()))
          {
            if(!IsTextPlaying() || (clip.pTextMpeg4Playback && clip.pTextMpeg4Playback->getPausedText()))
            {
              // Set state to PSEUDO_PAUSE state
        if(playerState != PLAYER_PSEUDO_PAUSE)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "set PLAYER_STATE_PSEUDO_PAUSE");
#ifndef FEATURE_WINCE
          event_report(EVENT_QTV_PLAYER_STATE_PSEUDO_PAUSE);
#endif   /*    FEATURE_WINCE   */
          player_pp_entry_time = pvGetTickCount();
          Notify(PLAYER_PSEUDO_PAUSE);
        }
            }
          }
#endif /* FEATURE_QTV_PSEUDO_STREAM */
        }
      }
        break;

      default:
         //can't pause now
         break;
   }
   if (!bPaused)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error, Mpeg4Player::PauseVideo failed");
   }

   return bPaused;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PauseText

DESCRIPTION
//
//  Pause
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::PauseText(QTV_PAUSE_TEXT_type *)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::PauseText" );

   bool bPaused=false;

   switch (playerState)
   {
      case PAUSED:
      case SUSPENDED:
         //ignore command
         bPaused=true;
         break;

      case BUFFERING:
      case PLAYING:
#ifdef FEATURE_QTV_PSEUDO_STREAM
      case PLAYER_PSEUDO_PAUSE:
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      case PLAYER_HTTP_PAUSE:
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
      {
         //
         //  Pause playing the clip
         //
#ifndef FEATURE_QTV_GENERIC_BCAST
         if (clip.bBcastFLO || clip.bBcastISDB)
#else
         if (clip.bLiveBcast)
#endif
         {
           return false;
         }


         bool bError = false;

         //pause streamer
         bool bStreamPaused = false;
         if (clip.bStreaming && streamer )
         {
            if (streamer->Pause(bError))
            {
               if (bError)
               {
                  //not sure whether streamer is paused or not
                  //so stop it.
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Pause");
                  StreamError();
               }
               //go ahead with the AV pause even if stream was stopped.
               bStreamPaused=true;
            }
         }

         //pause audio and video
         if (bStreamPaused || (!clip.bStreaming))
         {
            if (clip.bHasText)
            {
#ifndef FEATURE_WINCE
               if (textPlayer.Pause(bError))
               {
                  if (bError)
                  {
                     TimedTextError();
                  } else
                  {
                     bPaused=true;
                  }
               }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
            }
         }
        //change state if either audio or video paused.
        if (bPaused)
        {
           /*Set PauseText in FileBase*/
          Media * pTextMedia = GetTextMedia();
          if( clip.bHasText && pTextMedia)
          {
#ifdef FEATURE_FILE_FRAGMENTATION
            pTextMedia->setPausedText(TRUE);
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
            // Set state to HTTP_PAUSE state
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "set PLAYER_STATE_HTTP_PAUSE in PauseText");
            //event_report(EVENT_QTV_PLAYER_STATE_HTTP_PAUSE);
            player_http_pause_entry_time = pvGetTickCount();
            Notify(PLAYER_HTTP_PAUSE);
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Paused Text From App");
          }

#ifdef FEATURE_QTV_PSEUDO_STREAM
          if(!IsVideoPlaying() || (clip.pVideoMpeg4Playback && clip.pVideoMpeg4Playback->getPausedVideo()))
          {
            if(!IsAudioPlaying() || (clip.pAudioMpeg4Playback && clip.pAudioMpeg4Playback->getPausedAudio()))
            {
              // Set state to PSEUDO_PAUSE state
              if(playerState != PLAYER_PSEUDO_PAUSE)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "set PLAYER_STATE_PSEUDO_PAUSE");
#ifndef FEATURE_WINCE
          event_report(EVENT_QTV_PLAYER_STATE_PSEUDO_PAUSE);
#endif   /*    FEATURE_WINCE   */
          player_pp_entry_time = pvGetTickCount();
          Notify(PLAYER_PSEUDO_PAUSE);
        }
            }
          }
#endif /* FEATURE_QTV_PSEUDO_STREAM */
        }
      }
        break;

      default:
         //can't pause now
         break;
   }
   if (!bPaused)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Error, Mpeg4Player::PauseText failed");
   }

   return bPaused;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ResumeAudio

DESCRIPTION
//
//  Pause
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::ResumeAudio( QTV_RESUME_AUDIO_type * )
{
  QtvPlayer::PlayerStateRecordT ps;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::ResumeAudio" );

  GetPlayerState(ps);
  long start,stop;

  SetPlayTimes(-1, -1, true);

  if (!GetPlayTimes(start,stop))
  {
    Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
    return false;
  }

  bool bError=false;

  //resume streamer.
  if (clip.bStreaming && streamer )
  {
    if (streamer->Resume(bError))
    {
      //report an error.
      if (bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Resume");
        StreamError();
      }
    }
    //if it's not streaming just resume anyway, the AV will
    //run out of data soon enough.
  }

  bool bResumed = false;


  if(appControl.bStepAVSyncNeeded)
  {
    long PlayTime = 0;
    long AudioPlayTime;
    /* now synchronize audio and text with video */
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
       PlayTime = pActiveVideoPlayer->GetElapsedTime();
       pActiveVideoPlayer->SetPlayTimes(PlayTime, stop);
    }
#endif   /*    FEATURE_WINCE   */
    if(clip.bHasAudio)
    {
      /* Below temp variable and if condition is added to fix Klocwork warnings */
      Media *tempMedia = GetAudioMedia();
      if(tempMedia != NULL)
      {
        AudioPlayTime = tempMedia->SetAudioPosition(PlayTime, bError, ps.playbackMsec);
#ifndef FEATURE_WINCE
        AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, AudioPlayTime, stop);
        /* this will flush audio buffered in CMX */
        bError = AudioPlayerIf::Suspend(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
      }
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if(clip.bHasText)
    {
      bool lBError = false;

      Media *txtMedia = GetTextMedia();
      if (NULL != txtMedia)
      {
        (void)txtMedia->SetTextPosition(PlayTime, lBError, ps.playbackMsec);
      }
#ifndef FEATURE_WINCE
      textPlayer.SetPlayTimes(PlayTime, stop);
      (void)textPlayer.Suspend(lBError);
#endif   /*    FEATURE_WINCE   */
    }
#endif
    (void)PrepAudio();
  }

   //reset the audio/video sync point
  m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

  if( clip.bHasAudio )
  {
    if(appControl.bStepAVSyncNeeded)
    {
      appControl.bStepAVSyncNeeded = false;
      bResumed = StartAudio();
    }
    else
    {
#ifndef FEATURE_WINCE
      if ( AudioPlayerIf::Resume(activeAudioPlayerId, stop) )
        {
          bResumed=true;
        }
      else
#endif   /*    FEATURE_WINCE   */
      {
        /* audioPlayer.Resume returns false.  This means that Resume cannot be
         * performed due to Resume being issued while the audioPlayer is playing.
         * This can happen if the Media Player and Qtv states get out of sync.
         * This is not a serious error so just indicate that Qtv resumed which
         * has the effect of ignoring the Resume.
         */
        bResumed=true;
      }
    }
#ifdef FEATURE_FILE_FRAGMENTATION
    if( clip.bHasAudio && clip.pAudioMpeg4Playback && bResumed)
    {
      clip.pAudioMpeg4Playback->setPausedAudio(FALSE);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Resumed Audio From App");
    }
#endif  /* FEATURE_FILE_FRAGMENTATION */
  }
  if (!bResumed) QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::ResumeAudio failed");
  return bResumed;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ResumeVideo

DESCRIPTION
//
//  ResumeVideo
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::ResumeVideo( QTV_RESUME_VIDEO_type * )
{
  QtvPlayer::PlayerStateRecordT ps;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::ResumeVideo" );
  GetPlayerState(ps);


  long start,stop;

  SetPlayTimes(-1, -1, true);

  if (!GetPlayTimes(start,stop))
  {
    Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
    return false;
  }

  bool bError=false;

  //resume streamer.
  if (clip.bStreaming && streamer )
  {
    if (streamer->Resume(bError))
    {
      //report an error.
      if (bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Resume");
        StreamError();
      }
    }
    //if it's not streaming just resume anyway, the AV will
    //run out of data soon enough.
  }

  //reset the audio/video sync point
  m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

  bool bResumed = false;

  if(appControl.bStepAVSyncNeeded)
  {
    long PlayTime = 0;
    long AudioPlayTime = 0;
    /* now synchronize audio and text with video */
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
       PlayTime = pActiveVideoPlayer->GetElapsedTime();
       pActiveVideoPlayer->SetPlayTimes(PlayTime, stop);
    }
#endif   /*    FEATURE_WINCE   */
    if(clip.bHasAudio)
    {
      if (clip.pAudioMpeg4Playback )
      {
         AudioPlayTime = clip.pAudioMpeg4Playback->SetAudioPosition(PlayTime, bError, ps.playbackMsec);
      }
      else if(clip.pVideoMpeg4Playback)
      {
         AudioPlayTime = clip.pVideoMpeg4Playback->SetAudioPosition(PlayTime, bError, ps.playbackMsec);
      }
#ifndef FEATURE_WINCE
      AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, AudioPlayTime, stop);
      /* this will flush audio buffered in CMX */
      bError = AudioPlayerIf::Suspend(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if(clip.bHasText)
    {
      bool lBError = false;

      Media *txtMedia = GetTextMedia();
      if (NULL != txtMedia)
      {
        (void)txtMedia->SetTextPosition(PlayTime, lBError,ps.playbackMsec);
      }
#ifndef FEATURE_WINCE
      textPlayer.SetPlayTimes(PlayTime, stop);
      (void)textPlayer.Suspend(lBError);
#endif   /*    FEATURE_WINCE   */
    }
#endif

    //Prep audio & video.  The reason to have a separate step for this
    //is that there are some delays in this processing, so by getting
    //it out of the way we get better AV sync at startup.
    if (!PrepAudioVideo(true))
    {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio video prep failed");
       return false;
    }

    if (clip.bHasAudio)
    {
      //delay audio startup if this is an audio+video clip and not
      //starting at the beginning.  this allows time for decoding
      //all the non-rendered video frames before starting up.
      appControl.bDelayAudio = AudioPlayTime>0 && clip.bHasAudio && clip.bHasVideo;
      if (!appControl.bDelayAudio)
      {
         if (StartAudio())
         {
            bResumed = true;
         }
      }
    }

    if (clip.bHasVideo)
    {
#ifndef FEATURE_WINCE
      if (pActiveVideoPlayer && pActiveVideoPlayer->Resume(bError,stop))
      {
        if (bError)
        {
          VideoError();
        }
        else
        {
          bResumed=true;
        }
      }
      else
      {
        /* audioPlayer.Resume returns false.  This means that Resume cannot be
         * performed due to Resume being issued while the audioPlayer is playing.
         * This can happen if the Media Player and Qtv states get out of sync.
         * This is not a serious error so just indicate that Qtv resumed which has
         * the effect of ignoring the Resume.
         */
        bResumed=true;
      }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
    }

  #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if (clip.bHasText)
    {
#ifndef FEATURE_WINCE
      if (textPlayer.Start(bError, reposControl.bRestart))
      {
         if (bError)
         {
            TimedTextError();
         } else
         {
            bResumed = true;
         }
      }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
    }
  #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  }
  else //!appControl.bStepAVSyncNeeded
  {
    if (clip.bHasVideo)
    {
#ifndef FEATURE_WINCE
      if (pActiveVideoPlayer && pActiveVideoPlayer->Resume(bError,stop))
      {
        if (bError)
        {
          VideoError();
        }
        else
        {
          bResumed=true;
        }
      }
      else
      {
        /* audioPlayer.Resume returns false.  This means that Resume cannot be
         * performed due to Resume being issued while the audioPlayer is playing.
         * This can happen if the Media Player and Qtv states get out of sync.
         * This is not a serious error so just indicate that Qtv resumed which has
         * the effect of ignoring the Resume.
         */
        bResumed=true;
      }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
    }
  }

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  if ( !clip.bHasAudio )
  {
    /* We set qtv_cfg_enable_dsp_release back to false for video-only clips */
    qtv_cfg_enable_dsp_release = FALSE;
  }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

#ifdef FEATURE_FILE_FRAGMENTATION
  if(clip.pVideoMpeg4Playback && bResumed)
  {
    clip.pVideoMpeg4Playback->setPausedVideo(FALSE);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Resumed Video From App");
  }
#endif

  if (!bResumed) QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::ResumeVideo failed");
  return bResumed;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ResumeText

DESCRIPTION
//
//  ResumeText
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::ResumeText( QTV_RESUME_TEXT_type * )
{
  QtvPlayer::PlayerStateRecordT ps;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::ResumeText" );

  long start,stop;

  GetPlayerState(ps);

  if (!GetPlayTimes(start,stop))
  {
    Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
    return false;
  }

  bool bError=false;

  //resume streamer.
  if (clip.bStreaming && streamer )
  {
#ifndef FEATURE_WINCE
    if (streamer->Resume(bError))
    {
      //report an error.
      if (bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from Resume");
        StreamError();
      }
    }
#endif   /*    FEATURE_WINCE   */
    //if it's not streaming just resume anyway, the AV will
    //run out of data soon enough.
  }

  //reset the audio/video sync point
  m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

  bool bResumed = false;

  if(appControl.bStepAVSyncNeeded)
  {
    long PlayTime = 0;
    long AudioPlayTime;
    /* now synchronize audio and text with video */
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
       PlayTime = pActiveVideoPlayer->GetElapsedTime();
       pActiveVideoPlayer->SetPlayTimes(PlayTime, stop);
    }
#endif   /*    FEATURE_WINCE   */
    if(clip.bHasAudio)
    {
      if (clip.pAudioMpeg4Playback )
      {
         AudioPlayTime = clip.pAudioMpeg4Playback->SetAudioPosition(PlayTime, bError, ps.playbackMsec);
      }
      else
      {
         AudioPlayTime = clip.pVideoMpeg4Playback->SetAudioPosition(PlayTime, bError, ps.playbackMsec);
      }
#ifndef FEATURE_WINCE
      AudioPlayerIf::SetPlayTimes(activeAudioPlayerId, AudioPlayTime, stop);
      /* this will flush audio buffered in CMX */
      bError = AudioPlayerIf::Suspend(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
    }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    if(clip.bHasText)
    {
      bool lBError = false;

      Media *txtMedia = GetTextMedia();
      if (NULL != txtMedia)
      {
        (void)txtMedia->SetTextPosition(PlayTime, lBError, ps.playbackMsec);
      }
#ifndef FEATURE_WINCE
      textPlayer.SetPlayTimes(PlayTime, stop);
      (void)textPlayer.Suspend(lBError);
#endif   /*    FEATURE_WINCE   */
    }
#endif
    (void)PrepText();
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.bHasText)
  {
    if(appControl.bStepAVSyncNeeded)
    {
      appControl.bStepAVSyncNeeded = false;
#ifndef FEATURE_WINCE
      bResumed = textPlayer.Start(bError,false);
#endif   /*    FEATURE_WINCE   */
    }
    else
    {
#ifndef FEATURE_WINCE
      if (textPlayer.Resume(bError,stop))
      {
        if (bError)
        {
          TimedTextError();
        }
        else
        {
          bResumed=true;
        }
      }
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
    }
#ifdef FEATURE_FILE_FRAGMENTATION
    if( clip.bHasText && GetTextMedia() && bResumed)
    {
      GetTextMedia()->setPausedText(FALSE);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Resumed Text From App");
    }
#endif
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  if (!bResumed) QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Mpeg4Player::ResumeClip failed");
  return bResumed;
}
#endif /*FEATURE_FILE_FRAGMENTATION */

/* ======================================================================
FUNCTION:
  ReadPlaybackTracks

DESCRIPTION:
  This Synchronous method can be used to read the audio, video and text track
  information. Track information returned contains the track identifier,
  codec type such h264/AAC , and media type such as audio/video/text. Only
  base layers are reported.


INPUT/OUTPUT PARAMETERS:
  pnTrackCount(O) - When the trackList is passed as NULL pointer
                   the trackCount hold the exact number of tracks
                   present in the playback.
  pTrackList (O) - Track List Information pointer . Once the caller
                   obtain the track count, valid track list pointer
                   is passed to hold all the tracks.

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK    - Successful
  QTV_RETURN_ERROR - Generic Error and Read operation failed.

SIDE EFFECTS:
  None.
======================================================================*/
QtvPlayer::ReturnT
Mpeg4Player::ReadPlaybackTracks(uint32 *pnTrackCount,
                                QtvPlayer::TrackListT   *pTrackList)
{
  int numTracks                   = 0;
  ReferenceCountedPointer<ITrackList> currTrackList;
  int nCount                      = 0;
  int i;

  // Length pointer has to be non-NULL always
  // If the track list is allocated then track count should be non-zero
  if(!pnTrackCount                      ||
     (pTrackList && (*pnTrackCount == 0) ))
  {
    // Track list information not yet available.
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::Read Tracks - Invalid parameters passed" );
    return QtvPlayer::QTV_RETURN_INVALID_PARAMETER;
  }

  if (!clip.pVideoMpeg4Playback->GetTrackList(MakeOutPointer(currTrackList)) ||
      (currTrackList == NULL))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::Read Tracks Invalid state" );
    return QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
  }

  numTracks = currTrackList->GetNumTracksAvailable();

  if (numTracks == 0)
  {
    // Track list information not yet available.
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::Read Tracks Invalid state" );
    return QtvPlayer::QTV_RETURN_DATA_NOT_AVAILABLE;
  }

  // Count the number of base tracks.
  for(i=0;i<numTracks;i++)
  {
    // if the dependsOnID is negative then it is independent
    // on any track available.
    if ((currTrackList->GetDependsOnID(i) == -1) || (currTrackList->GetDependsOnID(i) == currTrackList->GetTrackID(i)))
    {
      nCount ++;
    }
  }

  if(nCount == 0 ||
     !pTrackList    )
  {
  if(nCount == 0)
  {
    // There are no base layers available , Strange , Raise error.
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::Read Tracks Zero Base layers" );
  }

    *pnTrackCount = nCount;

    return QtvPlayer::QTV_RETURN_OK;
  }

  // If the user has allocated memory for less number of tracks then
  // he will only get datails for that only.
  if(nCount > *pnTrackCount)
  {
    QTV_MSG_PRIO2(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::ReadPlaybackTracks Allocated = %d actual = %d",
                 *pnTrackCount,
                 nCount);
  }
  nCount = 0;
  for(i=0;((i < numTracks) && (nCount < *pnTrackCount));i++)
  {
    if ((currTrackList->GetDependsOnID(i) != -1)&&(currTrackList->GetDependsOnID(i) != currTrackList->GetTrackID(i)))
    {
      continue;
    }
    // It is a base layer.
    pTrackList[nCount].nTrackID = currTrackList->GetTrackID(i);
    pTrackList[nCount].encryptionType = currTrackList->GetEncryptionType(i);
    currTrackList->GetAudioTrackLanguage(pTrackList[nCount].language, i);
    GetCodecDetails(currTrackList->GetCodecType(i),
                    pTrackList[nCount].trackType,
                    pTrackList[nCount].mediaType);
    if(currTrackList->IsTrackSelected(i))
    {
      pTrackList[nCount].bmTrackState |= QTV_TRACK_STATE_SELECTED;
      switch(pTrackList[nCount].mediaType)
      {
        case QtvPlayer::MEDIA_AUDIO_ONLY:
        {
          /*if track was muted during Plaback or after PB_READY, on both the cases return proper
            track state information to the upper layers
          */
          if( (clip.bmMute       & QTV_AUDIO_TRACK_SELECTED) ||
        (clip.bmQueuedMute & QTV_AUDIO_TRACK_SELECTED)
      )
          {
            pTrackList[nCount].bmTrackState |= QTV_TRACK_STATE_MUTED;
          }
          break;
        }
        case QtvPlayer::MEDIA_VIDEO_ONLY:
        {
          /*if track was muted during Plaback or after PB_READY, on both the cases return proper
            track state information to the upper layers
          */
          if( (clip.bmMute       & QTV_VIDEO_TRACK_SELECTED) ||
        (clip.bmQueuedMute & QTV_VIDEO_TRACK_SELECTED)
      )
          {
            pTrackList[nCount].bmTrackState |= QTV_TRACK_STATE_MUTED;
          }
          break;
        }
        case QtvPlayer::MEDIA_TEXT:
        {
          /*if track was muted during Plaback or after PB_READY, on both the cases return proper
            track state information to the upper layers
          */
          if( (clip.bmMute       & QTV_TEXT_TRACK_SELECTED) ||
        (clip.bmQueuedMute & QTV_TEXT_TRACK_SELECTED)
      )
          {
            pTrackList[nCount].bmTrackState |= QTV_TRACK_STATE_MUTED;
          }
          break;
        }
        default:
        {
          break;
        }
      }
    }
    nCount++;
  }
  return QtvPlayer::QTV_RETURN_OK;
}

/* ======================================================================
FUNCTION:
  GetCodecDetails

DESCRIPTION:
  This Synchronous method can be used to figure out additional details
  about the codec.

INPUT/OUTPUT PARAMETERS:
  eMediaCodec(I) -  Media Codec
  eQtvCodec  (O) -  Qtv Codec
  eMediaType (O) -  Media Type

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void Mpeg4Player::GetCodecDetails(Media::CodecType eMediaCodec,
                                  QtvPlayer::TrackTypeT &eQtvCodec,
                                  QtvPlayer::MediaTypeT &eMediaType)
{
  switch (eMediaCodec)
  {
    case Media::EVRC_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_AUDIO_EVRC;
      eMediaType= QtvPlayer::MEDIA_AUDIO_ONLY;
      break;
    }
    case Media::QCELP_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_AUDIO_QCELP;
      eMediaType = QtvPlayer::MEDIA_AUDIO_ONLY;
      break;
    }
    case Media::AAC_CODEC:
 #ifdef FEATURE_QTV_BSAC
#error code not present
 #endif /* FEATURE_QTV_BSAC */
    {
      eQtvCodec = QtvPlayer::TRACK_AUDIO_MP4A_LATM;
      eMediaType= QtvPlayer::MEDIA_AUDIO_ONLY;
      break;
    }
    case Media::GSM_AMR_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_AUDIO_AMR;
      eMediaType= QtvPlayer::MEDIA_AUDIO_ONLY;
      break;
    }
#ifdef FEATURE_QTV_3GPP_AMR_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB */
#ifdef FEATURE_QTV_3GPP_AMR_WB_PLUS
#error code not present
#endif /* FEATURE_QTV_3GPP_AMR_WB_PLUS */

#ifdef FEATURE_QTV_3GPP_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_NB */

#ifdef FEATURE_QTV_3GPP_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_3GPP_EVRC_WB */
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */

#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */

    case Media::MPEG4_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_VIDEO_MP4V_ES;
      eMediaType= QtvPlayer::MEDIA_VIDEO_ONLY;
      break;
    }
    case Media::H263_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_VIDEO_H263_2000;
      eMediaType= QtvPlayer::MEDIA_VIDEO_ONLY;
      break;
    }
#ifdef FEATURE_H264_DECODER
    case Media::H264_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_VIDEO_H264;
      eMediaType= QtvPlayer::MEDIA_VIDEO_ONLY;
      break;
    }
#endif /* FEATURE_H264_DECODER */
    case Media::STILL_IMAGE_CODEC:
	case Media::STILL_IMAGE_H263_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_UNKNOWN;
      eMediaType= QtvPlayer::MEDIA_VIDEO_ONLY;
      break;
    }
#ifdef FEATURE_QTV_OSCAR_DECODER
#error code not present
#endif /* FEATURE_QTV_OSCAR_DECODER */
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    case Media::TIMED_TEXT_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_3GPP_TIMED_TEXT;
      eMediaType= QtvPlayer::MEDIA_TEXT;
      break;
    }
#endif
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    case Media::WMA_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_WM_AUDIO;
      eMediaType= QtvPlayer::MEDIA_AUDIO_ONLY;
      break;
    }

    case Media::WMV1_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_WM1_VIDEO;
      eMediaType= QtvPlayer::MEDIA_VIDEO_ONLY;
      break;
    }

    case Media::WMV2_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_WM2_VIDEO;
      eMediaType= QtvPlayer::MEDIA_VIDEO_ONLY;
      break;
    }
    case Media::WMV3_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_WM3_VIDEO;
      eMediaType= QtvPlayer::MEDIA_VIDEO_ONLY;
      break;
    }
#endif
    case Media::GENERIC_TEXT_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_GENERIC_TEXT;
      eMediaType= QtvPlayer::MEDIA_TEXT;
      break;
    }
 #if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    case Media::WMA_PRO_CODEC:
    {
       eQtvCodec = QtvPlayer::TRACK_WM_PRO_AUDIO;
       eMediaType= QtvPlayer::MEDIA_AUDIO_ONLY;
       break;
    }
    case Media::WMA_PRO_PLUS_CODEC:
    {
      eQtvCodec = QtvPlayer::TRACK_WM_PRO_PLUS_AUDIO;
      eMediaType= QtvPlayer::MEDIA_AUDIO_ONLY;
      break;
    }
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
    case Media::UNKNOWN_CODEC:
    default:
    {
      eQtvCodec  = QtvPlayer::TRACK_UNKNOWN;
      eMediaType = QtvPlayer::MEDIA_INVALID;
      break;
    }
  } /* end of switch(codec) */
  return;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION:
  SelectPlaybackTracks

DESCRIPTION:
  Asynchronous method used to select the new set of tracks from which to
  pull data. First parameter denotes the media combination and the remaining
  are the track identifiers which are to be selected.

INPUT/OUTPUT PARAMETERS:
  bmTrackSelected(I) - Track selection bit mask
  #define QTV_NO_TRACK_SELECTED             0x00
  #define QTV_VIDEO_TRACK_SELECTED          0x01
  #define QTV_AUDIO_TRACK_SELECTED          0x02
  #define QTV_TEXT_TRACK_SELECTED           0x04

  nAudioTrackID  (I) - Audio Track Identifier
  nVideoTrackID  (I) - Video Track Identifier
  nTextTrackID   (I) - Text Track Identifier
  bmSucceeded(O) - This bit mask says which one of them succeeded or not.
RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK - Any of the track selection succeeded.
  QTV_RETURN_ERROR - Generic Error Which means that none of them succeeded.

SIDE EFFECTS:
  None.

======================================================================*/
QtvPlayer::ReturnT
Mpeg4Player::SelectPlaybackTracks(uint32 bmTrackSelected,
                                  uint32 nAudioTrackID,
                                  uint32 nVideoTrackID,
                                  uint32 nTextTrackID,
                                  uint32 &bmSucceeded)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_OK;
  int nAudID  = -1;
  int nVidID  = -1;
  int nTextID = -1;

  // Initialize the combination.
  bmSucceeded = 0;

  if(!clip.pVideoMpeg4Playback)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::SelectPlaybackTracks Invalid Media" );
    return QtvPlayer::QTV_RETURN_ERROR;
  }

  // Video Track Selection
      if(bmTrackSelected & QTV_VIDEO_TRACK_SELECTED)
      {
    nVidID       = nVideoTrackID;
        bmSucceeded |= QTV_VIDEO_TRACK_SELECTED;
  }

  // Audio Track Selection
      if(bmTrackSelected & QTV_AUDIO_TRACK_SELECTED)
      {
    nAudID = nAudioTrackID;
        bmSucceeded |= QTV_AUDIO_TRACK_SELECTED;
  }

  // Text Track Selection
      if(bmTrackSelected & QTV_TEXT_TRACK_SELECTED)
      {
    nTextID = nTextTrackID;
        bmSucceeded |= QTV_TEXT_TRACK_SELECTED;
  }
      // Open the media clip
      if(bmSucceeded && retVal == QtvPlayer::QTV_RETURN_OK)
      {
    bool bRet = ActOnUserTrackSelection(nAudID,nVidID,nTextID);
        if(!bRet)
  {
    retVal = QtvPlayer::QTV_RETURN_ERROR;
  }
    }
  return retVal;
}
#endif //FEATURE_QTV_GENERIC_BCAST

/* ======================================================================
FUNCTION
  Mpeg4Player::MuteAudio

DESCRIPTION
//
//Mute Audio player.
//

DEPENDENCIES
  None.

RETURN VALUE
  QTV_RETURN_OK if everything went well.
  QTV_RETURN_ERROR is returned when player mute failed to execute.
  QTV_RETURN_INVALID_STATE is returned when attempted in invalid state.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT Mpeg4Player::MuteAudio()
{

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::MuteAudio" );

  /* If clip has no audio then return true since it is not applicable */
  if(!clip.bHasAudio)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                 "Mpeg4Player::MuteAudio Clip doesnt have audio" );
    return QtvPlayer::QTV_RETURN_OK;
  }
  if( playerState == PB_READY   )
  {
    /* Remember the mute combo */
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::MuteAudio Queued in PB Ready State" );
    clip.bmQueuedMute   |= QTV_AUDIO_TRACK_SELECTED;
    return QtvPlayer::QTV_RETURN_OK;
  }

  // If the clip has audio then the player can not be in any of the idle, opening
  // connecting or downloading states. Reasoning is that once the clip type is
  // set it moves to PB_READY state quickly.
  // The MUTE operation mainly intended for live/recorded broadcast streams.
  // When this code is upgraded for other media we need to think of HTTP_PAUSE
  // and PSUEDO_PAUSE states also.
  if(playerState != BUFFERING &&
     playerState != PLAYING   &&
     playerState != PAUSED    &&
     playerState != SUSPENDED)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::MuteAudio Error, Invalid state" );
    return QtvPlayer::QTV_RETURN_INVALID_STATE;
  }
   /* For streaming clips we need to issue the pause to the
    * streamer. Currently not implementing this since the
    * change is very specific to broadcast
    */
   ASSERT(clip.bStreaming == false);

   // Mute time is useful when the track is un-muted and none of the
   // tracks are playing already. In the case of live we will hint
   // the players with the estimated media time stamp. AV Sync offset
   // will be determined by the first timestamp returned from the media.
   // For the broadcast streams of recorded kind, we will hint the players
   // with the mute duration remembered. We will not do any estimation.

   m_uMuteClock       = pvGetTickCount();
#ifndef FEATURE_WINCE
   m_uMuteTimestamp   =
   m_ulMuteDuration   = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */

   if(playerState == PLAYING || playerState == BUFFERING)
   {
#ifndef FEATURE_WINCE
     if ( !AudioPlayerIf::Pause(activeAudioPlayerId)  )
     {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "Error, MuteAudio: Mute failed");
        return QtvPlayer::QTV_RETURN_ERROR;
     }
#endif   /*    FEATURE_WINCE   */
   }

   clip.bHasAudio =  false;
   clip.bmMute |= QTV_AUDIO_TRACK_SELECTED;

#ifdef FEATURE_QTV_GENERIC_BCAST
   if (clip.bBcastGeneric)
   {
     if(!GENERIC_BCAST_MEDIA_MUTE(clip.pVideoMpeg4Playback, selectedAudioTrackIdx))
     {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Media Source mute for Audio Track Failed" );
     }
   }
#endif // FEATURE_QTV_GENERIC_BCAST

     return QtvPlayer::QTV_RETURN_OK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::MuteVideo

DESCRIPTION
//
//  Mute Video Player
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
QtvPlayer::ReturnT Mpeg4Player::MuteVideo()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::MuteVideo" );

  /* If clip has no video then return true since it is not applicable */
  if(!clip.bHasVideo)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                 "Mpeg4Player::MuteVideo Clip doesnt have video" );
    return QtvPlayer::QTV_RETURN_OK;
  }

  /* if the player is not yet started playing
   * then remember the mute combo
   */
  if(playerState == PB_READY)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::MuteVideo Queued in PB Ready State" );
    clip.bmQueuedMute |= QTV_VIDEO_TRACK_SELECTED;
    return QtvPlayer::QTV_RETURN_OK;
  }

  // The PS_MUTE operation mainly intended for live/recorded broadcast streams.
  // When this code is upgraded for other media we need to think of HTTP_PAUSE
  // and PSUEDO_PAUSE states also.
  //ASSERT(playerState != PLAYER_PSUEDO_PAUSE);
  //ASSERT(playerState != PLAYER_HTTP_PAUSE);

  if(playerState != BUFFERING &&
     playerState != PLAYING   &&
     playerState != PAUSED    &&
     playerState != SUSPENDED)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "Mpeg4Player::MuteVideo Error, Invalid state" );
     return  QtvPlayer::QTV_RETURN_INVALID_STATE;
  }

  /* For streaming clips we need to issue the pause to the
   * streamer. Currently not implementing this since the
   * change is very specific to broadcast
   */
  ASSERT(clip.bStreaming == false);

  // Mute time is useful when the track is un-muted and none of the
  // tracks are playing already. In the case of live we will hint
  // the players with the estimated media time stamp. AV Sync offset
  // will be determined by the first timestamp returned from the media.
  // For the broadcast streams of recorded kind, we will hint the players
  // with the mute duration remembered. We will not do any estimation.
  m_uMuteClock           = pvGetTickCount();
#ifndef FEATURE_WINCE
     m_uMuteTimestamp       =
     m_ulMuteDuration      =   pActiveVideoPlayer->GetElapsedTime();
#endif   /*    FEATURE_WINCE   */

  /* Pause the video player */
  if(playerState == PLAYING || playerState == BUFFERING)
  {
    bool bError;
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer && !pActiveVideoPlayer->Pause(bError))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Error, MuteVideo: Mute failed");
      return QtvPlayer::QTV_RETURN_ERROR;
    }
#endif   /*    FEATURE_WINCE   */
  }

  clip.bmMute |= QTV_VIDEO_TRACK_SELECTED;
  clip.bHasVideo= false;
#ifdef FEATURE_QTV_GENERIC_BCAST
  if (clip.bBcastGeneric)
  {
    if(!GENERIC_BCAST_MEDIA_MUTE(clip.pVideoMpeg4Playback, selectedVideoTrackIdx))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Media Source mute for Video Track Failed" );
    }
  }
#endif // FEATURE_QTV_GENERIC_BCAST

  return QtvPlayer::QTV_RETURN_OK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::MuteText

DESCRIPTION
//
//  Mute Text Player
//

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
QtvPlayer::ReturnT Mpeg4Player::MuteText()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::MuteText" );

  /* If clip has no text then return true since it is not applicable */
  if(!clip.bHasText)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                 "Mpeg4Player::MuteText Clip doesnt have text" );
    return QtvPlayer::QTV_RETURN_OK;
  }

  /* if the player is not yet started then remember the mute combo */
  if( playerState == PB_READY)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::MuteText Queued in PB Ready State" );
    clip.bmQueuedMute |= QTV_TEXT_TRACK_SELECTED;
    return QtvPlayer::QTV_RETURN_OK;
  }

  // MUTE operation mainly intended for live/recorded broadcast streams.
  // When this code is upgraded for other media we need to think of HTTP_PAUSE
  // and PSUEDO_PAUSE states also.
  //ASSERT(playerState != PLAYER_PSUEDO_PAUSE);
  //ASSERT(playerState != PLAYER_HTTP_PAUSE);

  if(playerState != BUFFERING &&
     playerState != PLAYING   &&
     playerState != PAUSED    &&
     playerState != SUSPENDED)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "Mpeg4Player::MuteText Error, Invalid state" );
     return  QtvPlayer::QTV_RETURN_INVALID_STATE;
  }

  // Mute time is useful when the track is un-muted and none of the
  // tracks are playing already. In the case of live we will hint
  // the players with the estimated media time stamp. AV Sync offset
  // will be determined by the first timestamp returned from the media.
  // For the broadcast streams of recorded kind, we will hint the players
  // with the mute duration remembered. We will not do any estimation.
  m_uMuteClock           = pvGetTickCount();
#ifndef FEATURE_WINCE
  m_uMuteTimestamp       =
  m_ulMuteDuration       = textPlayer.GetElapsedTime();
#endif   /*    FEATURE_WINCE   */

  /* For streaming clips we need to issue the pause to the
   * streamer. Currently not implementing this since the
   * change is very specific to broadcast
   */
  ASSERT(clip.bStreaming == false);

  if(playerState == PLAYING || playerState == BUFFERING)
  {
    bool bError;
#ifndef FEATURE_WINCE
    if (!textPlayer.Pause(bError))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "Error, MuteText: Mute failed");
      return QtvPlayer::QTV_RETURN_ERROR;
    }
#endif   /*    FEATURE_WINCE   */
  }

  clip.bHasText          = false;
  clip.bmMute           |= QTV_TEXT_TRACK_SELECTED;

#ifdef FEATURE_QTV_GENERIC_BCAST
  if (clip.bBcastGeneric)
  {
    if(!GENERIC_BCAST_MEDIA_MUTE(clip.pVideoMpeg4Playback, selectedTextTrackIdx))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Media Source Mute for Text Track Failed" );
    }
  }
#endif //FEATURE_QTV_GENERIC_BCAST

  return QtvPlayer::QTV_RETURN_OK;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PrepSpecificTracks

DESCRIPTION
//
// Prep the specific tracks specified
// Return True if all is well.

DEPENDENCIES
  None.

RETURN VALUE
  True/False

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::PrepSpecificTracks(uint32 bmTrackSelected)
{
  bool bError=false;
  bool bOK   = true;

  /*
  *  PREP for Video is needed only when clip has Video that has not ended.
  */

  if (clip.bHasVideo && clip.pVideoMpeg4Playback && pActiveVideoPlayer && !pActiveVideoPlayer->TrackHasEnded())
  {
    if(bmTrackSelected & QTV_VIDEO_TRACK_SELECTED)
    {
      //Prep video
      if( !pActiveVideoPlayer->Prep(playbackID, clip.pVideoMpeg4Playback, bError,
                           true, GetAudioCodecType()) || bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video prep failed");
        VideoError();
        bOK =  false;

      }
      else
      {
        frameInfo.bInfoSetSinceLastPrep = false;
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Video prep Success!!");
      }
    }
  }

  if (clip.bHasAudio && !AudioPlayerIf::TrackHasEnded(activeAudioPlayerId))
  {
    if(bmTrackSelected & QTV_AUDIO_TRACK_SELECTED)
    {
      bOK = AudioPlayerIf::Prep(activeAudioPlayerId,playbackID,true,
            GetAudioMedia(),clip.bHasVideo,GetAudioCodecType());

      if (!bOK)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio prep failed");
        AudioError();
        bOK = false;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Audio prep Success!!");
      }
    }
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if (clip.bHasText && (bmTrackSelected & QTV_TEXT_TRACK_SELECTED))
  {
    if (!textPlayer.Prep(playbackID,true, GetTextMedia()))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Text prep Failed!!");
      TimedTextError();
      bOK=false;
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Text prep Success!!");
    }
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  return bOK;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::SuspendMedia

DESCRIPTION
//
//  Execute Suspend on all the applicable player tracks.
//

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::SuspendMedia(uint32 bmTrackSelected)
{
  bool            bError=false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::SuspendMedia" );

  if(bmTrackSelected & QTV_AUDIO_TRACK_SELECTED)
  {
    /* this will flush audio buffered in CMX */
    if(clip.bHasAudio &&
       !AudioPlayerIf::Suspend(activeAudioPlayerId) )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::SuspendMedia Audio failed");
      return false;
    }
  }


  if(bmTrackSelected & QTV_VIDEO_TRACK_SELECTED)
  {
    if(clip.bHasVideo)
    {
      if ((pActiveVideoPlayer && !pActiveVideoPlayer->Suspend(bError)) || bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "Mpeg4Player::SuspendMedia Video failed");
        return false;
      }
      if (pActiveVideoPlayer)
      {
         (void)pActiveVideoPlayer->Flush();
      }
    }
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(bmTrackSelected & QTV_TEXT_TRACK_SELECTED)
  {
    if(clip.bHasText && (!textPlayer.Suspend(bError) || bError))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::SuspendMedia Text failed");
      return false;
    }
  }
#endif

  return true;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::StartMedia

DESCRIPTION
//
//  Start all the applicable player tracks.
//

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::StartMedia(uint32 bmTrackSelected)
{
  bool            bError=false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::StartMedia" );

  if(bmTrackSelected & QTV_AUDIO_TRACK_SELECTED)
  {
    if (clip.bHasAudio)
    {
      bool bOK;

      appControl.bDoneAudioBuffering = false;
      #ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
      #else
        bOK = AudioPlayerIf::Start(activeAudioPlayerId, true);
      #endif /* FEATURE_MP4_DEBUG_AV_TICK */
      if (!bOK || bError)
      {

        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "Mpeg4Player::StartMedia audio failed");
        return false;
      }
    }
  }

  if(bmTrackSelected & QTV_VIDEO_TRACK_SELECTED)
  {
    if(clip.bHasVideo)
    {
      appControl.bReceivedFirstFrame = false;
      #ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
      #else
        if ( (pActiveVideoPlayer && !pActiveVideoPlayer->Start(bError, true)) || bError)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL,
                       QTVDIAG_PRIO_ERROR,
                       "Mpeg4Player::UnmuteVideo Start failed");
          return false;
        }
      #endif /* FEATURE_MP4_DEBUG_AV_TICK */
    }
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(bmTrackSelected & QTV_TEXT_TRACK_SELECTED)
  {
    if( clip.bHasText &&
        (!textPlayer.Start(bError,true) || bError))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::StartMedia audio failed");
      return false;
    }
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SuspendMedia

DESCRIPTION
//
//  Execute Suspend on all the applicable player tracks.
//

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::SuspendMedia()
{
#ifndef FEATURE_WINCE
  bool            bError=false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::SuspendMedia" );

  /* this will flush audio buffered in CMX */
  if(clip.bHasAudio &&
     !AudioPlayerIf::Suspend(activeAudioPlayerId) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::SuspendMedia Audio failed");
    return false;
  }


  if(clip.bHasVideo)
  {
    if ((pActiveVideoPlayer && !pActiveVideoPlayer->Suspend(bError)) || bError)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::SuspendMedia Video failed");
      return false;
    }
    if (pActiveVideoPlayer)
    {
      (void)pActiveVideoPlayer->Flush();
    }
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(clip.bHasText &&
     (!textPlayer.Suspend(bError) || bError))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::SuspendMedia Text failed");
    return false;
  }
#endif
#endif   /*    FEATURE_WINCE   */
  return true;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::StartMedia

DESCRIPTION
//
//  Start all the applicable player tracks.
//

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::StartMedia()
{
  bool            bError=false;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::SuspendMedia" );

  if( clip.bHasAudio && !StartAudio( ) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::StartMedia audio failed");
    return false;
  }

  if(clip.bHasVideo)
  {
    appControl.bReceivedFirstFrame = false;
#ifndef FEATURE_WINCE
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
    if (pActiveVideoPlayer && !(pActiveVideoPlayer->Start(bError, true))||
        bError)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::UnmuteVideo Start failed");
      return false;
    }
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
#endif   /*    FEATURE_WINCE   */
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifndef FEATURE_WINCE
  if( clip.bHasText &&
      (!textPlayer.Start(bError,true) || bError))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::StartMedia audio failed");
    return false;
  }
#endif   /*    FEATURE_WINCE   */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ResumeAll

DESCRIPTION
//
//  Bring up all the applicable track players.
//

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::ResumeAll(uint32 uPlayTime)
{

  // start and stop times.
  long start,stop;


  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::ResumeAll %d",uPlayTime );


  if (!GetPlayTimes(start,stop))
  {
    Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
    return false;
  }
  SetPlayTimes(uPlayTime,stop,true);

  //reset the audio/video sync point
  // AV Sync offset will be set by the first
  // media packet received.
  m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::VideoAV);
  m_mediaSync.ResetPlaybackOffset(AVSync::TextAV);

  if(!SuspendMedia())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::ResumeAll SuspendMedia failed");
    return false;
  }
  /* This will flush the video buffered */
  if(clip.bHasAudio)
  {
#ifndef FEATURE_WINCE
    AudioPlayerIf::SetPlayTimes(activeAudioPlayerId,uPlayTime,stop);
#endif   /*    FEATURE_WINCE   */
  }
#ifndef FEATURE_WINCE
  if(clip.bHasVideo && pActiveVideoPlayer)
  {
    pActiveVideoPlayer->SetPlayTimes(uPlayTime,stop);
  }
#endif   /*    FEATURE_WINCE   */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(clip.bHasText)
  {
#ifndef FEATURE_WINCE
    textPlayer.SetPlayTimes(uPlayTime,stop);
#endif   /*    FEATURE_WINCE   */
  }
#endif

  if(!PrepAudioVideo(true))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::UnmuteText Prep failed");
    return false;
  }

  if(!StartMedia())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::ResumeAll StartMedia failed");
    return false;
  }
  return true;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::ResumeAll

DESCRIPTION
//
//  Bring up all the applicable track players.
//

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

========================================================================== */
bool Mpeg4Player::ResumeAll(uint32 uPlayTime,uint32 bmTrackSelected)
{

  // start and stop times.
  long start,stop;


  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::ResumeAll %d, %x",uPlayTime,bmTrackSelected );

  if (!GetPlayTimes(start,stop))
  {
    return false;
  }


  if(!SuspendMedia(bmTrackSelected))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::ResumeAll SuspendMedia failed");
    return false;
  }
  /* This will flush the video buffered */
  if(clip.bHasAudio  && (bmTrackSelected & QTV_AUDIO_TRACK_SELECTED))
  {
    AudioPlayerIf::SetPlayTimes(activeAudioPlayerId,uPlayTime,stop);
  }
  if(clip.bHasVideo & (bmTrackSelected & QTV_VIDEO_TRACK_SELECTED))
  {
    if (pActiveVideoPlayer)
    {
       pActiveVideoPlayer->SetPlayTimes(uPlayTime,stop);
    }
  }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(clip.bHasText & (bmTrackSelected & QTV_TEXT_TRACK_SELECTED))
  {
    textPlayer.SetPlayTimes(uPlayTime,stop);
  }
#endif

  if(!PrepSpecificTracks(bmTrackSelected))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::UnmuteText Prep failed");
    return false;
  }

  if(!StartMedia(bmTrackSelected))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Mpeg4Player::ResumeAll StartMedia failed");
    return false;
  }
  return true;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::UnmuteMedia

DESCRIPTION
  Unmute the combo requested. It will just bring up the combination
  of media tracks requested.

DEPENDENCIES
  None.


RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
QtvPlayer::ReturnT Mpeg4Player::UnmuteMedia( uint32 &bmUnmuteCombo)
{
  unsigned long     playTime=0;
  QtvPlayer::ReturnT nReturn = QtvPlayer::QTV_RETURN_OK;

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
               "Mpeg4Player::UnmuteText" );

  // If we havent yet started playing then adjust
  // the queued mute combo remembered
  if(playerState == PB_READY)
  {
    bmUnmuteCombo     &= clip.bmQueuedMute;
    clip.bmQueuedMute ^= bmUnmuteCombo;
    return QtvPlayer::QTV_RETURN_OK;
  }

  /* And with the mute combination stored in the clip data structure.
   * This will make sure that we are un-muting the tracks which were
   * already muted.
   */
  bmUnmuteCombo &= clip.bmMute;

  if(!bmUnmuteCombo)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                 "Mpeg4Player::UnmuteMedia Not muted yet" );
    return QtvPlayer::QTV_RETURN_OK;
  }
#ifndef FEATURE_QTV_UNMUTE_OPTIMIZED
  // If the player is paused or suspended then defer the un-mute
  if(playerState == PLAYING || playerState == BUFFERING)
  {
    PauseMedia();
  }
#endif

  // Immaterial to how un-mute executed take out the bit from the bit mask.
  if(bmUnmuteCombo & QTV_AUDIO_TRACK_SELECTED)
  {
    clip.bHasAudio = true;

#ifdef FEATURE_QTV_GENERIC_BCAST
  if (clip.bBcastGeneric)
  {
    if(!GENERIC_BCAST_MEDIA_UNMUTE(clip.pVideoMpeg4Playback,selectedAudioTrackIdx))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Media Source Unmute for Audio Track Failed" );
    }
  }
#endif //FEATURE_QTV_GENERIC_BCAST

  }
  if(bmUnmuteCombo & QTV_VIDEO_TRACK_SELECTED)
  {
    clip.bHasVideo = true;

#ifdef FEATURE_QTV_GENERIC_BCAST
  if (clip.bBcastGeneric)
  {
    if(!GENERIC_BCAST_MEDIA_UNMUTE(clip.pVideoMpeg4Playback,selectedVideoTrackIdx))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Media Source Unmute for Video Track Failed" );
    }
  }
#endif // FEATURE_QTV_GENERIC_BCAST

  }
  if(bmUnmuteCombo & QTV_TEXT_TRACK_SELECTED)
  {
    clip.bHasText = true;

#ifdef FEATURE_QTV_GENERIC_BCAST
  if (clip.bBcastGeneric)
  {
    if(!GENERIC_BCAST_MEDIA_UNMUTE(clip.pVideoMpeg4Playback,selectedTextTrackIdx))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Media Source Unmute for Text Track Failed" );
    }
  }
#endif // FEATURE_QTV_GENERIC_BCAST

  }

  // If the player is paused or suspended then defer the un-mute
  if(playerState == BUFFERING || playerState == PLAYING)
  {
    uint32 bmReturnUnmuteCombo = 0;
    playTime = RepositionMediaDueToUnmute(bmUnmuteCombo, bmReturnUnmuteCombo);

    if (bmUnmuteCombo != bmReturnUnmuteCombo)
    {
      nReturn = QtvPlayer::QTV_RETURN_ERROR;
    }
    clip.bmMute ^= bmReturnUnmuteCombo;

#ifdef FEATURE_QTV_UNMUTE_OPTIMIZED
#error code not present
#else
    if(!ResumeAll(playTime))
#endif
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,
                   QTVDIAG_PRIO_ERROR,
                   "Mpeg4Player::UnmuteText ResumeAll failed");
      nReturn = QtvPlayer::QTV_RETURN_ERROR;
    }
  }
  else
  {
    clip.bmMute   ^= bmUnmuteCombo;
  }

  return nReturn;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetPlayerPositionConsideringMutes

DESCRIPTION
//
//  Calculate the Player position taking into consideration the track mute states.
//

DEPENDENCIES
  None

RETURN VALUE
  Returns the Player State position.

SIDE EFFECTS
  None

========================================================================== */
uint32
Mpeg4Player::GetPlayerPositionConsideringMutes(QtvPlayer::PlayerStateRecordT &currentPS)
{
  uint32 currTS = m_uMuteTimestamp;
  if(HasAudio() && (!IsAudioMuted()) )
  {
    currTS = currentPS.audioPlaybackMsec;
  }
  else if(HasVideo() && (!IsVideoMuted()))
  {
    currTS = currentPS.videoPlaybackMsec;
  }
  else if(HasText() && (!IsTextMuted()))
  {
    currTS = currentPS.textPlaybackMsec;
  }
  QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_DEBUG,
                "Player position returned by GetPlayerPositionConsideringMutes = %d",currTS);
  return currTS;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::CalculateNewPosition

DESCRIPTION
//
//  Calculate the new position for Un-mute
//

DEPENDENCIES
  None

RETURN VALUE
  Returns the new position for the un-muted track. It returns through the
  output parameters such as ullPlayTime and uPlayTimestamp.

SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::CalculateNewPosition( unsigned long &ulPlayTime,
                                        uint32    &uPlayTimestamp)
{
  QtvPlayer::PlayerStateRecordT ps;
  GetPlayerState(ps);
  uPlayTimestamp = GetPlayerPositionConsideringMutes(ps);
  /* Whether the stream is live or recorded unmute  always perform
   * the reposition since other streams could have moved from the
   * time when it was muted. In the case of live media, it moves even
   * if all the streams were muted.
   * For the recorded case all the streams can not be muted. If the
   * client wish to do this , one has to execute the pause operation.
   * we will always fail the attempt to mute all the tracks when the
   * broadcast stream is of recorded kind.
   */

  /* If Audio , Video or text is active then take the elapsed time from there.
   * Otherwise assume it is a live stream
   */
  if(clip.bHasAudio && !(clip.bmMute & QTV_AUDIO_TRACK_SELECTED))
  {
#ifndef FEATURE_WINCE
    ulPlayTime      = AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
  }
  else if(clip.bHasVideo && !(clip.bmMute & QTV_VIDEO_TRACK_SELECTED))
  {
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
      ulPlayTime      = pActiveVideoPlayer->GetElapsedTime();
    }
#endif   /*    FEATURE_WINCE   */
  }
  else if(clip.bHasText && !(clip.bmMute & QTV_TEXT_TRACK_SELECTED))
  {
#ifndef FEATURE_WINCE
    ulPlayTime      = textPlayer.GetElapsedTime();
#endif   /*    FEATURE_WINCE   */
  }
  else
  {
    /* For live broadcast streams , magic is simple.
     * We resume from the estimated time since we know that the
     * clock is continously ticking in the live case.
     * Recorded streams can not execute this case. Hence
     * the assert.
     */
    uint32 uClockDiff= pvGetTickCount() - m_uMuteClock;

    ulPlayTime       = m_ulMuteDuration    + uClockDiff;

  }
  return;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::RepositionMediaDueToUnmute

DESCRIPTION
//
//  Reposition media due to un-mute
//

DEPENDENCIES
  None

RETURN VALUE
  Media reported time which could be thought as the result of the reposition.

SIDE EFFECTS
  None

========================================================================== */
uint32 Mpeg4Player::RepositionMediaDueToUnmute(uint32 bmUnmuteCombo, uint32 &bmReturnUnmuteCombo)
{
  uint32        retVal=0;
  unsigned long     playTime=0;
  uint32       playTimestamp=0;
  bool bError = false;
  CalculateNewPosition(playTime,playTimestamp);

  retVal = playTime;
  bmReturnUnmuteCombo = bmUnmuteCombo;

#ifdef FEATURE_QTV_GENERIC_BCAST
  if(clip.bLiveBcast)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_DEBUG,
                     "LiveSource reposition not allowed");
    return retVal;
  }
#endif // FEATURE_QTV_GENERIC_BCAST

  if(bmUnmuteCombo & QTV_VIDEO_TRACK_SELECTED)
  {
    if(clip.bHasVideo && clip.pVideoMpeg4Playback)
    {
      (void)clip.pVideoMpeg4Playback->SetVideoPosition(playTime, bError,
                                                         playTimestamp);
      if(bError == TRUE)
      {
        bmReturnUnmuteCombo &= (~ QTV_VIDEO_TRACK_SELECTED);
        clip.bHasVideo = FALSE;
      }
    }
  }
  if(bmUnmuteCombo & QTV_AUDIO_TRACK_SELECTED)
  {
    Media* pAudioMedia = GetAudioMedia();
    if(clip.bHasAudio && pAudioMedia)
    {
      (void)pAudioMedia->SetAudioPosition(playTime, bError,
                                                playTimestamp);
      if(bError == TRUE)
      {
        bmReturnUnmuteCombo &= (~ QTV_AUDIO_TRACK_SELECTED);
        clip.bHasAudio = FALSE;
      }
    }
  }
  if(bmUnmuteCombo & QTV_TEXT_TRACK_SELECTED)
  {
    Media* pTextMedia = GetTextMedia();
    if(clip.bHasText && pTextMedia)
    {
      (void) pTextMedia->SetTextPosition(playTime, bError,
                                               playTimestamp);
      if(bError == TRUE)
      {
        bmReturnUnmuteCombo &= (~ QTV_TEXT_TRACK_SELECTED);
        clip.bHasText = FALSE;
      }
    }
  }
  return retVal;
}

/* ======================================================================
  FUNCTION:
    MutePlaybackTracks

  DESCRIPTION:
    This method is executed in the Caller task context synchronously. This
    Primitive is used to mute/un-mute the selected audio/text track of
    the player. This method shall not be used to mute/un-mute video track.

  INPUT/OUTPUT PARAMETERS:
    handle(I)   - Qtv Player Instance Handle
    bmTrackSelected(I) - Track selection bit mask
    #define QTV_AUDIO_TRACK_SELECTED          0x02
    #define QTV_TEXT_TRACK_SELECTED           0x04
    bmTrackState(I)   - Whether to Mute the track or not. Only the mute bit is used.
    #define TRACK_STATE_MUTED            0x01
    bmSucceeded(O) - This bit mask says which one of them succeeded or not.

  RETURN VALUE:
    QtvPlayer ReturnT type
    QTV_RETURN_OK - Successful mute or play of the specified track
    QTV_FEATURE_NOT_AVAILABLE - Video track mute/un-mute not supported.
    QTV_RETURN_ERROR - Generic Error while trying to mute or play.

  SIDE EFFECTS:
    a call to OpenURN will reset these values and flags passed in OpenURN
    will be valid.
  ======================================================================*/
QtvPlayer::ReturnT
Mpeg4Player::MutePlaybackTracks(uint32 bmTrackSelected,
                               uint32 bmTrackState,
                               uint32 *bmSucceeded)
{
  QtvPlayer::ReturnT retVal     =QtvPlayer::QTV_RETURN_OK;
  QtvPlayer::ReturnT tempRetVal = QtvPlayer::QTV_RETURN_OK;
  *bmSucceeded = 0;

  /* In streaming Mute tracks is not applicable. */
  if (clip.bStreaming == true)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Mute playback tracks feature not supported for streaming");
     return QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
  }

  if(bmTrackState == QTV_TRACK_MUTE)
  {
    // Text track mute
    if(bmTrackSelected & QTV_TEXT_TRACK_SELECTED)
    {
      if((tempRetVal = MuteText()) != QtvPlayer::QTV_RETURN_OK)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "Mpeg4Player::MutePlaybackTracks MuteText failed");
        retVal = tempRetVal;
      }
      else
      {
        *bmSucceeded |= QTV_TEXT_TRACK_SELECTED;
      }
    }

    // Audio Track Mute
    if(bmTrackSelected & QTV_AUDIO_TRACK_SELECTED)
    {
      if((tempRetVal = MuteAudio()) != QtvPlayer::QTV_RETURN_OK)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "Mpeg4Player::MutePlaybackTracks MuteAudio failed");
        retVal = tempRetVal;
      }
      else
      {
        *bmSucceeded |= QTV_AUDIO_TRACK_SELECTED;
      }
    }
    // Video Track Mute
    if(bmTrackSelected & QTV_VIDEO_TRACK_SELECTED)
    {
      if((tempRetVal = MuteVideo()) != QtvPlayer::QTV_RETURN_OK)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL,
                     QTVDIAG_PRIO_ERROR,
                     "Mpeg4Player::MutePlaybackTracks MuteVideo failed");
        retVal = tempRetVal;
      }
      else
      {
        *bmSucceeded |= QTV_VIDEO_TRACK_SELECTED;
      }
    }
  }
  else if(bmTrackState == QTV_TRACK_UNMUTE)
  {
    retVal = UnmuteMedia(bmTrackSelected);
    if(retVal == QtvPlayer::QTV_RETURN_OK)
    {
      *bmSucceeded = bmTrackSelected;
    }
  }
  else
  {
    // Invalid track action requested.
    retVal = QtvPlayer::QTV_RETURN_ERROR;
  }
  return retVal;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION:
  Reposition

DESCRIPTION:
  This is used to move the media cursor to the absolute time offset
  specified. This is not applicable to live streams. Equally applicable
  to Recorded or TIVO streams.  In addition it also expects that the
  player is in PAUSED state.

INPUT/OUTPUT PARAMETERS:
  nTimestamp(I) - Absolute time stamp
  bExtCmd(I)    - boolean to denote whether this is internal/external.
  uTransID(O)   - Transaction identifier generated. This is used to
                  correlate the response to the command issued.

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK - Primitive is successfully queued. Real response
                  will be delivered through the player status callback.
  QTV_RETURN_ERROR - Synchronous error from the broadcast stack.

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through
  the status callbacks registered:
  + QTV_PLAYER_STATUS_REPOSITION_DONE
    This event carries the new media cursor time offset along
    with usual attributes such as state, duration etc.,
  + QTV_PLAYER_STATUS_REPOSITION_FAILED

======================================================================*/
QtvPlayer::ReturnT Mpeg4Player::Reposition(uint32 nTimestamp,
                                           bool bExtCmd,
                                           uint32 uTransID)
{
  uint32 uSeqNo;  /* Media seq no */

  if(clip.bLiveBcast)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Reposition feature not supported for live broadcast");
    return QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
  }

  // Invalid state check
  if( playerState != PAUSED && playerState != SUSPENDED)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Reposition attempted at invalid states");
    return QtvPlayer::QTV_RETURN_INVALID_STATE;
  }

  if(!CmdSpaceAvailable())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Reposition failed due to lack of command space");
    return QtvPlayer::QTV_RETURN_ERROR;
  }

  if(!GENERIC_BCAST_MEDIA_REPOSITION(clip.pVideoMpeg4Playback,
                                     nTimestamp,
                                     &uSeqNo))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Reposition method received synchronous error from the stack");
    return QtvPlayer::QTV_RETURN_ERROR;
  }

  // Establish the mapping between command type and transaction identifier.
  CmdType cmd;
  if(bExtCmd)
  {
    cmd = EXTERNAL_REPOSITION_CMD;
  }
  else
  {
    cmd = INTERNAL_REPOSITION_CMD;
  }

  if(!CmdInsert(cmd,uTransID,uSeqNo))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "Reposition method failed due to duplicate ID from the stack");
    return QtvPlayer::QTV_RETURN_ERROR;
  }
  return QtvPlayer::QTV_RETURN_OK;
}


/* ======================================================================
FUNCTION:
  SeekToSync

DESCRIPTION:
  This is used to move the media cursor by specified number of sync frames.
  This movement could be forward or backward. Once this operation is
  successful which is communicated asynchronously through the status callback,
  PlayNextFrame should display that Sync Frame.

  This method is not applicable to live broadcast streams. Pre-condition
  for the successful execution of this method is that the player must be
  paused.

  1. Any negative offset which can move across the start of the media will
     position the media cursor to the first IDR frame.
  2. Any positive offset which can move beyond the end of the media will
     position the media cursor to the last IDR frame.
  3. If the cursor is already on a sync frame then zero offset is equivalent to a No-OP.
  4. If the cursor is already positioned at the first/Last sync frame , then
     seek Sync with any value apart from zero results in error.
  5. If the offset is zero and cursor is not on a I-frame then it will move
     to the previous sync frame if present. What if the previous IDR frame
     not available? Can we take the next one? <TBD>

INPUT/OUTPUT PARAMETERS:
  nSyncOffset(I) - Positive value move the cursor forward and
                   negative values backward by specified
                   number of Sync frames.

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK - Primitive is successfully queued. Real response
                  will be delivered through the player status callback.
  QTV_RETURN_ERROR - Error occured while queuing the primitive.

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through
  the status callbacks registered:
  + QTV_PLAYER_STATUS_SEEK_SYNC_DONE
    This event carries the new media cursor time offset along
    with usual attributes such as state, duration etc.,
  + QTV_PLAYER_STATUS_SEEK_SYNC_FAILED
  + QTV_PLAYER_NO_SYNC_FRAME_ERROR
======================================================================*/
QtvPlayer::ReturnT Mpeg4Player::SeekToSync(sint31 nSyncOffset,
                                           bool bExtCmd,
                                           uint32 uTransID)
{
  uint32 uSeqNo;   /* Media seq no */
  bool bOK = false;
  // For live broadcast we dont support seek to sync operation.
  if(clip.bLiveBcast)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync feature not supported for live broadcast");
    return QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
  }

  if(playerState != PAUSED && playerState != SUSPENDED)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync feature attempted at invalid state");
    return QtvPlayer::QTV_RETURN_INVALID_STATE;
  }

  if(!CmdSpaceAvailable())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync method failed due lack of command queue space");
    return QtvPlayer::QTV_RETURN_ERROR;
  }

  if (pActiveVideoPlayer)
  {
    bOK = GENERIC_BCAST_MEDIA_SEEK_SYNC_FROM_TIME(clip.pVideoMpeg4Playback,
                                                  nSyncOffset,
                                                  pActiveVideoPlayer->GetElapsedTime(),
                                                  &uSeqNo);
  }
  if(!bOK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync method failed due to synchronous error from stack");
    return QtvPlayer::QTV_RETURN_ERROR;
  }

  // Establish the mapping between command type and transaction identifier.
  CmdType cmd;
  if(bExtCmd)
  {
    cmd = EXTERNAL_SEEK_SYNC_CMD;
  }
  else
  {
    cmd = EXTERNAL_PLAY_SYNC_CMD;
  }

  if(!CmdInsert(cmd,uTransID,uSeqNo))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync method failed due to duplicate ID from source");
    return QtvPlayer::QTV_RETURN_ERROR;
  }
  return QtvPlayer::QTV_RETURN_OK;
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::SeekToSyncFromTime

DESCRIPTION:
  Seek sync to a timestamp.

PARAMETERS:
  sint31 nSyncOffset
    Positive value moves the cursor forward and negative value moves it backward
    by the specified number of sync frames.
  bool bExtCmd
    Used to specify if the command was internal or external.
  uint32 uTransID
    The transaction ID.
  long timeStamp
    The times tamp to sync to.

RETURN VALUE:
  None.
======================================================================*/
void Mpeg4Player::SeekToSyncFromTime(
  sint31 nSyncOffset,
  bool bExtCmd,
  uint32 uTransID,
  long timeStamp)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::SeekToSyncFromTime");

  uint32 uSeqNo;   /* Media seq no */
  bool bOK;

  if(!CmdSpaceAvailable())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync method failed due lack of command queue space");
  }

  bOK = GENERIC_BCAST_MEDIA_SEEK_SYNC_FROM_TIME(clip.pVideoMpeg4Playback,
                                                nSyncOffset,
                                                timeStamp,
                                                &uSeqNo);
  if(!bOK)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync method failed due to synchronous error from stack");
  }

  /* Establish the mapping between command type and transaction identifier. */
  CmdType cmd;
  if(bExtCmd)
  {
    cmd = EXTERNAL_SEEK_SYNC_CMD;
  }
  else
  {
    cmd = INTERNAL_SEEK_SYNC_CMD;
  }

  if(!CmdInsert(cmd,uTransID,uSeqNo))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "SeekToSync method failed due to duplicate ID from source");
  }
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::StartPlayingFromSeekedPoint

DESCRIPTION:
  Start playing from the timestamp returned by SeekToSyncFromTime.

PARAMETERS:
  long startTime
    The timestamp to start playback from.

RETURN VALUE:
  bool
    Returns true if there was a failure.
======================================================================*/
bool Mpeg4Player::StartPlayingFromSeekedPoint(long startTime)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::StartPlayingFromSeekedPoint");

   bool bSuccess = true;

   /* Set the playback times */
   SetPlayTimes(startTime, -1, false);

   /* Indicate play mode to media */
   bSuccess = GENERIC_BCAST_MEDIA_PLAY(clip.pVideoMpeg4Playback,
                                       UID_QTV_MEDIA_PARAM_PLAY_MODE_PLAY);
   if ( bSuccess )
   {
      if ( StartPlaying() )
      {
#ifndef FEATURE_WINCE
         event_report_payload(EVENT_QTV_CLIP_STARTED,
                              sizeof(qtv_event_clip_started_type),
                              &qtv_event_clip_started_payload);
#endif   /*    FEATURE_WINCE   */
      }
      else
      {
         ErrorAbort(START_PLAYING_ERROR);
         bSuccess = false;
      }
   }
   else
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Generic Bcast Media Play failed");
      bSuccess = false;
   }

   return ( !bSuccess );
}

/* ======================================================================
FUNCTION:
  PlaySyncFrame

DESCRIPTION:
  This method is used to move the video track by specified number
  of Sync frames in both directions while the player is paused. This
  method is not applicable to live broadcast streams.  In addition
  it will try to display the IDR frame at the new position if available.

INPUT/OUTPUT PARAMETERS:
  nSyncOffset(I) -  Denotes the number of sync frames by which media cursor
                    has to be moved forward/backward.
  uTransID(O)    -  token returned back to the caller which is used to correlate
                    the response to command issued.

RETURN VALUE:
  QtvPlayer ReturnT type
  QTV_RETURN_OK    - Successfully queued.
  QTV_RETURN_ERROR - Couldnt queue the primitive

SIDE EFFECTS:
  Following ASynchronous events shall be posted by the QTV through
  the status callbacks registered:
  + QTV_PLAYER_STATUS_PLAY_SYNC_FRAME_DONE
    This event carries the new media cursor time offset along
    with usual attributes such as state, duration etc.,
  + QTV_PLAYER_STATUS_PLAY_SYNC_FRAME_FAILED
  + QTV_PLAYER_NO_SYNC_FRAME_ERROR

======================================================================*/
QtvPlayer::ReturnT Mpeg4Player::PlaySyncFrame(sint31 nSyncOffset,
                                              uint32 uTransID)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  Media* pVM = GetVideoMedia();

  if(!pVM)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "PlaySyncFrame: Video Media doesnt exist ");

  }
  else if( playerState != PAUSED && playerState != SUSPENDED)
  {
    // Invalid state check
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "PlaySyncFrame attempted at invalid states");
    retVal = QtvPlayer::QTV_RETURN_INVALID_STATE;
  }
  else if(clip.bBcastGeneric)
  {
    if(clip.bLiveBcast)
    {
  // For live broadcast this feature not supported. Only applicable to
  // Recorded or TIVO kinds.
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "PlaySyncFrame feature not supported for live broadcast");
      retVal =  QtvPlayer::QTV_RETURN_FEATURE_NOT_AVAILABLE;
  }
    else
  {
  //Flush the video pipeline when the async callback status is recieved
  //within the player task.
  //Otherwise we could get into trouble.

  // Execute the seek to sync. When this returns asynchronously, we will
  // play the frame. What if the async indication missed? Then that IDR
  // frame wont be displayed. We can think of supporting some audit feature
  // to flush the command queue in the future. Need to check whether this
  // is really required. At present assumed that broadcast stack will provide
  // async indication for every command.
  return SeekToSync(nSyncOffset,false,uTransID);
}
  }
#ifdef FEATURE_QTV_REPOSITION_SYNC_FRAME
  else if(pVM->GetSource() == Media::FILE_SOURCE)
  {
    QtvPlayer::PlayerStateRecordT ps;
    bool bError;
    GetPlayerState(ps);
    pVM->seekToSync(nSyncOffset,bError,ps.playbackMsec);
    if(!bError)
    {
      if (pActiveVideoPlayer)
      {
        pActiveVideoPlayer->Flush();
      }
      bError = !(PlayNextFrame(NULL));
    }
    if(!bError)
    {
      retVal =  QtvPlayer::QTV_RETURN_OK;
      Notify(QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_DONE);
    }
    else
    {
      retVal = QtvPlayer::QTV_RETURN_ERROR;
      Notify(QtvPlayer::QTV_PLAYER_COMMAND_PLAY_SYNC_FRAME_FAILED);
    }
  }
#endif
  return retVal;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::InitCmdSpace

DESCRIPTION
  Initializes the command space data structure within player.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
========================================================================== */
void Mpeg4Player::InitCmdSpace()
{

  // 1. Invalidate all the entries of the command space array object
  memset(aCmdSpaceQ,0,sizeof(aCmdSpaceQ));

  // 2. Set the fresh value to the free space.
  uCmdFreeSpace = MAX_COMMAND_SPACE;
  return ;

}
/* ======================================================================
FUNCTION
  Mpeg4Player::CmdSpaceAvailable

DESCRIPTION
  Returns whether command queue has free space or not.

PARAMETERS
  None.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.
========================================================================== */
bool Mpeg4Player::CmdSpaceAvailable()
{
  return (uCmdFreeSpace > 0);

}

/* ======================================================================
FUNCTION
  Mpeg4Player::CmdInsert

DESCRIPTION
  Insert (type, token,SeqNo) pair to the command queue.

PARAMETERS
  cmd(I)   command type enumeration
  token(I) token generated for this command.
  uSeqNo(I) Sequence Number generated by the media source.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::CmdInsert(CmdType cmd,
                            uint32  token,
                            uint32  uSeqNo)
{
  uint32 i;

  // Assert for one free space at the least
  ASSERT(uCmdFreeSpace > 0);
  for(i=0;i<MAX_COMMAND_SPACE;i++)
  {
    if(aCmdSpaceQ[i].bValid &&
       aCmdSpaceQ[i].uSeqNo == uSeqNo)
    {
      // Media source cheating
      // Transaction ID already exists.
      return false;
    }
  }

  for(i=0;i<MAX_COMMAND_SPACE;i++)
  {
    if(!aCmdSpaceQ[i].bValid)
    {
      aCmdSpaceQ[i].bValid = true;
      aCmdSpaceQ[i].cmd     = cmd;
      aCmdSpaceQ[i].token   = token;
      aCmdSpaceQ[i].uSeqNo  = uSeqNo;
      break;
    }
  }
  // Assert for successful find within the array.
  ASSERT(i < MAX_COMMAND_SPACE);

  // Update the free space count
  uCmdFreeSpace--;

  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::CmdFree

DESCRIPTION
  Free the command associated with the sequence number

PARAMETERS
  uSeqNo(I) sequence number to search for
  token(O) Qtv Token to be given to the client
  cmd(O)   Command type output parameter.

RETURN VALUE
  Returns TRUE/FALSE depending on the search
  result.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::CmdFree(uint32 uSeqNo,
                          uint32 &token,
                          CmdType &cmd)
{
  uint32    i = 0; /* array index */
  bool    ret = false; /* return value */

  for(i=0;i<MAX_COMMAND_SPACE;i++)
  {
    if(aCmdSpaceQ[i].bValid &&
       aCmdSpaceQ[i].uSeqNo == uSeqNo)
    {
      cmd   = (CmdType) aCmdSpaceQ[i].cmd;
      token =           aCmdSpaceQ[i].token;
      // Mark it as available now
      aCmdSpaceQ[i].bValid = false;

      // Increment the free count
      uCmdFreeSpace++;
      ret = true;
      break;
    }
  }
  return ret;
}
#endif /* QTV_FEATURE_GENERIC_BCAST*/


/* ======================================================================
FUNCTION
  Mpeg4Player::PlayNextFrame

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
bool Mpeg4Player::PlayNextFrame(PV_NEXT_FRAME_type *)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::PlayNextFrame" );

  bool bDone=false;
  /* If there is no video then we should reject this command here itself. */
  if(!clip.bHasVideo)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "PlayNextFrame is not allowed");
    return false;
  }
  if (  (playerState!=PAUSED && playerState!=SUSPENDED) ||
        (((playerState == PAUSED) || (playerState == SUSPENDED)) && appControl.bResumePending) ||
#ifndef FEATURE_QTV_GENERIC_BCAST
        clip.bBcastISDB ||
        clip.bBcastFLO ||
#else
        clip.bLiveBcast ||
#endif
        clip.bStreaming )
    return bDone;

#ifndef FEATURE_WINCE
    bool bError = false;
    if (qtv_conc_mgr::can_process_command(this, QTV_PLAYER_MIME_TYPE))
    {
      if (playerState == SUSPENDED)
      {

        /* Video is currently disabled; need to enable before PlayNextFrame.
         * If Enable fails PlayNextFrame will too, so defer error handling
         * to PlayNextFrame. */
        if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->Enable(bError, GetAudioCodecType());
        }
        if (bError)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                       "Enable video failed");
        }
      }
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "PlayNextFrame not supported");
      /* Here no need to notify the failure message to the upper layers just
         returning false is enough caller of this function is taking care of
         notifying the error */
      return false;
    }

    if (pActiveVideoPlayer && pActiveVideoPlayer->PlayNextFrame(bError))
    {
      if (bError)
      {
        VideoError();
      }
      else
      {
        /* do A/V/T sync only if not looping */
        if(!bLoopTrackFlag)
        {
          if( (clip.bHasAudio && !AudioPlayerIf::IsDone(activeAudioPlayerId)) &&
        (clip.bHasVideo && pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone())))
            appControl.bStepAVSyncNeeded = TRUE;
        }
        if (clip.bHasAudio)
        {
          QtvPlayer::PlayerStateRecordT ps;
          GetPlayerState(ps);
          AudioPlayerIf::SetTSfromBytes(activeAudioPlayerId,ps.playbackMsec);
        }
        bDone=true;
      }
    }
#endif   /*    FEATURE_WINCE   */

  return bDone;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PlayPrevFrame

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
bool Mpeg4Player::PlayPrevFrame(PV_PREV_FRAME_type *)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::PlayPrevFrame" );

   bool bDone=false;
   /* If there is no video then we should reject this command here itself. */
   if(!clip.bHasVideo)
   {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "PlayNextFrame is not allowed");
     return false;
   }
   if ( (playerState!=PAUSED &&
        playerState!=SUSPENDED) ||
#ifndef FEATURE_QTV_GENERIC_BCAST
        clip.bBcastISDB ||
        clip.bBcastFLO ||
#else
        clip.bLiveBcast ||
#endif
        clip.bStreaming )
      return bDone;


      bool bError = false;
#ifndef FEATURE_WINCE
      if (qtv_conc_mgr::can_process_command(this, QTV_PLAYER_MIME_TYPE))
      {
        if (playerState == SUSPENDED)
        {
          /* Video is currently disabled; need to enable before PlayNextFrame.
           * If Enable fails PlayNextFrame will too, so defer error handling
           * to PlayPrevFrame. */
          if (pActiveVideoPlayer)
     {
       pActiveVideoPlayer->Enable(bError, GetAudioCodecType());
          }
          if (bError)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                         "Enable video failed");
          }
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "Mpeg4Player::PlayPrevFrame not supported");
        /* Here no need to notify the failure message to the upper layers just
           returning false is enough caller of this function is taking care of
           notifying the error */
        return false;
      }

      if (pActiveVideoPlayer && pActiveVideoPlayer->PlayPrevFrame(bError))
      {
         if (bError)
         {
            VideoError();
         } else
         {
            bDone=true;
         }
      }
#endif   /*    FEATURE_WINCE   */

   return bDone;
}

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
/* ======================================================================
FUNCTION
  Mpeg4Player::RotateVideo

DESCRIPTION
  If possible, get the video player to rotate its frames.

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.


DEPENDENCIES
  None.

RETURN VALUE
  True if the rotation command was issued properly, false otherwise.

SIDE EFFECTS
  This function sends an event to Diag with on successful command issue.

========================================================================== */
bool Mpeg4Player::RotateVideo(PV_VIDEO_ROTATE_type *pEvent)
{
   bool bSuccess = false;

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::RotateVideo" );

   // Rotation is done on the DSP, so we can't perform rotation once a frame
   // has already been displayed. If we are not actively decoding frames,
   // fail on receipt of a rotation command, since we won't be able to
   // rotate the frame immediately. Another option would be to save the
   // rotation command but that will require that the player check the
   // pending command when playback resumes. Besides being more complicated,
   // the user will probably not remember how many times the rotate command
   // was issued anyway -- we might as well throw away the command now.
#ifndef FEATURE_WINCE
   if (!clip.bHasVideo || (pActiveVideoPlayer && !(pActiveVideoPlayer->RotateVideo(pEvent->direction))))
   {
      bSuccess = false;
   }
   else
   {
      qtv_event_clip_rotate_type qtv_rotate_payload;

      qtv_rotate_payload.direction = pEvent->direction;
      event_report_payload(EVENT_QTV_CLIP_ROTATE,
                       sizeof(qtv_event_clip_rotate_type),
                       &qtv_rotate_payload);
      bSuccess = true;
   }
#endif   /*    FEATURE_WINCE   */

   return bSuccess;
}
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)

/* ======================================================================
FUNCTION
  Mpeg4Player::ScaleVideo

DESCRIPTION
  If the video player is playing (not buffering), scale frames according to
  the scalefactor specified in the event.

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

DEPENDENCIES
  None

RETURN VALUE
  True if the scale command was issued in a valid state, false otherwise.

SIDE EFFECTS
  None

========================================================================== */
bool Mpeg4Player::ScaleVideo(PV_VIDEO_SCALE_type *pEvent)
{
   bool bSuccess = false;

   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::ScaleVideo" );

   // Scaling is done on the DSP, so we can't perform scaling once a frame
   // has already been displayed. If we are not actively decoding frames,
   // fail on receipt of a scale command, since we won't be able to
   // scale the frame immediately. Another option would be to save the
   // scale command but that will require that the player check the
   // pending command when playback resumes. Besides being more complicated,
   // the user will probably not remember how many times the scale command
   // was issued anyway -- we might as well throw away the command now.
#ifndef FEATURE_WINCE
   if (clip.bHasVideo)
   {
#ifdef FEATURE_QTV_XSCALE_VIDEO
     if(pEvent->outFrameWidth && pEvent->outFrameHeight)
     {
       if (pActiveVideoPlayer && pActiveVideoPlayer->ScaleVideo(pEvent->outFrameWidth,pEvent->outFrameHeight))
       {
         bSuccess = true;
       }
     }
#endif //FEATURE_QTV_XSCALE_VIDEO
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
     if(pEvent->scaling != QtvPlayer::SCALE_NONE)
     {
       if (pActiveVideoPlayer && pActiveVideoPlayer->ScaleVideo(pEvent->scaling))
       {
         bSuccess = true;
       }
     }
#endif //FEATURE_MP4_FRAME_TRANSFORMATIONS
   }
#endif   /*    FEATURE_WINCE   */
   return bSuccess;
}

#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

/* ======================================================================
FUNCTION
  Mpeg4Player::SetAudioOverride

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
bool Mpeg4Player::SetAudioOverride(PV_SET_AUDIOOVER_type *pEvent)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::SetAudioOverride" );

  bool bDone=false;
  bool bError;
#ifndef FEATURE_WINCE
  if (pActiveVideoPlayer && pActiveVideoPlayer->SetAudioOverride(pEvent->audOverrideFormat, bError))
  {
    if (bError)
    {
      VideoError();
    }
    else
    {
      bDone=true;
    }
  }
#endif   /*    FEATURE_WINCE   */
  return bDone;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool Mpeg4Player::InitDownloadConfig()
//write the current URN and filename to the download config file.
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"InitDownloadConfig");
  if (URN.bValid && downloadInfo.bValid)
  {
    bool bOK=false;
    OSCL_FILE * fp = OpenDownloadConfigFile((char*)"wb");
    if (fp)
    {
      DownloadConfigFile data;

      if (!data.Init())
      {
        return false;
      }

      if (NULL != URN.sURN)
      {
        *data.sURN = *URN.sURN;
      }
      (void)std_strlcpy(data.sFilename,downloadInfo.info.Filename,sizeof(data.sFilename));
      if (OSCL_FileWrite(&data, 1, sizeof(data), fp) == sizeof(data))
      {
        bOK=true;
      }
      (void)OSCL_FileClose(fp);
    }
    //report errors in creating or writing the download config file.
    if (!bOK)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"PDS2: tempFile error");
      Notify(QtvPlayer::PV_PLAYER_STATUS_DOWNLOAD_TEMPFILE_ERROR);
    }
    return bOK;
  }
  return false;
}


bool Mpeg4Player::InitDownloadInfo(char * pName)
{
  if (pName)
  {
    QCUtils::EnterCritSect(&downloadInfo_CS);
    ZUtils::StrcpyN(downloadInfo.info.Filename,pName,QTV_MAX_FILENAME_BYTES);
    //make sure filename is not null.
    if (strlen(downloadInfo.info.Filename)>0)
    {
      downloadInfo.bValid=true;
      downloadInfo.info.InterruptedTime=(-1);
      downloadInfo.info.DownloadComplete=false;
    }
    QCUtils::LeaveCritSect(&downloadInfo_CS);
    return downloadInfo.bValid;
  }
  return false;
}


bool Mpeg4Player::PrepDownloadClip(PV_DOWNLOAD_CLIP_type *pEvent)
//preprocessing for download clip command.
{
  //Clear the old local file information
  clip.pVideoLocalFilename=NULL;

  bool bOK=false;

  //Set download info
  if (pEvent)
  {
    //Save new filename from app command.
    if (InitDownloadInfo(pEvent->filename))
    {
      if (InitDownloadConfig())
      {
        bOK=true;
      }
    }
  }
  else
  {
    //this is a resume download.
    //Find previous filename in config file.
    OSCL_FILE * fp = OpenDownloadConfigFile((char*)"rb");
    {
      if (fp)
      {
        DownloadConfigFile data;
        if (OSCL_FileRead(&data,1,sizeof(data),fp)==sizeof(data))
        {
          //re-initialize download info with old filename.
          if (InitDownloadInfo(data.sFilename))
          {
            bOK=true;
          }
        }
        (void)OSCL_FileClose(fp);
      }
    }
  }
  if (bOK)
  {
    clip.pVideoLocalFilename=downloadInfo.info.Filename;
    appControl.bStartPlaying = false;
    appControl.bPvxStart=false;
    if (pEvent)
    {
      appControl.bResumeDownload=false;
    }
    return true;
  }
  return false;
}


bool Mpeg4Player::DownloadClip(PV_DOWNLOAD_CLIP_type *pEvent)
//Download clip.
//(This is called with a Null event from Ready state to resume download.)
{
  //Make sure current URN is a download.
  if (!clip.bDownload)
  {
    return false;
  }

  switch(playerState)
  {
    case IDLE:
      //Start download from idle.
      if (PrepDownloadClip(pEvent))
      {
        //Must reconnect.
        appControl.bStartDownloading=true;
        if (OpenClip())
        {
          //(state is now Connecting).
          //(wait on DOWNLOAD_STATUS messages)
          return true;
        }
        else
        {
          ErrorAbort(OPEN_CLIP_ERROR);
        }
      }
      break;

    case CONNECTING:
      //Start download while connecting.
      if (PrepDownloadClip(pEvent))
      {
        //Just set flag and keep waiting on connection.
        appControl.bStartDownloading = true;
        return true;
      }
      break;

    case DL_READY:
      //Start download from Ready (already connected).
      if (PrepDownloadClip(pEvent))
      {
        // Here StartDownloader will return true since the file size is
        // available in this point. Any way we are not calling this function
        // in current implementation
        if (StartDownloader())
        {
          //Always check for "playback asap" option when starting download.
          HandlePlaybackAsap();

          //(wait on DOWNLOAD_STATUS messages)

          return true;
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                       "Downloader not Started,Waiting for file size");
        }
      }
      break;
  }
  return false;
}

bool Mpeg4Player::ResumeDownload(PV_RESUME_DOWNLOAD_type*)
{
  switch(playerState)
  {
    case IDLE:
      //resume a download from this or previous player instance
#if 0
      if (OpenURN(NULL))
      {
        if (DownloadClip(NULL))
        {
          return true;
        }
      }
#endif
      break;
  }
  return false;
}

bool Mpeg4Player::StopDownload(PV_STOP_DOWNLOAD_type*)
{
  //See whether download is underway
  bool bActive=false;
  switch(playerState)
  {
    case IDLE:
      //ignore
      break;
    case CONNECTING:
      //could be either download or streaming connect.
      bActive=clip.bDownload;
      break;
    case DL_READY:
    case DOWNLOADING:
      bActive=true;
      break;
    default:
      //for playback states we must check the downloading flag
      //to tell if download is underway
      bActive=bDownloading;
      break;
  }
  if (bActive)
  {
    bool bErr;
    StopDownloader(bErr);

    if (bErr)
    {
      //error's stopping, go ahead and go idle
      //but report an abort instead of a stopped code.
      DownloadComplete(ABORT);
    }
    else
    {
      DownloadComplete(STOPPED);
    }
  }
  return true;
}


bool Mpeg4Player::DeleteDownloadSession(PV_DELETE_DOWNLOAD_SESSION_type*)
{
  //make sure current URN is a download
  if (!clip.bDownload)
  {
    return false;
  }

  switch(playerState)
  {
    case IDLE:
      PurgeDownload();
      InitURN();
      return true;
  }
  return false;
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


#ifdef FORCE_REBUFFERING
/* ======================================================================
FUNCTION
  Mpeg4Player::ForceRebuffering

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
void Mpeg4Player::ForceRebuffering()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ForceRebuffering" );

   if (playerState==PLAYING && clip.bStreaming && pQtvStream)
   {
      //note: this requires modifying the stream api to expose
      //the resetChannelBuffers routine.
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "WARNING: Forcing rebuffering...");
      pQtvStream->resetChannelBuffers();
   }
}
#endif /* FORCE_REBUFFERING */

/* ======================================================================
FUNCTION
  Mpeg4Player::IsPlaying

DESCRIPTION
//return true if any stream is playing.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::IsPlaying()
{
#ifndef FEATURE_WINCE
   if (clip.bHasAudio)
   {
    if (AudioPlayerIf::IsPlaying(activeAudioPlayerId))
      {
         QTV_MSG(QTVDIAG_MP4_PLAYER,
                 "Mpeg4Player::IsPlaying --> Audio is Playing");
         return true;
      }
   }
   if (clip.bHasVideo)
   {
      if (pActiveVideoPlayer && pActiveVideoPlayer->IsPlaying())
      {
         QTV_MSG(QTVDIAG_MP4_PLAYER,
                 "Mpeg4Player::IsPlaying --> Video is Playing");
         return true;
      }
   }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   if (clip.bHasText)
   {
      if (textPlayer.IsPlaying())
      {
        QTV_MSG(QTVDIAG_MP4_PLAYER,
                 "Mpeg4Player::IsPlaying --> Text is Playing");
         return true;
      }
   }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */
   return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::IsPaused

DESCRIPTION
//return true if any stream is playing.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::IsPaused()
{
   if (clip.bHasAudio)
   {
      /* If the Pause is due to an incoming voice call, the Audio
         Player goes to the idle state, even though the Mpeg4Player does
         a Pause
      */
#ifndef FEATURE_WINCE
    if (!(AudioPlayerIf::IsPaused(activeAudioPlayerId)
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
            || ( qtv_cfg_enable_dsp_release &&
                 AudioPlayerIf::IsIdle(activeAudioPlayerId) )
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
          || AudioPlayerIf::IsPausedDisabled(activeAudioPlayerId)
         ))
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Mpeg4Player::IsPlaying --> Audio is not Paused");
         return false;
      }
#endif   /*    FEATURE_WINCE   */
   }
   if (clip.bHasVideo)
   {
#ifndef FEATURE_WINCE
      if (pActiveVideoPlayer && !(pActiveVideoPlayer->IsPaused() || pActiveVideoPlayer->IsPausedDisabled()))
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Mpeg4Player::IsPlaying --> Video is not Paused");
         return false;
      }
#endif   /*    FEATURE_WINCE   */
   }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   if (clip.bHasText)
   {
#ifndef FEATURE_WINCE
      if (!textPlayer.IsPaused())
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                 "Mpeg4Player::IsPlaying --> Text is not Paused");
         return false;
      }
#endif   /*    FEATURE_WINCE   */
   }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
   return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::setStopVideoInitFlag

DESCRIPTION
 Set the flag to stop H.263 decoder initialization during initial preroll
 buffering (During PrepPlay() )

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  NONE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::setStopVideoInitFlag(void)
{
  bStop = true ;
  if (isStreaming())
  {
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
      pActiveVideoPlayer->setStopVideoInitFlag();
    }
#endif   /*    FEATURE_WINCE   */
  }
  return;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::IsDone

DESCRIPTION
//return true if all streams are done or idle

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::IsDone()
{
#ifndef FEATURE_WINCE
   if (clip.bHasAudio
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#endif
      )
   {
    if (!AudioPlayerIf::IsDone(activeAudioPlayerId))
      {
         return false;
      }
   }
   if (clip.bHasVideo)
   {
      if (pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone()) && !clip.bStillImage)
      {
         return false;
      }
   }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   if (clip.bHasText)
   {
      if (!textPlayer.IsDone())
      {
         return false;
      }
   }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */
   return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Buffering

DESCRIPTION
//
// just got a buffering status message from a stream

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::Buffering()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::Buffering" );

   //ignore this message for a pure local file.
   if (appControl.bPureLocalFile)
   {
      return;
   }

   //see if we need to transition from Playing back to Buffering.
   switch (playerState)
   {
     case PLAYING:
       //don't transition if any stream is playing,
       //since one stream may report Buffering state
       //after the other stream has already reported Playing.
       if (!IsPlaying())
       {
         Notify(BUFFERING);
       }
       else
       {
         nPrerollDurationFilled = 0;
       }
       break;

     case BUFFERING:
       Notify(BUFFERING);
       break;

     case PAUSED:
     case SUSPENDED:
       if(!IsPaused())
       {
         Notify(BUFFERING);
       }
       break;
   }
}


/* ======================================================================
FUNCTION
  Mpeg4Player::AudioBufferingDone()

DESCRIPTION
  Handles the event of audio buffering ended. This will normally restart audio
  output but if bDelayAudioOutput is set, we will wait until the first frame
  has been rendered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::AudioBufferingDone()
{
  // Ignore this status while the player is PAUSED (which might happen since
  // the UI might pause the player after the AUDIO_BUFFERING_DONE message comes
  // from the audio player. When we resume audio, the audio player will notify
  // us again with AUDIO_BUFFERING_DONE, at which time we will not be in the
  // PAUSED state anymore and we can resume audio.
  if ( ((playerState != PAUSED && playerState != SUSPENDED)
        ||appControl.bResumePending) && (playerState != IDLE) )
  {
    bool wasDoneBefore = appControl.bDoneAudioBuffering;
    appControl.bDoneAudioBuffering = true;

    if (!wasDoneBefore && (!appControl.bDelayAudioOutput || appControl.bReceivedFirstFrame))
    {
        appControl.bDelayAudioOutput = false;
#ifndef FEATURE_WINCE
      // if audio player is in paused state do not post Restart Audio Output
      if (!AudioPlayerIf::IsPaused(activeAudioPlayerId))
      {
      AudioPlayerIf::RestartAudioOutput(activeAudioPlayerId);
      }
#endif   /*    FEATURE_WINCE   */
    }
  }
}

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */

/* ======================================================================
FUNCTION
  Mpeg4Player::StartAudio

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
bool Mpeg4Player::StartAudio()
{
#ifndef FEATURE_WINCE
   bool bOK;
#else
#error code not present
#endif   /*    FEATURE_WINCE   */

   appControl.bDoneAudioBuffering = false;

   if (clip.bHasAudio)
   {
    bool bError = false;

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
      if (qtv_cfg_enable_dsp_release)
      {
        /* We cannot resume the audio because the audio CMX interface
        ** was stopped by the incoming call.
        */
        //Here error would be reported via a separate audio_err notification event
#ifndef FEATURE_WINCE
        bOK = AudioPlayerIf::Start(activeAudioPlayerId, false);
#endif   /*    FEATURE_WINCE   */
      }
      else
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

      /* If starting audio in the SUSPENDED state this must be
         a fresh start because audio was disabled in the preceding
         state. */
      if ((playerState == SUSPENDED ||
           (playerState == REPOSITIONING && lastPlayerState == SUSPENDED)) &&
          playerAudioConcState == AUDIO_NOT_CONCURRENT)
      {
        bool bRestart = false;
#ifndef FEATURE_WINCE
        bOK = AudioPlayerIf::Start(activeAudioPlayerId, bRestart);
#endif   /*    FEATURE_WINCE   */
      }
      else
      {
#ifndef FEATURE_WINCE
        bOK = AudioPlayerIf::Start(activeAudioPlayerId, reposControl.bRestart);
#endif   /*    FEATURE_WINCE   */
      }
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
      if ( !appControl.bDelayAudio && (qtv_cfg_enable_dsp_release == TRUE)
           && !clip.bHasVideo )
      {
        /* Set qtv_cfg_enable_dsp_release back to false only if we are not
        ** delaying the start of audio.
        */
        qtv_cfg_enable_dsp_release = FALSE;
      }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
      if (bOK)
      {
         if (bError)
         {
            AudioError();
         } else
         {
            return true;
         }
      }
   }
   return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::VideoRendering

DESCRIPTION
//just got the video rendering message.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::VideoRendering()
{
  // While the player state is paused, ignore these messages for the purpose
  // of restarting audio output. We should get more video rendering mesages
  // later, which may trigger us to restart audio output.
  if (IsAudioPlaying() && ((playerState != PAUSED && playerState != SUSPENDED)
                           ||appControl.bResumePending) && (playerState != IDLE))
  {
    // If we wanted to delay audio output until the first frame is rendered
    // we have reached the trigger point, so restart audio output!
    if (appControl.bDelayAudioOutput && appControl.bDoneAudioBuffering &&
        !appControl.bReceivedFirstFrame)
    {
      appControl.bDelayAudioOutput = false;
#ifndef FEATURE_WINCE
      AudioPlayerIf::RestartAudioOutput(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
    }
  }
  appControl.bReceivedFirstFrame = true;

  switch (playerState)
  {
    case BUFFERING:
    case PLAYING:
      //start audio now if needed.
      if (appControl.bDelayAudio)
      {
        appControl.bDelayAudio=false;
      (void)StartAudio();
      }

      /* Do not notify the Player that Qtv is playing at this time.  It is too
       * early.  The video time stamp of the new play position has not yet
       * been updated.  This means that if Player passes on the PLAY
       * notification to the app (which it does) and the app does a
       * GetPlayerState(), the play time would still be at the old position.
       * This happens during repositioning.
       */
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::Playing

DESCRIPTION
//
// just got a playing status message from a stream

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::Playing()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::Playing" );

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   // Do not move out of RELEASE-PAUSE state
   if (qtv_cfg_enable_dsp_release)
   {
      return;
   }
#endif

   switch (playerState)
   {
      case BUFFERING:
         //Go from buffering to playing when audio starts playing
         //or video starts rendering.
         // if (!bVideo)
         {
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
            if (clip.bDownload)
             Notify(DOWNLOADED_DATA_AVAIL);
#endif
            // if (audioPlayer.IsPlaying())
            {
               Notify(PLAYING);
            }
         }

         break;
      case PAUSED:
#ifdef FEATURE_QTV_PSEUDO_STREAM
      case PLAYER_PSEUDO_PAUSE:
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      case PLAYER_HTTP_PAUSE:
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

      case SUSPENDED:

          //Go from Paused to Playing when anything starts playing.
         //Make sure streams are still playing since
         if (IsPlaying())
         {
            Notify(PLAYING);
         }
         break;
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::DonePlaying

DESCRIPTION
//
// just got a done playing status message from audio or video.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::DonePlaying(const bool bError, const bool bVideo)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::DonePlaying" );

   //transition to playing if necessary.
   if (playerState==BUFFERING)
   {
      if (IsPlaying() )
      {
         //One stream ended and the other is playing,
         //so go to playing.
         Notify(PLAYING);
         return;
      }
   }

   //may need to start audio now.
   if (bVideo && appControl.bDelayAudio)
   {
      switch (playerState)
      {
         case BUFFERING:
         case PLAYING:
            //video ended, but may need to start audio now.
            appControl.bDelayAudio=false;
            if (StartAudio())
            {
               //audio is started.
               //remain in Buffering state until audio
               //reports Playing.
               return;
            }
            break;
      }
   }

   // If video ends but we haven't started audio yet, do so now.
   if (bVideo)
   {
     if (appControl.bDelayAudioOutput &&
         appControl.bDoneAudioBuffering &&
         !appControl.bReceivedFirstFrame)
     {
       appControl.bReceivedFirstFrame = true;
       appControl.bDelayAudioOutput = false;
#ifndef FEATURE_WINCE
      AudioPlayerIf::RestartAudioOutput(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
     }
     else
     {
       appControl.bReceivedFirstFrame = true;
     }
   }

   if (!IsDone())
   {
     return;
   }

   //Catch the end of the clip.
   switch (playerState)
   {
      case PLAYING:
      case BUFFERING:
      case PAUSED:
      case REPOSITIONING:
      case SUSPENDED:
      {
        bool bErr = false;

        bool bCloseStreamer = clip.bStreaming && !bError;
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
        bool bNextClipReady = (clip.bStreaming && streamer &&
          streamer->IsNextClipInServerSidePlaylistReady());

        if (bNextClipReady)
        {
          bCloseStreamer = bError;
        }
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

#ifdef FEATURE_QTV_FCS
#error code not present
#endif

        // Stop everything except possibly the streamer.
        StopPlayer(bErr, bCloseStreamer);

        //Report error termination.
        if (bError)
        {
          PlaybackComplete(ABORT);
        }
        else
        {
#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
          if (bNextClipReady)
          {
            if (SwitchToNextClipInServerSidePlaylist(false))
            {
              reposControl.bRestart = false;
              SetState(PB_READY);
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
              if (m_useMAPILogic && clip.pVideoMpeg4Playback != NULL)
              {
                GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback, true);
              }
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */
              StartPlaying();
            }
            else
            {
              PlaybackComplete(ABORT);
            }
          }
          else
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
          {
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
            {
            PlaybackComplete(COMPLETE);
          }
        }
       }
      }
      break;
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::PlaybackComplete

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
void Mpeg4Player::PlaybackComplete(EndCode code)
{
   QtvPlayer::AudioVideoStatisticsT statistics;
   Common::LastFrame videoLastFrame;
   bool bVideoOK = true;
    /* Clear out the statistics structure */
   memset( (void*)&statistics, 0, sizeof(statistics) );
#ifndef FEATURE_QTV_GENERIC_BCAST
   if ( clip.bHasVideo &&
       (playerState != CONNECTING) &&
       (playerState != IDLE))
#else
     if ( (clip.bHasVideo || IsVideoMuted())&&
         (playerState != CONNECTING) &&
         (playerState != IDLE))
#endif
   {
#ifndef FEATURE_WINCE
       if(pActiveVideoPlayer)
       {
         bVideoOK = pActiveVideoPlayer->GetStatistics(statistics,videoLastFrame);
       }
       else
       {
         bVideoOK = false;
       }
#endif   /*    FEATURE_WINCE   */
   }

   qtv_event_clip_ended_type_ver2 qtv_event_clip_ended_payload;
   memset( (void*)&qtv_event_clip_ended_payload, 0, sizeof(qtv_event_clip_ended_payload) );

   if(bVideoOK == true )
   {
      qtv_event_clip_ended_payload.total_frames        = statistics.Video.frames;
      qtv_event_clip_ended_payload.decode_error_frames = statistics.Video.failedDecode;
      qtv_event_clip_ended_payload.dropped_frames      = statistics.Video.skippedDecode;

   }

   //report termination.
   /*
     During a streaming session if we get a redirect request from server,
     we filter out all notifications to QTV
   */
   if( !(clip.bStreaming && redirectFlag) )
   {
      switch (code)
      {
         case ABORT:
            if(bStop == true) //force abort to stop
            {
               Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_STOPPED );
               qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_STOPPED;
            }
            else
            {
                Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_ERROR_ABORT );
                qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_ERROR_ABORT;
            }
            break;
         case STOPPED:
            Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_STOPPED );
            qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_STOPPED;
            break;
         case COMPLETE:
            Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYBACK_COMPLETE );
            qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_SUCCESS;
            break;
         case FCS_COMPLETE:
            Notify(QtvPlayer::QTV_PLAYER_STATUS_FCS_PLAYBACK_COMPLETE );
            qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_SUCCESS;
            break;
        case FCS_SWITCH_ERROR:
            Notify(QtvPlayer::QTV_PLAYER_STATUS_SWITCH_STREAM_FAILED);
            qtv_event_clip_ended_payload.end_type = QTV_EVENT_CLIP_ENDED_ERROR_ABORT;
            break;
      }

      //replace an old event with newer version due to change in payload
      //reports video statistics after playback
#ifndef FEATURE_WINCE
      event_report_payload(EVENT_QTV_CLIP_ENDED_VER2,
                           sizeof(qtv_event_clip_ended_type_ver2),
                           (void *)&qtv_event_clip_ended_payload);
#endif   /*    FEATURE_WINCE   */
   }

   //Do state transition.
   // Always Notify we're IDLE! except if the session supports content switching
   if((code != FCS_COMPLETE) && (code != FCS_SWITCH_ERROR))
   {
     Notify(IDLE);
   }
   CleanupPlayback();
   bStop = false;
}

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
bool Mpeg4Player::SwitchToNextClipInServerSidePlaylist(bool bUserInitiated)
{
  ASSERT(clip.bStreaming);

  if (!streamer->SwitchToNextSDPInServerSidePlaylist())
  {
    QTV_MSG_PRIO(QTVDIAG_STREAMING, QTVDIAG_PRIO_ERROR,
      "Switch: Cannot switch to next SDP in server side playlist!");
    return false;
  }

  ((StreamMedia *) clip.pVideoMpeg4Playback)->ResetError();

  if (!DescribeClip(clip.pVideoMpeg4Playback, 0, NULL, 0, NULL, 0))
  {
    QTV_MSG_PRIO(QTVDIAG_STREAMING, QTVDIAG_PRIO_ERROR,
      "Switch: Cannot describe new clip!");
    return false;
  }

  if (!streamer->UpdateAfterSwitchingSDPForServerSidePlaylist(bUserInitiated))
  {
    QTV_MSG_PRIO(QTVDIAG_STREAMING, QTVDIAG_PRIO_ERROR,
      "Switch: Cannot update after switching SDP!");
    return false;
  }

  appControl.cbData1 = streamer->GetPlayingServerSidePlaylistClipIndex();
  appControl.cbData2 = 0;

  Notify(QtvPlayer::QTV_PLAYER_STATUS_CLIP_TRANSITION);

  return true;
}
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION
  Mpeg4Player::AudioSpec

DESCRIPTION

 process an audio Spec notifcation.


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::AudioSpec(const AUDIO_SPEC_type *pEvent)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::AudioSpec" );
  if (pEvent)
  {
    //set the audio bit rate
    if(clip.pAudioMpeg4Playback)
    {
      clip.pAudioMpeg4Playback->SetAudioBitRate(pEvent->bitRate);
      if(clip.pAudioMpeg4Playback->isGenericAudioFileInstance())
      {
        /*get the duration of the clip and assign it to clipInfo
          here we are assigning because in case of generic audio file
          media instance after knowing the bit rate only we can calculate
          the total duration*/
      clipInfo.info.Duration = clip.pAudioMpeg4Playback->GetDurationMsec();
      }
    }
  }
}
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
/* ======================================================================
FUNCTION
  Mpeg4Player::AudioTiming

DESCRIPTION
//
//  process an audio timing notifcation.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::AudioTiming(const AUDIO_TIMING_type *pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::AudioTiming" );

   if (pEvent)
   {
      //make sure this notice is still relevant by
      //checking the play ID tag and the state
      if (pEvent->playbackID==playbackID && playerState!=IDLE)
      {
#ifndef FEATURE_WINCE
         if (clip.bHasVideo && pActiveVideoPlayer)
         {
            pActiveVideoPlayer->TimingUpdate();
#ifdef SHOW_AUDIO_TICK
            QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Elpstime V %d A %d",
                     pActiveVideoPlayer->GetElapsedTime(),
                     AudioPlayerIf::GetElapsedTime(activeAudioPlayerId));
#endif
         }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
         if (clip.bHasText)
         {
            textPlayer.TimingUpdate();
#ifdef SHOW_AUDIO_TICK
            QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Elpstime T %d A %d",
                     textPlayer.GetElapsedTime(),
                     AudioPlayerIf::GetElapsedTime(activeAudioPlayerId));
#endif
         }
#endif
#endif   /*    FEATURE_WINCE   */

      }
   }
}

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

/* ======================================================================
FUNCTION
  Mpeg4Player::AudioStatus

DESCRIPTION
//
//  process an audio status notifcation.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::AudioStatus(const AUDIO_STATUS_type *pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::AudioStatus" );

   if (pEvent)
   {
      //make sure this notice is still relevant by
      //checking the play ID tag and the state
      if (pEvent->playbackID==playbackID)
      {
         //process notice
         const bool bVideo=false;
         switch (pEvent->status)
         {
            case Common::AUDIO_TRACK_STATE_CHANGE_NOTICE:
            {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
              "Received AUDIO_TRACK_STATE_CHANGE_NOTICE");
              #ifdef FEATURE_QTV_GENERIC_BCAST
              /* Update the track list information */
              UpdateTrackListInfo(QtvPlayer::MEDIA_AUDIO_ONLY);
              Notify(QtvPlayer::QTV_PLAYER_STATUS_TRACK_STATE_CHANGE);
              #endif /* FEATURE_QTV_GENERIC_BCAST */
              break;
            }
            case Common::AUDIO_BUFFERING:
               QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                 "Received AUDIO_BUFFERING, duration = %d", pEvent->durationBuffered);

               // If the state is REPOSITIONING, this message is for the previous
               // play section, and thus, too late. Don't update the duration
               // buffered, which should remain 0, so that when buffering events
               // happen, the duration starts from 0.
               if (playerState != REPOSITIONING)
               {
                 nPrerollDurationFilled = pEvent->durationBuffered;
               if(clip.pVideoMpeg4Playback)
               {
#ifndef FEATURE_QTV_GENERIC_BCAST
                 if(Media::BCAST_FLO_SOURCE == clip.pVideoMpeg4Playback->GetSource() &&
#else
                 if(clip.bLiveBcast &&
#endif
                    bAllowVidToSetOffSetOnAudUnderrun == true)
                 {
                   /*Reset the offset. Video will set it when it comes to rendering
                     or Audio will automatically set it when it's available */
                   m_mediaSync.ResetPlaybackOffset(AVSync::AudioAV);
                 }
               }
                 Buffering();
               }
               break;

            case Common::AUDIO_BUFFERING_END:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_BUFFERING_END");
               if(clip.pVideoMpeg4Playback)
               {
#ifndef FEATURE_QTV_GENERIC_BCAST
                 if(Media::BCAST_FLO_SOURCE == clip.pVideoMpeg4Playback->GetSource())
#else
                 if(clip.bLiveBcast)
#endif
                 {
                   /*This will allow video to set AV offset if once again
                     audio is not available */
                   bAllowVidToSetOffSetOnAudUnderrun = true;
                 }
               }
               AudioBufferingDone();
               break;

            case Common::AUDIO_PLAYING:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_PLAYING");
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
               if(clip.pAudioMpeg4Playback)
               {
                 QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "playerState %d lastPlayerState %d",playerState,lastPlayerState);
                 /*
                 * For generic audio formats, we need to reposition audio to the TS being played before DSP was released.
                 * For local playback, when DSP is released and AUDIO moves to PLAYING, player state would be SUSPENDED.
                 * In case of PD, player state would be BUFFERING but lastPlayerState would be SUSPENDED.
                 */
                 if( (clip.pAudioMpeg4Playback->isGenericAudioFileInstance())      &&
                     ((playerState == SUSPENDED)||(lastPlayerState == SUSPENDED))  &&
                     (clipInfo.info.RepositioningAllowed) )
                 {
                   bool bError;
                   QtvPlayer::PlayerStateRecordT ps;
                   GetPlayerState(ps);
                   /* here we are moving the audio cmx to the right offset
                     while resuming from the pause in case of generic audio formats */
                   clip.pAudioMpeg4Playback->SetAudioPosition(ps.playbackMsec, bError, ps.playbackMsec);
                 }
               }
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
               Playing();
               break;

            case Common::AUDIO_DONE:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_DONE");
               DonePlaying(false,bVideo);
               break;

            case Common::AUDIO_ERROR_ABORT:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_ERROR_ABORT");
               if (appControl.nAudioAbortsToIgnore <= 0)
               {
                 Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR);
                 DonePlaying(true,bVideo);
                 clip.bHasAudio=false;
    clip.bHasAudioWhenNotSuspended=false;
               }
               else
               {
                 appControl.nAudioAbortsToIgnore--;
               }
               break;

            case Common::AUDIO_EXCEPTION:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_EXCEPTION");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_ERROR);
               DonePlaying(true,bVideo);
               break;

            case Common::AUDIO_PAUSED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_PAUSED");
               break;

            case Common::AUDIO_SUSPENDED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_SUSPENDED");
               break;

            case Common::AUDIO_STOPPED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_STOPPED");
               break;

            case Common::AUDIO_DOWNLOAD_POLLING_START:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_DOWNLOAD_POLLING_START");
               break;

            case Common::AUDIO_RESTARTING_TRACK:
              Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_RESTARTING_TRACK);
              break;

            case Common::AUDIO_DOWNLOAD_POLLING_END:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_DOWNLOAD_POLLING_END");
               switch (playerState)
               {
                  case OPENING:
                     //time to parse the file and start pseudostreaming.
          /*
                     bool bOK;
                     bOK=false;
          not used any where to solve lint */
                      /* disable looping tracks for pseudo streaming */
                      bLoopTrackFlag = false;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
                     if (!FileSavedToBuffer)
                     {
                       clip.pVideoLocalBuffer = clip.pAudioLocalBuffer = NULL;
                     }
                     else
                     {
                    clip.pAudioLocalFilename = clip.pVideoLocalFilename = NULL;

                    clip.pVideoLocalBuffer =
                     clip.pAudioLocalBuffer   = (unsigned char*)pPVDL->getPDS2BufPtr();

                    clip.audioBufSize =
                     clip.videoBufSize = pPVDL->getPDS2BufSize();
                     }

                     if(!StartPlayingOrNotifyReady())
                     {
                        ErrorAbort(START_PSEUDO_STREAM_ERROR);
                     }
                     break;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

               }
               break;
            case Common::AUDIO_ELAPSE_TIME:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "receive AUDIO_ELAPSE_TIME");
               if (clip.bStreaming && redirectFlag)
               {
                  RedirectToNewURN();
               }
               else
               {
                  QTV_MSG2(QTVDIAG_RTSP, "Discard REDIRECT request (bStreaming=%d, redirect_flag=%d)",
                     clip.bStreaming, redirectFlag);
               }
               break;

            case Common::AUDIO_MEDIA_END:
              /* we have reached end of audio media/stream, but audio player/cmx
                 may still be playing last few frames */
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of audio media reached");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_AUDIO_MEDIA_END);
               break;

            case Common::AUDIO_RESTART:
              // Try to restart audio if it is worthy
              #define MAX_STARVING_TIME 2000
              long start,stop;
              (void)GetPlayTimes(start,stop);
              if (stop == -1 || (stop - start > MAX_STARVING_TIME))
              {
                appControl.bDoneAudioBuffering = false;
#ifndef FEATURE_WINCE
                  AudioPlayerIf::RestartAudio(activeAudioPlayerId);
#endif   /*    FEATURE_WINCE   */
              }
              break;

           case Common::AUDIO_PAUSED_DISABLED:
             {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_PAUSED_DISABLED");
               /* Notify change of status according to whether or not it is
               ** necessary to wait for video to be disabled.
               */
#ifndef FEATURE_WINCE
               if ( (!clip.bHasVideo || (pActiveVideoPlayer && pActiveVideoPlayer->IsDone())) &&
                  (!AudioPlayerIf::IsDone(activeAudioPlayerId))
                  )
               {
                 Notify(QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED);
               }
#endif   /*    FEATURE_WINCE   */
               break;
             }

            default:
               QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Received AUDIO_STATUS_??? %d",(int)pEvent->status);
               break;
         }
      } else
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received AUDIO_STATUS, ignored");
      }
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::VideoStatus

DESCRIPTION
//
//  process a video status notifcation.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::VideoStatus(const VIDEO_STATUS_type *pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::VideoStatus" );

   if (pEvent)
   {
      //make sure this notice is still relevant by
      //checking the play ID tag and the state
      if (pEvent->playbackID==playbackID)
      {
         //Process status
         const bool bVideo=true;
         switch (pEvent->status)
         {
            case Common::VIDEO_BUFFERING:
               QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                 "Received VIDEO_BUFFERING, duration = %d", pEvent->durationBuffered);

               // If the state is REPOSITIONING, this message is for the previous
               // play section, and thus, too late. Don't update the duration
               // buffered, which should remain 0, so that when buffering events
               // happen, the duration starts from 0.
               if ((clip.bStillImage && clip.bHasAudio) ||
                   (playerState == REPOSITIONING)||
                   ( clip.bHasAudio
#ifndef FEATURE_WINCE
               && AudioPlayerIf::IsPlaying(activeAudioPlayerId)
#endif   /*    FEATURE_WINCE   */
                       ))
               {
                 QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                              "Ignoring VIDEO_BUFFERING for still image");
               }
               else
               {
                 nPrerollDurationFilled = pEvent->durationBuffered;
                 Buffering();
               }
               break;

            case Common::VIDEO_PLAYING:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_PLAYING");
               Playing();
               break;

            case Common::VIDEO_DONE:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_DONE");
               DonePlaying(false,bVideo);
               break;

            case Common::VIDEO_ERROR_ABORT:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_ERROR_ABORT");
               if (appControl.nVideoAbortsToIgnore <= 0)
               {
                 VideoError();
                 DonePlaying(true,bVideo);
                 clip.bHasVideo=false;
               }
               else
               {
                 appControl.nVideoAbortsToIgnore--;
               }
               break;

            case Common::VIDEO_EXCEPTION:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_EXCEPTION");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR);
               DonePlaying(true,bVideo);
               break;

            case Common::VIDEO_PAUSED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_PAUSED");
               break;
            case Common::VIDEO_STOPPED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_STOPPED");
               break;
            case Common::VIDEO_SUSPENDED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_SUSPENDED");
               break;

            case Common::VIDEO_RENDERING:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_RENDERING");
               VideoRendering();
               break;

            case Common::VIDEO_NEXT_FRAME_DONE:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_NEXT_FRAME_DONE");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_DONE);
               break;
            case Common::VIDEO_NEXT_FRAME_FAILED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_NEXT_FRAME_FAILED");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_NEXT_FRAME_FAILED);
               break;
            case Common::VIDEO_PREV_FRAME_DONE:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_PREV_FRAME_DONE");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_DONE);
               break;
            case Common::VIDEO_PREV_FRAME_FAILED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_PREV_FRAME_FAILED");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_PREV_FRAME_FAILED);
               break;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
            case Common::VIDEO_ROTATION_DONE:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_ROTATION_DONE");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_ROTATION_DONE);
               break;
            case Common::VIDEO_ROTATION_FAILED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_ROTATION_FAILED");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_ROTATION_FAILED);
               break;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
            case Common::VIDEO_SCALING_DONE:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_SCALING_DONE");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_SCALING_DONE);
               break;
            case Common::VIDEO_SCALING_FAILED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_SCALING_FAILED");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_SCALING_FAILED);
               break;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS  || FEATURE_QTV_XSCALE_VIDEO*/

            case Common::VIDEO_RESTARTING_TRACK:
              Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESTARTING_TRACK);
              break;

            case Common::VIDEO_ELAPSE_TIME:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "receive VIDEO_ELAPSE_TIME");
               if (clip.bStreaming && redirectFlag)
               {
                  RedirectToNewURN();
               }
               else
               {
                  QTV_MSG2(QTVDIAG_RTSP, "Discard REDIRECT request (bStreaming=%d, redirect_flag=%d)",
                     clip.bStreaming, redirectFlag);
               }
               break;

            case Common::VIDEO_MEDIA_END:
              /* we have reached end of video media/stream, but video player may
                 still be playing last few frames */
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of video media reached");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_MEDIA_END);
               break;

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
           case Common::VIDEO_RELEASE_DONE:
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_RELEASE_DONE");
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#else /* FEATURE_QTV_QOS_SELECTION */
              Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_DONE);
#endif /* FEATURE_QTV_QOS_SELECTION */

              break;

           case Common::VIDEO_RELEASE_FAILED:
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_RELEASE_FAILED");
#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#else /* FEATURE_QTV_QOS_SELECTION */
              Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RELEASE_FAILED);
#endif /* FEATURE_QTV_QOS_SELECTION */
              break;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

           case Common::VIDEO_DISABLE_DONE:
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                          "Received VIDEO_DISABLE_DONE");
             if (playerState == SUSPENDED)
             {
               Notify(QtvPlayer::QTV_PLAYER_STATUS_SUSPENDED);
             }
             break;

           case Common::VIDEO_DISABLE_FAILED:
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                          "Received VIDEO_DISABLE_FAILED");
             if (playerState == SUSPENDED)
             {
               Notify(QtvPlayer::QTV_PLAYER_STATUS_SUSPEND_FAILED);
             }
             break;

           case Common::VIDEO_ENABLE_DONE:
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_ENABLE_DONE");
             break;

           case Common::VIDEO_ENABLE_FAILED:
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Received VIDEO_ENABLE_FAILED");
             Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_ERROR);
             break;

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */

            default:
               QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Received VIDEO_STATUS_??? %d",(int)pEvent->status);
               break;
         }
      } else
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received VIDEO_STATUS, ignored");
      }
   }
}

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION
  Mpeg4Player::TimedTextStatus

DESCRIPTION
//
// process a TimedText status notification.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::TimedTextStatus(const CPVTEXT_STATUS_type *pEvent)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::TimedTextStatus" );

   if (pEvent)
   {
      //make sure this notice is still relevant by
      //checking the play ID tag and the state
      if (pEvent->playbackID==playbackID)
      {
         //Process status
         const bool bVideo=false;
         switch (pEvent->status)
         {
            case Common::CPVTEXT_BUFFERING:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_BUFFERING");
               if(!textPlayer.IsDone())
               Buffering();
               break;

            case Common::CPVTEXT_PLAYING:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_PLAYING");
               Playing();
               break;

            case Common::CPVTEXT_DONE:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_DONE");
               DonePlaying(false,bVideo);
               break;

            case Common::CPVTEXT_ERROR_ABORT:
               if (appControl.nTextAbortsToIgnore <= 0)
               {
                 QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_ERROR_ABORT");
                 Notify(QtvPlayer::QTV_PLAYER_STATUS_TEXT_ERROR);
                 DonePlaying(true,bVideo);
                 clip.bHasText=false;
               }
               else
               {
                 appControl.nTextAbortsToIgnore--;
               }
               break;

            case Common::CPVTEXT_EXCEPTION:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_EXCEPTION");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_TEXT_ERROR);
               DonePlaying(true,bVideo);
               break;

            case Common::CPVTEXT_PAUSED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_PAUSED");
               break;

            case Common::CPVTEXT_STOPPED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_STOPPED");
               break;

            case Common::CPVTEXT_SUSPENDED:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_SUSPENDED");
               break;

            case Common::CPVTEXT_RENDERING:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_RENDERING");
               //<tbd>VideoRendering();
               break;

            case Common::CPVTEXT_RESTARTING_TRACK:
              Notify(QtvPlayer::QTV_PLAYER_STATUS_TEXT_RESTARTING_TRACK);
              break;

            case Common::CPVTEXT_ELAPSE_TIME:
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "receive CPVTEXT_ELAPSE_TIME");
               if (clip.bStreaming && redirectFlag)
               {
                  RedirectToNewURN();
               }
               else
               {
                  QTV_MSG2(QTVDIAG_RTSP, "Discard REDIRECT request (bStreaming=%d, redirect_flag=%d)",
                     clip.bStreaming, redirectFlag);
               }
               break;

            case Common::CPVTEXT_MEDIA_END:
              /* we have reached end of text media/stream, but text player may
                 still be playing last few frames */
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "End of text media reached");
               Notify(QtvPlayer::QTV_PLAYER_STATUS_TEXT_MEDIA_END);
               break;

            default:
               QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Received CPVTEXT_STATUS_??? %d",(int)pEvent->status);
               break;
         }
      } else
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received CPVTEXT_STATUS, ignored");
      }
   }
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::ParserStatus

DESCRIPTION
//
// process an EVENT received from parser thread.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */

void Mpeg4Player::ParserStatus(const QTV_PS_PARSER_STATUS_EVENT_type *pEvent)
{
  PV_PLAY_CLIP_type *pPlayClipEvent = NULL;
  //Set local file size
  int nVideoFileSize, nAudioFileSize, nTextFileSize;
  nVideoFileSize = nAudioFileSize = nTextFileSize = 0;

   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::ParserStatus" );
   if (pEvent)
   {
       parserStatus = (ParserStatusCode)(pEvent->status);
       switch (pEvent->status)
       {
        case Common::PARSER_IDLE:
               QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_IDLE");
           break;

        case Common::PARSER_READY:
               if(pEvent->bHasVideo)
                 QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_READY from Video");
               if(pEvent->bHasAudio)
                 QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_READY from Audio");

               qtv_event_parser_ready_type qtv_event_parser_ready;
#ifdef FEATURE_QTV_PSEUDO_STREAM
                    appControl.mediaCounter++;
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

#ifdef FEATURE_QTV_PSEUDO_STREAM
               if(clip.pVideoMpeg4Playback && pEvent->bHasVideo)
                 qtv_event_parser_ready.fragment_number =
                        clip.pVideoMpeg4Playback->GetParseFragmentNum();
               else if(clip.pAudioMpeg4Playback && pEvent->bHasAudio)
                 qtv_event_parser_ready.fragment_number =
                        clip.pAudioMpeg4Playback->GetParseFragmentNum();
               else
                 qtv_event_parser_ready.fragment_number = 0;

               if(clip.videoBufSize != 0 && pEvent->bHasVideo)
                 qtv_event_parser_ready.unfilled_buffer_size =
                        clip.videoBufSize - clip.pseudoStreamWritePtrOffset;
               else if (clip.audioBufSize != 0 && pEvent->bHasAudio)
                 qtv_event_parser_ready.unfilled_buffer_size =
                        clip.audioBufSize - clip.pseudoStreamWritePtrOffset;
               else
                 qtv_event_parser_ready.unfilled_buffer_size = 0;

               qtv_event_parser_ready.buffer_fill_rate =
                        appControl.nPseudoStreamDownloadRate;
#ifndef FEATURE_WINCE
               if(clip.bPseudoStreaming)
                 event_report_payload(
                        EVENT_QTV_PARSER_STATE_READY,
                        sizeof(qtv_event_parser_ready_type),
                        &qtv_event_parser_ready
                       );
#endif   /*    FEATURE_WINCE   */
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

         Notify(QtvPlayer::QTV_PS_PARSER_STATUS_READY);

               switch (playerState)
               {
                  case OPENING:
#ifdef FEATURE_QTV_PSEUDO_STREAM
                      if(appControl.mediaCounter != clip.mediaCounter)
                          break;
                      else
                          clip.mediaCounter = 0;
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

                     if(clip.pVideoLocalFilename)
                        nVideoFileSize = SetLocalFileSize(clip.pVideoLocalFilename);
                     else
                        nVideoFileSize = clip.videoBufSize;

                     if(clip.pAudioLocalFilename)
                        nAudioFileSize = SetLocalFileSize(clip.pAudioLocalFilename);
                     else
                        nAudioFileSize = clip.audioBufSize;

                     if(clip.pTextLocalFilename)
                        nTextFileSize = SetLocalFileSize(clip.pTextLocalFilename);
                     else
                        nTextFileSize = clip.textBufSize;

                     QTV_MSG(QTVDIAG_MP4_PLAYER,
                             "PARSER_READY, when player is in state OPENING");
                      //Describe
                     if (!DescribeClip( clip.pVideoMpeg4Playback, nVideoFileSize,
                                        clip.pAudioMpeg4Playback, nAudioFileSize,
                                        clip.pTextMpeg4Playback, nTextFileSize) ||
                         !(clip.bHasVideo || clip.bHasAudio
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                           || clip.bHasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
                          ))
                     {
                       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OpenLocalFile: Can't describe clip");
                     }

                     //Select
                     if ( clip.bHasVideo && clip.pVideoMpeg4Playback )
                     {
                        //get width and height
                        clipInfo.info.Width = 0;
                        clipInfo.info.Height = 0;
                        (void)clip.pVideoMpeg4Playback->GetVideoDimensions((uint32*)&clipInfo.info.Width, (uint32*)&clipInfo.info.Height);
                        if( (clipInfo.info.Width > MP4_MAX_DECODE_WIDTH) ||
                            (clipInfo.info.Height > MP4_MAX_DECODE_WIDTH) )
                        {
                          Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED);
                          QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_RESOLUTION_NOT_SUPPORTED H=%ld, W=%ld", clipInfo.info.Height, clipInfo.info.Width);
                        }
                        clipInfo.info.VideoFrameRate=clip.pVideoMpeg4Playback->GetVideoFrameRate();
                        clipInfo.info.VideoEncodedBitRate=clip.pVideoMpeg4Playback->GetVideoBitRate();
                     }
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#else
                     if ( clip.bHasAudio && clip.pAudioMpeg4Playback )
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
                     {
                        clipInfo.info.AudioSamplingFreq=clip.pAudioMpeg4Playback->GetAudioSamplingFreq(0);
                        clipInfo.info.NumAudioChannels=clip.pAudioMpeg4Playback->GetNumAudioChannels(0);
                        clipInfo.info.AudioEncodedBitRate=clip.pAudioMpeg4Playback->GetAudioBitRate(0);
                     }
                    /* if we have not got movie duration, get in now */
                    if(!clipInfo.info.Duration)
                    {
                      if(clip.bHasVideo && clip.pVideoMpeg4Playback)
                        clipInfo.info.Duration = clip.pVideoMpeg4Playback->GetDurationMsec();
                      else if(clip.bHasAudio && clip.pAudioMpeg4Playback)
                        clipInfo.info.Duration = clip.pAudioMpeg4Playback->GetDurationMsec();
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                      else if(clip.bHasText && clip.pTextMpeg4Playback)
                        clipInfo.info.Duration = clip.pTextMpeg4Playback->GetDurationMsec();
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
                    }

                    Notify(PB_READY);
                     if(appControl.bStartPlaying)
                     {
                       if (!StartPlaying())
                       {
                         ErrorAbort(START_PLAYING_ERROR);
                       }
                       else
                       {
#ifdef FEATURE_QTV_PSEUDO_STREAM
#ifndef FEATURE_WINCE
                         if(clip.bPseudoStreaming)
                           event_report(EVENT_QTV_PSEUDO_STREAM_STARTED);
                         event_report_payload(EVENT_QTV_CLIP_STARTED,
                                                sizeof(qtv_event_clip_started_type),
                                                &qtv_event_clip_started_payload);
#endif   /*    FEATURE_WINCE   */
#endif /*FEATURE_QTV_PSEUDO_STREAM*/
                       }
                     }
#ifdef FEATURE_QTV_PSEUDO_STREAM
                     appControl.mediaCounter = 0;
#endif /* FEATURE_QTV_PSEUDO_STREAM */
                     break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
                  case PLAYER_PSEUDO_PAUSE:
                     QTV_MSG(QTVDIAG_MP4_PLAYER,
                             "PARSER_READY, when player is in state PLAYER_PSEUDO_PAUSE");

                     pPlayClipEvent = QCCreateMessage(PV_PLAY_CLIP, this);/*lint!e641 */
                     if (pPlayClipEvent)
                     {
                       pPlayClipEvent->startTime=-1;
                       pPlayClipEvent->stopTime=-1;
                       PostMessage(pPlayClipEvent);
                     }
                     else
                     {
                       ErrorAbort(QTV_PSEUDO_RESUME_ERROR);
                     }

                     qtv_event_pp_duration_type qtv_event_pp_duration;
                     qtv_event_pp_duration.pseudo_pause_duration =
                                   pvGetTickCount() - player_pp_entry_time;
#ifndef FEATURE_WINCE
                     event_report_payload(
                                   EVENT_QTV_PLAYER_STATE_PSEUDO_RESUME,
                                   sizeof(qtv_event_pp_duration_type),
                                   &qtv_event_pp_duration
                         );
#endif   /*    FEATURE_WINCE   */
                     break;
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

                  case PLAYING:
                  case BUFFERING:
                     QTV_MSG1( QTVDIAG_MP4_PLAYER,
                               "PARSER_READY, when player is in state %d",playerState);
                     if ((clip.pVideoMpeg4Playback == clip.pAudioMpeg4Playback)
                          && (clip.pVideoMpeg4Playback))
                     {
                       clip.pVideoMpeg4Playback->resumeMedia();
                     }
                     else
                     {
                       if(clip.bHasAudio && clip.pAudioMpeg4Playback &&
                          pEvent->bHasAudio)
                       {
                         clip.pAudioMpeg4Playback->resumeMedia();
                       }
                       if(clip.bHasVideo && clip.pVideoMpeg4Playback &&
                          pEvent->bHasVideo)
                       {
                         clip.pVideoMpeg4Playback->resumeMedia();
                       }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                       if(clip.bHasText && clip.pTextMpeg4Playback &&
                          pEvent->bHasText)
                       {
                         clip.pTextMpeg4Playback->resumeMedia();
                       }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
                     }
#ifdef FEATURE_QTV_PSEUDO_STREAM
                     appControl.mediaCounter = 0;
#endif /* FEATURE_QTV_PSEUDO_STREAM */
                     break;

                  default:
                     QTV_MSG1( QTVDIAG_MP4_PLAYER,
                               "PARSER_READY, when player is in state %d",playerState);
                     /*Do nothing */
#ifdef FEATURE_QTV_PSEUDO_STREAM
                     appControl.mediaCounter = 0;
#endif /* FEATURE_QTV_PSEUDO_STREAM */
                     break;
               } //switch player state
         break;

       case Common::PARSER_PAUSE:
               QTV_MSG(QTVDIAG_MP4_PLAYER, "Received PARSER_PAUSE");

#ifdef FEATURE_QTV_PSEUDO_STREAM
               if(!parser_pause_entry_time)
                  parser_pause_entry_time = pvGetTickCount();

         //Start timer for pull interface
         if(clip.FetchBufferedDataSize)
         {
           HttpPullTimerStart();
         }
#endif /*FEATURE_QTV_PSEUDO_STREAM*/
         Notify(QtvPlayer::QTV_PS_PARSER_STATUS_PAUSE);
         break;

       case Common::PARSER_RESUME:
         QTV_MSG(QTVDIAG_MP4_PLAYER, "Received PARSER_RESUME");

#ifdef FEATURE_QTV_PSEUDO_STREAM
               qtv_event_pp_duration_type qtv_event_parser_pause_duration;
               qtv_event_parser_pause_duration.pseudo_pause_duration =
                                  pvGetTickCount() - parser_pause_entry_time;
#ifndef FEATURE_WINCE
               event_report_payload(
                              EVENT_QTV_PARSER_STATE_PSEUDO_RESUME,
                              sizeof(qtv_event_pp_duration_type),
                              &qtv_event_parser_pause_duration
                              );
#endif   /*    FEATURE_WINCE   */
               parser_pause_entry_time = 0;

         //Start timer for pull interface
         if(clip.FetchBufferedDataSize)
         {
           HttpPullTimerStart();
         }
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

         Notify(QtvPlayer::QTV_PS_PARSER_STATUS_RESUME);
         break;

       case Common::PARSER_FAILED:
         QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_FAILED");
         /* Following Notify() is not necessary if parser thread
            it self sends notification to OEM layer.
         */
         Notify(QtvPlayer::QTV_PS_PARSER_STATUS_FAILED);
         switch (playerState)
         {
          case OPENING:
             QTV_MSG( QTVDIAG_MP4_PLAYER,
                    "PARSER_FAILED when player is in state OPENING");
             ErrorAbort(OPEN_CLIP_ERROR);/*moves the player to IDLE state*/
             break;

          case PB_READY:

            QTV_MSG( QTVDIAG_MP4_PLAYER,
                    "PARSER_FAILED when player is in state PB_READY");

            QTV_MSG( QTVDIAG_MP4_PLAYER,
            "This should never happen. Please fix mpeg4player state machine");

            ErrorAbort(START_PLAYING_ERROR);/*moves the player to IDLE state*/

            break;

#ifdef FEATURE_QTV_PSEUDO_STREAM
          case PLAYER_PSEUDO_PAUSE:
             QTV_MSG( QTVDIAG_MP4_PLAYER,
                    "PARSER_FAILED when player is in state PLAYER_PSEUDO_PAUSE");
             ErrorAbort(QTV_PSEUDO_PARSER_FAILURE);/*moves the player to IDLE state*/
             break;
#endif /*FEATURE_QTV_PSEUDO_STREAM*/

          default:
             QTV_MSG1( QTVDIAG_MP4_PLAYER,
                       "PARSER_FAILED when player is in state %d",playerState);
             /*Do nothing */
             break;
         }
         break;
       }
   }
}
#endif /*FEATURE_FILE_FRAGMENTATION*/

#ifdef FEATURE_QTV_PSEUDO_STREAM
/* ======================================================================
FUNCTION
  Mpeg4Player::PseudoPauseHandler

DESCRIPTION
//
// process the event QTV_PS_PAUSE received from audio/video/text thread
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::PseudoPauseHandler(const QTV_PS_PAUSE_EVENT_type *)
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Received PSEUDO_PAUSE");
#ifndef FEATURE_WINCE
    event_report(EVENT_QTV_PLAYER_STATE_PSEUDO_PAUSE);
#endif   /*    FEATURE_WINCE   */
    player_pp_entry_time = pvGetTickCount();
    Notify(PLAYER_PSEUDO_PAUSE);
   //Start timer for pull interface
   if(clip.FetchBufferedDataSize)
   {
     HttpPullTimerStart();
   }
}
#endif /* FEATURE_QTV_PSEUDO_STREAM */


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

/* ======================================================================
FUNCTION
  Mpeg4Player::HttpPullDataStart

DESCRIPTION
//
// Start a timer if not already started and start pulling data from OEM.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::HttpPullTimerStart()
{
    if(!httpPullData.bTimerSet)
    {
       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "HttpPullDataStart: Interval=%d"
                      ,QTV_HTTP_PULL_DATA_INTERVAL );
    (void)rex_set_timer(httpPullData.pHttpPullTimer,QTV_HTTP_PULL_DATA_INTERVAL);
       httpPullData.bTimerSet = true;
    }

    return;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::HttpPullDataStop

DESCRIPTION
//
// Stop the timer if not already stopped pulling data from OEM.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::HttpPullTimerStop()
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "HttpPullDataStop: called..!");
    httpPullData.bTimerSet = false;
    return;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
/* ======================================================================
FUNCTION
  Mpeg4Player::ParserStatus

DESCRIPTION
//
// process an EVENT received from parser thread.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::HTTPParserStatus(const QTV_HTTP_PARSER_STATUS_EVENT_type *pEvent)
{
  //Set local file size
  uint32 nVideoFileSize, nAudioFileSize, nTextFileSize;
  nVideoFileSize = nAudioFileSize = nTextFileSize = 0;

   QTV_MSG( QTVDIAG_MP4_PLAYER, "Mpeg4Player::HTTPParserStatus" );
   if (pEvent)
   {
       parserStatus = (ParserStatusCode)(pEvent->status);
       switch (pEvent->status)
       {
        case Common::PARSER_IDLE:
               QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_IDLE");
           break;

        case Common::PARSER_READY:
               if(pEvent->bHasVideo)
                 QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_READY from Video");
               if(pEvent->bHasAudio)
                 QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_READY from Audio");

               qtv_event_parser_ready_type qtv_event_parser_ready;

                   appControl.mediaCounter++;

               if(clip.pVideoMpeg4Playback && pEvent->bHasVideo)
                 qtv_event_parser_ready.fragment_number = 1;
               else if(clip.pAudioMpeg4Playback && pEvent->bHasAudio)
                 qtv_event_parser_ready.fragment_number = 1;

               if(clip.videoBufSize != 0 && pEvent->bHasVideo)
                 qtv_event_parser_ready.unfilled_buffer_size =
                        clip.videoBufSize - clip.HTTPStreamWritePtrOffset;
               else if (clip.audioBufSize != 0 && pEvent->bHasAudio)
                 qtv_event_parser_ready.unfilled_buffer_size =
                        clip.audioBufSize - clip.HTTPStreamWritePtrOffset;
               else
                 qtv_event_parser_ready.unfilled_buffer_size = 0;

               qtv_event_parser_ready.buffer_fill_rate =
                        appControl.nHTTPStreamDownloadRate;
#ifndef FEATURE_WINCE

               if(clip.bHTTPStreaming)
                 event_report_payload(
                        EVENT_QTV_PARSER_STATE_READY,
                        sizeof(qtv_event_parser_ready_type),
                        &qtv_event_parser_ready
                       );
#endif   /*    FEATURE_WINCE   */

         Notify(QtvPlayer::QTV_HTTP_PARSER_STATUS_READY);

         switch (playerState)
               {
                  case OPENING:
                      if(appControl.mediaCounter != clip.mediaCounter)
                          break;
                      else
                          clip.mediaCounter = 0;

                     //Stop timer for pull interface
                     if(clip.FetchBufferedDataSize)
                     {
                        HttpPullTimerStop();
                     }

                     if(clip.pVideoLocalFilename)
                        nVideoFileSize = SetLocalFileSize(clip.pVideoLocalFilename);
                     else
                     {
                         if(clip.FetchBufferedDataSize)
                         {
#if defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ) && defined(FEATURE_QTV_GENERIC_AUDIO_FORMAT)
                            if(clipInfo.info.Size>0)
                            {
                              nVideoFileSize = clipInfo.info.Size;
                            }
                            else

#endif
                            {
                             //For Pull interface we dont know the file size.
                             nVideoFileSize = 0x7FFFFFFF;
                            }

                         }
                         else
                         {
                        nVideoFileSize = clip.videoBufSize;
                         }
                     }

                     if(clip.pAudioLocalFilename)
                        nAudioFileSize = SetLocalFileSize(clip.pAudioLocalFilename);
                     else
                     {
                         if(clip.FetchBufferedDataSize)
                         {
#if defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD ) && defined(FEATURE_QTV_GENERIC_AUDIO_FORMAT)
                            if(clipInfo.info.Size>0)
                            {
                              nAudioFileSize = clipInfo.info.Size;
                            }
                            else

#endif
                            {
                             //For Pull interface we dont know the file size.
                             nAudioFileSize = 0x7FFFFFFF;
                            }

                         }
                         else
                         {
                        nAudioFileSize = clip.audioBufSize;
                         }
                     }

                     if(nVideoFileSize)
                       nTextFileSize = nVideoFileSize;
                     else
                       nTextFileSize = nAudioFileSize;

                     QTV_MSG(QTVDIAG_MP4_PLAYER,
                             "PARSER_READY, when player is in state OPENING");
                      //Describe
                     if (!DescribeClip(clip.pVideoMpeg4Playback, nVideoFileSize,
                                       clip.pAudioMpeg4Playback, nAudioFileSize,
                                       clip.pTextMpeg4Playback, nTextFileSize) ||
                         !(clip.bHasVideo || clip.bHasAudio
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                           || clip.bHasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
                          ))
                     {
                       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                           "HTTPParserStatus: Can't describe clip");
                     }

                     if ( clip.bHasVideo && clip.pVideoMpeg4Playback )
                     {
                       //get width and height
                       clipInfo.info.Width = 0;
                       clipInfo.info.Height = 0;
                       (void)clip.pVideoMpeg4Playback->GetVideoDimensions((uint32*)&clipInfo.info.Width, (uint32*)&clipInfo.info.Height);
                       if( (clipInfo.info.Width > MP4_MAX_DECODE_WIDTH) ||
                           (clipInfo.info.Height > MP4_MAX_DECODE_WIDTH ) )
                       {
                         Notify(QtvPlayer::QTV_PLAYER_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED);
                         QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VIDEO_RESOLUTION_NOT_SUPPORTED H=%ld, W=%ld", clipInfo.info.Height, clipInfo.info.Width);
                       }
                       clipInfo.info.VideoFrameRate=clip.pVideoMpeg4Playback->GetVideoFrameRate();
                       clipInfo.info.VideoEncodedBitRate=clip.pVideoMpeg4Playback->GetVideoBitRate();
                     }

#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#else
                     if ( clip.bHasAudio && clip.pAudioMpeg4Playback )
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
                     {
                       clipInfo.info.AudioSamplingFreq=clip.pAudioMpeg4Playback->GetAudioSamplingFreq(0);
                       clipInfo.info.NumAudioChannels=clip.pAudioMpeg4Playback->GetNumAudioChannels(0);
                       clipInfo.info.AudioEncodedBitRate=clip.pAudioMpeg4Playback->GetAudioBitRate(0);
                     }

                    /* if we have not got movie duration, get in now */
                    if(!clipInfo.info.Duration)
                    {
                       if(clip.bHasVideo && clip.pVideoMpeg4Playback)
                         clipInfo.info.Duration = clip.pVideoMpeg4Playback->GetDurationMsec();
                       else if(clip.bHasAudio && clip.pAudioMpeg4Playback)
                         clipInfo.info.Duration = clip.pAudioMpeg4Playback->GetDurationMsec();
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                       else if(clip.bHasText && clip.pTextMpeg4Playback)
                         clipInfo.info.Duration = clip.pTextMpeg4Playback->GetDurationMsec();
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
                     }

                     Notify(PB_READY);

                     if(appControl.bStartPlaying)
                     {
                       if (!StartPlaying())
                       {
                         ErrorAbort(START_PLAYING_ERROR);
                       }
                       else
                       {
                         //if(clip.bHTTPStreaming)
                           //event_report(EVENT_QTV_PSEUDO_STREAM_STARTED);
#ifndef FEATURE_WINCE

                         event_report_payload(EVENT_QTV_CLIP_STARTED,
                                              sizeof(qtv_event_clip_started_type),
                                              &qtv_event_clip_started_payload);
#endif   /*    FEATURE_WINCE   */
                       }
                     }
                     appControl.mediaCounter = 0;
                     break;

                  default:
                     QTV_MSG1( QTVDIAG_MP4_PLAYER,
                               "PARSER_READY, when player is in state %d",playerState);
                     appControl.mediaCounter = 0;
                     break;
               } //switch player state
         break;

      case Common::PARSER_PAUSE:
               QTV_MSG(QTVDIAG_MP4_PLAYER, "Received PARSER_PAUSE");

               if(!parser_pause_entry_time)
                  parser_pause_entry_time = pvGetTickCount();

               Notify(QtvPlayer::QTV_HTTP_PARSER_STATUS_PAUSE);
               //Start timer for pull interface
               if(clip.FetchBufferedDataSize)
               {
                   HttpPullTimerStart();
               }
         break;

      case Common::PARSER_RESUME:
         QTV_MSG(QTVDIAG_MP4_PLAYER, "Received PARSER_RESUME");

               qtv_event_pp_duration_type qtv_event_parser_pause_duration;
               qtv_event_parser_pause_duration.pseudo_pause_duration =
                                  pvGetTickCount() - parser_pause_entry_time;
#ifndef FEATURE_WINCE

               event_report_payload(
                              EVENT_QTV_PARSER_STATE_PSEUDO_RESUME,
                              sizeof(qtv_event_pp_duration_type),
                              &qtv_event_parser_pause_duration
                              );
#endif   /*    FEATURE_WINCE   */
               parser_pause_entry_time = 0;
               //Start timer for pull interface
               if(clip.FetchBufferedDataSize)
               {
                   HttpPullTimerStart();
               }

         Notify(QtvPlayer::QTV_HTTP_PARSER_STATUS_RESUME);
         break;

      case Common::PARSER_FAILED:
         QTV_MSG( QTVDIAG_MP4_PLAYER, "Received PARSER_FAILED");
         /* Following Notify() is not necessary if parser thread
            it self sends notification to OEM layer.
         */

         Notify(QtvPlayer::QTV_HTTP_PARSER_STATUS_FAILED);
         switch (playerState)
         {
          case OPENING:
             QTV_MSG( QTVDIAG_MP4_PLAYER,
                    "PARSER_FAILED when player is in state OPENING");
             ErrorAbort(OPEN_CLIP_ERROR);/*moves the player to IDLE state*/
             break;

          case PB_READY:

            QTV_MSG( QTVDIAG_MP4_PLAYER,
                    "PARSER_FAILED when player is in state PB_READY");

            QTV_MSG( QTVDIAG_MP4_PLAYER,
            "This should never happen. Please fix mpeg4player state machine");

            ErrorAbort(START_PLAYING_ERROR);/*moves the player to IDLE state*/

            break;

          default:
             QTV_MSG1( QTVDIAG_MP4_PLAYER,
                       "PARSER_FAILED when player is in state %d",playerState);
             /*Do nothing */
             break;
         }
         break;
     }
   }
}

#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

/* ======================================================================
FUNCTION
  Mpeg4Player::StartStreaming

DESCRIPTION
  Start streaming, from Ready or Repositioning state

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::StartStreaming
(
  const URL* urlToSwitch
)
{
  bool bError = false;
  bool rslt = false;

  bool reconnecting = (streamer && (streamer->getReconnectionStatus() !=
                                                 RECONNECT_STATUS_INVALID));


  bool enablePipelining = (qtvConfigObject.GetQTVConfigItem(
    QtvConfig::QTVCONFIG_ENABLE_PIPELINING_FOR_STREAMING))>0?true:false;
  
   if ((!reconnecting && playerState!=PB_READY
       && playerState!=REPOSITIONING)
#ifdef FEATURE_QTV_FCS
#error code not present
#elif FEATURE_QTV_PIPELINING 
#error code not present
#endif
       )
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Weird state in StartStreaming");
      return false;
   }

#ifdef FEATURE_QTV_FCS
#error code not present
#endif /* FEATURE_QTV_FCS */

   long start,stop;
   bool reposition = (playerState==REPOSITIONING);

   if (!GetPlayTimes(start,stop))
   {
      Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAY_TIME_ERROR);
   } else
   {
      //save stream times
      reposControl.nStreamStop=stop;
      reposControl.nStreamStart=start;

      //note: there seems to be a bug in pvstream, if the stop time is
      //too close to the start time we never get data.  as a workaround
      //I'm always using "end of clip" for the stop time.  the AV players
      //will detect the stop time anyway.

      QTV_MSG2(QTVDIAG_STREAMING,
               "Elapsed time since start of play = %d and New play start time = %d", GetElapsedTime(), start);

      if (streamer && reconnecting)
      {
        if (!reposition)
        {
          start = GetElapsedTime();
        }
        if (playerState != PB_READY)
        {
          reposition = true;
        }
      }

if(streamer)
      {
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
        {
          rslt = streamer->StartStreaming(bError,start,-1/*stop*/,reposition);
        }
      }

      if (rslt)
      {
         if (bError)
         {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from StartStreaming");
            ErrorAbort(STREAM_START_ERROR);
         }
         else
         {
            if (playerState == REPOSITIONING)
            {
               qtv_event_clip_reposition_resume_type  qtv_event_clip_reposition_resume;
               qtv_event_clip_reposition_resume.resume_time = start;
               // for PAYLOAD use the "start"
#ifndef FEATURE_WINCE
               event_report_payload(EVENT_QTV_CLIP_REPOSITION_RESUME, // for STREAMING only
                                    sizeof(qtv_event_clip_reposition_resume_type),
                                    &qtv_event_clip_reposition_resume);
#endif
            }// if

            //wait on STREAM_START message
            return true;
         }
      }
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
   }
   QTV_USE_ARG1(urlToSwitch);
   return (bError == false);
}

/* ======================================================================
FUNCTION
  Mpeg4Player::RTSPStatusNotify

DESCRIPTION
  Check the status for a valid RTSP response from server that is other
 than "ok".  If so, and there is a registered callback handler for
 such statuses, forward the status and its text equivalent to that
 handler.
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

========================================================================== */
void Mpeg4Player::RTSPStatusNotify(const STREAMER_STATUS_type *pEvent)
{
  if ((pEvent->serverCode != CodeOK) && (pEvent->serverCode != 0) )
  {
    void *userData = NULL;
    Mpeg4Player::CallbackData cb ;
    QtvPlayer::PlayerStateRecordT ps;
    GetCallback(cb,ps);

    userData = QtvPlayer::GetRTSPStatusUserData(cb.handle);
    if (NULL != userData)
    {
      cb.pUserData = userData;
    }

    if (cb.handle)
    {
      QtvPlayer::RTSPStatusInstanceCallbackT fcnToInvoke =
                   QtvPlayer::GetRTSPStatusCallback(cb.handle);
      if (fcnToInvoke != 0)
      {
        fcnToInvoke(pEvent->serverCode, pEvent->serverMsg,
                    cb.handle, cb.pUserData);
      }
    }
    else
    {
      QtvPlayer::RTSPStatusCallbackT fcnToInvoke =
                         QtvPlayer::GetRTSPStatusCallback();
      QtvPlayer::RTSPStatusInstanceCallbackT instanceFcnToInvoke =
                   QtvPlayer::GetRTSPStatusCallback(cb.handle);
      if (cb.pUserData && instanceFcnToInvoke)
      {
        instanceFcnToInvoke(pEvent->serverCode, pEvent->serverMsg,
                            NULL, cb.pUserData);
      }
      else if (fcnToInvoke != 0)
      {
        fcnToInvoke(pEvent->serverCode, pEvent->serverMsg);
      }
    }
  }
}


/* ======================================================================
FUNCTION
  Mpeg4Player::StreamerStatus

DESCRIPTION
 //
 // process a streamer status notification.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::StreamerStatus(const STREAMER_STATUS_type *pEvent)
{
  QTV_NULL_PTR_CHECK(pEvent, RETURN_VOID);

  //make sure notification is valid.
  bool bOK = false;
  if (m_useMAPILogic)
  {
    bOK = (clip.bStreaming && streamer );
  }
  else
  {
    bOK = ((pEvent->streamID == streamID) &&
           clip.bStreaming && streamer &&
           (clip.pVideoMpeg4Playback != NULL));
  }

  if (bOK)
  {
    //Process the event.
    switch (pEvent->code)
    {
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
    case Common::STREAM_OPEN_COMPLETE:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_OPEN_COMPLETE");
      break;
    case Common::STREAM_OPEN_FAIL:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_OPEN_FAIL");
      Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
      ErrorAbort(OPEN_CLIP_ERROR);
      break;
    case Common::STREAM_CONNECTING:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_CONNECTING");
      Notify(CONNECTING);
      break;
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */
#ifdef FEATURE_QTV_FCS
#error code not present
#elif FEATURE_QTV_PIPELINING 
#error code not present
#endif
    case Common::STREAM_CONNECTED:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_CONNECTED");
      if (playerState != CONNECTING)
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
          "streamer status STREAM_CONNECTED but state = %d != CONNECTING", playerState);
        break;
      }

      /* This is initial buffering
       * passing a 'false' here sets all buffering parameters
       */
      bool bOK = true;
      //Mark the tracks to play.
      if (DescribeClip(clip.pVideoMpeg4Playback,0, NULL,0, NULL,0))
      {
        if (!(clip.bHasAudio || clip.bHasVideo
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
          || clip.bHasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
          ))
        {
          bOK = false;

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
          switch (clipInfo.info.streamtype)
          {
#ifdef FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST
            case QtvPlayer::MEDIA_WM_SERVER_SIDE_PLAYLIST:
              // No tracks selected in first clip of server-side
              // side playlist, notify the user so they can decide
              // what to do. The clip info should be set correctly.
              bOK = true;
              break;
#endif /* FEATURE_QTV_WM_SERVER_SIDE_PLAYLIST */

            default:
              // Not a playlist in which the clip encoding can
              // change.
              // Leave bOK false so we abort.
              break;
          }
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
        }
      }
      else
      {
        bOK = false;
      }

      if (!bOK)
      {
        Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
        ErrorAbort(STREAM_CONNECT_ERROR);
        break;
      }

      // move to PB_READY only when connected status is received from status
      // otherwise fast startup(play) is effected with the state change
       if(pEvent->code == Common::STREAM_CONNECTED)
        {
          Notify(PB_READY);
     }
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
      if (m_useMAPILogic && clip.pVideoMpeg4Playback != NULL)
      {
         GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback, true);
      }
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */

      //optionally start playing now.
      if (appControl.bStartPlaying)
      {
        if (!StartStreaming())
        {
          ErrorAbort(STREAM_START_ERROR);
        }
     // set this to false as streamer will come back with CONNECTED status
     // which results in sending play again
        appControl.bStartPlaying = false;

        //else wait for STREAM_START
      }
    }
    break;

#ifdef FEATURE_QTV_FCS
#error code not present
#endif /* FEATURE_QTV_FCS */

    case Common::STREAM_RECONNECT_PREPARE_FAIL:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "streamer status STREAM_RECONNECT_PREPARE_FAIL");
      // abort session
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
         "PrepareForReconnect failed, streamer status STREAM_ERROR_ABORT");
      Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
      ErrorAbort(STREAM_CONNECT_ERROR);
      ResetPlaybackData();
    }
    break;

    case Common::STREAM_RECONNECT_IN_PROGRESS:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                   "streamer status STREAM_RECONNECT_IN_PROGRESS");
      bool bError = true;

      //notify player with reconnect in progress
      Notify(QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_IN_PROGRESS);

      streamer->Reconnect(bError);
      if (bError)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Stream error from StartReconnecting");
        // abort session
        Notify(QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_FAIL);
        ErrorAbort(STREAM_CONNECT_ERROR);
        ResetPlaybackData();
      }
    }
    break;
#ifdef FEATURE_AUTOFALLBACK
#error code not present
#endif  
    case Common::STREAM_NETWORK_ERROR:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_NETWORK_ERROR");

      uint32 numberOfReconnectAttempt =
         qtvConfigObject.GetQTVConfigItem(QtvConfig::QTVCONFIG_WM_FAST_RECONNECT_MAX_ATTEMPTS);
      if ( numReconnectAttemptsMade >= numberOfReconnectAttempt)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
             "Max Retry Reached : streamer status STREAM_ERROR_ABORT");
        Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
        ErrorAbort(STREAM_CONNECT_ERROR);
        ResetPlaybackData();
        break;
      }

      bool isPlayerInReconnectAcceptableState = (playerState == PLAYING) ||
                                                (playerState == BUFFERING) ||
                                                (playerState == PAUSED) ||
                                                (playerState == SUSPENDED);

      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Network error in player state=%d", playerState);

      if (!isPlayerInReconnectAcceptableState)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                    "Reconnection can not made due to player state");
        Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
        ErrorAbort(STREAM_CONNECT_ERROR);
        ResetPlaybackData();
        break;
      }

      if (streamer)
      {
        if(!streamer->PrepareForReconnect())
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PrepareForReconnect failed, Aborting");
          // abort session
          Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
          ErrorAbort(STREAM_CONNECT_ERROR);
          ResetPlaybackData();
        }
        numReconnectAttemptsMade++;
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                    "numReconnectAttemptsMade :: %d", numReconnectAttemptsMade);
        
      }
    }
    break;

    case Common::STREAM_RECONNECT_SUCCESS:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_RECONNECT_SUCCESS");


      if ((playerState == PAUSED) || (playerState == SUSPENDED) || (playerState == PB_READY))
      {
        Notify(QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_SUCCESS);
        break;
      }

#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
      if (m_useMAPILogic && clip.pVideoMpeg4Playback != NULL)
      {
        GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback, true);
      }
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */

      if (!StartStreaming())
       {
         ErrorAbort(STREAM_START_ERROR);
       }
       else
       {
         Notify(QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_SUCCESS);
       }
    }
    break;

    case Common::STREAM_RECONNECT_FAIL:
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "streamer status STREAM_RECONNECT_FAILURE, Aborting");
      Notify(QtvPlayer::QTV_PLAYER_STATUS_RECONNECT_FAIL);
      ErrorAbort(STREAM_CONNECT_ERROR);
      ResetPlaybackData();
    }
    break;

    case Common::STREAM_START:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_START");
      // State checking here is important. The scenario, in which
      // incoming call interrupts a reposition operation (PLAY
      // request is out already), requires QTV to ignore the
      // PLAY-response. In that case the player state will be PAUSED.

      if (appControl.nStreamStartsToIgnore > 0)
      {
        // Ignore the PLAY response if we have a skip request pending, or
        // if we already called StartPlaying() in Pause() in the reposition
        // state.
        appControl.nStreamStartsToIgnore--;
      }
      else
      {
        switch (playerState)
        {
          case PB_READY:
          {
#ifdef FEATURE_QTV_WINDOWS_MEDIA
            if (clip.pVideoMpeg4Playback->IsWindowsMedia() && streamer)
            {
              streamer->SendConnectTimeLog();
            }
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
            /* Fall through to the next case to issue StartPlaying */
          }
          case REPOSITIONING:
          {
            /* In the normal case streamer is not paused when STREAM_START
            ** is received. However there is a corner case in which while
            ** waiting for STREAM_START, it is possible that a Pause/Suspend
            ** is issued to the player. In this scenario, the player pauses
            ** the streamer, but continues to remain in the PB_READY state.
            ** Since the player has been paused the received STREAM_START
            ** is ignored and StartPlaying is done on resuming from the
            ** Pause/Suspend.
            */
            if (streamer && !streamer->IsPaused())
            {
              if (!StartPlaying())
              {
                ErrorAbort(START_PLAYING_ERROR);
              }
            }
            break;
          }
        } /* switch (playerState) */
      }
      break;

#ifdef FEATURE_AUTOFALLBACK
#error code not present
#endif        
    case Common::STREAM_DONE:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_DONE");
      //Unless EOS, we'll end when the audio/video report in.
      if (pEvent->status == PV_STREAM_EOS)
      {
        bool bErr;
        StopPlayer(bErr);
        PlaybackComplete(COMPLETE);
      }
      break;

      /* Handle STREAM_AUTH_REQUIRED status here.
      Send event to OEM indicating that the session requires authorization.
      Set a flag indicating that we sent up a signal to the the OEM layer.
      Later, if we recv authentication info from user, player retries to
      bring up the RTSP control session with authentication info.
      If the user delays the response for too long time, server may teardown
      the TCP connection. In such situations, session will fail even if the
      user has provided proper authorization info.*/
    case Common::STREAM_AUTH_REQUIRED:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_AUTH_REQUIRED");
      RTSPStatusNotify(pEvent); // pass rtsp status to callback fcn
      if (CanAppAuthenticate() == TRUE)
      {  /* Check if App has expressed it's ability to provide authorization
          * information
          */
        (void)std_strlcpy(serverChallenge, pEvent->serverMsg, QTV_MAX_CHALLENGE_BYTES + 1);
        QTV_MSG_SPRINTF_1(QTVDIAG_MP4_PLAYER,"Server challenge copied"
          "to Mpeg4 player:%s",serverChallenge);

        challenged = true;
        Notify(QtvPlayer::QTV_PLAYER_STREAM_AUTH_REQUIRED );
        QTV_MSG(QTVDIAG_MP4_PLAYER,
          "QTV_PLAYER_STREAM_AUTH_REQUIRED sent to Qtv Player");

        return;
      }

      /* If App has no ability to provide authorization, fall through */
      /* Mimic STREAM ERROR ABORT                   */

    case Common::STREAM_ERROR_ABORT:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_ERROR_ABORT");
      RTSPStatusNotify(pEvent); // pass rtsp status to callback fcn
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "streamer status STREAM_ERROR_ABORT");
        Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
        ErrorAbort(STREAM_CONNECT_ERROR);
        ResetPlaybackData();
      }
      break;

    case Common::STREAM_EXCEPTION:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_EXCEPTION");
      RTSPStatusNotify(pEvent); // pass rtsp status to callback fcn
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "streamer status STREAM_EXCEPTION");
        /* JG Notify the upper layer regardless of state */
        Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_ERROR);
        ErrorAbort(STREAM_CONNECT_ERROR);
        ResetPlaybackData();
      }
      break;
    case Common::STREAM_SERVER_NOT_ENOUGH_BW:
      RTSPStatusNotify(pEvent); // pass rtsp status to callback fcn
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "streamer status STREAM_SERVER_NOT_ENOUGH_BW");
        /* JG Notify the upper layer regardless of state */
        Notify(QtvPlayer::QTV_PLAYER_STATUS_SERVER_NOT_ENOUGH_BW);
        ErrorAbort(STREAM_CONNECT_ERROR);
        ResetPlaybackData();  // reset the staticstics, pvdt#7961
      }
      break;

    case Common::STREAM_DATA_INACTIVITY_TIMEOUT:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_DATA_INACTIVITY_TIMEOUT");
      RTSPStatusNotify(pEvent); // pass rtsp status to callback fcn
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "streamer status STREAM_DATA_INACTIVITY_TIMEOUT");
        /* JG Notify the upper layer regardless of state */
        Notify(QtvPlayer::QTV_PLAYER_STATUS_DATA_INACTIVITY_ERROR);
        ErrorAbort(STREAM_CONNECT_ERROR);
        ResetPlaybackData();
      }
     break;

    case Common::STREAM_SERVER_CLOSED_EXCEPTION:
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
          "streamer status STREAM_SERVER_CLOSED_EXCEPTION");
        /* JG Notify the upper layer regardless of state */
        Notify(QtvPlayer::QTV_PLAYER_STATUS_STREAM_SERVER_CLOSED);

        // To do: check if this is necessary. Let OEM decide whether to continue playing or not.
        ErrorAbort(STREAM_CONNECT_ERROR);
      }
      break;

    case Common::STREAM_REDIRECT:
      QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
        "streamer status STREAM_REDIRECT: %s", pEvent->serverMsg);

      if (clip.bStreaming && pEvent->serverMsg != NULL)
      {
        if (pEvent->status == PV_STREAM_REDIRECT)
        {
          redirectFlag = true;
          RedirectToNewURN(pEvent->serverMsg);
        }
        else if (pEvent->status == PV_STREAM_DELAYED_REDIRECT && !redirectFlag )
        {
          /* NOTE: If second redirect request arrives before
          first request is complete, second request will be ignored */
          redirectFlag = true;
          redirectTime = pEvent->serverCode;
          if (NULL != redirectURL)
          {
            *redirectURL = pEvent->serverMsg;
          }
          if (clip.bHasAudio)
          {
#ifndef FEATURE_WINCE
            AudioPlayerIf::EnableTimeCheck( activeAudioPlayerId, redirectTime );
#endif   /*    FEATURE_WINCE   */
          }
          else if (clip.bHasVideo)
          {
#ifndef FEATURE_WINCE
            if (pActiveVideoPlayer)
            {
              pActiveVideoPlayer->EnableTimeCheck(redirectTime );
            }
#endif   /*    FEATURE_WINCE   */
          }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
          else if (clip.bHasText)
          {
#ifndef FEATURE_WINCE
            textPlayer.EnableTimeCheck( redirectTime );
#endif   /*    FEATURE_WINCE   */
          }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
        }
        else
        {
          QTV_MSG2(QTVDIAG_RTSP, "Discard REDIRECT request (status=%d, redirect_flag=%d)",
            pEvent->status, redirectFlag);
        }
      }
      else
      {
        QTV_MSG2(QTVDIAG_RTSP, "Discard REDIRECT request (bStreaming=%d, url_addr=0x%x)",
          clip.bStreaming, pEvent->serverMsg);
      }

      // release url buffer
      if (pEvent->serverMsg != NULL)
      {
        QTV_Free(pEvent->serverMsg);
      }
      break;

    case Common::STREAM_RATE_ADAPTATION_SUPPORTED:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "streamer status STREAM_RATE_ADAPTATION_SUPPORTED");
      RTSPStatusNotify(pEvent); // pass rtsp status to callback fcn
      Notify(QtvPlayer::QTV_STREAM_RATE_ADAPTATION_SUPPORTED);
      QTV_MSG(QTVDIAG_MP4_PLAYER, "QTV_STREAM_RATE_ADAPTATION_SUPPORTED sent to Qtv Player");
      break;

     case Common::STREAM_TRACKLIST_UNKNOWN_CODEC:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "streamer status STREAM_TRACKLIST_UNKNOWN_CODEC");
      RTSPStatusNotify(pEvent); // pass rtsp status to callback fcn
      Notify(QtvPlayer::QTV_STREAM_TRACKLIST_UNKNOWN_CODEC);
      QTV_MSG(QTVDIAG_MP4_PLAYER, "QTV_STREAM_TRACKLIST_UNKNOWN_CODEC sent to Qtv Player");
      break;

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION */

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
    case Common::STREAM_SERVER_SIDE_PLAYLIST_SKIP_SUCCESS:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
          "streamer status STREAM_SERVER_SIDE_PLAYLIST_SKIP_SUCCESS state = %d", playerState);

        bool bPauseMediaAfterStartPlaying =
          appControl.bPauseMediaAfterStartPlaying;

        appControl.bPauseMediaAfterStartPlaying = false;

        // Notify that the skip is successful before we start the player
        // again, so that we don't post buffering events before the skip
        // successful event.
        appControl.cbData1 = pEvent->clipIndex;
        appControl.cbData2 = pEvent->clipTransitionErrorReasons;
        {
          int64 transitionTime = pEvent->transitionTime;

          streamer->AdjustTransitionTimeForServerSidePlaylist(transitionTime);

          appControl.cbData3 = (uint32) transitionTime;

          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "transition time = %d",
            (uint32) transitionTime);
        }
        appControl.cbString1 = pEvent->playlistName;
        Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYLIST_PLAY_COMPLETE);

        if ((playerState != PB_READY) && (playerState != REPOSITIONING))
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
            "streamer status STREAM_SERVER_SIDE_PLAYLIST_SKIP_SUCCESS unexpected state, returning");
          return;
        }

        if (pEvent->shouldStop)
        {
          bool bError = false;
          StopPlayer(bError, false);

          if (bError)
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
              "Cannot stop player when transitioning clip!");
            return;
          }

          // Since we stopped all the media players, they will report ABORT_ERRORs
          // later. Ignore them so we don't mess up the state such as clip.bHasVideo.
          if (clip.bHasVideo)
          {
            appControl.nVideoAbortsToIgnore++;
          }

          if (clip.bHasAudio)
          {
            appControl.nAudioAbortsToIgnore++;
          }

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
          if (clip.bHasText)
          {
            appControl.nTextAbortsToIgnore++;
          }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

          if (SwitchToNextClipInServerSidePlaylist(true))
          {
            SetState(PB_READY);
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
            if (m_useMAPILogic && clip.pVideoMpeg4Playback != NULL)
            {
              GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback, true);
            }
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
              "streamer status STREAM_SERVER_SIDE_PLAYLIST_SKIP_SUCCESS can't switch to next clip");
            ErrorAbort(START_PLAYING_ERROR);
          }
        }

        // For PV, we need to use the NPT sent by the server to set the
        // play times since the media players use non-translated RTP time.
        //
        // If the notification is for a non-now transition, the
        // notification was triggered by the PLAYLIST_PLAY response, so the
        // the transition time is not present. Also we are not going to be playing
        // anyway, so no need to set the times.
        if (pEvent->transitionTime >= 0)
        {
          SetPlayTimes((long) pEvent->transitionTime, -1, appControl.bHasMediaStarted);
        }

        if ((playerState == REPOSITIONING) || (playerState == PB_READY))
        {
          reposControl.bRestart = (playerState == REPOSITIONING);

          if (clip.bHasVideo || clip.bHasAudio)
          {
            if (StartPlaying())
            {
              if (bPauseMediaAfterStartPlaying)
              {
                if (!PauseMedia())
                {
                  return;
                }
                Notify(PAUSED);
              }
            }
            else
            {
              ErrorAbort(START_PLAYING_ERROR);
            }
          }
          else
          {
            // Nothing to play, wait for upper layer to decide what to do.
            Notify(PB_READY);
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
            if (m_useMAPILogic && clip.pVideoMpeg4Playback != NULL)
            {
              GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback, true);
            }
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */
          }
        }
      }
      break;

    case Common::STREAM_SERVER_SIDE_PLAYLIST_SKIP_FAILED:
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
          "streamer status STREAM_SERVER_SIDE_PLAYLIST_SKIP_FAILED state = %d", playerState);

        bool bPauseMediaAfterStartPlaying =
          appControl.bPauseMediaAfterStartPlaying;

        appControl.bPauseMediaAfterStartPlaying = false;

        Notify(QtvPlayer::QTV_PLAYER_STATUS_PLAYLIST_PLAY_FAILED);

        if (playerState != REPOSITIONING)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
            "streamer status STREAM_SERVER_SIDE_PLAYLIST_SKIP_FAILED state not REPOSITIONING, returning");
          return;
        }

        if (appControl.bHasMediaStarted)
        {
          // We already paused, now just let the app know.
          if (bPauseMediaAfterStartPlaying)
          {
            Notify(PAUSED);
          }
          else
          {
            // So ResumePlay() is called in the right state.
            SetState(PAUSED);

            if (!ResumePlay())
            {
              ErrorAbort(RESUME_PLAY_ERROR);
            }
          }
        }
        else
        {
          if (clip.bHasVideo || clip.bHasAudio)
          {
            if (StartPlaying())
            {
              if (bPauseMediaAfterStartPlaying)
              {
                if (!PauseMedia())
                {
                  return;
                }
                Notify(PAUSED);
              }
            }
            else
            {
              ErrorAbort(START_PLAYING_ERROR);
            }
          }
          else
          {
            // Nothing to play, wait for upper layer to decide what to do.
            Notify(PB_READY);
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
            if (m_useMAPILogic && clip.pVideoMpeg4Playback != NULL)
            {
              GENERIC_BCAST_MEDIA_SET_STATE(clip.pVideoMpeg4Playback, true);
            }
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */
          }
        }
      }
      break;

    case Common::STREAM_SERVER_SIDE_PLAYLIST_TRANSITION:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
          "streamer status STREAM_SERVER_SIDE_PLAYLIST_TRANSITION state = %d", playerState);
      appControl.cbData1 = pEvent->clipIndex;
      appControl.cbData2 = pEvent->clipTransitionErrorReasons;
      // FIXME: set value of appControl.cbData3
      appControl.cbString1 = pEvent->playlistName;

      // Update Clip info
      SetClipInfo(clip.pVideoMpeg4Playback, 0, NULL, 0, NULL, 0);

      Notify(QtvPlayer::QTV_PLAYER_STATUS_CLIP_TRANSITION);
      break;
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */

#ifdef FEATURE_QTV_WM_SWITCHING_THINNING
    case Common::STREAM_TRACK_SELECTION_CHANGED:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
          "streamer status STREAM_TRACK_SELECTION_CHANGED");
      // For now we don't need to notify audio or video but this is likely to change.
      if (pEvent->trackList != NULL)
      {
        pEvent->trackList->Release();
      }
      break;
#endif /* FEATURE_QTV_WM_SWITCHING_THINNING */

#ifdef FEATURE_QTV_FCS
#error code not present
#endif /* FEATURE_QTV_FCS */

    default:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: Unknown stream status %d",(int)pEvent->status);
      break;
    }
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Invalid StreamerStatus notification, ignored");
  }
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
int Mpeg4Player::GetDownloadProgress()
//get download progress (percent complete) or zero if unknown.
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: GetDownloadProgress");
  int filesize=GetDownloadFilesize();
  if (filesize>0)
  {
    int remaining=downloadState.bytesRemaining;

    //if download is active get value from PVDL.
    if (bDownloading && pPVDL)
    {
      int n;
      if (pPVDL->GetBytesRemaining(&n)==PVDL_SUCCESS)
      {
        remaining=n;
      }
    }
    if (remaining<=0)
    {
      return 100;
    }
    else
    {
      float f = 100.0*(float)remaining/(float)filesize;
      return 100-(int)f;
    }
  }
  return 0;
}

unsigned long Mpeg4Player::GetPVDLFilesize()
{
//#ifdef FEATURE_PV_DOWNLOAD
#if 1
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: GetPVDLFilesize");

  if (pPVDL)
  {
    unsigned long nSize;
    if (pPVDL->GetMP4FileSize(&nSize)==PVDL_SUCCESS)
    {
      //I think download lib may sometimes return the value we input
      //in the session start data, just ignore this.
      if (nSize>0 && nSize!=appControl.nStorageSize)
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PVDL filesize %d",nSize);
        return nSize;
      }
    }
  }
#endif
  return 0;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetPVDLFilesize

DESCRIPTION
  This method is invoked to set the downloader file size.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void Mpeg4Player::SetPVDLFilesize(unsigned long fsize)
{

 QCUtils::EnterCritSect(&clipInfo_CS);
 clipInfo.info.Size= (int) fsize;
 QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
               "PDS2: Download file size --> %d",
                fsize);
 QCUtils::LeaveCritSect(&clipInfo_CS);
}

unsigned long Mpeg4Player::GetDownloadFilesize()
{
  //when download is active get value from PVDL, otherwise
  //get last known value.
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: GetDownloadFilesize");
  return bDownloading ? GetPVDLFilesize() : downloadState.fileSize;
}

void Mpeg4Player::UpdateDownloadState()
//update the download state from PVDL.
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: UpdateDownloadState");
  if (bDownloading)
  {
    //file size is only updated once, shortly after starting
    //the download.
    if (downloadState.fileSize==0)
    {
      downloadState.fileSize = GetPVDLFilesize();
    }
//#ifdef FEATURE_PV_DOWNLOAD
#if 1
    //bytes remaining is updated as data arrives.
    if (pPVDL)
    {
      int nRemaining;
      if (pPVDL->GetBytesRemaining(&nRemaining)==PVDL_SUCCESS)
      {
        downloadState.bytesRemaining= nRemaining;
      }
    }
    //playback time is set to zero when there's enough data
    //to parse the file, then updated as data arrives.
    if (pPVDL)
    {
      int nPos;
      if (pPVDL->GetPlaybackTime(&nPos)==PVDL_SUCCESS)
      {
        downloadState.playbackTime= nPos;
      }
    }
#endif
  }
}

void Mpeg4Player::CleanupDownload()
//
// player cleanup
//
{
//#ifdef FEATURE_PV_DOWNLOAD
#if 1
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: CleanupDownload");
  if (clip.pMpeg4Download)
  {
    QTV_Delete( clip.pMpeg4Download );
    clip.pMpeg4Download = NULL;
  }
#endif
}

/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::HandlePurgeAfterDownload()
//handle the PVX purge after download flag, right after download went idle.
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: HandlePurgeAfterDownload");
  if (pvxIF.bValid)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: PVX IF is Valid");
    if (pvxIF.appFile.PurgeDownloadFile(pvxIF.nPlaylistIndex))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: Call PurgeDownload");
      PurgeDownload();
    }
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void Mpeg4Player::PurgeDownload()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
             "PDS2: PurgeDownload");

  if (!clip.bDownload || playerState!=IDLE)
  {
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
             "Weird state in PurgeDownload");
    return;
  }



  //Delete download file if it exists.
  if (clip.pVideoLocalFilename)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
             "PDS2: Delete videoFileName");
    (void)OSCL_FileDelete(clip.pVideoLocalFilename);
  }
  else if (clip.pAudioLocalFilename)
    {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
             "PDS2: Delete audioFileName");
    (void)OSCL_FileDelete(clip.pAudioLocalFilename);
    }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"No File to Purge !!!");
  }

  //clear the download config file
  PurgeDownloadConfig();

  (void)DeleteDownloadDatFile();

  //clear the download info.
//  QCUtils::EnterCritSect(&downloadInfo_CS);
  downloadInfo.bValid=false;
  //QCUtils::LeaveCritSect(&downloadInfo_CS);
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

void Mpeg4Player::DownloadComplete(EndCode code)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
            "PDS2: DownloadComplete");

  //Set final download data
  UpdateDownloadState();
  //for some reason PVDL does not always set 0 bytes remainig
  //at EOF, resulting in 99% progress reports when it should
  //be 100, so just force it here.
  if (code==COMPLETE)
  {
    downloadState.bytesRemaining=0;
  }

  bDownloading=false;


  char DownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
  getDownloadTxtFileName(DownloadTxtFile, sizeof(DownloadTxtFile));

  //Update download info.
  if (code==COMPLETE)
  {
//    QCUtils::EnterCritSect(&downloadInfo_CS);
    downloadInfo.info.DownloadComplete=true;
//    QCUtils::LeaveCritSect(&downloadInfo_CS);
    if (OSCL_FileExists(DownloadTxtFile))
      (void)OSCL_FileDelete(DownloadTxtFile);
  }
  else
  {
//    QCUtils::EnterCritSect(&downloadInfo_CS);
    downloadInfo.info.InterruptedTime=downloadState.playbackTime;
//    QCUtils::LeaveCritSect(&downloadInfo_CS);

  }

  //Change playback mode from pseudo-stream to pure local file
  if (clip.pVideoMpeg4Playback)
  {
//#ifdef FEATURE_PV_DOWNLOAD
#if 1
    //tell filemedia interface to stop doing buffering.
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Call Video Download Done");
    ((FileMedia *)clip.pVideoMpeg4Playback)->DownloadDone();
#endif
    //tell player to ignore buffering messages.
    appControl.bPureLocalFile=true;
  }

  if (clip.pAudioMpeg4Playback)
  {
//#ifdef FEATURE_PV_DOWNLOAD
#if 1
    //tell filemedia interface to stop doing buffering.
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Call Audio Download Done");
    ((FileMedia *)clip.pAudioMpeg4Playback)->DownloadDone();
#endif
    //tell player to ignore buffering messages.
    appControl.bPureLocalFile=true;
  }

  if (clip.pTextMpeg4Playback)
  {
//#ifdef FEATURE_PV_DOWNLOAD
#if 1
    //tell filemedia interface to stop doing buffering.
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,"Call Audio Download Done");
    ((FileMedia *)clip.pTextMpeg4Playback)->DownloadDone();
#endif
    //tell player to ignore buffering messages.
    appControl.bPureLocalFile=true;
  }

  //Notify
  switch(code)
  {
    case COMPLETE:
      Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_COMPLETE);
      bDownloadDone = true;
      break;
    case ABORT:
      Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT);
      break;
    case STOPPED:
      Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_STOPPED);
      break;
    case ABORT_MEM_ERROR:
      Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ABORT_MEM_FAIL);
      break;
  }



}

void Mpeg4Player::HandlePlaybackAsap()
//handle playback asap flag right after starting download.
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: HandlePlaybackAsap");
  //software error check
  if (playerState!=DOWNLOADING)
  {
//    ErrorMsg( LOG_LVL_ERROR, "Weird state in HandlePlaybackAsap\n",0,0,0);
    //go ahead with the routine anyway.
  }

  if (appData.bValid)
  {
    if ( (appData.data.DownloadPlaybackControl==QtvPlayer::PLAYBACK_ASAP) ||
         (appData.data.DownloadPlaybackControl==QtvPlayer::NO_PLAYBACK) )
    {
      //Playback ASAP flag in PVX is set.
      //Use the play clip times from the PVX file.
      SetPlayTimes(appData.startTime,appData.stopTime,false);

      //pass the "pvx start" flag to trigger the data rate testing
      //in the filemedia code.
      if (!StartPseudoStream(true))
      {
        ErrorAbort(START_PSEUDO_STREAM_ERROR);
      }
    }
  }
}


void Mpeg4Player::HanldePlaybackAfterDownload()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: HandlePlaybackAsap");
  //look for "playback after download" flag.
  if (appData.bValid)
  {
    if (appData.data.DownloadPlaybackControl==QtvPlayer::PLAYBACK_AFTER_DOWNLOAD)
    {
      /* Check if it is ok to send PlayClip */
      if (!StartPseudoStream())
      {
        ErrorAbort(START_PSEUDO_STREAM_ERROR);
      }
    }
  }
}

void Mpeg4Player::DownloadStatus(const DOWNLOAD_STATUS_type *pEvent)
  //
  // process a download status notification.
{
//#ifdef FEATURE_PV_DOWNLOAD
#if 1
//  dprintf( LOG_LVL_LOW, "Mpeg4Player::DownloadStatus\n");
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: DownloadStatus");

  if (pEvent)
  {
    //make sure this is still relevant.
    if (pEvent->downloadID==downloadID && clip.bDownload)
    {
      //Process the event.
      switch (pEvent->status)
      {
        case Common::DOWNLOAD_CONNECTED:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: DOWNLOAD_CONNECTED");

          //download is connected.
          switch(playerState)
          {
            case CONNECTING:
              //Describe, select tracks.
              if (!DescribeClip(clip.pMpeg4Download,
                                GetPVDLFilesize(),
                                clip.pMpeg4Download,
                                GetPVDLFilesize(),
                                clip.pMpeg4Download,
                                GetPVDLFilesize()) ||
                  !(clip.bHasVideo || clip.bHasAudio
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
                    || clip.bHasText
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
                    ))
              {
                ErrorAbort(DOWNLOAD_TRACK_SELECT_ERROR);
              }
              else
              {
                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: DL_READY");
                Notify(DL_READY);

                //Optionally start download now.

                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: startdownloader");

                if (!StartDownloader())
                {
                  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
                               "Downloader not Started,Waiting for file size");
                }
                // We are setting the bDownloading flag value and changing the
                // state to DOWNLOADING in DownloadFileSizeStatus func
              }
              break;
          }
          break;

        case Common::DOWNLOAD_DATA_AVAIL:
                    if (appControl.bStartPlaying)
                    {
                      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: StartPseudoStream");
                      if (!StartPseudoStream())
                      {
                        ErrorAbort(START_PSEUDO_STREAM_ERROR);
                      }
                    }
                    //if no PlayClip then check for playback Asap flag
                    //in PVX.
                    else
                    {
                      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: HandlePlaybackAsap");
                      HandlePlaybackAsap();
                    }
              break;
        case Common::DOWNLOAD_COMPLETE:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"PDS2: DOWNLOAD_COMPLETE");

          if (bDownloading)
          {
            DownloadComplete(COMPLETE);
            HanldePlaybackAfterDownload();

          }
          break;

        case Common::DOWNLOAD_ERROR_ABORT:
        case Common::DOWNLOAD_EXCEPTION:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"PDS2: DOWNLOAD_EXCEPTION");
          if (bDownloading)
          {
            DownloadComplete(ABORT);
            PlaybackErrorAbort();
          }
          break;
        case Common::DOWNLOAD_MEMALLOC_FAIL:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"PDS2: DOWNLOAD_MEMALLOC_ERROR");
          if (bDownloading)
          {
            DownloadComplete(ABORT_MEM_ERROR);
            PlaybackErrorAbort();
          }
          break;

        default:
          break;
      }
    }
    else
    {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "PDS2: Received DOWNLOAD_STATUS, ignored");
    }
  }
#endif
}

#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2*/
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
/* ======================================================================
FUNCTION
  Mpeg4Player::HttpDownloadComplete

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
void Mpeg4Player::HttpDownloadComplete(EndCode code)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
            "QtvDL DownloadComplete");

  if (code==COMPLETE)
  {
    downloadState.bytesRemaining=0;
  }

  //Notify
  switch(code)
  {
    case COMPLETE:
      Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_COMPLETE);
      bDownloadDone = true;
      //Delete the corresponding txt file once download is complete
      char DownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
      memset( DownloadTxtFile, 0, sizeof(DownloadTxtFile) ); 
      getDownloadTxtFileName(DownloadTxtFile, sizeof(DownloadTxtFile));
      if (OSCL_FileExists (DownloadTxtFile))
      {
        OSCL_FileDelete(DownloadTxtFile);
      }
      break;
    case ABORT:
      Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ERROR_ABORT);
      break;
    case STOPPED:
      break;
    case ABORT_MEM_ERROR:
      Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOAD_ABORT_MEM_FAIL);
      break;
  }

}
/* ======================================================================
FUNCTION
  Mpeg4Player::HttpDownloadStatus

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
void Mpeg4Player::HttpDownloadStatus(const HTTP_DOWNLOAD_STATUS_type *pEvent)
{

  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"QtvDL DownloadStatus");

  if (pEvent)
  {
    //make sure this is still relevant.
    //if (pEvent->downloadID==downloadID)
    //{
      //Process the event.
      switch (pEvent->status)
      {
        bool retVal;
        case Common::DOWNLOAD_CONNECTED:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"QtvDL DOWNLOAD_CONNECTED");
          retVal = setupHttpParameters();
          if (retVal)
          {

            //download is connected.
            if (playerState == CONNECTING)
            {
              if (!StartHttpDownloader())
              {
                 HttpDownloadComplete(ABORT);
                 PlaybackErrorAbort();
              }
              else
              {
                 Notify(DOWNLOADING);
                 bDownloading = true;
              }
              break;
            }
            else
            {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QtvDL Improper Player State");
               HttpDownloadComplete(ABORT);
               PlaybackErrorAbort();
            }
          }
          else
          {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Http Setup failed");
             PlaybackErrorAbort();
          }
          break;

        case Common::DOWNLOAD_DATA_AVAIL:
          if (playerState != IDLE)
          {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"QtvDl DOWNLOAD_DATA_AVAIL");
          if (!fileOpened)
          {
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
             QTV_HTTP_STREAM_OPEN_EVENT_type *pEvent =
                QCCreateMessage(QTV_HTTP_STREAM_OPEN_EVENT, this);

             if (pEvent)
             {
               pEvent->FetchBufferedDataSize = DefaultFetchBufferedDataSize;
               pEvent->FetchBufferedData = DefaultFetchBufferedData;
               pEvent->pPSBuffer = NULL;
               pEvent->bufferSize = 0;
               pEvent->writePtrOffset = 0;
               pEvent->dwTrackSelected = 0x07;
               PostMessage(pEvent);
             }
#endif
             fileOpened = true;
          }
          Notify(QtvPlayer::QTV_PLAYER_STATUS_DOWNLOADED_DATA_AVAIL);
          }
          break;

        case Common::DOWNLOAD_COMPLETE:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,"QtvDl DOWNLOAD_COMPLETE");
          HttpDownloadComplete(COMPLETE);
          httpdownload.Destroy();
          break;

        case Common::DOWNLOAD_ERROR_ABORT:
        case Common::DOWNLOAD_EXCEPTION:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QtvDL DOWNLOAD_EXCEPTION");
          HttpDownloadComplete(ABORT);
          PlaybackErrorAbort();
          httpdownload.Destroy();
          break;

        case Common::DOWNLOAD_MEMALLOC_FAIL:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QtvDL DOWNLOAD_MEMALLOC_ERROR");
          HttpDownloadComplete(ABORT_MEM_ERROR);
          PlaybackErrorAbort();
          httpdownload.Destroy();
          break;

        case Common::DOWNLOAD_STOP:
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"QtvDL DOWNLOAD_STOP");
          HttpDownloadComplete(STOPPED);
          httpdownload.Destroy();
          break;

        default:
          break;
      }
    /*
    }
    else
    {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "PDS2: Received DOWNLOAD_STATUS, ignored");
    }
    */
  }
}/*lint!e550 pEvent is no used */

/* ======================================================================
FUNCTION
  Mpeg4Player::HTTPRegisterCallback
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

void Mpeg4Player::HTTPRegisterCallback(QTV_HTTP_REGISTER_CALLBACK_EVENT_type *pEvent)
{
   HttpDataInterface* tmpDataInterface;
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "Mpeg4Player::HTTPRegisterCallback");
   if (pEvent &&
       pEvent->IsMimeTypeSupported &&
       pEvent->FetchBufferedDataSize &&
       pEvent->FetchBufferedData)
   {
      isMimeTypeSupported = pEvent->IsMimeTypeSupported ;
      tmpDataInterface = QTV_New_Args (HttpDataInterface, (pEvent->FetchBufferedDataSize
                                                     ,pEvent->FetchBufferedData));

      //we have created a new interface now old interface can be deleted
      if(dataInterface)
      {
        QTV_Delete(dataInterface);
      }
      dataInterface = tmpDataInterface;

   }
   else
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "One of the Callbacks is NULL");
   }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::setuphttpParameter

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
bool Mpeg4Player::setupHttpParameters()
{
   int size = httpdownload.getContentSize();
   if (size)
   {
      clipInfo.info.Size = size;
   }
   else
   {
      return false;
   }
   char *mimeType = httpdownload.getMimeType();
   if (!mimeType)
   {
      return false;
   }
   dataInterface->getQtvCallbacks(DefaultFetchBufferedDataSize,
                                  DefaultFetchBufferedData);

   qtv_download_action retVal = CONTENT_NOT_ENCRYPTED;
   if (isMimeTypeSupported)
   {
     retVal = (*isMimeTypeSupported)(mimeType);
   }

   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"IsMimeSupported:retVal = %d", retVal);
   switch (retVal)
   {
     case APP_TO_DECRYPT:
       dataInterface->setIsContentEncrypted(true);
       dataInterface->setOemMallocCallback(OEMMallocBuffer, OEMFreeBuffer);
       break;

     case CONTENT_NOT_ENCRYPTED:
       dataInterface->setIsContentEncrypted(false);
       break;

     case QTV_STOP:
       HttpDownloadComplete(ABORT);
       httpdownload.CloseConnection();
       break;

     default:
       break;
   }
   return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::freeDownloadBuffer

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
void Mpeg4Player::freeDownloadBuffer(char *buffer)
{
   dataInterface->freeDownloadBuffer(buffer);
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetEncryptedData

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
char* Mpeg4Player::getEncryptedData(uint32 &size)
{
   return dataInterface->getEncryptedData(size);
}
#endif

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

/* ======================================================================
FUNCTION
  Mpeg4Player::GetAudioCodecType

DESCRIPTION
  Return the codec enumeration for the selected audio track

DEPENDENCIES
  DescribeAndSelectTracks must have been called

RETURN VALUE
  The selected audio codec

SIDE EFFECTS
  none

========================================================================== */
Media::CodecType Mpeg4Player::GetAudioCodecType()
{
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */
  {
    if (selectedAudioTrackIdx < 0)
    {
      return Media::UNKNOWN_CODEC;
    }

    Media *pMedia = GetAudioMedia();

    if (pMedia == NULL)
    {
      return Media::UNKNOWN_CODEC;
    }

    ReferenceCountedPointer<ITrackList> trackList;

    if (!pMedia->GetTrackList(MakeOutPointer(trackList)) ||
        (trackList == NULL))
    {
      return Media::UNKNOWN_CODEC;
    }

    Media::CodecType codec = trackList->GetCodecType(
      selectedAudioTrackIdx);

    return codec;
    }
    }

/* ======================================================================
FUNCTION
  Mpeg4Player::GetVideoCodecType

DESCRIPTION
  Return the codec enumeration for the selected video track

DEPENDENCIES
  DescribeAndSelectTracks must have been called

RETURN VALUE
  The selected video codec

SIDE EFFECTS
  none

========================================================================== */
Media::CodecType Mpeg4Player::GetVideoCodecType()
{
  if (selectedVideoTrackIdx < 0)
  {
    return Media::UNKNOWN_CODEC;
  }

  Media *pMedia = GetVideoMedia();

  if (pMedia == NULL)
  {
    return Media::UNKNOWN_CODEC;
  }

  ReferenceCountedPointer<ITrackList> trackList;

  if (!pMedia->GetTrackList(MakeOutPointer(trackList)) ||
      (trackList == NULL))
  {
    return Media::UNKNOWN_CODEC;
  }

  Media::CodecType codec = trackList->GetCodecType(
    selectedVideoTrackIdx);

  return codec;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::IsLocalMediaPLayback

DESCRIPTION
  checks if the current media playback is either from local file
  or local buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  returns TRUE if playback is from local content, else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::IsLocalMediaPLayback()
{
  if( clip.pVideoLocalFilename || clip.pAudioLocalFilename || clip.pVideoLocalBuffer || clip.pAudioLocalBuffer)
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::SetLoopTrackFlag

DESCRIPTION
  sets/resets the loop track flag for local media playback.
  If set:
    repositioning is disabled.
    as soon as any track ends, it will restart.
    qtv will stop only after getting stop command from user.

DEPENDENCIES
  This flag can be set only before parsing the file.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::SetLoopTrackFlag(bool bLoop)
{
    bool bRet = false;
    bLoopTrackFlag = bLoop;
    if( (playerState==IDLE) || (playerState==PB_READY) || (playerState==OPENING) )
    {
    if(clip.pVideoMpeg4Playback)
      clip.pVideoMpeg4Playback->SetLoopTrackFlag(bLoopTrackFlag);
    if(clip.pAudioMpeg4Playback)
      clip.pAudioMpeg4Playback->SetLoopTrackFlag(bLoopTrackFlag);
    if(clip.pTextMpeg4Playback)
      clip.pTextMpeg4Playback->SetLoopTrackFlag(bLoopTrackFlag);
    bRet = true;
  }

  return bRet;
}
/* ======================================================================
FUNCTION
  Mpeg4Player::GetLoopTrackFlag

DESCRIPTION
  Used to get bLoopTrackFlag value

DEPENDENCIES
  None

RETURN VALUE
   true: Indicates that the bLoopTrackFlag is set
   false: Indicates that the bLoopTrackFlag is not set

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::GetLoopTrackFlag(void)
{
 return bLoopTrackFlag;
}

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

/* ======================================================================
FUNCTION
  Mpeg4Player::SetMediaInfo

DESCRIPTION
  sets/resets the media specific info.
  Operation can be performed only in IDLE state.  We usually want to know
  about the media *before* we start playing.

DEPENDENCIES
  Media info can be set only in IDLE state.

RETURN VALUE
  TRUE if successful, else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::SetMediaInfo
(
  void  *pInfo, // Pointer to Media Information
  uint32 size   // size
)
{
  Media *pMedia = NULL;

  // Priority is given to Video Media
  pMedia = GetVideoMedia();
  if(!pMedia)
  {
    pMedia = GetAudioMedia();
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(!pMedia)
  {
    pMedia = GetTextMedia();
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  if(!pMedia && (playerState != IDLE))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
    "Neither Player state nor media valid ");
    return false;
  }


  if (pMedia && !pMedia->SetMediaInfo(pInfo,size))
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
    "Set Media Info operation failed ; pMedia(%p)",pMedia);
    return false;
  }


  /* Free any previous info */
  if(mediaInfo)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_LOW,
    "Freeing old media Information (%p)",mediaInfo);
    QTV_Free(mediaInfo);
    mediaInfo = NULL;
  }

  /* Alloc and copy the new media info */
  if(pInfo && size)
  {

    mediaInfo = QTV_Malloc(size);
    if(!mediaInfo)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
      "SetMediaInfo: Malloc failed ");
      return false;
    }
    memcpy(mediaInfo, pInfo, size);
    mediaInfoSize = size;
    QTV_MSG_PRIO2(QTVDIAG_GENERAL,QTVDIAG_PRIO_LOW,
    "Allocated new media Information (%p,%d)",mediaInfo,mediaInfoSize);
  }

  return true;

}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetMediaInfo

DESCRIPTION
  Gets the media specific info.
  Operation can be done only after the media object is properly instantiated.
  Client passes double pointer which is equivalent to saying that memory
  allocation is done only to store the pointer. It is the responsibility of
  allocating the output media information using OEM configure memory management
  routines.

DEPENDENCIES
  Media info can be read only after the instantiation of media object.

RETURN VALUE
  TRUE if successful, else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::GetMediaInfo(void **ppInfo, unsigned int *pnSize)
{
  bool bRet = false;
  Media *pMedia = NULL;

  pMedia = GetVideoMedia();
  if(!pMedia)
  {
    pMedia = GetAudioMedia();
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  if(!pMedia)
  {
    pMedia = GetTextMedia();
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  if(pMedia)
  {

    bRet = pMedia->GetMediaInfo(ppInfo,pnSize);
    if(!bRet)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
      "Mpeg4Player.GetMediaInfo failed due to invalid return from media");
      return bRet;
    }

  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
    "Mpeg4Player.GetMediaInfo failed due to NULL Media");
  }
  return bRet;
}


#ifdef FEATURE_QTV_WM_DRM_API
/* ======================================================================
FUNCTION
  Mpeg4Player::RegisterDRMDecryptMethod

DESCRIPTION
  Registers DRM decryption method.

DEPENDENCIES
  key can be set only in IDLE state.

RETURN VALUE
  TRUE if successful, else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::RegisterDRMDecryptMethod( const QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_type *pEvent )
{
   bool bRet = false;

   if(pEvent)
   {
     if(clip.pAudioMpeg4Playback)
     {
       bRet = clip.pAudioMpeg4Playback->RegisterDRMDecryptMethod(pEvent->pDecryptFunction, pEvent->pClientData);
#ifdef FEATURE_QTV_WM_DRM_API
       storedpDRMDecryptFunction = pEvent->pDecryptFunction;
       storedpClientData = pEvent->pClientData;
#endif
     }
     if(clip.pVideoMpeg4Playback)
     {
       bRet = clip.pVideoMpeg4Playback->RegisterDRMDecryptMethod(pEvent->pDecryptFunction, pEvent->pClientData);
#ifdef FEATURE_QTV_WM_DRM_API
       storedpDRMDecryptFunction = pEvent->pDecryptFunction;
       storedpClientData = pEvent->pClientData;
#endif
     }
   }

  return bRet;
}
#endif /* FEATURE_QTV_WM_DRM_API */

#ifdef FEATURE_QTV_NET_POLICY
/* ======================================================================
FUNCTION
  Mpeg4Player::SetNetPolicyInfo

DESCRIPTION
  Sets the network policy for a streaming session

DEPENDENCIES
  None

RETURN VALUE
  Always returns true

SIDE EFFECTS

========================================================================== */
bool Mpeg4Player::SetNetPolicyInfo(QtvPlayer::NetPolicyInfo* in_netPolicyInfo)
{
  /* if in_netPolicyInfo is NULL, the player resets the isSet flag indicating
   * that netPolicyInfo is not set. Hence, the streamer will originate the
   * call with default network policy.
   */
  if (in_netPolicyInfo)
  {
    netPolicyInfo.isSet = in_netPolicyInfo->isSet;
    netPolicyInfo.isQoSEnabled = in_netPolicyInfo->isQoSEnabled;
    netPolicyInfo.primaryPDPProfileNo = in_netPolicyInfo->primaryPDPProfileNo;
  }
  else
  {
    netPolicyInfo.isSet = false;
  }
#ifdef FEATURE_QTV_MAPI_STREAM_MEDIA
  /* Fails at this point of time ,will be set it in openStream()right
     after MediaSource Creation */
  if (clip.pVideoMpeg4Playback != NULL)
  {
    (void)((CStreamMedia_v2*)clip.pVideoMpeg4Playback)->SetNetPolicyInfo(in_netPolicyInfo);
  }
#endif /* FEATURE_QTV_MAPI_STREAM_MEDIA */

  return true;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetNetPolicyInfo

DESCRIPTION
  Fills up the network policy structure (allocated by the caller) pointed
  to by the parameter with the network policy information.

DEPENDENCIES
  None

RETURN VALUE
  true: Indicates that the out_netPolicyInfo is valid
  false: Indicates an error condition, e.g., out_netPolicyInfo is NULL.

SIDE EFFECTS

========================================================================== */
bool Mpeg4Player::GetNetPolicyInfo
(
  QtvPlayer::NetPolicyInfo* out_netPolicyInfo
)
{
  if (!out_netPolicyInfo)
  {
    return false;
  }
  else
  {
    out_netPolicyInfo->isSet = netPolicyInfo.isSet;
    out_netPolicyInfo->isQoSEnabled = netPolicyInfo.isQoSEnabled;
    out_netPolicyInfo->primaryPDPProfileNo = netPolicyInfo.primaryPDPProfileNo;
  }

  return true;
}
#endif /* FEATURE_QTV_NET_POLICY */

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
/* ======================================================================
FUNCTION
  Mpeg4Player::SetDescramblerHandler

DESCRIPTION
  gives descrambler handler instance to descrambler proxy.

DEPENDENCIES
  None

RETURN VALUE
  true: Indicates that instance is OK
  false: Indicates an error condition

SIDE EFFECTS

========================================================================== */
bool Mpeg4Player::SetDescramblerHandler(void * descrambler)
{
  /* if descrambler instance is NULL for IPsec case, we can ignore and continue whatever
  we are doing. Qtv needs to create descrambler for ISMACryp and SRTP if descrambler
  instance is NULL(QtvDescramblerProxy takes care of it).
  */

  if (descrambler)
  {
    streamer->SetSescramblerHandler(descrambler);
    return true;
  }

  return false;
}

#endif  /*FEATURE_QTV_ENCRYPTED_STREAMS*/

/* ======================================================================
FUNCTION
  Mpeg4Player::setPrerollDurationBuffered

DESCRIPTION
  Sets the duration of filled preroll buffer

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
NONE

========================================================================== */
void Mpeg4Player::setPrerollDurationBuffered(uint32 duration)
{
    nPrerollDurationFilled = duration;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetAmtBufferedInCMX

DESCRIPTION
  Get duration buffered in CMX

DEPENDENCIES
  None

RETURN VALUE
========================================================================== */
long Mpeg4Player::GetAmtBufferedInCMX()
{
#ifndef FEATURE_WINCE
  return AudioPlayerIf::GetAmtBufferedInCmx( activeAudioPlayerId );
#else
#error code not present
#endif   /*    FEATURE_WINCE   */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetElapsedTime

DESCRIPTION
  Get current elapsed time since starting of play

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
unsigned long Mpeg4Player::GetElapsedTime()
{
   unsigned long elapsedTime = 0;
#ifndef FEATURE_WINCE
   if (clip.bHasAudio)
   {
    elapsedTime = (unsigned long)AudioPlayerIf::GetElapsedTime(activeAudioPlayerId);
   }
   else if (clip.bHasVideo)
   {
     if (pActiveVideoPlayer)
     {
       elapsedTime = (unsigned long)pActiveVideoPlayer->GetElapsedTime();
     }
   }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
   else if (clip.bHasText)
   {
      elapsedTime = (uint32)textPlayer.GetElapsedTime();
   }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */
   return elapsedTime;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::RedirectToNewURN

DESCRIPTION
  Close current session and redirect request to new URN

INPUT PARAMETERS
  newURN - NULL -> use the information stored in redirectURL/redirectTime
           Otherwise, use newURN and start from current play time

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void Mpeg4Player::RedirectToNewURN( mbchar* newURN )
{
  /* Notify the application layer of REDIRECT */
  Notify(QtvPlayer::QTV_PLAYER_STATUS_REDIRECT);

  // open new url
  PV_OPEN_URN_type *URNEvent = QCCreateMessage(PV_OPEN_URN, this);/*lint !e641 */
  int32 elapsedTime;

  if (URNEvent)
  {
    URL newURL = *redirectURL;
    if (NULL != newURN)
    {
      newURL = newURN;
    }

    //memory will be released in after received this event in mpeg4player
    URNEvent->videoURN = QTV_New_Args(URL, (URL::MaxUrlSize+1));
    if ( (NULL != URNEvent->videoURN) && !newURL.IsEmpty() )
    {
      URNEvent->videoURN->SetUrl(newURL);
    }

    URNEvent->audioURN = QTV_New_Args(URL, (URL::MaxUrlSize+1));
    if ( (NULL != URNEvent->audioURN) && !newURL.IsEmpty())
    {
      URNEvent->audioURN->SetUrl(newURL);
    }

    URNEvent->textURN = QTV_New_Args(URL, (URL::MaxUrlSize+1));
    if ( (NULL != URNEvent->textURN) && !newURL.IsEmpty())
    {
      URNEvent->textURN->SetUrl(newURL);
    }

    PostMessage(URNEvent);
  }

  //If elapsed time is zero, then let the PLAY be invoked by the OEM App.
  //Yet to test the scenario when a RTSP redirect can be received in the
  //middle of a session
  if( (elapsedTime = GetElapsedTime()) == 0 )
  {
    return;
  }
  // play new url
  PV_PLAY_CLIP_type *playEvent = QCCreateMessage(PV_PLAY_CLIP, this);/*lint !e641 */
  if (playEvent)
  {
    long startTime = 0, stopTime = 0;
    (void)GetPlayTimes( startTime, stopTime );
    if (NULL != newURN)
    {
      redirectTime = elapsedTime;
    }
    playEvent->startTime = redirectTime;
    playEvent->stopTime = stopTime;
    PostMessage(playEvent);
  }
}


#ifdef FEATURE_QTV_SKT_MOD_MIDI
/* ======================================================================
FUNCTION:
  Mpeg4Player::GetMidiDataSize

DESCRIPTION:
  returns MIDI data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of MIDI data
  0 if MIDI not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 Mpeg4Player::GetMidiDataSize()
{
  Media * pMedia = NULL;
  pMedia = GetVideoMedia();
  if( pMedia && pMedia->IsMidiDataPresent() )
  {
    return pMedia->GetMidiDataSize();
  }
  pMedia = GetAudioMedia();
  if( pMedia && pMedia->IsMidiDataPresent() )
  {
    return pMedia->GetMidiDataSize();
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  pMedia = GetTextMedia();
  if( pMedia && pMedia->IsMidiDataPresent() )
  {
    return pMedia->GetMidiDataSize();
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  return 0;
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::GetMidiData

DESCRIPTION:
  copies the MIDI data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.
  offset- INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
  0 if no MIDI data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 Mpeg4Player::GetMidiData(uint8 *pBuf, uint32 size, uint32 offset)
{
  Media * pMedia = NULL;
  pMedia = GetVideoMedia();
  if( pMedia && pMedia->IsMidiDataPresent() )
  {
    return pMedia->GetMidiData(pBuf, size, offset);
  }
  pMedia = GetAudioMedia();
  if( pMedia && pMedia->IsMidiDataPresent() )
  {
    return pMedia->GetMidiData(pBuf, size, offset);
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  pMedia = GetTextMedia();
  if( pMedia && pMedia->IsMidiDataPresent() )
  {
    return pMedia->GetMidiData(pBuf, size, offset);
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  return 0;
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::GetLinkDataSize

DESCRIPTION:
  returns LINK data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of LINK data
  0 if LINK not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 Mpeg4Player::GetLinkDataSize()
{
  Media * pMedia = NULL;
  pMedia = GetVideoMedia();
  if( pMedia && pMedia->IsLinkDataPresent() )
  {
    return pMedia->GetLinkDataSize();
  }
  pMedia = GetAudioMedia();
  if( pMedia && pMedia->IsLinkDataPresent() )
  {
    return pMedia->GetLinkDataSize();
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  pMedia = GetTextMedia();
  if( pMedia && pMedia->IsLinkDataPresent() )
  {
    return pMedia->GetLinkDataSize();
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  return 0;
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::GetLinkData

DESCRIPTION:
  copies the LINK data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.

RETURN VALUE:
  actual bytes copied into buffer
  0 if no LINK data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 Mpeg4Player::GetLinkData(uint8 *pBuf, uint32 size)
{
  Media * pMedia = NULL;
  pMedia = GetVideoMedia();
  if( pMedia && pMedia->IsLinkDataPresent() )
  {
    return pMedia->GetLinkData(pBuf, size);
  }
  pMedia = GetAudioMedia();
  if( pMedia && pMedia->IsLinkDataPresent() )
  {
    return pMedia->GetLinkData(pBuf, size);
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  pMedia = GetTextMedia();
  if( pMedia && pMedia->IsLinkDataPresent() )
  {
    return pMedia->GetLinkData(pBuf, size);
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  return 0;
}

#endif /* FEATURE_QTV_SKT_MOD_MIDI */

/* ======================================================================
FUNCTION:
  Mpeg4Player::GetDataSize

DESCRIPTION:
  returns the specified data-type data size.

INPUT/OUTPUT PARAMETERS:
  dType - data-type.

RETURN VALUE:
  size of the specified data-type.
  0 if specified data not present.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 Mpeg4Player::GetDataSize(QtvPlayer::DataT dType, uint32 offset )
{
  Media * pMedia = NULL;
  pMedia = GetVideoMedia();
  if( pMedia && pMedia->IsDataPresent(dType, offset))
  {
    return pMedia->GetDataSize(dType, offset);
  }
  pMedia = GetAudioMedia();
  if( pMedia && pMedia->IsDataPresent(dType, offset))
  {
    return pMedia->GetDataSize(dType, offset);
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  pMedia = GetTextMedia();
  if( pMedia && pMedia->IsDataPresent(dType, offset))
  {
    return pMedia->GetDataSize(dType, offset);
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  return 0;
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::GetData

DESCRIPTION:
  copies the specified data-type data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  dType - data-type.
  pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
  size  - INPUT         - size of buffer and max data to be copied.
  offset- INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
  0 if no data is available

SIDE EFFECTS:
  None.
======================================================================*/
uint32 Mpeg4Player::GetData(QtvPlayer::DataT dType, uint8 *pBuf, uint32 size, uint32 offset)
{
  Media * pMedia = NULL;
  pMedia = GetVideoMedia();
  if( pMedia && pMedia->IsDataPresent(dType, offset))
  {
    return pMedia->GetData(dType, pBuf, size, offset);
  }
  pMedia = GetAudioMedia();
  if( pMedia && pMedia->IsDataPresent(dType, offset))
  {
    return pMedia->GetData(dType, pBuf, size, offset);
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  pMedia = GetTextMedia();
  if( pMedia && pMedia->IsDataPresent(dType, offset))
  {
    return pMedia->GetData(dType, pBuf, size, offset);
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  return 0;
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::IsAudioPlaying

DESCRIPTION:
  returns if audio track is still playing.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
    true:  if audio track is playing
    false: if clip does not have audio tarck or it has ended/aborted.

SIDE EFFECTS:
  None.
======================================================================*/
bool Mpeg4Player::IsAudioPlaying()
{
  if(clip.bHasAudio
#ifndef FEATURE_WINCE
      &&  !AudioPlayerIf::IsDone(activeAudioPlayerId)
#endif   /*    FEATURE_WINCE   */
      )
    return true;
  else
    return false;
}


/* ======================================================================
FUNCTION:
  Mpeg4Player::IsAudioOutputDelayed

DESCRIPTION:
  Returns true if audio is present but audio output is delayed pending
  either the completion of audio buffering or the first video frame.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  See description.

SIDE EFFECTS:
  None.
======================================================================*/
bool Mpeg4Player::IsAudioOutputDelayed()
{
  return (IsAudioPlaying() && appControl.bDelayAudioOutput);
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::IsVideoPlaying

DESCRIPTION:
  returns if video track is still playing.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
    true:  if video track is playing
    false: if clip does not have video tarck or it has ended/aborted.

SIDE EFFECTS:
  None.
======================================================================*/
bool Mpeg4Player::IsVideoPlaying()
{
  if(clip.bHasVideo
#ifndef FEATURE_WINCE
&&  pActiveVideoPlayer && !(pActiveVideoPlayer->IsDone())
#endif   /*    FEATURE_WINCE   */
)
    return true;
  else
    return false;
}


/* ======================================================================
FUNCTION:
  Mpeg4Player::IsTextPlaying

DESCRIPTION:
  returns if text track is still playing.

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
    true:  if text track is playing
    false: if clip does not have text tarck or it has ended/aborted.

SIDE EFFECTS:
  None.
======================================================================*/
bool Mpeg4Player::IsTextPlaying()
{
  if(clip.bHasText
#ifndef FEATURE_WINCE
      &&  !textPlayer.IsDone()
#endif   /*    FEATURE_WINCE   */
      )
    return true;
  else
    return false;
}

////////////////////////////////////////////////////////////////////
//
// Player thread.
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION
  Mpeg4Player::PlayerThread

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
void Mpeg4Player::PlayerThread(void)
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::PlayerThread" );

   QCUtils::SetMessageDispatchTable( playerEventTable, &playerER );

   //set the flag to indicate that our dispatch table is activated
   // and the thread has started.
   m_bThreadStartSync = true;

   QCMessageType *pEvent;
   m_bIsTaskDone = false;

   /**
    * For DCF playback, Calling task needs to register an IPC signal.
    * This signal is set by IxStream on completion
    * of synchronous/asynchronous operation.
    * IPC handler for the calling task will make sure
    * to invoke IxStreamDispatchMsg which eventually invokes
    * the callback provided by calling task.
    */
    #ifdef FEATURE_QTV_DRM_DCF
      ixipc_init(0,QTV_DRM_DCF_IPC_SIG) ;
    #endif

   while (1)
   {
      Mpeg4Player *pThis;
      // Message Loop
      pEvent = QCUtils::GetNextMessage(&playerER);
      if (pEvent)
      {
         pThis = (Mpeg4Player *)((zrex_event_header_type *)pEvent)->user_data;
         if (!pThis->bAbort)
         {
            QCUtils::DispatchMessage( pEvent, &playerER );
         }
         else
         {
            //This just expedites the processing of PV_STOP event which might be
            //sitting at the tail of the evt-q
            (void)pThis->Stop(NULL);
            //Now release the current event.
            QCUtils::ReleaseMessage(pEvent,NULL);
            pThis->threadIdle = true;

            if(1 == guMpeg4PlayerRefCnt)
            {
              break;
            }
            else
            {
              //cleanup all the pending/posted events on the evt-q
              QCUtils::ReleaseEventQ(&playerTC);
            }
         }
      }
      else
      {
         QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PlayerThread gets NULL event" );
      }
   }

   m_bIsTaskDone = true;

   #ifdef FEATURE_QTV_DRM_DCF
     //Unregister Mpeg4Player task with ixipc
     ixipc_terminate();
   #endif

   // Suspend the thread, so it can be killed by the Destroy function
   QCUtils::PauseThread(&playerTC);
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ThePlayerThread

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
void Mpeg4Player::ThePlayerThread(unsigned long param)
{
   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::ThePlayerThread param 0x%x", param );

   PlayerThread();
}


#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
/* ======================================================================
FUNCTION
  Mpeg4Player::CopyBufferToFile

DESCRIPTION
  Save any downloaded buffer to file system with the given
  file name. This method can also be used for other streaming modes
  which make use of buffer playback model.


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool Mpeg4Player::CopyBufferToFile(const QTV_COPY_BUFFER_TO_FILE_type * pEvent)
{
  QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                          "PDS2 Save As file name --> %s ",
                          pEvent->fullpathtoFilename);

  if(false == pEvent->ow) /* File overwrite not allowed*/
  {
     if (OSCL_FileExists((OSCL_STRING)pEvent->fullpathtoFilename))
     {  /* File with given name already exists*/
        /* Notify App*/
        QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                  "Save As: File %s already exists... File overwrite flag is OFF",
                   pEvent->fullpathtoFilename );
        Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FILE_ALREADY_EXISTS);
        return false;

     }
  }
  if (pvxIF.appFile.PurgeDownloadFile(pvxIF.nPlaylistIndex)
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  || http_bContentPurge
#endif
  )
  {
     /* Notify PDS2 Save operation is not allowed */
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Save As: Save operation is not allowed");
     Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_DISALLOWED);
     return false;
  }
  if (FileSavedToBuffer)
  {
     char* fileBuffer;
     uint32 bufferSize;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
     if (clip.bDownload && pPVDL)
     {
        // Specific to FastTrack
        fileBuffer = pPVDL->getPDS2BufPtr();
        bufferSize = pPVDL->getPDS2BufSize();
     }
     else
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
     if(clip.bQtvHttpStreaming && dataInterface)
     {
        fileBuffer = dataInterface->getDownloadBuffer();
        bufferSize = dataInterface->getDownloadBufferSize();
     }
     else
#endif

     {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"No download object exists");
        Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED);
        return false;
     }
     int64 fileSysFreeSpaceinBytes = OSCL_GetFileSysFreeSpace(pEvent->fullpathtoFilename);

     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
               "EFS Free space %u",
                fileSysFreeSpaceinBytes);

     if(fileSysFreeSpaceinBytes < bufferSize)
     {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
           "Copy failed: Free space %u < PDS2 buf size %u",
                     fileSysFreeSpaceinBytes,
                     bufferSize);
        Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_NOT_ENOUGH_SPACE);
        Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED);
        return false;
     }
     if(fileBuffer != NULL)
     {
        OSCL_FILE  *_Handle;
        _Handle = OSCL_FileOpen((OSCL_STRING)pEvent->fullpathtoFilename, _T("c"));
        if(NULL != _Handle)
        {
           uint64  numWritten=0;
           while((numWritten <= bufferSize) && !bStop )
           {
              //write in chunks of 16k bytes for huge files
              if( (bufferSize - numWritten) < 16000 )
              {
                 numWritten  += OSCL_FileWrite(fileBuffer, (bufferSize - numWritten),
                                 1, _Handle);
                 break;
               }
               else
               {
                 numWritten  += OSCL_FileWrite(fileBuffer, 16000, 1, _Handle);
                 fileBuffer += 16000;
               }
            }

            if (numWritten == 0)
            {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                  "Save As::WriteData OsclFileWrite -->FILE_ERROR ");
               (void)OSCL_FileClose(_Handle);
               return false;
            }
            else
            {
               QTV_MSG_SPRINTF_PRIO_1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                              " Save As %s succeeded",
                               pEvent->fullpathtoFilename);
               (void)OSCL_FileClose(_Handle);

            }
        }
        else
        {
           /* Notify file creation error*/
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Save As: Failed to create file");
           Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED);
           return false;
        }
     }
     else
     {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"No PDS2 buffer exists for COPY operation");
        Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED);
        return false;
     }
  }
  else
  {
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
     if (clip.bDownload && pPVDL)
     {
        if(OSCL_FileRename(downloadFileName,pEvent->fullpathtoFilename))
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2 :Renamed file");
        else
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2 :Renaming file failed");
          return false;
        }
     }
     else
#endif

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
      if(clip.bQtvHttpStreaming && dataInterface)
     {
        if(OSCL_FileRename(downloadFileName,pEvent->fullpathtoFilename))
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2 :Renamed file");
           (void)std_strlcpy(downloadFileName,nPvxFilePath, strlen(nPvxFilePath) + 1);
        }
        else
        {
           QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"PDS2 :Renaming file failed");
           return false;
        }
     }
     else
#endif

     {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"No download object exists");
        Notify(QtvPlayer::QTV_BUFFER_TO_FILE_COPY_FAILED);
        return false;
     }
     FileNeedstobeSaved = true;
  }
  if(!bDownloadDone)
  {

     //Create a Text file with the same name as the PVX file
     //so that the next time when one selects the file, this file
     //will indicate that previos download on this file was incomplete
     char DownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2];
     getDownloadTxtFileName(DownloadTxtFile, sizeof(DownloadTxtFile));

     OSCL_FILE  *fp;
     fp = OSCL_FileOpen(DownloadTxtFile,"c");
     if(fp)
     {
        (void)OSCL_FileWrite(pEvent->fullpathtoFilename, (strlen(pEvent->fullpathtoFilename) + 1), 1, fp);
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
        if (clip.bQtvHttpStreaming && dataInterface)
        {
           uint32 offset = dataInterface->getDownloadBufferSize();
           char buffer[16];
           std_strlprintf(buffer, sizeof(buffer), "%ld", offset);
           (void)OSCL_FileWrite(buffer, (strlen(buffer) + 1), 1, fp);
        }
#endif
        (void)OSCL_FileClose(fp);
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
        appControl.bResumeDownload = true;
#endif
     }
  }
  return true;
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
void Mpeg4Player::setDownloadFileName(char *clipUrl)
{
  int32 clipLen=0;
  if (!(strncmp( clipUrl, "http://", 7 )) ||
      !(strncmp( clipUrl, "HTTP://", 7 )))
  {
    int i =0, clipNameStart=0;
    while ((clipUrl[i]) !='\0')
    {
      if (clipUrl[i++] == '/')
      {
        clipNameStart = i;
      }
    }
    clipLen = strlen((clipUrl + clipNameStart));

    getDownloadFileName(clipUrl + clipNameStart,clipLen);

    // the clipUrl now shows the url without any special character and .3gp
    // extension

    // ClipLen is now different in case of URL lenght is more then the
    // EFS_MAX_FILE_LEN
    clipLen = strlen((clipUrl + clipNameStart));

    if (clipNameStart && clipLen)
    {
      // The total length of the file should be the sum of the length of the Path and
      // length of the temporary file name. An addition 2 bytes is for the NULL character
      // and for the '/' at the end of te path is the MediaPlayer provides a path name
      // without the '/' and 4 bytes for adding ".3gp extension

      int completeLen = strlen(nPvxFilePath) + clipLen + 2 +4;
      int usedLen = 0;
      if (completeLen <= URL::MaxUrlSize)
      {
        downloadFileName = (char*) QTV_Malloc(completeLen);
        if (downloadFileName)
        {
          (void)ZUtils::StrcpyN(downloadFileName,nPvxFilePath, completeLen);
          usedLen = strlen(nPvxFilePath);

          if (nPvxFilePath[strlen(nPvxFilePath)-1]!='/')
          {
             (void)ZUtils::StrcpyN(downloadFileName + usedLen,"/",
                                   completeLen - usedLen);
             usedLen++;
          }
          (void)ZUtils::StrcpyN(downloadFileName + usedLen,
                                (clipUrl + clipNameStart),
                                completeLen - usedLen);
          (void)std_strlcat(downloadFileName,".3gp",completeLen);
          QTV_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                         "FT:save to file Name %s",downloadFileName);
        }
      }
    }
  }
}

void Mpeg4Player::getDownloadTxtFileName(char *DownloadTxtFile, const int size)
{
  const char* pvxString = NULL;
  char *tempUrl =NULL;
  char *pUrl = NULL;
  int32 urlLen=0;
  if ((char *)(*URN.sURN).IsNull())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"*URN.sURN  is NULL, Returing..");
    return;
  }  
  else
  {  
    pvxString = strstr((const char*)(*URN.sURN),"pvx");
  }      

  if(!(URN.sURN->GetUrlLength()))
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"*URN.sURN length is NULL, Returing..");
     return;
  }

  if (!pvxString)
  {
#ifndef FEATURE_WINCE
    (void)std_strlcpy(rootpath, "/", sizeof(rootpath));
#else
#error code not present
#endif
  }

#ifndef FEATURE_WINCE
  //extract the clip name from url
  tempUrl = (char *)strrchr((const char*)(*URN.sURN), '/') + 1;
#else
#error code not present
#endif

  //Extract the length of the clip name
  if (tempUrl)
  {
    urlLen = strlen(tempUrl);
    pUrl = (char*)QTV_Malloc(urlLen + 1);
    if (pUrl != NULL)
    {
      (void)std_strlcpy(pUrl, tempUrl, urlLen + 1);
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"pUrl is NULL, Malloc failed Returing..");
      return;
    }
  }


  // the pUrl now will show the url without any special character and .3gp
  // extension
  getDownloadFileName(pUrl,urlLen);

  if ((strlen(pUrl) + strlen(rootpath) + strlen("brk_pvx")) <= size - 2)
  {
    (void)std_strlcpy(DownloadTxtFile,pUrl,urlLen+1);
    if(pvxString)
    {
      (void)std_strlcat(DownloadTxtFile,".brk_pvx",size - 2);
    }
    else
    {
      (void)std_strlcat(DownloadTxtFile,".3gp_url",size - 2);
    }

    char tempDownloadTxtFile[QTV_MAX_FILENAME_BYTES+MAX_TEMPFILENAME+2] = {'\0'};
    (void)std_strlcpy(tempDownloadTxtFile,rootpath, strlen(rootpath));
    (void)std_strlcat(tempDownloadTxtFile,DownloadTxtFile, size - 2);
    (void)std_strlcpy(DownloadTxtFile,tempDownloadTxtFile, sizeof(tempDownloadTxtFile));
  }
  QTV_Free(pUrl);
}



void Mpeg4Player::getDownloadFileName(char *tempUrl,int32 urlLen)
{
  char ch;
  int32 i,tmpLen;
  const int EFS_MAX_FILE_LEN = 64;

  // if download clip name contains any special character then replace it with 'a'
  // and if the length of clip should be 64(supprted by EFS) or clip length
  // which ever less

  tmpLen = (urlLen > EFS_MAX_FILE_LEN) ? EFS_MAX_FILE_LEN:urlLen;
  for( i=0;i<tmpLen;i++)
  {
    ch=tempUrl[i];

    //check to see if the input char is alphanumric or underscore.
    // if not the replace the character with 'a'

    if(isalnum(ch)||(ch =='_'))
    {
      tempUrl[i]=ch;
    }
    else
    {
      tempUrl[i]='a';
    }
  }
  tempUrl[i]='\0';
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

/* ======================================================================
FUNCTION:
  SelectPlaybackTracks

DESCRIPTION:
  It selects the playble tracks from a opened local file or progressive
  download / pseudo stream session. Not applicable for regular streaming.
  It will only succeed if Mpeg4Player is in READY/IDLE state (not playing).

INPUT/OUTPUT PARAMETERS:
  dwTrackFlag - bitwise flags for tracks to be selected
  #define QTV_NO_TRACK_SELECTED             0x00
  #define QTV_VIDEO_TRACK_SELECTED          0x01
  #define QTV_AUDIO_TRACK_SELECTED          0x02
  #define QTV_TEXT_TRACK_SELECTED           0x04

RETURN VALUE:
  QtvPlayer ReturnT type

SIDE EFFECTS:
  a call to OpenURN will reset these values and flags passed in OpenURN
  will be valid.
======================================================================*/
QtvPlayer::ReturnT Mpeg4Player::SelectPlaybackTracks(uint32 dwTrackFlag)
{
  QtvPlayer::ReturnT retVal = QtvPlayer::QTV_RETURN_ERROR;
  if( clip.bStreaming == false )
  {
    if( (playerState == PB_READY ) ||
        (playerState == IDLE ) )
    {
      if( !(dwTrackFlag & QTV_AUDIO_TRACK_SELECTED) )
      {
        clip.pAudioLocalFilename = NULL;
        clip.bHasAudio = false;
      }
      if( !(dwTrackFlag & QTV_VIDEO_TRACK_SELECTED) )
      {
        clip.pVideoLocalFilename = NULL;
        clip.bHasVideo = false;
      }
      if( !(dwTrackFlag & QTV_TEXT_TRACK_SELECTED) )
      {
        clip.pTextLocalFilename = NULL;
        clip.bHasText = false;
      }
      retVal = QtvPlayer::QTV_RETURN_OK;
    }
  } /* ( clip.bStreaming == false ) */
  return retVal;
}

////////////////////////////////////////////////////////////////////
//
// Constructor / Destructor
//  Note: currently most zrex calls will crash inside
//  constructor/destructor.
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION
  Mpeg4Player::Mpeg4Player

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
Mpeg4Player::Mpeg4Player()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::Mpeg4Player" );

   ++guMpeg4PlayerRefCnt;
   // indicates whether the player thread is IDLE or not
   threadIdle = true;

#ifdef FEATURE_QTV_WM_DRM_API
   storedpDRMDecryptFunction = NULL;
   storedpClientData = NULL;
#endif

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   lastAudioPosition = 0;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

   audioPositionWhenSuspended = 0;

   //instance counter
   bInstance=false;

#ifdef FEATURE_QTV_PSEUDO_STREAM
   ps_last_ts =0;
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   http_last_ts =0;
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
   m_bMediaAbort = false;
#ifdef FEATURE_QTV_DIVX_DRM
   m_bCallCommitPlayback = false;
#endif

   if(1 == guMpeg4PlayerRefCnt)
   {
#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER
     m_bcastXmitTask = QTV_New_Args( QTV_BroadcastTransmitterTask,
                                   ( QTVBcastXmitPriority, "QTV_BC_Xmit", -1, "QTV" )
                                 );
#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */

     playerTC.threadHandle = NULL;
     playerTC.pThreadFunction = (QCThreadFuncType)ThePlayerThread;
     playerTC.stackSize = COMMON_STACK_SIZE;
     QCUtils::SetThreadName(&playerTC,Common::PLAYER_THREAD_NAME);

   }
   //update max url size
  URL::MaxUrlSize = (uint32)qtvConfigObject.GetQTVConfigItem(
                                          QtvConfig::QTVCONFIG_MAX_URL_SIZE);
  QTV_MSG1(QTVDIAG_STREAMING,
           "QTVCONFIG_MAX_URL_SIZE = %d",
           URL::MaxUrlSize);
  URL::MaxUrlSizeWithCtrlPath =
                     URL::MaxUrlSize + QTV_CONTROL_RELATIVE_URL_LEN;

  (void)videoURN.Init();
  (void)audioURN.Init();
  (void)textURN.Init();

  (void)url.Init();

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
  (void)URN.Init();
   #endif

  redirectURL = QTV_New_Args(URL, (URL::MaxUrlSize+1));

   QCUtils::InitCritSect(&URN_CS);
   QCUtils::InitCritSect(&URL_CS);
   QCUtils::InitCritSect(&clipInfo_CS);
   QCUtils::InitCritSect(&frameToRelease_CS);
   QCUtils::InitCritSect(&frameInfo_CS);
   QCUtils::InitCritSect(&AVSync_CS);

   InitDefaults();
#ifdef PLATFORM_LTK
#error code not present
#endif // PLATFORM_LTK


#ifndef FEATURE_QTV_MAPI_STREAM_MEDIA
   streamer = QTV_New(Streamer);
#endif /* !FEATURE_QTV_MAPI_STREAM_MEDIA */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::~Mpeg4Player

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
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
Mpeg4Player::~Mpeg4Player()
{
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Mpeg4Player::~Mpeg4Player" );

   if (streamer != NULL)
   {
     QTV_Delete(streamer);
   }

   if( (m_bcastXmitTask) && (1 == guMpeg4PlayerRefCnt) )
   {
     QTV_Delete(m_bcastXmitTask);
     m_bcastXmitTask = NULL;
   }

   if (NULL != redirectURL)
   {
     QTV_Delete(redirectURL);
   }

/*
* Don't want to Delete IxStream* for audio/video/text when we finish the playback
* or when we hit destructor of mpeg4file/asf file or when we invoke Oscl_FileClose.
* This is because, if user tries to play without exiting QTV, read, write, tell
* will fail and playback will be aborted.
*/
#ifdef FEATURE_QTV_DRM_DCF
  if( (clip.videoURN->bValid)                                      &&
      (clip.videoURN->type == QtvPlayer::URN_INPUTSTREAM)
    )
  {
    if(clip.videoURN->inputStream)
    {
      QTV_Delete((IxStream*)clip.videoURN->inputStream);
    }
  }

  /*
  * Unless user specifies different file names, audio/video/text
  * use same IxStream instance.So,when using same instance
  * don't need to do free/delete again.
  */
  if( (clip.audioURN->bValid)                                      &&
        (clip.audioURN->type == QtvPlayer::URN_INPUTSTREAM)
      )
  {
    if( (clip.audioURN->inputStream) &&
        (clip.audioURN->inputStream != clip.videoURN->inputStream) )
    {
        QTV_Delete((IxStream*)clip.audioURN->inputStream);
    }
  }

  if( (clip.textURN->bValid)                                      &&
        (clip.textURN->type == QtvPlayer::URN_INPUTSTREAM)
      )
  {
    if( (clip.textURN->inputStream)                              &&
        (clip.textURN->inputStream != clip.videoURN->inputStream)&&
        (clip.textURN->inputStream != clip.audioURN->inputStream))
    {
        QTV_Delete((IxStream*)clip.textURN->inputStream);
    }
  }
  clip.videoURN->inputStream = NULL;
  clip.audioURN->inputStream = NULL;
  clip.textURN->inputStream = NULL;
#endif

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

   /* Quick hack during Orange IOT */
     /* This needs to be moved / fixed out from here */
   if ((TRUE == clip.bDownload) && (IDLE == playerState))
   {
      if (!FileSavedToBuffer)
      {
         QTV_MSG(MSG_LEGACY_HIGH,"~Mpeg4Player: Purge the downloaded file?");
         HandlePurgeAfterDownload();/*lint!e1551 */
      }

      if (nPvxFilePath)
      {
         QTV_Free(nPvxFilePath);
         nPvxFilePath =NULL;
      }
      QTV_Delete(pPVDL);
   }
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


#if defined (FEATURE_QTV_PSEUDO_STREAM)
   //Cleanup the timer and the static event.
   if(httpPullData.pHttpPullTimer)
   {
       rex_delete_timer_ex(httpPullData.pHttpPullTimer);
       httpPullData.pHttpPullTimer = NULL;
   }
   if(httpPullData.pUpdateWptrOffsetPS)
   {
      QTV_Free(httpPullData.pUpdateWptrOffsetPS);
      httpPullData.pUpdateWptrOffsetPS = NULL;
   }
#endif //defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#if defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   if(httpPullData.pHttpPullTimer)
   {
       rex_delete_timer_ex(httpPullData.pHttpPullTimer);
       httpPullData.pHttpPullTimer = NULL;
   }
   if(httpPullData.pUpdateWptrOffsetPD)
   {
      QTV_Free(httpPullData.pUpdateWptrOffsetPD);
      httpPullData.pUpdateWptrOffsetPD = NULL;
   }
#endif
   --guMpeg4PlayerRefCnt;
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/
/* ======================================================================
FUNCTION:                                                            GRB
  Mpeg4Player::ReleaseVideoBuffer

DESCRIPTION:
  Notify the player that higher levels have finished rendering the
  current video frame.  The player is free to reuse or free the buffer
  after this function is called.

INPUT/OUTPUT PARAMETERS:
  pBuffer - pointer of the buffer to be released or reused.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void Mpeg4Player::ReleaseVideoBuffer( void *pBuffer )
{
  if (pBuffer != NULL)
  {
/* Due to some race condition between UI and MDP, have added critical sections to the following piece of code so that
    all the frames are released with out any issues */
    QCUtils::EnterCritSect(&frameToRelease_CS);
    frameToRelease.info.RGBBuffer = pBuffer;
    frameToRelease.info.YUVBuffer = pBuffer;
    numBuffersSent--;
#ifndef FEATURE_WINCE
    if (pActiveVideoPlayer)
    {
      pActiveVideoPlayer->ReleaseBuffer( frameToRelease.info );
    }
#endif   /*    FEATURE_WINCE   */
    QCUtils::LeaveCritSect(&frameToRelease_CS);
  }
}
/* ======================================================================
FUNCTION:
  Mpeg4Player::LogCumulativeClipStatistics

DESCRIPTION:
  Log the clip statistics and reset if requested.

INPUT/OUTPUT PARAMETERS:
  resetStats  - parameter to indicate whether the data needs to be reset.
  for ex: when the media wants to reset the stats when there is change in
  frame rate.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/

void Mpeg4Player::LogCumulativeClipStatistics(bool reset_stats)
{
#ifndef PLATFORM_LTK

  // Audio/Video Stats Structure
  QtvPlayer::AudioVideoStatisticsT stats;

#ifndef FEATURE_WINCE
  //Calculate the PlayMsec
  if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->SetPlayedbackTime();
  }
#endif   /*    FEATURE_WINCE   */

  //Get the Audio & Video Statistics
  (void)GetAudioVideoStatistics(stats);

#ifndef FEATURE_WINCE
  //Once again set the tResume
  if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->SetResumeTime();
  }
#endif   /*    FEATURE_WINCE   */

  //Log the Audio and Video Stats
  Mpeg4Player::LogClipStatistics(stats);

  /* Reset Statistics if needed */
  if(reset_stats)
  {
#ifndef FEATURE_WINCE
    //Reset Video Stats.
    if (pActiveVideoPlayer)
    {
      pActiveVideoPlayer->ResetData();
    }
#endif   /*    FEATURE_WINCE   */
  }

#endif //PLATFORM_LTK
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::MediaNotifyCB

DESCRIPTION:
  Processes the request from media Layer.

INPUT/OUTPUT PARAMETERS:
  reqType  - Request to be processed.
  reqData1 - Data sent by Media to process the request
  reqData2 - Data sent by Media to process the request

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void Mpeg4Player::MediaNotifyCB(Media::MediaLayerRequest reqType,
                                Mpeg4Player *pThis,
                                uint32 reqData1, uint32 reqData2)
{
  bool reset = false;

  if(pThis)
  {
    reqData2 = reqData2; //lint
    switch(reqType)
    {
      case Media::LOG_CUMULATIVE_CLIP_STATS:
      {
        //Calculate and log the cumluative clip statistics
        if(reqData1)reset = true;
        pThis->LogCumulativeClipStatistics(reset);
      }break;
      case Media::SET_PREROLL_RENDERER:
      {
#ifndef FEATURE_WINCE
        //Set the Preroll Renderer Count
         if (pThis->pActiveVideoPlayer)
         {
             ((VideoPlayer*)pThis->pActiveVideoPlayer)->SetPrerollRendererCount(reqData1);
         }
#endif   /*    FEATURE_WINCE   */
      }break;
      case Media::LOG_FRAME_INFO:
      {
        //Process
      }break;
      case  Media::OEM_LAYER_INFO:
      {
        //Pass the data to OEM layer
        // reqData1 is a void * and reqData2 is the size of data
        if((void *)reqData1 != NULL && reqData2 != 0)
        {
          Mpeg4Player::CallbackData cb ;
          QtvPlayer::PlayerStateRecordT ps;
          pThis->GetCallback(cb,ps);

          /*Allocate memory in OEM heap and copy the data sent.
            The memory if allocated will be freed by the OEM layer
            in callbacknotify function */
          if(cb.func || cb.instanceFunc)
          {
            QtvPlayer::StatusInfoT StatusInfo;
            StatusInfo.Data = pThis->OEMMallocBuffer(NULL,NULL,reqData2);
            if(StatusInfo.Data != NULL)
            {
              StatusInfo.Size = reqData2;
              memcpy(StatusInfo.Data,(void *)reqData1,reqData2);

              //Pass on the data to OEM Layer
              cb.pClientData = &StatusInfo;
              if(cb.func)
           {
              (cb.func)(QtvPlayer::QTV_PLAYER_STATUS_INFO, cb.pClientData, ps);
           }
           else if(cb.instanceFunc)
           {
              (cb.instanceFunc)(QtvPlayer::QTV_PLAYER_STATUS_INFO,
                           cb.pClientData, ps, cb.handle,cb.pUserData);
           }
            }
            else
            {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Memory allocation failed");
            }
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"CallBack Function is NULL");
          }
        }
      }break;

      // When we detect a audio channel switch, record the event in the
      // audio mgr
      case Media::AUDIO_CHANNEL_SWITCH_FRAME:
      {
#ifndef FEATURE_WINCE
        AudioPlayerIf::SetAudioChannelSwitchTS(pThis->activeAudioPlayerId, reqData1);
#endif   /*    FEATURE_WINCE   */

      }break;

      default :
      {
        //Do nothing
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Unknown request ignoring");
      }break;
    }
  }
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::IsVideoRenderingOkInState

DESCRIPTION:
  returns true if video rendering is OK in the given state.

INPUT/OUTPUT PARAMETERS:
  const State state
    The state about which we are curious.

RETURN VALUE:
  bool.
    true  - render away!
    false - no drawing for you!

SIDE EFFECTS:
  None.
======================================================================*/
bool Mpeg4Player::IsVideoRenderingOkInState( const Mpeg4Player::State state )
{
  bool rc;

  switch ( state )
  {
    case IDLE:
    case CONNECTING:
    case PB_READY:
    case OPENING:
    #if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    case DL_READY:
    case DOWNLOADING:
    case DOWNLOADED_DATA_AVAIL:
    #endif
    {
      rc = false;
      break;
    }
    case BUFFERING:
    case PLAYING:
    case PAUSED:
    case REPOSITIONING:
    #ifdef FEATURE_QTV_PSEUDO_STREAM
    case PLAYER_PSEUDO_PAUSE:
    #endif
    #ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    case PLAYER_HTTP_PAUSE:
    #endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
    case SUSPENDED:
    {
      rc = true;
      break;
    }
    default:
    {
      rc = false;
      QTV_MSG_PRIO1( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                     "Unknown state in IsVideoRenderingOkInState: %d",
                     state );
    }
  }

  return rc;
}

/* ======================================================================
FUNCTION:
 Mpeg4Player::setSpeed

DESCRIPTION:
  sets AccDuration for Fast start/Fast Cache

INPUT/OUTPUT PARAMETERS:
  speed

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
 void Mpeg4Player::setSpeed(float speedParam)
 {
   //store time locally and push to streamer once streamer is created
   speed = speedParam;
 }

/* ======================================================================
FUNCTION:
  Mpeg4Player::setAccDuration

DESCRIPTION:
  sets AccDuration for Fast start/Fast Cache

INPUT/OUTPUT PARAMETERS:
  speed

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
 void Mpeg4Player::setAccDuration(int time)
 {
   //store time locally and push to streamer once streamer is created
   acceleratedDuration = time;
 }

/* ======================================================================
FUNCTION
  Mpeg4Player::SetQTVConfigItem

DESCRIPTION
  A function to invoke the QTVConfiguration method to set the qtv config

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

========================================================================== */
bool Mpeg4Player::SetQTVConfigItem( QtvConfig::QtvConfigItemId item_id, int32 value )
{
    return qtvConfigObject.SetQTVConfigItem( item_id, value);
}

/* ======================================================================
FUNCTION
  Mpeg4Player::GetQTVConfigItem

DESCRIPTION
  A function to invoke the QTVConfiguration method to provide the QTV configuration parameter

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
========================================================================== */
int32 Mpeg4Player::GetQTVConfigItem( QtvConfig::QtvConfigItemId item_id )
{
    return qtvConfigObject.GetQTVConfigItem( item_id );
}

#if (defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || \
     defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO))
/* ======================================================================
FUNCTION
  Mpeg4Player::SetPlaybackSpeed

DESCRIPTION
  Sets the playback speed.

PARAMETERS
  Common::PlaybackSpeedType pbSpeed
    The playback speed.

RETURN VALUE
  None.
========================================================================== */
void Mpeg4Player::SetPlaybackSpeed(Common::PlaybackSpeedType pbSpeed)
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
                "Mpeg4Player::SetPlaybackSpeed = %d",
                pbSpeed);

  /* Update the player with the playback speed */
  m_playbackSpeed = pbSpeed;

  if (clip.bHasAudio)
  {
#ifndef FEATURE_WINCE
    AudioPlayerIf::SetPlaybackSpeed(activeAudioPlayerId, pbSpeed);
#endif   /*    FEATURE_WINCE   */
  }
#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
  else
  {
    /* Update the video and text players with the playback speed only if
    ** the clip does not contain audio. If audio is present it is the master
    ** and when it starts playing faster it will force both video and text
    ** to play faster.
    */
#ifndef FEATURE_WINCE
    if (clip.bHasVideo && pActiveVideoPlayer)
    {
      pActiveVideoPlayer->SetPlaybackSpeed(pbSpeed);
    }
    if (clip.bHasText)
    {
      textPlayer.SetPlaybackSpeed(pbSpeed);
    }
#endif   /*    FEATURE_WINCE   */
  }
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */
}
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
          FEATURE_QTV_FAST_PLAYBACK_AUDIO */

/* ======================================================================
FUNCTION
  Mpeg4Player::MapQtvPBSpeedToCommonPBSpeed

DESCRIPTION
  Maps the QtvPlayer playabck speed type to Common playback speed type.

PARAMETERS
  QtvPlayer::PlaybackSpeedT pbSpeed
    The playback speed.

RETURN VALUE
  Common::PlaybackSpeedTYpe
    The mapped playback speed.
========================================================================== */
Common::PlaybackSpeedType Mpeg4Player::MapQtvPBSpeedToCommonPBSpeed(
  QtvPlayer::PlaybackSpeedT pbSpeed)
{
  Common::PlaybackSpeedType mappedPBSpeed = Common::PLAYBACK_SPEED_NORMAL;

  switch (pbSpeed)
  {
    case QtvPlayer::PLAYBACK_SPEED_NORMAL:
    {
      mappedPBSpeed = Common::PLAYBACK_SPEED_NORMAL;
      break;
    }

    case QtvPlayer::PLAYBACK_SPEED_1P3X:
    {
      mappedPBSpeed = Common::PLAYBACK_SPEED_1P3X;
      break;
    }

    default:
    {
      /* Do nothing */
        break;
    }
  } /* switch (pbSpeed) */

  return mappedPBSpeed;
}

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ======================================================================
FUNCTION
  Mpeg4Player::UpdateTrackListInfo

DESCRIPTION
  Updates the track list information.

PARAMETERS
  QtvPlayer::MediaTypeT media.
    Media whose track list is to be updated.

RETURN VALUE
  None.
========================================================================== */
void Mpeg4Player::UpdateTrackListInfo(QtvPlayer::MediaTypeT media)
{
  /* Update audio's track list info */
  if (media == QtvPlayer::MEDIA_AUDIO_ONLY)
  {
    /* Initialize the track list info */
    memset(&m_audioTrackListInfo, 0, sizeof(m_audioTrackListInfo));

    Media *pMedia = GetAudioMedia();
    if (pMedia)
    {
      // Get MIME Types
      ReferenceCountedPointer<ITrackList> trackList;

      if (!pMedia->GetTrackList(MakeOutPointer(trackList)) ||
        (trackList == NULL))
      {
        return;
      }

      if (trackList != NULL)
      {
        m_audioTrackListInfo.nTrackID = trackList->GetTrackID(selectedAudioTrackIdx);
        m_audioTrackListInfo.trackType = MapMediaCodecToQtvPlayerCodec(GetAudioCodecType());
      }
    }

    m_audioTrackListInfo.mediaType = media;

    /* Get the audio track specs */
    uint32 trackState = 0;
    Common::ChannelConfigType channelConfig = Common::AUDIO_CHANNEL_UNKNOWN;
    int samplingFreq = 0;
    bool bOk = AudioPlayerIf::GetAudioTrackSpec(activeAudioPlayerId,
                                                trackState,
                                                channelConfig,
                                                samplingFreq);

    if (bOk)
    {
      m_audioTrackListInfo.bmTrackState = trackState;

      m_audioTrackListInfo.codecInfo.aacInfo.nSamplingFreq = samplingFreq;

      m_audioTrackListInfo.codecInfo.aacInfo.eChannelConfig =
        MapCommonChConfigToQtvChConfig(channelConfig);
    }
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::MapCommonChConfigToQtvChConfig

DESCRIPTION
  Maps the Common channel configuration type to QtvPlayer channel
  configuration type.

PARAMETERS
  Common::eChannelConfigType channelConfig
    The audio channel configuration.

RETURN VALUE
  QtvPlayer::ChanneConfigT
    The mapped audio channel configuration.
========================================================================== */
QtvPlayer::ChannelConfigT Mpeg4Player::MapCommonChConfigToQtvChConfig(
  Common::ChannelConfigType channelConfig)
{
  QtvPlayer::ChannelConfigT mappedChannelConfig;

  switch (channelConfig)
  {
    case Common::AUDIO_CHANNEL_UNKNOWN:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_UNKNOWN;
      break;
    }

    case Common::AUDIO_CHANNEL_MONO:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_MONO;
      break;
    }

    case Common::AUDIO_CHANNEL_DUAL:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_DUAL;
      break;
    }

    case Common::AUDIO_CHANNEL_TRIPLE:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_TRIPLE;
      break;
    }

    case Common::AUDIO_CHANNEL_QUAD:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_QUAD;
      break;
    }

    case Common::AUDIO_CHANNEL_QUINTUPLE:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_QUINTUPLE;
      break;
    }

    case Common::AUDIO_CHANNEL_SEXTUPLE:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_SEXTUPLE;
      break;
    }

    case Common::AUDIO_CHANNEL_OCTUPLE:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_OCTUPLE;
      break;
    }

    case Common::AUDIO_CHANNEL_DUAL_MONO:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_DUAL_MONO;
      break;
    }

    case Common::AUDIO_CHANNEL_UNSUPPORTED:
    default:
    {
      mappedChannelConfig = QtvPlayer::AUDIO_CHANNEL_UNSUPPORTED;
      break;
    }
  } /* switch (channelConfig) */

  return mappedChannelConfig;
}

/* ======================================================================
FUNCTION
  Mpeg4Player::UpdateMaxTimestamp

DESCRIPTION
  Gets the timestamp rollover poit from media and updates the renderer.

PARAMETERS
  None.

RETURN VALUE
  None.
========================================================================== */
void Mpeg4Player::UpdateMaxTimestamp()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::UpdateMaxTimestamp");

  bool bResult =
    GENERIC_BCAST_MEDIA_GET_MAX_TIMESTAMP(clip.pVideoMpeg4Playback,
                                          m_uMaxTimestamp);

  if (!bResult)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "UpdateMaxTimestamp: Get timestamp from media failed");
  }
}

/* ======================================================================
FUNCTION
  Mpeg4Player::UpdateRendererQueuingMode

DESCRIPTION
  Gets the rendering queue mode from media and updates the renderer.

PARAMETERS
  None.

RETURN VALUE
  None.
========================================================================== */
void Mpeg4Player::UpdateRendererQueuingMode()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::UpdateRendererQueuingMode");

  bool isFIFORendering =
    GENERIC_BCAST_MEDIA_GET_IS_FIFO_RENDERING(clip.pVideoMpeg4Playback);

#ifndef FEATURE_WINCE
  if (pActiveVideoPlayer)
  {
    if (isFIFORendering)
    {
       /* Set FIFO rendering */
       ((VideoPlayer*)pActiveVideoPlayer)->SetRendererQueuingMode(VideoPlayer::RENDERER_QUEUING_MODE_FIFO);
    }
    else
    {
       /* Set SORTING (default) rendering */
       ((VideoPlayer*)pActiveVideoPlayer)->SetRendererQueuingMode(VideoPlayer::RENDERER_QUEUING_MODE_SORTING);
    }
  }
#endif   /*    FEATURE_WINCE   */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::ActivatePCRClockSync

DESCRIPTION
  Activates PCR clock sync and sets the maximum timestamp at which
  rollover occurs.
  For live playback (PCR is active), rollover is handled by the PCR
  mechanism, so rollover is set to 0xFFFFFFFF.

PARAMETERS
  None.

RETURN VALUE
  None.
========================================================================== */
void Mpeg4Player::ActivatePCRClockSync()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::ActivatePCRClockSync");

  /* PCR is active and will handle rollover. Set max timestamp to 0xFFFFFFFF */
  if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->SetMaxTimestamp(QTV_MAX_UINT32);
  }
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /*FEATURE_QTV_AUDIO_DISCONTINUITY */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
}

/* ======================================================================
FUNCTION
  Mpeg4Player::DeactivatePCRClockSync

DESCRIPTION
  Deactivates PCR clock sync and sets the maximum timestamp at which
  rollover occurs.
  For non-live playback (PCR is not active), rollover needs to be set. The
  rollover timestamp is obtained from the media source during open.

PARAMETERS
  None.

RETURN VALUE
  None.
========================================================================== */
void Mpeg4Player::DeactivatePCRClockSync()
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
               "Mpeg4Player::DeactivatePCRClockSync");

  /* PCR is not active. Set max timestamp to value obtained from media. */
  if (pActiveVideoPlayer)
  {
    pActiveVideoPlayer->SetMaxTimestamp(m_uMaxTimestamp);
  }
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /*FEATURE_QTV_AUDIO_DISCONTINUITY */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
}
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
/* ======================================================================
FUNCTION
  Mpeg4Player::SetDualMonoOutput

DESCRIPTION
  Sets the dual mono output configuration.

PARAMETERS
  Common::DualMonoOutputType dualMonoOutput
    The desired dual mono output configuration.

RETURN VALUE
  bool
    TRUE if configuration was set successfully, FALSE otherwise.
========================================================================== */
bool Mpeg4Player::SetDualMonoOutput(Common::DualMonoOutputType dualMonoOutput)
{
  bool bRet = false;
#ifndef FEATURE_WINCE
  bRet = AudioPlayerIf::SetDualMonoOutput(activeAudioPlayerId, dualMonoOutput);
#endif   /*    FEATURE_WINCE   */
  return bRet;
}
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

uint32 Mpeg4Player::GetTotalRxDataRate()
{
  return streamer->getTotalRxDataRate();
}
/* ======================================================================
FUNCTION
  Mpeg4Player::SetVDECParameter

DESCRIPTION
  Sets the VDEC parameter

PARAMETERS
  QtvVDECParameterIdT inputParamId
    input VDEC parameter id
  QtvVDECParameterDataT* pInputParam
    pointer to input VDEC parameter value

RETURN VALUE
  QtvPlayer::ReturnT
    Enum of the Qtv return type.
========================================================================== */
QtvPlayer::ReturnT Mpeg4Player::SetVDECParameter( QtvPlayer::QtvVDECParameterIdT inputParamId,  QtvPlayer::QtvVDECParameterDataT* pInputParam )
{
  QtvPlayer::ReturnT nReturn = QtvPlayer::QTV_RETURN_ERROR;
  VDEC_ERROR nVDECReturn = VDEC_ERR_MYSTERY_ERROR;

#ifndef FEATURE_WINCE
  if (pActiveVideoPlayer)
  {
      nVDECReturn = ((VideoPlayer*)pActiveVideoPlayer)->SetVDECParameter((VDEC_PARAMETER_ID)inputParamId, (VDEC_PARAMETER_DATA *)pInputParam);
  }
#endif   /*    FEATURE_WINCE   */

  if(nVDECReturn == VDEC_ERR_EVERYTHING_FINE)
  {
    nReturn = QtvPlayer::QTV_RETURN_OK;
  }
  return nReturn;
}


/* ======================================================================
FUNCTION
  Mpeg4Player::SetIncallVoiceCodec

DESCRIPTION
  Sets the voice codec type inside decoder Interface during incall Qtv

PARAMETERS
  Media::CodecType eVocoderType

RETURN VALUE
  void
========================================================================== */
void Mpeg4Player::SetIncallVoiceCodec(Media::CodecType eVocoderType)
{
#ifndef FEATURE_WINCE
   if (pActiveVideoPlayer)
   {
#ifdef FEATURE_QTV_IN_CALL_VIDEO
#error code not present
#else
    ((VideoPlayer*)pActiveVideoPlayer)->SetIncallVoiceCodec(eVocoderType);
#endif /* FEATURE_QTV_IN_CALL_VIDEO */
   }
#endif   /*    FEATURE_WINCE   */
}

/* ======================================================================
FUNCTION:
  Mpeg4Player::UpdateClipInfoDimensions

DESCRIPTION:
 This is a sync funciton which updates the height & width of clipinfo and notify to UI for clipinfo validation.

INPUT/OUTPUT PARAMETERS:
  int height : height of enoded frame.
  int width  : width of encoded frame.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
void  Mpeg4Player::UpdateClipInfoDimensions(int height, int width)
{
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
    "Mpeg4Player::UpdateClipInfoDimensions() : START");

    if( (Media::MPEG4_CODEC  == GetVideoCodecType()) ||
      (Media::H263_CODEC  == GetVideoCodecType()) ||
      (Media::H264_CODEC  == GetVideoCodecType()))
   {
      QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
      " SetImageDimensions(width:%d, height:%d)", height, width);
    //update clipinfo dimensions here
    SetImageDimensions(width, height);

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
    " Notify(QtvPlayer::QTV_PLAYER_STATUS_VALID_CLIPINFO_DIMENSIONS) : Notify to UI for clipinfo validation");
    //Notify to UI for clipinfo validation
    Notify(QtvPlayer::QTV_PLAYER_STATUS_VALID_CLIPINFO_DIMENSIONS);
  }

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
    " Mpeg4Player::UpdateClipInfoDimensions() : END");
}

#ifndef FEATURE_WINCE
/* ======================================================================
FUNCTION
  Mpeg4Player::WriteStatisticsToEFS

DESCRIPTION
  To write the clip statistics to a text file, if config item is set

PARAMETERS
  QtvPlayer::AudioVideoStatisticsT stats

RETURN VALUE
  0 for failure and 1 for success
========================================================================== */

int  Mpeg4Player::WriteStatisticsToEFS (QtvPlayer::AudioVideoStatisticsT stats)
{
#ifndef PLATFORM_LTK

    char str[QTV_STATS_LOCAL_BUFFER_SIZE];
    if (sUrnStatsName[0] != '\0')
    {
      char *endpos;
      char *pos = str;
      OSCL_FILE* fp = NULL;
     memset (str,0x00,QTV_STATS_LOCAL_BUFFER_SIZE);
      (void)std_strlcpy(str, "dbg/", strlen("dbg/") + 1);
      (void)std_strlcat(str, sUrnStatsName, sizeof(str));

      /* Make sure the complete directory path exists */
      endpos = strrchr(str, '/');
      if (endpos != NULL)
      {
         *endpos = '\0';
         while(pos)
         {
            pos = strchr(pos, '/');
            if (pos)
            {
               *pos = '\0';
            }
            efs_mkdir(str, S_IREAD | S_IWRITE);
            if (pos)
            {
               *pos = '/';
               pos++;
            }
         }
         *endpos = '/';
      }
      /* add the Clip STats extension, and open the file */
      (void)std_strlcat(str, ".cst", sizeof(str));
      fp = OSCL_FileOpen((OSCL_CHAR *)str, (OSCL_TCHAR *) _T("wb"));
      /* Fill in the statistics */
      if ( fp!= NULL )
      {
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (clip.bHasVideo)
#else
    if (clip.bHasVideo || IsVideoMuted())
#endif
    {
         /* Write the clip stats to file */
         std_strlprintf(str, sizeof(str), "Video:            \n\n");
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Decoded Frame Rate:             %8lu\r\n", stats.Video.decodedFrameRate);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Displayed Frame Rate:           %8lu\r\n", stats.Video.displayedFrameRate);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Number Frames:                  %8lu\r\n", stats.Video.frames);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Video Frames Interpolated:          %8lu\r\n", stats.Video.framesInterpolated);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Frames Dropped - Unrecoverable: %8lu\r\n", stats.Video.failedDecode);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Frames Dropped - Decode Time:   %8lu\r\n", stats.Video.skippedDecode);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Frames Dropped - Render Time:   %8lu\r\n", stats.Video.skippedDisplay);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Frames Dropped due to AV sync:   %8lu\r\n", stats.Video.avSyncDrops);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Total MacroBlocks Concealed:   %8lu\r\n",stats.Video.skippedDisplayDueToErrs);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Max consecutive frame drops:   %8lu\r\n",stats.Video.maxDrop);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  maxVideoDataLead:          %8ld\r\n", stats.Video.maxDataLead);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  minVideoDataLead:          %8ld\r\n", stats.Video.minDataLead);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Video Encoded Frame Rate:       %8lu\r\n", stats.Video.encodedFrameRate);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Video Format:                   %8lu\r\n", stats.Video.format);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Video Encoded Bitrate:          %8lu\r\n\n", stats.Video.encodedBitrate);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
   }

     if( stats.AVSync.avSyncValid )
        {
            std_strlprintf(str, sizeof(str), "A/V Sync:        \n");
            (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
            std_strlprintf(str, sizeof(str), "  Max Msec Video Ahead:           %8lu\r\n", stats.AVSync.videoAheadMsec);
            (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
            std_strlprintf(str, sizeof(str), "  Max Msec Video Behind:          %8lu\r\n\n", stats.AVSync.videoBehindMsec);
            (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
          }
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (clip.bHasAudio)
#else
    if (clip.bHasAudio || IsAudioMuted())
#endif
    {
     std_strlprintf(str, sizeof(str), "Audio:            \n");
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Audio Format:                   %8lu\r\n",stats.Audio.format);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Audio Encoded Bitrate:          %8lu\r\n", stats.Audio.encodedBitrate);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  maxAudioDataLead:          %8ld\r\n", stats.Audio.maxAudioDataLead);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  minAudioDataLead:          %8ld\r\n", stats.Audio.minAudioDataLead);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  max consecutive AudioDrop:          %8lu\r\n",  stats.Audio.maxDrop);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Number Audio Frames:          %8lu\r\n", stats.Audio.frames);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
        std_strlprintf(str, sizeof(str), "  Null Audio Samples:          %8lu\r\n\n", stats.Audio.nullSamples);
        (void)OSCL_FileWrite ((OSCL_CHAR *)str, (strlen(str)), 1, fp );
     }


#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#ifdef FEATURE_MP4_UNUSED_CODE
#error code not present
#endif /* FEATURE_MP4_UNUSED_CODE */
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

     (void)OSCL_FileClose(fp);
        fp = NULL;
      }
      else
      {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unable to open file");
       return 0;
       }
    }
    else
    {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Invalid file name");
                return 0;
    }
#endif
    return 1;
}
#endif   /*    FEATURE_WINCE   */

#ifdef FEATURE_WINCE
#error code not present
#endif   /*    FEATURE_WINCE   */

#ifdef FEATURE_QTV_FCS
#error code not present
#endif /* FEATURE_QTV_FCS */
