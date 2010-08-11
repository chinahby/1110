#ifndef PMAPP_IRQDEBOUNCER_H
#define PMAPP_IRQDEBOUNCER_H

/*! \file 
*  \n
*  \brief  pmapp_IrqDebouncer.h ---- Debouncer related class declaration
*  \details This header file contains class declarations of the Irq debouncer State Manager and 
*  the various debouncer states for the interrupt line debouncer developed for the 
*  Qualcomm Power Management Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_IrqDebouncer.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/08   aab      Removed timer type static defination since it is not needed here
08/08/08   aab      Made changes to properly initialize timer object
07/24/08   aab      Included pmapp_ads_compat.h to resolve ads120 compiler warnings
07/23/08   aab      Made changes to resolve ads120 compiler warnings
12/01/07   aab      Created.

========================================================================== */

/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/

extern "C"
{
#include "comdef.h"
#include "pm.h"
#include "assert.h" 
#include "timer.h" 
}

#include "pmapp_ads_compat.h"

/* =========================================================================
TYPE DEFINITIONS
========================================================================= */
/*!\brief Enum used for indicating debouncer status type
*  \details  
*/    
typedef enum
{
	IRQ_DEBOUNCER_STATUS__HIGH,
	IRQ_DEBOUNCER_STATUS__LOW,
	IRQ_DEBOUNCER_STATUS__UNKNOWN
}IrqDebouncerStatusType;



/*!\brief State Id Enum used in the debouncer state machine 
*  \details  
*/    
typedef enum
{
	DEBOUNCER_STATE__NULL = 0,
	DEBOUNCER_STATE__BOTH_IRQ_OFF,
	DEBOUNCER_STATE__COMP_IRQ_ON,
	DEBOUNCER_STATE__STABILIZED,
	DEBOUNCER_STATE__INVALID
}DebouncerStateEventIdType;


/*!\brief defines type of Irq debouncer call back function.
* This notification function is used to let the user of this debouncer know
* that a new "debounced" interrupt has happened and that the user can
* do his own processing to act on that interrupt.
*
* \detail It is extremely IMPORTANT to note that the user shall not perform
* IRQ level functionalities like clearing/masking/unmasking the interrupt
* because interrupt management is completely taken care of by this
* debouncer. The user is expected to perform only user level functionalities
* like what it means to get an interrupt of the specified type.
*/    
typedef void (*IrqDebouncerProcessIrqCb) (IrqDebouncerStatusType, void*);


/*===========================================================================
FORWARD DECLARATION
===========================================================================*/
class DebouncerBaseState;




/*===========================================================================
CLASS DEFINITION
===========================================================================*/

/**************************************************************************
*   class IrqDebouncerInstance											  
**************************************************************************/
/*! \class IrqDebouncerInstance 
*  \brief This class is used to hold Irq debouncer object instance variables.
*  \details This class is accessed by IrqDebouncerStateManager
*  \note none
*/
class IrqDebouncerInstance
{
public:

	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	IrqDebouncerInstance();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~IrqDebouncerInstance();

	// Statically initialized variables (variables that are initialized only once)
	/*! \brief debouncerInit_MagicPattern_Static is used as a flag to initialize vars 
	*		that needs to be initialized only once (the first time the debouncer is initialized)
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init
    */
	uint32									debouncerInit_MagicPattern_Static;

	/*! \brief current_state_ptr holds the current state of the debouncer state machine
    *   \details This variable will be used/updated by the following function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::ProcessCachedEvt()
    */
	DebouncerBaseState*						current_state_ptr;

	/*! \brief reset_request_pending is used as a flag that indicate debouncer 
	*           state machine reset request is pending
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li NullState::CachedEvtHandler()
    *   \li BothIrqOffState::CachedEvtHandler()
    *   \li ComplementaryIrqOnState::CachedEvtHandler()
    *   \li StabilizedState::CachedEvtHandler()
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::Reset()
    */
	boolean									reset_request_pending;


	/*! \brief cached_evt_handler_stack_depth is an event processing var that is used to help 
	*         properly executing state transitions if events happen to occur in a nested fashion
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::ProcessCachedEvt()
    */
	volatile uint8							cached_evt_handler_stack_depth;


	/*! \brief debouncerInit_MagicPattern_Dynamic var is used to indicate that the debouncer state machine
	*       is active or in reset.
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::Reset()
    *   \li IrqDebouncerStateManager::TimerServiceCb()
    *   \li IrqDebouncerStateManager::IrqServiceCb()
	*   \li NullState::CachedEvtHandler()
    */
	uint32                                   debouncerInit_MagicPattern_Dynamic;


