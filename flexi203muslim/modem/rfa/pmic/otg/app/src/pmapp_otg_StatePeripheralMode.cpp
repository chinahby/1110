/*! \file 
*  \n
*  \brief  pmapp_otg_StatePeripheralMode.cpp  ----- PM APP OTG State machine related class implementation
*  \details This file contains implementation of classes for OTG states 
*     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_StatePeripheralMode.cpp#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/25/08   aab      Modified B_Idle_State::Entry() to support USB LDO input power configuration for KIP/HAN
                    Added FEATURE_PMIC_KIP_HAN to support KIP/HAN PMIC's regester defination variation
08/18/08   aab      Added support for usb host suspended notification
		    		(usb_power_source_suspended_notification)
08/04/08   aab      Added FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_Interface.h"   

extern "C"
{
   #ifdef FEATURE_USB_ON_APPS_PROC 
   #include "dem.h"
   #endif
}

#ifdef FEATURE_PMIC_USB_OTG

/* ==========================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */



/**************************************************************************
     class BaseState											  
**************************************************************************/
BaseState::BaseState()
{
	mStateName = "";
	mStateId   = PM_APP_OTG_STATE__INVALID;
	BaseState::mOtgEventPayload = -1;

	mpEvtUtil = EvtUtil::GetInstance();

	//init more object ptrs
	mpUsbBusListener = UsbBusListener::GetInstance();
	mpGenericEvtDetectIsr = GenericEvtDetectIsr::GetInstance();
	mpOtgTimer = OtgTimer::GetInstance();
	mpPowerConsumer = PowerConsumer::GetInstance();
	mpUtil = Util::GetInstance();

	mpStateManager = StateManager::GetInstance();
	mpCustomHw = CustomHw::GetInstance();
	mpOtgDiagEvent = OtgDiagEvent::GetInstance();
	mpDpStateMonitor   = DpStateMonitor::GetInstance();
	mpDmStateMonitor   = DmStateMonitor::GetInstance();
}


BaseState::~BaseState()
{
}


void BaseState::Entry()
{
//{ throw "Method_Not_Available";}
} 

BaseState* BaseState::CachedEvtHdlr()
{
	return NULL;
}

boolean BaseState::SuspendEvtHdlr()
{ 
	return false;
}

boolean BaseState::ResumeEvtHdlr()
{
	return false;
}

boolean BaseState::RemoteHostWakeupSignaling_SetupRequestHdlr(boolean state)
{ 
	return state = false;
}
boolean BaseState::UsbVbusControlRequestHdlr(boolean need_control)
{
	return need_control = false;
}

boolean BaseState::CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode_type)
{
   carkit_mode_type = PM_APP_OTG_CARKIT_MODE__INVALID;
   return false;
}



/**************************************************************************
     class Null_State											  
**************************************************************************/
Null_State::Null_State()
{
	mStateName="NULL";
	mStateId=PM_APP_OTG_STATE__NULL;
}

Null_State::~Null_State()
{
}


void Null_State::Entry()
{
	//do nothing
}



BaseState* Null_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState*  next_state_ptr = mpStateFactory->mpNull_State;
	static boolean init_in_progress = false;

	if(mpEvtUtil->sm_reset_evt->IsHigh() )
	{
		mpEvtUtil->sm_reset_evt->Reset();

		// shut down USB-OTG event generation mechanism
		mpStateManager->ResetEvtGenEngine();

		// abort any init in progress
		mpCustomHw->CustomHwReset();

		mpEvtUtil->custom_hw_ready_evt->Reset();
		init_in_progress = false;

		// remain in the same state
	}
	else if ( !init_in_progress && mpEvtUtil->sm_init_evt->IsHigh() )
	{
		init_in_progress = true;

		// don't reset 'pm_app_otg_evt__sm_init' event since it is
		//  being used in pm_app_otg_init() and pm_app_otg_reset()
		//PM_APP_OTG_EVT_RESET(pm_app_otg_evt__sm_init);

		// initiate custom h/w init
		mpEvtUtil->custom_hw_ready_evt->Reset();

		//pm_app_otg_custom_hw_init(pm_app_otg_custom_hw_init_done_cb);
		mpCustomHw->CustomHwInit(mpStateManager->CustomHwInitDoneCb, (void*)mpStateManager);

		// remain in the same state
	}
	//else if ( PM_APP_OTG_EVT_IS_HIGH(pm_app_otg_evt__custom_hw_ready) )
	else if ( mpEvtUtil->custom_hw_ready_evt->IsHigh())
	{
		mpEvtUtil->custom_hw_ready_evt->Reset();

		// load all state machine params from the shadow before taking it out of reset
		mpUtil->ctrlr_info_curr = mpUtil->ctrlr_info_shadow;

		// kick start event generation engine
		mpStateManager->InitEvtGenEngine();

		// notify OTG controller about completion of initialization, and
		//  our readiness to use other callbacks provided by this controller.
		if (NULL != mpUtil->ctrlr_info_curr.init_done_cb)
		{
			mpUtil->ctrlr_info_curr.init_done_cb();
		}

		// mark init as completed
		init_in_progress = false;
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}

	return next_state_ptr;
}





/**************************************************************************
     class B_Idle_State											  
**************************************************************************/

B_Idle_State::B_Idle_State()
{
	mStateName="B_IDLE";
	mStateId=PM_APP_OTG_STATE__B_IDLE;
}


B_Idle_State::~B_Idle_State()
{
}


