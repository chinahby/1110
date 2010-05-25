/*! \file 
*  \n
*  \brief  pmapp_otg_StateCarkitMode.cpp ----- PM APP OTG Carkit related state class implementation
*  \details This file contains implementation of classes for OTG carkit states
*     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_StateCarkitMode.cpp#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/08   aab      Added support for usb host suspended notification
				    (usb_power_source_suspended_notification)
08/04/08   aab      Added FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_Interface.h"   

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
     class Carkitclient_Init_State											  
**************************************************************************/
Carkitclient_Init_State::Carkitclient_Init_State()
{
	mStateName="CARKITCLIENT_INIT"; 
	mStateId=PM_APP_OTG_STATE__CARKITCLIENT_INIT;
}


Carkitclient_Init_State::~Carkitclient_Init_State()
{
}


void Carkitclient_Init_State::Entry()
{
	pm_app_otg_remote_a_dev_info_type  carkit_info;
	PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();
	UsbPowerConsumerConfigInfo* pUsbPowerConsumerConfigInfo = UsbPowerConsumerConfigInfo::GetInstance();


	mOtgEventPayload = PM_APP_OTG_DEVICE_ATTACHED__CARKIT;
	mpOtgDiagEvent->OtgEventHandlePayload(EVENT_PM_APP_OTG_DEVICE_ATTACHED,1,(void *) &mOtgEventPayload);

	// save current operational mode
	mpUtil->current_operational_mode = PM_APP_OTG_OPERATIONAL_MODE__CARKIT;

	// reset current operational carkit mode
	mpUtil->current_carkit_mode = PM_APP_OTG_CARKIT_MODE__INVALID;

	// reset current requested carkit mode
	mpUtil->current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__INVALID;

	// reset USB carkit client controller's intent to modify interrupt detection mechanism
	mpUtil->need_to_enable_cr_int_det_flag = FALSE;

	// reset flag that would indicate if CR_DP_CON had been detected which is a
	//  key signaling step in the carkit connection sequence implemented in this
	//  state's cached_evt_hdlr()
	mpEvtUtil->cr_dp_con_det_evt->Reset();

	// ask USB carkit client controller to initialize itself
	if ( NULL != mpUtil->ctrlr_info_curr.carkitclient_mode_init_cb )
	{
		mpUtil->ctrlr_info_curr.carkitclient_mode_init_cb();

		// Remember that an active session has been started with the OTG controller
		mpUtil->active_controller_session_flag = TRUE;
	}

	// recognize remote A-dev as a USB carkit
	carkit_info.remote_a_dev = PM_APP_OTG_A_DEV_TYPE__USB_CARKIT;
	// and set info on how much current can be withdrawn from the carkit
	carkit_info.max_i_ma = pUsbPowerConsumerConfigInfo->pm_app_otg_max_i_ma__upon_carkit_connect;
	carkit_info.hostSuspended = false;
	pPmAppOtg->SetRemote_A_DevInfo(carkit_info);
}




BaseState* Carkitclient_Init_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpCarkitclient_Init_State;

	if ( mpEvtUtil->cr_client_mode_entry_req_evt->IsHigh() )
	{
		// we don't want to handle the same event multiple times
		mpEvtUtil->cr_client_mode_entry_req_evt->Reset();

		// wait for carkit to drop D- right after we pull D+ high
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
			PM_USB_D_MINUS_HI_FE_IRQ_HDL,
			mpEvtUtil->cr_dp_con_det_evt);

		// start timer Tcr_dp_con
		mpOtgTimer->StartTimer(mpUtil->cr_dp_con_tmr, mpEvtUtil->cr_dp_con_tmout_evt);

		// pull up on D+ to signal to the carkit that we are ready
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__NONE,
			PM_OTG_DP_CONFIG_CMD__PULL_UP,
			PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
			PM_OTG_ID_CONFIG_CMD__NONE );
	}
	else if ( mpEvtUtil->cr_dp_con_tmout_evt->IsHigh() )
	{
		mpEvtUtil->cr_dp_con_tmout_evt->Reset();

		// Inform the controller about the failure
		if ( NULL != mpUtil->ctrlr_info_curr.carkitclient_mode_entry_rsp_fn )
		{
			mpUtil->ctrlr_info_curr.carkitclient_mode_entry_rsp_fn(PM_APP_OTG_CARKIT_MODE__UART);
		}
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}
	else if ( mpEvtUtil->cr_dp_con_det_evt->IsHigh())
	{
		mpEvtUtil->cr_dp_con_det_evt->Reset();

		// stop timer
		mpOtgTimer->StopTimer(mpEvtUtil->cr_dp_con_tmout_evt);
		next_state_ptr = mpStateFactory->mpCarkitclient_Uart_State;
	}
	else if ( mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		// stop timer
		mpOtgTimer->StopTimer(mpEvtUtil->cr_dp_con_tmout_evt);
		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}

	return next_state_ptr;
}




