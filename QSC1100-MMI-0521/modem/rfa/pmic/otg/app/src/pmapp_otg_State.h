#ifndef PMAPP_OTG_STATE_H
#define PMAPP_OTG_STATE_H

/*! \file 
*  \n
*  \brief  pmapp_otg_State.h  ----- PM APP OTG State machine related class declaration
*  \details This header file contains class declarations of USB OTG states 
*     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_State.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pmapp_otg_Util.h"
#include "pmapp_otg_CustomHw.h"
#include "pmapp_otg_DiagEvent.h"

#ifdef FEATURE_PMIC_USB_OTG

/*===========================================================================
FORWARD DECLARATION
===========================================================================*/
class StateFactory;
class StateManager;

/*===========================================================================
CLASS DEFINITION
===========================================================================*/


/**************************************************************************
     class BaseState											  
**************************************************************************/
/*! \class BaseState 
*  \brief Base state for all states in PMIC APP OTG states machine. 
*  \details  All pm app otg states inheret from this class.  This class defines virtual functions
*		and variables that are used commonly by all states.
*  \note this class should not be instatnitated
*  \image html otg_state_machine_cb.png
*/
class BaseState 
{
	/*! \brief friend class of BaseState
	*  \details used to be able to delete instances of state
	*/
	friend class StateFactory; 
public:

	/*! \brief This is a virtual function used to perform the necessary actions required before the state
	*			machine could transition to a particular state.
	*  \details All classes inheriting form this state would need to implement this function
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This is a virtual function used to processes interrupts / external events 
	*		of interest to a particular state and returns the "next" state, should a state
	*		transition be necessary.
	*  \details All classes inheriting form this state would need to implement this function
	*  \param none
	*  \return Type: BaseState ; next state pointer
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

	/*! \brief    This function processes the request from OTG controller to suspend 
	*				the bus while in a particular state.
	*  \details none
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Type: boolean; Possible values:
	*		-	TRUE --> success
	*		-   FALSE --> failure
	*  \note none
	*/
	virtual boolean SuspendEvtHdlr();

	/*! \brief This function processes the request from OTG controller to resume 
	*			bus operation while in a particular state.
	*  \details none
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Type: boolean; Possible values:
	*		-	TRUE --> success
	*		-   FALSE --> failure
	*  \note none
	*/
	virtual boolean ResumeEvtHdlr();

	/*! \brief This virtual function is used to setup remote wake up capability
	*  \details When acting as USB peripheral and while the bus has been put into SUSPEND
	*		state by the remote host, this function lets the OTG controller request the
	*		transceiver to set itself up for remote wakeup signaling. The transceiver
	*		configures itself for the most optimal power consumption state while still
	*		letting USB controller perform remote wakeup signaling.
	*  \param state : Indicates if remote wakup is desired or not
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean RemoteHostWakeupSignaling_SetupRequestHdlr(boolean state);


	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to a state in which this request is considered valid
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);


	/*! \brief This function requests the OTG transceiver to change its operational
	*			mode to the one specified during a session with a carkit.
	*  \details none
	*  \param carkit_mode_type; Carkit operational mode to be entered
	*		- Type: pm_app_otg_carkit_mode_type
	*		- Valid inputs:
	*			- PM_APP_OTG_CARKIT_MODE__UART
	*			- PM_APP_OTG_CARKIT_MODE__MONO
	*  \return The immediate return value of this function indicates if the OTG 
	*		       transceiver has accepted the request successfully.
	*		- Type: pm_err_flag_type
	*		- Possible values:
	*			- TRUE   --->  Upon completion of the processing of this request,
	*                  the OTG transceiver will call back into the client
	*                  using 'carkitclient_mode_entry_rsp_fn' specified in 
	*                  pm_app_otg_init().
	*			- FALSE  --->  Client's callback function 'carkitclient_mode_entry_rsp_fn'
	*                  will not be invoked in this case.
	*  \note none
	*/
	virtual boolean CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode_type);

	/*! \brief This variable holds a unique state ID used to identify any state. 
	*  \details none
	*/
	StateEventIdType mStateId;