void B_Idle_State::Entry()
{
	pm_app_otg_remote_a_dev_info_type    a_dev_none;
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();

	// reset current operational mode
	mpUtil->current_operational_mode = PM_APP_OTG_OPERATIONAL_MODE__NONE;

	// disable D+/- debouncers
	mpDpStateMonitor->DpStateMonitorReset();
	mpEvtUtil->dp_evt->Reset();
	mpDmStateMonitor->DmStateMonitorReset();
	mpEvtUtil->dm_evt->Reset();

	// disable IRQs that might have been left enabled in various states
	// prior to entering this
	mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

	// reset remote A-dev information structure
	a_dev_none.remote_a_dev = PM_APP_OTG_A_DEV_TYPE__INVALID;
	a_dev_none.max_i_ma = 0;
	a_dev_none.hostSuspended = false;
	pPmAppOtg->SetRemote_A_DevInfo(a_dev_none);
	// and to be on the safer side (on behalf of the USB power consumer),
	// relinquish control over USB_VBUS
	mpUtil->UsbVbusControlRequestHdlr(FALSE);

	// notify USB ctrlr that the remote device was lost
	if ( mpUtil->active_controller_session_flag )
	{
		// Just before OTG controller is notified about the end of the current
		//  USB Peripheral or Carkit or RS232_Dongle mode session, fake
		//  a "relinquish" bus request to properly terminate the current session.
		//  This will prevent future bus acquisitions from automatically occuring
		//  until OTG controller explicitly "acquires" the bus again!
		pPmAppOtg->RelinquishBus();

		if ( NULL != mpUtil->ctrlr_info_curr.remote_dev_lost_cb )
		{
			mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_DEVICE_DETACHED);
			mpUtil->ctrlr_info_curr.remote_dev_lost_cb();
		}
	}
	// The active session with the OTG controller is now over
	mpUtil->active_controller_session_flag = FALSE;

	// common initialization for both dual role A/B device
	mpStateManager->PerformCommonDualRoleDevInit();

	#ifndef FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE
	    #ifdef FEATURE_PMIC_KIP_HAN //KIP/HAN : select USB_VBUS as the power source for the xcvr
	       pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VBUS);
	    #else  // PMIC3: select USB_VBUS as the power source for the xcvr
	       (void) pm_otg_pwr_sel(PM_OTG_PWR_SEL__VBUS);
	    #endif
	#else
	    #ifdef FEATURE_PMIC_KIP_HAN //KIP/HAN: select VREG_5V as the power source for the xcvr
	       pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VREG5V);
	    #else
	       // PMIC3: select VREG_5V as the power source for the xcvr
	       (void) pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST);
	    #endif
	#endif //FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE

	// assert sleep
	mpUtil->AssertSleep(TRUE);
}




BaseState* B_Idle_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
    BaseState* next_state_ptr = mpStateFactory->GetStateInstance(PM_APP_OTG_STATE__B_IDLE);

	if ( mpEvtUtil->sm_reset_evt->IsHigh() )
	{
		// Don't clear this event since it is used by NULL state
		//  to process this event appropriately, and clear it afterward

		// Shutdown the xcvr completely for max current savings 
		(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SHUTDOWN);

		next_state_ptr = mpStateFactory->mpNull_State;
	}
	else if ( mpEvtUtil->id_evt->IsLow() ) // ID line grounded (mini-A plug inserted)
	{
#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
		// Check if OTG controller is interested in A-dev (i.e. USB host mode) sessions
		if ( (NULL != mpUtil->ctrlr_info_curr.host_mode_init_cb)
			&& mpEvtUtil->a_bus_req_evt->IsHigh() )  // bus has been acquired
		{
			next_state_ptr = mpStateFactory->mpA_Idle_State;
		}
		else
		{
			next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
		}
#else //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
			next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
	}


	else if ( mpEvtUtil->b_sess_vld_evt->IsHigh() )
	{
		// Need to determine if we are connected to a standard USB host or a carkit
		// or a USB charger

		if ( mpEvtUtil->b_bus_req_evt->IsHigh() )  // bus has been acquired
		{
			next_state_ptr = mpStateFactory->mpB_RemoteDevDetectWait_State;
		}
		else
		{
			next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
		}
	}
	else if ( (mpUtil->resistive_id_flag) )  // Resistive ID plug inserted             
	{
		// ID line pulled down low by an external resistor as in
		//  case of RS-232 dongles   

#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
		// check if OTG controller is interested in RS232_Dongle mode sessions
		if ( (NULL != mpUtil->ctrlr_info_curr.rs232dongle_mode_init_cb)
			&& mpEvtUtil->a_bus_req_evt->IsHigh() ) // bus has been acquired
		{
			next_state_ptr = mpStateFactory->mpRs232dongle_Config_State;
		}
		else
		{
			next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
		}
#else //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
        next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
	}

	return next_state_ptr;
}





/**************************************************************************
     class B_RemoteDevDetectWait_State											  
**************************************************************************/

B_RemoteDevDetectWait_State::B_RemoteDevDetectWait_State()
{
	mStateName="B_REMOTE_DEV_DETECT_WAIT";
	mStateId=PM_APP_OTG_STATE__B_REMOTE_DEV_DETECT_WAIT;
}


B_RemoteDevDetectWait_State::~B_RemoteDevDetectWait_State()
{
}




