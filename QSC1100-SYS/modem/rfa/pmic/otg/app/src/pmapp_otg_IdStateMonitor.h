#ifndef PMAPP_OTG_ID_SM_H
#define PMAPP_OTG_ID_SM_H


/*! \file 
 *  \n
 *  \brief  pmapp_otg_IdStateMonitor.h ----- ID State Monitor related class declaration
 *  \details This header file contains class and type declarations for USB ID line voltage monitoring
 *     state machine used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_IdStateMonitor.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/08   aab      Made changes to properly initialize timer object
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
                            INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pmapp_otg_StateMonitor.h"  

#ifdef FEATURE_PMIC_USB_OTG


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
 
/*!\brief Enum for indicating the voltage condition of the ID line
*/
typedef enum
{
   OTG_ID_SM_INFO__FLT,
   OTG_ID_SM_INFO__GND,
   OTG_ID_SM_INFO__RES,
   OTG_ID_SM_INFO__UNKNOWN
} OtgIdSmInfoType;


/*!\brief Id line state monitor listener function type
*  \details  Type of the listener function that needs to be provided by the client of this
*   state machine so as to get notified about voltage transitions on ID line
*/
typedef void (*OtgIdSmListenerType) (OtgIdSmInfoType, void*);



/*===========================================================================
                              FORWARD DECLARATION
===========================================================================*/
class IdResStateMonitor;


/*===========================================================================
                              CLASS DEFINITION
===========================================================================*/



/**************************************************************************
     class IdStateMonitor												  
**************************************************************************/

/*! \class IdStateMonitor 
 *  \brief This class is used as a base class for class IdResStateMonitor, IdFltStateMonitor 
 *    and IdGndStateMonitor. It inherets from base calss UsbConnStateMonitor
 *  \details This class is used to hold common vars and type defination need by all childeren classes
 *  \note none
  */
class IdStateMonitor: public UsbConnStateMonitor
{
public:
   /*! \brief contructor.
   *  \details This constructor initializes member variables
   */
	IdStateMonitor();

    /*! \brief Destructor.
    */
	~IdStateMonitor();

    /*! \brief This member function returns the singlton instance of IdStateMonitor class object
    *  \details IdStateMonitor is a singlton object, this method ensures that there is only one
    *    instance of this calss object.
    *  \return singlton instance of IdStateMonitor class object
    *  \note to get instance of this class, this method must be called
   */
	static IdStateMonitor* GetInstance();

	/*! \brief Utility function used to initiate destruction of IdStateMonitor class object
	*/
    static void DestroyInstance();

protected:

	/*! \brief mpIdResStateMonitor holds pointer to IdResStateMonitor class 
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitor::IdFltStatusCb()
	*   \li IdGndStateMonitor::IdGndStatusCb()
	*/
	IdResStateMonitor* mpIdResStateMonitor;

	/*! \brief mpIdFltIrqDbStateMgr holds pointer to to Deboucer state manager class for Id Flt Irq
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitor::IdFltStateMonitorInit()
	*   \li IdFltStateMonitor::IdFltStateMonitorReset()
	*   \li IdResStateMonitor::IdResDetectTimerCb()
	*/
	IrqDebouncerStateManager* mpIdFltIrqDbStateMgr;  //ptr to Deboucer state manager for Id Flt Irq


	/*! \brief mpIdFltIrqDbStateMgr holds pointer to to Deboucer state manager class for Id Gnd Irq
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdGndStateMonitor::IdGndStateMonitorInit()
	*   \li IdGndStateMonitor::IdGndStateMonitorReset()
	*   \li IdResStateMonitor::IdResDetectTimerCb()
	*/
	IrqDebouncerStateManager* mpIdGndIrqDbStateMgr;  //ptr to Deboucer state manager for Id Gnd Irq

	/*! \brief Var that holds the current state of the USB ID line
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitor::IdFltStatusCb()
	*   \li IdGndStateMonitor::IdGndStatusCb()
	*   \li IdResStateMonitor::IdResDetectTimerCb()
	*/
    static OtgIdSmInfoType		mIdStateInfo;

