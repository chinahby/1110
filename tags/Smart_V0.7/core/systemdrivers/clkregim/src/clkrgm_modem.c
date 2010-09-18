/*=========================================================================

              M O D E M   C L O C K   D E V I C E   D R I V E R

GENERAL DESCRIPTION
  This file contains the implementation of the the Modem Clock Device
  Driver.  The modem clocks part of the Clock Device Driver is responsible
  for controlling the modem core clock blocks, and the mDSP clock block.

EXTERNALIZED FUNCTIONS
  clk_regime_modem_init
  clk_regime_reset
  clk_regime_is_on
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
  clk_regime_on
  clk_regime_off
  clk_regime_enable
  clk_regime_disable

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

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_modem.c#16 $
$DateTime: 2009/01/27 09:30:07 $
$Author: ovyas $


when       who      what, where, why
--------   ---      --------------------------------------------------------
01/20/09   hvo      Featurized the Modem Turbo Decode, only available for mid
                    and high tiers      
08/22/08   hvo      Enabled mDSP clock halter
10/16/07   bc       Ported from QSC6055 v3202.
06/08/07   cr       Use DEVMAN to set MDSP clock frequency.
06/06/07   cr       Fix MDSP frequency setting.
04/19/07   cr       Add NOPLL pll source (for TCXO) to configuration, so need
                    to test to make sure not to pass NOPLL to the PLL interface.
                    Added function to return MDSP frequency.
03/09/07   cr       Add shared clock client interface header. This is a poor
                    man's clock regime Resource Manager until 7K Resource Manager
                    integration can be completed.
02/20/07   vm       Added the support for ADC Sampling clock for SGPS
02/06/07   cr       Have to break the _is_on() logic out for rxf clocks.
01/10/07   cr       Clean up non-ported code.
01/10/07   cr       Add support for clk_regime_rxf_request().
12/08/06   vm       Corrected the Clock settings for Rx Front
07/23/05   wd       L4 Merge.
03/17/05   smkg     Merge updates from the MSM6800
12/13/04   smkg     Redesign for the MSM7500

==========================================================================*/


/*==========================================================================
                     INCLUDE FILES FOR MODULE
==========================================================================*/

#include "err.h"
#include "target.h"     /* Target specific definitions          */
#include "msm.h"
#include "msmhwio.h"
#include "comdef.h"

#include "clk.h"
#include "clkrgm_client.h"
#include "clkrgmi_devman.h"
#include "clkrgmi_msm.h"

#include CLK_RGMI_MODEM_H
#include CLKRGM_MSM_H
#include CLK_RGM_MODEM_H


#ifdef FEATURE_CLKREGIM_RM
#include "clkrgm_rm.h"

#define RM_CLK_INFO(clk) \
  (&clkrgm_modem.rm_clk_info[(int)clk - CLKRGM_MODEM_CLK_OFFSET])

#endif
/*==========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE
==========================================================================*/
/*=========================================================================
      Typedefs
==========================================================================*/
/* See clkrgm_modem.h and clkrgmi_modem.h */

/*=========================================================================
      Turn on for bringup only
==========================================================================*/
//#ifdef TIMETEST
//  #ifdef T_MSM6800B
//    #define T_BRINGUP
//  #endif
//#endif


/*=========================================================================
      Constants and Macros
==========================================================================*/

/*=========================================================================
      Variables
==========================================================================*/

extern clkrgm_msm_state_type clkrgm;

/* Configuration of the mDSP clock for all the available speeds */
extern const clkrgm_modem_mdsp_cfg_type  clkrgm_modem_mdsp_cfg
  [CLKRGM_MDSP_NUM_SPEEDS];

/* Modem clocks general state and configuration */
extern clkrgm_modem_state_type           clkrgm_modem;

/* Only toggle CHIPXN_DIV2, RXF_SAMP, and RXF_SAMP_REF if caller is 
 * clk_regime_rxf_request().
 */
static boolean clkrgm_manages_rxf = FALSE;

extern void clk_regime_modem_log
(
  clkrgm_modem_clk_log_type clk_num,
  uint8 transaction);

/*==========================================================================
               FUNCTION DECLARATIONS FOR MODULE
==========================================================================*/

/*==========================================================================

  FUNCTION      CLK_REGIME_MODEM_INIT

  DESCRIPTION   This function initializes the driver data structures and
                the modem core clock regimes.
                This function should be invoked during the chip clocks
                initialization procedure.

  PARAMETERS    None.

  DEPENDENCIES  MSM CLK initilization must have been invoked to turn on and
                initialize PLLs and CPU clock before this function is invoked.
                Ideally, this function should be called from within the MSM
                CLK initilaization.

  RETURN VALUE  None.

  SIDE EFFECTS  The modem core clocks are configured.

==========================================================================*/

void clk_regime_modem_init(void)
{
  /*-----------------------------------------------------------------------*/
  /* Local variables                                                       */
  /*-----------------------------------------------------------------------*/

  uint32  isave;

  /*-----------------------------------------------------------------------*/
  /* Lock interrupts                                                       */
  /*-----------------------------------------------------------------------*/

  INTLOCK_SAV(isave);


  /*-----------------------------------------------------------------------*/
  /* Initialize the driver state data structure                            */
  /*-----------------------------------------------------------------------*/

  /* Setup the driver state */
  clkrgm_modem.agc_src_is_on = TRUE;
  clkrgm_modem.clk_mode[CLKRGM_RX_CHAIN_0] = CLKRGM_MODEM_CLK_MODE_NUM_MODES;
  clkrgm_modem.clk_mode[CLKRGM_RX_CHAIN_1] = CLKRGM_MODEM_CLK_MODE_NUM_MODES;
  /*-----------------------------------------------------------------------*/
  /* Program the clocks                                                    */
  /*-----------------------------------------------------------------------*/

  #ifdef T_MSM6800B

  /* setup top level mux for main modem clocks */

  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, ANT0_CLK_DSP_CTL_EN,
                  CLK_RGM_SRC_SEL_ANT0_CLK_DSP_CTL_ARM_V);

  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, ANT1_CLK_DSP_CTL_EN,
                  CLK_RGM_SRC_SEL_ANT1_CLK_DSP_CTL_ARM_V);

  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, CDMA_CHIPXN_CLK_DIV,
                  CLK_RGM_DIV_SEL_CHIPXN_CLK_DIV_CHIPX16_DIV2_V);

  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, BT_SBI_CLK_DIV,
                  CLK_RGM_DIV_SEL_BT_SBI_CLK_DIV1_V);

  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, BT_SBI_CLK_SRC_SEL,
                  CLK_RGM_SRC_SEL_BT_SBI_CLK_SRC_TCXO1_V);

  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, BT_CLK_SRC_SEL,
                  CLK_RGM_SRC_SEL_BT_CLK_SRC_BT_CLK_V);

  #endif     

  #ifdef T_QSC6055
  /* Select the source of ChipxN to be derived from Modem ChipxN Div1*/
  CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_MISC_CTL, CDMA_CHIPXN_SRC_SEL, MODEM_CHIPXN_CLK_SRC_SEL_VAL);

    /* Done Due to bug in the Hardware that it shows to be R/W but its Read Only register*/
  /* Hence no shadow is mainitained*/
  HWIO_OUT(MODEM_RXF_SAMP_REF_CLK_CTL,CLK_RGM_MODEM_RXF_SAMP_REF_CLK_CTL_V);
  #endif

  //Always Select the ANT0 Sample Clk SRC coming from ADC
  CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_MISC_CTL, CDMA_RX_SAMPLE_RAW_SEL, 1);

  //Always invert the RXF Sample clk so that Rx Front
  //data is sampled at falling edge
  CLKRGM_SET(MODEM_CLK_INV, RXF_SAMP_CLK_INV);
  
  /* MODEM_CHIPXN clock configuration */
  CLKRGM_W(MODEM_CHIPXN_CLK_CTL, MODEM_CHIPXN_CLK_CTL_V);
  CLKRGM_W(MODEM_CDMA_CHIPXN_MNCNTR_M, MODEM_CHIPXN_MNCNTR_M_V);
  CLKRGM_W(MODEM_CDMA_CHIPXN_MNCNTR_NOT_2D, MODEM_CHIPXN_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_CDMA_CHIPXN_MNCNTR_NOT_N_M, MODEM_CHIPXN_MNCNTR_NOT_N_M_V);
  CLKRGM_W(MODEM_CDMA_CHIPXN_MNCTNR_UPDATE, 1);

  /* MODEM_OFFLINE clock configuration */
  CLKRGM_W(MODEM_OFFLINE_CLK_CTL, MODEM_OFFLINE_CLK_CTL_V);
  CLKRGM_W(MODEM_OFFLINE_MNCNTR_M, MODEM_OFFLINE_MNCNTR_M_V);
  CLKRGM_W(MODEM_OFFLINE_MNCNTR_NOT_2D, MODEM_OFFLINE_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_OFFLINE_MNCNTR_NOT_N_M, MODEM_OFFLINE_MNCNTR_NOT_N_M_V);

#ifdef CLKRGM_INCLUDE_GPS
  /* MODEM_GACC clock configuration */
  CLKRGM_W(MODEM_GACC_CLK_CTL, MODEM_GACC_CLK_CTL_V);
  CLKRGM_W(MODEM_GACC_MNCNTR_M, MODEM_GACC_MNCNTR_M_V);
  CLKRGM_W(MODEM_GACC_MNCNTR_NOT_2D, MODEM_GACC_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_GACC_MNCNTR_NOT_N_M, MODEM_GACC_MNCNTR_NOT_N_M_V);
#endif

  /* MODEM_GPS clock configuration */
  /* No need to start GPS clk at boot up time for power consumption purpose */
#ifdef T_BRINGUP
#ifdef CLKRGM_INCLUDE_GPS
  CLKRGM_W(MODEM_GPS_CLK_CTL, MODEM_GPS_CLK_CTL_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_M, MODEM_GPS_MNCNTR_M_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_NOT_2D, MODEM_GPS_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_NOT_N_M, MODEM_GPS_MNCNTR_NOT_N_M_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_UPDATE, 1);
#endif

#ifdef CLKRGM_INCLUDE_WCDMA
  /* MODEM_F9 clock configuration */
  CLKRGM_W(MODEM_F9_CLK_CTL, MODEM_F9_CLK_CTL_V);
#endif

  #ifndef T_MSM6800B

#ifdef CLKRGM_INCLUDE_GSM
  /* MODEM_EDGE_TX clock configuration */
  CLKRGM_W(MODEM_EDGE_TX_CLK_CTL, MODEM_EDGE_TX_CLK_CTL_V);
  CLKRGM_W(MODEM_EDGE_TX_MNCNTR_M, MODEM_EDGE_TX_MNCNTR_M_V);
  CLKRGM_W(MODEM_EDGE_TX_MNCNTR_NOT_2D, MODEM_EDGE_TX_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_EDGE_TX_MNCNTR_NOT_N_M, MODEM_EDGE_TX_MNCNTR_NOT_N_M_V);

  /* MODEM_EDGE_RX clock configuration */
  CLKRGM_W(MODEM_EDGE_RX_CLK_CTL, MODEM_EDGE_RX_CLK_CTL_V);
  CLKRGM_W(MODEM_EDGE_RX_MNCNTR_M, MODEM_EDGE_RX_MNCNTR_M_V);
  CLKRGM_W(MODEM_EDGE_RX_MNCNTR_NOT_2D, MODEM_EDGE_RX_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_EDGE_RX_MNCNTR_NOT_N_M, MODEM_EDGE_RX_MNCNTR_NOT_N_M_V);
#endif

#ifdef CLKRGM_INCLUDE_GPS
  /* MODEM_GSAC clock configuration */
  CLKRGM_W(MODEM_GSAC_CLK_CTL, MODEM_GSAC_CLK_CTL_V);
  CLKRGM_W(MODEM_GSAC_MNCNTR_M, MODEM_GSAC_MNCNTR_M_V);
  CLKRGM_W(MODEM_GSAC_MNCNTR_NOT_2D, MODEM_GSAC_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_GSAC_MNCNTR_NOT_N_M, MODEM_GSAC_MNCNTR_NOT_N_M_V);
#endif

  #else

#ifdef CLKRGM_INCLUDE_GSM
  /* MODEM_EDGE clock configuration */
  CLKRGM_W(MODEM_EDGE_CLK_CTL, MODEM_EDGE_TX_CLK_CTL_V);
  CLKRGM_W(MODEM_EDGE_MNCNTR_M, MODEM_EDGE_TX_MNCNTR_M_V);
  CLKRGM_W(MODEM_EDGE_MNCNTR_NOT_2D, MODEM_EDGE_TX_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_EDGE_MNCNTR_NOT_N_M, MODEM_EDGE_TX_MNCNTR_NOT_N_M_V);
