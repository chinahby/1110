/*=============================================================================

  Operating System Boot Loader HARDWARE INITIALIZATION

GENERAL DESCRIPTION
  This file does basic hardware initialization at power up. 
  
EXTERNALIZED FUNCTIONS
  osbl_hw_init 
  osbl_increase_clk_speed 
  osbl_hw_set_jtag_state
  osbl_hw_smc_lock
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:$
  $Header: //depot/asic/qsc1100/romboot/osbl/osbl_hw_init.c#10 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/25/09   dk      Fixing featurisation issues seen for osbl_power_down function.
05/13/09   dk      Added RTC software workaround when oscillators are stopped
                          when battery is removed which causes RTC time to be wrong.
03/24/09   dk      Removing compiler warnings.
03/19/09   dk      Added optimised method to initialise the modem RF part for 
                   fix related to CR140131.
10/16/08   tnk      Include fastenum ,and initialising the sbi,pmic,clkregime 
10/25/07   tkuo    Clean up the old code.
10/01/07   ly      Ported from MSM6260
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "osbl.h"
#include "osbl_target.h"
#include "osbl_msm.h"
#include "osbl_hw_init.h"
#include "boot_dbl_if.h"
#include "osbl_error_handler.h"
#ifdef GPIO_H
#include GPIO_H
#endif

#include "clk.h"
#include "clkrgm_msm.h"
#include "sdcc_boot_init.h"
#include "pm.h"
#include "Pmsbl.h"
#include "osbl_usb_chg_init.h"
#if defined(FEATURE_QHSUSB_FAST_ENUMERATION)
#include "qhsusb_al_fast_enum_api.h" 
#endif
#define OSBL_QHS_USB_ENUM_COUNT 1024

#define SBI_9p6MHZ                 2

/* Defining the Kick watchdog macro */
#define BOOTHW_KICK_WATCHDOG()                                               \
            HWIO_OUT(WDOG_RESET, HWIO_FMSK(WDOG_RESET,WATCH_DOG));

/* SBI initialization from the SBI library */
extern void sbi_init( uint32 freq );

extern void clk_regime_init_for_dload( void );


/* #include "boothw.h" */
/* #include "clk.h" */

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
static void
osbl_hw_pm_validate_rtc_state(void);

static void 
sbi_hw_init( void );

/*===========================================================================

  FUNCTION sbi_hw_init()

  DESCRIPTION
    This function would initialise Modem registers. Solution for CR 140131.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS

===========================================================================*/

static void 
sbi_hw_init( void )
{
   HWIO_OUT (MODEM_CLK_HALT, ~( (0x00000000) | 
    ((uint32)1<<HWIO_MODEM_CLK_HALT_CDMA_CHIPXN_CLK_HALT_SHFT ) | 
    ((uint32)1<<HWIO_MODEM_CLK_HALT_CDMA_CHIPXN_DIV2_CLK_HALT_SHFT ) | 
    ((uint32)1<<HWIO_MODEM_CLK_HALT_CDMA_CHIPXN_DIV4_CLK_HALT_SHFT ) | 
    ((uint32)1<<HWIO_MODEM_CLK_HALT_CDMA_CHIPXN_DIV4_CLK_HALT_SHFT ) | 
    ((uint32)1<<HWIO_MODEM_CLK_HALT_RXF_SAMP_CLK_HALT_SHFT ) | 
    ((uint32)1<<HWIO_MODEM_CLK_HALT_MODEM_SBI_CLK_HALT_SHFT ) ) );              

   HWIO_OUTI (RXF_SRCc_DACC_CTL, 0, 0); 
   HWIO_OUTI (RX_AGCc_SBI_ID, 0, 0);          
   HWIO_OUTI (RX_AGCc_SBI_ADDR, 0, 0xE1);   
} /* sbi_hw_init */


