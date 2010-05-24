/*! \file 
*  \n
*  \brief  pmapp_otg_Util.cpp ------ PM APP OTG state manager related utility class implementation
*  \details This file contains implementation of various utility classes used by the 
*   USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_Util.cpp#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/25/08   aab      Modified UsbVbusControlRequestHdlr()to support HAN/KIP targets
                    Added FEATURE_PMIC_KIP_HAN to support KIP and HAN PMIC's regester defination variation
10/17/08   aab      Made a change o fix suspend/resume state charging
10/14/08   aab      Made changes so that USB host suspend state notification callback 
                     gets called only during host suspend state
08/18/08   aab      Added support for usb host suspended notification
				    (usb_power_source_suspended_notification)
08/08/08   aab      Made changes to properly initialize timer object
08/04/08   aab      Commented out the use of Vectors.  It was added to support possible future expansion
07/23/08   aab      Made changes to resolve compiler warnings
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_StateManager.h"   

#ifdef FEATURE_PMIC_USB_OTG

/* ==========================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//initialize static variables
static timer_group_type  pm_app_otg_timer_group;

EvtDebugger* EvtDebugger::mpEvtDebugger_Instance=NULL;
Util* Util::mpUtil_Instance;
EvtUtil* EvtUtil::mpEvtUtil_Instance;
PowerConsumer* PowerConsumer::mpPowerConsumer_Instance;
EvtGen* EvtGen::mpEvtGen_Instance;
UsbBusListener* UsbBusListener::mpUsbBusListener_Instance;
GenericEvtDetectIsr* GenericEvtDetectIsr::mpGenericEvtDetectIsr_Instance;
OtgTimer* OtgTimer::mpOtgTimer_Instance;
UsbPowerConsumerConfigInfo* UsbPowerConsumerConfigInfo::mpUsbPowerConsumerConfigInfo_Instance=NULL;

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
     class Util											  
**************************************************************************/
Util::Util()
{
	ab_wait_bus_acquisition_tmr = 0;

	active_controller_session_flag          = false;
	bus_acquisition_pending_flag            = false;
	bus_idle_wait_reqd_flag                 = false;
	bus_idle_or_bus_acquisition_wait_reqd_flag = false;
	a_host_resumption_flag                  = false;
	b_peripheral_resumption_flag            = false;
	remote_wakeup_capability_enabled_flag   = false;
	usb_power_consumer_interested_flag      = false;
	resistive_id_flag                       = false;
	need_to_enable_cr_int_det_flag          = false;

	current_operational_mode = PM_APP_OTG_OPERATIONAL_MODE__NONE;

	pending_bus_acquisition_tmo_secs_tmr = 0;
	usb_speed = PM_OTG_USB_SPEED__INVALID; 
	active_ctrlr_info_ptr = NULL;

	//carkit
	current_carkit_mode = PM_APP_OTG_CARKIT_MODE__INVALID; // current carkit operational mode
	current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__INVALID;// current carkit mode requested by the controller
	cr_dp_con_tmr    = 300;         // 300 ms max value for Tcr_dp_con
	cr_disc_det_tmr  = 150;         // 150 ms max value for Tcr_disc_det
	cr_client_aud_bias_tmr = 10;    // 10 ms max value for Tph_aud_bias
	cr_client_mono_ack_tmr = 10;    // 10 ms max value for Tph_mono_ack

	mpEvtUtil = EvtUtil::GetInstance();
}


Util::~Util()
{
}



Util* Util::GetInstance()
{
	if(NULL == mpUtil_Instance)
	{
		mpUtil_Instance = new Util;
	}
	return mpUtil_Instance;
}




void Util::DestroyInstance()
{
	delete mpUtil_Instance;
	mpUtil_Instance = NULL;
}



