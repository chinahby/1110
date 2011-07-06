#ifndef _AUDIOMGR_H_
#define _AUDIOMGR_H_
/* =======================================================================
                               audiomgr.h
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

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audiomgr.h#27 $
$DateTime: 2010/11/16 05:21:40 $
$Change: 1519115 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
**  ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "media.h"
#include "QtvPlayer.h"
#include "audiocmx.h"
#include "QtvMediaADTSParser.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define SUCCESS     0
#define INBUF_ARRAY_INDEX_SHIFT  (3)
#define INBUF_BIT_WIDTH         (1<<(INBUF_ARRAY_INDEX_SHIFT))
#define INBUF_BIT_MODULO_MASK   ((INBUF_BIT_WIDTH)-1)

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
#define MAX_AVSYNC_AUDIO_BUFFER_SIZE   770
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
/*
 * If enable byte based AV Sync for AAC, and the datalead for is 1600 MSec,
 * based on max sampling freqency we support is 48KHz and 1024 samples per frame/channel,
 * 1600 MSec = (48000 / 1024) * 1.6 = 75 frames, we add 5 more, so get 80 frames
 * note that if sampling freq >= 32KHz, each frame contains 1024 samples,
 * if sampling freq < 32KHz, each frame contains 2048 samples.
 */
#ifdef FEATURE_QTV_LOW_POWER_AUDIO
  #define MAX_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   770
#else
  #define MAX_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   80
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC
  #define EVRC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  80	
#else
  #define EVRC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  0
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP
  #define QCELP_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  80	
#else
  #define QCELP_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  0
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
  #define MP3_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  300	
#else
  #define MP3_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  0
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC
  #ifdef FEATURE_QTV_LOW_POWER_AUDIO
    #define AAC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   770
  #else
    #define AAC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   80
  #endif
#else
  #define AAC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER 0
#endif

#define WMA_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   10
#define WMA_PRO_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   10
#define WMA_PRO_DSP_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   40
#define WMA_PRO_PLUS_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER   10

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3
#error code not present
#else
#define AC3_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  0
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3 */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM
#error code not present
#else
#define PCM_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER  0
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB
#error code not present
#else
  #define AMR_WB_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER 0
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR
  #define AMR_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER 80
#else
  #define AMR_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER 0
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB
#error code not present
#else
  #define EVRC_NB_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER 0
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB
#error code not present
#else
  #define EVRC_WB_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER 0
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB */

#endif

/*
 * A negative number was used for this mask so that it works on both
 * 16-bit or 32-bit machines. The mask must be cast to unsigned int to 
 * work with TI compiler, ver 1.80.
 */
#define BYTE_ALIGN_MASK    ((unsigned int)(-8))
#define BYTE_ALIGN_ROUNDUP  7
//end, WZeng, 05/22/02,


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
  AudioMgr

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
class AudioMgr
{
public: 
  AudioMgr(Media *pMedia);
  virtual ~AudioMgr();


  /////////////////////////////////////
  // Control by Audio Player.
  /////////////////////////////////////

  void SetAVSyncObj(void* pAVSync);

  void SetAudioCodec (Media::CodecType audioCodec)
  {
    m_audioCodec = audioCodec;
  }

  bool OpenAudioManager(Common::AudioNotifyFuncT pFunc, void* pClientData);

  void CloseAudioManager();

  void InitPlayData(bool bRestart);
  void CleanupPlay();
  virtual bool Prep(
    Media * pMpeg4In,       //media pointer
    long nTimingUpdateMsec, //requested timing update interval.
    bool bRestart
    );

  // Audio Track Specification 
  struct AudioTrackSpecParams 
  {
    Common::AudioTrackState   eTrackState;    /* Audio Track State */
    Common::ChannelConfigType eChannelConfig; /* Channel Configuration */
    int                       nSamplingRate;  /* SamplingRate */
  };

  // Object instance of Audio Track specification type 
  AudioTrackSpecParams audioTrackSpec; 

