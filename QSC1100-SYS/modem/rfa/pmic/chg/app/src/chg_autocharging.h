#ifndef CHG_AUTO_CHARGING_H
#define CHG_AUTO_CHARGING_H

/*! \file 
 *  \n
 *  \brief  chg_autocharging.h ---- AUTOMATIC CHARGING CLASS DECLARATION
 *  \details This header file contains class declarations of the AutoChargingManager 
 *  class.
 *  \n
 *  \n &copy; Copyright 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_autocharging.h#16 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/09   hs      Added support for software controlled CV topoff.
05/26/09   hs      Created two child classes to support pulse charging.
04/23/09   hs      Addes support for the resume-charging detection.
04/03/09   hs      Added support for unit tests.
03/27/09   hs      Updated the documentation.
03/11/09   hs      Added support for end-of-charging workaround.   
02/11/09   hs      Added pulse charging support
                   Updated the documentation for pulse charging.
01/27/09   hs      Code clean up.
12/17/08   hs      Replaced chg_auto_charging_state_name_type with 
                   pm_auto_chg_state_name_type.
12/01/08   hs      Added some private member variables to track the status of
                   some of the autonomous charging related PMIC interrupts.
11/25/08   hs      Removed the member variable used to track the overall 
                   charging time.
11/18/08   hs      Added a member variable to track the overall charging time.
                   Removed the member variable mFailOfChargingCounter.
11/13/08   hs      Updated the documentation.
09/26/08   vk      Fixed compiler warning
06/30/08   hs      Added FEATURE_CHG_TASK.
04/11/08   hs      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)
#include "chg_evt.h"         /* For class ChargingEventListener          */

/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
class ChargingEventListener;
class ChargingEventGenerator;
class ChargingFactory;
/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/
                      /*************************************/
                      /*     class AutoChargingManager     */
                      /*************************************/
/*! \class AutoChargingManager chg_autocharging.h "chg/chg_autocharging.h"
 *  \brief This is the class that defines the interface for automatic charging
 *  activities in the Charging Application
 *  \details This is the class that defines the interface for the automatic charging
 *  activities in the Charging Application. This class is a child class of the
 *  ChargingEventListener class and the ChargingEventGenerator class.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It is generated and destroyed by the ChargingFactory class.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>PMIC IRQs</td>
 *          <td>By listening to the IrqEventGenerator class.</td>
 *      </tr>
 *      <tr><td>REX timers</td>
 *          <td>By listening to the TimerEventGenerator class.</td>
 *      </tr>
 *      <tr><td>System events</td>
 *          <td>By listening to the SystemEventGenerator class.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  <table border="0">
 *      <tr><td>::PM_VALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-valid IRQ. The class uses this event to launch the   
 *              autonomous charging using UCP (USB Charging Port).</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-invalid IRQ. The class uses this event to stop the   
 *              autonomous charging using UCP (USB Charging Port).</td>
 *      </tr>
 *      <tr><td>::PM_VALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-valid IRQ. The class uses this event to launch the   
 *              autonomous charging using SHP (Standard Host Port).</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-invalid IRQ. The class uses this event to stop the   
 *              autonomous charging using SHP (Standard Host Port).</td>
 *      </tr>
 *      <tr><td>::PM_BATTCONNECT_IRQ_HDL</td>
 *          <td>The battery-connected IRQ. The class uses this event to detect the   
 *              availabitility of the battery.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_STATE_IRQ_HDL</td>
 *          <td>The charging-state-changed IRQ. The class uses this event to decide   
 *              the charging state that the hardware state machine is currently in, in
 *              order to let other applications, such as UI, to know what the 
 *              current charging status is.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_DONE_IRQ_HDL</td>
 *          <td>The end-of-charging IRQ. The class use this event to update some debug
 *              information.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_FAIL_IRQ_HDL</td>
 *          <td>The fail-of-charging IRO. The class uses this event to launch the software
 *              workaround for end-of-charging detection.</td>
 *      </tr>
 *      <tr><td>::PM_IT_BATT_TEMP_IRQ_HDL</td>
 *          <td>The bad-battery-temperature IRQ. The class uses this event to get informed
 *              that the battery is defective.</td>
 *      </tr>
 *      <tr><td>::PM_VBAT_DET_IRQ_HDL</td>
 *          <td>The VBATDET IRQ. The class use this event to update some debug
 *              information.</td>
 *      </tr>
 *      <tr><td>::PM_FASTCHG_IRQ_HDL</td>
 *          <td>The in-fast-charge IRQ. The class use this event to update some debug
 *              information.</td>
 *      </tr>
 *      <tr><td>::CHG_HEART_BEAT_FIRST_TIME</td>
 *          <td>The charger software heartbeat timer. The class use this event to do housekeeping
 *              such as updating the current state information, checking the availability of  
 *              the battery and re-enabing the charging related PMIC IRQs that were triggered 
 *              since the last heartbeat.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_CHARGE_DETECTION_T</td>
 *          <td>The end-of-charging-detection timer. The class use this event to detect if the
 *              autonomous charging is ending: if so, it will launch the final cycle of fast
 *              charging in the autonomous charger.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T</td>
 *          <td>The resume-charging-detection timer. The class use this event to detect if the
 *              battery is truely discharged after the autonomous charging is complete previously:
 *              if so, it will resume the autonomous charging. This time event is needed to
 *              avoid excessive charging of the battery caused by momentary high current events.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T</td>
 *          <td>The end-of-fast-charge-detection timer. The class use this event to detect if the
 *              fast charging is complete: if so, it will start short fast charging cycles with
 *              16-min timeout until the charging is complete. This time event is needed to
 *              make sure the the battery FET stay closed until the battery voltage is close to 
 *              to avoid big dips of VPH_PWR which may cause the degrading of the system 
 *              full performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__USB_SUSPEND</td>
 *          <td>The USB-in-suspended event. The class uses this event to get informed that the
 *              USB device is in suspended mode. The autonomous charging will be stopped.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_ON</td>
 *          <td>The system-in-call-mode event. The class uses this event to get informed that 
 *              the system is in the call mode. IMAXSEL will be lowered in order to protect the
 *              system performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_OFF</td>
 *          <td>The system-ot-of-call-mode event. The class uses this event to get informed that 
 *              the system is out of the call mode. IMAXSEL will be resumed for better charging 
 *              performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING</td>
 *          <td>The start auto charging event. The class uses this event to get informed 
 *              that a valid USB device has been attached to the phone; and charging  
 *              should be started. This event is currently not used.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING</td>
 *          <td>The stop auto charging event. The class uses this event to get informed that
 *              a valid USB device has been removed from the phone; and charging should be 
 *              stopped. This event is currently not used.</td>
 *      </tr>
 *  </table>
 *  
 *  <b>Generates the following events</b>
 *  \n NONE.
 */
