/*! \file 
*  \n
*  \brief  pmapp_otg.cpp  ---- PM APP OTG application APIs implementation
*  \details This file contains the implementation of the USB-OTG application developed
*  for the Qualcomm Power Manager Chip Set.
*  \n
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg.cpp#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/08   aab      Added support for usb host suspended notification
					(usb_power_source_suspended_notification)
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_interface.h"   /*!< Actual implementation of pmapp_otg APIs */

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

void pm_app_otg_init(
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

	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->Init( init_done_cb,
		host_mode_init_cb,
		peripheral_mode_init_cb,
		rs232dongle_mode_init_cb,
		carkitclient_mode_init_cb,
		carkitclient_int_hdlr,
		carkitclient_mode_entry_rsp_fn,
		remote_dev_lost_cb,
		remote_dev_resumed_cb,
		remote_host_wakeup_fn,
		error_notification_cb,
		sleep_assert_fn);

}




pm_app_otg_operational_mode_type pm_app_otg_get_current_operational_mode(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	return PmAppOtg_ptr->GetCurrentOperationalMode();
}



const pm_app_otg_ctrlr_info_type* pm_app_otg_get_ctrlr_info(void)
{
	Util* mpUtil = Util::GetInstance();
	return mpUtil->active_ctrlr_info_ptr;
}



void pm_app_otg_reset(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->Reset();
}



void pm_app_otg_acquire_bus(int8 tmo_secs)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->AcquireBus(tmo_secs);
}



void pm_app_otg_relinquish_bus(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->RelinquishBus();
}



uint32 pm_app_otg_get_local_vbus_current_sourcing_capability(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	return PmAppOtg_ptr->GetLocal_Vbus_CurrentSourcingCapability();
}




void pm_app_otg_set_remote_a_dev_info(pm_app_otg_remote_a_dev_info_type info)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->SetRemote_A_DevInfo(info);
}



void pm_app_otg_suspend(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->Suspend();
}



void pm_app_otg_resume(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->Resume();
}




void pm_app_otg_process_evt__remote_peripheral_disconnected(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->ProcessRemotePeripheral_DisconnectedEvt();
}



void pm_app_otg_set_remote_wakeup_capability(boolean  remote_wakeup_capability_enabled)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->SetRemote_WakeupCapability(remote_wakeup_capability_enabled);
}


void pm_app_otg_prepare_for_remote_host_wakeup_signaling(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->PrepareFor_RemoteHostWakeupSignaling();
}



void pm_app_otg_process_evt__remote_host_wakeup_signaling_done(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->ProcessRemoteHost_WakeupSignalingDoneEvt();
}


void pm_app_otg_configure_usb_power_consumer(
	pm_app_otg_usb_power_source_connected_notification_cb_type    source_connected_cb,
	pm_app_otg_usb_power_source_disconnected_notification_cb_type source_disconnected_cb,
	pm_app_otg_usb_power_sink_open_notification_cb_type           sink_open_notification_cb,
	pm_app_otg_usb_power_sink_close_notification_cb_type          sink_close_notification_cb,
	pm_app_otg_usb_power_source_suspended_notification_cb_type	 source_suspended_cb )
{
	PmAppOtg_PowConsumer* PmAppOtg_PowConsumer_ptr = PmAppOtg_PowConsumer::GetInstance();

	PmAppOtg_PowConsumer_ptr->ConfigureUsbPowerConsumer( source_connected_cb,\
		source_disconnected_cb,\
		sink_open_notification_cb,\
		sink_close_notification_cb,\
		source_suspended_cb  );

}



void pm_app_otg_set_usb_power_consumption_requirement(boolean need_usb_power)
{
	PmAppOtg_PowConsumer* PmAppOtg_PowConsumer_ptr = PmAppOtg_PowConsumer::GetInstance();
	PmAppOtg_PowConsumer_ptr->SetUsbPowerConsumptionRequirement(need_usb_power);
}



boolean pm_app_otg_process_usb_power_line_control_request(boolean need_control)
{
	PmAppOtg_PowConsumer* PmAppOtg_PowConsumer_ptr = PmAppOtg_PowConsumer::GetInstance();
	return PmAppOtg_PowConsumer_ptr->ProcessUsbPowerLineControlRequest(need_control);
}



boolean pm_app_otg_enter_carkit_mode(pm_app_otg_carkit_mode_type carkit_mode)
{
	PmAppOtg_Carkit* PmAppOtg_Carkit_ptr = PmAppOtg_Carkit::GetInstance();
	return PmAppOtg_Carkit_ptr->EnterCarkitMode(carkit_mode);
}



void pm_app_otg_enable_carkit_interrupt_detection(void)
{
	PmAppOtg_Carkit* PmAppOtg_Carkit_ptr = PmAppOtg_Carkit::GetInstance();
	PmAppOtg_Carkit_ptr->EnableCarkitInterruptDetection();
}



void pm_app_otg_disable_carkit_interrupt_detection(void)
{
	PmAppOtg_Carkit* PmAppOtg_Carkit_ptr = PmAppOtg_Carkit::GetInstance();
	PmAppOtg_Carkit_ptr->DisableCarkitInterruptDetection();
}



boolean pm_app_otg_data_during_audio_supported(void)
{
	PmAppOtg_Carkit* PmAppOtg_Carkit_ptr = PmAppOtg_Carkit::GetInstance();
	return PmAppOtg_Carkit_ptr->DataDuringAudioSupported();
}




#ifdef FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE

void pm_app_otg_start_b_dev_session(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->Start_B_DevSession();
}


void pm_app_otg_stop_b_dev_session(void)
{
	PmAppOtg* PmAppOtg_ptr = PmAppOtg::GetInstance();
	PmAppOtg_ptr->Stop_B_DevSession();
}

#endif //FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE

#endif //FEATURE_PMIC_USB_OTG

