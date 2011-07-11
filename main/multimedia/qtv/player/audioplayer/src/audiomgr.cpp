/* =======================================================================
                               audiomgr.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audiomgr.cpp#61 $
$DateTime: 2011/01/27 02:57:36 $
$Change: 1596592 $

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
#include "audioMgr.h"
#include "audiocmx.h"
#include "qtv_log.h"
#include "qtv_event.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Current(Dec 2004) AAC/AAC+ firmware supports only 1024 samples per frame.*/
#define QTV_AAC_SAMPLES_PER_FRAME 1024
// debug support
//#define SHOW_AUDIO_INFO
#define SHOW_TIMING
//#define SHOW_TIMING2
//#define SHOW_TIMING3
//#define SHOW_TIMING_LITE
//#define SHOW_FRAMES
#define SHOW_SYNC
//#define SHOW_OFFSET
//#define SHOW_DELTA
//#define SHOW_STARVE

#define CMX_DEFAULT_BUFFER_SIZE 2560

qtv_event_Audiochannelswitch_frame Audioeventpayload;

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
    extern "C" boolean qtv_cfg_enable_dsp_release;
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC
    // Currently max bytes per frame supported for AAC clip in CMX firmware is 1546 bytes.
    // We make it a bit more, 2048
    #define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_AAC  2048
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC
/**
   If required we can change average value of frame size in bytes
   based on testing performance 
*/ 
  #define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_EVRC  2048 
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
   /**
   If required we can change average value of frame size in bytes
   based on testing performance
   */
#define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_MP3  8192
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3 */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP
/**
   If required we can change average value of frame size in bytes
   based on testing performance
*/
    #define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_QCELP  2048
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR
    #define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_AMR  320
#endif
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
    #define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_WMA_PRO    512// WMA_PRO_CODEC,
    #define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_WMA_PRO_PLUS   512// WMA_PRO_PLUS_CODEC,
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3
   //Sateesh: Need to check with CMX folks before checking in
   // This calculation is based on 6144 * 20
   #define MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_AC3  122880
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3 */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM */

int const AudioMgr::NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[] =
{
  0,	                                            // UNKNOWN_CODEC,
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC
  EVRC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER, // EVRC_CODEC,
#else
  0, 
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB
#error code not present
#else
  0,
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB  */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB
#error code not present
#else
  0,
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB  */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP
  QCELP_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER, // QCELP_CODEC,
#else
  0,
#endif                                              // QCELP_CODEC,
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC
  AAC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER,  // AAC_CODEC,
#else
  0,
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR
  AMR_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER, // GSM_AMR_CODEC,
#else
  0,
#endif
  0,                                              // MPEG4_CODEC,
  0,                                              // H263_CODEC,
  0,                                              // H264_CODEC,
  0,                                              // STILL_IMAGE_CODEC,
  0,                                              // TIMED_TEXT_CODEC,
  0,                                                // GENERIC_TEXT_CODEC,
  0,                                              // JPEG_CODEC,
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
  MP3_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER,     // MP3_CODEC,
#else
  0,
#endif
#ifdef FEATURE_QTV_WINDOWS_MEDIA
  WMA_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER,  // WMA_CODEC,
#else
  0,
#endif
  0,                                              // WMV1_CODEC,
  0,                                              // WMV2_CODEC,
  0,                                              // WMV3_CODEC,
#if defined (FEATURE_QTV_WMA_PRO_DECODER)
  WMA_PRO_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER,  // WMA_PRO_CODEC,
#elif defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
  WMA_PRO_DSP_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER, // WMA_PRO_CODEC DSP based
#else
  0,
#endif
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
  WMA_PRO_PLUS_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER, // WMA_PRO_PLUS_CODEC,
#else
  0,
#endif // (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
  0,                                              // OSCAR_CODEC,
  0,                                              // CONC_CODEC, // audio codec of concurrent image not available for Qtv
  0,                                              // COOK_CODEC,
  0,                                              // SIPR_CODEC,
  0,                                              // RV30_CODEC,
  0,                                                // RV40_CODEC,
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB
#error code not present
#else
  0,
  0,
#endif
  0,                                                //NONMP4_MP3_CODEC,
  0,                                                //MIDI_CODEC,
  0,                                                //QCP_CODEC,
  0,                                                //NONMP4_AAC_CODEC,
  0,                                                //NONMP4_AMR_CODEC,
  0,                                                //DIVX311_CODEC,
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC
  AAC_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER,  // BSAC_CODEC,
#else
  0,
#endif
  0,                                                 //FGV_CODEC_NB,
  0,                                                 //FGV_CODEC_WB,
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3
#error code not present
#else
  0,
#endif  /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3  */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM
#error code not present
#else
  0,
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM */
  0                                                 //MAX_CODEC
};
#endif

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
  AudioMgr::CleanupPlay

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
void AudioMgr::CleanupPlay()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::CleanupPlay");

  //delete audio interface.
  DestroyAudioInterface();

#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif /* FEATURE_QTV_ADTS_PARSER */

  //Member 'pMeg4' could be accessed in the Video task context, therefore
  //guard this section
  QCUtils::EnterCritSect(&audioMgrCS);
  pMpeg4 = NULL;
  QCUtils::LeaveCritSect(&audioMgrCS);

  //free the allocated storage
  if ( audioBuffer.pData )
  {
    QTV_Free(audioBuffer.pData);
    audioBuffer.pData = NULL;
  }

  if ( audioBuffer.pPendingData )
  {
    QTV_Free(audioBuffer.pPendingData);
    audioBuffer.pPendingData = NULL;
  }

  if ( audioBuffer.pSize )
  {
    QTV_Free(audioBuffer.pSize);
    audioBuffer.pSize = NULL;
  }

  audioBuffer.nPendingData   = 0;
  audioBuffer.nPendingOffset = 0;
  audioBuffer.nPartialFrame  = 0;
}

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
void AudioMgr::DestroyAudioInterface()
{
  if ( pAudioCMX )
  {
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
    bFirstEntryPlayedOut = false;
    firstEntryOffset = 0;
#endif
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
bool AudioMgr::CreateAudioInterface(Media::CodecType codec)
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
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
  case Media::MP3_CODEC:
#ifdef FEATURE_MP4_AMR
  case Media::GSM_AMR_CODEC:
#endif /* FEATURE_MP4_AMR */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
  case Media::WMA_CODEC:
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
  case Media::WMA_PRO_CODEC:
  case Media::WMA_PRO_PLUS_CODEC:
#endif /* #if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
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
#ifdef FEATURE_QTV_AVI_AC3
#error code not present
#endif /* FEATURE_QTV_AVI_AC3 */
#ifdef FEATURE_QTV_PCM
#error code not present
#endif /* FEATURE_QTV_PCM */
    pAudioCMX = QTV_New_Args( AudioCMX, (codec) );
    break;
  }
  //Call the init function for the audio interface.
  if ( pAudioCMX )
  {
    pAudioCMX->Create( (void *)this, notifyFunc, m_pClientData );
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
    cmx_avsync_update.bCmxFlushed = true;
#endif
    return true;
  }

  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::CreateAudioInterface failed");
  return false;
}

/* ======================================================================
FUNCTION
  AudioMgr::Prep

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
bool AudioMgr::Prep(
                   Media * pMpeg4In,
                   long nTimingUpdateMsec,
                   bool bRestart)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::Prep %d", bRestart);

#ifdef SHOW_TIMING
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::StartPlaying entry %d",ZUtils::Clock());
#endif


  if ( !bRestart )
  {
    //save media pointer.
    pMpeg4     = pMpeg4In;
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_GENERIC_BCAST_PCR) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_GENERIC_BCAST_PCR ||
          FEATURE_QTV_IN_CALL_VIDEO */
    if ( pMpeg4==NULL )
    {
      return false;
    }



    pMpeg4->SetAudioPlayerData(playerData);


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
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "doing pMpeg4->resetInitialization()");
    //byte based feedback dependency check for FAST Audio Playback
    #ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
    Media::CodecType audioCodec = pMpeg4->GetAudioCodecType();
    if(!IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
    #endif
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK, "Byte Based AV Sync not enabled ;FAP Ignored");
      StopFastAudioPlayback();
    }
    pMpeg4->resetInitialization();

    //WZeng, 05/22/02
    // Decode audioSpecificConf information for aac
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
    if ( pMpeg4->GetAudioCodecType() == Media::AAC_CODEC )
#endif /* FEATURE_QTV_BSAC */
    {
      cmx_af_aac_spec_type *pCmx_af_aac_spec = NULL;

      QTV_MSG( QTVDIAG_AUDIO_TASK,  "PrepPlay Decode AudSpcCfg");
      if (pAudioCMX)
      {
      pCmx_af_aac_spec = (cmx_af_aac_spec_type *) ((AudioCMX*)pAudioCMX)->GetCmx_af_aac_spec();
      }
      int retVal = GetAndDecodeAudioSpecificConfig(pCmx_af_aac_spec);
      if ( retVal )
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PrepPlay FAILED Decode AudSpcCfg");
        return false;
      }
    }
    //end, WZeng, 05/22/02

    int mediaDuration = pMpeg4->GetAudioDurationMsec();
    audioInfo.nDurationMsec = (mediaDuration > 0) ? (long)mediaDuration : 0;

    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nMaxBytesPerFrame = pMpeg4->GetMaxAudioBufferSizeDB();
    QTV_MSG1( QTVDIAG_AUDIO_TASK,  "MaxBytesPerFrame %d", audioInfo.nMaxBytesPerFrame );
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nSamplingRate = pMpeg4->GetAudioSamplingFreq();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nSamplesPerFrame = pMpeg4->GetAudioSamplesPerFrame();
    QTV_MSG1( QTVDIAG_AUDIO_TASK,  "Samples per frame %ld", audioInfo.nSamplesPerFrame );

#ifdef FEATURE_QTV_BSAC
#error code not present
#else
    if (pMpeg4->GetAudioCodecType() == Media::AAC_CODEC)
#endif /* FEATURE_QTV_BSAC */
    {
       if(QTV_AAC_SAMPLES_PER_FRAME != audioInfo.nSamplesPerFrame)
       {
         /* Current(Dec 2004) AAC/AAC+ firmware supports only 1024 samples per frame.*/
#ifndef FEATURE_WINCE
            QTV_MSG1( QTVDIAG_AUDIO_TASK,
                      "Qtv does not support AAC %ld Samples per frame."
                      "Using 1024 samples per frame",
                       audioInfo.nSamplesPerFrame );
#endif
          audioInfo.nSamplesPerFrame = QTV_AAC_SAMPLES_PER_FRAME;
       }
    }

    audioInfo.nBitsPerSample = pMpeg4->GetAudioBitsPerSample();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

#if defined (FEATURE_QTV_WINDOWS_MEDIA) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)

    audioInfo.nChannels = pMpeg4->GetNumAudioChannels();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nVersion = pMpeg4->GetAudioCodecVersion();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;

    audioInfo.nAsfPacketSize = pMpeg4->GetFixedAsfAudioPacketSize();
    if ( pMpeg4->CheckError(Common::GETMAXAUDIO) ) bMp4Error = true;
#endif /* defined (FEATURE_QTV_WINDOWS_MEDIA) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */

    // Return now if any MP4 error.
    if ( bMp4Error && mediaInfo.dataState != Media::DATA_END )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "MP4 Error in audioMgr::Prep");
      return false;
    }

    ///////////////////////
    // Calculate more parameters.
    ///////////////////////

    //frames per msec.
    if ( audioInfo.nSamplesPerFrame==0 )
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


    float f;
    // Do the following computation only for media that gives a positive finite
    // value for 'durationMsec'
    if(audioInfo.nDurationMsec > 0)
    {
      //total audio bytes
      //get num frames
      f = (float)audioInfo.nDurationMsec * clipInfo.fFramesPerMsec;
      audioInfo.nTotalBytes = (long)f;
      //add one to compensate for truncation.
      audioInfo.nTotalBytes++;
      //get num bytes.  this is only accurate for file playback
      //of fixed sample size!  otherwise it will be too large.
      audioInfo.nTotalBytes *= audioInfo.nMaxBytesPerFrame;
    }
    else
    {
      audioInfo.nTotalBytes = 0;
    }
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

#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif

    ///////////////////////
    // Init audio buffer
    ///////////////////////

    audioBuffer.nMaxSize = clipInfo.nFramesPerBuffer;

    /* Audio buffer statically allocated for a very large size and in the run time used only
     * for couple of thousand bytes. Hence the audio buffer allocation is moved to fill audio
     * buffer method where it is allocated only for the necessary size
     */
    audioBuffer.nMaxData = 0;
          audioBuffer.pPendingData = (unsigned char*)QTV_Malloc(audioInfo.nMaxBytesPerFrame);
    if ( audioBuffer.pPendingData == NULL )
    {
      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
                  "Can't allocate data buffer of size %d",
                  audioInfo.nMaxBytesPerFrame);
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

#ifdef SHOW_TIMING
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::StartPlaying exit %d",ZUtils::Clock());
#endif
  return true;
}

/* ======================================================================
FUNCTION
  AudioMgr::AllocateAudioBuffer

DESCRIPTION
  This routine is executed before starting to fetch audio track data from the media.
  This routine makes sure that audio buffer has sufficient amount of memory.

DEPENDENCIES
  None.

RETURN VALUE
  True/False. Caller should treat the false return seriously since memory is not there.

SIDE EFFECTS
  None.

========================================================================== */
bool AudioMgr::AllocateAudioBuffer(int nSize)
{
  // Parameter validation
  if(nSize <= 0)
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
    "Invalid buffer size(%d) requested; Failing to allocate ",nSize);
    return false;
  }
  // We have non zero size if the execution reach here
  if(audioBuffer.pData && (audioBuffer.nMaxData >= nSize))
  {
    //If audio buffer size is sufficient then return silently. This will occur many times
    return true;
  }

  if(!audioBuffer.pData)
  {

    // Initial Case
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
    "Audio buffer initially allocated for size %d",nSize);
    audioBuffer.nMaxData = nSize;
  }
  else
  {
    // audioBuffer.nMaxData < nSize
    // If the audioBuffer.nMaxData >= nSize we would have returned from this
    // function already. We wouldnt be here. The fact that we are here implies
    // that Subsequent growth in audio buffer is necessary.
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_LOW,
    "Audio buffer expanded to  %d",nSize);
    audioBuffer.nMaxData = nSize;
    QTV_Free(audioBuffer.pData);
  }


  audioBuffer.pData = (unsigned char*)QTV_Malloc(audioBuffer.nMaxData);
  if ( audioBuffer.pData == NULL )
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
    "Can't allocate audio buffer of size %d",audioBuffer.nMaxData);
    // Since the allocation has failed set the MaxData to zero
    audioBuffer.nMaxData = 0;
    return false;
  }

  return true;
}

#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
/* ======================================================================
FUNCTION
  AudioMgr::HandleSampleFrequencyChange

DESCRIPTION
  This routine is executed whenever the sample frequency is changed
  during the playback.

DEPENDENCIES
  None.

RETURN VALUE
  True/False.

SIDE EFFECTS
  None.

========================================================================== */
bool AudioMgr::HandleSampleFrequencyChange()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::HandleSampleFrequencyChange");

  if(!pMpeg4)
  {
    // It should not happen since media has said that data is available
    QTV_ASSERT(0);
  QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_ERROR,
             "HandleSampleFrequencyChange: failed due to invalid media");
  return false;
  }
  audioInfo.nSamplingRate  = pMpeg4->GetAudioSamplingFreq();
  if (audioInfo.nSamplesPerFrame)
  {
     clipInfo.fFramesPerMsec  = (float)audioInfo.nSamplingRate / (float)audioInfo.nSamplesPerFrame;
  }
  clipInfo.fFramesPerMsec /= (float)1000;
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "Frames/ms = %d/1000",
            (int)(clipInfo.fFramesPerMsec*1000) );
  // Total audio bytes calculation
  // Get number of  frames ; duration multiplied by frames per msec
  // Get number of bytes by multiplying to the max bytes per frame.
  // this is only accurate for file playback of fixed sample size!
  // otherwise it will be too large.
  float f               = (float)audioInfo.nDurationMsec * clipInfo.fFramesPerMsec;
  audioInfo.nTotalBytes = (long)(f + 0.5);
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
  f = (float)m_nStoredTimingUpdateMsec * clipInfo.fFramesPerMsec / (float)clipInfo.nFramesPerBuffer;
  clipInfo.nTimingUpdateIntervalBuffers = ZMAX(1, (long)f);

#ifdef SHOW_TIMING
  QTV_MSG2( QTVDIAG_AUDIO_TASK, "FrmPerBuf %d, TimUpdInv %d",
            clipInfo.nFramesPerBuffer,
            clipInfo.nTimingUpdateIntervalBuffers);
#endif

  ///////////////////////
  // Update audio buffer
  ///////////////////////

  audioBuffer.nMaxSize = clipInfo.nFramesPerBuffer;


  return true;
    }

/* ======================================================================
FUNCTION
  AudioMgr::FlushDataRequests

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
bool AudioMgr::FlushDataRequests()
{
  Media::CodecType audioCodec = GetAudioCodecType();
#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
  if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
  {
    InitAVSyncAudioBuffer(true);
  }
#endif
//Flushing the pending data if there is any
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   if(!qtv_cfg_enable_dsp_release)
#endif
   {
     audioBuffer.nPendingData   = 0;
     audioBuffer.nPendingOffset = 0;
     audioBuffer.nPartialFrame  = 0;
   }
  if(!IS_GENERIC_AUDIO_FORMAT(audioCodec))
  {
    if(pAudioCMX)
    {
    pAudioCMX->Flush();
    }
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
    cmx_avsync_update.bCmxFlushed = true;
    bFirstEntryPlayedOut = false;
    firstEntryOffset = 0;
#endif
  }
  return true;
}
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif


/* ======================================================================
FUNCTION
  AudioMgr::DataIsAvailable

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
void AudioMgr::DataIsAvailable()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::DataIsAvailable");
  // set data state to allow reading data.
  mediaInfo.dataState = Media::DATA_OK;

#ifdef SHOW_TIMING
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::DataIsAvailable %d",ZUtils::Clock());
#endif
}


/* ======================================================================
FUNCTION
  AudioMgr::InitPlayData

DESCRIPTION
//init data once per playback interval.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::InitPlayData(bool bRestart)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::InitPlayData");

  if ( !bRestart )
  {
    //once per clip

    audioInfo.nMaxBytesPerFrame = 0;
    audioInfo.nTotalBytes = 0;
    audioInfo.nMediaTimescale=0;
    audioInfo.nSamplesPerFrame=0;
    audioInfo.nSamplingRate = 0;

    clipInfo.nFramesPerBuffer=0;
    clipInfo.nTimingUpdateIntervalBuffers=0;
    clipInfo.fFramesPerMsec=0;

    // Clear old audio data buffer
    if ( audioBuffer.pData )
    {
      QTV_Free(audioBuffer.pData);
      audioBuffer.pData = NULL;
    }

    if ( audioBuffer.pPendingData )
    {
      QTV_Free(audioBuffer.pPendingData);
      audioBuffer.pPendingData = NULL;
    }

    if ( audioBuffer.pSize )
    {
      QTV_Free(audioBuffer.pSize);
      audioBuffer.pSize = NULL;
    }
    audioBuffer.nMaxData = 0;
    audioBuffer.nMaxSize = 0;
    audioBuffer.nData = 0;
    audioBuffer.nSize = 0;
    audioBuffer.nPendingData = 0;
    audioBuffer.nPendingOffset = 0;

    audioTime.tStart=0;
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
    firstEntryOffset = 0;
    bFirstEntryPlayedOut = false;
#endif
    mediaInfo.dataState = Media::DATA_OK;
    InitFrame();
  }

  //once per play

  InitAVTiming();

 #ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
 Media::CodecType audioCodec = GetAudioCodecType();
   if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
   {
     InitAVSyncAudioBuffer(bRestart);
     m_bCurrentTSFromCMXBytesUpdatePending = false;
   }
 #endif

  InitStatistics();

  audioTime.tPlay=0;
  audioTime.tStop=0;

#ifdef SHOW_TIMING
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::InitPlayData %d",ZUtils::Clock());
#endif
}

/* ======================================================================
FUNCTION
  AudioMgr::InitFrame

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
void AudioMgr::InitFrame()
{
  mediaInfo.nFramesProcessed = 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::InitStatistics

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
void AudioMgr::InitStatistics()
{

  QCUtils::EnterCritSect(&audioStats.CS);
  audioStats.nNullFrames = 0;
  audioStats.nCurrentNullChunk=0;
  audioStats.nMaxNullChunk=0;
  audioStats.nMaxAhead=0;
  audioStats.nMaxBehind=0;
  audioStats.nDrift=0;
  audioStats.nMaxProcTime=0;
  audioStats.lastFrame.bValid=false;
  audioStats.lastFrame.nFrame=0;
  audioStats.lastFrame.nTime=0;
  audioStats.lastFrame.nTimestamp=0;
  audioStats.nFramesDiscarded=0;
  QCUtils::LeaveCritSect(&audioStats.CS);

}

/* ======================================================================
FUNCTION
  AudioMgr::InitElapsedTime

DESCRIPTION
//reset elapsed time control

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::InitElapsedTime(long lTime)
{
  elapsed.nLastTimestamp=ZMAX(0,lTime);
  elapsed.bPlaying=false;
  elapsed.nAmtBuffered=0;
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  m_nCurrentTSFromCMXBytes = lTime;
  cmx_avsync_update.elapsedTime = lTime;
#endif
}

/* ======================================================================
FUNCTION
  AudioMgr::StartElapsedTime

DESCRIPTION
//start elapsed time

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::StartElapsedTime()
{
  elapsed.bPlaying=true;
  elapsed.nAmtBuffered=0;
}

/* ======================================================================
FUNCTION
  AudioMgr::EndElapsedTime

DESCRIPTION
//end elapsed time

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::EndElapsedTime(long lTime)
{
  //make a note of the amt of data buffered when audio went from playing
  //to paused or stopped.
  long buffmsec = 0;
  if ( elapsed.bPlaying )
  {
    elapsed.bPlaying=false;
    buffmsec = GetAmtBufferedMsec(lTime);
    elapsed.nAmtBuffered=ZMAX(0,buffmsec);
  }
}

/* ======================================================================
FUNCTION
  AudioMgr::ResetThrottling()

DESCRIPTION
  If we get DATA ACCESS DELAY from CMX and we are currently throttling, reset the datalead.
DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  none

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
void AudioMgr::ResetThrottling()
{
  int nWait = 0;
  if(pAudioCMX)
  {
     nWait = GetAmtBufferedMsec(ZUtils::Clock())  - pAudioCMX->nAudioIFMaxAheadMsec;
  }
  if(nWait > 0)
  {
    nAVSyncAudioBufferSize--;
    if ((pMpeg4 != NULL) && is_GenericBcastMedia(pMpeg4))
    {
      // Set the Lead to zero here since delay indication received from Audio CMX
      nAVSyncAudioBufferSize      = 0;
      nAVSyncAudioBufferWriteIndex= 0;
    }
    QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "ResetThrottling():Updated nAVSyncAudioBufferSize to %d",nAVSyncAudioBufferSize);
  }
}
#endif

/* ======================================================================
FUNCTION
  AudioMgr::GetElapsedTime

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
long AudioMgr::GetElapsedTime()
{

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  Media::CodecType audioCodec = GetAudioCodecType();
  if((IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec)) && (m_nCurrentTSFromCMXBytes >= 0) )
  {
    /* Incase of byte-based AVSync, we should always return timestamp got from feedback */
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "elapsedTime from m_nCurrentTSFromCMXBytes is %d", m_nCurrentTSFromCMXBytes);
    return m_nCurrentTSFromCMXBytes;
  }