class AutoChargingManager: public ChargingEventListener,
                           public ChargingEventGenerator
{
public:
    /*! \brief This member function handles an auto charging related event passed by 
     *  the parameter. 
     *  \param event A auto-charging related event that needs to be handled.
     *  \return void
     *  \note When a new charging event is created, this function could get called
     *  by one of the two event generators:
     *  \li SystemEventGenerator
     *  \li IrqEventGenerator
     */ 
    virtual void EventFired(ChargingEvent* event);
    /*! \brief This member function returns if the charger hardware is
     *  charging the battery or not in the current state
     *  \note This function should get called when there is a need to know
     *  if the charger hardwear is charging the battery or not. 
     */
    bool IsCharging(){ return mIsCharging; };
    /*! \brief This static member function returns if the charger hardware is valid
     *  \note This function should get called when there is a need to know
     *  if the charger hardware is valid or not 
     */
    bool IsChargerValid(){ return mIsChargerValid; };
    /*! \brief This member function returns if the battery is valid or not
     *  \note This function should get called when there is a need to know
     *  if the battery is valid or not 
     */
    bool IsBatteryValid(){ return mIsBatteryValid; };
    /*! \brief This member function returns the UI (User Interface) status
     *  for the current state
     *  \note This function should get called when there is a need to know
     *  the UI status of the current state. 
     */
    chg_ui_event_type GetUiEvent(){ return mUiStatus; };
protected:
    /*! \brief This member function starts the automatic charging process
     *  \note This function should get called when there is a valid USB Standard
     *  Host Port (SHP) attached to the phone and the enumeration has done successfully. 
     *  The following member variables will be updated by this function:
     *  \li mIsChargerValid
     *  \li mChargerType
     */
    void startShpAutoCharging();
    /*! \brief This member function starts the automatic charging process
     *  \note This function should get called when there is a valid USB Charging
     *  Port (UCP) attached to the phone and the enumeration has done successfully. 
     *  The following member variables will be updated by this function:
     *  \li mIsChargerValid
     *  \li mChargerType
     */
    void startUcpAutoCharging();
    /*! \brief This member function Stops the automatic charging process
     *  \note This function should get called in the following scenarios:
     *  \li When there is a valid USB PC device removed from the phone.
     *  \li when there is a valid dumb charger removed from the phone.
     *  \li When the battery is defective.
     *
     *  The following member variables will be updated by this function:
     *  \li mIsChargerValid
     *  \li mChargerType
     *  \li mIsChargingComplete
     *  \li mIsInFinalCycle
     *  \li mLowVbattCounter
     */
    virtual void stopAutoCharging();
    /*! \brief This member function update the current state as well as the charging
     *  UI status based on the current state
     *  \note This function should get called when the state change IRQ gets triggered. 
     *  The following member variables get updated by this function:
     *  \li mIsChargingComplete
     *  \li mUiStatus
     *  \li mIsCharging
     *  \li mIsBatteryValid
     *  \li mBadBatteryTemp
     *  \li mAutoChargingInformation
     */
    void getCurrentState();
protected:
    /*! \brief The UI status of the state.    
     *  \details This variable should get updated when the state changes
     *  (::PM_IT_CHG_STATE_IRQ_HDL ). This variable will be updated by the following member 
     *  function(s)
     *  \li getCurrentState()
     */
    chg_ui_event_type      mUiStatus;
    /*! \brief If charging is considered happening in the current charging state.     
     *  \details This variable will be updated by the following member function(s): 
     *  \li getCurrentState()
     */
    bool                   mIsCharging;
    /*! \brief If the charger hardware is considered valid in the current charging state.     
     *  \details This variable will be updated by the following member function(s): 
     *  \li startShpAutoCharging()
     *  \li startUcpAutoCharging()
     *  \li stopAutoCharging()
     */
    bool                   mIsChargerValid;
    /*! \brief If the battery is considered valid in the current charging state.     
     *  \details This variable will be updated by the following member function(s): 
     *  \li getCurrentState()
     */
    bool                   mIsBatteryValid;
    /*! \brief The current state.     
     *  \details This variable should get updated when the state change IRQ 
     *  (::PM_IT_CHG_STATE_IRQ_HDL ) triggers. This variable will be updated by the 
     *  following member function(s):
     *  \li getCurrentState()
     *  \li EventFired()
     */
    pm_auto_chg_state_name_type        mCurrentState;
    /*! \brief The charger hardware type.     
     *  \details This variable should get updated when the valid/invalid charger IRQs, such 
     *  as ::PM_VALID_CHG_IRQ_HDL, ::PM_INVALID_CHG_IRQ_HDL, ::PM_VALID_USB_CHG_IRQ_HDL
     *  and ::PM_INVALID_USB_CHG_IRQ_HDL trigger. This variable will be updated by the
     *  following member function(s):
     *  \li EventFired()
     *  \li startShpAutoCharging()
     *  \li startUcpAutoCharging()
     *  \li stopAutoCharging()
     */
    chg_auto_charging_charger_type      mChargerType;
    /*! \brief The status of the change-of-state interrupt 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     */
    bool                                 mStateChangeIrqTriggered;
    /*! \brief The status of the charging-done interrupt 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     */
    bool                                 mChargingDoneIrqTriggered;
    /*! \brief The status of the charging-fail interrupt 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     */
    bool                                 mChargingFailIrqTriggered;
    /*! \brief The status of the VBAT_DET interrupt 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     */
    bool                                 mVbatdetIrqTriggered;
    /*! \brief The status of the battery availability 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li getCurrentState()
     */
    bool                                 mBatteryRemoved;
    /*! \brief The status of the battery temperature 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li getCurrentState()
     */
    bool                                 mBadBatteryTemp;
    /*! \brief The status of the final charging cycle 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li getCurrentState()
     *  \li stopAutoCharging()
     */
    bool                                 mIsInFinalCycle;
    /*! \brief The status of charging completion 
     *  \details This variable will be updated by the following member function(s): 
     *  \li getCurrentState()
     *  \li stopAutoCharging()
     */
    bool                                 mIsChargingComplete;
    /*! \brief The occurance of low battery voltage readings. 
     *  \details This variable is used to restore the occurance of consecutive readings of
     *  low battery voltage. If there are 5/(::AUTO_CHARGING_RESUME_CHHARGE_DETECTION_COUNTER)
     *  times in a row, the battery is considered not fully charged and the charging should resume.
     *  This variable should only be used in the resume-charge detection routine and will be
     *  updated by the following member function(s): 
     *  \li EventFired()
     *  \li stopAutoCharging()
     */
    unsigned char                        mLowVbattCounter;
protected:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    AutoChargingManager();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~AutoChargingManager();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
};

                  /******************************************/
                  /*     class Debug_AutoChargingManager     */
                  /******************************************/

