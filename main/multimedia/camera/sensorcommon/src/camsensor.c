/*============================================================================

                 Camera Interface Device Driver Source File
                             Generic SENSOR

   DESCRIPTION
     This file contains the definitions needed for the camera interface
     device driver.

   Copyright (c) 2002-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/6k/sensorcommon/main/latest/src/camsensor.c#19 $ $DateTime: 2009/04/08 03:25:27 $ $Author: cgera $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 04/08/09  cg     Fix for CR # 177437 (Cleaning up of unused functions)
 03/26/09  chai   Fix for CR 176219 Camera Sensor using VREG_MMC for PM6653 on 6246 target instead of VREG_GP2
 02/05/09  chai   Fixed compiler warning
 01/30/09  cg     Featurized PMIC Voting for LCU.
 12/31/08  hap    Added support for 6290 & 6246 variants
 12/18/08  sm     Added support for multi raw preview capture
 11/25/07  cg     Reverting back some changes done on power_on and power_off functions.
                  Mainlining BMP related changes.
 10/14/08  chai   Fixed Rvct warnings on ULC
 09/01/08  cg     Camera To vote for Turning On and Off the VREG GP2 as required.
 07/30/08  sm     Added code for burst mode run time detection
 07/03/08  rs     Made the file RVCT compiler compliant
 06/17/08  hap    Added support for ESM6290 JTAG ID
 06/02/08  rs     Camsensor_set_ev_compensation support added
 05/12/08  cg     Fix for Sensor to work on ULC FFA
 04/08/08  jg     Added support for KM5MP
 03/22/08  cg     Added the Jtag-IDs for ULC.
 03/09/08  cg     Resolved the compilation issue with the previous check-in
 03/06/08  cg     Added the partnum for new 6800 variants
 02/26/08  chai   defined  GPIO_CAMIF_EN_OFF_V to avoid compilation issues on other targets
 02/26/08  cg     Use macros instead of hardcoded values to ensure the proper power up and shutdown of the sensors. 
 02/21/08  srod   Added support for STM VB6801.
 12/19/07  cg     Fixed the compiler warning.
 12/14/07  nshree New hardware ID for 6800 target
 12/07/07  cg     Generic Implementaion I2C for AF in case of MDDI and Parallel sensors.
 12/07/07  rs     Added the Partnum for ESM6270  
 11/23/07  cg     Re-declared camsensor_connection_mode as a non-static variable.
 11/21/07  cg     Updated  camsensor_gpio_out() and camsensor_gpio_in() to be used only
                  for MDDI related GPIOs
 11/21/07  cg     Added the new Partnum for LCU(Q6270)
 11/08/07  hap    Fixing RVCT2.2 compiler warnings
 11/07/07  cg     Featurized the MDDI related clocks
 11/07/07  cg     Use macro CAMERA_CLK_REGIME_MSM_RESET for reseting the clock regime.
 10/02/07  cg     Added the MSM id for LCU(QSC6270) and changed the  MSM id 
                  of SC2X from MSM6270 to QSC6270.         
 09/20/07  hh     Reverted the replacement of gpio_out by camsensor_gpio_out.
 09/17/07  ven    Added support in function camsensor_msm_auto_detect to detect 
                  multiple calls to the function and simply return if called more than once.
 09/13/07  hh     Replaced GPIO_SignalType by byte to resolve inconsistent naming convention 
                  of gpio_signal_type across different targets
 09/13/07  cg     VREG_GP2 should be used for the camera on FFAs 
                  with PM6658 and H6/H6B baseband board
 09/12/07  hh     Added camsensor_config_camclk API for camclk_po 
                  is used as mddi_camif_clk (MDDI) and mclk (PARALLEL).
 09/05/07  dl     Export camsensor gpio function.
 08/24/07   cg     Added Support for fixed fps in case of YUV sensors.
 04/09/07  zl     Added F3 message in camsensor_start() to show the sensor type.
 03/07/07  zl     Added support for FEATURE_CAMERA_MULTI_SENSOR.
 02/23/07  khk    Adding Support for MT9T012 Piezo sensor.
 02/15/07  jv     Use CAMERA_CLK_REGIME_SEL_CAMCLK macro if CLK_REGIME_SEL_CAMCLK_EXT
                  is enabled. This provides compatibility for 6800 builds.
 01/31/07  ven    Added support for PMIC for powering the sensor in SC2X
 01/26/07  ven    Added support for SC2X
 01/18/07  rve    Enabled MT9T013 3MP Micron sensor.
 01/10/07  jv     Updated camsensor_msm_auto_detect for 6575
 01/08/07  khk    Added support for MT9D112 sensor - both Bayer and YUV versions.
 12/20/06  klee   Added fix on CR 107950 - NULL pointer problem when camera is running with YUV sensor
 12/08/06  rcc    Added 6260 FFA external sensor support
 12/07/06  srod   Added support for ST Micro VB6800
 12/06/06  khk    Reverting back the support for MT9D112 YUV sensor.
                  The support will be added later.
 11/23/06  khk    1. Added Support for MT9D112 YUV Sensor
                  2. Corrected the typo below for MT9M112 YUV sensor.
 09/29/06  jv     camsensor_config_camclk_po returns average frequency
                  obtained from clk_regime_sel_camclk.
 09/07/06  cg     1) Added support for MT9M112 YUV sensor
                  2) Added support for Antibanding, Special effects , Auto white balance , Exposure Mode
             to be done on sensor side.
          3) Modified Camsensor_init() function to re-initialize the functon pointer table during sensor
             Autodtection.

 09/05/06  klee   Update awb_enable to be FALSE for the default
 08/24/06  khk    Added support for Micron MT9V111 YUV
 08/17/06  khk    Added the call to _set_framerate in the
                  function camsensor_config
 08/11/06  klee   Removed the tuning variables related to ASF, LVCM and BVCM
 08/11/06  klee   Removed the tuning variables only related to AEC
 08/10/06  klee   Removed the tuning variables related to AWB/AEC
                  Removed the tuning variables related to Color matrix
 08/07/06  khk    Minor Tweaking for adding support to use custcamsensor.h
 08/04/06  dl     Removed extern variable camctrl_tbl.
 08/01/06  rcc    Inverted i2c_ce signal for MSM6280 FFA
 07/31/06  jfo    Added Micron MT9T012 3MP
 07/25/06  ven    Made the camsensor_msm_auto_detect function global and it is called
                  from camera_svcs_init to detect the msm type.
 07/06/06  cg     Removed  Panasonic MN101E19A 1.3 MP YUV Sensor Supoort for Mainline
 07/05/06  klee   Moved color matrix converting code into VFE
 06/28/06  dl     Moved msm auto detect to init function so we can change
                  settings based on msm before sensor auto detect.
 06/28/06  klee   Keep af_process_type in camsensor
 06/26/06  klee   Update FEATURE_CAMERA_INFO_RELOCATED to be CAMERA_INFO_RELOCATED,
                  because it is not camera feature
 06/15/06  klee   Added FEATURE_CAMERA_INFO_RELOCATED regarding variable relocation
 06/12/06  jv     Updated camsensor_msm_auto_detect for MSM6800B case.
 06/07/06  dl     camsensor_config_camclk_po should not update
                  camsensor_camclk_po_hz.
 06/02/06  dle    Added support for _get_fps_list().
 05/22/06  klee   Moved camctrl_validate_feature() to support rolloff selection
                  Updated camsensor_config_camclk_po() to keep the latest freq
 05/18/06  dle    Prepare for 6260 and new clk_regime_sel_camclk_ext().
 05/10/06  cg     Featurization of drivers, through USE_CAMSENSOR_XXX Flag
 04/26/06  dl     Removed dependency from hw.h.
 04/14/06  dl     Added camsensor_info structure.
 04/12/06  klee   Updated weighted vector for AWB
 03/17/06  dle    Removed MT9M011. I2C was not properly programmed.
                  Added 19.65 and 39.3MHz for pre-calculated MND.
 03/03/06  klee   Added default value on awb_weight_vector in camsensor_start()
                  Added default value on awb_agw_grid_dist_2_threshold  in camsensor_start()
 03/01/06  sch    Initialization of new variable 'current_camclk_po_hz'
                  to track the latest camclk_po freq requested.
 02/06/06  dle    Moved all 6550 related clk functions together so they are
                  all wrap in the same !T_MSM6275, to remove compiler error.
 01/13/06  jn     Defined frame work according to new architecture.
 01/06/06  dl     Set default sensor output to 8 bit.
 12/22/05  nrw    Fixed calculation of max values for MND registers.
 12/21/05  dle    Use generic GP_CLK_OFF instead of GPIOxx.
 12/14/05  nrw    Added support for NI-GPIO when FEATURE_NI_GPIO is defined;
                  added support for more accurate MND calculation when
                  MSMHW_CAMCLK_PO is defined; fixed Lint errors.
 12/14/05  ven    Added support for VFE 4X4 Statistics collection.
 12/01/05  ylee   Changed the ov2630 and ov2620 driver file name.
 11/30/05  dl     Added sony 3MP Piezo.
 11/30/05  ven    Fixed Lint errors with Lint 8.0
 11/21/05  ven    Added support for Micon 2MP MT9D111 sensor
 11/09/05  dle    6280 PAD drive is now multiple bits, different than 6275
                  and 6800. It is set to low drive at boot. Also
                  HWIO_GPIO_PAD_HDRIVE_SEL_0_OUTM is not defined in 6280.
 10/22/05  ylee   camsensor_unconfig_gp_clk() is updated with correct enum for 6275.
 10/19/05  ylee   Cleaned up lint warning.
 10/17/05  ylee   Added the new gp_clk config support functons.
 10/11/05  sch    Added sensor's zoom control handler.
                  Added sensor_features to camsensor_static_params to let
                  sensors decide whether to use natively supported sensor
                  features, Ex., using sensor's zoom control instead of MSM's.
 10/05/05  rve    Renamed IMX006FQ, IU011FS and NX5300 to new naming convention.
 10/04/05  dl     Rename the lzop3733 driver to include sh2m0vcyu.
 10/04/05  sai    Renamed the CHM MT9D011 2M sensor.
 09/30/05  ylee   Added support for Micron MT9D011 2M sensor with VCM AF.
 09/13/05  ven    added support for storing sensor specific exif paramters
 09/08/05  klee   added 5 default values in camsensor structure to support advanced AF
 09/08/05  dl     Resolved lint issues.
 08/30/05  rve    Renamed variables for WB bias.
 08/29/05  rve    Added support to AWB WB gain biases.
 08/25/05  dl     Added Sharp YUV 2MP LZOP3733.
 08/23/05  rve    Added Sony 2MP Piezo.
 08/01/05  mvd    Removed new Sharp LZOP3733 from sensor list.
 07/22/05  klee   add default values of low light correction variables
 06/29/05  klee   add 4 more parameters for AWB and their default values
 06/21/05  klee   add 2 more reference points for AWB illuminant type and their default values
 07/19/05  sch    merged Sai's implementation of average frequency
                  calculation in camsensor_config_camclk_po.
 07/19/05  mvd    Fixed bug in sensor detect table (placement of the NX5300)
                  Fixed bugs in 16x16 region processing with center-weighted
                  and spot-metering AEC.
 07/08/05  dl     Added the following feature flags
                  FEATURE_CAMERA_LVCM
                  FEATURE_CAMERA_BVCM
                  FEATURE_CAMERA_5x5_ASF
 07/08/05  sai    Modified camsensor_config_camclk_po to return average camclk.
 07/07/05  rve    Added black rows to clip after black level module used.
                  camsensor_params->blackrows.
 06/16/05  ds     Returned Micron 1.3 MP (MT9M011). Solved issues.
 05/25/05  ds     Added the Micron 2.0 MP (MT9D011) sensor.
 05/23/05  yl     Added OV2630 sensor support.
 05/18/05  mvd    Removed Miron (mt9m011) - has issues.
 05/18/05  mvd    Removed featurization on Sony 2.0 MP.  Added Miron.
 05/05/05  dl     Added BVCM supports.
 05/04/05  dle    Temporarily removed Micro 1.3MP sensor (mt9m011).
                  CAMSENSOR is now free of VFE:
                    Moved all CAMIF related defines into camif.h.
                    Added generic VFE AWB and AF config.
 04/21/05  uy     Added Micron 1.3MP sensor (mt9m011).
 04/08/05  lrw    Remove  clk_regime_msm_get_max_adsp_clk_khz
 03/23/05  mvd    Added the Sony 2.0 MP to the build via this compile flag
                  FEATURE_CAMSENSOR_SONY_IU011FS
 03/23/05  rve    Black offset added to START function.
 03/23/05  lrw    Added FEATURE_DUAL_CLK_RGM
 03/23/05  uy     Added two 1.3M sensors: Omnivision (ov9650) and Samsung (s5k3a1eb).
 03/15/05  dl     Added LPM supports.
 03/13/05  sch    Merge 6275 camclk_po related changes.
 03/09/05  sch    Fix for MSM6800 compilation error.
                  Use CAMCLK_PO_HIGH_DRIVE_MASK for low drive on gpio-13.
 03/04/05  mvd    Added defined( T_MSM6800) along side T_MSM6275
 02/22/05  mvd    Added Auto Frame Rate Support (AFR).  Added Sharp 2.0 MP
                  sensor back into sensor list.
 02/21/05  pdo    Abstracted GPIO_OUT_13 to CAMCLK_PO_OFF for future compatibility
 02/19/05  sch    Fixed complation error.
 02/18/05  mvd    Added antibanding support. Added real to register gain
                  and register to real gain functions
 02/16/05  mvd    Removed FEATURE_CAMSENSOR_SONY_IMX006FQ.  Fixed the sensor
                  auto-detect table.
 02/15/05  sch    Fixed bug in the ordering of init function in
                  camsensor_detect_table.
 02/10/05  sch    silly mistake, low drive now takes effect on camclk_po for
                  6275 and 6550.
 02/09/05  sch    Modified the camif_po high drive to low drive.
 02/09/05  mvd    Changes for 6275 target compatibility.  Cleaned up
                  hardware driver #includes.
 02/03/05  sai    Added Panasonic 1.3 CMOS MN101E19A YCbCr sensor to list
                  under the feature : FEATURE_CAMSENSOR_PANASONIC_MN101E19A.
                  Added pclk_invert to static params struct.
 01/26/05  mvd    Added Sony 1.3 CMOS IMX006FQ sensor to list under the
                  feature: FEATURE_CAMSENSOR_SONY_IMX006FQ
 01/25/05  sch    Shuts down any clock from the MSM going to the camera when
                  during camsensor_power_off().
 01/20/05  mvd    Renamed config_parms to _start which is more consistent
                  with our naming scheme.  Added indoor and outdoor index
                  init.  Re-instated AEC bias of 1.5 to avoid pinkish
                  VFE issue.  Cleaned up camsensor_start, initing all
                  params (that can then be changed in the specific drivers
                  start function).   Removed iu011fs from sensor list.
 01/20/05  sch    - Zooming feature
                  - Code clean up.
 01/12/05  mvd    Remvoed Sharp sensor.  Added config params moved from
                  Cam3a.c
 12/17/04  rve    Added Sony 2MP CMOS sensor IU011F-S.
 11/23/04  mvd    Added LZOP3731 to sensor init function list
 11/08/04  mvd    Added code to convert all chromatix floating point
                  values to decimal Q factor numbers.  Do this once
                  on camera start & it will not have to be done again.
 10/27/04  sch    Changed the width and height dimensions to uint16.
                  Arguments changed in camsensor_config().
                  MSMHW_CAMCLK_PO should be defined for camclk_po feature.
 10/14/04  mvd    Increased num bits of input params for write_exposure_gain
 10/05/04  mvd    Removed vfe_if.h #include - .h already includes this.
 08/08/04  sch    Added stubs for snapshot, raw snapshot related changes.
 08/26/04  mvd    Added camsensor_config_params for setting sensor specific
                  params.
 08/25/04  mvd    Fixed compile bugs.
 08/11/04  mvd    Stripped out funtions that can be implemented
                  in the higher services\camera layer.  Added tuning
                  parameters/structures to sensor config.
 -------------------- Old History from camif.c -----------------------------
 06/24/04  sch    Added camclk_po implementation for providing MSM clk to
                  the sensor.
 06/23/04  mvd    Added pointer to sensor specific gamma table for use in
                  the Camera layer.
 06/23/04  mvd    Added legacy global var camif_max_fps
 05/12/04  sch    M6550 adaptation

============================================================================*/

/*============================================================================
                        INCLUDE FILES
============================================================================*/
#include "camsensor.h"
#include "pm.h"

#ifndef FEATURE_CAMERA_SENSOR_AUTO_DETECT
#define FEATURE_CAMERA_SENSOR_AUTO_DETECT
#endif /* FEATURE_CAMERA_SENSOR_AUTO_DETECT */

#ifdef FEATURE_CAMERA
#ifdef FEATURE_NI_GPIO
#include "camsensor_ni_gpio.h"

#else /* FEATURE_NI_GPIO */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_PIEZO
#include "camsensor_mt9t012_mu3m0pz.h"
#endif /* USE_CAMSENSOR_MICRON_MT9T012_PIEZO */

#ifdef USE_CAMSENSOR_MICRON_MT9P012
#include "camsensor_mt9p012_mu5m0.h"
#endif /*  USE_CAMSENSOR_MICRON_MT9P012 */

#ifdef USE_CAMSENSOR_MICRON_MT9D011
#include "camsensor_mt9d011_mu2m0.h"
#endif /*USE_CAMSENSOR_MICRON_MT9D011*/

#ifdef  USE_CAMSENSOR_MICRON_MT9D011_VCM_AF
#include "camsensor_mt9d011_mu2m0vc.h"
#endif /* USE_CAMSENSOR_MICRON_MT9D011_VCM_AF */

#ifdef USE_CAMSENSOR_MICRON_MT9D111_MU2M0
#include "camsensor_mt9d111_mu2m0.h"
#endif /*USE_CAMSENSOR_MICRON_MT9D111_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0
#include "camsensor_mt9d112_mu2m0.h"
#endif /*USE_CAMSENSOR_MICRON_MT9D111_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU
#include "camsensor_mt9d112_mu2m0yu.h"
#endif /*USE_CAMSENSOR_MICRON_MT9D111_MU2M0YU */

#ifdef USE_CAMSENSOR_OMNI_VISION_2620
#include "camsensor_ov2620_ov2m0.h"
#endif /* USE_CAMSENSOR_OMNI_VISION_2620 */

#ifdef USE_CAMSENSOR_OMNI_VISION_2630
#include "camsensor_ov2630_ov2m0.h"
#endif /* USE_CAMSENSOR_OMNI_VISION_2630 */

#ifdef USE_CAMSENSOR_OMNI_VISION_9650
#include "camsensor_ov9650.h"
#endif /* USE_CAMSENSOR_OMNI_VISION_9650 */

#ifdef USE_CAMSENSOR_VB6800_STM3M2VC
#include "camsensor_vb6800_stm3m2vc.h"
#endif /* USE_CAMSENSOR_VB6800_STM3M2VC */

#ifdef USE_CAMSENSOR_VB6801_STM3M2VC
#include "camsensor_vb6801_stm3m2vc.h"
#endif /* USE_CAMSENSOR_VB6801_STM3M2VC */

#ifdef USE_CAMSENSOR_CASIO_NX5300
#include "camsensor_nx5300_cs2m0sm_ccd.h"
#endif /* USE_CAMSENSOR_CASIO_NX5300 */

#ifdef  USE_CAMSENSOR_SAMSUNG_S5K3A1EB
#include "camsensor_s5k3a1eb.h"
#endif

#ifdef USE_CAMSENSOR_MICRON_MT9V111_YCBCR
#include "camsensor_mt9v111_muvgayu.h"
#endif

#ifdef USE_CAMSENSOR_SONY_IMX006FQ
#include "camsensor_imx006fq_sn1m3.h"
#endif /* USE_CAMSENSOR_SAMSUNG_S5K3A1EB */

#ifdef USE_CAMSENSOR_MICRON_MT9T013
#include "camsensor_mt9t013_mu3m0.h"
#endif /*USE_CAMSENSOR_MICRON_MT9T013*/

#ifdef USE_CAMSENSOR_SONY_IU011FS
#include "camsensor_iu011fs_sn2m0sm.h"
#endif /* USE_CAMSENSOR_SONY_IU011FS */

#ifdef  USE_CAMSENSOR_SONY_IU011F_PIEZO
#include "camsensor_iu011f_sn2m0pz.h"
#endif

#ifdef USE_CAMSENSOR_SONY_IU018F_PIEZO
#include "camsensor_iu018f_sn3m0pz.h"
#endif /*  USE_CAMSENSOR_SONY_IU011F_PIEZO */


#ifdef USE_CAMSENSOR_SHARP_LZOP3731
#include "camsensor_lzop3731.h"
#endif /* USE_CAMSENSOR_SHARP_LZOP3731 */

#ifdef USE_CAMSENSOR_SHARP_LZOP3733_YCBCR
#include "camsensor_lzop3733_sh2m0vcyu.h"
#endif /* USE_CAMSENSOR_SHARP_LZOP3733_YCBCR */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_VCM_AF
#include "camsensor_mt9t012_mu3m0vc.h"
#endif /*USE_CAMSENSOR_MICRON_MT9T012_VCM_AF*/

#ifdef USE_CAMSENSOR_MICRON_MU1M3YU_MT9M112
#include "camsensor_mt9m112_mu1m3yu.h"
#endif /*USE_MICRON_MU1M3YU_MT9M112*/

#ifdef USE_CAMSENSOR_GC0309
#include "camsensor_gc0309_0p1mp.h"
#endif

