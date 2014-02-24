#ifndef MSMAUD_H
#define MSMAUD_H
/*===========================================================================

                          Audio Hardware Support

DESCRIPTION
  This file contains definitions that specify Audio H/W features and interfaces.

EXTERNALIZED FUNCTIONS
  MSMAUD_INIT_CODEC
    Macro to initialize the CODEC_CTL register.
  MSMAUD_RESET_ONES_DETECT
    Macro to reset the one's detector
  MSMAUD_READ_ONES_DETECT
    Macro to read the one's detector
  MSMAUD_RINGER_STOP
    Disable the Ringer M/N counters
  MSMAUD_RINGER_MN_A_M
    Set the 16-bit M value and enable the A M/N counter
  MSMAUD_RINGER_MN_A_N
    Set the 16-bit N value
  MSMAUD_RINGER_MN_A_DUTY
    Set the 16-bit duty cycle
  MSMAUD_RINGER_MN_B_M
    Set the 16-bit M value and enable the B M/N counter
  MSMAUD_RINGER_MN_B_N
    Set the 16-bit N value
  MSMAUD_RINGER_MN_B_DUTY
    Set the 16-bit duty cycle

Copyright(c) 2004 - 2009 by QUALCOMM, Incorporated.
All Rights Reserved. Qualcomm Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/drivers/QSC11X0/main/latest/inc/msmaud.h#7 $ $DateTime: 2011/01/05 16:15:51 $ $Author: kmodak $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/05/10   knm     Removed DC offset from Line Out by diabling Rx Filter for 
                   Speaker. 
04/21/10   knm     Fixed pop noise issue with FM recording by disconnecting
                   Rx filter before configuring iclock.
03/30/10   knm     Added dummy device for FM on handset or LineOut
06/01/09   nr      AMR NB disbaled for ULC Low Tier builds.
05/23/09   sud     Removed MSMAUD_QSC6270_FFA_AUDIO_CAL and
                   MSMAUD_QSC6270_SURF_AUDIO_CAL,& add MSMAUD_QSC1110_AUDIO_CAL
                   Updated Copyright.
05/05/09   knm     Changed adie configuration sequence to reduce the pop.
03/13/09   skr     Updating MIPS table for DCS feature for Mp3 and AAC playback
03/12/09   knm     Changed the deconfig macro for FM.
02/17/09   knm     Changed the DSP MIPS values of AAC for different clock rates
                   and then renamed the macros for QTUNES and AAC.
02/16/09   sj      Fixed CR 172214. FM recording with stereo headset was not 
                   working.
02/16/09   knm     Changed the adie sequence of all devices to reduce the pop
                   noise level.
01/06/09   knm     Modified Adie Power down sequence to optimize the power.
12/31/08   knm     Temporarily disabling MSMAUD_ADIE_PA_POWER_DOWN as currently
                   it has some side effects. 
12/22/08   knm     Added macros for AAC Dynamic clock switching.
11/24/08   knm     Defined MSMAUD_ADIE_PA_POWER_DOWN macro.
11/24/08   knm     Modified MSMAUD_ADSP_CLK_GATING_ENA macro to resolve compilation
                   issue.
11/21/08   knm     Defined MSMAUD_ADSP_CLK_GATING to save power.
11/18/08   knm     Added macros to configure Tx path for FM Recording &
                   FM over USB.
10/22/08   dp      FM over BT AG changes
09/24/08   vsud    Added feature guard for Ext Ringer.
09/17/08   knm     Added support for Aux Line In(FM) volume control.
09/05/08   sj      FM recording support. Added the adie configuration to
                   take tx input from LINE_IN if FM is on
06/20/08   knm     Added support for AuxPGA Line In Path Configuration.
10/06/08   vsud    Added Macro for reset and set DWA bit(noise issue).
					Applicable only for ULC.  
02/07/08   vs      Remove chip-bringup-feature and rfms-reset-feature.
02/05/08   vs      Updated adie power up sequeuce for audio devices
                   relating to headphone in capless mode.
01/28/08   vs      Added a pound-define to compile in audio calibration for
                   surf build.
01/23/08   vs      Turn off adsp macro clock to save power when adsp becomes idle.
01/21/08   vs      Added pound-define for audio cal data for Rx/Tx path in FFA.
01/15/08   vs      Removed MSMAUD_LCU_CHIP_BRINGUP pound-define to compile out
                   LCU bring up code.
12/07/07   vss     Undefined MSMAUD_CODEC_RFMS_RESET.
12/03/07   vss     Added DCS for mp3 and aac+ enhanced playback 
                   to fix compile error in CRM build for LCU.
11/30/07   vss     Verified codec settings in AMSS, all matched with table from
                   codec hardware team.
11/29/07   vss     Updated with codec settings from Deepak (Quoqing's team) 
                   for all audio paths.
11/21/07   vss     Added defines to featurize code to bringup LCU and reset 
                   RFMS when audio clock is halted/unhalted for LCU.
10/11/07   vss     Initial update for LCU.
09/26/07   anb     Featurized the grnd ref capless mode under
                   FEATURE_AUDIO_HEADSET_CAPLESS_MODE. 
08/28/09   wfd     MSMAUD_AUX_PCM_SDAC_SHARED and MSMAUD_MSM6050_CODEC_CONFIG
                   defines removed. THese are no longer used in voccal.c.
08/28/07   ymc     Updated codec configuration sequences and definitions.
           ymc     Updated Copyright.
07/28/07   anb     Modified the values of speaker,USB configurations
07/07/07   anb     Updated references to PA bias reg3/4 of Dragon-4 addresses mp3-power issue
06/21/07   anb     Removed reference to ADIE_GPR_MS1_REGISTER_R from Dragon 3/4 codec defs.
05/30/07   anb     Defined Dragon-4 related codec defs.
02/28/07    sp     Define MSMAUD_QSC60X5_AUDIO_HW.
09/11/06   act     Modified wideband configs to use 42MHz OTA BW instead of
                   80MHz for power savings.
08/07/06   act     Adding 4GV VOIP macro define.
06/01/06    sg     Defined MSMAUD_VOC_1X_COMMON for 1X vocoder support.
03/15/06   ymc     Added FEATURE_INTERNAL_SADC support.
03/01/06    ay     Modified the SDAC en/disable macros to use the clock regime.
02/13/06    ay     Modified all HPH_L and HPH_R devices to enable the
                   reference buffer for the headphone power amplifier.
11/06/05    sg     Modified to support dual mode clk rgm apis.
08/08/05    sm     Added Stereo USB support.
07/19/05    sg     Synced up to //depot/asic/msm6550/services/adsp/msmaud.h#12.
09/23/04    sg     Defined MSMAUD_VOC_IS127 for EVRC support.
04/28/04    sg     Initial version for MSM6550 derived from the 6500 archive.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"    /* Definitions for common variable types  */
#include "msm.h"       /* MSM Register definitions and utilities */
#include "adie.h"      /* Solo codec register defs               */


/* <EJECT> */
/*===========================================================================

                  H / W   A N D   D S P   F E A T U R E S

===========================================================================*/

/* Defines ADIE Register values for HPH_L/HPH_R in Capless mode */

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
#define MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE  \
        ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V
#else
#define MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE  \
        ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V
#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

/* Rx biquad filter reg address */
#define MSMAUD_ADIE_RX_BIQUAD_REG   ADIE_RX_PA_CONTROL_REGISTER1_R

/* MSM6100 DMA Path controls
*/
#define MSMAUD_6100_UP_DMA_PATH_CONTROL_VALUES

/* The uP sets the DMA path
*/
#define MSMAUD_UP_DMA_PATH_CONTROL

/* The uP sets the DMA path for VR
*/
#define MSMAUD_UP_DMA_PATH_CONTROL_VR

/* 3 Bits control PCM format instead of 2
*/
#define MSMAUD_PCM_FORMAT_3BIT

/* This target shares pins between SDAC and AUX PCM
*/
#define MSMAUD_AUX_PCM_SDAC_SHARED

/* Microphone reset bit available in Codec function control register
*/
#define MSMAUD_MIC_RESET_CTRL

/* The vocoder DSP image does not support a SW frame reference command
*/
#undef MSMAUD_SUPPORTS_FRAME_REF_CMD

/* The internal codec is on an analog die
*/
#define MSMAUD_ADIE_CODEC

/* Control for internal codec clocks in vocoder driver
*/
#define MSMAUD_CODEC_CLOCK_CTRL

/* TTY capability is integrated into the vocoder image
*/
#ifdef FEATURE_TTY
  #define MSMAUD_INTEGRATED_TTY
#endif

/* PCM Control for MSM6050
*/
#define MSMAUD_MSM6050_PCM_CTRL

/* The Audio hardware, expecially the config. registers for AUX_PCM, are 
** changed for all QSC60x5 chips. This feature has to be defined
** on all targets that has this new HW. 
*/
#define MSMAUD_QSC60X5_AUDIO_HW

/* PMIC MIC-BIAS Configuration */
#define MSMAUD_PMIC_CONFIG

#ifdef FEATURE_EXTERNAL_SDAC
/* On chip Stereo DAC support
*/
#define MSMAUD_ON_CHIP_SDAC_SUPPORT

/* Register is now one's compliment of 2*D
*/
#define MSMAUD_NEW_SDAC_DUTY_CYCLE_REG
#endif /* FEATURE_EXTERNAL_SDAC */

/* Support for different ADEC modes on QSynth image */
#define MSMAUD_QSYNTH_ADEC_MODE_SUPPORT

#ifdef FEATURE_WEBAUDIO
/* Support for hybrid mode on QSynth image */
#define MSMAUD_QSYNTH_HYBRID_MODE_SUPPORT
#endif /* FEATURE_WEBAUIDO */


#ifdef FEATURE_AMR_VOCODER
/* Support for AMR vocoder */
#define MSMAUD_VOC_AMR
#define MSMAUD_VOC_AMR_COMMON
#endif /* FEATURE_AMR_VOCODER */

#ifdef FEATURE_MP4_AMR
/* Support for MP4 AMR record and playback */
#define MSMAUD_VOC_AMR_COMMON
#endif /* FEATURE_MP4_AMR */

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */

/* EVRC support */
#define MSMAUD_VOC_IS127

/* 13k support */
#define MSMAUD_VOC_IS733

/* G.711 support */
#ifdef FEATURE_VOC_G711
  #define MSMAUD_VOC_G711
#endif /* FEATURE_VOC_G711 */

/* 4GV NB support */
#ifdef FEATURE_VOC_4GV
  #define MSMAUD_VOC_4GV
#endif /* FEATURE_VOC_4GV */

#if defined(FEATURE_VOICE_RECORD) || defined(FEATURE_VOICE_PLAYBACK)
/* Support for 1X record and playback
*/
#define MSMAUD_VOC_1X_COMMON
#endif /* FEATURE_VOICE_RECORD || FEATURE_VOICE_PLAYBACK */

#define MSMAUD_QSC1110_AUDIO_CAL

#if defined(T_FFA6280_EUROPA)
  #define MSMAUD_6280_FFA_MFLO_AUDIO_CAL
#endif /* T_FFA6280_EUROPA */

/* This is defined so that we can compile in code that does ssbi write/read
** test. This is so that we can, at run time, verify that ssbi is functional.
** Since this test uses data members of a data structure shared by audio 
** agc test, the audio agc test is disabled if this test is defined.
** Vandy: 11/20/2007
*/
#define MSMAUD_ADIE_SSBI_IO_TEST 

/*
** This is here temporarily until clock regim driver defines it. 
** When clock driver code supports it, this define should be removed.
** Vandy - 10/08/07
*/
/* #define CLK_RGM_SDAC_M  CLK_RGM_I2S_M */

/* This is defined so that the adsp macro clock will be turned off 
** when the adsp issues a wait_until instruction. The wait_until
** instruction is issued only when the adsp becomes idle. This is
** to save additional power when adsp is idle.  To force the macro
** to always be turned on, compile out this pound-define.
*/
#define MSMAUD_ADSP_CLK_GATING


/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Helper macro to create a bit mask.  The value is placed within the
** bit mask.
*/
#define GEN_MASK(val, mask) (val << SHIFT_FROM_MASK(mask))

/*
**  Audio Hardware Support for the MSM6050
*/

/* The bit mask used to generate a vocoder frame reference interrupt
*/
#define MSMAUD_FRAME_REF_INT_BIT 0x00000010

/* <EJECT> */
/*===========================================================================

                C O D E C   C O N T R O L   R E G I S T E R

===========================================================================*/
/* Bit mask values for the CODEC_CTL register
*/
#define CODEC_CTL_ADSP_CODEC_CTL_ARM_V  0
#define CODEC_CTL_ADSP_CODEC_CTL_DSP_V  HWIO_FMSK(AUX_CODEC_CTL,ADSP_CODEC_CTL_EN)
#define CODEC_CTL_ONES_DET_1_V          HWIO_FMSK(AUX_CODEC_CTL,ONES_POLARITY)
#define CODEC_CTL_ONES_DET_0_V          0

/* Mask of bits to write when initializing the CODEC_CTL register
*/
#define CODEC_CTL_INIT_MASK ( HWIO_FMSK(AUX_CODEC_CTL,ADSP_CODEC_CTL_EN) | \
                              HWIO_FMSK(AUX_CODEC_CTL,ONES_POLARITY) )

/* Give control of this register to the DSP and set the ONE's detect value
** which is always controlled by the uP.
*/
#define CODEC_CTL_INIT_VAL  ( CODEC_CTL_ADSP_CODEC_CTL_DSP_V | \
                              CODEC_CTL_ONES_DET_1_V )

/* Macro used to initialize the codec control register
*/
#define MSMAUD_INIT_CODEC() \
  HWIO_OUTM( CODEC_CTL, CODEC_CTL_INIT_MASK, CODEC_CTL_INIT_VAL )

/* Macro to reset the ONE's detector
*/
#ifdef FEATURE_AUXPCM
#define MSMAUD_RESET_ONES_DETECT() \
  HWIO_OUTM( CODEC_CTL, \
             HWIO_FMSK(AUX_CODEC_CTL,ONES_POLARITY), \
             HWIO_FMSK(AUX_CODEC_CTL,ONES_POLARITY) )
#else
#define MSMAUD_RESET_ONES_DETECT() 0
#endif

/* Macro to read the ONE's detector
*/
#define MSMAUD_READ_ONES_DETECT() 0

/* Macro to enable adsp clock gating
*/
#if defined(MSMAUD_ADSP_CLK_GATING)
#define MSMAUD_ADSP_CLK_GATING_ENA() \
  HWIO_ADSP_CFG_REG_OUTM(HWIO_ADSP_CFG_REG_ADSP_CLK_OVR_N_BMSK, \
                         0x1 << HWIO_ADSP_CFG_REG_ADSP_CLK_OVR_N_SHFT)
#endif /* MSMAUD_ADSP_CLK_GATING */

