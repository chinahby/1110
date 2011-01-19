/*=============================================================================

                        P M I C   I N I T   M O D U L E

GENERAL DESCRIPTION
  This function configures GPIO lines for PMIC SBI, sets the default
  MSMC voltage and turns on backlight.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2005,2006 by QUALCOMM, Inc.  All Rights Reserved.
==============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/boot/boot_pm_init.c#3 $    
$DateTime: 2009/02/20 09:45:21 $ $Author: taaran $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/20/09    tnk    added support for USB detection using Vchg
11/30/07    ps     Adding support for SECBOOT 2.0
04/26/06    ty     Changes to support multi task in OEMSBL
02/22/06   Vish    Replaced pm_init() with pm_init_comm() and 
                     pm_model_specific_init() in boot_pm_init().
01/25/06    ly     Remove pm_init_otg_usb from boot_pm_init. Call it in
                   oemsbl_dload_entry before entering dload mode.
11/22/05    ty     Modify to save PMIC power on status
05/26/05    ty     Modify to init full PMIC lib instead of mini PMIC lib
04/29/05    tkuo   Add FEATURE_UI_CORE_REMOVED.
04/02/04    ih     Initial revision

==============================================================================*/



/*=============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "comdef.h"
#include "target.h"

#include "pm.h"
  //#include "clk.h"

#if defined(FEATURE_PMIC_WDOG) && defined(FEATURE_SHARED_SEGMENT)

#include "boot_trap.h"
#include "boot_shared_seg.h"
#define PMIC_ERROR_FLAG  0xFFFF 

#endif

#ifdef BUILD_BOOT_CHAIN
#include BOOT_CUSTSBL_H
#endif

#define DISP_DEFAULT_BACKLIGHT 2
#ifdef FEATURE_PMIC_VREG_INIT_BYPASS  

/* MSMP Voltage setting  = 2.6 Volts */
#define MSMP_LEVEL        2600

/* MSME1 Voltage setting  = 1.80 Volts*/
#define MSME1_LEVEL       1800

/* MSMC1 Voltage setting  = 1.20 Volts*/
#define MSMC1_LEVEL       1200

/* MSMA Voltage setting  = 2.6 Volts */
#define MSMA_LEVEL        2600

/* TCXO Voltage setting  = 2.85 Volts */
#define TCXO_LEVEL        2850
#endif /* FEATURE_PMIC_VREG_INIT_BYPASS */

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
  pm_vreg_set_level( PM_VREG_USB_ID, 3300);

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
}

/*============================================================================

FUNCTION BOOT_PM_INIT

DESCRIPTION
  Initializes the PMIC library

DEPENDENCIES
  bio_gpio_init() has been called,
  sbi_init() has been called.

RETURN VALUE
  Returns 0.

SIDE EFFECTS
  PMIC is initialized to a known state.
  LCD backlight is turned on, LDO to LCD is turned on.
============================================================================*/
uint32 boot_pm_init ( void )
{
  /* initialize PMIC */
  if (pm_init_comm() == PM_ERR_FLAG__SUCCESS)
  {
     pm_model_specific_init();
  }

      #if !defined(FEATURE_UI_CORE_REMOVED) && defined(FEATURE_BOOT_BACKLIGHT_DISPLAY)
           /* Turn on LCD backlight */
           pm_set_led_intensity(PM_LCD_LED, DISP_DEFAULT_BACKLIGHT);
           pm_set_led_intensity(PM_KBD_LED, DISP_DEFAULT_BACKLIGHT);

          /* Turn on LDO to LCD */
          pm_vreg_control(PM_ON_CMD, PM_VREG_WLAN_M);
     #endif /* FEATURE_UI_CORE_REMOVED & FEATURE_BOOT_BACKLIGHT_DISPLAY */

  #ifdef FEATURE_PMIC_VREG_INIT_BYPASS_PORTED  
      /* Initialize the different VREGs Voltage settings. */
	  if(pm_vreg_set_level(PM_VREG_MSMP_ID,  MSMP_LEVEL) != PM_ERR_FLAG__SUCCESS)
	  {
	      ERR_FATAL("Unable to set VREG_MSMP voltage",0,0,0);
	  }
	  if(pm_vreg_set_level(PM_VREG_MSMA_ID,  MSMA_LEVEL) != PM_ERR_FLAG__SUCCESS)
	  {
	      ERR_FATAL("Unable to set VREG_MSMA voltage",0,0,0);
	  }
	  if(pm_vreg_set_level(PM_VREG_TCXO_ID,  TCXO_LEVEL) != PM_ERR_FLAG__SUCCESS)
	  {
	      ERR_FATAL("Unable to set VREG_TCXO voltage",0,0,0);
	  }
	  if(pm_vreg_set_level(PM_VREG_MSME1_LDO_ID,  MSME1_LEVEL) != PM_ERR_FLAG__SUCCESS)
	  {
	      ERR_FATAL("Unable to set VREG_MSME1_LDO voltage",0,0,0);
	  }
	  if(pm_vreg_set_level(PM_VREG_MSMC1_ID,  MSMC1_LEVEL) != PM_ERR_FLAG__SUCCESS)
	  {
	      ERR_FATAL("Unable to set VREG_MSMC1 voltage",0,0,0);
	  }
  #endif


  return 0;
}

