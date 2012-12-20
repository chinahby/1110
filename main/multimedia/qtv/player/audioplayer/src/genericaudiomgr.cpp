/* =======================================================================
                               genericaudiomgr.cpp
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
  is only needed if the order of operations  is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/genericaudiomgr.cpp#12 $
$DateTime: 2009/02/11 23:11:26 $
$Change: 839640 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer.h"
#include "audio.h"
#include "AVSync.h"
#include "genericaudioMgr.h"
#include "audiocmx.h"

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT

#define AUDIO_END_DELAY_AFTER_SEEK_TO_END 500 //in milliseconds
#define AUDIO_END_DELAY 4000 //in milliseconds
/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

//#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
//    extern "C" boolean qtv_cfg_enable_dsp_release;
//#endif

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION
  AudioMgr::DestroyAudioInterface

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
void genericAudioMgr::DestroyAudioInterface()
{
  if ( pAudioCMX )
  {
    pAudioCMX->Destroy();
    QTV_Delete( pAudioCMX );/*lint !e449 */
    pAudioCMX=NULL;
  }
}
/* ======================================================================
FUNCTION
  AudioMgr::CreateAudioInterface

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
bool genericAudioMgr::CreateAudioInterface(Media::CodecType codec)
{
   //delete old interface
  DestroyAudioInterface();


  //Create new interface
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "CreateAudioInterface(%d) through CMX", codec );
  switch ( codec )
  {
  case Media::EVRC_CODEC:
  case Media::QCELP_CODEC:
  case Media::AAC_CODEC:
  case Media::MP3_CODEC:
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  case Media::NONMP4_MP3_CODEC:
  case Media::QCP_CODEC:
  case Media::NONMP4_AMR_CODEC:
  case Media::NONMP4_AAC_CODEC:
  case Media::MIDI_CODEC:
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_MP4_AMR
  case Media::GSM_AMR_CODEC:
#endif /* FEATURE_MP4_AMR */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
  case Media::WMA_CODEC:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
    pAudioCMX = QTV_New_Args( AudioCMX, (codec) );
    break;
  }
  //Call the init function for the audio interface.
  if ( pAudioCMX )
  {
    pAudioCMX->Create( (void *)this, notifyFunc, m_pClientData );
    return true;
  }

  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::CreateAudioInterface failed");
  return false;
}

/* ======================================================================
FUNCTION
  genericAudioMgr::SetEndDelay

DESCRIPTION
Change the End delay returned by PredictEndDelay to 500ms

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void genericAudioMgr::SetEndDelay()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "genericAudioMgr::SetEndDelay");
 //Change the End delay returning by PredictEndDelay to 500ms.
  m_endDelay = AUDIO_END_DELAY_AFTER_SEEK_TO_END;
}

/* ======================================================================
FUNCTION
  genericAudioMgr::PredictEndDelay

DESCRIPTION
//
// entering "Ending" state.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void genericAudioMgr::PredictEndDelay(long &tEnd)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "genericAudioMgr::PredictEndDelay");
  //Default End delay is 4000 milliseconds.
  tEnd = m_endDelay;
}

/* ======================================================================
FUNCTION
  genericAudioMgr::GetElapsedTime

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
long genericAudioMgr::GetElapsedTime()
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "elapsed time that we are returning in GM **===**%ld", elapsedTimeForGenericAudio);
  return elapsedTimeForGenericAudio;

}

/* ======================================================================
FUNCTION
  genericAudioMgr::SetAudioSpec

DESCRIPTION
  Function to set the

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void genericAudioMgr::SetAudioSpec()
{
  QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "genericAudioMgr::SetAudioSpec %ld", audioInfo.nBitRate);
  if(pMpeg4)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "calling pMpeg4->SetAudioBitRate with %ld", audioInfo.nBitRate);
    pMpeg4->SetAudioBitRate(audioInfo.nBitRate);
  }
  if ( notifyFunc ) (notifyFunc)(Common::NOTIFY_SPEC, m_pClientData, NULL);
}

/* ======================================================================
FUNCTION
  genericAudioMgr::Prep

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
bool genericAudioMgr::Prep(
                   Media * pMpeg4In,
                   long nTimingUpdateMsec,
                   bool bRestart)
{
  Media::CodecType audioCodec = GetAudioCodecType();
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "genericAudioMgr::Prep %d", bRestart);

#ifdef SHOW_TIMING
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "genericAudioMgr::StartPlaying entry %d",ZUtils::Clock());
#endif


  if ( !bRestart )
  {
    //save media pointer.
    pMpeg4     = pMpeg4In;
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */
    if ( pMpeg4==NULL )
    {
      return false;
    }


    ///////////////////////
    //get audio parameters
    ///////////////////////
    bool bMp4Error = false;

    ///////////////////////
    //create new audio interface.
    ///////////////////////
    if ( !CreateAudioInterface(pMpeg4->GetAudioCodecType()) )
    {
      return false;
    }

    /* If this prep is after pause, reposition and resume we need to take
    elapsedTimeForGenericAudio from the media otherwise this value always set
    to zero. */
    if(pMpeg4->IsGenericAudioFileRepositioned())
    {
      elapsedTimeForGenericAudio = pMpeg4->GetTimeStampFromMedia();
    }

    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "doing pMpeg4->resetInitialization()");
    /* In audio.cpp we faked RESUME is successful when try to start/resume
     the audio even though the Media DATA_STATE is DATA_END. So we shouldn't
     reset m_bMediaInitialized, so that media won't wait on any CMX events.
     This will help to end the audio.
    */
    if(!m_faked_resume)
    {
      pMpeg4->resetInitialization();
    }

    //WZeng, 05/22/02
    // Decode audioSpecificConf information for aac
    if ( pMpeg4->GetAudioCodecType() == Media::AAC_CODEC )
    {
      cmx_af_aac_spec_type *pCmx_af_aac_spec;

      QTV_MSG( QTVDIAG_AUDIO_TASK,  "PrepPlay Decode AudSpcCfg");
      pCmx_af_aac_spec = (cmx_af_aac_spec_type *) ((AudioCMX*)pAudioCMX)->GetCmx_af_aac_spec();
      int retVal = GetAndDecodeAudioSpecificConfig(pCmx_af_aac_spec);
      if ( retVal )
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PrepPlay FAILED Decode AudSpcCfg");
        return false;
      }
    }
    //end, WZeng, 05/22/02

    audioInfo.nMaxBytesPerFrame = pMpeg4->GetMaxAudioBufferSizeDB();
    QTV_MSG1( QTVDIAG_AUDIO_TASK,  "MaxBytesPerFrame %d", audioInfo.nMaxBytesPerFrame );
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nSamplingRate = pMpeg4->GetAudioSamplingFreq();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nSamplesPerFrame = pMpeg4->GetAudioSamplesPerFrame();
    QTV_MSG1( QTVDIAG_AUDIO_TASK,  "Samples per frame %ld", audioInfo.nSamplesPerFrame );

    //sateesh: commented here
     //if (pMpeg4->GetAudioCodecType() == Media::AAC_CODEC)
    //{
    //   if(QTV_AAC_SAMPLES_PER_FRAME != audioInfo.nSamplesPerFrame)
    //   {
    //     /* Current(Dec 2004) AAC/AAC+ firmware supports only 1024 samples per frame.*/
    //        QTV_MSG1( QTVDIAG_AUDIO_TASK,
    //                  "Qtv does not support AAC %ld Samples per frame."
    //                  "Using 1024 samples per frame",
    //                   audioInfo.nSamplesPerFrame );

    //      audioInfo.nSamplesPerFrame = QTV_AAC_SAMPLES_PER_FRAME;
    //   }
    //}

    audioInfo.nDurationMsec = pMpeg4->GetAudioDurationMsec();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

