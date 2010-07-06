/*! \file 
 *  \n
 *  \brief  chg_autocharging.cpp ---- AUTO CHARGING RELATED DEFINITION
 *  \n
 *  \n This header file contains class declarations of the Battery class
 *  and the BatteryDebug class.
 *  \n
 *  \n &copy; Copyright 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_autocharging.cpp#26 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   hs      Fixed LINT errors.
                   Code cleanup.
09/17/09   hs      Added support for general charging status.
                   Added support for charger event notification through VBATT task interface.
                   Treated the USB valid/invalid events as system events.
08/14/09   hs      Revised end-of-fast-charging detection algorithm
08/05/09   hs      Added support for bad battery.
                   Revised end-of-fast-charging detection algorithm
07/30/09   hs      Added support for providing general charging status.
                   Added support for using vbatt task interface to get the 
                   battery temperature.
07/27/09   hs      Added support for using vbatt task interface to get the 
                   battery voltage.
06/15/09   hs      Added support for charging after battery removed/re-inserted.
06/05/09   hs      Added support for software controlled CV topoff.
05/29/09   hs      Created two child classes to support pulse charging.
05/26/09   hs      Added support for charging during call.
05/05/09   hs      Improved the algorithm of end-of-charge detection.
04/23/09   hs      Added support for the resume-charging detection.
04/03/09   hs      Added support for unit tests.
03/27/09   hs      Fixed some typo.
03/24/09   hs      Added register cleanup routine.
03/11/09   hs      Added support for end-of-charging workaround.   
                   Revised the autonomous charging setup.
02/09/09   hs      Revised the pulse charging routine.
01/27/09   hs      Code clean up.
                   Revised the way that charger UI status are updated.
12/11/08   hs      Disabled charger suspend mode during USB suspend. 
                   Before starting the autonomous charging, stop it 
                   first, to reset the hardware timer.
                   Replaced chg_auto_charging_state_name_type with 
                   pm_auto_chg_state_name_type.
12/04/08   hs      Revised the software workaround.
12/01/08   hs      Revised the way of handling the autonomous charging related
                   PMIC interrupts.
11/25/08   hs      Revised the end-of-chargign algorithm.
11/18/08   hs      Revised the end-of-charging algorithm.
11/13/08   hs      Added support for QSC1100.
10/04/08   vk      Added support for USB suspend state
09/29/08   vk      Added battery removal fix
09/05/08   vk      Modified code to use UCP charging path for wall USB charger
06/30/08   hs      Added FEATURE_CHG_TASK.
04/11/08   hs      Created.

========================================================================== */

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"          /* FEATURE definitions                     */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)
#ifdef T_WINNT
#error code not present
#else 
#include "assert.h"            /* For ASSERT()                             */
#endif /* T_WINNT */
#include "chg_autocharging.h"
#include "chg_isr.h"           /* For class IrqEventGenerator              */
#include "chg_timer.h"         /* For TimerEventGenerator                  */
#include "chg_system.h"        /* For class SystemEventGenerator           */
#include "chg_factory.h"       /* For class ChargingFactory                */
#include "chg_config.h"        /* For CHG_FAIL_OF_CHARGING_COUNTER_MAX     */
#include "chg_debug.h"         /* For class AutoChargingDebugEvent         */
extern "C"
{
#include "clk.h"               /* For clk_busy_wait()                      */
#ifdef CUST_EDITION
void oembatt_notify_chg_event(void);
#endif

}

                      /*************************************/
                      /*     class AutoChargingManager     */
                      /*************************************/
AutoChargingManager::AutoChargingManager():
    mCurrentState(CHG_AUTO__INIT_ST),
    mPreviousState(CHG_AUTO__INIT_ST),
    mStateChangeIrqTriggered(false),
    mChargingDoneIrqTriggered(false),
    mChargingFailIrqTriggered(false),
    mVbatdetIrqTriggered(false),
    mBattTemptIrqTriggered(false),
    mBattConnectIrqTriggered(false),
    mIsInFinalCycle(false),
    mIsChargingComplete(false),
    mIsChargingFailed(false),
    mLowVbattCounter(0)

{
    /* Initialize the general charging status */
    mGeneralStatus.is_charger_valid = FALSE;
    mGeneralStatus.is_charging = FALSE;
    mGeneralStatus.is_battery_valid = TRUE;
    mGeneralStatus.ui_event = CHG_UI_EVENT__IDLE;
    mGeneralStatus.charger_status = CHARGER_STATUS__INVALID;
    mGeneralStatus.charger_hardware_type = CHARGER_TYPE__INVALID;
    mGeneralStatus.battery_status = BATTERY_STATUS__INVALID;
    mGeneralStatus.battery_level = BATTERY_LEVEL__INVALID;
    mGeneralStatus.battery_voltage = 0;
    mGeneralStatus.battery_temp = 0;

    /* Add itself as a listener for system status change: USB device has been detected by the USB 
       driver and it has been successfully enumerated. */
    SystemEventGenerator *systemEventGenerator = ChargingFactory::GetSystemEventGenerator();
    systemEventGenerator->AddListenerForAutoChargingSwitch(this);
    systemEventGenerator->AddListenerForUsbValid(this);
    systemEventGenerator->AddListenerForUsbInvalid(this);
    systemEventGenerator->AddListenerForUsbSuspend(this);
    systemEventGenerator->AddListenerForTxWaitVote(this);

    /* Always listen for Valid/Invalid charger IRQs */
    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    irqEventGenerator->AddListenerForValidChargerIRQ(this);
    irqEventGenerator->AddListenerForInvalidChargerIRQ(this);
    (void)irqEventGenerator->AddListenerForChangeOfStateIrq(this);

    /* Clean up charger related PMIC registers*/
    (void)pm_chg_cleanup();
}

AutoChargingManager::~AutoChargingManager()
{
        /* Clean up charger related PMIC registers*/
        (void)pm_chg_cleanup();
}

void AutoChargingManager::startShpAutoCharging()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Stop Autonomojs charging first, to reset the hardware timers */
    err |= pm_chg_auto_disable(TRUE);

    /* Set VMAXSEL */
    err |= pm_chg_vmaxsel_set(AUTO_CHARGING_VMAXSEL); 

    /* Set VBATDET */
    err |= pm_chg_vbatdet_set(AUTO_CHARGING_VBATDET);

    /* Reset the fail-of-charging state clear bit */
    err |= pm_chg_failed_clear(FALSE);

    /* Set the end-of-charging current */
    err |= pm_chg_iterm_set(AUTO_CHARGING_ITERM);

    /* Set charging timeout to the maximum value */
    err |= pm_chg_tchg_set(512);

    /* Set trickle charging timeout */
    err |= pm_chg_ttrkl_set(AUTO_CHARGING_TRICKLE_T);

    #ifdef FEATURE_BATTERY_CHARGER_A0_WORKAROUND
#error code not present
    #endif

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

    /* Enable battery charging/powering PMIC from USB device */
    err |= pm_chg_enum_done_enable(TRUE);

    /* Wait for 20ms */
    clk_busy_wait(20000);

    /* Enable automatic charging */
    err |= pm_chg_auto_disable(FALSE);

    /* Add itself as a listener for PMIC IRQs. */
    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    err |= irqEventGenerator->AddListenerForEndOfChargingIrq(this);
    err |= irqEventGenerator->AddListenerForFailOfChargingIrq(this);
    err |= irqEventGenerator->AddListenerForFastChargeIrq(this);
    err |= irqEventGenerator->AddListenerForBattConnectIrq(this);
    err |= irqEventGenerator->AddListenerForVbatdetIrq(this);

    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
    /* Add itself as a listerner for charging heart beat */
    timerEventGenerator->AddListenerForChargingHeartBeat(this);
    /* Add itself as a listerner for max-charging-time detection */
    timerEventGenerator->AddListenerForMaxChargeTimeDetection(this);

    /* Update charger valid status */
    mGeneralStatus.is_charger_valid = TRUE;

    /* Update charger type */
    mGeneralStatus.charger_hardware_type = CHARGER_TYPE__USB_PC;

    /* Update charger status */
    mGeneralStatus.charger_status = CHARGER_STATUS__GOOD;

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
}

void AutoChargingManager::startUcpAutoCharging()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Stop Autonomojs charging first, to reset the hardware timers */
    err |= pm_chg_auto_disable(TRUE);

    /* Set Imaxsel first */
    if(TRUE == chg_system_tx_informaton.tx_on)/* System is in a call*/
    {
        err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_TX_ON); 
    }
    else /* System is not in a call */
    {
        err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_MAXIMUM); 
    }

    /* Set VMAXSEL */
    err |= pm_chg_vmaxsel_set(AUTO_CHARGING_VMAXSEL); 

    /* Set VBATDET */
    err |= pm_chg_vbatdet_set(AUTO_CHARGING_VBATDET);

    /* Reset the fail-of-charging state clear bit */
    err |= pm_chg_failed_clear(FALSE);

    /* Set the end-of-charging current */
    err |= pm_chg_iterm_set(AUTO_CHARGING_ITERM);

    /* Set charging timeout to the maximum value */
    err |= pm_chg_tchg_set(512);

    /* Set trickle charging timeout */
    err |= pm_chg_ttrkl_set(AUTO_CHARGING_TRICKLE_T);

    #ifdef FEATURE_BATTERY_CHARGER_A0_WORKAROUND
#error code not present
    #endif

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

    /* Enable battery charging/powering PMIC from USB device */
    err |= pm_chg_enum_done_enable(TRUE);

    /* Wait for 20ms */
    clk_busy_wait(20000);

    /* Enable automatic charging */
    err |= pm_chg_auto_disable(FALSE);

    /* Add itself as a listener for PMIC IRQs. */
    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    err |= irqEventGenerator->AddListenerForEndOfChargingIrq(this);
    err |= irqEventGenerator->AddListenerForFailOfChargingIrq(this);
    err |= irqEventGenerator->AddListenerForFastChargeIrq(this);
    err |= irqEventGenerator->AddListenerForBattConnectIrq(this);
    err |= irqEventGenerator->AddListenerForVbatdetIrq(this);

    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
    /* Add itself as a listern for charging heart beat */
    timerEventGenerator->AddListenerForChargingHeartBeat(this);
    /* Add itself as a listerner for max-charging-time detection */
    timerEventGenerator->AddListenerForMaxChargeTimeDetection(this);

    /* Update charger valid status */
    mGeneralStatus.is_charger_valid = TRUE;

    /* Update general status */
    mGeneralStatus.charger_status = CHARGER_STATUS__GOOD;

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
}

void AutoChargingManager::stopAutoCharging()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Disable automatic charging */
    err |= pm_chg_auto_disable(TRUE);

    /* Disable battery temperature monitoring */
    err |= pm_chg_batt_temp_disable(TRUE);

    /* Disable battery charging/powering PMIC from USB device */
    err |= pm_chg_enum_done_enable(FALSE);

    /* Reset the fail-of-charging state clear bit */
    err |= pm_chg_failed_clear(FALSE);

    /* Remove itself as a listener for PMIC IRQs */
    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    irqEventGenerator->RemoveListenerForEndOfChargingIrq(this);
    irqEventGenerator->RemoveListenerForFailOfChargingIrq(this);
    irqEventGenerator->RemoveListenerForBattTempIrq(this);
    irqEventGenerator->RemoveListenerForFastChargeIrq(this);
    irqEventGenerator->RemoveListenerForBattConnectIrq(this);
    irqEventGenerator->RemoveListenerForVbatdetIrq(this);

    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
    /* Remove itself as a listern for charging heart beat */
    timerEventGenerator->RemoveListenerForChargingHeatBeat(this);
    /* Remove itself as a listern for end-of-charging detection */
    timerEventGenerator->RemoveListenerForEndOfChargingDetectionTimeout(this);
    /* Remove itself as a listern for resume-charging detection */
    timerEventGenerator->RemoveListenerForResumeChargingDetectionTimeout(this);
    /* Remove itself as a listern for max-charging-time detection */
    timerEventGenerator->RemoveListenerForMaxChargeTimeDetection(this);
    /* Stop listening for the 1-sec delay timer */
    timerEventGenerator->RemoveListener(this, 998, 0);

    /* Update charger valid status */
    mGeneralStatus.is_charger_valid = FALSE;

    /* Update general status */
    mGeneralStatus.charger_status = CHARGER_STATUS__INVALID;

    /* Update charger type */
    mGeneralStatus.charger_hardware_type = CHARGER_TYPE__NONE;

    /* Reset some status variables */
    mGeneralStatus.battery_status = BATTERY_STATUS__INVALID;
    mIsInFinalCycle = false;
    mIsChargingComplete = false;
    mIsChargingFailed = false;
    mLowVbattCounter = 0;

    /* Clean up charger related PMIC registers*/
    err |= pm_chg_cleanup();

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
}

void AutoChargingManager::getCurrentState()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;
	ERR("getCurrentState Start", 0, 0, 0);
    /* Update previous state */
    mPreviousState = mCurrentState;

    /* Update current state */
    err |= pm_chg_charge_state_get(&mCurrentState);
	ERR("miaoxiaoming: mCurrentState=%d",mCurrentState,0,0);
    /* If the charge state was changed, send the notification */
    if(mCurrentState != mPreviousState)
    {
        /* Update the battery voltage and the battery temp */
        #ifdef FEATURE_VBATT_TASK /* FEATURE_VBATT_TASK */ 
        mGeneralStatus.battery_voltage     = vbatt_read_mv();

        mGeneralStatus.battery_temp        = vbatt_read_temperature();
        #else
        mGeneralStatus.battery_voltage     = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);

        mGeneralStatus.battery_temp        = chg_get_parameter_level(CHG_PARAMETER__BATT_THERM_DEGC);
        #endif

        /* Send the notification */
        #ifdef FEATURE_VBATT_TASK
          vbatt_notify_chg_event ( ) ;
        #endif
#ifdef CUST_EDITION		
        //when state is CHG_AUTO__PWR_ON_BATT_ST,dont need to notify mmi
		if (mCurrentState != CHG_AUTO__PWR_ON_BATT_ST)
		{
			oembatt_notify_chg_event();
		}
