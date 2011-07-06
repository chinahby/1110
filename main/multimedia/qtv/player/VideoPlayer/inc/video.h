/* =======================================================================
                              video.h
DESCRIPTION
  Video player include file

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

$Header: //source/qcom/qct/multimedia/qtv/player/videoplayer/main/latest/inc/video.h#21 $
$DateTime: 2009/08/14 03:00:59 $
$Change: 996932 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef _VIDEO_H_
#define _VIDEO_H_

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "media.h"
#include "QtvPlayer.h"
#include "videoDec.h"
#include "QtvConfig.h"
#include "PlayerBaseIF.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
class qtv_vrend_class; /* forward decl */

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
  VideoPlayer

DESCRIPTION
  This class defines a video player
========================================================================== */
class VideoPlayer : public PlayerBaseIF
{

public:
  VideoPlayer();
  virtual ~VideoPlayer();

  /////////////////////////
  // Control by AV Player
  /////////////////////////

#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

  virtual bool Create( void * pContext,
               QtvPlayer::MallocOutputBufferT MallocOutputBuffer,
               QtvPlayer::FreeOutputBufferT   FreeOutputBuffer,
               AVSync* pAVSync, 
               QtvConfig *pQtvConfig );

  virtual void ResetData();
  virtual bool Prep(const int playbackID, Media * pMpeg4,bool &bError,
            bool bRestart, Media::CodecType audioFormat);
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
  virtual bool Start(bool &bError,bool bRestart) ;
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
  virtual void Stop(bool &bError) ;
  virtual bool Pause(bool &bError) ;
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  virtual void Release(bool &bError);
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
  virtual void Disable(bool &bError);
  virtual void Enable(bool &bError, Media::CodecType audioCodec);
  virtual bool Resume(bool &bError,long stop) ;
  virtual bool Suspend(bool &bError);
  virtual bool IsDone();
  virtual bool IsPlaying();
  virtual bool IsPaused();
  virtual bool IsPausedDisabled();
  virtual void TimingUpdate();
  virtual long GetElapsedTime();
  virtual void SetPlayTimes(long start,long stop);
  virtual void set_last_frame_rendered_(const uint64& ts);
  
  virtual void UpdateStats(VideoDec::StatUpdateType action);
  // Flush all frames to be decoded and rendered. If render is true, render them as well.
  virtual bool PlayNextFrame(bool &bError);
  virtual bool PlayPrevFrame(bool &bError);

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  virtual bool RotateVideo(QtvPlayer::RotationType direction);
  virtual bool ScaleVideo(QtvPlayer::ScalingType scaling);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_XSCALE_VIDEO
  virtual bool ScaleVideo(uint32 scaledOutFrameWidth, uint32 scaledOutFrameHeight);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
  virtual void NotifyTransform(const Common::VideoStatusCode code);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

  virtual bool SetAudioOverride(int audioFormat, bool &bError);

#ifdef SHOW_INFO
  virtual void ShowInfo(bool bEnd) { decIF.ShowInfo(bEnd); }
#endif /* SHOW_INFO */

  virtual bool GetPosition(int32 &pos);
  virtual bool GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                                       Common::LastFrame &lastFrame);

  /* Set the flag to stop H.263 decoder initialization during initial preroll
     buffering
  */
  virtual void setStopVideoInitFlag(void);
  virtual void Destroy();
  virtual bool HasAudio();
  virtual bool HasVideo();
  virtual bool TrackHasEnded();
  virtual bool IsAudioPlaying();
  // Stop the video output (Sync method called from PVPlayer context) 
  virtual void StopVideoPlayerSync();

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef FEATURE_QTV_AVI
  virtual bool notifyExcessFrameDrop(void);
#endif
  /////////////////////////////
  // these are public for decoder IF
  /////////////////////////////

  // DisplayFrame returns 'true' if the frame was consumed (displayed
  // or dropped) and false if it was not consumed and should be retried.
  //
  virtual bool DisplayFrame( VDEC_FRAME& frame,
                     const long nPresTime, //presentation time
                     const bool bNoDrop,   //don't drop
                     const unsigned long nPresFrame
#ifdef FEATURE_QTV_MDP
                    ,bool &bFreeFrame
#endif /* FEATURE_QTV_MDP */
                   );

  virtual void StartRendering();
  virtual void UpdateClipInfoDimensions(int height, int width);
  /////////////////////////
  // OS callbacks (must be public
  // though only used internally)
  /////////////////////////

  //Main thread.
  static void TheDecoderThread(unsigned long param) ;

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */


  //display interface objects

  //thread info
  static QCThreadControl decoderTC;

  //decoder interface object
  VideoDec decIF;

  //Use to get Config items
  QtvConfig* m_pQtvConfigObject;

  //Critical section protecting 'outputBuffers'
  QCCritSectType bufferInfo_CS;

#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

  virtual bool VideoCommandPending();

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */

  //bool PauseVideoFromApp();
  //bool ResumeVideoFromApp();

  static VideoPlayer  *pCurrActiveVideoPlayerInst;
	