/*! \class Debug_AutoChargingManager chg_autocharging.h "chg/chg_autocharging.h"
 *  \brief This is the class that generates AutoChargingManager related debug events 
 *  in the Charging Applicaiton
 *  \details This is the class that generates AutoChargingManager related debug events
 *  in the Charging Applicaiton. It is a child class of the AutoChargingManager class.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It will be generated and destroyed by the ChargingFactory class, depending on whether 
 *  CHG_ENABLE_DEBUGGER is turned on or not in chg_config.h or not: if it is turned on,
 *  an instance of the class Debug_AutoChargingManager will be generated; otherwise,
 *  generated; an instance of the class AutoChargingManager will be generated.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>PMIC IRQs</td>
 *          <td>By listening to the IrqEventGenerator class.</td>
 *      </tr>
 *      <tr><td>REX timers</td>
 *          <td>By listening to the TimerEventGenerator class.</td>
 *      </tr>
 *      <tr><td>System events</td>
 *          <td>By listening to the SystemEventGenerator class.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  <table border="0">
 *      <tr><td>::PM_VALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-valid IRQ. </td>
 *      </tr>
 *      <tr><td>::PM_INVALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-invalid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_VALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-valid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-invalid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_BATTCONNECT_IRQ_HDL</td>
 *          <td>The battery-connected IRQ..</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_STATE_IRQ_HDL</td>
 *          <td>The charging-state-changed IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_DONE_IRQ_HDL</td>
 *          <td>The end-of-charging IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_FAIL_IRQ_HDL</td>
 *          <td>The fail-of-charging IRO.</td>
 *      </tr>
 *      <tr><td>::PM_IT_BATT_TEMP_IRQ_HDL</td>
 *          <td>The bad-battery-temperature IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_VBAT_DET_IRQ_HDL</td>
 *          <td>The VBATDET IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_FASTCHG_IRQ_HDL</td>
 *          <td>The in-fast-charge IRQ.</td>
 *      </tr>
 *      <tr><td>::CHG_HEART_BEAT_FIRST_TIME</td>
 *          <td>The charger software heartbeat timer.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_CHARGE_DETECTION_T</td>
 *          <td>The end-of-charging-detection timer. </td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T</td>
 *          <td>The resume-charging-detection timer. </td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T</td>
 *          <td>The end-of-fast-charge-detection timer. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__USB_SUSPEND</td>
 *          <td>The USB-in-suspended event. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_ON</td>
 *          <td>The system-in-call-mode event. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_OFF</td>
 *          <td>The system-ot-of-call-mode event.</td>
 *      </tr>
 *  </table>
 *  
 *  <b>Generates the following events</b>
 *  <table border="0">
 *      <tr><td>::CHG_CMD__AUTO_CHARGING_DEBUG</td>
 *          <td>EventFired() generates this.</td>
 *      </tr>
 *  </table>
 */
class Debug_AutoChargingManager: public AutoChargingManager
{
    /*! \brief This member function handles an auto charging related event passed by
     *  the parameter.
     *  \param event A auto-charging related event that needs to be handled.
     *  \return void
     *  \note When a new charging event is created, this function could get called
     *  by one of the two event generators:
     *  \li SystemEventGenerator
     *  \li IrqEventGenerator
     */
public:
    virtual void EventFired(ChargingEvent *event);
protected:
private:
    /*! \brief This member function update the automatic charigng information for 
     *  debugging purpose. The information includes the battery voltage, the battery
     *  temperature, the current charging state, the charging current and etc.
     *  \note This function should only get called the charging heart beat kicks. 
     *  The following member variable gets updated by this function:
     *  \li mAutoChargingInformation
     */
    void updateDebugInformation();
private:
    /*! \brief Auto charging information that's used for debug purpose 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li getCurrentState()
     */
    chg_auto_charging_debug_info_type    mAutoChargingInformation;

private:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    Debug_AutoChargingManager();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~Debug_AutoChargingManager();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
};

#if defined(FEATURE_CHG_TASK_USE_PULSE_TOPOFF) || defined (T_WINNT)

                  /************************************************/
                  /*     class AutoChargingManagerPulseTopoff     */
                  /************************************************/

