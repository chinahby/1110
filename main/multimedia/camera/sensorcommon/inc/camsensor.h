#ifndef CAMSENSOR_H
#define CAMSENSOR_H
/*============================================================================

               Camera Interface Phisical Device Driver Header File

   DESCRIPTION
     This file contains the definitions needed for the camera interface
     physical device driver (PDD).

   Copyright (c) 2005 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/sensorcommon/main/latest/inc/camsensor.h#7 $ $DateTime: 2009/04/08 03:25:27 $ $Author: cgera $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/08/09  cg     Fix for CR # 177437 (Cleaning up of unused functions)
 12/18/08  sm     Added support for multi raw preview capture
 07/30/08  sm     Added code for burst mode run time detection
 06/17/08  ww     Added enum camsensor_lc_type to support 3 rolloff tables for the Micron sensor
 06/02/08  rs     camsensor_set_ev_compensation function added 
 04/08/08  jg     Added support for KM5MP
 03/11/08  cg     Added the msm id for ULC.
 02/21/08  srod   Added support for STM VB6801.
 11/23/07  cg     Declared camsensor_connection_mode as external variable to be used in camsensor driver files.
 11/07 07  cg     Inluded bio.h and GPIO_H to resolve compilation error on LCU
 
 10/02/07  cg     Added the MSM id for LCU(QSC6270) and changed the  MSM id 
                  of SC2X from MSM6085 to QSC6085. 
 09/13/07  hh     Replaced GPIO_SignalType by byte to resolve inconsistent naming convention 
                  of gpio_signal_type across different targets
 09/12/07  hh     Added camsensor_config_camclk API for camclk_po 
                  is used as mddi_camif_clk (MDDI) and mclk (PARALLEL). 
                  Added camsensor_i2c_read_word and camsensor_i2c_write_word.
                  Added mddi_camif_clk to camsensor_dynamic_params.
 09/05/07  dl     Export camsensor gpio function. 
 08/24/07  cg     Added Support for fixed fps in case of YUV sensors.
 03/12/07  klee   Moved out camsensor_xx_matrix_type
 02/23/07  khk    Adding Support for MT9T012 Piezo sensor.
 01/26/07  ven    Added support for SC2X
 01/15/07  rve    Enable MT9T013
 01/08/07  khk    Added support for MT9D112 sensor - both Bayer and YUV versions.
 12/07/06  srod	  Added support for ST Micro VB6800
 12/06/06  khk    Reverting back the support for MT9D112 YUV sensor. 
                  The support will be added later.
 11/23/06  khk    1. Added Support for MT9D112 YUV Sensor
                  2. Corrected the typo below for MT9M112 YUV sensor.
 10/16/06  klee   Updated Low/Bright light definition for ASF to sync the order in chromatix
 09/26/06  cg     Added sensor_suffix to camsensor_static_params_type 
 07/09/06  cg     1) Added support for MT9M112 YUV Sensor.
                  2) Added support for Antibanding, Special effects , Auto white balance , Exposure Mode
                  to be done on sensor side.
 08/24/06  khk    Added support for Micron MT9V111 YUV
 08/11/06  klee   Removed the tuning variables related to ASF, LVCM and BVCM
 08/11/06  klee   Removed the tuning variables only related to AEC
 08/10/06  klee   Removed the tuning variables related to AWB/AEC
                  Removed the tuning variables related to Color matrix
 08/07/06  khk    Relocated the USE_CAMSENSOR_XXX defines to 
                  \drivers\camsensor\custcamsensor.h
 07/31/06  jfo    Added Micron MT9T012 3MP 
 07/25/06  ven    Made the camsensor_msm_auto_detect function global and it is called
                  from camera_svcs_init to detect the msm type.
 07/06/06  cg     Removed  Panasonic MN101E19A 1.3 MP YUV Sensor Supoort for Mainline
 07/05/06  klee   Removed floating->int converting API
 06/28/06  klee   Keep af_process_type in camsensor
 06/26/06  klee   Update FEATURE_CAMERA_INFO_RELOCATED to be CAMERA_INFO_RELOCATED, 
                  because it is not camera feature
 06/15/06  klee   Added FEATURE_CAMERA_INFO_RELOCATED regarding variable relocation
 06/12/06  jv     Added entry to camsensor_msm_id_type for MSM6800B.
 06/02/06  dle    Added support for _get_fps_list().
 05/19/06  cg     Proper Alignment of function table in camsensor.c
 05/18/06  dle    Added support for 6260.
 05/10/06  cg     Featurization of drivers, through USE_CAMSENSOR_XXX Flag
 04/14/06  dl     Added camsensor_info structure.  
 04/06/06  ven    Removed the sensor MT9M011 from the table to align with the 
                  function table in camsensor.c
 03/03/06  klee   Added awb_weight_vector from camera_awb.c
                  Added awb_agw_grid_dist_2_threshold from camera_awb.h
 01/06/06  dl     Added 10 bit raw output option.
 12/14/05  ven    Added support for VFE 4X4 Statistics collection.
 11/28/05  dl     Added sony 3MP Piezo.
 11/21/05  ven    Added support for Micon 2MP SOC MT9D111 sensor.
 11/07/05  dl     Moved feature definitions to the camera feature file. 
 10/19/05  ylee   Cleaned up lint warning.
 10/17/05  ylee   Added the new gp_clk config support functons.
 10/14/05  dle    Renamed VFE1X to VFE1P.
 10/11/05  sch    Added sensor's zoom control handler. 
                  Added sensor_features to camsensor_static_params to let
                  sensors decide whether to use natively supported sensor 
                  features, Ex., using sensor's zoom control instead of MSM's.
 10/04/05  sai    Renamed the CHM MT9D011 2M sensor.
 09/30/05  ylee   Added support for Micron MT9D011 2M sensor with VCM AF. 
 09/13/05  ven    added support for storing sensor specific exif paramters
 09/08/05  klee   added 5 variable in camsensor structure to support advanced AF
 09/08/05  dl     Resolved lint issues. 
 09/01/05  dl     Resolved lint warnings.
 08/30/05  rve    Renamed variables for WB bias.
 08/29/05  rve    Added support to AWB WB gain biases in camsensor structure.
 08/27/05  lrw    Added sensor deiver naming convention descriptions
 08/25/05  dl     Added Sharp LZOP3733 to sensor list.
 08/23/05  rve    Added Sony 2MP Piezo. 
 08/22/05  dle    Put enable_hw_rolloff in VFE1X etc.
 08/12/05  sch    Moved the roll off structures to chromatix params. vfe driver
                  computes the slope tables driectly from chromatix 
                  parameters.
                  Added enable_hw_rolloff.
 08/06/05  sch    support for vfe h/w rolloff.
 08/01/05  mvd    Removed new Sharp LZOP3733 from sensor list.
 07/22/05  klee   add low light correction variables into sensor parameter structure
 06/29/05  klee   update sensor parameter structure
                  by adding 4 more variable in AWB(from cam3a.c)
 07/19/05  mvd    Added comments to warn folks when changing sensor table.
                  No code change.
 07/08/05  dl     Added the following feature flags 
                  FEATURE_CAMERA_LVCM
                  FEATURE_CAMERA_BVCM
                  FEATURE_CAMERA_5x5_ASF
 07/08/05  sai    Modified camsensor_config_camclk_po to return average camclk.
 07/07/05  rve    Added black level lines to allow VFE clipping to calculate
                  clipping values if black lines are used by module.
 16/06/05  ds     Returned Micron 1.3 MP (MT9M011). Solved issues.
 05/25/05  ds     Added the Micron 2.0 MP (MT9D011) sensor.
 06/06/05  dl     Added ASF 5x5 support
 05/23/05  yl     Added OV2630 sensor support.
 05/18/05  mvd    Removed Miron (mt9m011) - has issues.
 05/18/05  mvd    Removed featurization on Sony 2.0 MP.  Added Miron.
 05/05/05  dl     Added BVCM supports.
 05/04/05  dle    CAMSENSOR is now free of VFE:
                    Moved all CAMIF related defines into camif.h.
                    Defined generic _color_conversion_type,
                    _color_correction_type, _3x3LumaFilterType,
                    _vfe_aec_awb_config_type, _vfe_af_config_type, and
                    _black_correction_type.
                    Added the new generic types into _static_params_type.
 04/21/05  uy     Added Micron 1.3MP sensor (mt9m011).
 03/23/05  mvd    Added the Sony 2.0 MP to the build via this compile flag
                  FEATURE_CAMSENSOR_SONY_IU011FS.  Fixed sensor table to 
                  match function table for auto detect.
 03/23/05  rve    Black offset added to START function.
 03/23/05  uy     Added two 1.3M sensors: Omnivision (ov9650) and Samsung (s5k3a1eb).
 03/03/05  dl     Added LPM support.
 02/22/05  mvd    Added Auto Frame Rate Support (AFR).  Added Sharp 2.0
                  MP sensor back into sensor list.
 02/17/05  mvd    Added antibanding support. Added real to register gain
                  and register to real gain functions
 02/16/05  mvd    Removed FEATURE_CAMSENSOR_SONY_IMX006FQ.  Fixed sensor_
                  model_type.  Added cloudy WB param and changed the name
                  on two AF params.
 02/15/05  sch    Corrected camsensor_sensor_model_type enumeration ordering
                  mismatch with the sensor init functions ordering.
 02/03/05  sai    Added Panasonic 1.3 CMOS MN101E19A YCbCr sensor to list
                  under the feature : FEATURE_CAMSENSOR_PANASONIC_MN101E19A.
                  Added pclk_invert to static params struct.
 01/26/05  mvd    Added Sony 1.3 CMOS IMX006FQ sensor to list under the
                  feature: FEATURE_CAMSENSOR_SONY_IMX006FQ
 01/20/05  mvd    Renamed config_parms to _start which is more consistent
                  with our naming scheme.  Added outdoor index for
                  AWB Advanced Grey World.  Removed gain_factor param.
                  Removed iu011fs from sensor list.
 01/13/05  sch    - Zooming support 
                  - Added camsensor_preview_resolution, 
                    camsensor_snapshot_resolution, 
                    camsensor_current_resolution,
                    to make the driver interface more generic.
                  - Preview mode is supported for all the sensor resolutions.
                    to speed up snapshot / raw snapshot bring up.
                  - standardized the function to read/write sensor registers. 
                  - Updated camsensor_static_params_type
 01/12/05  mvd    Remvoed Sharp sensor.  Added config params moved from
                  Cam3a.c
 12/17/04  rve    Added Sony 2MP CMOS sensor IU011F-S.
 11/23/04  mvd    Added LZOP3731 to sensor init function list
 11/04/04  mvd    Added AEC luma target bias to camsensor struct.
                  Removed unused parms (moved to services layer)
                  Added structs to camsensor_params so that Chromatix 
                  floating point output can be converted to Q factor numbers
 10/27/04  sch    Width and Height dimensions changed to uint16.
                  Changes to camsensor_config() arguments.
 10/14/04  mvd    Increased num bits of input params for write_exposure_gain
 10/05/04  mvd    Renamed #include vfe_if.h to vfe.h
 09/30/04  mvd    Removed "luma_target" - moved to services layer cam3A.
                  Added "auto_flash_target_to_current_ratio" for flash
                  support.
 09/21/04  mvd    Added sensor_name into static params struct.
 09/09/04  mvd    Added indoor_index for Auto White Balanced Advanced
                  Grey World Algorithm.
 09/08/04  sch    Added stubs for snapshot, raw snapshot related changes. 
 09/06/04  mvd    Move AWB and AEC params out into the camera layer 
                  (cam3a.h/c)
 08/26/04  mvd    Added camsensor_config_params for setting sensor specific
                  params.
 08/25/04  mvd    Fixed compile bugs.
 08/11/04  mvd    Stripped out funtions that can be implemented
                  in the higher services\camera layer.  Added tuning
                  parameters/structures to sensor config.
 ------------------- Old History from camif.h -----------------------------
 06/23/04  mvd    Added pointer to sensor specific gamma table for use in 
                  the Camera layer.
 06/23/04  mvd    Added new type for Camera color correction 
                  camif_color_corr_type6550
 05/12/04  sch    MSM6550 Adaptation

============================================================================*/

 
/*============================================================================

The sesnsor driver naming converntion is camsensor_sensormodel_aabbb[cc[d]].c:


Sensor driver naming conventions:
aa - abbreviation for ODM - sn (SONY), sh (Sharp), ov (Omini-vision), 
     ss (Samsung), mu (Micron), cs (Casio), mc (Panasonic).

bbb - size of pixels - 2m0, 1m3, 3m0, 3m0, 10m, and etc...

cc[d]- modules - pz (piezo AF), sm (stepping motor AF), vc (voice coil AF), 
     yu (YUV), ms (mechanical shutter), op (optical zoom) and etc. 
     this field is module specific. There is a chance that we have support 
     two modules with same capabilities then we need to use pz, pz1 pz2 and etc.
     This part can be repeated, example is a sensor module with             
     AF with optical zoom and mechanical shutter. 

============================================================================*/
             