	/*! \brief re_irq is used to store Raising-edge IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::IrqServiceDisable()
    *   \li ComplementaryIrqOnState::Entry()
    */
	pm_irq_hdl_type                          re_irq;
	
	/*! \brief fe_irq is used to store Falling-edge IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::IrqServiceDisable()
    *   \li ComplementaryIrqOnState::Entry()
    */
	pm_irq_hdl_type                          fe_irq;

	/*! \brief rt_irq is used to store Real-time state of the IRQ line
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::IrqServiceDisable()
    *   \li ComplementaryIrqOnState::Entry()
    */
	pm_rt_status_type                        rt_irq;

	/*! \brief tmr__both_irq_off holds length of time to wait in both_irq_off state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li BothIrqOffState::Entry()
    */
	int32                                    tmr__both_irq_off;

	/*! \brief tmr__comp_irq_on holds length of time to wait in comp_irq_on state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li ComplementaryIrqOnState::Entry()
    */
	int32                                    tmr__comp_irq_on;

	/*! \brief process_irq_cb holds the callback function to call when debouncer determains a stable state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li StabilizedState::Entry()
    */
	IrqDebouncerProcessIrqCb				 process_irq_cb;

	/*! \brief status holds the current state of the debounced line
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::GetStatus()
    *   \li NullState::CachedEvtHandler()
    *   \li BothIrqOffState::Entry()
    *   \li ComplementaryIrqOnState::Entry()
    *   \li StabilizedState::Entry()
	*	\li Possible values are:
	*		- IRQ_DEBOUNCER_STATUS__HIGH,
	*		- IRQ_DEBOUNCER_STATUS__LOW,
	*		- IRQ_DEBOUNCER_STATUS__UNKNOWN
    */
	IrqDebouncerStatusType					 status;

	/*! \brief curr_timer_sess_id holds the unique timer id number
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::TimerServiceCb()
    *   \li IrqDebouncerStateManager::StartTimer()
    */
	timer_cb_data_type                       curr_timer_sess_id;

	/*! \brief tmo_occured is a flag to indicate that time out event had occured
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::TimerServiceCb()
    *   \li IrqDebouncerStateManager::StartTimer()
    *   \li BothIrqOffState::CachedEvtHandler()
    *   \li ComplementaryIrqOnState::CachedEvtHandler()
    */
	boolean                                  tmo_occured;

	/*! \brief irq_occured is a flag to indicate that IRQ event had occured
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::IrqServiceCb()
    *   \li ComplementaryIrqOnState::CachedEvtHandler()
    *   \li StabilizedState::CachedEvtHandler()
    */
	boolean                                  irq_occured;


	/*! \brief irq_service__initialized is IRQ service internal var
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::IrqServiceReset()
    *   \li IrqDebouncerStateManager::IrqServiceDisable() 
    */
	boolean                                  irq_service__initialized;

	/*! \brief irq_service__en_irq is IRQ service internal var
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::IrqServiceEnable()
    *   \li IrqDebouncerStateManager::IrqServiceDisable() 
    */
	pm_irq_hdl_type                          irq_service__en_irq;

	/*! \brief irqStateMonitorObj holds call back function class object when regestering for irq event
	*         This object ptr will be passed allong (as an argument) when the callback function is called.
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li StabilizedState::Entry()
    */
	void*									irqStateMonitorObj;

	/*! \brief mDbTimer timer service internal var
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::StartTimer()
    *   \li IrqDebouncerStateManager::StopTimer()
    */
	timer_type								mDbTimer;  
};




/**************************************************************************
     class IrqDebouncerStateManager											  
**************************************************************************/
/*! \class IrqDebouncerStateManager 
*  \brief This class is used to manage debouncer state machine that is instantiated by user to 
*   monitor the signal status of a particular IRQ.
*
*  \details This class has methods to initiate/reset and manage the debouncer state machine.  
*    It also has call back functions to listen to the IRQ/Timer calls.  
*
*  \note none
*
* */
class IrqDebouncerStateManager
{
public:

	/*! \brief constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	IrqDebouncerStateManager();


	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~IrqDebouncerStateManager();

	/*! \brief PreInit() pre initializes IrqDebouncerStateManager
	*  \details instantiate all objects needed by the debouncer
	*  \param none
	*  \return none
	*  \note none
	*/
	void PreInit();


