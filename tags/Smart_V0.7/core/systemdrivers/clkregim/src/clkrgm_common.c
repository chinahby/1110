/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    Q S C 1 1 0 0    C L O C K    R E G I M E    D E V I C E   D R I V E R


GENERAL DESCRIPTION

  This module contains procedures to support clock regime configuration and
  control functions for the QSC1100 clock regimes.

EXTERNALIZED FUNCTIONS
  clk_regime_msm_enable
    Enable specified hardware clock regime.

  clk_regime_msm_disable
    Disable specified hardware clock regime.
    
  clk_regime_msm_misc_enable
    Enable specified miscellaneous hardware clock function.

  clk_regime_msm_misc_disable
    Disable specified miscellaneous hardware clock function.

  clk_regime_get_max_hclk_freq_khz
    Return frequency in KHz of the maximum HCLK frequency

  clk_regime_sbi_enable
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

  All clock regimes are enabled after reset.  The clk_regime_init() function
  should be called prior to enabling or disabling specific clock regimes.


  Copyright (c) 2004-2008      by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_common.c#32 $
$DateTime: 2009/04/02 01:06:49 $
$Author: bamidir $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/09    br     Added support for EFUSE Clock regime.
01/20/09   hvo     Featurized the Modem Turbo Decode, only available for mid
                   and high tiers      
11/19/08    kma    Fixed Lint errors.
05/15/08    bc     Fixed and enabled clock regime switching of the
                   ARM/AHB/Bus clocks.
04/18/08    bc     Removed GSBI0 source management until a UIM driver API can
                   be written.
10/11/07    bc     Initial port from QSC6055 rev 3202.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "clkregim.h"   /* Clock Regime definitions             */
#include "clkrgm_api.h" /* For download functions               */
#include "clkrgmi_msm.h"
#include "clkrgmi_modem.h"
#include "clkrgm_client.h"
#include "assert.h"
  
/* The file customesbl.h below does some undef's, so include it last */
#ifdef BUILD_BOOT_CHAIN
  #include BOOT_CUSTSBL_H
#else
  #include "clk.h"
#endif

#ifdef FEATURE_CLKREGIM_RM
  #define RM_MSM_CLK_INFO(clk) (&clkrgm.rm_clk_info[(int)clk])
  #define RM_MODEM_CLK_INFO(clk) (&clkrgm_modem.rm_clk_info[(int)clk])
#endif
/*===========================================================================

                     LOCAL FEATURE DEFINITIONS

===========================================================================*/

/*===========================================================================

                     FORWARD FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* --------------------------------------------------------------------------
   PLL configuration data structures.
-------------------------------------------------------------------------- */

  /* PLL configuration structure. */

  const clkrgm_pll_cfg_type clkrgm_pll_cfg = CLKRGM_PLL_CONFIG;

/* --------------------------------------------------------------------------
   Microprocessor configuration data structure.
-------------------------------------------------------------------------- */

  const clkrgm_mclk_clkrgm_type clkrgm_cpu_cfg [CLKRGM_CPU_NUM_SPEEDS] =
                                  CLKRGM_CPU_CONFIGS;

/* --------------------------------------------------------------------------
   DSP clock regime configuration data structures.
-------------------------------------------------------------------------- */
  const clkrgm_adsp_cfg_type clkrgm_adsp_cfg [CLKRGM_ADSP_NUM_SPEEDS] =
                               CLKRGM_ADSP_CONFIGS;

/* --------------------------------------------------------------------------
   Audio clock regime configuration data structures.
-------------------------------------------------------------------------- */

  /* Internal CODEC clock regime configuration structure array. */

  const clkrgm_msm_type1_cfg_type clkrgm_icodec_cfg [CLKRGM_CODEC_CLK_MAX] = CLKRGM_ICODEC_CONFIGS;

  /* External CODEC clock regime configuration structure. */

  const clkrgm_msm_type1_cfg_type clkrgm_ecodec_cfg[CLKRGM_ECODEC_CLK_MAX] = CLKRGM_ECODEC_CONFIG;

  /* Stereo DAC clock regime configuration structure array. */

  const clkrgm_msm_type1_cfg_type clkrgm_sdac_cfg [CLKRGM_SDAC_CLK_MAX] = CLKRGM_SDAC_CONFIGS;

/* --------------------------------------------------------------------------
   Peripheral clock regime configuration data structures.
-------------------------------------------------------------------------- */

  /* External camera (GPIO 13) clock regime configuration structure array. */

  const clkrgm_msm_type1_cfg_type clkrgm_camclk_cfg[CLKRGM_CAMCLK_CLK_MAX]
                                    = CLKRGM_CAMCLK_CONFIGS;

  /* SDCC clock regime configuration structure array. */

  const clkrgm_msm_type1_cfg_type clkrgm_sdcc_cfg [CLKRGM_SDCC_CLK_MAX ] = CLKRGM_SDCC_CONFIGS;

  /* UART_DM clock regime configuration structure array. */

  const clkrgm_msm_type1_cfg_type clkrgm_uart_dm_cfg [CLKRGM_UART_DM_CLK_MAX]
                                    = CLKRGM_UART_DM_CONFIGS;

  const clkrgm_msm_type1_cfg_type clkrgm_gsbi0_uart_cfg [CLKRGM_UART_DM_CLK_MAX]
                                    = CLKRGM_GSBI0_UART_CONFIGS;
  const clkrgm_msm_type1_cfg_type clkrgm_gsbi_1_2_uart_cfg[CLKRGM_UART_DM_CLK_MAX]
                                    = CLKRGM_GSBI_1_2_UART_CONFIGS;

/* --------------------------------------------------------------------------
   USB variables.
-------------------------------------------------------------------------- */

  boolean usb_xtal_on    = FALSE;

  /* Keep track of whether USB CHG code has set the MSM into charging mode */
  static clkrgm_usb_chg_mode_type clkrgm_usb_chg_mode = CLKRGM_USB_NORMAL_MODE;

/* --------------------------------------------------------------------------
   Modem clock state variables.
-------------------------------------------------------------------------- */


  /* Clock regime state information structure definition. */
  clkrgm_msm_state_type clkrgm;

  /* Modem clocks general state and configuration */
  clkrgm_modem_state_type           clkrgm_modem;

  /* Configuration of the mDSP clock for all the available speeds */
  const clkrgm_modem_mdsp_cfg_type  clkrgm_modem_mdsp_cfg[CLKRGM_MDSP_NUM_SPEEDS] = 
                                                                   CLKRGM_MDSP_CFG_DATA;

/* --------------------------------------------------------------------------
  Internal function forward declarations
-------------------------------------------------------------------------- */
extern void clk_regime_msm_off(clkrgm_msm_clk_type regime);
extern void clk_regime_msm_on(clkrgm_msm_clk_type regime);

extern void clk_regime_msm_log (
  clkrgm_msm_clk_log_type clk_num,
  uint8 transaction);

extern void clk_regime_modem_log
(
  clkrgm_modem_clk_log_type clk_num,
  uint8 transaction
);

static boolean clk_regime_pll_off_ok (void);

/*===========================================================================
FUNCTION CLK_REGIME_GET_MAX_HCLK_FREQ_KHZ

DESCRIPTION
  This function returns the frequency in KHz of the maximum HCLK frequency

DEPENDENCIES
  None.

RETURN VALUE
  The frequency in KHz of the maximum HCLK frequency

SIDE EFFECTS
  None.
===========================================================================*/
uint32 clk_regime_get_max_hclk_freq_khz( void )
{
  return CLKRGM_HCLK_FREQUENCY;
}
/*===========================================================================
FUNCTION CLK_REGIME_SBI_ENABLE

DESCRIPTION
  This function enables the SBI clock

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void clk_regime_sbi_enable (void)
{
  /* disable it */
  CLKRGM_SET( MODEM_CLK_HALT, MODEM_SBI_CLK_HALT );
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, NOT_ENABLED);
  clk_busy_wait(1);

  /* select TCXO for source */
  CLKRGM_SET_FVAL( MODEM_CLK_MISC_CTL, SBI_CLK_SRC_SEL, TCXO );
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_SRC_SEL, XO );

  /* enable it */
  CLKRGM_CLEAR( MODEM_CLK_HALT, MODEM_SBI_CLK_HALT );
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED);
}

/*==========================================================================

  FUNCTION      CLK_REGIME_MSM_VERIFY_CLK_SRC

  DESCRIPTION   This function verifies that the given clock's SRC_SEL
                maps to the expected PLL.

  PARAMETERS    clk     - clock whose source to verify
                pll     - expected PLL
                src_sel - value of the SRC_SEL field from the clock's CD_REG

  DEPENDENCIES  None.

  RETURN VALUE  The real PLL source to use.

  SIDE EFFECTS  None.

==========================================================================*/

