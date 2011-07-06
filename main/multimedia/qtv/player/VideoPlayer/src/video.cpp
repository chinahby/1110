/* =======================================================================
                              Video.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.

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

$Header: //source/qcom/qct/multimedia/qtv/player/videoplayer/main/latest/src/video.cpp#28 $
$DateTime: 2010/08/12 04:25:09 $
$Change: 1399701 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "Video.h"
#include "Events.h"
#include "Mpeg4Player.h"
#include "m4v_io.h"
#include "AVSync.h"
#include "qtv_vrend.h"
#ifdef FEATURE_QTV_GENERIC_BCAST
#include "GenericBcastMedia.h"
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_QTV_DRM_DCF
 #include "zrex.h"
 #include "rex.h"
 #include "ixipc.h"
#endif
#if defined(FEATURE_QTV_GENERIC_BCAST_PCR) && defined(FEATURE_DTV_TSIF)
#error code not present
#endif

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//Set test switches here...
// undef these for normal mode.
//#define PALETTE_TEST
//#define FORCE_REBUFFERING

//these only matter when printing is enabled
// debug support
//#define SHOW_SYNC
//#define SHOW_POLLING
//#define SHOW_FRAMES

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

const uint32 COMMON_WAKEUP_SYNC_POLL_TIME_MSEC = 150;
const uint32 COMMON_VIDEO_DATA_POLL_TIME_MSEC = 10;
const uint32 MAX_VIDEO_DATA_LEAD_TIME_MSEC = 1000;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

uint32 VideoPlayer::guVideoPlayerInstanceCnt = 0;

QCThreadControl VideoPlayer::decoderTC;
QCConditionType VideoPlayer::wakeupSync;
QCConditionType VideoPlayer::responseSync;
q_type VideoPlayer::m_commandQ;
QCCritSectType VideoPlayer::m_processCommands_CS;
QCCritSectType VideoPlayer::m_commandQ_CS;
VideoPlayer    *VideoPlayer::pCurrActiveVideoPlayerInst = NULL;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
extern "C" boolean qtv_cfg_enable_dsp_release;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
////////////////////////////////////////////////////////////////////
//
// Retrieve state info
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::IsDone

DESCRIPTION:
  Checks if the player is done.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player id done rendering, else returns false.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::IsDone()
{
  return (state==IDLE || bPlayDoneRendering);
}


/* ======================================================================
FUNCTION:
  VideoPlayer::IsPlaying

DESCRIPTION:
  Checks if the player is playing

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player is playing, else returns false

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::IsPlaying()
{
  return (state==PLAYING);
}

/* ======================================================================
FUNCTION
  VideoPlayer::IsPaused

DESCRIPTION
//
// tell if video is paused.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool VideoPlayer::IsPaused()
{
  return(state==PAUSED) ;
}

/* ======================================================================
FUNCTION
  VideoPlayer::IsPausedDisabled

DESCRIPTION
//
// tell if video is paused_disabled.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool VideoPlayer::IsPausedDisabled()
{
  return(state==PAUSED_DISABLED) ;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::HasAudio

DESCRIPTION:
  Returns true if the player has audio.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player has audio.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::HasAudio()
{
   if ( pAVPlayer )
     return pAVPlayer->HasAudio();
   else
     return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::IsAudioPlaying

DESCRIPTION:
  Function checks if the audio track is playing

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the audio track is playing

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::IsAudioPlaying()
{
   if ( pAVPlayer )
     return pAVPlayer->IsAudioPlaying();
   else
     return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::HasVideo

DESCRIPTION:
  Returns true if the player has video.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player has video.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::HasVideo()
{
   if ( pAVPlayer )
     return pAVPlayer->HasVideo();
   else
     return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::setVideoStopFlag

DESCRIPTION:
 Set the flag to stop H.263 decoder initialization during initial preroll
 buffering

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player has video.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::setStopVideoInitFlag(void)
{
   QTV_MSG(QTVDIAG_VIDEO_TASK,"Setting Player Stop flag");
   decIF.setStopH263VideoInitFlag();
}

////////////////////////////////////////////////////////////////////
//
// Get info.
//
////////////////////////////////////////////////////////////////////
/* ======================================================================
FUNCTION:
  VideoPlayer::VideoCommandPending

DESCRIPTION:
  Returns true if there are commands pending

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if there are commands pending

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::VideoCommandPending()
{
  return ( q_cnt(&m_commandQ) != 0 );
}

/* ======================================================================
FUNCTION:
  VideoPlayer::GetElapsedTime

DESCRIPTION:
  Returns the elapsed time

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns the elapsed time

SIDE EFFECTS:
  None.
======================================================================*/
long VideoPlayer::GetElapsedTime()
{
  /*
  * When VIDEO PLAYER is not in a PLAYING state, we should not return nElapsedTime based on clock and
  * offset calculation. This will increment the playback time during pause state as clock is ticking.
  *
  * Also, this will take care of the case when user is attempting ShowNextFrame/ShowPrevFrame,
  * singleStep operation. We should just return nElapsedTime updated in VideoPlayer::DisplayFrame
  * rather than calculating from clock and offset.
  */
  if(state != PLAYING)
  {
    QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW, 
                  "VideoPlayer::GetElapsedTime():(state != PLAYING) nElapsedTime %d",
                  nElapsedTime);
    return nElapsedTime;
  }
  uint32 timeOffset = 0;
  if(m_pAVSync)
  {
    timeOffset = m_pAVSync->ReadPlaybackOffset(  AVSync::VideoAV, ZUtils::Clock(), 
                                                 nElapsedTime,(HasAudio()?false:true) );
  }
  long lTime = ZUtils::Clock();

  /* During fast playback the progress bar needs to be updated at the rate at  
  ** which playback occurs, so we need to multiply nElapsedTime with the
  ** plyback speed ratio. 
  */
  nElapsedTime = ((lTime - timeOffset)*m_playbackSpeedNum)/m_playbackSpeedDen;

  nElapsedTime = ZMAX(0,nElapsedTime);
  QTV_MSG_PRIO3(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_LOW, 
                "VideoPlayer::GetElapsedTime():time %d timeOffset %d nElapsedTime %d",
                lTime,timeOffset,nElapsedTime);

  uint32 videoTs = decIF.GetLastReadVideoFrameTimeStamp();
  if(videoTs != (uint32)-1) //If timestamp is defined
  {
     videoDataLead = videoTs +
                     decIF.GetAverageVideoFrameInterval() -
                     nElapsedTime;
     QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_MED, "Video Data Lead %d", videoDataLead);
  }
  if(videoDataLead > maxVideoDataLead)
  {
     maxVideoDataLead = videoDataLead;
  }
  if(videoDataLead < minVideoDataLead)
  {
     minVideoDataLead = videoDataLead;
  }
  return nElapsedTime;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::GetPosition

DESCRIPTION:
  Returns the position

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns the position

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::GetPosition(int32 &pos)
// get video frame processing position (frame count)
{
  // This may be called from the player thread at any time.
    decIF.GetPosition(pos);
    return true;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::GetStatistics

DESCRIPTION:
  Gets statistics for the video player

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if successful, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                                Common::LastFrame &lastFrame)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::GetStatistics" );

  // This may be called from the API thread at any time.
    decIF.GetStatistics(stat,lastFrame);
    stat.Video.maxDataLead = maxVideoDataLead;
    stat.Video.minDataLead = minVideoDataLead;
    return true;
}

////////////////////////////////////////////////////////////////////
//
// Create/Destroy
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::Destroy

DESCRIPTION:
  Forces an immediate shutdown and cleanup

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::Destroy()
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Destroy" );

  // Renderer is last in, first out.
  QTV_MSG_PRIO2(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,"guVideoPlayerInstanceCnt = %d VideoPlayer = 0x%x", guVideoPlayerInstanceCnt, (VideoPlayer *)this);

  if (1 == guVideoPlayerInstanceCnt) 
  {
      //kill the thread
      QCUtils::KillThread(&decoderTC);
  }

  //cleanup
  CleanupPlay();

#ifndef FEATURE_WINCE
  if ( m_pRenderer != NULL )
  {
    QTV_Delete( m_pRenderer );
    m_pRenderer = NULL;
  }
#endif
  //destroy embedded objects
  decIF.Destroy();

  //reset context to effectively disable player
  pAVPlayer = NULL;
}


