#ifndef CAMCTRL_H
#define CAMCTRL_H
/*============================================================================
  
                 Camera Service Control Header File
  
   DESCRIPTION
     This file contains the definitions for camera service control table.
     
     Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
  
============================================================================*/
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameracontrol/main/latest/inc/camctrl.h#10 $ $DateTime: 2009/04/22 23:29:28 $ $Author: c_caluru $

 when      who    what, where, why   
 --------  -----  ----------------------------------------------------------
 04/23/09  chai   Fixed the compiler error
 04/07/09  cg     Re-Enabling Feature FEATURE_CAMERA_BURST_MODE 
 02/23/09  cg     Fixed certain issues with EzTune1.0 ( Dog Timeout while sending all the parameters , Gamma Table read and write issue through chromatix,
                  Preview becomes reddish while trying to send all the paramters , Enabling/Disabling for the VFE blocks) 
 02/12/09  cgera  Added support for MULTISHOT AEC 1.1,5MP SENSOR support
 12/28/08  cg     Updated for AEC 1.0
 11/25/08  hap    Changes made to resolve AHB overflow issues
 07/30/08  sm     Added code for burst mode run time detection
 06/17/08  ww     Added set_lens_correction, sensor_lc_is_supported and 
                  enable_sensor_rolloff_correction to camctrl_tbl_type
 05/24/07  klee   Enhanced outdoor image quality by discarding too bright regions
 01/10/07  sigu   ARM side changes for integrating the incall downsizer for 6260 target.
 12/04/06  klee   Enhanced AEC luma compensation to be controllable
 11/11/06  klee   Updated AWB aggressiveness and low-light CC support to be tunable
 11/02/06  klee   Added aec_digital_gain_is_supported for AEC Digital gain
 10/24/06  klee   Added aec_aggressiveness
 09/12/06  dl     Used tuning variables for la and cs from chromatix.
 08/18/06  rcc    Made changes to variable names for CS and LA structs and 
                  changed asf variable name.
 08/04/06  dl     Added enable_5x5_asf flag to determine sensor support. 
 08/04/06  dl     Export camctrl_tbl to header file.
 08/02/06  klee   Cleaned up API on ISO, AWB, FastConvergence, Nightshot mode
 07/30/06  dl     Moved post processing params to camctrl.
 07/25/06  ven    Removed HJR structure from vfe_sw_feature_type and added to cam_ctrl_tbl
 07/13/06  sch    Aspect ratio updates.
 06/26/06  klee   Update FEATURE_CAMERA_INFO_RELOCATED to be CAMERA_INFO_RELOCATED, 
                  because it is not camera feature
 06/15/06  klee   Added FEATURE_CAMERA_INFO_RELOCATED regarding variable relocation
 06/15/06  jn     Added camctrl flag to determine whether to move lens in
                  camera_drv_task context. Default is off.
 05/27/06  dle    Removed exp_index_limit_for_outdoor_gamma.
 05/22/06  klee   Added enable_rolloff_correction
                  Removed current_camclk_po_hz
 05/02/06  klee   Added AWB algorithm to be controlable
 04/28/06  nshree Added AEC flag, to set during zoom operation
 04/14/06  jn     Added AF flag undershoot_protect.
 04/12/06  klee   Aded AWB tuning params to support cluster concept
 04/06/06  jn     Added AF variable clip_to_af_ratio.
 03/31/06  klee   Added high_luma_region_threshold for AEC
 03/01/06  sch    Added 'current_camclk_po_hz' to track the latest camclk_po 
                  freq requested. 
 12/20/05  sch    Updated set_nightshot_mode_f_type signature.
 12/19/05  sch    Modifications for nightshot mode to go through cam3a 
                  to adjust the frame rate, indoor / outdoor indices.

 01/11/06  jn     Initial version.
 
============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/

#include "comdef.h"
#include "customer.h"
#include "camera_3a_shared.h"

#ifdef FEATURE_CAMERA
// To be removed after all features migrated to new architecture
#include "camsensor.h" 

/*============================================================================
*                         TYPE DECLARATIONS
============================================================================*/

