#ifndef CAMERA_AEC_H
#define CAMERA_AEC_H
/*=============================================================================
FILE:       camera_aec.c

SERVICES:   Auto Exposure Header File

DESCRIPTION: 

        Copyright © 2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/aec/main/latest/inc/camera_aec.h#5 $ $DateTime: 2009/04/22 23:29:28 $ $Author: c_caluru $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 04/22/09   cg       Fixed the issue with Multishot when linear AFR is enabled.
 04/07/09   cg       Re-Enabling Feature FEATURE_CAMERA_BURST_MODE 
 12/26/08   cg       Updates for AEC Multishot 1.0
 03/11/08   cg       Updates for Ez Tune 1.0 , 3A1.4 and 0x203 header 
 05/24/07   klee     Enhanced outdoor image quality by discarding too bright regions
 03/30/07   klee     Clean-up lint errors due to un-matched data type in 3a
 03/29/09   klee     Combined camera_aec_retrive_exposure_info 
                     and camera_aec_get_exposure_info_for_gamma_interpolation
 03/28/07   klee     Enhancement on gamma table interpolation
 03/28/07   klee     Modified the name awb_input_exposure_type to be camera_exposure_info_type
 03/15/07   klee     Updated preview re-configuration on re-starting
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 01/05/07   klee     Fixed flickering issue with AEC digital gain
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Added AEC Digital gain implementation
 10/20/06   jv      Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/16/06   ven      Featurized HJR
 10/11/06   ven      Added support for HJR tuning parameter to delay the
                     selection of multi frame registration based HJR.
 09/15/06   klee     Cleaned up the lint errors
 09/13/06   klee     Added outdoor_gamma_index for outdoor gamma table
 09/08/06   klee     Added exposure_index_step for fast convercenge
 08/29/06   klee     Consolidated antibanding calculation
 08/11/06   klee     Cleaned up the code regarding tuning variables
                     - AEC
 07/26/06   klee     Moved out AEC core algorithm from camera_aec_core
 07/25/06   klee     Added indoor/outdoor_index_org in cam3a_aec_state
 07/24/06   ven      Added support for Antibanding for HJR
 07/18/06   dle      Added camera_aec_set_bias_table().
 07/13/06   ven      Moved hjr setup function to camera_aec.c
 07/11/06   ven      Moved HJR AEC related code from camera svcs to aec
 05/27/06   ven      Added UI support for HJR.
 05/02/06   klee     Added camera_aec_set_nightshot_mode_legacy for old algorithm
                     and camera_aec_set_nighshot_mode_info which is called by svcs
 04/14/06   klee     Updated aec stats structure to support luma compensation with AWB gains
 03/30/06   klee     Added high_luma_region_count in AEC status structure
 03/03/06   klee     Updated camera_aec_implement()
                     - current_vfe_luma is not availble anymore
 02/24/06   ven      Added support for profiling awb and aec
 12/20/05   sch      Updated night shot mode handler arguments.
 12/19/05   sch      Modifications for nightshot mode to go through cam3a 
                     to adjust the frame rate, indoor / outdoor indices.

 12/01/05   klee     fix lint error for 4.3 release
                     lint error 818 has been simply surpressed for all over the file.
 10/04/05   klee     Initial code transfered from cam3a 

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include <comdef.h>
#include <math.h>
#include "msg.h"
#include "vfe.h"
#include "camera.h"
#include "camerai.h"
#include "camsensor.h"

#ifdef FEATURE_CAMERA_YCBCR_ONLY
/* Define External API */
#define camera_aec_init()
#define camera_aec_mem_allocate TRUE
#define camera_aec_mem_deallocate()
#define camera_aec_config(camsensorParamsPtr)
#define camera_aec_flash_needed() TRUE
#define camera_aec_get_aec_mode CAMERA_AEC_FRAME_AVERAGE
#define camera_aec_implement(num_pixels_per_region_aec, camsensorParamsPtr, numRegions)
#define camera_aec_set_bias_table(camsensorParamsPtr) FALSE
#define camera_aec_set_aec_mode(aec_mode, camsensorParamsPtr) FALSE
#define camera_aec_set_antibanding(state, camsensorParamsPtr) CAMERA_NOT_SUPPORTED
#define camera_aec_set_brightness(luma)
#define camera_aec_set_exposure_compensation(luma)
#define camera_aec_show_timing()
#define camera_aec_set_parm(id, parm) CAMERA_NOT_SUPPORTED
#define camera_aec_get_parm(id, parm) CAMERA_NOT_SUPPORTED
#define camera_aec_adjust_exposure_for_snapshot() CAMERA_NOT_SUPPORTED
#define camera_aec_prepare_for_hjr(camsensorParamsPtr)
#define camera_aec_adjust_exposure_as_per_hjr()
#define camera_aec_apply_digital_gain_for_preview(packed_cc_ptr)
#define camera_aec_apply_digital_gain_for_iso_snapshot(packed_cc_ptr)
#define camera_aec_update_color_correction_matrix(sourceCC, low_light_CC_ptr)
#define camera_aec_retrive_exposure_info(aec_enable, request_type, exposure_info_ptr)
#define camera_aec_restore_config_for_preview()
#else /* not defined FEATURE_CAMERA_YCBCR_ONLY */
/*==============================================================================
  FEATURES
==============================================================================*/