protected:
	/*! \brief Constructor
	*  \details none
	*/
	BaseState();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~BaseState();

	/*! \brief This var holds the name of the state
	*  \details Used only for debugging
	*/
	const char  *mStateName;     

	/*! \brief This variable is used to hold a payload value to diag event handler
	*  \details  Used for debugging
	*/
	int8 mOtgEventPayload;

	/*! \brief This variable is used to hold pointer to UsbBusListener object instance
	*  \details none
	*/
	UsbBusListener* mpUsbBusListener;

	/*! \brief This variable is used to hold pointer to GenericEvtDetectIsr object instance
	*  \details none
	*/
	GenericEvtDetectIsr* mpGenericEvtDetectIsr;

	/*! \brief This variable is used to hold pointer to OtgTimer object instance
	*  \details none
	*/
	OtgTimer* mpOtgTimer;

	/*! \brief This variable is used to hold pointer to Util object instance
	*  \details none
	*/
	Util* mpUtil;

	/*! \brief This variable is used to hold pointer to EvtUtil object instance
	*  \details none
	*/
	EvtUtil* mpEvtUtil;

	/*! \brief This variable is used to hold pointer to PowerConsumer object instance
	*  \details none
	*/
	PowerConsumer* mpPowerConsumer;

	/*! \brief This variable is used to hold pointer to StateFactory object instance
	*  \details none
	*/
	StateFactory* mpStateFactory;


	/*! \brief This variable is used to hold pointer to StateManager object instance
	*  \details none
	*/
	StateManager* mpStateManager;  


	/*! \brief This variable is used to hold pointer to CustomHw object instance
	*  \details none
	*/
	CustomHw* mpCustomHw;

	/*! \brief This variable is used to hold pointer to OtgDiagEvent object instance
	*  \details none
	*/
	OtgDiagEvent* mpOtgDiagEvent;

	/*! \brief This variable is used to hold pointer to DpStateMonitor object instance
	*  \details none
	*/
	DpStateMonitor* mpDpStateMonitor;


	/*! \brief This variable is used to hold pointer to DmStateMonitor object instance
	*  \details none
	*/
	DmStateMonitor* mpDmStateMonitor;

private:

};



/**************************************************************************
     class Null_State											  
**************************************************************************/
/*! \class Null_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*		state machine "NULL" state.
*  \details  "NULL" state is the default state befor PM APP OTG state machine is initialized.
*  \note none
*/
class Null_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Null_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Null_State();


	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Null" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();


	/*! \brief This function processes interrupts / external events of interest to "Null"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Null", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

};






/**************************************************************************
     class B_Idle_State											  
**************************************************************************/
/*! \class B_Idle_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "B_Idle" state.
*  \details  When host USB device is detected (based on ID line level), PM APP OTG 
*				state machine gets into "B_Idle" state.
*  \note none
*/
class B_Idle_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	B_Idle_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~B_Idle_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "B_Idle" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "B_Idle"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "B_Idle", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "Null"
	*		- "A_Idle"
	*		- "B_RemoteDevDetectWait"
	*		- "Rs232Dongle_Config"
	*		- "Unsought"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

};


/**************************************************************************
     class B_RemoteDevDetectWait_State											  
**************************************************************************/
/*! \class B_RemoteDevDetectWait_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "B_RemoteDevDetectWait" state.
*  \details  In "B_RemoteDevDetectWait" state, the various types of USB host
*				 devices (USB Host, USB wall charger) gets detected
*  \note none
*/
class B_RemoteDevDetectWait_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	B_RemoteDevDetectWait_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~B_RemoteDevDetectWait_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "B_RemoteDevDetectWait" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "B_RemoteDevDetectWait"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "B_RemoteDevDetectWait", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "b_Idle"
	*		- "Unsought"
	*		- "B_Peripheral"
	*		- "Chg_Conn"
	*		- "Carkitclient_Init"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

};



/**************************************************************************
     class B_Peripheral_State											  
**************************************************************************/
/*! \class B_Peripheral_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "B_Peripheral" state.
*  \details  When a host USB device is successfully detected, 
*			PM APP OTG state macine gets in to "B_Peripheral" state and acts 
*			as a peripheral device.
*  \note none
*/
class B_Peripheral_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	B_Peripheral_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~B_Peripheral_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "B_Peripheral" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "B_Peripheral"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "B_Peripheral", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "b_Idle"
	*		- "B_Suspend"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();


	/*! \brief This function processes the request from OTG controller to suspend the bus
	*			while in "B_Peripheral" state.
	*  \details none
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Type: boolean; Possible values:
	*		-	TRUE --> success
	*		-   FALSE --> failure
	*  \note none
	*/
	virtual boolean SuspendEvtHdlr();


	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to "B_Peripheral" state.
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);
protected:
private:

	/*! \brief variable to hold pointer to instance of UsbPowerConsumerConfigInfo class
	*  \details
	*/
	UsbPowerConsumerConfigInfo* mpUsbPowerConsumerConfigInfo;
};


