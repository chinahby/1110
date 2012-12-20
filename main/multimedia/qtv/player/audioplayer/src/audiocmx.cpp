/* =======================================================================
                               audiocmx.cpp
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
Copyright 2003 QUALCOMM Incorpo rated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audiocmx.cpp#48 $
$DateTime: 2011/02/23 02:44:00 $
$Change: 1628608 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#if defined(FEATURE_QTV_GENERIC_BCAST_PCR) && !defined(FEATURE_DTV_TSIF)
#error code not present
#endif
#include "qcplayer.h"
#include "audiocmx.h"
#include "audio.h"
#include "media.h"
#include "assert.h"
#include "mpeg4player.h"
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
// #define SHOW_AUDIO_INFO
// #define SHOW_CMX_DEBUG_INFO
// #define SHOW_CMX_DEBUG_INFO_ALL
// #define SHOW_TIMING
// #define SHOW_STATS
// #define LOG_AUDIO_BUFFER_SENT_TO_CMX

#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  #if(defined FEATURE_QTV_LOW_POWER_AUDIO)
    #define AUDIO_CMX_AV_SYNC_INTERVAL_LOW_POWER 500
  #endif
#define AUDIO_CMX_AV_SYNC_INTERVAL 75 //AV SYNC interval in MSEC.
#endif

#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
   /* We need to fine tune this value by trying with various clips like
      very low bitrate and very high bitrate clips. */
   #define MAX_AHEAD_FOR_WM_PRO 2400  //Msec

   #ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
    #define AUDIO_CMX_AV_SYNC_INTERVAL_AUDIO_ONLY_PLAYBACK 500 //in milliseconds
  #endif
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef LOG_AUDIO_BUFFER_SENT_TO_CMX
#include "fs_public.h"
#include "fs_fcntl.h"
#include "qtv_diag_main.h"
#include "oscl_file_io.h"
#endif

/* Tried with various size combinations like 1024, 1500, etc
   but 512 seems to have more real time performance
*/
#define LOG_AUDIO_BUFER_MAX_DATA_SIZE  512
LOG_RECORD_DEFINE(LOG_QTV_CMX_AUDIO_DATA_C)
  uint16 ChunkNumber;          /* Chunk number in the packet */
  uint16 DataSize;             /* Size of the audio packet */
  uint8  Data[LOG_AUDIO_BUFER_MAX_DATA_SIZE];    /* Dump of AUDIO data from QTV to CMX */
LOG_RECORD_END
typedef LOG_QTV_CMX_AUDIO_DATA_C_type log_qtv_to_cmx_audio_buffer_type;


//max size of audio data buffer we send to cmx.
//in current ltk a larger buffer works better, avoids audio dropout.
//(800 msec is max it can hold for FR qcelp)
static const long CMX_BUFFER_SIZE_MSEC = 800;

//this tells how far ahead of playback position we will send data.
//beyond this much the audio manager will wait for audio playback
//to catch up.
#ifdef FEATURE_QTV_LOW_POWER_AUDIO
  static const int CMX_MAX_AHEAD_MSEC_LOW_POWER   = 14000;
#endif
static const int CMX_MAX_AHEAD_MSEC = 500;

//If the audio is starved beyond the audio starvation threshold,
//audio will abort.
static const int CMX_AUDIO_STARVATION_THRESHOLD_MSEC = 10000;

static const long CMX_PLAY_TIMEOUT=1000;
static const long CMX_PLAY_POLLTIME=20;

const uint32 AudioCMX::SAFE_TO_ACCEPT_DATA_REQS   = 0xABBADABA;
const uint32 AudioCMX::UNSAFE_TO_ACCEPT_DATA_REQS = 0x00000000;

//Both the following sigs are the same as the one used by zrex to do sync
// events
#ifdef T_MSM7500
#error code not present
#else
#define CMX_SYNC_EVENT_SIG        0x02000000  // CMX sync event signal
#define CMX_SYNC_EVENT_TIMER_SIG  0x04000000  // CMX sync event timer signal
#endif //T_MSM7500

#ifdef LOG_AUDIO_BUFFER_SENT_TO_CMX
#define LOG_FILE_MAX_SIZE 102400 //100 KB
static OSCL_FILE* fp = NULL;
static long logfileSize = 0;
#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
AudioMgr * pAudioMgr=NULL;
AudioCMX * pLocalAudioCMX = NULL;
static bool bPlaying=false;
static rex_crit_sect_type  cmxDataCS;
static rex_crit_sect_type  ImageChangeCS;

/* -----------------------------------------------------------------------
** Forward Declarations Test
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  WaitTilPlaying

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
static bool WaitTilPlaying()
{
    if (bPlaying)
    {
        return true;
    }
    for (long waittime=0; waittime<CMX_PLAY_TIMEOUT; waittime+=CMX_PLAY_POLLTIME)
    {
        QCUtils::sleep(CMX_PLAY_POLLTIME);
        if (bPlaying) return true;
    }
    return false;
}

/* ======================================================================
FUNCTION
  WaitTilNotPlaying

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
static bool WaitTilNotPlaying()
{
    if (!bPlaying)
    {
        return true;
    }
    for (long waittime=0; waittime<CMX_PLAY_TIMEOUT; waittime+=CMX_PLAY_POLLTIME)
    {
        QCUtils::sleep(CMX_PLAY_POLLTIME);
        if (!bPlaying) return true;
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Timeout in WaitTilNotPlaying");
    return false;
}

/*===========================================================================
Function: qtv_to_cmx_alloc_audio_log_buffer

Description: Obtain a log buffer.Return NULL if logging is disabled.

Return Value:
   NULL or a pointer to a log buffer.

Side Effects:

============================================================================*/
static log_qtv_to_cmx_audio_buffer_type *qtv_to_cmx_alloc_audio_log_buffer( void )
{
  log_qtv_to_cmx_audio_buffer_type *pBuf = NULL;
  int no_of_attempts = LOG_QTV_CMX_AUDIO_DATA_C_RETRY;

  /* Because log_alloc() will return NULL when it is out of memory we
   * need to keep looping until we get memory.  We'll monitor
   * log_status() just in case the log gets disabled.
   */
  while (!pBuf && log_status( LOG_QTV_CMX_AUDIO_DATA_C ) && no_of_attempts)
  {
    pBuf = (log_qtv_to_cmx_audio_buffer_type*) log_alloc( LOG_QTV_CMX_AUDIO_DATA_C, sizeof(*pBuf) );
    if (!pBuf)
    {
      rex_sleep(LOG_QTV_SLEEP_TIMEOUT);
    }
    no_of_attempts--;

  }
  if(!pBuf)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL,
                 QTVDIAG_PRIO_ERROR,
                 "qtv_to_cmx_alloc_audio_log_buffer: Log attempts failed");
  }

  return pBuf;
}


/* <EJECT> */
/*===========================================================================
Function: qtv_to_cmx_log_audio_buffer

Description: Log a complete audio buffer by breaking it up into chuncks acceptable
             to the logging subsystem.  Won't return until the entire buffer has
             been logged or the logging is turned off by the DM.

Return Value:
   None.

Side Effects:
    Executed in the graph task context {mp4_process_decode_done()}.

============================================================================*/
LOCAL void qtv_to_cmx_log_audio_buffer( unsigned char *pAudioBuf, int bufSize )
{
  unsigned short LogChunkCount = 0;
  unsigned short LogSize = (uint16) MIN( bufSize, LOG_AUDIO_BUFER_MAX_DATA_SIZE );

  log_qtv_to_cmx_audio_buffer_type *pLogAudioBuffer = qtv_to_cmx_alloc_audio_log_buffer();

  if (pLogAudioBuffer)
  {
    /*
    **  Verify that we got a destination buffer, and that a clip reset hasn't
    **  de-allocated the source YUV input buffer.
    */
    while (pLogAudioBuffer && pAudioBuf)
    {
      ASSERT(pLogAudioBuffer);
      ASSERT(pAudioBuf);

      pLogAudioBuffer->ChunkNumber = LogChunkCount++;
      pLogAudioBuffer->DataSize = LogSize;
      memcpy( (void *) pLogAudioBuffer->Data, pAudioBuf, LogSize );
      log_commit( pLogAudioBuffer );

      bufSize -= LogSize;
      pAudioBuf += LogSize;

      if (bufSize > 0)
      {
        LogSize = (uint16) MIN( bufSize, LOG_AUDIO_BUFER_MAX_DATA_SIZE );

        pLogAudioBuffer = qtv_to_cmx_alloc_audio_log_buffer();
      }
      else
      {
        pLogAudioBuffer = NULL;
      }
    } /* while there are more frames to log */
  }
} /* end qtv_to_cmx_log_audio_buffer */


#ifndef PLATFORM_LTK
/* ======================================================================
FUNCTION
  AudioCMX::ImageChangeCallback

DESCRIPTION
  Callback that is used by CMX to inform QTV that an image change has
  occured and QTV has to take appropriate action.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  void.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::ImageChangeCallback(uint32 status)
{
  (void)status; //Compiler warning fix
  // We dont use status as of now

  if(pAudioMgr)
  {
    QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioCMX::ImageChangeCallback");
    if(!pLocalAudioCMX->no_flush)
    {
       QTV_MSG( QTVDIAG_AUDIO_TASK, "flush issued from ImageChangeCallback");
       pLocalAudioCMX->Flush();
    }
    else
    {
       QTV_MSG( QTVDIAG_AUDIO_TASK, "cmx already flushed...so avoid flush again");
    }
    pLocalAudioCMX->no_flush = false;
  }
}

#endif // !PLATFORM_LTK
/* ======================================================================
FUNCTION
  AudioCMX::Resume

DESCRIPTION
//
// resume CMX (assuming it's currently paused).

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioCMX::Resume(long &tResume)
{
	rex_timer_type cmx_resume_sync_timer;

    //Init response
    resumeResponse.bResponse=true;
    resumeResponse.callerTcb = rex_self();

    //init the sync event timer.
    rex_def_timer( &cmx_resume_sync_timer, rex_self(), CMX_SYNC_EVENT_TIMER_SIG );

#ifdef SHOW_CMX_DEBUG_INFO
    QTV_MSG( QTVDIAG_AUDIO_TASK, "Resuming CMX");
#endif

#ifndef FEATURE_WINCE
    cmx_audfmt_resume(ResumeCallback, (void *)this);
#endif

    //Wait on response
    if(rex_timed_wait((CMX_SYNC_EVENT_SIG|CMX_SYNC_EVENT_TIMER_SIG),&cmx_resume_sync_timer, COMMON_CMX_RESPONSE_TIMEOUT_MSEC)
       & CMX_SYNC_EVENT_SIG)
    {
        (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_SIG);
        (void)rex_clr_timer(&cmx_resume_sync_timer);

        if (resumeResponse.bSuccess)
        {
            //wait on playing to actually start.
            if (WaitTilPlaying())
            {
                //Update audio resume time
                tResume = ZUtils::Clock();
                #ifdef FEATURE_QTV_EXTENDED_CMX_API
                  if((_codec == Media::AAC_CODEC) && (cmx_codec.aac_codec.format == CMX_AF_AAC_DATA_FORMAT_ADTS))
                  {
                  if(pAudioMgr && pAudioMgr->IsFastPlaybackModeEnabled())
                  {
                    /* For fast playback modes disable SBR */
                    DisableSbrMode();
                    QTV_MSG( QTVDIAG_AUDIO_TASK,
                    "AudioCMX Resume: SBR disabled since fast playback enabled");
                  }
                  else
                  {
                    /* For normal playback modes enable SBR */
                    EnableSbrMode();
                    QTV_MSG( QTVDIAG_AUDIO_TASK,
                    "AudioCMX Resume: SBR enabled for normal playback mode");
                  }
                  }
                #endif
                return true;
            }
        }
    }
    else
    {
      (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_TIMER_SIG);
    }

    //Update audio resume time
    tResume = ZUtils::Clock();

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: CMX Resume failed");
    return false;
}