/*==============================================================================
  CONSTANTS and MACROS
==============================================================================*/
#define CAM3A_MAX_LUMA_ARRAY_INDEX 9
#define CAM3A_MAX_LUMA_TARGET 112

/* Num frames to require AEC Luma to be settled before
   performing auto focus (frame cnt here in ) */
#define CAM3A_LUMA_SETTLED_BEFORE_AF_CNT 2

/* Auto Frame Rate stability Count in seconds */
//#define CAM3A_AFR_STABLILITY_FRAME_COUNT 15

#define CAMERA_AEC_Q_NUM   10
/*==============================================================================
  DATA TYPES
==============================================================================*/
typedef struct
{
  /* Auto Exposure Control algorithm used */
  camera_auto_exposure_mode_type aec_algo;

  /* AEC aggressiveness selection (tolerance) */
  cam3a_aggressiveness_type aec_aggressiveness;

  uint8 luma_target;        /* Luma value to try and reach */
  uint8 current_luma;       /* current luma value */
  uint8 awb_compensated_luma; /* luma of the frame after it goes through VFE pipeline */
  uint8 luma_tolerance;     /* Tolerance + and - the target */
  /* +1 because this is a C decl */
  uint8 luma_array[CAM3A_MAX_LUMA_ARRAY_INDEX+1];
  uint8 luma_array_index;
  uint8 default_luma_target;   /* default Luma target   */
  uint8 outdoor_luma_target;   /* outdoor Luma target   */
  uint8 lowlight_luma_target;  /* lowlight Luma target   */
  uint8 backlight_luma_target; /* backlight Luma target   */
  boolean full_luma_array;
  int32 prev_exp_index;
  int32 exp_index;             /* Current index into sensor's exp table */
  uint16 indoor_index;          /* Indoor Exposure table threshold       */
  uint16 outdoor_index;         /* Outdoor Exposure table threshold       */
  int32 luma_settled_cnt;      /* Increments when adjust_luma is called
                                  but there is not adjustment made */
  uint16 * bias_table;         /* Table that interprets how the  AEC
                                  stats are summed into a current luma */

  /* When over two tolerances away (above outer or below inner), adjust both
   * gain and line number according to table. Else adjust line number first
   * and at small step and then adjust the gain as well. */
  uint8 outmost_max_luma_target;
  uint8 outmost_min_luma_target;
  uint8 outer_max_luma_target; 
  uint8 outer_min_luma_target;
  uint8 inner_max_luma_target;
  uint8 inner_min_luma_target;

  /* The trigger point for Auto Flash, this ratio applies as
     follows:
     (target_luma / current luma) > ratio; -> then use flash
     Default value is 2 (in Q0 format; from pre-6550 platforms)
     Value here should be in Q8 format, 2 = 512 in Q8          */
  int32 auto_flash_target_to_current_ratio;

  camera_antibanding_type antibanding;

  /* Antibanding tables */
  /* Lengths and pointers to the new exposure tables for
     antibanding */
  uint16 num_50hz_exposure_table_values;
  uint16 num_60hz_exposure_table_values;
  uint16 num_exposure_table_values;

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
  camsensor_chromatix_exposure_table_type * exposure_table_50hz_ptr;
  camsensor_chromatix_exposure_table_type * exposure_table_60hz_ptr;
  /* Current exposure table */
  camsensor_chromatix_exposure_table_type * exposure_table_ptr;
 #endif /* CAMERA_USES_CHROMATIX_0x203 */
  
  

  /* AFR variables (Auto Frame Rate) */
  int32 afr_index;
  int32 frame_in_current_fps;
  int32 afr_frame_rate; /* current frame rate -for tracking */
  float current_real_gain;
  /* Requested max and min FPS - as set by application */
  uint16 afr_max_fps;
  uint16 afr_min_fps;

  /* ISO variables */
  camera_iso_mode_type iso_mode;
  uint16               current_register_gain;
  uint32               current_line_count;

  /* Fast Convergence Related Variables */
  uint8  frame_skip;   /* Number of frames to skip */
  uint16 speed;        /* Q8 Number                */

  uint8 high_luma_region_count;

  uint32 luma_compensation_coefficient_r_gain;
  uint32 luma_compensation_coefficient_g_gain;
  uint32 luma_compensation_coefficient_b_gain;

  uint16 indoor_index_init;
  uint16 outdoor_index_init;

  boolean antibanding_enable;
  boolean afr_enable;

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  float band_60hz_gap;
  float band_50hz_gap;

  float exposure_index_adj_step;
  uint16 outdoor_gamma_index;

  /* HJR related */
  float one_to_two_frame_hjr_offset;

  /* camsensor related */
  camsensor_static_params_type * camsensorParamsPtr;

  /* digital gain */
  float digital_gain;
  float prev_digital_gain;
  float digital_gain_for_snapshot;
  float stored_digital_gain;

  /* variable for multi luma target */
  int32 offset_applied_to_luma;

  uint8 one_to_two_frame_hjr_offset_init;

  /* original exposure table */
  uint16 num_exposure_values_init;
  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #else
  camsensor_chromatix_exposure_table_type * exposure_table_init;
  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  uint32 outdoor_bright_region_discard_threshold;
  uint32 outdoor_bright_region_discarded_cnt;

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #endif /* CAMERA_USES_CHROMATIX_0x203 */
  
} cam3a_aec_state_struct;

