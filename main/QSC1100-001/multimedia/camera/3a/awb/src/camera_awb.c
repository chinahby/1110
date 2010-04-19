/*=============================================================================
FILE:       camera_awb.c

SERVICES:   Auto White Balance File

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/awb/main/latest/src/camera_awb.c#6 $ $DateTime: 2009/04/22 23:29:28 $ $Author: c_caluru $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 04/22/09   cg       Apply Global Color  Correction Bias to snapshot.
 04/14/09   cg       Changed the mannual flash to work without Bestshot Mode   
 04/14/09   cg       Added suppoty for BS_AWB_INDOOR required for partymode bestshot
 06/17/08   ww       set cam3a_awb_state.agw_sample_decision to TL84 as default for sensor rolloff
 07/05/07   dle      Fixed this: sourceCC may be used before checking for NULL.
                     Or it was not checked for NULL.
 05/09/07   klee     Enhanced color correction update logic by checking exposure index changes
 05/09/07   klee     Enhanced AWB preview with color correction bias interpolation
 04/05/07   klee     Modified awb low-light detection logic - not using FPS
 03/29/09   klee     Updated camera_aec_retrive_exposure_info() call
 03/28/07   klee     Modified the name awb_input_exposure_type to be camera_exposure_info_type
 03/15/07   klee     Fixed minor lint error
 03/13/07   klee     Moved in the code on special effect from camera_svcs
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/20/07   klee     Fixed all lint errors on AWB
 02/07/07   klee     Fixed white/grey world bounding box adjustment with BSM
 01/24/07   klee     Enahnced AWB algorithm with white_has_single_peak
 01/09/07   klee     Enhanced to support AEC with digital gains
 01/02/07   klee     Enhanced AWB to provide stable preview by history
 12/22/06   klee     Enhanced low-light color correction applying proper bounding box
 12/21/06   klee     Updated AWB to apply gain_adj only for the snapshot, not for preview
 12/01/06   klee     Removed the code under CAMERA_AWB_FLOAT_ON
 11/27/06   klee     Enhanced low-light color correction applying
 11/20/06   jv       Removed static saturated color conversion matrix. Now 
                     calculated from AWB chosen color conversion matrix.
 11/13/06   klee     Removed compiling warning on update_color_correction
 11/12/06   klee     Enhanced awb wb gains convergence to get to desired values
 11/11/06   klee     Updated AWB aggressiveness and low-light CC support to be tunable
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Enhance BSM initializing
 10/20/06   jv       Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/17/06   klee     Enhance AWB to update VFE with stored_prev_r/g/b_gain in case frame skip
 09/27/06   klee     Applied FEATURE_CAMERA_3A_USE_SGW_ALGO on SGW algorithm
 09/22/06   klee     Fix the bug on float->Q7 converting
 09/15/06   klee     Cleaned up the lint errors
 09/14/06   klee     Updated Beach/snow BSM support with new BS configuration
 09/08/06   klee     Updated camera_awb_implement() to use previous exposure_index from AEC
 09/08/06   klee     Added camera_awb_prepare_snapshot() for AWB modularization
 09/01/06   kadhir   Removed unwanted Cross Compilation Dependency 
                     When qcamcorder Feature Disabled
 08/31/06   klee     Enhance outdoor BSM support by D65 wb initializing
 08/31/06   klee     Bug fix on previous and current wb maintaining
 08/30/06   klee     Beach/snow BSM support with additional blue gain adjustment
 08/30/06   klee     Removed orange/green emphasized CV matrix support
 08/23/06   klee     Fixed the bug on white vs grey world bounding box toggling
 08/18/06   klee     Fixed critical and high lint errors
 08/18/06   klee     Fix the bug on AWB gain interpolation - update_color_correction
 08/10/06   klee     Cleaned up the code regarding tuning variables relocation 
                     - AWB/AEC
                     - Color matrix
 08/01/06   klee     AWB with white world algorithm
 07/25/06   jv       Added color correction matrix as argument to 
                     camera_awb_apply_new_wb_gains 
 07/18/06   dle      Make VFE_4X4_SUPPRT a default.
                     Featurize for VFE_16X16_SUPPORT.
 07/16/06   klee     Added bestshot support - AWB, color correction/conversion
 07/12/06   dle      Re-do profiling.
 07/05/06   klee     Updated to refer color matrix in VFE
 05/25/06   klee     Enhanced AGW AWB altorithm - green_line_b
 05/16/06   klee     Enhanced AGW AWB algorithm - rejectint, WV condition etc
 05/15/06   klee     Fixed critical lint error - possibly devided by 0
 05/02/06   klee     Made init and do AGW algorithm to be external API
                     camctrl needs to access to map to camctrl API
                     Updated config and implement function to use
                     camctrl AGW algorithm without making any decision.
 04/12/06   klee     Enhanced AGW AWB implementation.
                     Kept old AGW AWB implementation as AGW_AWB_WV_only.
                     Depending on driver support, one of them is applied.
 03/21/06   klee     Re-arranged functions of AWB algorithms
                     - simple grey world
                       - camera_awb_init_awb_simple_grey_world_algo()
                       - camera_awb_do_awb_simple_grey_world_algo()
                     - advanced grey world
                       - camera_awb_init_awb_advanced_grey_world_algo()
                       - camera_awb_do_awb_advanced_grey_world_algo()
                       if CAMERA_AWB_FLOAT_ON
                       - camera_awb_init_awb_advanced_grey_world_algo_float()
                       - camera_awb_do_awb_advanced_grey_world_algo_float()
                     Removed 
                       - camera_awb_get_current_option()
                       - camera_awb_change_current_option()
 03/03/06   klee     Move weight_vector into camsensor driver layer
                     - to make weight vector sensor-dependable
 02/24/06   ven      Added support for profiling awb and aec
 01/20/05   dl       Corrtected the low light CC matrix interpolation.
 12/14/05   ven      Added support for VFE 4X4 Statistics collection.
 12/01/05   klee     fix bugs on WV AWB algorithm and set it to be deault AWB algorithm
                     clean-up the code, fix lint error for 4.3 release
                     lint error 818 has been simply surpressed for all over the file.
 11/17/05   klee     set legacy AWB to be default and keep weighted AWB implementation code as secondaly
                     weighted AWB is not fully stable yet. Keep watching
                     little modification on legacy AWB - no new features.
 11/11/05   klee     merge new weighted AGW AWB algorithm and clean up the lint errors
 10/17/05   klee     clean up the lint error except
                     e653 on camera_awb_update_color_correction_or_wb()
                     e701, e704 camera_awb_update_color_conversion_for_wb()
                     e550 camera_awb_do_awb_simple_grey_world_algo()
                     e641, e550 camera_awb_do_awb_advanced_grey_world_algo()
 10/14/05   dle      Renamed VFE1X to VFE1P.
 10/04/05   klee     Initial code transfered from cam3a 

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#ifndef FEATURE_CAMERA_YCBCR_ONLY
#include <stdlib.h>             /* Dynamic memory allocation               */
#include "camera_3a_shared.h"
#include "camera_aec.h"
#include "camera_awb.h"
#ifdef FEATURE_CAMERA_HJR
#include "camera_hjr.h"
#endif /* FEATURE_CAMERA_HJR */
#include "camctrl.h"
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */

/*==============================================================================
  INTERNAL VARIABLES
==============================================================================*/
cam3a_awb_state_struct cam3a_awb_state;

#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
cam3a_simple_grey_world_type  * cam3a_sgw_ptr = NULL;
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
awb_input_stats_type          * awb_input_stats_ptr = NULL;
camera_exposure_info_type     * awb_input_exposure_ptr = NULL;


#ifdef FEATURE_CAMERA_3A_PROFILING
static cam3a_timing_type cam3a_awb_timing;
#endif /* FEATURE_CAMERA_3A_PROFILING */

static int32 agw_stats_convert_coeff_A11; 
static int32 agw_stats_convert_coeff_A12;
static int32 agw_stats_convert_coeff_A13;
static int32 agw_stats_convert_coeff_A21;
static int32 agw_stats_convert_coeff_A22;
static int32 agw_stats_convert_coeff_A23;
static int32 agw_stats_convert_coeff_A31;
static int32 agw_stats_convert_coeff_A32;
static int32 agw_stats_convert_coeff_A33;

#ifdef FEATURE_CAMERA_HJR
extern HJR_struct HJR_structure;
#endif /* FEATURE_CAMERA_HJR */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */
extern camsensor_static_params_type  camsensor_static_params [];
extern uint8 camera_asi;
/*==============================================================================
  INTERNAL API DECLARATIONS
==============================================================================*/
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
/*===========================================================================

FUNCTION    CAMERA_AWB_DO_AWB_SIMPLE_GREY_WORLD_ALGO

DESCRIPTION
  Implements Simple Grey World algorithm for Auto White Balance.

GLOBALS

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_awb_do_awb_simple_grey_world_algo (
  int32 numRegions,
/* Num regions in input sum of luma array  */
  VfeStatOutStruct * statStruct,
/* Output of VFE for AWB calcs          */
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  );
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