#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
/* ======================================================================
FUNCTION
  audiocmx_avsync_callback()

DESCRIPTION
  This is the callback function that will report the number of PCM samples
  played.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
  void AudioCMX::audiocmx_avsync_callback(qword timestamp,qword num_of_samples,qword num_of_bytes,const void *client_data )
#else
  #ifdef FEATURE_QTV_CMX_AV_SYNC_INTERFACE
#error code not present
  #else
    void AudioCMX::audiocmx_avsync_callback(qword timestamp,qword num_of_samples)
  #endif
#endif
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioCMX::Callback");
  QTV_MSG2( QTVDIAG_AUDIO_TASK, "audioCMX:current time reported by CMX but not used:%d %d",qw_hi(timestamp),qw_lo(timestamp));

#if(defined FEATURE_QTV_CMX_AV_SYNC_INTERFACE || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "audioCMX:client_data reported by CMX but not used:%d",&client_data);
#endif
  
  if (!bPlaying)
  {
     return;
  }

  uint64 samples;
  QTV_MSG2( QTVDIAG_AUDIO_TASK, "audioCMX:num of samples:%d %d",qw_hi(num_of_samples),qw_lo(num_of_samples));
  samples = (((uint64)qw_hi(num_of_samples))<< 32) + qw_lo(num_of_samples);

#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
  QTV_MSG2( QTVDIAG_AUDIO_TASK, "audioCMX:num of bytes:%d %d",qw_hi(num_of_bytes),qw_lo(num_of_bytes));
  Media::CodecType audioCodec = Media::UNKNOWN_CODEC;
  if(pAudioMgr)
  {
    audioCodec = pAudioMgr->GetAudioCodecType();
  }

  if( (pAudioMgr) && (AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec)))
  {
    long nBytes;
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "audioCMX:num of bytes:%d %d",qw_hi(num_of_bytes),qw_lo(num_of_bytes));
    nBytes = qw_lo(num_of_bytes);
    /* this function is called to do the computation of the offset based on
       the number of bytes played. The offset will determine the sleep time
       required for video */
    #ifndef T_MSM7500
    pAudioMgr->UpdateAVSyncFromCMXBytes(nBytes);
    #else  /* T_MSM7500 */
#error code not present
    #endif /* T_MSM7500 */
    if (pLocalAudioCMX && pLocalAudioCMX->no_flush && nBytes > 0)
    {
      pLocalAudioCMX->no_flush = false;
    }
  }
  else
#endif
  {
    /* this function is called to do the computation of the offset based on
       the number of samples played. The offset will determine the sleep time
       required for video */
    /* SetCurTimeStamp should not updated at end of clip or samples become zero. */
    if(pAudioMgr && (pAudioMgr->GetMediaStatus() != Media::DATA_END || (samples != 0)))
      pAudioMgr->SetCurTimeStamp(samples);
  }

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#if(defined FEATURE_QTV_CMX_AV_SYNC_INTERFACE || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  client_data = NULL;       /* To Fix compiler warnings */
  timestamp = 0;
#endif

}
#endif


#ifdef FEATURE_QTV_EXTENDED_CMX_API
/* ======================================================================
FUNCTION
  audiocmx_avsync_callback_ex()

DESCRIPTION
  This is the extended AV Sync callback function that will report the number
  of PCM samples or byte count rendered. This callback is very specific to
  AAC ADTS Codec. Hence it calls the generic AV Sync Callback Processing
  routine depending on the target and/or features enabled. Main intention here
  is not to duplicate the AVSync Callback processing in two different places.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void AudioCMX::audiocmx_avsync_callback_ex(cmx_av_sync_info_type info,
                                           const void *client_data )

{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioCMX::Callback_Ex(Sample Rate = %d)",info.sample_rate);
  #ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
    audiocmx_avsync_callback(info.timestamp,info.num_of_samples,info.num_of_bytes,client_data );
  #else
    #ifdef FEATURE_QTV_CMX_AV_SYNC_INTERFACE
#error code not present
    #else
      audiocmx_avsync_callback(info.timestamp,info.num_of_samples)
    #endif
  #endif
  return;
}

/* ======================================================================
FUNCTION
  audiocmx_codec_update_callback()

DESCRIPTION
  This callback routine reports the changes in either sample frequency
  or number of channels. Whenever the content changes from stereo/mono
  to dual-mono, CMX/DSP trigger this callback with appropriate data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
void AudioCMX::audiocmx_codec_update_callback(cmx_af_codec_info_type info,
                                              const void *client_data )

{
   QTV_MSG2( QTVDIAG_AUDIO_TASK, "AudioCMX::Codec Param update(Sample Rate = %d),ch=%d",
              info.aac_info.sample_rate,info.aac_info.channel_mode);
   AudioCMX * pThis = (AudioCMX *)client_data;

   if(!pThis)
   {
     QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_ERROR,
     "Ignoring Codec Update callback since client_data is NULL");
     return;
   }
   if(!pAudioMgr)
   {
     QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_ERROR,
     "Ignoring Codec Update callback since AudioMgr object does not exist");
     return;
   }
   /*  Track state change notification
    *  Assumption is that This callback is used to raise the channel change
    *  notifications independent of the target differences.
    *  For 7k targets we use this callback to raise the sampling rate chagne
    *  notification also.
    */

   Common::ChannelConfigType eCC;   /* Channel Configuration */
   Common::AudioTrackState  eState; /* Audio Track State */
   int nSamplingFreq;  /* Sampling Frequency */

   pAudioMgr->GetAudioTrackSpec(eState,eCC,nSamplingFreq);
   /* Convert the cmx channel configuration value to its QTV equivalent */
   eCC = ConvertChannelConfigurationDefn(info.aac_info.channel_mode);
   nSamplingFreq = info.aac_info.sample_rate;

   if (eState == Common::AUDIO_TRACK_STATE_INVALID)
   {
     // Do not raise the change notification for the first time
     eState = Common::AUDIO_TRACK_STATE_SELECTED;
     // Setup the Track Spec Parameters for the first time
     pAudioMgr->SetAudioTrackSpec(eState,eCC,nSamplingFreq);
   }
   else
   {
     if(info.aac_info.channel_mode == CMX_AF_AAC_CHANNEL_UNSUPPORTED)
     {
       // Update the channel to disabled if needed
       eState = Common::AUDIO_TRACK_STATE_DISABLED;
     }
     // Setup the Track Spec Parameters
     pAudioMgr->SetAudioTrackSpec(eState,eCC,nSamplingFreq);
     // Raise the Track State Change Notificaton
     (pThis->pNotify)(Common::NOTIFY_TRACK_STATE_CHANGE, pThis->m_pClientData, NULL);
   }
   return;

}
#endif

/* ======================================================================
FUNCTION
  ConvertChannelConfigurationDefn

DESCRIPTION
  This function maps the CMX channel configuration value to the one that
  could be understood by QTV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

========================================================================== */
Common::ChannelConfigType
AudioCMX::ConvertChannelConfigurationDefn(cmx_af_aac_channel_enum_type eCC)
{
  Common::ChannelConfigType eRet = Common::AUDIO_CHANNEL_UNKNOWN;
  switch(eCC)
  {
    case CMX_AF_AAC_CHANNEL_UNKNOWN:
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK, "Audio channel switched to Unknown");
      eRet = Common::AUDIO_CHANNEL_UNKNOWN;
      break;
    }
    case CMX_AF_AAC_CHANNEL_MONO:
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,
      "Audio channel switched to Single channel (mono) data");
      eRet = Common::AUDIO_CHANNEL_MONO;
      break;
    }
    case CMX_AF_AAC_CHANNEL_DUAL:
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to Stereo data 2(CPE)");
      eRet = Common::AUDIO_CHANNEL_DUAL;
      break;
    }
    case CMX_AF_AAC_CHANNEL_TRIPLE:    /*  (UNSUPPORTED) */
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to 3 channels: 1+2");
      eRet = Common::AUDIO_CHANNEL_TRIPLE;

      break;
    }
    case CMX_AF_AAC_CHANNEL_QUAD:       /*  (UNSUPPORTED) */
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to 4 channels: 1+2+1");
      eRet = Common::AUDIO_CHANNEL_QUAD;
      break;
    }
    case CMX_AF_AAC_CHANNEL_QUINTUPLE:  /*  (UNSUPPORTED)           */
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to 5 channels: 1+2+2");
      eRet = Common::AUDIO_CHANNEL_QUINTUPLE;
      break;
    }
    case CMX_AF_AAC_CHANNEL_SEXTUPLE:   /*  (UNSUPPORTED)       */
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to 5+1 channels: 1+2+2+1");
      eRet = Common::AUDIO_CHANNEL_SEXTUPLE;
      break;
    }
    case CMX_AF_AAC_CHANNEL_OCTUPLE:     /*  (UNSUPPORTED)     */
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to 7+1 channels: 1+2+2+2+1");
      eRet = Common::AUDIO_CHANNEL_OCTUPLE;
      break;
    }

#ifdef FEATURE_QTV_EXTENDED_CMX_API
    case CMX_AF_AAC_CHANNEL_DUAL_MONO:  /*  */
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to Dual Mono: 1+1 (Two SCEs)");
      eRet = Common::AUDIO_CHANNEL_DUAL_MONO;
      break;
    }
    case CMX_AF_AAC_CHANNEL_UNSUPPORTED:
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio channel switched to Unsupported");
      eRet = Common::AUDIO_CHANNEL_UNSUPPORTED;
      break;
    }
#endif /* ifdef FEATURE_QTV_EXTENDED_CMX_API */

    default:
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK, "Audio channel switched to Unknown by default");
      eRet = Common::AUDIO_CHANNEL_UNKNOWN;
      break;
    }
  }

  return eRet;
}

/* ======================================================================
FUNCTION
  AudioCMX::Start

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
bool AudioCMX::Start(long &tStart)
{

  if ( bPlaying )
  {
    QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioCMX::Start() Already Playing => Repos");
    return true;
  }

  rex_timer_type cmx_start_sync_timer;

  //Init resposne
  playResponse.bResponse=true;
  playResponse.callerTcb = rex_self();
#if (!defined PLATFORM_LTK) && (!defined T_MSM7600) && (!defined T_MSM7500) && (!defined T_MSM7200)
  #ifndef FEATURE_WINCE
  snd_set_qtv_cb_func(ImageChangeCallback);
  #endif
#endif // !PLATFORM_LTK

  //init the sync event timer.
  rex_def_timer( &cmx_start_sync_timer, rex_self(), CMX_SYNC_EVENT_TIMER_SIG );


#ifdef SHOW_CMX_DEBUG_INFO
    QTV_MSG( QTVDIAG_AUDIO_TASK, "Starting CMX");
#endif

#ifndef FEATURE_WINCE
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    if( ( _codec == Media::NONMP4_MP3_CODEC) ||
        ( _codec == Media::NONMP4_AAC_CODEC ) ||
        ( _codec == Media::NONMP4_AMR_CODEC ) ||
        ( _codec == Media::QCP_CODEC ) ||
        ( _codec == Media::MIDI_CODEC ))
    {
      // For MP3, QCP, AAC and AMR audio only files
      cmx_audfmt_play(&handle, CMX_AF_CALLBACK_VALUE, CMX_AUDFMT_START_NORMAL, genericPlayCallback, (void *)this);
    }
    else
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
    {
    #ifndef FEATURE_QTV_AVI
    if(_codec == Media::MP3_CODEC)
    {
      cmx_audfmt_play_codec(&handle, &cmx_codec, PlayCallback, (void *)this);
    }
    else
    #else
      #ifndef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
      //To Do: Need to know if the media is AVI file instance
      //Byte Based AV SYNC for Mp3 when played in AVI/DivX
      if(_codec == Media::MP3_CODEC)
      {
        cmx_audfmt_play_codec(&handle, &cmx_codec, PlayCallback, (void *)this);
      }
      else
      #endif
    #endif
    {
      // The MSM Way ...
      #if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
          /* if the cmx a/v sync callback support is enabled, then we call this function and pass
          cmx_av_sync having the callback interval and the callback function */
          #ifndef FEATURE_QTV_CMX_AV_SYNC_BYTES
            if(_codec == Media::WMA_CODEC)
            {
              /*
              * When FEATURE_QTV_CMX_AV_SYNC_BYTES is not defined and codec in WMA,
              * audiocmx::initplay won't fill in AV SYNC callback information in cmx_av_sync.
              * Thus, if we call  cmx_audfmt_play_codec_av_sync AudioCMX::Start will fail and WMA clips
              * won't play.
              */
              cmx_audfmt_play_codec(&handle, &cmx_codec, PlayCallback, (void *)this);
            }
            else
            {
              cmx_audfmt_avsync_playback(&handle, &cmx_codec, &cmx_av_sync, AVSyncPlayCallback, (void *)this);
            }
          #else
            #ifdef FEATURE_QTV_EXTENDED_CMX_API
              if((_codec == Media::AAC_CODEC) && (cmx_codec.aac_codec.format == CMX_AF_AAC_DATA_FORMAT_ADTS))
              {
                // Convert To Extended AAC ADTS Codec Specification
                // Adaptation for new cmx interface changes
                      PrepExtendedCodecSpec(cmx_codec,m_aacADTSSpec);
                      PrepExtendedAVSyncCfg(cmx_av_sync,m_aacADTSAVSync);
                   QTV_MSG(QTVDIAG_AUDIO_TASK,"Audio Start through *NEW* play codec av sync invoked");
                      cmx_audfmt_play_codec_av_sync_ex(&handle, &m_aacADTSSpec, &m_aacADTSAVSync, PlayCallback, (void *)this);
              }
              else
            #endif
              {
                cmx_audfmt_avsync_playback(&handle, &cmx_codec, &cmx_av_sync, AVSyncPlayCallback, (void *)this);
              }
          #endif
      #else
          cmx_audfmt_play_codec(&handle, &cmx_codec, PlayCallback, (void *)this);
      #endif
    }

    //device does not start until data is sent.
    tStart=(-1);

    //Wait on command response
    if(rex_timed_wait((CMX_SYNC_EVENT_SIG|CMX_SYNC_EVENT_TIMER_SIG), &cmx_start_sync_timer, COMMON_CMX_RESPONSE_TIMEOUT_MSEC)
       & CMX_SYNC_EVENT_SIG)
    {
        (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_SIG);
        (void)rex_clr_timer(&cmx_start_sync_timer);

        if (playResponse.bSuccess)
        {
            //wait on playing to actually start.
            if (WaitTilPlaying())
            {
                return true;
            }
        }
    }
    else
    {
      (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_TIMER_SIG);
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: CMX Start failed");
    return false;
}
#endif
  //device does not start until data is sent.
  tStart=(-1);
  return true;
}
#ifdef FEATURE_QTV_EXTENDED_CMX_API
/* ======================================================================
FUNCTION
  AudioCMX::PrepExtendedAVSyncCfg

DESCRIPTION
  Prepares the extended AV Sync Callback details  for AAC ADTS Codec.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

========================================================================== */
void AudioCMX::PrepExtendedAVSyncCfg(cmx_av_sync_type &original,
                                            cmx_av_sync_ex_type &extended)
{
  extended.av_sync_interval = original.av_sync_interval;
  extended.av_sync_mode     = original.av_sync_mode;
  extended.client_data      = original.client_data;
  extended.av_sync_cb_func  = audiocmx_avsync_callback_ex ;
  QTV_MSG2( QTVDIAG_AUDIO_TASK, "PrepExtendedAVSyncCfg: Interval = %d, mode = %d",
  original.av_sync_interval, original.av_sync_mode);

  return;
}