void B_RemoteDevDetectWait_State::Entry()
{
	boolean  dm_hi = FALSE;
	// negate sleep
	mpUtil->AssertSleep(FALSE);

	// Since we know that we are connected to an A-dev, we can safely
	// remove the DISCHG resistor internally attached to VBus. This
	// will reduce the current unnecessarily drawn from the remote A-dev.
	(void) pm_otg_config_interface(
		PM_OTG_VBUS_CONFIG_CMD__DISABLE_DISCHG,
		PM_OTG_DP_CONFIG_CMD__NONE,
		PM_OTG_DM_CONFIG_CMD__NONE,
		PM_OTG_ID_CONFIG_CMD__NONE );

	// Found that activation process took a couple of secs to get
	// the xcvr ready for signaling using pull-up resistor on D+.
	// VREG_USB was not stabilizing soon enough. Let's try the best by
	// enabling it well ahead of actual use.
	// So activate the xcvr right at the start to gain some time!!!

   #ifdef FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE
      	// vote to turn ON BOOST_5V
      	pm_vote_vreg_switch(
      		PM_ON_CMD,
      		PM_VREG_BOOST_ID,
      		PM_VOTE_VREG_BOOST_APP__OTG);
      	// PMIC OTG xcvr has already been configured to get its power
      	// from BOOST_5V in "B_IDLE" state
   #endif //FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE


   #ifndef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
        // turn ON VREG_USB
         #ifdef FEATURE_PMIC_KIP_HAN
            (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB3P3_M);
         #else
            (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB_M);
         #endif //FEATURE_PMIC_KIP_HAN
   #endif //FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

	// Power up from suspended state
	(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

	// Since it is possible that the carkit was already connected before the
	// PM xcvr powered ON, D- might have already gone HIGH and so we won't
	// be able to capture D- rising-edge
	mpEvtUtil->dm_evt->Reset();

	mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(PM_USB_D_MINUS_HI_RE_IRQ_HDL, mpEvtUtil->dm_evt);

	(void) pm_get_rt_status(PM_USB_D_MINUS_HI_RT_ST,&dm_hi);

	if (dm_hi)
	{
		// we have determined that it is carkit
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();
		mpEvtUtil->dm_evt->SetHigh();

		// just fall thro' the cached evt handler to
		// make a determination that it is carkit!
		mpEvtUtil->dp_evt->Reset();
	}
	else
	{
		// pull D+ high
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__NONE,
			PM_OTG_DP_CONFIG_CMD__PULL_UP,
			PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
			PM_OTG_ID_CONFIG_CMD__NONE );

		// Start D+ debouncer for detecting USB bus reset since the reset pulse is
		//   wide enough to be detected after debouncing.
		// In the case of carkit signaling, the D- transitions signaled by the carkit
		//   could be narrower than the width used for filtering by the debouncer; so
		//   use D- RE interrupt directly, since all we care about is D- ever going high!
		mpEvtUtil->dp_evt->Reset();
		mpDpStateMonitor->DpStateMonitorInit(mpUsbBusListener->DpListener, mpUsbBusListener);

	}
}




BaseState* B_RemoteDevDetectWait_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpB_RemoteDevDetectWait_State;
	boolean dm_high = FALSE;

	static  const int32 tmr__dp_low_warmup    = 60; // 60ms debouncer interval
	// see Tattdb (100ms) in USB2.0 spec.
	static  boolean   dp_low_detection_warmed_up = FALSE;
	static  boolean   dm_stabilization_timer_started = FALSE;

	if ( mpEvtUtil->id_evt->IsLow()
		|| mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}
	else if ( mpEvtUtil->dp_evt->IsHigh() )
	{
		mpEvtUtil->dp_evt->Reset();

		if ( !dp_low_detection_warmed_up && !dm_stabilization_timer_started)
		{
			// Now that D+ has become high, start the timer to filter out
			//   glitches that could cause us to erroneously detect USB carkit/charger as USB host
			mpOtgTimer->StartTimer(tmr__dp_low_warmup, mpEvtUtil->dp_low_detection_warmup_tmout_evt);

			// disable D+ debouncer to filter out D+ low glitches
			mpDpStateMonitor->DpStateMonitorReset();
		}

		// remain in the same state
	}
	else if ( mpEvtUtil->dp_low_detection_warmup_tmout_evt->IsHigh() )
	{
		mpEvtUtil->dp_low_detection_warmup_tmout_evt->Reset();

		//Now we can reliably detect USB bus reset
		dp_low_detection_warmed_up = TRUE;

		// restart D+ debouncer so as to detect D+ going (or having already gone) low
		mpDpStateMonitor->DpStateMonitorInit(mpUsbBusListener->DpListener, (void*)mpUsbBusListener); 

		// remain in the same state
	}
	else if ( mpEvtUtil->dp_evt->IsLow() )
	{
		mpEvtUtil->dp_evt->Reset();

		if ( dp_low_detection_warmed_up )
		{      
			// remote A-dev responded by pulling D+ low which is actually
			// a USB bus reset, so it must be a standard USB host like a PC

			// disable D+ debouncer and D- RE IRQ
			mpDpStateMonitor->DpStateMonitorReset();
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

			// remove D+ pull-up so that we start off on a clean slate in B_PERIPHERAL
			// and also to end this "test-only" session with the USB host!
			(void) pm_otg_config_interface(
				PM_OTG_VBUS_CONFIG_CMD__NONE,
				PM_OTG_DP_CONFIG_CMD__PULL_DOWN,
				PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
				PM_OTG_ID_CONFIG_CMD__NONE );

			// check if OTG controller is interested in B-dev (i.e. USB peripheral mode) sessions
			if (NULL != mpUtil->ctrlr_info_curr.peripheral_mode_init_cb)
			{
				mpUtil->b_peripheral_resumption_flag = FALSE; // we are entering B_PERIPHERAL for the first
				//  time since the last USB session (if any)
				next_state_ptr = mpStateFactory->mpB_Peripheral_State;
			}
			else
			{
				next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
			}
		}
		// else
		//   remain in the same state
	}
	else if ( mpEvtUtil->dm_evt->IsHigh() )
	{
		mpEvtUtil->dm_evt->Reset();

		// this event was actually generated by the generic ISR and so there is
		// no need to disable that ISR again

		// Remote A-dev responded by pulling D- high which means that it has to
		// be either a OTG carkit or USB charger. Remove the pull-up on D+ and
		// check how the A-dev behaves.
		// If after (Tcr_dp_con+Tcr_disc_det), the D- is still pulled high, it
		// would mean that the A-dev is carkit. If not it is a USB charger.
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__NONE,
			PM_OTG_DP_CONFIG_CMD__PULL_DOWN,
			PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
			PM_OTG_ID_CONFIG_CMD__NONE );

		// disable D+ debouncer since it is useful only for detecting USB host
		mpDpStateMonitor->DpStateMonitorReset();
		mpEvtUtil->dp_evt->Reset();
		// stop the D+ low detection warmup timer, if currently running

		mpOtgTimer->StopTimer(mpEvtUtil->dp_low_detection_warmup_tmout_evt);
		dp_low_detection_warmed_up = FALSE;

		mpOtgTimer->StartTimer(mpUtil->cr_dp_con_tmr + mpUtil->cr_disc_det_tmr, mpEvtUtil->dm_stabilization_tmout_evt);

		dm_stabilization_timer_started = TRUE;
		// remain in the same state
	}
	else if ( mpEvtUtil->dm_stabilization_tmout_evt->IsHigh() )
	{
		mpEvtUtil->dm_stabilization_tmout_evt->Reset();
		(void) pm_get_rt_status(PM_USB_D_MINUS_HI_RT_ST, &dm_high);

		if (dm_high)
		{
#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
			// it is a carkit
			//  check if OTG controller is interested in USB carkit client mode sessions
			if (NULL != mpUtil->ctrlr_info_curr.carkitclient_mode_init_cb)
			{
				next_state_ptr = mpStateFactory->mpCarkitclient_Init_State;
			}
			else
			{
				next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
			}
#else //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
			next_state_ptr = mpStateFactory->mpUnsought_Mode_State;
#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
		}
		else
		{
			// it is a USB charger
			next_state_ptr = mpStateFactory->mpChg_Conn_State;
		}
	}

	// Do critical cleanups before exiting this state
	if ( mpStateFactory->mpB_RemoteDevDetectWait_State != next_state_ptr )
	{
		mpOtgTimer->StopTimer(mpEvtUtil->dm_stabilization_tmout_evt);
		mpOtgTimer->StopTimer(mpEvtUtil->dp_low_detection_warmup_tmout_evt);

		mpEvtUtil->dm_stabilization_tmout_evt->Reset();
		mpEvtUtil->dp_low_detection_warmup_tmout_evt->Reset();
		dp_low_detection_warmed_up = FALSE;
		dm_stabilization_timer_started = FALSE;

		// assert sleep
		mpUtil->AssertSleep(TRUE);
	}

	return next_state_ptr;
}