/**************************************************************************
     class B_Suspend_State											  
**************************************************************************/
/*! \class B_Suspend_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "B_Suspend" state.
*  \details  When a host USB device goes to suspend state, and the upper
*		sotware layer (USB driver) signals to suspend, PM APP OTG 
*		state machne gets in to "B_Suspend" state and remains in this state
*		until the upper software layer signals to come out of this state.
*  \note none
*/
class B_Suspend_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	B_Suspend_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~B_Suspend_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "B_Suspend" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "B_Suspend"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "B_Suspend", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "B_Idle"
	*		- "B_Peripheral"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

	/*! \brief This function processes the request from OTG controller to resume 
	*			bus operation while in a "B_Suspend" state.  This results in a state
	*			transition to "B_Peripheral" state.
	*  \details none
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Type: boolean; Possible values:
	*		-	TRUE --> success
	*		-   FALSE --> failure
	*  \note none
	*/
	virtual boolean ResumeEvtHdlr();


	/*! \brief This function implements the virtual function defined in the base state
	*			for  "B_Suspend" state
	*  \details When acting as USB peripheral and while the bus has been put into SUSPEND
	*		state by the remote host, this function lets the OTG controller request the
	*		transceiver to set itself up for remote wakeup signaling. The transceiver
	*		configures itself for the most optimal power consumption state while still
	*		letting USB controller perform remote wakeup signaling.
	*  \param state : Indicates if remote wakup is desired or not
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Possible values:
	*		-	TRUE --> success
	*		-   FALSE --> failure
	*  \note none
	*/
	virtual boolean RemoteHostWakeupSignaling_SetupRequestHdlr(boolean setup_needed);


	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to "B_Suspend" state.
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);
protected:
private:
	/*! \brief variable to hold time delay in ms for A device disconnection detection
	*  \details
	*/
	int32 pm_app_otg_tmr__b_suspend_inter_a_dev_disconnect_detect_delay; //= 1000ms   

	/*! \brief variable to hold pointer to instance of UsbPowerConsumerConfigInfo class
	*  \details
	*/
	UsbPowerConsumerConfigInfo* mpUsbPowerConsumerConfigInfo;

};




/**************************************************************************
     class Chg_Conn_State											  
**************************************************************************/
/*! \class Chg_Conn_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "Chg_Conn" state.
*  \details  When USB charger successfuly detected, PM APP OTG state machine
*				 gets in to "Chg_Conn" state.  The routines in this class also
*				notifies the charger software layer to start charging and
*				also specify the max charging current level.
*  \note none
*/
class Chg_Conn_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Chg_Conn_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Chg_Conn_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Chg_Conn" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "Chg_Conn"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Chg_Conn", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();


	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to "Chg_Conn" state.
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);
protected:
private:
	/*! \brief variable to hold pointer to instance of UsbPowerConsumerConfigInfo class
	*  \details
	*/
	UsbPowerConsumerConfigInfo* mpUsbPowerConsumerConfigInfo;
};



/**************************************************************************
     class Unsought_Mode_State											  
**************************************************************************/
/*! \class Unsought_Mode_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "Unsought_Mode" state.
*  \details During the process of detection of USB device, if undesired
*			 situation happens, PM APP OTG gets in "" state and then it 
*			 re-attempts the detection process.
*  \note none
*/
class Unsought_Mode_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Unsought_Mode_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Unsought_Mode_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Unsought_Mode" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "Unsought_Mode"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Unsought_Mode", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();
};





#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION


/**************************************************************************
     class A_Idle_State											  
**************************************************************************/
/*! \class A_Idle_State 
*  \brief   This class contains the necessary routines for PM APP OTG 
*				state machine "A_Idle" state.
*  \details  When peripheral device is detected, PM APP OTG state machin gets
*				into "A_Idle" state.  In this situation, the PM APP OTG acts
*				as a host device.
*  \note none
*/
class A_Idle_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	A_Idle_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~A_Idle_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "A_Idle" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "A_Idle"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "A_Idle", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "B_Idle"
	*		- "A_Wait_Vrise"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();
};






