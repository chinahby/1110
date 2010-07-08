/*===========================================================================

            " C U S T C A M E R A "   H E A D E R   F I L E

DESCRIPTION
  Configuration for CAMERA Features on Enhanced Multimedia Targets.

  Copyright (c) 2004 - 2006 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/build/cust/qsc1110/main/latest/inc/custcamera.h#5 $ $DateTime: 2009/04/27 05:11:36 $ $Author: c_caluru $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
27/04/09   chai    Fixed the issue with MWB flash
25/11/08   chai    defined FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND
04/07/08  KC       Moved FEATURE_OVERLAY and FEATURE_ICAMERA from OEMFeatures.h
11/22/07   nshree  Added the Support for Chromatix 0x201
06/24/07   arv     Enabled the Standalone downsizer default for SC2x Low tier
                   and incall downsizer for high tier
03/28/07   ven     Defined the feature CAMERA_PREVIEW_ROTATION90
03/14/07   ven     Enabled the feature CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
01/25/07   ven     Undefined FEATURE_CAMERA_LCD_DIRECT_MODE if MDP is not defined.
07/05/06   jv      Readded FEATURE_CAMERA_LVCM for now to prevent compile errors
07/05/06   dle     Removed FEATURE_CAMERA_LVCM.
                   Added FEATURE_CAMERA_SVCS_DIAG, and
                   FEATURE_CAMERA_VFE_16X16_SUPPORT.
06/15/06   jv      Added FEATURE_IYCBCR
06/12/06   jv      Added FEATURE_CAMERA_VFE_4X4_SUPPORT
04/03/06   sch     Added CAMERA_VFE_SLOPE_SHIFT_FACTOR 
03/06/06   mvd     Added FEATURE_CAMERA_V770
02/27/06   jj      Added FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H as requested.
01/02/06   dle     Added FEATURE_CAMERA_10BIT_LUT and FEATURE_CAMERA_DRV_TASK.
11/07/05   dl      Defined LPM only if we are using 5x5 ASF or 
                   Luma Vignette Correction.
08/03/05   mvd     Commented out:
                     FEATURE_CAMERA_EXPOSURE_MODE, FEATURE_STROBE_FLASH,
                     FEATURE_CAMERA_MULTI_SENSORS
                   Added  
                     FEATURE_CAMERA_LVCM
                   Deleted: FEATURE_CAMERA_LOW_LIGHT_COLOR_CORRECTION,
                     FEATURE_VOC_ADPCM
06/16/05   mvd     Updates.
05/05/05   pdo     Initial revision.

===========================================================================*/

/*---------------------------------------------------------------------------
                            CAMERA Services
---------------------------------------------------------------------------*/

/* This Qcamera BREW Application is defined in msm6550.builds file */
#ifdef FEATURE_APP_QCAMERA

/* Additional features for the QCamera BREW Application, originally in msm6550.builds file */
#define FEATURE_APP_QALBUM
//#define FEATURE_APP_TEST_AUTOMATION 
//#define FEATURE_QCAMERA_TEST_AUTOMATION  - couldn't find this one

/* May revert to this instead of EV */
//#define FEATURE_CAMERA_BRIGHTNESS

#endif /* FEATURE_APP_QCAMERA */

/* Originally in both msm6550.builds & BREWSimfeatures.h */
#define FEATURE_CAMERA

// Include Overlay feature
#if defined(FEATURE_MDP)
#define FEATURE_OVERLAY
#endif
/* The following features are sub-features in Camera */
#ifdef FEATURE_CAMERA

#define FEATURE_ICAMERA    /* ICAMERA Extension */

/* originally in msm6550.builds file */
#ifndef FEATURE_I2C_CHIP_ENABLE
#define FEATURE_I2C_CHIP_ENABLE      
#endif

/* Camera Generation 7; enhanced Multimedia targets, originally in msm6550.builds file */
#define FEATURE_CAMERA_V7
/* For 6550 3.x branch only or for PRE-BREW3.1.4 */
//#define FEATURE_CAMERA_EXPOSURE_MODE
#define FEATURE_CAMERA_ENCODE_PROPERTIES

//#define FEATURE_STROBE_FLASH
/* Originally in camera_svcs.c */
#define FEATURE_WHITE_LED_FLASH          /* Locally define in camera file */

#if defined FEATURE_WHITE_LED_FLASH || defined FEATURE_STROBE_FLASH
#define ANY_FLASH
#endif /* FEATURE_WHITE_LED_FLASH || FEATURE_STROBE_FLASH */