static clkrgm_pll_type clk_regime_msm_verify_clk_src
(
  clkrgm_msm_clk_type            clk,
  clkrgm_pll_type                pll,
  clkrgm_msm_type1_src_type      src_sel
)
{
  clkrgm_pll_type src_pll;

  /* ------------------------------------------------------------------------
     Map the source to a PLL
  ------------------------------------------------------------------------ */

  switch (src_sel)
  {
    case CLKRGM_TYPE1_SRC_IS_PLL:
      src_pll = CLKRGM_PLL;
      break;

    default:
      src_pll = CLKRGM_NOPLL;
      break;
  }

  /* ------------------------------------------------------------------------
     Make sure it matches what we expect
  ------------------------------------------------------------------------ */

  if (src_pll != pll)
  {
     MSG_ERROR("Clock source mismatch: clk=%d, pll=%d, src_sel=%d",
               clk, pll, src_sel);
  }

  return src_pll;

} /* clk_regime_msm_verify_clk_src */
/*==========================================================================

  FUNCTION      CLK_REGIME_MSM_SRC_REQUEST

  DESCRIPTION   This function configures the required source (PLL) for the
                given MSM clock and mode (enabled or disabled).

  PARAMETERS    clk     - Clock whose source to configure
                request - PLL request type (enable or disable)

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_msm_src_request
(
  clkrgm_msm_clk_type     clk,
  clkrgm_pll_op_type      request
)
{
  clkrgm_pll_type pll = CLKRGM_NOPLL;
  clkrgm_msm_type1_src_type src_sel = CLKRGM_TYPE1_SRC_IS_INVALID;
  
  ASSERT( clkrgm.cpu_speed < CLKRGM_CPU_NUM_SPEEDS );
  ASSERT( clkrgm.adsp_speed < CLKRGM_ADSP_NUM_SPEEDS );
  /* ------------------------------------------------------------------------
     Determine the PLL for this clock
  ------------------------------------------------------------------------ */

  switch (clk)
  {
    case CLK_RGM_MCLK_M:               /* Micro (ARM) clock                 */
      /* Global clock source PLL */
      if( clkrgm_cpu_cfg[clkrgm.cpu_speed].global_cfg.src ==
            CLKRGM_MCLK_SRC_PLL )
      pll = CLKRGM_PLL;
      break;

    case CLK_RGM_HCLK_M:               /* Bus (AHB) clock                   */
    case CLK_RGM_ETM_M:                /* ETM                               */
    case CLK_RGM_UXMC_M:               /* UXMC                              */
    case CLK_RGM_EBI1_CLK_M:           /* EBI1 clock                        */
    case CLK_RGM_EBI2_CLK_M:           /* EBI2 clock                        */
      /* These clocks run off the global clock, which we gate based
         on the CLKRGM_MARM_CLK.  This is to simplify switching the global
         clock source - do not want to have to switch the PLL client list
         for all the global clock sourced regimes. */
      break;

    case CLK_RGM_SBI_M:                /* PMIC SSBI                         */
    case CLK_RGM_UART_LEGACY_M:        /* 2-pin UART muxed on GPIO 25,27    */
    case CLK_RGM_I2C_LEGACY_M:         /* I2C bus on GPIO 39,40             */
	case CLK_RGM_RFMS_XO_CLK_M:         /* I2C bus on GPIO 39,40             */
		
      /* TCXO */
      break;

    case CLK_RGM_GSBI0_M:            /* GSBI0 (superset of UART0,SPI0,I2C0) */
    case CLK_RGM_UART0_M:              /* UART on GSBI0                     */
    case CLK_RGM_I2C0_M:               /* I2C bus on GSBI0                  */
    case CLK_RGM_SPI0_M:               /* SPI bus on GSBI0                  */
      if( HWIO_INF(GSBI0_CD_REG, SRC_SEL) == 
            HWIO_FVAL(GSBI0_CD_REG, SRC_SEL, PLL_SRC) )
      {
        pll = CLKRGM_PLL;
      }
      break;

    case CLK_RGM_GSBI1_M:            /* GSBI1 (superset of UART1,SPI1,I2C1) */
    case CLK_RGM_UART1_M:              /* UART on GSBI1                     */
    case CLK_RGM_I2C1_M:               /* I2C bus on GSBI1                  */
    case CLK_RGM_SPI1_M:               /* SPI bus on GSBI1                  */
      if( HWIO_INF(GSBI1_CD_REG, SRC_SEL) == 
            HWIO_FVAL(GSBI1_CD_REG, SRC_SEL, PLL_SRC) )
      {
        pll = CLKRGM_PLL;
      }
      break;

    case CLK_RGM_GSBI2_M:            /* GSBI2 (superset of UART2,SPI2,I2C2) */
    case CLK_RGM_UART2_M:              /* UART on GSBI2                     */
    case CLK_RGM_I2C2_M:               /* I2C bus on GSBI2                  */
    case CLK_RGM_SPI2_M:               /* SPI bus on GSBI2                  */
      if( HWIO_INF(GSBI2_CD_REG, SRC_SEL) == 
            HWIO_FVAL(GSBI2_CD_REG, SRC_SEL, PLL_SRC) )
      {
        pll = CLKRGM_PLL;
      }
      break;


    case CLK_RGM_USB_MCLK_M:    
    case CLK_RGM_USB_AHB_CLK_M: 
    case CLK_RGM_USB_M:         
      pll = CLKRGM_PLL;
      break;

    case CLK_RGM_SDCC_MCLK_M:          /* SDCC MCLK (core)                  */
    case CLK_RGM_SDCC_HCLK_M:          /* SDCC HCLK (AHB/Bus)               */
      if( HWIO_INF(SDCC_CD_REG, SRC_SEL) == 
            HWIO_FVAL(SDCC_CD_REG, SRC_SEL, PLL_SRC) )
      {
        pll = CLKRGM_PLL;
      }
      break;

    case CLK_RGM_ADSP_M:
      if( clkrgm_adsp_cfg[clkrgm.adsp_speed].src == CLKRGM_TYPE1_SRC_IS_PLL ) {
        pll = CLKRGM_PLL;
      }
      /* Check the ADSP clock mux (0 or 1), and then the correct source */
      if( HWIO_INF(ADSP_CLK_CTL_REG, ADSP_SRC_SEL) == 0 ) {
        src_sel = HWIO_INF(ADSP_CLK_CTL_REG, SRC_SEL0);
      } else {
        src_sel = HWIO_INF(ADSP_CLK_CTL_REG, SRC_SEL1);
      }
      break;

    case CLK_RGM_ICODEC_TX_M:          /* Internal CODEC TX                 */
      if( HWIO_INF(ICODEC_TX_CD_REG, SRC_SEL) == 
            HWIO_FVAL(ICODEC_TX_CD_REG, SRC_SEL, PLL_SRC) )
      {
        pll = CLKRGM_PLL;
      }
      break;

    case CLK_RGM_ICODEC_M:             /* Internal CODEC                    */
    case CLK_RGM_ICODEC_RX_M:          /* Internal CODEC RX                 */
      if( HWIO_INF(ICODEC_TX_CD_REG, SRC_SEL) == 
            HWIO_FVAL(ICODEC_TX_CD_REG, SRC_SEL, PLL_SRC) )
      {
        pll = CLKRGM_PLL;
      }
      break;

    case CLK_RGM_ECODEC_M:
      if( clkrgm_ecodec_cfg[clkrgm.adsp_speed].src == CLKRGM_TYPE1_SRC_IS_PLL ) {
        pll = CLKRGM_PLL;
      }
      src_sel = HWIO_INF(ECODEC_CD_REG, SRC_SEL);
      break;

    case CLK_RGM_CAMCLK_M:             /* Camera clock                      */
    if (CLKRGM_TYPE1_SRC_IS_PLL == clkrgm.camclk.src) {
        pll = CLKRGM_PLL;
      }
	src_sel = clkrgm.camclk.src;
      break;

    case CLK_RGM_CAMIF_PCLK_M:
      pll = CLKRGM_NOPLL;
      break;

    case CLK_RGM_VIDEO_VFE_M:
      if( clkrgm.video_vfe.src == CLKRGM_TYPE1_SRC_IS_PLL ) {
        pll = CLKRGM_PLL;
      }
      if( HWIO_INF(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL)
            == HWIO_FVAL(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL, VIDEO_VFE_DIV_MND) )
      {
        src_sel = HWIO_INF(VIDEO_VFE_CD_REG, VIDEO_VFE_SRC_SEL);
      }
      break;

    case CLK_RGM_VIDEO_M:              /* Video AHB clock                   */
    case CLK_RGM_AUDIO_M:              /* Audio AHB clock                   */
    case CLK_RGM_SEC_HCLK_M:           /* security block                    */
    case CLK_RGM_MDP_M:                /* MDP                               */
      /* These clocks run off the global clock, which we gate based
         on the CLKRGM_MARM_CLK. */
      break;

    case CLK_RGM_SDAC_M:               /* I2S clock                         */
      if( clkrgm_sdac_cfg[clkrgm.sdac_speed].src == CLKRGM_TYPE1_SRC_IS_PLL ) {
        pll = CLKRGM_PLL;
      }
      src_sel = HWIO_INF(I2S_CD_REG, SRC_SEL);
      break;

    case CLK_RGM_QMEMBIST_M:           /* QMEMBIST core                     */
	case CLK_RGM_EFUSE_CLK:   /* EFUSE Clock */
      /* TCXO */
      break;

    case CLK_RGM_NOT_SUPPORTED:
      break;
      
    default:
      /* Error: Clock branch of out range */
      MSG_ERROR ("clk_regime_msm_src_request: clock out of range (%d)",
                 clk, 0, 0);
      return;
  }

  /* ------------------------------------------------------------------------
     Validate the PLL if possible
  ------------------------------------------------------------------------ */

  if ( src_sel != CLKRGM_TYPE1_SRC_IS_INVALID )
  {
    pll = clk_regime_msm_verify_clk_src(
            clk,
            pll,
            src_sel);
  }

  /* ------------------------------------------------------------------------
     Make the PLL request
  ------------------------------------------------------------------------ */

  if (pll != CLKRGM_NOPLL)
  {
    clk_regime_pll_request (pll, clk, request);
  }

} /* clk_regime_msm_src_request */


