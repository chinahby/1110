#ifndef CAMERAI_H
#define CAMERAI_H
/*============================================================================
  
                 Camera Service Internal Header File
  
   DESCRIPTION
     This file contains the definitions needed for the camera service
     internal implemented.
  
============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameracommoninterface/main/latest/inc/camerai.h#6 $ $DateTime: 2009/04/14 01:30:42 $ $Author: cgera $

 when      who    what, where, why   
 --------  -----  ----------------------------------------------------------
 04/14/09  cg      Added Support for BS_INDOOR_AWB required for PartyMode AWB 
 04/07/09  cg      Re-Enabling Feature FEATURE_CAMERA_BURST_MODE
 12/18/08  sm     Added support for multi raw preview capture
 11/25/08  hap    Adding support for shutter sound play and VFE capture 
                  concurrency
 11/07/07  cg     Defined a mcro according to the clock regime defined
 03/29/07  klee   Added camera_exposure_info_request_type for camera_exposure_info
 03/28/07  klee   Enhancement on gamma table interpolation
 03/28/07  klee   Modified the name awb_input_exposure_type to be camera_exposure_info_type
 03/13/07  klee   Modified by moving the code on special effect from camera_svcs to awb
 03/12/07  klee   Modified by 3A-restructuring
 02/20/07  klee   Fixed all lint errors on AWB
 11/08/06  dhaval Removed the camera_start_record prototype change.
 10/30/06  dhaval Changed prototype of camera_svcs_start_record routine to pass
                  space limit value
 10/26/06  jv     Added soft focus for best shot.
 10/20/06  jv     Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/18/06  jch    Preview async mode and new buffer mechanism.
 09/21/06  jv     Add entries to camera_log_info_type for termination
 09/14/06  jv     Added white balance adjustment to best shot table
 09/08/06  klee   Removed extern camera_aec_awb_delay() by adding API in 3a as replacement
 09/01/06  kadhir Removed unwanted Cross Compilation Dependency 
                  When qcamcorder Feature Disabled 
 08/30/06  klee   Removed orange/green emphasized CV matrix support
 08/15/06  jv     Added mode element to camera_bestshot_mode_type. Renamed 
                  camera_get_bestshot_config to camera_svcs_bestshot_get_config
 08/09/06  dle    Added provision to use CAMERA_MALLOC_LOGGING instead of
                  malloc_mgr.
 07/25/06  ven    Removed HJR structure from vfe_sw_feature_type and added 
                  to cam_ctrl_tbl 
 07/25/06  jv     Removed CAMERA_WB_AUTO_OR_FLASH and removed unsupported 
                  bestshot wb parameters.
 07/18/06  jv     Added CAMERA_WB_AUTO_OR_FLASH for bestshot only.
 07/11/06  jv     Separate BSM enum types from rest of camera types.
 06/26/06  jv     Changed parameters and return value for camera_get_bestshot_config
 06/20/06  jv     Changed name of camera_bs_override_type to 
                  camera_bs_setting_type. Added camera_get_bestshot_config.
 06/16/06  jv     Added Bestshot feature support.
 06/16/06  ven    Added HJR support with bayer filtering for MSMs with HW Roll OFF
 06/12/06  jch    Added camera_take_multiple_pictures and related functions. 
 05/22/06  klee   Added new enum to support bvcm/hw rolloff correction at the same time
 05/18/06  dle    chroma_upsampling problem is resolved using DSP. Not needed.
 05/09/06  jch    Moved Burstmode definitions from camera.h to cameri.h
 05/02/06  klee   Moved in the definition of VfeStatOutStruct from camera_3a_shared.h
 04/20/06  sch    Display preview frame at user specified x,y coordinates, 
                  CR91980.
 03/13/06  dle    Added CAMERA_MALLOC_LOGGING to enable or disaable
                  camera_malloc() and camera_free(). Default is disabled.
 03/07/06  klee   Removed CAM3A_AGW_AWB_SGW_DEFAULT from AWB illuminant enum list
                  Removed extern vfe_center_weighted_luma and extern camera_vfe_luma
                  Added CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT 
                  Re-define CAM3A_AGW_AWB_NUMBER_OF_LIGHTING_CONDITION 
 02/28/06  jn     Added AF_FULL_SWEEP definition.
 02/24/06  ven    Added support for profiling awb and aec
 02/21/06  jn     Moved camera_aec_awb_delay() and MSG_CAMERADEBUG out of
                  camera_svcs.c.
 01/29/06  ali    Added new parameter to camera_start_record  
 01/16/06  dle    Fixed all lint problems.
                  Removed _start_stand_alone(), _stop_stand_alone(), and
                  _encode_picture_stand_alone(), _color_convert(). Not
                  applicable for 6550.
 12/16/05  dle    Added DSP illegal command camera_log types and enums for
                  DSP commands.
 09/29/05  anb    Added logic to proactively check for memory leaks.
 09/12/05  klee   took AF process types from cam3a.h 
 09/02/05  vlc    Added support for in call voice hand off.
 08/22/05  dle    Moved vfe_hw_feature_type and vfe_sw_feature_type here.
                  Moved DATA DEFINITIONS section down after TYPE and
                  CONSTANT.
 08/18/05  dle    Added LPM logging info.
 08/15/05  cl     Added support for setting multiple sets of uuid.
 07/27/05  lrw    Add msm_help.h include file for 6800 clock
 07/25/05  jj     camera_enable_solarize(), camera_default_gamma() extern.
 06/22/05  dle    Enabled CAMERA_LOGGING.
 05/25/05  mvd    Made changes to use the new Video Resources management 
                  for clock resource requests.
 05/09/05  mvd    Removed FEATURE_CAMERA_BRIDLE  
                  Made an ENUM out of the AWB illuminant conditions
 04/18/05  sch    -Abstracted FEATURE_DUAL_CLK_RGM  
 04/05/05  drl    Revised camera_set_dimensions for consistant use between
                  camera and camcorder apps.
 03/24/05  mvd    Added camera_set_parm_2
 03/22/05  sai    Added support for offline snapshot.
 03/14/05  drl    Added encode_width and encode_height args to 
                  camera_set_dimensions for V7 builds.
 02/25/05  sch    Added *_focus_window handlers.
 02/23/05  mvd    Fixed spelling mistake.
 02/23/05  wh     Go back to version 6
 02/22/05  wh/dw  Bitrate control.
 01/25/05  jz/js  Added camera_enable_qvp in the function table, fixed 
                  QVP crash problem
 01/20/05  sch    - Fixed compilation failure when 
                    FEATURE_CAMERA_ENCODE_PROPERTIES is defined.
 01/14/05  drl    Added camera svc function pointer types.
 01/12/05  mvd    Moved over some CAM3A structures that the Camsensor
                  driver needs to reference to here.
 11/01/05  kln    Addeded prototypes for camera_init(), camera_process_pnge_msg(), 
                  and camera_process_qdsp_msg() to camerai.h
 10/11/04  dle    Initial release.

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/

#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "camera.h"
#include "video_resources.h"
#include "malloc_mgr.h"
#include "camqdsp.h"
#ifdef FEATURE_CAMERA

/*============================================================================
*                         TYPE DECLARATIONS
============================================================================*/
typedef enum
{
  /* _DEFAULT and _UNCHANGED are special cases. Use negative numbers to keep
     them out of Qmobicat's selection range. */
  AF_PROCESS_DEFAULT   = -2,
  AF_PROCESS_UNCHANGED = -1,
  AF_EXHAUSTIVE_SEARCH = 0,
  AF_EXHAUSTIVE_FAST,
  AF_HILL_CLIMBING_CONSERVATIVE,
  AF_HILL_CLIMBING_DEFAULT,
  AF_HILL_CLIMBING_AGGRESSIVE,
  AF_FULL_SWEEP,
  AF_PROCESS_MAX
} cam3a_af_process_enum_type;


