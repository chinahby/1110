#ifndef CHG_TIMER_H
#define CHG_TIMER_H

/*! \file 
 *  \n
 *  \brief  chg_timer.h ---- TIMER RELATED CLASS DECLARATION
 *  \details This header file contains class declarations of the TimerEvent 
 *  class and the TimerEventGenerator class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_timer.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/09   hs      Added support for software controlled CV topoff.
05/05/09   hs      Added support for the improved algorithm of end-of-charge detection.
04/23/09   hs      Added support for the resume-charging detection.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
03/10/09   hs      Added support for end of charge work around in the 
                   autonomous charging.
05/15/08   hs      Removed the usage of std.
                   Added FEATURE_CHG_TASK.
04/16/08   hs      Name changes for some member functions.
07/11/07   hs      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

extern "C"
{
#include "timer.h"             /* For timer definitions                    */
}
#include "chg_evt.h"           /* For classes ChargingEventListener,       *
                                * and ChargingEvent definition             */
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/

                      /***************************/
                      /*     class TimerEvent    */
                      /***************************/

/*! \class TimerEvent chg_timer.h "chg/chg_timer.h"
 *  \brief This is the class that defines the interface for the timer related 
 *  charging events in the Charging Application
 *  \details This is the class that defines the interface for the timer related
 *  charging events in the Charging Application. This class is a child class of
 *  the ChargingEvent class.
 *  \note The instances of this class would be generated and destroyed, at runtime,
 *  by a TimerEventGenerator class, when a timer expires.
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
class TimerEvent: public ChargingEvent
{
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
public:
    /*! \brief This member function returns the timer token
     *  \param none 
     *  \return An integer that contains the token of a timer that's just
     *  expired
     *  \note This function should be called in the following scenario:
     *  \li By the charger task, in order to put the correct information into 
     *  the task command queue.
     *  \li By a TimerEventGenerator class, in order to fire the timer event
     *  to the listeners that are registered to this timer event, instead of
     *  broadcasting it to all the listeners.
     */
    unsigned int GetToken()
    {
        return mToken;
    }
    /*! \brief This member function sets the delay of the first expiry
     *  \param delay The delay of the first expiry of a timer, in mili
     *  second. 
     *  \return void
     *  \note This function should be called by the TimerEventGenerator class, when  
     *  firing a new timer event to its listeners. The TimerEventGenerator class uses
     *  this function to update the timer event information that it's about to fire.
     */
    void SetFirstDelay(unsigned int delay)
    {
        mFirstDelay = delay;
    }
    /*! \brief This member function sets the delay of the repeated expiries
     *  \param repeat The delay of the repeated expiries of a timer. 
     *  \return void
     *  \note This function should be called by a TimerEventGenerator class, when  
     *  firing a new timer event to its listeners. The TimerEventGenerator class uses
     *  this function to update the timer event information that it's about to fire.
     */
    void SetRepeatDelay(unsigned int repeat)
    {
        mRepeatDelay = repeat;
    }
    /*! \brief This member function returns the delay of the first expiry
     *  \param none 
     *  \return An integer that contains the delay (in milli second) of the first expiry
     *  of a timer.
     *  \note This function should be called by a ChargingEventListener class, in order
     *  order to decide if the timer event is one of the events that it should handle. 
     */
    unsigned int GetFirstDelay( )
    {
        return mFirstDelay;
    }
protected:
    /*! \brief The delay of the first expiry. 
     *  \details This variable will be set by the following member funciton(s):
     *  \li SetFirstDelay()
     */
    unsigned int  mFirstDelay;
    /*! \brief Period (in milli second) between repeated expiries (0 = not periodic).
     *  \details This variable will be set by the following member funciton(s):
     *  \li SetRepeatDelay()
     */
    unsigned int  mRepeatDelay;
    /*! \brief Timer token.
     *  \details This token is used to differentiate different timer events. Each timer 
     *  event has a unique token. This variable should only be set when creating a new  
     *  timer event object.
     */
    unsigned int mToken;
