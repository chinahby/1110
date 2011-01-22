/*! \file 
 *  \n
 *  \brief  pmapp_otg_Interface.cpp  ----- PM APP OTG Interface related class implementation
 *  \details This file contains implementation of various classes used in USB-OTG 
 *    application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_Interface.cpp#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/08   aab      Added support for usb host suspended notification
					(usb_power_source_suspended_notification)
08/08/08   aab      Made changes to properly initialize timer object
07/23/08   aab      Made changes to resolve compiler warnings
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_CustomHw.h"    
#include "pmapp_otg_Interface.h"   


extern "C"
{
//#include "comdef.h"
//#include "err.h"
#include "pm66XXvregs.h" 
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

#ifndef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
   #define PM_APP_OTG_VREG_USB_LEVEL    (3300)   // Set VREG_USB to 3.3V
#endif //FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

PmAppOtg_PowConsumer* PmAppOtg_PowConsumer::mpPmAppOtg_PowConsumer_Instance = NULL;
PmAppOtg_Carkit* PmAppOtg_Carkit::mpPmAppOtg_Carkit_Instance = NULL;
PmAppOtg* PmAppOtg::mpPmAppOtg_Instance = NULL;


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
     class PmAppOtg												  
**************************************************************************/
PmAppOtg::PmAppOtg()
{
    mpEvtUtil = EvtUtil::GetInstance();
	mpUtil = Util::GetInstance();
	mpPowerConsumer = PowerConsumer::GetInstance();

    //instantiante pm app otg states
	mpStateManager = StateManager::GetInstance();
	mpStateManager->InitStateManager();

    mpEvtDebugger = EvtDebugger::GetInstance();
	mpUsbBusListener = UsbBusListener::GetInstance();
	mpGenericEvtDetectIsr = GenericEvtDetectIsr::GetInstance();
	mpOtgTimer = OtgTimer::GetInstance();
	mpOtgDiagEvent = OtgDiagEvent::GetInstance();
    mpCustomHw = CustomHw::GetInstance();

}




PmAppOtg::~PmAppOtg()
{
   EvtUtil::DestroyInstance();
   Util::DestroyInstance();
   PowerConsumer::DestroyInstance();

   //destruct pm app otg states
   StateManager::DestroyInstance();
   EvtDebugger::DestroyInstance();
   UsbBusListener::DestroyInstance();
   GenericEvtDetectIsr::DestroyInstance();
   OtgTimer::DestroyInstance();
   OtgDiagEvent::DestroyInstance();
   CustomHw::DestroyInstance();
}


PmAppOtg* PmAppOtg::GetInstance()
{
	if(NULL == mpPmAppOtg_Instance )
	{
		mpPmAppOtg_Instance = new PmAppOtg;
	}
	return mpPmAppOtg_Instance;
}


void PmAppOtg::DestroyInstance()
{
 delete mpPmAppOtg_Instance;
 mpPmAppOtg_Instance = NULL;
}





