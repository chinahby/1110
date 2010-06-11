/*=============================================================================

               O E M  S e c o n d a r y  B o o t  L o a d e r
                       USB Battery Charging Handler 

GENERAL DESCRIPTION
    This file contains the functions that will handle the scenario when the
phone boots from USB port and finds that battery is too low to assist the
boot process. The phone will attempt to charge the battery first before
continuing to boot.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/osbl/osbl_usb_auto_chg_init.c#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/15/09   dk      Removing compiler warnings.
03/13/09   HS      Added a parameter in pm_chg_failed_clear() since the prototype
                   of this API has been changed.
02/20/09   tnk     Added support for Usb detection using Vchg.
20/02/09   tnk     Removed lowering the clock in FLCB as it was not making much impact in power consumption
                   as PLL was not powered down.Added D- pull down code at the endof FLCB code
14/02/09   TNK     Changed WeakBatt Threshold from 3.5 to 3.2 and taken care of notenumerated case
                   with disconnect after 10secs and then charging.
12/11/08   HS      Modified the setup for autonomous charging.
                   Added charging support for USB suspended and USB enumeration
                   failed cases.
11/13/08   APU     Added the hsu_boot_stop_stack() for normal battery case.
11/12/08   APU     Removed the USB LDO switch on call.
11/11/08   APU/ETN Added funtion headers.
11/04/08   APU     All the known test cases are working.
10/31/08   APU     Basic Charging is working. Will verify the user cases now.
10/29/08   APU     Created.
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "osbl_usb_auto_chg_init.h"
#if defined(FEATURE_OSBL_CHG_SCREEN)
extern void disp_init(void);
extern void disp_epson_S1D19120_chg_fail_splash_screen (void);
extern void disp_epson_S1D19120_boot_chg_splash_screen (void);
extern void disp_off(void);
#endif

extern void    qhsusb_dci_disconnect(uint32 core_id);

volatile uint8 osbl_boot_debug = 0 ;
uint32 temp_delay_flag;/* for debug purpose to be removed*/

static boolean osbl_usb_device_valid(void);
/*=============================================================================

FUNCTION osbl_pm_get_batt_state 

DESCRIPTION
  Depending on the battery voltage, the battery type, i.e. dead or weak or
  wrong battery are set and returned.

NOTE
  Either a battery level is set or checked, not both.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
=============================================================================*/
osbl_batt_types_for_boot 
osbl_pm_get_batt_state ( pm_err_flag_type *err ,boolean enum_status)
{
    static unsigned int lo_mv = 0 ; 
    static unsigned int hi_mv = 0 ;
    static osbl_batt_types_for_boot batt_lev = DEAD_BATTERY ;

    *err = PM_ERR_FLAG__SUCCESS ;

    switch ( batt_lev ) 
    {
        case DEAD_BATTERY:
            if ( ( lo_mv != OSBL_USB_CHG_BAT_WIN_LWR_THRESH ) || 
                 ( hi_mv != OSBL_USB_CHG_BAT_WIN_MID_THRESH ) )
            {
                lo_mv = OSBL_USB_CHG_BAT_WIN_LWR_THRESH ;
                hi_mv = OSBL_USB_CHG_BAT_WIN_MID_THRESH ;  
                pm_sbl_set_batt_alarm_threshold ( lo_mv , hi_mv ) ; 

                /* *err = pm_chg_imaxsel_set( OBSL_AUTO_CHG_IMAXSEL ) ; */
            }
            else 
            {
                if (pm_sbl_read_bat_thresh() == 1 ) 
                {
                    batt_lev = NORMAL_BATTERY ;
   
                 }
            }
            break ;

        case WEAK_BATTERY:
            if ( ( lo_mv != OSBL_USB_CHG_BAT_WIN_MID_THRESH ) || 
                 ( hi_mv != OSBL_USB_CHG_BAT_WIN_UPR_THRESH ) )
            {
                    lo_mv = OSBL_USB_CHG_BAT_WIN_MID_THRESH ;
                    hi_mv = OSBL_USB_CHG_BAT_WIN_UPR_THRESH ;  
                    pm_sbl_set_batt_alarm_threshold ( lo_mv , hi_mv ) ;

                    /* *err = pm_chg_imaxsel_set( OBSL_AUTO_CHG_IMAXSEL ) ; */
            }
            else 
            {
                if (pm_sbl_read_bat_thresh() == 1 ) 
               {
                    batt_lev = NORMAL_BATTERY ;
               }
            }
            break ;

        case NORMAL_BATTERY:
        default:
            break ;
    }

    return batt_lev ;
}

