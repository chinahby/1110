/*! \file 
*  \n
*  \brief  pmapp_otg_StateHostMode.cpp  ----- PM APP OTG State machine related class implementation
*  \details This file contains implementation of classes for OTG states 
*     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_StateHostMode.cpp#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/08   aab      Modified A_Idle_State::Entry() to support USB LDO input power configuration for KIP/HAN
                    Added FEATURE_PMIC_KIP_HAN to support KIP and HAN PMIC's regester defination variation
08/04/08   aab      Added FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_Interface.h"   
extern "C"
{
	/* //Debug:  unit test 
	#include "pm66XXotg.h"
	*/
}

#ifdef FEATURE_PMIC_USB_OTG
#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION

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
     class A_Idle_State											  
**************************************************************************/
A_Idle_State::A_Idle_State()
{
	mStateName="A_IDLE";
	mStateId=PM_APP_OTG_STATE__A_IDLE;
}


A_Idle_State::~A_Idle_State()
{
}


void A_Idle_State::Entry()
{
	mpStateFactory = StateFactory::GetInstance();
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();

	// reset current operational mode
	mpUtil->current_operational_mode = PM_APP_OTG_OPERATIONAL_MODE__NONE;

	// notify USB ctrlr that the remote device was lost
	if ( mpUtil->active_controller_session_flag )
	{
		// Just before OTG controller is notified about the end of the current
		//  USB Host mode session, fake a "relinquish" bus request to properly
		//  terminate the current USB session. This will prevent future bus
		//  acquisitions from automatically occuring until OTG controller
		//  explicitly "acquires" the bus again!
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

	// select BOOST_5V as the power source for the xcvr
    #ifdef FEATURE_PMIC_KIP_HAN //KIP/HAN: select VREG_5V as the power source for the xcvr
         pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VREG5V);
    #else // PMIC3: select VREG_5V as the power source for the xcvr
         (void) pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST);
    #endif
    
}




BaseState* A_Idle_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpA_Idle_State;

	if(mpEvtUtil->id_evt->IsHigh()				 // mini-A plug was removed
		|| mpEvtUtil->a_bus_req_evt->IsLow() )	 // or bus has been relinquished
	{
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}
	else if(mpEvtUtil->a_bus_drop_evt->IsLow() 
		&& ( mpEvtUtil->a_bus_req_evt->IsHigh() || mpEvtUtil->a_srp_det_evt->IsHigh())  )
	{
		next_state_ptr = mpStateFactory->mpA_Wait_Vrise_State;
	}

	return next_state_ptr;

}




/**************************************************************************
     class A_Wait_Vrise_State											  
**************************************************************************/

A_Wait_Vrise_State::A_Wait_Vrise_State()
{
	mStateName="A_WAIT_VRISE";
	mStateId=PM_APP_OTG_STATE__A_WAIT_VRISE;
	pm_app_otg_tmr__a_wait_vrise = 100;
}


A_Wait_Vrise_State::~A_Wait_Vrise_State()
{
}




