/* =======================================================================
                               audio.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audio.cpp#34 $
$DateTime: 2010/08/17 05:07:51 $
$Change: 1404572 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "audio.h"
#include "Events.h"
#include "mp4dec_api.h"
#include "AVSync.h"
#include "audioMsgTypes.h"
#include "audioPlayerIf.h"
#include "mpeg4player.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
// #define SHOW_AUDIO_DEBUG_INFO
//#define SHOW_DEBUG_INFO
//#define SHOW_POLLING

//how often do we try to correct the timing?
static const int TIMING_UPDATE_INTERVAL_MSEC  =1000;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

// Thread Event Table

// Source
EVENT_POOL( AUDIO_POOL, 50 )
//to player
EVENT_SOURCE( AUDIO_STATUS, AUDIO_POOL )
EVENT_SOURCE( AUDIO_TIMING, AUDIO_POOL )
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
EVENT_SOURCE( AUDIO_SPEC, AUDIO_POOL )
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */


#ifdef FEATURE_QTV_DRM_DCF
#include "zrex.h"
#include "rex.h"
#include "ixipc.h"
#endif

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  AudioPlayer::ShowInfo

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
#ifdef AUDIO_SHOW_INFO
void AudioPlayer::ShowInfo()
{
  if(audioMgr)
  {
    audioMgr->ShowInfo();
  }
}
#endif /* AUDIO_SHOW_INFO */

/* ======================================================================
FUNCTION
  AudioPlayer::IsDone

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
bool AudioPlayer::IsDone()
{
  return TrackHasEnded();
}

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
/* ======================================================================
FUNCTION
  AudioPlayer::IsPlaying

DESCRIPTION
//
// tell if audio is playing.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::IsPlaying()
{
  #ifndef FEATURE_QTV_AUDIO_DISCONTINUITY
  return(state==PLAYING) ;
  #else /* FEATURE_QTV_AUDIO_DISCONTINUITY */
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
}

/* ======================================================================
FUNCTION
  AudioPlayer::IsPaused

DESCRIPTION
//
// tell if audio is paused.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::IsPaused()
{
  return(state==PAUSED) ;
}

/* ======================================================================
FUNCTION
  AudioPlayer::IsPausedDisabled

DESCRIPTION
//
// tell if audio is paused_disabled.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::IsPausedDisabled()
{
  return(state==PAUSED_DISABLED) ;
}

/* ======================================================================
FUNCTION
  AudioPlayer::IsIdle

DESCRIPTION
//
// tell if audio is Idle.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::IsIdle()
{
  return(state==IDLE) ;
}

/* ======================================================================
FUNCTION
  AudioPlayer::Destroy

DESCRIPTION
//
//  force an immediate shutdown and cleanup
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::Destroy()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Destroy");

  //cleanup
  CleanupPlay();
  if( audioMgr )
  {
  //destroy audio interface
    audioMgr->CloseAudioManager();
    QTV_Delete( audioMgr );
    audioMgr = NULL;
  }
  m_pClientData = NULL;
}

/* ======================================================================
FUNCTION
  AudioPlayer::SetPlayTimes

DESCRIPTION
//set start & stop times.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::SetPlayTimes(long play,long stop)
{
  if ( play>=0 )
  {
    #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
    #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
    if( audioMgr )
    {
      audioMgr->InitElapsedTime(play);
    }
  }
  if( audioMgr )
  {
    audioMgr->SetPlayTimes(play,stop);
  }
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool AudioPlayer::PollDownload(int playbackID,bool &bError)
{
  bError=false;
  if (CanPollDownload())
  {
    currentPlaybackID = playbackID;
    if (!SendCommand(POLL_DOWNLOAD, COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC))
    {
      //ErrorMsg( LOG_LVL_ERROR, "ERROR: Audio start timeout\n",0,0,0);
      bError=true;
    }
    return true;
  }
  //ErrorMsg( LOG_LVL_ERROR, "ERROR: AudioPlayer::PreBuffer failed\n",0,0,0);
  return false;
}
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

/* ======================================================================
FUNCTION
  AudioPlayer::SetTimeStampfromBytes

DESCRIPTION
//set start & stop times.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::SetTimestampfromBytes(long timestamp)
{
    if( audioMgr )
    {
      audioMgr->SetTimeStamp(timestamp);
    }
}

/* ======================================================================
FUNCTION
  AudioPlayer::Prep

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
bool AudioPlayer::Prep(int playbackID,bool bRestart,Media * pMpeg4In)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Prep");

  bLocalRestart = bRestart;

  if ( CanStart(bRestart) )
  {
    if ( !bRestart )
    {
      //save playback ID
      currentPlaybackID = playbackID;

      //save mp4 pointer.
      pMpeg4 = pMpeg4In;
    }

    if(!audioMgr)

    {
      initializeAudioplayer(NULL, pMpeg4In);
      if(!audioMgr)
      {
         return false;
      }
    }

    if(audioMgr)
    {
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
      audioMgr->SetAudioCodec(m_audioCodec);
    }

    /* If Media has reached to end, but CMX has not yet done playing and
       we get another Prep, then reset MEDIA STATUS back to MEDIA_OK, so
       that audio player can run and then exit properly */
    if( !TrackHasEnded() &&
        audioMgr->GetMediaStatus()== Media::DATA_END )
    {
      audioMgr->SetMediaStatus(Media::DATA_OK);
      audioDataState = Media::DATA_OK;
    }

    InitPlayData();

    if( audioMgr )
    {
      /*
      * Once we set audio codec in audioMgr, invoke
      * audioMgr->InitPlayData. This will make sure that
      * if BYTE BASED AV SYNC is enabled for given audio codec,
      * and if we have released DSP earlier, restore
      * Buffer entries won't be flushed out.
      * bRestart == FALSE indicates that the audio interface is
      * being created.
      */
      audioMgr->InitPlayData(bRestart);
      
      /* For DCF playback, Calling task needs to register an IPC signal. This signal is set by IxStream on completion
         * of an asynchronous operation. IPC handler for the calling task will make sure
         * to invoke IxStreamDispatchMsg which eventually invokes the callback provided by QTV in calling task context.
      */
      #ifdef FEATURE_QTV_DRM_DCF
      if(m_bIsAudioTaskRegisteredForIPC == false)
      {
        /*
         * Before reading any audio data from IxStream,
         * AUDIO task needs to have an IPC signal and to be registered with ixipc.
         * Without this, read from AUDIO task will get blocked foreever on DRM suite.
         */
         ixipc_init(0,QTV_DRM_DCF_IPC_SIG) ;
         m_bIsAudioTaskRegisteredForIPC = true;
      }
      #endif

      if ( !audioMgr->Prep(pMpeg4,TIMING_UPDATE_INTERVAL_MSEC,bRestart) )
      {
        QTV_MSG( QTVDIAG_AUDIO_TASK, "audioMgr.Prep failed");
        AbortAudioOutput();
        return false;
      }

      #ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
      /* At this point both audioMgr and audioCMX have been created, so set
      ** the dual mono output selection.
      */
      audioMgr->SetDualMonoOutput(m_dualMonoOutput);
      #endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

    }

    if(!bRestart)
	{
	    //CMX interface was created VIA audioMgr::Prep. Reset m_bCMXInterfaceStartDone
	    //to indicate we need to call START first bfore we could issue RESUME.
	    //Please refer to the "audio.h" where this variable is declared to know
	    //under which scenarios, this could happen.
	    m_bCMXInterfaceStartDone = FALSE;
	    QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,"CMX interface created via audioMgr::Prep, setting m_bCMXInterfaceStartDone to FALSE");
	}

    return true;
  }
  return false;
}


/* ======================================================================
FUNCTION
  AudioPlayer::Start

DESCRIPTION
//
//  start playing
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
bool AudioPlayer::Start(bool &bError,bool bRestart)
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Start");

  bError=false;
  bLocalRestart = bRestart;

  if ( CanStart(bRestart)
       ///make sure player was prepped.
       && pMpeg4!=NULL )
  {
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
    //command audio thread to start.
    if ( !SendCommand(PLAY, COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC) )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio start timeout");
      bError=true;
    }
    return true;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AudioPlayer::Start failed");
  return false;
}

/* ======================================================================
FUNCTION
  AudioPlayer::Stop

DESCRIPTION
//
//  stop playing
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::Stop(bool &bError)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Stop");
  bError=false;
  bPlayingNotifyPending = FALSE;
  if ( !SendCommand(STOP, COMMON_AUDIO_STOP_TIMEOUT_MSEC) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio stop timeout");
    bError=true;
  }
}

/* ======================================================================
FUNCTION
  AudioPlayer::Suspend

DESCRIPTION
//
//  suspend playing
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::Suspend(bool &bError)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Suspend");

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  /*
    Reposition is being done, so reset restore data buffer to take care of the
    case when "qtv_cfg_enable_dsp_release" is TRUE.
  */
  if( audioMgr )
  {
    (void)audioMgr->FlushRestoreDataBufferInfo();

  }
#endif
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif

  bError=false;

  bPlayEnding = false;
  audioDataState = Media::DATA_OK;

  if ( CanSuspend() )
  {
    bPlayingNotifyPending = FALSE;
    if ( !SendCommand(SUSPEND, COMMON_AUDIO_STOP_TIMEOUT_MSEC) )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio suspend timeout");
      bError=true;
    }
    return true;
  }
  return false;
}


/* ======================================================================
FUNCTION
  AudioPlayer::Pause

DESCRIPTION
//
//  pause playing.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::Pause(bool &bError)
{
  bool bOk = true;
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Pause");
  bError=false;

  if ( CanPause() )
  {
    bPlayingNotifyPending = FALSE;
    if ( !SendCommand(PAUSE, COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC) )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio pause timeout");
      bError=true;
    }
  }
  /* If the Audio state not changed to paused then return false. */
  if(state != PAUSED )
  {
    bOk = false;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio Pause Failed");
  }
  return bOk;
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool AudioPlayer::CanPollDownload()
{
	return (state==IDLE);
}
#endif