/*! \class AutoChargingManagerPulseTopoff chg_autocharging.h "chg/chg_autocharging.h"
 *  \brief This is the class that uses PMIC3 style (software controlled) pulse charging to 
 *  topoff the battery
 *  \details This is the class that uses PMIC3 style (software controlled) pulse charging 
 *  topoff the battery in the autonomous charging context. It is a child class of the
 *  AutoChargingManager class.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It will be generated and destroyed by the ChargingFactory class, depending on whether the
 *  feature FEATURE_CHG_TASK_USE_PULSE_TOPOFF is defined in custpmic3.h or not: if this
 *  feature is defined, an instance of the class AutoChargingManagerPulseTopoff will be
 *  generated; otherwise, an instance of the class AutoChargingManager will be generated.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>PMIC IRQs</td>
 *          <td>By listening to the IrqEventGenerator class.</td>
 *      </tr>
 *      <tr><td>REX timers</td>
 *          <td>By listening to the TimerEventGenerator class.</td>
 *      </tr>
 *      <tr><td>System events</td>
 *          <td>By listening to the SystemEventGenerator class.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  <table border="0">
 *      <tr><td>::PM_VALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-valid IRQ. The class uses this event to launch the   
 *              autonomous charging using UCP (USB Charging Port).</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-invalid IRQ. The class uses this event to stop the   
 *              autonomous charging using UCP (USB Charging Port).</td>
 *      </tr>
 *      <tr><td>::PM_VALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-valid IRQ. The class uses this event to launch the   
 *              autonomous charging using SHP (Standard Host Port).</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-invalid IRQ. The class uses this event to stop the   
 *              autonomous charging using SHP (Standard Host Port).</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_STATE_IRQ_HDL</td>
 *          <td>The charging-state-changed IRQ. The class uses this event to decide   
 *              the charging state that the hardware state machine is currently in, in
 *              order to let other applications, such as UI, to know what the 
 *              current charging status is.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_DONE_IRQ_HDL</td>
 *          <td>The end-of-charging IRQ. The class use this event to update some debug
 *              information.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_FAIL_IRQ_HDL</td>
 *          <td>The fail-of-charging IRO. The class uses this event to launch the software
 *              workaround for end-of-charging detection.</td>
 *      </tr>
 *      <tr><td>::PM_FASTCHG_IRQ_HDL</td>
 *          <td>The in-fast-charge IRQ. The class use this event to update some debug
 *              information.</td>
 *      </tr>
 *      <tr><td>PM_PULSE_CHG_DONE_IRQ_HDL</td>
 *          <td>The pulse-charging IRQ. The class uses this event to get informed that 
 *              pulse charging has been finished.</td>
 *      </tr>
 *      <tr><td>PM_VBAT_DET_IRQ_HDL</td>
 *          <td>The VBATDET IRQ. The class uses this event to get informed that 
 *              the VBATDET reached. It will start pulse charging to topoff the battery.
 *              </td>
 *      </tr>
 *      <tr><td>PM_BAT_STAT_CHANGED_IRQ_HDL</td>
 *          <td>The battery-alarm IRQ. The class uses this event to get informed that 
 *              the lower battery alarm threshold reached. It willstop pulse charging and 
 *              resume autonomus charging.</td>
 *      </tr>
 *      <tr><td>PM_BATTTEMP_IRQ_HDL</td>
 *          <td>The bad-battery-temp IRQ. The class uses this event to get informed that 
 *              the battery temperature has gone out of the valid range, it will stop the pulse   
 *              charging and resume the autonomous charging. After the autonomous is resumed,
 *              the hardware statemachine will handle the bad-battery-temp error properly. </td>
 *      </tr>
 *      <tr><td>::PM_BATTCONNECT_IRQ_HDL</td>
 *          <td>The battery-connected IRQ. The class uses this event to detect the   
 *              availabitility of the battery. If the battery is removed, it will stop the
 *              pulse chargign and resume the autonomous charging. After the autonomous 
 *              charging is resumed, the hardware statemachine will handle the 
 *              battery-removed error properly.</td>
 *      </tr>
 *      <tr><td>::CHG_HEART_BEAT_FIRST_TIME</td>
 *          <td>The charger software heartbeat timer. The class use this event to do housekeeping
 *              such as updating the current state information, checking the availability of  
 *              the battery and re-enabing the charging related PMIC IRQs that were triggered 
 *              since the last heartbeat.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_CHARGE_DETECTION_T</td>
 *          <td>The end-of-charging-detection timer. The class use this event to detect if the
 *              autonomous charging is ending: if so, it will launch the final cycle of fast
 *              charging in the autonomous charger.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T</td>
 *          <td>The resume-charging-detection timer. The class use this event to detect if the
 *              battery is truely discharged after the autonomous charging is complete previously:
 *              if so, it will resume the autonomous charging. This time event is needed to
 *              avoid excessive charging of the battery caused by momentary high current events.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T</td>
 *          <td>The end-of-fast-charge-detection timer. The class use this event to detect if the
 *              fast charging is complete: if so, it will start short fast charging cycles with
 *              16-min timeout until the charging is complete. This time event is needed to
 *              make sure the the battery FET stay closed until the battery voltage is close to 
 *              to avoid big dips of VPH_PWR which may cause the degrading of the system 
 *              full performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__USB_SUSPEND</td>
 *          <td>The USB-in-suspended event. The class uses this event to get informed that the
 *              USB device is in suspended mode. The autonomous charging will be stopped.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_ON</td>
 *          <td>The system-in-call-mode event. The class uses this event to get informed that 
 *              the system is in the call mode. The pulse charging will be stoped (if it is on
 *              previously) and the autonomous charging will be resumed with IMAXSEL lowered in 
 *              order to protect the system performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_OFF</td>
 *          <td>The system-ot-of-call-mode event. The class uses this event to get informed that 
 *              the system is out of the call mode. The pulse charging will be resumed and IMAXSEL  
 *              will be resumed for better charging performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING</td>
 *          <td>The start auto charging event. The class uses this event to get informed 
 *              that a valid USB device has been attached to the phone; and charging  
 *              should be started. This event is currently not used.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING</td>
 *          <td>The stop auto charging event. The class uses this event to get informed that
 *              a valid USB device has been removed from the phone; and charging should be 
 *              stopped. This event is currently not used.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Generates the following events</b>
 *  \na
 */