/**************************************************************************
     class A_Wait_Vrise_State											  
**************************************************************************/
/*! \class A_Wait_Vrise_State 
*  \brief    This class contains the necessary routines for PM APP OTG 
*				state machine "A_Wait_Vrise" state.
*  \details  "A_Wait_Vrise" is a wait state before transitioning to "A_Wait_Bcon" state
*  \note none
*/
class A_Wait_Vrise_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	A_Wait_Vrise_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~A_Wait_Vrise_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "A_Wait_Vrise" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "A_Wait_Vrise"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: pm_app_otg_state_ptr_type
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "A_Wait_Vrise", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "A_Wait_Bcon"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();
protected:
private:

	/*! \brief This variable holds timer value to wait in "A_Wait_Vrise" state
	*  \details Set to 100 ms max value for Ta_wait_vrise
	*/
	int32 pm_app_otg_tmr__a_wait_vrise;   
};





/**************************************************************************
     class A_Wait_Bcon_State											  
**************************************************************************/
/*! \class A_Wait_Bcon_State 
*  \brief This class contains the necessary routines for PM APP OTG 
*				state machine "A_Wait_Bcon" state.
*  \details  "A_Wait_Bcon" is a wait state before transitioning to "A_Host" state
*  \note none
*/
class A_Wait_Bcon_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	A_Wait_Bcon_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~A_Wait_Bcon_State();


	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "A_Wait_Bcon" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();	

	/*! \brief This function processes interrupts / external events of interest to "A_Wait_Bcon"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "A_Wait_Bcon", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "A_Host"
	*		- "A_Vbus_Err"
	*		- "A_Wait_Vfall"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();
protected:
private:
	/*! \brief This variable holds timer value in ms to wait in "A_Wait_Bcon" state
	*  \details none
	*/
	int32 pm_app_otg_tmr__a_wait_bcon; 

	/*! \brief This variable holds timer value in ms for shadow bus acquisition
	*  \details none
	*/
	int32 pm_app_otg_tmr__shadow_ab_wait_bus_acquisition;  
};




/**************************************************************************
     class A_Host_State											  
**************************************************************************/
/*! \class A_Host_State 
*  \brief This class contains the necessary routines for PM APP OTG 
*				state machine "A_Host" state.
*  \details  When a peripheral USB device is successfully detected, 
*			PM APP OTG state macine gets in to "A_Host" state and acts 
*			as a host device.
*  \note none
*/
class A_Host_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	A_Host_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~A_Host_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "A_Host" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "A_Host"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "A_Host", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "A_Suspend"
	*		- "A_Vbus_Err"
	*		- "A_Wait_Vfall"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();


	/*! \brief This function processes the request from OTG controller to suspend the bus
	*			while in "A_Host" state.
	*  \details none
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Type: boolean; Possible values:
	*		-	TRUE --> success
	*		-   FALSE --> failure
	*  \note none
	*/
	virtual boolean SuspendEvtHdlr();

};


/**************************************************************************
     class A_Suspend_State											  
**************************************************************************/
/*! \class A_Suspend_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "A_Suspend" state.
*  \details  PM APP OTG state machine gets in to "A_Suspend" from "A_Host" state
*				when ever the both host device and peripheral device is in suspend steate.
*  \note none
*/
class A_Suspend_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	A_Suspend_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~A_Suspend_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "A_Suspend" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();


	/*! \brief This function processes interrupts / external events of interest to "A_Suspend"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "A_Suspend", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "A_Host"
	*		- "A_Vbus_Err"
	*		- "A_Wait_Vfall"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();


	/*! \brief This function processes the request from OTG controller to resume 
	*			bus operation while in a "A_Suspend" state.  This results in a state
	*			transition to "A_Host" state.
	*  \details none
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine.	
	*	-	Type: boolean; Possible values:
	*		-	TRUE --> success
	*		-   FALSE --> failure
	*  \note none
	*/
	virtual boolean ResumeEvtHdlr();

};


/**************************************************************************
     class A_Vbus_Err_State											  
**************************************************************************/
/*! \class A_Vbus_Err_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "A_Vbus_Err" state.
*  \details  While PM APP OTG is in host mode (peripheral USB device is connected to it),
*				when ever there is Vbus error, the state machine gets in to "A_Vbus_Err" state
*  \note none
*/
class A_Vbus_Err_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	A_Vbus_Err_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~A_Vbus_Err_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "A_Vbus_Err" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "A_Vbus_Err"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "A_Vbus_Err", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "A_Wait_Vfall"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();
protected:
private:
};