#ifdef USE_CAMSENSOR_SIV121A
#ifdef T_QSC1110
#include "camsensor_SIV121A_yuv.h"
#else
#include "camsensor_siv121a.h"
#endif
#endif
#ifdef USE_CAMSENSOR_DB8B63A
#ifdef T_QSC1110
#include "camsensor_DB8V63A_yuv.h"
#endif
#endif

#ifdef USE_CAMSENSOR_SIC110A
#include "camsensor_sic110a.h"
#endif

#ifdef USE_CAMSENSOR_SID130B
#include "camsensor_sid130b.h"
#endif

#ifdef USE_CAMSENSOR_MICRON_SIV121D_0M3
#include "camsensor_siv121d.h"
#endif

#ifdef USE_CAMSENSOR_SP0838
#include "camsensor_sp0838.h"
#endif

#ifdef USE_CAMSENSOR_SP0A18
#include "camsensor_sp0a18.h"
#endif

#ifdef USE_CAMSENSOR_OV7675
#include "camsensor_ov7675.h"
#endif

#ifdef USE_CAMSENSOR_SIV120A
#include "camsensor_SIV120A_yuv.h"
#endif

#ifdef USE_CAMSENSOR_GC0329
#include "camsensor_gc0329.h"
#endif

#ifdef USE_CAMSENSOR_GC0311
#include "camsensor_gc0311.h"
#endif

#ifdef USE_CAMSENSOR_SP0828
#include "camsensor_sp0828.h"
#endif

#endif /* FEATURE_NI_GPIO*/

#include "ipl.h"
#include "msg.h"
#include "msm.h"
#include "stdlib.h"
#ifdef MSMHW_CAMCLK_PO
#include "math.h"
#endif /* MSMHW_CAMCLK_PO */
#include BOOTHW_MSM_H

#if defined T_FFA
#error code not present
#endif /* T_FFA */

/* cammddi is an interface to access */
/* mddi host bridge and mddi sensor */
#ifdef FEATURE_MDDI_CAMERA
#include "cammddi.h"
#endif /*FEATURE_MDDI_CAMERA*/

#include CLKRGM_H
#include GPIO_H

#include "msmhwio.h"
#include "msmhwioreg.h"
#ifdef CAMERA_USE_PMIC_TO_POWER_SENSOR
#include "pmapp.h"
#include "pm.h"
#endif /* CAMERA_USE_PMIC_TO_POWER_SENSOR */

/* auto detect msm id */
#include "hw.h"

#ifdef     FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
#include "camera_tune.h"
#endif //  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H


/*============================================================================
                        CONSTANT DEFINITIONS
============================================================================*/
#define CAMCLK_PO_HIGH_DRIVE_MASK  0x2000
#define CAMCLK_PO_LOW_DRIVE_VALUE  0

// Now add AWB_COLOR_CORRECT_BIAS_DEFAULT into CCorr mix
#define CAMSENSOR_DEFAULT_EXPOSURE_TARGET    72
#define CAMSENSOR_DEFAULT_EXPOSURE_TOLERANCE 4

/* Achiu changed to 16 and 235 on 8/19/04 */
///* defined at 255*0.2 */
//#define CAMSENSOR_DEFAULT_VFE_AF_MIN_Y       51
#define CAMSENSOR_DEFAULT_VFE_AF_MIN_Y       16
///* defined at 255*0.8 */
//#define CAMSENSOR_DEFAULT_VFE_AF_MAX_Y       204
#define CAMSENSOR_DEFAULT_VFE_AF_MAX_Y       235

#ifdef MSMHW_CAMCLK_PO
/* Default number of bits in the M register */
#define  M_REG_BITS        16
/* Default number of bits in the N register */
#define  N_REG_BITS        16
/* Default number of bits in the D register */
#define  D_REG_BITS        16
/* Default duty cycle of the target clock */
#define  TARGET_DUTY_CYCLE 50
#endif /* MSMHW_CAMCLK_PO */

#ifdef CAMERA_USE_PMIC_TO_POWER_SENSOR
#define PMIC_VOTE_INVALID -1
#endif /* CAMERA_USE_PMIC_TO_POWER_SENSOR */

#if defined(USE_CAMSENSOR_SIV121A) || defined(USE_CAMSENSOR_SIC110A) || defined(USE_CAMSENSOR_DB8B63A)
#define CAM_ENABLE_PULLUP
#endif

#ifdef CAM_ENABLE_PULLUP
#undef  GPIO_CAMIF_EN_ON_V
#undef  GPIO_CAMIF_EN_OFF_V
#define GPIO_CAMIF_EN_ON_V  1
#define GPIO_CAMIF_EN_OFF_V 0
#endif
/*============================================================================
                        INTERNAL ABSTRACT DATA TYPES
============================================================================*/
#ifdef MSMHW_CAMCLK_PO
/* statistics for the target clock */
typedef struct
{
  /* Flag to enable target clock statistics computation */
  boolean enable;
  double cycle_range;
  double ave_dc;
  double min_dc;
  double max_dc;
  double c2c_jitter;
  double ave_period;
  double min_period;
  double max_period;
  double min_pw;
  double max_pw;
  double min_lw;
  double max_lw;
  double min_ph;
  double max_ph;
  double min_pl;
  double max_pl;
  double small_period;
  double large_period;
} camsensor_clk_mnd_stats_type;

#if !defined (T_MSM6275) && !defined (T_MSM6800)
typedef struct
{
  /* Input frquency to M/N:D counter, usually sourced by a PLL */
  uint32 source_freq;

  /* Required Output frequency from M/N:D counter */
  uint32 target_freq;

  /* Acutual output frequency from M/N:D counter */
  double out_freq;

  /* Duty cycle of the target clock */
  uint32 target_dc;

  double freq_error;

  /* Number of bits for M, N, D values */
  uint8 m_bits;
  uint8 n_bits;
  uint8 d_bits;

  /* Maximum value allowed for M, N, D */
  uint32 max_m_val;
  uint32 max_n_val;
  double max_d_val;

  /* Calculated ideal (32-bit) values of M, N, D */
  uint32 m_ideal_val;
  uint32 n_ideal_val;
  double d_ideal_val;

  /* Actual M, N, D values */
  uint32 m_val;
  uint32 n_val;
  double d_val;

  /* Value that needs to be programmed into the registers */
  uint16 m_reg_val; /* = m_val          */
  uint16 n_reg_val; /* = ~(n_val-m_val) */
  uint16 d_reg_val; /* = 2*d_val        */

  /* Statistics values */
  double error;
  double ex_error;
  camsensor_clk_mnd_stats_type stats;
} camsensor_clk_mnd_type;
#endif /* nT_MSM6275 && nT_MSM6800 */
#endif /* MSMHW_CAMCLK_PO */


/*============================================================================
                        EXTERNAL VARIABLES DEFINITIONS
============================================================================*/
/* When MSM provides the clock to sensor, this value is used to configure
 * the MSM clock source for the required frequency
 *
 * Need a default value. This value is used by power_on, at that time _init
 * is not called and the clock rate had not set.
 * Also, because of this situation, the first clock rate may not be correct.
 * The problem is corrected by doing power_on, power_up, power_down and
 * power_off during camera_init. By the time camera application is started,
 * we already have good clock rate.
 */

#ifdef MSMHW_CAMCLK_PO
#if !defined (T_MSM6275) && !defined (T_MSM6800)
static camsensor_clk_mnd_type camsensor_clk_mnd;
#endif /* !defined (T_MSM6275) && !defined (T_MSM6800) */
/* camif_gp_clk_hz is only used if GP_CLK is used by a sensor. The
 * default value is used by power_on(). The correct sequence is power_on(),
 * power_up(), power_down(), and power_off() during camera_init() so that
 * a sensor can set the clock to the rate that it needs. */
uint32 camsensor_camclk_po_hz = 24576000;
#endif /* MSMHW_CAMCLK_PO */


/* Is the camera initialized? It is set in camsensor_init() when a sensor is
 * detected and is able to initialize the sensor. */
boolean camsensor_initialized = FALSE;

/* Used to send I2C command */
i2c_rw_cmd_type camsensor_i2c_command;
i2c_rw_cmd_type camsensor_i2c_af_command;

/* Sensor output resolution. */
camsensor_sensor_resolution_type camsensor_preview_resolution = CAMSENSOR_INVALID_SIZE;
camsensor_sensor_resolution_type camsensor_snapshot_resolution = CAMSENSOR_INVALID_SIZE;
camsensor_sensor_resolution_type camsensor_current_resolution = CAMSENSOR_INVALID_SIZE;

/* Used to tell VFE how to skip frames */
uint32 vfe_skip_width = 1;
uint32 vfe_skip_pattern = 1;
uint32 vfe_skip_patterns[30] =
{ 0x0001, 0x0001, 0x0101, 0x0421, 0x1111, 0x1249, 0x14a5, 0x5554, /* 0-7 */
  0x5555, 0x6b5a, 0x6db6, 0x6eee, 0x7bde, 0x7efe, 0x7ffe, 0x7fff, /* 8-15 */
  0x555aaaa, 0x56b55aa, 0x5ad6b5a, 0x6b6b6b6, /* 16-19 */
  0X6db6db6, 0X376edbb, 0X5ddeeee, 0x5ef7bde, /* 20-23 */
  0x37df7df, 0x77df7df, 0x77df7df, 0x7effbfe, /* 24-27 */
  0x7ff7ffe, 0x7fffffe                        /* 28-29 */
};

/* Sensor specific driver fills the callback during initialization */
camsensor_function_table_type camsensor_function_table;
camsensor_dynamic_params_type camsensor_dynamic_params;

/* Sensor infomation */
camsensor_info_type camsensor_info;
#ifdef FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
camsensor_chromatix_output_type * chromatix_parms_per_mobicat_ptr = NULL;
#endif /* FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H */
/*============================================================================
                        INTERNAL VARIABLES DEFINITIONS
============================================================================*/
/* --------------------- Auto-exposure tables ----------------------------- */

#if 0
EXAMPLE ONLY
/* Demonstraites a frame-average exposure bias table where every element
   is weighted equally.
   The sum of all entries must add up to the number
   of entries (64 in this case to assure uniform luma comparison results,
   if this isn't done, the result will be either a brigher or darker image) */

LOCAL uint aec_frame_average_bias_table_64[] =  /* Q8 format */
{
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0),
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0),
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0),
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0),
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0),
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0),
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0),
  (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0), (int)(1.0*256.0)
};
#endif

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT

#ifdef FEATURE_CAMERA_MULTI_SENSOR

camsensor_sensor_model_pair_type current_camsensor_pair;

camsensor_sensor_model_pair_id camsensor_pair[] = 
{
	{CAMSENSOR_SIV121A_ID,CAMSENSOR_ID_SID130B},        
	{CAMSENSOR_ID_SID130B,CAMSENSOR_SIV121A_ID},
	{CAMSENSOR_MICRON_SIV121D_0M3,CAMSENSOR_ID_SID130B},        
	{CAMSENSOR_ID_SID130B,CAMSENSOR_MICRON_SIV121D_0M3},
	{CAMSENSOR_ID_SP0838,CAMSENSOR_ID_SP0A18},
	{CAMSENSOR_ID_SP0A18,CAMSENSOR_ID_SP0838},
	{CAMSENSOR_ID_SIV120A,CAMSENSOR_ID_SID130B},        
	{CAMSENSOR_ID_SID130B,CAMSENSOR_ID_SIV120A},
	{CAMSENSOR_SIV121A_ID,CAMSENSOR_SIV121A_ID},
    {CAMSENSOR_ID_GC0329,CAMSENSOR_ID_GC0329},    
	{CAMSENSOR_ID_MAX,CAMSENSOR_ID_MAX},
};
#else
camsensor_sensor_model_type current_camsensor_id;
#endif

LOCAL camsensor_unactive_fn_type camsensor_unactive_value_table[CAMSENSOR_ID_MAX] = 
{
#ifdef USE_CAMSENSOR_SIV121A
  camsensor_SIV121A_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_SIC110A
  camsensor_sic110a_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_SID130B
  camsensor_SID130B_unactive,
#endif

#ifdef USE_CAMSENSOR_MICRON_SIV121D_0M3
  camsensor_SIV121D_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_SP0838
  camsensor_sp0838_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_SP0A18
   camsensor_sp0a18_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_SIV120A
   camsensor_SIV120A_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_GC0329
	camsensor_gc0329_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_GC0311
	camsensor_GC0311_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_SP0828
	camsensor_sp0828_ycbcr_unactive,
#endif

#ifdef USE_CAMSENSOR_OMNI_VISION_9650
  NULL,
#endif /* USE_CAMSENSOR_OMNI_VISION_9650 */

#ifdef USE_CAMSENSOR_MICRON_MT9P012
 NULL,
 #endif /*USE_CAMSENSOR_MICRON_MT9P012*/

#ifdef USE_CAMSENSOR_OMNI_VISION_2630
  NULL,
#endif /* USE_CAMSENSOR_OMNI_VISION_2630 */

#ifdef USE_CAMSENSOR_VB6800_STM3M2VC
  NULL,
#endif /* USE_CAMSENSOR_VB6800_STM3M2VC */

#ifdef USE_CAMSENSOR_VB6801_STM3M2VC
  NULL,
#endif /* USE_CAMSENSOR_VB6801_STM3M2VC */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_PIEZO
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9T012_PIEZO */

#ifdef USE_CAMSENSOR_MICRON_MT9V111_YCBCR
  NULL,
#endif

#ifdef USE_CAMSENSOR_MICRON_MT9D011_VCM_AF
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D011_VCM_AF */

#ifdef USE_CAMSENSOR_MICRON_MT9D011
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D011 */

#ifdef USE_CAMSENSOR_MICRON_MT9D111_MU2M0
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D111_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU */

#ifdef USE_CAMSENSOR_MICRON_MU1M3YU_MT9M112 
   NULL,
#endif

#ifdef USE_CAMSENSOR_CASIO_NX5300
  NULL,
#endif 
  /* Sharp YCbCr driver init should be called first when both YCbCr, Bayer
   * Modes are supported
   */
#ifdef USE_CAMSENSOR_SHARP_LZOP3733_YCBCR
  NULL,
#endif /* USE_CAMSENSOR_CASIO_NX5300 */

#ifdef USE_CAMSENSOR_SHARP_LZOP3731
  NULL,
#endif /*  USE_CAMSENSOR_SHARP_LZOP3731 */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_VCM_AF
	NULL,
#endif /*USE_CAMSENSOR_MICRON_MT9T012_VCM_AF*/

#ifdef USE_CAMSENSOR_MICRON_MT9T013
	NULL,
#endif /*USE_CAMSENSOR_MICRON_MT9T013*/

  /* this Sony 2.0 MP does not have auto-detect capability.  
   * Therefore, it must featurized and when enabled cannot be 
   * used with any sensor below it 
   */
#ifdef USE_CAMSENSOR_SONY_IU011FS
  NULL,
#endif /* USE_CAMSENSOR_SONY_IU011FS*/

#ifdef USE_CAMSENSOR_SONY_IU011F_PIEZO
  NULL,
#endif /* USE_CAMSENSOR_SONY_IU011F_PIEZO*/

#ifdef USE_CAMSENSOR_SONY_IU018F_PIEZO
  NULL,
#endif /* USE_CAMSENSOR_SONY_IU018F_PIEZO */
  /* this Sony 1.3 MP does not have auto-detect capability.  
   * Therefore, it must be last in the list 
   */

#ifdef USE_CAMSENSOR_SONY_IMX006FQ
  NULL,
#endif /* USE_CAMSENSOR_SONY_IMX006 */

#ifdef USE_CAMSENSOR_GC0309
  NULL,
#endif

#ifdef USE_CAMSENSOR_DB8B63A
  NULL,
#endif

#ifdef USE_CAMSENSOR_OV7675
  NULL,
#endif
};

LOCAL camsensor_active_fn_type camsensor_active_value_table[CAMSENSOR_ID_MAX] = 
{
#ifdef USE_CAMSENSOR_SIV121A
  camsensor_SIV121A_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_SIC110A
  camsensor_sic110a_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_SID130B
  camsensor_SID130B_active,
#endif

#ifdef USE_CAMSENSOR_MICRON_SIV121D_0M3
  camsensor_SIV121D_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_SP0838
  camsensor_sp0838_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_SP0A18
   camsensor_sp0a18_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_SIV120A
   camsensor_SIV120A_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_GC0329
	camsensor_gc0329_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_GC0311
	camsensor_GC0311_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_SP0828
	camsensor_sp0828_ycbcr_active,
#endif

#ifdef USE_CAMSENSOR_OMNI_VISION_9650
  NULL,
#endif /* USE_CAMSENSOR_OMNI_VISION_9650 */

#ifdef USE_CAMSENSOR_MICRON_MT9P012
 NULL,
 #endif /*USE_CAMSENSOR_MICRON_MT9P012*/

#ifdef USE_CAMSENSOR_OMNI_VISION_2630
  NULL,
#endif /* USE_CAMSENSOR_OMNI_VISION_2630 */

#ifdef USE_CAMSENSOR_VB6800_STM3M2VC
  NULL,
#endif /* USE_CAMSENSOR_VB6800_STM3M2VC */

#ifdef USE_CAMSENSOR_VB6801_STM3M2VC
  NULL,
#endif /* USE_CAMSENSOR_VB6801_STM3M2VC */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_PIEZO
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9T012_PIEZO */

#ifdef USE_CAMSENSOR_MICRON_MT9V111_YCBCR
  NULL,
#endif

#ifdef USE_CAMSENSOR_MICRON_MT9D011_VCM_AF
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D011_VCM_AF */

#ifdef USE_CAMSENSOR_MICRON_MT9D011
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D011 */

#ifdef USE_CAMSENSOR_MICRON_MT9D111_MU2M0
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D111_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU
  NULL,
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU */

#ifdef USE_CAMSENSOR_MICRON_MU1M3YU_MT9M112 
   NULL,
#endif

#ifdef USE_CAMSENSOR_CASIO_NX5300
  NULL,
#endif 
  /* Sharp YCbCr driver init should be called first when both YCbCr, Bayer
   * Modes are supported
   */
#ifdef USE_CAMSENSOR_SHARP_LZOP3733_YCBCR
  NULL,
#endif /* USE_CAMSENSOR_CASIO_NX5300 */

#ifdef USE_CAMSENSOR_SHARP_LZOP3731
  NULL,
#endif /*  USE_CAMSENSOR_SHARP_LZOP3731 */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_VCM_AF
	NULL,
#endif /*USE_CAMSENSOR_MICRON_MT9T012_VCM_AF*/

#ifdef USE_CAMSENSOR_MICRON_MT9T013
	NULL,
#endif /*USE_CAMSENSOR_MICRON_MT9T013*/

  /* this Sony 2.0 MP does not have auto-detect capability.  
   * Therefore, it must featurized and when enabled cannot be 
   * used with any sensor below it 
   */
#ifdef USE_CAMSENSOR_SONY_IU011FS
  NULL,
#endif /* USE_CAMSENSOR_SONY_IU011FS*/

#ifdef USE_CAMSENSOR_SONY_IU011F_PIEZO
  NULL,
#endif /* USE_CAMSENSOR_SONY_IU011F_PIEZO*/

#ifdef USE_CAMSENSOR_SONY_IU018F_PIEZO
  NULL,
#endif /* USE_CAMSENSOR_SONY_IU018F_PIEZO */
  /* this Sony 1.3 MP does not have auto-detect capability.  
   * Therefore, it must be last in the list 
   */

#ifdef USE_CAMSENSOR_SONY_IMX006FQ
  NULL,
#endif /* USE_CAMSENSOR_SONY_IMX006 */

#ifdef USE_CAMSENSOR_GC0309
  NULL,
#endif

#ifdef USE_CAMSENSOR_DB8B63A
  NULL,
#endif

#ifdef USE_CAMSENSOR_OV7675
  NULL,
#endif
};

#endif

LOCAL camsensor_sensor_model_type camsensor_id = CAMSENSOR_ID_MAX;

/* ********************* WARNING WARNING **************************** */
/* ********************* WARNING WARNING **************************** */
/* MUST CHANGE camsensor_sensor_model_type in camsensor.h when        */
/* modifying this table                                               */
/* ********************* WARNING WARNING **************************** */
/* The following table is used for auto detection of sensor. MUST
 * ensure CAMSENSOR_ID_MAX always be less than or equal to the number of entries
 * in  sensor_detect_table.*/


