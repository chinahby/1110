#ifndef PMAPP_OTG_UTIL_H
#define PMAPP_OTG_UTIL_H

/*! \file 
*  \brief  pmapp_otg_Util.h  ------ PM APP OTG state manager related utility class declaration
*  \details This header file contains various utility classes and typedef declarations 
*     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_Util.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/08   aab      Made changes to properly initialize timer object
08/04/08   aab      Commented out the use of Vectors.  It was added to support possible future expansion
07/23/08   aab      Made changes to resolve compiler warnings
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
//#include <vector>  /* for vector class library */

#include "pmapp_otg_VbusStateMonitor.h"
#include "pmapp_otg_DpStateMonitor.h"
#include "pmapp_otg_DmStateMonitor.h"
#include "pmapp_otg_IdStateMonitor.h"
#include "pmapp_otg_UsbPowerConsumerConfigInfo.h"

#ifdef FEATURE_PMIC_USB_OTG

//===========================================================================
//                              FORWARD DECLARATION
//===========================================================================
class Evt;
class EvtListener;


/* =========================================================================
TYPE DEFINITIONS
========================================================================= */

//#if defined(__GNUC__) || (__ARMCC_VERSION/10000 > 12)
//typedef std::vector <EvtListener*> EventListenerVectorType;
//#else
//typedef vector <EvtListener*, allocator<EvtListener*> > EventListenerVectorType;
//#endif //GCC or RVCT 
//typedef std::vector<EvtListener*>     EventListenerVectorType;  //added when using unit test in VS 2005 compiler



/*===========================================================================
CLASS DEFINITION
===========================================================================*/