/*
Added Defines for Reset and Set DWA bit in Rx Filter Reg in ADIE.
*/
#define ADIE_RX_FILTER_CONTROL_REGISTER1_DWA_M 0x01
#define ADIE_RX_FILTER_CONTROL_REGISTER1_DWA_SET_V 0x01
#define ADIE_RX_FILTER_CONTROL_REGISTER1_DWA_RESET_V 0x00

/* <EJECT> */
#ifndef FEATURE_NO_EXT_RINGER
/*===========================================================================

                       R I N G E R   C O N T R O L

===========================================================================*/
/* Define the type of PMIC device being used, if any. Current choices are
** MSMAUD_PM6050_RINGER and MSMAUD_PM6000_RINGER.
*/
#define MSMAUD_PM6050_RINGER

/* Ringer control definitions
*/
#define MSMAUD_RING_A_STOP_V    0
#define MSMAUD_RING_B_STOP_V    0

#define MSMAUD_RINGER_STOP() \
  HWIO_OUT(RINGER_MN_A_MDIV, MSMAUD_RING_A_STOP_V); \
  HWIO_OUT(RINGER_MN_B_MDIV, MSMAUD_RING_B_STOP_V)

/* Macros to set the M N and D values of M/N Counter A
*/

/* Set the M value and set the start bit to enable the ringer
*/
#define MSMAUD_RINGER_MN_A_M(mval) \
  HWIO_OUT(RINGER_MN_A_MDIV, mval | HWIO_FMSK(RINGER_MN_A_MDIV,RINGER_MN_A_EN))

#define MSMAUD_RINGER_MN_A_N(nval) \
  HWIO_OUT(RINGER_MN_A_NDIV,nval)

#define MSMAUD_RINGER_MN_A_DUTY(dval) \
  HWIO_OUT(RINGER_MN_A_DUTY,dval)

/* Macros to set the M N and D values of M/N Counter B
*/

/* Set the M value and set the start bit to enable the ringer
*/
#define MSMAUD_RINGER_MN_B_M(mval) \
  HWIO_OUT(RINGER_MN_B_MDIV, mval | HWIO_FMSK(RINGER_MN_B_MDIV,RINGER_MN_B_EN))

#define MSMAUD_RINGER_MN_B_N(nval) \
  HWIO_OUT(RINGER_MN_B_NDIV,nval)

#define MSMAUD_RINGER_MN_B_DUTY(dval) \
  HWIO_OUT(RINGER_MN_B_DUTY,dval)


/* <EJECT> */
#endif


#ifdef FEATURE_EXTERNAL_SDAC

/*===========================================================================

          E X T E R N A L   S D A C   C O N F I G U R A T I O N

===========================================================================*/
#ifdef FEATURE_AUDIO_44K_SAMPLE_RATE
#define MSMAUD_ENABLE_EXTERNAL_SDAC_CLOCKS(rate)        \
  {                                                     \
    clkrgm_sdac_clk_type clkrgm_val;                    \
    switch(rate) {                                      \
      case VOC_SAMPLE_RATE_8000:                        \
        clkrgm_val = CLKRGM_SDAC_CLK_256_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_11025:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_352P8_KHZ;         \
        break;                                          \
      case VOC_SAMPLE_RATE_12000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_384_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_16000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_512_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_22050:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_705P6_KHZ;         \
        break;                                          \
      case VOC_SAMPLE_RATE_24000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_768_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_32000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_1024_KHZ;          \
        break;                                          \
      case VOC_SAMPLE_RATE_44000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_1408_KHZ;          \
        break;                                          \
      case VOC_SAMPLE_RATE_44100:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_1411P2_KHZ;        \
        break;                                          \
      case VOC_SAMPLE_RATE_48000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_1536_KHZ;          \
        break;                                          \
      default:                                          \
        return;                                         \
    }                                                   \
    clk_regime_sel_sdac_clk(clkrgm_val);                \
    clk_regime_enable(CLK_RGM_SDAC_M);                  \
  }
#else 
#define MSMAUD_ENABLE_EXTERNAL_SDAC_CLOCKS(rate) \
  {                                                     \
    clkrgm_sdac_clk_type clkrgm_val;                    \
    switch(rate) {                                      \
      case VOC_SAMPLE_RATE_8000:                        \
        clkrgm_val = CLKRGM_SDAC_CLK_256_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_11025:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_352P8_KHZ;         \
        break;                                          \
      case VOC_SAMPLE_RATE_12000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_384_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_16000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_512_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_22050:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_705P6_KHZ;         \
        break;                                          \
      case VOC_SAMPLE_RATE_24000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_768_KHZ;           \
        break;                                          \
      case VOC_SAMPLE_RATE_32000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_1024_KHZ;          \
        break;                                          \
      case VOC_SAMPLE_RATE_44100:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_1411P2_KHZ;        \
        break;                                          \
      case VOC_SAMPLE_RATE_48000:                       \
        clkrgm_val = CLKRGM_SDAC_CLK_1536_KHZ;          \
        break;                                          \
      default:                                          \
        return;                                         \
    }                                                   \
    clk_regime_sel_sdac_clk(clkrgm_val);                \
    clk_regime_enable(CLK_RGM_SDAC_M);                  \
  }
#endif /* FEATURE_AUDIO_44K_SAMPLE_RATE */

#define MSMAUD_DISABLE_EXTERNAL_SDAC_CLOCKS() \
  clk_regime_disable(CLK_RGM_SDAC_M)

#endif /* FEATURE_EXTERNAL_SDAC */

/* <EJECT> */
/*===========================================================================

               A D I E   C O D E C   C O N F I G U R A T I O N S

===========================================================================*/

#ifdef MSMAUD_ADIE_CODEC
#include "adie_regdefs.h"

/*===========================================================================
    Solo - ADIE CODEC registers configuration options
===========================================================================*/

/* ================== REFERENCE_CONTROL1 ================== */

       /* current ref's for RX DAC & TX ADC */
#define MSMAUD_ADIE_REF_CNTL1_INIT_CORE   \
         ADIE_REFERENCE_CONTROL1_INIT_V

/* ================== REFERENCE_CONTROL2 ================== */
      
      /* enable Bandgap power down, enable bandgap ref
      ** power down, enable CCOMP RC filter. 
      */
#define MSMAUD_ADIE_REF_CNTL2_DISABLE_BGAP_REF  \
         (ADIE_REF_CNTL2_BANDGAP_POWER_DOWN_ENA_V        |\
          ADIE_REF_CNTL2_BANDGAP_REF_POWER_DOWN_ENA_V)

      /* disable Bandgap power down, disable bandgap ref
      ** power down, enable CCOMP RC filter. 
      */
#define MSMAUD_ADIE_REF_CNTL2_ENABLE_BGAP_REF  \
                 ADIE_REFERENCE_CONTROL2_INIT_V

/* ================== REFERENCE_CONTROL3 ================== */

#define MSMAUD_ADIE_REF_CNTL3_INIT_CORE   \
        (ADIE_REFERENCE_CONTROL3_INIT_V)

/* ================== AUX_PGA_CONTROL1 ================== */

      /* aux pga_L_R_channel disable
      ** L_R channel gain -21.5 DB 
      */
#define MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_START_UP  \
        (ADIE_AUX_PGA_CONTROL1_INIT_V)

      /* aux pga L_R channel gain 5.5db enable */
#define MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_GAIN_SET_5D5_DB \
        (ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V)

      /* aux pga_L_R_channel enable */
#define MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL   \
        ( ADIE_AUX_PGA_CONTROL1_INIT_V                |\
          ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V      |\
          ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V)

      /* aux pga_L_R_channel disable */
#define MSMAUD_ADIE_AUX_PGA_CNTL1_DISABLE_L_R_CHANNEL  \
        ( ADIE_AUX_PGA_CONTROL1_INIT_V )
    
      /* aux pga_L_channel enable */
#define MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_CHANNEL    \
        ( ADIE_AUX_PGA_CONTROL1_INIT_V                |\
          ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V )
 
/* ================== AUX_PGA_CONTROL2 ================== */

      /* reserved for future use!! */
#define MSMAUD_ADIE_AUX_PGA_CNTL2_INIT_CORE   \
        (ADIE_AUX_PGA_CONTROL2_INIT_V)

/* ================== CODEC_TX_CNTL_REG1 ================== */
         
      /* disable entire tx path */
#define MSMAUD_ADIE_TX_CNTL_REG1_DISABLE_TX_PATH                  \
         (ADIE_CODEC_TX_CNTL_REG1_TX_FE_DIS_V               |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_BYPASS_DIS_V        |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_DIS_V   |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_DIS_V   |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_DIS_V   |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_USB_IN_DIS_V)

      /* enable Mic1 */
#define MSMAUD_ADIE_TX_CNTL_REG1_ENABLE_MIC1             \
        (ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V )

      /* disable Mic1 */
#define MSMAUD_ADIE_TX_CNTL_REG1_DISABLE_MIC1             \
        ((ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V & \
            ~ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_M) | \
            ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_DIS_V)

      /* enable Mic2 */     
#define MSMAUD_ADIE_TX_CNTL_REG1_ENABLE_MIC2             \
        ( MSMAUD_ADIE_TX_CNTL_REG1_DISABLE_TX_PATH      |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V )

      /* enable Mic1, Mic2 */     
#define MSMAUD_ADIE_TX_CNTL_REG1_ENABLE_MIC1N2           \
        ( ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V           |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V )

      /* disable mic1 & mic2 */ 
#define MSMAUD_ADIE_TX_CNTL_REG1_DISABLE_MIC1N2          \
        ( ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_DIS_V   |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_DIS_V)

      /* selects Line-in as mono differntial input */
#define MSMAUD_ADIE_TX_CNTL_REG1_ENABLE_LINEI_AS_MONO_DIFF_I   \
        ( ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V           |\
          ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_MONO_DIFF_V )

      /* selects USB mic as input */
#define MSMAUD_ADIE_TX_CNTL_REG1_ENABLE_USB_MIC           \
        ( ADIE_CODEC_TX_CNTL_REG1_TX_FE_USB_IN_ENA_V )


/* ================== ATEST Register for codecTx ========== */

     /* 0x00 for all modes */
#define MSMAUD_ADIE_ATEST_TX_REGISTER_INIT_V   \
        ( ADIE_ATEST_TX_PATH_CONTROL_REGISTER_INIT_V )


/* ================== DDO_MS Register ================== */

  /* 0x00 for all modes */
#define MSMAUD_ADIE_DDO_MS_INIT_V   \
        ( ADIE_DDO_MS_REGISTER_INIT_V )

/* ================== CODEC_RX_CNTL_REG1 Register ================== */

    /* 0x00 for all modes */
#define MSMAUD_RX_FILTER_CONTROL_REGISTER1_INIT_V   \
        ( ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V )

/* ================== CODEC_RX_CNTL_REG2 Register ================== */

    /* Initial reg value (& for FM narrowband mode)*/
#define MSMAUD_ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V   \
        ( ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V )

   /* enable internal loopback from TxADC on L&R channels */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG2_ENABLE_LOOPBACK  \
        ( ADIE_RX_FILTER_CONTROL2_LEFT_CHAN_LOOPBACK_TX_TO_RX_V     |\
          ADIE_RX_FILTER_CONTROL2_RIGHT_CHAN_LOOPBACK_TX_TO_RX_V )

  /* disable internal loopback from TxADC on L&R channels */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG2_DISABLE_LOOPBACK  \
        ( ADIE_RX_FILTER_CONTROL2_LEFT_CHAN_LOOPBACK_NORMAL_V       |\
          ADIE_RX_FILTER_CONTROL2_RIGHT_CHAN_LOOPBACK_NORMAL_V )


/* ================== CODEC_RX_CNTL_REG4 Register ================== */

    /* Initial reg value 0x00 for all modes */
#define MSMAUD_ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V   \
        ( ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V )

/* ================== CODEC_RX_CNTL_REG5 Register ================== */

    /* Initial reg value 0x00 for all modes except loopback */
#define MSMAUD_ADIE_RX_FILTER_CONTROL_REGISTER5_INIT_V   \
        ( ADIE_RX_FILTER_CONTROL_REGISTER5_INIT_V )

/* ================== CODEC_RX_PA_ENABLE_REG1 Register ================== */

/* all PAs mask */
#define MSMAUD_ADIE_PA_REG1_ALL_PA_MASK     \
        ( ADIE_RX_PA_ENABLE_REGISTER1_INIT_M )
                                      
   /* Initial reg value 0x00  */
#define MSMAUD_ADIE_PA_REG1_DISABLE_RX_PATH  \
        ( ADIE_RX_PA_ENABLE_REGISTER1_INIT_V )

   /* enable PA bias current & EAR reference buffer */
#define MSMAUD_ADIE_PA_REG1_ENABLE_EAR_REF_BUF    \
        ( ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_ENA_V   |\
          ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V )

   /* enable ear piece   */
#define MSMAUD_ADIE_PA_REG1_ENABLE_EAR_PIECE      \
        ( ADIE_RX_PA_ENA_REG1_EAR_PA_ENA_V )

  /* disable ear piece   */
#define MSMAUD_ADIE_PA_REG1_DISABLE_EAR_PIECE      \
        ( ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_DIS_V |\
          ADIE_RX_PA_ENA_REG1_EAR_PA_DIS_V)

   /* enable PA bias current distribution */
#define MSMAUD_ADIE_PA_REG1_ENABLE_IBIAS_EN   \
        ( ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V )

  /* disable PA bias current distribution */
#define MSMAUD_ADIE_PA_REG1_DISABLE_IBIAS_EN   \
        ( ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_DIS_V )

   /* enable left HPH; put codec in either capless or legacy mode in
   ** a separate step since the bit is in a different register.  Default
   ** is legacy mode.
   */
#define MSMAUD_ADIE_PA_REG1_ENABLE_L_HPH  \
        ( ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                |\
          ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V      |\
          ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V )

   /* disable left HPH; put codec in either capless or legacy mode in
   ** a separate step since the bit is in a different register.  Default
   ** is legacy mode.
   */
#define MSMAUD_ADIE_PA_REG1_DISABLE_L_HPH   \
        ( ADIE_RX_PA_ENA_REG1_HPH_LEFT_DIS_V                   |\
          ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_DIS_V )

  /* enable right HPH; put codec in either capless or legacy mode in
  ** a separate step since the bit is in a different register.  Default
  ** is legacy mode.
  */
#define MSMAUD_ADIE_PA_REG1_ENABLE_R_HPH  \
        ( ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V        |\
          ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V                 |\
          ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V )

  /* disable right HPH; put codec in either capless or legacy mode in
  ** a separate step since the bit is in a different register.  Default
  ** is legacy mode.
  */
#define MSMAUD_ADIE_PA_REG1_DISABLE_R_HPH   \
        ( ADIE_RX_PA_ENA_REG1_HPH_RIGHT_DIS_V               |\
          ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_DIS_V )

  /* enable (stereo)left/right HPH; put codec in either capless or legacy mode in
  ** a separate step since the bit is in a different register.  Default
  ** is legacy mode.
  */