#ifdef FEATURE_QTV_WINDOWS_MEDIA
    audioInfo.nBitsPerSample = pMpeg4->GetAudioBitsPerSample();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nChannels = pMpeg4->GetNumAudioChannels();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nVersion = pMpeg4->GetAudioCodecVersion();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nAsfPacketSize = pMpeg4->GetFixedAsfAudioPacketSize();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

    // Return now if any MP4 error.
    if ( bMp4Error )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "MP4 Error in audioMgr::Prep");
      return false;
    }

    ///////////////////////
    // Calculate more parameters.
    ///////////////////////

    //frames per msec.
   if (!IS_GENERIC_AUDIO_FORMAT(audioCodec) &&
        ( audioInfo.nSamplesPerFrame==0 ))
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "nSamplesPerFrame == 0" );
      //prevent div by zero.
      return false;
    }

    //set the sampling rate if not AAC - jg
    if ( audioInfo.nSamplingRate == 0 )
    {
      audioInfo.nSamplingRate = 8000;
    }

    clipInfo.fFramesPerMsec  = (float)audioInfo.nSamplingRate / (float)audioInfo.nSamplesPerFrame;
    clipInfo.fFramesPerMsec /= (float)1000;
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Frames/ms = %d/1000", (int)(clipInfo.fFramesPerMsec*1000) );
#ifdef SHOW_TIMING2
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "MedTScale %d spfrm %d",
              audioInfo.nMediaTimescale,
              audioInfo.nSamplesPerFrame);
#endif

    //total audio bytes
    //get num frames
    float f = (float)audioInfo.nDurationMsec * clipInfo.fFramesPerMsec;
    audioInfo.nTotalBytes = (long)f;
    //add one to compensate for truncation, if the duration is nonzero.
    if(audioInfo.nDurationMsec > 0)
    {
      audioInfo.nTotalBytes++;
    }
    //get num bytes.  this is only accurate for file playback
    //of fixed sample size!  otherwise it will be too large.
    audioInfo.nTotalBytes *= audioInfo.nMaxBytesPerFrame;

    //max samples per buffer, 800 msec for CMX
    if ( pAudioCMX )
    {
      f = (float)pAudioCMX->nAudioIFBufferSizeMsec * clipInfo.fFramesPerMsec;
    }
    else
    {
      f = 0;
    }
    clipInfo.nFramesPerBuffer = ZMAX(1, (long)f);

    //timing update interval
    f = (float)nTimingUpdateMsec * clipInfo.fFramesPerMsec / (float)clipInfo.nFramesPerBuffer;
    clipInfo.nTimingUpdateIntervalBuffers = ZMAX(1, (long)f);

#ifdef SHOW_TIMING
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "FrmPerBuf %d, TimUpdInv %d",
              clipInfo.nFramesPerBuffer,
              clipInfo.nTimingUpdateIntervalBuffers);
#endif

    ///////////////////////
    // Init audio buffer
    ///////////////////////
    //allocate new buffer

    audioBuffer.nMaxSize = clipInfo.nFramesPerBuffer;
    audioBuffer.nMaxData = clipInfo.nFramesPerBuffer * audioInfo.nMaxBytesPerFrame;

    if ( audioBuffer.nMaxData > 0 )
    {
      if ( pAudioCMX )
      {
        if ( audioBuffer.pData==NULL )
        {
          audioBuffer.pData = (unsigned char*)QTV_Malloc(audioBuffer.nMaxData);
          if ( audioBuffer.pData == NULL )
          {
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Can't allocate data buffer of size %d",audioBuffer.nMaxData);
            return false;
          }
          audioBuffer.pPendingData = (unsigned char*)QTV_Malloc(audioInfo.nMaxBytesPerFrame);
          if ( audioBuffer.pPendingData == NULL )
          {
            QTV_Free(audioBuffer.pData);
            QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                        "Can't allocate data buffer of size %d",audioInfo.nMaxBytesPerFrame);
            return false;
          }
        }
      }
    }
    else
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio data size=(%ld) error", audioBuffer.nMaxData);
      return false;
    }
  }

  //start with empty buffer.
  audioBuffer.nData=0;              // number of bytes in audio buffer
  audioBuffer.nSize=0;              // number of samples in audio buffer
  audioBuffer.nFirstFrame=0;
  audioBuffer.nFirstTimestamp=(-1);
  audioBuffer.nLastFrame=0;
  audioBuffer.nLastTimestamp=(-1);
  audioBuffer.nPendingData = 0;
  audioBuffer.nPendingOffset = 0;
  audioBuffer.nPartialFrame  = 0;

  if ( !bRestart )
  {
    ///////////////////////
    //Init audio interface.
    ///////////////////////
    //This needs to come after audio buffer and audio info are initialized.
    if ( pAudioCMX )
    {
      (void)pAudioCMX->InitPlay(bRestart);
    }
  }
  /* We are setting the repositioning allowed flag to true so that
     after resume from pause we can move the CMX to the proper position
     for generic audio formats and will resume from the position that we paused. */
  if( mediaInfo.dataState==Media::DATA_END)
  {
    pMpeg4->setRepositioningAllowed(true);
  }

  /* Here for the generic audio formats changing mediaInfo.dataState to Media::DATA_OK
    from what ever the status may be because, there is corner case, if we reached media
    end and then if we do pause/resume or pause/reposition then we are failing to send
    data to CMX after start of CMX and this leads to Ab_error */
   mediaInfo.dataState = Media::DATA_OK;