/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_CAMERA
#include "camif.h"
#include "camera.h"
#include "camerai.h"
#include "i2c.h"
#include "clk.h"
#include "chromatix_6550.h"
#include "exif.h"
#include "custcamsensor.h"
#include "bio.h"
#include GPIO_H

/*============================================================================
                        CONSTANTS
============================================================================*/
#define CLIPI(a) ( ((a<0)||(a>255)) \
                 ? ((a<0)?0:255) \
                 :a);

#define Q14   0x00004000
#define Q13   0x00002000
#define Q12   0x00001000
#define Q12_2 0x00002000
#define Q12_4 0x00004000
#define Q11   0x00000800
#define Q10   0x00000400
#define Q8    0x00000100
#define Q7    0x00000080
#define Q6    0x00000040
#define Q4    0x00000010

/* RGB to YCBCR color conversion coefficients by ITU Rec 601 */
#ifdef FEATURE_CAMERA_ITU_601
#define YCBCR_COEFF_A11     2103
#define YCBCR_COEFF_A12     4130
#define YCBCR_COEFF_A13      802
#define YCBCR_COEFF_A21    -1214
#define YCBCR_COEFF_A22    -2384
#define YCBCR_COEFF_A23     3598
#define YCBCR_COEFF_A31     3598
#define YCBCR_COEFF_A32    -3013
#define YCBCR_COEFF_A33     -585
#define YCBCR_COEFF_By        16
#define YCBCR_COEFF_Bcb      128
#define YCBCR_COEFF_Bcr      128
#define YCBCR_MIN_CHROMA      16
#define YCBCR_MAX_CHROMA     240
#define YCBCR_MIN_LUMA        16
#define YCBCR_MAX_LUMA       235
/* RGB to YCBCR color conversion coefficients by ITU Rec 709 */
#elif defined FEATURE_CAMERA_ITU_709
#define YCBCR_COEFF_A11     1496
#define YCBCR_COEFF_A12     5032
#define YCBCR_COEFF_A13      508
#define YCBCR_COEFF_A21     -827
#define YCBCR_COEFF_A22    -2778
#define YCBCR_COEFF_A23     3598
#define YCBCR_COEFF_A31     3598
#define YCBCR_COEFF_A32    -3267
#define YCBCR_COEFF_A33     -331
#define YCBCR_COEFF_By        16
#define YCBCR_COEFF_Bcb      128
#define YCBCR_COEFF_Bcr      128
#define YCBCR_MIN_CHROMA      16
#define YCBCR_MAX_CHROMA     240
#define YCBCR_MIN_LUMA        16
#define YCBCR_MAX_LUMA       235
/* RGB to YCBCR color conversion coefficients by JPEG default */
#else
#define YCBCR_COEFF_A11     2449
#define YCBCR_COEFF_A12     4809
#define YCBCR_COEFF_A13      934
#define YCBCR_COEFF_A21    -1382
#define YCBCR_COEFF_A22    -2714
#define YCBCR_COEFF_A23     4096
#define YCBCR_COEFF_A31     4096
#define YCBCR_COEFF_A32    -3430
#define YCBCR_COEFF_A33     -666
#define YCBCR_COEFF_By         0
#define YCBCR_COEFF_Bcb      128
#define YCBCR_COEFF_Bcr      128
#define YCBCR_MIN_CHROMA       0
#define YCBCR_MAX_CHROMA     255
#define YCBCR_MIN_LUMA         0
#define YCBCR_MAX_LUMA       255
#endif /* FEATURE_CAMERA_ITU_XXX */

