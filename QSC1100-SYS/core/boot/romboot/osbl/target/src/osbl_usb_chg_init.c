
/*=============================================================================

               O E M  S e c o n d a r y  B o o t  L o a d e r
                       USB Battery Charging Handler 

GENERAL DESCRIPTION
    This file contains the functions that will handle the scenario when the
phone boots from USB port and finds that battery is too low to assist the
boot process. The phone will attempt to charge the battery first before
continuing to boot.

EXTERNALIZED FUNCTIONS
  osbl_usb_chg_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/09   dk      Removing compiler warnings.
05/25/09   dk      Fixing featurisation issues seen for osbl_power_down function.
03/10/09   dk      Added fix for removing unwanted LCD_LED call for keypad backlight.
02/20/09   tnk     Added the battery temp monitoring and USB_OTG_INIT

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "qhsusb_al_chg_api.h"
#include "bio.h"
#include "pm.h"
#include "pmsbl.h"
#include "osbl.h"
#include "boothw.h"
#include "clk.h"
#include "osbl_usb_auto_chg_init.h"
#include "clkregim.h"
#include "osbl_msm.h"
#include "osbl_target.h"

/*=============================================================================

            GLOBALS and EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE
This section contains externs and globals that will be needed by this module.
=============================================================================*/
/* This global variable to keep track of the clock frequency. We need this to 
 * make our "clock-wait" delays uniform. 
 */

extern void pm_init_otg_usb( void );
extern pm_model_type pm_ft_get_pmic_model(void);

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#define osbl_bus_speed_in_mhz 20

/* Set the battery alarm upper and lower threshold
 * BAT_UPR_THRESH = 3300 mV is a little higher than 3200 mV, which is when the
 * the battery can be given the as much as the current that the USB can be
 * supplied. The 100 mV tolerance provides enough leeway to avoid any 
 * oscillations between trickle charging and fast charging.  
 * BAT_LWR_THRESH = 3000 mV = This is where battery is very week.
 */
#ifndef OSBL_USB_CHG_BAT_UPR_THRESH
#define OSBL_USB_CHG_BAT_UPR_THRESH 3300
#endif

#ifndef OSBL_USB_CHG_BAT_LWR_THRESH
#define OSBL_USB_CHG_BAT_LWR_THRESH 3000
#endif

/*Low power mode frequency, which uses TCXO frequency , 19 Mhz */
#define OSBL_USB_CHG_LOW_POWER_MODE_FREQ 19

/* Normal power mode frequency, which uses PLL frequency.
** This needs to be checked for different clock plan.
*/
#define OSBL_USB_CHG_NORMAL_MODE_FREQ 48

/* This variable need to be updated at the start of function to get the 
 * speed of OEMSBL. 
 */
uint32  osbl_chg_normal_mode_freq = OSBL_USB_CHG_NORMAL_MODE_FREQ;
uint32 osbl_ahb_chg_freq =OSBL_USB_CHG_NORMAL_MODE_FREQ;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

FUNCTION get_osbl_ahb_bus_freq 

DESCRIPTION
 This function returns the clock frequency of AHB bus in osbl.It is only used 
 during dead battery charging where the frequency of the AHB before Dead battery 
 charging is 48Mz and during Dead Battery charging its 19.2 Mhz.
DEPENDENCIES
  
  
RETURN VALUE
  NONE

SIDE EFFECTS
  This function should not be called other than osbl_clk_busy_wait

=============================================================================*/
uint32 get_osbl_ahb_bus_freq()
{

    return osbl_ahb_chg_freq;
}
/*===========================================================================

FUNCTION OSBL_DELAY_IN_MS 

DESCRIPTION
  This function implements delay by calling clk_busy_wait. Also since this 
  is part of boot-code and there is no task to kick the watchdog, this 
  function periodically kicks the watch-dog.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  NONE

SIDE EFFECTS
  Since the MSM is running in single thread mode and this is not timer based,
  this processor time is time wasted, however this may be the only option 
  during boot-up.

===========================================================================*/
void
osbl_delay_in_ms ( uint32 wait_in_ms )
{
    uint32 wait_idx ;
    for ( wait_idx = 0 ; wait_idx < wait_in_ms ; wait_idx++ )
    {
        osbl_clk_busy_wait(500) ; 
        OSBL_KICK_WATCHDOG();
        osbl_clk_busy_wait(500) ; 
    }
}

