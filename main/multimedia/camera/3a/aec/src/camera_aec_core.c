/*=============================================================================
FILE:       camera_aec_core.c

SERVICES:   Core Auto Exposure Control File

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/aec/main/latest/src/camera_aec_core.c#11 $ $DateTime: 2009/04/20 02:28:51 $ $Author: c_caluru $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 04/20/09   cg       Changes to snapshot brightness same as preview.
 04/14/09   cg       Number of exposure values in Antibanding should be same as that of input exposure table.( Fix for CR # 178815)
 04/07/09   cg       Re-Enabling Feature FEATURE_CAMERA_BURST_MODE
 03/18/09   chai     Fixed Rvct compiler Warning
 03/12/09   cg       Fixed the dark snapshot issue for BMP builds.
 12/26/08   cg       Updates for AEC Multishot Feature
 11/24/18   chai     Fixed compiler warnings
 11/20/08   chai     Ez Tune1.0 and 3A 1.4 Updates
 04/02/08   hap      Fixed Klocwork errors
 11/08/07   hap      Fixing RVCT2.2 compiler warnings
 05/24/07   klee     Enhanced outdoor image quality by discarding too bright regions
 03/30/07   klee     Clean-up lint errors due to un-matched data type in 3a
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/26/07   klee     Enhanced camera_aec_core_update_indoor_outdoor_indices() 
                     not to apply fixed compensation, CAM3A_AFR_COMPENSATION_FOR_FPS_CHAHGE
 02/26/07   klee     Enhanced camera_aec_core_apply_multi_luma_target() with AFR
 02/21/07   klee     Enhanced AEC algorithm by applying multi luma target
 02/20/07   klee     Fixed all lint errors (critical, high, medium, low and lint)
 01/22/07   klee     Enhanced build_band_table ft. to support flicker_detecting in 7k
 01/05/07   klee     Fixed flickering issue with AEC digital gain
 12/21/06   klee     Added comment on luma compensation with wb gains
 12/04/06   klee     Fixed CR 107492 Camera snapshotis very dark under super bright light
 12/04/06   klee     Enhanced AEC luma compensation to be controllable
 11/30/06   klee     Fixed CR 107322 - AFR is not working with AEC digital gain
 11/20/06   ven      Fixed the CR 106554 on HJR exposure should be half of regular snapshot.
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Added AEC Digital gain implementation
 10/27/06   dlee     Reverted luma_settled_cnt to CAM3ADEBUG.
 10/20/06   jv       Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/11/06   ven      Added support for HJR tuning parameter to delay the
                     selection of multi frame registration based HJR.
 09/22/06   klee     Update AEC luma_settled_cnt message to be HIGH
 09/22/06   klee     Fix the bug on float->Q7 converting
 09/15/06   klee     Cleaned up the lint errors
 09/13/06   klee     Added outdoor_gamma_index for outdoor gamma table
 09/11/06   klee     Updated camera_aec_core_calculate_current_luma to prevent potential bug
 09/08/06   klee     Updated camera_aec_core_fast_convergence to use chromatix variable
 09/01/06   kadhir   Removed unwanted Cross Compilation Dependency 
                     When qcamcorder Feature Disabled 
 08/31/06   klee     Updated antibanding calculation with camsensorParamsPtr->pixel_clock_per_line
 08/31/06   klee     Bug fix on previous and current wb maintaining
 08/29/06   klee     Consolidated antibanding calculation
 08/21/06   klee     Fixed ECR99913 - crash on camera_aec_core_init()
                     by putting protection in case get_preview_pixels_per_line is NULL
 08/18/06   klee     Fixed critical and high lint errors
 08/16/06   klee     Fixed the bug with ISO - snapshot_linecount cannot be 0
 08/11/06   klee     Cleaned up the code regarding tuning variables
                     - AEC
 08/04/06   dl       Removed extern variable camctrl_tbl.
 07/26/06   klee     Moved in AEC core algorithm from camera_aec
 07/19/06   klee     Initial code

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#ifndef CAMERA_USES_SOFTDSP
#ifndef FEATURE_CAMERA_YCBCR_ONLY
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "camera_aec_core.h"
#include "camera_aec.h"
#include "camctrl.h"
#include "camera.h"
#include "camera_svcs.h"

/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/
/* To hold the dats for dark region suppression */
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /*  CAMERA_USES_CHROMATIX_0x203 */
/*===========================================================================

FUNCTION    CAMEMA_AEC_CORE_BUILD_BAND_TABLE

DESCRIPTION
          Create antibanding exposure table.

DEPENDENCIES
  None

RETURN VALUE
  Number of locations in new band table
  FALSE if not

SIDE EFFECTS
  None

===========================================================================*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
static uint16 camera_aec_core_build_band_table(
  const camsensor_static_params_type * camsensorParamsPtr,
  camsensor_chromatix_exposure_table_type *table,
  float antiband_gap_line,
  uint16 start_band_count
  );
#endif /* CAMERA_USES_CHROMATIX_0x203 */

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_FAST_CONVERGENCE_CONFIG

DESCRIPTION
  Update fast convergence related variables.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_fast_convergence_config( void );

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_FAST_CONVERGENCE

DESCRIPTION
  Find exposure index with current luma with fast convergence algorithm

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_fast_convergence(uint8 current_vfe_luma);

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_SLOW_CONVERGENCE

DESCRIPTION
  Find exposure index with current luma with slow convergence algorithm

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_slow_convergence(uint8 current_vfe_luma);

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_CONFIGURE_SNAPSHOT_LUMA_TARGET

DESCRIPTION   adjust luma target depending on count of high luma regions

DEPENDENCIES
  None

INPUT
  snapshot config pointer
  
SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_configure_snapshot_luma_target(
  void * snapshot_config
  );

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_APPLY_MULTI_LUMA_TARGET

DESCRIPTION
  Apply multi luma target to correct current_vfe_luma

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static uint8 camera_aec_core_apply_multi_luma_target(uint8 current_vfe_luma);

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_DARK_REGION_REDUCTION_CONFIG

