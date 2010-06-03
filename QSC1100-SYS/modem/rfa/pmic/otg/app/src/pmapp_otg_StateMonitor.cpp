/*! \file 
 *  \n
 *  \brief  pmapp_otg_StateMonitor.cpp ----- PM APP OTG State Monitor related class implementation
 *  \details This file contains implementation of USB state monitor classes methods 
 *    used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_StateMonitor.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_VbusStateMonitor.h"   

#ifdef FEATURE_PMIC_USB_OTG


/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
StateMonitorFactory* StateMonitorFactory::mpStateMonitorFactory_Instance = NULL;

IrqDebouncerStateManager* StateMonitorFactory::mpDbVbusValidStateMgr = NULL;
IrqDebouncerStateManager* StateMonitorFactory::mpDbVbusSessValidStateMgr = NULL;
IrqDebouncerStateManager* StateMonitorFactory::mpDbIdFltStateMgr = NULL;
IrqDebouncerStateManager* StateMonitorFactory::mpDbIdGndStateMgr = NULL;
IrqDebouncerStateManager* StateMonitorFactory::mpDbDpHiStateMgr = NULL;
IrqDebouncerStateManager* StateMonitorFactory::mpDbDmHiStateMgr = NULL;

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

/**************************************************************************
     class UsbConnStateMonitor											  
**************************************************************************/
UsbConnStateMonitor::UsbConnStateMonitor()
{
	mpDbStateMonitorFactory = StateMonitorFactory::GetInstance();
}


UsbConnStateMonitor::~UsbConnStateMonitor()
{
   StateMonitorFactory::DestroyInstance();
}


/**************************************************************************
     class StateMonitorFactory											  
**************************************************************************/
StateMonitorFactory::StateMonitorFactory()
{
	mpDbVbusValidStateMgr = NULL;
	mpDbVbusSessValidStateMgr = NULL;
	mpDbIdFltStateMgr = NULL;
	mpDbIdGndStateMgr = NULL;
	mpDbDpHiStateMgr = NULL;
	mpDbDmHiStateMgr = NULL;
}

StateMonitorFactory::~StateMonitorFactory()
{
	delete mpDbVbusValidStateMgr;
	delete mpDbVbusSessValidStateMgr;
	delete mpDbIdFltStateMgr;
	delete mpDbIdGndStateMgr;
	delete mpDbDpHiStateMgr;
	delete mpDbDmHiStateMgr;
}


StateMonitorFactory* StateMonitorFactory::GetInstance()
{
	if(NULL == mpStateMonitorFactory_Instance)
	{
		mpStateMonitorFactory_Instance = new StateMonitorFactory;
	}
	return mpStateMonitorFactory_Instance;
}

void StateMonitorFactory::DestroyInstance()
{
   delete mpStateMonitorFactory_Instance;
   mpStateMonitorFactory_Instance = NULL;
}



IrqDebouncerStateManager* StateMonitorFactory::GetIrqDebouncerStateManager(UsbLineDebouncerStateMonitorType stateManagerId)
{
	IrqDebouncerStateManager* pIrqDbStateMgr = NULL;

	switch(stateManagerId)
	{
	case VBUS_VALID_DEBOUNCER_STATE_MONITOR:
		if(NULL == mpDbVbusValidStateMgr)
		{
			mpDbVbusValidStateMgr = new IrqDebouncerStateManager;
            mpDbVbusValidStateMgr->PreInit();
			pIrqDbStateMgr = mpDbVbusValidStateMgr;
		}
		else
		{
			pIrqDbStateMgr = mpDbVbusValidStateMgr;
		}
		break;
	case VBUS_SESSION_VALID_DEBOUNCER_STATE_MONITOR:
		if(NULL == mpDbVbusSessValidStateMgr)
		{
			mpDbVbusSessValidStateMgr = new IrqDebouncerStateManager;
            mpDbVbusSessValidStateMgr->PreInit();
			pIrqDbStateMgr = mpDbVbusSessValidStateMgr;
		}
		else
		{
			pIrqDbStateMgr = mpDbVbusSessValidStateMgr;
		}
		break;
	case ID_FLT_DEBOUNCER_STATE_MONITOR:
		if(NULL == mpDbIdFltStateMgr)
		{
			mpDbIdFltStateMgr = new IrqDebouncerStateManager;
            mpDbIdFltStateMgr->PreInit();
			pIrqDbStateMgr = mpDbIdFltStateMgr;
		}
		else
		{
			pIrqDbStateMgr = mpDbIdFltStateMgr;
		}
		break;
	case ID_GND_DEBOUNCER_STATE_MONITOR:
		if(NULL == mpDbIdGndStateMgr)
		{
			mpDbIdGndStateMgr = new IrqDebouncerStateManager;
            mpDbIdGndStateMgr->PreInit();
			pIrqDbStateMgr = mpDbIdGndStateMgr;
		}
		else
		{
			pIrqDbStateMgr = mpDbIdGndStateMgr;
		}
		break;
	case DP_HI_DEBOUNCER_STATE_MONITOR:
		if(NULL == mpDbDpHiStateMgr)
		{
			mpDbDpHiStateMgr = new IrqDebouncerStateManager;
            mpDbDpHiStateMgr->PreInit();
			pIrqDbStateMgr = mpDbDpHiStateMgr;
		}
		else
		{
			pIrqDbStateMgr = mpDbDpHiStateMgr;
		}
		break;
	case DM_HI_DEBOUNCER_STATE_MONITOR:
		if(NULL == mpDbDmHiStateMgr)
		{
			mpDbDmHiStateMgr = new IrqDebouncerStateManager;
            mpDbDmHiStateMgr->PreInit();
			pIrqDbStateMgr = mpDbDmHiStateMgr;
		}
		else
		{
			pIrqDbStateMgr = mpDbDmHiStateMgr;
		}
		break;
	case DEBOUNCER_STATE_MONITOR_UNKNOWN:
		pIrqDbStateMgr = NULL;
		break;
	default:
		pIrqDbStateMgr = NULL;
		break;
	}

	return pIrqDbStateMgr;
}


#endif //FEATURE_PMIC_USB_OTG