/*==============================================================================
  EXTERNAL VARIABLES
==============================================================================*/
extern cam3a_aec_state_struct cam3a_aec_state;

/*==============================================================================
  EXTERNAL API DECLARATIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AEC_INIT

DESCRIPTION
   Memclears global and locally used structure for aec.
   Called on phone power up.

CALLERS:
  cam3a_init ()


DEPENDENCIES
  None

INPUT
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_init (void);

/*===========================================================================

FUNCTION        CAMERA_AEC_MEM_ALLOCATE

DESCRIPTION:

CALLERS:
  cam3a_mem_allocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
boolean camera_aec_mem_allocate (void);

/*===========================================================================

FUNCTION     CAMERA_AEC_MEM_DEALLOCATE

DESCRIPTION:

CALLERS:
  cam3a_mem_deallocate ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Global

=========================================================================== */
void camera_aec_mem_deallocate (void);

/*===========================================================================

FUNCTION      CAMERA_AEC_CONFIG

DESCRIPTION
   Initializes global and locally used structure.  Provides
   tuning parameters for specific implementations

CALLERS:
  cam3a_config ()

DEPENDENCIES
  None

GLOBALS

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_config (
  camsensor_static_params_type * camsensorParamsPtr
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_FLASH_NEEDED

DESCRIPTION
              See if flash is needed. The sensor device driver that supports
              auto flash should set camsensor_params->support_auto_flash to
              true.

CALLERS:
  cam3a_flash_needed ()

DEPENDENCIES
  Checks global AEC state struct for luma values

RETURN VALUE
  TRUE if flash is needed
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_aec_flash_needed(void);

/*===========================================================================

FUNCTION      CAMERA_AEC_GET_AEC_MODE

DESCRIPTION
  Get mode of the Auto Exposure Control

CALLERS:
  cam3a_get_aec_mode()

DEPENDENCIES

RETURN VALUE
  AEC mode in camera_auto_exposure_mode_type

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
camera_auto_exposure_mode_type camera_aec_get_aec_mode (void);

/*===========================================================================

FUNCTION      CAMERA_AEC_IMPLEMENT

DESCRIPTION
   
CALLERS:
  cam3a_parse_and_implement_aec_awb ()

DEPENDENCIES
  camera_3a_shared_parse_regions should be called first

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_implement (
  uint32             num_pixels_per_region_aec,
/* num pixels used for exposure stats in each region */
  camsensor_static_params_type * camsensorParamsPtr,