  // Set Audio Track Spec Parameters function 
  void SetAudioTrackSpec(Common::AudioTrackState   eAudioTrackState,
                         Common::ChannelConfigType eChannelConfig,
                         int                       nSamplingRate)
  {
    audioTrackSpec.eTrackState    = eAudioTrackState; 
    audioTrackSpec.eChannelConfig = eChannelConfig; 
    audioTrackSpec.nSamplingRate  = nSamplingRate; 
    return;
  }

// Get Audio Track Spec Parameters function 
  void GetAudioTrackSpec(Common::AudioTrackState   &eAudioTrackState,
                         Common::ChannelConfigType &eChannelConfig,
                         int                       &nSamplingRate)
  {
    eAudioTrackState = audioTrackSpec.eTrackState; 
    eChannelConfig   = audioTrackSpec.eChannelConfig; 
    nSamplingRate    = audioTrackSpec.nSamplingRate ; 
    return;
  }

  // Allocate Audio Buffer
  bool AllocateAudioBuffer(int nSize);

  // Handle dynamic sample frequency change 
  bool HandleSampleFrequencyChange();
  
#ifdef FEATURE_QTV_DYNAMIC_AAC_ADTS
#error code not present
#endif
  
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
  void InitFrame();
  void InitStatistics();

  void ShowInfo();

  virtual bool Resume(bool bRestart);
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
  bool Start();
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
  bool Stop();
  void Abort();
  bool Pause();
  void SetPlayTimes(long start,long stop);
  void SetTimeStamp(int32 ts);
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  virtual void SetAudioSpec();
  virtual void SetEndDelay();
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

  virtual void DataIsAvailable();
  Media* GetMediaPtr()
  {
    return(Media*)pMpeg4;
  }

  virtual void PredictEndDelay(long &tEnd);
  void UpdateAudioEndStats(unsigned long tEnd);

  virtual bool PlaySomeAudio(
    long &nWaitMsec,  //output: time calling thread should sleep before calling again.
    bool &bError,   //output: error occurred.
    Media::MediaStatus &dataStateOut, //output: data state
    long &cmxDataLead);               // output: CMX data lead

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_GENERIC_BCAST_PCR) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 || 
          FEATURE_QTV_GENERIC_BCAST_PCR || 
          FEATURE_QTV_IN_CALL_VIDEO */

  unsigned char *GetAudioSpecificConfig(int *Size);

  unsigned long GetFrame();
  void GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                     Common::LastFrame &lastFrame);

  // for elapsed time info
  void InitElapsedTime(long time);
  virtual long GetElapsedTime();

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

  uint32 GetSampleRate();
  uint16 GetNumChannels();

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)  
  uint16 GetCodecVersion();   
  uint32 GetBitRate();
  uint32 GetEncoderOptions();
  
  //Returns Audio virtual packet size. (in bytes)
  uint16 GetVirtualPacketSize();
 
  //Returns true is ASFFILE has started sending dummy bytes.
  bool   isStartOfDummyBytes();
   
  uint16 GetAdvancedEncodeOptions();
  uint32 GetAdvancedEncodeOptions2();
  uint16 GetChannelMask();
  uint16 GetArmDataReqThr();
  uint8 GetValidBitsperSample();   
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  //Resets datlead calculation when CMX reports DATA ACCESS DELAY and
  //we are throttling.
  void   ResetThrottling();