/* ======================================================================
FUNCTION:
  VideoPlayer::Create

DESCRIPTION:
  Creates the video player

INPUT/OUTPUT PARAMETERS:
  pContext

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::Create
(
  void * pContext,
  QtvPlayer::MallocOutputBufferT MallocOutputBuffer,
  QtvPlayer::FreeOutputBufferT   FreeOutputBuffer,
  AVSync* pAVSync,
  QtvConfig *pQtvConfig
)
{
  QTV_BroadcastTransmitter* qtv_broadcaster_ptr =
    QTV_BroadcastTransmitter::Locate( "QTV" );

  QTV_MSG_PRIO2(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_HIGH,"VideoPlayer::Create guVideoPlayerInstanceCnt = %d VideoPlayer = %x", guVideoPlayerInstanceCnt, (VideoPlayer *)this);
  unsigned int preroll_render = 1;

  InitDefaults();

  //save the context.
  pAVPlayer = (Mpeg4Player *)pContext;

  m_pAVSync = pAVSync;

  m_pQtvConfigObject = pQtvConfig;

  decIF.Create( (void *)this,
                MallocOutputBuffer,
                FreeOutputBuffer,
                qtv_broadcaster_ptr,
                m_pAVSync );

  // Setup the renderer pre-roll for FRE here
  // after checking the config item member
  #ifdef FEATURE_FRE
  if (decIF.IsQFREActive())
  {
    preroll_render = 2;
  }
  #endif  

  //start the video thread
  if (!StartThread())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "VideoPlayer::Create Failed 2" );
    return false;
  }

  // Renderer is last in, first out.
  if ( m_pRenderer == NULL )
  {
    m_pRenderer = QTV_New_Args( qtv_vrend_class, ( decIF, preroll_render ) );

    if ( m_pRenderer == NULL )
    {
      QTV_MSG( MSG_LEGACY_FATAL,
               "VideoPlayer::Create couldn't construct renderer (out of memory?)"
             );
    }
  }
  else
  {
    QTV_MSG1( MSG_LEGACY_ERROR,
              "VideoPlayer::Create found non-NULL m_pRenderer! (%x)",
              m_pRenderer );
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//
// Video Player control
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::ResetData

DESCRIPTION:
  Resets all the data that can be retrieved by the AV player

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::ResetData()
{
  //init elapsed time
  nElapsedTime=0;
  //init position
  decIF.InitFrame();
  //init stats
  decIF.InitStatistics();
}

/* ======================================================================
FUNCTION:
  VideoPlayer::Prep

DESCRIPTION:
  Preps for video playback

INPUT/OUTPUT PARAMETERS:
  playbackID
  pMpeg4In - pointer to a Media object
  bRestart

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::Prep(const int playbackID, Media * pMpeg4In, bool &bError,
                       bool bRestart, Media::CodecType audioCodec)
{
  bError=false;

  if (CanPrep(bRestart))
  {
    if (!bRestart)
    {
      //save playback ID
      currentPlaybackID = playbackID;

      //get some clip info.
      pMpeg4=pMpeg4In;
      pBitfile = pAVPlayer ? pAVPlayer->GetBitfile(true) : NULL;
    }

    // It is inappropriate to reset our state-flags if we're simply
    // restarting from a pause.  Repositions set the bRestart flag
    // to false.
    //
    if ( !bRestart )
    {
      InitPlayData();
    }

    bool bCannotPrep;
    if (PrepPlay(bRestart, bCannotPrep, audioCodec))
    {
      //set flag which will allow startup.
      bPrepped=true;
    }
    else if (bCannotPrep)
    {
      //cannot prep, no error.
      bPrepped=true;
    }
    else
    {
      //error.
      AbortVideoOutput();
      CleanupPlay();
      Notify(IDLE, Common::VIDEO_ERROR_ABORT);
      bError=true;
    }
    return true;
  }
  else
  {
    QTV_MSG_PRIO2(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_ERROR,
      "Cannot prep: restart = %d, state = %d",
      bRestart ? 1 : 0, state);
  }
  return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::SetPlayTimes

DESCRIPTION:
  Set start and stop times.  Assume video is idle or suspended when
  this is called.

INPUT/OUTPUT PARAMETERS:
  start
  stop

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::SetPlayTimes(long start,long stop)
{
  if (start>=0)
  {
    decIF.SetPos( start );
    /* set new elaspe time to repositioned time */
    nElapsedTime=start;
    /* wakeup renderer task, so it can reject the frame it is sleeping for */
    TimingUpdate();
  }
  decIF.SetStopTime(stop);
}

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
bool VideoPlayer::Start(bool &bError,bool bRestart)
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Start" );

  bError=false;

  if (CanStart(bRestart))
  {
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
    if (!SendCommand(PLAY, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
      bError=true;
    }
    return true;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer::Start failed");
  return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::Stop

DESCRIPTION:
  Stop playing, don't return until really stopped

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::Stop(bool &bError)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Stop" );

  bError=false;

  bPlayingNotifyPending = FALSE;
#ifdef FEATURE_QTV_MDP
  bStopRendering = TRUE;
#endif /* FEATURE_QTV_MDP */

  if (!SendCommand(STOP, COMMON_VIDEO_STOP_TIMEOUT_MSEC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video stop response timeout");
    bError=true;
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::Pause

DESCRIPTION:
  Pause playing

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::Pause(bool &bError)
{
  bool bOk = true;
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Pause" );

  bError=false;

  if (CanPause())
  {
    bPlayingNotifyPending = FALSE;
    if (!SendCommand(PAUSE, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
      bError=true;
    }    
  }

  /* If the video state is not changed to paused then return false. */
  if(state != PAUSED )
  {
    bOk = false;
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "video pause failed");
  } 
  return bOk;
}

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/* ======================================================================
FUNCTION:
  VideoPlayer::Release

DESCRIPTION:
  Release video

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::Release(bool &bError)
{
  bError=false;

  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Pause releasing DSP" );

  if (!SendCommand(RELEASE_DSP, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
    bError=true;
  }
}
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */


/* ======================================================================
FUNCTION:
  VideoPlayer::Resume

DESCRIPTION:
  Resume playing

INPUT/OUTPUT PARAMETERS:
  bError
  stop

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::Resume(bool &bError,long stop)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Resume" );

  bError=false;

  if (CanResume())
  {
    decIF.SetStopTime(stop);
    if (!SendCommand(PLAY, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
      bError=true;
    }
    return true;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "video resume failed");
  return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::PlayNextFrame

DESCRIPTION:
  Play the next frame

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::PlayNextFrame(bool &bError)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::PlayNextFrame" );

  bError=false;

  if (CanResume())
  {
    if (!SendCommand(PLAY_NEXT_FRAME, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
      bError=true;
    }
    return true;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer::PlayNextFrame failed");
  return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::PlayPrevFrame

DESCRIPTION:
  Plays the previous frame

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::PlayPrevFrame(bool &bError)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::PlayNextFrame" );

  bError=false;

  if (CanResume())
  {
    if (!SendCommand(PLAY_PREV_FRAME, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
      bError=true;
    }
    return true;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer::PlayNextFrame failed");
  return false;
}

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
/* ======================================================================
FUNCTION:
  VideoPlayer::RotateVideo

DESCRIPTION:
  Rotates the video clip.

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.


INPUT/OUTPUT PARAMETERS:
  direction The direction to rotate the original orientation.

RETURN VALUE:
  Returns true on success, false on error
SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::RotateVideo(QtvPlayer::RotationType direction)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::RotateVideo" );
  rotateDirection = direction;
  if (SendCommand(ROTATE_VIDEO, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
  {
    return true;
  } else {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
    return false;
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::ScaleVideo

DESCRIPTION:
  Scales the video by the specified scaling factor

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.


INPUT/OUTPUT PARAMETERS:
  scaling: The absolute scaling factor.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::ScaleVideo(QtvPlayer::ScalingType scaling)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::ScaleVideo" );

  scalingFactor = scaling;
  if ((scalingFactor != QtvPlayer::SCALE_NONE) &&
      SendCommand(SCALE_VIDEO, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
  {
    return true;
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
    return false;
  }
}
#endif /*FEATURE_MP4_FRAME_TRANSFORMATIONS*/

#ifdef FEATURE_QTV_XSCALE_VIDEO
/* ======================================================================
FUNCTION:
  VideoPlayer::ScaleVideo

DESCRIPTION:
  Scales the video by the specified scaling factor

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.
  Depending on the input parameter this function shall choose the DSP xScalar.


INPUT/OUTPUT PARAMETERS:
  uint32 scaledOutFrameWidth: The scaled width in pixels.
  uint32 scaledOutFrameHeight: The scaled height in pixels.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::ScaleVideo(uint32 scaledOutFrameWidth,
                             uint32 scaledOutFrameHeight)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::ScaleVideo" );

  this->scaledOutFrameWidth = scaledOutFrameWidth;
  this->scaledOutFrameHeight = scaledOutFrameHeight;

  if ( SendCommand(XSCALE_VIDEO, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC) )
  {
    return true;
  }
  else
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
    return false;
  }
}
#endif /*FEATURE_QTV_XSCALE_VIDEO*/

#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
/* ======================================================================
FUNCTION:
  VideoPlayer::NotifyTransform

DESCRIPTION:
  Call the notify() function for the video status code

INPUT/OUTPUT PARAMETERS:
  code - the video status code

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::NotifyTransform(const Common::VideoStatusCode code)
{
  Notify(state, code);
}
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO*/

/* ======================================================================
FUNCTION:
  VideoPlayer::SetAudioOverride

DESCRIPTION:
  Sets the audio override

INPUT/OUTPUT PARAMETERS:
  audioFormat
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::SetAudioOverride(int audioFormat, bool &bError)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::SetAudioDub" );

  bError=false;

  AudOverrideFormat = audioFormat;
  if (!SendCommand(AUDIO_OVERRIDE_FORMAT, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video response timeout");
    bError=true;
  }
  return true;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::Suspend

DESCRIPTION:
  Suspends the video player

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::Suspend(bool &bError)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Suspend" );
  bError=false;
  if (CanSuspend())
  {
    if (!SendCommand(SUSPEND, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video suspend timeout");
      bError=true;
    }
    return true;
  }
  return false;
}


/* ======================================================================
FUNCTION:
  VideoPlayer::TimingUpdate

DESCRIPTION:
  AV player calls this when audio thread has made a timing update

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::TimingUpdate()
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::TimingUpdate waking renderer" );
  if ( m_pRenderer )
  {
    m_pRenderer->avsync();
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::Disable

DESCRIPTION:
  Disable video

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::Disable(bool &bError)
{
  bError = false;

  QTV_MSG(QTVDIAG_VIDEO_TASK, "VideoPlayer::Disable" );

  /* Check if pause is needed before disable */
  if (CanPause())
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, 
                 "Pausing video before disable");

    bPlayingNotifyPending = FALSE;
    if (!SendCommand(PAUSE, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                   "Video response timeout");
      bError=true;
    }
  }

  if (CanDisable())
  {
    if (!SendCommand(DISABLE_VIDEO, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer::Disable response timeout");
      bError = true;
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                  "VideoPlayer::Disable bad state (%d)", state);
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::Enable

DESCRIPTION:
  Enable video

INPUT/OUTPUT PARAMETERS:
  bError

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::Enable(bool &bError, Media::CodecType audioCodec)
{
  bError = false;

  QTV_MSG1( QTVDIAG_VIDEO_TASK, "VideoPlayer::Enable, %d", audioCodec );

  if (CanEnable())
  {
    decIF.SetAudioCodec(audioCodec);

    if (!SendCommand(ENABLE_VIDEO, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer::Enable: response timeout");
      bError = true;
    }
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer::Enable bad state (%d)",
      state);
    bError = true;
  }
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
#ifdef FEATURE_QTV_AVI
/* =============================================================================
FUNCTION:
  VideoPlayer::notifyExcessFrameDrop

DESCRIPTION:
  Notifies the excessive frame drop

==============================================================================*/
bool VideoPlayer::notifyExcessFrameDrop()
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::notifyExcessFrameDrop()" );

  if(!SendCommand(VIDEO_EXCESSIVE_FRAME_DROPS, COMMON_VIDEO_RESPONSE_TIMEOUT_MSEC))
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                 "VideoPlayer::notifyExcessFrameDrop() response timeout");
    return false;
  }
  return true;
}
#endif
////////////////////////////////////////////////////////////////////
//
// Private Routines
//
////////////////////////////////////////////////////////////////////


/* ======================================================================
FUNCTION:
  VideoPlayer::CanPause

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player can be paused.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::CanPause()
{
  return (state != IDLE &&
          state != SUSPENDED &&
          state != PAUSED_DISABLED);
}

/* ======================================================================
FUNCTION:
  VideoPlayer::CanSuspend

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player can be suspended
SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::CanSuspend()
{
  return (state != IDLE &&
          state != PAUSED_DISABLED);
}

/* ======================================================================
FUNCTION:
  VideoPlayer::CanPrep

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player can be prepped

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::CanPrep(bool bRestart)
{
  return ((bRestart && ((state==SUSPENDED)||(state==PAUSED)) )
          || (!bRestart && state==IDLE));
}

/* ======================================================================
FUNCTION:
  VideoPlayer::CanStart

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player can be started.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::CanStart(bool bRestart)
{
  return CanPrep(bRestart) && bPrepped;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::CanResume

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player can be resumed

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::CanResume()
{
  return (state==PAUSED
    && lastState!=IDLE
    && lastState!=SUSPENDED
    && lastState!=PAUSED);
}

/* ======================================================================
FUNCTION:
  VideoPlayer::CanDisable

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player can be disabed.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::CanDisable()
{
  return (state == PAUSED || state == SUSPENDED);
}

/* ======================================================================
FUNCTION:
  VideoPlayer::CanEnable

DESCRIPTION:
  None.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if the player can be enabled

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::CanEnable()
{
  return (state == PAUSED_DISABLED);
}

////////////////////////////////////////////////////////////////////
//
// Command interface to video thread.
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::CommandComplete

DESCRIPTION:
  Wake up command thread which may be waiting on a command response

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::CommandComplete()
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "Video Cmd Done - responseSync" );
  QCUtils::SetCondition(&responseSync);
}

/* ======================================================================
FUNCTION:
  VideoPlayer::SendCommand

DESCRIPTION:
  Send a command, wait on it to be consumed.

INPUT/OUTPUT PARAMETERS:
  newCommand
  timeout

RETURN VALUE:
  Return TRUE if it was consumed, FALSE if we timed out

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::SendCommand(Command newCommand, unsigned long timeout)
{
  QTV_MSG3( QTVDIAG_VIDEO_TASK, "VideoPlayer::SendCommand(%d), timeout(%d), state(%d)",
            newCommand, timeout, state );

  // Figure out if this command is likely to result in a state change.
  // The renderer will look at this when deciding if it should render frames.
  //
  m_bIsStateChangePending = WillCommandCauseStateChange( state, newCommand );

  //clear any old abort signal that has been processed.
  if (bAbort && state==IDLE)
  {
    bAbort=false;
  }

  // Allocate a new command structure
  CommandQElem *pElem = (CommandQElem*) QTV_Malloc(sizeof(CommandQElem));
  if( pElem == NULL )
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unable to alloc command for video task" );
     return false;
  }
  // Set the command fields
  (void)q_link( pElem, &pElem->link );
  pElem->cmdEvent.cmd = newCommand;
  pElem->cmdEvent.pOwner = (uint32)this;

  QTV_MSG2( QTVDIAG_VIDEO_TASK, "VideoPlayer::SendCommand (%d) Owner ( 0x%x)",pElem->cmdEvent.cmd,pElem->cmdEvent.pOwner );

  //queue the command
  QCUtils::EnterCritSect(&m_commandQ_CS);
  q_put( &m_commandQ, &pElem->link );
  QCUtils::LeaveCritSect(&m_commandQ_CS);

  //handshake with the thread.
  if( timeout > 0 )
  {
    QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::SendCommand resetting response sync" );
    QCUtils::ResetCondition(&responseSync);
  }

  //wake up video thread
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::SendCommand setting wakeup sync" );
  QCUtils::SetCondition(&wakeupSync);

  bool bOK = true;
  if( timeout > 0 )
  {

    QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::SendCommand waiting for response sync" );
    bOK = QCUtils::WaitForCondition(&responseSync,timeout);

    if(newCommand==STOP && !IsDone())
    {
      bAbort = true;
      WakeRenderer();
    }
   
  }

  //if a stop command timed out, set the abort signal
  //so the thread will catch it eventually.
  if( !bOK && newCommand==STOP )
  {
    bAbort=true;
  }

  //cleanup
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::SendCommand done" );

  return bOK;
}

////////////////////////////////////////////////////////////////////
//
// Debug stuff
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::ShowState

DESCRIPTION:
  Show the state.

INPUT/OUTPUT PARAMETERS:
  input - the state

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::ShowState(State input)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::ShowState" );

  switch (input)
  {
  case REBUFFERING:
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "Video state REBUFFERING %d ",ZUtils::Clock());
    break;
  case BUFFERING:
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "Video state BUFFERING %d",ZUtils::Clock());
    break;
  case PLAYING:
    QTV_MSG2( QTVDIAG_VIDEO_TASK, "Video state PLAYING, %d ID %d",ZUtils::Clock(),currentPlaybackID);
    break;
  case PAUSED:
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "Video state PAUSED %d",ZUtils::Clock());
    break;
  case IDLE:
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "Video state IDLE %d",ZUtils::Clock());
    break;
  case SUSPENDED:
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "Video state SUSPENDED %d",ZUtils::Clock());
    break;
  case PAUSED_DISABLED:
    QTV_MSG1( QTVDIAG_VIDEO_TASK, "Video state PAUSED_DISABLED %d",ZUtils::Clock());
    break;
  }
}

////////////////////////////////////////////////////////////////////
//
// Start video thread.
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::StartThread

DESCRIPTION:
  This will start the thread in case it has not yet been started
  or it died due to hitting an exception.
  It also reverts the state to IDLE.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::StartThread()
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::StartThread" );

  state=IDLE;
  lastState=IDLE;
  lastStateWhenDisabled=IDLE;
  bAbort=false;

  if (!QCUtils::IsThreadActive(&decoderTC))
  {
     if (pAVPlayer)
     {
         QCUtils::ResetCondition(&wakeupSync);
         decoderTC.priority = QTVDecoderPriority;
         QTV_MSG( QTVDIAG_VIDEO_TASK, "Creating Decoder thread" );
         if (!QCUtils::CreateThread(&decoderTC, (unsigned long)this, false))
         {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer:Start thread failed 1");
            return false;
         }
     }
  }

  if (!QCUtils::IsThreadActive(&decoderTC))
  {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoPlayer::StartThread failed 2");
      return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////
//
// Data init and cleanup
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::CleanupPlay

DESCRIPTION:
  Cleanup after playback

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::CleanupPlay(void)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::CleanupPlay" );

  if (1 == guVideoPlayerInstanceCnt)
  {
     // Clear out residual commands from the command queue
      QCUtils::EnterCritSect( &m_commandQ_CS );
      CommandQElem *pElem;
      while( (pElem = (CommandQElem*) q_get(&m_commandQ)) != NULL )
      {
        QTV_Free(pElem);
      }
      QCUtils::LeaveCritSect( &m_commandQ_CS );
  }
  Flush();
  decIF.CleanupPlay();

  pMpeg4=NULL;
  pBitfile=NULL;

  bPrepped=false;

#ifdef FEATURE_QTV_M4V
#error code not present
#endif

}

/* ======================================================================
FUNCTION:
  VideoPlayer::InitDefaults

DESCRIPTION:
  Init data for each time player is created.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::InitDefaults()
//init data for each time player is created.
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::InitDefaults" );

  currentPlaybackID = 0;
  durationBuffered = 0;

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

  nElapsedTime=0;
  InitPlayData();

  checkTimeFlag = false;
  bPlayingNotifyPending = FALSE;
#ifdef FEATURE_QTV_MDP
  bStopRendering = FALSE;
#endif /* FEATURE_QTV_MDP */
  VideoAborted = false;

  m_playbackSpeedNum = 1;
  m_playbackSpeedDen = 1;

  videoDataLead = 0;
  maxVideoDataLead = 0;
  minVideoDataLead = 0;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::InitPlayData

DESCRIPTION:
  Init data for start or restart

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::InitPlayData(void)
//init data for start or restart.
{
  //once per play
  bPrepped=false;
  bPlayDoneRendering=false;
  bPlayDoneDecoding=false;
  nLastTimeStamp = 0;
#ifdef FEATURE_QTV_MDP
  bStopRendering = false;
#endif /* FEATURE_QTV_MDP */
  streamEndFlag = false;
}

////////////////////////////////////////////////////////////////////
//
// Internal play transitions
//
////////////////////////////////////////////////////////////////////
#ifdef FEATURE_H264_DECODER
/* ======================================================================
FUNCTION:
  VideoPlayer::PreParseHeader

DESCRIPTION:
Pre-parse the H.264 header

INPUT/OUTPUT PARAMETERS:

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::PreParseHeader(Media *pMpeg4In, bool bRestart,
                                 Media::CodecType audioCodec)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::PreParseHeader");
  if (!decIF.PreParseHeader( pMpeg4In, bRestart,audioCodec ) )
  {
    AbortVideoOutput();
    CleanupPlay();
    Notify(IDLE, Common::VIDEO_ERROR_ABORT);
    return false;
  }
  return true;
}
#endif /* FEATURE_H264_DECODER */
/* ======================================================================
FUNCTION:
  VideoPlayer::PrepPlay

DESCRIPTION:
  Prep playback

INPUT/OUTPUT PARAMETERS:
  bRestart
  bCannotPrep

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::PrepPlay(bool bRestart, bool &bCannotPrep, Media::CodecType audioCodec)
{
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "VideoPlayer::PrepPlay %d", bRestart );
  /*  if ( bRestart )
  return true;
  */
  if (decIF.PrepPlay(bCannotPrep,pMpeg4,pBitfile,bRestart,audioCodec))
  {
    if (InitImage())
    {
      return true;
    }
  }
  return false;
}


/* ======================================================================
FUNCTION:
  VideoPlayer::InitImage

DESCRIPTION:
  Do all the init that depends on the image dimensions.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::InitImage()
{
  if (pAVPlayer)
  {
    pAVPlayer->SetImageDimensions(decIF.GetWidth(),decIF.GetHeight());
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////
//
// Video Thread
//
////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////
//
// video thread routines
//
///////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::DecodeOneFrame

DESCRIPTION:
  Decodes and displays one frame

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::DecodeOneFrame( bool& bSleepAfter )
{
  //decode and display
  bool bContinue = decIF.DecodeOneFrame( bSleepAfter );

#ifdef FORCE_REBUFFERING
  if (bContinue)
  {
    ForceRebuffering();
  }
#endif

  return bContinue;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::ShowNextFrame

DESCRIPTION:
  Shows the next frame

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::ShowNextFrame()
{
  if(!decIF.ShowNextFrame() )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Can't show next frame");
    return false;
  }
  return true;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::ShowPrevFrame

DESCRIPTION:
  Shows the previous frame

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::ShowPrevFrame()
{
  if(!decIF.ShowPrevFrame() )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Can't get prev frame");
    return false;
  }
  return true;
}

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
/* ======================================================================
FUNCTION:
  VideoPlayer::VideoRotation

DESCRIPTION:
  Rotates the video

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

  If the player is not playing, we can't do scaling, so return false.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::VideoRotation()
{
  if (!IsPlaying() || !decIF.RotateVideo(rotateDirection) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Can't rotate video");
    return false;
  }
  return true;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::VideoScaling

DESCRIPTION:
  Scales the video

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

  If the player is not playing, we can't do scaling, so return false.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::VideoScaling()
{
  if(!IsPlaying() || !decIF.ScaleVideo(scalingFactor) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Can't scale video");
    return false;
  }
  return true;
}
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_XSCALE_VIDEO
/* ======================================================================
FUNCTION
  ScaleVideoCallback

DESCRIPTION
  Callbacks for handling the delayed status of xScale video cmd. It shall
  notify the upper layer depending on the stuatus returned

RETURN VALUE
  None, it's a callback.
========================================================================== */
void VideoPlayer::ScaleVideoCallback
(
  const VDEC_STREAM_ID      /*unused*/,
  const boolean             scaleDone,
  void * const              pCbData
)
{
  VideoPlayer * const pThis = ( VideoPlayer * )pCbData;
  ASSERT( pThis );


  if(scaleDone)
  {
    pThis->NotifyTransform(Common::VIDEO_SCALING_DONE);
  }
  else
  {
    pThis->NotifyTransform(Common::VIDEO_SCALING_FAILED);
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::VideoXScaling

DESCRIPTION:
  Scales the video via the DSP xScalar module. We also register the callback
  ScaleVideoCallback which gets called with the status for the scaling command.

  This function follows the general rule for setting parameters:
  The function is responsible for ensuring the callback is notified if it
  returns success or has no return value. Otherwise, no callback is notified.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::VideoXScaling()
{
  if(!decIF.ScaleVideo(scaledOutFrameWidth,scaledOutFrameHeight,
                       ScaleVideoCallback,this) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Can't xScale video");
  }
}
#endif /* FEATURE_QTV_XSCALE_VIDEO */

/* ======================================================================
FUNCTION:
  VideoPlayer::SetAudioOverridePriv

DESCRIPTION:
  Sets the audio override format

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::SetAudioOverridePriv()
{
  if(!decIF.SetAudioOverrideFormat(AudOverrideFormat) )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Can't set audio override");
    return false;
  }
  return true;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::DisplayFrame

DESCRIPTION:
  Displays one frame

INPUT/OUTPUT PARAMETERS:
  pImage - image buffer, or NULL for fixed image buffer
  nPresTime - presentation time
  bNoDrop - don't drop
  nPresFrame

RETURN VALUE:
  DisplayFrame returns 'true' if the frame was consumed (displayed
  or dropped) and false if it was not consumed.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::DisplayFrame(
  VDEC_FRAME& frame,
  const long nPresTime,
  const bool /* bNoDrop */,
  const unsigned long nPresFrame
#ifdef FEATURE_QTV_MDP
  ,bool &bFreeFrame
#endif /* FEATURE_QTV_MDP */
  )
{
  boolean            bDisplayResult = TRUE;

#ifdef FEATURE_QTV_MDP
  // Default to freeing the frame in case of an early return.
  bFreeFrame = true;
#endif /* FEATURE_QTV_MDP */

  /* If we are not rendering(as in case of repositioning in progress),
     just drop this frame without updating elaspe time */
  if(!decIF.IsRendering())
  {
    return false;
  }
  /*
  ** Keep track of timestamp range.
  ** We have to do this AFTER we sleep or we'll end up setting the elapsed time
  ** forward to the time of a frame to be displayed in the future.  Bad.
  */
  nElapsedTime = nPresTime;

  if (bPlayingNotifyPending)
  {
    if (state == PLAYING)
    {
      Notify(PLAYING, Common::VIDEO_PLAYING);
    }
    bPlayingNotifyPending = FALSE;
  }

  decIF.UpdateStats(VideoDec::DISPLAYED);

  //
  //  Display
  //

  // Update video time stats
  decIF.FrameDisplayed(nPresFrame,nPresTime,ZUtils::Clock());

#ifdef SHOW_FRAMES
  QTV_MSG2( QTVDIAG_VIDEO_TASK, "video disp f %d pres %d",nPresFrame,nPresTime);
#endif

#ifdef FEATURE_MPEG4_SHOW_RGB_BUFFERS
#error code not present
#endif

#ifdef FEATURE_QTV_MDP

  /* If the STOP command has been issued, don't display video frame */
  if ( !bStopRendering )
  {
#endif /* FEATURE_QTV_MDP */

    bDisplayResult =
      pAVPlayer->SetFrameInfo( frame.pBuf,
                               frame.pBuf,
                               frame.dimensions.height,
                               frame.dimensions.width,
                               nPresTime,
                                  frame.extFrame.numConcealedMbs,
                                  ( frame.frameType == VDEC_FRAMETYPE_I ? true : false ),
                                  (void *)frame.extFrame.pPostFilterMbInfo,
                                  frame.extFrame.numIntraMbs,
                                  frame.pMetaData,
                                  frame.cwin.x2,
                                  frame.cwin.y2);
#ifdef FEATURE_QTV_MDP
    // Check if we actually displayed the frame
    if(bDisplayResult != FALSE )
    {
      // don't need to free, this will be returned to us
      bFreeFrame = false;
    }
    else
    {
      // SetFrameInfo failed, free ourselves
      bFreeFrame = true;
    }
  } // if !bStopRendering
#endif /* FEATURE_QTV_MDP */

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 || 
          FEATURE_QTV_IN_CALL_VIDEO */

  return ( bDisplayResult != FALSE );
}

#ifdef FORCE_REBUFFERING
/* ======================================================================
FUNCTION:
  VideoPlayer::ForceRebuffering

DESCRIPTION:
  Test code to force rebuffering at arbitrary points

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::ForceRebuffering(void)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::ForceRebuffering" );

  if (pMpeg4)
  {
    if (pMpeg4->GetSource()==Media::STREAM_SOURCE)
    {
      if (nFrame%50==0)
      {
        if (pAVPlayer)
        {
          pAVPlayer->ForceRebuffering();
        }
      }
      if (nFrame>100)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "WARNING: Exiting early");
        overallDataState = Media::DATA_ERROR;
        return false;
      }
    }
  }
}
#endif

/* ======================================================================
FUNCTION:
  VideoPlayer::StartPlaying

DESCRIPTION:
  Going from buffering to playing

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::StartPlaying(void)
// Going from Buffering to Playing
{
  InitPlayData();
  if (decIF.StartPlaying())
  {
    if (InitImage())
    {
      return true;
    }
  }
  return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::SetState

DESCRIPTION:
  State transition

INPUT/OUTPUT PARAMETERS:
  newState

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::SetState(State newState)
{
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "VideoPlayer::SetState, newState=%d", newState );

  if (state == newState)
  {
    QTV_MSG1( QTVDIAG_VIDEO_TASK,
             "VideoPlayer::SetState is already in state %d", newState );
    return;
  }

  // Show new state
  ShowState(newState);

  // Go to new state.
  lastState=state;
  state=newState;
  m_bIsStateChangePending = false;

  switch ( newState )
  {
  case PLAYING:
  case PAUSED:
    // If we've entered the PLAYING state, we need to goose the renderer
    // back into life.
    //
    QTV_MSG( QTVDIAG_VIDEO_TASK, "Waking renderer due to state change" );
    WakeRenderer();
    break;

  case IDLE:
    //cleanup when going idle
    CleanupPlay();
    break;

  default:
    break;
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::Notify

DESCRIPTION:
  Send video status to the player thread and do a state transition.

INPUT/OUTPUT PARAMETERS:
  newState
  status

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::Notify(
    State newState,
    Common::VideoStatusCode status)
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::Notify" );

  // Note: the order is important here.  Need to save
  // the playback info, then change state, then notify
  // the AV player.

  int playbackID = currentPlaybackID;
  int nVideoFrames = decIF.GetFrameCount();

  //change state
  SetState(newState);


  //Notify player.
  if (pAVPlayer)
  {
    if (pAVPlayer->IsThreadActive())
    {
      VIDEO_STATUS_type *pEvent = QCCreateMessage(VIDEO_STATUS, pAVPlayer); /*lint !e641 */
      if (pEvent)
      {
        pEvent->playbackID = playbackID;
        pEvent->status = status;
        pEvent->nVideoFrames = nVideoFrames;
        pEvent->durationBuffered = durationBuffered;

        if((REBUFFERING == newState) && HasAudio())
        {
          uint32 audioDurationBuffered = 0;

          if (pMpeg4 && (pMpeg4->GetSource()==Media::STREAM_SOURCE))
          {
             audioDurationBuffered   = pMpeg4->GetAudioDurationAvailable();
             pEvent->durationBuffered=
                               MIN(audioDurationBuffered,durationBuffered);
          }
        }

        pAVPlayer->PostMessage(pEvent);
      }
    }
  }
}


