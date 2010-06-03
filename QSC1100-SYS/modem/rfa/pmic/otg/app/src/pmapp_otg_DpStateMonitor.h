#ifndef PMAPP_OTG_DP_SM_H
#define PMAPP_OTG_DP_SM_H

/*! \file 
 *  \n
 *  \brief  pmapp_otg_DpStateMonitor.h ---- Dp line state monitor related class declaration
 *  \details This header file contains class and type declarations for USB DP line voltage monitoring
 *     state machine used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_DpStateMonitor.h#1 $

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

/*!\brief Enum for indicating the voltage condition of USB DP line state type
*/    
typedef enum
{
   OTG_DP_SM_INFO__LOW,                // D+ low interrupt
   OTG_DP_SM_INFO__HIGH,               // D+ high interrupt
   OTG_DP_SM_INFO__BDIS_ACON,          // bdis_acon interrupt
   OTG_DP_SM_INFO__CR_INT              // carkit interrupt
} OtgDpSmInfoType;


/*!\brief Typedef that defines the type of the listener function that needs to be 
*	provided by the client of debouncer state machine so as to get notified
*	about voltage transitions on DP line
*/    
typedef void (*OtgDpSmListenerType) (OtgDpSmInfoType, void*);




/*===========================================================================
                              CLASS DEFINITION
===========================================================================*/
/**************************************************************************
     class DpStateMonitor											  
**************************************************************************/
/*! \class DpStateMonitor 
*  \brief This class contains the necessary routines used to instansiate, reset 
*		USB DP line voltage monitoring debouncer state machine. 
*  \details This class has a method to get the current stable state of DP line via a callback function
*  \note none
*/
class DpStateMonitor: public UsbConnStateMonitor
{
public:
	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	DpStateMonitor();

	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~DpStateMonitor();

	/*! \brief Utility function used to create a singlton instance of DpStateMonitor object
	*  \details none
	*  \param none
	*  \return singlton instance of DmStateMonitor class object
	*  \note none
	*/
	static DpStateMonitor* GetInstance();

	/*! \brief Utility function used to initiate destruction of DmStateMonitor class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
    static void DestroyInstance();


	//initializes and sets up the DP line monitoring state machine.
	/*! \brief This function initializes and sets up the DP line monitoring debouncer state machine
	*  \details none
	*  \param 1. listener : Callback function of the client that will be used for notifying about
    *    voltage transitions on the DP line.
    *	- Type: OtgDpSmListenerType
    *	- Valid inputs: Valid non-NULL listener callback fn
	*  \param 2. pListenerObj : Variable that holds pointer to class object that listener callback
	*		function belongs
	*	- Type: void*
	*  \return none
	*  \note none
	*/
	void DpStateMonitorInit(OtgDpSmListenerType listener, void* pListenerObj);


	/*! \brief This function resets the DP line monitoring debouncer state machine.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void DpStateMonitorReset(void);

	/*! \brief Debouncer related fns for the D+ HI comparator
	*  \details This is the service routine for the debounced USB DP-HI line raising/falling-edge
    *		interrupt. This notifies its listener about the USB DP line condition.
	*  \param status :  Current status of USB DP-HI line
	*		- Type : IrqDebouncerStatusType
	*		- Possible values: OTG_DP_SM_INFO__HIGH,  OTG_DP_SM_INFO__LOW
	*  \param pStateObj : pointer to the class object instance to which DmHiStatusCb() belongs
	*		- Type : void*
	*  \return none
	*  \note none
	*/
	static void DpHiStatusCb(IrqDebouncerStatusType status, void* pStateObj);

protected:

private:
	/*! \brief Var that holds the current state of the USB DP line
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DpHiStatusCb()
	*/
	OtgDpSmInfoType mDpStateInfo;

	/*! \brief Var that holds callback function pointer used to notify the state of DP line status
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DpStateMonitorInit()
	*   \li DpHiStatusCb()
	*/
	OtgDpSmListenerType  mpOtgDpHiStateMonitorCb;

	/*! \brief Var that holds the class object to which the listener callback function belongs
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DpStateMonitorInit()
	*   \li DpHiStatusCb()
	*/
	void* mpOtgDpHiListenerObj;

	/*! \brief Var that holds the length of time in ms to wait in Dm debouncer BothIrqOff state
	*			before transitioning to CompIrqOn state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DpStateMonitor()
	*   \li DpStateMonitorInit()
	*/
	const int32  mDpHiDebouncerTmr_BothIrqOff;

	/*! \brief Var that holds the length of time in ms to wait in Dm debouncer CompIrqOn state
	*			before transitioning to Stablized state
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DpStateMonitor()
	*   \li DpStateMonitorInit()
	*/
	const int32  mDpHiDebouncerTmr_CompIrqOn;

	/*! \brief Var flag used to indicate if Dm state monitor class object have been initialized
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DpStateMonitorInit()
	*   \li DpStateMonitorReset()
	*/
	boolean mDpSmInitialized;

	/*! \brief This static variable is used to hold the singlton instance of class DpStateMonitor object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static DpStateMonitor* mpDpStateMonitor_Instance;

	/*! \brief pointer variable that holds pointer to the debouncer state manager object for Dp Hi Irq
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li DpStateMonitor()
	*   \li DpStateMonitorInit()
	*	\li DpStateMonitorReset()
	*/
	IrqDebouncerStateManager* mpDpIrqDbStateMgr;

};


#endif //FEATURE_PMIC_USB_OTG
#endif // PMAPP_OTG_DP_SM_H
       