/**************************************************************************
     class B_Peripheral_State											  
**************************************************************************/
B_Peripheral_State::B_Peripheral_State()
{
	mStateName="B_PERIPHERAL"; 
	mStateId=PM_APP_OTG_STATE__B_PERIPHERAL; 
	mpUsbPowerConsumerConfigInfo = UsbPowerConsumerConfigInfo::GetInstance();
}


B_Peripheral_State::~B_Peripheral_State()
{
}


void B_Peripheral_State::Entry()
{
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();
	pm_app_otg_remote_a_dev_info_type  host_info;

	// we are coming into this state either for the first time or just after
	//  the recent resume operation. In either case, ensure that a_bus_suspend
	//  event does not remain active from the past.
	mpEvtUtil->a_bus_suspend_evt->Reset();

    //set USB LDO input to USB Vbus line
    mpUtil->UsbVbusControlRequestHdlr(FALSE);

	// There are initializations per session that we don't want to
	// perform everytime there is a b_peripheral->b_suspend->b_peripheral
	// transition.
	if ( !mpUtil->b_peripheral_resumption_flag )
	{
		mOtgEventPayload = PM_APP_OTG_DEVICE_ATTACHED__HOST_DEVICE;
		mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_DEVICE_ATTACHED,1,(void *) &mOtgEventPayload);  //host device is connected
		// save current operational mode
		mpUtil->current_operational_mode = PM_APP_OTG_OPERATIONAL_MODE__PERIPHERAL;

      #ifndef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
         // turn ON VREG_USB
         #ifdef FEATURE_PMIC_KIP_HAN
            (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB3P3_M);
         #else //PMIC3
            (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB_M);
         #endif //FEATURE_PMIC_KIP_HAN        
      #endif //FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

		// configure OTG transceiver for B-device/USB-peripheral mode
		(void) pm_otg_config_usb_mode(PM_OTG_DEV__B, PM_OTG_USB_SPEED__FULL);

		// power up from suspended state
		(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

		// Default to disabled status for "remote wakeup" feature at the beginning
		//  of every USB session by faking an invocation from OTG controller!
		pPmAppOtg->SetRemote_WakeupCapability(FALSE);

		// ask USB-controller to initialize itself
		if ( NULL != mpUtil->ctrlr_info_curr.peripheral_mode_init_cb )
		{
			mpUtil->ctrlr_info_curr.peripheral_mode_init_cb();

			// Remember that an active session has been started with the OTG controller
			mpUtil->active_controller_session_flag = TRUE;
		}

		// remove D+ pull down but still leave D- pull down operational
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__NONE,
			PM_OTG_DP_CONFIG_CMD__DISABLE_PULL_DOWN,
			PM_OTG_DM_CONFIG_CMD__NONE,
			PM_OTG_ID_CONFIG_CMD__NONE );

		// pull up on D+ to signal to the host that we are ready
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__NONE,
			PM_OTG_DP_CONFIG_CMD__PULL_UP,
			PM_OTG_DM_CONFIG_CMD__NONE,
			PM_OTG_ID_CONFIG_CMD__NONE );

		// recognize remote A-dev as a USB host
		host_info.remote_a_dev = PM_APP_OTG_A_DEV_TYPE__USB_HOST;
		// suspend current limit has to met until the bus reset happens
		host_info.max_i_ma = mpUsbPowerConsumerConfigInfo->pm_app_otg_max_i_ma__upon_bus_suspend;
		host_info.hostSuspended = false;
		pPmAppOtg->SetRemote_A_DevInfo(host_info);

		// enable IRQ for detecting the beginning of a bus reset to be
		// issued by the remote host
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
			PM_USB_D_PLUS_HI_FE_IRQ_HDL,
			mpEvtUtil->a_bus_reset_evt);
	}
	else
	{
		// we have properly resumed back in b_peripheral state
		// revert back to the original non-suspend current limit so that
		// the local USB power consumer can continue to function as before
		// the suspend took effect
		pPmAppOtg->SetRemote_A_DevInfo(mpUtil->saved_before_suspend_remote_host_info);
	}
}