void PmAppOtg::Init(
					pm_app_otg_ctrlr_init_done_cb_type               init_done_cb,
					pm_app_otg_ctrlr_host_mode_init_cb_type          host_mode_init_cb,
					pm_app_otg_ctrlr_peripheral_mode_init_cb_type    peripheral_mode_init_cb,
					pm_app_otg_ctrlr_rs232dongle_mode_init_cb_type   rs232dongle_mode_init_cb,
					pm_app_otg_ctrlr_carkitclient_mode_init_cb_type        carkitclient_mode_init_cb,
					pm_app_otg_ctrlr_carkitclient_int_hdlr_type            carkitclient_int_hdlr,
					pm_app_otg_ctrlr_carkitclient_mode_entry_rsp_fn_type   carkitclient_mode_entry_rsp_fn,
					pm_app_otg_ctrlr_remote_dev_lost_cb_type         remote_dev_lost_cb,
					pm_app_otg_ctrlr_remote_dev_resumed_cb_type      remote_dev_resumed_cb,
					pm_app_otg_ctrlr_remote_host_wakeup_fn_type      remote_host_wakeup_fn,
					pm_app_otg_ctrlr_error_notification_cb_type      error_notification_cb,
					pm_app_otg_ctrlr_sleep_assert_fn_type            sleep_assert_fn)
{
	static boolean sm_static_init_done = false;
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_INIT);

	// perform all one time initializations here ...
	if ( !sm_static_init_done )
	{
		sm_static_init_done = true;

        #ifndef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
		// set VREG_USB at 3.3V but do not enable it right now
		(void) pm_vreg_set_level(PM_VREG_USB_ID, PM_APP_OTG_VREG_USB_LEVEL);
        #endif //FEATURE_PMIC_NONFUNCTIONAL_VREG_USB


        #ifndef T_WINNT
            // setup the timer for the state machine to be able to start/stop timeouts
            mpOtgTimer->InitTimer();            
        #endif
 
	}

	if ( !mpEvtUtil->sm_init_evt->IsHigh() && (NULL != sleep_assert_fn) )
	{
		// record OTG controller callback info in the shadow
		mpUtil->ctrlr_info_shadow.init_done_cb = init_done_cb;
		mpUtil->ctrlr_info_shadow.host_mode_init_cb = host_mode_init_cb;
		mpUtil->ctrlr_info_shadow.peripheral_mode_init_cb = peripheral_mode_init_cb;
		mpUtil->ctrlr_info_shadow.rs232dongle_mode_init_cb = rs232dongle_mode_init_cb;
		mpUtil->ctrlr_info_shadow.carkitclient_mode_init_cb = carkitclient_mode_init_cb;
		mpUtil->ctrlr_info_shadow.carkitclient_int_hdlr = carkitclient_int_hdlr;
		mpUtil->ctrlr_info_shadow.carkitclient_mode_entry_rsp_fn = carkitclient_mode_entry_rsp_fn;
		mpUtil->ctrlr_info_shadow.remote_dev_lost_cb = remote_dev_lost_cb;
		mpUtil->ctrlr_info_shadow.remote_dev_resumed_cb = remote_dev_resumed_cb;
		mpUtil->ctrlr_info_shadow.remote_host_wakeup_fn = remote_host_wakeup_fn;
		mpUtil->ctrlr_info_shadow.error_notification_cb = error_notification_cb;
		mpUtil->ctrlr_info_shadow.sleep_assert_fn = sleep_assert_fn;

		INTLOCK();
		// ensure coherency of 'active_ctrlr_info_ptr' and 'pm_app_otg_evt__sm_init'
		mpUtil->active_ctrlr_info_ptr = &mpUtil->ctrlr_info_shadow;
		mpEvtUtil->sm_init_evt->SetHigh();
		INTFREE();

		mpStateManager->ProcessCachedEvt();
	}
	else
	{
		ERR_FATAL("pm_app_otg_init() failed!", 0, 0, 0);
	}

}




void PmAppOtg::Reset(void)
{
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_RESET);

	INTLOCK(); // ensure coherency of 'active_ctrlr_info_ptr' and 'pm_app_otg_evt__sm_init'
	// first, cancel any pending SM init requests
	mpEvtUtil->sm_init_evt->Reset();
	// there can be no active OTG controller while in reset
	mpUtil->active_ctrlr_info_ptr = NULL;
	INTFREE();

	// first, indicate a SM reset
	mpEvtUtil->sm_reset_evt->SetHigh();

	// followed by relinquishing any current (/pending) bus acquisition (request)
	RelinquishBus();

	mpStateManager->ProcessCachedEvt();

}



const pm_app_otg_ctrlr_info_type* PmAppOtg::GetCtrlrInfo(void)
{
	return mpUtil->active_ctrlr_info_ptr;
}