boolean Util::UsbVbusControlRequestHdlr(boolean need_control)
{
	static boolean current_usb_vbus_control_need = FALSE;
	boolean status = TRUE;    // --- SUCCESS

	if (need_control != current_usb_vbus_control_need)
	{
		if (need_control)
		{
#ifdef FEATURE_PMIC_VREG_5V_UNAVAILABLE
			// can't switch OTG xcvr power source to VREG_5V
			status = FALSE;            // --- FAILURE
#else // VREG_5V is available
			// first vote to turn on VREG_5V         
			pm_vote_vreg_switch(
				PM_ON_CMD,
				PM_VREG_BOOST_ID,
				PM_VOTE_VREG_BOOST_APP__OTG);

			// and then switch to it
            #ifdef FEATURE_PMIC_KIP_HAN //KIP/HAN: select VREG_5V as the power source for the xcvr
               pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VREG5V);
            #else // PMIC3: select VREG_5V as the power source for the xcvr
               (void) pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST);
            #endif
#endif //FEATURE_PMIC_VREG_5V_UNAVAILABLE
		}
		else  // client doesn't need control
		{
			// switch to USB_VBUS for powering OTG xcvr
            #ifdef FEATURE_PMIC_KIP_HAN //KIP/HAN : select USB_VBUS as the power source for the xcvr
                pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VBUS);
             #else  // PMIC3: select USB_VBUS as the power source for the xcvr
                (void) pm_otg_pwr_sel(PM_OTG_PWR_SEL__VBUS);
             #endif            
             
			// and vote to turn off VREG_5V
			pm_vote_vreg_switch(
				PM_OFF_CMD,
				PM_VREG_BOOST_ID,
				PM_VOTE_VREG_BOOST_APP__OTG);
		}

		if (status) //lint !e774 (Boolean within 'if' always evaluates to True)
			// Reason: What if FEATURE_PMIC_VREG_5V_UNAVAILABLE is defined?
		{
			// SUCCESS
			current_usb_vbus_control_need = need_control;
		}
	}

	return status;
}




pm_otg_usb_speed_type Util::DetermineRemotePeripheralSpeed(void)
{
	boolean  dp_hi = FALSE;
	boolean  dm_hi = FALSE;
	pm_otg_usb_speed_type  remote_peripheral_speed = PM_OTG_USB_SPEED__INVALID;

	if ( (PM_ERR_FLAG__SUCCESS == pm_get_rt_status(PM_USB_D_PLUS_HI_RT_ST, &dp_hi))
		&& (PM_ERR_FLAG__SUCCESS == pm_get_rt_status(PM_USB_D_MINUS_HI_RT_ST, &dm_hi)) )
	{
		if (dp_hi && !dm_hi)
		{
			remote_peripheral_speed = PM_OTG_USB_SPEED__FULL;
		}
		else if (!dp_hi && dm_hi)
		{
			remote_peripheral_speed = PM_OTG_USB_SPEED__LOW;
		}
	}

	return remote_peripheral_speed;
}




void Util::ProcessPendingBusAcquisitionRequest(void)
{
	boolean  bus_acquisition_events_generated = FALSE;
	StateManager* pStateManager = StateManager::GetInstance();

	INTLOCK();
	if (bus_acquisition_pending_flag)
	{
		if ( pending_bus_acquisition_tmo_secs_tmr < 0 )
		{
			// try bus acqusition forever by setting timeout duration to the max 32 bit signed +ve
			ab_wait_bus_acquisition_tmr = (int32) ( (~((uint32)0)) / 2 );
		}
		else
		{
			ab_wait_bus_acquisition_tmr = pending_bus_acquisition_tmo_secs_tmr * 1000; //record in milli-secs
		}

		// set A-dev statemachine events
		mpEvtUtil->a_bus_drop_evt->SetLow();
		mpEvtUtil->a_bus_req_evt->SetHigh();

		// set B-dev statemachine events
		mpEvtUtil->b_bus_req_evt->SetHigh();

		bus_acquisition_events_generated = TRUE;
		bus_acquisition_pending_flag = FALSE;
	}
	INTFREE();

	if (bus_acquisition_events_generated)
	{
		pStateManager->ProcessCachedEvt();
	}
}