BaseState* B_Peripheral_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpB_Peripheral_State;
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();

	// first process any events that have to be processed anyway and which don't
	// preclude other events from being processed in the same processing context
	if ( mpEvtUtil->remote_wakeup_evt->IsSet() )
	{
		mpUtil->remote_wakeup_capability_enabled_flag
			= mpEvtUtil->remote_wakeup_evt->IsHigh();
		mpEvtUtil->remote_wakeup_evt->Reset();
	}

	// ----- START of mutually exclusive event processing section -----
	if ( mpEvtUtil->id_evt->IsLow()
		|| mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}
	else if (mpEvtUtil->a_bus_suspend_evt->IsHigh())
	{
		mpEvtUtil->a_bus_suspend_evt->Reset();

		// before suspending, save current host info for reverting
		// back to later on upon resume
		mpUtil->saved_before_suspend_remote_host_info = mpUtil->remote_a_dev_info;

		next_state_ptr = mpStateFactory->mpB_Suspend_State;
	}
	else if ( mpEvtUtil->a_bus_reset_evt->IsHigh() )
	{
		pm_app_otg_remote_a_dev_info_type  host_info;

		mpEvtUtil->a_bus_reset_evt->Reset();

		host_info.remote_a_dev = PM_APP_OTG_A_DEV_TYPE__USB_HOST;
		host_info.max_i_ma = mpUsbPowerConsumerConfigInfo->pm_app_otg_max_i_ma__upon_bus_reset;
		host_info.hostSuspended = false;
		pPmAppOtg->SetRemote_A_DevInfo(host_info); 

		// continue to remain in the same state
	}
	// ----- END of mutually exclusive event processing section -----

	// perform critical clean ups before leaving this state
	if (mpStateFactory->mpB_Peripheral_State != next_state_ptr)
	{
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();
	}

	return next_state_ptr;
}



boolean B_Peripheral_State::SuspendEvtHdlr()
{
	// A-dev has suspended the bus by idling it for more than 3ms, as per USB 2.0
	mpEvtUtil->a_bus_suspend_evt->SetHigh();
	return true;
}



boolean B_Peripheral_State::UsbVbusControlRequestHdlr(boolean need_control)
{
	return mpUtil->UsbVbusControlRequestHdlr(need_control);
}





/**************************************************************************
     class B_Suspend_State											  
**************************************************************************/

B_Suspend_State::B_Suspend_State()
{
	mStateName="B_SUSPEND"; 
	mStateId=PM_APP_OTG_STATE__B_SUSPEND;
	pm_app_otg_tmr__b_suspend_inter_a_dev_disconnect_detect_delay=1000;
	mpUsbPowerConsumerConfigInfo = UsbPowerConsumerConfigInfo::GetInstance();
}


B_Suspend_State::~B_Suspend_State()
{
}



void B_Suspend_State::Entry()
{
	pm_app_otg_remote_a_dev_info_type suspend_info = { PM_APP_OTG_A_DEV_TYPE__INVALID, 0 };
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();

	// Reset any events that might be stale from past transition through this state!
	mpEvtUtil->a_bus_resume_evt->Reset();

	// VREG_USB to remain powered ON so that D+ pull-up remains active
	//  for the remote host to maintain connectivity status

	// local power consumer must now meet SUSPEND current limits
	suspend_info = mpUtil->remote_a_dev_info;
	suspend_info.max_i_ma = mpUsbPowerConsumerConfigInfo->pm_app_otg_max_i_ma__upon_bus_suspend;
	suspend_info.hostSuspended = true;
	pPmAppOtg->SetRemote_A_DevInfo(suspend_info);

	// Suspend the xcvr to minimize current drawn from USB_VBUS during
	//  "suspend" state.
	(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND);

	// At this point, the data lines D+/- are in idle state, which means
	//  that D+ is high (because of xcvr's D+ pull-up) and D- is low.

	// Only if OTG controller had registered a non-NULL remote_dev_resumed_cb,
	//  do the following:
	//  - wait for D+ FE to be able to detect both non-idle (resume) as well as SE0
	//    signaling to get the bus out of suspended state.   
	mpEvtUtil->a_bus_K_SE0_evt->Reset();
	if ( NULL != mpUtil->ctrlr_info_curr.remote_dev_resumed_cb )
	{
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
			PM_USB_D_PLUS_HI_FE_IRQ_HDL,
			mpEvtUtil->a_bus_K_SE0_evt);
	}

	// Request controller to issue a remote wakeup to the connected host only if
	//  - "remote wakeup" capability has been enabled by our OTG controller,
	//  - USB power consumer is still interested in drawing power from Vbus, and
	//  - 'remote_host_wakeup_fn' is non-NULL
	// Note that we are deliberately not checking for a non-NULL 'remote_dev_resumed_cb'
	//  here because we want to leave open the flexibility for certain OTG controller
	//  implementations to detect RESUME condition on their own without requiring
	//  the help of this transceiver application.
	if ( mpUtil->remote_wakeup_capability_enabled_flag
		&& mpUtil->usb_power_consumer_interested_flag
		&& (NULL != mpUtil->ctrlr_info_curr.remote_host_wakeup_fn) )
	{
		mpUtil->ctrlr_info_curr.remote_host_wakeup_fn();
	}

	if(mpUtil->usb_power_consumer_interested_flag) //start timer only if power consumer registered
	{
		// start the timer to wait for the beginning of A-dev disconnect detection phase
		mpOtgTimer->StartTimer(
			pm_app_otg_tmr__b_suspend_inter_a_dev_disconnect_detect_delay,
			mpEvtUtil->b_suspend_inter_a_dev_disconnect_delay_tmout_evt );
	}

}




