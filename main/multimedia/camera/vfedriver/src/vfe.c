/*=============================================================================
FILE:       vfe.c

SERVICES:   Video Front End Interface

DESCRIPTION:
   This file provides necessary interfaces for the Video Front End (VFE) that
   ultimately drives the camera.

LIMITATIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 2004 - 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camera/6k/vfedriver/main/latest/src/vfe.c#19 $ $DateTime: 2009/05/21 03:29:26 $ $Author: c_caluru $

CAMIF Version Numbers:

ABSTRACT:
   
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/09   cg      Disabled Black correction to resolve greenish preview issue on ULC
04/14/09   cg      Number of exposure values in Antibanding should be same as that of input exposure table.( Fix for CR # 178815)
03/18/09   chai    Fixed Rvct compiler Warning
03/12/09   cg      Fixed the featurization issue with YCBCR_ONLY feature
03/03/09   cg      fixed the green color snapshot issue
02/23/09   cg      Fixed certain issues with EzTune1.0 ( Dog Timeout while sending all the parameters , Gamma Table read and write issue through chromatix,
                   Preview becomes reddish while trying to send all the paramters , Enabling/Disabling for the VFE blocks) 
02/12/09   cg      Support for 5 MP on LCU and Ez Tune 1.0 bug fixes.
12/24/09   cg      Changes for Ez Tune1.0
12/18/08   sm      Added support for multi raw preview capture
11/25/08   hap     Changes made to resolve AHB overflow issues
11/24/18   chai    Fixed compiler errors and warnings
03/11/08   cg      Updates for Ez Tune 1.0, 3A 1.4 and 0x203 header
06/17/08   ww      Added support for sensor rolloff using illuminant update in VFE_ConfigureVideo(),
                   VFE_ConfigureSnapshot and VFE_ConfigureRawSnapshot(). 
                   Added VFE_Get_Illuminant_for_LC().
                   TL84 is used when sensor rolloff is called from VFE_ConfigureVideo()
05/08/08   cg     Added Support for YCBCR 420 Snpashot for VFE05
04/17/08   chai    Fixed some critical Lint Warnings
04/02/08   hap     Fixed Klocwork errors
03/11/08   cg       a)Support for new VFE firmware image for ULC.
                               b)Added the function vfe_get_type().
01/21/08   cg      a) Fixed the bug in INIT and DELTA Table generation for Mesh Lens Roll Off.
                              b) Enable Mesh Lens Roll Off Correction for Preview,Snapshot and Offline Snapshot.
                              c) Removed reconfiguring of Lens Roll Off while VFE is in capture state for LCU.   
12/19/07   cg      Generating the Solarize Gamma table since 0x201 Header just have the reflex Point 
                   (CR#134460 - solarize not working)
12/18/07   cg      Fixed the issue of preview getting dark when cloudy option is selected
                   in white balance setting
11/21/07   cg      a) Possible NULL pointer access.
                   b) Added support for Chromatix header 0x201.  
11/08/07   hap     Fixing RVCT2.2 compiler warnings
10/16/07   nshree  To fix the compiler warnings
10/09/07   arv     Added changes for VFE25 ABF and ASF
10/02/07   cg      Changed the  MSM id of SC2X from MSM6270 to QSC6270.
09/22/07   klee    Fixed the bug in washed out snapshot image
09/14/07   ven     Changed the VFE_TYPE detection mechanism to refer to 
                   camsensor_info.msm_id to decide the VFE_TYPE.
05/25/07   nshree  Fixed the linking error when FEATURE_CAMERA_YCBCR_ONLY is defined.
05/09/07   klee    Enhanced AWB preview with color correction bias interpolation
04/24/07   ven     Fixed the compilation problem when FEATURE_CAMERA_YCBCR_ONLY is defined.
04/02/07   dle     Restored vfeStatDspOutputBuf, still used by some VFE.
03/30/07   klee    Clean-up lint errors due to un-matched data type in 3a
03/29/09   klee    camera_aec_get_exposure_info_for_gamma_interpolation is integrated
                   into camera_aec_retrive_exposure_info()
03/28/07   klee    Enhancement on gamma table interpolation
03/15/07   klee    Updated preview re-configuration on re-starting
03/14/07   klee    Removed unnecessary conditional statement which is always TRUE
03/12/07   klee    Modified by 3A-restructuring
02/08/07   dle     Fixed lint errors.
11/20/06   jv      Removed static saturated color conversion matrix.
11/15/06   ven     Added the featuriztion flag for MP4 debug messages.
10/20/06   jv      Bayer featurization with FEATURE_CAMERA_YCBCR_ONLY.
10/16/06   klee    Enhanced vfe_init_rolloff_tables to support 3M sensor properly
08/30/06   klee    Removed orange/green emphasized CV matrix support
08/24/06   ven     Added support for Offline snapshot for msm6260
08/10/06   klee    Cleaned up the code regarding tuning variables relocation 
                   - AWB/AEC
                   - Color matrix
08/08/06   klee    Enhance vfe_convert_float_to_Qfactor() with rounding factor
08/03/06   klee    Updated q_factor for WB gains. It should be 7, not 12.
07/25/06   jv      Moved updating of whitebalance to camera_svcs layer
07/18/06   dle     When changing numberOfRegions, need to update bias_table.
07/18/06   dle     Fix AEC/AWB stats numRegion setting bug.
                   Make VFE_4X4_SUPPORT as default.
                   Featurize VFE_16X16_SUPPORT.
07/13/06   sch     Aspect ratio updates.
07/12/06   klee    Enhanced condition for YUV sensors
07/11/06   klee    Fixed crash with YUV sensors
07/10/06   klee    Added backlight gamma table
07/05/06   klee    Added color matrix converting code
06/27/06   jn      Added more AF window validation.
06/22/06   dl      luma 3x3 hw filter is in VFE_EAGLE_12, not in VFE_EAGLE_11
06/21/06   jv      In vfe_start prevented reading non-existant chromatix data 
                   for ycbcr sensors. Ensured color correction and color 
                   correction are disabled for ycbcr sensors.
06/16/06   ven     Moved ASF decision to camctrl
06/06/06   jn      Fill in VFE_VerifyAutoFocusConfigurationType().
06/05/06   ven     Fixed a problem with compilation for non VFE05 based MSMs.
06/02/06   ven     Removed unused feature for VFE05.
05/26/06   dle     Further optimize now vfeVideoConfig, vfeSnapshotConfig and
                   vfeRawConfig are here.
05/25/06   jv      Added 3x3 ASF/Luma support for snapshot, preview + offline
                   Update vfe_start() to take pointer to camsensor_static_params
                   and return camera_ret_code_type. Moved initial vfe 
                   configuration from camera_svcs_start() to vfe_start().
05/22/06   klee    Updated to use additional low-light rolloff table
05/18/06   dle     Align gamma table at 4 byte boundary.
                   Added support for CAMERA_NO_MDDI.
                   Removed chroma_upsampling, no longer needed.
                   In VFE_HisStats check if chroma_histogram is supported.
05/03/06   dle     Corrected a typo for VFE11 and VFE12 MSM mapping.
04/21/06   jch     Changes in VFE interface for burst mode support
04/03/06   dl      Fixed input size for camif subsampling.
04/02/06   sch     VFE20 Support.
03/22/06   klee    Updated vfe_start() to work with different targets
03/20/06   klee    cam3a.c and cam3a.h don't exist in 3a layer anymore.
                   vfe needs to call the same 3a functions with different names
03/20/06   klee    6 vfe variables have been transited from camera_svcs layer
                   Renamed vfe_features_auto_detect() to be vfe_init()
                   Added new function vfe_start()
02/16/06   dle     Linted.
                   Use VERIFY_UR for range, say from 1-4096.
                   Modify VERIFY_U for range that includes 0. Check for 0
                   causes lint warning as unsigned are never less than 0.
01/06/06   dl      Use raw output option set by sensor driver.
12/21/05   ven     Added support for dynamic selection of 4x4, 8x8,  16x16 
                   vfe regions based on the current resolution of the preview.
12/14/05   ven     Added support for VFE 4X4 Statistics collection.
10/14/05   dle     Renamed VFE1X to VFE1P.
10/03/05   cla     Modified video recording profiling code to support debugging 
                   using QXDM.
09/26/05   dl      resolved lint issues.
08/29/05   sch     clipping window is multiple of 8 for JPEG requirement.
08/25/05   dle     vfe_select_rolloff() streamlined, is void again.
                   vfe_update_rolloff_cfg() save params to my_*.
08/22/05   dle     Modified vfe_select_rolloff() to use TL84 only for preview.
                   Added vfe_update_rolloff_cfg(). Modified confiruation
                   functions to use the above new function.
                   Added vfe_build_rolloff_table.
                   Modified vfe_init_rolloff_tables() to return boolean and
                   use above new function.
                   Modified vfe_modify_contrast() to use new function for
                   10bit table.
08/19/05   dle     Added vfe_sw_features, gamma tables, and rolloff tables.
                   vfe_auto_detect() now also set vfe_sw_features.
                   VFE config functions now expect configuration pointers.
                   In VFE_ConfigureVideo(), VFE_ConfigureSnapshot(),
                   VFE_ConfigureOffline, and VFE_Update(), if new VFE modules
                   are present, it now also calculate configuration size, put
                   rolloff table, update rolloff config and white balance
                   config when needed.
                   Added the following functions:
                   - vfe_adjust_config_size(),
                   - vfe_select_rolloff(), 
                   - vfe_modify_contrast()
                   - vfe_init_gamma_table(),
                   - vfe_put_gamma_table().
08/16/05   dw      Updated video recording profiling and event logging system
08/09/05   sch     - Removed 'VFE_VerifyStatisticsEnableType' as it is not
                     used and causing compile warnings.
08/07/05   sch     - Cosmetic changes 
07/28/05   sch     - Includes hw_mm.h
                   - Added vfe_hw_features_auto_detect ()
                   - Added new vfe_hw_features global variable
                   - Changed the range checking functions to static.
                   - FEATURE_CAMERA_VFE_TYPES uses the h/w API to auto
                     detect VFE version and its capabilities. 6550 VFE
                     H/W features are default.
06/15/05   rve     Added black lines to VFE clipping calculation for sensors
                   that need VFE's black level module.   
06/08/05   mvd     Removed compile warning at management's request.
06/06/05   dl      Change range checking for luma filter to 3 bits.
05/04/05   dle     Added FEATURE_CAMERA
                   Added vfe_copy_sensor_snapshot_config(),
                   vfe_copy_sensor_raw_snapshot_config(),
                   vfe_copy_sensor_video_config(), vfe_copy_sensor_config(),
                   vfe_copy_aec_awb_config(), and vfe_copy_af_config().
02/15/05   dw      Added logic analyzer hooks with timelog under VIDEO_TIMETEST
02/03/05   dw      Added code for camcorder time analysis under VIDEO_TIMETEST 
11/30/04   mvd     Changes for new VFE/Camcorder image 0x1008.  Adds error
                   msgs and splits stat reporting.
10/04/04   mvd     Renamed to "vfe.c"
---------------------- OLD History of vfe_if.c ------------------------------
09/02/04   sch     Two different acks for output1 and output2. The ack message
                   contains the buffer addresses.
07/07/04   drl     Added application argument to VFE_Initialize to support
                   Camcorder, renamed CAMQDSP_Command to CAMQDSP_VFE_Command.
03/23/04   drl     Creation

===========================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "customer.h"
#include "comdef.h"
#ifndef CAMERA_USES_SOFTDSP
#ifdef FEATURE_CAMERA
#include "vfe.h"
#include "qdspext.h"
#include "ipl.h"
#include "hw_mm.h"
#include "camsensor.h"
#include "video_enc_aux.h"
#include "camera_3a_shared.h"
#include "assert.h"
#include "camctrl.h"
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */

/*============================================================================
                        MACROS
============================================================================*/

#define IS_UINT7(a) ((a)<=127)
#define IS_UINT8(a) ((a)<=255)
#define IS_UINT10(a) ((a)<=1023)
#define IS_UINT12(a) ((a)<=4095)
#define IS_INT8(a) (-128<=(a) && (a)<=127)
#define IS_INT11(a) (-1024<=(a) && (a)<=1023)
#define IS_INT12(a) (-2048<=(a) && (a)<=2047)


#define VERIFY_BUF_PTR(yPtr, cbcrPtr)  ((yPtr != 0) && (cbcrPtr != 0))
#define VERIFY_U(field,upper) (field < upper)
#define VERIFY_UE(field,upper) (field <= upper)
#define VERIFY_R(field,lower,upper) ((field >= lower) && (field <= upper))
#define VERIFY_INPUT_FORMAT(field)     (VERIFY_UE((int32)field,(int)CAMIF_YCbCr_Cr_Y_Cb_Y))
#define VERIFY_INPUT_FORMAT_OFFLINE_SNAPSHOT(field) (VERIFY_U((int32)field,(int)CAMIF_NumberOf_InputFormatType))
#define VERIFY_YCBCR_INPUTCOSITING(field) \
                                       (VERIFY_UE((int32)field,(int)VFE_CHROMA_COSITED))
#define VERIFY_ENABLE(field)           (VERIFY_UE((int32)field,(int)VFE_Enable)) 

#define VERIFY_IMAGE1FORMAT(field)      (VERIFY_UE((int32)field,(int)VFE_O1YCbCr420_LinePack))
#define VERIFY_IMAGE2FORMAT(field)      (VERIFY_UE((int32)field,(int)VFE_O2YCbCr420_LinePack))

#define VERIFY_COEFF_Q_FACTOR(field)   (VERIFY_UE((int32)field,(int)VFE_SIGNED_Q10))
#define VERIFY_COEFF_3BIT_Q_FACTOR(field) \
                                       (VERIFY_UE((int32)field,(int)VFE_SIGNED_3BIT_Q7))


#ifdef FEATURE_CAMERA_VFE_16X16_SUPPORT
  #define VERIFY_NUMBEROFREGIONS(field)  (VERIFY_UE((int32)field, (int)VFE_16x16))
#else
  #define VERIFY_NUMBEROFREGIONS(field)  (VERIFY_UE((int32)field, (int)VFE_4x4))
#endif /* FEATURE_CAMERA_VFE_16X16_SUPPORT*/
                                        
#define VERIFY_SNAPSHOTCOSITING(field) (VERIFY_UE((int32)field,(int)JPEG))

#ifndef QDSP_MODULE_VFE05_DEFINED
#define VERIFY_YCBCRIMAGEFORMAT(field) (VERIFY_UE((int32)field,(int)VFE_YCbCrH1V1))
#else
#define VERIFY_YCBCRIMAGEFORMAT(field) (VERIFY_UE((int32)field,(int)VFE_YCbCrH2V2_YUV))
#endif

#define VERIFY_PIXEL_DATA_WIDTH(field) (VERIFY_UE((int32)field,(int)VFE_10Bit))

#define BILINEAR_INTERPOLATION(v1, v2, ratio) ((v1) + ((ratio) * ((v2) - (v1))))

/*============================================================================
                        CONSTANTS
============================================================================*/
#define VFE_IMAGE_FRAGMENTS 4

#define QFACTOR             7
#define QVALUE             (1 << QFACTOR)
#define QROUNDING          (QVALUE >> 1)


/*============================================================================
                        GLOBAL VARIABLES
============================================================================*/
vfe_hw_feature_type vfe_hw_features;
vfe_sw_feature_type vfe_sw_features;
#ifndef FEATURE_CAMERA_YCBCR_ONLY
/* Copied or downsized from device default gamma table */
__align(4)
CAMQDSP_GammaTableType  vfe_default_gamma_table;
/* Updated with current contrast from default gamma table */
__align(4)
CAMQDSP_GammaTableType  vfe_current_gamma_table;
/* Copied or downsized from device, solarized gamma table */
__align(4)
CAMQDSP_GammaTableType  vfe_solarized_effects_gamma_table;
uint32 vfe_gamma_table_size;
CAMQDSP_GammaTableType  vfe_outdoor_gamma_table;
CAMQDSP_GammaTableType  vfe_lowlight_gamma_table;
CAMQDSP_GammaTableType  vfe_backlight_gamma_table;
CAMQDSP_GammaTableType  vfe_interpolated_gamma_table;
static int32 vfe_current_contrast;
static vfe_gamma_table_type vfe_current_gamma_table_type;
/* Rolloff table for lighting conditions a full scale */
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_A_F;
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_D65_F;
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_TL84_F;
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_low_light_F;
VFE_RolloffCfgType   vfe_rolloff_cfg_A_F;
VFE_RolloffCfgType   vfe_rolloff_cfg_D65_F;
VFE_RolloffCfgType   vfe_rolloff_cfg_TL84_F;
VFE_RolloffCfgType   vfe_rolloff_cfg_low_light_F;
/* Rolloff table for lighting conditions a quarter size scale */
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_A_Q;
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_D65_Q;
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_TL84_Q;
__align(4)
CAMQDSP_RolloffType  vfe_rolloff_table_low_light_Q;
VFE_RolloffCfgType   vfe_rolloff_cfg_A_Q;
VFE_RolloffCfgType   vfe_rolloff_cfg_D65_Q;
VFE_RolloffCfgType   vfe_rolloff_cfg_TL84_Q;
VFE_RolloffCfgType   vfe_rolloff_cfg_low_light_Q;
CAMQDSP_RolloffType *vfe_current_rolloff_table;
VFE_RolloffCfgType   vfe_current_rolloff_cfg;
VFE_AsfCfgType       vfe_asf_cfg;
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
uint32 vfeStatDspOutputBuf[1030];

boolean vfe_color_matrix_ready;
/* 2 preview color correction matrix */
camera_3x3_rgb_matrix_type vfe_color_correction_matrix[COLOR_CORRECTION_MAX_NUM];

/* 2 snapshot colcor correction matrices */
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */

/* 9 color conversion matrix */
camera_3x3_ycbcr_matrix_type vfe_color_conversion_matrix[COLOR_CONVERSION_MAX_NUM];
/* 3 color conversion matrix for special effect */
camera_3x3_ycbcr_matrix_type vfe_special_effect[SPECIAL_EFFECT_MAX_NUM];
/* 7 white balance */
camera_1x3_matrix_type vfe_white_balance[WHITE_BALANCE_MAX_NUM];

/*Initialize the Default Color Conversion Matrix*/

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
camsensor_color_conversion_matrix_type vfe_default_color_conversion_matrix=

{ 
    
        0.2990f,    0.5870f,    0.1140f,

        -0.1687f,    -0.3313f,    0.5000f,

        0.5000f,    -0.4187f,    -0.0813f,

         0,    128,    128

};
#endif /* FEATURE_CAMERA_USES_CAMERA_CHROMATIX_0x203 */
/* Leave this in just in case some VFE still DME to Arm memory, for
 * backward compatiblity */
/* Address where raw VFE output from DSP will be written */
/* Just a pointer */
VFE_ExposureAndColorBalanceConfigurationType vfeExpWbStatDspInput;
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
VFE_AutoFocusConfigurationType vfeAfStatDspInput;

/* variables from camera_svcs */
VFE_VideoConfigureType        vfeVideoConfig;
VFE_ConfigureRawType          vfeRawConfig;
VFE_ConfigureSnapshotType     vfeSnapshotConfig;
VFE_ConfigurationOfflineType  vfeOfflineConfig;
VFE_UpdateType                vfeUpdate;

#ifndef FEATURE_CAMERA_YCBCR_ONLY
int snapshot_offset_k0;
int snapshot_offset_k1;
int snapshot_offset_k2;
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
/*lint -save -e64 -e834 enum=num, parentheses */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
extern camsensor_static_params_type  camsensor_static_params [];
extern uint8 camera_asi;
#if defined  QDSP_MODULE_VFE25_DEFINED && !defined FEATURE_CAMERA_YCBCR_ONLY
extern float preview_downscale_factor;
extern float snapshot_downscale_factor;
#endif /* QDSP_MODULE_VFE25_DEFINED */
/*============================================================================
                        INTERNAL API DECLARATIONS
============================================================================*/
static boolean VFE_VerifyInputConfigurationType          (VFE_InputConfigurationType           config);
#ifdef QDSP_MODULE_VFE05_DEFINED
static boolean VFE_VerifyOutput1Size                     (VFE_YCbCrOutputConfigurationType     config);  
#endif /* QDSP_MODULE_VFE05_DEFINED */  
static boolean VFE_VerifyOutputConfigurationType         (VFE_OutputConfigurationType          config);
static boolean VFE_VerifyVideoOutput2ConfigurationType   (VFE_VideoOutput2ConfigurationType    config);
static boolean VFE_VerifyClipType                        (VFE_ClipType                         config);
static boolean VFE_VerifyAHBConfigurationType            (VFE_AHB_ConfigurationType            config);
//static boolean VFE_Verify_int_1_4_Type                   (VFE_int_1_4_Type                     config);
//static boolean VFE_VerifyEnableType                      (VFE_EnableType                       config);
static boolean VFE_Verify_int_1_4096_Type                (VFE_int_1_4096_Type                  config);
static boolean VFE_Verify_int_1_256_Type                 (VFE_int_1_256_Type                   config);
static boolean VFE_Verify_int_1_16_Type                  (VFE_int_1_16_Type                    config);
static boolean VFE_Verify_int_0_3_Type                   (VFE_int_0_3_Type                     config);
static boolean VFE_Verify_int_0_15_Type                  (VFE_int_0_15_Type                    config);
static boolean VFE_VerifyInputSnapshotConfigurationType  (VFE_InputSnapshotConfigurationType   config);
static boolean VFE_VerifyAck1Type                        (VFE_EOF_Ack1Type  config);
static boolean VFE_VerifyAck2Type                        (VFE_EOF_Ack2Type  config);
static boolean VFE_VerifySnapshotAck2Type                (VFE_EOF_Ack2Type  config);
static boolean VFE_VerifyHistogramConfigurationType      (VFE_HistogramConfigurationType       config);
static boolean VFE_VerifyExposureAndColorBalanceConfigurationType (VFE_ExposureAndColorBalanceConfigurationType config);
static boolean VFE_VerifyAutoFocusConfigurationType      (VFE_AutoFocusConfigurationType       config);
static boolean VFE_VerifyYCbCrOutputConfigurationType    (VFE_YCbCrOutputConfigurationType config);
static boolean VFE_VerifyOfflineInputConfigurationType   (VFE_OfflineInputConfigurationType config);
static boolean VFE_VerifyVideoOutputFrameSkipPatternType (VFE_VideoOutputFrameSkipPatternType  config);
static void vfe_default_color_conversion_thresholds (VFE_ColorConversionType *);
static boolean vfe_input_ycbcr (CAMIF_InputFormatType );
static CAMQDSP_VfeType vfe_type;

#ifdef FEATURE_CAMERA_YCBCR_ONLY
#define VFE_VerifyGammaCorrectionType(config)             TRUE
#define VFE_VerifyBlackCorrectionType(config)             TRUE
#define VFE_VerifyDefectPixelCorrectionType(config)       TRUE
#define VFE_VerifyColorConversionType(config)             TRUE
#define VFE_VerifyColorCorrectionType(config)             TRUE
#define VFE_Verify3x3LumaFilterType(config)               TRUE

#else
/* These functions should only exist for bayer build */
static boolean VFE_VerifyGammaCorrectionType             (VFE_GammaCorrectionType              config);
static boolean VFE_VerifyBlackCorrectionType             (VFE_BlackCorrectionType              config);
static boolean VFE_VerifyDefectPixelCorrectionType       (VFE_DefectPixelCorrectionType        config);
static boolean VFE_VerifyColorConversionType             (VFE_ColorConversionType    config);
static boolean VFE_VerifyColorCorrectionType             (VFE_ColorCorrectionType    config);
#if !defined QDSP_MODULE_VFE25_DEFINED
static boolean VFE_Verify3x3LumaFilterType               (VFE_3x3LumaFilterType                config);
#endif /* QDSP_MODULE_VFE25_DEFINED */
static void vfe_init_color_matrix(const camsensor_static_params_type *camsensor_params);
//static void vfe_update_3_3_asf_matrix(camsensor_static_params_type *camsensor)params , light_level_trigger_type light_level);

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else 
static boolean vfe_convert_color_correction_matrix_to_Qfactor(
  camsensor_color_correction_matrix_type * source,
  camera_3x3_rgb_matrix_type * dest,
  int q_factor
  );
static boolean vfe_convert_color_conversion_matrix_to_Qfactor(
  camsensor_color_conversion_matrix_type * source,
  camera_3x3_ycbcr_matrix_type * dest,
  int q_factor
  );
static boolean vfe_convert_special_effect_matrix_to_Qfactor(
  camsensor_special_effects_matrix_type * source,
  camera_3x3_ycbcr_matrix_type * dest,
  int q_factor
  );
#endif /* CAMERA_USES_CHROMATIX_0x203 */

#if defined (CAMERA_USES_CHROMATIX_0x201) || defined (CAMERA_USES_CHROMATIX_0x203)
static boolean vfe_convert_white_balance_matrix_to_Qfactor(
  chromatix_manual_white_balance_type * source,
  camera_1x3_matrix_type * dest,
  int q_factor
  );
#else
static boolean vfe_convert_white_balance_matrix_to_Qfactor(
  camsensor_white_balance_matrix_type * source,
  camera_1x3_matrix_type * dest,
  int q_factor
  );
#endif
static camera_3x3_rgb_matrix_type vfe_unpack_ColorCorrection(
  VFE_ColorCorrectionType *packed_cc_ptr
  );