LOCAL boolean (*camsensor_detect_table[])(camsensor_function_table_type *, camctrl_tbl_type *) =
{
#ifdef FEATURE_NI_GPIO
  camsensor_ni_gpio_init

#else /* FEATURE_NI_GPIO */

#ifdef USE_CAMSENSOR_SIV121A
  camsensor_siv121a_init,
#endif

#ifdef USE_CAMSENSOR_SIC110A
  camsensor_sic110a_init,
#endif

#ifdef USE_CAMSENSOR_SID130B
  camsensor_SID130B_init,
#endif

#ifdef USE_CAMSENSOR_MICRON_SIV121D_0M3
  camsensor_siv121d_0m3_init,
#endif

#ifdef USE_CAMSENSOR_SP0838
  camsensor_sp0838_init,
#endif

#ifdef USE_CAMSENSOR_SP0A18
  camsensor_sp0a18_init,
#endif

#ifdef USE_CAMSENSOR_SIV120A
  camsensor_siv120a_init,
#endif

#ifdef USE_CAMSENSOR_GC0329
	camsensor_gc0329_init,
#endif

#ifdef USE_CAMSENSOR_GC0311
	camsensor_GC0311_init,
#endif

#ifdef USE_CAMSENSOR_SP0828
	camsensor_sp0828_init,
#endif

#ifdef USE_CAMSENSOR_OMNI_VISION_9650
  camsensor_ov9650_init,
#endif /*USE_CAMSENSOR_OMNI_VISION_9650 */

#ifdef USE_CAMSENSOR_MICRON_MT9P012
  camsensor_mt9p012_mu5m0_init,
 #endif /*USE_CAMSENSOR_MICRON_MT9P012*/
 
#ifdef USE_CAMSENSOR_OMNI_VISION_2630
  camsensor_ov2630_ov2m0_init,
#endif /*USE_CAMSENSOR_OMNI_VISION_2630 */

#ifdef USE_CAMSENSOR_VB6800_STM3M2VC
 camsensor_vb6800_stm3m2vc_init,
#endif /* USE_CAMSENSOR_VB6800_STM3M2VC */

#ifdef USE_CAMSENSOR_VB6801_STM3M2VC
 camsensor_vb6801_stm3m2vc_init,
#endif /* USE_CAMSENSOR_VB6801_STM3M2VC */

#ifdef USE_CAMSENSOR_MICRON_MT9T012_PIEZO
 camsensor_mt9t012_mu3m0pz_init,
#endif /* USE_CAMSENSOR_MICRON_MT9T012_PIEZO */

#ifdef USE_CAMSENSOR_MICRON_MT9V111_YCBCR
  camsensor_mt9v111_muvgayu_init,
#endif

#ifdef USE_CAMSENSOR_MICRON_MT9D011_VCM_AF
  camsensor_mt9d011_mu2m0vc_init,
#endif /* USE_CAMSENSOR_MICRON_MT9D011_VCM_AF */

#ifdef USE_CAMSENSOR_MICRON_MT9D011
  camsensor_mt9d011_mu2m0_init,
#endif /* USE_CAMSENSOR_MICRON_MT9D011 */

#ifdef USE_CAMSENSOR_MICRON_MT9D111_MU2M0
 camsensor_mt9d111_mu2m0_init,
#endif /* USE_CAMSENSOR_MICRON_MT9D111_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0
 camsensor_mt9d112_mu2m0_init,
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0 */

#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU
 camsensor_mt9d112_mu2m0yu_init,
#endif /* USE_CAMSENSOR_MICRON_MT9D111_MU2M0YU */

#ifdef USE_CAMSENSOR_MICRON_MU1M3YU_MT9M112
  camsensor_mt9m112_mu1m3yu_init,
#endif /* USE_MICRON_MU1M3YU_MT9M112 */

#ifdef USE_CAMSENSOR_CASIO_NX5300
  camsensor_nx5300_cs2m0sm_ccd_init,
#endif /* USE_CAMSENSOR_CASIO_NX5300 */
  /* Sharp YCbCr driver init should be called first when both YCbCr, Bayer
   * Modes are supported
   */
#ifdef USE_CAMSENSOR_SHARP_LZOP3733_YCBCR
  camsensor_lzop3733_sh2m0vcyu_init,
#endif /* USE_CAMSENSOR_SHARP_LZOP3733_YCBCR */

#ifdef USE_CAMSENSOR_SHARP_LZOP3731
  camsensor_lzop3731_init,
#endif /* USE_CAMSENSOR_SHARP_LZOP3731*/

#ifdef USE_CAMSENSOR_MICRON_MT9T012_VCM_AF
  camsensor_mt9t012_mu3m0vc_init,
#endif /*USE_CAMSENSOR_MICRON_MT9T012_VCM_AF*/

#ifdef USE_CAMSENSOR_MICRON_MT9T013
  camsensor_mt9t013_mu3m0_init,
#endif /*USE_CAMSENSOR_MICRON_MT9T013*/

  /* this Sony 2.0 MP does not have auto-detect capability.
     Therefore, it must featurized and when enabled cannot be
     used with any sensor below it */
#ifdef USE_CAMSENSOR_SONY_IU011FS
  camsensor_iu011fs_sn2m0sm_init,
#endif /* USE_CAMSENSOR_SONY_IU011FS */

#ifdef USE_CAMSENSOR_SONY_IU011F_PIEZO
  camsensor_iu011f_sn2m0pz_init,
#endif /* USE_CAMSENSOR_SONY_IU011F_PIEZO*/

#ifdef USE_CAMSENSOR_SONY_IU018F_PIEZO
  camsensor_iu018f_sn3m0pz_init,
#endif /* USE_CAMSENSOR_SONY_IU018F_PIEZO */

  /* This Sony 1.3 MP does not auto-detect, therefore it must be last
     in this list */
#ifdef USE_CAMSENSOR_SONY_IMX006FQ
  camsensor_imx006fq_sn1m3_init,
#endif /* USE_CAMSENSOR_SONY_IMX006FQ */

#ifdef USE_CAMSENSOR_GC0309
  camsensor_gc0309_0p1mp_init,
#endif

#ifdef USE_CAMSENSOR_DB8B63A
  camsensor_DB8V63A_ycbcr_init,
#endif

#ifdef USE_CAMSENSOR_OV7675
  camsensor_ov7675_init
#endif

#endif /* FEATURE_NI_GPIO */
};
 
static void camsensor_create_fps_list
(
  const camsensor_static_params_type * camsensor_params
);

static void camsensor_init_func_tbl(void);
static int32 camsensor_snapshot_fps_list_size = 0;
static int32 camsensor_video_fps_list_size = 0;
static int32 camsensor_snapshot_fps_list[CAMSENSOR_NUM_DEFAULT_FRAME_RATES+1];
static int32 camsensor_video_fps_list[CAMSENSOR_NUM_DEFAULT_FRAME_RATES+1];

#ifdef FEATURE_MDDI_CAMERA
/* to track which connection mode sensor is using */
camsensor_connection_mode_type camsensor_connection_mode = PARALLEL_CONNECTION;

/* input and output parameters to/from CAMMDDI */
cammddi_inout_params_type cammddi_inout_params;
#endif /* FEATURE_MDDI_CAMERA */


/* ********************* WARNING WARNING **************************** */
/* ********************* WARNING WARNING **************************** */
/* MUST CHANGE camsensor_sensor_model_type in camsensor.h when        */
/* modifying this table                                               */
/* ********************* WARNING WARNING **************************** */


/*============================================================================
                        INTERNAL API DECLARATIONS
============================================================================*/
#ifdef FEATURE_MDDI_CAMERA
static boolean camsensor_cammddi_i2c_write(byte reg, byte *data, byte len);
static boolean camsensor_cammddi_i2c_write_byte(byte reg, byte data);
static boolean camsensor_cammddi_i2c_write_word(uint16 offset, uint16 data);
static boolean camsensor_cammddi_i2c_read(byte reg, byte *data, byte len);
static boolean camsensor_cammddi_i2c_read_byte(byte reg, byte *data);
static boolean camsensor_cammddi_i2c_read_word(uint16 reg, uint16 *data);
static boolean camsensor_cammddi_i2c_af_write_byte(byte reg,byte data);
static boolean camsensor_cammddi_i2c_af_read_byte(byte reg,byte *data);
static boolean camsensor_cammddi_i2c_af_write_word(uint16 offset,uint16 data);
static boolean camsensor_cammddi_i2c_af_read_word(uint16 offset,uint16 *data);
static void    camsensor_cammddi_inout_params_setup (camsensor_static_params_type *);
static void    camsensor_cammddi_inout_params_update (camsensor_static_params_type *);
#endif /* FEATURE_MDDI_CAMERA */
static boolean camsensor_parallel_i2c_write(byte reg, byte *data, byte len);
static boolean camsensor_parallel_i2c_write_byte(byte reg, byte data);
static boolean camsensor_parallel_i2c_read_byte(byte reg, byte *data);
static boolean camsensor_parallel_i2c_read(byte reg, byte *data, byte len);
static boolean camsensor_parallel_i2c_write_word(uint16 offset, uint16 data);
static boolean camsensor_parallel_i2c_read_word(uint16 reg, uint16 *data);
static boolean camsensor_parallel_i2c_af_write_byte(byte reg,byte data);
static boolean camsensor_parallel_i2c_af_read_byte(byte reg,byte *data);
static boolean camsensor_parallel_i2c_af_write_word(uint16 offset,uint16 data);
static boolean camsensor_parallel_i2c_af_read_word(uint16 offset,uint16 *data);
static boolean camsensor_sensor_start ( camsensor_static_params_type * );
static boolean camsensor_sensor_video_config (camsensor_static_params_type *);
static boolean camsensor_sensor_raw_snapshot_config (camsensor_static_params_type *);
static boolean camsensor_sensor_snapshot_config (camsensor_static_params_type *);
static void    camsensor_sensor_power_up (void);
static void    camsensor_sensor_power_down (void);
static camera_ret_code_type    camsensor_sensor_set_default_focus (void);
static camera_ret_code_type    camsensor_sensor_move_focus (camsensor_move_focus_enum_type, int32);
static camera_ret_code_type    camsensor_sensor_write_exposure_gain (uint16, uint32);
static float   camsensor_sensor_register_to_real_gain (uint16 reg_gain);
static uint16  camsensor_sensor_real_to_register_gain (float gain);
static camera_ret_code_type    camsensor_sensor_set_frame_rate (uint16 fps);
static camera_ret_code_type    camsensor_sensor_set_possible_frame_rate (uint16, uint16);


static camera_ret_code_type camsensor_sensor_set_brightness (int8 brightness);
static camera_ret_code_type camsensor_sensor_set_contrast   (int8 contrast);
static camera_ret_code_type camsensor_sensor_set_zoom (void *ptr);
static camera_ret_code_type camsensor_sensor_set_ev_compensation(int32 compensation);


#ifdef MSMHW_CAMCLK_PO
#if !defined (T_MSM6275) && !defined (T_MSM6800)
static uint32 camsensor_clk_gcd (uint32 l, uint32 s);
static int    camsensor_clk_mnd_compare (const void *p1, const void *p2);
static void   camsensor_clk_mnd_sort (double *ptr, uint32 size);
static void   camsensor_clk_reduce_error (camsensor_clk_mnd_type *mnd_ptr);
static double camsensor_clk_mnd_average (double const *ptr, uint32 size);
static void   camsensor_clk_quick_mn_search (camsensor_clk_mnd_type *mnd_ptr);
static void   camsensor_clk_exhaustive_mn_search (camsensor_clk_mnd_type *mnd_ptr);
static void   camsensor_clk_mnd_sim (camsensor_clk_mnd_type *mnd_ptr);
static void   camsensor_clk_compute_reg_values (camsensor_clk_mnd_type *mnd_ptr);
static boolean camsensor_clk_calculate_mnd
(
  uint32 source_freq,
  uint32 target_freq,
  uint8  m_reg_bits,
  uint8  duty_cycle
);
#endif /* !defined (T_MSM6275) && !defined (T_MSM6800) */
#endif /* MSMHW_CAMCLK_PO */

/* parallel as default */
static boolean  (*camsensor_i2c_write_ptr)     (byte , byte *, byte )        = camsensor_parallel_i2c_write;
static boolean  (*camsensor_i2c_write_byte_ptr)(byte , byte )                = camsensor_parallel_i2c_write_byte;
static boolean  (*camsensor_i2c_read_byte_ptr) (byte , byte *)               = camsensor_parallel_i2c_read_byte;
static boolean  (*camsensor_i2c_read_ptr)      (byte , byte *, byte)         = camsensor_parallel_i2c_read;
static boolean  (*camsensor_i2c_write_word_ptr)(uint16 offset, uint16 data)  = camsensor_parallel_i2c_write_word;
static boolean  (*camsensor_i2c_read_word_ptr) (uint16 reg, uint16 *data)    = camsensor_parallel_i2c_read_word;
static boolean  (*camsensor_i2c_af_write_byte_ptr)(byte,byte)                = camsensor_parallel_i2c_af_write_byte;
static boolean  (*camsensor_i2c_af_read_byte_ptr)(byte, byte *)              = camsensor_parallel_i2c_af_read_byte;
static boolean  (*camsensor_i2c_af_write_word_ptr)(uint16 offset,uint16 data)= camsensor_parallel_i2c_af_write_word;
static boolean  (*camsensor_i2c_af_read_word_ptr)(uint16 reg,uint16 *data)   = camsensor_parallel_i2c_af_read_word;

#ifdef CAMERA_USE_PMIC_TO_POWER_SENSOR
extern pm_model_type  pm_ft_get_pmic_model(void);
#ifdef CAMERA_VOTES_FOR_PMIC
#error code not present
#endif /* CAMERA_VOTES_FOR_PMIC */
#endif

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
extern uint8 camera_asi;
#endif
/*============================================================================
                        EXTERNAL API DEFINITIONS
============================================================================*/
     
/*===========================================================================
   
FUNCTION      CAMSENSOR_PARALLEL_I2C_WRITE
 
DESCRIPTION
              Write to i2c via parallel interface
 
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
 static boolean camsensor_parallel_i2c_write(byte reg, byte *data, byte len)
{
 
   uint32 i;
   camsensor_i2c_command.addr.reg = reg;
   camsensor_i2c_command.buf_ptr = data;
   camsensor_i2c_command.len = len;
   if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
   {
     /* delay to allow sensor to complete the operation */
     for (i=0; i<100; i++) ;
     return TRUE;
   }
   else
   {
     MSG_HIGH("I2C write failed, reg 0x%x", reg, 0, 0);
     return FALSE;
   }
} /* camsensor_parallel_i2c_write */

/*===========================================================================

FUNCTION      CAMSENSOR_I2C_WRITE
  
DESCRIPTION
              Write to I2C
   
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
 boolean camsensor_i2c_write(byte reg, byte *data, byte len)
{
  return (*camsensor_i2c_write_ptr)(reg , data, len );
}/*camsensor_i2c_write*/


/*===========================================================================
   
FUNCTION       CAMSENSOR_PARALLEL_I2C_READ_BYTE
  
DESCRIPTION
              Read from I2C via parallel mode 
    
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
static boolean camsensor_parallel_i2c_read_byte(byte reg, byte *data)
{
   camsensor_i2c_command.addr.reg = reg;
   camsensor_i2c_command.buf_ptr = data;
   camsensor_i2c_command.len = 1;
   if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
   {
     return TRUE;
   }
   else
   {
     /* retry once */
     if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
     {
       return TRUE;
     }
    /*lint -save -e34 -e40 AMSS common macro */
     MSG_HIGH("I2C read failed, reg 0x%x", reg, 0, 0);
     /*lint -restore */
     return FALSE;
   }
} /* camsensor_parallel_i2c_read_byte */


/*===========================================================================
   
FUNCTION       CAMSENSOR_PARALLEL_I2C_AF_READ_BYTE
  
DESCRIPTION
              Read from I2C via parallel mode 
    
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
static boolean camsensor_parallel_i2c_af_read_byte(byte reg, byte *data)
{
   camsensor_i2c_af_command.addr.reg = reg;
   camsensor_i2c_af_command.buf_ptr = data;
   camsensor_i2c_af_command.len = 1;
   if (i2c_read(&camsensor_i2c_af_command) == I2C_SUCCESS)
   {
     return TRUE;
   }
   else
   {
     /* retry once */
     if (i2c_read(&camsensor_i2c_af_command) == I2C_SUCCESS)
     {
       return TRUE;
     }
    /*lint -save -e34 -e40 AMSS common macro */
     MSG_HIGH("I2C read failed, reg 0x%x", reg, 0, 0);
     /*lint -restore */
     return FALSE;
   }
} /* camsensor_parallel_i2c_read_byte */
/*===========================================================================
 
FUNCTION      CAMSENSOR_PARALLEL_I2C_READ
  
DESCRIPTION
             Read from I2C via parallel mode 

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

static boolean camsensor_parallel_i2c_read(byte reg, byte *data, byte len)
{
   camsensor_i2c_command.addr.reg = reg;
   camsensor_i2c_command.buf_ptr = data;
   camsensor_i2c_command.len = len;
   if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
   {
     return TRUE;
   }
   else
   {
     /* retry once */
     if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
     {
       return TRUE;
     }
   }
   MSG_HIGH("I2C read failed, reg 0x%x", reg, 0, 0);
   return FALSE;
} /* camsensor_parallel_i2c_read */
   
/*===========================================================================
  
FUNCTION      CAMSENSOR_I2C_READ_BYTE
  
DESCRIPTION
              Read from I2C
   
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
  
boolean camsensor_i2c_read_byte(byte reg, byte *data)
{
  return (*camsensor_i2c_read_byte_ptr)(reg , data);
}/*camsensor_i2c_read_byte*/

/*===========================================================================
    
FUNCTION      CAMSENSOR_AF_I2C_READ_BYTE
  
DESCRIPTION
              Read from I2C
   
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
  
boolean camsensor_i2c_af_read_byte(byte reg, byte *data)
{
  return (*camsensor_i2c_af_read_byte_ptr)(reg , data);
}/*camsensor_i2c_read_byte*/

/*===========================================================================
    
FUNCTION      CAMSENSOR_I2C_READ
 
DESCRIPTION
             Read from I2C
 
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
  
boolean camsensor_i2c_read(byte reg, byte *data, byte len)
{
  return (*camsensor_i2c_read_ptr)(reg , data, len);
}/*camsensor_i2c_read*/
  
/*===========================================================================
 
FUNCTION      CAMSENSOR_PARALLEL_I2C_READ_WORD
    
DESCRIPTION
             Read one word from I2C
 
DEPENDENCIES
   None
 
RETURN VALUE
   None
  
SIDE EFFECTS
  None
   
===========================================================================*/
static boolean camsensor_parallel_i2c_read_word(uint16 offset, uint16 *data)
{
  static uint16 swapbytes; 
  uint8  i;
  
  if (data == NULL)
    return FALSE;
 
  camsensor_i2c_command.addr.offset = offset;
  camsensor_i2c_command.buf_ptr     = (byte *)(&swapbytes);
  camsensor_i2c_command.len         = 2;
 
  for (i =0; i < 3; ++i)
  {
    if (i2c_read(&camsensor_i2c_command) == I2C_SUCCESS)
    {
      *data  = (swapbytes & 0x00FF) << 8;
      *data |= (swapbytes & 0xFF00) >> 8;
      return TRUE;
    }
  }
  MSG_ERROR ("I2C read attempt from register 0x%x is failed", offset, 0, 0);
  return FALSE;
} /* camsensor_parallel_i2c_read_word */

/*===========================================================================

FUNCTION      CAMSENSOR_PARALLEL_I2C_AF_READ_WORD
    
DESCRIPTION
             Read one word from I2C
 
DEPENDENCIES
   None
 
RETURN VALUE
   None
  
SIDE EFFECTS
  None
   
===========================================================================*/
static boolean camsensor_parallel_i2c_af_read_word(uint16 offset, uint16 *data)
{
  static uint16 swapbytes; 
  uint8  i;
  
  if (data == NULL)
    return FALSE;
 
  camsensor_i2c_af_command.addr.offset = offset;
  camsensor_i2c_af_command.buf_ptr     = (byte *)(&swapbytes);
  camsensor_i2c_af_command.len         = 2;
 
  for (i =0; i < 3; ++i)
  {
    if (i2c_read(&camsensor_i2c_af_command) == I2C_SUCCESS)
    {
      *data  = (swapbytes & 0x00FF) << 8;
      *data |= (swapbytes & 0xFF00) >> 8;
      return TRUE;
    }
  }
  MSG_ERROR ("I2C read attempt from register 0x%x is failed", offset, 0, 0);
  return FALSE;
} /* camsensor_parallel_i2c_read_word */

/*===========================================================================

FUNCTION      CAMSENSOR_PARALLEL_I2C_WRITE_WORD
 
DESCRIPTION
              Write one word to I2C
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
 
===========================================================================*/
static boolean camsensor_parallel_i2c_write_word(uint16 offset, uint16 data)
{
  static uint16 swapbytes;
  uint8 i;

  swapbytes  = (data & 0xFF00) >> 8;
  swapbytes |= (data & 0x00FF) << 8;
 
  camsensor_i2c_command.addr.offset = offset;
  camsensor_i2c_command.buf_ptr     = (byte *)(&swapbytes);
  camsensor_i2c_command.len         = 2;
 
  for (i = 0; i < 3; ++i)
  {
    if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
    {
      return TRUE;
    }
  } /* camsensor_i2c_read_byte */
  MSG_ERROR ("I2C write attempt to register 0x%x is failed", offset, 0, 0);
  return FALSE;
} /* camsensor_parallel_i2c_write_word */

/*===========================================================================
    
FUNCTION      CAMSENSOR_PARALLEL_I2C_AF_WRITE_WORD
 
DESCRIPTION
              Write one word to I2C
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
 
===========================================================================*/
static boolean camsensor_parallel_i2c_af_write_word(uint16 offset, uint16 data)
{
  static uint16 swapbytes;
  uint8 i;

  swapbytes  = (data & 0xFF00) >> 8;
  swapbytes |= (data & 0x00FF) << 8;
 
  camsensor_i2c_af_command.addr.offset = offset;
  camsensor_i2c_af_command.buf_ptr     = (byte *)(&swapbytes);
  camsensor_i2c_af_command.len         = 2;
 
  for (i = 0; i < 3; ++i)
  {
    if (i2c_write(&camsensor_i2c_af_command) == I2C_SUCCESS)
    {
      return TRUE;
    }
  } /* camsensor_i2c_read_byte */
  MSG_ERROR ("I2C write attempt to register 0x%x is failed", offset, 0, 0);
  return FALSE;
} /* camsensor_parallel_i2c_write_word */

/*===========================================================================
    
FUNCTION      CAMSENSOR_PARALLEL_I2C_WRITE_BYTE

DESCRIPTION
              Write one byte to I2C
     
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
static boolean camsensor_parallel_i2c_write_byte(byte reg, byte data)
{
  static uint8 my_data;
  my_data = data;
  camsensor_i2c_command.addr.reg = reg;
  camsensor_i2c_command.buf_ptr = &my_data;
  camsensor_i2c_command.len = 1;

  if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
  {
    return TRUE;
  }
  else
  {
    /* Retry once */
    if (i2c_write(&camsensor_i2c_command) == I2C_SUCCESS)
    {
      return TRUE;
    }
  /*lint -save -e34 -e40 AMSS common macro */
    MSG_HIGH("I2C write failed, reg 0x%x", reg, 0, 0);
  /*lint -restore */
    return FALSE;
  }
} /* camsensor_parallel_i2c_write_byte */
 
