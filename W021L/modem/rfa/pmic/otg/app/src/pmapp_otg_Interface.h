#ifndef PMAPP_OTG_INTERFACE_H
#define PMAPP_OTG_INTERFACE_H

/*! \file 
 *  \brief  pmapp_otg_Interface.h ----- PM APP OTG Interface related class declaration
 *  \details This file contains class declarations for USB-OTG application developed for the Qualcomm Power
 *   Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_Interface.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/08   aab      Added support for usb host suspended notification
				    (usb_power_source_suspended_notification)
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
                            INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pmapp_otg_StateManager.h"
#ifdef FEATURE_PMIC_USB_OTG

/*===========================================================================
                              FORWARD DECLARATION
===========================================================================*/
class PmAppOtg;
class OtgDiagEvent;

/*===========================================================================
                              CLASS DEFINITION
===========================================================================*/

/**************************************************************************
     class PmAppOtg_PowConsumer											  
**************************************************************************/
/*! \class PmAppOtg_PowConsumer 
 *  \brief This class has power consumer related routines that are needed by charger software layer
 *  \details  ConfigureUsbPowerConsumer() is used as an API for charger software layer to register as
 *				a power consuming device.  APP OTG driver uses the registered callback functions to
 *				allow USB charging when ever a valid USB Host device or USB wall charger is connected.
 *  \note none
 */
class PmAppOtg_PowConsumer
{
public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	PmAppOtg_PowConsumer();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~PmAppOtg_PowConsumer();

	/*! \brief Utility function used to create a singlton instance of PmAppOtg_PowConsumer object
	*  \details none
	*  \param none
	*  \return singlton instance of PmAppOtg_PowConsumer object
	*  \note none
	*/
	static PmAppOtg_PowConsumer* GetInstance();

