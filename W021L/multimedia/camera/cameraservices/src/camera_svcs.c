/* =========================================================================
                 Camera Interface

   DESCRIPTIONFileSourceDevice Driver
     This file contains the definitions needed for the camera interface
     device driver (LDD).

   Design Note:
     All function calls are executed from the caller's context. The graph
     task context is only used in lieu of interrupt context. All state
     transition (except change to sensor ready which does not have racing
     issue) happens in the caller's context.

     Since state and callbacks are used in graph task context, these two
     variables are protected by disabling the interrupt.

   Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameraservices/main/latest/src/camera_svcs.c#58 $ $DateTime: 2009/04/22 23:29:28 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/22/09   cg     Enabled ASF for Viewfinder for LCU.
============================================================================*/
/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "assert.h"
#include "camera.h"
#include "camerai.h"
#include "camera_svcs.h"
#include "mobile.h"
#ifdef FEATURE_PNG_ENCODER
#include "pnge.h"
#endif /* FEATURE_PNG_ENCODER */
#ifdef FEATURE_CAMERA
#include CLKRGM_H
#include "bio.h"
#include "msm.h"
#include "msmhwio.h"
#include "camsensor.h"
#include "camctrl.h"
#include "disp.h"
#include "event.h"
#include "graphi.h"
#include "graph.h"
#include "ipl.h"
#include "msg.h"
#include <stdlib.h>
#ifdef CAMERA_USES_LIBSTD
#include "AEEstd.h"
#endif /* CAMERA_USES_LIBSTD */
#include "task.h"
#include "qcamraw.h"
#include "OEMFeatures.h"
#include "softdsp.h"
#include "camera_drv.h"


/* To obtain hardware version (chip id) or
 * partnum version (for Jaguar/Saber)
 */
#include "hw.h"
#define MSG_CAMERA_LATENCY_PROFILING(x,a,b,c) MSG_HIGH(x,a,b,c)

/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/
#define MSG_CAMERAPBUF(x,a,b,c) MSG_LOW(x,a,b,c)
#define CAMERA_MIN_CONTRAST    0
#define CAMERA_DEF_CONTRAST    5
#define CAMERA_MAX_CONTRAST    10


#define CAMERA_MIN_BRIGHTNESS 0
#define CAMERA_DEF_BRIGHTNESS 5
#define CAMERA_MAX_BRIGHTNESS 10

#define CEILING2(X)  (((X) + 0x0001) & 0xFFFE)
#define FLOOR16(X) ((X) & 0xFFF0)
/*
#define FLOOR8(X)  ((X) & 0xFFF8)
*/
#define FLOOR4(X)  ((X) & 0xFFFC)
#define FLOOR2(X)  ((X) & 0xFFFE)

/* LCD specific constants */

/* For QVGA LCD in portrait mode */
/* Can be overwritten by custcamera.h file if defined there */
#ifndef CAMERA_LCD_WIDTH
#define CAMERA_LCD_WIDTH          240
#endif /* nCAMERA_LCD_WIDTH */
#ifndef CAMERA_LCD_HEIGHT
#define CAMERA_LCD_HEIGHT         320
#endif /* nCAMERA_LCD_HEIGHT */

#define CAMERA_QVGA_WIDTH          320
#define CAMERA_QVGA_HEIGHT         240

#ifdef CAMERA_USES_OEM_HEAP
#define CAM_ALLOC_NO_ZMEM (0x0)
#else
#define CAM_ALLOC_NO_ZMEM (0x80000001)
#endif

#define CAMERA_BUF_ALIGN 64

#ifdef FEATURE_NEW_SLEEP_API
sleep_okts_handle camera_sleep_okts_handle;
boolean camera_sleep_handle_register=FALSE;
#endif

/*============================================================================
                        LOCAL DATA TYPES
============================================================================*/
/* Delay AEC/AWB stats */
#define UPDATE_CMPLT_STATS_DELAY 0
#define CAMERA_PIXELS_FOR_DEMOSAIC 12
#define CAMERA_LINES_FOR_DEMOSAIC 8

/*============================================================================
                     GLOBAL DATA DEFINITIONS
============================================================================*/

camera_frame_type          snapshot_frame;
camera_preview_mode_type   preview_mode;
boolean                    camera_ok_to_sleep;

/*lint -save */
/*lint -e46 uint32 as bit field */
/*lint -e64 Type mismatch */
/*lint -e413 offset of macro measure offset from start of struct */
/*lint -e611 cast to generic callback funtion pointers */
/*lint -e641 conversion xxx to int */
/*lint -e701 value are defined such as upper bit 16 | lower 16 bit */
/*lint -e702 10 bit black level to 8 bit */
/*lint -e704 shift right ok */
/*lint -e818 Could be pointer to const */
/*lint -e825 control flows into case/default without -fallthrough comment */
/*lint -e826 Suspicious pointer-to-pointer conversion (area too small) */
/*lint -e834 Operator '?' followed by operator '?' is confusing */

/*============================================================================
                     LOCAL DATA DEFINITIONS
============================================================================*/
/* The BLT window need to be calculated only when preview,
 * start record or take picture started */
boolean camera_blt_ext_recalculate_window = TRUE;
static CAMSoftDSP_IllegalCommandMessageType illegalCmdMessage;

typedef enum
{
  CAMERA_TERMINATE_CLOCK_STOPPED,
  CAMERA_TERMINATE_CLOCK_RUNNING,
  CAMERA_TERMINATE_CLOCK_EXPIRED
} camera_terminate_clk_state_type;

static camera_terminate_clk_state_type camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_STOPPED;
static clk_cb_type camera_terminate_clk;
static camera_handle_type                camera_handle;
static boolean camera_initialized = FALSE;
static boolean camera_timer_up    = FALSE;

/* A clock for timed wait */
static clk_cb_type camera_wait_clk;
static rex_crit_sect_type camera_wait_clk_cs;
static rex_tcb_type *camera_timed_wait_tcb;
static disp_info_type        camera_disp_info;
camera_state_type            camera_state  = CAMERA_STATE_INIT;
camera_cb_f_type      camera_app_cb = NULL;
static rex_tcb_type          *camera_tcb = NULL;

// this struct holds the min and max points in the zoom table
// we want to travel between (inclusive) to get our zoom factor
static camera_parm_info_type camera_parm_fps;
static camera_parm_info_type camera_parm_reflect;
static camera_parm_info_type camera_parm_preview_mode;
static camera_parm_info_type camera_parm_antibanding;
static camera_parm_info_type camera_parm_brightness;
static camera_parm_info_type camera_parm_contrast;
static camera_parm_info_type camera_parm_special_effects;
static camera_parm_info_type camera_parm_white_balance;
static camera_parm_info_type camera_parm_auto_exposure_mode;
static camera_parm_info_type camera_parm_exposure_compensation;
static camera_parm_info_type camera_parm_preview_fps;

/* camera_dx and camera_dy are in sensor orientation */
uint16               camera_dx = 0;
uint16               camera_dy = 0;

static camera_position_type camera_position;

static ipl_reflect_type        camera_reflect_ipl;
static qcamrawReflectType      camera_reflect;

static uint32 offline_read_wait = 0;
static uint32  camera_preview_buffer_size = 0;
static uint32  camera_encode_buffer_size  = 0;

/* New preview buffer scheme */
#define CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE          SOFTDSP_PREVIEW_BUFF_MAX
#define CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY      1
#define CAMERA_NUM_OF_PREVIEW_BUFFERS \
   (CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE + \
    CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY)

typedef enum
{
  AVAILABLE = 0,
  /* Means buffer is delivered to application. */
  BUSY_WITH_APP,
  /* Means buffer in the queue to be displayed */
  BUSY_WITH_DISPLAY,
  /* Means buffer sent to VFE */
  BUSY_WITH_VFE
} camera_preview_buffer_status_type;

typedef struct
{
  byte                               *buf_ptr;
  camera_preview_buffer_status_type  status;
} camera_preview_buffer_type;

typedef struct
{
  camera_preview_buffer_type      buffers[CAMERA_NUM_OF_PREVIEW_BUFFERS];
  /* Incremented when buffer is delivered to APP, decremented when
   * become available, either in BUSY_WITH_APP or BUSY_WITH_DISPLAY */
  uint32                          num_buffers_with_display;
  /* Incremented when buffer is delivered to VFE. Decremented when
   * it is received from VFE */
  uint32                          num_buffers_with_VFE;
} camera_preview_buffers_info_type;

static camera_preview_buffers_info_type      camera_preview_buffers;

#define CAMERA_INVALID_BUF_INDEX 10
static uint32 camera_app_buf_index[2];

static uint32 camera_capture_delay = 0; /* in ms */
static uint8 *snapshot_luma_buf = NULL;

#ifdef FEATURE_PNG_ENCODER
static qcamrawHeaderType my_png_image_header;
#endif /* FEATURE_PNG_ENCODER */

static camera_parm_type  camera_parm_id;
static camera_encode_properties_type camera_encode_properties;

camera_func_type   camera_func = CAMERA_FUNC_MAX;
/* camera_preview_dx and camera_preview_dy specifies the preview
 * display dimensions. They are always in landscape */
uint32             camera_preview_dx   = 0;
uint32             camera_preview_dy   = 0;

void             *camera_app_data = 0;
static boolean           camera_frame_callback_enabled = TRUE;
static boolean           camera_abort_picture = FALSE;
camsensor_op_mode_type      camera_camsensor_op_mode;
static boolean           scale_preview_overlay = FALSE;
static uint32  efs_fail_count = 0;

/********************************************************/

static uint32 qcamraw_header_size = 0;
boolean camera_stopping = FALSE;
/* Store Downscaler factor for Preview and Snasphot */
static camera_cb_type        camera_stopping_cb;

static fs_handle_type  camera_fs_handle = NULL;
static fs_rsp_msg_type fs_rsp;

#define NUM_OF_SENSORS_ON_PHONE  2

#ifdef FEATURE_CAMERA_MULTI_SENSOR
uint16 camera_number_of_sensors = 0;
/* camera_sensors[] is to allow the application to get sensor info
 * before camera_start() gets called. */
camera_info_type camera_sensors[NUM_OF_SENSORS_ON_PHONE] = {0};
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

uint8 camera_asi = 0;
// May need an array of these
camsensor_static_params_type camsensor_static_params[NUM_OF_SENSORS_ON_PHONE];

/* VFE output1 format for preview */
static camera_format_type camera_preview_format = CAMERA_RGB565;

/* VFE output1 and output2 format for snapshot. VFE doesn't
 * allow to specify different formats for output1 and output2.
 */
static camera_format_type camera_snapshot_format = CAMERA_RGB565;

static camera_ret_code_type camera_svcs_blt_ex
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void               *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy,
  camera_format_type format
);

#ifdef FEATURE_PNG_ENCODER
/* code for the PNG encoder */
static PNGE_spec_type   png_encode_spec;
static pnge_handle_type png_handle;
#endif /* FEATURE_PNG_ENCODER */

/* Set to true when a particular stats command is requested,
   Specific bit is cleared to false when that stats command is received */
int32 camera_pending_stats_cmds = NO_PENDING_AF_CMDS_TO_ISSUE;

/*  Following structure definition is used to copy/move data to
 *  camera_parm_info_type structure. Since camera_parm_info_type
 *  does not allow to transfer floats. This internal structure will
 *  store the meaning of the data that is being transfered.
 */
typedef struct
{
  int16  minimum_numerator_value;
  uint16 minimum_denominator_value;
  int16  maximum_numerator_value;
  uint16 maximum_denominator_value;
  int16  step_numerator_value;
  uint16 step_denominator_value;
  int16  default_numerator_value;
  uint16 default_denominator_value;
  uint32 value_to_get_index;
  float * ev_ptr;      /* EV Compensation table */
} camera_exposure_compensation_type;

/* ----------------------------------------------------------------------------
   Formula to calucalte the new EV target using adjustment step (EV_Comp) and
   the default value.
           EV_Comp
   Enew = 2        * EVdefault.

   We are using the adjustment step of 1/6, the following table contains the
    EV_comp
   2       value at different EV_Comp step.

   Enum camera_ev_compensation_type is used as an index to get the correct
   multiplying factor from this table.
---------------------------------------------------------------------------- */
float camera_ev_compensation_one_over_six_table[] =
{
    0.2500,    /* 2^EV_Comp = 2^-12/6 */
    0.2806,    /* 2^EV_Comp = 2^-11/6 */
    0.3150,    /* 2^EV_Comp = 2^-10/6 */
    0.3536,    /* 2^EV_Comp = 2^-9/6  */
    0.3969,    /* 2^EV_Comp = 2^-8/6  */
    0.4454,    /* 2^EV_Comp = 2^-7/6  */
    0.5000,    /* 2^EV_Comp = 2^-6/6  */
    0.5612,    /* 2^EV_Comp = 2^-5/6  */
    0.6299,    /* 2^EV_Comp = 2^-4/6  */
    0.7071,    /* 2^EV_Comp = 2^-3/6  */
    0.7937,    /* 2^EV_Comp = 2^-2/6  */
    0.8909,    /* 2^EV_Comp = 2^-1/6  */
    1.0000,    /* 2^EV_Comp = 2^0     */
    1.1225,    /* 2^EV_Comp = 2^1/6   */
    1.2599,    /* 2^EV_Comp = 2^2/6   */
    1.4142,    /* 2^EV_Comp = 2^3/6   */
    1.5874,    /* 2^EV_Comp = 2^4/6   */
    1.7818,    /* 2^EV_Comp = 2^5/6   */
    2.0000,    /* 2^EV_Comp = 2^6/6   */
    2.2449,    /* 2^EV_Comp = 2^7/6   */
    2.5198,    /* 2^EV_Comp = 2^8/6   */
    2.8284,    /* 2^EV_Comp = 2^9/6   */
    3.1748,    /* 2^EV_Comp = 2^10/6  */
    3.5636,    /* 2^EV_Comp = 2^11/6  */
    4.0000     /* 2^EV_Comp = 2^12/6  */
};

static camera_exposure_compensation_type camera_ev_compensation_table=
{
  -12, 6,      /* Minimun value  Numerator,Denominator  */
   12, 6,      /* Maximum value  Numerator,Denominator  */
   1, 6,       /* Step Value     Numerator,Denominator  */
   0, 6,       /* Default value  Numerator,Denominator  */
   12,         /* Value to convert fraction to index    */
   camera_ev_compensation_one_over_six_table,
};

static Camera_EndOfFrameMessageType      endOfFrameMessage[2];
static int32 camera_parm4_value = 0;

typedef struct
{
  boolean  abort;                    /* This will tell whether we need to abort the operation   */
  boolean encode_in_progress;
} camera_take_picture_status_type;

static camera_take_picture_status_type camera_take_picture_status;

typedef enum
{
  CAMERA_TAKE_PICTURE_RETRY_NONE,
  CAMERA_TAKE_PICTURE_RETRY_NEEDED, /* AHB overflow detected */
  CAMERA_TAKE_PICTURE_RETRY_IDLING, /* sending VFE to IDLE   */
  CAMERA_TAKE_PICTURE_RETRY_INP,    /* retry capturing       */
  CAMERA_TAKE_PICTURE_RETRY_DONE    /* picture received      */
} camera_take_picture_retry_state_type;
static camera_take_picture_retry_state_type camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_NONE;

camera_snapshot_timing_type camera_snapshot_timing;
camera_snapshot_information_type camera_snapshot_information;

#if ( defined (FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III)||defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE || \
     defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_II)||defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV && \
    (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
     defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)

typedef enum
{
  CAMERA_CLK_RESOURCE_VIEWFINDER = 0,
  CAMERA_CLK_RESOURCE_SNAPSHOT,
  CAMERA_CLK_RESOURCE_MAX
} camera_clk_resource_type;

#ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE /* for 6280, the default clock is low at 122/61MHz*/
#define CAMERA_CLK_RESOURCE_DEFAULT CAMERA_CLK_RESOURCE_VIEWFINDER
#define CAMERA_SET_CLK_VIEWFINDER()      {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}
/* Need to provide _HIGH() because if in case the pclk is higher than 61MHz, we need higher Hclk for 6280*/
#define CAMERA_SET_CLK_VIEWFINDER_HIGH() {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}
#ifdef QDSP_MODULE_VFE20_DEFINED /*6290 target */
#define CAMERA_SET_CLK_SNAPSHOT()        {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_HIGH);}
#define CAMERA_SET_CLK_HIGH_DEFAULT()     {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_HIGH);} 
#else
#define CAMERA_SET_CLK_SNAPSHOT()        {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}
#endif /*QDSP_MODULE_VFE20_DEFINED*/
#define CAMERA_SET_CLK_DEFAULT()         {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_MED);}

#elif defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
#define CAMERA_CLK_RESOURCE_DEFAULT CAMERA_CLK_RESOURCE_SNAPSHOT
#define CAMERA_SET_CLK_VIEWFINDER()       {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER);}

#define CAMERA_SET_CLK_VIEWFINDER_HIGH()  {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_MAX);}
#define CAMERA_SET_CLK_SNAPSHOT()         {clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_MAX);}
#define CAMERA_SET_CLK_DEFAULT()          {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_QCAMERA_VIEWFINDER);}
#define CAMERA_SET_CLK_HIGH_DEFAULT()     {clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_MAX);} 