#ifdef SHOW_TIMING
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::StartPlaying exit %d",ZUtils::Clock());
#endif
  return true;
}

/* ======================================================================
FUNCTION
  AudioMgr::PlaySomeAudio

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
bool genericAudioMgr::PlaySomeAudio(
                            long &nWaitMsec, //output: time calling thread should sleep before calling again.
                            bool &bError,    //output: error occurred.
                            Media::MediaStatus &dataStateOut, //output: data state
                            long &cmxDataLead                 //output: CMX data lead
                            )
{

   //default return vals.
  bError=false;
  nWaitMsec = 0;
  bool bSent=false;
  Media::CodecType audioCodec = GetAudioCodecType();
#ifdef FEATURE_QTV_GENERIC_AUDIO_DEBUGGING
#error code not present
#endif /* FEATURE_QTV_GENERIC_AUDIO_DEBUGGING */

  ////////////////////////////////
  // Do audio data throttling if necessary.  This stops audio
  // processing from getting too far ahead of playback.
  // Why is this necessary?
  // 1) For "pull" interfaces we don't know how much audio
  //    the device can buffer.  If the device buffers too much,
  //    the the player could get driven into rebuffering during
  //    streaming.
  // 2) For "push" interfaces this throttling prevents overflowing
  //    the device buffer.
  ////////////////////////////////

  if ( pAudioCMX->nAudioIFMaxAheadMsec!=(-1) )
  {
    //check data position vs calculated play position.
    long nDataLead = GetAmtBufferedMsec(ZUtils::Clock());

#ifdef SHOW_TIMING
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioDataLead %d", nDataLead);
#endif

    cmxDataLead = nDataLead;

    //compare data to play position and keep stats.
    if( ( nDataLead < 0 )&&
        (!IS_GENERIC_AUDIO_FORMAT(audioCodec)))
    {
      //if this happens audio will starve.
      QCUtils::EnterCritSect(&audioStats.CS);
      audioStats.nMaxBehind=ZMAX(audioStats.nMaxBehind,(-nDataLead));
      QCUtils::LeaveCritSect(&audioStats.CS);

      //if audio is starved for a long time then something is wrong.
      //Probably the device on the other side of a "pull" interface
      //has died.  Abort audio now.
      if ( ((-nDataLead)>pAudioCMX->nAudioIFMaxBehindMsec) && (mediaInfo.dataState!=Media::DATA_END) )
      {
      /*-------------------------------------------------------------------
        For Bcast Media sources, we don't care how far audio gets behind.
        Keep playing. This is equally true for file based play also.
      -------------------------------------------------------------------*/
#ifndef FEATURE_QTV_GENERIC_BCAST
        if (pMpeg4->GetSource() != Media::BCAST_FLO_SOURCE)
#else
        if (IsFileBcastMediaSource(pMpeg4,m_pAVSync))
#endif
        {
#ifdef SHOW_TIMING3
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "nDataLead:%d", abs(nDataLead));
#endif
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Audio device starved for %d",nDataLead);
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "...Time %d",ZUtils::Clock());
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nLastTimestampSent %d",xferInfo.nLastTimestampSent);
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nLastFrameSent %d",xferInfo.nLastFrameSent);
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nBytesSent %d",xferInfo.nBytesSent);
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "...nBuffersSentInInterval %d",xferInfo.nBuffersSentInInterval);
          QTV_MSG( QTVDIAG_AUDIO_TASK, "...Start of interval");
          //sateesh: commented here
        //ShowInterval(xferInfo.startOfInterval);
          QTV_MSG( QTVDIAG_AUDIO_TASK,  "PlaySomeAudio Audio Starved");
          bError=true;
        }
      }
    }
    else
    {
      QCUtils::EnterCritSect(&audioStats.CS);
      audioStats.nMaxAhead=ZMAX(audioStats.nMaxAhead,nDataLead);
      QCUtils::LeaveCritSect(&audioStats.CS);
    }

    //calcuate how long to wait for play position to catch up
    //to the maximum allowed data lead.
    nWaitMsec = nDataLead - pAudioCMX->nAudioIFMaxAheadMsec;

#ifdef FEATURE_QTV_WINDOWS_MEDIA
    if(( ((int)Media::WMA_CODEC == audioCodec) && isStartOfDummyBytes() ) ||
        (IS_GENERIC_AUDIO_FORMAT(audioCodec)))
    {
      //Don't throttle if asffile has already started sending dummy bytes.
      nWaitMsec = 0;
    }
#endif

#ifdef SHOW_TIMING
    QTV_MSG3( QTVDIAG_AUDIO_TASK, " Wait %d Lead %d MaxAhead %d",
              nWaitMsec, nDataLead, pAudioCMX->nAudioIFMaxAheadMsec);