#endif

  ////////////////////////////
  // Visible for AudioCMX
  ////////////////////////////

  //audio data buffer.
  Common::AudioDataBuffer audioBuffer;

  long elapsedTimeForGenericAudio;
  Common::AudioNotifyFuncT notifyFunc;

  bool FlushDataRequests();
  /* Made public for image swap fix*/
  void EndElapsedTime(long time);

  virtual bool FillAudioBuffer(const uint32 max_len, const uint32 offset) ;
  
  // Check to see if data being restored
  bool IsDataBeingRestored();

  /* Private Members related to Fast Playback Mode */
  bool bFastPlaybackMode;       /* FastPlaybackMode */
  int  *aFastPlaybackFilterConfig;       /* Fast Playback Filter Config */
  int  nFastPlaybackWindow;    /* Modulus */
  float fFastPlaybackWindowInverse; /* Inverse to avoid division */
  int  nFastPlaybackDropCount;  /* Drop Count */

  // Fast Audio Playback methods  - Begin
  // Constructor function to create fast audio playback 
  void CreateFastAudioPlaybackObject();
  // Fast Audio Playback Destructor
  void DestroyFastAudioPlaybackObject();
  // ProcessAudioSample 
  bool ProcessAudioSample(int nBytes, long nTimestamp, bool bRestoring, unsigned long nSampleCount);
  // Sets up the parameters ready for fast audio playback mode
  bool StartFastAudioPlayback(int nModulus, int nDropCount);
  // Check if the audio fast playback enabled or not
  bool IsFastPlaybackModeEnabled()
  {
    return bFastPlaybackMode; 
  }
  // Clears up the fast audio playback mode parameter once normal play resumes
  void StopFastAudioPlayback();
  // Sets up Fast Audio Playback filters to decide which ones to drop
  void SetupFastAudioPlaybackFilters();
  // Checks based on the frame number, current frame need to be dropped in fast playback mode
  bool IsCurrentFrameDropped();
  // Update Audio Buffer details after fetching media sample
  void UpdateAudioBuffer(int nBytes, long nTimestamp, unsigned long nSampleCount, bool bPlay);
  // Fast Audio Playback Adjust Timestamp -- It takes timestamp calculated and the frame id. 
  long FastAudioPlaybackAdjustTS(long nTS, int nFrame);
  #ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  // Insert entry into Restore data buffer
  void RestoreDataBufferInsertSample(int nBytes, long nTimestamp, unsigned long nSampleCount);
  // Insert entry into AV Sync buffer
  bool AVSyncAudioBufferInsertEntry(int nBytes, long nTimestamp);
  #endif
  // Fast Audio Playback Methods - End
  
  // Fast Audio Playback methods - End
  virtual Media::MediaStatus GetMediaStatus()
  {
    return mediaInfo.dataState;
  }
  void SetMediaStatus(Media::MediaStatus state)
  {
    mediaInfo.dataState=state;
  }
  void AddMediaProcessFrame( int inc)
  {
    mediaInfo.nFramesProcessed+=inc;
  }
  unsigned long GetMediaProcessFrame( void)
  {
    return mediaInfo.nFramesProcessed;
  }

  void ResyncAudioTime(void)
  {
    xferInfo.startOfInterval.bValid = false;
  }

  int getCMXMaxAheadInMsec(void);

  struct AudioInfo
  {
    long nSamplesPerFrame;
    long nSamplingRate;
    long nMediaTimescale;
    long nTotalBytes;
    long nMaxBytesPerFrame;
    long nDurationMsec;
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    long nBitRate;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
    long nChannels;
    long nVersion;          /* AUDIO/WMA codec version */
    long nBitsPerSample;
    
#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)    
    long nAsfPacketSize;    /* fixed ASF audio packet size */
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */
  };
  AudioInfo audioInfo;


  //for some reason arm compiler wants this public
  struct TimePoint
  {
    long timestamp;
    long time;
    bool bValid;
    //Frame number integer part
    long nFrame;
    //Frame number fraction part (precision determined by FRAME_FRACTION_BASE)
    int16 nFrameFraction;               // value range: -32768 ~ 32767
  };

  //Data transfer info.
  struct XferInfo
  {
    int nBuffersSentInInterval;
    long tStartInterval;
    long nLastTimestampSent;
    long nLastFrameSent;
    bool bTimingUpdatedInInterval;
    TimePoint startOfInterval, endOfInterval;
    int nBytesSent;
  };
  XferInfo xferInfo;

  //for some reason arm compiler wants this public
  

  #define FRAME_FRACTION_BASE (10000)    //ten-thousandth
  QCCritSectType audioMgrCS;
  QCCritSectType audioMgrCSforTS;
  //audio statistics
  struct AudioStats
  {
    QCCritSectType CS;

    long nNullFrames;
    long nMaxNullChunk;
    long nCurrentNullChunk;
    long nDrift;
    long nMaxAhead, nMaxBehind;
    long nFramesDiscarded;

    //max buffer processing time
    long nMaxProcTime;

    //For AV sync stats
    Common::LastFrame lastFrame;
  };
  AudioStats audioStats;

  //WZeng, 05/22/02, for decoding AAC audioSpecificConfig 
  typedef enum
  {
    /*
     * Program Configuration
     */
    Main_Profile  = 0, 
    LC_Profile    = 1, 

    Fs_48   = 3, 
    Fs_44   = 4, 
    Fs_32   = 5,

    LEN_PROFILE   = 2, 
    LEN_SAMP_IDX  = 4, 
    LEN_NUM_ELE   = 4, 
    LEN_NUM_LFE   = 2, 
    LEN_NUM_DAT   = 3, 
    LEN_NUM_CCE   = 4, 
    LEN_MIX_PRES  = 1, 
    LEN_MMIX_IDX  = 2, 
    LEN_PSUR_ENAB = 1, 
    LEN_ELE_IS_CPE  = 1,
    LEN_IND_SW_CCE  = 1,  
    LEN_COMMENT_BYTES = 8

  } eProgConfigConst;

  typedef enum
  {
    LEN_SE_ID   = 3,
    LEN_TAG   = 4,
    LEN_COM_WIN   = 1,
    LEN_ICS_RESERV  = 1, 
    LEN_WIN_SEQ   = 2,
    LEN_WIN_SH    = 1,
    LEN_MAX_SFBL  = 6, 
    LEN_MAX_SFBS  = 4, 
    LEN_CB    = 4,
    LEN_SCL_PCM   = 8,
    LEN_PRED_PRES = 1,
    LEN_PRED_RST  = 1,
    LEN_PRED_RSTGRP = 5,
    LEN_PRED_ENAB = 1,
    LEN_MASK_PRES = 2,
    LEN_MASK    = 1,
    LEN_PULSE_PRES  = 1,
    LEN_TNS_PRES  = 1,
    LEN_GAIN_PRES = 1,

    LEN_PULSE_NPULSE  = 2, 
    LEN_PULSE_ST_SFB  = 6, 
    LEN_PULSE_POFF  = 5, 
    LEN_PULSE_PAMP  = 4, 
    NUM_PULSE_LINES = 4, 
    PULSE_OFFSET_AMP  = 4, 

    LEN_IND_CCE_FLG = 1,  
    LEN_NCC   = 3,
    LEN_IS_CPE    = 1, 
    LEN_CC_LR   = 1,
    LEN_CC_DOM    = 1,
    LEN_CC_SGN    = 1,
    LEN_CCH_GES   = 2,
    LEN_CCH_CGP   = 1,

    LEN_D_ALIGN   = 1,
    LEN_D_CNT   = 8,
    LEN_D_ESC   = 8,
    LEN_F_CNT   = 4,
    LEN_F_ESC   = 8,
    LEN_BYTE    = 8,
    LEN_PAD_DATA  = 8,

    LEN_PC_COMM   = 8

  } eRawBitstreamConst;

  typedef enum
  {
    LEN_OBJ_TYPE = 5,
    LEN_SAMP_RATE_IDX = 4,
    LEN_SAMP_RATE   = 24,
    LEN_CHAN_CONFIG = 4,
    LEN_FRAME_LEN_FLAG = 1,
    LEN_DEPEND_ON_CORE = 1,
    LEN_CORE_DELAY = 14,
    LEN_EXT_FLAG = 1,
    LEN_EP_CONFIG = 2,
    LEN_LAYER_NUM = 3,
    LEN_SUB_FRAME = 5,
    LEN_LAYER_LEN = 11,
    LEN_SECT_RES_FLAG = 1,
    LEN_SCF_RES_FLAG = 1,
    LEN_SPEC_RES_FLAG = 1,
    LEN_EXT_FLAG3 = 1
  } eMP4FF_const;

  // Codec param change state variable
  bool m_bCodecParamChanged;

  // Method to get the state variable 
  bool IsCodecParamChanged()
  {
    return m_bCodecParamChanged;
  }

  // Set Codec Parameter changed boolean
  void SetCodecParamChanged(bool bVal)
  {
    m_bCodecParamChanged = bVal;
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
    return;
  }
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */
  

  int FetchAudioSample(Media *pMedia,Media::MediaStatus &eStatus,
                                        int *nSamples,int nMaxSize,int nChannel=0);
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
  // ReallocateAudioMediaBuffer
  bool ReallocateAudioMediaBuffer();
  unsigned long FetchAudioTimestamp(Media *pMedia);

  // Fetch Next Audio Timestamp
  unsigned long FetchNextAudioTimestamp(Media *pMedia);

  int GetAndDecodeAudioSpecificConfig(cmx_af_aac_spec_type *pCmx_af_aac_spec);
  //audio device interface.
  AudioCMX * pAudioCMX;
  //end, WZeng, 05/22/02

  void SetClientData(const void *client_data);


  long GetAmtBufferedMsec(unsigned long time);