boolean Carkitclient_Init_State::UsbVbusControlRequestHdlr(boolean need_control)
{
	return mpUtil->UsbVbusControlRequestHdlr(need_control);
}




boolean Carkitclient_Init_State::CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode)
{
	boolean status = FALSE;

	if ( (PM_APP_OTG_CARKIT_MODE__INVALID == mpUtil->current_carkit_req_mode)
		&& (PM_APP_OTG_CARKIT_MODE__UART == carkit_mode) )
	{
		mpUtil->current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__UART;
		mpEvtUtil->cr_client_mode_entry_req_evt->SetHigh();
		status = TRUE;
	}

	return status; 
}




/**************************************************************************
     class Carkitclient_Uart_State											  
**************************************************************************/

Carkitclient_Uart_State::Carkitclient_Uart_State()
{
	mStateName = "CARKITCLIENT_UART"; 
	mStateId = PM_APP_OTG_STATE__CARKITCLIENT_UART;
}


Carkitclient_Uart_State::~Carkitclient_Uart_State()
{
}


void Carkitclient_Uart_State::Entry()
{
	// reset current requested carkit mode
	mpUtil->current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__INVALID;

	// check if we are really entering UART mode from a non UART mode
	if (PM_APP_OTG_CARKIT_MODE__UART != mpUtil->current_carkit_mode)
	{
		// reset mode so that we don't have any interference from
		// the PMIC configuration that was in place before we entered
		// UART mode
		(void) pm_otg_reset_mode();

		// drive D- high but stop pulling D+ high
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__NONE,
			PM_OTG_DP_CONFIG_CMD__PULL_DOWN,
			PM_OTG_DM_CONFIG_CMD__PULL_UP,
			PM_OTG_ID_CONFIG_CMD__NONE );

		// reset D+ event to indicate that we have removed the pull-up on D+
		mpEvtUtil->dp_evt->Reset();

		// config the PMIC xcvr for UART mode
		(void) pm_otg_config_uart_mode();
		(void) pm_otg_set_uart_tx_modulation(PM_OTG_UART_TX_MODULATION__NONE);

		// remember current carkit mode
		mpUtil->current_carkit_mode = PM_APP_OTG_CARKIT_MODE__UART;

		// Inform the controller that UART mode entry has been completed
		if ( NULL != mpUtil->ctrlr_info_curr.carkitclient_mode_entry_rsp_fn )
		{
			mpUtil->ctrlr_info_curr.carkitclient_mode_entry_rsp_fn(PM_APP_OTG_CARKIT_MODE__UART);
		}
	}
}






