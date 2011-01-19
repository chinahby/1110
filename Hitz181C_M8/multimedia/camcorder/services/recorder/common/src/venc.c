/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Video Encode Engine")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     V I D E O   E N C O D E   E N G I N E

GENERAL DESCRIPTION
  This module contains video encode engine software.  This is layer which
  multiplexes compressed audio and video streams together into a multimedia
  file or sends them along to data services.

REFERENCES
  Video Encode Engine ISOD: 80-XXXXX-XX XX

EXTERNALIZED FUNCTIONS
  video_eng_init
    This function initializes the underlying audio and video compression
    layers, in preparation for recording.
  video_eng_start
    This function begins recording audio and video streams.
  video_eng_write_uuid
    This function is used to pass along user data atoms to be written to the
    movie file before the actual stream data.
  video_eng_pause
    This function pauses the current audio/video file being recorded.
  video_eng_resume
    This function resumes the recording of the current audio/video file.
  video_eng_stop
    This function stops recording audio and video, and finalizes the output
    data appropriately.
  video_eng_exit
    This function cleans up and exits the video engine.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/venc.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/08   sanal   Handle videoeng return types to avoid task time outs
12/05/08   sanal   Removed terminate request to XCODER from venc
05/14/08   anil    featurised code with FEATURE_MP4_TRANSCODER
04/04/08   sanal   Synchronous stopping in transcoder
03/06/08   jdas    Klocwork error fix
01/30/08   dhaval  Removed qtv isr specific code
11/02/07   sanal   Removed compiler error when FEATURE_VIDEO_ENCODE is disabled
11/19/07   dhaval  Removed compilation warnings
06/25/07   gopi    Added state checking while returning a frame to VENC client 
05/24/07   dhaval  Added VIDEO_ENC_RAW_ENG_ENCODE state checking in venc_exit
05/09/07   dhaval  Call video_eng_frame for frame encoding
03/05/07   gopi    Fix zero delta and clip length issues.
09/04/06   Ali     Initial version of VENC

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */
#include "camera.h"
#include "camqdsp.h"
#ifdef FEATURE_MP4_TRANSCODER
 #include "transcoder.h"
#endif
#include "graph.h"
#include "assert.h"
#ifdef FEATURE_VIDEO_ENCODE
#include "venc.h"

typedef enum{
    VIDEO_ENC_RAW_ENG_NOT_INIT=0,
    VIDEO_ENC_RAW_ENG_DSP_INIT,
    VIDEO_ENC_RAW_ENG_INIT,
    VIDEO_ENC_RAW_ENG_START,
    VIDEO_ENC_RAW_ENG_ENCODE,
    VIDEO_ENC_RAW_ENG_PAUSE,
    VIDEO_ENC_RAW_ENG_RESUME,
    VIDEO_ENC_RAW_ENG_STOPPING,
    VIDEO_ENC_RAW_ENG_STOPPED,
    VIDEO_ENC_RAW_ENG_MAX
} venc_state;

typedef struct {
  rex_tcb_type *tcb;
  venc_state state;
  video_status_type last_status;
  vencoder_frame_cb_fn callback;
  video_status_type  videoStatus;
  video_init_type    *raw_init_data;
  void *             client_data;
  void *             frame_cb_data;
  boolean            first_frame;
  venc_frame_type     frame_data;
  uint8*             be_data;
  boolean be_busy;
  boolean first_cb_sent;
  int encode_width;
  int encode_height;
  boolean first_frame_sent_to_be;
  boolean                 *frame_inuse;
  venc_frame_address_type **frame_address;
  int num_buffers;
} venc_context_type;

#define VENC_SIG 0x04000000
LOCAL   venc_context_type  *venc_context;

/* <EJECT> */
/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/