void Util::AssertSleep(boolean assert_sleep)
{
	if (NULL != ctrlr_info_curr.sleep_assert_fn)
	{
		ctrlr_info_curr.sleep_assert_fn(assert_sleep);
	}
}




boolean Util::IsBusIdle(void)		//Is anything connected to PMIC port?
{
	return ( mpEvtUtil->b_sess_vld_evt->IsLow() // no mini-B with externally powered VBUS
		&& mpEvtUtil->id_evt->IsHigh() );		// no mini-A/resistive plug
}




boolean Util::IsBusAcquired(void)	//Did OTG controller acquire the bus?
{
	return ( mpEvtUtil->a_bus_req_evt->IsHigh() && mpEvtUtil->b_bus_req_evt->IsHigh() );
}





/**************************************************************************
     class EvtUtil											  
**************************************************************************/
EvtUtil::EvtUtil()
{
	//create all events in advance 

	//OTG A-device event flags
	a_bus_drop_evt = new Evt;
	a_bus_req_evt = new Evt;
	a_bus_reset_evt = new Evt;
	a_bus_K_SE0_evt = new Evt;
	a_bus_resume_evt = new Evt;
	a_bus_suspend_evt = new Evt;
	a_sess_vld_evt = new Evt;
	a_suspend_req_evt = new Evt;
	a_srp_det_evt = new Evt;
	a_vbus_vld_evt = new Evt;
	a_wait_vrise_tmout_evt = new Evt;
	a_wait_bcon_tmout_evt = new Evt;

	// OTG B-device event flags
	b_bus_req_evt = new Evt;
	b_bus_resume_evt = new Evt;
	b_conn_evt = new Evt;
	b_sess_vld_evt = new Evt;
	b_suspend_inter_a_dev_disconnect_delay_tmout_evt = new Evt;

	//OTG A/B-device common event flags
	sm_init_evt = new Evt;
	custom_hw_ready_evt = new Evt;
	sm_reset_evt = new Evt;
	id_evt = new Evt;
	dp_evt = new Evt;
	dm_evt = new Evt;
	remote_wakeup_evt = new Evt;

	//Carkit client event flags
	cr_dp_con_det_evt = new Evt;
	cr_dp_con_tmout_evt = new Evt;
	cr_client_aud_bias_tmout_evt = new Evt;
	cr_client_mono_ack_tmout_evt = new Evt;
	cr_int_det_evt = new Evt;
	cr_client_mode_entry_req_evt = new Evt;

	//Other
	dm_stabilization_tmout_evt = new Evt;
	dp_low_detection_warmup_tmout_evt = new Evt;
	b_wait_vfall_tmout_evt = new Evt;
}




EvtUtil::~EvtUtil()
{
	//OTG A-device event flags
	delete a_bus_drop_evt;
	delete a_bus_req_evt;
	delete a_bus_reset_evt;
	delete a_bus_K_SE0_evt;
	delete a_bus_resume_evt;
	delete a_bus_suspend_evt;
	delete a_sess_vld_evt;
	delete a_suspend_req_evt;
	delete a_srp_det_evt;
	delete a_vbus_vld_evt;
	delete a_wait_vrise_tmout_evt;
	delete a_wait_bcon_tmout_evt;


	// OTG B-device event flags
	delete b_bus_req_evt;
	delete b_bus_resume_evt;
	delete b_conn_evt;
	delete b_sess_vld_evt;
	delete b_suspend_inter_a_dev_disconnect_delay_tmout_evt;

	//OTG A/B-device common event flags
	delete sm_init_evt;
	delete custom_hw_ready_evt;
	delete sm_reset_evt;
	delete id_evt;
	delete dp_evt;
	delete dm_evt;
	delete remote_wakeup_evt;

	//Carkit client event flags
	delete cr_dp_con_det_evt;
	delete cr_dp_con_tmout_evt;
	delete cr_client_aud_bias_tmout_evt;
	delete cr_client_mono_ack_tmout_evt;
	delete cr_int_det_evt;
	delete cr_client_mode_entry_req_evt;

	//Other
	delete dm_stabilization_tmout_evt;
	delete dp_low_detection_warmup_tmout_evt;
	delete b_wait_vfall_tmout_evt;
}