#elif defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV /* for LCU(6270/6240 default clock is LOW at 122/61 Mhz) */
#error code not present
#else /* for 6800, the default clock is high, at 268/89MHz*/
#define CAMERA_CLK_RESOURCE_DEFAULT CAMERA_CLK_RESOURCE_SNAPSHOT
#define CAMERA_SET_CLK_VIEWFINDER()      {clk_regime_register_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
/* in 6800, low hclk is 65MHz, the max pclk supported is 64MHz, so no need for higher hclk, just for compatibility*/
#define CAMERA_SET_CLK_VIEWFINDER_HIGH() {clk_regime_register_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
#define CAMERA_SET_CLK_SNAPSHOT()        {clk_regime_deregister_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
#define CAMERA_SET_CLK_DEFAULT()         {clk_regime_deregister_for_cpu_resource(CLKRGM_QCAM_VIEWFINDER);}
#endif /* FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE*/
static camera_clk_resource_type camera_clk_resource = CAMERA_CLK_RESOURCE_DEFAULT;
#endif /* defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE &&    \
          (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING || \
           defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) */
/*============================================================================
                     LOCAL FUNCTIONS DECLARATIONS
============================================================================*/

static void camera_svcs_queue_call_to_terminate(void);
static void camera_process_terminate(void);
static void camera_terminate_clk_cb (int4 ms);
static void camera_preview_set_buffer_status(uint8 buffer_index, camera_preview_buffer_status_type new_status);
extern void exif_set_date_time (uint8 *ptr);
void camera_reject (camera_cb_f_type callback, void * client_data,
                           camera_func_type func);
static void camera_process_start ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb, camera_ret_code_type *status);
static void camera_process_stop ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb);
static void camera_process_cleanup
(
  rex_tcb_type    *tcb
);
static void camera_process_start_preview ( camera_cb_f_type callback, void *client_data);
static void camera_process_stop_preview ( rex_tcb_type *tcb );
static void camera_process_take_picture (
  camera_cb_f_type callback,
  void *client_data
  );

static void camera_process_release_frame (uint8 buffer_index);
static void camera_process_blt (uint8 buffer_index);
static void camera_process_encode_picture ( camera_cb_f_type callback, void *client_data, qcamrawHeaderType *main_image_header, byte *main_image_buffer, qcamrawHeaderType *thumbnail_image_header, byte *thumbnail_image_buffer);
static camera_ret_code_type camera_encode_picture_common ( camera_frame_type *frame, camera_handle_type *handle, camera_cb_f_type callback, void * client_data);
static void camera_process_enable_frame_callback ( void );
static void camera_process_disable_frame_callback ( void );
static void camera_process_set_overlay ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb);
static void camera_process_clr_overlay ( camera_cb_f_type callback, void *client_data);
static void camera_process_set_reflect (void);
static void camera_process_set_dimensions ( camera_cb_f_type callback, void *client_data,rex_tcb_type *info1);

void camera_terminate (camera_cb_type cb, int32 parm4);
static void  camera_process_softdsp_output1_msg (Camera_EndOfFrameMessageType *msg);
static void  camera_process_softdsp_output2_msg (Camera_EndOfFrameMessageType *msg);
static void camera_process_softdsp_illegal_cmd_msg (void);
static void camera_wait_clk_cb (int4 ms);
boolean camera_set_preview_buffers (void);
void camera_set_preview_headers (void);
void camera_softdsp_cb (CAMSoftDSP_ResponseType *response);
static void camera_svcs_swap_dimensions (uint16 *x, uint16 *y);

#ifdef FEATURE_PNG_ENCODER
/* for PNG encoder */
static void camera_PNG_CB (PNGE_status_enum_type command, void *handle, void *pnge_data);
#endif /* FEATURE_PNG_ENCODER */

static void camera_clr_take_picture (void);
static void camera_process_handle_error (void);
static void camera_handle_frames_for_takepicture(void);
static void camera_process_take_single_picture(void);
static boolean camera_abort_takepicture_encoding_operation(void);
static void camera_svcs_optimize_resources (void);


/* Applies the overlay on the main image and thumbnail */
static camera_cb_type camera_svcs_translate_ret_val_to_cb
(
 camera_ret_code_type ret_val
);

/*===========================================================================

FUNCTION      CAMERA_TAKE_PICTURE_STATUS_SET_DEFAULT

DESCRIPTION
              Init the structure.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_take_picture_status_set_default()
{
  camera_take_picture_status.abort                    = FALSE;
  camera_take_picture_status.encode_in_progress       = FALSE;
}

/*===========================================================================

FUNCTION      CAMERA_FREE_PREVIEW_BUFFERS

DESCRIPTION
              Free preview buffers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_free_preview_buffers()
{
  uint32 current_buffer;
  MSG_CAMERADEBUG("CAMERA_SVCS: Freeing Preview Buffers", 0, 0, 0);
  /* Free all buffers */
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    CAM_Free (camera_preview_buffers.buffers[current_buffer].buf_ptr);
    camera_preview_buffers.buffers[current_buffer].buf_ptr = NULL;
    camera_preview_buffers.buffers[current_buffer].status  = AVAILABLE;
  }
  camera_preview_buffers.num_buffers_with_display = 0;
  camera_preview_buffers.num_buffers_with_VFE = 0;
  camera_app_buf_index[0] = CAMERA_INVALID_BUF_INDEX;
  camera_app_buf_index[1] = CAMERA_INVALID_BUF_INDEX;
}

/*===========================================================================

FUNCTION      CAMERA_RESET_PREVIEW_BUFFERS

DESCRIPTION
              Reset the preview buffer status.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_reset_preview_buffers(void)
{
  uint32 current_buffer;

  MSG_CAMERAPBUF("PBUF Reset buffer status.", 0, 0, 0);
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    if (camera_preview_buffers.buffers[current_buffer].buf_ptr)
    {
      camera_preview_buffers.buffers[current_buffer].status = AVAILABLE;
    }
  }
  camera_preview_buffers.num_buffers_with_display = 0;
  camera_preview_buffers.num_buffers_with_VFE = 0;
  camera_app_buf_index[0] = CAMERA_INVALID_BUF_INDEX;
  camera_app_buf_index[1] = CAMERA_INVALID_BUF_INDEX;
}

/*===========================================================================

FUNCTION      CAMERA_MALLOC_PREVIEW_BUFFERS

DESCRIPTION
              Malloc preview buffers.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_malloc_preview_buffers()
{
  uint32 current_buffer;
  MSG_CAMERAPBUF("PBUF ALLOC Preview Buffers", 0, 0, 0);
  /* Allocate all buffers */
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    camera_preview_buffers.buffers[current_buffer].buf_ptr =
      CAM_Malloc (camera_preview_buffer_size | CAM_ALLOC_NO_ZMEM);
    MSG_CAMERADEBUG("camera_malloc_preview_buffers 0x%x 0x%x", current_buffer, camera_preview_buffers.buffers[current_buffer].buf_ptr, 0);
  }
  camera_reset_preview_buffers();
}

/*===========================================================================

FUNCTION      CAMERA_INIT

DESCRIPTION
              Used to initialize the device at power up. To be called
              from startup routine such as mc_device_init().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_init(void)
{
#ifndef T_IO_CARD
  if (camera_initialized)
  {
    return;
  }

  camera_initialized = TRUE;

#ifdef FEATURE_NEW_SLEEP_API
  if(!camera_sleep_handle_register)
  {
    camera_sleep_okts_handle = sleep_register("CAMERA", TRUE);
    camera_sleep_handle_register = TRUE;

#ifdef CLK_REGIME_UP_SLEEP__MCLK_HCLK
    sleep_set_hclk_restriction (camera_sleep_okts_handle, CLKRGM_HCLK_NO_SWITCH);
    sleep_set_mclk_restriction (camera_sleep_okts_handle, CLKRGM_MCLK_ON);
#endif /* CLK_REGIME_UP_SLEEP__MCLK_HCLK */

#ifndef FEATURE_ARM_HALT_CONTROL_CAMERA_OKTH_SIG
    sleep_set_swfi_restriction(camera_sleep_okts_handle, SLEEP_SWFI_RESTRICT_ON);
#endif /* FEATURE_ARM_HALT_CONTROL_CAMERA_OKTH_SIG */


  }
#endif

  /* Don't need gpios and clk regimes yet */
  camsensor_unconfig_sensor_gpios_clks ();

  clk_def (&camera_wait_clk);
  rex_init_crit_sect(&camera_wait_clk_cs);
  clk_def (&camera_terminate_clk);

  /* The following sequence gets the camera initialized and then
   * put in power down mode. We cannot leave camera in power up state
   * until used.
   * Also, camsensor_gp_clk_hz is set during power_up, but is used at power_on.
   * This seemingly dummy sequence actually get the right clock rate set
   * when the application is started. */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  for (camera_asi = 0; camera_asi < NUM_OF_SENSORS_ON_PHONE; camera_asi++)
#endif /* FEATURE_CAMERA_MULTI_SENSOR */
  {
  camsensor_power_on  ();
  camsensor_power_up  ();

#ifdef FEATURE_CAMERA_MULTI_SENSOR
    /* Assign this sensor the available ID */
    camera_sensors[camera_asi].sensor_id = (uint16)camera_asi;

    if (camsensor_initialized == TRUE)
    {
      /* Sensor responded as expected during initization. */
      camera_sensors[camera_asi].sensor_available = TRUE;

      /* Don't set camera_sensors[camera_asi].sensor_width/height
       * here because camsensor_start() (called in camera_start())
       * change it based on the specific sensor selected. */

      /* Set the fps to the maximum supported by this sensor */
      camera_sensors[camera_asi].fps = 15 * Q8;

      /* Whether the device driver can sense when sensor is rotated */
      camera_sensors[camera_asi].sensor_rotation_sensing = FALSE;

      /* How the sensor are installed */
      camera_sensors[camera_asi].default_rotation = 0;
      camera_sensors[camera_asi].default_orientation = 0;

      camera_number_of_sensors++;
    }
    else
    {
      camera_sensors[camera_asi].sensor_available = FALSE;
    }
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

  camsensor_power_down();
  camsensor_power_off ();
  }
#ifdef FEATURE_CAMERA_MULTI_SENSOR
  camera_asi = 0; /* select the primary sensor */
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

  CAMERA_SLEEP_ALLOW ();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_OFF();

  camera_disp_info = disp_get_info ();
  if (camera_disp_info.disp_width == 0 || camera_disp_info.disp_height == 0)
  {
    camera_disp_info.disp_width = CAMERA_LCD_WIDTH;
    camera_disp_info.disp_height = CAMERA_LCD_HEIGHT;
  }

  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_INIT", 0, 0, 0);
  camera_state = CAMERA_STATE_INIT;
  camera_app_cb    = NULL;
  
  camera_parm_reflect.minimum_value = (int)CAMERA_NO_REFLECT;
  camera_parm_reflect.maximum_value = (int)CAMERA_MAX_REFLECT - 1;
  camera_parm_reflect.default_value = (int)CAMERA_NO_REFLECT;
  camera_parm_reflect.current_value = (int)CAMERA_NO_REFLECT;
  camera_parm_reflect.step_value    = 1;
  preview_mode         = CAMERA_PREVIEW_MODE_SNAPSHOT;

  camera_parm_preview_mode.minimum_value = 0;
  camera_parm_preview_mode.maximum_value = (int)CAMERA_MAX_PREVIEW_MODE - 1;
  camera_parm_preview_mode.default_value = (int)preview_mode;
  camera_parm_preview_mode.current_value = (int)preview_mode;
  camera_parm_preview_mode.step_value    = 1;

  camera_parm_antibanding.minimum_value = 0;
  camera_parm_antibanding.maximum_value = (int)CAMERA_MAX_ANTIBANDING - 1;
  camera_parm_antibanding.default_value = (int)CAMERA_ANTIBANDING_OFF;
  camera_parm_antibanding.current_value = (int)CAMERA_ANTIBANDING_OFF;
  camera_parm_antibanding.step_value    = 1;
  camera_parm_brightness.minimum_value = CAMERA_MIN_BRIGHTNESS;
  camera_parm_brightness.maximum_value = CAMERA_MAX_BRIGHTNESS;
  camera_parm_brightness.default_value = CAMERA_DEF_BRIGHTNESS;
  camera_parm_brightness.current_value = CAMERA_DEF_BRIGHTNESS;
  camera_parm_brightness.step_value    = 1;

  camera_parm_contrast.minimum_value = CAMERA_MIN_CONTRAST;
  camera_parm_contrast.maximum_value = CAMERA_MAX_CONTRAST;
  camera_parm_contrast.default_value = CAMERA_DEF_CONTRAST;
  camera_parm_contrast.current_value = CAMERA_DEF_CONTRAST;
  camera_parm_contrast.step_value    = 1;

  camera_parm_special_effects.minimum_value = (int)CAMERA_EFFECT_MIN_MINUS_1 +1;
  camera_parm_special_effects.maximum_value = (int)CAMERA_EFFECT_MAX_PLUS_1  -1;
  camera_parm_special_effects.default_value = (int)CAMERA_EFFECT_OFF;
  camera_parm_special_effects.current_value = (int)CAMERA_EFFECT_OFF;
  camera_parm_special_effects.step_value    = 1;

  camera_parm_white_balance.minimum_value = (int)CAMERA_WB_MIN_MINUS_1    +1;
  camera_parm_white_balance.maximum_value = (int)CAMERA_EFFECT_MAX_PLUS_1 -1;
  camera_parm_white_balance.default_value = (int)CAMERA_WB_AUTO;
  camera_parm_white_balance.current_value = (int)CAMERA_WB_AUTO;
  camera_parm_white_balance.step_value    = 1;

  camera_parm_auto_exposure_mode.minimum_value = (int)CAMERA_AEC_FRAME_AVERAGE;
  camera_parm_auto_exposure_mode.maximum_value = (int)CAMERA_AEC_MAX_MODES-1;
  camera_parm_auto_exposure_mode.default_value = (int)CAMERA_AEC_CENTER_WEIGHTED;
  camera_parm_auto_exposure_mode.current_value = (int)CAMERA_AEC_CENTER_WEIGHTED;
  camera_parm_auto_exposure_mode.step_value    = 1;

  camera_parm_exposure_compensation.minimum_value = ( camera_ev_compensation_table.minimum_numerator_value << 16 |
                                                      camera_ev_compensation_table.minimum_denominator_value
                                                    );
  camera_parm_exposure_compensation.maximum_value = ( camera_ev_compensation_table.maximum_numerator_value << 16 |
                                                      camera_ev_compensation_table.maximum_denominator_value
                                                    );
  camera_parm_exposure_compensation.default_value = ( camera_ev_compensation_table.default_numerator_value << 16 |
                                                      camera_ev_compensation_table.default_denominator_value
                                                    );
  camera_parm_exposure_compensation.current_value = ( camera_ev_compensation_table.default_numerator_value << 16 |
                                                      camera_ev_compensation_table.default_denominator_value
                                                    );
  camera_parm_exposure_compensation.step_value    = ( camera_ev_compensation_table.step_numerator_value << 16 |
                                                      camera_ev_compensation_table.step_denominator_value
                                                    );
  
  qcamraw_header_size = qcamrawInitHeaderSize();
  /* Initialize the trigerring flags */
  SoftDSP_Init();
#endif /* nT_IO_CARD */
}

#ifdef FEATURE_CAMERA_MULTI_SENSOR
/*===========================================================================

FUNCTION      CAMERA_SELECT_SENSOR

DESCRIPTION
              Set camera_asi to sensor_id.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_select_sensor(uint16 sensor_id)
{
  int i;

#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_SELECT_SENSOR, 0, 0, __LINE__);
  MSG_MED("camera_select_sensor", 0, 0, 0);

  if (camera_state != CAMERA_STATE_INIT)
  {
    event_report(EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  /* Check to make sure we have information for the sensor being selected.
   * Otherwise, its an invalid sensor selection and leave camera_asi
   * as it was. */
  for (i = 0; i < NUM_OF_SENSORS_ON_PHONE; i++)
  {
    if ((camera_sensors[i].sensor_available == TRUE) &&
        (camera_sensors[i].sensor_id == sensor_id))
    {
      camera_asi = sensor_id;
      return CAMERA_SUCCESS;
    }
  }

  return CAMERA_NO_SENSOR;
#endif /* T_IO_CARD */
} /* camera_select_sensor */
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

/*===========================================================================

FUNCTION      CAMERA_START

DESCRIPTION
              Get the camera services started.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_start
(
  camera_cb_f_type callback,
  void *client_data
)
{

#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  rex_tcb_type *tcb;

  camera_ret_code_type camera_start_status = CAMERA_SUCCESS;

  camera_timer_up = TRUE;
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_START, 0, 0, __LINE__);
  MSG_HIGH("camera_start, camera_state %d", camera_state, 0, 0);
  
  if (camera_state != CAMERA_STATE_INIT)
  {
    if (callback)
    {
      (*callback) (CAMERA_EXIT_CB_FAILED, client_data, CAMERA_FUNC_START, 0);
    }
    event_report (EVENT_CAMERA_INVALID_STATE);
    MSG_ERROR("CAMERA_SVCS: Invalid state for camera_state %d", camera_state, 0, 0);
    return CAMERA_INVALID_STATE;
  }

  if (camera_stopping)
  {
    if (callback)
    {
      (*callback) (CAMERA_EXIT_CB_FAILED, client_data, CAMERA_FUNC_START, 0);
    }
    event_report (EVENT_CAMERA_INVALID_STATE);
    MSG_ERROR("CAMERA_SVCS: Invalid state for camera_stopping", 0, 0, 0);
    return CAMERA_INVALID_STATE;
  }

  tcb = rex_self();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32)CAMERA_FUNC_START, (void *)callback,
                           client_data, (uint32)tcb, (uint32)&camera_start_status, 0, 0);

  (void) rex_wait (CAMERA_SIG);
  (void) rex_clr_sigs (tcb, CAMERA_SIG);


  return camera_start_status;

#endif /* T_IO_CARD */
} /* camera_svcs_start */