#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  /* this structure is used for cmx a/v sync callback support
     the callback returns the number of samples played and the
	 timestamp is computed based on that */     
  struct CMXAVSyncUpdate
  {
    long curTime; /* to capture the current time */
    long nFrames; /* to store the number of frames played */
    long curTimeStamp; /* to store the current time stamp computed */
    long prevResumedTimeStamp;/* to store the timestamp to which we repositioned */
    long prevResumedFrame;/* to store the start frame before we paused */
    unsigned long elapsedTime; /* to stroe audio elapsed time */
    bool cmxPause; /* to indicate if pause was pressed */         
    /*
    * This is set to TRUE when 
    * CMX is flushed(reposition) or 
    * when audioCMX interface is being created(initial play or resume after DSP release).
    *
    * When set to TRUE, bound checking will be done when A/V Sync feedback arrives.
    * This checking differs for given audio codec. Typically,
    * after a FLUSH, first valid feedback should be 0/1.
    */
    bool bCmxFlushed;
  };
  CMXAVSyncUpdate cmx_avsync_update;
  bool bFirstEntryPlayedOut;
  unsigned long firstEntryOffset;
  /* this function is used to compute the offset each time cmx reports the number 
  of samples played */
  void SetCurTimeStamp(uint64 num_of_samples); 
