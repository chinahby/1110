#ifndef PMAPP_OTG_VBUS_SM_H
#define PMAPP_OTG_VBUS_SM_H

/*! \file 
 *  \n
 *  \brief  pmapp_otg_VbusStateMonitor.h   ----- Vbus State Monitor related class declaration
 *  \details This header file contains classes and typedefs declarations for USB VBUS 
 *      line voltage monitoring state machine used by the USB-OTG application
 *      developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_VbusStateMonitor.h#1 $

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
/*!\brief Enum used for indicating the voltage condition of the VBUS line
*  \details  
*/    
typedef enum
{
   OTG_VBUS_SM_INFO__VBUS_VALID,
   OTG_VBUS_SM_INFO__VBUS_INVALID,
   OTG_VBUS_SM_INFO__VBUS_SESSION_VALID,
   OTG_VBUS_SM_INFO__VBUS_SESSION_INVALID
} OtgVbusSmInfoType;


/*!\brief Enum type used for indicating the voltage condition of the VBUS line
*  \details  
*/    
typedef struct
{
   boolean vbus_valid;
   boolean vbus_session_valid;
} OtgVbusInfoType;



/*!\brief Type of the listener function that needs to be provided by the client of this
*   state machine so as to get notified about voltage transitions on VBUS line
*  \details  
*/
typedef void (*OtgVbusSmListenerType) (OtgVbusSmInfoType, void*);



/*===========================================================================
                              CLASS DEFINITION
===========================================================================*/
/**************************************************************************
     class VbusStateMonitor											  
**************************************************************************/
/*! \class VbusStateMonitor 
 *  \brief This class is used as a base class for class VbusValidStateMonitor 
 *    and class SessionValidStateMonitor. It inherets from base calss UsbConnStateMonitor
 *
 *  \details This class is used as a base class for VbusValidStateMonitor 
 *    and SessionValidStateMonitor classes which are used for USB VBUS line voltage monitoring
 *
 *  \note This class is just used as an interface and it is should not be insantiated
 *
  */
class VbusStateMonitor: public UsbConnStateMonitor
{
public:

protected:
     /*! \brief A contructor.
     *  \details This constructor initializes member variables
     */
	VbusStateMonitor();

     /*! \brief A destructor.
     */
	virtual ~VbusStateMonitor();
private:

};



/**************************************************************************
     class VbusStateMonitor											  
**************************************************************************/
/*! \class VbusValidStateMonitor 
 *  \brief This class is used to initiate and monitor usb Vbus valid state. 
 *         It inherets from base calss VbusStateMonitor
 *
 *  \details This class has methods to initiate/reset/listen the VBus valid IRQ state debouncer .
 *     When debouncer detects VBus valid RE/FE event, it gets a call back from debouncer layer 
 *     and it notifies this event to pm app otg layer via a call back function.
 *
 *  \note none
 *
 */
class VbusValidStateMonitor: public VbusStateMonitor
{
public:
     /*! \brief A contructor.
     *  \details This constructor initializes member variables
     */
	VbusValidStateMonitor();

     /*! \brief A destructor.
     */
	virtual ~VbusValidStateMonitor();

     /*! \brief This member function returns to the instance of VbusValidStateMonitor class object
     *  \details VbusValidStateMonitor is a singlton object, this method ensures that there is only one
     *    instance of this calss object.
     *  \param none
     *  \return VbusValidStateMonitor
     *  \note to get instance of this class object, this method must be called
     */
	static VbusValidStateMonitor* GetInstance();

    /*! \brief This member function destroys instance of VbusValidStateMonitor class object
    */
    static void DestroyInstance();

     /*!\brief This member function initializes Vbus valid monitoring debouncer state machine
     *  \details This member function initializes usb vbus valid IRQ state monitor debouncer 
     *  \param listener : call backfunction that is used to call when vbus valid IRQ event happens
     *  \param pListenerObj:  the class object that the listener call back function is defined
     *  \return none
     *  \note  Before calling this function, the following function must have been called:
     *  \li rflib_init()
     *  \li pm_init()     
     *  \note This function can be called any time there is a need to monitor USB vbus line.
     */
    void VbusValidStateMonitorInit(OtgVbusSmListenerType listener, void* pListenerObj);