BaseState* Carkitclient_Uart_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpCarkitclient_Uart_State;

	static  boolean   cr_int_det_enabled = FALSE;

	// act on carkit client controller's request to enable/disable carkit interrupt detection mechanism
	// act on carkit client controller's request to enable/disable carkit interrupt detection mechanism
	if ( mpUtil->need_to_enable_cr_int_det_flag )
	{
		if ( !cr_int_det_enabled )
		{
			// enable carkit int detection

			// Look for carkit interrupt; carkit is allowed to interrupt the phone
			//   in UART mode by driving D+ low for a time of Tcr_pls_neg.
			// PM6650 has support for carkit interrupt detection only in AUDIO mode;
			//   so we'll have to resort to using the falling edge (FE) of D+ for detecting
			//   carkit interrupt in UART mode. There is a caveat to using the D+ FE
			//   because it will be triggered even in the case when the carkit supports
			//   UART protocol because the regular UART line transitions caused by
			//   the data transfer from carkit to phone will also trigger D+ FE
			//   interrupts. So we can employ this scheme only if the carkit doesn't
			//   support UART protocol.

			// We can't use D+ debouncer here because of the carkit interrupt being
			//   a pulse as opposed to a single D+ state transition.
			mpEvtUtil->cr_int_det_evt->Reset();

			INTLOCK();
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
				PM_USB_D_PLUS_HI_FE_IRQ_HDL,
				mpEvtUtil->cr_int_det_evt );
			cr_int_det_enabled = TRUE;
			INTFREE();
		}
	}
	else
	{
		if ( cr_int_det_enabled )
		{
			// disable carkit int detection
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();
			mpEvtUtil->cr_int_det_evt->Reset();
			cr_int_det_enabled = FALSE;
		}
	}

	// ---------- continue with other event processing -------------
	if ( mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		// disable carkit interrupt detection mechanism that was in use in UART mode
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

		// CR_INT is just D+ going low and PMIC could have detected this
		//  before VBUS going low; so reset CR_INT just to be safe!
		mpEvtUtil->cr_int_det_evt->Reset();
		cr_int_det_enabled = FALSE;

		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}   
	else if ( mpEvtUtil->cr_client_mode_entry_req_evt->IsHigh() )
	{
		// controller wants us to enter AUDIO mode ...

		// act on client's request once per occurence
		mpEvtUtil->cr_client_mode_entry_req_evt->Reset();

		// disable carkit interrupt detection mechanism while we prepare for
		// entering AUDIO mode
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();


		mpEvtUtil->cr_int_det_evt->Reset();
		// don't reset cr_int_det_enabled flag to FALSE since we don't want it to automatically
		// go back to high upon entering this cached evt handler again
		cr_int_det_enabled = TRUE;  // to be extra cautious, we can actually force it to TRUE
		// and leave it that way until the time we exit this state
		//    -----  Note that we are NOT exiting this state immediately  ------

		// we have been talking to carkit over UART, so reset PMIC xcvr mode
		(void) pm_otg_reset_mode();

		// start D+ debouncer now that UART Tx/Rx has finished; this way
		//  we have avoided detection of spurious transitions that would
		//  have otherwise been caused by UART data
		mpDpStateMonitor->DpStateMonitorInit(mpUsbBusListener->DpListener, (void*)mpUsbBusListener); 

		// we need to wait for D+ to go high before we can enter AUDIO mode
	}
	else if ( mpEvtUtil->dp_evt->IsHigh() )
	{
		mpEvtUtil->dp_evt->Reset();

		mpDpStateMonitor->DpStateMonitorReset();


		if ( mpEvtUtil->cr_int_det_evt->IsHigh() )
		{
			// carkit interrupted while we were in UART mode
			mpEvtUtil->cr_int_det_evt->Reset();

			// report to the controller but continue to remain in UART state
			if ( NULL != mpUtil->ctrlr_info_curr.carkitclient_int_hdlr )
			{
				mpUtil->ctrlr_info_curr.carkitclient_int_hdlr();
			}

			// enable detection of carkit interrupt again
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
				PM_USB_D_PLUS_HI_FE_IRQ_HDL,
				mpEvtUtil->cr_int_det_evt );
		}
		else if ( (PM_APP_OTG_CARKIT_MODE__MONO == mpUtil->current_carkit_req_mode)
			|| (PM_APP_OTG_CARKIT_MODE__STEREO == mpUtil->current_carkit_req_mode) )
		{
			// we can transition to AUDIO mode and we don't need D+ debouncer any more

			// this is the time to reset cr_int_det_enabled to FALSE because
			//  we are exiting this state and we have already turned off the interrupt
			//  detection as part of processing "audio mode entry" request.
			cr_int_det_enabled = FALSE;
			next_state_ptr = mpStateFactory->mpCarkitclient_Audio_State;
		}
	}
	else if ( mpEvtUtil->cr_int_det_evt->IsHigh() )
	{
		// it is not enough to just look for D+ FE as this could have been
		// caused by the disconnection from the carkit as well
		mpEvtUtil->dp_evt->Reset();
		mpDpStateMonitor->DpStateMonitorInit(mpUsbBusListener->DpListener, (void*)mpUsbBusListener); 
		// wait for D+ to go high before acknowledging D+ FE as the carkit interrupt

	}

	return next_state_ptr;
}




boolean Carkitclient_Uart_State::UsbVbusControlRequestHdlr(boolean need_control)
{
	return mpUtil->UsbVbusControlRequestHdlr(need_control);
}