class AutoChargingManagerPulseTopoff: public AutoChargingManager
{
public:
    /*! \brief This member function handles an auto charging related event passed by
     *  the parameter.
     *  \param event A auto-charging related event that needs to be handled.
     *  \return void
     *  \note When a new charging event is created, this function could get called
     *  by one of the two event generators:
     *  \li SystemEventGenerator
     *  \li IrqEventGenerator
     */
    virtual void EventFired(ChargingEvent *event);
protected:
    /*! \brief This member function Stops the automatic charging process
     *  \note This function should get called in the following scenarios:
     *  \li When there is a valid USB PC device removed from the phone.
     *  \li when there is a valid dumb charger removed from the phone.
     *  \li When the battery is defective.
     *
     *  The following member variables will be updated by this function:
     *  \li mIsPulseChargingOn
     */
    virtual void stopAutoCharging();
    /*! \brief This member function update the current state as well as the charging
     *  UI status based on the current state
     *  \note This function should get called when the state change IRQ gets triggered. 
     *  The following member variables get updated by this function:
     *  \li mIsPulseChargingOn
     */
    void getCurrentState();
private:
    /*! \brief This member function starts the pulse charging procedure 
     *  \details This function will get called after the VBATDET interrupt triggers,
     *  given that the pulse charging is chosen to top off the battery.
     *  \note In this function, the autonomous charging is disabled in order to enable 
     *  the pulse charging
     *  The following member variable gets updated by this function:
     *  \li mIsPulseChargingOn
     */
    void startTopoffPulseCharging();
    /*! \brief This member function starts the pulse charging procedure using the same
     *  pulse configurations as in Maint state in the software controlled charging
     *  state machine
     *  \details This function will get called after the Pulse_Done interrupt triggers,
     *  given that the pulse charging is chosen to top off the battery.
     *  \note In this function, the autonomous charging is disabled in order to enable 
     *  the pulse charging
     *  The following member variable gets updated by this function:
     *  \li mIsPulseChargingOn
     */
    void startMaintenancePulseCharging();
    /*! \brief This member function exits the pulse charging procedure
     *  \details This function will get called after the battery alarm interrupt triggers
     *  and the battery alarm lower threshold reaches, given that the pulse chargign is 
     *  chosen to top off the battery.
     *  \note In this function, the autonomous charging is resumed after the pulse charging
     *  is disabled.
     *  The following member variable gets updated by this function:
     *  \li mIsPulseChargingOn
     */
    void stopPulseCharging();
protected:
    /*! \brief Used to tell if the pulse charging is on going    
     *  \details This variable will get updated when pulse charging is enabled/disabled.
     *  This variable will be updated by the following member function(s):
     *  \li startTopoffPulseCharging()
     *  \li startMaintenancePulseCharging()
     *  \li stopPulseCharging()
     *  \li EventFired()
     */
    bool                                mIsPulseChargingOn;
    /*! \brief The status of the VBAT_STATUS interrupt 
     *  \details This variable will be updated when either side (lower threshold or 
     *  upper threshold) of the battery alarm reaches. It will be updated by the following 
     *  member function(s): 
     *  \li EventFired()
     *  \li stopPulseCharging()
     */
    bool                                mVBatteryAlarmIrqTriggered;

protected:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    AutoChargingManagerPulseTopoff();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~AutoChargingManagerPulseTopoff();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
};

                  /*****************************************************/
                  /*     class Debug_AutoChargingManagerPulseTopoff    */
                  /*****************************************************/

/*! \class Debug_AutoChargingManagerPulseTopoff chg_autocharging.h "chg/chg_autocharging.h"
 *  \brief This is the class that generates AutoChargingManagerPulseTopoff related debug events 
 *  in the Charging Applicaiton
 *  \details This is the class that generates AutoChargingManagerPulseTopoff related debug events
 *  in the Charging Applicaiton. It is a child class of the AutoChargingManagerPulseTopoff class.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It will be generated and destroyed by the ChargingFactory class, depending on whether 
 *  CHG_ENABLE_DEBUGGER is turned on or not in chg_config.h or not 
 *  (assuming FEATURE_CHG_TASK_USE_PULSE_TOPOFF has been defined in custpmic3.h): if it is turned on,
 *  an instance of the class Debug_AutoChargingManager will be generated; otherwise,
 *  generated; an instance of the class AutoChargingManagerPulseTopoff will be generated.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>PMIC IRQs</td>
 *          <td>By listening to the IrqEventGenerator class.</td>
 *      </tr>
 *      <tr><td>REX timers</td>
 *          <td>By listening to the TimerEventGenerator class.</td>
 *      </tr>
 *      <tr><td>System events</td>
 *          <td>By listening to the SystemEventGenerator class.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  <table border="0">
 *      <tr><td>::PM_VALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-valid IRQ. </td>
 *      </tr>
 *      <tr><td>::PM_INVALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-invalid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_VALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-valid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-invalid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_BATTCONNECT_IRQ_HDL</td>
 *          <td>The battery-connected IRQ..</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_STATE_IRQ_HDL</td>
 *          <td>The charging-state-changed IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_DONE_IRQ_HDL</td>
 *          <td>The end-of-charging IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_FAIL_IRQ_HDL</td>
 *          <td>The fail-of-charging IRO.</td>
 *      </tr>
 *      <tr><td>::PM_IT_BATT_TEMP_IRQ_HDL</td>
 *          <td>The bad-battery-temperature IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_VBAT_DET_IRQ_HDL</td>
 *          <td>The VBATDET IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_FASTCHG_IRQ_HDL</td>
 *          <td>The in-fast-charge IRQ.</td>
 *      </tr>
 *      <tr><td>::CHG_HEART_BEAT_FIRST_TIME</td>
 *          <td>The charger software heartbeat timer.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_CHARGE_DETECTION_T</td>
 *          <td>The end-of-charging-detection timer. </td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T</td>
 *          <td>The resume-charging-detection timer. </td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T</td>
 *          <td>The end-of-fast-charge-detection timer. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__USB_SUSPEND</td>
 *          <td>The USB-in-suspended event. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_ON</td>
 *          <td>The system-in-call-mode event. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_OFF</td>
 *          <td>The system-ot-of-call-mode event.</td>
 *      </tr>
 *  </table>
 *  
 *  <b>Generates the following events</b>
 *  <table border="0">
 *      <tr><td>::CHG_CMD__AUTO_CHARGING_DEBUG</td>
 *          <td>EventFired() generates this.</td>
 *      </tr>
 *  </table>
 */
