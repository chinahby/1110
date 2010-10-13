/* =========================================================================
   Camcorder Services

   This file contains the definitions needed for interaction
   between camera and camcorder

   Copyright (c) 2006 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/rel/1.1.0/src/camcorder_svcs.c#2 $

 when         who    what, where, why
 --------  -----  ----------------------------------------------------------
 03/31/10   kamit    Changes merged for simultaneous handling of failure and stop
 04/22/09  ps     Used QCAMERA client for voting high ARM clock
 04/14/09  ps     Changes done after changed in IPL for frame overlay
 03/26/09  ps     Added check for stopping video engine with No video and fade out effect
 03/17/09  ps     Fixed RVCT compiler warning
 02/25/09  kamit  Registering full screen frame size for display of Overlay image 
                  at MDP_LAYER_1. camera_svcs_display_frame_overlay(TRUE) is called in
                  camcorder_create_preview_overlay()
 01/20/09  sanal  Handle fast pause resume
 11/02/09  kamit  Added a check before copying overlay image data to preview. 
 22/01/09  girip  Added camcorder_process_set_default_parms to set global parms 
                  to default values while starting camcorder.
 12/17/08  sanal  Typecast to void videoeng API calls if return type is not handled
 12/23/08  girip  Featurise audio recording for FTM camcorder
 12/22/08  ps     Removed blocking of QVGA for short header
 11/24/08  sanal  stop video engine if DSP encode selection fails.
                  Remove redundant vote for BUS sleep.
 10/10/08   anil     RVCT compiler warning fixes
 09/09/08  ps     Featurization of Timestamp special effects support.
 09/01/08  shiven Camcorder error recovery after camera_config_vfe() failure 
 08/25/08  sanal  Removed compilation warnings.
 08/19/08   ps      Done changes for Runtime TE detection
 07/08/08  shiven Adding support for loci atom
 07/04/08  ps	  Removed RVCT compiler warnings
 06/30/08  yogi   Modified code to not initiate record start operation, 
                  when video engine is stopping
 05/15/08  anil   proper featurised FEATURE_VIDEO_SETATOMS_DYNAMIC
 05/14/08  sanal  Removed resetting of camera_ui_tcb while in use
 05/08/08  shiven Added support for 5fps in camcorder_process_resume_record()
 03/17/08  anil   Added support for udta atom
 03/11/08  anil   Fixed setting of time_stamp_on value properly in 
                  camcorder_svcs_set_parm_timestamp and support to return
                  time_stamp_on value to camera_svcs layer
 03/06/08  jdas   Klocwork error fix
 02/29/08  anil   Fixed compilation error when "FEATURE_CAMERA_V770" is undefined
 02/11/08  sanal  Adding Idiag support for frames dropped in a recorded clip 
 01/23/08  Yogi   Do not process FLUSH_CB and ENCODE_FRAME_CB callbacks when 
                  error recovery is in progress.
 12/04/07  Yogi   Removed the screen flicker when we move from Record Menu 
                  to Preview Menu. Changes in camera_recording_cleanup function. 
 12/04/07  Yogi   Introducing cleanup code for recording when 
                  CAMQDSP_SERVICE_UNAVAILABLE event is sent by qdsp service module.
 11/02/07  sanal  Removed compiler error when FEATURE_VIDEO_ENCODE is disabled
 11/17/07  Yogi   Added Post Card Recording Support
 11/19/07  dhaval Removed compilation warnings
 11/14/07  jdas   DCVS changes for SC2X target
 11/1/07   jdas   Merged from branch:
                  VFE is moved to idle state before proceeding with recording stop 
                  operation, inorder to prevent AHB overflow error.
 10/03/07  Yogi   Updating the file with review comments for Audio only recording 
                  related changes
 09/26/07  arv    Added featurization for VFE_25
 09/06/07  jdas   Dynamic Overlay support in camcorder
 09/03/07  Yogi   Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
 07/24/07  yogi   Added support for audio only recording   
 07/10/07  kadhir Enabled dynamic voting for QVGA recording
 06/29/07  kadhir Added max and dynamic bitrate support for camcorder recording
 06/26/07  arv    New sleep API for Camcorder
 05/08/07  Kadhir Added support in camcorder for dynamic voting of 
                  high arm clock for QVGA resolution 
 04/13/07  Gopi   undo changes of previous check-in for UI one target.
 03/28/07  dhaval Prevent rotation checking for UI one target.
 03/01/07  jdas   Fixed problem of recording_cleanup while AHB overflow error
                  occurred during the process of stop record
 02/27/07  dhaval Prevent 90 degree rotation for H.263 video recording 
 02/22/07  Gopi   Changes to prevent Flashing screen in camcorder app
 01/30/07  pavan  By default reorder_atom is made to false
 12/13/06  dhaval Fixed the stop recording problem when fadout effect is selected
                  and recording is in paused state
 11/30/06  jdas   Removed camera_video_engine_start_requested check from
                  in camcorder_svcs_stop_record
 11/28/06  Pavan  Added Support for Movieedit Interface
 11/21/06  Shahed AV Sync related changes to fix fragmented file recording.
 11/14/06  kadhir Removed compile error for 6280.
 11/13/06  kadhir Extracted camcorder specific functions from camera_svcs.c
============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "assert.h"
#include "camera_3a_shared.h"
#include "event.h"
#include "exif.h"
#include "graph.h"
#include "msg.h"
#include "sleep.h"
#include <stdlib.h>
#include "task.h"
#include "clk.h"
#include "AEEstd.h"
#include "camcorder_msg.h"

#ifdef FEATURE_VIDEO_ENCODE
#include "video_buffers.h"
#include "video_enc_dsp_if.h"
#include "videoeng.h"
#include "video_enc_aux.h"
#include "video_enc.h"
#include "camera_svcs.h"
#include "camcorder_svcs.h"

/* To obtain hardware version (chip id) or */
#if defined FEATURE_H264_ENCODER || defined FEATURE_CAMCORDER_AAC_REC
#include "hw.h"
#define CHECK_SUPPORTED_HW_REVISION_REPORT_ERROR { hw_rev_type hardware_revision_number;\
                            hw_partnum_version(&hardware_revision_number);\
                            if(((hardware_revision_number.hw_register >> 31) & 0x01) == 1 )\
                            return CAMERA_NOT_SUPPORTED; \
                            }
#endif

#define MSG_CAMQDSPIO(x,a,b,c)

/*============================================================================
                     LOCAL DATA DEFINITIONS
============================================================================*/
#ifdef CAMCORDER_VFE_RECONFIG_TEST
static clk_cb_type vfe_reconfigure_clk = {0};
#endif
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
static boolean            time_stamp_on = FALSE;
#endif
static boolean            camcorder_audio_only_record = FALSE;
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
/*===================================================================
   State machine for video encoder to handle VFE reset properly

   State transitions allowed:

    RECONFIGURABLE ----(OUTPUT2)--->DO_NOT_RECONFIGT
    DO_NOT_RECONFIG --(vfe_reset)->RECONFIG_REQUESTED (no vfe_reset call)
    RECONFIG_REQUESTED-(handsake)->RECONFIGURABLE (vfe_reset)
    DO_NOT_RECONFIG---(handshake)->RECONFIGURABLE (ack2)

===================================================================*/

typedef enum
{
  CAMERA_RECORDING_VFE_RECONFIGURABLE,
  CAMERA_RECORDING_VFE_DO_NOT_RECONFIG,
  CAMERA_RECORDING_VFE_RECONFIG_REQUESTED,
  CAMERA_RECORDING_VFE_ERROR,
  CAMERA_RECORDING_VFE_RESET_MAX
} CAMERA_RECORDING_VFE_RESET_STATE_TYPE;

static CAMERA_RECORDING_VFE_RESET_STATE_TYPE
   camera_recording_vfe_reset_state = CAMERA_RECORDING_VFE_RECONFIGURABLE;
static boolean camera_record_vfe_reset_block_be_restart = FALSE;

#ifdef FEATURE_NEW_SLEEP_API
extern sleep_okts_handle camera_sleep_okts_handle;
extern boolean camera_sleep_handle_register;
#endif

#ifdef FEATURE_VIDEO_ENCODE_FADING
/*  True if we are generating fade-in or fade-out special effects
*/
static boolean             efx_fade_in = FALSE;
static boolean             efx_fade_out = FALSE;
static boolean             efx_fade_out_running;
#endif
boolean    first_call = TRUE;


#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
/*  Transition effects are implemented between pause and resume.
**   ... called "special fade"
*/
static uint32 efx_special_fade;

/* the duration of video transition in milliseconds
*/
uint32   VidEncTransition_msec = VIDENC_TRANSITION_DURATION;


#endif  /* ifdef FEATURE_VIDENC_TRANSITION_EFFECTS */

static rex_tcb_type *camera_ui_tcb;

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
/* Structure containing all variables for transition effects
*/

static struct {
  /* this is VidEncTransition_flag that was sampled during the
  ** execution of pause and stored in SampledTransitionType.
  */
  VidEncTransitionType  SampledTransitionType;
  uint32                targetTransitionMsec; /* msec when it ends*/
  boolean               bTransitionIsActive;  /* true if transitioning */

  /*  Keep a pointer to the last frame from the previous record.
  **
  **  NULL acts as a flag for this pointer meaning it has not
  **  been assigned.
  **
  **  This frame is stored in the heap here between pause and resume.
  */
  VideoBuffersYCbCrAddressPairsType LastFrameForTransition;

} VidEncTransitionStruct;


#endif  /* FEATURE_VIDENC_TRANSITION_EFFECTS */

/*********************************************************
** Globals for frame overlay in Qcamcorder app          **
*********************************************************/
extern boolean camera_send_vfe_idle_and_wait(void);

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef CAMERA_USES_OEM_HEAP
#define CAMCORDER_ALLOC_NO_ZMEM (0x0)
#else
#define CAMCORDER_ALLOC_NO_ZMEM (0x80000001)
#endif

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
static vse_overlay_param_type camera_video_eng_overlay;
static vse_ptr_type            camera_special_effects_ptr=NULL;
/* Used to save the allocated preview_overlay in case of Movie mode */
static ipl_image_type   *preview_overlay_buffer = NULL;
static boolean           overlay_allocated = FALSE;
static ipl_image_type    temp_overlay;

static struct
{
  vse_packet_data_type   packet;
  vse_type               vse_struct;
  ipl_image_type         input_buffer;
} camera_special_effects_api_params =
  {
    { NULL,                               /* *output buffer */
      NULL,                               /* *input buffer */
      0,                                  /* msec */
      VSE_PACKET_STATUS_UNINITIALIZED,    /* status */
      BUFFER_FREE_STATUS_NOT_SET,         /* free buffer flag */
      NULL,                               /* *param           */
    },
    {
      VSE_ENGINE_UNINITIALIZED,           /* engine status  */
      0,0,                                /* npix_x, npix_y */
      NULL,                               /* params         */
      #ifdef FEATURE_CAMCORDER_MALLOC
        NULL,                             /* *malloc_context */
      #endif
    },
    {
      0,0,                                /* dx, dy   */
      IPL_YCrCb420_LINE_PK,                 /* MB packed*/
      NULL,                               /* img ptr  */
      NULL,                               /* clr ptr */
    }
  };
#ifdef FEATURE_VIDEO_ENCODE_FADING
static struct
{
  vse_packet_data_type packet;
  vse_type             vse_struct;
  vse_fadeInOutParamsType
                       params;

} camera_fadein_out_api_params;
#endif /* FEATURE_VIDEO_ENCODE_FADING */

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
static struct
{
  vse_packet_data_type packet;
  vse_type             vse_struct;
  vse_fadeInOutParamsType
                       params;
} camera_timestamp_params;
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

typedef enum
{
  CAMERA_VIDEO_ENGINE_INIT_CB,
  CAMERA_VIDEO_ENGINE_START_CB,
  CAMERA_VIDEO_ENGINE_PAUSE_CB,
  CAMERA_VIDEO_ENGINE_RESUME_CB,
  CAMERA_VIDEO_ENGINE_STOP_CB,
  CAMERA_VIDEO_ENGINE_EXIT_CB,
  CAMERA_VIDEO_ENGINE_UUID_CB
 #ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  ,
  CAMERA_VIDEO_ENGINE_ENCODE_FRAME_CB,
  CAMREA_VIDEO_ENGINE_FLUSH_CB       /* flush the remaining frame in buffer*/
 #endif
} camera_video_engine_cb_type;

static void camera_video_engine_cb (
  video_status_type  status,        /* Status of callback                 */
  void               *client_data   /* Client data from calling function  */
);
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
static void camera_audio_ready_cb ( void );
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

extern boolean camera_set_preview_buffers (void);
extern void camera_set_preview_headers (void);

static boolean            camcorder_video_engine_recovering = FALSE;
static video_enc_3x3_luma_filter_type video_encode_3x3filter;
boolean                   camera_video_engine_failed = FALSE;
camera_pausing_state_type camera_video_engine_paused;
static boolean                   camera_video_engine_pause_requested;
boolean                   camera_video_engine_stop_requested;
boolean                   camera_video_engine_start_requested;
static boolean                   camera_video_engine_resume_requested;
#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
static boolean                   camera_video_engine_throttle = FALSE;
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */
/* Set this flag to skip processing of N buffered frames */
static boolean                   camera_video_skip_buffer_flag = FALSE;

static video_fmt_type               camera_file_format;
static video_fmt_stream_video_type  camera_video_format;
static video_fmt_stream_audio_type  camera_audio_format;
static video_profile_type           camera_profile;
static video_handle_type            camera_destination;
static video_fmt_brand_type         camera_file_brand;
static boolean                      camera_short_header;
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
static video_eng_user_data_atom_type *camcorder_useratom_info;
static camera_ret_code_type camcorder_svcs_set_parm_udat_atom(int32 parm);
#endif
#ifdef FEATURE_VIDEO_MULTI_UUID
static video_uuid_info_type        *camera_uuid_info;
static uint32                       camera_uuid_count;
#else /* FEATURE_VIDEO_MULTI_UUID */
static uint8                       *camera_uuid;
static void                        *camera_uuid_data;
static uint32                       camera_uuid_data_size;
#endif /* FEATURE_VIDEO_MULTI_UUID */

static boolean                      camera_encoding_frame;
static int32                        camera_user_bitrate;
static uint8                        camera_space_limit;

typedef struct
{
  uint32 numerator   : 16;
  uint32 denominator : 16;
} camcorder_fps_type;

static camcorder_fps_type camcorder_fps;

#ifdef FEATURE_CAMCORDER_AAC_REC
/* Globals for advanced AAC settings */
static video_eng_audio_channel_type      camera_audio_channel = VIDEO_ENG_AUDIO_CHANNEL_MONO_2_MONO;
static video_eng_audio_sample_rate_type  camera_audio_sample_rate = VIDEO_ENG_AUDIO_SAMPLE_RATE_16000;
static video_eng_audio_profile_type      camera_audio_profile = VIDEO_ENG_AUDIO_PROFILE_HIGH;
#endif /* FEATURE_CAMCORDER_AAC_REC */

VideoBufferHandle  encodeBufferHandle  = {0};
VideoBufferHandle  output1BufferHandle = {0};

/* Flag indicates that we are encoding using output1
** instead of output 2.
**
** The only condition for this is
**  1. video encoding
**  2. special effects engine running
**  3. MDP is not able to display the equavalent special effect
**     that is being done for output2.
**
** Characteristics:
**  * Output2 will be turned off
**  * Output1BufferHandle will be used for NFrames delay implementation
**    using output1 results
**  *
*/
static void output2enc_ack2_reset( void );

static void camera_recording_vfe_process_output2( void );
//static boolean camera_recording_vfe_reconfig_permitted(boolean x);
static boolean camera_recording_vfe_ack2_permitted( void );
static void camera_recording_vfe_init( void );

typedef struct Camera_MP4_Encode_Frame_Struct
{
  uint16 *buf_ptr;
  uint32  time;
} Camera_MP4_Encode_Frame_Type;

static Camera_MP4_Encode_Frame_Type mp4_encode_frame_parm;

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
static void camera_initialize_timestamp( void );
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
static void camcorder_svcs_initiate_backend_encode( void );