boolean Carkitclient_Uart_State::CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode)
{
	boolean status = FALSE;

	if ( (PM_APP_OTG_CARKIT_MODE__INVALID == mpUtil->current_carkit_req_mode)
		&& ( (PM_APP_OTG_CARKIT_MODE__MONO == carkit_mode)
		|| (PM_APP_OTG_CARKIT_MODE__STEREO == carkit_mode) ) )
	{
		mpUtil->current_carkit_req_mode = carkit_mode;
		mpEvtUtil->cr_client_mode_entry_req_evt->SetHigh();
		status = TRUE;
	}

	return status; 
}








/**************************************************************************
     class Carkitclient_Audio_State											  
**************************************************************************/

Carkitclient_Audio_State::Carkitclient_Audio_State()
{
	mStateName = "CARKITCLIENT_AUDIO"; 
	mStateId = PM_APP_OTG_STATE__CARKITCLIENT_AUDIO;
}


Carkitclient_Audio_State::~Carkitclient_Audio_State()
{
}


void Carkitclient_Audio_State::Entry()
{
	// check if we are really entering AUDIO mode from a non AUDIO mode
	if ( (PM_APP_OTG_CARKIT_MODE__MONO != mpUtil->current_carkit_mode)
		&& (PM_APP_OTG_CARKIT_MODE__STEREO != mpUtil->current_carkit_mode) )
	{
		// Leave D+ tri-stated for both audio modes (i.e. remove
		//  pull-ups/downs on both D+/- as they will affect audio signals)
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__NONE,
			PM_OTG_DP_CONFIG_CMD__DISABLE_BOTH,
			PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH,
			PM_OTG_ID_CONFIG_CMD__NONE );

		if (PM_APP_OTG_CARKIT_MODE__MONO == mpUtil->current_carkit_req_mode)
		{
			// For MONO mode, route just the speaker signal on D- but don't enable 
			//  the MIC input from carkit just yet.
			(void) pm_otg_config_carkit_mode(
				PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT);
			// At this point, only the speaker bias is presented to the carkit to enable
			//  it to enter its audio mode
		}
		else
		{
			// For STEREO mode, route speaker signals on both D- and D+ lines.
			(void) pm_otg_config_carkit_mode(
				PM_OTG_CARKIT_AUDIO_MODE__STEREO_OUT);
		}

		// start a timer to wait for carkit to detect our speaker output on D- line
		mpOtgTimer->StartTimer(
			mpUtil->cr_client_aud_bias_tmr,
			mpEvtUtil->cr_client_aud_bias_tmout_evt );
	}
}