#ifdef FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
typedef enum
{
  NO_SHUTTER_SND = 0,
  SHUTTER_SND_PLAY_START,
  SHUTTER_SND_PLAY_DONE,
  SHUTTER_SND_PLAY_ABORT
}camera_shutter_snd_play_type;
#endif /* FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND */

typedef enum
{
  CAM3A_LOW_AGGRESSIVENESS,
  CAM3A_MED_AGGRESSIVENESS,
  CAM3A_HIGH_AGGRESSIVENESS
} cam3a_aggressiveness_type;

typedef enum
{
  CAM3A_NO_AWB = 0,
  CAM3A_AWB_SIMPLE_GREY_WORLD_ALGO,
  CAM3A_AWB_ADVANCED_GREY_WORLD_ALGO
} cam3a_auto_white_balance_algo_type;

typedef struct
{
  /* Two epoch interrupts */
  boolean epoch_int2; 
  boolean skip_first_frame;
  /* 2 sync and 2 async timers */
  boolean timers; 
  /* Second generation demosaicing. */
  boolean demosaic_2g; 
  /* 6260 can only support up to QVGA subsampling */
  boolean limited_sub_sampling;
  #ifndef QDSP_MODULE_VFE25_DEFINED
  /* 3x3 luma filter */
  boolean filter_3x3_luma; 
  /* 3x3 adaptive spatial filter */
  boolean filter_3x3_asf;
  #endif /* QDSP_MODULE_VFE25_DEFINED */
  boolean chroma_histogram;
  /* frame drop using programmable frame rate pattern */
  boolean frame_rate_pattern; 
} vfe_hw_feature_type;

typedef enum
{
  NO_ROLLOFF_CORRECTION = 0,
  VFE_ROLLOFF_CORRECTION,
} rolloff_correction_type;