/*===========================================================================

FUNCTION CLK_REGIME_MSM_DISABLE

DESCRIPTION
  This function will disable the specified clock regimes.  No attempt to
  determine if the source clock is enabled is made.

PARAMETERS
  regime - clock regime to be disabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Selected clock regime circuit is disabled.

===========================================================================*/
void clk_regime_msm_disable
(
  clkrgm_msm_clk_type clk 
)
{
  uint32 isav;

  /* ------------------------------------------------------------------------
     Filter out group-clocks for recursive handling.
  ------------------------------------------------------------------------ */

  switch (clk)
  {
    case CLK_RGM_ICODEC_M:
      clk_regime_msm_disable(CLK_RGM_ICODEC_RX_M);
      clk_regime_msm_disable(CLK_RGM_ICODEC_TX_M);
      return;

    case CLK_RGM_NOT_SUPPORTED:
      return;

    default:
      break;
  }

  /* ------------------------------------------------------------------------
     Ensure atomicity
  ------------------------------------------------------------------------ */

  INTLOCK_SAV(isav);

  #if defined(FEATURE_CLKREGIM_RM) && !defined(BUILD_BOOT_CHAIN)
  /* ------------------------------------------------------------------------
     Check with the resource manager
  ------------------------------------------------------------------------ */

  if (!clk_regime_rm_clk_disable(RM_MSM_CLK_INFO(clk)))
  {
    INTFREE_SAV(isav);
    return;
  }
  #endif

  /* ------------------------------------------------------------------------
     Disable the clock branch
  ------------------------------------------------------------------------ */

  clk_regime_msm_off (clk);

  /* ------------------------------------------------------------------------
     Configure any required source to disabled
  ------------------------------------------------------------------------ */

  clk_regime_msm_src_request(clk, CLKRGM_PLL_DISABLE);

  /* ------------------------------------------------------------------------
     Free interrupts
  ------------------------------------------------------------------------ */

  INTFREE_SAV(isav);

} /* clk_regime_msm_disable */


/*===========================================================================

FUNCTION CLK_REGIME_MSM_OFF

DESCRIPTION
  This function will disable the specified clock regimes.  No attempt to
  determine if the source clock is enabled is made.

PARAMETERS
  regime - clock regime to be disabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Selected clock regime circuit is disabled.

===========================================================================*/
void clk_regime_msm_off
(
  clkrgm_msm_clk_type regime
)
{
  INTLOCK();
  /* ------------------------------------------------------------------------
     Turn off specified clock regime(s).
  ------------------------------------------------------------------------ */
  switch( regime )
  {
    case CLK_RGM_GEN_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, XO4_CLK_EN, NOT_ENABLED );
      clk_busy_wait( CLKRGM_XO4_DISABLE_TIME_US );
      clk_regime_msm_log(CLKRGM_MSM_LOG_TCXO4_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_UART0_M:
      CLKRGM_SET_FVAL_NL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE1,
                       DISABLE_UART_DM_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, NOT_ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_SIM_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      /* ground the clock source in the mn counter */
      // CLKRGM_CD_REG_SET_MND( GSBI0_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_UART1_M:
      CLKRGM_SET_FVAL_NL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE1,
                       DISABLE_UART_DM_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI1_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_UART2_M:
      CLKRGM_SET_FVAL_NL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE1,
                       DISABLE_UART_DM_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI2_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_UART_LEGACY_M:
      CLKRGM_SET_FVAL_NL( UART_DM_CD_REG, UART_DM_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( UART_DM_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log( CLKRGM_MSM_LOG_UART1_DM_CLK,
                          CLKRGM_MSM_LOG_TRANSACTION_DISABLE );
      break;

    case CLK_RGM_I2C0_M:
      CLKRGM_SET_FVAL_NL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE2,
                       DISABLE_I2CC_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      /* ground the clock source in the mn counter */
      // CLKRGM_CD_REG_SET_MND( GSBI0_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_I2C1_M:
      CLKRGM_SET_FVAL_NL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE2,
                       DISABLE_I2CC_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI1_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_I2C2_M:
      CLKRGM_SET_FVAL_NL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE2,
                       DISABLE_I2CC_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI2_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;


    case CLK_RGM_I2C_LEGACY_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, I2C_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, I2C_SRC_SEL, EXT_CLK );
      clk_regime_msm_log(CLKRGM_MSM_LOG_I2C_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_RFMS_XO_CLK_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, RFMS_XO_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_XO_DISABLE_TIME_US);

      /* ground the clock source in the mn counter */
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, RFMS_XO_SRC_SEL, EXT_CLK );
      clk_regime_msm_log(CLKRGM_MSM_LOG_RFMS_XO_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_SPI0_M:
      CLKRGM_SET_FVAL_NL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE3,
                       DISABLE_SPI_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_SPI_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI0_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_SPI1_M:
      CLKRGM_SET_FVAL_NL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE3,
                       DISABLE_SPI_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_SPI_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI1_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_SPI2_M:
      CLKRGM_SET_FVAL_NL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE3,
                       DISABLE_SPI_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_SPI_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI2_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_GSBI0_M:
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_GSBI_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI0_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_GSBI1_M:
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_GSBI_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI1_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_GSBI2_M:
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_GSBI_PAUSE);

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( GSBI2_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_ECODEC_M:
      CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODEC_CLK_EN, NOT_ENABLED );
      CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODECIF_CLK_EN, NOT_ENABLED );
      CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODECIF_PO_CLK_EN, NOT_ENABLED );
      clk_busy_wait( CLKRGM_ECODEC_PAUSE );

      /* ground the clock source in the mn counter */
      CLKRGM_CD_REG_SET_MND( ECODEC_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_ECODECIF_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_ADSP_M:
      HWIO_OUTM( ADSP_CFG_REG,
                 HWIO_FMSK( ADSP_CFG_REG, ADSP_CLK_EN)
                 | HWIO_FMSK( ADSP_CFG_REG, ADSP_INTF_CLK_EN),
                 CR_FVAL( ADSP_CFG_REG, ADSP_CLK_EN, NOT_ENABLED )
                 | CR_FVAL( ADSP_CFG_REG, ADSP_INTF_CLK_EN, NOT_ENABLED )
               );
      clk_busy_wait( CLKRGM_ADSP_PAUSE );

      /* TBP: Can save power here if ADSP source is grounded */
      clk_regime_msm_log(CLKRGM_MSM_LOG_ADSP_HM_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

	  case CLK_RGM_ADSP_VIDEO_INTF_M:
    	 HWIO_OUTM( ADSP_CLK_CTL_REG,
                 HWIO_FMSK( ADSP_CLK_CTL_REG, ADSP_VIDEO_INTF_CLK_EN),    
                 CR_FVAL( ADSP_CLK_CTL_REG, ADSP_VIDEO_INTF_CLK_EN, NOT_ENABLED )
                
               );

      clk_busy_wait( CLKRGM_ADSP_PAUSE );

      clk_regime_msm_log(CLKRGM_MSM_LOG_ADSP_HM_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
	  break;

    case CLK_RGM_SBI_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, NOT_ENABLED );
      clk_busy_wait( CLKRGM_XO_DISABLE_TIME_US );
      clk_regime_msm_log(CLKRGM_MSM_LOG_SSBI_PMIC_CLK,
                         CLKRGM_MODEM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_ICODEC_M:
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN, NOT_ENABLED);
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_EN, NOT_ENABLED);
      CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, ICODEC_TX_CLK_EN, NOT_ENABLED);
      clk_busy_wait(CLKRGM_CODEC_PAUSE);

      /* ground clock source in mn counter */
      CLKRGM_CD_REG_SET_MND( ICODEC_RX_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      CLKRGM_CD_REG_SET_MND( ICODEC_TX_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );

      clk_regime_msm_log(CLKRGM_MSM_LOG_ICODEC_2X_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_ICODEC_RX_M:
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN, NOT_ENABLED);
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_EN, NOT_ENABLED);
      clk_busy_wait(CLKRGM_CODEC_PAUSE);

      /* ground clock source in mn counter */
      CLKRGM_CD_REG_SET_MND( ICODEC_RX_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );

      clk_regime_msm_log(CLKRGM_MSM_LOG_ICODEC_2X_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_ICODEC_TX_M:
      CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, ICODEC_TX_CLK_EN, NOT_ENABLED);
      clk_busy_wait(CLKRGM_CODEC_PAUSE);

      /* ground clock source in mn counter */
      CLKRGM_CD_REG_SET_MND( ICODEC_TX_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );

      clk_regime_msm_log(CLKRGM_MSM_LOG_ICODEC_2X_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_USB_MCLK_M:
      CLKRGM_SET_FVAL_NL( USB_CD_REG, USB_FS_CLK_EN, NOT_ENABLED );
      clk_busy_wait(10);
      /* ground clock source in mn counter */
      CLKRGM_CD_REG_SET_MND( USB_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_MCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_SDAC_M:
      CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_BIT_CLK_EN, NOT_ENABLED );
      CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_MSTR_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_SDAC_PAUSE);
      /* ground clock source in mn counter */
      CLKRGM_CD_REG_SET_MND( I2S_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_I2S_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_ETM_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, ETM_CLK_EN, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_ETM_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_CAMCLK_M:
      CLKRGM_SET_FVAL_NL( CAMCLK_CD_REG, CAMCLK_PO_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_CAMCLK_PAUSE);
      /* ground clock source in mn counter */
      HWIO_OUTM( CAMCLK_CD_REG,
                 HWIO_FMSK( CAMCLK_CD_REG, DIV_MND_EN )
                 | HWIO_FMSK( CAMCLK_CD_REG, CAMCLK_PO_SRC_SEL )
                 | HWIO_FMSK( CAMCLK_CD_REG, DIV_MND_MODE ),
                 CR_FVAL(CAMCLK_CD_REG, DIV_MND_EN, NOT_ENABLED)
                 | CR_FVAL(CAMCLK_CD_REG, CAMCLK_PO_SRC_SEL, EXT_CLK)
                 | CR_FVAL(CAMCLK_CD_REG, DIV_MND_MODE, BYPASS) );
      clk_regime_msm_log(CLKRGM_MSM_LOG_CAMCLK_PO,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_CAMIF_PCLK_M:
      /* This is really a source selection, not a clock regime on/off */
      CLKRGM_SET_FVAL_NL( CAMCLK_CD_REG, CAM_PCLK_SRC_SEL, NO_CLOCK );
      break;

    case CLK_RGM_VIDEO_VFE_M:
      CLKRGM_SET_FVAL_NL( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_EN, NOT_ENABLED );
      clk_busy_wait(10);
      /* ground clock source in mn counter */
      HWIO_OUTM( VIDEO_VFE_CD_REG,
                 HWIO_FMSK( VIDEO_VFE_CD_REG, DIV_MND_EN )
                 | HWIO_FMSK( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL )
                 | HWIO_FMSK( VIDEO_VFE_CD_REG, VIDEO_VFE_SRC_SEL )
                 | HWIO_FMSK( VIDEO_VFE_CD_REG, DIV_MND_MODE ),
                 CR_FVAL(VIDEO_VFE_CD_REG, DIV_MND_EN, NOT_ENABLED)
                 | CR_FVAL(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL, GND_TIE )
                 | CR_FVAL(VIDEO_VFE_CD_REG, VIDEO_VFE_SRC_SEL, EXT_CLK)
                 | CR_FVAL(VIDEO_VFE_CD_REG, DIV_MND_MODE, BYPASS) );
      clk_regime_msm_log(CLKRGM_MSM_LOG_VIDEO_VFE_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_VIDEO_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, VIDEO_AHB_CLK_EN, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_VIDEO_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_AUDIO_M:    /* Audio AHB clock                   */
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, AUDIO_AHB_CLK_EN, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_AUDIO_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_USB_AHB_CLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, USB_AHB_CLK_EN, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_OTG_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_SDCC_HCLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, SDCC_AHB_CLK_EN, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_SDCC_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_SDCC_MCLK_M:
      CLKRGM_SET_FVAL_NL( SDCC_CD_REG, SDCC_CLK_EN, NOT_ENABLED );
      clk_busy_wait(CLKRGM_SDCC_PAUSE);

      /* ground clock source in mn counter */
      CLKRGM_CD_REG_SET_MND( SDCC_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_SDCC_MCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_MDP_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, MDP_AHB_CLK_EN, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_MDP_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_QMEMBIST_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, QMEMBIST_CLK_EN, NOT_ENABLED );

      /* ground the clock source in the mn counter */
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, QMEMBIST_SRC_SEL, EXT_CLK );
      clk_regime_msm_log(CLKRGM_MSM_LOG_QMEMBIST_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_USB_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, USB_AHB_CLK_EN, NOT_ENABLED );
      CLKRGM_SET_FVAL_NL( USB_CD_REG, USB_FS_CLK_EN, NOT_ENABLED );
      clk_busy_wait(10);

      /* ground clock source in mn counter */
      CLKRGM_CD_REG_SET_MND( USB_CD_REG, EXT_CLK, BYPASS, NOT_ENABLED );

      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_MCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_OTG_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_SEC_HCLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, SEC_AHB_CLK_EN, NOT_ENABLED );

      clk_regime_msm_log(CLKRGM_MSM_LOG_SEC_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_EBI2_CLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, EBI2_CLK_SRC_EN, NOT_ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_EBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      /* EBI2_IO is branched from EBI2_CLK */
      clk_regime_msm_log(CLKRGM_MSM_LOG_EBI2_IO_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_IMEM_CLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, IMEM_CLK_SRC_EN, NOT_ENABLED );
      clk_regime_msm_log( CLKRGM_MSM_LOG_IMEM_CLK,
                          CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

	case CLK_RGM_EFUSE_CLK:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, EFUSE_CLK_EN, NOT_ENABLED );
      clk_regime_msm_log( CLKRGM_MSM_LOG_EFUSE_CLK,
                          CLKRGM_MSM_LOG_TRANSACTION_DISABLE);
      break;

    case CLK_RGM_MCLK_M:     /* Micro (ARM) clock                 */
    case CLK_RGM_HCLK_M:     /* Bus (AHB) clock                   */
    case CLK_RGM_UXMC_M:     /* UXMC                              */
    case CLK_RGM_EBI1_CLK_M: /* EBI1 clock                        */
      break;
  }
  
  INTFREE();
}


