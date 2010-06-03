#ifndef CHG_SYSTEM_H
#define CHG_SYSTEM_H

/*! \file 
 *  \n
 *  \brief  chg_system.h ---- SYSTEM RELATED CLASS DECLARATION
 *  \details This header file contains class declarations of the SystemEvent 
 *  class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_system.h#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/09   hs      Removed unnecessary header file inclusion.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Code cleanup.
10/04/08   vk      Add/Remove Listener for usb suspend case
05/14/08   hs      Removed the usage of std::list.
                   Added FEATURE_CHG_TASK.
09/05/07   hs      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#include "chg_evt.h"       /* For the ChargingEventListener
                                and ChargingEvent definition               */
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
class ChargingFactory;
/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/
                  /****************************/
                  /*     class SystemEvent    */
                  /****************************/

/*! \class SystemEvent chg_system.h "chg/chg_system.h"
 *  \brief This is the class that defines the interface for the system related 
 *  charging events in the Charging Application
 *  \details This is the class that defines the interface for the system related
 *  charging events in the Charging Application. This class is a child class of
 *  the ChargingEvent class.
 *  \note The instances of this class would be generated and destroyed, at runtime,
 *  by a SystemEventGenerator class, when a system event accurs.
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
class SystemEvent: public ChargingEvent
{
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  ChargerTimerEvent.
     */
friend class ChargingFactory;

public:
    /*! \brief This member function returns the sytem event ID
     *  \param none 
     *  \return An enum that contains the current system charging event ID.
     *  \note This function should be called by the ChargingEventListener class to get 
     *  the event ID in order to handle the event.
     */
    chg_system_event_id_type GetSystemEventId()
    {
        return mSystemEventId;
    }
    /*! \brief This member function returns the tx voter ID
     *  \param none 
     *  \return An enum that contains the tx voter ID.
     *  \note This function should be called by the charger task to get the
     *  tx voter ID in order to handle the event.
     */
    chg_tx_wait_voter_type GetTxVoterId()
    {
        return mVoterId;
    }
public:
    /*! \brief A contructor
     *  \details In this contructor, the event ID is to ::CHG_CMD__SYSTEM_STATUS_CHANGED.
     *  \param systemeventid The system event ID of this charging event object.
     *  \param voterid The voter  ID of this charging event object.
     *  \return NA
     */
    SystemEvent(chg_system_event_id_type systemeventid,
                chg_tx_wait_voter_type    voterid); 
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~SystemEvent();
private:
    /*! \brief The system event ID 
     *  \details THis variable is used to hold the system event ID
     *  \details This variable should only be set when a new system  event is created.
     */
    chg_system_event_id_type    mSystemEventId;
    /*! \brief The voter ID 
     *  \details This variable is used to tell which modulu outside PMIC is voting 
     *  TX_WAIT.
     *  \details This variable should only be set when a new system  event is created.
     */
    chg_tx_wait_voter_type       mVoterId;

}; /* end of class SystemEvent */

                  /**************************************/
                  /*     class SystemEventGenerator     */
                  /**************************************/

/*! \class SystemEventGenerator chg_system.h "chg/chg_system.h"
 *  \brief This is the class that defines the interface for managing the charging
 *  related system events
 *  \details This is the class that defines the interface for managing all the charging
 *  related system events, including adding/removing listeners for the specific system 
 *  events, and creating system events, etc.
 *  \note The charging events generated by this class cab be either in the \b task  
 *  context, or in the interrupt context.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It is generated and destroyed by the ChargingFactory class.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>Managing system events</td>
 *          <td>By Providing interface for using various system events.</td>
 *      </tr>
 *      <tr><td>Publishing new system charging events</td>
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
 *      <tr><td>::CHG_CMD__SYSTEM_STATUS_CHANGED</td>
 *          <td>TxWaitVote(), FtmChargingSwitch() and AutoChargingSwitch()
 *              generate this.</td>
 *      </tr>
 *  </table>
 */
