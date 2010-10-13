#ifndef CLKRGM_MODEM_H
#define CLKRGM_MODEM_H

/*=========================================================================

              M O D E M   C L O C K   D E V I C E   D R I V E R
               
         E X T E R N A L   I N T E R F A C E   D E F I N I T I O N S

GENERAL DESCRIPTION
  This file contains the external interface definitions for the Modem Clock
  Device Driver.  The modem clocks part of the Clock Device Driver is
  responsible for controlling the modem core clock blocks, and the mDSP clock
  blocks.

EXTERNALIZED FUNCTIONS
  clk_regime_modem_init
  clk_regime_reset
  clk_regime_on
  clk_regime_off
  clk_regime_is_on
  clk_regime_enable
  clk_regime_disable
  clk_regime_misc_enable
  clk_regime_misc_disable
  clk_regime_sel_clk_inv
  clk_regime_sel_clk_src
  clk_regime_sel_clk_div
  clk_regime_config_cdma_clocks
  clk_regime_config_gps_clocks
  clk_regime_agc_src_on
  clk_regime_set_agc_src_on
  clk_regime_setup_gsm_clk
  clk_regime_lcg_on
  clk_regime_lcg_off
  clk_regime_switch_mdsp_clk
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Invoke the clk_regime_modem_init function to initialize the Modem Clock
  Device Driver.

      Copyright (c) 2004 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2005 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2006 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
      Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.

==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
 $Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_modem.h#12 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
01/20/09   hvo     Featurized the Modem Turbo Decode, only available for mid
                   and high tiers      
06/30/08   hvo     Changed CX8, CX16 clock sources value for FEE
10/11/07    bc     Initial port form QSC6055v3202.
==========================================================================*/ 

#ifndef _ARM_ASM_

#ifndef COMDEF_H
  #include "comdef.h"
#endif

/*=========================================================================
      Constants & Macros & Typedefs
==========================================================================*/

/*--------------------------------------------------------------------------
  MODEM CLOCKS
---------------------------------------------------------------------------*/
#define CLKRGM_MODEM_CLK_OFFSET 128

typedef enum
{
  CLKRGM_CHIPXN_CLK = CLKRGM_MODEM_CLK_OFFSET,
  CLKRGM_CHIPXN_DIV2_CLK,
  CLKRGM_CHIPXN_DIV4_CLK,
  CLKRGM_RXF_SAMP_CLK,
  CLKRGM_RXF_SAMP_REF_CLK,
  #ifdef CLKRGM_INCLUDE_GSM
  CLKRGM_EDGE_TX_CLK,
  CLKRGM_EDGE_RX_CLK,
  CLKRGM_GSM_CLK,
  CLKRGM_GSAC_CLK,
  #endif
  CLKRGM_TX_PDM_CLK,
  CLKRGM_TCXO_PDM_CLK,
  CLKRGM_MDSP_CLK,
  CLKRGM_SLEEP_CLK,
  CLKRGM_SLPFAST_CHIPXN_REF1_CLK,
  CLKRGM_SLPFAST_CHIPXN_REF2_CLK,
  #ifdef CLKRGM_INCLUDE_GSM
  CLKRGM_SLPFAST_GSM_CLK,
  #endif
  CLKRGM_SLPFAST_FEE_CLK,
  CLKRGM_BT_CLK,
  CLKRGM_MODEM_SBI_CLK,
  CLKRGM_MODEM_WEB_CLK,
  CLKRGM_OFFLINE_CLK,
  #ifdef CLKRGM_INCLUDE_GPS
  CLKRGM_GACC_CLK,
  CLKRGM_GPS_CHIPX8_CLK,
  #endif
  #ifdef CLKRGM_INCLUDE_WCDMA
  CLKRGM_F9_CLK,
  #endif
  #ifdef CLKRGM_INCLUDE_GPS
  CLKRGM_MMGPS_CLK,
  CLKRGM_MMGPS_ARM_CLK,
  #endif
  CLKRGM_TX_DAC_CLK,
  CLKRGM_PC_DAC_CLK,
  CLKRGM_CDXO_Y_CUT_CLK,
  CLKRGM_AT_CUT_CLK,
  CLKRGM_SAMP_FIFO_CLK1,
  CLKRGM_SAMP_FIFO_CLK0,
  CLKRGM_SAMP_REF_CLK,
  #ifdef CLKRGM_INCLUDE_GPS
  CLKRGM_GPS_CHIPX32_CLK,
  CLKRGM_GPS_CHIPX16_CLK,
  #endif
  TX_RAM_CLK,
  SSRV_OFFLINE_CLK1,
  SSRV_OFFLINE_CLK0,
  SYM_BUFF_CLK,
  CC_RAM_CLK,
  #ifdef CLKRGM_INCLUDE_WCDMA
  WCDMA_SLPFAST_CHIPXN_REF_CLK,
  WCDMA_CHIPXN_CLK,
  WCDMA_CHIPXN_DIV2_CLK,
  WCDMA_CHIPXN_DIV4_CLK,
  #endif
  BT_SBI_CLK,
  CLKRGM_MDM_CLK,

  /* Grouped clocks for backward compatibility */
  CLKRGM_DEC_CLK,
  #ifdef CLKRGM_INCLUDE_HDR
  CLKRGM_HDR_TDEC_CLK,
  #endif
  CLKRGM_TURBO_DEC_CLK,
  CLKRGM_SVD_CLK,
  
  CLKRGM_CHIPX8_CLK,
  CLKRGM_CDMA_TX_CLK,
  CLKRGM_CDMA_RX_CLK,
  CLKRGM_RXCHIPX8_CLK,
  
  CLKRGM_ANT0_SAMP_CLK,
  CLKRGM_ANT1_SAMP_CLK,
  CLKRGM_FB_ANT0_SAMP_CLK,//CLKRGM_ANT0_SAMP_CLK
  CLKRGM_FB_ANT1_SAMP_CLK,
  CLKRGM_ANT_SAMP_CHIPXN_CLK,
  CLKRGM_SLPFAST_FEE_CHIPXN_CLK,

  /* SW control of HW gated clocks when HW gating is disabled */
  CLKRGM_DEM1X_OFFLINE_CLK,      /* demod offline clock               */
  CLKRGM_DEM1X_FFE_RESULT_CLK,   /* demod FFE results                 */
  CLKRGM_DEM1X_FFE_ENGINE_CLK,   /* demod FFE engine                  */
  CLKRGM_DEM1X_FFE_CONFIG_CLK,   /* demod FFE config                  */
  CLKRGM_DEM1X_CX8_CLK,          /* demod chipx8                      */
  CLKRGM_DEINT_MICRO_CLK,        /* deint block                       */
  CLKRGM_DEINT_MICRO_TASK_RD_CLK,/* deint task                        */
  CLKRGM_DEINT_MICRO_TD_RD_CLK,  /* turbo decoder read                */
  CLKRGM_DEINT_MICRO_VD_RD_CLK,  /* SVD read                          */
  CLKRGM_FL_SYM_BUFF_CLK,        /* forward link symbol buffer        */
  CLKRGM_MOD_CX16_CLK,           /* modulator chipx16                 */
  CLKRGM_MOD_CX8_CLK,            /* modulator chipx8                  */
  CLKRGM_MOD_PDM_CLK,            /* modulator PDM                     */
  CLKRGM_DECOB_MICRO_CLK,        /* decoder output buffer             */
  CLKRGM_SAMPSRV_CLK,            /* sample server                     */
  #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
  CLKRGM_TD_MICRO_CLK,           /* turbo decoder                     */
  #endif
  CLKRGM_VD_MICRO_CLK,           /* SVD                               */
  #ifdef CLKRGM_INCLUDE_GPS
  CLKRGM_GACC_MICRO_SW_CLK,      /* GPS accelerator                   */
  CLKRGM_GACC_MICRO_CLK,
  #endif

  #ifdef CLKRGM_INCLUDE_HDR
  CLKRGM_CDMA_RX_HDR_CLK,
  CLKRGM_CDMA_TX_HDR_CLK,
  #endif

  /* All modem clocks */
  CLKRGM_MODEM_ALL_CLK,

  /* Obsolete clock for backward compatibility */
  CLKRGM_OBSOLETE_CLK,

  CLKRGM_MODEM_NUM_OF_CLOCKS
} clkrgm_modem_clk_type;

