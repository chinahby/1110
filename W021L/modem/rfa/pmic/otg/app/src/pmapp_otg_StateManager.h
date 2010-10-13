#ifndef PMAPP_OTG_STATEMANAGER_H
#define PMAPP_OTG_STATEMANAGER_H

/*! \file 
*  \n
*  \brief  pmapp_otg_StateManager.h  ----- PM APP OTG state manager related class declaration
*  \details This header file contains class declarations of USB OTG states and state Manager
*     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_StateManager.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/08   aab      Added FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pmapp_otg_State.h"

#ifdef FEATURE_PMIC_USB_OTG

/*===========================================================================
FORWARD DECLARATION
===========================================================================*/
class StateManager;
class StateFactory;
class BaseState;
class StateChangeEvt;


/*===========================================================================
CLASS DEFINITION
===========================================================================*/

/**************************************************************************
     class StateManager											  
**************************************************************************/
/*! \class StateManager 
*  \brief  This class holds all the necessary functions used to mange 
*			PM APP OTG state machine.
*  \details There should only be one instance of this calss.
*		Instance of this class can be acquired through GetInstance().
*  \note   
*/
class StateManager:public EvtListener
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	StateManager();

	/*! \brief Destructor
	*  \details none
	*/
	~StateManager();

	/*! \brief This member function returns instance of StateManager class
	*  \details StateManager is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of StateManager class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static StateManager* GetInstance();

	/*! \brief Utility function used to initiate destruction of StateManager class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();

	/*! \brief This function is used to initialize StateManager class
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void InitStateManager();


	/*! \brief This function services any interrupts / external inputs to the 
	*			USB-OTG app state machine and causes state transition(s) as 
	*			required by the individual states.
	*  \details The state-entry handler function Entry() is called when the 
	*			state machine enters a new state after which its cached-event
	*			handler function CachedEvtHdlr() is called to process events
    *			accumulated till that point. This process is continued as long
	*			as state transitions are caused by the cached-event handling in
	*			the individual states.
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void ProcessCachedEvt(); 


	/*! \brief This function initializes and sets up the IRQ engine for enabling and
    *			handling USB-OTG related interrupts.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void InitEvtGenEngine(void);

	/*! \brief This function resets USB-OTG event generation mechanism.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void ResetEvtGenEngine(void);
   
	/*! \brief  Callback function for getting notified about completion of
	*			custom hardware initialization
	*  \details This is a callback function to know when the custom hardware
	*			is done initializing itself properly. Until then we will defer
	*			initialization of our OTG software state machine.
	*  \param pObj, type: void* ; It holds a pointer to StateManager class instance
	*  \return none
	*  \note none
	*/
	static void CustomHwInitDoneCb(void* pObj);

	/*! \brief  This function performs usb transciver initializations common to both 
	*			dual role devices - A and B
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void PerformCommonDualRoleDevInit(void);

	/*! \brief  This function gets called whenever there is OTG state change 
	*			and it then notifies the user
	*  \details used for debugging purpose
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void ProcessStateChangeEvt(){}; //do nothing for now

	/*! \brief  This function is an accessor function to get the current APP OTG state
	*  \details used for debugging/Unit Test purpose
	*  \param none
	*  \return none
	*  \note none
	*/
	BaseState* GetCurrState(){return mpCurrState;}

protected:
	/*! \brief  This variable holds pointer to the current APP OTG state
	*  \details none
	*/
	static BaseState* mpCurrState;  

	/*! \brief  This variable holds pointer to the previous APP OTG state
	*  \details none
	*/
	static BaseState* mpPrevState;  

