/*===========================================================================

             V I D E O    E N C O D E R    I N T E R F A C E    

DESCRIPTION
  This file contains the interfaces to execute various video encoder
  operations such as init, config, start record, stop record, and exit.

REFERENCES
  Video Encoder ISOD: XX-XXXXX-00

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/video_enc.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/15/07   sanal   Changes to remove linker errors when 
                   FEATURE_VIDEO_ENCODE_DEBUG is disabled 
11/07/06   jdas    Removed FEATURE_RAW_TRANSCODER related code
09/01/06   kadhir  Removed unwanted Cross Compilation Dependency 
                   When qcamcorder Feature Disabled 
04/20/06   Nag     wrapper added for malloc, free and realloc routines
03/29/06   jdas    Removed lint error
11/30/05   wyh     Fix memory leak.
11/28/05   wyh     VFE filtering using IPL during xcoding
11/02/05   wyh     CHenroid's change to get L4Linux to work
10/17/05   gm      Automated script to modify the MSG macro's
10/03/05   cla     Modified video recording profiling code to support debugging 
                   using QXDM.
09/23/05   dw/aa   Added error message if GRAPH calls video_encode_exit
09/22/05   wyh     For l4linux, when init must be zeroed after mallocing.
08/31/05   wyh     Implement swap for frame drop.
08/16/05   dw      Updated video recording profiling and event logging system
07/25/05   dw      Added VFE luma filter update mechanism
07/05/05   ali     Fixed the race condition where graph task could access 
                   buffers after VS had freed it
06/06/05   jk      Removed compile warning.
06/03/05   jk      Changed to support L4 and eliminate a potential NULL 
                   pointer dereference.
05/25/05   drl     Replaced include of custmp4.h with customer.h
05/25/05   jk      Moved video_driver_state check to video_encode_frame().
05/17/05   wyh     Pass brand from xcoder to init_video_encode_raw_frame()
04/13/05   wyh     Added a comment re xcoder partial frame callback.
03/24/05   wyh     With the new dsp image we have to request i frame for 
                   the first frame in video_encode_raw_frame().
03/18/05   jk      Fixed video_encode_exit to return success immediately if 
                   video_encode_ops is NULL.
03/16/05   wyh     Accodomate new API which does not allow for time_incr=0
03/04/05   wyh     Change rc floag for xcoder from RC_ON_VAR_FPS to RC_ON
02/22/05   wyh     Raw Encoder API changes.
02/15/05   dw      Added logic analyzer hooks with timelog under VIDEO_TIMETEST
02/10/05   wyh     Allow users to specify bitrate in transcoder.
02/09/05   wyh     Raw encode must return VIDEO_FAILURE if encoder fails.
02/03/05   dw      Added code for camcorder time analysis under VIDEO_TIMETEST 
01/31/05   wh      Use camcorderutil_malloc functions for transcoder.
01/21/05   wh      Eliminate all malloc()'s from the mp4 library.
01/20/05   pp      Cleaned up.
01/19/05   wh      Temporary API. Don't handshake with video_eng_frame
01/14/05   jk      Made Lint free. Made Changes for a new library.
12/29/04   wyh     Heap tracking support through videodrv_malloc();
12/02/04   ali     Changes for transcoder
11/29/04   drl     Added video_encode_permitted function.
11/23/04   jk      Changed video_encode_exit to avoid race condition.
11/03/04   jk      Change corresponding to the frame callback function change.
10/01/04   jk      Changed viden_encode_* API. This API now gets functions 
                   pointers from the encoder and calls them.
08/24/04   pp/wh   Merged in rate control
04/21/04   sj      Modified code to support consistent state checks in the 
                   entire mp4 encoder.
03/12/04   ali     Code cleanup
03/02/04   ali     Changes for offline encoder
01/12/04   yye     Completed per slice cb in video_encode_start
01/07/04   sj      Added a per slice cb to video_encode_start and mp4_encode_start
11/19/03   sj      Added video_encode_dynamic_set_config to allow the caller
                   to dynamically set the config.
11/11/03   sj      Added video_encode_request_i_frame to dynamically
                   request an I Frame while recording.
09/08/03   sj      Updated with new callback for video_engine
06/29/03   sj      Cleaned up code.  
04/02/03   sj      Rearranged code to encapsulate details in video_encmp4.c
03/28/03   sj/kel  Added Code for Range Checking
03/20/03   sj      Did Code Optimizations.
03/18/03   sj      Added Fractional Rate support and video_encode_set_config
                   and video_encode_get_config
03/11/03   sj/nm   Added Rate Control in ARM
03/07/03   rpw     Merged in optimizations identified by Scott, Naren, and
                   myself.
02/14/03   sa      Modified to get to compile as library
02/04/03   sa      Added code to setup, enable, and disable the motion 
                   estimation HW.
01/31/03   sa      Changed all VIDEO_EXIT to VIDEO_NOT_INIT
01/22/03   sj      Support EFS debug and Alpha Release
12/17/02   sj      Created file.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef T_ARM
#include "customer.h"
#endif

#ifdef FEATURE_VIDEO_ENCODE

#include <stdio.h>
#include <stdlib.h>
#include "camcorder_msg.h" 
#include "video_enc.h"
#include "video_enc_dsp_if.h"
#include "video_enc_aux.h"
#include "ipl.h"

#ifdef T_ARM
#include "qdspext.h"
#include "graph.h"
#include "rex.h"        /* For Signal related routines                     */
#include "task.h"       /* Added VIDEO_ENCODER_SIG                         */
#else
#include "video_eng_stream.h"
#endif