EvtUtil* EvtUtil::GetInstance()
{
	if(NULL == mpEvtUtil_Instance)
	{
		mpEvtUtil_Instance = new EvtUtil;
	}
	return mpEvtUtil_Instance;
}


void EvtUtil::DestroyInstance()
{
	delete mpEvtUtil_Instance;
	mpEvtUtil_Instance = NULL;
}





/**************************************************************************
     class PowerConsumer											  
**************************************************************************/
PowerConsumer::PowerConsumer()
{
	mpUsbPowerConsumerConfigInfo = UsbPowerConsumerConfigInfo::GetInstance();
	mpUtil = Util::GetInstance();
}


PowerConsumer::~PowerConsumer()
{
	UsbPowerConsumerConfigInfo::DestroyInstance();
}


PowerConsumer* PowerConsumer::GetInstance()
{
	if(NULL == mpPowerConsumer_Instance)
	{
		mpPowerConsumer_Instance = new PowerConsumer;
	}
	return mpPowerConsumer_Instance;
}

void PowerConsumer::DestroyInstance()
{
	delete mpPowerConsumer_Instance;
	mpPowerConsumer_Instance = NULL;
}



void PowerConsumer::UsbPowerConsumerEvtHdlr(const boolean                           *local_power_consumer_interest_ptr,\
											const pm_app_otg_remote_a_dev_info_type *remote_a_dev_info_ptr)
{
	static pm_app_otg_remote_a_dev_info_type  remote_a_dev_info = {
			PM_APP_OTG_A_DEV_TYPE__INVALID,   // remote_a_dev
			0                                 // max_i_ma
	};

	static boolean active_usb_power_consumer_session  = FALSE;
	static boolean usb_power_sink_open                = FALSE;
	static uint32  max_i_ma_in_use                    = 0;

	// change its previous value
	if ( local_power_consumer_interest_ptr )
	{
		// usb_power_consumer_interested_flag is exported outside of this function at
		// global scope for use by other fns elsewhere in this compilation unit.
		mpUtil->usb_power_consumer_interested_flag = *local_power_consumer_interest_ptr;
	}
	if ( remote_a_dev_info_ptr )
	{
		remote_a_dev_info = *remote_a_dev_info_ptr;
	}

	// CONNECT notification
	if ( (remote_a_dev_info.remote_a_dev < PM_APP_OTG_A_DEV_TYPE__INVALID)  // if valid A-dev,
		&& !active_usb_power_consumer_session                              // AND not already notified,
		&& mpUsbPowerConsumerConfigInfo->source_connected_notification_cb)  // AND a valid callback available
	{
		active_usb_power_consumer_session = TRUE;
		mpUsbPowerConsumerConfigInfo->source_connected_notification_cb(remote_a_dev_info.remote_a_dev);
	}

	// OPEN SINK notification
	if ( (remote_a_dev_info.remote_a_dev < PM_APP_OTG_A_DEV_TYPE__INVALID)  // if valid A-dev,
		&& (remote_a_dev_info.max_i_ma > 0)                                // AND provides usable current,
		&& (remote_a_dev_info.max_i_ma != max_i_ma_in_use)                 // AND different from the one currently being used,
		&& mpUtil->usb_power_consumer_interested_flag                  // AND client (still) interested in drawing current,
		&& mpUsbPowerConsumerConfigInfo->sink_open_notification_cb)         // AND a valid callback available
	{

       	
    	if ( (remote_a_dev_info.hostSuspended == true)   //USB Host Suspend State Notification
            &&(remote_a_dev_info.max_i_ma == mpUsbPowerConsumerConfigInfo->pm_app_otg_max_i_ma__upon_bus_suspend)  // AND provides usable current,
    		&& mpUsbPowerConsumerConfigInfo->source_suspended_notification_cb)		// AND a valid callback available
    	{
    		mpUsbPowerConsumerConfigInfo->source_suspended_notification_cb(remote_a_dev_info.hostSuspended);
            max_i_ma_in_use = remote_a_dev_info.max_i_ma;
    	}
        else
        {
           max_i_ma_in_use = remote_a_dev_info.max_i_ma;
           usb_power_sink_open = TRUE;
           mpUsbPowerConsumerConfigInfo->sink_open_notification_cb(remote_a_dev_info.max_i_ma);
        }

	}

	// CLOSE SINK notification                                                   
	if ( usb_power_sink_open                                                     // if sink already open, 
		&& ( (remote_a_dev_info.remote_a_dev >= PM_APP_OTG_A_DEV_TYPE__INVALID) // AND if invalid A-dev, 
		|| (0 == remote_a_dev_info.max_i_ma)                               //     OR zero current limit
		|| !mpUtil->usb_power_consumer_interested_flag )					//     OR client no longer interested
		&& mpUsbPowerConsumerConfigInfo->sink_close_notification_cb )			// AND a valid callback available
	{
		max_i_ma_in_use = 0;
		usb_power_sink_open = FALSE;
		mpUsbPowerConsumerConfigInfo->sink_close_notification_cb();
	}


	// DISCONNECT notification                                                   
	if ( active_usb_power_consumer_session                                       // if notified already about connection, 
		&& (remote_a_dev_info.remote_a_dev >= PM_APP_OTG_A_DEV_TYPE__INVALID)   // AND invalid A-dev,
		&& mpUsbPowerConsumerConfigInfo->source_disconnected_notification_cb )    // AND a valid callback available
	{                                                                            
		active_usb_power_consumer_session = FALSE;
		mpUsbPowerConsumerConfigInfo->source_disconnected_notification_cb();
	}

}





