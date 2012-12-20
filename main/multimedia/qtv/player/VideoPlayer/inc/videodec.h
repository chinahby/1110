/* =======================================================================
                              videoDec.h
DESCRIPTION
  Video decoder include file

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

$Header: //source/qcom/qct/multimedia/qtv/player/videoplayer/main/latest/inc/videodec.h#12 $
$DateTime: 2009/07/17 06:15:34 $
$Change: 970658 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifndef _VIDEODEC_H_
#define _VIDEODEC_H_

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "mp4dec_api.h"
#include "cmx.h"

#ifdef __cplusplus
} // extern "C"
#endif

#include "media.h"
#include "QtvPlayer.h"
#include "vdecoder.h"
#include "qtv_log.h"
#include "broadcast.h"
#include "FrameMap.h"
#ifndef FEATURE_WINCE
#include "QTV_VideoDelayAnalyzer.h"
#endif

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */
class AVSync;
class VideoPlayer;

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/* =======================================================================
**                        Class Declarations
** ======================================================================= */
/* ======================================================================
CLASS
  VideoDec
DESCRIPTION
  This class defines a video decoder

========================================================================== */
class VideoDec
{

public:
  VideoDec( QCConditionType &wakeupSync );
  ~VideoDec();

  /////////////////////////////////
  // For control by Video Player
  /////////////////////////////////

  static const int  DISPLAY_THRESH_LOCAL_MSEC ;
  static const int  DISPLAY_THRESH_STREAM_MSEC ;

  //
  // Init & Cleanup
  //

  void Create
  (
    void * player,
    QtvPlayer::MallocOutputBufferT  MallocOutputBuffer,
    QtvPlayer::FreeOutputBufferT    FreeOutputBuffer,
    QTV_BroadcastTransmitter* const broadcaster_ptr,
    AVSync* pAVSync
  );
  void Destroy();

  void CleanupPlay(void) ;
  void ResetData();

  bool StartPlaying(void) ;
  void InitFrame();

  void SetStopTime(long stop);
  void SetPos(long pos);

  void UpdateClipInfoDimensions(int height, int width);
  //
  // Use these during video playback.
  //

  bool DecodeOneFrame( bool& bSleepAfter );

  bool ShowNextFrame();
  bool ShowPrevFrame();

  /*
    Set the flag to stop H.263 decoder initialization during initial preroll
    buffering. Flag is not valid for pseudo streaming and local file playback
  */
  void setStopH263VideoInitFlag(void);


#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  bool RotateVideo(QtvPlayer::RotationType direction);
  bool ScaleVideo(QtvPlayer::ScalingType scale);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#ifdef FEATURE_QTV_XSCALE_VIDEO
  bool ScaleVideo(uint32 scaledOutFrameWidth,
                  uint32 scaledOutFrameHeight,
                  VDEC_SCALE_CB_FN scaleCbFn,
                  void * const     pCbData);

  VDEC_DIMENSIONS  m_outFrameSize;
  VDEC_SCALE_CB_FN m_scaleCbFn;
  void *           m_scaleCbData;
#endif /* FEATURE_QTV_XSCALE_VIDEO */
  VDEC_ERROR last_vdec_err;
  bool SetAudioOverrideFormat(int format);

  Media::MediaStatus GetDataState();

  enum FrameCheckContext
  {
    CHECK_BEFORE_DECODE,
    CHECK_BEFORE_QUEUE,
    CHECK_BEFORE_CONVERT,
    CHECK_BEFORE_DISPLAY,
    CHECK_BEFORE_DISPLAY_AGAIN
  };

  long GetElapsedTime();

  //(This needs to be public since sleep time can be updated during display.)
  bool CheckFrameTime(
    const unsigned long presentationtime,  //frame presentation time, or (-1) if none.
#ifdef SHOW_LATE
    const FrameCheckContext checkContext, //context
#endif /* SHOW_LATE */
    const long bNoDrop,
    long *pSleep);          //output: sleep time

  //
  // Info retrieval.
  //

  void GetPosition(int32 &pos);
  void GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                     Common::LastFrame &lastFrame);


  long GetWidth();
  long GetHeight();
  long GetFrameCount() { return nFramesProcessed; }
  void IncrementFrameCount() { ++nFramesProcessed; }