/**************************************************************************
     class EvtUtil											  
**************************************************************************/
/*! \class EvtUtil 
*  \brief  This class holds all needed event object definations needed
*			by PM APP OTG state machine
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class EvtUtil
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	EvtUtil();

	/*! \brief Destructor
	*  \details none
	*/
	~EvtUtil();

	/*! \brief This member function returns instance of EvtUtil class
	*  \details EvtUtil is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of EvtUtil class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static EvtUtil* GetInstance();

	/*! \brief Utility function used to initiate destruction of EvtUtil class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();

	//OTG A-device event flags
	Evt* a_bus_drop_evt;		/*!< defines a_bus_drop Evt object; used in OTG A-device operation*/
	Evt* a_bus_req_evt;			/*!< defines a_bus_req  Evt object; used in OTG A-device operation*/
	Evt* a_bus_reset_evt;		/*!< defines a_bus_reset Evt object; used in OTG A-device operation*/
	Evt* a_bus_K_SE0_evt;		/*!< defines a_bus_K_SE0 Evt object; used in OTG A-device operation*/
	Evt* a_bus_resume_evt;		/*!< defines a_bus_resume Evt object; used in OTG A-device operation*/
	Evt* a_bus_suspend_evt;		/*!< defines a_bus_suspend Evt object; used in OTG A-device operation*/

	Evt* a_sess_vld_evt;		/*!< defines a_sess_vld Evt object; used in OTG A-device operation*/
	Evt* a_suspend_req_evt;		/*!< defines a_suspend_req Evt object; used in OTG A-device operation*/
	Evt* a_srp_det_evt;			/*!< defines a_srp_det Evt object; used in OTG A-device operation*/
	Evt* a_vbus_vld_evt;		/*!< defines a_vbus_vld Evt object; used in OTG A-device operation*/
	Evt* a_wait_vrise_tmout_evt;/*!< defines a_wait_vrise_tmout Evt object; used in OTG A-device operation*/
	Evt* a_wait_bcon_tmout_evt;	/*!< defines a_wait_bcon_tmout Evt object; used in OTG A-device operation*/
	Evt* b_wait_vfall_tmout_evt;/*!< defines b_wait_vfall_tmout Evt object; used in OTG A-device operation*/


	//OTG B-device event flags
	Evt* b_bus_req_evt;			/*!< defines b_bus_req Evt object; used in OTG B-device operation*/
	Evt* b_bus_resume_evt;		/*!< defines b_bus_resume Evt object; used in OTG B-device operation*/
	Evt* b_conn_evt;			/*!< defines b_conn Evt object; used in OTG B-device operation*/
	Evt* b_sess_vld_evt;		/*!< defines b_sess_vld Evt object; used in OTG B-device operation*/
	Evt* b_suspend_inter_a_dev_disconnect_delay_tmout_evt;/*!< defines b_suspend_inter_a_dev_disconnect_delay_tmout Evt object; used in OTG B-device operation*/
	Evt* dm_stabilization_tmout_evt;		/*!< defines dm_stabilization_tmout Evt object; used in OTG B-device operation*/
	Evt* dp_low_detection_warmup_tmout_evt;	/*!< defines dp_low_detection_warmup_tmout Evt object; used in OTG B-device operation*/


	//OTG A/B-device common event flags
	Evt* sm_init_evt;			/*!< defines sm_init Evt object; used in OTG A/B-device common operation*/
	Evt* custom_hw_ready_evt;	/*!< defines custom_hw_ready Evt object; used in OTG A/B-device common operation*/
	Evt* sm_reset_evt;			/*!< defines sm_reset Evt object; used in OTG A/B-device common operation*/
	Evt* id_evt;				/*!< defines id Evt object; used in OTG A/B-device common operation*/
	Evt* dp_evt;				/*!< defines dp Evt object; used in OTG A/B-device common operation*/
	Evt* dm_evt;				/*!< defines dm Evt object; used in OTG A/B-device common operation*/
	Evt* remote_wakeup_evt;		/*!< defines remote_wakeup Evt object; used in OTG A/B-device common operation*/


	//Events for OTG Carkit Client mode operation
	Evt* cr_dp_con_det_evt;				/*!< defines cr_dp_con_det Evt object; used in OTG Carkit Client mode operation*/
	Evt* cr_dp_con_tmout_evt;			/*!< defines cr_dp_con_tmout Evt object; used in OTG Carkit Client mode operation*/
	Evt* cr_client_aud_bias_tmout_evt;	/*!< defines cr_client_aud_bias_tmout Evt object; used in OTG Carkit Client mode operation*/
	Evt* cr_client_mono_ack_tmout_evt;	/*!< defines cr_client_mono_ack_tmout Evt object; used in OTG Carkit Client mode operation*/
	Evt* cr_int_det_evt;				/*!< defines cr_int_det Evt object; used in OTG Carkit Client mode operation*/
	Evt* cr_client_mode_entry_req_evt;	/*!< defines cr_client_mode_entry_req Evt object; used in OTG Carkit Client mode operation*/


	//Other events that may be used in future
	//Evt* pm_app_otg_evt__a_conn;
	//Evt* a_set_b_hnp_en_evt;
	//Evt* b_bus_suspend_evt;
	//Evt* b_se0_srp_evt;
	//Evt* b_sess_end_evt;
	//Evt* b_srp_done_evt;
	//Evt* b_hnp_enable_evt;
	//Evt* b_dp_con_tmout_evt;
	//Evt* cr_disc_det_tmout_evt;
	//Evt* cr_aud_det_tmout_evt;

protected:
private:
	/*! \brief This static variable is used to hold the singlton instance of EvtUtil class
	*   \details none
	*/
	static EvtUtil* mpEvtUtil_Instance;
	
};