/**************************************************************************
     class EvtGen											  
**************************************************************************/
EvtGen::EvtGen()
{
//	mListenerVector.clear();
}

EvtGen::~EvtGen()
{
}


EvtGen* EvtGen::GetInstance()
{
	if(NULL == mpEvtGen_Instance )
	{
		mpEvtGen_Instance = new EvtGen;
	}
	return mpEvtGen_Instance;
}


void EvtGen::DestroyInstance()
{
	delete mpEvtGen_Instance;
	mpEvtGen_Instance = NULL;
}



void EvtGen::AddListener(EvtListener* )
{
//	mListenerVector.push_back(listener_ptr);

}



void EvtGen::RemoveListener(EvtListener*)
{
	//search the listener from the vector list and remove it ..... TBD
}



void EvtGen::Notify()
{
//	for(unsigned short i=0; i<mListenerVector.size(); i++)
//	{
//		mListenerVector[i]->ProcessCachedEvt();
//	}
}


void EvtGen::NotifyStateChage()
{
//	for(unsigned short i=0; i<mListenerVector.size(); i++)
//	{
//		mListenerVector[i]->ProcessStateChangeEvt();
//	}
}




/**************************************************************************
     class Evt											  
**************************************************************************/
Evt::Evt():mInfoAvailable(false),mHappened(false)
{
	mpEvtGen=EvtGen::GetInstance();
}


Evt::~Evt()
{
}



/**************************************************************************
     class UsbBusListener											  
**************************************************************************/
UsbBusListener::UsbBusListener()
{
	mpEvtUtil = EvtUtil::GetInstance(); 
	mpUtil = Util::GetInstance();
}

UsbBusListener::~UsbBusListener()
{
}