/* ======================================================================
FUNCTION
  AudioCMX::PrepExtendedCodecSpec

DESCRIPTION
  Prepares the extended codec specification for AAC ADTS Codec.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

========================================================================== */
void AudioCMX::PrepExtendedCodecSpec(cmx_codec_spec_type &original,
                                            cmx_codec_spec_ex_type &extended)
{
  extended.aac_bc_codec.codec_type.file_type = CMX_AF_FILE_AAC_BROADCAST;
#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  extended.aac_bc_codec.dual_mono_mode       =
    MapCommonDMTypeToCMXDMType(m_dualMonoOutput);
#else
    extended.aac_bc_codec.dual_mono_mode       = CMX_AF_DUAL_MONO_MODE_DEFAULT;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */
  extended.aac_bc_codec.sbr_mode             = CMX_AF_SBR_MODE_DEFAULT;
  extended.aac_bc_codec.sample_rate          = original.aac_codec.sample_rate;
  extended.aac_bc_codec.codec_update_cb_func = audiocmx_codec_update_callback ;
  if(pAudioMgr && pAudioMgr->IsFastPlaybackModeEnabled())
  {
    /* For fast playback modes disable SBR */
    extended.aac_bc_codec.sbr_mode       = CMX_AF_SBR_MODE_DISABLE;
    QTV_MSG( QTVDIAG_AUDIO_TASK,
    "PrepExtendedCodecSpec: SBR disabled since fast playback enabled");
  }
  // If the sample rate is more than 48K  then truncate it to 48K
  // This includes the Unknown sample rate also. Since we take the
  // sample rate from the valid ADTS header , it can not have the
  // unknown configuration any way.
  if (extended.aac_bc_codec.sample_rate > CMX_AF_SAMPLE_RATE_48000)
  {
    extended.aac_bc_codec.sample_rate = CMX_AF_SAMPLE_RATE_48000;
    QTV_MSG( QTVDIAG_AUDIO_TASK,"Sample rate truncated to 48K ");
  }
  QTV_MSG( QTVDIAG_AUDIO_TASK, "PrepExtendedCodecSpec: Codec = AAC ADTS");
  return;
}
/* ======================================================================
FUNCTION
  AudioCMX::EnableSbrMode

DESCRIPTION
Enable the SBR mode through newly designed CMX API.  If the SBR mode is enabled then audio
decoder render AAC SBR content. This is used when play mode switched normal from 1P3X mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void AudioCMX::EnableSbrMode()
{
  cmx_audio_playback_cmd_param_type cmdParam;

  QTV_MSG( QTVDIAG_AUDIO_TASK,  "AudioCMX Enable SBR Mode ");
  cmdParam.sbr_mode_cmd.mode = CMX_AF_SBR_MODE_ENABLE;
  // At present CMX implementation always return success ,
  // after queueing the command to the sound task.
  // Hence callback function is not supplied.
#ifndef FEATURE_WINCE
  cmx_audio_playback_control_ex(CMX_AUDIO_PB_CMD_SBR_MODE,&cmdParam,NULL,0);
#endif
  return;
}
/* ======================================================================
FUNCTION
  AudioCMX::DisableSbrMode

DESCRIPTION
Set the SBR Mode to enabled or disabled based on the input parameter passed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

========================================================================== */
void AudioCMX::DisableSbrMode()
{
  cmx_audio_playback_cmd_param_type cmdParam;

  QTV_MSG( QTVDIAG_AUDIO_TASK,  "AudioCMX Disable SBR Mode ");
  cmdParam.sbr_mode_cmd.mode = CMX_AF_SBR_MODE_DISABLE;
  // At present CMX implementation always return success ,
  // after queueing the command to the sound task.
  // Hence callback function is not supplied.
#ifndef FEATURE_WINCE
  cmx_audio_playback_control_ex(CMX_AUDIO_PB_CMD_SBR_MODE,&cmdParam,NULL,0);
#endif
  return;
}
#endif

/* ======================================================================
FUNCTION
  AudioCMX::Pause

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
bool AudioCMX::Pause(long &tPause)
{

    //Init response
    pauseResponse.bResponse=true;
    pauseResponse.callerTcb = rex_self();

    //init the sync event timer.
    rex_def_timer( &cmx_sync_timer, rex_self(), CMX_SYNC_EVENT_TIMER_SIG );


#ifdef SHOW_CMX_DEBUG_INFO
    QTV_MSG( QTVDIAG_AUDIO_TASK, "Pausing CMX");
#endif
#ifndef FEATURE_WINCE
    cmx_audio_playback_control(CMX_AUDIO_PB_CMD_PAUSE, PauseCallback, (void *)this);
#endif

    //Wait on response
    if(rex_timed_wait((CMX_SYNC_EVENT_SIG|CMX_SYNC_EVENT_TIMER_SIG), &cmx_sync_timer, COMMON_CMX_RESPONSE_TIMEOUT_MSEC)
       & CMX_SYNC_EVENT_SIG)
    {
        (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_SIG);
        (void)rex_clr_timer(&cmx_sync_timer);

        if (pauseResponse.bSuccess)
        {
            //wait on playing to actually pause.
            if (WaitTilNotPlaying())
            {
                tPause=ZUtils::Clock();
                return true;
            }
        }
    }
    else
    {
      (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_TIMER_SIG);
    }

    //update timing stuff after a pause.
    tPause=ZUtils::Clock();

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: CMX Pause failed");
    return false;
}

/* ======================================================================
FUNCTION
  AudioCMX::Abort

DESCRIPTION
//
// stop cmx, but don't handshake with it.  use this for
// error cases where we're not sure cmx is playing.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::Abort(long &tStop)
{
    QTV_MSG( QTVDIAG_AUDIO_TASK,  "AudioCMX ABORT/CMX_STOP");

    if (bPlaying)
    {
#ifndef FEATURE_WINCE
        cmx_audio_playback_control(CMX_AUDIO_PB_CMD_STOP, NULL, 0);
#endif
        FlushDataRequests();
        (void)WaitTilNotPlaying();
    }
#ifdef LOG_AUDIO_BUFFER_SENT_TO_CMX
  if (fp != NULL)
  {
    (void)OSCL_FileClose(fp);
    fp = NULL;
  }
#endif

    //keep track of audio end time for our stats
    tStop=ZUtils::Clock();
}

/* ======================================================================
FUNCTION
  AudioCMX::Stop

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
bool AudioCMX::Stop(long &tStop)
{
    QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioCMX CMX STOP" );
    rex_timer_type cmx_stop_sync_timer;
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
    if( (_codec != Media::WMA_CODEC) &&
        (_codec != Media::MP3_CODEC) &&
        (_codec != Media::NONMP4_MP3_CODEC) &&
        (_codec != Media::QCP_CODEC) &&
        (_codec != Media::NONMP4_AMR_CODEC) &&
        (_codec != Media::NONMP4_AAC_CODEC) &&
        (_codec != Media::MIDI_CODEC)
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
         && ( (pAudioMgr) && (! (AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(_codec)) ) )
#endif//#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
        )
    {
    /*  if cmxPause was set to true, we need to set it to false */
      pAudioMgr->cmx_avsync_update.cmxPause=false;
      pAudioMgr->cmx_avsync_update.prevResumedTimeStamp=-1;
      pAudioMgr->cmx_avsync_update.prevResumedFrame=0;
    }
#endif /* FEATURE_QTV_CMX_AV_SYNC || FEATURE_QTV_CMX_AV_SYNC_BYTES */

    //Init response
    stopResponse.bResponse=true;
    stopResponse.callerTcb = rex_self();

    //keep track of audio end time for our stats
    tStop=ZUtils::Clock();

    if (!bPlaying)
    {
        return true;
    }
    else
    {
      //init the sync event timer.
      rex_def_timer( &cmx_stop_sync_timer, rex_self(), CMX_SYNC_EVENT_TIMER_SIG );

#ifndef FEATURE_WINCE
      cmx_audio_playback_control(CMX_AUDIO_PB_CMD_FLUSH, StopCallback, (void *)this);
#endif

      //Wait on response
      if(rex_timed_wait((CMX_SYNC_EVENT_SIG|CMX_SYNC_EVENT_TIMER_SIG), &cmx_stop_sync_timer, COMMON_CMX_RESPONSE_TIMEOUT_MSEC)
         & CMX_SYNC_EVENT_SIG)
      {
        (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_SIG);
        (void)rex_clr_timer(&cmx_stop_sync_timer);

        if (stopResponse.bSuccess)
        {
          //wait on playing to actually stop.
          if (WaitTilNotPlaying())
          {
            return true;
          }
        }
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Failed to get stop reponse");
      }
      else
      {
        (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_TIMER_SIG);
      }
    }
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "ERROR: CMX Stop Failed");
    return false;
}


/* ======================================================================
FUNCTION
  AudioCMX::InitPlay

DESCRIPTION
// called before Start to init data.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioCMX::InitPlay(bool bRestart)
{

if(
   #ifndef FEATURE_QTV_AVI
   (_codec != Media::MP3_CODEC) &&
   #else
     //Byte Based AV SYNC for Mp3 when played in AVI/DivX
     //To Do: Need to know if the media is AVI file instance
     #ifndef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
     (_codec != Media::MP3_CODEC) &&
     #endif
   #endif
   (_codec != Media::NONMP4_MP3_CODEC) &&
   (_codec != Media::QCP_CODEC) &&
   (_codec != Media::NONMP4_AMR_CODEC) &&
   (_codec != Media::NONMP4_AAC_CODEC) &&
   (_codec != Media::MIDI_CODEC))
{
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  cmx_av_sync.av_sync_mode = CMX_AV_SYNC_TIME_MS;

#ifdef FEATURE_QTV_LOW_POWER_AUDIO
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  if(isAudioOnlyLocalPlayback() && _codec == Media::AAC_CODEC)
#endif /* FEATURE_QTV_BSAC */
    cmx_av_sync.av_sync_interval = AUDIO_CMX_AV_SYNC_INTERVAL_LOW_POWER;
  else
#endif

  cmx_av_sync.av_sync_interval = AUDIO_CMX_AV_SYNC_INTERVAL;
  cmx_av_sync.av_sync_cb_func = audiocmx_avsync_callback;
  cmx_av_sync.client_data = (void *)this;

  if( (_codec != Media::WMA_CODEC) &&
      ( (pAudioMgr) && (! (AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(_codec)) ) ) )
  {
    /* we initialize the av sync structures used here */
    pAudioMgr->cmx_avsync_update.curTime = 0;
    pAudioMgr->cmx_avsync_update.curTimeStamp = 0;
    pAudioMgr->cmx_avsync_update.cmxPause = false;
    pAudioMgr->cmx_avsync_update.prevResumedTimeStamp = -1;
    pAudioMgr->cmx_avsync_update.prevResumedFrame=0;
    pAudioMgr->cmx_avsync_update.nFrames = 0;
  }
  #if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
     if( (AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(_codec)) &&
      ((_codec == Media::WMA_PRO_CODEC) || (_codec == Media::WMA_PRO_PLUS_CODEC) ) )
     {
      cmx_av_sync.av_sync_interval = AUDIO_CMX_AV_SYNC_INTERVAL_AUDIO_ONLY_PLAYBACK;//In MSEC
  }
  #endif//defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