static void transcoder_video_engine_cb
(
  video_status_type  status,        /* Status of callback                 */
  void               *client_data   /* Client data from calling function  */
)
{
    boolean inbranch =FALSE;
    venc_context_type  *context;
 
    context = (venc_context_type *) client_data;

    if(!context || !venc_context)
    {
       MSG_ERROR("Raw engine not running, got a spurious callback",0,0,0);
       return;
    }
    venc_context->videoStatus = status;
    switch(status)    
    { 
    case VIDEO_SUCCESS:
    {
        switch((int32)client_data)
        {
        case VENC_FRAME_CALLBACK:
            venc_context->frame_inuse[(venc_context->num_buffers +4)%VENC_FRAME_BUFFER_SIZE]
                     = FALSE;
            inbranch = TRUE;
            break;
        default:
            break;
        }

        if(inbranch)
            break;
        switch(venc_context->state)
        {
        case VIDEO_ENC_RAW_ENG_DSP_INIT:
            /* Video Engine Initialization complete */
            venc_context->state = VIDEO_ENC_RAW_ENG_INIT;
            (void)rex_set_sigs((rex_tcb_type *)venc_context->tcb, VENC_SIG);
            break;
        case VIDEO_ENC_RAW_ENG_INIT:
            /* Video Engine Start Complete */
            venc_context->state = VIDEO_ENC_RAW_ENG_START;
            (void)rex_set_sigs((rex_tcb_type *)venc_context->tcb, VENC_SIG);
            break;
        case VIDEO_ENC_RAW_ENG_START:
            venc_context->state = VIDEO_ENC_RAW_ENG_ENCODE;
            (void)rex_set_sigs((rex_tcb_type *)venc_context->tcb, VENC_SIG);
            break;
        case VIDEO_ENC_RAW_ENG_STOPPING:
        case VIDEO_ENC_RAW_ENG_PAUSE:
        case VIDEO_ENC_RAW_ENG_RESUME:
        case VIDEO_ENC_RAW_ENG_ENCODE:
            (void)rex_set_sigs((rex_tcb_type *)venc_context->tcb, VENC_SIG);
            break;
        case VIDEO_ENC_RAW_ENG_STOPPED:
            if(venc_context->last_status == VIDEO_LIMIT_REACHED)
            {
                venc_context->callback(venc_context->last_status, 
                                       venc_context->frame_cb_data,
                                       venc_context->client_data);
                if(venc_context)
                {
                   free(venc_context);
                   venc_context = NULL;
                }
            }
            else
            {
                (void)rex_set_sigs((rex_tcb_type *)venc_context->tcb, VENC_SIG);
            }
            break;
         default:
            break;
        }

    }
    break;

    case VIDEO_FAILURE:
    case VIDEO_LIMIT_NEAR:
    case VIDEO_LIMIT_IMMINENT:
        /* Error from Video Engine, will be called in VS task */
        if (venc_context->callback != NULL) 
        {
            venc_context->callback(status, venc_context->frame_cb_data,
                                     venc_context->client_data);
        }

        (void)rex_set_sigs((rex_tcb_type *)venc_context->tcb, VENC_SIG);

        break;
	case VIDEO_LIMIT_REACHED:
        /* Limit reached now we have to call video_eng_exit */
        venc_context->last_status = status;
        venc_context->state = VIDEO_ENC_RAW_ENG_STOPPED;
        video_eng_exit(transcoder_video_engine_cb, (void*)venc_context);
        break;
    default:
        break;
    }
} 

