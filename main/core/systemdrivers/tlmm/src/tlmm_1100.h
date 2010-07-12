#ifndef TLMM_1100_H
#define TLMM_1100_H
/*===========================================================================

        T O P  L E V E L  M O D E  M U X   F O R   Q S C 1 1 0 0

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the TLMM functions for the QSC1100.

REFERENCES

Copyright (c) 2006 - 2008  by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/tlmm/tlmm_1100.h#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/09   hvo     Featurized SDCC, Camera as low tier does not support them
11/10/08   hvo     Configured GPIO 34 to input
07/02/07   vph     Ported from QSC6055 for QSC1100.
===========================================================================*/

#include "target.h"
#include "customer.h"
#include "comdef.h"
#include GPIO_H

/* FPGA address for reading version, available on SURF only */
#define TLMM_SURF_FPGA_BASE_ADDR 0x20000022

#define TLMM_QSC62X0_LANDO_DC     0x01  /* Lando daughter card, not support */
#define TLMM_QSC11X0_RICO_DC      0x02  /* Rico daughter card */
#define TLMM_QSC11X0_LIGER_DC     0x03  /* Liger daughter card */
#define TLMM_QSC11X0_TRESTLES_DC  0x04  /* Trestles daughter card */

/* Indicate FFA or SURF hardware */
typedef enum {
    TLMM_SURF,                        /* SURF hw platform */
    TLMM_FFA                          /* FFA hw platform */
} tlmm_hw_platform_type;

extern tlmm_hw_platform_type  tlmm_platform;
extern const GPIO_SignalType  TLMM_GPIO_CONFIG[GPIO_NUM_GPIOS];

/* TLMM configuations */
#if defined (FEATURE_UIM)
    #define TLMM_USES_UIM
#endif

/* --------------------------------------------------------------------------
   Sanity checks
-------------------------------------------------------------------------- */
#if defined(FEATURE_GSBI1_UART1) && defined(FEATURE_GSBI1_SPI)
   #error "Please select only UART1 or SPI on GSBI1\n"
#endif


/* Map GPIO pins to specific pin functions */
/* GPIO  0 */
#define GPIO_0_SIGNAL KYPD_9

/* GPIO  1 */
#define GPIO_1_SIGNAL KYPD_11

/* GPIO  2 */
#define GPIO_2_SIGNAL KYPD_15

/* GPIO  3 */
#define GPIO_3_SIGNAL KYPD_17

/* GPIO  4 */
#define GPIO_4_SIGNAL KYPD_MEMO

/* GPIO  5 */
#define GPIO_5_SIGNAL KEYSENSE_4_N

/* GPIO  6 */
#define GPIO_6_SIGNAL KEYSENSE_3_N

/* GPIO  7 */
#define GPIO_7_SIGNAL KEYSENSE_2_N

/* GPIO  8 */
#define GPIO_8_SIGNAL KEYSENSE_1_N

/* GPIO  9 */
#define GPIO_9_SIGNAL KEYSENSE_0_N

/* GPIO 10 BACKLIGHT_EN */
#define GPIO_10_SIGNAL GP_PDM

/* GPIO 11 */
#define GPIO_11_SIGNAL HEADSET_DET_N

/* GPIO 12 */
#ifdef FEATURE_GSBI0_RUIM
  #define GPIO_12_SIGNAL UIM_CLK
#else
  #define GPIO_12_SIGNAL GSBI0_0
#endif

/* GPIO 13 */
#ifdef FEATURE_BT
 #define GPIO_13_SIGNAL BT_PWR_EN
#else
 #define GPIO_13_SIGNAL GPIO_INPUT_13
#endif

/* GPIO 14 */
#ifdef FEATURE_GSBI0_RUIM
  #define GPIO_14_SIGNAL UIM_RESET
#else
  #define GPIO_14_SIGNAL GSBI0_2
#endif

/* GPIO 15 */
#ifdef FEATURE_GSBI0_RUIM
  #define GPIO_15_SIGNAL UIM_DATA
#else
  #define GPIO_15_SIGNAL GSBI0_3
#endif