#else
   #ifdef FEATURE_QTV_CMX_AV_SYNC
#error code not present
   #endif
#endif
}

    //Init stats.
stats.nFlush=0;
stats.nReceived=0;
stats.nGet=0;
stats.nReturned=0;
stats.nReceived=0;
stats.nAnswered=0;

    memset( &handle, 0, sizeof(handle) );
    memset( &cmx_codec, 0, sizeof(cmx_codec) );

    FlushDataRequests();

    if (!bRestart)
    {
#ifdef FEATURE_AAC
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
        if (_codec == Media::AAC_CODEC )
#endif /* FEATURE_QTV_BSAC */
        {
           QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioCMX::InitPlay Decoding AudSpcCfg");
           // Retrieve and decode audioSpecificConfig
           // int retVal = pAudioMgr->GetAndDecodeAudioSpecificConfig(&(cmx_codec.cmx_af_aac_spec));
           int retVal = pAudioMgr->GetAndDecodeAudioSpecificConfig(&(aac_spec));
           if(retVal!=0)
           {
              QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AudioCMX::InitPlay FAILED AudSpcCfg");
              return false;
           }

           // Copy the data over to the init struct.
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
          cmx_codec.aac_codec.codec_type.file_type  = aac_spec.file_type;
          cmx_codec.aac_codec.audio_object          = aac_spec.audio_object;
          cmx_codec.aac_codec.sample_rate           = aac_spec.sample_rate;
          cmx_codec.aac_codec.num_channels          = aac_spec.channel;
          cmx_codec.aac_codec.bit_rate              = aac_spec.bit_rate;
          cmx_codec.aac_codec.format                = CMX_AF_AAC_DATA_FORMAT_RAW;
          Media* pMedia = pAudioMgr->GetMediaPtr();

          Media::AACDataFormatType aacDataFormat = pMedia->GetAACDataFormat();

          switch (aacDataFormat)
          {
            case Media::AAC_DATA_FORMAT_ADTS:
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
              cmx_codec.aac_codec.format = CMX_AF_AAC_DATA_FORMAT_ADTS;
              break;

            case Media::AAC_DATA_FORMAT_RAW:
              cmx_codec.aac_codec.format = CMX_AF_AAC_DATA_FORMAT_RAW;
              break;

            case Media::AAC_DATA_FORMAT_PSEUDO_RAW:
              cmx_codec.aac_codec.format = CMX_AF_AAC_DATA_FORMAT_PSUEDO_RAW;
              pMedia->SetAacFrameMarkerNeeded(true);
              break;

            case Media::AAC_DATA_FORMAT_LOAS:
              cmx_codec.aac_codec.format = CMX_AF_AAC_DATA_FORMAT_LOAS;
			  //Removing this function call as AAC frame marker is not needed
              //for LOAS. 
              //pMedia->SetAacFrameMarkerNeeded(true);
              break;
          }

          cmx_codec.aac_codec.ep_config                             = pAudioMgr->AACERFlags.epConfig;
          cmx_codec.aac_codec.aac_section_data_resilience_flag      = pAudioMgr->AACERFlags.aacSectionDataResilienceFlag;
          cmx_codec.aac_codec.aac_scalefactor_data_resilience_flag  = pAudioMgr->AACERFlags.aacScalefactorDataResilienceFlag;
          cmx_codec.aac_codec.aac_spectral_data_resilience_flag     = pAudioMgr->AACERFlags.aacSpectralDataResilienceFlag;
#else /* FEATURE_MP4_AAC_FRAME_MARKER */
           // This is already initialized later
           // cmx_codec.aac_codec.codec_type   =
           cmx_codec.aac_codec.profile      = aac_spec.profile;
           cmx_codec.aac_codec.sample_rate  = aac_spec.sample_rate;
           cmx_codec.aac_codec.num_channels = aac_spec.channel;
           cmx_codec.aac_codec.bit_rate     = aac_spec.bit_rate;
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
        }
#endif /* FEATURE_AAC */
    }

    //
    // Set cmx handle
    //

    handle.source_type         = CMX_SOURCE_CLIENT;
    handle.client.source_type  = CMX_SOURCE_CLIENT;
    handle.client.data_config  = CMX_CLIENT_DATA_REAL_TIME;
    handle.client.data_req_ptr = DataRequestCallback;

    // Select the CMX Audio Codec Types
    switch( _codec )
    {
    case Media::EVRC_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_QCP_EVRC;
       break;
#ifdef FEATURE_MP4_AMR
    case Media::GSM_AMR_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_AMR;
       break;
#endif /* FEATURE_MP4_AMR */
    case Media::QCELP_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_QCP_13K;
       break;
    case Media::AAC_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_AAC;
       break;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
#if( defined(FEATURE_MP4_MP3) && defined( FEATURE_MP3 ) )
    case Media::MP3_CODEC:
       cmx_codec.mp3_codec.codec_type.file_type  = CMX_AF_FILE_MP3;
       nAudioIFMaxAheadMsec = 500;//In MSEC
       if(pAudioMgr)
       {
         Media* pMedia = pAudioMgr->GetMediaPtr();
         if(pMedia)
         {
           cmx_codec.mp3_codec.sample_rate = TranslateSampleRateForCMX(pMedia->GetAudioSamplingFreq());
         }
       }
       break;
#endif /* FEATURE_MP4_MP3 && FEATURE_MP3 */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
    case Media::NONMP4_MP3_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_MP3;
       break;
    case Media::NONMP4_AAC_CODEC:
      cmx_codec.qcp_codec.file_type = CMX_AF_FILE_AAC;
      break;
    case Media::NONMP4_AMR_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_AMR;
       break;
    case Media::QCP_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_QCP_13K;
       break;
    case Media::MIDI_CODEC:
       cmx_codec.qcp_codec.file_type = CMX_AF_FILE_MIDI;
       break;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#if (defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)) && defined(FEATURE_WMA)
#if defined(FEATURE_QTV_WINDOWS_MEDIA) && defined(FEATURE_WMA)
    case Media::WMA_CODEC:
#endif
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
    case Media::WMA_PRO_CODEC:
    case Media::WMA_PRO_PLUS_CODEC:
#endif
       if (pAudioMgr)
       {
        cmx_codec.wma_codec.file_type = CMX_AF_FILE_WMA;
        cmx_codec.wma_codec.sample_rate = TranslateSampleRateForCMX(pAudioMgr->GetSampleRate());
        cmx_codec.wma_codec.version = pAudioMgr->GetCodecVersion();
        cmx_codec.wma_codec.channels = pAudioMgr->GetNumChannels();
        cmx_codec.wma_codec.bytes_per_second = (uint16)((pAudioMgr->GetBitRate()) / 8);
        cmx_codec.wma_codec.encoder_options = (uint16)pAudioMgr->GetEncoderOptions();
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
        cmx_codec.wma_codec.advanced_encoder_opt1 = (uint16)pAudioMgr->GetAdvancedEncodeOptions();
        cmx_codec.wma_codec.advanced_encoder_opt2 = (uint32)pAudioMgr->GetAdvancedEncodeOptions2();
        cmx_codec.wma_codec.channel_mask = pAudioMgr->GetChannelMask();
        cmx_codec.wma_codec.format_tag = 0X162;
        cmx_codec.wma_codec.bits_per_sample = (uint16)pAudioMgr->GetValidBitsperSample();
#endif
        cmx_codec.wma_codec.virtual_pkt_len = pAudioMgr->GetVirtualPacketSize();

        /* Tuned to allow WM DSP to have enough audio data for low bitrate
        ** decoding. Audio delays are heard through-out the otherwise.
        */
        nAudioIFMaxAheadMsec = 2400;//In MSEC
       }
       break;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

#ifdef FEATURE_QTV_WMA_PRO_DECODER
    case Media::WMA_PRO_CODEC:
    case Media::WMA_PRO_PLUS_CODEC:
       cmx_codec.wave_codec.file_type       = CMX_AF_FILE_WAVE;
       cmx_codec.wave_codec.format          = CMX_AF_WAVE_FORMAT_LINEAR_PCM;
       cmx_codec.wave_codec.sample_format   = CMX_AF_WAVE_SAMPLE_FMT_SIGNED;
       cmx_codec.wave_codec.sample_rate     = pAudioMgr->GetSampleRate();
       cmx_codec.wave_codec.channels        = pAudioMgr->GetNumChannels();
       cmx_codec.wave_codec.bits_per_sample = pAudioMgr->audioInfo.nBitsPerSample;
       nAudioIFMaxAheadMsec                 = MAX_AHEAD_FOR_WM_PRO;
       break;
#endif /* FEATURE_QTV_WMA_PRO_DECODER */

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
#if defined (FEATURE_QTV_AVI_AC3) || defined (FEATURE_QTV_PCM)
#error code not present
#endif /* (FEATURE_QTV_AVI_AC3) || defined (FEATURE_QTV_PCM) */

    default:
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Unknown CMX codec(%d)", _codec );
      return false;
    }

    handle.client.data_len = ~0;

    return true;
}

/* ======================================================================
FUNCTION
 AudioCMX::DeferredInitPlay

DESCRIPTION
 This is called before deferred start to init play.

DEPENDENCIES
 None.

RETURN VALUE
 True/False.

SIDE EFFECTS
 None.

========================================================================== */
bool AudioCMX::DeferredInitPlay()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioCMX::DeferredInitPlay Decoding AudSpcCfg");
  // Retrieve and decode audioSpecificConfig
  int retVal = pAudioMgr->GetAndDecodeAudioSpecificConfig(&(aac_spec));
  if(retVal!=0)
  {
     QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "AudioCMX::DeferredInitPlay FAILED AudSpcCfg");
     return false;
  }
  // Update the sampleRate and channelConfiguration parameters
  cmx_codec.aac_codec.sample_rate           = aac_spec.sample_rate;
  cmx_codec.aac_codec.num_channels          = aac_spec.channel;
  cmx_codec.aac_codec.bit_rate              = aac_spec.bit_rate;
  return true;
}


/* ======================================================================
FUNCTION
  AudioCMX::Create

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
void AudioCMX::Create(void * pMgr, Common::AudioNotifyFuncT pNotifyFunc, void* pClientData)
{
  //save the input parameter
  pAudioMgr=(AudioMgr *)pMgr;
  pLocalAudioCMX = this;
  //save data
  pNotify = pNotifyFunc;

  //save client data
  m_pClientData = pClientData;
  nAudioIFMaxAheadMsec = pAudioMgr->getCMXMaxAheadInMsec();
  if(nAudioIFMaxAheadMsec)
  {
     nAudioIFBufferSizeMsec = CMX_BUFFER_SIZE_MSEC;
#ifndef FEATURE_WINCE
     QTV_MSG2( QTVDIAG_AUDIO_TASK,
               "AudioCMX:: Buffer size %d(Msec),"
               "Max Audio device Lead: %d(Msec)",
                nAudioIFBufferSizeMsec,
                nAudioIFMaxAheadMsec);
#endif
  }
  else
  {
#ifdef FEATURE_QTV_LOW_POWER_AUDIO
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
    if (isAudioOnlyLocalPlayback() && (_codec == Media::AAC_CODEC))
#endif /* FEATURE_QTV_BSAC */
      nAudioIFMaxAheadMsec = CMX_MAX_AHEAD_MSEC_LOW_POWER;
    else
#endif
    nAudioIFMaxAheadMsec   = CMX_MAX_AHEAD_MSEC;
#if defined (FEATURE_AAC) && \
    (defined(FEATURE_QTV_STREAM_RECORD) || \
    defined(FEATURE_QTV_BCAST_FLO) ||\
     defined(FEATURE_QTV_GENERIC_BCAST))

     /* Buffer sizes are tuned for acceptable AV sync performance of
        stream recorded clips.
     */
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
     if (_codec == Media::AAC_CODEC )
#endif /* FEATURE_QTV_BSAC */
     {
         /* Tuned for stream recording */
         nAudioIFBufferSizeMsec = CMX_BUFFER_SIZE_MSEC;

         QTV_MSG2( QTVDIAG_AUDIO_TASK,
               "AudioCMX::AAC Buffer size %d(Msec),"
               "Max Audio device Lead: %d(Msec)",
                nAudioIFBufferSizeMsec,
                nAudioIFMaxAheadMsec);
     }
     else
