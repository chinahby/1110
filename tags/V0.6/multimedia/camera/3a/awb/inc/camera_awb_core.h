#ifndef CAMERA_AWB_CORE_H
#define CAMERA_AWB_CORE_H
/*=============================================================================
FILE:       camera_awb_core.h

SERVICES:   Core Auto White Balance Header File

DESCRIPTION:

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary


$Header: //source/qcom/qct/multimedia/camera/6k/3a/awb/main/latest/inc/camera_awb_core.h#2 $ $DateTime: 2008/11/05 11:29:16 $ $Author: cgera $

===============================================================================
===============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   -----    ----------------------------------------------------------
 11/03/08   cg       Updates for AWB 1.4
 04/30/07   zl       Added definition of CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS.
 03/28/07   klee     Modified the name awb_input_exposure_type to be camera_exposure_info_type
 03/12/07   klee     Re-structured by moving out physical layer (vfe/chromatix) into VFE
 02/20/07   klee     Fixed all lint errors on AWB
 12/22/06   klee     Update AWB to provide stable preview by history
 12/21/06   klee     Updated AWB to apply gain_adj only for the snapshot, not for preview
 12/01/06   klee     Removed the code under CAMERA_AWB_FLOAT_ON
 11/03/06   klee     Beautified the code by indenting and removing special characters
 09/11/06   klee     Enhanced algorithm with two more tuning variables for green line
 08/01/06   klee     AWB with white world algorithm
 06/20/06   klee     Initial code

==============================================================================*/
/*==============================================================================
  INCLUDES
==============================================================================*/
#include "camerai.h"
#include "camsensor.h"

/*==============================================================================
  FEATURES
==============================================================================*/

/*==============================================================================
  CONSTANTS and MACROS
==============================================================================*/
/* Increased from 21 to 121 to increas the hybrid case
  sensitivity by 36 times. */
#define CAM3A_AGW_AWB_NUMBER_GRID_POINT 121
#define CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS 128
#define GREEN_Q_NUM 8
#define AWB_MAX_HISTORY 15
#define AWB_AEC_MAX_HISTORY 75

/*==============================================================================
  DATA TYPES
==============================================================================*/
typedef struct
{
  int     bin_cnt;
  int32   region_cnt;
  int32   rx[256];
  int32   gx[256];
  int32   bx[256];
} awb_input_stats_type;

typedef struct
{
  boolean   is_confident;
  cam3a_auto_white_balance_illuminant_type    sample_decision;
  float   wb_gain_r;
  float   wb_gain_g;
  float   wb_gain_b;
  float   gain_adj_r;
  float   gain_adj_b;
} awb_output_type;