/* ======================================================================
FUNCTION
  AudioPlayer::Disable

DESCRIPTION
  Disable playing.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::Disable(bool &bError)
{
  bError=false;

  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Disable");

  bPlayingNotifyPending = FALSE;
  if ( !SendCommand(DISABLE_AUDIO, COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio disable timeout");
    bError=true;
  }
  return true;
}

/* ======================================================================
FUNCTION
  AudioPlayer::Resume

DESCRIPTION
//
//  resume from a paused state.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::Resume(bool &bError,long stop)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Resume");

  bError=false;

  if ( CanResume())
  {
    if( audioMgr )
    {
      audioMgr->SetPlayTimes((-1),stop);
    }
    if ( !SendCommand(PLAY, COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC) )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio resume timeout");
      bError=true;
    }
    return true;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio Resume Failed");
  return false;
}

/* ======================================================================
FUNCTION
  AudioPlayer::SetAudioChannelSwitchTS

DESCRIPTION
  Sets the Channelswitched frame TS.
  (For real time channel switching scenario's)

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::SetAudioChannelSwitchTS(uint32 ChannelSwitchedTS)
{
  if( audioMgr )
  {
    audioMgr->SetChannelSwitchTS(ChannelSwitchedTS);
  }
}

/* ======================================================================
FUNCTION
  AudioPlayer::ResetData

DESCRIPTION
//reset all the data that can be retrieved by the AV player

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::ResetData()
{
  if( audioMgr )
  {
  //init elapsed time
    audioMgr->InitElapsedTime(0);
  }
}

/* ======================================================================
FUNCTION
  AudioPlayer::initializeAudioplayer

DESCRIPTION
Create and initialize audio manager based on the audio MIME type.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::initializeAudioplayer
(
  const char *audio_type,
  Media* pMedia
)
{
  //If the audioMgr is not created then only create an audioMgr.
  if(audioMgr == NULL)
  {
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    /*if audio only file type create generic audio manger
      otherwise create audio manger */
    if (audio_type && (strcmp(audio_type, AUDIO_ONLY_MIME_TYPE) == 0))
    {
      audioMgr = QTV_New( genericAudioMgr );
    }
    else
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
    {
      audioMgr = QTV_New_Args( AudioMgr, (pMedia) );
    }
  }
  if( audioMgr )
  {
    audioMgr->SetAVSyncObj(m_pAVSync);
    audioMgr->InitElapsedTime(0);
    audioMgr->InitPlayData(false);

    audioMgr->SetClientData(this);

    audioMgr->InitFrame();
    audioMgr->InitStatistics();
    audioMgr->SetAudioCodec(m_audioCodec);
    #ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO
      // At times when player thread sets the playback speed, Audio Manager
      // may not exist. In those scenarios, received command is queued.
      // Queued up command is issued here. If there were more than one setting
      // latest command is preserved in the audio player object.
      if(m_bPlaybackSpeedQueued)
      {
        QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH,
        "Issued Queued up playback speed setting command for %d",m_ePlaybackSpeed);
        // clear the boolean flag
        m_bPlaybackSpeedQueued = false;
        // Issue the playback setting command to audio manager.
        SetPlaybackSpeed(m_ePlaybackSpeed);
      }
    #endif
    //create audio interface
    if ( !audioMgr->OpenAudioManager(AudioNotice, (void*) this) )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AudioPlayer::Create failed");
    }
  }
  /* to fix the compilation warning if the
     FEATURE_QTV_GENERIC_AUDIO_FORMAT is turned off*/
 (void)audio_type;
}

/* ======================================================================
FUNCTION
  AudioPlayer::GetPosition

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
bool AudioPlayer::GetPosition(int32 &pos)
{
    //get position from audio interface
   if( audioMgr )
   {
     pos=audioMgr->GetFrame();
     return true;
   }
   return false;
}

/* ======================================================================
FUNCTION
  AudioPlayer::GetElapsedTime

DESCRIPTION
//
// figure out elapsed audio play time.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
long AudioPlayer::GetElapsedTime()
{
  if(audioMgr)
  {
    return audioMgr->GetElapsedTime();
  }
  return 0;
}


/* ======================================================================
FUNCTION
  AudioPlayer::GetStatistics

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
bool AudioPlayer::GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                                Common::LastFrame &lastFrame)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::GetStatistics");

  if(audioMgr)
  {
    //get stats from audio interface
    audioMgr->GetStatistics(stat,lastFrame);
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION
  AudioPlayer::InitDefaults

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
void AudioPlayer::InitDefaults()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::InitDefaults");

  currentPlaybackID = 0;
  durationBuffered = 0;
  if( audioMgr )
  {
    audioMgr->InitElapsedTime(0);
  }
  InitPlayData();
  pMpeg4=NULL;
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

  checkTimeFlag = false;
  bPlayingNotifyPending = FALSE;
  AudioAborted = false;

  starvingTime   = 0;
  abortAfterward = true;
  m_bHasVideo = false;

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */
}

/* ======================================================================
FUNCTION
  AudioPlayer::InitPlayData

DESCRIPTION
//
// init data for a new playback interval.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::InitPlayData()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::InitPlayData");

  audioTime.tEndDelay=0;
  audioTime.nWaitMsec=0;

  InitInterfaceState();

  bPlayEnding=false;
  bPlayDoneRendering=false;
  bPlayEndingMustBeSet = false;
  // Need to reset audioDataState, such that if reposition/stop is issued when QTV
  //    is in buffering state, next play request will properly put QTV in BUFFERING
  //    instead of REBUFFERING state.
  audioDataState = Media::DATA_OK;
  // Configure or Re-configure play mode members
  m_bPlayModeValid = false;
  m_bPlayMode      = false;
}


/* ======================================================================
FUNCTION
  AudioPlayer::CleanupPlay

DESCRIPTION
//
// cleanup after playback

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::CleanupPlay(void)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::CleanupPlay");

  if( audioMgr )
  {
    audioMgr->CleanupPlay();
  }
  m_bCMXInterfaceStartDone = FALSE;
  pMpeg4=NULL;
  // Configure or Re-configure play mode members
  m_bPlayModeValid = false;
  m_bPlayMode      = false;
}


/* ======================================================================
FUNCTION
  AudioPlayer::AudioNotice

DESCRIPTION
  This is the callback from AudioCMX for event notification.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::AudioNotice
(
  Common::AudioNotifyType notify,
  void* pClientData,
  void* pData
)
{
  AudioPlayer *pThis = (AudioPlayer *)pClientData;
  QTV_USE_ARG1(pData);

  if ( pThis )
  {
    switch ( notify )
    {
    case Common::NOTIFY_IMAGE_CHANGE:
      pThis->InitPlayData();
    break;
    case Common::NOTIFY_DONE:
      pThis->interfaceState.tDone = ZUtils::Clock();
      pThis->interfaceState.bDone=true;
      pThis->InterfaceStateUpdate();
      break;
    case Common::NOTIFY_DELAY:
      pThis->interfaceState.tDataDelay = ZUtils::Clock();
      pThis->interfaceState.bDataDelay=true;
      if(pThis->bPlayEndingMustBeSet)
          pThis->bPlayEnding = TRUE;
      pThis->InterfaceStateUpdate();
      break;
    case Common::NOTIFY_ERROR:
      pThis->interfaceState.tError = ZUtils::Clock();
      pThis->interfaceState.bError=true;
      pThis->InterfaceStateUpdate();
      break;
    case Common::NOTIFY_EXCEPTION:
      pThis->interfaceState.bException=true;
      pThis->InterfaceStateUpdate();
      break;
    case Common::NOTIFY_TIMING:
      pThis->TimingUpdate();
      break;
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    case Common::NOTIFY_SPEC:
      pThis->NotifySPec();
      break;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
case Common::NOTIFY_INITIATE_IMMEDIATE_DATA_DELIVERY:
      /*
       * Audio thread polls for DATA_REQUEST every REQUEST_POLL_TIME_MSEC milliseconds.
       * For ARM based decoder,we need to provide the data in strict timely manner
       * otherwise, it leads to glitches.This is because the the audio data duration sent to CMX
       * is typically smaller than REQUEST_POLL_TIME_MSEC.
       * Reducing REQUEST_POLL_TIME_MSEC does not help either as
       * audio keeps waking too frequently which kind of puts additional overhead.
       * So, we need to wake audio thread immediately as soon as CMX requests DATA.
       */
      pThis->InterfaceStateUpdate();
      break;
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
    case Common::NOTIFY_TRACK_STATE_CHANGE:
    {
      //Simply post the event using the Player callback.
      QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::Notify  Track State Change");
      pThis->NotifyTrackStateChange();
      break;
    }
    default:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unknown message(%d) in AudioNotice", notify);
      break;
    }
  }
}


/* ======================================================================
FUNCTION
  AudioPlayer::InitInterfaceState

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
void AudioPlayer::InitInterfaceState()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::InitInterfaceState");

  interfaceState.bDone = false;
  interfaceState.bException = false;
  interfaceState.bError = false;
  interfaceState.bDataDelay = false;
}


/* ======================================================================
FUNCTION
  AudioPlayer::InterfaceStateUpdate

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
void AudioPlayer::InterfaceStateUpdate()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::InterfaceStateUpdate");

  //wakeup the thread unless it's idle.
  if ( state!=IDLE )
  {
    //Do an immediate dispatch. Essentially wakeup the audio player.
    SelfDispatch(0);
}
}

/* ======================================================================
FUNCTION
  AudioPlayer::SendCommand

DESCRIPTION
//
// send a command, wait on it to be consumed, return TRUE if
// it was consumed, false if we timed out.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::SendCommand(Command newCommand, unsigned long)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::SendCommand");

  //clear any old abort signal that has been processed.
  if ( bAbort && state==IDLE )
  {
    bAbort=false;
  }

  //set the global command variable.
  command = newCommand;

  ProcessCommands();

  //cleanup
  command = NONE;
  return true;
}


/* ======================================================================
FUNCTION
  AudioPlayer::TimingUpdate

DESCRIPTION
//
// notify av player that the audio thread just did a timing update.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::TimingUpdate()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::TimingUpdate");

  // If playing is not yet stable then flip it
  if( (!m_bPlayingStable) && (state == PLAYING) )
  {
    QTV_MSG( QTVDIAG_AUDIO_TASK, "TimingUpdate: Playing Stable is true");
    m_bPlayingStable = true;
  }

  if ( m_playerCB )
    {
		AUDIO_TIMING_type *pEvent = QCCreateMessage(AUDIO_TIMING, m_pClientData);/*lint !e641 */

        if ( pEvent )
        {
           pEvent->hdr.user_data = (void *)m_pClientData;
           pEvent->playbackID = currentPlaybackID;
           //Post the event using the client callback..
          (m_playerCB)(pEvent, m_pClientData);
      }
    }
  }