DESCRIPTION
  Configure reduction rate based on gamma table

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /*CAMERA_USES_CHROMATIX_0x203*/
/*==============================================================================
  INTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION    CAMEMA_AEC_BUILD_BAND_TABLE

DESCRIPTION
          Create antibanding exposure table.

DEPENDENCIES
  None

RETURN VALUE
  Number of locations in new band table
  FALSE if not

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e550*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else

static uint16 camera_aec_core_build_band_table(
  const camsensor_static_params_type * camsensorParamsPtr,
  camsensor_chromatix_exposure_table_type *table,
  float antiband_gap_line,
  uint16 start_band_count
  )
#endif
{
  uint16 line_count, gain;
  uint16 antiband_start_line;
  uint16 gap;
  float real_gain, new_real_gain, pre_real_gain, next_real_gain;
  uint16 i, j;
  uint16 max_exp_index;
  // REVISIT
  //float min_gain_step;

  if(camsensorParamsPtr == NULL || table == NULL) return 0;

  //min_gain_step = camsensorParamsPtr->chromatix_parms->aec_min_gain_step;

  if(antiband_gap_line == 0.0)
  {
    return 0;
  }

  max_exp_index = cam3a_aec_state.num_exposure_values_init;

  gain = 
    cam3a_aec_state.exposure_table_init[cam3a_aec_state.exp_index].gain;
  antiband_start_line = (uint16)(antiband_gap_line * start_band_count);

  for(i=0, j=0; i<max_exp_index; i++)
  {
    if( cam3a_aec_state.exposure_table_init[i].line_count >= 
      antiband_start_line )
    {
      /* Calculates the gain compensation requiered to keep exposure
      * withing time constraints to remove banding.*/
      gap = 
        (uint16)(cam3a_aec_state.exposure_table_init[i].line_count /
        antiband_gap_line);
      line_count = (uint16)(gap * antiband_gap_line);

      if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
      {
        real_gain = (float) (cam3a_aec_state.exposure_table_init[i].gain) / 256; //Q8 value in table
        if(line_count != 0)
        {
          real_gain *= 
            (float) (cam3a_aec_state.exposure_table_init[i].line_count) / 
            line_count;
        }
        gain = (uint16) (real_gain * 256);
      }
      else /* no digital gain */
      {
        real_gain = camsensor_register_to_real_gain(
          cam3a_aec_state.exposure_table_init[i].gain);
        if(line_count != 0)
        {
          real_gain *= 
            (float) (cam3a_aec_state.exposure_table_init[i].line_count) / 
            line_count;
        }
        gain = camsensor_real_to_register_gain(real_gain);

        /* Verify new gain and exposure is not greater then next index
           gain x line_count, nor smaller than previous index gain x line_count */
        pre_real_gain = 0.0;
        next_real_gain = 0.0;
        new_real_gain = 0.0;

        if(i > 1)
        {
          pre_real_gain = camsensor_register_to_real_gain(
            cam3a_aec_state.exposure_table_init[i-1].gain);
          next_real_gain = camsensor_register_to_real_gain(
            cam3a_aec_state.exposure_table_init[i+1].gain);
          new_real_gain = camsensor_register_to_real_gain(gain);
        }

        if((pre_real_gain * table[j-1].line_count) > (new_real_gain * line_count))
        {
          /* Increase gain to prevent exposure being less than previous index */
          gain = camsensor_real_to_register_gain(new_real_gain) + 1;
        }

        if( (next_real_gain * 
          cam3a_aec_state.exposure_table_init[i+1].line_count) < 
          (new_real_gain * line_count) )
        {
          /* Decrease gain to prevent exposure being more than next index */
          gain = camsensor_real_to_register_gain(new_real_gain) - 1;
        }
      } /* no digital gain */

      table[j].line_count = line_count;
      table[j].gain = gain;
      j++;
    }
    else
    {
      table[j].line_count = 
        cam3a_aec_state.exposure_table_init[i].line_count;
      table[j].gain = cam3a_aec_state.exposure_table_init[i].gain;
      j++;
    }
  }

  j--;

  return j;
} /* cam3a_build_band_table */
/*lint -restore*/

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_FAST_CONVERGENCE_CONFIG

DESCRIPTION
  Update fast convergence related variables.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_fast_convergence_config( void )
{
  switch(cam3a_aec_state.aec_aggressiveness)
  {
    case  CAM3A_LOW_AGGRESSIVENESS:
      cam3a_aec_state.speed = (uint16)(0.1 * Q8);
      cam3a_aec_state.frame_skip = 1;
      break;

    case  CAM3A_MED_AGGRESSIVENESS:
      cam3a_aec_state.speed = (uint16)(0.3 * Q8);
      cam3a_aec_state.frame_skip = 1;
      break;

    case  CAM3A_HIGH_AGGRESSIVENESS:
      cam3a_aec_state.speed = (uint16)(0.8 * Q8);
      cam3a_aec_state.frame_skip = 1;      
      break;

    default: 
      cam3a_aec_state.speed = (uint16)(0.5 * Q8);
      cam3a_aec_state.frame_skip = 1;      
      break;

  }
}

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_FAST_CONVERGENCE

DESCRIPTION
              Adjust exposure on the camera sensor.  Implement a 
              generic algorithm that can work with any sensor

CALLERS:
  camera_aec_core_run_algorithm ()

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type:
  CAMERA_FAILED  - if we were unable to update sensor
  CAMERA_SUCCESS - if everything worked

SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_fast_convergence(uint8 current_vfe_luma)
{
  uint8  weighted_luma = 128;
  int8   luma_delta = 0;
  int32  exp_index_adjust = 0; 
  float  index_adjust;

  weighted_luma = current_vfe_luma;

#if defined  FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE && FEATURE_CAMERA_AEC_DURING_MULTISHOT */
 {
 
  if((cam3a_aec_state.frame_in_current_fps % (cam3a_aec_state.frame_skip+1)) == 0)
    return;
 }

  luma_delta = (int8) weighted_luma - (int8) cam3a_aec_state.luma_target;

  /*Luma Tolerance verification, if true, we adjust exposure */
  if( (uint8) (abs(luma_delta))  > cam3a_aec_state.luma_tolerance )
  {
    cam3a_aec_state.luma_settled_cnt = 0;

    if(weighted_luma ==0)
      weighted_luma = 1;

    index_adjust = 
      (float)((log10((double) cam3a_aec_state.luma_target/ weighted_luma))*cam3a_aec_state.exposure_index_adj_step);

    if(index_adjust < 0)
    {
      exp_index_adjust = (int32) ((cam3a_aec_state.speed * index_adjust)/Q8);

      if(exp_index_adjust ==0)
        exp_index_adjust=exp_index_adjust-1;
    }
    else
    {
      exp_index_adjust = (int32) ((cam3a_aec_state.speed * index_adjust * .5)/Q8 ); /* halve speed to go to darker scene.. */

#if defined  FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT    
#error code not present
 #endif /* defined  FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT */
      if(exp_index_adjust == 0)
        exp_index_adjust = exp_index_adjust+1;
    }

    cam3a_aec_state.exp_index = cam3a_aec_state.exp_index + exp_index_adjust;

  }
  else
  {
    index_adjust = 0;
    cam3a_aec_state.luma_settled_cnt++;
    MSG_CAM3ADEBUG("Current luma settled count : [%d] ", cam3a_aec_state.luma_settled_cnt, 0, 0);

  }

  /* Verify AEC table boundary */
  if((cam3a_aec_state.exp_index + exp_index_adjust) < 0)
    cam3a_aec_state.exp_index = 0;

  /* Subract 1 from num exposure values to get max exposure index */
  if(cam3a_aec_state.exp_index > (int32)(cam3a_aec_state.num_exposure_table_values-1))
  {
    cam3a_aec_state.exp_index = (int32)(cam3a_aec_state.num_exposure_table_values-1);
  }

}

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_SLOW_CONVERGENCE

DESCRIPTION
              Adjust exposure on the camera sensor.  Implement a 
              generic algorithm that can work with any sensor

CALLERS:
  camera_aec_core_run_algorithm ()

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type:
  CAMERA_FAILED  - if we were unable to update sensor
  CAMERA_SUCCESS - if everything worked

SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_slow_convergence(uint8 current_vfe_luma)
{
  int16  weight[] = {60,25,15,0,0,0,0,0,0,0};
  uint8  avg_center_weighted_luma = 128;
  uint8  oldest_luma_entry;
  uint8  luma_index, weight_index;
  int32  luma_sum = 0;

  if(cam3a_aec_state.luma_array_index > CAM3A_MAX_LUMA_ARRAY_INDEX)
  {
    cam3a_aec_state.luma_array_index = 0;
    cam3a_aec_state.full_luma_array = TRUE;
  }
  cam3a_aec_state.luma_array[cam3a_aec_state.luma_array_index] = current_vfe_luma;

  /* Calculating the weighted moving average of last 10 frames of
   * camera_center weighted luma. This moving average is used for
   * outdoor light where the illuminance is constantly fluctuate  */

  /* oldest entry in luma_array is the next entry of current entry */
  if(cam3a_aec_state.luma_array_index == CAM3A_MAX_LUMA_ARRAY_INDEX)
    oldest_luma_entry = 0;
  else
    oldest_luma_entry = cam3a_aec_state.luma_array_index + 1;

  luma_index = oldest_luma_entry;
  weight_index = CAM3A_MAX_LUMA_ARRAY_INDEX;

  /* calculate the weighted sum of last 10 frames's luma value */
  do
  {
    luma_sum += (cam3a_aec_state.luma_array[luma_index] * weight[weight_index]);
    luma_index++;
    weight_index--;
    if(luma_index > CAM3A_MAX_LUMA_ARRAY_INDEX)
      luma_index = 0;
  } while(luma_index != oldest_luma_entry);

  if(luma_sum >= 0)
  {
    luma_sum = luma_sum / 100;
    avg_center_weighted_luma = (uint8)CLIPI(luma_sum);
  }
  else
  {
    /*
    ** If the value is negative going from Bright to Dark
    */
    avg_center_weighted_luma = current_vfe_luma;
  }
  cam3a_aec_state.luma_array_index++;


  if(cam3a_aec_state.luma_array_index > CAM3A_MAX_LUMA_ARRAY_INDEX)
  {
    cam3a_aec_state.luma_array_index = 0;
    cam3a_aec_state.full_luma_array = TRUE;
  }

  if(avg_center_weighted_luma >= cam3a_aec_state.luma_target)
  {
    int cnt_temp = cam3a_aec_state.luma_settled_cnt;
    cam3a_aec_state.luma_settled_cnt = 0;

    if(avg_center_weighted_luma < cam3a_aec_state.inner_max_luma_target)
    {
      /* No need to adjust, increment luma settled to keep track of the
        relative brightness stability of the subject */
      cam3a_aec_state.luma_settled_cnt = cnt_temp+1;
      MSG_CAM3ADEBUG("Current luma settled count : [%d] ", cam3a_aec_state.luma_settled_cnt, 0, 0);
    }
    else if(avg_center_weighted_luma < cam3a_aec_state.outer_max_luma_target)
    {
      cam3a_aec_state.exp_index--;
    }
    else if(avg_center_weighted_luma < cam3a_aec_state.outmost_max_luma_target)
    {
      if(camctrl_tbl.camsensor_params_ptr->preview_fps == camctrl_tbl.camsensor_params_ptr->max_preview_fps)
      {
        cam3a_aec_state.exp_index--;
      }
      else
      {
        cam3a_aec_state.exp_index -= 2;
      }
    }
    else
    {
      if(cam3a_aec_state.aec_aggressiveness == CAM3A_LOW_AGGRESSIVENESS)
      {
        cam3a_aec_state.exp_index -= 4;
      }
      else if(cam3a_aec_state.aec_aggressiveness == CAM3A_MED_AGGRESSIVENESS)
      {
        cam3a_aec_state.exp_index -= 6;
      }
      else if(cam3a_aec_state.aec_aggressiveness == CAM3A_HIGH_AGGRESSIVENESS)
      {
        cam3a_aec_state.exp_index -= 8;
      }
    }

    if(cam3a_aec_state.exp_index < 0)
      cam3a_aec_state.exp_index = 0;
  }
  else
  {
    int cnt_temp = cam3a_aec_state.luma_settled_cnt;
    cam3a_aec_state.luma_settled_cnt = 0;

    if(avg_center_weighted_luma > cam3a_aec_state.inner_min_luma_target)
    {
      /* No need to adjust, increment luma settled to keep track of the
        relative brightness stability of the subject */
      cam3a_aec_state.luma_settled_cnt = cnt_temp+1;
      MSG_CAM3ADEBUG("Current luma settled count : [%d] ", cam3a_aec_state.luma_settled_cnt, 0, 0);
    }
    else if(avg_center_weighted_luma > cam3a_aec_state.outer_min_luma_target)
    {
      cam3a_aec_state.exp_index++;
    }
    else if(avg_center_weighted_luma > cam3a_aec_state.outmost_min_luma_target)
    {
      if(camctrl_tbl.camsensor_params_ptr->preview_fps == camctrl_tbl.camsensor_params_ptr->max_preview_fps)
      {
        cam3a_aec_state.exp_index++;
      }
      else
      {
        cam3a_aec_state.exp_index += 2;
      }
    }
    else
    {
      if(cam3a_aec_state.aec_aggressiveness == CAM3A_LOW_AGGRESSIVENESS)
      {
        cam3a_aec_state.exp_index += 4;
      }
      else if(cam3a_aec_state.aec_aggressiveness == CAM3A_MED_AGGRESSIVENESS)
      {
        cam3a_aec_state.exp_index += 6;
      }
      else if(cam3a_aec_state.aec_aggressiveness == CAM3A_HIGH_AGGRESSIVENESS)
      {
        cam3a_aec_state.exp_index += 8;
      }
    }

    /* Subract 1 from num exposure values to get max exposure index */
    if(cam3a_aec_state.exp_index > (int32)(cam3a_aec_state.num_exposure_table_values-1))
    {
      cam3a_aec_state.exp_index = (int32)(cam3a_aec_state.num_exposure_table_values-1);
    }

  } /* end of current_vfe_luma <  condition */

}

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_CONFIGURE_SNAPSHOT_LUMA_TARGET

DESCRIPTION   adjust luma target depending on count of high luma regions

DEPENDENCIES
  None

INPUT
  snapshot config pointer
  
SIDE EFFECTS
  None

===========================================================================*/
static void camera_aec_core_configure_snapshot_luma_target(void * snapshot_config)
{
  camsensor_snapshot_config_type * snapshot_config_ptr;
  uint16 gain_on_indoor_index;
  float real_gain_on_indoor_index;

  snapshot_config_ptr = (camsensor_snapshot_config_type *)snapshot_config;  
  /* adjust snapshot luma target */
  if(cam3a_aec_state.high_luma_region_count < 2)
  {
    snapshot_config_ptr->luma_target = cam3a_aec_state.luma_target; //no compensation
  }
  else if(cam3a_aec_state.high_luma_region_count >= 8)
  {
    snapshot_config_ptr->luma_target = cam3a_aec_state.luma_target - 8; //10;
  }
  else if(cam3a_aec_state.high_luma_region_count >= 4)
  {
    snapshot_config_ptr->luma_target = cam3a_aec_state.luma_target - 5; //6; 
  }
  else if(cam3a_aec_state.high_luma_region_count >= 2)
  {
    snapshot_config_ptr->luma_target = cam3a_aec_state.luma_target - 3; //4;
  }
  //
  if(camctrl_tbl.nightshot_is_supported == FALSE)
  {
    snapshot_config_ptr->luma_target = cam3a_aec_state.luma_target;
  }
  /* adjust current_real_gain for further ISO process, ONLY IF indoor case */
  gain_on_indoor_index 
    = cam3a_aec_state.exposure_table_ptr[cam3a_aec_state.indoor_index].gain;
  real_gain_on_indoor_index = camsensor_register_to_real_gain(gain_on_indoor_index);

  if(cam3a_aec_state.current_real_gain > real_gain_on_indoor_index) /* > 2 */
  {
    float gain_adj_ratio = 
      (float)snapshot_config_ptr->luma_target/cam3a_aec_state.luma_target;
    cam3a_aec_state.current_real_gain = cam3a_aec_state.current_real_gain*gain_adj_ratio;
  }
}

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_APPLY_MULTI_LUMA_TARGET