/* ======================================================================
FUNCTION:
  VideoPlayer::IsDataAvailable

DESCRIPTION:
  polling function, return true when data available

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true on success, false on error

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::IsDataAvailable()
{
  if (pMpeg4)
  {
    if (pMpeg4->IsVideoAvailable())
    {
      return true;
    }
  }
  else if (pBitfile)
  {
    //there's no buffering for bitstream files, so assume
    //data is available now.
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION
  VideoPlayer::GetVideoDurationAvailable

DESCRIPTION
 Returns the duration of buffered video in msec

DEPENDENCIES
 None

RETURN VALUE
 duration of buffered video in msec

SIDE EFFECTS
 None

========================================================================== */
uint32 VideoPlayer::GetVideoDurationAvailable()
{
  if (pMpeg4)
  {
    return pMpeg4->GetVideoDurationAvailable();
  }
  return 0;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::TrackHasEnded

DESCRIPTION:
  if video track has ended.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  Returns true if video track has ended or false, if not.

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::TrackHasEnded()
{
  Media::MediaStatus status = decIF.GetDataState();

  //DATA_ERROR case
  if(((status == Media::DATA_ERROR) && (state==IDLE)) ||
     /* DATA_END case. Earlier, state was not IDLE hence this function was returning FALSE. 
     Now it will return TRUE and we will not allow repositioning */
     ((status==Media::DATA_END) && (bPlayDoneRendering)))
  {
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::PauseVideoOutput

DESCRIPTION:
  Pauses the video output

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::PauseVideoOutput()
{
  if (!bPlayDoneRendering)
  {
    decIF.EndPlayInterval(ZUtils::Clock());
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::SetResumeTime

DESCRIPTION:
  Sets the Resume Time for PlayMsec calculations

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::SetResumeTime()
{
  decIF.StartPlayInterval(ZUtils::Clock());
}

/* ======================================================================
FUNCTION:
  VideoPlayer::SetPlayedbackTime

DESCRIPTION:
  Calculates and sets the Total played back time.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::SetPlayedbackTime()
{
  if (!bPlayDoneRendering)
  {
    decIF.EndPlayInterval(ZUtils::Clock());
  }
}

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/* ======================================================================
FUNCTION:
  VideoPlayer::ReleaseDSP

DESCRIPTION:
  Releases the DSP resources

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  true if DSP was released, false otherwise

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::ReleaseDSP()
{
  return decIF.ReleaseDSP();
}
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */


/* ======================================================================
FUNCTION:
  VideoPlayer::DisableVideo

DESCRIPTION:
  Disables video, releases DSP resources

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  true if video disabled, false otherwise

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::DisableVideo()
{
  return decIF.Disable();
}

/* ======================================================================
FUNCTION:
  VideoPlayer::EnableVideo

DESCRIPTION:
  Enables video, restores DSP resources

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  true if video enabled, false otherwise

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::EnableVideo()
{
  return decIF.Enable();
}

/* ======================================================================
FUNCTION:
  VideoPlayer::ResumeVideoOutput

DESCRIPTION:
  Resumes the video output

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::ResumeVideoOutput( bool clearState )
{
  if (clearState)
  {
    decIF.InitDataState();
  }
  if (!bPlayDoneRendering)
  {
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    if ( qtv_cfg_enable_dsp_release )
    {
      decIF.RestoreDSP();
    }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
#ifndef FEATURE_QTV_GENERIC_BCAST
    if (!(Media::BCAST_FLO_SOURCE == pMpeg4->GetSource() ||
         Media::BCAST_ISDB_SOURCE == pMpeg4->GetSource() ))
#else
    if (IsFileBcastMediaSource(pMpeg4,m_pAVSync))
#endif
    {
    decIF.StartPlayInterval(ZUtils::Clock());
    }
    
  }
}


/* ======================================================================
FUNCTION:
  VideoPlayer::StopVideoOutput

DESCRIPTION:
  Stops the video output

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::StopVideoOutput()
{
  //this can be called any time, so check the state.
  switch(state)
  {
  case PLAYING:
  case PAUSED:
  case REBUFFERING:
  case BUFFERING:
    if (!bPlayDoneRendering)
    {
      PauseVideoOutput();

      decIF.EndPlay();
    }
    break;
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::AbortVideoOutput

DESCRIPTION:
  Abort the video output

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::AbortVideoOutput(bool bError)
{
  StopVideoOutput();

  if (pMpeg4 != NULL)
  {
    pMpeg4->AbortVideo(bError);
    VideoAborted = true;
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::SuspendVideoOutput

DESCRIPTION:
  Suspends the video output

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::SuspendVideoOutput()
{
  //this can be called any time, so check the state.
  switch(state)
  {
  case PLAYING:
  case PAUSED:
  case REBUFFERING:
    if (!bPlayDoneRendering)
    {
      PauseVideoOutput();
    }
    break;
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::StartVideoOutput

DESCRIPTION:
  Starts the video output

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::StartVideoOutput()
{
  decIF.InitDataState();
  decIF.StartPlayInterval(ZUtils::Clock());
}

/* ======================================================================
FUNCTION:
  VideoPlayer::StartRendering

DESCRIPTION:
  Video decode interface calls this when it's time to start rendering.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::StartRendering()
{
  //notify AV Player
  Notify(state,Common::VIDEO_RENDERING);

}


/* ======================================================================
FUNCTION:
  VideoPlayer::ProcessAllCommands

DESCRIPTION:
  Process commands.  Commands are likely to generate a state change.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
uint32 VideoPlayer::ProcessAllCommands( void )
{
  CommandQElem *pCommand = NULL;
  VideoPlayer  *pThis  = NULL;

  QCUtils::EnterCritSect( &m_processCommands_CS );

  /* Process the command queue until it is empty.
  ** This is important, because of the following scenario:
  ** 1) mpeg4player sets up some state information, initializes
  **    videoplayer, and sends a command to videoplayer, which
  **    puts mpeg4player to sleep.
  ** 2) videoplayer wakes up and processes the command.
  **    It calls CommandComplete, which wakes Mpeg4Player back up,
  **    because Mpeg4Player is at a higher priority.
  ** 3) Mpeg4Player decides to change state again, tears down
  **    classes or structures it previously used to initialize
  **    VideoPlayer, and sends VideoPlayer a STOP (or something similar).
  ** 4) VideoPlayer wakes back up, but has finished processing
  **    the commands and doesn't know about the state change.
  **    VideoPlayer begins trying to process data, and stumbles onto
  **    invalid data and bad pointers.
  **
  ** Looping over the command queue ensures that VideoPlayer
  ** is aware of the correct state of affairs before it does any processing.
  */
  for
  (
    pCommand = GetNextQueuedCommand();
    pCommand != NULL;
    pCommand = GetNextQueuedCommand()
  )
  {
    if ( pCommand->cmdEvent.cmd != NONE )
    {
      pThis = (VideoPlayer *)pCommand->cmdEvent.pOwner;

      QTV_MSG2( QTVDIAG_VIDEO_TASK,
                "VideoPlayer::DecoderThread processing command %d Owner 0x%x",
                pCommand->cmdEvent.cmd, pCommand->cmdEvent.pOwner);

      pThis->ProcessOneCommand( pCommand->cmdEvent.cmd );
      pThis->CommandComplete();
    }
    else
    {
      QTV_MSG( QTVDIAG_VIDEO_TASK,
               "VideoPlayer::DecoderThread got NONE command" );
    }

    QTV_Free(pCommand);
  }

  QCUtils::LeaveCritSect( &m_processCommands_CS );

  return ((uint32)pThis);
}

/* ======================================================================
FUNCTION:
  VideoPlayer::ProcessOneCommand

DESCRIPTION:
  Process just one command.  Commands are likely to generate a state change.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::ProcessOneCommand
(
  const Command command
)
{
  //catch the abort signal
  if (bAbort && state!=IDLE)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Video Abort Signaled" );
    AbortVideoOutput(false);
    Notify(IDLE,Common::VIDEO_ERROR_ABORT);
    return ;
  }

  //process commands.
  switch (command)
  {
  case STOP:
    StopVideoOutput();
    if (state!=IDLE)
    {
      Notify(IDLE,Common::VIDEO_STOPPED);
    }
    break; /* command == STOP */

  case SUSPEND:
    if (CanSuspend())
    {
      SuspendVideoOutput();
      if (state!=SUSPENDED)
      {
        Notify(SUSPENDED,Common::VIDEO_SUSPENDED);
      }
    }
    break; /* command == SUSPEND */

  case PLAY:
    switch (state)
    {
    case SUSPENDED:
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
      if ( qtv_cfg_enable_dsp_release )
      {
        decIF.RestoreDSP();
      }
      // Intentional fall-through
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
    case IDLE:
      //goto buffering.
      durationBuffered = pMpeg4->GetVideoDurationAvailable();
      Notify(BUFFERING,Common::VIDEO_BUFFERING);
      break; /* command == PLAY, state == IDLE || SUSPENDED */

    case PAUSED:
      if (CanResume())
      {
        switch (lastState)
        {
        case PLAYING:

          if(IsDataAvailable())
          {
            //Resume playing
            ResumeVideoOutput(false);
            Notify(PLAYING,Common::VIDEO_PLAYING);
          }
          else
          {
            /* go to buffering */
            durationBuffered = pMpeg4->GetVideoDurationAvailable();
            Notify(BUFFERING,Common::VIDEO_BUFFERING);
          }
          break;
        case BUFFERING:
        case REBUFFERING:
          //Resume buffering
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
          if (qtv_cfg_enable_dsp_release)
          {
            decIF.RestoreDSP();
          }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
          durationBuffered = pMpeg4->GetVideoDurationAvailable();
          Notify(lastState,Common::VIDEO_BUFFERING);
          break;
        }
      }
      break; /* command == PLAY, state == PAUSED */
    }
    break; /* command == PLAY */

  case PAUSE:
    if (CanPause())
    {
      PauseVideoOutput();
      if (state!=PAUSED)
      {
        Notify(PAUSED,Common::VIDEO_PAUSED);
      }
    }
    break; /* command == PAUSE */

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  case RELEASE_DSP:
    if( ReleaseDSP() )
    {
      Notify(state,Common::VIDEO_RELEASE_DONE);
    }
    else
    {
      Notify(state,Common::VIDEO_RELEASE_FAILED);
    }
    break;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

  case DISABLE_VIDEO:
    if (CanDisable())
    {
      if( DisableVideo() )
      {
        // store the last state before transition to disabled
        lastStateWhenDisabled = lastState;
        Notify(PAUSED_DISABLED,Common::VIDEO_DISABLE_DONE);
      }
      else
      {
        Notify(state,Common::VIDEO_DISABLE_FAILED);
      }
    }
    break; /* command == DISABLE_VIDEO */

  case ENABLE_VIDEO:
    if (CanEnable())
    {
      if( EnableVideo() )
      {
        Notify(PAUSED,Common::VIDEO_ENABLE_DONE);
        // restore last state to its pre-disabled value (required for resume)
        lastState = lastStateWhenDisabled;
      }
      else
      {
        Notify(state,Common::VIDEO_ENABLE_FAILED);
      }
    }
    break;  /* command == ENABLE_VIDEO */

  case PLAY_NEXT_FRAME:
    if (CanResume())
    {
      if( ShowNextFrame() )
      {
        Notify(PAUSED,Common::VIDEO_NEXT_FRAME_DONE);
      }
      else
      {
        Notify(PAUSED,Common::VIDEO_NEXT_FRAME_FAILED);
      }
    }
    break; /* command == PLAY_NEXT_FRAME */

  case PLAY_PREV_FRAME:
    if (CanResume())
    {
      if( ShowPrevFrame() )
      {
        Notify(PAUSED,Common::VIDEO_PREV_FRAME_DONE);
      }
      else
      {
        Notify(PAUSED,Common::VIDEO_PREV_FRAME_FAILED);
      }
    }
    break; /* command == PLAY_PREV_FRAME */

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  case ROTATE_VIDEO:
    if( ! VideoRotation() )
    {
      // The general rule implies that if VideoRotation() returns false, no
      // callback notification will take place. Since there is no
      // return value for this function, we need to do the notification
      // ourselves.
      Notify(state,Common::VIDEO_ROTATION_FAILED);
    }
    break; /* command == ROTATE_VIDEO */

  case SCALE_VIDEO:
    if( ! VideoScaling() )
    {
      // The general rule implies that if VideoScaling() returns false, no
      // callback notification will take place. Since there is no
      // return value for this function, we need to do the notification
      // ourselves.
      Notify(state,Common::VIDEO_SCALING_FAILED);
    }
    break; /* command == SCALE_VIDEO */
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#ifdef FEATURE_QTV_XSCALE_VIDEO
  case XSCALE_VIDEO:
    VideoXScaling();
    break; /* command == XSCALE_VIDEO */

#endif /* FEATURE_QTV_XSCALE_VIDEO */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

  #ifdef FEATURE_QTV_AVI
  case VIDEO_EXCESSIVE_FRAME_DROPS:
    if(state == PLAYING)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, 
                   "Excessive Frame Drops: Moving to the next IFrame" );

      if((pMpeg4 != NULL)&&(pMpeg4->GetSource() == Media::FILE_SOURCE))
      {
        // skipNSync Frames here. Not sure if the media is available here.
        // May need to add the parameters
        if(pMpeg4->isAviFileInstance())
        {
         uint32 vtrackid = pMpeg4->GetVideoTrackID(0);
         FileBase* baseptr = pMpeg4->GetFile();
         if(baseptr)
         {
           bool bError = false;
           uint32 nextkeyTS = 
                    baseptr->skipNSyncSamples(
                             1,vtrackid,&bError,
                             baseptr->getMediaTimestampForCurrentSample(vtrackid));
    
           QTV_MSG_PRIO1( QTVDIAG_VIDEO_TASK,
                QTVDIAG_PRIO_DEBUG,
                "skipNSyncSamples nextkeyTS is %ld",nextkeyTS);
        
         }
        }
      }
      m_pAVSync->ResetPlaybackOffset(AVSync::VideoAV); // Reset Offset
    }
    break;
  #endif
  case AUDIO_OVERRIDE_FORMAT:
    (void)SetAudioOverridePriv();
    break; /* command == AUDIO_OVERRIDE_FORMAT */
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::GetNextQueuedCommand

DESCRIPTION:
  Returns the command structure from the head of the command queue,
  if one exists.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  CommandQElem*: a pointer to the command queue element returned from
  the command queue, or NULL if the queue was empty.

SIDE EFFECTS:
  None.
======================================================================*/
VideoPlayer::CommandQElem* VideoPlayer::GetNextQueuedCommand( void )
{
  CommandQElem *pCommand = NULL;

  QCUtils::EnterCritSect( &m_commandQ_CS );
  pCommand = (CommandQElem*) q_get( &m_commandQ );
  QCUtils::LeaveCritSect( &m_commandQ_CS );

  return pCommand;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::VideoThread

DESCRIPTION:
  Internal video thread routine.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::DecoderThread(void)
{
  //QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::DecoderThread" );

    // elapsed time check for mpeg4player
    if (checkTimeFlag)
    {
      if ((unsigned long)GetElapsedTime() >= checkTimeValue)
      {
        Notify(state, Common::VIDEO_ELAPSE_TIME);
        checkTimeFlag = false;
      }
    }

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

    switch (state)
    {
    case IDLE:
    case PAUSED:
    case SUSPENDED:
    case PAUSED_DISABLED:

      #ifdef FEATURE_QTV_DRM_DCF
      if( (m_bIsVideoTaskRegisteredForIPC == true) && (state == IDLE) )
      {
        //when done, de-register with ixipc.
        ixipc_terminate();
         m_bIsVideoTaskRegisteredForIPC = false;
      }
      #endif

      //
      // Wait for commands.
      //
      QTV_MSG( QTVDIAG_VIDEO_TASK,
               "VideoPlayer::DecoderThread sleeping (IDLE/PAUSED/SUSPENDED)" );
      (void)QCUtils::WaitForCondition(&wakeupSync,COMMON_IDLE_WAIT_TIMEOUT_MSEC);
      break;

    case BUFFERING:
    case REBUFFERING:

      if(bPlayDoneRendering == true)
      {
        StopVideoOutput();
        Notify(SUSPENDED,Common::VIDEO_DONE);
      }
      else if(decIF.GetDataState() == Media::DATA_ERROR) /* check if we encounter data error */
      {
        //error ending
        AbortVideoOutput();
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "video Data Error");
        Notify(IDLE, Common::VIDEO_ERROR_ABORT);
      }
      else if (!IsDataAvailable())  // Poll for data
      {
        static uint8 count = 0;
        if (++count % 5 == 0)
        {  /* Limit Max "Buffering" notifications to 4 per second */
          durationBuffered = GetVideoDurationAvailable();

          if(Media::DATA_UNDERRUN == decIF.GetDataState())
          {
            Notify(REBUFFERING,Common::VIDEO_BUFFERING);
          }
          else
          {
            Notify(BUFFERING,Common::VIDEO_BUFFERING);
          }

        }

        //sleep before polling again.
        QTV_MSG( QTVDIAG_VIDEO_TASK,
                 "VideoPlayer::DecoderThread sleeping (BUFFERING/REBUFFERING)" );
        (void)QCUtils::WaitForCondition(&wakeupSync,COMMON_BUFFERING_POLL_TIME_MSEC);
#ifdef SHOW_POLLING
        //message("Vid poll");
#endif
      }
      else
      {
        // Data is available
        // Start or resume playing.

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

        switch (state)
        {
        case BUFFERING:
          //Start playing.
          if (StartPlaying())
          {
            SetState(PLAYING);
            bPlayingNotifyPending = TRUE;
            StartVideoOutput();
          }
          else
          {
            //can't startup
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "video Init Play failed");
            Notify(IDLE, Common::VIDEO_ERROR_ABORT);
          }
          break;

        case REBUFFERING:
          //Resume playing.
          ResumeVideoOutput(true);
          SetState(PLAYING);
          bPlayingNotifyPending = TRUE;
          break;
        }
      }
      break;

    case PLAYING:
      if (bPlayDoneRendering)
      {
        //Done decoding and displaying, but last frame
        //is still up.  This is like an Idle state.
        //Stay here until player calls Stop.
        QTV_MSG( QTVDIAG_VIDEO_TASK,
                 "VideoPlayer::DecoderThread sleeping (PLAYING, done rendering)" );
        (void)QCUtils::WaitForCondition(&wakeupSync,COMMON_IDLE_WAIT_TIMEOUT_MSEC);
      }
      else if (bPlayDoneDecoding)
      {
        bool bStop = false;

        uint32 timeStamp;
        uint32 timeDuration;

        int latestTimestampID = 0;
        long tempTs           = 0;
        long latestTimestamp  = 0;

        /* Find the last (most recently displayed) video sample across all layers */
        for (int index=0; index<pMpeg4->GetNumberOfVideoTracks(); index++)
        {
          tempTs = pMpeg4->GetTimestampForCurrentLayeredVideoSample(index);

          if (tempTs>latestTimestamp)
          {
            latestTimestamp = tempTs;
            latestTimestampID = index;
          }
        }
        pMpeg4->GetCurVideoSampleInfo(latestTimestampID, &timeStamp, &timeDuration);

        /* Determine if the last video sample is done displaying for its full
        ** duration before we notify video done.
        */
        if (pMpeg4->GetSource()== Media::STREAM_SOURCE)
        {
          int videoStreamDuration = pMpeg4->GetDurationMsec();

          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Stream duration = %d", videoStreamDuration);

          // Post VIDEO_DONE event if:
          //    1. no clip duration information (live stream)
          //    2. soft abort in the middle of streaming (!EOS)
          //    3. Receive FIN from server
          if (pMpeg4->ShouldEndPlaybackImmediately())
          {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                "DoneDecoding (quit immediately). Set bStop to TRUE");
              bStop = true;
          }
          else
          {
             /* Note:
             If last few frames at the end of video stream are lost in the n/w,
             current frame will end up staying on the screen until the elapsed time
             reaches the clip duration
            */
            bStop =  ( (int)GetElapsedTime() > videoStreamDuration);
          }
        }
        else
        {

          bStop = ( GetElapsedTime() > (long)(nLastTimeStamp + timeDuration));
        }

        if (bStop)
        {
          StopVideoOutput();
          QTV_MSG( QTVDIAG_VIDEO_TASK, "Video is Done Displaying");
          bPlayDoneRendering=true;
          /* We can't go to IDLE because that cleans up data structures
          ** and statistics information which Mpeg4Player probably wants.
          ** However, we are done playing, and should not remain in the
          ** PLAYING state.  Other subsystems rely on our state to accurately
          ** reflect the, um, state of things.
          **
          ** Move to SUSPENDED as we wait for Mpeg4Player to clobber us.
          */
          Notify( SUSPENDED, Common::VIDEO_DONE );
        }
        else
        {
          /* check again after common polling interval */
          QTV_MSG( QTVDIAG_VIDEO_TASK, "Video is waiting for display to finish.");
          (void)QCUtils::WaitForCondition(&wakeupSync,COMMON_BUFFERING_POLL_TIME_MSEC);
        }
      }
      else
      {
        //
        // Normal play state.  Play frames until done
        //
        bool bSleepAfter;

        /**
     * For DCF playback, Calling task needs to register an IPC signal. This signal is set by IxStream on completion
     * of an asynchronous operation. IPC handler for the calling task will make sure
     * to invoke IxStreamDispatchMsg which eventually invokes the callback provided by QTV in calling task context.
     */
    #ifdef FEATURE_QTV_DRM_DCF
      if(m_bIsVideoTaskRegisteredForIPC == false)
      {
        /*
       * Before reading any video data from IxStream,
       * VIDEO task needs to have an IPC signal and to be registered with ixipc.
       * Without this, read from VIDEO task will get blocked foreever on DRM suite.
         */

      ixipc_init(0,QTV_DRM_DCF_IPC_SIG) ;
      m_bIsVideoTaskRegisteredForIPC = true;
      }
        #endif

        (void) DecodeOneFrame( bSleepAfter );

        if ( bSleepAfter )
        {
          QTV_MSG( QTVDIAG_VIDEO_TASK,
                   "VideoPlayer::DecoderThread sleeping (PLAYING, post-decode)" );
          bool bSignalled = QCUtils::WaitForCondition(&wakeupSync,COMMON_IDLE_WAIT_TIMEOUT_MSEC);
          if ( ! bSignalled )
          {
            QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "VideoDec Aborting TimedOut Waiting" );
            // Act like the clip terminated/completed
            (void)decIF.AbortDecoding();
          }
        }

        // Check the data state
        switch (decIF.GetDataState())
        {
        case Media::DATA_OK:
        case Media::DATA_INIT: // jg
          // Do Nothing, Okay
          break;

        case Media::DATA_END:
          //normal ending
          if( decIF.GetOutstandingFrameCount() == 0 )
          {
            QTV_MSG( QTVDIAG_VIDEO_TASK, "Video is Done Decoding");
            bPlayDoneDecoding=true;
            nLastTimeStamp = decIF.get_last_timestamp();
          }
          else
          {
            QTV_MSG2( QTVDIAG_VIDEO_TASK, "Waiting for %d DecodeDone's for %d ms",
                      decIF.GetOutstandingFrameCount(),COMMON_WAKEUP_SYNC_POLL_TIME_MSEC );
            (void)QCUtils::WaitForCondition(&wakeupSync, COMMON_WAKEUP_SYNC_POLL_TIME_MSEC );
          }
          break;

        case Media::DATA_UNDERRUN:

          if((pMpeg4->GetSource() == Media::STREAM_SOURCE) && (videoDataLead > 0))
          {
             QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Ignoring the media status Media::DATA_UNDERRUN since datalead %d is +ve", videoDataLead);
             if( MAX_VIDEO_DATA_LEAD_TIME_MSEC < videoDataLead )
             {
                QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Waiting for %d Msec to poll for video data", COMMON_WAKEUP_SYNC_POLL_TIME_MSEC);
               (void)QCUtils::WaitForCondition(&wakeupSync, COMMON_WAKEUP_SYNC_POLL_TIME_MSEC );
             }
             else
            { 
                QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "Waiting for %d Msec to poll for video data", COMMON_VIDEO_DATA_POLL_TIME_MSEC);
               (void)QCUtils::WaitForCondition(&wakeupSync, COMMON_VIDEO_DATA_POLL_TIME_MSEC );
            }
            /* This will make sure that Video Player will come out of 
               UNDERRUN in some corner cases
             */
            GetElapsedTime(); 
            break;
          }
        
          //goto rebuffering.
#ifndef FEATURE_QTV_GENERIC_BCAST
          if (!(pMpeg4->GetSource() == Media::BCAST_FLO_SOURCE ||
              pMpeg4->GetSource() == Media::BCAST_ISDB_SOURCE ))
#else
          if (IsFileBcastMediaSource(pMpeg4,m_pAVSync))
#endif
            /*---------------------------------------------------------------
              Broadcast FLO does NOT want AV sync to be altered during a
              video underrun.

              Video underflow is not fatal and should not affect AV timing.
              If we reset the playback offset, it will cause plenty of the
              next video frames to be dropped for "being late" when they're
              really on time.

              Also, broadcast FLO does NOT want to stop video output just
              because of an underrun.  Let the renderer take care of that.
            ---------------------------------------------------------------*/
          {
            PauseVideoOutput();
          }
          durationBuffered = pMpeg4->GetVideoDurationAvailable();
          Notify(REBUFFERING, Common::VIDEO_BUFFERING);
          break;

        case Media::DATA_FRAGMENT:
          QTV_MSG( QTVDIAG_VIDEO_TASK,
                   "VideoPlayer::DecoderThread sleeping (DATA_FRAGMENT)" );
          (void)QCUtils::WaitForCondition(&wakeupSync,COMMON_FRAGMENT_WAIT_TIMEOUT_MSEC);
          break;

        case Media::DATA_INSUFFICIENT:
          QTV_MSG( QTVDIAG_VIDEO_TASK,
                   "VideoPlayer::DecoderThread sleeping (DATA_INSUFFICIENT)" );
          (void)QCUtils::WaitForCondition(&wakeupSync,COMMON_DATA_INSUFFICIENT_WAIT_TIMEOUT_MSEC);
          break;

        case Media::DATA_REALLOCATE_BUFFER:
          // Do Nothing, Okay
          break;

        case Media::DATA_ERROR:
        default:
          //error ending
          AbortVideoOutput();
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "video Data Error");
          Notify(IDLE, Common::VIDEO_ERROR_ABORT);
          break;
        }
      }
      break;
    }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::TheVideoThread