#endif

  #endif

#else /* for normal mode set unnecessary clock source trees to TCXO */

#ifdef CLKRGM_INCLUDE_GPS
  CLKRGM_W(MODEM_GPS_CLK_CTL, MODEM_GPS_CLK_CTL_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_M, MODEM_GPS_MNCNTR_M_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_NOT_2D, MODEM_GPS_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_NOT_N_M, MODEM_GPS_MNCNTR_NOT_N_M_V);
  CLKRGM_W(MODEM_GPS_MNCNTR_UPDATE, 1);
#endif

#ifdef CLKRGM_INCLUDE_WCDMA
  /* MODEM_F9 clock configuration */
  CLKRGM_W(MODEM_F9_CLK_CTL, MODEM_F9_CLK_CTL_V);
#endif

#ifdef CLKRGM_INCLUDE_GSM
  /* MODEM_EDGE_TX clock configuration */
  #ifndef T_MSM6800B
  CLKRGM_W(MODEM_EDGE_TX_CLK_CTL, MODEM_EDGE_TX_CLK_CTL_V);
  CLKRGM_W(MODEM_EDGE_TX_MNCNTR_M, MODEM_EDGE_TX_MNCNTR_M_V);
  CLKRGM_W(MODEM_EDGE_TX_MNCNTR_NOT_2D, MODEM_EDGE_TX_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_EDGE_TX_MNCNTR_NOT_N_M, MODEM_EDGE_TX_MNCNTR_NOT_N_M_V);

  /* MODEM_EDGE_RX clock configuration */
  CLKRGM_W(MODEM_EDGE_RX_CLK_CTL, 0x0);
  CLKRGM_W(MODEM_EDGE_RX_MNCNTR_M, MODEM_EDGE_RX_MNCNTR_M_V);
  CLKRGM_W(MODEM_EDGE_RX_MNCNTR_NOT_2D, MODEM_EDGE_RX_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_EDGE_RX_MNCNTR_NOT_N_M, MODEM_EDGE_RX_MNCNTR_NOT_N_M_V);
#endif

#ifdef CLKRGM_INCLUDE_GPS
  /* MODEM_GSAC clock configuration */
  CLKRGM_W(MODEM_GSAC_CLK_CTL, 0x0);
  CLKRGM_W(MODEM_GSAC_MNCNTR_M, MODEM_GSAC_MNCNTR_M_V);
  CLKRGM_W(MODEM_GSAC_MNCNTR_NOT_2D, MODEM_GSAC_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_GSAC_MNCNTR_NOT_N_M, MODEM_GSAC_MNCNTR_NOT_N_M_V);
#endif

  #else

#ifdef CLKRGM_INCLUDE_GSM
  CLKRGM_W(MODEM_EDGE_CLK_CTL, MODEM_EDGE_TX_CLK_CTL_V);
  CLKRGM_W(MODEM_EDGE_MNCNTR_M, MODEM_EDGE_TX_MNCNTR_M_V);
  CLKRGM_W(MODEM_EDGE_MNCNTR_NOT_2D, MODEM_EDGE_TX_MNCNTR_NOT_2D_V);
  CLKRGM_W(MODEM_EDGE_MNCNTR_NOT_N_M, MODEM_EDGE_TX_MNCNTR_NOT_N_M_V);
#endif

  #endif

#endif /* T_BRINGUP */

  /* MODEM_WEB clock configuration */
  CLKRGM_W(MODEM_WEB_CLK_CTL, MODEM_WEB_CLK_CTL_V);

  /*-----------------------------------------------------------------------*/
  /* Invert the polarity of the modem clocks as required                   */
  /*-----------------------------------------------------------------------*/

#ifdef T_QSC6055
  #ifdef T_RUMI
#error code not present
  #endif
#else
  #ifdef T_RUMI
#error code not present
  #endif
#endif

  /*-----------------------------------------------------------------------*/
  /* Program the mDSP footswitch                                           */
  /*-----------------------------------------------------------------------*/

  CLKRGM_W(MODEM_CLK_MDSP_FS_CTL, CLKRGM_MODEM_CLK_MDSP_FS_CTL_V);

  /*-----------------------------------------------------------------------*/
  /* Configure the receiver analog clocks for CDMA mode                    */
  /*-----------------------------------------------------------------------*/

#ifdef T_QSC6055
  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, ANT_SAMP_CHIPXN_DIV_SEL,
                  CLK_RGM_DIV_SEL_ANALOG_CXN_DIV1_V);
#else
  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, ANT_SAMP_CHIPXN_DIV_SEL,
                  CLK_RGM_DIV_SEL_ANALOG_CXN_DIV2_V);
#endif
  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL,
                  CLK_RGM_SRC_SEL_ANT_SAMP_CLK_ANT_CDMA_CHIPXN_DIV_CLK_V);
  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL_2, ANT1_SAMP_CLK_SRC_SEL,
                  CLK_RGM_SRC_SEL_ANT_SAMP_CLK_ANT_CDMA_CHIPXN_DIV_CLK_V);

  /*-----------------------------------------------------------------------*/
  /* Program the mDSP clock to the highest speed                           */
  /*-----------------------------------------------------------------------*/

#ifndef T_QSC6055
  #if defined(T_RUMI) || defined(T_MSM7500)
#error code not present
  #else
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, MDSP_CLK_1X_MODE, 0);
  #endif
#else
    HWIO_OUTM(MODEM_CLK_MISC_CTL, HWIO_FMSK(MODEM_CLK_MISC_CTL, MDSP_CLK_1X_MODE),
                                  HWIO_FMSK(MODEM_CLK_MISC_CTL, MDSP_CLK_1X_MODE));
#endif /* T_QSC6055 */

  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, MDSP_DMA_CLK_OVR_N, 1);

  clkrgm_modem.mdsp_speed = CLKRGM_MDSP_SPEED_LOW;
  clk_regime_set_mdsp_clk_frequency (CLKRGM_MDSP_SPEED_HIGH);
  clk_regime_switch_mdsp_clk ();
  clk_regime_complete_mdsp_clk_switch ();
  /*-----------------------------------------------------------------------*/
  /* Enable default clocks                                                 */
  /* (Enable all the clocks for bringup)                                   */
  /*-----------------------------------------------------------------------*/

  /* De-assert all the clock reset branches */

  CLKRGM_W( MODEM_CLK_RESET, 0x0 );
  CLKRGM_W( MODEM_CLK_RESET_2, 0x0 );

#ifndef T_BRINGUP

  /* Enable all the modem clocks */
  /* this is needed before HW gating registers for all clocks can be accessed */
  clk_regime_enable_client(CLKRGM_MODEM_ALL_CLK, CLKRGM_CLKREGIM);

  /* Disable all dynamic HW controlled gating */
  /* these clocks will be controlled by SW */
  clk_regime_lcg_off(CLKRGM_LCG_ALL);

  /* disable all so that only what is needed can be enabled */
  clk_regime_disable_client(CLKRGM_MODEM_ALL_CLK, CLKRGM_CLKREGIM);

  /* Enable CDMA_TX clock regime.
     This is for enc_init() function modulator register initialization. */
  clk_regime_enable_client(CLKRGM_CDMA_TX_CLK, CLKRGM_CLKREGIM);

  #ifdef CLKRGM_INCLUDE_HDR
  /* Enable CLK_RGM_HDR_TDEC_M clock regime.
     This is for HDR DEC register initialization. */
  clk_regime_enable_client(CLKRGM_HDR_TDEC_CLK, CLKRGM_CLKREGIM);
  #endif /* CLKRGM_INCLUDE_HDR */

  /* Enable CLK_RGM_MDSP_M clock regime.  This is for srch4_init() register
     initialization. */
  clk_regime_enable_client(CLKRGM_MDSP_CLK, CLKRGM_CLKREGIM);

  /* Enable CLK_RGM_GSM_M clock regime.
     This is GSM_CORE_MICRO_RESET_REG register
     initialization tmc_define_tasks() function. */

  /* Enable the GSM clock for use by the GSM or RUIM interface, if required. */
#if defined(FEATURE_UIM_GSM_CLK_SRC) || defined(FEATURE_GSM)
#error code not present
#else
  #ifndef T_MSM6800B
  CLKRGM_W(MODEM_EDGE_TX_CLK_CTL, 0x0);
  #else
  CLKRGM_W(MODEM_EDGE_CLK_CTL, 0x0);
  #endif
#endif

  /* Enable sleep controller clock regimes.
     These are not referenced anywhere else in the code. */
  clk_regime_enable_client(CLKRGM_SLPFAST_FEE_CLK, CLKRGM_CLKREGIM);

  clk_regime_enable_client(CLKRGM_SLPFAST_CHIPXN_REF1_CLK, CLKRGM_CLKREGIM);
  clk_regime_enable_client(CLKRGM_SLPFAST_CHIPXN_REF2_CLK, CLKRGM_CLKREGIM);

#ifdef FEATURE_GSM
#error code not present
#endif

#else

#ifndef FEATURE_L4
  /* Configure the CDMA Rx chain */
  clk_regime_config_cdma_clocks(CLKRGM_RX_CHAIN_BOTH);
#endif

  /* Enable all the clocks for bringup */
  clk_regime_enable_client(CLKRGM_MODEM_ALL_CLK, CLKRGM_CLKREGIM);

  /* Disable all dynamic HW controlled gating */
  /* these clocks will be controlled by SW for now */
  clk_regime_lcg_off(CLKRGM_LCG_ALL);

#endif /* !T_BRINGUP */

  /* Enable mDSP clock halter to save power consumption */
  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, MDSP_HM_CLK_OVR_N, 1);
  CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, MDSP_CLK_HALT_TIMER_VAL, 4);

  /*-----------------------------------------------------------------------*/
  /* Unlock interrupts                                                     */
  /*-----------------------------------------------------------------------*/

  INTFREE_SAV(isave);
} /* clkrgm_modem_init */

/*==========================================================================

  FUNCTION      CLK_REGIME_RESET

  DESCRIPTION   This function resets the selected branch.

  PARAMETERS    branch  - Branch to reset.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified circuit is reset.
                PLEASE USE WITH DISCRETION.

==========================================================================*/

void clk_regime_reset (clkrgm_modem_clk_type branch)
{
  INTLOCK();

  switch (branch)
  {
    case CLKRGM_CHIPXN_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, CDMA_CHIPXN_RESET);
      break;

    case CLKRGM_CHIPXN_DIV2_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, CDMA_CHIPXN_DIV2_RESET);
      break;

    case CLKRGM_CHIPXN_DIV4_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, CDMA_CHIPXN_DIV4_RESET);
      break;

    case CLKRGM_RXF_SAMP_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, RXF_SAMP_RESET);
      break;

    #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_EDGE_TX_CLK:
      #ifndef T_MSM6800B
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, EDGE_TX_RESET);
      #else
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, EDGE_RESET);
      #endif
      break;

    case CLKRGM_EDGE_RX_CLK:
      #ifndef T_MSM6800B
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, EDGE_RX_RESET);
      #else
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, EDGE_RESET);
      #endif
      break;

    case CLKRGM_GSM_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, GSM_RESET);
      break;

    #ifndef T_MSM6800B
    case CLKRGM_GSAC_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, GSAC_RESET);
      break;
    #endif
    #endif /* CLKRGM_INCLUDE_GSM */

    case CLKRGM_TX_PDM_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, TX_PDM_RESET);
      break;

    case CLKRGM_TCXO_PDM_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, TCXO_PDM_RESET);
      break;

    case CLKRGM_MDSP_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, MDSP_RESET);
      break;

    case CLKRGM_SLEEP_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, SLEEP_RESET);
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF1_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, CDMA_SLPFAST_CHIPXN_REF1_RESET);
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF2_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, CDMA_SLPFAST_CHIPXN_REF2_RESET);
      break;

    #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_SLPFAST_GSM_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, SLPFAST_GSM_RESET);
      break;
    #endif /* CLKRGM_INCLUDE_GSM */

    case CLKRGM_SLPFAST_FEE_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, SLPFAST_FEE_RESET);
      break;

    case CLKRGM_BT_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, BT_RESET);
      break;

    case CLKRGM_MODEM_SBI_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, MODEM_SBI_RESET);
      break;

    case CLKRGM_MODEM_WEB_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, MODEM_WEB_RESET);
      break;

    case CLKRGM_OFFLINE_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, OFFLINE_RESET);
      break;

    #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, GACC_RESET);
      break;

    case CLKRGM_GPS_CHIPX8_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, GPS_CHIPX8_RESET);
      break;
    #endif /* CLKRGM_INCLUDE_GPS */

    #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GPS_CHIPX16_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET_2, GPS_CHIPX16_RESET);
      break;

    case CLKRGM_GPS_CHIPX32_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET_2, GPS_CHIPX32_RESET);
      break;
    #endif /* CLKRGM_INCLUDE_GPS */

    #ifdef CLKRGM_INCLUDE_WCDMA
    case CLKRGM_F9_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET, F9_RESET);
      break;
    #endif /* CLKRGM_INCLUDE_WCDMA */

    #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_MMGPS_CLK:
      break;

    case CLKRGM_MMGPS_ARM_CLK:
      break;
    #endif /* CLKRGM_INCLUDE_GPS */

    case CLKRGM_TX_DAC_CLK:
      break;

    case CLKRGM_PC_DAC_CLK:
      break;

    case CLKRGM_MDM_CLK:
      CLKRGM_PULSE_NL(MODEM_CLK_RESET_2, MDM_RESET);
      break;

    case CLKRGM_DEC_CLK:
      break;

    #ifdef CLKRGM_INCLUDE_HDR
    case CLKRGM_HDR_TDEC_CLK:
      break;
    #endif /* CLKRGM_INCLUDE_HDR */

    case CLKRGM_TURBO_DEC_CLK:
      break;

    case CLKRGM_SVD_CLK:
      break;

    case CLKRGM_CHIPX8_CLK:
      break;

    case CLKRGM_CDMA_TX_CLK:
      break;

    case CLKRGM_CDMA_RX_CLK:
      break;

    case CLKRGM_RXCHIPX8_CLK:
      break;

    case CLKRGM_MODEM_ALL_CLK:
      break;

    default:
      /* Error: Reset branch not supported (out of range) */
      ERR("clk_regime_reset: Reset branch out of range (%d)", branch, 0, 0);
      break;
  }

  INTFREE();
} /* clk_regime_reset */


