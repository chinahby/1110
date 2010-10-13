#ifndef CHROMATIX_6550_H
#define CHROMATIX_6550_H
/*============================================================================
                 C H R O M A T I X   D M S S 6 5 5 0

   DESCRIPTION
   Shared structure definition file between the Chromatix 
   sensor tuning tool and the Camera/Camif DMSS code layer. 

   Copyright (c) 2004 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/tuningdata/common/main/latest/inc/chromatix_6550.h#1 $ $DateTime: 2008/05/09 16:34:47 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 11/21/07  cg     Corrected the Typo.
 11/01/07  jpg    Added group and item names as in the 7k header.
 01/27/07  jp     final?? changes before checking in 6k 0201 header file
 09/19/07  klee   turn 0x0201 file format over to chromatix team for future admin
 11/20/06  jv     Version 0x0109. Added saturated_color_conversion_factor 
                  for best shot. Removed saturated_color_conversion matrix.
 10/26/06  jv     Version 0x0109. Added soft_focus_degree for best shot.
 09/08/06  klee   Changed the name aec_min_gain_step to be exposure_index_step
 08/10/06  klee   Added luma_tolerance and aec_min_gain_step for 0x0108
 07/06/06  klee   Added wb_exp_stats_hw_rolloff for 0x0108
 06/15/06  klee   Version 0x0108
                  added new tuning variables
                    new variables are :
                    awb_adj_snow
                    awb_adj_beach
                    outdoor_gamma_index
                    convergence_adj_ratio
                  re-locaated color tuning variables from camsensor driver
                    relocated variables are :
                    indoor_index
                    outdoor_index
                    awb compact cluster related variables
                    color_correction_bias
                    awb_min_gain
                    awb_max_gain
                    cloudy_white_balance
                    sample_influence
                    awb_weight_vector
                    awb_agw_grid_dist_2_threshold
                  removed awb_lighting lighting type to use the same info in camerai.h
 04/06/06  sch    Version 0x0107 
                  New:
                  Gamma tables for low light, out door, back light.
                  Manual white balance for strobe flash, white led flash,
                  spotlight.
                  radius, radius sqaure rolloff tables for low light.
                  luma_target for each flavor of gamma table.
                  AWB stats bounding box for normal, outdoor, low light.
                  Color Correction matrix for low light.
                  filter_mode to chromatix_asf_5_5_type.
                  Color Conversion for Sunset, Skin tone, Saturated,
                  Emphasized orange, Emphasized green.

                  Deprecated:
                  Removed GAMMA_TABLE_SIZE as it is not used. Gamma tables
                  always contain 1024 entries.
 10/06/05  sch    Version 0x0106.
                  Radius rolloff, Y-Rolloff, 5x5 ASF, 3x3 ASF, 
                  AWB Reference points with and without hw rolloff, 
                  Black level offset are added.
 09/08/05  dl     Resolved lint issues. 
 08/12/05  sch    VFE H/W roll off parameters.
 10/29/04  mvd    Version 0x0105.  Added comments indicating RGB order for
                  all params.  Changed manual WB matrix to be a simple 
                  3x1 matrix of WB gains.
 10/04/04  mvd    Renamed #include camqdsp_if.h to camqdsp.h
 09/30/04  mvd    Updates: separate matrix type structures.  Exposure
                  line count/time changed to uint32.  Added color conversion
                  types for each manual WB type.
 09/23/04  mvd    Changed special gamma table name to "solarize" not
                  negative (a mistake) 
 08/25/04  mvd    Changed type of gamma table to CAMQDSP_GammaTableType
                  to match dsp driver layer declaration and use.
 08/13/04  mvd    Added 2nd gamma table for negative effect.  Changed 
                  matrix struct to contain floating point numbers & 
                  removed Q factor type.
 07/22/04  mvd    Inital revision

============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/

#include "camqdsp.h"

/*============================================================================
                        INTERNAL FEATURES
============================================================================*/

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/
#define CHROMATIX_DMSS6550_VERSION 0x0201
/* Currently the gamma table is 256 8 bit entries
   90nm 6550, will push this to 1024 entries */
