/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    C L O C K    R E G I M E    D E V I C E   D R I V E R


GENERAL DESCRIPTION

  This file contains functions needed by boot, and normal execution.  Areas of difference are featurized with BUILD_BOOT_CHAIN.  The boot loader versions of the functions are linked into separate executables from the primary phone executable, so the changes are isolated to only boot.

EXTERNALIZED FUNCTIONS

clk_regime_init_for_dload
clk_regime_init_for_dload_thru_uart
clk_regime_init_for_dload_thru_usb
clk_regime_msm_enable
clk_regime_msm_on
clk_regime_set_dload_mclk

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_boot.c#8 $
$DateTime: 2009/04/02 01:06:49 $
$Author: bamidir $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/09    br     Added support for EFUSE Clock regime.
12/01/08    br     Added clk_regime_msm_is_on with limited functionality to remove CLKREGIME
                       dependencies on Boot omsbl compilation for the Dead Battery charging faeture.
04/18/08    bc     Removed GSBI0 MND source management until an API for UIM
                   can be established.
04/18/08    bc     Added more complete state initialization for boot loader
                   download mode.
03/31/08    bc     Moved functions into this file to isolate boot loader
                   dependencies.
===========================================================================*/
#include "clk.h"
#include "msm.h"
#include "clkrgmi_msm.h"

#if defined(BUILD_BOOT_CHAIN)

  /* remove logging for boot loader */
  #define clk_regime_msm_log(a,b)

#else

#ifdef FEATURE_CLKREGIM_RM
  #define RM_MSM_CLK_INFO(clk) (&clkrgm.rm_clk_info[(int)clk])
#endif

extern void clk_regime_msm_log
(
  clkrgm_msm_clk_log_type clk_num,
  uint8 transaction
);

extern void clk_regime_msm_src_request
(
  clkrgm_msm_clk_type     clk,
  clkrgm_pll_op_type      request
);

extern   clkrgm_msm_state_type clkrgm;

/* --------------------------------------------------------------------------
   Audio clock regime configuration data structures.
-------------------------------------------------------------------------- */

  /* Internal CODEC clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_icodec_cfg [ ];

  /* External CODEC clock regime configuration structure. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_ecodec_cfg[ ];

  /* Stereo DAC clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_sdac_cfg [ ];

/* --------------------------------------------------------------------------
   Peripheral clock regime configuration data structures.
-------------------------------------------------------------------------- */

  /* External camera (GPIO 13) clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_camclk_cfg[] ;

  /* SDCC clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_sdcc_cfg [ ] ;

  /* UART_DM clock regime configuration structure array. */

  extern const clkrgm_msm_type1_cfg_type clkrgm_uart_dm_cfg [ ] ;

  extern const clkrgm_msm_type1_cfg_type clkrgm_gsbi0_uart_cfg [ ] ;
  extern const clkrgm_msm_type1_cfg_type clkrgm_gsbi_1_2_uart_cfg [ ] ;
#endif


