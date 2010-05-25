#ifndef PMAPP_OTG_DIAG_EVENT_H
#define PMAPP_OTG_DIAG_EVENT_H

/*! \file 
*  \brief  pmapp_otg_DiagEvent.h ----- Diag Events related class declaration
*  \details This header file contains class declarations for diag event used by the USB-OTG application 
*     files developed for the Qualcomm Power Management Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_DiagEvent.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

extern "C"
{
#include "event_defs.h"
#include "event.h"
}

#ifdef FEATURE_PMIC_USB_OTG



/* =========================================================================
TYPE DEFINITIONS
========================================================================= */

/*!\brief Enum for USB OTG operational error event type 
*  \details  
*/    
typedef enum
{
	PM_APP_OTG_OPERATIONAL_ERROR__ACQUIRE_BUS_TIMEOUT = 0,
	PM_APP_OTG_OPERATIONAL_ERROR__BUS_OVERCURRENT_STATE = 1,
	PM_APP_OTG_OPERATIONAL_ERROR__INVALID
}OtgOperationalErrorEventType;



/*!\brief Enum for USB OTG device type
*  \details  
*/    
typedef enum
{
	PM_APP_OTG_DEVICE_ATTACHED__PERI_DEVICE = 0,
	PM_APP_OTG_DEVICE_ATTACHED__HOST_DEVICE,
	PM_APP_OTG_DEVICE_ATTACHED__RS232_DONGLE,
	PM_APP_OTG_DEVICE_ATTACHED__USB_WALL_CHARGER,
	PM_APP_OTG_DEVICE_ATTACHED__CARKIT,
	PM_APP_OTG_DEVICE_ATTACHED__INVALID
}OtgDeviceAttachedType;




/*!\brief Enum for PM APP OTG driver state machine state ID type
*  \details  
*/    
typedef enum
{
	PM_APP_OTG_STATE__NULL = 0,
	PM_APP_OTG_STATE__A_IDLE,
	PM_APP_OTG_STATE__A_WAIT_VRISE,
	PM_APP_OTG_STATE__A_WAIT_BCON,
	PM_APP_OTG_STATE__A_HOST,
	PM_APP_OTG_STATE__A_SUSPEND,
	PM_APP_OTG_STATE__A_VBUS_ERR,
	PM_APP_OTG_STATE__A_WAIT_VFALL,
	PM_APP_OTG_STATE__B_IDLE,
	PM_APP_OTG_STATE__B_REMOTE_DEV_DETECT_WAIT,
	PM_APP_OTG_STATE__B_PERIPHERAL,
	PM_APP_OTG_STATE__B_SUSPEND,
	PM_APP_OTG_STATE__RS232DONGLE_CONFIG,
	PM_APP_OTG_STATE__CARKITCLIENT_INIT,
	PM_APP_OTG_STATE__CARKITCLIENT_UART,
	PM_APP_OTG_STATE__CARKITCLIENT_AUDIO,
	PM_APP_OTG_STATE__CARKITCLIENT_CARKIT_IRQ,
	PM_APP_OTG_STATE__CHG_CONN,
	PM_APP_OTG_STATE__UNSOUGHT_MODE,
	PM_APP_OTG_STATE__INVALID
}StateEventIdType;



/*!\brief Typedef for PM App Otg state transition event type
*  \details  
*/    
typedef struct 
{
	StateEventIdType current_state;
	StateEventIdType next_state;
}OtgStateTransitionEventType;


/*!\brief Typedef forPM App OTG  remote a-device event type
*  \details  
*/    
typedef struct
{
	uint16      remote_a_dev_id;
	uint16      max_i_ma; //Max current (in milli-amps) that can be safely drawn from the Vbus supplied by the remote A-devc
} OtgEventRemote_a_DevInfoType;



/*===========================================================================
FORWARD DECLARATION
===========================================================================*/
class OtgDiagEvent;



/*===========================================================================
CLASS DEFINITION
===========================================================================*/