void PmAppOtg::AcquireBus(short tmo_secs)
{
	boolean successful = false;
	int32 tmo_secs_payload = tmo_secs;
	mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_ACQUIRE_BUS_REQ,sizeof(tmo_secs_payload), (void*)&tmo_secs_payload);

	// cache this acquisition request and handle it at the appropriate time
	//  using pm_app_otg_process_pending_bus_acquisition_request()

	INTLOCK();
	// check if the bus has already been acquired or if there is an acquisition
	//  request pending already;
	// this is to prevent consecutive pm_app_otg_acquire_bus() without an
	//  intervening pm_app_otg_relinquish_bus()
	if ( !mpUtil->IsBusAcquired()&& !mpUtil->bus_acquisition_pending_flag )
	{
		mpUtil->pending_bus_acquisition_tmo_secs_tmr = (int8)tmo_secs;
		mpUtil->bus_acquisition_pending_flag = TRUE;
		successful = TRUE;
	}
	else
	{
		ERR_FATAL("pm_app_otg_acquire_bus() - Caller violated bus acquisition protocol", 0, 0, 0);
	}
	INTFREE();

	if (successful)
	{
		mpStateManager->ProcessCachedEvt();
	}

}




void PmAppOtg::RelinquishBus(void)
{
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_RELINQUISH_BUS_REQ);

	INTLOCK();
	// first cancel any pending bus acquisition requests
	mpUtil->bus_acquisition_pending_flag = FALSE;

	mpUtil->ab_wait_bus_acquisition_tmr = 0;

	// set A-dev statemachine events
	mpEvtUtil->a_bus_drop_evt->SetHigh();
	mpEvtUtil->a_bus_req_evt->SetLow();

	// set B-dev statemachine events
	mpEvtUtil->b_bus_req_evt->SetLow();
	INTFREE();

	mpStateManager->ProcessCachedEvt();
}




unsigned int PmAppOtg::GetLocal_Vbus_CurrentSourcingCapability(void)
{
	// Return the MAX of capability of standalone xcvr, and any enhancements
	//  made possible by the custom h/w on board
	uint32 custom_hw_usb_vbus_boost_assist_capability = mpCustomHw->GetUsbVbusBoostAssistCapability();
	return MAX(
		25,  // replace it with PMIC library API
		custom_hw_usb_vbus_boost_assist_capability );

}




void PmAppOtg::SetRemote_A_DevInfo( pm_app_otg_remote_a_dev_info_type info)
{
	mpOtgDiagEvent->OtgEventHandleRemote_a_DevInfo(EVENT_PM_APP_OTG_PERI_MODE_SET_REM_A_DEV_INFO,info.remote_a_dev, (unsigned short)info.max_i_ma);

	INTLOCK();
	mpUtil->remote_a_dev_info = info;
	INTFREE();


	//only if power consumer is registered
	if(mpUtil->usb_power_consumer_interested_flag)
	{
		mpPowerConsumer->UsbPowerConsumerEvtHdlr(NULL, &mpUtil->remote_a_dev_info);
	}

}




void PmAppOtg::Suspend(void)
{
	boolean status = false;
	BaseState* pCurrState = mpStateManager->GetCurrState();
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_SUSPEND);

	INTLOCK();
	status = pCurrState->SuspendEvtHdlr();
	INTFREE();

	if(status)
	{
        mpStateManager->ProcessCachedEvt();
	}
}




void PmAppOtg::Resume(void)
{
	boolean status = false;
	BaseState* pCurrState = mpStateManager->GetCurrState();
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_RESUME);

	INTLOCK();
	status = pCurrState->ResumeEvtHdlr();
	INTFREE();

	if (status)
	{
		mpStateManager->ProcessCachedEvt();
	}
}




void PmAppOtg::ProcessRemotePeripheral_DisconnectedEvt(void)
{
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_HOST_MODE_REM_PERI_DIS);

	INTLOCK();
	mpEvtUtil->b_conn_evt->SetLow();
	INTFREE();

	mpStateManager->ProcessCachedEvt();
}





void PmAppOtg::SetRemote_WakeupCapability( boolean  remote_wakeup_capability_enabled)
{
	mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_SET_REM_WAKEUP_CAPABILITY, sizeof(remote_wakeup_capability_enabled),(void*) &remote_wakeup_capability_enabled);

	if(remote_wakeup_capability_enabled)
	{
		mpEvtUtil->remote_wakeup_evt->SetHigh();
	}
	else
	{
		mpEvtUtil->remote_wakeup_evt->SetLow();
	}

	mpStateManager->ProcessCachedEvt();
}