video_enc_state_type video_driver_state = VIDEO_NOT_INIT;

const video_encode_ops_type *video_encode_ops = NULL;

extern const video_encode_ops_type *mp4_encode_ops_get(void);

#ifdef T_ARM
/*
** Variable to block video_encode_exit or video_encode_stop
** while encoding is completed in graph task.
*/
rex_tcb_type *vsblocked_tcb=NULL;
extern rex_tcb_type *encoder_tcb;
#endif

#ifdef FEATURE_MP4_TRANSCODER

/* static variables for the transcoder 
*/

static uint8*               outputBuf1=NULL, *outputBuf2=NULL;
static uint8                *reconBuf1=NULL, *reconBuf2=NULL;
static uint8                *videoOutputBuf=NULL;
static int16                *mp4_decoder_internal_memory=NULL;
static video_enc_init_type  *init_type=NULL;

/* this structure passes the frame done message
** from the dsp to video_encode_frame()
*/
static struct
{
  uint16 *fraDoneMes_ptr;
  uint32  time;
} frameDoneMessage;

static uint16                            frameDoneMessage1[4];

static uint32               raw_encode_fake_msec = 0;
static uint32               raw_encode_duration_msec;
#endif


#ifdef FEATURE_H264_ENCODER
extern const video_encode_ops_type *h264_encode_ops_get(void);
#endif


/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_init

DESCRIPTION
  This function is used to initialize the driver.  
  This can only be called in VIDEO_NOT_INIT state.
  The state transitions to VIDEO_INIT.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_init_type* init_data:  Used to init the driver and dsp 
 video_cb_func_ptr_type callback_ptr: Used to inform engine of command success
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE  otherwise
  
===========================================================================*/
video_status_type video_encode_init(video_enc_init_type* init_data, 
                                    void *param,
                                    video_cb_func_ptr_type cbk_ptr, 
                                    void* client_data)
{
  if (video_driver_state != VIDEO_NOT_INIT)
  {
    /* video_encode_init can only be called in VIDEO_NOT_INIT state */
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  video_driver_state = VIDEO_INIT;

#ifdef FEATURE_CAMCORDER_MALLOC
  if (!videodrv_malloc_context)
  {
    static msg_const_type m;
    videodrv_malloc_context = 
      camcorderutil_malloc_init(__MODULE__,__LINE__,&m);
  }
#endif


#ifdef FEATURE_H264_ENCODER
  if (init_data->vfmt == VIDEO_FMT_STREAM_VIDEO_H264)
  {
    video_encode_ops = h264_encode_ops_get();
  } else
#endif
  {
    video_encode_ops = mp4_encode_ops_get();
  }

  if (video_encode_ops->init(init_data, param) != VIDEO_SUCCESS)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    video_driver_state = VIDEO_NOT_INIT;

    #ifdef FEATURE_CAMCORDER_MALLOC
    { static msg_const_type m;
      camcorderutil_malloc_terminate(videodrv_malloc_context,
                                     __LINE__,__func__,&m);
      videodrv_malloc_context = NULL;
    }
    #endif

    return VIDEO_FAILURE;
  }
  
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_init w=%d h=%d rate=%d", 
            init_data->frame_width,init_data->frame_height,
            init_data->time_incr_res);
#endif
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
  return VIDEO_SUCCESS;
}

#ifdef FEATURE_MP4_TRANSCODER