/**************************************************************************
     class A_Wait_Vfall_State											  
**************************************************************************/
/*! \class A_Wait_Vfall_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "A_Wait_Vfall" state.
*  \details  "A_Wait_Vfall" is a wait state before transitioning to "A_Idle" state
*  \note none
*/
class A_Wait_Vfall_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	A_Wait_Vfall_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~A_Wait_Vfall_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "A_Wait_Vfall" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "A_Wait_Vfall"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "A_Wait_Vfall", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "B_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

};




/**************************************************************************
     class Rs232Dongle_Config_State											  
**************************************************************************/
/*! \class Rs232Dongle_Config_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "Rs232Dongle_Config" state.
*  \details  When RS232 Dongle is detected, PM APP OTG state machine
*				 gets in to "Rs232Dongle_Config" state
*  \note none
*/
class Rs232Dongle_Config_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Rs232Dongle_Config_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Rs232Dongle_Config_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Rs232Dongle_Config" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "Rs232Dongle_Config"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Rs232Dongle_Config", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

};

//----------------------------------------------------------------------------------
//Carkit
//----------------------------------------------------------------------------------

/**************************************************************************
     class Carkitclient_Init_State											  
**************************************************************************/
/*! \class Carkitclient_Init_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "Carkitclient_Init" state.
*  \details  When carkit device is successfully detected, PM APP OTG 
*				statemachine gets in to "Carkitclient_Init" state
*  \note none
*/
class Carkitclient_Init_State: public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Carkitclient_Init_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Carkitclient_Init_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Carkitclient_Init" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "Carkitclient_Init"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details Carkit device is detected
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Carkitclient_Init", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "Carkitclient_Uart"
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to "Carkitclient_Init" state.
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);

	/*! \brief This function requests the OTG transceiver to change its operational
	*			mode to the one specified during a session with a carkit
	*			in "Carkitclient_Init" state.
	*  \details none
	*  \param carkit_mode_type; Carkit operational mode to be entered
	*		- Type: pm_app_otg_carkit_mode_type
	*		- Valid inputs:
	*			- PM_APP_OTG_CARKIT_MODE__UART
	*			- PM_APP_OTG_CARKIT_MODE__MONO
	*  \return The immediate return value of this function indicates if the OTG 
	*		       transceiver has accepted the request successfully.
	*		- Type: pm_err_flag_type
	*		- Possible values:
	*			- TRUE   --->  Upon completion of the processing of this request,
	*                  the OTG transceiver will call back into the client
	*                  using 'carkitclient_mode_entry_rsp_fn' specified in 
	*                  pm_app_otg_init().
	*			- FALSE  --->  Client's callback function 'carkitclient_mode_entry_rsp_fn'
	*                  will not be invoked in this case.
	*  \note none
	*/
	virtual boolean CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode_type);

};


/**************************************************************************
     class Carkitclient_Uart_State											  
**************************************************************************/
/*! \class Carkitclient_Uart_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "Carkitclient_Uart" state.
*  \details  
*  \note none
*/
class Carkitclient_Uart_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Carkitclient_Uart_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Carkitclient_Uart_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Carkitclient_Uart" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "Carkitclient_Uart"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Carkitclient_Uart", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "Carkitclient_Audio"
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to "Carkitclient_Uart" state.
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);


	/*! \brief This function requests the OTG transceiver to change its operational
	*			mode to the one specified during a session with a carkit
	*			in "Carkitclient_Uart" state.
	*  \details none
	*  \param carkit_mode_type; Carkit operational mode to be entered
	*		- Type: pm_app_otg_carkit_mode_type
	*		- Valid inputs:
	*			- PM_APP_OTG_CARKIT_MODE__UART
	*			- PM_APP_OTG_CARKIT_MODE__MONO
	*  \return The immediate return value of this function indicates if the OTG 
	*		       transceiver has accepted the request successfully.
	*		- Type: pm_err_flag_type
	*		- Possible values:
	*			- TRUE   --->  Upon completion of the processing of this request,
	*                  the OTG transceiver will call back into the client
	*                  using 'carkitclient_mode_entry_rsp_fn' specified in 
	*                  pm_app_otg_init().
	*			- FALSE  --->  Client's callback function 'carkitclient_mode_entry_rsp_fn'
	*                  will not be invoked in this case.
	*  \note none
	*/
	virtual boolean CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode);

};