#endif

#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  static bool IS_BYTE_BASED_AVSYNC_SUPPORTED(Media::CodecType audioCodec)
	{	
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
		if(audioCodec == Media::AAC_CODEC)
#endif /* FEATURE_QTV_BSAC */
		{
			return true;
		}
#endif
#ifdef FEATURE_QTV_WINDOWS_MEDIA
		if(audioCodec == Media::WMA_CODEC)
		{
			return true;
		}
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC		
    if(audioCodec == Media::EVRC_CODEC)
    {
      return true;
    }
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP		
	if(audioCodec == Media::QCELP_CODEC)
        {
          return true;
        }
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR
    if(audioCodec == Media::GSM_AMR_CODEC)
    {
      return true;
    }
#endif//#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    if(audioCodec == Media::Media::WMA_PRO_CODEC)
    {
      return true;
    }
if(audioCodec == Media::Media::WMA_PRO_PLUS_CODEC)
    {
      return true;
    }
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
    if(audioCodec == Media::MP3_CODEC)
    {
      return true;
    }
#endif//#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3
   if(audioCodec == Media::AC3_CODEC)
    {
      return true;
    }
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3 */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM */
   
		return false;
};
#endif

static bool IS_GENERIC_AUDIO_FORMAT(Media::CodecType audioCodec)
  {	
    if((audioCodec == Media::NONMP4_MP3_CODEC) || 
       (audioCodec == Media::QCP_CODEC) || 
       (audioCodec == Media::NONMP4_AMR_CODEC) || 
       (audioCodec == Media::NONMP4_AAC_CODEC) ||
       (audioCodec == Media::MIDI_CODEC))
    {
      return true;
    }
    return false;     
  };

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  
  //Variables/functions to manage AV SYNC mapping table
  void InitAVSyncAudioBuffer(bool bFlushRestoreBuffer);
  Common::AVSyncAudioDataBuffer AVSyncAudioBuffer[MAX_AVSYNC_AUDIO_BUFFER_SIZE];
  uint32 nAVSyncAudioWriteByteOffSet;
  uint16 nAVSyncAudioBufferWriteIndex;
  uint16 nAVSyncAudioBufferSize;
  #ifndef T_MSM7500
  void UpdateAVSyncFromCMXBytes(long num_of_bytes);
  #else /* T_MSM7500 */