/* Mapping of clocks for backward compatibility with existing code */
#define CLKRGM_RXF0_FRONT_CLK             CLKRGM_CHIPXN_DIV2_CLK
#define CLKRGM_RXF1_FRONT_CLK             CLKRGM_CHIPXN_DIV2_CLK
#define CLKRGM_RXF0_SAMPCTRL_CLK          CLKRGM_CHIPXN_DIV4_CLK
#define CLKRGM_RXF1_SAMPCTRL_CLK          CLKRGM_CHIPXN_DIV4_CLK
#define CLKRGM_RXF0_SAMP_CLK                CLKRGM_RXF_SAMP_CLK
#define CLKRGM_RXF1_SAMP_CLK                CLKRGM_RXF_SAMP_CLK
#define CLK_RGM_RXFRONT_M                   CLKRGM_RXF0_FRONT_CLK
#define CLK_RGM_RXFRONT0_M                  CLKRGM_RXF0_FRONT_CLK
#define CLK_RGM_RXFRONT1_M                  CLKRGM_RXF1_FRONT_CLK
#define CLK_RGM_CHIPX8_M                    CLKRGM_CHIPX8_CLK
#define CLK_RGM_CDMA_TX_M                   CLKRGM_CDMA_TX_CLK
#define CLK_RGM_CDMA_RX_M                   CLKRGM_CDMA_RX_CLK
#define CLK_RGM_TXCHIPX8_M                  CLKRGM_CDMA_TX_CLK
#define CLK_RGM_RXCHIPX8_M                  CLKRGM_RXCHIPX8_CLK
#define CLK_RGM_DEC_M                       CLKRGM_DEC_CLK
#define CLK_RGM_OFFLINE_M                   CLKRGM_OFFLINE_CLK
#define CLK_RGM_BT_SLP_M                    CLKRGM_BT_CLK
#define CLK_RGM_BT_M                        CLKRGM_BT_CLK
#ifdef CLKRGM_INCLUDE_HDR
#define CLK_RGM_HDR_TDEC_M                  CLKRGM_HDR_TDEC_CLK
#endif
#define CLK_RGM_CDMA_PDM_M                  CLKRGM_TCXO_PDM_CLK
#ifdef CLKRGM_INCLUDE_GSM
#define CLK_RGM_GSM_M                       CLKRGM_GSM_CLK
#define CLK_RGM_GSM_GSAC_M                  CLKRGM_GSAC_CLK
#endif
#define CLK_RGM_TURBO_DEC_M                 CLKRGM_TURBO_DEC_CLK
#define CLK_RGM_SVD_M                       CLKRGM_SVD_CLK
#define CLK_RGM_SLPCTL_CDMA_M               CLKRGM_SLPFAST_CHIPXN_REF1_CLK
#ifdef CLKRGM_INCLUDE_GSM
#define CLK_RGM_SLPCTL_GSM_M                CLKRGM_SLPFAST_GSM_CLK
#endif
#ifdef CLKRGM_INCLUDE_HDR
#define CLK_RGM_SLPCTL_HDR_M                CLKRGM_SLPFAST_CHIPXN_REF1_CLK
#endif
#define CLK_RGM_SLPFAST_CDMA_M              CLKRGM_SLPFAST_CHIPXN_REF1_CLK
#ifdef CLKRGM_INCLUDE_GSM
#define CLK_RGM_SLPFAST_GSM_M               CLKRGM_SLPFAST_GSM_CLK
#endif
#ifdef CLKRGM_INCLUDE_HDR
#define CLK_RGM_SLPFAST_HDR_M               CLKRGM_SLPFAST_CHIPXN_REF1_CLK
#endif
#define CLK_RGM_SLPFAST_FEE_M               CLKRGM_SLPFAST_FEE_CLK
#ifdef CLKRGM_INCLUDE_GPS
#define CLK_RGM_RTC_GPS_M                   CLKRGM_GPS_CHIPX8_CLK
#endif
#ifdef CLKRGM_INCLUDE_HDR
#define CLK_RGM_HDR_PDM_M                   CLKRGM_TCXO_PDM_CLK
#endif
#define CLK_RGM_MODEM_M                     CLKRGM_MDSP_CLK
#define CLK_RGM_MDSP_M                      CLKRGM_MDSP_CLK