void A_Wait_Vrise_State::Entry()
{
	mpStateFactory = StateFactory::GetInstance();

	// vote to turn ON BOOST_5V
	pm_vote_vreg_switch(
		PM_ON_CMD,
		PM_VREG_BOOST_ID,
		PM_VOTE_VREG_BOOST_APP__OTG);

   #ifndef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
      // turn ON VREG_USB
      #ifdef FEATURE_PMIC_KIP_HAN
         (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB3P3_M);
      #else //PMIC3
         (void) pm_vreg_control(PM_ON_CMD, PM_VREG_USB_M);
      #endif //FEATURE_PMIC_KIP_HAN        
   #endif //FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

	(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

	// drive VBUS with 5V supply and wait for the voltage to rise by starting
	// "a_wait_vrise" timer; also D+/- must have pull down resistors attached.
	(void) pm_otg_config_interface(
		PM_OTG_VBUS_CONFIG_CMD__DRV,
		PM_OTG_DP_CONFIG_CMD__PULL_DOWN,
		PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
		PM_OTG_ID_CONFIG_CMD__NONE );

	// turn on USB_VBUS assist implemented by the custom h/w
	mpCustomHw->TurnOnUsbVbusBoostAssist();

	mpOtgTimer->StartTimer(pm_app_otg_tmr__a_wait_vrise, mpEvtUtil->a_wait_vrise_tmout_evt);

}




BaseState* A_Wait_Vrise_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpA_Wait_Vrise_State;

	if( mpEvtUtil->id_evt->IsHigh()						// Mini-A plug was removed
		|| mpEvtUtil->a_bus_drop_evt->IsHigh()			// or TBD
		// Ignore vbus_valid and wait for Ta_wait_vrise(100ms max) to expire
		//  thereby ignoring the initial loading of Vbus soon after the remote peripheral
		//  is connected to us (host). At the end of the timeout interval, Vbus should
		//  have settled down and from that point onwards, vbus_valid can be used reliably.
		// || PM_APP_OTG_EVT_IS_HIGH(pm_app_otg_evt__a_vbus_vld)           // or VBUS_Valid
		|| mpEvtUtil->a_wait_vrise_tmout_evt->IsHigh() )// or wait for VBUS_Valid timed out
	{
		mpOtgTimer->StopTimer(mpEvtUtil->a_wait_vrise_tmout_evt);
		mpEvtUtil->a_wait_vrise_tmout_evt->Reset();
		next_state_ptr = mpStateFactory->mpA_Wait_Bcon_State;
	}

	return next_state_ptr;
}






/**************************************************************************
     class A_Wait_Bcon_State											  
**************************************************************************/
A_Wait_Bcon_State::A_Wait_Bcon_State()
{
	mStateName = "A_WAIT_BCON"; 
	mStateId = PM_APP_OTG_STATE__A_WAIT_BCON; 
	pm_app_otg_tmr__a_wait_bcon = 1000; // = 1000ms min value for Ta_wait_bcon
	pm_app_otg_tmr__shadow_ab_wait_bus_acquisition = 0;// shadow copy to operate on
}


A_Wait_Bcon_State::~A_Wait_Bcon_State()
{
}



void A_Wait_Bcon_State::Entry()
{
	mpStateFactory = StateFactory::GetInstance();
	// we haven't seen the remote B-dev yet
	mpUtil->usb_speed = PM_OTG_USB_SPEED__INVALID;
	mpEvtUtil->b_conn_evt->SetLow();

	// initialize the shadow variable for bus acquisition timer and operate on
	//  only the shadow copy while in this state
	pm_app_otg_tmr__shadow_ab_wait_bus_acquisition = mpUtil->ab_wait_bus_acquisition_tmr;

	// Start "a_wait_bcon" timer to wait for the B-device to signal a connection
	mpOtgTimer->StartTimer(pm_app_otg_tmr__a_wait_bcon, mpEvtUtil->a_wait_bcon_tmout_evt);
}