#endif /* FEATURE_AAC && (FEATURE_QTV_STREAM_RECORD or BCAST_FLO or GENERIC_BCAST)*/
     {
         nAudioIFBufferSizeMsec = CMX_BUFFER_SIZE_MSEC;
#ifndef FEATURE_WINCE
         QTV_MSG2( QTVDIAG_AUDIO_TASK,
               "AudioCMX::Buffer size %d(Msec),"
               "Max Audio Device Lead: %d(Msec)",
                nAudioIFBufferSizeMsec,
                nAudioIFMaxAheadMsec);
#endif
     }

  }
  //set the audio starvation threshold.
  nAudioIFMaxBehindMsec = CMX_AUDIO_STARVATION_THRESHOLD_MSEC;

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

  /*
  ** Now that we're completely initialized, we can accept CMX
  ** data-read requests.
  */
  m_canAcceptDataRequests = SAFE_TO_ACCEPT_DATA_REQS;
  bPlaying=false;
}

/* ======================================================================
FUNCTION
  AudioCMX::Destroy

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
void AudioCMX::Destroy()
{
    QTV_MSG( QTVDIAG_AUDIO_TASK,  "AudioCMX Destroy/CMX_STOP");

    pNotify = NULL;

    /*
    ** We are being destroyed!  Set the flag to prevent us from accepting
    ** further data read requests, since it's now unsafe for us to receive
    ** them, but we can't stop CMX from sending a few more before it stops.
    **
    ** There's nothing magical about UNSAFE_TO_ACCEPT_DATA_REQS.  It's
    ** just a handy mnemonic to remind us what we're doing.  Anything
    ** would work as long as it's not equal to SAFE_TO_ACCEPT_DATA_REQS.
    */
    m_canAcceptDataRequests = UNSAFE_TO_ACCEPT_DATA_REQS;
#ifndef FEATURE_WINCE
    //stop cmx
    cmx_audio_playback_control(CMX_AUDIO_PB_CMD_STOP, NULL, 0);
#endif
    (void)WaitTilNotPlaying();

    /* This critical section prevents destruction of AudioMgr
       while SendData() is not complete
    */
    QCUtils::EnterCritSect(&cmxDataCS);
    pAudioMgr=NULL;
    pLocalAudioCMX = NULL;
    FlushDataRequests();
    no_flush = true;
    /* Deregister the ImageChangeCallback function */
#if (!defined PLATFORM_LTK) && (!defined T_MSM7600) && (!defined T_MSM7500) && (!defined T_MSM7200)
  #ifndef FEATURE_WINCE
  snd_set_qtv_cb_func(NULL);
  #endif
#endif // !PLATFORM_LTK
    QCUtils::LeaveCritSect(&cmxDataCS);

    //CleanupPlay();
}

/* ======================================================================
FUNCTION
  AudioCMX::FlushDataRequests

DESCRIPTION
//
// flush all outstanding data requests by sending empty buffers to cmx.
// this is required after stopping cmx or else cmx can't be restarted.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::FlushDataRequests()
{
  int i, nItems;

  QTV_MSG( QTVDIAG_AUDIO_TASK, "Audio CMX Flush Data Requests");
  nItems = requestQ.NumberOfQueuedItems();

  for (i=0; i < nItems; i++)
  {
    if (requestQ.GetDataRequest(currentRequest))
    {
      if (currentRequest.data_req_cb)
      {
#ifdef SHOW_CMX_DEBUG_INFO_ALL
        QTV_MSG1( QTVDIAG_AUDIO_TASK,
                  "Flushing CMX data request %d",
                  currentRequest.server_client_data);
#endif
        FlushSingleDataRequest( currentRequest.data_req_cb,
                                currentRequest.server_client_data );
      }
      stats.nFlush++;
    }
  }
}

/* ======================================================================
FUNCTION
  AudioCMX::Flush

DESCRIPTION
  Issues a CMX flush and waits for the callback to happen.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::Flush()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "Audio CMX Flush");

  //Init response
  flushResponse.bResponse = true;
  flushResponse.callerTcb = rex_self();

  //init the sync event timer.
  rex_def_timer( &cmx_sync_timer, rex_self(), CMX_SYNC_EVENT_TIMER_SIG );

#ifndef FEATURE_WINCE
  cmx_audio_playback_control(CMX_AUDIO_PB_CMD_FLUSH,
                             FlushCallback,
                             (void *)this);
#endif

  //Wait on response
  if(rex_timed_wait((CMX_SYNC_EVENT_SIG|CMX_SYNC_EVENT_TIMER_SIG), &cmx_sync_timer, COMMON_CMX_RESPONSE_TIMEOUT_MSEC)
     & CMX_SYNC_EVENT_SIG)
  {
    (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_SIG);
    (void)rex_clr_timer(&cmx_sync_timer);

    if (!flushResponse.bSuccess)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "CMX Flush failed");
    }
  }
  else
  {
    (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_TIMER_SIG);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "CMX Flush timed out");
  }
}

/* ======================================================================
FUNCTION
  AudioCMX::FlushSingleDataRequest

DESCRIPTION
  Flush a single data request by sending an empty buffer to CMX.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
========================================================================== */
void AudioCMX::FlushSingleDataRequest
(
  cmx_client_req_cb_func_ptr_type data_req_cb,
  const void *                    server_client_data
)
{
  ASSERT( data_req_cb );
  static char dummy_bites[100];
  QTV_MSG( QTVDIAG_AUDIO_TASK, "Flush data request with 100 dummy bytes buffer");
  (*data_req_cb)( CMX_SUCCESS, (unsigned char*)&dummy_bites, 100, server_client_data );
}

/* ======================================================================
FUNCTION
  AudioCMX::SendDataToCmx

DESCRIPTION
//
// Send data to Cmx in response to a data request.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioCMX::SendDataToCmx(
    Common::AudioDataBuffer * pAudioBuffer,
    long &tPlay)
{
#ifdef SHOW_AUDIO_INFO
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio CMX Send %d Bytes", pAudioBuffer->nData);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Current request %d",currentRequest.data_req_cb);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "pAudio Buffer %d",pAudioBuffer);
#endif

    if (currentRequest.data_req_cb && pAudioBuffer!=NULL)
    {
#ifdef SHOW_AUDIO_INFO
        QTV_MSG1( QTVDIAG_AUDIO_TASK, "audio buffer size %d",pAudioBuffer->nData);
#endif
        if (pAudioBuffer->nData>0)
        {
            //provide data to CMX with the callback function.
#ifdef SHOW_CMX_DEBUG_INFO_ALL
            QTV_MSG1( QTVDIAG_AUDIO_TASK, "Answering CMX data request %d",currentRequest.server_client_data);
            QTV_MSG9( QTVDIAG_AUDIO_TASK, "nbytes %d:  %x %x %x %x %x %x %x %x",
                pAudioBuffer->nData,
                pAudioBuffer->pData[0],
                pAudioBuffer->pData[1],
                pAudioBuffer->pData[2],
                pAudioBuffer->pData[3],
                pAudioBuffer->pData[4],
                pAudioBuffer->pData[5],
                pAudioBuffer->pData[6],
                pAudioBuffer->pData[7]);
#endif
            (*currentRequest.data_req_cb)(CMX_SUCCESS,
                pAudioBuffer->pData,
                pAudioBuffer->nData,
                currentRequest.server_client_data);

#ifdef LOG_AUDIO_BUFFER_SENT_TO_CMX
       if(audioBuf_log_flag)
       {
         logfileSize += pAudioBuffer->nData;
         if(fp == NULL)
         {
           fp = OSCL_FileOpen((OSCL_CHAR *)"AudioBuffer.bin", (OSCL_TCHAR *) _T("wb"));
           if (fp == NULL)
           {
             QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"fp is NULL");
           }
         }
         else if (logfileSize <= LOG_FILE_MAX_SIZE)
         {
           (void)OSCL_FileWrite ( pAudioBuffer->pData, pAudioBuffer->nData, 1, fp );
         }
         else
         {
           (void)OSCL_FileClose(fp);
           fp = NULL;
         }
       }
#endif

            qtv_to_cmx_log_audio_buffer(pAudioBuffer->pData,pAudioBuffer->nData);
            //set start play time for the first buffer.
            //we assume audio starts immediately after the
            //buffer is sent.
            tPlay = ZUtils::Clock();

            stats.nAnswered++;

            //data was sent.
            return true;
        }
        QTV_MSG( QTVDIAG_AUDIO_TASK, "No audio data available");
    }
    return false;
}

/* ======================================================================
FUNCTION
  AudioCMX::SendData

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
bool AudioCMX::SendData(long &tPlayBegin,long &nWaitMsec)
{
    bool bSent=false;
    tPlayBegin=(-1);
    nWaitMsec=0;


  QCUtils::EnterCritSect(&ImageChangeCS);
  if(image_swap && pAudioMgr)
  {
    //Set the flags in the restore buffer to make sure that the next data
    //that will be played out is the unacknowledged data.Invaliate the regular
    //buffers.
      pAudioMgr->audioBuffer.nPendingData   = 0;
      pAudioMgr->audioBuffer.nPendingOffset = 0;

      pAudioMgr->nAVSyncAudioBufferWriteIndex= 0;
      pAudioMgr->nAVSyncAudioBufferSize      = 0;
      pAudioMgr->nAVSyncAudioWriteByteOffSet = 0;
      pAudioMgr->RestoreDataBufferInfo.nEntriesToRestore =
            pAudioMgr->RestoreDataBufferInfo.nEntriesInRestoreBuffer;
      if((int)pAudioMgr->RestoreDataBufferInfo.nEntriesToRestore < 0)
      {
         pAudioMgr->RestoreDataBufferInfo.nEntriesToRestore = 0;
      }
      pAudioMgr->RestoreDataBufferInfo.nTmpReadIndex = pAudioMgr->RestoreDataBufferInfo.nReadIndex;


#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
      if( (pLocalAudioCMX->_codec != Media::WMA_CODEC) &&
          (pLocalAudioCMX->_codec != Media::MP3_CODEC) &&
          (pLocalAudioCMX->_codec != Media::NONMP4_MP3_CODEC) &&
          (pLocalAudioCMX->_codec != Media::QCP_CODEC) &&
          (pLocalAudioCMX->_codec != Media::NONMP4_AMR_CODEC) &&
          (pLocalAudioCMX->_codec != Media::NONMP4_AAC_CODEC) &&
          (pLocalAudioCMX->_codec != Media::MIDI_CODEC)
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
           && ( (pAudioMgr) && (! (AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(pLocalAudioCMX->_codec)) ) )
#endif
         )
      {
      /*  if cmxPause was set to true, we need to set it to false */
        pAudioMgr->cmx_avsync_update.cmxPause=false;
        pAudioMgr->cmx_avsync_update.prevResumedTimeStamp=-1;
        pAudioMgr->cmx_avsync_update.prevResumedFrame=0;
      }
#endif
      pAudioMgr->EndElapsedTime(ZUtils::Clock());


    /* Resume after a reposition */
      pAudioMgr->InitAVTiming();
#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
      if(AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(pLocalAudioCMX->_codec))
      {
        pAudioMgr->m_bCurrentTSFromCMXBytesUpdatePending = true;
      }
#endif

      //Resume
      if (pAudioMgr->xferInfo.startOfInterval.bValid)
      {
         pAudioMgr->xferInfo.startOfInterval.time = ZUtils::Clock();
         pAudioMgr->SetPlaybackOffset(pAudioMgr->xferInfo.startOfInterval.time,
                             pAudioMgr->xferInfo.startOfInterval.timestamp);
      }
      pAudioMgr->StartElapsedTime();
  }
  image_swap = false;
  QCUtils::LeaveCritSect(&ImageChangeCS);


    // get the next data request and put it into the
    // global current request.
    if (requestQ.GetDataRequest(currentRequest))
    {
      stats.nGet++;
      if (currentRequest.stale)
      {
        FlushSingleDataRequest( currentRequest.data_req_cb,
                                currentRequest.server_client_data );
        return false;
      }
    }
    else
    {
       // no requests available now, wait a while before
       // trying again.
       nWaitMsec=REQUEST_POLL_TIME_MSEC;
       //no data was sent.
       return false;
    }

    //Send data
    /* Critical section to prevent destruction of AudioMgr while SendData is not complete */
    QCUtils::EnterCritSect(&cmxDataCS);

    if (pAudioMgr)
    {
        //fill buffer
        if (!pAudioMgr->audioBuffer.nData )
        {
          (void)pAudioMgr->FillAudioBuffer(currentRequest.max_len,currentRequest.offset );
          if(pAudioMgr->IsCodecParamChanged())
          {
            // Since this change has to be acted immediately, i am not setting any waiting time here.
            QTV_MSG(QTVDIAG_AUDIO_TASK,
                 "Dynamic Codec Param change detected while sending data to CMX");
       }

            //send data, even if its of 0 size to satisfy CMX Data Request
            bSent = SendDataToCmx(&pAudioMgr->audioBuffer, tPlayBegin);

#ifdef SHOW_CMX_DEBUG_INFO
            QTV_MSG2( QTVDIAG_AUDIO_TASK, "Data sent (%d) %d Bytes",bSent, pAudioMgr->audioBuffer.nData);
#endif
            pAudioMgr->audioBuffer.nData = 0;
        }
    }
    QCUtils::LeaveCritSect(&cmxDataCS);

    if (!bSent)
    {
        QTV_MSG( QTVDIAG_AUDIO_TASK, "Audio was not sent to CMX!!" );

         //re-queue the data request
         bool bError;
         requestQ.SaveDataRequest(false, //not a new request.
             currentRequest.max_len,
             currentRequest.data_req_cb,
             currentRequest.server_client_data,
             0,
             bError);
         stats.nReturned++;
    }

    return bSent;
}