#define CLK_RGM_CDMA_RX_1X_M                CLKRGM_RXCHIPX8_CLK
#ifdef CLKRGM_INCLUDE_HDR
#define CLK_RGM_CDMA_RX_HDR_M               CLKRGM_CDMA_RX_HDR_CLK
#endif
#define CLK_RGM_CDMA_TX_1X_M                CLKRGM_CDMA_TX_CLK
#ifdef CLKRGM_INCLUDE_HDR
#define CLK_RGM_CDMA_TX_HDR_M               CLKRGM_CDMA_TX_HDR_CLK
#endif


#ifdef CLKRGM_INCLUDE_HDR
#define CLK_RGM_HDR_DEMOD_M                 CLKRGM_OBSOLETE_CLK
#endif
#define CLK_RGM_ANALOG0_M                   CLKRGM_OBSOLETE_CLK
#define CLK_RGM_ANALOG1_M                   CLKRGM_OBSOLETE_CLK
#define CLK_RGM_MISC_ENA_CH0_ADIE_SE_EN     CLKRGM_OBSOLETE_CLK
#define CLK_RGM_MISC_ENA_CH1_ADIE_SE_EN     CLKRGM_OBSOLETE_CLK
#define CLK_RGM_MISC_ENA_CH0_ADIE_DIFF_SEL  CLKRGM_OBSOLETE_CLK
#define CLK_RGM_MISC_ENA_CH1_ADIE_DIFF_SEL  CLKRGM_OBSOLETE_CLK
#define CLK_RGM_DFM_M                       CLKRGM_OBSOLETE_CLK
#define CLK_RGM_CDMA_AGC_M                  CLKRGM_OBSOLETE_CLK
#define CLK_RGM_MISC_ENA_TURBO_DEC_SLEEP    CLKRGM_OBSOLETE_CLK
#define CLK_RGM_MISC_ENA_TXDAC_SLEEP        CLKRGM_OBSOLETE_CLK

/*--------------------------------------------------------------------------
  CLOCK SOURCE SELECTION VALUES
---------------------------------------------------------------------------*/
#ifndef T_QSC1100
/* CDMA sleep controller clock source selections. */
#define CLK_RGM_SRC_SEL_SLPCTL_CDMA_TCXODIV512_V        0
#define CLK_RGM_SRC_SEL_SLPCTL_CDMA_SLPXTAL_V           1
#endif

#ifdef CLKRGM_INCLUDE_GSM
/* GSM sleep controller clock source selections. */
#define CLK_RGM_SRC_SEL_SLPCTL_GSM_TCXODIV512_V         0
#define CLK_RGM_SRC_SEL_SLPCTL_GSM_SLPXTAL_V            1
#endif

#ifdef CLKRGM_INCLUDE_HDR
/* HDR sleep controller clock source selections. */
#define CLK_RGM_SRC_SEL_SLPCTL_HDR_TCXODIV512_V         0
#define CLK_RGM_SRC_SEL_SLPCTL_HDR_SLPXTAL_V            1
#endif

#ifdef CLKRGM_INCLUDE_GSM
/* GSM GSAC clock source selections. */
#define CLK_RGM_SRC_SEL_GSM_GSAC_MDSP_CLK_V             0
#define CLK_RGM_SRC_SEL_GSM_GSAC_ADSP_CLK_V             1
#define CLK_RGM_SRC_SEL_GSM_GSAC_CX16_CLK_V             2
#endif

/* Sleep XTAL frequency error estimator clock source selections. */
#define CLK_RGM_SRC_SEL_SLPFAST_FEE_CX16_CLK_V          6
#define CLK_RGM_SRC_SEL_SLPFAST_FEE_CX8_CLK_V           5
#ifdef CLKRGM_INCLUDE_WCDMA
#define CLK_RGM_SRC_SEL_SLPFAST_FEE_UMTS_CX16_CLK_V     2
#define CLK_RGM_SRC_SEL_SLPFAST_FEE_UMTS_CX8_CLK_V      3
#endif
#ifdef CLKRGM_INCLUDE_GSM
#define CLK_RGM_SRC_SEL_SLPFAST_FEE_GSM_CLK_V           4
#endif

/* SBI clock source selections. */
#define CLK_RGM_SRC_SEL_SBI_SRC_TCXO_V                  HWIO_FVAL(MODEM_CLK_MISC_CTL,SBI_CLK_SRC_SEL,TCXO)
#define CLK_RGM_SRC_SEL_SBI_SRC_SLPXTAL_V               HWIO_FVAL(MODEM_CLK_MISC_CTL,SBI_CLK_SRC_SEL,SLEEP_XTAL)

/* CHIPXN source selection */
#define CLK_RGM_SRC_SEL_CHIPXN_CHIPXN_REV_DIV1_V        0
#define CLK_RGM_SRC_SEL_CHIPXN_RX_SAMPLE_CLK_RAW_V      1