BaseState* A_Wait_Bcon_State::CachedEvtHdlr()
{
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();

	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpA_Wait_Bcon_State;


	if ( mpEvtUtil->id_evt->IsHigh()                 // Mini-A plug was removed
		|| mpEvtUtil->a_bus_drop_evt->IsHigh() )      // or TBD
	{
		mpOtgTimer->StopTimer(mpEvtUtil->a_wait_bcon_tmout_evt);

		next_state_ptr = mpStateFactory->mpA_Wait_Vfall_State;
	}
	else if (mpEvtUtil->a_wait_bcon_tmout_evt->IsHigh())
	{
		mpEvtUtil->a_wait_bcon_tmout_evt->Reset();
		mpOtgTimer->StopTimer(mpEvtUtil->a_wait_bcon_tmout_evt);

		// check if over-current situation has already happened, and process it here
		if (mpEvtUtil->a_vbus_vld_evt->IsLow() )      // VBUS fell below the "valid" threshold
		{
			next_state_ptr = mpStateFactory->mpA_vbus_Err_State;
		}
		else
		{
			//Time to determine the remote peripheral's chosen signaling speed
			mpUtil->usb_speed = mpUtil->DetermineRemotePeripheralSpeed();

			if (PM_OTG_USB_SPEED__INVALID == mpUtil->usb_speed)
			{
				// there is no remote B-dev on the other end of the cable; check if
				// we had been requested by USB-OTG controller to wait for some more time
				// for the connection to occur!
				if ( pm_app_otg_tmr__shadow_ab_wait_bus_acquisition > 0 )   // this check is there to avoid
					// the undesirable complications
					// of ever increasing -ve value
				{            
					pm_app_otg_tmr__shadow_ab_wait_bus_acquisition -= pm_app_otg_tmr__a_wait_bcon;
				}

				if ( pm_app_otg_tmr__shadow_ab_wait_bus_acquisition > 0 )
				{
					// restart "a_wait_bcon" timer to wait for the B-device to signal a connection
					mpOtgTimer->StartTimer(pm_app_otg_tmr__a_wait_bcon, mpEvtUtil->a_wait_bcon_tmout_evt);
					// remain in the same state
				}
				else
				{
					mOtgEventPayload = PM_APP_OTG_OPERATIONAL_ERROR__ACQUIRE_BUS_TIMEOUT;
					mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_OPERATIONAL_ERROR,1,(void *) &mOtgEventPayload);
					next_state_ptr = mpStateFactory->mpA_Wait_Vfall_State;

					// fake a bus relinquish request so that we don't continue to scan for remote B-dev until
					//  a new bus acquisition request comes from USB-OTG controller
					pPmAppOtg->RelinquishBus();

					// notify USB-OTG controller about B-dev detection failure
					if ( NULL != mpUtil->ctrlr_info_curr.error_notification_cb )
					{
						mpUtil->ctrlr_info_curr.error_notification_cb(
							PM_APP_OTG_OPERATIONAL_ERROR_TYPE__REMOTE_DEV_NOT_PRESENT);
					}
					// move to A_WAIT_VFALL state
				}
			}
			else
			{
				// remote peripheral signalled a "connect" indication!
				mpEvtUtil->b_conn_evt->SetHigh();
				mpUtil->a_host_resumption_flag = FALSE; // we are entering A_HOST for the first time
				//  since the last USB session (if any)
				next_state_ptr = mpStateFactory->mpA_Host_State;
			}
		}
	}
	else if ( mpEvtUtil->a_vbus_vld_evt->IsLow() )      // VBUS fell below the "valid" threshold
	{
		// process this as part of 'pm_app_otg_evt__a_wait_bcon_tmout' processing;
		// to avoid running into the situation of a transient PM_APP_OTG_EVT_IS_LOW(pm_app_otg_evt__a_vbus_vld)
		// occuring just before 'pm_app_otg_evt__a_wait_bcon_tmout', restart the timeout now ...
		mpOtgTimer->StopTimer(mpEvtUtil->a_wait_bcon_tmout_evt);

		mpOtgTimer->StartTimer(pm_app_otg_tmr__a_wait_bcon, mpEvtUtil->a_wait_bcon_tmout_evt);

		// remain in the same state
	}

	return next_state_ptr;
}




/**************************************************************************
     class A_Host_State											  
**************************************************************************/

A_Host_State::A_Host_State()
{
	mStateName="A_HOST"; 
	mStateId=PM_APP_OTG_STATE__A_HOST;
}


A_Host_State::~A_Host_State()
{
}