/*===========================================================================
   
FUNCTION      CAMSENSOR_PARALLEL_I2C_AF_WRITE_BYTE

DESCRIPTION
              Write one byte to I2C
     
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
static boolean camsensor_parallel_i2c_af_write_byte(byte reg, byte data)
{
  static uint8 my_data;
  my_data = data;
  camsensor_i2c_af_command.addr.reg = reg;
  camsensor_i2c_af_command.buf_ptr = &my_data;
  camsensor_i2c_af_command.len = 1;

  if (i2c_write(&camsensor_i2c_af_command) == I2C_SUCCESS)
  {
    return TRUE;
  }
  else
  {
    /* Retry once */
    if (i2c_write(&camsensor_i2c_af_command) == I2C_SUCCESS)
    {
      return TRUE;
    }
  /*lint -save -e34 -e40 AMSS common macro */
    MSG_HIGH("I2C write failed, reg 0x%x", reg, 0, 0);
  /*lint -restore */
    return FALSE;
  }
} /* camsensor_parallel_i2c_af_write_byte */
 
/*===========================================================================
   
FUNCTION      CAMSENSOR_I2C_WRITE_BYTE

DESCRIPTION
              Write one byte to I2C
   
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
 
boolean camsensor_i2c_write_byte(byte reg, byte data)
{
  return (*camsensor_i2c_write_byte_ptr)(reg , data);
}/*camsensor_i2c_wrte_byte*/

/*===========================================================================
 
FUNCTION      CAMSENSOR_I2C_AF_WRITE_BYTE

DESCRIPTION
              Write one byte to I2C
   
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
 
boolean camsensor_i2c_af_write_byte(byte reg, byte data)
{
  return (*camsensor_i2c_af_write_byte_ptr)(reg , data);
}/*camsensor_i2c_af_wrte_byte*/
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
boolean camsensor_i2c_read_word(uint16 offset, uint16 *data)
{
  return (*camsensor_i2c_read_word_ptr)(offset , data);
} /* camsensor_i2c_read_word */
   
/*===========================================================================
 
FUNCTION      CAMSENSOR_I2C_AF_READ_WORD
 
DESCRIPTION
              Read one word from I2C
 
DEPENDENCIES
   None
 
RETURN VALUE
   None
 
SIDE EFFECTS
   None
  
===========================================================================*/
boolean camsensor_i2c_af_read_word(uint16 offset, uint16 *data)
{
  return (*camsensor_i2c_af_read_word_ptr)(offset , data);
} /* camsensor_i2c_af_read_word */
   

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
boolean camsensor_i2c_write_word(uint16 offset, uint16 data)
{
  return (*camsensor_i2c_write_word_ptr)(offset , data);
} /* camsensor_i2c_write_word */
   
/*===========================================================================
 
FUNCTION      CAMSENSOR_AF_I2C_WRITE_WORD
 
DESCRIPTION
              Write one word to I2C
  
DEPENDENCIES
   None
 
RETURN VALUE
   None
    
SIDE EFFECTS
   None
   
===========================================================================*/
boolean camsensor_i2c_af_write_word(uint16 offset, uint16 data)
{
  return (*camsensor_i2c_af_write_word_ptr)(offset , data);
} /* camsensor_i2c_af_write_word */
   
#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================
   
FUNCTION      CAMSENSOR_CAMMDDI_I2C_WRITE
 
DESCRIPTION
              Write to i2c via cammddi interface
 
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
static boolean camsensor_cammddi_i2c_write(byte reg, byte *data, byte len)
{
  return cammddi_i2c_write(reg, data, len, &camsensor_i2c_command);
} /* camsensor_cammddi_i2c_write */
#endif /* FEATURE_MDDI_CAMERA */

#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================
   
FUNCTION      CAMSENSOR_CAMMDDI_I2C_WRITE_BYTE
 
DESCRIPTION
              Write to i2c via cammddi interface
 
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
static boolean camsensor_cammddi_i2c_write_byte(byte reg, byte data)
{
  return cammddi_i2c_write_byte(reg, data, &camsensor_i2c_command);
} /* camsensor_cammddi_i2c_write_byte */

/*===========================================================================
   
FUNCTION      CAMSENSOR_CAMMDDI_I2C_AF_WRITE_BYTE
 
DESCRIPTION
              Write to i2c via cammddi interface
 
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
static boolean camsensor_cammddi_i2c_af_write_byte(byte reg, byte data)
{
  return cammddi_i2c_write_byte(reg, data, &camsensor_i2c_af_command);
} /* camsensor_cammddi_i2c_af_write_byte */
#endif /* FEATURE_MDDI_CAMERA */

#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================
   
FUNCTION      CAMSENSOR_CAMMDDI_I2C_WRITE_WORD
 
DESCRIPTION
              Write to i2c via cammddi interface
 
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
static  boolean camsensor_cammddi_i2c_write_word(uint16 offset, uint16 data)
{
  return cammddi_i2c_write_word(offset, data, &camsensor_i2c_command);
} /* camsensor_cammddi_i2c_af_write_word */

/*===========================================================================
   
FUNCTION      CAMSENSOR_CAMMDDI_I2C_AF_WRITE_WORD
 
DESCRIPTION
              Write to i2c via cammddi interface
 
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
static  boolean camsensor_cammddi_i2c_af_write_word(uint16 offset, uint16 data)
{
  return cammddi_i2c_write_word(offset, data, &camsensor_i2c_af_command);
} /* camsensor_cammddi_i2c_af_write_word */
#endif /* FEATURE_MDDI_CAMERA */

#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================
   
FUNCTION       CAMSENSOR_CAMMDDI_I2C_READ
  
DESCRIPTION
              Read from I2C via parallel mode 
    
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
static  boolean camsensor_cammddi_i2c_read(byte reg, byte *data, byte len)
{
  return cammddi_i2c_read(reg, data, len, &camsensor_i2c_command);
} /* camsensor_cammddi_i2c_read */

#endif /* FEATURE_MDDI_CAMERA */

#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================
   
FUNCTION       CAMSENSOR_MDDI_I2C_READ_BYTE
  
DESCRIPTION
              Read from I2C via parallel mode 
    
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
static  boolean camsensor_cammddi_i2c_read_byte(byte reg, byte *data)
{
  return cammddi_i2c_read_byte(reg, data, &camsensor_i2c_command);
} /* camsensor_cammddi_i2c_read_byte */

/*===========================================================================
   
FUNCTION       CAMSENSOR_MDDI_I2C_AF_READ_BYTE
  
DESCRIPTION
              Read from I2C via MDDI mode 
    
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
static  boolean camsensor_cammddi_i2c_af_read_byte(byte reg, byte *data)
{
  return cammddi_i2c_read_byte(reg, data, &camsensor_i2c_af_command);
} /* camsensor_cammddi_i2c_af_read_byte */
#endif /* FEATURE_MDDI_CAMERA */

#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================
   
FUNCTION       CAMSENSOR_CAMMDDI_I2C_READ_WORD
  
DESCRIPTION
              Read from I2C via MDDI mode 
    
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
static  boolean camsensor_cammddi_i2c_read_word(uint16 reg, uint16 *data)
{
  return cammddi_i2c_read_word(reg, data, &camsensor_i2c_command);
} /* camsensor_cammddi_i2c_read_word */

   
   /*===========================================================================

FUNCTION       CAMSENSOR_CAMMDDI_I2C_AF_READ_WORD

DESCRIPTION
              Read from I2C via MDDI  mode 

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
static  boolean camsensor_cammddi_i2c_af_read_word(uint16 reg, uint16 *data)
{
  return cammddi_i2c_read_word(reg, data, &camsensor_i2c_af_command);
} /* camsensor_cammddi_i2c_af_read_word */
#endif /* FEATURE_MDDI_CAMERA */
   


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
  camsensor_static_params_type * camsensor_params /* Static Parameters */
)

{
  boolean ret_val;
  uint16 focus_window_pixel_width, focus_window_pixel_start;
  uint16 focus_window_line_start, focus_window_line_height;

  if (camsensor_params == NULL)
  {
    MSG_FATAL("Null pointer: camsensor_parms", 0, 0, 0);
    return FALSE;
  }

  /* ***** Pre sensor-specific config params - these should   ****** */
  /* ***** be overwritten by the specific sensor call below.  ****** */

  /* *************************************************************** */
  /* *****            Default settings below                  ****** */
  /* *************************************************************** */

  /* ------------------------------------------- */
  /* Configure the extra Camera Layer Parameters */
  /* Point to output of Chromatix - for sensor specific params */
  camsensor_params->chromatix_parms = NULL;

  /* ------------------  Sensor-specific Config -------------- */
  /* Make/model of sensor */
  camsensor_params->sensor_model = CAMSENSOR_ID_MAX;

  /* CCD or CMOS */
  camsensor_params->sensor_type = CAMSENSOR_CMOS;

  /* BAYER or YCbCr */
  camsensor_params->output_format = CAMSENSOR_BAYER;

  /* 8 bit, 10 bit, ... */
  camsensor_params->raw_output_option = CAMSENSOR_8_BIT_DIRECT;

  /* What is the maximum FPS that can be supported by this sensor
     in video mode? */
  camsensor_params->max_video_fps = 15 * Q8;
  /* Application assigned FPS in video mode */
  camsensor_params->video_fps = 15 * Q8;
  /* Snapshot mode operation */
  camsensor_params->max_preview_fps = 15 * Q8;
  camsensor_params->nightshot_fps = 15 * Q8 /2;
  /* May be assigned with max_preview_fps or nightshot_fps. */
  camsensor_params->preview_fps = 15 * Q8;

  /* Sensor output capability */
  camsensor_params->camsensor_width       = 0;
  camsensor_params->camsensor_height      = 0;

  camsensor_params->full_size_width       = 0;
  camsensor_params->full_size_height      = 0;

  camsensor_params->qtr_size_width        = 0;
  camsensor_params->qtr_size_height       = 0;

  camsensor_params->blackrows             = 0;
  camsensor_params->preview_dx_decimation = 0;
  camsensor_params->preview_dy_decimation = 0;

  /* A pointer to the sensor name for EXIF tags                */
  camsensor_params->sensor_name = NULL;

  /* ------------  Auto Exposure Control Config -------------- */
  camsensor_params->aec_enable = FALSE;

  /* ------------  Antibanding Config ------------------------ */
  camsensor_params->pixel_clock = 0;

  /*pixel_clock_per_line includes all blanking time*/
  camsensor_params->pixel_clock_per_line = 0;

  /* ------------  Auto Frame Rate Config (AFR) -------------- */
  /* A subset of Auto Exposure                                 */
  /* Number of entries or possible frame rates on this sensor
     Represents the depth used in the array below */
  camsensor_params->num_possible_frame_rates = 0;
  /* Zero the frame rate array initially */

  memset ((camsensor_params->frame_rate_array), \
    0, sizeof(camsensor_frame_rate_type)*CAMSENSOR_NUM_DEFAULT_FRAME_RATES );


  /* ------------  Auto White Balance Config ----------------- */
  /* AWB -Auto White Balance Parameters */
  camsensor_params->awb_enable = FALSE;

  /* ------------  Auto Focus Config ------------------------- */
  /* AF -Auto Focus Parameters */
  camsensor_params->af_enable = TRUE;

  /* Num steps to go across whole range */
  camsensor_params->num_steps_near_to_far = 0;

  /* Default position at optimum general focus: nearest infinity
     point.  This is in terms of number of steps from near side.
     This, with the number above, defines the search space. */
  camsensor_params->num_steps_near_to_closest_infinity = 0;

  /* Num steps for coarse first phase sweep search */
  camsensor_params->num_gross_steps_between_stat_points = 0;

  /* Num steps for fine second phase sweep search */
  camsensor_params->num_fine_steps_between_stat_points = 0;

  /* Num search points to gather when doing 2nd pass fine search */
  camsensor_params->num_fine_search_points = 0;

  /* option of process type */
  camsensor_params->af_process_type = AF_EXHAUSTIVE_SEARCH;
  /* possible closest position of lens */
  camsensor_params->position_near_end = 0;
  /* default lens position in macro search mode */
  camsensor_params->position_default_in_macro = 0;
  /* boundary between macro and normal search mode */
  camsensor_params->position_boundary = 0;
  /* default lens position in normal search mode */
  camsensor_params->position_default_in_normal = 0;
  /* possible farthest position of lens */
  camsensor_params->position_far_end = 0;

  /* ------------  Flash Support ----------------------------- */
  /* Does the sensor need/use a flash  */
  camsensor_params->support_auto_flash = FALSE;

  /* The trigger point for Auto Flash, this ratio applies as
     follows:
     (target_luma / current luma) > ratio; -> then use flash
     Default value is 2 (in Q0 format; from pre-6550 platforms)
     Value here should be in Q8 format, 2 = 512 in Q8          */
  camsensor_params->auto_flash_target_to_current_ratio = 0;

  /* VFE stat request structure for Auto Focus */
  camsensor_params->vfeAFConfig.minimumY   = CAMSENSOR_DEFAULT_VFE_AF_MIN_Y;
  camsensor_params->vfeAFConfig.maximumY   = CAMSENSOR_DEFAULT_VFE_AF_MAX_Y;

  /* Default window to middle 25% of whole preview frame */

  /* Width of focus window is half of today width for a 25% area */
  focus_window_pixel_width = camsensor_params->camsensor_width >> 1; // divide by 2
  focus_window_pixel_start = camsensor_params->camsensor_width >> 2; // divide by 4

  camsensor_params->vfeAFConfig.firstPixel = focus_window_pixel_start;
  camsensor_params->vfeAFConfig.lastPixel  =
     focus_window_pixel_start+focus_window_pixel_width;

  /* Width of focus window is half of today width for a 25% area */
  focus_window_line_height = camsensor_params->camsensor_height >> 1; // divide by 2
  focus_window_line_start  = camsensor_params->camsensor_height >> 2; // divide by 4

  camsensor_params->vfeAFConfig.firstLine  = focus_window_line_start;
  camsensor_params->vfeAFConfig.lastLine   =
      focus_window_line_start+focus_window_line_height;

  /* PCLK Invert */
  camsensor_params->pclk_invert = FALSE;

  /* Initialize all the sensor specific exif paramters as unavailable */
  camsensor_params->exif_info_from_sensor.exposureTime.active    = FALSE;
  camsensor_params->exif_info_from_sensor.fNumber.active         = FALSE;
  camsensor_params->exif_info_from_sensor.oecf.active            = FALSE;
  camsensor_params->exif_info_from_sensor.shutterSpeed.active    = FALSE;
  camsensor_params->exif_info_from_sensor.aperture.active        = FALSE;
  camsensor_params->exif_info_from_sensor.max_aperture.active    = FALSE;
  camsensor_params->exif_info_from_sensor.focalLength.active     = FALSE;
  camsensor_params->exif_info_from_sensor.exposureIndex.active   = FALSE;
  camsensor_params->exif_info_from_sensor.cfaPattern.active      = FALSE;
  camsensor_params->exif_info_from_sensor.focalLength35mm.active = FALSE;
  camsensor_params->exif_info_from_sensor.sensingMethod.active   = FALSE;

  /* Sensor driver determines if the camif error needs to be
   * ignored. Set the default to FALSE
   */
  camsensor_params->ignore_camif_error = FALSE;

  /* Set the default sensor's features to false */
  camsensor_params->sensor_features.zoom = FALSE;

  /* Initalize the new structure which will be used by the driver files from now on */
  camsensor_dynamic_params.snapshot_changed_fps = FALSE;

  /* ******************************************* */
  /* Call sensor-specific config params function */
  ret_val = camsensor_function_table.camsensor_start( camsensor_params );
  /* ******************************************* */

  #ifdef FEATURE_MDDI_CAMERA
  /* start mddi host bridge or sensor */
  if (camsensor_connection_mode == MDDI_CONNECTION)
  {
    camsensor_cammddi_inout_params_setup(camsensor_params);
    ret_val = cammddi_start(&cammddi_inout_params);
    camsensor_cammddi_inout_params_update(camsensor_params);
  }
#endif /* FEATURE_MDDI_CAMERA */
  MSG_SPRINTF_2(MSG_SSID_CAMERA, MSG_LEGACY_HIGH, "Sensor Model Type Detected: %d, %s",
                (int)camsensor_params->sensor_model, camsensor_params->sensor_name);

  camsensor_create_fps_list(camsensor_params);

  #ifdef     FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
  if(chromatix_parms_per_mobicat_ptr == NULL)
  {
    MSG_FATAL("No memory for chromatix_parms_per_mobicat", 0, 0, 0);
    return FALSE;
  }

  if(camsensor_params->output_format == CAMSENSOR_BAYER)
  {
    /* copy original chromatix info into the space for mobicat */
    memcpy(chromatix_parms_per_mobicat_ptr, camsensor_params->chromatix_parms,
           sizeof(camsensor_chromatix_output_type));
  }
  else
  {
    /* set the buffer to be all 0, because original chromatix is NULL */
    memset(chromatix_parms_per_mobicat_ptr, 0, sizeof(camsensor_chromatix_output_type));
  }

  /* switch chromatix pointer with the pointer on the space for mobicat */
  camsensor_params->chromatix_parms = chromatix_parms_per_mobicat_ptr;
  camera_tune_init();
  #endif //  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H

  /* Save the static param pointer in the new table */
  camctrl_tbl.camsensor_params_ptr = camsensor_params;
  /* camsensor_params may be transfered to camctrl as an argument
     because camsensor_params_ptr in camctrl_tbl should be removed eventually */
  camctrl_validate_features();

  return ret_val;
} /* camsensor_start */



/*===========================================================================

FUNCTION      camsensor_config

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
)
{
  boolean result = FALSE;

  #ifdef FEATURE_MDDI_CAMERA
  /* we want to send sensor configuration to sensors */
  /* put mddi in config mode stop capturing image and config */
  /* the link for reverse packet optimization */
  if (camsensor_connection_mode == MDDI_CONNECTION)
  {
    camsensor_cammddi_inout_params_setup(static_params);
    if (!cammddi_set_config_mode(&cammddi_inout_params))
    {
      MSG_ERROR(" camsensor_config_failed",0,0,0);
      return FALSE;
    }
    camsensor_cammddi_inout_params_update(static_params);
  } 
#endif /* FEATURE_MDDI_CAMERA */

  switch (mode)
  {
    case CAMSENSOR_OP_MODE_PREVIEW:
    case CAMSENSOR_OP_MODE_VIDEO:
#ifdef FEATURE_CAMERA_RAW_PREVIEW_CAPTURE
#error code not present
#endif /*FEATURE_CAMERA_RAW_PREVIEW_CAPTURE*/	
      result = camsensor_function_table.camsensor_video_config((camsensor_static_params_type *) static_params);
      if (result == TRUE)
      {
         /* Update the clock if it was changed during snapshot */
        if (camsensor_dynamic_params.snapshot_changed_fps == TRUE)
        {
          (void)camsensor_function_table.camsensor_set_frame_rate((uint16)static_params->preview_fps);
          camsensor_dynamic_params.snapshot_changed_fps = FALSE;
         }
      }
      break;
    case CAMSENSOR_OP_MODE_SNAPSHOT:
      result = camsensor_function_table.camsensor_snapshot_config((camsensor_static_params_type *) static_params);
      break;
    case CAMSENSOR_OP_MODE_RAW_SNAPSHOT:
      result = camsensor_function_table.camsensor_raw_snapshot_config((camsensor_static_params_type *) static_params);
      break;
    default:
      return FALSE;
  }

#ifdef FEATURE_MDDI_CAMERA 
  /* sensor configuration is completed */
  /* put mddi in capture mode to start capturing images  */
  if (camsensor_connection_mode == MDDI_CONNECTION)
  {
    camsensor_cammddi_inout_params_setup(static_params);
    result = cammddi_set_capture_mode (&cammddi_inout_params);
    camsensor_cammddi_inout_params_update(static_params);
  }  
#endif /*FEATURE_MDDI_CAMERA*/

  return result;
} /* camsensor_config */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
/*===========================================================================

FUNCTION      CAMSENSOR_SELECT_CAMERA

DESCRIPTION
              Selects the specified sensor.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_select_camera(uint8 camera_asi)
{
  switch (camera_asi)
  {
    case 0: /* the primary sensor */
#ifdef USE_CAMSENSOR_MICRON_MU1M3YU_MT9M112
      camsensor_id = CAMSENSOR_MICRON_MU1M3YU_MT9M112;
#else
      camsensor_id = CAMSENSOR_ID_MAX;
#endif /* USE_CAMSENSOR_MICRON_MU1M3YU_MT9M112 */
      break;

    case 1:
#ifdef USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU
      camsensor_id = CAMSENSOR_MICRON_MT9D112_MU2M0YU;
#else
      camsensor_id = CAMSENSOR_ID_MAX;