void osbl_pm_set_batt_state ( pm_err_flag_type *err )
{
    static unsigned int lo_mv = 0 ; 
    static unsigned int hi_mv = 0 ;
              
    lo_mv = OSBL_USB_CHG_BAT_WIN_MID_THRESH ;
    hi_mv = OSBL_USB_CHG_BAT_WIN_UPR_THRESH ;  
    pm_sbl_set_batt_alarm_threshold ( lo_mv , hi_mv ) ;

    /* *err = pm_chg_imaxsel_set( OBSL_AUTO_CHG_IMAXSEL ) ; */
    
}

/*=============================================================================

FUNCTION osbl_pm_auto_low_batt_chg

DESCRIPTION
  Core algorithm for autonomous charging. This function implements the core
  charging algorithm after configuring the PMIC. If wall-charger is detected 
  the phone boots else if PC USB is detected does the dead/fast USB charging
  depending upon the battery voltage.


DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
=============================================================================*/

void
osbl_pm_auto_low_batt_chg( void )
{
    /*
    1. Check if the voltage is good enough for booting. Do this by using PMIC 
       voltage comparator. If the voltage is good wnough for booting, boot up
       and skip all the other steps.
    2. If not, start the USB boot stack.
    3. Setup the autonomous charging.  
    4. Call osbl_pm_charging_FSM.
    5. Boot or power down.
    */
    boolean is_usb_wall_chg = 0;

    pm_err_flag_type pm_crit_err = PM_ERR_FLAG__SUCCESS ; 
    boolean hsu_stack_start_condition = FALSE ;

    /* This variable will let us know if the battery voltage is outside or 
       inside the voltage window.
       0x02 is nvalid battery threshold
    */
    int batt_threshold = 0x02 ; 

    /* Set the battery alarm upper and lower threshold
     * BAT_UPR_THRESH = 3600 mV = threshold for detection of battery absence
     * BAT_LWR_THRESH = 3200 mV = threshold for stopping charging
     */
    pm_sbl_set_batt_alarm_threshold ( OSBL_USB_CHG_BAT_WIN_LWR_THRESH ,
                                      OSBL_USB_CHG_BAT_WIN_MID_THRESH ) ; 

    /* apu : set the hysteresis to max value here */
    /* Enable battery alarm comparators */
    pm_sbl_enable_batt_alarm ( TRUE ) ;

    /* Check the return value. pm_sbl_enable_vreg_usb(TRUE)   
       Enable VREG_USB */
    //pm_crit_err |= pm_vreg_control ( PM_ON_CMD , PM_VREG_USB3P3_M ) ;

    is_usb_wall_chg = pm_sbl_is_dm_high();

    /* Start the HSUSB stack */
    //pm_init_otg_usb();
    hsu_stack_start_condition = hsu_boot_start_stack() ;   

    /* Delay for the VBATT comparator to settle down */
    osbl_delay_in_ms ( 20 ) ;

    /*  Two configurations are supported:
    *  1. Phone that only supports DC wall charger 
    *  2. Phone that only supports PC USB or USB charger 
    */ 

    /* If it's not USB, then it's DC wall, continue to boot. OR 
       If battery has enough juice, continue to boot.
       OTHERWISE 
       Execute the following code inside the IF block */

    batt_threshold = pm_sbl_read_bat_thresh () ;
    if (  /* Check if battery is low                     */
          (  batt_threshold < 1        )     &&
          /* Check if PMIC transaction has error        */
          ( ! pm_crit_err              )     &&
          /* Check if the HSU Stack started successfully */
          ( hsu_stack_start_condition  )     &&
          /* Check if the USB Wall Charger is connected  */
          ( !is_usb_wall_chg     )     && 
          /* Check if the USB is connected               */
          /*( pm_sbl_usb_sess_is_valid() )  )*/ 
          ( osbl_usb_device_valid() )   )
    {
        /* */

        /* boot_loader turns on both the keypad LED and LCD backlight
         * Turn it off to save current */
        pm_set_led_intensity( PM_LCD_LED , 0 ) ;
        pm_set_led_intensity( PM_KBD_LED , 0 ) ;


        /* Check for previous errors */
        if ( pm_crit_err )
        {
            /* If charger-setup or charging is successful, booting will continue 
            otherwise, because of lack of voltage, the phone will power down 
            and will never return back.
            */
            osbl_power_down () ;
        }
        else
        {
            /* Enable the autonomous charging with the charging current limit set to 
            100mA(slow charge) in order to comply with the USB spec.*/
            osbl_shp_autocharging_setup (OBSL_AUTO_CHG_IMAXSEL_MIN, 
                                         OBSL_AUTO_CHG_TRICKLE_LOW_CURRENT) ;

         /* Removed the clock lowering down code  as there are multiple clock plans and 
          there was not much power saving due to clock switch as PLL has to be on */

           osbl_pm_charging_FSM () ;
    
        }
    } 

    /* PD if not a wall chg and no juice */
    //if ( ! (pm_sbl_usb_sess_is_valid()) )
    if( !(osbl_usb_device_valid()) )
    {
        if ( batt_threshold < 1 )     
        {
            osbl_power_down () ;
        }
    }
    else 
    {     
        /* Check if the USB WCHG is connected, in that case make sure high 
               current does not damage the weak battery. */
        if ( is_usb_wall_chg ) 
        {
            osbl_pm_set_batt_state (NULL);
        }
    }
    /* At this point, we will continue with the booting. If so stop the HS USB
       stack so USB does not get enumerated as a HID device */
        hsu_boot_stop_stack () ;

        /* Pull down D- fist */
        qhsusb_dci_disconnect(0);

        #ifdef FEATURE_PMIC_USB_DETECTION_USING_VCHG
        /* Then turning off Boost Regulator */
        pm_vreg_control(PM_OFF_CMD, PM_VREG_BOOST_M);
        #endif /* endif FEATURE_PMIC_USB_DETECTION_USING_VCHG */
#if defined(FEATURE_OSBL_CHG_SCREEN)
        disp_off();
#endif
}