#define MSMAUD_ADIE_PA_REG1_ENABLE_L_R_HPH  \
        ( MSMAUD_ADIE_PA_REG1_ENABLE_L_HPH             |\
          MSMAUD_ADIE_PA_REG1_ENABLE_R_HPH )

  /* enable (stereo)left/right HPH reference buffer; 
  ** put codec in either capless or legacy mode in
  ** a separate step since the bit is in a different register.  Default
  ** is legacy mode.
  */
#define MSMAUD_ADIE_PA_REG1_ENABLE_L_R_HPHREF  \
        ( ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V              |\
          ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V             |\
          ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V )

  /* enable left HPH reference buffer; 
  ** put codec in either capless or legacy mode in
  ** a separate step since the bit is in a different register.  Default
  ** is legacy mode.
  */
#define MSMAUD_ADIE_PA_REG1_ENABLE_L_HPHREF  \
        ( ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V            |\
          ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V )


/* ================== CODEC_RX_PA_ENABLE_REG2 Register ================== */

   /* enable PA bias current & Lineout PMIC Ref buf   */
#define MSMAUD_ADIE_PA_REG2_ENABLE_LOPMIC_REF_BUF   \
        ( ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V |\
          ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V)

   /* line out external Right enable */
#define MSMAUD_ADIE_PA_REG2_ENABLE_EN_LOEXT_R   \
        ( ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_ENA_V)

   /* line out external Left enable */
#define MSMAUD_ADIE_PA_REG2_ENABLE_EN_LOEXT_L   \
        ( ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_ENA_V)

  /* line out external Right disable */
#define MSMAUD_ADIE_PA_REG2_DISABLE_EN_LOEXT_R   \
        ( ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_DIS_V)

  /* line out external Left disable */
#define MSMAUD_ADIE_PA_REG2_DISABLE_EN_LOEXT_L   \
       ( ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_DIS_V)

  /* line out external ref buffer enable */
#define MSMAUD_ADIE_PA_REG2_ENABLE_EN_LOEXT_BUF   \
        ( ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_REF_BUFFER_ENA_V |\
          ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_REF_BUFFER_ENA_V)

   /* line out external ref buffer disable */
#define MSMAUD_ADIE_PA_REG2_DISABLE_EN_LOEXT_BUF   \
        ( ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_REF_BUFFER_DIS_V |\
          ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_REF_BUFFER_DIS_V)

  /* Enable linout-external; must turn on pa bias distribution in
  ** a separate step since the bit is in a different register.
  */
#define MSMAUD_ADIE_PA_REG2_ENABLE_LOEXT      \
        ( MSMAUD_ADIE_PA_REG2_ENABLE_EN_LOEXT_L |\
          MSMAUD_ADIE_PA_REG2_ENABLE_EN_LOEXT_BUF   |\
          MSMAUD_ADIE_PA_REG2_ENABLE_EN_LOEXT_R)

/* all PAs mask */
#define MSMAUD_ADIE_PA_REG2_ALL_PA_MASK      \
        ( ADIE_RX_PA_ENABLE_REGISTER2_INIT_M )

   /* Initial reg value 0x00  */
#define MSMAUD_ADIE_PA_REG2_DISABLE_RX_PATH   \
        ( ADIE_RX_PA_ENABLE_REGISTER2_INIT_V )

  /* enable PMIC left */
#define MSMAUD_ADIE_PA_REG2_ENABLE_PMIC_L   \
        ( ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V |\
          ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V )

  /* disable PMIC left */
#define MSMAUD_ADIE_PA_REG2_DISABLE_PMIC_L   \
        ( ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_DIS_V |\
          ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_DIS_V )

  /* enable PMIC right */
#define MSMAUD_ADIE_PA_REG2_ENABLE_PMIC_R   \
        ( ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V |\
          ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V )

/* disable PMIC right */
#define MSMAUD_ADIE_PA_REG2_DISABLE_PMIC_R   \
        ( ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_DIS_V |\
          ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_DIS_V )

/* enable PMIC left and right */
#define MSMAUD_ADIE_PA_REG2_ENABLE_PMIC_LR  \
        ( MSMAUD_ADIE_PA_REG2_ENABLE_PMIC_L  | \
          MSMAUD_ADIE_PA_REG2_ENABLE_PMIC_R )

/* disable PMIC left and right */
#define MSMAUD_ADIE_PA_REG2_DISABLE_PMIC_LR  \
        ( MSMAUD_ADIE_PA_REG2_DISABLE_PMIC_L   | \
          MSMAUD_ADIE_PA_REG2_DISABLE_PMIC_R )

/* ================== CODEC_RX_FILTER_CNTL_REG1  Register ================== */

/* enable filter */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_ENABLE_FILTER  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V )

/* disable filter */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_DISABLE_FILTER  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_DIS_V )

/* enable left channel filter */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_ENABLE_FILTER_L  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V )

/* disable left channel filter */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_DISABLE_FILTER_L  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_DIS_V )

/* enable right channel filter */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_ENABLE_FILTER_R  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V )

/* disable right channel filter */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_DISABLE_FILTER_R  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_DIS_V )

/* use biquad filter (default) */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_BIQUAD_FILTER  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_BIQUAD_V )

/* use 1st order filter */
#define MSMAUD_ADIE_RX_FILTER_CNTL_REG1_1ST_ORDER_FILTER  \
        ( ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V )

/* ================== CODEC_RX_PA_CNTL_REG1  Register ================== */

    /* Initial reg value 0x00  */
#define MSMAUD_ADIE_RX_PA_CNTL_REGISTER1_INIT_V   \
         ( ADIE_RX_PA_CONTROL_REGISTER1_INIT_V )

   /* enable filter for ear piece */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_ENABLE_EAR_FILTER   \
        ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_V )

   /* disable filter for ear piece */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_DISABLE_EAR_FILTER   \
        ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_NONE_V )

   /* enable filter for lineout_PMIC in stereo mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_ENABLE_LOPMIC_FILTER  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_STEREO_V )

   /* enable filter for lineout_PMIC in mono mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_ENABLE_LOPMIC_FILTER_MONO  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_MONO_V )

   /* disable filter for lineout_PMIC */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_DISABLE_LOPMIC_FILTER  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_NONE_V )

   /* enable filter for lineout external in stereo mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_ENABLE_LOEXT_FILTER  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_STEREO_V )

   /* enable filter for lineout external in mono mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_ENABLE_LOEXT_FILTER_MONO  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_MONO_V )

   /* disable filter for lineout external */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_DISABLE_LOEXT_FILTER  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_NONE_V )

   /* enable filter for HPH in stereo mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_ENABLE_HPH_FILTER  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_STEREO_V )

   /* enable filter for HPH in mono mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_ENABLE_HPH_FILTER_MONO  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V )

   /* disable filter for HPH */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_DISABLE_HPH_FILTER  \
          ( ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_NONE_V )

   /* configure HPH PA for capless mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_CFG_HPH_PA_CAPLESS  \
          ( ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V )

   /* configure HPH PA for legacy mode (default) */
#define MSMAUD_ADIE_RX_PA_CNTL_REG1_CFG_HPH_PA_LEGACY  \
          ( ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V )


/* ================== CODEC_RX_PA_CNTL_REG2  Register ================== */

    /* Initial reg value 0x00  */
#define MSMAUD_ADIE_RX_PA_CNTL_REGISTER2_INIT_V   \
        ( ADIE_RX_PA_CONTROL_REGISTER2_INIT_V)

   /* enable Aux PGA to ear piece */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_ENABLE_AUX_EAR   \
        ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_EAR_PIECE_CONNECT_V )

   /* disable Aux PGA to ear piece */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_DISABLE_AUX_EAR   \
        ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_EAR_PIECE_CONNECT_NONE_V )

   /* enable Aux PGA to lineout_PMIC in stereo mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_ENABLE_AUX_SEL_LOPMIC  \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V )

  /* enable Aux PGA to lineout_PMIC in mono differential mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_ENABLE_AUX_SEL_LOPMIC_MONO_EN   \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_MONO_V )

  /* disable Aux PGA to lineout_PMIC  */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_DISABLE_AUX_SEL_LOPMIC   \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_NONE_V )

  /* enable Aux PGA to lineout_external in stereo mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_ENABLE_AUX_SEL_LOEXT  \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_LINEOUT_CONNECT_STEREO_V )

  /* enable Aux PGA to lineout_external in mono differential mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_ENABLE_AUX_SEL_LOEXT_MONO_EN   \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_LINEOUT_CONNECT_MONO_V )

  /* disable Aux PGA to lineout_external  */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_DISABLE_AUX_SEL_LOEXT   \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_LINEOUT_CONNECT_NONE_V )

  /* enable Aux PGA to HPH in stereo mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_ENABLE_AUX_SEL_HPH   \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V )

  /* enable Aux PGA to HPH in mono differential mode */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_ENABLE_AUX_SEL_HPH_MONO_EN    \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_MONO_V )

   /* disable Aux PGA to HPH  */
#define MSMAUD_ADIE_RX_PA_CNTL_REG2_DISABLE_AUX_SEL_HPH    \
          ( ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_NONE_V )


/******************* Solo codec defintions end here.***********************/


/* ================== GPR_B1 ================== */
/* ================== GPR_B2 ================== */
/* ================== DDO_B1 ================== */
/* ================== DDO_B2 ================== */
/* ================== DDI_B1 ================== */

/* ================== CODEC_EN1 ================== */
/* enable codec core, power down all amps. */
#define MSMAUD_ADIE_CODEC_EN1_INIT_CORE       \
        ( ADIE_CODEC_EN1_INIT_V               | \
          ADIE_CODEC_EN1_RXCK_RXCKDLY_L_ENA_V | \
          ADIE_CODEC_EN1_BIQU_L_ENA_V         | \
          ADIE_CODEC_EN1_CODEC_ENA_V )

/* enable EAR1 power amp. only */
#define MSMAUD_ADIE_CODEC_EN1_ENABLE_EAR1     \
        ( MSMAUD_ADIE_CODEC_EN1_INIT_CORE     | \
          ADIE_CODEC_EN1_EAR1_ENA_V )

/* enable left headphone power amp. only */
#define MSMAUD_ADIE_CODEC_EN1_ENABLE_HPH_L    \
        ( MSMAUD_ADIE_CODEC_EN1_INIT_CORE     | \
          ADIE_CODEC_EN1_HPH_L_ENA_V )

/* enable Rx AUX single-ended power amp. only */
#define MSMAUD_ADIE_CODEC_EN1_ENABLE_AUX_SE   \
        ( MSMAUD_ADIE_CODEC_EN1_INIT_CORE     | \
          ADIE_CODEC_EN1_AUX_SE_ENA_V )

/* enable left channel line-out power amp. only */
#define MSMAUD_ADIE_CODEC_EN1_ENABLE_LINE_OUT_L   \
        ( MSMAUD_ADIE_CODEC_EN1_INIT_CORE         | \
          ADIE_CODEC_EN1_LINE_OUT_L_ENA_V )

/* ================== CODEC_EN2 ================== */
/* disable right channel core, keep PA ref for left core */
#define MSMAUD_ADIE_CODEC_EN2_DISABLE_RIGHT_CHAN   \
        ( ADIE_CODEC_EN2_INIT_V                    | \
          ADIE_CODEC_EN2_PA_REF_ENA_V )

/* enable right channel CODEC clock, power down all amps. */
#define MSMAUD_ADIE_CODEC_EN2_INIT_RIGHT_CHAN   \
        ( ADIE_CODEC_EN2_INIT_V                 | \
          ADIE_CODEC_EN2_RXCK_RXCKDLY_R_ENA_V   | \
          ADIE_CODEC_EN2_BIQU_R_ENA_V           | \
          ADIE_CODEC_EN2_PA_REF_ENA_V )

/* enable right headphone power amp. only */
#define MSMAUD_ADIE_CODEC_EN2_ENABLE_HPH_R      \
        ( MSMAUD_ADIE_CODEC_EN2_INIT_RIGHT_CHAN | \
          ADIE_CODEC_EN2_HPH_R_ENA_V )

/* enable Rx capless headphone driver only */
#define MSMAUD_ADIE_CODEC_EN2_ENABLE_CHD        \
        ( MSMAUD_ADIE_CODEC_EN2_INIT_RIGHT_CHAN | \
          ADIE_CODEC_EN2_CHD_ENA_V )

/* enable right channel line-out power amp. only */
#define MSMAUD_ADIE_CODEC_EN2_ENABLE_LINE_OUT_R   \
        ( MSMAUD_ADIE_CODEC_EN2_INIT_RIGHT_CHAN   | \
          ADIE_CODEC_EN2_LINE_OUT_R_ENA_V )

/* ================== CODEC_EN3 ================== */
/* disable Tx left channel, power off all mic amps. */
#define MSMAUD_ADIE_CODEC_EN3_DISABLE_TX_LEFT   \
        ( ADIE_CODEC_EN3_INIT_V )

/* enable Tx left channel, power off micamps. */
#define MSMAUD_ADIE_CODEC_EN3_INIT_TX_LEFT   \
        ( ADIE_CODEC_EN3_INIT_V              | \
          ADIE_CODEC_EN3_MICBIAS_ENA_V       | \
          ADIE_CODEC_EN3_TXCK_L_ENA_V        | \
          ADIE_CODEC_EN3_DITHER_L_ENA_V      | \
          ADIE_CODEC_EN3_TXADC_L_ENA_V       | \
          ADIE_CODEC_EN3_AAF_L_ENA_V         | \
          ADIE_CODEC_EN3_REF_TXFE_L_ENA_V )

/* enable Tx left channel, power on micamp1 only. */
#define MSMAUD_ADIE_CODEC_EN3_ENABLE_MICAMP1_L   \
        ( MSMAUD_ADIE_CODEC_EN3_INIT_TX_LEFT     | \
          ADIE_CODEC_EN3_MICAMP1_L_ENA_V )

/* enable Tx left channel, power on micamp2 only. */
#define MSMAUD_ADIE_CODEC_EN3_ENABLE_MICAMP2_L   \
        ( MSMAUD_ADIE_CODEC_EN3_INIT_TX_LEFT     | \
          ADIE_CODEC_EN3_MICAMP2_L_ENA_V )

/* enable Tx left channel, power on both micamps. */
#define MSMAUD_ADIE_CODEC_EN3_ENABLE_MICAMP12_L  \
        ( MSMAUD_ADIE_CODEC_EN3_INIT_TX_LEFT     | \
          ADIE_CODEC_EN3_MICAMP1_L_ENA_V         | \
          ADIE_CODEC_EN3_MICAMP2_L_ENA_V )

/* ================== CODEC_EN4 ================== */
/* disable Tx right channel, power off all mic amps. */
#define MSMAUD_ADIE_CODEC_EN4_DISABLE_TX_RIGHT   \
        ( ADIE_CODEC_EN4_INIT_V )