#define MAX_EXPOSURE_TABLE_SIZE 500

/* VFE H/W Roll off table */
#define ROLLOFF_TABLE_SIZE 33

#define ASF_FILTER_SIZE 3

#define SMOOTHING_FILTER_SIZE (ASF_FILTER_SIZE * ASF_FILTER_SIZE)
#define LAPLACIAN_FILTER_SIZE SMOOTHING_FILTER_SIZE
#define HJR_K_TABLE_SIZE 20

#define MESH_ROLLOFF_SIZE    (17 * 13)
#define ADAPTIVE_BAYER_FILTER_TABLE_SIZE 20

/*============================================================================
                        ABSTRACT DATA TYPES
============================================================================*/
/* ***** Chromatix sub-structures ***** */
/* All data shall be in R G B order, which is the most
   straight forward from a users point of view */

/* Be carefull when changing the c correction, conversion, or
   special effects structs - the function 
   camsensor_convert_3x3_matrix_to_Qfactor() may have to 
   change as well */

/* For VFE Color Correction */
typedef struct
{
  float  a11;  // RtoR
  float  a12;  // GtoR
  float  a13;  // BtoR
  float  a21;  // RtoG
  float  a22;  // GtoG
  float  a23;  // BtoG
  float  a31;  // RtoB
  float  a32;  // GtoB
  float  a33;  // BtoB
  int16  bias_r;  // ROffset
  int16  bias_g;  // GOffset
  int16  bias_b;  // BOffset
} camsensor_color_correction_matrix_type;

/* For VFE Color Conversion */
typedef struct
{
  float  a11;  // RtoY_A11
  float  a12;  // GtoY_A12
  float  a13;  // BtoY_A13
  float  a21;  // RtoCb_A21
  float  a22;  // GtoCb_A22
  float  a23;  // BtoCb_A23
  float  a31;  // RtoCr_A31
  float  a32;  // GtoCr_A32
  float  a33;  // BtoCr_A33
  int16  bias_y;  // YOffset
  int16  bias_Cb;  // CbOffset
  int16  bias_Cr;  // CrOffset
} camsensor_color_conversion_matrix_type;



typedef struct
{
  /* converted to Q7 in the code */
  float green_even;                // GreenEvenRow
  float green_odd;                 // GreenOddRow
  float red;                       // Red
  float blue;                      // Blue
} chromatix_channel_balance_gains_type;

/* Manual white balance gains */
typedef struct
{
  float r_gain;                           // RGain
  float g_gain;                           // GGain
  float b_gain;                           // BGain
} chromatix_manual_white_balance_type;

/* Advanced gray world white balance gains */
/* For now Advanced gray world white balance gains is defined
 * same as manual white balance data type. 
 */
typedef chromatix_manual_white_balance_type chromatix_agw_white_balance_type;




/* For VFE Special Effects */
typedef struct
{
  float  a11;  // RtoY_A11
  float  a12;  // GtoY_A12
  float  a13;  // BtoY_A13
  float  a21;  // RtoCb_A21
  float  a22;  // GtoCb_A22
  float  a23;  // BtoCb_A23
  float  a31;  // RtoCr_A31
  float  a32;  // GtoCr_A32
  float  a33;  // BtoCr_A33
  int16  bias_y;  // YOffset
  int16  bias_Cb;  // CbOffset
  int16  bias_Cr;  // CrOffset
} camsensor_special_effects_matrix_type;

/******************************************************************************
black-level data types 
******************************************************************************/
typedef uint16  chromatix_black_level_offset_type;

typedef enum
{
  BLK_NORMAL_LIGHT =  0,  
  BLK_LOW_LIGHT,          
  BLK_MAX_LIGHT           
} blk_level_light_type;