/* ======================================================================
FUNCTION
  AudioPlayer::ShowState

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
void AudioPlayer::ShowState(State input)
{
  switch ( input )
  {
  case BUFFERING:
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio state BUFFERING %d",ZUtils::Clock());
    break;
  case REBUFFERING:
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio state REBUFFERING %d",ZUtils::Clock());
    break;
  case PLAYING:
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "Audio state PLAYING, %d ID %d",ZUtils::Clock(),currentPlaybackID);
    break;
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  case PAUSED:
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio state PAUSED %d",ZUtils::Clock());
    break;
  case SUSPENDED:
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio state SUSPENDED %d",ZUtils::Clock());
    break;
  case IDLE:
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio state IDLE %d",ZUtils::Clock());
    break;
  case PAUSED_DISABLED:
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio state PAUSED_DISABLED %d",ZUtils::Clock());
    break;
  case BUFFERING_DONE:
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio state BUFFERING_DONE %d",ZUtils::Clock());
    break;
  }
}

/* ======================================================================
FUNCTION
  AudioPlayer::SetState

DESCRIPTION
//
//  State transition.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::SetState(State newState)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::SetState");

  if ( state!=newState )
  {
    // For any state transitions reset the syncing flag safely
    // This member is local to the particular state.
    // This will make sure that this member is initialized properly
    // while entering to playing/buffering/re-buffering states.
    m_bSyncing = false;
    ShowState(newState);
    if(newState == PLAYING)
    {
      QTV_MSG( QTVDIAG_AUDIO_TASK, "SetState: Playing Stable is false");
      m_bPlayingStable = false;
    }
  }

  //always cleanup when idle, even if we didn't make it out of idle.
  if ( newState==IDLE || newState == PAUSED_DISABLED )
  {
    CleanupPlay();
  }

  // Go to new state.
  lastState=state;
  state=newState;
}



/* ======================================================================
FUNCTION
  AudioPlayer::Notify

DESCRIPTION
//
//  notify player thread of updated status and
//  do a state transition.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::Notify(State newState,
                         Common::AudioStatusCode status)
{
  QTV_MSG2( QTVDIAG_AUDIO_TASK, "AudioPlayer::Notify(%d,%d)", newState, status);

  //the order is important.  first save the playback id,
  //then change state, then send the notification.

  int playbackID = currentPlaybackID;
  int nAudioFrames = 0;
  if( audioMgr )
  {
    nAudioFrames = audioMgr->GetFrame() ;
  }

  SetState(newState);

  //Notify player.
  if ( m_playerCB )
    {
      AUDIO_STATUS_type *pEvent = QCCreateMessage(AUDIO_STATUS, m_pClientData);/*lint !e641 */
      if ( pEvent )
      {
        pEvent->playbackID = playbackID;
        pEvent->status = status;
        pEvent->nAudioFrames = nAudioFrames;
        pEvent->durationBuffered = durationBuffered;

      if((REBUFFERING == newState) && m_bHasVideo)
        {
         uint32 videoDurationBuffered = 0;

         if (pMpeg4 && (pMpeg4->GetSource()==Media::STREAM_SOURCE))
         {
            videoDurationBuffered   = pMpeg4->GetVideoDurationAvailable();
            pEvent->durationBuffered=
                              MIN(videoDurationBuffered,durationBuffered);
         }
        }

      //Post the event using the client callback..
      (m_playerCB)(pEvent, m_pClientData);
    }
  }
}
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION
  AudioPlayer::NotifySPec

DESCRIPTION

  notify player thread of updated status

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::NotifySPec()
{
  //Notify player.
  if ( m_playerCB )
    {
      AUDIO_SPEC_type *pEvent = QCCreateMessage(AUDIO_SPEC, m_pClientData);/*lint !e641 */
      if ( pEvent )
      {
        pEvent->bitRate = (uint32)audioMgr->audioInfo.nBitRate;
      }
     //Post the event using the client callback..
      (m_playerCB)(pEvent, m_pClientData);
  }
}
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

/* ======================================================================
FUNCTION
  AudioPlayer::NotifyTrackStateChange

DESCRIPTION

  notify track state change to the player thread

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void AudioPlayer::NotifyTrackStateChange()
{
  if ( m_playerCB )
  {
    AUDIO_STATUS_type *pEvent = QCCreateMessage(AUDIO_STATUS, m_pClientData);/*lint !e641 */

    if ( pEvent )
    {
      pEvent->hdr.user_data = (void *)m_pClientData;
      pEvent->playbackID       = currentPlaybackID;
      pEvent->status           = Common::AUDIO_TRACK_STATE_CHANGE_NOTICE;
      pEvent->nAudioFrames     = 0;
      if( audioMgr )
      {
        pEvent->nAudioFrames = audioMgr->GetFrame() ;
      }
      pEvent->durationBuffered = durationBuffered;
      (m_playerCB)(pEvent, m_pClientData);
    }
  }
  return;
}

/* ======================================================================
FUNCTION
  AudioPlayer::ProcessInterfaceState

DESCRIPTION
// process interface state updates.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::ProcessInterfaceState()
{
  switch ( state )
  {
  case IDLE:
  case BUFFERING:
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  case PAUSED_DISABLED:
    //ignore
    break;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  case POLLING_DOWNLOAD:
		//ignore
  break;
#endif


  case SUSPENDED:
  case PLAYING:
  case REBUFFERING:
  case PAUSED:
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
    //
    // check the status, in priority order
    //
    if ( interfaceState.bException )
    {
      //CALLBACK EXCEPTION
#ifndef T_MSM7500
      // Try to restart
      if (state == PLAYING)
      {
         abortAfterward = true;
         abortStatus = Common::AUDIO_EXCEPTION;
         Notify(state, Common::AUDIO_RESTART);
      }
      else
#endif
      {
         AbortAudioOutput();
         Notify(IDLE,Common::AUDIO_EXCEPTION);
      }
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "audio Callback exception");
      return;
    }
    else if ( interfaceState.bDone )
    {
      //CALLBACK AUDIO DONE
      //audio is done playing.
      //This is the normal ending when a byte count is
      //provided up front.
      if( audioMgr )
      {
        audioMgr->UpdateAudioEndStats(interfaceState.tDone);
      }
      bPlayDoneRendering=true;
      Notify(IDLE, Common::AUDIO_DONE);
      return;
    }
    else if ( interfaceState.bError )
    {
      //CALLBACK ERROR
      //Error during callback processing.
      AbortAudioOutput();
      Notify(IDLE,Common::AUDIO_ERROR_ABORT);
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Audio Callback Error");
      return;
    }
    else if ( interfaceState.bDataDelay )
    {
      //CALLBACK DATA DELAY.
      if ( state==PLAYING && bPlayEnding )
      {
        //audio is starved for data, and we're at the end of audio.
        //This is the normal ending when a byte count is not
        //provided up front.
        if( audioMgr )
        {
          audioMgr->UpdateAudioEndStats(interfaceState.tDataDelay);
        }
        StopAudioOutput();

        //CMX continus sending high frequent DELAY event after audio is done.
        // So ignore those event after play is done
        if (!bPlayDoneRendering)
        {
          bPlayDoneRendering=true;
          Notify(IDLE, Common::AUDIO_DONE);
        }
        return;
      }
      #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
      #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
      else
      {
#ifndef FEATURE_QTV_GENERIC_BCAST
        if (pMpeg4 &&
            (Media::BCAST_FLO_SOURCE == pMpeg4->GetSource() ||
            Media::BCAST_ISDB_SOURCE == pMpeg4->GetSource() ))
#else
        /* For all Bcast Media sources audio codec hardware data delay
         * treated as true codec underrun. Even for file based play
         * this is applicable.
         */
        if (is_GenericBcastMedia(pMpeg4))
#endif
        {
          /*-----------------------------------------------------------------
            For Bcast FLO, audio underruns don't ever get signalled by the
            media.  As such, the only TRUE underrun is when CMX detects that
            there has been a HW codec underrun.  They signal this to us by
            giving us this "data delay" flag.

            In response to a HW codec underrun, we'll pause audio and fake
            a media underrun and go to rebuffering.
          -----------------------------------------------------------------*/
          if (PLAYING == state)
          {
            interfaceState.bDataDelay = false;

            if (pMpeg4->IsAudioAvailable() || (!m_bPlayingStable))
            {
              /*-------------------------------------------------------------
                CMX underrun, but we're processing the message when its no
                longer relevent.  As such, ignore it.
              -------------------------------------------------------------*/
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                      "CMX underrun signalled, but we have data.  Ignoring");
            }
            else
            {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                         "CMX is starved, declaring underrun");
              durationBuffered = GetAudioDurationAvailable();
              Notify(REBUFFERING, Common::AUDIO_BUFFERING);
              audioDataState = Media::DATA_UNDERRUN; //hd
            }

          }
        }
        return;
      }
    }
    break;
  }
}

/* ======================================================================
FUNCTION
  AudioPlayer::PauseAudioOutput

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
bool AudioPlayer::PauseAudioOutput()
{
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  #ifndef FEATURE_QTV_AUDIO_DISCONTINUITY
  if ( (state==PLAYING || state == BUFFERING || state == REBUFFERING || state == BUFFERING_DONE ) && !bPlayDoneRendering )
  #else  /* FEATURE_QTV_AUDIO_DISCONTINUITY */
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  {
    if( audioMgr )
    {
      return audioMgr->Pause();
    }
	return false;
  }
  return false;
}

/* ======================================================================
FUNCTION
  AudioPlayer::ResumeAudioOutput

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
bool AudioPlayer::ResumeAudioOutput(bool bRestart)
{
  if ( bPlayDoneRendering )
  {
    return true;
  }
  else
  {
    if( audioMgr )
    {
      return audioMgr->Resume(bRestart);
    }
  }
  return false;
}


#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */

/* ======================================================================
FUNCTION
  AudioPlayer::StopAudioOutput

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
void AudioPlayer::StopAudioOutput()
{
  //this can be called anytime so check the state.
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  switch ( state )
  {
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  case PLAYING:
  case REBUFFERING:
  case PAUSED:
    if ( !bPlayDoneRendering )
    {
      if( audioMgr )
      {
        (void)audioMgr->Stop();
      }
    }
    break;
  }
}

/* ======================================================================
FUNCTION
  AudioPlayer::AbortAudioOutput

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
void AudioPlayer::AbortAudioOutput(bool bError)
{
  if( audioMgr )
  {
    audioMgr->Abort();
  }

  if (pMpeg4 != NULL)
  {
     pMpeg4->AbortAudio(bError);
     AudioAborted = true;
  }
  /* we have done with DATA_ERROR, set it to END */
  audioDataState = Media::DATA_END;
  if( audioMgr )
  {
    audioMgr->SetMediaStatus(audioDataState);
  }
}


/* ======================================================================
FUNCTION
  AudioPlayer::CanPause

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
bool AudioPlayer::CanPause()
{
  return (state!=IDLE && state!=SUSPENDED);
}

/* ======================================================================
FUNCTION
  AudioPlayer::CanSuspend

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
bool AudioPlayer::CanSuspend()
{
 #ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  return(state!=IDLE && state!=POLLING_DOWNLOAD
         /* If PAUSED_DISABLED then audio is already stopped.
          * No need to do it again.
          */
         && state!=PAUSED_DISABLED
        );
 #else
  /* If PAUSED_DISABLED then audio is already stopped.
   * No need to do it again.
   */
  return(state!=IDLE && state!=PAUSED_DISABLED);
 #endif  /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

}

/* ======================================================================
FUNCTION
  AudioPlayer::CanStart

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
bool AudioPlayer::CanStart(bool bRestart)
{
  return((state==IDLE && !bRestart)
         || (state==PAUSED_DISABLED && !bRestart)
         || (state==PAUSED && bRestart)
         || (state==SUSPENDED));
}

/* ======================================================================
FUNCTION
  AudioPlayer::CanResume

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
bool AudioPlayer::CanResume()
{
  return ((state == PAUSED || state == SUSPENDED) &&
          (lastState == BUFFERING || lastState == PLAYING ||
           lastState == REBUFFERING || lastState == PAUSED ||
#ifndef FEATURE_QTV_AUDIO_DISCONTINUITY
           lastState == BUFFERING_DONE));
#else  /* FEATURE_QTV_AUDIO_DISCONTINUITY */
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
}