/* enable Tx right channel, power off micamps. */
#define MSMAUD_ADIE_CODEC_EN4_INIT_TX_RIGHT   \
        ( ADIE_CODEC_EN4_INIT_V               | \
          ADIE_CODEC_EN4_TXCK_R_ENA_V         | \
          ADIE_CODEC_EN4_DITHER_R_ENA_V       | \
          ADIE_CODEC_EN4_TXADC_R_ENA_V        | \
          ADIE_CODEC_EN4_AAF_R_ENA_V          | \
          ADIE_CODEC_EN4_REF_TXFE_R_ENA_V )

/* enable Tx right channel, power on micamp1 only. */
#define MSMAUD_ADIE_CODEC_EN4_ENABLE_MICAMP1_R   \
        ( MSMAUD_ADIE_CODEC_EN4_INIT_TX_RIGHT    | \
          ADIE_CODEC_EN4_MICAMP1_R_ENA_V )

/* enable Tx right channel, power on micamp2 only. */
#define MSMAUD_ADIE_CODEC_EN4_ENABLE_MICAMP2_R   \
        ( MSMAUD_ADIE_CODEC_EN4_INIT_TX_RIGHT    | \
          ADIE_CODEC_EN4_MICAMP2_R_ENA_V )

/* enable Tx right channel, power on both micamps. */
#define MSMAUD_ADIE_CODEC_EN4_ENABLE_MICAMP12_R  \
        ( MSMAUD_ADIE_CODEC_EN4_INIT_TX_RIGHT    | \
          ADIE_CODEC_EN4_MICAMP1_R_ENA_V         | \
          ADIE_CODEC_EN4_MICAMP2_R_ENA_V )

/* ================== CODEC_EN5 ================== */
/* enable CODEC bandgap core, disable L/R AUX PGAs. */
#define MSMAUD_ADIE_CODEC_EN5_ENABLE_BANDGAP_NOAUXPGA   \
        ( ADIE_CODEC_EN5_INIT_V                         | \
          ADIE_CODEC_EN5_REF_BG_ENA_V                   | \
          ADIE_CODEC_EN5_BANDGAP_ENA_V )

/* enable CODEC bandgap core, enable left AUX PGA only. */
#define MSMAUD_ADIE_CODEC_EN5_ENABLE_BANDGAP_AUXPGA_L   \
        ( MSMAUD_ADIE_CODEC_EN5_ENABLE_BANDGAP_NOAUXPGA | \
          ADIE_CODEC_EN5_AUX_PGA_L_ENA_V )

/* enable CODEC bandgap core, enable right AUX PGA only. */
#define MSMAUD_ADIE_CODEC_EN5_ENABLE_BANDGAP_AUXPGA_R   \
        ( MSMAUD_ADIE_CODEC_EN5_ENABLE_BANDGAP_NOAUXPGA | \
          ADIE_CODEC_EN5_AUX_PGA_R_ENA_V )

/* enable CODEC bandgap core, enable both L/R AUX PGAs. */
#define MSMAUD_ADIE_CODEC_EN5_ENABLE_BANDGAP_AUXPGA_LR  \
        ( MSMAUD_ADIE_CODEC_EN5_ENABLE_BANDGAP_NOAUXPGA | \
          ADIE_CODEC_EN5_AUX_PGA_L_ENA_V                | \
          ADIE_CODEC_EN5_AUX_PGA_R_ENA_V )

/* ================== CODEC_EN6 ================== */
/* enable reference buffer for HPH PA and CHD during wakeup. */
#define MSMAUD_ADIE_CODEC_EN6_ENABLE_HPH_CHD_REF   \
        ( ADIE_CODEC_EN6_INIT_V                    | \
          ADIE_CODEC_EN6_HPH_CHD_REF_ENA_V )

/* ================== CODEC_EN7 : reserved ================== */


/* ================== CODEC_C1 ================== */
/* no gains at all for both left micamps. */
#define MSMAUD_ADIE_CODEC_C1_0_DB_0_DB_L       \
        ( ADIE_CODEC_C1_INIT_V                 | \
          ADIE_CODEC_C1_MICAMP1_GAIN_L_0_DB_V  | \
          ADIE_CODEC_C1_MICAMP2_GAIN_L_0_DB_V )

/* 24dB default gain for micamp1_L, no gain for micamp2_L */
#define MSMAUD_ADIE_CODEC_C1_24_DB_0_DB_L      \
        ( ADIE_CODEC_C1_INIT_V                 | \
          ADIE_CODEC_C1_MICAMP2_GAIN_L_0_DB_V )

/* 24dB default gain for micamp1_L, 6dB gain for micamp2_L */
#define MSMAUD_ADIE_CODEC_C1_24_DB_6_DB_L      \
        ( ADIE_CODEC_C1_INIT_V                 | \
          ADIE_CODEC_C1_MICAMP2_GAIN_L_6_DB_V )

/* 24dB default gain for micamp1_L, 10.5dB gain for micamp2_L */
#define MSMAUD_ADIE_CODEC_C1_24_DB_10D5_DB_L   \
        ( ADIE_CODEC_C1_INIT_V                 | \
          ADIE_CODEC_C1_MICAMP2_GAIN_L_10D5_DB_V )

/* no gain for micamp1_L, 25.5dB default gain for micamp2_L */
#define MSMAUD_ADIE_CODEC_C1_0_DB_25D5_DB_L    \
        ( ADIE_CODEC_C1_INIT_V                 | \
          ADIE_CODEC_C1_MICAMP1_GAIN_L_0_DB_V )

/* ================== CODEC_C2 ================== */
/* no gains at all for both right micamps. */
#define MSMAUD_ADIE_CODEC_C2_0_DB_0_DB_R       \
        ( ADIE_CODEC_C2_INIT_V                 | \
          ADIE_CODEC_C2_MICAMP1_GAIN_R_0_DB_V  | \
          ADIE_CODEC_C2_MICAMP2_GAIN_R_0_DB_V )

/* 24dB default gain for micamp1_R, no gain for micamp2_R */
#define MSMAUD_ADIE_CODEC_C2_24_DB_0_DB_R      \
        ( ADIE_CODEC_C2_INIT_V                 | \
          ADIE_CODEC_C2_MICAMP2_GAIN_R_0_DB_V )

/* no gain for micamp1_R, 25.5dB default gain for micamp2_R */
#define MSMAUD_ADIE_CODEC_C2_0_DB_25D5_DB_R    \
        ( ADIE_CODEC_C2_INIT_V                 | \
          ADIE_CODEC_C2_MICAMP1_GAIN_R_0_DB_V )

/* ================== CODEC_C3 ================== */
/* This configuration is divided into two portions.
   They could be OR-ed together to form complete configuration.
*/
/* Portion 1 (7:5) :
   L/R channel selection for each mic.
*/
/* left channel selected for all mics */
#define MSMAUD_ADIE_CODEC_C3_MIC12A_LLL   \
        ( ADIE_CODEC_C3_INIT_V            | \
          ADIE_CODEC_C3_MIC1_CH_L_V       | \
          ADIE_CODEC_C3_MIC2_CH_L_V       | \
          ADIE_CODEC_C3_MICA_CH_L_V )

#define MSMAUD_ADIE_CODEC_C3_MIC12A_RRR   \
        ( ADIE_CODEC_C3_INIT_V            | \
          ADIE_CODEC_C3_MIC1_CH_R_V       | \
          ADIE_CODEC_C3_MIC2_CH_R_V       | \
          ADIE_CODEC_C3_MICA_CH_R_V )

#define MSMAUD_ADIE_CODEC_C3_MIC12A_LRL   \
        ( ADIE_CODEC_C3_INIT_V            | \
          ADIE_CODEC_C3_MIC1_CH_L_V       | \
          ADIE_CODEC_C3_MIC2_CH_R_V       | \
          ADIE_CODEC_C3_MICA_CH_L_V )

/* Portion 2 (4:0) :
   input mic. selection and micamps bypass config. for left channel.
*/
/* MIC1pn to left channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C3_MIC1_L_NO_BYPASS   \
        ( ADIE_CODEC_C3_INPUT_L_MIC1_PN_V )

/* MIC1pn to left channel selection, bypass micamp2. */
#define MSMAUD_ADIE_CODEC_C3_MIC1_L_BYPASS_MICAMP2   \
        ( MSMAUD_ADIE_CODEC_C3_MIC1_L_NO_BYPASS      | \
          ADIE_CODEC_C3_MICAMP2_L_BYPASS_V )

/* MIC1pn to left channel selection, bypass micamp1. */
#define MSMAUD_ADIE_CODEC_C3_MIC1_L_BYPASS_MICAMP1   \
        ( MSMAUD_ADIE_CODEC_C3_MIC1_L_NO_BYPASS      | \
          ADIE_CODEC_C3_MICAMP1_L_BYPASS_V )

/* MIC2pn to left channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C3_MIC2_L_NO_BYPASS   \
        ( ADIE_CODEC_C3_INPUT_L_MIC2_PN_V )

/* MICApn to left channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C3_MICA_L_NO_BYPASS   \
        ( ADIE_CODEC_C3_INPUT_L_MICA_PN_V )

/* Left Line-in PN to left channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C3_LINEI_L_NO_BYPASS   \
        ( ADIE_CODEC_C3_INPUT_L_LINEI_PN_V )

/* ================== CODEC_C4 ================== */
/* This configuration is divided into two portions.
   They could be OR-ed together to form complete configuration.
*/
/* Portion 1 (7:5) :
   TxADC advanced clock phase on/off, micamp1 current config.
*/
/* clock phase : ON, micamp1 current : 500uA */
#define MSMAUD_ADIE_CODEC_C4_TXADC_CLKADV_ON_500_UA \
        ( ADIE_CODEC_C4_INIT_V                      | \
          ADIE_CODEC_C4_TXADC_CLK_ADV_ENA_V         | \
          ADIE_CODEC_C4_MICAMP1_I_500_UA_V )

/* clock phase : ON, micamp1 current : 1mA */
#define MSMAUD_ADIE_CODEC_C4_TXADC_CLKADV_ON_1_MA \
        ( ADIE_CODEC_C4_INIT_V                    | \
          ADIE_CODEC_C4_TXADC_CLK_ADV_ENA_V       | \
          ADIE_CODEC_C4_MICAMP1_I_1_MA_V )

/* Portion 2 (4:0) :
   input mic. selection and micamps bypass config. for right channel.
*/
/* MIC1pn to right channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C4_MIC1_R_NO_BYPASS   \
        ( ADIE_CODEC_C4_INPUT_R_MIC1_PN_V )

/* MIC2pn to right channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C4_MIC2_R_NO_BYPASS   \
        ( ADIE_CODEC_C4_INPUT_R_MIC2_PN_V )

/* MICApn to right channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C4_MICA_R_NO_BYPASS   \
        ( ADIE_CODEC_C4_INPUT_R_MICA_PN_V )

/* Right Line-in PN to right channel selection, no bypass config. */
#define MSMAUD_ADIE_CODEC_C4_LINEI_R_NO_BYPASS   \
        ( ADIE_CODEC_C4_INPUT_R_LINEI_PN_V )

/* ================== CODEC_C5 ================== */
/* set 3.33uA for TxADC bias current, default for other settings. */
#define MSMAUD_ADIE_CODEC_C5_INIT_TXADC_BIAS_3D33_UA   \
        ( ADIE_CODEC_C5_INIT_V                         | \
          ADIE_CODEC_C5_TXADC_BIAS_3D33_UA_V )

/* set 13.33uA for TxADC bias current, default for other settings. */
#define MSMAUD_ADIE_CODEC_C5_INIT_TXADC_BIAS_13D33_UA   \
        ( ADIE_CODEC_C5_INIT_V                          | \
          ADIE_CODEC_C5_TXADC_BIAS_13D33_UA_V )

/* ================== CODEC_C6 ================== */
/* wide band */
/* Biquad filter settings: Vref = 1.9V (default), OTA BW = 80MHz (default) */
#define MSMAUD_ADIE_CODEC_C6_BIQU_1D9_V_80_MHZ   \
        ( ADIE_CODEC_C6_INIT_V )

/* narrow band */
/* Biquad filter settings: Vref = 1.9V (default), OTA BW = 42MHz */
#define MSMAUD_ADIE_CODEC_C6_BIQU_1D9_V_42_MHZ   \
        ( ADIE_CODEC_C6_INIT_V                   | \
          ADIE_CODEC_C6_BIQU_OTA_BW_42_MHZ_V )

/* ================== CODEC_C7 ================== */

/* ================== CODEC_C7B ================== */
/* This configuration is divided into two portions.
   They could be OR-ed together to form complete configuration.
*/
/* Portion 1 (7:6) :
   CHD reference voltage connection, HSSD to Mic2 input connection
*/
/* CHD Vref : disconnect (default), HSSD-Mic2 : disconnect (default) */
#define MSMAUD_ADIE_CODEC_C7B_INIT   \
        ( ADIE_CODEC_C7B_INIT_V )

/* CHD Vref : connect, HSSD-Mic2 : disconnect */
#define MSMAUD_ADIE_CODEC_C7B_VREF_TO_CHD       \
        ( MSMAUD_ADIE_CODEC_C7B_INIT            | \
          ADIE_CODEC_C7B_VREF_TO_CHD_CONNECT_V )

/* CHD Vref : disconnect, HSSD-Mic2 : connect */
#define MSMAUD_ADIE_CODEC_C7B_HSSD_TO_MIC2      \
        ( MSMAUD_ADIE_CODEC_C7B_INIT            | \
          ADIE_CODEC_C7B_HSSD_TO_MIC2_CONNECT_V )

/* CHD Vref : connect, HSSD-Mic2 : connect */
#define MSMAUD_ADIE_CODEC_C7B_VREF_TO_CHD_HSSD_TO_MIC2   \
        ( MSMAUD_ADIE_CODEC_C7B_INIT                     | \
          ADIE_CODEC_C7B_VREF_TO_CHD_CONNECT_V           | \
          ADIE_CODEC_C7B_HSSD_TO_MIC2_CONNECT_V )

/* Portion 2 (5:0) :
   AUX_PGA configurations
*/

/* ================== CODEC_C8 ================== */
/* This configuration is divided into two portions.
   They could be OR-ed together to form complete configuration.
*/
/* Portion 1 (7:4, 1:0) :
   codec loopback mode
*/
/* no codec loopback */
#define MSMAUD_ADIE_CODEC_C8_INIT   \
        ( ADIE_CODEC_C8_INIT_V )

/* enable codec loopback mode */
#define MSMAUD_ADIE_CODEC_C8_CODEC_LOOPBACK    \
        ( MSMAUD_ADIE_CODEC_C8_INIT            | \
          ADIE_CODEC_C8_CODEC_LOOPBACK_L_ENA_V | \
          ADIE_CODEC_C8_CODEC_LOOPBACK_R_ENA_V )

