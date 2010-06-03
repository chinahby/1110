#ifndef CAMERA_SVCS_H
#define CAMERA_SVCS_H

/*============================================================================

                 Camera Services Header File

   DESCRIPTION
     This file contains camera application specific support for the
     general camera services interface.

============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameraservices/main/latest/inc/camera_svcs.h#4 $ $DateTime: 2009/02/12 03:16:09 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 02/12/09  cgera  Added support for MULTISHOT AEC 1.1,5MP SENSOR support
 08/04/08  hap    Reverting back the prototype changes
 06/17/08  hap    Remove the prototypes for camera_set_preview_buffers & 
                  camera_set_preview_headers
 04/04/08  kd     Added support for mobicat preview over Chromatix for PC
 11/17/07  Yogi   Added Post Card Recording Support
 06/26/07  arv    Vote for HCLK restriction during camera preview and snapshot
 03/07/07  zl     Updated FEATURE_CAMERA_MULTI_SENSOR support.
 02/27/07  jch    Include sleep.h such that camera votes to sleep. fix cr 109954
 02/22/07  Gopi   Changes to prevent Flashing screen in camcorder app
 11/13/06  kadhir Moved camcorder specific functions
                  to seperate file "camcorder_svcs.c"
 11/03/06  ven    Added latency logging for various modules.
 01/17/05  drl    Creation.

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/


#ifdef FEATURE_CAMERA
#include "camerai.h"
#include "vfe.h"
#include "camsensor.h"
#include "sleep.h"

/*============================================================================
*                         STRUCTURE DEFINITIONS
============================================================================*/

typedef struct
{

  uint32 autofocus_start;                         /* camera_af_start_focus */
  uint32 autofocus_end;                           /* camera_af_stop_focus */
  uint32 stop_viewfinder_start;                   /* camera_svcs_stop_preview */
  uint32 take_snapshot_start;                     /* camera_svcs_take_picture */
  uint32 raw_config_end;                          /* camera_process_qdsp_config_done_msg */
  uint32 raw_capture_start;                       /* camera_process_qdsp_config_done_msg */
  uint32 raw_capture_end;                         /* camera_handle_frames_for_takepicture */
  uint32 bvcm_start;                              /* camera_svcs_process_bvcm */
  uint32 bvcm_end;                                /* camera_svcs_bvcm_cb */
  uint32 hjr_single_frame_start;                  /* camera_handle_single_frame_for_handjitter */
  uint32 hjr_single_frame_end;                    /* camera_handle_single_frame_for_handjitter */
  uint32 offline_snapshot_config_start;           /* camera_svcs_bvcm_cb & camera_handle_frames_for_takepicture */
  uint32 offline_snapshot_config_end;             /* camera_process_qdsp_config_done_msg */
  uint32 offline_snapshot_capture_start;          /* camera_process_qdsp_config_done_msg */
  uint32 offline_snapshot_capture_end;            /* camera_handle_frames_for_takepicture */
  uint32 snapshot_config_end;                     /* camera_process_qdsp_config_done_msg */
  uint32 snapshot_capture_start;                  /* camera_process_qdsp_config_done_msg */
  uint32 snapshot_capture_end;                    /* camera_handle_frames_for_takepicture */
  uint32 downsizer_start;                         /* camera_handle_frames_for_takepicture */
  uint32 downsizer_end;                           /* camera_svcs_ycbcr_downsizer_cb */
  uint32 overlay_start;                           /* camera_apply_overlay_on_snapshot */
  uint32 overlay_end;                             /* camera_apply_overlay_on_snapshot */
  uint32 hjr_two_frame_start;                     /* camera_handle_multi_frames_for_handjitter */
  uint32 hjr_two_frame_end;                       /* camera_handle_multi_frames_for_handjitter */
  uint32 hjr_three_frame_start;                   /* camera_handle_multi_frames_for_handjitter */
  uint32 hjr_three_frame_end;                     /* camera_handle_multi_frames_for_handjitter */
  uint32 lpm_start;                               /* camera_svcs_process_lpm */
  uint32 lpm_end;                                 /* camera_svcs_lpm_cb */
  uint32 luma_adaptation_time;
  uint32 chroma_suppression_time;
  uint32 luma_filter_three_by_three_time;
  uint32 asf_five_by_five_single_fragment_time;
  uint32 encode_start;                            /* camera_svcs_encode_picture */
  uint32 encode_end;                              /* camera_jpeg_encode_callback */
  uint32 snapshot_completion_callback_start;      /* camera_jpeg_encode_callback */
  uint32 viewfinder_start;                        /* camera_svcs_start_preview */
  uint32 first_frame_to_display;                  /* camera_svcs_blt_ext */

} camera_snapshot_timing_type;


typedef struct
{
  uint32 arm_clock;
  uint32 dsp_clock;
  uint32 edge_clock;
} camera_clock_information_type;


typedef struct
{
  uint32 encode_picture_width;
  uint32 encode_picture_height;
  uint32 encoded_data_size;
  uint8  autofocus_mode;

  camera_clock_information_type snapshot_routine;
  camera_clock_information_type bvcm_routine;
  camera_clock_information_type lpm_routine;
  camera_clock_information_type jpeg_routine;

} camera_snapshot_information_type;

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /*FEATURE_CAMERA_AEC_DURING_MULTISHOT*/


typedef struct Camera_EndOfFrameMessageType
{
  CAMQDSP_EndOfFrameMessageType   eof_msg;
  uint32                          time;
} Camera_EndOfFrameMessageType;