void A_Host_State::Entry()
{
	// we are coming into this state either for the first time or just after
	//  the recent resume operation. In either case, ensure that a_suspend_req
	//  event does not remain active from the past.
	mpEvtUtil->a_suspend_req_evt->Reset();

	if ( !mpUtil->a_host_resumption_flag )
	{
		// We are entering host mode for the first time during this session
		mOtgEventPayload = PM_APP_OTG_DEVICE_ATTACHED__PERI_DEVICE;
		mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_DEVICE_ATTACHED,1,(void *) &mOtgEventPayload);

		// save current operational mode
		mpUtil->current_operational_mode = PM_APP_OTG_OPERATIONAL_MODE__HOST;

		// Configure OTG transceiver for A-device/USB-host mode
		// USB speed is determined actually by the pull-up on D+ or D-
		(void) pm_otg_config_usb_mode(PM_OTG_DEV__A, mpUtil->usb_speed);

		// Notify "OTG controller" that "B-device" has connected, so that it can
		// reset the bus to prepare the "B-device" for packet traffic ...
		if ( NULL != mpUtil->ctrlr_info_curr.host_mode_init_cb )
		{
			mpUtil->ctrlr_info_curr.host_mode_init_cb(mpUtil->usb_speed);

			// Remember that an active session has been started with the OTG controller
			mpUtil->active_controller_session_flag = TRUE;
		}      
	} 
}




BaseState* A_Host_State::CachedEvtHdlr()
{

	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpA_Host_State;


	if( mpEvtUtil->id_evt->IsHigh()				// Mini-A plug was removed
		|| mpEvtUtil->a_bus_drop_evt->IsHigh()	// or TBD
		|| mpEvtUtil->b_conn_evt->IsLow()	)	// or "B-device" disconnected
	{

		next_state_ptr = mpStateFactory->mpA_Wait_Vfall_State; // was a_wait_bcon
	}
	else if ( mpEvtUtil->a_vbus_vld_evt->IsLow() )      // VBUS fell below the "valid" threshold
	{
		next_state_ptr = mpStateFactory->mpA_vbus_Err_State;
	}
	else if ( mpEvtUtil->a_suspend_req_evt->IsHigh())
	{
		mpEvtUtil->a_suspend_req_evt->Reset();
		next_state_ptr = mpStateFactory->mpA_Suspend_State;
	}

	return next_state_ptr;

}



boolean A_Host_State::SuspendEvtHdlr()
{
	// Our OTG controller has stopped all activities on the bus, and has
	//  put the bus in SUSPEND state as per USB2.0
	// Note that we must continue to maintain the session though.
	mpEvtUtil->a_suspend_req_evt->SetHigh();
	return true;
}




/**************************************************************************
     class A_Suspend_State											  
**************************************************************************/
A_Suspend_State::A_Suspend_State()
{
	mStateName="A_SUSPEND";
	mStateId=PM_APP_OTG_STATE__A_SUSPEND;
}


A_Suspend_State::~A_Suspend_State()
{
}


void A_Suspend_State::Entry()
{
	pm_irq_hdl_type b_bus_resume_irq = PM_NUM_OF_IRQ_HDL;

	// Reset any events that might be stale from past transition through this state!
	mpEvtUtil->a_suspend_req_evt->Reset();

	// Suspend the xcvr to minimize battery drain during "suspend" state.
	(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND);

	// At this point, the data lines D+/- are in idle (J) state.
	// Note that the state of D+/- lines at this point is dependent upon the
	//  remote peripheral's chosen speed of operation (usb_speed).

	// Only if OTG controller had registered a non-NULL remote_dev_resumed_cb,
	//  and if 'remote_wakeup' capability is currently enabled, do the following:
	//  - wait for FE of remote peripheral's pulled up data line in order to
	//    detect both non-idle (resume) as well as SE0 signaling to get the
	//    bus out of suspended state.
	// CAUTION: Detecion behavior will change when HNP functionality is added as follows!!!
	//          [1] Give precedence to 'HNP' feature over 'remote_wakeup' capability
	//          [2] If 'HNP' is enabled, look only for a J->SE0
	//          [3] If 'HNP' is not set but 'remote_wakeup' is set, then look only
	//               for both J->K as well as J->SE0 to be entirely USB2.0 compliant.
	mpEvtUtil->b_bus_resume_evt->Reset();

	if ( mpUtil->remote_wakeup_capability_enabled_flag 
		&& (NULL != mpUtil->ctrlr_info_curr.remote_dev_resumed_cb) )
	{
		b_bus_resume_irq = (PM_OTG_USB_SPEED__LOW == mpUtil->usb_speed)
			? PM_USB_D_MINUS_HI_FE_IRQ_HDL
			: PM_USB_D_PLUS_HI_FE_IRQ_HDL;
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
			b_bus_resume_irq,
			mpEvtUtil->b_bus_resume_evt);
	}
}