/* <EJECT> */
/*===========================================================================

FUNCTION video_enc_raw_engine_stop

DESCRIPTION
  This procedure stops recording video, saving the output file.
  It also cleans up and exits the raw engine.
  This is a synchronous function, and must be called from the UI task.
  
ARGUMENTS IN    
 NONE
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type venc_exit( void      *client_data )
{  
    video_status_type ret_status;
    boolean           stop_failed = FALSE;
    uint8             i=0;
  
    if(!venc_context)
    {
        MSG_ERROR("Video Encode Raw Engine already stopped",0,0,0);
        return VIDEO_FAILURE;
    }
    /* check the state */
    if(venc_context->state != VIDEO_ENC_RAW_ENG_START && 
       venc_context->state != VIDEO_ENC_RAW_ENG_PAUSE &&
       venc_context->state != VIDEO_ENC_RAW_ENG_ENCODE
       
      )
    {
        if(venc_context)
        {
            free(venc_context);
            venc_context = NULL;
        }
        return VIDEO_FAILURE;
    }
  
    venc_context->state = VIDEO_ENC_RAW_ENG_STOPPING;
    /* Stop the Video Engine */
    venc_context->tcb = rex_self();
    rex_clr_sigs(venc_context->tcb, VENC_SIG);
    ret_status = video_eng_stop(transcoder_video_engine_cb, (void*)venc_context);
    if(ret_status == VIDEO_SUCCESS)
    {
       (void)rex_wait(VENC_SIG);
    }

	if(venc_context->videoStatus != VIDEO_SUCCESS)
    { 
        MSG_ERROR("Video Engine Stop Failed",0,0,0);
        stop_failed = TRUE;
    }
    else
    {
        MSG_MED("Video Engine Stop Success",0,0,0);
    }

    venc_context->state = VIDEO_ENC_RAW_ENG_STOPPED;

    /* Exit the Video Engine */
    rex_clr_sigs(venc_context->tcb, VENC_SIG);
    ret_status = video_eng_exit(transcoder_video_engine_cb, (void*)venc_context);
    if(ret_status == VIDEO_SUCCESS)
    {
        (void)rex_wait(VENC_SIG);
    }

    if(venc_context->videoStatus != VIDEO_SUCCESS)
    { 
        MSG_ERROR("Video Engine Exit Failure",0,0,0);
        stop_failed = TRUE;
    }
    else
    {
        MSG_MED("Video Engine Exit Success",0,0,0);
    }

    /* Free the data */
    for(i=0; i<VENC_FRAME_BUFFER_SIZE; i++)
    {
       if(venc_context->frame_address[i])
       {
           if(venc_context->frame_address[i]->Y_Address)
           {
               free(venc_context->frame_address[i]->Y_Address);
               venc_context->frame_address[i]->Y_Address = NULL;
           }
           free(venc_context->frame_address[i]);
            venc_context->frame_address[i] = NULL;
        }
    }

    if(venc_context->frame_address)
    {
        free(venc_context->frame_address);
        venc_context->frame_address = NULL;
    }
    
    if(venc_context->frame_inuse)
    {
        free(venc_context->frame_inuse);
        venc_context->frame_inuse   = NULL;  
    }
    venc_context->client_data   = NULL;
    venc_context->callback      = NULL;

    if(venc_context)
    {
        free(venc_context);
        venc_context = NULL;
    }

    if(stop_failed)
    {
        return VIDEO_FAILURE;
    }
  
    return VIDEO_SUCCESS;
}
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
video_status_type venc_create
(
 // venc_blob                 ** ppConfigs, 
    video_init_type           init_data,
    uint32                    target_bitrate,
    vencoder_frame_cb_fn      frame_callback_ptr,
    venc_cb_fn                callback_ptr,
    venc_malloc_fn            malloc_ptr,
    venc_free_fn              free_ptr,
    void *                    client_data

)
{  
    video_status_type ret_status;
    //init_data.
    if(venc_context)
    {
        MSG_ERROR("Video Encode Raw Engine already started",0,0,0);
        return VIDEO_FAILURE;
    }

    venc_context = malloc (sizeof (venc_context_type));

    if(venc_context)
    {
      memset(venc_context, 0, sizeof (venc_context_type));
      venc_context->state = VIDEO_ENC_RAW_ENG_NOT_INIT;
      venc_context->frame_address = malloc(sizeof(venc_context->frame_address) * VENC_FRAME_BUFFER_SIZE);
      venc_context->frame_inuse = malloc (sizeof(venc_context->frame_inuse) *VENC_FRAME_BUFFER_SIZE);
    }
    if (venc_context == NULL || venc_context->frame_address == NULL
       || venc_context->frame_inuse == NULL)
    {
        MSG_ERROR ("venc create: Unable to allocate raw engine "
                    "variable structure!",
                     0, 0, 0);
        MSG_HIGH ("start: Exiting function, early", 0, 0, 0);
        return VIDEO_FAILURE;
     }
     venc_context->tcb = rex_self();

     memset(venc_context->frame_address, 0, sizeof(venc_context->frame_address) * VENC_FRAME_BUFFER_SIZE);
     memset (venc_context->frame_inuse,0,sizeof(venc_context->frame_inuse)*VENC_FRAME_BUFFER_SIZE);

     venc_context->encode_height  = init_data.frame_height;
     venc_context->encode_width   = init_data.frame_width;
     venc_context->client_data    = init_data.client_data;
     venc_context->callback       = init_data.frame_callback_ptr;

     /*Over write callback function pointer addres to Venc Address*/
     init_data.frame_callback_ptr = (vencoder_frame_cb_fn) venc_framecallback;

     rex_clr_sigs(venc_context->tcb, VENC_SIG);

     venc_context->state = VIDEO_ENC_RAW_ENG_DSP_INIT;
     venc_context->num_buffers =0;

     MSG_MED("DSP Image Loaded Succesfully",0,0,0);

     ret_status = video_eng_init(&init_data, transcoder_video_engine_cb, (void*)venc_context);
     if(ret_status == VIDEO_SUCCESS)
     {
        (void)rex_wait(VENC_SIG);
     }

     if(venc_context->videoStatus != VIDEO_SUCCESS)
     {  

         MSG_ERROR("Video Engine Initialization failed",0,0,0);
     }
     else
     {
   
         MSG_MED("Video Engine Initialized Succesfully",0,0,0);
     }
     rex_clr_sigs(venc_context->tcb, VENC_SIG);

     /* Start the Video Engine */
     if (venc_context->videoStatus != VIDEO_SUCCESS) 
     {
         return VIDEO_FAILURE;
     }
     else
     {

         ret_status = video_eng_start(transcoder_video_engine_cb, (void*)venc_context);
         if(ret_status == VIDEO_SUCCESS)
         {
            (void)rex_wait(VENC_SIG);
         }

         rex_clr_sigs(venc_context->tcb, VENC_SIG); 

        if(venc_context->videoStatus != VIDEO_SUCCESS)
        {
            MSG_ERROR("Video Engine Start failed",0,0,0);
            return VIDEO_FAILURE;
        }
        else
        {
            MSG_MED("Video Engine Started Succesfully",0,0,0);
         }
    return VIDEO_SUCCESS;
    }
}