typedef struct
{
  uint16 black_even_row_even_col;  // BlackEvenRowEvenCol
  uint16 black_even_row_odd_col;   // BlackEvenRowOddCol
  uint16 black_odd_row_even_col;   // BlackOddRowEvenCol
  uint16 black_odd_row_odd_col;    // BlackOddRowOddCol
} chromatix_4_channel_black_level;

/******************************************************************************
Bad pixel correction data types 
******************************************************************************/
typedef enum
{
  BPC_NORMAL_LIGHT =  0,  
  BPC_LOW_LIGHT,          
  BPC_MAX_LIGHT           
} bpc_light_type;

typedef struct
{
  uint16  bpc_diff_threshold_r;    
  uint16  bpc_diff_threshold_g;  
  uint16  bpc_diff_threshold_b;   
} bpc_diff_threshold_type;

/******************************************************************************
exposure table data types 
******************************************************************************/
typedef enum
{
  EXPOSURE_GAIN_PER_REGISTER = 0,  // Register
  EXPOSURE_GAIN_PER_DIGITAL,       // Digital
  EXPOSURE_GAIN_MAX
} chromatix_exposure_gain_type;

typedef uint16 exposure_index_type;

typedef struct
{
  uint16 gain;
  uint32 line_count;
} camsensor_chromatix_exposure_table_type;

/******************************************************************************
AWB data types 
******************************************************************************/
typedef enum
{
  AGW_AWB_OUTDOOR_SUNLIGHT =  0,  // D65
  AGW_AWB_OUTDOOR_CLOUDY,         // D75
  AGW_AWB_INDOOR_INCANDESCENT,    // A
  AGW_AWB_INDOOR_WARM_FLO,        // TL84
  AGW_AWB_INDOOR_COLD_FLO,        // CW
  AGW_AWB_HORIZON,                // H
  AGW_AWB_OUTDOOR_SUNLIGHT1,      // D50
  AGW_AWB_INDOOR_CUSTOM_FLO,      // CustFluo
  AGW_AWB_OUTDOOR_NOON,           // Noon
  AGW_AWB_HYBRID,                 // Daylight
  AGW_AWB_MAX_LIGHT,
  AGW_AWB_INVALID_LIGHT = AGW_AWB_MAX_LIGHT
} awb_light_type;

typedef enum
{
  AWB_STATS_LOW_LIGHT = 0,
  AWB_STATS_NORMAL_LIGHT,
  AWB_STATS_OUTDOOR,
  AWB_STATS_MAX_LIGHT
} chromatix_awb_stats_light_type;

/* Auto white balance reference points */
typedef struct
{
  float red_gain;         // RedG
  float blue_gain;        // BlueG
  float red_gain_adj;     // RedAdj
  float blue_gain_adj;    // BlueAdj
} chromatix_awb_reference_type;

/* AWB stats */
typedef struct
{
  /* 8-bit, Q0, unsigned */
  uint8 y_min;                      // LumaMin

  /* 8-bit, Q0, unsigned */
  uint8 y_max;                      // LumaMax

  /* Slope of neutral region line 1 */
  /* 8-bit, Q4, signed */
  int8 m1;                          // Slope1

  /* Slope of neutral region line 2 */
  /* 8-bit, Q4, signed */
  int8 m2;                          // Slope2

  /* Slope of neutral region line 3 */
  /* 8-bit, Q4, signed */
  int8 m3;                          // Slope3

  /* Slope of neutral region line 4 */
  /* 8-bit, Q4, signed */
  int8 m4;                          // Slope4

  /* Cb intercept of neutral region line 1 */
  /* 12-bit, Q0, signed */
  int16 c1;                         // CbOffset1

  /* Cb intercept of neutral region line 2 */
  /* 12-bit, Q0, signed */
  int16 c2;                         // CrOffset2

  /* Cb intercept of neutral region line 3 */
  /* 12-bit, Q0, signed */
  int16 c3;                         // CbOffset3

  /* Cb intercept of neutral region line 4 */
  /* 12-bit, Q0, signed */
  int16 c4;                         // CrOffset4
} chromatix_wb_exp_stats_type;