/**************************************************************************
     class Util											  
**************************************************************************/
/*! \class Util 
*  \brief  This class holds all needed flags and global variable definations needed
*			by PM APP OTG state machine
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class Util
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Util();

	/*! \brief Destructor
	*  \details none
	*/
	~Util();

	/*! \brief This member function returns instance of Util class
	*  \details Util is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of Util class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static Util* GetInstance();

	/*! \brief Utility function used to initiate destruction of util class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();

	/*! \brief This function processes the request for control over USB_VBUS as it applies
	*			to any valid state in which this request is considered valid.
	*  \details none
	*  \param need_control : Indicates if a control over USB_VBUS line voltage is desired or not.
	*		- Type: boolean; 
	*		- Valid inputs:
	*			- TRUE  --> Control desired
	*			- FALSE --> Control no longer desired
	*  \return Type: boolean; Indicates if the request has been successfully
	*			processed by the PM OTG state machine.
	*		- Possible values:
	*		- TRUE   --->  Success
	*		- FALSE  --->  Failure
	*  \note none
	*/
	boolean UsbVbusControlRequestHdlr(boolean need_control);

	/*! \brief Determines the remote peripheral USB device speed
	*  \details Based on D+/D- USB line real time status, it detects if the USb device supports 
	*			dvice is Full/Low speed USB
	*  \param none
	*  \return pm_otg_usb_speed_type :  Full/Low speed USB device
	*  \note none
	*/
	pm_otg_usb_speed_type DetermineRemotePeripheralSpeed(void);

	/*! \brief This function processes any bus acquisition request that was put on hold
	*			while a prior bus relinquish operation was in progress
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void ProcessPendingBusAcquisitionRequest(void);

	/*! \brief This function is a wrapper function to call the regestered upper layer sleep call back function.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void AssertSleep(boolean assert_sleep);

	/*! \brief Checks if any USB device (host or peripheral) is connected to PMIC USB port
	*  \details 
	*  \param none
	*  \return Type boolean: 
	*		- Possible return values:
	*			- TRUE --> if no USB device is connected
	*			- FALSE --> if USB device is connected
	*  \note none
	*/
	boolean IsBusIdle(void);    // Is anything connected to PMIC port?

	/*! \brief this function checks if OTG controller acquire USB bus
	*  \details none
	*  \param none
	*  \return Type boolean: 
	*		- Possible return values:
	*			- TRUE  --> if USB bus is acquired
	*			- FALSE --> if USB bus ins not acquired yet
	*  \note none
	*/
	boolean IsBusAcquired(void);// Did OTG controller acquire the bus?


public:
	//timers 
	int32 ab_wait_bus_acquisition_tmr; /*!< Wait time in ms for transition to/from A/B mode of operation*/

	// Info about pending bus acquisition request
	int8 pending_bus_acquisition_tmo_secs_tmr;	/*!< Wait time in ms for pending bus acquisition*/
	pm_otg_usb_speed_type usb_speed;			/*!< USB speed (Full/Low) when acting as a host*/

	//Internal flags used by the state machines besides the above events ... 
	boolean active_controller_session_flag;				/*!< flag for active controller session */
	boolean bus_acquisition_pending_flag;				/*!< flag for bus acuisition pending*/
	boolean bus_idle_wait_reqd_flag;					/*!< flag for bus idle wait required*/
	boolean bus_idle_or_bus_acquisition_wait_reqd_flag;	/*!< flag for bus idle or bus acquisition wait required*/
	boolean a_host_resumption_flag;						/*!< flag for A_Host state resumption*/
	boolean b_peripheral_resumption_flag;				/*!< flag for B_Peripheral state resumption*/
	boolean remote_wakeup_capability_enabled_flag;		/*!< flag for remote wakeup capability enabled*/
	boolean usb_power_consumer_interested_flag;			/*!< flag for USB power consumer interested*/
	boolean resistive_id_flag;							/*!< flag for resistive USB Id line state*/
	boolean need_to_enable_cr_int_det_flag;				/*!< flag for a need to enable carkit initialization detection*/

	//Keeps track of the current operational mode
	pm_app_otg_operational_mode_type current_operational_mode;	/*!< current APP OTG operational mode*/

	// Holds info (like callbacks for notifying about important events) about the last programmed OTG controller
	pm_app_otg_ctrlr_info_type  ctrlr_info_shadow;  /*!< Used as a temporary storage for PM APP OTG initialization configuration*/
	pm_app_otg_ctrlr_info_type  ctrlr_info_curr;	/*!< Used to store for PM APP OTG initialization configuration*/
	pm_app_otg_ctrlr_info_type* active_ctrlr_info_ptr; /*!< Used to store for PM APP OTG initialization configuration*/

	//Info about the remote A-dev that this phone is connected to 
	pm_app_otg_remote_a_dev_info_type remote_a_dev_info;	/*!< sotres A-Device type and the max current that can safely drawn*/
	pm_app_otg_remote_a_dev_info_type saved_before_suspend_remote_host_info; /*!< sotres A-Device type and the max current that can safely drawn*/


    //Carkit client timers 
    int32 cr_dp_con_tmr;			/*!<  Carkit D+ Cnnect timer; 300 ms max value for Tcr_dp_con */
    int32 cr_disc_det_tmr;			/*!<  Carkit disconnect timer; 150 ms max value for Tcr_disc_det */
    int32 cr_client_aud_bias_tmr;   /*!<  Carkit client audio bias timer; 10 ms max value for Tph_aud_bias */
    int32 cr_client_mono_ack_tmr;   /*!<  Carkit client mono ack timer; 10 ms max value for Tph_mono_ack */

	//Carkit client mode variables
	pm_app_otg_carkit_mode_type current_carkit_mode;		/*< current carkit operational mode */
	pm_app_otg_carkit_mode_type current_carkit_req_mode;	/*< current carkit mode requested by the controller  */