/* ======================================================================
FUNCTION
  AudioPlayer::ProcessCommands

DESCRIPTION
//
// Process commands from AV player or data callback
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::ProcessCommands()
{
  // Catch the abort signal.  this takes priority over
  // commands.
  if ( bAbort )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "audio Abort signal detected");
    AbortAudioOutput();
    if ( state!=IDLE )
    {
      Notify(IDLE,Common::AUDIO_ERROR_ABORT);
    }
    return ;
  }

  // Process commands from the AV player.
  switch ( command )
  {
  case STOP:
    StopAudioOutput();
    if ( state!=IDLE )
    {
      Notify(IDLE,Common::AUDIO_STOPPED);
    }
    break;

  case SUSPEND:
    if ( CanSuspend() )
    {
      StopAudioOutput();
      if ( state!=SUSPENDED )
      {
        Notify(SUSPENDED,Common::AUDIO_SUSPENDED);
      }
    }
    break;

  case PAUSE:
    if ( CanPause() )
    {
      if ( state!=PAUSED )
      {
         if( (state == BUFFERING) || (state == BUFFERING_DONE) )
         {
          /* we don't have to pause CMX, as we have not started CMX yet */
            Notify(PAUSED,Common::AUDIO_PAUSED);
         }
         else if( PauseAudioOutput())
      {
        Notify(PAUSED,Common::AUDIO_PAUSED);
      }
      }
    }
    break;

  case DISABLE_AUDIO:
    StopAudioOutput();
    if ( state!=PAUSED_DISABLED )
    {
      Notify(PAUSED_DISABLED,Common::AUDIO_PAUSED_DISABLED);
    }
    break;

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */

  case PLAY:
    switch ( state )
    {
    case IDLE:
    case SUSPENDED:
    case PAUSED_DISABLED:
      durationBuffered = GetAudioDurationAvailable();
      #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
      #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
      //start buffering.
      Notify(BUFFERING,Common::AUDIO_BUFFERING);
      break;

    case PAUSED:
      if ( CanResume() )
      {
        switch ( lastState )
        {
        case BUFFERING:
        #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
        #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
        case REBUFFERING:
          durationBuffered = GetAudioDurationAvailable();
          #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
          #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
          //resume former state
          Notify(lastState,Common::AUDIO_BUFFERING);
          break;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
        case POLLING_DOWNLOAD:
		  //resume former state
		  Notify(lastState,Common::AUDIO_DOWNLOAD_POLLING_START);
		  break;
#endif

        case BUFFERING_DONE:
        // We were waiting for the player to tell us to restart audio output
        // when we were paused. If the player now tells us to play, notify
        // it so it can restart audio again, and go back to the BUFFERING_DONE
        // state.
        Notify(BUFFERING_DONE, Common::AUDIO_BUFFERING_END);
        break;

        case PLAYING:
          //resume playing
          if ( ResumeAudioOutput(false) )
          {
            SetState(PLAYING);
            bPlayingNotifyPending = TRUE;
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Cannot resume audio");
            Notify(IDLE,Common::AUDIO_ERROR_ABORT);
          }
          break;
        #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
          #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
        }
      }
      break;
    }
    break;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

    case POLL_DOWNLOAD:
		if (CanPollDownload())
		{
			Notify(POLLING_DOWNLOAD,Common::AUDIO_DOWNLOAD_POLLING_START);
		}
		break;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

    case RESTART_OUTPUT:
        (void)RestartAudioOutput();
        break;

  }
}

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
bool AudioPlayer::WaitForDownloadFileHeader()
{
    return false;
}

#endif


/* ======================================================================
FUNCTION
  AudioPlayer::IsDataAvailable

DESCRIPTION
//
// this is a polling function which returns true when
// audio data is available.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::IsDataAvailable()
{
#ifndef FEATURE_QTV_ADTS_PARSER
  return (pMpeg4?(pMpeg4->IsAudioAvailable()):false);
#else
#error code not present
#endif

}

/* ======================================================================
FUNCTION
  AudioPlayer::GetAudioDurationAvailable

DESCRIPTION
 Returns the duration of buffered audio in msec

DEPENDENCIES
 None

RETURN VALUE
  duration of buffered audio in msec

SIDE EFFECTS
 None

========================================================================== */
uint32 AudioPlayer::GetAudioDurationAvailable()
{
  return pMpeg4 ? pMpeg4->GetAudioDurationAvailable() : 0;
}


/* ======================================================================
FUNCTION:
  AudioPlayer::TrackHasEnded

DESCRIPTION:
  if audio track has ended.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if audio track has ended or false, if not.

SIDE EFFECTS:
  None.
======================================================================*/
bool AudioPlayer::TrackHasEnded()
{
  /*
  * If audio is paused after audio track has ended, audio 'state' won't be IDLE,
  * So, check for bPlayDoneRendering. It gets set to true when AUDIO_DONE is encountered.
  */
  if( ((audioDataState==Media::DATA_END)||(audioDataState==Media::DATA_ERROR)) && ((state==IDLE)||(bPlayDoneRendering)) )
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
  AudioPlayer::RestartAudioOutput()

DESCRIPTION:
  Restart audio output and move to the PLAYING state.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if restarting was successful, otherwise false.

SIDE EFFECTS:
  Aborts audio output if restarting was not successful.
======================================================================*/
bool AudioPlayer::RestartAudioOutput()
{
  QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "RestartAudioOutput");



  // Start or resume
  bool bOK = false;
  switch ( state )
  {

  case BUFFERING:
  case BUFFERING_DONE:
    if( audioMgr )
    {
      if(Media::DATA_END != audioMgr->GetMediaStatus())
      {
        //DataIsAvailable will change the status to DATA_OK
        //Do not change the state if data state is DATA_END
        //to end the audio properly.
        audioMgr->DataIsAvailable();
      }
    }

    if ( bLocalRestart && (m_bCMXInterfaceStartDone == TRUE) )
    {
      // Flush and Restart Playing
      // Only Flush upon reposition so Pause/Play does not play back choppy
      if( audioMgr )
      {
        (void)audioMgr->FlushDataRequests();
      }
      bOK = ResumeAudioOutput(bLocalRestart);
      bLocalRestart = false;
    }
    else
    {
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
      if( audioMgr )
      {
        bOK = audioMgr->Start();
      }
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

      if(bOK)
	  {
	    m_bCMXInterfaceStartDone = TRUE;
	    QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,"audioMgr::start returned TRUE, setting m_bCMXInterfaceStartDone to TRUE");
	  }
    }
    break;

  case REBUFFERING:
    if( audioMgr )
    {
      audioMgr->DataIsAvailable();
    }
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (Media::BCAST_FLO_SOURCE == pMpeg4->GetSource() ||
        Media::BCAST_ISDB_SOURCE == pMpeg4->GetSource() )
#else
    if (is_GenericBcastMedia(pMpeg4))
#endif
    {
      /*-------------------------------------------------------------
        For BCAST FLO, we don't ever want to stop time
      -------------------------------------------------------------*/
      bOK = true;
      #if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
       if( audioMgr )
       {
         #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
         #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
         {
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif // FEATURE_QTV_GENERIC_BCAST_PCR
       }
       }
      #endif
      if( audioMgr )
      {
        audioMgr->ResyncAudioTime();
      }
      //bOK = ResumeAudioOutput(false);
    }
    else
    {
      bOK = ResumeAudioOutput(false);
    }
    break;

    default:
    QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR,
      "Restart Audio Output, weird state %d", state);
    break;
  }
  /*
  * When trying to start/resume the audio, check the Media DATA_STATE.
  * If DATA_END, fake as if RESUME is successful.
  * This will help to end the audio when we attemp to pull the audio sample.
  */
  if( (!bOK) && audioMgr && (Media::DATA_END == audioMgr->GetMediaStatus()) )
  {
    //If Start/Resume failed but media status is DATA_END, fake that audio resumed/started.
    //This will make audio player to detect DATA_END and end the playback gracefully.
    bOK = true;
    /* This is to set the end delay returning in PredictEndDelay to 500ms
       to avoid avoid any race conditions if we hit this case*/
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    audioMgr->SetEndDelay();
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
    QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR,
    "RestartAudioOutput detected Media::DATA_END");
  }

  // Check results.
  if ( bOK )
  {
    //buffering has ended, we're playing.
    SetState(PLAYING);
    bPlayingNotifyPending = TRUE;
  }
  else
  {
    //failed to start or resume.
#if( defined T_MSM7500 || defined T_MSM7200 || defined T_MSM7600 )

    AbortAudioOutput();
    Notify(IDLE, Common::AUDIO_ERROR_ABORT);

#else
	// Try to restart
    abortAfterward = true;
    abortStatus = Common::AUDIO_ERROR_ABORT;
    Notify(state, Common::AUDIO_RESTART);
#endif
    QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR, "audio Failed to start or resume");
  }

  return bOK;
}

/* ======================================================================
FUNCTION
  AudioPlayer::ProcessStateTransition

DESCRIPTION
//
//  internal audio thread routine to process the state transitions.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::ProcessStateTransition(void)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::ProcessStateTransition()");

  while ( true )
  {
    //Process commands
    ProcessInterfaceState();

    // elapsed time check for mpeg4player
    if (checkTimeFlag)
    {
       if (GetElapsedTime() >= (long)checkTimeValue)
       {
          Notify(state, Common::AUDIO_ELAPSE_TIME);
          checkTimeFlag = false;
       }
    }

    switch ( state )
    {

    case IDLE:
    case PAUSED:
    case SUSPENDED:
    case PAUSED_DISABLED:
      //
      // Wait for commands..
      //

      #ifdef FEATURE_QTV_DRM_DCF
	    if( (m_bIsAudioTaskRegisteredForIPC == true) && (state == IDLE) )
	    {
	      //when done, de-register with ixipc.
 	  	  ixipc_terminate();
  	  	m_bIsAudioTaskRegisteredForIPC = false;
	  	}
      #endif

      m_nWaitMsec = COMMON_IDLE_WAIT_TIMEOUT_MSEC;
      break;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2

     case POLLING_DOWNLOAD:
	//wait for data to download
	 if (WaitForDownloadFileHeader())
	 {
             m_nWaitMsec = COMMON_DOWNLOAD_POLLING_INTERVAL_MSEC;
	 }
	 else
	 {
		Notify(IDLE,Common::AUDIO_DOWNLOAD_POLLING_END);
	 }
 	 break;

#endif
    #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
    #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

    case BUFFERING:
    case REBUFFERING:
      if ( bPlayDoneRendering && state==REBUFFERING )
      {
        //We'll get here if the device reports "Done Rendering"
        //while rebuffering.  If so just go to the Playing state to
        //await the Stop command.
        SetState(PLAYING);
        bPlayingNotifyPending = TRUE;
      }
      else
      {
        //
        // Normal buffering state.
        if(audioDataState == Media::DATA_ERROR) /* check if we encounter data error */
        {
          QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR, "Audio Data Error");
          AbortAudioOutput();
          Notify(IDLE, Common::AUDIO_ERROR_ABORT);
        }
        else if ( !IsDataAvailable() ) // Poll for data.
        {
          static uint8 count = 0;
          if (++count % 10 == 0)
          {
            durationBuffered = GetAudioDurationAvailable();
            // Once we are in the rebuffering state we should
            // stay in that state no matter what the audiodatastate
            // Similar thing holds good for buffering also.
            // In live broadcast we will be switching between different
            // states of data availability. In those scenario state
            // should remain the same.

            if(REBUFFERING == state)
            {
              Notify(REBUFFERING,Common::AUDIO_BUFFERING);
            }
            else
            {
              Notify(BUFFERING,Common::AUDIO_BUFFERING);
            }
          }
          m_nWaitMsec = COMMON_BUFFERING_POLL_TIME_MSEC;
        }
        else
        {
          // Data is available.

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#else
          if (state == BUFFERING)
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
          {
            #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
            #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
            // Notify the player which will in turn restart audio when it's
            // ready.
            Notify(BUFFERING_DONE, Common::AUDIO_BUFFERING_END);
          }
          else if (state == REBUFFERING)
          {
            // No need to wait for player to restart us.
            (void)RestartAudioOutput();
          }
          else
          {
            QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR,
              "Unknown state %d to transition from when buffering ended!",
              state);
          }
        }
      }
      break;

    case BUFFERING_DONE:
      // Just wait until the player tells us to restart audio.
      m_nWaitMsec = COMMON_BUFFERING_POLL_TIME_MSEC;
      break;

    case PLAYING:
      if ( bPlayDoneRendering )
      {
        //Audio is done, just wait for a Stop command from the player.
        m_nWaitMsec = COMMON_IDLE_WAIT_TIMEOUT_MSEC;
        QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR,
                     "Audio is done, just waiting for a Stop command from the player!");
      }
      else if ( bPlayEnding )
      {
        //
        // We've reached the end of the data, but want to give audio time to finish
        // playing the data that it has buffered. note: this will catch
        // cases where audio does not report "done" or "data delay" at the
        // end of playback.
        //
        if ( audioTime.tEndDelay>0 )
        {
          //wait for interval to expire
          m_nWaitMsec = audioTime.tEndDelay;
          audioTime.tEndDelay = 0;
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Audio Waiting to finish in %dms",m_nWaitMsec);
          if (bPlayingNotifyPending)
           {
             if (state == PLAYING)
               {
                  Notify(PLAYING, Common::AUDIO_PLAYING);
               }
             bPlayingNotifyPending = FALSE;
           }
       }
        else
        {
          StopAudioOutput();
          bPlayDoneRendering=true;
          Notify(IDLE, Common::AUDIO_DONE);
        }
      }
      // Intelligent re-sync case when play mode swtich from buffered to live
      else if (IsResyncReqd())
      {
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
          if( audioMgr && !IsPTSSyncd(audioMgr->GetAmtBufferedMsec(ZUtils::Clock())) )
          {
            /* we are transitioning from TSB to live and there is data loss/discontinuety */
            /* Force audio player to re-buffering state. Any way smoother transition from TSB to live is not possible in this scenario. */

            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Audio Re-buffering when transition from TSB to Live ");
          m_nWaitMsec = COMMON_BUFFERING_POLL_TIME_MSEC;
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
          }
          else
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Audio Smooth transition from TSB to Live ");
          }

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

      }

      else
      {
        //
        // Normal play state.  Send data.
        //

        long cmxDataLeadEstimation = 0;
        audioDataState = Media::DATA_OK;

       bool bError = false;
       long nWaitMsec = 0;
        if( audioMgr )
        {
          #ifndef FEATURE_QTV_AUDIO_DISCONTINUITY
          if ( !audioMgr->PlaySomeAudio(nWaitMsec,bError,audioDataState, cmxDataLeadEstimation) )
          #else  /* FEATURE_QTV_AUDIO_DISCONTINUITY */
#error code not present
          #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
          {
            // If there is no error then make sure that codec parameters have not changed on the fly
            if(!bError && audioMgr->IsCodecParamChanged())
            {
              HandleDynamicCodecParamChange(bError);
            }


            // Didn't send data, check the reason.
            if ( bError )
            {
              //An error occurred, just abort.
              AbortAudioOutput();
              Notify(IDLE, Common::AUDIO_ERROR_ABORT);
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                         "audio error in PlaySomeAudio");
            }
            //otherwise check the data state to see what to do...
            else
            {
              switch ( audioDataState )
              {
              case Media::DATA_END:
                if(audioMgr->IsDataBeingRestored())
                {
                  audioDataState = Media::DATA_OK;
                  break;
                }
                //we're out of data,
                // inform Mpeg4Player (no need to change audio player state) only once
                if( (bPlayEnding==false) && (bPlayEndingMustBeSet==false) )
                {
                  Notify(state, Common::AUDIO_MEDIA_END);
                }
                //wait for audio to finish playing what it has.
                audioMgr->PredictEndDelay(audioTime.tEndDelay);
                QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio is ENDING in %d msec", audioTime.tEndDelay);
                /* If we were already in a Data Delay state, then it is not the right time to
                 * to conclude that the clip is ending. It is appropriate to set this flag
                 * bPlayEnding when we get bDataDelay after we hit the DATA_END - not before.
                 * Hence just remember that we are at the end by setting bPlayEndingMustBeSet
                 * When it has sent all the audio data to CMX, it should wait for the CMX_DATA_ACCESS_DELAY
                 * callback from CMX. (This indicates that CMX has finished playing all data _and_ that the
                 * DSP has also run out of data).  Once the CMX_DATA_ACCESS_DELAY is received, just send the
                 * stop command playback using cmx_audio_playback_control(CMX_AUDIO_PB_CMD_STOP, ..,..........);
                 */
                if(! interfaceState.bDataDelay)
                   bPlayEnding=true;
                else
                {
                  interfaceState.bDataDelay = FALSE;
                  bPlayEndingMustBeSet = true;
                }

		//This is required in case Audio track is shorter than the video track
                //ans seek location is greater than the audio track length AV sync offset 
                // is wrongly set based on the last audio time stamps because of which video 
                // frames start sleeping if
                //if Audio media end occurs and we reset the Video AVsync offset it will enable 
                //video to set the AVsync offset and video frames will display without freeze
                if(m_bHasVideo)
                {
                  m_pAVSync->ProhibitAVSync(true);    
                  m_pAVSync->ResetPlaybackOffset(AVSync::VideoAV);
                }
                break;

              case Media::DATA_UNDERRUN:
                //pause audio and goto rebuffering.
#ifndef FEATURE_QTV_GENERIC_BCAST
                if (Media::BCAST_FLO_SOURCE != pMpeg4->GetSource() &&
                    Media::BCAST_ISDB_SOURCE != pMpeg4->GetSource())
#else
                if (!is_GenericBcastMedia(pMpeg4))
#endif
                  /*-----------------------------------------------------------
                    For BCAST FLO, we don't ever want to stop time
                  -----------------------------------------------------------*/
                {
                  PauseAudioOutput();
                }

                durationBuffered = GetAudioDurationAvailable();
                Notify(REBUFFERING, Common::AUDIO_BUFFERING);
                break;

              case Media::DATA_SKIP:
                //As audio is not really in under-run state, notify that data is actually
                //   available, such that player will ask for data again after poll-period
                m_nWaitMsec = REQUEST_POLL_TIME_MSEC;
                audioMgr->DataIsAvailable();
                break;

              case Media::DATA_ERROR:
                QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Data Error");
                AbortAudioOutput();
                Notify(IDLE, Common::AUDIO_ERROR_ABORT);
                break;

              case Media::DATA_OK:
#ifndef T_MSM7500
#ifndef FEATURE_QTV_GENERIC_BCAST
                if (Media::BCAST_FLO_SOURCE != pMpeg4->GetSource())
#else
                if (IsFileBcastMediaSource(pMpeg4,m_pAVSync))
#endif
                {
                    /* Reverting back STARVING_TIME to 10s */
                    #define MAX_STARVING_TIME 10000
                   // we should send, but did not. Either there is no CMX request or
                  //    we are not below low-watermark
                  if (cmxDataLeadEstimation < 0)
                  {
                     // there is no CMX request
                     if ((starvingTime - cmxDataLeadEstimation) > MAX_STARVING_TIME)
                     {
                        // request to restart audio if already starved for more than 2sec.
                        abortAfterward = false;
                        Notify(state, Common::AUDIO_RESTART);
                        //
                        starvingTime = cmxDataLeadEstimation;
                        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                                     "Reach max starving time, restart audio");
                     }
                  }
                }
#endif
                break;

              case Media::DATA_FRAGMENT:
                audioMgr->DataIsAvailable();
                m_nWaitMsec = COMMON_FRAGMENT_WAIT_TIMEOUT_MSEC;
                break;

              case Media::DATA_INSUFFICIENT:
                audioMgr->DataIsAvailable();
                m_nWaitMsec = COMMON_DATA_INSUFFICIENT_WAIT_TIMEOUT_MSEC;
                break;

              default:
                QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unexpected data state %d",audioDataState);
                AbortAudioOutput();
                Notify(IDLE, Common::AUDIO_ERROR_ABORT);
                break;
              }
            }
            if ( nWaitMsec>0 && !bPlayEnding )
            {
              //The audio interface wants us to wait a bit.
              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Info: Audio Waiting to poll %dms",nWaitMsec);
              m_nWaitMsec = nWaitMsec;
              //keep stats on wait time.
              audioTime.nWaitMsec+=nWaitMsec;
            }
          }
        }
        if (bPlayingNotifyPending)
        {
          if (state == PLAYING)
          {
            Notify(PLAYING, Common::AUDIO_PLAYING);
          }
          bPlayingNotifyPending = FALSE;
        }
        if (cmxDataLeadEstimation >= 0)
        {
           // Not starving, reset starvingTime
           starvingTime = 0;
        }
      }
      break;
    }

    //Check for client commands
    if( m_numAudioDispatchesPending > 0 )
    {
      //m_msgType better be 0.. checkup in the f3 msg below..!
      m_nWaitMsec = 0;
      break;
    }
    else if(m_nWaitMsec)
    {
      //if nWaitMsec > 0, we'll send a delayed dispatch to ourselves.
      m_msgType = SelfDispatch(m_nWaitMsec);
      //if m_msgType ==0, we ran out of timer resources..!
      //but we still should break and process the pending delayed disp msgs..
      break;
    }
    else
    {
      //Should only have a few number of consecutive occurance..!
      QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::ProcessStateTransition() while loop..!");
    }
  } //while ( true )

  QTV_MSG3( QTVDIAG_AUDIO_TASK, "AudioPlayer::ProcessStateTransition() Done..! waitMsec=%d, state=%d, dispMsgType=%d"
  , m_nWaitMsec, state, m_msgType);

  return;
}

/* ======================================================================
FUNCTION
  AudioPlayer::HandleDynamicCodecParamChange

DESCRIPTION
  This routine handles the critical codec parameter changes during normal playback.
  Basically restart the audio DSP from the scratch.

DEPENDENCIES
  None.

RETURN VALUE
  Error is signalled through the output parameter passed.

SIDE EFFECTS
  None.

========================================================================== */
void AudioPlayer::HandleDynamicCodecParamChange(bool &bError)
{
  // By default assume that everything will go fine ; Dont do the mistake of returning error always
  bError = false;

  if(!audioMgr)
  {
    // This should never happen since this is invoked from the ProcessStateTransition when we are
    // in the playing state.
    QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
                 "AudioMgr is NULL. This is not possible");
    bError = true;
    return;
  }

  if(audioMgr->IsCodecParamChanged())
  {
    // Restart the audio DSP if the sample frequency change detected
    QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                 "Attempting to Restart the Audio DSP");
    #ifndef FEATURE_QTV_CODEC_CHANGE_RESTART
    // Reset the flag before acting ; Dont want to forget this and debug for a day or so
    audioMgr->SetCodecParamChanged(false);
    #endif

    bError = RestartAudioDSP();

    if(bError)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_ERROR,
                   "Audio Restart Failed!!! Audio Will be aborted!!!");
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                   "Audio Restart Succeeded  Thanks to DSP");
    }

  }
  // either no sample freq change or error is set through RestartAudioDSP method
  return ;
}

/* ======================================================================
FUNCTION
  AudioPlayer::AudioPlayer

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
AudioPlayer::AudioPlayer( CLIENT_NOTIFY_CB_FN  playerCB,
                          void*                pClientData,
                          QCConditionType&     responseSync,
                          qtv_task_if_class*   pDispatcher )
: m_responseSync( responseSync ),
  m_numAudioDispatchesPending( 0 )
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::AudioPlayer");

  //init state
  state = IDLE;
  lastState = IDLE;

  audioMgr = NULL;
  m_pClientData = pClientData;
  m_audioCodec = Media::UNKNOWN_CODEC;

  InitDefaults();

  m_playerCB = playerCB;
  m_pDispatcher = pDispatcher;
  m_msgType = 0;
  m_nWaitMsec = 0;
  audioDataState = Media::DATA_OK;

  bAbort=false;
  InitInterfaceState();

#ifdef FEATURE_QTV_DRM_DCF
 //Variable to indicate that IPC/NS registration is not done
 m_bIsAudioTaskRegisteredForIPC = false;
#endif

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  m_dualMonoOutput = Common::DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

  InitDefaults();

  QCUtils::InitCritSect( &m_SelfDispatchCS);
  #ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO
    m_bPlaybackSpeedQueued = false;
    m_ePlaybackSpeed       = Common::PLAYBACK_SPEED_NORMAL;
  #endif
  // Initialize the Intra state member to remember the syncing
  m_bSyncing = false;
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  m_bPlayingStable = false;

}

/* ======================================================================
FUNCTION
  AudioPlayer::~AudioPlayer

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
AudioPlayer::~AudioPlayer()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayer::~AudioPlayer");

#ifdef FEATURE_QTV_DRM_DCF
    //reset variable so that we can register during next playback.
    m_bIsAudioTaskRegisteredForIPC = false;
#endif
    QCUtils::EnterCritSect(&m_SelfDispatchCS);
    if(m_msgType)
    {
      m_pDispatcher->cancel_dispatch(m_msgType);
    }
    QCUtils::LeaveCritSect(&m_SelfDispatchCS);

  QCUtils::DinitCritSect(&m_SelfDispatchCS);

  //not needed, the Destroy routine will be used
}

/* ======================================================================
FUNCTION
  AudioPlayer::EnableTimeCheck

DESCRIPTION
  Enable elapse time check

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioPlayer::EnableTimeCheck( unsigned long timeValue )
{
   checkTimeFlag = true;
   checkTimeValue = timeValue;
}

/* ======================================================================
FUNCTION
  AudioPlayer::RestartAudio

DESCRIPTION
  Restart audio track:
      audioplayer::stop()
      audioplayer::prep()
      audioplayer::start()

DEPENDENCIES
  Error handling by restarting audio track

RETURN VALUE
  None

SIDE EFFECTS
  CMX will be stopped and started immediately with the hope that this will
  bring CMX/DSP back in known state
========================================================================== */
bool AudioPlayer::RestartAudio()
{
   bool bError = false;

   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Request audio to restart: %ld", GetElapsedTime());

   if (!IsDone())
   {
      int playbackID  = currentPlaybackID;
      Media* mediaPtr = pMpeg4;
      // Stop audio
      Stop(bError);

      if (!bError)
      {
         // Audio interface must be reinitialized.
         if (Prep(playbackID,false,mediaPtr))
         {
            //Reset the audio/video sync point
            if(m_pAVSync)
            {
              m_pAVSync->ResetPlaybackOffset(AVSync::VideoAV);
              m_pAVSync->ResetPlaybackOffset(AVSync::AudioAV);
              m_pAVSync->ResetPlaybackOffset(AVSync::TextAV);
            }
            // Start audio
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
            if (Start(bError, false) && !bError)
#endif
            {
               QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "audio successfully restarted!");
               return true;
            }// Start() error
         }// Prep() error
      }// Stop() error
   }// isDone()

   // RestartAudio failed
   if (abortAfterward)
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "audio failed to restart, Abort!");
      AbortAudioOutput();
      Notify(IDLE,abortStatus);
   }
   else
   {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "audio failed to restart");
   }
   return false;
}


/* ======================================================================
FUNCTION
  AudioPlayer::RestartAudioDSP

DESCRIPTION
  Restart audio track:
      audioplayer::stop()
      audioplayer::prep()
      audioplayer::start()

DEPENDENCIES
  Dynamic codec parameter change handling by restarting audio track

RETURN VALUE
  None

SIDE EFFECTS
  CMX will be stopped and started immediately
========================================================================== */
bool AudioPlayer::RestartAudioDSP()
{
   bool bError = false;

   QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "Request audio to restart: %ld", GetElapsedTime());

   if (IsDone())
   {
     // If the audio track is finished then ignore the request to restart the audio DSP
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
	 	          "Request to restart audio track after audio track ended");
     return bError;
   }
   #ifndef T_MSM7500
     // Try to restart
     abortAfterward = true;
     abortStatus = Common::AUDIO_ERROR_ABORT;
     Notify(state, Common::AUDIO_RESTART);
   #else
#error code not present
   #endif
   return bError;
}





/* ======================================================================
FUNCTION:
  AudioPlayer::InitiateRestartAudioOutput()

DESCRIPTION:
  Send the command to restart audio output. This may be called in a
  task other than the audio task.

INPUT/OUTPUT PARAMETERS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
======================================================================*/
void AudioPlayer::InitiateRestartAudioOutput()
{
   (void)SendCommand(RESTART_OUTPUT, COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC);
}