DESCRIPTION:
  Static video thread routine.

INPUT/OUTPUT PARAMETERS:
  param

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::TheDecoderThread(unsigned long param)
{
  QTV_MSG1( QTVDIAG_VIDEO_TASK, "VideoPlayer::TheDecoderThread param 0x%x",param );
  VideoPlayer  *pReturn = NULL;
  
   while (true)
   {
        pReturn = (VideoPlayer  *)ProcessAllCommands();
	   
       if(pReturn)
       {
            pCurrActiveVideoPlayerInst = (VideoPlayer *) pReturn;
            QTV_MSG1( QTVDIAG_VIDEO_TASK, "VideoPlayer::TheDecoderThread Owner ( 0x%x)",pReturn);		 
       }		
       if (pCurrActiveVideoPlayerInst )
       {
          // if a valid video player object, with all its valid member variables then execute the thread function
           pCurrActiveVideoPlayerInst->DecoderThread();
       }
       else
       {
           QTV_MSG1( QTVDIAG_VIDEO_TASK, "No commands to process, Waiting for %d ms", COMMON_WAKEUP_SYNC_POLL_TIME_MSEC );
           (void)QCUtils::WaitForCondition(&wakeupSync, COMMON_WAKEUP_SYNC_POLL_TIME_MSEC );	 
       }	
   }
}

