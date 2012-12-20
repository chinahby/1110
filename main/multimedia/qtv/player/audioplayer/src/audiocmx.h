#ifndef _AUDIOCMX_H_
#define _AUDIOCMX_H_
/* =======================================================================
                               audiocmx.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =================================================== ====================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/audioplayer/main/latest/src/audiocmx.h#12 $
$DateTime: 2010/05/05 05:27:11 $
$Change: 1288565 $


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

extern "C" {
# include "cmx.h"
#include "snd.h"
} // extern "C"

#include "audioRequestQ.h"
#include "media.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//polling time when waiting on data requests from cmx.
static const int REQUEST_POLL_TIME_MSEC = 50;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

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
  AudioCMX

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
class AudioCMX
{

public: 
  AudioCMX(Media::CodecType codec);

  /////////////////////////////////////
  // General control.
  /////////////////////////////////////
  void Create(void * pMgr, Common::AudioNotifyFuncT pNotifyFunc, void* pClientData);
  void Destroy();
  bool InitPlay(bool bRestart);
  // Deferred InitPlay method for AAC ADTS Codec
  bool DeferredInitPlay();

  void SetPlayTime(long) { }
  bool Stop(long &tStop);
  void Abort(long &tStop);
  bool Pause(long &tPause);
  bool Start(long &tStart);
  bool Resume(long &tResume);
  void Flush();
  bool   image_swap;
  bool   no_flush;  
#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif

  Media::CodecType GetCodec(void) { return _codec; }
  
  /////////////////////////////////////
  // For sending data.
  /////////////////////////////////////

  bool SendData(long &tPlayBegin,long &nWaitMsec);

  bool FlushData( void );

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  /* Sets the dual mono output configuration */
  bool SetDualMonoOutput(Common::DualMonoOutputType newDualMonoOutput);
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

  /////////////////////////////////////
  // CMX Callbacks
  /////////////////////////////////////

  static void StopCallback (
    cmx_status_type    status,        /* Status of event                       */
    const void         *client_data   /* Client data passed in to the command  */
    );

  static void PauseCallback (
    cmx_status_type    status,        /* Status of event                       */
    const void         *client_data   /* Client data passed in to the command  */
    );

  static void ResumeCallback (
    cmx_status_type    status,        /* Status of event                       */
    const void         *client_data   /* Client data passed in to the command  */
    );

  static void PlayCallback(
    cmx_status_type status, 
    const void * client_data
    );

    static void AVSyncPlayCallback(
    cmx_status_type status, 
    const void * client_data,
    cmx_af_server_data_type *server_data
    );

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  static void genericPlayCallback(
    cmx_status_type status, 
    const void * client_data,
    cmx_af_server_data_type *server_data
    );

static void setAudioSpec(cmx_af_audio_spec_type * psd);
#endif /* FEATURE_QTV_GENERIC_AUDIO_DEBUGGING */

  static void DataRequestCallback(
    uint32 position,
    uint32 max_len,
    cmx_client_req_cb_func_ptr_type data_req_cb,
    const void *server_client_data,
    const void *client_data
    );

  static void FlushCallback (
    cmx_status_type    status,        /* Status of event                       */
    const void         *client_data   /* Client data passed in to the command  */
    );

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  static void DualMonoOutputCallback(
    cmx_status_type    status,        /* Status of event                       */
    const void         *client_data   /* Client data passed in to the command  */
    );
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

  cmx_af_aac_spec_type * GetCmx_af_aac_spec()
  {
    return &(aac_spec);
  }

  void FlushDataRequests();

  //these are set during the "create" call.
  long nAudioIFBufferSizeMsec ;
  int nAudioIFMaxAheadMsec ;
  int nAudioIFMaxBehindMsec ;


 #ifdef FEATURE_QTV_CMX_AV_SYNC_BYTES  
  /* cmx_av_sync is used to provide the time interval
    after which we want the callback function to report the
    number of samples played. The callback function that 
    must be called is also specified in this. This is
    initialized in the function audiocmx::initplay */
    cmx_av_sync_type cmx_av_sync;  
    /* this is the callback function that will report the number bytes played */    	  
    static void  audiocmx_avsync_callback(qword timestamp,qword num_of_samples,qword num_of_bytes,const void *client_data );    
