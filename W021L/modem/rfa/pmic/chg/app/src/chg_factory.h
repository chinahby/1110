#ifndef CHG_FACTORY_H
#define CHG_FACTORY_H
/*! \file 
 *  \n
 *  \brief  chg_factory.h ---- CHARGING FACTORY CLASS DECLARATION
 *  \details This header file contains class declarations of the ChargingFactory 
 *  \n
 *  \n This header file contains class declarations of class ChargingFactory.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_factory.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   hs      Removed unnecessary header file inclusion.
09/17/09   hs      Removed some redundent functions.
06/08/09   hs      Added support for CV topoff.
05/27/09   hs      Added support for pulse charging.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
04/18/08   hs      Added 4 APIs for charging UI interface.
07/11/07   hs      Created.

========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

/*===========================================================================

                         TYPE AND CONSTANT DEFINITIONS

===========================================================================*/
/*! \def CHARGER_TIMER_EVENT_GENERATOR_VER
 *  \brief Used to distinguish different OS.
 *  \li When it's 1, it means we are using REX.
 */
#define CHARGER_TIMER_EVENT_GENERATOR_VER                 1
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
class TimerEventGenerator;
class RexTimerEventGenerator;
class IrqEventGenerator;
class SystemEventGenerator;
class ChargingDebugger;
class AutoChargingManager;

#ifdef T_WINNT
#error code not present
#endif
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/
                  /**************************************/
                  /*     class ChargingFactory          */
                  /**************************************/

/*! \class ChargingFactory chg_factory.h "chg/chg_factory.h"
 *  \brief  This is the class for creating instances of various classes in the
 *  charger task
 *  \details This is the class that defines the interface for creating instances of
 *  all the classes in the charger task. 
 *  \note The instances of this class should not be created.
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
class ChargingFactory
{
public:
    /*! \brief A destructor.
     */
    virtual ~ChargingFactory();
    /*! \brief This static member function returns a single instance of class  
     *  SystemEventGenerator
     *  \note This function should get called when there is a need to get an
     *  instance of the SystemEventGenerator class. 
     */
    static SystemEventGenerator*       GetSystemEventGenerator();
    /*! \brief This static member function returns a boolean which tells you if 
     *  the pointer mpSystemEventGenerator* is NULL or not
     */
    static bool IsSystemEventGeneratorNull();
    /*! \brief This static member function returns a single instance of class  
     *  ChargerTimerEventGenerator 
     *  \note This function should get called when there is a need to get an
     *  instance of the TimerEventGenerator class. 
     */
    static TimerEventGenerator*          GetTimerEventGenerator();
    /*! \brief This static member function returns a boolean which tells you if 
     *  the pointer mpTimerEventGenerator* is NULL or not
     */
    static bool IsTimerEventGeneratorNull();
    /*! \brief This static member function returns a single instance of class  
     *  IrqEventGenerator 
     *  \note This function should get called when there is a need to get an
     *  instance of the IrqEventGenerator class. 
     */
    static IrqEventGenerator*            GetIrqEventGenerator();
    /*! \brief This static member function returns a boolean which tells you if 
     *  the pointer mpIrqEventGenerator* is NULL or not
     */
    static bool IsIrqEventGeneratorNull();
    /*! \brief This static member function returns a single instance of class  
     *  ChargingDebugger 
     *  \note This function should get called when there is a need to get an
     *  instance of the ChargingDebugger class. 
     */
    static ChargingDebugger*             GetChargingDebugger();
    /*! \brief This static member function returns a boolean which tells you if 
     *  the pointer mpChargingDebugger* is NULL or not
     */
    static bool IsChargingDebuggerNull();
    /*! \brief This static member function returns a single instance of class  
     *  AutoChargingManager 
     *  \note This function should get called when there is a need to get an
     *  instance of the AutoChargingManager class. 
     */
    static AutoChargingManager*          GetAutoChargingManager();
    /*! \brief This static member function returns a boolean which tells you if 
     *  the pointer mpAutoChargingManager* is NULL or not
     */
    static bool IsAutoChargingManagerNull();
    /*! \brief This static member function deletes all the memory that is used   
     *  in the charging application 
     *  \note This function should get called when there is a need to clean up
     *  all the memory used in the charging application. 
     */
    static void                          DestroyAll();
private:
    /*! \brief This static member function returns a single instance of class  
     *  RexTimerEventGenerator 
     */
    static RexTimerEventGenerator*       getRexTimerEventGenerator();
private:
    
    /*! \brief A contructor.
     */
    ChargingFactory();
    /*! \brief ChargingEventGenerator instance
     */
    static SystemEventGenerator          *mpSystemEventGenerator;
    /*! \brief Charging related Rex timer event generator instance
     */
    static TimerEventGenerator           *mpRexTimerEventGenerator;
    /*! \brief PMIC IRQ event generator instance
     */
    static IrqEventGenerator             *mpIrqEventGenerator;
    /*! \brief Charging Debugger instance
     */
    static ChargingDebugger              *mpChargingDebugger;
    /*! \brief Charging QXDM Debugger instance
     */
    static ChargingDebugger              *mpChargingQxdmDebugger;
    /*! \brief Auto charging manager instance
     */
    static AutoChargingManager           *mpAutoChargingManager;
    /*! \brief Auto charging manager debug instance
     */
    static AutoChargingManager           *mpAutoChargingManagerDebug;
    /*! \brief AutoChargingManager_PulseCharg instance
     */
    static AutoChargingManager           *mpAutoChargingManager_PulseCharger;
    /*! \brief AutoChargingManager_PulseChargingDebug instance
     */
    static AutoChargingManager           *mpAutoChargingManagerDebug_PulseCharger;
    /*! \brief AutoChargingManagerCVTopoff instance
     */
    static AutoChargingManager           *mpAutoChargingManagerCVTopoff;
    /*! \brief Debug_AutoChargingManagerCVTopoff instance
     */
    static AutoChargingManager           *mpDebug_AutoChargingManagerCVTopoff;
#ifdef T_WINNT
#error code not present
#endif
};

#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif

