/* =======================================================================

                          audioPlayerIf.cpp

DESCRIPTION
  Implementation of audio subsystem public interface. This interface uses
  the support of the message/IPC-based qtv common-task class to interact
  with unique audio player instaces.

Copyright 2005, 2006 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audioplayerif.cpp#13 $
$DateTime: 2009/11/30 03:18:44 $
$Change: 1098040 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "audio.h"
#include "audioPlayerIf.h"
#include "audioMsgTypes.h"
#include "qtv_task_if.h"
extern "C"
{
  #include "task.h"
}

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
static uint8 numAudioPlayer = 0;
static qtv_task_if_class* m_pAudioDispatcher = NULL;
static bool bPlayerIfInitDone = false;
static rex_crit_sect_type  dispatcherCS;

/* Sync event used to synchronize the response from the audio player inst. */
static QCConditionType responseSync;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
/*Function used internally by AudioPlayerIf to dispatch msg to sink objects*/
static bool DispatchMsgToSink( void* pMsg, 
                               size_t msgSize, 
                               AUDIO_PLAYER_ID playerID );


/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Init

DESCRIPTION:
  Initialize the audio subsystem/player public interface. This should be the
  first call before invoking any other operations.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void AudioPlayerIf::Init()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Init()");
  if(!bPlayerIfInitDone)
  {
    numAudioPlayer = 0;
    m_pAudioDispatcher = NULL;
    QCUtils::CreateCondition(&responseSync,false,false);
    bPlayerIfInitDone = true;
    QCUtils::InitCritSect( &dispatcherCS);
  }
  else
  {
    QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Init already initialized..!!");
  }
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::CreateAudioPlayer

DESCRIPTION:
  Initialize the audio subsystem/player public interface. This should be the
  first call before invoking any other operations.

PARAMETERS:
  AUDIO_PLAYER_ID& apID
    Will contaian the audio player id (>0) if it get successfully created.

  PLAYER_NOTIFY_CB_FN  playerCB
    Client callback function.

  void*  pClientData
    Client specific data

RETURN VALUE:
  True
    If a new audio player is created.
===========================================================================*/
bool AudioPlayerIf::CreateAudioPlayer(AUDIO_PLAYER_ID&     apID,
                                      PLAYER_NOTIFY_CB_FN  playerCB,
                                      void*                pClientData)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::CreateAudioPlayer()");

  bool bOK = false;

  if(!m_pAudioDispatcher)
  {
#ifndef FEATURE_WINCE
  
    m_pAudioDispatcher = QTV_New_Args( qtv_task_if_class,
                                         ( &qtv_audio_tcb,
                                           qtv_task_audio_info )
                                      );
#endif									  

  }

  if( m_pAudioDispatcher )
  {

    AudioPlayer* audioPlayer = QTV_New_Args(AudioPlayer,
                                            ((CLIENT_NOTIFY_CB_FN)playerCB, pClientData, responseSync,
                                              m_pAudioDispatcher));
    if(audioPlayer)
    {
      apID = (AUDIO_PLAYER_ID)audioPlayer;
      numAudioPlayer++;
      bOK = true;
    }
    else
    {
      apID = 0;
      QTV_MSG( QTVDIAG_AUDIO_TASK,
                "AudioPlayerIf::CreateAudioPlayer couldn't construct AudioPlayer (out of memory?)"
              );
    }
  }
  else
  {
    QTV_MSG( QTVDIAG_AUDIO_TASK,
              "AudioPlayerIf::CreateAudioPlayer couldn't construct Audio Task (out of memory?)"
            );
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::CreateAudioPlayer() return=%d",bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::SetAVSyncObj

DESCRIPTION:
  Used to inform the audio player that it should use the specified
  obj ptr to set AV sync update information.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  void* pAVSync
    AVSync obj shared by all media types.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
void AudioPlayerIf::SetAVSyncObj(AUDIO_PLAYER_ID const recipientID, void* pAVSync)
{
  ((AudioPlayer*)recipientID)->SetAVSyncObj(pAVSync);
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::initializeAudioplayer

DESCRIPTION:
  Initialize audio player.
  Since this function is called to initialize the audio player we make a
  direct call onto the specified audio player inst; without going through
  the qtv_task_if msg queue.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.
  audio_type
     Type of audio manager to be created
  pMedia
     Pointer to Media object

RETURN VALUE:
  long
    Elapsed time.
===========================================================================*/
bool AudioPlayerIf::initializeAudioplayer
(
  AUDIO_PLAYER_ID const recipientID,
  const char *audio_type,
  Media* pMedia)
{
  //Direct call
  if(recipientID)
  {
    ((AudioPlayer*)recipientID)->initializeAudioplayer(audio_type,
                                                       pMedia);
  }
  return true;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Start

DESCRIPTION:
  Start the audio player.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  bool bRestart
    Used internally by audio player.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::Start(AUDIO_PLAYER_ID const recipientID, bool bRestart)
{
/*      create qtv_msg_struct msg containing:
            msg.payload = start_event_id,data
        queue msg to task:
            m_pAudioDispatcher->dispatch(recipient_prt,msg);
*/
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Start() recipientID=%d", recipientID);

  bool bOK = false;
  bool bStart = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_start_msg audioStart;
  audioStart.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_START;
  audioStart.bRestart = bRestart;
  audioStart.pbStart = &bStart;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioStart,
                          sizeof(audioStart),
                          recipientID);

  if(bOK)
  {
    bOK = bStart;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Start() return=%d",bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Pause

DESCRIPTION:
  Pause the audio player.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::Pause(AUDIO_PLAYER_ID const recipientID)
{
/*      create qtv_msg_struct msg containing:
            msg.payload = pause_event_id
        queue msg to task:
            m_pAudioDispatcher->dispatch(recipient_ptr,msg);
*/
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Pause() recipientID=%d", recipientID);

  bool bOK =false;
  bool bPause = false;

  //create/prepare the msg
  AudioMsgTypes::audio_cmd_pause_msg audioPause;
  audioPause.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_PAUSE;
  audioPause.pbPause = &bPause;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioPause,
                          sizeof(audioPause),
                          recipientID);
  if(bOK)
  {
    bOK = bPause;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Pause() return=%d",bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Resume

DESCRIPTION:
  Resume the audio player.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  long stop
    Used internally by audio player.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::Resume(AUDIO_PLAYER_ID const recipientID, long stop)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Resume() recipientID=%d", recipientID);

  bool bOK=false;
  bool bResume = false;

  //create/prepare the msg
  AudioMsgTypes::audio_cmd_resume_msg audioResume;
  audioResume.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_RESUME;
  audioResume.stop = stop;
  audioResume.pbResume = &bResume;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioResume,
                          sizeof(audioResume),
                          recipientID);
  if(bOK)
  {
    bOK = bResume;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Resume() return=%d",bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Suspend

DESCRIPTION:
  Suspend the audio player.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation fails. It reports an err status.
===========================================================================*/
bool AudioPlayerIf::Suspend(AUDIO_PLAYER_ID const recipientID)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Suspend() recipientID=%d", recipientID);

  bool bOK = false;
  bool bSuspend = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_suspend_msg audioSuspend;
  audioSuspend.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_SUSPEND;
  audioSuspend.pbSuspend = &bSuspend;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioSuspend,
                          sizeof(audioSuspend),
                          recipientID);
  if(bOK)
  {
    bOK = bSuspend;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Suspend() return=%d",bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Stop

DESCRIPTION:
  Stop the audio player.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation fails. It reports an err status.
===========================================================================*/
bool AudioPlayerIf::Stop(AUDIO_PLAYER_ID const recipientID)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Stop() recipientID=%d", recipientID);

  bool bOK = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_stop_msg audioStop;
  audioStop.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_STOP;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioStop,
                          sizeof(audioStop),
                          recipientID);

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Stop() return=%d",!bOK);
  //Need to report the error status.
  return !bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::RestartAudioOutput

DESCRIPTION:
  Start audio after buffering is over.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::RestartAudioOutput(AUDIO_PLAYER_ID const recipientID)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::RestartAudioOutput() recipientID=%d", recipientID);

  bool bOK =false;
  bool bRestartAudioOutput = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_restart_output_msg audioRestartOutput;
  audioRestartOutput.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_RESTART_AUDIO_OUTPUT;
  audioRestartOutput.pbRestartAudioOutput = &bRestartAudioOutput;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioRestartOutput,
                          sizeof(audioRestartOutput),
                          recipientID);
  if(bOK)
  {
    bOK = bRestartAudioOutput;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::RestartAudioOutput() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Prep

DESCRIPTION:
  Used to setup the audio manager.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  playbackID, bRestart, pMedia, bHasVideo
    Used internally by audio player.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::Prep(AUDIO_PLAYER_ID const recipientID, int playbackID, bool bRestart,
                         void* pMedia, bool bHasVideo, Media::CodecType audioCodec)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Prep() recipientID=%d", recipientID);

  bool bOK = false;
  bool bPrep = false;
  //create/prepare the msg
  AudioMsgTypes::audio_prep_msg audioPrep;
  audioPrep.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_PREP;
  audioPrep.playbackID = playbackID;
  audioPrep.bRestart = bRestart;
  audioPrep.pMedia = pMedia;
  audioPrep.bHasVideo = bHasVideo;
  audioPrep.pbPrep = &bPrep;
  audioPrep.audioCodec = audioCodec;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioPrep,
                          sizeof(audioPrep),
                          recipientID);
  if(bOK)
  {
    bOK = bPrep;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Prep() return=%d", bOK);
  return bOK;
}


/* ========================================================================
FUNCTION:
  AudioPlayerIf::GetElapsedTime

DESCRIPTION:
  Fetch the audio track elapsed time in msec. Since this function is called
  periodically every few msec we make a direct call onto the specified audio
  player inst; without going through the qtv_task_if msg queue.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  long
    Elapsed time.
===========================================================================*/
long AudioPlayerIf::GetElapsedTime(AUDIO_PLAYER_ID const recipientID)
{
  //Direct call

  long elapsedTime = 0;
  if(recipientID)
  {
    elapsedTime = ((AudioPlayer*)recipientID)->GetElapsedTime();
  }

  return elapsedTime;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::GetAmtBufferedInCmx

DESCRIPTION:
  See how far data processing was ahead of audio playback at the current clock time.
  Since this function is called periodically every few msec we make a direct call
  onto the specified audio player inst; without going through the qtv_task_if msg queue.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  long
    amd buffered in cmx in msec.
===========================================================================*/
long AudioPlayerIf::GetAmtBufferedInCmx(AUDIO_PLAYER_ID const recipientID)
{
  //Direct call

  long cmxAmtBufferedMsec = 0;
  if(recipientID)
  {
    cmxAmtBufferedMsec = ((AudioPlayer*)recipientID)->GetAmtBufferedInCMX();
  }

  return cmxAmtBufferedMsec;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::GetPosition

DESCRIPTION:
  Fetch the number of audio frames so far process witin the audio track.
  Since this function is called periodically every few msec we make a direct
  call onto the specified audio player inst; without going through the
  qtv_task_if msg queue.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If more than one frame has been processed.
===========================================================================*/
bool AudioPlayerIf::GetPosition( AUDIO_PLAYER_ID const recipientID, int32& pos )
{
  //Direct call
  bool bOK = false;

  if(recipientID)
  {
    bOK = ((AudioPlayer*)recipientID)->GetPosition(pos);
  }

  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::EnableTimeCheck

DESCRIPTION:
  Used to enable time check during streaming when the streamer
  returns Common::STREAM_REDIRECT.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  long timeValue
    Used internally by audio player.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::EnableTimeCheck( AUDIO_PLAYER_ID const recipientID, unsigned long timeValue )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::EnableTimeCheck() recipientID=%d", recipientID);

  bool bOK =false;
  //create/prepare the msg
  AudioMsgTypes::audio_enable_time_check_msg audioEnableTimeCheck;
  audioEnableTimeCheck.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_ENABLE_TIME_CHECK;
  audioEnableTimeCheck.timeValue = timeValue;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioEnableTimeCheck,
                          sizeof(audioEnableTimeCheck),
                          recipientID);


  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::EnableTimeCheck() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::GetStatistics

DESCRIPTION:
  Used to collect audio playback stats.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  QtvPlayer::AudioVideoStatisticsT &stats
  Common::LastFrame &lastFrame
    Used to collect statistical data.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::GetStatistics( AUDIO_PLAYER_ID const recipientID,
                                 QtvPlayer::AudioVideoStatisticsT &stats,
                                 Common::LastFrame &lastFrame)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::GetStatistics() recipientID=%d", recipientID);
  bool bOK = false;
  if(recipientID)
  {
    bOK = ((AudioPlayer*)recipientID)->GetStatistics(stats, lastFrame);
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::GetStatistics() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::IsDone

DESCRIPTION:
  See if audio player is in IDLE state or done playing. Since this function
  is called periodically every few msec we make a direct call onto the
  specified audio player inst; without going through the qtv_task_if msg queue.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If the audio player is done !.
===========================================================================*/
bool AudioPlayerIf::IsDone( AUDIO_PLAYER_ID const recipientID )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsDone() recipientID=%d", recipientID);

  bool bIsDone = false;
  if(recipientID)
  {
     bIsDone = ((AudioPlayer*)recipientID)->IsDone();
  }

  return bIsDone;
}

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */


/* ========================================================================
FUNCTION:
  AudioPlayerIf::IsPlaying

DESCRIPTION:
  Check if audio player is in PLAYING state.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If the audio player is still playing !.
===========================================================================*/
bool AudioPlayerIf::IsPlaying( AUDIO_PLAYER_ID const recipientID )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsPlaying() recipientID=%d", recipientID);

  bool bOK = false;
  bool bIsPlaying = false;
  //create/prepare the msg
  AudioMsgTypes::audio_get_isplaying_msg audioGetIsPlaying;
  audioGetIsPlaying.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_GET_ISPLAYING;
  audioGetIsPlaying.pIsPlaying = &bIsPlaying;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioGetIsPlaying,
                          sizeof(audioGetIsPlaying),
                          recipientID);
  if(bOK)
  {
    bOK = bIsPlaying;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsPlaying() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::IsPaused

DESCRIPTION:
  Check if audio player is in PAUSE state.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If the audio player is paused !.
===========================================================================*/
bool AudioPlayerIf::IsPaused( AUDIO_PLAYER_ID const recipientID )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsPaused() recipientID=%d", recipientID);

  bool bOK = false;
  bool bIsPaused = false;
  //create/prepare the msg
  AudioMsgTypes::audio_get_ispaused_msg audioGetIsPaused;
  audioGetIsPaused.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_GET_ISPAUSED;
  audioGetIsPaused.pIsPaused = &bIsPaused;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioGetIsPaused,
                          sizeof(audioGetIsPaused),
                          recipientID);
  if(bOK)
  {
    bOK = bIsPaused;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsPaused() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::IsPausedDisabled

DESCRIPTION:
  Check if audio player is in PAUSED_DISABLED state.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If the audio player is PAUSED_DISABLED !.
===========================================================================*/
bool AudioPlayerIf::IsPausedDisabled( AUDIO_PLAYER_ID const recipientID )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, 
            "AudioPlayerIf::IsPausedDisabled() recipientID=%d", 
            recipientID);

  bool bOK = false;
  bool bIsPausedDisabled = false;
  //create/prepare the msg
  AudioMsgTypes::audio_get_ispaused_disabled_msg audioGetIsPausedDisabled;
  audioGetIsPausedDisabled.msgHdr.msgId = 
    (uint16)AudioMsgTypes::AUDIO_PLAYER_GET_ISPAUSED_DISABLED;
  audioGetIsPausedDisabled.pIsPausedDisabled = &bIsPausedDisabled;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioGetIsPausedDisabled,
                          sizeof(audioGetIsPausedDisabled),
                          recipientID);
  if(bOK)
  {
    bOK = bIsPausedDisabled;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsPausedDisabled() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::IsIdle

DESCRIPTION:
  Check if audio player is in IDLE state.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If the audio player is idle !.
===========================================================================*/
bool AudioPlayerIf::IsIdle(AUDIO_PLAYER_ID const recipientID)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsIdle() recipientID=%d", recipientID);

  bool bOK = false;
  bool bIsIdle = false;
  //create/prepare the msg
  AudioMsgTypes::audio_get_isidle_msg audioGetIsIdle;
  audioGetIsIdle.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_GET_ISIDLE;
  audioGetIsIdle.pIsIdle = &bIsIdle;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioGetIsIdle,
                          sizeof(audioGetIsIdle),
                          recipientID);
  if(bOK)
  {
    bOK = bIsIdle;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::IsIdle() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::TrackHasEnded

DESCRIPTION:
  Check if audio player has finished fetching all the audio samples OR there
  was an error an the player state is set to Idle.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If the despription condition was met !.
===========================================================================*/
bool AudioPlayerIf::TrackHasEnded( AUDIO_PLAYER_ID const recipientID )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::TrackHasEnded() recipientID=%d", recipientID);

  bool bOK = false;
  bool bIsTrackEnded = false;
  //create/prepare the msg
  AudioMsgTypes::audio_get_track_ended_msg audioGetTrackEnded;
  audioGetTrackEnded.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_GET_TRACK_ENDED;
  audioGetTrackEnded.pIsTrackEnded = &bIsTrackEnded;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioGetTrackEnded,
                          sizeof(audioGetTrackEnded),
                          recipientID);
  if(bOK)
  {
    bOK = bIsTrackEnded;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::TrackHasEnded() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::ResetData

DESCRIPTION:
  Used to reset the audio playback data and audio statistical info. This is a
  direct call to the audio player. The outcome of this operation does not
  depend on commands on the queue..!

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  None.
===========================================================================*/
void AudioPlayerIf::ResetData( AUDIO_PLAYER_ID const recipientID )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::ResetData() recipientID=%d", recipientID);

  //Direct call
  if(recipientID)
  {
    ((AudioPlayer*)recipientID)->ResetData();
  }

  return;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::RestartAudio

DESCRIPTION:
  It Stops->Preps->Starts the audio player.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::RestartAudio( AUDIO_PLAYER_ID const recipientID )
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::RestartAudio() recipientID=%d", recipientID);

  bool bOK = false;
  bool bPrepRestartAudioOutput = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_prep_and_restart_output_msg audioPrepAndRestartOutput;
  audioPrepAndRestartOutput.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_PREP_AND_RESTART_AUDIO_OUTPUT;
  audioPrepAndRestartOutput.pbPrepRestartAudioOutput = &bPrepRestartAudioOutput;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioPrepAndRestartOutput,
                          sizeof(audioPrepAndRestartOutput),
                          recipientID);
  if(bOK)
  {
    bOK = bPrepRestartAudioOutput;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::RestartAudio() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::SetPlayTimes

DESCRIPTION:
  Used during repositioning to set the new start/stop time for the audio track.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  long start, long stop
    specifies the new start and stop time for the player.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::SetPlayTimes( AUDIO_PLAYER_ID const recipientID, long start, long stop)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::SetPlayTimes() recipientID=%d", recipientID);

  bool bOK = false;
  //create/prepare the msg
  AudioMsgTypes::audio_set_play_times_msg audioSetPlayTimes;
  audioSetPlayTimes.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_SET_PLAY_TIME;
  audioSetPlayTimes.start = start;
  audioSetPlayTimes.stop = stop;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioSetPlayTimes,
                          sizeof(audioSetPlayTimes),
                          recipientID);

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::SetPlayTimes() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::SetTSfromBytes

DESCRIPTION:
  

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  long TSfromBytes
    

RETURN VALUE:
  None
===========================================================================*/

void AudioPlayerIf::SetTSfromBytes(AUDIO_PLAYER_ID const recipientID, int32 TSfromBytes)
{
    AudioPlayer* audPlayer = ( AudioPlayer* )recipientID;
    if (audPlayer)
    {
       audPlayer->SetTimestampfromBytes(TSfromBytes);
    }
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::DrainAudioBuffer

DESCRIPTION:
  Used to drain the audio buffer during release-restore.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  uint32 timestamp
    Used internally by audio player.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::DrainAudioBuffer(AUDIO_PLAYER_ID const recipientID, uint32 timestamp)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::DrainAudioBuffer() recipientID=%d", recipientID);

  bool bOK = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_drain_audio_buffer_msg audioDrainBuffer;
  audioDrainBuffer.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_DRAIN_AUDIO_BUFFER;
  audioDrainBuffer.timestamp = timestamp;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioDrainBuffer,
                          sizeof(audioDrainBuffer),
                          recipientID);

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::DrainAudioBuffer() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Disable

DESCRIPTION:
  Used to disable the audio player during release-restore.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::Disable(AUDIO_PLAYER_ID const recipientID)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Disable() recipientID=%d", recipientID);

  bool bOK = false;
  bool bDisable = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_disable_msg audioDisable;
  audioDisable.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_DISABLE;
  audioDisable.pbDisable = &bDisable;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioDisable,
                          sizeof(audioDisable),
                          recipientID);
  if(bOK)
  {
    bOK = bDisable;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Disable() return=%d", bOK);
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::SetAudioChannelSwitchTS

DESCRIPTION:
  Used to switch the audio channel ts.!.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::SetAudioChannelSwitchTS(AUDIO_PLAYER_ID const recipientID, uint32 reqData)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::SetAudioChannelSwitchTS() recipientID=%d", recipientID);

  bool bOK = true;
  if(recipientID)
  {
    (void)((AudioPlayer*)recipientID)->SetAudioChannelSwitchTS(reqData);
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::SetAudioChannelSwitchTS() return=%d", bOK);
  return bOK;

}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::ShowInfo

DESCRIPTION:
  Used to show statistical information about the audio manager.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool AudioPlayerIf::ShowInfo(AUDIO_PLAYER_ID const recipientID)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::ShowInfo() recipientID=%d", recipientID);

  bool bOK = false;
  //create/prepare the msg
  AudioMsgTypes::audio_cmd_show_info_msg audioShowInfo;
  audioShowInfo.msgHdr.msgId = (uint16)AudioMsgTypes::AUDIO_PLAYER_SHOW_INFO;

  //Dispatch the msg
  bOK = DispatchMsgToSink((void*)&audioShowInfo,
                          sizeof(audioShowInfo),
                          recipientID);

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::ShowInfo() return=%d", bOK);
  return bOK;

}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::DispatchMsgToSink

DESCRIPTION:
  Used internally by AudioPlayerIf to dispatch msg to sink objects (AudioPlayer inst).
  This is a blocking call and will wait until the sink obj responds to the
  msg sent OR COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC has elapsed.

PARAMETERS:
  void* pMsg
    pointer to the msg.

  size_t msgSize
    size of the msg.

  qtv_msg_sink_class* pSink
    pointer to the sink obj.

RETURN VALUE:
  True
    If this operation is successful.
===========================================================================*/
bool DispatchMsgToSink(void* pMsg, size_t msgSize, AUDIO_PLAYER_ID playerID)
{
  /* AUDIO_PLAYER_ID is just a disguised audioPlayer pointer. */
  AudioPlayer* pSink = ( AudioPlayer* )playerID;

  if(!pSink)
  {
    return false;
  }

  qtv_msg_struct* msg = qtv_msg_struct::malloc();

  if(!msg)
  {
    QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::DispatchMsgToSink.. Error!! Cannot create msg..");
    return false;
  }
  memset(msg->payload,0,sizeof(msg->payload));

  bool bOK = false;
  //copy msg into the payload
  if(msgSize <= sizeof(msg->payload))
  {
    memcpy(msg->payload, pMsg, msgSize);

    QCUtils::EnterCritSect(&dispatcherCS);
    QCUtils::ResetCondition(&responseSync);
    //dispatch the msg to the "PV Audio" task..
    ++( pSink->NumAudioDispatchesPending() );
    if( m_pAudioDispatcher->dispatch(pSink, msg) )
    {
      //wait for response.
      bOK = QCUtils::WaitForCondition(&responseSync,COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC);

      if(!bOK)
      {
        QTV_MSG( QTVDIAG_AUDIO_TASK,
                  "AudioPlayerIf::DispatchMsgToSink.. Error!! COMMON_AUDIO_RESPONSE_TIMEOUT_MSEC..");
        bool bError = false;
        ((AudioPlayer*)pSink)->Stop(bError);
        ((AudioPlayer*)pSink)->Notify(AudioPlayer::IDLE,Common::AUDIO_EXCEPTION);
      }
    }
    else
    {
      --( pSink->NumAudioDispatchesPending() );
      qtv_msg_struct::free(msg);
    }
    QCUtils::LeaveCritSect(&dispatcherCS);
  }
  else
  {
    qtv_msg_struct::free(msg);
  }
  return bOK;
}

/* ========================================================================
FUNCTION:
  AudioPlayerIf::Destroy

DESCRIPTION:
  Used to release all the audio subsystem resources and cleanup.

PARAMETERS:
  None.

RETURN VALUE:
  None.
===========================================================================*/
void AudioPlayerIf::Destroy(AUDIO_PLAYER_ID const recipientID)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Destroy() recipientID=%d", recipientID);

  if(recipientID && numAudioPlayer)
  {
    ((AudioPlayer*)recipientID)->Destroy();
    QTV_Delete( (AudioPlayer*)recipientID );
    --numAudioPlayer;

    if(!numAudioPlayer)
	{
      // Before destroying the sync variable, signal 
      // all the tasks waiting on the variable
      QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioPlayerIf::Destroy() setting responsesync");

      QCUtils::SetCondition(&responseSync);

      QCUtils::DestroyCondition(&responseSync);

      if ( m_pAudioDispatcher != NULL )
      {
        QTV_Delete( m_pAudioDispatcher );
        m_pAudioDispatcher = NULL;
      }
      bPlayerIfInitDone = false;
    }
  }
  QCUtils::DinitCritSect(&dispatcherCS);
  return;
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO
/* ========================================================================
FUNCTION:
  AudioPlayerIf::SetPlaybackSpeed

DESCRIPTION:
  Sets the playback speed.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  Common::PlaybackSpeedType pbSpeed
    The desired playback speed.

RETURN VALUE:
  None.
===========================================================================*/
void AudioPlayerIf::SetPlaybackSpeed(AUDIO_PLAYER_ID const recipientID, 
                                     Common::PlaybackSpeedType const pbSpeed)
{
  if (recipientID)
  {
    ((AudioPlayer*)recipientID)->SetPlaybackSpeed(pbSpeed);
  }
}
#endif /* FEATURE_QTV_FAST_PLAYBACK_AUDIO */

#ifdef FEATURE_QTV_GENERIC_BCAST
/* ========================================================================
FUNCTION:
  AudioPlayerIf::GetAudioTrackSpec

DESCRIPTION:
  Returns the audio track information.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

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
bool AudioPlayerIf::GetAudioTrackSpec( AUDIO_PLAYER_ID const     recipientID, 
                                       uint32                    &bmTrackState, 
                                       Common::ChannelConfigType &eChannelConfig, 
                                       int                       &nSamplingFreq)
{
  bool bRet = false;

  if (recipientID)
  {
    bRet = ((AudioPlayer*)recipientID)->GetAudioTrackSpec(bmTrackState,
                                                          eChannelConfig,
                                                          nSamplingFreq);
  }

  return bRet;
}
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
/* ========================================================================
FUNCTION:
  AudioPlayerIf::SetDualMonoOutput

DESCRIPTION:
  Sets the dual mono output configuration.

PARAMETERS:
  AUDIO_PLAYER_ID const recipientID
    Specifies the audio player id to which this operation should be routed to.

  Common::DualMonoOutputType dualMonoOutput
    The desired dual mono output configuration.

RETURN VALUE:
  bool 
    TRUE if configuration was set successfully, FALSE otherwise.
===========================================================================*/
bool AudioPlayerIf::SetDualMonoOutput(
  AUDIO_PLAYER_ID const recipientID, 
  Common::DualMonoOutputType const dualMonoOutput)
{
  bool bRet = false;

  if (recipientID)
  {
    bRet = ((AudioPlayer*)recipientID)->SetDualMonoOutput(dualMonoOutput);
  }

  return bRet;
}
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY 
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
//-----------------------------------------------------------------------
//
//
//