	/*! \brief This member function initializes debouncer state machine
	*  \details initializes and sets up the specified IRQ debouncer for detection
	*		and notification of true state of any IRQ line
	*  \param void*	 irqStateMonObj: object pointer whrere the call back function process_irq_cb belongs
	*  \param pm_irq_hdl_type re_irq: 
	*     - Raising-edge IRQ, if it is an EDGE triggered interrupt, e.g. PM_USB_BUS_VALID_RE_IRQ_HDL
    *	  - For LEVEL triggered interrupt, use the one that indicates the
	*        positive/high/valid state of the interrupt, e.g. PM_VALID_CHG_IRQ_HDL
	*  \param pm_irq_hdl_type fe_irq:
    *	  - Falling-edge IRQ, if it is an EDGE triggered interrupt, e.g. PM_USB_BUS_VALID_FE_IRQ_HDL
    *	  - For LEVEL triggered interrupt, use the one that indicates the
    *		    negative/low/invalid state of the interrupt, e.g. PM_INVALID_CHG_IRQ_HDL
	*  \param pm_rt_status_type rt_irq:
    *	  - Real-time state of the IRQ line for EDGE triggered interrupts, e.g. PM_USB_BUS_VALID_RT_ST
    *	  - For LEVEL triggered interrupt, use the one that corresponds to the
    *	    positive/high/valid state of the interrupt, e.g. PM_VALID_CHG_RT_ST
	*  \param int32 tmr__both_irq_off: length of time to wait in both_irq_off state
	*  \param int32 tmr__comp_irq_on: length of time to wait in comp_irq_on state
	*  \param IrqDebouncerProcessIrqCb process_irq_cb:
    *	  - User function that is to be called back whenever the debouncer detects a new "debounced"
    *	    interrupt on the specified IRQ line.
	*  \return pm_err_flag_type,  Possible values: PM_ERR_FLAG__SUCCESS indicates that Operation succeeded
	*  \note The following function(s) must have been called: rflib_init(),  pm_init()
	*  \note Interrupts are locked during the execution of this function.
	*/
	pm_err_flag_type Init (
			void*									 irqStateMonObj,  
			pm_irq_hdl_type                          re_irq,
			pm_irq_hdl_type                          fe_irq,
			pm_rt_status_type                        rt_irq,
			int32                                    tmr__both_irq_off,
			int32                                    tmr__comp_irq_on,
			IrqDebouncerProcessIrqCb			     process_irq_cb
		);


	/*! \brief This member function resets debouncer
	*  \details This function stops the debouncer state machine in its tracks and prevents
	*		any future notifications of "debounced" IRQ state.
	*		To restart the debouncer, call pm_app_irq_debouncer_init() again. 
	*  \param none
	*  \return pm_err_flag_type
	*  \note none
	*/
	pm_err_flag_type Reset();



	/*! \brief This member function provids current status of IRQ line
	*  \details This function returns the current status of the IRQ line serviced by the indicated IRQ debouncer. 
	*  \param none
	*  \return returns one of IrqDebouncerStatusType:
	*   - IRQ_DEBOUNCER_STATUS__HIGH     -->  IRQ happened (debounced status)
    *	- IRQ_DEBOUNCER_STATUS__LOW      -->  IRQ gone     (debounced status)
    *	- IRQ_DEBOUNCER_STATUS__UNKNOWN  -->  IRQ debounce in progress or NULL debouncer_ptr
	*  \note none
	*/
	IrqDebouncerStatusType GetStatus();

	//
	/*! \brief IRQ management service for the debouncer state machine
	*  \details resets IRQ service
	*  \param none
	*  \return none
	*  \note none
	*/
	void IrqServiceReset();


	/*! \brief This member function enables Irq service
	*  \details none
	*  \param pm_irq_hdl_type irq
	*  \return none
	*  \note none
	*/
	void IrqServiceEnable(pm_irq_hdl_type irq);

	/*! \brief This member function disable IRQ service
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void IrqServiceDisable();

	/*! \brief This member function starts timer
	*  \details Timer management functionality for the debouncer state machine
	*  \param int32 tmo_delay (length of time to delay before the timer expires in ms)
	*  \return none
	*  \note none
	*/
	void StartTimer(int32 tmo_delay);

	/*! \brief This member function stops timer
	*  \details Timer management functionality for the debouncer state machine
	*  \param none
	*  \return none
	*  \note none
	*/
	void StopTimer();


