#ifndef _MPEG4PLAYER_H_
#define _MPEG4PLAYER_H_
/* =======================================================================
                               mpeg4player.h
DESCRIPTION
  This module contains the declaration for the Mpeg4Player class.
  The class is the high-level interface to player functionality.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/playertask/main/latest/inc/mpeg4player.h#38 $
$DateTime: 2010/06/29 22:58:48 $
$Change: 1351543 $

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
#include "PlayerBaseIF.h"
#include "Video.h"
#include "audioPlayerIf.h"
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#include "timedtext.h"
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

class QtvStream;
class Streamer;

#include "QtvPlayer.h"
#include "Events.h"
#include "media.h"
#include "fileMedia.h"
#include "AppFile.h"
#include "sdpfile.h"
#include "QtvConfig.h"

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
#include "Download.h"
#endif

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#include "downloadMedia.h"
#endif

#include "broadcast_task.h"
#include "AVSync.h"

#ifdef FEATURE_QTV_DRM_DCF

  #include "IxCallbackDef.h"
  #include "IxStream.h"

#endif


#ifdef FEATURE_QTV_HTTP_DOWNLOAD
#include "HttpDownload.h"
#include "HttpDataInterface.h"
#include "oem_header.h"
#endif

extern "C" {
#include "qtv_event.h"
}

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
class ITrackList;
class QTV_BroadcastTransmitterTask;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

#define QTV_LOCAL_MEDIA_TRACK_LIST_IDENTICAL   0x1

#define QTV_STATS_LOCAL_BUFFER_SIZE   2048

#define HTTP_DEFAULT_STARTUP_TIME         4000
#define HTTP_DEFAULT_REBUFFERING_TIME     4000


/* =======================================================================
MACRO MYOBJ

ARGS
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
/* ======================================================================
CLASS
  Mpeg4Player

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
class Mpeg4Player
{

public:
  Mpeg4Player();
  ~Mpeg4Player();

  qtv_event_clip_started_type    qtv_event_clip_started_payload;
  AVSync m_mediaSync;
  bool m_bMediaAbort;

  ///////////////////////
  // API control
  ///////////////////////
  bool CreatePlayer( QtvPlayer::MallocOutputBufferT,
                     QtvPlayer::FreeOutputBufferT );
  bool Destroy();
  void DestroyMedia();

  void UpdateClipInfoDimensions(int height, int width);
  
  QtvPlayer::ReturnT GetPlayerStateExternal(QtvPlayer::PlayerStateRecordT &state);
  QtvPlayer::ReturnT GetRTPStatistics(QtvPlayer::RTPStatisticsT &statistics);
  QtvPlayer::ReturnT GetAudioVideoStatistics(QtvPlayer::AudioVideoStatisticsT &statistics);
  QtvPlayer::ReturnT GetURNType(QtvPlayer::URNTypeT &URNType, bool bVideoURN);
  QtvPlayer::ReturnT GetURLType(QtvPlayer::URLTypeT &URLType);
  bool SetFrameInfo(void *pRGBFrame, void *pYUVFrame,
                    uint16 SrcHeight, uint16 SrcWidth,
                    uint32 Timestamp, uint32 NumconcealedMB,
                    uint8 fVOP, void *pPostFilterMbInfo, unsigned long NumIntraMbs,
                    void *pMetaData,
                    uint16 wCropWinX2, uint16 wCropWinY2);
  /* GetFrameInfo returns the frame and any extended frame info
     associated with the current frame */
  QtvPlayer::ReturnT GetFrameInfo(QtvPlayer::FrameInfoT &info,void **ppExtFrmInfo);
  QtvPlayer::ReturnT GetClipInfo(QtvPlayer::ClipInfoT &info);
  QtvPlayer::ReturnT GetServerChallenge(char *);
  Media::CodecType GetAudioCodecType();
  Media::CodecType GetVideoCodecType();

  char sUrnStatsName[QTV_STATS_LOCAL_BUFFER_SIZE];
  int  WriteStatisticsToEFS (QtvPlayer::AudioVideoStatisticsT stats);


  #ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  bool WaitForDownloadFileHeader();
  #endif

  QtvPlayer::ReturnT RetrySessionWithQtvUserAuthInfo(char *);
  void AppCanAuthenticate(bool);
  bool CanAppAuthenticate(void);

  bool IsAudioVideoTextFromSeparateFiles();

  /* Set the flag to stop H.263 decoder initialization
     during intial preroll buffering
  */
  void setStopVideoInitFlag(void);

#ifdef FEATURE_QTV_STREAM_RECORD
  bool SetStrRecEFSReservedSpace(uint32 resSpaceinBytes);
#endif

  Media * GetAudioMedia();
  Media * GetVideoMedia();
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  Media * GetTextMedia();
#endif

#ifndef FEATURE_WINCE
  PlayerBaseIF * GetVideoPlayer(){return pActiveVideoPlayer;};
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  TimedText   * GetTextPlayer() {return &textPlayer;};
#endif
#endif   /*    FEATURE_WINCE   */
  void SetIncallVoiceCodec(Media::CodecType eVocoderType);

  QtvPlayer::ReturnT SetUserAgentParameters(const char *man,
                                                         const char *ctn);
  char * GetUserAgentMAN()
  {
    return userAgentMAN;
  }
  char * GetUserAgentCTN()
  {
    return userAgentCTN;
  }
  char * GetUserAgentVCT()
  {
    return userAgentVCT;
  }
  char * GetUserAgentACT()
  {
    return userAgentACT;
  }
  char * GetUserAgentSCT()
  {
    return userAgentSCT;
  }
  char * GetUserAgentCLR()
  {
    return userAgentCLR;
  }
  char * GetUserAgentLCD()
  {
    return userAgentLCD;
  }
  char * GetUserAgentVER()
  {
    return userAgentVER;
  }

  uint32 GetTotalRxDataRate();

  void SetAbortFlag()
  {
    bAbort = true;
  }

  void SetMediaAbortFlag(bool bFlag)
  {
    m_bMediaAbort = bFlag;
  }

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */

  bool isStreaming()
  {
    return clip.bStreaming;
  }

 #ifdef FEATURE_QTV_GENERIC_BCAST_ISDB
 bool isBcastISDB()
 {
   return clip.bBcastISDB;
 }
 #endif

  bool isBcastFLO()
  {
    return clip.bBcastFLO;
  }

#ifdef FEATURE_QTV_GENERIC_BCAST
  bool isBcastGeneric()
  {
    return clip.bBcastGeneric;
  }


  bool isBcastStream()
  {
    return clip.bBcastStream;
  }
  bool isLiveBcast()
  {
    return clip.bLiveBcast;
  }
#endif

  QtvPlayer::ReturnT SetUserAgent(const char *userAgentName);

  void InitUserAgentParams(const char* min = NULL);

  char * GetUserAgent()
  {
     return userAgent;
  }

  QtvPlayer::ReturnT SetUserAgentParameters(const char* params);

  OSCL_STRING& GetUserAgentParams()
  {
     return userAgentParams;
  }

  UserAgentParamsType GetUserAgentParamsType()
  {
    return userAgentParamsType;
  }

  bool StartThread();
  bool IsThreadActive();
  bool IsPlayerThreadIdle()
  {
    return (threadIdle == true);
  }

  bool IsPlayerIdle()
  {
    if (playerState == IDLE)
      return true;
    else
      return false;
  }

  #ifdef FEATURE_QTV_GENERIC_BCAST
  //Player is paused or not
  bool IsPlayerPaused()
  {
    switch(playerState)
    {
#ifdef FEATURE_QTV_PSEUDO_STREAM
      case PLAYER_PSEUDO_PAUSE:
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
      case PLAYER_HTTP_PAUSE:
#endif
      case PAUSED:
        {
          return true;
        }
      default:
        {
          return false;
        }
    }

  }
  // Player is active or not
  bool IsPlayerActive()
  {
    switch(playerState)
    {
      case PLAYING:
      case BUFFERING:
      case REPOSITIONING:
        {
          return true;
        }
      default:
        {
          return false;
        }
    }

  }
  #endif

  ///////////////////////
  // Public routines for
  // all threads
  ///////////////////////
  void PostMessage(QCMessageType *pEvent);
  static void EventPostCallback( QCMessageType * const   pEvent,
                          void * const            pCbData
                        );

  ///////////////////////
  // Public routines for
  // audio & video thread
  ///////////////////////
  void SendAVTick(int id);
  void SetImageDimensions(int width,int height);
  OSCL_FILE * GetBitfile(bool bVideoBitFile);

  // Checks whether the Audio is muted or not?
  bool IsAudioMuted()
  {
    return ((clip.bmMute & QTV_AUDIO_TRACK_SELECTED) == QTV_AUDIO_TRACK_SELECTED);
  }
  // Checks whether the Video is muted or not?
  bool IsVideoMuted()
  {
    return ((clip.bmMute & QTV_VIDEO_TRACK_SELECTED) == QTV_VIDEO_TRACK_SELECTED);
  }
  // Checks whether the Text is muted or not?
  bool IsTextMuted()
  {
    return ((clip.bmMute & QTV_TEXT_TRACK_SELECTED) == QTV_TEXT_TRACK_SELECTED);
  }

  uint32 GetPlayerPositionConsideringMutes(QtvPlayer::PlayerStateRecordT &currentPS);

  bool HasAudio()
  {
    return clip.bHasAudio;
  }
  bool IsAudioPlaying();

  bool IsAudioOutputDelayed();

  bool HasVideo()
  {
    return clip.bHasVideo;
  }
  bool IsVideoPlaying();

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  bool HasText()
  {
    return clip.bHasText;
  }
  bool IsTextPlaying();