public:
    /*! \brief A contructor
     *  \details In this contructor, the event ID is set to ::CHG_CMD__TIMER_EXPIRED;
     *  and the timer token is set by the parameter.
     *  \param token The timer token.
     *  \return NA
     */
    TimerEvent(unsigned int token);
    /*! \brief A destructor
     *  \param none
     *  \return NA
     */
    virtual ~TimerEvent()
    {

    }
}; /* end of class TimerEvent */

                  /*************************************/
                  /*     class TimerEventGenerator     */
                  /*************************************/

/*! \class TimerEventGenerator chg_timer.h "chg/chg_timer.h"
 *  \brief This is the class that defines the interface for managing the charging
 *  related timers 
 *  \details This is the class that defines the interface for managing all the charging
 *  related timers, including adding/removing listeners for specific timers, and
 *  setting up / cleaning up timers, etc. It is the base class of the
 *  RexTimerEventGenerator class.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It is generated and destroyed by the ChargingFactory class.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>Managing timers</td>
 *          <td>By providing interface for using the charging related timers.</td>
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
class TimerEventGenerator
{
public:
    /*! \brief This member function fires the specific timer event, passed by the
     *  parameter, to its listeners 
     *  \param event A timer charging event that needs to be handled.
     *  \return void
     *  \note This function should be only called when a new timer charging event of type 
     *  TimerEvent is created.
     */ 
    void FireEvent(ChargingEvent *event);
    /*! \brief This member function provides an interface for adding the listener for
     *  the charging heart beat timer (::CHG_HEART_BEAT)
     *  \param listener A listener
     *  \return void
     *  \note The charging heart beat timer is a recurring timer. This function should
     *  be called only when a ChargingEventListener class wants to get informed when
     *  the charging heat beat timer expires.
     */ 
    virtual void AddListenerForChargingHeartBeat(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the charging heart beat timer (::CHG_HEART_BEAT)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the charging heat beat timer expires.
     */ 
    virtual void RemoveListenerForChargingHeatBeat(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-charging detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-charging detection timer is a one-shot timer. This function should 
     *  be called only when a ChargingEventListener class wants to get informed when  the
     *  end-of-charging detection timer expires.
     */ 
    virtual void AddListenerForEndOfChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the end-of-charging detection timer (::AUTO_CHARGING_END_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-charging detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  resuming-charging detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note The resume-charging detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  resume-charging detection timer expires.
     */ 
    virtual void AddListenerForResumeChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the resume-charging detection timer (::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the resume-charging  detection timer expires.
     */ 
    virtual void RemoveListenerForResumeChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-fast-charge detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-fast-charge detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  end-of-fast-charge detection timer expires.
     */ 
    virtual void AddListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the end-of-fast-charge detection timer (::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-fast-charge  detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener *listener);

#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-CV(Constant Voltage)-topoff detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-CV-topoff detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  end-of-CV-topoff detection timer expires.
     */ 
    virtual void AddListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the end-of-CV-topoff detection timer (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-CV-topoff  detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-CV(Constant Voltage)-topoff detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-CV-topoff detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  end-of-CV-topoff detection timer expires.
     */ 
    virtual void AddListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the end-of-CV-topoff detection timer (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-CV-topoff  detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener *listener);
#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */

    /*! \brief This member function adds the specific listener 
     *  \details This member function adds the specific listener, with its information,
     *  passed by the parameter, to the timer event generator's listener list.
     *  \param listener A listener that wants to be informed when a charging realted 
     *  timer expires.
     *  \param firstdelay The delay of the first expiry.
     *  \param repeatdelay The delay of the repeated expiries.
     *  \param shared A boolean that tells if the timer is going to be shared by
     *  multiple listeners. By default, tiemr is not shared.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  specific timer expires.
     */ 
     virtual void AddListener(
        ChargingEventListener *listener,
        unsigned int firstdelay, 
        unsigned int repeatdelay,
        bool shared = false
        );
     /*! \brief This member function removes the specific listener
      *  \details This member function removes the specific listener, with its information,
      *  passed by the parameter, from the timer event generator's listener list. 
      *  \param listener A listener that no long wants to be informed when a charging  
      *  related timer expires.
      *  \param firstdelay The delay of the first expiry.
      *  \param repeatdelay The delay of the repeated expiries.
      *  \return void
      *  \note This function should be called when a class no longer wants to be informed 
      *  when a specific timer expires.
      */ 
     virtual void RemoveListener(
        ChargingEventListener *listener,
        unsigned int firstdelay, 
        unsigned int repeatdelay
    );
protected:
    /*! \struct TimerListenerInfo
     *  \brief Used to store all information about the event listener
     *  \details A structure for holding the timer informatmion of the listener.
     *  \note This type is used to store listener information in the listener list.
     */
    struct TimerListenerInfo
    {
        /*! Event listener */
        ChargingEventListener*   mpListener; 
        /*! Time (in ms) until first timer expiry */
        unsigned int             mFirstTimeOut;   
        /*! Period (in ms) between repeated expiries (0 = not periodic) */
        unsigned int             mRepeatTimeOut;  
    };

    /*! \struct TimerListenerLinkInfo
     *  \brief Used to store the link information about the event listener
     *  \details A structure for holding the timer informatmion of the listener.
     *  \note This type is used to store listener information in the listener list.
     */
    struct TimerListenerLinkInfo
    {
        /*! Queue link.                                                 */
        q_link_type              mLink;    
        /*! Event listener                                              */
        ChargingEventListener*   mpListener; 
        /*! Time (in ms) until first timer expiry                       */
        unsigned int             mFirstTimeOut;   
        /*! Period (in ms) between repeated expiries (0 = not periodic) */
        unsigned int             mRepeatTimeOut;  
        /*! Timer token                                                 */
        unsigned int             mTimerToken;   
    };
protected:
    /*! \brief This member function returns a buffer from the listener free queue 
     *  \param void
     *  \return a pointer to the buffer
     *  \note The returned buffer may be NULL if there were no free listener buffers
     *  available. 
     */ 
    TimerListenerLinkInfo* getListenerBuffer(void);
    /*! \brief This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue 
     *  \details This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue. This function is used  
     *  to determine if an item is in the queue by comparing the first delay of the
     *  timer, the repeat delay of the timer, as well as the listener object. 
     *  \param item A pointer to the comparing item.
     *  \param compare_val A pointer to the compared value.
     *  \return No-zero is the item is in the queue, zero otherwise
     *  \note This function compares the first delay of the timer, the repeat delay
     *  of the timer, as well as the listener object.
     */ 
    static int listenerInformationCompare(void* item, void* compare_val);

protected:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    TimerEventGenerator();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~TimerEventGenerator();
protected:
    /*! \brief The listener buffer 
     */
    TimerListenerLinkInfo  mListenerBuffer[CHG_TIMER_LISTENER_BUF_CNT ];
    /*! \brief The listener queue 
     *  \details This listener queue contains the listener information for each  
     *  individual listeners.
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s):  
     *  \li AddListener()
     *  \li RemoveListener()
     */
    q_type                mListenerQueue;
    /*! \brief The listener free queue 
     *  \details This listener queue contains the unused listener buffers
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s): 
     *  \li AddListener()
     *  \li RemoveListener()
     */
    q_type                mListenerFreeQueue;

    /*! \brief The timer token 
     *  \details This timer token is used to differentiate various timers that are
     *  created in the class. 
     *  \note The token for each timer should be unique. 
     */
    unsigned int mTimerToken;
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
}; /* end class TimerEventGenerator */

                      /****************************************/
                      /*     class RexTimerEventGenerator     */
                      /****************************************/

/*! \class RexTimerEventGenerator chg_timer.h "chg/chg_timer.h"
 *  \brief This is the class that defines the interface for managing all the charging
 *  related timers, using REX timer interface
 *  \details This is the class that defines the interface for publishing all the charging
 *  related timer events, using REX timer interface, including providing a generic timer
 *  call back and creating timer events. It is a child class of the TimerEventGenerator
 *  class.
 *  \note The charging events generated by this class should be in the \b interrupt  
 *  context, instead of in the task context.
 *  \note Only one instance of this class will exist in the Charing Application;
 *  It is generated and destroyed by the ChargingFactory class.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>Managing timers</td>
 *          <td> In the following ways:
 *              \li Providing a generic timer callback.
 *              \li Creating and destroying REX timers at runtime.
 *              \li Creating unique timer tokens to differentiate different timers.
 *              \li By providing interface for using the charging related timers.</td>
 *      </tr>
 *      <tr><td>Publishing new timer charging events</td>
 *          <td>By using the ChargingEventListener interface.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  \n None
 *
 *  <b>Generates the following events</b>
 *  <table border="0">
 *      <tr><td>::CHG_CMD__TIMER_EXPIRED</td>
 *          <td>timerCallBack() generate this.</td>
 *      </tr>
 *  </table>
 */
class RexTimerEventGenerator : public TimerEventGenerator
{
public:
    /*! \brief This member function provides an interface for adding the listener for
     *  the charging heart beat timer (::CHG_HEART_BEAT)
     *  \param listener A listener
     *  \return void
     *  \note The charging heart beat timer is a recurring timer. This function should
     *  be called only when a ChargingEventListener class wants to get informed when
     *  the charging heat beat timer expires.
     */ 
    virtual void AddListenerForChargingHeartBeat(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the charging heart beat timer (::CHG_HEART_BEAT)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the charging heat beat timer expires.
     */ 
    virtual void RemoveListenerForChargingHeatBeat(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-charging detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-charging detection timer is a one-shot timer. This function should 
     *  be called only when a ChargingEventListener class wants to get informed when  the
     *  end-of-charging detection timer expires.
     */ 
    virtual void AddListenerForEndOfChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the charging heart beat timer (::AUTO_CHARGING_END_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-charging detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  resuming-charging detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note The resume-charging detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  resume-charging detection timer expires.
     */ 
    virtual void AddListenerForResumeChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the resume-charging detection timer (::AUTO_CHARGING_RESUME_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the resume-charging  detection timer expires.
     */ 
    virtual void RemoveListenerForResumeChargingDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-fast-charge detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-fast-charge detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  end-of-fast-charge detection timer expires.
     */ 
    virtual void AddListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the end-of-fast-charge detection timer (::AUTO_CHARGING_END_FAST_CHARGE_DETECTION_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-fast-charge  detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfFastChargeDetectionTimeout(ChargingEventListener *listener);
#if defined(FEATURE_CHG_TASK_USE_CV_TOPOFF) || defined(T_WINNT)
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-CV(Constant Voltage)-topoff detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-CV-topoff detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  end-of-CV-topoff detection timer expires.
     */ 
    virtual void AddListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the end-of-CV-topoff detection timer (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_1_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-CV-topoff  detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfCVTopoffDetection1Timeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for adding the listener for
     *  end-of-CV(Constant Voltage)-topoff detection timer in the autonomous charging 
     *  (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T)
     *  \param listener A listener
     *  \return void
     *  \note The end-of-CV-topoff detetion timer is a recurring timer. This function should be 
     *  called only when a ChargingEventListener class wants to get informed when the 
     *  end-of-CV-topoff detection timer expires.
     */ 
    virtual void AddListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener *listener);
    /*! \brief This member function provides an interface for removing the listener from
     *  listening for the end-of-CV-topoff detection timer (::AUTO_CHARGING_END_CV_TOPOFF_DETECTION_2_T)
     *  \param listener A listener
     *  \return void
     *  \note This function should be called only when a ChargingEventListener class no
     *  longer wants to get informed when the end-of-CV-topoff  detection timer expires.
     */ 
    virtual void RemoveListenerForEndOfCVTopoffDetection2Timeout(ChargingEventListener *listener);
#endif /* FEATURE_CHG_TASK_USE_CV_TOPOFF */

    /*! \brief This member function adds the specific listener
     *  \details This member function adds the specific listener, with its information 
     *  specified by the parameter, to the timer event generator's listener list. It also
     *  create a REX timer that the listener could listen for, if the timer has not existed
     *  yet.
     *  \param listener A listener that wants to be informed when a charging related 
     *  timer expires.
     *  \param firstdelay The delay of the first expiry.
     *  \param repeatdelay The delay of the repeated expiries.
     *  \param shared A boolean that tells if this timer is going to be shared by multiple
     *  listenners.
     *  \return void
     *  \note This function should be called when a class wants to be informed when a 
     *  specific timer expires.
     */ 
     virtual void AddListener(
        ChargingEventListener *listener,
        unsigned int firstdelay, 
        unsigned int repeatdelay,
        bool shared
        );
     /*! \brief This member function removes the specific listener
      *  \details This member function removes the specific listener, with its information
      *  specified by the parameter, from the timer event generator's listener list. It
      *  also destroys the timer if no one else listeners for it.
      *  \param listener A listener that no long wants to listen for the timer
      *  \param firstdelay The delay of the first expiry.
      *  \param repeatdelay The delay of the repeated expiries.
      *  \return void
      *  \note This function should be called when a class no longer wants to be informed 
      *  when a specific timer expires.
      */ 
     virtual void RemoveListener(
        ChargingEventListener *listener,
        unsigned int firstdelay, 
        unsigned int repeatdelay
    );
protected:
    /*! \struct TimerLinkInfo
     *  \brief Used to store the link information about the timers
     *  \details A structure for holding the link informatmion of the timer.
     *  \note this type is used to store timer information in the timer queue.
     */
    typedef struct
    {
        /*! Queue link.                                  */
        q_link_type                     mLink;    
        /*! Timer                                        */
        timer_type*                     mpTimer;  
    }TimerLinkInfo;

private:
    /*! \brief This static member function is the timer call back funtion
     *  \param time_ms The timer expiry
     *  \param data The data associated with the timer. This data is used to as the timer
     *  token that differentiates various timers.
     *  \return void
     *  \note This function should only be called when a timer expires.
     */
    static void timerCallBack(int32 time_ms, timer_cb_data_type data);
    /*! \brief This member function returns a buffer from the timer free queue 
     *  \param void
     *  \return a pointer to the buffer
     *  \note The returned buffer may be NULL if there were no free timer buffers
     *  available. 
     */ 
    TimerLinkInfo* getTimerBuffer(void);
    /*! \brief This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue 
     *  \details This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue. This function is used  
     *  to determine if an item is in the queue by comparing the first delay of the
     *  timer and the repeat delay of the timer. 
     *  \param item A pointer to the comparing item.
     *  \param compare_val A pointer to the compared value.
     *  \return No-zero is the item is in the queue, zero otherwise
     *  \note This function compares the first delay of the timer and the repeat delay
     *  of the timer.
     */ 
    static int timerInformationCompare(void* item, void* compare_val);
    /*! \brief This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue 
     *  \details This static member function is the user defined compare function 
     *  that is required when doing linear search in a queue. This function is used  
     *  to determine if an item is in the queue by comparing the token of the timer.
     *  \param item A pointer to the comparing item.
     *  \param compare_val A pointer to the compared value.
     *  \return No-zero is the item is in the queue, zero otherwise
     *  \note This function compares the token of the timer.
     */ 
    static int tokenInformationCompare(void* item, void* compare_val);

private:
    /*! \brief The timer buffer 
     */
    TimerLinkInfo         mTimerBuffer[CHG_TIMER_CNT ];
    /*! \brief The timer queue 
     *  \details This timer queue contains the timer information for each  
     *  individual timers.
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s):  
     *  \li AddListener()
     *  \li RemoveListener()
     */
    q_type                mTimerQueue;
    /*! \brief The timer free queue 
     *  \details This timer queue contains the unused listener buffers
     *  \note This variable should be initialized before using. It is updated by 
     *  the following member function(s): 
     *  \li AddListener()
     *  \li RemoveListener()
     */
    q_type                mTimerFreeQueue;
private:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     */
    RexTimerEventGenerator();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~RexTimerEventGenerator();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
#ifdef T_WINNT
#error code not present
#endif
}; /* end class RexChargerTimerEventGenator */

#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif /* CHG_TIMER_H */