#endif
    long GetAmtBufMsec = GetAmtBufferedMsec(ZUtils::Clock());
    uint32 buffered_duration = (uint32)ZMAX(0,GetAmtBufMsec);

  if ( elapsed.bPlaying  && buffered_duration )
  {
    if ( elapsed.nLastTimestamp < (long)buffered_duration )
    {
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
      cmx_avsync_update.elapsedTime =0;
      return cmx_avsync_update.elapsedTime;
#else
      return 0;
#endif
    }
    else
    {
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
      //subtract out amt of audio currently buffered.
      cmx_avsync_update.elapsedTime = elapsed.nLastTimestamp - (long)buffered_duration;
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "elapsedTime is %d", cmx_avsync_update.elapsedTime);
      return cmx_avsync_update.elapsedTime;
#else
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "elapsedTime is %d", elapsed.nLastTimestamp - (long)buffered_duration);
      return elapsed.nLastTimestamp - (long)buffered_duration;
#endif
    }
  }
  else
  {
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "elapsedTime is %d", cmx_avsync_update.elapsedTime);
    return cmx_avsync_update.elapsedTime;
#else
    //subtract out amt of audio that was buffered when audio went idle.
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "elapsedTime is %d", elapsed.nLastTimestamp-elapsed.nAmtBuffered);
    return elapsed.nLastTimestamp-elapsed.nAmtBuffered;
#endif
  }
}


/* ======================================================================
FUNCTION
  AudioMgr::GetAmtBufferedMsec

DESCRIPTION
//see how far data processing was ahead of audio playback at indicated time.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
long AudioMgr::GetAmtBufferedMsec(unsigned long lTime)
{
  long nCurTimestamp;
  float nCurFrame;
  long cmxAmtBufferedMse = 0;

  //Member 'pMeg4' could be set to NULL in the audio task and this function
  //could be called in the video task context, therefore guard this section of
  //code
  QCUtils::EnterCritSect(&audioMgrCS);

  if (pMpeg4 != NULL)
  {
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
    Media::CodecType audioCodec = GetAudioCodecType();
    if(Media::WMA_CODEC == audioCodec)
    {
    #if defined (FEATURE_QTV_WINDOWS_MEDIA) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
      //Calculate how much data is buffered based on number of entries
      //in the mapping table and estimated duration per ASF packet.
      //This is particularly useful in streaming WM when there is packet drop.
      //Calculation based on frames per MSEC doesn't work correctly.

      int DurationPerpacket = pMpeg4->GetAudioASFPacketDuration(0);
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "DurationPerpacket is %d", DurationPerpacket);
      long nDataLead = nAVSyncAudioBufferSize * DurationPerpacket;
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioDataLead calculated from DurationPerpacket is %d", nDataLead);
      QCUtils::LeaveCritSect(&audioMgrCS);
      return nDataLead;
    #else

      QCUtils::LeaveCritSect(&audioMgrCS);
      return 0;
    #endif /* (FEATURE_QTV_WINDOWS_MEDIA) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
    }

    /*
    *IS_BYTE_BASED_AVSYNC_SUPPORTED returns TRUE if BYTE BASE AV SYNC is enabled for given audio codec.
    */
    if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
    {
    #ifdef FEATURE_QTV_AVI
      if(pMpeg4->isAviFileInstance())
      {
        long nDataLead =0;
        if(nAVSyncAudioBufferSize != 0)
        {
          int nBottomIndex = nAVSyncAudioBufferWriteIndex - 1;
          if(nBottomIndex < 0)
          {
            nBottomIndex += MAX_AVSYNC_AUDIO_BUFFER_SIZE;
          }
          QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
            "GetAmtBufferedMsec: nAVSyncAudioBufferWriteIndex: %d,nAVSyncAudioBufferSize: %d, nBottomIndex: %d",
            nAVSyncAudioBufferWriteIndex, nAVSyncAudioBufferSize, nBottomIndex);

          if( nAVSyncAudioBufferSize > 1 )
          {
            if(audioBuffer.nPartialFrame == 0)
            {
              nDataLead = AVSyncAudioBuffer[nBottomIndex].nTimeStamp +
                          AVSyncAudioBuffer[nBottomIndex].nSampleDelta -
                          m_nCurrentTSFromCMXBytes;
            }
            else
            {
              nDataLead = AVSyncAudioBuffer[nBottomIndex].nTimeStamp - m_nCurrentTSFromCMXBytes;
            }
          }
          else
          {
            if(audioBuffer.nPartialFrame == 0)
            {
              nDataLead = AVSyncAudioBuffer[nBottomIndex].nTimeStamp +
                          AVSyncAudioBuffer[nBottomIndex].nSampleDelta - m_nCurrentTSFromCMXBytes;
            }
            else
            {
              nDataLead = 0;
            }
          }
          QTV_MSG_PRIO1( QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "GetAmtBufferedMsec::nDataLead is %d", nDataLead);
        }
        else
        {
          QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,"AudioMgr::GetAmtBufferedMsec: nAVSyncAudioBufferSize is 0, so nDataLead is 0");
        }
        QCUtils::LeaveCritSect(&audioMgrCS);
        return nDataLead;
      }
      else
    #endif
      {
        //Calculate how much data is buffered based on number of entries
        //in the mapping table and duration per frame for given audio codec.
        float bufferedDuration = 0;
        if (clipInfo.fFramesPerMsec)
        {
          QTV_MSG1( QTVDIAG_AUDIO_TASK, "DurationPerFrame is %d", 1/clipInfo.fFramesPerMsec);
          bufferedDuration = (float)nAVSyncAudioBufferSize / clipInfo.fFramesPerMsec;
        }
        long nDataLead = (long)bufferedDuration;
        QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioDataLead calculated from DurationPerFrame is %d", nDataLead);
        QCUtils::LeaveCritSect(&audioMgrCS);
        return nDataLead;
      }
    }
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */

     if(pMpeg4->GetSource() == Media::STREAM_SOURCE)
     {
       if (xferInfo.startOfInterval.bValid)
       {
         // Calclulate elapsed time since start of interval.
         long nElapsedTime = (long)lTime - xferInfo.startOfInterval.time;
         if (nElapsedTime < 0)
         {
           nElapsedTime = 0;
         }
         // nElapsedTime should be >= 0 at this point

         // Calculate elapsed frames in interval.
         float f = (float)nElapsedTime * clipInfo.fFramesPerMsec;

         // Get current position by adding elapsed time to start of interval.
         nCurFrame = xferInfo.startOfInterval.nFrame +
                    (float)xferInfo.startOfInterval.nFrameFraction/FRAME_FRACTION_BASE + f;
   long cmxRemainedFrames   = (long)audioBuffer.nLastFrame - (long)nCurFrame;
         if (clipInfo.fFramesPerMsec)
         {
           cmxAmtBufferedMse = (long)(cmxRemainedFrames / clipInfo.fFramesPerMsec);
         }

#ifdef SHOW_TIMING
         QTV_MSG2( QTVDIAG_AUDIO_TASK, "Time %d Last %d",
             lTime, cmxAmtBufferedMse);
#endif
         QCUtils::LeaveCritSect(&audioMgrCS);
         return cmxAmtBufferedMse;
       }//xferInfo.startOfInterval.bValid
     } // pMpeg4->GetSource() == Media::STREAM_SOURCE
     else
     {
       if ( GetAudioPosition(lTime,nCurTimestamp,nCurFrame) )
       {
#ifdef SHOW_TIMING
         QTV_MSG3( QTVDIAG_AUDIO_TASK, "Time %d Last %d Curr %d",
                   lTime, xferInfo.nLastTimestampSent - nCurTimestamp, nCurTimestamp);
#endif
         QCUtils::LeaveCritSect(&audioMgrCS);
         return(xferInfo.nLastTimestampSent - nCurTimestamp);
       }
     }// pMpeg4->GetSource() == Media::STREAM_SOURCE
  }// pMpeg4 != NULL
  QCUtils::LeaveCritSect(&audioMgrCS);
  return 0;
}

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_GENERIC_BCAST_PCR) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_GENERIC_BCAST_PCR ||
          FEATURE_QTV_IN_CALL_VIDEO */

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
bool AudioMgr::PlaySomeAudio(
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

  // QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,  "PlaySomeAudio");

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

  if ( (pAudioCMX) && (pAudioCMX->nAudioIFMaxAheadMsec!=(-1)) )
  {
    //check data position vs calculated play position.
    long nDataLead = GetAmtBufferedMsec(ZUtils::Clock());

#ifdef SHOW_TIMING
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioDataLead %d", nDataLead);
#endif

    cmxDataLead = nDataLead;

    /* Audo data lead caliculation (Min and Max) */
    long lAudioDataLead = cmxDataLead;
    if (m_MaxCmxAudioDataLead < lAudioDataLead) {
       m_MaxCmxAudioDataLead = lAudioDataLead;
    }
    else if (lAudioDataLead < m_MinCmxAudioDataLead) {
       m_MinCmxAudioDataLead = lAudioDataLead;
    }
    //compare data to play position and keep stats.
    if ( nDataLead < 0 )
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
        For Bcast Sources , we don't care how far audio gets behind.  Keep playing.
        Since we need to support the Mute command , this trick needs to be applied
        to the Bcast file/trick play also.
      -------------------------------------------------------------------*/
#ifndef FEATURE_QTV_GENERIC_BCAST
        if ( (pMpeg4) && (pMpeg4->GetSource() != Media::BCAST_FLO_SOURCE) )
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
          ShowInterval(xferInfo.startOfInterval);
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

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
   if( (Media::WMA_CODEC == GetAudioCodecType()
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
        ||Media::WMA_PRO_CODEC == GetAudioCodecType()
        ||Media::WMA_PRO_PLUS_CODEC == GetAudioCodecType()
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
        ) && isStartOfDummyBytes() )
    {
      //Don't throttle if asffile has already started sending dummy bytes.
      nWaitMsec = 0;
    }
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef SHOW_TIMING
    QTV_MSG3( QTVDIAG_AUDIO_TASK, " Wait %d Lead %d MaxAhead %d",
              nWaitMsec, nDataLead, pAudioCMX->nAudioIFMaxAheadMsec);
#endif
  }

  if ( (pAudioCMX) &&
       ( (nWaitMsec <= 0) || (pAudioCMX->image_swap) || ( (nWaitMsec > 0) && (audioBuffer.nPartialFrame) ) ) &&
       (!bError)
     )
  {
    //Send another buffer.
    long tProc;
    long tPlayBegin;

    tProc = (long)ZUtils::Clock();
    tPlayBegin = tProc;

    bSent = pAudioCMX->SendData(tPlayBegin, nWaitMsec);
    // If dynamic codec param change detected then bail out immediately.
    if(IsCodecParamChanged())
    {
      QTV_MSG(QTVDIAG_AUDIO_TASK,"Dynamic Codec Parameter change detected");
      bError = false;
      return false;

    }
    tProc = (long)ZUtils::Clock() - tProc;

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
      /* This is to avoid the avoid situations like giving -1 as the
         audio playback time (which should never be the case) to the above layers
      */
      elapsed.nLastTimestamp=ZMAX(0, audioBuffer.nLastTimestamp);

      //Set AV timing on each buffer.  Update AV timing using the end
      //of buffer point in order to correct for any discontinuities.
      SetAVTiming(tPlayBegin,audioBuffer.nFirstTimestamp,audioBuffer.nFirstFrame);

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
      Media::CodecType audioCodec = GetAudioCodecType();
       if( (!IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
      #ifdef FEATURE_QTV_AVI
          &&(pMpeg4 && !pMpeg4->isAviFileInstance())
      #endif
         )
#endif
      {
        UpdateAVTiming(audioBuffer.nLastTimestamp,audioBuffer.nLastFrame);
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
      //Make sure device doesn't wait forever without
      //sending any data.
      if ( (xferInfo.nBuffersSentInInterval==0) && (nWaitMsec>0) && (mediaInfo.dataState!=Media::DATA_END) )
      {
        long n = (long)ZUtils::Clock()-xferInfo.tStartInterval;
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
 if (  (pMpeg4) &&
                   (Media::BCAST_FLO_SOURCE == pMpeg4->GetSource() ||
                    Media::BCAST_ISDB_SOURCE == pMpeg4->GetSource())
                &&
       (Media::DATA_UNDERRUN == mediaInfo.dataState) )
#else

  /* The concept of audio hardware based underrun is equally applicable
   * to file/trick play as well.
   */
  if ( is_GenericBcastMedia(pMpeg4) &&
       Media::DATA_UNDERRUN == mediaInfo.dataState )
#endif
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

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif  //FEATURE_QTV_GENERIC_BCAST_PCR
    {
    /*-------------------------------------------------------------------
      Media audio underruns shall never be passed up to the audio thread.
      Override it so that the audio thread can recover.
    -------------------------------------------------------------------*/
    mediaInfo.dataState = Media::DATA_OK;
  }

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

/* ========================================================================
FUNCTION:
  AudioMgr::IsDataBeingRestored

DESCRIPTION:
  Check to see if data being restored .

PARAMETERS:
  None.

RETURN VALUE:
  true/false.
===========================================================================*/
bool AudioMgr::IsDataBeingRestored()
{
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  Media::CodecType audioCodec = GetAudioCodecType();
  int32 nMaxEntries = NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec];
  if((RestoreDataBufferInfo.nEntriesToRestore > 0) &&
     (RestoreDataBufferInfo.nTmpReadIndex >= 0)    &&
     (RestoreDataBufferInfo.bRestoreBufferValid) &&
      ((RestoreDataBufferInfo.nWriteIndex -
        RestoreDataBufferInfo.nTmpReadIndex + nMaxEntries)%nMaxEntries > 0 ))
  {
    return true;
  }
#endif
  return false;
}

/* ======================================================================
FUNCTION
  AudioMgr::FillAudioBuffer

DESCRIPTION
//
//  Fill the audio buffer with data
//  Set dataState when changed.
//  Return true if there's data.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioMgr::FillAudioBuffer(const uint32 max_len, const uint32 offset)
{
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
   int outDataSize = 0;
   //Initialize the data source to unknown. We will set this to an appropriate
   //value below.
   m_DataSrcFlag = UNKNOWN;
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
   long VirtualPacketSize=0;
#endif /* #if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

  // QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "AudioMgr::FillAudioBuffer");
  audioBuffer.nData=0;
  audioBuffer.nSize=0;

  // Make sure that sufficient room is available within audio buffer
  if(!AllocateAudioBuffer(max_len))
  {
    mediaInfo.dataState = Media::DATA_ERROR;
    Abort();
    return false;
  }
  //don't exceed the number of bytes requested
  //or the actual buffer size.
  int nLimit = ZMIN(audioBuffer.nMaxData, (int)max_len);

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
   if( (Media::WMA_CODEC == GetAudioCodecType()
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
        ||Media::WMA_PRO_CODEC == GetAudioCodecType()
        ||Media::WMA_PRO_PLUS_CODEC == GetAudioCodecType()
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
        ) && (!isStartOfDummyBytes()) )
  {
    VirtualPacketSize=GetVirtualPacketSize();

    /*
    Try to send as many packets as possible in allowed MAX limit while answering first data request.
    For some OEMs,when we report PLAYING, some fancy stuff
    in UI etc. might now give enough CPU to send subsequent packets which might starve the DSP.
    */

    nLimit = ZMIN(2* VirtualPacketSize, nLimit);
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "Adjusted nLimit: %d",nLimit);
  }
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_WMA_PRO_DECODER
if ( (Media::WMA_PRO_CODEC != GetAudioCodecType()) && (Media::WMA_PRO_PLUS_CODEC != GetAudioCodecType()) )
#endif//#ifdef FEATURE_QTV_WMA_PRO_DECODER
{
  /*
  * When WMA_PRO_DECODER feature is turned ON, CMX buffer size can be as big as 56K.
  * While playing non Pro codec, we don't want to end up sending too much ahead
  * otherwise, it will cause frequent buffering/re-buffering.
  */
  nLimit = ZMIN(CMX_DEFAULT_BUFFER_SIZE, nLimit);
}


  // Its possible that the Audio Decoder's buffer may be smaller (1600) than the largest
  // possible AudioFrameSize (2048) for example.
  //int nLargestFrameSize = ZMIN(audioInfo.nMaxBytesPerFrame, (int)max_len);

  if ( pMpeg4 && pAudioCMX )
  {
    if ( audioBuffer.pData )
    {
      //Add samples to the buffer
      int  nBytes = 0;
      int numSample = 1;
      Media::MediaStatus status = Media::DATA_NULLFILL;
      long nTimestampMsec = 0;
      int duration_msec =0;
      int frame_duration =  0;
      bool bDataBeingRestore = false;
      bool bFetchedFromPendingBuffer = false;
	  bool bPendingDataAboveMaxDataLead = false;
      if(clipInfo.fFramesPerMsec)
      {
         frame_duration = (int)(1/clipInfo.fFramesPerMsec);
      }

      while (
            //while there's data
            (mediaInfo.dataState == Media::DATA_OK||IsDataBeingRestored())
            //and sample count not exceeded
            && (audioBuffer.nSize < audioBuffer.nMaxSize)
            //and there's room for one more maximum size sample
            // && ((audioBuffer.nData + nLargestFrameSize) <= nLimit)
            && (audioBuffer.nData < nLimit)
            && (  pMpeg4->IsMinAudioBuffered()  ||
                 ( (duration_msec + GetAmtBufferedMsec(ZUtils::Clock())) < pAudioCMX->nAudioIFMaxAheadMsec)  ||
                 (audioBuffer.nPartialFrame) )
            )
      {
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
        if( ( m_audioCodec == Media::GSM_AMR_CODEC
             || m_audioCodec == Media::AAC_CODEC
             || m_audioCodec == Media::BSAC_CODEC
             || m_audioCodec == Media::QCELP_CODEC 
			 || m_audioCodec == Media::EVRC_CODEC )
             && IS_BYTE_BASED_AVSYNC_SUPPORTED(m_audioCodec) )
        {
          int nDataLeadFromCurrentSamples = 0;
          /*
          * Calculate duration for all audio samples accumulated so far, while filling the audio buffer.
          * We need to make sure we never send more than the MAX lead.
          * This will especially help to reduce size of restore buffer for AMR
          * as we might end up sending too many samples in order to fill the entire buffer.
          * Subsequently, this can be done for all audio codecs except WMA which
          * have byte base AV SYNC support.
          */
          if( clipInfo.fFramesPerMsec && (pAudioCMX) )
          {
            if(pMpeg4->isAviFileInstance())
            {
              // For AVI case we dont require clock time to calculate datalead
              nDataLeadFromCurrentSamples = GetAmtBufferedMsec(0);
            }
            else
            {
              nDataLeadFromCurrentSamples =(int) ((float)audioBuffer.nSize / clipInfo.fFramesPerMsec);
            }

            QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_LOW,
                          "#samples accumulated %d nDataLeadFromCurrentSamples %d pAudioCMX->nAudioIFMaxAheadMsec %d",
                          audioBuffer.nSize,nDataLeadFromCurrentSamples,pAudioCMX->nAudioIFMaxAheadMsec);
            if( (audioBuffer.nPartialFrame == 0) &&
                (pAudioCMX->nAudioIFMaxAheadMsec < GetAmtBufferedMsec(ZUtils::Clock())))
            {
              QTV_MSG( QTVDIAG_AUDIO_TASK,"Not filling any more samples..");
              return true;
            }
          }
        }
#endif
        //Get audio sample
        numSample = 1;

        // QTV_MSG( QTVDIAG_AUDIO_TASK, "B %d BMax %d FMB: %d #Sz %d Lim %d",
        // audioBuffer.nData, audioBuffer.nMaxSize,
        // audioInfo.nMaxBytesPerFrame, audioBuffer.nSize, nLimit);

        //WZeng, add "int channel" to support aac silence insertion when streaming, 06/05/02
        cmx_af_aac_spec_type *pCmx_af_aac_spec = NULL;
        if(pAudioCMX)
        {
          pCmx_af_aac_spec = (cmx_af_aac_spec_type *) ((AudioCMX*)pAudioCMX)->GetCmx_af_aac_spec();
        }

        // Change Logic to put 1600 bytes even in each audio buffer.
        // i.e. Not Frame Aligned
        bFetchedFromPendingBuffer = false;
        if ( audioBuffer.nPendingData )
        {
          // Calculate the Pending data pointer
          unsigned char * pStart = audioBuffer.pPendingData + audioBuffer.nPendingOffset;
          //Thre data that we are sending to the cmx is from the pending
          //buffer. Setting the flag accordingly.
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
          m_DataSrcFlag = PENDING;
#endif
          // Pick the timestamp value from the one stored when fill audio buffer was called previously
          // This is essential when the samples are retrieved from the restore data buffer.
    nTimestampMsec = audioBuffer.nPartialTimestamp;
          bFetchedFromPendingBuffer = true;

          //New debug message. Might come in handy while collecting logs.
          QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH,
                              "DBG_MSG Restoring from pending buffer with length%ld",audioBuffer.nPendingData );
          if((audioBuffer.nPendingData + audioBuffer.nData)<= nLimit)
          {
            // Case 1: When the partial frame status cleared since all the bytes are grabbed
            QTV_MSG1( QTVDIAG_AUDIO_TASK,
                      "Completed Partial Frame by Grabbing %d Pending Bytes",
                      audioBuffer.nPendingData);
            // Simulate the media fetch by grabbing the entire buffer fragmented
          status = Media::DATA_OK;
            nBytes = audioBuffer.nPendingData;
            memcpy( (audioBuffer.pData + audioBuffer.nData), pStart, nBytes);
            // Clear the Pending flags so that next pull will be made from the media
            audioBuffer.nPendingData   = 0;
            audioBuffer.nPendingOffset = 0;
         }
          else
          {
            // Case 2. Partial frame status continue
            // since entire buffer pending could not be accomadated ,
            // copy for the space available in the buffer
            status = Media::DATA_OK;
            nBytes = nLimit - audioBuffer.nData;
            QTV_MSG1( QTVDIAG_AUDIO_TASK, "Grabbed %d Pending Bytes", nBytes);
            memcpy( (audioBuffer.pData + audioBuffer.nData), pStart, nBytes);
            // Move the pending offset by number of bytes copied; update pending byte count
            audioBuffer.nPendingData   -= nBytes;
            audioBuffer.nPendingOffset += nBytes;
          }
          if( pAudioCMX->nAudioIFMaxAheadMsec < GetAmtBufferedMsec(0))
          {
            QTV_MSG( QTVDIAG_AUDIO_TASK,"Filling only partial frames as maxAudioLead is satisfied.");
            bPendingDataAboveMaxDataLead = true;
          } 
        }
        else
        {

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
         //Obtain the capacity of the restore buffer. This will be used in
         //the calculation below.
         Media::CodecType audioCodec = GetAudioCodecType();
         int32 nMaxEntries = NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec];

          //Don't add data back to call data buffer while restoring as it already
          //exists in the call buffer. It will be flushed out when feedback comes in from DSP.
          //Also make sure that we are restoring only those number of entries
          //that are actually present in the restore buffer. For this check
          //the nTmpReadIndex against the write index. Without this check,
          //we may end up giving data to cmx from a non-existent buffer entry.
          if(
              (IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))  &&
              (RestoreDataBufferInfo.nEntriesToRestore > 0) &&
              (RestoreDataBufferInfo.nTmpReadIndex >= 0)    &&
              (RestoreDataBufferInfo.bRestoreBufferValid) &&
              ((RestoreDataBufferInfo.nWriteIndex -
                RestoreDataBufferInfo.nTmpReadIndex + nMaxEntries)
                %nMaxEntries > 0 ))
          {
            bDataBeingRestore = true;
            audioBuffer.nPartialFrame =0;
            //More logs for debug.
            QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH,
                              "DBG_MSG Restoring from data buffer with entries%ld", RestoreDataBufferInfo.nEntriesToRestore);
            if(RestoreDataBufferInfo.nTmpReadIndex >= NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec])
            {
              RestoreDataBufferInfo.nTmpReadIndex = 0;
            }
            if( audioBuffer.pPendingData )
            {
              //Retrieve the data
              //Copy data from the restore buffer and set the flag to signal
              //that we have obtained this data from the restore buffer.
              m_DataSrcFlag = RESTORE;
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
                m_bFirstAudioFragment = true;
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
          else
          {
            // If the status changes witin the while loop we need to fetch the sample from media
            // Otherwise what was happening is that media sample is not retrieved from the media
            // untill the fill audio buffer is called for the next time.
            bDataBeingRestore = false;
          }
#endif
         if ((!bDataBeingRestore) && (audioBuffer.pPendingData))
         {
            nBytes = FetchAudioSample(pMpeg4,status,&numSample,audioInfo.nMaxBytesPerFrame,
                                                   (int)pCmx_af_aac_spec->channel);
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
            //Read from file, as there is no data in pending or restore buffers.
            m_DataSrcFlag = MEDIA;
           outDataSize = nBytes;
           m_bFirstAudioFragment = true;
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
         }


          /* some time we get NO-DATA frames, so just skip that */
          if ( (nBytes==0) && (status==Media::DATA_OK) )
          {
            /* this is NO-DATA frame */
            continue;
          }
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif // FEATURE_QTV_ADTS_PARSER

          if(IsCodecParamChanged())
          {
            #ifdef FEATURE_QTV_DYNAMIC_AAC_ADTS
#error code not present
            #else
              #ifdef FEATURE_QTV_CODEC_CHANGE_RESTART
#error code not present
              #endif
            #endif
          }


          if ( (nBytes > 0) && ((audioBuffer.nData + nBytes) <= nLimit) && audioBuffer.pPendingData)
          {
            //QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Base %X Offset %X %d Bytes", audioBuffer.pData, audioBuffer.pData + audioBuffer.nData, nBytes);
            memcpy( (audioBuffer.pData + audioBuffer.nData), audioBuffer.pPendingData, nBytes);
            audioBuffer.nPendingData = 0;
          }
          else if(nBytes > 0 && audioBuffer.pPendingData)
          {
           /*
           * It's possible to have nBytes as 0 and status as
           * DATA_SKIP or DATA_UNDERRUN etc. Thus, if there is no audio data, we should not attempt
           * to send anything by copying the data into audioBuffer.pData.
           * Otherwise, when nBytes is 0, audioBuffer.nPendingData might become negative
           * when we execute 'nBytes - (nLimit - audioBuffer.nData)' and will cause
           * the crash.
           */
            // Frame is crossing the audio buffer boundary
            // Wait for the next call
            // Store the true size of the frame in nPartialFrame attribute
            // (nPendingData,nPendingOffset) will contain the remaining buffer details
            audioBuffer.nPartialFrame   = nBytes;
            audioBuffer.nPartialSamples = numSample;
            // nBytes modified to nBytesToCopy
            nBytes = nLimit - audioBuffer.nData;
            memcpy( (audioBuffer.pData + audioBuffer.nData), audioBuffer.pPendingData, nBytes);
            audioBuffer.nPendingData   = audioBuffer.nPartialFrame - nBytes;
            audioBuffer.nPendingOffset = nBytes;
            QTV_MSG2( QTVDIAG_AUDIO_TASK, "Pending %d Bytes; size = %d",
              audioBuffer.nPendingData,audioBuffer.nPartialFrame);
          }
        }

        // QTV_MSG( QTVDIAG_AUDIO_TASK, "FillAudioBuffer nBytes %d %d status %d", nBytes, audioBuffer.nData + nBytes, status);

        //Check status
        switch ( status )
        {
        case Media::DATA_OK:
        case Media::DATA_NULLFILL:
          {
            mediaInfo.dataState = status;
            ProcessAudioSample(nBytes,nTimestampMsec,
                               (bDataBeingRestore || bFetchedFromPendingBuffer),
                               numSample);
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
			if(bPendingDataAboveMaxDataLead)
			{
				return(audioBuffer.nData>0);
			}

            break;
          }// OK or NULLFILL

#ifdef FEATURE_FILE_FRAGMENTATION
        case Media::DATA_FRAGMENT:
          //look at the error detail
          (void)pMpeg4->CheckError(Common::GETAUDIO);
          mediaInfo.dataState = status;
          break;
#endif //FEATURE_FILE_FRAGMENTATION
        case Media::DATA_REALLOCATE_BUFFER:
          //look at the error detail
          (void)pMpeg4->CheckError(Common::GETAUDIO);
          uint32 bufferSize;
          bufferSize = 0;

          if(pMpeg4)
            bufferSize = pMpeg4->GetMaxAudioBufferSizeDB();
          if(bufferSize > (uint32)audioInfo.nMaxBytesPerFrame)
          {
            audioInfo.nMaxBytesPerFrame = bufferSize;
                audioBuffer.pPendingData = (unsigned char *)QTV_Realloc(audioBuffer.pPendingData,
                                                                    audioInfo.nMaxBytesPerFrame);
                if(!audioBuffer.pPendingData)
                {
              QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
              "Can't re-allocate data buffer of size %d", audioInfo.nMaxBytesPerFrame);
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
         duration_msec += frame_duration;
      }
    }
  }