typedef enum
{
  /* */
  DSP_DISABLED,
  /* */
  DSP_ENABLING,
  /* */
  DSP_ENABLED,
  /* */
  DSP_CONFIGURING,
  /* */
  DSP_CONFIGURED
} camera_dsp_state_type;

/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

#ifdef FEATURE_CLKRGM_AHB_CLIENT
/* Tell clkregime you don't need the bus */
#define CAMERA_BUS_OFF() {clk_regime_deregister_ahb_constant(CLK_AHB_CAMERA);}
/* Tell clkregime you need the bus and not to drop it */
#define CAMERA_BUS_ON() {clk_regime_register_ahb_constant(CLK_AHB_CAMERA);}
#else
#define CAMERA_BUS_OFF()
#define CAMERA_BUS_ON()
#endif

/* Halting shuts down Arm */
#ifdef SLEEP_CAMERA_OKTH_SIG
#define CAMERA_HALT_ALLOW() \
  { (void) rex_set_sigs( &sleep_tcb, SLEEP_CAMERA_OKTH_SIG); }
#define CAMERA_HALT_FORBID() \
  { (void) rex_clr_sigs( &sleep_tcb, SLEEP_CAMERA_OKTH_SIG); }
#else /* SLEEP_CAMERA_OKTH_SIG */
#define CAMERA_HALT_ALLOW()
#define CAMERA_HALT_FORBID()
#endif /* SLEEP_CAMERA_OKTH_SIG */

/* Sleeping stops TCXO clock, hence all clocks. */
#ifdef FEATURE_NEW_SLEEP_API 
#define CAMERA_SLEEP_ALLOW() \
  { sleep_assert_okts(camera_sleep_okts_handle); }
#define CAMERA_SLEEP_FORBID() \
  { sleep_negate_okts(camera_sleep_okts_handle); }
#elif SLEEP_CAMERA_OKTS_SIG
#define CAMERA_SLEEP_ALLOW() \
  { (void) rex_set_sigs( &sleep_tcb, SLEEP_CAMERA_OKTS_SIG); }
#define CAMERA_SLEEP_FORBID() \
  { (void) rex_clr_sigs( &sleep_tcb, SLEEP_CAMERA_OKTS_SIG); }
#else /* SLEEP_CAMERA_OKTS_SIG */
#define CAMERA_SLEEP_ALLOW() \
  { camera_ok_to_sleep = TRUE; }
#define CAMERA_SLEEP_FORBID() \
  { camera_ok_to_sleep = FALSE; }
#endif /* SLEEP_CAMERA_OKTS_SIG */


#ifndef TIMETEST
/* #define CAMERA_TIMETEST
   extern boolean logic_timetest = FALSE; */
#endif /* TIMETEST */

#ifdef CAMERA_TIMETEST
#error code not present
#endif /* CAMERA_TIMETEST */

/*============================================================================
*                         GLOBAL VARIABLE DECLARATIONS
============================================================================*/
extern camera_dsp_state_type camera_dsp_state;
extern boolean camera_stopping;
extern boolean camera_stopping_record;
extern boolean               camera_update_vfe;

#ifdef FEATURE_QVPHONE
extern boolean camera_qvp_enabled;
#endif

extern boolean camera_odd_encode_rotation;

/* camera_dx and camera_dy are in sensor orientation */
extern uint16 camera_dx;
extern uint16 camera_dy;
/* camera_preview_dx and camera_preview_dy specifies the preview
 * display dimensions. They are always in landscape */
extern uint32 camera_preview_dx;
extern uint32 camera_preview_dy;

extern boolean camera_take_images;
extern VFE_EOF_Ack2Type  ackOutput2;

extern boolean camera_blt_ext_recalculate_window;

extern camera_func_type camera_func;
extern camera_cb_f_type camera_app_cb;
extern void             *camera_app_data;
extern camsensor_op_mode_type      camera_camsensor_op_mode;

#ifdef FEATURE_CAMERA_MULTI_SENSOR
extern uint16           camera_number_of_sensors;
extern camera_info_type camera_sensors[];
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

/*============================================================================
*                         GLOBAL FUNCTION DECLARATIONS
============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_SVCS_INIT_SELF

DESCRIPTION
              Initialize function pointer table for camera application
              specific routines interfaced from the generalized camera
              services API.

RETURN VALUE
   None.

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_init_self (camerai_func_type *funcTable);

void camera_dsp_error_status_set_default(void);
void camera_qdsp_cb (CAMQDSP_ResponseType *response);

/*===========================================================================

FUNCTION      CAMERA_PROCESS_FUNC

DESCRIPTION
              Processes function call in graph task context.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_process_func
(
  camera_func_type function,
  void  *callback,
  void  *client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
);

boolean camera_config_vfe(void);
void camera_initiate_capture (void);
void camera_reject
(
  camera_cb_f_type callback,
  void * client_data,
  camera_func_type func
);

void camera_terminate (camera_cb_type cb, int32 parm4);
boolean camera_set_preview_buffers (void);
void camera_set_preview_headers (void);

#ifdef FEATURE_MDP
void  camera_deregister_video (boolean);
void  camera_deregister_layer1 (void);
#endif /* FEATURE_MDP */

#ifdef  FEATURE_CAMERA_MOBICAT_PREVIEW
boolean camera_svcs_start_mobicat_preview(uint32 mode, char*);
#endif  //FEATURE_CAMERA_MOBICAT_PREVIEW

/* This function is used by the camcorder_svcs file */
boolean camera_send_vfe_idle_and_wait(void);

#endif /* FEATURE_CAMERA */
#endif