#ifdef SHOW_INFO
  void ShowInfo(bool bEnd);
#endif /* SHOW_INFO */

  //
  // Timing & statistic control
  //
  void EndPlayInterval(long time);
  void StartPlayInterval(long time);
  void EndPlay();

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  bool ReleaseDSP();
  void RestoreDSP();
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

  bool Disable();
  bool Enable();
  void SetAudioCodec(Media::CodecType audioCodec);

  void FrameDisplayed(long nPresFrame,long nPresTime,long time);

  //in call video specific
  void SetIncallVoiceCodec(Media::CodecType eVocoderType);
  Media::CodecType GetIncallVoiceCodec();
  void SetIncallStateParameter();
  
  enum FrameReleaseReason
  {
    FRAME_RENDERED,
    FRAME_DROPPED_AVSYNC,
    FRAME_DROPPED_RENDER_FAILURE,
    FRAME_DROPPED_RENDER_SUBMIT_FAILURE,
    FRAME_DROPPED_DECODE_FAILURE,
    FRAME_DROPPED_REQUIRED_QUALITY_FAILURE,
    FRAME_FLUSHED
  };

  uint32 LastFrameRenderedTS;

  enum StatUpdateType
  {
    DROP_BEFORE_DECODE,
    DROP_BEFORE_DISPLAY,
    DROP_REQUIRED_QUALITY_FAILURE,
    FAILED_DECODE,
    CONVERTED,
    DISPLAYED,
    DROP_AVSYNC,    
    INTERPOLATED
};
  void UpdateStats(StatUpdateType action);
  void UpdateStats(StatUpdateType action, void * const pFrame);
  void UpdateFrameIntervalStats(long timestampMsec);
  void UpdateFrameIntervalStatsForReposition();
  void UpdateTimeOffset(const long nPresTime);
  void InitStatistics();

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

  bool IsRendering() {return repos.bRendering;};
  bool AbortDecoding();

  void InitDataState();

  uint32 GetLastReadVideoFrameTimeStamp();
  uint32 GetAverageVideoFrameInterval(); 
  
 // video player class object will be stored here
  VideoPlayer  *pPlayer;

#ifdef FEATURE_QTV_6250_ARM_CLK_COMPENSATION
/* added the following variables to support drift fix */
  float driftConstant;
  bool reposition;
  long resumedTime;
#endif
  
private:
  // Callback functions for allocating output frame buffers.  These callback
  // functions are passed to vdec_create for use by the video decoder
  VDEC_MALLOC_FN m_Malloc;
  VDEC_FREE_FN   m_Free;

  //AV Sync object shared by all the media types
  AVSync* m_pAVSync;

  // This flag indicates clip's First Frame.
  bool isbFirstFrameDecoded;

  // We need access to the video player decoder thread's wakeup event.
  //vdec_suspend is asynchrounous. 
  QCConditionType &m_wakeupSync;
  QCConditionType m_suspendSync;

  int LipSyncWindowSize;
  bool bRebufferHappened;
  // Used to Remember the last time stamp before rebuffering
  uint32 m_uLastTimestampBeforeRebuffering; 

  //total frames processed.
  uint32 nFramesProcessed;
  uint32 nFramesRendered;
  bool isPlayTimeCalculated;

  enum VideoDecoderState { VIDEODEC_INIT=0, VIDEODEC_IDLE, VIDEODEC_BUSY, VIDEODEC_DISABLED };
  volatile VideoDecoderState _state;

  /*
   Flag: stopH263VideoInit

   TRUE, stop H.263 decoder initialization during initial preroll
   buffering.

   Flag is not valid for pseudo streaming and local file playback
  */
  bool stopH263VideoInit;
  bool bSingleStepOn;

  //incall video specific
  Media::CodecType nInCallVocodertype;

  /////////////////////////////////
  // Internal implementation.
  /////////////////////////////////

  enum VideoFrameType { VIDEO_I_FRAME, VIDEO_P_FRAME, VIDEO_UNKNOWN_FRAME };

  //data state
  volatile Media::MediaStatus dataState[COMMON_MAX_LAYERS];
  volatile Media::MediaStatus overallDataState;

  //file pointers
  Media     * pMpeg4;
  OSCL_FILE * pBitfile;