protected:

private:
	/*! \brief This static variable is used to hold the singlton instance of Util class
	*   \details none
	*/
	static Util* mpUtil_Instance;


	/*! \brief This variable is used to hold pointer to EvtUtil object instance
	*  \details none
	*/
	EvtUtil* mpEvtUtil;

};



/**************************************************************************
     class PowerConsumer											  
**************************************************************************/
/*! \class PowerConsumer 
*  \brief  This class have routines needed to manage USB Power Consumer functioanlity
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class PowerConsumer
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	PowerConsumer();

	/*! \brief Destructor
	*  \details none
	*/
	~PowerConsumer();

	/*! \brief This member function returns instance of PowerConsumer class
	*  \details PowerConsumer is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of PowerConsumer class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static PowerConsumer* GetInstance();

	/*! \brief Utility function used to initiate destruction of PowerConsumer class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	/*! \brief This function is used to process USB power cosumer releated events
	*  \details This function manages Connection/Disconnection and open/close sink to
	*			power consumer device, depending on the USB host/wall charger connection state.
    *  \param  local_power_consumer_interest_ptr; Type: boolean
	*  \param  remote_a_dev_info_ptr; Type: pm_app_otg_remote_a_dev_info_type
	*  \return none
	*  \note 	Charger Callback Functions: (See where this call back functions are called on the state diagram)
	*			cb13: SourceConnected
	*			cb14: SourceDisconnected
	*			cb15: SinkOpenNotification
	*			cb16: SinkCloseNotification
	*
	*/
	void UsbPowerConsumerEvtHdlr(\
		const boolean                            *local_power_consumer_interest_ptr,\
		const pm_app_otg_remote_a_dev_info_type  *remote_a_dev_info_ptr \
		);

private:
	/*! \brief This static variable is used to hold the singlton instance of PowerConsumer class
	*   \details none
	*/
	static PowerConsumer* mpPowerConsumer_Instance;

	/*! \brief This variable is used to hold pointer to UsbPowerConsumerConfigInfo object instance
	*  \details none
	*/
	UsbPowerConsumerConfigInfo* mpUsbPowerConsumerConfigInfo;

	/*! \brief This variable is used to hold pointer to Util object instance
	*  \details none
	*/
	Util* mpUtil;
};