/* YCBCR to RGB color conversion coefficients by JPEG default */
#define RGB_COEFF_A11     9539 /* 1.1644 */
#define RGB_COEFF_A12       -1 /* 0 */
#define RGB_COEFF_A13    13074 /* 1.5959 */
#define RGB_COEFF_A21     9539 /* 1.1644 */
#define RGB_COEFF_A22    -3209 /* -0.3917 */
#define RGB_COEFF_A23    -6660 /* -0.8130 */
#define RGB_COEFF_A31     9539 /* 1.1644 */
#define RGB_COEFF_A32    16526 /* 2.0173 */
#define RGB_COEFF_A33        1 /* 0 */
#define RGB_COEFF_Br      -223 /* -223 */
#define RGB_COEFF_Bg       135 /* 135 */
#define RGB_COEFF_Bb      -277
//#define RGB_COEFF_Bb      -277 /* -277 */

/* For Auto Frame Rate */
#define CAMSENSOR_NUM_DEFAULT_FRAME_RATES 15

/* ASF settings */
#define LOW_LIGHT    0
#define NORMAL_LIGHT 1
#define BRIGHT_LIGHT 2

#define DUAL_FILTER    0
#define SINGLE_FILTER  1

/* Vignette Correction Table */
#define NUMBER_OF_TBL  3
#define NUMBER_OF_MEASURES  33

/*============================================================================
                        ABSTRACT DATA TYPES
============================================================================*/
typedef enum
{
  CAMSENSOR_FULL_SIZE,
  CAMSENSOR_QTR_SIZE,
  CAMSENSOR_INVALID_SIZE,
} camsensor_sensor_resolution_type;

typedef enum
{
  /* Sensor egisters that need to be updated during initialization */
  CAMSENSOR_REG_INIT,
  /* Sensor egisters that needs periodic I2C writes */ 
  CAMSENSOR_REG_UPDATE_PERIODIC,
  /* All the sensor Registers will be updated */ 
  CAMSENSOR_REG_UPDATE_ALL,
  /* Not valid update */
  CAMSENSOR_REG_UPDATE_INVALID
} camsensor_i2c_register_update_type;

