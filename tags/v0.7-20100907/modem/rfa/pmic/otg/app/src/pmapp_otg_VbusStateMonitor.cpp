/*! \file 
 *  \n
 *  \brief  pmapp_otg_VbusStateMonitor.cpp  ----- Vbus State Monitor related class implementation
 *  \details This file contains the implementation of USB VBUS line voltage monitor classes VbusStateMonitor, 
 *   VbusValidStateMonitor and class VbusSessionStateMonitor methods used by the USB-OTG application
 *    developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_VbusStateMonitor.cpp#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/08   aab       Added FEATURE_PMIC_USB_DETECTION_USING_VCHG to support the use of 
                     VCHG interrupt in the absense of USB Vbus line.  This feature can only 
                     be used along witht he needed hardware change(External USB device's 
                     Vbus line would need to be routed to VCHG pin on PMIC). 
                     Also, this feature can only be used for devices that supports peripheral mode only.
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
VbusValidStateMonitor* VbusValidStateMonitor::mpVbusValidStateMonitor_Instance=NULL;
SessionValidStateMonitor* SessionValidStateMonitor::mpSessionValidStateMonitor_Instance=NULL;

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
     class VbusStateMonitor											  
**************************************************************************/
VbusStateMonitor::VbusStateMonitor()
{
}


VbusStateMonitor::~VbusStateMonitor()
{
}




/**************************************************************************
     class VbusValidStateMonitor											  
**************************************************************************/
VbusValidStateMonitor::VbusValidStateMonitor():
	mOtgVbusValid_DebouncerTmr_BothIrqOff(31),
	mOtgVbusValid_DebouncerTmr_CompIrqOn(31)
{
	mpOtgVbusValidStateMonitorCb = NULL;
	mpOtgVbusValidListenerObj = NULL;

	mOtgVbusValidSmInitialized = FALSE;
	mpVbusValidIrqDbStateMgr = StateMonitorFactory::GetIrqDebouncerStateManager(VBUS_VALID_DEBOUNCER_STATE_MONITOR);
}



VbusValidStateMonitor::~VbusValidStateMonitor()
{
}


VbusValidStateMonitor* VbusValidStateMonitor::GetInstance()
{
	if(NULL == mpVbusValidStateMonitor_Instance )
	{
		mpVbusValidStateMonitor_Instance = new VbusValidStateMonitor;
	}
	return mpVbusValidStateMonitor_Instance;
}

void VbusValidStateMonitor::DestroyInstance()
{
   delete mpVbusValidStateMonitor_Instance;
   mpVbusValidStateMonitor_Instance = NULL;
}


SessionValidStateMonitor* SessionValidStateMonitor::GetInstance()
{
	if(NULL == mpSessionValidStateMonitor_Instance )
	{
		mpSessionValidStateMonitor_Instance = new SessionValidStateMonitor;
	}
	return mpSessionValidStateMonitor_Instance;
}

void SessionValidStateMonitor::DestroyInstance()
{
   delete mpSessionValidStateMonitor_Instance;
   mpSessionValidStateMonitor_Instance = NULL;
}



void VbusValidStateMonitor::VbusValidStateMonitorInit(OtgVbusSmListenerType listener, void* pListenerObj)
{
   pm_err_flag_type  init_status__vbus_valid = PM_ERR_FLAG__SUCCESS;

   if (!mOtgVbusValidSmInitialized)
   {
      mpOtgVbusValidStateMonitorCb = listener;
	  mpOtgVbusValidListenerObj = pListenerObj;


      init_status__vbus_valid = 
		  mpVbusValidIrqDbStateMgr->Init(
		    this,
            PM_USB_BUS_VALID_RE_IRQ_HDL,
            PM_USB_BUS_VALID_FE_IRQ_HDL,
            PM_USB_BUS_VALID_RT_ST,
            mOtgVbusValid_DebouncerTmr_BothIrqOff,
            mOtgVbusValid_DebouncerTmr_CompIrqOn,
            //(void)VbusValidStatusCb );
            VbusValidStatusCb );


      mOtgVbusValidSmInitialized = ( (PM_ERR_FLAG__SUCCESS == init_status__vbus_valid) );

      if (!mOtgVbusValidSmInitialized)
      {
		  mpVbusValidIrqDbStateMgr->Reset();
      }
   }
}





/**************************************************************************
     class SessionValidStateMonitor											  
**************************************************************************/
SessionValidStateMonitor::SessionValidStateMonitor():
	mOtgSessionValid_DebouncerTmr_BothIrqOff(67),
	mOtgSessionValid_DebouncerTmr_CompIrqOn(67)
{
	mpOtgSessionValidStateMonitorCb = NULL;
	mpOtgSessionValidListenerObj = NULL;

	mOtgSessionValid_SmInitialized = FALSE;
	mpSessionValidIrqDbStateMgr = StateMonitorFactory::GetIrqDebouncerStateManager(VBUS_SESSION_VALID_DEBOUNCER_STATE_MONITOR);
}