#ifdef FEATURE_VIDEO_ENCODE_FADING
  static void camera_initialize_fadeInOut( void );
#endif

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
/*static void camera_video_transition_effects_store
      (VideoBuffersYCbCrAddressPairsType *lastAddress);*/
static void camera_start_transition_effects( void );
#endif

static int output2BuffersQueueSize;

/*===========================================================================

FUNCTION      CAMERA_START_RECORD

DESCRIPTION
              Video recording is started.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EVT_CB_FRAME when VFE frame is available.
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_ABORT.
   5. CAMERA_EXIT_CB_DSP_IDLE
   6. CAMERA_EXIT_CB_DONE if recording is terminated.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 code not reference */
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
)
{

#ifdef T_IO_CARD
#error code not present
#else
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_START_RECORD, 0, 0, __LINE__);
  MSG_HIGH("camera_start_record, camera_state %d", camera_state, 0, 0);
  if (camera_video_engine_stop_requested)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#ifdef FEATURE_H264_ENCODER
  if(video_format == VIDEO_FMT_STREAM_VIDEO_H264)
  {
    CHECK_SUPPORTED_HW_REVISION_REPORT_ERROR;
  }
#endif

#ifdef FEATURE_CAMCORDER_AAC_REC
  if(audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
  {
    CHECK_SUPPORTED_HW_REVISION_REPORT_ERROR;
  }
#endif


#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#ifdef FEATURE_QVPHONE
  if (camera_qvp_enabled)
  {
    MSG_ERROR ("Invalid state", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_STATE);
    ret_code = CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_QVPHONE */
  if (camsensor_initialized == FALSE)
  {
    ret_code = CAMERA_NO_SENSOR;
    event_report (EVENT_CAMERA_NO_SENSOR);
  }

  /* Do not allow short header and 90/270 rotation.
     Also do not allow short header and QVGA size */
  if (short_header)
  {
    if (
        camera_odd_encode_rotation ||
        camera_dx == 96            ||
        camera_dx == 80)
      {
         MSG_ERROR ("Unsupported Header/Rotation combination", 0, 0, 0);
         event_report (EVENT_CAMERA_INVALID_STATE);
         ret_code = CAMERA_INVALID_STATE;
      }
  }
  /*Check for Aspect Ratio's of Output1 and output2
  Reason being:
  HW cannot fully support two outputs with very different aspect ratios.
  In particular, we cannot support the following configurations:
  W1 > W2 & H1 <H2; or W1 < W2 & H1>H2;
  where W1, H1, W2, H2 are the widths and heights of output 1 and output 2, respectively.
  */
  if( ((camera_preview_dx > camera_dx) && (camera_preview_dy < camera_dy))
      ||
      ((camera_preview_dx < camera_dx) && (camera_preview_dy > camera_dy))
     )
  {
      MSG_ERROR ("Unsupported Aspect Ratio for Output1 and Output2", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_CONFIG_PARM);
      /*Return Error if above condition is encountered*/
      ret_code =  CAMERA_INVALID_PARM;
  }


#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  /* Terminate recording if frame overlay size does not match encode size */
  /* Currently camcorder only supports landscape overlays */
  if(camera_video_eng_overlay.frame != NULL &&
     !(camera_video_eng_overlay.frame->dx == camera_dx &&
        camera_video_eng_overlay.frame->dy == camera_dy))
  {
    MSG_ERROR("Overlay frame size does not match encode frame size: %d,%d",
              camera_video_eng_overlay.frame->dx,
              camera_video_eng_overlay.frame->dy,
              0);

    event_report (EVENT_CAMERA_INVALID_STATE);
    ret_code = CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_START_RECORD);
    return ret_code;
  }

  camera_file_format    = file_format;
  camera_video_format   = video_format;
  camera_audio_format   = audio_format;
  camera_profile        = profile;
  camera_file_brand     = video_brand;
  camera_short_header   = short_header;
  camera_destination    = *destination;
  camera_take_images    = FALSE;

#ifdef FEATURE_VIDEO_MULTI_UUID
  camera_uuid_info      = uuid_info;
  camera_uuid_count     = 0;
#else /* FEATURE_VIDEO_MULTI_UUID */
  camera_uuid_data      = uuid_data;
  camera_uuid_data_size = uuid_data_size;
  camera_uuid           = uuid_atom;
#endif /* FEATURE_VIDEO_MULTI_UUID */

  camera_encoding_frame = FALSE;
  camera_user_bitrate   = user_bitrate;

#if (defined (FEATURE_CAMERA_V770) && defined (FEATURE_POSTCARD_RECORDING))
#error code not present
#endif /* FEATURE_CAMERA_V770 & FEATURE_POSTCARD_RECORDING */

  if(camera_video_format == VIDEO_FMT_STREAM_VIDEO_NONE)
  {
    camcorder_audio_only_record = TRUE;
  }
  else
  {
    camcorder_audio_only_record = FALSE;
  }


  ackOutput2.eofAck2[0].Y_Address    = NULL;
  ackOutput2.eofAck2[0].CbCr_Address = NULL;
  /* Reset the output2 flags*/
  output2enc_ack2_reset ();

  graph_queue_camera_func((uint32)CAMERA_FUNC_START_RECORD, (void *)callback,
      client_data, 0, 0, 0, 0);
  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_start_record */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_PAUSE_RECORD

DESCRIPTION
              Video recording is paused.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in RECORDING state
  Use start_record callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camcorder_svcs_pause_record ( void )
{
#ifdef T_IO_CARD
#error code not present
#else
  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_PAUSE_RECORD, 0, 0, __LINE__);
  MSG_HIGH("camera_pause_record, camera_state %d", camera_state, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#ifdef FEATURE_QVPHONE
  if (camera_qvp_enabled)
  {
    MSG_ERROR ("Invalid state", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_QVPHONE */
  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_RECORDING)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  if (camera_video_engine_paused != CAMERA_PAUSING_STATE_NOT_PAUSED)
    {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
    }

  graph_queue_camera_func((uint32) CAMERA_FUNC_PAUSE_RECORD, (void *)0, (void *)0, 0, 0, 0, 0);

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_pause_record */


/*===========================================================================

FUNCTION      CAMERA_RESUME_RECORD

DESCRIPTION
              Video recording is resumed.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in RECORDING state.
  Use start_record callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camcorder_svcs_resume_record ( void )
{
#ifdef T_IO_CARD
#error code not present
#else
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_RESUME_RECORD, 0, 0, __LINE__);
  MSG_HIGH("camera_resume_record, camera_state %d", camera_state, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#ifdef FEATURE_QVPHONE
  if (camera_qvp_enabled)
  {
    MSG_ERROR ("Invalid state", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_QVPHONE */
  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_RECORDING)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  if (camera_video_engine_paused != CAMERA_PAUSED)
    {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
    }

  graph_queue_camera_func ((uint32) CAMERA_FUNC_RESUME_RECORD, (void *)0, (void *)0, 0, 0, 0, 0);

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_resume_record */


/*===========================================================================

FUNCTION      CAMERA_STOP_RECORD

DESCRIPTION
              Video recording is stopped.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in record state.
  Use start_record callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is taken
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camcorder_svcs_stop_record ( void )
{
#ifdef T_IO_CARD
#error code not present
#else
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_STOP_RECORD, 0, 0, __LINE__);
  MSG_HIGH("camera_stop_record, camera_state %d", camera_state, 0, 0);

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

#ifdef FEATURE_QVPHONE
  if (camera_qvp_enabled)
  {
    MSG_ERROR ("Invalid state", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#endif /* FEATURE_QVPHONE */

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_RECORDING)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

#ifdef FEATURE_VIDEO_ENCODE_FADING
  if(!efx_fade_out)
    camera_video_engine_stop_requested  = TRUE;
#endif

  camera_ui_tcb = rex_self ();
  ( void ) rex_clr_sigs (camera_ui_tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32) CAMERA_FUNC_STOP_RECORD, (void *)0, (void *)0, 0, 0, 0, 0);

  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop_record", 0, 0, 0 );
  ( void ) rex_wait (CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop_record", 0, 0, 0 );
  ( void ) rex_clr_sigs (camera_ui_tcb, CAMERA_SIG);
  camera_ui_tcb = NULL;

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_stop_record */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_START_RECORD

DESCRIPTION
              Recording is started.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camcorder_process_start_record ( camera_cb_f_type callback, void *client_data)
{
  CAMQDSP_ImageType     application;


  camera_blt_ext_recalculate_window = TRUE;
  /* Terminate an existing function first */
  if ((camera_state != CAMERA_STATE_INIT) &&
      (camera_state != CAMERA_STATE_READY))
  {
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_DONE, 0);
  }
  #ifdef FEATURE_CAMCORDER_DCVS
    clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1);
  #elif (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
       defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)

    if(!camcorder_audio_only_record && 
   (camera_dx * camera_dy >= CAMCORDER_QVGA_SIZE))
    {
    #ifdef CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK
#error code not present
    #else
      clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER);
    #endif /* CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK */
    }
  #endif

  CAMERA_SLEEP_FORBID ();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_ON();

  camera_func                = CAMERA_FUNC_START_RECORD;
  camcorder_video_engine_recovering = FALSE;
  camera_video_engine_failed = FALSE;
  camera_video_engine_paused = CAMERA_PAUSING_STATE_NOT_PAUSED;
  camera_video_engine_pause_requested
                             = FALSE;
  camera_video_engine_resume_requested
                             = FALSE;
  camera_video_engine_stop_requested
                             = FALSE;
  camera_video_engine_start_requested
                             = TRUE;
  #ifdef FEATURE_VIDEO_ENCODE_THROTTLE
  camera_video_engine_throttle = FALSE;
  #endif /* FEATURE_VIDEO_ENCODE_THROTTLE */

  camera_app_cb              = callback;
  camera_app_data            = client_data;
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_RECORDING", 0, 0, 0);
  camera_state = CAMERA_STATE_RECORDING;
  camera_camsensor_op_mode   = CAMSENSOR_OP_MODE_VIDEO;
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  if(camera_video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
  {
    camera_set_preview_buffers ();
    camera_set_preview_headers ();
    camera_dsp_error_status_set_default();
  }

  camera_dsp_command = CAMQDSP_CMD_MAX;

  camera_log (LOG_VFEC, 0, 0, (uint32)VFE_INIT, __LINE__);

  application = CAMQDSP_MPEG4_ENC;

#ifdef FEATURE_CAMCORDER_AAC_REC
    if (camera_audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
      application = CAMQDSP_MPEG4_AAC_ENC;
    }
#endif /* FEATURE_CAMCORDER_AAC_REC */

#ifdef FEATURE_H264_ENCODER
  if (camera_video_format == VIDEO_FMT_STREAM_VIDEO_H264)
  {
#ifdef FEATURE_CAMCORDER_AAC_REC
    if (camera_audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
    {
      application = CAMQDSP_H264_AAC_ENC;
    }
    else
#endif /* FEATURE_CAMCORDER_AAC_REC */
    {
      application = CAMQDSP_H264_ENC;
    }
  }
#endif

  camera_dsp_state = DSP_ENABLING;
  if (VideoEncDSP_Initialize (camera_qdsp_cb, application) != CAMQDSP_SUCCESS)
  {
    camera_dsp_state = DSP_DISABLED;
    camera_log (LOG_INFO, 0, 0, (uint32)INIT_REJ, __LINE__);
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
    MSG_ERROR ("Cannot load DSP. State transition to ready", 0, 0, 0);
    event_report (EVENT_CAMERA_CANNOT_LOAD_DSP);
  }
  else
  {
    MSG_HIGH ("State transition to record", 0, 0, 0);
    event_report (EVENT_CAMERA_RECORD);
  }
} /* camera_process_start_record */

#ifdef FEATURE_MP4_TRANSCODER
/*===========================================================================

FUNCTION      CAMERA_ENABLE_OFFLINE

DESCRIPTION
              Initialize backend for offline encode mode


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camcorder_svcs_enable_offline( void )
{
  if(VideoEncDSP_OfflineInitialize(camera_qdsp_cb) != CAMQDSP_SUCCESS)
  {
    MSG_ERROR("Unable to initialize DSP image for offline encode mode",0,0,0);
    return CAMERA_FAILED;
  }
  return CAMERA_SUCCESS;
}
#endif

/*===========================================================================

FUNCTION      CAMERA_PROCESS_PAUSE_RECORD

DESCRIPTION
              Pause the Recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camcorder_process_pause_record ( void )
{
  MSG_HIGH ("camera process pause record", 0, 0, 0);

  if(camcorder_audio_only_record 
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
     )
  {
    camera_video_engine_pause_requested = FALSE;
    camera_video_engine_paused = CAMERA_PAUSED;
    (void)video_eng_pause(camera_video_engine_cb,
                          (void*) CAMERA_VIDEO_ENGINE_PAUSE_CB);
  }
  else
  {

     camera_video_engine_pause_requested = TRUE;
     #ifdef FEATURE_CAMCORDER_DCVS
        clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1);
     #elif (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
          defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)

     if(camera_dx * camera_dy >= CAMCORDER_QVGA_SIZE)
     {
     #ifdef CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK
#error code not present
     #else
       clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER);
     #endif /*CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK*/
     }
     #endif

  }

} /* camera_process_pause_record */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_RESUME_RECORD

DESCRIPTION
              Resume the Recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camcorder_process_resume_record ( void )
{
  uint16 fps;

  MSG_HIGH ("camera process resume record", 0, 0, 0);

  camera_video_engine_paused = CAMERA_PAUSING_STATE_NOT_PAUSED;

  if(!camcorder_audio_only_record
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
     )
  {
   #ifdef FEATURE_CAMCORDER_DCVS
      clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1);
   #elif (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
         defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)
   
      if(camera_dx * camera_dy >= CAMCORDER_QVGA_SIZE)
      {
      #ifdef CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK
#error code not present
      #else  
        clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER);
      #endif /* CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK */
    }
    #endif

    #ifdef FEATURE_VIDEO_ENCODE_THROTTLE
    camera_video_engine_throttle = FALSE;
    #endif /* FEATURE_VIDEO_ENCODE_THROTTLE */

    fps = (uint16) ((camcorder_fps.numerator<<8)/camcorder_fps.denominator);

    switch (fps )
    {
      case 3840:  /* 15 fps (256*15/1) = 3840*/
       vfeUpdate.output2SkipPattern.skipBitPattern  = 0x3FFFFFFF;
       break;

      case 2560: /* 10 fps (256*10/1) = 2560 */
                 /* skip 1 out of 3: 01 1011 0110 1101 1011 0110 1101 1011
                    =    1   B   6     D    B    6    D    B
                  */
       vfeUpdate.output2SkipPattern.skipBitPattern = 0x1B6DB6DB;
       break;

      case 1920:  /* 7.5 fps (256*15/2) = 1920 */
       vfeUpdate.output2SkipPattern.skipBitPattern = 0x2AAAAAAA;
       break;

      case 1280:  /* 5 fps (256*10/2) = 1280 */
       vfeUpdate.output2SkipPattern.skipBitPattern = 0x24924924;
       break;

      default:
       MSG_LOW("Unrecognized frame rate",0,0,0);
    }

    #ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    camera_start_transition_effects();
    #endif

    camera_update_vfe = TRUE;
  }

  (void)video_eng_resume(camera_video_engine_cb, (void*)CAMERA_VIDEO_ENGINE_RESUME_CB);
} /* camera_process_resume_record */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_STOP_RECORD

DESCRIPTION
              Stop the Recording.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void camcorder_process_stop_record ( void )
{
  MSG_HIGH ("camera_process_stop_record", 0,0,0);
#ifdef FEATURE_CAMCORDER_DCVS
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER_RECORD1);
#elif (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
   defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)
 
  if(!camcorder_audio_only_record && 
 (camera_dx * camera_dy >= CAMCORDER_QVGA_SIZE))
  {
#ifdef CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK
#error code not present
#else
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMCORDER);
#endif /* CAMCORDER_USE_CAMERA_CLIENT_FOR_HIGH_CLOCK*/
  }
 #endif