	/*! \brief mTimerType timer service internal var
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li IdFltStateMonitor::IdFltStateMonitorInit()
    *   \li IdFltStateMonitor::IdFltStateMonitorReset()
    *   \li IdGndStateMonitor::IdGndStateMonitorInit()
    *   \li IdGndStateMonitor::IdGndStateMonitorReset()
    *   \li IdResStateMonitor::IdStartResDetectTimer()
    *   \li IdResStateMonitor::IdStopResDetectTimer()
    */
	timer_type	mIdSmType;  //timer object ......
private:

	/*! \brief This static variable is used to hold the singlton instance of class IdStateMonitor object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static IdStateMonitor* mpIdStateMonitor_Instance;
};



/**************************************************************************
*     class IdResStateMonitor											  *
**************************************************************************/
/*! \class IdResStateMonitor 
 *  \brief This class is used to detect ID resistive state based on ID ground and ID floating IRQ states
 *  \details This class inherits from class IdStateMonitor
 *  \note none
 */
class IdResStateMonitor: public IdStateMonitor
{
public:
   /*! \brief Contructor.
   *  \details This constructor initializes member variables
   */
	IdResStateMonitor();

    /*! \brief  Destructor.
    */
	~IdResStateMonitor();

 
    /*! \brief This member function returns the singlton instance of IdResStateMonitor class object
    *  \details IdResStateMonitor is a singlton object, this method ensures that there is only one
    *    instance of this calss object.
    *  \return singlton instance of IdResStateMonitor class object
    *  \note to get instance of this class, this method must be called
   */
	static IdResStateMonitor* GetInstance();

	/*! \brief Utility function used to initiate destruction of IdStateMonitor class object
	*/
    static void DestroyInstance();


    /*! \brief This member function initializes IdResStateMonitor 
    *  \details none
	*  \param mpOtgIdResStateMonitorCb, Type: OtgIdSmListenerType, holds pointer to the callback function
	*  \param mpOtgIdResListenerObj,  Type: void*, holds pointer to the class object where the listener callback function belongs
    *  \return none
    *  \note none
   */
	void IdResStateMonitorInit(OtgIdSmListenerType listener, void* pListenerObj);


    /*! \brief This member function reset IdResStateMonitor object
    *  \details none
	*  \param none
    *  \return none
    *  \note none
    */
	void IdResStateMonitorReset(void);


    /*! \brief ID resistive state detection start timer fn
    *  \details none
	*  \param none
    *  \return none
    *  \note none
    */
	void IdStartResDetectTimer(void);

    /*! \brief ID resistive state detection stop timer fn
    *  \details none
	*  \param none
    *  \return none
    *  \note none
    */
	void IdStopResDetectTimer(void);

    /*! \brief ID resistive state detection timer callback function
	*  \details none
	*  \param none
    *  \return none
    *  \note none
    */
	static void IdResDetectTimerCb(int32 time_ms, timer_cb_data_type token);

protected:

private:

	/*! \brief Var that holds callback function pointer used to notify the state of ID line status
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdResStateMonitor()
	*   \li IdResStateMonitorInit()
	*   \li IdResDetectTimerCb()
	*/
	OtgIdSmListenerType  mpOtgIdResStateMonitorCb;

	/*! \brief Var that holds the class object to which the listener callback function belongs
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdResStateMonitorInit()
	*   \li IdResDetectTimerCb()
	*/
	void* mpOtgIdResListenerObj;

	/*! \brief flag used to indicate whether IdResStateMonitor object is initialized or not
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdResStateMonitor()
	*   \li IdResStateMonitorInit()
	*   \li IdResStateMonitorReset()
	*/
	boolean mIdResSmInitialized;  


	/*! \brief flag used to indicate if Id resistive timer (used to detect ID resistive state)is running or not
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdResStateMonitor()
	*   \li IdStartResDetectTimer()
	*   \li IdStopResDetectTimer()
	*/
    boolean mIdResDetectTimerRunning;

	/*! \brief internal var to hold the length of time in ms needed to detect ID resistive state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdResStateMonitor()
	*   \li IdStartResDetectTimer()
	*/
	const int32  mIdResDetectTmoDelay;

