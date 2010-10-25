/*=============================================================================
FILE:       VIDEO_ENC_DSP_IF.c

SERVICES:   Video Encode DSP Interface Implementation

DESCRIPTION:
   This file provides necessary implementation for the Video Encoder that
   ultimately drives the Video Encode Backend of the DSP.

LIMITATIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 2004 QUALCOMM Incorporated.
        All Rights Reserved.
        QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/lowtier/main/latest/src/video_enc_dsp_if.c#4 $ $DateTime: 2009/01/28 02:15:36 $ $Author: sanalk $

CAMIF Version Numbers:

ABSTRACT:
   
============================================================================*/
/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/09  sanal    Removed RVCT compiler warnings.
09/12/08  shiven   Fixed Linking issues when Post Editing features are disabled
09/10/08  shiven   Camcorder related WB Cacche Policy Support
03/06/08  jdas     Klocwork error fix
31/10/07  sanal    Merging change from mainline:
                   Changes to remove linker errors when
                   FEATURE_VIDEO_ENCODE_DEBUG is disabled 
06/30/06   gopi    initial version for 6260.
03/29/06   jdas    Removed lint error
10/03/05   cla     Modified video recording profiling code to support debugging 
                   using QXDM.
09/13/05   dw      Allow different DSP images to be loaded for Raw Mode
08/16/05   dw      Updated video recording profiling and event logging system
08/17/05   kwa     Coverity - fixed NULL_RETURNS and REVERSE_INULL in function
                   VideoEncDSP_VideoEncRawFrame
07/25/05   dw      Sync with csim - changes to include new header file
06/01/05   jk      Passed CAMQDSP_ImageType to VideoEncDSP_Initialize for
                   concurrent DSP.
05/25/05   drl     Replaced include of custmp4.h with customer.h
04/22/05   dw      Added raw mode support
03/18/05   jk      Synced up with csim.
02/22/05   wyh     Raw Encoder API changes.
02/15/05   dw      Added logic analyzer hooks with timelog under VIDEO_TIMETEST
01/25/05   wh      Cosmetic changes from code review of video_eng_frame api
01/24/05   pp/jk   Made compatible with c-sim.
01/20/05   wh      Fixed a xcoder bug.
01/19/05   wh      Added video_eng_frame API.
12/05/04   wyh     Added the VideoEncSleep command.
12/02/04   ali     Changes for transcoder
12/02/04   wyh     VideoEncDSP_VideoEncRawFrame resets requestIFrame
11/29/04   drl     Added VideoEncDSP_VideoEncRawFrame.
10/04/04   mvd     Removed #include camqdsp_if because it is included
                   in video_enc_dsp_if.
09/06/04   pp/wh   Pass pointers only to VideoEncDSP_VideoEncSelection(),
                   VideoEncDSP_VideoEncConfiguration(), VideoEncDSP_VideoEnc-
				   Start(), and VideoEncDSP_VideoEncUpdate()..
07/13/04   drl     Creation

============================================================================*/
#ifdef T_ARM
#include "customer.h"
#endif

#ifdef FEATURE_VIDEO_ENCODE

#include "assert.h"
#include "comdef.h"

#ifdef T_ARM
#include "rex.h"
#include "qdspext.h"
#include "vfe.h"
#include "vs.h"
#else
#include "qdsp_sim.h"
#endif

#include "qdsp.h"

#include "video_enc_dsp_if.h"
#include "video_enc_aux.h"
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  #include "videoengi.h"
#endif

typedef enum VideoEncDSP_RawEncodeStateEnum
{
  VIDEO_ENC_DSP_UNKNOWN,
  VIDEO_ENC_DSP_CONFIG_PENDING,
  VIDEO_ENC_DSP_START_PENDING
#ifdef FEATURE_VIDEO_ENCODE_RAW
  ,VIDEO_ENC_DSP_SELECT_PENDING
#endif /* FEATURE_VIDEO_ENCODE_RAW */
} VideoEncDSP_RawEncodeStateType;

LOCAL VideoEncDSP_RawEncodeStateType rawEncodeState;
LOCAL VideoEncDSP_CBType             rawEncodecb; /* This is the callback made when */
                                                  /* a raw encode is complete       */
LOCAL byte*                          rawYAddress;
LOCAL byte*                          rawCbCrAddress;

/* Sizes used for the Flushing of caches */
uint32                               allocBufSize;  /* Allocated BUffer Size */
uint32                               rawHeaderSize; /* Raw YcbCr Header Size */