/* ======================================================================
FUNCTION
  AudioCMX::FlushData

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
bool AudioCMX::FlushData( void )
{
    bool bSent=false;
    long tPlayBegin=(-1);

    // get the next data request and put it into the
    // global current request.
    if (requestQ.GetDataRequest(currentRequest))
    {
      stats.nGet++;
    }
    else
    {
       // no requests available now,
       //no data was sent.
       return false;
    }

    //Send data

    if ( pAudioMgr && (pAudioMgr->audioBuffer.nData) )
    {
        bSent = SendDataToCmx(&pAudioMgr->audioBuffer, tPlayBegin);

#ifdef SHOW_CMX_DEBUG_INFO
        QTV_MSG2( QTVDIAG_AUDIO_TASK, "Data sent (%d) %d Bytes",bSent, pAudioMgr->audioBuffer.nData);
#endif
        pAudioMgr->audioBuffer.nData = 0;
    }

    if (!bSent)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio was not sent to CMX!!" );

         //re-queue the data request
         bool bError;
         requestQ.SaveDataRequest(false, //not a new request.
             currentRequest.max_len,
             currentRequest.data_req_cb,
             currentRequest.server_client_data,
             0,
             bError);
         stats.nReturned++;
    }

    return bSent;
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef SHOW_CMX_DEBUG_INFO
/* ======================================================================
FUNCTION
  ShowStatus

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
static void ShowStatus(cmx_status_type status)
{
    switch(status)
    {
        case CMX_SUCCESS:
       QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_SUCCESS");
       break;
        case CMX_DONE:
       QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_DONE");
       break;
        case CMX_ENABLED:
       QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_ENABLED");
       break;
        case CMX_DISABLED:
       QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_DISABLED");
       break;
        case CMX_TICK_UPDATE:
             QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_TICK_UPDATE");
       break;
        case CMX_REPEAT:
       QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_REPEAT");
       break;
        case CMX_DATA_ACCESS_DELAY:
             QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_DATA_ACCESS_DELAY");
       break;
        case CMX_ABORTED:
       QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_ABORTED");
       break;
        case CMX_FLUSHED:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_FLUSHED");
      break;
        case CMX_INVALID_DATA_SOURCE:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_INVALID_DATA_SOURCE");
      break;
        case CMX_REWIND:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_REWIND");
      break;
        case CMX_FFORWARD:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_FFORWARD");
      break;
        case CMX_PAUSE:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_PAUSE");
      break;
        case CMX_RESUME:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_RESUME");
      break;
        case CMX_FAILURE:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_FAILURE");
      break;
        case CMX_ERROR:
      QTV_MSG( QTVDIAG_AUDIO_TASK, "   CMX_ERROR");
      break;
        default :
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "   CMX Status %d",status);
      break;
    }
}
#endif /* SHOW_CMX_DEBUG_INFO */

/* ======================================================================
FUNCTION
  AudioCMX::CallbackResponse

DESCRIPTION
//
// provide a cmx command success/fail to the audio thread.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::CallbackResponse(const cmx_status_type status, CmxResponse &response)
{
    if (response.bResponse)
    {
        QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioCMX::CallbackResponse callerTcb=0x%x",
                  response.callerTcb);

        //response needed.
        switch (status)
        {
        case CMX_SUCCESS:
            response.bSuccess=true;
            rex_set_sigs(response.callerTcb, CMX_SYNC_EVENT_SIG);
            break;
        case CMX_FAILURE:
            response.bSuccess=false;
            rex_set_sigs(response.callerTcb, CMX_SYNC_EVENT_SIG);
            break;
        default:
            //not a success/fail, do not respond
            break;
        }
    }
}

/* ======================================================================
FUNCTION
  AudioCMX::StopCallback

DESCRIPTION
//
//  CMX is notifying us of results of Stop command.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::StopCallback (
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data   /* Client data passed in to the command  */
)
{
    AudioCMX * pThis = (AudioCMX *)client_data;

#ifdef SHOW_CMX_DEBUG_INFO
    QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX stop callback"); ShowStatus(status);
#endif

    if(status==CMX_SUCCESS)
    {
      bPlaying = FALSE;
    }

    // Provide response to audio thread.
    if (pAudioMgr && pThis)
    {
        pThis->CallbackResponse(status, pThis->stopResponse);
    }

}

/* ======================================================================
FUNCTION
  AudioCMX::PauseCallback

DESCRIPTION
//
//  CMX is notifying us of results of Pause command.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::PauseCallback (
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data   /* Client data passed in to the command  */
)
{
    AudioCMX * pThis = (AudioCMX *)client_data;

#ifdef SHOW_CMX_DEBUG_INFO
    QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX pause callback"); ShowStatus(status);
#endif


    // Provide response to audio thread.
    if (pAudioMgr && pThis)
    {
        pThis->CallbackResponse(status, pThis->pauseResponse);
    }


}

/* ======================================================================
FUNCTION
  AudioCMX::ResumeCallback

DESCRIPTION
//
//  CMX is notifying us of results of Resume command.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::ResumeCallback (
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data   /* Client data passed in to the command  */
)
{
    AudioCMX * pThis = (AudioCMX *)client_data;

#ifdef SHOW_CMX_DEBUG_INFO
    QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX resume callback "); ShowStatus(status);
#endif

    // Provide response to audio thread.
    if (pAudioMgr && pThis)
    {
        pThis->CallbackResponse(status, pThis->resumeResponse);
    }

}

/* ======================================================================
FUNCTION
  AudioCMX::AVSyncPlayCallback

DESCRIPTION
//
//  CMX is notifying us of results of Play command.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::AVSyncPlayCallback(
  cmx_status_type status,
  const void * client_data,
  cmx_af_server_data_type *server_data
)
{
    AudioCMX * pThis = (AudioCMX *)client_data;
    /*
       Sometimes in rapid start stop scenarios, CMX takes more time than
       the timeout of 1s and QTV deletes the AudioCMX instance. By the time
       we get this callback, QTV might have a new AudioCMX instance for new
       playback.If we ignore this callback, CMX can proceed with new playback
    */
    if (pAudioMgr && pThis != pAudioMgr->pAudioCMX)
    {
       return;
    }
    #ifdef SHOW_CMX_DEBUG_INFO
       QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX play callback"); ShowStatus(status);
    #endif

    //catch state changes.
    switch(status)
    {
      case CMX_SUCCESS:
      case CMX_RESUME:
        bPlaying=true;
        break;
      case CMX_ABORTED:
      case CMX_DONE:
      case CMX_PAUSE:
      case CMX_FAILURE:
        bPlaying=false;
        break;
      case CMX_FLUSHED:
        QTV_MSG( QTVDIAG_AUDIO_TASK, "Recieved CMX_FLUSHED");
        if(pAudioMgr && pThis)
        {
          pThis->requestQ.InvalidateDataRequests();
        }
        QCUtils::EnterCritSect(&ImageChangeCS);
	if(pThis)
	{
           pThis->image_swap = true;
	   pThis->no_flush = true;
	}
        rex_leave_crit_sect(&ImageChangeCS);
        break;
    }

    if (pAudioMgr && pThis)
    {
      // Provide command response to audio thread.
      pThis->CallbackResponse(status, pThis->playResponse);
    }

    // Catch callback status updates.
    switch (status)
    {
    case CMX_DATA_ACCESS_DELAY:
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Data Access Delay/EOF/EOS?");

      #ifdef FEATURE_QTV_WINDOWS_MEDIA
        if( pAudioMgr
           && ((pAudioMgr->GetAudioCodecType()== Media::WMA_CODEC)
           #ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
            ||(pAudioMgr->GetAudioCodecType()== Media::WMA_PRO_CODEC)
            ||(pAudioMgr->GetAudioCodecType()== Media::WMA_PRO_PLUS_CODEC)
           #endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
               ) && (pAudioMgr->isStartOfDummyBytes()) )
        {
          //We have already started sending DUMMY bytes and when we get
          //DATA_ACCESS_DELAY from CMX, we won't be getting any more data request
          //so we may not be able to send all of the dummy bytes.In this case,
          //audio will be aborted with audio device starved error.
          //We should indicate Media::DATA_END in sucs case;
          pAudioMgr->SetMediaStatus(Media::DATA_END);
        }
        else if ( pAudioMgr && (pAudioMgr->GetAudioCodecType()== Media::WMA_CODEC) )
        {
          //Reset data lead calculation if we are throttling currently and
          //we receive DATA ACCESS DELAY, which means CMX is out of data
          QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR, "Calling pAudioMgr->ResetThrottling()");
      #ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
          pAudioMgr->ResetThrottling();
      #endif
        }
      #endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
      if( (pAudioMgr)                                        &&
          (pAudioMgr->GetAudioCodecType()!= Media::WMA_CODEC)&&
          (AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(pAudioMgr->GetAudioCodecType()))  )
        {
          //Reset data lead calculation if we are throttling currently and
          //we receive DATA ACCESS DELAY, which means CMX is out of data
          QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR, "Calling pAudioMgr->ResetThrottling()");
          pAudioMgr->ResetThrottling();
        }
#endif//#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES

        // this means cmx is out of data.
        if (pAudioMgr && pThis && pThis->pNotify)
        {
          (pThis->pNotify)(Common::NOTIFY_DELAY, pThis->m_pClientData, NULL);
        }
        break;

    case CMX_DONE:
        //we'll get this if we provided a data byte count
        //when we started up and all bytes have been played.
        if (pAudioMgr && pThis && pThis->pNotify)
        {
          (pThis->pNotify)(Common::NOTIFY_DONE, pThis->m_pClientData, NULL);
        }
        break;

    case CMX_ERROR:
    case CMX_FAILURE:

     if (pAudioMgr && pThis && pThis->pNotify)
     {
       (pThis->pNotify)(Common::NOTIFY_ERROR, pThis->m_pClientData, NULL);
     }
     break;

    case CMX_ABORTED:

     switch(server_data->abort_status)
     {
       case CMX_ABORT_STATUS_CLIENT_ABORT:
         break;

       case CMX_ABORT_STATUS_DATA_ACCESS_ERR:
       case CMX_ABORT_STATUS_DATA_ERR:
       case CMX_ABORT_STATUS_REASON_UNKNOWN:
       case CMX_ABORT_STATUS_RESOURCE_ERR:
       case CMX_ABORT_STATUS_PREEMPTED:
         if (pAudioMgr && pThis && pThis->pNotify)
         {
           (pThis->pNotify)(Common::NOTIFY_ERROR, pThis->m_pClientData, NULL);
         }
       }
     }
}
/* ======================================================================
FUNCTION
  AudioCMX::PlayCallback

DESCRIPTION
//
//  CMX is notifying us of results of Play command.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::PlayCallback(
  cmx_status_type status,
  const void * client_data
)
{
    AudioCMX * pThis = (AudioCMX *)client_data;
    /*
       Sometimes in rapid start stop scenarios, CMX takes more time than
       the timeout of 1s and QTV deletes the AudioCMX instance. By the time
       we get this callback, QTV might have a new AudioCMX instance for new
       playback.If we ignore this callback, CMX can proceed with new playback
    */
    if (pAudioMgr && pThis != pAudioMgr->pAudioCMX)
    {
       return;
    }

#ifdef SHOW_CMX_DEBUG_INFO
    QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX play callback"); ShowStatus(status);