void PmAppOtg::PrepareFor_RemoteHostWakeupSignaling(void)
{
	boolean status = false;
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_PERI_MODE_PREPARE_FOR_REM_HOST_WAKEUP_SIG);

	BaseState* pCurrState = mpStateManager->GetCurrState();

	INTLOCK();
	status = pCurrState->RemoteHostWakeupSignaling_SetupRequestHdlr(true); // setup needed
	INTFREE();

	if (status)
	{
		mpStateManager->ProcessCachedEvt();
	}
}





void PmAppOtg::ProcessRemoteHost_WakeupSignalingDoneEvt(void)
{
	boolean status = false;
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_PERI_MODE_REM_HOST_WAKEUP_SIG_DONE);

	BaseState* pCurrState = mpStateManager->GetCurrState();

	INTLOCK();
	status = pCurrState->RemoteHostWakeupSignaling_SetupRequestHdlr(false); // setup no longer needed
	INTFREE();

	if (status) //if current state implements RemoteHostWakeupSignaling_SetupRequestHdlr
	{
		mpStateManager->ProcessCachedEvt();
	}
}



#ifdef FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE
void PmAppOtg::Start_B_DevSession(void)
{
	pm_app_otg_acquire_bus(-1);
	mpUsbBusListener->VbusListener(OTG_VBUS_SM_INFO__VBUS_SESSION_VALID,mpUsbBusListener);
}





void PmAppOtg::Stop_B_DevSession(void)
{
	pm_app_otg_relinquish_bus();
	mpUsbBusListener->VbusListener(OTG_VBUS_SM_INFO__VBUS_SESSION_INVALID,mpUsbBusListener);
}
#endif    //FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE





pm_app_otg_operational_mode_type PmAppOtg::GetCurrentOperationalMode(void)
{
	return mpUtil->current_operational_mode;
}





/**************************************************************************
     class PmAppOtg_PowConsumer											  
**************************************************************************/
PmAppOtg_PowConsumer::PmAppOtg_PowConsumer()
{
   PmAppOtg* pPmAppOtg;
   if(NULL == PmAppOtg::mpPmAppOtg_Instance)
   {
      pPmAppOtg = PmAppOtg::GetInstance();
   }

	mpPowerConsumer = PowerConsumer::GetInstance();
	mpUsbPowerConsumerConfigInfo = UsbPowerConsumerConfigInfo::GetInstance();
	mpOtgDiagEvent = OtgDiagEvent::GetInstance();
}


PmAppOtg_PowConsumer::~PmAppOtg_PowConsumer()
{
}



PmAppOtg_PowConsumer* PmAppOtg_PowConsumer::GetInstance()
{
	if(NULL == mpPmAppOtg_PowConsumer_Instance )
	{
		mpPmAppOtg_PowConsumer_Instance = new PmAppOtg_PowConsumer;
	}
	return mpPmAppOtg_PowConsumer_Instance;
}



void PmAppOtg_PowConsumer::DestroyInstance()
{
 delete mpPmAppOtg_PowConsumer_Instance;
 mpPmAppOtg_PowConsumer_Instance = NULL;
}






void PmAppOtg_PowConsumer::ConfigureUsbPowerConsumer(
	pm_app_otg_usb_power_source_connected_notification_cb_type    source_connected_cb,
	pm_app_otg_usb_power_source_disconnected_notification_cb_type source_disconnected_cb,
	pm_app_otg_usb_power_sink_open_notification_cb_type           sink_open_notification_cb,
	pm_app_otg_usb_power_sink_close_notification_cb_type          sink_close_notification_cb,
	pm_app_otg_usb_power_source_suspended_notification_cb_type	  source_suspended_notification_cb )
{
#if !defined(FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE) \
	|| defined(FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE_BUT_USB_POWER_MGMT_REQD)

	//pm_app_otg_usb_power_consumer_config_info_type   info;
	mpOtgDiagEvent->OtgEventHandle(EVENT_PM_APP_OTG_CONFIGURE_USB_POWER_CONSUMER);

	if ( source_connected_cb
		&& source_disconnected_cb
		&& sink_open_notification_cb
		&& sink_close_notification_cb
		&& source_suspended_notification_cb)
	{
		INTLOCK();
		mpUsbPowerConsumerConfigInfo->source_connected_notification_cb = source_connected_cb;
		mpUsbPowerConsumerConfigInfo->source_disconnected_notification_cb = source_disconnected_cb;
		mpUsbPowerConsumerConfigInfo->sink_open_notification_cb = sink_open_notification_cb;
		mpUsbPowerConsumerConfigInfo->sink_close_notification_cb = sink_close_notification_cb;
		mpUsbPowerConsumerConfigInfo->source_suspended_notification_cb = source_suspended_notification_cb;
		INTFREE();

		mpPowerConsumer->UsbPowerConsumerEvtHdlr(NULL, NULL);
	}

#endif //FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE


}