/*===========================================================================

FUNCTION      CAMERA_STOP

DESCRIPTION
              The camera service is terminated and the sensor enters power
              down or saving mode.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful:
   CAMERA_EXIT_CB_DONE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_stop
(
  camera_cb_f_type callback,
  void *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  rex_tcb_type *tcb;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_STOP, 0, 0, __LINE__);
  MSG_HIGH ("camera_stop, camera_state:%d", camera_state, 0, 0);
  
  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_STOP);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_STOP);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
  
  tcb = rex_self ();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32)CAMERA_FUNC_STOP, (void *)callback,
                           client_data, (uint32)tcb, 0, 0, 0);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop", 0, 0, 0 );
  (void) rex_wait (CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop", 0, 0, 0 );
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32)CAMERA_FUNC_CLEANUP, (void *)callback,
                           client_data, (uint32)tcb, 0, 0, 0);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop for camera cleanup", 0, 0, 0 );
  (void) rex_wait (CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop after camera cleanup", 0, 0, 0 );
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_stop */



/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_DIMENSIONS

DESCRIPTION
              Set picture and display sizes.
              This function is executed in the caller's context.

DEPENDENCIES
  Must be in READY state.
  ui_picture_width, ui_picture_height are used for recording.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_dimensions
(
  uint16           ui_picture_width,  /* width of the picture  */
  uint16           ui_picture_height, /* height of the picture */
  uint16           display_width,     /* width of display      */
  uint16           display_height,    /* height of display */
  camera_cb_f_type callback,
  void             *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  rex_tcb_type *tcb;

  MSG_HIGH("camera_set_dimension, picture width %d, picture height %d, display width %d", ui_picture_width, ui_picture_height, display_width);
  if (camsensor_initialized == FALSE)
  {
    MSG_ERROR ("No sensor", 0, 0, 0);
    ret_code = CAMERA_NO_SENSOR;
    event_report (EVENT_CAMERA_NO_SENSOR);
  }
  else if ((camera_state != CAMERA_STATE_READY) &&
           (camera_state != CAMERA_STATE_INIT))
  {
    MSG_ERROR ("Invalid state", 0, 0, 0);
    event_report (EVENT_CAMERA_INVALID_STATE);
    ret_code = CAMERA_INVALID_STATE;
  }

  if ((display_width == 0) || (display_height == 0))
  {
    MSG_ERROR ("Format not supported", 0, 0, 0);
    event_report (EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }

  if ((ui_picture_width  == 0) || (ui_picture_height == 0))
  {
    MSG_ERROR ("Format not supported", 0, 0, 0);
    event_report (EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_DIMENSIONS);
    return ret_code;
  }

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_DIMENSIONS, 0, 0, __LINE__);

  /* Be smart about setting dimensions.  Note that internal buffer dimensions
     during preview/capture are ALWAYS landscape: so the larger UI dimension
     is the "width" and the smaller the height */
  if (ui_picture_width < ui_picture_height) /* Landscape from UI */
  {
    camera_svcs_swap_dimensions (&ui_picture_width, &ui_picture_height);
  }
  if (display_width < display_height)
  {
    camera_svcs_swap_dimensions (&display_width, &display_height);
  }

  /* Limiting the display width & display height to QVGA resolution */
  if (display_width > CAMERA_QVGA_WIDTH)
  {
    display_width = CAMERA_QVGA_WIDTH;
  }

  if (display_height > CAMERA_QVGA_HEIGHT)
  {
    display_height = CAMERA_QVGA_HEIGHT;
  }
  
  /* Set the image size for snapshot */
  camera_dx = ui_picture_width;
  camera_dy = ui_picture_height;

  /* set preview size */
  camera_preview_dx = display_width;
  camera_preview_dy = display_height;

  /* 4:2:0 = X * Y * 1.5
   * 4:2:2 = X * Y * 2
   * Set to max size. These are used for camcorder.
   */
  camera_preview_buffer_size = camera_preview_dx * (camera_preview_dy+2) * 2 + qcamraw_header_size;
  camera_encode_buffer_size  = camera_dx * (camera_dy+2) * 2 + qcamraw_header_size;
  
  tcb = rex_self();
  (void) rex_clr_sigs (tcb, CAMERA_SIG);

  graph_queue_camera_func ((uint32)CAMERA_FUNC_SET_DIMENSIONS,
                       (void *)callback, client_data, (uint32)tcb, 0, 0, 0);

  (void) rex_wait (CAMERA_SIG);
  (void) rex_clr_sigs (tcb, CAMERA_SIG);
  return ret_code;
#endif /* T_IO_CARD */
} /* camera_svcs_set_dimensions */


/*===========================================================================

FUNCTION      CAMERA_START_PREVIEW

DESCRIPTION
              Enters preview state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EVT_CB_FRAME when VFE frame is available.
   3. CAMERA_EXIT_CB_DONE if preview is terminated.
   4. CAMERA_EXIT_CB_ERROR if there is a resource error.
   5. CAMERA_EXIT_CB_ABORT.
   6. CAMERA_EXIT_CB_DSP_IDLE

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_start_preview
(
  camera_cb_f_type callback,
  void *client_data
)
{
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  camera_snapshot_timing.viewfinder_start = (uint32)timetick_get_ms();
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_START_PREVIEW, 0, 0, __LINE__);
  MSG_HIGH("camera_start_preview, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    /* Need to power on sensor, initialize it, power it up, before continue */
    MSG_HIGH("CAMERA_SVCS: Call POWER ON sequence in camera_svcs_start_preview", 0, 0, 0);
    camsensor_power_on();
    camsensor_power_up();
    MSG_CAMERADEBUG("CAMERA_SVCS: Configure GPIO settings", 0, 0, 0);
    camsensor_config_sensor_gpios_clks ();
  }

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_START_PREVIEW);
    return ret_code;
  }
  graph_queue_camera_func ((uint32)CAMERA_FUNC_START_PREVIEW,
                           (void *)callback, client_data, 0, 0, 0, 0);

  return ret_code;

#endif /* T_IO_CARD */
} /* camera_start_preview */


/*===========================================================================

FUNCTION      CAMERA_STOP_PREVIEW

DESCRIPTION
              Exit preview state.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PREVIEW state.
  Use start_preview callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE if preview is terminated.

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_svcs_stop_preview(void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  rex_tcb_type *tcb;

  camera_snapshot_timing.stop_viewfinder_start = (uint32)timetick_get_ms();

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_STOP_PREVIEW, 0, 0, __LINE__);
  MSG_HIGH ("camera_stop_preview",0,0,0);

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state != CAMERA_STATE_PREVIEW)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  /* Block caller until function is executed */
  tcb = rex_self();
  (void)rex_clr_sigs(tcb, CAMERA_SIG);
  graph_queue_camera_func ((uint32)CAMERA_FUNC_STOP_PREVIEW, (void *)0,
                           (void *)0, (uint32)tcb, 0, 0, 0);
  MSG_CAMERADEBUG("Waiting on CAMERA_SIG in camera_svcs_stop_preview", 0, 0, 0 );
  (void)rex_wait(CAMERA_SIG);
  MSG_CAMERADEBUG("Got CAMERA_SIG in camera_svcs_stop_preview", 0, 0, 0 );
  (void)rex_clr_sigs(tcb, CAMERA_SIG);
  return CAMERA_SUCCESS;

#endif /* T_IO_CARD */
} /* camera_stop_preview */


/*===========================================================================

FUNCTION      CAMERA_TAKE_PICTURE

DESCRIPTION
              The next frame is taken for encoding. Raw data is returned
              in CAMERA_EXIT_CB_DONE camera_frame_type.

              The picture buffer is allocated by the camera service, and must
              be released by the application via either camera_encode_picture
              or camera_release_picture.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is taken
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_svcs_take_picture
(
  camera_cb_f_type callback,
  void * client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

  camera_snapshot_timing.take_snapshot_start = (uint32)timetick_get_ms();

  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_TAKE_PICTURE, 0, 0, __LINE__);
  MSG_HIGH("camera_take_picture, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    ret_code = CAMERA_NO_SENSOR;
    event_report(EVENT_CAMERA_NO_SENSOR);
  }
  else if ((camera_dx == 0) || (camera_dy == 0))
  {
    MSG_ERROR("Format not supported", 0, 0, 0);
    event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }
  
  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject(callback, client_data, CAMERA_FUNC_TAKE_PICTURE);
    return ret_code;
  }
  
  camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_NONE;

  graph_queue_camera_func((uint32)CAMERA_FUNC_TAKE_PICTURE, (void *)callback, client_data, 0, 0, 0, 0);

  return ret_code;

#endif /* nT_IO_CARD */

} /* camera_take_picture */


/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE

DESCRIPTION
              Encode the application picture.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is encoded
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_svcs_encode_picture
(
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data
)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  camera_snapshot_information.encode_picture_width = (uint32) frame->captured_dx;
  camera_snapshot_information.encode_picture_height = (uint32) frame->captured_dy;

  camera_log(LOG_FUNC, (uint32)CAMERA_FUNC_ENCODE_PICTURE, 0, 0, __LINE__);
  MSG_HIGH("camera_encode_picture", 0, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    ret_val = CAMERA_NO_SENSOR;
    event_report(EVENT_CAMERA_NO_SENSOR);
  }
  else if (camera_state == CAMERA_STATE_INIT)
  {
    MSG_ERROR("Invalid state", 0, 0, 0);
    event_report(EVENT_CAMERA_INVALID_STATE);
    ret_val = CAMERA_INVALID_STATE;
  }

  if (ret_val == CAMERA_SUCCESS) {
    ret_val = camera_encode_picture_common( frame, handle, callback, client_data );
  }

  return ret_val;

} /* camera_encode_picture */


/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE_COMMON

DESCRIPTION
              Encode the application picture.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.
      then,
   2. CAMERA_EXIT_CB_DONE when picture is encoded
   3. CAMERA_EXIT_CB_ERROR if there is a resource error.
   4. CAMERA_EXIT_CB_DSP_IDLE when DSP becomes idle
   5. CAMERA_EXIT_CB_ABORT callback if DSP fatal error

SIDE EFFECTS
  None

===========================================================================*/

static camera_ret_code_type camera_encode_picture_common
(
  camera_frame_type *frame,
  camera_handle_type *handle,
  camera_cb_f_type callback,
  void * client_data
)
{
  camera_ret_code_type ret_code = CAMERA_SUCCESS;

#ifdef FEATURE_PNG_ENCODER
  qcamrawHeaderType *png_image_header;
  uint8  *png_image_buffer;
#endif /* FEATURE_PNG_ENCODER */

  camera_handle = *handle;

  if((camera_encode_properties.format != CAMERA_JPEG) &&
#ifdef FEATURE_PNG_ENCODER
           (camera_encode_properties.format != CAMERA_PNG)  &&
#endif /* FEATURE_PNG_ENCODER */
           (camera_encode_properties.format != CAMERA_RAW)  &&
           (camera_encode_properties.format != CAMERA_YCBCR_ENCODE))
  {
    MSG_ERROR("Invalid format", 0, 0, 0);
    event_report(EVENT_CAMERA_FORMAT_NOT_SUPPORTED);
    ret_code = CAMERA_INVALID_FORMAT;
  }

  if (ret_code != CAMERA_SUCCESS)
  {
    camera_reject(callback, client_data, CAMERA_FUNC_ENCODE_PICTURE);
    return ret_code;
  }
  
#ifdef FEATURE_PNG_ENCODER
  if (camera_encode_properties.format == CAMERA_PNG)
  {
      if (qcamrawIsHeader(frame->buffer))
      {
          png_image_header = (qcamrawHeaderType *) frame->buffer;
          png_image_buffer = frame->buffer + png_image_header->headerSize + png_image_header->padSize;

      }
      else
      {
          png_image_header      = &my_png_image_header;
          png_image_buffer      = frame->buffer;
      }

      graph_queue_camera_func((uint32)CAMERA_FUNC_ENCODE_PICTURE, (void *)callback, client_data, (uint32) png_image_header, (uint32) png_image_buffer, 0, 0);
  }
#endif /* FEATURE_PNG_ENCODER */
  return CAMERA_SUCCESS;
} /* camera_encode_picture_common */

/*===========================================================================

FUNCTION      CAMERA_SVCS_GET_THUMBNAIL_PROPERTIES

DESCRIPTION
              Get thumbnail size and quality

              This function is executed in the caller's context.

DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_get_thumbnail_properties
(
  uint32 *width,   /* width of the thumbnail */
  uint32 *height,  /* height of the thumbnail */
  uint32 *quality  /* encode quality of the thumbnail */
  /* When encoding, picture of this size and up must have thumbnail */
  //uint32 *picture_size
)
{
  return CAMERA_INVALID_PARM;
}


/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_THUMBNAIL_PROPERTIES

DESCRIPTION
              Set thumbnail size and quality

              This function is executed in the caller's context.

DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_thumbnail_properties
(
  uint32 width,   /* width of the thumbnail */
  uint32 height,  /* height of the thumbnail */
  uint32 quality  /* encode quality of the thumbnail */
  /* When encoding, picture of this size and up must have thumbnail */
  //uint32 picture_size
)
{
    return CAMERA_INVALID_PARM;
}

/*===========================================================================

FUNCTION      CAMERA_RELEASE_PICTURE

DESCRIPTION
              The picture buffer in camera_take_picture is released.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_release_picture (void)
{
  return CAMERA_SUCCESS;
} /* camera_release_picture */

/*===========================================================================

FUNCTION      CAMERA_ENABLE_FRAME_CALLBACK

DESCRIPTION
              Enable frame callback.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview, start_record, or start_encode callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_enable_frame_callback (void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_ENABLE_FRAME_CALLBACK, 0, 0, __LINE__);
  MSG_HIGH("camera_enable_frame_callback, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }
  if ((camera_state == CAMERA_STATE_PREVIEW)      ||
      (camera_state == CAMERA_STATE_QVP_ENCODING) ||
      (camera_state == CAMERA_STATE_RECORDING))
  {
    graph_queue_camera_func ((uint32) CAMERA_FUNC_ENABLE_FRAME_CALLBACK,
                             (void *) 0, (void *)0, 0, 0, 0, 0);
    return CAMERA_SUCCESS;
  }
  else
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#endif /* T_IO_CARD */
} /* camera_enable_frame_callback */

/*===========================================================================

FUNCTION      CAMERA_DISABLE_FRAME_CALLBACK

DESCRIPTION
              Disable frame callback.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview, start_record, or start_encode callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_disable_frame_callback (void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_DISABLE_FRAME_CALLBACK, 0, 0, __LINE__);
  MSG_HIGH("camera_disable_frame_callback, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }
  if ((camera_state == CAMERA_STATE_PREVIEW)      ||
      (camera_state == CAMERA_STATE_QVP_ENCODING) ||
      (camera_state == CAMERA_STATE_RECORDING))
  {
    graph_queue_camera_func ((uint32) CAMERA_FUNC_DISABLE_FRAME_CALLBACK,
                             (void *)0, (void *)0, 0, 0, 0, 0);
    return CAMERA_SUCCESS;
  }
  else
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
#endif /* T_IO_CARD */
} /* camera_disable_frame_callback */

/*===========================================================================

FUNCTION      CAMERA_RELEASE_FRAME

DESCRIPTION
              Release the current frame so buffer could be reused.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview, start_record, or start_encode callback.

RETURN VALUE
  camera_ret_code_type
  If successful,
   1. CAMERA_RSP_CB_SUCCESS callback if function is executed successfully.

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_release_frame (void)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_RELEASE_FRAME, 0, 0, __LINE__);
  MSG_CAMERADEBUG("camera_release_frame, camera_state %d", camera_state, 0, 0);
  
  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }
  graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_FRAME, (void *)0,
      (void *)0, CAMERA_INVALID_BUF_INDEX, 0, 0, 0);
  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
} /* camera_release_frame */

/*===========================================================================

FUNCTION      CAMERA_SET_ICON_ARRAY

DESCRIPTION
              Set the ICON array. ICON array is copied. The ICON array is a
              array of ICONS in RGB565. dx and dy provides the dimension of a
              frame where the ICONs are located.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
    CAMERA_EXIT_CB_DONE when set is executed
    CAMERA_EXIT_CB_FAILED if set execution failed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_icon_array
(
  /* Display size */
  uint32          dx,
  uint32          dy,
  /* Transparent color, must be the same for all icons and overlay */
  uint32          transparent_color,
  ipl_icon_type **icon_array
)
{
  return CAMERA_NOT_SUPPORTED;
} /* camera_svcs_set_icon_array */


/*===========================================================================

FUNCTION      CAMERA_CLR_ICON_ARRAY

DESCRIPTION
              Clear the ICON array.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
    CAMERA_EXIT_CB_DONE when set is executed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_clr_icon_array(void)
{
  return CAMERA_NOT_SUPPORTED;
}


/*===========================================================================

FUNCTION      CAMERA_SET_POSITION

DESCRIPTION
              Set position.

              The position is retained for only the next snapshot, then it
              is cleared.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_position
(
  camera_position_type *position,
  camera_cb_f_type      callback,
  void                 *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_POSITION, 0, 0, __LINE__);
  MSG_HIGH("camera_set_position, camera_state %d", camera_state, 0, 0);

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_POSITION);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_POSITION);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  camera_position = *position;

  graph_queue_camera_func ((uint32) CAMERA_FUNC_SET_POSITION,
                           (void *)callback, client_data, 0, 0, 0, 0);

  return CAMERA_SUCCESS;
#endif /* T_IO_CARD */
}