/*==========================================================================

  FUNCTION      CLK_REGIME_CFG_CLK_SRC

  DESCRIPTION   This function configures the given clock's source (PLL) to
                enabled or disabled.

  PARAMETERS    clk     - clock whose PLL to configure
                request - enable or disable

  DEPENDENCIES  Successful enabling depends on the source clock being
                oscillating and stable.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void clk_regime_src_request
(
  clkrgm_modem_clk_type clk,
  clkrgm_pll_op_type    request
)
{
  clkrgm_pll_type pll = CLKRGM_NO_PLL;

  /* ------------------------------------------------------------------------
     Determine the PLL for this clock
  ------------------------------------------------------------------------ */

  switch (clk)
  {
    case CLKRGM_CHIPXN_CLK:
    case CLKRGM_CHIPXN_DIV2_CLK:
    case CLKRGM_CHIPXN_DIV4_CLK:
  #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_EDGE_TX_CLK:
    case CLKRGM_EDGE_RX_CLK:
    case CLKRGM_GSM_CLK:
    case CLKRGM_GSAC_CLK:
    case CLKRGM_SLPFAST_GSM_CLK:
  #endif
    case CLKRGM_TX_PDM_CLK:
    case CLKRGM_TCXO_PDM_CLK:
    case CLKRGM_SLPFAST_CHIPXN_REF1_CLK:
    case CLKRGM_SLPFAST_CHIPXN_REF2_CLK:
    case CLKRGM_SLPFAST_FEE_CLK:
    case CLKRGM_BT_CLK:
    case CLKRGM_MODEM_SBI_CLK:
    case CLKRGM_MODEM_WEB_CLK:
    case CLKRGM_OFFLINE_CLK:
  #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_CLK:
    case CLKRGM_GPS_CHIPX8_CLK:
    case CLKRGM_GPS_CHIPX16_CLK:
    case CLKRGM_GPS_CHIPX32_CLK:
    case CLKRGM_MMGPS_CLK:
    case CLKRGM_MMGPS_ARM_CLK:
  #endif
  #ifdef CLKRGM_INCLUDE_WCDMA
    case CLKRGM_F9_CLK:
  #endif
    case CLKRGM_TX_DAC_CLK:
    case CLKRGM_PC_DAC_CLK:
    case CLKRGM_MDM_CLK:
    case CLKRGM_DEC_CLK:
  #ifdef CLKRGM_INCLUDE_HDR
    case CLKRGM_HDR_TDEC_CLK:
  #endif
    case CLKRGM_TURBO_DEC_CLK:
    case CLKRGM_SVD_CLK:
    case CLKRGM_CHIPX8_CLK:
    case CLKRGM_CDMA_TX_CLK:
    case CLKRGM_CDMA_RX_CLK:
    case CLKRGM_RXCHIPX8_CLK:
    case CLKRGM_MODEM_ALL_CLK:
    case CLKRGM_RXF_SAMP_CLK:
    case CLKRGM_RXF_SAMP_REF_CLK:
      pll = CLKRGM_PLL;
      break;

    case CLKRGM_MDSP_CLK:
      pll = clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_speed].pll;
      break;
  
    case CLKRGM_FB_ANT0_SAMP_CLK:  
    case CLKRGM_FB_ANT1_SAMP_CLK:
      //don't need to request digital pll since the above two are from RF.
      break; 
    case CLKRGM_OBSOLETE_CLK:
      break;

    default:
      /* Error: Clock branch of out range */
      ERR("clk_regime_enable: Clock branch out of range (%d)", clk, 0, 0);
      break;
  }

  /* ------------------------------------------------------------------------
     Make the PLL request
  ------------------------------------------------------------------------ */

  if (pll != CLKRGM_NO_PLL)
  {
    clk_regime_pll_request (pll, clk, request);
  }

} /* clk_regime_src_request */
/*==========================================================================

  FUNCTION      CLK_REGIME_ENABLE

  DESCRIPTION   This function ensures that the clock source (e.g., Modem PLL)
                is enabled before turning on the specified clock.
                1.  If applicable, invoke the clk_regime_modem_pll_request
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

void clk_regime_enable (clkrgm_modem_clk_type clk)
{
  uint32 isav;

  /* ------------------------------------------------------------------------
     Intercept group clocks.  All such clocks should be obseleted and
     RM should be used instead.
  ------------------------------------------------------------------------ */


  /* ------------------------------------------------------------------------
     Ensure that this operation is atomic
  ------------------------------------------------------------------------ */

  INTLOCK_SAV(isav);


  #if defined(FEATURE_CLKREGIM_RM) && !defined(OEMSBL)
  /* ------------------------------------------------------------------------
     Check with the resource manager
  ------------------------------------------------------------------------ */

  if (!clk_regime_rm_clk_enable(RM_CLK_INFO(clk)))
  {
    INTFREE_SAV(isav);
    return;
  }
  #endif

  /* ------------------------------------------------------------------------
     Configure any required source
  ------------------------------------------------------------------------ */

  clk_regime_src_request(clk, CLKRGM_PLL_ENABLE);

  /* ------------------------------------------------------------------------
     Enable the clock
  ------------------------------------------------------------------------ */

  clk_regime_on(clk);

  /* ------------------------------------------------------------------------
     Free interrupts
  ------------------------------------------------------------------------ */

  INTFREE_SAV(isav);

} /* clk_regime_enable */

/*==========================================================================

  FUNCTION      CLK_REGIME_DISABLE

  DESCRIPTION   This function turns off the specified clock, and ensures that
                the clock source (e.g., Modem PLL) is disabled if no longer
                used.
                1.  Invoke the clk_regime_off function to turn off the
                specified clock.
                2.  If applicable, invoke the clk_regime_modem_pll_request
                function to ensure that the Modem PLL is disabled if no
                longer used.

  PARAMETERS    clk  - Clock to disable.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock is disabled.  If the Modem PLL is not
                used anymore, then it is disabled as well to conserve power.

==========================================================================*/

void clk_regime_disable (clkrgm_modem_clk_type clk)
{
  uint32 isav;

  /* ------------------------------------------------------------------------
     Intercept group clocks.  All such clocks should be obseleted and
     RM used instead.
  ------------------------------------------------------------------------ */

  /* ------------------------------------------------------------------------
     Ensure that this operation is atomic
  ------------------------------------------------------------------------ */

  INTLOCK_SAV(isav);

  #if defined(FEATURE_CLKREGIM_RM) && !defined(OEMSBL)
  /* ------------------------------------------------------------------------
     Check with the resource manager
  ------------------------------------------------------------------------ */

  if (!clk_regime_rm_clk_disable(RM_CLK_INFO(clk)))
  {
    INTFREE_SAV(isav);
    return;
  }
  #endif

  /* Check if modem clocks are being forced to remain on */
  if (clkrgm_modem.debug.clks_always_on)
  {
    INTFREE_SAV(isav);
    return;
  }

  /* ------------------------------------------------------------------------
     Disable the clock branch
  ------------------------------------------------------------------------ */

  clk_regime_off(clk);

  /* ------------------------------------------------------------------------
     Configure any required source to disabled
  ------------------------------------------------------------------------ */

  clk_regime_src_request(clk, CLKRGM_PLL_DISABLE);

  /* ------------------------------------------------------------------------
     Free interrupts
  ------------------------------------------------------------------------ */

  INTFREE_SAV(isav);

} /* clk_regime_disable */

/*==========================================================================

  FUNCTION      CLK_REGIME_ON

  DESCRIPTION   This function turns on a specific clock without checking
                if the source clock (e.g., Modem PLL) is enabled.
                Clear the halt bit or set the enable bit as appropriate
                for the clock.
                
                CHIPXN_DIV2, RXF_SAMP, and RXF_SAMP_REF are only toggled
                if the caller is clk_regime_rxf_request(). Any other caller
                has no effect.
  
  PARAMETERS    clk  - Clock to turn on.

  DEPENDENCIES  Successful enabling depends on the source clock being
                oscillating and stable.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock branch is turned on.

==========================================================================*/