/*===========================================================================

FUNCTION    CAMERA_AWB_DO_AWB_ADVANCED_GREY_WORLD_ALGO

DESCRIPTION
  This function uses a weight vector to weight different light 
  sources depending on indoor, outdoor or inout door.
  Implements Advanced Grey World algorithm for Auto White Balance.
  An AWB algorithm based on grid distance and outlier removal.
  Output is just a selection of the lighting conditions pre-determined gains
  for R and G

  The shortest distance on the grid to the lighting cond. gets selected

  In addition, this algorithm has cluster concept to cover daylight, fluorescent
  and incandescent light conditions. It also involves green line concept.
  Call cam3a_init_awb_advanced_grey_world() before calling
  cam3a_do_awb_advanced_grey_world_algo()

  In this function, fixed point number is being used

  This AWB algo is availble only to the drivers which doesn't have
  latest tuning parameters.

GLOBALS
  cam3a_awb_state - uses varibles in this struct
  cam3a_agw_ptr - Variables, structs and scratch memory here

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static int camera_awb_do_awb_advanced_grey_world_algo(
  int32 numRegions,
/* Num regions in input sum of luma array  */
  VfeStatOutStruct * statStruct,
/* Output of VFE for AWB calcs          */
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  );

#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
/*===========================================================================

FUNCTION    CAMERA_AWB_INIT_AWB_SIMPLE_GREY_WORLD_ALGO

DESCRIPTION
  Initialization for Simple Grey World Auto White Balance algorithm.
  This function initializes the grids for AWB R/G and B/G bins
  Note that the SGW grid is different from AGW

GLOBALS
  cam3a_sgw_ptr - inits varibles in this struct

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  Changes global structures.

===========================================================================*/
static void camera_awb_init_awb_simple_grey_world_algo(void);
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

/*===========================================================================

FUNCTION      CAMERA_AWB_UPDATE_COLOR_CORRECTION_OR_WB

DESCRIPTION:
  Updates new red and blue gains to the VFE update color correction
  matrix based on past history, and aggressiveness settings.

CALLERS:

GLOBALS:
  Modified the following for history:
    cam3a_awb_state

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:

USAGE:

SCOPE:
 Local

===========================================================================*/
static void camera_awb_update_color_correction_or_wb(
  float redGain,
  float greenGain,
  float blueGain,
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  );

/*===========================================================================

FUNCTION    CAMERA_AWB_UPDATE_COLOR_CONVERSION_FOR_WB

DESCRIPTION
  Updates new red and blue gains to the VFE update color conversion
  matrix.  Takes into account past history & aggressiveness settings.

GLOBALS
  Uses the following:
    cam3a_awb_state

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
static void camera_awb_update_color_conversion_for_wb(
  cam3a_auto_white_balance_illuminant_type illuminantChoice, 
/* Illuminant choice */
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  );

/*===========================================================================

FUNCTION      CAMERA_AWB_DELIMIT_WB_GAINS

DESCRIPTION:
  Enforce user-configurable min and max WB gains.
  These gains are floats in Q0 format.

CALLERS:
  camera_awb_apply_new_wb_gains ()

GLOBALS:
  cam3a_awb_state - For min and max thresholds.

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
 None

USAGE:

SCOPE:
 Local

=========================================================================== */
static void camera_awb_delimit_wb_gains (
  camera_1x3_matrix_type *wb
  );

/*===========================================================================

FUNCTION      CAMERA_AWB_UPDATE_PREV_WB_GAINS

DESCRIPTION:
  Updates previous whitebalance gains.

CALLERS:
  camera_awb_apply_new_wb_gains ()

DEPENDENCIES:

SIDE EFFECTS:
  Update the global cam3a_awb_state

RETURN VALUE:
 None

USAGE:

SCOPE:
 Local

=========================================================================== */
static void camera_awb_update_prev_wb_gains (
  camera_1x3_matrix_type wb 
  );

/*===========================================================================

FUNCTION      CAMERA_AWB_WB_GAINS_APPLY_BIAS

DESCRIPTION:
  Compensates white balance gains for darker exposure ctrl settings.

CALLERS:
  camera_awb_apply_new_wb_gains ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
 None

USAGE:

SCOPE:
 Local

============================================================================*/
static void camera_awb_wb_gains_apply_bias (
  camera_1x3_matrix_type *wb
  );

/*==============================================================================
  INTERNAL DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AWB_UPDATE_COLOR_CORRECTION_OR_WB

DESCRIPTION:
  Updates new red and blue gains to the VFE update color correction
  matrix based on past history, and aggressiveness settings.

CALLERS:

GLOBALS:
  Modified the following for history:
    cam3a_awb_state

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:

USAGE:

SCOPE:
 Local

===========================================================================*/
/*lint -save -e818*/
static void camera_awb_update_color_correction_or_wb(
  float redGain,
  float greenGain,
  float blueGain,
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  )
{
  camera_1x3_matrix_type weighted_wb;
  int16 blackOffset;
  camera_3x3_rgb_matrix_type * sourceCC;
  camera_3x3_rgb_matrix_type low_light_CC;
  float wb_weight;
  float one_minus_wb_weight;
  camera_1x3_matrix_type temp_wb;
  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  if(camsensorParamsPtr == NULL) return;

  blackOffset = -(cam3a_awb_state.black_level);

  weighted_wb.q_factor = 1;
  weighted_wb.r_gain = vfe_convert_float_to_Qfactor(redGain, 7);
  weighted_wb.g_gain = vfe_convert_float_to_Qfactor(greenGain, 7);
  weighted_wb.b_gain = vfe_convert_float_to_Qfactor(blueGain, 7);

  /* Intermediate output */
  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #else
  sourceCC = vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL);
  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  if(sourceCC == NULL)
  {
    MSG_ERROR ("color correction matrix is not ready in VFE", 0, 0, 0);
    return;
  }

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #else
  low_light_CC = *(vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_LOW_LIGHT));
  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */
  {
  camera_aec_update_color_correction_matrix(sourceCC, &low_light_CC);
  }

  /* to support snapshot */
  cam3a_awb_state.hybrid_CC.A11 = low_light_CC.A11;
  cam3a_awb_state.hybrid_CC.A12 = low_light_CC.A12;
  cam3a_awb_state.hybrid_CC.A13 = low_light_CC.A13;

  cam3a_awb_state.hybrid_CC.A21 = low_light_CC.A21;
  cam3a_awb_state.hybrid_CC.A22 = low_light_CC.A22;
  cam3a_awb_state.hybrid_CC.A23 = low_light_CC.A23;

  cam3a_awb_state.hybrid_CC.A31 = low_light_CC.A31;
  cam3a_awb_state.hybrid_CC.A32 = low_light_CC.A32;
  cam3a_awb_state.hybrid_CC.A33 = low_light_CC.A33;

  cam3a_awb_state.hybrid_CC.bias_r = low_light_CC.bias_r;
  cam3a_awb_state.hybrid_CC.bias_g = low_light_CC.bias_g;
  cam3a_awb_state.hybrid_CC.bias_b = low_light_CC.bias_b;

  /* Enforce user-configurable min and max WB gains.
  * These gains are int in Q7 format.
  */
  camera_awb_delimit_wb_gains (&weighted_wb);

  cam3a_awb_state.prev_weighted_wb.r_gain = cam3a_awb_state.prev_wb.r_gain;
  cam3a_awb_state.prev_weighted_wb.g_gain = cam3a_awb_state.prev_wb.g_gain;
  cam3a_awb_state.prev_weighted_wb.b_gain = cam3a_awb_state.prev_wb.b_gain;

  cam3a_awb_state.curr_weighted_wb.r_gain = weighted_wb.r_gain;
  cam3a_awb_state.curr_weighted_wb.g_gain = weighted_wb.g_gain;
  cam3a_awb_state.curr_weighted_wb.b_gain = weighted_wb.b_gain;

  /* If either param changed, then update VFE */
  if(
    ( (weighted_wb.r_gain - cam3a_awb_state.prev_wb.r_gain) != 0 ) ||
    ( (weighted_wb.g_gain - cam3a_awb_state.prev_wb.g_gain) != 0 ) ||
    ( (weighted_wb.b_gain - cam3a_awb_state.prev_wb.b_gain) != 0 ) ||
    ( cam3a_aec_state.exp_index != cam3a_aec_state.prev_exp_index )
    )
  {
    /* Use 1/8 of the new WB gain to change the old */
    if(cam3a_awb_state.awb_aggressiveness == CAM3A_LOW_AGGRESSIVENESS)
    {
      wb_weight = cam3a_awb_state.awb_low_aggressiveness;
    }
    else if(cam3a_awb_state.awb_aggressiveness == CAM3A_MED_AGGRESSIVENESS)
    {
      wb_weight = cam3a_awb_state.awb_med_aggressiveness;
    }
    /* By implication, must be high */
    else /* if (cam3a_awb_state.awb_aggressiveness == CAM3A_HIGH_AGGRESSIVENESS) */
    {
      wb_weight = cam3a_awb_state.awb_high_aggressiveness;
    }
    one_minus_wb_weight = 1.0 - wb_weight;

    temp_wb.q_factor = 1;
    temp_wb.r_gain  = 
      (int32)((wb_weight*(float)weighted_wb.r_gain) + (one_minus_wb_weight*(float)cam3a_awb_state.prev_wb.r_gain));
    temp_wb.g_gain  = 
      (int32)((wb_weight*(float)weighted_wb.g_gain) + (one_minus_wb_weight*(float)cam3a_awb_state.prev_wb.g_gain));
    temp_wb.b_gain  = 
      (int32)((wb_weight*(float)weighted_wb.b_gain) + (one_minus_wb_weight*(float)cam3a_awb_state.prev_wb.b_gain));

    /* small adjustment may be < 1 step, need to move it */
    if(temp_wb.r_gain == cam3a_awb_state.prev_wb.r_gain)
    {
      if(weighted_wb.r_gain > temp_wb.r_gain)
        temp_wb.r_gain++;
      else if(weighted_wb.r_gain < temp_wb.r_gain)
        temp_wb.r_gain--;
    }
    if(temp_wb.g_gain == cam3a_awb_state.prev_wb.g_gain)
    {
      if(weighted_wb.g_gain > temp_wb.g_gain)
        temp_wb.g_gain++;
      else if(weighted_wb.g_gain < temp_wb.g_gain)
        temp_wb.g_gain--;
    }
    if(temp_wb.b_gain == cam3a_awb_state.prev_wb.b_gain)
    {
      if(weighted_wb.b_gain > temp_wb.b_gain)
        temp_wb.b_gain++;
      else if(weighted_wb.b_gain < temp_wb.b_gain)
        temp_wb.b_gain--;
    }

    /* set weighted_wb as a current WB gains after adjustment */
    weighted_wb = temp_wb;

    /* Set current to previous for next time.
      * Do this BEFORE applying the color correction bias!!
      */
    camera_awb_update_prev_wb_gains (weighted_wb);

    /* Now apply bias to all gains to make up for intentionally
      * darker exposure ctrl setting
      */
    camera_awb_wb_gains_apply_bias (&weighted_wb);

    /* APPLY again to try and correct cropping errors.
      * Enforce user-configurable min and max WB gains.
      * These gains are floats in Q0 format.
      */
    camera_awb_delimit_wb_gains (&weighted_wb);

    /* Tell Camera Layer to update VFE if WB has changed */
    camera_update_vfe = 1;

    /* Apply white balance and color correction */
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */
    {
     vfe_update_with_new_gain_to_color_correction_or_wb (
      weighted_wb,
      blackOffset, 
      &low_light_CC);
    }
 
  }
} /* cam3a_update_color_correction_or_wb */
/*lint restore*/