/* AGW AWB weight vector */
typedef struct
{
  int indoor_weight;                // Indoor
  int outdoor_weight;               // Outdoor
  int inoutdoor_weight;             // InOut
} chromatix_awb_weight_vector_type;

/* AWB sample influence */
typedef struct
{
  float outdoor_influence;          // Outdoor
  float indoor_influence;           // Indoor
} chromatix_awb_sample_influence_type;

/******************************************************************************
roll-off data types 
******************************************************************************/
typedef enum
{
  ROLLOFF_TL84_LIGHT = 0, /* Flourescent */    // TL84
  ROLLOFF_A_LIGHT,        /* Incandescent */   // A
  ROLLOFF_D65_LIGHT,      /* Day Light */      // D65
  ROLLOFF_LOW_LIGHT,      /* Low Light */      // LowLight
  ROLLOFF_MAX_LIGHT,
  ROLLOFF_INVALID_LIGHT = ROLLOFF_MAX_LIGHT
} camsensor_chromatix_rolloff_light_type;

typedef uint32 rolloff_radius_square_table_type [ROLLOFF_TABLE_SIZE];
typedef double rolloff_correction_factor_table_type [ROLLOFF_TABLE_SIZE];
typedef uint16 rolloff_image_center_type;
typedef uint16 rolloff_image_dimension_type;
typedef uint8  rolloff_intervals_type;

/* Radius Square Rolloff Table */
typedef struct
{
  /* Horizontal coordinate of the center position (Cx) 
   * with respect to the top-left pixel of the clipped window.
   */
  rolloff_image_center_type             cx;         // CenterX
  /* Vertical coordinate of the center position (Cy) 
   * with respect to the top-left pixel of the clipped window.
   */
  rolloff_image_center_type             cy;         // CenterY
  rolloff_image_dimension_type          width;      // Width
  rolloff_image_dimension_type          height;     // Height
  rolloff_intervals_type                N;          // Intervals
  rolloff_radius_square_table_type      radius_square_table;  // RSqTable
  rolloff_correction_factor_table_type  red_cf_table;    // RedTable
  rolloff_correction_factor_table_type  green_cf_table;  // GreenTable
  rolloff_correction_factor_table_type  blue_cf_table;   // BlueTable
} camsensor_chromatix_rolloff_table_type;

typedef struct
{
  float r_gain[MESH_ROLLOFF_SIZE];                  // RGain
  float gr_gain[MESH_ROLLOFF_SIZE];                 // GRGain
  float gb_gain[MESH_ROLLOFF_SIZE];                 // GBGain
  float b_gain[MESH_ROLLOFF_SIZE];                  // BGain
} chromatix_mesh_rolloff_table_type;

/******************************************************************************
5x5 ASF data types 
******************************************************************************/
/* 5 x 5 Adaptive Spatial Filter.
 * There are two components in this filter
 * High Pass Filter (HPF): 5 x 5
 * Low Pass Filter (LPF): 3 x 3
 * HPF can use upto two 5 x 5 filters. The sensor driver chooses to use
 * both or single filter.
 * LPF can be implemented in H/W Luma Filter module or in IPL.
 * LPF is a function of Smoothing and Laplacian filters of size 3 x 3.
 */
typedef struct
{
  int16  a11;
  int16  a12;
  int16  a13;
  int16  a14;
  int16  a15;
  
  int16  a21;
  int16  a22;
  int16  a23;
  int16  a24;
  int16  a25;

  int16  a31;
  int16  a32;
  int16  a33;
  int16  a34;
  int16  a35;
  
  int16  a41;
  int16  a42;
  int16  a43;
  int16  a44;
  int16  a45;

  int16  a51;
  int16  a52;
  int16  a53;
  int16  a54;
  int16  a55;
} matrix_5_5_type;