#ifndef CAMERA_USES_SOFTDSP
typedef struct
{
  boolean enable_hw_filter_3x3_asf;
  rolloff_correction_type rolloff_correction;
} vfe_sw_feature_type;

typedef struct
{
  uint16   number_of_frames_to_ignore;   /* Number of frames to ignore after reconfiguring VFE */
  boolean  reconfigure_vfe;              /* Variable used to force VFE to be reconfigured      */
  boolean  vfe_idle_msg_failed;          /* Variable used to tell whether the idle message failed */
  boolean  wait_for_dsp_idle;            /* Variable used to wait for DSP idle command response   */
} camera_vfe_configuration_control_and_status_type;
#endif
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#ifndef CAMERA_USES_SOFTDSP
extern camera_vfe_configuration_control_and_status_type 
       camera_vfe_control_and_status;
#endif
typedef enum
{
  GAMMA_TABLE_DEFAULT = 0,
  GAMMA_TABLE_OUTDOOR,
  GAMMA_TABLE_LOWLIGHT,
  GAMMA_TABLE_BACKLIGHT,
  GAMMA_TABLE_INTERPOLATED,
  GAMMA_TABLE_INVALID
} vfe_gamma_table_type;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

/*============================================================================
*                         CONSTANTS
============================================================================*/
#define CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT     9 /* Increased here because of   AGW_AWB_OUTDOOR_NOON is also updated */
/* Daylight_hybrid is considered to be the same as outdoor_sunlight, 
   so # of lighting condition is equal to # of reference points + 1 */
#define CAM3A_AGW_AWB_NUMBER_OF_LIGHTING_CONDITION   (CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT + 1 )

#define NO_PENDING_AF_CMDS_TO_ISSUE        0
#define PENDING_AF_STATS_CMD_TO_ISSUE      0x1

#define CAMERA_SATURATION_DEFAULT          1.0

//
typedef enum                   
{
  CAM3A_AGW_AWB_INVALID = -1,
  CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT =  0,
  CAM3A_AGW_AWB_OUTDOOR_CLOUDY,
  CAM3A_AGW_AWB_INDOOR_INCANDESCENT,
  CAM3A_AGW_AWB_INDOOR_WARM_FLO,
  CAM3A_AGW_AWB_INDOOR_COLD_FLO,
  CAM3A_AGW_AWB_HORIZON,
  CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1,         
  CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO, 
  CAM3A_AGW_AWB_OUTDOOR_NOON,
  CAM3A_AGW_AWB_DAYLIGHT_HYBRID,
  CAM3A_AGW_AWB_INDOOR_NOON,
  CAM3A_AGW_AWB_SGW_DEFAULT
} cam3a_auto_white_balance_illuminant_type;
//

/*============================================================================
*                         DATA DEFINITIONS
============================================================================*/
/* We have YCBCR1 and YCBCR2 VFE (DSP).
 *
 * YCBCR1 uses averaging for decimation over 2 and nearest neighbor for
 * decimation less than 2. For example, a decimation of 3 would cause
 * an averaging to reduce decimation to 1.5, then use nearest neighbor
 * for the remining 1.5.
 *
 * YCBCR2 uses M/N downsizing. It also supports VFE_UPDATE command that
 * changes color conversion matrix and skip frame pattern. */
extern boolean camera_use_ycbcr1;
extern uint32 preview_red_gain, preview_green_gain, preview_blue_gain;
extern uint32 preview_gain;
extern uint8 preview_red_average, preview_green_average, preview_blue_average;
extern camera_preview_mode_type preview_mode;
/* When set to true, the VFE is reconfigured */
extern boolean camera_reconfigure;
/* When set to TRUE, the VFE configuration is updated */
extern boolean camera_update_vfe;
extern camera_nightshot_mode_type camera_nightshot_mode;
#ifdef FEATURE_VIDEO_ENCODE
extern byte * camera_current_frame;
#endif /* FEATURE_VIDEO_ENCODE */
extern boolean camera_ok_to_sleep;
#ifndef CAMERA_USES_SOFTDSP
extern vfe_hw_feature_type vfe_hw_features;
extern vfe_sw_feature_type vfe_sw_features;
extern CAMQDSP_VFE_CommandType camera_dsp_command;
#endif
extern camera_state_type       camera_state;
extern int32                   camera_pending_stats_cmds;
extern camera_parm_info_type   camera_parm_manual_focus;


/*============================================================================
*                         LOGGING DEFINITIONS
============================================================================*/
#ifndef CAMERA_USES_SOFTDSP
#define CAMERA_LOGGING
#endif
#ifdef CAMERA_LOGGING

typedef enum
{
  LOG_FUNC,
  LOG_PFUNC,
  LOG_RSP,
  LOG_INFO,
  LOG_VFEC,
  LOG_DSPC,    /* DSP command */
  LOG_DSPILL,  /* DSP says command illegal */
  LOG_TRM,
  LOG_QCB,
  LOG_NEXT
} camera_log_type;