BaseState* Carkitclient_Audio_State::CachedEvtHdlr()
{
	static  boolean              cr_int_det_enabled = FALSE;
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpCarkitclient_Audio_State;
	boolean                      cr_int_in_progress = FALSE;
	pm_otg_carkit_int_hyst_type  cr_int_hyst        = PM_OTG_CARKIT_INT_HYST__INVALID;

	// act on carkit client controller's request to enable/disable carkit interrupt detection mechanism
	if ( mpUtil->need_to_enable_cr_int_det_flag )
	{
		if ( !cr_int_det_enabled )
		{
			// enable carkit int detection

			// look for carkit interrupt. D+ HI interrupt not useful as its threshold
			// falls in the valid range of audio (phone's right spkr or carkit's MIC) signal on D+ line
			if ( (PM_APP_OTG_CARKIT_MODE__MONO == mpUtil->current_carkit_mode)
				|| (PM_APP_OTG_CARKIT_MODE__MONO == mpUtil->current_carkit_req_mode) )
			{
				cr_int_hyst = PM_OTG_CARKIT_INT_HYST__LOW;
			}
			else
			{
				cr_int_hyst = PM_OTG_CARKIT_INT_HYST__HIGH;
			}
			(void) pm_otg_set_carkit_interrupt_hysteresis(
				cr_int_hyst);

			mpEvtUtil->cr_int_det_evt->Reset();

			INTLOCK();
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
				PM_USB_CARKIT_INT_RE_IRQ_HDL,
				mpEvtUtil->cr_int_det_evt );
			cr_int_det_enabled = TRUE;
			INTFREE();
		}
	}
	else
	{
		if ( cr_int_det_enabled )
		{
			// disable carkit int detection
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();
			mpEvtUtil->cr_int_det_evt->Reset();
			cr_int_det_enabled = FALSE;
		}
	}

	// ---------- continue with other event processing -------------
	if ( mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		// stop timer
		mpOtgTimer->StopTimer(mpEvtUtil->cr_client_aud_bias_tmout_evt);

		// disable ISR for detecting carkit interrupt
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

		// CR_INT is just D+ going low and PMIC could have detected this
		//  before VBUS going low; so reset CR_INT just to be safe!
		mpEvtUtil->cr_int_det_evt->Reset();
		cr_int_det_enabled = FALSE;

		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}   
	else if ( mpEvtUtil->cr_client_aud_bias_tmout_evt->IsHigh() )
	{
		mpEvtUtil->cr_client_aud_bias_tmout_evt->Reset();

		// time to actually begin routing speaker/mic signals to/from the carkit
		if (PM_APP_OTG_CARKIT_MODE__MONO == mpUtil->current_carkit_req_mode)
		{
			// enable MIC path from the carkit all the way thro' AUXI_P to MSM
			(void) pm_otg_config_carkit_mode(
				PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT_MIC_IN);
		}
		// else STEREO (nothing extra needs to be done)

		// remember current carkit mode
		mpUtil->current_carkit_mode = mpUtil->current_carkit_req_mode;

		// reset current requested carkit mode
		mpUtil->current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__INVALID;      

		// Inform the controller that AUDIO mode entry has been completed
		if ( NULL != mpUtil->ctrlr_info_curr.carkitclient_mode_entry_rsp_fn )
		{
			mpUtil->ctrlr_info_curr.carkitclient_mode_entry_rsp_fn(
				mpUtil->current_carkit_mode);
		}
	}
	else if ( mpEvtUtil->cr_client_mode_entry_req_evt->IsHigh() )
	{
		// controller wants us to exit audio mode and enter UART mode
		mpEvtUtil->cr_client_mode_entry_req_evt->Reset();

		// stop timer
		mpOtgTimer->StopTimer(mpEvtUtil->cr_client_aud_bias_tmout_evt);

		// disable ISR for detecting carkit interrupt
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();
		mpEvtUtil->cr_int_det_evt->Reset();
		cr_int_det_enabled = FALSE;

		// mute MIC signal on D+ while continuing to output SPKR signal on D-
		(void) pm_otg_config_carkit_mode(
			PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT);

		next_state_ptr = mpStateFactory->mpCarkitclient_Uart_State;
	}
	else if ( mpEvtUtil->cr_int_det_evt->IsHigh() )
	{
		// it is not enough to just look for the beginning of the -ve D+ pulse
		// as this could have been caused by the disconnection from the carkit as well
		mpEvtUtil->dp_evt->Reset();
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable(); // since we are not clearing 
		// pm_app_otg_evt__cr_int_det flag
		// when processing this event, there
		// is a significant possibility that
		// an extraneous execution of this cached_evt_hdlr()
		// would cause re-enabling of the IRQ
		// resulting in an ERR_FATAL. That's why
		// we disable IRQ before re(enabling) it.
		mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Enable(
			PM_USB_CARKIT_INT_FE_IRQ_HDL,
			mpEvtUtil->dp_evt);
		// since the carkit -ve interrupt pulse width < 1us, it is quite possible
		// that we would have missed the beginning of the +ve pulse, so do
		// a real-time check as well to be robust enough in detecting
		// the carkit pulse correctly
		cr_int_in_progress = TRUE;
		(void) pm_get_rt_status(
			PM_USB_CARKIT_INT_RT_ST,
			&cr_int_in_progress);
		if (!cr_int_in_progress)
		{
			// carkit -ve interrupt pulse has completed!
			mpGenericEvtDetectIsr->GenericEvtDetectionIsr_Disable();

			mpEvtUtil->cr_int_det_evt->Reset();
			cr_int_det_enabled = FALSE;

			next_state_ptr = mpStateFactory->mpCarkitclient_Carkit_Irq_State;
		}
		else
		{
			// wait for D+ to go high above carkit interrupt threshold
			// before acknowledging falling D+ as carkit interrupt
		}
	}
	else if ( mpEvtUtil->dp_evt->IsHigh() )
	{
		mpEvtUtil->dp_evt->Reset();

		if ( mpEvtUtil->cr_int_det_evt->IsHigh() )
		{
			mpEvtUtil->cr_int_det_evt->Reset();
			cr_int_det_enabled = FALSE;

			next_state_ptr = mpStateFactory->mpCarkitclient_Carkit_Irq_State;
		}
	}

	return next_state_ptr;
}