DESCRIPTION
  Apply multi luma target to correct current_vfe_luma

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static uint8 camera_aec_core_apply_multi_luma_target(uint8 current_vfe_luma)
{
  uint8 outdoor_target, indoor_target; 
  int32 offset, temp;
  float ratio;

  offset = 0;
  ratio = 1.0;
  indoor_target = cam3a_aec_state.luma_target; 
  /*lint -save -e40*/
  temp = (int32)( ( (float)(cam3a_aec_state.outdoor_luma_target*cam3a_aec_state.luma_target) / 
                    (float)(cam3a_aec_state.default_luma_target) ) );
  /*lint -restore*/
  if (temp > 255)
  {
    outdoor_target = 255;
  }
  else
  {
    outdoor_target = (uint8) temp;
  }

  if (cam3a_aec_state.exp_index < (int32)cam3a_aec_state.outdoor_index)
  {
    /* apply outdoor luma target */
    offset = outdoor_target - indoor_target;
  }
  else if (cam3a_aec_state.exp_index < (int32)cam3a_aec_state.indoor_index)
  {
    ratio = (float)( (float)(cam3a_aec_state.exp_index - cam3a_aec_state.outdoor_index) /
                     (float)(cam3a_aec_state.indoor_index - cam3a_aec_state.outdoor_index) );
    offset = (int32)( ((1.0-ratio)*(float)outdoor_target) + (ratio*(float)indoor_target) );
    offset = offset - indoor_target;
  }
  else if (cam3a_aec_state.exp_index < (int32)(cam3a_aec_state.num_exposure_table_values -1))
  {
    /* apply default luma target */
    offset = 0;
  }

  if (offset > current_vfe_luma)
    offset = current_vfe_luma;

  temp = current_vfe_luma - offset;

  if (temp > 255)
    temp = 255;
  if (temp < 0)
    temp = 0;

  cam3a_aec_state.offset_applied_to_luma = offset;
  
  return (uint8)temp;
}