#endif /* USE_CAMSENSOR_MICRON_MT9D112_MU2M0YU */
      break;

    default:
      camsensor_id = CAMSENSOR_ID_MAX;
      break;
  }
}
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

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
void camsensor_power_on(void)
{

#ifdef CAMERA_USE_PMIC_TO_POWER_SENSOR
pm_model_type pmic_model;
#ifdef CAMERA_VOTES_FOR_PMIC
#error code not present
#endif
#endif /* CAMERA_USE_PMIC_TO_POWER_SENSOR */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  extern uint8 camera_asi;
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

#ifdef T_FFA
#error code not present
#endif

#ifdef CAMERA_USE_PMIC_TO_POWER_SENSOR

 /* VREG_GP2 should be used for the camera on FFAs with PM6658 and H6/H6B baseband board */
  pmic_model= pm_ft_get_pmic_model();

  if((pmic_model==PMIC_IS_PM6658)||(pmic_model==PMIC_IS_PM6653))
   {
     pm_vreg_set_level(PM_VREG_GP2_ID, 2850);
     pm_vreg_control( PM_ON_CMD, PM_VREG_GP2_M);
   }

#ifdef CAMERA_VOTES_FOR_PMIC
#error code not present
#endif /* CAMERA_VOTES_FOR_PMIC */
else {
    pm_vreg_set_level(PM_VREG_MMC_ID, 2850);
    pm_vote_vreg_switch( PM_ON_CMD, PM_VREG_MMC_ID, PM_VOTE_VREG_MMC_APP__MMC );
}
#endif /* CAMERA_USE_PMIC_TO_POWER_SENSOR */

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  	/* Enable the Sensor Module */
	camera_timed_wait(10);

	CAMERA_CONFIG_GPIO(CAMSENSOR1_POWER_PIN);
	CAMERA_CONFIG_GPIO(CAMSENSOR2_POWER_PIN);
	
    ERR("current_camsensor_pair = %d",current_camsensor_pair,0,0);
	if ( current_camsensor_pair < CAMSENSOR_ID_PAIR_END )
	{
		if ( camera_asi == 0)
		{
            camsensor_active_fn_type pfn_active = camsensor_active_value_table[camsensor_pair[current_camsensor_pair].camsensor_first];
            camsensor_unactive_fn_type pfn_unactive = camsensor_unactive_value_table[camsensor_pair[current_camsensor_pair].camsensor_second];
            
            if(pfn_active)
            {
			    gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)pfn_active());
            }
            if(pfn_unactive)
            {
	  		    gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)pfn_unactive());
            }
		}
		else if ( camera_asi == 1)
		{
            camsensor_active_fn_type pfn_active = camsensor_active_value_table[camsensor_pair[current_camsensor_pair].camsensor_second];
            camsensor_unactive_fn_type pfn_unactive = camsensor_unactive_value_table[camsensor_pair[current_camsensor_pair].camsensor_first];
            
            if(pfn_unactive)
            {
			    gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)pfn_unactive());
            }
            if(pfn_active)
            {
	  		    gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)pfn_active());
            }
		}
        else
		{
            camsensor_unactive_fn_type pfn_unactive1 = camsensor_active_value_table[camsensor_pair[current_camsensor_pair].camsensor_first];
            camsensor_unactive_fn_type pfn_unactive2 = camsensor_unactive_value_table[camsensor_pair[current_camsensor_pair].camsensor_second];
            
            if(pfn_unactive1)
            {
			    gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)pfn_unactive1());
            }
            if(pfn_unactive2)
            {
	  		    gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)pfn_unactive2());
            }
		}
	}
  	(void)camsensor_config_camclk_po(camsensor_camclk_po_hz);
  	camera_timed_wait(13);
#else
	camera_timed_wait(13);
	CAMERA_CONFIG_GPIO(CAMIF_EN_N);
    MSG_FATAL("current_camsensor_id================%d %d",current_camsensor_id,CAMSENSOR_ID_MAX,0);
	if ( current_camsensor_id < CAMSENSOR_ID_MAX)
	{
		camsensor_active_fn_type pfn_active = camsensor_active_value_table[current_camsensor_id];
        
		if(pfn_active)
	    {
		    gpio_out(CAMIF_EN_N, (GPIO_ValueType)pfn_active());
	    }
    }
#endif

#else
#ifdef FEATURE_CAMERA_MULTI_SENSOR
	MSG_FATAL("camsensor_power_on camera_asi = %d",camera_asi,0,0);
	camera_timed_wait(13);
	CAMERA_CONFIG_GPIO(CAMSENSOR1_POWER_PIN);
	CAMERA_CONFIG_GPIO(CAMSENSOR2_POWER_PIN);
	
	camera_timed_wait(10);
	if ( camera_asi == 0)
	{
		gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)0);
  		gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)0);
	}
	else if ( camera_asi == 1)
	{
		gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)1);
  		gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)1);
	}
	else
	{
		gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)1);
  		gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)1);
	}
#else
  #ifdef GPIO_CAMIF_EN_ON_V
  CAMERA_CONFIG_GPIO(CAMIF_EN_N);
  #if defined(FEATURE_CAMERA_SP0828)
  gpio_out(CAMIF_EN_N, (GPIO_ValueType)GPIO_CAMIF_EN_OFF_V);
  #else
  gpio_out(CAMIF_EN_N, (GPIO_ValueType)GPIO_CAMIF_EN_ON_V);
  #endif
  #endif
#endif/* FEATURE_CAMERA_MULTI_SENSOR */
#endif

  (void) camsensor_config_camclk_po(camsensor_camclk_po_hz);
  camera_timed_wait(13);
#ifdef FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
  if(NULL != chromatix_parms_per_mobicat_ptr)
  {
    CAM_Free( chromatix_parms_per_mobicat_ptr );
    chromatix_parms_per_mobicat_ptr = NULL;
  }
  chromatix_parms_per_mobicat_ptr = CAM_Malloc( sizeof(camsensor_chromatix_output_type) );
#endif /* FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H */

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  /* Select the appropriate sensor */
  camsensor_select_camera(camera_asi);
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

#ifdef FEATURE_MDDI_CAMERA 
  /* this is auto detect methold between mddi and parallel connection */
  /* if mddi can be powered on, a valid mddi device to attached to MSM */
  /* and mddi link is awake */
  if (cammddi_power_on())
  {
    camsensor_connection_mode        = MDDI_CONNECTION;
    camsensor_i2c_write_ptr          = camsensor_cammddi_i2c_write;
    camsensor_i2c_read_byte_ptr      = camsensor_cammddi_i2c_read_byte;
    camsensor_i2c_read_ptr           = camsensor_cammddi_i2c_read;
    camsensor_i2c_write_byte_ptr     = camsensor_cammddi_i2c_write_byte;
    camsensor_i2c_read_word_ptr      = camsensor_cammddi_i2c_read_word;
    camsensor_i2c_write_word_ptr     = camsensor_cammddi_i2c_write_word;
    camsensor_i2c_af_read_byte_ptr   = camsensor_cammddi_i2c_af_read_byte;
    camsensor_i2c_af_write_byte_ptr  = camsensor_cammddi_i2c_af_write_byte;
    camsensor_i2c_af_read_word_ptr   = camsensor_cammddi_i2c_af_read_word;
    camsensor_i2c_af_write_word_ptr  = camsensor_cammddi_i2c_af_write_word;
  }
  else
  {
    camsensor_connection_mode        = PARALLEL_CONNECTION;
    camsensor_i2c_write_ptr          = camsensor_parallel_i2c_write;
    camsensor_i2c_read_byte_ptr      = camsensor_parallel_i2c_read_byte;
    camsensor_i2c_read_ptr           = camsensor_parallel_i2c_read;
    camsensor_i2c_write_byte_ptr     = camsensor_parallel_i2c_write_byte;
    camsensor_i2c_read_word_ptr      = camsensor_parallel_i2c_read_word;
    camsensor_i2c_write_word_ptr     = camsensor_parallel_i2c_write_word;
    camsensor_i2c_af_read_byte_ptr   = camsensor_parallel_i2c_af_read_byte;
    camsensor_i2c_af_write_byte_ptr  = camsensor_parallel_i2c_af_write_byte;
    camsensor_i2c_af_read_word_ptr   = camsensor_parallel_i2c_af_read_word;
    camsensor_i2c_af_write_word_ptr  = camsensor_parallel_i2c_af_write_word;
  }
#endif /* FEATURE_MDDI_CAMERA */
} /* camsensor_power_on */

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
void camsensor_power_off(void)
{

#ifdef CAMERA_USE_PMIC_TO_POWER_SENSOR
pm_model_type pmic_model;
#endif /*CAMERA_USE_PMIC_TO_POWER_SENSOR */

#ifdef FEATURE_MDDI_CAMERA
  /* turn off power to bridge */
  if (camsensor_connection_mode== MDDI_CONNECTION)
  {
    (void) cammddi_power_off();
  }
#endif /*FEATURE_MDDI_CAMERA*/

  camsensor_unconfig_camclk_po();

#ifdef T_FFA
#error code not present
#endif /* T_FFA */

#ifdef CAMERA_USE_PMIC_TO_POWER_SENSOR
 pmic_model= pm_ft_get_pmic_model();

  if(pmic_model==PMIC_IS_PM6658)
   {
     pm_vreg_control( PM_OFF_CMD,PM_VREG_GP2_M);
   }
#ifdef CAMERA_VOTES_FOR_PMIC
#error code not present
#endif /* CAMERA_VOTES_FOR_PMIC */

else {
    (void)pm_vote_vreg_switch( PM_OFF_CMD, PM_VREG_MMC_ID,
                               PM_VOTE_VREG_MMC_APP__MMC );
}
#endif /* CAMERA_USE_PMIC_TO_POWER_SENSOR */
  
/* Disable the Sensor when not in use */
#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
#ifdef FEATURE_CAMERA_MULTI_SENSOR
	gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)(*camsensor_unactive_value_table[camsensor_pair[current_camsensor_pair].camsensor_first])());
  	gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)(*camsensor_unactive_value_table[camsensor_pair[current_camsensor_pair].camsensor_second])());
#else
	if ( current_camsensor_id < CAMSENSOR_ID_MAX)
	{
		camsensor_unactive_fn_type pfn_unactive = camsensor_unactive_value_table[current_camsensor_id];

		if(pfn_unactive)
	    {
		    gpio_out(CAMIF_EN_N, (GPIO_ValueType)pfn_unactive());
	    }
    }
#endif
#else
#ifdef FEATURE_CAMERA_MULTI_SENSOR
	gpio_out(CAMSENSOR1_POWER_PIN, (GPIO_ValueType)1);
	gpio_out(CAMSENSOR2_POWER_PIN, (GPIO_ValueType)0);  
#else
	#ifdef GPIO_CAMIF_EN_OFF_V
		#if defined(FEATURE_CAMERA_SP0828)
			gpio_out(CAMIF_EN_N, (GPIO_ValueType)GPIO_CAMIF_EN_ON_V);
		#else
			gpio_out(CAMIF_EN_N, (GPIO_ValueType)GPIO_CAMIF_EN_OFF_V);
		#endif
	#endif
#endif
#endif
  /* If the sensor need to be initialized next time when powered
   * on, then you need to clear camsensor_initialized */
  camsensor_initialized = FALSE;

#ifdef FEATURE_CAMERA_MULTI_SENSOR
  /* Deselect the sensor */
  camsensor_id = CAMSENSOR_ID_MAX;
#endif /* FEATURE_CAMERA_MULTI_SENSOR */
} /* camsensor_power_off */

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
void camsensor_power_up(void)
{
#ifdef FEATURE_MDDI_CAMERA
  /* mddi host bridge initialization and detection */
  /* mddi sensor initialization and detection */
  /* parallel sensor initialization and detection in mddi connection */
  if (camsensor_connection_mode== MDDI_CONNECTION)
  {    
    if (!cammddi_init()) return;
    
    if (!camsensor_init()) return;
 
  /* we have detected a sensor in mddi connection */
  /* power up mddi camera */
    camsensor_cammddi_inout_params_setup(NULL);
    (void)cammddi_power_up (&cammddi_inout_params);
  }
  else 
#endif /* FEATURE_MDDI_CAMERA */
  {
    if (camsensor_init() == FALSE)
    {
      return;
    }
  }
 
  camsensor_function_table.camsensor_power_up ();
} /* camsensor_power_up */

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
void camsensor_power_down(void)
{
#ifdef FEATURE_MDDI_CAMERA
  /* stop capturing image before sensor power down */
  if (camsensor_connection_mode== MDDI_CONNECTION)
  {
    (void)cammddi_power_down();
  }   
#endif /*FEATURE_MDDI_CAMERA*/

  camsensor_function_table.camsensor_power_down ();
#ifdef FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
  CAM_Free( chromatix_parms_per_mobicat_ptr );
  chromatix_parms_per_mobicat_ptr = NULL;
#endif /* FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H */
} /* camsensor_power_down */

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
camera_ret_code_type    camsensor_set_default_focus (void)
{
   return (camsensor_function_table.camsensor_set_default_focus());
} /* camsensor_set_default_focus */

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
  CAMERA_FOCUS_SENSOR_FAILED  - if I2C reported an error

SIDE EFFECTS
  None

===========================================================================*/
camera_ret_code_type    camsensor_move_focus (camsensor_move_focus_enum_type direction, int32 num_steps)
{
   return (camsensor_function_table.camsensor_move_focus(direction, num_steps));
} /* camsensor_move_focus */

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
camera_ret_code_type    camsensor_write_exposure_gain (uint16 gain, uint32 line_count)
{
   return (camsensor_function_table.camsensor_write_exposure_gain(gain, line_count));
} /* camsensor_write_exposure_gain */

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
float    camsensor_register_to_real_gain (uint16 reg_gain)
{
   return (camsensor_function_table.camsensor_register_to_real_gain(reg_gain));
} /* camsensor_register_to_real_gain */

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
uint16    camsensor_real_to_register_gain (float gain)
{
   return (camsensor_function_table.camsensor_real_to_register_gain(gain));
} /* camsensor_register_to_real_gain */

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
camera_ret_code_type    camsensor_set_frame_rate (uint16 fps)
{
   return (camsensor_function_table.camsensor_set_frame_rate(fps));
} /* camsensor_set_frame_rate */

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
camera_ret_code_type    camsensor_set_possible_frame_rate (uint16 min_fps, uint16 max_fps)
{
   return (camsensor_function_table.camsensor_set_possible_frame_rate(min_fps, max_fps));
} /* camsensor_set_possible_frame_rate */
/*lint -restore */

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
camera_ret_code_type camsensor_set_brightness (int8 brightness)
{
  return (camsensor_function_table.camsensor_set_brightness (brightness));
} /* camsensor_set_brightness */

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
camera_ret_code_type camsensor_set_contrast (int8 contrast)
{
  return (camsensor_function_table.camsensor_set_contrast (contrast));
} /* camsensor_set_contrast */

/*===========================================================================

FUNCTION:
  CAMSENSOR_SET_ZOOM

DESCRIPTION:
  Entry to sensor specific zoom handler.

CALLERS:
  camera_set_dimensions ()
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
camera_ret_code_type camsensor_set_zoom (void *ptr)
{
  return (camsensor_function_table.camsensor_set_zoom (ptr));
} /* camsensor_set_zoom */


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
boolean camsensor_config_camclk (camsensor_dynamic_params_type* dynamic_params)
{
  uint32 freq=0;

  if (dynamic_params==NULL)
  {
    MSG_ERROR ("camsensor dynamic params is NULL", 0, 0, 0);
    return FALSE;
  }

#ifdef FEATURE_MDDI_CAMERA

  if (camsensor_connection_mode == MDDI_CONNECTION)
  {
    if (dynamic_params->mddi_camif_clk)
    {
      freq = dynamic_params->mddi_camif_clk;
    }
    else
    {
      /* calculate the camif clk required */
      if (dynamic_params->bpp!=0 && dynamic_params->mddi_data_rate!=0)
      {
        int temp = dynamic_params->mddi_data_rate/dynamic_params->bpp;
        freq = temp + (16000000 - temp%16000000);
      }
      else
      {
        return FALSE;
      }
    }
  }
  else
#endif 
  {
    if (dynamic_params->mclk)
    {
      freq = dynamic_params->mclk;
    }
    else
    {
      return FALSE;
    }
  }

  camsensor_camclk_po_hz = camsensor_config_camclk_po(freq);

  return TRUE;

} /* camsensor_config_camclk_po */

/*===========================================================================

FUNCTION      CAMSENSOR_CONFIG_CAMCLK_PO

DESCRIPTION
  Config camclk info.

DEPENDENCIES
  None

RETURN VALUE
  the average camclk.

SIDE EFFECTS
  None

===========================================================================*/
#if defined (T_MSM6275) || defined (T_MSM6800)
#ifndef T_QSC1110
uint32 camsensor_config_camclk_po (uint32 freq)
{
  /* GPIO - 10 */
  CAMERA_CONFIG_GPIO (GP_PDM);
  
  /* Set new PDM0 value */
  HWIO_PDM0_CTL_OUT((0  & HWIO_PDM0_CTL_PDM0_DAT_BMSK) << HWIO_PDM0_CTL_PDM0_DAT_SHFT);
    
  /* Set PDM signal to normal polarity (sense of PDM output not inverted)*/
  HWIO_TCXO_PDM_CTL_OUTM(HWIO_TCXO_PDM_CTL_PDM0_POLARITY_BMSK,(0 << HWIO_TCXO_PDM_CTL_PDM0_POLARITY_SHFT));

  /* Drive backlight by enabling PDM output*/
  HWIO_TCXO_PDM_CTL_OUTM(HWIO_TCXO_PDM_CTL_PDM0_EN_BMSK,(1 << HWIO_TCXO_PDM_CTL_PDM0_EN_SHFT));
  
  return freq;
}
#else
uint32 camsensor_config_camclk_po (uint32 freq)
{
  uint32 avg_camclk_freq = 0;
#ifndef CLK_REGIME_SEL_CAMCLK_EXT
  uint16 M = 0, N = 0, NOT_N_M = 0;
#endif /* CLK_REGIME_SEL_CAMCLK_EXT */

  /* 2000 is the common denominator for defined freq
   * or the reasonable upper limit of rounding */
  #define CD 2000
  uint32 odd = freq % CD;

  /* Round up or down freq */
  if (odd)
  {
    if (odd < CD/2)
    {
      freq -= odd;
    }
    else
    {
      freq += CD - odd;
    }
  }

  /* Set low drive for camclk_po to improve de-sensitivity issue */
  /*lint -save -e737 HW macro */
#ifdef HWIO_GPIO_PAD_HDRIVE_SEL_0_OUTM
  /* 6280 PAD drive is now multiple bits. It is set to low drive at boot */
  HWIO_OUTM(GPIO_PAD_HDRIVE_SEL_0, CAMCLK_PO_HIGH_DRIVE_MASK, CAMCLK_PO_LOW_DRIVE_VALUE);
#endif

  /* GPIO - 13 */
  CAMERA_CONFIG_GPIO (CAMCLK_PO);

  /* Program the required frequency */
#ifdef CLK_REGIME_SEL_CAMCLK_EXT
  avg_camclk_freq = CAMERA_CLK_REGIME_SEL_CAMCLK (freq);
#else /* CLK_REGIME_SEL_CAMCLK_EXT */
  CAMERA_CLK_REGIME_SEL_CAMCLK  (freq);
#endif /* CLK_REGIME_SEL_CAMCLK_EXT */

  /* configure the clk regime */
  CAMERA_CLK_REGIME_ENABLE      (CLK_RGM_CAMCLK_M);

#ifndef CLK_REGIME_SEL_CAMCLK_EXT
  /* Calculate the average frequency actually programmed in the
   * hardware registers, the average freqency might differ
   * slightly from the requested frequency depending on the constraints
   * clk regime imposes on the clock characteristics,
   * viz., min. jitter, duty cycle, etc.
   * This implementation will be moved to clk regime eventually.
   */
#ifdef CAMERA_USES_CAMCLK_MN
  M          = (uint16) (MSM_R(CAMCLK_MN_REG) >> 16);
  NOT_N_M    =  (uint16) (MSM_R(CAMCLK_MN_REG)& (0x0000FFFF));
#else
  M          = (uint16) (MSM_R(CAMCLK_PO_CLK_MD) >> 16);
  NOT_N_M    = (uint16) (MSM_R(CAMCLK_PO_CLK_NS) >> 16);
#endif

  N          = (~NOT_N_M) + M;

  avg_camclk_freq = (uint32) (CAMCLK_PLL_FREQ * ((float) M / N));
#endif /* CLK_REGIME_SEL_CAMCLK_EXT */
  /*lint -restore */

  /*lint -save -e34 -e40 AMSS common macro */
  MSG_HIGH ("camclk_po requested freq = %d, average freq = %d", freq, avg_camclk_freq, 0);
  /*lint -restore */

  /* Return the average frequency */
  return avg_camclk_freq;
} /* camsensor_config_camclk_po */
#endif //#ifndef T_QSC1110
#else  /* MSM6550 */

/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_GCD

DESCRIPTION:
              Calculates GCD of two numbers.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:
  GCD of two numbers.

=========================================================================== */
static uint32 camsensor_clk_gcd (uint32 l, uint32 s)
{
  if ((l % s) == 0)
  {
    return s;
  }
  else
  {
    return camsensor_clk_gcd (s, (l % s));
  }
} /* camsensor_clk_gcd */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_REDUCE_ERROR

DESCRIPTION:
              Reduces the error in M, N, D values.

DEPENDENCIES:
  None

SIDE EFFECTS:
  mnd_ptr fields are modified

RETURN VALUE:
  None