typedef enum
{
  DUMMY,
  /* Used for LOG_TRM */
  INV_STATE,
  INV_PARM,
  /* Used for LOG_VFEC */
  VFE_INIT,  /* Initialize DSP               */
  VFE_IDLE,  /* Idle DSP                     */
  VFE_CFG,   /* Config VFE                   */
  VFE_CAP,   /* Do capture                   */
  VFE_TERM,  /* Terminate DSP                */
  VFE_RLS,   /* Release frame                */
  VFE_UPDATE,/* Update VFE                   */
  /* Used for LOG_QCB */
  IMG_ME,    /* My image                     */
  IMG_MEOK,  /* My image, correct state      */
  RST_ME,    /* Reset my image               */
  RST_MEOK,  /* Reset my image, correct st   */
  RST_ACK,   /* Reset not correct state      */
  /* Used for LOG_INFO */
  PIC_SAVED,
  INIT_REJ,   /* Init rejected                */
  IDLE_REJ,   /* Idle rejected                */
  CFG_REJ,    /* Config rejected              */
  CAP_REJ,    /* Capture rejected             */
  UPDATE_REJ, /* Update rejected              */
  EFS_FAIL,   /* EFS failed                   */
  DET_ERR,
  ABORT,
  LPM_START,
  LPM_END,
  QDSP_CRPT,
  TERM_WAIT_FOR_ENCODE,
  TERM_WAIT_FOR_VFE,
  TERM_RESUME,
  TERM_CLK_EXP,
  TERM_SRVC_UNAVAIL,
  TERM_START,
  TERM_END,
  CONFIG_VIDEO        =   0x700,
  CONFIG_RAW_SNAPSHOT =   0x701,
  CONFIG_SNAPSHOT     =   0x702,
  CONFIG_OFFLINE      =   0x703,
  CAPTURE             =   0x704,
  IDLE                =   0x705,
  UPDATE              =   0x706,
  END_OF_FRAME_ACK1   =   0x707,
  END_OF_FRAME_ACK2   =   0x708,
  AF_STATS_START      =   0x709,
  AF_STATS_STOP       =   0x70a,
  AE_AWB_STATS_REQ    =   0x70b,
  HIST_STATS_REQ      =   0x70c
} camera_log_info_type;

void camera_log (camera_log_type, uint32, uint32, uint32, uint32);
#else /* CAMERA_LOGGING */
#define camera_log(a, b, c, d, e)
#endif /* CAMERA_LOGGING */

/* Camera needs to monitor the state of the mobile.  When the mobile is in a voice call,
** Camera needs to allow for a voice image swap if a hand off requires it.
** Data calls may need to be added in the future to support Voice over IP.
*/
typedef enum
{
  NOT_IN_VOICE_CALL,             /* Phone is not in a call */
  IN_VOICE_CALL,                 /* Phone is in voice call */

} camera_phone_state_type;

extern camera_phone_state_type camera_phone_state;

/*============================================================================
*                         MACRO DEFNITIONS
============================================================================*/

#define MSG_CAMERADEBUG(x,a,b,c) MSG_MED(x,a,b,c)

#define CAM3A_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CAM3A_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define MSG_CAM3ADEBUG(x,a,b,c) MSG_LOW(x,a,b,c)

#define CAM3A_IS_UINT8(a) ((a)<=255)
#define CAM3A_IS_INT8(a) (-128<=(a) && (a)<=127)
#define CAM3A_IS_INT12(a) (-2048<=(a) && (a)<=2047)

#ifdef FEATURE_DUAL_CLK_RGM

#define CAMERA_CLK_REGIME_SEL_CLK_INV      clk_regime_msm_sel_clk_inv
#define CAMERA_CLK_REGIME_SEL_CLK_SRC      clk_regime_msm_sel_clk_src  
#define CAMERA_CLK_REGIME_SEL_CAMCLK       clk_regime_msm_sel_camclk 
#define CAMERA_CLK_REGIME_MSM_RESET        clk_regime_msm_reset

#else

#define CAMERA_CLK_REGIME_SEL_CLK_INV      clk_regime_sel_clk_inv
#define CAMERA_CLK_REGIME_SEL_CLK_SRC      clk_regime_sel_clk_src      
#define CAMERA_CLK_REGIME_SEL_CAMCLK       clk_regime_sel_camclk 
#define CAMERA_CLK_REGIME_MSM_RESET        clk_regime_reset

#endif /* FEATURE_DUAL_CLK_RGM */

#define CAMERA_CLK_REGIME_ENABLE(clk)      video_resources_clock_on (VIDEO_RESOURCES_CLIENT_QCAMERA, clk)
#define CAMERA_CLK_REGIME_DISABLE(clk)     video_resources_clock_off (VIDEO_RESOURCES_CLIENT_QCAMERA, clk)

#define CAMERA_CONFIG_GPIO                 gpio_tlmm_config