/*===========================================================================

FUNCTION    CAMERA_AWB_UPDATE_COLOR_CONVERSION_FOR_WB

DESCRIPTION
  Updates new red and blue gains to the VFE update color conversion
  matrix.  Takes into account past history & aggressiveness settings.

GLOBALS
  Uses the following:
    cam3a_awb_state

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e818*/
static void camera_awb_update_color_conversion_for_wb(
  cam3a_auto_white_balance_illuminant_type illuminantChoice, /* Illuminant choice */
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  )
{
  /* Intermediate output */
  camera_3x3_ycbcr_matrix_type * newCConversion = NULL;
  float aggressiveness;
  float saturation = CAMERA_SATURATION_DEFAULT;

  if(camsensorParamsPtr == NULL) return;
  /* Cannot touch/update the Color Convert Array if we are in special effects
    mode */
  if(cam3a_awb_state.color_conversion_state == SPECIAL_EFFECTS_COLOR_CONVERSION)
  {
    return;
  }

  /* Use daylight color conversion array */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
  {
    switch(illuminantChoice)
    {
    /* Use daylight color conversion array */
      case CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT:
      case CAM3A_AGW_AWB_DAYLIGHT_HYBRID:
      case CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1:
      case CAM3A_AGW_AWB_OUTDOOR_CLOUDY:
        newCConversion = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65);
        break;

    /* Use flourescent color conversion array */
      case CAM3A_AGW_AWB_INDOOR_WARM_FLO:
      case CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO:
      case CAM3A_AGW_AWB_INDOOR_COLD_FLO:
        newCConversion = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84);
        break;

    /* Use incandescent color conversion array */
      case CAM3A_AGW_AWB_INDOOR_INCANDESCENT:
      case CAM3A_AGW_AWB_HORIZON:
        newCConversion = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A);
        break;

      default:
        return;
    }
  }

  if(newCConversion == NULL)
  {
    MSG_ERROR("color conversion matrix is not available in VFE",0,0,0);
    return;
  }

  if(cam3a_awb_state.awb_aggressiveness == CAM3A_LOW_AGGRESSIVENESS)
  {
    aggressiveness = cam3a_awb_state.awb_low_aggressiveness;
  }
  else if(cam3a_awb_state.awb_aggressiveness == CAM3A_MED_AGGRESSIVENESS)
  {
    aggressiveness = cam3a_awb_state.awb_med_aggressiveness;
  }
  else /* if (cam3a_awb_state.awb_aggressiveness == CAM3A_HIGH_AGGRESSIVENESS) */
  {
    aggressiveness = cam3a_awb_state.awb_high_aggressiveness;
  }

  /* there are two locations that modify CV, we should always do the change per AWB first, then modify it per AEC results. 
     This way, lowlight CV will overwrite normal light CV under lowlight condition. */
  /* Suggested by Ying.Becasue for example if low light is selected by AEC and if temporal filter is not applied then large variations in
     will be visible */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */

  /* Apply temporal filter */ 
  vfe_update_color_conversion_for_wb(newCConversion, aggressiveness, saturation);

  /* Tell Camera Layer to update VFE if WB has changed */
  camera_update_vfe = 1;

  /* Leave bias values alone for now */
} /* cam3a_update_color_conversion_for_wb */
/*lint -restore*/
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
/*===========================================================================

FUNCTION    CAMERA_AWB_INIT_AWB_SIMPLE_GREY_WORLD_ALGO

DESCRIPTION
  Initialization for Simple Grey World Auto White Balance algorithm.
  This function initializes the grids for AWB R/G and B/G bins
  Note that the SGW grid is different from AGW

GLOBALS
  cam3a_sgw_ptr - inits varibles in this struct

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  Changes global structures.

===========================================================================*/
static void camera_awb_init_awb_simple_grey_world_algo(void)
{
  int index;
  int n;
  float delta;
  /* Fix the min and max ratios for SGW, since we have no reference
    points */
  float awb_min_ratio=0.25, awb_max_ratio=4.0;


  if(cam3a_sgw_ptr == NULL)
  {
    MSG_FATAL ("Camera: cam3a_sgw_ptr is not allocated", 0, 0, 0);
    return;
  }

  n=(CAM3A_SGW_AWB_NUMBER_GRID_POINT-1)/2;

  if(awb_max_ratio<(1.0/awb_min_ratio))
    awb_max_ratio=1.0/awb_min_ratio;
  else if(awb_min_ratio>(1.0/awb_max_ratio))
    awb_min_ratio=1.0/awb_max_ratio;


  // for region < 1
  //delta=(1.0-awb_min_ratio)/((float)n+0.5);
  delta=(1.0-awb_min_ratio)/(float)(n+0.5);
  cam3a_sgw_ptr->rgbg_grid[0]=awb_min_ratio;
  for(index=1; index<=n; index++)
  {
    cam3a_sgw_ptr->rgbg_grid[index]=cam3a_sgw_ptr->rgbg_grid[index-1]+delta;
  }

  // for region > 1
  /*lint -save -e790 Suspicious truncation, integral to float */
  //delta=2.0*(awb_max_ratio-1.0)/((float) (n*(n+1)));
  //delta = (float)( 2.0*(float)( (awb_max_ratio-1.0)/n*(n+1) ) ) ;
  delta = (float)( 2.0*( (float)(awb_max_ratio-1.0)/(float)(n*(n+1)) ) ) ;
  /*lint -restore */
  cam3a_sgw_ptr->rgbg_grid[n+1]=1.0+delta;
  for(index=n+2; index<CAM3A_SGW_AWB_NUMBER_GRID_POINT; index++)
  {
    cam3a_sgw_ptr->rgbg_grid[index] =
      cam3a_sgw_ptr->rgbg_grid[index-1]+delta*(float) ((index-n)-1);
  }
  cam3a_sgw_ptr->rgbg_grid[CAM3A_SGW_AWB_NUMBER_GRID_POINT]=awb_max_ratio;

}  /* cam3a_init_awb_simple_grey_world */
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
/*===========================================================================

FUNCTION    CAMERA_AWB_DO_AWB_SIMPLE_GREY_WORLD_ALGO

DESCRIPTION
  Implements Simple Grey World algorithm for Auto White Balance.

GLOBALS

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e550 -e818 numNonZeroRegions not accessed */
static void camera_awb_do_awb_simple_grey_world_algo (
  int32 numRegions,
/* Num regions in input sum of luma array  */
  VfeStatOutStruct * statStruct,
/* Output of VFE for AWB calcs          */
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer        */
  )
{
  int32 regionIndex, i, j;
  int32 numNonZeroRegions;
  int32 numGridPoints;
  float cb_avg, cr_avg, y1_avg;
  float numPixelsSumedInThisRegion;
  float redGain, blueGain, greenGain;
  float rg_ratio, bg_ratio;
  float r,g,b;
  double sumRed, sumGreen, sumBlue;
  int x_grid, y_grid;
#ifdef FEATURE_CAMERA_3A_PROFILING
  int start_time, end_time, processing_time;
#endif /* FEATURE_CAMERA_3A_PROFILING */


  if(camsensorParamsPtr == NULL) return;

  MSG_CAM3ADEBUG ("cam3a_do_awb_simple_grey_world_algo",0,0,0);

  if(cam3a_sgw_ptr == NULL)
  {
    MSG_FATAL ("Camera: cam3a_sgw_ptr is not allocated", 0, 0, 0);
    return;
  }
#ifdef FEATURE_CAMERA_3A_PROFILING
  start_time = timetick_get_ms ();
#endif /* FEATURE_CAMERA_3A_PROFILING */


  /* Clear out sample grid before calculation */
  for(i = 0; i < SAMPLE_GRID_POINTS; i++)
  {
    for(j = 0; j < SAMPLE_GRID_POINTS; j++)
    {
      cam3a_sgw_ptr->sample_grid[i][j] = 0;
      cam3a_sgw_ptr->sample_grid[i][j] = 0;

    }
  }

  sumRed = 0.0;
  sumGreen = 0.0;
  sumBlue = 0.0;
  numGridPoints = 0;
  numNonZeroRegions = 0;

  for(regionIndex = 0; regionIndex < numRegions; regionIndex++)
  {
    /* Must divide each sum by the num pixels used to produce it
    on a per region basis */
    numPixelsSumedInThisRegion = (float)statStruct->NSCb[regionIndex];

    /* Avoid divide by zero issues */
    if(numPixelsSumedInThisRegion == 0)
    {
      r = g = b = 0.0;
    }
    else
    {
      /* Record number of non-zero regions */
      numNonZeroRegions++;

      /* else compute average component value for each region */
      y1_avg = (statStruct->SY1[regionIndex] / numPixelsSumedInThisRegion);
      cb_avg = (statStruct->SCb[regionIndex] / numPixelsSumedInThisRegion)-128.0;
      cr_avg = (statStruct->SCr[regionIndex] / numPixelsSumedInThisRegion)-128.0;

      /* Now translate to RGB domain */
      /* straight from shung code   */
      /* Black offset in 10 bit, stats in 8 bit.  Divide black level by 4. */
      r = (float)((((1.0*y1_avg) - (0.228*cb_avg)) + (1.402*cr_avg)) -
        camera_3a_shared_update_black_level(cam3a_awb_state.black_level));
      g = (float)((((1.0*y1_avg) - (0.228*cb_avg)) - (0.598*cr_avg)) -
        camera_3a_shared_update_black_level(cam3a_awb_state.black_level));
      b = (float)((((1.0*y1_avg) + (1.772*cb_avg)) - (0.598*cr_avg)) -
        camera_3a_shared_update_black_level(cam3a_awb_state.black_level));

      if(g > 0)
      {
        rg_ratio= r/g;
        bg_ratio= b/g;

        x_grid = y_grid = CAM3A_SGW_AWB_NUMBER_GRID_POINT;
        // compute the grid coordinate on the 2D plot
        for(i=0; i < CAM3A_SGW_AWB_NUMBER_GRID_POINT; i++)
        {  // find the R/G grid index
          if((rg_ratio>=cam3a_sgw_ptr->rgbg_grid[i]) && 
            (rg_ratio<cam3a_sgw_ptr->rgbg_grid[i+1]))
          {
            x_grid=i;
            break;
          }
        }
        // compute the grid coordinate on the 2D plot
        for(i=0; i < CAM3A_SGW_AWB_NUMBER_GRID_POINT; i++)
        {  // find the B/G grid index
          if((bg_ratio>=cam3a_sgw_ptr->rgbg_grid[i]) && 
            (bg_ratio<cam3a_sgw_ptr->rgbg_grid[i+1]))
          {
            y_grid=i;
            break;
          }
        }

        /* Ensure 2D point is valid */
        if((x_grid < CAM3A_SGW_AWB_NUMBER_GRID_POINT) &&
          (y_grid < CAM3A_SGW_AWB_NUMBER_GRID_POINT))
        {
          /* Only sum if we have hit this point less than
            SGW_MAX_NUM_POINTS_ON_SINGLE_GRID_LOCATION number of times */
          if((cam3a_sgw_ptr->sample_grid[y_grid][x_grid]) <
            SGW_MAX_NUM_POINTS_ON_SINGLE_GRID_LOCATION )
          {
            numGridPoints++;
            /* Sum output for each component */
            sumRed  += r;
            sumGreen += g;
            sumBlue  += b;
          }

          cam3a_sgw_ptr->sample_grid[y_grid][x_grid]++;
        }
      } /* END if (g) */
    } /* END else clause of if (numPixelsSumedInThisRegion == 0) */
  } /* END  for (regionIndex = 0; regionIndex < numRegions; regionIndex++) */

  /* If we got at least 1 region with valid statistics .. */
  if(numGridPoints > 0)
  {
    /* Now choose the largest & make it the numerator for
      the divide (the normalization factor so that the
      other two will be > 1.0). Green will usually
      win here. */
    if((sumGreen >= sumRed) && (sumGreen >= sumBlue))
    {
      /* Green is the numerator, normalize to this */
      greenGain = 1.0;

      /* To avoid a divide by zero, check for being > zero */
      if(sumRed > 0.0)
        redGain = (float)(sumGreen / sumRed);
      else
        redGain = (float)sumGreen;

      if(sumBlue > 0.0)
        blueGain = (float)(sumGreen / sumBlue);
      else
        blueGain = (float)sumGreen;

    }
    else if((sumBlue >= sumRed) && (sumBlue >= sumGreen))
    {
      /* Blue is the numerator, normalize to this */
      blueGain = 1.0;

      /* To avoid a divide by zero, check for being > zero */
      if(sumRed > 0.0)
        redGain = (float)(sumBlue / sumRed);
      else
        redGain = (float)sumBlue;

      if(sumGreen > 0.0)
        greenGain = (float)(sumBlue / sumGreen);
      else
        greenGain = (float)sumBlue;
    }
    else
    {
      /* Red is the numerator, normalize to this */
      redGain = 1.0;

      /* To avoid a divide by zero, check for being > zero */
      if(sumGreen > 0.0)
        greenGain = (float)(sumRed / sumGreen);
      else
        greenGain = (float)sumRed;

      if(sumBlue > 0.0)
        blueGain = (float)(sumRed / sumBlue);
      else
        blueGain = (float)sumRed;
    }

    camera_awb_update_color_correction_or_wb(
      redGain, 
      greenGain, 
      blueGain,
      camsensorParamsPtr);
  }
  else
  {
    /* No statistics collected, therefore perform no color correction &
      keep prev gains the same */
  }
#ifdef FEATURE_CAMERA_3A_PROFILING
  /*Calculate the processing Time */
  end_time = timetick_get_ms ();

  processing_time = end_time - start_time;
  if(processing_time > 0)
  {
    cam3a_awb_timing.total_processing_time += processing_time;
    cam3a_awb_timing.total_frames++;
  }
#endif /* FEATURE_CAMERA_3A_PROFILING */


} /* cam3a_do_awb_simple_grey_world_algo */
/*lint -restore */
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