#if 0 /* Not used for KIP and HAN charging code */

/*===========================================================================

FUNCTION OSBL_PM_SET_USB_SUSPEND_CURRENT 

DESCRIPTION
  This function sets the USB current that can be drawn by phone from PC to
  about 0 mA. The spec says it should be less than 2 mA.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  pm_err_flag_type
  0   : Success.
  > 0 : Failure.

SIDE EFFECTS
  USB-based-Charging is disabled after call to this function.

===========================================================================*/
pm_err_flag_type
osbl_pm_set_usb_suspend_current ( void )
{
    pm_err_flag_type  err            = PM_ERR_FLAG__SUCCESS ;
    pm_chg_uchg_q_imax_type rawValue = 0 ;

    err = pm_chg_usb_charger_transistor_imaxsel ( rawValue ) ;
    err |= pm_chg_usb_charger_transistor_switch ( PM_OFF_CMD ) ;

    return err ;
}

/*===========================================================================

FUNCTION OSBL_PM_CHECK_N_SET_USB_CURRENT 

DESCRIPTION
  This function basically polls the USB for USB state changes and looks for
  relevant events like USB connection, disconnection, suspension etc.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  pm_err_flag_type
  1 if: 
  	1. Wall or USB charger connected.
    2. NO PC-USB physical connection exists any longer. 
  0 if:
    1. HS-USB not enumerated.
	2. HS-USB suspended and disconnected.

SIDE EFFECTS
  NON

===========================================================================*/
boolean
osbl_pm_check_n_set_usb_current ( 
                                     boolean *hsusb_enumerated ,
                                     boolean *usb_wall_charger_connected , 
                                     boolean *has_fast_charging_started ,
									 boolean *has_usb_cable_been_disconn 
                                  )
{

    boolean return_value = FALSE ;
    qhsusb_boot_poll_result poll_result = hsu_boot_poll_stack() ;

    /* HSUSB enumerated */
    if ( poll_result == HSU_BOOT_POLL_RESULT_ENUMERATED ) 
    {
      *hsusb_enumerated = TRUE;
    }
    else if ( poll_result == HSU_BOOT_POLL_RESULT_SUSPENDED ||
              poll_result == HSU_BOOT_POLL_RESULT_DISCONNECTED )
    { 
        /* when HSUSB is no longer in ENUMERATED state, will fall back to low current
         * charging thus opening the BAT FET
         */
      osbl_pm_set_usb_suspend_current () ; 
      *hsusb_enumerated = FALSE ;
      *has_fast_charging_started = FALSE ;
    }
    else if ( poll_result == HSU_BOOT_POLL_RESULT_CONNECTED_TO_WALL_CHG )
    {
        *usb_wall_charger_connected = TRUE;
        return_value = TRUE  ;
    }
	else if ( poll_result == HSU_BOOT_POLL_RESULT_NOT_CONNECTED ) 
    {
      /* when HSUSB is no longer in ENUMERATED state, will fall back to low current
       * charging thus opening the BAT FET
       */
      osbl_pm_set_usb_suspend_current () ; 
      *hsusb_enumerated = FALSE ;
      *has_fast_charging_started = FALSE ;
	  *has_usb_cable_been_disconn = TRUE ;
      
	  return_value = TRUE  ;
    }
 
	return return_value ;

}

#endif /* Not used for KIP and HAN charging code */

/*===========================================================================

FUNCTION OSBL_USB_CHG_SET_LED

DESCRIPTION
  This function switches the LED ON or OFF as specified by the argument

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Phone draws current & this might not be desired when the battery is low.

===========================================================================*/