/**************************************************************************
     class EvtListener											  
**************************************************************************/
/*! \class EvtListener 
*  \brief  This class is used as a base class to other classes that needs to listen to event.
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class EvtListener
{
public:
	/*! \brief pure virtual function 
	*  \details must be implemented by the all childern classes
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void ProcessCachedEvt(void)=0;

	/*! \brief pure virtual function 
	*  \details must be implemented by the all childern classes
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void ProcessStateChangeEvt(void)=0;
};




/**************************************************************************
     class EvtGen											  
**************************************************************************/
/*! \class EvtGen 
*  \brief  This class have routines to handle event generator functionality
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class EvtGen
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	EvtGen();

	/*! \brief Destructor
	*  \details none
	*/
	~EvtGen();

	/*! \brief This member function returns instance of EvtGen class
	*  \details EvtGen is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of EvtGen class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static EvtGen* GetInstance();

	/*! \brief Utility function used to initiate destruction of util class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();

	/*! \brief Register class object that is is interesed to listen to an event
	*  \details none
	*  \param Type: EvtListener*
	*  \return none
	*  \note none
	*/
	void AddListener(EvtListener* );

	/*! \brief Removes class object that was registered to listen to an event
	*  \details none
	*  \param Type: EvtListener*
	*  \return none
	*  \note none
	*/
	void RemoveListener(EvtListener*);

	/*! \brief This function notifies all registered class objects when ever there is an event
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void Notify();

	/*! \brief This function notifies all registered class objects when ever there is state change event
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void NotifyStateChage();

protected:
private:
	//std::vector <EvtListener*> mListenerVector;  //vector used to store listener objects

	/*! \brief This vector variable is used to hold a list of event listener objects
	*			registered to listen for any event
	*   \details none
	*/
//	EventListenerVectorType mListenerVector;

	/*! \brief This static variable is used to hold the singlton instance of EvtGen class
	*   \details none
	*/
	static EvtGen* mpEvtGen_Instance;
};






/**************************************************************************
     class Evt											  
**************************************************************************/
/*! \class Evt 
*  \brief  This class have routines that defines a generic Evt object
*			needed by PM APP OTG state machine
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class Evt
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	Evt();

	/*! \brief Destructor
	*  \details none
	*/
	~Evt();

	/*! \brief This function Reset the event as un-available
	*  \details This means the event is neither true or false
	*/
	inline void Reset()   { mInfoAvailable = FALSE;}

	/*! \brief This function setst the event to true and make it available
	*  \details none
	*/
	inline void SetHigh()
	{
		mInfoAvailable = TRUE;
		if(mHappened == FALSE)
		{
			mHappened = TRUE; 
			mpEvtGen->Notify();
		}
	}

	/*! \brief This function setst the event to false and make it available
	*  \details none
	*/
	inline void SetLow() 
	{
		mInfoAvailable = TRUE;
		if(mHappened == TRUE)
		{
			mHappened = FALSE; 
			mpEvtGen->Notify();
		}
	}

	/*! \brief This function returns true if the event is set to true; 
	*			It returns false if the event is set to false
	*  \details In any case, if the event is not available, it will return false.
	*/
	inline boolean IsHigh() { return (mInfoAvailable && mHappened); }

	/*! \brief This function returns true if the event is set to false; 
	*			It returns false if the event is set to true
	*  \details In any case, if the event is not available, it will return false.
	*/
	inline boolean IsLow()  { return (mInfoAvailable && !mHappened); }

	/*! \brief This function returns event info availability
	*  \details It returns true if the event info is avaialable
	*			It returns false if the event info is not available
	*/
	inline boolean IsSet()  { return mInfoAvailable; }

protected:
	/*! \brief This variable is used as a flag to indicate event info availablility
	*  \details do we have valid information available about this evt? 
	*/
	boolean	mInfoAvailable; 

	/*! \brief This variable holds the state of the event
	*  \details none
	*/
	boolean	mHappened;     

	/*! \brief This variable is used to hold pointer to EvtGen object instance
	*  \details none
	*/
	EvtGen* mpEvtGen;
private:

};