/*===========================================================================

FUNCTION CLK_REGIME_PLL_ON

DESCRIPTION
  Turn selected PLL on and wait for it to warm up.

PARAMETERS
  pll - PLL to turn on.

DEPENDENCIES
  Selected input clock source should be on and stable.

RETURN VALUE
  None.

SIDE EFFECTS
  The selected PLL is turned on.

===========================================================================*/
void clk_regime_pll_on
(
  clkrgm_pll_type pll
)
{
  switch ( pll )
  {
    /* ----------------------------------------------------------------------
       MODEM PLL
    ---------------------------------------------------------------------- */
    case CLKRGM_PLL:
    {
      if (clk_regime_pll_off_ok())
      {

        /* Put the PLL into the reset mode and bypass. */
        HWIO_OUTM( PLL_MODE_REG,
                   HWIO_FMSK(PLL_MODE_REG, RESET_N) 
                   | HWIO_FMSK(PLL_MODE_REG, BYPASSNL)
                   | HWIO_FMSK(PLL_MODE_REG, OUTCTRL),
                   0 );

        /* Program internal PLL M/N/L values */
        HWIO_OUT( PLL_L_VAL_REG,     clkrgm_pll_cfg.l_reg_val );
        HWIO_OUT( PLL_M_VAL_REG,     clkrgm_pll_cfg.m_reg_val );
        HWIO_OUT( PLL_N_VAL_REG,     clkrgm_pll_cfg.n_reg_val );
        HWIO_OUT( PLL_TEST_CTL_REG,  clkrgm_pll_cfg.test_ctrl_val );
        HWIO_OUT( PLL_CONFIG_REG,    clkrgm_pll_cfg.config_reg_val );

        /* Take the PLL out of Bypass */
        CLKRGM_SET( PLL_MODE_REG, BYPASSNL );

        /* Wait 5 usec */
        clk_busy_wait(5);  

        /* Take the PLL out of reset */
        CLKRGM_SET( PLL_MODE_REG, RESET_N );

        /* Wait for the PLL warmup time. */
        clk_busy_wait( PLL_WARMUP_TIME_US );

        /* Put the PLL into the active state. */
        CLKRGM_SET( PLL_MODE_REG, OUTCTRL );

        /* Update PLL state variable. */
        clkrgm.pll_state = CLKRGM_PLL_ON;
      }

      break;
    } 
    
    /* ----------------------------------------------------------------------
       Unsupported PLL selections are ignored.
    ---------------------------------------------------------------------- */
    default:
    {
      ERR("PLL not supported!",0,0,0);
      break;
    }
  }
}

boolean clk_regime_pll_is_on(clkrgm_pll_type pll)
{
  if (pll == CLKRGM_PLL)
  {
    return (HWIO_IN(PLL_MODE_REG) == 
      (HWIO_FMSK(PLL_MODE_REG, OUTCTRL)  |
       HWIO_FMSK(PLL_MODE_REG, BYPASSNL) |
       HWIO_FMSK(PLL_MODE_REG, RESET_N)));
  }
  return TRUE;
}

static boolean clk_regime_pll_off_ok (void)
{
  uint32                  arm_src_sel;
  uint32                  src_sel;
  boolean                 result=TRUE;

  arm_src_sel = HWIO_INF(ARM_CFG_REG, ARM_SRC_SEL);
  if (arm_src_sel == 0)
  {
    src_sel = HWIO_INF(ARM_CFG_REG, SRC_SEL0);
  }
  else
  {
    src_sel = HWIO_INF(ARM_CFG_REG, SRC_SEL1);
  }

  switch (src_sel)
  {
    case HWIO_FVAL(ARM_CFG_REG, SRC_SEL0, PLL_SRC):
      result = FALSE;
      break;
    default:
      break;
  }
  return result;
}


/*===========================================================================

FUNCTION CLK_REGIME_PLL_OFF

DESCRIPTION
  Selected PLL will enter off mode and will be completely shut down. Output
  and DC power in the core are gated off.

PARAMETERS
  pll - PLL to turn off.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The selected PLL is turned off.

===========================================================================*/
void clk_regime_pll_off
(
  clkrgm_pll_type pll
)
{
  switch ( pll )
  {
    /* ----------------------------------------------------------------------
       PLL
    ---------------------------------------------------------------------- */
    case CLKRGM_PLL:
    {

      if (clk_regime_pll_off_ok())
      {
        /* Shut down PLL */
        HWIO_OUTM( PLL_MODE_REG, HWIO_RMSK(PLL_MODE_REG), 0);

        /* Program PLL shutdown values. */
        CLKRGM_W( PLL_TEST_CTL_REG, 0x0);
        CLKRGM_W( PLL_CONFIG_REG, 0x0);

        /* Update PLL state variable. */
        clkrgm.pll_state = CLKRGM_PLL_OFF;
      }

      break;
    }

    default:
    {
      ERR("PLL not supported!",0,0,0);
      break;
    }
  }
}