//////////////////////////////////////////////////////////////////////
//
// Constructor/Destructor
//
////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  VideoPlayer::VideoPlayer

DESCRIPTION:
  Default constructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
VideoPlayer::VideoPlayer()
: decIF( wakeupSync ),
  m_bIsStateChangePending( false ),
  m_pRenderer( NULL )
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::VideoPlayer" );

  ++guVideoPlayerInstanceCnt;

  if (1 == guVideoPlayerInstanceCnt)
  {
     decoderTC.threadHandle = NULL;
     decoderTC.pThreadFunction = (QCThreadFuncType)TheDecoderThread;
     decoderTC.stackSize = COMMON_STACK_SIZE;
     QCUtils::SetThreadName(&decoderTC,Common::VIDEO_THREAD_NAME);
     QCUtils::CreateCondition(&wakeupSync,false,false);
     QCUtils::CreateCondition(&responseSync,false,false);
     (void)q_init( &m_commandQ );
     QCUtils::InitCritSect( &m_commandQ_CS );	  
     QCUtils::InitCritSect(&m_processCommands_CS);	
     pCurrActiveVideoPlayerInst = NULL;
  }

  state = IDLE;
  lastState = IDLE;
  lastStateWhenDisabled = IDLE;

  pMpeg4=NULL;
  pBitfile=NULL;

  videoDataLead = 0;
  maxVideoDataLead = 0;
  minVideoDataLead = 0;

  QCUtils::InitCritSect(&bufferInfo_CS);

  InitDefaults();

  #ifdef FEATURE_QTV_DRM_DCF
    m_bIsVideoTaskRegisteredForIPC = false;
  #endif
  pAVPlayer = NULL;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
}