#ifdef FEATURE_VIDEO_ENCODE_FADING
  if( efx_fade_out && !camcorder_audio_only_record)
  {
    MSG_HIGH("begin fade out %d",mp4e_get_time_ms(),0,0);

    camera_initialize_fadeInOut();

    camera_fadein_out_api_params.params.fadeInOutCommand =
        VSE_FADE_OUT_COMMAND;
    camera_fadein_out_api_params.params.nmsecs = 2000; /* 2 sec fadeout*/
    ( void ) vse_fadeInOut(&camera_fadein_out_api_params.packet,
                         &camera_fadein_out_api_params.vse_struct,
                         VSE_INIT_ENG);

    efx_fade_out_running = TRUE;

    /* Check the video engine state*/ 
    if(camera_video_engine_paused == CAMERA_PAUSED)
    {
      /* Resume the recording to receive more frames
         to add fadout effect in recorded clip */
      camcorder_process_resume_record();
    }
  }
  else
#endif    // FEATURE_VIDEO_ENC_DE_FADING
  {
    camera_video_engine_stop_requested  = TRUE;
    camera_video_engine_pause_requested = FALSE;
    camera_video_engine_paused = CAMERA_PAUSING_STATE_NOT_PAUSED;

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

    /* See if we were paused and already flushed, if so, proceed to stop
       the video engine, otherwise, wait for next frame to drain the
       video buffer */
    if (camera_video_format == VIDEO_FMT_STREAM_VIDEO_NONE || 
        VideoBufferUseQueueSize (&encodeBufferHandle) == 0)
    {
      (void)video_eng_stop(camera_video_engine_cb, (void*) CAMERA_VIDEO_ENGINE_STOP_CB);
    }
    else
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: State transition to STOP_RECORD_AT_NEXT_HANDSHAKE", 0, 0, 0);
    }
  }

} /* camera_process_stop_record */

/*===========================================================================

FUNCTION      CAMCORDER_SVCS_GET_PARM

DESCRIPTION
             CAMCORDER_SVCS_GET_PARM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camcorder_svcs_get_parm
(
  camera_parm_type       id,
  camera_parm_info_type *parm
)
{
  camera_ret_code_type retVal = CAMERA_SUCCESS;
  if(NULL == parm)
    return CAMERA_FAILED;
  switch(id)
  {
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
   case CAMERA_PARM_FRAME_TIMESTAMP:
    parm->current_value = (int32) time_stamp_on;
    break;
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
  default:
    retVal = CAMERA_INVALID_PARM;
  }
  return retVal;
}

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
/*===========================================================================
FUNCTION    : cleanup_internal_useratom_info     
DESCRIPTION : Cleans up the the internally allocated memory for strings in
              camcorder_useratom_info
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
static void cleanup_internal_useratom_info ()
{
    /* Free title string*/
    if (camcorder_useratom_info->title.string.utf8)
    {
        CAM_Free((void *)camcorder_useratom_info->title.string.utf8);
        camcorder_useratom_info->title.string.utf8 = NULL;
    }

    /* Free Author string*/
    if (camcorder_useratom_info->author.string.utf8)
    {
        CAM_Free((void *)camcorder_useratom_info->author.string.utf8);
        camcorder_useratom_info->author.string.utf8 = NULL;
    }

    /* Free Description string*/
    if (camcorder_useratom_info->description.string.utf8)
    {
        CAM_Free((void *)camcorder_useratom_info->description.string.utf8);
        camcorder_useratom_info->description.string.utf8 = NULL;
    }

    /* Free Cprt string*/
    if (camcorder_useratom_info->copyright.string.utf8)
    {
        CAM_Free((void *)camcorder_useratom_info->copyright.string.utf8);
        camcorder_useratom_info->copyright.string.utf8 = NULL;
    }

    /* Free Location Info strings*/
    if (camcorder_useratom_info->loci.name.string.utf8)
    {
        CAM_Free((void *)camcorder_useratom_info->loci.name.string.utf8);
        camcorder_useratom_info->loci.name.string.utf8 = NULL;
    }

    if (camcorder_useratom_info->loci.astr_body.string.utf8)
    {
        CAM_Free((void *)camcorder_useratom_info->loci.astr_body.string.utf8);
        camcorder_useratom_info->loci.astr_body.string.utf8 = NULL;
    }

    if (camcorder_useratom_info->loci.add_notes.string.utf8)
    {
        CAM_Free((void *)camcorder_useratom_info->loci.add_notes.string.utf8);
        camcorder_useratom_info->loci.add_notes.string.utf8 = NULL;
    }
}
#endif /*FEATURE_VIDEO_SETATOMS_DYNAMIC*/

/*===========================================================================

FUNCTION      CAMERA_RECORDING_CLEANUP

DESCRIPTION
              Process video engine callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_recording_cleanup(camera_cb_type cb, int32 parm4)
{
  MSG_CAMERADEBUG ("CAMERA_SVCS: Recording Cleanup camera_state:%d camera_stopping:%d", camera_state, camera_stopping, 0);

  if (camera_state == CAMERA_STATE_RECORDING)
  {
    camera_stopping_record = FALSE;
#ifdef FEATURE_MDP
    /* De-register the video */
#ifdef FEATURE_MMOVERLAY
    camera_deregister_video(FALSE);
#else 
    camera_deregister_video(TRUE);
#endif /* FEATURE_MMOVERLAY */
    /* De-register the overlay layer */
    camera_deregister_layer1();
#endif /* FEATURE_MDP */
    camera_terminate(cb, parm4);
#ifdef FEATURE_VIDEO_ENCODE
    camera_video_engine_failed = FALSE;
#endif /* FEATURE_VIDEO_ENCODE */
  }

  MSG_CAMERADEBUG ("CAMERA_SVCS: Video Buffer Deallocate", 0, 0, 0);

  if(camera_video_format != VIDEO_FMT_STREAM_VIDEO_NONE)
  {
    ( void ) VideoBuffersDeallocate (&encodeBufferHandle);  
  }

  ackOutput2.eofAck2[0].Y_Address = NULL;
  output2enc_ack2_reset ();
  memset(&encodeBufferHandle,0,sizeof(encodeBufferHandle));
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC  
  if(camcorder_useratom_info)
  {
    cleanup_internal_useratom_info();
    CAM_Free(camcorder_useratom_info);
    camcorder_useratom_info = NULL;
  }
#endif
  if(camera_ui_tcb)
  {
    MSG_MED("Set signal to camera_svcs_stop_record", 0, 0, 0); 
    ( void ) rex_set_sigs (camera_ui_tcb, CAMERA_SIG);
  }

  camcorder_video_engine_recovering = FALSE;
}

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

/*===========================================================================

FUNCTION      CAMCORDER_PROCESS_VIDEO_ENGINE_CB

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
)
{
  video_status_type           status;
  camera_video_engine_cb_type callback;

  callback = (camera_video_engine_cb_type) ((uint32) cb);
  status   = (video_status_type) ((uint32) st);

  if (status == VIDEO_FAILURE)
  {
    switch (callback)
    {
      case CAMERA_VIDEO_ENGINE_INIT_CB:
        /* Video engine not initialized */
        camera_recording_cleanup(CAMERA_EXIT_CB_ERROR,(int32)CAMERA_ERROR_VIDEO_ENGINE);
        event_report (EVENT_CAMERA_VIDEO_FAILED);
        break;

      case CAMERA_VIDEO_ENGINE_START_CB:
        /* Video engine has initialized, but start failed */
        if (camera_state == CAMERA_STATE_RECORDING)
        {
          ( void ) video_eng_exit (camera_video_engine_cb,
                                 (void *) CAMERA_VIDEO_ENGINE_EXIT_CB);
        }
       break;

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
      case CAMERA_VIDEO_ENGINE_ENCODE_FRAME_CB:
        MSG_ERROR("Frame callback with VIDEO_FAILURE",0,0,0);

        /* Drain video buffers */
        camera_video_engine_failed = TRUE;
        camcorder_video_engine_error_recovery(status);
        break;

      case CAMREA_VIDEO_ENGINE_FLUSH_CB:
        MSG_CAMQDSPIO("flush cb",0,0,0);
        ASSERT(camera_encoding_frame == TRUE);
        camera_encoding_frame = FALSE;

#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
        camera_video_engine_throttle = FALSE;
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */

        /* Drain video buffers */
        camcorder_video_engine_error_recovery(status);
        break;

#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

      case CAMERA_VIDEO_ENGINE_STOP_CB:
        camera_video_engine_stop_requested = FALSE;
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
        if(camcorder_useratom_info)
        {
          cleanup_internal_useratom_info();
          CAM_Free(camcorder_useratom_info);
          camcorder_useratom_info = NULL;
        }
#endif  
        /* fall thru is intentional */
      case CAMERA_VIDEO_ENGINE_PAUSE_CB:
      case CAMERA_VIDEO_ENGINE_RESUME_CB:
        /* Error has occured in video engine Hence Log this Error and send it to client
        ** after closing video engine. */
        camera_video_engine_failed = TRUE;
        if (camera_app_cb)
        {
          (camera_app_cb) (CAMERA_EXIT_CB_FAILED, camera_app_data, CAMERA_FUNC_STOP_RECORD, 0);
        }
       if ((camera_state == CAMERA_STATE_RECORDING) ||
           (camera_state == CAMERA_STATE_INIT)  ||
           (camera_state == CAMERA_STATE_PREVIEW)
          )
       {
         ( void ) video_eng_exit (camera_video_engine_cb, (void *) CAMERA_VIDEO_ENGINE_EXIT_CB);
       }
       break;

      case CAMERA_VIDEO_ENGINE_EXIT_CB:
        camera_video_engine_failed = TRUE;
        MSG_CAMERADEBUG ("CAMERA_SVCS: Stop encode. State transition to stop encoding", 0, 0, 0);
        camera_recording_cleanup(CAMERA_EXIT_CB_FAILED,(int32)CAMERA_ERROR_VIDEO_ENGINE);
        event_report (EVENT_CAMERA_VIDEO_FAILED);
        break;
      default:
        break;
    }
  }
  else if(status == VIDEO_ERROR)
  {
    /* Just set flag to indicate error recovery in progress */
    if(camera_state == CAMERA_STATE_RECORDING)
    {
      camcorder_video_engine_error_recovery(status);
    }
  }
  else if ( status == VIDEO_LIMIT_NEAR                   ||
            status == VIDEO_LIMIT_NEAR_SAMPLE_TABLE      ||
            status == VIDEO_LIMIT_NEAR_CHUNK_TABLE       ||
            status == VIDEO_LIMIT_NEAR_STORAGE           ||
            status == VIDEO_LIMIT_NEAR_MOVIE_DURATION    ||
            status == VIDEO_LIMIT_IMMINENT               ||
            status == VIDEO_LIMIT_IMMINENT_SAMPLE_TABLE  ||
            status == VIDEO_LIMIT_IMMINENT_CHUNK_TABLE   ||
            status == VIDEO_LIMIT_IMMINENT_STORAGE       ||
            status == VIDEO_LIMIT_IMMINENT_MOVIE_DURATION )
  {
    /* Do a callback to application to indicate limited memory */
    if (camera_app_cb)
      (camera_app_cb)(CAMERA_STATUS_CB, camera_app_data,
                      CAMERA_FUNC_START_RECORD, (int32) status);

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

  }
  else if ( status == VIDEO_LIMIT_REACHED                ||
            status == VIDEO_LIMIT_REACHED_SAMPLE_TABLE   ||
            status == VIDEO_LIMIT_REACHED_CHUNK_TABLE    ||
            status == VIDEO_LIMIT_REACHED_MOVIE_DURATION ||
            status == VIDEO_LIMIT_REACHED_STORAGE )
  {
    /* To indicate space out event */
    if(camera_state == CAMERA_STATE_RECORDING)
    {
      /* Drain N-frames buffer before stopping video engine */
      camcorder_video_engine_error_recovery(status);
    }
  }
  else
  {
    switch (callback)
    {
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
       case CAMERA_VIDEO_ENGINE_ENCODE_FRAME_CB:

         /* Process this command only if video engine is not recovering from error */
         if(!camcorder_video_engine_recovering)
         {
#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
           if((camera_stopping_record &&          /* Stopping Record */
               camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED) ||
              (!camera_stopping &&          /* Pausing Record  */
               camera_video_engine_paused == CAMERA_PAUSING_FLUSHING_BUFFER)
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
             )
           {
             if(!camera_video_engine_throttle)
             {
               camera_video_engine_throttle = TRUE;
               camera_dsp_command = CAMQDSP_CMD_END_OF_FRAME_ACK2;
               output2enc_ack2_criteria(HANDSHAKE_RECEIVED2);
             }

             /* Signal indicating encoding of one frame complete */
             ASSERT(camera_encoding_frame == TRUE);
             camera_encoding_frame = FALSE;
           }
           else
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */
           {
             /* Video engine frame callback used to trigger */
             camera_svcs_process_func(CAMERA_FUNC_VIDEO_HANDSHAKE,
                                      NULL,NULL,0,0,0,0);
           }
         }
         break;

#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

      case CAMERA_VIDEO_ENGINE_INIT_CB:
         if (camera_app_cb)
         {
           (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_INIT_RECORD, 0);
         }
        /* Start the video engine, only if video engine is not stopping */
        if (camera_state == CAMERA_STATE_RECORDING && !camera_video_engine_stop_requested )
        {
#ifdef FEATURE_VIDEO_MULTI_UUID
          if (camera_uuid_info->nUuid > 0)
          {
            camera_uuid_count++;
            (void)video_eng_write_uuid (camera_uuid_info->pAtom[0],
                                  camera_uuid_info->pData[0],
                                  camera_uuid_info->dataSize[0],
                                  camera_video_engine_cb,
                                  (void*) CAMERA_VIDEO_ENGINE_UUID_CB);
          }
#else /* FEATURE_VIDEO_MULTI_UUID */
          if (camera_uuid && camera_uuid_data)
          {
            (void)video_eng_write_uuid (camera_uuid,
                                  camera_uuid_data,
                                  camera_uuid_data_size,
                                  camera_video_engine_cb,
                                  (void*) CAMERA_VIDEO_ENGINE_UUID_CB);
          }
#endif /* FEATURE_VIDEO_MULTI_UUID */
          else
          {
            (void)video_eng_start (camera_video_engine_cb,
                            (void *) CAMERA_VIDEO_ENGINE_START_CB);

#ifdef CAMCORDER_VFE_RECONFIG_TEST
            /* setoff the artificial VFE  reconfigure clock*/

            if(vfe_reconfigure_clk.group == NULL )
              clk_def (&vfe_reconfigure_clk);

            clk_reg (&vfe_reconfigure_clk, vfe_reconfigure_cb,
                    2000, 0, FALSE);
#endif  // #ifdef CAMCORDER_VFE_RECONFIG_TEST
          }
        }
        break;
      case CAMERA_VIDEO_ENGINE_UUID_CB:
#ifdef FEATURE_VIDEO_MULTI_UUID
        if (camera_uuid_info->nUuid > camera_uuid_count)
        {
          camera_uuid_count++;
          (void)video_eng_write_uuid (camera_uuid_info->pAtom[camera_uuid_count-1],
                                camera_uuid_info->pData[camera_uuid_count-1],
                                camera_uuid_info->dataSize[camera_uuid_count-1],
                                camera_video_engine_cb,
                                (void*) CAMERA_VIDEO_ENGINE_UUID_CB);
        }
        else
#endif /* FEATURE_VIDEO_MULTI_UUID */
        {
          (void)video_eng_start (camera_video_engine_cb,
                           (void *) CAMERA_VIDEO_ENGINE_START_CB);
        }
        break;
      case CAMERA_VIDEO_ENGINE_START_CB:
         if (camera_app_cb)
         {
           (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_START_RECORD, 0);
         }
        /* Configure the VFE */
        if (camera_state == CAMERA_STATE_RECORDING)
        {

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
          /* start of encoding will not have transition effects */
          VidEncTransitionStruct.bTransitionIsActive = FALSE;

#endif  /* ifdef FEATURE_VIDENC_TRANSITION_EFFECTS */
          if(camcorder_audio_only_record)
          {
           *encoder_acked = 2;
           camera_video_engine_start_requested = FALSE;
          }
          else{

            /* initialize the state machine */
            camera_recording_vfe_init();

            if (camera_config_vfe ())
            {
              *encoder_acked = 2;
              MSG_MED("encoder_acked - %d", *encoder_acked, 0, 0);
            }
            else
            {
              camera_log (LOG_INFO, 0, 0, (uint32)CFG_REJ, __LINE__);
              camcorder_video_engine_error_recovery (VIDEO_FAILURE);
              MSG_ERROR ("Cannot Configure Vfe", 0, 0, 0);
              event_report (EVENT_CAMERA_CANNOT_IDLE_DSP);
            }

          }
        }
        break;

      case CAMERA_VIDEO_ENGINE_PAUSE_CB:
        if (camera_app_cb)
          (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_PAUSE_RECORD, 0);
        break;

      case CAMERA_VIDEO_ENGINE_RESUME_CB:
        if (camera_app_cb)
          (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_RESUME_RECORD, 0);
        break;

      case CAMERA_VIDEO_ENGINE_STOP_CB:
        if ((camera_state == CAMERA_STATE_RECORDING) ||
            (camera_state == CAMERA_STATE_INIT) ||
            (camera_state == CAMERA_STATE_PREVIEW)
           )
        {
          ( void ) video_eng_exit (camera_video_engine_cb, (void *) CAMERA_VIDEO_ENGINE_EXIT_CB);
        }
        else if (camera_app_cb)
        {
          (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_STOP_RECORD, 0);
        }

        break;

      case CAMERA_VIDEO_ENGINE_EXIT_CB:
        MSG_CAMERADEBUG ("CAMERA_SVCS: Stop encode. State transition to stop encoding", 0, 0, 0);
        if (camera_video_engine_failed)
        {
          camera_recording_cleanup(CAMERA_EXIT_CB_FAILED,(int32)CAMERA_ERROR_VIDEO_ENGINE);
          event_report (EVENT_CAMERA_VIDEO_FAILED);
        }
        else
        {
            camera_recording_cleanup(CAMERA_EXIT_CB_DONE,0);
        }
        break;
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
      case CAMREA_VIDEO_ENGINE_FLUSH_CB:
        /* Process this command only if video engine is not recovering from error */
        if(!camcorder_video_engine_recovering)
        {

          MSG_CAMQDSPIO("fluch cb",0,0,0);
          ASSERT(camera_encoding_frame == TRUE);
          camera_encoding_frame = FALSE;

#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
          camera_video_engine_throttle = FALSE;
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */


#ifndef FEATURE_VIDEO_ENCODE_THROTTLE
          /* When FEATURE_VIDEO_ENCODE_THROTLE is diabled, VFE is moved to idle state 
           * during stop before control reaches this point. 
           * Since VFE is already in idle state, should not send Output2 EOF ACK to VFE. */
          if( !camera_stopping_record )
#endif /* ! FEATURE_VIDEO_ENCODE_THROTTLE */
          {
            #ifdef FEATURE_POSTCARD_RECORDING
#error code not present
            #endif /* FEATURE_POSTCARD_RECORDING */
            {
              camera_dsp_command = CAMQDSP_CMD_END_OF_FRAME_ACK2;
              output2enc_ack2_criteria(HANDSHAKE_RECEIVED2);
            }
          }

          if (camera_stopping || camera_stopping_record)
          {
            (void)video_eng_stop(camera_video_engine_cb, (void*) CAMERA_VIDEO_ENGINE_STOP_CB);
            MSG_CAMQDSPIO("called video_eng_stop",0,0,0);
          }
          else if (camera_video_engine_paused == CAMERA_PAUSING_FLUSHING_BUFFER)
          {
            *runningOutput2SkipPatternPtr = vfeUpdate.output2SkipPattern.skipBitPattern;
            vfeUpdate.output2SkipPattern.skipBitPattern  = 0x0;
            camera_update_vfe = TRUE;
            camera_video_engine_paused = CAMERA_PAUSED;
            (void)video_eng_pause(camera_video_engine_cb,
                            (void*) CAMERA_VIDEO_ENGINE_PAUSE_CB);
          }
        }
        break;
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
      default:
        break;
    }
  }
} /* camcorder_process_video_engine_cb */

