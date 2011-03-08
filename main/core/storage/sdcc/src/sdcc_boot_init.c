/***********************************************************************
 * sdcc_boot_init.c
 *
 * SDCC1(Secure Digital Card Controller1) Basic Init at Boot Code level 
 *
 * This file implements the necessary GPIO, PMIC, CLK and 
 * basic SDCC1 controller config
 *
 * Copyright (c) 2009 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================
                        Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_boot_init.c#2 $
$DateTime: 2009/04/22 22:40:56 $ $Author: spuliven $

when         who     what, where, why
----------   ---     -----------------------------------------------------
2009-04-21   sp      Add support to eliminate sdcc_boot_init for low-tier
2009-02-11   sp      Initial revision
======================================================================*/


/*======================================================================

                            INCLUDE FILES FOR MODULE

======================================================================*/

#include "sdcc_boot_init.h"

#include "msm.h"
#include "hw.h"
#if defined(T_MSM6275) || defined(T_MSM6280)
#error code not present
#else
#include "tlmm.h"
#endif
#include GPIO_H
#include "pm.h"
#include "clkregim.h"

/*======================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

======================================================================*/

/* MMC VREG Voltage level set to 3.05v */
#define   SDCC_PM_VREG_LEVEL      3050

/* Set MCI Power to ON */
#define   MCI_POWER_CTRL_ON       3
/* MCI Clock Enable */
#define   MCI_CLK_ENABLED         (1UL << HWIO_MCI_CLK_ENABLE_SHFT)
/* MCI Clock in PowerSave mode */
#define   MCI_CLK_SET_PWRSAVE     (1UL << HWIO_MCI_CLK_PWRSAVE_SHFT)

#ifdef FEATURE_DUAL_CLK_RGM
#define CLK_REGIME_ENABLE   clk_regime_msm_enable
#else
#define CLK_REGIME_ENABLE   clk_regime_enable
#endif

#if defined(T_QSC1100) || defined(T_QSC1110)
/* Default SDCC MCLK Src is EXT_CLK and hence setting it to TCXO */
#define SDCC_MCLK_SRC_TCXO   (1UL<<HWIO_SDCC_CD_REG_SRC_SEL_SHFT)
#define CLK_REGIME_SDCC_MCLK_SRC_SEL()               \
                     HWIO_OUTM(SDCC_CD_REG,          \
                     HWIO_FMSK(SDCC_CD_REG, SRC_SEL),\
                     SDCC_MCLK_SRC_TCXO);
#else
#define CLK_REGIME_SDCC_MCLK_SRC_SEL()
#endif

/*======================================================================

                              FUNCTION DEFINITIONS

======================================================================*/

/***********************************************************************
* Name: sdcc_boot_gpio_init.
*
* Description:
  This function performs SDCC1 related GPIO initialization.

* Arguments:
*       void
*
* Returns:
*       void
*
***********************************************************************/
void 
sdcc_boot_gpio_init( void )
{  
#ifdef FEATURE_6275_COMPILE
   gpio_config(SDCC_CLK);
   gpio_config(SDCC_CMD);
   gpio_config(SDCC_DAT_0);
   gpio_config(SDCC_DATOUT_1);
   gpio_config(SDCC_DATOUT_2);
   gpio_config(SDCC_DATOUT_3);
#else
   tlmm_init();
   gpio_tlmm_config(SDCC_CLK);
   gpio_tlmm_config(SDCC_CMD);
   gpio_tlmm_config(SDCC_DAT_0);
   gpio_tlmm_config(SDCC_DATOUT_1);
   gpio_tlmm_config(SDCC_DATOUT_2);
   gpio_tlmm_config(SDCC_DATOUT_3);
#endif
}