#endif
  }

  if ( nWaitMsec<=0 && !bError )
  {
    //Send another buffer.
    long tProc;
    long tPlayBegin;

    tProc = ZUtils::Clock();
    tPlayBegin = tProc;

    bSent = pAudioCMX->SendData(tPlayBegin, nWaitMsec);
    tProc=ZUtils::Clock() - tProc;

    if ( bSent )
    {
      //Audio was sent

//#ifdef SHOW_TIMING
      QTV_MSG3( QTVDIAG_AUDIO_TASK, "  AUDIO SENT f %d, pres %d size %d",
                audioBuffer.nFirstFrame,
                audioBuffer.nFirstTimestamp,
                audioBuffer.nData);
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "  ...clk %d", ZUtils::Clock());
      QTV_MSG2( QTVDIAG_AUDIO_TASK, "  ...last f %d ts %d",
                audioBuffer.nLastFrame,
                audioBuffer.nLastTimestamp);
//#endif

      //update audio transfer info
      xferInfo.nBuffersSentInInterval++;

      //keep track of data sent to device.
      xferInfo.nLastTimestampSent=audioBuffer.nLastTimestamp;
      xferInfo.nLastFrameSent=audioBuffer.nLastFrame;

      //keep track of elapsed time info.
      elapsed.nLastTimestamp=audioBuffer.nLastTimestamp;

      //Set AV timing on each buffer.  Update AV timing using the end
      //of buffer point in order to correct for any discontinuities.
      //sateesh: commented here
     //SetAVTiming(tPlayBegin,audioBuffer.nFirstTimestamp,audioBuffer.nFirstFrame);

      if(((int)Media::WMA_CODEC != audioCodec) &&
         (!IS_GENERIC_AUDIO_FORMAT(audioCodec)))
      {
       //sateesh: commented here
        // UpdateAVTiming(audioBuffer.nLastTimestamp,audioBuffer.nLastFrame);
      }

#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

      //clear the data buffer.
      audioBuffer.nData=0;
      audioBuffer.nSize=0;

      //keep track of max processing time
      QCUtils::EnterCritSect(&audioStats.CS);
      if ( tProc > audioStats.nMaxProcTime )
      {
        audioStats.nMaxProcTime=tProc;
      }
      QCUtils::LeaveCritSect(&audioStats.CS);
    }
    else
    {
      //Audio was not sent.
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW, "Info:No audio sent on polling: nWaitMsec(%d)",nWaitMsec);
      /*We reached media end so in generic audio formats
       it is not possible to do the repositioning so don't
       allow repositioning */
      if( mediaInfo.dataState==Media::DATA_END)
      {
        pMpeg4->setRepositioningAllowed(false);
      }
      //Make sure device doesn't wait forever without
      //sending any data.
      if ( (xferInfo.nBuffersSentInInterval==0) && (nWaitMsec>0) && (mediaInfo.dataState!=Media::DATA_END) )
      {
        long n = ZUtils::Clock()-xferInfo.tStartInterval;
        if ( n > pAudioCMX->nAudioIFMaxBehindMsec )
        {
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "Error, Audio idle for %d",n);
          bError=true;
        }
      }
    }
  }
  else
  {
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio must wait: %dms", nWaitMsec);
  }
#ifndef FEATURE_QTV_GENERIC_BCAST
  if ( (Media::BCAST_FLO_SOURCE == pMpeg4->GetSource() ||
        Media::BCAST_ISDB_SOURCE == pMpeg4->GetSource()) &&
#else

  /* Audio hardware based uner-run concept is equally apply to
   * file based Bcast play.
   */
  if (  IsLiveBcastMediaSource(pMpeg4) &&
#endif
       (Media::DATA_UNDERRUN == mediaInfo.dataState) )
  {
    /*-------------------------------------------------------------------
      We don't want to allow audio underruns, so force nWaitMSec to a
      positive value, faking that we need to poll CMX later.  Audio
      underruns will be signalled instead by CMX when it gives us a
      "data delay" msg which indicates a HW codec underrun.
    -------------------------------------------------------------------*/
    if (nWaitMsec < COMMON_BUFFERING_POLL_TIME_MSEC)
    {
      nWaitMsec = COMMON_BUFFERING_POLL_TIME_MSEC;
    }

    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED,
              "Audio media underrun, sleeping for %dms",
              COMMON_BUFFERING_POLL_TIME_MSEC);

    /*-------------------------------------------------------------------
      Media audio underruns shall never be passed up to the audio thread.
      Override it so that the audio thread can recover.
    -------------------------------------------------------------------*/
    mediaInfo.dataState = Media::DATA_OK;
  }

  //Set data state.
  dataStateOut=mediaInfo.dataState;

#ifdef SHOW_STARVE
  //make sure sleep time isn't contributing the starvation.
  if ( !bSent && !bError && nWaitMsec>0 )
  {
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio wait %d",nWaitMsec);
  }
#endif

  if((mediaInfo.dataState == Media::DATA_FRAGMENT) && (bSent))
      return false;
  else
      return bSent;
}

/* ======================================================================
FUNCTION
  genericAudioMgr::DataIsAvailable

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
void genericAudioMgr::DataIsAvailable()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "genericAudioMgr::DataIsAvailable");
  // set data state to allow reading data.
  mediaInfo.dataState = Media::DATA_OK;
#ifdef SHOW_TIMING
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "genericAudioMgr::DataIsAvailable %d",ZUtils::Clock());
#endif
}

/* ======================================================================
FUNCTION
  AudioMgr::FillAudioBuffer

DESCRIPTION

  Fill the audio buffer with data
  Set dataState when changed.
  Return true if there's data.


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool genericAudioMgr::FillAudioBuffer(const uint32 max_len, const uint32 offset)
{
#ifdef FEATURE_QTV_GENERIC_AUDIO_DEBUGGING
#error code not present
#endif /* FEATURE_QTV_GENERIC_AUDIO_DEBUGGING */

#if defined (FEATURE_QTV_CMX_AV_SYNC_BYTES) && defined (FEATURE_QTV_WINDOWS_MEDIA)
   int outDataSize = 0;
#endif /* (FEATURE_QTV_CMX_AV_SYNC_BYTES) && (FEATURE_QTV_WINDOWS_MEDIA) */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
//   long VirtualPacketSize=0;
#endif

  uint32 offset_to_read = offset;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "AudioMgr::FillAudioBuffer");