#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

#ifdef FEATURE_H264_DECODER
  virtual bool PreParseHeader(Media* pMpeg4In, bool bRestart,Media::CodecType audioCodec);
#endif /* FEATURE_H264_DECODER */

  virtual void Notify(State newState, const Common::VideoStatusCode);

  virtual State GetState(){return state;};

  virtual void EnableTimeCheck( unsigned long timeValue );

  virtual inline void SetStreamEnd(bool streamEnd)
  {
    streamEndFlag = streamEnd;
  }

  virtual inline bool IsVideoAborted()
  {
    return VideoAborted;
  }

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  virtual void RestoreVideo(void);
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */
   int32 videoDataLead;
   int32 maxVideoDataLead;
   int32 minVideoDataLead;

public:
  bool bPlayDoneDecoding,bPlayDoneRendering,bPlayDoneFragment;
  uint32 nLastTimeStamp;

  // Pointer to the AV player in which this video player resides.
  Mpeg4Player * pAVPlayer;

  //commands from the AV player
  enum Command {
    NONE
    , PLAY
    , PAUSE
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    , RELEASE_DSP
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
    , STOP
    , SUSPEND
    , AUDIO_OVERRIDE_FORMAT
    , PLAY_NEXT_FRAME
    , PLAY_PREV_FRAME
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
    , ROTATE_VIDEO
    , SCALE_VIDEO
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#ifdef FEATURE_QTV_XSCALE_VIDEO
    , XSCALE_VIDEO
#endif
    , DISABLE_VIDEO
    , ENABLE_VIDEO
    , PCR_RESET
    #ifdef FEATURE_QTV_AVI
    , VIDEO_EXCESSIVE_FRAME_DROPS
    #endif
    , NUM_COMMANDS // Not a real command - used to know how many there are
  };

#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

virtual void StopVideoOutput();
virtual void SetResumeTime();
virtual void SetPlayedbackTime();

#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
  /* Sets the playback speed */
  virtual void SetPlaybackSpeed(Common::PlaybackSpeedType pbSpeed);
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */

private:
//gives information about number of instances running
  static uint32 guVideoPlayerInstanceCnt;

  //Pointer to the AV sync object shared by all the media types
  AVSync* m_pAVSync;

  //video and renderer thread sync events
  static  QCConditionType wakeupSync;
  static  QCConditionType responseSync;

  // elapsed time check for Mpeg4Player
  bool checkTimeFlag;
  unsigned long checkTimeValue;

  State state;
  State lastState;
  State lastStateWhenDisabled;

  /* If the clip has no audio, m_playbackSpeedNum and m_playbackSpeedDen 
  ** contain the playback speed ratio.
  ** If audio is present m_playbackSpeedNum = m_playbackSpeedDen = 1 and
  ** since audio is the master, playing audio faster will automatically force
  ** video to playback faster.
  */
  uint32 m_playbackSpeedNum;
  uint32 m_playbackSpeedDen;
  
  // If the mpeg4player has sent commands which are intended to move us
  // into a different state, we record that here.  Whether or not
  // we actually make a state change is another thing...
  //
  bool m_bIsStateChangePending;

  // Given our current state and a pending command, guess if we'll change
  // state.
  //
  bool WillCommandCauseStateChange( const State   state,
                                    const Command command ) const;

  // Are we in a state where rendering is appropriate?
  //
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

public:

#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

  virtual bool IsStateOKForRendering( void ) const;
  virtual bool IsStateChangePending( void ) const { return m_bIsStateChangePending; }

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */

  // Event Structure
  struct CommandEvent{
     Command   cmd;
     uint32    pOwner;
  };

  
private:

  // Command queue
  struct CommandQElem {
    q_link_type  link;
    CommandEvent cmdEvent;
  };
  static q_type m_commandQ;
  static QCCritSectType m_commandQ_CS;
  
  bool bAbort;
  bool bPlayingNotifyPending;
#ifdef FEATURE_QTV_MDP
  /* The following flag is set when the STOP command is issued to prevent
  ** the frame from being sent up to the application for display.
  */
  bool bStopRendering;
#endif /* FEATURE_QTV_MDP */

  //playback ID
  int currentPlaybackID;

  // Buffered duration in msec
  uint32 durationBuffered;

  //media interface
  Media     * pMpeg4;
#ifndef FEATURE_WINCE
  OSCL_FILE * pBitfile;
#endif
  bool        bPrepped;

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

  long nElapsedTime;

  //Internal routines.
  bool PrepPlay(bool bRestart,bool &bCannotPrep, Media::CodecType audioCodec);
  bool StartPlaying(void);
  void InitPlayData(void);
  void NewPlayTimes(long start,long stop,bool bStill);
  bool InitImage();
  void InitDefaults();
  void CleanupPlay();
  void DecoderThread();
  
  static CommandQElem* GetNextQueuedCommand( void );
  bool IsDataAvailable();
  uint32 GetVideoDurationAvailable();
#ifdef FORCE_REBUFFERING
  void ForceRebuffering(void);
