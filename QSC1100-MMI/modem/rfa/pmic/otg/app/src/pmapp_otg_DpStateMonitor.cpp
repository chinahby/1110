/*! \file 
 *  \n
 *  \brief  pmapp_otg_DpStateMonitor.cpp ---- Dp line state monitor related class implementation
 *  \details This header file contains implementation for USB DP line voltage monitoring
 *    state machine used by the USB-OTG application developed for the 
 *    Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_DpStateMonitor.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_DpStateMonitor.h"

#ifdef FEATURE_PMIC_USB_OTG

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
DpStateMonitor* DpStateMonitor::mpDpStateMonitor_Instance=NULL;

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
     class DpStateMonitor											  
**************************************************************************/
DpStateMonitor::DpStateMonitor():
	mDpHiDebouncerTmr_BothIrqOff(1),
	mDpHiDebouncerTmr_CompIrqOn(1)
{
	mpOtgDpHiStateMonitorCb = NULL;
	mpOtgDpHiListenerObj = NULL;
	mDpSmInitialized = FALSE;
	mpDpIrqDbStateMgr = StateMonitorFactory::GetIrqDebouncerStateManager(DP_HI_DEBOUNCER_STATE_MONITOR);
}


DpStateMonitor::~DpStateMonitor()
{
}




DpStateMonitor* DpStateMonitor::GetInstance()
{
	if(NULL == mpDpStateMonitor_Instance )
	{
		mpDpStateMonitor_Instance = new DpStateMonitor;
	}
	return mpDpStateMonitor_Instance;
}



void DpStateMonitor::DestroyInstance()
{
   delete mpDpStateMonitor_Instance;
   mpDpStateMonitor_Instance = NULL;
}



void DpStateMonitor::DpStateMonitorInit(OtgDpSmListenerType listener, void* pListenerObj)
{
   pm_err_flag_type  init_status = PM_ERR_FLAG__SUCCESS;

   if (!mDpSmInitialized)
   {
       	mpOtgDpHiStateMonitorCb = listener;
		mpOtgDpHiListenerObj = pListenerObj;

	  init_status = mpDpIrqDbStateMgr->Init(
                       this,
                       PM_USB_D_PLUS_HI_RE_IRQ_HDL,
                       PM_USB_D_PLUS_HI_FE_IRQ_HDL,
                       PM_USB_D_PLUS_HI_RT_ST,
                       mDpHiDebouncerTmr_BothIrqOff,
                       mDpHiDebouncerTmr_CompIrqOn,
                       DpHiStatusCb); 

      mDpSmInitialized = (PM_ERR_FLAG__SUCCESS == init_status);
   }
}


void DpStateMonitor::DpStateMonitorReset(void)
{
   if (mDpSmInitialized)
   {
	   mpDpIrqDbStateMgr->Reset();
      mDpSmInitialized = FALSE;
   }
}


void DpStateMonitor::DpHiStatusCb(IrqDebouncerStatusType status, void* pStateObj)
{
   DpStateMonitor* pDpStateMonitor = static_cast< DpStateMonitor* >((void*)pStateObj);
   ASSERT( pDpStateMonitor != NULL );

   if (IRQ_DEBOUNCER_STATUS__HIGH == status)
   {
      pDpStateMonitor->mDpStateInfo = OTG_DP_SM_INFO__HIGH; // "D+ HI" has happened
   }
   else
   {
      pDpStateMonitor->mDpStateInfo = OTG_DP_SM_INFO__LOW; // "D+ HI" became false
   }

   // and notify our listener about "DP HI" condition ...
   pDpStateMonitor->mpOtgDpHiStateMonitorCb(pDpStateMonitor->mDpStateInfo, pDpStateMonitor->mpOtgDpHiListenerObj);
}



#endif //FEATURE_PMIC_USB_OTG