class Debug_AutoChargingManagerPulseTopoff: public AutoChargingManagerPulseTopoff
{
    /*! \brief This member function handles an auto charging related event passed by
     *  the parameter.
     *  \param event A auto-charging related event that needs to be handled.
     *  \return void
     *  \note When a new charging event is created, this function could get called
     *  by one of the two event generators:
     *  \li SystemEventGenerator
     *  \li IrqEventGenerator
     */
public:
    virtual void EventFired(ChargingEvent *event);
private:
    /*! \brief This member function update the automatic charigng information for 
     *  debugging purpose. The information includes the battery voltage, the battery
     *  temperature, the current charging state, the charging current and etc.
     *  \note This function should only get called after the charging heart beat kicks. 
     *  The following member variable gets updated by this function:
     *  \li mIsPulseChargingOn
     */
    void updateDebugInformation();
private:
    /*! \brief Auto charging information that's used for debug purpose 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li getCurrentState()
     */
    chg_auto_charging_debug_info_type    mAutoChargingInformation;
private:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    Debug_AutoChargingManagerPulseTopoff();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~Debug_AutoChargingManagerPulseTopoff();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
};

#endif /* FEATURE_CHG_TASK_USE_PULSE_TOPOFF */

#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined (T_WINNT)

                  /*********************************************/
                  /*     class AutoChargingManagerCVTopoff     */
                  /*********************************************/

/*! \class AutoChargingManagerCVTopoff chg_autocharging.h "chg/chg_autocharging.h"
 *  \brief This is the class that uses PMIC3 style (software controlled) CV (Constant 
 *  Voltage) to topoff the battery
 *  \details This is the class that uses PMIC3 style (software controlled) CV (Constant 
 *  Voltage) to topoff the battery in the autonomous charging context. It is a child 
 *  class of the AutoChargingManager class.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It will be generated and destroyed by the ChargingFactory class, depending on whether the
 *  feature FEATURE_CHG_TASK_USE_CV_TOPOFF is defined in custpmic3.h or not: if this
 *  feature is defined, an instance of the class AutoChargingManagerCVTopoff will be
 *  generated; otherwise, an instance of the class AutoChargingManager will be generated.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>PMIC IRQs</td>
 *          <td>By listening to the IrqEventGenerator class.</td>
 *      </tr>
 *      <tr><td>REX timers</td>
 *          <td>By listening to the TimerEventGenerator class.</td>
 *      </tr>
 *      <tr><td>System events</td>
 *          <td>By listening to the SystemEventGenerator class.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  <table border="0">
 *      <tr><td>::PM_VALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-valid IRQ. The class uses this event to launch the   
 *              autonomous charging using UCP (USB Charging Port).</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-invalid IRQ. The class uses this event to stop the   
 *              autonomous charging using UCP (USB Charging Port).</td>
 *      </tr>
 *      <tr><td>::PM_VALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-valid IRQ. The class uses this event to launch the   
 *              autonomous charging using SHP (Standard Host Port).</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-invalid IRQ. The class uses this event to stop the   
 *              autonomous charging using SHP (Standard Host Port).</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_STATE_IRQ_HDL</td>
 *          <td>The charging-state-changed IRQ. The class uses this event to decide   
 *              the charging state that the hardware state machine is currently in, in
 *              order to let other applications, such as UI, to know what the 
 *              current charging status is.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_DONE_IRQ_HDL</td>
 *          <td>The end-of-charging IRQ. The class use this event to update some debug
 *              information.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_FAIL_IRQ_HDL</td>
 *          <td>The fail-of-charging IRO. The class uses this event to launch the software
 *              workaround for end-of-charging detection.</td>
 *      </tr>
 *      <tr><td>::PM_FASTCHG_IRQ_HDL</td>
 *          <td>The in-fast-charge IRQ. The class use this event to update some debug
 *              information.</td>
 *      </tr>
 *      <tr><td>PM_VBAT_DET_IRQ_HDL</td>
 *          <td>The VBATDET IRQ. The class uses this event to get informed that 
 *              the VBATDET reached. It will start pulse charging to topoff the battery.
 *              </td>
 *      </tr>
 *      <tr><td>PM_BAT_STAT_CHANGED_IRQ_HDL</td>
 *          <td>The battery-alarm IRQ. The class uses this event to get informed that 
 *              the lower battery alarm threshold reached. It willstop pulse charging and 
 *              resume autonomus charging.</td>
 *      </tr>
 *      <tr><td>PM_BATTTEMP_IRQ_HDL</td>
 *          <td>The bad-battery-temp IRQ. The class uses this event to get informed that 
 *              the battery temperature has gone out of the valid range, it will stop the pulse   
 *              charging and resume the autonomous charging. After the autonomous is resumed,
 *              the hardware statemachine will handle the bad-battery-temp error properly. </td>
 *      </tr>
 *      <tr><td>::PM_BATTCONNECT_IRQ_HDL</td>
 *          <td>The battery-connected IRQ. The class uses this event to detect the   
 *              availabitility of the battery. If the battery is removed, it will stop the
 *              pulse chargign and resume the autonomous charging. After the autonomous 
 *              charging is resumed, the hardware statemachine will handle the 
 *              battery-removed error properly.</td>
 *      </tr>
 *      <tr><td>::CHG_HEART_BEAT_FIRST_TIME</td>
 *          <td>The charger software heartbeat timer. The class use this event to do housekeeping
 *              such as updating the current state information, checking the availability of  
 *              the battery and re-enabing the charging related PMIC IRQs that were triggered 
 *              since the last heartbeat.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_CHARGE_DETECTION_T</td>
 *          <td>The end-of-charging-detection timer. The class use this event to detect if the
 *              autonomous charging is ending: if so, it will launch the final cycle of fast
 *              charging in the autonomous charger.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T</td>
 *          <td>The resume-charging-detection timer. The class use this event to detect if the
 *              battery is truely discharged after the autonomous charging is complete previously:
 *              if so, it will resume the autonomous charging. This time event is needed to
 *              avoid excessive charging of the battery caused by momentary high current events.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T</td>
 *          <td>The end-of-fast-charge-detection timer. The class use this event to detect if the
 *              fast charging is complete: if so, it will start short fast charging cycles with
 *              16-min timeout until the charging is complete. This time event is needed to
 *              make sure the the battery FET stay closed until the battery voltage is close to 
 *              to avoid big dips of VPH_PWR which may cause the degrading of the system 
 *              full performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__USB_SUSPEND</td>
 *          <td>The USB-in-suspended event. The class uses this event to get informed that the
 *              USB device is in suspended mode. The autonomous charging will be stopped.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_ON</td>
 *          <td>The system-in-call-mode event. The class uses this event to get informed that 
 *              the system is in the call mode. The pulse charging will be stoped (if it is on
 *              previously) and the autonomous charging will be resumed with IMAXSEL lowered in 
 *              order to protect the system performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_OFF</td>
 *          <td>The system-ot-of-call-mode event. The class uses this event to get informed that 
 *              the system is out of the call mode. The pulse charging will be resumed and IMAXSEL  
 *              will be resumed for better charging performance.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING</td>
 *          <td>The start auto charging event. The class uses this event to get informed 
 *              that a valid USB device has been attached to the phone; and charging  
 *              should be started. This event is currently not used.</td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING</td>
 *          <td>The stop auto charging event. The class uses this event to get informed that
 *              a valid USB device has been removed from the phone; and charging should be 
 *              stopped. This event is currently not used.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Generates the following events</b>
 *  \na
 */
