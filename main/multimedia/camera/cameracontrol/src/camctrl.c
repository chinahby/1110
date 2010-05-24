/* =========================================================================
                 Camera Service Control

   DESCRIPTION
      This file contains functions that supports camera service control
   table. 

   Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
   
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/cameracontrol/main/latest/src/camctrl.c#9 $ $DateTime: 2009/04/13 23:57:33 $ $Author: c_caluru $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/07/09  cg     Re-Enabling Feature FEATURE_CAMERA_BURST_MODE 
 02/23/09  cg     Fixed certain issues with EzTune1.0 ( Dog Timeout while sending all the parameters , Gamma Table read and write issue through chromatix,
                  Preview becomes reddish while trying to send all the paramters , Enabling/Disabling for the VFE blocks) 
 02/12/09  cgera  Added support for MULTISHOT AEC 1.1,5MP SENSOR support
 12/28/08  cg     Updated for AEC 1.0
 11/25/08  hap    Changes made to resolve AHB overflow issues
 07/30/08  sm     Added code for burst mode run time detection
 06/17/08  ww     Added camctrl_lc_is_supported() for 3 sensor rolloff tables
 05/24/07  klee   Enhanced outdoor image quality by discarding too bright regions
 01/25/07  ven    SC2X is set to use Stand alone downsizer only.
 01/10/07  sigu   ARM side changes for integarating the incall downsizer for 6260 target.
 12/20/06  klee   Added fix on CR 107950 - NULL pointer problem when camera is running with YUV sensor
 12/04/06  klee   Enhanced AEC luma compensation to be controllable
 11/11/06  klee   Updated AWB aggressiveness and low-light CC support to be tunable
 11/02/06  klee   Added aec_digital_gain_is_supported and its default is FALSE
 10/24/06  klee   Added aec_aggressiveness and its default value to be HIGH
 10/05/06  ven    Added support for using chromatix variables in HJR.
 09/12/06  dl     Used tuning variables for la and cs from chromatix.
 08/18/06  rcc    Changed variable names to make consistent.
 08/04/06  dl     Added enable_5x5_asf flag to determine sensor support.  
 08/02/06  klee   Cleaned up API on ISO, AWB, FastConvergence, Nightshot mode
 07/30/06  dl     Moved post processing params to camctrl.
 07/24/06  ven    Added support for configuring bayer filtering and max 
                  frames for HJR from sensor driver
 07/13/06  sch    Aspect ratio updates. 
 06/16/06  ven    Added HJR support with bayer filtering for MSMs with HW Roll OFF
 06/15/06  jn     Added camctrl flag to determine whether to move lens in
                  camera_drv_task context. Default is off.
 05/22/06  klee   Added camctrl_select_rolloff_correction()
 05/02/06  klee   Updated feature decision logic
                  service or 3a layer doesn't need to decide which algorithm is proper to use.
                  Bassed on the driver's capability, camctrl chooses the capability
 04/28/06  nshree Added AEC flag. Default is TRUE.
 04/14/06  jn     Added AF flag undershoot_protect. Default is FALSE.
 04/12/06  klee   Added AWB tuning params to support cluster concept
                  Added camctrl_cluster_is_supported()
 04/06/06  jn     Added default vfe clipping to af window ratio.
 03/31/06  klee   Added validate function for high luma compensation on AEC
 01/25/06  sch    Added validate function for nightshot support.
 01/11/06  jn     Initial version.
 
============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_CAMERA
#include "camctrl.h"

/*============================================================================
*                         DEFINITIONA
============================================================================*/
#define CAMCTRL_TBL_INVALID_LONG   0xFFFFFFFF
#define CAMCTRL_NO_HIGH_LUMA_COMPENSATION_THRESHOLD   255
#define CAMCTRL_DEFAULT_CLIP_TO_AF_RATIO   2

/*============================================================================
                        VARIABLES
============================================================================*/
camctrl_tbl_type camctrl_tbl;

/*============================================================================
                        INTERNAL API DECLARATIONS
============================================================================*/
static boolean camctrl_iso_is_supported (void);
static boolean camctrl_af_use_isp (void);
static uint32 camctrl_high_luma_compensation_is_supported (void);
static void camctrl_select_vfe_sw_features (const camsensor_static_params_type *camsensorParamsPtr);
static boolean camctrl_lc_is_supported (void);