/*==========================================================================

  FUNCTION      CLK_REGIME_PLL_REQUEST

  DESCRIPTION   Request that a specified PLL be enabled or disabled on behalf
                of the specified client.  A voting mechanism is implemented
                to make sure that each PLL is on as long as there is at least
                one active client.

  PARAMETERS    pll       - PLL to turn on or off
                client    - Requesting client (clock sourced by the PLL)
                request   - Request to turn on or off the PLL

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  The PLL is turned on or off as applicable.

==========================================================================*/

void clk_regime_pll_request (
  clkrgm_pll_type         pll,
  int                     client,
  clkrgm_pll_op_type      request)
{
  if(client >= CLK_RGM_MAX_REGIMES) {
  	MSG_ERROR("Invalid Client : client=%d", client, 0, 0);
  	return;
  }
  	
  switch (pll)
  {
    case CLKRGM_PLL:
      switch (request)
      {
        case CLKRGM_PLL_DISABLE:
		  if (clkrgm.pll_clients[client])
          {
            clkrgm.pll_clients[client] = FALSE;
            clkrgm.pll_usage--;
          }
          if ((clkrgm.pll_state == CLKRGM_PLL_ON) &&
              (clk_regime_pll_usage_count (CLKRGM_PLL) == 0))
          {
            clk_regime_pll_off (CLKRGM_PLL); 
          }
          break;

        case CLKRGM_PLL_ENABLE:
	   	  if ( !clkrgm.pll_clients[client] )
          {
            clkrgm.pll_clients[client] = TRUE;
            clkrgm.pll_usage++;
          }
          if (clkrgm.pll_state == CLKRGM_PLL_OFF)
          {
            clk_regime_pll_on (CLKRGM_PLL); 
          }
          break;
          
        default:
            break;
      }
      break;

    default:
      break;
  }

} /* clk_regime_pll_request */


/*==========================================================================

  FUNCTION      CLK_REGIME_PLL_USAGE_COUNT

  DESCRIPTION   Return the usage counter of the specified PLL.

  PARAMETERS    pll       - PLL

  DEPENDENCIES  None.

  RETURN VALUE  The usage counter of the specified PLL.

  SIDE EFFECTS  None.

==========================================================================*/

int clk_regime_pll_usage_count (
  clkrgm_pll_type pll)
{
  int count = 0;

  /* Validate input arguments */
  if (pll >= CLKRGM_NUM_PLLS)
  {
    /* Error: PLL out of range */
    ERR ("clk_regime_pll_usage_count: PLL out of range (%d)",
         pll, 0, 0);
    return 0;
  }
  
  switch (pll)
  {
    case CLKRGM_PLL:
      count = (clkrgm.pll_usage);
      break;
  
    default:
      break;
  }

  return count;
} /* clk_regime_pll_usage_count */

/*===========================================================================

FUNCTION CLK_REGIME_MSM_IS_ON

DESCRIPTION
  This function will determine if the specified clock regime is on or off.

PARAMETERS
  regime - clock regime to query.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if enabled, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean clk_regime_msm_is_on
(
  clkrgm_msm_clk_type regime
)
{
  boolean is_on = FALSE;

  switch( regime )
  {
    case CLK_RGM_GEN_M:
      if ( CLKRGM_GET( CLK_OFF_B, XO4_CLK ) == 0  ) is_on = TRUE;
      break;

    case CLK_RGM_GSBI0_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI0_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_GSBI1_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI1_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_GSBI2_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI2_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_UART0_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI0_CLK ) == 0  &&
           CLKRGM_GET( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE1 ) == 1 ) is_on = TRUE;
      break;

    case CLK_RGM_UART1_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI1_CLK ) == 0  &&
           CLKRGM_GET( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE1 ) == 1 ) is_on = TRUE;
      break;

    case CLK_RGM_UART2_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI2_CLK ) == 0  &&
           CLKRGM_GET( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE1 ) == 1 ) is_on = TRUE;
      break;

    case CLK_RGM_UART_LEGACY_M:
      if ( CLKRGM_GET( CLK_OFF_B, UART_DM_CLK ) == 0  ) is_on = TRUE;
      break;

    case CLK_RGM_ECODEC_M:
      if ( CLKRGM_GET( CLK_OFF_B, ECODECIF_CLK ) == 0  &&
           CLKRGM_GET( CLK_OFF_B, ECODEC_CLK ) == 0    &&
           CLKRGM_GET( CLK_OFF_B, ECODECIF_PO_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_ADSP_M:
      if ( CLKRGM_GET( CLK_OFF_B, ADSP_CLK ) == 0 ) is_on = TRUE;
      break;

	case CLK_RGM_ADSP_VIDEO_INTF_M:
      if ( CLKRGM_GET( CLK_OFF_B, ADSP_VIDEO_INTF_CLK) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_ICODEC_M:
      if ( CLKRGM_GET( CLK_OFF_B, ICODEC_2X_RX_CLK ) == 0  ) is_on = TRUE;
      break;

    case CLK_RGM_USB_M:
      if ( CLKRGM_GET( CLK_OFF_B, USB_FS_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_ETM_M:
      if ( CLKRGM_GET( CLK_OFF_A, ETM_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_I2C0_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI0_CLK ) == 0  &&
           CLKRGM_GET( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE2 ) == 1 ) is_on = TRUE;
      break;

    case CLK_RGM_I2C1_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI1_CLK ) == 0  &&
           CLKRGM_GET( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE2 ) == 1 ) is_on = TRUE;
      break;

    case CLK_RGM_I2C2_M:
      if ( CLKRGM_GET( CLK_OFF_B, GSBI2_CLK ) == 0  &&
           CLKRGM_GET( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE2 ) == 1 ) is_on = TRUE;
      break;

    case CLK_RGM_I2C_LEGACY_M:
      if ( CLKRGM_GET( CLK_OFF_B, I2C_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_RFMS_XO_CLK_M:
      if ( CLKRGM_GET( CLK_OFF_B, RFMS_XO_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_CAMCLK_M:
      if ( CLKRGM_GET( CLK_OFF_B, CAMCLK_PO ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_CAMIF_PCLK_M:
      if ( HWIO_INM(CAMCLK_CD_REG, HWIO_FMSK(CAMCLK_CD_REG, CAM_PCLK_SRC_SEL))
           != HWIO_FVAL(CAMCLK_CD_REG, CAM_PCLK_SRC_SEL, NO_CLOCK ) )
        is_on = TRUE;

    case CLK_RGM_VIDEO_VFE_M:
      if ( CLKRGM_GET( CLK_OFF_B, VIDEO_VFE_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_VIDEO_M:
      if ( CLKRGM_GET( CLK_OFF_A, VIDEO_AHB_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_USB_AHB_CLK_M:
      if ( CLKRGM_GET( CLK_OFF_A, USB_AHB_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_SDCC_HCLK_M:
      if ( CLKRGM_GET( CLK_OFF_A, SDCC_AHB_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_SDCC_MCLK_M:
      if ( CLKRGM_GET( CLK_OFF_B, SDCC_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_MDP_M:
      if ( CLKRGM_GET( CLK_OFF_A, MDP_AHB_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_QMEMBIST_M:
      if ( CLKRGM_GET( CLK_OFF_B, QMEMBIST_CLK ) == 0 ) is_on = TRUE;
      break;

    case CLK_RGM_EFUSE_CLK:
      if ( CLKRGM_GET( CLK_OFF_B, EFUSE_CLK) == 0  ) is_on = TRUE;
      break; 

    case CLK_RGM_NOT_SUPPORTED:
      break;

    default:
      ERR_FATAL("Request for unsupported clock regime", 0,0,0);
  }

  return is_on;
}


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

boolean clk_regime_is_on (clkrgm_modem_clk_type clk)
{
  boolean is_on = FALSE;

  switch (clk)
  {
    case CLKRGM_CHIPXN_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, CDMA_CHIPXN_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_ANT_SAMP_CHIPXN_CLK:
    case CLKRGM_CHIPXN_DIV2_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, CDMA_CHIPXN_DIV2_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_CHIPXN_DIV4_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, CDMA_CHIPXN_DIV4_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_RXF_SAMP_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, RXF_SAMP_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_FB_ANT0_SAMP_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, FB_ANT0_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_FB_ANT1_SAMP_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, FB_ANT1_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_RXF_SAMP_REF_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD_2, RXF_SAMP_REF_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_EDGE_TX_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, EDGE_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_EDGE_RX_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, EDGE_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_GSM_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, GSM_CLK_OFF) == 0)
        is_on = TRUE;
      break;
    #endif /* CLKRGM_INCLUDE_GSM */

    case CLKRGM_TX_PDM_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, TX_PDM_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_TCXO_PDM_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, TCXO_PDM_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_MDSP_CLK:
      if (CLKRGM_GET(MODEM_CLK_HALT, MDSP_CLK_HALT) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_SLEEP_CLK:
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF1_CLK:
      if (CLKRGM_GET(MODEM_CLK_HALT, CDMA_SLPFAST_CHIPXN_REF1_HALT) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_SLPFAST_CHIPXN_REF2_CLK:
      if (CLKRGM_GET(MODEM_CLK_HALT, CDMA_SLPFAST_CHIPXN_REF2_HALT) == 0)
        is_on = TRUE;
      break;

    #ifdef CLKRGM_INCLUDE_GSM
    case CLKRGM_SLPFAST_GSM_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, SLPFAST_GSM_CLK_OFF) == 0)
        is_on = TRUE;
      break;
    #endif /* CLKRGM_INCLUDE_GSM */

    case CLKRGM_SLPFAST_FEE_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, SLPFAST_FEE_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_BT_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, BT_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_MODEM_SBI_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, MODEM_SBI_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_MODEM_WEB_CLK:
      break;

    case CLKRGM_OFFLINE_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, OFFLINE_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    #ifdef CLKRGM_INCLUDE_GPS
    case CLKRGM_GACC_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, GACC_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_GPS_CHIPX8_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, GPS_CHIPX8_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_GPS_CHIPX16_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD_2, GPS_CHIPX16_CLK_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_GPS_CHIPX32_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD_2, GPS_CHIPX32_CLK_OFF) == 0)
        is_on = TRUE;
      break;
    #endif /* CLKRGM_INCLUDE_GPS */

    #ifdef CLKRGM_INCLUDE_WCDMA
    case CLKRGM_F9_CLK:
      if (CLKRGM_GET(MODEM_CLK_STATUS_RD, F9_CLK_OFF) == 0)
        is_on = TRUE;
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
      break;

    case CLKRGM_DEC_CLK:
      if (CLKRGM_GET(DEINT_CLK_STATUS, DEINT_CLK_IS_OFF) != 0xF)
        is_on = TRUE;
      break;

    #ifdef CLKRGM_INCLUDE_HDR
    case CLKRGM_HDR_TDEC_CLK:
      if (CLKRGM_GET(TD_CLK_STATUS, TD_CLK_IS_OFF) == 0)
        is_on = TRUE;
      break;
    #endif /* CLKRGM_INCLUDE_HDR */

    #ifdef CLKRGM_INCLUDE_TD  // need to define for tier that supports TD
    case CLKRGM_TURBO_DEC_CLK:
      if (CLKRGM_GET(TD_CLK_STATUS, TD_CLK_IS_OFF) == 0)
        is_on = TRUE;
      break;
    #endif

    case CLKRGM_SVD_CLK:
      if (CLKRGM_GET(VD_CLK_STATUS, VD_CLK_IS_OFF) == 0)
        is_on = TRUE;
      break;

    case CLKRGM_CHIPX8_CLK:
      break;

    case CLKRGM_CDMA_TX_CLK:
      if (CLKRGM_GET(MOD_CLK_STATUS, MOD_CX8_CLK_IS_OFF) == 0) is_on = TRUE;
      break;

    case CLKRGM_CDMA_RX_CLK:
      break;

    case CLKRGM_RXCHIPX8_CLK:
      if(CLKRGM_GET(DEM1X_CX8_CLK_STATUS, CX8_CLK_IS_OFF) == 0) is_on = TRUE;
      break;

    case CLKRGM_MODEM_ALL_CLK:
      break;

    case CLKRGM_OBSOLETE_CLK:
      break;

    default:
      /* Error: Clock branch of out range */
      ERR("clk_regime_is_on: Clock branch out of range (%d)", clk, 0, 0);
      break;
  }

  return is_on;

} /* clk_regime_is_on */


