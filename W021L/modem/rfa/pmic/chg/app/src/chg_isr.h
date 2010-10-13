#ifndef CHG_ISR_H
#define CHG_ISR_H

/*! \file 
 *  \n
 *  \brief  chg_isr.h ---- PMIC IRQ RELATED CLASS DECLARATION
 *  \details This header file contains class declarations of the IrqEvent 
 *  class and the IrqEventGenerator class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_isr.h#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   hs      Removed unnecessary header file inclusion.
06/60/09   hs      Integrated the vbatt task interface for the battery alarm
                   status.
06/05/09   hs      Added support for software controlled CV topoff.
05/07/09   hs      Removed unnecessary header file inclusion.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
11/13/08   hs      Added support for QSC1100.
                   Updated the documentation.
09/29/08   vk      Added Add/Remove listener handle battery removal case
06/25/08   hs      Added support for autonomous charging.
05/07/08   hs      Removed the usage of std::list 
                   Added FEATURE_CHG_TASK.
04/16/08   hs      Name changes for some member functions.
07/11/07   hs      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#include "chg_evt.h"    /* For classes ChargingEventListener,            *
                         * and ChargingEvent definition                  */
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
class ChargingFactory;
class PowerControlManager;
class CurrentControlManager;
/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/
                          /***************************/
                          /*     class IrqEvent      */
                          /***************************/

/*! \class IrqEvent chg_isr.h "chg/chg_isr.h"
 *  \brief This is the class that defines the interface for the charging related
 *   pmic IRQ events in the Charging Application
 *  \details This is the class that defines the interface for the charging related
 *   pmic IRQ events in the Charging Application. This class is a child class of
 *  the ChargingEvent class.
 *  \note The instances of this class would be generated and destroyed, at runtime,
 *  by a IrqEventGenerator class, when a charging related PMIC IRQ triggers.
 *
 *  <b>Handles the following</b>
 *  \n None
 *
 *  <b>Listens for the following events</b>
 *  \n None
 *
 *  <b>Generates the following events</b>
 *  \n None
 */
class IrqEvent: public ChargingEvent
{
public:
    /*! \brief This member function returns the PMIC IRQ ID
     *  \param none 
     *  \return An enum that contains the PMIC IRQ ID.
     *  \note This function should be called in the following scenario:
     *  \li By the charger task, in order to put the correct information into 
     *  the task command queue.
     *  \li By a IrqEventGenerator class, in order to inform the IRQ event
     *  to the listeners that are registered to this IRQ event, instead of
     *  broadcasting it to all the listeners.
     *  \li By a ChargingEventListener class, in order to decide if the IRQ
     *  event is one of the events that this class should handle. 
     */
    pm_irq_hdl_type GetIrqId()
    {
        return mIrqId;
    }
protected:
    /*! \brief The IRQ ID 
     *  \details This ID is used to differentiate different IRQ events. This variable 
     *  should be set only when a new IRQ event is created.
     */
    pm_irq_hdl_type    mIrqId;
public:
    /*! \brief A contructor
     *  \details In this contructor, the event ID is set to ::CHG_CMD__IRQ_TRIGGERED;
     *  and the IRQ ID is set by the parameter.
     *  \param irqid The IRQ ID.
     *  \return NA
     */
    IrqEvent(pm_irq_hdl_type irqid); 
    /*! \brief A destructor
     *  \param none
     *  \return NA
     */
    virtual ~IrqEvent();
    /*! \brief friend class ChargingFactory.
     *  Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
}; /* end of class IrqEvent */