#ifdef SHOW_FRAMES
  if ( audioBuffer.nData>0 )
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "Aud Frame %d ts %d",audioBuffer.nFirstFrame,audioBuffer.nFirstTimestamp);
#endif


  /* To fix the compilation warning */
  (void)offset;

  return(audioBuffer.nData>0);
}

#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
/* ======================================================================
FUNCTION
  AudioMgr::RestoreDataBufferInsertSample

DESCRIPTION
  Inserts the data sample read into the restore buffer.

DEPENDENCIES
  None.

RETURN VALUE
  True/False , depending on whether the insert is successful or not.

SIDE EFFECTS
  None.

========================================================================== */
void AudioMgr::RestoreDataBufferInsertSample(int nBytes,
                                             long nTimestamp,
                                             unsigned long nSampleCount)
{



  //Don't add packet into RestoreDataBufferInfo.UnAckData if RestoreDataBufferInfo.nEntriesToRestore > 0,
  //which means we are restoring existing data from the buffer.
  Media::CodecType audioCodec = GetAudioCodecType();
  int nCurrEntrySizeAllocated = GetRestoreDataBufferEntrySize(audioCodec);
  int nMaxEntries = NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec];

  if(nBytes > nCurrEntrySizeAllocated)
  {
    QTV_MSG_PRIO2(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_FATAL,
                   "Sample size  %d is more than the restore buffer entry size  %d returning without restoring",
                   nBytes, nCurrEntrySizeAllocated);
    QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_FATAL,
                   "we need to increase the restore buffer entry size to %d ", nBytes);

     /* Here we are retrunig even though we don't restore because we should continue the playback
      * if we don't restore and continue playback may lead some issues during pause/resume
      */
    return;
  }

  QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH,
                "DBG_MSG: Inserting sample into restore data buffer %ld, %ld, %ld",
            nBytes, nTimestamp,nSampleCount );


  //make sure we are not over-writing
  if( (RestoreDataBufferInfo.bRestoreBufferValid) &&
      (!RestoreDataBufferInfo.nEntriesToRestore)  &&
      (nBytes <  nCurrEntrySizeAllocated) )
  {

    if((nLastWriteIndex >=0) && (nLastWriteIndex < nMaxEntries))
    {
      //Make sure that we add an entry to the restore buffer only if its time
      //stamp is greater than the previous entry added to the restore buffer.
      //This prevents duplicate entries.
      if(RestoreDataBufferInfo.UnAckDataBuffer[nLastWriteIndex].nPresTime >= nTimestamp)
      {
        QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH,
                      "Sample With Timestamp %ld Already Inserted in Restore Databuffer,So Rejecting The Duplicate entry",
            nTimestamp );
        return;
      }
    }

    if(RestoreDataBufferInfo.nWriteIndex == nMaxEntries)
    {
      RestoreDataBufferInfo.nWriteIndex = 0;
    }

    RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].nSize = nBytes;
    RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].nFrames = nSampleCount;
    RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].nPresTime = nTimestamp;
    memcpy(RestoreDataBufferInfo.UnAckDataBuffer[RestoreDataBufferInfo.nWriteIndex].pDataPtr,
           audioBuffer.pPendingData,
           nBytes);
    nLastWriteIndex = RestoreDataBufferInfo.nWriteIndex;
    RestoreDataBufferInfo.nWriteIndex++;
    RestoreDataBufferInfo.nEntriesInRestoreBuffer++;

    if(RestoreDataBufferInfo.nEntriesInRestoreBuffer > nMaxEntries)
    {
      RestoreDataBufferInfo.nEntriesInRestoreBuffer = nMaxEntries;
      RestoreDataBufferInfo.nReadIndex++;
      if(RestoreDataBufferInfo.nReadIndex > nMaxEntries)
      {
        RestoreDataBufferInfo.nReadIndex = 0;
      }
      QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_FATAL,
                    "OVERFLOW RestoreDataBufferInfo.nEntriesInRestoreBuffer %ld",
                    (RestoreDataBufferInfo.nEntriesInRestoreBuffer- nMaxEntries));
    }
  }

  return;

}

/* ======================================================================
FUNCTION
  AudioMgr::AVSyncAudioBufferInsertEntry

DESCRIPTION
  Inserts the data sample read into the AV Sync buffer.

DEPENDENCIES
  None.

RETURN VALUE
  True/False , depending on weather the insert is successful or not.

SIDE EFFECTS
  None.

========================================================================== */
bool AudioMgr::AVSyncAudioBufferInsertEntry(int nBytes, long nTimestamp)
{
  // Assume everything went well
  bool bRet = true;

  if(nAVSyncAudioBufferSize < MAX_AVSYNC_AUDIO_BUFFER_SIZE)
  {

    /* Storing timestamps and byteoffsets for CMX AVSync Callback */
    //Even if we are in the process of restoring data from restore data buffer,
    //add it to the AV SYNC buffer as it gets flushed out.

    nAVSyncAudioWriteByteOffSet += nBytes;
    AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nByteOffSet  = nAVSyncAudioWriteByteOffSet;
    AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nTimeStamp   = nTimestamp;
    AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nFrame       = mediaInfo.nFramesProcessed;
    AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nSampleDelta =
                      pMpeg4->GetTimestampDeltaForCurrentAudioSample(0);
    QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
              "added entry in mapping_table, Bytes %ld  TS %ld SampleDelta %ld",
              AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nByteOffSet,
              AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nTimeStamp,
              AVSyncAudioBuffer[nAVSyncAudioBufferWriteIndex].nSampleDelta);

    if((++nAVSyncAudioBufferWriteIndex) >= MAX_AVSYNC_AUDIO_BUFFER_SIZE)
    {
      nAVSyncAudioBufferWriteIndex = 0;
    }
    nAVSyncAudioBufferSize++;
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,QTVDIAG_PRIO_HIGH,
                  "AVSyncAudioBuffer overflow at timestamp: %d",
                  audioBuffer.nLastTimestamp);
    bRet = false;
  }
  return bRet;
}

#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
/* ======================================================================
FUNCTION
  AudioMgr:ProcessAudioSample

DESCRIPTION
  This routine process the audio sample.

DEPENDENCIES
  None.

RETURN VALUE
 .

SIDE EFFECTS
  None.

========================================================================== */
bool AudioMgr::ProcessAudioSample(int          nBytes,
                                       long       nTimestamp,
                                       bool           bRestoring,
                                       unsigned long nSampleCount)
{
   QTV_MSG3( QTVDIAG_AUDIO_TASK, "DBG_MSG: Bytes = %d, timestamp = %d sample count = %d",
              nBytes,nTimestamp, nSampleCount);
  // If we are not restoring samples from the buffer then obtain the TS from Media
  if(!bRestoring)
  {
    nTimestamp = FetchAudioTimestamp(pMpeg4);
  }

        //If EOS is set in MediaBuffer and if we are restoring from CallDataBuffer
        //don't check error via pMpeg4->CheckError as it will be PV_STREAM_NETWORK_ERROR in
        //StreamMedia. It gets changed to PV_STREAM_SUCESS only when we pull data from MediaBuffer.
        //This will happen when are done with restoring and invoke GetNextAudioSample above.
        //If we don't do this, we won't be able to resume after voice call for WM.
        //For 3GP, we don't maintain extra data buffer so it works correctly.

  if( (bRestoring)|| ((nBytes>0) && !pMpeg4->CheckError(Common::GETAUDIOTIME)) )
          {
            /*
     * Initialized convertedTS to nTimestamp as when we are
     * restoring from restore buffer, nTimestamp is already converted
            * so no need to convert again.
            * If not restoring from restore buffer, convertedTS will be updated again
     * by converting nTimestamp.
            */
    long convertedTS = nTimestamp;
    if(!bRestoring)
           {
             //Not from restore buffer, convert timestamp to msec.
      convertedTS = pMpeg4->ConvertAudioTimestamp(nTimestamp);
           }

#ifdef SHOW_TIMING
    QTV_MSG3( QTVDIAG_AUDIO_TASK, "Audio sample f %d ts %ld converted to %ld ",
              mediaInfo.nFramesProcessed+1, nTimestamp, convertedTS );
#endif

            if((pMpeg4->GetSource() == Media::BCAST_FLO_SOURCE) &&
               (CheckForChannelSwitchFrame == TRUE) &&
       (nTimestamp == ChannelSwitchedTS))
            {
              QTV_MSG1( QTVDIAG_AUDIO_TASK, "  Received first frame after ChannelSwitch = %d",convertedTS);
              AudioChannelSwitchedTS = convertedTS;
            }

            //check for continuity.
    if ( convertedTS < audioBuffer.nLastTimestamp )
            {
              QTV_MSG3( QTVDIAG_AUDIO_TASK, "WARNING weird audio TS seq. %d %d, Media TS %d",
                        audioBuffer.nLastTimestamp,
                convertedTS,
                nTimestamp);
            }
    nTimestamp = convertedTS;

    if(audioBuffer.nPendingData > 0)
    {
      // If we are splitting up the sample across the audio buffer boundary
      // We need to store the media time stamp for the next invocation of the fill audio buffer
      audioBuffer.nPartialTimestamp = nTimestamp;
    }


            //See if the stop time was reached.
    if ( stopTime.bStop && (unsigned long)nTimestamp>stopTime.nStop )
            {
              //when stop time is reached, act like it's the end
              //of the data.
              mediaInfo.dataState=Media::DATA_END;
            }
            //see if this sample is before the current start time
    else if ( nStartTime>=0 && nTimestamp<nStartTime )
            {
              //keep stats on this but add the frame anyway.
              //I would discard it but then there's a risk of
              //CPU hogging in this routine.  With our FF and QtvStream
              //the audio position will always be what we want anyway.
              audioStats.nFramesDiscarded++;
            }

          }
          else
          {
            //unexpected error.
            mediaInfo.dataState=Media::DATA_ERROR;
          }

  if ( mediaInfo.dataState == Media::DATA_OK ||
       mediaInfo.dataState == Media::DATA_NULLFILL)
          {
    bool bPlay = true;


    if(IsCurrentFrameDropped())
            {
      // As long as media reports thate number of bytes read is greater than zero
      // One should treat it as a valid frame even when the status returned as
      // NULL_FILL
      if(nBytes > 0)
      {
  // Drop due to faster playback mode
        bPlay = false;
      }
            }

    UpdateAudioBuffer(nBytes, nTimestamp, nSampleCount,bPlay);
    return true;
  }
  return false;
}
/* ======================================================================
FUNCTION
  AudioMgr:SetFastAudioPlayback

DESCRIPTION
  Prepares the audio player for fast playback mode.

DEPENDENCIES
  None.

RETURN VALUE
  false when invalid modulus or drop count is passed. Otherwise return true.

SIDE EFFECTS
  None.

========================================================================== */
bool AudioMgr::StartFastAudioPlayback(int nModulus, int nDropCount)
            {
  bool bRet = false;
#ifdef FEATURE_QTV_FAST_PLAYBACK_AUDIO
  // Removed the byte based feedback dependency check from here
  // Player setting will be received here un-conditionally
  // Later at the prep time we will double check and reset if necessary.
  if(nModulus == nDropCount)
  {
    QTV_MSG1(QTVDIAG_AUDIO_TASK,
    "Modulus(%d) and Drop Count are equal; FAP Ignored ",
    nModulus);
  }
  // Modulus or drop count has to whole positive
  // Modulus or window has to be greater than Drop Count
  else if ((nModulus <= 0) ||
           (nDropCount <= 0) ||
           (nModulus < nDropCount))
  {
    QTV_MSG2(QTVDIAG_AUDIO_TASK,
    "FAP Ignored due to invalid parameters;drop count(%d) modulus(%d) ",
    nDropCount,nModulus);
            }
            else
            {

    bFastPlaybackMode          = true;
    nFastPlaybackWindow        = nModulus;
    // Magic to reverse the division to multiplication
    // Calculate the inverse to avoid division later
    fFastPlaybackWindowInverse = 1.0;
    if(nModulus)
    {
      fFastPlaybackWindowInverse = (1.0/(float)nModulus);
    }
    nFastPlaybackDropCount     = nDropCount;
    SetupFastAudioPlaybackFilters();
    QTV_MSG2(QTVDIAG_AUDIO_TASK, "Fast Audio Playback Started(%d,%d)",
             nModulus,nDropCount);
    bRet = true;
            }
#else
  // To fix compiler warnings for not using the parameters passed
  (void)nModulus;
  (void)nDropCount;
#endif


  return bRet;
            }

/* ======================================================================
FUNCTION
  AudioMgr:ResetFastAudioPlayback

DESCRIPTION
  Switches the audio player back to the normal playing mode.

DEPENDENCIES
  None.

RETURN VALUE
  None..

SIDE EFFECTS
  None.

========================================================================== */
void AudioMgr::StopFastAudioPlayback()
              {

  QTV_MSG(QTVDIAG_AUDIO_TASK,"ResetFastAudioPlayback");

  if(bFastPlaybackMode)
                {
    DestroyFastAudioPlaybackObject();
    aFastPlaybackFilterConfig = NULL;
  }


  return;
}

/* ======================================================================
FUNCTION
  AudioMgr:DestroyFastAudioPlaybackObject

DESCRIPTION
  Cleanup the members related to the Fast Audio Playback.

DEPENDENCIES
  None.

RETURN VALUE
  None..

SIDE EFFECTS
  None.

========================================================================== */
void AudioMgr::DestroyFastAudioPlaybackObject()
                  {

  QTV_MSG(QTVDIAG_AUDIO_TASK,"DestroyFastAudioPlayback");

  bFastPlaybackMode      = false;
  nFastPlaybackWindow    = 0;
  nFastPlaybackDropCount = 0;
  if(aFastPlaybackFilterConfig)
                    {
    QTV_Free(aFastPlaybackFilterConfig);
  }
  return;
                    }

// Modulus calculation without division operation
// Multiply by the inverse and take the mathematical floor of result
// Then multiply with the window value and subtract it from argument
#define QTV_AUDIOMGR_FAST_PLAYBACK_MODULUS(argX) \
        ((argX) - (((int)((float)(argX) * fFastPlaybackWindowInverse)) * nFastPlaybackWindow))
/* ======================================================================
FUNCTION
  AudioMgr:FastAudioPlaybackAdjustTS

DESCRIPTION
  Adjust the timestamp for fast audio playback if needed.

DEPENDENCIES
  None.

RETURN VALUE
  None..

SIDE EFFECTS
  None.

========================================================================== */
long AudioMgr::FastAudioPlaybackAdjustTS(long nTimestamp , int nFrame)
                    {
  long nTS = nTimestamp;

  if(bFastPlaybackMode && (nFastPlaybackWindow > nFastPlaybackDropCount))
                      {
    long nFramesToPlay = nFastPlaybackWindow - nFastPlaybackDropCount;
    int frame_duration = 0;
    int delta = 0;
    // Calculate the Frame duration
    if(clipInfo.fFramesPerMsec)
    {
      frame_duration = (int)(1/clipInfo.fFramesPerMsec);
      // Whenever frame 1 is played we add two times delta
      // Whenever frame 2 is played we add three times delta
      // Whenever frame 4 is played we add delta once
      if(nFramesToPlay)
      {
        delta =   frame_duration/nFramesToPlay;
                      }
      nTS  +=   (QTV_AUDIOMGR_FAST_PLAYBACK_MODULUS(nFrame)+1) * delta;
                    }

                  }
  return nTS;
}
/* ======================================================================
FUNCTION
  AudioMgr:CreateFastAudioPlaybackObject

DESCRIPTION
  Initialize the members related to the Fast Audio Playback.

DEPENDENCIES
  None.

RETURN VALUE
  None..

SIDE EFFECTS
  None.

========================================================================== */
void AudioMgr::CreateFastAudioPlaybackObject()
                  {

  QTV_MSG(QTVDIAG_AUDIO_TASK,"CreateFastAudioPlayback");

  bFastPlaybackMode         = false;
  nFastPlaybackWindow       = 0;
  nFastPlaybackDropCount    = 0;
  aFastPlaybackFilterConfig = NULL;

  return;
                  }


/* ======================================================================
FUNCTION
  AudioMgr:SetupFastAudioPlaybackFilters

DESCRIPTION
  Sets the filter configuration array for Fast playback mode.We can enhance this routine to
  support several different patterns of dropping in the future. Right now simplest configuration
  is assumed which is targeting to avoid consecutive drops.

DEPENDENCIES
  None.

RETURN VALUE
  true/false depending on whether the drop is necessary or not.

SIDE EFFECTS
  None.

========================================================================== */
void AudioMgr::SetupFastAudioPlaybackFilters()
                  {
  if(aFastPlaybackFilterConfig)
  {
    QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_ERROR,
     "Most Probably Stop Fast Playback Mode setting skipped???");
    // Free the old memory ; It should not happen if the resetting is done before.
    QTV_Free(aFastPlaybackFilterConfig);
                  }

  // Allocate the configuration array
  aFastPlaybackFilterConfig = (int *) QTV_Malloc(sizeof(int)*nFastPlaybackWindow);
  if(aFastPlaybackFilterConfig == NULL)
                  {
    QTV_MSG(QTVDIAG_AUDIO_TASK, "Memory Allocation failed; reverting to default machanism");
                  }
                  else
                  {
    int i,j;
    for(i=0;i<nFastPlaybackWindow;i++)
    {
      aFastPlaybackFilterConfig[i] = 0;
    }
    // Since the drop count is less than window size we wont exceed the array boundary for sure
    for(i=0,j=0;(j < nFastPlaybackDropCount);j++)
    {
      if(aFastPlaybackFilterConfig[i])
      {
        // If the modulus is odd number then we will jump across the squares nicely.
  // If the modulus is even number then we will get to an occupied square when the wrap around
  // happens . In that case we just need to jump the next one.
  i = (i+1)% nFastPlaybackWindow;
      }
      aFastPlaybackFilterConfig[i] = 1;
      i = (i+2) % nFastPlaybackWindow;
    }
  }
  return;
                  }

/* ======================================================================
FUNCTION
  AudioMgr:IsCurrentFrameDropped

DESCRIPTION
  Whether to drop the currrent frame or not.

DEPENDENCIES
  None.

RETURN VALUE
  true/false depending on whether the drop is necessary or not.

SIDE EFFECTS
  None.

========================================================================== */
bool AudioMgr::IsCurrentFrameDropped()
              {
  int nModulus;
  bool bRet = false;
  if(bFastPlaybackMode)
  {
    nModulus = QTV_AUDIOMGR_FAST_PLAYBACK_MODULUS((mediaInfo.nFramesProcessed + 1));
    if(aFastPlaybackFilterConfig)
    {
      bRet    =  (aFastPlaybackFilterConfig[nModulus] == 1);
              }
              else
              {
      // Fallback mechanism , in case malloc failed. Ofcourse that will not happen!!
      bRet = (nModulus < nFastPlaybackDropCount);
              }
            }
  if(bRet == true)
  {
    QTV_MSG1(QTVDIAG_AUDIO_TASK, "Frame %d getting dropped",
             mediaInfo.nFramesProcessed+1);
  }
  return bRet;
}