/**************************************************************************
     class UsbBusListener											  
**************************************************************************/
/*! \class UsbBusListener 
*  \brief  This class have call back function to handle various USB line debounced state
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class UsbBusListener
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	UsbBusListener();

	/*! \brief Destructor
	*  \details none
	*/
	~UsbBusListener();

	/*! \brief This member function returns instance of UsbBusListener class
	*  \details UsbBusListener is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of UsbBusListener class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static UsbBusListener* GetInstance();

	/*! \brief Utility function used to initiate destruction of util class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	/*! \brief This is a callback function registered with the VBUS line voltage measurement state
	*			machine so that the OTG application state machine could get notified about
	*			voltage transitions on the VBUS line.
	*  \details This function then generates events based on the voltage condition of the VBUS
	*			and requests OTG application state machine to process them.
	*  \param info : VBUS voltage condition
	*	- Type: OtgVbusInfoType
	*	- Valid inputs:
	*	   - OTG_VBUS_SM_INFO__VBUS_VALID
	*	   - OTG_VBUS_SM_INFO__VBUS_INVALID
	*	   - OTG_VBUS_SM_INFO__VBUS_SESSION_VALID
	*	   - OTG_VBUS_SM_INFO__VBUS_SESSION_INVALID
	*  \param pUsbBusListenerObj : pointer to class object where VbusListener() is defined
	*	- Type: void*
	*  \return none
	*  \note none
	*/
	static void VbusListener(OtgVbusSmInfoType info, void* pUsbBusListenerObj);


	/*! \brief This is a callback function registered with the ID line voltage measurement state
	*			machine so that the OTG application state machine could get notified about
	*			voltage transitions on the ID line.
	*  \details This function then generates events based on the voltage condition of the ID
	*			and requests OTG application state machine to process them.
	*  \param info : An enumeration containing ID line voltage info
	*	- Type: OtgVbusInfoType
	*	- Valid inputs:
	*	   - OTG_ID_SM_INFO__FLT  (ID line is floating high)
	*	   - OTG_ID_SM_INFO__GND  (ID line is grounded)
	*	   - OTG_ID_SM_INFO__RES  (ID line is in between floating high and grounded)
	*  \param pUsbIdListenerObj : pointer to class object where IdListener() is defined
	*	- Type: void*
	*  \return none
	*  \note none
	*/
	static void IdListener(OtgIdSmInfoType info, void* pUsbIdListenerObj);


	/*! \brief This is a callback function registered with the D- line voltage measurement state
	*			machine so that the OTG application state machine could get notified about
	*			voltage transitions on the D- line.
	*  \details This function then generates events based on the voltage condition of the D-
	*			and requests OTG application state machine to process them.
	*  \param info : An enumeration containing D- line voltage info
	*	- Type: OtgDpSmInfoType
	*	- Valid inputs:
	*	   - OTG_DM_SM_INFO__LOW  (D- low interrupt )
	*	   - OTG_DM_SM_INFO__HIGH (D- high interrupt)
	*  \param pUsbDmListenerObj : pointer to class object where DmListener() is defined
	*	- Type: void*
	*  \return none
	*  \note none
	*/
	static void DmListener(OtgDmSmInfoType info, void* pUsbDmListenerObj);


	/*! \brief This is a callback function registered with the D+ line voltage measurement state
	*			machine so that the OTG application state machine could get notified about
	*			voltage transitions on the D+ line.
	*  \details This function then generates events based on the voltage condition of the D+
	*			and requests OTG application state machine to process them.
	*  \param info : An enumeration containing D+ line voltage info
	*	- Type: OtgDpSmInfoType
	*	- Valid inputs:
	*	   - OTG_DP_SM_INFO__LOW (D+ low interrupt)
	*	   - OTG_DP_SM_INFO__HIGH (D+ high interrupt)
	*	   - OTG_DP_SM_INFO__BDIS_ACON (bdis_acon interrupt)
	*	   - OTG_DP_SM_INFO__CR_INT (carkit interrupt)
	*  \param pUsbDpListenerObj : pointer to class object where DpListener() is defined
	*	- Type: void*
	*  \return none
	*  \note none
	*/
	static void DpListener(OtgDpSmInfoType info, void* pUsbDpListenerObj);