/*==============================================================================
  EXTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_MEM_ALLOCATE

DESCRIPTION
  allocate memory for AEC

CALLERS:
  camera_aec_mem_allocate

RETURN VALUE
  None

===========================================================================*/
boolean camera_aec_core_mem_allocate (void)
{
  /* Allocate two exposure tables for Antibanding.  Although note
    that it is only CMOS sensors with need these */
  CAM_Free( cam3a_aec_state.exposure_table_50hz_ptr );
  CAM_Free( cam3a_aec_state.exposure_table_60hz_ptr );
  /* Allocate enough entries for 2 full sized exposure tables */
  cam3a_aec_state.exposure_table_50hz_ptr =
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
    CAM_Malloc(
    sizeof(camsensor_chromatix_exposure_table_type)*MAX_EXPOSURE_TABLE_SIZE );
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  if(cam3a_aec_state.exposure_table_50hz_ptr == NULL)
  {
    camera_no_memory();
    return FALSE;
  }
  cam3a_aec_state.exposure_table_60hz_ptr =
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
    CAM_Malloc(
    sizeof(camsensor_chromatix_exposure_table_type)*MAX_EXPOSURE_TABLE_SIZE );
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  if(cam3a_aec_state.exposure_table_60hz_ptr == NULL)
  {
    camera_no_memory();
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_MEM_DEALLOCATE

DESCRIPTION
  free the memory allocated for AEC

CALLERS:
  camera_aec_mem_deallocate

RETURN VALUE
  None

===========================================================================*/
void camera_aec_core_mem_deallocate (void)
{
  CAM_Free( cam3a_aec_state.exposure_table_50hz_ptr );
  cam3a_aec_state.exposure_table_50hz_ptr = NULL;

  CAM_Free( cam3a_aec_state.exposure_table_60hz_ptr );
  cam3a_aec_state.exposure_table_60hz_ptr = NULL;
}

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_INIT

DESCRIPTION
  initiate core AEC algorithm

CALLERS:
  camera_aec_config()

RETURN VALUE
  None

===========================================================================*/
/*lint -save -e818 */
void camera_aec_core_init(camsensor_static_params_type* camsensorParamsPtr)
{
  /* uint8 camera_vfe_luma = 128; was in camera_svcs.c */
  cam3a_aec_state.current_luma = 128;

  cam3a_aec_state.indoor_index = cam3a_aec_state.indoor_index_init;
  cam3a_aec_state.outdoor_index = cam3a_aec_state.outdoor_index_init;

  /* Just copy to a "cam3a" struct so that we have a local copy */
  /* For flash support */
  cam3a_aec_state.auto_flash_target_to_current_ratio =
    camsensorParamsPtr->auto_flash_target_to_current_ratio;

  if(cam3a_aec_state.antibanding_enable == TRUE)
  {
    /* generate antibanding table */
    /* Get max number of indexes in exposure table */
    uint16 start_band_count = 1;

    /* band_gap defines how multiple of lines of integration are required to get rid
     of beat frequency */
    if(camctrl_tbl.get_preview_pixels_per_line)
    {
      cam3a_aec_state.band_60hz_gap =
        (float)( ((float)1/120) * ( ((float)(camsensorParamsPtr->pixel_clock)) /
        ((float)(camctrl_tbl.get_preview_pixels_per_line())) ) );
    }
    else
    {
      cam3a_aec_state.band_60hz_gap =
        (float)( ((float)1/120) * ( ((float)(camsensorParamsPtr->pixel_clock)) /
        ((float)(camsensorParamsPtr->pixel_clock_per_line)) ) );
    }

    cam3a_aec_state.band_50hz_gap = (float)(cam3a_aec_state.band_60hz_gap * 120.0 / 100.0);
    cam3a_aec_state.band_50hz_gap = (int)(cam3a_aec_state.band_50hz_gap + 0.5);
    cam3a_aec_state.band_60hz_gap = (int)(cam3a_aec_state.band_60hz_gap + 0.5);


    /* Takes pointer that has max number of exposure entries it can
      accomodate and creates the banding table.  Return value is the
      number of actual indices in the new table */
    cam3a_aec_state.num_50hz_exposure_table_values =
      camera_aec_core_build_band_table(camsensorParamsPtr,
      cam3a_aec_state.exposure_table_50hz_ptr,
      cam3a_aec_state.band_50hz_gap, start_band_count);

    cam3a_aec_state.num_60hz_exposure_table_values =
      camera_aec_core_build_band_table(camsensorParamsPtr,
      cam3a_aec_state.exposure_table_60hz_ptr,
      cam3a_aec_state.band_60hz_gap, start_band_count);
  }

  /* Set Fast convergence variables */
  cam3a_aec_state.frame_in_current_fps = 0;
  camera_aec_core_fast_convergence_config();

  /* luma compensation coefficients in fixed point num with rounding */
  cam3a_aec_state.luma_compensation_coefficient_r_gain 
    = (int)(0.299*(float)(1 << CAMERA_AEC_Q_NUM)+ 0.5);
  cam3a_aec_state.luma_compensation_coefficient_g_gain 
    = (int)(0.587*(float)(1 << CAMERA_AEC_Q_NUM)+ 0.5);
  cam3a_aec_state.luma_compensation_coefficient_b_gain 
    = (int)(0.114*(float)(1 << CAMERA_AEC_Q_NUM)+ 0.5);

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #endif /* FEATURE_CAMERA_CHROMATIX_0x203 */

 

}
/*lint -restore */

/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_CALCULATE_CURRENT_LUMA

DESCRIPTION
  Calculates the luma for the preview frame from VFE statistics
  & based on an input weight table.  This function also explicitly
  knows about frame_average mode for AEC where no bias weight is applied
  (i.e. every AEC region is weighed into the current luma calc
   evenly).
  Otherwise, the passed in table of Q8 format numbers is applied
  to the VFE array of brightness sums.  The table may be center
  weighted, spot metering, or virtually any other combination.

  Applies for 8x8 arrays and 16x16 arrays.

DEPENDENCIES
  None

RETURN VALUE
  Current Luma

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e818 -e737 sumLumaArray could be declared as pointing to const */
uint8 camera_aec_core_calculate_current_luma (
  int32 numRegions,
/* Num regions in input sum of luma array  */
  uint32 * sumLumaArray,
/* Input sum of luma array            */
  int32 pixelsPerRegion
/* num pixels used for exposure stats in each region */
  )
{
  uint32 lumaSum = 0;
  uint32 spotLuma = 0;
  int32 i;
  uint32 high_luma_region_threshold;

  uint32 weightSum = 0;
  uint32 high_luma_region_threshold_per_outdoor, temp_threshold, aec_int_range;
  uint32 discarded_regions, range;
  uint16 temp_bias_weight = 1;
  float ratio;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */

if (sumLumaArray == NULL) return 0;

MSG_LOW ("cam3a_calculate_current_luma", 0,0,0);

if (pixelsPerRegion <= 0) {
    MSG_FATAL ("cam3a pixelsPerRegion <= 0; & therefore bad. %d", 
               pixelsPerRegion, 0, 0);
    return(0);
}

discarded_regions = 0;            
temp_threshold = camctrl_tbl.aec_outdoor_bright_region_threshold_LO;
range = (cam3a_aec_state.indoor_index - cam3a_aec_state.outdoor_index)/4;


#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else /* CAMERA_USES_CHROMATIX_0x203*/

/*lint -save -e574 */
if (cam3a_aec_state.exp_index < cam3a_aec_state.outdoor_index/2) {
    temp_threshold = (uint32) (camctrl_tbl.aec_outdoor_bright_region_threshold_HI);
    aec_int_range = 1;
} else if (cam3a_aec_state.exp_index < (cam3a_aec_state.outdoor_index-range)) {
    aec_int_range = (cam3a_aec_state.outdoor_index-range) - cam3a_aec_state.outdoor_index/2;
    ratio = (float) ((cam3a_aec_state.outdoor_index-range) - cam3a_aec_state.exp_index)/ aec_int_range;
    temp_threshold = (uint32) (temp_threshold*(1-ratio) + (camctrl_tbl.aec_outdoor_bright_region_threshold_HI)*ratio);
} else if (cam3a_aec_state.exp_index < (cam3a_aec_state.outdoor_index+range)) {
    aec_int_range = range*2;
    ratio = (float) (cam3a_aec_state.outdoor_index+range - cam3a_aec_state.exp_index)/ aec_int_range;
    temp_threshold = (uint32) (256*(1-ratio) + temp_threshold*ratio);
} else {
    temp_threshold = 256;
    aec_int_range = 0;
}
/*lint -restore */

if (camctrl_tbl.aec_outdoor_bright_region_is_supported == TRUE) {
    cam3a_aec_state.outdoor_bright_region_discard_threshold = temp_threshold;
} else {
    cam3a_aec_state.outdoor_bright_region_discard_threshold = 256 ;
}

#endif /* CAMERA_USES_CHROMATIX_0x203 */

/* The bayer data that is considered to be too bright */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else /* CAMERA_USES_CHROMATIX_0x203 */
high_luma_region_threshold 
= pixelsPerRegion*camctrl_tbl.high_luma_region_threshold;
cam3a_aec_state.high_luma_region_count = 0;

high_luma_region_threshold_per_outdoor 
= pixelsPerRegion*cam3a_aec_state.outdoor_bright_region_discard_threshold;

#endif /* CAMERA_USES_CHROMATIX_0x203 */
/* This check for frame average is specifically to avoid
  having a frame average bias table that contains all ones
  in it - this just seems like a waste of memory! */
if (cam3a_aec_state.aec_algo == CAMERA_AEC_FRAME_AVERAGE) {
    for (i = 0; i < numRegions; i++) {
        /* Just add 'em up */
        lumaSum += sumLumaArray[i];
        if (sumLumaArray[i] > high_luma_region_threshold) {
            cam3a_aec_state.high_luma_region_count++;
        }
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else /* CAMERA_USES_CHROMATIX_0x203 */

        if (sumLumaArray[i] > high_luma_region_threshold) {
            cam3a_aec_state.high_luma_region_count++;
        }
        if ((sumLumaArray[i] > high_luma_region_threshold_per_outdoor) && 
            (discarded_regions < (numRegions*camctrl_tbl.aec_outdoor_bright_region_discard_ratio))) {
            discarded_regions++;
            lumaSum -= (uint32)(sumLumaArray[i]*(1-camctrl_tbl.aec_outdoor_bright_region_reduction));
        }
    }
    weightSum = ((uint32)(numRegions - discarded_regions*(1-camctrl_tbl.aec_outdoor_bright_region_reduction)))<<8;
#endif /* CAMERA_USES_CHROMATIX_0x203 */
}/* if  .... */

else {
    if (cam3a_aec_state.bias_table == NULL) {
        MSG_FATAL ("cam3a ERROR bias_table pointer is zero for camera AEC aborting.", 
                   0, 0, 0);
        return(0);
    }

    for (i = 0; i < numRegions; i++) {
        /* Multiply each entry by Q8 format bias table and sum  results
          On the bias table:
          Table that interprets how the  AEC stats are summed into a
          current luma  */
        if (sumLumaArray[i] > high_luma_region_threshold) {
            cam3a_aec_state.high_luma_region_count++;
        }

#ifdef CAMERA_USES_CHROMATIX_0x203 
#error code not present
#else /* CAMERA_USES_CHROMATIX_0x203 */
        if (sumLumaArray[i] > high_luma_region_threshold) {
            cam3a_aec_state.high_luma_region_count++;
        }

        if (
           (sumLumaArray[i] > high_luma_region_threshold_per_outdoor) && 
           (discarded_regions < (numRegions*camctrl_tbl.aec_outdoor_bright_region_discard_ratio)) &&
           (cam3a_aec_state.aec_algo != CAMERA_AEC_SPOT_METERING)
           ) {
            discarded_regions++;
            temp_bias_weight = (uint16) (cam3a_aec_state.bias_table[i] * camctrl_tbl.aec_outdoor_bright_region_reduction);
        } else { /* include in stats */
            temp_bias_weight = cam3a_aec_state.bias_table[i];
        }

        spotLuma = (temp_bias_weight * sumLumaArray[i]);
        weightSum = weightSum + temp_bias_weight;

        /* Perform the shift down by 8 before summing, this effectively
          converts the bias back to a Q0 number.  We lose some
          precision, but not much after all the shifting to follow */
        lumaSum += (spotLuma >> 8);

#endif /* CAMERA_USES_CHROMATIX_0x203 */
    }/* for (i=0 ...... */
} /* else .. */

cam3a_aec_state.outdoor_bright_region_discarded_cnt = discarded_regions;
weightSum = weightSum >> 8;
if (weightSum == 0) {
    MSG_FATAL ("weightSum is zero! %d",weightSum,0,0);
    weightSum = 1;
}
lumaSum = lumaSum / weightSum; 

/* Finally, divided the weighed average of all regions by the
  number of pixels per region */
lumaSum = lumaSum / (uint32)pixelsPerRegion;
MSG_CAM3ADEBUG ("lumaSum: %d, pixelsPerRegion: %d, weightSum: %d", lumaSum, pixelsPerRegion, weightSum);
MSG_CAM3ADEBUG ("cam3a_aec_state.outdoor_bright_region_discarded_cnt: %d", cam3a_aec_state.outdoor_bright_region_discarded_cnt, 0, 0);

if (lumaSum > 255) {
    MSG_CAM3ADEBUG ("cam3a Luma sum > expected %d. %d", lumaSum, numRegions, 0);
    lumaSum = 255;
}


  return((uint8) lumaSum);
} /* camera_aec_core_calculate_current_luma */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_RUN_ADJUST_EXPOSURE_ALGORITHM

DESCRIPTION
  core AEC algorithm

CALLERS:
  camera_aec_adjust_exposure

RETURN VALUE
  aec_output_type

===========================================================================*/
/*lint -save -e737 */
aec_output_type camera_aec_core_run_adjust_exposure_algorithm(uint8 current_vfe_luma)
{
  uint32 comp_ratio, temp_vfe_luma;
  aec_output_type output;
  uint16 digital_gained_luma;

  if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
  {
    /*lint -save -e524 -e732*/
    digital_gained_luma = current_vfe_luma*cam3a_aec_state.stored_digital_gain;
    /*lint -restore*/
    if(digital_gained_luma > 255)
      digital_gained_luma = 255;
    current_vfe_luma = (uint8) digital_gained_luma;
    cam3a_aec_state.current_luma = (uint8) digital_gained_luma;
  }

  current_vfe_luma  = camera_aec_core_apply_multi_luma_target(current_vfe_luma);

  /************************************************************************************* 
    Y value in the image is based on r/g/b gains from AWB.
    This means, even under the same brightness, different color temperature may generate 
    different brightness in the image.
    This code will compensate luma value to generate the same brightness.
  **************************************************************************************/
  if(camctrl_tbl.aec_luma_compensation_is_supported == TRUE)
  {
    /* comp_ratio in Q17 because coeffs in Q10 and awb prev_wb in Q7 */
    comp_ratio = cam3a_aec_state.luma_compensation_coefficient_r_gain*cam3a_awb_state.prev_wb.r_gain + 
      cam3a_aec_state.luma_compensation_coefficient_g_gain*cam3a_awb_state.prev_wb.g_gain + 
      cam3a_aec_state.luma_compensation_coefficient_b_gain*cam3a_awb_state.prev_wb.b_gain;

    /* temp_vfe_luma in Q17 with Q0 current_vfe_luma and Q17 comp_ratio */
    temp_vfe_luma = current_vfe_luma*comp_ratio; 

    /* temp_vfe_luma in Q0 by shifting 17 */
    temp_vfe_luma = temp_vfe_luma >> (CAMERA_AEC_Q_NUM+7);

    /* check boundary */
    if(temp_vfe_luma > 255)
      temp_vfe_luma = 255;

    /* update current_vfe_luma */
    current_vfe_luma = (uint8)temp_vfe_luma;
  }

  cam3a_aec_state.awb_compensated_luma = current_vfe_luma;

  if(camctrl_tbl.fast_convergence_is_supported)
    camera_aec_core_fast_convergence(current_vfe_luma);
  else
    camera_aec_core_slow_convergence(current_vfe_luma);

  if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
  {
    /* Q8 real gain is in exposure table */
    cam3a_aec_state.current_real_gain = (float)cam3a_aec_state.exposure_table_ptr[cam3a_aec_state.exp_index].gain / 256.0;
    output.gain = camsensor_real_to_register_gain(cam3a_aec_state.current_real_gain);
    output.digital_gain = (cam3a_aec_state.current_real_gain / camsensor_register_to_real_gain(output.gain));
  }
  else
  {
    /* register gain is in exposure table */
    output.gain = cam3a_aec_state.exposure_table_ptr[cam3a_aec_state.exp_index].gain;
    cam3a_aec_state.current_real_gain = camsensor_register_to_real_gain(output.gain);
    output.digital_gain = 1.0;
  }

  output.line_count = 
    cam3a_aec_state.exposure_table_ptr[cam3a_aec_state.exp_index].line_count;

  return output;
}
/*lint -restore */
/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_RUN_AUTO_FRAME_RATE_ALGORITHM

DESCRIPTION
  Implements AFR (Auto Frame Rate) generically from sensor params.

GLOBALS
  cam3a_aec_state - uses varibles in this struct

DEPENDENCIES
  on external variables:
  cam3a_aec_state - global structure holding the AEC state
  On: cam3a_aec_state.afr_max_fps and cam3a_aec_state.afr_min_fps
    1) Set in the CAMERA_PARM_PREVIEW_FPS parameter
    2) Must be no greater than the max sensor FPS (defined by sensor clock)
    3) Must be no less than the min sensor FPS (defined by sensor clock)
    4) Must be in Q8 format (i.e. 15 FPS = (15 * 256)
    5) If either are zero, this function does nothing
    6) If both are the same (non zero) then we adjust the current
      FPS down step by step to meet the new fixed rate.
    7) Other wise, we implmenent true "Auto" frame rate mode.
 "use_in_auto_frame_rate" - this is not supported yet from the driver