/* GPIO 16 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI1_SPI
  #define GPIO_16_SIGNAL SPI_CS_N
#elif defined (FEATURE_GSBI1_UART)
  #define GPIO_16_SIGNAL UART1_RFR_N
#else
  #define GPIO_16_SIGNAL GSBI1_0
#endif

/* GPIO 17 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI1_SPI
  #define GPIO_17_SIGNAL SPI_CLK
#elif defined (FEATURE_GSBI1_UART)
  #define GPIO_17_SIGNAL UART1_CTS_N
#else
  #define GPIO_17_SIGNAL GSBI1_1
#endif

/* GPIO 18 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI1_SPI
  #define GPIO_18_SIGNAL SPI_DATA_I
#elif defined (FEATURE_GSBI1_UART)
  #define GPIO_18_SIGNAL UART1_RX_DAT
#else
  #define GPIO_18_SIGNAL GSBI1_2
#endif

/* GPIO 19 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI1_SPI
  #define GPIO_19_SIGNAL SPI_DATA_O
#elif defined (FEATURE_GSBI1_UART)
  #define GPIO_19_SIGNAL UART1_TX_DAT
#else
  #define GPIO_19_SIGNAL GSBI1_3
#endif

/* GPIO 20 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI2_AUX_AUDIO
  #define GPIO_20_SIGNAL AUX_PCM_CLK
#else
  #define GPIO_20_SIGNAL GSBI2_0
#endif

/* GPIO 21 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI2_AUX_AUDIO
  #define GPIO_21_SIGNAL AUX_PCM_SYNC
#else
  #define GPIO_21_SIGNAL GSBI2_1
#endif

/* GPIO 22 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI2_AUX_AUDIO
  #define GPIO_22_SIGNAL AUX_PCM_DIN
#else
  #define GPIO_22_SIGNAL GSBI2_2
#endif

/* GPIO 23 */ /* GSBI Wrapper */
#ifdef FEATURE_GSBI2_AUX_AUDIO
  #define GPIO_23_SIGNAL AUX_PCM_DOUT
#else
  #define GPIO_23_SIGNAL GSBI2_3
#endif

/* GPIO 24 */
#ifdef FEATURE_DRV_SDCC
#ifndef T_QSC1100
#define GPIO_24_SIGNAL SDCC_CLK
/* GPIO 25 */
#define GPIO_25_SIGNAL SDCC_DAT_0
/* GPIO 26 */
#define GPIO_26_SIGNAL SDCC_DATOUT_1
/* GPIO 27 */
#define GPIO_27_SIGNAL SDCC_DATOUT_2
/* GPIO 28 */
#define GPIO_28_SIGNAL SDCC_DATOUT_3
/* GPIO 29 */
#define GPIO_29_SIGNAL SDCC_CMD
#else
#define GPIO_24_SIGNAL GPIO_SDCC_CLK
/* GPIO 25 */
#define GPIO_25_SIGNAL GPIO_SDCC_DAT_0_OUT
/* GPIO 26 */
#define GPIO_26_SIGNAL GPIO_SDCC_DATOUT_1_OUT
/* GPIO 27 */
#define GPIO_27_SIGNAL GPIO_SDCC_DATOUT_2_OUT
/* GPIO 28 */
#define GPIO_28_SIGNAL GPIO_SDCC_DATOUT_3_OUT
/* GPIO 29 */
#define GPIO_29_SIGNAL GPIO_SDCC_CMD_OUT

#endif
#else
#define GPIO_24_SIGNAL GPIO_INPUT_24
/* GPIO 25 */
#define GPIO_25_SIGNAL GPIO_INPUT_25
/* GPIO 26 */
#define GPIO_26_SIGNAL GPIO_INPUT_26
/* GPIO 27 */
#define GPIO_27_SIGNAL GPIO_INPUT_27
/* GPIO 28 */
#define GPIO_28_SIGNAL GPIO_INPUT_28
/* GPIO 29 */
#define GPIO_29_SIGNAL GPIO_INPUT_29
#endif

/* GPIO 30 */
#define GPIO_30_SIGNAL XO_EN_N

/* GPIO 31 */
#if defined(FEATURE_PROJECT_W021) || defined(FEATURE_PROJECT_W022)
#define GPIO_31_SIGNAL GPIO_OUTPUT_31
#else
#define GPIO_31_SIGNAL SIM_SEL
#endif
/* GPIO 32 */

#if defined(FEATURE_PROJECT_W203) || defined(FEATURE_PROJECT_W204)
#define GPIO_32_SIGNAL GPIO_OUTPUT_32
#else
#define GPIO_32_SIGNAL LCD_FLAG
#endif

#if defined(FEATURE_PROJECT_W203) || defined(FEATURE_PROJECT_W204)
/* GPIO 33, we need to feature it, as we don't use it if Mem size is < 16 MB */
#define GPIO_33_SIGNAL KYPD_EX1