#endif

    //catch state changes.
    switch(status)
    {
      case CMX_SUCCESS:
      case CMX_RESUME:
        bPlaying=true;
        break;
      case CMX_ABORTED:
      case CMX_DONE:
      case CMX_PAUSE:
      case CMX_FAILURE:
        bPlaying=false;
        break;
      case CMX_FLUSHED:
        // Invalidate queued data requests.
        QTV_MSG( QTVDIAG_AUDIO_TASK, "Recieved CMX_FLUSHED");
        if(pAudioMgr && pThis)
        {
        pThis->requestQ.InvalidateDataRequests();
        }
        QCUtils::EnterCritSect(&ImageChangeCS);
	if (pThis)
	{
           pThis->image_swap = true;
	   pThis->no_flush = true;
	}
        rex_leave_crit_sect(&ImageChangeCS);
        break;
    }

    if (pAudioMgr && pThis)
    {
        // Provide command response to audio thread.
        pThis->CallbackResponse(status, pThis->playResponse);
    }

    // Catch callback status updates.
    switch (status)
    {
    case CMX_DATA_ACCESS_DELAY:
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Data Access Delay/EOF/EOS?");

      #ifdef FEATURE_QTV_WINDOWS_MEDIA
        if( pAudioMgr
           && ((pAudioMgr->GetAudioCodecType()== Media::WMA_CODEC)
           #ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
            ||(pAudioMgr->GetAudioCodecType()== Media::WMA_PRO_CODEC)
            ||(pAudioMgr->GetAudioCodecType()== Media::WMA_PRO_PLUS_CODEC)
           #endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
               ) && (pAudioMgr->isStartOfDummyBytes()) )
        {
          //We have already started sending DUMMY bytes and when we get
          //DATA_ACCESS_DELAY from CMX, we won't be getting any more data request
          //so we may not be able to send all of the dummy bytes.In this case,
          //audio will be aborted with audio device starved error.
          //We should indicate Media::DATA_END in sucs case;
          pAudioMgr->SetMediaStatus(Media::DATA_END);
        }
        else if ( pAudioMgr && (pAudioMgr->GetAudioCodecType()== Media::WMA_CODEC) )
        {
          //Reset data lead calculation if we are throttling currently and
          //we receive DATA ACCESS DELAY, which means CMX is out of data
          QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR, "Calling pAudioMgr->ResetThrottling()");
      #ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
          pAudioMgr->ResetThrottling();
      #endif
        }
      #endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
      if( (pAudioMgr)                                        &&
          (pAudioMgr->GetAudioCodecType()!= Media::WMA_CODEC)&&
          (AudioMgr::IS_BYTE_BASED_AVSYNC_SUPPORTED(pAudioMgr->GetAudioCodecType()))  )
        {
          //Reset data lead calculation if we are throttling currently and
          //we receive DATA ACCESS DELAY, which means CMX is out of data
          QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR, "Calling pAudioMgr->ResetThrottling()");
          pAudioMgr->ResetThrottling();
        }
#endif//#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES

        // this means cmx is out of data.
        if (pAudioMgr && pThis && pThis->pNotify)
        {
          (pThis->pNotify)(Common::NOTIFY_DELAY, pThis->m_pClientData, NULL);
        }
        break;

    case CMX_DONE:
        //we'll get this if we provided a data byte count
        //when we started up and all bytes have been played.
        if (pAudioMgr && pThis && pThis->pNotify)
        {
          (pThis->pNotify)(Common::NOTIFY_DONE, pThis->m_pClientData, NULL);
        }
        break;

    case CMX_ERROR:
    case CMX_FAILURE:
    case CMX_ABORTED:
        //some audio error occurred.
        if (pAudioMgr && pThis && pThis->pNotify)
        {
          (pThis->pNotify)(Common::NOTIFY_ERROR, pThis->m_pClientData, NULL);
        }
    }
}

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION
  AudioCMX::setAudioSpec

DESCRIPTION
Audio CMX is notifying the results to the audio thread.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
========================================================================== */
void AudioCMX::setAudioSpec(cmx_af_audio_spec_type * psd)
{

  /*
     If user presses Start/Stop button rapidly, in some cases AudioManger object might
     be deleted before we receive response from CMX so check whether pAudioMgr is still
     valid or not.If it's already deleted then simply ignore the reponse
   */
  if( pAudioMgr )
  {
    if (CMX_AF_FILE_MP3 == psd->file_type)
    {
      cmx_af_mp3_spec_type *  pc = (cmx_af_mp3_spec_type *)psd;
      pAudioMgr->audioInfo.nVersion = (int)pc->version;
      pAudioMgr->audioInfo.nBitRate = (uint32)pc->bitrate;
      pAudioMgr->audioInfo.nChannels = (byte)pc->channel;
      // Convert the sample rate...
      switch (pc->sample_rate)
      {
        case CMX_AF_MP3_SAMPLE_RATE_8000: pAudioMgr->audioInfo.nSamplingRate = 8000; break;
#if defined(FEATURE_STEREO_DAC)
        case CMX_AF_MP3_SAMPLE_RATE_11025: pAudioMgr->audioInfo.nSamplingRate = 11025; break;
        case CMX_AF_MP3_SAMPLE_RATE_12000: pAudioMgr->audioInfo.nSamplingRate = 12000; break;
        case CMX_AF_MP3_SAMPLE_RATE_16000: pAudioMgr->audioInfo.nSamplingRate = 16000; break;
        case CMX_AF_MP3_SAMPLE_RATE_22050: pAudioMgr->audioInfo.nSamplingRate = 22050; break;
        case CMX_AF_MP3_SAMPLE_RATE_24000: pAudioMgr->audioInfo.nSamplingRate = 24000; break;
        case CMX_AF_MP3_SAMPLE_RATE_32000: pAudioMgr->audioInfo.nSamplingRate = 32000; break;
        case CMX_AF_MP3_SAMPLE_RATE_44100: pAudioMgr->audioInfo.nSamplingRate = 44100; break;
        case CMX_AF_MP3_SAMPLE_RATE_48000: pAudioMgr->audioInfo.nSamplingRate = 48000; break;
#endif // defined(FEATURE_STEREO_DAC)
        case CMX_AF_MP3_SAMPLE_RATE_UNKNOWN: // fall thru...
        default:
          pAudioMgr->audioInfo.nSamplingRate = 0;
          break;
      }
    }
    else if( CMX_AF_FILE_AAC == psd->file_type )
    {
      cmx_af_aac_spec_type *  pc = (cmx_af_aac_spec_type *)psd;
      pAudioMgr->audioInfo.nBitRate = (uint16)pc->bit_rate;
      pAudioMgr->audioInfo.nChannels = (byte)pc->channel;
    }
    /* If the file type is of AMR */
    else if(CMX_AF_FILE_AMR == psd->file_type )
    {
       /* Here we need to set all the parameters that we received from CMX
       based on the requirement we can add more */
    }
    /* If the file type is of QCP_13K */
    else if(CMX_AF_FILE_QCP_13K == psd->file_type )
    {
      /* Here we need to set all the parameters that we received from CMX
      based on the requirement we can add more */
    }
    /* If the file type is of QCP_EVRC */
    else if(CMX_AF_FILE_QCP_EVRC == psd->file_type )
    {
      /* Here we need to set all the parameters that we received from CMX
         based on the requirement we can add more */
    }
    /* If the file type is of QCP_AMR */
    else if(CMX_AF_FILE_QCP_AMR == psd->file_type )
    {
      /* Here we need to set all the parameters that we received from CMX
        based on the requirement we can add more */
    }
    /* If the file type is of QCP_EFR */
    else if(CMX_AF_FILE_QCP_EFR == psd->file_type )
    {
      /* Here we need to set all the parameters that we received from CMX
        based on the requirement we can add more */
    }
    /* If the file type is of QCP_FR */
    else if(CMX_AF_FILE_QCP_FR == psd->file_type )
    {
      /* Here we need to set all the parameters that we received from CMX
        based on the requirement we can add more */
    }
    /* If the file type is of QCP_HR */
    else if(CMX_AF_FILE_QCP_HR == psd->file_type )
    {
      /* Here we need to set all the parameters that we received from CMX
      based on the requirement we can add more */
    }
    /* If the file type is of MIDI */
    else if(CMX_AF_FILE_MIDI == psd->file_type )
    {
      /* Here we need to set all the parameters that we received from CMX
         based on the requirement we can add more */
    }
    pAudioMgr->SetAudioSpec();
  }
}


  /*We may have to remove this and merge this with the playcallback
   I'm keeping this seperate because after wards we may require to handle the
   events seperately may be right now there is only one even i.e CMX_AUDIO_SPEC.*/
/* ======================================================================
FUNCTION
  AudioCMX::genericPlayCallback

DESCRIPTION

  CMX is notifying us of results of Play command.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::genericPlayCallback(
  cmx_status_type status,
  const void * client_data,
  cmx_af_server_data_type *server_data
)
{
  AudioCMX * pThis = (AudioCMX *)client_data;
#ifdef SHOW_CMX_DEBUG_INFO
  QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX play callback"); ShowStatus(status);
#endif

  //catch state changes.
  switch(status)
  {
    case CMX_SUCCESS:
    case CMX_RESUME:
      bPlaying=true;
      break;
    case CMX_ABORTED:
    case CMX_DONE:
    case CMX_PAUSE:
    case CMX_FAILURE:
      bPlaying=false;
      break;
    case CMX_TICK_UPDATE:
      if (pAudioMgr)
      {
      pAudioMgr->elapsedTimeForGenericAudio += 1000;
      }
      break;
    case CMX_FLUSHED:
      // Invalidate queued data requests.
        if (pAudioMgr && pThis)
        {
      pThis->requestQ.InvalidateDataRequests();
        }
      break;
  }

  if (pAudioMgr && pThis)
  {
    // Provide command response to audio thread.
    pThis->CallbackResponse(status, pThis->playResponse);
  }

  // Catch callback status updates.
  switch (status)
  {
    case CMX_DATA_ACCESS_DELAY:
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Data Access Delay/EOF/EOS?");
#ifdef FEATURE_QTV_WINDOWS_MEDIA
    if( pAudioMgr && (pAudioMgr->GetAudioCodecType()== (int)Media::WMA_CODEC) && (pAudioMgr->isStartOfDummyBytes()) )
    {
      //We have already started sending DUMMY bytes and when we get
      //DATA_ACCESS_DELAY from CMX, we won't be getting any more data request
      //so we may not be able to send all of the dummy bytes.In this case,
      //audio will be aborted with audio device starved error.
      //We should indicate Media::DATA_END in sucs case;
      pAudioMgr->SetMediaStatus(Media::DATA_END);
    }
    else if ( pAudioMgr && (pAudioMgr->GetAudioCodecType()== (int)Media::WMA_CODEC) )
    {
      //Reset data lead calculation if we are throttling currently and
      //we receive DATA ACCESS DELAY, which means CMX is out of data
      QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR, "Calling pAudioMgr->ResetThrottling()");
      #ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
      pAudioMgr->ResetThrottling();
      #endif
    }
#endif
    // this means cmx is out of data.
    if (pAudioMgr && pThis && pThis->pNotify)
    {
      (pThis->pNotify)(Common::NOTIFY_DELAY, pThis->m_pClientData, NULL);
    }
    break;

  case CMX_DONE:
    //we'll get this if we provided a data byte count
    //when we started up and all bytes have been played.
    if (pAudioMgr && pThis && pThis->pNotify)
    {
      (pThis->pNotify)(Common::NOTIFY_DONE, pThis->m_pClientData, NULL);
    }
    break;

  case CMX_AUDIO_SPEC:
    setAudioSpec((cmx_af_audio_spec_type *)server_data);
    break;

  case CMX_ERROR:
  case CMX_FAILURE:
  case CMX_ABORTED:
    //some audio error occurred.
    if (pAudioMgr && pThis && pThis->pNotify)
    {
      (pThis->pNotify)(Common::NOTIFY_ERROR, pThis->m_pClientData, NULL);
    }
  }
}
#endif /*FEATURE_QTV_GENERIC_AUDIO_FORMAT */