void clk_regime_on (clkrgm_modem_clk_type clk)
{
  uint32 sav_var;

  /*-----------------------------------------------------------------------*/
  /* Interrupts are locked to prevent race conditions                      */
  /*-----------------------------------------------------------------------*/
  INTLOCK_SAV (sav_var);

  switch (clk)
  {
    case CLKRGM_CHIPXN_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_ANT_SAMP_CHIPXN_CLK:
    case CLKRGM_CHIPXN_DIV2_CLK:
      if (clkrgm_manages_rxf == TRUE)
      {
        CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
      break;

    case CLKRGM_CHIPXN_DIV4_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV4_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

   case CLKRGM_RXF_SAMP_CLK:
      if (clkrgm_manages_rxf == TRUE)
      {
        
        CLKRGM_CLEAR_NL(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT);    
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
     break;

    case CLKRGM_RXF_SAMP_REF_CLK:
      if (clkrgm_manages_rxf == TRUE)
      {
        CLKRGM_CLEAR_NL(MODEM_CLK_HALT_2,RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
      break;

    case CLKRGM_FB_ANT0_SAMP_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, FB_ANT0_CLK_HALT);
      clk_regime_modem_log( CLKRGM_MODEM_LOG_FB_ANT0_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_FB_ANT1_SAMP_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, FB_ANT1_CLK_HALT);
      clk_regime_modem_log( CLKRGM_MODEM_LOG_FB_ANT1_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

  #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_EDGE_TX_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, EDGE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_EDGE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_EDGE_RX_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, EDGE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_EDGE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_GSM_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, GSM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GSM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_GSAC_CLK:
      #ifndef T_MSM6800B
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, GSAC_CLK_HALT);
      #endif
      break;
  #endif

    case CLKRGM_TX_PDM_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_TCXO_PDM_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TCXO_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TCXO_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_MDSP_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MDSP_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MDSP_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_SLEEP_CLK:
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF1_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_SLPFAST_CHIPXN_REF1_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SLPFAST_CHIPXN_REF1_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF2_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_SLPFAST_CHIPXN_REF2_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SLPFAST_CHIPXN_REF2_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

  #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_SLPFAST_GSM_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, SLPFAST_GSM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SLPFAST_GSM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif

    case CLKRGM_SLPFAST_FEE_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, SLPFAST_FEE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SLPFAST_FEE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_BT_CLK:
      #ifdef T_MSM6800B
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT_2, BT_SBI_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_BT_SBI_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      #endif
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, BT_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_BT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_MODEM_SBI_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_SBI_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_MODEM_WEB_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_WEB_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_OFFLINE_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

  #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_GPS_CHIPX8_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, GPS_CHIPX8_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GPS_CHIPX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_GPS_CHIPX16_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT_2, GPS_CHIPX16_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GPS_CHIPX16_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_GPS_CHIPX32_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT_2, GPS_CHIPX32_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GPS_CHIPX32_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif

  #ifdef CLKRGM_INCLUDE_WCDMA
    case CLKRGM_F9_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, F9_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_F9_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif

  #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_MMGPS_CLK:
      break;

    case CLKRGM_MMGPS_ARM_CLK:
      break;
  #endif

    case CLKRGM_TX_DAC_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_PC_DAC_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, PC_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_PC_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_MDM_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT_2, MDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEC_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NLS(CDMA_SAMPSRV_CLK_DISABLE, CLK_OFF);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SAMPSRV_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  
  #ifdef CLKRGM_INCLUDE_HDR
    case CLKRGM_HDR_TDEC_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_SET_NL(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif
  
  #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
    case CLKRGM_TURBO_DEC_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif  
    case CLKRGM_SVD_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(VD_MICRO_CLK_CTL, VD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_VD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_VD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_VD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  
    case CLKRGM_CHIPX8_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV4_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_WEB_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_SBI_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  
    case CLKRGM_CDMA_TX_CLK:

      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV4_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_WEB_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX16_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      #ifdef T_QSC6055
      CLKRGM_W(MOD_MICRO_CLK_CTL,HWIO_RMSK(MOD_MICRO_CLK_CTL));
      #endif
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_SBI_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

  #ifdef CLKRGM_INCLUDE_HDR
    case CLKRGM_CDMA_TX_HDR_CLK:

      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CCDMA_HIPXN_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV4_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_WEB_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX16_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      #ifdef T_QSC6055
      CLKRGM_W(MOD_MICRO_CLK_CTL,HWIO_RMSK(MOD_MICRO_CLK_CTL));
      #endif
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_SBI_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif
  
    case CLKRGM_CDMA_RX_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV4_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_WEB_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_CONFIG_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NLS(CDMA_SAMPSRV_CLK_DISABLE, CLK_OFF);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SAMPSRV_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
      break;
  

    case CLKRGM_RXCHIPX8_CLK:
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV4_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_WEB_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_CONFIG_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(FL_SYM_BUFF_CLK_OVERRIDE, CLK_ENABLE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FL_SYM_BUFF_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
      break;

  
    case CLKRGM_DEM1X_OFFLINE_CLK:      
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEM1X_FFE_RESULT_CLK:   
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEM1X_FFE_ENGINE_CLK:   
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEM1X_FFE_CONFIG_CLK:   
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_CONFIG_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEM1X_CX8_CLK:          
      CLKRGM_SET_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEINT_MICRO_CLK:        
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEINT_MICRO_TASK_RD_CLK:
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEINT_MICRO_TD_RD_CLK:  
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DEINT_MICRO_VD_RD_CLK:  
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_VD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_VD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_FL_SYM_BUFF_CLK:        
      CLKRGM_SET_NL(FL_SYM_BUFF_CLK_OVERRIDE, CLK_ENABLE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FL_SYM_BUFF_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_MOD_CX16_CLK:           
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX16_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_MOD_CX8_CLK:            
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_MOD_PDM_CLK:            
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_DECOB_MICRO_CLK:        
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLKRGM_SAMPSRV_CLK:            
      CLKRGM_CLEAR_NLS(CDMA_SAMPSRV_CLK_DISABLE, CLK_OFF);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SAMPSRV_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

  #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
    case CLKRGM_TD_MICRO_CLK:           
      CLKRGM_SET_NLS(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif
    case CLKRGM_VD_MICRO_CLK:           
      CLKRGM_SET_NLS(VD_MICRO_CLK_CTL, VD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_VD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_MICRO_SW_CLK:
      CLKRGM_SET_NL(GACC_CLK_CTL, GACC_SW_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_SW_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
    #endif

  #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_MICRO_CLK:
      CLKRGM_SET_NL(GACC_CLK_CTL, GACC_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;
  #endif

    case CLKRGM_MODEM_ALL_CLK:
      CLKRGM_CLEAR_MASK(MODEM_CLK_HALT, CLKRGM_MODEM_REG_ALL_MASK);
      
      #ifdef T_MSM6800B
      CLKRGM_CLEAR_MASK(MODEM_CLK_HALT_2, CLKRGM_MODEM_REG_ALL_MASK);
      #endif
      clk_regime_modem_log(CLKRGM_MODEM_LOG_NONE,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);

      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_CONFIG_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);

      CLKRGM_SET_NL(FL_SYM_BUFF_CLK_OVERRIDE, CLK_ENABLE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FL_SYM_BUFF_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);

      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NLS(DEINT_MICRO_CLK_CTL, MICRO_VD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_VD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);

      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX16_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      #ifdef T_QSC6055
      CLKRGM_W(MOD_MICRO_CLK_CTL,HWIO_RMSK(MOD_MICRO_CLK_CTL));
      #endif
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_SET_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
      CLKRGM_SET_NLS(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      #endif
      CLKRGM_SET_NLS(VD_MICRO_CLK_CTL, VD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_VD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);

      #ifdef CLKRGM_INCLUDE_GPS
      CLKRGM_SET_NL(GACC_CLK_CTL, GACC_SW_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_SW_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      CLKRGM_SET_NL(GACC_CLK_CTL, GACC_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);

      CLKRGM_CLEAR_NL(CDMA_SAMPSRV_CLK_DISABLE, CLK_OFF);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SAMPSRV_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      #endif

      #ifdef T_MSM6800B
      CLKRGM_CLEAR_NL(MODEM_CLK_HALT_2, MDM_CLK_HALT);
      #else
      CLKRGM_CLEAR_NL(MODEM_CLK_MISC_CTL, MDM_CLK_HALT);
      #endif
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);

      HWIO_OUT(MODEM_CLK_MISC_CTL, HWIO_FMSK(MODEM_CLK_MISC_CTL, MDSP_CLK_1X_MODE));

      break;

    case CLKRGM_OBSOLETE_CLK:
      break;

    default:
      /* Error: Clock branch of out range */
      ERR("clk_regime_on: Clock branch out of range (%d)", clk, 0, 0);
      break;
  }

  /*-----------------------------------------------------------------------*/
  /* Unlock interrupts                                                     */
  /*-----------------------------------------------------------------------*/
  INTFREE_SAV (sav_var);

} /* clk_regime_on */

/*==========================================================================

  FUNCTION      CLK_REGIME_OFF

  DESCRIPTION   This function turns off a specific clock without disabling
                any source clock (e.g., without turning off the Modem PLL).
                Set the halt bit or clear the enable bit as appropriate for
                the clock.
                
                CHIPXN_DIV2, RXF_SAMP, and RXF_SAMP_REF are only toggled
                if the caller is clk_regime_rxf_request(). Any other caller
                has no effect.
  
  PARAMETERS    clk  - Clock to turn off.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The specified clock branch is turned off.

==========================================================================*/

void clk_regime_off (clkrgm_modem_clk_type clk)
{
  uint32 sav_var;

  /*-----------------------------------------------------------------------*/
  /* Interrupts are locked to prevent race conditions                      */
  /*-----------------------------------------------------------------------*/
  INTLOCK_SAV (sav_var);

  switch (clk)
  {
    case CLKRGM_CHIPXN_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, CDMA_CHIPXN_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_ANT_SAMP_CHIPXN_CLK:
    case CLKRGM_CHIPXN_DIV2_CLK:
      if (clkrgm_manages_rxf == TRUE)
      {
        CLKRGM_SET_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      }
      break;

    case CLKRGM_CHIPXN_DIV4_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV4_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV4_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

   case CLKRGM_RXF_SAMP_CLK:
      if (clkrgm_manages_rxf == TRUE)
      {
        CLKRGM_SET_NL(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT);    
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      }
     break;

    case CLKRGM_RXF_SAMP_REF_CLK:
      if (clkrgm_manages_rxf == TRUE)
      {
        CLKRGM_SET_NL(MODEM_CLK_HALT_2,RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      }
      break;

    case CLKRGM_FB_ANT0_SAMP_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, FB_ANT0_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FB_ANT0_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_FB_ANT1_SAMP_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, FB_ANT1_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FB_ANT1_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

  #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_EDGE_TX_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, EDGE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_EDGE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_EDGE_RX_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, EDGE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_EDGE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_GSM_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, GSM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GSM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_GSAC_CLK:
      #ifndef T_MSM6800B
      CLKRGM_SET_NL(MODEM_CLK_HALT, GSAC_CLK_HALT);
      #endif
      break;
  #endif

    case CLKRGM_TX_PDM_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_TCXO_PDM_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, TCXO_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TCXO_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_MDSP_CLK:
#if 0 /* Always Enable the MDSP Clock for initail release*/
#ifndef T_RUMI
      CLKRGM_SET_NL(MODEM_CLK_HALT, MDSP_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MDSP_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
#endif
#endif
      break;

    case CLKRGM_SLEEP_CLK:
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF1_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, CDMA_SLPFAST_CHIPXN_REF1_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SLPFAST_CHIPXN_REF1_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF2_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, CDMA_SLPFAST_CHIPXN_REF2_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SLPFAST_CHIPXN_REF2_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

  #ifdef FEATURE_INCLUDE_GSM
#error code not present
  #endif

    case CLKRGM_SLPFAST_FEE_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, SLPFAST_FEE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SLPFAST_FEE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_BT_CLK:
      #ifdef T_MSM6800B
      CLKRGM_SET_NL(MODEM_CLK_HALT_2, BT_SBI_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_BT_SBI_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      #endif
      CLKRGM_SET_NL(MODEM_CLK_HALT, BT_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_BT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_MODEM_SBI_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, MODEM_SBI_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_SBI_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_MODEM_WEB_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, MODEM_WEB_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MODEM_WEB_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_OFFLINE_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, OFFLINE_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

  #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_GPS_CHIPX8_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT, GPS_CHIPX8_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GPS_CHIPX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_GPS_CHIPX16_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT_2, GPS_CHIPX16_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GPS_CHIPX16_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      break;

    case CLKRGM_GPS_CHIPX32_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, GACC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT_2, GPS_CHIPX32_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GPS_CHIPX32_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      break;
  #endif

  #ifdef CLKRGM_INCLUDE_WCDMA
    case CLKRGM_F9_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, F9_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_F9_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  #endif

  #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_MMGPS_CLK:
      break;

    case CLKRGM_MMGPS_ARM_CLK:
      break;
  #endif

    case CLKRGM_TX_DAC_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_PC_DAC_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, PC_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_PC_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_MDM_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT_2, MDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEC_CLK:
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  
  #ifdef CLKRGM_INCLUDE_HDR
    case CLKRGM_HDR_TDEC_CLK:
      CLKRGM_CLEAR_NLS(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  #endif
  #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
    case CLKRGM_TURBO_DEC_CLK:
      CLKRGM_CLEAR_NLS(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  #endif  
    case CLKRGM_SVD_CLK:
      CLKRGM_CLEAR_NLS(VD_MICRO_CLK_CTL, VD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_VD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_VD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_VD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  
    case CLKRGM_CHIPX8_CLK:
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  
    case CLKRGM_CDMA_TX_CLK:

      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      break;

  #ifdef CLKRGM_INCLUDE_HDR
    case CLKRGM_CDMA_TX_HDR_CLK:
      if (!clkrgm_cdma_tx_1x)
      {
      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_DAC_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_DAC_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_SET_NL(MODEM_CLK_HALT, TX_PDM_CLK_HALT);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TX_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      }

      clkrgm_cdma_tx_hdr = FALSE;

      break;
  #endif
  
    case CLKRGM_CDMA_RX_CLK:
      CLKRGM_CLEAR_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_CONFIG_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  
    case CLKRGM_RXCHIPX8_CLK:
      CLKRGM_CLEAR_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(FL_SYM_BUFF_CLK_OVERRIDE, CLK_ENABLE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FL_SYM_BUFF_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

  
    case CLKRGM_DEM1X_OFFLINE_CLK:      
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEM1X_FFE_RESULT_CLK:   
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEM1X_FFE_ENGINE_CLK:   
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEM1X_FFE_CONFIG_CLK:   
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEM1X_CX8_CLK:          
      CLKRGM_CLEAR_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEINT_MICRO_CLK:        
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEINT_MICRO_TASK_RD_CLK:
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEINT_MICRO_TD_RD_CLK:  
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DEINT_MICRO_VD_RD_CLK:  
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_VD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_VD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_FL_SYM_BUFF_CLK:        
      CLKRGM_CLEAR_NL(FL_SYM_BUFF_CLK_OVERRIDE, CLK_ENABLE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FL_SYM_BUFF_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_MOD_CX16_CLK:           
      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX16_CLK,           
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_MOD_CX8_CLK:            
      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX8_CLK,           
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_MOD_PDM_CLK:            
      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_DECOB_MICRO_CLK:        
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_SAMPSRV_CLK:            
      CLKRGM_SET_NLS(CDMA_SAMPSRV_CLK_DISABLE, CLK_OFF);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SAMPSRV_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
    case CLKRGM_TD_MICRO_CLK:           
      CLKRGM_CLEAR_NLS(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  #endif
    case CLKRGM_VD_MICRO_CLK:           
      CLKRGM_CLEAR_NLS(VD_MICRO_CLK_CTL, VD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_VD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

  #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_MICRO_SW_CLK:
      CLKRGM_CLEAR_NL(GACC_CLK_CTL, GACC_SW_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_SW_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLKRGM_GACC_MICRO_CLK:
      CLKRGM_CLEAR_NL(GACC_CLK_CTL, GACC_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;
  #endif

    case CLKRGM_MODEM_ALL_CLK:
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, OFFLINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_OFFLINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_RESULT_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_RESULT_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_ENGINE_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_OFFLINE_CLK_CTL, FFE_CONFIG_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_FFE_ENGINE_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(DEM1X_MICRO_CX8_CLK_CTL, CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEM1X_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      CLKRGM_CLEAR_NL(FL_SYM_BUFF_CLK_OVERRIDE, CLK_ENABLE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_FL_SYM_BUFF_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TASK_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TASK_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_TD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_TD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NLS(DEINT_MICRO_CLK_CTL, MICRO_VD_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DEINT_MICRO_VD_RD_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_CX16_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX16_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_CX8_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_CX8_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(MOD_MICRO_CLK_CTL, MOD_PDM_CLK_OVERRIDE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MOD_PDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      #ifdef T_QSC6055
      CLKRGM_W(MOD_MICRO_CLK_CTL,0x0);
      #endif
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_CLK_EN);
      CLKRGM_CLEAR_NLS(DECOB_MICRO_CLK_CTL, DECOB_MICRO_RD_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_DECOB_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
  #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
      CLKRGM_CLEAR_NLS(TD_MICRO_CLK_CTL, TD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_TD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
  #endif
      CLKRGM_CLEAR_NLS(VD_MICRO_CLK_CTL, VD_MICRO_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_VD_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      #ifdef CLKRGM_INCLUDE_GPS
      CLKRGM_CLEAR_NL(GACC_CLK_CTL, GACC_SW_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_SW_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      CLKRGM_CLEAR_NL(GACC_CLK_CTL, GACC_CLK_EN);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_GACC_MICRO_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      CLKRGM_SET_NLS(CDMA_SAMPSRV_CLK_DISABLE, CLK_OFF);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_SAMPSRV_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      #endif

      #ifdef T_MSM6800B
      CLKRGM_SET_NL(MODEM_CLK_HALT_2, MDM_CLK_HALT);
        CLKRGM_SET_MASK(MODEM_CLK_HALT, CLKRGM_MODEM_REG_ALL_MASK);
        CLKRGM_SET_MASK(MODEM_CLK_HALT_2, CLKRGM_MODEM_REG_ALL_MASK);
      #else
      CLKRGM_SET_NL(MODEM_CLK_MISC_CTL, MDM_CLK_HALT);
        CLKRGM_SET_MASK(MODEM_CLK_HALT, CLKRGM_MODEM_REG_ALL_MASK);
      #endif
      clk_regime_modem_log(CLKRGM_MODEM_LOG_MDM_CLK,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      clk_regime_modem_log(CLKRGM_MODEM_LOG_NONE,
                           CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);

      break;

    case CLKRGM_OBSOLETE_CLK:
      break;

    default:
      /* Error: Clock branch of out range */
      ERR("clk_regime_off: Clock branch out of range (%d)", clk, 0, 0);
      break;
  }

  /*-----------------------------------------------------------------------*/
  /* Unlock interrupts                                                     */
  /*-----------------------------------------------------------------------*/
  INTFREE_SAV (sav_var);

} /* clk_regime_off */

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

void clk_regime_lcg_on (clk_regime_lcg_type item)
{
  INTLOCK();

  switch (item)
  {
    case CLKRGM_LCG_DEM1X_FFE_RESULT:
      CLKRGM_SET_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_RESULT_LCG_EN);
      break;

    case CLKRGM_LCG_DEM1X_FFE_ENGINE:
      CLKRGM_SET_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_ENGINE_LCG_EN);
      break;

    case CLKRGM_LCG_DEM1X_FFE_CONFIG:
      CLKRGM_SET_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_CONFIG_LCG_EN);
      break;

    case CLKRGM_LCG_DEINT_TASK:
      CLKRGM_SET_NLS(DEINT_HW_CLK_CTL, HW_TASK_RD_LCG_EN);
      break;

    case CLKRGM_LCG_DEINT_TD_RD:
      CLKRGM_SET_NLS(DEINT_HW_CLK_CTL, HW_TD_RD_LCG_EN);
      break;

    case CLKRGM_LCG_DEINT_VD_RD:
      CLKRGM_SET_NLS(DEINT_HW_CLK_CTL, HW_VD_RD_LCG_EN);
      break;

    case CLKRGM_LCG_FL_SYM_BUFF:
      CLKRGM_SET_NL(FL_SYM_BUFF_CLK_CTL, CLK_LCG_EN);
      break;

    case CLKRGM_LCG_MOD_CX16:
      CLKRGM_SET_NL(MOD_HW_CLK_CTL, MOD_CX16_HW_LCG_ENA);
      break;

    case CLKRGM_LCG_MOD_CX8:
      CLKRGM_SET_NL(MOD_HW_CLK_CTL, MOD_CX8_HW_LCG_ENA);
      break;

    case CLKRGM_LCG_MOD_PDM:
      CLKRGM_SET_NL(MOD_HW_CLK_CTL, MOD_PDM_HW_LCG_ENA);
      break;

    case CLKRGM_LCG_DECOB:
      CLKRGM_SET_NLS(DECOB_HW_CLK_CTL, DECOB_HW_LCG_EN);
      break;

    #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
    case CLKRGM_LCG_TD:
      CLKRGM_SET_NLS(TD_HW_CLK_CTL, TD_HW_LCG_EN);
      break;
    #endif
    case CLKRGM_LCG_VD:
      CLKRGM_SET_NLS(VD_HW_CLK_CTL, VD_HW_LCG_EN);
      break;

#ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_LCG_GACC:
      CLKRGM_SET_NL(GACC_CLK_CTL, GACC_HW_LCG_EN);
      break;
#endif

    case CLKRGM_LCG_ALL:
      CLKRGM_SET_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_RESULT_LCG_EN);
      CLKRGM_SET_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_ENGINE_LCG_EN);
      CLKRGM_SET_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_CONFIG_LCG_EN);
      CLKRGM_SET_NLS(DEINT_HW_CLK_CTL, HW_TASK_RD_LCG_EN);
      CLKRGM_SET_NLS(DEINT_HW_CLK_CTL, HW_TD_RD_LCG_EN);
      CLKRGM_SET_NLS(DEINT_HW_CLK_CTL, HW_VD_RD_LCG_EN);
      CLKRGM_SET_NL(FL_SYM_BUFF_CLK_CTL, CLK_LCG_EN);
      CLKRGM_SET_NL(MOD_HW_CLK_CTL, MOD_CX16_HW_LCG_ENA);
      CLKRGM_SET_NL(MOD_HW_CLK_CTL, MOD_CX8_HW_LCG_ENA);
      CLKRGM_SET_NL(MOD_HW_CLK_CTL, MOD_PDM_HW_LCG_ENA);
      CLKRGM_SET_NLS(DECOB_HW_CLK_CTL, DECOB_HW_LCG_EN);
      #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
      CLKRGM_SET_NLS(TD_HW_CLK_CTL, TD_HW_LCG_EN);
	  #endif
      CLKRGM_SET_NLS(VD_HW_CLK_CTL, VD_HW_LCG_EN);
#ifdef CLKRGM_INCLUDE_GPS
      CLKRGM_SET_NL(GACC_CLK_CTL, GACC_HW_LCG_EN);
#endif
      break;

    default:
      /* Enable not supported for this item. */
      ERR("clk_regime_lcg_on: Clock branch out of range (%d)", item, 0, 0);
      break;
  }

  INTFREE();
} /* clk_regime_lcg_on */

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

void clk_regime_lcg_off (clk_regime_lcg_type item)
{
  INTLOCK();

  switch (item)
  {
    case CLKRGM_LCG_DEM1X_FFE_RESULT:
      CLKRGM_CLEAR_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_RESULT_LCG_EN);
      break;

    case CLKRGM_LCG_DEM1X_FFE_ENGINE:
      CLKRGM_CLEAR_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_ENGINE_LCG_EN);
      break;

    case CLKRGM_LCG_DEM1X_FFE_CONFIG:
      CLKRGM_CLEAR_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_CONFIG_LCG_EN);
      break;

    case CLKRGM_LCG_DEINT_TASK:
      CLKRGM_CLEAR_NLS(DEINT_HW_CLK_CTL, HW_TASK_RD_LCG_EN);
      break;

    case CLKRGM_LCG_DEINT_TD_RD:
      CLKRGM_CLEAR_NLS(DEINT_HW_CLK_CTL, HW_TD_RD_LCG_EN);
      break;

    case CLKRGM_LCG_DEINT_VD_RD:
      CLKRGM_CLEAR_NLS(DEINT_HW_CLK_CTL, HW_VD_RD_LCG_EN);
      break;

    case CLKRGM_LCG_FL_SYM_BUFF:
      CLKRGM_CLEAR_NL(FL_SYM_BUFF_CLK_CTL, CLK_LCG_EN);
      break;

    case CLKRGM_LCG_MOD_CX16:
      CLKRGM_CLEAR_NL(MOD_HW_CLK_CTL, MOD_CX16_HW_LCG_ENA);
      break;

    case CLKRGM_LCG_MOD_CX8:
      CLKRGM_CLEAR_NL(MOD_HW_CLK_CTL, MOD_CX8_HW_LCG_ENA);
      break;

    case CLKRGM_LCG_MOD_PDM:
      CLKRGM_CLEAR_NL(MOD_HW_CLK_CTL, MOD_PDM_HW_LCG_ENA);
      break;

    case CLKRGM_LCG_DECOB:
      CLKRGM_CLEAR_NLS(DECOB_HW_CLK_CTL, DECOB_HW_LCG_EN);
      break;

    #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
    case CLKRGM_LCG_TD:
      CLKRGM_CLEAR_NLS(TD_HW_CLK_CTL, TD_HW_LCG_EN);
      break;
    #endif
    case CLKRGM_LCG_VD:
      CLKRGM_CLEAR_NLS(VD_HW_CLK_CTL, VD_HW_LCG_EN);
      break;

#ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_LCG_GACC:
      CLKRGM_CLEAR_NL(GACC_CLK_CTL, GACC_HW_LCG_EN);
      break;
#endif

    case CLKRGM_LCG_ALL:
      CLKRGM_CLEAR_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_RESULT_LCG_EN);
      CLKRGM_CLEAR_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_ENGINE_LCG_EN);
      CLKRGM_CLEAR_NLS(DEM1X_HW_OFFLINE_CLK_CTL, FFE_CONFIG_LCG_EN);
      CLKRGM_CLEAR_NLS(DEINT_HW_CLK_CTL, HW_TASK_RD_LCG_EN);
      CLKRGM_CLEAR_NLS(DEINT_HW_CLK_CTL, HW_TD_RD_LCG_EN);
      CLKRGM_CLEAR_NLS(DEINT_HW_CLK_CTL, HW_VD_RD_LCG_EN);
      CLKRGM_CLEAR_NL(FL_SYM_BUFF_CLK_CTL, CLK_LCG_EN);
      CLKRGM_CLEAR_NL(MOD_HW_CLK_CTL, MOD_CX16_HW_LCG_ENA);
      CLKRGM_CLEAR_NL(MOD_HW_CLK_CTL, MOD_CX8_HW_LCG_ENA);
      CLKRGM_CLEAR_NL(MOD_HW_CLK_CTL, MOD_PDM_HW_LCG_ENA);
      CLKRGM_CLEAR_NLS(DECOB_HW_CLK_CTL, DECOB_HW_LCG_EN);
      #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
      CLKRGM_CLEAR_NLS(TD_HW_CLK_CTL, TD_HW_LCG_EN);
	  #endif
      CLKRGM_CLEAR_NLS(VD_HW_CLK_CTL, VD_HW_LCG_EN);
#ifdef CLKRGM_INCLUDE_GPS
      CLKRGM_CLEAR_NL(GACC_CLK_CTL, GACC_HW_LCG_EN);
#endif
      break;

    default:
      /* Disable not supported for this item. */
      ERR("clk_regime_lcg_off: Clock branch out of range (%d)", item, 0, 0);
      break;
  }

  INTFREE();

} /* clk_regime_lcg_off */

/*===========================================================================

FUNCTION        CLK_REGIME_ENABLE_BT_PCM_CLK

DESCRIPTION     This function will enable/disable the blue tooth pcm clk.

DEPENDENCIES    Successful enabling depends on the source clock being
                oscillating and stable.

RETURN VALUE    None.

SIDE EFFECTS    MSM circuits are enabled/disabled.

===========================================================================*/

void clk_regime_enable_bt_pcm_clk (boolean enable)
{
  /* NOT SUPPORTED. */
}
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

void clk_regime_sel_clk_div (
  clkrgm_modem_clk_type   clk,
  uint32                  divider)
{
  boolean is_on = FALSE;
  uint32  isave;

  /* ------------------------------------------------------------------------
     Lock interrupts around this operation since the corresponding clock
     may be turned off.
  ------------------------------------------------------------------------ */
  INTLOCK_SAV(isave);

  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     changing the clock divider.
  ------------------------------------------------------------------------ */
  is_on = clk_regime_is_on(clk);

  if (is_on == TRUE)
  {
    clk_regime_off(clk);
  }

  /* ------------------------------------------------------------------------
     Insert an unconditional delay since the caller may have disabled the
     clock regime immediately prior to calling this function.
  ------------------------------------------------------------------------ */
  /* CLK_OFF_A and CLK_OFF_B represent the true state of the clock
     with no cycle delay.  No need to wait 3 clock cycles.
    if (clk != CLKRGM_RXF_SAMP_REF_CLK)
      clk_busy_wait(CLKRGM_MODEM_DISABLE_TIME_US);
  */

  /* ------------------------------------------------------------------------
     Switch the clock divider.
  ------------------------------------------------------------------------ */
  switch(clk)
  {
    /* Receiver chain receiver sample clock. */
    case CLKRGM_ANT_SAMP_CHIPXN_CLK:
      CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, ANT_SAMP_CHIPXN_DIV_SEL, divider);
      break;

   #ifdef T_QSC6055
      /* Explicitly manage the on/off directly through clk_halt registers/bits
       * since RXF_SAMP_REF won't be toggled by call to clk_regime_on/off() 
       * unless called from clk_regime_rxf_request().
       */
   case CLKRGM_RXF_SAMP_REF_CLK:
      if (is_on)
      {
        CLKRGM_SET(MODEM_CLK_HALT_2, RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
        clk_busy_wait(1);
      }
      CLKRGM_W_MASKED(MODEM_RXF_SAMP_REF_CLK_CTL,CLK_DIV, divider);
      if (is_on)
      {
        CLKRGM_CLEAR(MODEM_CLK_HALT_2, RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
      break;
   #endif
   
   #ifdef CLKRGM_INCLUDE_GPS
   case CLKRGM_GACC_CLK:
      CLKRGM_W_MASKED(MODEM_GACC_CLK_CTL,CLK_SRC_DIV, divider);
      break;

   case CLKRGM_CHIPXN_CLK:
      CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_CTL,CDMA_CLK_SRC_DIV, divider);
      break;
   #endif /* CLKRGM_INCLUDE_GPS */

   #ifdef CLKRGM_INCLUDE_WCDMA
   case WCDMA_CHIPXN_CLK:
      CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_CTL,WCDMA_CLK_SRC_DIV, divider);
      break;
   #endif /* CLKRGM_INCLUDE_WCDMA */

   case CLKRGM_OFFLINE_CLK:
      CLKRGM_W_MASKED(MODEM_OFFLINE_CLK_CTL, CLK_SRC_DIV, divider);
      break;

   #ifdef CLKRGM_INCLUDE_WCDMA
   case CLKRGM_F9_CLK:
      CLKRGM_W_MASKED(MODEM_F9_CLK_CTL, F9_CLK_DIV_SEL, divider);
      break;
   #endif /* CLKRGM_INCLUDE_WCDMA */
   
   case CLKRGM_MODEM_WEB_CLK:
      CLKRGM_W_MASKED(MODEM_WEB_CLK_CTL, WEB_CLK_DIV_SEL, divider);
      break;

    /* Unsupported clock regimes. */
    default:
      /* Option not supported. */
      /* Error: Clock branch of out range */
      ERR("clk_regime_sel_clk_div: Clock branch out of range (%d)", clk, 0, 0);
      break;
  }

  /* ------------------------------------------------------------------------
     If the clock regime was enabled, then re-enable the clock regime after
     switching the source.
  ------------------------------------------------------------------------ */
  if (is_on == TRUE)
  {
    clk_regime_on(clk);
  }

  /* ------------------------------------------------------------------------
     Unlock interrupts now that the selected clock is back on.
  ------------------------------------------------------------------------ */
  INTFREE_SAV(isave);

} /* clk_regime_sel_clk_div */

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

void clk_regime_sel_clk_inv (clkrgm_modem_clk_type  clk,
                             boolean                invert
                            )
{
  uint32  isave = 0;
  boolean is_on = FALSE;

  /* ------------------------------------------------------------------------
     Lock interrupts around this operation since the corresponding clock
     may be turned off.
  ------------------------------------------------------------------------ */
  INTLOCK_SAV(isave);

  /* ------------------------------------------------------------------------
     If the clock is enabled, then disable the clock before
     changing the clock polarity.
  ------------------------------------------------------------------------ */
  is_on = clk_regime_is_on(clk);

  if (is_on == TRUE)
  {
    clk_regime_off(clk);
  }

  /* ------------------------------------------------------------------------
     Insert an unconditional delay since the caller may have disabled the
     clock regime immediately prior to calling this function.
  ------------------------------------------------------------------------ */
  /* CLK_OFF_A and CLK_OFF_B represent the true state of the clock
     with no cycle delay.  No need to wait 3 clock cycles.
    if (clk != CLKRGM_RXF_SAMP_REF_CLK)
      clk_busy_wait(CLKRGM_MODEM_DISABLE_TIME_US);
  */

  /* ------------------------------------------------------------------------
     Switch the clock polarity.
  ------------------------------------------------------------------------ */

  switch (clk)
  {
    case CLKRGM_CHIPXN_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, CDMA_CHIPXN_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, CDMA_CHIPXN_CLK_INV);
      break;

     /* For CHIPXN_DIV2, explicitly manage the on/off by direct
       * access to clk_halt registers/bits because the api functions clk_regime_off/on
       * are modified to disallow any operation on these clocks except by 
       * clk_regime_rxf_request().
       */
    case CLKRGM_CHIPXN_DIV2_CLK:
      if (is_on)
      {
        CLKRGM_SET_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      }
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, CDMA_CHIPXN_DIV2_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, CDMA_CHIPXN_DIV2_CLK_INV);
      if (is_on)
      {
        CLKRGM_CLEAR_NL(MODEM_CLK_HALT, CDMA_CHIPXN_DIV2_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_CHIPXN_DIV2_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
      break;

    case CLKRGM_CHIPXN_DIV4_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, CDMA_CHIPXN_DIV4_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, CDMA_CHIPXN_DIV4_CLK_INV);
      break;

     /* For RXF_SAMP and RXF_SAMP_REF, explicitly manage the on/off by direct
       * access to clk_halt registers/bits because the api functions clk_regime_off/on
       * are modified to disallow any operation on these clocks except by 
       * clk_regime_rxf_request().
       */
    case CLKRGM_RXF_SAMP_CLK:
      if (is_on)
      {
        CLKRGM_SET_NL(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
        clk_busy_wait(1);
      }
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, RXF_SAMP_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, RXF_SAMP_CLK_INV);
      if (is_on)
      {
        CLKRGM_CLEAR_NL(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
      break;

    case CLKRGM_RXF_SAMP_REF_CLK:
      if (is_on)
      {
        CLKRGM_SET_NL(MODEM_CLK_HALT_2, RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      }
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV_2, RXF_SAMP_REF_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV_2, RXF_SAMP_REF_CLK_INV);
      if (is_on)
      {
        CLKRGM_CLEAR_NL(MODEM_CLK_HALT_2, RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
      break;


    #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_EDGE_TX_CLK:
  	  #ifndef T_MSM6800B
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, EDGE_TX_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, EDGE_TX_CLK_INV);
      #else
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, EDGE_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, EDGE_CLK_INV);
      #endif
      break;

    case CLKRGM_EDGE_RX_CLK:
  	  #ifndef T_MSM6800B
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, EDGE_RX_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, EDGE_RX_CLK_INV);
      #else
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, EDGE_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, EDGE_CLK_INV);
      #endif
      break;

    case CLKRGM_GSM_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, GSM_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, GSM_CLK_INV);
      break;

    #ifndef T_MSM6800B
    case CLKRGM_GSAC_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, GSAC_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, GSAC_CLK_INV);
      break;
    #endif
    #endif /* CLKRGM_INCLUDE_GSM */
    case CLKRGM_TX_PDM_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, TX_PDM_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, TX_PDM_CLK_INV);
      break;

    case CLKRGM_TCXO_PDM_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, TCXO_PDM_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, TCXO_PDM_CLK_INV);
      break;

    case CLKRGM_MDSP_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, MDSP_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, MDSP_CLK_INV);
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF1_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, CDMA_SLPFAST_CHIPXN_REF1_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, CDMA_SLPFAST_CHIPXN_REF1_INV);
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF2_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, CDMA_SLPFAST_CHIPXN_REF2_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, CDMA_SLPFAST_CHIPXN_REF2_INV);
      break;

    #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_SLPFAST_GSM_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, SLPFAST_GSM_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, SLPFAST_GSM_CLK_INV);
      break;
    #endif /* CLKRGM_INCLUDE_GSM */

    case CLKRGM_SLPFAST_FEE_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, SLPFAST_FEE_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, SLPFAST_FEE_CLK_INV);
      break;

    case CLKRGM_BT_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, BT_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, BT_CLK_INV);
      break;

    case CLKRGM_MODEM_SBI_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, MODEM_SBI_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, MODEM_SBI_CLK_INV);
      break;

    case CLKRGM_MODEM_WEB_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, MODEM_WEB_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, MODEM_WEB_CLK_INV);
      break;

    case CLKRGM_OFFLINE_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, OFFLINE_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, OFFLINE_CLK_INV);
      break;

    #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, GACC_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, GACC_CLK_INV);
      break;

    case CLKRGM_GPS_CHIPX8_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, GPS_CHIPX8_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, GPS_CHIPX8_CLK_INV);
      break;
    #endif /* CLKRGM_INCLUDE_GPS */

    #ifdef CLKRGM_INCLUDE_WCDMA
    case CLKRGM_F9_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, F9_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, F9_CLK_INV);
      break;
    #endif /* CLKRGM_INCLUDE_WCDMA */

    case CLKRGM_TX_DAC_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, TX_DAC_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, TX_DAC_CLK_INV);
      break;

    case CLKRGM_PC_DAC_CLK:
      if (invert)
        CLKRGM_SET_NL(MODEM_CLK_INV, PC_DAC_CLK_INV);
      else
        CLKRGM_CLEAR_NL(MODEM_CLK_INV, PC_DAC_CLK_INV);
      break;

    default:
      /* Error: Clock branch of out range */
      ERR("clk_regime_sel_clk_inv: Clock branch out of range (%d)", clk, 0, 0);
      break;
  }

  /* ------------------------------------------------------------------------
     If the clock was enabled, then re-enable the clock regime.
  ------------------------------------------------------------------------ */
  if (is_on == TRUE)
  {
    clk_regime_on(clk);
  }

  /* ------------------------------------------------------------------------
     Unlock interrupts now that the selected clock is back on.
  ------------------------------------------------------------------------ */
  INTFREE_SAV(isave);

} /* clk_regime_sel_clk_inv */

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

void clk_regime_config_cdma_clocks (clkrgm_rx_chain_type rx_chain)
{
  boolean rxf0_is_on;
  boolean rxf1_is_on;
  uint32 isave;

  /* process only if mode has to change */
  if (rx_chain == CLKRGM_RX_CHAIN_0 && clkrgm_modem.clk_mode[rx_chain] == CLKRGM_MODEM_CLK_MODE_CDMA)
  {
    return;
  }

  if (rx_chain == CLKRGM_RX_CHAIN_1 && clkrgm_modem.clk_mode[rx_chain] == CLKRGM_MODEM_CLK_MODE_CDMA)
  {
    return;
  }

  if (rx_chain == CLKRGM_RX_CHAIN_BOTH &&
      clkrgm_modem.clk_mode[CLKRGM_RX_CHAIN_0] == CLKRGM_MODEM_CLK_MODE_CDMA &&
      clkrgm_modem.clk_mode[CLKRGM_RX_CHAIN_1] == CLKRGM_MODEM_CLK_MODE_CDMA
     )
  {
    return;
  }

  /* ------------------------------------------------------------------------
     Lock interrupts.
  ------------------------------------------------------------------------ */

  INTLOCK_SAV(isave);

  /* ------------------------------------------------------------------------
     Determine which clock regimes are currently enabled.
  ------------------------------------------------------------------------ */

  /* Receiver front end clocks. */
  rxf0_is_on = clk_regime_is_on(CLKRGM_RXF0_FRONT_CLK);
  rxf1_is_on = clk_regime_is_on(CLKRGM_RXF1_FRONT_CLK);

  /* ------------------------------------------------------------------------
     Disable the relevent clock regimes, if necessary.
  ------------------------------------------------------------------------ */

  /* Receiver chain 0 clocks. */
  if ((rx_chain == CLKRGM_RX_CHAIN_0) || (rx_chain == CLKRGM_RX_CHAIN_BOTH))
  {
    if (rxf0_is_on == TRUE) clk_regime_rxf_request(CLKRGM_RXF_RF_0, CLKRGM_RXF_DISABLE);
  }

  /* Receiver chain 1 clocks. */
  if ((rx_chain == CLKRGM_RX_CHAIN_1) || (rx_chain == CLKRGM_RX_CHAIN_BOTH))
  {
    if (rxf1_is_on == TRUE) clk_regime_rxf_request(CLKRGM_RXF_RF_1, CLKRGM_RXF_DISABLE);
  }

  /* ------------------------------------------------------------------------
     Wait for clock output to stop before switching the clock sources.
  ------------------------------------------------------------------------ */

  clk_busy_wait(1);

  /* ------------------------------------------------------------------------
     Select the proper clock sources.
  ------------------------------------------------------------------------ */

  /* Receiver chain 0 clocks. */
  if ((rx_chain == CLKRGM_RX_CHAIN_0) || (rx_chain == CLKRGM_RX_CHAIN_BOTH))
  {

    clk_regime_sel_clk_inv(CLKRGM_RXF0_SAMP_CLK, FALSE);

    CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_MISC_CTL, CDMA_CHIPXN_SRC_SEL,
                    CLK_RGM_SRC_SEL_CHIPXN_CHIPXN_REV_DIV1_V);

#if 0  /* cryan -- rxbrlh0013 won't acquire if 0 written here */ 
    CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_MISC_CTL, RX_SAMPLE_RAW_SEL,
                    CLK_RGM_SRC_SEL_RX_SAMPLE_RAW_ANT0_FB_V);
#endif

#ifdef T_QSC6055
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TX_DAC_CHIPXN_SEL,
                    CLK_RGM_SRC_SEL_TX_DAC_CHIPXN_DIV1_V);
#else
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TX_DAC_CHIPXN_SEL,
                    CLK_RGM_SRC_SEL_TX_DAC_CHIPXN_DIV2_V);
#endif

    /* set up source for TX_PDM and TCXO_PDM */
#ifdef T_RUMI
#error code not present
#else
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TX_PDM_CLK_SRC_SEL,
                    CLK_RGM_SRC_SEL_TX_PDM_CHIPXN_DIV2_V);
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TCXO_PDM_CLK_SRC_SEL,
                    CLK_RGM_SRC_SEL_TCXO_PDM_TCXO_V);
#endif

}

  /* Receiver chain 1 clocks. */
  if ((rx_chain == CLKRGM_RX_CHAIN_1) || (rx_chain == CLKRGM_RX_CHAIN_BOTH))
  {

    clk_regime_sel_clk_inv(CLKRGM_RXF1_SAMP_CLK, FALSE);

    CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_MISC_CTL, CDMA_CHIPXN_SRC_SEL,
                    CLK_RGM_SRC_SEL_CHIPXN_CHIPXN_REV_DIV1_V);