/*===========================================================================
FUNCTION      CAMCORDER_PROCESS_AUDIO_READY_CB
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
void camcorder_process_audio_ready_cb ()
{
   glb_avs_info.audioConfig_done = TRUE;
   // Start Camera if video configuration is complete
   if(glb_avs_info.videoConfig_done == TRUE)
   {
      camera_initiate_capture();
   }
}

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
void camcorder_video_engine_error_recovery (video_status_type status)
{
  VideoBuffersYCbCrAddressPairsType YCbCrAddresses;

  if(!camcorder_video_engine_recovering)
  {
    camcorder_video_engine_recovering = TRUE;

    /* Drain buffers so video engine can proceed with clean up */
    while(VideoBufferUseQueueSize(&encodeBufferHandle) > 0)
    {
      ( void ) VideoBufferDequeueUse(&encodeBufferHandle,&YCbCrAddresses);
      ( void ) VideoBufferReturnBufferToPool(&encodeBufferHandle,YCbCrAddresses);
      MSG_HIGH("Camera draining video buffer: dQ-> Pool %x",
               YCbCrAddresses.Y_Address,0,0);
    }

    /* No need to ack output2 during error recovery */
    ackOutput2.eofAck2[0].Y_Address = NULL;
    output2enc_ack2_reset();

    /* Send client callback to indicate error event happened */
    if(camera_app_cb)
      (camera_app_cb)(CAMERA_STATUS_CB, camera_app_data,
                      CAMERA_FUNC_START_RECORD, (int32) status);


    /* UI will no longer issue stop record, stop video engine now */
    (void)video_eng_stop(camera_video_engine_cb, (void*) CAMERA_VIDEO_ENGINE_STOP_CB);

  }
  else
  {
    MSG_HIGH("Video Engine already recovering from an error", 0,0,0);
  }

} /* camcorder_video_engine_error_recovery */

void camera_process_qdsp_video_frame_done_msg (uint16 *msg)
{
  VideoBuffersYCbCrAddressPairsType YCbCrAddress;

  ( void ) VideoBufferPeakUse (&encodeBufferHandle, &YCbCrAddress);

  mp4_encode_frame_parm.buf_ptr = msg;
  mp4_encode_frame_parm.time  = YCbCrAddress.time ;
  event_report(EVENT_DSP_VIDEO_ENC_FRAME_DONE);

  video_encode_frame ((uint8 *) &mp4_encode_frame_parm);
}

void camera_process_qdsp_video_encode_selection_done_msg ( void )
{
   MSG_LOW ("Video Encode Selection Done.", 0, 0, 0);
   event_report(EVENT_DSP_VIDEO_ENC_SELECTION_DONE);

   glb_avs_info.videoConfig_done = TRUE;
   if(glb_avs_info.audioConfig_done == TRUE)
   {
      camera_initiate_capture ();
   }
}

void camera_process_qdsp_video_encode_config_done_msg ( void )
{
  event_report(EVENT_DSP_VIDEO_ENC_CONFIG_DONE);
}

void camera_process_qdsp_video_encode_idle_done_msg ( void )
{
  /* As of now this code is not used. Whoever wants to use this should
  ** consider setting the signal by the following code.
  ** rex_sigs_type sigs;
  ** sigs = rex_set_sigs (camera_ui_tcb, CAMERA_SIG);
  */
  (void)video_eng_stop(camera_video_engine_cb, (void*)CAMERA_VIDEO_ENGINE_STOP_CB);
}

void camera_process_qdsp_video_encode_feature_update_done_msg ( void )
{
  MSG_LOW ("Encode Feature Update Complete.", 0, 0, 0);
}

/*===========================================================================

FUNCTION      CAMERA_VIDEO_ENGINE_CB

DESCRIPTION
              Accepts video engine callback.

              Callback is queued and processed at camera_process_func().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_video_engine_cb
(
  video_status_type  status,        /* Status of callback                 */
  void               *client_data   /* Client data from calling function  */
)
{
  graph_queue_camera_func((uint32)CAMERA_FUNC_VIDEO_ENGINE_CB, client_data,
      (void *)status, 0, 0, 0, 0);
} /* camera_video_engine_cb */

#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
/*===========================================================================

FUNCTION      camera_audio_ready_cb

DESCRIPTION
              Accepts callback about first audio data being received.

              Callback is queued and processed at camera_process_func().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_audio_ready_cb ()
{
  graph_queue_camera_func((uint32)CAMERA_FUNC_AUDIO_READY_CB, (void *)0,
      (void *)0, 0, 0, 0, 0);

} /* camera_audio_ready_cb */
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/

/* initialize the state machine at start of recording*/
static void camera_recording_vfe_init( void )
{
  MSG_CAMQDSPIO("VFE RECONFIGURABLE state",0,0,0);
  camera_recording_vfe_reset_state = CAMERA_RECORDING_VFE_RECONFIGURABLE;
}

/* call from ISR when Output2 is received
   Should not be called if VFE is resetting (i.e. check flag before callign)
*/
static void camera_recording_vfe_process_output2( void )
{
  if(camera_recording_vfe_reset_state != CAMERA_RECORDING_VFE_RECONFIGURABLE)
  {
    MSG_CAMQDSPIO("VFE_RECONFIG_STATE_ERROR",0,0,0);
    camera_recording_vfe_reset_state = CAMERA_RECORDING_VFE_ERROR;
    return;
  }

  MSG_CAMQDSPIO("VFE_DO_NOT_RECONFIG_STATE",0,0,0);
  camera_recording_vfe_reset_state = CAMERA_RECORDING_VFE_DO_NOT_RECONFIG;
}

/* call camera_recording_vfe_reconfig_permitted() from camera_config_vfe()

   -- If we block the vfe_reconfig we need to change the state to
      RECONFIG_REQUESTED so it will know to recofigure VFE after handshake
      is received.

   -- If we reconfigure vfe not from init then we need to block BE-restart
      after the VFE reconfigures.
*/
boolean camera_recording_vfe_reconfig_permitted
(
  boolean vfe_reset_request_not_during_init
)
{
  boolean retval;

  retval = !(camera_recording_vfe_reset_state ==
            CAMERA_RECORDING_VFE_DO_NOT_RECONFIG );

  if ( retval )
  {
    if(vfe_reset_request_not_during_init)
    {
      /* VFE reconfigure only ... do not block BE initialization when
      ** VFE complete re-initialization
      */
      camera_record_vfe_reset_block_be_restart = TRUE;
    }
    else
    {
      /* this is initialization ... therefore do not block
      ** BE intialization
      */
      camera_record_vfe_reset_block_be_restart = FALSE;
    }
  }
  else
  {
    MSG_CAMQDSPIO("VFE_RECONFIG_REQUESTED state",0,0,0);
    camera_recording_vfe_reset_state = CAMERA_RECORDING_VFE_RECONFIG_REQUESTED;
  }

  return retval;
}

/* call this from handshake, before ACK2, to handle a reset */
static boolean camera_recording_vfe_ack2_permitted( void )
{
  if (camera_recording_vfe_reset_state ==
           CAMERA_RECORDING_VFE_RECONFIG_REQUESTED
     )
  {
    MSG_CAMQDSPIO("VFE_RECONFIGURABLE state",0,0,0);
    camera_recording_vfe_reset_state = CAMERA_RECORDING_VFE_RECONFIGURABLE;
    return FALSE;  /* call camera_config_vfe upon return */
  }

  ASSERT(camera_recording_vfe_reset_state ==
         CAMERA_RECORDING_VFE_DO_NOT_RECONFIG);
  camera_recording_vfe_reset_state = CAMERA_RECORDING_VFE_RECONFIGURABLE;
  return TRUE;     /* client calls ack2 upon return */
}

/*====================================================================
** the 2 done events are Special Effects Done and Handshake received
** The doneevent_flags keeps track of these two
====================================================================*/
static output2_doneevent_flags_type output2_doneevent_flags = {0};

static void output2enc_ack2_reset( void )
{
  memset(&output2_doneevent_flags,0,sizeof(output2_doneevent_flags));
}