     /*!\brief This member function resets Vbus valid monitoring debouncer state machine
     *  \details This member function resets usb vbus valid IRQ state monitor debouncer 
     *  \return none
     *  \note This function can be called any thime there is no need to monitor USB vbus line.
     */
	void VbusValidStateMonitorReset(void);

     /*!\brief This member function is used as a call back function when there is VBus valid event happens
     *  \details This member function is used as a call back function when usb vbus valid IRQ 
     *   state monitor debouncer detects a state change.
     *  This is the service routine for the "VBUS valid raising-edge" interrupt.
     *   It clears the interrupt and notifies its listener that the "VBUS_Valid"
     *   condition has become TRUE.          
     *  \param status :   current state of vbus valid IRQ event
     *  \param pStateObj:  class object where this member function is defined
     *  \return none
     *  \note This function is only called by vbus valid debouncer state machine
     */    
	static void VbusValidStatusCb(IrqDebouncerStatusType status, void* pStateObj);

protected:

private:
   /*! \brief Pointer that holds the otg layer VbusValid call back function
   *   \details This variable will be used/updated by the following member function(s): 
   *   \li VbusValidStateMonitor()
   *   \li VbusValidStateMonitorInit()
   *   \li VbusValidStatusCb()
   */
	OtgVbusSmListenerType  mpOtgVbusValidStateMonitorCb;

    /*! \brief Pointer that holds class object of the otg layer VbusValid call back function
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li VbusValidStateMonitor()
    *   \li VbusValidStateMonitorInit()
    *   \li VbusValidStatusCb()
    */
	void* mpOtgVbusValidListenerObj;

    /*! \brief Pointer that holds VbusValid debouncer State manager object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li VbusValidStateMonitor()
    *   \li VbusValidStateMonitorInit()
    *   \li VbusValidStateMonitorReset()
    */
	IrqDebouncerStateManager* mpVbusValidIrqDbStateMgr;  //ptr to Deboucer state manager for VBUS Valid Irq

    /*! \brief Holds the duration, in ms, where VbusValid IRQ debouncer state machine stays in BothIrqOff state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li VbusValidStateMonitorInit()
    */
	const int32 mOtgVbusValid_DebouncerTmr_BothIrqOff;

    /*! \brief Holds the duration, in ms, where VbusValid IRQ debouncer state machine stayes in CompIrqOn state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li VbusValidStateMonitorInit()
    */
    const int32 mOtgVbusValid_DebouncerTmr_CompIrqOn;

    /*! \brief Flag used to indicate if the VbusValid state monitor is initalized
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li VbusValidStateMonitorInit()
    */
    boolean  mOtgVbusValidSmInitialized;

    /*! \brief Static pointer used to hold the only instantiation of VbusValidStateMonitor object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetInstance()
    */
	static VbusValidStateMonitor* mpVbusValidStateMonitor_Instance;

};



/**************************************************************************
     class SessionValidStateMonitor											  
**************************************************************************/
/*! \class SessionValidStateMonitor 
 *  \brief This class is used to initiate and monitor usb Vbus session valid state. 
 *         It inherets from base calss VbusStateMonitor
 *
 *  \details This class has methods to initiate/reset/listen the VBus session valid IRQ state debouncer .
 *     When debouncer detects VBus valid RE/FE event, it gets a call back from debouncer layer 
 *     and it notifies this event to pm app otg layer via a call back function.
 *
 *  \note none
 *
 */
class SessionValidStateMonitor: public VbusStateMonitor
{
public:
     /*! \brief A contructor.
     *  \details This constructor initializes member variables
     */
	SessionValidStateMonitor();

     /*! \brief A destructor.
     */
	virtual ~SessionValidStateMonitor();

     /*! \brief This member function returns to the instance of VbusValidStateMonitor class object
     *  \details VbusValidStateMonitor is a singlton object, this method ensures that there is only one
     *    instance of this calss object.
     *  \param none
     *  \return VbusValidStateMonitor
     *  \note to get instance of this class object, this method must be called
     */
	static SessionValidStateMonitor* GetInstance();

