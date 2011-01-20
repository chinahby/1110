#ifndef CAMCORDER_SVCS_H
#define CAMCORDER_SVCS_H
#ifdef FEATURE_VIDEO_ENCODE
/*============================================================================
  
                 Camcorder Services Header File
  
   DESCRIPTION
     This file contains camcorder application specific support for the 
     camera services interface.
  
============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/inc/camcorder_svcs.h#4 $

 when      who    what, where, why   
 --------  -----  ----------------------------------------------------------
 22/01/09  girip  Added camcorder_process_set_default_parms to set global parms 
                  to default values while starting camcorder.
 09/09/08  ps     Featurization of Timestamp special effects support.
 07/08/08  shiven Adding support for loci atom
 03/17/08  anil   Added support for udta atom
 03/11/08  anil   Changed camcorder_svcs_set_parm_timestamp prototype to return
                  camera_ret_code_type
 02/22/08  sanal  Removed compilation warning
 12/17/07  dhaval Removed compilation warning
 12/04/07  Yogi   Introducing cleanup code for recording when 
                  CAMQDSP_SERVICE_UNAVAILABLE event is sent by qdsp service module.
 11/02/07  sanal  Remove compilation error on defeaturizing 
                   FEATURE_VIDEO_ENCODE
 17/11/07  Yogi   Added Post Card Recording Support
 09/06/07  jdas   Dynamic Overlay support in camcorder
 05/08/07  Kadhir Added support in camcorder for dynamic voting of 
                  high arm clock for QVGA resolution 
 03/12/07  jdas   Added camera_recording_cleanup() declaration
 02/27/07  dhaval Define CAMCORDER_VIDEO_ROTATION macro.
 11/13/06  Kadhir Creation.

============================================================================*/

/*============================================================================
                        STRUCTURE DEFINITIONS
============================================================================*/
typedef enum{
  SPECIAL_EFFECTS_DONE,
  MDP_FINISHED_BLITTING,
  HANDSHAKE_RECEIVED
} output1_doneevent_type;

typedef enum{
  SPECIAL_EFFECTS_DONE2,
  HANDSHAKE_RECEIVED2
} output2_doneevent_type;

typedef struct
{
  boolean encode_output2_special_effects_done_flag;
  boolean encode_output2_handshake_flag;
} output2_doneevent_flags_type;

typedef struct
{
  boolean encode_special_effects_done_flag;
  boolean encode_output1_handshake_flag;
} output1_donevent_flags_type;


typedef enum
{
  CAMERA_PAUSING_STATE_NOT_PAUSED = 0,
  CAMERA_PAUSING_FLUSHING_BUFFER,
  CAMERA_PAUSED
} camera_pausing_state_type;
#endif //FEATURE_VIDEO_ENCODE
/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
#define CAMERA_MINIMUM_BACKEND_BUFFER_SIZE  3
#define CAMERA_DELAYED_ENCODE_FRAME_SIZE    2
#define CAMERA_TOTAL_BACKEND_BUFFER_SIZE   (CAMERA_MINIMUM_BACKEND_BUFFER_SIZE + \
                                            CAMERA_DELAYED_ENCODE_FRAME_SIZE)
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE */

#ifdef FEATURE_VIDEO_ENCODE
#ifdef FEATURE_UIONE_HDK
#define CAMCORDER_VIDEO_ROTATION    90
#endif //FEATURE_UIONE_HDK

#ifdef FEATURE_VIDEO_ENCODE
#define CAMCORDER_QVGA_SIZE (320 * 240)
#endif
/*============================================================================
                      GLOBAL VARIABLE DECLARATIONS
============================================================================*/
extern VideoBufferHandle  encodeBufferHandle;
extern VideoBufferHandle  output1BufferHandle;

extern boolean                   camera_video_engine_start_requested;
extern boolean                   camera_video_engine_stop_requested;
extern camera_pausing_state_type camera_video_engine_paused;
extern boolean                   camera_video_engine_failed;

// Used for time based A/V Sync in Camcorder
extern glb_avs_info_type glb_avs_info;

/*============================================================================
                     GLOBAL FUNCTIONS DECLARATIONS
============================================================================*/
boolean camera_recording_vfe_reconfig_permitted(boolean x);
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
boolean camcorder_svcs_clr_overlay(ipl_image_type**);
camera_ret_code_type camcorder_create_preview_overlay(ipl_image_type*,
                                                      uint16,uint16);
camera_ret_code_type camcorder_svcs_set_overlay(ipl_image_type **, 
                                                ipl_image_type *,uint32,
                                                void*);
camera_ret_code_type camcorder_set_video_eng_overlay (uint16, 
                                                      ipl_image_type*);
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

void camera_recording_cleanup(camera_cb_type cb, int32 parm4);

void camcorder_svcs_set_parm_fps(uint16 numerator, uint16 denominator);
camera_ret_code_type camcorder_svcs_set_parm_audio_fmt(int32 parm1, int32 parm2);