/* ======================================================================
FUNCTION
  AudioMgr:UpdateAudioBuffer

DESCRIPTION
  Update all the relevant data structures to take into account of the retrieved sample.
  Perform the book keeping for the recent audio sample retrieved.
  This routine also knows what things should be skipped when a frame is dropped due to
  fast audio playback.

DEPENDENCIES
  None.

RETURN VALUE
 .

SIDE EFFECTS
  None.

========================================================================== */
void AudioMgr::UpdateAudioBuffer(int nBytes, long nTimestamp,
                                 unsigned long nSampleCount,
                                 bool bPlay)
          {
   QTV_MSG3(QTVDIAG_AUDIO_TASK, "DBG_MSG: UpdateAudioBuffer %d,%d,%d",
      nBytes,nTimestamp,nSampleCount);

  Media::CodecType audioCodec = GetAudioCodecType();

  if(bPlay)
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
    //keep track of data in buffer.
    if ( audioBuffer.nSize==1 )
            {
      audioBuffer.nFirstFrame = mediaInfo.nFramesProcessed-nSampleCount+1;
      audioBuffer.nFirstTimestamp = nTimestamp;
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
      if( (m_bCurrentTSFromCMXBytesUpdatePending)&&
         IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
            {
        #ifdef T_MSM7500
#error code not present
        #endif /* T_MSM7500 */
        m_bCurrentTSFromCMXBytesUpdatePending = false;
            }
#endif
            }
          }

  // Check if the partial frame is completely consumed
  if((audioBuffer.nPartialFrame > 0) && (audioBuffer.nPendingData == 0))
  {
    audioBuffer.nPartialFrame = 0;
  }
  else
  {
#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
    if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec) && (nBytes > 0) )
    {
#if defined(FEATURE_QTV_WINDOWS_MEDIA)
      if( (Media::WMA_CODEC != GetAudioCodecType())|| (!isStartOfDummyBytes()) )
      {
#endif  /* FEATURE_QTV_WINDOWS_MEDIA */
      //You should add a data to the restore buffer only if it has been
      //read from the pending buffer or the file. Adding data that has been
      //read from the restore buffer again to the restore buffer may cause
      //repeat of the same audio data, jumps in playback time, etc.
      if( audioBuffer.nPartialFrame > 0 && (m_DataSrcFlag!=RESTORE))
      {
        RestoreDataBufferInsertSample(audioBuffer.nPartialFrame,nTimestamp,audioBuffer.nPartialSamples);
      }
      else if(m_DataSrcFlag != RESTORE)
      {
        RestoreDataBufferInsertSample(nBytes,nTimestamp,nSampleCount);
      }
#ifdef FEATURE_QTV_WINDOWS_MEDIA
      }
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
      if( RestoreDataBufferInfo.nEntriesToRestore > 0 )
      {
         RestoreDataBufferInfo.nEntriesToRestore--;
      }
    }
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */
  }

#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
    if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec) && (nBytes > 0) )
#if defined(FEATURE_QTV_WINDOWS_MEDIA)
    if( (Media::WMA_CODEC != GetAudioCodecType())|| (!isStartOfDummyBytes()) )
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
  {
    //Don't add entry in mapping table if it's a start of dummy bytes.
    //This is because DSP may not request all dummy bytes,then as we
    //calculate amount of data buffered based on number of entries in mapping table
    //we will think that there is some data buffered even when audio is ended.
    //This will cause jump in the progress bar timer towards the end of the clip.
      if(bPlay)
      {
        if( m_bFirstAudioFragment && (audioBuffer.nPartialFrame > 0))
    {
          //Audio frame can't fit into outgoing audio buffer
          //so it is being sent in multiple chunks
          AVSyncAudioBufferInsertEntry(audioBuffer.nPartialFrame,nTimestamp);
        }
        else if(m_bFirstAudioFragment)
      {
          //Audio frame can fit into outgoing buffer
        AVSyncAudioBufferInsertEntry(nBytes,nTimestamp);
      }
      }
      m_bFirstAudioFragment = false;
      // Entries are added to the restore buffer always
      // This is to take care of the normal transitions resulted from pause
      // Assumption is that the user release the DSP for PAUSE
      // If the user does not release the DSP then there will be som glitches due to the
      // queued up content to CMX
    }
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES */

  // Following Logic is performed at every frame boundary
  // Increments the number of frames processed un-conditionally
  // Insert into Restore data buffer un-conditionally
  // Insert into AVSync buffer only if the frame is not dropped.
  // Update the LastFrame & Last Timestamp values.

  if (!audioBuffer.nPartialFrame)
  {
    //count total frames
    mediaInfo.nFramesProcessed +=  nSampleCount;
    audioBuffer.nLastFrame = mediaInfo.nFramesProcessed;

    //Update timestamp if there are more than 1 sample/frames based on frames per msec.
    if(nSampleCount>1 && clipInfo.fFramesPerMsec)
    {
      audioBuffer.nLastTimestamp = (long)(nTimestamp + (nSampleCount-1)/clipInfo.fFramesPerMsec);
    }
    else
    {
      audioBuffer.nLastTimestamp = nTimestamp;
    }
  }


  return;

}


/* ======================================================================
FUNCTION
  AudioMgr::SetClientData

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
void AudioMgr::SetClientData(const void *client_data)
{
  playerData = client_data;
}


/* ======================================================================
FUNCTION
  AudioMgr::Stop

DESCRIPTION
//
// stop, and wait on completion.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioMgr::Stop()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::Stop");

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES

  /*
    Reset AV SYNC Mapping Table variables.In some cases,last entry from mapping table may not be
    removed because of the way we do mapping.
    (We take TS from the entry next to the matched index and entry gets flushed out only when
    feedback is greater than the byte count).If we don't do this, at the end of playback,
    we return elapsed time by subtracting buffered duration from last time stamp sent.
    buffered_duration is calculated based on number of entries in the mapping table.
    Thus,unless we do this, TS will jump back by one packet duration.
  */

  nAVSyncAudioBufferWriteIndex= 0;
  nAVSyncAudioBufferSize      = 0;
  nAVSyncAudioWriteByteOffSet = 0;

  //Get audio statistics  before tearing down CMX interface.
  //This will be used in audioMgr::GetElapsedTime if pAudioCMX is NULL.
  {
    GetStatistics(m_nAudioVideoStat,m_nAudioLastFrameStat);
  }

#endif //FEATURE_QTV_CMX_AV_SYNC_BYTES

  if ( pAudioCMX )
  {
    if ( pAudioCMX->Stop(audioTime.tStop) )
    {
      EndElapsedTime(audioTime.tStop);
      return true;
    }
    return false;
  }
  return true;
}

/* ======================================================================
FUNCTION
  AudioMgr::Abort

DESCRIPTION
//
// stop, but don't handshake with it.  use this for
// error cases where we're not sure audio is playing.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::Abort()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::Abort");

  if ( pAudioCMX )
  {
    pAudioCMX->Abort(audioTime.tStop);
    EndElapsedTime(audioTime.tStop);
  }
}


/* ======================================================================
FUNCTION
  AudioMgr::Pause

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
bool AudioMgr::Pause()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::Pause");

  long tPause;
  if ( pAudioCMX && pAudioCMX->Pause(tPause) )
  {
    EndElapsedTime(tPause);

    //pause timing.
    //be careful to do this only once per interval end.
    if ( !xferInfo.endOfInterval.bValid )
    {
      //get audio position when playback paused.
      long nCurTimestamp;
      float nCurFrame;
      if ( GetAudioPosition(tPause, nCurTimestamp, nCurFrame) )
      {
        xferInfo.endOfInterval.bValid = true;
        xferInfo.endOfInterval.time = tPause;
        xferInfo.endOfInterval.nFrame = (long)nCurFrame;
        xferInfo.endOfInterval.nFrameFraction = (int16)
           ((nCurFrame - xferInfo.endOfInterval.nFrame)*FRAME_FRACTION_BASE);
        xferInfo.endOfInterval.timestamp = nCurTimestamp;

#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
        Media::CodecType audioCodec = GetAudioCodecType();
        if(
           (Media::MP3_CODEC != audioCodec) &&
           (!IS_GENERIC_AUDIO_FORMAT(audioCodec)) &&
           (!IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec)) )
        {
          /* cmxPause is used for a/v sync callback support. If the callback support
          is available for a codec type, then cmx_avsync_update.curTimeStamp will have the
          current timestamp where we paused, so assign that value to end of interval
          timestamp */
          if(cmx_avsync_update.curTimeStamp !=0)
          {
            xferInfo.endOfInterval.timestamp=cmx_avsync_update.curTimeStamp;
            xferInfo.endOfInterval.nFrame = cmx_avsync_update.nFrames;

            /* whenever we resume after a pause, we update the start of interval timestamp
            to the timestamp at which we paused, whereas cmx will report the number of samples
            from the beginning of play or from the time we repositioned till we paused,
            so we need to store the startofinterval timestamp in a variable only for the first
            time we paused */
            if(cmx_avsync_update.cmxPause==false)
            {
              cmx_avsync_update.cmxPause = true;
              cmx_avsync_update.prevResumedTimeStamp = xferInfo.startOfInterval.timestamp;
              cmx_avsync_update.prevResumedFrame = xferInfo.startOfInterval.nFrame;
            }
          }
        }
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
        if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
        {
          if(m_nCurrentTSFromCMXBytes > 0)
          {
            //Update the end interval time based on feedbacked time stamp.
            xferInfo.endOfInterval.timestamp = m_nCurrentTSFromCMXBytes;
            QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,"AudioMgr::Pause, xferInfo.endOfInterval.timestamp updating with m_nCurrentTSFromCMXBytes %ld",xferInfo.endOfInterval.timestamp);
          }
        }
#endif
#endif


#ifdef SHOW_TIMING
        QTV_MSG( QTVDIAG_AUDIO_TASK, "  Start Interval");
        ShowInterval(xferInfo.startOfInterval);
        QTV_MSG( QTVDIAG_AUDIO_TASK, "  End Interval");
        ShowInterval(xferInfo.endOfInterval);
#endif /* SHOW_TIMING */
      }
    }
    return true;
  }
  return false;
}

#ifdef FEATURE_QTV_DYNAMIC_AAC_ADTS
#error code not present
#endif

/* ======================================================================
FUNCTION
  AudioMgr::Start

DESCRIPTION
//
// start audio and return TRUE if it started.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#else
bool AudioMgr::Start()
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::Start");

  //init tick and watchdog control
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */

  //This is a new playback
  InitAVTiming();

  // If the boolean lazy start is set to true , then  dont
  // start the audio Decoder/DSP component. This will be done
  // Once we receive the first sample with ADTS Header.
  // Please note that the restore data buffer is initialized
  // assuming successful start.
  if ( m_bADTSLazyStart || (pAudioCMX && pAudioCMX->Start(audioTime.tStart)) )
  {
    StartElapsedTime();

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
    Media::CodecType audioCodec = GetAudioCodecType();
    if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
    {
      m_bCurrentTSFromCMXBytesUpdatePending = true;
    }
#endif

#ifdef SHOW_TIMING
    QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::Start %d",ZUtils::Clock());
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  /*
  *  MAX entries that could be in RestoreDataBufferInfo is
  *  MAX_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER.
  *  Make sure max number of entries for given audio codec never exceed
  *  MAX number of entries that RestoreBuffer could hold.
  *  This will cause the overflow and thus the crash.
  *  Print a FATAL message to report the ERROR.
  */
  if(MAX_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER < NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec])
  {
     QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK,
                   QTVDIAG_PRIO_FATAL,
                   "FATAL ERROR:MAX #Entries allowed %d,audiocodec %d #Entries configured %d",
                   MAX_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER,
                   audioCodec,
                   NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec]);
  }
  else
  {
    if( IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec) )
    {
      /*
      * getRestoreDatabufferEntrySize returns an entry size needed for
      * given audio codec. When same clip is being played,
      * value returned from GetRestoreDataBufferEntrySize will be same as
      * RestoreDataBufferInfo.nSizeOfEachEntry.
      * This will not be the case if user is attempting to play
      * different clip with out doing QtvPlayer::Terminate.
      */
      int nCurrEntrySizeNeeded = GetRestoreDataBufferEntrySize(audioCodec);
      /*
      * Check if memory is already allocated
      * for restore data buffer.
      */
      if(RestoreDataBufferInfo.bRestoreBufferValid)
      {
        /*
        * When RestoreDataBufferInfo.bRestoreBufferValid is TRUE
        * RestoreDataBufferInfo.nSizeOfEachEntry gives size of each entry
        * in restore data buffer.However,if nCurrEntrySizeNeeded returned above
        * is different than RestoreDataBufferInfo.nSizeOfEachEntry it means
        * we are playing another clip with out doing QtvPlayer::Terminate,
        * and we need restore buffer of different size.
        * In such case, destroy the old buffer and allocate new one.
        */

        if(RestoreDataBufferInfo.nSizeOfEachEntry != nCurrEntrySizeNeeded)
        {
          QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,
                        QTVDIAG_PRIO_HIGH,
                        "Deleting existing restore buffer B4 doing malloc nCurrEntrySizeNeeded %d",
                        nCurrEntrySizeNeeded);
          DestroyRestoreDataBufferInfo();
        }
      }
      if(!RestoreDataBufferInfo.bRestoreBufferValid)
      {
        RestoreDataBufferInfo.nEntriesInRestoreBuffer = 0;
        RestoreDataBufferInfo.nWriteIndex = 0;
        RestoreDataBufferInfo.nReadIndex = 0;
        RestoreDataBufferInfo.nEntriesToRestore = 0;
        int iterator = 0;

        for(iterator =0; iterator <NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec]; iterator++)
        {
          if(nCurrEntrySizeNeeded > 0)
          {
            if (RestoreDataBufferInfo.UnAckDataBuffer[iterator].pDataPtr == NULL)
            {
              RestoreDataBufferInfo.UnAckDataBuffer[iterator].pDataPtr = (unsigned char*)QTV_Malloc(nCurrEntrySizeNeeded);
            }
          }
          if(RestoreDataBufferInfo.UnAckDataBuffer[iterator].pDataPtr == NULL)
          {
            RestoreDataBufferInfo.bRestoreBufferValid = false;

            //If we can't allocate memory for the entire restore data buffer,
            //free up the memory allocated so far, set RestoreDataBufferInfo.bRestoreBufferValid to FALSE and break out of the loop.
            for(int index = iterator; index >= 0; --index)
            {
              if(RestoreDataBufferInfo.UnAckDataBuffer[index].pDataPtr != NULL)
              {
                QTV_Free(RestoreDataBufferInfo.UnAckDataBuffer[index].pDataPtr);
                RestoreDataBufferInfo.UnAckDataBuffer[index].pDataPtr = NULL;
              }
            }
            break;
          }
          memset(RestoreDataBufferInfo.UnAckDataBuffer[iterator].pDataPtr, 0, nCurrEntrySizeNeeded);
          RestoreDataBufferInfo.bRestoreBufferValid = true;

          //Update the size information in RestoreDataBufferInfo
          RestoreDataBufferInfo.nSizeOfEachEntry = nCurrEntrySizeNeeded;

        }//for(iterator =0; iterator <NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec]; iterator++)
      }//if(!RestoreDataBufferInfo.bRestoreBufferValid)
    }
    else
    {
      //If BYTE BASED AV SYNC is not supported for given audioCodec
      //Make all pDataPtr to NULL, set RestoreDataBufferInfo.bRestoreBufferValid to FALSE.
      for(int iterator =0; iterator <NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec]; iterator++)
      {
        RestoreDataBufferInfo.UnAckDataBuffer[iterator].pDataPtr = NULL;
        RestoreDataBufferInfo.bRestoreBufferValid = false;
      }
    }
  }
#endif
    return true;
  }
  return false;
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
bool AudioMgr::Resume(bool bRestart)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::Resume %d", bRestart);

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
    /* Resume after a reposition */
    InitAVTiming();
#ifdef  FEATURE_QTV_CMX_AV_SYNC_BYTES
    Media::CodecType audioCodec = GetAudioCodecType();
    if(IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))
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
    StartElapsedTime();
  }

  return bOK;
}

/* ======================================================================
FUNCTION
  AudioMgr::SetPlayTimes

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
void AudioMgr::SetPlayTimes(long play,long stop)
{
  if ( play>=0 )
  {
    nStartTime=play;
    if ( pAudioCMX )
    {
      pAudioCMX->SetPlayTime(play);
    }
  }
  if ( stop<0 )
  {
    stopTime.bStop=false;
  }
  else
  {
    stopTime.nStop=(unsigned long)stop;
    stopTime.bStop=true;
  }
}

/* ======================================================================
FUNCTION
  AudioMgr::SetTimeStamp

DESCRIPTION
  Right now we use this function to update the m_nCurrentTSFromCMXBytes
  If byte based A/V Sync is enabled. We may use this in future for sample based
  A/V Sync also if required.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::SetTimeStamp(int32 timestamp)
{
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  QCUtils::EnterCritSect(&audioMgrCSforTS);
     m_nCurrentTSFromCMXBytes = timestamp;
     QTV_MSG1( QTVDIAG_AUDIO_TASK, "AudioMgr::SetTimeStamp %d",m_nCurrentTSFromCMXBytes);
  QCUtils::LeaveCritSect(&audioMgrCSforTS);
#endif //(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
}

/* ======================================================================
FUNCTION
  AudioMgr::InitAVTiming

DESCRIPTION
//
//  init timing before starting play.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::InitAVTiming()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::InitAVTiming");

  xferInfo.nBuffersSentInInterval = 0;
  xferInfo.tStartInterval=ZUtils::Clock();
  xferInfo.nLastTimestampSent = 0;
  xferInfo.nLastFrameSent=0;
  xferInfo.bTimingUpdatedInInterval=false;
  xferInfo.startOfInterval.bValid = false;
  xferInfo.endOfInterval.bValid = false;
  xferInfo.nBytesSent=0;
}


#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES

/* ======================================================================
FUNCTION
  AudioMgr::InitAVSyncAudioBuffer

DESCRIPTION
//
//  init timing before starting play.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

INPUT/OUTPUT:
  bFlushRestoreBuffer to indicate whether restore buffer to be flushed out.
RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
 ========================================================================== */
 void AudioMgr::InitAVSyncAudioBuffer(bool bFlushRestoreBuffer)
 {
   QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::InitAVSyncAudioBuffer");
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
   if(qtv_cfg_enable_dsp_release)
   {
     /*
     * qtv_cfg_enable_dsp_release TRUE indicates that audio interface is just being
     * created. This could be the fresh start, or resuming after a voice call
     * or releasing the DSP.
     * In such case, RestoreDataBufferInfo.nEntriesInCallDataBuffer gives
     * audio data entries that were sent to CMX but were not acknowledged
     * via DSP feedback.This data would have been flushed out from CMX as a
     * result of releasing the DSP.
     * So, record number of entries in RestoreBuffer.
     * These entries will be restored via AudioMgr::FillAudioBuffer
     * when audio resumes.This will avoid the loss of audio data.
     */
     RestoreDataBufferInfo.nEntriesToRestore = RestoreDataBufferInfo.nEntriesInRestoreBuffer;
     if((int)RestoreDataBufferInfo.nEntriesToRestore < 0)
     {
       RestoreDataBufferInfo.nEntriesToRestore = 0;
     }
     RestoreDataBufferInfo.nTmpReadIndex = RestoreDataBufferInfo.nReadIndex;
     QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "AudioMgr::InitAVSyncAudioBuffer:RestoreDataBufferInfo.nEntriesToRestore= %ld ",RestoreDataBufferInfo.nEntriesToRestore);
   }
   else
#endif
   {
#ifndef FEATURE_QTV_QDSP_RELEASE_RESTORE
      if(!bFlushRestoreBuffer)
      {
        /*
        * bFlushRestoreBuffer FALSE indicates that
        * audio interface is just created.This could be
        * fresh start, or resuming
        * after releasing DSP as a result of incoming call
        * or normal pause|resume with DSP release.
        *
        * Record number of entries in RestoreBuffer.
        * RestoreDataBufferInfo.nEntriesInRestoreBuffer gives # entries in
        * mapping table that have not been acknowledged
        * by DSP.These entries will be restored via
        * AudioMgr::FillAudioBuffer when audio resumes.
        */
        RestoreDataBufferInfo.nEntriesToRestore = RestoreDataBufferInfo.nEntriesInRestoreBuffer;
        if((int)RestoreDataBufferInfo.nEntriesToRestore < 0)
        {
          RestoreDataBufferInfo.nEntriesToRestore = 0;
        }
        RestoreDataBufferInfo.nTmpReadIndex = RestoreDataBufferInfo.nReadIndex;
        QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK,
                      QTVDIAG_PRIO_HIGH,
                      "AudioMgr::InitAVSyncAudioBuffer:RestoreDataBufferInfo.nEntriesToRestore= %ld ",RestoreDataBufferInfo.nEntriesToRestore);
      }
      else
#endif
      {
        //Reset all RestoreBuffer variables.
        RestoreDataBufferInfo.nEntriesToRestore = 0;
        RestoreDataBufferInfo.nTmpReadIndex = -1;
        RestoreDataBufferInfo.nEntriesInRestoreBuffer = 0;
        RestoreDataBufferInfo.nWriteIndex = 0;
        RestoreDataBufferInfo.nReadIndex = 0;
      }
   }
   //Reset AV SYNC Mapping Table variables irrespective of the incoming call or not.
   nAVSyncAudioBufferWriteIndex= 0;
   nAVSyncAudioBufferSize      = 0;
   nAVSyncAudioWriteByteOffSet = 0;
 }
#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */

  /* ======================================================================
FUNCTION
  AudioMgr::UpdateAVSyncFromCMXBytes

DESCRIPTION
This function is called from the cmx callback function to
modify the offset, this will determine the sleep time of video

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef T_MSM7500
#error code not present
#else /* T_MSM7500 */
void AudioMgr::UpdateAVSyncFromCMXBytes(long num_of_bytes)
#endif /* T_MSM7500 */
{
   int32 tempTimeStamp = -1;
   bool bGotTimeStamp = false;
   int nFrame = -1;

   QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "AVSyncAudioBuffer size before AVSync Set:%d",nAVSyncAudioBufferSize );
   tempTimeStamp = GetTimeStampFromCMXBytes(num_of_bytes,nFrame);

   #ifdef T_MSM7500
#error code not present
   #endif /* T_MSM7500 */

   if(tempTimeStamp >= 0)
   {
     // nFrame key is added to each and every entry of the AVSync audio buffer
     // This is added to intelligently pad the timestamps
     // If we dont do this then the timestamps will jump suddenly
     int32 ts;
     ts = FastAudioPlaybackAdjustTS(tempTimeStamp,nFrame);
     QCUtils::EnterCritSect(&audioMgrCSforTS);
        m_nCurrentTSFromCMXBytes = ts;
     QCUtils::LeaveCritSect(&audioMgrCSforTS);
     bGotTimeStamp = true;
   }
   if( m_nCurrentTSFromCMXBytes > 0)
   {
     /* Here we need to update the elapsedTime because we are flushing
        this m_nCurrentTSFromCMXBytes in FlushRestoreDataBufferInfo.
        This FlushRestoreDataBufferInfo is called once we issue pause.
        This is causing us to return 0 as elapsedtime after pause and
        before playing.
     */
     cmx_avsync_update.elapsedTime = m_nCurrentTSFromCMXBytes;
   }

   QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "m_nCurrentTSFromCMXBytes is %d",m_nCurrentTSFromCMXBytes);

   if(bGotTimeStamp)
   {
     /* now update AVSync offset as the current time - current timestamp */
#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#else
     SetPlaybackOffset(ZUtils::Clock(),m_nCurrentTSFromCMXBytes);
     QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "AVSyncAudioBuffer size after AVSync Set: %d",nAVSyncAudioBufferSize );
#endif
   }
}