#if defined(FEATURE_PMIC_WDOG) && defined(FEATURE_SHARED_SEGMENT)
/*============================================================================

FUNCTION BOOT_PM_SAVE_POWER_ON_STATUS

DESCRIPTION
  Saves the PMIC power on status into IRAM for AMSS

DEPENDENCIES
  pm_init() has been called.
  sbi_init() has been called.

RETURN VALUE
  None

SIDE EFFECTS
  PMIC power on status is saved into shared segment
============================================================================*/
void boot_pm_save_power_on_status(void)  
{
  int i;
  pm_rt_status_type status_req[]=
    {
      PM_KPD_PWRON_EVENT_RT_ST,
      PM_WDOG_TOUT_RT_ST,
      PM_OVER_TEMP_RT_ST,
      PM_CABLE_IN_RT_ST,
      PM_SMPL_RT_ST,
      PM_PWR_RST_RT_ST,
      PM_OSC_HALT_RT_ST,
      PM_RTC_ALRM_RT_ST,
      PM_VCP_IRQ_RT_ST,
      PM_PULSE_CHG_DONE_RT_ST,
      PM_CHG_P_LIMITING_RT_ST,
      PM_CHG_I_LIMITING_RT_ST,
      PM_INVALID_USB_CHG_RT_ST,
      PM_VALID_USB_CHG_RT_ST,
      PM_INVALID_CHG_RT_ST,
      PM_VALID_CHG_RT_ST
    };
  const int status_req_size = sizeof(status_req)/sizeof(pm_rt_status_type);
  boolean status;
  uint32 result=0;
  boot_trap_shared_type *boot_info_ptr;


  if (!BOOT_SHARED_SEG_ALLOC(boot_trap_shared, sizeof(boot_trap_shared_type), TRUE, (void**)(&boot_info_ptr)))
  {
    /* failed to get memory, or it was already initialized */
    return;
  }
  for (i=0;i<status_req_size;i++)
  {
    if (pm_get_rt_status(status_req[i], &status) != PM_ERR_FLAG__SUCCESS)
    {
      result = PMIC_ERROR_FLAG;
      break;
    }
    if (status)
    {
      result |= (1<<i);
    }
  }
  boot_info_ptr->pon_status = (uint16)result;

  pm_clear_irq(PM_OVER_TEMP_IRQ_HDL);
  pm_clear_irq(PM_CABLE_IN_IRQ_HDL);
  pm_clear_irq(PM_SMPL_IRQ_HDL);
  pm_clear_irq(PM_WDOG_TOUT_IRQ_HDL);
}
#endif

#ifndef FEATURE_OEMSBL_MULTI_TASK
/* Stub functions from respective files */

/* drivers/pm/pmapp.c */
void pm_app_clk_busy_wait(uint32 us)
{
    /* Halts the Microprocessor for the specified amount of usecs. */
    clk_busy_wait(us);
}

#endif