BaseState* B_Suspend_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpB_Suspend_State;

	// max time duration for which USB_VBUS is pulled to ground through the 
	//  DISCHG resistor to cause (potentially) floating USB_VBUS to settle down
	//  to less than sess_valid threshold (1.8V).   
	static const int32 tmr__vbus_dischg_for_a_dev_disconnect_detect = 15; // 15ms >= 2*RC (2*1K*6.25uF)
	//mpEvtUtil->b_wait_vfall_tmout_evt->Reset(); //??

	if ( mpEvtUtil->b_suspend_inter_a_dev_disconnect_delay_tmout_evt->IsHigh() )
	{
		mpEvtUtil->b_suspend_inter_a_dev_disconnect_delay_tmout_evt->Reset();

		// attach discharge resistor on VBUS momentarily (using timer) to reliably 
		//  detect the disconnection of remote A-device.
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__DISCHG,
			PM_OTG_DP_CONFIG_CMD__NONE,
			PM_OTG_DM_CONFIG_CMD__NONE,
			PM_OTG_ID_CONFIG_CMD__NONE );

		mpOtgTimer->StartTimer(tmr__vbus_dischg_for_a_dev_disconnect_detect, mpEvtUtil->b_wait_vfall_tmout_evt);

		// remain in the same state
	}
	else if ( mpEvtUtil->b_wait_vfall_tmout_evt->IsHigh() )
	{
		mpEvtUtil->b_wait_vfall_tmout_evt->Reset();

		// stop the USB_VBUS discharge process
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__DISABLE_DISCHG,
			PM_OTG_DP_CONFIG_CMD__NONE,
			PM_OTG_DM_CONFIG_CMD__NONE,
			PM_OTG_ID_CONFIG_CMD__NONE );

		mpOtgTimer->StartTimer(
			pm_app_otg_tmr__b_suspend_inter_a_dev_disconnect_detect_delay,
			mpEvtUtil->b_suspend_inter_a_dev_disconnect_delay_tmout_evt);

		// remain in the same state
	}
	else if ( mpEvtUtil->a_bus_K_SE0_evt->IsHigh() )
	{
		mpEvtUtil->a_bus_K_SE0_evt->Reset();

		// This is the ISR for detecting non-idle resume signaling initiated by
		// the remote USB A-device (could be either USB host or a peripheral) to get the
		// USB bus out of suspended state. The OTG controller is notified about the
		// resume signaling so that it could get ready for interacting with the remote
		// USB device.

		// It is to be noted that this ISR by itself will NOT activate the transceiver;
		// it has to be explicitly requested for by the OTG controller using 
		// pm_app_otg_resume().

		// Not sure if we need a tramp here. For now just call
		// remote_dev_resumed_cb directly. TBD
		if ( NULL != mpUtil->ctrlr_info_curr.remote_dev_resumed_cb )
		{
			mpUtil->ctrlr_info_curr.remote_dev_resumed_cb();
		}
		// continue to remain in the same state
	}
	else if (mpEvtUtil->a_bus_resume_evt->IsHigh())
	{
		mpEvtUtil->a_bus_resume_evt->Reset();

		// get the xcvr out of SUSPEND state
		(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

		mpUtil->b_peripheral_resumption_flag = TRUE; // NOT entering B_PERIPHERAL for the first time
		//  during this (peripheral mode) session.
		next_state_ptr = mpStateFactory->mpB_Peripheral_State;
	}
	else if ( mpEvtUtil->id_evt->IsLow()
		|| mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		// turning OFF resume detection ISR will be done in "B_IDLE" state
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}

	// Do critical cleanups before exiting this state
	if ( mpStateFactory->mpB_Suspend_State != next_state_ptr )
	{
		// stop the timer      
		mpOtgTimer->StopTimer(mpEvtUtil->b_suspend_inter_a_dev_disconnect_delay_tmout_evt);
		mpOtgTimer->StopTimer(mpEvtUtil->b_wait_vfall_tmout_evt);

		// stop the USB_VBUS discharge process      
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__DISABLE_DISCHG,
			PM_OTG_DP_CONFIG_CMD__NONE,
			PM_OTG_DM_CONFIG_CMD__NONE,
			PM_OTG_ID_CONFIG_CMD__NONE );
	}

	return next_state_ptr;
}




boolean B_Suspend_State::ResumeEvtHdlr()
{
	// There are 2 scenarios in which this can be called ...
	// Scenario #1: Remote host has put the bus in suspend state and is
	//              waking us (peripheral) up now by using non-idle signaling.
	//              In this case, our "resume detection ISR" would
	//              have already triggered a callback into OTG controller
	//              which is now asking us to activate (resume) the
	//              bus. In this case, the ISR would have already been
	//              turned OFF.
	// Scenario #2: Remote host has put the bus in suspend state but even
	//              before the host wanted us (peripheral) to wake up,
	//              we want to wake the host up ourself. This is known
	//              as "remote wakeup" functionality. In this case, the
	//              "resume detection ISR" is currently ON, is not useful
	//              anymore and so can be turned OFF.
	// In either case, ISR will be turned OFF.
	mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

	// The USB-controller has asked us to re-activate the bus
	mpEvtUtil->a_bus_resume_evt->SetHigh();

	return true;
}