/* ********************* WARNING WARNING **************************** */
/* ********************* WARNING WARNING **************************** */
/* MUST CHANGE camsensor_detect_table in camsensor.c when             */
/* modifying this table                                               */
/* ********************* WARNING WARNING **************************** */
typedef enum
{
#ifndef FEATURE_NI_GPIO
#ifdef USE_CAMSENSOR_SIV121A
  CAMSENSOR_SIV121A_ID,
#endif
#ifdef USE_CAMSENSOR_SIC110A
  CAMSENSOR_SIC110A_ID,
#endif

#ifdef USE_CAMSENSOR_SID130B
  CAMSENSOR_ID_SID130B,
#endif

#ifdef USE_CAMSENSOR_MICRON_SIV121D_0M3
  CAMSENSOR_MICRON_SIV121D_0M3,
#endif

#ifdef USE_CAMSENSOR_SP0838
  CAMSENSOR_ID_SP0838,
#endif

#ifdef USE_CAMSENSOR_SP0A18
   CAMSENSOR_ID_SP0A18,
#endif

#ifdef USE_CAMSENSOR_SIV120A
   CAMSENSOR_ID_SIV120A,
#endif

#ifdef USE_CAMSENSOR_GC0329
	CAMSENSOR_ID_GC0329,
#endif

#ifdef USE_CAMSENSOR_GC0311
	CAMSENSOR_ID_GC0311,
#endif

#ifdef USE_CAMSENSOR_SP0828
	CAMSENSOR_ID_SP0828,
#endif

#ifdef USE_CAMSENSOR_SP0A19
   CAMSENSOR_ID_SP0A19,
#endif


#ifdef USE_CAMSENSOR_OMNI_VISION_9650
  CAMSENSOR_OMNI_VISION_9650,
#endif /* USE_CAMSENSOR_OMNI_VISION_9650 */

#ifdef USE_CAMSENSOR_MICRON_MT9P012
 CAMSENSOR_MICRON_MT9P012,
 #endif /*USE_CAMSENSOR_MICRON_MT9P012*/

#ifdef USE_CAMSENSOR_OMNI_VISION_2630
  CAMSENSOR_OMNI_VISION_2630,
#endif /* USE_CAMSENSOR_OMNI_VISION_2630 */

#ifdef USE_CAMSENSOR_VB6800_STM3M2VC
  CAMSENSOR_VB6800_STM3M2VC,
#endif /* USE_CAMSENSOR_VB6800_STM3M2VC */

#ifdef USE_CAMSENSOR_VB6801_STM3M2VC
  CAMSENSOR_VB6801_STM3M2VC,
#endif /* USE_CAMSENSOR_VB6801_STM3M2VC */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_PIEZO
  CAMSENSOR_MICRON_MT9T012_PIEZO,
#endif /* USE_CAMSENSOR_MICRON_MT9T012_PIEZO */

#ifdef USE_CAMSENSOR_MICRON_MT9V111_YCBCR
  CAMSENSOR_MICRON_MT9V111_MUVGAYU,
#endif

#ifdef USE_CAMSENSOR_MICRON_MT9D011_VCM_AF
  CAMSENSOR_MICRON_MT9D011_VCM_AF,
#endif /* USE_CAMSENSOR_MICRON_MT9D011_VCM_AF */

#ifdef USE_CAMSENSOR_MICRON_MT9D011
  CAMSENSOR_MICRON_MT9D011,
#endif /* USE_CAMSENSOR_MICRON_MT9D011 */

#ifdef USE_CAMSENSOR_MICRON_MT9D111_MU2M0
  CAMSENSOR_MICRON_MT9D111_MU2M0,
#endif /* USE_CAMSENSOR_MICRON_MT9D111_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0
  CAMSENSOR_MICRON_MT9D112_MU2M0,
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU
  CAMSENSOR_MICRON_MT9D112_MU2M0YU,
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU */

#ifdef USE_CAMSENSOR_MICRON_MU1M3YU_MT9M112 
   CAMSENSOR_MICRON_MU1M3YU_MT9M112,
#endif

#ifdef USE_CAMSENSOR_CASIO_NX5300
  CAMSENSOR_CASIO_NX5300,
#endif 
  /* Sharp YCbCr driver init should be called first when both YCbCr, Bayer
   * Modes are supported
   */
#ifdef USE_CAMSENSOR_SHARP_LZOP3733_YCBCR
  CAMSENSOR_SHARP_LZOP3733_YCBCR,
#endif /* USE_CAMSENSOR_CASIO_NX5300 */

#ifdef USE_CAMSENSOR_SHARP_LZOP3731
  CAMSENSOR_SHARP_LZOP3731,
#endif /*  USE_CAMSENSOR_SHARP_LZOP3731 */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_VCM_AF
	CAMSENSOR_MT9T012_STW_VCM_AF,
#endif /*USE_CAMSENSOR_MICRON_MT9T012_VCM_AF*/

#ifdef USE_CAMSENSOR_MICRON_MT9T013
	CAMSENSOR_MT9T013,
#endif /*USE_CAMSENSOR_MICRON_MT9T013*/

  /* this Sony 2.0 MP does not have auto-detect capability.  
   * Therefore, it must featurized and when enabled cannot be 
   * used with any sensor below it 
   */
#ifdef USE_CAMSENSOR_SONY_IU011FS
  CAMSENSOR_SONY_IU011FS,
#endif /* USE_CAMSENSOR_SONY_IU011FS*/

#ifdef USE_CAMSENSOR_SONY_IU011F_PIEZO
  CAMSENSOR_SONY_IU011F_PIEZO,
#endif /* USE_CAMSENSOR_SONY_IU011F_PIEZO*/

#ifdef USE_CAMSENSOR_SONY_IU018F_PIEZO
  CAMSENSOR_SONY_IU018F_PIEZO,
#endif /* USE_CAMSENSOR_SONY_IU018F_PIEZO */
  /* this Sony 1.3 MP does not have auto-detect capability.  
   * Therefore, it must be last in the list 
   */

#ifdef USE_CAMSENSOR_SONY_IMX006FQ
  CAMSENSOR_SONY_IMX006FQ,
#endif /* USE_CAMSENSOR_SONY_IMX006 */

#ifdef USE_CAMSENSOR_GC0309
  CAMSENSOR_GC0309_ID,
#endif

#ifdef USE_CAMSENSOR_DB8B63A
  CAMSENSOR_DB8V63A_ID,
#endif

#ifdef USE_CAMSENSOR_OV7675
  CAMSENSOR_OV7675_ID,
#endif

  CAMSENSOR_ID_MAX
#else
	CAMSENSOR_NI_GPIO,
  	CAMSENSOR_ID_MAX
#endif/* FEATURE_NI_GPIO */
} camsensor_sensor_model_type;


#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
/* ********************* WARNING WARNING **************************** */
/* ********************* WARNING WARNING **************************** */
/* MUST CHANGE camsensor_detect_table in camsensor.c when             */
/* modifying this table                                               */
/* ********************* WARNING WARNING **************************** */
typedef enum
{
	CAMSENSOR_ID_PAIR_START=0,
	CAMSENSOR_ID_PAIR_SIV121A_SID130B = CAMSENSOR_ID_PAIR_START,
	CAMSENSOR_ID_PAIR_SID130B_SIV121A,
	CAMSENSOR_ID_PAIR_SIV121D_SID130B,
	CAMSENSOR_ID_PAIR_SID130B_SIV121D,
	CAMSENSOR_ID_PAIR_SP0838_SP0A18,
	CAMSENSOR_ID_PAIR_SP0A18_SP0838,
	CAMSENSOR_ID_PAIR_SIV120A_SID130B,
	CAMSENSOR_ID_PAIR_SID130B_SIV120A,
	CAMSENSOR_ID_PAIR_SIV121A_SIV121A,
	CAMSENSOR_ID_PAIR_GC0329_GC0329,
	CAMSENSOR_ID_PAIR_END
}camsensor_sensor_model_pair_type;

typedef struct camsensor_sensor_model_pair_id
{
	camsensor_sensor_model_type camsensor_first;
	camsensor_sensor_model_type camsensor_second;
}camsensor_sensor_model_pair_id;

typedef uint8 (*camsensor_active_fn_type)(void);
typedef uint8 (*camsensor_unactive_fn_type)(void);
#endif

/* ********************* WARNING WARNING **************************** */
/* ********************* WARNING WARNING **************************** */
/* MUST CHANGE camsensor_detect_table in camsensor.c when             */
/* modifying this table                                               */
/* ********************* WARNING WARNING **************************** */

typedef enum
{
  CAMSENSOR_OP_MODE_SNAPSHOT,
  CAMSENSOR_OP_MODE_RAW_SNAPSHOT,
  CAMSENSOR_OP_MODE_PREVIEW,
  CAMSENSOR_OP_MODE_VIDEO,
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/
  CAMSENSOR_OP_MODE_MAX
} camsensor_op_mode_type;

typedef enum
{
  CAMSENSOR_MOVE_FOCUS_NEAR,
  CAMSENSOR_MOVE_FOCUS_FAR
} camsensor_move_focus_enum_type;

typedef enum
{
  CAMSENSOR_CCD,
  CAMSENSOR_CMOS
} camsensor_sensor_type;

typedef enum
{
  CAMSENSOR_BAYER,
  CAMSENSOR_YCBCR
} camsensor_sensor_output_format_type;

typedef enum
{
  D65_LC,
  TL84_LC,
  A_LC,
  USER_DEFINED_LC1,
  USER_DEFINED_LC2,
  USER_DEFINED_LC3,
  USER_DEFINED_LC4,
  USER_DEFINED_LC5,
  USER_DEFINED_LC6,
  INVALID_LC
} camsensor_lc_type;

typedef enum
{
    LC_SUCCESS,
    LC_FAILURE,
    LC_UNSUPPORTED
}lc_return_type;

typedef enum
{
  CAMSENSOR_8_BIT_DIRECT,
  CAMSENSOR_10_BIT_DIRECT
} camsensor_raw_output_type;

typedef enum
{
  PARALLEL_CONNECTION,
  MDDI_CONNECTION 
}camsensor_connection_mode_type;
 

typedef enum
{
  MSM6550_130,
  MSM6550_90,
  MSM6275,
  MSM6280,
  MSM6290,
  MSM6800_R1,
  MSM6800_R2,
  MSM6800B,
  MSM6246,
  MSM6260,
  SC2X,
#ifdef FEATURE_CAMIF_INTERNAL_TEST
  QSC6085,
#endif
  QSC6270,
  QSC1110
} camsensor_msm_id_type;