/*==========================================================================

  FUNCTION      CLK_REGIME_DETECT_CPU_CLK

  DESCRIPTION   This function detect the current speed of the CPU.

  PARAMETERS    None.

  DEPENDENCIES  None.

  RETURN VALUE  The current CPU clock frequency.

  SIDE EFFECTS  None.

==========================================================================*/

clkrgm_cpu_speed_type clk_regime_detect_cpu_clk (void)
{
  uint32                  arm_src_sel;
  uint32                  global_src;
  uint32                  global_div;
  uint32                  arm_rate_sel;
  clkrgm_cpu_speed_type   cpu_clk;

  arm_src_sel = HWIO_INF(ARM_CFG_REG, ARM_SRC_SEL);
  if (arm_src_sel == 0)
  {
    global_src = HWIO_INF(ARM_CFG_REG, SRC_SEL0);
    global_div = HWIO_INF(ARM_MOD_REG, ARM_DIV_MOD0);
  }
  else
  {
    global_src = HWIO_INF(ARM_CFG_REG, SRC_SEL1);
    global_div = HWIO_INF(ARM_MOD_REG, ARM_DIV_MOD1);
  }

  arm_rate_sel = HWIO_INF(ARM_CFG_REG, ARM_RATE_SEL);

  for( cpu_clk = CLKRGM_CPU_SLPOSC_SPEED;
       cpu_clk < CLKRGM_CPU_NUM_SPEEDS;
       cpu_clk++)
  {
    if( (clkrgm_cpu_cfg[cpu_clk].global_cfg.src == global_src)
        && (clkrgm_cpu_cfg[cpu_clk].global_cfg.div == global_div)
        && (clkrgm_cpu_cfg[cpu_clk].mclk_div == arm_rate_sel) )
    {
      break;
    }
  }

  return cpu_clk;
} /* clk_regime_detect_cpu_clk */
/*===========================================================================

  FUNCTION CLK_REGIME_SWITCH_MCLK_MAX

  DESCRIPTION
    Switch the MCLK source to maximum.

  PARAMETERS
    None.

  DEPENDENCIES
    This function can only be used if memroy controller parameters have
    previousely been configured for max speed.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.

===========================================================================*/
void clk_regime_switch_mclk_max (void)
{
  clkrgm.cpu_speed = clk_regime_detect_cpu_clk();
  if( clkrgm.cpu_speed == CLKRGM_CPU_NUM_SPEEDS)
  {
    ERR_FATAL("Unknown CPU speed detected", 0,0,0);
  }

  if (clkrgm.cpu_speed != CLKRGM_CPU_FULL_SPEED)
  {

    /* make sure PLL is on */
    if ( clkrgm_cpu_cfg[ CLKRGM_CPU_FULL_SPEED ].global_cfg.src == 
         CLKRGM_MCLK_SRC_PLL )
    {
      clk_regime_pll_request( CLKRGM_PLL, (int)CLKRGM_PLL_CPU_CLIENT,
                              CLKRGM_PLL_ENABLE );
    }
  clk_regime_switch_mclk( CLKRGM_CPU_FULL_SPEED );
  } 

#ifdef FEATURE_PLL_384

  if (clkrgm.cpu_speed != CLKRGM_CPU_128_SPEED)
  	{
	  /* make sure PLL is on */
		  if ( clkrgm_cpu_cfg[ CLKRGM_CPU_128_SPEED ].global_cfg.src == 
			   CLKRGM_MCLK_SRC_PLL )
		  {
			clk_regime_pll_request(CLKRGM_PLL, (int)CLKRGM_PLL_CPU_CLIENT,
									CLKRGM_PLL_ENABLE );
		  }
		clk_regime_switch_mclk(CLKRGM_CPU_128_SPEED);

  }
#endif  
}

/*===========================================================================
FUNCTION  CLKRGM_CLK_OFFA_OUTM

DESCRIPTION
  This function sets ARM_CLK_EN_REG to the on/off states passed in as determined by a
  previous read from CLK_OFF_A.  NOTE: The bits are *mostly* one to one between these
  two registers, but remapping is done here for the bits that trade places with other
  bits.

===========================================================================*/
void clkrgm_clk_off_a_outm( uint32 clk_off_a_mask, uint32 clk_off_a_value )
{
  uint32 mask, value;

  /* In CLK_OFF_A, 0 = clock on, 1 = clock off, but for all the enable bits in the 
     following registers, 1 is on, and 0 is off, so invert clk_off_a_value */
  clk_off_a_value = ~clk_off_a_value;

  /* Strip off one bit in CLK_OFF_A, shift it to bit 0, then shift it up to 
     the appropriate spot in ARM_CLK_EN_REG (appending _EN to the field) */
#define CLKRGM_OFFA_REARRANGE(x,y) \
  (((x) & HWIO_FMSK(CLK_OFF_A, y)) >> HWIO_SHFT(CLK_OFF_A, y) << HWIO_SHFT(ARM_CLK_EN_REG,y##_EN))

  mask = 0;
  value = 0;

  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, ADSP_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, ADSP_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, AHB_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, AHB_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, ARM_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, ARM_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, AUDIO_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, AUDIO_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, EBI1_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, EBI1_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, EBI1_IO_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, EBI1_IO_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, EBI2_CLK_SRC);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, EBI2_CLK_SRC);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, ETM_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, ETM_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, GSBI_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, GSBI_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, IMEM_CLK_SRC);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, IMEM_CLK_SRC);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, MDP_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, MDP_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, MODEM_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, MODEM_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, MTI_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, MTI_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, PERIPH_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, PERIPH_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, PERPH_WEB_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, PERPH_WEB_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, SDCC_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, SDCC_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, SEC_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, SEC_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, TLMM_GPIO_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, TLMM_GPIO_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, UART_DM_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, UART_DM_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, USB_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, USB_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, UXMC_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, UXMC_AHB_CLK);
  mask |= CLKRGM_OFFA_REARRANGE(clk_off_a_mask, VIDEO_AHB_CLK);
  value |= CLKRGM_OFFA_REARRANGE(clk_off_a_value, VIDEO_AHB_CLK);

  HWIO_OUTM(ARM_CLK_EN_REG, mask, value);
}