/*===========================================================================

FUNCTION osbl_shp_autocharging_setup

DESCRIPTION
  Sets the PIMC in Auto charging mode.

DEPENDENCIES
  Should only be called from and within boot-code.
  
PARAMETER
  Trickle charger current

RETURN VALUE
  Error flag

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
===========================================================================*/

pm_err_flag_type 
osbl_shp_autocharging_setup ( uint32 imaxsel, uint32 trickle_current_setting )
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS ;

    osbl_boot_debug ++ ;

    /* Set Imaxsel first */
    err |= pm_chg_imaxsel_set( imaxsel ) ; 

    /* Set VMAXSEL */
    err |= pm_chg_vmaxsel_set( OBSL_AUTO_CHG_VMAXSEL ) ; 

    /* Set VBATDET */
    err |= pm_chg_vbatdet_set(OBSL_AUTO_CHG_VBATDET);

    /* Set trickle charging current */
    err |= pm_chg_itrkl_set(trickle_current_setting);

    /* Set end-of-charging current*/
    err |= pm_chg_iterm_set(OSBL_AUTO_CHG_ITERM);

    /* Set maximum time for automatic fast charging */
    err |= pm_chg_tchg_set(OSBL_AUTO_CHG_T);

    /* Exit charging-fail state */
    err |= pm_chg_failed_clear(TRUE);

    /* Enable battery temperature monitoring */
