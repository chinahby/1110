#ifndef CAMERA_AWB_H
#define CAMERA_AWB_H
/*=============================================================================
FILE:       camera_awb.h

SERVICES:   Auto White Balance Header File

DESCRIPTION: 

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

   
$Header: //source/qcom/qct/multimedia/camera/6k/3a/awb/main/latest/inc/camera_awb.h#2 $ $DateTime: 2008/11/14 10:16:02 $ $Author: cgera $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 03/13/07   klee     Moved in the code on special effect from camera_svcs
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/20/07   klee     Fixed all lint errors on AWB
 01/24/07   klee     Enahnced AWB algorithm with white_has_single_peak
 01/09/07   klee     Enhanced to support AEC with digital gains
 12/21/06   klee     Updated AWB to apply gain_adj only for the snapshot, not for preview
 12/01/06   klee     Removed the code under CAMERA_AWB_FLOAT_ON
 11/03/06   klee     Beautified the code by indenting and removing special characters
 11/02/06   klee     Enhance BSM initializing
 10/20/06   jv       Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
 10/17/06   klee     Added stored_prev_r/g/b_gain for AWB convergence problem
 09/27/06   klee     Applied FEATURE_CAMERA_3A_USE_SGW_ALGO on SGW algorithm
 09/08/06   klee     Added camera_awb_prepare_snapshot() for AWB modularization
 08/31/06   klee     Enhance outdoor BSM support by D65 wb initializing
 08/31/06   klee     Bug fix on previous and current wb maintaining
 08/23/06   klee     Fixed the bug on white vs grey world bounding box toggling
 08/10/06   klee     Cleaned up the code regarding tuning variables relocation 
                     - AWB/AEC
                     - Color matrix
 08/01/06   klee     AWB with white world algorithm
 07/25/06   jv       Added color correction matrix as argument to 
                     camera_awb_apply_new_wb_gains 
 07/16/06   klee     Updated cam3a_awb_state sturcture to support bestshot
 05/02/06   klee     Made init and do AGW algorithm to be external API
                     camctrl needs to access to map to camctrl API
 04/12/06   klee     Added support for the latest AWB algorithm (weighted & cluster)
 03/21/06   klee     Added definition and updated awb state structure 
                     regarding fixed point calculation
 03/03/06   klee     Moved CAM3A_AGW_GRID_DIST_2_THRESHOLD into camsensor driver layer
                     - tuning issue
 02/24/06   ven      Added support for profiling awb and aec
 12/01/05   klee     clean-up the code, fix lint error for 4.3 release
                     lint error 818 has been simply surpressed for all over the file.
 11/11/05   klee     add toggling function - requeted by system team
 10/04/05   klee     Initial code transfered from cam3a 

==============================================================================*/
/*==============================================================================
  FEATURES
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
#include "camera_awb_core.h"

#ifdef FEATURE_CAMERA_YCBCR_ONLY
/* define external api for YCBCR */
#define camera_awb_init()
#define camera_awb_mem_allocate() FALSE
#define camera_awb_mem_deallocate()
#define camera_awb_config(camsensorParamsPtr)
#define camera_awb_implement(camsensorParamsPtr, numRegions)
#define camera_awb_show_timing()
#define camera_awb_set_wb(camsensorParamsPtr, wb_parm, se_parm) CAMERA_NOT_SUPPORTED
#define camera_awb_apply_new_wb_gains(wb, sourceCC)
#define camera_awb_prepare_wb_for_snapshot(camera_fire_flash, hjr_enabled)
#define camera_awb_effects_off(current_wb_parm) NULL
#define camera_awb_set_special_effect(parm, current_wb_parm, current_special_effect_ptr) CAMERA_NOT_SUPPORTED
#else /* not defined FEATURE_CAMERA_YCBCR_ONLY */

/*==============================================================================
  CONSTANTS and MACROS
==============================================================================*/
/* Simple Grey World Grid can be smaller than AGW */
#define CAM3A_SGW_AWB_NUMBER_GRID_POINT  21

#define AWB_LOW_AGGRESSIVENESS 0.05
#define AWB_MED_AGGRESSIVENESS 0.15
#define AWB_HIGH_AGGRESSIVENESS 0.45
//#define BLACK_LEVEL 64

#define SGW_MAX_NUM_POINTS_ON_SINGLE_GRID_LOCATION 4
#define SAMPLE_GRID_POINTS 21

#define RGBG_GRID_Q_NUM   10

#define YMIN_HIGH_LIMIT   90    // percent
#define YMIN_LOW_LIMIT    50    // percent

