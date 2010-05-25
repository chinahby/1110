/*! \file 
 *  \n
 *  \brief  chg_debug.cpp ---- DEBUG RELATED CLASS DEFINITION
 *  \details This header file contains class definitions of the BatteryDebugEvent
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

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_debug.cpp#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/09   hs      Added support for software controlled CV topoff.
06/03/09   hs      Added support for charing during call.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
02/18/09   hs      Added support for pulse charging.
01/30/09   hs      Added battery connected/removed debug info.
11/25/08   hs      Replaced "charging complete" with "charging enging" 
                   debug message.
11/18/08   hs      Added "charging complete" debug message.
09/23/08   vk      Fixed charger code warnings
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
10/03/07   hs      Created.

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) ||defined(T_WINNT)

#include "chg_debug.h"       /* For class declarations                   */
#include "msg.h"             /* For reporting of messages for            *  
                              * errors and debugging                     */
#ifdef T_WINNT
#error code not present
#else
#include "assert.h"          /* For ASSERT()                             */
#endif
#include "chg_factory.h"     /* For the ChargingFactory class            */
#include "chg_autocharging.h"/* For the AutoChargingManager class        */
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

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


    /****************************************/
    /*       class AutoChargingDebugEvent   */
    /****************************************/
AutoChargingDebugEvent::AutoChargingDebugEvent(chg_auto_charging_debug_info_type  information):
ChargingEvent(CHG_CMD__AUTO_CHARGING_DEBUG),
mAutoChargingInformation(information)
{

}
AutoChargingDebugEvent::~AutoChargingDebugEvent()
{

}

                  /**************************************/
                  /*     class ChargingDebugger         */
                  /**************************************/
ChargingDebugger::ChargingDebugger()
{
    AutoChargingManager* autoChargingManager = ChargingFactory::GetAutoChargingManager();
    autoChargingManager->AddListener(this);
}

ChargingDebugger::~ChargingDebugger()
{
    /* Remove itself as a listener to AutoChargingManager */
    if(false == ChargingFactory::IsAutoChargingManagerNull())
    {
        ChargingFactory::GetAutoChargingManager()->RemoveListener(this);
    }
    
}

void ChargingDebugger::EventFired(ChargingEvent* event)
{
    ASSERT( NULL != event );

    switch( event->GetEventId() )
    {
        case CHG_CMD__AUTO_CHARGING_DEBUG:
        {
            AutoChargingDebugEvent* autochargingevent = dynamic_cast< AutoChargingDebugEvent* >( event );

            ASSERT(autochargingevent != NULL);

            outputAutoChargingDebugMessages(autochargingevent->mAutoChargingInformation);
        }
        break;
    default:
        break;
    }

}

void ChargingDebugger::outputAutoChargingDebugMessages(chg_auto_charging_debug_info_type)
{

}

                  /**************************************/
                  /*     class ChargingQxdmDebugger     */
                  /**************************************/
ChargingQxdmDebugger::ChargingQxdmDebugger()
{

}

ChargingQxdmDebugger::~ChargingQxdmDebugger()
{

}

void ChargingQxdmDebugger::outputAutoChargingDebugMessages(chg_auto_charging_debug_info_type information)
{
    if( chg_get_debug_state() >= CHG_DEBUG_INF_MINIMUM )
    {
        if(information.change_of_state_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- change of state IRQ triggered; ", 0, 0, 0); 
            MSG_FATAL("Charging Debug: previous charging state = %d, the current charging state = %d; ", 
                      information.previous_state, 
                      information.current_state, 
                      0); 
        }
        if(information.end_of_charging_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- end of charging IRQ triggered; ", 0, 0, 0); 
        }
        if(information.fail_of_charging_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- fail of charging IRQ triggered; ", 0, 0, 0); 
        }
        if(information.bad_battery_temp_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- bad battery temp IRQ triggered; ", 0, 0, 0); 
        }
        if(information.battery_connect_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- battery connected IRQ triggered; ", 0, 0, 0); 
        }
        if(information.vbat_det_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- vbat_det IRQ triggered; ", 0, 0, 0); 
        }
        if(information.pulse_done_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- pulse_done IRQ triggered; ", 0, 0, 0); 
        }
        if(information.batt_alarm_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- battery alarm IRQ triggered; ", 0, 0, 0); 
        }
        if(information.vcp_irq_triggered  == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- VCP IRQ triggered; ", 0, 0, 0); 
        }
        if(information.chgtlimit_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- charger in temperature limiting IRQ triggered; ", 0, 0, 0); 
        }
        if(information.charger_irq_triggered == TRUE)
        {
            MSG_FATAL("Charging Debug: auto charging -- valid/invalid charger IRQ triggered; ", 0, 0, 0);

            if(information.charger_type == CHG_AUTO_CHARGING__CHARGER_NONE)
            {
                MSG_FATAL("Charging Debug: auto charging -- charger removed; ", 0, 0, 0);
            }
            else if(information.charger_type == CHG_AUTO_CHARGING__CHARGER_UCP)
            {
                MSG_FATAL("Charging Debug: auto charging -- A USB charging port attached; ", 0, 0, 0);
            }
            else if(information.charger_type == CHG_AUTO_CHARGING__CHARGER_SHP)
            {
                MSG_FATAL("Charging Debug: auto charging -- A USB standard host port attached; ", 0, 0, 0);
            }
        }

        if(information.charger_type != CHG_AUTO_CHARGING__CHARGER_NONE)
        {
            if(information.charger_type == CHG_AUTO_CHARGING__CHARGER_UCP)
            {
                MSG_FATAL("Charging Debug: auto charging -- charger type: USB charging port (UCP) ; ", 0, 0, 0);

            }
            else if(information.charger_type == CHG_AUTO_CHARGING__CHARGER_SHP)
            {
                MSG_FATAL("Charging Debug: auto charging -- charger type: USB standard host port (SHP) ; ", 0, 0, 0);
            }


            MSG_FATAL("Charging Debug: the current charging_state = %d; ",
                      information.current_state, 
                      0, 
                      0); 

            if(information.pulse_charging_on == TRUE)
            {
                MSG_FATAL("Charging Debug: pulse charging is on", 0, 0, 0);
            }

            if(information.cv_topoff_on == TRUE)
            {
                MSG_FATAL("Charging Debug: software controlled CV topoff is on", 0, 0, 0);
            }

            if(information.charging_in_final_cycle == TRUE)
            {
                MSG_FATAL("Charging Debug: auto charging -- in final cycle.; ", 0, 0, 0); 
            }
            if(information.charging_finished == TRUE)
            {
                MSG_FATAL("Charging Debug: auto charging -- complete!; ", 0, 0, 0); 
            }
        }

        if(information.TX_on == TRUE)
        {
            MSG_FATAL("Charging Debug: system is in call mode; ", 0, 0, 0); 
        }
        if(information.battery_removed == FALSE)
        {
            MSG_FATAL("Charging Debug: battery_voltage = %d; battery_temperature = %d; charging_current = %d; ",
                      information.battery_voltage, 
                      information.battery_temperature, 
                      information.charging_current); 
        }
        else
        {
            MSG_FATAL("Charging Debug: battery is removed !!", 0, 0, 0);
        }
    } /* end if( chg_get_debug_state() >= CHG_DEBUG_INF_MINIMUM ) */
}

#endif /* FEATURE_CHG_TASK || T_WINNT */