#ifdef FEATURE_VIDEO_ENCODE_FADING
void camcorder_svcs_set_parm_fading(int32 parm);
#endif

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
void camcorder_svcs_set_parm_transit(int32 parm);
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
camera_ret_code_type camcorder_svcs_set_parm_timestamp(int32 parm);
#endif /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP */

void camcorder_svcs_set_parm_space_limit(int32 parm);

camera_ret_code_type camcorder_svcs_set_parm(camera_parm_type id, int32 parm);
camera_ret_code_type camcorder_svcs_get_parm(
                          camera_parm_type       id,
                          camera_parm_info_type *parm
                          );


void camcorder_svcs_process_video_hanshake( void );
void camcorder_svcs_update_vfe_filter( void );

void camcorder_process_start_record ( camera_cb_f_type callback, void *client_data);
void camcorder_process_pause_record ( void );
void camcorder_process_resume_record ( void );
void camcorder_process_stop_record ( void );
void camcorder_process_qdsp_start_record_msg(uint16 video_encode_rotation);

void camera_process_qdsp_video_frame_done_msg (uint16 *msg);
void camera_process_qdsp_video_encode_selection_done_msg ( void );
void camera_process_qdsp_video_encode_config_done_msg ( void );
void camera_process_qdsp_video_encode_idle_done_msg ( void );
void camera_process_qdsp_video_encode_feature_update_done_msg ( void );


void camcorder_process_qdsp_output2_msg(Camera_EndOfFrameMessageType* msg);
void camcorder_process_qdsp_config_done_msg
(
  CAMQDSP_VideoEncStateType * vidEncState
);
void camcorder_process_alloc_memory(uint32 camera_encode_buffer_size);

void camcorder_config_vfe( void );


void output2enc_ack2_criteria(output2_doneevent_type event);
boolean camcorder_qdsp_cb
(
  Camera_EndOfFrameMessageType *endOfFrameMessagePtr, 
  CAMQDSP_ResponseMsgType responseMsg
);

camera_ret_code_type camcorder_svcs_start_record
(
  video_fmt_type               file_format,       /* i.e. MP4, AVI, ...   */
  video_fmt_stream_video_type  video_format,      /* i.e. MPEG-4, ...     */
  video_fmt_brand_type         video_brand,
  boolean                      short_header,
  video_fmt_stream_audio_type  audio_format,      /* i.e. EVRC, AMR, ...  */
#ifdef FEATURE_CAMERA_V770
  boolean                      bAudioPostCard,
#endif
#ifdef FEATURE_VIDEO_MULTI_UUID
  video_uuid_info_type        *uuid_info,
#else /* FEATURE_VIDEO_MULTI_UUID */
  void *                       uuid_data,
  uint32                       uuid_data_size,
  uint8 *                      uuid_atom,
#endif /* FEATURE_VIDEO_MULTI_UUID */
  video_profile_type           profile,          /* video profile        */
  int32                        user_bitrate,     /* bitrate specified by usr */
  video_handle_type           *destination,      /* where output goes    */
  camera_cb_f_type             callback,
  void                        *client_data
);

camera_ret_code_type camcorder_svcs_pause_record ( void );
camera_ret_code_type camcorder_svcs_resume_record ( void );
camera_ret_code_type camcorder_svcs_stop_record ( void );

#ifdef FEATURE_MP4_TRANSCODER
camera_ret_code_type camcorder_svcs_enable_offline( void );
#endif

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
void camera_video_transition_effects_store
      (VideoBuffersYCbCrAddressPairsType *lastAddress);
#endif

void camcorder_process_set_default_global_parms( void );

/*===========================================================================

FUNCTION      CAMERA_PROCESS_VIDEO_ENGINE_CB

DESCRIPTION
              Process video engine callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camcorder_process_video_engine_cb
(
  camera_cb_f_type  cb,
  void             *st,
  VFE_SkipBitMaskType *runningOutput2SkipPatternPtr,
  int16 *encoder_acked
);

/*===========================================================================
FUNCTION      CAMERA_PROCESS_AUDIO_READY_CB
DESCRIPTION
              Initiates camera capture based on first audio data
              callback and video configuration being complete.
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void camcorder_process_audio_ready_cb ( void );

#ifdef CAMCORDER_VFE_RECONFIG_TEST
void vfe_process_reconfigure_unit_test( void );
#endif

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

/*===========================================================================

FUNCTION      CAMCORDER_VIDEO_ENGINE_ERROR_RECOVERY

DESCRIPTION
              This function helps camera services to recover from
              video engine failures, given that video engine has began to
              recover and is cleaning up. This added information about video
              engine indicates that contents of N-frames buffer and output2
              acks are no longer necessary as video recording is stopping.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camcorder_video_engine_error_recovery (video_status_type status);

#endif /* FEATURE_VIDEO_ENCODE */
#endif /* CAMCORDER_SVCS_H */