#error code not present
  #endif /* T_MSM7500 */
  int32 GetTimeStampFromCMXBytes(long num_of_bytes, int &nFrame);  
  int32 m_nCurrentTSFromCMXBytes;
  bool  m_bCurrentTSFromCMXBytesUpdatePending;  

  //This flag will keep track of where the current data came from.It can
  //come from the file, the restore buffer or the pending buffer.
  enum{UNKNOWN, PENDING, RESTORE, MEDIA} m_DataSrcFlag;


  //Variables/functions to manage restore data buffer
  //This is used when resuming after voice call or after releasing the DSP.
  static int const NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[Media::MAX_CODEC+1];  

  typedef struct rDataBuffer
  {
    //size in bytes for the actual data placed at this entry
    uint32 nSize;

    //number of frames 
    uint32 nFrames;

    //Presentation time
    uint32 nPresTime;

    //pointer to audio data
    unsigned char* pDataPtr;

  }restoreDataBuffer;

  typedef struct rBufferInfo
  {
    /*
    * Each entry in restore data buffer for given audiocodec and clip
    * has same size/length.This is used to prevent overwriting the data into
    * data buffer.Also used to detect if we are playing different clip without QTV
    * terminate being called.
    */
    int32 nSizeOfEachEntry;

    /*
    * Indicates number of entries in restore data buffer
    * at any given time. These entries basically store the data 
    * that is being sent to CMX but not yet acknowledged via DSP feedbacks.
    */
    int16 nEntriesInRestoreBuffer;
    
    //Write index used when storing the data into restore data buffer
    int16 nWriteIndex;
    
    //Read index when reading the data from restore data buffer.
    int16 nReadIndex;
    
    /*
    * Indicates number of entries that are not acknowledged with DSP feedback.
    * These entries will be used when restoring the data.
    */
    int16 nEntriesToRestore;
    
    //Boolean flag indicating whether restore data buffer is valid.
    bool bRestoreBufferValid;
    
    //variable used when restoring the data
    int  nTmpReadIndex;
    
    restoreDataBuffer UnAckDataBuffer[MAX_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER];    
  }restoreInfo;  
  restoreInfo RestoreDataBufferInfo;

  void  FlushRestoreDataBufferInfo();  
  int   GetRestoreDataBufferEntrySize(Media::CodecType audioCodec);
  void  DestroyRestoreDataBufferInfo();
  void  UpdateRestoreDataBuffer(int tsToMatched);
#endif

Media::CodecType GetAudioCodecType();

  /* For Logging first frame after Audio Channel switch */
  uint32 ChannelSwitchedTS;
  uint32 AudioChannelSwitchedTS;
  bool   CheckForChannelSwitchFrame;
  
  void LogAudioChannelSwitch(uint32 TimeDiff); 
  void SetChannelSwitchTS(uint32 ChannelSwitchedTS);
  void InitAVTiming();
  void SetPlaybackOffset(long time,long timestamp);
  void StartElapsedTime();

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  /* Sets the dual mono output configuration */
  bool SetDualMonoOutput(Common::DualMonoOutputType dualMonoOutput);
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

#ifdef FEATURE_QTV_LOW_POWER_AUDIO
  bool isAudioOnlyLocalPlayback();
#endif

protected:
// Audio data lead  
  long m_MaxCmxAudioDataLead;
  long m_MinCmxAudioDataLead;

  AVSync* m_pAVSync;
  void* m_pClientData;
  Media::CodecType m_audioCodec;
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif

  // Lazy Start boolean for AAC-ADTS codec
  bool m_bADTSLazyStart; 
  // Timing update (in msec) given in original Prep
  long m_nStoredTimingUpdateMsec;

 
#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
   // This varible holds the last write index for the restore data buffer
  int  nLastWriteIndex;
  /*
  * We need to add only one entry in AV SYNC mapping table irrespective of whether
  * we will be sending audio frame/packet in multiple chunks or in one shot.
  * Initialized to true by default in constructor and also when we fetch the audio sample.
  * Variable is reset when entry gets added into AV SYNC mapping table.
  */
  bool m_bFirstAudioFragment;
#endif // FEATURE_QTV_CMX_AV_SYNC_BYTES


  const void *playerData;