/*===========================================================================

FUNCTION video_encode_raw_frame

DESCRIPTION
  This is the main structure that does the encoding for a video frame for offline
  encoding. It is called every frame
  
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  inputBuf: data to be encoded
  frame_duration: Duration of frame
  width:  width of encoded frame
  height: height of encoded frame
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
      
SIDE EFFECTS
  None
  
 
===========================================================================*/
video_status_type venc_get_buffer (venc_frame_address_type *frame_data)
{
    int i=0;
    if(!venc_context->frame_address[venc_context->num_buffers])
    {
        venc_context->frame_address[venc_context->num_buffers] 
            = malloc(sizeof(venc_frame_address_type));
        venc_context->frame_address[venc_context->num_buffers]->Y_Address 
            = malloc (2*venc_context->encode_height * venc_context->encode_width);

        frame_data->Y_Address 
            = venc_context->frame_address[venc_context->num_buffers]->Y_Address;
        frame_data->CbCr_Address
            = venc_context->frame_address[venc_context->num_buffers]->Y_Address +
                       venc_context->encode_height * venc_context->encode_width;
        venc_context->frame_inuse[venc_context->num_buffers] = TRUE;
        venc_context->num_buffers = ((venc_context->num_buffers+1) %VENC_FRAME_BUFFER_SIZE);
    }
    else
    {
        for(i = 0; i < VENC_FRAME_BUFFER_SIZE; i++)
        {
            if(!venc_context->frame_inuse[i])
            {
                venc_context->frame_inuse[i] = TRUE;
                frame_data->Y_Address = venc_context->frame_address[i]->Y_Address;
                frame_data->CbCr_Address = venc_context->frame_address[i]->Y_Address +
                               venc_context->encode_height * venc_context->encode_width;
            }
            if (i == VENC_FRAME_BUFFER_SIZE)
            {
                return VIDEO_FAILURE;
            }
        }
    }
    return VIDEO_SUCCESS;
}