typedef struct
{
  camsensor_msm_id_type msm_id;
  uint8  msm_version;
  
  /* snapshot info */
  uint32 snapshot_width;
  uint32 snapshot_height;
  uint32 snapshot_dummy_pixels;
  uint32 snapshot_dummy_lines;
  
  /* preview info */
  uint32 preview_width;
  uint32 preview_height;
  uint32 preview_dummy_pixels;
  uint32 preview_dummy_lines;

  boolean pclk_invert;
  boolean i2c_ce_invert;

} camsensor_info_type;

typedef struct
{
   /* Frames per second, or frame rate in Q8 format */
   uint16 fps;

   /* Flag to determine if this frame rate is to be 
      used in the Auto Frame Rate Algorithm in the Services
      Layer */
   boolean use_in_auto_frame_rate;

   /* Used only when the above flag is true, these are
      gain "triggers" when determining what frame rate 
      to use.  Normally an unsigned 16 bit value here,
      but declare a full 32 bits.  n/a on these will == 0 */ 
   /* These are real gains, not register gains */
   float faster_fps_gain_trigger;
   float slower_fps_gain_trigger;

   /* exposure table index modifications       */
   int32 faster_fps_exp_table_index_mod;
   int32 slower_fps_exp_table_index_mod;

} camsensor_frame_rate_type;

typedef struct
{
  boolean enable;
  uint16  minThreshold;
  uint16  maxThreshold;
} camsensor_pixel_correction_type;

typedef struct
{
  boolean                enable;
  uint16                 Ch2_max;
  uint16                 Ch1_max;
  uint16                 Ch0_max;
  uint16                 Ch2_min;
  uint16                 Ch1_min;
  uint16                 Ch0_min;
} camsensor_color_conversion_type;

typedef struct
{
  boolean                enable;
  uint16                 Ch2_max;
  uint16                 Ch1_max;
  uint16                 Ch0_max;
  uint16                 Ch2_min;
  uint16                 Ch1_min;
  uint16                 Ch0_min;
} camsensor_color_correction_type;

typedef struct
{
  boolean enable;
  int16  A11;
  int16  A12;
  int16  A13;
  int16  A21;
  int16  A22;
  int16  A23;
  int16  A31;
  int16  A32;
  int16  A33;
} camsensor_3x3LumaFilterType;

typedef struct
{
  uint16 maximumYValue;
  uint16 minimumYValue;
  int16  M1;
  int16  M2;
  int16  M3;
  int16  M4;
  int16  C1;
  int16  C2;
  int16  C3;
  int16  C4;
} camsensor_vfe_aec_awb_config_type;

typedef struct
{
  uint16 lastPixel;
  uint16 reserved1;
  uint16 firstPixel;
  uint16 lastLine;
  uint16 firstLine;
  uint16 maximumY;
  uint16 minimumY;
} camsensor_vfe_af_config_type;

typedef struct
{
  boolean enable;
  uint16 lastPixel;
  uint16 firstPixel;
  uint16 lastLine;
  uint16 firstLine;
  uint16 shift;
} camsensor_black_correction_type;

/* super set of sensor supported capabilities. 
 * sensor specific driver can decide if it wants to use
 * sensor's processing or MSM's processing 
 */
typedef struct
{
  /* Sensor specific driver sets this variable to TURE,
   * if it wants to use sensor's zoom.
   */
  boolean zoom;
} camsensor_sensor_features;

typedef struct
{
   /* ------------------  Chromaxtix Output Config -------------- */
   /* Point to output of Chromatix - for sensor specific params */
   camsensor_chromatix_output_type * chromatix_parms;

   /* ------------------  Misc Sensor Config -------------- */
   /* Make/model of sensor */
   camsensor_sensor_model_type sensor_model;

   /* CCD or CMOS */
   camsensor_sensor_type sensor_type;

   /* BAYER or YCbCr */
   camsensor_sensor_output_format_type output_format;

   /* 8 bit, 10 bit, ... */
   camsensor_raw_output_type raw_output_option;

   /* What is the maximum FPS that can be supported by this sensor
      in video mode? */
   uint16 max_video_fps;
   /* Application assigned FPS in video mode */
   uint16 video_fps;
   /* Snapshot mode operation */
   uint16 max_preview_fps;
   uint16 nightshot_fps;
   /* May be assigned with max_preview_fps or nightshot_fps. */
   uint16 preview_fps;

   /* Sensor output capability */

   /* Full size input to the VFE. */
   uint16 full_size_width;
   uint16 full_size_height;

   /* Qtr Size input to the VFE */
   uint16 qtr_size_width;
   uint16 qtr_size_height;

   /* current sensor ouput resolution */
   uint16 camsensor_width; 
   uint16 camsensor_height; 

   /* Decimation supported by the sensor, Q12 number */
   uint32 preview_dx_decimation;
   uint32 preview_dy_decimation;

   /* A pointer to the sensor name for EXIF tags                */
   const char * sensor_name; 

   /* Pointer to sensor suffix for Qmobicat requirement*/

   const char * sensor_suffix; 

   /* ------------  Auto Exposure Control Config -------------- */
   boolean aec_enable;

   /* ------------  Antibanding Config ------------------------ */
   /* A subset of Auto Exposure for CMOS sensors only           */
   /* For sensor device driver that require antibanding, i.e. sensors
      with rolling shutter exposure control. */
   uint32 pixel_clock;
   /*pixel_clock_per_line includes all blanking time*/
   uint32 pixel_clock_per_line;

   /* ------------  Auto Frame Rate Config (AFR) -------------- */
   /* A subset of Auto Exposure                                 */
   /* Number of entries or possible frame rates on this sensor
      Represents the depth used in the array below */
   int32 num_possible_frame_rates;
   /* By convention, the highest frame rate will be first in the 
      array (zeroth index) and the lowest last (in order). */
   camsensor_frame_rate_type frame_rate_array[CAMSENSOR_NUM_DEFAULT_FRAME_RATES];


   /* ------------  Auto White Balance Config ----------------- */
   /* Auto white balance algorithm used */
   boolean awb_enable;

   /* ------------  Auto Focus Config ------------------------- */
   /*  VFE structure for Auto Focus */
   boolean af_enable;

   /* Num steps to go across whole range */
   int32 num_steps_near_to_far;

   /* Default position at optimum general focus: nearest infinity
      point.  This is in terms of number of steps from near side.
      This, with the number above, defines the search space. */
   int32 num_steps_near_to_closest_infinity;

   /* Num sensor steps for coarse first phase sweep search.
      This is the num steps between each stat point.  So, the
      number of total stat points =
      (num_steps_near_to_closest_infinity / num_gross_steps_between_stat_points)*/
   int32 num_gross_steps_between_stat_points;

   /* Num sensor steps for fine second phase sweep search.
      This is the num steps between each stat point.  So, the
      number of total stat points for fine search =
      (num_steps_near_to_closest_infinity / num_fine_steps_between_stat_points)*/
   int32 num_fine_steps_between_stat_points;

   /* Num search points to gather when doing 2nd pass fine search */
   int32 num_fine_search_points;

   /* option of process type */
   cam3a_af_process_enum_type af_process_type;
   /* possible closest position of lens */
   int32 position_near_end;
   /* default lens position in macro search mode */
   int32 position_default_in_macro;
   /* boundary between macro and normal search mode */
   int32 position_boundary;
   /* default lens position in normal search mode */
   int32 position_default_in_normal;
   /* possible farthest position of lens */
   int32 position_far_end;

   /* ------------  Flash Support ----------------------------- */
   /* Does the sensor have & use a flash?  */
   boolean support_auto_flash;

   /* The trigger point for Auto Flash, this ratio applies as   
      follows:
      (target_luma / current luma) > ratio; -> then use flash 
      Default value is 2 (in Q0 format; from pre-6550 platforms)
      Value here should be in Q8 format, 2 = 512 in Q8          */
   int32 auto_flash_target_to_current_ratio;

   /* Config information for focus Stats coming from the VFE */
   camsensor_vfe_af_config_type vfeAFConfig;

   /* ------------  Default Misc Parmas Config ---------------- */
   /* PCLK Invert */
   boolean pclk_invert;
   boolean                              discardFirstFrame;
   boolean                              ignore_camif_error;
   uint16                               epoch_line;
   CAMIF_InputFormatType                format;
   boolean                              gammaCorrection;
   camsensor_pixel_correction_type      defectPixelCorrection;
   camsensor_color_conversion_type      colorConversion;
   camsensor_color_correction_type      colorCorrection;
   camsensor_3x3LumaFilterType          f3x3LumaFilter;
   camsensor_black_correction_type      blackCorrection;
   uint16                               blackrows; 
   CAMIF_ConfigType                     camif_config;
   CAMIF_EFS_ConfigType                 camif_efs_config;
   CAMIF_FrameConfigType                camif_frame_config;
   CAMIF_WindowWidthConfigType          camif_window_width_config;
   CAMIF_WindowHeightConfigType         camif_window_height_config;
   CAMIF_Subsample1ConfigType           camif_subsample1_config;
   CAMIF_Subsample2ConfigType           camif_subsample2_config;
   CAMIF_EpochIntrType                  camif_epoch_intr;
   sensor_info_type                     exif_info_from_sensor;
   camsensor_sensor_features            sensor_features;
} camsensor_static_params_type;