#endif

  void RedirectToNewURN(mbchar* newURN = NULL);

  //For test support, called by video thread.
  void ForceRebuffering();

  ///////////////////////
  //ZREX OS callbacks.  These all have to be public even though they're
  //only used internally.
  ///////////////////////

  //Message handlers
  static void PV_OPEN_URN_handler(QCMessageHandleType handle) ;
  static void PV_OPEN_BUFFER_URN_handler(QCMessageHandleType handle) ;

#ifdef FEATURE_QTV_DRM_DCF
  //An event posted by QtvPlayer which contains IxStream for
  // audio/video/text
  static void PV_OPEN_INPUTSTREAM_URN_handler(QCMessageHandleType handle) ;
#endif

  static void PV_PLAY_CLIP_handler(QCMessageHandleType handle) ;

#ifdef FEATURE_QTV_STREAM_RECORD
  static void QTV_RECORD_CLIP_handler(QCMessageHandleType handle);
  static void QTV_STOP_RECORDING_handler(QCMessageHandleType handle);
  uint32 getRecordedClipDuration(void);
#endif /* FEATURE_QTV_STREAM_RECORD */

  static void PV_REGISTER_FOR_CALLBACK_handler(QCMessageHandleType handle) ;
  static void PV_REGISTER_FOR_CALLBACK_EX_handler(QCMessageHandleType handle) ;
  static void PV_SET_PREROLL_BUFFER_SIZE_handler(QCMessageHandleType handle) ;
  static void PV_SET_VIDEO_WINDOW_POS_handler(QCMessageHandleType handle) ;
  static void PV_SET_ZOOM_handler(QCMessageHandleType handle) ;
  static void PV_SET_DISPLAY_AREA_handler(QCMessageHandleType handle) ;
  static void PV_SET_DISPLAY_QUEUE_DEPTH_handler(QCMessageHandleType handle) ;
#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif
  static void PV_PAUSE_handler(QCMessageHandleType handle) ;
  static void PV_STOP_handler(QCMessageHandleType handle) ;
  static void PV_NEXT_FRAME_handler(QCMessageHandleType handle);
  static void PV_PREV_FRAME_handler(QCMessageHandleType handle);
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  static void PV_VIDEO_ROTATE_handler(QCMessageHandleType handle);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
  static void PV_VIDEO_SCALE_handler(QCMessageHandleType handle);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

  static void PV_SET_AUDIOOVER_handler(QCMessageHandleType handle);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  static void PV_DELETE_DOWNLOAD_SESSION_handler(QCMessageHandleType handle);
  static void PV_DOWNLOAD_CLIP_handler(QCMessageHandleType handle);
  static void PV_RESUME_DOWNLOAD_handler(QCMessageHandleType handle);
  static void PV_STOP_DOWNLOAD_handler(QCMessageHandleType handle);
  static void DOWNLOAD_STATUS_handler(QCMessageHandleType handle);
  static void DOWNLOAD_FILE_SIZE_STATUS_handler(QCMessageHandleType handle);
  QtvPlayer::ReturnT GetDownloadTime(QtvPlayer::PlayerStateRecordT &state);
  bool StartPlayingOrNotifyReady(void);
  #endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) ||defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  QtvPlayer::DownloadPlaybackControlT getDownloadClipType(void);
  void SetPvxSaveOption(char *filePath);
  void SetPvxPreroll(int pvxPreroll);
#endif
  static void VIDEO_STATUS_handler(QCMessageHandleType handle) ;
  static void AUDIO_STATUS_handler(QCMessageHandleType handle) ;
  static void AUDIO_TIMING_handler(QCMessageHandleType handle) ;
  static void AV_TICK_handler(QCMessageHandleType handle) ;
  static void STREAMER_STATUS_handler(QCMessageHandleType handle);
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  static void AUDIO_SPEC_handler(QCMessageHandleType handle) ;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */


  static void QTV_MUTE_handler(QCMessageHandleType handle);
#ifdef FEATURE_QTV_GENERIC_BCAST
  static void GENERIC_BCAST_STATUS_handler(QCMessageHandleType handle);
  static void QTV_SELECT_PB_TRACK_handler(QCMessageHandleType handle);
  static void QTV_REPOSITION_handler(QCMessageHandleType handle);
  static void QTV_SEEK_SYNC_handler(QCMessageHandleType handle);
  static void QTV_PLAY_SYNC_handler(QCMessageHandleType handle);
#endif /* FEATURE_QTV_GENERIC_BCAST */

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  static void CPVTEXT_STATUS_handler(QCMessageHandleType handle);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  static void QTV_PAUSE_AUDIO_handler(QCMessageHandleType handle);
  static void QTV_PAUSE_VIDEO_handler(QCMessageHandleType handle);
  static void QTV_PAUSE_TEXT_handler(QCMessageHandleType handle);
  static void QTV_RESUME_AUDIO_handler(QCMessageHandleType handle);
  static void QTV_RESUME_VIDEO_handler(QCMessageHandleType handle);
  static void QTV_RESUME_TEXT_handler(QCMessageHandleType handle);
#endif /* FEATURE_FILE_FRAGMENTATION || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_PSEUDO_STREAM
  static void QTV_PS_PARSER_STATUS_EVENT_handler(QCMessageHandleType handle);
#endif

  static void PV_PLAYER_EXIT_handler(QCMessageHandleType handle);

#ifdef FEATURE_QTV_PSEUDO_STREAM
  static void QTV_PS_PROCESS_PSEUDO_STREAM_handler(QCMessageHandleType handle);
  static void QTV_PS_STREAM_OPEN_EVENT_handler(QCMessageHandleType handle);
  static void QTV_PS_PAUSE_EVENT_handler(QCMessageHandleType handle);
  static void QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_handler(QCMessageHandleType handle);
#endif

#ifdef FEATURE_FILE_FRAGMENTATION
  static void QTV_SKIP_CLIP_handler(QCMessageHandleType handle);
#endif /*FEATURE_FILE_FRAGMENTATION*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  static void QTV_HTTP_STREAM_OPEN_EVENT_handler(QCMessageHandleType handle);
  static void QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_handler(
                                                 QCMessageHandleType handle);
  static void QTV_HTTP_PARSER_STATUS_EVENT_handler(QCMessageHandleType handle);
  static void QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_handler(QCMessageHandleType handle);
  static void QTV_PROCESS_HTTP_STREAM_handler(QCMessageHandleType handle);
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  static void QTV_HTTP_BUFFER_UPDATE_handler(QCMessageHandleType handle);
  static void QTV_HTTP_EVENT_handler(QCMessageHandleType handle);
#endif


#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  static void QTV_HTTP_REGISTER_CALLBACK_EVENT_handler(QCMessageHandleType handle);
  static void HTTP_DOWNLOAD_STATUS_handler(QCMessageHandleType handle);
#endif

  static void QTV_SUSPEND_handler(QCMessageHandleType handle);
  static void QTV_RESUME_handler(QCMessageHandleType handle);
#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */

#ifdef FEATURE_QTV_WM_DRM_API
  static void QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_handler(QCMessageHandleType handle);
#endif /* FEATURE_QTV_WM_DRM_API */

#ifdef FEATURE_QTV_PLAYLIST
  static void QTV_SKIP_handler(QCMessageHandleType handle);
#endif /* FEATURE_QTV_PLAYLIST */

#ifdef FEATURE_QTV_AVI
  static void QTV_EXCESSIVE_FRAME_DROPS_handler(QCMessageHandleType handle);
#endif

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  static void QTV_COPY_BUFFER_TO_FILE_handler(QCMessageHandleType handle) ;
#endif

  static void QTV_AUDIO_RESTARTING_TRACK_handler(QCMessageHandleType);

  //Main thread routine.
  static void ThePlayerThread(unsigned long param) ;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  bool CopyBufferToFile(const QTV_COPY_BUFFER_TO_FILE_type * pEvent);
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

  //(arm compiler complains if this isn't public)
  //Callback data
  struct CallbackData
  {
    QtvPlayer::CallbackFuncT func;
    QtvPlayer::CallbackInstanceFuncT instanceFunc;
    QtvPlayer::InstanceHandleT handle;
    void *pClientData;
    void *pUserData;
  };

  bool SetLoopTrackFlag(bool bLoop);
  bool GetLoopTrackFlag(void);
  bool IsLocalMediaPLayback();