UsbBusListener* UsbBusListener::GetInstance()
{
	if(NULL == mpUsbBusListener_Instance)
	{
		mpUsbBusListener_Instance = new UsbBusListener;
	}
	return mpUsbBusListener_Instance;
}

void UsbBusListener::DestroyInstance()
{
	delete mpUsbBusListener_Instance;
	mpUsbBusListener_Instance = NULL;
}



void UsbBusListener::VbusListener(OtgVbusSmInfoType info, void* pUsbBusListenerObj)
{
	StateManager* pStateManager = StateManager::GetInstance();
	UsbBusListener* pUsbBusListener = static_cast< UsbBusListener* >((void*)pUsbBusListenerObj);
	ASSERT( pUsbBusListener != NULL );

	EvtUtil* pEvtUtil = pUsbBusListener->mpEvtUtil;


	switch (info)
	{
	case OTG_VBUS_SM_INFO__VBUS_VALID:
		pEvtUtil->a_vbus_vld_evt->SetHigh();
		break;

	case OTG_VBUS_SM_INFO__VBUS_INVALID:
		pEvtUtil->a_vbus_vld_evt->SetLow();
		break;

	case OTG_VBUS_SM_INFO__VBUS_SESSION_VALID:
		pEvtUtil->a_sess_vld_evt->SetHigh();
		pEvtUtil->b_sess_vld_evt->SetHigh();
		break;

	case OTG_VBUS_SM_INFO__VBUS_SESSION_INVALID:
		pEvtUtil->a_sess_vld_evt->SetLow();
		pEvtUtil->b_sess_vld_evt->SetLow();
		break;
	};

	pStateManager->ProcessCachedEvt(); 

}




void UsbBusListener::IdListener(OtgIdSmInfoType info, void* pUsbIdListenerObj)
{
	StateManager* pStateManager = StateManager::GetInstance();
	UsbBusListener* pUsbIdListener = static_cast< UsbBusListener* >((void*)pUsbIdListenerObj);
	ASSERT( pUsbIdListener != NULL );

	EvtUtil* pEvtUtil = pUsbIdListener->mpEvtUtil;
	Util* mpUtil = pUsbIdListener->mpUtil;

	pEvtUtil->id_evt->Reset();
	mpUtil->resistive_id_flag = FALSE;

	switch (info)
	{
	case OTG_ID_SM_INFO__FLT:
		pEvtUtil->id_evt->SetHigh();
		break;

	case OTG_ID_SM_INFO__GND:
		pEvtUtil->id_evt->SetLow();
		break;

	case OTG_ID_SM_INFO__RES:
		mpUtil->resistive_id_flag = TRUE;
		break;

	default:
		break;
	}

	// call OTG app SM to process the new event
	pStateManager->ProcessCachedEvt();  

}




void UsbBusListener::DpListener(OtgDpSmInfoType info, void* pUsbDpListenerObj)
{
	StateManager* pStateManager = StateManager::GetInstance();
	UsbBusListener* pUsbDpListener = static_cast< UsbBusListener* >((void*)pUsbDpListenerObj);
	ASSERT( pUsbDpListener != NULL );
	EvtUtil* pEvtUtil = pUsbDpListener->mpEvtUtil;

	switch (info)
	{
	case OTG_DP_SM_INFO__LOW:
		pEvtUtil->dp_evt->SetLow();
		break;
	case OTG_DP_SM_INFO__HIGH:
		pEvtUtil->dp_evt->SetHigh();
		break;
	default:
		break;
	}
	// call OTG app SM to process the new event
	pStateManager->ProcessCachedEvt();
}