static void vfe_interpolate_gamma_table(void);
static void vfe_reset_color_correction_bias_per_snapshot(
  VFE_ColorCorrectionType *packed_cc_ptr
  );
static camsensor_lc_type VFE_Get_Illuminant_for_LC(void);

#endif /* FEATURE_CAMERA_YCBCR_ONLY */

static void vfe_calculate_video_camif_subsample (void);

/*============================================================================
                        INTERNAL API DEFINITIONS
============================================================================*/
/*lint -save -e650 NumberOf_*+1 out of field range */
static boolean VFE_VerifyInputConfigurationType          (VFE_InputConfigurationType           config)
{
  return (VERIFY_INPUT_FORMAT (config.format) &&
          VERIFY_YCBCR_INPUTCOSITING (config.YCbCrCositing));
} /* VFE_VerifyInputConfigurationType */

#ifdef QDSP_MODULE_VFE05_DEFINED
static boolean VFE_VerifyOutput1Size                     (VFE_YCbCrOutputConfigurationType      config)
{
   return (VERIFY_UE(config.width ,320)&&
           VERIFY_UE(config.height,240));                                                
}
#endif /* QDSP_MODULE_VFE05_DEFINED */

static boolean VFE_VerifyOutputConfigurationType         (VFE_OutputConfigurationType          config)
{
  return (VERIFY_IMAGE1FORMAT (config.format) &&
          VERIFY_YCBCR_INPUTCOSITING (config.YCbCrCositing));
} /* VFE_VerifyOutputConfigurationType */

static boolean VFE_VerifyVideoOutput2ConfigurationType   (VFE_VideoOutput2ConfigurationType    config)
{
  return (VERIFY_ENABLE (config.output2Enable) &&
          VERIFY_ENABLE (config.startBackend)  &&
          VERIFY_IMAGE2FORMAT (config.format));
} /* VFE_VerifyVideoOutput2ConfigurationType */

static boolean VFE_VerifyAHBConfigurationType            (VFE_AHB_ConfigurationType            config)
{
  return (VERIFY_R (config.burstFactor1ForYBuffers,    1, 4096) &&
          VERIFY_R (config.burstFactor2ForYBuffers,    1, 256)  &&
          VERIFY_R (config.burstLenForYBuffers,        1, 16)   &&
          VERIFY_R (config.numBurstForYBuffers,        1, 16)   &&
          VERIFY_R (config.burstFactor1ForCbCrBuffers, 1, 4096) &&
          VERIFY_R (config.burstFactor2ForCbCrBuffers, 1, 256)  &&
          VERIFY_R (config.burstLenForCbCrBuffers,     1, 16)   &&
          VERIFY_R (config.numBurstForCbCrBuffers,     1, 16));
} /* VFE_VerifyAHBConfigurationType */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
static boolean VFE_VerifyGammaCorrectionType             (VFE_GammaCorrectionType              config)
{
  return (VERIFY_ENABLE (config.enable));
} /* VFE_VerifyGammaCorrectionType */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
static boolean VFE_VerifyBlackCorrectionType             (VFE_BlackCorrectionType              config)
{
  return (VERIFY_ENABLE (config.enable));
} /* VFE_VerifyBlackCorrectionType */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
static boolean VFE_VerifyDefectPixelCorrectionType       (VFE_DefectPixelCorrectionType        config)
{
  return (VERIFY_ENABLE (config.enable));
} /* VFE_VerifyDefectPixelCorrectionType */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
static boolean VFE_VerifyColorCorrectionType             (VFE_ColorCorrectionType    config)
{
  return (VERIFY_ENABLE (config.enable)       &&
          VERIFY_COEFF_Q_FACTOR (config.coefficient_Q_Factor));
} /* VFE_VerifyColorCorrectionType */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
static boolean VFE_VerifyColorConversionType             (VFE_ColorConversionType    config)
{
  return (VERIFY_ENABLE (config.enable)       &&
          VERIFY_COEFF_Q_FACTOR (config.coefficient_Q_Factor));
} /* VFE_VerifyColorConversionType */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifndef QDSP_MODULE_VFE25_DEFINED
static boolean VFE_Verify3x3LumaFilterType               (VFE_3x3LumaFilterType                config)
{
  if (vfe_hw_features.filter_3x3_luma == FALSE)
  {
    return TRUE;
  }

  return (VERIFY_ENABLE (config.enable) &&
          VERIFY_COEFF_3BIT_Q_FACTOR (config.coefficient_Q_Factor));
} /* VFE_Verify3x3LumaFilterType */
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*lint -save -e715 config not referenced */
static boolean VFE_VerifyVideoOutputFrameSkipPatternType (VFE_VideoOutputFrameSkipPatternType config)
{
  return (TRUE);
} /* VFE_VerifyVideoOutputFrameSkipPatternType */

static boolean VFE_VerifyClipType                        (VFE_ClipType                         config)
{
  return (TRUE);
} /* VFE_VerifyClipType */
/*lint -restore */

#if 0
static boolean VFE_Verify_int_1_4_Type                   (VFE_int_1_4_Type                     config)
{
  return VERIFY_R (config, 1, 4);
} /* VFE_Verify_int_1_4_Type */

static boolean VFE_VerifyEnableType                      (VFE_EnableType                       config)
{
  return VERIFY_ENABLE (config);
} /* VFE_VerifyEnableType */
#endif 
static boolean VFE_Verify_int_0_3_Type                   (VFE_int_0_3_Type                     config)
{
  return VERIFY_U (config, 4);
}
static boolean VFE_Verify_int_0_15_Type                  (VFE_int_0_15_Type                    config)
{
  return VERIFY_U (config, 16);
}
static boolean VFE_Verify_int_1_4096_Type                (VFE_int_1_4096_Type                  config)
{
  return VERIFY_R (config, 1, 4096);
} /* VFE_Verify_int_1_4096_Type */

static boolean VFE_Verify_int_1_256_Type                 (VFE_int_1_256_Type                   config)
{
  return VERIFY_R (config, 1, 256);
} /* VFE_Verify_int_1_256_Type */

static boolean VFE_Verify_int_1_16_Type                  (VFE_int_1_16_Type                    config)
{
  return VERIFY_R (config, 1, 16);
} /* VFE_Verify_int_1_16_Type */

static boolean VFE_VerifyInputSnapshotConfigurationType  (VFE_InputSnapshotConfigurationType   config)
{
  return (VERIFY_INPUT_FORMAT        (config.format)
#ifdef QDSP_MODULE_VFE05_DEFINED
          );
#else
         && VERIFY_YCBCR_INPUTCOSITING (config.YCbCrCositing));
#endif
} /* VFE_VerifyInputSnapshotConfigurationType */

static boolean VFE_VerifyAck1Type (VFE_EOF_Ack1Type config)
{
  return VERIFY_BUF_PTR (config.eofAck1.Y_Address, config.eofAck1.CbCr_Address);
} /* VFE_VerifyAck1Type */

static boolean VFE_VerifyAck2Type (VFE_EOF_Ack2Type config)
{
  return VERIFY_BUF_PTR (config.eofAck2[0].Y_Address, config.eofAck2[0].CbCr_Address); 
} /* VFE_VerifyAck2Type */

static boolean VFE_VerifySnapshotAck2Type (VFE_EOF_Ack2Type config)
{
  uint32 i;
  boolean result = TRUE;
  for ( i = 0; i < VFE_MAX_BUFFER_FRAGMENTS; i++) 
  {
    result = result && VERIFY_BUF_PTR (config.eofAck2[i].Y_Address, config.eofAck2[i].CbCr_Address); 
  }
  return result;
} /* VFE_VerifyAck2Type */

/*lint -save -e715 config not referenced */
static boolean VFE_VerifyHistogramConfigurationType      (VFE_HistogramConfigurationType       config)
{
  return (TRUE);
} /* VFE_VerifyHistogramConfigurationType */
/*lint -restore */

static boolean VFE_VerifyExposureAndColorBalanceConfigurationType
                                                  (VFE_ExposureAndColorBalanceConfigurationType config)
{
  return (VERIFY_NUMBEROFREGIONS (config.numberOfRegions));
} /* VFE_VerifyExposureAndColorBalanceConfigurationType */

static boolean VFE_VerifyAutoFocusConfigurationType      (VFE_AutoFocusConfigurationType       config)
{
  uint16 tmpFirst, tmpLast;

  /* AF window coordinates are relative to the clipping window */
  if ((config.firstPixel > vfeVideoConfig.clip.lastPixel - vfeVideoConfig.clip.firstPixel) ||
      (config.lastPixel  > vfeVideoConfig.clip.lastPixel - vfeVideoConfig.clip.firstPixel) ||
      (config.firstLine  > vfeVideoConfig.clip.lastLine - vfeVideoConfig.clip.firstLine) ||
      (config.lastLine   > vfeVideoConfig.clip.lastLine - vfeVideoConfig.clip.firstLine))
  {
    return FALSE;
  }

  /* after subsampling by 2, AF window width must be even, width and height must be <= 508 */
  tmpFirst =  config.firstPixel >> 1;
  tmpLast  =  config.lastPixel  >> 1;
  if (((tmpFirst & 1) != 0) || ((tmpLast & 1) != 1) || ((tmpLast - tmpFirst + 1) > 508))
  {
    return FALSE;
  }
  tmpFirst =  config.firstLine >> 1;
  tmpLast  =  config.lastLine  >> 1;
  if ((tmpLast - tmpFirst + 1) > 508)
  {
    return FALSE;
  }

  return TRUE;
} /* VFE_VerifyAutoFocusConfigurationType */

static boolean VFE_VerifyYCbCrOutputConfigurationType (VFE_YCbCrOutputConfigurationType config)
{
  return (VERIFY_SNAPSHOTCOSITING (config.cositing)    &&
          VERIFY_YCBCRIMAGEFORMAT (config.imageFormat));

} /* VFE_VerifyYCbCrOutputConfigurationType */

static boolean VFE_VerifyOfflineInputConfigurationType (VFE_OfflineInputConfigurationType config)
{
#ifdef QDSP_MODULE_VFE05_DEFINED
  return (VERIFY_INPUT_FORMAT_OFFLINE_SNAPSHOT (config.format)        &&
          VERIFY_PIXEL_DATA_WIDTH    (config.dataWidth));
#else
  return (VERIFY_YCBCR_INPUTCOSITING (config.YCbCrCositing) &&
          VERIFY_INPUT_FORMAT_OFFLINE_SNAPSHOT (config.format)        &&
          VERIFY_PIXEL_DATA_WIDTH    (config.dataWidth));
#endif
} /* VFE_VerifyOfflineInputConfigurationType */
/*lint -restore */


/*===========================================================================

FUNCTION:
  VFE_DEFAULT_COLOR_CONVERSION_THRESHOLDS

DESCRIPTION:
  Color conversion thresholds need to be set even though the H/W module
  is not used. This is a work around for blank images along either 
  output1 or output2 path for YCbCr sensors.

CALLERS:
  VFE_ConfigureVideo ()
  VFE_ConfigureSnapshot ()
  VFE_ConfigureOffline ()
  VFE_Update ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  Must be called before sending config commands to VFE.

SCOPE:
  Local 

============================================================================*/
static void vfe_default_color_conversion_thresholds (VFE_ColorConversionType *color_conversion_ptr)
{
  color_conversion_ptr->Ch0_max = 255;
  color_conversion_ptr->Ch1_max = 255;
  color_conversion_ptr->Ch2_max = 255;

  color_conversion_ptr->Ch0_min = 0;
  color_conversion_ptr->Ch1_min = 0;
  color_conversion_ptr->Ch2_min = 0;
} /* vfe_default_color_conversion_thresholds */

/*===========================================================================

FUNCTION:
  VFE_INPUT_YCBCR

DESCRIPTION:
  Utility function to determine if the input format is YCbCr

CALLERS:
  VFE_ConfigureVideo ()
  VFE_ConfigureSnapshot ()
  VFE_ConfigureOffline ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  Must be called before sending config commands to VFE.

SCOPE:
  Local 

============================================================================*/
static boolean vfe_input_ycbcr (CAMIF_InputFormatType input_format)
{
  if ((input_format == CAMIF_YCbCr_Y_Cb_Y_Cr) ||
      (input_format == CAMIF_YCbCr_Y_Cr_Y_Cb) ||
      (input_format == CAMIF_YCbCr_Cb_Y_Cr_Y) ||
      (input_format == CAMIF_YCbCr_Cr_Y_Cb_Y) ||
      (input_format == CAMIF_YCbCr_4_2_2_linepacked) ||
      (input_format == CAMIF_YCbCr_4_2_0_linepacked))
  {
    return TRUE;
  }
  return FALSE;
} /* vfe_input_ycbcr */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  VFE_INIT_COLOR_MATRIX

DESCRIPTION:
  Convert floating color matrix values into int values

CALLERS:
  vfe_start ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local 

============================================================================*/
static void vfe_init_color_matrix(const camsensor_static_params_type *camsensor_params)
{
  vfe_color_matrix_ready = FALSE;

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
  
  /************** color correction matrix *****************/

  if(!vfe_convert_color_correction_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->color_correction),
        &(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL]),
        7)) return;

  if(!vfe_convert_color_correction_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->low_light_color_correction),
        &(vfe_color_correction_matrix[COLOR_CORRECTION_LOW_LIGHT]),
        7)) return;

#endif /* FEATURE_CAMERA_USES_0x203 */

  /************* color conversion matrix ****************/

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
 #else

  if(!vfe_convert_color_conversion_matrix_to_Qfactor(
         &vfe_default_color_conversion_matrix,
        &(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT]),
        10)) return;
  if(!vfe_convert_color_conversion_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->TL84_flourescent_color_conversion),
        &(vfe_color_conversion_matrix[COLOR_CONVERSION_TL84]),
        10)) return;
  if(!vfe_convert_color_conversion_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->incandescent_color_conversion),
        &(vfe_color_conversion_matrix[COLOR_CONVERSION_A]),
        10)) return;
  if(!vfe_convert_color_conversion_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->daylight_color_conversion),
        &(vfe_color_conversion_matrix[COLOR_CONVERSION_D65]),
        10)) return;
  if(!vfe_convert_color_conversion_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->sunset_color_conversion),
        &(vfe_color_conversion_matrix[COLOR_CONVERSION_SUNSET]),
        10)) return;
  if(!vfe_convert_color_conversion_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->skintone_color_conversion),
        &(vfe_color_conversion_matrix[COLOR_CONVERSION_SKINTONE]),
        10)) return;

  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  /************* special effect matrix ****************/

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else 
  if(!vfe_convert_special_effect_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->monochrome_special_effect),
        &(vfe_special_effect[SPECIAL_EFFECT_MONOCHROME]),
        10)) return;
  if(!vfe_convert_special_effect_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->sepia_special_effect),
        &(vfe_special_effect[SPECIAL_EFFECT_SEPIA]),
        10)) return;
  if(!vfe_convert_special_effect_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->negative_special_effect),
        &(vfe_special_effect[SPECIAL_EFFECT_NEGATIVE]),
        10)) return;

  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  /************* white balance matrix ****************/

#if defined (CAMERA_USES_CHROMATIX_0x201) || defined (CAMERA_USES_CHROMATIX_0x203)
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->chromatix_tl84_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_TL84]),
        7)) return;
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->chromatix_incandescent_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_A]),
        7)) return;
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->chromatix_d65_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_CLOUDY]),
        7)) return;
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->chromatix_d50_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_D65]),
        7)) return;
#else
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->TL84_flourescent_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_TL84]),
        7)) return;
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->incandescent_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_A]),
        7)) return;
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->daylight_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_D65]),
        7)) return;
#endif
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->strobe_flash_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_STROBE_FLASH]),
        7)) return;
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->led_flash_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_LED_FLASH]),
        7)) return;
#if !defined( CAMERA_USES_CHROMATIX_0x201 ) && !defined (CAMERA_USES_CHROMATIX_0x203)/* to review this change */
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->spotlight_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_SPOTLIGHT]),
        7)) return;
  if(!vfe_convert_white_balance_matrix_to_Qfactor(
        &(camsensor_params->chromatix_parms->cloudy_white_balance),
        &(vfe_white_balance[WHITE_BALANCE_CLOUDY]),
        7)) return;
#endif
  /* check normal color correction matrix */
  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A11));
  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A12));
  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A13));

  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A21));
  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A22));
  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A23));

  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A31));
  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A32));
  ASSERT(IS_INT12(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A33));

  ASSERT(IS_INT11(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_r));
  ASSERT(IS_INT11(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_g));
  ASSERT(IS_INT11(vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_b));

  /* check default color conversion matrix */
  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A11));
  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A12));
  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A13));

  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A21));
  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A22));
  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A23));

  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A31));
  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A32));
  ASSERT(IS_INT12(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A33));

  ASSERT(IS_INT11(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_y));
  ASSERT(IS_INT11(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cb));
  ASSERT(IS_INT11(vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cr));

  vfe_color_matrix_ready = TRUE;

  snapshot_offset_k0 = 0;
  snapshot_offset_k1 = 0;
  snapshot_offset_k2 = 0;
} /* vfe_init_color_matrix */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  VFE_CONVERT_COLOR_CORRECTION_MATRIX_TO_QFACTOR

DESCRIPTION:
  Converts to the required Q number.

CALLERS:
  vfe_init_color_matrix ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  None

USAGE:

SCOPE:
  Local

============================================================================*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else

static boolean vfe_convert_color_correction_matrix_to_Qfactor (
   camsensor_color_correction_matrix_type * source,
   camera_3x3_rgb_matrix_type * dest,
   int q_factor
)

#endif /* CAMERA_USES_CHROMATIX_0x203 */
{
  if (source == NULL)
  {
    MSG_FATAL("Null pointer: source", 0, 0, 0);
    return FALSE;
  }

  if (dest == NULL)
  {
    MSG_FATAL("Null pointer: dest", 0, 0, 0);
    return FALSE;
  }

  dest->q_factor = q_factor;
  dest->A11 = vfe_convert_float_to_Qfactor( source->a11, q_factor );
  dest->A12 = vfe_convert_float_to_Qfactor( source->a12, q_factor );
  dest->A13 = vfe_convert_float_to_Qfactor( source->a13, q_factor );
  dest->A21 = vfe_convert_float_to_Qfactor( source->a21, q_factor );
  dest->A22 = vfe_convert_float_to_Qfactor( source->a22, q_factor );
  dest->A23 = vfe_convert_float_to_Qfactor( source->a23, q_factor );
  dest->A31 = vfe_convert_float_to_Qfactor( source->a31, q_factor );
  dest->A32 = vfe_convert_float_to_Qfactor( source->a32, q_factor );
  dest->A33 = vfe_convert_float_to_Qfactor( source->a33, q_factor );

  dest->bias_r = source->bias_r;
  dest->bias_g = source->bias_g;
  dest->bias_b = source->bias_b;

  return TRUE;
} /* vfe_convert_color_correction_matrix_to_Qfactor */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  VFE_CONVERT_COLOR_CONVERSION_MATRIX_TO_QFACTOR

DESCRIPTION:
  Converts to the required Q number.

CALLERS:
  vfe_init_color_matrix ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  None

USAGE:

SCOPE:
  Local

============================================================================*/

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
static boolean vfe_convert_color_conversion_matrix_to_Qfactor (
  camsensor_color_conversion_matrix_type * source,
  camera_3x3_ycbcr_matrix_type * dest,
  int q_factor
#endif /* CAMERA_USES_CHROMATIX_0x203 */
)
{
  if (source == NULL)
  {
    MSG_FATAL("Null pointer: source", 0, 0, 0);
    return FALSE;
  }

  if (dest == NULL)
  {
    MSG_FATAL("Null pointer: dest", 0, 0, 0);
    return FALSE;
  }

  dest->q_factor = q_factor;
  dest->A11 = vfe_convert_float_to_Qfactor( source->a11, q_factor );
  dest->A12 = vfe_convert_float_to_Qfactor( source->a12, q_factor );
  dest->A13 = vfe_convert_float_to_Qfactor( source->a13, q_factor );
  dest->A21 = vfe_convert_float_to_Qfactor( source->a21, q_factor );
  dest->A22 = vfe_convert_float_to_Qfactor( source->a22, q_factor );
  dest->A23 = vfe_convert_float_to_Qfactor( source->a23, q_factor );
  dest->A31 = vfe_convert_float_to_Qfactor( source->a31, q_factor );
  dest->A32 = vfe_convert_float_to_Qfactor( source->a32, q_factor );
  dest->A33 = vfe_convert_float_to_Qfactor( source->a33, q_factor );

  dest->bias_y = source->bias_y;
  dest->bias_Cb = source->bias_Cb;
  dest->bias_Cr = source->bias_Cr;

  return TRUE;
} /* vfe_convert_color_conversion_matrix_to_Qfactor */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  VFE_CONVERT_SPECIAL_EFFECT_MATRIX_TO_QFACTOR

DESCRIPTION:
  Converts to the required Q number.

CALLERS:
  vfe_init_color_matrix ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  None

USAGE:

SCOPE:
  Local

============================================================================*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
static boolean vfe_convert_special_effect_matrix_to_Qfactor (
  camsensor_special_effects_matrix_type * source,
  camera_3x3_ycbcr_matrix_type * dest,
  int q_factor
)
#endif /* CAMERA_USES_CHROMATIX_0x203 */


{
  if (source == NULL)
  {
    MSG_FATAL("Null pointer: source", 0, 0, 0);
    return FALSE;
  }

  if (dest == NULL)
  {
    MSG_FATAL("Null pointer: dest", 0, 0, 0);
    return FALSE;
  }

  dest->q_factor = q_factor;
  dest->A11 = vfe_convert_float_to_Qfactor( source->a11, q_factor );
  dest->A12 = vfe_convert_float_to_Qfactor( source->a12, q_factor );
  dest->A13 = vfe_convert_float_to_Qfactor( source->a13, q_factor );
  dest->A21 = vfe_convert_float_to_Qfactor( source->a21, q_factor );
  dest->A22 = vfe_convert_float_to_Qfactor( source->a22, q_factor );
  dest->A23 = vfe_convert_float_to_Qfactor( source->a23, q_factor );
  dest->A31 = vfe_convert_float_to_Qfactor( source->a31, q_factor );
  dest->A32 = vfe_convert_float_to_Qfactor( source->a32, q_factor );
  dest->A33 = vfe_convert_float_to_Qfactor( source->a33, q_factor );

  dest->bias_y = source->bias_y;
  dest->bias_Cb = source->bias_Cb;
  dest->bias_Cr = source->bias_Cr;

  return TRUE;
} /* vfe_convert_special_effect_matrix_to_Qfactor */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  VFE_CONVERT_WHITE_BALANCE_MATRIX_TO_QFACTOR

DESCRIPTION:
  Converts to the required Q number.

CALLERS:
  vfe_init_color_matrix ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  None

USAGE:

SCOPE:
  Local

============================================================================*/
#if defined (CAMERA_USES_CHROMATIX_0x203) || defined (CAMERA_USES_CHROMATIX_0x201)
static boolean vfe_convert_white_balance_matrix_to_Qfactor (
  chromatix_manual_white_balance_type * source,
  camera_1x3_matrix_type * dest,
  int q_factor
)
#else
static boolean vfe_convert_white_balance_matrix_to_Qfactor (
  camsensor_white_balance_matrix_type * source,
  camera_1x3_matrix_type * dest,
  int q_factor
)
#endif
{
  if (source == NULL)
  {
    MSG_FATAL("Null pointer: source", 0, 0, 0);
    return FALSE;
  }

  if (dest == NULL)
  {
    MSG_FATAL("Null pointer: dest", 0, 0, 0);
    return FALSE;
  }

  dest->q_factor = q_factor;
  dest->r_gain = vfe_convert_float_to_Qfactor( source->r_gain, q_factor );
  dest->g_gain = vfe_convert_float_to_Qfactor( source->g_gain, q_factor );
  dest->b_gain = vfe_convert_float_to_Qfactor( source->b_gain, q_factor );

  return TRUE;
} /* vfe_convert_white_balance_matrix_to_Qfactor */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  VFE_UNPACK_COLORCORRECTION

DESCRIPTION:
  unpack VFE_ColorCorrectionType 

CALLERS:
  vfe_apply_digital_gain_for_iso_snapshot ()
  vfe_apply_digital_gain_for_preview ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  None

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e818*/
static camera_3x3_rgb_matrix_type vfe_unpack_ColorCorrection(
  VFE_ColorCorrectionType *packed_cc_ptr)
{
  camera_3x3_rgb_matrix_type unpacked_ColorCorrection;

  /*lint -save -e701 shift left of signed quantity */
  unpacked_ColorCorrection.A11 = (packed_cc_ptr->A11 << 20);
  unpacked_ColorCorrection.A12 = (packed_cc_ptr->A12 << 20);
  unpacked_ColorCorrection.A13 = (packed_cc_ptr->A13 << 20);
  unpacked_ColorCorrection.A21 = (packed_cc_ptr->A21 << 20);
  unpacked_ColorCorrection.A22 = (packed_cc_ptr->A22 << 20);
  unpacked_ColorCorrection.A23 = (packed_cc_ptr->A23 << 20);
  unpacked_ColorCorrection.A31 = (packed_cc_ptr->A31 << 20);
  unpacked_ColorCorrection.A32 = (packed_cc_ptr->A32 << 20);
  unpacked_ColorCorrection.A33 = (packed_cc_ptr->A33 << 20);
  /*lint -restore */

  /*lint -save -e704 shift right of signed quantity */
  unpacked_ColorCorrection.A11 >>= 20;
  unpacked_ColorCorrection.A12 >>= 20;
  unpacked_ColorCorrection.A13 >>= 20;
  unpacked_ColorCorrection.A21 >>= 20;
  unpacked_ColorCorrection.A22 >>= 20;
  unpacked_ColorCorrection.A23 >>= 20;
  unpacked_ColorCorrection.A31 >>= 20;
  unpacked_ColorCorrection.A32 >>= 20;
  unpacked_ColorCorrection.A33 >>= 20;
  /*lint -restore */

  unpacked_ColorCorrection.q_factor = 0;
  unpacked_ColorCorrection.bias_r = 0;
  unpacked_ColorCorrection.bias_g = 0;
  unpacked_ColorCorrection.bias_b = 0;

  return unpacked_ColorCorrection;
}
/*lint -restore*/
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  VFE_INTERPOLATE_GAMMA_TABLE