	/*! \brief Utility function used to initiate destruction of DmStateMonitor class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
    static void DestroyInstance();


	/*!
	*	\breif pm_app_otg_configure_usb_power_consumer()
	*	\details This function initializes and sets up a USB power consumer which can
	*	   utilize the power available from the Vbus supplied by the remote A-device.
	*	   An example of a local power consumer would be a "USB charger" application.
	*	
	*	\param source_connected_cb  
	*	 - Callback function that lets the USB power consumer know when the USB power
	*	    source has connected. This by itself doesn't indicate if power
	*	    can be drawn from the Vbus. Wait for 'sink_open_notification_cb'
	*	     to know when power can be safely drawn from the Vbus line.
	*	 - Type: pm_app_otg_usb_power_source_connected_notification_cb_type
	*	 - Valid inputs: Valid non-NULL callback fn
	*   
	*	\param source_disconnected_cb
	*	 - Callback function that lets the USB power consumer know when the USB power
	*	         source has disconnected. At this point, the source is no longer
	*	         available and so power can't be drawn. Do not rely solely on this
	*	         callback to know when to stop drawing power from the Vbus though;
	*	         use 'sink_close_notification_cb' for that purpose.        
	*	 - Type: pm_app_otg_usb_power_source_disconnected_notification_cb_type
	*	 - Valid inputs: Valid non-NULL callback fn
	*	    
	*	\param sink_open_notification_cb
	*	 - Callback function that lets the USB power consumer know when it can start
	*	         drawing power from the Vbus. The parameter to the callback indicates the
	*	         maximum amount of current (in milli-amps) that can be safely drawn.
	*	 - Type: pm_app_otg_usb_power_sink_open_notification_cb_type
	*	 - Valid inputs: Valid non-NULL callback fn
	*	    
	*	\param sink_close_notification_cb
	*	 - Callback function that lets the USB power consumer know when it MUST stop
	*	         drawing power from the Vbus.
	*	 - Type: pm_app_otg_usb_power_sink_close_notification_cb_type
	*	 - Valid inputs: Valid non-NULL callback fn
	*	 \return None.
	*	 \dependencies The following function(s) must have been called: pm_init()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	void ConfigureUsbPowerConsumer(
			pm_app_otg_usb_power_source_connected_notification_cb_type    source_connected_cb,
			pm_app_otg_usb_power_source_disconnected_notification_cb_type source_disconnected_cb,
			pm_app_otg_usb_power_sink_open_notification_cb_type           sink_open_notification_cb,
			pm_app_otg_usb_power_sink_close_notification_cb_type          sink_close_notification_cb,
			pm_app_otg_usb_power_source_suspended_notification_cb_type	 source_suspended_cb 
		);



	/*!
	*	 \brief pm_app_otg_set_usb_power_consumption_requirement(boolean need_usb_power)
	*	 \details This function indicates to the OTG transceiver if the local USB power
	*	    consumer is interested in drawing power from the Vbus. Only if there is
	*	    an interest, will the transceiver manage the power sink by opening/closing
	*	    as necessary. Otherwise the power sink remains closed.
	*	    An example of a local power consumer would be a "USB charger" application.
	*	 
	*	 \param need_usb_power
	*		- Indicates if the power consumer is interested in drawing power
	*	         from the Vbus powered by a remote A-device. Note however that an
	*	         interest doesn't mean that Vbus could be used immediately. Wait
	*	         for a notification through 'pm_app_otg_usb_power_sink_open_notification_cb_type'.        
	*		- Valid boolean inputs:
	*			-   TRUE  --> Interested in drawing power from the Vbus
	*			-   FALSE --> Otherwise
	*	 \return None.
	*	 \dependencies The following function(s) must have been called:
	*	 \li pm_init()
	*	 \li pm_app_otg_configure_usb_power_consumer()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	void SetUsbPowerConsumptionRequirement( boolean need_usb_power);

	/*!
	*	 \breif pm_app_otg_process_usb_power_line_control_request(boolean need_control)
	*	 \details This function places a request with PM OTG application for control over
	*	    the voltage on USB_VBUS line. This function shall be used by the client
	*	    when there is a desire or a potential chance (as a precautionary measure)
	*	    for affecting voltage on USB_VBUS while connected to an OTG A-device.
	*	    
	*	    Note that a request, if granted, will be in effect only for the current
	*	    session with the remote A-dev. That is, if the current A-dev disconnects,
	*	    a new request will have to made to regain control, if necessary, while
	*	    connected to any OTG A-dev in the future.
	*	    
	*	    Also it is highly important that the client shall NOT distort USB_VBUS to
	*	    such an extent that it could cause problems for the remote A-dev that is
	*	    supplying power on USB_VBUS.
	*	       
	*	    It is highly recommended that the time duration of the granted control be
	*	    kept to a minimum as much as possible as it will otherwise interfere with
	*	    the optimal operation of OTG transceiver (as far as power utilization is
	*	    concerned).
	*	 
	*	 \param need_control 
	*		-  Indicates if a control over USB_VBUS line voltage is desired or not.
	*	    -  Valid boolean inputs:
	*			-  TRUE  --> Control desired
	*			-  FALSE --> Control no longer desired
	*	                          
	*	 \return Indicates if the request has been successfully processed by the PM OTG application.
	*		- Possible booleanvalues:
	*			- TRUE   --->  Success
	*			- FALSE  --->  Failure  (In this case, client SHALL not wrest control away from OTG application)
	*	 
	*	 \dependencies The following function(s) must have been called:
	*	 \li  pm_init()
	*	 \li  pm_app_otg_configure_usb_power_consumer()
	*	 
	*	 \note Interrupts are locked during the execution of this function.
	*/
	boolean ProcessUsbPowerLineControlRequest( boolean need_control);

protected:
private:

	/*! \brief mpPowerConsumer is used to hold pointer to PowerConsumer object instance
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li PmAppOtg_PowConsumer()
	*   \li ConfigureUsbPowerConsumer()
	*   \li SetUsbPowerConsumptionRequirement()
	*/
	PowerConsumer* mpPowerConsumer;

	/*! \brief mpUsbPowerConsumerConfigInfo is used to hold pointer to UsbPowerConsumerConfigInfo object instance
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li PmAppOtg_PowConsumer()
	*   \li ConfigureUsbPowerConsumer()
	*/
    UsbPowerConsumerConfigInfo* mpUsbPowerConsumerConfigInfo;


	/*! \brief This static variable is used to hold the singlton instance of class PmAppOtg_PowConsumer object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static PmAppOtg_PowConsumer* mpPmAppOtg_PowConsumer_Instance;

	/*! \brief mpOtgDiagEvent is used to hold pointer to OtgDiagEvent object instance
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li PmAppOtg_PowConsumer()
	*   \li ConfigureUsbPowerConsumer()
	*   \li SetUsbPowerConsumptionRequirement()
	*/
	OtgDiagEvent* mpOtgDiagEvent;

};




/**************************************************************************
     class PmAppOtg_Carkit												  
**************************************************************************/