#ifdef CUST_EDITION
    /* Enable battery temperature monitoring */
#if defined(CHG_BATT_TEMP_MONITOR_ENABLE)
			/* Enable battery temperature monitoring */
	err |= pm_chg_batt_temp_disable(FALSE);
#else
	err |= pm_chg_batt_temp_disable(TRUE);
#endif

#else
	err |= pm_chg_batt_temp_disable(FALSE);
#endif
    /* Enable automatic charging */
    err |= pm_chg_auto_disable ( FALSE ) ;

    /* Enable battery charging/powering PMIC from USB device */
    err |= pm_chg_enum_done_enable ( TRUE ) ;

    /* Unsuspend the USB for PC */
    //err |= pm_chg_usb_suspend ( FALSE ) ;

   return err ;
}

pm_err_flag_type
osbl_usb_chg_suspend ( void )
{
    pm_err_flag_type err = PM_ERR_FLAG__SUCCESS ;

    /* apu: check this Limit the current and disable auto-chg */
    err = pm_chg_auto_disable( TRUE ) ;
    //err |= pm_chg_usb_suspend ( TRUE ) ;

   return err ;
}

boolean osbl_usb_device_valid(void)
{
    #ifdef FEATURE_PMIC_USB_DETECTION_USING_VCHG
    return !(pm_sbl_ext_chg_is_invalid());
    #else
    return pm_sbl_usb_sess_is_valid();
    #endif
}
/*===========================================================================

FUNCTION osbl_pm_get_chg_state

DESCRIPTION
  Gets the battery status and sets the charging status flags based on which 
  charging or boot-up is done.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
===========================================================================*/