/*===========================================================================

FUNCTION video_encode_raw_init

DESCRIPTION
  This function is used to initialize the offline encoder driver.  
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
video_status_type venc_encode (venc_frame_type *frame_data1)
{

    if (venc_context == NULL ||
        venc_context->state != VIDEO_ENC_RAW_ENG_START) 
    {
        return VIDEO_FAILURE;
    }
    video_eng_frame(
                    frame_data1->framePtr->Y_Address,
                    frame_data1->framePtr->CbCr_Address,
                    frame_data1->image_format,
                    frame_data1->frame_duration,
                    transcoder_video_engine_cb,
                    venc_context
                  );

    //set VENC to encode state
    venc_context->state = VIDEO_ENC_RAW_ENG_ENCODE;

    //return success by defult because video_eng_frame is having void return data type.
    return VIDEO_SUCCESS;
}


/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_start

DESCRIPTION
  This function is used to start the lossless encoding.  
  This function can be called from VIDEO_INIT or VIDEO_STOPPED state
  The state transitions to VIDEO_ENCODING
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
 cb_ptr: Used every frame to send data to the video engine
 void* client_data:  Uniquely identifies the transaction
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type venc_destroy(void* client_data)
{
   return VIDEO_FAILURE;
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
video_status_type venc_set_param
(
    venc_blob *ppBlob_ret,
    venc_dynamic_config_type *pnewConfig
)
{
   return VIDEO_FAILURE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_stop

DESCRIPTION
  This function is used to stop the lossless encoding.  
  This function can be called all states except VIDEO_EXIT
  The state transitions to VIDEO_STOPPED
  
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
video_status_type venc_pause
(
void* client_data
)
{
   return VIDEO_FAILURE;
}


/* <EJECT> */
/*===========================================================================

FUNCTION video_encode_exit

DESCRIPTION
  This function is used to exit the driver.
  The exit can be called in any state.
  The state transitions to VIDEO_EXIT
  
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
video_status_type venc_resume
(
void* client_data
)
{
   return VIDEO_FAILURE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION venc_encode_permitted

DESCRIPTION
  This function is used to find whether next frame should be dropped or not
  
DEPENDENCIES
  None
 
ARGUMENTS IN    
  None
   
ARGUMENTS IN/OUT   
  None
  
ARGUMENTS OUT
  None
      
RETURN VALUE
  VIDEO_SUCCESS  if encoding is permitted
  VIDEO_FAILURE  if encoding is not permitted
  
===========================================================================*/
video_status_type venc_encode_permitted(void)
{
  if(video_encode_permitted(NULL))
  {
      return VIDEO_SUCCESS;
  }
  else
  {
      return VIDEO_FAILURE;
  }
}

void venc_framecallback       (video_status_type    status,
                               vencoder_frame_done_type *pFrame, 
                               void * const         client_data)
{
    venc_context = (venc_context_type *) client_data;

    if(venc_context == NULL)
    {
        ASSERT(0);
        return;
    }

    if ( venc_context->callback && 
         (venc_context->state == VIDEO_ENC_RAW_ENG_ENCODE || venc_context->state == VIDEO_ENC_RAW_ENG_INIT)
       )
    {
        venc_context->callback (status,pFrame,(void *)venc_context->client_data);

        if(status == VIDEO_ENC_FRAME || status == VIDEO_ENC_PARTIAL_FRAME) 
        {
            venc_context->frame_inuse[(venc_context->num_buffers +4)%VENC_FRAME_BUFFER_SIZE] = FALSE;

            if (venc_context->state == VIDEO_ENC_RAW_ENG_ENCODE)
                venc_context->state = VIDEO_ENC_RAW_ENG_START;
        }
    }
}
#endif //FEATURE_VIDEO_ENCODE