DESCRIPTION:
  interpolate gamma table based on exposure information 

CALLERS:
  vfe_put_gamma_table ()

RETURN VALUE:
  None

SCOPE:
  Local

============================================================================*/
static void vfe_interpolate_gamma_table(void)
{
  uint32 index;
  camera_exposure_info_type exposure_info;
  float ratio_f;
  uint32 ratio;

  camera_aec_retrive_exposure_info(TRUE, EXPOSURE_PER_GAMMA, &exposure_info);

  /* apply outdoor gamma table */
  if (exposure_info.current_exposure_index <= exposure_info.outdoor_midpoint) 
  {
    memcpy( &vfe_interpolated_gamma_table, &vfe_outdoor_gamma_table, vfe_gamma_table_size );    
  }
  /* interpolate outdoor/default gamma tables */
  else if (exposure_info.current_exposure_index < exposure_info.inoutdoor_midpoint) 
  {
    ratio_f = ((float)(exposure_info.current_exposure_index-exposure_info.outdoor_midpoint)/
               (float)(exposure_info.inoutdoor_midpoint-exposure_info.outdoor_midpoint));
    ratio = vfe_convert_float_to_Qfactor(ratio_f, QFACTOR); /*ratio in Q number */

    for (index = 0; index < vfe_gamma_table_size; index++)
    { 
      vfe_interpolated_gamma_table.arr[index] 
        = (uint8)((vfe_default_gamma_table.arr[index]*ratio + 
                   vfe_outdoor_gamma_table.arr[index]*(QVALUE - ratio) + QROUNDING) >> QFACTOR);
    }
  }
  /* apply default gamma table */
  else if (exposure_info.current_exposure_index <= exposure_info.indoor_midpoint) 
  {
    memcpy( &vfe_interpolated_gamma_table, &vfe_default_gamma_table, vfe_gamma_table_size );    
  }
  /* interpolate default/lowlight gamma tables */
  else 
  {
    ratio_f = ((float)(exposure_info.current_exposure_index - exposure_info.indoor_midpoint)/
               (float)(exposure_info.max_exp_index - exposure_info.indoor_midpoint));
    ratio = vfe_convert_float_to_Qfactor(ratio_f, QFACTOR); /*ratio in Q number */

    for (index = 0; index < vfe_gamma_table_size; index++)
    { 
      vfe_interpolated_gamma_table.arr[index] 
        = (uint8)((vfe_lowlight_gamma_table.arr[index]*ratio + 
                   vfe_default_gamma_table.arr[index]*(QVALUE - ratio) + QROUNDING) >> QFACTOR);
    }
  }
}

/*===========================================================================

FUNCTION:
  VFE_RESET_COLOR_CORRECTION_BIAS_PER_SNAPSHOT

DESCRIPTION:
  set color correction bias to be 0 per snapshot.
  Per snapshot, different gamma is applied so no bias calculation is needed.

CALLERS:
  VFE_ConfigureOffline(), VFE_ConfigureSnapshot()

RETURN VALUE:
  None

SCOPE:
  Local

============================================================================*/
static void vfe_reset_color_correction_bias_per_snapshot(
  VFE_ColorCorrectionType *packed_cc_ptr
  )
{
  camera_3x3_rgb_matrix_type unpacked_CCorrection;

  unpacked_CCorrection = vfe_unpack_ColorCorrection(packed_cc_ptr);

  packed_cc_ptr->k0 = snapshot_offset_k0;
  packed_cc_ptr->k1 = snapshot_offset_k1;
  packed_cc_ptr->k2 = snapshot_offset_k2;
  
  packed_cc_ptr->A11 = unpacked_CCorrection.A11 & 0x0FFF;
  packed_cc_ptr->A12 = unpacked_CCorrection.A12 & 0x0FFF;
  packed_cc_ptr->A13 = unpacked_CCorrection.A13 & 0x0FFF;
  packed_cc_ptr->A21 = unpacked_CCorrection.A21 & 0x0FFF;
  packed_cc_ptr->A22 = unpacked_CCorrection.A22 & 0x0FFF;
  packed_cc_ptr->A23 = unpacked_CCorrection.A23 & 0x0FFF;
  packed_cc_ptr->A31 = unpacked_CCorrection.A31 & 0x0FFF;
  packed_cc_ptr->A32 = unpacked_CCorrection.A32 & 0x0FFF;
  packed_cc_ptr->A33 = unpacked_CCorrection.A33 & 0x0FFF;
}

#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION      vfe_convert_float_to_Qfactor

DESCRIPTION
              Converts a floating point number into an integer number
              with a particular Q factor desired

DEPENDENCIES
  None

INPUT
  Float input
  Qfactor of desired output (i.e. number of places to give fractional
    portion)

RETURN VALUE
  int32 converted number

SIDE EFFECTS
  None

===========================================================================*/
int32 vfe_convert_float_to_Qfactor(float dfloat, int32 qfactor)
{
  float rounding_factor = 0.5;
  return (int)(dfloat * (float)(1 << qfactor) + rounding_factor); 
} /* vfe_convert_float_to_Qfactor */

/*===========================================================================

FUNCTION      VFE_ADJUST_CONFIG_SIZE

DESCRIPTION
              Adjust configuration table size based on hardware features.

DEPENDENCIES
  None

RETURN VALUE
  Adjusted table size.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 vfe_adjust_config_size(uint32 size)
{
 #ifndef QDSP_MODULE_VFE25_DEFINED  
  if ((vfe_hw_features.filter_3x3_asf == FALSE))
  {
    size -= sizeof(VFE_AsfCfgType) / 4;
  }
 #endif /* nQDSP_MODULE_VFE25_DEFINED */
  return size;
}

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_SELECT_ROLLOFF

DESCRIPTION
              See if rolloff is enabled. If so, select the correct rolloff
              table and rolloff config based on light condition and current
              resolution.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  vfe_rolloff_cfg modified.
  vfe_current_rolloff_table modified.

===========================================================================*/
void vfe_select_rolloff
(
  camsensor_sensor_resolution_type resolution,
  boolean                          preview
)
{

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
  camsensor_chromatix_rolloff_light_type rolloff_light;
#endif

  if (vfe_sw_features.rolloff_correction != VFE_ROLLOFF_CORRECTION)
  {
    vfe_current_rolloff_cfg = vfe_rolloff_cfg_TL84_F;
    vfe_current_rolloff_cfg.enable = FALSE;
    return;
  }

  vfe_current_rolloff_cfg.enable = TRUE;
  if (preview)
  {
    /* Always use TL84 */
    if (resolution == CAMSENSOR_FULL_SIZE)
    {
      vfe_current_rolloff_table = &vfe_rolloff_table_TL84_F;
      vfe_current_rolloff_cfg = vfe_rolloff_cfg_TL84_F;
      vfe_current_rolloff_cfg.enable = TRUE;
    }
    else
    {
      vfe_current_rolloff_table = &vfe_rolloff_table_TL84_Q;
      vfe_current_rolloff_cfg = vfe_rolloff_cfg_TL84_Q;
      vfe_current_rolloff_cfg.enable = TRUE;
    }
  }
  else
  {
    rolloff_light = camera_3a_select_rolloff_light();

    if (resolution == CAMSENSOR_FULL_SIZE)
    {
      if (rolloff_light == ROLLOFF_A_LIGHT)
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_A_F;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_A_F;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
      else if (rolloff_light == ROLLOFF_D65_LIGHT)
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_D65_F;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_D65_F;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
      else if (rolloff_light == ROLLOFF_TL84_LIGHT)
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_TL84_F;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_TL84_F;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
      else /* (rolloff_light == ROLLOFF_LOW_LIGHT) */
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_low_light_F;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_low_light_F;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
    }
    else
    {
      if (rolloff_light == ROLLOFF_A_LIGHT)
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_A_Q;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_A_Q;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
      else if (rolloff_light == ROLLOFF_D65_LIGHT)
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_D65_Q;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_D65_Q;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
      else if (rolloff_light == ROLLOFF_TL84_LIGHT)
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_TL84_Q;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_TL84_Q;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
      else /* (rolloff_light == ROLLOFF_LOW_LIGHT) */
      {
        vfe_current_rolloff_table = &vfe_rolloff_table_low_light_Q;
        vfe_current_rolloff_cfg = vfe_rolloff_cfg_low_light_Q;
        vfe_current_rolloff_cfg.enable = TRUE;
      }
    }
  }
} /* vfe_select_rolloff */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifndef QDSP_MODULE_VFE25_DEFINED
/*===========================================================================

FUNCTION      VFE_UPDATE_ROLLOFF_CFG

DESCRIPTION
              Update rolloff configuration variable based on the clipping
              window.

DEPENDENCIES
  None

RETURN VALUE
  Updated configuration.

SIDE EFFECTS
  None

===========================================================================*/
VFE_RolloffCfgType vfe_update_rolloff_cfg
(
  uint32 first_pixel,
  uint32 last_pixel,
  uint32 first_line,
  uint32 last_line
)
{
  static uint32 my_first_pixel = 100000, my_last_pixel = 100000;
  static uint32 my_first_line = 100000, my_last_line = 100000;
  static VFE_RolloffCfgType rolloff_cfg;
  int32 cx, cy, dx, dy;

  if (my_first_pixel == first_pixel && my_last_pixel == last_pixel &&
      my_first_line == first_line && my_last_line == last_line)
  {
    /*lint -save -e727 rollof_cfg is static, value already set */
    return rolloff_cfg;
    /*lint -restore */
  }
  my_first_pixel = first_pixel;
  my_last_pixel = last_pixel;
  my_first_line = first_line;
  my_last_line = last_line;
  rolloff_cfg.reserved1 = 0;
  rolloff_cfg.reserved2 = 0;
  rolloff_cfg.enable = TRUE;
  cx = (int)vfe_current_rolloff_cfg.cx - (int)first_pixel + 6;
  dx = (int)last_pixel - (int)first_pixel + 12;
  cy = (int)vfe_current_rolloff_cfg.cy - (int)first_line + 3;
  dy = (int)last_line - (int)first_line + 6;
  if (cx < 0)
  {
    rolloff_cfg.cx = 0;
  }
  else if (cx > dx)
  {
    rolloff_cfg.cx = (uint32)dx;
  }
  else
  {
    rolloff_cfg.cx = (uint32)cx;
  }
  if (cy < 0)
  {
    rolloff_cfg.cy = 0;
  }
  else if (cy > dy)
  {
    rolloff_cfg.cy = (uint32)dy;
  }
  else
  {
    rolloff_cfg.cy = (uint32)cy;
  }
  return rolloff_cfg;

} /* vfe_update_rolloff_cfg */
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION      VFE_COPY_SENSOR_SNAPSHOT_CONFIG

DESCRIPTION
              Copy the sensor config to VFE config for snapshot mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean vfe_copy_sensor_snapshot_config
(
  /* VFE output dimenstion along output1 path. 
   * Output1 is used for preview and thumbnail
   */
  uint16                  output1_width,
  uint16                  output1_height,
  /* VFE output dimenstion along output2 path.
   * Output2 is used for main image
   */
  uint16                  output2_width,
  uint16                  output2_height,
  const camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
  uint16 window_width, window_height;
#ifndef FEATURE_CAMERA_YCBCR_ONLY 
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY  */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
  {
    if(!vfe_color_matrix_ready)
    {
      MSG_ERROR ("color matrix is not available", 0, 0, 0);
      return FALSE;
    }
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

  /* Items copied from the device driver */

   /* 1. CAMIF configs */
  vfeSnapshotConfig.camif_config = camsensor_params->camif_config;
  vfeSnapshotConfig.camif_efs_config = camsensor_params->camif_efs_config;
  vfeSnapshotConfig.camif_frame_config = camsensor_params->camif_frame_config;
  vfeSnapshotConfig.camif_window_width_config = camsensor_params->camif_window_width_config;
  vfeSnapshotConfig.camif_window_height_config = camsensor_params->camif_window_height_config;

  /* 2. VFE input format and size */
  vfeSnapshotConfig.inputConfiguration.format = camsensor_params->format;
  vfeSnapshotConfig.inputConfiguration.width   = camsensor_params->camsensor_width;
  vfeSnapshotConfig.inputConfiguration.height  = camsensor_params->camsensor_height;
  vfeSnapshotConfig.inputConfiguration.numContinuousShotsMinus1    = 0;
#ifdef FEATURE_CAMERA_YCBCR_ONLY
  vfeSnapshotConfig.gammaCorrection.enable       = VFE_Disable;
  vfeSnapshotConfig.defectPixelCorrection.enable = VFE_Disable;
  vfeSnapshotConfig.colorConversion.enable       = VFE_Disable;
  vfeSnapshotConfig.colorCorrection.enable       = VFE_Disable;
  vfeSnapshotConfig.blackCorrection.enable       = VFE_Disable;
#ifndef QDSP_MODULE_VFE25_DEFINED
  vfeSnapshotConfig.f3x3LumaFilter.enable        = VFE_Disable;
#endif /* nQDSP_MODULE_VFE25_DEFINED */
  vfeSnapshotConfig.asf_cfg.enable               = VFE_Disable;
  /*vfeSnapshotConfig.blackCorrection.enable       = VFE_Disable;*/
#else
  /* 3. Gamma correction */
  if (vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)) == TRUE)
  {
    vfeSnapshotConfig.gammaCorrection.enable       = VFE_Disable;
  }
  else
  {
    vfeSnapshotConfig.gammaCorrection.enable       = (VFE_EnableType)(camsensor_params->gammaCorrection);
  }

  /* 4. Defect Pixel correction */
  if (vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)) == TRUE)
  {
    vfeSnapshotConfig.defectPixelCorrection.enable = VFE_Disable;
  }
  else
  {
    vfeSnapshotConfig.defectPixelCorrection.enable = VFE_Enable;
  #if defined (CAMERA_USES_CHROMATIX_0x201) || defined (CAMERA_USES_CHROMATIX_0x203)
    ASSERT(IS_UINT7(camsensor_params->chromatix_parms->bpc_Fmin));   
    vfeSnapshotConfig.defectPixelCorrection.minThreshold = camsensor_params->chromatix_parms->bpc_Fmin;
    ASSERT(IS_UINT7(camsensor_params->chromatix_parms->bpc_Fmax));
    vfeSnapshotConfig.defectPixelCorrection.maxThreshold = camsensor_params->chromatix_parms->bpc_Fmax;
  #else
    ASSERT(IS_UINT7(camsensor_params->defectPixelCorrection.minThreshold));
    vfeSnapshotConfig.defectPixelCorrection.minThreshold = camsensor_params->defectPixelCorrection.minThreshold;
    ASSERT(IS_UINT7(camsensor_params->defectPixelCorrection.maxThreshold));
    vfeSnapshotConfig.defectPixelCorrection.maxThreshold = camsensor_params->defectPixelCorrection.maxThreshold;
  #endif /* CAMERA_USES_CHROMATIX_0x201 */
 
  #ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
  #endif /* QDSP_MODULE_VFE25_DEFINED */
  }

  /* 5. Color conversion parameters */
  if (vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)) == TRUE)
  {
    vfeSnapshotConfig.colorConversion.enable       = VFE_Disable;
  }
  else
  {
    vfeSnapshotConfig.colorConversion.enable = (VFE_EnableType)(camsensor_params->colorConversion.enable);
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch0_max));
    vfeSnapshotConfig.colorConversion.Ch0_max = camsensor_params->colorConversion.Ch0_max;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch1_max));
    vfeSnapshotConfig.colorConversion.Ch1_max = camsensor_params->colorConversion.Ch1_max;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch2_max));
    vfeSnapshotConfig.colorConversion.Ch2_max = camsensor_params->colorConversion.Ch2_max;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch0_min));
    vfeSnapshotConfig.colorConversion.Ch0_min = camsensor_params->colorConversion.Ch0_min;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch1_min));
    vfeSnapshotConfig.colorConversion.Ch1_min = camsensor_params->colorConversion.Ch1_min;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch2_min));
    vfeSnapshotConfig.colorConversion.Ch2_min = camsensor_params->colorConversion.Ch2_min;
    /* VFE deals with GBR. The color conversion table is in RGB,
      So RGB -> GBR == 123 -> 231
      A11 A12 A13    <-   A22 A23 A21
      A21 A22 A23    <-   A32 A33 A31
      A31 A32 A33    <-   A12 A13 A11
      */
    vfeSnapshotConfig.colorConversion.A11 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A12;
    vfeSnapshotConfig.colorConversion.A12 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A13;
    vfeSnapshotConfig.colorConversion.A13 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A11;

    vfeSnapshotConfig.colorConversion.A21 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A22;
    vfeSnapshotConfig.colorConversion.A22 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A23;
    vfeSnapshotConfig.colorConversion.A23 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A21;

    vfeSnapshotConfig.colorConversion.A31 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A32;
    vfeSnapshotConfig.colorConversion.A32 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A33;
    vfeSnapshotConfig.colorConversion.A33 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A31;

    vfeSnapshotConfig.colorConversion.k0  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_y;
    vfeSnapshotConfig.colorConversion.k1  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cb;
    vfeSnapshotConfig.colorConversion.k2  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cr;

    if (vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].q_factor == 10)
    {
       vfeSnapshotConfig.colorConversion.coefficient_Q_Factor = VFE_SIGNED_Q10;
    }
    else
    {
       return FALSE;
    }
  }

  /* 6. Color Correction Parameters */
  if (vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)) == TRUE)
  {
    vfeSnapshotConfig.colorCorrection.enable       = VFE_Disable;
  }
  else
  {
    vfeSnapshotConfig.colorCorrection.enable = (VFE_EnableType)(camsensor_params->colorCorrection.enable);
    ASSERT(IS_UINT10(camsensor_params->colorCorrection.Ch0_max));
    vfeSnapshotConfig.colorCorrection.Ch0_max = camsensor_params->colorCorrection.Ch0_max;
    ASSERT(IS_UINT10(camsensor_params->colorCorrection.Ch1_max));
    vfeSnapshotConfig.colorCorrection.Ch1_max = camsensor_params->colorCorrection.Ch1_max;
    ASSERT(IS_UINT10(camsensor_params->colorCorrection.Ch2_max));
    vfeSnapshotConfig.colorCorrection.Ch2_max = camsensor_params->colorCorrection.Ch2_max;
    ASSERT(IS_UINT10(camsensor_params->colorCorrection.Ch0_min));
    vfeSnapshotConfig.colorCorrection.Ch0_min = camsensor_params->colorCorrection.Ch0_min;
    ASSERT(IS_UINT10(camsensor_params->colorCorrection.Ch1_min));
    vfeSnapshotConfig.colorCorrection.Ch1_min = camsensor_params->colorCorrection.Ch1_min;
    ASSERT(IS_UINT10(camsensor_params->colorCorrection.Ch2_min));
    vfeSnapshotConfig.colorCorrection.Ch2_min = camsensor_params->colorCorrection.Ch2_min;
    /* Also note that Chromatix data is RGB, while the VFE uses GBR
     So RGB -> GBR == 123 -> 231
     A11 A12 A13    <-   A22 A23 A21
     A21 A22 A23    <-   A32 A33 A31
     A31 A32 A33    <-   A12 A13 A11
     */
    vfeSnapshotConfig.colorCorrection.A11 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A22;
    vfeSnapshotConfig.colorCorrection.A12 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A23;
    vfeSnapshotConfig.colorCorrection.A13 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A21;

    vfeSnapshotConfig.colorCorrection.A21 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A32;
    vfeSnapshotConfig.colorCorrection.A22 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A33;
    vfeSnapshotConfig.colorCorrection.A23 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A31;

    vfeSnapshotConfig.colorCorrection.A31 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A12;
    vfeSnapshotConfig.colorCorrection.A32 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A13;
    vfeSnapshotConfig.colorCorrection.A33 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A11;

    vfeSnapshotConfig.colorCorrection.k0  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_g;
    vfeSnapshotConfig.colorCorrection.k1  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_b;
    vfeSnapshotConfig.colorCorrection.k2  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_r;

    if (vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].q_factor == 7)
    {
       vfeSnapshotConfig.colorCorrection.coefficient_Q_Factor = VFE_SIGNED_Q7;
    }
    else
    {
       return FALSE;
    }
  }

  /* 7. Luma/ASF Filter Parameters */
 
  if(vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)) == TRUE)
  {
 #ifndef QDSP_MODULE_VFE25_DEFINED
    vfeSnapshotConfig.f3x3LumaFilter.enable = VFE_Disable;
    vfeSnapshotConfig.asf_cfg.enable = VFE_Disable;
 #endif /* nQDSP_MODULE_VFE25_DEFINED */
  }
 #ifndef QDSP_MODULE_VFE25_DEFINED
  else if (vfe_hw_features.filter_3x3_asf == TRUE && 
      vfe_sw_features.enable_hw_filter_3x3_asf == TRUE)
  {
    /* Use the 3x3 Hardware ASF */
    vfeSnapshotConfig.asf_cfg = vfe_asf_cfg;
  }

 #else /* nQDSP_MODULE_VFE25_DEFINED */
#error code not present
#endif /* nQDSP_MODULE_VFE25_DEFINED */ 


  
  if(!vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)))
  {
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
    if (vfe_hw_features.filter_3x3_asf == TRUE &&
           vfe_sw_features.enable_hw_filter_3x3_asf == TRUE)
    {
    /* Use the 3x3 Hardware ASF */
    vfeSnapshotConfig.asf_cfg = vfe_asf_cfg;
    }
#endif /* nQDSP_MODULE_VFE25_DEFINED */
  }
  else
  {
#ifndef QDSP_MODULE_VFE25_DEFINED
    vfeSnapshotConfig.f3x3LumaFilter.enable = VFE_Disable;
    vfeSnapshotConfig.asf_cfg.enable = VFE_Disable;
#endif /* nQDSP_MODULE_VFE25_DEFINED */
  }

  /* luma filter will be configured later */
 #ifndef QDSP_MODULE_VFE25_DEFINED
  if (vfe_hw_features.filter_3x3_asf == FALSE)
  {
    vfeSnapshotConfig.asf_cfg.enable = VFE_Disable;
  }

  if (vfe_hw_features.filter_3x3_luma == FALSE)
  {
    vfeSnapshotConfig.f3x3LumaFilter.enable = VFE_Disable;
  }
 #endif /* nQDSP_MODULE_VFE25_DEFINED */

  /* 8. Black level correction */
  if (vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)) == TRUE)
  {
    vfeSnapshotConfig.blackCorrection.enable      = VFE_Disable;
  }
  else
  {
    vfeSnapshotConfig.blackCorrection.enable = (VFE_EnableType)(camsensor_params->blackCorrection.enable);
 #ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
 #else
    vfeSnapshotConfig.blackCorrection.lastPixel   = camsensor_params->blackCorrection.lastPixel;
    vfeSnapshotConfig.blackCorrection.firstPixel  = camsensor_params->blackCorrection.firstPixel;
    vfeSnapshotConfig.blackCorrection.lastLine    = camsensor_params->blackCorrection.lastLine;
    vfeSnapshotConfig.blackCorrection.firstLine   = camsensor_params->blackCorrection.firstLine;
    vfeSnapshotConfig.blackCorrection.shift       = camsensor_params->blackCorrection.shift;
 #endif /* QDSP_MODULE_VFE25_DEFINED */
  }

#endif /* FEATURE_CAMERA_YCBCR_ONLY */
  
  /* 9. Epoch line */
  vfeSnapshotConfig.camif_epoch_intr.epoch_line = camsensor_params->epoch_line;

  /* Above items are copied from the device driver */

  vfeSnapshotConfig.camif_config.MISR_En          = FALSE;
#ifdef CAMERA_NO_MDDI
  vfeSnapshotConfig.camif_config.VSYNCSampleEdge  = 0;
  vfeSnapshotConfig.camif_config.HSYNCSampleEdge  = 0;
  vfeSnapshotConfig.camif_config.DataSampleEdge   = 0;
#endif /* CAMERA_NO_MDDI */
  vfeSnapshotConfig.camif_config.INTR_SubsampleEn = FALSE;
  vfeSnapshotConfig.camif_config.AHB_OutputEn     = FALSE;
#ifndef CAMERA_NO_MDDI
  vfeSnapshotConfig.camif_config.DMA_OutputEn     = FALSE;
#endif /* nCAMERA_NO_MDDI */

  /* Set to TRUE for real-time snapshot mode. Set to FALSE for
   * Off-line snapshot mode.
   */
  vfeSnapshotConfig.camif_config.VFE_OutputEn     = TRUE;
  
  /* Set to FALSE for real-time snapshot mode. Set to TRUE for
   * Off-line snapshot mode.
   */
  vfeSnapshotConfig.camif_config.AHB_SubsampleEn  = FALSE;