/* To enable profiling of awb and aec, 
simply uncomment the next line to define FEATURE_CAMERA_3A_PROFILING */
//#define FEATURE_CAMERA_3A_PROFILING        1

#if defined   (T_MSM6275) 
#error code not present
#elif defined (T_MSM6800)
#include "msm_help.h"
#define CAMCLK_PLL_FREQ CLKRGM_MPLL_SPEED
#else /* MSM6550 and default */
#define CAMCLK_PLL_FREQ 196608000 
#endif /* T_MSM6275 */

/* when CAMERA_MALLOC_LOGGING is defined, malloc_mgr is disabled for this
 * module. */
//#define CAMERA_MALLOC_LOGGING
#ifdef CAMERA_MALLOC_LOGGING
#define CAM_Malloc(size) camera_malloc(size, __LINE__)
#define CAM_Free(ptr) if(NULL!=ptr){camera_free(ptr, __LINE__);ptr=NULL;}
#else
#define CAM_Malloc(size) MM_MALLOC(size, CAMERA_SVCS_GROUP, __FILE__, __LINE__)
#define CAM_Free(ptr) if(NULL!=ptr){MM_FREE(ptr, __FILE__, __LINE__);ptr=NULL;}
#endif

typedef void (* camera_init_f_type) (void);

typedef camera_ret_code_type (* camera_start_f_type)
   (camera_cb_f_type callback,
    void            *client_data);

typedef camera_ret_code_type (* camera_stop_f_type)
   (camera_cb_f_type callback,
    void            *client_data);

typedef camera_ret_code_type (* camera_set_dimensions_f_type)
   (uint16           picture_width,
    uint16           picture_height,
    uint16           display_width,
#ifdef FEATURE_CAMERA_V7
    uint16           display_height, /*   dimen. as compared to the display */
                                     /*   dimension */
#endif
    camera_cb_f_type callback,
    void            *client_data);

typedef camera_ret_code_type (* camera_start_preview_f_type)
   (
    camera_cb_f_type    callback,
    void               *client_data);

typedef camera_ret_code_type (* camera_stop_preview_f_type) (void);

typedef camera_ret_code_type (* camera_take_picture_f_type)
   (
     camera_cb_f_type    callback,
     void               *client_data
#if !defined FEATURE_CAMERA_ENCODE_PROPERTIES && defined FEATURE_CAMERA_V7
     ,camera_snapshot_type camera_snapshot_mode
#endif /* nFEATURE_CAMERA_ENCODE_PROPERTIES && FEATURE_CAMERA_V7 */
   );

typedef camera_ret_code_type (* camera_encode_picture_f_type)
   (camera_frame_type  *frame,
    camera_handle_type *handle,
    camera_cb_f_type    callback,
    void               *client_data);

typedef camera_ret_code_type (* camera_color_convert_f_type)
   (camera_frame_type *frame,
    camera_format_type format,
    uint32             length,
    byte              *buffer,
    camera_cb_f_type   callback,
    void              *client_data
);

typedef camera_ret_code_type (* camera_get_thumbnail_properties_f_type)
   (uint32 *width,
    uint32 *height,
    uint32 *quality
);

typedef camera_ret_code_type (* camera_set_thumbnail_properties_f_type)
   (uint32 width,
    uint32 height,
    uint32 quality);

typedef camera_ret_code_type (* camera_start_focus_f_type)
   (camera_focus_e_type focus,
    camera_cb_f_type    callback,
    void               *client_data);

typedef camera_ret_code_type (* camera_stop_focus_f_type) (void);

typedef camera_ret_code_type (* camera_enable_frame_callback_f_type) (void);

typedef camera_ret_code_type (* camera_disable_frame_callback_f_type) (void);

typedef camera_ret_code_type (* camera_release_frame_f_type) (void);

typedef camera_ret_code_type (* camera_set_overlay_f_type)
  (uint16            dx,
   uint16            dy,
   uint32            transparent_color,
   void             *overlay_bmp,
   camera_cb_f_type  callback,
   void             *client_data);

typedef camera_ret_code_type (* camera_clr_overlay_f_type)
   (camera_cb_f_type callback,
    void            *client_data);
  
typedef camera_ret_code_type (* camera_set_icon_array_f_type)
   (uint32          dx,
    uint32          dy,
    uint32          transparent_color,
    ipl_icon_type **icon_array);

typedef camera_ret_code_type (* camera_clr_icon_array_f_type) (void);

typedef camera_ret_code_type (* camera_set_position_f_type)
   (camera_position_type *position,
    camera_cb_f_type      callback,
    void                 *client_data);

typedef camera_ret_code_type (* camera_set_exif_tag_f_type)
   (camera_exif_tag_type *tag,
    camera_cb_f_type      callback,
    void                 *client_data);