/*! \class PmAppOtg_Carkit 
 *  \brief This class has Carkit related APIs used by upper software layer (USb carkit driver)
 *  \details  The APIs include:
 *	-  Enabling carkit mode, 
 *	-  Enabling/disabling Carkit interrupt detection and
 *	-  Enabling Data during audio mode
 *  \note none
 */
class PmAppOtg_Carkit
{
public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	PmAppOtg_Carkit();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~PmAppOtg_Carkit();


	/*! \brief Utility function used to create a singlton instance of PmAppOtg_Carkit object
	*  \details none
	*  \param none
	*  \return singlton instance of PmAppOtg_Carkit object
	*  \note none
	*/
	static PmAppOtg_Carkit* GetInstance();

	/*! \brief Utility function used to initiate destruction of PmAppOtg_Carkit class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
    static void DestroyInstance();

	/*!
	*	 \brief pm_app_otg_enter_carkit_mode(pm_app_otg_carkit_mode_type carkit_mode)
	*	 \details This function requests the OTG transceiver to change its operational
	*	    mode to the one specified during a session with a carkit.
	*	 
	*	 \param carkit_mode
	*	 \li Carkit operational mode to be entered
	*	 \li   Valid pm_app_otg_carkit_mode_type inputs:
	*	 \li         PM_APP_OTG_CARKIT_MODE__UART
	*	 \li         PM_APP_OTG_CARKIT_MODE__MONO
	*	 
	*	 \return The immediate return value of this function indicates if the OTG 
	*       transceiver has accepted the request successfully.
	*	 \li   - Possible pm_err_flag_type values:
	*	 \li     - TRUE   --->  Upon completion of the processing of this request,
	*	                       the OTG transceiver will call back into the client
	*	                       using 'carkitclient_mode_entry_rsp_fn' specified in 
	*	                       pm_app_otg_init().
	*	 \li     - FALSE  --->  Client's callback function 'carkitclient_mode_entry_rsp_fn'
	*                      will not be invoked in this case.
	*
	*	 \dependencies The following function(s) must have been called: pm_init(), pm_app_otg_init()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	boolean EnterCarkitMode( pm_app_otg_carkit_mode_type carkit_mode); 

	/*!
	*	 \brief pm_app_otg_enable_carkit_interrupt_detection()
	*	 \details This function enables the mechanism for detecting interrupt pulses
	*	    generated by carkit on USB D+ line.
	*	 \param None.
	*	 \return None.
	*	 \dependencies The following function(s) must have been called: pm_init(), pm_app_otg_init()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	void EnableCarkitInterruptDetection(void);                         

	/*!
	*	 \brief pm_app_otg_disable_carkit_interrupt_detection()
	*	 \details This function disables the mechanism for detecting interrupt pulses
	*	    generated by carkit on USB D+ line.
	*	 \param None.
	*	 \return None.
	*	 \dependencies The following function(s) must have been called:pm_init(), pm_app_otg_init()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	void DisableCarkitInterruptDetection(void); 

	/*!
	*	 \brief pm_app_otg_data_during_audio_supported()
	*	 \detail This function queries if the OTG transceiver supports data-during-audio
	*	    capability for interfacing with USB smart carkits.
	*	 \param None.
	*	 \return Type: boolean
	*	 \li Possible values:
	*	 \li	TRUE   --->  Data-during-audio capability supported
	*	 \li    FALSE  --->  Otherwise
	*	 \dependencies The following function(s) must have been called:pm_init(), pm_app_otg_init()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	boolean DataDuringAudioSupported(void); 
protected:
private:

	/*! \brief mpPmAppOtg_Carkit_Instance is used to hold pointer to PmAppOtg_Carkit object instance
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static PmAppOtg_Carkit* mpPmAppOtg_Carkit_Instance;

	/*! \brief mpStateManager is used to hold pointer to StateManager object instance
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li PmAppOtg_Carkit()
	*   \li EnterCarkitMode()
	*   \li EnableCarkitInterruptDetection()
	*   \li DisableCarkitInterruptDetection()
	*/
	StateManager* mpStateManager;

	/*! \brief mpUtil is used to hold pointer to Util object instance
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li PmAppOtg_Carkit()
	*   \li EnableCarkitInterruptDetection()
	*   \li DisableCarkitInterruptDetection()
	*/
	Util* mpUtil;
};






/**************************************************************************
     class PmAppOtg														  
**************************************************************************/

/*! \class PmAppOtg 
 *  \brief This class has routins that supports PMIC USB APP OTG driver related APIs needed by upper software layer
 *  \details  
 *  \note Init() must be called first to initialize PM APP OTG driver
 */