protected:

private:
	/*! \brief This static variable is used to hold the singlton instance of UsbBusListener class
	*   \details none
	*/
	static UsbBusListener* mpUsbBusListener_Instance;

	/*! \brief This variable is used to hold pointer to EvtUtil object instance
	*  \details none
	*/
	EvtUtil* mpEvtUtil;

	/*! \brief This variable is used to hold pointer to Util object instance
	*  \details none
	*/
	Util* mpUtil;

};



/**************************************************************************
     class GenericEvtDetectIsr											  
**************************************************************************/
/*! \class GenericEvtDetectIsr 
*  \brief  This class have routines needed for Generic ISR event detection functioanlity
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class GenericEvtDetectIsr
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	GenericEvtDetectIsr();

	/*! \brief Destructor
	*  \details none
	*/
	~GenericEvtDetectIsr();

	/*! \brief This member function returns instance of GenericEvtDetectIsr class
	*  \details GenericEvtDetectIsr is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of Util class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static GenericEvtDetectIsr* GetInstance();

	/*! \brief Utility function used to initiate destruction of GenericEvtDetectIsr class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	// A generic ISR for handling various types of IRQs
	/*! \brief This function is used as a callback function for the Generic event detecion
	*  \details When this function is called it sets the registered event to high state
	*  \param pIsrObj : class object where GenericEvtDetectionIsrCb() is defined
	*			-  Type: GenericEvtDetectIsr
	*  \return none
	*  \note Need to be static so that we can pass it as a function pointer
	*/
	static void GenericEvtDetectionIsrCb(void* pIsrObj);

	/*! \brief This function is used to enable Generic event detecion for a particular IRQ
	*  \details none
	*  \param irq: The IRQ to be detected;  Type: pm_irq_hdl_type
	*  \param evt_ptr: The Evt object pointer to be updated upon IRQ evt detection; Type: pm_irq_hdl_type
	*  \return none
	*  \note Need to be static so that we can pass it as a function pointer
	*/
	void GenericEvtDetectionIsr_Enable(pm_irq_hdl_type irq, Evt *evt_ptr);

	void GenericEvtDetectionIsr_Disable(void);

protected:
private:
	/*! \brief This static variable is used to hold the singlton instance of GenericEvtDetectIsr class
	*   \details none
	*/
	static GenericEvtDetectIsr* mpGenericEvtDetectIsr_Instance;

	//boolean pm_app_otg_flag__GenericEvtDetectionIsr_enabled;
	/*! \brief This variable is used as aflag to indicate the state(enable/disable)
	*			of ISR generic event detection
	*  \details none
	*/
	boolean mGenericEvtDetectionIsr_enabled_flag;

	//pm_irq_hdl_type pm_app_otg_generic_evt_detection_irq;
	/*! \brief This variable is used to hold a particular IRQ whose state is to be detected
	*  \details none
	*/	
	pm_irq_hdl_type mGeneric_evt_detection_irq;

	/*! \brief This variable is used to hold pointer to the Evt object whose state needs
	*			 to be changed when the generic event detection happens.
	*  \details none
	*/
	Evt* mpGeneric_evt;

	/*! \brief This variable is used to hold pointer to EvtUtil object instance
	*  \details none
	*/
	EvtUtil* mpEvtUtil;
};