pm_err_flag_type 
osbl_pm_get_chg_state ( osbl_pm_chg_batt_fsm_var_type 
                        *osbl_pm_batt_chg_state  )
{
    /* Update current state */
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Register reads are byte reads */
    pm_auto_chg_state_name_type charging_reg_state = CHG_AUTO__INVALID_ST ;

    /* Maps the registers to the enum we have */
    osbl_pm_chg_auto_charging_state_name_type chg_state ;

    /* Make sure to check for the errors */
    err = pm_chg_charge_state_get( &charging_reg_state ) ;

    chg_state = (osbl_pm_chg_auto_charging_state_name_type) 
                 charging_reg_state ;

    /* Update UI status */
    switch( chg_state )
    {
        /* OFF state, all outputs are set to 0 (INITIAL STATE) */
        case OSBL_PM_CHG_AUTO__INIT_ST: /* state 0 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__IDLE ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = FALSE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ;
        }
        break;

        /* Power on from the charger (turns on charger regulator) */
        case OSBL_PM_CHG_AUTO__PWR_ON_CHGR_ST: /* state 1 */ 
        {
            /* TBD: Was OSBL_PM_CHG_UI_EVENT__NORMAL_POWER */
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__VERY_LOW_POWER ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = FALSE ;
       }
        break;

        /* Auto trickle charging */
        case OSBL_PM_CHG_AUTO__AUTO_TRICKLE_ST: /* state 2 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__VERY_LOW_POWER ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = TRUE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ;
        }
        break;

        /* Power on from the battery (closes battery FET) */
        case OSBL_PM_CHG_AUTO__PWR_ON_BATT_ST: /* state 3 */
        {
            /* Assume this to be a low power state and we can adjust later */
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__VERY_LOW_POWER ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = FALSE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ;

        }
        break;

        /* Auto trickle charging has failed (flash LED at a slow frequency) */
        case OSBL_PM_CHG_AUTO__AUTO_TRICKLE_FAILED_ST: /* state 4 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__VERY_LOW_POWER ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = FALSE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = FALSE ; 

        }
        break;

        /* Transition state, gives time to analog circuitry to settle.*/
        case OSBL_PM_CHG_AUTO__TRANS_ST: /* state 5 */
        {
           //no need to update the status in this state;
           //since it is a transition state.
        }
        break;

        /* Power on from battery and charger with a current limit of ~90mA 
           after auto trickle charging */
        case OSBL_PM_CHG_AUTO__POST_AUTO_TRICKLE_ST: /* state 6 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__LOW_POWER ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = TRUE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ; 

        }
        break;

        /* Fast charging */
        case OSBL_PM_CHG_AUTO__FAST_ST: /* state 7 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__NORMAL_POWER ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = TRUE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ; 

        }
        break;

        /* Trickle charging */
        case OSBL_PM_CHG_AUTO__TRICKLE_ST: /* state 8 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
            OSBL_PM_CHG_UI_EVENT__VERY_LOW_POWER ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = TRUE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ;

        }
        break;

        /* Charging failed */
        case OSBL_PM_CHG_AUTO__FAILED_ST:/* state 9 */
        {
            // no need to update the charging status here
            // since it will be updated in EventFired() based on
            // the counter.
        }
        break;

        /* End of charge */
        case OSBL_PM_CHG_AUTO__DONE_ST: /* state 10 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
                OSBL_PM_CHG_UI_EVENT__DONE ;
            osbl_pm_batt_chg_state->osbl_pm_is_charging = TRUE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ;

        }
        break;

        /* Brings up VDD with ~90mA maximum current drawn from VBUS */
        case OSBL_PM_CHG_AUTO__LOW_ILIM_ST: /* state 11 */
        {
            //no need to update the status in this state;
            //since it is a transition state.
        }
        break;

        /* USB suspend mode */
        case OSBL_PM_CHG_AUTO__USB_SUSP_ST: /* state 12 */
        {
            osbl_pm_batt_chg_state->osbl_pm_batt_status = 
            OSBL_PM_CHG_UI_EVENT__NORMAL_POWER ; 
            osbl_pm_batt_chg_state->osbl_pm_is_charging = FALSE ;
            osbl_pm_batt_chg_state->osbl_pm_is_batt_valid = TRUE ;

        }
        break;

        default:
        {

        }
        break;
    } /* end switch() */

    return err ;
}



/*===========================================================================

FUNCTION osbl_pm_get_usb_status

DESCRIPTION
  Polls the USB status gets various USB stimuli.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
===========================================================================*/