BaseState* A_Suspend_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpA_Suspend_State;

	if ( mpEvtUtil->id_evt->IsHigh()                    // Mini-A plug was removed
		|| mpEvtUtil->a_bus_drop_evt->IsHigh()  )       // or OTG controller relinquished the bus
	{
		next_state_ptr = mpStateFactory->mpA_Wait_Vfall_State;
	}
	else if ( mpEvtUtil->a_vbus_vld_evt->IsLow() )      // VBUS fell below the "valid" threshold
	{
		next_state_ptr = mpStateFactory->mpA_vbus_Err_State;
	}
	else if (mpEvtUtil->b_bus_resume_evt->IsHigh() )
	{
		mpEvtUtil->b_bus_resume_evt->Reset();

		// This is the ISR for detecting remote wakeup signaling initiated by
		// the remote USB B-device acting in USB Peripheral mode to get the
		// USB bus out of SUSPEND state. Notify the OTG controller about remote device's
		// attempt to resume the bus.

		if ( NULL != mpUtil->ctrlr_info_curr.remote_dev_resumed_cb )
		{
			mpUtil->ctrlr_info_curr.remote_dev_resumed_cb();
		}

		// It is to be noted that this ISR by itself will NOT activate the transceiver;
		// it has to be explicitly requested for by the OTG controller using 
		// pm_app_otg_resume().

		// Continue to remain in the same state until 'a_suspend_req' becomes low
		//  as a result of pm_app_otg_resume().
	}
	else if ( mpEvtUtil->a_suspend_req_evt->IsLow())
	{
		mpEvtUtil->a_suspend_req_evt->Reset();

		// OTG controller wants to resume the bus

		// get the xcvr out of SUSPEND state
		(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

		mpUtil->a_host_resumption_flag = TRUE;  // NOT entering A_HOST for the first time
		//  during this (host mode) session.
		next_state_ptr = mpStateFactory->mpA_Host_State;
	}
	else if ( mpEvtUtil->b_conn_evt->IsLow() )
	{
		// Handling this depends on whether HNP is enabled or not.

		// Case HNP is ENABLED. Consider this as HNP hand-shake and move to A_PERIPHERAL

		// Case: HNP is DISABLED. Treat it as a disconnect.
		next_state_ptr = mpStateFactory->mpA_Wait_Vfall_State;
	}

	// perform clean-ups before exiting this state
	if ( mpStateFactory->mpA_Suspend_State != next_state_ptr )
	{
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();
	}

	return next_state_ptr;
}




boolean A_Suspend_State::ResumeEvtHdlr()
{
	// There are 2 scenarios in which this could be called ...
	// Scenario #1: Our Host controller has put the bus in suspend state but the
	//              remote B-device (acting in Peripheral mode) is performing
	//              remote wakeup signaling. In this case, our "remote_wakeup"
	//              detection ISR would have already triggered a callback
	//              into Host controller which is now asking us to activate
	//              (resume) the bus. In this case, the ISR would have
	//              already been turned OFF.
	// Scenario #2: Our Host controller has put the bus in suspend state but
	//              before the remote peripheral attempts to issue "remote_wakeup"
	//              signaling (if it was configured to allow remote_wakeup signaling
	//              in the first place!), our host controller wants to resume the
	//              bus. In this case, the "resume detection ISR" may be currently ON,
	//              is not useful anymore and so can be turned OFF.
	// In either case, ISR will be turned OFF.

	mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

	// The USB-controller has asked us to exit the suspend state
	mpEvtUtil->a_suspend_req_evt->SetLow();

	return true;
}