typedef camera_ret_code_type (* camera_blt_f_type)
   (camera_frame_type *frame,
    uint16             dst_dx,
    uint16             dst_dy,
    void              *dst_ptr,
    uint16             x,
    uint16             y,
    uint16             dx,
    uint16             dy);

typedef camera_ret_code_type (* camera_blt_ext_f_type)
   (camera_frame_type  *frame,
    uint16             dst_dx,
    uint16             dst_dy,
    void               *dst_ptr,
    uint16             x,
    uint16             y,
    uint16             dx,
    uint16             dy,
    camera_format_type format);

typedef camera_ret_code_type (* camera_set_parm_f_type)
   (camera_parm_type id,
    int32            parm,
    camera_cb_f_type callback,
    void            *client_data);

typedef camera_ret_code_type (* camera_set_parm_2_f_type)
   (camera_parm_type id,
    int32            parm1,
    int32            parm2,
    camera_cb_f_type callback,
    void            *client_data);


typedef void (* camera_get_info_f_type) (camera_info_type *info);

typedef camera_ret_code_type (* camera_get_parm_f_type)
  (camera_parm_type       id,
   camera_parm_info_type *parm);

typedef camera_ret_code_type (* camera_get_parm_2_f_type)
  (camera_parm_type      id,
   int32                *parm1_ptr,
   int32                *parm2_ptr);

typedef void (* camera_process_func_f_type)
   (camera_func_type,
    void  *callback,
    void  *client_data,
    uint32 info1,
    uint32 info2,
    uint32 info3,
    uint32 info4);

typedef void (* camera_process_qdsp_msg_f_type)
   (uint32 msg, 
    void  *buffer);

typedef camera_ret_code_type (* camera_release_picture_f_type) (void);

#ifdef FEATURE_QVPHONE
typedef camera_ret_code_type (* camera_enable_qvp_f_type)
   (void              **cb,
    camera_sensor_type *sensor_type,
    boolean             use_ycbcr1,
    boolean             arm_kick_backend);

typedef camera_ret_code_type (* camera_disable_qvp_f_type) (void);

typedef camera_ret_code_type (* camera_start_qvp_encode_f_type)
   (
    camera_cb_f_type    callback,
    void               *client_data);

typedef camera_ret_code_type (* camera_stop_qvp_encode_f_type) (void);

typedef void(* camera_qvp_reset_f_type)(void);

#endif /* FEATURE_QVPHONE*/

#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
typedef void (* camera_video_handshake_f_type) (uint32 code);
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE */
#ifdef FEATURE_VIDEO_ENCODE
typedef camera_ret_code_type (* camera_start_record_f_type)
  (
    video_fmt_type               file_format,
    video_fmt_stream_video_type  video_format,
    video_fmt_brand_type         video_brand,
    boolean                      short_header,
    video_fmt_stream_audio_type  audio_format,
#ifdef FEATURE_CAMERA_V770
    boolean                      bAudioPostCard,
#endif
#ifdef FEATURE_VIDEO_MULTI_UUID
    video_uuid_info_type        *uuid_info,
#else /* FEATURE_VIDEO_MULTI_UUID */
    void                        *uuid_data,
    uint32                       uuid_data_size,
    uint8                       *uuid_atom,
#endif /* FEATURE_VIDEO_MULTI_UUID */
    video_profile_type           profile,
#ifdef FEATURE_CAMERA_V7
    int32                        user_bitrate,
#endif
    video_handle_type           *destination,
    camera_cb_f_type             callback,
    void                        *client_data
);

typedef camera_ret_code_type (* camera_pause_record_f_type) (void);

typedef camera_ret_code_type (* camera_resume_record_f_type) (void);

typedef camera_ret_code_type (* camera_stop_record_f_type) (void);

#endif /* FEATURE_VIDEO_ENCODE */


#ifdef FEATURE_MP4_TRANSCODER
typedef camera_ret_code_type (* camera_enable_offline_f_type) (void);
#endif /* FEATURE_MP4_TRANSCODER */

#ifdef FEATURE_PNG_ENCODER
typedef void (* camera_process_pnge_msg_f_type) (uint32 msg);
#endif /* FEATURE_PNG_ENCODER */

#ifdef FEATURE_CAMERA_V7
typedef camera_ret_code_type (* camera_get_focus_window_f_type) (camera_focus_window_type *camera_display_focus_window);
typedef camera_ret_code_type (* camera_set_focus_window_f_type) (camera_focus_window_type *camera_display_focus_window);
typedef camera_ret_code_type (* camera_offline_snapshot_f_type) (OfflineInputConfigurationType offline_config_reg, camera_cb_f_type callback, void* client_data);
#endif /* FEATURE_CAMERA_V7 */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/