	/*! \brief This member function used indicate the current state of debouncer
	*  \details  used to support UnitTest
	*  \param none
	*  \return DebouncerStateEventIdType 
	*  \note none
	*/
	uint8 GetCurrentStateID(); 

	//protected:

	/*! \brief This member function is timer service callback function
	*  \details When the timer expires, this call back function gets called and this function creats an event
	*  \param int32 time_ms ; length of the timer that has expired
	*  \param int32 timer_cb_data_type token; pointer to the class object where TimerServiceCb() belongs
	*  \return none
	*  \note none
	*/
	static void TimerServiceCb(int32 time_ms, timer_cb_data_type token);

	/*! \brief This member function is the IRQ service callback function
	*  \details When there is IRQ event, this callback function gets calld and this function creats an event
	*  \param void* data; pointer to class object where IrqServiceCb() belongs
	*  \return none
	*  \note none
	*/
	static void IrqServiceCb(void* data);   


	/*! \brief Pointer used to debouncer instance class object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li (Used by all member functions)
    */
	IrqDebouncerInstance* mpIrqDebouncerInstance;

	/*! \brief mpDbNullState holds a pointer to Null state object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li PreInit()
    *   \li Init()
    *   \li NullState::CachedEvtHandler()
    *   \li BothIrqOffState::CachedEvtHandler()
    *   \li ComplementaryIrqOnState::CachedEvtHandler()
    *   \li StabilizedState::CachedEvtHandler()
    */
	DebouncerBaseState* mpDbNullState;


	/*! \brief mpDbBothIrqOffState holds a pointer to Both IRQ Off state object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li PreInit()
    *   \li NullState::CachedEvtHandler()
    *   \li BothIrqOffState::CachedEvtHandler()
    *   \li ComplementaryIrqOnState::CachedEvtHandler()
    *   \li StabilizedState::CachedEvtHandler()
    */
	DebouncerBaseState* mpDbBothIrqOffState;


	/*! \brief mpDbComplIrqOnState holds a pointer to Comp IRQ On state object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li PreInit()
    *   \li BothIrqOffState::CachedEvtHandler()
    *   \li ComplementaryIrqOnState::CachedEvtHandler()
    */
	DebouncerBaseState* mpDbComplIrqOnState;


	/*! \brief mpDbStabilizedState holds a pointer to Stablized state object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li PreInit()
    *   \li ComplementaryIrqOnState::CachedEvtHandler()
    *   \li StabilizedState::CachedEvtHandler()
    */
	DebouncerBaseState* mpDbStabilizedState;

protected:
	/*! \brief Pointer used to hold the current state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::ProcessCachedEvt()
    */
	DebouncerBaseState* mpCurrState;  

	/*! \brief Pointer used to hold previous state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IrqDebouncerStateManager::Init()
    *   \li IrqDebouncerStateManager::ProcessCachedEvt()
    */
	DebouncerBaseState* mpPrevState;  

private:
	
	/*! \brief ProcessCachedEvt() is used to manage debouncer states machine
	*  \details This function services any interrupts / external inputs to the IRQ debouncer
	*		state machine and causes state transition(s) as required by the individual
	*		states. The state-entry handler is called when the state machine enters a
	*		new state after which its cached-event handler is called to process events
	*		accumulated till that point. This process is continued as long as state
	*		transitions are caused by the cached-event handling in the individual states.
	*  \param none
	*  \return none
	*  \note none
	*/
	void ProcessCachedEvt();
};



/**************************************************************************
     class DebouncerBaseState											  
**************************************************************************/
/*! \class DebouncerBaseState 
*  \brief This class is used as a base sttate to all debouncer state machine states
*  \details none
*  \note none
*	\image html debouncer_state_machine.PNG
* */
class DebouncerBaseState
{

public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	DebouncerBaseState();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual ~DebouncerBaseState();	

	/*! \brief virtual Entry() function
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry()=0;

	/*! \brief virtual CachedEvtHandler() function
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual DebouncerBaseState* CachedEvtHandler()=0;

	/*! \brief char pointer used to store the name of the state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li (constractor of each debouncer state)
    */
	const char  *mStateName;     // just for debugging

	/*! \brief Pointer used to store the state Id
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li (constractor of each debouncer state)
    */
	DebouncerStateEventIdType mStateId;

protected:
	/*! \brief Pointer used to store pointer to debouncer state machine manager, IrqDebouncerStateManager
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li Constructor of all debouncer state
    *   \li CachedEvtHandler() function of all debouncer states
    */
	IrqDebouncerStateManager* mpIrqDbStateMgr;