LOCAL CAMQDSP_MsgCBType              dspCallback=NULL;
                                                  /* This is the callback specified */
                                                  /* during initialization.  We     */
                                                  /* intercept the callback to      */
                                                  /* perform some local callback    */
                                                  /* processing before making the   */
                                                  /* callback specified by the user */


LOCAL boolean selection_recieved;
void VideoEncDSP_ResponseHandler (CAMQDSP_ResponseType *response);

CAMQDSP_ReturnCodeType VideoEncDSP_Initialize 
(
  CAMQDSP_MsgCBType   cb, 
  CAMQDSP_ImageType   app
)
{
  MP4E_DEBUG_CLEAR_TIMELOG();
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2DSP_INITIALIZE_CMD,NULL,0);

  dspCallback    = cb;
  rawEncodeState = VIDEO_ENC_DSP_UNKNOWN;
  selection_recieved = TRUE;

  return CAMQDSP_Initialize (VideoEncDSP_ResponseHandler, app);
}


#ifdef T_ARM
void VideoEncDSP_rawEncodeCb(
  CAMQDSP_ReturnCodeType result,    
  CAMQDSP_ResponseType  *response
)
{
  if ( response->responseMsg == CAMQDSP_VIDEO_FRAME_DONE ) {
    video_eng_rawEncodeFrameDoneCb_isr(result,response);
  }
  else if (response->responseMsg == CAMQDSP_MSG_CONFIG_COMPLETE
           && result != CAMQDSP_SUCCESS ) {
    video_eng_rawEncodeConfigCompleteErrorCb_isr();
  }
  else if ( result != CAMQDSP_SUCCESS ) {
    graph_video_eng_queue_cmd(VIDENG_GRAPH_ERROR,NULL);
  }
  else {
    switch(response->responseMsg)
    {
    // all other commands gets equal treatment
    case CAMQDSP_MSG_CONFIG_COMPLETE:
    case CAMQDSP_SERVICE_UNAVAILABLE:    
    case CAMQDSP_MSG_OUTPUT2_END_OF_FRAME:
    case CAMQDSP_MSG_IDLE_COMPLETE:
    case CAMQDSP_MSG_ILLEGAL_COMMAND:
    case CAMQDSP_MSG_AHB_OVERFLOW_ERROR:
    case CAMQDSP_MSG_CAMIF_ERROR:
    case CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR:
    case CAMQDSP_MSG_UPDATE_COMPLETE:
    case CAMQDSP_MSG_AF_STATS_COMPLETE:
    case CAMQDSP_MSG_AE_AWB_STATS_COMPLETE:
    case CAMQDSP_MSG_HIST_STATS_COMPLETE:
    case CAMQDSP_VIDEO_ENCODE_CONFIG_DONE:
    case CAMQDSP_VIDEO_ENCODE_IDLE_DONE:
    case CAMQDSP_EPOCH_INTR:

      /* let the current camera routines handle this one */
      dspCallback(response);
      break;

    default:
      ASSERT(0);
    }
  }
}
#endif


CAMQDSP_ReturnCodeType VideoEncDSP_AudioCodecConfiguration (VIDEO_ENC_DSP_TBD)
{
  return CAMQDSP_INVALID_CMD;
}

CAMQDSP_ReturnCodeType VideoEncDSP_AudioConfiguration      (VIDEO_ENC_DSP_TBD)
{
  return CAMQDSP_INVALID_CMD;
}

CAMQDSP_ReturnCodeType VideoEncDSP_DTMFRequest             (VIDEO_ENC_DSP_TBD)
{
  return CAMQDSP_INVALID_CMD;
}

CAMQDSP_ReturnCodeType VideoEncDSP_AudioSleep              (VIDEO_ENC_DSP_TBD)
{
  return CAMQDSP_INVALID_CMD;
}

CAMQDSP_ReturnCodeType VideoEncDSP_AudioIdle               (VIDEO_ENC_DSP_TBD)
{
  return CAMQDSP_INVALID_CMD;
}

CAMQDSP_ReturnCodeType VideoEncDSP_AudioVocoderSelection   (VIDEO_ENC_DSP_TBD)
{
  return CAMQDSP_INVALID_CMD;
}

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncSelection 
                          (VideoEncDSP_EncoderSelectionCommandType * config)
{
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SELECTION,NULL,0);

  return CAMQDSP_VidEnc_Command (CAMQDSP_VIDEO_ENCODE_SELECTION, 
                                 (void *) config, 
                                 sizeof (VideoEncDSP_EncoderSelectionCommandType)/2);
}


CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncConfiguration 
                          (VideoEncodeDSP_EncoderConfigurationType *config)
{
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_CONFIGURATION,NULL,0);

  return CAMQDSP_VidEnc_Command (CAMQDSP_VIDEO_ENCODE_CONFIGURATION, 
                                 (void *) config, 
                                 sizeof (VideoEncodeDSP_EncoderConfigurationType)/2);
}

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncStart
                          (VideoEncodeDSP_EncoderStartType *config)
{
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_START,NULL,
                            MP4E_TIMELOG_FRAME_DSP_INC);

  return CAMQDSP_VidEnc_Command (CAMQDSP_VIDEO_ENCODE_START, 
                                 (void *) config,
                                 sizeof (VideoEncodeDSP_EncoderStartType)/2);
}

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncUpdate 
                          (VideoEncodeDSP_VideoEncodeUpdateType *config)
{
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_FEATURE_UPDATE,NULL,0);

  return CAMQDSP_VidEnc_Command (CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE, 
                                 (void *) config, 
                                 sizeof (VideoEncodeDSP_VideoEncodeUpdateType)/2);
}

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncSleep(void)
{
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_SLEEP,NULL,0);

  return CAMQDSP_VidEnc_Command(CAMQDSP_VIDEO_ENCODE_SLEEP,NULL,0);
}

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncIdle  (void)
{
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2DSP_VIDEO_ENCODE_IDLE,NULL,0);

  return CAMQDSP_VidEnc_Command (CAMQDSP_VIDEO_ENCODE_IDLE, NULL, 0);
}

CAMQDSP_ReturnCodeType VideoEncDSP_VideoEncRawFrame (byte *frame_YAddress, 
                                                     byte *frame_CbCrAddress,
                                                     ipl_col_for_type image_format,
                                                     VideoEncDSP_CBType cb)
{
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  CAMQDSP_ReturnCodeType ret;
#endif
  VideoEncodeDSP_EncoderConfigurationType * encoderConfig = mp4e_codec_config ();
  CAMQDSP_ResponseType *dummy = NULL;

  if (encoderConfig == NULL)
  {
    return CAMQDSP_INVALID_STATE;
  }

  switch(image_format)
  {
   default:
#ifdef T_ARM
     ASSERT(0);
#endif
   //lint -fallthrough
   case IPL_YCrCb420_MB_PK:
     encoderConfig->videoEncInputFormat.input_format = 
       VIDEO_ENC_INPUT_FORMAT_MB_BY_MB;
     break;
   case IPL_YCrCb420_LINE_PK:
     encoderConfig->videoEncInputFormat.input_format = 
       VIDEO_ENC_INPUT_FORMAT_LINE_BY_LINE;
     break;
   }
   
  
  if(!selection_recieved)
  {/* cannot proceed until selection is done this is only for xcoder */
     rawEncodeState = VIDEO_ENC_DSP_UNKNOWN;   
     cb (CAMQDSP_INVALID_STATE, dummy); /* We want to return the error, we don't care */ 
               /* about the value of CAMQDSP_ResponseType */  
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
     return CAMQDSP_QDSP_BUSY;
#endif
  }

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  if ( rawEncodeState != VIDEO_ENC_DSP_UNKNOWN &&
       rawEncodeState != VIDEO_ENC_DSP_CONFIG_PENDING // xcoder already config
     ) {
    return CAMQDSP_QDSP_BUSY;
  }
#endif

  rawEncodecb    = cb;
  rawYAddress    = frame_YAddress;
  rawCbCrAddress = frame_CbCrAddress;

  if (cb == NULL) return CAMQDSP_INVALID_PARM;

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  ret = CAMQDSP_SUCCESS;
#endif

  INTLOCK ()
  if (VideoEncDSP_VideoEncConfiguration (encoderConfig) != CAMQDSP_SUCCESS)
  {
    rawEncodeState = VIDEO_ENC_DSP_UNKNOWN;
    cb (CAMQDSP_INVALID_STATE, dummy); /* We want to return the error, we don't care */
                                       /* about the value of CAMQDSP_ResponseType */
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
    ret = CAMQDSP_INVALID_STATE;
#endif
  }
  else
  {
     rawEncodeState = VIDEO_ENC_DSP_CONFIG_PENDING;
     /* set the request I frame to FALSE for the next config */
     encoderConfig->requestIntraFrame = FALSE;
  }

  INTFREE ()
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  return ret;
#else
  return CAMQDSP_SUCCESS;
#endif //FEATURE_VIDEO_ENG_RAW_API
}