/* Collects all the events required before an ACK2 can be sent
** to the VFE
*/
void output2enc_ack2_criteria(output2_doneevent_type event)
{
  VFE_EOF_Ack2Type  ack2;
  VideoBuffersYCbCrAddressPairsType addressPair;

  TASKLOCK();

  switch(event)
  {
  case SPECIAL_EFFECTS_DONE2:
    MSG_CAMQDSPIO("got SE callback %d",mp4e_get_time_ms(),0,0);
    ASSERT(output2_doneevent_flags.encode_output2_special_effects_done_flag
           == FALSE);
    output2_doneevent_flags.encode_output2_special_effects_done_flag = TRUE;
    break;

  case HANDSHAKE_RECEIVED2:
    MSG_CAMQDSPIO("got handshake %d",mp4e_get_time_ms(),0,0);
    output2_doneevent_flags.encode_output2_handshake_flag = TRUE;
    break;

  default:
    { TASKFREE();
    ASSERT(0);
    MSG_ERROR("received invalid output2 handshake event",0,0,0);
    return;
  }

  if ( output2_doneevent_flags.encode_output2_special_effects_done_flag &&
       output2_doneevent_flags.encode_output2_handshake_flag
     ) {

    ASSERT(ackOutput2.eofAck2[0].Y_Address != NULL);
    ack2 = ackOutput2;
    ackOutput2.eofAck2[0].Y_Address = NULL;

    output2enc_ack2_reset();

    if(camera_video_engine_resume_requested &&
       camera_video_engine_paused == CAMERA_PAUSED)
    {
      camera_video_engine_resume_requested = FALSE;
      camera_video_engine_paused = CAMERA_PAUSING_STATE_NOT_PAUSED;
      while(0 != VideoBufferUseQueueSize(&encodeBufferHandle))
      {
        ( void ) VideoBufferDequeueUse(&encodeBufferHandle,&addressPair);
        MSG_CAMQDSPIO("resume flushing %x T=%d",addressPair.Y_Address,
                      addressPair.time,0);
        ( void ) VideoBufferReturnBufferToPool(&encodeBufferHandle,
                                             addressPair);
      }
    }

    MSG_CAMQDSPIO("ack2 %x",ack2.eofAck2[0].Y_Address,0,0);
    if(ack2.eofAck2[0].Y_Address)
    {
      if (camera_recording_vfe_ack2_permitted())
        ( void ) VFE_EOF_Ack2(ack2);
      else
        ( void ) camera_config_vfe();
    }
  }
  TASKFREE();
}

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
#ifdef FEATURE_VIDEO_ENCODE_FADING
static void camera_initialize_fadeInOut( void )
{
  if ( camera_fadein_out_api_params.vse_struct.engine_status
       != VSE_ENGINE_UNINITIALIZED)
  {
    ( void ) vse_fadeInOut(&camera_fadein_out_api_params.packet,
                         &camera_fadein_out_api_params.vse_struct,
                         VSE_TERMINATE_PACKET);

    ( void ) vse_fadeInOut(&camera_fadein_out_api_params.packet,
                         &camera_fadein_out_api_params.vse_struct,
                         VSE_TERMINATE);
  }

  camera_special_effects_api_params.input_buffer.cFormat
     = IPL_YCrCb420_LINE_PK;
  camera_special_effects_api_params.input_buffer.dx = camera_dx;
  camera_special_effects_api_params.input_buffer.dy = camera_dy;

  camera_fadein_out_api_params.packet.input_buffer =
        &camera_special_effects_api_params.input_buffer;
  camera_fadein_out_api_params.packet.msec = mp4e_get_time_ms();
  camera_fadein_out_api_params.packet.param =
       (void *) &camera_fadein_out_api_params.params;
  camera_fadein_out_api_params.params.fadeInOutCommand =
        VSE_FADE_IN_COMMAND;
  camera_fadein_out_api_params.params.nmsecs = 2000; /*2 second fadein*/
}
#endif  // FEATURE_CAMERA_VIDEO_ENCODE_FADING

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
static void camera_initialize_timestamp( void )
{
  if ( camera_timestamp_params.vse_struct.engine_status
       != VSE_ENGINE_UNINITIALIZED)
  {
    ( void ) vse_timestamp(&camera_timestamp_params.packet,
                         &camera_timestamp_params.vse_struct,
                         VSE_TERMINATE_PACKET);

    ( void ) vse_timestamp(&camera_timestamp_params.packet,
                         &camera_timestamp_params.vse_struct,
                         VSE_TERMINATE);
  }

  camera_special_effects_api_params.input_buffer.cFormat =
    IPL_YCrCb420_LINE_PK;
  camera_special_effects_api_params.input_buffer.dx = camera_dx;
  camera_special_effects_api_params.input_buffer.dy = camera_dy;

  camera_timestamp_params.packet.input_buffer =
    &camera_special_effects_api_params.input_buffer;

}
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
#ifdef FEATURE_MDP
extern void camera_svcs_display_frame_overlay(boolean clear_overlay_layer);
#endif /* FEATURE_MDP */
camera_ret_code_type camcorder_create_preview_overlay(ipl_image_type *overlay, 
                                                      uint16 camera_default_preview_rotation,
                                                      uint16 camera_transparent_color)
{
  uint32          size;
  ipl_image_type  img;
  uint16          overlay_rotation = 0;
  ipl_image_type * preview_overlay = preview_overlay_buffer;

#ifdef FEATURE_MDP
  if(first_call)
  	{
       /*To register frame of full size at MDP_LAYER_1.
	         But this function should be called only once and not everytime
	         when there is change in overlay frame. */
       camera_svcs_display_frame_overlay(TRUE);
	   first_call = FALSE;
  	}
#endif

  if (overlay == NULL)
{
    return CAMERA_FAILED;
  }
  MSG_HIGH("camera_create_preview_overlay: enters",0,0,0);
  size = camera_preview_dx * camera_preview_dy * 2;

  /* First, create preview overlay in the same mode as input overlay */
  if (overlay->dx < overlay->dy)
  {
    /* Portrait mode */
    preview_overlay->dx=camera_preview_dy;
    preview_overlay->dy=camera_preview_dx;
  }
  else
  {
    /* Landscape mode */
    preview_overlay->dx      = camera_preview_dx;
    preview_overlay->dy      = camera_preview_dy;
  }
  preview_overlay->cFormat = overlay->cFormat;
  if((overlay->dx == preview_overlay->dx) &&
  	 (overlay->dy == preview_overlay->dy))
  {
    memcpy(preview_overlay->imgPtr, overlay->imgPtr, size);
  }
  else
  {
    CAMCORDER_MSG_HIGH("camcorder_create_preview_overlay: Overlay Size and Preview Size differ",0,0,0);
    return CAMERA_FAILED;
  }

  /* Rotate overlay frame to match the preview mode. */
  if (camera_default_preview_rotation == 0 || camera_default_preview_rotation == 180)
  {
    if (preview_overlay->dx < preview_overlay->dy)
    {
      /* Preview in landscape mode, overlay image is in portrait.
       * So need to rotate 270 */
      overlay_rotation = 270;
    }
  }
  else
  {
    if (preview_overlay->dx > preview_overlay->dy)
    {
      /* Preview in portrait mode, overlay image is in landscape.
       * So need to rotate 90 */
      overlay_rotation = 90;
    }
  }

  if (overlay_rotation > 0 )
  {
    ipl_rotate90_type ipl_rotation = IPL_NOROT;

    switch (overlay_rotation)
    {
      case 90:
        ipl_rotation = IPL_ROT90;
        break;
      case 270:
      default:
        ipl_rotation = IPL_ROT270;
        break;
    }
    img.dx      = preview_overlay->dy;
    img.dy      = preview_overlay->dx;
    img.imgPtr = temp_overlay.imgPtr;
    img.cFormat = preview_overlay->cFormat;
    if (ipl_rotate90_frame (preview_overlay, &img, ipl_rotation, camera_transparent_color) != IPL_SUCCESS)
    {
      MSG_HIGH("camera_create_preview_overlay: IPL rotation failed",0,0,0);
      return CAMERA_FAILED;
    }
    temp_overlay.imgPtr = preview_overlay->imgPtr;
    preview_overlay->imgPtr  = img.imgPtr;
    preview_overlay->dx      = img.dx;
    preview_overlay->dy      = img.dy;
  }
#ifdef FEATURE_MDP
  camera_svcs_display_frame_overlay(FALSE);
#endif /* FEATURE_MDP */
  return CAMERA_SUCCESS;
}

boolean camcorder_svcs_clr_overlay( ipl_image_type ** preview_overlay_addr)
{
  if (preview_overlay_buffer)
  {
    *preview_overlay_addr = preview_overlay_buffer;
    preview_overlay_buffer = NULL;
  }
  if (temp_overlay.imgPtr)
  {
      MSG_HIGH("temp_overlay deallocated.", 0, 0, 0);
      CAM_Free(temp_overlay.imgPtr);
      temp_overlay.imgPtr = NULL;
  }
  if(camera_video_eng_overlay.frame)
  {
    CAM_Free(camera_video_eng_overlay.frame->imgPtr);
    CAM_Free(camera_video_eng_overlay.frame);
    camera_video_eng_overlay.frame = NULL;
    MSG_HIGH("deallocates video_eng_overlay.frame & image", 0, 0, 0);
  }
  if (camera_video_eng_overlay.valid_check)
  {
    camera_video_eng_overlay.valid_check = 0;
    /*lint -save -e522 VSE funny thing */
    vse_overlay(
        &camera_special_effects_api_params.packet,
        &camera_special_effects_api_params.vse_struct,
        VSE_TERMINATE_PACKET
      );
    vse_overlay(
        &camera_special_effects_api_params.packet,
        &camera_special_effects_api_params.vse_struct,
        VSE_TERMINATE
      );
    camera_video_eng_overlay.new_frame = FALSE;
    /*lint -restore */
    camera_special_effects_ptr = NULL;
    MSG_HIGH("camcorder_svcs_clr_overlay: shut off encode overlay", 0, 0, 0);

    memset(&camera_special_effects_api_params,0,
           sizeof(camera_special_effects_api_params));
    camera_special_effects_api_params.input_buffer.cFormat =
      IPL_YCrCb420_LINE_PK;
  }
  overlay_allocated = FALSE;

#ifdef FEATURE_VIDEO_ENCODE_FADING
  if (efx_fade_in || efx_fade_out )
  {
    ( void ) vse_fadeInOut(&camera_fadein_out_api_params.packet,
                         &camera_fadein_out_api_params.vse_struct,
                         VSE_TERMINATE_PACKET
                        );
    ( void ) vse_fadeInOut(&camera_fadein_out_api_params.packet,
                         &camera_fadein_out_api_params.vse_struct,
                         VSE_TERMINATE
                        );
  }
#endif //#ifdef FEATURE_VIDEO_ENCODE_FADING

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
  if(time_stamp_on)
  {
    ( void ) vse_timestamp(&camera_timestamp_params.packet,
                         &camera_timestamp_params.vse_struct,
                         VSE_TERMINATE_PACKET
                        );
    ( void ) vse_timestamp(&camera_timestamp_params.packet,
                         &camera_timestamp_params.vse_struct,
                         VSE_TERMINATE
                        );
  }
#endif  /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
  return TRUE;
}
#endif  // #ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API


void camcorder_svcs_set_parm_fps(uint16 numerator, uint16 denominator)
{
   camcorder_fps.numerator   = numerator;
   camcorder_fps.denominator = denominator;
}

camera_ret_code_type camcorder_svcs_set_parm_audio_fmt(int32 parm1, int32 parm2)
{
#ifdef FEATURE_CAMCORDER_AAC_REC
  if (parm1 == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
  {
    camera_audio_channel     = (video_eng_audio_channel_type)
                               (((uint32)parm2 >> 24) & 0xFF);
    camera_audio_sample_rate = (video_eng_audio_sample_rate_type)
                               (((uint32)parm2 >> 16) & 0xFF);
    camera_audio_profile     = (video_eng_audio_profile_type)
                               (((uint32)parm2 >> 8)  & 0xFF);
    return CAMERA_SUCCESS;
  }
#endif /* FEATURE_CAMCORDER_AAC_REC */
  return CAMERA_NOT_SUPPORTED;
}

#ifdef FEATURE_VIDEO_ENCODE_FADING
void camcorder_svcs_set_parm_fading(int32 parm)
{
  switch(parm)
  {
    case CAMERA_PARM_FADE_IN:
      /* fade in */
      efx_fade_in  = TRUE;
      efx_fade_out = FALSE;
      break;

    case CAMERA_PARM_FADE_OUT:
      /* fade out */
      efx_fade_in  = FALSE;
      efx_fade_out = TRUE;
      break;
    case CAMERA_PARM_FADE_IN_OUT:
      /* fade in fade out */
      efx_fade_in    = TRUE;
      efx_fade_out   = TRUE;
      break;
   default:
      efx_fade_in  = FALSE;
      efx_fade_out = FALSE;
      break;
  }
}
#endif /* FEATURE_VIDEO_ENCODE_FADING */

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
void camcorder_svcs_set_parm_transit(int32 parm)
{
  switch(parm)
  {
  case CAMERA_PARM_TRANSITION_ALPHA:
    efx_special_fade  = VIDENCTRANSITION_ALPHA_FADE;
    break;
  case CAMERA_PARM_TRANSITION_CURTAIN:
    efx_special_fade  = VIDENCTRANSITION_CURTAIN_FADE;
    break;
  case CAMERA_PARM_TRANSITION_OFF:
  default:
    efx_special_fade  = VIDENCTRANSITION_NONE;
  }
}
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
camera_ret_code_type camcorder_svcs_set_parm_timestamp(int32 parm)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
  if(parm < 0 || parm > 1)
  {
    ret_val = CAMERA_INVALID_PARM;
  }
  else
  {
    time_stamp_on = (boolean) parm;
  }
  return ret_val;
}
#endif /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP */

void camcorder_svcs_set_parm_space_limit(int32 parm)
{
  camera_space_limit    = parm;
}

void camcorder_svcs_process_video_hanshake( void )
{
  VideoBuffersYCbCrAddressPairsType YCbCrAddresses;
  VideoBufferHandle                *videoBufferHandle;
  ASSERT(camera_encoding_frame == TRUE);
  camera_encoding_frame = FALSE;

  videoBufferHandle = &encodeBufferHandle;

  ( void ) VideoBufferDequeueUse (videoBufferHandle, &YCbCrAddresses);

  if (!camera_stopping &&
      !camera_stopping_record &&
      camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED )
  {

    ackOutput2.eofAck2[0].Y_Address    = YCbCrAddresses.Y_Address;
    ackOutput2.eofAck2[0].CbCr_Address = YCbCrAddresses.CbCr_Address;

    if (camera_video_engine_stop_requested )
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Request camera_stopping=STOP_RECORD", 0, 0, 0);
      camera_stopping_record = TRUE;

      camera_video_engine_paused          = CAMERA_PAUSING_STATE_NOT_PAUSED;
      camera_video_engine_stop_requested  = FALSE;

#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
      MSG_HIGH("camera_svcs_process_func: STOP requested - \
               camera_state %d camera_stopping %d camera_video_engine_paused %d",
               camera_state, camera_stopping, camera_video_engine_paused);
#else /* !FEATURE_VIDEO_ENCODE_THROTTLE */
          /* VFE is moved to idle state inorder to reduce the AHB access when recording is stopped */
          (void) camera_send_vfe_idle_and_wait();
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */
    }
    else if (camera_video_engine_pause_requested)
    {
      camera_video_engine_paused          = CAMERA_PAUSING_FLUSHING_BUFFER;

#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
      MSG_HIGH("camera_svcs_process_func: PAUSE requested - \
               camera_state %d camera_stopping %d camera_video_engine_paused %d",
               camera_state, camera_stopping, camera_video_engine_paused);
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */
    }
    camera_video_engine_pause_requested = FALSE;

    if (!camera_stopping &&
        !camera_stopping_record &&
        camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED 
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
        )
    {
      camera_dsp_command = CAMQDSP_CMD_END_OF_FRAME_ACK2;
      output2enc_ack2_criteria(HANDSHAKE_RECEIVED2);
    }
  }

  if (((camera_stopping_record &&           /* Stopping Record */
        camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED) ||
       (!camera_stopping &&           /* Pausing Record  */
        camera_video_engine_paused == CAMERA_PAUSING_FLUSHING_BUFFER))
#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
      && !camera_video_engine_throttle
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */
#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */
     )
  {
    boolean notDropped = TRUE;
    int nBufs;

/* we should return this to pool ONLY if this is not the
** buffer that we're going to ack2 on
*/
    if ( ackOutput2.eofAck2[0].Y_Address != YCbCrAddresses.Y_Address )
    {
      MSG_CAMQDSPIO("return %x to pool",YCbCrAddresses.Y_Address,0,0);
      ( void ) VideoBufferReturnBufferToPool (videoBufferHandle, YCbCrAddresses);
    }

/* If camera_video_skip_buffer_flag == TRUE, skip processing of all
** N frames in buffer, or enable FEATURE_VIDEO_ENCODE_THROTTLE */
    while ((nBufs=VideoBufferUseQueueSize(videoBufferHandle)) > 0)
    {
      ( void ) VideoBufferPeakUse (videoBufferHandle, &YCbCrAddresses);
      notDropped = video_eng_check_frame_encode_permitted(videoBufferHandle,YCbCrAddresses.time);

       if(camera_video_skip_buffer_flag ||
         (notDropped==FALSE))
       {
          ( void ) VideoBufferDequeueUse(videoBufferHandle,&YCbCrAddresses);
          ( void ) VideoBufferReturnBufferToPool(videoBufferHandle,YCbCrAddresses);
           MSG_CAMQDSPIO("dropped, dQ-> Pool %x nbufs %d",
                    YCbCrAddresses.Y_Address,nBufs,0);
       }
       else
           break;
    }

    if (nBufs == 0)
    {
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
      if ( camera_video_engine_paused != CAMERA_PAUSING_STATE_NOT_PAUSED)
      {
        camera_video_transition_effects_store(&YCbCrAddresses);
      }
      else
      {
/* in the case of a stop, we free all malloc'ed buffers */
        camera_video_transition_effects_store(NULL);
      }
#endif
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
/* there is one more buffer in the encoder queue
** cannot drop this one or we'll loose clip time at
** the end
*/
      camera_encoding_frame = TRUE;
      MSG_CAMQDSPIO("encoding NULL (flushing)",0,0,0);
      video_eng_frame(NULL,NULL,IPL_COL_MAX,0,camera_video_engine_cb,
                      (void *) CAMREA_VIDEO_ENGINE_FLUSH_CB);
    }
    else
    {
      ( void ) camcorder_svcs_initiate_backend_encode();
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
    }
    MSG_MED("notDropped - %d", notDropped, 0, 0);
  }
}
camera_ret_code_type camcorder_svcs_set_parm(camera_parm_type id, int32 parm)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
  switch(id)
  {
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
    case CAMERA_PARM_USER_ATOM:
       ret_val = camcorder_svcs_set_parm_udat_atom(parm);
       break;
#endif
    default:
      MSG_ERROR("Invalid control", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      ret_val = CAMERA_INVALID_PARM;
      break;
  }
  return ret_val;
}

#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
static void camcorder_svcs_wcscpy(short *dst, const short *src, const uint32 length)
{
    uint32 count;
    for (count = 0; count != length ; ++count)
    {
        dst [count] = src [count];
        if(!(dst [count]))
        {
          break;
        }
    }
    /* Append null character in the end*/
    dst[length-1] = 0;
}