class SystemEventGenerator
{
public:
    /*! \brief This member function fires the specific system event, passed by the
     *  parameter, to its listeners 
     *  \param event A system charging event that needs to be handled.
     *  \return void
     *  \note This function should be only called when a new system charging event of type 
     *  SystemEvent is created.
     */ 
    void FireEvent(ChargingEvent *event);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the Tx Wait Vote system events (::CHG_SYSTEM__TX_ON 
     *  and ::CHG_SYSTEM__TX_OFF)
     *  \param listener A listener that wants to gets informed when some of the modules
     *  in the system vote to put the charger task in/out of the TX WAIT state.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  module in the system votes to put the charger task in/out of the TX WAIT state.
     */ 
    void AddListenerForTxWaitVote(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for the Tx Wait Vote system events (::CHG_SYSTEM__TX_ON 
     *  and ::CHG_SYSTEM__TX_OFF)
     *  \param listener A listener that no longer wants to gets informed when some of the 
     *  modules in the system vote to put the charger task in/out of the TX WAIT state.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed 
     *  when a module in the system votes to put the charger task in/out of the TX WAIT 
     *  state.
     */ 
    void RemoveListenerForTxWaitVote(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for the FTM enabling/disabling charging events 
     *  (::CHG_SYSTEM__FTM_SWITCH_ON_CHRGING and ::CHG_SYSTEM__FTM_SWITCH_OFF_CHRGING)
     *  \param listener A listener that wants to gets informed when FTM switchs on/off
     *  charging.
     *  \return void
     *  \note This function should be called when a class wants to be informed when FTM 
     *  switchs on/off charging.
     */ 
    void AddListenerForFtmChargingSwitch(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for the FTM enabling/disabling charging events 
     *  (::CHG_SYSTEM__FTM_SWITCH_ON_CHRGING and ::CHG_SYSTEM__FTM_SWITCH_OFF_CHRGING)
     *  \param listener A listener that no longer wants to gets informed when FTM
     *  switchs on/off charging.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed 
     *  when FTM switchs on/off charging.
     */ 
    void RemoveListenerForFtmChargingSwitch(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for enabling/disabling automatic charging events 
     *  (::CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING and ::CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING)
     *  \param listener A listener that wants to gets informed when switching on/off
     *  automatic charging.
     *  \return void
     *  \note This function should be called when a class wants to be informed when  
     *  switching on/off automatic charging.
     */ 
    void AddListenerForAutoChargingSwitch(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for enabling/disabling automatic charging events 
     *  (::CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING and ::CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING)
     *  \param listener A listener that no longer wants to gets informed when switching
     *  on/off automatic charging.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed 
     *  when switching on/off automatic charging.
     */ 
    void RemoveListenerForAutoChargingSwitch(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the specific listener, 
     *  passed by the parameter, for starting the USB suspend events 
	 *  (::CHG_SYSTEM__USB_SUSPEND)
     *  \param listener A listener that wants to gets informed when USB in suspend 
     *  state.
     *  \return void
     *  \note This function should be called when a class wants to be informed when  
     *  USB in suspend state.
     */ 
    void AddListenerForUsbSuspend(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the specific listener, 
     *  passed by the parameter, for USB suspend events 
     *  (::CHG_SYSTEM__USB_SUSPEND)
     *  \param listener A listener that no longer wants to gets informed when USB suspend
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed 
     *  when USB suspended.
     */ 
    void RemoveListenerForUsbSuspend(ChargingEventListener *listener);     
    /*! \brief This member function is called when a module votes to put the
     *  charger task in/out of the TX WAIT state
     *  \param cmd Command used to put the charger task in/out of the TX WAIT state
     *  \param data Useful information, such as the voter ID, associated with this event
     *  \return void
     *  \note This function should only be be called through the charger task interface
     *  chg_tx_wait_vote().
     */ 
    void TxWaitVote(chg_cmd_type cmd, void* data);
    /*! \brief This member function is called when FTM switches on/off charging 
     *  \param cmd Command used to switch on/off charging.
     *  \return void
     *  \note This function should only be called through the charger task interface
     *  chg_ftm_charging_switch().
     */ 
    void FtmChargingSwitch(chg_cmd_type cmd);
    /*! \brief This member function is called when the system switches on/off
     *  automatic charging 
     *  \param cmd Command used to switch on/off automatic charging.
     *  \return void
     *  \note This function should only be called through the charger task interface
     *  chg_auto_charging_switch().
     */ 
    void AutoChargingSwitch(chg_cmd_type cmd);
protected:
    /*! \struct SystemEventListenerInfo
     *  \brief Used to store all information about the event listener
     *  \details A structure for holding the informatmion of the listener.
     *  \note this type is used to store listener information in the listener list.
     */
    struct SystemEventListenerInfo
    {
        /*! Event listener                        */
        ChargingEventListener        *mpListener; 
        /*! System event ID                       */
        chg_system_event_id_type      mSystemEventId;  
    };
    /*! \struct SystemEventListenerLinkInfo
     *  \brief Used to store all information about the event listener
     *  \details A structure for holding the informatmion of the listener.
     *  \note this type is used to store listener information in the listener list.
     */
    struct SystemEventListenerLinkInfo
    {
        /*! Queue link.                           */
        q_link_type                     mLink;    
        /*! Event listener                        */
        ChargingEventListener        *mpListener; 
        /*! System event ID                       */
        chg_system_event_id_type      mSystemEventId;  
    };
protected:
    /*! \brief This member function returns a buffer from the listener free queue 
     *  \param void
     *  \return a pointer to the buffer
     *  \note The returned buffer may be NULL if there were no free listener buffers
     *  available. 
     */ 
    SystemEventListenerLinkInfo* getListenerBuffer(void);
    /*! \brief This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue 
     *  \details This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue. This function is used  
     *  to determine if an item is in the queue by comparing the system event ID as
     *  well as the listener object. 
     *  \param item A pointer to the comparing item.
     *  \param compare_val A pointer to the compared value.
     *  \return No-zero is the item is in the queue, zero otherwise
     *  \note This function compares the system event ID as well as the listener object.
     */ 
    static int listenerInformationCompare(void* item, void* compare_val);
    /*! \brief This member function adds the specific listener 
     *  \details This member function adds the specific listener, with its information,
     *  passed by the parameter, to the system event generator's listener list.
     *  \param listener A listener that wants to be informed when a specific system 
     *  event happens.
     *  \param systemeventid The system event ID that the specific listener wants to 
     *  listen for.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  specific system event happens.
     */ 
    void addListener(
        ChargingEventListener          *listener,
        chg_system_event_id_type       systemeventid);
    /*! \brief This member function removes the specific listener 
     *  \details This member function removes the specific listener, with its information,
     *  passed by the parameter, from the event generator's listener list.
     *  \param listener A listener that no longer wants to be informed when the specific 
     *  system event happens.
     *  \param systemeventid The system event ID that the specific listener no longer 
     *  wants to listen to.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed
     *  when the specific system event happens.
     */ 
    void removeListener(
        ChargingEventListener          *listener,
        chg_system_event_id_type       systemeventid);

protected:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    SystemEventGenerator();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~SystemEventGenerator();
protected:
    /*! \brief The system event listener buffer 
     */
    SystemEventListenerLinkInfo   mListenerBuffer[CHG_SYS_EVT_LISTENER_BUF_CNT];
    /*! \brief The listener queue 
     *  \details This listener queue contains the listener information for each  
     *  individual listeners.
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s):  
     *  \li addListener()
     *  \li removeListener()
     */
    q_type                  mListenerQueue;
    /*! \brief The listener free queue 
     *  \details This listener queue contains the unused listener buffers
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s): 
     *  \li addListener()
     *  \li removeListener()
     */
    q_type                  mListenerFreeQueue;

    /*! \brief friend class ChargingFactory
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
}; /* end class SystemEventGenerator */

#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif /* CHG_SYSTEM_H */

