#ifndef VENC_H
#define VENC_H
/*===========================================================================

    V I D E O    E N C O D E R    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface 
  with the video encoder driver.

REFERENCES
  Video Encoder ISOD: 00-XXXXX-XX

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
  
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/venc.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/08   dhaval  Removed qtv isr specific code
10/12/07   dhaval  Added venc_encode_permitted()
09/04/06   Ali     Initial version of VENC

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "video_common.h"
#include "video_enc.h"
#include "videoeng.h"

#ifdef T_ARM
#include "customer.h"
#endif

#ifdef FEATURE_VIDEO_ENCODE
/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* No of frames that VENC is going to handle */
#define VENC_FRAME_BUFFER_SIZE 5

/* Callback value which will be received from video engine */
#define VENC_FRAME_CALLBACK    99

/* Frame types - how was this frame encoded?*/
typedef enum
{
  VENC_FRAMETYPE_I,
  VENC_FRAMETYPE_P
}
venc_ivop_type;

/* Structure used to pass in VFE stats */
typedef struct venc_vfe_stats_type_struct
{
  uint32  stat1;
}venc_vfe_stats_type;   

/* Structure used to fill in Y and CbCr addresses*/
typedef struct venc_frame_address_type_struct
{
  uint8 *Y_Address;      /* Luma Address   */
  uint8 *CbCr_Address;   /* Chroma Address */
} venc_frame_address_type;

/* Stats from BE */
typedef struct venc_frame_stats_type_struct
{
  uint32 psnr;  /*psnr passed back from BE*/
} venc_frame_stats_type;

/* Structure used to manage frame buffers in the encorder*/
typedef struct venc_frame_type_struct
{
  uint32 frame_duration;      /* If 0, time_incr and time_resolution     */
                              /* specified with init_data in venc_create */
                              /* are used to set the frame duration. If  */
                              /* non-zero, this value is used to set     */
                              /* number of ticks between consecutive     */
                              /* frames given in units of time_resolution*/ 
                              /* (provided at venc_create)               */
  ipl_col_for_type 
           image_format;   /* image format : line-packed of mb-packed */
  venc_ivop_type    frame_type;
  venc_vfe_stats_type    *vfe_stats_ptr; /* VFE stats */
  venc_frame_address_type *framePtr; /* Pointer to frame data */
  void*             frame_client_data;    /* frame specific data  */



} venc_frame_type;


/* Callback function used to pass frame buffers to the application, and
** indicate status of last frame encode
**
** Parameters:
**
**   status:  status of the call. Whether the call was a success or not
**   pFrame:  This is an optional parameter, and will be filled in when 
**            output method in init will be set as VIDEO_METHOD_CLIENT
**   client_data: Frame specific data.
*/
typedef union venc_dynamic_config_union
{
  video_enc_config_ops_type      op;  
  video_enc_config_bitrate_type  bitrate;
  video_enc_config_iperiod_type  iperiod;
  video_enc_config_timing_type   timing;
} venc_dynamic_config_type ;

/* Frame types - how was this frame encoded?*/
//typedef enum
//{
  //VENC_PARAM_ALL,
///  VENC_PARAM_MAX
//}
//venc_ivop_type;

/* The VENC_BLOB (Binary Large OBject) is a structure used to contain 
** large opaque amounts of data. The encoder will allocate these when 
** the application requests big chunks of data. The pData pointer inside 
** them should be freed with a call to vdec_free_blob. 
*/ 
typedef struct 
{ 
  uint32 id; /* a code used by venc to identify the contents of 
             this blob - applications should ignore this */ 
  uint32 size; /* the number of bytes stored in this blob */ 
  byte  * pData; /* the bytes */ 
} 
venc_blob; 

/* Callback function used to notify the client of the status of last 
** call to the encoder.
**
** Parameters:
**
**   status:  status of the call. Whether the call was a success or not.
**   client_data: Caller specific data.
*/
typedef void (*venc_cb_fn)( video_status_type    status,
                            void * const         client_data);

/* Memory allocator and deallocator passed into the encoder 
** must be of these types. 
*/ 
typedef void * (*venc_malloc_fn)( uint32 uSize ); 
typedef void (*venc_free_fn)( void *ptr ); 



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
  venc_cb_fn              callback_ptr,
  venc_malloc_fn             malloc_ptr,
  venc_free_fn              free_ptr,
  void *                    client_data
);



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
video_status_type venc_get_buffer (venc_frame_address_type *frame_data);

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
video_status_type venc_encode (venc_frame_type *frame_data);


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
video_status_type venc_destroy(void* client_data);
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
);

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
);


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
);

video_status_type venc_exit (void *client_data);

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
  VIDEO_SUCCESS  if operation went fine
  VIDEO_FAILURE    otherwise
  
===========================================================================*/
video_status_type venc_encode_permitted(void);

void venc_framecallback(video_status_type    status,
                        vencoder_frame_done_type *pFrame, 
                        void * const         client_data);

#endif /* FEATURE_VIDEO_ENCODE */
#endif /* VENC_H */

