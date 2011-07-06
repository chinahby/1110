#ifndef __PLAYER_BASE_IF__
#define __PLAYER_BASE_IF__


/* =======================================================================
                              PlayerBaseIF.h
DESCRIPTION
  Definition of the public interface to the Video subsystem. Clients of the 
  Video Player can only communicate to the player via this interface.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playertask/main/latest/inc/PlayerBaseIF.h#10 $
$DateTime: 2009/10/08 21:35:07 $
$Change: 1048967 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */


/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "media.h"
#include "QtvPlayer.h"
#include "Videodec.h"
#include "QtvConfig.h"
#include "vdecoder_types.h"
#include "AVSync.h"


/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
//class qtv_vrend_class; /* forward decl */
class VideoPlayer; /* forward decl */

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
**                        Class Declarations
** ======================================================================= */
/* ======================================================================
CLASS
  PlayerBaseIF

DESCRIPTION
  This class defines a video player
========================================================================== */
class PlayerBaseIF
{

public:
  PlayerBaseIF()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::PlayerBaseIF" );
  }
  
  virtual ~PlayerBaseIF()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::~PlayerBaseIF" );
  }

  /////////////////////////
  // Control by AV Player
  /////////////////////////

  virtual bool Create( void*,
               QtvPlayer::MallocOutputBufferT,
               QtvPlayer::FreeOutputBufferT,
               AVSync*, 
               QtvConfig*)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Create" );

     return false;
  }
  	

  virtual void ResetData()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::ResetData" );
  }
  
  virtual bool Prep(const int, Media*,bool&,
            bool, Media::CodecType)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetPlayTimes" );

     return false;
  }

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
  virtual bool Start(bool&,bool) 
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Start" );

     return false;
  }

  virtual void Stop(bool&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Stop" );
  }

  // Stop the video output (Sync method called from PVPlayer context) 
  virtual void StopVideoPlayerSync()
  {
      QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::StopVideoPlayerSync" );
  }

  virtual bool Pause(bool&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Pause" );

     return false;
  }

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  virtual void Release(bool&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Release" );
  }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

  virtual void Disable(bool&)
  {
     QTV_MSG(QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Disable" );
  }
  
  virtual void Enable(bool&, Media::CodecType)
  {
     QTV_MSG(QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Enable" );
  }
  
  virtual bool Resume(bool&,long)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Resume" );

     return false;
  }
  
  virtual bool Suspend(bool&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Suspend" );

     return false;
  }
  
  virtual bool IsDone()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsDone" );
	
     return false;
  }
  	
  virtual bool IsPlaying()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsPlaying" );
	
     return false;
  }
  
  virtual bool IsPaused()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsPaused" );
	
     return false;
  }
  
  virtual bool IsPausedDisabled()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsPausedDisabled" );
	
     return false;
  }
  
  virtual void TimingUpdate()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::TimingUpdate" );
  }
  
  virtual long GetElapsedTime()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::GetElapsedTime" );

     return -1;
  }
  	
  virtual void SetPlayTimes(long,long)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetPlayTimes" );
  }
  
  virtual void UpdateStats(VideoDec::StatUpdateType)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::UpdateStats" );
  }
  
  // Flush all frames to be decoded and rendered. If render is true, render them as well.
  virtual bool PlayNextFrame(bool&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::PlayNextFrame" );

     return false;
  }
  	
  virtual bool PlayPrevFrame(bool&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::PlayPrevFrame" );

     return false;
  }

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  virtual bool RotateVideo(QtvPlayer::RotationType)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::RotateVideo" );

     return false;
  }

  virtual bool ScaleVideo(QtvPlayer::ScalingType)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::ScaleVideo" );

     return false;
  }
		
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_XSCALE_VIDEO
  virtual bool ScaleVideo(uint32, uint32)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::ScaleVideo" );

     return false;
  }
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
  virtual void NotifyTransform(const Common::VideoStatusCode)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::NotifyTransform" );
  }
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

  virtual bool SetAudioOverride(int, bool&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetAudioOverride" );

     return true;
  }


#ifdef SHOW_INFO
  virtual void ShowInfo(bool)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::ShowInfo" );
  }
#endif /* SHOW_INFO */


  virtual bool GetPosition(int32&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::GetPosition" );

     return false;
  }

  virtual bool GetStatistics(QtvPlayer::AudioVideoStatisticsT&,
                     Common::LastFrame&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::GetStatistics" );

     return false;
  }

  /* Set the flag to stop H.263 decoder initialization during initial preroll
     buffering
  */
  virtual void setStopVideoInitFlag(void)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::setStopVideoInitFlag" );
  }

  virtual void Destroy()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Destroy" );
  }
  
  virtual bool HasAudio()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::HasAudio" );

     return false;
  }
  
  virtual bool HasVideo()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::HasAudio" );

     return false;
  }
  
  virtual bool TrackHasEnded()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::TrackHasEnded" );

     return false;
  }
  	
  virtual bool IsAudioPlaying()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsAudioPlaying" );

     return false;
  }

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef FEATURE_QTV_AVI
  virtual bool notifyExcessFrameDrop(void)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "VideoPlayer::notifyExcessFrameDrop" );

     return true;
  }