class PmAppOtg
{
public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	PmAppOtg();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~PmAppOtg();

	/*! \brief Utility function used to create a singlton instance of PmAppOtg object
	*  \details none
	*  \param none
	*  \return singlton instance of PmAppOtg object
	*  \note none
	*/
	static PmAppOtg* GetInstance();

	/*! \brief Utility function used to initiate destruction of PmAppOtg class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
    static void DestroyInstance();


	/*!
	*  \brief Init():  Initilaize PM APP OTG driver
	*  \details This function initializes and sets up the USB-OTG transceiver for
	*    - Detection and handling of connection/disconnection of external OTG devices.
	*    - Detecting "resume" signal initiated by the remote device (host or peripheral) on the bus.
	*  \param 1. init_done_cb
	*     - Callback function into OTG controller for notifying about the completion of this
	*       initialization process; it is guaranteed that this notification will be
	*       made before any other callbacks begin to get invoked. This gives the OTG
	*       controller a chance to prepare itself for some last minute initializations
	*       before it could handle the other operational callbacks properly.
	*       - Type: pm_app_otg_ctrlr_init_done_cb_type
	*       - Valid inputs:   Valid non-NULL callback fn
	*       
	*  \param 2. host_mode_init_cb
	*     - Callback function into OTG controller for initializing it for "host/A-dev"
	*       mode operation; this callback will be invoked as soon as the transceiver
	*       detects connection to a "peripheral/B-dev".
	*       - Type: pm_app_otg_ctrlr_host_mode_init_cb_type
	*       - Valid inputs: Valid non-NULL callback fn
	*       
	*  \param 3. peripheral_mode_init_cb
	*      - Callback function into OTG controller for initializing it for "peripheral/B-dev"
	*        mode operation; this callback will be invoked as soon as the transceiver
	*        detects connection to a "host/A-dev".
	*        - Type: pm_app_otg_ctrlr_peripheral_mode_init_cb_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 4. rs232dongle_mode_init_cb
	*      - Callback function into OTG controller for initializing it for "RS-232 dongle"
	*        mode operation; this callback will be invoked as soon as the transceiver
	*        detects connection to an "RS-232 dongle".
	*        - Type: pm_app_otg_ctrlr_rs232dongle_mode_init_cb_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 5. carkitclient_mode_init_cb
	*      - Callback function into OTG controller for initializing it for a session with
	*        a carkit; this callback will be invoked as soon as the transceiver
	*        detects connection to a carkit.
	*        - Type: pm_app_otg_ctrlr_carkitclient_mode_init_cb_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 6. carkitclient_int_hdlr
	*      - Callback function into OTG controller for notification of carkit interrupts.
	*        - Type: pm_app_otg_ctrlr_carkitclient_int_hdlr_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 7. carkitclient_mode_entry_rsp_fn
	*      - Callback function into OTG controller for responding back to an earlier request
	*        from controller to change carkit's operational mode.
	*        - Type: pm_app_otg_ctrlr_carkitclient_mode_entry_rsp_fn_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 8. remote_dev_lost_cb
	*      - Callback function for notifying OTG controller about disconnection from the 
	*        external OTG device.
	*        - Type: pm_app_otg_ctrlr_remote_dev_lost_cb_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 9. remote_dev_resumed_cb
	*      - Callback function for notifying OTG controller about "resume" signal indicated
	*        by the external USB device on the bus.
	*        - Type: pm_app_otg_ctrlr_remote_dev_resumed_cb_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 10. remote_host_wakeup_fn
	*     - Function for requesting OTG controller to initiate signaling for performing
	*       remote wakeup of the suspended host.
	*       - Type: pm_app_otg_ctrlr_remote_host_wakeup_fn_type
	*       - Valid inputs: Valid non-NULL fn
	*
	*  \param 11. error_notification_cb
	*      - Callback function for notifying OTG controller about any errors that have
	*        happened during operation.
	*        - Type: pm_app_otg_ctrlr_error_notification_cb_type
	*        - Valid inputs: Valid non-NULL callback fn
	*
	*  \param 12. sleep_assert_fn
	*     - Callback function for requesting OTG controller to assert or negate sleep
	*       on behalf of this transceiver.
	*       - Type: pm_app_otg_ctrlr_sleep_assert_fn_type
	*       - Valid inputs: Valid non-NULL callback fn
	*
	*  \return    None.
	*  \dependencies  The following function(s) must have been called: 1) pm_init()
	*  \note  Interrupts are disabled during this function.
	*  \note  Will ERR_FATAL if back-to-back pm_app_otg_init() calls are made without
	*		  an intervening pm_app_otg_reset().
	*	\note Table of Call back functions and functions where they are called:
	*		- CB1. init_done_cb		: B_Idle::Entry()
	*		- CB2. host_mode_init_cb		: A_Host::Entry()
	*		- CB3. peripheral_mode_init_cb	: B_Peripheral_State::Entry()
	*		- CB4. rs232dongle_mode_init_cb	: Rs232Dongle_Config_State::Entry()
	*		- CB5. carkitclient_mode_init_cb	: Carkitclient_Init_State::Entry()
	*		- CB6. carkitclient_int_hdlr	: Carkitclient_Uart_State::CachedEvtHdlr()
	*				and Carkitclient_Carkit_Irq_State::Entry()
	*		- CB7. carkitclient_mode_entry_rsp_fn:Carkitclient_Init_State::CachedEvtHdlr(),
	*				Carkitclient_Uart_State::Entry() and Carkitclient_Audio_State::CachedEvtHdlr()
	*		- CB8. remote_dev_lost_cb		: B_Idle_State::Entry()
	*		- CB9. remote_dev_resumed_cb	: A_Suspend_State::CachedEvtHdlr()
	*		- CB10. remote_host_wakeup_fn	: B_Suspend_State::Entry()
	*		- CB11. error_notification_cb	: A_Wait_Bcon_State::CachedEvtHdlr() and A_Vbus_Err_State::Entry()
	*		- CB12. sleep_assert_fn		: B_Idle_State::Entry():true,  B_RemoteDevDetectWait_State::Entry():false
	*/
	void Init(
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
		pm_app_otg_ctrlr_sleep_assert_fn_type            sleep_assert_fn 
		);