/* ======================================================================
FUNCTION:
  VideoPlayer::~VideoPlayer

DESCRIPTION:
  Destructor

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
VideoPlayer::~VideoPlayer()
{
  QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::~VideoPlayer" );

  if (1 == guVideoPlayerInstanceCnt)
  {
     QCUtils::DestroyCondition(&wakeupSync);
     QCUtils::DestroyCondition(&responseSync);
     pCurrActiveVideoPlayerInst = NULL;	  
  }
  else if(pCurrActiveVideoPlayerInst == (VideoPlayer *)this)
  {
     pCurrActiveVideoPlayerInst = NULL;
  }

  QTV_MSG2( QTVDIAG_VIDEO_TASK, "this (0x%x) Owner ( 0x%x)  ",(uint32)this,pCurrActiveVideoPlayerInst);	

  if (1 == guVideoPlayerInstanceCnt)
  {
    QCUtils::DinitCritSect( &m_commandQ_CS );
    QCUtils::DinitCritSect(&m_processCommands_CS);
  }
   // decrement the instance reference count
   --guVideoPlayerInstanceCnt;
   QCUtils::DinitCritSect(&bufferInfo_CS);
}

/* ======================================================================
FUNCTION
  VideoPlayer::EnableTimeCheck

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
void VideoPlayer::EnableTimeCheck( unsigned long timeValue )
{
  checkTimeFlag = true;
  checkTimeValue = timeValue;
}

/* ======================================================================
FUNCTION
  VideoPlayer::WillCommandCauseStateChange

DESCRIPTION
  Given our current state and a pending command, decide if the command
  will generate a state change (assuming no errors)

DEPENDENCIES
  None.

RETURN VALUE
  boolean. true if this command is going to make us change state

SIDE EFFECTS
  None.
========================================================================== */
bool VideoPlayer::WillCommandCauseStateChange
(
 const State   lState,
 const Command command
)
const
{
  const bool transitions[ NUM_COMMANDS ][ NUM_STATES ] =
  {

  //IDLE   BUFFER REBUFF PLAYIN PAUSED SUSPEN SUSP_D
    false, false, false, false, false, false, false, // NONE
    true,  false, false, false, true,  true,  false, // PLAY
    false, true,  true,  true,  false, true,  false, // PAUSE
    false, true,  true,  true,  true,  true,  false, // STOP
    false, true,  true,  true,  true,  false, false, // SUSPEND
    false, false, false, false, false, false, false, // AUDIO_OVERRIDE_FORMAT
    false, true,  true,  true,  false, true,  false, // PLAY_NEXT_FRAME
    false, true,  true,  true,  false, true,  false, // PLAY_PREV_FRAME
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
    false, false, false, false, false, false, false, // ROTATE_VIDEO
    false, false, false, false, false, false, false, // SCALE_VIDEO
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    false, false, false, false, false, false, false, // RELEASE_DSP
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
    true,  false, false, false, false, true,  false, // DISABLE_VIDEO
    false, false, false, false, false, false, true,  // ENABLE_VIDEO
#ifdef FEATURE_QTV_AVI
    false, false, false, false, false, false, false  // VIDEO_EXCESSIVE_FRAME_DROPS
#endif
  };

  return transitions[ command ][ lState ];
}