void
osbl_usb_chg_set_led(boolean value)
{

    static boolean led_on = FALSE;

    if ((value == TRUE) &&  (led_on == FALSE))
    {   
         /* Turn on the keypad backlight */
        pm_set_led_intensity(PM_KBD_LED, 1);
        led_on = TRUE;
    }
    else if ((value == FALSE) &&  (led_on == TRUE))
    {
        /* Turn off the keypad backlight. This should have an effect of
         * a flash. Keypad backlight flashing is an indication to the
         * user that the phone is attempting to charge the battery up
         */

        pm_set_led_intensity(PM_KBD_LED, 0);
        led_on = FALSE;        
    }
}






/*===========================================================================

FUNCTION OSBL_USB_CHG_SET_CLOCK_MODE 

DESCRIPTION
  This function changes the MSM clock as specified by the argument

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  This function should be called carefully.

===========================================================================*/
#if 0
void
osbl_usb_chg_set_clock_mode ( clkrgm_usb_chg_mode_type value )
{

    if ( ( value ==CLKRGM_USB_CHG_MODE ) && 
			 ( clkrgm_get_usb_chg_mode() != CLKRGM_USB_CHG_MODE ) ) 
		{
          osbl_bus_speed_in_mhz = OSBL_USB_CHG_LOW_POWER_MODE_FREQ ;
          clkrgm_set_usb_chg_mode ( CLKRGM_USB_CHG_MODE ) ;
        }
        else if ( ( value ==CLKRGM_USB_NORMAL_MODE ) && 
				( clkrgm_get_usb_chg_mode() != CLKRGM_USB_CHG_MODE ) )
        {
          osbl_bus_speed_in_mhz = osbl_chg_normal_mode_freq;
          clkrgm_set_usb_chg_mode(CLKRGM_USB_NORMAL_MODE);
		}
}
#endif



#if 0 /* Not used for KIP and HAN charging code */
/*===========================================================================

FUNCTION OSBL_USB_CHG_POLL_WHILE_WAITING 

DESCRIPTION
  This function:
  1. Waits in a loop as per the 1st argument.
  2. Polls the USB status every 10 mS or less.
  3. Updates the status of the remaining arguments.  
  4. Blinks the LED. 

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  This function should be called carefully.

===========================================================================*/
boolean
osbl_usb_chg_poll_while_waiting ( 
				                     uint32 time_period,
                                     boolean *hsusb_enumerated ,
                                     boolean *usb_wall_charger_connected , 
                                     boolean *has_fast_charging_started ,
									 boolean *has_usb_cable_been_disconn 
                                   )
{

        uint32  wait_idx;

        /* Wait for time_period milli-seconds */ 
        for ( wait_idx = 0 ; wait_idx < time_period ; wait_idx++ )
        {
            osbl_delay_in_ms ( 1 ) ;

            /* Less then 10 msecs, update the poll_result as per specs and
             * also check the usb-enumeration.
             */
            if   ( !( wait_idx % 8 ) )
            {
                
                /* Retrun immediately if the USB Wall Charger is discovered */  
                if ( osbl_pm_check_n_set_usb_current ( 
                                       hsusb_enumerated , 
                                       usb_wall_charger_connected ,
                                       has_fast_charging_started ,
									   has_usb_cable_been_disconn ) )
                {
                        return TRUE ;       
                }
            }

            if ( wait_idx > time_period/2 )
            {   
                osbl_usb_chg_set_led( TRUE ) ;
            }
            else
            {
                osbl_usb_chg_set_led( FALSE ) ;
            }

        }
    return FALSE ;      
}

#endif /* Not used for KIP and HAN charging code */