void UsbBusListener::DmListener(OtgDmSmInfoType info, void* pUsbDmListenerObj)
{
	StateManager* pStateManager = StateManager::GetInstance();
	UsbBusListener* pUsbDmListener = static_cast< UsbBusListener* >((void*)pUsbDmListenerObj);
	ASSERT( pUsbDmListener != NULL );
	EvtUtil* pEvtUtil = pUsbDmListener->mpEvtUtil;

	switch (info)
	{
	case OTG_DM_SM_INFO__LOW:
		pEvtUtil->dm_evt->SetLow();
		break;
	case OTG_DM_SM_INFO__HIGH:
		pEvtUtil->dm_evt->SetHigh();
		break;
	default:
		break;
	}
	// call OTG app SM to process the new event
	pStateManager->ProcessCachedEvt();

}






/**************************************************************************
     class GenericEvtDetectIsr											  
**************************************************************************/
GenericEvtDetectIsr::GenericEvtDetectIsr()
{
	mGenericEvtDetectionIsr_enabled_flag = false;
	mGeneric_evt_detection_irq = PM_NUM_OF_IRQ_HDL;
	mpGeneric_evt = NULL;

	mpEvtUtil = EvtUtil::GetInstance();
}

GenericEvtDetectIsr::~GenericEvtDetectIsr()
{
}


GenericEvtDetectIsr* GenericEvtDetectIsr::GetInstance()
{
	if(NULL == mpGenericEvtDetectIsr_Instance )
	{
		mpGenericEvtDetectIsr_Instance = new GenericEvtDetectIsr;
	}
	return mpGenericEvtDetectIsr_Instance;
}

void GenericEvtDetectIsr::DestroyInstance()
{
	delete mpGenericEvtDetectIsr_Instance;
	mpGenericEvtDetectIsr_Instance = NULL;
}




void GenericEvtDetectIsr::GenericEvtDetectionIsrCb(void* pIsrObj)
{
	StateManager* pStateManager = StateManager::GetInstance();
	GenericEvtDetectIsr* pGenericEvtDetectIsr = static_cast< GenericEvtDetectIsr* >((void*)pIsrObj);
	ASSERT( pGenericEvtDetectIsr != NULL );

	pGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();
	pGenericEvtDetectIsr->mpGeneric_evt->SetHigh();

	pStateManager->ProcessCachedEvt();
}




void GenericEvtDetectIsr::GenericEvtDetectionIsr_Enable(pm_irq_hdl_type irq, Evt* evt_ptr)
{
	if (!mGenericEvtDetectionIsr_enabled_flag)
	{
		// Enable ISR only when it is not already active
		mGeneric_evt_detection_irq = irq;
		mpGeneric_evt = evt_ptr;
		evt_ptr->Reset();
		pm_clear_irq(irq);

		INTLOCK();
        (void)pm_set_irq_multi_handle(irq, GenericEvtDetectionIsrCb, this); 
		mGenericEvtDetectionIsr_enabled_flag = TRUE;
		INTFREE();
	}
	else
	{
		ERR_FATAL("GenericEvtDetectionIsr_Enable(irq=%d, evt_ptr=0x%x) failed!", irq, (int)evt_ptr, 0);
	}
}




void GenericEvtDetectIsr::GenericEvtDetectionIsr_Disable(void)
{
	if (mGenericEvtDetectionIsr_enabled_flag)
	{
		// Disable ISR only when it is already active
		// Turn OFF IRQ
		(void)pm_clear_irq_multi_handle(mGeneric_evt_detection_irq, GenericEvtDetectionIsrCb);
		// and clear IRQ, just in case ...
		(void) pm_clear_irq(mGeneric_evt_detection_irq);

		mGenericEvtDetectionIsr_enabled_flag = FALSE;
	}
}



/**************************************************************************
     class OtgTimer											  
**************************************************************************/
OtgTimer::OtgTimer()
{
	mpEvtUtil = EvtUtil::GetInstance();
	mpCurrent_tmo_evt = NULL;
	mpUtil = Util::GetInstance();
    mDelay = 0;
}


OtgTimer::~OtgTimer()
{
}