/* Portion 2 (3:2) :
   HSSD mic2 external capacitor value, HPH differential mode selection
*/
/* 0.1uF (default), differential-mode HPH */
#define MSMAUD_ADIE_CODEC_C8_0D1_UF_DIFF_HPH   \
        ( ADIE_CODEC_C8_HSSD_MIC2_CAP_0D1_UF_V | \
          ADIE_CODEC_C8_HPH_DIFF_SEL_V )

/* ================== CODEC_C9 ================== */
/* This configuration is divided into two portions.
   They could be OR-ed together to form complete configuration.
*/
/* Portion 1 (7:5, 3:0) :
   left channel Rx Biquad filter output enable for differential config.,
   left PA input L/R channel cascade selection
*/
/* no left biquad output (default), no L/R channel cascade (default) */
#define MSMAUD_ADIE_CODEC_C9_INIT   \
        ( ADIE_CODEC_C9_INIT_V )

/* enable left biquad output in differential configuration */
#define MSMAUD_ADIE_CODEC_C9_BIQU_L_OUTPUT_DIFF   \
        ( MSMAUD_ADIE_CODEC_C9_INIT               | \
          ADIE_CODEC_C9_BIQU_L_OUTPUT_SEL_V )

/* feed the left channel to both output amps. in stereo mode */
#define MSMAUD_ADIE_CODEC_C9_LEFT_TO_BOTH_CHANNELS   \
        ( MSMAUD_ADIE_CODEC_C9_INIT                  | \
          ADIE_CODEC_C9_PA_INPUT_L_TO_R_SEL_V        | \
          ADIE_CODEC_C9_PA_INPUT_R_TO_L_SEL_V )

/* Portion 2 (4) :
   line_out differential mode
*/
/* enable line_out differential mode */
#define MSMAUD_ADIE_CODEC_C9_DIFF_LINE_OUT   \
        ( ADIE_CODEC_C9_LINE_OUT_DIFF_SEL_V )

/* ================== CODEC_C10 ================== */

/* ================== CODEC_C11 ================== */

/* ================== CODEC_C12 ================== */
/* This configuration is divided into two portions.
   They could be OR-ed together to form complete configuration.
*/
/* Portion 1 (5:0) :
   micamp2 current control, AAF current control
*/
/* micamp2 : 500uA (default), AAF : 190uA (OTA1), 230uA (OTA2) (default) */
#define MSMAUD_ADIE_CODEC_C12_500_UA_190_UA_230_UA   \
        ( ADIE_CODEC_C12_INIT_V                      | \
          ADIE_CODEC_C12_MICAMP2_I_500_UA_V          | \
          ADIE_CODEC_C12_OTA1_190_UA_OTA2_230_UA_V )

/* micamp2 : 500uA (default), AAF : 380uA (OTA1), 460uA (OTA2) */
#define MSMAUD_ADIE_CODEC_C12_500_UA_380_UA_460_UA   \
        ( ADIE_CODEC_C12_INIT_V                      | \
          ADIE_CODEC_C12_MICAMP2_I_500_UA_V          | \
          ADIE_CODEC_C12_OTA1_380_UA_OTA2_460_UA_V )

/* micamp2 : 1mA, AAF : 760uA (OTA1), 920uA (OTA2) */
#define MSMAUD_ADIE_CODEC_C12_1_MA_760_UA_920_UA   \
        ( ADIE_CODEC_C12_INIT_V                    | \
          ADIE_CODEC_C12_MICAMP2_I_1_MA_V          | \
          ADIE_CODEC_C12_OTA1_760_UA_OTA2_920_UA_V )

/* Portion 2 (7:6) :
   HSSD selection, HSSD-HKADC connection
*/
/* HSSD : selected, HSSD-HKADC : disconnected */
#define MSMAUD_ADIE_CODEC_C12_HSSD   \
        ( ADIE_CODEC_C12_HSSD_SEL_V )

/* HSSD : selected, HSSD-HKADC : connected */
#define MSMAUD_ADIE_CODEC_C12_HSSD_TO_HKADC   \
        ( MSMAUD_ADIE_CODEC_C12_HSSD          | \
          ADIE_CODEC_C12_HSSD_TO_HKADC_ENA_V )

/* ================== CODEC_C13 ================== */
/* This configuration is divided into two portions.
   They could be OR-ed together to form complete configuration.
*/
/* Portion 1 (1:0) :
   test and loopback configurations
*/
/* no 1-bit data loopback */
#define MSMAUD_ADIE_CODEC_C13_INIT   \
        ( ADIE_CODEC_C13_INIT_V )

/* internal 1-bit data loopback */
#define MSMAUD_ADIE_CODEC_C13_INT_1BIT_LOOPBACK   \
        ( MSMAUD_ADIE_CODEC_C13_INIT              | \
          ADIE_CODEC_C13_LOOPBACK_INT_ENA_V )

/* external 1-bit data loopback */
#define MSMAUD_ADIE_CODEC_C13_EXT_1BIT_LOOPBACK   \
        ( MSMAUD_ADIE_CODEC_C13_INIT              | \
          ADIE_CODEC_C13_LOOPBACK_EXT_ENA_V )

/* Portion 2 (3:2) :
   bit 3: reduce front end Tx current by half for 8kHz voice
   bit 2: reduce Tx ADC current, both L/R channels, by half for 8kHz voice
*/
#define MSMAUD_ADIE_CODEC_C13_TXI_HALF_FE_HALF_ADC   \
        ( ADIE_CODEC_C13_HPH_TO_TXAUX_SEL_V          | \
          ADIE_CODEC_C13_EAR1_TO_TXAUX_SEL_V )
   
/* Portion 3 (5) :
   HSSD capless mode configuration
*/
/* enable HSSD capless mode */
#define MSMAUD_ADIE_CODEC_C13_HSSD_CAPLESS_MODE   \
        ( ADIE_CODEC_C13_HSSD_CAPLESS_MODE_ENA_V )

/* ================== CODEC_C14 ================== */
/* disconnect both left and right Biquad filters */
#define MSMAUD_ADIE_CODEC_C14_INIT   \
        ( ADIE_CODEC_C14_INIT_V )

/* hookup EAR1 output */
#define MSMAUD_ADIE_CODEC_C14_EAR1   \
        ( MSMAUD_ADIE_CODEC_C14_INIT | \
          ADIE_CODEC_C14_BIQU_L_TO_EAR1_SEL_V )

/* hookup mono HPH output */
#define MSMAUD_ADIE_CODEC_C14_HPH_L  \
        ( MSMAUD_ADIE_CODEC_C14_INIT | \
          ADIE_CODEC_C14_BIQU_L_TO_HPH_L_SEL_V )

/* hookup stereo HPH output */
#define MSMAUD_ADIE_CODEC_C14_HPH_LR           \
        ( MSMAUD_ADIE_CODEC_C14_INIT           | \
          ADIE_CODEC_C14_BIQU_L_TO_HPH_L_SEL_V | \
          ADIE_CODEC_C14_BIQU_R_TO_HPH_R_SEL_V )

/* hookup mono Line out */
#define MSMAUD_ADIE_CODEC_C14_LINEO_L           \
        ( MSMAUD_ADIE_CODEC_C14_INIT            | \
          ADIE_CODEC_C14_BIQU_L_TO_LINEO_L_SEL_V )

/* hookup stereo Line out */
#define MSMAUD_ADIE_CODEC_C14_LINEO_LR           \
        ( MSMAUD_ADIE_CODEC_C14_INIT             | \
          ADIE_CODEC_C14_BIQU_L_TO_LINEO_L_SEL_V | \
          ADIE_CODEC_C14_BIQU_R_TO_LINEO_R_SEL_V )

/* hookup single-ended AUX */
#define MSMAUD_ADIE_CODEC_C14_AUXSE   \
        ( MSMAUD_ADIE_CODEC_C14_INIT  | \
          ADIE_CODEC_C14_BIQU_L_TO_AUXSE_SEL_V )



/*==========================================================================
	Rx Filter Reg set / reset, DWA  bit 
	This bit should be reset and set in ADIE for QSC 1100 
	to avoid NOISE. The bit should be set and reset after every 
	codec clocks changes or DSP image change.For now have keep the 
	Macro function simple, when stable use ternary operator for
	small foot print of MACRO. Need to report return status in vocstate.
	Assume as of now SBI is success always.
	So do nothing if fail or success.
===========================================================================*/

#define MSMAUD_RX_FILTER_REG_DWA_SET_RESET()                                            \
{                                                                                       \
  adie_result_type adieretval;											                \
  uint8 rx_filter_reg_ptr=0x00;                                                         \
  adieretval = adie_read_byte(ADIE_RX_FILTER_CONTROL_REGISTER1_R,&rx_filter_reg_ptr);   \
  if(adieretval==ADIE_SUCCESS && (rx_filter_reg_ptr & 0x1)){						    \
  if(adie_write_verify(ADIE_RX_FILTER_CONTROL_REGISTER1_R,                              \
  	ADIE_RX_FILTER_CONTROL_REGISTER1_DWA_M,                                             \
  	ADIE_RX_FILTER_CONTROL_REGISTER1_DWA_RESET_V) != ADIE_FAILURE){			            \
	if(adie_write_verify(ADIE_RX_FILTER_CONTROL_REGISTER1_R,                            \
                       ADIE_RX_FILTER_CONTROL_REGISTER1_DWA_M,                          \
                       ADIE_RX_FILTER_CONTROL_REGISTER1_DWA_SET_V) != ADIE_FAILURE){	\
		}																				\
    }																					\
  }																						\
}

/************ Solo codec mode definitions start here.*******/

/*===========================================================================
     Configuration for powering down ADIE codec
===========================================================================*/

#define ADIE_RX_FILTER_CONTROL1_BIT0_DIS_V 0x00

#define MSMAUD_ADIE_CODEC_PD_CONFIG_SEQ \
{                                       \
  /*0x00*/ADIE_MIX_SIGNAL_PRESET_R,            0xFF,                                    0,                                                                               0,  \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0,  \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0,  \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0,  \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0,  \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0,  \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0,  \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0,  \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0,  \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_40P5_LEGACY_MINUS_40P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_40P5_LEGACY_MINUS_40P5_CAPLESS_DB_V, 0,  \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_46P5_LEGACY_MINUS_46P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_46P5_LEGACY_MINUS_46P5_CAPLESS_DB_V, 0,  \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0,  \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_NONE_V                          \
                                                                                 | ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_NONE_V                    \
                                                                                 | ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_NONE_V               \
                                                                                 | ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_NONE_V                  \
                                                                                 | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                             0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_DIS_V                                 \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_DIS_V                              \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_DIS_V                                         \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_DIS_V                             \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_DIS_V                                        \
                                                                                 | ADIE_RX_PA_ENA_REG1_EAR_PA_DIS_V                                           \
                                                                                 | ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_DIS_V),                           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_DIS_V                                         \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_DIS_V                             \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_DIS_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                          \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V           \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_DIS_V                                         \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_DIS_V                                           \
                                                                                 | ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_DIS_V                                        \
                                                                                 | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_27P5_DB_V),                      0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,                                                                                        \
                                                                                (ADIE_REF_CNTL2_BANDGAP_POWER_DOWN_ENA_V                                      \
                                                                                 | ADIE_REF_CNTL2_BANDGAP_REF_POWER_DOWN_ENA_V                                \
                                                                                 | ADIE_REF_CNTL2_CCOMP_RC_FILTER_ENA_V),                                  0, \
}
 
/*===========================================================================
    Configure for HANDSET mode - (MIC1 input -> EARPIECE output)
===========================================================================*/

/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_HANDSET_DELAY_CONFIG_SIZE 2
#define MSMAUD_ADIE_CODEC_HANDSET_CONFIG_SEQ                             \
{                                                                        \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                       | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                   0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     (ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_870_UA_LEGACY_3P33_MA_CAPLESS_V \
                                                                                 | ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V              \
                                                                                 | ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V),    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_10P0_UA_V              \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_05P0_UA_V),                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                            \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_ENA_V                           \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_ENA_V),                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_V                    \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x86,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x02,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x00,   0,*/ \
  /*0x9D,     0xFF,   0x3C,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
    Configure for HANDSET mode without micamp2 - (MIC1 input -> EARPIECE output)
    phone. LCU codec (Solo) does not use micamp2 for MIC1 input, so this
    device configuration is identical to above. 
===========================================================================*/
#define MSMAUD_ADIE_CODEC_HANDSET_NO_MICAMP2_CONFIG_SEQ            \
{                                                                  \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                   0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     (ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_870_UA_LEGACY_3P33_MA_CAPLESS_V \
                                                                                 | ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V              \
                                                                                 | ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V),    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_10P0_UA_V              \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_05P0_UA_V),                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                            \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_ENA_V                           \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_ENA_V),                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_V                    \
                                                                                             | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x86,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x02,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x00,   0,*/ \
  /*0x9D,     0xFF,   0x3C,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
    Configure for HANDSET NO_MIC mode - (no input -> EARPIECE output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_HANDSET_NO_MIC_DELAY_CONFIG_SIZE 2
#define MSMAUD_ADIE_CODEC_HANDSET_NO_MIC_CONFIG_SEQ                        \
{                                                                          \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V,                                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_REGISTER_INIT_V,                                    0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_V,                              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_V,                              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                   0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     (ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_870_UA_LEGACY_3P33_MA_CAPLESS_V \
                                                                                 | ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V              \
                                                                                 | ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V),    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_10P0_UA_V              \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_05P0_UA_V),                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                            \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_ENA_V                           \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_ENA_V),                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_V                    \
                                                                                             | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x86,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x02,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x00,   0,*/ \
  /*0x9D,     0xFF,   0x3C,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   Configure for MONO HEADSET mode - (MIC2 input -> HPH_L output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_MONO_HEADSET_DELAY_CONFIG_SIZE 8

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_MONO_HEADSET_CONFIG_SEQ                          \
{                                                                          \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /* Turn on bias current and reference buffer for hph L */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V),                                       0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                                  0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xC0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
}

#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_MONO_HEADSET_CONFIG_SEQ                  \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /* Turn on bias current and reference buffer for hph L */                \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V),                                       0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                10000, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                             0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                             0, \
  /* Turn on HPH L PA */                                                   \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                          0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x97,     0xFF,   0xC0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   10000,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
}

#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */


/*===========================================================================
   Configure for TTY MONO HEADSET mode - (MIC2 input -> HPH_L output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_MONO_HEADSET_TTY_DELAY_CONFIG_SIZE 8

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_MONO_HEADSET_TTY_CONFIG_SEQ                      \
{                                                                          \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /* Turn on bias current and reference buffer for hph L */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V),                                       0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                                  0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xC0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
}

#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_MONO_HEADSET_TTY_CONFIG_SEQ              \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /* Turn on bias current and reference buffer for hph L */                \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V),                                       0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                10000, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                             0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                             0, \
  /* Turn on HPH L PA */                                                   \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                          0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x97,     0xFF,   0xC0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   10000,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
}

#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