/*===========================================================================

FUNCTION init_video_encode_raw_frame

DESCRIPTION
  This is the main structure that does the encoding for a video frame for 
  offline encoding. It is called every frame
  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  inputBuf: data to be encoded
  target_height: target height of the encoded frame
  target_width: target width of the encoded frame
  frame_duration: Duration of frame
  initialize: state
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
      
SIDE EFFECTS
  None
  
 
===========================================================================*/
video_status_type 
init_video_encode_raw_frame
( 
  uint16                            target_height, 
  uint16                            target_width, 
  uint16                            frame_duration,
  video_profile_type                profile,
  video_fmt_brand_type              brand,
  uint32                            target_bitrate_bps,
  video_client_req_cb_func_ptr_type cb
)
{
  video_status_type                 retval;
  video_enc_config_type             config;
  const video_encode_ops_type       *mp4_encode_ops;
#ifdef FEATURE_CAMCORDER_MALLOC
  extern camcorderutil_malloc_context_type *videoeng_malloc_context;
#endif

#ifdef FEATURE_CAMCORDER_MALLOC
  if (!videodrv_malloc_context) {
    static msg_const_type m;
    videodrv_malloc_context = 
      camcorderutil_malloc_init(__MODULE__,__LINE__,&m);
  }
#endif

#ifdef FEATURE_CAMCORDER_MALLOC
  if (!videoeng_malloc_context) {
    static msg_const_type m;
    videoeng_malloc_context = 
      camcorderutil_malloc_init(__MODULE__,__LINE__,&m);
  }
#endif


  //allocate the buffers if necessary
  if (outputBuf1 == NULL )
  {
    outputBuf1 = (uint8*) 
      videodrv_malloc(((target_height * target_width * 
                      QDSP_MB_LEN * sizeof(uint16)) >> 8)+132
                     );
  }
  if(outputBuf2 == NULL)
  {
    outputBuf2 = (uint8*) 
    videodrv_malloc(((target_height * target_width * 
                    QDSP_MB_LEN * sizeof(uint16)) >> 8)+132
                   );
  }
  if(reconBuf1 == NULL)
  {
    reconBuf1 = (uint8*) videodrv_malloc(target_height * target_width*3 / 2);
  }
  if(reconBuf2 == NULL)
  {
      reconBuf2 = (uint8*) videodrv_malloc(target_height * target_width*3 / 2);
  }
  if(videoOutputBuf == NULL)
  {
    /* This number, target_height*target_width*3/2, must be identical
    ** to the setting inside the xcoder for client_to_enc_data->buf_size
    ** in xcoder_video_eng_transcode_normal_cb.
    */
    videoOutputBuf = (uint8*) videodrv_malloc(target_height * target_width*3 / 2);
  }

  if (init_type == NULL ) {
    init_type = (video_enc_init_type *) 
          videodrv_malloc(sizeof(video_enc_init_type));
  }

  if(init_type == NULL  || videoOutputBuf == NULL ||reconBuf2 == NULL || 
     reconBuf1 == NULL || outputBuf2 == NULL || outputBuf1 == NULL )
  {
    CAMCORDER_MSG_ERROR("video_eng_init malloc error",0,0,0);
    cb(VIDEO_FAILURE,NULL,NULL,NULL);
    video_offline_encode_free_buffer();
    return VIDEO_FAILURE;
  }

  memset(init_type,0,sizeof(video_enc_init_type));

  init_type->frame_height = target_height;
  init_type->frame_width  = target_width;
  if(frame_duration == 0 )
    init_type->time_incr    = 133;
  else
    init_type->time_incr    = frame_duration;
  init_type->time_incr_res = 1000;  
  init_type->enc_pkt_ptr1 = outputBuf1;
  init_type->enc_pkt_ptr2 = outputBuf2;
  init_type->codec_init.mp4.short_header = FALSE;
  init_type->profile = profile;
  init_type->target_bitrate = (int32)target_bitrate_bps;
  init_type->vop_recon_buf_ptr1 = reconBuf1;
  init_type->vop_recon_buf_ptr2 = reconBuf2;
  init_type->ved_buf_ptr = videoOutputBuf;
  init_type->vfmt = VIDEO_FMT_STREAM_VIDEO_MPEG4;

  init_type->vbrand = brand;

  /* ved_buf_size is the size of videoOutputBuf */
  init_type->ved_buf_size =  target_height * target_width*3 / 2;
  init_type->enc_pkt_size = ((target_height * target_width * 
                        QDSP_MB_LEN * sizeof(uint16)) >> 8) + 132;
  init_type->codec_init.mp4.short_header = FALSE;

  if (mp4_decoder_internal_memory == NULL )
  {
   
    init_type->inter_memory_size = VIDEO_ENC_INTERNAL_MEMORY_SIZE(target_width,target_height)+ 
                                   VIDEO_ENC_INTRA_MEMORY_SIZE(init_type->time_incr_res,init_type->time_incr);
    mp4_decoder_internal_memory = (int16 *)videodrv_malloc(init_type->inter_memory_size);
  }

  if( mp4_decoder_internal_memory == NULL )
  {
    CAMCORDER_MSG_ERROR("video_eng_init malloc error",0,0,0);
    cb(VIDEO_FAILURE,NULL,NULL,NULL);
    video_offline_encode_free_buffer();
    return VIDEO_FAILURE;
  }
  init_type->internal_memory = mp4_decoder_internal_memory;

  /* We don't use video buffer for transcoding but we need to fake something
  ** non-zero to get VFE filters working.
  */
  init_type->bufferHandle = (VideoBufferHandle *) 0x1;

  /* write all the transcoder DMA variables */
  if(video_encode_raw_init(init_type,  NULL, NULL) != VIDEO_SUCCESS)
  { 
     CAMCORDER_MSG_ERROR("Error in video encode raw init",0,0,0);
     cb(VIDEO_FAILURE,NULL,NULL,NULL);
     video_offline_encode_free_buffer();
     return VIDEO_FAILURE;
  }

  /* set the rate control to RC_ON (not the default RC_ON_VAR_FPS */

  if(VIDEO_SUCCESS != video_encode_get_config(&config,NULL,NULL,NULL))
  {
    CAMCORDER_MSG_ERROR("Transcoder error in getting the config params",0,0,0);
    cb(VIDEO_FAILURE,NULL,NULL,NULL);
    video_offline_encode_free_buffer();
    return VIDEO_FAILURE;
  }

  /* Enable the following code if you do not wish to drop frames in transcoder
  */
  #if 0
  if(config.rc_ctl == RC_ON_VAR_FPS )
  {
    config.rc_ctl = RC_ON;
  
    if (VIDEO_SUCCESS != video_encode_set_config(&config,NULL,NULL,NULL))
    {
      CAMCORDER_MSG_ERROR("Transcoder error in set params",0,0,0);
      cb(VIDEO_FAILURE,NULL,NULL,NULL);
      video_offline_encode_free_buffer();
      return VIDEO_FAILURE;
    }
  }
  #endif

  mp4_encode_ops = mp4_encode_ops_get();
  (void)mp4_encode_ops->req_i_frame();

  /*first time, we want to write the bitstream header, */
  /*so call the same function */ 
  /* as we do for real time encoding*/
  retval = 
    video_encode_start((video_client_req_cb_func_ptr_type)cb, 
                       (void *)videoOutputBuf);

  /* Since mp4_initialize() time based on the real
  ** time clock we start our fake time on this clock also
  */

  raw_encode_fake_msec = mp4e_get_time_ms();
  
  if(retval != VIDEO_SUCCESS)
  {
      CAMCORDER_MSG_ERROR("Error in mp4 encode start",0,0,0);
      cb(VIDEO_FAILURE,NULL,NULL,NULL);
      return   retval;
  }

  return VIDEO_SUCCESS;

} /* init_video_encode_raw_frame() */