/**************************************************************************
     class OtgDiagEvent											  
**************************************************************************/
/*! \class OtgDiagEvent 
*  \brief This class contains implementation for diagonsitcs event for PM APP USB-OTG driver
*  \details The following are declared in event_defs.h.  needed to extern them to satisfy lint warning 628
*	extern void event_report (event_id_enum_type event_id);
*	extern void event_report_payload (event_id_enum_type event_id, uint8 length, void *data);
*   -  event_defs.h also defines PM_APP_OTG related Event ID enums (EventIdType)
*		- EVENT_PM_APP_OTG_INIT,				                   --> No payload
*		- EVENT_PM_APP_OTG_RESET,			                       --> No payload
*		- EVENT_PM_APP_OTG_ACQUIRE_BUS_REQ,    	                   --> 2 byte payload
*		- EVENT_PM_APP_OTG_RELINQUISH_BUS_REQ,	                   --> No payload
*		- EVENT_PM_APP_OTG_SUSPEND,			                       --> No payload
*		- EVENT_PM_APP_OTG_RESUME,			                       --> No payload
*		- EVENT_PM_APP_OTG_DEVICE_ATTACHED,                        --> 1 byTe payload
*		- EVENT_PM_APP_OTG_DEVICE_DETACHED,   		               --> No payload
*		- EVENT_PM_APP_OTG_HOST_MODE_REM_PERI_DIS,                 --> No payload
*		- EVENT_PM_APP_OTG_PERI_MODE_PREPARE_FOR_REM_HOST_WAKEUP_SIG--> No payload
*		- EVENT_PM_APP_OTG_PERI_MODE_REM_HOST_WAKEUP_SIG_DONE,		--> No payload
*		- EVENT_PM_APP_OTG_SET_REM_WAKEUP_CAPABILITY,   	        --> 1 byte payload
*		- EVENT_PM_APP_OTG_OPERATIONAL_ERROR,                       --> 1 byte Payload
*		- EVENT_PM_APP_OTG_CONFIGURE_USB_POWER_CONSUMER,		    --> No payload
*		- EVENT_PM_APP_OTG_SET_USB_POWER_CONSUMPTION_REQUIREMENT,   --> 1 byte payload
*		- EVENT_PM_APP_OTG_PERI_MODE_PROCESS_USB_POWER_LINE_CONT_REQ--> 1 byte payload
*		- EVENT_PM_APP_OTG_PERI_MODE_SET_REM_A_DEV_INFO,	        --> 3 byte payload
*		- EVENT_PM_APP_OTG_STATE_TRANSITION                         --> 2 byte payload
*  \note none
*/
class OtgDiagEvent
{
public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	OtgDiagEvent();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~OtgDiagEvent();


	/*! \brief Utility function used to create a singlton instance of OtgDiagEvent class object
	*  \details none
	*  \param none
	*  \return the Singlton instance of OtgDiagEvent class object
	*  \note none
	*/
	static OtgDiagEvent* GetInstance();


	/*! \brief Utility function used to initiate destruction of OtgDiagEvent class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	/*! \brief this function handles diag event call with no payload from PM App OTG
	*  \details none
	*  \param event_id:  diag event ID
	*		- Type event_id_enum_type
	*  \return none
	*  \note none
	*/
	void OtgEventHandle(const event_id_enum_type event_id);


	//handles diag event call with payload from PM App OTG 
	/*! \brief This function handles diag event call with payload from PM App OTG 
	*  \details none
	*  \param 1. event_id : diag event ID
	*	-	Type: EventIdType
	*  \param 2. payload_size : payload size
	*	-	Type: uint8
	*  \param 3. payload :  payload data that needs to be send to diag output
	*	-	Type: void*
	*  \return none
	*  \note none
	*/
	void OtgEventHandlePayload(const event_id_enum_type event_id, const uint8 payload_size, void* payload);



	/*! \brief This function handles diag event (EVENT_PM_APP_OTG_PERI_MODE_SET_REM_A_DEV_INFO) call from PM App OTG 
	*  \detail none
	*  \param 1. event_id : diag event ID
	*	-  Type: EventIdType
	*  \param 2. a_dev_id :  a device ID
	*	-  Type: uint16
	*  \param 3. max_i_ma:  max current (in mA) that the A device can source
	*	-  Type: uint16
	*  \return none
	*  \note none
	*/
	void OtgEventHandleRemote_a_DevInfo(const event_id_enum_type event_id, const uint16 a_dev_id, const uint16 max_i_ma);



	/*! \brief This function handles state transition diag event (EVENT_PM_APP_OTG_STATE_TRANSITION) call from PM App OTG
	*  \details none
	*  \param 1. event_id : Diag event ID
	*  -	Type: EventIdType
	*  \param 2. curr_state_id : current state
	*  -	Type: StateEventIdType
	*  \param 3. next_state_id : next state
	*  -	Type: StateEventIdType
	*  \return none
	*  \note none
	*/
	void OtgEventHandleStateTransition(const event_id_enum_type event_id, const StateEventIdType curr_state_id, const StateEventIdType next_state_id);

protected:

private:

	/*! \brief This static variable is used to hold the singlton instance of class OtgDiagEvent object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static OtgDiagEvent* mpOtgDiagEvent_Instance;

};



#endif //FEATURE_PMIC_USB_OTG
#endif // PMAPP_OTG_DIAG_EVENT_H      