/*==============================================================================
  DATA TYPES
==============================================================================*/
typedef enum
{
  DEFAULT_COLOR_CONVERSION = 0,
  SPECIAL_EFFECTS_COLOR_CONVERSION
} cam3a_color_conversion_state_enum_type;

typedef enum
{
  AWB_STAT_REGULAR = 0,
  AWB_STAT_WHITE
} awb_stat_config_type;

typedef struct
{
#ifdef FEATURE_CAMERA_3A_USE_SGW_ALGO
  /* Choose AWB algorithm: Simple Grey World or Advanced Grey World */
  cam3a_auto_white_balance_algo_type awb_algo;
#endif /* FEATURE_CAMERA_3A_USE_SGW_ALGO */

  /* AWB min/max allowed gains */
  camera_1x3_matrix_type awb_min_wb; /* gains in Q7 */
  camera_1x3_matrix_type awb_max_wb; /* gains in Q7 */

  cam3a_aggressiveness_type awb_aggressiveness;

  float awb_low_aggressiveness;
  float awb_med_aggressiveness;
  float awb_high_aggressiveness;

  /* For WB history to provide a smooth update transition */
  camera_1x3_matrix_type prev_wb; /* gains in Q7 */
#ifdef FEATURE_CAMERA_BESTSHOT_MODE    
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
  /* To pump-up Color correct and compensate for a 
     too-small luma target.  This will work around the VFE
     bug that produces saturation in C Correction matrix */
  int32 color_correction_bias; /* bias in Q7 */

  cam3a_auto_white_balance_illuminant_type agw_sample_decision; /* Adv Grey World illuminant choice */

  /* State of Color Conversion Array */
  cam3a_color_conversion_state_enum_type color_conversion_state;

  /* 3by3 matrix for hybrid color correction matrix */
  /* hybrid_CC is calibrated with sourceCC and low_light_CC */
  camera_3x3_rgb_matrix_type hybrid_CC;

  boolean agw_ptr_ready;

#ifdef FEATURE_CAMERA_BESTSHOT_MODE    
#error code not present
#endif /* FEATURE_CAMERA_BESTSHOT_MODE */
  /* bounding boxes from chromatix */
  chromatix_wb_exp_stats_type bounding_box_low_light;
  chromatix_wb_exp_stats_type bounding_box_normal;
  chromatix_wb_exp_stats_type bounding_box_outdoor;
  /* for white world AWB */
  awb_stat_config_type current_awb_stat_config;
  int awb_skipped_frame_cnt;  
  boolean regular_stat_on;
  boolean white_stat_on;
  cam3a_auto_white_balance_illuminant_type white_decision; 
  cam3a_auto_white_balance_illuminant_type regular_decision;
  int reg_green_cnt;
  int white_y_min_percent;
  float regular_ave_rg_ratio;
  float regular_ave_bg_ratio;
  float white_ave_rg_ratio;
  float white_ave_bg_ratio;
  boolean white_has_single_peak;

  int16 black_level;
  boolean low_light_CC_enable;

  float stored_prev_r_gain;
  float stored_prev_g_gain;
  float stored_prev_b_gain;

  uint32 prev_r_gain_adj; /* red gain adjustment in Q7 */
  uint32 prev_b_gain_adj; /* blue gain adjustment in Q7 */

  camera_1x3_matrix_type prev_weighted_wb; /* weighted gains in Q7 */
  camera_1x3_matrix_type curr_weighted_wb; /* weighted gains in Q7 */

  float saturated_color_conversion_factor;
  float snow_blue_gain_adj_ratio;
  float beach_blue_gain_adj_ratio;
} cam3a_awb_state_struct;

typedef struct
{
  float rgbg_grid[CAM3A_SGW_AWB_NUMBER_GRID_POINT+1];
  int32 sample_grid[SAMPLE_GRID_POINTS][SAMPLE_GRID_POINTS];
} cam3a_simple_grey_world_type;

/*==============================================================================
  EXTERNAL VARIABLES
==============================================================================*/
extern cam3a_awb_state_struct cam3a_awb_state;

/*==============================================================================
  EXTERNAL API DECLARATIONS
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
void camera_awb_init (void);

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
boolean camera_awb_mem_allocate (void);

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
void camera_awb_mem_deallocate (void);

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
  );

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
  );

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
void camera_awb_show_timing (void);
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
  int32 wb_parm, int32 se_parm);

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
  );

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
void camera_awb_prepare_wb_for_snapshot(boolean camera_fire_flash, boolean hjr_enabled );

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
camera_3x3_ycbcr_matrix_type * camera_awb_effects_off(int32 current_wb_parm);

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
                                                   int32 * current_special_effect_ptr);

#endif /* FEATURE_CAMERA_YCBCR_ONLY */
#endif /* CAMERA_AWB_H */