#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

  

  //Fixed parameters for this clip.
  struct ClipInfo
  {
    int nFramesPerBuffer;
    int nTimingUpdateIntervalBuffers;
    float fFramesPerMsec;
  };
  ClipInfo clipInfo;

  //Data related to sample processing.
  struct MediaInfo
  {
    //current frame number
    unsigned long nFramesProcessed ;

    //current state of data stream.
    Media::MediaStatus dataState;
  };
  MediaInfo mediaInfo;

   //Elapsed time stuff.
  struct ElapsedTime
  {
    long nLastTimestamp;
    long nAmtBuffered;
    bool bPlaying;
  };
  ElapsedTime elapsed;

  struct AudioTime
  {
    long tStop,tStart,tPlay;
  };
  AudioTime audioTime;

  //stop time
  struct StopTime
  {
    unsigned long nStop;
    bool bStop;
  };
  StopTime stopTime;
  //start time.
  long nStartTime;
  
 /*
  * This variables are used to collect the audio statistics. Although the name says AudioVideoStatisticsT,
  * we collect only audio statistics in audioMgr.cpp. (Don't want to separet out/break 
  * AudioVideoStatisticsT into audioStat and videoStat etv.) 
  *
  * Reason for adding these members:
  * When user lets the clip play till the end,(with out pressing "STOP")
  * we clean up CMX interface before calling audioMgr::GetStatistics. SO, in this case, statistics
  * values are not populated correctly.Fix remembers these values in following members 
  * and just copy over in audioMgr::GetStatistics.
  * 
  * 
  */
  QtvPlayer::AudioVideoStatisticsT m_nAudioVideoStat;
  Common::LastFrame                m_nAudioLastFrameStat;


  

  bool GetAudioPosition(long time, long &nCurTimestamp, float &nCurFrame);
  void ShowInterval(TimePoint interval);
  void ShowLastFrame(Common::LastFrame lastFrame);

  void SetAVTiming(long tPlayBegin, long timestamp, long frame);
  void UpdateAVTiming(long timestamp, long frame);

  Media * pMpeg4;
  

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_GENERIC_BCAST_PCR) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 || 
          FEATURE_QTV_GENERIC_BCAST_PCR || 
          FEATURE_QTV_IN_CALL_VIDEO */

  virtual void DestroyAudioInterface();
  virtual bool CreateAudioInterface(Media::CodecType codec);
  void InitDefaults();

  
  

  //WZeng, 05/22/02, for decoding AAC audioSpecificConfig 

  class TAudioSpecificConfig
  {
  public:
    // buffer containing the codec specific info for the codec
    unsigned char* iConfig;
    // number of bytes in the buffer
    int iNumBytes;
  };

public:
  /*----------------------------------------------------------------------------*/
  /*
   * Name: BITS
   * Description: Holds information for processing the input data buffer
   *    as a "stream". The data is in packed format.
   * Fields:
   *    pBuffer - pointer to the beginning of the buffer. If the data type of
   *        this changes, make sure to update the constants in ibstream.h
   *    usedBits - number of bits read thus far from the buffer. Bit 0 is 
   *        the LSB of pBuffer[0].
   *    availableBits - number of bits available in the buffer.
   *    byteAlignOffset - used with ADTS in case sync word is not aligned
                          on a boundary.
   */
  typedef struct
  {
    unsigned char    *pBuffer;
    unsigned int      usedBits;      /* Keep this unsigned so can go to 65536 */
    unsigned int      availableBits; /* Ditto */
    int      byteAlignOffset; /* Used in ADTS.  See find_adts_syncword() */
  } BITS;

  /*
   * Note: most of the names of the variables put into this structure were kept
   * the same because the name is also used in called functions.
   *
   * bno - block number
   *
   */

  typedef struct
  {
    int num_ele;
    int ele_is_cpe[(1<<LEN_TAG)];
    int ele_tag[(1<<LEN_TAG)];
  } EleList;

  typedef struct
  {
    int present;
    int ele_tag;
    int pseudo_enab;
  } MIXdown;

  typedef struct
  {
    int     profile;
    int     sampling_rate_idx;
    EleList front;
    EleList side;
    EleList back;
    EleList lfe;
    EleList data;
    EleList coupling;
    MIXdown mono_mix;
    MIXdown stereo_mix;
    MIXdown matrix_mix;

    //Char    comments[(1<<LEN_PC_COMM)+1]; /* TO BE DELETED */

    long   buffer_fullness; /* put this transport level info here */
    int     headerless_frames;  /* For ADTS use only */
    int     frame_length;       /* For use by ADTS only */
    int     CRC_absent;         /* For use by ADTS only */
    unsigned int    CRC_check;          /* For use by ADTS only */

  } ProgConfig;

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  /* See ISO/IEC 14496-3:2001(E) for details on the parameters below */
  typedef struct
  {
    byte  epConfig;
    byte  aacSectionDataResilienceFlag;
    byte  aacScalefactorDataResilienceFlag;
    byte  aacSpectralDataResilienceFlag;
  } AAC_ER_Flags;
  AAC_ER_Flags AACERFlags;