#ifdef FEATURE_QTV_GENERIC_AUDIO_DEBUGGING
#error code not present
#endif /* FEATURE_QTV_GENERIC_AUDIO_DEBUGGING */
/* currently there is bug in cmx code which will ask twice the firrst chunk
   of data. so we have to emove this check once they checked in their fixes.
   here checking with huge nuber because the dummy request which they are
   asking with negative offset */
  if( (int32) offset_to_read < 0 )
  {
    return true;
  }
  audioBuffer.nData=0;
  audioBuffer.nSize=0;

  //don't exceed the number of bytes requested
  //or the actual buffer size.
  int nLimit = ZMIN(audioBuffer.nMaxData, (int)max_len);

#ifdef FEATURE_QTV_WINDOWS_MEDIA
  if( ((int)Media::WMA_CODEC == GetAudioCodecType()) && (!(isStartOfDummyBytes())) )
  {
//    VirtualPacketSize=
    GetVirtualPacketSize();

    /*
    Try to send as many packets as possible in allowed MAX limit while answering first data request.
    For some OEMs,when we report PLAYING, some fancy stuff
    in UI etc. might now give enough CPU to send subsequent packets which might starve the DSP.
    */

    //sateesh
     /*if(!m_bIsFirstData)
    nLimit = ZMIN(2* VirtualPacketSize, nLimit);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Adjusted nLimit: %d",nLimit);
    m_bIsFirstData = false;       */
  }
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

  // Its possible that the Audio Decoder's buffer may be smaller (1600) than the largest
  // possible AudioFrameSize (2048) for example.
  int nLargestFrameSize = ZMIN(audioInfo.nMaxBytesPerFrame, (int)max_len);

  if ( pMpeg4 && pAudioCMX )
  {
    if ( audioBuffer.pData )
    {
      //Add samples to the buffer
      int  nBytes = 0;
      int numSample = 1;
      Media::MediaStatus status = Media::DATA_NULLFILL;
      long nTimestampMsec = 0;
#if defined (FEATURE_QTV_WINDOWS_MEDIA) && defined (FEATURE_QTV_CMX_AV_SYNC_BYTES)
      Media::CodecType audioCodec = GetAudioCodecType();
#endif /* (FEATURE_QTV_WINDOWS_MEDIA) && (FEATURE_QTV_CMX_AV_SYNC_BYTES) */

      while (
            //while there's data
            (mediaInfo.dataState == Media::DATA_OK)
            //and sample count not exceeded
            && (audioBuffer.nSize < audioBuffer.nMaxSize)
            //and there's room for one more maximum size sample
            && ((audioBuffer.nData + nLargestFrameSize) <= nLimit)
            && (audioBuffer.nData < nLimit)
            )
      {
        //Get audio sample
        numSample = 1;

        // QTV_MSG( QTVDIAG_AUDIO_TASK, "B %d BMax %d FMB: %d #Sz %d Lim %d",
        // audioBuffer.nData, audioBuffer.nMaxSize,
        // audioInfo.nMaxBytesPerFrame, audioBuffer.nSize, nLimit);

        //WZeng, add "int channel" to support aac silence insertion when streaming, 06/05/02
        cmx_af_aac_spec_type *pCmx_af_aac_spec;
        pCmx_af_aac_spec = (cmx_af_aac_spec_type *) ((AudioCMX*)pAudioCMX)->GetCmx_af_aac_spec();

        // Change Logic to put 1600 bytes even in each audio buffer.
        // i.e. Not Frame Aligned
        if ( audioBuffer.nPendingData )
        {
          unsigned char * pStart = audioBuffer.pPendingData + audioBuffer.nPendingOffset;
          int bytesToCopy = MIN(audioBuffer.nPendingData, nLimit);
          QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Base %X Offset %X %d Bytes", audioBuffer.pData, audioBuffer.pData + audioBuffer.nData, audioBuffer.nPendingData);
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "Grabbed %d Pending Bytes", bytesToCopy);

          memcpy( (audioBuffer.pData + audioBuffer.nData), pStart, bytesToCopy);
          nBytes = bytesToCopy;
          status = Media::DATA_OK;
          if(audioBuffer.nPendingData)
            audioBuffer.nPartialFrame++;
          audioBuffer.nPendingData = audioBuffer.nPendingData - bytesToCopy;
          audioBuffer.nPendingOffset = audioBuffer.nPendingOffset + bytesToCopy;
        }
        else
        {
#ifdef FEATURE_QTV_WINDOWS_MEDIA

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
           //Don't add data back to restore data buffer while restoring as it already
          //exists in therestore buffer. It will be flushed out when feedback comes in from DSP.
          bool bDataBeingRestore = false;
          if(
              (IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))  &&
              (RestoreDataBufferInfo.nEntriesToRestore > 0) &&
              (RestoreDataBufferInfo.nTmpReadIndex >= 0)    &&
              (RestoreDataBufferInfo.bRestoreBufferValid) )
          {
            bDataBeingRestore = true;
            if(RestoreDataBufferInfo.nTmpReadIndex >= NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec])
            {
              RestoreDataBufferInfo.nTmpReadIndex = 0;
            }
            if( audioBuffer.pPendingData )
             {
               //Retrieve the data
               numSample = RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nTmpReadIndex].nFrames;
               outDataSize = nBytes = RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nTmpReadIndex].nSize;
               status = Media::DATA_OK;
              if(outDataSize <= audioInfo.nMaxBytesPerFrame)
              {
                 memcpy(audioBuffer.pPendingData,
                       RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nTmpReadIndex].pDataPtr,
                       RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nTmpReadIndex].nSize);
                 nTimestampMsec = RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nTmpReadIndex].nPresTime;
                 RestoreDataBufferInfo.nTmpReadIndex++;
                 QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,
                              QTVDIAG_PRIO_HIGH,
                              "Grabbed prv. DATA PACKET drom DataBuffer with TS %ld", nTimestampMsec);
              }
              else
              {
                /*
                * If outDataSize is > audioInfo.nMaxBytesPerFrame, something is wrong/screwd up already.
                * Print a FATAL message and stop restoring data by resetting restore data buffer.
                */
                QTV_MSG_PRIO2(QTVDIAG_AUDIO_TASK,
                              QTVDIAG_PRIO_FATAL,
                              "Resetting restore info: outDataSize(%d) > audioInfo.nMaxBytesPerFrame(%d)",
                              outDataSize,audioInfo.nMaxBytesPerFrame);
                InitAVSyncAudioBuffer(true);
                bDataBeingRestore = false;
              }
             }
           }
           if(!bDataBeingRestore)
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
          /////////////////////////////////////////////////////////////////////////
          {
              nBytes = pMpeg4->GetNextGenericAudioSample(status,
                                                    audioBuffer.pPendingData,
                                                    audioInfo.nMaxBytesPerFrame,
                                                    offset_to_read,
                                                    &numSample,
                                                    NULL,
                                                    (int) pCmx_af_aac_spec->channel);
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
                        outDataSize = nBytes;
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
          }