/* ======================================================================
FUNCTION:
  AudioPlayer::dispatch()

DESCRIPTION:
  General demux for all the msgs sent to this message sink.  The subsystem
  has accepted ownership of the message and will delete it when the time is right.

INPUT/OUTPUT PARAMETERS:
  qtv_msg_struct* const msg_ptr
    message/command to be processed

RETURN VALUE:
  None

SIDE EFFECTS:
  None
======================================================================*/
void AudioPlayer::dispatch( qtv_msg_struct* const msg_ptr )
{

    AudioMsgTypes::audio_msg_hdr* pMsg = (AudioMsgTypes::audio_msg_hdr*)
                                          msg_ptr->payload;

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                  "AudioPlayer::dispatch recvd msgId=%d",pMsg->msgId);

    //bError at present is a placeholder. once its removed from audio player api's
    //we can get rid of it..!
    bool bError = false;

    /* The fact that we are now processing a dispatch means there's one
    ** less outstanding. */
    if ( m_numAudioDispatchesPending > 0 )
    {
      --m_numAudioDispatchesPending;
    }
    else
    {
      QTV_MSG_PRIO( QTVDIAG_AUDIO_TASK,
                    QTVDIAG_PRIO_FATAL,
                    "AudioPlayer::dispatch found 0 dispatches pending" );
    }

    //Check for self delayed dispatch msg..
    if( (pMsg->msgId == AudioMsgTypes::AUDIO_PLAYER_SELF_DISPATCH)
        && CheckSelfDelayedDispatch(msg_ptr) )
    {
      m_nWaitMsec = 0;
      m_msgType = 0;
    }
    else
    {
      //Process the client command..
      if(m_msgType)
      {
        //Cancel the delayed dispatch msg..
        if ( m_numAudioDispatchesPending > 0 )
        {
          --m_numAudioDispatchesPending;
        }
        else
        {
          QTV_MSG_PRIO( QTVDIAG_AUDIO_TASK,
                        QTVDIAG_PRIO_FATAL,
                        "AudioPlayer::dispatch found 0 dispatches pending" );
        }
        QCUtils::EnterCritSect(&m_SelfDispatchCS);
        if(m_msgType)
        {
          m_pDispatcher->cancel_dispatch(m_msgType);
        }
        QCUtils::LeaveCritSect(&m_SelfDispatchCS);
        m_nWaitMsec = 0;
        m_msgType = 0;
      }

      if(bPlayEnding && state == PLAYING)
      {
        //command present.  process it before continuing.
        //recalc the end delay since some time may have elapsed.
        if( audioMgr )
        {
          audioMgr->PredictEndDelay(audioTime.tEndDelay);
        }
      }

      switch(pMsg->msgId)
      {
        case AudioMsgTypes::AUDIO_PLAYER_STOP:
        {
          Stop(bError);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_SUSPEND:
        {
          AudioMsgTypes::audio_cmd_suspend_msg* audioSuspend =
            (AudioMsgTypes::audio_cmd_suspend_msg*) msg_ptr->payload;

          *(audioSuspend->pbSuspend) = Suspend(bError);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_START:
        {
          AudioMsgTypes::audio_cmd_start_msg* audioStart =
            (AudioMsgTypes::audio_cmd_start_msg*) msg_ptr->payload;

          *(audioStart->pbStart) = Start(bError,audioStart->bRestart);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_PAUSE:
        {
          AudioMsgTypes::audio_cmd_pause_msg* audioPause=
            (AudioMsgTypes::audio_cmd_pause_msg*) msg_ptr->payload;

          *(audioPause->pbPause) = Pause(bError);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_RESUME:
        {
          AudioMsgTypes::audio_cmd_resume_msg* audioResume =
            (AudioMsgTypes::audio_cmd_resume_msg*) msg_ptr->payload;

          *(audioResume->pbResume) = Resume(bError,audioResume->stop);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_RESTART_AUDIO_OUTPUT:
        {
          AudioMsgTypes::audio_cmd_restart_output_msg* audioRestartOutput =
            (AudioMsgTypes::audio_cmd_restart_output_msg*) msg_ptr->payload;

          *(audioRestartOutput->pbRestartAudioOutput) = RestartAudioOutput();
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_PREP_AND_RESTART_AUDIO_OUTPUT:
        {
          AudioMsgTypes::audio_cmd_prep_and_restart_output_msg* audioPrepAndRestartOutput =
            (AudioMsgTypes::audio_cmd_prep_and_restart_output_msg*) msg_ptr->payload;

          *(audioPrepAndRestartOutput->pbPrepRestartAudioOutput) = RestartAudio();
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_PREP:
        {
          AudioMsgTypes::audio_prep_msg* audioPrep =
            (AudioMsgTypes::audio_prep_msg*) msg_ptr->payload;

          m_audioCodec = audioPrep->audioCodec;
          m_bHasVideo = audioPrep->bHasVideo;
          *(audioPrep->pbPrep) = Prep(audioPrep->playbackID,audioPrep->bRestart,(Media *)audioPrep->pMedia);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_ENABLE_TIME_CHECK:
        {
          AudioMsgTypes::audio_enable_time_check_msg* audioEnableTimeCheck =
                (AudioMsgTypes::audio_enable_time_check_msg*) msg_ptr->payload;
          EnableTimeCheck(audioEnableTimeCheck->timeValue);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_GET_STATS:
        {
          AudioMsgTypes::audio_get_stats_msg* audioGetStats =
            (AudioMsgTypes::audio_get_stats_msg*) msg_ptr->payload;

          GetStatistics(*audioGetStats->pStats,*audioGetStats->pLastFrame);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_GET_ISIDLE:
        {
          AudioMsgTypes::audio_get_isidle_msg* audioGetIsIdle =
            (AudioMsgTypes::audio_get_isidle_msg*) msg_ptr->payload;

          *(audioGetIsIdle->pIsIdle) = IsIdle();
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_GET_ISPAUSED:
        {
          AudioMsgTypes::audio_get_ispaused_msg* audioGetIsPaused =
            (AudioMsgTypes::audio_get_ispaused_msg*) msg_ptr->payload;

          *(audioGetIsPaused->pIsPaused) = IsPaused();
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_GET_ISPAUSED_DISABLED:
        {
          AudioMsgTypes::audio_get_ispaused_disabled_msg* audioGetIsPausedDisabled =
            (AudioMsgTypes::audio_get_ispaused_disabled_msg*) msg_ptr->payload;

          *(audioGetIsPausedDisabled->pIsPausedDisabled) = IsPausedDisabled();
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_GET_ISPLAYING:
        {
          AudioMsgTypes::audio_get_isplaying_msg* audioGetIsPlaying =
            (AudioMsgTypes::audio_get_isplaying_msg*) msg_ptr->payload;

          *(audioGetIsPlaying->pIsPlaying) = IsPlaying();
          break;
        }

        #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
        #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */


        case AudioMsgTypes::AUDIO_PLAYER_GET_TRACK_ENDED:
        {
          AudioMsgTypes::audio_get_track_ended_msg* audioGetTrackEnded =
            (AudioMsgTypes::audio_get_track_ended_msg*) msg_ptr->payload;

          *(audioGetTrackEnded->pIsTrackEnded) = TrackHasEnded();
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_SET_PLAY_TIME:
        {
          AudioMsgTypes::audio_set_play_times_msg* audioSetPlayTime =
            (AudioMsgTypes::audio_set_play_times_msg*) msg_ptr->payload;

          SetPlayTimes(audioSetPlayTime->start, audioSetPlayTime->stop);
          break;
        }

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */

        case AudioMsgTypes::AUDIO_PLAYER_DISABLE:
        {
          AudioMsgTypes::audio_cmd_disable_msg* audioDisable=
            (AudioMsgTypes::audio_cmd_disable_msg*) msg_ptr->payload;

          *(audioDisable->pbDisable) = Disable(bError);
          break;
        }

        case AudioMsgTypes::AUDIO_PLAYER_SET_AUDIO_CHANNELSWITCH_TS:
        {
          AudioMsgTypes::audio_cmd_set_audio_channelswitch_msg* audioSetChSwitch =
            (AudioMsgTypes::audio_cmd_set_audio_channelswitch_msg*) msg_ptr->payload;

          SetAudioChannelSwitchTS(audioSetChSwitch->reqData);
          break;

        }

#ifdef AUDIO_SHOW_INFO
        case AudioMsgTypes::AUDIO_PLAYER_SHOW_INFO:
        {
          ShowInfo();
          break;
        }
#endif //AUDIO_SHOW_INFO

        case AudioMsgTypes::AUDIO_PLAYER_SELF_DISPATCH:
          //This is the one sent by AudioPlayer::InterfaceStateUpdate
          break;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

        default:
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AudioPlayer::dispatch: Unknown msg type");
        }
      }
    }

    if((pMsg->msgId != AudioMsgTypes::AUDIO_PLAYER_SELF_DISPATCH) &&
       (pMsg->msgId != AudioMsgTypes::AUDIO_PLAYER_CMX_FEEDBACK))
    {
      //Wakeup the client thread
      QCUtils::SetCondition(&m_responseSync);
    }
    #ifdef FEATURE_QTV_CODEC_CHANGE_RESTART
#error code not present
    #endif
    //Delete the msg
    qtv_msg_struct::free(msg_ptr);

    #ifdef FEATURE_QTV_CODEC_CHANGE_RESTART
#error code not present
    #endif
    {
    //Continue processing the state transition..
    ProcessStateTransition();
    }

    return;
}

/* ======================================================================
FUNCTION:
  AudioPlayer::CheckSelfDelayedDispatch()

DESCRIPTION:
  Used to check if the recvd delayed dispatch msg was sent
  by this particular audio player inst.

INPUT/OUTPUT PARAMETERS:
  qtv_msg_struct* const msg_ptr
    message/command to be processed

RETURN VALUE:
  bool
    True if above description is satisfied.!

SIDE EFFECTS:
  None
======================================================================*/
bool AudioPlayer::CheckSelfDelayedDispatch( qtv_msg_struct* const msg_ptr )
{
  AudioMsgTypes::audio_cmd_self_dispatch_msg* audioDispatchMsg =
    (AudioMsgTypes::audio_cmd_self_dispatch_msg*) msg_ptr->payload;

  bool bSelfDelayedDispatch = false;
  if( audioDispatchMsg->delay_ms )
  {
    bSelfDelayedDispatch = true;
  }
  //audioDispatchMsg->delay_ms == 0 implies that it was sent via AudioPlayer::InterfaceStateUpdate
  //it shoudl not be the case that m_nWaitMsec == 0 and audioDispatchMsg->delay_ms != 0..it
  //essentially means that delayed_dispatch msg did not get cancelled when we recvd an incoming
  //command..
  QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                "AudioPlayer::CheckSelfDelayedDispatch: m_nWaitMsec=%d, DispatchMsg->timeOut=%d!",
                m_nWaitMsec, audioDispatchMsg->delay_ms);

  return bSelfDelayedDispatch;
}

/* ========================================================================
FUNCTION:
  AudioPlayer::SelfDispatch

DESCRIPTION:
  Used by the audioplayer to send a delayed dispatch msg to itself.

PARAMETERS:
  uint32 dealy_ms
    Specifies the timer for the delayed dispatch. if set to 0, its an immediate
    dispatch.

RETURN VALUE:
  dispatch_id_type
    Id of the msg dispatched. It can be later used to cancel the msg if
    necessary.
===========================================================================*/
qtv_task_if_class::dispatch_id_type AudioPlayer::SelfDispatch(uint32 delay_ms)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayer::SelfDispatch() delay_ms=%d", delay_ms);

  qtv_msg_struct* msg = qtv_msg_struct::malloc();

  if(!msg)
  {
    QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::SelfDispatch.. Error!! Cannot create msg..");
    return 0;
  }
  memset(msg->payload,0,sizeof(msg->payload));

  //create/prepare the msg
  AudioMsgTypes::audio_cmd_self_dispatch_msg audioDispatchMsg;
  audioDispatchMsg.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_SELF_DISPATCH;
  audioDispatchMsg.delay_ms = delay_ms;

  qtv_task_if_class::dispatch_id_type msgType = 0;
  //copy msg into the payload
  if(sizeof(audioDispatchMsg) <= sizeof(msg->payload))
  {
    memcpy(msg->payload,(void*)&audioDispatchMsg,sizeof(audioDispatchMsg));
    //dispatch the msg to the "PV Audio" task..
    ++m_numAudioDispatchesPending;
    if (m_pDispatcher)
    {
      QCUtils::EnterCritSect(&m_SelfDispatchCS);
      msgType = m_pDispatcher->dispatch( this, msg, delay_ms );
      QCUtils::LeaveCritSect(&m_SelfDispatchCS);
    }
    else
    {
      qtv_msg_struct::free(msg);
    }
  }
  else
  {
    qtv_msg_struct::free(msg);
  }
  return msgType;
}
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
/* ========================================================================
FUNCTION:
  AudioPlayer::SetAVSyncObj

DESCRIPTION:
  Used to inform the audio player that it should use the specified
  obj ptr to set AV sync update information.

PARAMETERS:
  void* pAVSync
    AVSync obj shared by all media types.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
void AudioPlayer::SetAVSyncObj(void* pAVSync)
{
  m_pAVSync = (AVSync*)pAVSync;
  if(audioMgr)
  {
    audioMgr->SetAVSyncObj(pAVSync);
  }
}

#ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO

// 1P3X Audio Playback related definitions
// Drop one out of every four frames. Hence window size is four
#define AUDIO_PLAYBACK_SPEED_1P3X_WINDOW     4
#define AUDIO_PLAYBACK_SPEED_1P3X_DROP_COUNT 1
/* ========================================================================
FUNCTION:
  AudioPlayer::SetPlaybackSpeed

DESCRIPTION:
  Sets the playback speed.

PARAMETERS:
  Common::PlaybackSpeedType ePlaybackSpeed
    The desired playback speed.

RETURN VALUE:
  None.
===========================================================================*/
void AudioPlayer::SetPlaybackSpeed(Common::PlaybackSpeedType ePlaybackSpeed)
{
  m_ePlaybackSpeed = ePlaybackSpeed;
  if(audioMgr)
  {
    switch(ePlaybackSpeed)
    {
      case Common::PLAYBACK_SPEED_1P3X:
      {
        QTV_MSG(QTVDIAG_AUDIO_TASK,"SetPlaybackAudioSpeed  to 1P3X");
        audioMgr->StartFastAudioPlayback(AUDIO_PLAYBACK_SPEED_1P3X_WINDOW,
                                         AUDIO_PLAYBACK_SPEED_1P3X_DROP_COUNT);
        break;
      }
      case Common::PLAYBACK_SPEED_NORMAL:
      {
        QTV_MSG(QTVDIAG_AUDIO_TASK,"SetPlaybackAudioSpeed  to Normal");
        audioMgr->StopFastAudioPlayback();
        break;
      }
      default:
      {
        QTV_MSG(QTVDIAG_AUDIO_TASK,
                "SetPlaybackAudioSpeed received No Change; Ignored");
        break;
      }
    }//switch
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH,
            "SetPlaybackAudioSpeed Queued since Audio Manager Not created yet");
    m_bPlaybackSpeedQueued = true;
    m_ePlaybackSpeed       = ePlaybackSpeed;
  }
  return;
}
#endif /* FEATURE_QTV_FAST_PLAYBACK_AUDIO */

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ========================================================================
FUNCTION:
  AudioPlayer::GetAudioTrackSpec

DESCRIPTION:
  Returns the audio track information.

PARAMETERS:
  uint32 &trackState
    The audio track state.

  Common::ChannelConfigType &eChannelConfig
    The audio channel configuration.

  int &nSamplingFreq
    The audio sampling frequency.

RETURN VALUE:
  bool
    TRUE if the track information was obtained, FALSE otherwise.
===========================================================================*/
bool AudioPlayer::GetAudioTrackSpec( uint32                    &bmTrackState,
                                     Common::ChannelConfigType &eChannelConfig,
                                     int                       &nSamplingFreq)
{
  bool bRet= true;
  // Default values in case audio manager does not exist
  bmTrackState   = 0;
  eChannelConfig = Common::AUDIO_CHANNEL_UNKNOWN;
  nSamplingFreq  = 0;
  if(audioMgr)
  {
    Common::AudioTrackState eState;  /* Audio Track State */
    audioMgr->GetAudioTrackSpec(eState,eChannelConfig,nSamplingFreq);
    // Conversion of audio Track state enumeration to QtvPlayer bitmask type
    // <TBD> Following logic need to be moved to Mpeg4Player module later
    if(eState == Common::AUDIO_TRACK_STATE_READY)
    {
      bmTrackState = QTV_TRACK_STATE_READY;
    }
    else if(eState == Common::AUDIO_TRACK_STATE_DISABLED)
    {
      bmTrackState = QTV_TRACK_STATE_DISABLED;
    }
    else
    {
      bmTrackState = QTV_TRACK_STATE_SELECTED;
    }
  }
  else
  {
    bRet = false;
    QTV_MSG(QTVDIAG_AUDIO_TASK,
            "GetAudioTrackSpec returning default values due to null audio mgr");
  }
  return bRet;
}
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
/* ========================================================================
FUNCTION:
  AudioPlayer::SetDualMonoOutput

DESCRIPTION:
  Sets the dual mono output configuration.

PARAMETERS:
  Common::DualMonoOutputType dualMonoOutput
    The desired dual mono output configuration.

RETURN VALUE:
  bool
    TRUE if configuration was set successfully, FALSE otherwise.
===========================================================================*/
bool AudioPlayer::SetDualMonoOutput(Common::DualMonoOutputType dualMonoOutput)
{
  bool bRet = true;

  /* Save the new setting */
  m_dualMonoOutput = dualMonoOutput;

  if (audioMgr)
  {
    bRet = audioMgr->SetDualMonoOutput(dualMonoOutput);
  }

  return bRet;
}
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif /* FEATURE_QTV_ADTS_PARSER */
/* ======================================================================
FUNCTION
  AudioPlayer::IsPTSSyncd

DESCRIPTION
//
// This method is used to check whether the next media sample
// is sync'd to the PCR. This method receive the audio data
// lead parameter to account for the sample queued in the
// CMX buffer.

DEPENDENCIES
   Depends of FEATURE_QTV_GENERIC_BCAST_PCR


RETURN VALUE
  True/False

SIDE EFFECTS
  None.

========================================================================== */
bool AudioPlayer::IsPTSSyncd(int nDataLead)
{

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#else
  QTV_USE_ARG1(nDataLead);
  return true;
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
}

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
/* ======================================================================
FUNCTION
  AudioPlayer::IsResyncReqd

DESCRIPTION
//
// This method is used to check whether the audio stream
// needs to be re-syncd in playing state.

DEPENDENCIES
   Depends of FEATURE_QTV_GENERIC_BCAST_PCR


RETURN VALUE
  True/False

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioPlayer::IsResyncReqd(void)
{

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#else
  return false;
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
}

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

/* ========================================================================
FUNCTION:
  EventMSG::EventMSG

DESCRIPTION:
  Constructor EventMSG will memset to ZERO for all class members and create MIN_EVENT_COUNT Event Message.

PARAMETERS:


RETURN VALUE:
  None.
===========================================================================*/

EventMSG::EventMSG()
{
  QCUtils::InitCritSect( &m_event_cs);

  audio_timing_e.readIndex = audio_timing_e.writeIndex = audio_timing_e.count = 0;
  audio_status_e.readIndex = audio_status_e.writeIndex = audio_status_e.count = 0;

  pushEvents();
}

/* ========================================================================
FUNCTION:
  EventMSG::~EventMSG

DESCRIPTION:
  ~EventMSG will release unused Events from array event_ptr.

PARAMETERS:


RETURN VALUE:
  None.
===========================================================================*/
EventMSG::~EventMSG()
{
  QTV_MSG_PRIO2(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_LOW, "EventMSG::~EventMSG() audio_timing_e.count: %d audio_status_e.count: %d", audio_timing_e.count, audio_status_e.count);

  QCUtils::EnterCritSect(&m_event_cs);
  if( audio_timing_e.count != 0 )
  {
    releaseEvents(AUDIO_TIMING);
  }
  if( audio_status_e.count != 0 )
  {
    releaseEvents(AUDIO_STATUS);
  }
  QCUtils::LeaveCritSect(&m_event_cs);

  QCUtils::DinitCritSect(&m_event_cs);
}

/* ========================================================================
FUNCTION:
  EventMSG::releaseEvents

DESCRIPTION:
  ~EventMSG will release unused Events from array event_ptr.

PARAMETERS:


RETURN VALUE:
  None.
===========================================================================*/
void EventMSG::releaseEvents(uint16 eventID)
{
  event_info *eventInfo = NULL;

  if(eventID == AUDIO_TIMING)
  {
    eventInfo = &audio_timing_e;
  }
  else if(eventID == AUDIO_STATUS)
  {
    eventInfo = &audio_status_e;
  }
  else
  {
    return;
  }
  if(eventInfo->readIndex > eventInfo->writeIndex)
  {
    do
    {
      zrex_event_release(eventInfo->event_ptr[eventInfo->readIndex]);
      if(eventInfo->readIndex == (MAX_EVENT_COUNT - 1 ))
        eventInfo->readIndex = 0;
      else
        eventInfo->readIndex ++;
    }while(eventInfo->readIndex != eventInfo->writeIndex);
  }
  else
  {
    do
    {
      zrex_event_release(eventInfo->event_ptr[eventInfo->readIndex]);
      eventInfo->readIndex ++;
    }while(eventInfo->readIndex < eventInfo->writeIndex);
  }
  eventInfo->readIndex = eventInfo->writeIndex = eventInfo->count = 0;
}
/* ========================================================================
FUNCTION:
  EventMSG::pushEvents

DESCRIPTION:
  This function will adds Events in an array event_ptr.

PARAMETERS:


RETURN VALUE:
  None.
===========================================================================*/
void EventMSG::pushEvents()
{
  QTV_MSG_PRIO3( QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_LOW, "EventMSG::pushEvent audio_timing_e.count: %d , audio_timing_e.readIndex: %d, audio_timing_e.writeIndex: %d", audio_timing_e.count, audio_timing_e.readIndex, audio_timing_e.writeIndex);

  QCUtils::EnterCritSect(&m_event_cs);

  if(audio_timing_e.count != MIN_EVENT_COUNT)
  {
    QCUtils::LeaveCritSect(&m_event_cs);

    while(true)
    {
      if(pushEvent(AUDIO_TIMING) == false)
      {
        break;
      }

      QCUtils::EnterCritSect(&m_event_cs);
      if(audio_timing_e.count >= MIN_EVENT_COUNT)
      {
        QCUtils::LeaveCritSect(&m_event_cs);
    	break;
      }
      QCUtils::LeaveCritSect(&m_event_cs);
    }
  }
  else
  {
    QCUtils::LeaveCritSect(&m_event_cs);
  }

  QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_LOW, "EventMSG::pushEvent audio_status_e.count: %d , audio_status_e.readIndex: %d, audio_status_e.writeIndex: %d", audio_status_e.count, audio_status_e.readIndex, audio_status_e.writeIndex);

  QCUtils::EnterCritSect(&m_event_cs);
  if(audio_status_e.count!=MIN_EVENT_COUNT)
  {
    QCUtils::LeaveCritSect(&m_event_cs);
    while(true)
    {
      if(pushEvent(AUDIO_STATUS) == false)
      {
        break;
      }
      QCUtils::EnterCritSect(&m_event_cs);
      if(audio_status_e.count >= MIN_EVENT_COUNT)
      {
        QCUtils::LeaveCritSect(&m_event_cs);
        break;
      }
      QCUtils::LeaveCritSect(&m_event_cs);
    }
  }
  else
  {
    QCUtils::LeaveCritSect(&m_event_cs);
  }
}

/* ========================================================================
FUNCTION:
  EventMSG::pushEvents

DESCRIPTION:
  This function will create Event and add to array called event_ptr.

PARAMETERS:


RETURN VALUE:
  Success / Failure.
===========================================================================*/
bool EventMSG::pushEvent(uint16 eventID)
{
  event_info *eventInfo = NULL;

  if(eventID == AUDIO_TIMING)
  {
    void * event_ptr = QCCreateMessage( AUDIO_TIMING, NULL);
    if(event_ptr == NULL)
    {
      return false;
    }
    eventInfo = &audio_timing_e;

    QCUtils::EnterCritSect(&m_event_cs);
    eventInfo->event_ptr[eventInfo->writeIndex] = event_ptr;
    QCUtils::LeaveCritSect(&m_event_cs);
  }
  else if(eventID == AUDIO_STATUS)
  {
    void * event_ptr = QCCreateMessage( AUDIO_STATUS, NULL);
    if(event_ptr == NULL)
    {
      return false;
    }
    eventInfo = &audio_status_e;

    QCUtils::EnterCritSect(&m_event_cs);
    eventInfo->event_ptr[eventInfo->writeIndex] = event_ptr;
    QCUtils::LeaveCritSect(&m_event_cs);
  }
  else
  {
    return false;
  }

  QCUtils::EnterCritSect(&m_event_cs);
  eventInfo->count++;
  eventInfo->writeIndex++;
  if(eventInfo->writeIndex >= MAX_EVENT_COUNT)
  {
    eventInfo->writeIndex = 0;
  }
  QCUtils::LeaveCritSect(&m_event_cs);

  return true;
}

/* ========================================================================
FUNCTION:
  EventMSG::popEvent

DESCRIPTION:
  This function will pop Event from array called event_ptr and will return Event pointer.

PARAMETERS:


RETURN VALUE:
  Event Pointer.
===========================================================================*/
void* EventMSG::popEvent(uint16 eventID)
{
  event_info *eventInfo = NULL;
  void *pEvent = NULL;

  if(eventID == AUDIO_TIMING)
  {
    eventInfo = &audio_timing_e;
  }
  else if(eventID == AUDIO_STATUS)
  {
    eventInfo = &audio_status_e;
  }
  else
  {
    return NULL;
  }

  QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_LOW, "EventMSG::popEvent");

  QCUtils::EnterCritSect(&m_event_cs);
  if(eventInfo->count == 0)
  {
    QCUtils::LeaveCritSect(&m_event_cs);
    QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH, "EventMSG::popEvent count is ZERO nothing to POP eventId: %d", eventID);
    return NULL;
  }
  else
  {
    pEvent = eventInfo->event_ptr[eventInfo->readIndex];
    eventInfo->count--;
    eventInfo->readIndex++;
    if(eventInfo->readIndex >= MAX_EVENT_COUNT)
    {
      eventInfo->readIndex = 0;
    }
  }
  QCUtils::LeaveCritSect(&m_event_cs);
  return pEvent;
}