#endif

  typedef struct
  {
    //WZeng    UInt32         bno;
    //WZeng    Int            status;  /* save the status */

    int            current_program;
    //WZeng    Int            frameLength;
    //WZeng     Int            quantSpec[LN2]; /* LN2 = 1024 used by huffspec_fxp */
    //WZeng    Int            tmp_spec[LN2];

    BITS           inputStream;

    ProgConfig     prog_config;
    ProgConfig     scratch_prog_config;

    //WZeng     Int            SFBWidth128[(1<<LEN_MAX_SFBS)];
    //WZeng     Int            huffBookUsed[248];

    /*
     * One of the two arrays should be deleted in the final version.
     */
    //WZeng, no need
    /**
    FrameInfo      longFrameInfo;
    FrameInfo      shortFrameInfo;
    FrameInfo     *winmap[NUM_WIN_SEQ];
    **/
    /*
     * Might not need to persist the adif_header, but for now will keep it.
     */
    //WZeng, no need
    /**
    ADIF_Header    adif_header;
    **/
    /*
     * Pns variables.
     */
    //WZeng     Int32         pns_cur_noise_state;

    /*
     *
     */
    //WZeng     MC_Info        mc_info;
    //WZeng     MC_Info        savedMCInfo;

    //WZeng     PulseInfo      pulseInfo;

    //WZeng     tDec_Int_Chan  perChan[Chans];

    //WZeng     UInt32         syncword;
    //WZeng     Int            invoke;

    /*
     * Scratch, not sure where to put it.
     */
    //WZeng     Int32  memoryForFFT[LONG_WINDOW];      /* 1024 */

    //WZeng     Int32  predictedSamples[LONG_BLOCK1];  /* 2048 */
    //WZeng     Int32  scratchTNSMemory[TNS_MAX_ORDER];  // FIXME !!

  } tDec_Int_File;


  typedef enum eMP4AudioObjectType
  {
    MP4AUDIO_NULL            =  0, /*                                       */
    MP4AUDIO_AAC_MAIN        =  1, /*                                       */
    MP4AUDIO_AAC_LC          =  2, /* LC = Low Complexity                   */
    MP4AUDIO_AAC_SSR         =  3, /* SSR = Scalable Sampling Rate          */
    MP4AUDIO_LTP             =  4, /* LTP = Long Term Prediction            */
    /* 5 is reserved                         */
    MP4AUDIO_AAC_SCALABLE    =  6, /* scales both bitrate and sampling rate */
    MP4AUDIO_TWINVQ          =  7, /* low bit rate                          */
    MP4AUDIO_CELP            =  8, 
    MP4AUDIO_HVXC            =  9,
    /* 10 is reserved                        */
    /* 11 is reserved                        */
    MP4AUDIO_TTSI            = 12,
    /* 13-16 are synthesis and MIDI types    */
    MP4AUDIO_ER_AAC_LC       = 17, /*                                       */
    /* 18 is reserved                        */
    MP4AUDIO_ER_AAC_LTP      = 19, /*                                       */
    MP4AUDIO_ER_AAC_SCALABLE = 20, /*                                       */
    MP4AUDIO_ER_TWINVQ       = 21, /*                                       */
    MP4AUDIO_ER_BSAC         = 22, /*                                       */
    MP4AUDIO_ER_AAC_LD       = 23, /*                                       */
    MP4AUDIO_ER_CELP         = 24, /*                                       */
    MP4AUDIO_ER_HVXC         = 25, /*                                       */
    MP4AUDIO_ER_HILN         = 26, /*                                       */
    MP4AUDIO_PARAMETRIC      = 27,  /*                                       */
    MP4AUDIO_PARAMETRIC_STEREO = 29  /*            */
                               /* 28-21 are reserved                    */

  } tMP4AudioObjectType;

  int DecodeAACHeader(TAudioSpecificConfig* aInfo,
                                cmx_af_aac_spec_type *pCmx_af_aac_spec
                               );
  int get_audio_specific_config( tDec_Int_File  * const pVars,
                                           cmx_af_aac_spec_type * pCmx_af_aac_spec
                                         );
  int get_GA_specific_config(
                                      tDec_Int_File * const pVars,
                                      BITS    *pInputStream,
                                      unsigned int     channel_config,
                                      const int audioObjectType
                                      );
  unsigned long getbits(
                                 const unsigned int  neededBits,
                                 BITS       *pInputStream);
  int get_prog_config(
                               tDec_Int_File *pVars,
                               ProgConfig    *pScratchPCE);
  void get_ele_list(
                             EleList     *pElementList,
                             BITS        *pInputStream,
                             const bool   enableCPE);

  void byte_align(
                           BITS  *pInputStream);

  //end WZeng, 05/22/02
};

#endif //_AudioMgr_H_