osbl_usb_stimuli_type 
osbl_pm_get_usb_status ( uint32 *retry_count , boolean *new_usb_event, boolean *enum_complete )
{
    osbl_usb_stimuli_type usb_stimulus = INIT_STATE ; 
    /* Var to hold the USB status */
    qhsusb_boot_poll_result poll_result = hsu_boot_poll_stack() ;

    *new_usb_event = TRUE ;
    switch ( poll_result )
    {
        /* No change in USB state resulted by polling */
        case HSU_BOOT_POLL_RESULT_NO_NEW_EVENT:
        {
            /* We store the last USB state by not doing the following: */
            /* usb_stimulus = MAINTAIN_PREV_STATE_N_WAIT ; */

            /* Signal that something happened */
            *new_usb_event = FALSE ;
        }
        break ;


    
        /* Device is enumerated successfully */
        case HSU_BOOT_POLL_RESULT_ENUMERATED:             
        {
            /* Set up the autonomous charging */
            usb_stimulus = READY_FOR_AUTO_CHG ;
            osbl_shp_autocharging_setup (OBSL_AUTO_CHG_IMAXSEL,
                                         OBSL_AUTO_CHG_TRICKLE_HIGH_CURRENT) ;
            *retry_count = 0 ;
            *enum_complete = TRUE;
        }
        break ;


        /* USB cable disconnected. Also happens prior to Enumeration so we 
           have to retry */
        case HSU_BOOT_POLL_RESULT_DISCONNECTED:           
        /* USB cable is not connected */
        case HSU_BOOT_POLL_RESULT_NOT_CONNECTED: 
        case HSU_BOOT_POLL_RESULT_ERROR:
        {
            usb_stimulus = FAILURE_DO_POWER_DOWN ; 
        }
        break ;

        /* USB cable connected to wall charger */
        case HSU_BOOT_POLL_RESULT_CONNECTED_TO_WALL_CHG:  
        {
            usb_stimulus = READY_TO_BOOT ;
        }
        break ;

        /* Device enumerated succesfully, but the device is connected to a bus-powered hub */
        /* In this case, slow charge the battery */
    case HSU_BOOT_POLL_RESULT_ENUMERATED_100MA:
        {
            /* if enumeration failed, slow charge the battery */
            usb_stimulus = READY_FOR_AUTO_CHG ;
            osbl_shp_autocharging_setup (OBSL_AUTO_CHG_IMAXSEL_MIN,
                                         OBSL_AUTO_CHG_TRICKLE_LOW_CURRENT) ;
            *retry_count = 0 ;
            *enum_complete = TRUE;
        }
        break;
        /* Device is resumed, but not enumerated yet. When resuming 
         *after* enumeration, the ENUMERATED event will be raised */
        /* This case can also be used for enumeration failed */
        case HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED: 
        {
            /* if enumeration failed, slow charge the battery */
            usb_stimulus = READY_FOR_AUTO_CHG ;
            osbl_shp_autocharging_setup (OBSL_AUTO_CHG_IMAXSEL_MIN,
                                         OBSL_AUTO_CHG_TRICKLE_LOW_CURRENT) ;
            *retry_count = 0 ;
        }
        break;
        /* USB cable connected to a PC */
        case HSU_BOOT_POLL_RESULT_CONNECTED_TO_HOST_PC:   
        {
            usb_stimulus = READY_FOR_TRICKLE_CHARGING ;
        }
        break ;

        /* Device is suspended */
        case HSU_BOOT_POLL_RESULT_SUSPENDED:              
        {
            /* Do not do anything on the first pass as it can be a 
               "disconnect" event. */

            if ( hsu_boot_poll_stack() == HSU_BOOT_POLL_RESULT_NO_NEW_EVENT ) 
            {   
                /* in SUSPENDED mode, slow charge the battery */
                osbl_shp_autocharging_setup (OBSL_AUTO_CHG_IMAXSEL_MIN,
                                             OBSL_AUTO_CHG_TRICKLE_LOW_CURRENT) ;
#if defined(FEATURE_OSBL_CHG_SCREEN)
                disp_off();
#endif
                usb_stimulus = RETRY_MULTIPLE_TIMES ;
            }

          }
        break ;

        case HSU_BOOT_POLL_RESULT_MAX:
        default:
        {   
            usb_stimulus = RETRY_ONCE ;
        }
    }

    return usb_stimulus ;
}


/*===========================================================================

FUNCTION osbl_pm_charging_FSM

DESCRIPTION
 This is a SW FSM that works on USB status and the battery status. This 
 function only returns when we have battery is charged to have enough 
 juice or a wall charger connected to it or the phone powers down or 
 timeout happens.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
===========================================================================*/