/*===========================================================================

FUNCTION    CAMERA_AWB_DO_AWB_ADVANCED_GREY_WORLD_ALGO

DESCRIPTION
  This function uses a weight vector to weight different light 
  sources depending on indoor, outdoor or inout door.
  Implements Advanced Grey World algorithm for Auto White Balance.
  An AWB algorithm based on grid distance and outlier removal.
  Output is just a selection of the lighting conditions pre-determined gains
  for R and G

  The shortest distance on the grid to the lighting cond. gets selected

  In addition, this algorithm has cluster concept to cover daylight, fluorescent
  and incandescent light conditions. It also involves green line concept.
  Call cam3a_init_awb_advanced_grey_world() before calling
  cam3a_do_awb_advanced_grey_world_algo()

  In this function, fixed point number is being used

  This AWB algo is availble only to the drivers which doesn't have
  latest tuning parameters.

GLOBALS
  cam3a_awb_state - uses varibles in this struct
  cam3a_agw_ptr - Variables, structs and scratch memory here

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e641 -e550 -e818 -e734 -e702 -e834 */
static int camera_awb_do_awb_advanced_grey_world_algo(
  int32                         numRegions,             /* Num regions in input sum of luma array */
  VfeStatOutStruct              * statStruct,           /* Output of VFE for AWB calcs */
  camsensor_static_params_type  * camsensorParamsPtr    /* Parameters for camsensor layer */
  )
{
  int index;
  int y_ave_x, cb_ave_x, cr_ave_x;
  awb_output_type awb_output;
  int white_y_min;
#ifdef FEATURE_CAMERA_3A_PROFILING
  int start_time, end_time, processing_time;
#endif /* FEATURE_CAMERA_3A_PROFILING */

  memset(awb_input_exposure_ptr, 0, sizeof(camera_exposure_info_type));
  camera_aec_retrive_exposure_info(camsensorParamsPtr->aec_enable, EXPOSURE_PER_AWB_ALGO, awb_input_exposure_ptr);
  camera_awb_core_update_complete_aec_history(awb_input_exposure_ptr->current_exposure_index);

  if(camsensorParamsPtr == NULL) return 0;

  MSG_CAM3ADEBUG ("cam3a_do_awb_advanced_grey_world_algo",0,0,0);

#ifdef FEATURE_CAMERA_3A_PROFILING
  start_time = timetick_get_ms ();
#endif /* FEATURE_CAMERA_3A_PROFILING */

  if(cam3a_awb_state.awb_skipped_frame_cnt < 2)
  { /* not OK to use STAT*/
    cam3a_awb_state.awb_skipped_frame_cnt++; 

    if(cam3a_awb_state.current_awb_stat_config == AWB_STAT_REGULAR)
      cam3a_awb_state.regular_stat_on = FALSE;
    else /*cam3a_awb_state.awb_stat_config == AWB_STAT_WHITE*/
      cam3a_awb_state.white_stat_on = FALSE;
    MSG_CAM3ADEBUG("### fxied point: skipped frame count = %d. return", cam3a_awb_state.awb_skipped_frame_cnt,0,0);

    camera_awb_update_color_correction_or_wb(
      cam3a_awb_state.stored_prev_r_gain, 
      cam3a_awb_state.stored_prev_g_gain, 
      cam3a_awb_state.stored_prev_b_gain, 
      camsensorParamsPtr);

    camera_awb_update_color_conversion_for_wb(
      cam3a_awb_state.agw_sample_decision,
      camsensorParamsPtr);

    return 0;  
  }
  else
  {
    cam3a_awb_state.awb_skipped_frame_cnt = 0; /* reset the skipped frame count */
  }

  /* setup awb input stats data start */
  memset(awb_input_stats_ptr, 0, sizeof(awb_input_stats_type));

  /* among the 64 or 256 bins of SCb, SCr, SY1, some are zeros.
     Skip the empty bins and work on the non-empty bins */
  for(index = 0; index < numRegions; index++)
  {
    if(statStruct->NSCb[index]!=0)
    {
      // convert sum of Y, Cb and Cr into ave Y, Cb, Cr, and also remove the offset of 128
      /*lint -save -e713*/
      y_ave_x=((statStruct->SY1[index]<<8) + (statStruct->NSCb[index]>>1))/statStruct->NSCb[index];
      cb_ave_x=((statStruct->SCb[index]<<8) + (statStruct->NSCb[index]>>1))/statStruct->NSCb[index]-(128<<8);
      cr_ave_x=((statStruct->SCr[index]<<8) + (statStruct->NSCb[index]>>1))/statStruct->NSCb[index]-(128<<8);
      /*lint -restore*/
      /* fixed point computation of R, G, B  */
      /*lint -save -e704*/
      awb_input_stats_ptr->rx[awb_input_stats_ptr->bin_cnt]=
        (agw_stats_convert_coeff_A11*y_ave_x) - 
        (agw_stats_convert_coeff_A12*cb_ave_x) +
        (agw_stats_convert_coeff_A13*cr_ave_x) - 
        (camera_3a_shared_update_black_level(cam3a_awb_state.black_level)<<(RGBG_GRID_Q_NUM+8));
      awb_input_stats_ptr->rx[awb_input_stats_ptr->bin_cnt]=(awb_input_stats_ptr->rx[awb_input_stats_ptr->bin_cnt]+128)>>8;  
      awb_input_stats_ptr->gx[awb_input_stats_ptr->bin_cnt]=
        (agw_stats_convert_coeff_A21*y_ave_x) - 
        (agw_stats_convert_coeff_A22*cb_ave_x) -
        (agw_stats_convert_coeff_A23*cr_ave_x) - 
        (camera_3a_shared_update_black_level(cam3a_awb_state.black_level)<<(RGBG_GRID_Q_NUM+8));
      awb_input_stats_ptr->gx[awb_input_stats_ptr->bin_cnt]=(awb_input_stats_ptr->gx[awb_input_stats_ptr->bin_cnt]+128)>>8; 
      awb_input_stats_ptr->bx[awb_input_stats_ptr->bin_cnt]= 
        (agw_stats_convert_coeff_A31*y_ave_x) + 
        (agw_stats_convert_coeff_A32*cb_ave_x) -
        (agw_stats_convert_coeff_A33*cr_ave_x) - 
        (camera_3a_shared_update_black_level(cam3a_awb_state.black_level)<<(RGBG_GRID_Q_NUM+8));
      awb_input_stats_ptr->bx[awb_input_stats_ptr->bin_cnt]=(awb_input_stats_ptr->bx[awb_input_stats_ptr->bin_cnt]+128)>>8; 
      /*lint -restore*/
      awb_input_stats_ptr->bin_cnt++;
    }
  }
  awb_input_stats_ptr->region_cnt = numRegions;
  /* setup awb input stats data end */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
  {
    awb_input_exposure_ptr->search_mode = AWB_AGW_INOUTDOOR_BOTH;

    if(cam3a_awb_state.current_awb_stat_config == AWB_STAT_REGULAR)
    {
      chromatix_wb_exp_stats_type bounding_box;

      cam3a_awb_state.regular_stat_on = TRUE;
      cam3a_awb_state.white_stat_on = FALSE;
      cam3a_awb_state.current_awb_stat_config = AWB_STAT_WHITE;

      white_y_min=(cam3a_awb_state.bounding_box_normal.y_max*cam3a_awb_state.white_y_min_percent)/100;
      MSG_CAM3ADEBUG("### fixed point: white_y_min= %d", white_y_min, 0,0);

    /* check bounding box condition for next frame */
      if(awb_input_exposure_ptr->current_exposure_index == awb_input_exposure_ptr->max_exp_index)
      {   /* fully relax the bounding box */
        bounding_box = cam3a_awb_state.bounding_box_low_light;
        bounding_box.y_min = white_y_min;
        (void)vfe_update_awb_stats_config(bounding_box);
      }
      else
      {   /* restore to the original bounding box (normal light) */
        bounding_box = cam3a_awb_state.bounding_box_normal;
        bounding_box.y_min = white_y_min;
        (void)vfe_update_awb_stats_config(bounding_box);
      }
    }
    else /*cam3a_awb_state.awb_stat_config == AWB_STAT_WHITE */
    {
      cam3a_awb_state.regular_stat_on = FALSE;
      cam3a_awb_state.white_stat_on = TRUE;
      cam3a_awb_state.current_awb_stat_config = AWB_STAT_REGULAR;

    /* check bounding box condition for next frame */
      if(awb_input_exposure_ptr->current_exposure_index == awb_input_exposure_ptr->max_exp_index)
      {   /* fully relax the bounding box */
        (void)vfe_update_awb_stats_config(cam3a_awb_state.bounding_box_low_light);
      }
      else
      {   /* restore to the original bounding box (normal light) */
        (void)vfe_update_awb_stats_config(cam3a_awb_state.bounding_box_normal);
      }
    }
  }

  if(!awb_input_stats_ptr->bin_cnt)
  {
    /* No data, nothing to do */
    if(cam3a_awb_state.white_stat_on == TRUE)
    {
      if(cam3a_awb_state.white_y_min_percent >= YMIN_LOW_LIMIT)
        cam3a_awb_state.white_y_min_percent--;
      cam3a_awb_state.white_decision = CAM3A_AGW_AWB_INVALID;
      cam3a_awb_state.white_has_single_peak=FALSE;
      MSG_CAM3ADEBUG("### white stat : zero bins. return, white decision reset to -1, percentage=%d", 
        cam3a_awb_state.white_y_min_percent, 0, 0);
    }
    else
    {
      MSG_CAM3ADEBUG("### reg stat : zero bins. return", 0, 0, 0);
    }

    camera_awb_update_color_correction_or_wb(
      cam3a_awb_state.stored_prev_r_gain, 
      cam3a_awb_state.stored_prev_g_gain, 
      cam3a_awb_state.stored_prev_b_gain, 
      camsensorParamsPtr);

    camera_awb_update_color_conversion_for_wb(
      cam3a_awb_state.agw_sample_decision,
      camsensorParamsPtr);

    return 0;
  }

  MSG_CAM3ADEBUG("### fixed point: white stat=%d, regular stat=%d, n_bins = %d", 
    cam3a_awb_state.white_stat_on, cam3a_awb_state.regular_stat_on, awb_input_stats_ptr->bin_cnt);

  MSG_CAM3ADEBUG("### ======== AWB begin : prev white decision = %d , reg decision= %d, exp index=%d", 
    cam3a_awb_state.white_decision, cam3a_awb_state.regular_decision, awb_input_exposure_ptr->current_exposure_index);
  MSG_CAM3ADEBUG("### prev r gain = %d , g gain= %d, b gain=%d", 
    cam3a_awb_state.prev_wb.r_gain, cam3a_awb_state.prev_wb.g_gain, cam3a_awb_state.prev_wb.b_gain);

  awb_output = camera_awb_core_run_agw_algorithm(awb_input_stats_ptr,awb_input_exposure_ptr);

  if(!awb_output.is_confident)
  {
    camera_awb_update_color_correction_or_wb(
      cam3a_awb_state.stored_prev_r_gain, 
      cam3a_awb_state.stored_prev_g_gain, 
      cam3a_awb_state.stored_prev_b_gain, 
      camsensorParamsPtr);

    camera_awb_update_color_conversion_for_wb(
      cam3a_awb_state.agw_sample_decision,
      camsensorParamsPtr);

    return 0;
  }

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

  /* store gain adjustment for snapshot */
  /*lint -save -e732*/
  cam3a_awb_state.prev_r_gain_adj = 
    vfe_convert_float_to_Qfactor(awb_output.gain_adj_r,7);
  cam3a_awb_state.prev_b_gain_adj =
    vfe_convert_float_to_Qfactor(awb_output.gain_adj_b,7);
  /*lint -restore*/
  /* AGW/SGW combination */
  camera_awb_update_color_correction_or_wb(
    awb_output.wb_gain_r, 
    awb_output.wb_gain_g, 
    awb_output.wb_gain_b,
    camsensorParamsPtr);

  camera_awb_update_color_conversion_for_wb(
    awb_output.sample_decision,
    camsensorParamsPtr);

  cam3a_awb_state.agw_sample_decision = awb_output.sample_decision;

  cam3a_awb_state.stored_prev_r_gain = awb_output.wb_gain_r;
  cam3a_awb_state.stored_prev_g_gain = awb_output.wb_gain_g; 
  cam3a_awb_state.stored_prev_b_gain = awb_output.wb_gain_b; 

#ifdef FEATURE_CAMERA_3A_PROFILING
  /*Calculate the processing Time */
  end_time = timetick_get_ms ();

  processing_time = end_time - start_time;
  if(processing_time > 0)
  {
    cam3a_awb_timing.total_processing_time += processing_time;
    cam3a_awb_timing.total_frames++;
  }
#endif /* FEATURE_CAMERA_3A_PROFILING */

  return 0;
} /* camera_awb_do_awb_advanced_grey_world_algo */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_AWB_DELIMIT_WB_GAINS