/*===========================================================================
   Configure for MONO HEADPHONE mode - (no input -> HPH_L output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_MONO_HEADPHONE_DELAY_CONFIG_SIZE 8

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE 

#define MSMAUD_ADIE_CODEC_MONO_HEADPHONE_CONFIG_SEQ                \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V),                                       0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V),                                           0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REGISTER_INIT_V),                                             0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_V,                                         0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_V),                                       0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_V,                                         0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                                  0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xC0,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
}

#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_MONO_HEADPHONE_CONFIG_SEQ                \
{                                                                  \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                                  0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V,                                             0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_REGISTER_INIT_V,                                               0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_V,                                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_V,                                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                             0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                          0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V,                             0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}
#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */


/*===========================================================================
  Configure for mono mp3/ringer playback mode - using MIC2 and both 
  headphone channels (MIC2 input -> HPH_Diff output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_STEREO_HEADSET_DELAY_CONFIG_SIZE 16

/* FM over BT AG */
#define MSMAUD_ADIE_CODEC_FM_BT_AG_DELAY_CONFIG_SIZE 14

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

/* FM over BT AG */
#define MSMAUD_ADIE_CODEC_FM_BT_AG_CONFIG_SEQ                        \
{                                                                          \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V   \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V  \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0xA0,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0xA0,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0xA0,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0xA0,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0xA0,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
  /*0xA0,     0xFF,   0x00,   0,*/ \
}


#define MSMAUD_ADIE_CODEC_STEREO_HEADSET_CONFIG_SEQ                        \
{                                                                          \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0xA0,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0xA0,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0xA0,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0xA0,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0xA0,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
  /*0xA0,     0xFF,   0x00,   0,*/ \
}

#else /* Legacy mode */

/* FM over BT AG */
#define MSMAUD_ADIE_CODEC_FM_BT_AG_CONFIG_SEQ                \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V   \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V  \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                10000, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                             0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   10000,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
}


#define MSMAUD_ADIE_CODEC_STEREO_HEADSET_CONFIG_SEQ                \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                10000, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    ADIE_RX_PA_ENABLE_REGISTER1_INIT_V,                                               0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V | 0x1),                                0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                           \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V | 0x1),                                           0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   10000,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
}
#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */


/*===========================================================================
   Configure for STEREO mp3/ringer playback -- (no input -> HPH_LR output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_STEREO_HEADPHONE_DELAY_CONFIG_SIZE 16

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_STEREO_HEADPHONE_CONFIG_SEQ              \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V),                                           0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_DEM_ENA_V),                                                   0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                               \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                              0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V                     \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                                   \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                              0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                                  \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_STEREO_V                              \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                         0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0x81,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0xA0,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0xA0,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0xA0,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0xA0,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0xA0,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
  /*0xA0,     0xFF,   0x00,   0,*/ \
}

#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_STEREO_HEADPHONE_CONFIG_SEQ              \
{                                                                  \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V,                                             0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                                     0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                               \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V                     \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                                   \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                                  \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                       | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                       | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                       | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                                  \
                                                                                       | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                       | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                       | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                       | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                             | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                             | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                                  \
                                                                                             | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                             | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                             | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                             | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                             | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                                  \
                                                                                             | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                             | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    ADIE_RX_PA_ENABLE_REGISTER1_INIT_V,                                               0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V | 0x1),                                0, \
  /* Turn on HPH L/R PA's */                                                                                                                                                \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                           \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V | 0x1),                                           0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                        70000, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_STEREO_V                                      \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                          0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x80,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}
#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

/*===========================================================================
   Dummy definition for handset & LineOut so that FM can be supported on
   other devices.
===========================================================================*/
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_CONFIG_SIZE 0xFF
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_DECONFIG_SIZE 0xFF
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DECONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_CONFIG_SIZE 0xFF
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_DECONFIG_SIZE 0xFF
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DECONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE 0xFF
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL0_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL1_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL2_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL3_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL4_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL5_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL6_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL7_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE 0xFF
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL0_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL1_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL2_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL3_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL4_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL5_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL6_CONFIG_SEQ {0,0,0,0}
#define MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL7_CONFIG_SEQ {0,0,0,0}

/*===========================================================================
   Configure AuxPGA Line In on Stereo Headset
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_DELAY_CONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_CONFIG_SEQ              \
{                                                                            \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           /*0xE0*/ADIE_REFERENCE_CONTROL2_INIT_M,          /*0x00*/ ADIE_REFERENCE_CONTROL2_INIT_V,                                                   0, \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,    /*0x00*/ ADIE_CODEC_TX_CNTL_REG1_TX_FE_DIS_V,                                              0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        /*0xFE*/ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       /*0xFE*/  (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                                           | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                                           | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                                           | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                                           | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                                           | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                                           | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  /*0xFF*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, /*0x58*/ (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                                \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                                                        0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  /*0xFF*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, /*0x64*/ (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V                                                  \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),                                        0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  /*0xFE*/ ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, /*0x64*/ (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V                     \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                                                             \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                                                        0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  /*0xFF*/ ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, /*0x00*/ ADIE_RX_FILTER_CONTROL1_FILTER_DIS_V,                                            0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  /*0xFC*/ ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, /*0x00*/ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V ,                                         0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  /*0xFC*/ ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, /*0x00*/ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V ,                                         0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       /*0xFE*/ ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      /*0xF8*/ (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                                              \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                                                         \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                                             \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                                                   0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     /*0x00*/ MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE ,                                      0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      /*0xFE*/ ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     /*0x0A*/ (  ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_870_UA_LEGACY_3P33_MA_CAPLESS_V     \
                                                                                                       | ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V                           \
                                                                                                       | ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V),                 0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     /*0x55*/ ADIE_RX_PA_CONTROL_REGISTER5_INIT_V,                                             0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      /*0xFC*/ ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     /*0x0C*/ ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      /*0xFC*/ ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     /*0x0C*/ ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x80,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   DeConfigure AuxPGA Line In on Stereo Headset
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_DELAY_DECONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_DECONFIG_SEQ              \
{                                                                  \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x00*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_NONE_V,                               0, \
}

/*===========================================================================
   Configure for AuxPGA Line In on SPEAKER NO_MIC mode. This is for Line In
   audio application
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_DELAY_CONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_CONFIG_SEQ                        \
{                                                                                      \
  /*0x83*/ ADIE_REFERENCE_CONTROL1_R,          /*0xFC*/ ADIE_REFERENCE_CONTROL1_INIT_M,        /*0x18*/  ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  /*0x84*/ ADIE_REFERENCE_CONTROL2_R,          /*0xE0*/ ADIE_REFERENCE_CONTROL2_INIT_M,        /*0x00*/  ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  /*0x88*/ ADIE_CODEC_TX_CNTL_REGISTER1_R,     /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL1_FILTER_DIS_V,                                 0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                          \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V              \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V ),                   0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE,                            0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V,                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0xF0,   0,*/ \
  /*0x99,     0xFF,   0x20,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x18,   0,*/ \
  /*0x9D,     0xFF,   0x01,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   Deconfigure AuxPGA Line In on SPEAKER NO_MIC mode.
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_DELAY_DECONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_DECONFIG_SEQ                        \
{                                                                  \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_NONE_V,                      0, \
}


/*===========================================================================
   Configure for AuxPGA Line In on Stereo USB mode. This is for Line In
   audio application
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_DELAY_CONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_CONFIG_SEQ                        \
{                                                                          \
  /*0x83*/ ADIE_REFERENCE_CONTROL1_R,          /*0xFC*/ ADIE_REFERENCE_CONTROL1_INIT_M,        /*0x18*/  ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  /*0x84*/ ADIE_REFERENCE_CONTROL2_R,          /*0xE0*/ ADIE_REFERENCE_CONTROL2_INIT_M,        /*0x00*/  ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  /*0x88*/ ADIE_CODEC_TX_CNTL_REGISTER1_R,     /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL1_FILTER_DIS_V,                                 0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                          \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V              \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V ),                   0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE,                            0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V,                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0xF0,   0,*/ \
  /*0x99,     0xFF,   0x20,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x18,   0,*/ \
  /*0x9D,     0xFF,   0x01,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   Deconfigure AuxPGA Line In on stereo USB mode.
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_DELAY_DECONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_DECONFIG_SEQ                        \
{                                                                  \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_NONE_V,                      0, \
}


/*===========================================================================
   Configuration of AUX Line In Volume for Headset
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL0_CONFIG_SEQ       \
{                                                                  \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x00*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_NONE_V,                               0, \
}

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL1_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xC0*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_27P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x80*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
}

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL2_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xC8*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_21P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x80*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
}

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL3_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xDC*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_06P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x80*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
}

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL4_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xE4*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_00P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x80*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
}

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL5_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xEC*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                             0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x80*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
}

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL6_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xF4*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_11P5_DB_V),                             0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x80*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
}

#define MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL7_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xFC*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_17P5_DB_V),                             0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x80*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
}

/*===========================================================================
   Configuration of AUX Line In Volume for Speaker or USB
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE 0

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL0_CONFIG_SEQ       \
{                                                                  \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_NONE_V,                      0, \
}

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL1_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xC0*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_27P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,                    0, \
}

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL2_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xC8*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_21P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,                    0, \
}

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL3_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xDC*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_06P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,                    0, \
}

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL4_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xE4*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_MINUS_00P5_DB_V),                            0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,                    0, \
}

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL5_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xEC*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                             0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,                    0, \
}

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL6_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xF4*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_11P5_DB_V),                             0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/  ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,                   0, \
}

#define MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL7_CONFIG_SEQ       \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             /*0xFC*/ ADIE_AUX_PGA_CONTROL1_INIT_M,            /*0xFC*/ (MSMAUD_ADIE_AUX_PGA_CNTL1_ENABLE_L_R_CHANNEL                                       \
                                                                                                           | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_17P5_DB_V),                             0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      /*0xFF*/ ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     /*0x08*/ ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V,                    0, \
}

/*===========================================================================
   Configuration of FM Recording path to DSP
===========================================================================*/
#define MSMAUD_ADIE_CODEC_LINE_IN_STEREO_TX_DELAY_CONFIG_SIZE  0

#define MSMAUD_ADIE_CODEC_LINE_IN_STEREO_TX_CONFIG_SEQ             \
{                                                                  \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      /*0xFF*/ ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     /*0xC8*/ (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V), 0, \
}

#ifdef FEATURE_INTERNAL_SADC
/*===========================================================================
        Configure for line-in mono mode (Line-in Left -> HPH_Diff output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_MONO_SADC_STEREO_HEADSET_DELAY_CONFIG_SIZE 15

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_MONO_SADC_STEREO_HEADSET_CONFIG_SEQ              \
{                                                                          \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V                 \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                                     0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                               \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                              0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V                     \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                                   \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                              0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V                                                 \
                                                                                 | ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V                                              \
                                                                                 | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                             0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V,                                         0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V,                                         0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,                                         0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V,                                     0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     (ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_MONO_V                                  \
                                                                                 | ADIE_RX_PA_CONTROL2_HPH_PA_RIGHT_MONO_CONFIG_DIFF_V),                         0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0x01,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0xA0,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0x98,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x86,     0xFF,   0xEC,   0,*/ \
  /*0x91,     0xFF,   0x00,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0x00,   0,*/ \
  /*0x94,     0xFF,   0x00,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0xC1,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0xA0,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0xA0,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0xA0,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0xA0,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
  /*0xA0,     0xFF,   0x00,   0,*/ \
}

#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_MONO_SADC_STEREO_HEADSET_CONFIG_SEQ              \
{                                                                          \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                                  0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V                                                 \
                                                                                 | ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V                                              \
                                                                                 | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                             0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V                 \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V),                             0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                                     0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                               \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V                     \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                                   \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,                                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     (ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_MONO_V                                  \
                                                                                       | ADIE_RX_PA_CONTROL2_HPH_PA_RIGHT_MONO_CONFIG_DIFF_V),                         0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0xEC,   0,*/ \
  /*0x88,     0xFF,   0x98,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0x00,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0x00,   0,*/ \
  /*0x94,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0xC1,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

/*===========================================================================
        Configure for FM playback thru stereo headphone -- 
        line_in, aux_pga, HPH_LR: (Line_in_LR input -> HPH_LR output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_STEREO_SADC_STEREO_HEADSET_DELAY_CONFIG_SIZE 14

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_STEREO_SADC_STEREO_HEADSET_CONFIG_SEQ                \
{                                                                              \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V,                                     0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V                 \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                                     0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                               \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                              0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V                     \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                                   \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                              0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V                                                 \
                                                                                 | ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V                                              \
                                                                                 | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                             0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V,                                         0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V,                                         0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,                                         0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0x01,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0xA0,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0x98,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x86,     0xFF,   0xEC,   0,*/ \
  /*0x91,     0xFF,   0x00,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0x00,   0,*/ \
  /*0x94,     0xFF,   0x00,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x80,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0xA0,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0xA0,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0xA0,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0xA0,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
  /*0xA0,     0xFF,   0x00,   0,*/ \
}
#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_STEREO_SADC_STEREO_HEADSET_CONFIG_SEQ            \
{                                                                          \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                                  0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V                                                 \
                                                                                 | ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V                                              \
                                                                                 | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                             0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V                 \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V),                             0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                                     0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                               \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V                     \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                                   \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,                                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL2_AUX_PGA_TO_HPH_CONNECT_STEREO_V,                             0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0xEC,   0,*/ \
  /*0x88,     0xFF,   0x98,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0x00,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0x00,   0,*/ \
  /*0x94,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x80,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}
#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
#endif /* FEATURE_INTERNAL_SADC */

/*===========================================================================
   Configure for speaker phone stereo (external stereo amplifier):
   (MIC1 input -> Line_out_LR output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_AUX_CONFIG_SEQ                           \
{                                                                  \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_0P25X_NOMINAL_V,     0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_10P0_UA_V,                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_REF_BUFFER_ENA_V                      \
                                                                                       | ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_ENA_V                               \
                                                                                       | ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_REF_BUFFER_ENA_V                   \
                                                                                       | ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_ENA_V),                         0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_STEREO_V               \
                                                                                             | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0xF0,   0,*/ \
  /*0x99,     0xFF,   0x20,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x18,   0,*/ \
  /*0x9D,     0xFF,   0x01,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}


/*===========================================================================
   Configure for external stereo speaker for mp3 playback --
   This mode uses external amplifier (no input -> Line_out_LR output). 
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_AUX_NO_MIC_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_AUX_NO_MIC_CONFIG_SEQ                    \
{                                                                  \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V,                                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                          0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                    \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                 \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V             \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V          \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                   0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_0P25X_NOMINAL_V,     0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_10P0_UA_V,                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_REF_BUFFER_ENA_V                      \
                                                                                       | ADIE_RX_PA_ENA_REG2_LINE_OUT_LEFT_ENA_V                               \
                                                                                       | ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_REF_BUFFER_ENA_V                   \
                                                                                       | ADIE_RX_PA_ENA_REG2_LINE_OUT_RIGHT_ENA_V),                         0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_LINEOUT_CONNECT_STEREO_V               \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0xF0,   0,*/ \
  /*0x99,     0xFF,   0x20,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x18,   0,*/ \
  /*0x9D,     0xFF,   0x01,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

#ifdef FEATURE_K_AMPLIFIER
/*===========================================================================
   Configure for SPEAKER PHONE Stereo - (MIC1 input -> HPH_L output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_SPEAKER_DELAY_CONFIG_SIZE 16

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_SPEAKER_CONFIG_SEQ                          \
{                                                                          \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0xA0,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0xA0,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0xA0,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0xA0,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0xA0,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
  /*0xA0,     0xFF,   0x00,   0,*/ \
}
#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_SPEAKER_CONFIG_SEQ                  \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                10000, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    ADIE_RX_PA_ENABLE_REGISTER1_INIT_V,                                               0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V | 0x1),                                0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                           \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V | 0x1),                                           0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   10000,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
}