#ifndef CAMERA_NO_MDDI
  vfeSnapshotConfig.camif_config.DMA_SubsampleEn  = FALSE;
#endif /* nCAMERA_NO_MDDI */

  vfeSnapshotConfig.camif_config.VFE_SubsampleEn      = FALSE;
  vfeSnapshotConfig.camif_subsample1_config.pixelSkip = 0xFFFF;
  vfeSnapshotConfig.camif_subsample1_config.lineSkip  = 0xFFFF;
  vfeSnapshotConfig.camif_subsample2_config.frameSkip = CAMIF_ALL_FRAMES0;

  /* Number of fragments */
  vfeSnapshotConfig.output2Configuration.fragmentsMinus1 = VFE_IMAGE_FRAGMENTS - 1;

  /* VFE clipping */
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  if (vfe_input_ycbcr((CAMIF_InputFormatType)(vfeSnapshotConfig.inputConfiguration.format)) == FALSE )
  {
    vfeSnapshotConfig.clip.firstPixel = 6;
    vfeSnapshotConfig.clip.lastPixel = vfeSnapshotConfig.inputConfiguration.width - 6 - 1;
    vfeSnapshotConfig.clip.lastPixel -= (vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel + 1) % 8;

    vfeSnapshotConfig.clip.firstLine = 3 + 2 + camsensor_params->blackrows;
    vfeSnapshotConfig.clip.lastLine = vfeSnapshotConfig.inputConfiguration.height - 3 - 1;
    vfeSnapshotConfig.clip.lastLine -= (vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine + 1) % 8;
  }
  else
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
  {
    vfeSnapshotConfig.clip.firstPixel = 0;
    vfeSnapshotConfig.clip.lastPixel = vfeSnapshotConfig.inputConfiguration.width - 1;

    vfeSnapshotConfig.clip.firstLine = 0;
    vfeSnapshotConfig.clip.lastLine = vfeSnapshotConfig.inputConfiguration.height - 1;
  }

  /* Output1 dimensions */
  if (output1_width > (vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel + 1)) 
  {
    output1_width = (vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel + 1);
  }

  if (output1_height > (vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine + 1))  
  {
    output1_height = (vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine + 1);
  }

  vfeSnapshotConfig.output1Configuration.width  = output1_width;
  vfeSnapshotConfig.output1Configuration.height = output1_height;

  /* Output2 dimensions */ 
  window_width = vfeSnapshotConfig.clip.lastPixel - vfeSnapshotConfig.clip.firstPixel + 1;
  if (output2_width > window_width)
  {
    output2_width = window_width;
  }

  window_height = vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine + 1;
  if (output2_height > window_height)
  {
    output2_height = (vfeSnapshotConfig.clip.lastLine - vfeSnapshotConfig.clip.firstLine + 1);
  }

  vfeSnapshotConfig.output2Configuration.height = output2_height;
  vfeSnapshotConfig.output2Configuration.width  = output2_width;

  /* Enable sub-sampling if resizing factor is more than 16 */
  if ((vfeSnapshotConfig.inputConfiguration.width / output1_width) >= 16) {
    vfeSnapshotConfig.inputConfiguration.width /= 2;
    vfeSnapshotConfig.camif_subsample1_config.pixelSkip =  0xF0F0;
    vfeSnapshotConfig.camif_config.VFE_SubsampleEn  = TRUE; 
  }

  if ((vfeSnapshotConfig.inputConfiguration.height / output1_height) >= 16) {
    vfeSnapshotConfig.inputConfiguration.height /= 2;
    vfeSnapshotConfig.camif_subsample1_config.lineSkip = 0xF0F0;
    vfeSnapshotConfig.camif_config.VFE_SubsampleEn  = TRUE; 
  }

  return TRUE;
} /* vfe_copy_sensor_snapshot_config */

/*===========================================================================

FUNCTION      VFE_COPY_SENSOR_RAW_SNAPSHOT_CONFIG

DESCRIPTION
              Copy the sensor config to VFE config for raw snapshot mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

static boolean vfe_copy_sensor_raw_snapshot_config
(
  const camsensor_static_params_type *camsensor_params /* Other config params */
) 
{
  /* Items copied from the camsensor device driver */

  /* 1. CAMIF parameters */
  vfeRawConfig.camif_config = camsensor_params->camif_config;
  vfeRawConfig.camif_efs_config = camsensor_params->camif_efs_config;
  vfeRawConfig.camif_frame_config = camsensor_params->camif_frame_config;
  vfeRawConfig.camif_window_width_config = camsensor_params->camif_window_width_config;
  vfeRawConfig.camif_window_height_config = camsensor_params->camif_window_height_config;

  /* 7. Epoch line */
  vfeRawConfig.camif_epoch_intr.epoch_line = camsensor_params->epoch_line;

  /* The above items are copied from the device driver */

  /* 8 bit, 10 bit, ... */
  if (camsensor_params->raw_output_option == CAMSENSOR_8_BIT_DIRECT)
  {
    vfeRawConfig.rawOutputOption = VFE_8_BIT_DIRECT;
  }
  else if (camsensor_params->raw_output_option == CAMSENSOR_10_BIT_DIRECT)
  {
    vfeRawConfig.rawOutputOption = VFE_10_BIT_DIRECT;
  }
  
  vfeRawConfig.numContinuousShotsMinus1    = 0;
  vfeRawConfig.numOutputFragsMinus1  = 0;
  vfeRawConfig.camif_config.MISR_En          = FALSE;
#ifdef CAMERA_NO_MDDI
  vfeRawConfig.camif_config.VSYNCSampleEdge  = 0;
  vfeRawConfig.camif_config.HSYNCSampleEdge  = 0;
  vfeRawConfig.camif_config.DataSampleEdge   = 0;
#endif /* CAMERA_NO_MDDI */
  vfeRawConfig.camif_config.INTR_SubsampleEn = FALSE;
  vfeRawConfig.camif_config.AHB_OutputEn     = TRUE;
#ifndef CAMERA_NO_MDDI
  vfeRawConfig.camif_config.DMA_OutputEn     = FALSE;
#endif /* nCAMERA_NO_MDDI */
  vfeRawConfig.camif_config.VFE_OutputEn     = FALSE;

  vfeRawConfig.camif_config.AHB_SubsampleEn      = FALSE;
#ifndef CAMERA_NO_MDDI
  vfeRawConfig.camif_config.DMA_SubsampleEn      = FALSE;
#endif /* nCAMERA_NO_MDDI */
  vfeRawConfig.camif_config.VFE_SubsampleEn      = FALSE;
  vfeRawConfig.camif_subsample1_config.pixelSkip = 0xFFFF;
  vfeRawConfig.camif_subsample1_config.lineSkip  = 0xFFFF;
  vfeRawConfig.camif_subsample2_config.frameSkip = CAMIF_ALL_FRAMES0;

  return TRUE;
} /* vfe_copy_sensor_raw_snapshot_config */


#ifdef FEATURE_CAMERA_YCBCR_ONLY
#define vfe_set_aec_awb_num_regions(window_width, window_height, camsensor_params)
#else
/*===========================================================================

FUNCTION      VFE_UPDATE_AEC_AWB_REGION_SETTING

DESCRIPTION
              Copy the sensor config to VFE config for video mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

static void vfe_set_aec_awb_num_regions
(
 uint32 window_width,
 uint32 window_height,
 const camsensor_static_params_type *camsensor_params
)
{
  uint16 old_val, new_val;

  old_val = vfeExpWbStatDspInput.numberOfRegions;
  if ( (window_width < 136) || (window_height < 136) )
  {
    new_val = VFE_4x4;
  }
#ifdef FEATURE_CAMERA_VFE_16X16_SUPPORT
  else if ( (window_width > 272) && (window_height > 272) )
  {
    new_val = VFE_16x16;
  }
#endif /* FEATURE_CAMERA_VFE_16X16_SUPPORT */
  else 
  {
    new_val = VFE_8x8;
  }
  if (old_val != new_val)
  {
    vfeExpWbStatDspInput.numberOfRegions = (VFE_NumberOfRegionsType)new_val;
    (void)camera_aec_set_bias_table(camsensor_params);
  }
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION      VFE_COPY_SENSOR_VIDEO_CONFIG

DESCRIPTION
              Copy the sensor config to VFE config for video mode

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

static boolean vfe_copy_sensor_video_config
(
  /* VFE output dimenstion along output1 path. 
   * Output1 is used for preview and thumbnail
   */
  uint16                  output1_width,
  uint16                  output1_height,
  const camsensor_static_params_type *camsensor_params /* Other config params */
)
{
  uint32 window_width, window_height;
#ifndef FEATURE_CAMERA_YCBCR_ONLY 
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY  */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
  {
    if(!vfe_color_matrix_ready)
    {
      MSG_ERROR ("color matrix is not available", 0, 0, 0);
      return FALSE;
    }
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

  /* Items copied from the camsensor device driver */

  /* 1. CAMIF configs */
  vfeVideoConfig.camif_config = camsensor_params->camif_config;
  vfeVideoConfig.camif_efs_config = camsensor_params->camif_efs_config;
  vfeVideoConfig.camif_frame_config = camsensor_params->camif_frame_config;
  vfeVideoConfig.camif_window_width_config = camsensor_params->camif_window_width_config;
  vfeVideoConfig.camif_window_height_config = camsensor_params->camif_window_height_config;

  /* 2. VFE input format and size */
  vfeVideoConfig.inputConfiguration.format = camsensor_params->format;
  vfeVideoConfig.inputConfiguration.width   = camsensor_params->camsensor_width;
  vfeVideoConfig.inputConfiguration.height  = camsensor_params->camsensor_height;

#ifdef FEATURE_CAMERA_YCBCR_ONLY
    vfeVideoConfig.gammaCorrection.enable       = VFE_Disable;
    vfeVideoConfig.defectPixelCorrection.enable = VFE_Disable;
    vfeVideoConfig.colorConversion.enable       = VFE_Disable;
    vfeVideoConfig.colorCorrection.enable       = VFE_Disable;
    vfeVideoConfig.blackCorrection.enable       = VFE_Disable;
#ifndef QDSP_MODULE_VFE25_DEFINED
    vfeVideoConfig.f3x3LumaFilter.enable        = VFE_Disable;
#endif /* QDSP_MODULE_VFE25_DEFINED */
    vfeVideoConfig.asf_cfg.enable               = VFE_Disable;
/*    vfeVideoConfig.blackCorrection.enable       = VFE_Disable;*/
#else
  /* 3. Gamma correction */
  if (vfe_input_ycbcr(vfeVideoConfig.inputConfiguration.format) == TRUE)
  {
    vfeVideoConfig.gammaCorrection.enable       = VFE_Disable;
  }
  else
  {
    vfeVideoConfig.gammaCorrection.enable       = (VFE_EnableType)(camsensor_params->gammaCorrection);
  }

  /* 4. Defect Pixel Correction parameters */
  if (vfe_input_ycbcr(vfeVideoConfig.inputConfiguration.format) == TRUE)
  {
    vfeVideoConfig.defectPixelCorrection.enable = VFE_Disable;
  }
  else
  {
#if defined (CAMERA_USES_CHROMATIX_0x201)|| defined (CAMERA_USES_CHROMATIX_0x203)
    vfeVideoConfig.defectPixelCorrection.enable = VFE_Enable;
    ASSERT(IS_UINT7(camsensor_params->chromatix_parms->bpc_Fmin));
    vfeVideoConfig.defectPixelCorrection.minThreshold = camsensor_params->chromatix_parms->bpc_Fmin;
    ASSERT(IS_UINT7(camsensor_params->chromatix_parms->bpc_Fmax));
    vfeVideoConfig.defectPixelCorrection.maxThreshold = camsensor_params->chromatix_parms->bpc_Fmax;
#else
    vfeVideoConfig.defectPixelCorrection.enable = (VFE_EnableType)(camsensor_params->defectPixelCorrection.enable);
    ASSERT(IS_UINT7(camsensor_params->defectPixelCorrection.minThreshold));
    vfeVideoConfig.defectPixelCorrection.minThreshold = camsensor_params->defectPixelCorrection.minThreshold;
    ASSERT(IS_UINT7(camsensor_params->defectPixelCorrection.maxThreshold));
    vfeVideoConfig.defectPixelCorrection.maxThreshold = camsensor_params->defectPixelCorrection.maxThreshold;
#endif /* CAMERA_USES_CHROMATIX_0x201 */

#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
  
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
  }
  /* 5. Color conversion parameters */
  if (vfe_input_ycbcr(vfeVideoConfig.inputConfiguration.format) == TRUE)
  {
    vfeVideoConfig.colorConversion.enable       = VFE_Disable;
  }
  else
  {
    vfeVideoConfig.colorConversion.enable       = (VFE_EnableType)(camsensor_params->colorConversion.enable);
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch0_max));
    vfeVideoConfig.colorConversion.Ch0_max = camsensor_params->colorConversion.Ch0_max;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch1_max));
    vfeVideoConfig.colorConversion.Ch1_max = camsensor_params->colorConversion.Ch1_max;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch2_max));
    vfeVideoConfig.colorConversion.Ch2_max = camsensor_params->colorConversion.Ch2_max;
   
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch0_min));
    vfeVideoConfig.colorConversion.Ch0_min = camsensor_params->colorConversion.Ch0_min;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch1_min));
    vfeVideoConfig.colorConversion.Ch1_min = camsensor_params->colorConversion.Ch1_min;
    ASSERT(IS_UINT8(camsensor_params->colorConversion.Ch2_min));
    vfeVideoConfig.colorConversion.Ch2_min = camsensor_params->colorConversion.Ch2_min;
    /* VFE deals with GBR. The color conversion table is in RGB,
     So RGB -> GBR == 123 -> 231
     A11 A12 A13    <-   A22 A23 A21
     A21 A22 A23    <-   A32 A33 A31
     A31 A32 A33    <-   A12 A13 A11
     */
    vfeVideoConfig.colorConversion.A11 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A12;
    vfeVideoConfig.colorConversion.A12 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A13;
    vfeVideoConfig.colorConversion.A13 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A11;

    vfeVideoConfig.colorConversion.A21 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A22;
    vfeVideoConfig.colorConversion.A22 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A23;
    vfeVideoConfig.colorConversion.A23 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A21;

    vfeVideoConfig.colorConversion.A31 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A32;
    vfeVideoConfig.colorConversion.A32 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A33;
    vfeVideoConfig.colorConversion.A33 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A31;

    vfeVideoConfig.colorConversion.k0  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_y;
    vfeVideoConfig.colorConversion.k1  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cb;
    vfeVideoConfig.colorConversion.k2  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cr;

    if (vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].q_factor == 10)
    {
       vfeVideoConfig.colorConversion.coefficient_Q_Factor = VFE_SIGNED_Q10;
    }
    else
    {
       return FALSE;
    }
  }

  /* 6. Color Correction Parameters */
  if (vfe_input_ycbcr(vfeVideoConfig.inputConfiguration.format) == TRUE)
  {
    vfeVideoConfig.colorCorrection.enable       = VFE_Disable;
  }
  else
  {
    /* Only enable if/when AWB is run, but place
       sensor specific tuning params here */
    /* Enable max range (10 bit number) */
    vfeVideoConfig.colorCorrection.enable       = (VFE_EnableType)(camsensor_params->colorCorrection.enable);
    vfeVideoConfig.colorCorrection.A11 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A22;
    vfeVideoConfig.colorCorrection.A12 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A23;
    vfeVideoConfig.colorCorrection.A13 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A21;

    vfeVideoConfig.colorCorrection.A21 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A32;
    vfeVideoConfig.colorCorrection.A22 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A33;
    vfeVideoConfig.colorCorrection.A23 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A31;

    vfeVideoConfig.colorCorrection.A31 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A12;
    vfeVideoConfig.colorCorrection.A32 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A13;
    vfeVideoConfig.colorCorrection.A33 = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].A11;

    vfeVideoConfig.colorCorrection.k0  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_g;
    vfeVideoConfig.colorCorrection.k1  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_b;
    vfeVideoConfig.colorCorrection.k2  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_r;

    if (vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].q_factor == 7)
    {
       vfeVideoConfig.colorCorrection.coefficient_Q_Factor = VFE_SIGNED_Q7;
    }
    else
    {
       return FALSE;
    }
  }

  /* 7. Luma/ASF Filter Parameters */
  if(vfe_input_ycbcr(vfeVideoConfig.inputConfiguration.format) == TRUE)
  {
 #ifndef QDSP_MODULE_VFE25_DEFINED
    vfeVideoConfig.f3x3LumaFilter.enable = VFE_Disable;
 #endif /* nQDSP_MODULE_VFE25_DEFINED */
    vfeVideoConfig.asf_cfg.enable = VFE_Disable;
  }
 #ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
 #else /* QDSP_MODULE_VFE25_DEFINED */
  else if ( vfe_hw_features.filter_3x3_asf == TRUE )
  {
    vfeVideoConfig.f3x3LumaFilter.enable = VFE_Disable;
    if ( vfe_sw_features.enable_hw_filter_3x3_asf == TRUE )
    {
      /* Use HW 3x3 ASF in preview */
      vfeVideoConfig.asf_cfg = vfe_asf_cfg;
    } 
    else
    {
      /* use 3x3 ASF hardware as luma filter */
      vfeVideoConfig.asf_cfg.enable = (VFE_EnableType)(camsensor_params->f3x3LumaFilter.enable);
      vfeVideoConfig.asf_cfg.lpf_q_factor = VFE_SIGNED_3BIT_Q4;
      /* set filter thresholds so that lowpass always used,
       * highpass never used*/
      vfeVideoConfig.asf_cfg.lpf_threshold = 0x3FF;
      vfeVideoConfig.asf_cfg.hpf_threshold = 0x3FF;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A11))
        vfeVideoConfig.asf_cfg.l11 = camsensor_params->f3x3LumaFilter.A11;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A12))
        vfeVideoConfig.asf_cfg.l12 = camsensor_params->f3x3LumaFilter.A12;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A13))
        vfeVideoConfig.asf_cfg.l13 = camsensor_params->f3x3LumaFilter.A13;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A21))
        vfeVideoConfig.asf_cfg.l21 = camsensor_params->f3x3LumaFilter.A21;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A22))
        vfeVideoConfig.asf_cfg.l22 = camsensor_params->f3x3LumaFilter.A22;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A23))
        vfeVideoConfig.asf_cfg.l23 = camsensor_params->f3x3LumaFilter.A23;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A31))
        vfeVideoConfig.asf_cfg.l31 = camsensor_params->f3x3LumaFilter.A31;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A32))
        vfeVideoConfig.asf_cfg.l32 = camsensor_params->f3x3LumaFilter.A32;
      ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A33))
        vfeVideoConfig.asf_cfg.l33 = camsensor_params->f3x3LumaFilter.A33;
    }
  }
  else if (vfe_hw_features.filter_3x3_luma == TRUE)
  {
    /* Use the Luma Filter */
    vfeVideoConfig.asf_cfg.enable = VFE_Disable;
    vfeVideoConfig.f3x3LumaFilter.enable = (VFE_EnableType)(camsensor_params->f3x3LumaFilter.enable);
    vfeVideoConfig.f3x3LumaFilter.coefficient_Q_Factor = VFE_SIGNED_3BIT_Q4;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A11))
    vfeVideoConfig.f3x3LumaFilter.A11 = camsensor_params->f3x3LumaFilter.A11;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A12))
    vfeVideoConfig.f3x3LumaFilter.A12 = camsensor_params->f3x3LumaFilter.A12;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A13))
    vfeVideoConfig.f3x3LumaFilter.A13 = camsensor_params->f3x3LumaFilter.A13;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A21))
    vfeVideoConfig.f3x3LumaFilter.A21 = camsensor_params->f3x3LumaFilter.A21;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A22))
    vfeVideoConfig.f3x3LumaFilter.A22 = camsensor_params->f3x3LumaFilter.A22;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A23))
    vfeVideoConfig.f3x3LumaFilter.A23 = camsensor_params->f3x3LumaFilter.A23;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A31))
    vfeVideoConfig.f3x3LumaFilter.A31 = camsensor_params->f3x3LumaFilter.A31;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A32))
    vfeVideoConfig.f3x3LumaFilter.A32 = camsensor_params->f3x3LumaFilter.A32;
    ASSERT(IS_INT8(camsensor_params->f3x3LumaFilter.A33))
    vfeVideoConfig.f3x3LumaFilter.A33 = camsensor_params->f3x3LumaFilter.A33;
  }
  else
  {
    vfeVideoConfig.asf_cfg.enable = VFE_Disable;
    vfeVideoConfig.f3x3LumaFilter.enable = VFE_Disable;
  }
#endif /* !QDSP_MODULE_VFE25_DEFINED */

  /* 8. Black level correction */
  if (vfe_input_ycbcr(vfeVideoConfig.inputConfiguration.format) == TRUE)
  {
    vfeVideoConfig.blackCorrection.enable      = VFE_Disable;
  }
  else
  {
    vfeVideoConfig.blackCorrection.enable      = (VFE_EnableType)(camsensor_params->blackCorrection.enable);
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
    vfeVideoConfig.blackCorrection.lastPixel   = camsensor_params->blackCorrection.lastPixel;
    vfeVideoConfig.blackCorrection.firstPixel  = camsensor_params->blackCorrection.firstPixel;
    vfeVideoConfig.blackCorrection.lastLine    = camsensor_params->blackCorrection.lastLine;
    vfeVideoConfig.blackCorrection.firstLine   = camsensor_params->blackCorrection.firstLine;
    vfeVideoConfig.blackCorrection.shift       = camsensor_params->blackCorrection.shift;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  }
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

  /* 9. Epoch line */
  vfeVideoConfig.camif_epoch_intr.epoch_line = camsensor_params->epoch_line;

  vfeVideoConfig.camif_config.VFE_OutputEn     = TRUE;
#ifdef CAMERA_NO_MDDI
  vfeVideoConfig.camif_config.VSYNCSampleEdge  = 0;
  vfeVideoConfig.camif_config.HSYNCSampleEdge  = 0;
  vfeVideoConfig.camif_config.DataSampleEdge   = 0;
#endif /* CAMERA_NO_MDDI */
  vfeVideoConfig.camif_config.INTR_SubsampleEn = FALSE;
  vfeVideoConfig.camif_config.AHB_OutputEn     = FALSE;
#ifndef CAMERA_NO_MDDI
  vfeVideoConfig.camif_config.DMA_OutputEn     = FALSE;
#endif /* nCAMERA_NO_MDDI */
  vfeVideoConfig.camif_config.AHB_SubsampleEn  = FALSE;
#ifndef CAMERA_NO_MDDI
  vfeVideoConfig.camif_config.DMA_SubsampleEn  = FALSE;
#endif /* nCAMERA_NO_MDDI */

  vfeVideoConfig.camif_config.VFE_SubsampleEn       = FALSE; 
  vfeVideoConfig.camif_subsample1_config.pixelSkip  = 0xFFFF;
  vfeVideoConfig.camif_subsample1_config.lineSkip   = 0xFFFF;
  vfeVideoConfig.camif_subsample2_config.frameSkip = CAMIF_ALL_FRAMES0;

  vfeVideoConfig.videoOutput1FrameSkip.skipBitPattern  = vfe_skip_pattern;
  vfeVideoConfig.videoOutput1FrameSkip.bitPatternWidth = vfe_skip_width;

  vfeVideoConfig.output2Configuration.startBackend   = VFE_Disable;
  vfeVideoConfig.output2Configuration.output2Enable  = VFE_Disable;

  /* Disable the Epoch Interrupt by setting more number of lines than 
   * in a frame 
   */
  vfeVideoConfig.camif_epoch_intr.epoch_line = 0x3FFF;

  /* VFE clipping */
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  if (vfe_input_ycbcr(vfeVideoConfig.inputConfiguration.format) == FALSE)
  {
    vfeVideoConfig.clip.firstPixel = 6;
    vfeVideoConfig.clip.lastPixel = vfeVideoConfig.inputConfiguration.width - 1 - 6 ;
    vfeVideoConfig.clip.lastPixel -= (vfeVideoConfig.clip.lastPixel - vfeVideoConfig.clip.firstPixel + 1) % 8;

    vfeVideoConfig.clip.firstLine = 3 + 2 + camsensor_params->blackrows;
    vfeVideoConfig.clip.lastLine = vfeVideoConfig.inputConfiguration.height - 3;
    vfeVideoConfig.clip.lastLine -= (vfeVideoConfig.clip.lastLine - vfeVideoConfig.clip.firstLine + 1) % 8;
  }
  else
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
  {
    vfeVideoConfig.clip.firstPixel = 0;
    vfeVideoConfig.clip.lastPixel = vfeVideoConfig.inputConfiguration.width - 1;

    vfeVideoConfig.clip.firstLine = 0;
    vfeVideoConfig.clip.lastLine = vfeVideoConfig.inputConfiguration.height - 1;
  }

  /* Set the AEC/AWB stats region number after clip window is set. */
  window_width = vfeVideoConfig.clip.lastPixel - vfeVideoConfig.clip.firstPixel + 1;
  window_height = vfeVideoConfig.clip.lastLine - vfeVideoConfig.clip.firstLine + 1;
  vfe_set_aec_awb_num_regions(window_width, window_height, camsensor_params);

  /* Output1 dimensions. */
  if (output1_width > (vfeVideoConfig.clip.lastPixel - vfeVideoConfig.clip.firstPixel + 1)) 
  {
    output1_width  = (vfeVideoConfig.clip.lastPixel - vfeVideoConfig.clip.firstPixel + 1);
  }

  if (output1_height > (vfeVideoConfig.clip.lastLine - vfeVideoConfig.clip.firstLine + 1))
  {
    output1_height = (vfeVideoConfig.clip.lastLine - vfeVideoConfig.clip.firstLine + 1);
  }

  vfeVideoConfig.output1Configuration.width         = output1_width;
  vfeVideoConfig.output1Configuration.height        = output1_height;

  /* Configure camif_subsample */
  vfe_calculate_video_camif_subsample ();

  return TRUE;
} /* vfe_copy_sensor_video_config */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_MODIFY_CONTRAST