#ifdef FEATURE_QTV_PSEUDO_STREAM
  boolean updatePseudoStreamBufferOffset(const QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_type * pEvent);
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  bool updateHTTPStreamBufferOffset(
            const QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_type * pEvent);
  void setStartAndBufferingTime(uint32 startupTime, uint32 bufferingResumeTime);
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  static void HttpPullDataInterface(unsigned long mySelf);

  struct HttpPullDataT
  {
    bool bTimerSet;
    uint32 dnldDataSize;
    QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize;
    QtvPlayer::InstanceHandleT m_QtvInstanceHandle;
    rex_timer_type* pHttpPullTimer;

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    QTV_HTTP_STREAM_UPDATE_WBUFFER_OFFSET_EVENT_type *pUpdateWptrOffsetPD;
#endif
#if defined FEATURE_QTV_PSEUDO_STREAM
    QTV_PS_UPDATE_WBUFFER_OFFSET_EVENT_type *pUpdateWptrOffsetPS;
#endif

  };

  HttpPullDataT httpPullData;
#endif /*defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)*/

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  void freeDownloadBuffer(char *buffer);
  char *getEncryptedData(uint32 &size);
  QtvPlayer::OemHttpHeaderResult AddOemHttpHeaders(
       const char *headerName,
       const char *headerValue);
#endif

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

  void * mediaInfo;
  bool SetMediaInfo(void *media_info, uint32 size);
  bool bAllowVidToSetOffSetOnAudUnderrun;

#ifdef FEATURE_QTV_WM_DRM_API
  QtvPlayer::DRMDecryptMethodT storedpDRMDecryptFunction;
  void  *storedpClientData;
  bool RegisterDRMDecryptMethod( const QTV_REGISTER_DRM_DECRYPT_METHOD_EVENT_type *pEvent);
#endif /* FEATURE_QTV_WM_DRM_API */

#ifdef FEATURE_QTV_NET_POLICY
  bool SetNetPolicyInfo(QtvPlayer::NetPolicyInfo* netPolicyInfo);
  bool GetNetPolicyInfo(QtvPlayer::NetPolicyInfo* netPolicyInfo);
  const QtvPlayer::NetPolicyInfo& GetNetPolicyInfo(){return netPolicyInfo;};
#endif /* FEATURE_QTV_NET_POLICY */

#ifdef FEATURE_QTV_ENCRYPTED_STREAMS
  bool SetDescramblerHandler(void * descrambler);
#endif

  // Player state
  enum State
  {
    IDLE                          = 0,
    CONNECTING                    = 1,
    PB_READY                      = 2,
    OPENING                       = 3,
    BUFFERING                     = 4,
    PLAYING                       = 5,
    PAUSED                        = 6,
    REPOSITIONING                 = 7
#ifdef FEATURE_QTV_PSEUDO_STREAM
    ,PLAYER_PSEUDO_PAUSE           = 8
#endif
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    ,PLAYER_HTTP_PAUSE             = 9
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
    ,DL_READY                      = 10
    ,DOWNLOADING                   = 11
    ,DOWNLOADED_DATA_AVAIL         = 12
#endif
    ,SUSPENDED                     = 13
  };

  // Audio concurrency state
  enum AudioConcurrencyState
  {
    AUDIO_NOT_CONCURRENT,           /* Audio is from the clip */
    AUDIO_CONCURRENT,               /* Audio is from concurrent source
                                       (voice call, MIDI/MP3 ringer */
    AUDIO_CONCURRENCY_NOT_SUPPORTED /* No concurrent DSP image available, play
                                       is not allowed */
  };

  void setPrerollDurationBuffered(uint32 duration);
  void Notify(State newState);
  void Notify(QtvPlayer::PlayerStatusT status) ;

#ifdef FEATURE_QTV_SKT_MOD_MIDI
  uint32 GetMidiDataSize();
  uint32 GetMidiData(uint8 *pBuf, uint32 size, uint32 offset);

  uint32 GetLinkDataSize();
  uint32 GetLinkData(uint8 *pBuf, uint32 size);
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

  uint32 GetDataSize(QtvPlayer::DataT dType, uint32 offset);
  uint32 GetData(QtvPlayer::DataT dType, uint8 *pBuf, uint32 size, uint32 offset);

  long   GetAmtBufferedInCMX();

  unsigned long GetElapsedTime();

  QtvPlayer::ReturnT SelectPlaybackTracks(uint32 dwTrackFlag);

  /*Functions to access the methods of QTV Config Items.  Called from wrapper functions of QtvPlayer*/
  bool SetQTVConfigItem( QtvConfig::QtvConfigItemId item_id, int32 value );
  int32 GetQTVConfigItem( QtvConfig::QtvConfigItemId item_id );

  State GetCurrPlayerState()
  {
    return playerState;
  }

  State GetPrevPlayerState()
  {
    return lastPlayerState;
  }

  class URNData
  {
  public:
    URL * sURN;
    unsigned char * pBufURN;
    uint32 bufSize;
    QtvPlayer::URNTypeT type;
    bool bValid;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    bool bHttpURN;
#endif

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
    bool bHttpPDURN;
#endif

    QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize;
    QtvPlayer::FetchBufferedDataT FetchBufferedData;
    QtvPlayer::InstanceHandleT m_QtvInstanceHandle;

#ifdef FEATURE_QTV_DRM_DCF
    dcf_ixstream_type inputStream;
#endif

    URNData()
    {
      sURN        = NULL;
      pBufURN     = NULL;
      bufSize     = 0;
      bValid      = false;
      type        = QtvPlayer::URN_NONE;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      bHttpURN    = false;
#endif
#ifdef FEATURE_QTV_DRM_DCF
      inputStream = NULL;
#endif
    }

    ~URNData()
    {
      if (NULL != sURN)
      {
        QTV_Delete(sURN);
        sURN = NULL;
      }
    }

    bool Init()
    {
      pBufURN     = NULL;
      bufSize     = 0;
      bValid      = false;
      type        = QtvPlayer::URN_NONE;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      bHttpURN    = false;
#endif
#ifdef FEATURE_QTV_DRM_DCF
      inputStream = NULL;
#endif
      if (sURN == NULL)
      {
        sURN = QTV_New_Args(URL, (URL::MaxUrlSize+1));
      }
      else
      {
        sURN->Reset();
      }
      bool rsl = false;
      if (NULL != sURN)
      {
        rsl = true;
      }
      return rsl;
    }

    URNData(const URNData &urnData)
    {
      sURN        = NULL;
      type        = urnData.type;
      bValid      = urnData.bValid;
      bufSize     = urnData.bufSize;
      pBufURN     = urnData.pBufURN;
      FetchBufferedDataSize = urnData.FetchBufferedDataSize;
      FetchBufferedData   = urnData.FetchBufferedData;
      m_QtvInstanceHandle = urnData.m_QtvInstanceHandle;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      bHttpURN    = urnData.bHttpURN;
#endif
#ifdef FEATURE_QTV_DRM_DCF
      inputStream = urnData.inputStream;
#endif
      if(urnData.sURN)
      {
        sURN = QTV_New_Args(URL, (URL::MaxUrlSize+1));
        if(sURN)
        {
          *sURN = *urnData.sURN;
        }
      }
      else
      {
        sURN = NULL;
      }
    }

    URNData& operator=( URNData &urnData )
    {
      type        = urnData.type;
      bValid      = urnData.bValid;
      bufSize     = urnData.bufSize;
      pBufURN     = urnData.pBufURN;
      FetchBufferedDataSize = urnData.FetchBufferedDataSize;
      FetchBufferedData   = urnData.FetchBufferedData;
      m_QtvInstanceHandle = urnData.m_QtvInstanceHandle;
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
      bHttpURN    = urnData.bHttpURN;
#endif
#ifdef FEATURE_QTV_DRM_DCF
      inputStream = urnData.inputStream;
#endif
      if ((NULL != sURN) && (NULL != urnData.sURN))
      {
        *sURN = *urnData.sURN;
      }
      return *this;
    }
  };

  bool isAudioParsedSuccessfully(Media*);
  bool isVideoParsedSuccessfully(Media*);


private:

  /////////////////////////
  // API data
  /////////////////////////
  int selectedAudioTrackIdx;
  int selectedVideoTrackIdx;
  uint32 numReconnectAttemptsMade;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  int selectedTextTrackIdx;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
  URNData URN;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

  URNData videoURN;
  URNData audioURN;
  URNData textURN;