/* New Structure to be used by the sensor driver to store all the sensor
 * specific information.
 */
typedef struct
{
  uint16  preview_pixelsPerLine;
  uint16  preview_linesPerFrame;
  uint16  snapshot_pixelsPerLine;
  uint16  snapshot_linesPerFrame;
  boolean snapshot_changed_fps;
  /* snapshot of current sensor setting */  
  uint32 active_width;   /* the number of pixels hsync is active */
  uint32 active_height;  /* the number of lines vsync is active */
  uint32 dummy_pixels;   /* the number unused pixels after hsync active (including oblack) */
  uint32 dummy_lines;    /* the number unused lines after vsync active (including oblack) */
  uint32 capture_width;  /* the number pixles programmed to capture */
  uint32 capture_height; /* the number lines programmed to capture */
  uint32 hblank;         /* the number of blank pixles in a line (hsync not active) */
  uint32 vblank;         /* the number of blank lines in a frame (vsync not active) */
  uint32 pclk;           /* pixel clock coming from the sensor */
  uint32 mclk;           /* master clock going to the sensor */
  uint32 bpp;            /* number bits per pixels */
  uint32 i2c_freq;       /* i2c frequency */
  uint32 three_wire_freq;/* 3wire frequency */

  /* the following are the optional mddi link configuration parameters */
  /* if camsensor driver do not proivde these values, */
  /* mddi driver will provide the link configuration with the best safty margin*/  
  uint32 mddi_data_rate;      /* mddi data rate in bps */
  
  /* mddi link parameter during sensor configuration */
  uint32 mddi_config_bps;             /* byte per subframe*/
  uint32 mddi_config_rev_encap_size;  /* reverse encapsulation size */
  
  /* mddi link parameter during video capture */
  uint32 mddi_capture_bps;            /* byte per subframe*/
  uint32 mddi_capture_fifo_threshold; /* line fifo threshold */  
  uint32 mddi_capture_rev_encap_size; /* reverse encapsulation size */
  uint32 mddi_camif_clk; 
 
} camsensor_dynamic_params_type;

typedef struct
{ 
  boolean auto_mode;          /* Whether the mode is auto or determined */
  uint8   luma_target;        /* Luma value to try and reach */
  uint8   current_luma;       /* current luma value */
  uint16  gain;               /* Snapshot gain */
  uint32  linecount;          /* Snapshot line count */
  float   digital_gain;
}camsensor_snapshot_config_type;

/* Every sensor MUST initialize ALL the function pointers in the below
 * structure even if any of those routines are not used for a 
 * particular sensor. Sanity checking will be added later  
 */

typedef struct 
{
  boolean                 (*camsensor_start) (camsensor_static_params_type *);
  boolean                 (*camsensor_video_config) (camsensor_static_params_type *);
  boolean                 (*camsensor_raw_snapshot_config) (camsensor_static_params_type *);
  boolean                 (*camsensor_snapshot_config) (camsensor_static_params_type *);
  void                    (*camsensor_power_up) (void);
  void                    (*camsensor_power_down) (void);
  camera_ret_code_type    (*camsensor_write_exposure_gain) (uint16, uint32);
  camera_ret_code_type    (*camsensor_set_default_focus) (void);
  camera_ret_code_type    (*camsensor_move_focus) (camsensor_move_focus_enum_type, int32);
  float                   (*camsensor_register_to_real_gain) (uint16);
  uint16                  (*camsensor_real_to_register_gain) (float);
  camera_ret_code_type    (*camsensor_set_frame_rate) (uint16); 

  camera_ret_code_type    (*camsensor_set_brightness) (int8 brightness);
  camera_ret_code_type    (*camsensor_set_contrast)   (int8 contrast);
  camera_ret_code_type    (*camsensor_set_zoom) (void *ptr);
  camera_ret_code_type    (*camsensor_set_effect) (int8 effect);
  camera_ret_code_type    (*camsensor_set_antibanding) (int8 antibanding);
  camera_ret_code_type    (*camsensor_set_wb) (int8 wb);
  camera_ret_code_type    (*camsensor_set_exposure_mode) (int8 mode);
  camera_ret_code_type    (*camsensor_set_ev_compensation)(int32 compensation);
  camera_ret_code_type    (*camsensor_set_possible_frame_rate) (uint16, uint16);
} camsensor_function_table_type;


/*============================================================================
                        EXTERNAL VARIABLES DECLARATIONS
============================================================================*/
/* Sensor output resolution. */ 
extern camsensor_sensor_resolution_type camsensor_preview_resolution;
extern camsensor_sensor_resolution_type camsensor_snapshot_resolution;
extern camsensor_sensor_resolution_type camsensor_current_resolution;

/* I2C command packet */
extern i2c_rw_cmd_type camsensor_i2c_command;
extern i2c_rw_cmd_type camsensor_i2c_af_command;

/* Connection Mode ( Parallel or MDDI) */

extern camsensor_connection_mode_type camsensor_connection_mode;

/* TURE - Sensor auto detected  
 * FALSE - No Sensor detected
 */
extern boolean camsensor_initialized;

/* number of bits in the frame skip pattern */
extern uint32 vfe_skip_width;

/* Frame skip pattern */
extern uint32 vfe_skip_pattern;