#endif		
    }

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }

    /* Update UI status */
    switch(mCurrentState)
    {
        case CHG_AUTO__INIT_ST: /* state 0 */
        {
            mGeneralStatus.ui_event = CHG_UI_EVENT__IDLE;
            mGeneralStatus.is_charging = FALSE;
            mGeneralStatus.is_battery_valid = TRUE;
        }
        break;
        case CHG_AUTO__PWR_ON_CHGR_ST: /* state 1 */ 
        {
            // check battery status
            boolean             rt_status;

            /* First debounce batt_connect IRQ. */
            err |= pm_get_rt_status(PM_BATTCONNECT_RT_ST, &rt_status);

            if(rt_status)/*battery is removed */
            {
                mGeneralStatus.battery_status = BATTERY_STATUS__REMOVED;
            }
            else
            {
                /* Then debounce batt_temp IRQ. */
                err |= pm_get_rt_status(PM_BATTTEMP_RT_ST, &rt_status);
				
                if(rt_status) /* Battery temp is bad */
                {
                    mGeneralStatus.battery_status = BATTERY_STATUS__BAD_TEMP;

                }
                else
                {
                    if(true == mIsChargingFailed)
                    {
                        mGeneralStatus.battery_status = BATTERY_STATUS__BAD;
                    }
                    else
                    {
                        mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
                    }
                }
            }

            /* If battery is removed or is too hot/cold or is bad */
            if(BATTERY_STATUS__GOOD != mGeneralStatus.battery_status)
            {
                /* Update general status */
                mGeneralStatus.ui_event = CHG_UI_EVENT__VERY_LOW_POWER;
                mGeneralStatus.is_charging = FALSE;
                mGeneralStatus.is_battery_valid = FALSE;
                mGeneralStatus.battery_level = BATTERY_LEVEL__DEAD;
            }
            else /* battery is normal */
            {
                /* The final charging cycle has just finished, Charging is complete */
                if(true == mIsInFinalCycle)
                {
                	ERR("miaoxiaoming:mIsInFinalCycle=%d",mIsInFinalCycle,0,0);
                    /* Set charging-complete flag */
                    mIsChargingComplete = true;

                    /* Reset charing-final-cycle flag */
                    mIsInFinalCycle = false;

                    /*************************************************************************
                     To prevent excessive charging it is necessary to make sure that momentary 
                     high current events do not retrigger charging, especially on GSM systems.
                     For this reason VBATDET should be set to 3.9V once charging has completed.
                     After lowering VBATDET to 3.9V, the software should poll VBAT once per 
                     minute while charging still plugged in. If 5 consecutive readings are 
                     below 4.1V, then charging may resume. After charging is resumed, VBATDET 
                     should be set back to 4.1V. 
                    **************************************************************************/
                    /* Lower VBATDET to 3.9V */
                    err |= pm_chg_vbatdet_set(AUTO_CHARGING_RESUME_DETECTION_VBATDET);
                    /* Start 1-minute timer to take VBATT readings */
                    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                    timerEventGenerator->AddListenerForResumeChargingDetectionTimeout(this);
                    /* Stop listening for the max-charging-timeout timer */
                    timerEventGenerator->RemoveListenerForMaxChargeTimeDetection(this);

                } /* end if(true == mIsInFinalCycle)*/

                // Update UI status
                if(true == mIsChargingComplete)
                {
                    /* Update general status */
                    mGeneralStatus.ui_event = CHG_UI_EVENT__DONE;
                    mGeneralStatus.is_charging = TRUE;
                    mGeneralStatus.is_battery_valid = TRUE;
                    mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
                    mGeneralStatus.battery_level = BATTERY_LEVEL__FULL;
                }
            } /* end if battery is missing or hot/cold */
        }
        break;
        case CHG_AUTO__AUTO_TRICKLE_ST: /* state 2 */
        {
            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__VERY_LOW_POWER;
            mGeneralStatus.is_charging = TRUE;
            mGeneralStatus.is_battery_valid = TRUE;
            mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__DEAD;
        }
        break;
        case CHG_AUTO__PWR_ON_BATT_ST: /* state 3 */
        {
            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__NORMAL_POWER;
            mGeneralStatus.is_charging = FALSE;
            mGeneralStatus.is_battery_valid = TRUE;
            mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__GOOD;
        }
        break;
        case CHG_AUTO__AUTO_TRICKLE_FAILED_ST: /* state 4 */
        {
            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__VERY_LOW_POWER;
            mGeneralStatus.is_charging = FALSE;
            mGeneralStatus.is_battery_valid = FALSE; 
            mGeneralStatus.battery_status = BATTERY_STATUS__BAD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__DEAD;
        }
        break;
        case CHG_AUTO__TRANS_ST: /* state 5 */
        {
           //no need to update the status in this state;
           //since it is a transition state.
        }
        break;
        case CHG_AUTO__POST_AUTO_TRICKLE_ST: /* state 6 */
        {
            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__LOW_POWER;
            mGeneralStatus.is_charging = TRUE;
            mGeneralStatus.is_battery_valid = TRUE; 
            mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__WEAK;
        }
        break;
        case CHG_AUTO__FAST_ST: /* state 7 */
        {
            /* charging has been completed earlier; if after that, the state machine comes back
               to state 7, it means that the battery is not fully charged anymore */
            /* Therefore reset charging complete flag, restart the charging process */
            if(true == mIsChargingComplete)
            {
                mIsChargingComplete = false;
            }

            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__NORMAL_POWER;
            mGeneralStatus.is_charging = TRUE;
            mGeneralStatus.is_battery_valid = TRUE; 
            mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__GOOD;
        }
        break;
        case CHG_AUTO__TRICKLE_ST: /* state 8 */
        {
            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__VERY_LOW_POWER;
            mGeneralStatus.is_charging = TRUE;
            mGeneralStatus.is_battery_valid = TRUE;
            mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__DEAD;
        }
        break;
        case CHG_AUTO__FAILED_ST:/* state 9 */
        {
            /* In trickle charging previously: trickle charging failed */
            if(CHG_AUTO__TRICKLE_ST == mPreviousState) 
            {
                /* Stop Autonomous charging */
                err |= pm_chg_auto_disable(TRUE);

                /* Stop listening for the max-charging-timeout timer */
                TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                timerEventGenerator->RemoveListenerForMaxChargeTimeDetection(this);

                /* update charging-failed status */
                mIsChargingFailed = true;
            }
            else /* In fast charge state previously: fast charging failed */
            {
                if( (false == mIsInFinalCycle) &&
                    (CHG_AUTO__FAST_ST == mPreviousState) &&
                    (false == mIsChargingFailed) )
                {
                    /* Listen for end-of-detection timeout */
                    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                    timerEventGenerator->AddListenerForEndOfChargingDetectionTimeout(this);
                }
            }

            /* Exit the fail-of-charging state */
            err |= pm_chg_failed_clear(TRUE);

        }
        break;
        case CHG_AUTO__DONE_ST: /* state 10 */
        {
            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__DONE;
            mGeneralStatus.is_charging = FALSE;
            mGeneralStatus.is_battery_valid = TRUE;
            mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__FULL;
        }
        break;
        case CHG_AUTO__LOW_ILIM_ST: /* state 11 */
        {
            //no need to update the status in this state;
            //since it is a transition state.
        }
        break;
        case CHG_AUTO__USB_SUSP_ST: /* state 12 */
        {
            /* Update general status */
            mGeneralStatus.ui_event = CHG_UI_EVENT__NORMAL_POWER;
            mGeneralStatus.is_charging = FALSE;
            mGeneralStatus.is_battery_valid = TRUE;
            mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
            mGeneralStatus.battery_level = BATTERY_LEVEL__GOOD;
        }
        break;
        default:
        {

        }
        break;
    } /* end switch() */
	ERR("getCurrentState End", 0, 0, 0);
}