#ifdef FEATURE_QTV_FCS
#error code not present
#endif

  QCCritSectType URN_CS;

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */

  class URLData
  {
  public:
    URL * sURL;
    QtvPlayer::URLTypeT type;
    bool bValid;
    bool bLocalFileFromPVX;

    URLData()
    {
      sURL              = NULL;
      type              = QtvPlayer::URL_NONE;
      bValid            = false;
      bLocalFileFromPVX = false;
    }

    bool Init()
    {
      type              = QtvPlayer::URL_NONE;
      bValid            = false;
      bLocalFileFromPVX = false;
      if (sURL == NULL)
      {
        sURL = QTV_New_Args(URL, (URL::MaxUrlSize+1));
      }
      else
      {
        sURL->Reset();
      }
      bool rsl = false;
      if (NULL != sURL)
      {
        rsl = true;
      }
      return rsl;
    }

    ~URLData()
    {
      if (NULL != sURL)
      {
        QTV_Delete(sURL);
        sURL = NULL;
      }
    }

    URLData(const URLData &urlData)
    {
      sURL              = NULL;
      type              = urlData.type;
      bValid            = urlData.bValid;
      bLocalFileFromPVX = urlData.bLocalFileFromPVX;

      if(urlData.sURL)
      {
        sURL = QTV_New_Args(URL, (URL::MaxUrlSize+1));
        if(sURL)
        {
          *sURL = *urlData.sURL;
        }
      }
      else
      {
        sURL = NULL;
      }
    }

    URLData& operator=( URLData &urlData )
    {
      type              = urlData.type;
      bValid            = urlData.bValid;
      bLocalFileFromPVX = urlData.bLocalFileFromPVX;
      if ((NULL != sURL) && (NULL != urlData.sURL))
      {
        *sURL = *urlData.sURL;
      }
      return *this;
    }
  };
  URLData url;
  QCCritSectType URL_CS;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
   class AppData
   {
   public:
    QtvPlayer::AppParametersT data;
    bool bValid;
    long startTime;
    long stopTime;
    unsigned int flags;

      AppData( void )
      :  bValid( false ),
         startTime( 0 ),
         stopTime( 0 ),
         flags( 0 )
    {
         data.Secured = false;
         data.UserPlaybackAllowed = false;
         data.RepositioningAllowed = false;
         data.DownloadPlaybackControl = QtvPlayer::NO_PLAYBACK;
         data.EndOfClipAction = NO_END_OF_CLIP_ACTION;
         data.NextURL = QTV_New_Args(URL, (URL::MaxUrlSize + 1));
    }

    ~AppData( void )
    {
      if ( data.NextURL )
      {
        QTV_Delete(data.NextURL);
        data.NextURL = 0;
      }
    }

      AppData& operator=( const AppData& other )
    {
         bValid = other.bValid;
         startTime = other.startTime;
         stopTime = other.stopTime;
         flags = other.flags;
         data.Secured = other.data.Secured;
         data.UserPlaybackAllowed = other.data.UserPlaybackAllowed;
         data.RepositioningAllowed = other.data.RepositioningAllowed;
         data.DownloadPlaybackControl = other.data.DownloadPlaybackControl;
         data.EndOfClipAction = other.data.EndOfClipAction;
         if ( data.NextURL )
      {
            if ( other.data.NextURL )
            {
               data.NextURL->SetUrl( (const char *)other.data.NextURL );
      }
            else
    {
               data.NextURL->Reset();
      }
    }

      return *this;
    }

   private:
      // Copy construction is unsupported.  If this ever is
      // to be supported, care must be taken to do a proper depth copy of
      // the AppParametersT member contained herein.
      AppData(const AppData& );
   };

   AppData appData;
   QCCritSectType appData_CS;
#endif

  bool bStop; //Flag to indicate stop

  bool bNotifyUnknownCodec; //Flag to indicate unknown codec has been notified

  /* store challenge rcvd from server */
  char serverChallenge[QTV_MAX_CHALLENGE_BYTES + 1];

  /* store authorization info provided from user */
  /* we send it out as is */
  char userAuthInfo[QTV_MAX_CHALLENGE_BYTES + 1];

  /* Flag to remember if the player was challenged to authenticate*/
  bool challenged;

  /* Flag to record if Qtv application has indicated it's ability to
     provide authorize sessions */
  bool canAppAuthenticate;

#ifdef FEATURE_QTV_PSEUDO_STREAM
  uint32 player_pp_entry_time;
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  uint32 player_http_pause_entry_time;
  uint32 http_startupTime;
  uint32 http_bufferingResumeTime;
  uint32 parser_pause_entry_time;
#endif /*defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD))*/

#if defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
  bool http_bContentPurge;
  int32 http_ContentSizeFromSDP;
#endif

  bool bAbort;
#ifdef FEATURE_QTV_DIVX_DRM
  /*
  * Variable to determine whether player needs to call commit before
  * DivX rental playback can begin.
  * For rental clips, QTV reports QTV_PLAYER_CONSUME_RENTAL_VIEW_CONFIRMATION
  * once PB_READY is reported. Commit will increase the current playback count.
  * It is assumed that for rental clips, if user calls PlayClip,
  * it means QTV is authorized to consume one of the views.
  */
  bool m_bCallCommitPlayback;
#endif

  struct FrameInfo
  {
    QtvPlayer::FrameInfoT info;
    bool bValid;
    bool bInfoSetSinceLastPrep;
    void *pMetaData;   /* Meta data for use by upper layers */
  };
  FrameInfo frameInfo;
  FrameInfo frameToRelease;
  QCCritSectType frameToRelease_CS;
  QCCritSectType frameInfo_CS;
  uint32 numBuffersSent;

  struct ClipInfo
  {
    QtvPlayer::ClipInfoT info;
    bool bValid;
  };
  ClipInfo clipInfo;
  QCCritSectType clipInfo_CS;

  // Function to allocate memory in OEM heap
  QtvPlayer::MallocOutputBufferT OEMMallocBuffer;
  QtvPlayer::FreeOutputBufferT OEMFreeBuffer;

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  QtvPlayer::IsMimeTypeSupportedT isMimeTypeSupported;
  QtvPlayer::FetchBufferedDataSizeT DefaultFetchBufferedDataSize;
  QtvPlayer::FetchBufferedDataT DefaultFetchBufferedData;

  bool setupHttpParameters();
  bool fileOpened;
#endif

  char userAgentMAN[QTV_MAX_USER_AGENT_MAN_BYTES];
  char userAgentCTN[QTV_MAX_USER_AGENT_CTN_BYTES];
  char userAgentVCT[QTV_MAX_USER_AGENT_VCT_BYTES];
  char userAgentACT[QTV_MAX_USER_AGENT_ACT_BYTES];
  char userAgentSCT[QTV_MAX_USER_AGENT_SCT_BYTES];
  char userAgentCLR[QTV_MAX_USER_AGENT_CLR_BYTES];
  char userAgentLCD[QTV_MAX_USER_AGENT_LCD_BYTES];
  char userAgentVER[QTV_MAX_USER_AGENT_VER_BYTES];

  char userAgent[QTV_MAX_USER_AGENT_BYTES];

  OSCL_STRING userAgentParams;
  UserAgentParamsType userAgentParamsType;

#ifdef FEATURE_QTV_NET_POLICY
  /* Network policy information to be used by the streamer
   * while bringing up the network
   */
  QtvPlayer::NetPolicyInfo netPolicyInfo;
#endif /* FEATURE_QTV_NET_POLICY */

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
   struct DownloadInfo
  {
    QtvPlayer::DownloadInfoT info;
    bool bValid;
  };
  DownloadInfo downloadInfo;
  QCCritSectType downloadInfo_CS;

#endif

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
    char rootpath[QTV_MAX_FILENAME_BYTES];
#endif
    char pdrootpath[QTV_MAX_FILENAME_BYTES];   

#if (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION)
#error code not present
#endif /* (defined FEATURE_QTV_QDSP_RELEASE_RESTORE && defined FEATURE_QTV_QOS_SELECTION) */


  /////////////////////////
  // Player data.
  /////////////////////////

  //crit sect for avsync
  QCCritSectType AVSync_CS;

  boolean bSelectAudio;
  boolean bSelectVideo;

  //init parameters

  //internal clip information
  class Clip
  {
  public:
     Clip()
     {
        Init();
     };

     void Init()
     {
        videoURN = NULL;
        audioURN = NULL;
        textURN  = NULL;
        bValid   = false;
        pAudioBitfile         = NULL;
        pVideoBitfile         = NULL;
        pAudioLocalFilename   = NULL;
        pVideoLocalFilename   = NULL;
        pVideoMpeg4Playback   = NULL;
        pAudioMpeg4Playback   = NULL;
        pVideoLocalBuffer     = NULL;
        pAudioLocalBuffer     = NULL;
        videoBufSize          = 0;
        audioBufSize          = 0;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        pTextMpeg4Playback    = NULL;
        pTextBitfile          = NULL;
        pTextLocalFilename    = NULL;
        pTextLocalBuffer      = NULL;
        textBufSize           = 0;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_QTV_PSEUDO_STREAM
        bPseudoStreaming            = false;
        pseudoStreamWritePtrOffset  = 0;
        mediaCounter                = 0;
#endif

 #if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
        bHTTPStreaming           = false;
#endif   /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
        /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
        HTTPStreamWritePtrOffset = 0;
        mediaCounter             = 0;
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        FetchBufferedDataSize    = 0;
        FetchBufferedData        = NULL;
        m_QtvInstanceHandle      = NULL;
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)*/

        bBcastISDB               = false;
        bBcastFLO                = false;
#ifdef FEATURE_QTV_GENERIC_BCAST
        bBcastStream             = false;
        bLiveBcast               = false;


        bBcastGeneric            = false;
        eGenericBcastMediaID     = Common::GENERIC_BCAST_MEDIA_INVALID;
#endif
        // Initialize the mute combination inside the clip object
        bmQueuedMute             =  0;
        bmMute                   =  0;
        bStillImage              = false;
        bM4vFile                 = false;
        bStreaming               = false;
        bDownload                = false;
        bHasAudio                = false;
        bHasAudioWhenNotSuspended = false;
        bHasVideo                = false;
        bHasEnhancedVideo        = false;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
        bHasText                 = false;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

        bQtvHttpStreaming        = false;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        pMpeg4Download        = NULL;
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
        bmMute = 0;
        bmQueuedMute = 0;
     }

    URNData* videoURN;
    URNData* audioURN;
    URNData* textURN;
    bool bValid;

    //Bitfile
    OSCL_FILE *pVideoBitfile;
    OSCL_FILE *pAudioBitfile;
    //pointer to the filename for local file or pseudo-streaming.
    char * pVideoLocalFilename;
    char * pAudioLocalFilename;
    //Mpeg4 data interface
    Media * pVideoMpeg4Playback;
    Media * pAudioMpeg4Playback;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    Media * pTextMpeg4Playback;
    char * pTextLocalFilename;
    unsigned char * pTextLocalBuffer;
    uint32 textBufSize;
    OSCL_FILE *pTextBitfile;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    unsigned char * pVideoLocalBuffer;
    unsigned char * pAudioLocalBuffer;
    uint32 videoBufSize;
    uint32 audioBufSize;