/* ======================================================================
FUNCTION
  VideoPlayer::IsStateOKForRendering

DESCRIPTION
  Return TRUE if we're in a state where it's appropriate for the renderer
  to be running...

DEPENDENCIES
  None.

RETURN VALUE
  boolean. true if the renderer should be going.

SIDE EFFECTS
  None.
========================================================================== */
bool VideoPlayer::IsStateOKForRendering( void ) const
{
  return ( state == PLAYING     ||
           state == BUFFERING   ||
           state == REBUFFERING );
}

/* ======================================================================
FUNCTION:                                                            GRB
  VideoPlayer::ReleaseBuffer

DESCRIPTION:
  Notify the player that higher levels have finished rendering the
  current video frame.  The player is free to reuse or free the buffer
  after this function is called.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::ReleaseBuffer( const QtvPlayer::FrameInfoT &frame )
{
   decIF.ReleaseBuffer( frame ); 
}

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
/* ======================================================================
FUNCTION
  VideoPlayer::RestoreVideo

DESCRIPTION
  Restores video so that it can resume video playback.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
========================================================================== */
void VideoPlayer::RestoreVideo( void )
{
  if ( qtv_cfg_enable_dsp_release )
  {
    decIF.RestoreDSP();
  }
}
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

/* ======================================================================
FUNCTION
  VideoPlayer::WakeRenderer

DESCRIPTION
  A little utility function which handles all the bookkeeping commonly
  associated with waking the renderer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
========================================================================== */
void VideoPlayer::WakeRenderer( void )
{
  if ( !m_pRenderer )
  {
    return;
  }

  if ( IsStateOKForRendering() ) // && !IsStateChangePending() )
  {
    m_pRenderer->enable();
  }
  else
  {
    m_pRenderer->disable();
  }
}