#else
    #ifdef FEATURE_QTV_CMX_AV_SYNC  
#error code not present
    #endif
#endif

#ifdef FEATURE_QTV_EXTENDED_CMX_API
  // audio cmx av sync callback extended
  static void audiocmx_avsync_callback_ex(cmx_av_sync_info_type info,const void *client_data);

  // audio cmx codec update callback 
  static void audiocmx_codec_update_callback(cmx_af_codec_info_type info, const void *client_data);

  // Prepare Extended Codec Specification
  void PrepExtendedCodecSpec(cmx_codec_spec_type &original, cmx_codec_spec_ex_type &extended);

  // Prepare Extended AV Sync Configuration 
  void PrepExtendedAVSyncCfg(cmx_av_sync_type &original, cmx_av_sync_ex_type &extended);

  // Enable SBR Mode
  void EnableSbrMode();

  // Disable SBR Mode
  void DisableSbrMode();
#endif

#ifdef FEATURE_QTV_LOW_POWER_AUDIO
  bool isAudioOnlyLocalPlayback();
#endif


private:

  void* m_pClientData;
  //cmx handle
  cmx_handle_type         handle;
  cmx_codec_spec_type     cmx_codec;
  cmx_af_aac_spec_type    aac_spec;
#ifdef FEATURE_QTV_EXTENDED_CMX_API
  /* AAC ADTS Codec extended member */
  cmx_codec_spec_ex_type  m_aacADTSSpec;
  
  /* AAC ADTS Codec AV Sync member */
  cmx_av_sync_ex_type m_aacADTSAVSync;
#endif
  


  //cmx sync points
  struct CmxResponse
  {
    bool bResponse;
    bool bSuccess;
    rex_tcb_type* callerTcb;
  };
  CmxResponse stopResponse, pauseResponse, playResponse, resumeResponse,
              flushResponse;
#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  CmxResponse dualMonoOutputResponse;
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

  rex_timer_type cmx_sync_timer;

  /*
  ** A flag to keep track of whether we can accept CMX data requests.
  ** Since CMX runs at a higher priority than we do, it's possible to get
  ** data requests after we've called Destroy and after we've been destructed.
  ** We'll check m_canAcceptDataRequests against SAFE_TO_ACCEPT_DATA_REQS
  ** to see if we're still in memory and in a state where we can accept
  ** data requests before we try to queue them up.
  */
  static const uint32 SAFE_TO_ACCEPT_DATA_REQS;
  static const uint32 UNSAFE_TO_ACCEPT_DATA_REQS;  

  uint32 m_canAcceptDataRequests;
  
  //Request queue.
  AudioRequestQ requestQ;

  void CallbackResponse(const cmx_status_type status, CmxResponse &response);

  AudioRequestQ::DataRequest currentRequest; 

  bool SendDataToCmx(
    Common::AudioDataBuffer * pAudioBuffer,
    long &tPlay);

  struct Stats
  {
    int nFlush;
    int nReceived;
    int nAnswered;
    int nGet;
    int nReturned;
  };
  Stats stats;

  void CleanupPlay();
  static void FlushSingleDataRequest( cmx_client_req_cb_func_ptr_type data_req_cb, 
                                      const void *server_client_data );


  static void ImageChangeCallback(uint32 status);
  Media::CodecType _codec;

  Common::AudioNotifyFuncT pNotify;

  cmx_af_sample_rate_enum_type TranslateSampleRateForCMX(uint32 SampleRate);

  // Converts cmx channel type to QTV type
  static Common::ChannelConfigType 
  ConvertChannelConfigurationDefn(cmx_af_aac_channel_enum_type eCC);

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
  Common::DualMonoOutputType m_dualMonoOutput;

  /* Maps the Common dual mono output type to CMX dual mono output type */
  cmx_af_dual_mono_mode_enum_type MapCommonDMTypeToCMXDMType(
    Common::DualMonoOutputType dualMonoOutput);
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */

#ifdef FEATURE_QTV_GENERIC_BCAST_PCR
#error code not present
#endif /* FEATURE_QTV_GENERIC_BCAST_PCR */
};

#endif //_AUDIOCMX_H_ 