/**************************************************************************
     class OtgTimer											  
**************************************************************************/
/*! \class OtgTimer 
*  \brief  This class have routines needed to manage PM APP OTG Timer
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class OtgTimer
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	OtgTimer();

	/*! \brief Destructor
	*  \details none
	*/
	~OtgTimer();

	/*! \brief This member function returns instance of OtgTimer class
	*  \details OtgTimer is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of OtgTimer class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static OtgTimer* GetInstance();

	/*! \brief Utility function used to initiate destruction of OtgTimer class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();

    /*!\brief Utility function used to initialize timer
	*  \details none
	*  \param none
	*  \return none
	*  \note none
    */
    void InitTimer();

	/*! \brief This function is used to start APP OTG related timers
	*  \details Insures that there is only one timer running at a time
	*  \param tmo_delay :  length of time in ms; type : int32
	*  \param tmo_flag_ptr : Event object that need to be updated when the timer expires
	*  \return none
	*  \note none
	*/
	void StartTimer(int32 tmo_delay, Evt *tmo_flag_ptr);


	/*! \brief This function is used to stop APP OTG related timers
	*  \details none
	*  \param tmo_evt_ptr : Event object that need to be updated when the timer is stopped
	*  \return none
	*  \note none
	*/
	void StopTimer(const Evt* tmo_evt_ptr);

	/*! \brief This function is used as a callback function that is called when the timer expires
	*  \details It updates the registered event and call up on APP OTG state manager to process the new event
	*  \param time_ms :  length of time in ms; type : int32
	*  \param token : class object pointer where TimerCb() is defined. Type: timer_cb_data_type
	*  \return none
	*  \note none
	*/
	static void TimerCb(int32 time_ms, timer_cb_data_type token);


	/*! \brief This variable is used to hold pointer to the Evt object whose state needs
	*			 to be changed when the timer event happens.
	*  \details none
	*/
	Evt* mpCurrent_tmo_evt;  

	/*! \brief This variable is used to hold timer_type
	*  \details none
	*/
	timer_type	mPmOtgTimer;

protected:
private:
	/*! \brief This static variable is used to hold the singlton instance of OtgTimer class
	*   \details none
	*/
	static OtgTimer* mpOtgTimer_Instance; 

	/*! \brief This variable is used to hold pointer to EvtUtil object instance
	*  \details none
	*/
	EvtUtil *mpEvtUtil;

	/*! \brief This variable is used to hold pointer to Util object instance
	*  \details none
	*/
	Util* mpUtil;

	/*! \brief This variable is used to hold delay value
	*  \details none
	*/
    int32 mDelay;

	//static pm_app_timer_sess_id_type  pm_app_otg_timer_sess_id;
	//pm_app_timer_sess_id_type  pm_app_otg_timer_sess_id;
};



/**************************************************************************
     class EvtDebugger											  
**************************************************************************/
/*! \class EvtDebugger 
*  \brief  This class have routines needed for generating event debug messages
*			Debug messages can be sent to QXDM
*  \details There should only be one instance of this calss.
*  \note   GetInstance() should be called to get instance of this class
*/
class EvtDebugger:public EvtListener
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	EvtDebugger();

	/*! \brief Destructor
	*  \details none
	*/
	~EvtDebugger();

	/*! \brief This member function returns instance of EvtDebugger class
	*  \details EvtDebugger is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of EvtDebugger class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static EvtDebugger* GetInstance();

	/*! \brief Utility function used to initiate destruction of util class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	/*! \brief  This function gets called whenever there is an event 
	*			and it then notifies the user
	*  \details used for debugging purpose
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void ProcessCachedEvt(void);

	/*! \brief  This function gets called whenever there is OTG state change 
	*			and it then notifies the user
	*  \details used for debugging purpose
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void ProcessStateChangeEvt();

private:
	/*! \brief This variable is used to hold pointer to EvtGen object instance
	*  \details none
	*/
	EvtGen* mpEvtGen;

	/*! \brief This static variable is used to hold the singlton instance of EvtDebugger class
	*   \details none
	*/
	static EvtDebugger* mpEvtDebugger_Instance;

	//add ptr to QXDM object
};



#endif  //FEATURE_PMIC_USB_OTG
#endif	//PMAPP_OTG_UTIL_H