/*===========================================================================

FUNCTION      CAMERA_SET_EXIF_TAG

DESCRIPTION
              Set EXIF tag.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_exif_tag
(
  camera_exif_tag_type *tag,
  camera_cb_f_type      callback,
  void                 *client_data
)
{
    camera_reject (callback, client_data, CAMERA_FUNC_SET_EXIF_TAG);
    event_report (EVENT_CAMERA_SET_FAILED);
    return CAMERA_NOT_SUPPORTED;
}

/*===========================================================================

FUNCTION      CAMERA_BLT

DESCRIPTION
              Color conversion, rotation, or adding frame and block transfer
              the result into destination buffer. Output color is RGB565.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_blt
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void              *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy
)
{
  return camera_svcs_blt_ex(frame, dst_dx, dst_dy, dst_ptr, x, y, dx, dy, CAMERA_YCBCR_4_2_0);
}


/*===========================================================================

FUNCTION      CAMERA_SVCS_BLT_EXT

DESCRIPTION
              Color conversion, rotation, or adding frame and block transfer
              the result into destination buffer. Output color is RGB565.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_blt_ext
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void               *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy,
  camera_format_type format
)
{
  return camera_svcs_blt_ex(frame, dst_dx, dst_dy, dst_ptr, x, y, dx, dy, format);
}

/*===========================================================================

FUNCTION      CAMERA_SVCS_BLT_EX

DESCRIPTION
  Display to either LCD directly or via bitmap.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_blt_ex
(
  camera_frame_type *frame,
  /* The destination RGB buffer dimension and pointer. Pointer indeces
   * beginning of data buffer. */
  uint16             dst_dx,
  uint16             dst_dy,
  void               *dst_ptr,
  /* Rectangle of the destination used for update */
  uint16             x,
  uint16             y,
  uint16             dx,
  uint16             dy,
  camera_format_type format
)
{
/*lint -save -e715 function param not referenced */
  qcamrawHeaderType *header;
  byte *bmy_addr;
  
  /* Do a Null Check for frame pointer */
  if(frame->buffer!=NULL)
  {
     header= (qcamrawHeaderType *) frame->buffer;
  }
  else
  {  
     return CAMERA_INVALID_PARM;
  }
  
  if (header->frameType == QCAMRAW_PICTURE_FRAME)
  {

    if ((*(uint32*)(frame->thumbnail_image)   != 0x4D414351) ||
        (*(uint32*)(frame->thumbnail_image+4) != 0x20574152))
    {
      event_report (EVENT_CAMERA_INVALID_STATE);
      return CAMERA_INVALID_FORMAT;
    }
    bmy_addr = qcamrawGetDataYPtr (frame->thumbnail_image);
  }
  else if ((header->frameType == QCAMRAW_PREVIEW_FRAME) &&
           ((camera_state == CAMERA_STATE_PREVIEW) ||
            (camera_state == CAMERA_STATE_RECORDING) ||
            (camera_state == CAMERA_STATE_QVP_ENCODING)))
  {
    if ((header->buffer_index >= CAMERA_NUM_OF_PREVIEW_BUFFERS) ||
        ((uint32)(frame->buffer) % 4 != 0) ||
        (*(uint32*)(frame->buffer) != 0x4D414351) ||
        (*(uint32*)(frame->buffer+4) != 0x20574152))
    {
      event_report (EVENT_CAMERA_INVALID_STATE);
      return CAMERA_INVALID_FORMAT;
    }
    bmy_addr = qcamrawGetDataYPtr (frame->buffer);
  }
  else
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_FORMAT;
  }
  
  if(format == CAMERA_RGB565 && frame->format == CAMERA_RGB565)
  {
    uint16 *pBitsSrc16 = (uint16*)bmy_addr;
    uint16 *pBitsDst16 = (uint16*)dst_ptr;
    int i;
    int nSize;
    int nSizeMod;
    uint16 *pDest,*pSrc;
    int16 wCpyWidth, wCpyHeight, nSrcPitch, nDstPitch;
    
    wCpyWidth  = MIN(dx,frame->dx);
    wCpyHeight = MIN(dy,frame->dy);
    nSrcPitch  = frame->dx;
    nDstPitch  = dx;
    pBitsDst16+= y*nDstPitch+x;
    
    for(i=0; i<wCpyHeight; i++)
    {
        pDest    = pBitsDst16;
        pSrc     = pBitsSrc16;
        nSize    = wCpyWidth>>3;
        nSizeMod = wCpyWidth&0x7;
        
        while(nSize--)
        {
            *pDest++ = *pSrc++;
            *pDest++ = *pSrc++;
            *pDest++ = *pSrc++;
            *pDest++ = *pSrc++;
            *pDest++ = *pSrc++;
            *pDest++ = *pSrc++;
            *pDest++ = *pSrc++;
            *pDest++ = *pSrc++;
        }
        
        while(nSizeMod--)
        {
            *pDest++ = *pSrc++;
        }
        
        pBitsDst16 += nDstPitch;
        pBitsSrc16 += nSrcPitch;
    }
  }
  else
  {
    if (header->frameType == QCAMRAW_PREVIEW_FRAME)
    {
       graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_FRAME,(void *)0, (void *)0, (uint32) header->buffer_index, 0, 0, 0);
    }
    return CAMERA_INVALID_FORMAT;
  }
  
  if (header->frameType == QCAMRAW_PREVIEW_FRAME)
  {
    graph_queue_camera_func ((uint32)CAMERA_FUNC_RELEASE_FRAME,(void *)0, (void *)0, (uint32) header->buffer_index, 0, 0, 0);
  }
  return CAMERA_SUCCESS;

/*lint -restore */
} /* camera_svcs_blt_ex */

/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_PARM

DESCRIPTION
              Set various control on the camera sensor.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_parm
(
  camera_parm_type  id,
  int32             parm,
  camera_cb_f_type  callback,
  void             *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_PARM, (uint32) id, (uint32) parm, __LINE__);
  MSG_HIGH("camera_set_parm, id %d, parm %d", id, parm, 0);

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  camera_parm_id = id;

  switch (id)
  {
    case CAMERA_PARM_ZOOM:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_PREVIEW_ROTATION:
    case CAMERA_PARM_ENCODE_ROTATION:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_CONTRAST:
      /* Update the brightness index */
      if (parm > CAMERA_MAX_CONTRAST)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else if (parm < CAMERA_MIN_CONTRAST)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else
      {
        ret_val = CAMERA_SUCCESS;

        if (camera_parm_contrast.current_value != parm)
        {
          camera_parm_contrast.current_value = parm;
          (void) camsensor_set_contrast ((int8) parm);
        }
      }
      break;

    case CAMERA_PARM_BRIGHTNESS:
      if (parm > camera_parm_brightness.maximum_value)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else if (parm < camera_parm_brightness.minimum_value)
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else
      {
        (void) camsensor_set_brightness ((int8) parm);
         camera_parm_brightness.current_value = parm;
         ret_val = CAMERA_SUCCESS;
      }
      break;

    case CAMERA_PARM_EXPOSURE_COMPENSATION:
     if ((parm > camera_parm_exposure_compensation.maximum_value) ||(parm < camera_parm_exposure_compensation.minimum_value) )
      {
        ret_val =  CAMERA_INVALID_PARM;
      }
      else
      {
        (void) camsensor_set_ev_compensation ((int32)parm);
        camera_parm_exposure_compensation.current_value =  parm;
        ret_val = CAMERA_SUCCESS;
      }
      break;

    case CAMERA_PARM_SHARPNESS:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_EXPOSURE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_WB:
      ret_val=camsensor_set_wb ((int8) parm);
      if(ret_val == CAMERA_SUCCESS)
      {
        /* Update global wb setting */
        camera_parm_white_balance.current_value =  parm;
      }
      break;

    case CAMERA_PARM_EFFECT:
      camera_parm_special_effects.current_value =  parm;
     ret_val=camsensor_set_effect ((int8) parm);
     break;

    case CAMERA_PARM_FPS:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_FLASH:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_FLASH_STATE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_NIGHTSHOT_MODE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_REFLECT:

      if((parm < CAMERA_NO_REFLECT) || (parm >= CAMERA_MAX_REFLECT))
      {
        ret_val = CAMERA_INVALID_PARM;
        break;
      }
      if (parm == camera_parm_reflect.current_value)
      {
        break;
      }
      camera_parm_reflect.current_value = parm;
      graph_queue_camera_func ((uint32)CAMERA_FUNC_SET_REFLECT,
                               (void *) callback, client_data, 0, 0, 0, 0);
      break;

    case CAMERA_PARM_PREVIEW_MODE:
      if ((parm == (int)CAMERA_PREVIEW_MODE_MOVIE) ||
          (parm == (int)CAMERA_PREVIEW_MODE_SNAPSHOT))
      {
        camera_parm_preview_mode.current_value = parm;
        preview_mode                           = (camera_preview_mode_type)parm;
      }
      else
      {
        ret_val = CAMERA_INVALID_PARM;
      }
      break;

    case CAMERA_PARM_ANTIBANDING:

      if ((int)camera_parm_antibanding.current_value != parm)
      {
        if (parm < (int)CAMERA_MAX_ANTIBANDING)
        {
          camera_parm_antibanding.current_value = parm;
          ret_val=camsensor_set_antibanding ((int8) parm);
        }
        else
        {
          ret_val = CAMERA_INVALID_PARM;
        }
      }
      break;
    case CAMERA_PARM_RED_EYE_REDUCTION:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_FOCUS_STEP:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;


    case CAMERA_PARM_EXPOSURE_METERING:
    case CAMERA_PARM_AUTO_EXPOSURE_MODE:  /* Legacy parm, EXPOSURE_METERING has replaced it */

      /* Invalid state if the current value is out of range */
      if ((parm < camera_parm_auto_exposure_mode.minimum_value) ||
          (parm > camera_parm_auto_exposure_mode.maximum_value))
      {

        ret_val = CAMERA_INVALID_PARM;
      }
      else
      {
        /* Even though exposure metering is controlled by bestshot we will allow
         * user to change value after bestshot mode is applied
         */
        camera_parm_auto_exposure_mode.current_value = parm;
        ret_val= camsensor_set_exposure_mode ((int8) parm);
      }

      break;

#ifdef FEATURE_VIDENC_TRANSITION_EFFECTS
    case CAMERA_PARM_TRANSITION:
      camcorder_svcs_set_parm_transit(parm);
      break;
#endif /* FEATURE_VIDENC_TRANSITION_EFFECTS */

    case CAMERA_PARM_ISO:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_APERTURE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_SHUTTER_SPEED:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_LUMA_ADAPTATION:
    default:
      MSG_ERROR("Invalid control", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      ret_val = CAMERA_INVALID_PARM;
      break;
  }
  if (ret_val == CAMERA_SUCCESS)
  {
    if (callback)
    {
      (callback) (CAMERA_EXIT_CB_DONE,
                  client_data,
                  CAMERA_FUNC_SET_PARM,
                  (int)camera_parm_id);
    }
    MSG_HIGH ("Set done.", 0, 0, 0);
    event_report (EVENT_CAMERA_SET_SUCCEEDED);
  }
  return ret_val;

#endif /* T_IO_CARD */
} /* camera_svcs_set_parm */




/*===========================================================================

FUNCTION      CAMERA_SVCS_SET_PARM_2

DESCRIPTION
              Set various control on the camera sensor.
              Set two control parameters with this function.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  if successful, then
    CAMERA_EXIT_CB_DONE when set is executed --------------- PREVIEW
    CAMERA_EXIT_CB_FAILED if set execution failed ---------- PREVIEW

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_set_parm_2
(
  camera_parm_type  id,
  int32             parm1,
  int32             parm2,
  camera_cb_f_type  callback,
  void             *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_ret_code_type ret_val = CAMERA_SUCCESS;

  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_SET_PARM, (uint32) id, (uint32) parm1, __LINE__);
  MSG_LOW ("Camera set parm2", id, parm1, parm2);

  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_SET_PARM);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  camera_parm_id = id;

  switch (id)
  {
    /* For now, this parm only has application for Auto Frame Rate */
    case CAMERA_PARM_PREVIEW_FPS:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_PARM_AF_MODE:
      {
        ret_val = CAMERA_NOT_SUPPORTED;
        break;
      }
    
    default:
      MSG_ERROR("Invalid control", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      ret_val = CAMERA_INVALID_PARM;
      break;
  }

  if (ret_val == CAMERA_SUCCESS)
  {
    if (callback)
    {
      (callback) (CAMERA_EXIT_CB_DONE,
                  client_data,
                  CAMERA_FUNC_SET_PARM,
                  (int)camera_parm_id);
    }
    MSG_HIGH ("Set done.", 0, 0, 0);
    event_report (EVENT_CAMERA_SET_SUCCEEDED);
  }
  return ret_val;

#endif /* T_IO_CARD */
} /* camera_svcs_set_parm_2 */




/*===========================================================================

FUNCTION      CAMERA_GET_INFO

DESCRIPTION
              Retrieve sensor info

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/

void camera_svcs_get_info(camera_info_type *info)
{
  camera_log (LOG_FUNC, (uint32)CAMERA_FUNC_GET_INFO, 0, 0, __LINE__);

  if ((info != NULL) && 
      (camera_state != CAMERA_STATE_INIT))
  {
    /* Sensor ID - not sure exactly what this is, but we'll assume it
       is the sensor enum in a multiple-sensor camera */
    info->sensor_id = (uint16) camera_asi;

    /* char *strncpy( char *strDest, const char *strSource, size_t count ); */
#ifdef CAMERA_USES_LIBSTD
    (void) std_strlcpy (info->sensor_model,
                    camsensor_static_params[camera_asi].sensor_name,
                    31 ); /* truncate to 30 characters */
#else /* CAMERA_USES_LIBSTD */
    (void) strncpy (info->sensor_model,
                    camsensor_static_params[camera_asi].sensor_name,
                    31 ); /* truncate to 30 characters */
    info->sensor_model[31] = 0; /* Null terminate string if not already */
#endif /* CAMERA_USES_LIBSTD */

    /* Assuming we need full size width and height */
    info->sensor_width            = camsensor_static_params[camera_asi].full_size_width;
    info->sensor_height           = camsensor_static_params[camera_asi].full_size_height;
    /* This puts it in Q8 format by default */
    info->fps                     = (uint16) camera_parm_fps.current_value;
    info->sensor_rotation_sensing = FALSE;
    info->default_rotation        = 0;
    info->default_orientation     = CAMERA_ORIENTATION_LANDSCAPE;
  }

} /* camera_svcs_get_info */



/*===========================================================================

FUNCTION      CAMERA_GET_PARM

DESCRIPTION
              Retrieve control values.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_svcs_get_parm
(
  camera_parm_type       id,
  camera_parm_info_type *parm
)
{
  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  switch (id)
  {
    case CAMERA_PARM_STATE:
      parm->current_value = (int)camera_state;
      break;

    case CAMERA_PARM_ACTIVE_CMD:
      parm->current_value = (int)camera_func;
      break;

    case CAMERA_PARM_CONTRAST:
      *parm = camera_parm_contrast;
      break;

    case CAMERA_PARM_BRIGHTNESS:
      *parm = camera_parm_brightness;
      break;

    case CAMERA_PARM_SHARPNESS:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_EXPOSURE:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_WB:
      *parm = camera_parm_white_balance;
      break;
      
    case CAMERA_PARM_RED_EYE_REDUCTION:
      return CAMERA_NOT_SUPPORTED;
    
    case CAMERA_PARM_NIGHTSHOT_MODE:
      return CAMERA_NOT_SUPPORTED;
      
    case CAMERA_PARM_REFLECT:
      *parm = camera_parm_reflect;
      break;

    case CAMERA_PARM_PREVIEW_MODE:
      *parm = camera_parm_preview_mode;
      break;

    case CAMERA_PARM_ANTIBANDING:
      *parm = camera_parm_antibanding;
      break;

    case CAMERA_PARM_FOCUS_STEP:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_EXPOSURE_METERING:
    case CAMERA_PARM_AUTO_EXPOSURE_MODE:  /* Legacy parm, EXPOSURE_METERING has replaced it */
      *parm = camera_parm_auto_exposure_mode;
      break;

    case CAMERA_PARM_EXPOSURE_COMPENSATION:
      *parm = camera_parm_exposure_compensation;
      break;

    case CAMERA_PARM_PREVIEW_FPS:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_ISO:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_APERTURE:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_SHUTTER_SPEED:
      return CAMERA_NOT_SUPPORTED;

    case CAMERA_PARM_LUMA_ADAPTATION:
      return CAMERA_NOT_SUPPORTED;
      
    default:
      MSG_ERROR ("Invalid control", 0, 0, 0);
      event_report (EVENT_CAMERA_INVALID_SET_ID);
      return CAMERA_INVALID_PARM;
  }
  return CAMERA_SUCCESS;
} /* camera_get_parm */


/*===========================================================================

FUNCTION      CAMERA_GET_PARM_2

DESCRIPTION
              Retrieve control values.

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/

camera_ret_code_type camera_svcs_get_parm_2
(
  camera_parm_type      id,
  int32                *parm1_ptr,
  int32                *parm2_ptr)
{
  camera_ret_code_type ret_val = CAMERA_NOT_SUPPORTED;

  if (camsensor_initialized == FALSE)
  {
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }

  switch (id)
  {
  case CAMERA_PARM_AF_MODE:
  default:
    ret_val = CAMERA_NOT_SUPPORTED;
    break;
  }

  return ret_val;
} /* camera_get_parm_2 */