void AutoChargingManager::EventFired(ChargingEvent* event)
{
	ERR("EventFired Start", 0, 0, 0);
    ASSERT(event != NULL);

    switch( event->GetEventId() )
    {
        case CHG_CMD__IRQ_TRIGGERED:
        {
            IrqEvent* irqevent = dynamic_cast< IrqEvent* >( event );

            ASSERT( irqevent != NULL );

            pm_irq_hdl_type irqid = irqevent->GetIrqId();
			ERR("miaoxiaoming: irqid = %d",irqid,0,0);
            if(irqid == PM_VALID_CHG_IRQ_HDL) /* A charging source is attached */
            {
                /* Start autonomous charging */
                startUcpAutoCharging();

                /* Update charger hardware type */
                mGeneralStatus.charger_hardware_type = CHARGER_TYPE__WALL;
#ifdef CUST_EDITION
				oembatt_notify_chg_event();
#endif				
            }
            else if(irqid == PM_INVALID_CHG_IRQ_HDL ) /* A charging source is removed */
            {
                /* Stop autonomous charging */
                stopAutoCharging();
#ifdef CUST_EDITION		
			oembatt_notify_chg_event();
#endif				
            }
            else if( irqid == PM_BATTCONNECT_IRQ_HDL ) /* Battery connected/removed */
            {
                pm_err_flag_type    err = PM_ERR_FLAG__SUCCESS;
                boolean             rt_status;

                /* First debounce the IRQ. */
                err |= pm_get_rt_status(PM_BATTCONNECT_RT_ST, &rt_status);

                if (rt_status) /* the battery is removed */
                {
                    if(BATTERY_STATUS__REMOVED != mGeneralStatus.battery_status)/* the battery was just removed */
                    {
                        // Update battery status
                        mGeneralStatus.battery_status = BATTERY_STATUS__REMOVED;

                        /* Stop listenning for the bad-battery-temp IRQ */
                        IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                        irqEventGenerator->RemoveListenerForBattTempIrq(this);

                        /*******************************************************
                         If the battery is removed, stop autonomous charging;
                         but keep battery monitoring on
                        *******************************************************/

                        /* Disable automatic charging */
                        err |= pm_chg_auto_disable(TRUE);

                        /* Enable battery temperature monitoring */
                        err |= pm_chg_batt_temp_disable(FALSE);
                    } /* if(false == mBatteryRemoved) */
                 }
                 else /* the battery is attached */
                 {
                     if(BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status) /* The battery was just attached back */
                     {
                         /*******************************************************
                          the battery was removed previously, the autonomous 
                          charging was stopped because of it; so wait 1 sec to 
                          resume autonomous charging after the battery was inserted
                          back
                         *******************************************************/

                         /* Keep listenning for battery temp IRQ */
                         IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                         err |= irqEventGenerator->AddListenerForBattTempIrq(this);

                         /* Start the 1-sec timer */
                         TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                         timerEventGenerator->AddListener(this, 998, 0, false );

                        /* Update battery removal status */
                        mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;

                     } /* end if(true == mBatteryRemoved) */
                 } /* end if (rt_status) */

                 /* Update status */
                 mBattConnectIrqTriggered = true;

                 /* Find out if there was an PMIC API error. */
                 if (err != PM_ERR_FLAG__SUCCESS) 
                 {
                     MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                 }
            } /* end if( irqid == PM_BATTCONNECT_IRQ_HDL  */
            else if(irqid == PM_BATTTEMP_IRQ_HDL) /* Battery temp has gone out of range */
            {
                /* Update status */
                mBattTemptIrqTriggered = true;

                /* Stop listenning for the IRQ */
                IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                irqEventGenerator->RemoveListenerForBattTempIrq(this);
            }
            else if(irqid == PM_CHGSTATE_IRQ_HDL) /* Charging state has changed */
            {
                // Update charging status based on the current charging state.
                getCurrentState();

                /* Update status */
                mStateChangeIrqTriggered = true;
            }
            else if( irqid == PM_CHGDONE_IRQ_HDL ) /* Charging has finished successfully */
            {
                /* Update status */
                mChargingDoneIrqTriggered = true;

                #ifdef FEATURE_BATTERY_CHARGER_A0_WORKAROUND
#error code not present
                #endif

            }
            else if(irqid == PM_CHGFAIL_IRQ_HDL) /* Charging has failed */
            {
                /* Update status */
                mChargingFailIrqTriggered = true;

            }
            else if(irqid == PM_VBAT_DET_IRQ_HDL) /* VBATDET has reached */
            {
                pm_err_flag_type  err =  PM_ERR_FLAG__SUCCESS;

                /* Update status */
                mVbatdetIrqTriggered = true;

                /* Set the fast charge timeout to the end-of-charging detection value */
                err |= pm_chg_tchg_set(AUTO_CHARGING_T);

                /* Start listening for end-of-charging detection timer */
                TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                timerEventGenerator->AddListenerForEndOfChargingDetectionTimeout(this);

                /* Find out if there was an PMIC API error. */
                if (err != PM_ERR_FLAG__SUCCESS) 
                {
                    MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                }
            }
            else if(irqid == PM_FASTCHG_IRQ_HDL) /* in fast charging */
            {
                #ifdef FEATURE_BATTERY_CHARGER_A0_WORKAROUND
#error code not present
                #endif
            }
        } /* case CHG_CMD__IRQ_TRIGGERED */
        break;
        case CHG_CMD__TIMER_EXPIRED:
        {
            TimerEvent* timerevent = dynamic_cast< TimerEvent* >( event );

            ASSERT( timerevent != NULL );

            unsigned int  firstdelay = timerevent->GetFirstDelay();
            ERR("miaoxiaoming: firstdelay=%d",firstdelay,0,0);
            /* Charger state machine heart beat */
            if(firstdelay == CHG_HEART_BEAT_FIRST_TIME )
            {
                pm_err_flag_type    err = PM_ERR_FLAG__SUCCESS;
                boolean             rt_status;
                IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
 
                /* Check if the battery is connected. */
                err |= pm_get_rt_status(PM_BATTCONNECT_RT_ST, &rt_status);

                 /*Check for battery temperature only if the battery is connected */
                 if (!rt_status) 
                 {
                     if(BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status) /* The battery was just attached back */
                     {
                         /*******************************************************
                          the battery was removed previously, the autonomous 
                          charging was stopped because of it; so wait 1 sec to 
                          resume autonomous charging after the battery was inserted
                          back
                         *******************************************************/

                         /* Keep listenning for battery temp IRQ */
                         err |= irqEventGenerator->AddListenerForBattTempIrq(this);

                         /* Start the 1-sec timer */
                         TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                         timerEventGenerator->AddListener(this, 998, 0, false );

                        /* Update battery status */
                        mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;

                     } /* end if(true == mBatteryRemoved) */
                 } /* end if (!rt_status) */

                 /* Update curernt state */
                 getCurrentState();

                 /* Update general status */
                 #ifdef FEATURE_VBATT_TASK /* FEATURE_VBATT_TASK */ 
                 mGeneralStatus.battery_temp = vbatt_read_temperature();

                 mGeneralStatus.battery_voltage = vbatt_read_mv();
                 #else
                 mGeneralStatus.battery_temp = chg_get_parameter_level(CHG_PARAMETER__BATT_THERM_DEGC);

                 mGeneralStatus.battery_voltage = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);
                 #endif
                 
                 /* Keep listenning to the change-of-state IRQ */
                if(true == mStateChangeIrqTriggered)
                {
                    irqEventGenerator->RemoveListenerForChangeOfStateIrq(this);
                    err |= irqEventGenerator->AddListenerForChangeOfStateIrq(this);
                    mStateChangeIrqTriggered = false;
                }

                /* Keep listenning to the charging-done IRQ */
                if(true == mChargingDoneIrqTriggered)
                {
                    irqEventGenerator->RemoveListenerForEndOfChargingIrq(this);
                    err |= irqEventGenerator->AddListenerForEndOfChargingIrq(this);
                    mChargingDoneIrqTriggered = false;
                }

                /* Keep listenning to the charging-fail IRQ */
                if(true == mChargingFailIrqTriggered)
                {
                    irqEventGenerator->RemoveListenerForFailOfChargingIrq(this);
                    err |= irqEventGenerator->AddListenerForFailOfChargingIrq(this);
                    mChargingFailIrqTriggered = false;
                }

                /* Keep listening for the BATT_TEMP IRQ */
                if(true == mBattTemptIrqTriggered)
                {
                    err |= irqEventGenerator->AddListenerForBattTempIrq(this);
                    mBattTemptIrqTriggered = false;
                }

                /* Keep listening for the BATT_CONNECT IRQ */
                if(true == mBattConnectIrqTriggered)
                {
                    irqEventGenerator->RemoveListenerForBattConnectIrq(this);
                    err |= irqEventGenerator->AddListenerForBattConnectIrq(this);
                    mBattConnectIrqTriggered = false;
                }

                /* Find out if there was an PMIC API error. */
                if (err != PM_ERR_FLAG__SUCCESS) 
                {
                    MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                }

            } /* end CHG_HEART_BEAT */
            /* End-of-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_END_CHARGE_DETECTION_T )
            {
                pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;
                pm_auto_chg_state_name_type  current_state = CHG_AUTO__INIT_ST;

                /* Stop listening for the end-of-charging detection timer */
                TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                timerEventGenerator->RemoveListenerForEndOfChargingDetectionTimeout(this);

                /* Check current charging state */
                err |= pm_chg_charge_state_get(&current_state);

                /* Power on from the charger state.*/
                if( (CHG_AUTO__PWR_ON_CHGR_ST == current_state) &&
                    (false == mIsInFinalCycle)                  &&
                    (false == mIsChargingComplete) ) 
                {
                    // check battery status
                    boolean             rt_status1;
                    boolean             rt_status2;

                    /* Check batt_connect IRQ. */
                    err |= pm_get_rt_status(PM_BATTCONNECT_RT_ST, &rt_status1);

                    if(!rt_status1)/*battery is connected */
                    {
                        /* Then debounce batt_temp IRQ. */
                        err |= pm_get_rt_status(PM_BATTTEMP_RT_ST, &rt_status2);

                        if(!rt_status2) /* Battery temp is normal */
                        {
                            /* set charging time to AUTO_CHARGING_FINAL_CYCLE_T*/
                            err |= pm_chg_tchg_set(AUTO_CHARGING_FINAL_CYCLE_T);

                            /* Force charging for the final cycle */
                            err |= pm_chg_charge_state_set(CHG_AUTO__FAST_ST);

                            /* Update the status */
                            mIsInFinalCycle = true;

                        } /* end if(!rt_status2) */
                    } /* end if(!rt_status1) */
                }

                /* Find out if there was an PMIC API error. */
                if (err != PM_ERR_FLAG__SUCCESS) 
                {
                    MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                }
            }/* end AUTO_CHARGING_END_CHARGE_DETECTION_T */
            /* Resume-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_RESUME_CHARGE_DETECTION_T )
            {
                /* Check if we are still in charging-complete status */
                if(true == mIsChargingComplete)
                {
                    /* Get the battery voltage */
                    short vbatt = 0;
                    #ifdef FEATURE_VBATT_TASK /* FEATURE_VBATT_TASK */ 
                    vbatt   = vbatt_read_mv();
                    #else
                    vbatt   = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);
                    #endif

                    /* If the battery voltage is lower than VBATDET, increment the counter;
                       otherwise reset the counter */
                    if(vbatt < AUTO_CHARGING_VBATDET)
                    {
                        /* Increase the counter*/
                        mLowVbattCounter++;
                    }
                    else
                    {
                        /* Reset the counter */
                        mLowVbattCounter = 0;
                    }

                    /* Check if the counter has reached the limit; if yes, resume autonomous charging by
                     restoring the VBATDET setting from AUTO_CHARGING_RESUME_DETECTION_VBATDET to its 
                     normal value AUTO_CHARGING_VBATDET*/
                    if(mLowVbattCounter == AUTO_CHARGING_RESUME_CHARGE_DETECTION_COUNTER)
                    {
                        /* Reset the counter */
                        mLowVbattCounter = 0;

                        pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;
                        /* Set VBATDET back to 4100 */
                        err |= pm_chg_vbatdet_set(AUTO_CHARGING_VBATDET);

                        /* Set the fast charge timeout to the maxinum value */
                        err |= pm_chg_tchg_set(512);

                        TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                        /* Stop listening for resume-charging detection timeout */
                        timerEventGenerator->RemoveListenerForResumeChargingDetectionTimeout(this);
                        /* Add itself as a listerner for max-charging-time detection */
                        timerEventGenerator->AddListenerForMaxChargeTimeDetection(this);

                        /* Add iteself as a listener for VBATDET IRQ */
                        IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                        err |= irqEventGenerator->AddListenerForVbatdetIrq(this);

                        /* Find out if there was an PMIC API error. */
                        if (err != PM_ERR_FLAG__SUCCESS) 
                        {
                            MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                        }
                    }
                }
            } /* end AUTO_CHARGING_RESUME_CHARGE_DETECTION_T */
            /* max-charging-time detection times out */
            else if(firstdelay == (AUTO_CHARGING_MAX_CHARGE_T*60000) )
            {
                pm_err_flag_type  err =  PM_ERR_FLAG__SUCCESS;

                /* The maximum charging time reached, stop autonomous charging */
                err |= pm_chg_auto_disable(TRUE);

                /* Update the charging-failed status */
                mIsChargingFailed = true;

                /* Find out if there was an PMIC API error. */
                if (err != PM_ERR_FLAG__SUCCESS) 
                {
                    MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                }

            } /* end AUTO_CHARGING_MAX_CHARGE_T */
            else if(firstdelay == 998)
            {
                /*******************************************************
                 the battery was removed previously, the autonomous 
                 charging was stopped because of it; after the battery
                 is inserted back, wait for 1 sec and resume autonomous
                 charging
                *******************************************************/
                if(BATTERY_STATUS__REMOVED != mGeneralStatus.battery_status)
                {
                    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

                    /* Enable automatic charging */
                    err |= pm_chg_auto_disable(FALSE);

                    /* Find out if there was an PMIC API error. */
                    if (err != PM_ERR_FLAG__SUCCESS) 
                    {
                        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                    }
                }

                TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                /* Stop listening for the 1-sec delay timer */
                timerEventGenerator->RemoveListener(this, 998, 0);

            } /* end if(firstdelay == 998) */
        }
        break; /* case CHG_CMD__TIMER_EXPIRED */
        case CHG_CMD__SYSTEM_STATUS_CHANGED:
        {
            SystemEvent* systemevent = dynamic_cast< SystemEvent* >( event );

            ASSERT( systemevent != NULL );

            chg_system_event_id_type  systemeventid = systemevent->GetSystemEventId();

            /* Turn on auto charging */
            if(systemeventid == CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING)
            {
                /* Start autonomous charging */
                

            }
            /* Turn off auto charging */
            else if(systemeventid == CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING)
            {
                /* Stop autonomous charging */
                

            }
            else if(systemeventid == CHG_SYSTEM__USB_VALID) /* A USB source is attached */
            {
                /* A PC-USB/ SHP just attached */
                if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_HOST)
                {
                    /* Start autonomous charging */
                    startShpAutoCharging();
                }
                else if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_CHARGER)
                {
                    /* Start autonomous charging */
                    startUcpAutoCharging();

                    /* Update charger hardware type */
                    mGeneralStatus.charger_hardware_type = CHARGER_TYPE__USB_WALL;
                }
            }
            else if(systemeventid == CHG_SYSTEM__USB_INVALID) /* A USB source is removed */
            {
                /* Stop autonomous charging */
                stopAutoCharging();
            }
            /* USB device is in suspend mode */
            else if(systemeventid == CHG_SYSTEM__USB_SUSPEND) 
            {
                pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

                /* Stop autonomous charging */
                stopAutoCharging();

                /* Disable PMIC USB suspended mode */
                err |= pm_chg_usb_suspend(false);

                /* Set the charging state to "power from the battery" */
                err |= pm_chg_charge_state_set(CHG_AUTO__PWR_ON_BATT_ST);

                /* Find out if there was an PMIC API error. */
                if (err != PM_ERR_FLAG__SUCCESS) 
                {
                    MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                }
            }
            /* System is in a call */
            else if(systemeventid == CHG_SYSTEM__TX_ON) 
            {
                pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

                if((CHARGER_TYPE__WALL == mGeneralStatus.charger_hardware_type) ||
                   (CHARGER_TYPE__USB_WALL == mGeneralStatus.charger_hardware_type))
                {
                    /* Reduce the IMAXSEL to avoid possible degrading of the RF performance */
                    err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_TX_ON); 
                }

                /* Find out if there was an PMIC API error. */
                if (err != PM_ERR_FLAG__SUCCESS) 
                {
                    MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                }
            } /* CHG_SYSTEM__TX_ON */
            /* System is NOT in a call anymore */
            else if(systemeventid == CHG_SYSTEM__TX_OFF) 
            {
                pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

                if((CHARGER_TYPE__WALL == mGeneralStatus.charger_hardware_type) ||
                   (CHARGER_TYPE__USB_WALL == mGeneralStatus.charger_hardware_type))
                {
                    /* Restore the IMAXSEL */
                    err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_MAXIMUM); 
                }

                /* Find out if there was an PMIC API error. */
                if (err != PM_ERR_FLAG__SUCCESS) 
                {
                    MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                }
            } /* CHG_SYSTEM__TX_OFF */

        } /* end CHG_CMD__SYSTEM_STATUS_CHANGED */
        break;
    } /* switch() */
	ERR("EventFired End", 0, 0, 0);
} /* EventFired() */

                  /******************************************/
                  /*     class Debug_AutoChargingManager     */
                  /******************************************/

Debug_AutoChargingManager::Debug_AutoChargingManager()
{
    /* Initialize debug information */
    mAutoChargingInformation.battery_temperature = 0;
    mAutoChargingInformation.battery_voltage = 0;
    mAutoChargingInformation.charging_current = 0;
    mAutoChargingInformation.current_state = CHG_AUTO__INIT_ST;
    mAutoChargingInformation.previous_state = CHG_AUTO__INIT_ST;
    mAutoChargingInformation.charger_type = CHARGER_TYPE__INVALID;
    mAutoChargingInformation.battery_connect_irq_triggered = FALSE;
    mAutoChargingInformation.bad_battery_temp_irq_triggered = FALSE;
    mAutoChargingInformation.change_of_state_irq_triggered = FALSE;
    mAutoChargingInformation.end_of_charging_irq_triggered = FALSE;
    mAutoChargingInformation.fail_of_charging_irq_triggered = FALSE;
    mAutoChargingInformation.vbat_det_irq_triggered = FALSE;
    mAutoChargingInformation.pulse_done_irq_triggered = FALSE;
    mAutoChargingInformation.batt_alarm_irq_triggered = FALSE;
    mAutoChargingInformation.charger_irq_triggered = FALSE;
    mAutoChargingInformation.chgtlimit_irq_triggered = FALSE;
    mAutoChargingInformation.vcp_irq_triggered = FALSE;
    mAutoChargingInformation.battery_removed = FALSE;
    mAutoChargingInformation.battery_bad = FALSE;
    mAutoChargingInformation.pulse_charging_on = FALSE;
    mAutoChargingInformation.cv_topoff_on = FALSE;
    mAutoChargingInformation.charging_finished = FALSE;
    mAutoChargingInformation.charging_in_final_cycle = FALSE;
    mAutoChargingInformation.TX_on = FALSE;
}

Debug_AutoChargingManager::~Debug_AutoChargingManager()
{

}