#if 0  /* cryan -- rxbrlh0013 won't acquire if 0 written here */ 
    CLKRGM_W_MASKED(MODEM_CROSSBAR_CTL, RX_SAMPLE_RAW_SEL,
                    CLK_RGM_SRC_SEL_RX_SAMPLE_RAW_ANT0_FB_V);
#endif
#ifdef T_QSC6055
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TX_DAC_CHIPXN_SEL,
                    CLK_RGM_SRC_SEL_TX_DAC_CHIPXN_DIV1_V);
#else
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TX_DAC_CHIPXN_SEL,
                    CLK_RGM_SRC_SEL_TX_DAC_CHIPXN_DIV2_V);
#endif

    /* set up source for TX_PDM and TCXO_PDM */
#ifdef T_RUMI
#error code not present
#else
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TX_PDM_CLK_SRC_SEL,
                    CLK_RGM_SRC_SEL_TX_PDM_CHIPXN_DIV2_V);
    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, TCXO_PDM_CLK_SRC_SEL,
                    CLK_RGM_SRC_SEL_TCXO_PDM_TCXO_V);
#endif
  
  }

  /* ------------------------------------------------------------------------
     Reset the receiver clock crossbar logic.
  ------------------------------------------------------------------------ */

  // Cross bar shouldn't be reset at this point since it causes
  // clocks to stop while the reset is asserted.
  // clk_regime_misc_reset(CLK_RGM_MISC_RST_RX_CROSSBAR);

  /* ------------------------------------------------------------------------
     Enable the relevent clock regimes, if necessary.
  ------------------------------------------------------------------------ */

  /* Receiver chain 0 clocks. */
  if ((rx_chain == CLKRGM_RX_CHAIN_0) || (rx_chain == CLKRGM_RX_CHAIN_BOTH))
  {
    if (rxf0_is_on == TRUE) clk_regime_rxf_request(CLKRGM_RXF_RF_0, CLKRGM_RXF_ENABLE);
  }

  /* Receiver chain 1 clocks. */
  if ((rx_chain == CLKRGM_RX_CHAIN_1) || (rx_chain == CLKRGM_RX_CHAIN_BOTH))
  {
    if (rxf1_is_on == TRUE) clk_regime_rxf_request(CLKRGM_RXF_RF_1, CLKRGM_RXF_ENABLE);
  }


  if (rx_chain == CLKRGM_RX_CHAIN_0)
  {
    clkrgm_modem.clk_mode[rx_chain] = CLKRGM_MODEM_CLK_MODE_CDMA;
  }
  else if (rx_chain == CLKRGM_RX_CHAIN_1)
  {
    clkrgm_modem.clk_mode[rx_chain] = CLKRGM_MODEM_CLK_MODE_CDMA;
  }
  else if (rx_chain == CLKRGM_RX_CHAIN_BOTH)
  {
    clkrgm_modem.clk_mode[CLKRGM_RX_CHAIN_0] = CLKRGM_MODEM_CLK_MODE_CDMA;
    clkrgm_modem.clk_mode[CLKRGM_RX_CHAIN_1] = CLKRGM_MODEM_CLK_MODE_CDMA;
  }



  /* ------------------------------------------------------------------------
     Free interrupts.
  ------------------------------------------------------------------------ */

  INTFREE_SAV(isave);

} /* clk_regime_config_cdma_clocks */