DESCRIPTION:
  Enforce user-configurable min and max WB gains.
  These gains are floats in Q0 format.

CALLERS:
  camera_awb_apply_new_wb_gains ()

GLOBALS:
  cam3a_awb_state - For min and max thresholds.

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
 None

USAGE:

SCOPE:
 Local

=========================================================================== */
static void camera_awb_delimit_wb_gains (camera_1x3_matrix_type *wb)
{
  if(wb == NULL) return;

  /* De-limit the red gain */
  if(wb->r_gain > cam3a_awb_state.awb_max_wb.r_gain)
  {
    wb->r_gain = cam3a_awb_state.awb_max_wb.r_gain;
  }
  else if(wb->r_gain < cam3a_awb_state.awb_min_wb.r_gain)
  {
    wb->r_gain = cam3a_awb_state.awb_min_wb.r_gain;
  }

  /* De-limit the green gain */
  if(wb->g_gain > cam3a_awb_state.awb_max_wb.g_gain)
  {
    wb->g_gain = cam3a_awb_state.awb_max_wb.g_gain;
  }
  else if(wb->g_gain < cam3a_awb_state.awb_min_wb.g_gain)
  {
    wb->g_gain = cam3a_awb_state.awb_min_wb.g_gain;
  }

  /* De-limit the blue gain */
  if(wb->b_gain > cam3a_awb_state.awb_max_wb.b_gain)
  {
    wb->b_gain = cam3a_awb_state.awb_max_wb.b_gain;
  }
  else if(wb->b_gain < cam3a_awb_state.awb_min_wb.b_gain)
  {
    wb->b_gain = cam3a_awb_state.awb_min_wb.b_gain;
  }
} /* cam3a_delimit_wb_gains */