#else
          nBytes = pMpeg4->GetNextGenericAudioSample(status,
                                                 audioBuffer.pPendingData,
                                                 audioInfo.nMaxBytesPerFrame,
                                                 offset_to_read,
                                                 &numSample,
                                                 NULL,
                                                 (int) pCmx_af_aac_spec->channel);
#endif
          if(nBytes)
          {
            offset_to_read = offset_to_read + nBytes;
          }

          /* some time we get NO-DATA frames, so just skip that */
          if ( (nBytes==0) && (status==Media::DATA_OK) )
          {
            /* this is NO-DATA frame */
            continue;
          }

            // Partial Frame, nBytes modified to bytes added
            audioBuffer.nPendingData = 0;
            audioBuffer.nPendingOffset =0;

            QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Base %X Offset %X %d Bytes", audioBuffer.pData, audioBuffer.pData + audioBuffer.nData, nBytes);
            if (audioBuffer.pPendingData)
            {
              memcpy( (audioBuffer.pData + audioBuffer.nData), audioBuffer.pPendingData, nBytes);
            }
#if 0
            QTV_MSG1( QTVDIAG_AUDIO_TASK, "Pending %d Bytes", audioBuffer.nPendingData);
#endif
          }

        // QTV_MSG( QTVDIAG_AUDIO_TASK, "FillAudioBuffer nBytes %d %d status %d", nBytes, audioBuffer.nData + nBytes, status);

        //Check status
        switch ( status )
        {
        case Media::DATA_OK:
        case Media::DATA_NULLFILL:
          //We got a sample.

          bool bAddSample ;
          bAddSample=false;
          mediaInfo.dataState = status;

           //If we are in the process of restoring data from call data buffer,
           //don't pull TS from media as it is already obtained above.Otherwise,
          //Get timestamp in media timescale units.
          #ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
          if(RestoreDataBufferInfo.nEntriesToRestore == 0)
          #endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
          nTimestampMsec = pMpeg4->GetTimestampForCurrentAudioSample();

        //If EOS is set in MediaBuffer and if we are restoring from CallDataBuffer
        //don't check error via pMpeg4->CheckError as it will be PV_STREAM_NETWORK_ERROR in
        //StreamMedia. It gets changed to PV_STREAM_SUCESS only when we pull data from MediaBuffer.
        //This will happen when are done with restoring and invoke GetNextAudioSample above.
        //If we don't do this, we won't be able to resume after voice call for WM.
        //For 3GP, we don't maintain extra data buffer so it works correctly.

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
   if( (RestoreDataBufferInfo.nEntriesToRestore > 0)||
       ((nBytes>0) && !pMpeg4->CheckError(Common::GETAUDIOTIME)) )
#else
          if ( (nBytes>0) && !pMpeg4->CheckError(Common::GETAUDIOTIME) )
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
          {
            //convert timestamp to msec.
            long convertedTS = pMpeg4->ConvertAudioTimestamp(nTimestampMsec);
#ifdef SHOW_TIMING
            QTV_MSG3( QTVDIAG_AUDIO_TASK, "Audio sample f %d rtp_ts %ld converted to %ld ",
               mediaInfo.nFramesProcessed+1, nTimestampMsec, convertedTS );
#endif

            if((pMpeg4->GetSource() == Media::BCAST_FLO_SOURCE) &&
               (CheckForChannelSwitchFrame == TRUE) &&
               (nTimestampMsec == ChannelSwitchedTS))
            {
              QTV_MSG1( QTVDIAG_AUDIO_TASK, "  Received first frame after ChannelSwitch = %d",convertedTS);
              AudioChannelSwitchedTS = convertedTS;
            }
            nTimestampMsec = convertedTS;

            //check for continuity.
            if ( nTimestampMsec < audioBuffer.nLastTimestamp )
            {
              QTV_MSG3( QTVDIAG_AUDIO_TASK, "WARNING weird audio TS seq. %d %d, Media TS %d",
                        audioBuffer.nLastTimestamp,
                        nTimestampMsec,
                        pMpeg4->GetTimestampForCurrentAudioSample());
            }

            //See if the stop time was reached.
            if ( stopTime.bStop && (unsigned long)nTimestampMsec>stopTime.nStop )
            {
              //when stop time is reached, act like it's the end
              //of the data.
              mediaInfo.dataState=Media::DATA_END;
            }
            //see if this sample is before the current start time
            else if ( nStartTime>=0 && nTimestampMsec<nStartTime )
            {
              //keep stats on this but add the frame anyway.
              //I would discard it but then there's a risk of
              //CPU hogging in this routine.  With our FF and QtvStream
              //the audio position will always be what we want anyway.
              audioStats.nFramesDiscarded++;
              bAddSample=true;
            }
            else
            {
              bAddSample=true;
            }
          }
          else
          {
            //unexpected error.
            mediaInfo.dataState=Media::DATA_ERROR;
          }

          if ( bAddSample )
          {
            //add this sample to the buffer
            audioBuffer.nData += nBytes;

            //save the sample size
            if ( audioBuffer.pSize )
            {
              audioBuffer.pSize[audioBuffer.nSize]=(unsigned char)nBytes;
            }

            //count samples put in the buffer.
            audioBuffer.nSize++;

            if ( ! audioBuffer.nPartialFrame )
            {
              //count total frames
              mediaInfo.nFramesProcessed = mediaInfo.nFramesProcessed + numSample;
            }
            else
              audioBuffer.nPartialFrame= 0;

            //keep track of data in buffer.
            if ( audioBuffer.nSize==1 )
            {
              audioBuffer.nFirstFrame = mediaInfo.nFramesProcessed-numSample+1;
              audioBuffer.nFirstTimestamp = nTimestampMsec;
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
              if( (m_bCurrentTSFromCMXBytesUpdatePending)&&
             ((int)Media::WMA_CODEC == GetAudioCodecType()) )
              {
                m_nCurrentTSFromCMXBytes = audioBuffer.nFirstTimestamp;
                m_bCurrentTSFromCMXBytesUpdatePending = false;
              }
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
            }

            audioBuffer.nLastFrame = mediaInfo.nFramesProcessed;

#if defined(FEATURE_QTV_CMX_AV_SYNC_BYTES) && defined(FEATURE_QTV_WINDOWS_MEDIA)
              if( ((int)Media::WMA_CODEC == GetAudioCodecType())&& (isStartOfDummyBytes()) )
              {
                //Don't add entry in mapping table if it's a start of dummy bytes.
                //This is because DSP may not request all dummy bytes,then as we
                //calculate amount of data buffered based on number of entries in mapping table
                //we will think that there is some data buffered even when audio is ended.
                //This will cause jump in the progress bar timer towards the end of the clip.
                outDataSize = 0;
              }
#endif /* (FEATURE_QTV_CMX_AV_SYNC_BYTES) && (FEATURE_QTV_WINDOWS_MEDIA) */

#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
            if( (int)Media::WMA_CODEC == GetAudioCodecType() )
            {
            #ifdef FEATURE_QTV_WINDOWS_MEDIA
              if(outDataSize)
              {
                /*
                *Storing timestamps and byteoffsets for CMX AVSync Callback
                 *Even if we are in the process of restoring data from restore data buffer,
                 *add it to the AV SYNC buffer as it gets flushed out.
                */

                nAVSyncAudioWriteByteOffSet += outDataSize;
                AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nByteOffSet  = nAVSyncAudioWriteByteOffSet;
                AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nTimeStamp   = nTimestampMsec;
                QTV_MSG_PRIO2(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "added entry in mapping_table, Bytes %ld  TS %ld",AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nByteOffSet,AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nTimeStamp);

                //Don't add packet into restore buffer if RestoreDataBufferInfo.nEntriesToRestore > 0,
                 //which means we are restoring existing data from the buffer.

                //make sure we are not over-writing
                 if( (RestoreDataBufferInfo.bRestoreBufferValid)&&
                    (!RestoreDataBufferInfo.nEntriesToRestore) &&
                    ( outDataSize < (2 * GetVirtualPacketSize())) )
                {

                    if(RestoreDataBufferInfo.nWriteIndex == NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec])
                  {
                      RestoreDataBufferInfo.nWriteIndex = 0;
                  }

                    RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].nSize = outDataSize;
                    RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].nFrames = numSample;
                   RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].nPresTime = nTimestampMsec;
                    memcpy(RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].pDataPtr,
                         audioBuffer.pPendingData,
                         outDataSize);
                    RestoreDataBufferInfo.nWriteIndex++;
                    RestoreDataBufferInfo.nEntriesInRestoreBuffer++;

                    if(RestoreDataBufferInfo.nEntriesInRestoreBuffer > NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec])
                  {
                    RestoreDataBufferInfo.nEntriesInRestoreBuffer = NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec];
                    RestoreDataBufferInfo.nReadIndex++;
                    if(RestoreDataBufferInfo.nReadIndex > NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec])
                    {
                      RestoreDataBufferInfo.nReadIndex = 0;
                    }
                    QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,
                                  QTVDIAG_PRIO_FATAL,
                                  "OVERFLOW RestoreDataBufferInfo.nEntriesInRestoreBuffer %ld",
                                  (RestoreDataBufferInfo.nEntriesInRestoreBuffer-NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec]));
                  }
                }

                if((++nAVSyncAudioBufferWriteIndex) >= MAX_AVSYNC_AUDIO_BUFFER_SIZE)
                nAVSyncAudioBufferWriteIndex = 0;

                  if( RestoreDataBufferInfo.nEntriesToRestore > 0 )
                {
                  RestoreDataBufferInfo.nEntriesToRestore--;
                  }

                if(nAVSyncAudioBufferSize < MAX_AVSYNC_AUDIO_BUFFER_SIZE)
                {
                  nAVSyncAudioBufferSize++;
                  outDataSize = 0;
                }
                else
                {
                  QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "AVSyncAudioBuffer overflow at timestamp: %d",audioBuffer.nLastTimestamp);
                }
              }//end of  if(outDataSize)
            #endif//ifdef FEATURE_QTV_WINDOWS_MEDIA
             }//end of if( Media::WMA_CODEC == GetAudioCodecType() )
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */

            //Update timestamp if there are more than 1 sample/frames based on frames per msec.
            if(numSample>1)
              audioBuffer.nLastTimestamp = (long)(nTimestampMsec + (numSample-1)/clipInfo.fFramesPerMsec);
            else
            audioBuffer.nLastTimestamp = nTimestampMsec;

            //count total null samples
            if ( status==Media::DATA_NULLFILL )
            {
              QCUtils::EnterCritSect(&audioStats.CS);
              audioStats.nNullFrames++;
              audioStats.nCurrentNullChunk++;
              audioStats.nMaxNullChunk=ZMAX(audioStats.nMaxNullChunk,audioStats.nCurrentNullChunk);
              QCUtils::LeaveCritSect(&audioStats.CS);
            }
            else
            {
              QCUtils::EnterCritSect(&audioStats.CS);
              audioStats.nCurrentNullChunk=0;
              QCUtils::LeaveCritSect(&audioStats.CS);
            }
          }
          break;

        case Media::DATA_REALLOCATE_BUFFER:
           //look at the error detail
          (void)pMpeg4->CheckError(Common::GETAUDIO);
          uint32 bufferSize;
          bufferSize = 0;
          bufferSize = pMpeg4->GetMaxAudioBufferSizeDB();

          if(bufferSize > (uint32)audioInfo.nMaxBytesPerFrame)
          {
            audioInfo.nMaxBytesPerFrame = bufferSize;
            audioBuffer.nMaxData = clipInfo.nFramesPerBuffer * audioInfo.nMaxBytesPerFrame;

            audioBuffer.pData = (unsigned char *)QTV_Realloc(audioBuffer.pData,audioBuffer.nMaxData);
            if(audioBuffer.pData)
            {
                audioBuffer.pPendingData = (unsigned char *)QTV_Realloc(audioBuffer.pPendingData,
                                                                    audioInfo.nMaxBytesPerFrame);
                if(!audioBuffer.pPendingData)
                {
                    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Can't re-allocate data buffer of size %d",
                              audioInfo.nMaxBytesPerFrame);
                }
            }
            else
            {
                QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Can't re-allocate data buffer of size %d",audioBuffer.nMaxData);
            }
            if(!audioBuffer.pData || !audioBuffer.pPendingData)
            {
               mediaInfo.dataState = Media::DATA_ERROR;
               Abort();
            }
            else
            {
                mediaInfo.dataState = Media::DATA_OK;
            }
          }
          break;

        case Media::DATA_ERROR:
          //look at the error detail
          (void)pMpeg4->CheckError(Common::GETAUDIO);
          mediaInfo.dataState = status;
          break;

        case Media::DATA_SKIP:
        case Media::DATA_UNDERRUN:
        case Media::DATA_END:
        case Media::DATA_INSUFFICIENT:
          mediaInfo.dataState = status;
          break;

        default:
          //unexpected error.
          mediaInfo.dataState = Media::DATA_ERROR;
          break;
        }