/*===========================================================================
FUNCTION  CLKRGM_CLK_OFF_B_OUTM

DESCRIPTION
  This function sets a wide variety of registers to the on/off states passed in as 
  determined by a previous read from CLK_OFF_B.  Many registers can be touched by this
  as the enable bits are scattered among the *_CD_REG and others.

===========================================================================*/
void clkrgm_clk_off_b_outm( uint32 clk_off_b_mask, uint32 clk_off_b_value )
{
  uint32 mask=0, value=0;

  /* In CLK_OFF_A, 0 = clock on, 1 = clock off, but for all the enable bits in the 
     following registers, 1 is on, and 0 is off, so invert clk_off_a_value */
  clk_off_b_value = ~clk_off_b_value;

  /* A macro to strip out one bit from OFF_B, and reposition it to the appropriate
     position in the writeable register "wreg" */

  #define CLK_OFFB_REARRANGE(offb_bit, wreg, wreg_bit) \
    ((clk_off_b_value) & HWIO_FMSK(CLK_OFF_B, offb_bit)) >> HWIO_SHFT(CLK_OFF_B, offb_bit) \
                                           << HWIO_SHFT(wreg, wreg_bit)


  if(clk_off_b_mask & HWIO_FMSK( CLK_OFF_B, ADSP_CLK ))
    HWIO_OUTM( ADSP_CFG_REG,
               HWIO_FMSK( ADSP_CFG_REG, ADSP_CLK_EN ), 
               CLK_OFFB_REARRANGE( ADSP_CLK, ADSP_CFG_REG, ADSP_CLK_EN ));

  if(clk_off_b_mask & HWIO_FMSK( CLK_OFF_B, ADSP_VIDEO_INTF_CLK ))
    HWIO_OUTM( ADSP_CLK_CTL_REG, 
               HWIO_FMSK( ADSP_CLK_CTL_REG, ADSP_VIDEO_INTF_CLK_EN ), 
               CLK_OFFB_REARRANGE( ADSP_VIDEO_INTF_CLK, ADSP_CLK_CTL_REG,
                                   ADSP_VIDEO_INTF_CLK_EN ));

  /* Gather the ECODEC_CD_REG modifications */
  if(clk_off_b_mask & HWIO_FMSK( CLK_OFF_B, ECODECIF_PO_CLK )) 
  {
    mask |= HWIO_FMSK(ECODEC_CD_REG, ECODECIF_PO_CLK_EN);
    value |= CLK_OFFB_REARRANGE( ECODECIF_PO_CLK, ECODEC_CD_REG, ECODECIF_PO_CLK_EN);
  }

  if(clk_off_b_mask & HWIO_FMSK( CLK_OFF_B, ECODECIF_CLK ))
  {
    mask |= HWIO_FMSK( ECODEC_CD_REG, ECODECIF_CLK_EN ); 
    value |= CLK_OFFB_REARRANGE( ECODECIF_CLK, ECODEC_CD_REG, ECODECIF_CLK_EN);
  }

  if(clk_off_b_mask & HWIO_FMSK( CLK_OFF_B, ECODEC_CLK ))
  {
    mask |= HWIO_FMSK( ECODEC_CD_REG, ECODEC_CLK_EN );
    value |= CLK_OFFB_REARRANGE( ECODEC_CLK, ECODEC_CD_REG, ECODEC_CLK_EN );
  }

  /* Write the ECODEC_CD_REG modifications */
  if(mask) 
  {
    HWIO_OUTM( ECODEC_CD_REG, mask, value);
    mask = 0;
    value = 0;
  }

  /* Gather up the GSBI0_CD_REG clock changes */
  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, GSBI0_SIM_CLK))
  {
    mask |= HWIO_FMSK( GSBI0_CD_REG, GSBI0_SIM_CLK_EN );
    value |= CLK_OFFB_REARRANGE( GSBI0_SIM_CLK, GSBI0_CD_REG, GSBI0_SIM_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, GSBI0_CLK))
  {
    mask |= HWIO_FMSK( GSBI0_CD_REG, GSBI0_CLK_EN ), 
    value |= CLK_OFFB_REARRANGE( GSBI0_CLK, GSBI0_CD_REG, GSBI0_CLK_EN );
  }

  /* Write the GSBI0_CD_REG clock changes */
  if(mask) 
  {
    HWIO_OUTM( GSBI0_CD_REG, mask, value );
    mask = 0;
    value = 0;
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, GSBI1_CLK))
    HWIO_OUTM( GSBI1_CD_REG, 
               HWIO_FMSK( GSBI1_CD_REG, GSBI1_CLK_EN ), 
               CLK_OFFB_REARRANGE(GSBI1_CLK, GSBI1_CD_REG, GSBI1_CLK_EN ) );

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, GSBI2_CLK))
    HWIO_OUTM( GSBI2_CD_REG, 
               HWIO_FMSK( GSBI2_CD_REG, GSBI2_CLK_EN ),
               CLK_OFFB_REARRANGE( GSBI2_CLK, GSBI2_CD_REG, GSBI2_CLK_EN ) );

  /* Gather up the I2S_CD_REG clock changes */
  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, I2S_BIT_CLK)) 
  {
    mask |= HWIO_FMSK( I2S_CD_REG, I2S_BIT_CLK_EN );
    value |= CLK_OFFB_REARRANGE( I2S_BIT_CLK, I2S_CD_REG, I2S_BIT_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, I2S_MSTR_CLK))
  {
    mask |= HWIO_FMSK( I2S_CD_REG, I2S_MSTR_CLK_EN );
    value |= CLK_OFFB_REARRANGE( I2S_MSTR_CLK, I2S_CD_REG, I2S_MSTR_CLK_EN );
  }

  /* Write the I2S_CD_REG clock changes */
  if(mask) 
  {
    HWIO_OUTM( I2S_CD_REG, mask, value );
    mask = 0;
    value = 0;
  }

  /* Gather up the ICODEC_RX_CD_REG clock changes */
  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, ICODEC_RX_CLK))
  {
    mask |= HWIO_FMSK( ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN );
    value |= CLK_OFFB_REARRANGE( ICODEC_RX_CLK, ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, ICODEC_2X_RX_CLK))
  {
    mask |= HWIO_FMSK( ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_EN );
    value |= CLK_OFFB_REARRANGE( ICODEC_2X_RX_CLK, ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_EN );
  }

  /* Write the ICODEC_RX_CD_REG clock changes */
  if(mask) 
  {
    HWIO_OUTM( ICODEC_RX_CD_REG, mask, value );
    mask = 0;
    value = 0;
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, ICODEC_TX_CLK))
    HWIO_OUTM( ICODEC_TX_CD_REG,
               HWIO_FMSK( ICODEC_TX_CD_REG, ICODEC_TX_CLK_EN ), 
               CLK_OFFB_REARRANGE( ICODEC_TX_CLK, ICODEC_TX_CD_REG, ICODEC_TX_CLK_EN ));
               

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, SDCC_CLK))
    HWIO_OUTM( SDCC_CD_REG, 
               HWIO_FMSK( SDCC_CD_REG, SDCC_CLK_EN ), 
               CLK_OFFB_REARRANGE( SDCC_CLK, SDCC_CD_REG, SDCC_CLK_EN ));

#if 0
  /* CLK_OFF_B has no indicator for EXTERNAL_CLK, so I'll skip this. */
  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, EXTERNAL_CLK))
    HWIO_OUTM( MISC_CFG_REG, 
               HWIO_FMSK( MISC_CFG_REG, EXTERNAL_CLK_EN ), 
               CLK_OFFB_REARRANGE( EXTERNAL_CLK, MISC_CFG_REG, EXTERNAL_CLK_EN ));
#endif

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, RFMS_XO_CLK))
  {
    mask |= HWIO_FMSK( MISC_CFG_REG, RFMS_XO_CLK_EN );
    value |= CLK_OFFB_REARRANGE( RFMS_XO_CLK, MISC_CFG_REG, RFMS_XO_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, I2C_CLK))
  {
    mask |= HWIO_FMSK( MISC_CFG_REG, I2C_CLK_EN );
    value |= CLK_OFFB_REARRANGE( I2C_CLK, MISC_CFG_REG, I2C_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, RXADC_CLK))
  {
    mask |= HWIO_FMSK( MISC_CFG_REG, RXADC_CLK_EN );
    value |= CLK_OFFB_REARRANGE( RXADC_CLK, MISC_CFG_REG, RXADC_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, XO4_CLK))
  {
    mask |= HWIO_FMSK( MISC_CFG_REG, XO4_CLK_EN );
    value |= CLK_OFFB_REARRANGE( XO4_CLK, MISC_CFG_REG, XO4_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, SSBI_PMIC_CLK))
  {
    mask |= HWIO_FMSK( MISC_CFG_REG, SSBI_PMIC_CLK_EN );
    value |= CLK_OFFB_REARRANGE( SSBI_PMIC_CLK, MISC_CFG_REG, SSBI_PMIC_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, QMEMBIST_CLK))
  {
    mask |= HWIO_FMSK( MISC_CFG_REG, QMEMBIST_CLK_EN );
    value |= CLK_OFFB_REARRANGE( QMEMBIST_CLK, MISC_CFG_REG, QMEMBIST_CLK_EN );
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, EFUSE_CLK))
  {
    mask |= HWIO_FMSK( MISC_CFG_REG, EFUSE_CLK_EN );
    value |= CLK_OFFB_REARRANGE( EFUSE_CLK, MISC_CFG_REG, EFUSE_CLK_EN );
  }

  /* Write the MISC_CFG_REG clock changes */
  if(mask) 
  {
    HWIO_OUTM( MISC_CFG_REG, mask, value );
    mask = 0;
    value = 0;
  }

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, UART_DM_CLK))
    HWIO_OUTM( UART_DM_CD_REG,
               HWIO_FMSK( UART_DM_CD_REG, UART_DM_CLK_EN ), 
               CLK_OFFB_REARRANGE( UART_DM_CLK, UART_DM_CD_REG, UART_DM_CLK_EN ) );

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, USB_FS_CLK))
    HWIO_OUTM( USB_CD_REG,
               HWIO_FMSK( USB_CD_REG, USB_FS_CLK_EN ), 
               CLK_OFFB_REARRANGE( USB_FS_CLK, USB_CD_REG, USB_FS_CLK_EN ) );

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, CAMCLK_PO))
    HWIO_OUTM( CAMCLK_CD_REG,
               HWIO_FMSK( CAMCLK_CD_REG, CAMCLK_PO_EN ), 
               CLK_OFFB_REARRANGE( CAMCLK_PO, CAMCLK_CD_REG, CAMCLK_PO_EN ) );

  if(clk_off_b_mask & HWIO_FMSK(CLK_OFF_B, VIDEO_VFE_CLK))
    HWIO_OUTM( VIDEO_VFE_CD_REG,
               HWIO_FMSK( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_EN ), 
               CLK_OFFB_REARRANGE( VIDEO_VFE_CLK, VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_EN ) );

}