private:
	/*! \brief This static variable is used to hold the singlton instance of StateManager class
	*   \details none
	*/
	static StateManager* mpStateManager_Instance;

	/*! \brief  This variable holds pointer to UsbBusListener class instance
	*  \details none
	*/
	UsbBusListener* mpUsbBusListener;

	/*! \brief  This variable holds pointer to EvtGen class instance
	*  \details none
	*/
	EvtGen* mpEvtGen;

	/*! \brief  This variable holds pointer to StateChangeEvt class instance
	*  \details none
	*/
	StateChangeEvt* mpStateChangeEvt;

	/*! \brief  This variable holds pointer to VbusValidStateMonitor class instance
	*  \details none
	*/
	VbusValidStateMonitor* mpVbusValidStateMonitor;

	/*! \brief  This variable holds pointer to SessionValidStateMonitor class instance
	*  \details none
	*/
	SessionValidStateMonitor* mpSessionValidStateMonitor;

	/*! \brief  This variable holds pointer to DpStateMonitor class instance
	*  \details none
	*/
	DpStateMonitor* mpDpStateMonitor;

	/*! \brief  This variable holds pointer to DmStateMonitor class instance
	*  \details none
	*/
	DmStateMonitor* mpDmStateMonitor;

	/*! \brief  This variable holds pointer to IdFltStateMonitor class instance
	*  \details none
	*/
	IdFltStateMonitor* mpIdFltStateMonitor;

	/*! \brief  This variable holds pointer to IdGndStateMonitor class instance
	*  \details none
	*/
	IdGndStateMonitor* mpIdGndStateMonitor;

	/*! \brief  This variable holds pointer to IdResStateMonitor class instance
	*  \details none
	*/
	IdResStateMonitor* mpIdResStateMonitor;
};




/**************************************************************************
     class StateFactory											  
**************************************************************************/
/*! \class StateFactory 
*  \brief  This class is used to inforce a single instance of each 
*      PM APP OTG states
*  \details The state objects can be instansiated as needed by calling
*		GetStateInstance().  StateFactoryInit() is used if we need to 
*		instansiate all state objects up front.
*		
*  \note There should only be a single instance of this object. 
*			Instance of this object can be acquired via GetInstance()
*/
class StateFactory
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	StateFactory();

	/*! \brief Destructor
	*  \details none
	*/
	~StateFactory();

	/*! \brief This member function returns instance of StateFactory class
	*  \details StateFactory is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of StateFactory class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static StateFactory* GetInstance();

	/*! \brief Utility function used to initiate destruction of StateFactory class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();

	/*! \brief This function is used to initialize StateFactory class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void StateFactoryInit();


	/*! \brief This function is used to get pointer to an instance of any of the PM APP OTG states
	*  \details This function ensures that there is only one instance of each OTG state.
	*  \param mStateId,  Type: StateEventIdType;  Each state has a unique ID
	*  \return Type of BaseState
	*		- If the state object is already created, it will return the object pointer
	*		- If the state object have not been created, then it will create a new object
	*				and return its pointer
	*  \note none
	*/
	BaseState* GetStateInstance(StateEventIdType mStateId);


#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION

	/*! \brief  This variable holds pointer to A_Idle_State class instance
	*  \details none
	*/
	BaseState* mpA_Idle_State;

	/*! \brief  This variable holds pointer to A_Wait_Vrise_State class instance
	*  \details none
	*/
	BaseState* mpA_Wait_Vrise_State;

	/*! \brief  This variable holds pointer to A_Wait_Bcon_State class instance
	*  \details none
	*/
	BaseState* mpA_Wait_Bcon_State;

	/*! \brief  This variable holds pointer to A_Host_State class instance
	*  \details none
	*/
	BaseState* mpA_Host_State;

	/*! \brief  This variable holds pointer to A_Suspend_State class instance
	*  \details none
	*/
	BaseState* mpA_Suspend_State;

	/*! \brief  This variable holds pointer to A_vbus_Err_State class instance
	*  \details none
	*/
	BaseState* mpA_vbus_Err_State;

	/*! \brief  This variable holds pointer to A_Wait_Vfall_State class instance
	*  \details none
	*/
	BaseState* mpA_Wait_Vfall_State;


    /*! \brief  This variable holds pointer to Carkitclient_Init_State class instance
    *  \details none
    */
    BaseState* mpCarkitclient_Init_State;

    /*! \brief  This variable holds pointer to Carkitclient_Uart_State class instance
    *  \details none
    */
    BaseState* mpCarkitclient_Uart_State;

    /*! \brief  This variable holds pointer to Carkitclient_Audio_State class instance
    *  \details none
    */
    BaseState* mpCarkitclient_Audio_State;

    /*! \brief  This variable holds pointer to Carkitclient_Carkit_Irq_State class instance
    *  \details none
    */
    BaseState* mpCarkitclient_Carkit_Irq_State;

    /*! \brief  This variable holds pointer to Rs232dongle_Config_State class instance
    *  \details none
    */
    BaseState* mpRs232dongle_Config_State;