typedef struct
{  
  /* AF Related functions ---- Start */
  /* isp means sensor ISP based AF functions */
  void  (*isp_af_start)(uint32, void *);
  int32 (*isp_af_get_position)(void);
  /* AF Related functions ---- End */

  /******** Sensor functions ********/
  /* Bayer only */

  /* YCbCr only */

  /* Both Bayer and YCbCr */
  uint16    (*get_preview_pixels_per_line)(void);
  uint16    (*get_preview_lines_per_frame)(void);
  uint16    (*get_snapshot_pixels_per_line)(void);
  uint16    (*get_snapshot_lines_per_frame)(void);
  uint16    (*get_snapshot_frame_per_sec)(uint16);
  uint32    (*get_snapshot_max_exposure_line_count)(void);
  camera_ret_code_type (*set_snapshot_frame_rate) (uint16);
  camera_ret_code_type (*set_snapshot_exposure_gain) (void *);
  camera_ret_code_type (*set_lens_correction) (camsensor_lc_type);

  /******** Sensor static params ********/
  /* Feature flags are to be set during sensor init valiation phase */
  /* ISO Related Variables ---- Start*/
  boolean   iso_is_supported;
  uint32    iso100_gain;
  /* ISO Related Variables ---- End  */

 /*Lens Correction Related Variable*/
  boolean   sensor_lc_is_supported;
  boolean   enable_sensor_rolloff_correction;

  /* fast convergence Related Variables ---- Start */
  boolean   fast_convergence_is_supported;
  /* fast convergence Related Variables ---- End  */

  /* Nightshot Related Vairibales ---- Start */
  boolean   nightshot_is_supported;
  /* Nightshot Related Variables ---- End  */

  /* Multishot burst mode Related Vairibales ---- Start */

  boolean   multishot_burstmode_is_supported; 

  /* Multishot burst mode  Related Variables ---- End  */

  /* AF Related Variables ----- Start */
  boolean    af_is_supported;    /* whether auto focus is supported */
  boolean    af_use_isp;         /* whether to use sensor ISP based AF */
  uint32     clip_to_af_ratio;   /* vfe clipping to AF window ratio in one dimension */
  boolean    undershoot_protect; /* whether sensor needs undershoot protection */
  int32      undershoot_adjust;  /* number of steps to adjust when undershoot is detected */
  boolean    change_context;     /* whether to move lens in camera_drv context */
  boolean    reset_lens_after_snap; /* whether reset lens to home position after snapshot */
  /* AF Related Variables ----- End  */
  
  /* AEC Related Variables ----- Start */
  uint32 high_luma_region_threshold;
  boolean defer_aec_when_zoom; /* flag to set AEC during zoom operation */
  /* AEC Related Variables ----- end */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  /* Incall downsizer Related Variables ----- Start */
  boolean use_incall_downsizer; /* flag to set whether to use incall or standalone downsizer */
  /* Incall downsizer Related Variables ----- end */
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
#if defined QDSP_MODULE_VFE05_CAPTURE_DEFINED || defined QDSP_IMAGE_VFE_SA_DEFINED
  boolean use_vfe_image_swap; /* flag to set whether to use swap image in case of snapshot */
#endif /* defined QDSP_MODULE_VFE05_CAPTURE_DEFINED */

  boolean enable_rolloff_correction;

  /* CAMIF sub-sampling needed to keep the same aspect ratio along dx
   * during viewfinder. 
   */
  uint8   camif_x_subsample;

  /* HJR related varaibles */
  boolean hjr_bayer_filtering_enable;
  uint8   hjr_max_frames;  

  /* chroma suppression */
  boolean cs_is_supported;

  /* luma adaptation */
  boolean la_is_supported;

  /* 5x5 asf */
  boolean asf_5x5_is_supported;

#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */

  /* AEC Durinf Multishot */
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

  /* Some sensors takes a long time to switch from snapshot mode to preview
   * mode. When we have a situation where picture is taken, preview is entered
   * but momentarily stopped. The sensor is switching from snapshot to preview
   * mode. In order to stop preview, we have to put VFE in IDLE. VFE cannot
   * be idle until it receives a frame. Since it takes a long time for some
   * sensors to source the first preview frame, we must wait. This wait time
   * depends on sensor responsiveness. By default, we set that value to 600.
   * For a sensor that needs more time, please modify this value. */
  uint32 mode_transition_time_ms;
  /* AEC aggressiveness needs to be tunable depending on sensor */
  cam3a_aggressiveness_type aec_aggressiveness;
  /* AEC digital gain */
  boolean aec_digital_gain_is_supported;
  /* AWB aggressiveness and low-light CC support */
  cam3a_aggressiveness_type awb_aggressiveness;
  boolean awb_low_light_CC_is_supported;
  boolean aec_luma_compensation_is_supported;
  /* these are temporary, until new reference points are available */
  float noon_rg;
  float noon_bg;
  /* these are temporary, until new reference points are available */
  /* bright region discrimination */
  boolean aec_outdoor_bright_region_is_supported;
  float   aec_outdoor_bright_region_reduction;
  uint32  aec_outdoor_bright_region_threshold_LO;
  uint32  aec_outdoor_bright_region_threshold_HI;
  float   aec_outdoor_bright_region_discard_ratio;
  /* bright region discrimination */
  /******** To be removed after all features migrated to new architecture */
  camsensor_static_params_type* camsensor_params_ptr;
} camctrl_tbl_type;


/*============================================================================
*                         EXTERNAL FUNCTION DECLARATIONS
============================================================================*/
extern void camctrl_init_tbl(void);
extern void camctrl_validate_features (void);
extern uint8 camctrl_get_camif_x_subsample (void);

/*============================================================================
*                         EXTERNAL VARIABLE
============================================================================*/
extern camctrl_tbl_type camctrl_tbl;


#endif /* FEATURE_CAMERA */
#endif /* CAMCTRL_H */

