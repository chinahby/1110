/*! \file 
 *  \n
 *  \brief  chg_factory.cpp ---- CHARGING FACTORY CLASS DEFINITION
 *  \details This header file contains class definitions of the ChargingEvent
 *  class, the ChargingEventGenerator class and the ChargingEventListener 
 *  class.
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_factory.cpp#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/09   hs      Added support for software controlled CV topoff.
05/27/09   hs      Added support for pulse charging.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
04/18/08   hs      Added 4 APIs for charging UI interface.
                   Included "chg_config.h"
07/12/07   hs      Created.

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#ifdef T_WINNT
#error code not present
#else
#include "assert.h"                /* For ASSERT()                         */
#endif
#include "chg_common.h"            /* For various definitions              */
#include "chg_factory.h"           /* For class ChargingFactory            */
#include "chg_timer.h"             /* For class TimerEventGenerator 
                                          class RexTimerEventGenerator
                                          class TimerEvent                 */
#include "chg_isr.h"               /* For class IrqEventGenerator 
                                          class ChargerIrqEvent            */
#include "chg_system.h"            /* For class SystemEventGenerator       */
#include "chg_debug.h"             /* For class ChargingQxdmDebugger       */
#include "chg_config.h"            /* For various type definitions         */
#include "chg_autocharging.h"      /* For class AutoChargingManager        */
#ifdef T_WINNT
#error code not present
#endif
 /* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
SystemEventGenerator          *ChargingFactory::mpSystemEventGenerator = NULL;
TimerEventGenerator           *ChargingFactory::mpRexTimerEventGenerator = NULL;
IrqEventGenerator             *ChargingFactory::mpIrqEventGenerator = NULL;
ChargingDebugger              *ChargingFactory::mpChargingDebugger = NULL;
ChargingDebugger              *ChargingFactory::mpChargingQxdmDebugger = NULL;
AutoChargingManager           *ChargingFactory::mpAutoChargingManager = NULL;
AutoChargingManager           *ChargingFactory::mpAutoChargingManagerDebug = NULL;
AutoChargingManager           *ChargingFactory::mpAutoChargingManager_PulseCharger = NULL;
AutoChargingManager           *ChargingFactory::mpAutoChargingManagerDebug_PulseCharger = NULL;
AutoChargingManager           *ChargingFactory::mpAutoChargingManagerCVTopoff = NULL;
AutoChargingManager           *ChargingFactory::mpDebug_AutoChargingManagerCVTopoff = NULL;
/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
                  /**************************************/
                  /*     class ChargingFactory          */
                  /**************************************/
ChargingFactory::ChargingFactory()
{
}

ChargingFactory::~ChargingFactory()
{
}

SystemEventGenerator*  ChargingFactory::GetSystemEventGenerator()
{
    if( mpSystemEventGenerator == NULL )
    {
        mpSystemEventGenerator = new SystemEventGenerator();
        #if defined(T_WINNT)
#error code not present
        #endif
        ASSERT(mpSystemEventGenerator != NULL);
    }

    return mpSystemEventGenerator;
}

/*! \brief This static member function returns a single instance of class  
 *  ChargerTimerEventGenerator. 
 */
bool ChargingFactory::IsSystemEventGeneratorNull()
{
    return mpSystemEventGenerator == NULL;
}

/*! \brief This static member function returns a single instance of class  
 *  ChargerTimerEventGenerator. 
 */
TimerEventGenerator* ChargingFactory::GetTimerEventGenerator()
{
    return getRexTimerEventGenerator();
}

/*! \brief This static member function returns a single instance of class  
 *  RexTimerEventGenerator. 
 */
RexTimerEventGenerator*  ChargingFactory::getRexTimerEventGenerator()
{
    if( mpRexTimerEventGenerator == NULL )
    {
        mpRexTimerEventGenerator = new RexTimerEventGenerator();
        #if defined(T_WINNT)
#error code not present
        #endif
        ASSERT(mpRexTimerEventGenerator != NULL);
    }

    return static_cast<RexTimerEventGenerator*>(mpRexTimerEventGenerator);
}

/*! \brief This static member function returns a single instance of class  
 *  Battery. 
 */