typedef enum
{
  ASF_LOW_LIGHT = 0,
  ASF_NORMAL_LIGHT,
  ASF_BRIGHT_LIGHT,
  ASF_MAX_LIGHT,
  ASF_INVALID_LIGHT = ASF_MAX_LIGHT 
} chromatix_asf_brightness_level_type;

typedef enum
{
  ASF_MODE_DUAL = 0,
  ASF_MODE_SINGLE,
  ASF_MODE_SMART,
  ASF_MODE_MAX,
  ASF_MODE_INVALID = ASF_MODE_MAX 
} chromatix_asf_mode_type;

typedef int8   smoothing_filter_type [SMOOTHING_FILTER_SIZE];
typedef int8   laplacian_filter_type [LAPLACIAN_FILTER_SIZE];
typedef float  normalize_factor_type;
typedef int8   filter_threshold_type;
typedef float  filter_sharpen_degree_type;
typedef uint8  filter_smoothing_degree_type;

typedef struct
{
  /* noise level to filter out */
  filter_threshold_type        lower_threshold;           // LowThresh
  /* max sharpening limit */
  filter_threshold_type        upper_threshold;           // UpThresh
  /* negative limit */
  filter_threshold_type        negative_threshold;        // NegThresh
  /* max sharpening limit f2 */
  filter_threshold_type        upper_threshold_f2;        // UpThreshF2
  /* negative limit f2 */
  filter_threshold_type        negative_threshold_f2;     // NegThreshF2
  /* filter1 sharpening degree */
  filter_sharpen_degree_type   sharpen_degree_f1;         // SharpAmtF1
  /* filter2 sharpening degree */
  filter_sharpen_degree_type   sharpen_degree_f2;         // SharpAmtF2
  /* smoothing degree */
  filter_smoothing_degree_type smoothing_percent;         // SmoothPct
} asf_setting_type;

typedef struct
{
  /* Filter mode, 0 or 1 */
  uint32                  filter_mode;              // FilterMode
  /* gain for bright light */
  float                   bright_light_gain;        // BrightLightGain
  /* gain for normal light */
  float                   normal_light_gain;        // NormalLightGain
  /* gain for low light */
  float                   low_light_gain;           // LowLightGain
  /* LPF: 3x3 smoothing filter used to calculate luma filter */
  smoothing_filter_type   smoothing_filter;         // SmoothFltr
  /* LPF: 3x3 laplacian filter used to calculate luma filter */
  laplacian_filter_type   laplacian_filter;         // LapFltr
  /* normalize factor to filter 1 */
  normalize_factor_type   normalize_factor1;        // Normal1
  /* normalize factor to filter 2 */
  normalize_factor_type   normalize_factor2;        // Normal2
  /* HPF: 5x5 filter 1 coefficients */
  matrix_5_5_type         filter1;                  // Filter1
  /* HPF: 5x5 filter 2 coefficients */
  matrix_5_5_type         filter2;                  // Filter2
  /* extraction factor */
  uint8                   extraction_factor;        // ExtrFactor
  /* asf settings based on lighting condition */
  asf_setting_type        setting[ASF_MAX_LIGHT];   // Setting
} chromatix_asf_5_5_type;

/******************************************************************************
3x3 H/W ASF data types 
******************************************************************************/
typedef float   asf_filter_type [ASF_FILTER_SIZE][ASF_FILTER_SIZE];
typedef uint16  asf_threshold_type;             /* 10-bit, Q0, unsigned */
typedef uint8   asf_q_factor_type;              /* range 0-8 */
typedef uint8   asf_edge_detection_flag_type;   /* range 0-1 */

typedef struct
{
  asf_filter_type               edge_filter;           // Edge
  asf_filter_type               noise_filter;          // Noise
  asf_threshold_type            noise_threshold;       // EdgeThresh
  asf_threshold_type            edge_threshold;        // NoiseThresh
  asf_q_factor_type             edge_filter_q_factor;  // EdgeQ
  asf_q_factor_type             noise_filter_q_factor; // NoiseQ
  asf_edge_detection_flag_type  edge_detection_flag;   // EdgeFlag
} chromatix_adaptive_spatial_filter_type;