	/*!
	*	\brief Reset():  Resets PM APP OTG driver
	*
	*	\detail This function shuts down the OTG application state machine gracefully.
	*	   Use this function when it is no longer necessary to detect (dis)connection
	*	   of remote OTG devices. An example of such an instance would be when the
	*	   system is powering down.
	*   
	*   Use pm_app_otg_init() to restore OTG application functionality back again
	*   in case it is ever needed after this shutdown.
	*	\param None
	*	\return none           
	*	\dependencies The following function(s) must have been called: pm_init()
	*	\note Interrupts are disabled during this function.
	*/
	void Reset(void); 

	/*!
	*  \brief GetCtrlrInfo()
	*  \details This function returns info about the current USB-OTG controller that was
	*     last programmed through pm_app_otg_init(). pm_app_otg_reset() will clear
	*     info about USB-OTG controller that may have been setup using prior
	*     pm_app_otg_init() call; in that case this function will continue to return
	*     NULL until pm_app_otg_init() is subsequently invoked.
	*  \param None.          
	*  \return Returns info about the currently programmed USB-OTG controller.
	*    - Possible pm_app_otg_ctrlr_info_type return values:
	*      - non-NULL  --->  Valid info about the currently programmed USB-OTG controller
	*      - NULL      --->  Not setup with any USB-OTG controller yet
	*	\dependencies The following function(s) must have been called: pm_init(), PmAppOtg::Init()
	*/
	const pm_app_otg_ctrlr_info_type* GetCtrlrInfo(void);


	/*!
	*	\breif AcquireBus(short tmo_secs)
	*	\detail This function initiates bus acquisition procedure for detecting any remote
	*	   USB device when no USB device has been detected yet since the last
	*	   disconnection. Upon successful detection of any type of USB device on the
	*	   bus, the appropriate USB mode initialization callback function (registered
	*	   by this transceiver application's client through pm_app_otg_init()) will
	*	   be called.
	*	
	*	   If a USB device is not detected before the timeout occurs, the error
	*	   notification callback function (again registered through pm_app_otg_init())
	*	   will be called with the error type 
	*	   PM_APP_OTG_OPERATIONAL_ERROR_TYPE__REMOTE_DEV_NOT_PRESENT.
	*	\param tmo_secs
	*	      - Attempt to acquire USB bus until timeout as specified in 'tmo_secs'
	*	        (described below) happens.
	*	      - Value provided in 'tmo_secs' is interpreted in the following manner:
	*	        - Any negative value means infinite timeout duration
	*	        - A value of zero is used for indicating that the acquisition
	*	          procedure be conducted just once
	*	        - Any positive value indicates the timeout duration in seconds
	*	      - Type: int8 (signed)
	*	\return none
	*	\dependencies The following function(s) must have been called: pm_init(), PmAppOtg::Init()
	*	\note Interrupts are locked during the execution of this function.
	*/
	void AcquireBus(short tmo_secs);  //#ifdef FEATURE_HTORPC_METACOMMENTS