=========================================================================== */
static void camsensor_clk_reduce_error (camsensor_clk_mnd_type *mnd_ptr)
{
  uint32 m_tmp0     = 0;
  uint32 m_tmp1     = 0;

  uint32 n_tmp0     = 0;
  uint32 n_tmp1     = 0;

  int32 n_minus_m0 = 0;
  int32 n_minus_m1 = 0;

  double ratio;
  double err_d;
  double err_u;

  if (mnd_ptr == NULL)
  {
    MSG_FATAL ("Null pointer: mnd_ptr", 0, 0, 0);
    return;
  }

  ratio = (double) mnd_ptr->m_ideal_val / mnd_ptr->n_ideal_val;

  /* Round Down */
  m_tmp0 = mnd_ptr->m_val / 2;

  /* Error by truncating */
  err_d  = m_tmp0;
  err_d  /= mnd_ptr->m_val;
  err_d  *= 2;

  n_tmp0 = (uint32)(((((double)mnd_ptr->n_val) / 2.0) * err_d) + 0.5);

  /* Round Up */
  m_tmp1 = (mnd_ptr->m_val / 2) + 1;

  /* Error by truncation and round */
  err_u  = m_tmp1;
  err_u  /= mnd_ptr->m_val;
  err_u  *= 2;

  n_tmp1 = (uint32)(((((double)mnd_ptr->n_val) / 2.0) * err_u) + 0.5);

  if( (m_tmp0 <= mnd_ptr->max_m_val) &&
      (m_tmp1 > mnd_ptr->max_m_val) )
  {
    mnd_ptr->m_val = m_tmp0;
    mnd_ptr->n_val = n_tmp0;

    return;
  }

  /* Do the checking for n - m */
  n_minus_m0 = (int32)(n_tmp0 - m_tmp0);
  n_minus_m1 = (int32)(n_tmp1 - m_tmp1);

  if( (n_minus_m0 <= (int32)mnd_ptr->max_n_val) &&
      (n_minus_m1 > (int32)mnd_ptr->max_n_val) )
  {
    mnd_ptr->m_val = m_tmp0;
    mnd_ptr->n_val = n_tmp0;
    return;
  }

  if(fabs (ratio - (double) m_tmp0 / n_tmp0) < fabs (ratio - (double) m_tmp1 / n_tmp1))
  {
    mnd_ptr->m_val = m_tmp0;
    mnd_ptr->n_val = n_tmp0;
    return;
  }

  mnd_ptr->m_val = m_tmp1;
  mnd_ptr->n_val = n_tmp1;
  return;
} /* camsensor_clk_reduce_error */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_QUICK_MN_SEARCH

DESCRIPTION:
              Coarse searches of M, N, D values in the allowed range

DEPENDENCIES:
  None

SIDE EFFECTS:
  mnd_ptr fields are modified

RETURN VALUE:
  None

=========================================================================== */
static void camsensor_clk_quick_mn_search (camsensor_clk_mnd_type *mnd_ptr)
{
  uint32 common_fact;

  if (mnd_ptr == NULL)
  {
    MSG_FATAL ("Null pointer: mnd_ptr", 0, 0, 0);
    return;
  }

  common_fact = camsensor_clk_gcd (mnd_ptr->source_freq, mnd_ptr->target_freq);

  mnd_ptr->m_ideal_val = mnd_ptr->target_freq / common_fact;
  mnd_ptr->n_ideal_val = mnd_ptr->source_freq / common_fact;

  mnd_ptr->d_ideal_val = mnd_ptr->n_ideal_val;
  mnd_ptr->d_ideal_val *= mnd_ptr->target_dc;
  mnd_ptr->d_ideal_val /= 50;
  mnd_ptr->d_ideal_val = (int32) mnd_ptr->d_ideal_val;
  mnd_ptr->d_ideal_val /= 2;

  mnd_ptr->m_val = mnd_ptr->m_ideal_val;
  mnd_ptr->n_val = mnd_ptr->n_ideal_val;
  mnd_ptr->d_val = mnd_ptr->d_ideal_val;

  /* Check for overflow but minimize freq error */
  while (mnd_ptr->m_val > mnd_ptr->max_m_val)
  {
    camsensor_clk_reduce_error (mnd_ptr);

    mnd_ptr->d_val = mnd_ptr->n_val;
    mnd_ptr->d_val /= 50;
    mnd_ptr->d_val *= mnd_ptr->target_dc;
    mnd_ptr->d_val = (int32) mnd_ptr->d_val;
    mnd_ptr->d_val /= 2;
  }

  /* Check N for overflow. */
  /* N is programmed as ~(N-M) */
  while((int32)(mnd_ptr->n_val - mnd_ptr->m_val) > (int32)mnd_ptr->max_n_val)
  {
    camsensor_clk_reduce_error (mnd_ptr);

    mnd_ptr->d_val = mnd_ptr->n_val;
    mnd_ptr->d_val *= mnd_ptr->target_dc;
    mnd_ptr->d_val /= 50;
    mnd_ptr->d_val = (int32) mnd_ptr->d_val;
    mnd_ptr->d_val /= 2;
  }

  /* Check bit widths of D. If D is too big, reduce duty cycle. */
  if((2.0 * mnd_ptr->d_val) > mnd_ptr->max_d_val)
  {
    mnd_ptr->d_val = mnd_ptr->max_d_val;
  }

  /* Check D value to see that (M / 2) <= D <= (N - M) */
  if (mnd_ptr->d_val > (mnd_ptr->n_val - mnd_ptr->m_val))
  {
    mnd_ptr->d_val = mnd_ptr->n_val - mnd_ptr->m_val;
  }
  else if (mnd_ptr->d_val < (double)(mnd_ptr->m_val / 2.0))
  {
    mnd_ptr->d_val = (double)(mnd_ptr->m_val / 2.0);
  }

  mnd_ptr->error = mnd_ptr->source_freq;
  mnd_ptr->error /= mnd_ptr->n_val;
  mnd_ptr->error *= mnd_ptr->m_val;
  mnd_ptr->error -= mnd_ptr->target_freq;
  mnd_ptr->error = -mnd_ptr->error;

} /* camsensor_clk_quick_mn_search */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_EXHAUSTIVE_MN_SEARCH

DESCRIPTION:
              Finer search for M, N, D values.

DEPENDENCIES:
  None

SIDE EFFECTS:
  mnd_ptr fields are modified

RETURN VALUE:
  None

=========================================================================== */
static void camsensor_clk_exhaustive_mn_search (camsensor_clk_mnd_type *mnd_ptr)
{

  double m = 0;
  double n = 0;

  double best_m = 0;
  double best_n = 0;
  double best_d = 0;

  double error = 1e15;

  int32 done = 0;

  if (mnd_ptr == NULL)
  {
    MSG_FATAL ("Null pointer: mnd_ptr", 0, 0, 0);
    return;
  }

  n = mnd_ptr->source_freq;
  n /= mnd_ptr->target_freq;
  n = (int32) n;

  best_m = m;
  best_n = n;

  mnd_ptr->ex_error = 1e15;

  while (!done)
  {
    error = mnd_ptr->source_freq;
    error /= n;
    error *= m;
    error -= mnd_ptr->target_freq;
    error = -error;

    /* if the answer is exact, quit. */
    if (error == 0)
    {
      done = 1;
    }

    /* if this is our best result so far record it. */
    if (fabs (error) < fabs (mnd_ptr->ex_error))
    {
      /* check bit-width limits. */
      if ((m <= mnd_ptr->max_m_val) && ((n - m) <= mnd_ptr->max_n_val))
      {
        mnd_ptr->ex_error = error;
        best_m = m;
        best_n = n;
      }
    }

    if (error < 0)
    {
      n++;
      m = mnd_ptr->target_freq;
      m /= mnd_ptr->source_freq;
      m *= n;
      m = (int32) (m);
    }
    else
    {
      m++;
    }

    /* If we run out of bits then don't continue to search */
    if (((m - 2) > mnd_ptr->max_m_val) || (((n - m) - 2) > mnd_ptr->max_n_val))
    {
      done = 1;
    }
  }

  best_d = best_n;
  best_d *= mnd_ptr->target_dc;
  best_d /= 50;
  best_d = (int32) best_d;
  best_d /= 2;

  /* Check bit widths of D.
   * If D is too big reduce Duty cycle.
   */
  if (best_d > mnd_ptr->max_d_val / 2.0)
  {
    best_d = mnd_ptr->max_d_val / 2.0;
  }

  /* Check D value to see that (M / 2) <= D <= (N - M) */
  if (best_d > (best_n - best_m))
  {
    best_d = best_n - best_m;
  }
  else if (best_d < (best_m / 2))
  {
    best_d = best_m / 2;
  }

  if (fabs (mnd_ptr->ex_error) <= fabs (mnd_ptr->error))
  {
    mnd_ptr->m_val = (uint32) best_m;
    mnd_ptr->n_val = (uint32) best_n;
    mnd_ptr->d_val = (uint32) best_d;
  }
} /* camsensor_clk_exhaustive_mn_search */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_MND_COMPARE

DESCRIPTION:
              Compares two numbers.

DEPENDENCIES:
  None.

SIDE EFFECTS:


RETURN VALUE:
   1 - First argument is more than Second argument.
   0 - First argument is equal to Second argument.
  -1 - First argument is less than Second argument.

=========================================================================== */
static int camsensor_clk_mnd_compare (const void *p1, const void *p2)
{
  double i;
  double j;

  if (p1 == NULL)
  {
    MSG_FATAL ("Null pointer: p1", 0, 0, 0);
    return 0;
  }

  if (p2 == NULL)
  {
    MSG_FATAL ("Null pointer: p2", 0, 0, 0);
    return 0;
  }

  i = *((double *) p1);
  j = *((double *) p2);

  if (i > j)
  {
    return (1);
  }

  if (i < j)
  {
    return (-1);
  }

  return (0);
} /* camsensor_clk_mnd_compare */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_MND_SORT

DESCRIPTION:
              Calculates M, N, D values to provide required MSM clock to the
              camera sensor.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:
  TRUE  - Success
  FALSE - Failure

=========================================================================== */
static void camsensor_clk_mnd_sort (double *ptr, uint32 size)
{
  if (ptr == 0)
  {
    MSG_FATAL("Null pointer: ptr", 0, 0, 0);
    return;
  }

  qsort ((void *) ptr, size, sizeof (double), camsensor_clk_mnd_compare);
} /* camsensor_clk_mnd_sort */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_MND_AVERAGE

DESCRIPTION:
              Calculates average of an array

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

RETURN VALUE:
  Average

=========================================================================== */
static double camsensor_clk_mnd_average (double const *ptr, uint32 size)
{
  double total = 0;
  uint32 i;

  if (ptr == 0)
  {
    MSG_HIGH ("ptr == NULL", 0, 0, 0);
    return 0.0;
  }

  for (i = 0; i < size; ++i)
  {
    total += ptr [i];
  }

  return (total / size);
} /* camsensor_clk_mnd_average */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_MND_SIM

DESCRIPTION:
              Simulates the characteristics of the target clock

DEPENDENCIES:
  None

SIDE EFFECTS:
  mnd_ptr fields are modified

RETURN VALUE:
  None

=========================================================================== */
static void camsensor_clk_mnd_sim (camsensor_clk_mnd_type *mnd_ptr)
{
  uint32 i = 0;
  uint32 j = 0;

  double accum = 0;
  double *out_clk;

  double high_num   = 0;
  double total_ones = 0;
  double cycle_num  = 0;

  double *duty_cycle;
  double *periods;
  double *high_pulse_width;
  double *low_pulse_width;

  if (mnd_ptr == NULL)
  {
    MSG_FATAL("Null pointer: mnd_ptr", 0, 0, 0);
    return;
  }

  if (mnd_ptr->stats.enable == FALSE)
  {
    return;
  }

  out_clk = malloc (2 * mnd_ptr->n_val * sizeof (double) + sizeof (double));

  if (out_clk == NULL)
  {
    MSG_FATAL("Null pointer: out_clk", 0, 0, 0);
    return;
  }

  duty_cycle = malloc (2 * mnd_ptr->n_val * sizeof (double) + sizeof (double));

  if (duty_cycle == NULL)
  {
    MSG_FATAL("Null pointer: duty_cycle", 0, 0, 0);
    free(out_clk);
    out_clk = NULL;
    return;
  }

  periods = malloc (2 * mnd_ptr->n_val * sizeof (double) + sizeof (double));

  if (periods == NULL)
  {
    MSG_FATAL("Null pointer: periods", 0, 0, 0);
    free(out_clk);
    out_clk = NULL;
    free(duty_cycle);
    duty_cycle = NULL;
    return;
  }

  high_pulse_width = malloc (2 * mnd_ptr->n_val * sizeof (double) + sizeof (double));

  if (high_pulse_width == NULL)
  {
    MSG_FATAL("Null pointer: high_pulse_width", 0, 0, 0);
    free(out_clk);
    out_clk = NULL;
    free(duty_cycle);
    duty_cycle = NULL;
    free(periods);
    periods = NULL;
    return;
  }

  low_pulse_width = malloc (2 * mnd_ptr->n_val * sizeof (double) + sizeof (double));

  if (low_pulse_width == NULL)
  {
    MSG_FATAL("Null pointer: low_pulse_width", 0, 0, 0);
    free(out_clk);
    out_clk = NULL;
    free(duty_cycle);
    duty_cycle = NULL;
    free(periods);
    periods = NULL;
    free(high_pulse_width);
    high_pulse_width = NULL;
    return;
  }

  for (i = 0; i < mnd_ptr->n_val ; i++)
  {
    /* Need to obtain the psedo code for the followign
     * section of code.
     */
    if ( (accum < mnd_ptr->m_val) &&
         (accum >= mnd_ptr->m_val / 2.0) )
    {
      out_clk [2 * i - 1] = 1;
      out_clk [2 * i]     = 1;

      if (accum < mnd_ptr->d_val)
      {
        out_clk [2 * i + 1] = 1;
      }
      else
      {
        out_clk [2 * i + 1] = 0;
      }
    }
    else if ( (accum < (mnd_ptr->d_val + mnd_ptr->m_val)) &&
              (accum >= (mnd_ptr->d_val + mnd_ptr->m_val / 2.0)) )
    {
      out_clk [2 * i - 1] = 0;
      out_clk [2 * i]     = 0;
      out_clk [2 * i + 1] = 0;
    }
    else if ( accum < mnd_ptr->d_val )
    {
      out_clk [2 * i]     = 1;
      out_clk [2 * i + 1] = 1;
    }
    else
    {
      out_clk [2 * i]     = 0;
      out_clk [2 * i + 1] = 0;
    }

    accum = accum + mnd_ptr->m_val;

    if (accum >= mnd_ptr->n_val)
    {
      accum = accum - mnd_ptr->n_val;
    }
  }

  out_clk [2 * mnd_ptr->n_val] = 1;

  for (i = 0; i <= 2 * mnd_ptr->n_val; i++)
  {
    if ((cycle_num > 0) && (i > 0) &&
        (out_clk [i]) && !(out_clk [i - 1]))
    {
      duty_cycle[j]        = high_num / cycle_num;
      periods[j]           = cycle_num;
      high_pulse_width[j]  = high_num;
      low_pulse_width[j++] = cycle_num - high_num;
      high_num             = 0;
      cycle_num            = 0;
    }

    if (out_clk [i])
    {
      high_num++;
    }

    cycle_num++;
    total_ones += out_clk [i];
  }

  /* Inplace sorting algorithm */
  camsensor_clk_mnd_sort (duty_cycle, j);
  camsensor_clk_mnd_sort (periods, j);
  camsensor_clk_mnd_sort (high_pulse_width, j);
  camsensor_clk_mnd_sort (low_pulse_width, j);

  mnd_ptr->stats.min_dc = duty_cycle [0];
  mnd_ptr->stats.max_dc = duty_cycle [mnd_ptr->m_val - 1];
  mnd_ptr->stats.ave_dc = (total_ones - 1.0) / (2.0 * mnd_ptr->n_val);

  mnd_ptr->stats.ave_period = camsensor_clk_mnd_average(periods, j);
  mnd_ptr->stats.min_period = periods [0];
  mnd_ptr->stats.max_period = periods [mnd_ptr->m_val - 1];

  mnd_ptr->stats.min_ph = high_pulse_width[0];
  mnd_ptr->stats.max_ph = high_pulse_width[mnd_ptr->m_val - 1];

  mnd_ptr->stats.min_pl = low_pulse_width[0];
  mnd_ptr->stats.max_pl = low_pulse_width[mnd_ptr->m_val - 1];

  /* When M = 1 The M/N:D counter does Modulo-N division
   * When M = 2 The M/N:D counter does Half Integer division
   * When M > 2 The M/N:D counter does M/N division.
   * Set cycle_range when not in special modes.
   */
  if (mnd_ptr->m_val > 2)
  {
    mnd_ptr->stats.cycle_range = 1.0;
  }

  /* Calculate min/max pulse width */
  mnd_ptr->stats.min_pw = mnd_ptr->stats.min_ph;
  mnd_ptr->stats.min_pw /= 2;
  mnd_ptr->stats.min_pw /= mnd_ptr->source_freq;

  mnd_ptr->stats.max_pw = mnd_ptr->stats.max_ph;
  mnd_ptr->stats.max_pw /= 2;
  mnd_ptr->stats.max_pw /= mnd_ptr->source_freq;

  /* Calcualte min/max low time */
  mnd_ptr->stats.min_lw = mnd_ptr->stats.min_pl / (2.0 * mnd_ptr->source_freq);
  mnd_ptr->stats.max_lw = mnd_ptr->stats.max_pl / (2.0 * mnd_ptr->source_freq);

  /* Calculate Minimum Period */
  mnd_ptr->stats.small_period = mnd_ptr->n_val;
  mnd_ptr->stats.small_period /= mnd_ptr->m_val;
  mnd_ptr->stats.small_period *= 2;
  mnd_ptr->stats.small_period = (int32) (mnd_ptr->stats.small_period);

  mnd_ptr->stats.min_period = mnd_ptr->stats.small_period;
  mnd_ptr->stats.min_period /= 2;
  mnd_ptr->stats.min_period /= mnd_ptr->source_freq;

  /* Calculate Maximum Period */
  mnd_ptr->stats.large_period = mnd_ptr->n_val;
  mnd_ptr->stats.large_period /= mnd_ptr->m_val;
  mnd_ptr->stats.large_period *= 2;
  mnd_ptr->stats.large_period += mnd_ptr->stats.cycle_range;
  mnd_ptr->stats.large_period = (int32) (mnd_ptr->stats.large_period);

  mnd_ptr->stats.max_period = mnd_ptr->stats.large_period;
  mnd_ptr->stats.max_period /= 2;
  mnd_ptr->stats.max_period /= mnd_ptr->source_freq;

  /* Calculate True output frequency */
  mnd_ptr->out_freq =  mnd_ptr->source_freq;
  mnd_ptr->out_freq *= mnd_ptr->m_val;
  mnd_ptr->out_freq /= mnd_ptr->n_val;

  /* Calculate Ouput frequency error */
  mnd_ptr->freq_error = fabs (mnd_ptr->target_freq - mnd_ptr->out_freq);

  /* Calculate Average Period */
  mnd_ptr->stats.ave_period = 1.0 / mnd_ptr->out_freq;

  /* Calculate the Cycle to Cycle jitter. */
  if(mnd_ptr->m_val <= 2)
  {
    mnd_ptr->stats.c2c_jitter = 0.0;
  }
  else
  {
    mnd_ptr->stats.c2c_jitter = 1.0;
    mnd_ptr->stats.c2c_jitter /= mnd_ptr->source_freq;
    mnd_ptr->stats.c2c_jitter /= 2.0;
  }

  /* Free the memory allocated */
  free (out_clk);
  free (duty_cycle);
  free (periods);
  free (high_pulse_width);
  free (low_pulse_width);
} /* camsensor_clk_mnd_sim */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_COMPUTE_REG_VALUES

DESCRIPTION:
              Calculates M, N, D register values suitable to write into the
              h/w registers.

DEPENDENCIES:
  None

SIDE EFFECTS:
  mnd_ptr fields are modified

RETURN VALUE:
  None

=========================================================================== */
static void camsensor_clk_compute_reg_values (camsensor_clk_mnd_type *mnd_ptr)
{

  if (mnd_ptr == 0)
  {
    MSG_FATAL("Null pointer: mnd_ptr", 0, 0, 0);
    return;
  }

  mnd_ptr->m_reg_val = ((uint16)mnd_ptr->m_val) & ((uint16)mnd_ptr->max_m_val);

  mnd_ptr->n_reg_val = ((uint16)(~(mnd_ptr->n_val - mnd_ptr->m_val))) & ((uint16)mnd_ptr->max_n_val);

  mnd_ptr->d_reg_val = ((uint16)(~((uint32)(2 * mnd_ptr->d_val)))) & ((uint16)mnd_ptr->max_d_val);
} /* camsensor_clk_compute_reg_values */


/*===========================================================================

FUNCTION:     CAMSENSOR_CLK_CALCULATE_MND

DESCRIPTION:
              Calculates M, N, D values to provide required MSM clock to the
              camera sensor.

DEPENDENCIES:
  None

SIDE EFFECTS:
  camsensor_clk_mnd values are set.

RETURN VALUE:
  TRUE  - Success
  FALSE - Failure

=========================================================================== */
static boolean camsensor_clk_calculate_mnd
(
  /* Input frequency to M/N:D counter */
  uint32 source_freq,
  /* Output frequency of M/N:D counter */
  uint32 target_freq,
  /* Certain MSM targets have different M register bits */
  uint8  m_reg_bits,
  /* Duty cycle of the target clock */
  uint8  duty_cycle
)
{
  uint32 temp_max_d_val; /* To make Lint happy */

  /* Initialization */
  camsensor_clk_mnd.m_bits = m_reg_bits;
  camsensor_clk_mnd.n_bits = N_REG_BITS;
  camsensor_clk_mnd.d_bits = D_REG_BITS;

  camsensor_clk_mnd.max_m_val = (0x00000001 << camsensor_clk_mnd.m_bits) - 1;
  camsensor_clk_mnd.max_n_val = (0x00000001 << camsensor_clk_mnd.n_bits) - 1;

  temp_max_d_val = (0x00000001 << camsensor_clk_mnd.d_bits) - 1;
  camsensor_clk_mnd.max_d_val = (double)temp_max_d_val;

  /* Disable the statistics */
  camsensor_clk_mnd.stats.enable = FALSE;

  /* Check the number of bits for M, N, D registers */
  if((camsensor_clk_mnd.m_bits <  1) || (camsensor_clk_mnd.n_bits <  1) ||
     (camsensor_clk_mnd.d_bits <  1) || (camsensor_clk_mnd.m_bits > 32) ||
     (camsensor_clk_mnd.n_bits > 32) || (camsensor_clk_mnd.d_bits > 32))
  {
    MSG_HIGH ("Invalid range for M, N, D registers bits.", 0, 0, 0);
    return FALSE;
  }

  camsensor_clk_mnd.source_freq = source_freq;
  camsensor_clk_mnd.target_freq = target_freq;
  camsensor_clk_mnd.target_dc   = duty_cycle;

  if (camsensor_clk_mnd.target_freq > (camsensor_clk_mnd.source_freq * 2 / 3))
  {
    MSG_HIGH ("Target clk freq < 2 / 3 of src clk freq", 0, 0, 0);
    return FALSE;
  }

  if (camsensor_clk_mnd.target_freq > (camsensor_clk_mnd.source_freq / 2))
  {
    MSG_HIGH ("Target clk freq < 1 / 2 of src clk freq", 0, 0, 0);
    return FALSE;
  }

  /* Do a quick search based on ideal values. */
  camsensor_clk_quick_mn_search (&camsensor_clk_mnd);

  /* Perform an exhaustive search of register bit space,
   * If the Quick search did not find a perfect answer.
   */
  if (camsensor_clk_mnd.error != 0)
  {
    /* should only update the m, n, d values only if
     * the error is less than the quick search
     */
    camsensor_clk_exhaustive_mn_search (&camsensor_clk_mnd);
  }

  /* Put in register format.
   * Implement exp function
   */
  camsensor_clk_compute_reg_values (&camsensor_clk_mnd);

  /* Simulate the M/N:D with these parameters */
  camsensor_clk_mnd_sim (&camsensor_clk_mnd);

  return TRUE;
} /* camsensor_clk_calculate_mnd */