/* Look up table for frame skip pattern to support different
 * frame rates. 
 * Index: Frame rate (range 1 - 29)
 */
extern uint32 vfe_skip_patterns[30];

#ifndef MSMHW_CAMCLK_PO
#define MSMHW_CAMCLK_PO
#endif /* MSMHW_CAMCLK_PO */

#ifdef MSMHW_CAMCLK_PO
/* MSM Clock rate to the sensor.
 * Need a default value. This value is used by power_on, at that time _init
 * is not called and the clock rate had not set.
 * Also, because of this situation, the first clock rate may not be correct.
 * The problem is corrected by doing power_on, power_up, power_down and
 * power_off during camera_init. By the time camera application is started,
 * we already have good clock rate. 
 */
extern uint32 camsensor_camclk_po_hz;
#endif /* MSMHW_CAMCLK_PO */

extern camsensor_dynamic_params_type camsensor_dynamic_params;

extern camsensor_info_type camsensor_info;

/*============================================================================
                        EXTERNAL API DECLARATIONS
============================================================================*/
/*===========================================================================

FUNCTION      CAMSENSOR_INIT

DESCRIPTION
              Initialize the camera sensor;

DEPENDENCIES
  None

RETURN VALUE
  if successful
    TRUE
  else
    FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_init(void);


/*===========================================================================

FUNCTION      CAMSENSOR_I2C_WRITE

DESCRIPTION
              Write to I2C

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_i2c_write(byte reg, byte *data, byte len);

/*===========================================================================

FUNCTION      CAMSENSOR_I2C_READ_BYTE

DESCRIPTION
              Read from I2C

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_i2c_read_byte(byte reg, byte *data);

/*===========================================================================

FUNCTION      CAMSENSOR_I2C_WRITE_BYTE

DESCRIPTION
              Write one byte to I2C

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_i2c_write_byte(byte reg, byte data);

/*===========================================================================
 
FUNCTION      CAMSENSOR_I2C_READ_WORD
 
DESCRIPTION
              Read one word from I2C
 
DEPENDENCIES
   None
 
RETURN VALUE
   None
 
SIDE EFFECTS
   None
  
===========================================================================*/
boolean camsensor_i2c_read_word(uint16 offset, uint16 *data);
   
/*===========================================================================
 
FUNCTION      CAMSENSOR_I2C_WRITE_WORD
 
DESCRIPTION
              Write one word to I2C
  
DEPENDENCIES
   None
 
RETURN VALUE
   None
    
SIDE EFFECTS
   None
   
===========================================================================*/
boolean camsensor_i2c_write_word(uint16 offset, uint16 data);

/*===========================================================================

FUNCTION      CAMSENSOR_I2C_AF_WRITE_BYTE

DESCRIPTION
              Write one byte to I2C

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_i2c_af_write_byte(byte reg, byte data);


/*===========================================================================

FUNCTION      camsensor_start

DESCRIPTION
              Configure the camera sensor and the camera interface

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_start
(
  camsensor_static_params_type * static_params /* Static Parameters */
);

/*===========================================================================

FUNCTION      CAMSENSOR_CONFIG

DESCRIPTION
              Configure the camera sensor and the camera interface

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successful
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_config
(
  camsensor_op_mode_type  mode,
  camsensor_static_params_type * static_params /* Static Parameters */
);

/*===========================================================================

FUNCTION      CAMSENSOR_POWER_ON

DESCRIPTION
              Turn on the power to the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_power_on(void);

/*===========================================================================

FUNCTION      CAMSENSOR_POWER_OFF

DESCRIPTION
              Turn off the power to the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_power_off(void);

/*===========================================================================

FUNCTION      CAMSENSOR_POWER_UP

DESCRIPTION
              Power up the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_power_up(void);

/*===========================================================================

FUNCTION      CAMSENSOR_POWER_DOWN

DESCRIPTION
              Power down the sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_power_down(void);


/*===========================================================================

FUNCTION      CAMSENSOR_MOVE_FOCUS

DESCRIPTION
  Move focus either near or toward infinity the number of steps
  specified.

DEPENDENCIES
  None

INPUT
  camsensor_move_focus_enum_type: - either of the two following
    CAMSENSOR_MOVE_FOCUS_NEAR - moves to near end for closer objects
    CAMSENSOR_MOVE_FOCUS_FAR - moves to the far end for more distant objs
  int32 num_steps - num steps to move focal motor

RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if no error comes back from I2C
  CAMERA_FAILED  - if I2C reported an error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type    camsensor_move_focus (camsensor_move_focus_enum_type direction, int32 num_steps);

/*===========================================================================

FUNCTION      CAMSENSOR_SET_DEFAULT_FOCUS

DESCRIPTION
  Move focus to location best suited to any subject: at the nearest 
  point where infinity is still in focus.
  specified.

DEPENDENCIES
  None

RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if no error comes back from I2C
  CAMERA_FOCUS_SENSOR_FAILED  - if I2C reported an error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type    camsensor_set_default_focus (void);

/*===========================================================================

FUNCTION      CAMSENSOR_WRITE_EXPOSURE_GAIN

DESCRIPTION   UPdate exposure gain on sensor

DEPENDENCIES
  None

INPUT
  uint8 gain - value for new gain param
  uint16 line_count - num lines in whole gain table
  
RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if no error comes back from I2C
  CAMERA_FAILED  - if I2C reported an error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type    camsensor_write_exposure_gain (uint16 gain, uint32 line_count);

/*===========================================================================

FUNCTION      CAMSENSOR_REGISTER_TO_REAL_GAIN

DESCRIPTION   Converts Exposure Table gain to a real gain value.

DEPENDENCIES
  None

INPUT
  uint16 reg_gain - value of register gain to convert

RETURN VALUE
  float gain - real gain in linear scale

SIDE EFFECTS
  None

===========================================================================*/
float    camsensor_register_to_real_gain (uint16 reg_gain);

/*===========================================================================

FUNCTION      CAMSENSOR_REAL_TO_REGISTER_GAIN

DESCRIPTION   Converts real sensor gain to a valid register value to write
              directly to sensor.  Used to generate antibanding tables.

DEPENDENCIES
  None

INPUT
  float gain - real gain in linear scale

RETURN VALUE
  uint16 reg_gain - value of register gain to convert

SIDE EFFECTS
  None

===========================================================================*/
uint16    camsensor_real_to_register_gain (float gain);