	/*!
	*	\brief RelinquishBus()
	*	\detail This function serves either to halt a bus acquisition procedure still in 
	*	   progress or for relinquishing the bus that has already been acquired
	*	   (for communicating with a remote USB device). 
	*	\param None
	*	\return None.
	*	\dependencies The following function(s) must have been called:
	*	   1) pm_init()
	*	   2) pm_app_otg_init()
	*	   3) pm_app_otg_acquire_bus()
	*	\note Interrupts are locked during the execution of this function.
	*/
	void RelinquishBus(void);	//#ifdef FEATURE_HTORPC_METACOMMENTS


	/*!
	*	\brief GetLocal_Vbus_CurrentSourcingCapability()
	*	\detail This function returns the amount of current in milli-Amps that the local
	*	   PMIC USB-OTG xcvr is rated to reliably source into connected B-devices
	*	   while still being able to meet USB2.0 VBUS voltage requirements.
	*	\param None
	*	\return Type: uint32, Source current rating in mA
	*	\note None.
	*/
	unsigned int GetLocal_Vbus_CurrentSourcingCapability(void); //#ifdef FEATURE_HTORPC_METACOMMENTS


	/*!
	*	\breif SetRemote_A_DevInfo(pm_app_otg_remote_a_dev_info_type info)
	*	\detail This function records information on the OTG A-dev connected on the remote
	*	   end of the USB cable as seen by this phone.
	*	\param info : Info on the remote OTG A-dev., Type: pm_app_otg_remote_a_dev_info_type
	*	\return None.
	*	\note Interrupts are locked during the execution of this function.
	*/
	void SetRemote_A_DevInfo( pm_app_otg_remote_a_dev_info_type info); //#ifdef FEATURE_HTORPC_METACOMMENTS


	/*!
	*	\brief Suspend()
	*	\details This function forces the data bus into an idle (suspended) state. The
	*	   transceiver's operating mode is also changed to a hibernating state so
	*	   that it draws only a minimal amount of current.
	*	\param None.
	*	\return None.
	*	\dependencies The following function(s) must have been called: pm_init(), pm_app_otg_init()
	*	\note Interrupts are locked during the execution of this function.
	*/
	void Suspend(void); //#ifdef FEATURE_HTORPC_METACOMMENTS


	/*!
	*	\brief pm_app_otg_resume()
	*	\details This function gets the data bus out of the idle (suspended) state. The
	*	   transceiver's operating mode is reverted back to a fully functional state
	*	   so that data transfers on the bus can take place again.
	*	\param None.
	*	\return None.
	*	\dependencies The following function(s) must have been called:
	*	\li pm_init()
	*	\li pm_app_otg_init()
	*	\li pm_app_otg_suspend()
	*	\note Interrupts are locked during the execution of this function.
	*/
	void Resume(void); //#ifdef FEATURE_HTORPC_METACOMMENTS


	/*!
	*	\breif ProcessRemotePeripheral_DisconnectedEvt()
	*	\details When acting as USB host, this function lets the OTG controller notify the
	*	   transceiver about the disconnection of the remote USB peripheral.
	*	\param None.
	*	\return None.
	*	\dependencies The following function(s) must have been called:
	*	   1) pm_init()
	*	   2) pm_app_otg_init()
	*	   3) Host mode initialization callback that is registered through
	*	   pm_app_otg_init() must have been invoked by this transceiver before
	*	   this function can be called to notify this transceiver that the remote peripheral
	*	   disconnected.
	*	\note Interrupts are locked during the execution of this function.
	*/	
	void ProcessRemotePeripheral_DisconnectedEvt(void); //#ifdef FEATURE_HTORPC_METACOMMENTS