#ifdef SHOW_TIMING
        QTV_MSG4( QTVDIAG_AUDIO_TASK, "   #FrmTotal %d  TS %d  #Frm %d bPar %d",
                  mediaInfo.nFramesProcessed, nTimestampMsec, audioBuffer.nSize, audioBuffer.nPartialFrame );
#endif

      }
    }
  }

#ifdef SHOW_FRAMES
  if ( audioBuffer.nData>0 )
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "Aud Frame %d ts %d",audioBuffer.nFirstFrame,audioBuffer.nFirstTimestamp);
#endif

#if 0
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "FillAudioBuffer Size %d",audioBuffer.nData);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "Data state %d",mediaInfo.dataState);
#endif

  return(audioBuffer.nData>0);
}

/* ======================================================================
FUNCTION
  AudioMgr::Resume

DESCRIPTION
//
// resume audio (assuming it's currently paused).

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool genericAudioMgr::Resume(bool bRestart)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "genericAudioMgr::Resume %d", bRestart);

  bool bOK;
  long tResume;

  if (!bRestart)
  {
    //New play interval
    xferInfo.nBuffersSentInInterval = 0;
    xferInfo.bTimingUpdatedInInterval=false;
    xferInfo.tStartInterval=ZUtils::Clock();

    //This interval starts where the last one ended.
    xferInfo.startOfInterval = xferInfo.endOfInterval;
    xferInfo.endOfInterval.bValid = false;
  }
  else
  {
   elapsedTimeForGenericAudio = pMpeg4->GetTimeStampFromMedia();
     /* Resume after a reposition */
    InitAVTiming();