/*===========================================================================

FUNCTION video_encode_raw_init

DESCRIPTION
  This function is used to initialize the driver for offline encoding.  
  This can only be called in VIDEO_NOT_INIT state.
  The state transitions to VIDEO_INIT.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_init_type* init_data:  Used to init the driver and dsp 
 video_cb_func_ptr_type callback_ptr: Used to inform engine of command success
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE  otherwise
  
===========================================================================*/
video_status_type video_encode_raw_init(video_enc_init_type* init_data, 
                                        video_cb_func_ptr_type cbk_ptr, 
                                        void *client_data)
{ 
  
  if (video_driver_state != VIDEO_NOT_INIT)
  {
    /* video_encode_init can only be called in VIDEO_NOT_INIT state */
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  video_driver_state = VIDEO_INIT;

  #ifdef FEATURE_CAMCORDER_MALLOC
  if (!videodrv_malloc_context) {
    static msg_const_type m;
    videodrv_malloc_context = 
      camcorderutil_malloc_init(__MODULE__,__LINE__,&m);
  }
  #endif


#ifdef FEATURE_H264_ENCODER
  if (init_data->vfmt == VIDEO_FMT_STREAM_VIDEO_H264)
  {
    video_encode_ops = h264_encode_ops_get();
  } else
#endif
  {
    video_encode_ops = mp4_encode_ops_get();
  }

  //initialize the mp4 encoder 
  if (video_encode_ops->raw_init(init_data) != VIDEO_SUCCESS)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }

    video_offline_encode_free_buffer();
    #ifdef FEATURE_CAMCORDER_MALLOC
    { static msg_const_type m;
      camcorderutil_malloc_terminate(videodrv_malloc_context,
                                     __LINE__,__func__,&m);
      videodrv_malloc_context = NULL;
    }
    #endif

    video_driver_state = VIDEO_NOT_INIT;
    return VIDEO_FAILURE;
  }

  if (qdsp_read(QDSP_videoEncState) == 0xFFFF)
  {
    if(VideoEncDSP_VideoEncSelection (&codec_selection_cmd) != CAMQDSP_SUCCESS)
    {
      if (cbk_ptr)
      {
        (*cbk_ptr)(VIDEO_FAILURE,client_data);
      }
      video_driver_state = VIDEO_NOT_INIT;

      video_offline_encode_free_buffer();
      #ifdef FEATURE_CAMCORDER_MALLOC
      { static msg_const_type m;
        camcorderutil_malloc_terminate(videodrv_malloc_context,
                                       __LINE__,__func__,&m);
        videodrv_malloc_context = NULL;
      }
      #endif

      return VIDEO_FAILURE;
    }
  }
  
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_init w=%d h=%d rate=%d", 
             init_data->frame_width,init_data->frame_height,init_data->time_incr_res);