DESCRIPTION
              Modify the regular gamma table for the contrast parm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vfe_modify_contrast(int parm, vfe_gamma_table_type gamma_table_type)
{
  CAMQDSP_GammaTableType* selected_gamma_table_ptr = NULL;

  switch(gamma_table_type)
  {
    case GAMMA_TABLE_DEFAULT:
      selected_gamma_table_ptr = &vfe_default_gamma_table;
      break;
    case GAMMA_TABLE_OUTDOOR:
      selected_gamma_table_ptr = &vfe_outdoor_gamma_table;
      break;
    case GAMMA_TABLE_LOWLIGHT:
      selected_gamma_table_ptr = &vfe_lowlight_gamma_table;
      break;
    case GAMMA_TABLE_BACKLIGHT:
      selected_gamma_table_ptr = &vfe_backlight_gamma_table;
      break;
    case GAMMA_TABLE_INTERPOLATED: 
      selected_gamma_table_ptr = &vfe_interpolated_gamma_table;
      break;
    default:
      selected_gamma_table_ptr = &vfe_default_gamma_table;
      break;
  }

  (void)ipl_set_contrast_Nentries(parm, 10, (uint8*)selected_gamma_table_ptr,
      (uint8*)&vfe_current_gamma_table); 
} /* vfe_modify_contrast */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifndef QDSP_MODULE_VFE25_DEFINED
/*===========================================================================

FUNCTION      VFE_BUILD_ROLLOFF_TABLE

DESCRIPTION
              Modify the regular gamma table for the contrast parm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
double cf_delta, rs_delta;
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
int vfe_build_rolloff_table
(
 CAMQDSP_RolloffType *dst,
 uint32 divider, /* 1 for full size. 4 for quarter size. */
 const camsensor_chromatix_rolloff_table_type *src
)
#endif /* CAMERA_USES_CHROMATIX_0x203 */
{
/* make cf a Q10 number */
#define CF_FRACTIONAL_PART  10
/* make cf a 13u number */
#define CF_PRECISION   13

/* make slope a Q10 number */
#define SLOPE_FRACTIONAL_PART 10
/* make slope a 16u number */
#define SLOPE_PRECISION   19
/* shift the actual slope value up */
#define SLOPE_SHIFT    20

  int i;
  double boost_scale;
  double f_scale;
  int i_scale;
  double f_val;
  uint32 u_val;
  int i_val;
  unsigned long and_mask;
  int max_val, min_val;
  double red_slope_table[33], green_slope_table[33], blue_slope_table[33];

  /* CF tables (always positive numbers ) */
  i_scale=1 << CF_FRACTIONAL_PART;
  f_scale=(double) i_scale;
  and_mask= (1 << CF_PRECISION ) -1; /* the maximum value of CF */

  /* Fill in CF table entries */
  for (i=0;i<src->N;i++)
  {
    /* red CF table */
    f_val= (f_scale * src->red_cf_table[i]) +0.5;
    u_val= (uint32) f_val;
    if (u_val>and_mask)
    {
      return -2;
    }
    dst->SCFTable_R[i].c_factor = u_val;

    /* green CF table */
    f_val= (f_scale * src->green_cf_table[i]) +0.5;
    u_val= (uint32) f_val;
    if (u_val>and_mask)
    {
      return -2;
    }
    dst->SCFTable_G[i].c_factor = u_val;

    /* blue CF table */
    f_val= (f_scale * src->blue_cf_table[i]) +0.5;
    u_val= (uint32) f_val;
    if (u_val>and_mask)
    {
      return -2;
    }
    dst->SCFTable_B[i].c_factor = u_val;
  }

  /* Generate slope tables in double */
  for (i=0; i<32; i++)
  {
    rs_delta = src->radius_square_table[i+1] - src->radius_square_table[i];
    cf_delta = src->red_cf_table[i+1] - src->red_cf_table[i];
    red_slope_table[i] = (cf_delta / rs_delta) * divider;
    cf_delta = src->green_cf_table[i+1] - src->green_cf_table[i];
    green_slope_table[i] = (cf_delta / rs_delta) * divider;
    cf_delta = src->blue_cf_table[i+1] - src->blue_cf_table[i];
    blue_slope_table[i] = (cf_delta / rs_delta) * divider;
  }

  /* Slope tables ( 1 bit sign + 8 bit magintue + 10 bit fractional) */
  i_scale=1 << SLOPE_SHIFT;
  boost_scale=(double) i_scale;
  i_scale=1 << SLOPE_FRACTIONAL_PART;
  f_scale = (double) i_scale;   /* the scale factor to be multiplied */
  and_mask=(1 << SLOPE_PRECISION) -1;
  max_val= (1 << (SLOPE_PRECISION-1)) -1; /* the maximum value of slope */
  min_val= -1 - max_val;

  /* Fill in slope and radius square table */
  for (i=0;i<src->N;i++)
  {
    /*lint -save -e653 loss of fraction */
    dst->RSTable[i].radius_square = (int)(src->radius_square_table[i]/divider+0.5);
    /*lint -restore */
    /* red */
    f_val=red_slope_table[i]* boost_scale;
    f_val=(f_val*f_scale) + 0.5;
    i_val= (int) f_val;
    if (i_val>max_val || i_val < min_val)
    {
      if (i==(src->N-1))
      {
        if (i_val<min_val) i_val=min_val;
        if (i_val>max_val) i_val=max_val;
      }
      else
      {
        return -3;
      }
    }
    dst->SCFTable_R[i].slope=i_val;

    /* green */
    f_val=green_slope_table[i]* boost_scale;
    f_val=(f_val*f_scale) + 0.5;
    i_val= (int) f_val;
    if (i_val>max_val || i_val < min_val)
    {
      if (i==(src->N-1))
      {
        if (i_val<min_val) i_val=min_val;
        if (i_val>max_val) i_val=max_val;
      }
      else
      {
        return -3;
      }
    }
    dst->SCFTable_G[i].slope=i_val;

    /* blue */
    f_val=blue_slope_table[i]* boost_scale;
    f_val=(f_val*f_scale) + 0.5;
    i_val= (int) f_val;
    if (i_val>max_val || i_val < min_val)
    {
      if (i==(src->N-1))
      {
        if (i_val<min_val) i_val=min_val;
        if (i_val>max_val) i_val=max_val;
      }
      else
      {
        return -3;
      }
    }
    dst->SCFTable_B[i].slope=i_val;
  }

  return 0;
} /* vfe_build_rolloff_table */
#endif /* !QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifndef QDSP_MODULE_VFE25_DEFINED
/*===========================================================================

FUNCTION:

  VFE_INIT_3X3_ASF_DATA

DESCRIPTION:
  Initialize the asf configuration data
  
CALLERS:
  vfe_start()

SIDE EFFECTS:
  None

RETURN VALUE:
  None

SCOPE:
  Local

============================================================================*/
static boolean vfe_init_3x3_asf_data
(
 const camsensor_static_params_type *camsensor_params
)
{

  if (vfe_hw_features.filter_3x3_asf == TRUE)
  {
    /* 3x3 ASF HW is available and to be used as ASF */
    vfe_asf_cfg.enable = VFE_Enable;


#ifdef CAMERA_USES_CHROMATIX_0x203
    vfe_asf_cfg.hpf_q_factor = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor;
    vfe_asf_cfg.lpf_q_factor = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor;
    vfe_asf_cfg.hpf_threshold = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_threshold;
    vfe_asf_cfg.lpf_threshold = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_threshold;
    vfe_asf_cfg.sobel_mask = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_detection_flag;
    camera_trigger_set_asf_3x3(&cam3a_aec_state,&camsensor_static_params[camera_asi]);
#elif defined (CAMERA_USES_CHROMATIX_0x201) 
    vfe_asf_cfg.hpf_q_factor = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor;
    vfe_asf_cfg.lpf_q_factor = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor;
    vfe_asf_cfg.hpf_threshold = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_threshold;
    vfe_asf_cfg.lpf_threshold = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_threshold;
    vfe_asf_cfg.sobel_mask = camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_detection_flag;
    /* Edge (High Pass) Filter */
    vfe_asf_cfg.h11 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[0][0],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h12 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[0][1],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h13 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[0][2],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h21 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[1][0],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h22 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[1][1],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h23 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[1][2],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h31 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[2][0],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h32 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[2][1],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    vfe_asf_cfg.h33 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter[2][2],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.edge_filter_q_factor);
    /* Noise (Low Pass) Filter */
    vfe_asf_cfg.l11 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[0][0],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l12 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[0][1],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l13 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[0][2],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l21 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[1][0],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l22 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[1][1],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l23 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[1][2],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l31 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[2][0],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l32 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[2][1],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
    vfe_asf_cfg.l33 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter[2][2],
        camsensor_params->chromatix_parms->chromatix_adaptive_spatial_filter.noise_filter_q_factor);
#else
    vfe_asf_cfg.hpf_q_factor = camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor;
    vfe_asf_cfg.lpf_q_factor = camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor;
    vfe_asf_cfg.hpf_threshold = camsensor_params->chromatix_parms->hw_asf_3_3.edge_threshold;
    vfe_asf_cfg.lpf_threshold = camsensor_params->chromatix_parms->hw_asf_3_3.noise_threshold;
    vfe_asf_cfg.sobel_mask = camsensor_params->chromatix_parms->hw_asf_3_3.edge_detection_flag;
    /* Edge (High Pass) Filter */
    vfe_asf_cfg.h11 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[0][0],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h12 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[0][1],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h13 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[0][2],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h21 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[1][0],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h22 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[1][1],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h23 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[1][2],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h31 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[2][0],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h32 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[2][1],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    vfe_asf_cfg.h33 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter[2][2],
        camsensor_params->chromatix_parms->hw_asf_3_3.edge_filter_q_factor);
    /* Noise (Low Pass) Filter */
    vfe_asf_cfg.l11 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[0][0],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l12 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[0][1],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l13 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[0][2],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l21 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[1][0],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l22 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[1][1],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l23 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[1][2],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l31 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[2][0],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l32 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[2][1],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
    vfe_asf_cfg.l33 = vfe_convert_float_to_Qfactor(
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter[2][2],
        camsensor_params->chromatix_parms->hw_asf_3_3.noise_filter_q_factor);
#endif
  }
  else
  {
    vfe_asf_cfg.enable = VFE_Disable;
  }
  return TRUE;
} /* vfe_init_3x3_asf_data */
#endif /* !QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION:

  VFE_CALCULATE_VIDEO_CAMIF_SUBSAMPLE 

DESCRIPTION:
  Configure camif subsampling pattern for video mode.
  Camif sub-sampling is used for
  - Preserve aspect ratio, set by camsensor driver.
  - Resize factor is more than scalers capability, i.e., more than 16
  Updated global variables:
  - vfeVideoConfig.
  
CALLERS:
  vfe_copy_sensor_video_config ()

SIDE EFFECTS:
  None

RETURN VALUE:
  None

SCOPE:
  Local

============================================================================*/
static void vfe_calculate_video_camif_subsample (void)
{
  /* Camif sub-sampling required to offset scalers limitation */
  float  resize_camif_x_subsample;
  uint32 scaler_limit = 16;
  uint32 final_camif_x_subsample;
  uint32 aspect_ratio_camif_x_subsample;


  aspect_ratio_camif_x_subsample = camctrl_get_camif_x_subsample ();

  if (!aspect_ratio_camif_x_subsample)
  {
    /* If 0, treat it as 1, this will also take care of the
     * backward compatibility with the sensor drivers that 
     * don't set the camif_x_subsample 
     */
    aspect_ratio_camif_x_subsample = 1;
  }
 

  resize_camif_x_subsample = 
    (float) vfeVideoConfig.inputConfiguration.width / 
    (aspect_ratio_camif_x_subsample * 
     vfeVideoConfig.output1Configuration.width);

  resize_camif_x_subsample /= scaler_limit;

  /* See if there is any fraction, ceil to the next integer if any */
  if (resize_camif_x_subsample - (uint32) resize_camif_x_subsample)
  {
    resize_camif_x_subsample = (uint32) resize_camif_x_subsample + 1;
  }

  final_camif_x_subsample = (uint32) resize_camif_x_subsample *
                           aspect_ratio_camif_x_subsample; 

  /* CAMIF pixel sub-sampling to preserve aspect ratio. 
   * sub-sampling ratios, 2, 3, 4 are supported.
   */
  switch (final_camif_x_subsample)
  {
    case 2:
      vfeVideoConfig.camif_subsample1_config.pixelSkip = 0xF0F0;
      vfeVideoConfig.camif_subsample2_config.PixelSkipWrap = 
          CAMIF_Every_16_Cycles;
      vfeVideoConfig.camif_config.VFE_SubsampleEn  = TRUE; 
      vfeVideoConfig.inputConfiguration.width /= 2;
      break;

    case 3:
      /* Skip pattern starts from MSB */
      vfeVideoConfig.camif_subsample1_config.pixelSkip = 0xC300;
      vfeVideoConfig.camif_config.VFE_SubsampleEn  = TRUE; 
      vfeVideoConfig.camif_subsample2_config.PixelSkipWrap = 
          CAMIF_Every_12_Cycles;
      vfeVideoConfig.inputConfiguration.width /= 3;
      break;

    case 4:
      vfeVideoConfig.camif_subsample1_config.pixelSkip = 0xF000;
      vfeVideoConfig.camif_config.VFE_SubsampleEn  = TRUE; 
      vfeVideoConfig.camif_subsample2_config.PixelSkipWrap = 
          CAMIF_Every_16_Cycles;
      vfeVideoConfig.inputConfiguration.width /= 4;
      break;

    default:
      vfeVideoConfig.camif_config.VFE_SubsampleEn  = FALSE; 
      break;
  }

  /* Enable sub-sampling if resizing factor is more than 16 
   * Compute the decimation factors again. 
   */
  if ((vfeVideoConfig.inputConfiguration.height / 
       vfeVideoConfig.output1Configuration.height) >= 16) 
  {
    vfeVideoConfig.inputConfiguration.height /= 2;
    vfeVideoConfig.camif_config.VFE_SubsampleEn  = TRUE; 
    vfeVideoConfig.camif_subsample1_config.lineSkip   = 0xF0F0;
  }
} /* vfe_calculate_video_camif_subsample */



/*=== =========================================================================
                        EXTERNAL API DEFINITIONS
============================================================================*/

/*===========================================================================


FUNCTION:

  VFE_GET_VERSION

DESCRIPTION:
  Returns the VFE Version 
CALLERS:
  

DEPENDENCIES:
  None

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global

============================================================================*/

CAMQDSP_VfeType vfe_get_type()

{

   return vfe_type;

}

/*===========================================================================

FUNCTION:

  VFE_INIT (former VFE_FEATURES_AUTO_DETECT)

DESCRIPTION:
  Determines the VFE H/W features supported based on the
  VFE version. Populates a global structure with the h/w and s/w
  features supported. Applications use this global variable 
  for deciding whether to use H/W vs ARM/DSP based solutions.

CALLERS:
  Populates vfe_hw_features and vfe_sw_features global variables.

USAGE:
  During Camera services initialization, this routine is called
  to determine the VFE H/W features supported. Based on this information,
  camera services decides whether to use H/W features or ARM/DSP
  implemented features.

RETURN VALUE:
  None.

=========================================================================== */
void vfe_init (void)
{
  
  /* Initialize */
  vfe_hw_features.epoch_int2             = FALSE; 
  vfe_hw_features.skip_first_frame       = FALSE;
  vfe_hw_features.timers                 = FALSE; 
  vfe_hw_features.demosaic_2g            = FALSE; 
  vfe_hw_features.limited_sub_sampling   = FALSE;
 #ifndef QDSP_MODULE_VFE25_DEFINED  
  vfe_hw_features.filter_3x3_luma        = FALSE; 
  vfe_hw_features.filter_3x3_asf         = FALSE;
 #endif /* QDSP_MODULE_VFE25_DEFINED */
  vfe_hw_features.chroma_histogram       = FALSE;
  vfe_hw_features.frame_rate_pattern     = FALSE; 

  camsensor_msm_auto_detect();

  switch (camsensor_info.msm_id)
  {
    case MSM6260:
    case SC2X:
#ifdef FEATURE_CAMIF_INTERNAL_TEST
    case QSC6085:
#endif
    case MSM6246:
    case QSC1110:
		/* VFE_EAGLE_05 */
      vfe_hw_features.epoch_int2             = TRUE; 
      vfe_hw_features.skip_first_frame       = TRUE;
      vfe_hw_features.demosaic_2g            = TRUE; 
      vfe_hw_features.limited_sub_sampling   = TRUE;
      vfe_hw_features.frame_rate_pattern     = TRUE; 
      vfe_type = CAMQDSP_VFE05;
      break;
    case MSM6800_R2:
      /* VFE_EAGLE_11 */
      vfe_hw_features.chroma_histogram       = TRUE;
      vfe_type = CAMQDSP_VFE1P;
      break;
    case MSM6550_90:
    case MSM6280:
      /* VFE_EAGLE_12 */
 #ifndef QDSP_MODULE_VFE25_DEFINED  
      vfe_hw_features.filter_3x3_luma        = TRUE; 
 #endif /* QDSP_MODULE_VFE25_DEFINED */
      vfe_hw_features.chroma_histogram       = TRUE;
      vfe_type = CAMQDSP_VFE1P;
      break;
    case MSM6290:
    case MSM6800B:
      /* VFE_EAGLE_20 */
      vfe_hw_features.timers                 = TRUE; 
 #ifndef QDSP_MODULE_VFE25_DEFINED  
      vfe_hw_features.filter_3x3_asf         = TRUE;
 #endif /* QDSP_MODULE_VFE25_DEFINED */
      vfe_hw_features.chroma_histogram       = TRUE;
      vfe_type = CAMQDSP_VFE20;
     break;
   case QSC6270:
      /* QSC6240/6270 */
      vfe_hw_features.chroma_histogram       = TRUE;
      vfe_type = CAMQDSP_VFE25;
      break;

    default:
      /* No VFE H/W features */
      vfe_type = CAMQDSP_VFE10;
      break;
  } /* switch: camsensor_info.msm_id */
  CAMQDSP_SelectModuleList(vfe_type);
} /* vfe_features_auto_detect */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_INIT_GAMMA_TABLE

DESCRIPTION
              Determine the right gamma table size, and copy or downsize
              from device gamma tables.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
void vfe_init_gamma_table
(
  const camsensor_static_params_type *camsensor_params /* Other config params */
)
{
  #if defined (CAMERA_USES_CHROMATIX_0x201)|| defined (CAMERA_USES_CHROMATIX_0x203)
  uint16 reflex_point;
  uint16 i;
  #endif
  vfe_gamma_table_size = 1024;

  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #elif defined CAMERA_USES_CHROMATIX_0x201
  /* Generating the Solarize Gamma table since 0x201 Header Just have the relex Point */
  reflex_point= (camsensor_params->chromatix_parms->solarized_gamma_reflection_point<<2);
  memcpy(&vfe_solarized_effects_gamma_table, &(camsensor_params->chromatix_parms->default_gamma_table), 
         (reflex_point + 1) * sizeof (uint8));
  for (i=reflex_point;i<1024;i++) {
      vfe_solarized_effects_gamma_table.arr[i] = ((255-(camsensor_params->chromatix_parms->default_gamma_table.arr[i]))\
                                               * camsensor_params->chromatix_parms->default_gamma_table.arr[reflex_point - 1] /\
                                              (255 - camsensor_params->chromatix_parms->default_gamma_table.arr[reflex_point - 1]));
  }
  #else
  memcpy( &vfe_solarized_effects_gamma_table, &(camsensor_params->chromatix_parms->solarized_effects_gamma_table), vfe_gamma_table_size );
  #endif
  
  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #else

  memcpy( &vfe_default_gamma_table, &(camsensor_params->chromatix_parms->default_gamma_table), vfe_gamma_table_size );
  memcpy( &vfe_outdoor_gamma_table, &(camsensor_params->chromatix_parms->outdoor_gamma_table), vfe_gamma_table_size );
  memcpy( &vfe_lowlight_gamma_table, &(camsensor_params->chromatix_parms->lowlight_gamma_table), vfe_gamma_table_size );
  memcpy( &vfe_backlight_gamma_table, &(camsensor_params->chromatix_parms->backlight_gamma_table), vfe_gamma_table_size );

  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  vfe_current_contrast = 0xffff;
  vfe_current_gamma_table_type = GAMMA_TABLE_INVALID;
} /* vfe_init_gamma_table */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifndef QDSP_MODULE_VFE25_DEFINED

/*===========================================================================

FUNCTION:

  VFE_INIT_ROLLOFF_TABLES

DESCRIPTION:
  Initialize the rolloff table for all 3 lighting conditions and for both
  full size scale and quarter size scale.

SIDE EFFECTS:
  None

RETURN VALUE:
  None

SCOPE:
  Local

============================================================================*/
boolean vfe_init_rolloff_tables
(
 const camsensor_static_params_type *camsensor_params
)
{
  int i;
  uint32 divider,divider_sqr;

  if (vfe_sw_features.rolloff_correction != VFE_ROLLOFF_CORRECTION)
    return TRUE;


  /* r squire re-size calculation in Q12 format with Q11 rounding */
  divider = (uint32)((camsensor_params->preview_dy_decimation + 2048)/4096);
  divider_sqr = divider*divider;

  /* Build rolloff tables for all lights and full scale */
  /*lint -save -e737 loss of sign */

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
  i = vfe_build_rolloff_table(&vfe_rolloff_table_A_F, 1,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT]);
  i += vfe_build_rolloff_table(&vfe_rolloff_table_D65_F, 1,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT]);
  i += vfe_build_rolloff_table(&vfe_rolloff_table_TL84_F, 1,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT]);
  i += vfe_build_rolloff_table(&vfe_rolloff_table_low_light_F, 1,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT]);
  if (i) return FALSE;

  /* Build rolloff tables for all lights and quarter scale */
  i += vfe_build_rolloff_table(&vfe_rolloff_table_A_Q, divider_sqr,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT]);
  i += vfe_build_rolloff_table(&vfe_rolloff_table_D65_Q, divider_sqr,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT]);
  i += vfe_build_rolloff_table(&vfe_rolloff_table_TL84_Q, divider_sqr,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT]);
  i += vfe_build_rolloff_table(&vfe_rolloff_table_low_light_Q, divider_sqr,
      &camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT]);
  /*lint -restore */
  if (i) return FALSE;

  /* Build rolloff_cfgs */
  vfe_rolloff_cfg_A_F.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cx;
  vfe_rolloff_cfg_A_F.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cy;
  vfe_rolloff_cfg_A_Q.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cx / divider;
  vfe_rolloff_cfg_A_Q.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_A_LIGHT].cy / divider;

  vfe_rolloff_cfg_D65_F.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cx;
  vfe_rolloff_cfg_D65_F.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cy;
  vfe_rolloff_cfg_D65_Q.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cx / divider;
  vfe_rolloff_cfg_D65_Q.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_D65_LIGHT].cy / divider;

  vfe_rolloff_cfg_TL84_F.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cx;
  vfe_rolloff_cfg_TL84_F.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cy;
  vfe_rolloff_cfg_TL84_Q.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cx / divider;
  vfe_rolloff_cfg_TL84_Q.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_TL84_LIGHT].cy / divider;

  vfe_rolloff_cfg_low_light_F.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cx;
  vfe_rolloff_cfg_low_light_F.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cy;
  vfe_rolloff_cfg_low_light_Q.cx = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cx / divider;
  vfe_rolloff_cfg_low_light_Q.cy = camsensor_params->chromatix_parms->vfe_hw_rolloff_data[ROLLOFF_LOW_LIGHT].cy / divider;
 
  return TRUE;
#endif /* FEATURE_CAMERA_USES_CHROMATIX_0x203 */

} /* vfe_init_rolloff_tables */
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_PUT_ROLLOFF_TABLE

DESCRIPTION
              Copy radius square rolloff table to DSP.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void vfe_put_rolloff_table(void)
{
  CAMQDSP_put_RolloffTable(vfe_current_rolloff_table);
} /* vfe_put_rolloff_table(void) */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_PUT_GAMMA_TABLE

DESCRIPTION
              Put the correct gamma table to the DSP.

