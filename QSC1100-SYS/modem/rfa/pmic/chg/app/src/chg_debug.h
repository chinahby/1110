#ifndef CHG_DEBUG_H
#define CHG_DEBUG_H

/*! \file 
 *  \n
 *  \brief  chg_debug.h ---- DEBUG RELATED CLASS DECLARATION
 *  \details This header file contains class declarations of the BatteryDebugEvent
 *  class, the ChargerDebugEvent class, the ChargingStateDebugEvent class,
 *  the CurrentControlDebugEvent class, the PowerControlDebugEvent class,
 *  the ChargingDebugger class, and the ChargingQxdmDebugger class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_debug.h#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/09   hs      Removed unnecessary header file inclusion.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
10/03/07   hs      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#include "chg_evt.h"       /* For the ChargingEvent class                *
                                  the ChargingEventListener  class       */
/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
class ChargingFactory;
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/

                  /**************************************/
                  /*     class AutoChargingDebugEvent        */
                  /**************************************/
/*! \class AutoChargingDebugEvent chg_debug.h "chg/chg_debug.h"
 *  \n
 *  \brief This is the class that defines the interface for the auto charging debug
 *  events in the Charging Application
 *  \details This is the class that defines the interface for the auto charging debug
 *  events in the Charging Application. This class is a child class of the
 *  ChargingEvent class.
 *  \note The instances of this class should be generated and destroyed, at runtime,
 *  by the AutoChargingManagerDebug class.
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
class AutoChargingDebugEvent: public ChargingEvent
{
public:
    /*! \brief A contructor.
     *  \param information The auto charging information that's used for debug purpose.
     *  \return NA
     *  \details This constructor sets the membervariable mAutoChargingInformation
     *  through the parameter.
     */
    AutoChargingDebugEvent(chg_auto_charging_debug_info_type information);
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~AutoChargingDebugEvent();
protected:
    /*! \brief Battery information that's used for debug purpose.
     *  \details This variable should be set only by the constructor.
     */ 
    chg_auto_charging_debug_info_type      mAutoChargingInformation;
    /*! \brief friend class ChargingDebugger.
     *  \details Declare ChargingDebugger as a friend, so it could access the private
     *  membervariable mBatteryInformation. 
     */
friend class ChargingDebugger;
}; /* End of class AutoChargingDebugEvent */

                  /**************************************/
                  /*     class ChargingDebugger         */
                  /**************************************/
/*! \class ChargingDebugger chg_debug.h "chg/chg_debug.h"
 *  \brief This is the class that defines the interface for debugging related
 *  activities in the Charging Application
 *  \details This is the class that defines the interface for debugging related
 *  activities in the Charging Application. This class is the base class of the
 *  ChargingQxdmDebugger class. This class is also a child class of the
 *  ChargingEventListener class.
 *  \note Only one instance of this class should exist in the Charing Application;
 *  It is generated and destroyed by the ChargingFactory class.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>Managing debug events</td>
 *          <td>By providing interface for outputing the charging related debug 
 *              messages.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  \n None
 *
 *  <b>Generates the following events</b>
 *  \n None
 */
class ChargingDebugger: public ChargingEventListener
{
public:
    /*! \brief This member function handles the debug event passed by the parameter 
     *  \param event A charging debug event that needs to be handled.
     *  \return void
     *  \note This function should be called by a ChargingEventGenerator class when a 
     *  charging debug event is created.
     */ 
    virtual void EventFired(ChargingEvent* event);
protected:
    /*! \brief This member function outputs the interface for providing the auto charging 
     *  state related debug messages
     *  \param information The auto charging related debug informaiton.
     *  \return void
     *  \note This function should be called when this class receives any auto charging
     *  related debug event.
     */
    virtual void outputAutoChargingDebugMessages(
                                 chg_auto_charging_debug_info_type   information);
protected:
    /*! \brief A contructor.
     *  \param none
     *  \return NA
     *  \details This constructor adds the class as a listener to any kind of charging   
     *  debug events . Those classes are:
     *  \li AutoChargingManagerDebug
     */
    ChargingDebugger();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     *  \details This destructor removes this class as a listener from all the 
     *  classes that generate charging debug events. Those classes are:
     *  \li AutoChargingManagerDebug
     */
    virtual ~ChargingDebugger();
    /*! \brief friend class ChargingFactory
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
}; /* end of class ChargingDebugger */

                  /**************************************/
                  /*     class ChargingQxdmDebugger     */
                  /**************************************/
/*! \class ChargingQxdmDebugger chg_debug.h "chg/chg_debug.h"
 *  \brief This is the class for output debug messages through the QXDM terminal
 *  in Charging application
 *  \details This is the class for output debug messages through the QXDM terminal
 *  in Charging application. This class is a child class of the ChargingDebugger class. 
 *  \note Only one instance of this class should exist in the Charing Application;
 *  It is generated and destroyed by the ChargingFactory class.
 *
 *  <b>Handles the following</b>
 *  <table border="0">
 *      <tr><td>Managing debug events</td>
 *          <td>By providing interface for outputing the charging related debug 
 *              messages through QXDM terminal.</td>
 *      </tr>
 *  </table>
 *
 *  <b>Listens for the following events</b>
 *  \n None
 *
 *  <b>Generates the following events</b>
 *  \n None
 */
class ChargingQxdmDebugger: public ChargingDebugger
{
protected:
    /*! \brief This member function outputs the interface for providing the auto charging 
     *  state related debug messages
     *  \param information The auto charging related debug informaiton.
     *  \return void
     *  \note This function should be called when this class receives any auto charging
     *  related debug event.
     */
    virtual void outputAutoChargingDebugMessages(
                                 chg_auto_charging_debug_info_type   information);
private:
    /*! \brief A constructor.
     *  \param none
     *  \return NA
     */
    ChargingQxdmDebugger();
    /*! \brief A destructor.
     *  \param none
     *  \return NA
     */
    virtual ~ChargingQxdmDebugger();
    /*! \brief friend class ChargingFactory.
     *  \details Declare ChargingFactory as a friend, so it could create an instance of 
     *  this class.
     */
friend class ChargingFactory;
}; /* end of class ChargingQxdmDebugger */

#endif /* FEATURE_CHG_TASK || T_WINNT */

#endif /* CHG_DEBUG_H */