RETURN VALUE
  afr_output_type

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e734 -e818 */
afr_output_type camera_aec_core_run_auto_frame_rate_algorithm (
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  )
{
  afr_output_type output;
  float real_gain;
  float faster_fps_trigger;
  float slower_fps_trigger;
  uint16 rounded_fps;
  int32 settled_frames;
  uint16 temp_fps;
  int32 fps_indx;

  output.afr_status = AFR_SKIP_FRAME;
  output.fps_adjustment = 0;

  /* Keep frame rate here for now (settled count) */
  rounded_fps = camsensorParamsPtr->preview_fps >> 8;
  settled_frames = CAM3A_AFR_STABLILITY_SEC *  rounded_fps;

  if(cam3a_aec_state.frame_in_current_fps < settled_frames)
  {
    /* Return -do not modify frame rate until previous change
      has been in effect for CAM3A_AFR_STABLILITY_SEC seconds */
    return output;
  }

  /* If either max or min are zero, then exit.  Nothing to do */
  if((cam3a_aec_state.afr_max_fps == 0) ||
    (cam3a_aec_state.afr_min_fps == 0))
  {
    return output;
  }


  /* If FPS index for AFR doesn't match current preview FPS then
    search for correct index */
  if(camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index].fps !=
    camsensorParamsPtr->preview_fps)
  {
    /* Search for current FPS in AFR table */
    for(fps_indx = 0; fps_indx < camsensorParamsPtr->num_possible_frame_rates; 
      fps_indx++)
    {
      temp_fps = camsensorParamsPtr->frame_rate_array[fps_indx].fps;
      if(temp_fps == camsensorParamsPtr->preview_fps)
      {
        break;
      }
    }

    if(fps_indx == camsensorParamsPtr->num_possible_frame_rates)
    {
      MSG_HIGH ("Failed to adjust frame rate ",0,0,0);
      output.afr_status = AFR_FAILED;
      return output;
    }
    else
    {
      /* Init afr_index to where we found it for current FPS */
      cam3a_aec_state.afr_index = fps_indx;
    }
  }

  /* If max == min == current preview FPS then exit.  Nothing to do */
  if( (cam3a_aec_state.afr_max_fps == cam3a_aec_state.afr_min_fps) &&
    (cam3a_aec_state.afr_max_fps == camsensorParamsPtr->preview_fps) )
  {
    return output;
  }

  /* Cases:
    1) Max == min != current preview FPS -> in this case the user
      wants a fixed rate, raise or lower the FPS by one step until
      we achieve the desired rate (or as close as possible)
    2) Max != min; in this case a true "auto" frame rate is desired
      where we choose the rate based on the sensor's gain.
  */

  real_gain = cam3a_aec_state.current_real_gain;

  faster_fps_trigger = 
    camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index].faster_fps_gain_trigger + cam3a_aec_state.one_to_two_frame_hjr_offset;
  slower_fps_trigger = 
    camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index].slower_fps_gain_trigger + cam3a_aec_state.one_to_two_frame_hjr_offset;

  /* ************* Bounding FPS to within max and min requested values *** */
  /* Fixed rate, but current FPS does not match the requested rate */
  if( (cam3a_aec_state.afr_min_fps > camsensorParamsPtr->preview_fps) &&
    (cam3a_aec_state.afr_index > 0) )
  /* Zero being the highest rate, cannot go higher */
  {
    /* Proposed new FPS */
    temp_fps = 
      camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index-1].fps;
    /*  Only Adjust FPS up if the new FPS will be <= max requested FPS */
    if(temp_fps <= cam3a_aec_state.afr_max_fps)
    {
      output.fps_adjustment = 1; /* Adjust up to next faster FPS */
    }
  }
  else if( (cam3a_aec_state.afr_max_fps < camsensorParamsPtr->preview_fps) &&
    ((cam3a_aec_state.afr_index+1) < camsensorParamsPtr->num_possible_frame_rates) )
  {
    /*  Always adjust FPS down when the max is < the actual; even
      if it is less than the min FPS */
    output.fps_adjustment = -1; /* Adjust down to next slower FPS */
  }


  /* ******** Auto Adjust based on sensor gain level only if already with in the
          max & min bounds **** */
  /* If we are with in max and min && if the requested max doesn't == the
    min, then we perform true "auto" frame rate */
  if((output.fps_adjustment == 0) &&
    (cam3a_aec_state.afr_max_fps != cam3a_aec_state.afr_min_fps))
  {

    if( (real_gain > slower_fps_trigger) &&
      ((cam3a_aec_state.afr_index+1) < camsensorParamsPtr->num_possible_frame_rates))
    {
      /* Go to next (lower) frame rate */
      output.fps_adjustment = -1; /* Adjust down to next slower FPS */

      if(camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index+1].use_in_auto_frame_rate == FALSE)
        MSG_ERROR("ERROR, use_in_auto_frame_rate is not supported yet.",0,0,0);
    }

    if((real_gain < faster_fps_trigger) &&
      (cam3a_aec_state.afr_index > 0))
    /* Zero being the highest rate, cannot go higher */
    {
      output.fps_adjustment = 1; /* Adjust up to next faster FPS */

      if(camsensorParamsPtr->frame_rate_array[cam3a_aec_state.afr_index-1].use_in_auto_frame_rate == FALSE)
        MSG_ERROR("ERROR, use_in_auto_frame_rate is not supported yet.",0,0,0);
    }

  } /* end true "Auto" frame rate based on sensor gain level */
  output.afr_status = AFR_SUCCESS;

  return output;
}
/*lint -restore */
/*===========================================================================

FUNCTION      CAMERA_AEC_CORE_RUN_ADJUST_EXPOSURE_ALGORITHM_PER_ISO

DESCRIPTION
  core AEC algorithm

CALLERS:
  camera_aec_adjust_exposure

RETURN VALUE
  aec_output_type

===========================================================================*/
aec_output_type_per_iso camera_aec_core_run_adjust_exposure_algorithm_per_iso(
  camsensor_snapshot_config_type * snapshot_config_ptr,
  aec_forced_input_type forced_input)
{
  aec_output_type_per_iso output;
  float   preview_exposuretime, snapshot_exposuretime;
  uint16  snapshot_fps;
  boolean snapshot_fps_changed = FALSE;
  float   snapshot_real_gain;

  output.snapshot_gain = 0;
  output.snapshot_linecount = 0;
  output.apply_shapshot_fps = FALSE;
  output.snapshot_fps = 0;
  output.digital_gain_for_snapshot = 0;

  /*Update the snapshot_config variable*/
  snapshot_config_ptr->current_luma = cam3a_aec_state.current_luma;
  camera_aec_core_configure_snapshot_luma_target((void*)snapshot_config_ptr);

  if(cam3a_aec_state.iso_mode == CAMERA_ISO_AUTO)
  {
    snapshot_real_gain = cam3a_aec_state.current_real_gain;
    snapshot_config_ptr->auto_mode = TRUE;
  }
  else if(cam3a_aec_state.iso_mode == CAMERA_ISO_DEBLUR)
  {
    snapshot_config_ptr->auto_mode = TRUE;
    if(forced_input.apply_forced_input == TRUE)
    {
      snapshot_real_gain =  forced_input.forced_sensor_gain;
    }
    else
    {
      /* Even though HJR is enabled by UI, it has been disabled by
       * best shot or some other feature. So set the setting same 
       * as ISO_AUTO */
      snapshot_real_gain = cam3a_aec_state.current_real_gain;
    }
  }
  else
  {
    uint16 iso_multiplier = 1;
    float  iso_100_real_gain = ((float)camctrl_tbl.iso100_gain) / Q8;

    if(cam3a_aec_state.iso_mode == CAMERA_ISO_800)
      iso_multiplier = 8;
    else if(cam3a_aec_state.iso_mode == CAMERA_ISO_400)
      iso_multiplier = 4;
    else if(cam3a_aec_state.iso_mode == CAMERA_ISO_200)
      iso_multiplier = 2;
    else if(cam3a_aec_state.iso_mode == CAMERA_ISO_100)
      iso_multiplier = 1;

    snapshot_config_ptr->auto_mode = FALSE;
    /* To make sure we are using the correct/available sensor gain due to 
     * only discrete gains available from sensor.
     */
    snapshot_real_gain = iso_100_real_gain * iso_multiplier;

    if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
    {
      /* Need to use VFE's digital gain option */
      /*lint -save -e524 -e732*/
      output.snapshot_gain = snapshot_real_gain; /* real gains */
      /*lint -restore*/
    }
    else
    {
      output.snapshot_gain = camsensor_real_to_register_gain ( snapshot_real_gain );
      snapshot_real_gain = camsensor_register_to_real_gain(output.snapshot_gain);
    }
  }

  /* ISO ALGORITHM */
  {
    uint16 preview_linesPerFrame, snapshot_linesPerFrame;
    uint16 preview_fps = Q8;
    uint32 max_snapshot_linecount;

    preview_linesPerFrame  = camctrl_tbl.get_preview_lines_per_frame();
    snapshot_linesPerFrame = camctrl_tbl.get_snapshot_lines_per_frame();
    max_snapshot_linecount = camctrl_tbl.get_snapshot_max_exposure_line_count();

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  {
    preview_fps = camctrl_tbl.camsensor_params_ptr->preview_fps;
  }
/* Disable Auto Mode For ISO to prevent change in frame rate and gain based on the luma target */
#if defined FEATURE_CAMERA_BURST_MODE && defined FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_BURST_MODE  && FEATURE_CAMERA_AEC_DURING_MULTISHOT */

    /* Step 1 Calculate Preview ExposureTime */
    preview_exposuretime = (float)preview_fps/Q8;

    if(forced_input.apply_forced_input == TRUE)
    {
      preview_exposuretime = (forced_input.forced_line_count)/(preview_exposuretime * preview_linesPerFrame);
      /* Step 2 Calculate Snapshot Exposure Time */
      snapshot_exposuretime = (forced_input.forced_sensor_gain * preview_exposuretime)/snapshot_real_gain;
    }
    else
    {
      preview_exposuretime = (cam3a_aec_state.current_line_count)/(preview_exposuretime * preview_linesPerFrame);
      /* Step 2 Calculate Snapshot Exposure Time */
      snapshot_exposuretime = (cam3a_aec_state.current_real_gain * preview_exposuretime)/snapshot_real_gain;
    }

    /* Step 3 Calculate Snapshot Exposure Settings */
    output.apply_shapshot_fps = FALSE;
    snapshot_fps = camctrl_tbl.get_snapshot_frame_per_sec(preview_fps);
    output.snapshot_linecount = (uint32)(((snapshot_exposuretime * snapshot_linesPerFrame *  snapshot_fps)/Q8) + 0.5);
    if(output.snapshot_linecount == 0)
      output.snapshot_linecount = 1;

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */   
   {
    if(cam3a_aec_state.afr_enable == TRUE)
	  {
      uint16 preview_snapshot_fps_ratio, snapshot_fps_divider;

      preview_snapshot_fps_ratio = preview_fps/snapshot_fps;
      snapshot_fps_divider = 1;
      while(output.snapshot_linecount > max_snapshot_linecount)
      {
        snapshot_fps_divider = (uint16)(snapshot_fps_divider * 2);
        snapshot_fps_changed = TRUE;
        output.snapshot_linecount = (uint32)(((snapshot_exposuretime * snapshot_linesPerFrame *  snapshot_fps)/(Q8 * snapshot_fps_divider)) + 0.5);  
        if(output.snapshot_linecount == 0)
          output.snapshot_linecount = 1;
      }

      if(snapshot_fps_changed == TRUE)
      {
        uint16 new_fps_wrt_preview_fps;
        int i;

        new_fps_wrt_preview_fps = (uint16)((preview_snapshot_fps_ratio * snapshot_fps)/snapshot_fps_divider);

        i = camctrl_tbl.camsensor_params_ptr->num_possible_frame_rates - 1;
        if(new_fps_wrt_preview_fps >= camctrl_tbl.camsensor_params_ptr->frame_rate_array[i].fps)
        {
          for(i=0; i<camctrl_tbl.camsensor_params_ptr->num_possible_frame_rates; i++)
          {
            if(new_fps_wrt_preview_fps == camctrl_tbl.camsensor_params_ptr->frame_rate_array[i].fps)
            {
              /*lint -save -e525 */
              output.apply_shapshot_fps = TRUE;
              /*lint -restore */
              output.snapshot_fps = new_fps_wrt_preview_fps;
              break;
            }
          } /* for (i=0 ......*/
        }
        else
        {
          /* The algorithm needs lower FPS which is not supported so under expose */
          MSG_HIGH("ISO FPSneeded:%d MinFPSsupported:%d",
            new_fps_wrt_preview_fps,
            camctrl_tbl.camsensor_params_ptr->frame_rate_array[i].fps, 0);
          if(preview_fps != camctrl_tbl.camsensor_params_ptr->frame_rate_array[i].fps)
          {
            /*lint -save -e525 */
            output.apply_shapshot_fps = TRUE;
            /*lint -restore */
            output.snapshot_fps = camctrl_tbl.camsensor_params_ptr->frame_rate_array[i].fps;
          }

          /* Set the line count to max line count the Sensor supports.... */
          MSG_HIGH("ISO LineCountBefore:%d LineCountWillbe:%d",
            output.snapshot_linecount, max_snapshot_linecount, 0);
          output.snapshot_linecount = max_snapshot_linecount;
        }

      }/* if (snapshot_fps_changed == TRUE) */
    }
    else
    {
      if(output.snapshot_linecount > max_snapshot_linecount)
        output.snapshot_linecount = max_snapshot_linecount;
    }
  }
    if(output.snapshot_linecount == 0) /* linecount cannot be 0, if so, it should be 1 */
      output.snapshot_linecount = 1;

    /*lint -save -e539 */
    output.snapshot_gain = camsensor_real_to_register_gain(snapshot_real_gain);
    /*lint -restore */
    output.digital_gain_for_snapshot = 1.0;

    if(camctrl_tbl.aec_digital_gain_is_supported == TRUE)
    {
      output.digital_gain_for_snapshot
        = snapshot_real_gain / camsensor_register_to_real_gain(output.snapshot_gain);
    }
  }

  return output;
}


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
  )
{
  float compensation;

  if(camctrl_tbl.camsensor_params_ptr == NULL)
  {
    return;
  }

  compensation = (float)(log10(((double)camctrl_tbl.camsensor_params_ptr->max_preview_fps/fps)));
  compensation = compensation * cam3a_aec_state.exposure_index_adj_step;

  /* compensate indoor/outdoor index */
  cam3a_aec_state.indoor_index = (uint16)((int32)cam3a_aec_state.indoor_index_init - (int32)compensation);
  cam3a_aec_state.outdoor_index = (uint16)((int32)cam3a_aec_state.outdoor_index_init - (int32)compensation);
} /* camera_update_indoor_outdoor_indices_per_fps */
/*===========================================================================

FUNCTION    CAMERA_AEC_CORE_UPDATE_INDOOR_OUTDOOR_INDICES

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
void camera_aec_core_update_indoor_outdoor_indices 
  (
  /* Index into the supported frame rates table */
  int16 index
  )
{
  float compensation;

  if(camctrl_tbl.camsensor_params_ptr == NULL)
  {
    return;
  }

  compensation = (float)(log10(((double)camctrl_tbl.camsensor_params_ptr->max_preview_fps/
                              camctrl_tbl.camsensor_params_ptr->frame_rate_array[index].fps)));
  compensation = compensation * cam3a_aec_state.exposure_index_adj_step;

  /* compensate indoor/outdoor index */
  cam3a_aec_state.indoor_index = (uint16)((int32)cam3a_aec_state.indoor_index_init - (int32)compensation);
  cam3a_aec_state.outdoor_index = (uint16)((int32)cam3a_aec_state.outdoor_index_init - (int32)compensation);
} /* camera_update_indoor_outdoor_indices */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
#endif //#ifndef CAMERA_USES_SOFTDSP