SessionValidStateMonitor::~SessionValidStateMonitor()
{
}


void SessionValidStateMonitor::SessionValidStateMonitorInit(OtgVbusSmListenerType listener, void* pListenerObj)
{
   pm_err_flag_type  init_status__session_valid = PM_ERR_FLAG__SUCCESS;

   if (!mOtgSessionValid_SmInitialized)
   {
      mpOtgSessionValidStateMonitorCb = listener;
	  mpOtgSessionValidListenerObj = pListenerObj;


#ifdef FEATURE_PMIC_USB_DETECTION_USING_VCHG //External USB device Vbus line would need to be routed to VCHG pin on PMIC
      init_status__session_valid =
		  mpSessionValidIrqDbStateMgr->Init(
			this,
            PM_VALID_CHG_IRQ_HDL,
            PM_INVALID_CHG_IRQ_HDL,
            PM_VALID_CHG_RT_ST,
            mOtgSessionValid_DebouncerTmr_BothIrqOff,
            mOtgSessionValid_DebouncerTmr_CompIrqOn,
            SessionValidStatusCb);
#else
    init_status__session_valid =
		  mpSessionValidIrqDbStateMgr->Init(
			this,
            PM_USB_SN_VALID_RE_IRQ_HDL,
            PM_USB_SN_VALID_FE_IRQ_HDL,
            PM_USB_SN_VALID_RT_ST,
            mOtgSessionValid_DebouncerTmr_BothIrqOff,
            mOtgSessionValid_DebouncerTmr_CompIrqOn,
            SessionValidStatusCb);
#endif //FEATURE_PMIC_USB_DETECTION_USING_VCHG


      mOtgSessionValid_SmInitialized = ( PM_ERR_FLAG__SUCCESS == init_status__session_valid );

      if (!mOtgSessionValid_SmInitialized)
      {
		  mpSessionValidIrqDbStateMgr->Reset();
      }
   }
}




void VbusValidStateMonitor::VbusValidStateMonitorReset(void)
{
   if (mOtgVbusValidSmInitialized)
   {  
	  mpVbusValidIrqDbStateMgr->Reset();
      mOtgVbusValidSmInitialized = FALSE;
   }
}



void SessionValidStateMonitor::SessionValidStateMonitorReset(void)
{
   if (mOtgSessionValid_SmInitialized)
   {  
	  mpSessionValidIrqDbStateMgr->Reset();
      mOtgSessionValid_SmInitialized = FALSE;
   }
}


void VbusValidStateMonitor::VbusValidStatusCb(IrqDebouncerStatusType status, void* pStateObj)
{
   VbusValidStateMonitor* pVbusVldStMonitor = static_cast< VbusValidStateMonitor* >((void*)pStateObj);
   ASSERT( pVbusVldStMonitor != NULL );
   
   OtgVbusSmInfoType vbus_valid_info = OTG_VBUS_SM_INFO__VBUS_INVALID;

   if (IRQ_DEBOUNCER_STATUS__HIGH == status)
   {
      vbus_valid_info = OTG_VBUS_SM_INFO__VBUS_VALID;
   }

   // notify our listener about updates to vbus_valid condition ...
   pVbusVldStMonitor->mpOtgVbusValidStateMonitorCb(vbus_valid_info, pVbusVldStMonitor->mpOtgVbusValidListenerObj);
}




void SessionValidStateMonitor::SessionValidStatusCb(IrqDebouncerStatusType status, void* pStateObj)
{
   SessionValidStateMonitor* pSessionVldStMonitor = static_cast< SessionValidStateMonitor* >((void*)pStateObj);
   ASSERT( pSessionVldStMonitor != NULL );
   //UsbBusListener* pUsbBusListener = UsbBusListener::GetInstance(); //we can also make a direct call

   OtgVbusSmInfoType vbus_session_valid_info = OTG_VBUS_SM_INFO__VBUS_SESSION_INVALID;

   if (IRQ_DEBOUNCER_STATUS__HIGH == status)
   {
      vbus_session_valid_info = OTG_VBUS_SM_INFO__VBUS_SESSION_VALID;
   }

   // Notify our listener about updates to vbus_session_valid condition ...
   pSessionVldStMonitor->mpOtgSessionValidStateMonitorCb(vbus_session_valid_info, pSessionVldStMonitor->mpOtgSessionValidListenerObj);
}



#endif //FEATURE_PMIC_USB_OTG