/*===========================================================================

FUNCTION      CAMSENSOR_CONFIG_CAMCLK_PO

DESCRIPTION
              Configure camclk_po.

DEPENDENCIES
  None

RETURN VALUE
  the average camclk.

SIDE EFFECTS
  None

===========================================================================*/
uint32 camsensor_config_camclk_po (uint32 freq)
{
  uint16 M = 0, NOT_N_M = 0, D = 0;

  /* 2000 is the common denominator for defined freq
   * or the reasonable upper limit of rounding */
  #define CD 2000
  uint32 odd = freq % CD;

  /* Give out_freq a value for return value. Value could be refined by
   * camsensor_clk_calculate_mnd() if stats are enabled. */
  camsensor_clk_mnd.out_freq = freq;

  /* Round up or down freq */
  if (odd)
  {
    if (odd < CD/2)
    {
      freq -= odd;
    }
    else
    {
      freq += CD - odd;
    }
  }

  switch (freq)
  {
    case 6000000: /* 6.00MHz */
      M = 0x36B;        NOT_N_M = 0x936A;     D = 0x8FFF;
      break;

    case 6144000: /* 6.144MHz Jitter free */
      M = 0x3FF;        NOT_N_M = 0x841E;     D = 0x801F;
      break;

    case 9000000: /* 9.00MHz */
      M = 0x177;        NOT_N_M = 0xE176;     D = 0xDFFF;
      break;

    case 10000000: /* 10.00MHz */
      M = 0x271;        NOT_N_M = 0xD270;     D = 0xCFFF;
      break;

    case 11000000: /* 11.00MHz */
      M = 0x55F;        NOT_N_M = 0xA55E;     D = 0x9FFF;
      break;

    case 12000000: /* 12.00MHz */
      M = 0x0F23;       NOT_N_M = 0x1722;     D = 0x07FF;
      break;

    case 12288000: /* 12.288MHz  Jitter Free */
      M = 0x0FFF;       NOT_N_M = 0x100E;     D = 0x000F;
      break;

    case 13000000: /* 13.00MHz */
      M = 0x659;        NOT_N_M = 0xA658;     D = 0x9FFF;
      break;

    case 13500000: /* 13.50MHz */
      M = 0x465;        NOT_N_M = 0xC464;     D = 0xBFFF;
      break;

    case 14000000: /* 14.00MHz */
      M = 0x36B;        NOT_N_M = 0xD36A;     D = 0xCFFF;
      break;

    case 15000000: /* 15.00MHz */
      M = 0x271;        NOT_N_M = 0xE270;     D = 0xDFFF;
      break;

    case 16000000: /* 16.00MHz */
      M = 0x07D;        NOT_N_M = 0xFA7C;     D = 0xF9FF;
      break;

    case 17000000: /* 17.00MHz */
      M = 0x84D;        NOT_N_M = 0xA84C;     D = 0x9FFF;
      break;

    case 18000000: /* 18.00MHz */
      M = 0x177;        NOT_N_M = 0xF176;     D = 0xEFFF;
      break;

    case 19000000: /* 19.00MHz */
      M = 0x974;        NOT_N_M = 0xA946;     D = 0x9FFF;
      break;

    case 19650000: /* 19.65MHz */
      M = 0x1000;        NOT_N_M = 0x6FFF;     D = 0x6FFF;
      break;

    case 20000000: /* 20.00MHz */
      M = 0x217;        NOT_N_M = 0xEA70;     D = 0xE7FF;
      break;

    case 24576000: /* 24.576MHz  Jitter Free */
      M = 0x0010;       NOT_N_M = 0xFF8F;     D = 0xFF7F;
      break;

    case 24000000: /* 24.00MHz */
      M = 0x07D;        NOT_N_M = 0xFC7C;     D = 0xFBFF;
      break;

    case 39300000: /* 39.30MHz */
      M = 0x2000;        NOT_N_M = 0x7FFF;     D = 0x7FFF;
      break;

    case 48000000: /* 48.00MHz */
      M = 0x3E03;       NOT_N_M = 0x4002;     D = 0x01FF;
      break;

    case 49152000: /* 49.152MHz Jitter free */
      M = 0x0008;       NOT_N_M = 0xFFE7;     D = 0xFFDF;
      break;

    default:
    {
      if (camsensor_clk_calculate_mnd(CAMCLK_PLL_FREQ, freq, M_REG_BITS, TARGET_DUTY_CYCLE))
      {
        M = camsensor_clk_mnd.m_reg_val;
        NOT_N_M = camsensor_clk_mnd.n_reg_val;
        D = camsensor_clk_mnd.d_reg_val;
      }
      break;
    }
  }

  HWIO_OUTM(PAD_HDRIVE_SEL_0, CAMCLK_PO_HIGH_DRIVE_MASK, CAMCLK_PO_LOW_DRIVE_VALUE);

  /* GPIO - 13 */
  CAMERA_CONFIG_GPIO (CAMCLK_PO);

#ifdef FEATURE_CLK_MGR
  clk_regime_configure_camclk( M, NOT_N_M, D);
#else /* FEATURE_CLK_MGR */
  /*lint -save -e737 HW macro */
  /* disable M/N counter */
  HWIO_OUTM(CAMCLK_PO_CLK_NS, HWIO_CAMCLK_PO_CLK_NS_MNCNTR_SEL_BMSK, 0);

  /* select raw source - 0x04 is PLL0 */
  HWIO_OUTM(CAMCLK_PO_CLK_NS, HWIO_CAMCLK_PO_CLK_NS_CLK_SRC_SEL_BMSK, 4 << HWIO_CAMCLK_PO_CLK_NS_CLK_SRC_SEL_SHFT);

  /* configure M value */
  HWIO_OUTM(CAMCLK_PO_CLK_MD, HWIO_CAMCLK_PO_CLK_MD_M_VAL_BMSK, M << HWIO_CAMCLK_PO_CLK_MD_M_VAL_SHFT);

  /* configure D value */
  HWIO_OUTM(CAMCLK_PO_CLK_MD, HWIO_CAMCLK_PO_CLK_MD_D_VAL_BMSK, D << HWIO_CAMCLK_PO_CLK_MD_D_VAL_SHFT);

  /* configure N value */
  HWIO_OUTM(CAMCLK_PO_CLK_NS, HWIO_CAMCLK_PO_CLK_NS_N_VAL_BMSK, NOT_N_M << HWIO_CAMCLK_PO_CLK_NS_N_VAL_SHFT);

  /* enable M/N counter */
  HWIO_OUTM(CAMCLK_PO_CLK_NS, HWIO_CAMCLK_PO_CLK_NS_MNCNTR_SEL_BMSK, 1 << HWIO_CAMCLK_PO_CLK_NS_MNCNTR_SEL_SHFT);
  /*lint -restore */
#endif /* FEATURE_CLK_MGR */

  /* configure the clk regime */
  CAMERA_CLK_REGIME_ENABLE (CLK_RGM_CAMCLK_M);

  /* return actual output frequency which may be slightly different than
   * requested frequency */
  return (uint32)camsensor_clk_mnd.out_freq;
} /* camsensor_config_camclk_po */

#endif /* T_MSM6275 || T_MSM6800 */

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
#ifndef T_QSC1110
void camsensor_unconfig_camclk_po(void)
{
#ifndef FEATURE_DSP
    /* GPIO - 10 */
    CAMERA_CONFIG_GPIO (GPIO_OUTPUT_10); // GP_PDM
    gpio_out(GPIO_OUTPUT_10,0);
#endif
    /* Set new PDM0 value */
    HWIO_PDM0_CTL_OUT((0  & HWIO_PDM0_CTL_PDM0_DAT_BMSK) << HWIO_PDM0_CTL_PDM0_DAT_SHFT);
    
    /* Drive backlight by enabling PDM output*/
    HWIO_TCXO_PDM_CTL_OUTM(HWIO_TCXO_PDM_CTL_PDM0_EN_BMSK,(0 << HWIO_TCXO_PDM_CTL_PDM0_EN_SHFT));
} /* camsensor_unconfig_camclk_po */
#else
void camsensor_unconfig_camclk_po(void)
{

  /* Shutdown CAM_CLK output to sensor */
  CAMERA_CONFIG_GPIO (CAMCLK_PO_OFF);

  /* Disable the clock regime */
  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_CAMCLK_M);

} /* camsensor_unconfig_camclk_po */
#endif
#endif /* MSMHW_CAMCLK_PO */

#ifndef CAMERA_USES_XO4_CLK
/*===========================================================================

FUNCTION      CAMSENSOR_CONFIG_GP_CLK

DESCRIPTION
              Configure GP_CLK.

DEPENDENCIES
  For 6550 and 6800, the rotary switch should be set to 4.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void camsensor_config_gp_clk
(
 uint16 m,
 uint16 not_n_m,
 uint16 not_2_d
)
{
#define HWIO_PAD_HDRIVE_SEL_2_HDRIVE_GP_CLK_BMSK 0x10000
#define HWIO_PAD_HDRIVE_SEL_2_HDRIVE_GP_CLK_SHFT 0x10

/*lint -save -e737 HW macro */
#if defined (T_MSM6275) || defined (T_MSM6800)
#ifdef HWIO_GPIO_PAD_HDRIVE_SEL_2_OUTM
#error code not present
#endif /* HWIO_GPIO_PAD_HDRIVE_SEL_2_OUTM */
#else /* Not 6275 or 6800, mainly 6550 */
  HWIO_OUTM(PAD_HDRIVE_SEL_2, HWIO_PAD_HDRIVE_SEL_2_HDRIVE_GP_CLK_BMSK, 0 << HWIO_PAD_HDRIVE_SEL_2_HDRIVE_GP_CLK_SHFT);
#endif /* T_MSM */
  CAMERA_CONFIG_GPIO (GP_CLK);
      /* disable M/N counter */
  HWIO_OUTM(GP_CLK_NS, HWIO_GP_CLK_NS_MNCNTR_SEL_BMSK, 0);

  /* select raw source - 0x04 is PLL0 */
  HWIO_OUTM(GP_CLK_NS, HWIO_GP_CLK_NS_CLK_SRC_SEL_BMSK, 4 << HWIO_GP_CLK_NS_CLK_SRC_SEL_SHFT);

  /* configure M value */
  HWIO_OUTM(GP_CLK_MD, HWIO_GP_CLK_MD_M_VAL_BMSK, m << HWIO_GP_CLK_MD_M_VAL_SHFT);

  /* configure D value */
  HWIO_OUTM(GP_CLK_MD, HWIO_GP_CLK_MD_D_VAL_BMSK, not_2_d << HWIO_GP_CLK_MD_D_VAL_SHFT);

  /* configure N value */
  HWIO_OUTM(GP_CLK_NS, HWIO_GP_CLK_NS_N_VAL_BMSK, not_n_m << HWIO_GP_CLK_NS_N_VAL_SHFT);

  /* enable M/N counter */
  HWIO_OUTM(GP_CLK_NS, HWIO_GP_CLK_NS_MNCNTR_SEL_BMSK, 1 << HWIO_GP_CLK_NS_MNCNTR_SEL_SHFT);
/*lint -restore */
}

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
void camsensor_update_gp_clk_not_2_d
(
 uint16 not_2_d
)
{
  /* configure D value */
  /*lint -save -e737 HW macro */
  HWIO_OUTM(GP_CLK_MD, HWIO_GP_CLK_MD_D_VAL_BMSK, not_2_d << HWIO_GP_CLK_MD_D_VAL_SHFT);
  /*lint -restore */
}

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
void camsensor_unconfig_gp_clk(void)
{
  CAMERA_CONFIG_GPIO (GP_CLK_OFF);
}

#endif /* CAMERA_USES_XO4_CLK */
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
camera_ret_code_type camsensor_get_fps_list(int32 *p1, int32 *p2)
{
  if (camsensor_initialized)
  {
    /* May return depend on P1 value. For now, return same */
    if ((camera_preview_mode_type)(*p1) == CAMERA_PREVIEW_MODE_MOVIE)
    {
      *p1 = (int32)(&camsensor_video_fps_list[0]);
      *p2 = camsensor_video_fps_list_size;
    }
    else
    {
      *p1 = (int32)(&camsensor_snapshot_fps_list[0]);
      *p2 = camsensor_snapshot_fps_list_size;
    }
    return CAMERA_SUCCESS;
  }
  else
  {
    return CAMERA_NO_SENSOR;
  }
}

/*============================================================================
                        INTERNAL API DEFINITIONS
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
boolean camsensor_init (void)
{
	if (camsensor_initialized)
	{
		return TRUE;
	}
#ifndef FEATURE_CAMERA_MULTI_SENSOR
  	camsensor_id = CAMSENSOR_ID_MAX;
#endif /* FEATURE_CAMERA_MULTI_SENSOR */

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT

#ifdef FEATURE_CAMERA_MULTI_SENSOR
	if ( camera_asi == 0 )
	{
		if (camsensor_detect_table[camsensor_pair[current_camsensor_pair].camsensor_first])
		{
			camctrl_init_tbl();
			camsensor_init_func_tbl();
			camsensor_initialized = (*camsensor_detect_table[camsensor_pair[current_camsensor_pair].camsensor_first])(&camsensor_function_table,
	                                              &camctrl_tbl);
		}
	}
	else if ( camera_asi == 1 )
	{
		if (camsensor_detect_table[camsensor_pair[current_camsensor_pair].camsensor_second])
		{
			camctrl_init_tbl();
			camsensor_init_func_tbl();
			camsensor_initialized = (*camsensor_detect_table[camsensor_pair[current_camsensor_pair].camsensor_second])(&camsensor_function_table,&camctrl_tbl);
		}
	}
#else
    MSG_FATAL("current_camsensor_id================%d %d",current_camsensor_id,CAMSENSOR_ID_MAX,0);   
	if ( current_camsensor_id <  CAMSENSOR_ID_MAX )
	{
		if (camsensor_detect_table[current_camsensor_id])
		{
			camctrl_init_tbl();
			camsensor_init_func_tbl();
			camsensor_initialized = (*camsensor_detect_table[current_camsensor_id])(&camsensor_function_table, &camctrl_tbl);
		}
	}
#endif /* nFEATURE_CAMERA_MULTI_SENSOR */
#else
#ifdef FEATURE_CAMERA_SENSOR_AUTO_DETECT
#ifdef FEATURE_CAMERA_MULTI_SENSOR
	#ifdef FEATURE_NI_GPIO
  		for (camsensor_id = CAMSENSOR_NI_GPIO; camsensor_id < CAMSENSOR_ID_MAX; ++camsensor_id)
	#else /* FEATURE_NI_GPIO */
		/* camsensor_id starts with -1 instead of CAMSENSOR_OV2620 as before since
		 * customer may not want to include USE_CAMSENSOR_OV2620 in the build in which
		 * case this shoots an error. Care should be taken to see that camsensor_id starts
		 * with the first value of the enum "camsensor_sensor_model_type" */
  		for (camsensor_id = (camsensor_sensor_model_type) 0; camsensor_id < CAMSENSOR_ID_MAX; ++camsensor_id)
	#endif /* FEATURE_NI_GPIO */
#else
	#ifdef FEATURE_NI_GPIO
  		for (camsensor_id = CAMSENSOR_NI_GPIO; camsensor_id < CAMSENSOR_ID_MAX; ++camsensor_id)
	#else /* FEATURE_NI_GPIO */
		/* camsensor_id starts with -1 instead of CAMSENSOR_OV2620 as before since
		 * customer may not want to include USE_CAMSENSOR_OV2620 in the build in which
		 * case this shoots an error. Care should be taken to see that camsensor_id starts
		 * with the first value of the enum "camsensor_sensor_model_type" */
  		for (camsensor_id = (camsensor_sensor_model_type) 0; camsensor_id < CAMSENSOR_ID_MAX; ++camsensor_id)
	#endif /* FEATURE_NI_GPIO */
#endif /* FEATURE_CAMERA_MULTI_SENSOR */
	{
		MSG_FATAL("camsensor_id================%d",camsensor_id,0,0);
		if (camsensor_detect_table[camsensor_id])
		{
		  camctrl_init_tbl();
		  camsensor_init_func_tbl();
		  camsensor_initialized = (*camsensor_detect_table[camsensor_id])(&camsensor_function_table,
		                                                                  &camctrl_tbl);
		}

#ifdef FEATURE_CAMERA_MULTI_SENSOR
	    if (camsensor_initialized)
	    {
	      	break;
	    }
#else
		if (camsensor_initialized)
	    {
	      	break;
	    }
#endif /* nFEATURE_CAMERA_MULTI_SENSOR */
	}
#endif /* FEATURE_CAMERA_SENSOR_AUTO_DETECT */
#endif

	return camsensor_initialized;
} /* camsensor_init */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_START

DESCRIPTION
              Initializes the sensor configuration parameters. This is the
              Default function in the sensor function table


DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 -e818 parameters not referenced in dummy function */
static boolean camsensor_sensor_start ( camsensor_static_params_type * camsensor_params)
{
   return TRUE;
} /* camsensor_sensor_start */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_VIDEO_CONFIG

DESCRIPTION
              Obtains VFE configuration for preview. This is the Default
        function in the sensor function table.

DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 -e818 parameters not referenced in dummy function */
static boolean camsensor_sensor_video_config
(
  camsensor_static_params_type * static_params /* Static Parameters */
)
{
  return TRUE;
} /* camsensor_sensor_video_config */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_RAW_SNAPSHOT_CONFIG

DESCRIPTION
              Obtains VFE configuration for raw snapshot. This is the Default
        function in the sensor function table.

DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 -e818 parameters not referenced in dummy function */
static boolean camsensor_sensor_raw_snapshot_config
(
  camsensor_static_params_type *camsensor_params
)
{
  return TRUE;
} /* camsensor_sensor_raw_snapshot_config */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_SNAPSHOT_CONFIG

DESCRIPTION
              Obtains VFE configuration for snapshot. This is the Default
        function in the sensor function table.

DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 -e818 parameters not referenced in dummy function */
static boolean camsensor_sensor_snapshot_config
(
  camsensor_static_params_type *camsensor_params
)
{
  return TRUE;
} /* camsensor_sensor_snapshot_config */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_POWER_UP

DESCRIPTION
              Sensor power up. This is the Default
        function in the sensor function table.

DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_sensor_power_up (void)
{
} /* camsensor_sensor_power_up */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_POWER_DOWN

DESCRIPTION
              Sensor power Down. This is the Default
        function in the sensor function table.

DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_sensor_power_down (void)
{
} /* camsensor_sensor_power_down */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_SET_DEFAULT_FOCUS

DESCRIPTION
              Sets the default focus. This is the Default
        function in the sensor function table.


DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
static camera_ret_code_type    camsensor_sensor_set_default_focus (void)
{
   return CAMERA_SUCCESS;
} /* camsensor_sensor_set_default_focus */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_MOVE_FOCUS

DESCRIPTION
              Sensor move focus. This is the Default
        function in the sensor function table.

DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type    camsensor_sensor_move_focus (camsensor_move_focus_enum_type move, int32 steps)
{
   return CAMERA_SUCCESS;
} /* camsensor_sensor_move_focus */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_WRITE_EXPOSURE_GAIN

DESCRIPTION
              Write Sensor exposure. This is the Default
        function in the sensor function table.

DEPENDENCIES
  None

INPUT

RETURN VALUE
        TRUE
  FALSE

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type    camsensor_sensor_write_exposure_gain (uint16 gain, uint32 num_lines)
{
   return CAMERA_SUCCESS;
} /* camsensor_sensor_write_exposure_gain */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_REGISTER_TO_REAL_GAIN

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
/*lint -save -e715 parameters not referenced in dummy function */
static float    camsensor_sensor_register_to_real_gain (uint16 reg_gain)
{
   return 0.0;
} /* camsensor_sensor_register_to_real_gain */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_REAL_TO_REGISTER_GAIN

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
/*lint -save -e715 parameters not referenced in dummy function */
static uint16    camsensor_sensor_real_to_register_gain (float gain)
{
   return 0;
} /* camsensor_sensor_register_to_real_gain */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_SET_FRAME_RATE

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
static camera_ret_code_type    camsensor_sensor_set_frame_rate (uint16 fps)
{
   return CAMERA_SUCCESS;
} /* camsensor_sensor_set_frame_rate */
/*lint -restore */