#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

/*===========================================================================
   Configure for SPEAKER PHONE Mono no mic - (no input -> HPH_L output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_SPEAKER_NO_MIC_DELAY_CONFIG_SIZE 16

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_SPEAKER_NO_MIC_CONFIG_SEQ                          \
{                                                                          \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Turn on bias current and reference buffer for hph L/R */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0xA0,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0xA0,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0xA0,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0xA0,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0xA0,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
  /*0xA0,     0xFF,   0x00,   0,*/ \
}
#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_SPEAKER_NO_MIC_CONFIG_SEQ                  \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V),                  0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                10000, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    ADIE_RX_PA_ENABLE_REGISTER1_INIT_V,                                               0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x18,                                                                             0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V | 0x1),                                0, \
  /* Turn on HPH L/R PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                           \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V | 0x1),                                           0, \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       0xFF,                                    (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                                         0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      0xFF,                                    0x38,                                                                            0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_LEGACY_V),                                  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x97,     0xFF,   0xD0,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x40,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   10000,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
  /*0x97,     0xFF,   0xF8,   0,*/ \
}

#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */

#else
/*===========================================================================
   Configure for SPEAKER PHONE Mono -- 
   (MIC1 input -> Line_Out_PMIC_Diff output)
===========================================================================*/

/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_SPEAKER_DELAY_CONFIG_SIZE 12
/*
** NOTE: The UI keeps using speaker as target audio device eventhough
** we chose headset from the device settings menu; hacked for now
** to re-route to stereo headset. When this is addressed, put back speaker
** codec settings. 
** Vandy - 02/04/2008
*/
#define MSMAUD_ADIE_CODEC_SPEAKER_CONFIG_SEQ                               \
{                                                                          \
    ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V, 0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
 /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V),                                      0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                       | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                \
  																				 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V                                               \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_REF_BUFFER_ENA_V                                   \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_RIGHT_ENA_V),                               0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V             \
  																				 | ADIE_RX_PA_ENABLE_REGISTER2_INIT_V                                  \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                      \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V          \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V ),               0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_MONO_V            \
                                                                                 | ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V                        \
																				 | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     (ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V         \
                                                                                 | ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V),      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V                     \
                                                                                 | ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V),                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     (ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V       \
                                                                                 | ADIE_RX_PA_CONTROL_REGISTER6_INIT_V),                              0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V, 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0xA0*/ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL8_HPH_RIGHT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V, 0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0xF0,   0,*/ \
  /*0x99,     0xFF,   0x20,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x18,   0,*/ \
  /*0x9D,     0xFF,   0x01,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   Configure for SPEAKER NO_MIC mode. This is for USB audio 
   application --  (no input -> Line_out_PMIC_LR output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_SPEAKER_NO_MIC_DELAY_CONFIG_SIZE 12
/*
** NOTE: The UI keeps using speaker as target audio device eventhough
** we chose headset from the device settings menu; hacked for now
** to re-route to stereo headset. When this is addressed, put back speaker
** codec settings. 
** Vandy - 02/04/2008
*/
#define MSMAUD_ADIE_CODEC_SPEAKER_NO_MIC_CONFIG_SEQ                        \
{                                                                          \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                       | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                      \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V          \
                                                                                 | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V ),               0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_MONO_V            \
                                                                                 | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V,                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0xF0,   0,*/ \
  /*0x99,     0xFF,   0x20,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x18,   0,*/ \
  /*0x9D,     0xFF,   0x01,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}
#endif

/*===========================================================================
   Configure for TTY VCO mode - (MIC1 input -> HPH_L output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_TTY_VCO_DELAY_CONFIG_SIZE 8

#ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE

#define MSMAUD_ADIE_CODEC_TTY_VCO_CONFIG_SEQ                       \
{                                                                  \
  /* Enable bandgap */                                                     \
  /*0x83*/ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  /*0x84*/ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                               5000, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_58P5_LEGACY_MINUS_58P5_CAPLESS_DB_V,  0, \
  /* Turn on bias current and reference buffer for hph L */              \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V),                                       0, \
  /* Configure Tx control registers */                                     \
  /*0x88*/ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                             0, \
  /*0x89*/ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  /*0x8B*/ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  /*0x8C*/ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  /*0x8D*/ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  /*0x8E*/ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  /* Configure AUX_PGA control registers */                                \
  /*0x86*/ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  /* TBD: Wakeup DSP and send PCM offset for XYZ mV then wait 10ms*/       \
  /* Configure Rx filter control registers */                              \
  /*0x91*/ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  /*0x92*/ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  /*0x93*/ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  /*0x94*/ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  /* Configure Rx PA control registers */                                  \
  /*0x98*/ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  /*0x9A*/ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  /*0x9B*/ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  /*0x9C*/ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  /*0x9D*/ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  /*0x9E*/ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  /* Turn on HPH L PA's */                                               \
  /*0x97*/ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                            \
                                                                                 | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                                  0, \
  /*0x99*/ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                 | ADIE_RX_PA_CONTROL1_HPH_PA_CONFIG_CAPLESS_V),                                 0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   5000,*/ \
  /*0x99,     0xFF,   0xC1,   0,*/ \
  /*0x9F,     0xFF,   0x9C,   0,*/ \
  /*0x97,     0xFF,   0xC0,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
  /*0x9F,     0xFF,   0x5C,   0,*/ \
  /*0x9F,     0xFF,   0x3C,   0,*/ \
  /*0x9F,     0xFF,   0x2C,   0,*/ \
  /*0x9F,     0xFF,   0x1C,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0x9F,     0xFF,   0x00,   0,*/ \
}

#else /* Legacy mode */

#define MSMAUD_ADIE_CODEC_TTY_VCO_CONFIG_SEQ                       \
{                                                                  \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                                  0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                                  0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                                    0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                                \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                                       \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC1_IN_SEL_ENA_V),                             0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                             0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                                        \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                                       \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                                     \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                               \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                                      \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                                    0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                                  0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V                       \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),              0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,                  0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                               \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                                \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                                   \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),                      0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                                         0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),                       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                               \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                                       \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                                   0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                              0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_180_UA_LEGACY_0830_UA_CAPLESS_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_10P0_UA_V,                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                             0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                             0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                                       \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_REF_BUFFER_ENA_V                                    \
                                                                                       | ADIE_RX_PA_ENA_REG1_HPH_LEFT_ENA_V),                                          0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_HPH_CONNECT_MONO_V                                \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                             0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_34P5_LEGACY_MINUS_34P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_22P5_LEGACY_MINUS_22P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_16P5_LEGACY_MINUS_16P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_10P5_LEGACY_MINUS_10P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_MINUS_04P5_LEGACY_MINUS_04P5_CAPLESS_DB_V,  0, \
  /*0x9F*/ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL7_HPH_LEFT_PA_GAIN_NONE_LEGACY_PLUS_0P0_CAPLESS_DB_V,  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC2,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0xE0,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0xC0,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0xA0,   0,*/ \
  /*0x9D,     0xFF,   0x40,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

#endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */


/*===========================================================================
   Configure for TTY HCO mode - (MIC2 input -> EARPIECE output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_TTY_HCO_DELAY_CONFIG_SIZE 2

#define MSMAUD_ADIE_CODEC_TTY_HCO_CONFIG_SEQ                               \
{                                                                          \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_MIC2_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      ADIE_RX_PA_ENABLE_REGISTER2_INIT_V,                                   0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     (ADIE_RX_PA_CONTROL4_HPH_PA_OUTPUT_STAGE_BIAS_870_UA_LEGACY_3P33_MA_CAPLESS_V \
                                                                                 | ADIE_RX_PA_CONTROL4_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V              \
                                                                                 | ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_2X_NOMINAL_V),    0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_10P0_UA_V              \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_05P0_UA_V),                  0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      (ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V                            \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_REF_BUFFER_ENA_V                           \
                                                                                       | ADIE_RX_PA_ENA_REG1_EAR_PA_ENA_V),                                 0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_EAR_PIECE_CONNECT_V                    \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xC4,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x86,   0,*/ \
  /*0x98,     0xFF,   0x00,   0,*/ \
  /*0x99,     0xFF,   0x02,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x00,   0,*/ \
  /*0x9D,     0xFF,   0x3C,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   Configure for usb phone -- (USB mic input -> Line_Out_PMIC_L output)
   (intended for USB Audio carkit).
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_USB_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_USB_CONFIG_SEQ             \
{                                                    \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_USB_IN_ENA_V),                       0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_12P5_UA_V                         \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_25P0_UA_V            \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_12P5_UA_V                       \
                                                                                 | ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V),              0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V),                     0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_MONO_V            \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xE0,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0x0C,   0,*/ \
  /*0x99,     0xFF,   0x0C,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x02,   0,*/ \
  /*0x9D,     0xFF,   0x00,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}


/*===========================================================================
   Configure for mono ringer thru speaker -- 
   (no input -> Line_Out_PMIC_Diff output) 
   (intended for USB Audio carkit)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_USB_NO_MIC_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_USB_NO_MIC_CONFIG_SEQ                     \
{                                                                   \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V,                                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                          0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                    \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                 \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V             \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V          \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                   0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_12P5_UA_V                         \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_25P0_UA_V            \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_12P5_UA_V                       \
                                                                                 | ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V),              0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                          \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V              \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V),                    0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_MONO_V            \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0xD6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0x0F,   0,*/ \
  /*0x99,     0xFF,   0x0C,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x02,   0,*/ \
  /*0x9D,     0xFF,   0x00,   0,*/ \
  /*0x9E,     0xFF,   0x10,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   Configure for usb phone -- (USB MIC input -> Line_Out_PMIC_LR output) 
   (intended for USB Audio carkit)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_STEREO_USB_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_STEREO_USB_CONFIG_SEQ                                \
{                                                                              \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_GAIN_24_DB_V                            \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_USB_IN_ENA_V),                       0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       (ADIE_CODEC_TX_ADC_REF_ENA_V                                             \
                                                                                 | ADIE_CODEC_TX_ADC_DAC_REF_COMPENSATE_ENA_V                            \
                                                                                 | ADIE_CODEC_TX_ADC_OTA1_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_OTA2_ENA_V                                          \
                                                                                 | ADIE_CODEC_TX_ADC_COMPARATOR_ENA_V                                    \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ENA_V                                           \
                                                                                 | ADIE_CODEC_TX_ADC_DEM_ERROR_DETECT_ENA_V),                         0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V,                       0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V,       0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_12P5_UA_V                         \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_25P0_UA_V            \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_12P5_UA_V                       \
                                                                                 | ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V),              0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                          \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V              \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V),                    0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_STEREO_V          \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0xE0,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0xFE,   0,*/ \
  /*0x8C,     0xFF,   0x08,   0,*/ \
  /*0x8D,     0xFF,   0x44,   0,*/ \
  /*0x8E,     0xFF,   0x40,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0x0F,   0,*/ \
  /*0x99,     0xFF,   0x08,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x02,   0,*/ \
  /*0x9D,     0xFF,   0x00,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
   Configure for stereo USB mp3/ringer playback -- 
   (no input -> Line_Out_PMIC_LR output)
   (intended for USB Audio carkit)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_STEREO_USB_NO_MIC_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_STEREO_USB_NO_MIC_CONFIG_SEQ               \
{                                                                    \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            ADIE_AUX_PGA_CONTROL1_INIT_V,                                         0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER1_INIT_V,                                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                          0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                    \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                 \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V             \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V          \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                   0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, (ADIE_RX_FILTER_CONTROL1_FILTER_ENA_V                                    \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_LEFT_CHAN_ENA_V                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_RIGHT_CHAN_ENA_V                       \
                                                                                 | ADIE_RX_FILTER_CONTROL1_FILTER_CONFIG_1ST_ORDER_V                     \
                                                                                 | ADIE_RX_FILTER_CONTROL1_DWA_TO_RX_FILTER_FALLING_EDGE_SYNC_CLK_V      \
                                                                                 | ADIE_RX_FILTER_CONTROL1_BIT0_V                                        \
                                                                                 | ADIE_RX_FILTER_CONTROL1_RX_DAC_REF_BUFFER_DECOUP_ENA_V),           0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, (ADIE_RX_FILTER_CONTROL3_CMFB_REF_BUFFER_BIAS_15_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VICM_REF_BUFFER_BIAS_075_UA_V                 \
                                                                                 | ADIE_RX_FILTER_CONTROL3_VOCM_REF_BUFFER_BIAS_075_UA_V),            0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, (ADIE_RX_FILTER_CONTROL4_DAC_REF_BUFFER_BIAS_105_UA_V                    \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA1_BIAS_005_UA_V                            \
                                                                                 | ADIE_RX_FILTER_CONTROL4_OTA2_BIAS_05_UA_V),                        0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER2_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_12P5_UA_V                         \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_25P0_UA_V            \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_12P5_UA_V                       \
                                                                                 | ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V),              0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                          \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V              \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V),                    0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     (ADIE_RX_PA_CONTROL1_RX_FILTER_TO_PMIC_LINEOUT_CONNECT_STEREO_V          \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x08,   0,*/ \
  /*0x88,     0xFF,   0x02,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0xF6,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0xFC,   0,*/ \
  /*0x94,     0xFF,   0xFC,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0x0F,   0,*/ \
  /*0x99,     0xFF,   0x08,   0,*/ \
  /*0x9A,     0xFF,   0x00,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x02,   0,*/ \
  /*0x9D,     0xFF,   0x00,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

#ifdef FEATURE_INTERNAL_SADC
/*===========================================================================
        Configure for USB mp3/ringer playback --
        (Line_in_L input -> Line_out_PMIC_Diff output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_MONO_SADC_MONO_HANDSET_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_MONO_SADC_MONO_HANDSET_CONFIG_SEQ                    \
{                                                                              \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            (ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V                                     \
                                                                                 | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V      \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                          0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                    \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                 \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V             \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V          \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                   0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,                              0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER1_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_12P5_UA_V                         \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_25P0_UA_V            \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_12P5_UA_V                       \
                                                                                 | ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V),              0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL6_PMIC_LINEOUT_RIGHT_PA_POLARITY_INVERTED_ENA_V,    0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                          \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V              \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V),                    0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     (ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_MONO_V              \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0x6C,   0,*/ \
  /*0x88,     0xFF,   0x98,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0x00,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0x00,   0,*/ \
  /*0x94,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0x0F,   0,*/ \
  /*0x99,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x0C,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x02,   0,*/ \
  /*0x9D,     0xFF,   0x00,   0,*/ \
  /*0x9E,     0xFF,   0x10,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

/*===========================================================================
        Configure for stereo USB audio mode -- 
        (Line_in_LR input -> Line_out_PMIC_LR output)
===========================================================================*/
/* Size of the number of entries to delay (from the end) to allow adie warmup */
#define MSMAUD_ADIE_CODEC_STEREO_SADC_MONO_HANDSET_DELAY_CONFIG_SIZE 3

