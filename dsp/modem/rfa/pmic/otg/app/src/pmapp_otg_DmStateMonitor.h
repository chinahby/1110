#ifndef PMAPP_OTG_DM_SM_H
#define PMAPP_OTG_DM_SM_H

/*! \file 
 *  \n
 *  \brief  pmapp_otg_DmStateMonitor.h  ----- Dm State Monitor related class declaration
 *  \details This header file contains class and type declarations for USB DM line voltage monitoring
 *     state machine used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_DmStateMonitor.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
/*!\brief Enum for indicating the voltage condition of USB DM line state type
*/    
typedef enum
{
   OTG_DM_SM_INFO__LOW,          // D- low interrupt
   OTG_DM_SM_INFO__HIGH          // D- high interrupt
} OtgDmSmInfoType;



/*!\brief Typedef that defines the type of the listener function that needs to be 
*	provided by the client of debouncer state machine so as to get notified
*	about voltage transitions on DM line
*/    
typedef void (*OtgDmSmListenerType) (OtgDmSmInfoType, void*);




/*===========================================================================
                              CLASS DEFINITION
===========================================================================*/
/**************************************************************************
     class DmStateMonitor											  
**************************************************************************/
/*! \class DmStateMonitor 
*  \brief This class contains the necessary routines used to instansiate, reset 
*		USB DM line voltage monitoring debouncer state machine. 
*  \details This class has a method to get the current stable state of DM line via a callback function
*  \note none
*/
class DmStateMonitor: public UsbConnStateMonitor
{
public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	DmStateMonitor();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~DmStateMonitor();

	/*! \brief Utility function used to create a singlton instance of DmStateMonitor object
	*  \details none
	*  \param none
	*  \return singlton instance of DmStateMonitor class object
	*  \note none
	*/
	static DmStateMonitor* GetInstance();


	/*! \brief Utility function used to initiate destruction of DmStateMonitor class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	/*! \brief This function initializes and sets up the DM line monitoring debouncer state machine
	*  \details none
	*  \param 1. listener : Callback function of the client that will be used for notifying about
    *    voltage transitions on the DM line.
    *	- Type: OtgDmSmListenerType
    *	- Valid inputs: Valid non-NULL listener callback fn
	*  \param 2. pListenerObj : Variable that holds pointer to class object that listener callback
	*		function belongs
	*  \return none
	*  \note none
	*/
	void DmStateMonitorInit(OtgDmSmListenerType listener, void* pListenerObj);


	/*! \brief This function resets the DM line monitoring debouncer state machine.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void DmStateMonitorReset(void);



	/*! \brief Debouncer related fns for the D- HI comparator
	*  \details This is the service routine for the debounced USB DM-HI line raising/falling-edge
    *		interrupt. This notifies its listener about the USB DM line condition.
	*  \param status :  Current status of USB DM-HI line
	*		- Type : IrqDebouncerStatusType
	*		- Possible values: OTG_DM_SM_INFO__HIGH,  OTG_DM_SM_INFO__LOW
	*  \param pStateObj : pointer to the class object instance to which DmHiStatusCb() belongs
	*		- Type : void*
	*  \return none
	*  \note none
	*/
	static void DmHiStatusCb(IrqDebouncerStatusType status, void* pStateObj);

protected:

private:

	/*! \brief Var that holds the current state of the USB DM line
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DmHiStatusCb()
	*/
	OtgDmSmInfoType  mDmStateInfo;

	/*! \brief Var that holds callback function pointer used to notify the state of DM line status
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DmStateMonitorInit()
	*   \li DmHiStatusCb()
	*/
	OtgDmSmListenerType  mpOtgDmHiStateMonitorCb;

	/*! \brief Var that holds the class object to which the listener callback function belongs
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DmStateMonitorInit()
	*   \li DmHiStatusCb()
	*/
	void* mpOtgDmHiListenerObj;

	/*! \brief Var that holds the length of time in ms to wait in Dm debouncer BothIrqOff state
	*			before transitioning to CompIrqOn state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DmStateMonitor()
	*   \li DmStateMonitorInit()
	*/
	const int32  mDmHiDebouncerTmr_BothIrqOff;

	/*! \brief Var that holds the length of time in ms to wait in Dm debouncer CompIrqOn state
	*			before transitioning to Stablized state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DmStateMonitor()
	*   \li DmStateMonitorInit()
	*/
	const int32  mDmHiDebouncerTmr_CompIrqOn;

	/*! \brief Var flag used to indicate if Dm state monitor class object have been initialized
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DmStateMonitorInit()
	*   \li DmStateMonitorReset()
	*/
	boolean  mDmSmInitialized;

	/*! \brief This static variable is used to hold the singlton instance of class DmStateMonitor object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static DmStateMonitor* mpDmStateMonitor_Instance;

	/*! \brief pointer variable that holds pointer to the debouncer state manager object for DM Hi Irq
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DmStateMonitor()
	*   \li DmStateMonitorInit()
	*	\li DmStateMonitorReset()
	*/
	IrqDebouncerStateManager* mpDmIrqDbStateMgr;

};


#endif //FEATURE_PMIC_USB_OTG

#endif // PMAPP_OTG_DM_SM_H