/* ======================================================================
FUNCTION
  AudioMgr::GetTimeStampFromCMXBytes

DESCRIPTION
This function is called from the cmx callback function to
modify the offset, this will determine the sleep time of video

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int32 AudioMgr::GetTimeStampFromCMXBytes(long num_of_bytes, int &nFrame)
{
    //Some of the following messages are only for the purpose of debugging//
    //Once the code in this routine gets stabilize, we have to remove them//
    int16 nTopIndex;
    int16 nMiddleIndex;
    int16 nBottomIndex;
    int16 nLastIndex = -1;
    int32 nSize = nAVSyncAudioBufferSize;
    int nMatchedIndex = 0;
    int32 nRet = 0;
    float nTimeDiff = 0;
    float nByteDiff = 0;
    float nTime = 0;
    int   nPrevIndex = 0;
    bool  bNotExactMatch = false;

    /* A Binary Search to find the time stamp from number of bytes (processed by CMX/DSP) */

    QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
    "AudioMgr::GetTimeStampFromCMXBytes: nSize = nAVSyncAudioBufferSize is %d",nSize);

    if(nSize == 0)
    {
      QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
      "GetTimeStampFromCMXBytes returing -1 as mapping table is empty, nSize %d",nSize);
      nFrame = -1;
      return -1;
    }

    // Calculate the Top & Bottom
    nTopIndex    = nAVSyncAudioBufferWriteIndex - nSize;
    if(nTopIndex < 0)
    {
      nTopIndex += MAX_AVSYNC_AUDIO_BUFFER_SIZE;
    }


      nBottomIndex = nAVSyncAudioBufferWriteIndex-1;
    if(nBottomIndex < 0)
    {
      nBottomIndex += MAX_AVSYNC_AUDIO_BUFFER_SIZE;

    }
    // At present DSP feedback is an even number
    // In future if they change this , this logic is equally applicable
    // loosing least significant bytes is to align to the word boundary.
    num_of_bytes &= (~1);

    //DEBUG PURPOSE ONLY///

    /*
         * QTV_MSG_PRIO3(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
         * "nTopIndex:%d and nMiddleIndex %d nBottomIndex:%d",nTopIndex,nMiddleIndex,nBottomIndex);
         * QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
         * "AVSyncAudioBuffer[nTopIndex].nByteOffSet %d AVSyncAudioBuffer[nBottomIndex].nByteOffSet %d",
         * AVSyncAudioBuffer[nTopIndex].nByteOffSet,AVSyncAudioBuffer[nBottomIndex].nByteOffSet );
         * QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
          * "AVSyncAudioBuffer[nMiddleIndex].nByteOffSet %d",AVSyncAudioBuffer[nMiddleIndex].nByteOffSet );
    */

    //When Virtual packet length is multiple of blockAlign bytes,
    //DSP will report feedback for each block Align bytes. In this case,
    //all the feedbacks with fewer number of bytes than the virtual
    //packet length will be mapped to the same time stamp and we won't flush
    //this entry unless we get a feedback which has greater than or equal
    //to virtual packet length bytes.
    if( num_of_bytes < (long)(AVSyncAudioBuffer[nTopIndex].nByteOffSet & (~1)))
    {
      QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "First entry TS..: %ld",AVSyncAudioBuffer[nTopIndex].nTimeStamp);
      /* calculating the exact time stamp if the feedback is less than the first
      entry in the mapping table. */
      if( nTopIndex + 1 <= nBottomIndex)
      {
        /* firstEntryOffset : the byte offset of the just before the current
                              top entry
           bFirstEntryPlayedOut : Flag to find out if the very first entry after starting
                                  playback(fresh start/ pause-resume/seek) has been played out
                                  Only if this is set to true , we can access the entry before the current top entry
        */
        nFrame = -1;
        nTimeDiff = AVSyncAudioBuffer[nTopIndex + 1].nTimeStamp - AVSyncAudioBuffer[nTopIndex].nTimeStamp;
        if (!bFirstEntryPlayedOut && !firstEntryOffset)
        {
           firstEntryOffset = AVSyncAudioBuffer[nTopIndex].nByteOffSet;
        }

        if (num_of_bytes > firstEntryOffset)
        {
           bFirstEntryPlayedOut = true;
        }
        if(bFirstEntryPlayedOut)
        {
          if(nTopIndex != 0)
          {
            firstEntryOffset = AVSyncAudioBuffer[nTopIndex-1].nByteOffSet;
          }
          else
          {
            firstEntryOffset = AVSyncAudioBuffer[nTopIndex-1+MAX_AVSYNC_AUDIO_BUFFER_SIZE].nByteOffSet;
          }
          QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "First Entry Offset = %d",firstEntryOffset);
          num_of_bytes -=firstEntryOffset;
        }
        if (firstEntryOffset != AVSyncAudioBuffer[nTopIndex].nByteOffSet)
        {
          nByteDiff = (AVSyncAudioBuffer[nTopIndex].nByteOffSet & (~1))- firstEntryOffset;
        }
        else
        {
          nByteDiff = (AVSyncAudioBuffer[nTopIndex].nByteOffSet &(~1));
        }
        nTime = (nTimeDiff/nByteDiff)*(num_of_bytes) + AVSyncAudioBuffer[nTopIndex].nTimeStamp;

      if( (nTime < AVSyncAudioBuffer[nTopIndex + 1].nTimeStamp) && (nTime >= AVSyncAudioBuffer[nTopIndex].nTimeStamp) )
      {
          QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "nTime = %d lies in between the top 2 entries",nTime);
        return nTime;
      }
      else
      {
          QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "Failed to interpolate between top 2 entries");
        return AVSyncAudioBuffer[nTopIndex].nTimeStamp;
      }
    }
    if(bFirstEntryPlayedOut)
    {
      if(nTopIndex != 0)
      {
        firstEntryOffset = AVSyncAudioBuffer[nTopIndex-1].nByteOffSet;
      }
      else
      {
        firstEntryOffset = AVSyncAudioBuffer[nTopIndex-1+MAX_AVSYNC_AUDIO_BUFFER_SIZE].nByteOffSet;
      }
      QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "First Entry Offset = %d",firstEntryOffset);
      num_of_bytes -=firstEntryOffset;
     }
     else
     {
       firstEntryOffset = 0;
     }
     if(AVSyncAudioBuffer[nTopIndex].nSampleDelta)
     {
       nTimeDiff = AVSyncAudioBuffer[nTopIndex].nSampleDelta;
     }
     else
     {
       QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "returning AVSyncAudioBuffer[nTopIndex].nTimeStamp:%d",AVSyncAudioBuffer[nTopIndex].nTimeStamp);
       return AVSyncAudioBuffer[nTopIndex].nTimeStamp;
     }
     nByteDiff = AVSyncAudioBuffer[nTopIndex].nByteOffSet - firstEntryOffset;
     nTime = (nTimeDiff/nByteDiff)*num_of_bytes + AVSyncAudioBuffer[nTopIndex].nTimeStamp;

     QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH, "returning timestamp calculated from delta %d",nTime);
     return nTime;
   }

    if((AVSyncAudioBuffer[nBottomIndex].nByteOffSet & (~1))< (uint32)num_of_bytes)
    {
      //Since we don't add mapping table entry for dummy bytes,
      //we will get the feedback when DSP consumes dummy bytes
      //which will be outside of our mapping table.
      // Flush AVSync buffer in this case and return -1;
      //Discard the feedback is it lies outside the mapping table/////?????
      //Need to double check ////////////////////////////////////
      QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
      "Not calling InitAVSyncAudioBuffer as feedback is outside the mapping table.");
      QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
      "UNABLE TO FIND BYTE OFFSET SENT BY CMX OFFSET= %d",nAVSyncAudioBufferSize );
      nFrame = -1;
      return -1;
    }

    for(;nTopIndex != nBottomIndex;)
    {
      // Calculate the Mid Index for the circular buffer
      if(nBottomIndex > nTopIndex)
      {
        // Normal scenario
        nMiddleIndex = (nTopIndex + nBottomIndex)/2;
      }
      else
      {
        // Wrap around scenario
        nSize = MAX_AVSYNC_AUDIO_BUFFER_SIZE - nTopIndex;
        nSize += nBottomIndex + 1;
        // TopIndex + size -1 will give the imaginary index of the last entry
        nMiddleIndex = (nTopIndex + ((nSize - 1)/2)) % MAX_AVSYNC_AUDIO_BUFFER_SIZE;

      }

      if((AVSyncAudioBuffer[nMiddleIndex].nByteOffSet & (~1)) == (uint32)num_of_bytes)
      {
        // Store the middle index into last index in case the buffer size reduced to zero
        nLastIndex    = nMiddleIndex;
        //#ifndef T_MSM7500
        nMatchedIndex = (nMiddleIndex + 1) % MAX_AVSYNC_AUDIO_BUFFER_SIZE;
        //#else /* T_MSM7500 */
        // In 7500 targets, feedback is not aligned to the frame boundaries and hence we
        // should not take the upper bound as done above. If we take the upper bound ,
        // then the error could be positive which makes the renderer to increase the sleep
        // time while waiting.
        //nMatchedIndex = nMiddleIndex;
        //#endif /* T_MSM7500 */
        break;
      }

      if((AVSyncAudioBuffer[nMiddleIndex].nByteOffSet & (~1)) > (uint32)num_of_bytes)
        {
        if(nMiddleIndex == nTopIndex)
        {
          nBottomIndex = nTopIndex;
      }
      else
      {
          nBottomIndex = nMiddleIndex -1;
          if(nBottomIndex < 0)
        {
            nBottomIndex += MAX_AVSYNC_AUDIO_BUFFER_SIZE;
      }
    }
    }
      else
    {
        if(nMiddleIndex == nBottomIndex)
      {
          nTopIndex = nBottomIndex;
      }
      else
      {
          nTopIndex = (nMiddleIndex + 1) % MAX_AVSYNC_AUDIO_BUFFER_SIZE;
      }
    }
    }

    if(nTopIndex == nBottomIndex)
    {
      // We didnt get the exact entry ; However search narrowed down to this one
      // byte count lies within this range
      nLastIndex = nMatchedIndex = nTopIndex;
      //#ifndef T_MSM7500
      // In 7500 targets, feedback is not aligned to the frame boundaries and hence we
      // should not take the upper bound as done here. If we take the upper bound ,
      // then the error could be positive which makes the renderer to increase the sleep
      // time while waiting.
      if((AVSyncAudioBuffer[nMatchedIndex].nByteOffSet & (~1)) == (uint32)num_of_bytes)
      {
        nMatchedIndex = (nMatchedIndex + 1) % MAX_AVSYNC_AUDIO_BUFFER_SIZE;
      }
      else if((AVSyncAudioBuffer[nMatchedIndex].nByteOffSet & (~1)) < (uint32)num_of_bytes)
      {
        // move the matched Index by one
        nPrevIndex     = nMatchedIndex;
        QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
          "nMatchedIndex ts = %d ",AVSyncAudioBuffer[nMatchedIndex].nTimeStamp);
        nMatchedIndex  = (nMatchedIndex + 1) % MAX_AVSYNC_AUDIO_BUFFER_SIZE;
        QTV_MSG_PRIO1(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
          "nMatchedIndex ts after increment= %d ",AVSyncAudioBuffer[nMatchedIndex].nTimeStamp);
        bNotExactMatch = true;
        QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
          "Setting bNotExactMatch = true ");
      }
      else
      {
        bNotExactMatch = true;
        nPrevIndex = nMatchedIndex -1;
      }

      // There is a possibility that previous for loop may not be executed
      // when there is only one element in the queue.
      if(nMatchedIndex ==nAVSyncAudioBufferWriteIndex)
      {
        nMatchedIndex = nLastIndex;
      }
      //#endif /* T_MSM7500 */
    }




    // Compute nAVSyncAudioBufferSize after flushing the the obsolate entries.
    if(nAVSyncAudioBufferWriteIndex > nMatchedIndex)
    {
      nAVSyncAudioBufferSize = nAVSyncAudioBufferWriteIndex - nMatchedIndex;
    }
    else if(nAVSyncAudioBufferWriteIndex == nMatchedIndex)
    {
      nAVSyncAudioBufferSize = 0;
    }
    else
    {
      nAVSyncAudioBufferSize = MAX_AVSYNC_AUDIO_BUFFER_SIZE - (nMatchedIndex-nAVSyncAudioBufferWriteIndex);
    }

    if(nAVSyncAudioBufferSize == 0)
    {
      nRet   = AVSyncAudioBuffer[nLastIndex].nTimeStamp;
      nFrame = AVSyncAudioBuffer[nLastIndex].nFrame;
      UpdateRestoreDataBuffer(nRet);
    }
    else
    {
      if( bNotExactMatch )
      {
        if(AVSyncAudioBuffer[nMatchedIndex].nSampleDelta)
        {
          nTimeDiff = AVSyncAudioBuffer[nMatchedIndex].nSampleDelta;
        }
        else if((0 == AVSyncAudioBuffer[nMatchedIndex].nSampleDelta) && (nMatchedIndex == nAVSyncAudioBufferWriteIndex -1))
        {
          nTimeDiff = 0;
        }
        else
        {
          nTimeDiff = AVSyncAudioBuffer[(nMatchedIndex+1)% MAX_AVSYNC_AUDIO_BUFFER_SIZE].nTimeStamp - AVSyncAudioBuffer[nMatchedIndex].nTimeStamp;
        }
        nByteDiff = (AVSyncAudioBuffer[nMatchedIndex].nByteOffSet & (~1))- (AVSyncAudioBuffer[nPrevIndex].nByteOffSet & (~1));
        nTime = (nTimeDiff/nByteDiff)*(num_of_bytes - AVSyncAudioBuffer[nPrevIndex].nByteOffSet & (~1));
        nRet  = AVSyncAudioBuffer[nMatchedIndex].nTimeStamp + (int32)nTime;
      }
      else
      {
         nRet   = AVSyncAudioBuffer[nMatchedIndex].nTimeStamp;
   QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
          "Returning Matched index timestamp. Marking first entry played out to true");
      }
      nFrame = AVSyncAudioBuffer[nMatchedIndex].nFrame;
      if(nFrame > 0)
      {
        nFrame--;
      }
      /**
      We always enter samples into restore data buffer at their frame boundsaries, when ever we get feedback
      not in frame boundaries, then we calculate the approximate timestamp based on the number of bytes played,
      this timestamp entry won't be there in restore data buffer so it's always good to pass next sample time
      stamp( At least This won't disturb the existing logic).
    */
      UpdateRestoreDataBuffer(AVSyncAudioBuffer[nMatchedIndex].nTimeStamp);
    }

    //Decremented in order to take care of the fact that previous frame is played out
    bFirstEntryPlayedOut = true;
    return nRet;
}

/* ======================================================================
FUNCTION
  AudioMgr::FlushRestoreDataBufferInfo

DESCRIPTION
Empties the restore data buffer

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::FlushRestoreDataBufferInfo()
{
  /*
  *FOR WMA, we always use m_nCurrentTSFromCMXBytes for audio playback time when QTV_CMX_AV_SYNC_BYTES
  *is defined. Earlier, we were resetting this variable in AudioMgr::InitPlayData which gets called during
  *normal pause and resume if qtv_cfg_enable_dsp_release is set to TRUE.
  *This was causing jump in the TS as max. data lead for WM is 2.4 seconds.
  *(Once m_nCurrentTSFromCMXBytes is reset to zero, we use last TS sent to CMX as the current playback time).

  *Removing this reset from AudioMgr::InitPlayData will take care of this problem, however,
  *after the playback is complete, if we don't exit QTV and try to play again, m_nCurrentTSFromCMXBytes will
  *have old value. So, we do it here as this gets called wheneevr we do reposition.
  */

  m_nCurrentTSFromCMXBytes = 0;
  #ifdef T_MSM7500
#error code not present
  #endif /* T_MSM7500 */
  if(RestoreDataBufferInfo.bRestoreBufferValid)
  {
    RestoreDataBufferInfo.nEntriesInRestoreBuffer = 0;
    RestoreDataBufferInfo.nWriteIndex = 0;
    RestoreDataBufferInfo.nReadIndex = 0;
    RestoreDataBufferInfo.nEntriesToRestore = 0;
    nLastWriteIndex = -1;
    QTV_MSG_PRIO(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,"AudioMgr::FlushRestoreDataBufferInfo");
  }
  SetMediaStatus(Media::DATA_OK);
}
/* ======================================================================
FUNCTION
  AudioMgr::UpdateRestoreDataBuffer

DESCRIPTION
Manages the restore data buffer.When a packet is sent to CMX, entry gets added into this buffer
and gets flushed out when feedback comes in from DSP.This buffer is used to restore data.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */

void AudioMgr::UpdateRestoreDataBuffer(int tsToMatched)
{
  int16 tempReadIndex = RestoreDataBufferInfo.nReadIndex;
  if(RestoreDataBufferInfo.bRestoreBufferValid)
  {
    int nToItr = RestoreDataBufferInfo.nEntriesInRestoreBuffer;
    int itr = 0;
    Media::CodecType audioCodec = GetAudioCodecType();
    int nMaxEntries = NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec];
    QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
                  "RestoreBuffer = (Rd = %d,Write = %d,Size = %d)",
                  RestoreDataBufferInfo.nReadIndex,
                  RestoreDataBufferInfo.nWriteIndex,
                  RestoreDataBufferInfo.nEntriesInRestoreBuffer);
    while( itr < nToItr)
    {
      if(tempReadIndex >= nMaxEntries)
      {
        tempReadIndex = 0;
      }
      if( RestoreDataBufferInfo.UnAckDataBuffer[tempReadIndex].nPresTime == tsToMatched)
      {
        QTV_MSG_PRIO2(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
                      "Located TS %d @ index %d ",tsToMatched,tempReadIndex);
        tempReadIndex++;
        if(tempReadIndex >= nMaxEntries)
        {
          tempReadIndex = 0;
        }
        // Update the circular buffer  size
        if( tempReadIndex <= RestoreDataBufferInfo.nWriteIndex)
        {
          // Case 1. Without circular wrap around   (WriteIndex >= ReadIndex)
          RestoreDataBufferInfo.nEntriesInRestoreBuffer = RestoreDataBufferInfo.nWriteIndex-
            tempReadIndex;
        }
        else
        {
          // Case 2. With Circular Wrap around (WriteIndex < ReadIndex)
          RestoreDataBufferInfo.nEntriesInRestoreBuffer = nMaxEntries - tempReadIndex +
            RestoreDataBufferInfo.nWriteIndex;
        }

        RestoreDataBufferInfo.nReadIndex = tempReadIndex;      // Update ReadIndex
        QTV_MSG_PRIO3(QTVDIAG_AUDIO_TASK, QTVDIAG_PRIO_HIGH,
                  "Updated RestoreBuffer = (Rd = %d,Write = %d,Size = %d)",
                  RestoreDataBufferInfo.nReadIndex,
                  RestoreDataBufferInfo.nWriteIndex,
                  RestoreDataBufferInfo.nEntriesInRestoreBuffer);
  break;
      }
      itr++;
      tempReadIndex++;
    }
  }
}
#endif

#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
/* ======================================================================
FUNCTION
  AudioMgr::SetCurTimeStamp

DESCRIPTION
This function is called from the cmx callback function to
modify the offset, this will determine the sleep time of video

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::SetCurTimeStamp(uint64 num_of_samples)
{
 /* execute this function for local file playback and streaming*/
 long nFrames;
 long timestamp;
 int32 ChannelSwitchDelay = -1;

 if( (mediaInfo.dataState == Media::DATA_END)&&
     (num_of_samples == 0) )
 {
   /*
   * At the end of audio stream playback, we get A/V SYNC feedback with 0 samples
   * as we issue flush/stop to the CMX.
   * In case of A+V clip with audio track shorted than video track, this updates
   * the A/V offset as audio presentation time becomes 0 and
   * hence video freezes for the long time.
   *
   * Thus, we should discard the feedback(not update the a/v offset)if audio data state is DATA_END
   * and feedback reports 0 number of samples.
   */
   QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH, "SetCurTimeStamp detected DATA_END and num_of_samples is 0..Discarding feedback");
   return;
 }
 if(cmx_avsync_update.bCmxFlushed == true)
 {
   //FLUSH was done so
   //do bound checking on number of samples reported via feedback.
   if( /*For non AAC codecs*/(num_of_samples != 1) &&
       /*For AAC codecs    */(num_of_samples != 0))
   {
     QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
       "SetCurTimeStamp,Discarding feedback as cmx_avsync_update.bCmxFlushed is TRUE and num of samples %d",num_of_samples);
    return;
   }
 }
 cmx_avsync_update.bCmxFlushed = false;


 /* the current time is obtained */
 cmx_avsync_update.curTime = ZUtils::Clock();

#if(defined FEATURE_QTV_CMX_AV_SYNC_INTERFACE || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
     if(GetAudioCodecType() == Media::AAC_CODEC)
#endif /* FEATURE_QTV_BSAC */
 {
   /*this compensation is being done for low sampling rate AAC clips till the DSP image
   with the correction becomes available */
   if(audioInfo.nSamplingRate == 8000)
   {
     if(num_of_samples > (2*1024))
       nFrames = (long)((float)(num_of_samples - (2*1024))/audioInfo.nSamplesPerFrame);
     else
       nFrames = 0;
   }
   else
   {
     if(num_of_samples > 1024)
       nFrames = (long)((float)(num_of_samples - 1024)/audioInfo.nSamplesPerFrame);
     else
       nFrames = 0;
   }

   timestamp = (long)((float)nFrames/clipInfo.fFramesPerMsec);
 }
 else
 {
   /* For speech codecs (AMR/EVRC/QCELP), in AV SYNC callback, CMX returns number of frames played (not number of
   samples as in case of AAC). Some time when we read one access unit from Mpeg4File, it returns multiple frames, so
   we should account for that as well).Here num_of_samples is number of audio frame played by CMX and nFrames is
   number of access unit which we read from Mpeg4File. */
   nFrames = (long)num_of_samples / (audioInfo.nSamplesPerFrame/Media::AMR_SAMPLES_PER_FRAME);

   /* Duration of one audio frame for speech codec is fixed at 20 ms */
   timestamp = (long)num_of_samples * 20;
 }
#else
 /* convert the number of samples passed to number of frames */
 nFrames = (float)num_of_samples/audioInfo.nSamplesPerFrame;
 timestamp = (float)nFrames/clipInfo.fFramesPerMsec;
#endif

 if( (long)audioBuffer.nLastFrame < nFrames )
 {
   QTV_MSG1(QTVDIAG_AUDIO_TASK,"DISCARDED CMX Feedback num of samples:%d",num_of_samples);
   QTV_MSG2( QTVDIAG_AUDIO_TASK, "audioBuffer.nLastFrame %d nFrames %d",audioBuffer.nLastFrame,nFrames);
   return;
 }

 QTV_MSG1(QTVDIAG_AUDIO_TASK,"cmx api:num of samples:%d",num_of_samples);
 QTV_MSG1(QTVDIAG_AUDIO_TASK,"cmx api:num of frames:%d",nFrames);
 QTV_MSG1(QTVDIAG_AUDIO_TASK,"cmx api:timestamp:%d",timestamp);

 /* for cases of normal play or reposition without pause and resume, compute the
 current timestamp as start of interval timestamp + the timestamp obtained from
 the number of samples and frames because the number of samples that are reported
 by cmx are from the point where we started playing the clip or from the point we
 repositioned.
 Incase of pause resume, compute the current timestamp as prevResumedTimeStamp +
 the computed timestamp from the samples reported by cmx. This is because when
 we resume, we modify the start of interval timestamp, whereas the timestamp computed
 from the number of samples reported by cmx is the total timestamp from the
 beginning of play or from the point we last repositioned. This is to avoid ending up
 with twice the timestamp */
 if((cmx_avsync_update.cmxPause==true))
 {
   cmx_avsync_update.curTimeStamp = cmx_avsync_update.prevResumedTimeStamp + timestamp;
   cmx_avsync_update.nFrames = cmx_avsync_update.prevResumedFrame + nFrames;
 }
 else
 {
   cmx_avsync_update.curTimeStamp = xferInfo.startOfInterval.timestamp + timestamp;
   cmx_avsync_update.nFrames = xferInfo.startOfInterval.nFrame + nFrames;
 }

 QTV_MSG2(QTVDIAG_AUDIO_TASK,"cmx api:time:%d timestamp:%d",cmx_avsync_update.curTime,
   cmx_avsync_update.curTimeStamp);
 QTV_MSG1(QTVDIAG_AUDIO_TASK,"cmx api:offset:%d",cmx_avsync_update.curTime -
   cmx_avsync_update.curTimeStamp);

 if(pMpeg4 != NULL)
 {
   if( (pMpeg4->GetSource() == Media::BCAST_FLO_SOURCE) && (CheckForChannelSwitchFrame == true))
   {
     ChannelSwitchDelay = cmx_avsync_update.curTimeStamp - AudioChannelSwitchedTS;
     if(ChannelSwitchDelay >= 0)
     {
#ifndef FEATURE_WINCE
       QTV_MSG2( QTVDIAG_AUDIO_TASK,                                      \
         "First Audio frame after channel switch rendered "       \
         " CurrentTS = %d, ChannelSwitchedTS = %d",               \
         cmx_avsync_update.curTimeStamp,AudioChannelSwitchedTS);
#endif
       Audioeventpayload.ChannelSwitchedTS =  AudioChannelSwitchedTS;
       Audioeventpayload.ApproxDelay = ChannelSwitchDelay;
#ifndef FEATURE_WINCE
       event_report_payload(EVENT_QTV_AUDIO_CHANNEL_SWITCH_FRAME,
         sizeof(qtv_event_Audiochannelswitch_frame),&Audioeventpayload);
#endif
       LogAudioChannelSwitch(ChannelSwitchDelay);
       CheckForChannelSwitchFrame = false;
     }
   }
 }

 /* now compute offset as the current time - current timestamp */
 SetPlaybackOffset(cmx_avsync_update.curTime,cmx_avsync_update.curTimeStamp);
}
#endif



