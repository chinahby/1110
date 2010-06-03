/*===========================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                          H W   I n i t  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the HW 
  initialization.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/qsc1100/romboot/sbl/fsbl/fsbl_hw.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/09  tnk      Added support for USB detection through Vchg for USB chg
07/30/07   ANB     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "fsbl.h"
#include "fsbl_hw.h"
#include "boot_msm.h"
#include "fsbl_error_handler.h"

#ifdef FSBL_ENABLED_DLOAD
#include "fsbl_config.h"
#include "clk.h"
#include "clkrgm_msm.h"
#include "pm.h"
#include "pm_vreg_target.h"
#include GPIO_H
#include "bio.h"
#ifdef USE_DM
#include "dmov.h"
#endif /* USE_DM */

#ifdef TIMETEST
#include "timetest.h"
#endif
#endif /*FSBL_ENABLED_DLOAD*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FSBL_ENABLED_DLOAD
/* SBI 9.6 MHz this is defined in the library */
#define SBI_9p6MHZ                 2

/* SBI initialization from the SBI library */
extern void sbi_init( uint32 freq );

#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FSBL_ENABLED_DLOAD

/*=============================================================================
FUNCTION      FSBL_HW_CLK_BUSY_WAIT

DESCRIPTION   
  This function stalls the PAUSE timer. Conversion from microseconds to 
  pause time is done in the function.

DEPENDENCIES  
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
  
=============================================================================*/
void fsbl_hw_clk_busy_wait(uint32 micro_sec)
{
  uint32 wait_remaining_cycle =0;/* how many cycles we have to wait*/  
  uint32 pause_cycle=0;/* max is 65535. unit is clock cycle*/
  const boot_clk_speed_type *clk_speed;

  /* Get the current clock speed */
  clk_speed = fsbl_config_get_clock_speed();
  FSBL_VERIFY( clk_speed != NULL && clk_speed->ahb > 0, FSBL_ERR_NULL_PTR );
   
  /* How many cycles we should stall? */
  wait_remaining_cycle = clk_speed->ahb * micro_sec;
 
  /* Each penalty of write is 12 cycles */    
  while (wait_remaining_cycle >= 13)
  {
    /* Max of PAUSE TIME stall time is 65535 cycles */
    pause_cycle = MIN(65535, (wait_remaining_cycle-13));
    HWIO_OUT (PAUSE_TIMER, pause_cycle);
    wait_remaining_cycle -= (pause_cycle + 13);
  }
}/* fsbl_hw_clk_busy_wait() */

/*===========================================================================

FUNCTION boot_pm_init( void )

DESCRIPTION
  Initializes SBI and PMIC in OSBL

SIDE EFFECTS
  PMIC, SBI are initialized.

===========================================================================*/
void boot_pm_init( void )
{
  /* Initialize the serial bus controllers */
  sbi_init( SBI_9p6MHZ );
  FSBL_VERIFY( pm_init_comm() == PM_ERR_FLAG__SUCCESS, FSBL_ERR_PM_INIT_FAIL );
  
  /* Init model specific parameters such as no of regs, mpps */
  pm_model_specific_init();

  /* Wait 102 microseconds for the rail to charge */
  clk_busy_wait(102);  
} /* boot_pm_init( void ) */

/*===========================================================================

FUNCTION pm_init_otg_usb( void )

DESCRIPTION
  Initialize OTG USB in OSBL
  
DEPENDENCIES
  PMIC must be initialized.
  
RETURN VALUE
  NONE

SIDE EFFECTS
  OTG USB is initialized through the PMIC library
  
===========================================================================*/
void pm_init_otg_usb( void )
{
  /* derive power for OTG xcvr from USB_VBUS provided by the PC */
  #ifdef FEATURE_PMIC_USB_DETECTION_USING_VCHG
  pm_vreg_control(PM_ON_CMD, PM_VREG_BOOST_M);

  pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST);
  #else
  pm_otg_pwr_sel( PM_OTG_PWR_SEL__VBUS);
  #endif

  /* turn on VREG_USB */
  pm_vreg_set_level( PM_VREG_USB_ID, PM_VREG_USB_LEVEL);

  pm_vreg_control( PM_ON_CMD, PM_VREG_USB3P3_M);

  /* activate the OTG xcvr */
  pm_otg_config_power_management( PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

  /* pull down D+/- low initially to start off from a USB disconnect state */
  pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
                             PM_OTG_DP_CONFIG_CMD__PULL_DOWN, 
                             PM_OTG_DM_CONFIG_CMD__PULL_DOWN, 
                             PM_OTG_ID_CONFIG_CMD__NONE);

  /* reset OTG transceiver's mode */
  pm_otg_reset_mode();

  /* config OTG xcvr for USB B-dev mode */
  pm_otg_config_usb_mode( PM_OTG_DEV__B, PM_OTG_USB_SPEED__FULL);

  /* pull D+ high to indicate to PC that the phone is ready for USB traffic */
  pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE,
                            PM_OTG_DP_CONFIG_CMD__PULL_UP,
                            PM_OTG_DM_CONFIG_CMD__NONE, 
                            PM_OTG_ID_CONFIG_CMD__NONE);

} /* pm_init_otg_usb( void ) */

/*===========================================================================

FUNCTION  FSBL_HW_INIT_USB

DESCRIPTION
  This function performs hardware initialization to setup GPIO, SBI, PMIC,
  clocks, and USB/UART for downloader. 

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  GPIO, SBI, PMIC, and USB are all initialized.

===========================================================================*/
void fsbl_hw_init_usb( void )
{
  /* Setup GPIOs for UART operation */  
  gpio_init();
  
  /* Setup clocks for dload, SBI and PMIC */
  clk_regime_init_for_dload();

  /* Setup SBI and PMIC */
  boot_pm_init();

  /* Initialize USB */
  pm_init_otg_usb();
}

/*===========================================================================

FUNCTION  FSBL_HW_INIT_AND_ENTER_DLOAD

DESCRIPTION
  This function performs hardware initialization to setup USB/UART and
  other hardware for the downloader.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.

===========================================================================*/
void fsbl_hw_init_and_enter_dload( void )
{

  /* Call for dload entry contained within boot_error_handler.s */
  extern boot_downloader( void );

  /* Call for dload entry contained within boot_error_handler.s */
  //extern dload_entry( void );
  
  /* Setup USB/UART for downloader. */
  fsbl_hw_init_usb();

  /* Enter downloader for QPST */  
  //dload_entry();
  boot_downloader();

} /* fsbl_hw_init_and_enter_dload */
#endif /* FSBL_ENABLED_DLOAD */

/*============================================================================

FUNCTION  FSBL_HW_INIT

DESCRIPTION
  This function performs hardware initialization.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void fsbl_hw_init(void)
{
  HWIO_OUT(SW_RESET_REG, 0x0);
}