OtgTimer* OtgTimer::GetInstance()
{
	if(NULL == mpOtgTimer_Instance )
	{
		mpOtgTimer_Instance = new OtgTimer;
	}
	return mpOtgTimer_Instance;
}


void OtgTimer::DestroyInstance()
{
	delete mpOtgTimer_Instance;
	mpOtgTimer_Instance = NULL;
}


void OtgTimer::InitTimer()
{
    timer_def2(&mPmOtgTimer, &pm_app_otg_timer_group );
}


void OtgTimer::StartTimer(int32 tmo_delay, Evt *tmo_evt_ptr)
{
	if (NULL != tmo_evt_ptr)
	{
		tmo_evt_ptr->Reset();	//Init to indicate that timeout hasn't happened yet

		//Start the timer only if it was not currently running
		if (NULL == mpCurrent_tmo_evt)
		{
			mpCurrent_tmo_evt = tmo_evt_ptr;
            mDelay = tmo_delay;  //update the delay value

            #ifndef T_WINNT
      			// start the timer
      			timer_reg(&mPmOtgTimer,			/*!< Timer to set */
      				(timer_t2_cb_type)TimerCb,	/*!< Function to call at timer expiry */
      				(timer_cb_data_type)this,	/*!< Arbitrary data for func(time_ms, data). */
      				tmo_delay,					/*!< Time (in ms) until first timer expiry */
      				0);							/*!< Period (in ms) between repeated expiries (0 = not periodic) */
            #endif
		}
		else
		{
			ERR_FATAL("pm_app_otg_start_timer(delay=%d, tmo_evt_ptr=0x%x) failed!", tmo_delay, (int)tmo_evt_ptr, 0);
		}
	}
}




void OtgTimer::StopTimer(const Evt *tmo_evt_ptr)
{
	// stop only if it is the current timer that was started in the first place!
	if ( (NULL != tmo_evt_ptr)
		&& (tmo_evt_ptr == mpCurrent_tmo_evt) )
	{
		mpCurrent_tmo_evt=NULL;

        #ifndef T_WINNT
		    timer_clr( &mPmOtgTimer, T_NONE );
        #endif
	}
}




void OtgTimer::TimerCb(int32 time_ms, timer_cb_data_type token)
{
	StateManager* pStateManager = StateManager::GetInstance();
	OtgTimer* pOtgTimer = static_cast< OtgTimer* >((void*)token);
	ASSERT( pOtgTimer != NULL );

    if (time_ms == pOtgTimer->mDelay)
    {
      //delay value matches the actual setup value
    }

	if ( NULL != pOtgTimer->mpCurrent_tmo_evt )
	{
		pOtgTimer->mpCurrent_tmo_evt->SetHigh(); // indicate timeout has happened

		pOtgTimer->StopTimer(pOtgTimer->mpCurrent_tmo_evt);

		// ask the OTG state machine to process the new timeout event
		pStateManager->ProcessCachedEvt();
	}

}



/**************************************************************************
     class EvtDebugger											  
**************************************************************************/
EvtDebugger::EvtDebugger()
{
	mpEvtGen = EvtGen::GetInstance();
	mpEvtGen->AddListener(this); //register to EvtGen
}


EvtDebugger::~EvtDebugger()
{
	mpEvtGen->RemoveListener(this);
}


EvtDebugger* EvtDebugger::GetInstance()
{
	if(NULL == mpEvtDebugger_Instance){
		mpEvtDebugger_Instance = new EvtDebugger;
	}
	return mpEvtDebugger_Instance;
}


void EvtDebugger::DestroyInstance()
{
	delete mpEvtDebugger_Instance;
	mpEvtDebugger_Instance = NULL;
}



void EvtDebugger::ProcessCachedEvt(void)
{
	//PM APP OTG event happened, send this event to diag
	//TBD.....
}


void EvtDebugger::ProcessStateChangeEvt(void)
{
	//State change event happened, send this event to diag
	//..... TBD 
}


#endif //FEATURE_PMIC_USB_OTG