/**************************************************************************
     class A_Vbus_Err_State											  
**************************************************************************/

A_Vbus_Err_State::A_Vbus_Err_State()
{
	mStateName="A_VBUS_ERR"; 
	mStateId=PM_APP_OTG_STATE__A_VBUS_ERR;
}


A_Vbus_Err_State::~A_Vbus_Err_State()
{
}



void A_Vbus_Err_State::Entry()
{
	// This is a Vbus over-current condition, so drop the Vbus immediately.
	//  Notify the "system software" and sit in this state until the
	//  recovery which may be one of:
	//   - removal of Mini-A plug
	//   - "system software" indicating its desire to clear this error condition

	// turn off USB_VBUS assist implemented by the custom h/w
	mpCustomHw->TurnOffUsbVbusBoostAssist();

	// drop Vbus
	(void) pm_otg_config_interface(
		PM_OTG_VBUS_CONFIG_CMD__DISCHG,    // Just disabling driving of
		//  VBUS leaves VBUS floating
		//  and so need to discharge
		//  through a resistor to ground
		PM_OTG_DP_CONFIG_CMD__NONE,
		PM_OTG_DM_CONFIG_CMD__NONE,
		PM_OTG_ID_CONFIG_CMD__NONE );

	// notify USB-OTG controller about Vbus over-current situation
	if ( NULL != mpUtil->ctrlr_info_curr.error_notification_cb )
	{
		mOtgEventPayload = PM_APP_OTG_OPERATIONAL_ERROR__BUS_OVERCURRENT_STATE;
		mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_OPERATIONAL_ERROR,1,(void *) &mOtgEventPayload);
		mpUtil->ctrlr_info_curr.error_notification_cb(PM_APP_OTG_OPERATIONAL_ERROR_TYPE__BUS_OVERCURRENT_STATE);
	}
}




BaseState* A_Vbus_Err_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpA_vbus_Err_State;

	if ( mpEvtUtil->id_evt->IsHigh()                      // Mini-A plug was removed
		|| mpEvtUtil->a_bus_drop_evt->IsHigh() )         // or there was a bus relinquish request
	{
		next_state_ptr = mpStateFactory->mpA_Wait_Vfall_State;
	}

	return next_state_ptr;
}




/**************************************************************************
     class A_Wait_Vfall_State											  
**************************************************************************/

A_Wait_Vfall_State::A_Wait_Vfall_State()
{
	mStateName="A_WAIT_VFALL"; 
	mStateId=PM_APP_OTG_STATE__A_WAIT_VFALL;
}


A_Wait_Vfall_State::~A_Wait_Vfall_State()
{
}


void A_Wait_Vfall_State::Entry()
{
	// turn off USB_VBUS assist implemented by the custom h/w
	mpCustomHw->TurnOffUsbVbusBoostAssist();

	// drop VBUS
	// To speed things up, we'll discharge VBUS by pulling it to the
	// ground thro' the DISCHG resistor.
	(void) pm_otg_config_interface(
		PM_OTG_VBUS_CONFIG_CMD__DISCHG,    // Just disabling driving of
		//  VBUS leaves VBUS floating
		//  and so need to discharge
		//  through a resistor to ground
		PM_OTG_DP_CONFIG_CMD__NONE,
		PM_OTG_DM_CONFIG_CMD__NONE,
		PM_OTG_ID_CONFIG_CMD__NONE );
	// and wait for VBUS to fall below A-device's "VBUS_Session_Valid" threshold.

}



BaseState* A_Wait_Vfall_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpA_Wait_Vfall_State;

	if ( mpEvtUtil->a_sess_vld_evt->IsLow()    // VBUS is below our sess_valid threshold,
		&& mpEvtUtil->b_conn_evt->IsLow() )   //  and remote B-dev has disconnected
	{
		next_state_ptr = mpStateFactory->mpA_Idle_State;
	}

	return next_state_ptr;
}


#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
#endif //FEATURE_PMIC_USB_OTG