#define MSMAUD_ADIE_CODEC_STEREO_SADC_MONO_HANDSET_CONFIG_SEQ              \
{                                                                          \
  ADIE_REFERENCE_CONTROL1_R,           ADIE_REFERENCE_CONTROL1_INIT_M,          ADIE_REFERENCE_CONTROL1_INIT_V,                                       0, \
  ADIE_REFERENCE_CONTROL2_R,           ADIE_REFERENCE_CONTROL2_INIT_M,          ADIE_REFERENCE_CONTROL2_INIT_V,                                       0, \
  ADIE_AUX_PGA_CONTROL1_R,             ADIE_AUX_PGA_CONTROL1_INIT_M,            (ADIE_AUX_PGA_CNTL1_LEFT_CHAN_ENA_V                                      \
                                                                                 | ADIE_AUX_PGA_CNTL1_RIGHT_CHAN_ENA_V                                   \
                                                                                 | ADIE_AUX_PGA_CNTL1_L_R_CHAN_GAIN_PLUS_05P5_DB_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER1_R,      ADIE_CODEC_TX_CNTL_REGISTER1_INIT_M,     (ADIE_CODEC_TX_CNTL_REG1_TX_FE_ENA_V                                     \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_MODE_STEREO_SINGLE_ENDED_V      \
                                                                                 | ADIE_CODEC_TX_CNTL_REG1_TX_FE_LINE_IN_SEL_ENA_V),                  0, \
  ADIE_CODEC_TX_CNTL_REGISTER2_R,      ADIE_CODEC_TX_CNTL_REGISTER2_INIT_M,     ADIE_CODEC_TX_CNTL_REGISTER2_INIT_V,                                  0, \
  ADIE_CODEC_TX_ADC_REGISTER_R,        ADIE_CODEC_TX_ADC_REGISTER_INIT_M,       ADIE_CODEC_TX_ADC_DEM_ENA_V,                                          0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER1_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_DDA_BIAS_10_UA_V                    \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_SRC_BIAS_160_UA_V                 \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG1_DAC_REF_1P6_VOLT_V),                   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER2_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG2_COMPARATOR_THRESHOLD_356_MVOLT_V            \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_BIAS_200_UA_V             \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG2_VICM_REF_BUFFER_OUTPUT_1P15_VOLT_V),   0, \
  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_R,  ADIE_CODEC_TX_ADC_CNTL_REGISTER3_INIT_M, (ADIE_CODEC_TX_ADC_CNTL_REG3_VOCM_REF_BUFFER_OUTPUT_0P90_VOLT_V          \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA1_BIAS_020_UA_V                        \
                                                                                 | ADIE_CODEC_TX_ADC_CNTL_REG3_OTA2_BIAS_010_UA_V),                   0, \
  ADIE_RX_FILTER_CONTROL_REGISTER1_R,  ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER1_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER2_R,  ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER2_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER3_R,  ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER3_INIT_V,                              0, \
  ADIE_RX_FILTER_CONTROL_REGISTER4_R,  ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_M, ADIE_RX_FILTER_CONTROL_REGISTER4_INIT_V,                              0, \
  ADIE_RX_PA_CONTROL_REGISTER1_R,      ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER1_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER3_R,      ADIE_RX_PA_CONTROL_REGISTER3_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER3_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER4_R,      ADIE_RX_PA_CONTROL_REGISTER4_INIT_M,     ADIE_RX_PA_CONTROL4_PMIC_LINEOUT_PA_OUTPUT_STAGE_BIAS_NOMINAL_V,      0, \
  ADIE_RX_PA_CONTROL_REGISTER5_R,      ADIE_RX_PA_CONTROL_REGISTER5_INIT_M,     (ADIE_RX_PA_CONTROL5_HPH_PA_INPUT_BIAS_12P5_UA_V                         \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_REF_BUFFER_INPUT_BIAS_25P0_UA_V            \
                                                                                 | ADIE_RX_PA_CONTROL5_EAR_PA_INPUT_BIAS_12P5_UA_V                       \
                                                                                 | ADIE_RX_PA_CONTROL5_LINEOUT_PA_INPUT_BIAS_12P5_UA_V),              0, \
  ADIE_RX_PA_CONTROL_REGISTER6_R,      ADIE_RX_PA_CONTROL_REGISTER6_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER6_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER7_R,      ADIE_RX_PA_CONTROL_REGISTER7_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER7_INIT_V,                                  0, \
  ADIE_RX_PA_CONTROL_REGISTER8_R,      ADIE_RX_PA_CONTROL_REGISTER8_INIT_M,     ADIE_RX_PA_CONTROL_REGISTER8_INIT_V,                                  0, \
  ADIE_RX_PA_ENABLE_REGISTER1_R,       ADIE_RX_PA_ENABLE_REGISTER1_INIT_M,      ADIE_RX_PA_ENA_REG1_PA_BIAS_DISTRIBUTE_ENA_V,                         0, \
  ADIE_RX_PA_ENABLE_REGISTER2_R,       ADIE_RX_PA_ENABLE_REGISTER2_INIT_M,      (ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_REF_BUFFER_ENA_V                 \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_LEFT_ENA_V                          \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_REF_BUFFER_ENA_V              \
                                                                                       | ADIE_RX_PA_ENA_REG2_PMIC_LINE_OUT_RIGHT_ENA_V),                    0, \
  ADIE_RX_PA_CONTROL_REGISTER2_R,      ADIE_RX_PA_CONTROL_REGISTER2_INIT_M,     (ADIE_RX_PA_CONTROL2_AUX_PGA_TO_PMIC_LINEOUT_CONNECT_STEREO_V            \
                                                                                       | MSMAUD_ADIE_RX_PA_CNTL1_HPH_PA_CNFIG_MODE),                  0, \
  /*0x83,     0xFF,   0x18,   0,*/ \
  /*0x84,     0xFF,   0x00,   0,*/ \
  /*0x86,     0xFF,   0xEC,   0,*/ \
  /*0x88,     0xFF,   0x98,   0,*/ \
  /*0x89,     0xFF,   0x34,   0,*/ \
  /*0x8B,     0xFF,   0x04,   0,*/ \
  /*0x8C,     0xFF,   0x58,   0,*/ \
  /*0x8D,     0xFF,   0x64,   0,*/ \
  /*0x8E,     0xFF,   0x64,   0,*/ \
  /*0x91,     0xFF,   0x00,   0,*/ \
  /*0x92,     0xFF,   0x00,   0,*/ \
  /*0x93,     0xFF,   0x00,   0,*/ \
  /*0x94,     0xFF,   0x00,   0,*/ \
  /*0x97,     0xFF,   0x80,   0,*/ \
  /*0x98,     0xFF,   0x0F,   0,*/ \
  /*0x99,     0xFF,   0x00,   0,*/ \
  /*0x9A,     0xFF,   0x08,   0,*/ \
  /*0x9B,     0xFF,   0x00,   0,*/ \
  /*0x9C,     0xFF,   0x02,   0,*/ \
  /*0x9D,     0xFF,   0x00,   0,*/ \
  /*0x9E,     0xFF,   0x00,   0,*/ \
  /*0x9F,     0xFF,   0x0C,   0,*/ \
  /*0xA0,     0xFF,   0x0C,   0,*/ \
}

#endif /* T_QSC60X5REV1_1 - SC2x (Dragon-4/5) codec definitions end here */

/************Ramsis codec mode-(Dragon-4/5) definitions end here.***************************/




/* The following definitions are used for audio override mode */
/* This defines the maximum number of ADIE registers that will get initialized
** for any given configuration.
*/
#define MSMAUD_ADIE_MAX_CONFIG_REGS 30

/* This is the ADIE register that must be modified for audio override mode
*/
#define MSMAUD_ADIE_MIC_AMP1_GAIN_REG        ADIE_CODEC_C1_R

/* This macro performs the necessary modification to the register setting
** for audio override mode. In this case, the micamp1 gain is being set
** to 24dB.
*/
#define MSMAUD_ADIE_ADJUST_MIC_AMP1_GAIN(v)  \
        (v = (v & ~(ADIE_CODEC_C1_MICAMP1_GAIN_L_M)) \
             | (ADIE_CODEC_C1_MICAMP1_GAIN_L_24_DB_V))

/* power down PAs only */
#define MSMAUD_ADIE_PA_POWER_DOWN()                            \
        adie_codec_write(ADIE_RX_PA_ENABLE_REGISTER1_R,        \
                         MSMAUD_ADIE_PA_REG1_ALL_PA_MASK,      \
                         ADIE_RX_PA_ENABLE_REGISTER1_INIT_V);  \
        adie_codec_write(ADIE_RX_PA_ENABLE_REGISTER2_R,        \
                         MSMAUD_ADIE_PA_REG2_ALL_PA_MASK,      \
                         ADIE_RX_PA_ENABLE_REGISTER2_INIT_V)

/* Disconnect Rx Filter from PA */
#define MSMAUD_ADIE_RX_FILTER_DISCONNECT()                     \
        adie_codec_write(ADIE_RX_PA_CONTROL_REGISTER1_R,       \
                         ADIE_RX_PA_CONTROL_REGISTER1_INIT_M,  \
                         ADIE_RX_PA_CONTROL_REGISTER1_INIT_V)

#endif  /* MSMAUD_ADIE_CODEC */

#if defined(FEATURE_BT_AG) && !defined (FEATURE_BT_SOC)
/* Macro to set PCM path to normal operation (ADSP PCM data from AUX) */
#define MSMAUD_SEL_PCM_PATH_NORM()  HWIO_OUTM( PCM_PATH_CTL,               \
                                               HWIO_FMSK(PCM_PATH_CTL,     \
                                                         VDSP_RPCM_SEL), 0 )

/* Macro to set PCM path to BT AG operation (ADSP PCM data from MDSP) */
#define MSMAUD_SEL_PCM_PATH_MDSP()  HWIO_OUTM( PCM_PATH_CTL,              \
                                               HWIO_FMSK( PCM_PATH_CTL,   \
                                                          VDSP_RPCM_SEL ),\
                                               HWIO_FMSK( PCM_PATH_CTL,   \
                                                          VDSP_RPCM_SEL ) )
                                                          
#else /* defined(FEATURE_BT_AG) && !defined (FEATURE_BT_SOC) */

#define MSMAUD_SEL_PCM_PATH_NORM() 
#define MSMAUD_SEL_PCM_PATH_MDSP() 

#endif


/* Workaround for DSP bug (port clock not enabled at reset) */
#define MSM6800_ADSP_POST_IMAGE_DWNLD_SETUP(image) \
  HWIO_OUT(ADSP_CLK_GATE_CTL, 0x3fb);

#ifdef FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL
/*===========================================================================*/
/* MIPs usage numbers that are specific to QDSP_IMAGE_TUNES.                 */
/* MIPs values are multiplied by 10 to eliminate fractions.                  */
/*===========================================================================*/

#define MSMAUD_TUNES_ADSP_CLK_RATE_0 150
#define MSMAUD_TUNES_ADSP_CLK_RATE_1 236
#define MSMAUD_TUNES_ADSP_CLK_RATE_2 382
#define MSMAUD_TUNES_ADSP_CLK_RATE_3 530

/* Indexed by voc_data_pp_interface_enum_type */
#define MSMAUD_QTUNES_MP3_MIPS_USAGE \
  {                                  \
    30,  /* FIR      */              \
    35,  /* IIR      */              \
    21,  /* ADRC     */              \
    84,  /* EQ       */              \
    32,  /* SPA      */              \
    30,  /* Qconcert */              \
    28,  /* STF      */              \
    120  /* SBC      */              \
  }

/* Module to MIPs mapping - needs to be in ascending order of MIPS and
   terminated by QDSP_MODULE_MAX! */
#define MSMAUD_QTUNES_MP3_MODULE_TO_MIPS_MAP              \
  {                                                       \
    QDSP_MODULE_TUNES,     MSMAUD_TUNES_ADSP_CLK_RATE_0,  \
    QDSP_MODULE_TUNES_PP1, MSMAUD_TUNES_ADSP_CLK_RATE_1,  \
    QDSP_MODULE_TUNES_PP2, MSMAUD_TUNES_ADSP_CLK_RATE_2,  \
    QDSP_MODULE_TUNES_PP3, MSMAUD_TUNES_ADSP_CLK_RATE_3,  \
    QDSP_MODULE_MAX,       0                              \
  }

/* Mips usage for the decoder itself, excluding any post-proc. */
#define MSMAUD_QTUNES_MP3_MIPS 150

/*===========================================================================*/
/* MIPs usage numbers that are specific to QDSP_IMAGE_ENH_AAC_PLUS.          */
/* MIPs values are multiplied by 10 to eliminate fractions.                  */
/*===========================================================================*/

#define MSMAUD_AAC_ADSP_CLK_RATE_0 465
#define MSMAUD_AAC_ADSP_CLK_RATE_1 551
#define MSMAUD_AAC_ADSP_CLK_RATE_2 697
#define MSMAUD_AAC_ADSP_CLK_RATE_3 845

/* Indexed by voc_data_pp_interface_enum_type */
#define MSMAUD_ENH_AAC_PLUS_MIPS_USAGE MSMAUD_QTUNES_MP3_MIPS_USAGE

/* Module to MIPs mapping - needs to be in ascending order of MIPS and
   terminated by QDSP_MODULE_MAX! */
#define MSMAUD_AAC_PLUS_ENH_MODULE_TO_MIPS_MAP            \
  {                                                       \
    QDSP_MODULE_AAC,     MSMAUD_AAC_ADSP_CLK_RATE_0,      \
    QDSP_MODULE_AAC_PP1, MSMAUD_AAC_ADSP_CLK_RATE_1,      \
    QDSP_MODULE_AAC_PP2, MSMAUD_AAC_ADSP_CLK_RATE_2,      \
    QDSP_MODULE_AAC_PP3, MSMAUD_AAC_ADSP_CLK_RATE_3,      \
    QDSP_MODULE_MAX,     0                                \
  }

/* Mips usage for the decoder itself, excluding any post-proc. */
#define MSMAUD_ENH_AAC_PLUS_MIPS 465

#endif /* FEATURE_AVS_DYNAMIC_ADSP_CLK_SEL */
#endif /* MSMAUD_H */