typedef struct camerai_func_struct
{
  camera_init_f_type                       camera_init;
  camera_start_f_type                      camera_start;
  camera_stop_f_type                       camera_stop;
  camera_set_dimensions_f_type             camera_set_dimensions;
  camera_start_preview_f_type              camera_start_preview;
  camera_stop_preview_f_type               camera_stop_preview;
  camera_take_picture_f_type               camera_take_picture;
  camera_encode_picture_f_type             camera_encode_picture;
  camera_get_thumbnail_properties_f_type   camera_get_thumbnail_properties;
  camera_set_thumbnail_properties_f_type   camera_set_thumbnail_properties;
  camera_start_focus_f_type                camera_start_focus;
  camera_stop_focus_f_type                 camera_stop_focus;
  camera_enable_frame_callback_f_type      camera_enable_frame_callback;
  camera_disable_frame_callback_f_type     camera_disable_frame_callback;
  camera_release_frame_f_type              camera_release_frame;
  camera_set_overlay_f_type                camera_set_overlay;
  camera_clr_overlay_f_type                camera_clr_overlay;
  camera_set_icon_array_f_type             camera_set_icon_array;
  camera_clr_icon_array_f_type             camera_clr_icon_array;
  camera_set_position_f_type               camera_set_position;
  camera_set_exif_tag_f_type               camera_set_exif_tag;
  camera_blt_f_type                        camera_blt;
  camera_blt_ext_f_type                    camera_blt_ext;
  camera_set_parm_f_type                   camera_set_parm;
  camera_set_parm_2_f_type                 camera_set_parm_2;
  camera_get_info_f_type                   camera_get_info;
  camera_get_parm_f_type                   camera_get_parm;
  camera_get_parm_2_f_type                 camera_get_parm_2;
  camera_process_func_f_type               camera_process_func;
  camera_process_qdsp_msg_f_type           camera_process_qdsp_msg;
  camera_release_picture_f_type            camera_release_picture;
#if defined FEATURE_VIDEO_ENCODE || defined FEATURE_QVPHONE
  camera_video_handshake_f_type            camera_video_handshake;
#endif /* FEATURE_VIDEO_ENCODE || FEATURE_QVPHONE */
#ifdef FEATURE_VIDEO_ENCODE
  camera_start_record_f_type               camera_start_record;
  camera_pause_record_f_type               camera_pause_record;
  camera_resume_record_f_type              camera_resume_record;
  camera_stop_record_f_type                camera_stop_record;
#endif /* FEATURE_VIDEO_ENCODE */
#ifdef FEATURE_QVPHONE
  camera_enable_qvp_f_type                 camera_enable_qvp;
  camera_disable_qvp_f_type                camera_disable_qvp;
  camera_start_qvp_encode_f_type           camera_start_qvp_encode;
  camera_stop_qvp_encode_f_type            camera_stop_qvp_encode;
  camera_qvp_reset_f_type                  camera_qvp_reset;
#endif /* FEATURE_QVPHONE */
#ifdef FEATURE_MP4_TRANSCODER
  camera_enable_offline_f_type             camera_enable_offline;
#endif /* FEATURE_MP4_TRANSCODER */
#ifdef FEATURE_PNG_ENCODER
  camera_process_pnge_msg_f_type           camera_process_pnge_msg;
#endif /* FEATURE_PNG_ENCODER */
#ifdef FEATURE_CAMERA_V7
  camera_get_focus_window_f_type           camera_get_focus_window;
  camera_set_focus_window_f_type           camera_set_focus_window;
  camera_offline_snapshot_f_type           camera_offline_snapshot;
#endif /* FEATURE_CAMERA_V7 */
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE */
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
} camerai_func_type;

typedef enum camerai_client_enum
{
  CAMERAI_CAMERA,
  CAMERAI_CAMCORDER,
  CAMERAI_QVPHONE,
  CAMERAI_CLIENT_TYPE_LAST
} camerai_client_type;

extern camerai_func_type camerai_client [CAMERAI_CLIENT_TYPE_LAST];

typedef enum
{
  AEC_AWB_DELAY_INIT = 0,
  AEC_AWB_DELAY_UPDATE,
  AEC_AWB_DELAY_DECR,
  AEC_AWB_DELAY_INVALID
} camera_aec_awb_delay_type;