void VideoEncDSP_ResponseHandler (CAMQDSP_ResponseType *response)
{
  VideoEncodeDSP_EncoderStartType  *start_codec;
  CAMQDSP_ResponseType            *dummy = NULL;

  switch (rawEncodeState)
  {
  case VIDEO_ENC_DSP_UNKNOWN:
    /* This callback is not expected here, propagate it out to the next level
       of callbacks.  */
    dspCallback (response);
    break;

#ifdef FEATURE_VIDEO_ENCODE_RAW
    case VIDEO_ENC_DSP_SELECT_PENDING:
      /* Raw encoder requires a callback to issue video_eng_start 
      ** after QDSP selection is done - raw_encoder_qdsp_cb */
      if( response->responseMsg == CAMQDSP_VIDEO_ENCODE_SELECTION_DONE)
      {  
        selection_recieved = TRUE;

        /* Assign state to utilize video_eng_frame API */
        rawEncodeState = VIDEO_ENC_DSP_CONFIG_PENDING;
      }
      
      dspCallback(response);
      break;
#endif /* FEATURE_VIDEO_ENCODE_RAW */

  case VIDEO_ENC_DSP_CONFIG_PENDING:
    if (response->responseMsg == CAMQDSP_VIDEO_ENCODE_CONFIG_DONE)
    {
       start_codec = mp4e_codec_start ();
       if(start_codec)
       {
           start_codec->frameInputLumaPtrHigh   = ((uint32) rawYAddress    & 0xFFFF0000) >> 16;
           start_codec->frameInputLumaPtrLow    = ((uint32) rawYAddress    & 0x0000FFFF);
           start_codec->frameInputChromaPtrHigh = ((uint32) rawCbCrAddress & 0xFFFF0000) >> 16;
           start_codec->frameInputChromaPtrLow  = ((uint32) rawCbCrAddress & 0x0000FFFF);
       }

       /* Flush the Y,CbCr buffers for the WB Cache policy 
	      YCbCR Data is consecutive*/
       FLUSH_DATA_CACHE((byte *) ((uint32) rawYAddress - rawHeaderSize),allocBufSize);
	   
       if (start_codec == NULL || VideoEncDSP_VideoEncStart (start_codec) != CAMQDSP_SUCCESS)
       {
          rawEncodeState = VIDEO_ENC_DSP_UNKNOWN;
          rawEncodecb (CAMQDSP_INVALID_STATE, dummy); /* We want to return the error, we don't care */
       }
       else 
       {
	     rawEncodeState = VIDEO_ENC_DSP_START_PENDING;
       }
    }
    else if( response->responseMsg == CAMQDSP_VIDEO_ENCODE_SELECTION_DONE)
    { 
	  selection_recieved = TRUE;
    }
    else
    {
      dspCallback (response);
    }
    break;
  case VIDEO_ENC_DSP_START_PENDING:
    if (response->responseMsg == CAMQDSP_VIDEO_FRAME_DONE)
    {
       rawEncodeState = VIDEO_ENC_DSP_UNKNOWN;
       rawEncodecb (CAMQDSP_SUCCESS, response);
    }
    else
    {
      dspCallback (response);
    }
    break;

  default:
     dspCallback (response);
   }
}

#ifdef T_ARM
#ifdef FEATURE_MP4_TRANSCODER
CAMQDSP_ReturnCodeType VideoEncDSP_OfflineInitialize (CAMQDSP_MsgCBType cb)  
{

   dspCallback    = cb;    
   selection_recieved = FALSE;
   rawEncodeState = VIDEO_ENC_DSP_CONFIG_PENDING;  
   return CAMQDSP_Offline_Encode_Initialize (VideoEncDSP_ResponseHandler, CAMQDSP_MPEG4_ENC); 
}
#endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_VIDEO_ENCODE_RAW
CAMQDSP_ReturnCodeType VideoEncDSP_RawInitialize (CAMQDSP_MsgCBType cb, CAMQDSP_ImageType app)  
{
  dspCallback    = cb;    
  selection_recieved = FALSE;
  rawEncodeState = VIDEO_ENC_DSP_SELECT_PENDING;
  return CAMQDSP_Initialize (VideoEncDSP_ResponseHandler, app); 
}
#endif /* FEATURE_VIDEO_ENCODE_RAW */
#endif /* T_ARM */

#endif  /* FEATURE_VIDEO_ENCODE */