	/*! \brief Pointer used to store pointer to debouncer instance utility object, IrqDebouncerInstance
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li Constructor of all debouncer state
    *   \li CachedEvtHandler() function of all debouncer states
    */
	IrqDebouncerInstance* mpIrqDbInstance;

private:

};



/**************************************************************************
     class DebouncerNullState											  
**************************************************************************/
/*! \class DebouncerNullState 
*  \brief This class represents one of the state in the debouncer state machine.
*
*  \details We get into this state once the debouncer gets initialized.  
*     If the debouncer is not in reset, then we will automatically transition to BothIrqOff state.
*
*  \note none
*
*/
class DebouncerNullState: public DebouncerBaseState
{
public:
	/*! \brief 
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	DebouncerNullState(IrqDebouncerStateManager* pIrqDbSM);

	/*! \brief 
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual ~DebouncerNullState();

	/*! \brief Executes routines that needs to be executed only during entrance of Null state
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief Executes routines that needs to be executed while in Null state
	*  \details This routine gets executed when ever there is irq or timer event
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual DebouncerBaseState* CachedEvtHandler();

};



/**************************************************************************
     class DebouncerBothIrqOffState											  
**************************************************************************/
/*! \class DebouncerBothIrqOffState 
*  \brief  This class represents one of the state in the debouncer state machine.
*
*  \details When debouncer gets in this state, it starts a timer to wait in this state.
*      If the timer wait expires and there was no new IRQ event or reset pending, then,
*		the state transitions to ComplementaryIrqOn state.
*
*  \note none
*
*/
class DebouncerBothIrqOffState: public DebouncerBaseState
{
public:
	/*! \brief 
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	DebouncerBothIrqOffState(IrqDebouncerStateManager* pIrqDbSM);

	/*! \brief 
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual ~DebouncerBothIrqOffState();

	/*! \brief Executes routines that needs to be executed only during entrance of BothIrqOff state
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief Executes routines that needs to be executed while in BothIrqOff state
	*  \details This routine gets executed when ever there is irq or timer event
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual DebouncerBaseState* CachedEvtHandler();

};




/**************************************************************************
     class DebouncerComplementaryIrqOnState											  
**************************************************************************/
/*! \class DebouncerComplementaryIrqOnState 
*  \brief  This class represents one of the state in the debouncer state machine.
*
*  \details When debouncer gets into this state:
*     - Depending on the current real time status of the IRQ, it will register to either RE/FE IRQ event
*	   - It also starts a timer to wait in this state.
*      If the timer wait expires before there is any new IRQ event or reset comand, then,
*		the state transitions to Stablized state.
*
*  \note none
*
*/
class DebouncerComplementaryIrqOnState: public DebouncerBaseState
{
public:
	/*! \brief 
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	DebouncerComplementaryIrqOnState(IrqDebouncerStateManager* pIrqDbSM);

	/*! \brief 
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual ~DebouncerComplementaryIrqOnState();

	/*! \brief Executes routines that needs to be executed only during entrance of ComplementaryIrqOn state
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief Executes routines that needs to be executed while in ComplementaryIrqOn state
	*  \details This routine gets executed when ever there is irq or timer event
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual DebouncerBaseState* CachedEvtHandler();
};




/**************************************************************************
     class DebouncerStabilizedState											  
**************************************************************************/
/*! \class DebouncerStabilizedState 
*  \brief This class represents one of the state in the debouncer state machine.
*
*  \details Debouncer gets in Stabilized state once it successfully detects a stable signal.
*    Upon entry in to this state, the registered callback function will be called to indicate that
*    the signal line Irq that we are debouncing has a stable state.
*
*  \note none
*
*/
class DebouncerStabilizedState: public DebouncerBaseState
{
public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	DebouncerStabilizedState(IrqDebouncerStateManager* pIrqDbSM);

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual ~DebouncerStabilizedState();

	/*! \brief Executes routines that needs to be executed only during entrance of stablized state
	*  \details notifies the registered callback function with the current stablized state.
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual void Entry();

	/*! \brief Executes routines that needs to be executed while in Stablized state
	*  \details This routine gets executed when ever there is irq or timer event
	*  \param none
	*  \return none
	*  \note none
	*/
	virtual DebouncerBaseState* CachedEvtHandler();

};

#endif // PMAPP_IRQDEBOUNCER_H