/*===========================================================================

FUNCTION      CAMSENSOR_SET_FRAME_RATE

DESCRIPTION   Set the Frames Per Second Rate in Preview mode.

DEPENDENCIES
  None

INPUT
  uint16 fps - New Frames Per Second rate in Q8 format

RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if we changed frame rate successfully
  CAMERA_NOT_SUPPORTED - function not implemented
  CAMERA_FAILED  - if frame rate change failed

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type    camsensor_set_frame_rate (uint16 fps); 

/*===========================================================================

FUNCTION      CAMSENSOR_SET_POSSIBLE_FRAME_RATE

DESCRIPTION   Set the Frames Per Second Rate in Preview mode.

DEPENDENCIES
  None

INPUT
  uint16 fps - New Frames Per Second rate in Q8 format

RETURN VALUE
  camera_ret_code_type
  CAMERA_SUCCESS - if we changed frame rate successfully
  CAMERA_NOT_SUPPORTED - function not implemented
  CAMERA_FAILED  - if frame rate change failed

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
camera_ret_code_type    camsensor_set_possible_frame_rate (uint16 min_fps, uint16 max_fps);

/*===========================================================================

FUNCTION      CAMSENSOR_EOF_I2C_UPDATE

DESCRIPTION
              This function updates I2C at End of Frame.

DEPENDENCIES
  None

INPUT
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_eof_i2c_update(void);
/*===========================================================================

FUNCTION      CAMSENSOR_GET_FPS_LIST

DESCRIPTION
              This function calculates and returns the fps list based
              on the mode.

DEPENDENCIES
  None

INPUT
  P1 is mode.

RETURN VALUE
  camera_ret_code_type
  P1 pointer to the null terminated string in swFPS format
  Set TRUE to P2

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type camsensor_get_fps_list(int32 *p1, int32 *p2);

#ifdef MSMHW_CAMCLK_PO
/*===========================================================================

FUNCTION      CAMSENSOR_CONFIG_CAMCLK

DESCRIPTION
              Configure camclk_po.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_config_camclk (camsensor_dynamic_params_type* dynamic_params);

/*===========================================================================

FUNCTION      CAMSENSOR_CONFIG_CAMCLK_PO

DESCRIPTION
              Configure camclk_po.

DEPENDENCIES
  None

RETURN VALUE
  The average camclk  

SIDE EFFECTS
  None

===========================================================================*/
uint32 camsensor_config_camclk_po(uint32 freq);

/*===========================================================================

FUNCTION      CAMSENSOR_UNCONFIG_CAMCLK_PO

DESCRIPTION
              Unconfigure camclk_po. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_unconfig_camclk_po(void);

#endif /* MSMHW_CAMCLK_PO */

/*===========================================================================

FUNCTION      CAMSENSOR_CONFIG_GP_CLK

DESCRIPTION
              Configure GP_CLK.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_config_gp_clk
(
 uint16 m,
 uint16 not_n_m,
 uint16 not_2_d
);

/*===========================================================================

FUNCTION      CAMSENSOR_UPDATE_GP_CLK_NOT_2_D

DESCRIPTION
              Update NOT_2_D register.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_update_gp_clk_not_2_d(uint16 not_2_d);

/*===========================================================================

FUNCTION      CAMSENSOR_UNCONFIG_GP_CLK

DESCRIPTION
              Unconfigure GP_CLK. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_unconfig_gp_clk(void);

/*===========================================================================

FUNCTION:
  CAMSENSOR_SET_BRIGHTNESS

DESCRIPTION:
  Changes the brightness

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS - if we changed brightness successfully
  CAMERA_FAILED  - if brightness change failed

USAGE:
  Applicable for YCbCr sensors only.

SCOPE:
  Global

============================================================================*/
camera_ret_code_type camsensor_set_brightness (int8 brightness);



/*===========================================================================


FUNCTION:
  CAMSENSOR_SET_EV_COMPENSATION

DESCRIPTION:
  Changes the brightness

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS - if we changed exposure compensation successfully
  CAMERA_FAILED  - if exposure compensation failed

USAGE:
  Applicable for YCbCr sensors only.

SCOPE:
  Global

============================================================================*/
camera_ret_code_type camsensor_set_ev_compensation (int32 compensation);

/*===========================================================================

FUNCTION:
  CAMSENSOR_SET_CONTRAST

DESCRIPTION:
  Changes the contrast

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS - if we changed contrast successfully
  CAMERA_FAILED  - if contrast change failed

USAGE:
  Applicable for YCbCr sensors only.

SCOPE:
  Global

============================================================================*/
camera_ret_code_type camsensor_set_contrast (int8 contrast);

/*===========================================================================

FUNCTION:
  CAMSENSOR_SET_ZOOM

DESCRIPTION:
  Entry to sensor specific zoom handler. 

CALLERS:
  camera_svcs_set_dimensions ()
  camera_svcs_set_parm ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS

USAGE:
  When the sensor's zooms functionality is used instead of MSM's.

SCOPE:
  Global

============================================================================*/
camera_ret_code_type camsensor_set_zoom (void *ptr);

/*===========================================================================


FUNCTION:
  CAMSENSOR_SET_EFFECT

DESCRIPTION:
  Changes the effect

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS - if we changed brightness successfully
  CAMERA_FAILED  - if brightness change failed

USAGE:
  Applicable for YCbCr sensors only.

SCOPE:
  Global

============================================================================*/
camera_ret_code_type camsensor_set_effect (int8 effect);

  

/*=============================================================================

FUNCTION:
  CAMSENSOR_SET_EXPOSURE_MODE

DESCRIPTION:
  Changes the exposure controlling algorithm

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS - if we changed mode successfully
  CAMERA_FAILED  - if mode change failed

USAGE:
  Applicable for YCbCr sensors only.

SCOPE:
  Global

===================================================================================*/

camera_ret_code_type camsensor_set_exposure_mode(int8 mode);

/*==================================================================================

FUNCTION:
  CAMSENSOR_SET_WB

DESCRIPTION:
  Changes the White Balance According to the Lighting condition

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS - if we changed mode successfully
  CAMERA_FAILED  - if mode change failed

USAGE:
  Applicable for YCbCr sensors only.

SCOPE:
  Global
==================================================================================*/

camera_ret_code_type camsensor_set_wb(int8 wb);

/*==================================================================================

FUNCTION:
  CAMSENSOR_SET_ANTIBANDING

DESCRIPTION:
  Changes the White Balance According to the Lighting condition

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS - if we changed mode successfully
  CAMERA_FAILED  - if mode change failed

USAGE:
  Applicable for YCbCr sensors only.

SCOPE:
  Global
  
==================================================================================*/

camera_ret_code_type camsensor_set_antibanding(int8 anribanding);

/*==================================================================================



FUNCTION:
  CAMSENSOR_MSM_AUTO_DETECT

DESCRIPTION:
  identifies the current MSM.
  
CALLERS:
  camera_svcs_init ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:
 It is called while starting the camera to identify the MSM.

SCOPE:
  Global

============================================================================*/
void camsensor_msm_auto_detect (void);

/*===========================================================================

FUNCTION     CAMSENSOR_CONFIG_SENSOR_GPIOS_CLKS

DESCRIPTION
  Configure gpios and clocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_config_sensor_gpios_clks (void);

/*===========================================================================

FUNCTION      CAMSENSOR_UNCONFIG_SENSOR_GPIOS_CLKS

DESCRIPTION
           Un-configure gpios and clocks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_unconfig_sensor_gpios_clks (void);

/*===========================================================================

FUNCTION      CAMSENSOR_GPIO_IN

DESCRIPTION
          Retrieve GPIO signal's value.  When the connection is MDDI, host
  bridge's function is used.  

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE  - if successful 
  FALSE - otherwise 

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_gpio_in(byte gpio_signal, byte * value_ptr);

/*===========================================================================

FUNCTION      CAMSENSOR_GPIO_OUT

DESCRIPTION
          Program GPIO output with respect to given GPIO number and value.
  When the connection type is MDDI, host bridge's function is used.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE  - if successful 
  FALSE - otherwise 

SIDE EFFECTS
  None

===========================================================================*/
boolean camsensor_gpio_out(byte gpio_signal, byte gpio_value);

#endif /* FEATURE_CAMERA */
#endif /* CAMSENSOR_H */