/***********************************************************************
* Name: sdcc_boot_pmic_init.
*
* Description:
  This function performs SDCC1 related PMIC initialization.

* Arguments:
*       void
*
* Returns:
*       void
*
***********************************************************************/
void 
sdcc_boot_pmic_init( void )
{
#if defined(T_QSC1100) || defined(T_QSC1110)
   unsigned long  qsc11x0_pmic_hw_version = pm_get_pmic_revision();

   /* ULC uses LCD_DRV_N to gate power to SDCC slot on Kip B0 */
   if ( qsc11x0_pmic_hw_version >= 3)
   {
      /* Enable power to SDCC */
      pm_set_led_intensity(PM_LCD_LED, 1);
   }
   /* on Kip A0 and A1, PM_MPP4 is used */
   else
   {
      pm_mpp_config_digital_output(PM_MPP_4,
                                   PM_MPP__DLOGIC__LVL_MSMP,
                                   PM_MPP__DLOGIC_OUT__CTRL_LOW);
   }
#else /* !defined(T_QSC1100) || !defined(T_QSC1110) */
#ifdef FEATURE_WLAN_MODIFIED_FFA
#error code not present
#else
   pm_vreg_set_level(PM_VREG_MMC_ID, SDCC_PM_VREG_LEVEL);
#endif /* FEATURE_WLAN_MODIFIED_FFA */

#if defined FEATURE_SDCC_TURN_ON_PWR_SC2X
   pm_mpp_config_digital_output(PM_MPP_4,
                                PM_MPP__DLOGIC__LVL_MSMP,
                                PM_MPP__DLOGIC_OUT__CTRL_LOW);
#endif /* FEATURE_SDCC_TURN_ON_PWR_SC2X */

#if defined(T_QSC60X5)
   pm_vreg_control(PM_ON_CMD, PM_VREG_MMC_M);
#else
#ifdef FEATURE_WLAN_MODIFIED_FFA
#error code not present
#else
   pm_vote_vreg_switch(PM_ON_CMD,
                       PM_VREG_MMC_ID,
                       PM_VOTE_VREG_MMC_APP__MINI_SD);
#endif /* FEATURE_WLAN_MODIFIED_FFA */
#endif /* defined(T_QSC60X5)  */
#endif /* defined(T_QSC1100) || defined(T_QSC1110) */
}

/***********************************************************************
* Name: sdcc_boot_clk_init.
*
* Description:
  This function performs SDCC1 related CLK initialization.

* Arguments:
*       void
*
* Returns:
*       void
*
***********************************************************************/
void 
sdcc_boot_clk_init( void )
{
   /* Enabled H-CLK for SDCC */
   CLK_REGIME_ENABLE(CLK_RGM_SDCC_HCLK_M);
   /* Enabled M-CLK for SDCC */
   CLK_REGIME_ENABLE(CLK_RGM_SDCC_MCLK_M);
   /* Set SDCC MCLK Src selection as TCXO - ULC only since EXT_CLK is default */
   CLK_REGIME_SDCC_MCLK_SRC_SEL();
}

/***********************************************************************
* Name: sdcc_boot_init.
*
* Description:
  This function performs SDCC1 related initialization.

* Arguments:
*       void
*
* Returns:
*       void
*
***********************************************************************/
void 
sdcc_boot_init(void)
{
/* Certain Low-Tier builds do not support SDCC and hence compiling
   out the SDCC_BOOT_INIT for such builds */
#if defined(FEATURE_BUILD_MMC)
#if defined(T_QSC1100) || defined(T_QSC1110)
#else
   sdcc_boot_pmic_init();
#endif
   sdcc_boot_gpio_init();
   sdcc_boot_clk_init();

   /* set the power mode to 'power on' */
   HWIO_OUTM(MCI_POWER,
             HWIO_FMSK(MCI_POWER, CONTROL),
             MCI_POWER_CTRL_ON);
   /* enable the clock for the SD mode*/
   HWIO_OUTM(MCI_CLK,
             HWIO_FMSK(MCI_CLK, ENABLE)| HWIO_FMSK(MCI_CLK, PWRSAVE),
             MCI_CLK_ENABLED | MCI_CLK_SET_PWRSAVE);
#endif /* FEATURE_BUILD_MMC */
}