#ifdef FEATURE_GPSONE
#error code not present
#endif /* #ifdef FEATURE_GPSONE */

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

boolean clk_regime_agc_src_on (void)
{
  return clkrgm_modem.agc_src_is_on;
} /* clk_regime_agc_src_on */

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

void clk_regime_set_agc_src_on (void)
{
  clkrgm_modem.agc_src_is_on = TRUE;
} /* clk_regime_set_agc_src_on */

/*==========================================================================

  FUNCTION      CLK_REGIME_SETUP_GSM_CLK

  DESCRIPTION   This function configures various clocks for GSM/GPRS mode.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The GSM/GPRS clock blocks are initialized and activated.

==========================================================================*/

void clk_regime_setup_gsm_clk (void)
{
  /* TBD */
} /* clk_regime_setup_gsm_clk */

void clk_regime_set_mdsp(clkrgm_mdsp_clk_frequency_type mdsp_speed )
{
#ifndef T_RUMI

  clkrgm_modem.mdsp_pending_speed = mdsp_speed;

  /* disable foot switch */
  CLKRGM_SET( MODEM_CLK_MDSP_FS_CTL, HM_CLK_UP_FS_ON );

  if (clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_pending_speed].mode !=
      clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_speed].mode)
  {
    /* Hardware governed switching is disabled */
    CLKRGM_SET(MODEM_CLK_DYN_SW_BYPASS, BYPASS);

    CLKRGM_W(MDSP_MNCNTR_MD,
             clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_pending_speed].mdsp_mncntr_md);
    CLKRGM_W(MDSP_MNCNTR_NS,
             clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_pending_speed].mdsp_mncntr_ns);

    CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL,
                    MDSP_CLK_1X_MODE, clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_pending_speed].mode);

    /* Hardware governed switching is enabled */
    CLKRGM_CLEAR(MODEM_CLK_DYN_SW_BYPASS, BYPASS);

    /* Switch the mDSP clock to the programmed frequency */
    CLKRGM_SET  (MDSP_CLK_SWITCH, DATA);
    CLKRGM_CLEAR(MDSP_CLK_SWITCH, DATA);
  }
  else
  {

    /* Hardware governed switching is enabled */
    CLKRGM_CLEAR(MODEM_CLK_DYN_SW_BYPASS, BYPASS);

    CLKRGM_W(MDSP_MNCNTR_MD,
             clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_pending_speed].mdsp_mncntr_md);
    CLKRGM_W(MDSP_MNCNTR_NS,
           clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_pending_speed].mdsp_mncntr_ns);

    /* Switch the mDSP clock to the programmed frequency */
    CLKRGM_SET  (MDSP_CLK_SWITCH, DATA);
    CLKRGM_CLEAR(MDSP_CLK_SWITCH, DATA);
  }


