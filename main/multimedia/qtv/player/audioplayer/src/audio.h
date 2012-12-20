#ifndef _AUDIO_H_
#define _AUDIO_H_
/* =======================================================================
                               audio.h
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

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audio.h#13 $
$DateTime: 2009/11/26 23:17:22 $
$Change: 1097320 $

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
#include "audioMgr.h"
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
#include "genericaudioMgr.h"
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#include "media.h"
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
#include "QtvPlayer.h"
#include "qtv_task_if.h"
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

#define MAX_EVENT_COUNT 60
#define MIN_EVENT_COUNT 50

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* Callback function used to return post an event to the client.
**
** Parameters:
**
**   pEvent:  The event to pe posted onto the client on the client
**            task context.
**   pCbData: the client data the application passed in when it created
**            the audio player in the first place.
*/
typedef void (*CLIENT_NOTIFY_CB_FN)( QCMessageType * const   pEvent,
                                     void * const            pCbData );
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
**                        Class Declarations
** ======================================================================= */


/* ======================================================================
CLASS
  EventMSG

DESCRIPTION
   event_ptr:  Implemented as an array of size MAX_EVENT_COUNT, of whatever type you need 
   readIndex:  A read index (whichever is more efficient for your processor) 
  writeIndex: A write index 
       count: A counter indicating how much data is in the array  
			  Every time you write data, you advance the write index and increment the counter. When you read data, 
			  you increase the read index and decrement the counter. If either pointer reaches n, set it to zero.


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
class EventMSG
{
private:
  struct event_info
  {
    void *event_ptr[MAX_EVENT_COUNT];	
    int readIndex, writeIndex, count;
  };
  struct event_info audio_timing_e;
  struct event_info audio_status_e;

  rex_crit_sect_type  m_event_cs;

  bool pushEvent(uint16 eventID);
  void releaseEvents(uint16 eventID);
public: 
  EventMSG();
  ~EventMSG();
  void pushEvents();
  void* popEvent(uint16 eventID);
};

/* ======================================================================
CLASS
  AudioPlayer

DESCRIPTION
  AudioPlayer will serve as a message sink for the msg dispatched via
  the qtv_task_if framework.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */

class AudioPlayer : public qtv_msg_sink_class
{

public:
  AudioPlayer(CLIENT_NOTIFY_CB_FN  playerCB,
              void*                pClientData,
              QCConditionType&     responseSync,
              qtv_task_if_class*   pDispatcher);
  virtual ~AudioPlayer();

  ///////////////////////////
  // Control by AV Player
  ///////////////////////////

  /* General demux for all the msgs sent to this message sink */
  virtual void dispatch( qtv_msg_struct* const msg_ptr );

  void initializeAudioplayer(const char *audio_type, Media *pMedia = NULL);
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  void NotifySPec();
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
  long GetElapsedTime();
  void SetTimestampfromBytes(long ts);
  inline long GetAmtBufferedInCMX()
  {
     if( audioMgr )
    {
      return audioMgr->GetAmtBufferedMsec(ZUtils::Clock());
    }
    return 0;
  }
  void ResetData();
  bool GetPosition(int32 &pos);
  void Destroy();
  bool IsDone();
  bool IsIdle();
  void SetAVSyncObj(void* pAVSync);

  enum State
  {
    IDLE,
    BUFFERING,
    PLAYING,
    REBUFFERING,
    PAUSED,
    SUSPENDED
    ,PAUSED_DISABLED
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    ,POLLING_DOWNLOAD
#endif
    ,BUFFERING_DONE
    #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
    #endif 
  };

  void Notify(State newState, Common::AudioStatusCode status);
  void Stop(bool &bError) ;
  bool GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                     Common::LastFrame &lastFrame);

  //Sets a variable to track the Audio channel switch time - MFLO)
  void SetAudioChannelSwitchTS(uint32 ChannelSwitchedTS);

#ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO
  /* Sets the playback speed */
  void SetPlaybackSpeed(Common::PlaybackSpeedType ePlaybackSpeed);