	/*! \brief This static variable is used to hold the singlton instance of class IdResStateMonitor object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static IdResStateMonitor* mpIdResStateMonitor_Instance;

};



/**************************************************************************
     class IdFltStateMonitor											  
**************************************************************************/

/*! \class IdFltStateMonitor 
 *  \brief This class has functions that are used to detect ID floating state of the USB ID line
 *  \details  It also instantiate debouncer that monitors ID floating IRQ. 
 *		This class inherits from class IdStateMonitor
 *  \note none
 */
class IdFltStateMonitor: public IdStateMonitor
{
public:
   /*! \brief Contructor.
   *  \details This constructor initializes member variables
   */
	IdFltStateMonitor();

    /*! \brief Destructor.
    */
	~IdFltStateMonitor();


    /*! \brief This member function returns the singlton instance of IdFltStateMonitor class object
    *  \details IdFltStateMonitor is a singlton object, this method ensures that there is only one
    *    instance of this calss object.
    *  \return singlton instance of IdFltStateMonitor class object
    *  \note to get instance of this class, this method must be called
   */
	static IdFltStateMonitor* GetInstance();

	/*! \brief Utility function used to initiate destruction of IdStateMonitor class object
	*/
    static void DestroyInstance();


    /*! \brief Initializes and sets up the ID line monitoring state machine
    *  \details This member function initializes initializes USB ID floating IRQ state monitor debouncer
    *  \param listener : Callback function of the client that will be used for notifying about voltage transitions on the ID line.
    *  \param pListenerObj:  Class object where the listener call back function is defined
    *  \return none
    *  \note none
    */
	void IdFltStateMonitorInit(OtgIdSmListenerType listener, void* pListenerObj);


    /*! \brief resets the ID floating monitoring state machine
    *  \details none
	*  \param none
    *  \return none
    *  \note none
    */
	void IdFltStateMonitorReset(void);


	/*! \brief Debouncer related fns for the ID-floating comparator
	*  \details This is the service routine for the debounced USB ID line raising/falling-edge
    *		interrupt. This notifies its listener about the USB ID line condition.
	*  \param status :  Current status of USB ID-Floating line
	*		- Type : IrqDebouncerStatusType
	*  \param pStateObj : pointer to the class object instance to which IdFltStatusCb() belongs
	*		- Type : void*
	*  \return none
	*  \note none
	*/
	static void IdFltStatusCb(IrqDebouncerStatusType status, void* pStateObj);


protected:

private:

	/*! \brief Var that holds callback function pointer used to notify the state of ID line status
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitor()
	*   \li IdFltStateMonitorInit()
	*   \li IdFltStatusCb()
	*/
	OtgIdSmListenerType  mpOtgIdFltStateMonitorCb;

	/*! \brief Var that holds the class object to which the listener callback function belongs
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitorInit()
	*   \li IdFltStatusCb()
	*/
	void* mpOtgIdFltListenerObj;

	/*! \brief flag used to indicate whether IdFltStateMonitor object is initialized or not
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitor()
	*   \li IdFltStateMonitorInit()
	*   \li IdFltStateMonitorReset()
	*/
	boolean mIdFltSmInitialized;


	/*! \brief Var that holds the length of time in ms to wait in ID-Float debouncer BothIrqOff state
	*			before transitioning to CompIrqOn state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitor()
	*   \li IdFltStateMonitorInit()
	*/
	const int32  mIdFltDebouncerTmr_BothIrqOff;

	/*! \brief Var that holds the length of time in ms to wait in ID-Float debouncer CompIrqOn state
	*			before transitioning to Stablized state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdFltStateMonitor()
	*   \li IdFltStateMonitorInit()
	*/
	const int32  mIdFltDebouncerTmr_CompIrqOn;

	/*! \brief This static variable is used to hold the singlton instance of class IdFltStateMonitor object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static IdFltStateMonitor* mpIdFltStateMonitor_Instance;

};



/**************************************************************************
     class IdGndStateMonitor											  
**************************************************************************/

/*! \class IdGndStateMonitor 
 *  \brief This class has functions that are used to detect ID ground state of the USB ID line
 *  \details  It also instantiate debouncer that monitors ID ground IRQ. 
 *		This class inherits from class IdStateMonitor
 *  \note none
 */

class IdGndStateMonitor: public IdStateMonitor
{
public:
   /*! \brief Contructor.
   *  \details This constructor initializes member variables
   */
	IdGndStateMonitor();

