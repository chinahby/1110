#ifndef PMAPP_OTG_STATEMONITOR_H
#define PMAPP_OTG_STATEMONITOR_H

/*! \file 
 *  \brief  pmapp_otg_StateMonitor.h ----- PM APP OTG State Monitor related class declaration
 *  \details This file contains class declaration for USB state monitor used by the USB-OTG 
 *     application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_StateMonitor.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
                            INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pmapp_IrqDebouncer.h"
extern "C"
{
   #include "assert.h"          /*!< For ASSERT() */
}

#ifdef FEATURE_PMIC_USB_OTG


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
/*!\brief Enum used for usb line debouncer state monitor
*  \details  
*/    
typedef enum
{
   VBUS_VALID_DEBOUNCER_STATE_MONITOR,
   VBUS_SESSION_VALID_DEBOUNCER_STATE_MONITOR,
   ID_FLT_DEBOUNCER_STATE_MONITOR,
   ID_GND_DEBOUNCER_STATE_MONITOR,
   DP_HI_DEBOUNCER_STATE_MONITOR,
   DM_HI_DEBOUNCER_STATE_MONITOR,
   DEBOUNCER_STATE_MONITOR_UNKNOWN
} UsbLineDebouncerStateMonitorType;




/*===========================================================================
                              CLASS DEFINITION
===========================================================================*/


/**************************************************************************
     class StateMonitorFactory											  
**************************************************************************/

/*! \class StateMonitorFactory 
*  \brief  This class is used to inforce a single instance of DebouncerStateManager class
*			for each usb lines Irq
*  \details This class maintanins pointers to:
*		-	VbusValid debouncer State Manager
*		-	VbusSessValid debouncer State Manager
*		-	Id float debouncer State Manager
*		-	Id Gnd debouncer State Manager
*		-	D+ Hi debouncer State Manager
*		-	D- Hi debouncer State Manager
*		
*  \note There should only be a single instance of StateMonitorFactory class. 
*			Instance of this object can be acquired via GetInstance()
*/
class StateMonitorFactory  
{
public:
     /*! \brief Contructor.
     *  \details none
      */
	StateMonitorFactory();

     /*! \brief Destructor
     *  \details none
      */
	~StateMonitorFactory();

	/*! \brief This member function returns instance of StateMonitorFactory class
	*  \details StateMonitorFactory is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of StateMonitorFactory class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static StateMonitorFactory* GetInstance();

    /*! \brief This member function destroys instance of StateMonitorFactory class object
    *  \details 
    */
    static void DestroyInstance();

   /*! \brief Used to creat/return singlton instance of IrqDebouncerStateManager object
   *			based on the UsbLineDebouncerStateMonitorType parameter passed
   *  \details none
   *  \param stateManagerId;  Type: UsbLineDebouncerStateMonitorType
   *  \return Type: IrqDebouncerStateManager
   */
    static IrqDebouncerStateManager* GetIrqDebouncerStateManager(UsbLineDebouncerStateMonitorType stateManagerId);

protected:

private:
    /*! \brief Static pointer used to hold the only instantiation of StateMonitorFactory object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetInstance()
    */
	static StateMonitorFactory* mpStateMonitorFactory_Instance;

    /*! \brief Static pointer used to hold the only instantiation of IrqDebouncerStateManager object
    *    that monitors Vbus Valid IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetIrqDebouncerStateManager()
    */
	static IrqDebouncerStateManager* mpDbVbusValidStateMgr;

    /*! \brief Static pointer used to hold the only instantiation of IrqDebouncerStateManager object
    *    that monitors Vbus Session Valid IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetIrqDebouncerStateManager()
    */
	static IrqDebouncerStateManager* mpDbVbusSessValidStateMgr;

    /*! \brief Static pointer used to hold the only instantiation of IrqDebouncerStateManager object
    *    that monitors ID floating IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetIrqDebouncerStateManager()
    */
	static IrqDebouncerStateManager* mpDbIdFltStateMgr;

    /*! \brief Static pointer used to hold the only instantiation of IrqDebouncerStateManager object
    *    that monitors ID ground IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetIrqDebouncerStateManager()
    */
	static IrqDebouncerStateManager* mpDbIdGndStateMgr;

    /*! \brief Static pointer used to hold the only instantiation of IrqDebouncerStateManager object
    *    that monitors D+ High IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetIrqDebouncerStateManager()
    */
	static IrqDebouncerStateManager* mpDbDpHiStateMgr;

    /*! \brief Static pointer used to hold the only instantiation of IrqDebouncerStateManager object
    *    that monitors D- High IRQ
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetIrqDebouncerStateManager()
    */
	static IrqDebouncerStateManager* mpDbDmHiStateMgr;

};



/**************************************************************************
     class UsbConnStateMonitor											  
**************************************************************************/
/*! \class UsbConnStateMonitor 
 *  \brief  USB state monitor base calss
 *  \details Serves as a base class to classes:
 *		-  VbusStateMonitor
 *		-  DpStateMonitor
 *		-  DmStateMonitor
 *		-  IdStateMonitor
 *  \note none
 */
class UsbConnStateMonitor
{
public:
    /*! \brief Pointer that holds debouncer state monitor factor object, StateMonitorFactory
    *   \details used by all children classes
    */
	StateMonitorFactory* mpDbStateMonitorFactory;
protected:
     /*! \brief Contructor
     *  \details 
     */
	UsbConnStateMonitor();

     /*! \brief Destructor
     *  \details 
     */
	~UsbConnStateMonitor(); 

private:

};


#endif //FEATURE_PMIC_USB_OTG
#endif // PMAPP_OTG_STATEMONITOR_H