/* RX_SAMPLE_RAW source selection */
#define CLK_RGM_SRC_SEL_RX_SAMPLE_RAW_ANT0_FB_V         0
#define CLK_RGM_SRC_SEL_RX_SAMPLE_RAW_ANT1_FB_V         1

/* TX_DAC_CHIPXN source selection */
#define CLK_RGM_SRC_SEL_TX_DAC_CHIPXN_DIV1_V            0
#define CLK_RGM_SRC_SEL_TX_DAC_CHIPXN_DIV2_V            1

/* TX_PDM source selection */
#define CLK_RGM_SRC_SEL_TX_PDM_TCXO_V                   0
#define CLK_RGM_SRC_SEL_TX_PDM_CHIPXN_DIV1_V            1
#define CLK_RGM_SRC_SEL_TX_PDM_CHIPXN_DIV2_V            2
#define CLK_RGM_SRC_SEL_TX_PDM_CHIPXN_DIV4_V            3

/* TCXO_PDM source selection */
#define CLK_RGM_SRC_SEL_TCXO_PDM_TCXO_V                 0
#define CLK_RGM_SRC_SEL_TCXO_PDM_CHIPXN_DIV1_V          1
#define CLK_RGM_SRC_SEL_TCXO_PDM_CHIPXN_DIV2_V          2
#define CLK_RGM_SRC_SEL_TCXO_PDM_CHIPXN_DIV4_V          3

/* Receive chain 0 receiver sample clock source selections. */
#define CLK_RGM_SRC_SEL_CH0_RXSAMPLE_CXN_DIV1_V         0
#define CLK_RGM_SRC_SEL_CH0_RXSAMPLE_CXN_DIV2_V         1
#define CLK_RGM_SRC_SEL_CH0_RXSAMPLE_CXN_DIV4_V         3

/* Receive chain 0 receiver front end clock source selections. */
#define CLK_RGM_SRC_SEL_CH0_RXFRONT_CXN_DIV1_V          0
#define CLK_RGM_SRC_SEL_CH0_RXFRONT_CXN_DIV2_V          1
#define CLK_RGM_SRC_SEL_CH0_RXFRONT_CXN_DIV4_V          3

/* Receive chain 0 sample controller clock source selections. */
#define CLK_RGM_SRC_SEL_CH0_SAMPCTRL_CXN_DIV2_V         0
#define CLK_RGM_SRC_SEL_CH0_SAMPCTRL_CXN_DIV4_V         1

/* Receive chain 0 mode selections. */
#define CLK_RGM_CH0_CDMA_MODE_V                         0
#define CLK_RGM_CH0_GPS_MODE_V                          1

#if defined(T_QSC6055)
#define CLK_RGM_SRC_SEL_RX_SAMPLE_CLK_ANT0_SAMP_CLK_V   1
#define CLK_RGM_SRC_SEL_RX_SAMPLE_CLK_ANT1_SAMP_CLK_V   2
#define CLK_RGM_SRC_RXF_SAMP_REF_CLK_GPLL0_V            6
#define CLK_RGM_SRC_RXF_SAMP_REF_CLK_GPLL0_DIV1_V       0
#define CLK_RGM_SRC_RXF_SAMP_REF_CLK_GPLL0_DIV4_V       3
#define CLK_RGM_SRC_SEL_RX_SAMPLE_CLK_EXTERNAL_SRC      3
#define CLK_RGM_SRC_SEL_RX_SAMPLE_CLK_TCXO_V            0

#if defined  FEATURE_PLL_192
#define CLK_RGM_MODEM_RXF_SAMP_REF_CLK_CTL_V            6
#elif defined FEATURE_PLL_384
#define CLK_RGM_MODEM_RXF_SAMP_REF_CLK_CTL_V            0x16
#endif
#endif

#ifdef T_MSM6800B

#define CLK_RGM_SRC_SEL_ANT0_CLK_DSP_CTL_ARM_V          0
#define CLK_RGM_SRC_SEL_ANT0_CLK_DSP_CTL_DSP_V          1
#define CLK_RGM_SRC_SEL_ANT1_CLK_DSP_CTL_ARM_V          0
#define CLK_RGM_SRC_SEL_ANT1_CLK_DSP_CTL_DSP_V          1

#define CLK_RGM_DIV_SEL_CHIPXN_CLK_DIV_CHIPX16_DIV2_V   0
#define CLK_RGM_DIV_SEL_CHIPXN_CLK_DIV_CHIPX16_DIV1_V   1

#define CLK_RGM_DIV_SEL_BT_SBI_CLK_DIV1_V               0
#define CLK_RGM_DIV_SEL_BT_SBI_CLK_DIV2_V               1
#define CLK_RGM_DIV_SEL_BT_SBI_CLK_DIV3_V               2
#define CLK_RGM_DIV_SEL_BT_SBI_CLK_DIV4_V               3

#define CLK_RGM_SRC_SEL_BT_SBI_CLK_SRC_TCXO1_V          0
#define CLK_RGM_SRC_SEL_BT_SBI_CLK_SRC_TCXO2_V          1
#define CLK_RGM_SRC_SEL_BT_SBI_CLK_SRC_EXT_CLK0_V       2
#define CLK_RGM_SRC_SEL_BT_SBI_CLK_SRC_EXT_CLK1_V       3

#define CLK_RGM_SRC_SEL_BT_CLK_SRC_BT_CLK_V             0
#define CLK_RGM_SRC_SEL_BT_CLK_SRC_EXT_CLK0_V           1
#define CLK_RGM_SRC_SEL_BT_CLK_SRC_EXT_CLK1_V           2
#define CLK_RGM_SRC_SEL_BT_CLK_SRC_GND_V                3

#endif