	/*!
	*	\breif pm_app_otg_set_remote_wakeup_capability()
	*	
	*	\detail This function is used for setting the status of remote wakeup capability
	*	   depending on operational mode as follows:
	*	\li In Peripheral mode, this function would be used to indicate if the
	*	      remote USB host has enabled this feature for USB-OTG controller and this
	*	      transceiver to attempt a remote wakeup in SUSPEND state.
	*	\li In Host mode, this function would be used to indicate if the USB-OTG
	*	      controller had enabled remote wakeup capability on the connected peripheral.
	*	
	*	\li This setting will remain in effect only for the duration of a USB session as
	*	   delimited by host/peripheral mode initialization callback and remote device
	*	   lost callback into the USB-OTG controller.
	*	   
	*	\li Remote wakeup capability will default to disabled status upon every host/peripheral
	*	   mode initialization and will remain that way until explicitly enabled/disabled
	*	   through the use of this function.
	*	   
	*	\param remote_wakeup_capability_enabled
	*		- Used for indicating if remote wakeup feature has been enabled for the current USB session.
	*		- Valid boolean inputs:
	*			- TRUE  --->  Remote wakeup capability has been enabled
	*			- FALSE --->  Otherwise 
	*	\return None.
	*	\dependencies The following function(s) must have been called:
	*	\li pm_init()
	*	\li pm_app_otg_init()
	*	\li Host/peripheral mode initialization callback that is registered through pm_app_otg_init().
	*/
	void SetRemote_WakeupCapability( boolean  remote_wakeup_capability_enabled); // #ifdef FEATURE_HTORPC_METACOMMENTS



	/*!
	*	\breif PrepareFor_RemoteHostWakeupSignaling()
	*	\details When acting as USB peripheral and while the bus has been put into SUSPEND
	*	   state by the remote host, this function lets the OTG controller request the
	*	   transceiver to set itself up for remote wakeup signaling. The transceiver
	*	   configures itself for the most optimal power consumption state while still
	*	   letting USB controller perform remote wakeup signaling. It is possible for
	*	   the current drawn from USB_VBUS to exceed USB2.0 SUSPEND current limit during
	*	   the interval between the invocation of this function and the subsequent
	*	   pm_app_otg_process_evt__remote_host_wakeup_done().
	*	
	*	   That's why it is very important for the OTG controller to keep the remote
	*	   wakeup duration to be as short as possible and to invoke
	*	   pm_app_otg_process_evt__remote_host_wakeup_signaling_done() as soon as remote wakeup
	*	   signaling is completed. Stated another way, OTG controller shall NOT wait
	*	   for remote host to actually wake up before it invokes
	*	   pm_app_otg_process_evt__remote_host_wakeup_signaling_done(). The remote wakeup detection
	*	   will be carried out through 'remote_dev_resumed_cb' registered through
	*	   pm_app_otg_init().
	*	
	*	\param None.
	*	\return None.
	*	
	*	\dependencies The following function(s) must have been called:
	*	\li pm_init()
	*	\li pm_app_otg_init() with a non-NULL 'remote_dev_resumed_cb'
	*	\li Peripheral mode initialization callback that is registered through pm_app_otg_init().
	*	\li Remote wakeup capability must have already been turned ON for the current USB session 
	*       using pm_app_otg_set_remote_wakeup_capability()
	*	\li pm_app_otg_suspend() to put the transceiver in SUSPEND mode
	*
	*	\note Interrupts are locked during the execution of this function.
	*/
	void PrepareFor_RemoteHostWakeupSignaling(void); //#ifdef FEATURE_HTORPC_METACOMMENTS

	
	
	/*!
	*	\brief ProcessRemoteHost_WakeupSignalingDoneEvt()
	*	\details When acting as USB peripheral, this function lets the OTG controller
	*	   indicate to the transceiver about the completion of USB2.0 remote wakeup
	*	procedure. This lets the transceiver go into USB2.0 suspend current limit
	*	state while still being able to detect non-idle signaling from the remote host.
	*	\param None.
	*	\return None.
	*	\dependencies The following function(s) must have been called:
	*	\li pm_init()
	*	\li pm_app_otg_init() with a non-NULL 'remote_dev_resumed_cb'
	*	\li Peripheral mode initialization callback that is registered through pm_app_otg_init().
	*	\li Remote wakeup capability must have already been turned ON for the current USB session 
	*	    using pm_app_otg_set_remote_wakeup_capability()
	*	\li pm_app_otg_suspend() to put the transceiver in SUSPEND mode
	*	\li pm_app_otg_prepare_for_remote_host_wakeup_signaling() to request that xcvr be
	*	    setup for remote wakeup signaling while in SUSPEND state.
	*	\note Interrupts are locked during the execution of this function.
	*/
	void ProcessRemoteHost_WakeupSignalingDoneEvt(void); //#ifdef FEATURE_HTORPC_METACOMMENTS