/* Parameters for camsensor layer        */
  int32               numRegions
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_IMPLEMENT_FOR_MULTISHOT

DESCRIPTION
   
CALLERS:
  cam3a_parse_and_implement_aec_awb_for_snapshot ()

DEPENDENCIES
  camera_3a_shared_parse_regions should be called first

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/

#if defined FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* defined FEATURE_CAMERA_MULTISHOT_SINGLE_ENCODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT */

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_BIAS_TABLE

DESCRIPTION
  Set bias table based on the AEC mode.

DEPENDENCIES

RETURN VALUE
  TRUE if operation completes successfully
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_aec_set_bias_table(
  const camsensor_static_params_type *camsensorParamsPtr 
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_AEC_MODE

DESCRIPTION
  Set mode of the Auto Exposure Control

CALLERS:
  cam3a_set_aec_mode ()

DEPENDENCIES

RETURN VALUE
  TRUE if operation completes successfully
  FALSE if not

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
boolean camera_aec_set_aec_mode(
  camera_auto_exposure_mode_type aec_mode,
  const camsensor_static_params_type *camsensorParamsPtr 
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_ANTIBANDING

DESCRIPTION
   Turn antibanding on (50 or 60hz) or off in AEC.

CALLERS:
  cam3a_set_antibanding ()

DEPENDENCIES
  None

INPUT
  None

GLOBALS
  cam3a_aec_state - Auto exposure varibles and settings

RETURN VALUE
  camera_ret_code_type:
  CAMERA_FAILED  - if this is a CCD sensor or if antibanding is disabled.
  CAMERA_SUCCESS - if everything worked

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_aec_set_antibanding (
  camera_antibanding_type state,
  const camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_BRIGHTNESS

DESCRIPTION
  Set brightness in AEC state and associated variables

CALLERS:
  camera_set_brightness ()

DEPENDENCIES
  None

INPUT

GLOBALS
  cam3a_aec_state - Auto exposure varibles and settings

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_set_brightness (uint8 luma_value);

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_EXPOSURE_COMPENSATION

DESCRIPTION
  Set EV in AEC state and associated variables

CALLERS:
  cam3a_set_exposure_compensation ()

DEPENDENCIES
  None

INPUT

GLOBALS
  cam3a_aec_state - Auto exposure varibles and settings

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_set_exposure_compensation (uint8 luma_value);

#ifdef FEATURE_CAMERA_3A_PROFILING
/*===========================================================================

FUNCTION      CAMERA_AEC_SHOW_TIMING

DESCRIPTION
   Displays the time taken by the aec algorithm for the current frame
   and the average time taken for all the frame processed previously.
      
CALLERS:
  cam3a_show_timing

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_show_timing (void);
#endif /* FEATURE_CAMERA_3A_PROFILING */

/*===========================================================================

FUNCTION      CAMERA_AEC_SET_PARM

DESCRIPTION
  set camera_parm_info in aec

CALLERS:
  CAMERA_SVCS_SET_PARAM

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_aec_set_parm
  (
  camera_parm_type  id,
  int32             parm
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_GET_PARM

DESCRIPTION
  provide camera_parm_info in aec

CALLERS:
  CAMERA_SVCS_GET_PARAM

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_aec_get_parm
  (
  camera_parm_type       id,
  camera_parm_info_type *parm
  );

/*===========================================================================

FUNCTION      CAMERA_AEC_ADJUST_SNAPSHOT_EXPOSURE_FOR_SNAPSHOT

DESCRIPTION
  adjust exposure for snapshot using iso

CALLERS:
  camera_config_vfe()

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type:

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camera_aec_adjust_exposure_for_snapshot (void);

#ifdef FEATURE_CAMERA_HJR
/*===========================================================================

FUNCTION: CAMERA_AEC_PREPARE_FOR_HJR

DESCRIPTION: 
  PREPARES THE INPUT DATA AND RUNS THE HAND JITTER CONTROl ALGO TO DECIDE
  THE EXPOSURE TIME, GAIN, FRAME RATE, NUMBER OF FRAMES TO CAPTURE.

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_prepare_for_hjr(const camsensor_static_params_type *camsensorParamsPtr);

/*===========================================================================

FUNCTION: CAMERA_AEC_ADJUST_EXPOSURE_AS_PER_HJR

DESCRIPTION: 
  Adjusts the exposure and frame rate prior to taking snapshot

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_adjust_exposure_as_per_hjr(void);
#endif /* FEATURE_CAMERA_HJR */
#ifndef CAMERA_USES_SOFTDSP
/*===========================================================================

FUNCTION: CAMERA_AEC_APPLY_DIGITAL_GAIN_FOR_PREVIEW

DESCRIPTION: 
  apply digital gain for preview

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_apply_digital_gain_for_preview(VFE_ColorCorrectionType *packed_cc_ptr);

/*===========================================================================

FUNCTION: CAMERA_AEC_APPLY_DIGITAL_GAIN_FOR_ISO_SNAPSHOT

DESCRIPTION: 
  apply digital gain for snapshot with ISO

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_apply_digital_gain_for_iso_snapshot(VFE_ColorCorrectionType *packed_cc_ptr);
#endif
/*===========================================================================

FUNCTION: CAMERA_AEC_UPDATE_COLOR_CORRECTION_MATRIX

DESCRIPTION: 
  update color correction matrix with normal and low-light color correction matrice

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/

void camera_aec_update_color_correction_matrix(
  camera_3x3_rgb_matrix_type * sourceCC, 
  camera_3x3_rgb_matrix_type * low_light_CC_ptr);

/*===========================================================================

FUNCTION: CAMERA_AEC_RETRIVE_EXPOSURE_INFO

DESCRIPTION: 
  retrive aec exposure info

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
void camera_aec_retrive_exposure_info(
  boolean aec_enable,
  camera_exposure_info_request_type request_type,
  camera_exposure_info_type * exposure_info_ptr);

/*===========================================================================

FUNCTION      CAMERA_AEC_RESTORE_CONFIG_FOR_PREVIEW

DESCRIPTION
  restore aec gain and line_count for preview restart
   
CALLERS:
  camera_restore_config_for_preview()

===========================================================================*/
void camera_aec_restore_config_for_preview(void);

/*===========================================================================

FUNCTION      CAMERA_AEC_RESTORE_CONFIG_FOR_PREVIEW

DESCRIPTION
  restore aec gain and line_count for preview restart
   
CALLERS:
  camera_restore_config_for_preview()

===========================================================================*/

boolean camera_aec_settled(cam3a_aec_state_struct *aec_state);

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_UPDATE_INDOOR_OUTDOOR_INDICES_PER_FPS

DESCRIPTION
  Modifies the indoor/outdoor indices as per the frame rate changes.

GLOBALS
  indoor_index_org
  outdoor_index_org

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_aec_core_update_indoor_outdoor_indices_per_fps 
  (
  /* Index into the supported frame rates table */
  int16 index,
  uint16 fps
  );

/* ==========================================================================*/
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
#endif /* CAMERA_AEC_H */