/*===========================================================================

FUNCTION      CAMERA_AWB_UPDATE_PREV_WB_GAINS

DESCRIPTION:
  Updates previous whitebalance gains.

CALLERS:
  camera_awb_apply_new_wb_gains ()

DEPENDENCIES:

SIDE EFFECTS:
  Update the global cam3a_awb_state

RETURN VALUE:
 None

USAGE:

SCOPE:
 Local

=========================================================================== */
static void camera_awb_update_prev_wb_gains (camera_1x3_matrix_type wb)
{
  cam3a_awb_state.prev_wb.r_gain = wb.r_gain;
  cam3a_awb_state.prev_wb.g_gain = wb.g_gain;
  cam3a_awb_state.prev_wb.b_gain = wb.b_gain;
} /* cam3a_update_prev_wb_gains */

/*===========================================================================

FUNCTION      CAMERA_AWB_WB_GAINS_APPLY_BIAS

DESCRIPTION:
  Compensates white balance gains for darker exposure ctrl settings.

CALLERS:
  camera_awb_apply_new_wb_gains ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
 None

USAGE:

SCOPE:
 Local

============================================================================*/
static void camera_awb_wb_gains_apply_bias (camera_1x3_matrix_type *wb)
{
  if(wb == NULL) return;
  /*lint -save -e704*/
  wb->r_gain = (int)((wb->r_gain*cam3a_awb_state.color_correction_bias) >> 7);
  wb->g_gain = (int)((wb->g_gain*cam3a_awb_state.color_correction_bias) >> 7);
  wb->b_gain = (int)((wb->b_gain*cam3a_awb_state.color_correction_bias) >> 7);
  /*lint -restore*/
} /* cam3a_wb_gains_apply_bias */

/*==============================================================================
  EXTERNAL API DEFINITIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AWB_INIT

DESCRIPTION
   Memclears global and locally used structure for 3A.
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
void camera_awb_init (void)
{
  /* Init AWB state struct */
  memset (&cam3a_awb_state, 0, sizeof(cam3a_awb_state_struct));
}

/*===========================================================================

FUNCTION        CAMERA_AWB_MEM_ALLOCATE

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
boolean camera_awb_mem_allocate (void)
{
  cam3a_awb_state.agw_ptr_ready = FALSE;

  if(!camera_awb_core_mem_allocate())
    return FALSE;

  /* Pointer for AGW AWB input stats */
  CAM_Free( awb_input_stats_ptr );
  awb_input_stats_ptr = CAM_Malloc( sizeof(awb_input_stats_type) );
  if(awb_input_stats_ptr == NULL)
  {
    camera_no_memory();
    return FALSE;
  }
  /* Pointer for AGW AWB input exposure */
  CAM_Free( awb_input_exposure_ptr );
  awb_input_exposure_ptr = CAM_Malloc( sizeof(camera_exposure_info_type) );
  if(awb_input_exposure_ptr == NULL)
  {
    camera_no_memory();
    return FALSE;
  }

  cam3a_awb_state.agw_ptr_ready = TRUE;

#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
  /* Pointer to initied and scratch memory for SGW White Balance */
  CAM_Free( cam3a_sgw_ptr );
  cam3a_sgw_ptr = CAM_Malloc( sizeof(cam3a_simple_grey_world_type) );
  if(cam3a_sgw_ptr == NULL)
  {
    camera_no_memory();
    return FALSE;
  }
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

  return TRUE;
}