boolean Carkitclient_Audio_State::UsbVbusControlRequestHdlr(boolean need_control)
{
	return mpUtil->UsbVbusControlRequestHdlr(need_control);
}




boolean Carkitclient_Audio_State::CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode)
{
	boolean status = FALSE;

	if ( (PM_APP_OTG_CARKIT_MODE__INVALID == mpUtil->current_carkit_req_mode)
		&& (PM_APP_OTG_CARKIT_MODE__UART == carkit_mode) )
	{
		mpUtil->current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__UART;
		mpEvtUtil->cr_client_mode_entry_req_evt->SetHigh();
		status = TRUE;
	}

	return status;  //base class returns false; to prevent inhereted class from executing it ?

}





/**************************************************************************
     class Carkitclient_Carkit_Irq_State											  
**************************************************************************/
Carkitclient_Carkit_Irq_State::Carkitclient_Carkit_Irq_State()
{
	mStateName="CARKITCLIENT_CARKIT_IRQ";
	mStateId=PM_APP_OTG_STATE__CARKITCLIENT_CARKIT_IRQ;
}


Carkitclient_Carkit_Irq_State::~Carkitclient_Carkit_Irq_State()
{
}



void Carkitclient_Carkit_Irq_State::Entry()
{
	// reset current requested carkit mode
	mpUtil->current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__INVALID;

	// let controller know that carkit has interrupted; let controller
	// decide whether to honor or not within Tph_mono_ack
	mpOtgTimer->StartTimer(
		mpUtil->cr_client_mono_ack_tmr,
		mpEvtUtil->cr_client_mono_ack_tmout_evt );

	if ( NULL != mpUtil->ctrlr_info_curr.carkitclient_int_hdlr )
	{
		mpUtil->ctrlr_info_curr.carkitclient_int_hdlr();
	}
}




BaseState* Carkitclient_Carkit_Irq_State::CachedEvtHdlr()
{
	mpStateFactory = StateFactory::GetInstance();
	BaseState* next_state_ptr = mpStateFactory->mpCarkitclient_Carkit_Irq_State;

	if ( mpEvtUtil->b_sess_vld_evt->IsLow()
		|| mpEvtUtil->b_bus_req_evt->IsLow() )
	{
		// stop timer
		mpOtgTimer->StopTimer(mpEvtUtil->cr_client_mono_ack_tmout_evt );

		next_state_ptr = mpStateFactory->mpB_Idle_State;
	}
	else if ( mpEvtUtil->cr_client_mode_entry_req_evt->IsHigh() )
	{
		// we don't want to handle this more than once
		mpEvtUtil->cr_client_mode_entry_req_evt->Reset();

		// controller wants to honor carkit's interrupt

		// stop timer
		mpOtgTimer->StopTimer(mpEvtUtil->cr_client_mono_ack_tmout_evt);

		next_state_ptr = mpStateFactory->mpCarkitclient_Uart_State;
	}
	else if ( mpEvtUtil->cr_client_mono_ack_tmout_evt->IsHigh() )
	{
		mpEvtUtil->cr_client_mono_ack_tmout_evt->Reset();

		// controller didn't want to honor carkit's interrupt, so remain in AUDIO mode
		next_state_ptr = mpStateFactory->mpCarkitclient_Audio_State;
	}
	return next_state_ptr;
}




boolean Carkitclient_Carkit_Irq_State::UsbVbusControlRequestHdlr(boolean need_control)
{
	return mpUtil->UsbVbusControlRequestHdlr(need_control);
}





boolean Carkitclient_Carkit_Irq_State::CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode)
{
	boolean status = FALSE;

	if ( (PM_APP_OTG_CARKIT_MODE__INVALID == mpUtil->current_carkit_req_mode)
		&& (PM_APP_OTG_CARKIT_MODE__UART == carkit_mode) )
	{
		mpUtil->current_carkit_req_mode = PM_APP_OTG_CARKIT_MODE__UART;
		mpEvtUtil->cr_client_mode_entry_req_evt->SetHigh();
		status = TRUE;
	}

	return status;
}

#endif  //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
#endif //FEATURE_PMIC_USB_OTG