DEPENDENCIES
  Assumes that the VFE has been put into IDLE state first.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void vfe_put_gamma_table(int32 special_effect, int32 contrast, camsensor_op_mode_type op_mode)
{
  vfe_gamma_table_type selected_gamma_table_type;

  if (special_effect == (int)CAMERA_EFFECT_SOLARIZE)
  {
    /* Use Solarize effects table */
    CAMQDSP_put_GammaTable( &vfe_solarized_effects_gamma_table, vfe_gamma_table_size );
  }
  else
  {
    selected_gamma_table_type = camera_3a_select_gamma_table(op_mode);

    if ( (contrast != vfe_current_contrast) || (selected_gamma_table_type != vfe_current_gamma_table_type) 
         || (selected_gamma_table_type == GAMMA_TABLE_INTERPOLATED) ) 
    {
      if(selected_gamma_table_type == GAMMA_TABLE_INTERPOLATED)

      {
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
        {
          vfe_interpolate_gamma_table();
        }
      }
      vfe_modify_contrast(contrast, selected_gamma_table_type);
      vfe_current_contrast = contrast;
      vfe_current_gamma_table_type = selected_gamma_table_type;
    }
    CAMQDSP_put_GammaTable( &vfe_current_gamma_table, vfe_gamma_table_size );
  }
} /* vfe_put_gamma_table */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION: 

  VFE_INITIALIZE

DESCRIPTION:
  Downloads the VFE firmware image.

CALLERS:
  camera_process_start_preview ()
  camera_process_take_picture ()
  camera_process_offline_snapshot ()
  
SIDE EFFECTS:

RETURN VALUE:

USAGE:
  Upon entry of camera service operations.

SCOPE:
  Global 

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Initialize (CAMQDSP_MsgCBType cb)
{
#ifdef QDSP_MODULE_VFE05_CAPTURE_DEFINED
    if(camctrl_tbl.use_vfe_image_swap == TRUE)
      return CAMQDSP_Initialize (cb, CAMQDSP_VFE_CAPTURE);
    else
#endif /*QDSP_MODULE_VFE05_CAPTURE_DEFINED*/

#ifdef QDSP_MODULE_VFE25_SA_DEFINED
#error code not present
#endif /*QDSP_MODULE_VFE25_SA_DEFINED*/

  return CAMQDSP_Initialize (cb, CAMQDSP_VFE);
} /* VFE_Initialize */

/*===========================================================================

FUNCTION:  

  VFE_CONFIGUREVIDEO

DESCRIPTION:
  - Configures VFE for video mode.
  - Bound checking of VFE parameters passed from upper layers.

CALLERS:
  camera_config_vfe ()

SIDE EFFECTS:

RETURN VALUE:

USAGE:
  For continuous and real-time processing of camera sensor data.

SCOPE:
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureVideo
(
 VFE_VideoConfigureType *config
)
{
  uint32 size;
  #ifndef FEATURE_CAMERA_YCBCR_ONLY
  camsensor_lc_type new_lc;
  #endif

  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_VerifyInputConfigurationType          (config->inputConfiguration)    &&
        VFE_VerifyOutputConfigurationType         (config->output1Configuration)  &&
        VFE_VerifyVideoOutput2ConfigurationType   (config->output2Configuration)  &&
        VFE_VerifyAHBConfigurationType            (config->output1_AHB_Configuration) &&
        VFE_VerifyGammaCorrectionType             (config->gammaCorrection)       &&
        VFE_VerifyBlackCorrectionType             (config->blackCorrection)       &&
        VFE_VerifyVideoOutputFrameSkipPatternType (config->videoOutput1FrameSkip) &&
        VFE_VerifyVideoOutputFrameSkipPatternType (config->videoOutput2FrameSkip) &&
        VFE_VerifyClipType                        (config->clip)                  &&
        VFE_VerifyDefectPixelCorrectionType       (config->defectPixelCorrection) &&
        VFE_VerifyColorCorrectionType             (config->colorCorrection)       &&
        VFE_VerifyColorConversionType             (config->colorConversion)
#ifndef QDSP_MODULE_VFE25_DEFINED
        && VFE_Verify3x3LumaFilterType            (config->f3x3LumaFilter)
#endif /* nQDSP_MODULE_VFE25_DEFINED */
     ))
  {
    return CAMQDSP_INVALID_PARM;
  }

  if (vfe_input_ycbcr (config->inputConfiguration.format) == TRUE)
  {
    vfe_default_color_conversion_thresholds (&config->colorConversion);
  }

#ifndef FEATURE_CAMERA_YCBCR_ONLY
 #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
    {
        vfe_select_rolloff(camsensor_preview_resolution, TRUE);
    }
  /* If you want to see a color ring, select the largest picture resolution,
   * put a break point below here and then execute previewcircle.cmm file. */
  if (vfe_current_rolloff_cfg.enable)
  {
    vfe_put_rolloff_table();
 #ifndef QDSP_MODULE_VFE25_DEFINED
    config->rolloffCfg = vfe_update_rolloff_cfg(
        config->clip.firstPixel, config->clip.lastPixel,
        config->clip.firstLine, config->clip.lastLine);
 #else
#error code not present
 #endif /* nQDSP_MODULE_VFE25_DEFINED */
 

  }
  else
  {
    config->rolloffCfg.enable = FALSE;
  }

  /* In the future we could have both VFE and sensor rolloff for the same sensor*/
  /* Check if sensor roll off is enabled 
  Always set the illuminat to TL84 for video config */
   
  if(camctrl_tbl.sensor_lc_is_supported == TRUE)
  {
    new_lc = TL84_LC;
    (void) camctrl_tbl.set_lens_correction(new_lc);
  }
#endif

  /* Calculate VFE command size */
  size = sizeof (VFE_VideoConfigureType)/4;
  size = vfe_adjust_config_size(size);
  config->whiteBalance = vfeUpdate.whiteBalance;

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_CONFIG_VIDEO, 
                              (void *) config, (int)size);
} /* VFE_ConfigureVideo */

/*===========================================================================

FUNCTION:    

  VFE_CONFIGURERAWSNAPSHOT

DESCRIPTION: 
  - Configures VFE for raw snapshot mode.  
  - Bound checking of VFE parameters passed from upper layers.

CALLERS:
  camera_config_vfe ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:
  For obtaining the sensor data as is into memory.
  These raw images are later used for image tuning.

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureRawSnapshot
(
 VFE_ConfigureRawType *config
)
{
  #ifndef FEATURE_CAMERA_YCBCR_ONLY
  camsensor_lc_type new_lc;
  #endif
  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_Verify_int_0_3_Type    (config->numOutputFragsMinus1)         &&
        VFE_Verify_int_0_15_Type   (config->numContinuousShotsMinus1)     &&
        VFE_Verify_int_1_4096_Type (config->burstFactor1ForBuffers)       &&
        VFE_Verify_int_1_256_Type  (config->burstFactor2ForBuffers)       &&
        VFE_Verify_int_1_16_Type   (config->burstLenForBuffers)           &&
        VFE_Verify_int_1_16_Type   (config->numBurstForBuffers)))
  {
    return CAMQDSP_INVALID_PARM;
  }
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  /* Check if sensor roll off is enabled */
  if(camctrl_tbl.sensor_lc_is_supported == TRUE)
  {
    new_lc = VFE_Get_Illuminant_for_LC(); 
    (void) camctrl_tbl.set_lens_correction(new_lc);
  }
#endif
  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_CONFIG_RAW_SNAPSHOT, 
                              (void *) config, 
                              sizeof (VFE_ConfigureRawType)/4);
} /* VFE_ConfigureRawSnapshot */

/*===========================================================================

FUNCTION:    
  VFE_CONFIGURESNAPSHOT

DESCRIPTION: 
  - Configures VFE for Snapshot mode
  - Bound checking of VFE parameters passed from upper layers.

CALLERS:
  camera_config_vfe ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:
  - The data obtained form the VFE can be encoded, ex., JPEG

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureSnapshot
(
 VFE_ConfigureSnapshotType *config
)
{
  uint32 size;
  #ifndef FEATURE_CAMERA_YCBCR_ONLY
  camsensor_lc_type new_lc;
  vfe_reset_color_correction_bias_per_snapshot(&(config->colorCorrection));
  #endif
  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_VerifyInputSnapshotConfigurationType  (config->inputConfiguration)   &&
        VFE_VerifyYCbCrOutputConfigurationType    (config->output1Configuration) &&
        VFE_VerifyAHBConfigurationType            (config->output1_AHB_Configuration) &&
        VFE_VerifyAHBConfigurationType            (config->output2_AHB_Configuration) &&
        VFE_VerifyGammaCorrectionType             (config->gammaCorrection) &&
        VFE_VerifyBlackCorrectionType             (config->blackCorrection) &&
        VFE_VerifyClipType                        (config->clip)            &&
        VFE_VerifyDefectPixelCorrectionType       (config->defectPixelCorrection) &&
        VFE_VerifyColorCorrectionType             (config->colorCorrection) &&
        VFE_VerifyColorConversionType             (config->colorConversion)
#ifndef CUST_EDITION
#ifdef QDSP_MODULE_VFE05_DEFINED
       && VFE_VerifyOutput1Size                   (config->output1Configuration)
#endif /* QDSP_MODULE_VFE05_DEFINED */
#endif
#ifndef QDSP_MODULE_VFE25_DEFINED
        && VFE_Verify3x3LumaFilterType            (config->f3x3LumaFilter)
#endif /* nQDSP_MODULE_VFE25_DEFINED */
     ))
  {
    return CAMQDSP_INVALID_PARM;
  }

  if (vfe_input_ycbcr ((CAMIF_InputFormatType)(config->inputConfiguration.format)) == TRUE)
  {
    vfe_default_color_conversion_thresholds (&config->colorConversion);
  }

#ifndef FEATURE_CAMERA_YCBCR_ONLY

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* CAMERA_USES_CHROMATIX_0x203 */
  {
     vfe_select_rolloff(camsensor_snapshot_resolution, FALSE);
  }
  /* If you want to see a color ring, select the largest picture resolution,
   * put a break point below here and then execute snapshotcircle.cmm file. */
  if (vfe_current_rolloff_cfg.enable)
  {
    vfe_put_rolloff_table();
 #ifndef QDSP_MODULE_VFE25_DEFINED
    config->rolloffCfg = vfe_update_rolloff_cfg(
        config->clip.firstPixel, config->clip.lastPixel,
        config->clip.firstLine, config->clip.lastLine);
 #else
#error code not present
 #endif /* nQDSP_MODULE_VFE25_DEFINED */
  }
  else
  {
    config->rolloffCfg.enable = FALSE;
  }

  /* In the future we could have both VFE and sensor rolloff for the same sensor*/
  /* Check if sensor roll off is enabled */
  
  if(camctrl_tbl.sensor_lc_is_supported == TRUE)
  {
    new_lc = VFE_Get_Illuminant_for_LC(); 
    (void) camctrl_tbl.set_lens_correction(new_lc);
  }
#endif

  /* Calculate VFE command size */
  size = sizeof (VFE_ConfigureSnapshotType)/4;
  size = vfe_adjust_config_size(size);

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_CONFIG_SNAPSHOT, 
                              (void *) config, (int)size);
} /* VFE_ConfigureSnapshot */

/*===========================================================================

FUNCTION:    
  VFE_CONFIGUREOFFLINE

DESCRIPTION: 

CALLERS:
  camera_config_vfe ()
  camera_config_offline_snapshot ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:
  Process the image from memory, ex., Bayer Vignette Correction

SCOPE: 

============================================================================*/
CAMQDSP_ReturnCodeType VFE_ConfigureOffline
(
 VFE_ConfigurationOfflineType *config
)
{
  uint32 size;
#ifndef FEATURE_CAMERA_YCBCR_ONLY
  vfe_reset_color_correction_bias_per_snapshot(&(config->colorCorrection));
#endif
  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_VerifyOfflineInputConfigurationType (config->input)                 &&
        VFE_VerifyAHBConfigurationType          (config->input_AHB)             &&
        VFE_VerifyYCbCrOutputConfigurationType  (config->output1Configuration)  &&
        VFE_VerifyAHBConfigurationType          (config->output1_AHB)           &&
        VFE_VerifyAHBConfigurationType          (config->output2_AHB)           &&
        VFE_VerifyGammaCorrectionType           (config->gammaCorrection)       &&
        VFE_VerifyBlackCorrectionType           (config->blackCorrection )      &&
        VFE_VerifyClipType                      (config->clip)                  &&
        VFE_VerifyDefectPixelCorrectionType     (config->defectPixelCorrection) &&
        VFE_VerifyColorCorrectionType           (config->colorCorrection)       &&
        VFE_VerifyColorConversionType           (config->colorConversion)
#ifndef QDSP_MODULE_VFE25_DEFINED
        && VFE_Verify3x3LumaFilterType          (config->f3x3LumaFilter)
#endif /* nQDSP_MODULE_VFE25_DEFINED */
     ))
  {
    return CAMQDSP_INVALID_PARM;
  }

  if (vfe_input_ycbcr (config->input.format) == TRUE)
  {
    vfe_default_color_conversion_thresholds (&config->colorConversion);
  }

#ifndef FEATURE_CAMERA_YCBCR_ONLY
  vfe_select_rolloff(camsensor_snapshot_resolution, FALSE);
  if (vfe_current_rolloff_cfg.enable)
  {
    vfe_put_rolloff_table();
 #ifndef QDSP_MODULE_VFE25_DEFINED
    config->rolloffCfg = vfe_update_rolloff_cfg(
        config->clip.firstPixel, config->clip.lastPixel,
        config->clip.firstLine, config->clip.lastLine);
 #else
#error code not present
 #endif /* nQDSP_MODULE_VFE25_DEFINED */
  }
  else
  {
    config->rolloffCfg.enable = FALSE;
  }
  
#endif
  /* Calculate VFE command size */
  size = sizeof (VFE_ConfigurationOfflineType)/4;
  size = vfe_adjust_config_size(size);

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_CONFIG_OFFLINE, 
                              (void *) config, (int)size);
} /* VFE_ConfigureOffline */

/*===========================================================================

FUNCTION:    

  VFE_CAPTURE

DESCRIPTION: 
  Causes the VFE H/W to start capturing the camera sensor data.

CALLERS:
  camera_initiate_capture ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:
  Before starting the VFE H/W please ensure the VFE H/W is properly
  configured.

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Capture (void)
{
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_CAPTURE,
                              NULL,
                              0);
} /* VFE_Capture */

/*===========================================================================

FUNCTION:    

  VFE_IDLE

DESCRIPTION: 
  Stops the VFE H/W. 

CALLERS:
  camera_do_stopping ()
  camera_process_stop ()
  camera_process_start_preview ()
  camera_process_release_picture ()
  camera_terminate ()
  camera_process_qdsp_output1_msg ()
  camera_process_qdsp_output2_msg ()
  camera_process_qdsp_detect_error_msg ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:
  Turning off the camera services. 
  VFE must be stopped before turning off the pclk to avoid
  system hangs.

SCOPE: 
  Global.

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Idle    (void)
{
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_IDLE,
                              NULL,
                              0);
} /* VFE_Idle */

/*===========================================================================

FUNCTION:    

  VFE_UPDATE

DESCRIPTION: 
  Updates the VFE parameters. All the parameters in the update are
  double buffered in the h/w and the new parameters take effect on the
  next frame boundary.

CALLERS:
  camera_process_qdsp_output1_msg ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_Update
(
 VFE_UpdateType *config,
 const camsensor_static_params_type *camsensor_params
)
{
  uint32 window_width, window_height;
  uint32 size;

  window_width = config->clip.lastPixel - config->clip.firstPixel + 1;
  window_height = config->clip.lastLine - config->clip.firstLine + 1;
  vfe_set_aec_awb_num_regions(window_width, window_height, camsensor_params);

  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_VerifyVideoOutputFrameSkipPatternType    (config->output1SkipPattern)     &&
        VFE_VerifyVideoOutputFrameSkipPatternType    (config->output2SkipPattern)     &&
        VFE_VerifyClipType                           (config->clip)))
  {
    return CAMQDSP_INVALID_PARM;
  }

  /* The following range checks don't apply for YCbCr */ 
  if (!vfe_input_ycbcr (camsensor_params->format) &&
      !(VFE_VerifyDefectPixelCorrectionType     (config->defectPixelCorrection) &&
        VFE_VerifyColorCorrectionType           (config->colorCorrection)       &&
        VFE_VerifyColorConversionType           (config->colorConversion)
#ifndef QDSP_MODULE_VFE25_DEFINED
        && VFE_Verify3x3LumaFilterType          (config->f3x3LumaFilter)
#endif /* nQDSP_MODULE_VFE25_DEFINED */
     ))
  {
    return CAMQDSP_INVALID_PARM;
  }

  if (vfe_input_ycbcr (camsensor_params->format) == TRUE)
  {
    vfe_default_color_conversion_thresholds (&config->colorConversion);
  }

#ifndef FEATURE_CAMERA_YCBCR_ONLY
#ifndef QDSP_MODULE_VFE25_DEFINED
  if (vfe_current_rolloff_cfg.enable)
  {
 
    config->rolloffCfg = vfe_update_rolloff_cfg(
        config->clip.firstPixel, config->clip.lastPixel,
        config->clip.firstLine, config->clip.lastLine);
 
  }
  else
  {
    config->rolloffCfg.enable = FALSE;
  }
  #endif /* nQDSP_MODULE_VFE25_DEFINED */
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

  /* Calculate VFE command size */
  size = sizeof (VFE_UpdateType)/4;
  size = vfe_adjust_config_size(size);

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_UPDATE,
                              (void *) config, (int)size);
} /* VFE_Update */

/*===========================================================================

FUNCTION:    

  VFE_EOF_ACK1

DESCRIPTION: 

CALLERS:
  camera_process_release_frame ()
  camera_process_qdsp_output1_msg ()
  output1enc_ack1_criteria ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_EOF_Ack1 (VFE_EOF_Ack1Type ack)
{
  /* First check all fields in config to make sure that they are within range */
  if (!VFE_VerifyAck1Type (ack))
  {
    return CAMQDSP_INVALID_PARM;
  }

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_END_OF_FRAME_ACK1, 
                              (void *) &ack, 
                              sizeof (VFE_EOF_Ack1Type)/4);
} /* VFE_EOF_Ack1 */

/*===========================================================================

FUNCTION:    

  VFE_EOF_ACK2

DESCRIPTION: 

CALLERS:
  output2enc_ack2_criteria ()
  
SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_EOF_Ack2 (VFE_EOF_Ack2Type ack)
{
  /* First check all fields in config to make sure that they are within range */
  if (!VFE_VerifyAck2Type (ack))
  {
    return CAMQDSP_INVALID_PARM;
  }
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
  MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK,NULL,0);

  MP4E_DEBUG_PRINT_TIMELOG(
    MP4E_FORMAT_ARM2VFE_OUTPUT2_END_OF_FRAME_ACK);
#endif

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_END_OF_FRAME_ACK2, 
                                 (void *) &ack, 
                                 sizeof (VFE_EOF_Ack2Type)/4);
} /* VFE_EOF_Ack2 */


/*===========================================================================

FUNCTION:             VFE_EOF_SnapshotACK2

DESCRIPTION:  This function is for Raw snapshot or YCbCr snapshot capture
              In snapshot mode, we need to supply 4 pairs of valid address 
              to the VFE. Error checking is needed. 

CALLERS:
  
SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_EOF_SnapshotAck2 (VFE_EOF_Ack2Type ack)
{
  /* First check all fields in config to make sure that they are within range */
  if (!VFE_VerifySnapshotAck2Type (ack))
  {
    return CAMQDSP_INVALID_PARM;
  }
  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_END_OF_FRAME_ACK2, 
                                 (void *) &ack, 
                                 sizeof (VFE_EOF_Ack2Type)/4);
} /* VFE_EOF_SnapshotAck2 */
/*===========================================================================

FUNCTION:    

  VFE_AFSTATSSTART

DESCRIPTION: 

CALLERS:
  camera_process_start_focus ()
  camera_process_qdsp_output1_msg ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_AFStatsStart (VFE_AutoFocusConfigurationType afStats)
{
  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_VerifyAutoFocusConfigurationType (afStats)))
  {
    return CAMQDSP_INVALID_PARM;
  }

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_AF_STATS_START,
                              (void *) &afStats,
                              sizeof (VFE_AutoFocusConfigurationType)/4);
} /* VFE_AFStatsStart */

/*===========================================================================

FUNCTION:    

  VFE_AFSTATSSTOP

DESCRIPTION: 

CALLERS:
  camera_svcs_take_picture ()
  camera_process_stop ()
  camera_process_stop_preview ()
  camera_process_qdsp_af_stats_complete_msg ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global 

============================================================================*/
CAMQDSP_ReturnCodeType VFE_AFStatsStop (void)
{
  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_AF_STATS_STOP,
                              0,
                              0);
} /* VFE_AFStatsStop */

/*===========================================================================

FUNCTION:    

  VFE_AE_AWB_STATS

DESCRIPTION: 

CALLERS:
  camera_process_qdsp_output1_msg ()
  camera_process_qdsp_ae_awb_stats_complete_msg ()

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_AE_AWB_Stats (VFE_ExposureAndColorBalanceConfigurationType stats)
{
  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_VerifyExposureAndColorBalanceConfigurationType (stats)))
  {
    return CAMQDSP_INVALID_PARM;
  }

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_AE_AWB_STATS_REQ,
                              (void *) &stats,
                              sizeof (VFE_ExposureAndColorBalanceConfigurationType)/4);
} /* VFE_AE_AWB_Stats */

/*===========================================================================

FUNCTION:    

  VFE_HISTSTATS

DESCRIPTION: 

CALLERS:

SIDE EFFECTS: 

RETURN VALUE: 

USAGE:

SCOPE: 
  Global

============================================================================*/
CAMQDSP_ReturnCodeType VFE_HistStats (VFE_HistogramConfigurationType stats)
{
  /* Check to see if supported. */
  if (vfe_hw_features.chroma_histogram == FALSE)
  {
    return CAMQDSP_INVALID_CMD;
  }
  /* First check all fields in config to make sure that they are within range */
  if (!(VFE_VerifyHistogramConfigurationType (stats) ))
  {
    return CAMQDSP_INVALID_PARM;
  }

  /* Next, issue the actual command to the QDSP */
  return CAMQDSP_VFE_Command (CAMQDSP_CMD_HIST_STATS_REQ,
                              (void *) &stats,
                              sizeof (VFE_HistogramConfigurationType)/4);
} /* VFE_HistStats */

/*===========================================================================

FUNCTION:

  VFE_COPY_SENSOR_CONFIG

DESCRIPTION:
  Copy the sensor config to VFE config

CALLERS:
  camera_config_vfe ()

DEPENDENCIES:
  None

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global

============================================================================*/
boolean vfe_copy_sensor_config
(
  camsensor_op_mode_type        mode,
  /* VFE output dimenstion along output1 path.
   * Output1 is used for preview and thumbnail
   */
  uint16                        output1_width,
  uint16                        output1_height,
  /* VFE output dimenstion along output2 path.
   * Output2 is used for main image
   */
  uint16                        output2_width,
  uint16                        output2_height,
  const camsensor_static_params_type *camsensor_params /* Static Parameters */
)
{
  switch (mode) {
    case CAMSENSOR_OP_MODE_PREVIEW:
    case CAMSENSOR_OP_MODE_VIDEO:
      return vfe_copy_sensor_video_config(output1_width, output1_height, (camsensor_static_params_type *) camsensor_params);

    case CAMSENSOR_OP_MODE_SNAPSHOT:
      return vfe_copy_sensor_snapshot_config(output1_width, output1_height, output2_width, output2_height, (camsensor_static_params_type *) camsensor_params);

    case CAMSENSOR_OP_MODE_RAW_SNAPSHOT:
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
      return vfe_copy_sensor_raw_snapshot_config((camsensor_static_params_type *) camsensor_params);
    default:
      break;
  }
  return FALSE;
} /* vfe_copy_sensor_config */