/*===========================================================================

FUNCTION     CAMERA_AWB_MEM_DEALLOCATE

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
void camera_awb_mem_deallocate (void)
{
  camera_awb_core_mem_deallocate();

  CAM_Free( awb_input_stats_ptr );
  awb_input_stats_ptr = NULL;
  CAM_Free( awb_input_exposure_ptr );
  awb_input_exposure_ptr = NULL;

  cam3a_awb_state.agw_ptr_ready = FALSE;

#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
  CAM_Free( cam3a_sgw_ptr );
  cam3a_sgw_ptr = NULL;
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
}

/*===========================================================================

FUNCTION      CAMERA_AWB_CONFIG

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
void camera_awb_config (
  camsensor_static_params_type * camsensorParamsPtr
/* Parameters for camsensor layer */
  )
{
  if(camsensorParamsPtr == NULL) return;

  /* Init AWB state struct */
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
  cam3a_awb_state.awb_algo = CAM3A_NO_AWB;
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

#ifdef FEATURE_CAMERA_3A_PROFILING
  /* Init AWB Processing Time calculation structures */
  cam3a_awb_timing.total_frames          = 0;
  cam3a_awb_timing.total_processing_time = 0;
#endif /* FEATURE_CAMERA_3A_PROFILING */

#ifdef FEATURE_CAMERA_BESTSHOT_MODE    
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

  if(camsensorParamsPtr->awb_enable)
  {
    if(cam3a_awb_state.agw_ptr_ready)
    {
      vfe_config_awb_per_chromatix(camsensorParamsPtr);
    }

    /*   --------------- AUTO WHITE BALANCE INIT -------- */
    /* Choose AWB algorithm: Simple Grey World or Advanced Grey
      World */
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
    /* cam3a_awb_state.awb_algo = CAM3A_AWB_SIMPLE_GREY_WORLD_ALGO; */
    cam3a_awb_state.awb_algo = CAM3A_AWB_ADVANCED_GREY_WORLD_ALGO;
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

    cam3a_awb_state.current_awb_stat_config = AWB_STAT_REGULAR;
    cam3a_awb_state.awb_skipped_frame_cnt = 0;  
    cam3a_awb_state.regular_stat_on = FALSE;
    cam3a_awb_state.white_stat_on = FALSE;
    cam3a_awb_state.white_decision = CAM3A_AGW_AWB_INVALID; 
    cam3a_awb_state.regular_decision = CAM3A_AGW_AWB_INVALID;
    cam3a_awb_state.reg_green_cnt = 0;
    cam3a_awb_state.regular_ave_rg_ratio = 0.0;
    cam3a_awb_state.regular_ave_bg_ratio = 0.0;
    cam3a_awb_state.white_ave_rg_ratio = 0.0;
    cam3a_awb_state.white_ave_bg_ratio = 0.0;
    cam3a_awb_state.white_has_single_peak=FALSE;

  /* How quickly the AWB gains adapt to current frame biases & hence color
      balance is adjusted */
    cam3a_awb_state.awb_aggressiveness = camctrl_tbl.awb_aggressiveness;

#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
    if((cam3a_awb_state.awb_algo == CAM3A_AWB_SIMPLE_GREY_WORLD_ALGO) &&
      (cam3a_sgw_ptr != NULL))
    {
      camera_awb_init_awb_simple_grey_world_algo();
    }
    else if((cam3a_awb_state.awb_algo == CAM3A_AWB_ADVANCED_GREY_WORLD_ALGO) &&
      (cam3a_awb_state.agw_ptr_ready))
#else
    if(cam3a_awb_state.agw_ptr_ready)
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
    {
      agw_stats_convert_coeff_A11=1 << RGBG_GRID_Q_NUM;
      agw_stats_convert_coeff_A12=(int) (0.228*(float) (1<<RGBG_GRID_Q_NUM)+0.5);
      agw_stats_convert_coeff_A13=(int) (1.402*(float) (1<<RGBG_GRID_Q_NUM)+0.5);
      agw_stats_convert_coeff_A21=agw_stats_convert_coeff_A11;
      agw_stats_convert_coeff_A22=agw_stats_convert_coeff_A12;
      agw_stats_convert_coeff_A23=(int) (0.598*(float) (1<<RGBG_GRID_Q_NUM)+0.5);
      agw_stats_convert_coeff_A31=agw_stats_convert_coeff_A11;
      agw_stats_convert_coeff_A32=(int) (1.772*(float) (1<<RGBG_GRID_Q_NUM)+0.5);
      agw_stats_convert_coeff_A33=agw_stats_convert_coeff_A23;

      camera_awb_core_init_agw_algorithm();
    }
    else
    {
      MSG_ERROR ("cam3a_agw or sgw ptr is not allocated", 0, 0, 0);
    }

    cam3a_awb_state.awb_low_aggressiveness  = AWB_LOW_AGGRESSIVENESS;
    cam3a_awb_state.awb_med_aggressiveness  = AWB_MED_AGGRESSIVENESS;
    cam3a_awb_state.awb_high_aggressiveness = AWB_HIGH_AGGRESSIVENESS;

    /* Default WB to florescent as the grey world algo comes up to speed */
    /* Slam the history to default it! */
    if(vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_TL84) == NULL)
    {
      MSG_ERROR ("wb matrix is not ready", 0, 0, 0);
      return;
    }
    cam3a_awb_state.prev_wb = *(vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_TL84));
    cam3a_awb_state.prev_weighted_wb = cam3a_awb_state.prev_wb;
    cam3a_awb_state.curr_weighted_wb = cam3a_awb_state.prev_wb;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE    
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */

    camera_awb_apply_new_wb_gains(
      cam3a_awb_state.prev_wb,
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
  vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL)
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  
  );

    cam3a_awb_state.low_light_CC_enable = camctrl_tbl.awb_low_light_CC_is_supported;

    cam3a_awb_state.prev_r_gain_adj = 128; /* Q7 */
    cam3a_awb_state.prev_b_gain_adj = 128; /* Q7 */
  }
  cam3a_awb_state.agw_sample_decision = CAM3A_AGW_AWB_INDOOR_WARM_FLO;
}

/*===========================================================================

FUNCTION      CAMERA_AWB_APPLY_NEW_WB_GAINS

DESCRIPTION
   Applies new red and blue gains to the VFE update color correction
   matrix.  Takes into account past history.

CALLERS:
  cam3a_apply_new_wb_gains ()

GLOBALS
   Modified the following two for history:
      cam3a_awb_state

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camera_awb_apply_new_wb_gains(
  camera_1x3_matrix_type wb,
  camera_3x3_rgb_matrix_type *sourceCC
  )
{
  int16 blackOffset;

  blackOffset = -(cam3a_awb_state.black_level);

  if(sourceCC == NULL)
  {
    MSG_ERROR ("color correction matrix is not ready in VFE", 0, 0, 0);
    return;
  }

  /* initializing color correction matrix in awb state */
  cam3a_awb_state.hybrid_CC = *sourceCC;

  camera_awb_delimit_wb_gains (&wb);

  /* Set current to previous for next time. Do this BEFORE applying the
   * color correction bias!!
   */
  camera_awb_update_prev_wb_gains (wb);

  /* Now apply bias to all gains to make up for intentionally darker
   * exposure ctrl setting
   */
  camera_awb_wb_gains_apply_bias (&wb);

  /* APPLY again to try and correct cropping errors */
  camera_awb_delimit_wb_gains (&wb);

  /* Tell Camera Layer to update VFE if WB has changed */
  camera_update_vfe = 1;

  /* Apply white balance and color correction */
  vfe_update_with_new_gain_to_color_correction_or_wb (
    wb, blackOffset, sourceCC);

} /* cam3a_apply_new_wb_gains */

/*===========================================================================

FUNCTION      CAMERA_AWB_IMPLEMENT

DESCRIPTION
   Do inital parse of AEC AWB stats comming back from VFE
   and call functions to implement AEC and AWB.
   
CALLERS:
  cam3a_parse_and_implement_aec_awb ()

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
void camera_awb_implement (
  camsensor_static_params_type * camsensorParamsPtr,
/* Parameters for camsensor layer        */
  int32               numRegions
  )
{
  if(camsensorParamsPtr == NULL) return;

  /* If enabled, go ahead */
  if(camsensorParamsPtr->awb_enable == TRUE)
  {
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
    if(cam3a_awb_state.awb_algo == CAM3A_AWB_SIMPLE_GREY_WORLD_ALGO)
    {
      if(cam3a_sgw_ptr != NULL)
      {
        camera_awb_do_awb_simple_grey_world_algo (
          numRegions,
          vfeStatStruct,
          camsensorParamsPtr);
      }
      else /* else if (cam3a_agw_ptr != NULL) */
      {
        MSG_ERROR ("Simple Grey World cam3a_sgw_ptr is not allocated", 0, 0, 0);
      }
    }
    else if(cam3a_awb_state.awb_algo == CAM3A_AWB_ADVANCED_GREY_WORLD_ALGO)
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
    {
      if(cam3a_awb_state.agw_ptr_ready)
      {
        (void)camera_awb_do_awb_advanced_grey_world_algo(
          numRegions,
          vfeStatStruct,
          camsensorParamsPtr);
      }
      else /* else if (cam3a_agw_ptr != NULL) */
      {
        MSG_ERROR ("Adv. Grey World cam3a_agw_ptr is not allocated", 0, 0, 0);
      }
    }
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
    else
    {
      MSG_ERROR ("Could not call AWB fn!! algo %d", 
        cam3a_awb_state.awb_algo, 0, 0);
    }
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
  }
}
#ifdef FEATURE_CAMERA_3A_PROFILING
/*===========================================================================

FUNCTION      CAMERA_AWB_SHOW_TIMING

DESCRIPTION
   Displays the time taken by the awb algorithm for the current frame
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
void camera_awb_show_timing (void)
{
  uint32 avg_processing_time;

  if(cam3a_awb_timing.total_frames == 0)
    return;

  avg_processing_time = cam3a_awb_timing.total_processing_time * 1000 /
    cam3a_awb_timing.total_frames;
  MSG_HIGH("AWB Average processing time per frame = %dus",
    avg_processing_time, 0, 0);
  cam3a_awb_timing.total_processing_time = 0;
  cam3a_awb_timing.total_frames = 0;
}
#endif /* FEATURE_CAMERA_3A_PROFILING */