#endif

  /* If the sourcing PLL of the new clock frequency is different from the
     previous sourcing PLL, then request to disable the old PLL */
  if ((clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_speed].pll !=
      clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_pending_speed].pll) 
      &&
     (clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_speed].pll !=
      CLKRGM_NOPLL))
  {
    clk_regime_pll_request(
      clkrgm_modem_mdsp_cfg[clkrgm_modem.mdsp_speed].pll,
      (int)CLKRGM_MDSP_CLK,
      CLKRGM_PLL_DISABLE);
  }
}

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

void clk_regime_switch_mdsp_clk (void)
{

  if (clkrgm_modem.mdsp_pending_speed == clkrgm_modem.mdsp_speed)
  {
    return;
  }

  clkrgm_modem.mdsp_speed = clkrgm_modem.mdsp_pending_speed;

} /* clk_regime_switch_mdsp_clk */

/*===========================================================================

FUNCTION CLK_REGIME_COMPLETE_MDSP_CLK_SWITCH

DESCRIPTION
  Completes the process of switching RXDSP clock.This function is provided to
  complete any operations by the ARM after switching the RXDSP clock. 

DEPENDENCIES
  clk_regime_switch_mdsp_clk() should have been called prior to switch the
  RXDSP clock to the desired frequency prior to calling this function .

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void clk_regime_complete_mdsp_clk_switch ( void )
{
  #ifdef FEATURE_REPORT_CLK_FREQUENCY_CHANGE_EVENT
  {
     event_report_payload
     (
        EVENT_MDSP_CLK_FREQUENCY_CHANGE,
        sizeof(clkrgm_modem.mdsp_pending_speed),
        (void*)(&clkrgm_modem.mdsp_pending_speed)
     );
  }
  #endif /* FEATURE_REPORT_CLK_FREQUENCY_CHANGE_EVENT */

} /*clk_regime_complete_mdsp_clk_switch */

clkrgm_mdsp_clk_frequency_type clk_regime_get_mdsp_clk_frequency(void)
{
  return clkrgm_modem.mdsp_speed;
}

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
void clk_regime_set_mdsp_clk_frequency
(
   clkrgm_mdsp_clk_frequency_type clk_frequency
)
{
   clkrgm_device_param_type  param;

   if (clk_frequency >= CLKRGM_MDSP_NUM_SPEEDS)
   {
     ERR_FATAL("Inavlid MDSP clock frequency request",0,0,0);
   }

   param.minimum_speed = 
     clkrgm_modem_mdsp_cfg[clk_frequency].clk_freq_khz;

   clk_regime_devman_request(
     &clkrgm.devman_client, CLKRGM_DEVICE_MDSP,
     CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED, &param);

} /* clk_regime_set_mdsp_clk_frequency */