/* ======================================================================
FUNCTION
  AudioMgr::LogAudioChannelSwitch

DESCRIPTION
  Logs the Audio channel switch event with approxmate delay.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  None.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */

void AudioMgr::LogAudioChannelSwitch(uint32 TimeDiff)
{
#ifndef PLATFORM_LTK
  log_qtv_audio_channel_type *pBuf = NULL ;


  /* Check if logging is turned on */
  if (!log_status( LOG_QTV_AUDIO_CHANNEL_SWITCH_FRAME_C ) )
  {
    return;
  }
  /* Allocate a buffer */
  pBuf = (log_qtv_audio_channel_type*) log_alloc( LOG_QTV_AUDIO_CHANNEL_SWITCH_FRAME_C, sizeof(*pBuf) );

  if ( pBuf )
  {
    /* Fill in the statistics */
    /* Log the buffer */
    pBuf->ChannelSwitchedTS = AudioChannelSwitchedTS;
    pBuf->ApproxDelay = TimeDiff;
    log_commit( pBuf );
  }
#endif // PLATFORM_LTK
}


/* ======================================================================
FUNCTION
  AudioMgr::GetAudioPosition

DESCRIPTION
//
// Calculate audio position at "time" assuming audio is playing.
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool AudioMgr::GetAudioPosition(long iTime, long &nCurTimestamp,
  float &nCurFrame)
{
  // Our calculation assumes the following:
  // 1) audio timestamps are continuous.
  // 2) clock is accurate relative to audio device playback rate.
  if ( xferInfo.startOfInterval.bValid )
  {
    // Calclulate elapsed time since start of interval.
    long nElapsedTime = iTime - xferInfo.startOfInterval.time;

    if ( nElapsedTime>0 )
    {
      // Calculate elapsed frames in interval.
      float f = (float)nElapsedTime * clipInfo.fFramesPerMsec;



#ifdef SHOW_TIMING2

    // Round up to the nearest whole frame since
      // we assume audio always plays whole frames.
      long nElapsedFrames = (long)f; //ZUtils::ceil(f) ;

      QTV_MSG2( QTVDIAG_AUDIO_TASK, "time %d start interval %d",iTime,xferInfo.startOfInterval.time);
      QTV_MSG2( QTVDIAG_AUDIO_TASK, "elapsed frames %d.%d",
                nElapsedFrames, (f-nElapsedFrames)*FRAME_FRACTION_BASE);
#endif

      // Recalculate the elapsed time to match the whole frame count.
//      f = (float)nElapsedFrames / clipInfo.fFramesPerMsec;
//      nElapsedTime=(long)f;

#ifdef SHOW_TIMING2
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "elapsed time %d",nElapsedTime);
#endif

      // Get current position by adding elapsed time to start of interval.
      nCurFrame = xferInfo.startOfInterval.nFrame +
                  (float)xferInfo.startOfInterval.nFrameFraction/FRAME_FRACTION_BASE + f;

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
      Media::CodecType audioCodec = GetAudioCodecType();
      if((IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec))&& (m_nCurrentTSFromCMXBytes > 0))
      {
        //Return the current audio position timestamp based on feedbacks.
        //m_nCurrentTSFromCMXBytes gets updated whenever DSP generates feedback.
        nCurTimestamp = m_nCurrentTSFromCMXBytes;
        QTV_MSG1( QTVDIAG_AUDIO_TASK, "nCurTimestamp = m_nCurrentTSFromCMXBytes: %d",nCurTimestamp);
      }
      else
#endif /* FEATURE_QTV_CMX_AV_SYNC */

      nCurTimestamp = xferInfo.startOfInterval.timestamp + nElapsedTime;

#ifdef SHOW_TIMING3
      QTV_MSG3( QTVDIAG_AUDIO_TASK, "Calculated next frame#%d.%d, timestamp:%d", (long)nCurFrame,
                (nCurFrame - (long)nCurFrame)*FRAME_FRACTION_BASE, nCurTimestamp);
#endif
    }
    else
    {
      //weird case.
      //caller is asking for audio position before audio started
      //playing.  assume this is due to device startup delay.

      //audio position is still at start of interval.
      nCurFrame=xferInfo.startOfInterval.nFrame +
                (float)xferInfo.startOfInterval.nFrameFraction/FRAME_FRACTION_BASE;
      nCurTimestamp=xferInfo.startOfInterval.timestamp;
    }
#ifdef SHOW_TIMING2
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "cur frame %d ts %d",(long)nCurFrame, nCurTimestamp);
#endif

#ifdef SHOW_SYNC
    QTV_MSG3( QTVDIAG_AUDIO_TASK, "Audio pos t %d f %d ts %d",iTime,(long)nCurFrame,nCurTimestamp);
#endif
    return true;
  }
  return false;
}

/* ======================================================================
FUNCTION
  AudioMgr::ShowInterval

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
void AudioMgr::ShowInterval(TimePoint interval)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "   Interval");
  if ( interval.bValid )
    QTV_MSG4( QTVDIAG_AUDIO_TASK, "     F %d.%d TS %d Clk %d",
              interval.nFrame, interval.nFrameFraction,
              interval.timestamp, interval.time);
  else
    QTV_MSG( QTVDIAG_AUDIO_TASK, "     invalid");
}

/* ======================================================================
FUNCTION
  AudioMgr::ShowLastFrame

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
void AudioMgr::ShowLastFrame(Common::LastFrame lastFrame)
{
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   last frame valid %d",lastFrame.bValid);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "     frame %d",lastFrame.nFrame);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "     time %d",lastFrame.nTime);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "     timestamp %d",lastFrame.nTimestamp);
}

/* ======================================================================
FUNCTION
  AudioMgr::SetAVTiming

DESCRIPTION
//set or update AV timing after sending a buffer of data.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::SetAVTiming(long tPlayBegin,long nTimestamp,long nFrame)
{
  if ( !xferInfo.startOfInterval.bValid )
  {
    //Set initial timing

    //Some audio devices have a delay before audio starts.
    //I'm not sure timing algorithm can handle a start time
    //in the future, so just sleep til the start time.
    long delay=tPlayBegin - (long)ZUtils::Clock();
    if ( delay>0 )
    {
#ifdef SHOW_TIMING
      QTV_MSG1( QTVDIAG_AUDIO_TASK, "Audio Start Sleep %d",delay);
#endif

      QCUtils::sleep(delay);
    }
    if ( tPlayBegin<0 )
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
        "ERROR no play time was set");
    }
    else
    {
      //Set start of playback interval
      xferInfo.startOfInterval.timestamp = nTimestamp;
      xferInfo.startOfInterval.nFrame = nFrame;
      xferInfo.startOfInterval.nFrameFraction = 0;
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
      if(pAudioCMX && (pAudioCMX->GetCodec() == Media::AAC_CODEC))
#endif /* FEATURE_QTV_BSAC */
      {
        /* for AAC codec, Audio DSP genarates 2 frame time delay
           (plus some fixed ARM side delay), so we need to compensate it
           whenever we start audio (even after repositioning) */
        int startTimeComp = 50;
        long SamplingFreq = pMpeg4->GetAudioSamplingFreq();
        switch(SamplingFreq)
        {
          case 8000:
            startTimeComp += 250;
            break;
          case 11025:
            startTimeComp += 200;
            break;
          case 12000:
            startTimeComp += 175;
            break;
          case 16000:
            startTimeComp += 150;
            break;
          case 22050:
            startTimeComp += 50;
            break;
          default:
            break;
        }
        xferInfo.startOfInterval.time = tPlayBegin + startTimeComp;
      }
      else
      {
        xferInfo.startOfInterval.time = tPlayBegin;
      }
      xferInfo.startOfInterval.bValid = true;

#if(defined FEATURE_QTV_CMX_AV_SYNC_INTERFACE || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)

      /*On doing rapid FWD till we reach almost EOF followed by rapid RW
      untill we reach BOF and pause|resume results in loss of audio.In
      some scenerios, cmx_avsync_update.curTimeStamp doesn't get updated
      correctly as AV SYNC feedback doesn't come right away after the
      pause. Thus, when we resume, our TS is wrong and we abort the
      audio with audio device stravation error.*/
      Media::CodecType audioCodec = GetAudioCodecType();
      if(
         (Media::MP3_CODEC != audioCodec) &&
         (Media::NONMP4_MP3_CODEC != audioCodec) &&
         (Media::QCP_CODEC != audioCodec) &&
         (Media::NONMP4_AMR_CODEC != audioCodec) &&
         (Media::NONMP4_AAC_CODEC != audioCodec) &&
         (!IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec)) &&
         (cmx_avsync_update.curTimeStamp > 0))
      {
        cmx_avsync_update.curTimeStamp = xferInfo.startOfInterval.timestamp;
      }
#endif

#ifdef SHOW_TIMING
      QTV_MSG4( QTVDIAG_AUDIO_TASK,
        "Start of Interval f %d.%d ts %d clk %d",
        xferInfo.startOfInterval.nFrame,
        xferInfo.startOfInterval.nFrameFraction,
        xferInfo.startOfInterval.timestamp,
        xferInfo.startOfInterval.time);
#endif

      audioTime.tPlay = tPlayBegin;

      // Set the AV player timing.
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
        "Setting AV Sync for first time");
      SetPlaybackOffset(
                       xferInfo.startOfInterval.time,
                       xferInfo.startOfInterval.timestamp);
    }
  }
}

/* ======================================================================
FUNCTION
  AudioMgr::UpdateAVTiming

DESCRIPTION
//Once initial timing is set, do periodic updates.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::UpdateAVTiming(long nTimestamp,long nFrame)
{
  if ( xferInfo.startOfInterval.bValid )
  {
    // Given an actual audio frame timestamp in the current playback
    // interval, adjust the start of this interval to match.
    // this makes up for irregularities or discontinuities
    // in the audio timestamps.
#ifdef SHOW_TIMING3
    QTV_MSG1(QTVDIAG_AUDIO_TASK, "TimingUpdateInterval:%d",
      clipInfo.nTimingUpdateIntervalBuffers);
    QTV_MSG3(QTVDIAG_AUDIO_TASK,"SetAVTiming: nFirstFrame:%d - nFrame:%d.%d",
      audioBuffer.nFirstFrame, xferInfo.startOfInterval.nFrame,
      xferInfo.startOfInterval.nFrameFraction);
#endif

#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
    Media::CodecType audioCodec = GetAudioCodecType();
    if((Media::MP3_CODEC != audioCodec) &&
       (!IS_GENERIC_AUDIO_FORMAT(audioCodec)) &&
       (!IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec)) )
   {
     if(cmx_avsync_update.prevResumedTimeStamp!=-1)
     {
       float numFrames =  nFrame - cmx_avsync_update.prevResumedFrame;
       if ( numFrames >= 0 )
       {
         float timestamp = numFrames / clipInfo.fFramesPerMsec;
         cmx_avsync_update.prevResumedTimeStamp = nTimestamp -
          (long)timestamp;
       }
     }
   }
#endif
    // See how many frames have passed in this interval.
  float nFramesPassed =  nFrame - xferInfo.startOfInterval.nFrame -
    (float)xferInfo.startOfInterval.nFrameFraction/FRAME_FRACTION_BASE;
  if ( nFramesPassed >= 0 )
  {
    // Recalculate the start of interval timing backward using
    // the actual frame timestamp.
    float f = nFramesPassed / clipInfo.fFramesPerMsec;
    long newstart = nTimestamp - (long)f;
#ifdef SHOW_TIMING3
    QTV_MSG2( QTVDIAG_AUDIO_TASK, "newstart:%d, xfertimestamp:%d",
      newstart, xferInfo.startOfInterval.timestamp);
#endif
    // Make an adjustment if necessary.
    if ( newstart != xferInfo.startOfInterval.timestamp )
    {
      // Keep track of the drift for our stats.
      long delta = newstart - xferInfo.startOfInterval.timestamp;
      QCUtils::EnterCritSect(&audioStats.CS);
      audioStats.nDrift += (long)abs(delta);
      QCUtils::LeaveCritSect(&audioStats.CS);

      if (abs((delta)) > 2)
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
                      "Time has changed by %dms (missing audio data)",
                      delta);
      }

#ifdef SHOW_DELTA
      QTV_MSG3( QTVDIAG_AUDIO_TASK,
        "-> Audio Delta %d ts-nStrt %d  nDrft %d <-",
        delta, xferInfo.startOfInterval.timestamp - newstart,
        audioStats.nDrift);
#endif

#ifdef SHOW_SYNC
        //update the interval start.
      QTV_MSG3( QTVDIAG_AUDIO_TASK, "ADJUSTED:  xferStart.ts %d ms %d %d ",
                xferInfo.startOfInterval.timestamp - newstart,
                xferInfo.startOfInterval.timestamp,
                newstart );
#endif
      xferInfo.startOfInterval.timestamp = newstart;
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
       if((Media::MP3_CODEC != audioCodec) &&
         (!IS_GENERIC_AUDIO_FORMAT(audioCodec)) &&
         (!IS_BYTE_BASED_AVSYNC_SUPPORTED(audioCodec)))
      {
        /*readjust the playback offset with the updated interval only
        incase of streaming if this feature is defined, since the cmx
        a/v sync callback changes are restricted to only local file
        playback. This should not be called for local file playback
        since we are updating the offset based on the current timestamp*/
        if(cmx_avsync_update.curTimeStamp==0)
        {
          SetPlaybackOffset(xferInfo.startOfInterval.time,
            xferInfo.startOfInterval.timestamp);
        }
      }
#else
      /* if the feature is not defined, then readjust the playback offset as usual */
      SetPlaybackOffset(xferInfo.startOfInterval.time,
        xferInfo.startOfInterval.timestamp);
#endif
      }// newstart
    }// nFrameprocessed
  }// startOfInterval.bValid
}


/* ======================================================================
FUNCTION
  AudioMgr::SetPlaybackOffset

DESCRIPTION
//set playback offset using start of interval position.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::SetPlaybackOffset(long lTime,long timestamp)
{
  if(m_pAVSync)
  {
    m_pAVSync->SetPlaybackOffset(AVSync::AudioAV, lTime, timestamp);
  }

  if ( notifyFunc )
  {
    (notifyFunc)(Common::NOTIFY_TIMING, m_pClientData, NULL);
  }

#ifdef SHOW_TIMING
  QTV_MSG2( QTVDIAG_AUDIO_TASK,"  audio SetTiming clk %d ts %d",
    lTime,timestamp);
#endif
}


/* ======================================================================
FUNCTION
  AudioMgr::UpdateAudioEndStats

DESCRIPTION
//
// update audio end stats
//

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::UpdateAudioEndStats(unsigned long tEnd)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::UpdateAudioEndStats");

  float f = 0;

  QCUtils::EnterCritSect(&audioStats.CS);

  audioStats.lastFrame.bValid=true;
  //adjust back to start of last sample
  if (clipInfo.fFramesPerMsec)
  {
    f = (float)tEnd - ((float)1 / clipInfo.fFramesPerMsec);
  }
  audioStats.lastFrame.nTime = (long)f;
  audioStats.lastFrame.nTimestamp = xferInfo.nLastTimestampSent;
  audioStats.lastFrame.nFrame = xferInfo.nLastFrameSent;

  QCUtils::LeaveCritSect(&audioStats.CS);

}

/* ======================================================================
FUNCTION
  AudioMgr::PredictEndDelay

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
void AudioMgr::PredictEndDelay(long &tEnd)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::PredictEndDelay");

  /* return difference of last sent frames's time stamp to CMX and
     current time. We are adding 500ms just to be safe that CMX
     plays all the data
   */
  tEnd = GetAmtBufferedMsec(ZUtils::Clock()) + 500;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetStatistics

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
void AudioMgr::GetStatistics(QtvPlayer::AudioVideoStatisticsT &stat,
                             Common::LastFrame &lastFrame)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::GetStatistics");

  /*Code to obtain clipstats calls this fn even though audio interface
  was not created.Workaround: If pAudioCMX is NULL, just return.
  */
  if(NULL == pAudioCMX )
  {
    memcpy(&stat.Audio, &m_nAudioVideoStat.Audio, sizeof(stat.Audio) );
    memcpy(&lastFrame , &m_nAudioLastFrameStat, sizeof(lastFrame) );
    return;
  }

  QCUtils::EnterCritSect(&audioStats.CS);

  stat.Audio.format         = (int)pAudioCMX->GetCodec();
  stat.Audio.frames         = mediaInfo.nFramesProcessed;
  stat.Audio.nullSamples    = audioStats.nNullFrames;
  stat.Audio.maxDrop        = audioStats.nMaxNullChunk;
  stat.Audio.maxAudioDataLead  = (int32)m_MaxCmxAudioDataLead;;
  stat.Audio.minAudioDataLead  = (int32)m_MinCmxAudioDataLead;;

  stat.Audio.encodedBitrate   = 0;
  if( pMpeg4 != NULL )
  {
    uint32 audioTrackID = pMpeg4->GetAudioTrackID(0);
    switch( pMpeg4->GetSource() )
    {
    case Media::FILE_SOURCE:
    {
      FileBase *pFile = pMpeg4->GetFile();
      if(pFile)
      {
        stat.Audio.encodedBitrate   =  pFile->getTrackAverageBitrate(audioTrackID);
      }
      break;
    }
    case Media::STREAM_SOURCE:
    {
      stat.Audio.encodedBitrate = pMpeg4->GetAudioBitRate(0);
      break;
    }
#ifndef FEATURE_QTV_GENERIC_BCAST
    case Media::BCAST_FLO_SOURCE:
    {
      stat.Audio.encodedBitrate = pMpeg4->GetAvgAudioBitRate();
      break;
    }
#endif

    default: /* already been initialized to 0 */
      break;
    }
#ifdef FEATURE_QTV_GENERIC_BCAST
    if(IsLiveBcastMediaSource(pMpeg4))
    {
      stat.Audio.encodedBitrate = pMpeg4->GetAvgAudioBitRate();
    }
#endif
  }
  lastFrame = audioStats.lastFrame;
  QCUtils::LeaveCritSect(&audioStats.CS);
}

/* ======================================================================
FUNCTION
  AudioMgr::GetFrame

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
unsigned long AudioMgr::GetFrame()
{
  return mediaInfo.nFramesProcessed;
}

/* ======================================================================
FUNCTION
  AudioMgr::SetChannelSwitchTS

DESCRIPTION
 Sets an indication of the channel switched frame

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  void

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::SetChannelSwitchTS(uint32 ChannelSwitchTS)
{
  ChannelSwitchedTS = ChannelSwitchTS;
  CheckForChannelSwitchFrame = true;
}

/* ======================================================================
FUNCTION
  AudioMgr::ShowInfo

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
#ifdef SHOW_AUDIO_INFO
void AudioMgr::ShowInfo()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::ShowInfo");

  QTV_MSG( QTVDIAG_AUDIO_TASK, "");
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "***** Audio Frames %d",
    mediaInfo.nFramesProcessed);

  QTV_MSG( QTVDIAG_AUDIO_TASK, "Xfer Info:");
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nFramesPerBuffer: %d",
    clipInfo.nFramesPerBuffer);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nTimingUpdateIntervalBuffers: %d",
    clipInfo.nTimingUpdateIntervalBuffers);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nBuffersSentInInterval: %d",
    xferInfo.nBuffersSentInInterval);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nLastTimestampSent: %d",
    xferInfo.nLastTimestampSent);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nLastFrameSent: %d",
    xferInfo.nLastFrameSent);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   bTimingUpdatedInInterval: %d",
    xferInfo.bTimingUpdatedInInterval);

  ShowInterval(xferInfo.startOfInterval);
  ShowInterval(xferInfo.endOfInterval);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nBytesSent: %d",xferInfo.nBytesSent);

  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nSamplesPerFrame %d",
    audioInfo.nSamplesPerFrame);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nMediaTimescale %d",
    audioInfo.nMediaTimescale);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nSamplingRate %d",
    audioInfo.nSamplingRate);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   Total bytes %d",audioInfo.nTotalBytes);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nMaxBytesPerFrame %d",
    audioInfo.nMaxBytesPerFrame);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nLastTimestamp %d",
    elapsed.nLastTimestamp);

  //audio time
  QTV_MSG( QTVDIAG_AUDIO_TASK, "Audio Time:");
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   audio Start %d",audioTime.tStart);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   audio Stop %d",audioTime.tStop);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   audio Play %d",audioTime.tPlay);

  QTV_MSG( QTVDIAG_AUDIO_TASK, "Audio Stats:");
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nNullFrames %d",
    audioStats.nNullFrames);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nMaxNullChunk %d",
    audioStats.nMaxNullChunk);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nCurrentNullChunk %d",
    audioStats.nCurrentNullChunk);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nDrift %d",audioStats.nDrift);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nMaxAhead %d",audioStats.nMaxAhead);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nMaxBehind %d",
    audioStats.nMaxBehind);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nMaxProcTime %d",
    audioStats.nMaxProcTime);
  QTV_MSG1( QTVDIAG_AUDIO_TASK, "   nFramesDiscarded %d",
    audioStats.nFramesDiscarded);
  ShowLastFrame(audioStats.lastFrame);

  QTV_MSG( QTVDIAG_AUDIO_TASK, " *****************");
  QTV_MSG( QTVDIAG_AUDIO_TASK, "");
}
#endif

/* ======================================================================
FUNCTION
  AudioMgr::OpenAudioManager

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
bool AudioMgr::OpenAudioManager(Common::AudioNotifyFuncT pNotifyFunc,
  void* pClientData)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::OpenAudioManager");

  InitDefaults();

  //save the parameters
  notifyFunc=pNotifyFunc;
  m_pClientData = pClientData;

  return true;
}


/* ======================================================================
FUNCTION
  AudioMgr::CloseAudioManager

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
void AudioMgr::CloseAudioManager()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::CloseAudioManager");

  notifyFunc=NULL;

  CleanupPlay();

#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO ))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */
  m_pClientData = NULL;
  m_pAVSync = NULL;
}

/* ======================================================================
FUNCTION
  AudioMgr::InitDefaults

DESCRIPTION
//init data for each time audio manager is created.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::InitDefaults()
{
  InitPlayData(false);

  pMpeg4     = NULL;
#if (defined (FEATURE_QTV_IN_CALL_PHASE_2) || \
     defined (FEATURE_QTV_IN_CALL_VIDEO))
#error code not present
#endif /* FEATURE_QTV_IN_CALL_PHASE_2 ||
          FEATURE_QTV_IN_CALL_VIDEO */

  notifyFunc=NULL;

  InitElapsedTime(0);
  stopTime.bStop=false;
  nStartTime=0;
  elapsedTimeForGenericAudio = 0;
#ifdef FEATURE_MP4_DEBUG_AV_TICK
#error code not present
#endif /* FEATURE_MP4_DEBUG_AV_TICK */
 /*Audio data lead*/
  m_MaxCmxAudioDataLead = 0;
  m_MinCmxAudioDataLead = 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::AudioMgr

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
AudioMgr::AudioMgr(Media *pMedia)
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::AudioMgr");

  m_audioCodec = Media::UNKNOWN_CODEC;
  pMpeg4 = pMedia;
 //Audio Data Lead for statistics purposes
  m_MaxCmxAudioDataLead = 0;
  m_MinCmxAudioDataLead = 0;
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

  audioBuffer.pData = NULL;
  audioBuffer.pSize = NULL;
  audioBuffer.pPendingData = NULL;
  pAudioCMX=NULL;