/* ======================================================================
FUNCTION
  AudioCMX::DataRequestCallback

DESCRIPTION
//
//  CMX is requesting data.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioCMX::DataRequestCallback(
  uint32 position,
  uint32 max_len,
  cmx_client_req_cb_func_ptr_type data_req_cb,
  const void *server_client_data,
  const void *client_data
)
{
  AudioCMX * pThis = (AudioCMX *)client_data;

#ifdef SHOW_CMX_DEBUG_INFO_ALL
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "CMX Audio DataReq maxLen %d",max_len);
#endif

  if (pThis)
  {
    /*
    ** Check to see if 'pThis' still points to an AudioCMX object willing
    ** to accept data read requests. If m_canAcceptDataRequests
    ** doesn't match SAFE_TO_ACCEPT_DATA_REQS, then we're looking at
    ** an AudioCMX object which doesn't want any more data requests
    ** or we're looking at memory which used to have an AudioCMX object
    ** in it, but which since has been reused for something else.
    ** Needless to say, it would be unsafe to try and use anything
    ** under 'pThis' in those cases, so just return a null buffer
    ** to CMX and move on if we don't see the magic pattern.
    **
    ** This is a hack, but it's the 'best' hack, given that we
    ** don't want to (or can't):
    ** - turn AudioCMX into a singleton
    ** - rewrite the interface between the snd and audio tasks
    **   to completely remove the possibility of 'late' CMX
    **   data read requests.
    */
    if ( pThis->m_canAcceptDataRequests == SAFE_TO_ACCEPT_DATA_REQS )
    {
      bool bError;
      pThis->requestQ.SaveDataRequest(true, max_len, data_req_cb, server_client_data, position, bError);
      pThis->stats.nReceived++;
      if (bError && pThis->pNotify)
      {
        (pThis->pNotify)(Common::NOTIFY_ERROR, pThis->m_pClientData, NULL);
      }
      /* This is to give the data to CMX even the audio player is in the paused state
         to fix the repositioning issue in generic audio formats*/

      if( (!bPlaying) &&
          (( pThis->_codec == Media::NONMP4_MP3_CODEC) ||
          (pThis->_codec == Media::NONMP4_AAC_CODEC) ||
          (pThis->_codec == Media::NONMP4_AMR_CODEC) ||
          (pThis->_codec == Media::QCP_CODEC)))
          {
            //Send another buffer.
            long tProc;
            long tPlayBegin;
            long nWaitMsec = 0;
            bool bSent=false;

            tProc = ZUtils::Clock();
            tPlayBegin = tProc;
            bSent = pThis->SendData(tPlayBegin, nWaitMsec);
            tProc=ZUtils::Clock() - tProc;
            if ( bSent )
            {
              //Audio was sent
              //clear the data buffer.
              pAudioMgr->audioBuffer.nData=0;
              pAudioMgr->audioBuffer.nSize=0;
              //keep track of max processing time
              QCUtils::EnterCritSect(&pAudioMgr->audioStats.CS);
              if ( tProc > pAudioMgr->audioStats.nMaxProcTime )
              {
                pAudioMgr->audioStats.nMaxProcTime=tProc;
              }
              QCUtils::LeaveCritSect(&pAudioMgr->audioStats.CS);
            }
            else
            {
              //Audio was not sent.
              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Info:No audio sent on polling: nWaitMsec(%d)",nWaitMsec);
            }
          }
          else
          {
            /*
             * Audio thread polls for DATA_REQUEST every REQUEST_POLL_TIME_MSEC milliseconds.
             * For ARM based decoder,we need to provide the data in strict timely manner
             * otherwise, it leads to glitches.This is because the the audio data duration sent to CMX
             * is typically smaller than REQUEST_POLL_TIME_MSEC.
             * Reducing REQUEST_POLL_TIME_MSEC does not help either as
             * audio keeps waking too frequently which kind of puts additional overhead.
             * So, we need to wake audio thread immediately as soon as CMX requests DATA.
             */
             if( (pThis->pNotify) && (!bError) && (pAudioMgr))
             {
               //We have accepted the data request successfully
               int nLead = pAudioMgr->GetAmtBufferedMsec(ZUtils::Clock()) - pThis->nAudioIFMaxAheadMsec;
               /*
                * Before waking make sure audio is not throttling.
                * Otherwise, even though we wake it, it's going to sleep again with out sending any data.
                * Thus, if we know we are not going to send, then don't even bother to wake it.
               */
               if(nLead <= 0)
               {
                 (pThis->pNotify)(Common::NOTIFY_INITIATE_IMMEDIATE_DATA_DELIVERY, pThis->m_pClientData, NULL);
               }
            }
         }
    }
    else
    {
      /*
      ** pThis is unsafe. Return an empty buffer to CMX.
      */

      QTV_MSG( QTVDIAG_AUDIO_TASK,  "AudioCMX flushing late CMX data request" );

      FlushSingleDataRequest( data_req_cb, server_client_data );
    }
  }
}

/* ======================================================================
FUNCTION
  AudioCMX::FlushCallback

DESCRIPTION
//
//  CMX is notifying us of results of Flush command.
//
========================================================================== */
void AudioCMX::FlushCallback(
  cmx_status_type    status,        /* Status of event                       */
  const void         *client_data   /* Client data passed in to the command  */
)
{
  AudioCMX * pThis = (AudioCMX *)client_data;

#ifdef SHOW_CMX_DEBUG_INFO
  QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX flush callback "); ShowStatus(status);
#endif

  // Provide response to audio thread.
  if (pThis)
  {
    pThis->CallbackResponse(status, pThis->flushResponse);
  }
}


/* ======================================================================
FUNCTION
  AudioCMX::CleanupPlay

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
void AudioCMX::CleanupPlay()
{
    //flush any leftover data requests.  must do this or else CMX can't be
    //restarted.
    Flush();

    //show final stats.
#ifdef SHOW_STATS
    QTV_MSG( QTVDIAG_AUDIO_TASK, "AUDIO STATS");
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nFlush %d",stats.nFlush);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nReceived %d",stats.nReceived);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nGet %d",stats.nGet);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nReturned %d",stats.nReturned);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nAnswered %d",stats.nAnswered);
#endif
  QCUtils::DinitCritSect(&cmxDataCS);
  QCUtils::DinitCritSect(&ImageChangeCS);
}

/* ======================================================================
FUNCTION
  AudioCMX::AudioCMX

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
AudioCMX::AudioCMX(Media::CodecType codec) :
m_canAcceptDataRequests( UNSAFE_TO_ACCEPT_DATA_REQS ),
_codec(codec)
{
     nAudioIFBufferSizeMsec=800;
     nAudioIFMaxAheadMsec=0;
     nAudioIFMaxBehindMsec=10000;
     pNotify = NULL;

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  /* Default setting for dual mono output */
  m_dualMonoOutput = Common::DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

    QCUtils::InitCritSect( &cmxDataCS);
    QCUtils::InitCritSect( &ImageChangeCS);
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
     memset(&cmx_av_sync,0,sizeof(cmx_av_sync_type));
    #ifdef FEATURE_QTV_EXTENDED_CMX_API
      // Initialize Extended AV Sync configuration object
      memset(&m_aacADTSAVSync,0,sizeof(m_aacADTSAVSync));
    #endif
#endif
     memset(&handle,0,sizeof(cmx_handle_type ));
     memset(&cmx_codec,0,sizeof(cmx_codec_spec_type));
  #ifdef FEATURE_QTV_EXTENDED_CMX_API
    // Initialize Extended configuration objects
    memset(&m_aacADTSSpec,0,sizeof(m_aacADTSSpec));
  #endif
     memset(&aac_spec,0,sizeof(cmx_af_aac_spec_type));
     memset(&stopResponse,0,sizeof(CmxResponse));
     memset(&pauseResponse,0,sizeof(CmxResponse));
     memset(&playResponse,0,sizeof(CmxResponse));
     memset(&resumeResponse,0,sizeof(CmxResponse));
     memset(&flushResponse,0,sizeof(CmxResponse));
     memset(&currentRequest,0,sizeof( AudioRequestQ::DataRequest));
     memset(&stats,0,sizeof(Stats ));
     image_swap = false;
     no_flush = false;
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
}

/* ======================================================================
FUNCTION
  AudioCMX::TranslateSampleRateForCMX

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
cmx_af_sample_rate_enum_type AudioCMX::TranslateSampleRateForCMX(uint32 SampleRate)
{
  cmx_af_sample_rate_enum_type cmxSampleRate;
  switch(SampleRate)
  {
    case 8000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_8000;
      break;

    case 11025:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_11025;
      break;

    case 12000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_12000;
      break;

    case 16000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_16000;
      break;

    case 22050:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_22050;
      break;

    case 24000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_24000;
      break;

    case 32000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_32000;
      break;

    case 44100:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_44100;
      break;

    case 48000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_48000;
      break;

#ifdef FEATURE_AAC
    case 64000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_64000;
      break;

    case 88200:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_88200;
      break;

    case 96000:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_96000;
      break;
#endif /* FEATURE_AAC        */

    default:
      cmxSampleRate = CMX_AF_SAMPLE_RATE_UNKNOWN;
      break;
  }
  return cmxSampleRate;
}

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
/* ======================================================================
FUNCTION
  AudioCMX::SetDualMonoOutput

DESCRIPTION
  Sets the dual mono output configuration.

PARAMETERS:
  Common::DualMonoOutputType dualMonoOutput
    The desired dual mono output configuration.

RETURN VALUE:
  bool
    TRUE if configuration was set successfully, FALSE otherwise.
========================================================================== */
bool AudioCMX::SetDualMonoOutput(Common::DualMonoOutputType dualMonoOutput)
{
  QTV_MSG1(QTVDIAG_AUDIO_TASK,
           "Audio CMX SetDualMonoOutput %d", dualMonoOutput);

  bool bRet = true;
  cmx_audio_playback_cmd_param_type cmxPlaybackCmdParam;
  rex_timer_type cmx_dual_mono_sync_timer;

  /* Save the new setting */
  m_dualMonoOutput = dualMonoOutput;

  /* Send the command to CMX only if we're playing */
  if (bPlaying)
  {
    cmxPlaybackCmdParam.dm_mode_cmd.chan_mode =
      MapCommonDMTypeToCMXDMType(m_dualMonoOutput);

    /* Init response */
    dualMonoOutputResponse.bResponse = true;
    dualMonoOutputResponse.callerTcb = rex_self();

    /* Init the sync event timer */
    rex_def_timer( &cmx_dual_mono_sync_timer, rex_self(), CMX_SYNC_EVENT_TIMER_SIG );
#ifndef FEATURE_WINCE
    cmx_audio_playback_control_ex(CMX_AUDIO_PB_CMD_DUAL_MONO_MODE,
                                  &cmxPlaybackCmdParam,
                                  DualMonoOutputCallback,
                                  (void *)this);
#endif

    /* Wait on response */
    if(rex_timed_wait((CMX_SYNC_EVENT_SIG|CMX_SYNC_EVENT_TIMER_SIG), &cmx_dual_mono_sync_timer, COMMON_CMX_RESPONSE_TIMEOUT_MSEC)
     & CMX_SYNC_EVENT_SIG)
    {
      (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_SIG);
      (void)rex_clr_timer(&cmx_dual_mono_sync_timer);

      if (!dualMonoOutputResponse.bSuccess)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                     "Audio CMX SetDualMonoOutput failed");

        bRet = false;
      }
    }
    else
    {
      (void)rex_clr_sigs(rex_self(), CMX_SYNC_EVENT_TIMER_SIG);

      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
                   "Audio CMX SetDualMonoOutput timed out");

      bRet = false;
    }
  }

  QTV_MSG1(QTVDIAG_AUDIO_TASK,
           "Audio CMX SetDualMonoOutput returns %d", bRet);

  return bRet;
}

/* ======================================================================
FUNCTION
  AudioCMX::DualMonoOutputCallback

DESCRIPTION
  Callback for CMX to notify Qtv the status of dual mono setting.

PARAMETERS:
  cmx_status_type status
    Status event.
  const void *client_data
    Client data.

RETURN VALUE:
  None.
========================================================================== */
void AudioCMX::DualMonoOutputCallback(
  cmx_status_type    status,
  const void         *client_data
)
{
  AudioCMX * pThis = (AudioCMX *)client_data;

  QTV_MSG( QTVDIAG_AUDIO_TASK, "CMX DualMonoOutput callback");
#ifdef SHOW_CMX_DEBUG_INFO
  ShowStatus(status);
#endif /* SHOW_CMX_DEBUG_INFO */

  /* Provide response to audio thread. */
  if (pAudioMgr && pThis)
  {
    pThis->CallbackResponse(status, pThis->dualMonoOutputResponse);
  }
}

/* ======================================================================
FUNCTION
  AudioCMX::MapCommonDMTypeToCMXDMType

DESCRIPTION
  Maps the Common dual mono output type to CMX dual mono output type.

PARAMETERS:
  Common::DualMonoOutputType dualMonoOutput
    The desired dual mono output configuration.

RETURN VALUE:
  cmx_af_dual_mono_mode_enum_type
    The mapped dual mono output.
========================================================================== */
cmx_af_dual_mono_mode_enum_type AudioCMX::MapCommonDMTypeToCMXDMType(
  Common::DualMonoOutputType dualMonoOutput)
{
  cmx_af_dual_mono_mode_enum_type mappedDualMonoOutput;

  switch (dualMonoOutput)
  {
    case Common::DUAL_MONO_OUTPUT_MAIN_TO_L_AND_R:
    {
      mappedDualMonoOutput = CMX_AF_DUAL_MONO_MODE_FL_FR;
      break;
    }

    case Common::DUAL_MONO_OUTPUT_SUB_TO_L_AND_R:
    {
      mappedDualMonoOutput = CMX_AF_DUAL_MONO_MODE_SL_SR;
      break;
    }

    case Common::DUAL_MONO_OUTPUT_MAIN_TO_L_SUB_TO_R:
    {
      mappedDualMonoOutput = CMX_AF_DUAL_MONO_MODE_FL_SR;
      break;
    }

    case Common::DUAL_MONO_OUTPUT_SUB_TO_L_MAIN_TO_R:
    {
      mappedDualMonoOutput = CMX_AF_DUAL_MONO_MODE_SL_FR;
      break;
    }

    default:
    {
      mappedDualMonoOutput = CMX_AF_DUAL_MONO_MODE_FL_FR;
      break;
    }
  } /* switch (dualMonoOutput) */

  return mappedDualMonoOutput;
}
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

#ifdef FEATURE_QTV_LOW_POWER_AUDIO
bool AudioCMX::isAudioOnlyLocalPlayback()
{
  if(pAudioMgr)
  {
    return pAudioMgr->isAudioOnlyLocalPlayback();
  }
  return false;
}
#endif