#endif
  /////////////////////////////
  // these are public for decoder IF
  /////////////////////////////

  // DisplayFrame returns 'true' if the frame was consumed (displayed
  // or dropped) and false if it was not consumed and should be retried.
  //
   virtual bool DisplayFrame(VDEC_FRAME&,
                     const long, //presentation time
                     const bool,   //don't drop
                     const unsigned long
#ifdef FEATURE_QTV_MDP
                    ,bool&
#endif /* FEATURE_QTV_MDP */
                   )
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::DisplayFrame" );

     return false;
  }

  virtual void StartRendering()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::StartRendering" );
  }

  /////////////////////////
  // OS callbacks (must be public
  // though only used internally)
  /////////////////////////

  //Main thread.
  static void TheDecoderThread(unsigned long)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::TheDecoderThread" );
  }

  virtual bool VideoCommandPending()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::VideoCommandPending" );

     return false;
  }

  //virtual bool PauseVideoFromApp();
  //virtual bool ResumeVideoFromApp();


#ifdef FEATURE_H264_DECODER
  virtual bool PreParseHeader(Media*, bool, Media::CodecType)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::PreParseHeader");

     return true;
  }
#endif /* FEATURE_H264_DECODER */

  //player state
  enum State
  {
    IDLE,
    BUFFERING,
    REBUFFERING,
    PLAYING,
    PAUSED,
    SUSPENDED,
    PAUSED_DISABLED,
    NUM_STATES // Not a real state - used to know how many there are
  };


  virtual void Notify(State, const Common::VideoStatusCode)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Notify" );
  }

  virtual State GetState()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::GetState" );

     State state = IDLE;

     return state;
  }

  virtual void EnableTimeCheck(unsigned long)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::EnableTimeCheck" );
  }

  virtual inline void SetStreamEnd(bool)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetStreamEnd" );
  }

  virtual inline bool IsVideoAborted()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsVideoAborted" );

     return false;
  }

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  virtual void RestoreVideo(void)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::RestoreVideo" );
  }
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */


  virtual void StopVideoOutput()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::StopVideoOutput" );
  }
  
  virtual void SetResumeTime()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetResumeTime" );
  }
  
  virtual void SetPlayedbackTime()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetPlayedbackTime" );
  }

#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
  /* Sets the playback speed */
  virtual void SetPlaybackSpeed(Common::PlaybackSpeedType)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetPlaybackSpeed" );
  }

#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */


  virtual bool IsStateOKForRendering( void ) const
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsStateOKForRendering" );
  
     
     return false;
  }

  virtual bool IsStateChangePending( void ) const
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::IsStateChangePending" );
  
     
     return false;
  }



  //                                                                       //
  ///////////////////////////////////////////////////////////////////////////
  // Frame-buffer deallocation                                         GRB //
  //                                                                       //
  // When the OEM layer has completed rendering a video frame buffer,      //
  // it calls this member (via QtvPlayer and Mpeg4Player ) to notify       //
  // the player that it can release (or reuse) the buffer.                 //
  //                                                                       //

  virtual void ReleaseBuffer(const QtvPlayer::FrameInfoT&)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::ReleaseBuffer" );
  }


  // A little utility function which handles all the bookkeeping commonly
  // associated with waking the renderer.

  virtual void WakeRenderer( void )
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::WakeRenderer" );
  }
  virtual void set_last_frame_rendered_(const uint64&)
  {
	  QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::set_last_frame_rendered_" );
  }
  

  ///////////////////////////////////////////////////////////////////////////
  // Video-buffer flushing                                                 //
  //                                                                       //
  // Call Flush() to flush out all decoder and renderer buffer queues.     //
  // Video should be stopped or suspended when we do this, or the buffer   //
  // queues'll fill up again...unless that's what you wanted...            //
  //                                                                       //
  // This function is synchronous - it will not return until all buffers   //
  // have been flushed.  That means that the calling task will block.      //
  
  virtual void Flush( void )
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::Flush" );
  }

  ///////////////////////////////////////////////////////////////////////////
  // Media timestamp limit setting
  //
  // Many subsystems belonging to VideoPlayer need to know the maximum
  // allowable timestamp during playback so they can handle rollover properly.
  // Passing that information in via this function disseminates that data
  // to all interested VideoPlayer-owned subsystems.
  //
  // If max-timestamp is not set, it is assumed to be 2^32 - 1.
  
  virtual void SetMaxTimestamp(uint32)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetMaxTimestamp" );
  }

  //Sets the Preroll Count - dynamically set by Media
  virtual void SetPrerollRendererCount(uint32)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetPrerollRendererCount" );
  }

  virtual VDEC_ERROR SetVDECParameter(VDEC_PARAMETER_ID,  VDEC_PARAMETER_DATA*)
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetVDECParameter" );

     VDEC_ERROR nReturn = VDEC_ERR_MYSTERY_ERROR;

     return nReturn;
  }

  virtual VDEC_ERROR GetLastVDECError()
  {
     QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::GetLastVDECError" );

     VDEC_ERROR nReturn = VDEC_ERR_UNSUPPORTED_DIMENSIONS;

     return nReturn;
  }

  virtual void SetIncallVoiceCodec(Media::CodecType)
  {

   QTV_MSG( QTVDIAG_VIDEO_TASK, "PlayerBaseIF::SetIncallVoiceCodec" );   
  }

  /* Creates an Instance of Video Player */
  static PlayerBaseIF* CreateVideoPlayerInstance();
  
  /* Destroys the Instance of Video Player */
  static void DestroyVideoPlayerInstance(PlayerBaseIF* pActiveVideoPlayer);
  
  /* Ascertains the availbility of Video Player */
  virtual bool IsVideoPlayerAvailable()
  {
     return false;
  }
};
#endif