/*===========================================================================

FUNCTION      CAMSENSOR_SENSOR_SET_POSSIBLE_FRAME_RATE

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
static camera_ret_code_type    camsensor_sensor_set_possible_frame_rate (uint16 min_fps, uint16 max_fps)
{
   return CAMERA_SUCCESS;
} /* camsensor_sensor_set_possible_frame_rate */
/*lint -restore */

/*===========================================================================

FUNCTION:
  CAMSENSOR_SENSOR_SET_BRIGHTNESS

DESCRIPTION:

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_brightness (int8 brightness)
{
  return CAMERA_SUCCESS;
} /* camsensor_sensor_set_brightness */
/*lint -restore */

/*===========================================================================

FUNCTION:
  CAMSENSOR_SENSOR_SET_CONTRAST

DESCRIPTION:

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_contrast (int8 contrast)
{
  return CAMERA_SUCCESS;
} /* camsensor_sensor_set_contrast */
/*lint -restore */

/*===========================================================================
FUNCTION:
  CAMSENSOR_SENSOR_SET_EFFECT

DESCRIPTION:

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_effect (int8 effect)
{
  return CAMERA_SUCCESS;
} /* camsensor_sensor_set_contrast */
/*lint -restore */
/*============================================================================

FUNCTION:
  CAMSENSOR_SENSOR_SET_WB

DESCRIPTION:

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_wb (int8 wb)
{
  return CAMERA_SUCCESS;
} /* camsensor_sensor_set_wb */
/*lint -restore */

/*===========================================================================


FUNCTION:
  CAMSENSOR_SENSOR_SET_EXPOSURE_MODE

DESCRIPTION:

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_exposure_mode (int8 mode)
{
  return CAMERA_SUCCESS;
} /* camsensor_sensor_set_contrast */
/*lint -restore */


/*===========================================================================


FUNCTION:
  CAMSENSOR_SENSOR_SET_EV_COMPENSATION

DESCRIPTION:

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_ev_compensation (int32 compensation)
{
  return CAMERA_SUCCESS;
} /* camsensor_sensor_set_ev_compensation */

/*==============================================================================

FUNCTION:
  CAMSENSOR_SENSOR_SET_ANTIBANDING

DESCRIPTION:

CALLERS:

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:

USAGE:

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_antibanding (int8 antibanding)
{
  return CAMERA_SUCCESS;
} /* camsensor_sensor_set_antibanding */
/*lint -restore */

/*============================================================================


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
camera_ret_code_type camsensor_set_effect (int8 effect)
{
  return (camsensor_function_table.camsensor_set_effect (effect));
} /* camsensor_set_effect */

/*===========================================================================

FUNCTION:
  CAMSENSOR_SET_EXPOSURE_MODE

DESCRIPTION:
  Changes the mode

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

===============================================================================*/


camera_ret_code_type camsensor_set_exposure_mode (int8 mode)
{
  return (camsensor_function_table.camsensor_set_exposure_mode (mode));
} /* camsensor_set_exposure_mode */


/*===========================================================================

FUNCTION:
  CAMSENSOR_SET_EV_COMPENSATION

DESCRIPTION:
  Changes the mode

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

===============================================================================*/


camera_ret_code_type camsensor_set_ev_compensation (int32 compensation)
{
  return (camsensor_function_table.camsensor_set_ev_compensation(compensation));
} /* camsensor_set_ev_compensation */


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

======================================================================================*/

camera_ret_code_type camsensor_set_wb(int8 wb)

{
  return (camsensor_function_table.camsensor_set_wb (wb));

}

/*======================================================================================

FUNCTION:
  CAMSENSOR_SET_ANTIBANDING

DESCRIPTION:
  Changes the Antibanding acording to the light

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

=========================================================================================*/


camera_ret_code_type camsensor_set_antibanding(int8 antibanding)

{
   return (camsensor_function_table.camsensor_set_antibanding(antibanding));

}

/*=========================================================================================



FUNCTION:
  CAMSENSOR_SENSOR_SET_ZOOM

DESCRIPTION:

CALLERS:
  camsensor_set_zoom ()

GLOBALS:

DEPENDENCIES:

SIDE EFFECTS:

RETURN VALUE:
  CAMERA_SUCCESS

USAGE:
  Default function handler when sensor's zoom control is used.

SCOPE:
  Local

============================================================================*/
/*lint -save -e715 -e818 parameters not referenced in dummy function */
static camera_ret_code_type camsensor_sensor_set_zoom (void *ptr)
{
  return CAMERA_SUCCESS;
} /* camsensor_set_zoom */
/*lint -restore */


/*===========================================================================

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

void camsensor_msm_auto_detect(void)
{
  hw_rev_type hardware_revision_number;
  static uint8 camsensor_msm_auto_detection_done = 0;

  if (camsensor_msm_auto_detection_done !=0 )
  {
    /* Auto detection is already done */
    /* Simply return from this function */
    return;
  }
  camsensor_msm_auto_detection_done = 1;
  
  camsensor_info.i2c_ce_invert = FALSE;

  hw_partnum_version(&hardware_revision_number);
  switch (hardware_revision_number.partnum)
  {
    case 0x0057:                /* 0x0057  Eagle130 no hdr */
      camsensor_info.msm_id = MSM6550_130;
      break;
    case 0x2057:                /* 0x2057  Eagle90 foundry 1 */
    case 0x4057:                /* 0x4057  Eagle90 foundry 2 */
    case 0x6057:                /* 0x6057  Eagle90 foundry 3 */
      camsensor_info.msm_id = MSM6550_90;
      break;

    case 0x60:
      camsensor_info.msm_id = MSM6275;
#if defined T_FFA && defined FEATURE_I2C_CHIP_ENABLE
#error code not present
#endif
      break;
    case 0xC0:
    case 0xC2:
      camsensor_info.msm_id = MSM6280;
#if defined T_FFA && defined FEATURE_I2C_CHIP_ENABLE
#error code not present
#endif
      break;
    case 0x01C0:
    case 0x01C1:
    case 0x01C2:
      camsensor_info.msm_id = MSM6290;
#if defined T_FFA && defined FEATURE_I2C_CHIP_ENABLE
#error code not present
#endif
      break;
    case 0xA334:
    case 0x0334:
      camsensor_info.msm_id = MSM6800_R2;
      break;
    case 0x2070: /* 6800-65nm */
    case 0x2078: /* 6801      */
    case 0x2074: /* 6575      */
    case 0x4070: /*6801A CHRT */
    case 0x4074: /*6571 CHRT */
    case 0x4078: /* 6171 CHRT */ 
    case 0x0070: 
    case 0x0078: 
    case 0x0074: 
      camsensor_info.msm_id = MSM6800B;
      break;
    /* 6245, 6255A and 6260 has the same SW interface */
    case 0xD0: /* 6260  */
    case 0xD2: /* 6255A */
    case 0xD4: /* 6245  */
    case 0xD6: /* 6235 */
      camsensor_info.msm_id = MSM6260;
#if defined T_FFA && defined FEATURE_I2C_CHIP_ENABLE
#error code not present
#endif
      break;
    case 0x0120: /* 6246  */
    case 0x0121: /* 6236  */
    case 0x0122: 
      camsensor_info.msm_id = MSM6246;
#if defined T_FFA && defined FEATURE_I2C_CHIP_ENABLE
#error code not present
#endif
      break;
    case 0xFF:
    case 0xFE:
    case 0xFA:
    case 0xF8:
      camsensor_info.msm_id = SC2X;
      break;
#ifdef FEATURE_CAMIF_INTERNAL_TEST
    case 0xF0:
      camsensor_info.msm_id = QSC6085;
      break;
#endif
    case 0x015E:
    case 0x015F:
    case 0x0150:
      camsensor_info.msm_id=QSC6270;
	  break;
    case 0x2C:
	  camsensor_info.msm_id=QSC1110;
	  break;
    default:
      camsensor_info.msm_id = MSM6800_R1;
      break;
  }
   camsensor_info.msm_version = hardware_revision_number.version;
	 
} /* camsensor_msm_auto_detect */


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
void camsensor_config_sensor_gpios_clks ()
{
#ifndef FEATURE_DSP
#ifndef T_QSC1110
  CAMERA_CONFIG_GPIO (GPIO_INPUT_51);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_52);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_42);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_43);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_44);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_45);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_46);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_47);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_48);
  CAMERA_CONFIG_GPIO (GPIO_INPUT_49);
  
  CAMERA_CONFIG_GPIO (CAMIF_PCLK_DISABLE);
#ifdef CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
  CAMERA_CONFIG_GPIO(CAMIF_FLASH);
#endif /* CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH */
#else
  CAMERA_CONFIG_GPIO (CAMIF_HSYNC_OUT);
  CAMERA_CONFIG_GPIO (CAMIF_VSYNC_OUT);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_0);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_1);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_2);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_3);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_4);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_5);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_6);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_7);
#ifndef CAMERA_8_BIT_INTERFACE
  CAMERA_CONFIG_GPIO (CAMIF_DATA_8);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_9);
#endif /* CAMERA_8_BIT_INTERFACE */
#ifdef T_FFA
#error code not present
#endif /* T_FFA */

  CAMERA_CONFIG_GPIO (CAMIF_PCLK);
#ifdef CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
  CAMERA_CONFIG_GPIO(CAMIF_FLASH);
#endif /* CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH */

  /* select parallel camera. Default is parallel camera. Cannot touch this
   * register without proper clock being already present.
   * To initialize MDDI_CAMIF_CFG, the recommendation is selecting HCLK
   * just for the duration of configuration. */

  /* HWIO_OUT (MDDI_CAMIF_CFG, 0x2); */

  /* camclk_po -> camera sensor -> camif_pclk -> MDDI client clock regime */
  /*
     18 :17 MDDI_CLIENT_VFE_CLK_SRC_SEL
     00 : hclk
     01 : camif_pclk
     10 : camclk_po
     11 : undefined_encoding

    FOR SC2X

     18 :17 MDDI_CLIENT_VFE_CLK_SRC_SEL
     00 : hclk
     01 : camclk_po
     10 : camif_pclk
     11 : undefined_encoding


  */
#ifdef FEATURE_MDDI_CAMERA
  if (camsensor_connection_mode == PARALLEL_CONNECTION)
#endif 
  {

    CAMERA_CLK_REGIME_SEL_CLK_SRC (CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE, CLK_RGM_SRC_SEL_MDDI_CLIENT_VFE_SRC_CAMIF_V);
  #ifdef CAMERA_USES_CAMIF_PAD
    CAMERA_CLK_REGIME_MSM_RESET(CLK_RGM_CAMIF_PCLK_M);
    CAMERA_CLK_REGIME_SEL_CLK_SRC (CLK_RGM_SRC_SEL_CAMIF_PCLK, CLK_RGM_SRC_SEL_CAMIF_PCLK_SRC_TLMM_CAM_PCLK);
  #endif /* CAMERA_USES_CAMIF_PAD */
  }

  CAMERA_CLK_REGIME_ENABLE      (CLK_RGM_MDDI_CLIENT_VFE_M);
  CAMERA_CLK_REGIME_ENABLE      (CLK_RGM_VIDEO_M);
  CAMERA_CLK_REGIME_ENABLE      (CLK_RGM_VIDEO_VFE_M);
  CAMERA_CLK_REGIME_ENABLE      (CLK_RGM_CAMCLK_M);
#endif //#if defined(T_QSC1100)
#endif //#ifndef FEATURE_DSP
} /* camera_svcs_config_sensor_gpios_clks */

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
void camsensor_unconfig_sensor_gpios_clks ()
{
#ifndef FEATURE_DSP
  CAMERA_CONFIG_GPIO (CAMIF_HSYNC_OUT_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_VSYNC_OUT_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_0_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_1_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_2_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_3_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_4_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_5_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_6_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_7_DISABLE);
#ifdef T_QSC1110
 #ifndef CAMERA_8_BIT_INTERFACE
  CAMERA_CONFIG_GPIO (CAMIF_DATA_8_DISABLE);
  CAMERA_CONFIG_GPIO (CAMIF_DATA_9_DISABLE);
 #endif /* CAMERA_8_BIT_INTERFACE */

  CAMERA_CONFIG_GPIO (CAMIF_PCLK_DISABLE);

  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_MDDI_CLIENT_VFE_M);
  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_VIDEO_M);
  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_VIDEO_VFE_M);
  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_CAMCLK_M);
  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_MDDI_CLIENT_VFE_M);
 #ifdef FEATURE_MDDI_CAMERA
  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_MDDI_CLIENT_IO_M);
  CAMERA_CLK_REGIME_DISABLE (CLK_RGM_MDDI_CLIENT_DIV4_M);
 #endif
 
#endif
#endif //#ifndef FEATURE_DSP
} /* camsensor_unconfig_sensor_gpios_clks */


/*===========================================================================
FUNCTION      CAMSENSOR_CREATE_FPS_LIST

DESCRIPTION
              Create FPS list.

DEPENDENCIES
              None

INPUT
              None

RETURN VALUE
              None

SIDE EFFECTS
              None

============================================================================*/

static void camsensor_create_fps_list
(
  const camsensor_static_params_type * camsensor_params /* Static Parameters */
)
{
  int32 i;
  uint32 fps;
  uint32 temp;

  camsensor_snapshot_fps_list_size = 0;
  camsensor_video_fps_list_size = 0;
  for (i = 0; i < camsensor_params->num_possible_frame_rates; i++)
  {
    temp = camsensor_params->frame_rate_array[i].fps;
    /* Terminate calculate when frame rate is zero. */
    if (temp == 0) break;
    if (temp & 0xff)
    {
      temp *= 2;
      if (temp & 0xff)
      {
        temp *= 2;
        if (temp & 0xff)
        {
          temp *= 2;
          if (temp & 0xff)
          {
            temp *= 2;
            fps = 0x100000;
          }
          else
          {
            fps = 0x80000;
          }
        }
        else
        {
          fps = 0x40000;
        }
      }
      else
      {
        fps = 0x20000;
      }
    }
    else
    {
      fps = 0x0;
    }
    fps += temp >> 8;
    /* For snapshot mode, only list ones used in AFR.
     * For video mode, list all. */
    if (camsensor_params->frame_rate_array[i].use_in_auto_frame_rate)
    {
      camsensor_snapshot_fps_list[camsensor_snapshot_fps_list_size++] = (int32)fps;
    }
    camsensor_video_fps_list[camsensor_video_fps_list_size++] = (int32)fps;
  }
  /* Make sure null terminated */
  camsensor_snapshot_fps_list[camsensor_snapshot_fps_list_size] = 0;
  camsensor_video_fps_list[camsensor_video_fps_list_size] = 0;
}

/* End of Internal API.
 * New global functions should go into EXTERNAL API section.
 */


/* ===================================================================================

FUNCTION      CAMSENSOR_INIT_FUNC_TBL

DESCRIPTION
              Initializes the camsensor Function Table with Dummy Function Pointers.

DEPENDENCIES
              None

INPUT
              None

RETURN VALUE
              None

SIDE EFFECTS
              None


========================================================================================*/

static void camsensor_init_func_tbl()

{
    /* Initialize with dummy function pointers */
  camsensor_function_table.camsensor_start                  = camsensor_sensor_start;
  camsensor_function_table.camsensor_video_config           = camsensor_sensor_video_config;
  camsensor_function_table.camsensor_raw_snapshot_config    = camsensor_sensor_raw_snapshot_config;
  camsensor_function_table.camsensor_snapshot_config        = camsensor_sensor_snapshot_config;
  camsensor_function_table.camsensor_power_up               = camsensor_sensor_power_up;
  camsensor_function_table.camsensor_power_down             = camsensor_sensor_power_down;
  camsensor_function_table.camsensor_write_exposure_gain    = camsensor_sensor_write_exposure_gain;
  camsensor_function_table.camsensor_set_default_focus      = camsensor_sensor_set_default_focus;
  camsensor_function_table.camsensor_move_focus             = camsensor_sensor_move_focus;
  camsensor_function_table.camsensor_register_to_real_gain  = camsensor_sensor_register_to_real_gain;
  camsensor_function_table.camsensor_real_to_register_gain  = camsensor_sensor_real_to_register_gain;
  camsensor_function_table.camsensor_set_frame_rate         = camsensor_sensor_set_frame_rate;

  /* The below function are applicable for YCbCr sensors */
  camsensor_function_table.camsensor_set_brightness         = camsensor_sensor_set_brightness;
  camsensor_function_table.camsensor_set_contrast           = camsensor_sensor_set_contrast;
  camsensor_function_table.camsensor_set_zoom               = camsensor_sensor_set_zoom;
  camsensor_function_table.camsensor_set_effect             = camsensor_sensor_set_effect;
  camsensor_function_table.camsensor_set_exposure_mode      = camsensor_sensor_set_exposure_mode;
  camsensor_function_table.camsensor_set_wb                 = camsensor_sensor_set_wb;
  camsensor_function_table.camsensor_set_antibanding        = camsensor_sensor_set_antibanding;
  camsensor_function_table.camsensor_set_ev_compensation    = camsensor_sensor_set_ev_compensation; 

  camsensor_function_table.camsensor_set_possible_frame_rate = camsensor_sensor_set_possible_frame_rate;
} /*camsensor_init_func_tbl*/


#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================

FUNCTION      camsensor_cammddi_inout_params_setup

DESCRIPTION
          Initiate appropriate CAMMDDI input parameters. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_cammddi_inout_params_setup(camsensor_static_params_type * camsensor_params)
{
  cammddi_inout_params.in.active_width                = camsensor_dynamic_params.active_width;
  cammddi_inout_params.in.hblank                      = camsensor_dynamic_params.hblank;
  cammddi_inout_params.in.mclk                        = camsensor_dynamic_params.mclk;
  cammddi_inout_params.in.bpp                         = camsensor_dynamic_params.bpp;
  cammddi_inout_params.in.i2c_freq                    = camsensor_dynamic_params.i2c_freq;
  cammddi_inout_params.in.three_wire_freq             = camsensor_dynamic_params.three_wire_freq;
  cammddi_inout_params.in.mddi_data_rate              = camsensor_dynamic_params.mddi_data_rate;
  cammddi_inout_params.in.mddi_config_bps             = camsensor_dynamic_params.mddi_config_bps;
  cammddi_inout_params.in.mddi_config_rev_encap_size  = camsensor_dynamic_params.mddi_config_rev_encap_size;
  cammddi_inout_params.in.mddi_capture_bps            = camsensor_dynamic_params.mddi_capture_bps;
  cammddi_inout_params.in.mddi_capture_fifo_threshold = camsensor_dynamic_params.mddi_capture_fifo_threshold;
  cammddi_inout_params.in.mddi_capture_rev_encap_size = camsensor_dynamic_params.mddi_capture_rev_encap_size;

  /* when static params are not needed to pass to cammddi, null is assigned */
  if (camsensor_params != NULL)
  {
    cammddi_inout_params.in.camif_config                = camsensor_params->camif_config;
    cammddi_inout_params.in.camif_efs_config            = camsensor_params->camif_efs_config;
    cammddi_inout_params.in.camif_frame_config          = camsensor_params->camif_frame_config;
    cammddi_inout_params.in.camif_window_width_config   = camsensor_params->camif_window_width_config;
    cammddi_inout_params.in.camif_window_height_config  = camsensor_params->camif_window_height_config;
    cammddi_inout_params.in.camif_subsample1_config     = camsensor_params->camif_subsample1_config;
    cammddi_inout_params.in.camif_subsample2_config     = camsensor_params->camif_subsample2_config;
  }
} /* camsensor_cammddi_inout_params_setup */
#endif /* FEATURE_MDDI_CAMERA */

#ifdef FEATURE_MDDI_CAMERA
/*===========================================================================

FUNCTION      CAMSENSOR_CAMMDDI_INOUT_PARAMS_UPDATE

DESCRIPTION
          Update static parameters from cammdi_out_params. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void camsensor_cammddi_inout_params_update(camsensor_static_params_type * camsensor_params)
{
  camsensor_params->camif_config               = cammddi_inout_params.out.camif_config;
  camsensor_params->camif_frame_config         = cammddi_inout_params.out.camif_frame_config;
  camsensor_params->camif_window_width_config  = cammddi_inout_params.out.camif_window_width_config;
  camsensor_params->camif_window_height_config = cammddi_inout_params.out.camif_window_height_config;
  camsensor_params->camif_subsample1_config    = cammddi_inout_params.out.camif_subsample1_config;
} /* camsensor_cammddi_inout_params_update */

#endif /* FEATURE_MDDI_CAMERA */

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
boolean camsensor_gpio_in(byte gpio_signal, byte * value_ptr)
{
  if (!value_ptr)
  {
     MSG_ERROR("cammddi_gpio_in: failed due to null pointer",0,0,0); 
     return FALSE;
  }

#ifdef FEATURE_MDDI_CAMERA
  if (camsensor_connection_mode == MDDI_CONNECTION)
  {
    if (!cammddi_gpio_in((byte)gpio_signal, value_ptr))
    {
      MSG_ERROR("cammddi_gpio_in: failed",0,0,0);
      return FALSE;
    }
  }
  
#endif /* FEATURE_MDDI_CAMERA */
  return TRUE;
  
} /* camsensor_gpio_in */

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
boolean camsensor_gpio_out(byte gpio_signal, byte gpio_value)
{
#ifdef FEATURE_MDDI_CAMERA
  if (camsensor_connection_mode == MDDI_CONNECTION)
  {
    return cammddi_gpio_out((byte)gpio_signal, (byte)gpio_value);
  }
  
#endif /* FEATURE_MDDI_CAMERA */
  return TRUE;
} /* camsensor_gpio_out */

#endif /* FEATURE_CAMERA */