/*============================================================================
*                         EXPORTED FUNCTIONS
*             These functions are used below the BREW layer
============================================================================*/


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
)
{
  camera_cb_f_type                  cb       = (camera_cb_f_type) callback;
  void                             *data     = client_data;
  camera_log (LOG_PFUNC, (int)function, 0, 0, __LINE__);
  switch (function)
  {
    case CAMERA_FUNC_START:
      camera_process_start (cb, data, (rex_tcb_type *) info1, (camera_ret_code_type *) info2);
      break;

    case CAMERA_FUNC_STOP:
      camera_process_stop (cb, data, (rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_CLEANUP:
      camera_process_cleanup ((rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_START_PREVIEW:
      camera_process_start_preview (cb, data);
      break;

    case CAMERA_FUNC_STOP_PREVIEW:
      camera_process_stop_preview ((rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_TAKE_PICTURE:
      camera_process_take_picture (cb, data
                                  );
      break;
      
    case CAMERA_FUNC_ENCODE_PICTURE:
      camera_process_encode_picture (cb, data, (qcamrawHeaderType *) info1,
                                     (uint8 *) info2,
                                     (qcamrawHeaderType *) info3, (uint8 *) info4);
      break;

    case CAMERA_FUNC_ENABLE_FRAME_CALLBACK:
      camera_process_enable_frame_callback ();
      break;

    case CAMERA_FUNC_DISABLE_FRAME_CALLBACK:
      camera_process_disable_frame_callback ();
      break;

    case CAMERA_FUNC_RELEASE_FRAME:
      camera_process_release_frame ((uint8) info1);
      break;

    case CAMERA_FUNC_BLT:
      camera_process_blt ((uint8) info1);
      break;

    case CAMERA_FUNC_SET_OVERLAY:
      camera_process_set_overlay (cb, data, (rex_tcb_type *) info1);
      break;

    case CAMERA_FUNC_CLR_OVERLAY:
      camera_process_clr_overlay (cb, data);
      break;

    case CAMERA_FUNC_SET_REFLECT:
      camera_process_set_reflect ();
      break;

    case CAMERA_FUNC_SET_DIMENSIONS:
      camera_process_set_dimensions (cb, data,(rex_tcb_type*)info1);
      break;

    case CAMERA_FUNC_SET_POSITION:
      break;

    case CAMERA_FUNC_START_FOCUS:
      if(cb)
      {
        cb(CAMERA_RSP_CB_SUCCESS, data, CAMERA_FUNC_START_FOCUS, 0);
      }
      break;

    case CAMERA_FUNC_MOVE_LENS_DONE:
      break;

    case CAMERA_FUNC_HANDLE_ERROR:
      camera_process_handle_error ();
      break;
      
    case CAMERA_FUNC_TERMINATE:
      camera_process_terminate();
      break;

    default:
      break;
  }
} /* camera_process_func */



/*===========================================================================

FUNCTION      CAMERA_PROCESS_QDSP_MSG

DESCRIPTION
              Processes QDSP events in graph task context.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_process_qdsp_msg (uint32 msg, void *buffer)
{
  CAMSoftDSP_ResponseMsgType qdsp_msg = (CAMSoftDSP_ResponseMsgType) msg;
  switch (qdsp_msg)
  {
    case CAMSOFTDSP_MSG_START_OF_FRAME:
      SoftDSP_HandleMSG(qdsp_msg, NULL);
      break;
      
    case CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME:
      camera_process_softdsp_output1_msg ((Camera_EndOfFrameMessageType *) buffer);
      break;
      
    case CAMSOFTDSP_MSG_OUTPUT2_END_OF_FRAME:
      camera_process_softdsp_output2_msg ((Camera_EndOfFrameMessageType *) buffer);
      break;
      
    case CAMSOFTDSP_MSG_ILLEGAL_COMMAND:
      camera_process_softdsp_illegal_cmd_msg ();
      break;
      
    default:
      break;
  }
} /* camera_process_qdsp_msg */

/*===========================================================================

FUNCTION      CAMERA_WAIT_CLK_CB

DESCRIPTION
              Handle Callback after a delay has completed.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_wait_clk_cb(int4 ms)
{
  if (camera_timed_wait_tcb)
  {
    (void) rex_set_sigs (camera_timed_wait_tcb, CAMERA_TIMER_SIG);
  }
}
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_TIMED_WAIT

DESCRIPTION
              Wait for some milli-seconds.
              Resolution is 5ms.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
void camera_timed_wait (uint32 ms)
{
  rex_enter_crit_sect(&camera_wait_clk_cs);
  if (camera_timer_up)
  {
    uint32        save;

    /* Lock to make sure timer callback does
     * not happen until rex_wait is called */
    INTLOCK_SAV (save);

    camera_timed_wait_tcb = rex_self ();

    clk_reg (&camera_wait_clk, camera_wait_clk_cb, ms, 0, FALSE);
    (void) rex_wait (CAMERA_TIMER_SIG);
    /* We are here due to either timeout or other conditions.
     * Make sure timer is stopped. */
    clk_dereg (&camera_wait_clk);
    (void) rex_clr_sigs (camera_timed_wait_tcb, CAMERA_TIMER_SIG);

    camera_timed_wait_tcb = NULL;

    INTFREE_SAV (save);
  }
  else
  {
    clk_busy_wait (ms * 1000);
  }
  rex_leave_crit_sect(&camera_wait_clk_cs);
} /* camera_timed_wait */

/* ------------------------------------------------------------------ */
/* Local Function Implementations                                     */
/* ------------------------------------------------------------------ */

/*===========================================================================

FUNCTION      CAMERA_REJECT

DESCRIPTION
              Reject a function call. It is used only in pre-processing
              of the function calls.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_reject
(
 camera_cb_f_type callback,
 void            *client_data,
 camera_func_type func
)
{
  MSG_CAMERADEBUG("Camera Reject called. camera_state:%d function:%d", camera_state, func, 0);
  if (callback)
  {
    (*callback) (CAMERA_EXIT_CB_FAILED, client_data, func, 0);
  }
  event_report (EVENT_CAMERA_FUNCTION_REJECTED);
}

/*===========================================================================

FUNCTION      CAMERA_CLR_OVERLAY

DESCRIPTION
              Clear the overlay frame.

              Command is queued and processed at camera_process_func().

DEPENDENCIES
  Must not be in INIT state.

RETURN VALUE
  camera_ret_code_type
  If successful,
    CAMERA_EXIT_CB_DONE when set is executed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_svcs_clr_overlay
(
  camera_cb_f_type callback,
  void            *client_data
)
{
#ifdef T_IO_CARD
#error code not present
#else /* T_IO_CARD */

  camera_log (LOG_FUNC, (int)CAMERA_FUNC_CLR_OVERLAY, 0, 0, __LINE__);
  MSG_LOW ("Camera clear overlay", 0, 0, 0);
  if (camsensor_initialized == FALSE)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_CLR_OVERLAY);
    event_report (EVENT_CAMERA_NO_SENSOR);
    return CAMERA_NO_SENSOR;
  }

  if (camera_state == CAMERA_STATE_INIT)
  {
    camera_reject (callback, client_data, CAMERA_FUNC_CLR_OVERLAY);
    event_report (EVENT_CAMERA_INVALID_STATE);
    return CAMERA_INVALID_STATE;
  }
  
  graph_queue_camera_func ((uint32) CAMERA_FUNC_CLR_OVERLAY,
                           (void *) callback, client_data, 0, 0, 0, 0);
  return CAMERA_SUCCESS;

#endif /* T_IO_CARD */
} /* camera_clr_overlay */

/* ---------------------------------------------------------------- */
/* Local functions which are executed in the graph task's context.  */
/* ---------------------------------------------------------------- */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_START

DESCRIPTION
              Set the LCD display window where application can render on.
              Other parts of LCD may be owned by others.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_start
(
  camera_cb_f_type callback,
  void            *client_data,
  rex_tcb_type    *tcb,
  camera_ret_code_type *status
)
{
#ifndef T_IO_CARD
  camsensor_static_params_type *camsensor_params = &(camsensor_static_params[camera_asi]);
  
  /* Need to power on sensor, initialize it, power it up, before continue */
  camsensor_power_on();
  camsensor_power_up();

  if (camsensor_initialized == FALSE)
  {
    if (callback)
    {
      (*callback) (CAMERA_EXIT_CB_FAILED, client_data, CAMERA_FUNC_START, 0);
    }
    event_report (EVENT_CAMERA_NO_SENSOR);
    MSG_ERROR("CAMERA_SVCS: Invalid state for camsensor_initialized %d", camsensor_initialized, 0, 0);
    *status = CAMERA_NO_SENSOR;
    if (tcb)
    {
      (void) rex_set_sigs (tcb, CAMERA_SIG);
    }
    return;
  }

  /* Set config params for VFE for this specific sensor.  Place
     these in global structure */
  (void) camsensor_start( camsensor_params );

  camera_parm_fps.maximum_value = camsensor_static_params[camera_asi].max_video_fps >> 8;
  camera_parm_fps.default_value = camsensor_static_params[camera_asi].video_fps >> 8;

  if (SoftDSP_Start(&camsensor_static_params[camera_asi]) != CAMERA_SUCCESS)
  {
    *status = CAMERA_FAILED;
    if (tcb)
    {
      (void) rex_set_sigs (tcb, CAMERA_SIG);
    }
    return;

  }

  camera_dx            = 0;
  camera_dy            = 0;

  camera_parm_fps.current_value             = camsensor_static_params[camera_asi].video_fps >> 8;
  camera_parm_reflect.current_value        = (int)CAMERA_NO_REFLECT;
  camera_reflect_ipl                       = IPL_NOREF;
  camera_reflect                           = QCAMRAW_NO_REFLECT;

  preview_mode         = CAMERA_PREVIEW_MODE_SNAPSHOT;

  camera_parm_preview_mode.current_value      = (int)preview_mode;
  camera_parm_antibanding.current_value       = (int)CAMERA_ANTIBANDING_OFF;
  
  scale_preview_overlay = FALSE; /* do not resize overlay frame by default */
  
  /* Set these back to default */
  camera_parm_exposure_compensation.current_value = camera_parm_exposure_compensation.default_value;
  camera_parm_brightness.current_value = camera_parm_brightness.default_value;

  camera_parm_contrast.current_value = camera_parm_contrast.default_value;

  /* Set these back to default */
  camera_parm_special_effects.current_value = camera_parm_special_effects.default_value;
  camera_parm_white_balance.current_value = camera_parm_white_balance.default_value;
  camera_parm_auto_exposure_mode.current_value = camera_parm_auto_exposure_mode.default_value;

  {
    /* Min == max FPS here initially. */
    /* Denominator in the MS 16 bits; numerator time denominator in the LS 16 */
    camera_parm_preview_fps.minimum_value = ((CAMERA_FPS_DENOMINATOR << 16) |
       (0xFFFF & (CAMERA_FPS_DENOMINATOR*(camsensor_static_params[camera_asi].max_preview_fps>>8))));

    /* Max FPS supported from this sensor */
    camera_parm_preview_fps.maximum_value = camera_parm_preview_fps.minimum_value;
    camera_parm_preview_fps.default_value = camera_parm_preview_fps.minimum_value;
    camera_parm_preview_fps.current_value = camera_parm_preview_fps.default_value;

  }
  
  camera_take_picture_status_set_default();
  camera_capture_delay = 0;
  
  /* One exception here. We need to set READY state in client context.
   * Client may call other functions right away, before the function is
   * executed in the service context. */
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
  camera_state = CAMERA_STATE_READY;

  camera_func     = CAMERA_FUNC_START;
  
  camera_stopping = FALSE;
  /* Configure the gpios and clk regimes */
  MSG_CAMERADEBUG("CAMERA_SVCS: Configure GPIO settings", 0, 0, 0);
  camsensor_config_sensor_gpios_clks ();

  *status = CAMERA_SUCCESS;

  if (callback)
    (callback) (CAMERA_EXIT_CB_DONE, client_data, camera_func, 0);

  MSG_HIGH("CAMERA_SVCS: Start. State transition to ready", 0, 0, 0);

  event_report (EVENT_CAMERA_START);
  if (tcb)
  {
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }
#endif /* nT_IO_CARD */
} /* camera_process_start */

/*===========================================================================

FUNCTION      CAMERA_ABORT_TAKEPICTURE_OPERATION

DESCRIPTION
              This function will decide whether camera service layer need to
              abort take picture or encode operation.

DEPENDENCIES
              Reason needs to send to this function as to why the abort
              TRUE  if STOPPING the application.
              FALSE if SERVICE UNAVAILABLE.

RETURN VALUE
              TRUE if aborting.
              FALSE if not.

SIDE EFFECTS
  None

===========================================================================*/
static boolean camera_abort_takepicture_encoding_operation(void)
{
  boolean ret_val = FALSE;

  if ( ( camera_state == CAMERA_STATE_ENCODE_PICTURE ) ||
       ( camera_state == CAMERA_STATE_TAKE_PICTURE )
     )
  {
    if (camera_state == CAMERA_STATE_ENCODE_PICTURE && camera_take_picture_status.encode_in_progress)
    {
      camera_take_picture_status.abort = TRUE;
      switch (camera_encode_properties.format)
      {
#ifdef FEATURE_PNG_ENCODER
        case CAMERA_PNG:
          MSG_CAMERADEBUG("CAMERA_SVCS: PNG ENCODING IN PROGRESS SO ABORT", 0, 0, 0);
          pnge_encode_abort();
          ret_val = TRUE;
          break;
#endif /* FEATURE_PNG_ENCODER */
        case CAMERA_JPEG:
        case CAMERA_RAW:
        default:
          /* RAW case nothing to abort. */
          MSG_CAMERADEBUG("CAMERA_SVCS: RAW ENCODING IN PROGRESS NOTHING TO ABORT", 0, 0, 0);
          camera_take_picture_status.abort = FALSE;
      }
    }
    else if (camera_state == CAMERA_STATE_TAKE_PICTURE)
    {
    }
  }

  return ret_val;

}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_STOP

DESCRIPTION
              The camera operation is stopped and the sensor enters power
              saving mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_stop
(
  camera_cb_f_type callback,
  void            *client_data,
  rex_tcb_type    *tcb
)
{
#ifndef T_IO_CARD
  camera_func     = CAMERA_FUNC_STOP;
  camera_app_cb   = callback;
  camera_app_data = client_data;
  camera_tcb      = tcb;
  MSG_HIGH ("Stop. camera_process_stop camera_state:%d camera_stopping:%d", camera_state, camera_stopping, 0);
  event_report (EVENT_CAMERA_STOP);
  camera_terminate (CAMERA_EXIT_CB_DONE, 0);
#endif /* nT_IO_CARD */
} /* camera_process_stop */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_CLEANUP

DESCRIPTION
              The camera operation is stopped, the snapshot buffers are cleared
              and sensor is powered down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_cleanup
(
  rex_tcb_type    *tcb
)
{
#ifndef T_IO_CARD
  camera_svcs_optimize_resources();

  if (camsensor_initialized == TRUE)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Call power down sequence", 0, 0, 0);
    camsensor_power_down ();
    camsensor_power_off ();

    /* default the gpios and disable clk
     * regimes */
    MSG_CAMERADEBUG("CAMERA_SVCS: Default GPIO settings", 0, 0, 0);
    camsensor_unconfig_sensor_gpios_clks ();
  }
  
  camera_clr_take_picture ();
  
  MSG_CAMERADEBUG("CAMERA_SVCS: Clean Up: Set camera_state=CAMERA_STATE_INIT", 0, 0, 0);
  camera_state = CAMERA_STATE_INIT;

  if (tcb)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Clean Up: Set CAMERA_SIG", 0, 0, 0 );
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }

#endif /* nT_IO_CARD */
} /* camera_process_cleanup */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_START_PREVIEW

DESCRIPTION
              Enters previow mode.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_start_preview
(
  camera_cb_f_type callback,
  void            *client_data
)
{
#ifndef T_IO_CARD
  camera_blt_ext_recalculate_window = TRUE;

  /* Terminate an existing function first */
  /* Be certain this is the VFE/MP4E image by checking the state */
  if ((camera_state == CAMERA_STATE_PREVIEW) ||
      (camera_state == CAMERA_STATE_QVP_ENCODING))
  {
    //MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    //camera_terminate (CAMERA_EXIT_CB_DONE, 0);
    //MSG_HIGH ("Stop previous command.", 0, 0, 0);
    if (camera_app_cb)
    {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, client_data, CAMERA_FUNC_START_PREVIEW, 0);
    }
    return;
  }

  CAMERA_SLEEP_FORBID ();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_ON();

  camera_clr_take_picture();
  camera_func          = CAMERA_FUNC_START_PREVIEW;
  MSG_CAMERADEBUG("CAMERA_SVCS: Request camera_stopping=NO_STOPPING", 0, 0, 0);
  camera_stopping = FALSE;
  camera_app_cb        = callback;
  camera_app_data      = client_data;
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_PREVIEW", 0, 0, 0);
  camera_state = CAMERA_STATE_PREVIEW;
  camera_camsensor_op_mode = CAMSENSOR_OP_MODE_PREVIEW;

#ifndef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
  camera_svcs_optimize_resources ();
#endif /* FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III */

  MSG_HIGH ("State transition to preview", 0, 0, 0);
  event_report (EVENT_CAMERA_PREVIEW);

  if((camera_preview_dx == 0 || camera_preview_dy == 0))
  {
    MSG_ERROR("Invalid Preview Dimensions, Failing in camera_process_start_preview",0,0,0);

    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate (CAMERA_EXIT_CB_ERROR, (int)CAMERA_ERROR_CONFIG);
    return;
  }
  
  if(camera_set_preview_buffers())
  {
      MSG_HIGH("No memory!, Failing camera_process_start_preview",0,0,0);
      return;
  }
  camera_set_preview_headers ();
  {
    boolean ret_val;
    int i;
    camera_reset_preview_buffers();
    camsensor_config(CAMSENSOR_OP_MODE_PREVIEW, &camsensor_static_params[camera_asi]);
    for(i=0;i<CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE;i++)
    {
        SoftDSP_PushPreviewBuff(camera_preview_buffers.buffers[i].buf_ptr+qcamraw_header_size);
        camera_preview_set_buffer_status(i, BUSY_WITH_VFE);
    }
    ret_val = (SoftDSP_Preview (camera_softdsp_cb,camera_preview_dx,camera_preview_dy) != 0);
    
    if (ret_val)
    {
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      MSG_ERROR ("CAMERA_SVCS: Cannot load DSP. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_LOAD_DSP);
    }
    else
    {
      if (camera_app_cb)
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
      }
    }
  }