/*===========================================================================

FUNCTION  OSBL_LOW_BATT_CHG

DESCRIPTION
    This function implements the core algorithm that handles the special 
case where the phone boots from USB and finds the battery is too low. It will
attempt to charge up the battery by trickle charging before boot-up continues.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/
static void osbl_low_batt_chg( void )
{
#if 0 /* Not used for KIP and HAN charging code */
  uint16 idx = 0, wait_idx = 0;uint32 delay_var =2500 ;
  /* tc_on: period when trickle charge is enabled
   * tc_off: period when trickle charge is disabled
   */
  uint16 tc_on = 250, tc_off = 2000 ;

  /* max number of tries to charge */
  uint32 max_loop_num = 50000;
  /* flag to indicate if HSUSB is already enumerated */
  boolean hsusb_enumerated = FALSE ;
  /* flag to indicate if a high current USB charger is connected */
  boolean usb_wall_charger_connected = FALSE ;

  /* Flag to indicate the bad battery */
  boolean bad_battery = FALSE ;  

  /* Flag to indicate if the fast charging has started or not */
  boolean has_fast_charging_started = FALSE ;

  /* Flag to indicate if the USB-cable has been disconnected */
  boolean has_usb_cable_been_disconn = FALSE ;

 /* Battery Level Indicator */  
  volatile int vbatt_threshold_indicator  = 0 ;


 
 if  ( pm_sbl_usb_sess_is_valid() )  // If it's not USB, then it's DC wall, continue to boot
  {
  
    pm_set_led_intensity(PM_LCD_LED, 0);
    pm_set_led_intensity(PM_KBD_LED, 0);

    /* Set the battery alarm upper and lower threshold
     * BAT_UPR_THRESH = 3600 mV = threshold for detection of battery absence
     * BAT_LWR_THRESH = 3200 mV = threshold for stopping charging
     */
    pm_sbl_set_batt_alarm_threshold(OSBL_USB_CHG_BAT_LWR_THRESH,
                                    OSBL_USB_CHG_BAT_UPR_THRESH); 

    /* Enable battery alarm comparators */
    pm_sbl_enable_batt_alarm(TRUE);

    /* Enable USB charging by closing the USB pass transistor */
    pm_chg_usb_charger_transistor_switch(PM_ON_CMD);

    /* Open battery FET */
    pm_chg_battery_transistor_switch(PM_OFF_CMD);

   /* Wait for sometime to settle the hardware circuitry */ 
    for (wait_idx = 0; wait_idx < 10; wait_idx++)
    {
      BOOTHW_KICK_WATCHDOG();
      clk_busy_wait(1000); 
    }
    /* Start the HSUSB stack */
    hsu_boot_start_stack();

    /* Pending: Check on the value of max_loop_num */
    for (idx = 0; idx < max_loop_num; idx++) 
    {
	  /* Delay variable for various charging phases */
	  uint32 delay_var = 0 ;

      /* Disable trickle charging */
      pm_chg_trickle_config(PM_CHG_TR_IMAX_00mA);

      /* It has to wait long enough because:
       * 1. The comparator needs time to settle before an accurate
       *    VBAT status can be read;
       * 2. The backlight has to stay on long enough for the user
       *    to see it
       */ 

     /*
     ** For trickle charging , tc_off would also serve to wait for the off pulse time
     ** For HSUSB charging , it would be a delay to make sure the VBAT
     ** is read after proper settling time.
      */

      if ( has_fast_charging_started == FALSE ) 
	  {
		  delay_var = tc_off ;  
          
	  }
	  else
	  {
		  delay_var = 2500 ;
	  }

  osbl_usb_chg_poll_while_waiting( 
                                           delay_var ,
                                           &hsusb_enumerated ,
                                           &usb_wall_charger_connected , 
                                           &has_fast_charging_started ,
										   &has_usb_cable_been_disconn ) ;
   
       if ( ( usb_wall_charger_connected ) || ( has_usb_cable_been_disconn ) )
       {
           break; /* Continue bootup with usb wall charger connected */
       }
   

      /* Check the VBAT status */
/*      vbatt_threshold_indicator = pm_sbl_read_bat_thresh() ;  */
      if ( vbatt_threshold_indicator > 0 ) 
      {
          /* If the battery is bad or disconnected, power down the phone */
          if ( bad_battery )
          {
              osbl_power_down() ; 
          }

        /* VBAT is higher than the desired level, battery is identified as good , Continue bootup*/
          break;
      }
      else if ( vbatt_threshold_indicator < 0 || !hsusb_enumerated ) 
      {
          /* VBAT is lower than the desired threshold and trickle-charging has
         * to begin/continue.
         * Put the phone in low power mode if it has not been already done. 
         */ 
         osbl_usb_chg_set_clock_mode(CLKRGM_USB_CHG_MODE);

        /* FET can switched off before we start the trickle-charging */
        pm_chg_battery_transistor_switch(PM_OFF_CMD);

        /* Enable trickle charging with current of 40mA */
        pm_chg_trickle_config(PM_CHG_TR_IMAX_40mA);

       /*
       ** For trickle charging , tc_on would serve as charging time
       */
       osbl_usb_chg_poll_while_waiting( 
                                            tc_on,
                                            &hsusb_enumerated ,
                                            &usb_wall_charger_connected , 
                                            &has_fast_charging_started ,
										    &has_usb_cable_been_disconn  ) ;
                                            
    
        if ( TRUE == usb_wall_charger_connected )
        {
            break; /* Continue bootup with usb wall charger connected */
        }
   
       
                    
        /* If any one of the conditions below is true, reset the phone:
         * 1. Battery is absent, which is indicated by VBAT being higher
              than the upper threshold
         * 2. USB is no longer connected (before the battery is charged
              up to the desired voltage level)
         * 3. It times out and the battery is still not charged up to
              the desired level, which indicates the battery may be
              very low or even dead
         */ 
        if ( !pm_sbl_usb_sess_is_valid() ||
             (idx == max_loop_num - 1) )
        {
          osbl_power_down () ; 
        }
        else if (pm_sbl_read_bat_thresh() > 0)
        { /* Battery is read again while trickle charging to get the most 
            updated value*/

          /* When the battery is absent, it takes much longer for perhaps
           * some kind of capacitance to discharge before VBAT can give
           * an accurate reading again. So set the wait state with trickle
           * charging disabled to a much longer period
           */
          tc_off = 1500 ;

		  /* Bad Battery detection needs a more complex algorithm and 
		   * that might be addressed in the next release. For this 
		   * release we will assume that we never get a bad battery.
		   
		   bad_battery = TRUE ; */
        }
        else
        {
          /* The battery is present and will continue to be charged.
           * It takes much shorter in this case to have an accurate
           * VBAT reading. Also a shorter period with trickle charge
           * disabled improves the efficiency of the charging
           * algorithm
           */
          tc_off = 50;
        }
      } 
      else if ( vbatt_threshold_indicator == 0 )
      {
        /* Battery voltage is good to facilitate HSUSB fast charging */
        if ( hsusb_enumerated && !has_fast_charging_started )
        {
          /* has_fast_charging_started is also used as flag to indicate HSUSB 
             is enumerated such that:
           * 1. hsu_boot_poll_stack is only called once
           * 2. the below configuration for high current charging will only be 
                done once
           */
          has_fast_charging_started  = TRUE ;

          /* Disable trickle charging, before fast charging is enabled */
          pm_chg_trickle_config(PM_CHG_TR_IMAX_00mA);
          /* Set current limit to 500mA for enumerated USB */
          pm_chg_usb_charger_transistor_imaxsel( PM_CHG_UCHG_Q_IMAX_050mV_div_R );
          /* Switch on the transistor to let the charging current through */
          pm_chg_battery_transistor_switch(PM_ON_CMD);

        }
      }
    } /* END for */
    /* Disable trickle charging */
    pm_chg_trickle_config(PM_CHG_TR_IMAX_00mA);

    /* Reset the battery alarm threshold */
    pm_sbl_set_batt_alarm_threshold(3200, 4000); 

    /* Disable battery alarm comparators */
    pm_sbl_enable_batt_alarm(FALSE);

    /* Put the phone out of lower mode to resume fast boot up */
    osbl_usb_chg_set_clock_mode(CLKRGM_USB_NORMAL_MODE);

     /* Disable USB charging by opening the USB pass transistor */
     if ( !usb_wall_charger_connected )
     {
        osbl_pm_set_usb_suspend_current () ; 
     } 
     /* Stop the stack and boot-up */
     hsu_boot_stop_stack();

   
  /* Put the USB transceiver back in SHUTDOWN mode */
  pm_sbl_usb_otg_shutdown(TRUE);

  /* Disable VREG_USB */
  pm_sbl_enable_vreg_usb(FALSE);

  /* Switch off the keypad light and LED */
  osbl_usb_chg_set_led( FALSE ) ;
 }