class AutoChargingManagerCVTopoff: public AutoChargingManager
{
public:
    /*! \brief This member function handles an auto charging related event passed by
     *  the parameter.
     *  \param event A auto-charging related event that needs to be handled.
     *  \return void
     *  \note When a new charging event is created, this function could get called
     *  by one of the two event generators:
     *  \li SystemEventGenerator
     *  \li IrqEventGenerator
     */
    virtual void EventFired(ChargingEvent *event);
protected:
    /*! \brief This member function Stops the automatic charging process
     *  \note This function should get called in the following scenarios:
     *  \li When there is a valid USB PC device removed from the phone.
     *  \li when there is a valid dumb charger removed from the phone.
     *  \li When the battery is defective.
     *
     *  The following member variables will be updated by this function:
     *  \li mIsCVTopoffOn
     */
    virtual void stopAutoCharging();
    /*! \brief This member function update the current state as well as the charging
     *  UI status based on the current state
     *  \note This function should get called when the state change IRQ gets triggered. 
     *  The following member variables get updated by this function:
     *  \li mIsCVTopoffOn
     */
    void getCurrentState();
private:
    /*! \brief This member function starts the software controlled CV Topoff procedure 
     *  \details This function will get called after the VBATDET interrupt triggers,
     *  given that the software controlled CV is chosen to top off the battery.
     *  \note In this function, the autonomous charging is disabled in order to enable 
     *  the software controlled CV Topoff charging
     *  The following member variable gets updated by this function:
     *  \li mIsCVTopoffOn
     */
    void startCVTopoff();
    /*! \brief This member function exits the software controlled CV Topoff procedure
     *  \details This function will get called after the battery alarm interrupt triggers
     *  and the battery alarm lower threshold reaches, given that the software controlled
     *  CV Topoff is chosen to top off the battery.
     *  \note In this function, the autonomous charging is resumed after the software
     *  controlled CV Topoff is disabled.
     *  The following member variable gets updated by this function:
     *  \li mIsCVTopoffOn
     */
    void stopCVTopoff();
    /*! \brief This member function detects if the CV topoff is close to end
     *  \details This function will get called after the end-of-CV-topoff timer 
     *  (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T) expires. 
     *  \note This function is used to do the step-1 detection of the ending of the CV
     *  Topoff. To final determine (the step-2 detection)if the CV topoff is truely 
     *  completed, endCVTopoffDetection2() will be used.
     */
    void endCVTopoffDetection1();
    /*! \brief This member function detects if the CV topoff is truely completed
     *  \details This function will get called after the end-of-CV-topoff timer 
     *  (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T) expires. 
     *  \note This function is used to do the final (step-2) detection of the ending of 
     *  the CV topoff. 
     *  The following member variable gets updated by this function:
     *  \li mIsChargingComplete
     *  \li mEndCVTopoffCounter
     */
    void endCVTopoffDetection2();

private:
protected:
    /*! \brief Used to tell if the pulse charging is on going    
     *  \details This variable will get updated when pulse charging is enabled/disabled.
     *  This variable will be updated by the following member function(s):
     *  \li startCVTopoff()
     *  \li stopCVTopoff()
     *  \li EventFired()
     */
    bool                                mIsCVTopoffOn;
    /*! \brief The status of the VBAT_STATUS interrupt 
     *  \details This variable will be updated when either side (lower threshold or 
     *  upper threshold) of the battery alarm reaches. It will be updated by the following 
     *  member function(s): 
     *  \li EventFired()
     *  \li stopCVTopoff()
     */
    bool                                mVBatteryAlarmIrqTriggered;
    /*! \brief The status of the VCP interrupt 
     *  \details This variable will be updated when the VCP (Voltage Collapse Protection) 
     *  interrupt triggers. It will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li stopCVTopoff()
     */
    bool                                mVcpIrqTriggered;
    /*! \brief The status of the charger-in-temperature-limiting interrupt 
     *  \details This variable will be updated when the charger-in-temperature-limiting 
     *  interrupt triggers. It will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li stopCVTopoff()
     */
    bool                                mChgTLimitIrqTriggered;
    /*! \brief Used to detect the end of CV topoff 
     *  \details This variable will be updated when the end-of-CV-topoff timer #2 expires 
     *  It will be updated by the following member function(s): 
     *  \li endCVTopoffDetection2()
     *  \li stopCVTopoff
     */
    char                                mEndCVTopoffCounter;

protected:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    AutoChargingManagerCVTopoff();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~AutoChargingManagerCVTopoff();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
};

                  /**************************************************/
                  /*     class Debug_AutoChargingManagerCVTopoff    */
                  /**************************************************/