#endif /* nT_IO_CARD */
} /* camera_process_start_preview */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_STOP_PREVIEW

DESCRIPTION
              Stop the preview mode.
              CAMERA_PREVIEW_STOPPED callback is make before function
              returns.

DEPENDENCIES
  The camera sensor must be make ready first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_stop_preview ( rex_tcb_type *tcb )
{
#ifndef T_IO_CARD

  camera_tcb = tcb;
  
  if ( !camera_stopping)
  {
    camera_terminate (CAMERA_EXIT_CB_DONE, 0);
  }
  else
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: PROCESS_STOP_PREVIEW camera_stopping:%d camera_state:%d", camera_stopping, camera_state, 0);
  }


#endif /* nT_IO_CARD */
} /* camera_process_stop_preview */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_TAKE_SINGLE_PICTURE

DESCRIPTION
              The next frame is taken for encoding at the specified format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_take_single_picture(void)
{
#ifndef T_IO_CARD
  boolean ret_val = FALSE;
  snapshot_frame.thumbnail_image = 0;
  snapshot_frame.buffer = 0;

  camera_abort_picture = FALSE;

  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_TAKE_PICTURE", 0, 0, 0);
  camera_state = CAMERA_STATE_TAKE_PICTURE;

  camera_svcs_optimize_resources ();
  
  camera_take_picture_status_set_default();
  camera_capture_delay = 0;
  camera_camsensor_op_mode = CAMSENSOR_OP_MODE_SNAPSHOT;

  if (camera_encode_properties.format == CAMERA_RAW)
  {
    camera_camsensor_op_mode = CAMSENSOR_OP_MODE_RAW_SNAPSHOT;
  }

  MSG_HIGH ("State transition to take picture", 0, 0, 0);
  event_report (EVENT_CAMERA_TAKE_PICTURE);

  if (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_NONE)
  {
    camsensor_config(CAMSENSOR_OP_MODE_SNAPSHOT, &camsensor_static_params[camera_asi]);
    /* Main image: Allocate exact memory required for the output format */
    CAM_Free (snapshot_luma_buf);
    /* CAM_Malloc gives 32-bit aligned memory buffer. */
    snapshot_luma_buf = CAM_Malloc ((camera_dx*camera_dy*sizeof(uint16) +
                                   sizeof (qcamrawHeaderType)+ CAMERA_BUF_ALIGN)
                                   | CAM_ALLOC_NO_ZMEM);
    if (snapshot_luma_buf == NULL)
    {
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      event_report (EVENT_CAMERA_CANNOT_LOAD_DSP);
      return;
    }
    
    /* Set the header for main image */
    qcamrawSetHeader((qcamrawHeaderType *) snapshot_luma_buf, 0, camera_dx*camera_dy*sizeof(uint16), camera_snapshot_format, QCAMRAW_PICTURE_FRAME,
                     QCAMRAW_NO_REFLECT, 0, camera_dx,
                     camera_dy, camera_dx, camera_dy, 0, 0, 0, 0,
                     camera_dx*camera_dy, 0, 0, (uint8 *)0);
    
    SoftDSP_SetCaptureBuff(snapshot_luma_buf+qcamraw_header_size);
    ret_val = (SoftDSP_Capture (camera_softdsp_cb,camera_dx,camera_dy) != 0);
    
    if (ret_val)
    {
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      MSG_ERROR ("Cannot load DSP. State transition to ready", 0, 0, 0);
      event_report (EVENT_CAMERA_CANNOT_LOAD_DSP);
    }
    else
    {
      if (camera_app_cb)
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
      }
    }
  }
#endif /* nT_IO_CARD */
} /* camera_process_take_single_picture */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_TAKE_PICTURE

DESCRIPTION
              The next frame is taken for encoding at the specified format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_take_picture
(
  camera_cb_f_type callback,
  void * client_data
)
{
#ifndef T_IO_CARD

  /* Terminate an existing function first */
  if ((camera_state != CAMERA_STATE_INIT) &&
      (camera_state != CAMERA_STATE_READY))
  {
    camera_log(LOG_TRM, 0, 0, (uint32)INV_STATE, __LINE__);
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
    MSG_HIGH("Stop previous command.", 0, 0, 0);
  }

  camera_app_cb = callback;
  camera_app_data = client_data;
  camera_func = CAMERA_FUNC_TAKE_PICTURE;
  camera_blt_ext_recalculate_window = TRUE;
  CAMERA_SLEEP_FORBID();
  CAMERA_HALT_FORBID();
  CAMERA_BUS_ON();

  /* Call the Hand Jitter Reduction Controller to decide on the
   * Gain, Exposure Time and No of frames to capture
   */
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Hand Jitter Disabled", 0, 0, 0);
    camera_process_take_single_picture();
  }

#endif /* nT_IO_CARD */
} /* camera_process_take_picture */

/*===========================================================================

FUNCTION: CAMERA_PREVIEW_SET_BUFFER_STATUS

DESCRIPTION:
  To keep track of which preview buffers are busy, meaning buffers that are
  used either by the VFE or display.
  Or, to clr the busy flag when the preview buffers are not used either by
  the display, or the VFE.
DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  None
============================================================================*/
static void camera_preview_set_buffer_status(uint8 buffer_index, camera_preview_buffer_status_type new_status)
{
  camera_preview_buffer_status_type old_status;
  old_status = camera_preview_buffers.buffers[buffer_index].status;
  if (buffer_index < CAMERA_NUM_OF_PREVIEW_BUFFERS)
  {
    if (new_status == BUSY_WITH_APP)
    {
      if (camera_preview_buffers.num_buffers_with_display < CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_DISPLAY)
      {
        camera_preview_buffers.num_buffers_with_display++;
      }
      else
      {
        MSG_ERROR("CAMERA_SVCS: Number of buffers with display is more than 2", 0, 0, 0);
        return;
      }
    }
    /* We do not modify num_buffers_with_display when changing to
     * BUSY_WITH_DISPLAY */
    else if (new_status == BUSY_WITH_VFE) /* meaning new_status = BUSY_WITH_VFE */
    {
      if (camera_preview_buffers.num_buffers_with_VFE < CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE)
      {
        camera_preview_buffers.num_buffers_with_VFE++;
      }
      else
      {
        MSG_ERROR("CAMERA_SVCS: Number of buffers with VFE is more than 3", 0, 0, 0);
        return;
      }
    }
    else if (new_status == AVAILABLE)
    {
      /* if buffer is previously with display */
      if (old_status == BUSY_WITH_APP || old_status == BUSY_WITH_DISPLAY)
      {
        if (camera_preview_buffers.num_buffers_with_display)
        {
          camera_preview_buffers.num_buffers_with_display--;
        }
        else
        {
          MSG_ERROR("CAMERA_SVCS: Number of buffers with display is negative", 0, 0, 0);
          return;
        }
      }
      else if (camera_preview_buffers.buffers[buffer_index].status == BUSY_WITH_VFE)
      {
        if (camera_preview_buffers.num_buffers_with_VFE)
        {
          camera_preview_buffers.num_buffers_with_VFE--;
        }
        else
        {
          MSG_ERROR("CAMERA_SVCS: Number of buffers with display is negative", 0, 0, 0);
          return;
        }
      }
    }
    camera_preview_buffers.buffers[buffer_index].status = new_status;
    MSG_CAMERAPBUF("PBUF SET status preview buffer %d new status=%d old status=%d",
        buffer_index, new_status, old_status);
    if (new_status == BUSY_WITH_APP)
    {
      if (camera_app_buf_index[0] == CAMERA_INVALID_BUF_INDEX)
      {
        camera_app_buf_index[0] = buffer_index;
        MSG_CAMERAPBUF("PBUF SET status, preview buffer %d is app0", buffer_index, 0, 0);
      }
      else
      {
        camera_app_buf_index[1] = buffer_index;
        MSG_CAMERAPBUF("PBUF SET status, preview buffer %d is app1", buffer_index, 0, 0);
      }
    }
  }
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_RELEASE_FRAME

DESCRIPTION
              Release frame for VFE reuse.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_release_frame (uint8 buffer_index)
{
  camera_preview_buffer_status_type status;
  /* If invalid, it is a release call from application. */
  if (buffer_index == CAMERA_INVALID_BUF_INDEX)
  {
    /* if the first application buffer is invalid, nothing to do. */
    if (camera_app_buf_index[0] == CAMERA_INVALID_BUF_INDEX)
    {
      MSG_CAMERAPBUF("PBUF process_release_frame, app0 is not valid", 0, 0, 0);
      return;
    }

    buffer_index = camera_app_buf_index[0];
    /* Move up the buffer index */
    camera_app_buf_index[0] = camera_app_buf_index[1];
    camera_app_buf_index[1] = CAMERA_INVALID_BUF_INDEX;
    status = camera_preview_buffers.buffers[buffer_index].status;
    /* if the first application buffer is not with application or display,
     * nothing to do */
    if ((status != BUSY_WITH_APP) && (status != BUSY_WITH_DISPLAY))
    {
      MSG_CAMERAPBUF("PBUF process_release_frame, preview buffer %d is not app0", buffer_index, 0, 0);
      return;
    }

    if(status == BUSY_WITH_DISPLAY)
    {
      /* If buffer with display, wait for display callback */
      MSG_CAMERAPBUF("PBUF process_release_frame, preview buffer %d is with display", buffer_index, 0, 0);
      return;
    }
  }
  /* Set the already displayed buffer to AVAILABLE and also to be the acknowledgement buffer */
  camera_preview_set_buffer_status(buffer_index, AVAILABLE);
  MSG_CAMERAPBUF("PBUF process_release_frame, SET buffer %d AVAILABLE",
      buffer_index, 0, 0);
} /* camera_process_release_frame */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_BLT

DESCRIPTION
              Post processing of BLT function. Change buffer state to
              busy_with_display.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_blt (uint8 buffer_index)
{
  if (camera_preview_buffers.buffers[buffer_index].status == BUSY_WITH_APP)
  {
    camera_preview_set_buffer_status(buffer_index, BUSY_WITH_DISPLAY);
    MSG_CAMERAPBUF("PBUF process_blt, SET buffer %d BUSY_WITH_DISPLAY", buffer_index, 0, 0);
  }
} /* camera_process_blt */

/*===========================================================================

FUNCTION      CAMERA_COPY_TO_FILE

DESCRIPTION
              The raw picture is copied to the specified file.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/

void camera_copy_to_file(uint32 size, uint8 *buffer, uint32 size2, uint8 *buffer2)
{
#ifndef T_IO_CARD
  fs_open_xparms_type open_parms;
  char *copyfilename = NULL;
  copyfilename = camera_handle.efs.filename;

 if(copyfilename != NULL)
 {
    fs_remove(copyfilename, NULL, &fs_rsp);
    open_parms.create.cleanup_option = FS_OC_CLOSE;
    open_parms.create.buffering_option = FS_OB_PROHIBIT;
    fs_open(copyfilename, FS_OA_CREATE, &open_parms, NULL, &fs_rsp);
    if (fs_rsp.open.status != FS_OKAY_S)
    {
      efs_fail_count++;
      camera_log(LOG_INFO, 0, 0, (uint32)EFS_FAIL, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate(CAMERA_EXIT_CB_ERROR, 0);
      MSG_ERROR("EFS failed. %d State transition to ready", efs_fail_count, 0, 0);
      event_report(EVENT_CAMERA_EFS_FAILED);
      return;
    }
    camera_fs_handle = fs_rsp.open.handle;
  
    fs_write(camera_fs_handle, buffer, size, NULL, &fs_rsp);
    if (fs_rsp.write.count != size || fs_rsp.write.status != FS_OKAY_S)
    {
      efs_fail_count++;
      camera_log(LOG_INFO, 0, 0, (uint32)EFS_FAIL, __LINE__);
      camera_terminate(CAMERA_EXIT_CB_ERROR, 0);
      MSG_ERROR("EFS failed to wr data. %d %d State transition to ready", fs_rsp.write.count, size, 0);
      event_report(EVENT_CAMERA_EFS_FAILED);
      fs_close(camera_fs_handle, NULL, &fs_rsp);
      camera_fs_handle = 0;
      return;
    }
  
    if(buffer2!=NULL)
    {
      fs_seek_write(camera_fs_handle, FS_SEEK_SET, size, buffer2, size2, NULL, &fs_rsp);
  
      if (fs_rsp.seek_write.count != size2 || fs_rsp.seek_write.status != FS_OKAY_S)
      {
        efs_fail_count++;
        camera_log(LOG_INFO, 0, 0, (uint32)EFS_FAIL, __LINE__);
        camera_terminate(CAMERA_EXIT_CB_ERROR, 0);
        MSG_ERROR("EFS failed to append data. %d %d State transition to ready", fs_rsp.seek_write.count, size2, 0);
        event_report(EVENT_CAMERA_EFS_FAILED);
        fs_close(camera_fs_handle, NULL, &fs_rsp);
        camera_fs_handle = 0;
        return;
      }
    }
  
    fs_close(camera_fs_handle, NULL, &fs_rsp);
    camera_fs_handle = 0;
    event_report_payload (EVENT_CAMERA_PICTURE_SAVED, (uint8) strlen (copyfilename), copyfilename);
   }
#endif /* nT_IO_CARD */
} /* camera_copy_to_file */

/*===========================================================================

FUNCTION      CAMERA_ENCODE_PICTURE_HANDLER

DESCRIPTION
              The picture is encoded to the specified file name or copied
              into client memory.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
void camera_encode_picture_handler
(
  qcamrawHeaderType *main_image_header,
  uint8             *main_image_buffer,
  qcamrawHeaderType *thumbnail_image_header,
  uint8             *thumbnail_image_buffer
)
{

#ifdef FEATURE_PNG_ENCODER
  /* If it is PNG encoding then prepare for PNG encoding */
  /* PNG encoding is only ARM base */
  /* PNG encoder is using GRAPH thread. If Camera using GRAPH thread to start    */
  /* the PNG encoder then it should release the thread for the encoder to process */
  if (camera_encode_properties.format == CAMERA_PNG)
  {

    png_encode_spec.input_data = RGB565;
    /* Set up essential parameters */
    png_encode_spec.input_ptr    = (uint8 *) main_image_buffer ;
    png_encode_spec.chroma_ptr   = main_image_header->chromaBuffer;
    png_encode_spec.image_width  = main_image_header->picWidth;
    png_encode_spec.image_height = main_image_header->picHeight;
    png_encode_spec.bayer_image_width  = main_image_header->picWidth;
    png_encode_spec.bayer_image_height = main_image_header->picHeight;
#ifdef CAMERA_USES_LIBSTD
    (void) std_strlcpy(png_encode_spec.filename, camera_handle.efs.filename,
                  FS_FILENAME_MAX_LENGTH_P - 1 );
#else /* CAMERA_USES_LIBSTD */
    (void) strcpy(png_encode_spec.filename, camera_handle.efs.filename);
#endif /* CAMERA_USES_LIBSTD */
    if (camera_handle.device == CAMERA_DEVICE_EFS)
    {
       png_handle.device = PNGE_DEVICE_EFS;
       png_handle.fs_rsp = fs_rsp;
    }
    else if (camera_handle.device == CAMERA_DEVICE_MEM)
    {
       png_handle.device = PNGE_DEVICE_MEM;
       png_handle.buffer = camera_handle.mem.buffer;
       png_handle.length = camera_handle.mem.length;
    }

    if ( pnge_encode (png_encode_spec, &png_handle, camera_PNG_CB ) != PNGE_SUCCESS)
    {
        MSG_ERROR(" Can't encode the image", 0, 0, 0);
        camera_clr_take_picture ();
        camera_terminate(CAMERA_EXIT_CB_FAILED, 0);
    }
    else
    {
      camera_take_picture_status.encode_in_progress = TRUE;
      /* if queueing command successfully then notify the apps */
      if (camera_app_cb)
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        (camera_app_cb)(CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
        MSG_HIGH("Picture saved. State transition to ready", 0, 0, 0);
        event_report_payload (EVENT_CAMERA_PICTURE_SAVED, 0, 0);
      }
    }

  }
#endif /* FEATURE_PNG_ENCODER */
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_ENCODE_PICTURE_STANDALONE

DESCRIPTION
              The picture is encoded to the specified file name or copied
              into client memory.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_encode_picture_standalone
(
  qcamrawHeaderType *main_image_header,
  uint8             *main_image_buffer,
  qcamrawHeaderType *thumbnail_image_header,
  uint8             *thumbnail_image_buffer
)
{
#ifndef T_IO_CARD
  camera_func               = CAMERA_FUNC_ENCODE_PICTURE;
  camera_abort_picture      = FALSE;
  camera_take_picture_status.encode_in_progress = FALSE;

  CAMERA_SLEEP_FORBID();
  CAMERA_HALT_ALLOW();
  CAMERA_BUS_ON();
  MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_ENCODE_PICTURE", 0, 0, 0);
  camera_state = CAMERA_STATE_ENCODE_PICTURE;
  camera_svcs_optimize_resources();

  MSG_HIGH("camera_process_encode_picture", 0, 0, 0);
  event_report(EVENT_CAMERA_SAVE_PICTURE);


  if (camera_app_cb)
  {
    MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
    (camera_app_cb)(CAMERA_RSP_CB_SUCCESS, camera_app_data, camera_func, 0);
  }

  /* If Raw image store to file */
  if (camera_encode_properties.format == CAMERA_RAW)
  {
    /* Store to EFS */
    camera_copy_to_file (main_image_header->YSize, main_image_buffer, 0, NULL);
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
    MSG_HIGH("Picture saved. State transition to ready", 0, 0, 0);
    return;
  }

  /* If YCbCr image store it to the file */
  if (camera_encode_properties.format == CAMERA_YCBCR_ENCODE) 
  {
    /* Store to EFS */
    camera_copy_to_file (main_image_header->YSize, 
                         (uint8*)main_image_buffer, main_image_header->CbCrSize, 
                         (uint8*)main_image_header->chromaBuffer);

    if (camera_app_cb)
    {
      (camera_app_cb)( CAMERA_EXIT_CB_DONE, camera_app_data, camera_func, 0);
    }
    CAM_Free (snapshot_luma_buf);
    snapshot_luma_buf = NULL;
    return;
  }
  
  {
     camera_encode_picture_handler(main_image_header, main_image_buffer, thumbnail_image_header, thumbnail_image_buffer);
  }

#endif /* nT_IO_CARD */
} /* camera_process_encode_picture_standalone */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_ENCODE_PICTURE