                  /*************************************/
                  /*     class IrqEventGenerator       */
                  /*************************************/

/*! \class IrqEventGenerator chg_isr.h "chg/chg_isr.h"
 *  \brief This is the class that defines the interface for managing the charging
 *  related PMIC IRQs
 *  \details This is the class that defines the interface for managing all the charging
 *  related PMIC IRQs, including adding/removing listeners for the specific IRQs, 
 *  enabling/disabling IRQs, and creating IRQ events, etc.
 *  \note The charging events generated by this class should be in the \b interrupt  
 *  context, instead of in the task context.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It is generated and destroyed by the ChargingFactory class.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>Managing PMIC interrupts</td>
 *          <td> In the following ways:
 *               \li Providing a generic Interrupt Service Routine.
 *               \li Providing specific Interrupt Service Routines.
 *               \li Providing interface for using various charging related IRQs.</td>
 *      </tr>
 *      <tr><td>Publishing new PMIC IRQ charging events</td>
 *          <td>By using the ChargingEventListener interface.</td>
 *      </tr>
 *      <tr><td>Monitoring the listener information</td>
 *          <td>By maintaining the listener list.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  \n None
 *
 *  <b>Generates the following events</b>
 *  <table border="0">
 *      <tr><td>::CHG_CMD__IRQ_TRIGGERED</td>
 *          <td>isr() and batteryAlarmIsr() generate this.</td>
 *      </tr>
 *  </table>
 */
class IrqEventGenerator
{
public:
    /*! \brief This member function fires the specific IRQ event, passed by the
     *  parameter, to its listeners 
     *  \param event A IRQ charging event that needs to be handled.
     *  \return void
     *  \note This function should be only called when a new IRQ charging event of type 
     *  IrqEvent is created.
     */ 
    void FireEvent(ChargingEvent *event);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the valid charger IRQ (::PM_VALID_CHG_IRQ_HDL) and
     *  for the valid usb charger IRQ (::PM_VALID_USB_CHG_IRQ_HDL)
     *  \param listener A listener that wants to gets informed when the valid charger
     *  IRQ or valid usb charger IRQ triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  charger hardware is attached to the phone.
     */ 
    void AddListenerForValidChargerIRQ(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for the valid charger IRQ (::PM_VALID_CHG_IRQ_HDL) and
     *  for the valid usb charger IRQ (::PM_VALID_USB_CHG_IRQ_HDL)
     *  \param listener A listener that no longer wants to gets informed when the valid 
     *  charger IRQ or valid usb charger IRQ triggers.
     *  \return void
     *  \note This function should be called when a class  no longer wants to be informed 
     *  when a  charger hardware is attached to the phone.
     */ 
    void RemoveListenerForValidChargerIRQ(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the invalid charger IRQ (::PM_INVALID_CHG_IRQ_HDL) and
     *  invalid usb charger IRQ (::PM_INVALID_USB_CHG_IRQ_HDL)
     *  \param listener A listener that wants to gets informed when the invalid charger
     *  IRQ or the invalid usb charger IRQ triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  charger hardware is removed from the phone.
     */ 
    void AddListenerForInvalidChargerIRQ(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for the invalid charger IRQ (::PM_INVALID_CHG_IRQ_HDL) and
     *  invalid usb charger IRQ (::PM_INVALID_USB_CHG_IRQ_HDL)
     *  \param listener A listener that no longer wants to gets informed when the invalid 
     *  charger RQ or the invalid usb charger IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when a charger hardware is removed from the phone.
     */ 
    void RemoveListenerForInvalidChargerIRQ(ChargingEventListener *listener);
#if defined(FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the battery larm IRQ (::PM_BAT_STAT_CHANGED_IRQ_HDL)
     *  \param listener A listener that wants to gets informed when the battery alarm
     *  IRQ triggers.
     *  \param vmax The battery alarm upper threshold.
     *  \param vmin The battery alarm lower threshold.
     *  \return Error flag.
     *  \note This function should be called when a class wants to be informed when the 
     *  the battery alarm IRQ triggers.
     */ 
    pm_err_flag_type AddListenerForBatteryAlarmIrq(ChargingEventListener  *listener,
                                                  unsigned short          vmax,
                                                  unsigned short          vmin);
    /*! \brief This member function provides an interface for removing the specific 
     * listener, passed by the parameter, for the battery alarm IRQ 
     *  (::PM_BAT_STAT_CHANGED_IRQ_HDL) 
     *  \param listener A listener that no longer wants to gets informed when the  
     *  battery alarm IRQ triggers.
     *  \return Error flag.
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the the battery alarm IRQ triggers.
     */ 
    pm_err_flag_type RemoveListenerForBatteryAlarmIrq(ChargingEventListener *listener);
#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF || FEATURE_CHG_TASK_USE_CV_TOPOFF */
#if defined( FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(T_WINNT)
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the pulse charging done IRQ 
     *  (::PM_PULSE_CHG_DONE_IRQ_HDL)
     *  \param listener A listener that wants to gets informed when the pulse charging
     *  done triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  the pulse charging done triggers.
     */ 
    pm_err_flag_type AddListenerForPulseTDoneIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific 
     *  listener, passed by the parameter, for the pulse charging done IRQ
     *  (::PM_PULSE_CHG_DONE_IRQ_HDL)
     *  \param listener A listener that no longer wants to gets informed when the pulse 
     *  charging done IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the the pulse charging done IRQ triggers.
     */ 
    void RemoveListenerForPulseTDoneIrq(ChargingEventListener *listener);
#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF */
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the End-Of-Charging IRQ (::PM_CHGDONE_IRQ_HDL)
     *  in automatic charging solution
     *  \param listener A listener that wants to gets informed when the end-of-charging
     *  IRQ triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  end-of-charging IRQ triggers in the automatic charging solution.
     */ 
    pm_err_flag_type AddListenerForEndOfChargingIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific 
     *  listener, passed by the parameter, for the End-Of-Charging IRQ (::PM_CHGDONE_IRQ_HDL)
     *  in automatic charging solution
     *  \param listener A listener that no longer wants to gets informed when the  
     *  end-of-charging IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the end-of-charging IRQ triggers in the automatic charging solution.
     */ 
    void RemoveListenerForEndOfChargingIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the Fail-Of-Charging IRQ (::PM_CHGFAIL_IRQ_HDL)
     *  in automatic charging solution
     *  \param listener A listener that wants to gets informed when the fail-of-charging
     *  IRQ triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  fail-of-charging IRQ triggers in the automatic charging solution.
     */ 
    pm_err_flag_type AddListenerForFailOfChargingIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific 
     *  listener, passed by the parameter, for the Fail-Of-Charging IRQ 
     *  (::PM_CHGFAIL_IRQ_HDL) Lin automatic charging solution
     *  \param listener A listener that no longer wants to gets informed when the  
     *  fail-of-charging IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the fail-of-charging IRQ triggers in the automatic charging solution.
     */ 
    void RemoveListenerForFailOfChargingIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the Change-Of-State IRQ (::PM_CHGSTATE_IRQ_HDL)
     *  in automatic charging solution
     *  \param listener A listener that wants to gets informed when the change-of-state
     *  IRQ triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  change-of-state IRQ triggers in the automatic charging solution.
     */ 
    pm_err_flag_type AddListenerForChangeOfStateIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific 
     *  listener, passed by the parameter, for the Change-Of-State IRQ 
     *  (::PM_CHGSTATE_IRQ_HDL) Lin automatic charging solution
     *  \param listener A listener that no longer wants to gets informed when the  
     *  change-of-state IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the change-of-state IRQ triggers in the automatic charging solution.
     */ 
    void RemoveListenerForChangeOfStateIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the Bad-Batt-Temp IRQ (::PM_BATTTEMP_IRQ_HDL)
     *  in automatic charging solution
     *  \param listener A listener that wants to gets informed when the bad-battery-temp
     *  IRQ triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  battery IRQ triggers in the automatic charging solution.
     */ 
    pm_err_flag_type AddListenerForBattTempIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific 
     *  listener, passed by the parameter, for the Bad-Batt-Temp IRQ 
     *  (::PM_BATTTEMP_IRQ_HDL) in automatic charging solution
     *  \param listener A listener that no longer wants to gets informed when the  
     *  Bad-Batt-Temp IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the battery IRQ triggers in the automatic charging solution.
     */ 
    void RemoveListenerForBattTempIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the VBATDET IRQ (::PM_VBAT_DET_IRQ_HDL)
     *  in automatic charging solution
     *  \param listener A listener that wants to gets informed when the VBATDET IRQ
     *  triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  VBATDET IRQ triggers in the automatic charging solution.
     */ 
    pm_err_flag_type AddListenerForVbatdetIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific 
     *  listener, passed by the parameter, for the VBATDET IRQ (::PM_VBAT_DET_IRQ_HDL) 
     *  in automatic charging solution
     *  \param listener A listener that no longer wants to gets informed when the  
     *  VBATDET IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the VBATDET IRQ triggers in the automatic charging solution.
     */ 
    void RemoveListenerForVbatdetIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the Fast Charge IRQ (::PM_FASTCHG_IRQ_HDL)
     *  in automatic charging solution
     *  \param listener A listener that wants to gets informed when the in-fast-charge
     *  IRQ triggers.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  in-fast-charge IRQ triggers in the automatic charging solution.
     */ 
    pm_err_flag_type AddListenerForFastChargeIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific 
     *  listener, passed by the parameter, for the in-fast-charge IRQ (::PM_FASTCHG_IRQ_HDL) 
     *  in automatic charging solution
     *  \param listener A listener that no longer wants to gets informed when the  
     *  in-fast-charge IRQ triggers.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed  
     *  when the in-fast-charge IRQ triggers in the automatic charging solution.
     */ 
    void RemoveListenerForFastChargeIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the valid charger IRQ (::PM_VALID_CHG_IRQ_HDL) and
     *  for the valid usb charger IRQ (::PM_VALID_USB_CHG_IRQ_HDL)
     *  \param listener A listener that wants to get informed when battery connect/disconnect
     *  IRQ gets triggered.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  battery hardware is removed from the phone.
     */ 
    pm_err_flag_type AddListenerForBattConnectIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for the valid charger IRQ (::PM_VALID_CHG_IRQ_HDL) and
     *  for the valid usb charger IRQ (::PM_VALID_USB_CHG_IRQ_HDL)
     *  \param listener A listener that no longer wants to get informed when battery 
     *  is removed.
     *  \return void
     *  \note This function should be called when a class  no longer wants to be informed 
     *  when a  battery is attached to the phone.
     */ 
    void RemoveListenerForBattConnectIrq(ChargingEventListener *listener);
#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the VCP (Voltage Collapse Protection) IRQ 
     *  (::PM_VCP_IRQ_HDL)
     *  \param listener A listener that wants to get informed when the VCP IRQ gets triggered.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  VCP happens.
     */ 
    pm_err_flag_type AddListenerForVcpIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for the VCP (Voltage Collapse Protection) IRQ 
     *  (::PM_VCP_IRQ_HDL) 
     *  \param listener A listener that no longer wants to get informed when the VCP happens 
     *  \return void
     *  \note This function should be called when a class  no longer wants to be informed 
     *  when the VCP happens.
     */ 
    void RemoveListenerForVcpIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the charger-in-temperature_limiting IRQ 
     *  (::PM_CHGTLIMIT_IRQ_HDL)
     *  \param listener A listener that wants to get informed when the charger is in 
     *  temperature limiting.
     *  \return void
     *  \note This function should be called when a class wants to be informed when the 
     *  charger is in temperature limiting.
     */ 
    pm_err_flag_type AddListenerForChgTLimitIrq(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for the charger-in-temperature-limiting IRQ 
     *  (::PM_CHGTLIMIT_IRQ_HDL) 
     *  \param listener A listener that no longer wants to get informed when the charger is
     *  in temperature limiting  
     *  \return void
     *  \note This function should be called when a class  no longer wants to be informed 
     *  when the charger is in temperature limiting.
     */ 
    void RemoveListenerForChgTLimitIrq(ChargingEventListener *listener);
#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */
#if defined(FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
#ifdef FEATURE_VBATT_TASK
    /*! \brief This member function provides an interface for getting informed by the VBATT 
     *  task when the battery alarm upper threshold reached 
     *  \param void  
     *  \return void
     *  \note This function should only be called when the VBATT task is running and 
     *  the battery alarm upper threshold reached.
     */ 
    void VbattUpperThresholdCallback( void );
    /*! \brief This member function provides an interface for getting informed by the VBATT 
     *  task when the battery alarm lower threshold reached 
     *  \param void  
     *  \return void
     *  \note This function should only be called when the VBATT task is running and
     *  the battery alarm lower threshold reached.
     */ 
    void VbattLowerThresholdCallback( void );
#endif /* FEATURE_VBATT_TASK */
#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF || FEATURE_CHG_TASK_USE_CV_TOPOFF */
    /*! \brief This member function returns the battery alarm status when the battery
     *  alarm IRQ triggers
     *  \param none
     *  \return The battery alarm status.
     *  \retval ::CHG_VBAT_TH_NONE False battery alarm
     *  \retval ::CHG_VBAT_TH_LOWER The battery alarm lower threshold reached
     *  \retval ::CHG_VBAT_TH_UPPER The battery alarm upper threshold reashed
     *  \note This function should be called when there is a need to know the battery
     *  alarm status.
     */
    chg_vbat_threshold_type GetBatteryAlarmStatus()
    {
        return mBatteryAlarmStatus;
    }
protected:
    /*! \struct IrqListenerInfo
     *  \brief Used to store all information about the event listener
     *  \details A structure for holding the irq informatmion of the listener.
     *  \note The information stored in this type is part of the ink information for
     *  each listener.
     */
    typedef struct
    {
        /*! Event listener                               */
        ChargingEventListener          *mpListener; 
        /*! IRQ ID                                       */
        pm_irq_hdl_type                 mIrq;  
    }IrqListenerInfo;
    /*! \struct IrqListenerLinkInfo
     *  \brief Used to store the link information about the event listener
     *  \details A structure for holding the link informatmion of the listener.
     *  \note this type is used to store listener information in the listener queue.
     */
    typedef struct
    {
        /*! Queue link.                                  */
        q_link_type                     mLink;    
        /*! Event listener                               */
        ChargingEventListener          *mpListener; 
        /*! IRQ ID                                       */
        pm_irq_hdl_type                 mIrq;  
    }IrqListenerLinkInfo;
// Member Functions
protected:
    /*! \brief This member function adds the specific listener 
     *  \details This member function adds the specific listener, with its information,
     *  passed by the parameter, to the IRQ event generator's listener list.
     *  \param listener A listener that wants to be informed when a PMIC IRQ triggers.
     *  \param irqid The IRQ ID that the specific listener wants to listen to.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  specific PMIC IRQ triggers.
     */ 
    void addListener(
        ChargingEventListener *listener,
        pm_irq_hdl_type       irqid);
    /*! \brief This member function removes the specific listener 
     *  \details This member function removes the specific listener, with its information,
     *  passed by the parameter, from the event generator's listener list.
     *  \param listener A listener that no longer wants to be informed when the specific 
     *  PMIC IRQ triggers.
     *  \param irqid The IRQ ID that the specific listener no longer wants to listen to.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed when the
     *  specific IRQ triggers.
     */ 
    void removeListener(
        ChargingEventListener *listener,
        pm_irq_hdl_type       irqid);
    /*! \brief This member function returns a buffer from the listener free queue 
     *  \param void
     *  \return a pointer to the buffer
     *  \note The returned buffer may be NULL if there were no free listener buffers
     *  available. 
     */ 
    IrqListenerLinkInfo* getListenerBuffer(void);
    /*! \brief This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue 
     *  \details This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue. This function is used  
     *  to determine if an item is in the queue by comparing the IRQ ID as well as 
     *  the listener object. 
     *  \param item A pointer to the comparing item.
     *  \param compare_val A pointer to the compared value.
     *  \return No-zero is the item is in the queue, zero otherwise
     *  \note This function compares the IRQ ID as well as the listener object.
     */ 
    static int listenerInformationCompare(void* item, void* compare_val);
    /*! \brief This static member function is the ISR when a charging related PMIC IRQ
     *  triggers 
     *  \param data Useful information, such as the IRQ ID, associated with this interrupt.
     *  \return void
     *  \note This function should be called when a charging related PMIC IRQ(excluding the
     *  battery alarm IRQ) triggers.
     */ 
    static void isr(void* data);
#if defined(FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
#ifndef FEATURE_VBATT_TASK
private:
    /*! \brief This static member function is the ISR when the battery alarm IRQ triggers. 
     *  \details This function is the battery threshold Interrupt Service Routine.
     *  It gets called if the battery voltage goes outside the range established
     *  by memberfunction AddListenerToBatteryAlarmIrq(). 
     *  \param data Useful information associated with this interrupt.
     *  \return void
     *  \note This function should be called when the VBATT task is not running and the battery 
     *  alarm IRQ triggers.
     */ 
    static void batteryAlarmIsr(void* data);
#endif /* FEATURE_VBATT_TASK */
#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF || FEATURE_CHG_TASK_USE_CV_TOPOFF */
protected:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    IrqEventGenerator();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~IrqEventGenerator();
protected:
    /*! \brief The listener buffer 
     */
    IrqListenerLinkInfo  mListenerBuffer[CHG_IRQ_LISTENER_BUF_CNT ];
    /*! \brief The listener queue 
     *  \details This listener queue contains the listener information for each  
     *  individual listeners.
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s):  
     *  \li addListener()
     *  \li removeListener()
     */
    q_type               mListenerQueue;
    /*! \brief The listener free queue 
     *  \details This listener queue contains the unused listener buffers
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s): 
     *  \li addListener()
     *  \li removeListener()
     */
    q_type               mListenerFreeQueue;
    /*! \brief The battery alarm status 
     *  \details This membervariable stores the current battery alarm status.
     *  \li ::CHG_VBAT_TH_NONE False battery alarm
     *  \li ::CHG_VBAT_TH_LOWER The battery alarm lower threshold reached
     *  \li ::CHG_VBAT_TH_UPPER The battery alarm upper threshold reached
     *  \note This variable should be updated by the following memberfunctions: 
     *  \li batteryAlarmIsr()
     */
    chg_vbat_threshold_type         mBatteryAlarmStatus;

    /*! \brief friend class ChargingFactory
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
}; /* end class IrqEventGenerator */

#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif /* CHG_ISR_H */