/* Receive chain 0 receiver analog clock source selections. */
#define CLK_RGM_SRC_SEL_CH0_ANALOG_CXN_DIV_V            0
#define CLK_RGM_SRC_SEL_CH0_ANALOG_GSM_V                3
#define CLK_RGM_SRC_SEL_CH0_ANALOG_GPS_ADIE_V           5
#define CLK_RGM_SRC_SEL_CH0_ANALOG_CM_CHIPX8_V          2
#define CLK_RGM_SRC_SEL_CH0_ANALOG_CM_CHIPX16_V         1
#define CLK_RGM_SRC_SEL_CH0_ANALOG_EDGE_DIV2_V          6
#define CLK_RGM_SRC_SEL_CH0_ANALOG_GND_V                4
#define CLK_RGM_SRC_SEL_CH0_ANALOG_VDD_V                7

#define CLK_RGM_DIV_SEL_ANALOG_CXN_DIV1_V               0
#define CLK_RGM_DIV_SEL_ANALOG_CXN_DIV2_V               1
#define CLK_RGM_DIV_SEL_ANALOG_CXN_DIV3_V               2
#define CLK_RGM_DIV_SEL_ANALOG_CXN_DIV4_V               3

#define CLK_RGM_SRC_SEL_GPS_ANT0_SAMPLE_CLK_FB_V        0

#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_ANT_CDMA_CHIPXN_DIV_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
      ANT_CDMA_CHIPXN_DIV_CLK)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_CDMA_CHIPX16_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
      CM_CDMA_CHIPX16)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_CDMA_CHIPX8_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
      CM_CDMA_CHIPX8)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_GSM_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
      GSM_CLK)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_GROUND_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
      GROUND)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_GPS_ANT_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
      GPS_ANT_CLK)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_EDGE_DIV2_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
      EDGE_DIV2_CLK)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_VDD_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
     VDD)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_ANT_WCDMA_CHIPXN_DIV_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
    ANT_WCDMA_CHIPXN_DIV_CLK)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_WCDMA_CHIPX16_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
    CM_WCDMA_CHIPX16)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_WCDMA_CHIPX8_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
    CM_WCDMA_CHIPX8)
#define CLK_RGM_SRC_SEL_ANT_SAMP_CLK_GPS_CHIPX16_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, \
    GPS_CHIPX16_CLK)

#define CLK_RGM_SRC_SEL_SAMPLE_FIFO_CLK_SRC_CDMA_CHIPXN_DIV2_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, SAMPLE_FIFO_CLK_SRC_SEL0_, \
    CDMA_CHIPXN_DIV2)
#define CLK_RGM_SRC_SEL_SAMPLE_FIFO_CLK_SRC_WCDMA_CHIPXN_DIV2_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, SAMPLE_FIFO_CLK_SRC_SEL0, \
    WCDMA_CHIPXN_DIV2)
#define CLK_RGM_SRC_SEL_SAMPLE_FIFO_CLK_SRC_GPS_CHIPX16_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, SAMPLE_FIFO_CLK_SRC_SEL0, \
    GPS_CHIPX16_CLK_V)
#define CLK_RGM_SRC_SEL_SAMPLE_FIFO_CLK_SRC_GPS_CHIPX32_CLK_V \
  HWIO_FVAL(MODEM_CLK_MISC_CTL_2, SAMPLE_FIFO_CLK_SRC_SEL0, \
    GPS_CHIPX32_CLK)

#define CLK_RGM_SRC_SEL_GPS_INTERNAL_CLK_V              0
#define CLK_RGM_SRC_SEL_GPS_FB_CLK_V                    1

/* Receive chain 1 receiver sample clock source selections. */
#define CLK_RGM_SRC_SEL_CH1_RXSAMPLE_CXN_DIV1_V         0
#define CLK_RGM_SRC_SEL_CH1_RXSAMPLE_CXN_DIV2_V         1
#define CLK_RGM_SRC_SEL_CH1_RXSAMPLE_CXN_DIV4_V         3

/* Receive chain 1 receiver front end clock source selections. */
#define CLK_RGM_SRC_SEL_CH1_RXFRONT_CXN_DIV1_V          0
#define CLK_RGM_SRC_SEL_CH1_RXFRONT_CXN_DIV2_V          1
#define CLK_RGM_SRC_SEL_CH1_RXFRONT_CXN_DIV4_V          3

/* Receive chain 1 sample controller clock source selections. */
#define CLK_RGM_SRC_SEL_CH1_SAMPCTRL_CXN_DIV2_V         0
#define CLK_RGM_SRC_SEL_CH1_SAMPCTRL_CXN_DIV4_V         1

/* Receive chain 1 mode selections. */
#define CLK_RGM_CH1_CDMA_MODE_V                         0
#define CLK_RGM_CH1_GPS_MODE_V                          1

/* Receive chain 1 receiver analog clock source selections. */
#ifndef T_MSM6800B
#define CLK_RGM_SRC_SEL_CH1_ANALOG_CXN_DIV_V            0
#define CLK_RGM_SRC_SEL_CH1_ANALOG_GSM_V                1
#define CLK_RGM_SRC_SEL_CH1_ANALOG_MMGPS_ADIE_V         2
#define CLK_RGM_SRC_SEL_CH1_ANALOG_GPS_ADIE_V           2
#define CLK_RGM_SRC_SEL_CH1_ANALOG_MDSP_V               3
#else
#define CLK_RGM_SRC_SEL_CH1_ANALOG_CXN_DIV_V            0
#define CLK_RGM_SRC_SEL_CH1_ANALOG_GSM_V                3
#define CLK_RGM_SRC_SEL_CH1_ANALOG_GPS_ADIE_V           5
#define CLK_RGM_SRC_SEL_CH1_ANALOG_CM_CHIPX8_V          2
#define CLK_RGM_SRC_SEL_CH1_ANALOG_CM_CHIPX16_V         1
#define CLK_RGM_SRC_SEL_CH1_ANALOG_EDGE_DIV2_V          6
#define CLK_RGM_SRC_SEL_CH1_ANALOG_GND_V                4
#define CLK_RGM_SRC_SEL_CH1_ANALOG_VDD_V                7
#endif