/*============================================================================
                        EXTERNAL API DEFINITIONS
============================================================================*/
/*===========================================================================

FUNCTION      CAMCTRL_INIT_TBL

DESCRIPTION
   This function is to initialize camctrl_tbl. It sets all function pointers to
   NULL and all static params to invalid or default data.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  camctrl_tbl is initialized

===========================================================================*/
void camctrl_init_tbl(void)
{
  memset(&camctrl_tbl, 0, sizeof(camctrl_tbl));
  camctrl_tbl.iso100_gain = CAMCTRL_TBL_INVALID_LONG;
  camctrl_tbl.high_luma_region_threshold = CAMCTRL_TBL_INVALID_LONG;

  /* AF common tuning parameters -- start */
  camctrl_tbl.af_is_supported  = FALSE;
  camctrl_tbl.clip_to_af_ratio = CAMCTRL_DEFAULT_CLIP_TO_AF_RATIO;
  camctrl_tbl.undershoot_protect = FALSE;
  camctrl_tbl.undershoot_adjust  = 3;
  camctrl_tbl.change_context     = FALSE;
#if defined QDSP_MODULE_VFE05_CAPTURE_DEFINED || (defined QDSP_IMAGE_VFE_SA_DEFINED)
  camctrl_tbl.use_vfe_image_swap = FALSE;
#endif /* defined QDSP_MODULE_VFE05_CAPTURE_DEFINED || (defined QDSP_IMAGE_VFE_SA_DEFINED)*/
  /* AF common tuning parameters -- end */

  /* OEMs can enable AEC during zoom operation by setting the flag to FALSE */
  /* Should not defer processing of stats on LCU ( Need to revisit ) */

#ifndef QDSP_MODULE_VFE25_DEFINED
  camctrl_tbl.defer_aec_when_zoom = TRUE;
#else
#error code not present
#endif

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  /* OEMs can use Standalone downsizer by setting the flag to FALSE */
  /* By default it will be set to TRUE to use Incall downsizer */
#ifdef FEATURE_CAMERA_USE_STANDALONE_DOWNSIZER
  /* Stand alone downsizer only is used for SC2X */
  camctrl_tbl.use_incall_downsizer = FALSE;
#else
  camctrl_tbl.use_incall_downsizer = TRUE;
#endif /* FEATURE_CAMERA_USE_STANDALONE_DOWNSIZER */
  
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/

  /*HJR Initialization*/
#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
  camctrl_tbl.hjr_max_frames = 1;
#endif  /* FEATURE_CAMERA_BURST_MODE */

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

#ifdef FEATURE_CAMERA_BURST_MODE
#error code not present
#else
camctrl_tbl.multishot_burstmode_is_supported = FALSE;
#endif /* FEATURE_CAMERA_BURST_MODE */


  /* Bayer filtering is enabled by default */
  camctrl_tbl.hjr_bayer_filtering_enable = TRUE;
  
  if ( camsensor_info.msm_id == MSM6260)
  {
    /* Disable bayer filtering for the MSMs fall in Low cost unit category since they
     * are low cost and have less memory. If OEM wants to enable it then
     * they can do it from the camsensor driver. */
    camctrl_tbl.hjr_bayer_filtering_enable = FALSE;
  }
  /* chroma suppression support */
  camctrl_tbl.cs_is_supported = TRUE;

  /* luma adaptation support */
  camctrl_tbl.la_is_supported = TRUE;

  /* 5x5 ASF */
  camctrl_tbl.asf_5x5_is_supported = TRUE;


#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */



  /* Some sensors takes a long time to switch from snapshot mode to preview
   * mode. When we have a situation where picture is taken, preview is entered
   * but momentarily stopped. The sensor is switching from snapshot to preview
   * mode. In order to stop preview, we have to put VFE in IDLE. VFE cannot
   * be idle until it receives a frame. Since it takes a long time for some
   * sensors to source the first preview frame, we must wait. This wait time
   * depends on sensor responsiveness. By default, we set that value to 600ms.
   * For a sensor that needs more time, please modify this value. */
  camctrl_tbl.mode_transition_time_ms = 600;
  /* default AEC aggressiveness */
  camctrl_tbl.aec_aggressiveness = CAM3A_HIGH_AGGRESSIVENESS;
  /* AEC digital gain */
  camctrl_tbl.aec_digital_gain_is_supported = FALSE;
  /* AWB aggressiveness and low-light CC support */
  camctrl_tbl.awb_aggressiveness = CAM3A_LOW_AGGRESSIVENESS;
  camctrl_tbl.awb_low_light_CC_is_supported = FALSE;
  camctrl_tbl.aec_luma_compensation_is_supported = FALSE;

  /* these are temporary, until new reference points are available */
  camctrl_tbl.noon_rg = CAMCTRL_TBL_INVALID_LONG;
  camctrl_tbl.noon_bg = CAMCTRL_TBL_INVALID_LONG;
  /* these are temporary, until new reference points are available */

  camctrl_tbl.aec_outdoor_bright_region_is_supported = TRUE;
  camctrl_tbl.aec_outdoor_bright_region_reduction = 0.25;
  camctrl_tbl.aec_outdoor_bright_region_threshold_LO = CAMCTRL_TBL_INVALID_LONG;
  camctrl_tbl.aec_outdoor_bright_region_threshold_HI = CAMCTRL_TBL_INVALID_LONG;
  camctrl_tbl.aec_outdoor_bright_region_discard_ratio = 0.5;
} /* camctrl_init_tbl */