void Debug_AutoChargingManager::updateDebugInformation()
{
    /* Update Battery Voltage and battery temperature. */
    #ifdef FEATURE_VBATT_TASK /* FEATURE_VBATT_TASK */ 
    mAutoChargingInformation.battery_voltage     = vbatt_read_mv();

    mAutoChargingInformation.battery_temperature = vbatt_read_temperature();
    #else
    mAutoChargingInformation.battery_voltage     = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);

    mAutoChargingInformation.battery_temperature = chg_get_parameter_level(CHG_PARAMETER__BATT_THERM_DEGC);
    #endif

    /* Update charging current */
    mAutoChargingInformation.charging_current = chg_get_parameter_level(CHG_PARAMETER__CHARGER_I);

    /* Update charging state */
    mAutoChargingInformation.current_state = mCurrentState;

    if(true == mIsChargingFailed)
    {
        mAutoChargingInformation.battery_bad = TRUE;
    }
    else
    {
        if( BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status)
        {
            mAutoChargingInformation.battery_removed = TRUE;
        }
        else
        {
            mAutoChargingInformation.battery_removed = FALSE;
        }

        mAutoChargingInformation.battery_bad = FALSE;
    }

    if(TRUE == chg_system_tx_informaton.tx_on)
    {
        mAutoChargingInformation.TX_on = TRUE;
    }

    if(true == mIsInFinalCycle)
    {
        mAutoChargingInformation.charging_in_final_cycle = TRUE;
    }
    else
    {
        mAutoChargingInformation.charging_in_final_cycle = FALSE;
    }

    if(true == mIsChargingComplete)
    {
        mAutoChargingInformation.charging_finished = TRUE;
    }
    else
    {
        mAutoChargingInformation.charging_finished = FALSE;
    }
}