void 
osbl_pm_charging_FSM ( void )
{
    /* Flag to signal if power down is required */
    boolean power_down = FALSE ;

    /* Flag to signal if we are ready for boot */
    boolean ready_for_boot = FALSE ;

    /* Master delay counter for this FSM */
    uint32 delay = 0 ;

    /* Var to provide USB stimuli to the FSB */
    osbl_usb_stimuli_type usb_stimulus = INIT_STATE ; 

    /* Keeps counts of the retries */
    uint32 retry_count = 9 ;

    /* For Toggling the LED */
    boolean led_state = FALSE ;

    /* Structure that carries the batt and chg states */

    /* To keep track of any new USB event */
    boolean new_usb_event = FALSE ;

    /* To keep track of the status of the enumeration */
    boolean enumeration_complete = FALSE;

    /* FSM main loop, guranteed to execute once */
    do
    {
        /* Decrement the Retry count */
        retry_count -- ;

        /* USB will need to be polled atleast once less then every 10 mS */
         osbl_delay_in_ms (OSBL_POLL_DELAY) ;

        /* Toggle LED to display charging every second or so */
        if ( ! ( delay % 1000 ) ) 
        {
            /* Toggle LED here */
            led_state = (led_state == TRUE) ? FALSE : TRUE ;
            osbl_usb_chg_set_led ( led_state ) ;
        }


        /* Update the USB state variables every 10 mS or so */
        usb_stimulus = osbl_pm_get_usb_status ( &retry_count ,
                                                &new_usb_event,
                                                &enumeration_complete ) ;

        
        /* Then check if battery has enough juice */
        if ( usb_stimulus == FAILURE_DO_POWER_DOWN )
        {
           power_down = TRUE ;
        }
        else if ( usb_stimulus == READY_TO_BOOT )
        {
            ready_for_boot = TRUE ;
            break ;
        }
        else if ( usb_stimulus == READY_FOR_TRICKLE_CHARGING )
        {
            /* Setup trickle charging */
        }
        //else if  (!pm_sbl_usb_sess_is_valid())
        else if ( !(osbl_usb_device_valid()) )
        {
            power_down=TRUE;
        }

        /* Check battery status every 5 seconds or so */
        if ( ! ( delay % (OSBL_POLL_DELAY*100) ) ) 
        {
            pm_err_flag_type pm_err = PM_ERR_FLAG__SUCCESS ;  

            /* Update the CHG state variables */
            osbl_batt_types_for_boot batt_stat =
            osbl_pm_get_batt_state ( &pm_err ,enumeration_complete) ;

            /* If battery is not good or PMIC error, time to quit.    
             ( ! osbl_pm_batt_chg_state.osbl_pm_is_batt_valid ) ||  */
            if ( pm_err )          
            { 
               /* PD */
               power_down = TRUE ;
            }
            else /* Make a decision on the basis of USB and CHG FSM */
            {  
                switch ( batt_stat )
                {

                    /* Enough Power for most applications.
                       Done charging, batt full.          
                       Boot up */
                    case NORMAL_BATTERY:           
                    {
                        ready_for_boot = TRUE ;
                    }
                    break ;

                    default:
                    {
                    }
                }
            }
        }

        /* If after 10 sec that the device was connected 
        there was no enumeration yet and the device still 
        have low battery and still want to get 100ma then 
        it should disconnect.*/
        if((delay == OSBL_TCON_ISUSP) && 
           (enumeration_complete == FALSE))
        {
            /* Disconnect from the bus */
            qhsusb_dci_disconnect(0);

        }
                                                /* Leave this FSM when: */
    } while (    ( delay ++ < TIME_OUT_PERIOD )  /* Waited long enough */ 
              && ( ! power_down )                /* OR Power down condition */
              && ( ! ready_for_boot ) ) ;        /* OR when Ready to Boot */


    if ( power_down || ( delay >= TIME_OUT_PERIOD ) )
    {
        /* Switch of the LED to save current */
        led_state = FALSE ;
        osbl_usb_chg_set_led ( led_state ) ;

        hsu_boot_stop_stack () ;
#if defined(FEATURE_OSBL_CHG_SCREEN)
        disp_init();
        disp_epson_S1D19120_chg_fail_splash_screen ();
        osbl_delay_in_ms (1000) ;
        
#endif
        osbl_power_down () ;
    }

    /*  If we are at this point, the battery has enough juice and we will continue booting */
}