/******************************************************************************
Adaptive Bayer Filter data types 
******************************************************************************/
typedef struct
{
  uint16  abf_lp_threshold_table[ADAPTIVE_BAYER_FILTER_TABLE_SIZE];
  uint8   abf_shift;
  float   abf_ratio_table[ADAPTIVE_BAYER_FILTER_TABLE_SIZE];
  uint16  abf_min_table[ADAPTIVE_BAYER_FILTER_TABLE_SIZE];
  uint16  abf_max_table[ADAPTIVE_BAYER_FILTER_TABLE_SIZE];
} chromatix_adaptive_bayer_filter_data_type;

/******************************************************************************
Auto Flicker Detection data types 
******************************************************************************/
typedef struct
{
  float  afd_std_threshold;                             // StdThresh
  uint8  afd_percent_threshold;                         // PctThresh
  uint32 afd_diff_threshold;                            // DiffThresh
  uint32 afd_frame_ct_threshold;                        // FrameCtThresh
} chromatix_auto_flicker_detection_data_type;

/* Chromatix Full Output Structure */
typedef struct
{
  /******************************************************************************
   Chromatix_dmss6550.h version
  ******************************************************************************/
  uint16 chromatix_version;

  /******************************************************************************
   Color Correction    (ColorCorr)
  ******************************************************************************/
  camsensor_color_correction_matrix_type  color_correction;                         // Default
  camsensor_color_correction_matrix_type  low_light_color_correction;               // LowLight

  /******************************************************************************
   Color Conversion    (ColorConv)
  ******************************************************************************/
  camsensor_color_conversion_matrix_type TL84_flourescent_color_conversion;         // TL84
  camsensor_color_conversion_matrix_type incandescent_color_conversion;             // A
  camsensor_color_conversion_matrix_type daylight_color_conversion;                 // D65
  camsensor_special_effects_matrix_type  monochrome_special_effect;                 // Mono
  camsensor_special_effects_matrix_type  sepia_special_effect;                      // Sepia
  camsensor_special_effects_matrix_type  negative_special_effect;                   // Negative

  /******************************************************************************
   Best-Shot-Mode Color Conversion    (BSMColorConv)
  ******************************************************************************/
  float                                  saturated_color_conversion_factor;         // SatFactor
  camsensor_color_conversion_matrix_type sunset_color_conversion;                   // Sunset
  camsensor_color_conversion_matrix_type skintone_color_conversion;                 // SkinTL84
  camsensor_color_conversion_matrix_type skintone_color_conversion_a;               // SkinA
  camsensor_color_conversion_matrix_type skintone_color_conversion_d65;             // SkinD65

  /******************************************************************************
   MWB
  ******************************************************************************/
  /* Manual white balance gains */
  chromatix_manual_white_balance_type    chromatix_tl84_white_balance;            // Fluorescent
  chromatix_manual_white_balance_type    chromatix_d50_white_balance;             // Sunny
  chromatix_manual_white_balance_type    chromatix_incandescent_white_balance;    // Tungsten
  chromatix_manual_white_balance_type    chromatix_d65_white_balance;             // Cloudy/Shade
  chromatix_manual_white_balance_type    strobe_flash_white_balance;              // Strobe
  chromatix_manual_white_balance_type    led_flash_white_balance;                 // LED
  
  /******************************************************************************
   AWB
  ******************************************************************************/
  /* channel balance */
  chromatix_channel_balance_gains_type   chromatix_channel_balance_gains;          // ChannelGain

  /* Auto white balance reference points, h/w lens rolloff present */
  chromatix_awb_reference_type           awb_reference_hw_rolloff [AGW_AWB_MAX_LIGHT-1]; // RefPoints
  /* Stats bounding box with hw rolloff*/
  chromatix_wb_exp_stats_type            wb_exp_stats_hw_rolloff [AWB_STATS_MAX_LIGHT];  // StatConfig

  int32                                  awb_indoor_index;                            // IndoorIndex
  int32                                  awb_outdoor_index;                           // OutdoorIndex

  float                                  snow_blue_gain_adj_ratio;                // SnowBlueGainAdj
  float                                  beach_blue_gain_adj_ratio;               // BeachBlueGainAdj
  
  int                                    outlier_distance;                        // OutlierDist
  int                                    green_offset_rg;                         // GreenOffRG
  int                                    green_offset_bg;                         // GreenOffBG


/******************************************************************************
 AWBExt  (extended AWB parameters)
******************************************************************************/
  uint16                                 compact_cluster_r2;                      // CClusterDistSq
  uint16                                 compact_cluster_to_ref_point_r2;         // CClusterToRefSq
  uint8                                  a_cluster_threshold;  /* pcercentage */  // AClusterThresh
  uint8                                  f_cluster_threshold;  /* pcercentage */  // FClusterThresh
  uint8                                  day_cluster_threshold;  /* pcercentage */ // DayClusterThresh
  uint8                                  outdoor_green_threshold;                  // OutdoorGThresh
  uint8                                  outdoor_green_threshold_bright_F;         // OutdoorGThrBrtF 
  uint8                                  outdoor_green_threshold_dark_F;           // OutdoorGThrDarkF
  uint8                                  day_cluster_threshold_for_F;              // DayClusterThrF
  uint8                                  white_balance_allow_fline;                // WBAllowFLine
  uint8                                  outdoor_valid_sample_cnt_threshold;       // OutValidSamplesThr
  uint8                                  outdoor_green_upper_threshold;            // OutGreenUpThresh
  uint16                                 r2_threshold;  /* distance square */      // RadiusSquaredThr
  uint8                                  outdoor_green_threshold_bright_A;         // OutGThreshBrightA
  uint8                                  outdoor_green_threshold_dark_A;           // OutGThreshDarkA
  uint8                                  day_cluster_threshold_for_A;              // DayClusterThrA

  float                                  color_correction_global_gain;              // CCGlobalGain
  chromatix_manual_white_balance_type    awb_min_gains;                             // MinGains
  chromatix_manual_white_balance_type    awb_max_gains;                             // MaxGains
  chromatix_awb_sample_influence_type    sample_influence;                          // SampleInfluence
  chromatix_awb_weight_vector_type       awb_weight_vector [AGW_AWB_MAX_LIGHT];     // WeightVector

  int                                    awb_white_world_y_min_ratio;               // WhiteWorldYMinRatio



  /******************************************************************************
   Gamma LUT       (Gamma)
  ******************************************************************************/
#ifndef CAMERA_USES_SOFTDSP
  CAMQDSP_GammaTableType  default_gamma_table;                                      // Default
  CAMQDSP_GammaTableType  lowlight_gamma_table;                                     // LowLight
  CAMQDSP_GammaTableType  outdoor_gamma_table;                                      // Outdoor
  CAMQDSP_GammaTableType  backlight_gamma_table;                                    // Backlight
  uint8                   solarized_gamma_reflection_point;                         // SolarizeReflection
#endif
  /******************************************************************************
   Black Level     (BlackLevel)
  ******************************************************************************/
  chromatix_black_level_offset_type        black_level_offset;                      // NormalLightBlackLevelOffset
  chromatix_black_level_offset_type        black_level_offset_per_low_light;        // LowLightLevelOffset
  chromatix_4_channel_black_level          black_level_4_channel[BLK_MAX_LIGHT];    // 4Channel

  /******************************************************************************
   Lens Rolloff   (Rolloff)
  ******************************************************************************/
  camsensor_chromatix_rolloff_table_type   vfe_hw_rolloff_data[ROLLOFF_MAX_LIGHT];        // Table
  chromatix_mesh_rolloff_table_type        vfe_hw_mesh_rolloff_data[ROLLOFF_MAX_LIGHT];   // Mesh

  /******************************************************************************
   AEC
  ******************************************************************************/
  chromatix_exposure_gain_type             exposure_gain_type;                            // GainType
  exposure_index_type                      num_exposure_values;                           // NumValues
  camsensor_chromatix_exposure_table_type  exposure_table[MAX_EXPOSURE_TABLE_SIZE];       // ExposureEntries
  uint8                                    default_luma_target;                           // Default
  uint8                                    outdoor_luma_target;                           // Outdoor
  uint8                                    lowlight_luma_target;                          // LowLight
  uint8                                    backlight_luma_target;                         // BacklightLuma
  int32                                    luma_tolerance;                                // LumaTolerance
  float                                    exposure_index_adj_step;                       // ExpIndexAdjStep
  uint32                                   iso100_gain;                                   // ISO100Gain
  exposure_index_type                      aec_indoor_index;                              // IndoorIndex
  exposure_index_type                      aec_outdoor_index;                             // OutdoorIndex

  /******************************************************************************
   3x3 H/W ASF   (3X3ASF)
  ******************************************************************************/
  chromatix_adaptive_spatial_filter_type    chromatix_adaptive_spatial_filter;      // Regular
  chromatix_adaptive_spatial_filter_type    chromatix_low_light_asf;                // LowLight

  /******************************************************************************
   5x5 ASF      (5X5ASF)
  ******************************************************************************/
  chromatix_asf_5_5_type             asf_5_5;               // SnapshotFilter
  filter_sharpen_degree_type         soft_focus_degree;     // SoftFocusDegree
  chromatix_asf_5_5_type             asf_5_5_preview;       // PreviewFilter

  /******************************************************************************
   Luma Adaptation   (LumaAdapt)
  ******************************************************************************/
  uint8  la_threshold;           // Thresh            

  /******************************************************************************
   Chroma Suppression  (ChromaSupp)
  ******************************************************************************/
  uint8 cs_luma_threshold1;       // LumaThresh1
  uint8 cs_luma_threshold2;       // LumaThresh2
  uint8 cs_luma_threshold3;       // LumaThresh3
  uint8 cs_luma_threshold4;       // LumaThresh4
  uint8 cs_chroma_threshold1;     // ChromaThresh1
  uint8 cs_chroma_threshold2;     // ChromaThresh2

  /******************************************************************************
   Hand Jittering Reduction  (HJR)
  ******************************************************************************/
  uint8 max_number_of_frames;                // MaxFrames
  uint8 one_to_two_frame_hjr_offset;         // 1to2FrameOffset
  uint8 flat_area_noise_reduction_level;     // FlatAreaNoise
  uint8 texture_noise_reduction_level;       // TextureNoise
  uint8 texture_threshold;                   // TextureThresh
  uint8 hjr_k_table[HJR_K_TABLE_SIZE];       // KTable
  uint8 hjr_bayer_filtering_enable;          // BayerFilterEnable

  /******************************************************************************
   Adaptive Bayer Filter     (ABF)
  ******************************************************************************/
  chromatix_adaptive_bayer_filter_data_type    adaptive_bayer_filter_data;           // SnapshotFilterData
  chromatix_adaptive_bayer_filter_data_type    adaptive_bayer_filter_data_preview;   // PreviewFilterData

  /******************************************************************************
   Auto Flickering Detection  (AFD)
  ******************************************************************************/
  chromatix_auto_flicker_detection_data_type   auto_flicker_detection_data;   // DetectionData

  
  /******************************************************************************
   Bad Pixel Correction  (BPC)
  ******************************************************************************/
  uint8                              bpc_Fmin;                             // FMin
  uint8                              bpc_Fmax;                             // FMax
  bpc_diff_threshold_type            bpc_diff_threshold[BPC_MAX_LIGHT];    // DiffThresh


} camsensor_chromatix_output_type;

#endif /* CHROMATIX_6550_H */