#ifdef FEATURE_QTV_PSEUDO_STREAM
    bool   bPseudoStreaming;
    uint32 pseudoStreamWritePtrOffset;
#endif

 #if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
    bool   bHTTPStreaming;
#endif   /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
        /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */


#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    uint32 HTTPStreamWritePtrOffset;
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize;
   QtvPlayer::FetchBufferedDataT FetchBufferedData;
   QtvPlayer::InstanceHandleT m_QtvInstanceHandle;
   uint32 mediaCounter;
#endif /*defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)*/

     //File Type (mpeg-4 or video bitstream)
     bool bM4vFile;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
     DownloadMedia  *pMpeg4Download;
#endif

     //Media components
     bool  bHasAudio;
     bool  bHasAudioWhenNotSuspended;
     bool  bHasVideo;
     bool  bHasEnhancedVideo;
     bool  bStillImage;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
     bool  bHasText;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

     //type of playback.
     bool  bStreaming;
     bool  bDownload;
     bool  bBcastISDB;
     bool  bBcastFLO;
     bool bQtvHttpStreaming;
#ifdef FEATURE_QTV_GENERIC_BCAST
     bool bBcastStream;
     bool bLiveBcast;


     bool bBcastGeneric;
     Common::GenericBcastMediaSourceID eGenericBcastMediaID;
#endif

     uint32 bmQueuedMute; /* Queued Mute bitmask */
     uint32 bmMute; /* Mute bitmask */
  } ;

  Clip clip;

  #ifdef FEATURE_QTV_STREAM_RECORD
  char recFileName [QTV_MAX_URN_BYTES+1];
  bool bRecFileOverWrite;
  uint32 recDuration;
  uint8 recMode;
  bool bRecPending;
  bool bRecording;
  #endif


  //PVX file interface.
  struct PVXInterfaceT
  {
    AppFile  appFile;
    bool     bValid;
    int      nPlaylistLength;
    int      nPlaylistIndex;
    bool     bPlaylistComplete;
  };
  PVXInterfaceT pvxIF;

  //SDP file interface.
  class SDPInterfaceT
  {
  public:
    SDPFile  sdpFile;
    SDP_Parser         sdpParser;
    URL                url;
    QtvPlayer::URLTypeT URLType;
    KEY_DEF          * pKey;
    SDPInfo          * pSDPInfo;
    bool               bValid;
    int                nPlaylistLength;
    int                nPlaylistIndex;
    bool               bPlaylistComplete;

    void SetConfigObject(QtvConfig *config)
    {
      sdpParser.init(config);
      sdpFile.getSDPParser().init(config);
    }
  };
  SDPInterfaceT sdpIF;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
  //This tells whether download is active during the playback states.
  bool bDownloading;
  bool bDownloadDone;
  int FileSavedToBuffer;
  int nPvxPreroll;
  char* nPvxFilePath;
  bool FileNeedstobeSaved;
  #endif

  //Player control data
  State playerState;
  State lastPlayerState;
  AudioConcurrencyState playerAudioConcState;
  QtvPlayer::AudioSourceFormatT audioSource;
  AudioConcurrencyState prevAudioConcState;

  bool threadIdle; // indicates whether the player thread is IDLE or not

  enum ParserStatusCode
  {
    PARSER_IDLE,
    PARSER_READY,
    PARSER_PAUSE,
    PARSER_RESUME,
    PARSER_FAILED
  };
  ParserStatusCode parserStatus;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
  struct DownloadState
  {
    int playbackTime;
    int bytesRemaining;
    int fileSize;
  };
  DownloadState downloadState;
#endif

  //player thread info
  static QCThreadControl playerTC;

  //message receive block.
  static QCMessageReceive playerER;

#ifndef FEATURE_WINCE
  /* Current Active Video Player Instance */
  PlayerBaseIF* pActiveVideoPlayer;

  /* Current active audio player inst */
  AUDIO_PLAYER_ID activeAudioPlayerId;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  //Timed Text player object
  TimedText         textPlayer;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
#endif   /*    FEATURE_WINCE   */

  //Streamer object
  QtvStream * pQtvStream;
  Streamer* streamer;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  PVDL * pPVDL;
  Download download;
#endif

#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  HttpDownload httpdownload;
  HttpDataInterface* dataInterface;
  OemHeaders *httpHeaderManager;
#endif

  //Duration of filled preroll buffer
  uint32 nPrerollDurationFilled;

  //Percentage of preroll duration filled
  uint32 nPrerollPercentageFilled;

#ifdef FEATURE_QTV_STREAM_RECORD
  uint32 recorded_clip_duration;
#endif


  //App control
  class AppControl
  {

  public:
    AppControl() { }

    ~AppControl() { }

    //for a streaming clip, this says to start playing
    //when connected and tracks selected.
    //for a download, this says to start pseudo streaming
    //when ready
    bool bStartPlaying;

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    uint32 mediaCounter;
#endif

#ifdef FEATURE_QTV_PSEUDO_STREAM
    unsigned int nPseudoStreamDownloadRate;
#endif /*  FEATURE_QTV_PSEUDO_STREAM */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    unsigned int nHTTPStreamDownloadRate;
#endif /*  FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */


    //callback data
    CallbackData callback;

    //when set audio will not start until video starts rendering.
    bool bDelayAudio;

    //when set audio video sync is needed because user has single stepped
    bool bStepAVSyncNeeded;

    //flag telling whether playback mode is local file.
    bool bPureLocalFile;

    // flag indicating whether or not to delay audio output until the first
    // frame has been rendered and audio has finished buffering
    bool bDelayAudioOutput;

    // flag indicating whether audio has finished buffering
    bool bDoneAudioBuffering;

    // flag indicating whether the first frame from the beginning,
    // or after repositioning has been rendered
    bool bReceivedFirstFrame;

    // flag indicating whether player resume is pending,
    bool bResumePending;

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    //for a download, this says to start downloading
    //when connected and tracks selected.
    bool bStartDownloading;



      //flag telling whether pseudo-stream is from "asap" flag or not.
    bool bPvxStart;

    //storage size to send to downloader.
    unsigned long nStorageSize;

#endif
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    //flag telling whether to resume previous download.
    bool bResumeDownload;
#endif
    // True if any of the media decoder has started yet. Used to determine if we
    // need to set the restart flag for SetPlayTimes()
    bool bHasMediaStarted;

    bool bPauseMediaAfterStartPlaying;
    int nStreamStartsToIgnore;

    int nVideoAbortsToIgnore;
    int nAudioAbortsToIgnore;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    int nTextAbortsToIgnore;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    // Extra members to hold callback data used in Notify()
    uint32 cbData1;
    uint32 cbData2;
    uint32 cbData3;

    OSCL_STRING cbString1;
 };
  AppControl appControl;

  //repositioning control
  struct ReposControl
  {
    bool bTimesValid;
    bool bRestart;
    long nStart;
    long nRestart;
    long nStop;
    //for a streaming clip, these are the last times sent to the streamer
    long nStreamStop,nStreamStart;

    char reposType;
  };
  ReposControl reposControl;

  bool bLoopTrackFlag;

  int playbackID;
  int streamID;

  //This flag indicates whether or not to take MAPI path for streaming
  //It is set based on whether streaming session is multicast/unicast
  bool m_useMAPILogic;

 #if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
  int downloadID;
  #endif

  bool redirectFlag;
  int32 redirectTime;
  URL * redirectURL;

  #ifdef FEATURE_QTV_STREAM_RECORD
  enum RecorderStatusCode
  {
  FILE_SYS_LIMIT_APPROACHING,
  FILE_SYS_LIMIT_REACHED,
  FILE_ALREADY_EXISTS,
  FILE_OW_FAILURE,
  FILE_SYS_WRITE_ERR,
  GENERIC_ERR,
    RECORDING_COMPLETED
  };
  #endif

  // Internal error codes.
  /* If you modify this enum, please make the corresponding change to
   * Mpeg4Player::ErrorStrings.  Don't conditionally compile out any
     of the strings in ErrorStrings[] !
     NOTE: Explicitly set all enum values to handle conditional
     compilation
   */
  enum ErrorCode
  {
    MPEG4PLAYER_MIN_ERROR = 0,
    OPEN_CLIP_ERROR       = MPEG4PLAYER_MIN_ERROR
    ,PLAY_CLIP_ERROR = 1
    ,STOP_ERROR = 2
    ,START_PSEUDO_STREAM_ERROR = 3
    ,START_PLAYING_ERROR = 4
    ,RESUME_PLAY_ERROR = 5
    ,QTV_PSEUDO_RESUME_ERROR = 6
  ,QTV_PSEUDO_PARSER_FAILURE = 7
    ,RESTART_ACTIVE_CLIP_ERROR = 8
    ,PREP_AV_ERROR = 9
    ,STREAM_START_ERROR = 10
    ,STREAM_CONNECT_ERROR = 11
  ,QTV_PARSER_INIT_ERROR = 12
    ,QTV_SKIP_CLIP_ERROR = 13
    ,DOWNLOAD_TRACK_SELECT_ERROR = 14
    ,START_HTTP_STREAM_ERROR = 15
    ,QTV_HTTP_RESUME_ERROR = 16
    ,MPEG4PLAYER_MAX_ERROR = QTV_HTTP_RESUME_ERROR
  };
  static const char* const ErrorStrings[];

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  class DownloadConfigFile
  {
  public:
    URL *sURN;
    char sFilename[QTV_MAX_FILENAME_BYTES];

    DownloadConfigFile()
    {
      sURN = NULL;
    }

    bool Init()
    {
      sURN = QTV_New_Args(URL, (URL::MaxUrlSize));
      if (NULL == sURN)
      {
        return false;
      }
      return true;
    }

    ~DownloadConfigFile()
    {
      if (NULL != sURN)
      {
        QTV_Delete(sURN);
      }
    }
  };