#ifdef FEATURE_H264_DECODER
  //Blob pointer
  VDEC_BLOB** ppHeaderBlob;
#endif /* FEATURE_H264_DECODER */

  bool InitDecodeBuffer(bool bPlayRestart);
  bool RunVideoQualityThreshold( VDEC_FRAME* const pFrame );

  void SetLipSyncWindowSize(Media * pMpeg4In);
  void GetFrameDimensionsFromMedia(Media * pMpeg4); 
   bool bFirstFrameRendered  ;
   bool bFirstIFrameRendered ;
   bool bIsFrameErrorThresholdMet;

public:

  VDEC_ERROR SetVDECParameter( VDEC_PARAMETER_ID inputParamId,  VDEC_PARAMETER_DATA * pInputParam);

  bool RenderVideoFrame
  (
    bool       &bAbort,
    VDEC_FRAME &frame
#ifdef FEATURE_QTV_MDP
    ,bool      &bFreeFrame
#endif /* FEATURE_QTV_MDP */
    );

  void DoGenericVdecReuseFrame( FrameReleaseReason Reason,
                                VDEC_FRAME *       pFrame );

private:
#ifndef PLATFORM_LTK
  // Log the rendering of a video frame.
  //
  void LogFrameRenderInfo( const VDEC_FRAME &frame ) const;

  // Log the rendering events
  void LogRenderEvents( const VDEC_FRAME &frame ) ;

  // Log the decoding of a video frame.
  //
  void LogFrameDecodeInfo( const VDEC_FRAME &frame ) const;

  // Log the a/v sync info of a video frame.
  //
  void LogAVSync( const long timestamp, const long delay, const uint8 Framedrop ) const;

#endif /* ! PLATFORM_LTK */

  // Log the reason for releasing a frame buffer.
  void LogFrameReuseReason( FrameReleaseReason Reason, uint32 TimeStamp );

  //last rendered video frame information
  struct LastRenderedFrameInfo
  {
    uint16 layer;
    bool bIsIntra;
    uint32 timestamp;
  }lastRenderedFrameInfo;

  //Video clip info
  struct VideoInfo
  {
    uint32 nLayers;
    int imageWidth, imageHeight;
    uint32 originalWidth;
    uint32 originalHeight;
    Media::CodecType videoCodec;
  };
  VideoInfo videoInfo;

  //video stats
  struct VideoStats
  {
    QCCritSectType CS;

    //Failed Decode stats
    int32 nFailedDecode;

    /* Number of Frames Interpolated by the Decoder */
    int32 nFramesInterpolated;

    //Drop frame stats
    int32 nDroppedDecode;  //drop before decode
    int32 nDroppedConvert; //drop before color convert.
    int32 nDroppedDisplay; //drop before display
    int32 nDroppedAVSync; //drop due to AV sync
    int32 nDropMacroBlockErrs; // Drop due to macrobolock errors
    int32 nMaxChunk;     //max # consecutive dropped frames.
    int32 nCurrentChunkDecode, nCurrentChunkDisplay; //used to get nMaxChunk

    int32 nStopPresTime; //pres time of 1st frame beyond stop time.

    //For AV sync stats
    Common::LastFrame lastFrame;

    int32 nMaxBehindMsec; //max late time.
    int32 nMaxAheadMsec; //max ahead time.
    int32 nPlayMsec; //total time in playing state
    int32 tPause, tResume; //used to compute nPlayMsec.
    float nFPSProcessed;  // total frames processed div total time in play state.
    float nFPSRendered;   // total frames rendered div total time in play state.
    int32 nFirstTimestamp,nLastTimestamp;

    // Timestamp of the last frame we tried to decode. May be different from 
    // lastFrame.nTimestamp since that is for the last displayed frame.
    int32 prevFrameTimestampMsec;

    // Weighted average of the time between frames.
    float averageInterFrameIntervalMsec;

    /*Count of different types of frames*/
    uint32 nIFrameTally; // Count of I Frames 
    uint32 nPFrameTally; // Count of P Frames
    uint32 nBFrameTally; // Count of B Frames
  };
  VideoStats videoStat;

  //AV timing info.
  bool bTimeOffsetValid;
  long timeOffset;

  //decoding buffers
  struct DecodeBuffer
  {
    uint8* pData[COMMON_MAX_LAYERS];
    uint32 maxSize[COMMON_MAX_LAYERS];
    int32  size[COMMON_MAX_LAYERS];
    uint64 timestamp[COMMON_MAX_LAYERS];
    uint32 delta[COMMON_MAX_LAYERS];
    int32  pos[COMMON_MAX_LAYERS];
    void  *userdata[COMMON_MAX_LAYERS];
    bool bUseAgain[COMMON_MAX_LAYERS];
  };
  DecodeBuffer decodeBuf;

  //Stop time.
  struct StopTime
  {
    unsigned long nStop;
    bool bStop;
  };
  StopTime stopTime;

  //repositioning
  struct Repos
  {
    unsigned long nPos;
    bool bUpdate;
    bool bRendering;
    long nRenderPos;
    long timestamp;
  };
  Repos repos;


  // Private routines.

  void SetAudioFormat( VideoDecControls* );
  bool DropFrameBeforeDecode
  (const long nPresTime,
   const bool bIframe,
   const int nLayer);

  bool DropFrameAfterDecode
  (const long nPresTime,
   const bool bNoDrop,
   const FrameCheckContext checkContext,
   long *pSleep);

  //arm compiler can't accept  in front of these
  void RefillDecodeBuf(const uint32 layer);
  void RefillDecodeBuf();

  void DiscardDecodeBuf();
  void DiscardDecodeLayer(const int nLayer);

  void RefillBitstreamDecodeBuf(const int layer);
  void RefillMpeg4DecodeBuf(const int layer);
  void ResetVideoPlayback(const int layer);

  Common::FileType GetFileType();

  bool ProcessVolHeader(bool &bH263);
  void InitPlayData();
  void CleanupDecoder();
  void InitVideoInfo();
  bool NewClip(Media * pMpeg4In, OSCL_FILE * pBitfileIn);

  /*
  ** Functions to make identifying image sizes cleaner reading.
  */