#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION

    //pointer to all possible state objects
    /*! \brief  This variable holds pointer to Null_State class instance
    *  \details none
    */
    BaseState* mpNull_State;

	/*! \brief  This variable holds pointer to B_Idle_State class instance
	*  \details none
	*/
	BaseState* mpB_Idle_State;

	/*! \brief  This variable holds pointer to B_RemoteDevDetectWait_State class instance
	*  \details none
	*/
	BaseState* mpB_RemoteDevDetectWait_State;

	/*! \brief  This variable holds pointer to B_Peripheral_State class instance
	*  \details none
	*/
	BaseState* mpB_Peripheral_State;

	/*! \brief  This variable holds pointer to B_Suspend_State class instance
	*  \details none
	*/
	BaseState* mpB_Suspend_State;

	/*! \brief  This variable holds pointer to Chg_Conn_State class instance
	*  \details none
	*/
	BaseState* mpChg_Conn_State;

	/*! \brief  This variable holds pointer to Unsought_Mode_State class instance
	*  \details none
	*/
	BaseState* mpUnsought_Mode_State;

protected:
private:
	/*! \brief This static variable is used to hold the singlton instance of StateFactory class
	*   \details none
	*/
	static StateFactory* mpStateFactory_Instance;
};






/**************************************************************************
     class StateChangeEvt											  
**************************************************************************/
/*! \class StateChangeEvt 
*  \brief  This class is responsible for pm app otg state change event 
*  and initiate notification of this event
*  \details It is drived from class Evt.  It is used for debug purpose.  
*		
*  \note We can only have a single instance of this object.  Use GetInstance() 
*  to get the instance.
*/
class StateChangeEvt:public Evt
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	StateChangeEvt();

	/*! \brief Destructor
	*  \details none
	*/
	~StateChangeEvt();

	/*! \brief This member function returns instance of StateChangeEvt class
	*  \details StateChangeEvt is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of StateChangeEvt class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static StateChangeEvt* GetInstance();

	/*! \brief Utility function used to initiate destruction of StateChangeEvt class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	/*! \brief This  function sets the state change event to High and call event listener
	*			so that then new state change is processed by all registered objects
	*  \details override the base Evt class implementation
	*  \param none
	*  \return none
	*  \note none
	*/
	void SetHigh();

	/*! \brief This  function sets the state change event to Lowh and call event listener
	*			so that then new state change is processed by all registered objects
	*  \details override the base Evt class implementation
	*  \param none
	*  \return none
	*  \note none
	*/
	inline void SetLow() ;

private:

	/*! \brief  This variable holds pointer to the previous APP OTG state
	*  \details none
	*/
	BaseState* mpPrevState;  

	/*! \brief  This variable holds pointer to the current APP OTG state
	*  \details none
	*/
	BaseState* mpCurrState;

	/*! \brief This static variable is used to hold the singlton instance of StateFactory class
	*   \details none
	*/
	static StateChangeEvt* mpStateChangeEvt_Instance;

};


#endif //FEATURE_PMIC_USB_OTG
#endif //PMAPP_OTG_STATEMANAGER_H