void Debug_AutoChargingManager::EventFired(ChargingEvent* event)
{
    ASSERT(event != NULL);

    AutoChargingManager::EventFired(event);

    switch( event->GetEventId() )
    {
        case CHG_CMD__IRQ_TRIGGERED:
        {
            IrqEvent* irqevent = dynamic_cast< IrqEvent* >( event );

            ASSERT( irqevent != NULL );

            pm_irq_hdl_type irqid = irqevent->GetIrqId();

            if(irqid == PM_VALID_CHG_IRQ_HDL) /* A charging source is attached */
            {
                MSG_HIGH(" chg_debug: Regular Wall charger has plugged in", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if(irqid == PM_INVALID_CHG_IRQ_HDL ) /* A charging source is removed */
            {
                MSG_HIGH(" chg_debug: Regular Wall charger is removed", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if( irqid == PM_BATTCONNECT_IRQ_HDL ) /* Battery connected/removed */
            {
                if(BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status)
                {
                    MSG_HIGH(" chg_debug: Battery is removed", 0, 0, 0);

                    /* Update debug info */
                    mAutoChargingInformation.battery_removed = TRUE;
                }
                else
                {
                    MSG_HIGH(" chg_debug: Battery is connected", 0, 0, 0);

                    /* Update debug info */
                    mAutoChargingInformation.battery_removed = FALSE;
                }

                /* Update debug information */
                mAutoChargingInformation.battery_connect_irq_triggered = TRUE;
            }
            else if(irqid == PM_CHGSTATE_IRQ_HDL) /* Charging state has changed */
            {
                MSG_HIGH(" chg_debug: charging state changed", 0, 0, 0);

                /* Update previous state */
                mAutoChargingInformation.previous_state = mAutoChargingInformation.current_state;

                /* Update debug information */
                mAutoChargingInformation.change_of_state_irq_triggered = TRUE;
                mAutoChargingInformation.current_state = mCurrentState;
            }
            else if( irqid == PM_CHGDONE_IRQ_HDL ) /* Charging has finished successfully */
            {
                MSG_HIGH(" chg_debug: charging-done IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.end_of_charging_irq_triggered = TRUE;
            }
            else if(irqid == PM_CHGFAIL_IRQ_HDL) /* Charging has failed */
            {
                MSG_HIGH(" chg_debug: charging-fail IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.fail_of_charging_irq_triggered = TRUE;
            }
            else if(irqid == PM_BATTTEMP_IRQ_HDL) /* Battery temp has gone out of range */
            {
                /*check for battery temperature only when valid battery is connected*/
                if(BATTERY_STATUS__REMOVED != mGeneralStatus.battery_status)
                {
                    MSG_HIGH(" chg_debug: bad-batt-temp IRQ triggered", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.bad_battery_temp_irq_triggered = TRUE;
                }
            }
            else if(irqid == PM_VBAT_DET_IRQ_HDL) /* VBATDET has reached */
            {
                MSG_HIGH(" chg_debug: vbatdet IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.vbat_det_irq_triggered = TRUE;
            }
            else if(irqid == PM_FASTCHG_IRQ_HDL) /* in fast charging */
            {
                MSG_HIGH(" chg_debug: in-fast-charging IRQ triggered", 0, 0, 0);
            }
        } /* case CHG_CMD__IRQ_TRIGGERED */
        break;
        case CHG_CMD__TIMER_EXPIRED:
        {
            TimerEvent* timerevent = dynamic_cast< TimerEvent* >( event );

            ASSERT( timerevent != NULL );

            unsigned int  firstdelay = timerevent->GetFirstDelay();
            
            /* Charger state machine heart beat */
            if(firstdelay == CHG_HEART_BEAT_FIRST_TIME )
            {
                 /* Update debug information */
                 updateDebugInformation();

                 /* Fire debug event */
                 ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                 FireEvent(pEvent);
                 /* Release memory */
                 delete pEvent;

                 /* Reset debug information */
                 mAutoChargingInformation.battery_connect_irq_triggered = FALSE;
                 mAutoChargingInformation.bad_battery_temp_irq_triggered = FALSE;
                 mAutoChargingInformation.change_of_state_irq_triggered = FALSE;
                 mAutoChargingInformation.end_of_charging_irq_triggered = FALSE;
                 mAutoChargingInformation.fail_of_charging_irq_triggered = FALSE;
                 mAutoChargingInformation.vbat_det_irq_triggered = FALSE;
                 mAutoChargingInformation.charger_irq_triggered = FALSE;
            } /* end CHG_HEART_BEAT */
            /* End-of-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_END_CHARGE_DETECTION_T )
            {
                MSG_HIGH(" chg_debug: end-of-charging detection timer expired", 0, 0, 0);

                if(true == mIsInFinalCycle)
                {
                    mAutoChargingInformation.charging_in_final_cycle = TRUE;
                }
                else
                {
                    mAutoChargingInformation.charging_in_final_cycle = FALSE;
                }

            }/* end AUTO_CHARGING_END_CHARGE_DETECTION_T */
            /* Resume-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_RESUME_CHARGE_DETECTION_T )
            {
                MSG_HIGH(" chg_debug: resume-charge detection timer expired", 0, 0, 0);
            } /* end AUTO_CHARGING_RESUME_CHARGE_DETECTION_T */
            /* max-charging-time detection times out */
            else if(firstdelay == (AUTO_CHARGING_MAX_CHARGE_T*60000) )
            {
                MSG_HIGH(" chg_debug: max-charging-time detection timer expired", 0, 0, 0);
            } /* end AUTO_CHARGING_MAX_CHARGE_T */
        }
        break; /* case CHG_CMD__TIMER_EXPIRED */
        case CHG_CMD__SYSTEM_STATUS_CHANGED:
        {
            SystemEvent* systemevent = dynamic_cast< SystemEvent* >( event );

            ASSERT( systemevent != NULL );

            chg_system_event_id_type  systemeventid = systemevent->GetSystemEventId();

            if(systemeventid == CHG_SYSTEM__USB_VALID) /* A USB source is attached */
            {
                MSG_HIGH(" chg_debug: Valid USB device is connected", 0, 0, 0);

                /* A PC-USB/ SHP just attached */
                if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_HOST)
                {
                    MSG_HIGH(" chg_debug: a PC-USB/ SHP charger has plugged in", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.charger_irq_triggered = TRUE;
                    mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;
                }
                else if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_CHARGER)
                {
                    MSG_HIGH(" chg_debug: a Wall-USB/UCP charger has plugged in", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.charger_irq_triggered = TRUE;
                    mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;
                }

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if(systemeventid == CHG_SYSTEM__USB_INVALID) /* A USB source is removed */
            {
                MSG_HIGH(" chg_debug: The USB device has removed", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            /* USB device is in suspend mode */
            else if(systemeventid == CHG_SYSTEM__USB_SUSPEND) 
            {
                MSG_HIGH(" chg_debug: In USB suspend state", 0, 0, 0);

            }
            /* System is in a call */
            else if(systemeventid == CHG_SYSTEM__TX_ON) 
            {
                MSG_HIGH(" chg_debug: In TX ON state", 0, 0, 0);

                mAutoChargingInformation.TX_on = TRUE;
            } /* CHG_SYSTEM__TX_ON */
            /* System is NOT in a call anymore */
            else if(systemeventid == CHG_SYSTEM__TX_OFF) 
            {
                MSG_HIGH(" chg_debug: In TX OFF state", 0, 0, 0);

                mAutoChargingInformation.TX_on = FALSE;
            } /* CHG_SYSTEM__TX_OFF */

        } /* end CHG_CMD__SYSTEM_STATUS_CHANGED */
        break;
    } /* switch() */
} /* EventFired() */

#if defined(FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(T_WINNT)
                  /************************************************/
                  /*     class AutoChargingManagerPulseTopoff     */
                  /************************************************/
AutoChargingManagerPulseTopoff::AutoChargingManagerPulseTopoff():
    mIsPulseChargingOn(false),
    mVBatteryAlarmIrqTriggered(false)
{
    
}

AutoChargingManagerPulseTopoff::~AutoChargingManagerPulseTopoff()
{

}

void AutoChargingManagerPulseTopoff::startTopoffPulseCharging()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    pm_chg_pulse_chg_config_type   pulse_config;
    pulse_config.p_type = CHG_TOPOFF_PULSE_TYPE;
    pulse_config.t_on   = CHG_TOPOFF_PULSE_TON;
    pulse_config.t_off  = CHG_TOPOFF_PULSE_TOFF;
    pulse_config.t_done = CHG_TOPOFF_PULSE_TDONE;

    /* Configure the charger transistor max voltage (VMAXSEL) and 
     charger transistor max current(IMAXSEL). */
    err |= pm_chg_vmaxsel_set(AUTO_CHARGING_PULSE_VMAXSEL);
    err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_MAXIMUM);

    /* Configure pulse charger type and timers. */
    err |= pm_chg_pulse_charger_config(&pulse_config);

    /* Configure vbatdet. */
    err |= pm_chg_vbatdet_set(AUTO_CHARGING_PULSE_VBATDET);

    /* Disable autonomous charging */
    err |= pm_chg_auto_disable(TRUE);

    /* Enable pulse charging. */
    err |= pm_chg_pulse_charger_switch(PM_ON_CMD);

    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    /* Add itself as a listener for PMIC PulseDone IRQ. */
    err |= irqEventGenerator->AddListenerForPulseTDoneIrq(this);
    /* Add itself as a listerner for PMIC battery alarm IRQ */
    err |= irqEventGenerator->AddListenerForBatteryAlarmIrq(this,CHG_VMAX_TOPOFF, CHG_VMIN_TOPOFF);

    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
    /* Stop listening for end-of-charging detection timeout */
    timerEventGenerator->RemoveListenerForEndOfChargingDetectionTimeout(this);
    /* Stop listening for resume-of-charging detection timeout */
    timerEventGenerator->RemoveListenerForResumeChargingDetectionTimeout(this);

    /* update pulse charging status */
     mIsPulseChargingOn = true;

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
}

void AutoChargingManagerPulseTopoff::startMaintenancePulseCharging()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    pm_chg_pulse_chg_config_type   pulse_config;
    pulse_config.p_type = CHG_MAINT_PULSE_TYPE;
    pulse_config.t_on   = CHG_MAINT_PULSE_TON;
    pulse_config.t_off  = CHG_MAINT_PULSE_TOFF;
    pulse_config.t_done = CHG_MAINT_PULSE_TDONE;

    /* Configure the charger transistor max voltage (VMAXSEL) and 
     charger transistor max current(IMAXSEL). */
    err |= pm_chg_vmaxsel_set(AUTO_CHARGING_PULSE_VMAXSEL);
    err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_MAXIMUM);

    /* Configure pulse charger type and timers. */
    err |= pm_chg_pulse_charger_config(&pulse_config);

    /* Configure vbatdet. */
    err |= pm_chg_vbatdet_set(AUTO_CHARGING_PULSE_VBATDET);

    /* Disable autonomous charging */
    err |= pm_chg_auto_disable(TRUE);

    /* Enable pulse charging. */
    err |= pm_chg_pulse_charger_switch(PM_ON_CMD);

    /* Add itself as a listener for PMIC PulseDone IRQ. */
    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    err |= irqEventGenerator->AddListenerForPulseTDoneIrq(this);
    /* Add itself as a listerner for PMIC battery alarm IRQ */
    err |= irqEventGenerator->AddListenerForBatteryAlarmIrq(this,CHG_VMAX_TOPOFF, CHG_VMIN_TOPOFF);

    /* update pulse charging status */
     mIsPulseChargingOn = true;

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
}

void AutoChargingManagerPulseTopoff::stopPulseCharging()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Disable Pulse charging. */
    err = pm_chg_pulse_charger_switch(PM_OFF_CMD);

    /* Enable autonomous charging */
    err |= pm_chg_auto_disable(FALSE);

    /* Remove itself as a listener for PMIC PulseDone IRQ. */
    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    irqEventGenerator->RemoveListenerForPulseTDoneIrq(this);
    (void)irqEventGenerator->RemoveListenerForBatteryAlarmIrq(this);
    /* Resume listening for VBATDET IRQ */
    err |= irqEventGenerator->AddListenerForVbatdetIrq(this);

    /* Reset pulse charging status */
     mIsPulseChargingOn = false;
     mVBatteryAlarmIrqTriggered = false;

     /* Find out if there was an PMIC API error. */
     if (err != PM_ERR_FLAG__SUCCESS) 
     {
         MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
     }
}

void AutoChargingManagerPulseTopoff::stopAutoCharging()
{
    /* Stop pulse charging */
    stopPulseCharging();

    AutoChargingManager::stopAutoCharging();
}

void AutoChargingManagerPulseTopoff::getCurrentState()
{
    AutoChargingManager::getCurrentState();

    /* Update current state */
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    err |= pm_chg_charge_state_get(&mCurrentState);

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }

    /* Update UI status */
    switch(mCurrentState)
    {
        case CHG_AUTO__PWR_ON_CHGR_ST: /* state 1 */ 
        {
            /* Pulse charging is on going */
            if(true == mIsPulseChargingOn)
            {
                /* Check the real time status of Pulse charging done interrupt */
                boolean             rt_status;

                err |= pm_get_rt_status(PM_PULSE_CHG_DONE_RT_ST, &rt_status);

                if(rt_status)/* battery is nearly full */
                {
                    /* Set charging-complete flag */
                    mIsChargingComplete = true;

                    /* Update general status */
                    mGeneralStatus.ui_event = CHG_UI_EVENT__DONE;
                    mGeneralStatus.is_charging = TRUE;
                    mGeneralStatus.is_battery_valid = TRUE;
                    mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
                    mGeneralStatus.battery_level = BATTERY_LEVEL__FULL;
                }
                else /* the battery is not full yet */
                {
                    /* Set charging-complete flag */
                    mIsChargingComplete = false;

                    /* Update general status */
                    mGeneralStatus.ui_event = CHG_UI_EVENT__NORMAL_POWER;
                    mGeneralStatus.is_charging = TRUE;
                    mGeneralStatus.is_battery_valid = TRUE;
                    mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
                    mGeneralStatus.battery_level = BATTERY_LEVEL__GOOD;
                }
            } /* end if it's pulse charging  */
        }
        break;
        default:
        {

        }
        break;
    } /* end switch() */
}

void AutoChargingManagerPulseTopoff::EventFired(ChargingEvent* event)
{
    ASSERT(event != NULL);

    AutoChargingManager::EventFired(event);

    switch( event->GetEventId() )
    {
        case CHG_CMD__IRQ_TRIGGERED:
        {
            IrqEvent* irqevent = dynamic_cast< IrqEvent* >( event );

            ASSERT( irqevent != NULL );

            pm_irq_hdl_type irqid = irqevent->GetIrqId();

            if(irqid == PM_VBAT_DET_IRQ_HDL) /* VBATDET has reached */
            {
                /* Only start pulse charging when the system is not in a call and high current charging source */
                if( (false == mIsPulseChargingOn)             &&
                    (FALSE == chg_system_tx_informaton.tx_on) &&
                    ( (CHARGER_TYPE__WALL == mGeneralStatus.charger_hardware_type) ||
                      (CHARGER_TYPE__USB_WALL == mGeneralStatus.charger_hardware_type) ) )
                {
                    /* Start Topoff pulse charging */
                    startTopoffPulseCharging();
                }
            } /* PM_VBAT_DET_IRQ_HDL */
            else if(irqid == PM_PULSE_CHG_DONE_IRQ_HDL) /* pulse charging has done */
            {
                /* Go to / stay in maintenance state */
                if(true == mIsPulseChargingOn)
                {
                    /* Remove itself as a listener for PMIC PulseDone IRQ. */
                    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                    irqEventGenerator->RemoveListenerForPulseTDoneIrq(this);

                    /* start maintenance pulse charging */
                    startMaintenancePulseCharging();
                }
            } /* PM_PULSE_CHG_DONE_IRQ_HDL */
            else if(irqid == PM_BAT_STAT_CHANGED_IRQ_HDL) /* battery status has changed */
            {
                mVBatteryAlarmIrqTriggered = true;

                /* Get battery alarm status */
                IrqEventGenerator *irqeventgenerator = ChargingFactory::GetIrqEventGenerator();
                chg_vbat_threshold_type  status = irqeventgenerator->GetBatteryAlarmStatus();

                /* Battery voltage has dropped below the lower threshold. */
                /* Or the battery voltage has gone above the upper threshold */
                /* In both cases, the pulse charging should be stopped */
                if( (status == CHG_VBAT_TH_LOWER) || (status == CHG_VBAT_TH_UPPER))
                {
                    /* battery voltage has dropped, stop doing pulse charging */
                    /* and turn on autonomous charging */
                    if(true == mIsPulseChargingOn)
                    {
                        stopPulseCharging();
                    }
                }
            } /* PM_BAT_STAT_CHANGED_IRQ_HDL */
            else if(irqid == PM_BATTTEMP_IRQ_HDL) /* Battery temp has gone out of range */
            {
                if( (true == mIsPulseChargingOn) &&
                    (BATTERY_STATUS__BAD_TEMP == mGeneralStatus.battery_status) )
                {
                    /* Stop pulse charging when the temp is out-of-range */
                    stopPulseCharging();
                }
            } /* PM_BATTTEMP_IRQ_HDL */
            else if( irqid == PM_BATTCONNECT_IRQ_HDL ) /* Battery connected/removed */
            {
                if( (true == mIsPulseChargingOn) &&
                    (BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status) )
                {
                    /* Stop pulse charging when the temp is out-of-range */
                    stopPulseCharging();
                }
            }
        } /* case CHG_CMD__IRQ_TRIGGERED */
        break;
        case CHG_CMD__TIMER_EXPIRED:
        {
            TimerEvent* timerevent = dynamic_cast< TimerEvent* >( event );

            ASSERT( timerevent != NULL );

            unsigned int  firstdelay = timerevent->GetFirstDelay();
            
            /* Charger state machine heart beat */
            if(firstdelay == CHG_HEART_BEAT_FIRST_TIME )
            {
                /* Update current state */
                getCurrentState();

                /* Keep listening to the BATT_STATUS IRQ */
                if(true == mVBatteryAlarmIrqTriggered)
                {
                    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                    (void)irqEventGenerator->RemoveListenerForBatteryAlarmIrq(this);
                    (void)irqEventGenerator->AddListenerForBatteryAlarmIrq(this, CHG_VMAX_TOPOFF, CHG_VMIN_TOPOFF);
                     mVBatteryAlarmIrqTriggered = false;
                }
            } /* end CHG_HEART_BEAT */
        }
        break; /* case CHG_CMD__TIMER_EXPIRED */
        case CHG_CMD__SYSTEM_STATUS_CHANGED:
        {
            SystemEvent* systemevent = dynamic_cast< SystemEvent* >( event );

            ASSERT( systemevent != NULL );

            chg_system_event_id_type  systemeventid = systemevent->GetSystemEventId();

            /* System is in a call */
            if(systemeventid == CHG_SYSTEM__TX_ON) 
            {
                if(true == mIsPulseChargingOn)
                {
                    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

                    stopPulseCharging();

                    /* Reduce the IMAXSEL to avoid possible degrading of the RF performance */
                    err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_TX_ON); 

                    /* Find out if there was an PMIC API error. */
                    if (err != PM_ERR_FLAG__SUCCESS) 
                    {
                        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                    }
                }
            } /* CHG_SYSTEM__TX_ON */
        } /* end CHG_CMD__SYSTEM_STATUS_CHANGED */
        break;
    } /* switch() */
} /* EventFired() */

                  /******************************************************/
                  /*     class Debug_AutoChargingManagerPulseTopoff     */
                  /******************************************************/

Debug_AutoChargingManagerPulseTopoff::Debug_AutoChargingManagerPulseTopoff()
{
    /* Initialize debug information */
    mAutoChargingInformation.battery_temperature = 0;
    mAutoChargingInformation.battery_voltage = 0;
    mAutoChargingInformation.charging_current = 0;
    mAutoChargingInformation.current_state = CHG_AUTO__INIT_ST;
    mAutoChargingInformation.previous_state = CHG_AUTO__INIT_ST;
    mAutoChargingInformation.charger_type = CHARGER_TYPE__INVALID;
    mAutoChargingInformation.battery_connect_irq_triggered = FALSE;
    mAutoChargingInformation.bad_battery_temp_irq_triggered = FALSE;
    mAutoChargingInformation.change_of_state_irq_triggered = FALSE;
    mAutoChargingInformation.end_of_charging_irq_triggered = FALSE;
    mAutoChargingInformation.fail_of_charging_irq_triggered = FALSE;
    mAutoChargingInformation.vbat_det_irq_triggered = FALSE;
    mAutoChargingInformation.pulse_done_irq_triggered = FALSE;
    mAutoChargingInformation.batt_alarm_irq_triggered = FALSE;
    mAutoChargingInformation.charger_irq_triggered = FALSE;
    mAutoChargingInformation.chgtlimit_irq_triggered = FALSE;
    mAutoChargingInformation.vcp_irq_triggered = FALSE;
    mAutoChargingInformation.battery_removed = FALSE;
    mAutoChargingInformation.battery_bad = FALSE;
    mAutoChargingInformation.pulse_charging_on = FALSE;
    mAutoChargingInformation.cv_topoff_on = FALSE;
    mAutoChargingInformation.charging_finished = FALSE;
    mAutoChargingInformation.charging_in_final_cycle = FALSE;
    mAutoChargingInformation.TX_on = FALSE;
}

Debug_AutoChargingManagerPulseTopoff::~Debug_AutoChargingManagerPulseTopoff()
{

}

void Debug_AutoChargingManagerPulseTopoff::updateDebugInformation()
{
    /* Update Battery Voltage and battery temperature. */
    #ifdef FEATURE_VBATT_TASK /* FEATURE_VBATT_TASK */ 
    mAutoChargingInformation.battery_voltage     = vbatt_read_mv();

    mAutoChargingInformation.battery_temperature = vbatt_read_temperature();
    #else
    mAutoChargingInformation.battery_voltage     = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);

    mAutoChargingInformation.battery_temperature = chg_get_parameter_level(CHG_PARAMETER__BATT_THERM_DEGC);
    #endif

    /* Update charging current */
    mAutoChargingInformation.charging_current = chg_get_parameter_level(CHG_PARAMETER__CHARGER_I);

    /* Update charging state */
    mAutoChargingInformation.current_state = mCurrentState;

    if(true == mIsChargingFailed)
    {
        mAutoChargingInformation.battery_bad = TRUE;
    }
    else
    {
        if( BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status)
        {
            mAutoChargingInformation.battery_removed = TRUE;
        }
        else
        {
            mAutoChargingInformation.battery_removed = FALSE;
        }

        mAutoChargingInformation.battery_bad = FALSE;
    }

    if(TRUE == chg_system_tx_informaton.tx_on)
    {
        mAutoChargingInformation.TX_on = TRUE;
    }
    else
    {
        mAutoChargingInformation.TX_on = FALSE;
    }

    if(true == mIsInFinalCycle)
    {
        mAutoChargingInformation.charging_in_final_cycle = TRUE;
    }
    else
    {
        mAutoChargingInformation.charging_in_final_cycle = FALSE;
    }

    if(true == mIsChargingComplete)
    {
        mAutoChargingInformation.charging_finished = TRUE;
    }
    else
    {
        mAutoChargingInformation.charging_finished = FALSE;
    }

    if(true == mIsPulseChargingOn)
    {
        mAutoChargingInformation.pulse_charging_on = TRUE;
    }
    else
    {
        mAutoChargingInformation.pulse_charging_on = FALSE;
    }

}

void Debug_AutoChargingManagerPulseTopoff::EventFired(ChargingEvent* event)
{
    ASSERT(event != NULL);

    AutoChargingManagerPulseTopoff::EventFired(event);

    switch( event->GetEventId() )
    {
        case CHG_CMD__IRQ_TRIGGERED:
        {
            IrqEvent* irqevent = dynamic_cast< IrqEvent* >( event );

            ASSERT( irqevent != NULL );

            pm_irq_hdl_type irqid = irqevent->GetIrqId();

            if(irqid == PM_VALID_CHG_IRQ_HDL) /* A charging source is attached */
            {
                MSG_HIGH(" chg_debug: Regular Wall charger has plugged in", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if(irqid == PM_INVALID_CHG_IRQ_HDL ) /* A charging source is removed */
            {
                MSG_HIGH(" chg_debug: Regular Wall charger is removed", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if( irqid == PM_BATTCONNECT_IRQ_HDL ) /* Battery connected/removed */
            {
                if(BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status)
                {
                    MSG_HIGH(" chg_debug: Battery is removed", 0, 0, 0);

                    /* Update debug info */
                    mAutoChargingInformation.battery_removed = TRUE;

                    if(TRUE == mAutoChargingInformation.pulse_charging_on)
                    {
                        MSG_HIGH(" chg_debug: stop pulse charging", 0, 0, 0);

                        mAutoChargingInformation.pulse_charging_on = FALSE;
                    }
                }
                else
                {
                    MSG_HIGH(" chg_debug: Battery is connected", 0, 0, 0);

                    /* Update debug info */
                    mAutoChargingInformation.battery_removed = FALSE;
                }

                /* Update debug information */
                mAutoChargingInformation.battery_connect_irq_triggered = TRUE;
            }
            else if(irqid == PM_CHGSTATE_IRQ_HDL) /* Charging state has changed */
            {
                MSG_HIGH(" chg_debug: charging state changed", 0, 0, 0);

                /* Update previous state */
                mAutoChargingInformation.previous_state = mAutoChargingInformation.current_state;

                /* Update debug information */
                mAutoChargingInformation.change_of_state_irq_triggered = TRUE;
                mAutoChargingInformation.current_state = mCurrentState;
            }
            else if( irqid == PM_CHGDONE_IRQ_HDL ) /* Charging has finished successfully */
            {
                MSG_HIGH(" chg_debug: charging-done IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.end_of_charging_irq_triggered = TRUE;
            }
            else if(irqid == PM_CHGFAIL_IRQ_HDL) /* Charging has failed */
            {
                MSG_HIGH(" chg_debug: charging-fail IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.fail_of_charging_irq_triggered = TRUE;
            }
            else if(irqid == PM_BATTTEMP_IRQ_HDL) /* Battery temp has gone out of range */
            {
                /*check for battery temperature only when valid battery is connected*/
                if(BATTERY_STATUS__REMOVED != mGeneralStatus.battery_status)
                {
                    MSG_HIGH(" chg_debug: bad-batt-temp IRQ triggered", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.bad_battery_temp_irq_triggered = TRUE;

                    if(TRUE == mAutoChargingInformation.pulse_charging_on)
                    {
                        MSG_HIGH(" chg_debug: stop pulse charging", 0, 0, 0);

                        mAutoChargingInformation.pulse_charging_on = FALSE;
                    }
                }
            }
            else if(irqid == PM_VBAT_DET_IRQ_HDL) /* VBATDET has reached */
            {
                MSG_HIGH(" chg_debug: vbatdet IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.vbat_det_irq_triggered = TRUE;

                if(true == mIsPulseChargingOn)
                {
                    MSG_HIGH(" chg_debug: start pulse charging", 0, 0, 0);

                    mAutoChargingInformation.pulse_charging_on = TRUE;
                }

            }
            else if(irqid == PM_FASTCHG_IRQ_HDL) /* in fast charging */
            {
                MSG_HIGH(" chg_debug: in-fast-charging IRQ triggered", 0, 0, 0);
            }
            else if(irqid == PM_PULSE_CHG_DONE_IRQ_HDL) /* pulse charging has done */
            {
                MSG_HIGH(" chg_debug: PulseDone IRQ triggered", 0, 0, 0);

                mAutoChargingInformation.pulse_done_irq_triggered = TRUE;

            } /* PM_PULSE_CHG_DONE_IRQ_HDL */
            else if(irqid == PM_BAT_STAT_CHANGED_IRQ_HDL) /* battery status has changed */
            {
                MSG_HIGH(" chg_debug: battery status changed IRQ triggered", 0, 0, 0);

                mAutoChargingInformation.batt_alarm_irq_triggered = TRUE;

                if(TRUE == mAutoChargingInformation.pulse_charging_on)
                {
                    MSG_HIGH(" chg_debug: stop pulse charging", 0, 0, 0);

                    mAutoChargingInformation.pulse_charging_on = FALSE;
                }
            } /* PM_BAT_STAT_CHANGED_IRQ_HDL */
        } /* case CHG_CMD__IRQ_TRIGGERED */
        break;
        case CHG_CMD__TIMER_EXPIRED:
        {
            TimerEvent* timerevent = dynamic_cast< TimerEvent* >( event );

            ASSERT( timerevent != NULL );

            unsigned int  firstdelay = timerevent->GetFirstDelay();
            
            /* Charger state machine heart beat */
            if(firstdelay == CHG_HEART_BEAT_FIRST_TIME )
            {
                 /* Update debug information */
                 updateDebugInformation();

                 /* Fire debug event */
                 ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                 FireEvent(pEvent);
                 /* Release memory */
                 delete pEvent;

                 /* Reset debug information */
                 mAutoChargingInformation.battery_connect_irq_triggered = FALSE;
                 mAutoChargingInformation.bad_battery_temp_irq_triggered = FALSE;
                 mAutoChargingInformation.change_of_state_irq_triggered = FALSE;
                 mAutoChargingInformation.end_of_charging_irq_triggered = FALSE;
                 mAutoChargingInformation.fail_of_charging_irq_triggered = FALSE;
                 mAutoChargingInformation.vbat_det_irq_triggered = FALSE;
                 mAutoChargingInformation.charger_irq_triggered = FALSE;
                 mAutoChargingInformation.pulse_done_irq_triggered = FALSE;
                 mAutoChargingInformation.batt_alarm_irq_triggered = FALSE;
            } /* end CHG_HEART_BEAT */
            /* End-of-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_END_CHARGE_DETECTION_T )
            {
                MSG_HIGH(" chg_debug: end-of-charging detection timer expired", 0, 0, 0);

                if(true == mIsInFinalCycle)
                {
                    mAutoChargingInformation.charging_in_final_cycle = TRUE;
                }
                else
                {
                    mAutoChargingInformation.charging_in_final_cycle = FALSE;
                }

            }/* end AUTO_CHARGING_END_CHARGE_DETECTION_T */
            /* Resume-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_RESUME_CHARGE_DETECTION_T )
            {
                MSG_HIGH(" chg_debug: resume-charge detection timer expired", 0, 0, 0);
            } /* end AUTO_CHARGING_RESUME_CHARGE_DETECTION_T */
            /* max-charging-time detection times out */
            else if(firstdelay == (AUTO_CHARGING_MAX_CHARGE_T*60000) )
            {
                MSG_HIGH(" chg_debug: max-charging-time detection timer expired", 0, 0, 0);
            } /* end AUTO_CHARGING_MAX_CHARGE_T */
        }
        break; /* case CHG_CMD__TIMER_EXPIRED */
        case CHG_CMD__SYSTEM_STATUS_CHANGED:
        {
            SystemEvent* systemevent = dynamic_cast< SystemEvent* >( event );

            ASSERT( systemevent != NULL );

            chg_system_event_id_type  systemeventid = systemevent->GetSystemEventId();

            if(systemeventid == CHG_SYSTEM__USB_VALID) /* A USB source is attached */
            {
                MSG_HIGH(" chg_debug: Valid USB device is connected", 0, 0, 0);

                /* A PC-USB/ SHP just attached */
                if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_HOST)
                {
                    MSG_HIGH(" chg_debug: a PC-USB/ SHP charger has plugged in", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.charger_irq_triggered = TRUE;
                    mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;
                }
                else if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_CHARGER)
                {
                    MSG_HIGH(" chg_debug: a Wall-USB/UCP charger has plugged in", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.charger_irq_triggered = TRUE;
                    mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;
                }

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if(systemeventid == CHG_SYSTEM__USB_INVALID) /* A USB source is removed */
            {
                MSG_HIGH(" chg_debug: The USB device has removed", 0, 0, 0);

                /* Stop autonomous charging */
                stopAutoCharging();

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            /* USB device is in suspend mode */
            else if(systemeventid == CHG_SYSTEM__USB_SUSPEND) 
            {
                MSG_HIGH(" chg_debug: In USB suspend state", 0, 0, 0);
            }
            /* System is in a call */
            else if(systemeventid == CHG_SYSTEM__TX_ON) 
            {
                MSG_HIGH(" chg_debug: In TX ON state", 0, 0, 0);

                mAutoChargingInformation.TX_on = TRUE;

                if(TRUE == mAutoChargingInformation.pulse_charging_on )
                {
                    MSG_HIGH(" chg_debug: stop pulse charging", 0, 0, 0);

                    mAutoChargingInformation.pulse_charging_on = FALSE;
                }
            } /* CHG_SYSTEM__TX_ON */
            /* System is NOT in a call anymore */
            else if(systemeventid == CHG_SYSTEM__TX_OFF) 
            {
                MSG_HIGH(" chg_debug: In TX OFF state", 0, 0, 0);

                mAutoChargingInformation.TX_on = FALSE;
            } /* CHG_SYSTEM__TX_OFF */

        } /* end CHG_CMD__SYSTEM_STATUS_CHANGED */
        break;
    } /* switch() */
} /* EventFired() */

#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF */

#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)

                  /*********************************************/
                  /*     class AutoChargingManagerCVTopoff     */
                  /*********************************************/

AutoChargingManagerCVTopoff::AutoChargingManagerCVTopoff():
    mIsCVTopoffOn(false),
    mVBatteryAlarmIrqTriggered(false),
    mVcpIrqTriggered(false),
    mChgTLimitIrqTriggered(false),
    mEndCVTopoffCounter(0)
{
    
}

AutoChargingManagerCVTopoff::~AutoChargingManagerCVTopoff()
{

}

void AutoChargingManagerCVTopoff::startCVTopoff()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Disable autonomous charging */
    err |= pm_chg_auto_disable(TRUE);

    /* Enable charger PNP. */
    err |= pm_chg_charge_disable(FALSE);

    /* Enable battery FET */
    err |= pm_chg_charge_bat_enable(TRUE);

    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
    /* Stop listening for end-of-charging detection timeout */
    timerEventGenerator->RemoveListenerForEndOfChargingDetectionTimeout(this);
    /* Stop listening for resume-of-charging detection timeout */
    timerEventGenerator->RemoveListenerForResumeChargingDetectionTimeout(this);
    /* Start listening for end-of-CV-charging detection timeout */
    timerEventGenerator->AddListenerForEndOfCVTopoffDetection1Timeout(this);

    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    /* Add itself as a listener for PMIC VCP IRQ. */
    err |= irqEventGenerator->AddListenerForVcpIrq(this);
    /* Add itself as a listener for PMIC CHGTLIMIT IRQ. */
    err |= irqEventGenerator->AddListenerForChgTLimitIrq(this);
    /* Add itself as a listerner for PMIC battery alarm IRQ */
    err |= irqEventGenerator->AddListenerForBatteryAlarmIrq(this,CHG_VMAX_TOPOFF, CHG_VMIN_TOPOFF);

    /* update CV topoff status */
     mIsCVTopoffOn = true;

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
}

void AutoChargingManagerCVTopoff::stopCVTopoff()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Enable autonomous charging */
    err |= pm_chg_auto_disable(FALSE);

    /* Remove itself as a listener for PMIC IRQs. */
    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
    /* Remove itself as a listener for PMIC VCP IRQ. */
    irqEventGenerator->RemoveListenerForVcpIrq(this);
    /* Remove itself as a listener for PMIC CHGTLIMIT IRQ. */
    irqEventGenerator->RemoveListenerForChgTLimitIrq(this);
    /* Remove itself as a listerner for PMIC battery alarm IRQ */
    err |= irqEventGenerator->RemoveListenerForBatteryAlarmIrq(this);
    /* Resume listening for VBATDET IRQ */
    err |= irqEventGenerator->AddListenerForVbatdetIrq(this);

    TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
    /* Remove listening for end-of-CV-chraging detection timeouts */
    timerEventGenerator->RemoveListenerForEndOfCVTopoffDetection1Timeout(this);
    timerEventGenerator->RemoveListenerForEndOfCVTopoffDetection2Timeout(this);

    /* update CV topoff status */
     mIsCVTopoffOn = false;
     mVcpIrqTriggered = false;
     mChgTLimitIrqTriggered = false;
     mEndCVTopoffCounter = false;

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }
}

void AutoChargingManagerCVTopoff::stopAutoCharging()
{
    /* Stop CV Topoff */
    stopCVTopoff();

    AutoChargingManager::stopAutoCharging();
}


void AutoChargingManagerCVTopoff::endCVTopoffDetection1()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Used to take the average of the measurements */
    char ichg_counter = 0;
    /* Used to save the one-time current measurement 
       before openning the battery FET */
    short ichg_prev = 0;
    /* Used to save the one-time current measurement 
       after openning the battery FET */
    short ichg_pro = 0;
    /* Used to save the sum of current measurements 
       before openning the battery FET */
    int ichg_prev_avg = 0;
    /* Used to save the sum of current measurements 
       after openning the battery FET */
    int ichg_pro_avg = 0;
    /* Used the save the difference of the charging current
       before and after oppening the battery FET */
    short ichg_diff = 0;
    /* Used to save the RT status of the VCP IRQ */
    bool vcp_flag = false;
    /* Used to save the RT status of the CHGTLIMIT IRQ */
    bool tlimit_flag = false;

    #ifdef T_WINNT
#error code not present
    #endif

    /* Measure the charging current before openning the battery FET */
    while(ichg_counter < AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX)
    {
        /* Measure charging current */
        ichg_prev = chg_get_parameter_level(CHG_PARAMETER__CHARGER_I);
        /* Accumulate the measurements */
        ichg_prev_avg = ichg_prev_avg + ichg_prev;
        ichg_counter ++;
    }

    /* Take the average of the measured current.*/
    ichg_prev = (short)(ichg_prev_avg / AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX);

    /* Reset the counter */
    ichg_counter = 0;

    /* Open the battery FET */
    err |= pm_chg_charge_bat_enable(FALSE);

    #ifdef T_WINNT
#error code not present
    #endif

    /* Halt the CPU for 300 usec. */
    clk_busy_wait(AUTO_CHARGING_END_CV_TOPOFF_VCP_CHECK_WAIT);

    boolean             rt_status;

    /* Check the RT status of VCP IRQ */
    err |= pm_get_rt_status(PM_VCP_IRQ_RT_ST, &rt_status);

    if(rt_status)/* VCP triggered */
    {
        /* Reset the VCP IRQ */
        IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
        irqEventGenerator->RemoveListenerForVcpIrq(this);
        err |= irqEventGenerator->AddListenerForVcpIrq(this);
    }

    /* Measure the charging current after openning the battery FET */
    while(ichg_counter < AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX)
    {
        /* Measure charging current */
        ichg_pro = chg_get_parameter_level(CHG_PARAMETER__CHARGER_I);
        /* Accumulate the measurements */
        ichg_pro_avg = ichg_pro_avg + ichg_pro;
        ichg_counter ++;
    }

    /* Take the average of the measured current.*/
    ichg_pro = (short)(ichg_pro_avg / AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX);

    /* Reset the counter */
    ichg_counter  = 0;

    /* Check the RT status of VCP IRQ */
    err |= pm_get_rt_status(PM_VCP_IRQ_RT_ST, &rt_status);

    if(rt_status)/*VCP triggered */
    {
        vcp_flag = true;
    }

    /* Check the RT status of CHGTLIMIT IRQ */
    err |= pm_get_rt_status(PM_CHGTLIMIT_RT_ST, &rt_status);

    if(rt_status)/*VCP triggered */
    {
        tlimit_flag = true;
    }

    /* Close the battery FET */
    err |= pm_chg_charge_bat_enable(TRUE);

    /* Check if the CV topoff is ending */
    if((ichg_prev > ichg_pro) &&  /* Correct measurements */
        (false == vcp_flag)    &&  /* VCP is not triggered */
        (false == tlimit_flag))     /* charger is not in temperature limiting */
    {
        ichg_diff = ichg_prev - ichg_pro;

        /* CV charging is ending */
        if(ichg_diff < AUTO_CHARGING_END_CV_TOPOFF_I_DELTA)
        {
            TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
            /* Stop listening for end-of-CV-charging detection timeout #1 */
            timerEventGenerator->RemoveListenerForEndOfCVTopoffDetection1Timeout(this);
            /* Start listening for end-of-CV-charging detection timeout #2 */
            timerEventGenerator->AddListenerForEndOfCVTopoffDetection2Timeout(this);
        }
    }

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }

}

void AutoChargingManagerCVTopoff::endCVTopoffDetection2()
{
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    /* Used to take the average of the measurements */
    char ichg_counter = 0;
    /* Used to save the one-time current measurement 
       before openning the battery FET */
    short ichg_prev = 0;
    /* Used to save the one-time current measurement 
       after openning the battery FET */
    short ichg_pro = 0;
    /* Used to save the sum of current measurements 
       before openning the battery FET */
    int ichg_prev_avg = 0;
    /* Used to save the sum of current measurements 
       after openning the battery FET */
    int ichg_pro_avg = 0;
    /* Used the save the difference of the charging current
       before and after oppening the battery FET */
    short ichg_diff = 0;
    /* Used to save the RT status of the VCP IRQ */
    bool vcp_flag = false;
    /* Used to save the RT status of the CHGTLIMIT IRQ */
    bool tlimit_flag = false;

    #ifdef T_WINNT
#error code not present
    #endif

    /* Measure the charging current before openning the battery FET */
    while(ichg_counter < AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX)
    {
        /* Measure charging current */
        ichg_prev = chg_get_parameter_level(CHG_PARAMETER__CHARGER_I);
        /* Accumulate the measurements */
        ichg_prev_avg = ichg_prev_avg + ichg_prev;
        ichg_counter ++;
    }

    /* Take the average of the measured current.*/
    ichg_prev = (short)(ichg_prev_avg / AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX);

    /* Reset the counter */
    ichg_counter = 0;

    /* Open the battery FET */
    err |= pm_chg_charge_bat_enable(FALSE);

    #ifdef T_WINNT
#error code not present
    #endif

    /* Halt the CPU for 300 usec. */
    clk_busy_wait(AUTO_CHARGING_END_CV_TOPOFF_VCP_CHECK_WAIT);

    boolean             rt_status;

    /* Check the RT status of VCP IRQ */
    err |= pm_get_rt_status(PM_VCP_IRQ_RT_ST, &rt_status);

    if(rt_status)/*VCP triggered */
    {
        /* Reset the VCP IRQ */
        IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
        irqEventGenerator->RemoveListenerForVcpIrq(this);
        err |= irqEventGenerator->AddListenerForVcpIrq(this);
    }

    /* Measure the charging current after openning the battery FET */
    while(ichg_counter < AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX)
    {
        /* Measure charging current */
        ichg_pro = chg_get_parameter_level(CHG_PARAMETER__CHARGER_I);
        /* Accumulate the measurements */
        ichg_pro_avg = ichg_pro_avg + ichg_pro;
        ichg_counter ++;
    }

    /* Take the average of the measured current.*/
    ichg_pro = (short)(ichg_pro_avg / AUTO_CHARGING_END_CV_TOPOFF_I_READ_COUNTER_MAX);

    /* Reset the counter */
    ichg_counter  = 0;

    /* Check the RT status of VCP IRQ */
    err |= pm_get_rt_status(PM_VCP_IRQ_RT_ST, &rt_status);

    if(rt_status)/*VCP triggered */
    {
        vcp_flag = true;
    }

    /* Check the RT status of CHGTLIMIT IRQ */
    err |= pm_get_rt_status(PM_CHGTLIMIT_RT_ST, &rt_status);

    if(rt_status)/*VCP triggered */
    {
        tlimit_flag = true;
    }

    /* Close the battery FET */
    err |= pm_chg_charge_bat_enable(TRUE);

    /* Check if the CV topoff is complete */
    if((ichg_prev > ichg_pro) &&  /* Correct measurements */
        (false == vcp_flag)    &&  /* VCP is not triggered */
        (false == tlimit_flag))    /* charger is not in temperature limiting */
    {
        ichg_diff = ichg_prev - ichg_pro;

        /* CV charging is ending */
        if(ichg_diff < AUTO_CHARGING_END_CV_TOPOFF_I_DELTA)
        {
            /* Update the counter */
            mEndCVTopoffCounter++;

            /* CV topoff is truly completed. */
            if(AUTO_CHARGING_END_CV_TOPOFF_DETECTION_COUNTER_MAX == mEndCVTopoffCounter)
            {
                /* Reset the counter */
                mEndCVTopoffCounter = 0;

                /* Update the charging status */
                mIsChargingComplete = true;

                TimerEventGenerator *timerEventGenerator = ChargingFactory::GetTimerEventGenerator();
                /* Stop listening for end-of-CV-charging detection timeout #2 */
                timerEventGenerator->RemoveListenerForEndOfCVTopoffDetection2Timeout(this);
            }
        }
        else
        {
            /* Reset the counter */
            mEndCVTopoffCounter = 0;
        }/* if(ichg_diff < AUTO_CHARGING_END_CV_TOPOFF_I_DELTA) */
    }
    else
    {
        /* Reset the counter */
        mEndCVTopoffCounter = 0;
    } /* if( (ichg_prev > ichg_pro) && (false == vcp_flag) ) */

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }

}

void AutoChargingManagerCVTopoff::getCurrentState()
{
    AutoChargingManager::getCurrentState();

    /* Update current state */
    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

    err |= pm_chg_charge_state_get(&mCurrentState);

    /* Find out if there was an PMIC API error. */
    if (err != PM_ERR_FLAG__SUCCESS) 
    {
        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
    }

    /* Update UI status */
    switch(mCurrentState)
    {
        case CHG_AUTO__PWR_ON_CHGR_ST: /* state 1 */ 
        {
            /* CV topoff is on going */
            if(true == mIsCVTopoffOn)
            {
                if(true == mIsChargingComplete)
                {
                    /* Update general status */
                    mGeneralStatus.ui_event = CHG_UI_EVENT__DONE;
                    mGeneralStatus.is_charging = TRUE;
                    mGeneralStatus.is_battery_valid = TRUE;
                    mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
                    mGeneralStatus.battery_level = BATTERY_LEVEL__FULL;
                }
                else
                {
                    /* Update general status */
                    mGeneralStatus.ui_event = CHG_UI_EVENT__NORMAL_POWER;
                    mGeneralStatus.is_charging = TRUE;
                    mGeneralStatus.is_battery_valid = TRUE;
                    mGeneralStatus.battery_status = BATTERY_STATUS__GOOD;
                    mGeneralStatus.battery_level = BATTERY_LEVEL__GOOD;
                }
            } /* end if it's CV topoff */
        }
        break;
        default:
        {

        }
        break;
    } /* end switch() */
}

void AutoChargingManagerCVTopoff::EventFired(ChargingEvent* event)
{
    ASSERT(event != NULL);

    AutoChargingManager::EventFired(event);

    switch( event->GetEventId() )
    {
        case CHG_CMD__IRQ_TRIGGERED:
        {
            IrqEvent* irqevent = dynamic_cast< IrqEvent* >( event );

            ASSERT( irqevent != NULL );

            pm_irq_hdl_type irqid = irqevent->GetIrqId();

            if(irqid == PM_VBAT_DET_IRQ_HDL) /* VBATDET has reached */
            {
                /* Only start CV Topoff when the system is not in a call */
                if((false == mIsCVTopoffOn)             &&
                   (FALSE == chg_system_tx_informaton.tx_on))
                {
                    /* Start Topoff pulse charging */
                    startCVTopoff();
                }
            } /* PM_VBAT_DET_IRQ_HDL */
            else if(irqid == PM_VCP_IRQ_HDL) /* VCP */
            {
                mVcpIrqTriggered = true;
            } /* PM_VCP_IRQ_HDL */
            else if(irqid == PM_CHGTLIMIT_IRQ_HDL) /* CHGTLIMIT */
            {
                mChgTLimitIrqTriggered = true;
            } /* PM_CHGTLIMIT_IRQ_HDL */
            else if(irqid == PM_BAT_STAT_CHANGED_IRQ_HDL) /* battery status has changed */
            {
                mVBatteryAlarmIrqTriggered = true;

                /* Get battery alarm status */
                IrqEventGenerator *irqeventgenerator = ChargingFactory::GetIrqEventGenerator();
                chg_vbat_threshold_type  status = irqeventgenerator->GetBatteryAlarmStatus();

                /* Battery voltage has dropped below the lower threshold. */
                /* Or the battery voltage has gone above the upper threshold */
                /* In both cases, the CV topoff should be stopped */
                if( (status == CHG_VBAT_TH_LOWER) || (status == CHG_VBAT_TH_UPPER))
                {
                    /* battery voltage has dropped, stop doing CV topoff */
                    /* and turn on autonomous charging */
                    if(true == mIsCVTopoffOn)
                    {
                        stopCVTopoff();
                    }
                }
            } /* PM_BAT_STAT_CHANGED_IRQ_HDL */
            else if(irqid == PM_BATTTEMP_IRQ_HDL) /* Battery temp has gone out of range */
            {
                if( (true == mIsCVTopoffOn) &&
                    (BATTERY_STATUS__BAD_TEMP == mGeneralStatus.battery_status) )
                {
                    /* Stop CV topoff when the temp is out-of-range */
                    stopCVTopoff();
                }
            } /* PM_BATTTEMP_IRQ_HDL */
            else if( irqid == PM_BATTCONNECT_IRQ_HDL ) /* Battery connected/removed */
            {
                if( (true == mIsCVTopoffOn) &&
                    (BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status) )
                {
                    /* Stop CV topoff when the temp is out-of-range */
                    stopCVTopoff();
                }
            }
        } /* case CHG_CMD__IRQ_TRIGGERED */
        break;
        case CHG_CMD__TIMER_EXPIRED:
        {
            TimerEvent* timerevent = dynamic_cast< TimerEvent* >( event );

            ASSERT( timerevent != NULL );

            unsigned int  firstdelay = timerevent->GetFirstDelay();
            
            /* Charger state machine heart beat */
            if(firstdelay == CHG_HEART_BEAT_FIRST_TIME )
            {
                /* Update current state */
                getCurrentState();

                /* Keep listening to the BATT_STATUS IRQ */
                if(true == mVBatteryAlarmIrqTriggered)
                {
                    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                    (void)irqEventGenerator->RemoveListenerForBatteryAlarmIrq(this);
                    (void)irqEventGenerator->AddListenerForBatteryAlarmIrq(this, CHG_VMAX_TOPOFF, CHG_VMIN_TOPOFF);
                     mVBatteryAlarmIrqTriggered = false;
                }
                /* Keep listening to the VCP IRQ */
                if(true == mVcpIrqTriggered)
                {
                    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                    irqEventGenerator->RemoveListenerForVcpIrq(this);
                    (void)irqEventGenerator->AddListenerForVcpIrq(this);
                }
                /* Keep listening to the CHGTLIMIT IRQ */
                if(true == mChgTLimitIrqTriggered)
                {
                    IrqEventGenerator *irqEventGenerator = ChargingFactory::GetIrqEventGenerator();
                    irqEventGenerator->RemoveListenerForChgTLimitIrq(this);
                    (void)irqEventGenerator->AddListenerForChgTLimitIrq(this);
                }
            } /* end CHG_HEART_BEAT */
            /* end-of-CV-topoff-detection timer #1 */
            if(firstdelay == AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T )
            {
                /* Only perform CV topoff detecton routine when the system is not in a call */
                if(FALSE == chg_system_tx_informaton.tx_on)
                {
                    endCVTopoffDetection1();
                }
            } /* end AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T */
            /* end-of-CV-topoff-detection timer #2 */
            if(firstdelay == AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T )
            {
                /* Only perform CV topoff detecton routine when the system is not in a call */
                if(FALSE == chg_system_tx_informaton.tx_on)
                {
                    endCVTopoffDetection2();
                }
            } /* end AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T */
        }
        break; /* case CHG_CMD__TIMER_EXPIRED */
        case CHG_CMD__SYSTEM_STATUS_CHANGED:
        {
            SystemEvent* systemevent = dynamic_cast< SystemEvent* >( event );

            ASSERT( systemevent != NULL );

            chg_system_event_id_type  systemeventid = systemevent->GetSystemEventId();

            /* System is in a call */
            if(systemeventid == CHG_SYSTEM__TX_ON) 
            {
                if(true == mIsCVTopoffOn)
                {
                    pm_err_flag_type  err = PM_ERR_FLAG__SUCCESS;

                    stopCVTopoff();

                    /* Reduce the IMAXSEL to avoid possible degrading of the RF performance */
                    err |= pm_chg_imaxsel_set(AUTO_CHARGING_UCP_IMAX_TX_ON); 

                    /* Find out if there was an PMIC API error. */
                    if (err != PM_ERR_FLAG__SUCCESS) 
                    {
                        MSG_ERROR("PMIC API ERROR(0x%x).",err,0,0);
                    }
                }
            } /* CHG_SYSTEM__TX_ON */
        } /* end CHG_CMD__SYSTEM_STATUS_CHANGED */
        break;
    } /* switch() */
} /* EventFired() */

                  /**************************************************/
                  /*     class Debug_AutoChargingManagerCVTopoff    */
                  /**************************************************/
Debug_AutoChargingManagerCVTopoff::Debug_AutoChargingManagerCVTopoff()
{
    /* Initialize debug information */
    mAutoChargingInformation.battery_temperature = 0;
    mAutoChargingInformation.battery_voltage = 0;
    mAutoChargingInformation.charging_current = 0;
    mAutoChargingInformation.current_state = CHG_AUTO__INIT_ST;
    mAutoChargingInformation.previous_state = CHG_AUTO__INIT_ST;
    mAutoChargingInformation.charger_type = CHARGER_TYPE__INVALID;
    mAutoChargingInformation.battery_connect_irq_triggered = FALSE;
    mAutoChargingInformation.bad_battery_temp_irq_triggered = FALSE;
    mAutoChargingInformation.change_of_state_irq_triggered = FALSE;
    mAutoChargingInformation.end_of_charging_irq_triggered = FALSE;
    mAutoChargingInformation.fail_of_charging_irq_triggered = FALSE;
    mAutoChargingInformation.vbat_det_irq_triggered = FALSE;
    mAutoChargingInformation.pulse_done_irq_triggered = FALSE;
    mAutoChargingInformation.batt_alarm_irq_triggered = FALSE;
    mAutoChargingInformation.charger_irq_triggered = FALSE;
    mAutoChargingInformation.chgtlimit_irq_triggered = FALSE;
    mAutoChargingInformation.vcp_irq_triggered = FALSE;
    mAutoChargingInformation.battery_removed = FALSE;
    mAutoChargingInformation.battery_bad = FALSE;
    mAutoChargingInformation.pulse_charging_on = FALSE;
    mAutoChargingInformation.cv_topoff_on = FALSE;
    mAutoChargingInformation.charging_finished = FALSE;
    mAutoChargingInformation.charging_in_final_cycle = FALSE;
    mAutoChargingInformation.TX_on = FALSE;
}

Debug_AutoChargingManagerCVTopoff::~Debug_AutoChargingManagerCVTopoff()
{

}

void Debug_AutoChargingManagerCVTopoff::updateDebugInformation()
{
    /* Update Battery Voltage and battery temperature. */
    #ifdef FEATURE_VBATT_TASK /* FEATURE_VBATT_TASK */ 
    mAutoChargingInformation.battery_voltage     = vbatt_read_mv();

    mAutoChargingInformation.battery_temperature = vbatt_read_temperature();
    #else
    mAutoChargingInformation.battery_voltage     = chg_get_parameter_level(CHG_PARAMETER__BATTERY_VOLT);

    mAutoChargingInformation.battery_temperature = chg_get_parameter_level(CHG_PARAMETER__BATT_THERM_DEGC);
    #endif

    /* Update charging current */
    mAutoChargingInformation.charging_current = chg_get_parameter_level(CHG_PARAMETER__CHARGER_I);

    /* Update charging state */
    mAutoChargingInformation.current_state = mCurrentState;

    if(true == mIsChargingFailed)
    {
        mAutoChargingInformation.battery_bad = TRUE;
    }
    else
    {
        if( BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status)
        {
            mAutoChargingInformation.battery_removed = TRUE;
        }
        else
        {
            mAutoChargingInformation.battery_removed = FALSE;
        }

        mAutoChargingInformation.battery_bad = FALSE;
    }

    if(TRUE == chg_system_tx_informaton.tx_on)
    {
        mAutoChargingInformation.TX_on = TRUE;
    }
    else
    {
        mAutoChargingInformation.TX_on = FALSE;
    }

    if(true == mIsInFinalCycle)
    {
        mAutoChargingInformation.charging_in_final_cycle = TRUE;
    }
    else
    {
        mAutoChargingInformation.charging_in_final_cycle = FALSE;
    }

    if(true == mIsChargingComplete)
    {
        mAutoChargingInformation.charging_finished = TRUE;
    }
    else
    {
        mAutoChargingInformation.charging_finished = FALSE;
    }

    if(true == mIsCVTopoffOn)
    {
        mAutoChargingInformation.cv_topoff_on = TRUE;
    }
    else
    {
        mAutoChargingInformation.cv_topoff_on = FALSE;
    }

}

void Debug_AutoChargingManagerCVTopoff::EventFired(ChargingEvent* event)
{
    ASSERT(event != NULL);

    AutoChargingManagerCVTopoff::EventFired(event);

    switch( event->GetEventId() )
    {
        case CHG_CMD__IRQ_TRIGGERED:
        {
            IrqEvent* irqevent = dynamic_cast< IrqEvent* >( event );

            ASSERT( irqevent != NULL );

            pm_irq_hdl_type irqid = irqevent->GetIrqId();

            if(irqid == PM_VALID_CHG_IRQ_HDL) /* A charging source is attached */
            {
                MSG_HIGH(" chg_debug: Regular Wall charger has plugged in", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if(irqid == PM_INVALID_CHG_IRQ_HDL ) /* A charging source is removed */
            {
                MSG_HIGH(" chg_debug: Regular Wall charger is removed", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if( irqid == PM_BATTCONNECT_IRQ_HDL ) /* Battery connected/removed */
            {
                if(BATTERY_STATUS__REMOVED == mGeneralStatus.battery_status)
                {
                    MSG_HIGH(" chg_debug: Battery is removed", 0, 0, 0);

                    /* Update debug info */
                    mAutoChargingInformation.battery_removed = TRUE;

                    if(TRUE == mAutoChargingInformation.cv_topoff_on)
                    {
                        MSG_HIGH(" chg_debug: CV topoff off", 0, 0, 0);

                        mAutoChargingInformation.cv_topoff_on = FALSE;
                    }
                }
                else
                {
                    MSG_HIGH(" chg_debug: Battery is connected", 0, 0, 0);

                    /* Update debug info */
                    mAutoChargingInformation.battery_removed = FALSE;
                }

                /* Update debug information */
                mAutoChargingInformation.battery_connect_irq_triggered = TRUE;
            }
            else if(irqid == PM_CHGSTATE_IRQ_HDL) /* Charging state has changed */
            {
                MSG_HIGH(" chg_debug: charging state changed", 0, 0, 0);

                /* Update previous state */
                mAutoChargingInformation.previous_state = mAutoChargingInformation.current_state;

                /* Update debug information */
                mAutoChargingInformation.change_of_state_irq_triggered = TRUE;
                mAutoChargingInformation.current_state = mCurrentState;
            }
            else if( irqid == PM_CHGDONE_IRQ_HDL ) /* Charging has finished successfully */
            {
                MSG_HIGH(" chg_debug: charging-done IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.end_of_charging_irq_triggered = TRUE;
            }
            else if(irqid == PM_CHGFAIL_IRQ_HDL) /* Charging has failed */
            {
                MSG_HIGH(" chg_debug: charging-fail IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.fail_of_charging_irq_triggered = TRUE;
            }
            else if(irqid == PM_BATTTEMP_IRQ_HDL) /* Battery temp has gone out of range */
            {
                /*check for battery temperature only when valid battery is connected*/
                if(BATTERY_STATUS__REMOVED != mGeneralStatus.battery_status)
                {
                    MSG_HIGH(" chg_debug: bad-batt-temp IRQ triggered", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.bad_battery_temp_irq_triggered = TRUE;

                    if(TRUE == mAutoChargingInformation.cv_topoff_on)
                    {
                        MSG_HIGH(" chg_debug: CV topoff off", 0, 0, 0);

                        mAutoChargingInformation.cv_topoff_on = FALSE;
                    }
                }
            }
            else if(irqid == PM_VBAT_DET_IRQ_HDL) /* VBATDET has reached */
            {
                MSG_HIGH(" chg_debug: vbatdet IRQ triggered", 0, 0, 0);

                /* Update debug information */
                mAutoChargingInformation.vbat_det_irq_triggered = TRUE;

                if(true == mIsCVTopoffOn)
                {
                    MSG_HIGH(" chg_debug: CV topoff on", 0, 0, 0);

                    mAutoChargingInformation.cv_topoff_on = TRUE;
                }

            }
            else if(irqid == PM_FASTCHG_IRQ_HDL) /* in fast charging */
            {
                MSG_HIGH(" chg_debug: in-fast-charging IRQ triggered", 0, 0, 0);
            }
            else if(irqid == PM_VCP_IRQ_HDL) /* VCP triggered */
            {
                MSG_HIGH(" chg_debug: VCP IRQ triggered", 0, 0, 0);

                mAutoChargingInformation.vcp_irq_triggered = TRUE;

            } /* PM_VCP_IRQ_HDL */
            else if(irqid == PM_CHGTLIMIT_IRQ_HDL) /* CHGTLIMIT triggered */
            {
                MSG_HIGH(" chg_debug: CHGTLIMIT IRQ triggered", 0, 0, 0);

                mAutoChargingInformation.chgtlimit_irq_triggered = TRUE;

            } /* PM_CHGTLIMIT_IRQ_HDL */
            else if(irqid == PM_BAT_STAT_CHANGED_IRQ_HDL) /* battery status has changed */
            {
                MSG_HIGH(" chg_debug: battery status changed IRQ triggered", 0, 0, 0);

                mAutoChargingInformation.batt_alarm_irq_triggered = TRUE;

                if(TRUE == mAutoChargingInformation.cv_topoff_on)
                {
                    MSG_HIGH(" chg_debug: CV topoff off", 0, 0, 0);

                    mAutoChargingInformation.cv_topoff_on = FALSE;
                }
            } /* PM_BAT_STAT_CHANGED_IRQ_HDL */
        } /* case CHG_CMD__IRQ_TRIGGERED */
        break;
        case CHG_CMD__TIMER_EXPIRED:
        {
            TimerEvent* timerevent = dynamic_cast< TimerEvent* >( event );

            ASSERT( timerevent != NULL );

            unsigned int  firstdelay = timerevent->GetFirstDelay();
            
            /* Charger state machine heart beat */
            if(firstdelay == CHG_HEART_BEAT_FIRST_TIME )
            {
                 /* Update debug information */
                 updateDebugInformation();

                 /* Fire debug event */
                 ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                 FireEvent(pEvent);
                 /* Release memory */
                 delete pEvent;

                 /* Reset debug information */
                 mAutoChargingInformation.battery_connect_irq_triggered = FALSE;
                 mAutoChargingInformation.bad_battery_temp_irq_triggered = FALSE;
                 mAutoChargingInformation.change_of_state_irq_triggered = FALSE;
                 mAutoChargingInformation.end_of_charging_irq_triggered = FALSE;
                 mAutoChargingInformation.fail_of_charging_irq_triggered = FALSE;
                 mAutoChargingInformation.vbat_det_irq_triggered = FALSE;
                 mAutoChargingInformation.charger_irq_triggered = FALSE;
                 mAutoChargingInformation.vcp_irq_triggered = FALSE;
                 mAutoChargingInformation.chgtlimit_irq_triggered = FALSE;
                 mAutoChargingInformation.batt_alarm_irq_triggered = FALSE;
            } /* end CHG_HEART_BEAT */
            /* End-of-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_END_CHARGE_DETECTION_T )
            {
                MSG_HIGH(" chg_debug: end-of-charging detection timer expired", 0, 0, 0);

                if(true == mIsInFinalCycle)
                {
                    mAutoChargingInformation.charging_in_final_cycle = TRUE;
                }
                else
                {
                    mAutoChargingInformation.charging_in_final_cycle = FALSE;
                }

            }/* end AUTO_CHARGING_END_CHARGE_DETECTION_T */
            /* Resume-charging detection times out */
            else if(firstdelay == AUTO_CHARGING_RESUME_CHARGE_DETECTION_T )
            {
                MSG_HIGH(" chg_debug: resume-charge detection timer expired", 0, 0, 0);
            } /* end AUTO_CHARGING_RESUME_CHARGE_DETECTION_T */
            /* max-charging-time detection times out */
            else if(firstdelay == (AUTO_CHARGING_MAX_CHARGE_T*60000) )
            {
                MSG_HIGH(" chg_debug: max-charging-time detection timer expired", 0, 0, 0);

            } /* end AUTO_CHARGING_MAX_CHARGE_T */
            else if(firstdelay == AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T )
            {
                MSG_HIGH(" chg_debug: end-of-CV-topoff detection timer #1 expired", 0, 0, 0);
            } /* end AUTO_CHARGING_RESUME_CHARGE_DETECTION_T */
            else if(firstdelay == AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T )
            {
                MSG_HIGH(" chg_debug: end-of-CV-topoff detection timer #2 expired", 0, 0, 0);

                if(true == mIsChargingComplete)
                {
                    MSG_HIGH(" chg_debug: CV topoff completed!", 0, 0, 0);
                }
            } /* end AUTO_CHARGING_RESUME_CHARGE_DETECTION_T */
        }
        break; /* case CHG_CMD__TIMER_EXPIRED */
        case CHG_CMD__SYSTEM_STATUS_CHANGED:
        {
            SystemEvent* systemevent = dynamic_cast< SystemEvent* >( event );

            ASSERT( systemevent != NULL );

            chg_system_event_id_type  systemeventid = systemevent->GetSystemEventId();

            if(systemeventid == CHG_SYSTEM__USB_VALID) /* A USB source is attached */
            {
                MSG_HIGH(" chg_debug: Valid USB device is connected", 0, 0, 0);

                /* A PC-USB/ SHP just attached */
                if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_HOST)
                {
                    MSG_HIGH(" chg_debug: a PC-USB/ SHP charger has plugged in", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.charger_irq_triggered = TRUE;
                    mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;
                }
                else if (chg_usb_otg_dev == PM_APP_OTG_A_DEV_TYPE__USB_CHARGER)
                {
                    MSG_HIGH(" chg_debug: a Wall-USB/UCP charger has plugged in", 0, 0, 0);

                    /* Update debug information */
                    mAutoChargingInformation.charger_irq_triggered = TRUE;
                    mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;
                }

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            else if(systemeventid == CHG_SYSTEM__USB_INVALID) /* A USB source is removed */
            {
                MSG_HIGH(" chg_debug: The USB device has removed", 0, 0, 0);

                /* Stop autonomous charging */
                stopAutoCharging();

                /* Update debug information */
                mAutoChargingInformation.charger_irq_triggered = TRUE;
                mAutoChargingInformation.charger_type = mGeneralStatus.charger_hardware_type;

                /* Fire debug event */
                ChargingEvent *pEvent = new AutoChargingDebugEvent(mAutoChargingInformation);
                FireEvent(pEvent);
                /* Release memory */
                delete pEvent;

                /* Reset debug information */
                mAutoChargingInformation.charger_irq_triggered = FALSE;
            }
            /* USB device is in suspend mode */
            else if(systemeventid == CHG_SYSTEM__USB_SUSPEND) 
            {
                MSG_HIGH(" chg_debug: In USB suspend state", 0, 0, 0);
            }
            /* System is in a call */
            else if(systemeventid == CHG_SYSTEM__TX_ON) 
            {
                MSG_HIGH(" chg_debug: In TX ON state", 0, 0, 0);

                mAutoChargingInformation.TX_on = TRUE;

                if(TRUE == mAutoChargingInformation.cv_topoff_on )
                {
                    MSG_HIGH(" chg_debug: stop CV topoff", 0, 0, 0);

                    mAutoChargingInformation.cv_topoff_on = FALSE;
                }
            } /* CHG_SYSTEM__TX_ON */
            /* System is NOT in a call anymore */
            else if(systemeventid == CHG_SYSTEM__TX_OFF) 
            {
                MSG_HIGH(" chg_debug: In TX OFF state", 0, 0, 0);

                mAutoChargingInformation.TX_on = FALSE;
            } /* CHG_SYSTEM__TX_OFF */

        } /* end CHG_CMD__SYSTEM_STATUS_CHANGED */
        break;
    } /* switch() */
} /* EventFired() */

#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */

#endif /* FEATURE_CHG_TASK || T_WINNT */