/* ======================================================================
FUNCTION
  VideoPlayer::Flush

DESCRIPTION
// Call Flush() to flush out all decoder and renderer buffer queues.     //
// Video should be stopped or suspended when we do this, or the buffer   //
// queues'll fill up again...unless that's what you wanted...            //
//                                                                       //
// This function is synchronous - it will not return until all buffers   //
// have been flushed.  That means that the calling task will block.      //

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
========================================================================== */
void VideoPlayer::Flush( void )
{
  // Flushing is a two part process:  first we must flush the decoder
  // and then the renderer.  If we do it the other way, the decoder can put
  // stuff back in the renderer after we've flushed the renderer but before
  // we get around to flushing the decoder.

  decIF.Flush(); // decoders
  if ( m_pRenderer != NULL )
  {
    (void)m_pRenderer->flush();
  }
}

#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
/* ======================================================================
FUNCTION
  VideoPlayer::SetPlaybackSpeed

DESCRIPTION
  Sets the playback speed.

PARAMETERS
  Common::PlaybackSpeedT pbSpeed
    The desired playback speed.

RETURN VALUE
  None.
========================================================================== */
void VideoPlayer::SetPlaybackSpeed(Common::PlaybackSpeedType pbSpeed)
{
  switch (pbSpeed)
  {
    case Common::PLAYBACK_SPEED_NORMAL:
    {
      m_playbackSpeedNum = 1;
      m_playbackSpeedDen = 1;
      break;
    }

    case Common::PLAYBACK_SPEED_1P3X:
    {
      m_playbackSpeedNum = 4;
      m_playbackSpeedDen = 3;
      break;
    }

    case Common::PLAYBACK_SPEED_NO_CHANGE:
    default:
    {
      /* Nothing to do */
      break;
    }
  } /* switch (pbSpeed) */

  /* Set the playback speed ratio in videoDec */
  decIF.SetPlaybackSpeed(m_playbackSpeedNum, m_playbackSpeedDen);
}
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */

/* ======================================================================
FUNCTION
  VideoPlayer::SetMaxTimestamp

DESCRIPTION
 Many subsystems belonging to VideoPlayer need to know the maximum
 allowable timestamp during playback so they can handle rollover properly.
 Passing that information in via this function disseminates that data
 to all interested VideoPlayer-owned subsystems.

 If max-timestamp is not set, it is assumed to be 2^32 - 1.

PARAMETERS
  uint32 max: the new max.

RETURN VALUE
  None.
========================================================================== */
void VideoPlayer::SetMaxTimestamp( uint32 max )
{
   decIF.SetMaxTimestamp( max );
   if ( m_pRenderer )
   {
      m_pRenderer->set_max_timestamp( max );
   }
}

/* ======================================================================
FUNCTION
  VideoPlayer::SetRendererQueuingMode

DESCRIPTION
 Sets the renderer queuing mode.

 PARAMETERS
  Common::RenderQueuingMode mode
    The desired queuing mode

RETURN VALUE
  None.
========================================================================== */
void VideoPlayer::SetRendererQueuingMode(RendererQueuingMode mode) 
{
  QTV_MSG_PRIO1(QTVDIAG_VIDEO_TASK, QTVDIAG_PRIO_MED,
                "VideoPlayer::SetRendererQueuingMode is %d", mode);

  if (m_pRenderer)
  {
    m_pRenderer->set_queueing_mode(mode);
  }
}

/* ======================================================================
FUNCTION:
  VideoPlayer::UpdateStats

DESCRIPTION:
 Updates the statistics information in the Decoder Interface

INPUT/OUTPUT PARAMETERS:
  Statistics Update type .

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::UpdateStats(VideoDec::StatUpdateType action)
{
   decIF.UpdateStats(action);
}



//Sets the Preroll Count - dynamically set by Media
void VideoPlayer::SetPrerollRendererCount(uint32 prerollCount)
{
   QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetPrerollRendererCount" );

   decIF.SetPrerollRendererCount(prerollCount);

}


VDEC_ERROR VideoPlayer::SetVDECParameter( VDEC_PARAMETER_ID inputParamId,  VDEC_PARAMETER_DATA * pInputParam)
{
   QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetVDECParameter" );

   VDEC_ERROR nReturn = VDEC_ERR_MYSTERY_ERROR;

   nReturn = decIF.SetVDECParameter(inputParamId, pInputParam);

   return nReturn;
}

VDEC_ERROR VideoPlayer::GetLastVDECError()
{
 QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::GetLastVDECError" );

 return decIF.last_vdec_err;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::IsVideoPlayerAvailable

DESCRIPTION:
  Updates the Caller about the instantiation of Video Player

INPUT/OUTPUT PARAMETERS:
  Statistics Update type .

RETURN VALUE:
  bool - TRUE

SIDE EFFECTS:
  None.
======================================================================*/
bool VideoPlayer::IsVideoPlayerAvailable()
{
   return true;
}

/* ======================================================================
FUNCTION:
  VideoPlayer::StopVideoPlayerSync

DESCRIPTION:
 This is a sync funciton so that we can clean up the dicoder and stop the Videooutput and 
 move the state to IDLE. This function should be called only in suspended instance 

INPUT/OUTPUT PARAMETERS:
  none

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::StopVideoPlayerSync()
{
    StopVideoOutput();
    if (state!=IDLE)
    {
      Notify(IDLE,Common::VIDEO_STOPPED);
    }
}


/* ======================================================================
FUNCTION:
  VideoPlayer::UpdateClipInfoDimensions

DESCRIPTION:
 This is a sync funciton which updates the height and width of clipinfo.

INPUT/OUTPUT PARAMETERS:
  int height : height of enoded frame.
  int width  : width of encoded frame.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::UpdateClipInfoDimensions(int height, int width)
{
	if (pAVPlayer)
	{
       pAVPlayer->UpdateClipInfoDimensions(height, width);
	}
}
/* ======================================================================
FUNCTION:
  VideoPlayer::set_last_frame_rendered_

DESCRIPTION:
 This will set the last rendered time stanp inside renderer

INPUT/OUTPUT PARAMETERS:
  const uint64

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
======================================================================*/
void VideoPlayer::set_last_frame_rendered_(const uint64& ts )
{
	if ( m_pRenderer != NULL )
	{
		m_pRenderer->set_last_frame_rendered_(ts);
	}
}