/* GPIO 34 */
#define GPIO_34_SIGNAL KYPD_EX2
#else
/* GPIO 33, we need to feature it, as we don't use it if Mem size is < 16 MB */
#define GPIO_33_SIGNAL GPIO_INPUT_33

/* GPIO 34 */
#define GPIO_34_SIGNAL SIM_SEL
#endif
/* GPIO 35 */
#define GPIO_35_SIGNAL PA_ON_0

/* GPIO 36 */
#define GPIO_36_SIGNAL PA_ON_1

/* GPIO 37 */
#define GPIO_37_SIGNAL PA_R0

/* GPIO 38 */ 
#define GPIO_38_SIGNAL PA_ON_2

/* GPIO 39 */ /* I2C vs SPI - Camera, no SPI */
#if defined(TLMM_USES_SPI)
  #define GPIO_39_SIGNAL SPI_CS3_N
#else
  #define GPIO_39_SIGNAL I2C_SDA
#endif

/* GPIO 40 */ /* I2C vs SPI - Camera, no SPI */
#if defined(TLMM_USES_SPI)
  #define GPIO_40_SIGNAL SPI_CS2_N
#else
  #define GPIO_40_SIGNAL I2C_SCL
#endif

/* GPIO 41 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_41_SIGNAL CAMIF_DISABLE
#else
  #define GPIO_41_SIGNAL GPIO_INPUT_41
#endif

/* GPIO 42 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_42_SIGNAL CAMIF_DATA_0
#else
  #define GPIO_42_SIGNAL GPIO_INPUT_42
#endif

/* GPIO 43 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_43_SIGNAL CAMIF_DATA_1
#else
  #define GPIO_43_SIGNAL GPIO_INPUT_43
#endif

/* GPIO 44 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_44_SIGNAL CAMIF_DATA_2
#else
  #define GPIO_44_SIGNAL GPIO_INPUT_44
#endif

/* GPIO 45 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_45_SIGNAL CAMIF_DATA_3
#else
  #define GPIO_45_SIGNAL GPIO_INPUT_45
#endif

/* GPIO 46 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_46_SIGNAL CAMIF_DATA_4
#else
  #define GPIO_46_SIGNAL GPIO_INPUT_46
#endif

/* GPIO 47 */
#if defined(FEATURE_PROJECT_W203) || defined(FEATURE_PROJECT_W204)
#define GPIO_47_SIGNAL GPIO_OUTPUT_47
#else
#if defined(TLMM_USES_CAMIF)
  #define GPIO_47_SIGNAL CAMIF_DATA_5
#else
  #define GPIO_47_SIGNAL GPIO_INPUT_47
#endif
#endif
/* GPIO 48 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_48_SIGNAL CAMIF_DATA_6
#else
  #define GPIO_48_SIGNAL GPIO_INPUT_48
#endif

/* GPIO 49 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_49_SIGNAL CAMIF_DATA_7
#else
  #define GPIO_49_SIGNAL GPIO_INPUT_49
#endif

/* GPIO 50 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_50_SIGNAL CAMIF_PCLK
#else
  #define GPIO_50_SIGNAL GPIO_INPUT_50
#endif

/* GPIO 51 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_51_SIGNAL CAMIF_HSYNC
#else
  #define GPIO_51_SIGNAL GPIO_INPUT_51
#endif

/* GPIO 52 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_52_SIGNAL CAMIF_VSYNC
#else
  #define GPIO_52_SIGNAL GPIO_INPUT_52
#endif

/* GPIO 53 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_53_SIGNAL CAMIF_MCLK
#else
  #define GPIO_53_SIGNAL GPIO_INPUT_53
#endif

/* GPIO 54 */
#if defined(TLMM_USES_CAMIF)
  #define GPIO_54_SIGNAL MDP_VSYNC_P
#else
  #define GPIO_54_SIGNAL KYPD_EX3
#endif

/* GPIO 55 */
#define GPIO_55_SIGNAL EBI1_A_D_16

/* GPIO 56 */
#define GPIO_56_SIGNAL EBI1_A_D_17

/* GPIO 57 */
#define GPIO_57_SIGNAL EBI1_A_D_18

/* GPIO 58 */
#define GPIO_58_SIGNAL EBI1_A_D_19

/* GPIO 59 */
#define GPIO_59_SIGNAL EBI1_A_D_20

/* GPIO 60 */
#define GPIO_60_SIGNAL EBI1_A_D_21

/* GPIO 61 */
#define GPIO_61_SIGNAL EBI1_A_D_22

/* GPIO 62 */
#define GPIO_62_SIGNAL EBI1_A_D_23

#endif /* TLMM_1100_H */