/* 5x5 ASF */
//#define FEATURE_CAMERA_5x5_ASF

/* Bayer Vignette Correction */
//#define FEATURE_CAMERA_BVCM

/* 10 bit (1024 entries of 8 bit value field) gamma look up table */
#define FEATURE_CAMERA_10BIT_LUT

/* Enable Luma Processing Module if either LVCM or 5x5 ASF is defined */ 
#if defined (FEATURE_CAMERA_5x5_ASF)
#define FEATURE_CAMERA_LPM
#else
#undef FEATURE_CAMERA_LPM
#endif

/*  Chromatix/PC-MobiCaT requires FEATURE_CAMERA_SVCS_DIAG. */
/*  FEATURE_CAMERA_MOBICAT enables runtime tuning, and      */
/*  writing settings to JPEG headers.                       */
/*  FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H enables setting  */
/*  compile-time default values which are used to init      */
/*  variables at camera services startup time (run-time),   */
/*  which is done in the camera_config.h file in            */
/*  .../services/camera/mobicat/camera_config.h .           */

#define FEATURE_CAMERA_SVCS_DIAG   



/* Camera 3A driver */
#define FEATURE_CAMERA_DRV_TASK

#define FEATURE_CAMERA_VFE_4X4_SUPPORT
/* 16X16 feature is disabled. It takes 9ms instead of 3.5ms compare to 8x8 */
//#define FEATURE_CAMERA_VFE_16X16_SUPPORT

#define FEATURE_CAMERA_V770
/* This enables CAMERA_PARM_FPS_LIST in the OEM and application */
#define FEATURE_CAMERA_V790

#define FEATURE_IYCBCR

/* The clk_regime_sel_camclk_ext returns the configured freq */
/* TODO Need to look into this */

/* 6260, 6255A, 6245, SC2X does not have MDDI */
#define CAMERA_NO_MDDI

/* Enable Best Shot Feature */
//#define FEATURE_CAMERA_BESTSHOT_MODE

/* Enabling UI support for Hand Jitter */
//#define FEATURE_CAMERA_HJR

/*Added support for DCVS for SC2X*/
//#define FEATURE_CAMERA_REGISTER_CLOCK_RESOURCE_III

//#define FEATURE_CAMERA_LCD_DIRECT_MODE
/*MDP Related */
#ifdef FEATURE_CAMERA_LCD_DIRECT_MODE
  #ifndef FEATURE_MDP
    #undef FEATURE_CAMERA_LCD_DIRECT_MODE
  #endif /* FEATURE_MDP */
#endif /* FEATURE_CAMERA_LCD_DIRECT_MODE */
//#define FEATURE_CAMERA_YCBCR_ONLY

#ifndef CUST_EDITION
#define FEATURE_CAMERA_MOBICAT
#define FEATURE_CAMERA_MOBICAT_CAMERA_CONFIG_H
#endif

#ifdef FEATURE_CAMERA_YCBCR_ONLY
#undef FEATURE_CAMERA_LPM
#undef FEATURE_CAMERA_BVCM
#undef FEATURE_CAMERA_5x5_ASF 
#undef FEATURE_CAMERA_BESTSHOT_MODE
#undef FEATURE_CAMERA_HJR
#endif

/* Use LIBSTD for strcpy and strncpy */
#define CAMERA_USES_LIBSTD
/* SC2X related */
//#define CAMERA_USE_PMIC_TO_POWER_SENSOR
//#define FEATURE_CAMERA_VFE_SNAPSHOT_YCBCR420
//#define CAMERA_USES_CAMIF_PAD
//#define CAMERA_THUMBNAIL_SIZE_QCIF
#define FEATURE_CAMERA_USE_STANDALONE_DOWNSIZER
#define FEATURE_CAMIF_INTERNAL_TEST
#define FEATURE_CAMERA_ENCODE_YCBCR
//#define CAMERA_USE_GPIO_TLMM_FOR_CAMIF_FLASH
#define CAMERA_PREVIEW_ROTATION90
#define CAMERA_USES_CHROMATIX_0x201
#define CAMERA_8_BIT_INTERFACE
#define CAMERA_USES_CAMCLK_MN
#define CAMERA_USES_XO4_CLK

#define FEATURE_CAMERA_CONCURRENT_SHUTTER_SOUND

#endif /* FEATURE_CAMERA */