#endif
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
  return VIDEO_SUCCESS;
  
}

/*===========================================================================

FUNCTION videoenc_videoencDSP_cb

DESCRIPTION
  This is the callback which gets called in ISR context for transcoder
  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  result: parameter from DSP
  response: parameter from DSP
  
ARGUMENTS OUT
  None
===========================================================================*/
/*lint -e{818} 'response it pointing to const object
we can't change it as it requires changes CAMQDSP_MsgCBType in camera\camqdsp.h  */
void videoenc_videoencDSP_cb (CAMQDSP_ReturnCodeType result,    
                             CAMQDSP_ResponseType  *response) 
{                                                                                           
  if ((result != CAMQDSP_SUCCESS) || 
      (response->responseMsg != CAMQDSP_VIDEO_FRAME_DONE)
     ) 
  {
    CAMCORDER_MSG_HIGH("GOT something other then CAMQDSP_VIDEO_FRAME_DONE",0,0,0);
  }
  else
  {
   memcpy (frameDoneMessage1,(void *) response->responsePayload.mybuf,
           4*sizeof(uint16)
          );
   frameDoneMessage.fraDoneMes_ptr = &frameDoneMessage1[0];
   frameDoneMessage.time           = raw_encode_fake_msec;

   /* this will be the time of the next buffer */
   raw_encode_fake_msec += raw_encode_duration_msec;

   graph_queue_offline_encode_qdsp_msg
      ((uint32)response->responseMsg,&frameDoneMessage,sizeof(frameDoneMessage));
  }
}

/*===========================================================================

FUNCTION video_encode_raw_frame

DESCRIPTION
  This is the main structure that does the encoding for a video frame for 
  offline encoding. It is called every frame
  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  inputBuf: data to be encoded
  frame_duration: Duration of frame
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
      
SIDE EFFECTS
  None
  
 
===========================================================================*/
video_status_type 
video_encode_raw_frame(uint8 *inputBuf, uint32 frame_duration,
					    uint16 width, uint16 height)
{ 
  raw_encode_duration_msec = frame_duration;
  
 (void )VideoEncDSP_VideoEncRawFrame ((byte *) inputBuf,
                               (byte *) (inputBuf + (width*height)),    
                               IPL_YCrCb420_LINE_PK,
                               videoenc_videoencDSP_cb);    
 return VIDEO_SUCCESS;

} /* video_encode_raw_frame() */


/*===========================================================================

FUNCTION video_offline_encode_free_buffer

DESCRIPTION
  This routine is used to free the memory allocated during offline encoding
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
      
SIDE EFFECTS
  None
  
AUTHOR
  Suhail Jalil
  
MODIFIED
  11/14/02  Created
  
===========================================================================*/

void video_offline_encode_free_buffer(void)
{
  if ( outputBuf1)
  {
    videodrv_free(outputBuf1);
    outputBuf1=NULL;
  }
  if(outputBuf2)
  {
    videodrv_free(outputBuf2);
    outputBuf2=NULL;
  }
  if (reconBuf1 )
  {
    videodrv_free(reconBuf1);
    reconBuf1=NULL;
  }
  if(reconBuf2)
  {
    videodrv_free(reconBuf2);
    reconBuf2=NULL;

  }
  if(videoOutputBuf )
  {
    videodrv_free(videoOutputBuf);
    videoOutputBuf = NULL;
  }

  if (mp4_decoder_internal_memory) 
  {
    videodrv_free(mp4_decoder_internal_memory);
    mp4_decoder_internal_memory = NULL;
  }

  if(init_type)
  {
    videodrv_free(init_type);
    init_type = NULL;
  }

  return;
}

#endif   // FEATURE_MP4_TRANSCODER