typedef struct /* VfeStatOutStruct */
{
  unsigned long Focus;    /* 27 bits, focus value */
  unsigned long NFocus;   /* 9 bits, count of number of rows having */
                          /* at least one pixel meeting the condition */
  unsigned long SY[256];  /* 24 bits, sum of luminance in each of the X regions */
  unsigned long SCb[256]; /* 24 bits, sum of Cb in each of the X regions for */
                          /* the pixels that meet the inequalities */
  unsigned long SCr[256]; /* 24 bits, sum of Cr in each of the X regions for */
                          /* the pixels that meet the inequalities */
  unsigned long SY1[256]; /* 24 bits, sum of Y in each of the X regions for */
                          /* the pixels that meet the inequalities */
  unsigned long NSCb[256];/* 16 bits, number of pixels included inSCb in each of the
                             X regions for the pixels that meet the inequalities */
  unsigned long Hist1DCbInner[128]; /* 20 bit, 1D histogram of Cb of inner region */
  unsigned long Hist1DCrInner[128]; /* 20 bit, 1D histogram of Cr of inner region */
  unsigned long Hist1DCbOuter[128]; /* 20 bit, 1D histogram of Cb of outer region */
  unsigned long Hist1DCrOuter[128]; /* 20 bit, 1D histogram of Cr of outer region */
  /* Added the following on 6/3/04 */
  /* Average black value that is subtracted from the active image region */
  unsigned long AvgBlackValue;      /* 10 bits used for average black value */
  /* Num red, blue defective pixels corrected in frame, 8 bits */
  unsigned long RBDefectPixelCount; /* 8 bits for Red, Blue defect pixel cnt */
  /* Num green defective pixels corrected in frame, 8 bits */
  unsigned long GDefectPixelCount;  /* 8 bits for green defect pixel cnt */
} VfeStatOutStruct;

typedef struct
{
  int32  q_factor;
  /* R or Cr */
  int32  A11;
  int32  A12;
  int32  A13;
  /* G or Y  */
  int32  A21;
  int32  A22;
  int32  A23;
  /* B or Cb */
  int32  A31;
  int32  A32;
  int32  A33;

  int16  bias_y;
  int16  bias_Cb;
  int16  bias_Cr;
} camera_3x3_ycbcr_matrix_type;

typedef struct
{
  int32  q_factor;
  /* R or Cr */
  int32  A11;
  int32  A12;
  int32  A13;
  /* G or Y  */
  int32  A21;
  int32  A22;
  int32  A23;
  /* B or Cb */
  int32  A31;
  int32  A32;
  int32  A33;

  int16  bias_r;
  int16  bias_g;
  int16  bias_b;
} camera_3x3_rgb_matrix_type;

typedef struct
{
  int    q_factor;
  int32  r_gain;
  int32  g_gain;
  int32  b_gain;
} camera_1x3_matrix_type;

typedef enum
{
  AWB_AGW_INDOOR_ONLY = 0,
  AWB_AGW_OUTDOOR_ONLY,
  AWB_AGW_INOUTDOOR_BOTH
} awb_agw_search_mode;

typedef enum
{
  EXPOSURE_PER_AWB_ALGO = 0,
  EXPOSURE_PER_GAMMA
} camera_exposure_info_request_type;

typedef struct
{
  uint16   indoor_index;
  uint16   outdoor_index;
  uint16   current_exposure_index;
  uint16   max_exp_index;
  uint16   indoor_midpoint;
  uint16   inoutdoor_midpoint;
  uint16   outdoor_midpoint;
  awb_agw_search_mode search_mode;
} camera_exposure_info_type;

/*============================================================================
*                         INTERNAL FUNCTIONS
============================================================================*/
void camera_no_memory(void);

/*===========================================================================

FUNCTION      CAMERAI_INIT

DESCRIPTION
   Initialize camerai to correctly route camera service layer function calls
              

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
void camerai_init (void);

/*===========================================================================

FUNCTION      CAMERAI_SET_CLIENT

DESCRIPTION
   Set the current camera service client for proper camera service function
   call routing.

RETURN VALUE
   None

SIDE EFFECTS
  None

===========================================================================*/
void camerai_set_client (camerai_client_type client);

/*===========================================================================

FUNCTION      CAMERA_TIMED_WAIT

DESCRIPTION
              Wait for some milli-seconds.

RETURN VALUE
  if in graph context
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
void camera_timed_wait(uint32 ms);

#ifdef CAMERA_MALLOC_LOGGING
/*===========================================================================

FUNCTION      CAMERA_MALLOC & CAMERA_FREE

DESCRIPTION
              When used in debug mode, do malloc and free and keep track
              of memory allocations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void *camera_malloc (uint32 size, uint32 linenumber);
void camera_free(void *ptr, uint32 linenumber);
#endif /* CAMERA_MALLOC_LOGGING */

#ifdef FEATURE_PNG_ENCODER
/*===========================================================================

FUNCTION      CAMERA_PROCESS_PNGE_MSG

DESCRIPTION
              Process all messages fr PNG encoder

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void camera_process_pnge_msg(uint32 msg);
#endif /* FEATURE_PNG_ENCODER */

#ifdef FEATURE_CAMERA_YCBCR_ONLY
#define camera_enable_solarize() CAMERA_NOT_SUPPORTED
#define camera_default_gamma() CAMERA_NOT_SUPPORTED
#else
#ifndef CAMERA_USES_SOFTDSP
extern camera_ret_code_type camera_enable_solarize( void ) ;
extern camera_ret_code_type camera_default_gamma  ( void ) ;
#endif //#ifndef CAMERA_USES_SOFTDSP
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

extern uint16 camera_recent_fps( void );

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

#endif /* FEATURE_CAMERA */
#endif /* CAMERAI_H */