DESCRIPTION
              The picture is encoded to the specified file name or copied
              into client memory.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_encode_picture
(
  camera_cb_f_type   callback,
  void              *client_data,
  qcamrawHeaderType *main_image_header,
  uint8             *main_image_buffer,
  qcamrawHeaderType *thumbnail_image_header,
  uint8             *thumbnail_image_buffer
)
{
#ifndef T_IO_CARD

  /* Terminate an existing function first */
  if ((camera_state != CAMERA_STATE_INIT) &&
      (camera_state != CAMERA_STATE_READY)
     )
  {
    MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
    MSG_HIGH("Stop previous command.", 0, 0, 0);
  }

  camera_app_cb    = callback;
  camera_app_data  = client_data;

  camera_process_encode_picture_standalone ( main_image_header, main_image_buffer,
                                             thumbnail_image_header, thumbnail_image_buffer
                                           );
#endif /* nT_IO_CARD */
} /* camera_process_encode_picture */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_ENABLE_FRAME_CALLBACK

DESCRIPTION
              Process enable frame callback.

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview or start_record callback.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_enable_frame_callback (void)
{
  camera_frame_callback_enabled = TRUE;

  if (camera_app_cb)
  {
    MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
    (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_ENABLE_FRAME_CALLBACK, 0);
  }
} /* camera_process_enable_frame_callback */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_DISABLE_FRAME_CALLBACK

DESCRIPTION
              process disable frame callback.

DEPENDENCIES
  Must be in PERVIEW or RECORDING state
  Use start_preview or start_record callback.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_disable_frame_callback (void)
{
  camera_frame_callback_enabled = FALSE;

  if (camera_app_cb)
  {
    MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
    (camera_app_cb) (CAMERA_RSP_CB_SUCCESS, camera_app_data, CAMERA_FUNC_DISABLE_FRAME_CALLBACK, 0);
  }
} /* camera_process_disable_frame_callback */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_OVERLAY

DESCRIPTION
              Process set overlay

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_set_overlay
(
  camera_cb_f_type callback,
  void            *client_data,
  rex_tcb_type    *tcb
)
{
  camera_cb_type cb;
  camera_ret_code_type ret_val;
  ret_val = CAMERA_NOT_SUPPORTED;
  if (tcb)
  {
    (void) rex_set_sigs (tcb, CAMERA_SIG);
  }

  if (callback)
  {
    cb = camera_svcs_translate_ret_val_to_cb(ret_val);
    (callback)(cb, client_data, CAMERA_FUNC_SET_OVERLAY, NULL);
  }
} /* camera_process_set_overlay */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_CLR_OVERLAY

DESCRIPTION
              Process clear overlay

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_clr_overlay
(
  camera_cb_f_type callback,
  void            *client_data
)
{
  if (callback)
  {
    (callback) (CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_CLR_OVERLAY, 0);
  }
} /* camera_process_clr_overlay */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_REFLECT

DESCRIPTION
              Process set_reflect

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_set_reflect (void)
{
  if (camera_parm_reflect.current_value == (int)CAMERA_MIRROR_REFLECT)
  {
    camera_reflect = QCAMRAW_MIRROR_REFLECT;
    if (camera_reflect_ipl != IPL_YREF)
    {
      /* Need to apply the reflection to the overlay frame only for preview*/
      camera_reflect_ipl = IPL_YREF;
    }
  }
  else if (camera_parm_reflect.current_value == (int)CAMERA_WATER_REFLECT)
  {
    camera_reflect = QCAMRAW_WATER_REFLECT;
    if (camera_reflect_ipl != IPL_XREF)
    {
      camera_reflect_ipl = IPL_XREF;
    }
  }
  else
  {
    camera_reflect = QCAMRAW_NO_REFLECT;
    camera_reflect_ipl = IPL_NOREF;
  }

  camera_set_preview_headers();
  return;
} /* camera_process_set_reflect */

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SET_DIMENSIONS

DESCRIPTION
              Process set_dimensions

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_set_dimensions ( camera_cb_f_type callback, void *client_data, rex_tcb_type *tcb)
{
  /* This function will check the overlay frame dimension and clear them if they
   * do not match with the new dimensions
   */
  if (callback)
  {
    (callback) (CAMERA_EXIT_CB_DONE, client_data, CAMERA_FUNC_SET_DIMENSIONS, 0);
  }

  MSG_HIGH("Camera Set Dimensions calling UI",0,0,0);

  if(tcb)
  {
    (void) rex_set_sigs(tcb, CAMERA_SIG);
  }

} /* camera_process_set_dimensions */

/*===========================================================================

FUNCTION      CAMERA_TERMINATE

DESCRIPTION
  Bring the VFE to Idle state.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void camera_terminate (camera_cb_type cb, int32 parm4)
{
#ifndef T_IO_CARD
  if(!camera_stopping)
  {
    camera_stopping = TRUE;
    camera_stopping_cb = cb;
    camera_parm4_value = parm4;
  }
  else
  {
    MSG_ERROR("CAMERA_SVCS: camera terminate called while stopping", 0, 0, 0);
  }
  camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_START, __LINE__);
  camera_process_terminate();
#endif /* nT_IO_CARD */
} /* camera_terminate */

/*===========================================================================

FUNCTION      CAMERA_SVCS_ACK_SOFTDSP_OUTPUT1

DESCRIPTION
              Send VFE acknowledgement during preview

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_ack_softdsp_output1 (void)
{
  uint32 current_buffer;
  
  /* Check if VFE has less than 3 buffers, if it does, send acknowledgement */
  if (camera_preview_buffers.num_buffers_with_VFE < CAMERA_NUM_OF_PREVIEW_BUFFERS_WITH_VFE)
  {
    /* select the available buffer if the display has not released frame yet */
    for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
    {
      if (camera_preview_buffers.buffers[current_buffer].status == AVAILABLE)
      {
        MSG_CAMERADEBUG("camera_svcs_ack_softdsp_output1 0x%x 0x%x", current_buffer, camera_preview_buffers.buffers[current_buffer].buf_ptr, 0);
        SoftDSP_PushPreviewBuff(camera_preview_buffers.buffers[current_buffer].buf_ptr+qcamraw_header_size);
        camera_preview_set_buffer_status(current_buffer, BUSY_WITH_VFE);
        break;
      }
    }
  }
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_SOFTDSP_OUTPUT1_MSG

DESCRIPTION
  Data along output1 path. Preview uses output1 path for display.
  Handles QDSP events in graph task context. In the snapshot mode,
  thumbnail is configured on output1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void  camera_process_softdsp_output1_msg (Camera_EndOfFrameMessageType *msg)
{
  CAMSoftDSP_EndOfFrameMessageType *eof_msg = &msg->eof_msg;
  camera_frame_type frame;
  
  qcamrawHeaderType *header = (qcamrawHeaderType *)
    ((uint32)eof_msg->pBuff - qcamraw_header_size);

  MSG_CAMERADEBUG("softdsp_output1_msg %d 0x%x 0x%x", camera_state, eof_msg->pBuff, header);
  if ((camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) ||
      (camera_stopping)) 
  {
    MSG_ERROR("CAMERA_SVCS: Output1 Msg received in camera_state %d, camera_dsp_state %d. Ignoring",
              camera_state, camera_dsp_state,0);
    return;
  }
  
  if(camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
  {
    MSG_ERROR("CAMERA_SVCS: Output1 Msg received in camera_camsensor_op_mode %d.Ignoring", camera_camsensor_op_mode ,0,0);
	return;
  } 
  
  camera_preview_set_buffer_status(header->buffer_index, AVAILABLE);
  
  if (camera_abort_picture)
  {
    /* To reconfigure or abort, go back to idle first */
    if (SoftDSP_Stop () != 0)
    {
      camera_terminate (CAMERA_EXIT_CB_DSP_ABORT, (int)CAMERA_ERROR_CONFIG);
      event_report (EVENT_CAMERA_CANNOT_IDLE_DSP);
    }
    return;
  }
  if (camera_state == CAMERA_STATE_PREVIEW)
  {
    if (camera_frame_callback_enabled && camera_app_cb)
    {
      /* Callback is enabled and callback present, do callback if
       * display has less than 2 buffers at the moment.
       */
      frame.format   = camera_preview_format;
      frame.dx       = camera_preview_dx;
      frame.dy       = camera_preview_dy;
      frame.rotation = 0;
      frame.buffer   = (byte *) ((uint32) eof_msg->pBuff - qcamraw_header_size);
      
      camera_preview_set_buffer_status (header->buffer_index, BUSY_WITH_APP);
      (camera_app_cb) (CAMERA_EVT_CB_FRAME, camera_app_data, camera_func,
                       (int32) (&frame));
    }
    camera_svcs_ack_softdsp_output1();
  }
} /*  camera_process_softdsp_output1_msg */


/*===========================================================================

FUNCTION      CAMERA_PROCESS_SoftDSP_OUTPUT2_MSG

DESCRIPTION
              Handles QDSP events in graph task context.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_softdsp_output2_msg (Camera_EndOfFrameMessageType *msg)
{
  //CAMSoftDSP_EndOfFrameMessageType *eof_msg = &msg->eof_msg;
  
  if ((camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) ||
      (camera_stopping))
  {
    MSG_ERROR("CAMERA_SVCS: Output2 Msg received in camera_state %d, camera_dsp_state %d. Ignoring",
              camera_state, camera_dsp_state, 0);
    return;
  }
  MSG_CAMERADEBUG("softdsp_output2_msg %d %d", camera_state, camera_camsensor_op_mode, 0);
  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
  {
    /* main image */
    snapshot_frame.format       = camera_snapshot_format;
    snapshot_frame.captured_dx  = camera_dx;
    snapshot_frame.captured_dy  = camera_dy;
    snapshot_frame.thumbnail_image = NULL;
    snapshot_frame.buffer       = (byte *) (snapshot_luma_buf);
    
    if (
        (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT) ||
        (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
       )
    {
      if (camera_take_picture_status.abort)
      {
        camera_svcs_queue_call_to_terminate();
      }
      else
      {
        camera_handle_frames_for_takepicture();
      }
    }
  }
} /* camera_process_Softdsp_output2_msg */

static void camera_process_softdsp_illegal_cmd_msg (void)
{
  event_report (EVENT_CAMERA_DSP_REQ_ILLEGAL);
  camera_terminate(CAMERA_EXIT_CB_FAILED, 0);
} /* camera_process_qdsp_illegal_cmd_msg */

/*===========================================================================

FUNCTION      CAMERA_SET_PREVIEW_BUFFERS

DESCRIPTION
              Set preview (and video mode) buffers.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

boolean camera_set_preview_buffers(void)
{
  boolean malloc_fail = FALSE;
  uint32 current_buffer;
  MSG_CAMERADEBUG("CAMERA_SVCS: camera_set_preview_buffers", 0, 0, 0);
  camera_free_preview_buffers();
  camera_malloc_preview_buffers();

  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    malloc_fail = (malloc_fail || !camera_preview_buffers.buffers[current_buffer].buf_ptr);
  }
  if (malloc_fail)
  {
    camera_free_preview_buffers();
    camera_no_memory ();
  }
  return malloc_fail;
} /* camera_set_preview_buffers */

/*===========================================================================

FUNCTION      CAMERA_SET_PREVIEW_HEADERS

DESCRIPTION
              Set the raw data format header for the preview frame.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void camera_set_preview_headers(void)
{
  uint16 pic_dx, pic_dy;
  qcamrawDataType data_type;
  uint32 y_size = 0, y_pad = 0, cbcrsize = 0;
  uint32 y_size_encode = 0, y_pad_encode = 0, cbcrsize_encode =0;
  uint16  rotation;
  uint32 current_buffer;

  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    if (camera_preview_buffers.buffers[current_buffer].buf_ptr == NULL)
    {
      return;
    }
  }

  pic_dx = (uint16) camera_preview_dx;
  pic_dy = (uint16) camera_preview_dy;

  /* In preview state, set data format based on camera_preview_format */
  switch (camera_preview_format)
  {
  case CAMERA_RGB565:
    data_type = QCAMRAW_RGB565;
    y_size    = camera_preview_dx * camera_preview_dy;
    y_pad     = y_size % 4;
    cbcrsize  = camera_preview_dx * camera_preview_dy;

    y_size_encode   = camera_dx * camera_dy;
    cbcrsize_encode = camera_dx * camera_dy;
    y_pad_encode    = y_size_encode % 4;
    break;

  default:
     MSG_ERROR ("Unsupported preview format %d.", camera_preview_format, 0, 0);
     return;
  }

  rotation = 0;
  
  for (current_buffer = 0; current_buffer < CAMERA_NUM_OF_PREVIEW_BUFFERS; current_buffer++)
  {
    qcamrawSetHeader((qcamrawHeaderType *)camera_preview_buffers.buffers[current_buffer].buf_ptr,
                      current_buffer, camera_preview_buffer_size, data_type, QCAMRAW_PREVIEW_FRAME,
                      camera_reflect, rotation, pic_dx, pic_dy, (uint16)camera_preview_dx,
                     (uint16)camera_preview_dy, 0, 0, 0, 0, y_size, y_pad, cbcrsize,
                     (uint8 *)((uint32)camera_preview_buffers.buffers[current_buffer].buf_ptr +
                               qcamrawHeaderSize + 0 + y_size + y_pad));
  }
} /* camera_set_preview_header */

/*===========================================================================

FUNCTION      CAMERA_NO_MEMORY

DESCRIPTION
              Terminate due to no memory.
              Should only be called in post processing after callback
              and client data are saved.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void camera_no_memory(void)
{
  MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
  camera_terminate (CAMERA_EXIT_CB_ERROR, (int)CAMERA_ERROR_NO_MEMORY);
  MSG_ERROR ("No memory. State transition to ready", 0, 0, 0);
  event_report (EVENT_CAMERA_NO_MEMORY);
} /* camera_no_memory */

void camera_softdsp_cb (CAMSoftDSP_ResponseType *response)
{
  switch (response->responseMsg)
  {
  case CAMSOFTDSP_MSG_START_OF_FRAME:
    graph_queue_camqdsp_msg (response->responseMsg, 0, 0);
    break;
    
  case CAMSOFTDSP_MSG_OUTPUT1_END_OF_FRAME:
    /* Reset so error can be reported for the next frame */
    endOfFrameMessage[0].eof_msg = response->responsePayload.endOfFrame;
    endOfFrameMessage[0].time = timetick_get_ms();
    //graph_queue_camqdsp_msg (response->responseMsg, (void *) &endOfFrameMessage[0], 0);
    camera_process_softdsp_output1_msg(&endOfFrameMessage[0]);
    break;
    
  case CAMSOFTDSP_MSG_OUTPUT2_END_OF_FRAME:
    endOfFrameMessage[1].eof_msg = response->responsePayload.endOfFrame;
    endOfFrameMessage[1].time = timetick_get_ms();
    //graph_queue_camqdsp_msg (response->responseMsg, (void *) &endOfFrameMessage[1], 0);
    camera_process_softdsp_output2_msg(&endOfFrameMessage[1]);
    break;
    
  case CAMSOFTDSP_MSG_ILLEGAL_COMMAND:
     illegalCmdMessage = response->responsePayload.illegalCmd;
     graph_queue_camqdsp_msg (response->responseMsg, (void *) &illegalCmdMessage, 0);
     break;
     
  default:
     graph_queue_camqdsp_msg (response->responseMsg, 0, 0);
  }
} /* camera_softdsp_cb */