#define CLK_RGM_SRC_SEL_GPS_ANT1_SAMPLE_CLK_FB_V        1      

#ifdef T_MSM6800B
  #define CLK_RGM_SRC_SEL_CHIPXN_PLL_DIV5_V               0x09
  #define CLK_RGM_SRC_SEL_CHIPXN_PLL_DIV10_V              0x0D
#else
  #define CLK_RGM_SRC_SEL_CHIPXN_PLL_DIV5_V               0x0A
  #define CLK_RGM_SRC_SEL_CHIPXN_PLL_DIV10_V              0x0E
#endif

/* Receive chain 0 receive sample clock divider values. */
#define CLK_RGM_DIV_SEL_CH0_RXSAMPLE_DIV1_V             0
#define CLK_RGM_DIV_SEL_CH0_RXSAMPLE_DIV2_V             1

/* Receive chain 1 receive sample clock divider values. */
#define CLK_RGM_DIV_SEL_CH1_RXSAMPLE_DIV1_V             0
#define CLK_RGM_DIV_SEL_CH1_RXSAMPLE_DIV2_V             1

/*--------------------------------------------------------------------------
  mDSP CLOCK CONFIGURATION
---------------------------------------------------------------------------*/

/* mDSP clock speeds */
typedef enum
{
  CLKRGM_MDSP_TCXO = 0,
  CLKRGM_MDSP_64MHZ,
  #define CLKRGM_MDSP_NUM_SPEEDS (CLKRGM_MDSP_64MHZ+1) /* for klocwork */
} clkrgm_mdsp_clk_frequency_type;

#define CLKRGM_MDSP_19P2MHZ_SPEED CLKRGM_MDSP_TCXO
#define CLKRGM_MDSP_64MHZ_SPEED   CLKRGM_MDSP_64MHZ

#define CLKRGM_MDSP_SPEED_LOW    CLKRGM_MDSP_TCXO
#define CLKRGM_MDSP_SPEED_MID    CLKRGM_MDSP_64MHZ
#define CLKRGM_MDSP_SPEED_HIGH   CLKRGM_MDSP_64MHZ
#define CLKRGM_MDSP_SPEED_MAX    CLKRGM_MDSP_64MHZ
#define CLKRGM_MDSP_SPEED_NOMINAL CLKRGM_MDSP_SPEED_LOW

/*--------------------------------------------------------------------------
  RXCHAIN DEFINITION
---------------------------------------------------------------------------*/

typedef enum
{
  CLKRGM_RX_CHAIN_0,
  CLKRGM_RX_CHAIN_1,
  CLKRGM_RX_CHAIN_BOTH
} clkrgm_rx_chain_type;

/*--------------------------------------------------------------------------
  GPS SYNCHRONIZATION MODE
---------------------------------------------------------------------------*/

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/*--------------------------------------------------------------------------
  DYNAMIC HW CLOCK GATING CONTROLS
---------------------------------------------------------------------------*/

typedef enum
{
  CLKRGM_LCG_DEM1X_FFE_RESULT = 0x0,
  CLKRGM_LCG_DEM1X_FFE_ENGINE,
  CLKRGM_LCG_DEM1X_FFE_CONFIG,
  CLKRGM_LCG_DEINT_TASK,
  CLKRGM_LCG_DEINT_TD_RD,
  CLKRGM_LCG_DEINT_VD_RD,
  CLKRGM_LCG_FL_SYM_BUFF,
  CLKRGM_LCG_MOD_CX16,
  CLKRGM_LCG_MOD_CX8,
  CLKRGM_LCG_MOD_PDM,
  CLKRGM_LCG_DECOB,
  CLKRGM_LCG_TD,
  CLKRGM_LCG_VD,
  CLKRGM_LCG_GACC,
  CLKRGM_LCG_ALL,
  CLKRGM_LCG_NUM_REGIMES,
  CLKRGM_LCG_NOT_SUPPORTED = 0xff
} clk_regime_lcg_type;

/*=========================================================================
      Functions
==========================================================================*/

/*==========================================================================

  FUNCTION      CLK_RGM_MODEM_INIT

  DESCRIPTION   This function initializes the driver data structures and
                the modem core clock regimes.
                This function should be invoked during the chip clocks
                initialization procedure (e.g., TMC).

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The modem core clocks are configured.

==========================================================================*/

extern void clk_regime_modem_init(void);

/*==========================================================================

  FUNCTION      CLK_REGIME_RESET

  DESCRIPTION   This function resets the selected branch.

  PARAMETERS    branch  - Branch to reset.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified circuit is reset.
                PLEASE USE WITH DISCRETION.

==========================================================================*/

extern void clk_regime_reset (clkrgm_modem_clk_type branch);

/*==========================================================================

  FUNCTION      CLK_REGIME_ON

  DESCRIPTION   This function turns on a specific clock without checking
                if the source clock (e.g., Modem PLL) is enabled.
                Clear the halt bit or set the enable bit as appropriate
                for the clock.
  
  PARAMETERS    clk  - Clock to turn on.

  DEPENDENCIES  Successful enabling depends on the source clock being
                oscillating and stable.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock branch is turned on.

==========================================================================*/

extern void clk_regime_on (clkrgm_modem_clk_type clk);

/*==========================================================================

  FUNCTION      CLK_REGIME_OFF

  DESCRIPTION   This function turns off a specific clock without disabling
                any source clock (e.g., without turning off the Modem PLL).
                Set the halt bit or clear the enable bit as appropriate for
                the clock.
  
  PARAMETERS    clk  - Clock to turn off.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock branch is turned off.

==========================================================================*/

extern void clk_regime_off (clkrgm_modem_clk_type clk);

/*===========================================================================

FUNCTION        CLK_REGIME_LCG_OFF

DESCRIPTION     This function will disable the specified dynamic HW gating for
                the clock regime.
                No attempt to determine if the source clock is enabled is made.

PARAMETERS      item - LCG to be disabled for the clock regime.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    Selected LCG is turned off to disable dynamic gating.

===========================================================================*/