#endif /* FORCE_REBUFFERING */

  void SetState(State newState);
  void ShowState(State input);
  bool StartThread() ;

  bool SendCommand(Command newCommand, unsigned long timeout);
  void CommandComplete();

  bool DecodeOneFrame( bool &bSleepAfter);
  bool ShowNextFrame();
  bool ShowPrevFrame();

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  QtvPlayer::RotationType  rotateDirection;
  bool VideoRotation();
  QtvPlayer::ScalingType   scalingFactor;
  bool VideoScaling();
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_XSCALE_VIDEO
  uint32 scaledOutFrameWidth;
  uint32 scaledOutFrameHeight;
  void VideoXScaling();

  static void ScaleVideoCallback ( const VDEC_STREAM_ID  stream,
                                   const boolean         scaleDone,
                                   void * const          pCbData );
#endif //FEATURE_QTV_XSCALE_VIDEO

  int  AudOverrideFormat;
  bool SetAudioOverridePriv();

  bool CanStart(bool bRestart);
  bool CanPrep(bool bRestart);
  bool CanPause();
  bool CanResume();
  bool CanDisable();
  bool CanEnable();
  bool CanSuspend();
  void PauseVideoOutput();
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  bool ReleaseDSP();
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

  bool DisableVideo();
  bool EnableVideo();

  void ResumeVideoOutput( bool clearState );
  void StartVideoOutput();
  void AbortVideoOutput(bool bError = true);
  void SuspendVideoOutput();

  struct DecodeInfo
  {
    long nPresentationTime;
    bool bPresTimeValid;
    bool bNoDrop;
  };
  DecodeInfo decodeInfo;

  bool streamEndFlag;

  ///////////////////////////////////////////////////////////////////////////
  // Distributed command processing                                        //
  // Commands are processed in both the decoder and the renderer task,     //
  // to ensure that state changes are caught as early as possible.         //
  //                                                                       //

private:

  // Process every pending command.
  //
  static  uint32 ProcessAllCommands( void );

  // Process a single command.  Called repeatedly by ProcessAllCommands.
  //
  void ProcessOneCommand( const Command command );

  // Critical section which prevents multiple tasks from simultaneously
  // running the process-commands loop.
  //
  static QCCritSectType m_processCommands_CS;
  bool VideoAborted;

#ifdef FEATURE_QTV_DRM_DCF
      bool m_bIsVideoTaskRegisteredForIPC;
#endif

  //                                                                       //
  ///////////////////////////////////////////////////////////////////////////
  // Frame-buffer deallocation                                         GRB //
  //                                                                       //
  // When the OEM layer has completed rendering a video frame buffer,      //
  // it calls this member (via QtvPlayer and Mpeg4Player ) to notify       //
  // the player that it can release (or reuse) the buffer.                 //
  //                                                                       //

public:

#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

  virtual void ReleaseBuffer( const QtvPlayer::FrameInfoT &frame );

  // A little utility function which handles all the bookkeeping commonly
  // associated with waking the renderer.

#ifndef FEATURE_WINCE
  virtual void WakeRenderer( void );
#endif

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */

#ifndef FEATURE_WINCE
  qtv_vrend_class* m_pRenderer;
#endif


  ///////////////////////////////////////////////////////////////////////////
  // Video-buffer flushing                                                 //
  //                                                                       //
  // Call Flush() to flush out all decoder and renderer buffer queues.     //
  // Video should be stopped or suspended when we do this, or the buffer   //
  // queues'll fill up again...unless that's what you wanted...            //
  //                                                                       //
  // This function is synchronous - it will not return until all buffers   //
  // have been flushed.  That means that the calling task will block.      //

#ifndef FEATURE_QTV_DISCARD_VIDEO_PLAYER

  virtual void Flush( void );

  ///////////////////////////////////////////////////////////////////////////
  // Media timestamp limit setting
  //
  // Many subsystems belonging to VideoPlayer need to know the maximum
  // allowable timestamp during playback so they can handle rollover properly.
  // Passing that information in via this function disseminates that data
  // to all interested VideoPlayer-owned subsystems.
  //
  // If max-timestamp is not set, it is assumed to be 2^32 - 1.

   virtual void SetMaxTimestamp( uint32 max );

  //Sets the Preroll Count - dynamically set by Media
  virtual void SetPrerollRendererCount(uint32 prerollCount);

  virtual VDEC_ERROR SetVDECParameter( VDEC_PARAMETER_ID inputParamId,  VDEC_PARAMETER_DATA * pInputParam);

  virtual VDEC_ERROR GetLastVDECError();

  /* Ascertains the availbility of Video Player */
  virtual bool IsVideoPlayerAvailable();

#endif /* FEATURE_QTV_DISCARD_VIDEO_PLAYER */

   enum RendererQueuingMode
   {
     RENDERER_QUEUING_MODE_SORTING,
     RENDERER_QUEUING_MODE_FIFO
   };

   /* Sets the renderer queuing mode. */
   void SetRendererQueuingMode(RendererQueuingMode mode);

};
#endif