/*===========================================================================

FUNCTION:

VFE_COPY_AEC_AWB_CONFIG

DESCRIPTION:
  Copy the auto exposure and auto white balance info from
  camsensor parameter table.

CALLERS:
  cam3a_config ()

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global

============================================================================*/
#ifndef FEATURE_CAMERA_YCBCR_ONLY
boolean vfe_copy_aec_awb_config
(
  VFE_ExposureAndColorBalanceConfigurationType *aec_config,
  const camsensor_static_params_type                 *camsensor_params /* Static Parameters */
)
{
  chromatix_wb_exp_stats_type bounding_box;

  aec_config->destinationAddress = (VFE_AddressType)&vfeStatDspOutputBuf;

  bounding_box = camsensor_params->chromatix_parms->wb_exp_stats_hw_rolloff[AWB_STATS_NORMAL_LIGHT];

  /* No need to assert as the field is also 8-bit field
   * ASSERT(IS_UINT8(bounding_box.y_max)); */
  aec_config->maximumYValue = bounding_box.y_max;
  /* No need to assert as the field is also 8-bit field
   * ASSERT(IS_UINT8(bounding_box.y_min)); */
  aec_config->minimumYValue = bounding_box.y_min;
  /* No need to assert as the field is also 8-bit field
   * ASSERT(IS_INT8(bounding_box.m1)); */
  aec_config->M1 = bounding_box.m1;
  /* No need to assert as the field is also 8-bit field
   * ASSERT(IS_INT8(bounding_box.m2)); */
  aec_config->M2 = bounding_box.m2;
  /* No need to assert as the field is also 8-bit field
   * ASSERT(IS_INT8(bounding_box.m3)); */
  aec_config->M3 = bounding_box.m3;
  /* No need to assert as the field is also 8-bit field
   * ASSERT(IS_INT8(bounding_box.m4)); */
  aec_config->M4 = bounding_box.m4;
  ASSERT(IS_INT12(bounding_box.c1));
  aec_config->C1 = bounding_box.c1;
  ASSERT(IS_INT12(bounding_box.c2));
  aec_config->C2 = bounding_box.c2;
  ASSERT(IS_INT12(bounding_box.c3));
  aec_config->C3 = bounding_box.c3;
  ASSERT(IS_INT12(bounding_box.c4));
  aec_config->C4 = bounding_box.c4;
  return TRUE;
} /* vfe_copy_aec_awb_config */
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
/*===========================================================================

FUNCTION:

VFE_COPY_AF_CONFIG

DESCRIPTION:
  Copy the auto focus info from camsensor parameter table.

CALLERS:
  cam3a_config ()

DEPENDENCIES:
  None

SIDE EFFECTS:

RETURN VALUE:
  TRUE if successful
  FALSE otherwise

USAGE:

SCOPE:
  Global
============================================================================*/
boolean vfe_copy_af_config
(
  VFE_AutoFocusConfigurationType *af_config,
  const camsensor_static_params_type   *camsensor_params /* Static Parameters */
)
{
  ASSERT(IS_UINT8(camsensor_params->vfeAFConfig.minimumY))
  af_config->minimumY = camsensor_params->vfeAFConfig.minimumY;
  ASSERT(IS_UINT8(camsensor_params->vfeAFConfig.maximumY))
  af_config->maximumY = camsensor_params->vfeAFConfig.maximumY;
  ASSERT(IS_UINT12(camsensor_params->vfeAFConfig.firstPixel))
  af_config->firstPixel = camsensor_params->vfeAFConfig.firstPixel;
  ASSERT(IS_UINT12(camsensor_params->vfeAFConfig.lastPixel))
  af_config->lastPixel = camsensor_params->vfeAFConfig.lastPixel;
  ASSERT(IS_UINT12(camsensor_params->vfeAFConfig.firstLine))
  af_config->firstLine = camsensor_params->vfeAFConfig.firstLine;
  ASSERT(IS_UINT12(camsensor_params->vfeAFConfig.lastLine))
  af_config->lastLine = camsensor_params->vfeAFConfig.lastLine;
  return TRUE;
} /* vfe_copy_af_config */

/*===========================================================================

FUNCTION      VFE_START

DESCRIPTION
              Get the vfe service started.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type vfe_start 
(
 const camsensor_static_params_type *camsensor_params
)
{
  /* default WB unit gain on vfeVideoConfig*/
  vfeVideoConfig.whiteBalance.Y_G_gain = 128; /* Q7 */
  vfeVideoConfig.whiteBalance.Cb_B_gain = 128; /* Q7 */
  vfeVideoConfig.whiteBalance.Cr_R_gain = 128; /* Q7 */
  /* default WB unit gain on vfeSnapshotConfig*/
  vfeSnapshotConfig.whiteBalance.Y_G_gain = 128; /* Q7 */
  vfeSnapshotConfig.whiteBalance.Cb_B_gain = 128; /* Q7 */
  vfeSnapshotConfig.whiteBalance.Cr_R_gain = 128; /* Q7 */
  /* default WB unit gain on vfeOfflineConfig*/
  vfeOfflineConfig.whiteBalance.Y_G_gain = 128; /* Q7 */
  vfeOfflineConfig.whiteBalance.Cb_B_gain = 128; /* Q7 */
  vfeOfflineConfig.whiteBalance.Cr_R_gain = 128; /* Q7 */
  /* default WB unit gain on vfeUpdate*/
  vfeUpdate.whiteBalance.Y_G_gain = 128; /* Q7 */
  vfeUpdate.whiteBalance.Cb_B_gain = 128; /* Q7 */
  vfeUpdate.whiteBalance.Cr_R_gain = 128; /* Q7 */

  /* Init the vfeUpdate struct to NOT skip frames when issued */
  vfeUpdate.output1SkipPattern.skipBitPattern  = 0x3FFFFFFF;
  vfeUpdate.output1SkipPattern.bitPatternWidth = 30;
  vfeUpdate.output2SkipPattern.skipBitPattern  = 0x3FFFFFFF;
  vfeUpdate.output2SkipPattern.bitPatternWidth = 30;

#ifdef FEATURE_CAMERA_YCBCR_ONLY
    vfeUpdate.colorCorrection.enable = VFE_Disable;
    vfeUpdate.colorConversion.enable = VFE_Disable;
#else 
  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
  {
    /* initialize color matrix by converting the values from floating to int */
    vfe_init_color_matrix(camsensor_params); 

    if(!vfe_color_matrix_ready)
    {
      MSG_ERROR ("Failed converting color matrix into Qnumber format", 0, 0, 0);
      return CAMERA_FAILED;
    }
  }

  /* Need to copy or downsize gamma table from device table to VFE table */
  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
    vfe_init_gamma_table(camsensor_params);

  /* Need to initialize the rolloff tables */
  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
  {
   
 #ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
 #else
    if (vfe_init_rolloff_tables(camsensor_params) == FALSE)
    {
      return CAMERA_FAILED;
    }
 #endif /* QDSP_MODULE_VFE25_DEFINED */
  }
  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
  {
 #ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
 #else
    if (vfe_init_3x3_asf_data(camsensor_params) == FALSE)
    {
      return CAMERA_FAILED;
    }
 #endif /* QDSP_MODULE_VFE25_DEFINED */
  }
  /* Init VFE update struct here to known defaults using
     sensors Chromatix Params.  Do this here because requests
     (via set_parm) to change them come in BEFORE the
     camera_config_vfe() function. */

  
  /* Must enable color conversion for Bayer sensors */
  vfeUpdate.colorConversion.enable = VFE_Enable;
  vfeUpdate.colorConversion.Ch0_max = 255;
  vfeUpdate.colorConversion.Ch1_max = 255;
  vfeUpdate.colorConversion.Ch2_max = 255;

  vfeUpdate.colorConversion.Ch0_min = 0;
  vfeUpdate.colorConversion.Ch1_min = 0;
  vfeUpdate.colorConversion.Ch2_min = 0;

  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
  {
    if (vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].q_factor == 10)
    {
      vfeUpdate.colorConversion.coefficient_Q_Factor = VFE_SIGNED_Q10;
    }
    else
    {
      return CAMERA_FAILED;
    }
  }  

  /* Convert from floating point param to Q10 numbers */
  /* Also note that Chromatix data is RGB, while the VFE uses GBR
    So RGB -> GBR == 123 -> 231
    A11 A12 A13    <-   A22 A23 A21
    A21 A22 A23    <-   A32 A33 A31
    A31 A32 A33    <-   A12 A13 A11  */
  vfeUpdate.colorConversion.A11 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A12;
  vfeUpdate.colorConversion.A12 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A13;
  vfeUpdate.colorConversion.A13 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A11;

  vfeUpdate.colorConversion.A21 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A22;
  vfeUpdate.colorConversion.A22 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A23;
  vfeUpdate.colorConversion.A23 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A21;

  vfeUpdate.colorConversion.A31 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A32;
  vfeUpdate.colorConversion.A32 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A33;
  vfeUpdate.colorConversion.A33 = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].A31;


  vfeUpdate.colorConversion.k0  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_y;
    //camsensor_chromatix_nx5300_struct.default_color_conversion.bias_y;
  vfeUpdate.colorConversion.k1  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cb;
  vfeUpdate.colorConversion.k2  = vfe_color_conversion_matrix[COLOR_CONVERSION_DEFAULT].bias_Cr;

  /* Only enable if/when AWB is run, but place
     sensor specific tuning params here */
  /* Enable max range (10 bit number) */
  vfeUpdate.colorCorrection.enable = VFE_Enable;
  vfeUpdate.colorCorrection.Ch0_max = 1023; /* G or Y */
  vfeUpdate.colorCorrection.Ch1_max = 1023; /* B or Cb */
  vfeUpdate.colorCorrection.Ch2_max = 1023; /* R or Cr */
  vfeUpdate.colorCorrection.Ch0_min = 0;
  vfeUpdate.colorCorrection.Ch1_min = 0;
  vfeUpdate.colorCorrection.Ch2_min = 0;

  /* Need to disable color correction/conversion for ycbcr sensors */
  if (vfe_input_ycbcr(camsensor_params->format) == TRUE)
  {
    vfeUpdate.colorCorrection.enable = VFE_Disable;
    vfeUpdate.colorConversion.enable = VFE_Disable;
  }

  if (vfe_input_ycbcr(camsensor_params->format) != TRUE)
  {
    if (vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].q_factor == 7)
    {
      vfeUpdate.colorCorrection.coefficient_Q_Factor = VFE_SIGNED_Q7;
    }
    else
    {
      return CAMERA_FAILED;
    }
  }

  vfeUpdate.colorCorrection.k0  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_g;
  vfeUpdate.colorCorrection.k1  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_b;
  vfeUpdate.colorCorrection.k2  = vfe_color_correction_matrix[COLOR_CORRECTION_NORMAL].bias_r;
#endif /* FEATURE_CAMERA_YCBCR_ONLY */
  return CAMERA_SUCCESS;

}

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_GET_COLOR_CORRECTION_MATRIX_PTR

DESCRIPTION
              Provide the pointer of color correction matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_3x3_rgb_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
camera_3x3_rgb_matrix_type* vfe_get_color_correction_matrix_ptr
(
 VFE_ColorCorrectionMatrixType mId 
)
#endif /* CAMERA_USES_CHROMATIX_0x203 */
{
  camera_3x3_rgb_matrix_type* ptr;

  if (vfe_color_matrix_ready)
  {
#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#endif /* FEATURE_CAMERA_USES_0x203 */
    ptr = &vfe_color_correction_matrix[mId];
  }
  else
  {
    ptr = NULL;
  }

  return ptr;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_GET_COLOR_CONVERSION_MATRIX_PTR

DESCRIPTION
              Provide the pointer of color conversion matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_3x3_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
camera_3x3_ycbcr_matrix_type* vfe_get_color_conversion_matrix_ptr
(
 VFE_ColorConversionMatrixType mId
)
{
  camera_3x3_ycbcr_matrix_type* ptr;

  if (vfe_color_matrix_ready)
  {
    ptr = &vfe_color_conversion_matrix[mId];
  }
  else
  {
    ptr = NULL;
  }

  return ptr;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_GET_SPECIAL_EFFECT_MATRIX_PTR

DESCRIPTION
              Provide the pointer of special effect matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_3x3_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
camera_3x3_ycbcr_matrix_type* vfe_get_special_effect_matrix_ptr
(
  VFE_SpecialEffectMatrixType mId
)
{
  camera_3x3_ycbcr_matrix_type* ptr;

  if (vfe_color_matrix_ready)
  {
    ptr = &vfe_special_effect[mId];
  }
  else
  {
    ptr = NULL;
  }

  return ptr;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_GET_WHITE_BALANCE_MATRIX_PTR

DESCRIPTION
              Provide the pointer of white balance matrix

DEPENDENCIES
  None

RETURN VALUE
  if matrix is ready, camsensor_1x3_matrix_type pointer
  otherwise, NULL

SIDE EFFECTS
  None

===========================================================================*/
camera_1x3_matrix_type* vfe_get_white_balance_matrix_ptr
(
 VFE_WhiteBalanceMatrixType mId
)
{
  camera_1x3_matrix_type* ptr;

  if (vfe_color_matrix_ready)
  {
    ptr = &vfe_white_balance[mId];
  }
  else
  {
    ptr = NULL;
  }

  return ptr;
}
#endif /* FEATURE_CAMERA_YCBCR_ONLY */

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION      VFE_PARSE_REGIONS_PER_AWB_AEC

DESCRIPTION
   Do inital parse of AEC AWB stats comming back from VFE
   
CALLERS:
  camera_3a_parse_and_implement_aec_awb ()

DEPENDENCIES
  None

RETURN VALUE

LIMITATIONS

SIDE EFFECTS
  None

===========================================================================*/
int32 vfe_parse_regions_per_awb_aec (void)
{
  int32 numRegions;
  int32 i;
  uint32 * current_region;

  /* No memory to complete this fn */

  if(vfeStatStruct == NULL)
  {
    MSG_HIGH ("cam3a parse aec awb failed, no memory", 0,0, 0);
    return -1;
  }

  if((VFE_NumberOfRegionsType)vfeExpWbStatDspInput.numberOfRegions == VFE_4x4)
  {
    numRegions = 16;
  }
#ifdef FEATURE_CAMERA_VFE_16X16_SUPPORT
  else if((VFE_NumberOfRegionsType)vfeExpWbStatDspInput.numberOfRegions == VFE_16x16)
  {
    numRegions = 256;
  }
#endif /* FEATURE_CAMERA_VFE_16X16_SUPPORT */
  else
  {
    numRegions = 64;
  }

  /* Translate packed 4 32 bit word per region struct comming from the VFE
    into more usable struct for microprocessor algorithms */

  /* up to 4k output of DSP from VFE block for AEC and
    AWB control */
  current_region = (uint32*)QDSP_vfeMicroDMAAddress;

  /*lint -save -e737 loss of sign in promotion from int to unsigned long */
  for( i = 0; i < numRegions; i++) /* Either 16 or 64 or 256 regions processed here */
  {
    /* 24 bits sum of Y. */
    vfeStatStruct->SY[i] = ((*(current_region+0)) & 0xFFFFFF);

    /* 24 bits sum of Y that meets boundary conditions */
    vfeStatStruct->SY1[i] = ((*(current_region+1)) & 0xFFFFFF);

    /* Assemble Sub Cb from two words to produce the 24 bit value */
    vfeStatStruct->SCb[i] = (((*(current_region+1)) & 0xFF000000) >>24) |
      (((*(current_region+2)) & 0xFFFF) <<8);

    /* Assemble Sub Cr from two words to produce the 24 bit value */
    vfeStatStruct->SCr[i] = (((*(current_region+2)) & 0xFFFF0000) >>16) |
      (((*(current_region+3)) & 0xFF) <<16);

    /* Number of pixels from this region that meet the inequalities */
    vfeStatStruct->NSCb[i] = (((*(current_region+3)) & 0xFFFF0000) >>16);

    current_region += 4; /* each region is 4 32 bit words or 16 bytes long */
    /* increment int32 pointer to next region      */
  }
  /*lint -restore */

  return numRegions;
} 

/*===========================================================================

FUNCTION      VFE_UPDATE_AWB_STATS_CONFIG

DESCRIPTION
  set updated awb stats config in vfeExpWbStatDspInput
   
CALLERS:
  camera_awb_do_awb_advanced_grey_world_algo()

===========================================================================*/
boolean vfe_update_awb_stats_config(chromatix_wb_exp_stats_type bounding_box)
{
  /*lint -save -e685 -e730 -e718 -e746*/
  ASSERT(CAM3A_IS_UINT8(bounding_box.y_max));
  vfeExpWbStatDspInput.maximumYValue = bounding_box.y_max;
  ASSERT(CAM3A_IS_UINT8(bounding_box.y_min));
  vfeExpWbStatDspInput.minimumYValue = bounding_box.y_min;
  ASSERT(CAM3A_IS_INT8(bounding_box.m1));
  vfeExpWbStatDspInput.M1 = bounding_box.m1;
  ASSERT(CAM3A_IS_INT8(bounding_box.m2));
  vfeExpWbStatDspInput.M2 = bounding_box.m2;
  ASSERT(CAM3A_IS_INT8(bounding_box.m3));
  vfeExpWbStatDspInput.M3 = bounding_box.m3;
  ASSERT(CAM3A_IS_INT8(bounding_box.m4));
  vfeExpWbStatDspInput.M4 = bounding_box.m4;
  ASSERT(CAM3A_IS_INT12(bounding_box.c1));
  vfeExpWbStatDspInput.C1 = bounding_box.c1;
  ASSERT(CAM3A_IS_INT12(bounding_box.c2));
  vfeExpWbStatDspInput.C2 = bounding_box.c2;
  ASSERT(CAM3A_IS_INT12(bounding_box.c3));
  vfeExpWbStatDspInput.C3 = bounding_box.c3;
  ASSERT(CAM3A_IS_INT12(bounding_box.c4));
  vfeExpWbStatDspInput.C4 = bounding_box.c4;
  /*lint -restore */
  return TRUE;
}

/*===========================================================================

FUNCTION      VFE_CONFIG_AWB_PER_CHROMATIX

DESCRIPTION
  set chromatix info
   
CALLERS:
  camera_awb_config()

===========================================================================*/
void vfe_config_awb_per_chromatix(camsensor_static_params_type * camsensorParamsPtr)
{
  int i;

  /* black_level value is used for AWB and AEC */
  /*lint -save -e50 -e713*/
#ifndef QDSP_MODULE_VFE25_DEFINED
  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #else
  cam3a_awb_state.black_level = camsensorParamsPtr->chromatix_parms->black_level_offset;
  #endif
#endif /* QDSP_MODULE_VFE25_DEFINED */
  
  /*lint -restore */
  /* bounding box is used for AWB and AEC */
  cam3a_awb_state.bounding_box_low_light 
    = camsensorParamsPtr->chromatix_parms->wb_exp_stats_hw_rolloff[AWB_STATS_LOW_LIGHT];
  cam3a_awb_state.bounding_box_normal 
    = camsensorParamsPtr->chromatix_parms->wb_exp_stats_hw_rolloff[AWB_STATS_NORMAL_LIGHT];
  cam3a_awb_state.bounding_box_outdoor 
    = camsensorParamsPtr->chromatix_parms->wb_exp_stats_hw_rolloff[AWB_STATS_OUTDOOR];

  cam3a_awb_state.white_y_min_percent = camsensorParamsPtr->chromatix_parms->awb_white_world_y_min_ratio;
#if defined (CAMERA_USES_CHROMATIX_0x201) ||defined (CAMERA_USES_CHROMATIX_0x203)
  cam3a_awb_state.color_correction_bias = 
    vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->color_correction_global_gain,7);
#else
  cam3a_awb_state.color_correction_bias = 
    vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->color_correction_bias,7);
#endif

  /* Setting AWB min/max gains */
  cam3a_awb_state.awb_min_wb.r_gain 
    = vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->awb_min_gains.r_gain,7);
  cam3a_awb_state.awb_min_wb.g_gain 
    = vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->awb_min_gains.g_gain,7);
  cam3a_awb_state.awb_min_wb.b_gain 
    = vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->awb_min_gains.b_gain,7);
  cam3a_awb_state.awb_max_wb.r_gain 
    = vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->awb_max_gains.r_gain,7);
  cam3a_awb_state.awb_max_wb.g_gain 
    = vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->awb_max_gains.g_gain,7);
  cam3a_awb_state.awb_max_wb.b_gain 
    = vfe_convert_float_to_Qfactor(camsensorParamsPtr->chromatix_parms->awb_max_gains.b_gain,7);


  cam3a_awb_state.stored_prev_r_gain = 
    camsensorParamsPtr->chromatix_parms->chromatix_tl84_white_balance.r_gain;
  cam3a_awb_state.stored_prev_g_gain =
    camsensorParamsPtr->chromatix_parms->chromatix_tl84_white_balance.g_gain;
  cam3a_awb_state.stored_prev_b_gain =
    camsensorParamsPtr->chromatix_parms->chromatix_tl84_white_balance.b_gain;

  cam3a_awb_state.saturated_color_conversion_factor = 
    camsensorParamsPtr->chromatix_parms->saturated_color_conversion_factor;
  cam3a_awb_state.snow_blue_gain_adj_ratio = 
    camsensorParamsPtr->chromatix_parms->snow_blue_gain_adj_ratio;
  cam3a_awb_state.beach_blue_gain_adj_ratio = 
    camsensorParamsPtr->chromatix_parms->beach_blue_gain_adj_ratio;

  /* Set Adv. Grey World 1/gain tables for each lighting condition    */
  /* Just copy the pointer over */
  for(i=0;i<CAM3A_AGW_AWB_NUMBER_OF_REFERENCE_POINT;i++)
  {
    cam3a_agw_ptr->red_gain_table[i] = 
      camsensorParamsPtr->chromatix_parms->awb_reference_hw_rolloff[i].red_gain;
    cam3a_agw_ptr->blue_gain_table[i] = 
      camsensorParamsPtr->chromatix_parms->awb_reference_hw_rolloff[i].blue_gain;

    cam3a_agw_ptr->red_gain_adj[i] = 
      camsensorParamsPtr->chromatix_parms->awb_reference_hw_rolloff[i].red_gain_adj;
    cam3a_agw_ptr->blue_gain_adj[i] = 
      camsensorParamsPtr->chromatix_parms->awb_reference_hw_rolloff[i].blue_gain_adj;
  }

  cam3a_agw_ptr->outdoor_sample_influence = camsensorParamsPtr->chromatix_parms->sample_influence.outdoor_influence;
  cam3a_agw_ptr->indoor_sample_influence = camsensorParamsPtr->chromatix_parms->sample_influence.indoor_influence;

  cam3a_agw_ptr->compact_cluster_r2 = camsensorParamsPtr->chromatix_parms->compact_cluster_r2;
  cam3a_agw_ptr->compact_cluster_to_ref_point_r2 = camsensorParamsPtr->chromatix_parms->compact_cluster_to_ref_point_r2;
  cam3a_agw_ptr->a_cluster_threshold = camsensorParamsPtr->chromatix_parms->a_cluster_threshold;
  cam3a_agw_ptr->f_cluster_threshold = camsensorParamsPtr->chromatix_parms->f_cluster_threshold;
  cam3a_agw_ptr->day_cluster_threshold = camsensorParamsPtr->chromatix_parms->day_cluster_threshold;
  cam3a_agw_ptr->outdoor_green_threshold = camsensorParamsPtr->chromatix_parms->outdoor_green_threshold;
  cam3a_agw_ptr->outdoor_green_threshold_bright_F = camsensorParamsPtr->chromatix_parms->outdoor_green_threshold_bright_F;
  cam3a_agw_ptr->outdoor_green_threshold_dark_F = camsensorParamsPtr->chromatix_parms->outdoor_green_threshold_dark_F;
  cam3a_agw_ptr->day_cluster_threshold_for_F = camsensorParamsPtr->chromatix_parms->day_cluster_threshold_for_F;
  cam3a_agw_ptr->white_balance_allow_fline = camsensorParamsPtr->chromatix_parms->white_balance_allow_fline;
  cam3a_agw_ptr->outdoor_valid_sample_cnt_threshold = camsensorParamsPtr->chromatix_parms->outdoor_valid_sample_cnt_threshold;
  cam3a_agw_ptr->outdoor_green_upper_threshold = camsensorParamsPtr->chromatix_parms->outdoor_green_upper_threshold;
  cam3a_agw_ptr->r2_threshold = camsensorParamsPtr->chromatix_parms->r2_threshold;
  cam3a_agw_ptr->outdoor_green_threshold_bright_A = camsensorParamsPtr->chromatix_parms->outdoor_green_threshold_bright_A;
  cam3a_agw_ptr->outdoor_green_threshold_dark_A = camsensorParamsPtr->chromatix_parms->outdoor_green_threshold_dark_A;
  cam3a_agw_ptr->day_cluster_threshold_for_A = camsensorParamsPtr->chromatix_parms->day_cluster_threshold_for_A;

  cam3a_agw_ptr->green_offset_rg = camsensorParamsPtr->chromatix_parms->green_offset_rg;
  cam3a_agw_ptr->green_offset_bg = camsensorParamsPtr->chromatix_parms->green_offset_bg;

  cam3a_agw_ptr->awb_agw_grid_dist_2_threshold = camsensorParamsPtr->chromatix_parms->outlier_distance;
  for(i=0;i<CAM3A_AGW_AWB_NUMBER_OF_LIGHTING_CONDITION;i++)
  {
    cam3a_agw_ptr->awb_weight_vector[i][0] 
      = camsensorParamsPtr->chromatix_parms->awb_weight_vector[i].indoor_weight;
    cam3a_agw_ptr->awb_weight_vector[i][1] 
      = camsensorParamsPtr->chromatix_parms->awb_weight_vector[i].outdoor_weight;
    cam3a_agw_ptr->awb_weight_vector[i][2] 
      = camsensorParamsPtr->chromatix_parms->awb_weight_vector[i].inoutdoor_weight;
  }
}