    /*! \brief This member function returns to the instance of VbusValidStateMonitor class object
    */
    static void DestroyInstance();

     /*! \brief This member function initializes Vbus session valid monitoring debouncer state machine
     *  \details This member function initializes usb vbus session valid IRQ state monitor debouncer 
     *  \param listener : call backfunction that is used to call when the vbus session valid event happens
     *  \param pListenerObj:  the class object that the listener call back function is defined
     *  \return none
     *  \note  Before calling this function, the following function must have been called:
     *  \li rflib_init()
     *  \li pm_init()     
     *  \note This function can be called any thime there is a need to monitor USB vbus line.
     */
	void SessionValidStateMonitorInit(OtgVbusSmListenerType listener, void* pListenerObj);

     /*!\brief This member function resets Vbus session valid monitoring debouncer state machine
     *  \details This member function resets USB Vbus session valid IRQ state monitor debouncer 
     *  \return none
     *  \note This function can be called any thime there is no need to monitor USB vbus line.
     */
	void SessionValidStateMonitorReset(void);

	//Debouncer related fns for the VBUS_Session_Valid comparator
     /*!\brief This member function is used as a call back function when there is VBus session valid event happens
     *  \details This member function is used as a call back function when USB VBus session  valid IRQ 
     *   state monitor debouncer detects a state change.
     *  This is the service routine for the "VBUS session valid raising-edge" interrupt.
     *   It clears the interrupt and notifies its listener that the "VBUS_Session_Valid"
     *   condition has become TRUE.     
     *  \param status :   current state of vbus session valid IRQ event
     *  \param pStateObj: class object pointer where this member function is defined
     *  \return none
     *  \note This function is only called by vbus session valid debouncer state machine
     */        
	static void SessionValidStatusCb(IrqDebouncerStatusType status, void* pStateObj);

protected:

private:

   /*! \brief Pointer that holds the otg layer SessionValid call back function
   *   \details This variable will be used/updated by the following member function(s): 
   *   \li SessionValidStateMonitor()
   *   \li SessionValidStateMonitorInit()
   *   \li SessionValidStatusCb()
   */
	OtgVbusSmListenerType mpOtgSessionValidStateMonitorCb;

   /*! \brief Pointer that holds class object of the otg layer VbusValid call back function
   *   \details This variable will be used/updated by the following member function(s): 
   *   \li SessionValidStateMonitor()
   *   \li SessionValidStateMonitorInit()
   *   \li SessionValidStatusCb()
   */
	void* mpOtgSessionValidListenerObj;

    /*! \brief Pointer used to hold SessionValid debouncer State manager object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li SessionValidStateMonitor()
    *   \li SessionValidStateMonitorInit()
    *   \li SessionValidStateMonitorReset()    
    */
	IrqDebouncerStateManager* mpSessionValidIrqDbStateMgr;  //ptr to Deboucer state manager for VBUS Session Valid Irq

    /*! \brief Holds the the duration, in ms, where SessionValid IRQ debouncer state machine stays in BothIrqOff state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li SessionValidStateMonitorInit()
    */
    const int32 mOtgSessionValid_DebouncerTmr_BothIrqOff;

    /*! \brief Holds the the duration, in ms, where SessionValid IRQ debouncer state machine stays in BothIrqOn state
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li SessionValidStateMonitorInit() 
    */
	const int32 mOtgSessionValid_DebouncerTmr_CompIrqOn;

    /*! \brief Flag used to indicate if the SessionValid state monitor is initalized
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li SessionValidStateMonitorInit() 
    */
	boolean  mOtgSessionValid_SmInitialized;

    /*! \brief Static pointer used to hold the only instantiation of VbusValidStateMonitor object
    *   \details This variable will be used/updated by the following member function(s): 
    *   \li GetInstance()
    */
	static SessionValidStateMonitor* mpSessionValidStateMonitor_Instance;
};


#endif //FEATURE_PMIC_USB_OTG
#endif // PMAPP_OTG_VBUS_SM_H