#endif /* FEATURE_QTV_FAST_PLAYBACK_AUDIO */

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
#ifdef FEATURE_QTV_GENERIC_BCAST
  /* Returns the audio track information */
  bool GetAudioTrackSpec(uint32 &bmTrackState,
                         Common::ChannelConfigType &eChannelConfig,
                         int &nSamplingFreq);
#endif /* FEATURE_QTV_GENERIC_BCAST */

  // Method to send the track state change notice
  void NotifyTrackStateChange(); 

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  /* Sets the dual mono output configuration */
  bool SetDualMonoOutput(Common::DualMonoOutputType dualMonoOutput);
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

private:

  AVSync* m_pAVSync;
  EventMSG m_eventMsg;
  CLIENT_NOTIFY_CB_FN m_playerCB;
  QCConditionType &m_responseSync;

  //The dispatcher thread who manages the msg-Q for msg sent by client.
  qtv_task_if_class*   m_pDispatcher;

  //id of the msg self dispatched message.
  qtv_task_if_class::dispatch_id_type m_msgType;

  //timout value used for delayed dispatch..
  long m_nWaitMsec;

  /* Used to check if the recvd delayed dispatch msg was sent 
  ** by this particular audio player inst*/
  bool CheckSelfDelayedDispatch( qtv_msg_struct* const msg_ptr );

  /*Used by the audioplayer to send a delayed dispatch msg to itself*/
  qtv_task_if_class::dispatch_id_type SelfDispatch(uint32 delay_ms);
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

  bool Prep(int playbackID,bool bRestart,Media * pMpeg4In) ;
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
  bool Start(bool &bError,bool bRestart) ;
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
  bool Pause(bool &bError) ;
  bool Suspend(bool &bError);
  bool Disable(bool &bError) ;
  bool Resume(bool &bError,long stop) ;
  void SetPlayTimes(long start,long stop);

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  bool PollDownload(int playbackID, bool &bError);
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */
  bool IsPlaying();
  
  bool IsPaused();
  bool IsPausedDisabled();

#ifdef AUDIO_SHOW_INFO
  void ShowInfo();
#endif /* AUDIO_SHOW_INFO */

  bool TrackHasEnded();

  ///////////////////////////
  // For CMX Interface
  ///////////////////////////

  //Functions CMX calls to notify audio thread of events
  //during playback.
  static void AudioNotice(Common::AudioNotifyType notify,
                          void* pClientData,
                          void* pData);

  void EnableTimeCheck( unsigned long timeValue );

  inline bool IsAudioAborted()
  {
    return AudioAborted;
  }

  // Error handling
  bool RestartAudio();

  // RestartAudioDSP
  bool RestartAudioDSP();

  // Handle Dynamic codec parameter change
  void HandleDynamicCodecParamChange(bool &bError);

  void InitiateRestartAudioOutput();

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 || 
          FEATURE_QTV_IN_CALL_VIDEO */


  //pointer to audio manager object
  AudioMgr *audioMgr;

  State state;
  State lastState;
  Media::MediaStatus audioDataState;
  bool bPlayEnding;
  bool bPlayDoneRendering;
  bool bPlayEndingMustBeSet;
  bool bLocalRestart;
  bool bPlayingNotifyPending;
  bool m_bHasVideo;
  Media::CodecType m_audioCodec;
  void* m_pClientData;

  //playback ID
  int currentPlaybackID;

  // Buffered audio duration in msec
  uint32 durationBuffered;

  // elapsed time check for Mpeg4Player
  bool checkTimeFlag;
  unsigned long checkTimeValue;

  //commands from the AV player to the audio thread.
  enum Command
  {
    NONE,
    PLAY,
    PAUSE,
    STOP,
    SUSPEND,
    DISABLE_AUDIO,
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 || 
          FEATURE_QTV_IN_CALL_VIDEO */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
    POLL_DOWNLOAD,
#endif
    RESTART_OUTPUT
  };
  Command command;
  bool bAbort;

  struct AudioTime
  {
    //time the thread spends waiting.
    long nWaitMsec;
    //end time.
    long tEndDelay;
  };
  AudioTime audioTime;

  //current state of device interface
  //audio player interface state (more than one
  //can be set at a time).
  struct AudioInterfaceState
  {
    bool bException;  //exception occurred in a callback

    bool bError;    //general error
    long tError;    //time error was reported.

    bool bDataDelay;  //device is starved for data.
    long tDataDelay;  //time delay was reported.

    bool bDone;     //device is done playing data.
    long tDone;     //time done was reported.
  };
  AudioInterfaceState interfaceState;
  #ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
  #endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  void InitInterfaceState();
  void InterfaceStateUpdate();

  //Media interface
  Media * pMpeg4;

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

  void CleanupPlay();
  void InitPlayData();
  void InitDefaults();

  bool SendCommand(Command newCommand, unsigned long);
  bool IsDataAvailable();
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif /* FEATURE_QTV_ADTS_PARSER  */
#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  bool WaitForDownloadFileHeader();
#endif

  uint32 GetAudioDurationAvailable();
  void ProcessStateTransition();
  void ProcessCommands();
  bool PauseAudioOutput();
  bool ResumeAudioOutput(bool bRestart);
  void StopAudioOutput();
  void AbortAudioOutput(bool bError = true);
  void ProcessInterfaceState();
  void ShowState(State input);
  void SetState(State newState);

  bool GetAudioPosition(long time, long &nCurTimestamp, long &nCurFrame);
  bool AdjustAudioPosition(int timestamp, long frame, long &delta);

  void NewPlayTimes(long play,long stop);
  void InitElapsedTime(long time);

  bool CanStart(bool bRestart);
  bool CanDisable();
  bool CanPause();
  bool CanResume();
  bool CanSuspend();