/*===========================================================================

FUNCTION CLK_REGIME_MSM_ON

DESCRIPTION
  This function will enable the specified clock regimes. No attempt to
  determine if the source clock is disabled is made.

PARAMETERS
  regime - clock regime to be enabled.

DEPENDENCIES
  Successful enabling depends on the source clock being oscillating and
  stable.

RETURN VALUE
  None.

SIDE EFFECTS
  Selected clock regime circuit is enabled.

===========================================================================*/
void clk_regime_msm_on
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
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, XO4_CLK_EN, ENABLED );
      clk_busy_wait( CLKRGM_XO4_DISABLE_TIME_US );
      clk_regime_msm_log(CLKRGM_MSM_LOG_TCXO4_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_UART0_M:
    #if defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI0_MN_REG, CLKRGM_GSBI0_UART_7P3728_MHZ_MN_REG_VAL);
      HWIO_OUT( GSBI0_CD_REG, CLKRGM_GSBI0_UART_7P3728_MHZ_CD_REG_VAL);
    #else
      // HWIO_OUT( GSBI0_CD_REG, clkrgm.gsbi[0].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE1,
                       ENABLE_UART_DM_CLOCK );
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_SIM_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;


    case CLK_RGM_UART1_M:
    #if defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI1_MN_REG, CLKRGM_GSBI_1_2_UART_7P3846_MHZ_MN_REG_VAL );
      HWIO_OUT( GSBI1_CD_REG, CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CD_REG_VAL );
    #else
      HWIO_OUT( GSBI1_CD_REG, clkrgm.gsbi[1].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE1,
                       ENABLE_UART_DM_CLOCK );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_UART2_M:
    #if defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI2_MN_REG, CLKRGM_GSBI_1_2_UART_7P3846_MHZ_MN_REG_VAL );
      HWIO_OUT( GSBI2_CD_REG, CLKRGM_GSBI_1_2_UART_7P3846_MHZ_CD_REG_VAL );
    #else
      HWIO_OUT( GSBI2_CD_REG, clkrgm.gsbi[2].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE1,
                       ENABLE_UART_DM_CLOCK );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_UART_LEGACY_M:
    #if defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( UART_DM_MN_REG, CLKRGM_UART_DM_7P3728_MHZ_MN_REG_VAL );
      HWIO_OUT( UART_DM_CD_REG, CLKRGM_UART_DM_7P3728_MHZ_CD_REG_VAL );
    #else
      HWIO_OUT( UART_DM_CD_REG, clkrgm.uart_legacy.cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( UART_DM_CD_REG, UART_DM_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_UART_DM_PAUSE);

      clk_regime_msm_log( CLKRGM_MSM_LOG_UART1_DM_CLK,
                          CLKRGM_MSM_LOG_TRANSACTION_DISABLE );
      break;

    case CLK_RGM_I2C0_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI0_CD_REG, clkrgm.gsbi[0].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE2,
                       ENABLE_I2CC_CLOCK );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_I2C1_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI1_CD_REG, clkrgm.gsbi[1].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE2,
                       ENABLE_I2CC_CLOCK );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_I2C2_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI2_CD_REG, clkrgm.gsbi[2].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE2,
                       ENABLE_I2CC_CLOCK );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;


    case CLK_RGM_I2C_LEGACY_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, I2C_SRC_SEL, XO );
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, I2C_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_I2C_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_I2C_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

	  case CLK_RGM_RFMS_XO_CLK_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, RFMS_XO_SRC_SEL, XO );
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, RFMS_XO_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_XO_DISABLE_TIME_US);

      clk_regime_msm_log(CLKRGM_MSM_LOG_RFMS_XO_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;
    case CLK_RGM_SPI0_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI0_CD_REG, clkrgm.gsbi[0].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE3,
                       ENABLE_SPI_CLOCK );
      clk_busy_wait(CLKRGM_SPI_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_SPI1_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI1_CD_REG, clkrgm.gsbi[1].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE3,
                       ENABLE_SPI_CLOCK );
      clk_busy_wait(CLKRGM_SPI_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_SPI2_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI2_CD_REG, clkrgm.gsbi[2].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE3,
                       ENABLE_SPI_CLOCK );
      clk_busy_wait(CLKRGM_SPI_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_GSBI0_M:
    #if !defined(BUILD_BOOT_CHAIN)
      // HWIO_OUT( GSBI0_CD_REG, clkrgm.gsbi[0].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI0_CD_REG, GSBI0_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_GSBI_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI0_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_GSBI1_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI1_CD_REG, clkrgm.gsbi[1].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI1_CD_REG, GSBI1_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_GSBI_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI1_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_GSBI2_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( GSBI2_CD_REG, clkrgm.gsbi[2].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( GSBI2_CD_REG, GSBI2_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_GSBI_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_GSBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_ECODEC_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( ECODEC_CD_REG, clkrgm_ecodec_cfg[ clkrgm.ecodec_speed ].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODEC_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODECIF_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( ECODEC_CD_REG, ECODECIF_PO_CLK_EN, ENABLED );
      clk_busy_wait( CLKRGM_ECODEC_PAUSE );

      clk_regime_msm_log(CLKRGM_MSM_LOG_ECODECIF_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_ADSP_M:
      HWIO_OUTM( ADSP_CFG_REG,
                 HWIO_FMSK( ADSP_CFG_REG, ADSP_CLK_EN)
                 | HWIO_FMSK( ADSP_CFG_REG, ADSP_INTF_CLK_EN),
                 CR_FVAL( ADSP_CFG_REG, ADSP_CLK_EN, ENABLED )
                 | CR_FVAL( ADSP_CFG_REG, ADSP_INTF_CLK_EN, ENABLED )
               );

      clk_busy_wait( CLKRGM_ADSP_PAUSE );

      clk_regime_msm_log(CLKRGM_MSM_LOG_ADSP_HM_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

	  case CLK_RGM_ADSP_VIDEO_INTF_M:
    	 HWIO_OUTM( ADSP_CLK_CTL_REG,
                 HWIO_FMSK( ADSP_CLK_CTL_REG, ADSP_VIDEO_INTF_CLK_EN),    
                 CR_FVAL( ADSP_CLK_CTL_REG, ADSP_VIDEO_INTF_CLK_EN, ENABLED )
                
               );

      clk_busy_wait( CLKRGM_ADSP_PAUSE );

      clk_regime_msm_log(CLKRGM_MSM_LOG_ADSP_HM_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
	  break;

    case CLK_RGM_SBI_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED );
      clk_busy_wait( CLKRGM_XO_DISABLE_TIME_US );
      clk_regime_msm_log(CLKRGM_MSM_LOG_SSBI_PMIC_CLK,
                         CLKRGM_MODEM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_ICODEC_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( ICODEC_RX_CD_REG, clkrgm_icodec_cfg[ clkrgm.icodec_speed ].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, CLK_SEL, ICODEC_RX );
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN, ENABLED);
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_EN, ENABLED);
      CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, ICODEC_TX_CLK_EN, ENABLED);

      clk_busy_wait(CLKRGM_CODEC_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_ICODEC_2X_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_ICODEC_RX_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( ICODEC_RX_CD_REG, clkrgm_icodec_cfg[ clkrgm.icodec_speed ].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN, ENABLED);
      CLKRGM_SET_FVAL_NL( ICODEC_RX_CD_REG, ICODEC_2X_RX_CLK_EN, ENABLED);
      clk_busy_wait(CLKRGM_CODEC_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_ICODEC_2X_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_ICODEC_TX_M:
      if( HWIO_INF(ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN) ==
          HWIO_FVAL(ICODEC_RX_CD_REG, ICODEC_RX_CLK_EN, NOT_ENABLED) )
      {
        /* Enable RX clk, because TX is derived from RX */
      #if !defined(BUILD_BOOT_CHAIN)
        HWIO_OUT( ICODEC_RX_CD_REG, clkrgm_icodec_cfg[ clkrgm.icodec_speed ].cd_reg );
      #endif
      }

      CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, CLK_SEL, ICODEC_RX );
      CLKRGM_SET_FVAL_NL( ICODEC_TX_CD_REG, ICODEC_TX_CLK_EN, ENABLED);
      clk_busy_wait(CLKRGM_CODEC_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_ICODEC_2X_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_USB_MCLK_M:
    #if defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( USB_MN_REG, CLKRGM_USB_60_MHZ_MN_REG_VAL );
      HWIO_OUT( USB_CD_REG, CLKRGM_USB_60_MHZ_CD_REG_VAL );
    #else
      HWIO_OUT( USB_CD_REG, clkrgm.usb.cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( USB_CD_REG, USB_FS_CLK_EN, ENABLED );
      clk_busy_wait(10);
      
      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_MCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_SDAC_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( I2S_CD_REG, clkrgm_sdac_cfg[ clkrgm.sdac_speed ].cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_BIT_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( I2S_CD_REG, I2S_MSTR_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_SDAC_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_I2S_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_ETM_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, ETM_CLK_EN, ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_ETM_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_CAMCLK_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( CAMCLK_CD_REG, clkrgm.camclk.cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( CAMCLK_CD_REG, CAMCLK_PO_EN, ENABLED );
      clk_busy_wait(CLKRGM_CAMCLK_PAUSE);

      clk_regime_msm_log(CLKRGM_MSM_LOG_CAMCLK_PO,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_CAMIF_PCLK_M:
      /* This is really a source selection, not a clock regime on/off */
      CLKRGM_SET_FVAL_NL( CAMCLK_CD_REG, CAM_PCLK_SRC_SEL, TLMM_CAM_PCLK );
      break;

    case CLK_RGM_VIDEO_VFE_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUTM( VIDEO_VFE_CD_REG,
                 HWIO_FMSK(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL),
                 clkrgm.video_vfe.src << HWIO_SHFT(VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_SRC_SEL) );
    #endif
      CLKRGM_SET_FVAL_NL( VIDEO_VFE_CD_REG, VIDEO_VFE_CLK_EN, ENABLED );
      clk_busy_wait(10);

      clk_regime_msm_log(CLKRGM_MSM_LOG_VIDEO_VFE_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_VIDEO_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, VIDEO_AHB_CLK_EN, ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_VIDEO_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_AUDIO_M:    /* Audio AHB clock                   */
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, AUDIO_AHB_CLK_EN, ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_AUDIO_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_USB_AHB_CLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, USB_AHB_CLK_EN, ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_OTG_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_SDCC_HCLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, SDCC_AHB_CLK_EN, ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_SDCC_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_SDCC_MCLK_M:
    #if !defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( SDCC_CD_REG, clkrgm.sdcc.cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( SDCC_CD_REG, SDCC_CLK_EN, ENABLED );
      clk_busy_wait(CLKRGM_SDCC_PAUSE);

      /* ground clock source in mn counter */
      clk_regime_msm_log(CLKRGM_MSM_LOG_SDCC_MCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_MDP_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, MDP_AHB_CLK_EN, ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_MDP_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_QMEMBIST_M:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, QMEMBIST_SRC_SEL, XO );
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, QMEMBIST_CLK_EN, ENABLED );

      clk_regime_msm_log(CLKRGM_MSM_LOG_QMEMBIST_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_USB_M:
    #if defined(BUILD_BOOT_CHAIN)
      HWIO_OUT( USB_MN_REG, CLKRGM_USB_60_MHZ_MN_REG_VAL );
      HWIO_OUT( USB_CD_REG, CLKRGM_USB_60_MHZ_CD_REG_VAL );
    #else
      HWIO_OUT( USB_CD_REG, clkrgm.usb.cd_reg );
    #endif
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, USB_AHB_CLK_EN, ENABLED );
      CLKRGM_SET_FVAL_NL( USB_CD_REG, USB_FS_CLK_EN, ENABLED );
      clk_busy_wait(10);


      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_MCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      clk_regime_msm_log(CLKRGM_MSM_LOG_USB_OTG_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_SEC_HCLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, SEC_AHB_CLK_EN, ENABLED );

      clk_regime_msm_log(CLKRGM_MSM_LOG_SEC_HCLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_EBI2_CLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, EBI2_CLK_SRC_EN, ENABLED );
      clk_regime_msm_log(CLKRGM_MSM_LOG_EBI2_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      /* EBI2_IO is branched from EBI2_CLK */
      clk_regime_msm_log(CLKRGM_MSM_LOG_EBI2_IO_CLK,
                         CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_IMEM_CLK_M:
      CLKRGM_SET_FVAL_NL( ARM_CLK_EN_REG, IMEM_CLK_SRC_EN, ENABLED );
      clk_regime_msm_log( CLKRGM_MSM_LOG_IMEM_CLK,
                          CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
      break;

    case CLK_RGM_EFUSE_CLK:
      CLKRGM_SET_FVAL_NL( MISC_CFG_REG, EFUSE_CLK_EN, ENABLED );
      clk_regime_msm_log( CLKRGM_MSM_LOG_EFUSE_CLK,
                          CLKRGM_MSM_LOG_TRANSACTION_ENABLE);
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

FUNCTION CLK_REGIME_MSM_ENABLE

DESCRIPTION
  This function will enable the specified clock regimes. No attempt to
  determine if the source clock is disabled is made.

PARAMETERS
  regime - clock regime to be enabled.

DEPENDENCIES
  Successful enabling depends on the source clock being oscillating and
  stable.

RETURN VALUE
  None.

SIDE EFFECTS
  Selected clock regime circuit is enabled.

===========================================================================*/
void clk_regime_msm_enable (clkrgm_msm_clk_type clk)
{
  uint32 isav;

  /* ------------------------------------------------------------------------
     Filter out group-clocks for recursive handling.
  ------------------------------------------------------------------------ */

  switch (clk)
  {
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

  if (!clk_regime_rm_clk_enable(RM_MSM_CLK_INFO(clk)))
  {
    /* At this time, always return true, this is never reached */
    INTFREE_SAV(isav);
    return;
  }
  #endif

  /* ------------------------------------------------------------------------
     Configure any required source
  ------------------------------------------------------------------------ */

  #if !defined(BUILD_BOOT_CHAIN)  /* Assume PLL on during boot */
  clk_regime_msm_src_request(clk, CLKRGM_PLL_ENABLE);
  #endif

  /* ------------------------------------------------------------------------
     Enable the clock
  ------------------------------------------------------------------------ */

  clk_regime_msm_on (clk);

  /* ------------------------------------------------------------------------
     Free interrupts
  ------------------------------------------------------------------------ */

  INTFREE_SAV(isav);

} /* clk_regime_msm_enable */

/*===========================================================================

  FUNCTION CLK_REGIME_INIT_FOR_DLOAD_THRU_UART

  DESCRIPTION
    This is the MSM clock regime initialization function prior
    to invoking dload.

  PARAMETERS
    None.

  DEPENDENCIES
    TCXO and SLEEP xtal clock sources need to be stable.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The PLL and clock regime hardware is initialized.

===========================================================================*/
void clk_regime_init_for_dload_thru_uart( void )
{
  /* Set the SBI clock source to be the TCXO clock. */
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_SRC_SEL, XO);
  CLKRGM_SET_FVAL( MODEM_CLK_MISC_CTL, SBI_CLK_SRC_SEL, TCXO );

  /* Enable SSBI */
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED );
  HWIO_OUTF( MODEM_CLK_HALT, MODEM_SBI_CLK_HALT, 0 );

  /* First disable the UART clk regimes */
  #ifdef CLKRGM_GSBI0_IS_UART
    CLKRGM_SET_FVAL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE1,
                     DISABLE_UART_DM_CLOCK );
    CLKRGM_SET_FVAL( GSBI0_CD_REG, GSBI0_CLK_EN, NOT_ENABLED );
  #endif
  #ifdef CLKRGM_GSBI1_IS_UART
    CLKRGM_SET_FVAL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE1,
                     DISABLE_UART_DM_CLOCK );
    CLKRGM_SET_FVAL( GSBI1_CD_REG, GSBI1_CLK_EN, NOT_ENABLED );
  #endif
  #ifdef CLKRGM_GSBI2_IS_UART
    CLKRGM_SET_FVAL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE1,
                     DISABLE_UART_DM_CLOCK );
    CLKRGM_SET_FVAL( GSBI2_CD_REG, GSBI2_CLK_EN, NOT_ENABLED );
  #endif
  CLKRGM_SET_FVAL( UART_DM_CD_REG, UART_DM_CLK_EN, NOT_ENABLED );
 
  /* Allow time for the clocks to stop */
  clk_busy_wait(CLKRGM_UART_DM_PAUSE);

  /* Set the UART clock sources to either the TCXO or TCXO/4 clock. */

  #ifdef FEATURE_UART_TCXO_CLK_FREQ

    /* UART clock TCXO selection. */
    #ifdef CLKRGM_GSBI0_IS_UART
      CLKRGM_CD_REG_SET_MND( GSBI0_CD_REG, XO, BYPASS, NOT_ENABLED );
    #endif
    #ifdef CLKRGM_GSBI1_IS_UART
      CLKRGM_CD_REG_SET_MND( GSBI1_CD_REG, XO, BYPASS, NOT_ENABLED );
    #endif
    #ifdef CLKRGM_GSBI2_IS_UART
      CLKRGM_CD_REG_SET_MND( GSBI2_CD_REG, XO, BYPASS, NOT_ENABLED );
    #endif
    CLKRGM_CD_REG_SET_MND( UART_DM_CD_REG, XO, BYPASS, NOT_ENABLED );

  #else
    #error "CLKREGIM: only TCXO available to UART"
  #endif

  /* Now enable the UART clk regime*/
  #ifdef CLKRGM_GSBI0_IS_UART
    CLKRGM_SET_FVAL( GSBI0_GSBI_CTRL_REG, CLOCK_ENABLE1,
                     ENABLE_UART_DM_CLOCK );
    CLKRGM_SET_FVAL( GSBI0_CD_REG, GSBI0_CLK_EN, ENABLED );
  #endif
  #ifdef CLKRGM_GSBI1_IS_UART
    CLKRGM_SET_FVAL( GSBI1_GSBI_CTRL_REG, CLOCK_ENABLE1,
                     ENABLE_UART_DM_CLOCK );
    CLKRGM_SET_FVAL( GSBI1_CD_REG, GSBI1_CLK_EN, ENABLED );
  #endif
  #ifdef CLKRGM_GSBI2_IS_UART
    CLKRGM_SET_FVAL( GSBI2_GSBI_CTRL_REG, CLOCK_ENABLE1,
                     ENABLE_UART_DM_CLOCK );
    CLKRGM_SET_FVAL( GSBI2_CD_REG, GSBI2_CLK_EN, ENABLED );
  #endif
  CLKRGM_SET_FVAL( UART_DM_CD_REG, UART_DM_CLK_EN, ENABLED );

}

/*===========================================================================

  FUNCTION CLK_REGIME_INIT_FOR_DLOAD_THRU_USB

  DESCRIPTION
    This is the MSM clock regime initialization function prior
    to invoking dload.

  PARAMETERS
    None.

  DEPENDENCIES
    TCXO and SLEEP xtal clock sources need to be stable.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The PLL and clock regime hardware is initialized.

===========================================================================*/
void clk_regime_init_for_dload_thru_usb( void )
{

  /* Set the SBI clock source to be the TCXO clock. */
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_SRC_SEL, XO);
  CLKRGM_SET_FVAL( MODEM_CLK_MISC_CTL, SBI_CLK_SRC_SEL, TCXO );

  /* Enable SSBI */
  CLKRGM_SET_FVAL( MISC_CFG_REG, SSBI_PMIC_CLK_EN, ENABLED );
  HWIO_OUTF( MODEM_CLK_HALT, MODEM_SBI_CLK_HALT, 0 );

  /* Disable USB clock during MND config */
  CLKRGM_SET_FVAL( ARM_CLK_EN_REG, USB_AHB_CLK_EN, NOT_ENABLED );
  CLKRGM_SET_FVAL( USB_CD_REG, USB_FS_CLK_EN, NOT_ENABLED );
  clk_busy_wait(10);

  /* USB clock is 60 MHz */
  HWIO_OUT( USB_MN_REG, CLKRGM_USB_60_MHZ_MN_REG_VAL );
  HWIO_OUT( USB_CD_REG, CLKRGM_USB_60_MHZ_CD_REG_VAL );

  /* ------------------------------------------------------------------------
     Enable the USB AHB clock
  ------------------------------------------------------------------------ */

  CLKRGM_SET_FVAL( ARM_CLK_EN_REG, USB_AHB_CLK_EN, ENABLED );
  CLKRGM_SET_FVAL( USB_CD_REG, USB_FS_CLK_EN, ENABLED );
}

/*===========================================================================

  FUNCTION CLK_REGIME_INIT_FOR_DLOAD

  DESCRIPTION
    This is the MSM clock regime initialization function prior
    to invoking dload.

  PARAMETERS
    None.

  DEPENDENCIES
    TCXO and SLEEP xtal clock sources need to be stable.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The PLL and clock regime hardware is initialized.

===========================================================================*/
void clk_regime_init_for_dload( void )
{

  /* Remove reset signal from all blocks */
  HWIO_OUT( MODEM_CLK_RESET, 0 );
  HWIO_OUT( MODEM_CLK_RESET_2, 0 );
  HWIO_OUT( SW_RESET_REG, 0 );
  clk_busy_wait(100);

  /* Set up the UARTS */
  clk_regime_init_for_dload_thru_uart();
  /* Set up USB */
  clk_regime_init_for_dload_thru_usb();
}

/*===========================================================================

  FUNCTION CLK_REGIME_SET_DLOAD_MCLK

  DESCRIPTION
    Configure the ARM clock to run at the designed ARM download frequency

  DEPENDENCIES
    None.

  RETURN VALUE
    None
===========================================================================*/
void clk_regime_set_dload_mclk( void )
{
  /* Not supported */
}

#ifdef BUILD_BOOT_CHAIN
/*===========================================================================

  FUNCTION CLK_REGIME_MSM_IS_ON

  DESCRIPTION
   

  DEPENDENCIES
    Adding this function for BOOT compilation
   for Dead Battery Charging support on ULC.

  RETURN VALUE
    TRUE if the particular clock regime is on.
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
    case CLK_RGM_EFUSE_CLK:
      if ( CLKRGM_GET( CLK_OFF_B, EFUSE_CLK) == 0  ) is_on = TRUE;
      break; 
    default:
      break;
  }

  return is_on;
}
#endif