#ifdef FEATURE_MP4_ONEEIGHTH_VGA
  inline bool IsOneEighthVGA( const uint Height, const uint Width )
  {
    return ( Height == ( uint16 )MP4_ONEEIGHTH_VGA_HEIGHT &&
             Width  == ( uint16 )MP4_ONEEIGHTH_VGA_WIDTH );
  }
#endif /* FEATURE_MP4_ONEEIGHTH_VGA */

#ifdef FEATURE_MP4_ONEQUARTER_VGA
  inline bool IsOneQuarterVGA( const uint Height, const uint Width )
  {
    return ( Height == ( uint16 )MP4_ONEQUARTER_VGA_HEIGHT &&
             Width  == ( uint16 )MP4_ONEQUARTER_VGA_WIDTH );
  }
#endif /* FEATURE_MP4_ONEQUARTER_VGA */

  /**************************************************************************
  ** BELOW THIS POINT, NEW STUFF TO SUPPORT THE GENERIC VDEC API           **
  ** I don't even want to get involved in the mess up there, so I'm just   **
  ** putting my new additions down here.                                   **
  **************************************************************************/
private:
  /* Critical section to prevent Renderer Task and Video Task
  ** from clobbering common data structures such as frameRequestCount
  */
  QCCritSectType frameReqUpdate_CS;

  /*************************************************************************/

  /* Critical section to prevent the VideoPlayer and UI/Brew tasks from calling
   * VideoDec::CleanupDecoder simultaneously and causing a race condition
   * freeing m_pDecoderSettings.
   */
  rex_crit_sect_type m_shutdown_CS;

  bool DoGenericVdecInit( VDEC_BLOB * const pBlob,Media * pMpeg4);

  VDEC_ERROR InitExternalClock( void );

  bool DoGenericVdecSuspend( void );
  bool DoGenericVdecResume( void );

  boolean DoGenericVdecShutdown( void );

  static void InputBufferFreeCallback( const VDEC_STREAM_ID      stream,
                                       VDEC_INPUT_BUFFER * const pBuffer,
                                       void * const              pCbData );
  void InputBufferFreeHandler( VDEC_INPUT_BUFFER * const pBuffer );

  static void FrameBufferReadyCallback( const VDEC_STREAM_ID stream,
                                        VDEC_CB_STATUS       status,
                                        VDEC_FRAME * const   pFrame,
                                        uint32               nBytesConsumed,
                                        void * const         pCbDat );
  void FrameBufferReadyHandler( const VDEC_CB_STATUS    status,
                                VDEC_FRAME * const      pFrame );

  /* FrameBufferReadyHandler dispatches to one of these handlers,
  ** depending on the status message... */

  void HandleFrameReady( VDEC_FRAME * const pFrame );
    /* Handles 'frame ready' events for frames that should be displayed */

  void HandleDND_FrameReady( VDEC_FRAME * const pFrame );
    /* Handles 'ready' events for do-not-display frames */

  bool StateIsOkForFrameReady( void ) const;
    /* Returns true if we're OK to handle a frame-done normally. */

  /* utility functions used by FrameBufferReadyHandler */
  /*************************************************************************/
  VDEC_STREAM_ID m_vdec; /* Interface to decoder layers below */

  /* There exist race conditions in which multiple tasks, in response to
  ** unusual abort situations, can attempt to create or destroy the vdec
  ** simultaneously.  This critical section guards the act of vdec creation
  ** and destruction to prevent sadness. */
  rex_crit_sect_type m_vdecCreationLock;