#endif


#ifdef FEATURE_FILE_FRAGMENTATION
  struct ProcessFragmentData
  {
    uint32 lastFragmentBytes;
    uint32 lastFragmentFrames;
  };
  ProcessFragmentData ProcessAudioFragmentData;
  ProcessFragmentData ProcessVideoFragmentData;
#endif


  //Internal message handlers.
  bool OpenURN(const PV_OPEN_URN_type * pEvent) ;
  bool OpenURN(const PV_OPEN_BUFFER_URN_type * pEvent) ;

#ifdef FEATURE_QTV_DRM_DCF
  //OpenURN to start parsing DCF media
  bool OpenURN(PV_OPEN_INPUTSTREAM_URN_type * pEvent) ;
#endif

#ifdef FEATURE_QTV_PSEUDO_STREAM
  bool OpenPseudoStream(const QTV_PS_STREAM_OPEN_EVENT_type * pEvent);
  void setPseudoStream(unsigned char *pBuf,uint32 bufSize,
                       QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize,
                       QtvPlayer::FetchBufferedDataT FetchBufferedData,
                       QtvPlayer::InstanceHandleT m_QtvInstanceHandle,
             URNData &destURN);
  void PseudoPauseHandler(const QTV_PS_PAUSE_EVENT_type *pEvent);
#endif /* FEATURE_QTV_PSEUDO_STREAM */

#if defined (FEATURE_FILE_FRAGMENTATION) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  bool PauseAudio(QTV_PAUSE_AUDIO_type *pEvent);
  bool PauseVideo(QTV_PAUSE_VIDEO_type *pEvent);
  bool PauseText(QTV_PAUSE_TEXT_type *pEvent);
  bool ResumeAudio(QTV_RESUME_AUDIO_type *pEvent);
  bool ResumeVideo(QTV_RESUME_VIDEO_type *pEvent);
  bool ResumeText(QTV_RESUME_TEXT_type *pEvent);
#endif /* FEATURE_FILE_FRAGMENTATION || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

  #ifdef FEATURE_QTV_PSEUDO_STREAM
  void ParserStatus(const QTV_PS_PARSER_STATUS_EVENT_type *pEvent);
  bool ProcessPseudoStream(const QTV_PS_PROCESS_PSEUDO_STREAM_type *pEvent);
#endif /*FEATURE_QTV_PSEUDO_STREAM*/
  bool PlayClip(const PV_PLAY_CLIP_type * pEvent);
  bool PlayClip(int32 startTime,
                int32 stopTime,
                const URL* urlToSwitch = NULL,
                Common::PlaybackSpeedType pbSpeed = Common::PLAYBACK_SPEED_NO_CHANGE);

#ifdef FEATURE_QTV_STREAM_RECORD
  bool RecordClip(const PV_RECORD_CLIP_type * pEvent);
  bool initStreamRecording(char *filePath);
  bool stopStreamRecording(const PV_RECORD_STOP_type * pEvent);
  static void recorderCallback(/*RecorderStatusCode*/ int status,
                               void* client_data);
#endif

  bool RegisterForCallback(const PV_REGISTER_FOR_CALLBACK_type * pEvent);
  bool RegisterForCallback(const PV_REGISTER_FOR_CALLBACK_EX_type * pEvent);
  bool Stop(PV_STOP_type *pEvent);
  bool Pause(PV_PAUSE_type *pEvent);
  bool PauseMedia();

  bool Suspend(QTV_SUSPEND_type *pEvent);
  bool Resume(QTV_RESUME_type *pEvent);
#ifdef FEATURE_QTV_IN_CALL_PHASE_2
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 */
  void GenerateSuspendEvent();
  bool RestoreResumeAudio(long stop);
  void UpdateAudioConcState();
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */

#ifdef  FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
  bool PlayNextFrame(PV_NEXT_FRAME_type *pEvent);
  bool PlayPrevFrame(PV_PREV_FRAME_type *pEvent);
#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
  bool RotateVideo(PV_VIDEO_ROTATE_type *pEvent);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */
#if defined (FEATURE_MP4_FRAME_TRANSFORMATIONS) || defined (FEATURE_QTV_XSCALE_VIDEO)
  bool ScaleVideo(PV_VIDEO_SCALE_type *pEvent);
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS || FEATURE_QTV_XSCALE_VIDEO */

  bool SetAudioOverride(PV_SET_AUDIOOVER_type *pEvent);
  void AudioStatus(const AUDIO_STATUS_type *pEvent);
  void AudioTiming(const AUDIO_TIMING_type *pEvent);
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  void AudioSpec(const AUDIO_SPEC_type *pEvent);
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
  void AVTick(const AV_TICK_type *pEvent);
  void VideoStatus(const VIDEO_STATUS_type *pEvent);
  void StreamerStatus(const STREAMER_STATUS_type *pEvent);

bool CheckForAllMute(uint32 bmTrackSelected);

#ifdef FEATURE_QTV_GENERIC_BCAST
  bool GenericBcastDescribeClip(Media *pVideo);
  bool GenericBcastDescribeTracks(Media *pVdieo);
  void GenericBcastMediaStatusHandler(const GENERIC_BCAST_STATUS_type *pEvent);
  const char* GenericBcastConvertStatusToString(Common::GenericBcastMediaStatusCode eStatus);
  const char* ConvertGenericBcastSourceToStr(Common::GenericBcastMediaSourceID eID);




  /***********************************/
  /*  Command space object interface */
  /***********************************/
  #define MAX_COMMAND_SPACE 100
  enum CmdType{
    EXTERNAL_REPOSITION_CMD,
    INTERNAL_REPOSITION_CMD,
    EXTERNAL_SEEK_SYNC_CMD,
    INTERNAL_SEEK_SYNC_CMD,
    EXTERNAL_PLAY_SYNC_CMD
  };
  struct CmdEntry{
    bool bValid;
    uint32 token;
    uint32 cmd;
    uint32 uSeqNo;
  };
  uint32 uCmdFreeSpace;
  CmdEntry aCmdSpaceQ[MAX_COMMAND_SPACE];

  void InitCmdSpace();
  bool CmdSpaceAvailable();
  bool CmdInsert(CmdType cmd, uint32 token,uint32 uSeqNo);
  bool CmdFree(uint32 uSeqNo,uint32 &token, CmdType &cmd);

  /*************************************/
  /* SelectPlaybackTracks interface    */
  /*************************************/
  QtvPlayer::ReturnT
  SelectPlaybackTracks(uint32 bmTrackSelected,
                       uint32 nAudioTrackID,
                       uint32 nVideoTrackID,
                       uint32 nTextTrackID,
                       uint32 &bmSucceeded);

  // PlaySyncFrame interface
  QtvPlayer::ReturnT
  PlaySyncFrame(sint31 nSyncOffset,
                uint32 uTransID);

  // Seek To Sync interface
  QtvPlayer::ReturnT
  SeekToSync(sint31 nSyncOffset,
             bool   bExtCmd,
             uint32 uTransID);

  /* Seek sync to a timestamp */
  void SeekToSyncFromTime(
    sint31 nSyncOffset,
    bool bExtCmd,
    uint32 uTransID,
    long timeStamp);

  /* Start playing from the timestamp returned by SeekToSyncFromTime */
  bool StartPlayingFromSeekedPoint(long startTime);

  // Reposition interface
  QtvPlayer::ReturnT
  Reposition(uint32 nTimestamp,
             bool   bExtCmd,
             uint32 uTransID);

  //Activate DSP : Takes the comparison result and playback position as input.
  bool ActivatePlayer(unsigned int nResult,unsigned long start);

  //Freeze or Activate DSP
  bool StateBasedFreezeOrActivate(bool bFreeze, unsigned int nResult,unsigned long start);

  //Release DSP
  bool FreezePlayer(unsigned int nResult);

  // Audio Player reposition
  bool AudioMediaReposition(unsigned int nResult,unsigned long start, long stop);

  // Audio Player Prep
  bool AudioMediaPrep(unsigned int nResult);

  // Audio player start
  bool AudioMediaStart(unsigned int nResult, long stop);

  // Video Player reposition
  bool VideoMediaReposition(unsigned int nResult,unsigned long &start, long stop);

  // Video Player Prep
  bool VideoMediaPrepOrEnable(unsigned int nResult);

  // Video Player start
  bool VideoMediaStartOrResume(unsigned int nResult,long stop);

  // Text Player Reposition
  bool TextMediaReposition(unsigned int nResult,unsigned long start, long stop);

  // Text Player Prep
  bool TextMediaPrep(unsigned int nResult);

  // Text Player Start
  bool TextMediaStartOrResume(unsigned int nResult, long stop);

  // Generic Broadcast Upgrade Tracks
  bool GenericBcastUpgradeTracks(unsigned int nResult);

  // Act On User Track Selection
  bool ActOnUserTrackSelection(int nAudID, int nVidID, int nTextID);

  // Act on Network Track List Update
  bool ActOnNetworkTrackListUpdate(void);

  // Update the track list data structure changes to to the player clip information
  void UpdateClipInfo(Media *pMedia,unsigned int nResult);

  // Upgrade Qtv Media Type in general based on teh comparison result.
  QtvPlayer::MediaTypeT UpgradeQtvMediaType(Media *pMedia, unsigned int nResult,
                                            QtvPlayer::MediaTypeT eType);

  // Upgrade Media Type for video track addition
  QtvPlayer::MediaTypeT UpgradeMediaTypeForVideo(Media *pMedia,
                                                 QtvPlayer::MediaTypeT eType);

  // Upgrade Media Type for audio track addition
  QtvPlayer::MediaTypeT UpgradeMediaTypeForAudio( QtvPlayer::MediaTypeT eType);

  // Upgrade Media Type for text track addition
  QtvPlayer::MediaTypeT UpgradeMediaTypeForText(QtvPlayer::MediaTypeT eType);