/*==========================================================================

  FUNCTION      CLK_REGIME_MODEM_LOG

  DESCRIPTION   This function generates a clock log record according to the
                specified clock number and transaction.

  PARAMETERS    clk_num     - Clock number
                transaction - Transaction (enable/disable)

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  A clock log record will be generated.

==========================================================================*/

void clk_regime_modem_log
(
  clkrgm_modem_clk_log_type clk_num,
  uint8 transaction)
{
  #ifndef BUILD_BOOT_CHAIN_OEMSBL
  clkrgm_modem_log_type *log = NULL;

  if (log_status(LOG_CLOCK_C))
  {
    log = (clkrgm_modem_log_type *)log_alloc(LOG_CLOCK_C,
                                             sizeof(clkrgm_modem_log_type));
    if (log == NULL)
    {
      ERR("clk_regime_modem_log: Log packet allocation failure", 0, 0, 0);
      return;
    }
    log->hw_rev_num = HWIO_IN(HW_REVISION_NUMBER);
    log->clock_domain = CLKRGM_MODEM_LOG_DOMAIN;
    log->transaction_type = transaction;
    log->clock_num = clk_num;
    log->status_reg_count = CLKRGM_MODEM_LOG_STATUS_REG_COUNT;
    log->status_reg[0] = HWIO_IN(MODEM_CLK_HALT);
    log->status_reg[1] = HWIO_IN(MODEM_CLK_HALT_2);
    log_commit(log);
  }
  #endif
} /* clk_regime_msm_log */

/*==========================================================================

  FUNCTION      CLK_REGIME_RXF_ON

  DESCRIPTION   Turn on CHIPXN_DIV2, RXF_SAMP and RXF_SAMP_REF. Only this 
                function is able to turn these clocks on by virtue of code
                in clk_regime_on() that tests boolean clkrgm_manages_rxf 
                before toggling clk_halt register bits.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  RXF clocks are turned on.

==========================================================================*/
static void clk_regime_rxf_on( clkrgm_rxf_client_type  client )
{
   clkrgm_manages_rxf = TRUE;

   clk_regime_enable_client( CLKRGM_CHIPXN_DIV2_CLK, client );

    /* Turn ON the RXF_SAMPLE_CLK */
   clk_regime_enable_client( CLKRGM_RXF_SAMP_CLK, client);
    /* Turn ON the RXF_SAMPLE_REF_CLK for RESAMPLER*/
   clk_regime_enable_client(CLKRGM_RXF_SAMP_REF_CLK, client);

   clkrgm_modem.rxf_state = CLKRGM_RXF_ON;
   clkrgm_manages_rxf = FALSE;
}

/*==========================================================================

  FUNCTION      CLK_REGIME_RXF_OFF

  DESCRIPTION   Turn off CHIPXN_DIV2, RXF_SAMP and RXF_SAMP_REF. Only this 
                function is able to turn these clocks off by virtue of code
                in clk_regime_on() that tests boolean clkrgm_manages_rxf 
                before toggling clk_halt register bits.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  RXF clocks are turned off.

==========================================================================*/
static void clk_regime_rxf_off( clkrgm_rxf_client_type  client )
{
   clkrgm_manages_rxf = TRUE;
    /* Turn ON the RXF_SAMPLE_CLK */
   clk_regime_disable_client( CLKRGM_RXF_SAMP_CLK, client);
    /* Turn ON the RXF_SAMPLE_REF_CLK for RESAMPLER*/
   clk_regime_disable_client(CLKRGM_RXF_SAMP_REF_CLK, client);

   clk_regime_disable_client( CLKRGM_CHIPXN_DIV2_CLK, client );

   clkrgm_modem.rxf_state = CLKRGM_RXF_OFF;
   clkrgm_manages_rxf = FALSE;
}

/*==========================================================================

  FUNCTION      CLK_REGIME_RXF_REQUEST

  DESCRIPTION   Request that RXF be enabled or disabled on behalf
                of the specified client.  A voting mechanism is implemented
                to make sure that RXF is on as long as there is at least
                one active client.

  PARAMETERS    client    - Requesting client (clock sourced by the PLL)
                request   - Request to turn on or off the PLL

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  RXF is turned on or off as applicable.

==========================================================================*/

void clk_regime_rxf_request (
  clkrgm_rxf_client_type  client,
  clkrgm_rxf_op_type      request)
{
  uint32 sav_var;
  
  /* Validate input arguments */
  
  if ((client < 0) || (client >= CLKRGM_RXF_MAX_CLIENTS))
  {
    /* Error: Client out of range */
    ERR ("clk_regime_rxf_request: Client out of range (%d)",
         client, 0, 0);
    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Interrupts are locked to prevent race conditions between RXF clients  */
  /*-----------------------------------------------------------------------*/
  
  INTLOCK_SAV (sav_var);

  /*-----------------------------------------------------------------------
   Algorithm:
    1. Determine the specific operation to perform
    2. If DISABLE:
      a. Decrement the client usage counter (never negative)
      b. Decrement the RXF usage counter (never negative)
      c. If the RXF is ON and the usage counter is zero, then
          turn off the RXF
    3. If ENABLE:
      a. Increment the client usage counter
      b. Increment the RXF usage counter
      c. If the RXF is OFF, then turn it on
  -----------------------------------------------------------------------*/
  
    switch (request)
    {
      case CLKRGM_RXF_DISABLE:
        if (clkrgm.rxf_clients[client])
        {
          clkrgm.rxf_clients[client] = FALSE;
          clkrgm.rxf_usage--;
        }
        if ((clkrgm.rxf_state == CLKRGM_RXF_ON) &&
            (clkrgm.rxf_usage == 0))
        {
          clk_regime_rxf_off (client); 
        }
        break;

      case CLKRGM_RXF_ENABLE:
        if ( !clkrgm.rxf_clients[client] )
        {
          clkrgm.rxf_clients[client] = TRUE;
          clkrgm.rxf_usage++;
        }
        if (clkrgm.rxf_state == CLKRGM_RXF_OFF)
        {
          clk_regime_rxf_on (client); 
        }
        break;
        
      default:
          break;
    }

  /*-----------------------------------------------------------------------*/
  /* Unlock interrupts                                                     */
  /*-----------------------------------------------------------------------*/
  
  INTFREE_SAV (sav_var);

} /* clk_regime_rxf_request */


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

void clk_regime_misc_enable (clkrgm_modem_clk_type clk)
{
  clk_regime_enable( clk );

} /* clk_regime_misc_enable */

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

void clk_regime_misc_disable (clkrgm_modem_clk_type clk)
{
  clk_regime_disable( clk );

} /* clk_regime_misc_disable */

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

void clk_regime_sel_clk_src (
  clkrgm_modem_clk_type      clk,
  uint32                     source)
{
  boolean is_on = FALSE;
  uint32  isave;

  /* ------------------------------------------------------------------------
     Lock interrupts around this operation since the corresponding clock
     may be turned off.
  ------------------------------------------------------------------------ */
  INTLOCK_SAV(isave);

  /* ------------------------------------------------------------------------
     If the clock regime is enabled, then disable the clock regime before
     switching the clock source.
  ------------------------------------------------------------------------ */
  is_on = clk_regime_is_on(clk);  

  if (is_on == TRUE)
  {
    clk_regime_off(clk);
  }

  /* ------------------------------------------------------------------------
     Insert an unconditional delay of at least 3 clocks since the caller
     may have disabled the clock regime immediately prior to calling this
     function.
  ------------------------------------------------------------------------ */
  if( clk == CLKRGM_SLEEP_CLK )
  {
    clk_busy_wait( CLKRGM_MODEM_SLEEP_DISABLE_TIME_US );
  } else {
    clk_busy_wait( CLKRGM_MODEM_1MHZ_DISABLE_TIME_US );
  }

  /* ------------------------------------------------------------------------
     Switch the clock source.
  ------------------------------------------------------------------------ */
  switch(clk)
  {
    /* CDMA sleep controller. */
    case CLKRGM_SLPFAST_FEE_CLK:
      CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_MISC_CTL, SLPFAST_FEE_SRC_SEL, source);
      break;

    /* SBI. */
    case CLKRGM_MODEM_SBI_CLK:
      CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL, SBI_CLK_SRC_SEL, source);
      break;

    /* Receive chain 0 analog clock. */
    case CLKRGM_ANT0_SAMP_CLK:
      CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL_2, ANT0_SAMP_CLK_SRC_SEL, source);
      break;

    /* Receive chain 1 analog clock. */
    case CLKRGM_ANT1_SAMP_CLK:
      CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL_2, ANT1_SAMP_CLK_SRC_SEL, source);
      break;

   case CLKRGM_SAMP_FIFO_CLK0:
      CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL_2, SAMPLE_FIFO_CLK_SRC_SEL0, source);
      break;
   case CLKRGM_SAMP_FIFO_CLK1:
      CLKRGM_W_MASKED(MODEM_CLK_MISC_CTL_2, SAMPLE_FIFO_CLK_SRC_SEL1, source);
      break;

   case CLKRGM_RXF_SAMP_CLK:
      /* For RXF_SAMP and RXF_SAMP_REF, explicitly manage the on/off by direct
       * access to clk_halt registers/bits because the api function clk_regime_off/on
       * are modified to disallow any operation on these clocks except by 
       * clk_regime_rxf_request().
       */
#ifndef BUILD_BOOT_CHAIN_OEMSBL
      MSM_INTLOCK();
#endif
      if (is_on)
      {
        
        CLKRGM_SET(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT);    
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
        clk_busy_wait(1);
      }

      CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_MISC_CTL, CDMA_RX_SAMPLE_RAW_SEL, source);

      if (is_on)
      {
        CLKRGM_CLEAR(MODEM_CLK_HALT, RXF_SAMP_CLK_HALT);    
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
#ifndef BUILD_BOOT_CHAIN_OEMSBL
      MSM_INTFREE();
#endif
      break;

   case CLKRGM_RXF_SAMP_REF_CLK:
      if (is_on)
      {
        CLKRGM_SET(MODEM_CLK_HALT_2, RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
        clk_busy_wait(1);
      }
      CLKRGM_W_MASKED(MODEM_RXF_SAMP_REF_CLK_CTL,CLK_SRC_SEL, source);
      if (is_on)
      {
        CLKRGM_CLEAR(MODEM_CLK_HALT_2, RXF_SAMP_REF_CLK_HALT);
        clk_regime_modem_log(CLKRGM_MODEM_LOG_RXF_SAMP_REF_CLK,
                             CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      }
      break;

   #ifdef CLKRGM_INCLUDE_GPS
   case CLKRGM_GACC_CLK:
      CLKRGM_W_MASKED(MODEM_GACC_CLK_CTL,CLK_SRC_SEL, source);
      break;
   #endif /* CLKRGM_INCLUDE_GPS */

   case CLKRGM_CHIPXN_CLK:
      CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_CTL,CDMA_CLK_SRC_SEL, source);
      break;

   #ifdef CLKRGM_INCLUDE_WCDMA
   case WCDMA_CHIPXN_CLK:
      CLKRGM_W_MASKED(MODEM_CHIPXN_CLK_CTL,WCDMA_CLK_SRC_SEL, source);
      break;
   #endif /* CLKRGM_INCLUDE_WCDMA */

   case CLKRGM_OFFLINE_CLK:
      CLKRGM_W_MASKED(MODEM_OFFLINE_CLK_CTL, CLK_SRC_SEL, source);
      break;

   #ifdef CLKRGM_INCLUDE_WCDMA
   case CLKRGM_F9_CLK:
      CLKRGM_W_MASKED(MODEM_F9_CLK_CTL, CLK_SRC_SEL, source);
      break;
   #endif /* CLKRGM_INCLUDE_WCDMA */
   
   case CLKRGM_MODEM_WEB_CLK:
      CLKRGM_W_MASKED(MODEM_WEB_CLK_CTL, CLK_SRC_SEL, source);
      break;
  
   /* Unsupported clock regimes. */
   default:
      /* Option not supported. */
      /* Error: Clock branch of out range */
      ERR("clk_regime_sel_clk_src: Clock branch out of range (%d)", clk, 0, 0);
      break;
  }

  /* ------------------------------------------------------------------------
     If the clock regime was enabled, then re-enable the clock regime after
     switching the source.
  ------------------------------------------------------------------------ */
  if (is_on == TRUE)
  {
    clk_regime_on(clk);
  }

  /* ------------------------------------------------------------------------
     Unlock interrupts now that the selected clock is back on.
  ------------------------------------------------------------------------ */
  INTFREE_SAV(isave);

} /* clk_regime_sel_clk_src */