#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
    if((int)Media::WMA_CODEC == GetAudioCodecType())
    {
      InitAVSyncAudioBuffer(bRestart);
      m_bCurrentTSFromCMXBytesUpdatePending = true;
    }
#endif
  }

  if(!pAudioCMX)
    return false;

  //Resume
  if ( xferInfo.startOfInterval.bValid )
  {
    //Normal case, resume from paused frame.
    bOK = pAudioCMX->Resume(tResume);

    if ( bOK )
    {
      if ( tResume>=0 )
      {
        //We know the device resume time.
        //Update start of interval time & set AV timing.

        xferInfo.startOfInterval.time = tResume;

#ifdef SHOW_TIMING
        QTV_MSG2( QTVDIAG_AUDIO_TASK, "Audio resumed: time=%u,ts=%u",
           tResume,
           xferInfo.startOfInterval.timestamp );
#endif

        SetPlaybackOffset(
                         xferInfo.startOfInterval.time,
                         xferInfo.startOfInterval.timestamp);
      }
      else
      {
        //Resume time is not known.  reset start of interval
        //so AV timing will get set when we start sending data.
        xferInfo.startOfInterval.bValid=false;
      }
    }
  }
  else
  {
    /* Resume after a reposition */
    bOK = pAudioCMX->Resume(tResume);
  }

  if ( bOK )
  {
    /* Here for the generic audio formats changing mediaInfo.dataState to Media::DATA_OK
    from what ever the status may be because, there is corner case, if we reached media
    end and then if we do pause/resume or pause/reposition then we are failing to send
    data to CMX after start of CMX and this leads to Ab_error */
    mediaInfo.dataState = Media::DATA_OK;
    StartElapsedTime();
  }

  /* In audio.cpp we are faking RESUME is successful when try to start/resume
     the audio, if the Media DATA_STATE is DATA_END. So we need to set the
     m_bMediaInitialized so that media won't wait on any CMX events.
     This will help to end the audio.
  */
  if( !bOK )
  {
    if(pMpeg4)
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "calling pMpeg4->SetAudioBitRate with %ld", audioInfo.nBitRate);
      pMpeg4->SetAudioBitRate(audioInfo.nBitRate);
      /* m_faked_resume this flag to help in ::prep not to reset m_bMediaInitialized
        even  though the Media DATA_STATE is DATA_END.
      */
      m_faked_resume = true;
    }
  }
  return bOK;
}


/* ======================================================================
FUNCTION
  genericAudioMgr::genericAudioMgr

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
genericAudioMgr::genericAudioMgr() : AudioMgr(NULL)
{
  m_endDelay = AUDIO_END_DELAY;
  m_faked_resume = false;
}

/* ======================================================================
FUNCTION
  genericAudioMgr::~genericAudioMgr

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
genericAudioMgr::~genericAudioMgr()
{
}

#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