#if(defined FEATURE_QTV_CMX_AV_SYNC || defined FEATURE_QTV_CMX_AV_SYNC_BYTES)
  cmx_avsync_update.elapsedTime = 0;
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  nLastWriteIndex = -1;
  m_nCurrentTSFromCMXBytes = 0;
#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */
  RestoreDataBufferInfo.bRestoreBufferValid = false;
  memset(&RestoreDataBufferInfo,0,sizeof(RestoreDataBufferInfo));
  m_bCurrentTSFromCMXBytesUpdatePending = false;
  /*
  We need to add only one entry in AV SYNC mapping table
  irrespective of whether we will be sending audio frame/packet in
  multiple chunks or in one shot.Initialized to true by default in
  constructor and also when we fetch the audio sample.Variable is reset
  when entry gets added into AV SYNC mapping table.
  */
  m_bFirstAudioFragment = true;
  //We don't know yet what the audio codec is, so initialising the entire UnAckDataBuffer
  for(int iterator=0;
    iterator<MAX_NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER;
    iterator++)
  {
    memset(&RestoreDataBufferInfo.UnAckDataBuffer[iterator],0,
      sizeof(RestoreDataBufferInfo.UnAckDataBuffer[iterator]));
  }
#endif


  QCUtils::InitCritSect(&audioStats.CS);
  QCUtils::InitCritSect(&audioMgrCS);
  QCUtils::InitCritSect(&audioMgrCSforTS);
  InitDefaults();
  memset( &m_nAudioVideoStat,0, sizeof(m_nAudioVideoStat) );
  memset( &m_nAudioLastFrameStat,0, sizeof(m_nAudioLastFrameStat) );

  // Initialize ADTSLazyStart to false.
  m_bADTSLazyStart          = false;
  m_nStoredTimingUpdateMsec = 0;
  m_bCodecParamChanged      = false;
  CreateFastAudioPlaybackObject();
  // Initialize the audio track state Object
  SetAudioTrackSpec(Common::AUDIO_TRACK_STATE_INVALID,
                    Common::AUDIO_CHANNEL_UNKNOWN,0);

}
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
/* ======================================================================
FUNCTION
  audioMgr::GetRestoreDataBufferEntrySize

DESCRIPTION
  if BYTE BASED AV SYNC is enabled for given audio codec, returns
  max size needed for each entry in restore data buffer.
  For a gicen clip, size of each entry in restore buffer is same.

DEPENDENCIES
  none

RETURN VALUE
  Max size needed for each entry in restore buffer.

SIDE EFFECTS
  none

========================================================================== */
int AudioMgr::GetRestoreDataBufferEntrySize(Media::CodecType audioCodec)
{

#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
   if( Media::WMA_CODEC == GetAudioCodecType()
#ifdef FEATURE_QTV_WMA_PRO_DSP_DECODER
        ||Media::WMA_PRO_CODEC == GetAudioCodecType()
        ||Media::WMA_PRO_PLUS_CODEC == GetAudioCodecType()
#endif /* FEATURE_QTV_WMA_PRO_DSP_DECODER */
      )
  {
    return (2 * GetVirtualPacketSize());
  }
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AAC
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  if(Media::AAC_CODEC == audioCodec)
#endif /* FEATURE_QTV_BSAC */
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_AAC;
  }
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC
  if(audioCodec == Media::EVRC_CODEC)
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_EVRC;
  }
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_QCELP
  if(audioCodec == Media::QCELP_CODEC)
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_QCELP;
  }
#endif

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR_WB */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AMR
  if(Media::GSM_AMR_CODEC == audioCodec)
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_AMR;
  }
#endif
#if defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER)
  if(Media::WMA_PRO_CODEC == audioCodec)
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_WMA_PRO;
  }
  if(Media::WMA_PRO_PLUS_CODEC == audioCodec)
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_WMA_PRO_PLUS;
  }
#endif /* defined (FEATURE_QTV_WMA_PRO_DECODER) || defined (FEATURE_QTV_WMA_PRO_DSP_DECODER) */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_MP3
  if(Media::MP3_CODEC == audioCodec)
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_MP3;
  }
#endif
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_NB */
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_EVRC_WB*/

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3
  if(Media::AC3_CODEC == audioCodec)
  {
    return MAX_BYTES_PER_RESTORE_BUFFER_ENTRY_FOR_AC3;
  }
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_AC3*/

#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM
#error code not present
#endif /* FEATURE_QTV_CMX_AV_SYNC_BYTES_PCM*/

  /*
  * We should never come here as GetRestoreDataBufferEntrySize gets called
  * when BYTE BASED AV SYNC is supported for given audio codec.
  * In such case, bytes per restore buffer entry should be non-zero.
  */
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL,
    "AudioMgr::GetRestoreDataBufferEntrySize RETURNING 0..");
  return 0;
}
/* ======================================================================
FUNCTION8
  AudioMgr::DestroyRestoreDataBufferInfo

DESCRIPTION
  If restore data buffer is allocated,
  Frees the memory and sets bRestoreBufferValid to FALSE

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::DestroyRestoreDataBufferInfo()
{
  Media::CodecType audioCodec = GetAudioCodecType();
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
    "AudioMgr::DestroyRestoreDataBufferInfo");

  for(int k=0;k<NUMBER_OF_ENTRIES_TO_HOLD_IN_RESTORE_BUFFER[audioCodec];k++)
  {
    if(RestoreDataBufferInfo.bRestoreBufferValid &&
       RestoreDataBufferInfo.UnAckDataBuffer[k].pDataPtr)
    {
      QTV_Free(RestoreDataBufferInfo.UnAckDataBuffer[k].pDataPtr);
      RestoreDataBufferInfo.UnAckDataBuffer[k].pDataPtr = NULL;
    }
  }
  RestoreDataBufferInfo.bRestoreBufferValid = false;
}
#endif
/* ======================================================================
FUNCTION
  AudioMgr::~AudioMgr

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
AudioMgr::~AudioMgr()
{
  QTV_MSG( QTVDIAG_AUDIO_TASK, "AudioMgr::~AudioMgr");
#ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES
  DestroyRestoreDataBufferInfo();
#endif
  // Cleanup the data structures related to fast playback mode
  DestroyFastAudioPlaybackObject();
  aFastPlaybackFilterConfig = NULL;

  QCUtils::DinitCritSect(&audioStats.CS);
  QCUtils::DinitCritSect(&audioMgrCS);
  QCUtils::DinitCritSect(&audioMgrCSforTS);
}

/*======================================================================
FUNCTION:
    GetAudioSpecificConfig

DESCRIPTION:
    This function returns a pointer to the audio specific configuration data

INPUT/OUTPUT PARAMETERS:
    Size - returned configuration size.

RETURN VALUE:
    Pointer to audio configuration data.

SIDE EFFECTS:
    None
======================================================================*/
unsigned char *AudioMgr::GetAudioSpecificConfig(int *Size)
{
  if ( pMpeg4 )
  {
    return pMpeg4->GetAACHeader(0, *Size);
  }
  else
  {
    *Size = 0;
    return NULL;
  }
} // GetAudioSpecificConfig

/* ======================================================================
FUNCTION
  AudioMgr::GetAndDecodeAudioSpecificConfig

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
int AudioMgr::GetAndDecodeAudioSpecificConfig(cmx_af_aac_spec_type *pCmx_af_aac_spec)
{
  TAudioSpecificConfig codecConfig, * pCodecConfig = NULL;
  pCodecConfig = &codecConfig;


  codecConfig.iConfig = GetAudioSpecificConfig(&(codecConfig.iNumBytes) );


  int retVal;
#ifndef FEATURE_QTV_GENERIC_BCAST
  if (pMpeg4->GetSource() != Media::BCAST_FLO_SOURCE &&
      pMpeg4->GetSource() != Media::BCAST_ISDB_SOURCE)
#else
#ifndef FEATURE_QTV_ADTS_PARSER
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  if (IsFileBcastMediaSource(pMpeg4))
#endif /* FEATURE_QTV_BSAC */
#else
#error code not present
#endif /* ! FEATURE_QTV_ADTS_PARSER */

#endif
  {

    /*-----------------------------------------------------------------------
      Stream/File media use AAC header info to specify AAC parms.
    -----------------------------------------------------------------------*/
    retVal = DecodeAACHeader(pCodecConfig, pCmx_af_aac_spec);

  }
  else
  {
    /*-----------------------------------------------------------------------
      For the bcast media (FLO or ISDB), we don't really have any AAC header
      to decode, so we have another interface for filling in the CMX config
      info.
    -----------------------------------------------------------------------*/
    retVal = pMpeg4->FillCMXAACConfig(pCmx_af_aac_spec);

    /* Error Resilience (I'm guessing that's what ER stands for) flags.
       Dunno what epConfig is, but safe to say that the DSP/CMX doesn't
       yet support this stuff */
    AACERFlags.epConfig = 0;
    AACERFlags.aacSectionDataResilienceFlag = 0;
    AACERFlags.aacScalefactorDataResilienceFlag = 0;
    AACERFlags.aacSpectralDataResilienceFlag = 0;
  }

  QTV_MSG1( QTVDIAG_AUDIO_TASK,  "AudSpcCfg %d Bytes", codecConfig.iNumBytes);
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  QTV_MSG3( QTVDIAG_AUDIO_TASK,  "Obj %d SampRt %d nCh %d ",
            pCmx_af_aac_spec->audio_object,
            pCmx_af_aac_spec->sample_rate,
            pCmx_af_aac_spec->channel );
#else
  QTV_MSG3( QTVDIAG_AUDIO_TASK,  "Prof %d SampRt %d nCh %d ",
            pCmx_af_aac_spec->profile,
            pCmx_af_aac_spec->sample_rate,
            pCmx_af_aac_spec->channel );
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
  QTV_MSG1( QTVDIAG_AUDIO_TASK,  "BitRate %d ", pCmx_af_aac_spec->bit_rate);
  return retVal;
}

/* ======================================================================
FUNCTION
  AudioMgr::DecodeAACHeader

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
int AudioMgr::DecodeAACHeader(TAudioSpecificConfig* aInfo,
                              cmx_af_aac_spec_type *pCmx_af_aac_spec
                             )
{
  tDec_Int_File  Vars, *pVars;           /* Helper pointer */
  int status = SUCCESS;

  /*
   * Initialize "helper" pointers to existing memory.
   */
  pVars = &Vars;

  /*
   * Translate input buffer variables.
   */
  pVars->inputStream.pBuffer = aInfo->iConfig;

  pVars->inputStream.availableBits =
  (uint)(aInfo->iNumBytes << 3);

  pVars->inputStream.usedBits = 0;

  if(pVars->inputStream.availableBits>0)
  {

    /*
     * Buffer is not overrun, then
     * decode the AudioSpecificConfig() structure
     */
    status =
    get_audio_specific_config(pVars, pCmx_af_aac_spec);

  }
  else
    status=1;
  return status;
}

/* ======================================================================
FUNCTION
  AudioMgr::get_audio_specific_config

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
int AudioMgr::get_audio_specific_config(
  tDec_Int_File  * const pVars,
  cmx_af_aac_spec_type * pCmx_af_aac_spec
  )
{

  uint    temp;
  int     audioObjectType;
  uint32  sampling_rate;
  uint    channel_config;
  BITS   *pInputStream;
  int     status;
  int     nBitRate =0;
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  int     extension_sampling_rate_idx;
#endif
  status = SUCCESS;

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  memset(&AACERFlags, 0, sizeof(AACERFlags));
#endif

  pInputStream = &(pVars->inputStream);

  temp =
  getbits(
         LEN_OBJ_TYPE + LEN_SAMP_RATE_IDX,/*lint!e641 !e656 */
         pInputStream);

  /*
   * The following code can directly set the values of elements in
   * MC_Info, rather than first setting the values in pVars->prog_config
   * and then copy these values to MC_Info by calling set_mc_info.
   * In order to keep consistent with get_prog_config (ADIF) and
   * get_adts_header (ADTS), the code here is still copying
   * the info, and set the pVars->current_program = 0
   */

  /* AudioObjectType */
  audioObjectType = (temp & 0x1f0) >> 4;

  /* sampling rate index */
  pVars->prog_config.sampling_rate_idx = temp & 0xf;
  //sampling_rate_idx = temp & 0xf;
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */

  //somehow the table from Qualcomm and the table defined in 1.6.3.3 are in reverse order
  switch ( pVars->prog_config.sampling_rate_idx )
  {
  case 0:
    /* 96k     */
    pCmx_af_aac_spec->sample_rate =  CMX_AF_SAMPLE_RATE_96000;
    sampling_rate = 96000;
    break;
  case 1:
    /* 88.2k   */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_88200;
    sampling_rate = 88200;
    break;
  case 2:
    /* 64k     */
    pCmx_af_aac_spec->sample_rate= CMX_AF_SAMPLE_RATE_64000;
    sampling_rate = 64000;
    break;
  case 3:
    /* 48k     */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_48000;
    sampling_rate = 48000;
    break;
  case 4:
    /* 44.1k   */
    pCmx_af_aac_spec->sample_rate =  CMX_AF_SAMPLE_RATE_44100;
    sampling_rate = 44100;
    break;
  case 5:
    /* 32k     */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_32000;
    sampling_rate = 32000;
    break;
  case 6:
    /* 24k     */
    pCmx_af_aac_spec->sample_rate= CMX_AF_SAMPLE_RATE_24000;
    sampling_rate = 24000;
    break;
  case 7:
    /* 22.050k */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_22050;
    sampling_rate = 22050;
    break;
  case 8:
    /* 16k     */
    pCmx_af_aac_spec->sample_rate =  CMX_AF_SAMPLE_RATE_16000;
    sampling_rate = 16000;
    break;
  case 9:
    /* 12k     */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_12000;
    sampling_rate = 12000;
    break;
  case 10:
    /* 11.025k */
    pCmx_af_aac_spec->sample_rate= CMX_AF_SAMPLE_RATE_11025;
    sampling_rate = 11025;
    break;
  case 11:
    /* 8k      */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_8000;
    sampling_rate = 8000;
    break;
  case 12:
    /* Unknown rate */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_UNKNOWN;
    sampling_rate = 7350;
    status = 1; /* do not support now */
    break;
  case 15:
    /* Unknown rate */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_UNKNOWN;
    /* sampling rate not listed in Table 1.6.2, the value is dierctly
       stored in following bits. */
    sampling_rate = getbits((uint32)LEN_SAMP_RATE, pInputStream);
    status = 1; /* do not support now */
    break;
  default:
    /* Unknown rate */
    pCmx_af_aac_spec->sample_rate = CMX_AF_SAMPLE_RATE_UNKNOWN;
    sampling_rate = 0;
    status = 1;
    break;
  }
  nBitRate = sampling_rate;
  pMpeg4->SetAudioSamplingFreq(sampling_rate);
  channel_config =getbits((uint)LEN_CHAN_CONFIG,pInputStream);
  // For AAC ADTS Codec assign the channel returned from the media
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
  if(pMpeg4->GetAACDataFormat() == Media::AAC_DATA_FORMAT_ADTS)
#endif /* FEATURE_QTV_BSAC */
  {
    pCmx_af_aac_spec->channel = CMX_AF_AAC_CHANNEL_UNKNOWN;
    if(channel_config == 1)
    {
      pCmx_af_aac_spec->channel = CMX_AF_AAC_CHANNEL_MONO;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
        ": AAC ADTS MONO  SPEAKER");
    }
    else if (channel_config == 2)
    {
      pCmx_af_aac_spec->channel = CMX_AF_AAC_CHANNEL_DUAL;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
        ": AAC ADTS STEREO SPEAKER ");
    }
    else
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
        ": AAC ADTS UNKNOWN SPEAKER ");
    }
  }

  if(channel_config > 2 )
  {
    status = 1; /* do not support more than two channels */
  }

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  if((audioObjectType==5) && (status==SUCCESS) )
  {
    /* as per ISO/IEC 14496-3:2001/FDAM 1:2003(E) Table 1.8 */
    extension_sampling_rate_idx =
      getbits((int)LEN_SAMP_RATE_IDX, pInputStream);
    if(extension_sampling_rate_idx == 0x0f)
    {
      status = 1; /* do not support this rate */
    }
    else
    {
      audioObjectType = getbits( (int)LEN_OBJ_TYPE, pInputStream);
    }
  }

  /* saving an audioObjectType into a profile field */
  pVars->prog_config.profile = audioObjectType;

  if((audioObjectType==5) || (audioObjectType==18) )
  {
    status = 1; /* do not support this */
  }
  else if(((((int)audioObjectType>=(int)MP4AUDIO_AAC_MAIN)&&((int)audioObjectType<=(int)MP4AUDIO_TWINVQ)) ||
              (((int)audioObjectType>=(int)MP4AUDIO_ER_AAC_LC)&&((int)audioObjectType<=(int)MP4AUDIO_ER_AAC_LD))) &&
            (status == SUCCESS) )
#else
  {
    // -1 to be consistent with the operation in get_prog_config
    pVars->prog_config.profile = audioObjectType-1;
  }
  if((/*(audioObjectType == MP4AUDIO_AAC_MAIN)     ||*/
     (audioObjectType == MP4AUDIO_AAC_LC)||
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
        /*(audioObjectType == MP4AUDIO_AAC_SSR)       ||*/
        (audioObjectType == MP4AUDIO_LTP)           /*||*/
        /*(audioObjectType == MP4AUDIO_AAC_SCALABLE)  ||*/
        /*(audioObjectType == MP4AUDIO_TWINVQ)*/) && (status == SUCCESS) )
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
  {
    // Invoke further parsing only if needed
    if ( pInputStream->availableBits > pInputStream->usedBits )
    {
      status =
      get_GA_specific_config(
                          pVars,
                          pInputStream,
                          channel_config,
                          audioObjectType
                          );
    }

  }
  else
  {
    status = 1;
  }

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  if ( (audioObjectType >= (int)MP4AUDIO_ER_AAC_LC) &&
       (audioObjectType <= (int)MP4AUDIO_ER_AAC_LD) &&
       (status == SUCCESS) )
  {
    AACERFlags.epConfig = (byte)getbits((int)LEN_EP_CONFIG, pInputStream);
  }
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */

  if ( status == SUCCESS )
  {
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */
    pCmx_af_aac_spec->file_type=CMX_AF_FILE_AAC;

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
    pCmx_af_aac_spec->audio_object = (cmx_af_mp4_audio_object_type)pVars->prog_config.profile;
#else /* FEATURE_MP4_AAC_FRAME_MARKER */
    switch ( pVars->prog_config.profile )
    {
    case 0:
      pCmx_af_aac_spec->profile =CMX_AF_AAC_PROFILE_MAIN; /* Main profile (UNSUPPORTED)  */
      break;
    case 1:
      pCmx_af_aac_spec->profile =CMX_AF_AAC_PROFILE_LC;   /* Low Complexity profile  */
      break;
    case 2:
      pCmx_af_aac_spec->profile =CMX_AF_AAC_PROFILE_SSR;  /* Scalable Sampling Rate (UNSUPPORTED)   */
      break;
    default:
      pCmx_af_aac_spec->profile =CMX_AF_AAC_PROFILE_UNKNOWN;  /* Profile unknown   */
      break;
    }
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
#ifdef FEATURE_QTV_BSAC
#error code not present
#else
    if(pMpeg4->GetAACDataFormat() != Media::AAC_DATA_FORMAT_ADTS)
#endif /* FEATURE_QTV_BSAC */
    {
    switch ( pVars->prog_config.front.ele_is_cpe[0] )
    {
    case 0:
      pCmx_af_aac_spec->channel = CMX_AF_AAC_CHANNEL_MONO;    /* Single channel (mono) data */
      nBitRate *= 1;
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," ID KI: AAC Mono channel");
      pMpeg4->setAACChannelMode(0);

      break;
    case 1:
      pCmx_af_aac_spec->channel = CMX_AF_AAC_CHANNEL_DUAL;    /* 2 channels (Stereo)*/
#ifndef FEATURE_MP4_AAC_FRAME_MARKER
      nBitRate *= 2;
#endif
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH," ID KI: AAC stereo channel");
      pMpeg4->setAACChannelMode(1);
      break;
    default:
      //more than 2 channels not supported yet
      break;
    }
    } /* if */
  }

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  pCmx_af_aac_spec->bit_rate = nBitRate;
#else
  pCmx_af_aac_spec->bit_rate = nBitRate*2; // 16 bits per sample only
#endif
  return status;
}

/* ======================================================================
FUNCTION
  AudioMgr::get_GA_specific_config

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
int AudioMgr::get_GA_specific_config(
                                    tDec_Int_File * const pVars,
                                    BITS    *pInputStream,
                                    uint     channel_config,
                                    const int audioObjectType
                                    )
{

  int status = SUCCESS;
  uint frameLenFlag;
  uint dependsOnCoreCoder;
  uint extFlag;

#ifdef FEATURE_MP4_AAC_FRAME_MARKER
  uint extFlag3;
#else
  /* These variables are left for future implementation */
  /* UInt layer_num; */
  /* UInt numOfSubFrame; */
  /* UInt layer_len; */
  /* UInt aacSectionDataResilienceFlag; */
  /* UInt aacScalefactorDataResilienceFlag; */
  /* UInt aacSpectralDataResilienceFlag; */
  /* Int  extFlag3; */
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */

  /*
   * frame length flag == 0, 1024 samples/frame
   * frame length flag == 1,  960 samples/frame
   */
  frameLenFlag = getbits((int)LEN_FRAME_LEN_FLAG, pInputStream);
  if ( frameLenFlag == 0 )
  {
    pMpeg4->SetAACSamplesPerFrame(1024);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
      "AAC samples per frame : 1024");
  }
  else if ( frameLenFlag == 1 )
  {
    pMpeg4->SetAACSamplesPerFrame(960);
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,
      "AAC samples per frame : 960");
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,
      "AAC: Invalid FrameLengthFlag: %d", frameLenFlag );
    pMpeg4->SetAACSamplesPerFrame(1024);
  }


  /*
   * dependsOnCoreCoder == 1, core coder has different sampling rate
   * in a scalable bitstream
   */
  dependsOnCoreCoder =
  getbits(
         (int)LEN_DEPEND_ON_CORE,
         pInputStream);