boolean B_Suspend_State::RemoteHostWakeupSignaling_SetupRequestHdlr(boolean setup_needed)
{
	// Only if remote wakeup capability has been enabled, does it make
	//  sense for OTG controller to prepare the xcvr for remote wakeup functionality!
	if ( mpUtil->remote_wakeup_capability_enabled_flag )
	{
		if ( setup_needed )
		{
			// Setup xcvr for remote wakeup signaling

			// Disable resume signaling detection ISR as this is useful only when the remote
			//  host initiates resume signaling.
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

			// Activate the xcvr so that remote wakeup signaling from USB-OTG controller
			//  passes through even while we are in SUSPEND state
			(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);
		}
		else
		{
			// Remote wakeup signaling setup no longer needed

			// Suspend the xcvr to minimize current drawn from USB_VBUS during
			//  "suspend" state.
			(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND);

			// At this point, the data lines D+/- are in idle state, which means
			//  that D+ is high (because of xcvr's D+ pull-up) and D- is low.

			// Only if OTG controller had registered a non-NULL remote_dev_resumed_cb,
			//  do the following:
			//  - wait for D+ FE to be able to detect both non-idle (resume) as well as SE0
			//    signaling to get the bus out of suspended state.   
			mpEvtUtil->a_bus_K_SE0_evt->Reset();
			if ( NULL != mpUtil->ctrlr_info_curr.remote_dev_resumed_cb )
			{
				mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
					PM_USB_D_PLUS_HI_FE_IRQ_HDL,
					mpEvtUtil->a_bus_K_SE0_evt);
			}
		} //else of "if ( setup_needed )"
	} //if ( remote_wakeup_capability_enabled_flag )

	return true;
}




boolean B_Suspend_State::UsbVbusControlRequestHdlr(boolean need_control)
{
	return mpUtil->UsbVbusControlRequestHdlr(need_control);
}




/**************************************************************************
     class Chg_Conn_State											  
**************************************************************************/

Chg_Conn_State::Chg_Conn_State()
{
	mStateName="CHG_CONN"; 
	mStateId=PM_APP_OTG_STATE__CHG_CONN; 
	mpUsbPowerConsumerConfigInfo = UsbPowerConsumerConfigInfo::GetInstance();
}


Chg_Conn_State::~Chg_Conn_State()
{
}


void Chg_Conn_State::Entry()
{
	pm_app_otg_remote_a_dev_info_type  charger_info;
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();

	mOtgEventPayload = PM_APP_OTG_DEVICE_ATTACHED__USB_WALL_CHARGER;
	mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_DEVICE_ATTACHED,1,(void *) &mOtgEventPayload);

	// pull D+ high and D- low
	(void) pm_otg_config_interface(
		PM_OTG_VBUS_CONFIG_CMD__NONE,
		PM_OTG_DP_CONFIG_CMD__PULL_UP,
		PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
		PM_OTG_ID_CONFIG_CMD__NONE );
	// and start D- debouncer
	mpEvtUtil->dm_evt->Reset();

	mpDmStateMonitor->DmStateMonitorInit(mpUsbBusListener->DmListener, (void*)mpUsbBusListener); 

	// recognize remote A-dev as a USB charger
	charger_info.remote_a_dev = PM_APP_OTG_A_DEV_TYPE__USB_CHARGER;
	// and set info on how much current can be withdrawn from the USB charger
	charger_info.max_i_ma = mpUsbPowerConsumerConfigInfo->pm_app_otg_max_i_ma__upon_charger_connect;
	charger_info.hostSuspended = false;
	pPmAppOtg->SetRemote_A_DevInfo(charger_info);

}



BaseState* Chg_Conn_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpChg_Conn_State;


	if ( mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->dm_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}

	return next_state_ptr;
}



boolean Chg_Conn_State::UsbVbusControlRequestHdlr(boolean need_control)
{
	return mpUtil->UsbVbusControlRequestHdlr(need_control); 
}



/**************************************************************************
     class Unsought_Mode_State											  
**************************************************************************/

Unsought_Mode_State::Unsought_Mode_State()
{
	mStateName="UNSOUGHT_MODE"; 
	mStateId=PM_APP_OTG_STATE__UNSOUGHT_MODE;
}


Unsought_Mode_State::~Unsought_Mode_State()
{
}


void Unsought_Mode_State::Entry()
{
	//mpStateFactory = StateFactory::GetInstance();
	//BaseState* statePtr = mpStateFactory->mpA_Idle_State;
	//printf("current state id : %d ,  state Name : %s \n",statePtr->mStateId,statePtr->mStateName);

	// only one of the following flags may be set to TRUE during entry to this state
	mpUtil->bus_idle_wait_reqd_flag = FALSE;
	mpUtil->bus_idle_or_bus_acquisition_wait_reqd_flag = FALSE;

	if ( mpUtil->IsBusAcquired() )
	{
		// Check if the bus has already been acquired;
		//  if so, the only reason we have ended up here is because of connection
		//  to an unsupported device.
		//  Remedy: Wait for the physical detachment of the device before
		//          exiting this state; otherwise, we will cycle unnecessarily
		//          through this state forever until the device is physically
		//          removed.
		mpUtil->bus_idle_wait_reqd_flag = TRUE;
	}
	else
	{
		// If the bus has not been acquired yet, check if there is some device
		//  on the bus. In this case, we don't know if the device is supported or not.
		//  Remedy: We'll have to conservatively assume that the device is supported,
		//          and we'll exit this state only after an acquisition is made or
		//          after the device is removed.
		if ( !mpUtil->IsBusIdle() )
		{
            #ifdef FEATURE_USB_ON_APPS_PROC 
            // USB is being administered by the applications processor. In this case
            //  we need to ensure that the applications processor is awake otherwise
            //  it will not be possible to determine the characteristics of the 
            //  connected device.
            //  Remedy: If the applications processor is asleep then wake it up to
            //          allow it to reconfigure the USB state machines and 
            //          re-aquire the bus
            if ( !dem_wakeup_apps(DEM_WAKEUP_REASON_OTHER) )
            {
               (void) dem_powerup_apps(DEM_WAKEUP_REASON_OTHER);
            }
            #endif
         
			mpUtil->bus_idle_or_bus_acquisition_wait_reqd_flag = TRUE;
		}
		// else
		//  we shouldn't have come to this state in the first place!
		//  arrange to exit this state immediately.
	}
}