extern void clk_regime_lcg_off (clk_regime_lcg_type item);

/*===========================================================================

FUNCTION        CLK_REGIME_LCG_ON

DESCRIPTION     This function will enable the specified dynamic HW gating for
                the clock regime.
                No attempt to determine if the source clock is enabled is made.

PARAMETERS      item - LCG to be enabled for the clock regime.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    Selected LCG is turned on for dynamic gating.

===========================================================================*/

extern void clk_regime_lcg_on (clk_regime_lcg_type item);

/*==========================================================================

  FUNCTION      CLK_REGIME_IS_ON

  DESCRIPTION   This function checks the status of the specified clock.
                Read the halt or status bit associated with the clock,
                and return its value.
  
  PARAMETERS    clk  - Clock to check.

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if the clock is enabled;
                FALSE otherwise.

  SIDE EFFECTS  None.

==========================================================================*/

extern boolean clk_regime_is_on (clkrgm_modem_clk_type clk);

/*==========================================================================

  FUNCTION      CLK_REGIME_ENABLE

  DESCRIPTION   This function ensures that the clock source (e.g., Modem PLL)
                is enabled before turning on the specified clock.
                1.  If applicable, invoke the clk_regime_msm_modem_pll_request
                function to ensure that the Modem PLL is enabled.
                2.  Invoke the clk_regime_on function to turn on the
                specified clock.

  PARAMETERS    clk  - Clock to enable.

  DEPENDENCIES  Successful enabling depends on the source clock being
                oscillating and stable.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock is enabled.  If the Modem PLL was not
                enabled, then it is enabled.

==========================================================================*/

extern void clk_regime_enable (clkrgm_modem_clk_type clk);

/*==========================================================================

  FUNCTION      CLK_REGIME_MISC_ENABLE

  DESCRIPTION   This function invokes the clk_regime_enable function.
                This is a wrapping function for backward compatibility with
                existing code.

  PARAMETERS    clk  - Clock to enable.

  DEPENDENCIES  Successful enabling depends on the source clock being
                oscillating and stable.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock is enabled.  If the Modem PLL was not
                enabled, then it is enabled.

==========================================================================*/

extern void clk_regime_misc_enable (clkrgm_modem_clk_type clk);

/*===========================================================================

FUNCTION        CLK_REGIME_ENABLE_BT_PCM_CLK

DESCRIPTION     This function will enable/disable the blue tooth pcm clk.

DEPENDENCIES    Successful enabling depends on the source clock being
                oscillating and stable.

RETURN VALUE    None.

SIDE EFFECTS    MSM circuits are enabled/disabled.

===========================================================================*/

extern void clk_regime_enable_bt_pcm_clk (boolean enable);

/*==========================================================================

  FUNCTION      CLK_REGIME_DISABLE

  DESCRIPTION   This function turns off the specified clock, and ensures that
                the clock source (e.g., Modem PLL) is disabled if no longer
                used.
                1.  Invoke the clk_regime_off function to turn off the
                specified clock.
                2.  If applicable, invoke the clk_regime_msm_modem_pll_request
                function to ensure that the Modem PLL is disabled if no
                longer used.

  PARAMETERS    clk  - Clock to disable.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock is disabled.  If the Modem PLL is not
                used anymore, then it is disabled as well to conserve power.

==========================================================================*/

extern void clk_regime_disable (clkrgm_modem_clk_type clk);

/*==========================================================================

  FUNCTION      CLK_REGIME_MISC_DISABLE

  DESCRIPTION   This function invokes the clk_regime_disable function.
                This is a wrapping function for backward compatibility with
                existing code.

  PARAMETERS    clk  - Clock to disable.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock is disabled.  If the Modem PLL is not
                used anymore, then it is disabled as well to conserve power.

==========================================================================*/

extern void clk_regime_misc_disable (clkrgm_modem_clk_type clk);

/*===========================================================================

FUNCTION        CLK_REGIME_SEL_CLK_SRC

DESCRIPTION     Select a clock source for a specified clock regime.

PARAMETERS      regime - selected clock regime
                source - selected clock source

DEPENDENCIES    Clock regime input should be stable before enabling clock
                regime output.

RETURN VALUE    None.

SIDE EFFECTS    Clock regime source is switched.

===========================================================================*/

extern void clk_regime_sel_clk_src (clkrgm_modem_clk_type clk, uint32 source);

/*===========================================================================

FUNCTION        CLK_REGIME_SEL_CLK_DIV

DESCRIPTION     Select a clock divider for a specified clock regime.

PARAMETERS      regime - selected clock regime
                divider - selected clock divider

DEPENDENCIES    Clock regime input should be stable before enabling clock
                regime output.

RETURN VALUE    None.

SIDE EFFECTS    Clock divider will be updated for selected clock regime.

===========================================================================*/

extern void clk_regime_sel_clk_div (clkrgm_modem_clk_type clk, uint32 divider);

/*==========================================================================

  FUNCTION      CLK_REGIME_SEL_CLK_INV

  DESCRIPTION   This function selects a clock polarity for the specified clock.
                1.  Lock interrupts.
                2.  If the clock is enabled (see clk_regime_is_on), then
                disable the clock first (see clk_regime_off).
                3.  Switch the clock polarity.
                4.  If the clock was previously enabled, then turn on the
                clock (see clk_regime_on).
                5.  Unlock interrupts.

  PARAMETERS    clk     - Clock to invert polarity.
                invert  - TRUE to invert clock polarity;
                          FALSE to not invert clock polarity

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock polarity is inverted or not inverted
                accordingly.

==========================================================================*/

extern void clk_regime_sel_clk_inv (clkrgm_modem_clk_type  clk,
                                    boolean                invert
                                    );