#endif /* Not used for KIP and HAN charging code */
 }
void HWIO_OUT_PTR(unsigned int addr,unsigned int val)
{
    unsigned int *ptr;
    ptr=(unsigned int *)addr;
    *ptr=val;
     
}
void osbl_usb_chg_set_clock_mode(clkrgm_usb_chg_mode_type clk_regime)
/*void osbl_usb_chg_set_clock_mode(int Clk)*/
{
 if(clk_regime==LowSpeed)
{
        

    HWIO_OUT(ARM_MOD_REG,0x01);
    osbl_delay_in_ms ( 1 ) ;
    HWIO_OUT(ARM_MOD_REG,0x02);
    osbl_delay_in_ms ( 1 ) ;
    HWIO_OUT(ARM_MOD_REG,0x03);

    osbl_delay_in_ms ( 1 ) ;
    HWIO_OUT(ARM_MOD_REG,0x04);
    osbl_delay_in_ms ( 1 ) ;
    HWIO_OUT(ARM_CFG_REG,0x0);                                           
    osbl_delay_in_ms ( 1 ) ;
    osbl_ahb_chg_freq=OSBL_USB_CHG_LOW_POWER_MODE_FREQ;
}
else
{


    HWIO_OUT(ARM_CFG_REG,0x0DB4);                                         
    osbl_delay_in_ms ( 1 ) ;

    HWIO_OUT(ARM_MOD_REG,0x03);
    osbl_delay_in_ms ( 1 ) ;
    HWIO_OUT(ARM_MOD_REG,0x02);
    osbl_delay_in_ms ( 1 ) ;
    HWIO_OUT(ARM_MOD_REG,0x01);

    osbl_delay_in_ms ( 1 ) ;
    HWIO_OUT(ARM_MOD_REG,0x00);
    osbl_delay_in_ms ( 1 ) ;
    osbl_ahb_chg_freq=OSBL_USB_CHG_NORMAL_MODE_FREQ;

}
/*
if(Clk==0)
{
        HWIO_OUT_PTR( 0x80000000,0x1);
        osbl_delay_in_ms ( 1 ) ;
        HWIO_OUT_PTR( 0x80000000,0x2);
        osbl_delay_in_ms ( 1 ) ;
        HWIO_OUT_PTR( 0x80000000,0x3);
        osbl_delay_in_ms ( 1 ) ;
HWIO_OUT_PTR( 0x80000000,0x4);
osbl_delay_in_ms ( 1 ) ;
HWIO_OUT_PTR( 0x80000004,0x0);
}
else
{

HWIO_OUT_PTR( 0x80000004,0xDB4);
osbl_delay_in_ms ( 1 ) ;
HWIO_OUT_PTR( 0x80000000,0x3);
osbl_delay_in_ms ( 1 ) ;
HWIO_OUT_PTR( 0x80000000,0x2);
osbl_delay_in_ms ( 1 ) ;
HWIO_OUT_PTR( 0x80000000,0x1);
osbl_delay_in_ms ( 1 ) ;
HWIO_OUT_PTR( 0x80000000,0x0);
osbl_delay_in_ms ( 1 ) ;
}
*/

}
/*===========================================================================

FUNCTION OSBL_USB_CHG_INIT

DESCRIPTION
  Initializes GPIO, SSBI and PMIC in OSBL

SIDE EFFECTS
  PMIC, SBI are initialized.

===========================================================================*/
void osbl_usb_chg_init(bl_shared_data_type *bl_shared_data )
{

    /* Save the normal mode frequency at the beginning */
  osbl_chg_normal_mode_freq = osbl_bus_speed_in_mhz;
    /* Core charging algorithm */

  switch ( pm_ft_get_pmic_model() ) 
    {
        case PMIC_IS_KIP:
        case PMIC_IS_HAN:
        {
            // turn on MSMA in order for battery temp monitoring to work.
            pm_vreg_control(PM_ON_CMD, PM_VREG_MSMA_M);

            /* turn on VREG Boost regulator to provide 5V to USB_VBUS */
            pm_init_otg_usb();

            osbl_pm_auto_low_batt_chg () ;
        }
        break ;

        default:
        {
            osbl_low_batt_chg() ;
        }
    }


} /* osbl_usb_chg_init( void ) */