#endif /* FEATURE_QTV_GENERIC_BCAST */

  // Map the Media Codec to player codec understandable by the application.
  QtvPlayer::TrackTypeT MapMediaCodecToQtvPlayerCodec(Media::CodecType codec);


/*************************************/
  /*  Mute & Unmute Feature interface  */
  /*************************************/
  QtvPlayer::ReturnT
  MutePlaybackTracks(uint32 bmTrackSelected,
                    uint32 bmTrackState,
                    uint32 *bmSucceeded);

  // Track specific mute methods
  QtvPlayer::ReturnT MuteAudio();
  QtvPlayer::ReturnT MuteVideo();
  QtvPlayer::ReturnT MuteText();

  // Unmute method
  QtvPlayer::ReturnT UnmuteMedia(uint32 &bmUnmuteCombo);

  // Unmute Utils
// Method -1 which resumes all the clip tracks
  bool ResumeAll(uint32 );
  
  //  Mehtod-2 which resumes the specific tracks only without affecting others
  bool ResumeAll(uint32 uPlayTime,uint32 bmTrackSelected);

  // Suspend & Flush all applicable tracks.
  bool SuspendMedia();

  // Suspend and FLush the specific tracks only
  bool SuspendMedia(uint32 bmTrackSelected);

  // Start all applicable tracks.
  bool StartMedia();

  // Start the specific tracks only
  bool StartMedia(uint32 bmTrackSelected);

  // Prep the specific tracks only
  bool PrepSpecificTracks(uint32 bmTrackSelected);

  // Calculate the new position
  void CalculateNewPosition(unsigned long &ulPlayTime,
                            uint32 &uPlayTimestamp);

  uint32 RepositionMediaDueToUnmute(uint32 bmUnmuteCombo, uint32 &bmReturnUnmuteCombo);

  void GetCodecDetails(Media::CodecType eMediaCodec,
                       QtvPlayer::TrackTypeT &eQtvCodec,
                       QtvPlayer::MediaTypeT &eMediaType);

  void UpdateLocalClipInfo(Media * pMedia,unsigned int nResult);
  bool LocalMediaPLaybackUpgradeTracks(unsigned int nResult);
  enum EndCode
  {
    COMPLETE,ABORT,STOPPED,ABORT_MEM_ERROR,FCS_COMPLETE,FCS_SWITCH_ERROR
  };
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  bool DeleteDownloadSession(PV_DELETE_DOWNLOAD_SESSION_type *pEvent);
  bool StopDownload(PV_STOP_DOWNLOAD_type *pEvent);
  bool DownloadClip(PV_DOWNLOAD_CLIP_type *pEvent);
  bool ResumeDownload(PV_RESUME_DOWNLOAD_type *pEvent);
  bool PrepDownloadClip(PV_DOWNLOAD_CLIP_type *pEvent);
  bool InitDownloadInfo(char * pName);
  bool InitDownloadConfig();
  void DownloadStatus(const DOWNLOAD_STATUS_type *pEvent);
  void DownloadFileSizeStatus(const DOWNLOAD_FILE_SIZE_STATUS_type *pEvent);
  unsigned long GetPVDLFilesize();
  void SetPVDLFilesize(unsigned long fsize);
  int GetDownloadProgress();
  unsigned long GetDownloadFilesize();
  void UpdateDownloadState();
  void CleanupDownload();
  void HandlePurgeAfterDownload();
  void PurgeDownload();
  void DownloadComplete(EndCode code);
  void SetAppData();
  OSCL_FILE * OpenDownloadConfigFile(char *mode);
  OSCL_FILE * OpenDownloadDatFile(char *mode);
  bool DeleteDownloadDatFile(void);
  bool StartPseudoStream(bool bPvxStart=false);
#endif

  void RTSPStatusNotify(const STREAMER_STATUS_type *pEvent);

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  void TimedTextStatus(const CPVTEXT_STATUS_type *pEvent);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifdef FEATURE_FILE_FRAGMENTATION
    bool SkipClip(const QTV_SKIP_CLIP_type * pEvent);
    bool RestartActiveClip(int32 skipNumber);
    bool RepositionAccessPoint(int32 skipNumber);
#endif /*FEATURE_FILE_FRAGMENTATION*/

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  bool OpenHTTPStream(const QTV_HTTP_STREAM_OPEN_EVENT_type * pEvent);
  void setHTTPStream(unsigned char *pBuf,
                   uint32 bufSize,
                   QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize,
                   QtvPlayer::FetchBufferedDataT FetchBufferedData,
                   QtvPlayer::InstanceHandleT m_QtvInstanceHandle,
           URNData &destURN);
  void HTTPParserStatus(const QTV_HTTP_PARSER_STATUS_EVENT_type *pEvent);
  bool ProcessHTTPStream(const QTV_PROCESS_HTTP_STREAM_type *pEvent);
  bool HTTPPause(const QTV_HTTP_STREAM_BUFFER_UNDERRUN_EVENT_type *pEvent);
  bool HTTPCanPlayTracks(uint32 pbTime);
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  void ProcessHTTPBufferUpdate(const QTV_HTTP_BUFFER_UPDATE_type *pEvent);
  void ProcessHTTPStateEvent(const QTV_HTTP_EVENT_type *pEvent);
#endif


#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  void HttpPullTimerStart();
  void HttpPullTimerStop();
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

#ifdef FEATURE_QTV_PLAYLIST
  bool Skip(const QTV_SKIP_type * pEvent);
  bool Skip(const char *playlistName, int32 clipIndex,
            bool isClipIndexRelative, int32 offset, int32 when = -1);

#ifdef FEATURE_QTV_SERVER_SIDE_PLAYLIST
  bool SwitchToNextClipInServerSidePlaylist(bool bUserInitiated);
#endif /* FEATURE_QTV_SERVER_SIDE_PLAYLIST */
#endif /* FEATURE_QTV_PLAYLIST */

#ifdef FEATURE_QTV_FCS
#error code not present
#endif /*  FEATURE_QTV_FCS */

  //Internal Player implementation routines
  void SetURN(const URL &sURN, URNData &destURN);
  void SetURN(unsigned char *pBufURN, uint32 bufSize, URNData &destURN);

#ifdef FEATURE_QTV_DRM_DCF
  void SetURN(dcf_ixstream_type inputStream, URNData& desstURN);
#endif

  bool SetURL();
  bool SetClipType();
  bool SetULHC(int x, int y, bool bCenter);
  void SetClipInfo(Media * pVideo, int VideoFileSize, Media * pAudio,
                   int AudioFileSize, Media * pText, int TextFileSize);
  QtvPlayer::MediaTypeT GetPVPlayerMediaType(Media::MediaFileType eMp4Filetype);
  QtvPlayer::ClipExtInfoKindT GetClipExtInfo(Media * p, ClipInfo &clipInfo);
  bool SetClipComponents(bool bAudioSelected,bool bVideoSelected,
                                             bool bTextSelected);