/*===========================================================================

FUNCTION      CAMCTRL_VALIDATE_FEATURES

DESCRIPTION
   This function is called during camera services start up to validate the
   sensor's capability and set the feature flags in camctrl_tbl.

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  Feature flags in camctrl_tbl are initialized

===========================================================================*/
void camctrl_validate_features (void)
{
  camctrl_tbl.iso_is_supported = camctrl_iso_is_supported();
  camctrl_tbl.af_use_isp = camctrl_af_use_isp();
  camctrl_tbl.high_luma_region_threshold = camctrl_high_luma_compensation_is_supported();
  camctrl_select_vfe_sw_features(camctrl_tbl.camsensor_params_ptr);
  /* these are temporary, until new reference points are available */
  /* assigning reference points on D50 as default, if sensor doesn't have any epecific values */
  if((camctrl_tbl.noon_rg == CAMCTRL_TBL_INVALID_LONG) && (camctrl_tbl.noon_bg == CAMCTRL_TBL_INVALID_LONG))
  {
    camctrl_tbl.noon_rg = 
      camctrl_tbl.camsensor_params_ptr->chromatix_parms->awb_reference_hw_rolloff[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1].red_gain;
    camctrl_tbl.noon_bg =
      camctrl_tbl.camsensor_params_ptr->chromatix_parms->awb_reference_hw_rolloff[CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1].blue_gain;
  }
  /* these are temporary, until new reference points are available */
#ifndef CAMERA_USES_CAMCTRL_AEC
  if(camctrl_tbl.aec_outdoor_bright_region_threshold_LO == CAMCTRL_TBL_INVALID_LONG)
  {
    camctrl_tbl.aec_outdoor_bright_region_threshold_LO = 
      (camctrl_tbl.camsensor_params_ptr->chromatix_parms->default_luma_target)*4;
  }
  if(camctrl_tbl.aec_outdoor_bright_region_threshold_HI == CAMCTRL_TBL_INVALID_LONG)
  {
    camctrl_tbl.aec_outdoor_bright_region_threshold_HI = 
      (uint32)((camctrl_tbl.camsensor_params_ptr->chromatix_parms->default_luma_target)*2.5 + 0.5);
  }
#endif  
  camctrl_tbl.sensor_lc_is_supported = camctrl_lc_is_supported();
} /* camctrl_validate_features */

/*===========================================================================

FUNCTION      CAMCTRL_GET_CAMIF_X_SUBSAMPLE

DESCRIPTION
   Provides extra sub-sampling required to preserve aspect ratio.

DEPENDENCIES
   None

RETURN VALUE
  uint8: Subsampling ratio >= 1.

SIDE EFFECTS
  None

===========================================================================*/
uint8 camctrl_get_camif_x_subsample(void)
{
  if (camctrl_tbl.camif_x_subsample)
  {
   return camctrl_tbl.camif_x_subsample;
  }
  
  /* return minimum camif sub_sampling */
  return 1;
 } /* camctrl_camif_subsample_needed */


/* End of External API. 
 * New static functions should go into INTERNAL API section.
 */