bool ChargingFactory::IsTimerEventGeneratorNull()
{
    return mpRexTimerEventGenerator == NULL;
}

/*! \brief This static member function returns a single instance of class  
 *  IrqEventGenerator. 
 */
IrqEventGenerator* ChargingFactory::GetIrqEventGenerator()
{
    if( mpIrqEventGenerator == NULL )
    {
        mpIrqEventGenerator = new IrqEventGenerator();
        #if defined(T_WINNT)
#error code not present
        #endif
        ASSERT(mpIrqEventGenerator != NULL);
    }

    return mpIrqEventGenerator;
}

bool ChargingFactory::IsIrqEventGeneratorNull()
{
    return mpIrqEventGenerator == NULL;
}

/*! \brief This static member function returns a single instance of class  
 *  ChargingDebugger. 
 */
ChargingDebugger*  ChargingFactory::GetChargingDebugger()
{
    //Enable QXDM
    if( mpChargingQxdmDebugger == NULL )
    {
        mpChargingQxdmDebugger = new ChargingQxdmDebugger();
        #if defined(T_WINNT)
#error code not present
        #endif
        ASSERT(mpChargingQxdmDebugger != NULL);
    }

    return mpChargingQxdmDebugger;
}

/*! \brief This static member function returns a single instance of class  
 *  CommonIdle. 
 */
bool ChargingFactory::IsChargingDebuggerNull()
{
    return mpChargingDebugger == NULL;
}

AutoChargingManager* ChargingFactory::GetAutoChargingManager()
{
    /* Enable Debugger */
    #if CHG_ENABLE_DEBUGGER
        #ifdef T_WINNT
#error code not present
        #else
             #if defined( FEATURE_CHG_TASK_USE_PULSE_TOPOFF) 
                 if(NULL == mpAutoChargingManagerDebug_PulseCharger)
                 {
                     mpAutoChargingManagerDebug_PulseCharger = new Debug_AutoChargingManagerPulseTopoff();
                     #if defined(T_WINNT)
#error code not present
                     #endif /* T_WINNT */
                     ASSERT(mpAutoChargingManagerDebug_PulseCharger != NULL);
                 }

                 return mpAutoChargingManagerDebug_PulseCharger;
             #elif defined( FEATURE_CHG_TASK_USE_CV_TOPOFF )
                 if(NULL == mpDebug_AutoChargingManagerCVTopoff)
                 {
                     mpDebug_AutoChargingManagerCVTopoff = new Debug_AutoChargingManagerCVTopoff();
                     #if defined(T_WINNT)
#error code not present
                     #endif
                     ASSERT(mpDebug_AutoChargingManagerCVTopoff != NULL);
                 }

                 return mpDebug_AutoChargingManagerCVTopoff;
             #else
                 if(NULL == mpAutoChargingManagerDebug)
                 {
                     mpAutoChargingManagerDebug = new Debug_AutoChargingManager();
                     #if defined(T_WINNT)
#error code not present
                     #endif
                     ASSERT(mpAutoChargingManagerDebug != NULL);
                 }

                 return mpAutoChargingManagerDebug;
             #endif /* FEATURE_CHG_TASK_USE_PULSE_CHARGER */
        #endif /* T_WINNT */
    #else
        #ifdef T_WINNT
#error code not present
        #else
            #if defined( FEATURE_CHG_TASK_USE_PULSE_TOPOFF)
                if(NULL == mpAutoChargingManager_PulseCharger)
                {
                    mpAutoChargingManager_PulseCharger = new AutoChargingManagerPulseTopoff();
                    #if defined(T_WINNT)
#error code not present
                    #endif
                    ASSERT(mpAutoChargingManager_PulseCharger != NULL);
                }

                return mpAutoChargingManager_PulseCharger;
            #elif defined( FEATURE_CHG_TASK_USE_CV_TOPOFF )
                if(NULL == mpAutoChargingManagerCVTopoff)
                {
                    mpAutoChargingManagerCVTopoff = new AutoChargingManagerCVTopoff();
                    #if defined(T_WINNT)
#error code not present
                    #endif
                    ASSERT(mpAutoChargingManagerCVTopoff != NULL);
                }

                return mpAutoChargingManagerCVTopoff;
            #else
                if(NULL == mpAutoChargingManager)
                {
                    mpAutoChargingManager = new AutoChargingManager();
                    #if defined(T_WINNT)
#error code not present
                    #endif
                    ASSERT(mpAutoChargingManager != NULL);
                }

                return mpAutoChargingManager;
            #endif /* FEATURE_CHG_TASK_USE_PULSE_CHARGER */
        #endif /* T_WINNT */
    #endif /* CHG_ENABLE_DEBUGGER */
}