/*! \class Debug_AutoChargingManagerCVTopoff chg_autocharging.h "chg/chg_autocharging.h"
 *  \brief This is the class that generates AutoChargingManagerPulseTopoff related debug events 
 *  in the Charging Applicaiton
 *  \details This is the class that generates AutoChargingManagerCVTopoff related debug events
 *  in the Charging Applicaiton. It is a child class of the AutoChargingManagerCVTopoff class.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It will be generated and destroyed by the ChargingFactory class, depending on whether 
 *  CHG_ENABLE_DEBUGGER is turned on or not in chg_config.h or not 
 *  (assuming FEATURE_CHG_TASK_USE_CV_TOPOFF has been defined in custpmic3.h): if it is turned on,
 *  an instance of the class Debug_AutoChargingManager will be generated; otherwise,
 *  generated; an instance of the class AutoChargingManagerCVTopoff will be generated.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>PMIC IRQs</td>
 *          <td>By listening to the IrqEventGenerator class.</td>
 *      </tr>
 *      <tr><td>REX timers</td>
 *          <td>By listening to the TimerEventGenerator class.</td>
 *      </tr>
 *      <tr><td>System events</td>
 *          <td>By listening to the SystemEventGenerator class.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  <table border="0">
 *      <tr><td>::PM_VALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-valid IRQ. </td>
 *      </tr>
 *      <tr><td>::PM_INVALID_CHG_IRQ_HDL</td>
 *          <td>The wall-charger-invalid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_VALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-valid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_INVALID_USB_CHG_IRQ_HDL</td>
 *          <td>The usb-charger-invalid IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_BATTCONNECT_IRQ_HDL</td>
 *          <td>The battery-connected IRQ..</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_STATE_IRQ_HDL</td>
 *          <td>The charging-state-changed IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_DONE_IRQ_HDL</td>
 *          <td>The end-of-charging IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_IT_CHG_FAIL_IRQ_HDL</td>
 *          <td>The fail-of-charging IRO.</td>
 *      </tr>
 *      <tr><td>::PM_IT_BATT_TEMP_IRQ_HDL</td>
 *          <td>The bad-battery-temperature IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_VBAT_DET_IRQ_HDL</td>
 *          <td>The VBATDET IRQ.</td>
 *      </tr>
 *      <tr><td>::PM_FASTCHG_IRQ_HDL</td>
 *          <td>The in-fast-charge IRQ.</td>
 *      </tr>
 *      <tr><td>::CHG_HEART_BEAT_FIRST_TIME</td>
 *          <td>The charger software heartbeat timer.</td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_CHARGE_DETECTION_T</td>
 *          <td>The end-of-charging-detection timer. </td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T</td>
 *          <td>The resume-charging-detection timer. </td>
 *      </tr>
 *      <tr><td>::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T</td>
 *          <td>The end-of-fast-charge-detection timer. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__USB_SUSPEND</td>
 *          <td>The USB-in-suspended event. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_ON</td>
 *          <td>The system-in-call-mode event. </td>
 *      </tr>
 *      <tr><td>::CHG_SYSTEM__TX_OFF</td>
 *          <td>The system-ot-of-call-mode event.</td>
 *      </tr>
 *  </table>
 *  
 *  <b>Generates the following events</b>
 *  <table border="0">
 *      <tr><td>::CHG_CMD__AUTO_CHARGING_DEBUG</td>
 *          <td>EventFired() generates this.</td>
 *      </tr>
 *  </table>
 */
class Debug_AutoChargingManagerCVTopoff: public AutoChargingManagerCVTopoff
{
    /*! \brief This member function handles an auto charging related event passed by
     *  the parameter.
     *  \param event A auto-charging related event that needs to be handled.
     *  \return void
     *  \note When a new charging event is created, this function could get called
     *  by one of the two event generators:
     *  \li SystemEventGenerator
     *  \li IrqEventGenerator
     */
public:
    virtual void EventFired(ChargingEvent *event);
private:
    /*! \brief This member function update the automatic charigng information for 
     *  debugging purpose. The information includes the battery voltage, the battery
     *  temperature, the current charging state, the charging current and etc.
     *  \note This function should only get called after the charging heart beat kicks. 
     *  The following member variable gets updated by this function:
     *  \li mIsPulseChargingOn
     */
    void updateDebugInformation();
private:
    /*! \brief Auto charging information that's used for debug purpose 
     *  \details This variable will be updated by the following member function(s): 
     *  \li EventFired()
     *  \li getCurrentState()
     */
    chg_auto_charging_debug_info_type    mAutoChargingInformation;
private:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    Debug_AutoChargingManagerCVTopoff();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~Debug_AutoChargingManagerCVTopoff();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
};

#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */

#endif /* FEATURE_CHG_TASK || T_WINNT*/
#endif /* CHG_AUTO_CHARGING_H */

