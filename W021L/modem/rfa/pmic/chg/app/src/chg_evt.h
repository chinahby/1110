#ifndef CHG_EVT_H
#define CHG_EVT_H

/*! \file 
 *  \n
 *  \brief  chg_evt.h ---- CHARGING EVENT RELATED CLASS DECLARATION
 *  \details This header file contains class declarations of the ChargingEvent
 *  class, the ChargingEventGenerator class, and the ChargingEventListener
 *  class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_evt.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   hs      Removed unnecessary header file inclusion.
05/07/09   hs      Removed unnecessary header file inclusion.
04/03/09   hs      Added support for unit tests.
05/12/08   hs      Removed the usage of std::list.
                   Added FEATURE_CHG_TASK.
07/11/07   hs      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#include "chg_task.h"        /* For chg_task_cmd_id_type type definition */
#include "chg_config.h"      /* For CHG_EVT_LISTENER_BUF_CNT definition  */
/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
class ChargingEvent;
class ChargingEventGenerator;
class ChargingEventListener;
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/
                  /**************************************/
                  /*        class ChargingEvent         */
                  /**************************************/
/*! \class ChargingEvent chg_evt.h "chg/chg_evt.h"
 *  \brief This is the class that defines the interface for the charging events
 *  in the Charging Application
 *  \details This is the class that defines the interface for the charging events
 *  in the Charging Application. This class is the base class for all types of
 *  the charging event classes, including the IrqEvent class, the TimerEvent
 *  class, the SystemEvent class, the BatteryDebugEvent class, the ChargerDebugEvent
 *  class, the ChargingStateDebugEvent class, the CurrentControlDebugEvent class,
 *  and the PowerControlDebugEvent class.
 *  \note The instances of this class would be generated and destroyed, at runtime,
 *  by a ChargingEventGenerator class, when any charging related status is changed.
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
class ChargingEvent
{
public:
    /*! \brief This member function returns the charigng event ID
     *  \param none 
     *  \return An enum that contains the current charging event ID.
     *  \note This function should be called by the ChargingEventListener to get the
     *  event ID in order to handle the event.
     */
    chg_task_cmd_id_type GetEventId()
    {
        return mEventId;
    }
public:
    /*! \brief A contructor
     *  \details In this contructor, the event ID is set by the parameter.
     *  \param evt_id The event ID of this charging event object.
     *  \return NA
     */
    ChargingEvent(chg_task_cmd_id_type   evt_id):
        mEventId(evt_id)
    {

    }
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~ChargingEvent()
    {

    }
protected:
    /*! \brief Event ID 
     *  \details This variable should only be set when a new charging event is created.
     */
    chg_task_cmd_id_type      mEventId;
}; /* end of class ChargingEvent */

                      /****************************************/
                      /*     class ChargingEventGenerator     */
                      /****************************************/

/*! \class ChargingEventGenerator chg_evt.h "chg/chg_evt.h"
 *  \brief This is the class that defines the interface for publishing charging events
 *   in the Charging Application
 *  \details This is the class that defines the interface for publishing charging events
 *  in the Charging Application. This class is the base class for various classes in the
 *  Charging Applicaiton, including the Charger class, the Battery class, the 
 *  PowerControlManager class, the CurrentControlManager class, and the SwapStates class, 
 *  etc.
 *  \note The charging events generated by this class should be in the \b task context, 
 *  instead of in the interrupt context.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>Firing new charging events</td>
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
 *  \n None
 */