static void
osbl_hw_pm_validate_rtc_state(void)
{
#ifdef FEATURE_POWER_ON_CONTROL_FROM_RTC_WORKAROUND
/* NOTE
** The following code has been suggested by PMIC team as part of the solution for the issue.
** This workaround has been reviewed by PMIC team and  if any issue seen in future,
**  PMIC team should be able to address the issue.
*/
    boolean trigger_state = 0;
    boolean rtc_state = 0;

    /* RTC software workaround to resolve oscillator stopped working when battery is removed 
       causing wrong RTC time */
    if (pm_hal_rtc_get_pwr_on_triggered_from_rtc(&trigger_state) == PM_ERR_FLAG__SUCCESS) 
    {
        if( trigger_state == TRUE )
        {
            if (pm_hal_rtc_get_reset_trigger(&rtc_state) == PM_ERR_FLAG__SUCCESS)
            {
                /* Check to see if RTC must be reset */
                if( rtc_state == FALSE )
                {
                    /* Reset RTC alarms */
                    if( pm_hal_rtc_disable_alarm(PM_RTC_ALL_ALARMS) == PM_ERR_FLAG__SUCCESS)
                    {
                        /* Power down phone because RTC is not reliable */
                        osbl_power_down();
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
    }
#endif
}

/*--------------------------------------------------------------------------
   External function declarations.
--------------------------------------------------------------------------*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS
FUNCTION  osbl_boot_pm_init

DESCRIPTION
  This function performs hardware initialization
  required for configurig PMIC communication interface.
  For Pmic to be initialise sbi_init() is needed.Since sbi_init is not being called 
  till now in boot these calls were added. 

DEPENDENCIES
  Function added for enumeration feature and dead battery charging feature
  
RETURN VALUE
  None

SIDE EFFECTS
  None


*/


void osbl_boot_pm_init( void )
{
  /* Initialize the serial bus controllers */
  sbi_init( SBI_9p6MHZ );
    if (pm_init_comm() == PM_ERR_FLAG__SUCCESS)
  {
     pm_model_specific_init();
  }
    else 
         OSBL_ERR_FATAL(BL_ERR_OSBL);

  /* Wait 102 microseconds for the rail to charge */
  clk_busy_wait(102);  
} /* boot_pm_init( void ) */


/*===========================================================================

FUNCTION  OSBL_HW_INIT

DESCRIPTION
  This function performs hardware initialization. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_hw_init( bl_shared_data_type *bl_shared_data )
{


   /* Setup clocks for dload, SBI and PMIC */
  gpio_init();
  /*  gpio_init();   */
clk_regime_init_for_dload();

  /* Setup SBI and PMIC * /
  boot_pm_init();*/
osbl_boot_pm_init();
#if defined(FEATURE_QHSUSB_FAST_ENUMERATION)
  /* Initialize USB */
  pm_init_otg_usb();
#endif

   /* Need to make sure KEypad and LCD is turned Off to satisfy the fast charging compilance
   ** current limits.
   */
   pm_set_led_intensity(PM_LCD_LED, 0x0);
   pm_set_led_intensity(PM_KBD_LED, 0x0);
           
  /* Fix for the Jammer Interrupt CR140131 */
  sbi_hw_init();

  /*CR177613 - SDCC GPIO lines not clean at boot-up time*/
  sdcc_boot_init();

  /* CR181788 - PMIC API to be used to check if RTC clock and RTC alarm is still valid for SW workaround */
  osbl_hw_pm_validate_rtc_state();

    return;
}/* osbl_hw_init */ 

/*===========================================================================

FUNCTION  OSBL_INCREASE_CLK_SPEED

DESCRIPTION
  This function increase clk speed to normal operation speed

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_increase_clk_speed
(
  bl_shared_data_type *bl_shared_data
)
{

}/* osbl_increase_clk_speed() */

/*===========================================================================

FUNCTION  OSBL_HW_SET_JTAG_STATE

DESCRIPTION

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.

===========================================================================*/
void osbl_hw_set_jtag_state( boolean enable_jtag )
{
  /* Determine if mARM JTAG debugging should be enabled. */
  if( enable_jtag == TRUE )
  {
     /* Enable the JTAG ability */
     out_dword(HWIO_ADDR(ENABLE_ARM9_JTAG), 0x1);
  }
  else
  {
     /* Disable the JTAG ability */
     out_dword(HWIO_ADDR(ENABLE_ARM9_JTAG), 0x0);
  }
} /* osbl_hw_set_jtag_state() */

/*===========================================================================

FUNCTION  OSBL_HW_SMC_LOCK

DESCRIPTION
  Disable modem Tx, enable JTAG access, and lock e-fuse override
  registers.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.

===========================================================================*/
void osbl_hw_smc_lock( void )
{
  /*SMC_LOCK();*/
} /* osbl_hw_smc_lock() */
/*===========================================================================

FUNCTION  do_fast_enum

DESCRIPTION
  Initialise the QHSUSB STACK AND POLL FOR THE ENUMERATION TO 
  FOR 1.2 SECS.
DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Hardware is initialized.

===========================================================================*/

#if defined(FEATURE_QHSUSB_FAST_ENUMERATION) && defined(FEATURE_QHSUSB) 
void do_fast_enum(void)
{
    unsigned int count=0;
    boolean usb_detected=FALSE;

     qhsusb_al_fast_enum_init();
    while ( (usb_detected != TRUE )&&
            (count < OSBL_QHS_USB_ENUM_COUNT ) )
        {
        usb_detected = qhsusb_al_fast_enum_poll();
        count++;
        OSBL_KICK_WATCHDOG();
        qhsusb_al_fast_enum_delay_usec(1000);
        }

    // Allow USB Host to complete installation before shutdown
    for(count=0;count<100;count++)
    {
        qhsusb_al_fast_enum_poll();
        OSBL_KICK_WATCHDOG();
        qhsusb_al_fast_enum_delay_usec(1000);
    }
    /*shuts down the HS-USB stack, and disconnects from the host*/
    qhsusb_al_fast_enum_shutdown();
}
#endif


/*===========================================================================

FUNCTION OSBL_POWER_DOWN 

DESCRIPTION
  This function powers down the phone by holding the PS_HOLD low.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Phone enters the power-down.

===========================================================================*/
void 
osbl_power_down ( void )
{

      #if defined(FEATURE_PMIC_MOMENTARY_SUDDEN_PWR_LOSS)
      /* Disable Sudden Momentary power loss driver to prevent the phone from
         automatically restarting */
      pm_smpld_switch(PM_OFF_CMD);
      #endif

      #if defined(FEATURE_PMIC_WDOG)
      /* Disable Watchdog Reset in PMIC */
      pm_watchdog_reset_detect_switch(PM_OFF_CMD);
      #endif

      /* Pull PS_HOLD low */
      /*BIO_OUT( BIO_PS_HOLD_REG, BIO_PS_HOLD_M, BIO_PS_HOLD_OFF_V );*/
      HWIO_OUTF(PMIC_CONTROL,PS_HOLD,0x0);
      /* The MSM resets the phone, no more code execution beyond
       * this point
       */
      for (;;)
      {
        BOOTHW_KICK_WATCHDOG();
      }
}