    /*! \brief Destructor.
    */
	~IdGndStateMonitor();


    /*! \brief This member function returns the singlton instance of IdGndStateMonitor class object
    *  \details IdGndStateMonitor is a singlton object, this method ensures that there is only one
    *    instance of this calss object.
    *  \return IdGndStateMonitor
    *  \note to get instance of this class, this method must be called
    */
	static IdGndStateMonitor* GetInstance();

	/*! \brief Utility function used to initiate destruction of IdStateMonitor class object
	*/
    static void DestroyInstance();


//??????????????????why do we have to have this function here >>>???????????????????????????????????????
   /*!\brief This member function  initializes and sets up the ID line Resistive state monitoring
   *  \details Id resistive state is determined based on ID ground and Id floating states
   *  \param listener : Callback function of the client that will be used for notifying about voltage transitions on the ID line.
   *  \param pListenerObj: Class object where the listener call back function is defined
   *  \return none
   *  \note This function only called by Id ground and Id floating state monitoring functions 
   */
/*
    void IdResStateMonitorInit(OtgIdSmListenerType listener, void* pListenerObj);
*/


    /*! \brief Initializes and sets up the ID line monitoring state machine
    *  \details This member function initializes initializes USB ID ground IRQ state monitor debouncer
    *  \param listener : Callback function of the client that will be used for notifying about voltage transitions on the ID line.
    *  \param pListenerObj:  Class object where the listener call back function is defined
    *  \return none
    *  \note This function can be called any time there is a need to monitor USB ID line status.
    */
	void IdGndStateMonitorInit(OtgIdSmListenerType listener, void* pListenerObj);


    /*! \brief resets the ID ground monitoring state machine
    *  \details none
	*  \param none
    *  \return none
    *  \note none
    */
	void IdGndStateMonitorReset(void);


	/*! \brief Debouncer related fns for the ID-ground comparator
	*  \details This is the service routine for the debounced USB ID line raising/falling-edge
    *		interrupt. This notifies its listener about the USB ID line condition.
	*  \param status :  Current status of USB ID-ground line
	*		- Type : IrqDebouncerStatusType
	*  \param pStateObj : pointer to the class object instance to which IdGndStatusCb() belongs
	*		- Type : void*
	*  \return none
	*  \note none
	*/
	static void IdGndStatusCb(IrqDebouncerStatusType status, void* pStateObj);

protected:

private:

	/*! \brief Var that holds callback function pointer used to notify the state of ID-Ground IRQ status
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdGndStateMonitor()
	*   \li IdGndStateMonitorInit()
	*   \li IdGndStatusCb()
	*/
	OtgIdSmListenerType  mpOtgIdGndStateMonitorCb;


	/*! \brief Var that holds the class object to which the listener callback function belongs
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdGndStateMonitorInit()
	*   \li IdGndStatusCb()
	*/
	void* mpOtgIdGndListenerObj;


	/*! \brief flag used to indicate whether IdGndStateMonitor object is initialized or not
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdGndStateMonitor()
	*   \li IdGndStateMonitorInit()
	*   \li IdGndStateMonitorReset()
	*/
	boolean mIdGndSmInitialized; 

	/*! \brief Var that holds the length of time in ms to wait in ID-Gnd debouncer BothIrqOff state
	*			before transitioning to CompIrqOn state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdGndStateMonitor()
	*   \li IdGndStateMonitorInit()
	*/

	const int32  mIdGndDebouncerTmr_BothIrqOff;

	/*! \brief Var that holds the length of time in ms to wait in ID-Gnd debouncer CompIrqOn state
	*			before transitioning to Stablized state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li IdGndStateMonitor()
	*   \li IdGndStateMonitorInit()
	*/
	const int32  mIdGndDebouncerTmr_CompIrqOn;


	/*! \brief This static variable is used to hold the singlton instance of class IdGndStateMonitor object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static IdGndStateMonitor* mpIdGndStateMonitor_Instance;
};


#endif //FEATURE_PMIC_USB_OTG
#endif // PMAPP_OTG_ID_SM_H
       