/*===========================================================================

FUNCTION:  CAMERA_AWB_SET_WB

DESCRIPTION: 
  set white balance with white balance parm and special effect parm

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
camera_ret_code_type camera_awb_set_wb(camsensor_static_params_type * camsensorParamsPtr, 
  int32 wb_parm, int32 se_parm)
{
  camera_ret_code_type ret_val;
  camera_3x3_ycbcr_matrix_type * matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT);
  camera_3x3_rgb_matrix_type * sourceCC;
  float saturation = 1.0;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
{
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
   sourceCC = vfe_get_color_correction_matrix_ptr(COLOR_CORRECTION_NORMAL);
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  }
  if(sourceCC == NULL)
  {
    MSG_ERROR ("color correction matrix is not ready in VFE", 0, 0, 0);
    return CAMERA_INVALID_PARM;
  }

  switch(wb_parm)
  {
    case CAMERA_WB_AUTO:

      /* Auto is only supported if we are using an AWB algorithm */
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
      if(cam3a_awb_state.awb_algo != CAM3A_NO_AWB)
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
      {
        camsensorParamsPtr->awb_enable = TRUE;
        matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT);
        ret_val = CAMERA_SUCCESS;
      }
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
      else
      {
        ret_val = CAMERA_NOT_SUPPORTED;
      }
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */
      break;

    case CAMERA_WB_CUSTOM:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_WB_CLOUDY_DAYLIGHT:
      camsensorParamsPtr->awb_enable = FALSE; /* Turn off auto WB */
      /* Note the use of "day light" color conversion for cloudy here */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65);
      camera_awb_apply_new_wb_gains(
        *(vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_CLOUDY)),
        sourceCC);
      ret_val = CAMERA_SUCCESS;
      break;

    case CAMERA_WB_TWILIGHT:
    case CAMERA_WB_SHADE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_WB_INCANDESCENT:  /* a.k.a "A" */
      camsensorParamsPtr->awb_enable = FALSE; /* Turn off auto WB */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A);
      camera_awb_apply_new_wb_gains(
        *(vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_A)),
        sourceCC);
      ret_val = CAMERA_SUCCESS;
      break;

    case CAMERA_WB_FLUORESCENT:  /* a.k.a "TL84" */
      camsensorParamsPtr->awb_enable = FALSE; /* Turn off auto WB */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84);
      camera_awb_apply_new_wb_gains(
        *(vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_TL84)),
        sourceCC);
      ret_val = CAMERA_SUCCESS;
      break;

    case CAMERA_WB_DAYLIGHT:  /* a.k.a "D65" */
      camsensorParamsPtr->awb_enable = FALSE; /* Turn off auto WB */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65);
      camera_awb_apply_new_wb_gains(
        *(vfe_get_white_balance_matrix_ptr(WHITE_BALANCE_D65)),
        sourceCC);
      ret_val = CAMERA_SUCCESS;
      break;

    default:
      ret_val = CAMERA_INVALID_PARM;
      break;
  }
  if(ret_val == CAMERA_SUCCESS)
  {
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
    /* Only update Color Conversion if Special Effect is OFF
       If Special Effects is on then we just do not
       touch Color Conversion */
    if(se_parm ==  (int)CAMERA_EFFECT_OFF
#ifdef FEATURE_CAMERA_BESTSHOT_MODE
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
      )
    {
      vfe_set_color_conversion(matrix_ptr, saturation);
    }

    camera_update_vfe = TRUE;
  }

  return ret_val;
} /* camera_awb_set_wb */

/*===========================================================================

FUNCTION      CAMERA_AWB_PREPARE_WB_FOR_SNAPSHOT

DESCRIPTION
  calculate WB for snapshot

CALLERS:
  camera_svcs_setparam_vfe_snapshot ()

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 */
void camera_awb_prepare_wb_for_snapshot(boolean camera_fire_flash, boolean hjr_enabled )
{
  camera_3x3_rgb_matrix_type *sourceCC;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */
  camera_1x3_matrix_type wb;

#if defined FEATURE_CAMERA_BESTSHOT_MODE && defined ANY_FLASH
#error code not present
#endif /* defined FEATURE_CAMERA_BESTSHOT_MODE && defined ANY_FLASH */
  {
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */
    {
    sourceCC = &cam3a_awb_state.hybrid_CC;
    }
    memset(&wb, 0, sizeof(camera_1x3_matrix_type));
    /*lint -save -e737*/
    wb.r_gain = (int)((cam3a_awb_state.prev_wb.r_gain*cam3a_awb_state.prev_r_gain_adj) >> 7);
    wb.g_gain = cam3a_awb_state.prev_wb.g_gain;
    wb.b_gain = (int)((cam3a_awb_state.prev_wb.b_gain*cam3a_awb_state.prev_b_gain_adj) >> 7);
    /*lint -restore*/

  /* Apply bias to the gains for snapshot */
	camera_awb_wb_gains_apply_bias(&wb);

#ifdef FEATURE_CAMERA_HJR
    if((hjr_enabled) == TRUE && (HJR_structure.new_dig_gain > 128))
    {
      /*lint -save -e704*/
      wb.r_gain = (int)((wb.r_gain*HJR_structure.new_dig_gain) >> 7);
      wb.g_gain = (int)((wb.g_gain*HJR_structure.new_dig_gain) >> 7);
      wb.b_gain = (int)((wb.b_gain*HJR_structure.new_dig_gain) >> 7);
      /*lint -restore*/
      vfe_apply_new_gain_to_color_correction_or_wb_for_snapshot (
        wb,
        -(cam3a_awb_state.black_level),
        sourceCC
        );
    }
    else
#endif /* FEATURE_CAMERA_HJR */
    {
      vfe_apply_new_gain_to_color_correction_or_wb_for_snapshot (
        wb,
        -(cam3a_awb_state.black_level),
        sourceCC
        );      
    }
  }
}
/*lint -restore */

/*===========================================================================

FUNCTION      CAMERA_AWB_EFFECTS_OFF

DESCRIPTION
              Set the color conversion table to what WB dictates

DEPENDENCIES
  None

RETURN VALUE
  pointer on color conversion matrix

SIDE EFFECTS
  None

===========================================================================*/
camera_3x3_ycbcr_matrix_type * camera_awb_effects_off(int32 current_wb_parm)
{
  camera_3x3_ycbcr_matrix_type * matrix_ptr;

  /* Revert to the "default" color conversion array only if
     we are in auto white balance; manual WB requires separate
     arrays, dealt with here */
  switch (current_wb_parm)
  {
    case CAMERA_WB_CLOUDY_DAYLIGHT:
      /* Note the use of "day light" color conversion for cloudy here */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65);
      break;
    case CAMERA_WB_INCANDESCENT:  /* a.k.a "A" */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_A);
    break;

    case CAMERA_WB_FLUORESCENT:  /* a.k.a "TL84" */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_TL84);
    break;

    case CAMERA_WB_DAYLIGHT:  /* a.k.a "D65" */
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_D65);
    break;

    case CAMERA_WB_AUTO:
    case CAMERA_WB_CUSTOM:
    case CAMERA_WB_TWILIGHT:
    case CAMERA_WB_SHADE:
    default:
      matrix_ptr = vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT);
      /* Follows the set to default of the special effects array */
      cam3a_awb_state.color_conversion_state = DEFAULT_COLOR_CONVERSION;
    break;
  }
  return matrix_ptr;
}

/*===========================================================================

FUNCTION: CAMERA_AWB_SET_SPECIAL_EFFECT

DESCRIPTION: 
  Apply Special Effects

DEPENDENCIES:

SIDE EFFECTS: 
  None

RETURN VALUE: 
  camera_ret_code_type

============================================================================*/
camera_ret_code_type camera_awb_set_special_effect(int32 parm, 
                                                   int32 current_wb_parm,
                                                   int32 * current_special_effect_ptr)
{
  camera_ret_code_type ret_val = CAMERA_SUCCESS;
  camera_3x3_ycbcr_matrix_type * matrix_ptr = 
    vfe_get_color_conversion_matrix_ptr(COLOR_CONVERSION_DEFAULT);

  switch (parm)
  {
    case CAMERA_EFFECT_OFF:
      /* Revert to the "default" color conversion array only if
         we are in auto white balance; manual WB requires separate
         arrays, dealt with here */
      matrix_ptr = camera_awb_effects_off(current_wb_parm);
      ret_val = CAMERA_SUCCESS;
      break;

    case CAMERA_EFFECT_MONO:
      matrix_ptr = vfe_get_special_effect_matrix_ptr(SPECIAL_EFFECT_MONOCHROME);
      /* Tell AWB algo that special effects are enabled */
      cam3a_awb_state.color_conversion_state = SPECIAL_EFFECTS_COLOR_CONVERSION;
      ret_val = CAMERA_SUCCESS;
      break;

    case CAMERA_EFFECT_NEGATIVE:
      matrix_ptr = vfe_get_special_effect_matrix_ptr(SPECIAL_EFFECT_NEGATIVE);
      /* Tell AWB algo that special effects are enabled */
      cam3a_awb_state.color_conversion_state = SPECIAL_EFFECTS_COLOR_CONVERSION;
      ret_val = CAMERA_SUCCESS;
      break;

    case CAMERA_EFFECT_SOLARIZE:
      //ret_val = camera_enable_solarize();
      ret_val = CAMERA_SUCCESS;
      /* Tell AWB algo that special effects are enabled */
      cam3a_awb_state.color_conversion_state =
        SPECIAL_EFFECTS_COLOR_CONVERSION;
      matrix_ptr = camera_awb_effects_off(current_wb_parm);
      break;

    case CAMERA_EFFECT_PASTEL:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_EFFECT_MOSAIC:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_EFFECT_RESIZE:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;

    case CAMERA_EFFECT_SEPIA:
      matrix_ptr = vfe_get_special_effect_matrix_ptr(SPECIAL_EFFECT_SEPIA);
      /* Tell AWB algo that special effects are enabled */
      cam3a_awb_state.color_conversion_state = SPECIAL_EFFECTS_COLOR_CONVERSION;
      ret_val = CAMERA_SUCCESS;
      break;

    default:
      ret_val = CAMERA_NOT_SUPPORTED;
      break;
  }
  if (ret_val == CAMERA_SUCCESS)
  {
    /* Update global special effect setting */
    *current_special_effect_ptr = parm;
    vfe_set_color_conversion(matrix_ptr, (float)CAMERA_SATURATION_DEFAULT);
    camera_update_vfe = TRUE;
  }
  return ret_val;
}

#endif /* FEATURE_CAMERA_YCBCR_ONLY */