public:
  VDEC_STREAM_ID GetVDEC( void ) const { return m_vdec; }

  /**************************************************************************
  ** The VideoDecControls structure allows us to keep track of decoder-    **
  ** independent settings like transformations and audio.                  **
  **                                                                       **
  ** It's kept as a pointer as a means of keeping state - if the settings  **
  ** pointer is NULL, we know we haven't been initialized yet.             **
  **                                                                       */
private:
  VideoDecControls *m_pDecoderSettings;

  /* Allocate and initialize the decoder settings to known defaults.
  */
  VideoDecControls* InitDecoderSettings(const uint32 audioFormat);
#ifdef FEATURE_H264_DECODER
  /* Prepend the NAL length to a header stream */
  bool PrependNalLength(uint8* buf, uint32 size,uint32 sizeOfNalLengthField);

public:
  /* The PreParseHeader function parses the H.264 header before the clip is played */
  bool PreParseHeader(Media* pMpeg4In, bool bRestart,Media::CodecType audioCodec);
#endif /* FEATURE_H264_DECODER */


  /*                                                                       **
  ***************************************************************************
  ** The PrepPlay functions do one-time startup initialization.  The main  **
  ** function does all the global prepwork, and media types which need     **
  ** special attention have their own sub-functions which are called from  **
  ** the main.                                                             **
  **                                                                       */
public:
  bool PrepPlay(bool             &bCannotPrep,
                Media            *pMpeg4In,
                OSCL_FILE        *pBitfileIn,
                bool              bRestart,
                Media::CodecType  audioCodec);

  bool DoGenericVdecCreate(void);

private:
  /* DecodeStreamHeader_XXX will decode the header data for the specific decoder
  */
  bool  DecodeStreamHeader_MP4 (void);
  bool  DecodeStreamHeader_H264(void);
  bool  DecodeStreamHeader_WMV (void);

  /*                                                                       **
  ***************************************************************************
  ** Frame recording                                                       **
  **                                                                       */
#ifdef FEATURE_QTV_STREAM_RECORD

private:

  void RecordFrame( const LastRenderedFrameInfo * const pFrameInfo );

