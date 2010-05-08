/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B O O T    H A R D W A R E    I N I T I A L I Z A T I O N

GENERAL DESCRIPTION
  This module performs MSM  specific hardware initialization.

EXTERNALIZED FUNCTIONS
  boot_hw_target_init()
    Initialize MSM specific hardware.

Copyright (c) 2004 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boothw_target.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/07   ps      Adding support for SECBOOT 2.0
05/11/07   cr      Add HW REV 1.X and MPM conditional compilation on SLEEP_XTAL_EN.
02/02/07    cr     Integrate CLKREGIM_DEVMAN.
05/09/05   rp      Created target specific file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"
#include "comdef.h"
#include BOOTHW_MSM_H
#include BOOTHWI_MSM_H
#include GPIO_H
#include "bio.h"

#ifdef BUILD_BOOT_CHAIN
 #include "custosbl.h"
 #include "pm.h"
#endif 
#ifndef FEATURE_PMIC_VREG_INIT_BYPASS  
  #ifdef T_MSM6800B
    #define MSMC_NOMINAL_VOLTAGE_IN_MV    1200
  #else
    #define MSMC_NOMINAL_VOLTAGE_IN_MV    1375
  #endif
#endif /* FEATURE_PMIC_VREG_INIT_BYPASS */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* --------------------------------------------------------------------------
   External symbol declarations.
-------------------------------------------------------------------------- */
extern void clk_regime_switch_mclk_max (void);
extern uint32 boot_pm_init(void);
extern uint32 boot_sbi_init(void);
#if defined(FEATURE_PMIC_WDOG) && defined(FEATURE_SHARED_SEGMENT)
extern void boot_pm_save_power_on_status(void);
#endif
/* --------------------------------------------------------------------------
   Local variable definitions.
-------------------------------------------------------------------------- */
/*===========================================================================

  FUNCTION boot_hw_ctrl_init()

  DESCRIPTION
    Initialize MSM external bus interface (EBI) registers to enable access
    to the target memory and display devices.

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The above hardware circuits are initialized.

===========================================================================*/
void boot_hw_ctrl_init()
{
  /* ------------------------------------------------------------------------
     Reset the hardware watchdog circuit.
  ------------------------------------------------------------------------ */

  BOOTHW_KICK_WATCHDOG();

 #ifdef BUILD_BOOT_CHAIN

  /* Perform target specific initialization */
  gpio_init();

  /* Init SBI */
  boot_sbi_init();

  /* Target needs MPM configuration set to 0 before SSBI1 will communicate to EPIC */
  HWIO_OUTM( MPM_PMIC_SBI_SEL, 0xF, 0);

  /* Perform PMIC initialization */
  boot_pm_init();

#if defined(FEATURE_PMIC_WDOG) && defined(FEATURE_SHARED_SEGMENT)
  boot_pm_save_power_on_status();
#endif

#ifndef FEATURE_PMIC_VREG_INIT_BYPASS  
  /* Set the MSM core voltage to the normal value */
  if(pm_vreg_set_level(PM_VREG_MSMC1_ID, MSMC_NOMINAL_VOLTAGE_IN_MV) != PM_ERR_FLAG__SUCCESS)
  {
    ERR_FATAL("Unable to set MSM core voltage",0,0,0);
  }
  /* Unlock interrupts to allow preemption */
  clk_busy_wait(CLKRGM_VOLTAGE_SWITCHING_DELAY_US);
#endif
  /* Set the ARM speed to the maximum recommended value */
  clk_regime_switch_mclk_max();

#endif /* BUILD_BOOT_CHAIN */
  return;
} /* boot_hw_ctrl_init */


/*===========================================================================

  FUNCTION boot_hw_tlmm_init()

  DESCRIPTION
    Initializes GPIO

  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The MSM chip is initialized.
===========================================================================*/
uint32 boot_hw_tlmm_init(void)
{

  BOOTHW_KICK_WATCHDOG();

  gpio_init();


#ifdef TIMETEST
  /* Clear timetest port */
  outpw ( TIMETEST_PORT, 0x0000 );
#endif

  /* Disable IRQ interrupts  */
  HWIO_OUT( IRQ_EN_0, 0x00000000 );
  HWIO_OUT( IRQ_EN_1, 0x00000000 );

  /* Clear interrupts    */
  HWIO_OUT( IRQ_CLEAR_0, HWIO_RMSK( IRQ_CLEAR_0 ) );
  HWIO_OUT( IRQ_CLEAR_1, HWIO_RMSK( IRQ_CLEAR_1 ) );

  /* Disable FIQ interrupts  */
  HWIO_OUT( FIQ_EN_0, 0x00000000 );
  HWIO_OUT( FIQ_EN_1, 0x00000000 );

  /* Clear interrupts    */
  HWIO_OUT( FIQ_CLEAR_0, HWIO_RMSK( FIQ_CLEAR_0 ) );
  HWIO_OUT( FIQ_CLEAR_1, HWIO_RMSK( FIQ_CLEAR_1 ) );

  /* Disable GPIO GROUP interrupts */
  HWIO_OUT( MSM_GPIO1_INT_EN_0, 0x00000000 );
  HWIO_OUT( MSM_GPIO1_INT_EN_1, 0x00000000 );
  HWIO_OUT( MSM_GPIO2_INT_EN, 0x00000000 );

  /* Clear GPIO GROUP interrupts */
  HWIO_OUT( MSM_GPIO1_INT_CLEAR_0, HWIO_RMSK( MSM_GPIO1_INT_CLEAR_0 ) );
  HWIO_OUT( MSM_GPIO1_INT_CLEAR_1, HWIO_RMSK( MSM_GPIO1_INT_CLEAR_1 ) );
  HWIO_OUT( MSM_GPIO2_INT_CLEAR, HWIO_RMSK( MSM_GPIO2_INT_CLEAR ) );

/* --------------------------------------------------------------------------
   Initialize GPIO PAD strengths
-------------------------------------------------------------------------- */
#if 0 /* Would revisit it later */
  HWIO_OUT(GPIO_PAD_HDRIVE_MSEL_1, 0x00000);
  HWIO_OUT(GPIO_PAD_HDRIVE_MSEL_2, 0x02110000);
  HWIO_OUT(GPIO_PAD_HDRIVE_MSEL_3, 0x0011000);
#endif 
  return 0;

}

