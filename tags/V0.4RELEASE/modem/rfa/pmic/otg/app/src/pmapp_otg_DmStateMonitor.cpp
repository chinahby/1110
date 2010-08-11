/*! \file 
 *  \n
 *  \brief  pmapp_otg_DmStateMonitor.cpp  ----- Dm State Monitor related class implementation
 *  \details This header file contains implementation for USB DM line voltage monitoring
 *     state machine used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_DmStateMonitor.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_DmStateMonitor.h"        

#ifdef FEATURE_PMIC_USB_OTG


/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
DmStateMonitor* DmStateMonitor::mpDmStateMonitor_Instance=NULL;

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
     class DmStateMonitor											  
**************************************************************************/
DmStateMonitor::DmStateMonitor():
	mDmHiDebouncerTmr_BothIrqOff(1),
	mDmHiDebouncerTmr_CompIrqOn(1)
{
	mpOtgDmHiStateMonitorCb = NULL;
	mDmSmInitialized = FALSE;
	mpDmIrqDbStateMgr = StateMonitorFactory::GetIrqDebouncerStateManager(DM_HI_DEBOUNCER_STATE_MONITOR);
}


DmStateMonitor::~DmStateMonitor()
{
}




DmStateMonitor* DmStateMonitor::GetInstance()
{
	if(NULL == mpDmStateMonitor_Instance )
	{
		mpDmStateMonitor_Instance = new DmStateMonitor;
	}
	return mpDmStateMonitor_Instance;
}



void DmStateMonitor::DestroyInstance()
{
   delete mpDmStateMonitor_Instance;
   mpDmStateMonitor_Instance = NULL;
}





void DmStateMonitor::DmStateMonitorInit(OtgDmSmListenerType listener, void* pListenerObj)
{
   pm_err_flag_type  init_status = PM_ERR_FLAG__SUCCESS;

   if (!mDmSmInitialized)
   {
      mpOtgDmHiStateMonitorCb = listener;
	  mpOtgDmHiListenerObj = pListenerObj;

	  init_status = mpDmIrqDbStateMgr->Init(
                       this,
                       PM_USB_D_MINUS_HI_RE_IRQ_HDL,
                       PM_USB_D_MINUS_HI_FE_IRQ_HDL,
                       PM_USB_D_MINUS_HI_RT_ST,
                       mDmHiDebouncerTmr_BothIrqOff,
                       mDmHiDebouncerTmr_CompIrqOn,
                       DmHiStatusCb);

      mDmSmInitialized = (PM_ERR_FLAG__SUCCESS == init_status);
   }
}




void DmStateMonitor::DmStateMonitorReset(void)
{
   if (mDmSmInitialized)
   {
	   mpDmIrqDbStateMgr->Reset();

      mDmSmInitialized = FALSE;
   }
}



void DmStateMonitor::DmHiStatusCb(IrqDebouncerStatusType status, void* pStateObj)
{
   DmStateMonitor* pDmStateMonitor = static_cast< DmStateMonitor* >((void*)pStateObj);
   ASSERT( pDmStateMonitor != NULL );


   if (IRQ_DEBOUNCER_STATUS__HIGH == status)
   {
      pDmStateMonitor->mDmStateInfo = OTG_DM_SM_INFO__HIGH; // "D- HI" has happened
   }
   else
   {
      pDmStateMonitor->mDmStateInfo = OTG_DM_SM_INFO__LOW; // "D- HI" became false
   }

   // and notify our listener about "DM HI" condition ...
   pDmStateMonitor->mpOtgDmHiStateMonitorCb(pDmStateMonitor->mDmStateInfo,pDmStateMonitor->mpOtgDmHiListenerObj);
}


#endif //FEATURE_PMIC_USB_OTG