/**************************************************************************
     class Carkitclient_Audio_State											  
**************************************************************************/
/*! \class Carkitclient_Audio_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "Carkitclient_Audio" state.
*  \details  
*  \note none
*/
class Carkitclient_Audio_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Carkitclient_Audio_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Carkitclient_Audio_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Carkitclient_Audio" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "Carkitclient_Audio"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Carkitclient_Audio", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "Carkitclient_Carkit_Irq"
	*		- "Carkitclient_Uart"
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to "Carkitclient_Audio" state.
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);


	/*! \brief This function requests the OTG transceiver to change its operational
	*			mode to the one specified during a session with a carkit
	*			in "Carkitclient_Audio" state.
	*  \details none
	*  \param carkit_mode_type; Carkit operational mode to be entered
	*		- Type: pm_app_otg_carkit_mode_type
	*		- Valid inputs:
	*			- PM_APP_OTG_CARKIT_MODE__UART
	*			- PM_APP_OTG_CARKIT_MODE__MONO
	*  \return The immediate return value of this function indicates if the OTG 
	*		       transceiver has accepted the request successfully.
	*		- Type: pm_err_flag_type
	*		- Possible values:
	*			- TRUE   --->  Upon completion of the processing of this request,
	*                  the OTG transceiver will call back into the client
	*                  using 'carkitclient_mode_entry_rsp_fn' specified in 
	*                  pm_app_otg_init().
	*			- FALSE  --->  Client's callback function 'carkitclient_mode_entry_rsp_fn'
	*                  will not be invoked in this case.
	*  \note none
	*/
	virtual boolean CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode);

};


/**************************************************************************
     class Carkitclient_Carkit_Irq_State											  
**************************************************************************/
/*! \class Carkitclient_Carkit_Irq_State 
*  \brief  This class contains the necessary routines for PM APP OTG 
*				state machine "Carkitclient_Carkit_Irq" state.
*  \details  
*  \note none
*/
class Carkitclient_Carkit_Irq_State:public BaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Carkitclient_Carkit_Irq_State();

	/*! \brief Destructor
	*  \details none
	*/
	virtual ~Carkitclient_Carkit_Irq_State();

	/*! \brief This function performs the necessary actions required before the state
	*			machine could transition to "Carkitclient_Carkit_Irq" state.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief This function processes interrupts / external events of interest to "Carkitclient_Carkit_Irq"
	*			state and returns the "next" state, should a state transition be necessary.
	*  \details none
	*  \param none
	*  \return Type: BaseState,  Possible values:
	*	 - Pointer to "Carkitclient_Carkit_Irq", if state transition is NOT required
	*	 - Pointer to one of the following states, if state transition is required:
	*		- "Carkitclient_Audio"
	*		- "Carkitclient_Uart"
	*		- "b_Idle"
	*  \note none
	*/
	virtual BaseState* CachedEvtHdlr();

	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to "Carkitclient_Carkit_Irq" state.
	*  \details none
	*  \param need_control: Indicates if a control over USB_VBUS line voltage is desired or not.
	*	- Type: boolean,  Valid inputs:
	*		- TRUE  --> Control desired
	*		- FALSE --> Control no longer desired
	*  \return Indicates if the request has been successfully processed by the PM OTG
	*			state machine. 
	*	-	Possible values:
	*		-	TRUE  --> Success
	*		-   FALSE --> Failure
	*  \note none
	*/
	virtual boolean UsbVbusControlRequestHdlr(boolean need_control);

	/*! \brief This function requests the OTG transceiver to change its operational
	*			mode to the one specified during a session with a carkit
	*			in "Carkitclient_Carkit_Irq" state.
	*  \details none
	*  \param carkit_mode_type; Carkit operational mode to be entered
	*		- Type: pm_app_otg_carkit_mode_type
	*		- Valid inputs:
	*			- PM_APP_OTG_CARKIT_MODE__UART
	*			- PM_APP_OTG_CARKIT_MODE__MONO
	*  \return The immediate return value of this function indicates if the OTG 
	*		       transceiver has accepted the request successfully.
	*		- Type: pm_err_flag_type
	*		- Possible values:
	*			- TRUE   --->  Upon completion of the processing of this request,
	*                  the OTG transceiver will call back into the client
	*                  using 'carkitclient_mode_entry_rsp_fn' specified in 
	*                  pm_app_otg_init().
	*			- FALSE  --->  Client's callback function 'carkitclient_mode_entry_rsp_fn'
	*                  will not be invoked in this case.
	*  \note none
	*/
	virtual boolean CarkitModeEntryReqHdlr(pm_app_otg_carkit_mode_type carkit_mode);

};

#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION


#endif //FEATURE_PMIC_USB_OTG
#endif //PMAPP_OTG_STATE_H