#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
  bool CanPollDownload();
#endif

  void TimingUpdate();

  bool RestartAudioOutput();

  bool AudioAborted;

  // Saved status to report if restart fails.
  Common::AudioStatusCode abortStatus;
  // Whether to abort if restart fails.
  bool abortAfterward;
  // Beginning of starving time.
  long starvingTime;

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 || 
          FEATURE_QTV_IN_CALL_VIDEO */

  /* After creating CMX interface, we should call ::start first before we could issue Resume.
   * This could happen in following scenarios:
   *
   * (1).
   * when PAUSED during initial buffering and FF/RW, we will end up calling RESUME before START
   * so audio won't start with "CMX resume failed" error
   *
   * (2).
   * If user releases DSP in normal pause and resume and if after PAUSE, attempts multiple repositions
   * with out transitioning to PLAYING, we will end up calling RESUME before START
   * so audio won't start with "CMX resume failed" error
   */
  bool m_bCMXInterfaceStartDone;

  #ifdef FEATURE_QTV_DRM_DCF
    //Member variable to indicate whether AUDIO task is registered
    //with NS/and have an IPC signal associated with it.
    bool m_bIsAudioTaskRegisteredForIPC;
  #endif

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
  #ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO
    // Playback Speed Command storage related members
    bool m_bPlaybackSpeedQueued; // Whether the Playback Speed Command Queued or not
    Common::PlaybackSpeedType m_ePlaybackSpeed; // Playback Speed enumeration
  #endif

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  Common::DualMonoOutputType m_dualMonoOutput;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */
  // Intra state member to remember the syncing sub state
  bool m_bSyncing; 

  // Intra state member to remember the playing-stable sub state
  bool m_bPlayingStable;
private:
  /* The number of outstanding audio dispatches must be tracked in order
  ** for proper behavior in ProcessStateTransition(). */
  uint32 m_numAudioDispatchesPending;
  rex_crit_sect_type  m_SelfDispatchCS;

public:
  /* This public member lets audioPlayerIf modify our dispatches-pending count,
  ** as it is able to send dispatches on our behalf. */
  uint32& NumAudioDispatchesPending( void ) { return m_numAudioDispatchesPending; }
private: 
  bool m_bPlayMode; /* Live or Buffered mode tracking */
  bool m_bPlayModeValid; /* PlayMode Valid or not */
  // Is next sample syncd to the PCR
  bool IsPTSSyncd(int nDataLead);
  // Is Intelligent Re-sync required again 
  bool IsResyncReqd();  

};

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
#endif