/*===========================================================================

FUNCTION      VFE_UPDATE_WITH_NEW_GAIN_TO_COLOR_CORRECTION_OR_WB

DESCRIPTION:
  Apply wb to either color correction matrix or white balance config.

CALLERS:
  camera_awb_apply_new_wb_gains ()
  camera_awb_update_color_correction_or_wb ()

============================================================================*/
/*lint -save -e818 -e702 sourceCC could be declared as pointing to const */
void vfe_update_with_new_gain_to_color_correction_or_wb (
  camera_1x3_matrix_type wb,
  int16 blackOffset,
  camera_3x3_rgb_matrix_type *sourceCC
  )
{
  int a11, a12, a13, a21, a22, a23, a31, a32, a33;

  if(sourceCC == NULL) return;

  /* VFE H/W has independed RGB gains module. */
  /* Ensure that convertion to Q7 format happened already */
  /* Need to modify camsensor drivers to use Q7 instead of Q12
   * conversion for white balance parameters.
   */
  vfeUpdate.whiteBalance.Y_G_gain  = (uint16)wb.g_gain;
  vfeUpdate.whiteBalance.Cb_B_gain = (uint16)wb.b_gain;
  vfeUpdate.whiteBalance.Cr_R_gain = (uint16)wb.r_gain;

  /* White balance gain is not in axx, but need to be in K */
  a11 = sourceCC->A11;
  a12 = sourceCC->A12;
  a13 = sourceCC->A13;

  a21 = sourceCC->A21;
  a22 = sourceCC->A22;
  a23 = sourceCC->A23;

  a31 = sourceCC->A31;
  a32 = sourceCC->A32;
  a33 = sourceCC->A33;

  /* Black level offset and bias */
  /* wb and sourceCC have Q7 values */
  /*lint -save -e704*/
  snapshot_offset_k0 = (int)(((wb.r_gain*a21 + wb.g_gain*a22 + wb.b_gain*a23)*blackOffset) >> 14);
  snapshot_offset_k1 = (int)(((wb.r_gain*a31 + wb.g_gain*a32 + wb.b_gain*a33)*blackOffset) >> 14);
  snapshot_offset_k2 = (int)(((wb.r_gain*a11 + wb.g_gain*a12 + wb.b_gain*a13)*blackOffset) >> 14);

  vfeUpdate.colorCorrection.k0 = snapshot_offset_k0 + sourceCC->bias_g;
  vfeUpdate.colorCorrection.k1 = snapshot_offset_k1 + sourceCC->bias_b;
  vfeUpdate.colorCorrection.k2 = snapshot_offset_k2 + sourceCC->bias_r;
  vfeUpdate.colorCorrection.enable = (VFE_EnableType)(camsensor_static_params[camera_asi].colorCorrection.enable);
  /*lint -restore*/

  /* Now apply new CC matrix to VFE params, must put in VFE order */
  /* Also note that Chromatix data is RGB, while the VFE uses GBR
  So RGB -> GBR == 123 -> 231
   A11 A12 A13   <-  A22 A23 A21
   A21 A22 A23   <-  A32 A33 A31
   A31 A32 A33   <-  A12 A13 A11  */
  vfeUpdate.colorCorrection.A11 = (0x0FFF & a22);
  vfeUpdate.colorCorrection.A12 = (0x0FFF & a23);
  vfeUpdate.colorCorrection.A13 = (0x0FFF & a21);

  vfeUpdate.colorCorrection.A21 = (0x0FFF & a32);
  vfeUpdate.colorCorrection.A22 = (0x0FFF & a33);
  vfeUpdate.colorCorrection.A23 = (0x0FFF & a31);

  vfeUpdate.colorCorrection.A31 = (0x0FFF & a12);
  vfeUpdate.colorCorrection.A32 = (0x0FFF & a13);
  vfeUpdate.colorCorrection.A33 = (0x0FFF & a11);
} /* cam3a_apply_new_gain_to_color_correction_or_wb */
/*lint -restore */

/*===========================================================================

FUNCTION:  VFE_APPLY_NEW_GAIN_TO_COLOR_CORRECTION_OR_WB_FOR_SNAPSHOT

DESCRIPTION: 
  applies the digital gain on the color correction matrix.

DEPENDENCIES:

SIDE EFFECTS: 

RETURN VALUE: 
  NONE
============================================================================*/
/*lint -save -e702 -e818*/
void vfe_apply_new_gain_to_color_correction_or_wb_for_snapshot (
  camera_1x3_matrix_type wb,
  int16 blackOffset,
  camera_3x3_rgb_matrix_type *sourceCC
  )
{
  int a11, a12, a13, a21, a22, a23, a31, a32, a33;

  if(sourceCC == NULL) return;

  /* VFE H/W has independed RGB gains module. */
  /* Ensure that convertion to Q7 format happened already */
  /* Need to modify camsensor drivers to use Q7 instead of Q12
   * conversion for white balance parameters.
   */
  /* wb has Q7 wb gains */
  vfeSnapshotConfig.whiteBalance.Y_G_gain  = (uint16)wb.g_gain;
  vfeSnapshotConfig.whiteBalance.Cb_B_gain = (uint16)wb.b_gain;
  vfeSnapshotConfig.whiteBalance.Cr_R_gain = (uint16)wb.r_gain;

  /* White balance gain is not in axx, but need to be in K */
  a11 = sourceCC->A11;
  a12 = sourceCC->A12;
  a13 = sourceCC->A13;

  a21 = sourceCC->A21;
  a22 = sourceCC->A22;
  a23 = sourceCC->A23;

  a31 = sourceCC->A31;
  a32 = sourceCC->A32;
  a33 = sourceCC->A33;

  /* Black level offset and bias */
  /* wb and sourceCC have Q7 values */
  /*lint -save -e713 -e704*/
  vfeSnapshotConfig.colorCorrection.k0 =
    (int)(((wb.r_gain*a21 + wb.g_gain*a22 + wb.b_gain*a23)*blackOffset) >> 14) +
    sourceCC->bias_g;
  vfeSnapshotConfig.colorCorrection.k1 =
    (int)(((wb.r_gain*a31 + wb.g_gain*a32 + wb.b_gain*a33)*blackOffset) >> 14) +
    sourceCC->bias_b;
  vfeSnapshotConfig.colorCorrection.k2 =
    (int)(((wb.r_gain*a11 + wb.g_gain*a12 + wb.b_gain*a13)*blackOffset) >> 14) +
    sourceCC->bias_r;
  /*lint -restore*/

  /* Now apply new CC matrix to VFE params, must put in VFE order */
  /* Also note that Chromatix data is RGB, while the VFE uses GBR
  So RGB -> GBR == 123 -> 231
   A11 A12 A13   <-  A22 A23 A21
   A21 A22 A23   <-  A32 A33 A31
   A31 A32 A33   <-  A12 A13 A11  */
  vfeSnapshotConfig.colorCorrection.A11 = (0x0FFF & a22);
  vfeSnapshotConfig.colorCorrection.A12 = (0x0FFF & a23);
  vfeSnapshotConfig.colorCorrection.A13 = (0x0FFF & a21);

  vfeSnapshotConfig.colorCorrection.A21 = (0x0FFF & a32);
  vfeSnapshotConfig.colorCorrection.A22 = (0x0FFF & a33);
  vfeSnapshotConfig.colorCorrection.A23 = (0x0FFF & a31);

  vfeSnapshotConfig.colorCorrection.A31 = (0x0FFF & a12);
  vfeSnapshotConfig.colorCorrection.A32 = (0x0FFF & a13);
  vfeSnapshotConfig.colorCorrection.A33 = (0x0FFF & a11);
} /* camera_svcs_apply_digital_gain_to_cc */
/*lint -restore */

/*===========================================================================

FUNCTION      VFE_SET_COLOR_CONVERSION

DESCRIPTION:
  Apply color conversion matrix for BSM settings.

CALLERS:
  camera_awb_set_wb_per_bsm ()

============================================================================*/
void vfe_set_color_conversion(
  camera_3x3_ycbcr_matrix_type * matrix_ptr,
  float saturation
  )
{
  if(matrix_ptr->q_factor == 10)
  {
    vfeUpdate.colorConversion.coefficient_Q_Factor = VFE_SIGNED_Q10;
    vfeUpdate.colorConversion.enable = VFE_Enable;
  }
  else
  {
    vfeUpdate.colorConversion.enable = VFE_Disable;
  }

  /* Convert from floating point param in chromatix color
     convert matrix to Q10 number for the VFE */
  /* Also note that Chromatix data is RGB, while the VFE uses GBR
     So RGB -> GBR == 123 -> 231
     A11 A12 A13    <-   A12 A13 A11
     A21 A22 A23    <-   A22 A23 A21
     A31 A32 A33    <-   A32 A33 A31  */
  vfeUpdate.colorConversion.A11 = matrix_ptr->A12;
  vfeUpdate.colorConversion.A12 = matrix_ptr->A13;
  vfeUpdate.colorConversion.A13 = matrix_ptr->A11;

  vfeUpdate.colorConversion.A21 = (int)(matrix_ptr->A22 * saturation) & 0xFFFF;
  vfeUpdate.colorConversion.A22 = (int)(matrix_ptr->A23 * saturation) & 0xFFFF;
  vfeUpdate.colorConversion.A23 = (int)(matrix_ptr->A21 * saturation) & 0xFFFF;

  vfeUpdate.colorConversion.A31 = (int)(matrix_ptr->A32 * saturation) & 0xFFFF;
  vfeUpdate.colorConversion.A32 = (int)(matrix_ptr->A33 * saturation) & 0xFFFF;
  vfeUpdate.colorConversion.A33 = (int)(matrix_ptr->A31 * saturation) & 0xFFFF;

  vfeUpdate.colorConversion.k0  = matrix_ptr->bias_y;
  vfeUpdate.colorConversion.k1  = matrix_ptr->bias_Cb;
  vfeUpdate.colorConversion.k2  = matrix_ptr->bias_Cr;
}

/*===========================================================================

FUNCTION      VFE_UPDATE_COLOR_CONVERSION_FOR_WB

DESCRIPTION:
  Update color converstion matrix for wb.

CALLERS:
  camera_awb_update_color_conversion_for_wb ()

============================================================================*/
void vfe_update_color_conversion_for_wb(
  camera_3x3_ycbcr_matrix_type * newCConversion,
  float aggressiveness,
  float saturation
  )
{
  camera_3x3_ycbcr_matrix_type currentCConversion;

  /* Get sign correct on this 12 bit number */
  /*lint -save -e701 shift left of signed quantity */
  currentCConversion.A12 = (vfeUpdate.colorConversion.A11 << 20);
  currentCConversion.A13 = (vfeUpdate.colorConversion.A12 << 20);
  currentCConversion.A11 = (vfeUpdate.colorConversion.A13 << 20);

  currentCConversion.A22 = (vfeUpdate.colorConversion.A21 << 20);
  currentCConversion.A23 = (vfeUpdate.colorConversion.A22 << 20);
  currentCConversion.A21 = (vfeUpdate.colorConversion.A23 << 20);

  currentCConversion.A32 = (vfeUpdate.colorConversion.A31 << 20);
  currentCConversion.A33 = (vfeUpdate.colorConversion.A32 << 20);
  currentCConversion.A31 = (vfeUpdate.colorConversion.A33 << 20);
  /*lint -restore */

  /*lint -save -e704 shift right of signed quantity */
  currentCConversion.A12 >>= 20;
  currentCConversion.A13 >>= 20;
  currentCConversion.A11 >>= 20;

  currentCConversion.A22 >>= 20;
  currentCConversion.A23 >>= 20;
  currentCConversion.A21 >>= 20;

  currentCConversion.A32 >>= 20;
  currentCConversion.A33 >>= 20;
  currentCConversion.A31 >>= 20;
  /*lint -restore */

  /* Now pull the current toward the new with a aggressiveness bias
   * number */
  currentCConversion.A11 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A11) +
    (aggressiveness * (float)newCConversion->A11));
  currentCConversion.A12 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A12) +
    (aggressiveness * (float)newCConversion->A12));
  currentCConversion.A13 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A13) +
    (aggressiveness * (float)newCConversion->A13));

  currentCConversion.A21 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A21) +
    (aggressiveness * (float)newCConversion->A21 * saturation));
  currentCConversion.A22 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A22) +
    (aggressiveness * (float)newCConversion->A22 * saturation));
  currentCConversion.A23 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A23) +
    (aggressiveness * (float)newCConversion->A23 * saturation));

  currentCConversion.A31 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A31) +
    (aggressiveness * (float)newCConversion->A31 * saturation));
  currentCConversion.A32 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A32) +
    (aggressiveness * (float)newCConversion->A32 * saturation));
  currentCConversion.A33 = 
    (int32)(((1.0-aggressiveness) * (float)currentCConversion.A33) +
    (aggressiveness * (float)newCConversion->A33 * saturation));

  /* Now apply new WB/CC matrix to VFE params, must put in VFE order */
  /* Also note that Chromatix data is RGB, while the VFE uses GBR
  So RGB -> GBR == 123 -> 231
    A11 A12 A13   <-  A22 A23 A21
    A21 A22 A23   <-  A32 A33 A31
    A31 A32 A33   <-  A12 A13 A11  */
  vfeUpdate.colorConversion.A11 = 0x0FFF & currentCConversion.A12;
  vfeUpdate.colorConversion.A12 = 0x0FFF & currentCConversion.A13;
  vfeUpdate.colorConversion.A13 = 0x0FFF & currentCConversion.A11;

  vfeUpdate.colorConversion.A21 = 0x0FFF & currentCConversion.A22;
  vfeUpdate.colorConversion.A22 = 0x0FFF & currentCConversion.A23;
  vfeUpdate.colorConversion.A23 = 0x0FFF & currentCConversion.A21;

  vfeUpdate.colorConversion.A31 = 0x0FFF & currentCConversion.A32;
  vfeUpdate.colorConversion.A32 = 0x0FFF & currentCConversion.A33;
  vfeUpdate.colorConversion.A33 = 0x0FFF & currentCConversion.A31;
  vfeUpdate.colorConversion.enable = (VFE_EnableType)(camsensor_static_params[camera_asi].colorConversion.enable);
}

/*===========================================================================

FUNCTION      VFE_APPLY_DIGITAL_GAIN_PER_AEC

DESCRIPTION:
  Update color converstion matrix for wb.

CALLERS:
  camera_aec_apply_digital_gain_for_preview ()

============================================================================*/
void vfe_apply_digital_gain_per_aec(
  VFE_ColorCorrectionType *packed_cc_ptr,
  float temp_gain
  )
{
  camera_3x3_rgb_matrix_type unpacked_CCorrection;
  
  unpacked_CCorrection = vfe_unpack_ColorCorrection(packed_cc_ptr);

  unpacked_CCorrection.A11 = (int16)((float)unpacked_CCorrection.A11 * temp_gain);
  unpacked_CCorrection.A12 = (int16)((float)unpacked_CCorrection.A12 * temp_gain);
  unpacked_CCorrection.A13 = (int16)((float)unpacked_CCorrection.A13 * temp_gain);
  unpacked_CCorrection.A21 = (int16)((float)unpacked_CCorrection.A21 * temp_gain);
  unpacked_CCorrection.A22 = (int16)((float)unpacked_CCorrection.A22 * temp_gain);
  unpacked_CCorrection.A23 = (int16)((float)unpacked_CCorrection.A23 * temp_gain);
  unpacked_CCorrection.A31 = (int16)((float)unpacked_CCorrection.A31 * temp_gain);
  unpacked_CCorrection.A32 = (int16)((float)unpacked_CCorrection.A32 * temp_gain);
  unpacked_CCorrection.A33 = (int16)((float)unpacked_CCorrection.A33 * temp_gain);

  packed_cc_ptr->A11 = unpacked_CCorrection.A11 & 0x0FFF;
  packed_cc_ptr->A12 = unpacked_CCorrection.A12 & 0x0FFF;
  packed_cc_ptr->A13 = unpacked_CCorrection.A13 & 0x0FFF;
  packed_cc_ptr->A21 = unpacked_CCorrection.A21 & 0x0FFF;
  packed_cc_ptr->A22 = unpacked_CCorrection.A22 & 0x0FFF;
  packed_cc_ptr->A23 = unpacked_CCorrection.A23 & 0x0FFF;
  packed_cc_ptr->A31 = unpacked_CCorrection.A31 & 0x0FFF;
  packed_cc_ptr->A32 = unpacked_CCorrection.A32 & 0x0FFF;
  packed_cc_ptr->A33 = unpacked_CCorrection.A33 & 0x0FFF;
}

/*===========================================================================

FUNCTION      VFE_CONFIG_AEC_PER_CHROMATIX

DESCRIPTION
  set chromatix info
   
CALLERS:
  camera_aec_config()

===========================================================================*/
void vfe_config_aec_per_chromatix(camsensor_static_params_type * camsensorParamsPtr)
{
  /* original indoor/outdoor index for AFR adjustment */
 #if defined (CAMERA_USES_CHROMATIX_0x201) || defined (CAMERA_USES_CHROMATIX_0x203)
  cam3a_aec_state.indoor_index_init = (uint16)(camsensorParamsPtr->chromatix_parms->awb_indoor_index);
  cam3a_aec_state.outdoor_index_init = (uint16)(camsensorParamsPtr->chromatix_parms->awb_outdoor_index);
  cam3a_aec_state.outdoor_gamma_index = (camsensorParamsPtr->chromatix_parms->awb_outdoor_index)/2;
#else
  cam3a_aec_state.indoor_index_init = (uint16)(camsensorParamsPtr->chromatix_parms->indoor_index);
  cam3a_aec_state.outdoor_index_init = (uint16)(camsensorParamsPtr->chromatix_parms->outdoor_index);
  cam3a_aec_state.outdoor_gamma_index = camsensorParamsPtr->chromatix_parms->outdoor_gamma_index;
#endif /* defined (CAMERA_USES_CHROMATIX_0x201) || defined (CAMERA_USES_CHROMATIX_0x203) */

  cam3a_aec_state.exposure_index_adj_step = 
    camsensorParamsPtr->chromatix_parms->exposure_index_adj_step;

  /* tolerance indicates how close we need to get to the target      */
  cam3a_aec_state.luma_tolerance = (uint8)camsensorParamsPtr->chromatix_parms->luma_tolerance;

  /* Save the default Exposure Value */
  /*lint -save -e40 -e63*/
  cam3a_aec_state.default_luma_target = camsensorParamsPtr->chromatix_parms->default_luma_target;
  cam3a_aec_state.outdoor_luma_target = camsensorParamsPtr->chromatix_parms->outdoor_luma_target;
  cam3a_aec_state.lowlight_luma_target = camsensorParamsPtr->chromatix_parms->lowlight_luma_target;
  cam3a_aec_state.backlight_luma_target = camsensorParamsPtr->chromatix_parms->backlight_luma_target;
  /*lint -restore*/

  /* Default exposure table index value from older target implementations */
  /* OLDER TARGETS: exp_index = MAX_EXP_INDEX * 3 / 4; */
  /* Subract 1 to get max exposure index */
  #ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
  #else
  cam3a_aec_state.exp_index =
    (int32)(((camsensorParamsPtr->chromatix_parms->num_exposure_values-1)*3)/4);
  #endif /* CAMERA_USES_CHROMATIX_0x203 */

  cam3a_aec_state.one_to_two_frame_hjr_offset_init = camsensorParamsPtr->chromatix_parms->one_to_two_frame_hjr_offset;

#ifdef CAMERA_USES_CHROMATIX_0x203
#error code not present
#else
  cam3a_aec_state.num_exposure_values_init = camsensorParamsPtr->chromatix_parms->num_exposure_values;
  cam3a_aec_state.exposure_table_init = camsensorParamsPtr->chromatix_parms->exposure_table;
#endif /*CAMERA_USES_CHROMATIX_0x203 */


  
}

/*===========================================================================

FUNCTION      VFE_RESTORE_CONFIG_FOR_PREVIEW

DESCRIPTION
  restore wb gains and color correction matrix for preview restart
   
CALLERS:
  camera_restore_config_for_preview()

===========================================================================*/
void vfe_restore_config_for_preview(void)
{
  vfeVideoConfig.colorCorrection = vfeUpdate.colorCorrection;
  vfeVideoConfig.whiteBalance = vfeUpdate.whiteBalance;
}

#endif /* nFEATURE_CAMERA_YCBCR_ONLY */

/*===========================================================================

FUNCTION    VFE_CALCULATE_AF_WINDOW

DESCRIPTION
  Calculate AF window size based on VFE clipping window size.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void vfe_calculate_af_window
(
  uint32 vfe_clip_width,
  uint32 vfe_clip_height
)
{
  uint32 tmpFirst, tmpLast, tmpDelta;
  
  /* AF window should be in center of clipping window
     Width & Height of AF window is according to camctrl_tbl.clip_to_af_ratio */
  /* check the width */
  tmpFirst = (vfe_clip_width - (vfe_clip_width / camctrl_tbl.clip_to_af_ratio)) >> 1;
  tmpLast  = tmpFirst + (vfe_clip_width / camctrl_tbl.clip_to_af_ratio);
  /* convert tmp to VFE subsampling by 2 format*/
  tmpFirst = tmpFirst >> 1;
  tmpLast  = tmpLast  >> 1;
  /* after VFE subsampling, AF window width must be <= 508, use 506 to be safe */
  tmpDelta = tmpLast - tmpFirst;
  if (tmpDelta > (506 - 1))
  {
    tmpFirst = tmpFirst + ((tmpDelta - (506 - 1)) >> 1);
    tmpLast  = tmpLast  - ((tmpDelta - (506 - 1)) >> 1);
  }
  /* after VFE subsampling, make sure the 1st pixel is even and the last pixel is odd
     so that the subsampled AF window width is even */
  tmpFirst = (tmpFirst >> 1) << 1;
  tmpLast  = tmpLast | 1;
  /* convert tmp back to pre-subsampling format */
  vfeAfStatDspInput.firstPixel = (uint16)(tmpFirst << 1);
  vfeAfStatDspInput.lastPixel  = (uint16)(tmpLast << 1);

  /* check the height */
  tmpFirst = (vfe_clip_height - (vfe_clip_height / camctrl_tbl.clip_to_af_ratio)) >> 1;
  tmpLast  = tmpFirst + (vfe_clip_height / camctrl_tbl.clip_to_af_ratio);
  /* convert tmp to VFE subsampling by 2 format*/
  tmpFirst = tmpFirst >> 1;
  tmpLast  = tmpLast  >> 1;
  /* after VFE subsampling, AF window height must be <= 508, use 506 to be safe */
  tmpDelta = tmpLast - tmpFirst;
  if (tmpDelta > (506 - 1))
  {
    tmpFirst = tmpFirst + ((tmpDelta - (506 - 1)) >> 1);
    tmpLast  = tmpLast  - ((tmpDelta - (506 - 1)) >> 1);
  }
  /* convert tmp back to pre-subsampling format */
  vfeAfStatDspInput.firstLine = (uint16)(tmpFirst << 1);
  vfeAfStatDspInput.lastLine  = (uint16)(tmpLast << 1);

  camera_pending_stats_cmds |= PENDING_AF_STATS_CMD_TO_ISSUE;
  /* 1st AF start command will be issued after vfe_capture 
     later ones will be issued at output1 messages */
}

#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*===========================================================================

FUNCTION:
  vfe_update_3x3_asf

DESCRIPTION:
  Set up 3x3 Adaptive Spatial Filter for VFE update.

CALLERS:
  camera_parms_write

RETURN VALUE:
  None

SCOPE:
  Global

============================================================================*/
#ifndef QDSP_MODULE_VFE25_DEFINED
void vfe_update_3x3_asf
(
  const camsensor_static_params_type *camsensor_params
)
{
    if (vfe_hw_features.filter_3x3_asf == TRUE)
	{
	  vfe_init_3x3_asf_data(camsensor_params);
      vfeUpdate.asf_cfg = vfe_asf_cfg;
	  camera_update_vfe=1;

	}

	else
	{
	  MSG_ERROR("3x3 hw ASF is not supported",0,0,0);
	}

}
#endif /* QDSP_MODULE_VFE25_DEFINED */
   
/*===========================================================================

FUNCTION:
  vfe_update_5x5_asf

DESCRIPTION:
  Set up 5x5 Adaptive Spatial Filter for VFE update.
  For LCU 5x5 ASF is used both in preview and snapshot but on non-LCU targets it is only used for snapshot.

CALLERS:
  camera_parms_write
  
RETURN VALUE:
  None
  
SCOPE:
  Global

============================================================================*/
void vfe_update_5x5_asf
(
  const camsensor_static_params_type *camsensor_params,
  VFE_ConfigOutputType vfe_cfg_op
)
{
  if(vfe_cfg_op == VFE_PREVIEW)
  {
     #ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
     #endif
  }
  else
  {
    MSG_MED("No Need to update VFE in snapshot mode", 0, 0, 0);
  }

}

#ifdef QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */
#endif /* nFEATURE_CAMERA_YCBCR_ONLY */
#ifndef FEATURE_CAMERA_YCBCR_ONLY
/*==========================================================================
Function: VFE_Get_Illuminant_for_LC

Description: Maps the illuminant from AWB to D65, TL84 or A for 
             sensor lens correction

Return Value: illuminant for lens correction

============================================================================ */
static camsensor_lc_type VFE_Get_Illuminant_for_LC(void)
{
    cam3a_auto_white_balance_illuminant_type illuminant;
    camsensor_lc_type change_lc; 

    illuminant = camera_3a_shared_get_illuminant();

    /* Use daylight color conversion array */
    if ((illuminant == CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT) ||
        (illuminant == CAM3A_AGW_AWB_DAYLIGHT_HYBRID) ||
        (illuminant == CAM3A_AGW_AWB_OUTDOOR_SUNLIGHT1) || 
        (illuminant == CAM3A_AGW_AWB_OUTDOOR_CLOUDY))
    {
        change_lc  = D65_LC;
    }

    /* Use flourescent color conversion array */
    else if ((illuminant == CAM3A_AGW_AWB_INDOOR_WARM_FLO) ||
             (illuminant == CAM3A_AGW_AWB_INDOOR_CUSTOM_FLO) || 
             (illuminant == CAM3A_AGW_AWB_INDOOR_COLD_FLO))
    {
        change_lc = TL84_LC;
    }

    /* Use incandescent color conversion array */
    else if ((illuminant == CAM3A_AGW_AWB_INDOOR_INCANDESCENT) ||
             (illuminant == CAM3A_AGW_AWB_HORIZON))
    {
        change_lc = A_LC;
    }

    else
    {
        change_lc = INVALID_LC;
    }

    return change_lc;
}
#endif
/* End of External API. 
 * New static functions should go into INTERNAL API section.
 */
/*lint -restore */
#endif /* FEATURE_CAMERA */
#endif