#endif // FEATURE_QTV_STREAM_RECORD

  // frameRequestCount: a sad and sordid history.
  //
  // frameRequestCount was used by videoDec to keep track of the number
  // of frames that had been parsed on the ARM side and queued for submission
  // to the DSP for final decode.  This allowed us to check for task completion
  // and such.
  //
  // However, with the introduction of the vdec api, a lot of the transparency
  // which allowed that mechanism to work well has been lost.  Queueing
  // of input buffers is not tied to the receipt of decoded frames by the API.
  // In fact, the API does not even mandate ordering for these callbacks,
  // and they can and do come in different orders with differing decoders
  // or conditions.
  //
  // So how do we tell how much work is pending at any given moment?
  //
  // We use a metric called 'outstandingFrameCount'.  I am not using
  // frameRequestCount again to emphasise the fact that we're doing something
  // very different from the old frameRequestCount.
  //
  // The outstandingFrameCount ( OFC ) is defined as the *absolute* difference
  // between the number of frame-ready callbacks we've received and the
  // number of frame events we expect.  When it's zero, it means we're
  // 'caught up' - that there is no outstanding work remaining in the pipeline.
  //
  // Unfortunately, this number is tricky to calculate because of some
  // ambiguity on the input end...we don't know how many frames to expect
  // from any given decode until the input buffer is returned and we can
  // examine the bFrameEventPending flags in it.  However, just because we
  // haven't had the input buffer returned doesn't mean there's no work
  // in progress...it is an error to ignore that state.
  //
  // So how do we calculate OFC?  Because OFC is the _absolute_ difference
  // between the input and output counts, we can assign arbitrary signs
  // to them and have everything work fine.  We'll be using positive numbers
  // to indicate event-pending flags on the input end and negative numbers
  // to represent frame-done events.
  //
  // OFC is maintained by two seperate 'processes', one on the input end
  // and one on the output end:
  //
  // On the input end:
  //
  // - we increment OFC once for every input layer when we call
  // vdec_decode.  This produces a 'high water' value.  It's not likely
  // that every layer will generate a frame, but it's okay - at least
  // we're aware that there's work pending.  The number will be corrected
  // in the second part of the input-end processing, which is:
  // - when an input buffer is returned (InputBufferFreeHandler) we decrement
  // OFC by the number of layers which don't have their bFrameEventPending
  // flags set.
  //
  // After both of these events have occurred, OFC will have been incremented
  // by the number of frames which should be produced by the decoder as
  // a result of the input.
  //
  // On the output end:
  //
  // - we decrement OFC every time we get a decode-done event.  Easy!
  //
  // The net result of all this work is a value whose absolute value
  // is at least the number of frames outstanding in the decoding queues.
  // When this number reaches zero, no outstanding work remains.
  //
  // This metric works regardless of the order in which events arrive,
  // as long as they all eventually do arrive.

public:
  int GetOutstandingFrameCount( void );

private:
  int m_outstandingFrameCount; // remember, the real value is |ofc|!
  
  // This function alters the outstanding frame count and, if appropriate,
  // wakes up the decoder task, which can go to sleep if there's too much
  // work outstanding (as a throttling mechanism) or if it thinks it's done
  // providing input data and is just waiting for the pipes to clear....

  int AlterOutstandingFrameCount( const int   delta,
                                           const char* szDiagHint = NULL );

  // This member lets the renderer ask us when it's appropriate to drop
  // a video frame.
public:
  bool IsItOkayToDropThisFrame( const VDEC_FRAME* const pFrame ) const;

  // When the renderer shuffles the rendering queue, it calls this member
  // to tell us what the new 'current' (IE, of the next frame to be rendered)
  // video dimensions are.

  void UpdateVideoDimensions( const VDEC_FRAME& frame );

  // This function allows the renderer to figure out how much of a preroll
  // it should be maintaining from frame to frame.

  uint32 GetRequiredRendererPreroll( void ) const;

  //Sets the Preroll Count - dynamically set by Media
  void SetPrerollRendererCount(uint32 prerollCount);

  ///////////////////////////////////////////////////////////////////////////
  // Video-buffer flushing                                                 //
  //                                                                       //
  // Call Flush() to flush out all decoderbuffer queues.                   //
  // Video should be stopped or suspended when we do this, or the buffer   //
  // queues'll fill up again...unless that's what you wanted...            //
  //                                                                       //
  // This function is synchronous - it will not return until all buffers   //
  // have been flushed.  That means that the calling task will block.      //
public:
  void Flush( void );

#ifdef FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY
  /* Sets the playback speed ratio */
  void SetPlaybackSpeed(uint32 pbSpeedNum, uint32 pbSpeedDen);
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY */

private:
  VideoDecoderState Set_State( const VideoDecoderState newState );

  // We have one VDEC_INPUT_BUFFER and we reference it through a pointer.
  // When we submit it, the pointer is NULLed until it is returned.
  //
  VDEC_INPUT_BUFFER  m_VDEC_InputBuffer; 
  VDEC_INPUT_BUFFER* m_pVDEC_InputBuffer;

  /**************************************************************************
  ** The pointer to the system broadcaster is passed in at create.  We     **
  ** keep it here for use in all things broadcasty.                        */