bool ChargingFactory::IsAutoChargingManagerNull()
{
    return mpAutoChargingManager == NULL                           || 
           mpAutoChargingManagerDebug == NULL                      ||
           mpAutoChargingManager_PulseCharger == NULL              ||
           mpAutoChargingManagerDebug_PulseCharger == NULL         ||
           mpAutoChargingManagerCVTopoff == NULL                   ||
           mpDebug_AutoChargingManagerCVTopoff == NULL;
}

/*! \brief This static member function deletes all the memory that is used   
 *  in the charging application. 
 */
void ChargingFactory::DestroyAll()
{
    #if defined(T_WINNT)
#error code not present
    #endif

    /* delete memory */
    if(NULL != mpSystemEventGenerator)
    {
        #if defined(T_WINNT)
#error code not present
        #endif

        delete mpSystemEventGenerator;
        mpSystemEventGenerator = NULL;
    }
    if(NULL != mpRexTimerEventGenerator)
    {
        #if defined(T_WINNT)
#error code not present
        #endif

        delete mpRexTimerEventGenerator;
        mpRexTimerEventGenerator = NULL;
    }
    if(NULL != mpIrqEventGenerator)
    {
        #if defined(T_WINNT)
#error code not present
        #endif

        delete mpIrqEventGenerator;
        mpIrqEventGenerator = NULL;
    }
    if(NULL != mpChargingDebugger)
    {
        #if defined(T_WINNT)
#error code not present
        #endif

        delete mpChargingDebugger;
        mpChargingDebugger = NULL;
    }
    if(NULL != mpChargingQxdmDebugger)
    {
        #if defined(T_WINNT)
#error code not present
        #endif

        delete mpChargingQxdmDebugger;
        mpChargingQxdmDebugger = NULL;
    }
    if(NULL != mpAutoChargingManager)
    {
        #if defined(T_WINNT)
#error code not present
        #endif
        
        delete mpAutoChargingManager;
        mpAutoChargingManager = NULL;
    }
    if(NULL != mpAutoChargingManagerDebug)
    {
        delete mpAutoChargingManagerDebug;
        mpAutoChargingManagerDebug = NULL;
    }
    if(NULL != mpAutoChargingManager_PulseCharger)
    {
        delete mpAutoChargingManager_PulseCharger;
        mpAutoChargingManager_PulseCharger = NULL;
    }
    if(NULL != mpAutoChargingManagerDebug_PulseCharger)
    {
        delete mpAutoChargingManagerDebug_PulseCharger;
        mpAutoChargingManagerDebug_PulseCharger = NULL;
    }
    if(NULL != mpAutoChargingManagerCVTopoff)
    {
        delete mpAutoChargingManagerCVTopoff;
        mpAutoChargingManagerCVTopoff = NULL;
    }
    if(NULL != mpDebug_AutoChargingManagerCVTopoff)
    {
        delete mpDebug_AutoChargingManagerCVTopoff;
        mpDebug_AutoChargingManagerCVTopoff = NULL;
    }

    #if defined(T_WINNT)
#error code not present
    #endif

}

bool ChargingFactory::IsCharging()
{
    if( true == GetAutoChargingManager()->IsCharging() )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool ChargingFactory::IsBatteryValid()
{
    if( true == ChargingFactory::GetAutoChargingManager()->IsBatteryValid() )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool ChargingFactory::IsChargerValid()
{
    if( true == GetAutoChargingManager()->IsChargerValid() )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

chg_ui_event_type ChargingFactory::UiEventRead()
{
    return GetAutoChargingManager()->GetUiEvent();
}

#endif /* FEATURE_CHG_TASK || T_WINNT */