	//#ifdef FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE
	/*!
	*	 \brief Start_B_DevSession()
	*	 \details This function serves to start an OTG B-dev session on the PM OTG 
	*	    transceiver. This interface is not available when a physical connection
	*	    exists between PM OTG xcvr's and remote A-dev's USB_VBUS line; in such
	*	    a scenario, this OTG application monitors the USB_VBUS voltage condition
	*	    on its own and acts autonomously without any external assistance.
	*	 \param None.
	*	 \return None.
	*	 \dependencies The following function must have been called:pm_init(), pm_app_otg_init()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	void Start_B_DevSession(void);


	/*!
	*	 \brief Stop_B_DevSession()
	*	 \details This function serves to end an OTG B-dev session already in progress on the
	*	    PM OTG transceiver. This interface is not available when a physical 
	*	    connection exists between PM OTG xcvr's and remote A-dev's USB_VBUS line;
	*	    in such a scenario, this OTG application monitors the USB_VBUS voltage
	*	    condition on its own and acts autonomously without any external assistance.
	*	 \param None.
	*	 \return None.
	*	 \dependencies The following function must have been called:pm_init(), pm_app_otg_init()
	*	 \note Interrupts are locked during the execution of this function.
	*/
	void Stop_B_DevSession(void);
	//#endif //FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE

	/*!
	*  \brief GetCurrentOperationalMode()
	*  \details This function returns info on the current mode that this xcvr application
	*		is operating in.
	*  \param none   
	*  \return Returns info about the current operational mode of this xcvr.
	*   - Possible pm_app_otg_operational_mode_type return values:
	*     - PM_APP_OTG_OPERATIONAL_MODE__PERIPHERAL  --> operating as a USB peripheral (not necessarily B-dev)
	*     - PM_APP_OTG_OPERATIONAL_MODE__HOST        --> operating as a USB host (not necessarily A-dev)
	*     - PM_APP_OTG_OPERATIONAL_MODE__CARKIT      --> connected to a USB CEA-936-A carkit
	*     - PM_APP_OTG_OPERATIONAL_MODE__RS232DONGLE --> connected to an RS232 dongle
	*     - PM_APP_OTG_OPERATIONAL_MODE__NONE        --> not operating in any of the above modes
	*  \dependencies The following function(s) must have been called:pm_init(), PmAppOtg::Init()
	*/
	pm_app_otg_operational_mode_type GetCurrentOperationalMode(void);

	/*! \brief mpPmAppOtg_Instance is used to hold pointer to PmAppOtg object instance
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static PmAppOtg* mpPmAppOtg_Instance; //had to make it public to avoid unneeded inheretance of PmAppOtg_Carkit and PmAppOtg_PowConsumer objects from PmAppOtg
										  //the other alternative is to merge these objects with class PmAppOtg
protected:

private:
	/*! \brief mpUtil is used to hold pointer to Util object instance
	*/
	Util* mpUtil;

	/*! \brief mpEvtUtil is used to hold pointer to EvtUtil object instance
	*/
	EvtUtil* mpEvtUtil;

	/*! \brief mpPowerConsumer is used to hold pointer to PowerConsumer object instance
	*/
	PowerConsumer* mpPowerConsumer;

	/*! \brief mpStateManager is used to hold pointer to StateManager object instance
	*/
	StateManager* mpStateManager;

	/*! \brief mpEvtDebugger is used to hold pointer to EvtDebugger object instance
	*/
	EvtDebugger* mpEvtDebugger;

	/*! \brief mpUsbBusListener is used to hold pointer to UsbBusListener object instance
	*/
	UsbBusListener* mpUsbBusListener;

	/*! \brief mpGenericEvtDetectIsr is used to hold pointer to GenericEvtDetectIsr object instance
	*/
	GenericEvtDetectIsr* mpGenericEvtDetectIsr;

	/*! \brief mpOtgTimer is used to hold pointer to OtgTimer object instance
	*/
	OtgTimer* mpOtgTimer;

	/*! \brief mpOtgDiagEvent is used to hold pointer to OtgDiagEvent object instance
	*/
	OtgDiagEvent* mpOtgDiagEvent;

	/*! \brief mpCustomHw is used to hold pointer to CustomHw object instance
	*/
    CustomHw* mpCustomHw;
};

#endif //FEATURE_PMIC_USB_OTG

#endif //PMAPP_OTG_INTERFACE_H