#ifdef FEATURE_QTV_BSAC
#error code not present
#endif /* FEATURE_QTV_BSAC */

  if ( dependsOnCoreCoder != FALSE )
  {
    /* Scalable coding is not supported.  If the dependsOnCoreCoder
    ** flag is set, we simply parse past the coreCoderDelay field.
    */
    /* coreCoderDelay = */(void)getbits( (int)LEN_CORE_DELAY, pInputStream );
  }

  /*
   * extension flag indicates if Amendment 1 objects are used or not
   * extension flag == 0 objects = 1, 2, 3, 4, 6, 7
   * extension flag == 1 objects = 17, 19, 20, 21, 22, 23
   */
  extFlag =
  getbits(
         (int)LEN_EXT_FLAG,
         pInputStream);

  if ( status == SUCCESS )
  {

    if(channel_config==0 &&
        (pInputStream->availableBits > pInputStream->usedBits))
    {
      status =
      get_prog_config(
                     pVars,
                     &pVars->scratch_prog_config);

      memcpy( &(pVars->prog_config.front),
        &(pVars->scratch_prog_config.front), sizeof(EleList) );
    }
    else
    {
      /*
       * dummy tag = 0 and
       * set up decoding configurations
       */
      channel_config--;
      pVars->prog_config.front.ele_is_cpe[0] = channel_config;
      pVars->prog_config.front.ele_tag[0] = 0;

      //WZeng  comment out, 05/23/02
      //            status =
      //                set_mc_info(
      //                    &(pVars->mc_info),
      //                    audioObjectType, /* previously profile */
      //                    pVars->prog_config.sampling_rate_idx,
      //                    pVars->prog_config.front.ele_tag[0],
      //                    pVars->prog_config.front.ele_is_cpe[0],
      //                    pVars->winmap, /*pVars->pWinSeqInfo,*/
      //                    pVars->SFBWidth128);
      //end WZeng

    } /* if (channel_config) */

  } /* if(status) */

  /*
   * This layer_num is not found in ISO/IEC specs,
   * but it is defined in San Diego spec for scalable bitstream
   */
  if ( (audioObjectType == (int)MP4AUDIO_AAC_SCALABLE) ||
       (audioObjectType == (int)MP4AUDIO_ER_AAC_SCALABLE) )
  {
    /*layer_num =
     *    getbits(
     *        LEN_LAYER_NUM,
     *        pInputStream);
     */

    status = 1; /* for this release only */
  }

  if ( extFlag != FALSE )
  {
#ifdef FEATURE_MP4_AAC_FRAME_MARKER
    if ( audioObjectType == (int)MP4AUDIO_ER_BSAC )
    {
      /* numOfSubFrame = */
      (void)getbits(
             (int)LEN_SUB_FRAME,
             pInputStream);

      /* layer_len = */
      (void)getbits(
             (int)LEN_LAYER_LEN,
             pInputStream);

    }

    if ( (audioObjectType == 17) || (audioObjectType == 19) ||
         (audioObjectType == 20) || (audioObjectType == 22) ||
         (audioObjectType == 23) )
    {
      AACERFlags.aacSectionDataResilienceFlag =
      (byte)getbits(
                   (int)LEN_SECT_RES_FLAG,
                   pInputStream);

      AACERFlags.aacScalefactorDataResilienceFlag =
      (byte)getbits(
                   (int)LEN_SCF_RES_FLAG,
                   pInputStream);

      AACERFlags.aacSpectralDataResilienceFlag =
      (byte)getbits(
                   (int)LEN_SPEC_RES_FLAG,
                   pInputStream);
    }

    /*
    * this flag is tbd in version 3 of ISO/IEC spec
    * if the encoder generates this bit, then it has to be read
    * current adif2mp4ff does not write this bit. If this bit is to
    * be read, it can be done by the following code:
    */
    extFlag3 =
    getbits(
           (int)LEN_EXT_FLAG3,
           pInputStream);

    if ( extFlag3 != FALSE )
    {
      status = 1;
    }

#else /* FEATURE_MP4_AAC_FRAME_MARKER */
    /*
     * currently do not implement these functionalities
     * defined in Amendment 1
     * keep it here for future release
     */
    /*if (audioObjectType == MP4AUDIO_ER_BSAC)
     *{
     *     numOfSubFrame =
     *         getbits(
     *             LEN_SUB_FRAME,
     *             pInputStream);
     *
     *     layer_len =
     *         getbits(
     *           LEN_LAYER_LEN,
     *           pInputStream);
     *
     *}
     */

    /*
     * The following code is equivalent to
     * if ((audioObjectType == 17) || (audioObjectType == 18) ||
     *     (audioObjectType == 19) || (audioObjectType == 20) ||
     *     (audioObjectType == 21) || (audioObjectType == 23))
     */

    /*if (((audioObjectType > 16) && (audioObjectType < 22)) ||
     *     (audioObjectType == 23))
     *{
     *    aacSectionDataResilienceFlag =
     *        getbits(
     *           LEN_SECT_RES_FLAG,
     *           pInputStream);

     *    aacScalefactorDataResilienceFlag =
     *        getbits(
     *           LEN_SCF_RES_FLAG,
     *           pInputStream);

     *    aacSpectralDataResilienceFlag =
     *        getbits(
     *           LEN_SPEC_RES_FLAG,
     *           pInputStream);
     *}
     */

    status = 1; /* for this release only */
#endif /* FEATURE_MP4_AAC_FRAME_MARKER */
  }

  /*
   * this flag is tbd in version 3 of ISO/IEC spec
   * if the encoder generates this bit, then it has to be read
   * current adif2mp4ff does not write this bit. If this bit is to
   * be read, it can be done by the following code:
   */
  /*extFlag3 =
   *   getbits(
   *       LEN_EXT_FLAG3,
   *       pInputStream);
   *
   *if (extFlag3 != FALSE)
   *{
   *   status = 1;
   *}
   */

  return status;
}

/* ======================================================================
FUNCTION
  AudioMgr::getbits

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
uint32 AudioMgr::getbits(
                        const uint  neededBits,
                        BITS       *pInputStream)
{
  uint32   returnValue;
  uint     offset;
  uint     bitIndex;
  /* Needs to be same type as pInput->pBuffer */
  unsigned char    *pElem;

  // Check for exceptions
  // If all the bits used up , then bail out by returning zero
  if(pInputStream->usedBits >= pInputStream->availableBits)
  {
    return 0;
  }

  offset = (pInputStream->usedBits) >> 3;

  pElem = pInputStream->pBuffer + offset;

  returnValue = (((uint32) *(pElem  )) << 24) |
                (((uint32) *(pElem+1)) << 16) |
                (((uint32) *(pElem+2)) <<  8) |
                ((uint32) *(pElem+3));

  /* Remove extra high bits by shifting up */
  bitIndex = (uint)((pInputStream->usedBits) & INBUF_BIT_MODULO_MASK);

  /* This line is faster way to mask off the high bits. */
  returnValue = returnValue << (bitIndex);

  /* Move the field down. */
  returnValue = returnValue >> (32-neededBits);

  pInputStream->usedBits += neededBits;

  return(returnValue);

} /* getbits() */

/* ======================================================================
FUNCTION
  AudioMgr::get_prog_config

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
int AudioMgr::get_prog_config(
                             tDec_Int_File *pVars,
                             ProgConfig    *pScratchPCE)
{
  int    i;
  int    tag;
  int    numChars;
  int    temp;
  bool   flag;
  int    status          = SUCCESS;
  BITS  *pInputStream   = &(pVars->inputStream);


  /*
   * The tag is used at the very end to see if this PCE is
   * the one to be used. Otherwise it does not need to be saved for the
   * the simple configurations to be used in this version of an AAC
   * decoder.
   *
   * All of the bits of this PCE must be read even if this PCE will not
   * be used. They are read into a temporary PCE, then later it is decided
   * whether to keep this PCE.
   *
   * To allow quick removal of the fields from the ProgConfig structure
   * that will probably not be used at a later date,
   * while still advancing the bitstream pointer,the return value of
   * getbits is saved into a temporary variable, then transfered to
   * the structure item.
   */
  tag =
  getbits(
         (int)LEN_TAG,
         pInputStream);

  pScratchPCE->profile =
  getbits(
         (int)LEN_PROFILE,
         pInputStream);

  pScratchPCE->sampling_rate_idx =
  getbits(
         (int)LEN_SAMP_IDX,
         pInputStream);

  /*
   * Retrieve the number of element lists for each of
   * front, side, back, lfe, data, and coupling.
   *
   * For two-channel stereo or mono, only the data in the front needs
   * to be saved. However, ALL fields need to be skipped over in some
   * fashion. Also, the number of elements needs to be temporarily saved
   * to call get_ele_list(). If that function was changed to pass in
   * the number of points to be read, the memory set aside inside the
   * ProgConfig structure could be removed.
   */

  /*
   * The next six function calls could be combined into one, then use
   * shifts and masks to retrieve the individual fields.
   */
  temp =
  getbits(
         (int)LEN_NUM_ELE,
         pInputStream);

  pScratchPCE->front.num_ele = temp;

  /* Needed only to read in the element list. */
  temp =
  getbits(
         (int)LEN_NUM_ELE,
         pInputStream);

  pScratchPCE->side.num_ele = temp;

  /* Needed only to read in the element list. */
  temp =
  getbits(
         (int)LEN_NUM_ELE,
         pInputStream);

  pScratchPCE->back.num_ele = temp;

  /* Needed only to read in the element list. */
  temp =
  getbits(
         (int)LEN_NUM_LFE,
         pInputStream);

  pScratchPCE->lfe.num_ele = temp;

  /* Needed only to read in the element list. */
  temp =
  getbits(
         (int)LEN_NUM_DAT,
         pInputStream);
  pScratchPCE->data.num_ele = temp;

  /* Needed only to read in the element list. */
  temp =
  getbits(
         (int)LEN_NUM_CCE,
         pInputStream);

  pScratchPCE->coupling.num_ele = temp;

  /*
   * Read in mix down data.
   *
   * Whether these fields can be removed and have proper operation
   * will be determined at a later date.
   */

  /* Read presence of mono_mix */
  flag =
  (getbits(
          (int)LEN_MIX_PRES,
          pInputStream) != 0);

  pScratchPCE->mono_mix.present = flag;

  if ( flag != FALSE )
  {
    temp =
    getbits(
           (int)LEN_TAG,
           pInputStream);

    pScratchPCE->mono_mix.ele_tag = temp;

  } /* end if (flag != FALSE) */

  /* Read presence of stereo mix */
  flag =
  (getbits(
          (int)LEN_MIX_PRES,
          pInputStream) != 0);

  pScratchPCE->stereo_mix.present = flag;

  if ( flag != FALSE )
  {
    temp =
    getbits(
           (int)LEN_TAG,
           pInputStream);

    pScratchPCE->stereo_mix.ele_tag = temp;

  } /* end if (flag != FALSE) */

  /* Read presence of matrix mix */
  flag =
  (getbits(
          (int)LEN_MIX_PRES,
          pInputStream) != 0);

  pScratchPCE->matrix_mix.present = flag;

  if ( flag != FALSE )
  {
    temp =
    getbits(
           (int)LEN_MMIX_IDX,
           pInputStream);

    pScratchPCE->matrix_mix.ele_tag = temp;

    temp =
    getbits(
           (int)LEN_PSUR_ENAB,
           pInputStream);

    pScratchPCE->matrix_mix.pseudo_enab = temp;

  } /* end if (flag != FALSE) */

  /*
   * Get each of the element lists. Only the front information will be
   * used for the PV decoder, but the usedBits field of pInputStream must
   * be advanced appropriately.
   *
   * This could be optimized by advancing the bit stream for the
   * elements that do not need to be read.
   */
  get_ele_list(
              &pScratchPCE->front,
              pInputStream,
              TRUE);

  get_ele_list(
              &pScratchPCE->side,
              pInputStream,
              TRUE);

  get_ele_list(
              &pScratchPCE->back,
              pInputStream,
              TRUE);

  get_ele_list(
              &pScratchPCE->lfe,
              pInputStream,
              FALSE);

  get_ele_list(
              &pScratchPCE->data,
              pInputStream,
              FALSE);

  get_ele_list(
              &pScratchPCE->coupling,
              pInputStream,
              TRUE);

  /*
   * The standard requests a byte alignment before reading in the
   * comment. This can be done because LEN_COMMENT_BYTES == 8.
   */
  byte_align(pInputStream);

  numChars =
  getbits(
         (int)LEN_COMMENT_BYTES, pInputStream);

  /*
   * Ignore the comment - it requires 65 bytes to store (or worse on DSP).
   * If this field is restored, make sure to append a trailing '\0'
   */
  for ( i = numChars; i > 0; i-- )
  {
    (void)getbits(
           (int)LEN_BYTE,
           pInputStream);

  } /* end for */

  if ( pVars->current_program < 0 )
  {
    /*
     * If this is the first PCE, it becomes the current, regardless of
     * its tag number.
     */
    pVars->current_program = tag;

  } /* end if (pVars->current_program < 0) */


  if ( tag == pVars->current_program )
  {
    /*
     * This branch is reached under two conditions:
     * 1) This is the first PCE found, it was selected in the above if
     *    block. In all encoders found thus far, the tag value has been
     *    zero.
     * 2) A PCE has been sent by the encoder with a tag that matches the
     *    the first one sent. It will then be re-read. No encoder found
     *    thus far re-sends a PCE, when looking at ADIF files.
     *
     * Regardless, the temporary PCE will now be copied into the
     * the one official program configuration.
     */
    memcpy(
          &pVars->prog_config,
          pScratchPCE,
          sizeof(ProgConfig));

    /*if there is program_config_element in esds atom then we have to map
    the abject type value to the profile values that CMX is using */
    switch(pVars->prog_config.profile)
    {
      /* AAC MAIN profile */
      case 0:
        pVars->prog_config.profile = (int)CMX_AF_MP4_AUDIO_OBJECT_AAC_MAIN;
        break;
      /* AAC LC profile */
      case 1:
        pVars->prog_config.profile = (int)CMX_AF_MP4_AUDIO_OBJECT_AAC_LC;
        break;
      /* AAC LTP profile */
      case 3:
        pVars->prog_config.profile = (int)CMX_AF_MP4_AUDIO_OBJECT_AAC_LTP;
        break;
      default:
        pVars->prog_config.profile = (int)CMX_AF_MP4_AUDIO_OBJECT_UNKNOWN;
        break;
    }

    //comment out by WZeng, 05/23/02
    /* enter configuration into MC_Info structure */
    //        status =
    //            set_mc_info(
    //                &pVars->mc_info,
    //                pVars->prog_config.profile + 1,  /* add 1 to get an object */
    //                pVars->prog_config.sampling_rate_idx,
    //                pVars->prog_config.front.ele_tag[0],
    //                pVars->prog_config.front.ele_is_cpe[0],
    //                pVars->winmap,
    //                pVars->SFBWidth128);

  } /* end if (tag == pVars->current_program) */

  return(status);
}

/* ======================================================================
FUNCTION
  AudioMgr::get_ele_list

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
void AudioMgr::get_ele_list(
                           EleList     *pElementList,
                           BITS        *pInputStream,
                           const bool   enableCPE)
{
  int index;
  int *pEleIsCPE;
  int *pEleTag;

  pEleIsCPE = &pElementList->ele_is_cpe[0];
  pEleTag   = &pElementList->ele_tag[0];

  for ( index = pElementList->num_ele; index > 0; index-- )
  {
    if ( enableCPE != FALSE )
    {
      *pEleIsCPE++ = getbits((int)LEN_ELE_IS_CPE, pInputStream);
    }
    else
    {
      *pEleIsCPE++ = FALSE;
    }
    *pEleTag++ = getbits((int)LEN_TAG, pInputStream);
  }
  return;
}

/* ======================================================================
FUNCTION
  AudioMgr::byte_align

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
void AudioMgr::byte_align(
                         BITS  *pInputStream)
{
  /*
   * Round up to the next byte by adding 7 and masking off with
   * FFF8 or FFFFFFF8. The masking operation is a faster way to
   * perform modulo arithmetic if the number is a power of 2.
   *
   * This code is the same as
   * pInputStream->usedBits += (pInputStream->usedBits + 7) % 8
   */
  pInputStream->usedBits += BYTE_ALIGN_ROUNDUP;
  pInputStream->usedBits &= BYTE_ALIGN_MASK;

  return;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetAudioCodecType

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
Media::CodecType AudioMgr::GetAudioCodecType()
{
   return m_audioCodec;
}

/* ======================================================================
FUNCTION
  AudioMgr::ReallocateAudioMediaBuffer

DESCRIPTION
  Reallocate audio Media buffer for AAC Codecs. Assumption is that PCR
  logic is applicable only to AAC codec.

DEPENDENCIES
  If PCR Logic is extended for other codecs, this logic has to be expanded.

RETURN VALUE
  true/false

SIDE EFFECTS
  None.
 ========================================================================== */
bool AudioMgr::ReallocateAudioMediaBuffer()
{
  // By default assume false return
  bool         bRet = true;  /* Return Value */
  uint32 bufferSize =     0;  /* Buffer Size  */
  if(pMpeg4)
  {
    bufferSize = pMpeg4->GetMaxAudioBufferSizeDB();
    if(bufferSize > (uint32)audioInfo.nMaxBytesPerFrame)
    {
      audioInfo.nMaxBytesPerFrame = bufferSize;
      audioBuffer.pPendingData =
        (unsigned char *)QTV_Realloc(
        audioBuffer.pPendingData,audioInfo.nMaxBytesPerFrame);
      if(!audioBuffer.pPendingData)
      {
        bRet = false;
        QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_FATAL,
          "Can't re-allocate data buffer of size %d",
          audioInfo.nMaxBytesPerFrame);
        mediaInfo.dataState = Media::DATA_ERROR;
        Abort();
      }
      else
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL,QTVDIAG_PRIO_HIGH,
                      "re-allocated to  data buffer of size %d",
                      audioInfo.nMaxBytesPerFrame);
      }
    }
  }
  return bRet;
}

/* ======================================================================
FUNCTION
  AudioMgr::FetchAudioSample

DESCRIPTION
  Gets the next audio sample. For AAC ADTS Codec it obtains the sample from media and feed it to ADTS Parser.
  ADTS Parser output is used to set the output parameters for this function.

DEPENDENCIES
  None.

RETURN VALUE
  Number of bytes pulled either directly from media or indirectly through ADTS parser state machine.

SIDE EFFECTS
  Detail any side effects.
 ========================================================================== */
int AudioMgr::FetchAudioSample(Media *pMedia,
                               Media::MediaStatus &eStatus,
                               int *nSamples,
                               int nMaxSize,
                               int nChannel)
{
  int nBytesPulled = 0;
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
  // By default assume zero samples and zero bytes.
  *nSamples = 0;

  if(pMedia)
  {
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
    {
      nBytesPulled = pMedia->GetNextAudioSample(eStatus,
        audioBuffer.pPendingData,nMaxSize,nSamples,0,nChannel);
    }
  }
  return nBytesPulled;
}
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
/* ======================================================================
FUNCTION
  AudioMgr::FetchNextAudioTimestamp

DESCRIPTION
  Returns the next timestamp wating to be pulled
  from the media.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next available timestamp.

SIDE EFFECTS
  None.
 ========================================================================== */
unsigned long AudioMgr::FetchNextAudioTimestamp(Media *pMedia)
{
  unsigned long nTS = QTV_MEDIA_INVALID_TIMESTAMP;
  if(pMedia)
  {
    // Peek into next audio sample by passing in 0 for MaxSize
    // and get its timestamp
    Media::MediaStatus eStatus;
    int nSamples;
    FetchAudioSample(pMedia, eStatus, &nSamples, 0);
    nTS = FetchAudioTimestamp(pMedia);
  }
  return nTS;
}

/* ======================================================================
FUNCTION
  AudioMgr::FetchAudioTimestamp

DESCRIPTION
  Checks the State Machine and returns the current output timestamp for AAC ADTS Codec. For other codec
  fetch the timestamp from the media.

DEPENDENCIES
  None.

RETURN VALUE
  Returns number of bytes pulled.

SIDE EFFECTS
  Detail any side effects.
 ========================================================================== */
unsigned long AudioMgr::FetchAudioTimestamp(Media *pMedia)
{
  unsigned long nTS = QTV_MEDIA_INVALID_TIMESTAMP;
  if(!pMedia)
  {
    return nTS;
  }

#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif
{
   nTS = pMedia->GetTimestampForCurrentAudioSample();
}

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif
  return nTS;
}
#ifdef FEATURE_QTV_ADTS_PARSER
#error code not present
#endif


/* ======================================================================
FUNCTION
  AudioMgr::getCMXMaxAheadInMsec

DESCRIPTION
  returns a value for CMXMaxAheadInMsec if corresponding config item is set
  This is specific to broadcast/multicast streaming

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int AudioMgr::getCMXMaxAheadInMsec()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->getCMXMaxAheadInMsec();
  }

  return 0;

}

/* ======================================================================
FUNCTION
  AudioMgr::GetSampleRate

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
uint32 AudioMgr::GetSampleRate()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioSamplingFreq();
  }
  return 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetNumChannels

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
uint16 AudioMgr::GetNumChannels()
{
  if( pMpeg4 != NULL )
  {
    return (uint16)pMpeg4->GetNumAudioChannels();
  }
  return 0;
}


#if defined(FEATURE_QTV_WINDOWS_MEDIA) || defined(FEATURE_QTV_WMA_PRO_DSP_DECODER)
/* ======================================================================
FUNCTION
  AudioMgr::GetCodecVersion

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
uint16 AudioMgr::GetCodecVersion()
{
  if( pMpeg4 != NULL )
  {
    return (uint16)pMpeg4->GetAudioCodecVersion();
  }
  return 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetBitRate

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
uint32 AudioMgr::GetBitRate()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioBitRate(0);
  }
  return 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetVirtualPacketSize()

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

uint16 AudioMgr::GetVirtualPacketSize()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioVirtualPacketSize(0);
  }
  return 0;
}
/* ======================================================================
FUNCTION
  AudioMgr::isStartOfDummyBytes()

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
bool AudioMgr::isStartOfDummyBytes()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->isStartOfDummyBytes(0);
  }
  return false;
}
/* ======================================================================
FUNCTION
  AudioMgr::GetEncoderOptions

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
uint32 AudioMgr::GetEncoderOptions()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioEncoderOptions(0);
  }
  return 0;
}


/* ======================================================================
FUNCTION
  AudioMgr::GetAdvancedEncodeOptions

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
uint16 AudioMgr::GetAdvancedEncodeOptions()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioAdvancedEncodeOptions(0);
  }
  return 0;
}


/* ======================================================================
FUNCTION
  AudioMgr::GetAdvancedEncodeOptions2

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
uint32 AudioMgr::GetAdvancedEncodeOptions2()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioAdvancedEncodeOptions2(0);
  }
  return 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetChannelMask

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
uint16 AudioMgr::GetChannelMask()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioChannelMask(0);
  }
  return 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetArmDataReqThr

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
uint16 AudioMgr::GetArmDataReqThr()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioArmDataReqThr(0);
  }
  return 0;
}

/* ======================================================================
FUNCTION
  AudioMgr::GetValidBitsperSample

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
uint8 AudioMgr::GetValidBitsperSample()
{
  if( pMpeg4 != NULL )
  {
    return pMpeg4->GetAudioValidBitsperSample(0);
  }
  return 0;
}
#endif /* defined(FEATURE_QTV_WINDOWS_MEDIA) ||
          defined(FEATURE_QTV_WMA_PRO_DSP_DECODER) */


/* ========================================================================
FUNCTION:
  AudioMgr::SetAVSyncObj

DESCRIPTION:
  Used to inform the audio mgr that it should use the specified
  obj ptr to set AV sync update information.

PARAMETERS:
  void* pAVSync
    AVSync obj shared by all media types.

RETURN VALUE:
  void
===========================================================================*/
void AudioMgr::SetAVSyncObj(void* pAVSync)
{
  m_pAVSync = (AVSync*)pAVSync;
}

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/* ======================================================================
FUNCTION
  AudioMgr::SetAudioSpec

DESCRIPTION


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::SetAudioSpec()
{
  return;
}
/* ======================================================================
FUNCTION
  AudioMgr::SetEndDelay

DESCRIPTION
  Change the End delay returned by PredictEndDelay.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
void AudioMgr::SetEndDelay()
{
  return;
}
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
/* ========================================================================
FUNCTION:
  AudioMgr::SetDualMonoOutput

DESCRIPTION:
  Sets the dual mono output configuration.

PARAMETERS:
  Common::DualMonoOutputType dualMonoOutput
    The desired dual mono output configuration.

RETURN VALUE:
  bool
    TRUE if configuration was set successfully, FALSE otherwise.
===========================================================================*/
bool AudioMgr::SetDualMonoOutput(Common::DualMonoOutputType dualMonoOutput)
{
  bool bRet = true;
  if (pAudioCMX)
  {
    bRet = pAudioCMX->SetDualMonoOutput(dualMonoOutput);
  }
  return bRet;
}
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

#ifdef FEATURE_QTV_LOW_POWER_AUDIO
bool AudioMgr::isAudioOnlyLocalPlayback()
{
  Media* pMedia = GetMediaPtr();
  if(pMedia != NULL)
  {
    Media::MediaFileType fileType = pMedia->GetFileType();
    return ((pMedia->GetFile()!= NULL) && (!pMedia->isHTTPStreaming()) && (fileType == Media::CONTENT_AUDIO));
  }
  return false;
}
#endif

#ifdef FEATURE_QTV_AUDIO_DISCONTINUITY
#error code not present
#endif /* FEATURE_QTV_AUDIO_DISCONTINUITY */