static boolean camcorder_svcs_copy_language_coded_string(
  video_fmt_language_coded_string_type        *target,
  const video_fmt_language_coded_string_type  *source
)
{
    video_fmt_alloc_type alloc;
    target->language = source->language;
    target->isUTF16  = source->isUTF16;
    target->length   = source->length;
    /* If the String length is zero ignore the string */
    if (!source->length)
    {
        target->string.utf8 = NULL;
        return TRUE;
    }

    if (source->isUTF16)
    {
        /* if string not NULL terminated include null character*/
        if(source->string.utf16[source->length-1] != 0 )
        {
          target->length += 1;
        }
        /* Restrict the string size to be not more than 200bytes, if more than 200bytes truncate*/ 
        if(target->length > 100)
        {
          target->length = 100;
        }
        alloc.size = sizeof (short) * target->length;
        /*Check if we need to allocate memory*/
        if(target->string.utf16 == NULL)
        {
            alloc.ptr = CAM_Malloc(alloc.size);
            if (!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf16 = (const short *) alloc.ptr;
        }
        else
        {
            alloc.ptr = (void *)target->string.utf16;
            alloc.ptr = realloc(alloc.ptr, alloc.size);
            if(!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf16 = (const short *) alloc.ptr;
        }
        camcorder_svcs_wcscpy((short *)target->string.utf16, source->string.utf16, target->length);
    }
    else
    {
        /* if string not NULL terminated include null character */
        if(source->string.utf8[source->length-1] != 0 )
        {
          target->length += 1;
        }
        alloc.size = sizeof (char) * target->length;
        if(target->length > 200)
        {
          target->length = 200;
        }
        /*Check if memory allocation is neccessary*/
        if(target->string.utf8 == NULL)
        {
            alloc.ptr = CAM_Malloc(alloc.size);
            if (!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf8 = (const char *) alloc.ptr;
        }
        else
        {
            alloc.ptr = (void *)target->string.utf8;
            alloc.ptr = realloc(alloc.ptr, alloc.size);
            if (!alloc.ptr)
            {
                return FALSE;
            }
            target->string.utf8 = (const char *) alloc.ptr;
        }
        std_strlcpy((char *) target->string.utf8, source->string.utf8, alloc.size);
    }
    return TRUE;
}

static boolean camcorder_svcs_copy_location
(
  video_fmt_location_type       *target,
  const video_fmt_location_type *source
)
{
    target->role      = source->role;
    target->altitude  = source->altitude;
    target->longitude = source->longitude;
    target->latitude  = source->latitude;
    if ( !camcorder_svcs_copy_language_coded_string (&target->name, &source->name ) ||
         !camcorder_svcs_copy_language_coded_string (&target->astr_body, &source->astr_body ) ||
         !camcorder_svcs_copy_language_coded_string (&target->add_notes, &source->add_notes )
       )
    {
        MSG_ERROR ("camcorder_svcs_copy_location: alloc failure!", 0, 0, 0);
        return FALSE;
    }
    return TRUE;
}

static camera_ret_code_type camcorder_svcs_set_parm_udat_atom(int32 parm)
{
  video_eng_user_data_atom_type *useratom;
  if(!parm)
  {
    return CAMERA_INVALID_PARM;
  }
  useratom = (video_eng_user_data_atom_type *)parm;

  if(!useratom->atom_is_valid)
  {
      return  CAMERA_INVALID_PARM;
  }

  if(!camcorder_useratom_info)
  {
    camcorder_useratom_info = (video_eng_user_data_atom_type *)CAM_Malloc(sizeof(video_eng_user_data_atom_type));

    if(!camcorder_useratom_info)
    {
      return CAMERA_NO_MEMORY;
    }
    memset(camcorder_useratom_info,0,sizeof(video_eng_user_data_atom_type));
  }

  if(useratom->titl_auth_dscp_cprt_valid)
  {

  if(useratom->title.string.utf8       == NULL &&
     useratom->author.string.utf8      == NULL &&
     useratom->description.string.utf8 == NULL &&
     useratom->copyright.string.utf8   == NULL
    )
  {
    return CAMERA_INVALID_PARM;
  }

  if(!camcorder_svcs_copy_language_coded_string(&camcorder_useratom_info->author, &useratom->author)||
     !camcorder_svcs_copy_language_coded_string(&camcorder_useratom_info->title, &useratom->title)||
     !camcorder_svcs_copy_language_coded_string(&camcorder_useratom_info->description, &useratom->description)||
     !camcorder_svcs_copy_language_coded_string(&camcorder_useratom_info->copyright, &useratom->copyright)
  )
  {
    return CAMERA_NO_MEMORY;
  }
      camcorder_useratom_info->titl_auth_dscp_cprt_valid = TRUE;
      camcorder_useratom_info->atom_is_valid = TRUE;
  }

  /* Check for loci */
  if (useratom->loci.loci_valid)
  {
      if(useratom->loci.name.string.utf8      == NULL	&&
         useratom->loci.astr_body.string.utf8 == NULL	&&
         useratom->loci.add_notes.string.utf8 == NULL
        )
      {
          return  CAMERA_INVALID_PARM;
      }

      if(!camcorder_svcs_copy_location(&camcorder_useratom_info->loci, &useratom->loci))
      {
          return CAMERA_NO_MEMORY;
      }
      camcorder_useratom_info->loci.loci_valid = TRUE;
      camcorder_useratom_info->atom_is_valid = TRUE;
  }

  return CAMERA_SUCCESS;
}
/* 
** camcorder_copy_user_atoms_client_cb function will be called by 
** videoengine when recording ends.Client can pass the titl,auth,descp 
** and cprt atoms.
*/
static void camcorder_copy_user_atoms_client_cb(
                video_eng_user_data_atom_type *puseratominfo,
                void                          *client_data
               )
{
  if(!camcorder_useratom_info)
  {
    return;
  }
  if(camcorder_useratom_info->atom_is_valid)
  {
    *puseratominfo = *camcorder_useratom_info;
  }
}
#endif /* FEATURE_VIDEO_SETATOMS_DYNAMIC */

void camcorder_process_qdsp_start_record_msg(uint16 video_encode_rotation)
{
  video_init_type   init_data;

  memset(&init_data, 0, sizeof(video_init_type));
  init_data.file_format  = camera_file_format;
  init_data.video_format = camera_video_format;
  init_data.audio_format = camera_audio_format;
  init_data.file_brand   = camera_file_brand;
  init_data.short_header = camera_short_header;


  if (camera_odd_encode_rotation)
  {
    init_data.frame_width  = (uint16)camera_dy;
    init_data.frame_height = (uint16)camera_dx;
  }
  else
  {
    init_data.frame_width  = (uint16)camera_dx;
    init_data.frame_height = (uint16)camera_dy;
  }

  /* Default: 15 fps if upper layers do not specify fps */
  if(!camcorder_fps.denominator || !camcorder_fps.numerator)
  {
    camcorder_fps.numerator   = 0x1E; /* 30 */
    camcorder_fps.denominator = 0x2;  /* 2  */
  }

  init_data.time_increment  = camcorder_fps.denominator;
  init_data.time_resolution = camcorder_fps.numerator;

  switch(camera_profile)
  {
  case VIDEO_ENC_MAX_RATE :
    init_data.profile                = VIDEO_ENC_USER_DEFINED;
    init_data.max_bitrate_enable     = TRUE;
    init_data.dynamic_bitrate_enable = FALSE;
    break;

  case VIDEO_ENC_DYNAMIC_RATE:
    init_data.profile                = VIDEO_ENC_USER_DEFINED;
    init_data.max_bitrate_enable     = FALSE;
    init_data.dynamic_bitrate_enable = TRUE;
    break;

  default:
  init_data.profile         = camera_profile;
    init_data.max_bitrate_enable     = FALSE;
    init_data.dynamic_bitrate_enable = FALSE;
  }
  init_data.user_bitrate    = camera_user_bitrate;
  init_data.rotation_flag   = (int16) video_encode_rotation;
  init_data.destination     = camera_destination;
  init_data.text_type       = VIDEO_ENG_TEXT_INPUT_NONE;
  init_data.fixed_fps_mode  = FALSE;
  init_data.movie_size_limit = 0;
#ifndef FEATURE_CAMCORDER_AUDIO_REC_DISABLE
  init_data.audio_data1_cb  = camera_audio_ready_cb;
#endif /*FEATURE_CAMCORDER_AUDIO_REC_DISABLE*/
  init_data.reorder_atom    = FALSE;
  init_data.space_limit     = camera_space_limit;
#ifdef FEATURE_VIDEO_SETATOMS_DYNAMIC
  init_data.useratom_cbptr  = camcorder_copy_user_atoms_client_cb;
#endif
#ifdef FEATURE_VIDEO_MOVIEEDIT
  init_data.no_coding = FALSE;
#endif /* FEATURE_VIDEO_MOVIEEDIT */

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION

#ifdef FEATURE_CAMCORDER_AAC_REC
  if (init_data.audio_format == VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC)
  {
    init_data.audio_channel     = camera_audio_channel;
    init_data.audio_sample_rate = camera_audio_sample_rate;
    init_data.audio_profile     = camera_audio_profile;
  }
#endif /* FEATURE_CAMCORDER_AAC_REC */

  init_data.bufferHandle    = &encodeBufferHandle;

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

  if(VideoBuffersReturnEncodeBufferCount(&encodeBufferHandle,
                                         (int*)&init_data.bufferCount)
     == VIDEO_BUFFERS_FAILURE)
  {
    init_data.bufferCount = 0;
  }

  (void)video_eng_init (&init_data, camera_video_engine_cb,
                  (void*) CAMERA_VIDEO_ENGINE_INIT_CB);
}

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
/*===========================================================================

FUNCTION      camera_start_transition_effects

DESCRIPTION
  Called upon resume. The SampledTransitionType was set by
  camera_video_transition_effects_store() during pause.
  If there is a frame stored from the pause (last frame before pause),
  and the VidEncTransition_flag calls for a transition effect,
  then we can do the transition effects.

DEPENDENCIES

GLOBAL VARIABLES

RETURN VALUE
  None

SIDE EFFECTS
  Sets the bTransitionIsActive flag to TRUE or FALSE depending on whether
  we have a transition effect or not.

===========================================================================*/

static void camera_start_transition_effects( void )
{

  if(VidEncTransitionStruct.SampledTransitionType
       != VIDENCTRANSITION_NONE &&
     VidEncTransitionStruct.LastFrameForTransition.Y_Address != NULL
    )
  {
    /* All criteria are met for transition effect to begin
    */
    VidEncTransitionStruct.bTransitionIsActive = TRUE;

    /* length of transition set here */
    VidEncTransitionStruct.targetTransitionMsec =
      mp4e_get_time_ms() + VidEncTransition_msec;

    if(VidEncTransition_msec > 60000 )
    {
      MSG_ERROR("Warning: transition time limited to 1 minute",0,0,0);
      VidEncTransitionStruct.targetTransitionMsec =
        mp4e_get_time_ms() + 60000;
    }
  }
  else
  {
    /* transition effect will not happen */
    VidEncTransitionStruct.bTransitionIsActive = FALSE;
    /* free the previous frame buffer */
    if(VidEncTransitionStruct.LastFrameForTransition.Y_Address )
    {
      CAM_Free(VidEncTransitionStruct.LastFrameForTransition.Y_Address);
      VidEncTransitionStruct.LastFrameForTransition.Y_Address = NULL;
    }
    if(VidEncTransitionStruct.LastFrameForTransition.CbCr_Address )
    {
      CAM_Free(VidEncTransitionStruct.LastFrameForTransition.CbCr_Address);
      VidEncTransitionStruct.LastFrameForTransition.CbCr_Address = NULL;
    }

  }

}


/*===========================================================================

FUNCTION      camera_video_transition_effects_store

DESCRIPTION
   Called at the end of pause_record or stop_record.  Store the last frame
   in a persistent buffer so that upon start_record or resume_record the
   stored frame can be used for transition effects.

   Uses memcpy so it does not effect the input buffer.

   Can be called more than once ... in that case it would simply memcpy over
   the previously stored frame.  However if the dimensions are not right it
   will free the old memory before creating new one.

   If input is NULL then the save frame buffers will be NULL

DEPENDENCIES

EFFECTED GLOBALS:
   VidEncTransitionStruct.bTransitionIsActive = FALSE
   VidEncTransitionStruct.lastAddress is set if lastAddress is
     provided or freed if lastAddress is NULL

INPUT VARIABLES:
   Untouched.

RETURN VALUE
  None

SIDE EFFECTS
  Turns off transition effect if it was still active.
  On Error all buffers will be set to NULL.

===========================================================================*/

void camera_video_transition_effects_store(
  VideoBuffersYCbCrAddressPairsType *lastAddress)
{

  /* at this point sample VidEncTransition_flag */
  VidEncTransitionStruct.SampledTransitionType =
    (VidEncTransitionType)efx_special_fade;

  if(lastAddress == NULL ||
     VidEncTransitionStruct.SampledTransitionType==VIDENCTRANSITION_NONE
    )
  {
    /* this is the case of a stop */
    if(VidEncTransitionStruct.LastFrameForTransition.Y_Address != NULL)
    {
      CAM_Free(VidEncTransitionStruct.LastFrameForTransition.Y_Address);
      CAM_Free(VidEncTransitionStruct.LastFrameForTransition.CbCr_Address);
      VidEncTransitionStruct.LastFrameForTransition.Y_Address    = NULL;
      VidEncTransitionStruct.LastFrameForTransition.CbCr_Address = NULL;
    }

    return;
  }


  if(VidEncTransitionStruct.LastFrameForTransition.Y_Address == NULL )
  {

    VidEncTransitionStruct.LastFrameForTransition.Y_Address =
      CAM_Malloc(camera_dx * camera_dy );
    if(VidEncTransitionStruct.LastFrameForTransition.Y_Address == NULL)
    {
      MSG_ERROR("malloc error in transition store",0,0,0);
      return;
    }
    VidEncTransitionStruct.LastFrameForTransition.CbCr_Address =
      CAM_Malloc(camera_dx * camera_dy / 2);
    if(VidEncTransitionStruct.LastFrameForTransition.CbCr_Address == NULL)
    {
      free(VidEncTransitionStruct.LastFrameForTransition.Y_Address);
      MSG_ERROR("malloc error in transition store",0,0,0);
      return;
    }
  }

  VidEncTransitionStruct.LastFrameForTransition.image_format =
    lastAddress->image_format;

  memcpy(VidEncTransitionStruct.LastFrameForTransition.Y_Address,
         lastAddress->Y_Address,camera_dx*camera_dy);
  memcpy(VidEncTransitionStruct.LastFrameForTransition.CbCr_Address,
       lastAddress->CbCr_Address,camera_dx*camera_dy/2);
}
#endif /* ifdef FEATURE_VIDENC_TRANSITION_EFFECTS */

void camcorder_process_qdsp_output2_msg(Camera_EndOfFrameMessageType* msg)
{
  VideoBuffersYCbCrAddressPairsType *allocation;
  VideoBuffersYCbCrAddressPairsType  newBuffer;

  if( !camera_stopping &&
      !camera_stopping_record &&
      camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED )
  {

    if( CAMERA_DELAYED_ENCODE_FRAME_SIZE != output2BuffersQueueSize)
    {
      ASSERT(ackOutput2.eofAck2[0].Y_Address == NULL );
      allocation = VideoBuffersGetNextAvailableBuffer(&encodeBufferHandle);
      ackOutput2.eofAck2[0].Y_Address   = allocation->Y_Address;
      ackOutput2.eofAck2[0].CbCr_Address= allocation->CbCr_Address;
      MSG_CAMQDSPIO("next buffer %x",allocation->Y_Address,0,0);
    }

    newBuffer.Y_Address    = msg->eof_msg.output_Y_Address;
    newBuffer.CbCr_Address = msg->eof_msg.output_CbCr_Address;
    newBuffer.time         = msg->time;
    newBuffer.image_format = IPL_YCrCb420_LINE_PK;
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
    /* IPL based special effects processing
    */
    if( camera_special_effects_ptr 
#ifdef FEATURE_VIDEO_ENCODE_FADING
        || efx_fade_in || efx_fade_out_running
#endif
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
        ||  time_stamp_on 
#endif /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP */
        )
    {
      MSG_CAMQDSPIO("starting SE %d",mp4e_get_time_ms(),0,0);
      camera_special_effects_api_params.input_buffer.imgPtr =
        (unsigned char *) newBuffer.Y_Address;
      camera_special_effects_api_params.input_buffer.clrPtr =
        (unsigned char *) newBuffer.CbCr_Address;
      camera_special_effects_api_params.input_buffer.cFormat =
        IPL_YCrCb420_LINE_PK;
      camera_special_effects_api_params.packet.msec = newBuffer.time;
    }

    if (camera_special_effects_ptr ) {
      vse_return_type retval;

      retval = camera_special_effects_ptr(
        &camera_special_effects_api_params.packet,
        &camera_special_effects_api_params.vse_struct,
        VSE_RUN);
      ASSERT(retval == VSE_SUCCESS);
      ASSERT(camera_special_effects_api_params.packet.free_buffer_flag
             == INPUT_BUFFER_USED
             );

      MSG_CAMQDSPIO("SE done %d",mp4e_get_time_ms(),0,0);
    }
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP

    if ( time_stamp_on)
    {
      camera_timestamp_params.packet.msec = newBuffer.time;
      ( void ) vse_timestamp(&camera_timestamp_params.packet,
                           &camera_timestamp_params.vse_struct,
                           VSE_RUN);
    }
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  

#ifdef FEATURE_VIDEO_ENCODE_FADING
    if(efx_fade_in || efx_fade_out_running )
    {
      vse_return_type retval;

      camera_fadein_out_api_params.packet.msec = newBuffer.time;
      retval = vse_fadeInOut(&camera_fadein_out_api_params.packet,
                             &camera_fadein_out_api_params.vse_struct,
                             VSE_RUN);

      MSG_CAMQDSPIO("fading done %d",mp4e_get_time_ms(),0,0);

      if(efx_fade_out_running && retval == VSE_COMPLETED)
      {
        MSG_CAMQDSPIO("stop issued at end of fade out",0,0,0);
        efx_fade_out_running = FALSE;
        camera_video_engine_stop_requested  = TRUE;
        camera_video_engine_pause_requested = FALSE;
        camera_video_engine_paused = CAMERA_PAUSING_STATE_NOT_PAUSED;
      }
    }
#endif /* FEATURE_VIDEO_ENCODE_FADING */
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    if(VidEncTransitionStruct.bTransitionIsActive)
    { ipl_image_type imageFromVFE, storedFrame;
      uint32 alphaFadeFraction,curtainNPixels;
      ipl_status_type retval;

      if(newBuffer.time > VidEncTransitionStruct.targetTransitionMsec)
      {
        VidEncTransitionStruct.bTransitionIsActive = FALSE;
        CAM_Free(VidEncTransitionStruct.LastFrameForTransition.Y_Address);
        CAM_Free(VidEncTransitionStruct.LastFrameForTransition.CbCr_Address);
        VidEncTransitionStruct.LastFrameForTransition.Y_Address    = NULL;
        VidEncTransitionStruct.LastFrameForTransition.CbCr_Address = NULL;
      }
      else
      {
        imageFromVFE.cFormat = IPL_YCrCb420_LINE_PK;
        imageFromVFE.dx      = camera_dx;
        imageFromVFE.dy      = camera_dy;
        imageFromVFE.imgPtr  = (unsigned char *) newBuffer.Y_Address;
        imageFromVFE.clrPtr  = (uint8 *)         newBuffer.CbCr_Address;

        storedFrame.cFormat =
          VidEncTransitionStruct.LastFrameForTransition.image_format;
        storedFrame.imgPtr  = (unsigned char *)
             VidEncTransitionStruct.LastFrameForTransition.Y_Address;
        storedFrame.clrPtr  = (uint8 *)
             VidEncTransitionStruct.LastFrameForTransition.CbCr_Address;
        storedFrame.dx      = camera_dx;
        storedFrame.dy      = camera_dy;


        switch(VidEncTransitionStruct.SampledTransitionType)
        {
        case VIDENCTRANSITION_ALPHA_FADE:
          alphaFadeFraction = 255*
            (VidEncTransitionStruct.targetTransitionMsec -  newBuffer.time);
          alphaFadeFraction = 255 - (alphaFadeFraction /
                                     VidEncTransition_msec);

          retval =
            ipl_compose(&imageFromVFE,&storedFrame,NULL,IPL_BLEND,
                      &alphaFadeFraction);
          break;
        case VIDENCTRANSITION_CURTAIN_FADE:
          curtainNPixels = camera_dx*
            (VidEncTransitionStruct.targetTransitionMsec -
                       newBuffer.time);
          curtainNPixels = camera_dx -
            (curtainNPixels / VidEncTransition_msec);
          retval =
            ipl_compose(&imageFromVFE,&storedFrame,&imageFromVFE,IPL_CENTER,
                      &curtainNPixels);
          break;
        default:
          MSG_ERROR("Warning: Invalid transition type",0,0,0);
          retval = IPL_FAILURE;
        }

        if(retval != IPL_SUCCESS )
        {
          MSG_ERROR("transition effect %d failed with status %d",
                    VidEncTransitionStruct.SampledTransitionType,
                    retval,0);
          /* if we set the time to the current time, then on the
          ** next frame it will shut off automatically
          */
          VidEncTransitionStruct.targetTransitionMsec = newBuffer.time;
        }
      }

    }
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

    MSG_CAMQDSPIO("enqueing %x T=%d",newBuffer.Y_Address,newBuffer.time,0);
    ( void ) VideoBufferEnqueueUse(&encodeBufferHandle,newBuffer);

    output2enc_ack2_criteria(SPECIAL_EFFECTS_DONE2);

    #ifdef FEATURE_POSTCARD_RECORDING
#error code not present
    #endif /* FEATURE_POSTCARD_RECORDING */

  } /* if NO_STOPPING */
#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
  else if ((camera_stopping_record &&          /* Stopping Record */
            camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED) ||
           (!camera_stopping &&          /* Pausing Record  */
            camera_video_engine_paused == CAMERA_PAUSING_FLUSHING_BUFFER))
  {
    MSG_HIGH("Received output2 frame during pausing/stopping record",0,0,0);

    /* We are in the process of pausing or stopping record so we need
    ** output2 interrupts to throttle the encoder to prevent AHB overflow */
    if(camera_video_engine_throttle && !camera_encoding_frame)
    {
      /* Kick off throttled encoding right here if we are not currently encoding */
      VideoBuffersYCbCrAddressPairsType YCbCrAddresses;
      VideoBufferHandle                 *videoBufferHandle;
      boolean                           notDropped;
      int                               nBufs;

      MSG_HIGH("camera_process_qdsp_output2_msg: Start throttled encoding",0,0,0);

      videoBufferHandle = &encodeBufferHandle;

      VideoBufferDequeueUse (videoBufferHandle, &YCbCrAddresses);


      /* we should return this to pool ONLY if this is not the
      ** buffer that we're going to ack2 on
      */
      if ( ackOutput2.eofAck2[0].Y_Address != YCbCrAddresses.Y_Address )
      {
        MSG_CAMQDSPIO("return %x to pool",YCbCrAddresses.Y_Address,0,0);
        VideoBufferReturnBufferToPool (videoBufferHandle, YCbCrAddresses);
      }

      while((nBufs=VideoBufferUseQueueSize(videoBufferHandle)) > 0)
      {
        VideoBufferPeakUse (videoBufferHandle, &YCbCrAddresses);        
        if((notDropped=video_eng_check_frame_encode_permitted(videoBufferHandle,
                                              YCbCrAddresses.time))==FALSE)
        {
           VideoBufferDequeueUse(videoBufferHandle,&YCbCrAddresses);
           VideoBufferReturnBufferToPool(videoBufferHandle,YCbCrAddresses);
           MSG_CAMQDSPIO("dropped, dQ-> Pool %x nbufs %d",
                      YCbCrAddresses.Y_Address,nBufs,0);
        }
        else
            break;
      }

      if (nBufs == 0)
      {

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
        if( camera_video_engine_paused != CAMERA_PAUSING_STATE_NOT_PAUSED)
        {
          camera_video_transition_effects_store(&YCbCrAddresses);
        }
        else
        {
          /* in the case of a stop, we free all malloc'ed buffers */
          camera_video_transition_effects_store(NULL);
        }
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

        /* there is one more buffer in the encoder queue
        ** cannot drop this one or we'll loose clip time at
        ** the end
        */
        camera_encoding_frame = TRUE;
        MSG_CAMQDSPIO("encoding NULL (flushing)",0,0,0);
        video_eng_frame(NULL,NULL,0,0,camera_video_engine_cb,
                        (void *) CAMREA_VIDEO_ENGINE_FLUSH_CB);
      }
      else
      {
        ( void ) camcorder_svcs_initiate_backend_encode();
      }
    }
    else
    {
      /* We have already sent an ACK2 in this case in camera_qdsp_cb */
      MSG_HIGH("camera_process_qdsp_output2_msg: Throttled encoding busy, send ACK",0,0,0);
    }
  }
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */
  else
  {
    MSG_HIGH("Received output2 frame in wrong state, ACK immediately",0,0,0);

    ackOutput2.eofAck2[0].Y_Address    = msg->eof_msg.output_Y_Address;
    ackOutput2.eofAck2[0].CbCr_Address = msg->eof_msg.output_CbCr_Address;
    ( void ) VFE_EOF_Ack2(ackOutput2);
    ackOutput2.eofAck2[0].Y_Address    = NULL;
  }
}

void camcorder_process_qdsp_config_done_msg(CAMQDSP_VideoEncStateType * vidEncState)
{
  VideoEncDSP_EncoderSelectionCommandType *encoderSelectionConfig;

  if ( (*vidEncState) != CAMQDSP_VIDEO_ENC_STATE_VIDEO_INIT )
  {
    /* we're not in the init state. send sleep command
    */
    if ( VideoEncDSP_VideoEncSleep ()  != CAMQDSP_SUCCESS )
    {
      camera_log (LOG_INFO, 0, 0, (uint32)CFG_REJ, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      video_eng_stop(camera_video_engine_cb, (void*) CAMERA_VIDEO_ENGINE_STOP_CB);
      MSG_ERROR ("Cannot configure video encoder. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_CONFIG_VFE);
      return;
    }
  }
  else
  {

    encoderSelectionConfig = mp4e_codec_selection ();
    if ( (encoderSelectionConfig == NULL)  ||
     (VideoEncDSP_VideoEncSelection (encoderSelectionConfig)
       != CAMQDSP_SUCCESS))
    {
      camera_log (LOG_INFO, 0, 0, (uint32)CFG_REJ, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      video_eng_stop(camera_video_engine_cb, (void*) CAMERA_VIDEO_ENGINE_STOP_CB);
      MSG_ERROR ("Cannot select video encoder. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_CONFIG_VFE);
      return;
    }
  }
  if (camera_record_vfe_reset_block_be_restart)
  {

    /* we're here because the VFE reconfigured but BackEnd does not
    ** need a reset.
    */

    camera_record_vfe_reset_block_be_restart = FALSE;

    /* send a fake selection done message so the state machine
    ** can move forward
    */
    graph_queue_camqdsp_msg (CAMQDSP_VIDEO_ENCODE_SELECTION_DONE,
                             (void *) 0, 0
                            );

    return;
  }
}

void camcorder_process_alloc_memory(uint32 camera_encode_buffer_size)
{
  VideoBuffersStatus ret;

#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
  /* Restrictions placed because we're using ARM to do special
  ** effects procesisng:
  **  1. Encode from OUTPUT1, which provides LINE_PAK format
  **  2. OUTPUT1 (for display) must equal OUTPUT2
  **
  */
  if ( camera_special_effects_ptr != NULL
#ifdef FEATURE_VIDEO_ENCODE_FADING
       || efx_fade_in || efx_fade_out
#endif
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
       || time_stamp_on 
#endif  /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP */
       )
  {
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
    camera_initialize_timestamp();
#endif /*FEATURE_CAMCORDER_OVERLAY_TIMESTAMP*/  
#ifdef FEATURE_VIDEO_ENCODE_FADING
    camera_initialize_fadeInOut();
    if (efx_fade_in)
    {
      camera_fadein_out_api_params.params.fadeInOutCommand =
         VSE_FADE_IN_COMMAND;
      ( void ) vse_fadeInOut(&camera_fadein_out_api_params.packet,
                           &camera_fadein_out_api_params.vse_struct,
                           VSE_INIT_ENG);
    }
#endif
  }
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
  MSG_CAMERADEBUG ("CAMERA_SVCS: Video Buffer Allocate", 0, 0, 0);
  ret = VideoBuffersAllocate (
    &encodeBufferHandle,
    CAMERA_TOTAL_BACKEND_BUFFER_SIZE,
    CAMERA_DELAYED_ENCODE_FRAME_SIZE,
    (int) camera_encode_buffer_size);
  if (ret != VIDEO_BUFFERS_SUCCESS)
  {
    camera_no_memory ();
  }
}

void camcorder_config_vfe( void )
{
  VideoBuffersYCbCrAddressPairsType *addressPair;
  memset(&video_encode_3x3filter,0,sizeof(video_enc_3x3_luma_filter_type));

  vfeVideoConfig.output2Configuration.startBackend   = VFE_Disable;
  vfeVideoConfig.output2Configuration.width          = camera_dx;
  vfeVideoConfig.output2Configuration.height         = camera_dy;
  vfeVideoConfig.output2Configuration.format         = VFE_O2YCbCr420_LinePack;

  vfeVideoConfig.output2Configuration.output2Enable    = VFE_Enable;
  addressPair = VideoBuffersGetNextAvailableBuffer (&encodeBufferHandle);
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer0   =
      (VFE_AddressType) addressPair->Y_Address;
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer0 =
      (VFE_AddressType) addressPair->CbCr_Address;
  addressPair = VideoBuffersGetNextAvailableBuffer (&encodeBufferHandle);
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer1   =
      (VFE_AddressType) addressPair->Y_Address;
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer1 =
      (VFE_AddressType) addressPair->CbCr_Address;
  addressPair = VideoBuffersGetNextAvailableBuffer (&encodeBufferHandle);
    vfeVideoConfig.output2Buffer.Y_OutputTripleBuffer2   =
      (VFE_AddressType) addressPair->Y_Address;
    vfeVideoConfig.output2Buffer.CbCrOutputTripleBuffer2 =
      (VFE_AddressType) addressPair->CbCr_Address;

  if(VFE_O2YCbCr420_LinePack == vfeVideoConfig.output2Configuration.format)
  {
    vfeVideoConfig.output2_AHB_Configuration = vfeVideoConfig.output1_AHB_Configuration;
  }

  vfeVideoConfig.videoOutput2FrameSkip                 = vfeVideoConfig.videoOutput1FrameSkip;
  vfeVideoConfig.colorConversion                       = vfeUpdate.colorConversion;
#ifndef QDSP_MODULE_VFE25_DEFINED
  /* Video driver will determine what filter the vfe should be initialized
  ** with. When we include dynamic filter updates, video_encode_update_filter
  ** will have to be called during recording */
  if((vfe_hw_features.filter_3x3_luma == TRUE) &&
     video_encode_update_filter(&video_encode_3x3filter))
  {
    /* Make sure luma filter is enabled */
    vfeVideoConfig.f3x3LumaFilter.enable = VFE_Enable;

    /* Update the 9 filter coefficients */
    vfeVideoConfig.f3x3LumaFilter.A11 = video_encode_3x3filter.A11;
    vfeVideoConfig.f3x3LumaFilter.A12 = video_encode_3x3filter.A12;
    vfeVideoConfig.f3x3LumaFilter.A13 = video_encode_3x3filter.A13;

    vfeVideoConfig.f3x3LumaFilter.A21 = video_encode_3x3filter.A21;
    vfeVideoConfig.f3x3LumaFilter.A22 = video_encode_3x3filter.A22;
    vfeVideoConfig.f3x3LumaFilter.A23 = video_encode_3x3filter.A23;

    vfeVideoConfig.f3x3LumaFilter.A31 = video_encode_3x3filter.A31;
    vfeVideoConfig.f3x3LumaFilter.A32 = video_encode_3x3filter.A32;
    vfeVideoConfig.f3x3LumaFilter.A33 = video_encode_3x3filter.A33;

    /* Update the Q factor */
    vfeVideoConfig.f3x3LumaFilter.coefficient_Q_Factor =
      (VFE_Coefficient_3Bit_Q_FactorType)
       video_encode_3x3filter.coefficient_Q_Factor;
  }
#endif /* nQDSP_MODULE_VFE25_DEFINED */
}
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
camera_ret_code_type camcorder_svcs_set_overlay (
                    ipl_image_type ** preview_overlay_addr, 
                        ipl_image_type * work,uint32 size,
                        void* overlay_bmp)
{
  camera_ret_code_type retval = CAMERA_INVALID_STATE;
  if (overlay_allocated)
  {
    if (overlay_bmp == NULL)
    {
      //we need to disable overlay
      *preview_overlay_addr = NULL;
      camera_special_effects_ptr = NULL;
#ifdef FEATURE_MDP
      camera_svcs_display_frame_overlay(TRUE);
#endif /* FEATURE_MDP */
      retval = CAMERA_SUCCESS;
    }
    else if (overlay_bmp != (void*)1)
    {
      //enable the overlay
      work->imgPtr = temp_overlay.imgPtr;
      *preview_overlay_addr = preview_overlay_buffer;
      retval = CAMERA_SUCCESS;
    }
  }
  else
  {
    if (overlay_bmp == (void*)1)
    {
      if (preview_overlay_buffer || camera_video_eng_overlay.frame)
      {
        return CAMERA_INVALID_STATE;
      }
      //allocate all the buffers at once and keep it for future use
      temp_overlay.imgPtr = (uint8*)CAM_Malloc(size|CAMCORDER_ALLOC_NO_ZMEM);
      if (!temp_overlay.imgPtr)
        return CAMERA_NO_MEMORY;
      MSG_HIGH("temp_overlay allocated with %d bytes", size, 0, 0);
      preview_overlay_buffer = (ipl_image_type*)CAM_Malloc(sizeof(ipl_image_type));
      if (!preview_overlay_buffer)
      {
        CAM_Free (temp_overlay.imgPtr);
        temp_overlay.imgPtr = NULL;
        return CAMERA_NO_MEMORY;
      }
      preview_overlay_buffer->imgPtr = 
  (uint8*)CAM_Malloc(size|CAMCORDER_ALLOC_NO_ZMEM);
      if (!preview_overlay_buffer->imgPtr)
      {
        CAM_Free (temp_overlay.imgPtr);
        temp_overlay.imgPtr = NULL;
        CAM_Free(preview_overlay_buffer->imgPtr);
        CAM_Free (preview_overlay_buffer);
        preview_overlay_buffer = NULL;
        MSG_ERROR("memory allocation for preview_overlay failed",0,0,0);
        return CAMERA_NO_MEMORY;
      }
      camera_video_eng_overlay.frame = 
  (ipl_image_type*)CAM_Malloc(sizeof(ipl_image_type));
      if (!camera_video_eng_overlay.frame)
      {
        CAM_Free (temp_overlay.imgPtr);
        temp_overlay.imgPtr = NULL;
        CAM_Free(preview_overlay_buffer->imgPtr);
        CAM_Free (preview_overlay_buffer);
        preview_overlay_buffer = NULL;
        MSG_ERROR("overlay.frame memory allocation failed ", 0, 0, 0);
        return CAMERA_NO_MEMORY;
      }
      camera_video_eng_overlay.frame->imgPtr = 
    (uint8*)CAM_Malloc(size | CAMCORDER_ALLOC_NO_ZMEM);
      if (!camera_video_eng_overlay.frame->imgPtr)
      {
        CAM_Free (temp_overlay.imgPtr);
        temp_overlay.imgPtr = NULL;
        CAM_Free(preview_overlay_buffer->imgPtr);
        CAM_Free (preview_overlay_buffer);
        preview_overlay_buffer = NULL;
        CAM_Free(camera_video_eng_overlay.frame);
        camera_video_eng_overlay.frame = NULL;
        MSG_ERROR("overlay.frame->imgPtr memory allocation failed ", 0, 0, 0);
        return CAMERA_NO_MEMORY;
      }
      camera_video_eng_overlay.frame->dx  = camera_dx;
      camera_video_eng_overlay.frame->dy  = camera_dy;

      memset(&camera_special_effects_api_params,0, 
   sizeof(camera_special_effects_api_params));
      camera_special_effects_api_params.packet.input_buffer =
        &camera_special_effects_api_params.input_buffer;
      camera_special_effects_api_params.input_buffer.dx = camera_dx;
      camera_special_effects_api_params.input_buffer.dy = camera_dy;
      camera_special_effects_api_params.input_buffer.cFormat = IPL_YCrCb420_LINE_PK;
      camera_special_effects_api_params.packet.param =
        (void *) &camera_video_eng_overlay;
      camera_special_effects_api_params.vse_struct.npix_x = camera_dx;
      camera_special_effects_api_params.vse_struct.npix_y = camera_dy;

      overlay_allocated = TRUE;
      retval = CAMERA_SUCCESS;
    }
  }
  return retval;
}
camera_ret_code_type camcorder_set_video_eng_overlay
                     (uint16 camera_transparent_color, 
                     ipl_image_type *ui_overlay_ptr)
{
  if (ui_overlay_ptr->imgPtr)
  {
    camera_video_eng_overlay.frame->cFormat = ui_overlay_ptr->cFormat;
    MSG_HIGH("camcorder_set_video_eng_overlay: copying image to set overlay", 0, 0, 0);
    memcpy(camera_video_eng_overlay.frame->imgPtr, ui_overlay_ptr->imgPtr, 
     (ui_overlay_ptr->dx*ui_overlay_ptr->dy*2));
    camera_video_eng_overlay.transparent_value = camera_transparent_color;
    /*lint -save -e620 VSE funny thing */
    camera_video_eng_overlay.valid_check = VSE_OVERLAY_VALID_MAGIC;
    camera_video_eng_overlay.new_frame = TRUE;
    /*lint -restore */
    camera_special_effects_ptr = vse_overlay;
  }
  return CAMERA_SUCCESS;
}
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */

void camcorder_svcs_update_vfe_filter( void )
{
#ifndef QDSP_MODULE_VFE25_DEFINED
  /* Check if video encoder has indicated need to update VFE filter */
  if((vfe_hw_features.filter_3x3_luma == TRUE) &&
     video_encode_update_filter(&video_encode_3x3filter))
  {
    /* Make sure luma filter is enabled */
    vfeUpdate.f3x3LumaFilter.enable = (VFE_EnableType)video_encode_3x3filter.enable;

    /* Update the 9 filter coefficients */
    vfeUpdate.f3x3LumaFilter.A11 = video_encode_3x3filter.A11;
    vfeUpdate.f3x3LumaFilter.A12 = video_encode_3x3filter.A12;
    vfeUpdate.f3x3LumaFilter.A13 = video_encode_3x3filter.A13;

    vfeUpdate.f3x3LumaFilter.A21 = video_encode_3x3filter.A21;
    vfeUpdate.f3x3LumaFilter.A22 = video_encode_3x3filter.A22;
    vfeUpdate.f3x3LumaFilter.A23 = video_encode_3x3filter.A23;

    vfeUpdate.f3x3LumaFilter.A31 = video_encode_3x3filter.A31;
    vfeUpdate.f3x3LumaFilter.A32 = video_encode_3x3filter.A32;
    vfeUpdate.f3x3LumaFilter.A33 = video_encode_3x3filter.A33;

    /* Update the Q factor */
    vfeUpdate.f3x3LumaFilter.coefficient_Q_Factor =
      (VFE_Coefficient_3Bit_Q_FactorType)
       video_encode_3x3filter.coefficient_Q_Factor;

    /* Indicate need to update VFE */
    camera_update_vfe = TRUE;

    MSG_HIGH("camera svcs updating filter struct",0,0,0);
  }
#endif /* nQDSP_MODULE_VFE25_DEFINED */ 
}

boolean camcorder_qdsp_cb(Camera_EndOfFrameMessageType *endOfFrameMessagePtr, CAMQDSP_ResponseMsgType responseMsg)
{
  if(camera_vfe_control_and_status.reconfigure_vfe )
  {
    /* We received an output2 while we were trying to reset VFE
    ** This output2 interrupt will be rejected.
    */
    return FALSE;
  }

  if ( !camera_stopping &&
       !camera_stopping_record &&
      camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED )
  {
    VideoBuffersYCbCrAddressPairsType   YCbCrAddress;
    VideoBuffersStatus                  stat;

    camera_recording_vfe_process_output2();

    ASSERT(ackOutput2.eofAck2[0].Y_Address == NULL);

    graph_queue_camqdsp_msg (responseMsg, (void *) endOfFrameMessagePtr, 0);
    output2BuffersQueueSize = VideoBufferUseQueueSize(&encodeBufferHandle);

    if (output2BuffersQueueSize == CAMERA_DELAYED_ENCODE_FRAME_SIZE)
    {
      /* get the next buffer to encode */
      stat = VideoBufferPeakUse(&encodeBufferHandle,&YCbCrAddress);

      ASSERT(stat == VIDEO_BUFFERS_SUCCESS);

      MSG_CAMQDSPIO("Encode %x %d",
                    YCbCrAddress.Y_Address,mp4e_get_time_ms(),0
                   );

      ASSERT(camera_encoding_frame == FALSE);
#ifdef FEATURE_VIDEO_ENG_FRAME_PROCESSING_API
      if (video_eng_check_frame_encode_permitted(&encodeBufferHandle,  YCbCrAddress.time))
      {
        camera_encoding_frame = TRUE;

        /* fire off the encode process */
        video_eng_frame((uint8 *) YCbCrAddress.Y_Address,(uint8 *) YCbCrAddress.CbCr_Address,
                        YCbCrAddress.image_format,YCbCrAddress.time,
                        camera_video_engine_cb,
                        (void *) CAMERA_VIDEO_ENGINE_ENCODE_FRAME_CB
                       );
        /* set up Ack1 buffers for release by handshake*/
        ackOutput2.eofAck2[0].Y_Address =
           (VFE_AddressType) YCbCrAddress.Y_Address;
        ackOutput2.eofAck2[0].CbCr_Address =
           (VFE_AddressType) YCbCrAddress.CbCr_Address;
      }
      else
#endif /* FEATURE_VIDEO_ENG_FRAME_PROCESSING_API */
      {
        VideoBuffersYCbCrAddressPairsType addressPair;
        ( void ) VideoBufferDequeueUse(&encodeBufferHandle,&addressPair);
        ackOutput2.eofAck2[0].Y_Address    = addressPair.Y_Address;
        ackOutput2.eofAck2[0].CbCr_Address = addressPair.CbCr_Address;
        graph_queue_camqdsp_msg(CAMQDSP_OUTPUT2ACK2_CRITERIA,NULL,0);
        event_report(EVENT_CAMCORDER_FRAME_DROP);
      }
    }
    else
    {
      graph_queue_camqdsp_msg(CAMQDSP_OUTPUT2ACK2_CRITERIA,NULL,0);
    }
  }  /* if NO_STOPPING */
#ifdef FEATURE_VIDEO_ENCODE_THROTTLE
  else if ((camera_stopping_record &&           /* Stopping Record */
            camera_video_engine_paused == CAMERA_PAUSING_STATE_NOT_PAUSED) ||
           (!camera_stopping &&           /* Pausing Record  */
            camera_video_engine_paused == CAMERA_PAUSING_FLUSHING_BUFFER))
  {
    /* In the event that we are pausing or stopping record, we want to
    ** use this signal to trigger the BE DSP to start encoding next frame
    ** Queue this event onto graph task to process this frame */

    if(camera_video_engine_throttle && !camera_encoding_frame)
    {
      MSG_HIGH("camera_qdsp_cb: Pausing/Stopping Record, Queue encoding event",0,0,0);
      graph_queue_camqdsp_msg (responseMsg, (void *) endOfFrameMessagePtr, 0);
    }
    else
    {
      camera_log (LOG_RSP, 0, (int)responseMsg, 0, __LINE__);
    }

    /* This frame is used as a trigger only, we need to don't encode it */
    ackOutput2.eofAck2[0].Y_Address    = endOfFrameMessagePtr->eof_msg.output_Y_Address;
    ackOutput2.eofAck2[0].CbCr_Address = endOfFrameMessagePtr->eof_msg.output_CbCr_Address;
    VFE_EOF_Ack2(ackOutput2);
    ackOutput2.eofAck2[0].Y_Address    = NULL;
  }
#endif /* FEATURE_VIDEO_ENCODE_THROTTLE */
  else
  {
    camera_log (LOG_RSP, 0, (int)responseMsg, 0, __LINE__);
    /* Ack output2 instantly if in 'recording' and 'paused' state. */
    ackOutput2.eofAck2[0].Y_Address = endOfFrameMessagePtr->eof_msg.output_Y_Address;
    ackOutput2.eofAck2[0].CbCr_Address = endOfFrameMessagePtr->eof_msg.output_CbCr_Address;
    ( void ) VFE_EOF_Ack2(ackOutput2);
    ackOutput2.eofAck2[0].Y_Address    = NULL;
  }
  return TRUE;
}

#ifdef CAMCORDER_VFE_RECONFIG_TEST
/* this is the timer callback
** it will be called from timer task or isr
*/
static void vfe_reconfigure_cb(int32 ms)
{
  graph_queue_camqdsp_msg (CAMQDSP_VFE_RECONFIGURE_UNIT_TEST, NULL, 0);
}

/* this is called from the GRAPH task
*/
void vfe_process_reconfigure_unit_test( void )
{
  MSG_CAMQDSPIO("reconfigure vfe sim",0,0,0);
  camera_vfe_control_and_status.reconfigure_vfe = TRUE;
  /* set off another reconfigure clock in 2 seconds */
  clk_reg (&vfe_reconfigure_clk, vfe_reconfigure_cb,
           2000, 0, FALSE);
}
#endif  // #ifdef CAMCORDER_VFE_RECONFIG_TEST



#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */



/*===========================================================================

FUNCTION     CAMCORDER_SVCS_INITIATE_BACKEND_ENCODE

DESCRIPTION
  This function does the following operations:
  - Check if the Used Queue is not empty.
  - If there are buffers in the used queue then, get the buffer information
    at the beginning of the queue.
  - Send the buffer for backend processing by calling video_eng_frame().

DEPENDENCIES
  

RETURN VALUE
  void 

SIDE EFFECTS
  None

===========================================================================*/
static void camcorder_svcs_initiate_backend_encode( void )
{
  VideoBuffersYCbCrAddressPairsType YCbCrAddresses = {0};

  if(VideoBufferUseQueueSize (&encodeBufferHandle) != 0)
  {
    ( void ) VideoBufferPeakUse (&encodeBufferHandle, &YCbCrAddresses);
    MSG_CAMQDSPIO("In camcorder_svcs_initiate_backend_encode, encoding %x",
                  YCbCrAddresses.Y_Address,0,0);
    camera_encoding_frame = TRUE;
    video_eng_frame((uint8 *) YCbCrAddresses.Y_Address,
                    (uint8 *) YCbCrAddresses.CbCr_Address,
                    YCbCrAddresses.image_format,
                    YCbCrAddresses.time,
                    camera_video_engine_cb,
                    (void *) CAMERA_VIDEO_ENGINE_ENCODE_FRAME_CB
                   );
  }
} /* CAMCORDER_SVCS_INITIATE_BACKEND_ENCODE */

/*===========================================================================

FUNCTION     CAMCORDER_PROCESS_SET_DEFAULT_GLOBAL_PARMS

DESCRIPTION
  This function does the following operations:
  - Resets the global camcorder parameters

DEPENDENCIES
  

RETURN VALUE
  void 

SIDE EFFECTS
  None

===========================================================================*/
void camcorder_process_set_default_global_parms( void )
{
#ifdef FEATURE_CAMCORDER_OVERLAY_TIMESTAMP
   time_stamp_on = FALSE;
#endif /* FEATURE_CAMCORDER_OVERLAY_TIMESTAMP */

#ifdef FEATURE_POSTCARD_RECORDING
#error code not present
#endif /* FEATURE_POSTCARD_RECORDING */

#ifdef FEATURE_VIDEO_ENCODE_FADING
/*  True if we are generating fade-in or fade-out special effects
*/
   efx_fade_in = FALSE;
   efx_fade_out = FALSE;
#endif /* FEATURE_VIDEO_ENCODE_FADING */

} /*CAMCORDER_PROCESS_SET_DEFAULT_PARMS*/
#endif//FEATURE_VIDEO_ENCODE