/*==========================================================================

  FUNCTION      CLK_REGIME_CONFIG_CDMA_CLOCKS

  DESCRIPTION   This function sets various clocks for CDMA mode.
                This function is required to switch the MSM back to CDMA
                mode for the specified receiver chain.

  PARAMETERS    rx_chain - Receiver chain to configure.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The modem core clocks are configured and activated for
                CDMA mode.

==========================================================================*/

extern void clk_regime_config_cdma_clocks (clkrgm_rx_chain_type rx_chain);

/*==========================================================================

 GPS functions
==========================================================================*/

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/*==========================================================================

  FUNCTION      CLK_REGIME_AGC_SRC_ON

  DESCRIPTION   This function exists for backward compatibility.
                This function queries a flag indicating whether the GPS clock,
                which is also a clock source for AGC and the RX front-end,
                has been turned on or not.
                Due to asynchronous nature of GPS ChipX8 clock, as soon as
                GPS Clocks are configured, GPS ChipX8 doesn't turn on
                immediately.
                While the GPS ChipX8 is not turned on, but the AGC clock
                source is GPS ChipX16, the AGC block registers should not be
                touched.
                This function returns the state of the GPS clock, so that it
                can be queried before any access to AGC block.
                This function returns the state of the flag indicating whether
                the GPS clock has been turned on or not.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  TRUE if the GPS clock has been turned on;
                FALSE otherwise.

  SIDE EFFECTS  None.

==========================================================================*/

extern boolean clk_regime_agc_src_on (void);

/*==========================================================================

  FUNCTION      CLK_REGIME_SET_AGC_SRC_ON

  DESCRIPTION   This function exists for backward compatibility.
                This function sets a flag indicating whether the GPS clock,
                which is also a clock source for AGC and the RX front-end,
                has been turned on or not.
                Due to asynchronous nature of GPS ChipX8 clock, as soon as
                GPS Clocks are configured, GPS ChipX8 doesn't turn on
                immediately.
                While the GPS ChipX8 is not turned on, but the AGC clock
                source is GPS ChipX16, the AGC block registers should not be
                touched.
                This function is called to set the state of GPS clock when
                it is known that the clock has been turned on.
                This function sets the state of the flag indicating whether
                the GPS clock has been turned on or not.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

extern void clk_regime_set_agc_src_on (void);

/*==========================================================================

  FUNCTION      CLK_REGIME_SETUP_GSM_CLK

  DESCRIPTION   This function configures various clocks for GSM/GPRS mode.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The GSM/GPRS clock blocks are initialized and activated.

==========================================================================*/

extern void clk_regime_setup_gsm_clk (void);

/*==========================================================================

  FUNCTION      CLK_REGIME_SWITCH_MDSP_CLK

  DESCRIPTION   This function switches the mDSP clock frequency
                to the mdsp_pending_speed option.
                1.  Set MODEM_CLK_DYN_SW_BYPASS:BYPASS = 0.
                2.  Set MDSP_MNCNTR_NS(25):MDSP_TRIGGER_SEL = 0 to select the
                mARM as the trigger source (it is not needed to clear this bit
                if it's cleared before the clock switch).
                3.  Program MDSP_MNCNTR_MD with proper value (if the M/N:D
                counter output will be used as the mDSP clock source).
                4.  Program MDSP_MNCNTR_NS with proper value to set up the
                right MUXs.
                5.  Set MDSP_CLK_SWITCH:DATA = 1.
                6.  Set MDSP_CLK_SWITCH:DATA = 0.
                NOTE:  The software could bypass the dynamic clock switch
                circuitry by setting MODEM_CLK_DYN_SW_BYPASS = 1 and then
                program proper value to MDSP_MNCNTR_NS register.

  PARAMETERS    None.

  DEPENDENCIES  clk_regime_set_mdsp_clk_frequency should be called before
                calling this function.
                
                Interrupts should be locked by the caller

  RETURN VALUE  None.

  SIDE EFFECTS  The mDSP clock frequency is switched as requested.

==========================================================================*/

extern void clk_regime_switch_mdsp_clk (void);

/*===========================================================================

FUNCTION CLK_REGIME_COMPLETE_MDSP_CLK_SWITCH

DESCRIPTION
  Completes the process of switching RXDSP clock.This function is provided to
  complete any operations by the ARM after switching the RXDSP clock. For
  example when dynamically switching the RXDSP clock, the codec pll might
  need to be turned off if the clock frequency generation doesn't need CPLL.

DEPENDENCIES
  clk_regime_switch_mdsp_clk() should have been called prior to switch the
  RXDSP clock to the desired frequency prior to calling this function .

RETURN VALUE
  None

SIDE EFFECTS
  The CPLL might be turned off.
===========================================================================*/
extern void clk_regime_complete_mdsp_clk_switch ( void );

extern clkrgm_mdsp_clk_frequency_type clk_regime_get_mdsp_clk_frequency(void);

/*===========================================================================

FUNCTION CLK_REGIME_SET_MDSP_CLK_FREQUENCY

DESCRIPTION
  Allows outside users to inform Clock Regime services as to what will be the
  RXDSP clock frequency is going to be. This function is used to prepare the
  data structures for the RXDSP clock before changing the clock frequency.This
  function doesn't actually switch the clock source for the RXDSP, that will
  be done by clk_regime_switch_mdsp_clk().

  This function is provided so as to reduce the time during which the DSP is
  held in wait mode for switching the clock frequency. This way, the selection
  of the frequency can be done any time before the actual switching and the DSP
  can continue to operate while the clock frequency selection is being carried
  out. The DSP needs to be put into wait mode only when the actual switching of
  the clock frequency is required.

DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void clk_regime_set_mdsp_clk_frequency
(
   clkrgm_mdsp_clk_frequency_type clk_frequency
);

#endif  /* _ARM_ASM_ */

#endif  /* CLKRGM_MODEM_H */ 