private:
  QTV_BroadcastTransmitter* m_broadcaster_ptr;
  bool m_bPreParseHeaderStatus;

  /* If the clip has no audio, m_playbackSpeedNum and m_playbackSpeedDen 
  ** contain the playback speed ratio.
  ** If audio is present m_playbackSpeedNum = m_playbackSpeedDen = 1 and
  ** since audio is the master, playing audio faster will automatically force
  ** video to playback faster.
  */
  uint32 m_playbackSpeedNum;
  uint32 m_playbackSpeedDen;

#ifdef FEATURE_FRE
  //controls enable/disable of Qtv feature
  VDEC_FRE_ENABLE m_bEnableQFRE;
#endif

  /** Routines to help us make decisions about timestamps */
  /*@{*/
public:
  uint32 get_last_timestamp( void );
  
  // Check to make sure that renderer frame belong to current play interval
  bool CheckPlayIntervalMatch(uint32 uPresTime);
private:
  uint32 m_last_timestamp;
  bool timestamp_is_defined( const uint32 ts );
  void record_timestamp( VDEC_FRAME* const frame_ptr );
  void repair_timestamp( VDEC_FRAME* const frame_ptr );

  /*@}*/

  ///////////////////////////////////////////////////////////////////////////
  // Rendering-frame tracking
  //
  // At levels below this one, video frames belong to the decoder and are
  // referred to with VDEC_FRAME structures.
  //
  // At levels above this one, video frames are reduced to a single YUV
  // buffer pointer and are referred to by FrameInfoT structs.
  //
  // When a frame is sent to the UI for rendering, it is not safe to reuse
  // until the UI returns it, but the UI returns FrameInfoTs and the decoder
  // is only interested in VDEC_FRAMEs.  It is therefore the VideoPlayer's
  // job to keep a mapping of FrameInfoT to VDEC_FRAME for cross-layer
  // translation.
  //
  // Because the VDEC_FRAMEs belong to the decoder, VideoPlayer is not
  // in control of their lifetimes; they can become invalid at any time.
  // Therefore there's no way of knowing if any given ReleaseBuffer call
  // is legitimate...all we can do is convert the buffer to last-known
  // VDEC_FRAME and pass it on.  The decoder will catch mismatches.
public:
  void ReleaseBuffer( const QtvPlayer::FrameInfoT& frame );
  
  //Maps the input override format to the Vdec understandable format
  VDEC_CONCURRENCY_CONFIG MapToVdecConcurrencyFormat(uint32 format);  
  	
private:
  FrameMap m_renderingFrames;

  // Audio media coded got from Mpeg4player while playing the clip
  Media::CodecType  AudioMediaCodecTypeSet;

  
  ////////////////////////////////////////////////////////////////////////////
  // Arbitrary timestamp rollover
  //
  // Timestamps will not necessarily overflow at 2^32 - 1, so time intervals
  // must be calculated with knowledge of that rollover point.
  //
  // If we ever get to the point where timestamp rolls over at 2^64 - 1, this
  // ( and the Difference/Sum functions in qtv_utils ) will have to be
  // revisited, as a rollover of 2^64 - 1 will cause a div-by-0.
public:
  void   SetMaxTimestamp( const uint64& m ) { m_maxTimestamp = m; }
  uint64 GetMaxTimestamp( void )     { return m_maxTimestamp; }
private:
  uint64 m_maxTimestamp;
public:
  #ifdef FEATURE_FRE
  bool IsQFREActive(void)
  {
    //controls enable/disable of Qtv feature
    return m_bEnableQFRE ; 
  }
  #endif  
  // boolean function to return broadcast lead check failures. 
  bool IsBroadcastLeadCheckFailed(long nSleepMsec);  

   //////////////////////////////////////////////////////////////////////////
   // Video display-delay analysis
   //
   // In conjunction with AVSync, monitors rendering delays and attempts
   // to minimize interruptions to playback.
private:
#ifndef FEATURE_WINCE
   QTV_VideoDelayAnalyzer* m_pVideoDelayAnalyzer;
#endif
};
#endif