typedef struct
{
  float red_gain_table[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];
  float blue_gain_table[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];
  float red_gain_adj[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];
  float blue_gain_adj[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];

  float outdoor_sample_influence;
  float indoor_sample_influence;

  uint16 compact_cluster_r2;   /* compact cluster's radius square */
  uint16 compact_cluster_to_ref_point_r2;    /* the distance square between cluster center and reference point */
  uint8  a_cluster_threshold;  /* pcercentage */
  uint8  f_cluster_threshold;  /* pcercentage */
  uint8  day_cluster_threshold;  /* pcercentage */
  uint8  outdoor_green_threshold;  /* pcercentage */
  uint8  outdoor_green_threshold_bright_F;  /* pcercentage */
  uint8  outdoor_green_threshold_dark_F;  /* pcercentage */
  uint8  day_cluster_threshold_for_F;  /* percentage */
  uint8  white_balance_allow_fline;   /* allow F-line for white balance or not */
  uint8  outdoor_valid_sample_cnt_threshold;  /* percentage */
  uint8  outdoor_green_upper_threshold;  /* pcercentage */
  uint16 r2_threshold;  /* distance square */
  uint8  outdoor_green_threshold_bright_A;  /* pcercentage */
  uint8  outdoor_green_threshold_dark_A;  /* pcercentage */
  uint8  day_cluster_threshold_for_A;  /* percentage */

  int awb_agw_grid_dist_2_threshold;
  int awb_weight_vector[CAM3A_AGW_AWB_NUMBER_OF_LIGHTING_CONDITION][3];

  //float rg_adj;
  //float bg_adj;
  int day_line_1[CAM3A_AGW_AWB_NUMBER_GRID_POINT][2];
  int day_line_2[CAM3A_AGW_AWB_NUMBER_GRID_POINT][2];
  int n_day1;
  int n_day2;
  int rg_grid[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];
  int bg_grid[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];
  float rgbg_grid[CAM3A_AGW_AWB_NUMBER_GRID_POINT+1];
  int sample_rg_grid[256];
  int sample_bg_grid[256];

  /* fixed point parameters used in calculation in
   * fixed point white balance algorithm. */
  int32 red_gain_table_x[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];
  int32 blue_gain_table_x[CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT];
  int32 rgbg_grid_x[CAM3A_AGW_AWB_NUMBER_GRID_POINT+1];
  /* weighted & cluster AWB parameters */
  float green_line_m, green_line_b;
  int n_fline, Fline[CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS][2];
  int n_aline1, Aline1[CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS][2];
  int n_aline2, Aline2[CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS][2];
  int n_day_f_line, Day_F_line[CAM3A_AGW_AWB_F_A_LINE_NUM_POINTS][2];
  int green_offset_rg;
  int green_offset_bg;
  int green_line_mx;
  int green_line_bx;

  int16 awb_history_rg_ratio[AWB_MAX_HISTORY];
  int16 awb_history_bg_ratio[AWB_MAX_HISTORY];
  cam3a_auto_white_balance_illuminant_type  awb_history_decision[AWB_MAX_HISTORY];
  int16 awb_history_exp[AWB_MAX_HISTORY];
  int8  awb_history_replaced[AWB_MAX_HISTORY];
  int8  awb_history_is_white[AWB_MAX_HISTORY];
  int16 awb_history_num;
  int16 awb_history_complete_exp[AWB_AEC_MAX_HISTORY];
  int16 awb_history_complete_exp_cnt;
  
  int n_day3;
  int day3_rg[CAM3A_AGW_AWB_NUMBER_GRID_POINT];
  int day3_bg[CAM3A_AGW_AWB_NUMBER_GRID_POINT];  
  float noon_rg;     // =0.61;  // Sony 2MP PZ
  float noon_bg;     // =0.89;  // Sony 2MP PZ
  float shifted_d50_rg;
  float shifted_d50_bg;
  int indoor_F_WB_locked;
} cam3a_advanced_grey_world_type;

/*==============================================================================
  EXTERNAL VARIABLES
==============================================================================*/
extern cam3a_advanced_grey_world_type * cam3a_agw_ptr;

/*==============================================================================
  EXTERNAL API DECLARATIONS
==============================================================================*/
/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_MEM_ALLOCATE

DESCRIPTION
  allocate memory for AWB

CALLERS:
  camera_awb_mem_allocate

RETURN VALUE
  None

===========================================================================*/
boolean camera_awb_core_mem_allocate (void);

/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_MEM_DEALLOCATE

DESCRIPTION
  free the memory allocated for AWB

CALLERS:
  camera_awb_mem_deallocate

RETURN VALUE
  None

===========================================================================*/
void camera_awb_core_mem_deallocate (void);

/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_INIT_AGW_ALGORITHM

DESCRIPTION
  initiate core AWB algorithm using advanced grey world
  this function is using fixed point number

CALLERS:
  camera_awb_init_awb_advanced_grey_world_algo

RETURN VALUE
  None

===========================================================================*/
void camera_awb_core_init_agw_algorithm(void);

/*===========================================================================

FUNCTION      CAMERA_AWB_CORE_RUN_AGW_ALGORITHM

DESCRIPTION
  core AWB algorithm using advanced grey world
  this function is using fixed point number

CALLERS:
  camera_awb_do_awb_advanced_grey_world_algo

RETURN VALUE
  awb_output_type

===========================================================================*/
awb_output_type camera_awb_core_run_agw_algorithm(awb_input_stats_type* input_stats_ptr,
  camera_exposure_info_type* input_exposure_ptr);

/*===========================================================================

FUNCTION    CAMERA_AWB_CORE_UPDATE_COMPLETE_AEC_HISTORY

DESCRIPTION
  update awb history for stable preview

GLOBALS

DEPENDENCIES
  None

===========================================================================*/
void camera_awb_core_update_complete_aec_history(uint16 current_exposure_index);

#endif /* CAMERA_AWB_CORE_H */