void PmAppOtg_PowConsumer::SetUsbPowerConsumptionRequirement( boolean need_usb_power)
{
#if !defined(FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE) \
	|| defined(FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE_BUT_USB_POWER_MGMT_REQD)

	mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_SET_USB_POWER_CONSUMPTION_REQUIREMENT, sizeof(need_usb_power),(void*) &need_usb_power);

	mpPowerConsumer->UsbPowerConsumerEvtHdlr(&need_usb_power, NULL);
#endif //FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE

}



boolean PmAppOtg_PowConsumer::ProcessUsbPowerLineControlRequest( boolean need_control)
{
	boolean status = FALSE;

#ifndef FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE
	//pm_app_otg_usb_vbus_control_request_hdlr_type usb_vbus_control_request_hdlr = NULL;
	StateManager* pStateManager = StateManager::GetInstance();
	BaseState* pCurrState = pStateManager->GetCurrState();


	INTLOCK();
	//if ( NULL != (usb_vbus_control_request_hdlr = pm_app_otg_curr_state_ptr->process_usb_vbus_control_request) )
	//{
	mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_PERI_MODE_PROCESS_USB_POWER_LINE_CONT_REQ, sizeof(need_control),(void*) &need_control);

	status = pCurrState->UsbVbusControlRequestHdlr(need_control);
	//}
	INTFREE();
#endif //FEATURE_PMIC_USB_VBUS_POWER_UNAVAILABLE

	return status;

}






/**************************************************************************
     class PmAppOtg_Carkit											  
**************************************************************************/
PmAppOtg_Carkit::PmAppOtg_Carkit()
{
   PmAppOtg* pPmAppOtg;
   if(NULL == PmAppOtg::mpPmAppOtg_Instance)
   {
      pPmAppOtg = PmAppOtg::GetInstance();
   }

	mpStateManager = StateManager::GetInstance();
	mpUtil = Util::GetInstance();
}


PmAppOtg_Carkit::~PmAppOtg_Carkit()
{
}


PmAppOtg_Carkit* PmAppOtg_Carkit::GetInstance()
{
	if(NULL == mpPmAppOtg_Carkit_Instance )
	{
		mpPmAppOtg_Carkit_Instance = new PmAppOtg_Carkit;
	}
	return mpPmAppOtg_Carkit_Instance;
}


void PmAppOtg_Carkit::DestroyInstance()
{
 delete mpPmAppOtg_Carkit_Instance;
 mpPmAppOtg_Carkit_Instance = NULL;
}





boolean PmAppOtg_Carkit::EnterCarkitMode( pm_app_otg_carkit_mode_type carkit_mode)
{
	boolean status = false;
	BaseState* pCurrState = mpStateManager->GetCurrState();

	INTLOCK();
	status = pCurrState->CarkitModeEntryReqHdlr(carkit_mode);
	INTFREE();

	if ( status )
	{
        mpStateManager->ProcessCachedEvt();
	}

	return status;

}



void PmAppOtg_Carkit::EnableCarkitInterruptDetection(void)
{
	mpUtil->need_to_enable_cr_int_det_flag = TRUE;
	mpStateManager->ProcessCachedEvt();
}



void PmAppOtg_Carkit::DisableCarkitInterruptDetection(void)
{
	mpUtil->need_to_enable_cr_int_det_flag = FALSE;
	mpStateManager->ProcessCachedEvt();
}



boolean PmAppOtg_Carkit::DataDuringAudioSupported(void)
{
	// return FALSE until PM7500 support is introduced in the PMIC library
	return FALSE;
}


#endif //FEATURE_PMIC_USB_OTG