/*============================================================================
                        INTERNAL API DEFINITIONS
============================================================================*/
/*===========================================================================

FUNCTION      CAMCTRL_ISO_IS_SUPPORTED

DESCRIPTION
   This function is called during camera services start up to validate the
   ISO capability.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if ISO is supported
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean camctrl_iso_is_supported (void)
{
  if ((!camctrl_tbl.get_preview_lines_per_frame)  ||
      (!camctrl_tbl.get_preview_pixels_per_line)  ||
      (!camctrl_tbl.get_snapshot_lines_per_frame) ||
      (!camctrl_tbl.get_snapshot_pixels_per_line) ||
      (!camctrl_tbl.get_snapshot_frame_per_sec)  ||
      (!camctrl_tbl.get_snapshot_max_exposure_line_count) ||
      (!camctrl_tbl.set_snapshot_frame_rate) ||
      (!camctrl_tbl.set_snapshot_exposure_gain) ||
      (camctrl_tbl.iso100_gain == CAMCTRL_TBL_INVALID_LONG))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* camctrl_iso_is_supported */

/*===========================================================================

FUNCTION      CAMCTRL_AF_USE_ISP

DESCRIPTION
   This function is called during camera services start up to validate the
   sensor ISP based AF capability.

DEPENDENCIES
   None

RETURN VALUE
  TRUE if ISP based AF is supported
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean camctrl_af_use_isp (void)
{
  if ((camctrl_tbl.af_is_supported) &&
      (camctrl_tbl.isp_af_start) &&
      (camctrl_tbl.isp_af_get_position))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* camctrl_af_use_isp */

/*===========================================================================

FUNCTION      CAMCTRL_HIGH_LUMA_COMPENSATION_IS_SUPPORTED

DESCRIPTION
   High luma compensation in indoor condition - AEC
DEPENDENCIES
   None

RETURN VALUE
  CAMCTRL_NO_HIGH_LUMA_COMPENSATION_THRESHOLD - 255
  or
  high_luma_region_threshold depending on the sensor

SIDE EFFECTS
  None

===========================================================================*/
static uint32 camctrl_high_luma_compensation_is_supported (void)
{
  if (camctrl_tbl.high_luma_region_threshold == CAMCTRL_TBL_INVALID_LONG)
  {
    return CAMCTRL_NO_HIGH_LUMA_COMPENSATION_THRESHOLD;
  }

  return camctrl_tbl.high_luma_region_threshold;
} /* camctrl_high_luma_compensation_is_supported */

/*===========================================================================

FUNCTION      CAMCTRL_SELECT_VFE_SW_FEATURES

DESCRIPTION
   selects the s/w features for VFE
    - hw rolloff, bvcm or no rolloff correction
    - hw 3x3 ASF or s/w
    - hjr support based on vfe features

DEPENDENCIES
   None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camctrl_select_vfe_sw_features (const camsensor_static_params_type *camsensorParamsPtr)
{
  /* Roll off */
  if (camsensorParamsPtr->output_format == CAMSENSOR_BAYER && 
      camctrl_tbl.enable_rolloff_correction == TRUE)
  {
    vfe_sw_features.rolloff_correction = VFE_ROLLOFF_CORRECTION;
  }
  else
  {
    vfe_sw_features.rolloff_correction = NO_ROLLOFF_CORRECTION;
  }

  /* ASF */
#ifndef QDSP_MODULE_VFE25_DEFINED
  if (vfe_hw_features.filter_3x3_asf)

  {
    /* change vfe_sw_features.enable_hw_filter_3x3_asf to TRUE to enable 3x3 ASF
     * in HW. Otherwise 5x5 ASF will be used and 3x3 hw will be configured as a
     * smoothing filter.
     */
    vfe_sw_features.enable_hw_filter_3x3_asf = FALSE;
  }
  else
  {
    vfe_sw_features.enable_hw_filter_3x3_asf = FALSE;
  }
  
 #endif
  if (camsensorParamsPtr->output_format == CAMSENSOR_BAYER)
  {
    /* HJR */
    camctrl_tbl.hjr_max_frames = camsensorParamsPtr->chromatix_parms->max_number_of_frames;
#ifndef FEATURE_CAMERA_BURST_MODE
    camctrl_tbl.hjr_max_frames = 1;
#endif /* FEATURE_CAMERA_BURST_MODE */
  }
}

/*===========================================================================

FUNCTION      CAMCTRL_LC_IS_SUPPORTED

DESCRIPTION
   This function is called during camera services start up to validate if 
   sensor rolloff is desired

DEPENDENCIES
   None

RETURN VALUE
  TRUE if sensor lens correction is supported
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean camctrl_lc_is_supported (void)
{
  if ((!camctrl_tbl.enable_sensor_rolloff_correction)|| 
       (!camctrl_tbl.set_lens_correction))     
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* camctrl_lc_is_supported */

/* End of Internal API. 
 * New global functions should go into EXTERNAL API section.
 */

#endif /* FEATURE_QCAMERA */