/*===========================================================================

FUNCTION  CLKRGM_SET_USB_CHG_MODE

DESCRIPTION
  This function initializes the MSM into a low power state to improve the USB
  charging capabilities at boot up. It puts the MSM into low power mode, and
  takes it out of low power mode by restoring the registers that it touches.
  When MSM is in low power mode, CPU clock rate is switched down, and turns off
  the DCPLL, in order to save power. 

DEPENDENCIES
  NONE
  
RETURN VALUE
  TRUE if successful in setting the mode, FALSE otherwise

SIDE EFFECTS
  Clock speed, clock regimes may be changed.

===========================================================================*/
#ifdef CLKRGM_TEST
#error code not present
#else
#define clk_off_a_mask CLKRGM_REG_A_ALL_MASK
#define clk_off_b_mask CLKRGM_REG_B_ALL_MASK
#endif

boolean clkrgm_set_usb_chg_mode(clkrgm_usb_chg_mode_type mode)
{
  static dword clkrgm_clk_off_a=0;
  static dword clkrgm_clk_off_b=0;
  static clkrgm_cpu_speed_type clkrgm_micro_speed=0;

  if ((mode == CLKRGM_USB_CHG_MODE) && (clkrgm_usb_chg_mode == CLKRGM_USB_NORMAL_MODE))
  {
    clkrgm_micro_speed = clk_regime_detect_cpu_clk();

    /* change micro clock to XO  */
    clk_regime_switch_mclk(CLKRGM_CPU_XO_SPEED);

    clkrgm_clk_off_a = HWIO_IN(CLK_OFF_A);
    clkrgm_clk_off_b = HWIO_IN(CLK_OFF_B);
    clkrgm_clk_off_a_outm( clk_off_a_mask, 0x0 );
    clkrgm_clk_off_b_outm( clk_off_b_mask, 0x0 );

    /* then turn off the PLL */
    clk_regime_pll_off( CLKRGM_PLL ); 

    clkrgm_usb_chg_mode = CLKRGM_USB_CHG_MODE;
  }
  else if ((mode == CLKRGM_USB_NORMAL_MODE) && (clkrgm_usb_chg_mode == CLKRGM_USB_CHG_MODE))
  {
    /* turn on PLL */
    clk_regime_pll_on (CLKRGM_PLL); 

    clkrgm_clk_off_a_outm( clk_off_a_mask, clkrgm_clk_off_a );
    clkrgm_clk_off_b_outm( clk_off_b_mask, clkrgm_clk_off_b );

    /* switch up to PLL based micro clock frequency */
    clk_regime_switch_mclk(clkrgm_micro_speed);

    clkrgm_usb_chg_mode = CLKRGM_USB_NORMAL_MODE;
  }

  /* if mode is same as requested mode, then success */
  return (clkrgm_usb_chg_mode == mode);
}

/*===========================================================================

FUNCTION  CLKRGM_GET_USB_CHG_MODE

DESCRIPTION
  This function returns whether USB CHG code has put the MSM into
  CLKRGM_USB_CHG_MODE, or if it is in CLKRGM_USB_NORMAL_MODE

DEPENDENCIES
  NONE
  
RETURN VALUE
  Returns the state of the MSM from USB CHG perspective   

SIDE EFFECTS
  None.

===========================================================================*/
clkrgm_usb_chg_mode_type clkrgm_get_usb_chg_mode(void)
{
  return clkrgm_usb_chg_mode;
}

extern void clk_regime_set_core_voltage(uint16, uint16);

/*===========================================================================

FUNCTION CLK_REGIME_USB_XTAL_OFF

DESCRIPTION
  USB 48 Mhz crystal is completely shut down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_regime_usb_xtal_off( void )
{
  /* Disable USB clock regime */
  clk_regime_msm_disable_client(CLK_RGM_USB_M, CLKRGM_USB);

  /* Update the local state variable. */
  usb_xtal_on = FALSE;

#if defined (FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
  /* Rely on DCVS on flag to prevent failure at init time. If DCVS is on 
   * at init time, then enc_init() will fail because set core voltage
   * will power down the MODEM clocks when it lowers voltage to 1.2V.
  */
  if (clk_regime_dyn_voltage_on)
    clk_regime_set_core_voltage( MSMC_VOLTAGE_HIGH_V, MSMC_VOLTAGE_LOW_V );
#endif

}


/*===========================================================================

FUNCTION CLK_REGIME_USB_XTAL_ON

DESCRIPTION
  USB 48 Mhz crystal is powered up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void clk_regime_usb_xtal_on( void )
{
  /* Update the local state variable. */
  usb_xtal_on = TRUE;

#if defined (FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) \
    || defined (FEATURE_CLKREGIM_DYNAMIC_VOLTAGE)
  if (clk_regime_dyn_voltage_on)
    clk_regime_set_core_voltage( MSMC_VOLTAGE_LOW_V, MSMC_VOLTAGE_HIGH_V );
#endif

  /* Enable USB clock regime */
  clk_regime_msm_enable_client( CLK_RGM_USB_M, CLKRGM_USB);
}

/*==========================================================================

  FUNCTION      CLK_REGIME_DCVS_LOG

  DESCRIPTION   This function generates a clock log record according to the
                specified clock number and transaction.

  PARAMETERS    clk_num     - Clock number
                transaction - Transaction (enable/disable)

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  A clock log record will be generated.

==========================================================================*/

void clk_regime_dcvs_log (
  clkrgm_dcvs_clk_log_type reg_dreg,
  uint8 transaction)
{
  #ifndef BUILD_BOOT_CHAIN

  clkrgm_dcvs_log_type *log = NULL;

  if (log_status(LOG_CLOCK_C))
  {
    log = (clkrgm_dcvs_log_type *)log_alloc(LOG_CLOCK_C,
                                           sizeof(clkrgm_dcvs_log_type));
    if (log == NULL)
    {
      ERR("clk_regime_msm_log: Log packet allocation failure", 0, 0, 0);
      return;
    }
    log->hw_rev_num = HWIO_IN(HW_REVISION_NUMBER);
    log->clock_domain = CLKRGM_DCVS_LOG_DOMAIN;
    log->transaction_type = transaction;
    log->clock_num = reg_dreg;
    log->status_reg_count = CLKRGM_DCVS_LOG_STATUS_REG_COUNT;

    log->status_reg[0] = (uint8)clkrgm.cpu_speed;
    log->status_reg[1] = (uint8)clkrgm.adsp_speed;
    log->status_reg[2] = (uint8)clkrgm_modem.mdsp_speed;
    log->status_reg[3] = clkrgm.cpu_client_mask>>32; //high order uint64 part
    log->status_reg[4] = clkrgm.cpu_client_mask&0x00000000ffffffff; //low order uint64 part

    #ifdef FEATURE_CLKREGIM_DYNAMIC_VOLTAGE
    log->status_reg[5] = (uint16)clkrgm.cpu_mv;
    #endif
    log_commit(log);
  }

  #endif
} /* clk_regime_msm_log */

/*==========================================================================

  FUNCTION      CLK_REGIME_MSM_LOG

  DESCRIPTION   This function generates a clock log record according to the
                specified clock number and transaction.

  PARAMETERS    clk_num     - Clock number
                transaction - Transaction (enable/disable)

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  A clock log record will be generated.

==========================================================================*/

void clk_regime_msm_log (
  clkrgm_msm_clk_log_type clk_num,
  uint8 transaction)
{
  #ifndef BUILD_BOOT_CHAIN

  clkrgm_msm_log_type *log = NULL;

  if (log_status(LOG_CLOCK_C))
  {
    log = (clkrgm_msm_log_type *)log_alloc(LOG_CLOCK_C,
                                           sizeof(clkrgm_msm_log_type));
    if (log == NULL)
    {
      ERR("clk_regime_msm_log: Log packet allocation failure", 0, 0, 0);
      return;
    }
    log->hw_rev_num = HWIO_IN(HW_REVISION_NUMBER);
    log->clock_domain = CLKRGM_MSM_LOG_DOMAIN;
    log->transaction_type = transaction;
    log->clock_num = clk_num;
    log->status_reg_count = CLKRGM_MSM_LOG_STATUS_REG_COUNT;
    log->status_reg[0] = HWIO_IN(CLK_OFF_A);
    log->status_reg[1] = HWIO_IN(CLK_OFF_B);
    log_commit(log);
  }

  #endif
} /* clk_regime_msm_log */