class ChargingEventGenerator
{
public:
    /*! \brief This member function fires the specific charging event, passed by the
     *  parameter, to its listeners 
     *  \param event A charging event that needs to be handled.
     *  \return void
     *  \note This function should be only called when a new charging event, of type 
     *  ChargingEvent, is created.
     */ 
    void FireEvent(ChargingEvent *event);
    /*! \brief This member function adds the specific listener
     *  \details This member function adds the specific listener, passed by the parameter,
     *  to the event generator's listener list.
     *  \param listener A listener that wants to listen to some events of type 
     *  ChargingEvent.
     *  \return void
     *  \note This function should be called when a ChargingEventListener wants to be  
     *  informed when any charging event happens.
     */ 
    void AddListener(ChargingEventListener *listener);
    /*! \brief This member function removes the specific listener
     *  \details This member function removes the specific listener, passed by the parameter,
     *  from the event generator's listener list.
     *  \param listener A listener that no longer wants to listen to the charging event
     *  generator.
     *  \return void
     *  \note This function should be called when a class no longer wants to be informed 
     *  when any charging event happens.
     */ 
    void RemoveListener(ChargingEventListener *listener);
protected:
    /*! \struct EventListenerLinkInfo
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
    }EventListenerLinkInfo;
protected:
    /*! \brief This member function returns a buffer from the listener free queue 
     *  \param void
     *  \return a pointer to the buffer
     *  \note The returned buffer may be NULL if there were no free listener buffers
     *  available. 
     */ 
    EventListenerLinkInfo* getListenerBuffer(void);
    /*! \brief This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue 
     *  \details This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue. This function is used  
     *  to determine if an item is in the queue by comparing the listener object. 
     *  \param item A pointer to the comparing item.
     *  \param compare_val A pointer to the listener object.
     *  \return No-zero is the item is in the queue, zero otherwise
     *  \note This function compares the listener object.
     */ 
    static int listenerInformationCompare(void* item, void* compare_val);
protected:
    /*! \brief The charging event listener buffer 
     */
    EventListenerLinkInfo   mListenerBuffer[CHG_EVT_LISTENER_BUF_CNT];
    /*! \brief The listener queue 
     *  \details This listener queue contains the listener information for each  
     *  individual listeners.
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s):  
     *  \li AddListener()
     *  \li RemoveListener()
     */
    q_type                  mListenerQueue;
    /*! \brief The listener free queue 
     *  \details This listener queue contains the unused listener buffers
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s): 
     *  \li AddListener()
     *  \li RemoveListener()
     */
    q_type                  mListenerFreeQueue;
protected:
    /*! \brief A constructor.
     *  \param none
     *  \return NA
     */
    ChargingEventGenerator();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~ChargingEventGenerator();
    /*! \brief friend class ChargingFactory
     *  \details Declare ChargingFactory as a friend, so it could create an instance  
     *  of this class.
     */
friend class ChargingFactory;
};
                  /****************************************/
                  /*     class ChargingEventListener      */
                  /****************************************/
/*! \class ChargingEventListener chg_evt.h "chg/chg_evt.h"
 *  \brief This is the class that defines the interface for handling charging events.
 *  \details This is the class that defines the interface for handling charging events.
 *  This class is the base class for various classes in the Charging Applicaiton, 
 *  including the Charger class, the Battery class, the PowerControlManager class, the
 *  CurrentControlManager class, the ChargingState class, the ChargingDebugger class,
 *  and the SwapStates class, etc.
 *  \note The instance of this class should not be created directly in the Charging 
 *  Application.
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
class ChargingEventListener
{
public:
    /*! \brief This member function handles the specific event passed by the
     *  parameter. 
     *  \param event A charging event that needs to be handled.
     *  \return void
     *  \note When a charging event is created, this function could get called
     *  by one of the three event generators:
     *  \li ChargingEventGenerator
     *  \li IrqEventGenerator
     *  \li TimerEventGenerator
     *  \li SystemEventGenerator
     */ 
    virtual void EventFired(ChargingEvent* event);
protected:
    /*! \brief A constructor.
     *  \param none
     *  \return NA
     */
    ChargingEventListener();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~ChargingEventListener();
    /*! \brief friend class ChargingFactory
     *  \details Declare ChargingFactory as a friend, so it could create an instance  
     *  of this class.
     */
friend class ChargingFactory;
}; /* end class ChargingEventListener */

#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif /* CHG_EVT_H */