bool DescribeAndSelectTracks(Media *p,bool &bAudioSelected,bool &VideoSelected,
                             bool &TextSelected);

  void HandleTrackListChange(ITrackList *trackList /* in */,
                             Media *media,
                             bool &bAudioSelected,
                             bool &bVideoSelected,
                             bool &bTextSelected);

  void NotifyTrackListChange(ITrackList *trackList /* in */);

  bool StartAudioVideo();

  bool OpenClip();
  void OpenPlaybackSession();
  bool OpenBitstream();
  bool OpenStream(KEY_DEF *p);
  bool OpenLocalFile();

  #ifdef FEATURE_QTV_GENERIC_BCAST
  bool OpenGenericBcastMediaSource();
  #endif
  bool OpenBcastFLO();
  bool PrepAudioVideo(bool isReposSuccessful);
  bool PrepAudio();
  bool PrepVideo();
  bool PrepText();
  bool StartConnecting();
  bool DescribeClip(Media *pVideo, int VideoFileSize, Media *pAudio,
                    int AudioFileSize, Media *pText, int TextFileSize);
  bool StartPlaying();
  bool ResumePlay();
  bool StartNextURL();
  void SetPlayTimes(long start,long stop,bool brestart);
  bool GetPlayTimes(long &start,long &stop);
  bool RestartActiveClip();

  void CleanupPlayback();
  static void PlayerThread();

  void PlaybackComplete(EndCode code);

  void SetState(State newState) ;

  void GetCallback(CallbackData &cb, QtvPlayer::PlayerStateRecordT &ps);
  void Callback(QtvPlayer::PlayerStatusT status,
                CallbackData cb,
                QtvPlayer::PlayerStateRecordT ps);
  void GetPlayerState(QtvPlayer::PlayerStateRecordT &ps);

  void InitClip();
  void InitURL();
  void InitURN();
#ifdef SHOW_INFO
  void ShowClip();
  void ShowClipInfo();
#endif /* SHOW_INFO */
  void InitDefaults();

  bool IsPlaying();
  bool IsPaused();

  bool IsDone();
  void Buffering();
  void Playing();
  void DonePlaying(const bool bError,const bool bVideo);
  void ShowFlags();

  void ShowStatus(QtvPlayer::PlayerStatusT status,
                  QtvPlayer::PlayerStateRecordT ps,
                  Mpeg4Player::CallbackData);


  void VideoRendering();
  bool StartAudio();
  void AudioBufferingDone();

  void AudioError();
  void VideoError();
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  void TimedTextError();
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  void StreamError();
  void HandlePlaybackAsap();
  void HanldePlaybackAfterDownload();
  void UpdateStopTime(long stop);
  void ResetPlaybackData();

  uint32  SetLocalFileSize(const char *pFileName);
#ifdef FEATURE_QTV_DRM_DCF
  int  SetLocalFileSize(URNData& urn);
#endif
  void LogClipStatistics(QtvPlayer::AudioVideoStatisticsT &stats);
  void StopPlayer(bool &bError, bool bCloseStreamer = true);
  bool StartStreaming( const URL* urlToSwitch = NULL );
  void PlaybackErrorAbort();
  void ErrorAbort(ErrorCode code);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  bool StartDownloader();
  void StopDownloader(bool &bError);
  void DownloadErrorAbort();
  void OpenDownloadSession();
  bool OpenDownload(KEY_DEF * pKey);
  bool SetSessionStart(SESSION_START *p,KEY_DEF *pKey);
  void PurgeDownloadConfig();
#endif
#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)||defined (FEATURE_QTV_HTTP_DOWNLOAD)
  void ResetDownloadData();
#endif
#ifdef FEATURE_QTV_HTTP_DOWNLOAD
  bool StartHttpDownloader();
  void StopHttpDownloader(bool &bError);
  void OpenHTTPDownloadSession();
  bool OpenHTTPDownload();
  void HttpDownloadComplete(EndCode code);
  void HttpDownloadStatus(const HTTP_DOWNLOAD_STATUS_type *pEvent);
  void HTTPRegisterCallback(QTV_HTTP_REGISTER_CALLBACK_EVENT_type *pEvent);
  char* checkforHttpBrokenDownload(uint32 &);
#endif

  /***************************************************************************
  ** Frame-buffer deallocation                                          GRB **
  **                                                                        **
  ** When the OEM layer has completed rendering a video frame buffer,       **
  ** it calls this member to notify the player that it can release          **
  ** (or reuse) the buffer.                                                 **
  **                                                                        */

public:
  /*************************************/
  /*  ReadPlaybackTracks interface     */
  /*************************************/
  QtvPlayer::ReturnT
  ReadPlaybackTracks(uint32 *nTrackCount,
                     QtvPlayer::TrackListT *pTrackList);


  /* Maps the QtvPlayer playabck speed type to Common playback speed type */
  Common::PlaybackSpeedType MapQtvPBSpeedToCommonPBSpeed(
    QtvPlayer::PlaybackSpeedT pbSpeed);

  // Get Media Information - Synchronous
  bool GetMediaInfo(void **ppInfo,unsigned int *size);

  // OEMAllocateMemory - Allocate memory in OEM space
  void *OEMAllocateMemory(uint32 size)
  {
    void *ptr = NULL;
    if(OEMMallocBuffer)
    {
      // adjusted to the new signature of QtvPlayerT::MallocOutputBufferT
      ptr = OEMMallocBuffer(NULL,NULL,size);
    }
    return ptr;
  }

  void ReleaseVideoBuffer( void *pBuffer );

  //Funtion to log the cumulative clip stats
  void LogCumulativeClipStatistics(bool resetStats);

  //Function to process all request from Media Layer
  static void MediaNotifyCB(Media::MediaLayerRequest reqType, Mpeg4Player *pThis,
                                       uint32 reqData1, uint32 reqData2);

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  /* Sets the dual mono output configuration */
  bool SetDualMonoOutput(Common::DualMonoOutputType dualMonoOutput);
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

  /*                                                                    GRB **
  ***************************************************************************/

  /**********
  ** Task-termination signalling
  **
  ** This event is set when the player thread is ready to be terminated.
  */
#ifdef FEATURE_WINCE
#error code not present
#endif
QtvPlayer::ReturnT SetVDECParameter( QtvPlayer::QtvVDECParameterIdT inputParamId,  QtvPlayer::QtvVDECParameterDataT* pInputParam);
 void setSpeed(float speed);
 void setAccDuration(int time);
private:

  static bool m_bIsTaskDone;
  static bool m_bThreadStartSync;

  static uint32 guMpeg4PlayerRefCnt;

  bool release_qos_for_qdsp_release;

  // A helper function to make logic in SetFrameInfo clearer.

  static bool IsVideoRenderingOkInState( const State state );

  /**********
  ** QTV system broadcast transmitter
  **
  ** Clients wishing to access this transmitter may obtain a pointer
  ** by calling QTV_BroadcastTransmitter::Locate( "QTV" )
  */
 float speed;
 int acceleratedDuration;
private:
  static  QTV_BroadcastTransmitterTask* m_bcastXmitTask;

  unsigned long m_nVideoPlaybackTime;
  unsigned long m_nAudioPlaybackTime;

  #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
     uint32 m_nTextPlaybackTime;
  #endif

  /*Object of QtvConfig*/
  QtvConfig qtvConfigObject;

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
  long lastAudioPosition;
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */

  long audioPositionWhenSuspended;

  //instance counter
  bool bInstance;

#ifdef FEATURE_QTV_PSEUDO_STREAM
  unsigned int ps_last_ts;
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  unsigned int http_last_ts;
#endif /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
  uint32 m_uMuteClock;
  uint32 m_uMuteTimestamp;
  unsigned long m_ulMuteDuration;
  unsigned int mediaInfoSize;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_HTTP_DOWNLOAD)|| defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  char* downloadFileName;
  void getDownloadFileName(char *tempUrl,int32 urlLen);
  void setDownloadFileName(char*);
  void getDownloadTxtFileName(char *, const int size);
#endif

  Common::PlaybackSpeedType m_playbackSpeed;
#if (defined(FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY) || \
     defined(FEATURE_QTV_FAST_PLAYBACK_AUDIO))
  /* Sets the playback speed */
  void SetPlaybackSpeed(Common::PlaybackSpeedType pbSpeed);
#endif /* FEATURE_QTV_FAST_PLAYBACK_VIDEO_ONLY ||
          FEATURE_QTV_FAST_PLAYBACK_AUDIO */

#ifdef FEATURE_QTV_GENERIC_BCAST
  /* Audio track list info */
  QtvPlayer::TrackListT m_audioTrackListInfo;

  /* Updates the track list information */
  void UpdateTrackListInfo(QtvPlayer::MediaTypeT media);

  /* Maps the Common channel configuration type to QtvPlayer channel
  ** configuration type
  */
  QtvPlayer::ChannelConfigT MapCommonChConfigToQtvChConfig(
    Common::ChannelConfigType channelConfig);

  /* Gets the rollover point from media and updates the renderer. */
  void UpdateMaxTimestamp();
  uint32 m_uMaxTimestamp;

  /* Gets the rendering queue mode from media and updates the renderer. */
  void UpdateRendererQueuingMode();

  /* Activates PCR clock sync and sets the maximum timestamp at which rollover
  ** occurs.
  */
  void ActivatePCRClockSync();

  /* Deactivates PCR clock sync and sets the maximum timestamp at which rollover
  ** occurs.
  */
  void DeactivatePCRClockSync();
#endif /* FEATURE_QTV_GENERIC_BCAST */
};

#endif /* _MPEG4PLAYER_H_ */