/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_start

DESCRIPTION
  This function is used to start the lossless encoding.  
  This function can be called from VIDEO_INIT or VIDEO_STOPPED state
  The state transitions to VIDEO_ENCODING_STARTED
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 cb_ptr: Used every frame to send data to the video engine
 slice_cb_ptr: Used every slice to send slice data to upper layer (maybe null)
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_start(video_client_req_cb_func_ptr_type cbk_ptr,
                                     const void* client_data)
{
  if (cbk_ptr == NULL)
  {
    return VIDEO_FAILURE;
  }

  if ((video_driver_state != VIDEO_INIT) && 
      (video_driver_state!= VIDEO_ENCODING_STOPPED))
  {
    /* 
    ** video_encode_start can only be called in VIDEO_INIT or 
    ** VIDEO_ENCODING_STOPPED state 
    */
    (*cbk_ptr)(VIDEO_FAILURE,NULL,NULL,client_data);
    return VIDEO_FAILURE;
  }

  if (video_encode_ops->start(cbk_ptr, client_data) != VIDEO_SUCCESS)
  {
    (*cbk_ptr)(VIDEO_FAILURE,NULL,NULL,client_data);
    return VIDEO_FAILURE;
  }

#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_start called",0,0,0);
#endif

  video_driver_state = VIDEO_ENCODING_STARTED;
  (*cbk_ptr)(VIDEO_SUCCESS,NULL,NULL,client_data);
  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_request_i_frame

DESCRIPTION
  This function is used to request an I Frame in the near future.
  This function can only be called in the VIDEO_ENCODING_STARTED state
  (i.e when recording)
  The state remains the same after this function call.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type callback_ptr: Used to inform engine of command success
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE  otherwise
  
===========================================================================*/
video_status_type video_encode_request_i_frame(video_cb_func_ptr_type cbk_ptr, 
                                               void* client_data)
{
  if (video_driver_state != VIDEO_ENCODING_STARTED)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }

  if (video_encode_ops->req_i_frame() != VIDEO_SUCCESS)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_request_i_frame called",0,0,0);
#endif
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_config

DESCRIPTION
  This function is used to configure the driver parameters.
  It can be called numerous times.  
  It can only be called in the VIDEO_INIT state.
  The state remains at VIDEO_INIT.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_profile_type profile: Configure the profile to use in encoding
 video_cb_func_ptr_type cb_ptr: Function to call after config is complete
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_config(video_profile_type profile, 
                                      video_cb_func_ptr_type cbk_ptr, 
                                      void* client_data)
{
  if ( video_driver_state != VIDEO_INIT )
  {
    /* 
    ** video_encode_config can only be called after initialization
    */
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  if (video_encode_ops->config(profile) != VIDEO_SUCCESS)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  /* video_driver_state stays as VIDEO_INIT */
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_config profile=%d",profile,0,0);
#endif
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_stop

DESCRIPTION
  This function is used to stop the lossless encoding.  
  This function can be called all states except VIDEO_NOT_INIT
  The state transitions to VIDEO_ENCODING_STOPPED
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type cb_ptr: Function to call after stopping
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_stop(video_cb_func_ptr_type cbk_ptr,
                                    void* client_data)
{
  if ( video_driver_state == VIDEO_NOT_INIT )
  {
    /* 
    ** video_encode_stop cannot called after the driver has exit
    */
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
#ifdef T_ARM
  if (video_driver_state == VIDEO_ENCODING_FRAME) 
  {
    /*
    ** Video encode driver is encoding a frame in Graph task
    ** video_encode_stop is called in VS task, this should wait
    ** for graph task to be done encoding a frame.
    ** VS is higher priority then graph.
    */
    vsblocked_tcb = rex_self();
    (void)rex_clr_sigs(vsblocked_tcb, VIDEO_ENCODER_SIG);
    (void)rex_wait(VIDEO_ENCODER_SIG);
    (void)rex_clr_sigs(vsblocked_tcb, VIDEO_ENCODER_SIG);
    vsblocked_tcb = NULL;
  }
#endif
  
  if (video_encode_ops->stop() != VIDEO_SUCCESS)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_stop called",0,0,0);
#endif
  video_driver_state = VIDEO_ENCODING_STOPPED;
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_exit

DESCRIPTION
  This function is used to exit the driver.
  The exit can be called in any state.
  The state transitions to VIDEO_NOT_INIT
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type cb_ptr: Function to call after exiting
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_exit(video_cb_func_ptr_type cbk_ptr, 
                                    void* client_data) 
{
#ifdef T_ARM
  /*
  ** Signal used to wait for encode frame to be done
  ** Before exit is returned
  */

  if(rex_self() == encoder_tcb)
  {
    CAMCORDER_MSG_ERROR("Exit called from encoder task",0,0,0);
  }

  if (video_driver_state == VIDEO_ENCODING_FRAME) {
    /*
    ** Video encode driver is encoding a frame in Graph task
    ** video_encode_exit is called in VS task, this should wait
    ** for graph task to be done encoding a frame.
    ** VS is higher priority then graph
    */
    vsblocked_tcb = rex_self();
    (void)rex_clr_sigs(vsblocked_tcb, VIDEO_ENCODER_SIG);
    (void)rex_wait(VIDEO_ENCODER_SIG);
    (void)rex_clr_sigs(vsblocked_tcb, VIDEO_ENCODER_SIG);
	  vsblocked_tcb = NULL;
  }
#endif
 
  video_driver_state = VIDEO_NOT_INIT;

#ifdef FEATURE_L4
#error code not present
#else
  if (video_encode_ops == NULL || video_encode_ops->exit == NULL)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_SUCCESS,client_data);
    }
    return VIDEO_SUCCESS;
  }
#endif

  if (video_encode_ops->exit() != VIDEO_SUCCESS)
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_exit called",0,0,0);
#endif
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
#ifdef FEATURE_MP4_TRANSCODER
#ifdef T_ARM
  /* free buffers just in case this was started in the transcoder mode
  */
  video_offline_encode_free_buffer();
#endif
#endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_CAMCORDER_MALLOC
  { static msg_const_type m;
    camcorderutil_malloc_terminate(videodrv_malloc_context,
                                   __LINE__,__func__,&m);
    videodrv_malloc_context = NULL;
  }
#endif

  video_encode_ops = NULL;

  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_set_config

DESCRIPTION
  This function is used to set the Video Driver Configuration
  It should only be used when the Caller has intimate knowledge 
  of the internals ofthe Video Encoder or when recommended by Qualcomm.
  This function can be called only in the VIDEO_INIT state and it doesnt
  change the state machine.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_config_type* new_cfg:  The new configuration to use.
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_set_config
(
  video_enc_config_type   *new_cfg_ptr, 
  void                    *param,
  video_cb_func_ptr_type  cbk_ptr,
  void                    *client_data
)
{
  if ( video_driver_state != VIDEO_INIT )
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  if ( video_encode_ops->set_config(new_cfg_ptr, param) != VIDEO_SUCCESS ) 
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_set_config called",0,0,0);
#endif
  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_dynamic_set_config

DESCRIPTION
  This function is used to set the Video Driver Configuration
  It should only be used when the Caller has intimate knowledge 
  of the internals ofthe Video Encoder or when recommended by Qualcomm.
  This function can be called in VIDEO_INIT or VIDEO_ENCODING_STARTED state.
  It doesnt change the state machine.
  It will only change the following parameters: 
    quantizer_scale_intra, quantizer_scale_inter, quant_precision,
    intra_period, target_bitrate, rc_window 
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_config_type* new_cfg:  The new configuration to use.
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_dynamic_set_config
(
  video_enc_dynamic_config_type *new_cfg_ptr, 
  void                          *param,
  video_cb_func_ptr_type        cbk_ptr,
  void                          *client_data
)
{
  if ( (video_driver_state != VIDEO_INIT ) && 
       (video_driver_state != VIDEO_ENCODING_STARTED))
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  if ( video_encode_ops->dynamic_set_config(new_cfg_ptr, param) 
       != VIDEO_SUCCESS ) 
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_dynamic_set_config called",0,0,0);
#endif
  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_get_config

DESCRIPTION
  This function is used to get the current Video Driver Configuration
  This function can be called only in the VIDEO_INIT or VIDEO_ENCODING_STARTED
  state and it doesnt change the state machine.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_enc_config_type* cfg_ptr:  The output cfg ptr
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_get_config
(
  video_enc_config_type   *cfg_ptr, 
  void                    *param,
  video_cb_func_ptr_type  cbk_ptr,
  void                    *client_data
)
{
  if ( ( video_driver_state != VIDEO_INIT ) && 
       ( video_driver_state != VIDEO_ENCODING_STARTED))
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
  if ( video_encode_ops->get_config(cfg_ptr, param) != VIDEO_SUCCESS ) 
  {
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }
#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_get_config called",0,0,0);
#endif
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
  return VIDEO_SUCCESS;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_frame

DESCRIPTION
  This is a wrapper function of the main structure that does the encoding 
  for a video frame. It is called every frame.  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  None
      
SIDE EFFECTS
  None
  
AUTHOR
  
MODIFIED

===========================================================================*/
void video_encode_frame(uint8* buf_ptr)
{
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM_START_LOSSLESS_ENCODE,NULL,
                            MP4E_TIMELOG_FRAME_ARM_INC);

#ifdef T_ARM
  encoder_tcb = rex_self();
#endif 

  TASKLOCK ();
  if (video_driver_state == VIDEO_ENCODING_STARTED)
  {
    video_driver_state = VIDEO_ENCODING_FRAME;
  }
  
  TASKFREE ();

  if(video_driver_state == VIDEO_ENCODING_FRAME){
    video_encode_ops->encode_frame(buf_ptr);
    video_driver_state = VIDEO_ENCODING_STARTED;
  }
#ifdef T_ARM
  if (vsblocked_tcb)
  {
    (void)rex_set_sigs(vsblocked_tcb, VIDEO_ENCODER_SIG);
  }
#endif

}


/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_get_decoder_specific_info

DESCRIPTION
  This function is used to get the decoder specific info.
  This function can be called only in the VIDEO_INIT 
  state and it doesnt change the state machine.
  The ptr to the decoder specific information (buf_ptr) is valid
  until video_encode_start is called.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 video_cb_func_ptr_type cb_ptr: Return Callback Function to call
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  buf_ptr: Ptr to the decoder specific info.
  len:     Number of Bytes of decoder specific info.
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type video_encode_get_decoder_specific_info(uint32** buf_ptr,
                                    uint32* len, 
                                    video_cb_func_ptr_type cbk_ptr,
                                    void* client_data)
{
  if (video_driver_state != VIDEO_INIT)
  {
    /* 
    ** video_encode_get_decoder_specific_info can only be called in VIDEO_INIT 
    */
    if (cbk_ptr)
    {
      (*cbk_ptr)(VIDEO_FAILURE,client_data);
    }
    return VIDEO_FAILURE;
  }

  /*
  ** Get the decoder specific info
  */
  video_encode_ops->get_decoder_specific_info(buf_ptr, len);

#ifdef T_ARM
  CAMCORDER_MSG_HIGH ("video_encode_get_decoder_specific_info called",0,0,0);
#endif
  if (cbk_ptr)
  {
    (*cbk_ptr)(VIDEO_SUCCESS,client_data);
  }
  return VIDEO_SUCCESS;
}

/*===========================================================================

FUNCTION video_encode_permitted

DESCRIPTION
  This function is used to indicate if the current frame is permitted to 
  be encoded.  A value of true means that the frame should be encoded,
  a value of false means that the frame should not be encoded.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None

RETURN VALUE
  boolean: true means encode frame, false means drop the frame.
  
===========================================================================*/
boolean video_encode_permitted (VideoBufferHandle *bufferHandle)
{
  boolean retval = FALSE;
 
  if(video_encode_ops)
  {
    retval = video_encode_ops->encode_permitted (bufferHandle);
    if (!retval)  
    { /* log RC drops frame */
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM_RC_DROP_FRAME,NULL,0);
    }
  }
  return (retval);
}

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
/*===========================================================================

FUNCTION video_encode_swap

DESCRIPTION
  This function swaps the encode Ping-pong buffer. Encoding is done using
  pong-pong buffer to enable parallel processing and to keep track of 
  prediction frame.  Normally the BackEnd is used to keep track of the
  ping-pong buffers. However, in certain cases, as in the case of frame
  dropping due to rate control, we might need to enforce a swap on top of
  the swapping already done by qdsp.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None

RETURN VALUE
  VIDEO_SUCCESS
  VIDEO_FAILURE
  
===========================================================================*/
video_status_type video_encode_swap (void)
{
  if(video_encode_ops)
  {
    return video_encode_ops->swapper ();
  }
  else
  {
    return VIDEO_FAILURE;
  }
}
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */


/*===========================================================================

FUNCTION video_encode_update_filter

DESCRIPTION
  This function is used to indicate if the video encoder driver has updated
  the VFE 3x3 luma filter. A return value of TRUE means that the argument
  has been updated with the latest filter coefficients and that VFE update
  should be sent at the next possible instance. A return value of FALSE means
  that no filter is necessary.
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 None
   
ARGUMENTS IN/OUT   
  video_enc_3x3_luma_filter_type *filter
  
ARGUMENTS OUT
  None

RETURN VALUE
  boolean: true means update filter, false means no update needed.
  
===========================================================================*/
boolean video_encode_update_filter (video_enc_3x3_luma_filter_type *filter)
{
  /* We are supposed to update the filter here and return a TRUE 
  ** once the update is complete */
  if(video_encode_ops)
  {
    return video_encode_ops->update_filter (filter);
  }
  else
    return FALSE;
}

#endif /* FEATURE_VIDEO_ENCODE */