BaseState* Unsought_Mode_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpUnsought_Mode_State;

	// process pending bus acquisition request, if any
	mpUtil->ProcessPendingBusAcquisitionRequest();

	if ( mpUtil->bus_idle_wait_reqd_flag )
	{
		if ( mpUtil->IsBusIdle() )
		{
			// physical detachment has been done, now we are free to process events
			mpUtil->bus_idle_wait_reqd_flag = FALSE;
		}
	}
	else if ( mpUtil->bus_idle_or_bus_acquisition_wait_reqd_flag )
	{
		if ( mpUtil->IsBusIdle()
			|| mpUtil->IsBusAcquired() )
		{
			mpUtil->bus_idle_or_bus_acquisition_wait_reqd_flag = FALSE;
		}
	}

	// Note that we we have a separate 'if' block instead of an 'else' in order
	//  to not miss the last event processing cycle from taking us out of this state.
	//  This could happen if the last event were to be the "physical detachment" in the
	//  the above 'if' block, for instance.   
	if ( ( !mpUtil->bus_idle_wait_reqd_flag
		&& !mpUtil->bus_idle_or_bus_acquisition_wait_reqd_flag )
		|| mpEvtUtil->sm_reset_evt->IsHigh() )
	{
		next_state_ptr =mpStateFactory->mpB_Idle_State;
	}

	return next_state_ptr;
}

#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
/**************************************************************************
     class Rs232Dongle_Config_State											  
**************************************************************************/
Rs232Dongle_Config_State::Rs232Dongle_Config_State()
{
	mStateName="RS232DONGLE_CONFIG"; 
	mStateId=PM_APP_OTG_STATE__RS232DONGLE_CONFIG;
}


Rs232Dongle_Config_State::~Rs232Dongle_Config_State()
{
}


void Rs232Dongle_Config_State::Entry()
{
	// Configure for RS-232 dongle
	mOtgEventPayload = PM_APP_OTG_DEVICE_ATTACHED__RS232_DONGLE;
	mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_DEVICE_ATTACHED,1,(void *) &mOtgEventPayload);

	// save current operational mode
	mpUtil->current_operational_mode = PM_APP_OTG_OPERATIONAL_MODE__RS232DONGLE;

	// vote to turn ON BOOST_5V
	pm_vote_vreg_switch(
		PM_ON_CMD,
		PM_VREG_BOOST_ID,
		PM_VOTE_VREG_BOOST_APP__OTG);

   #ifndef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
      // turn ON VREG_USB
      #ifdef FEATURE_PMIC_KIP_HAN
         (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB3P3_M);
      #else
         (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB_M);
      #endif //FEATURE_PMIC_KIP_HAN    
   #endif //FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

	
    #ifdef FEATURE_PMIC_KIP_HAN  //KIP/HAN: select VREG_5V as the power source for the xcvr
       (void) pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VREG5V);
    #else   // PMIC3: select VREG_5V as the power source for the xcvr
		(void) pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST);
    #endif

	// config OTG xcvr for UART mode
	(void) pm_otg_config_uart_mode();

	// activate the xcvr
	(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

	// drive VBUS with 5V supply and leave pull-down resistor connected on D+/-
	(void) pm_otg_config_interface(
		PM_OTG_VBUS_CONFIG_CMD__DRV,
		PM_OTG_DP_CONFIG_CMD__PULL_DOWN,
		PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
		PM_OTG_ID_CONFIG_CMD__NONE );

	// turn on USB_VBUS assist implemented by the custom h/w
	mpCustomHw->TurnOnUsbVbusBoostAssist();

	// finally ask OTG-controller to initialize itself
	if ( NULL != mpUtil->ctrlr_info_curr.rs232dongle_mode_init_cb )
	{
		mpUtil->ctrlr_info_curr.rs232dongle_mode_init_cb();

		// Remember that an active session has been started with the OTG controller
		mpUtil->active_controller_session_flag = TRUE;
	}

}



BaseState* Rs232Dongle_Config_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpRs232dongle_Config_State;

	static boolean vbus_dischg_in_progress = FALSE;

	if ( vbus_dischg_in_progress )
	{
		if ( mpEvtUtil->a_sess_vld_evt->IsLow() )
		{
			vbus_dischg_in_progress = FALSE;
			next_state_ptr = mpStateFactory->mpB_Idle_State;
		}
	}
	else if ( (!mpUtil->resistive_id_flag)           // RS-232 dongle has been removed, or
		|| mpEvtUtil->a_bus_req_evt->IsLow() )	 	// OTG controller relinquished the bus
	{      
		// turn off USB_VBUS assist implemented by the custom h/w
		mpCustomHw->TurnOffUsbVbusBoostAssist();

		// Drop VBUS
		// To speed things up, we'll discharge VBUS by pulling it to the
		//  ground thro' the DISCHG resistor
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__DISCHG,    // Just disabling driving of
			//  VBUS leaves VBUS floating
			//  and so need to discharge
			//  through a resistor to ground
			PM_OTG_DP_CONFIG_CMD__NONE,
			PM_OTG_DM_CONFIG_CMD__NONE,
			PM_OTG_ID_CONFIG_CMD__NONE );
		//  and wait for VBUS to fall below A-device's "VBUS_Session_Valid" threshold.

		vbus_dischg_in_progress = TRUE;

		// remain in the same state
	}

	return next_state_ptr;
}
#endif  //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION


#endif //FEATURE_PMIC_USB_OTG