#ifdef FEATURE_PNG_ENCODER
/*===========================================================================

FUNCTION      CAMERA_PNG_CB

DESCRIPTION
              A designated callback function for the PNG encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_PNG_CB (PNGE_status_enum_type command, void *handle, void *pnge_data)
{
  camera_snapshot_timing.encode_end = (uint32)timetick_get_ms();
  camera_snapshot_timing.snapshot_completion_callback_start = (uint32)timetick_get_ms();

  camera_clr_take_picture ();

  camera_take_picture_status.encode_in_progress = FALSE;

  if (camera_take_picture_status.abort)
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: PNG_CB Queue STOP_STANDALONE", 0, 0, 0);
    camera_svcs_queue_call_to_terminate();
  }
  else
  {
    camera_terminate(CAMERA_EXIT_CB_DONE, 0);
  }
}
/*lint -restore */
/*===========================================================================

FUNCTION      CAMERA_PROCESS_PNGE_MSG

DESCRIPTION
              Process all messages fr PNG encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camera_svcs_process_pnge_msg(uint32 msg)
{
  PNGE_status_enum_type command = (PNGE_status_enum_type) msg;

#ifdef TEST_PNGE_MEM_DEVICE
  fs_rsp_msg_type     temp_fs_resp;
#endif /* TEST_PNGE_MEM_DEVICE */

  camera_take_picture_status.encode_in_progress = FALSE;

  switch (command)
  {
    case PNGE_CB_DONE:

      /* Pass back the size of the new encoded PNG image to the app */
      // CAN'T DO THIS
      //camera_app_data = (void *) png_handle.length;

      // Some operation in this function is essential, else the
      // phone will error fatal when trying enter another menu (like
      // config menu) after doing PNG encoding
      // Will also transition to CAMERA_STATE_READY
      camera_log(LOG_TRM, 0, 0, (uint32)PIC_SAVED, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate(CAMERA_EXIT_CB_DONE, 0);

      MSG_LOW("PNG encode done", 0, 0, 0);
      break;

    case PNGE_CB_ABORT:

      // Some operation in this function is essential, else the
      // phone will error fatal when trying enter another menu (like
      // config menu) after doing PNG encoding
      // Will also transition to CAMERA_STATE_READY
      camera_log(LOG_TRM, 0, 0, (uint32)ABORT, __LINE__);
      MSG_CAMERADEBUG("camera_terminate called", 0, 0, 0);
      camera_terminate(CAMERA_EXIT_CB_ABORT, 0);

      MSG_LOW("PNG encoder abort", 0, 0, 0);
      break;

    default:
      MSG_HIGH("PNG encoder - unknown message", 0, 0, 0);
  }
}

#endif /* FEATURE_PNG_ENCODER */


/*===========================================================================

FUNCTION      CAMERA_SET_ENCODE_PROPERTIES

DESCRIPTION
              Set encode properties.

              If encode properties had changed, this function must be called
              before camera_take_picture is called.

              This function is synchronous and is executed in the caller's
              context.

DEPENDENCIES
  Must be in READY state.

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_set_encode_properties
(
  camera_encode_properties_type  *encode_properties
)
{
  camera_encode_properties = *encode_properties;

  return CAMERA_SUCCESS;
} /* camera_set_encode_properties */

void camera_svcs_init_self (camerai_func_type *funcTable)
{
  funcTable->camera_init                       = camera_svcs_init;
  funcTable->camera_start                      = camera_svcs_start;
  funcTable->camera_stop                       = camera_svcs_stop;
  funcTable->camera_set_dimensions             = camera_svcs_set_dimensions;
  funcTable->camera_start_preview              = camera_svcs_start_preview;
  funcTable->camera_stop_preview               = camera_svcs_stop_preview;
  funcTable->camera_take_picture               = camera_svcs_take_picture;
  funcTable->camera_encode_picture             = camera_svcs_encode_picture;
  funcTable->camera_get_thumbnail_properties   = camera_svcs_get_thumbnail_properties;
  funcTable->camera_set_thumbnail_properties   = camera_svcs_set_thumbnail_properties;
  funcTable->camera_start_focus                = NULL;
  funcTable->camera_stop_focus                 = NULL;
  funcTable->camera_enable_frame_callback      = camera_svcs_enable_frame_callback;
  funcTable->camera_disable_frame_callback     = camera_svcs_disable_frame_callback;
  funcTable->camera_release_frame              = camera_svcs_release_frame;
  funcTable->camera_set_overlay                = NULL;
  funcTable->camera_clr_overlay                = NULL;
  funcTable->camera_set_position               = camera_svcs_set_position;
  funcTable->camera_set_exif_tag               = camera_svcs_set_exif_tag;
  funcTable->camera_blt                        = camera_svcs_blt;
  funcTable->camera_blt_ext                    = camera_svcs_blt_ext;
  funcTable->camera_set_parm                   = camera_svcs_set_parm;
  funcTable->camera_set_parm_2                 = camera_svcs_set_parm_2;
  funcTable->camera_get_info                   = camera_svcs_get_info;
  funcTable->camera_get_parm                   = camera_svcs_get_parm;
  funcTable->camera_get_parm_2                 = camera_svcs_get_parm_2;
  funcTable->camera_process_func               = camera_svcs_process_func;
  funcTable->camera_process_qdsp_msg           = camera_svcs_process_qdsp_msg;
  funcTable->camera_release_picture            = camera_svcs_release_picture;
#ifdef FEATURE_PNG_ENCODER
  funcTable->camera_process_pnge_msg           = camera_svcs_process_pnge_msg;
#endif /* FEATURE_PNG_ENCODER */
} /* camera_svcs_init_self */


/*===========================================================================

FUNCTION      CAMERA_SVCS_SWAP_DIMENSIONS

DESCRIPTION
              swap the values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_swap_dimensions (uint16 *x, uint16 *y)
{
  *x ^= *y;
  *y ^= *x;
  *x ^= *y;
} /* camera_svcs_swap_dimensions */

/*===========================================================================

FUNCTION      CAMERA_CLR_TAKE_PICTURE

DESCRIPTION
              Clear take picture variable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_clr_take_picture()
{
  CAM_Free (snapshot_luma_buf);
  snapshot_luma_buf = NULL;
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_HANDLE_ERROR

DESCRIPTION
              Error Handler.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_process_handle_error ()
{
  MSG_CAMERADEBUG("Error Handler: camera_state:%d camera_stopping:%d", camera_state, camera_stopping, 0);

  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
  {
    if (camera_take_picture_status.abort)
    {
      camera_svcs_queue_call_to_terminate();
    }
    else if (!camera_stopping)
    {
      if (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT||
          camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT
         )
      {
        camera_handle_frames_for_takepicture();
      }
      else
      {
        camera_clr_take_picture();
        camera_terminate(CAMERA_EXIT_CB_FAILED, 0);
      }
    }
  }
}

/*===========================================================================

FUNCTION      CAMERA_HANDLE_FRAMES_FOR_TAKEPICTURE

DESCRIPTION
              Handle frames for take picture.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_handle_frames_for_takepicture()
{
  if (camera_state == CAMERA_STATE_TAKE_PICTURE)
  {
    if (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_NEEDED)
    {
      {
        camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_IDLING;
      }
      return;
    }
    else if (camera_take_picture_retry == CAMERA_TAKE_PICTURE_RETRY_INP)
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Take picture retry done", 0, 0, 0);
      camera_take_picture_retry = CAMERA_TAKE_PICTURE_RETRY_DONE;
    }
  }
  
  {
     MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
     camera_state = CAMERA_STATE_READY;
     MSG_HIGH("Picture taken. State transition to ready", 0, 0, 0);
     event_report(EVENT_CAMERA_PICTURE_TAKEN);
  }

  if (camera_app_cb)
  {
    if (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_SNAPSHOT)
    {
      /* wait for memory transfer to complete */
      if (offline_read_wait)
      {
        camera_timed_wait(offline_read_wait);
        offline_read_wait = 0;
      }
      {
        if ( camera_snapshot_timing.offline_snapshot_capture_start != 0)
        {
          /* It is offline snapshot */
          camera_snapshot_timing.offline_snapshot_capture_end = (uint32)timetick_get_ms();
        }
        else
        {
          /* It is regular snapshot */
          camera_snapshot_timing.snapshot_capture_end = (uint32)timetick_get_ms();
        }
        
        camera_terminate(CAMERA_EXIT_CB_DONE, (int32) &snapshot_frame);
        /* reset lens */
        if (camctrl_tbl.reset_lens_after_snap)
        {
          camera_drv_queue_reset_lens(NULL, 0, 0, 0, 0, 0);
        }
      }
    }
    else if (camera_camsensor_op_mode == CAMSENSOR_OP_MODE_RAW_SNAPSHOT)
    {
      camera_snapshot_timing.raw_capture_end = (uint32)timetick_get_ms();
      {
        MSG_CAMERADEBUG("Camera_Svcs-->UI camera_func:%d", camera_func, 0, 0);
        camera_terminate(CAMERA_EXIT_CB_DONE, (int32) &snapshot_frame);
        /* reset lens */
        if (camctrl_tbl.reset_lens_after_snap)
        {
          camera_drv_queue_reset_lens(NULL, 0, 0, 0, 0, 0);
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION: CAMERA_SVCS_OPTIMIZE_RESOURCES

DESCRIPTION:
  Adjust the resources available

DEPENDENCIES:

SIDE EFFECTS:
  None

RETURN VALUE:
  Void

============================================================================*/
static void camera_svcs_optimize_resources (void)
{
#if (defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III||defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE|| \
     defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_II)|| defined FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV && \
    (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING   || \
     defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)
    switch (camera_state) {
    case CAMERA_STATE_PREVIEW:
        if (camera_clk_resource != CAMERA_CLK_RESOURCE_VIEWFINDER) {
          #ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III /* SC2X */
            if (camsensor_static_params[camera_asi].output_format == CAMSENSOR_YCBCR) {
                if ((camsensor_static_params[camera_asi].preview_fps <= (15*Q8)) \
                   || (camsensor_static_params[camera_asi].max_video_fps <=(15*Q8))) {
                    CAMERA_SET_CLK_VIEWFINDER();
                } else {
                    CAMERA_SET_CLK_VIEWFINDER_HIGH();
                }
            }

            else {
                #ifndef FEATURE_CAMERA_YCBCR_ONLY
                if ((cam3a_aec_state.afr_max_fps <= (15*Q8)) \
                   || (camsensor_static_params[camera_asi].max_video_fps <=(15*Q8))) {
                    CAMERA_SET_CLK_VIEWFINDER();
                } else {
                    CAMERA_SET_CLK_VIEWFINDER_HIGH();
                }
               #endif /* FEATURE_CAMERA_YCBCR_ONLY */

             }
            camera_clk_resource = CAMERA_CLK_RESOURCE_VIEWFINDER;
           #else
              CAMERA_SET_CLK_VIEWFINDER();
              camera_clk_resource = CAMERA_CLK_RESOURCE_VIEWFINDER;
           #endif
      }
           
      /* if the pixel clock for sensor is too high, when the real time dynamic pclk value is available
       * should switch to using that one instead of this static variable */
    #ifndef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III /* SC2x */
        if ((clk_regime_msm_get_clk_freq_khz(CLK_RGM_HCLK_M) < camsensor_static_params[camera_asi].pixel_clock / 1000) ) {
        CAMERA_SET_CLK_VIEWFINDER_HIGH();
      }
    #endif
      break;

    case CAMERA_STATE_TAKE_PICTURE:
    case CAMERA_STATE_ENCODE_PICTURE:
    #ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
        CAMERA_SET_CLK_SNAPSHOT();
    #else
        if(camera_clk_resource != CAMERA_CLK_RESOURCE_SNAPSHOT)
         {
            CAMERA_SET_CLK_SNAPSHOT();
            camera_clk_resource = CAMERA_CLK_RESOURCE_SNAPSHOT;
         }
    #endif
      break;
    case CAMERA_STATE_READY:
        if (camera_clk_resource != CAMERA_CLK_RESOURCE_DEFAULT) {
    #ifdef FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III
        CAMERA_SET_CLK_DEFAULT();
        CAMERA_SET_CLK_HIGH_DEFAULT();
        camera_clk_resource = CAMERA_CLK_RESOURCE_DEFAULT;
    #else
        CAMERA_SET_CLK_DEFAULT();
    #if defined (FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE) && defined (QDSP_MODULE_VFE20_DEFINED)
        CAMERA_SET_CLK_HIGH_DEFAULT();
    #endif /*FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE && QDSP_MODULE_VFE20_DEFINED*/
        camera_clk_resource = CAMERA_CLK_RESOURCE_DEFAULT;
    #endif
      }
      break;
    default:
      break;
  }
#endif /* (defined(FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III)||defined (FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE  ||
                   FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE) || FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_IV &&
         (defined FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING ||
          defined FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING) */

} /* camera_svcs_optimize_resources */

static camera_cb_type camera_svcs_translate_ret_val_to_cb
(
 camera_ret_code_type ret_val
)
{
  camera_cb_type cb;
  switch (ret_val)
  {
    case CAMERA_SUCCESS:
      cb = CAMERA_EXIT_CB_DONE;
      break;
    case CAMERA_NO_MEMORY:
    case CAMERA_FAILED:
    case CAMERA_INVALID_STAND_ALONE_FORMAT:
    case CAMERA_MALLOC_FAILED_STAND_ALONE:
      cb = CAMERA_EXIT_CB_FAILED;
      break;
    case CAMERA_NOT_SUPPORTED:
    case CAMERA_INVALID_STATE:
    case CAMERA_INVALID_PARM:
    case CAMERA_INVALID_FORMAT:
    case CAMERA_NO_SENSOR:
    default:
      cb = CAMERA_EXIT_CB_ERROR;
      break;
  }
  return cb;
}
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_SVCS_QUEUE_CALL_TO_TERMINATE

DESCRIPTION
              The camera operation is stopped in graph context.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camera_svcs_queue_call_to_terminate(void)
{
  camera_take_picture_status.abort                     = FALSE;
  
  MSG_CAMERADEBUG("CAMERA_SVCS: Queuing camera_process_terminate to graph task", 0, 0, 0);
  graph_queue_camera_func ((uint32)CAMERA_FUNC_TERMINATE,
                                 (void *)0, (void *)0, 0, 0, 0, 0);
}

/*===========================================================================

FUNCTION      CAMERA_PROCESS_TERMINATE

DESCRIPTION
              The camera operation is stopped, VFE is brought to idle and dsp
              services are released. All memory except snapshot buffers are
              freed. Final state is CAMERA_STATE_READY.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void camera_process_terminate(void)
{
  MSG_CAMERADEBUG("CAMERA_SVCS: Enter camera_process_terminate", 0, 0, 0);

  /* check if this is not first time through function */
  if (camera_terminate_clk_state == CAMERA_TERMINATE_CLOCK_RUNNING)
  {
    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_RESUME, __LINE__);
    MSG_HIGH("CAMERA_SVCS: Termination Resumed", 0, 0, 0);
    clk_dereg(&camera_terminate_clk);
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_STOPPED;
  }
  else if ( camera_terminate_clk_state == CAMERA_TERMINATE_CLOCK_EXPIRED)
  {
    /* we are here because a timer expired, go to ready state and clean up */
    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_CLK_EXP, __LINE__);
    MSG_HIGH("CAMERA_SVCS: Timed out waiting for termination", 0, 0, 0);
    MSG_CAMERADEBUG("CAMERA_SVCS: Set camera_state=CAMERA_STATE_READY", 0, 0, 0);
    camera_state = CAMERA_STATE_READY;
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_STOPPED;
  }
  /* camera_abort_takepicture_encoding_operation will set
   * camera_take_picture_status.abort to TRUE, call appropriate abort command
   * if capture, YCBCR downsizer, BVCM, LPM, or encoding is in progress, and
   * return TRUE. If camera_abort_takepicture_encoding_operation returns true
   * a timer is set for 2000ms and camera_process_terminate returns. Upon a
   * callback from the abort request or receiving expected frame for capture
   * camera_process_terminate will be called again and we will continue
   * termination. If the timer expires camera_process_terminate is called.
   * In the case of multishot we will continue to return at this
   * point until capture of all frames is complete. With maximum of 16 frames
   * with 1 second delay this means there is a potential delay of 16 seconds.
   * Eventually the DSP firmware will support sending VFE_Idle during capture.
   * When this is true camera_abort_takepicture_encoding_operation should be
   * modified to return false if capture is in progress.
   */
  if ( camera_abort_takepicture_encoding_operation() == TRUE )
  {
    /* set a timer to make sure termination will continue */
    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_WAIT_FOR_ENCODE, __LINE__);
    MSG_HIGH("CAMERA_SVCS: Abort take picture encoding requested. Wait upto 2000ms", 0, 0, 0);
    clk_reg (&camera_terminate_clk, camera_terminate_clk_cb , (int32) 2000, 0, FALSE);
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_RUNNING;
    return;
  }
  
  camera_state = CAMERA_STATE_READY;
  
  if( (camera_state == CAMERA_STATE_READY) ||
      (camera_state == CAMERA_STATE_INIT) )
  {
    MSG_CAMERADEBUG("CAMERA_SVCS: Termination Cleanup", 0, 0, 0);

    camera_take_picture_status_set_default();
    camera_stopping = FALSE;
    camera_svcs_optimize_resources();

    SoftDSP_Stop();
    
    /* Return all buffers */
    camera_free_preview_buffers();

    CAMERA_SLEEP_ALLOW ();
    CAMERA_HALT_ALLOW();
    CAMERA_BUS_OFF();

    camera_log(LOG_INFO, (uint32)camera_func, 0, (uint32)TERM_END, __LINE__);
    event_report(EVENT_CAMERA_EXIT);
    MSG_HIGH("CAMERA_SVCS: Termination complete", 0, 0, 0);

    if (camera_app_cb)
    {
      MSG_CAMERADEBUG("Camera_Svcs-->UI stopping_cb:%d camera_func:%d",
          camera_stopping_cb, camera_func, camera_parm4_value);
      (camera_app_cb) (camera_stopping_cb, camera_app_data, camera_func, camera_parm4_value);
    }

    if (camera_tcb)
    {
      MSG_CAMERADEBUG("CAMERA_SVCS: Set CAMERA_SIG", 0, 0, 0 );
      (void) rex_set_sigs (camera_tcb, CAMERA_SIG);
      camera_tcb = NULL;
    }
  }

}

/*===========================================================================

FUNCTION      CAMERA_TERMINATE_CLK_CB

DESCRIPTION
              Callback to queue camera_process_terminate to graph context when
              terminate timer set expires.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 function param not referenced */
static void camera_terminate_clk_cb(int4 ms)
{
    camera_terminate_clk_state = CAMERA_TERMINATE_CLOCK_EXPIRED;
    MSG_HIGH("CAMERA_SVCS: Timed out waiting for abort", 0, 0, 0);
    camera_svcs_queue_call_to_terminate();
}
/*lint -restore */
#endif /* FEATURE_CAMERA */

