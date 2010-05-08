/*! \file 
 *  \n
 *  \brief  pmapp_otg_IdStateMonitor.h  ----- ID State Monitor related class implementation
 *  \details This header file contains implementation for USB ID line voltage monitoring
 *     state machine used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_IdStateMonitor.cpp#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/08   aab      Added timer_group_type defination and made related changes
08/08/08   aab      Made changes to properly initialize timer object
07/23/08   aab      Made changes to resolve compiler warnings
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_IdStateMonitor.h"  

#ifdef FEATURE_PMIC_USB_OTG


/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static timer_group_type  pm_app_otg_timer_group;

IdStateMonitor* IdStateMonitor::mpIdStateMonitor_Instance=NULL;
IdFltStateMonitor* IdFltStateMonitor::mpIdFltStateMonitor_Instance = NULL;
IdGndStateMonitor* IdGndStateMonitor::mpIdGndStateMonitor_Instance = NULL;
IdResStateMonitor* IdResStateMonitor::mpIdResStateMonitor_Instance = NULL;

OtgIdSmInfoType		IdStateMonitor::mIdStateInfo = OTG_ID_SM_INFO__UNKNOWN;

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
     class IdStateMonitor												  
**************************************************************************/

IdStateMonitor::IdStateMonitor()
{
	mpIdFltIrqDbStateMgr = StateMonitorFactory::GetIrqDebouncerStateManager(ID_FLT_DEBOUNCER_STATE_MONITOR);
	mpIdGndIrqDbStateMgr = StateMonitorFactory::GetIrqDebouncerStateManager(ID_GND_DEBOUNCER_STATE_MONITOR);
}


IdStateMonitor::~IdStateMonitor()
{
}



IdStateMonitor* IdStateMonitor::GetInstance()
{
	if(NULL == mpIdStateMonitor_Instance )
	{
		mpIdStateMonitor_Instance = new IdStateMonitor;
	}
	return mpIdStateMonitor_Instance;
}


void IdStateMonitor::DestroyInstance()  
{
   delete mpIdStateMonitor_Instance;
   mpIdStateMonitor_Instance = NULL;
}




/**************************************************************************
     class IdFltStateMonitor											  
**************************************************************************/

IdFltStateMonitor::IdFltStateMonitor():
	mIdFltDebouncerTmr_BothIrqOff(13),
	mIdFltDebouncerTmr_CompIrqOn(13)
{
	mIdStateInfo = OTG_ID_SM_INFO__UNKNOWN;
	mpOtgIdFltStateMonitorCb = NULL;
	mIdFltSmInitialized = FALSE;
    mpIdResStateMonitor = IdResStateMonitor::GetInstance();
}



IdFltStateMonitor::~IdFltStateMonitor()
{
}


IdFltStateMonitor* IdFltStateMonitor::GetInstance()
{
	if(NULL == mpIdFltStateMonitor_Instance )
	{
		mpIdFltStateMonitor_Instance = new IdFltStateMonitor;
	}
	return mpIdFltStateMonitor_Instance;
}


void IdFltStateMonitor::DestroyInstance()
{
   delete mpIdFltStateMonitor_Instance;
   mpIdFltStateMonitor_Instance = NULL;
}




void IdFltStateMonitor::IdFltStateMonitorInit(OtgIdSmListenerType listener, void* pListenerObj)
{
   pm_err_flag_type  idFlt_InitStatus = PM_ERR_FLAG__SUCCESS;

   if (!mIdFltSmInitialized)
   {
	  mpOtgIdFltStateMonitorCb = listener;
	  mpOtgIdFltListenerObj = pListenerObj;

      idFlt_InitStatus = 
		  mpIdFltIrqDbStateMgr->Init(
            this,
            PM_USB_ID_FLOATING_RE_IRQ_HDL,
            PM_USB_ID_FLOATING_FE_IRQ_HDL,
            PM_USB_ID_FLOATING_RT_ST,
            mIdFltDebouncerTmr_BothIrqOff,
            mIdFltDebouncerTmr_CompIrqOn,
            IdFltStatusCb);  


      mIdFltSmInitialized = ( PM_ERR_FLAG__SUCCESS == idFlt_InitStatus );

      if (mIdFltSmInitialized)
      {
         #ifndef T_WINNT
             // Initialize ID state monitor detection timer
             timer_def2(&mIdSmType, &pm_app_otg_timer_group );
         #endif
      }
      else
      {
		  mpIdFltIrqDbStateMgr->Reset();
      }
   }
}




void IdFltStateMonitor::IdFltStateMonitorReset(void)
{
   if (mIdFltSmInitialized)
   {
      #ifndef T_WINNT
	     timer_clr( &mIdSmType, T_NONE );
      #endif
	  mpIdFltIrqDbStateMgr->Reset();
	  mIdFltSmInitialized = FALSE;
   }
}



void IdFltStateMonitor::IdFltStatusCb(IrqDebouncerStatusType status, void* pStateObj)
{  
   IdFltStateMonitor* pIdFltStMonitor = static_cast< IdFltStateMonitor* >((void*)pStateObj);
   ASSERT( pIdFltStMonitor != NULL );


   if (IRQ_DEBOUNCER_STATUS__HIGH == status)
   {
      // Adding a check to make sure ID floating detection is not
      //  done in a redundant manner since this happens at 2 places:
      //  - [1] ID_GND becoming LOW (after removal of mini-A plug)
      //  - [2] ID_FLOATING becoming HIGH (after removal of RS-232 dongle)
      
      if (OTG_ID_SM_INFO__FLT != pIdFltStMonitor->mIdStateInfo)
      {
         pIdFltStMonitor->mIdStateInfo = OTG_ID_SM_INFO__FLT;

         // Stop ID resistive state detection timer since we
         //  know that ID is FLOATING now
		 pIdFltStMonitor->mpIdResStateMonitor->IdStopResDetectTimer();

         // Notify our listener about floating ID condition ...
		 pIdFltStMonitor->mpOtgIdFltStateMonitorCb(pIdFltStMonitor->mIdStateInfo, pIdFltStMonitor->mpOtgIdFltListenerObj);
      }
   }
   else
   {
      //IRQ_DEBOUNCER_STATUS__LOW

      // ID_FLOATING going LOW could be because of 2 reasons:
      //  - [1] Mini-A plug is connected (eventual state is ID_GND)
      //  - [2] Resistive ID plug (like that of RS-232 dongle) is connected
      //        (eventual state is ID resistive)
      
      // Start ID resistive state detection timer. If this
      //  times out before ID_GND status becomes HIGH, then
      //  it means that ID is resistive since ID_GND and ID_FLOATING
      //  will be both LOW at that time.
      pIdFltStMonitor->mpIdResStateMonitor->IdStartResDetectTimer();
   }
}






/*************************************************************************
     class IdGndStateMonitor											  
**************************************************************************/

IdGndStateMonitor::IdGndStateMonitor():
	mIdGndDebouncerTmr_BothIrqOff(47),
	mIdGndDebouncerTmr_CompIrqOn(47)
{
	mIdStateInfo = OTG_ID_SM_INFO__UNKNOWN;
	mpOtgIdGndStateMonitorCb = NULL;
	mIdGndSmInitialized = FALSE; 
    mpIdResStateMonitor = IdResStateMonitor::GetInstance();
}



IdGndStateMonitor::~IdGndStateMonitor()
{
}



IdGndStateMonitor* IdGndStateMonitor::GetInstance()
{
	if(NULL == mpIdGndStateMonitor_Instance )
	{
		mpIdGndStateMonitor_Instance = new IdGndStateMonitor;
	}
	return mpIdGndStateMonitor_Instance;
}



void IdGndStateMonitor::DestroyInstance()
{
   delete mpIdGndStateMonitor_Instance;
   mpIdGndStateMonitor_Instance = NULL;
}




void IdGndStateMonitor::IdGndStateMonitorInit(OtgIdSmListenerType listener, void* pListenerObj)
{
   pm_err_flag_type  idGnd_InitStatus = PM_ERR_FLAG__SUCCESS;

   if (!mIdGndSmInitialized)
   {
	  mpOtgIdGndStateMonitorCb = listener;
	  mpOtgIdGndListenerObj = pListenerObj;

      idGnd_InitStatus = 
		  mpIdGndIrqDbStateMgr->Init(
            this,
            PM_USB_ID_GND_RE_IRQ_HDL,
            PM_USB_ID_GND_FE_IRQ_HDL,
            PM_USB_ID_GND_RT_ST,
            mIdGndDebouncerTmr_BothIrqOff,
            mIdGndDebouncerTmr_CompIrqOn,
            IdGndStatusCb);


      mIdGndSmInitialized = ( PM_ERR_FLAG__SUCCESS == idGnd_InitStatus );

      if (mIdGndSmInitialized)// Id Gnd debouncer initialized successfully
      {
         #ifndef T_WINNT
            //Initialize ID state monitor detection timer
            timer_def2(&mIdSmType, &pm_app_otg_timer_group );
         #endif
      }
      else
      {
		  mpIdGndIrqDbStateMgr->Reset();
      }
   }
}


void IdGndStateMonitor::IdGndStateMonitorReset(void)
{
   if (mIdGndSmInitialized)
   {
      #ifndef T_WINNT
	     timer_clr( &mIdSmType, T_NONE );
      #endif
	  mpIdGndIrqDbStateMgr->Reset();
      mIdGndSmInitialized = FALSE;
   }
}




void IdGndStateMonitor::IdGndStatusCb(IrqDebouncerStatusType status, void* pStateObj)
{
   IdGndStateMonitor* pIdGndStMonitor = static_cast< IdGndStateMonitor* >((void*)pStateObj);
   ASSERT( pIdGndStMonitor != NULL );


   if (IRQ_DEBOUNCER_STATUS__HIGH == status)
   {
      pIdGndStMonitor->mIdStateInfo = OTG_ID_SM_INFO__GND;

      // Stop ID resistive state detection timer since we
      //  know that ID is GROUNDED now
	  pIdGndStMonitor->mpIdResStateMonitor->IdStopResDetectTimer();

      // Notify our listener about grounded ID condition ...
      pIdGndStMonitor->mpOtgIdGndStateMonitorCb(pIdGndStMonitor->mIdStateInfo, pIdGndStMonitor->mpOtgIdGndListenerObj);
   }
   else
   {
      //IRQ_DEBOUNCER_STATUS__LOW
      
      // ID_GND going LOW could be because of 2 reasons:
      //  - [1] Mini-A plug is removed (eventual state is ID_FLOATING)
      //  - [2] Resistive ID plug (like that of RS-232 dongle) is connected
      //        before PMIC is powered ON (eventual state is ID resistive)

      // We need to figure out which of the above is the cause of this instance
      // of ID line going low. Except for one situation where the resistive ID
      // plug is already plugged in before the xcvr is powered on, there is no
      // direct path from ID grounded to resistive state without an intervening
      // floating state (state of ID pin after removal of mini-A plug and
      // before plugging in of resistive ID plug like that of an RS-232 dongle)
       
      // If pm_app_otg_id_sm__my_info == OTG_ID_SM_INFO__UNKNOWN then it
      // means that the xcvr has just been powered ON and so it is possible that
      // a resistive ID has already been plugged in. Except for this situation,
      // the ID line is guaranteed to be floating.

      if (OTG_ID_SM_INFO__UNKNOWN == pIdGndStMonitor->mIdStateInfo)
      {
         // Start ID resistive state detection timer. If this
         //  times out before ID_FLOATING status becomes HIGH, then
         //  it means that ID is resistive since ID_GND and ID_FLOATING
         //  will be both LOW at that time.
		  pIdGndStMonitor->mpIdResStateMonitor->IdStartResDetectTimer();
      }
      else
      {
         // Adding a check to make sure ID floating detection is not
         //  done in a redundant manner since this happens at 2 places:
         //  - [1] ID_GND becoming LOW (after removal of mini-A plug)
         //  - [2] ID_FLOATING becoming HIGH (after removal of RS-232 dongle)
         if (OTG_ID_SM_INFO__FLT != pIdGndStMonitor->mIdStateInfo)
         {
            pIdGndStMonitor->mIdStateInfo = OTG_ID_SM_INFO__FLT;

            // Stop ID resistive state detection timer since we know 
            //  that ID is (going to be) FLOATING (anytime) now
			pIdGndStMonitor->mpIdResStateMonitor->IdStopResDetectTimer();

            // Notify our listener about floating ID condition ...
			pIdGndStMonitor->mpOtgIdGndStateMonitorCb(pIdGndStMonitor->mIdStateInfo, pIdGndStMonitor->mpOtgIdGndListenerObj);
         }
      }
   }
}





/**************************************************************************
     class IdResStateMonitor											  
**************************************************************************/

IdResStateMonitor::IdResStateMonitor():
	mIdResDetectTmoDelay((3 * (47+47)) + 25) // 3x the cycle for the slowest debouncer + some margin (arbitrary)
{
	mIdStateInfo = OTG_ID_SM_INFO__UNKNOWN;
	mpOtgIdResStateMonitorCb = NULL;
	mIdResSmInitialized = FALSE;
	// timer callback for detecting ID resistive state
	mIdResDetectTimerRunning = FALSE;
}



IdResStateMonitor::~IdResStateMonitor()
{
}



IdResStateMonitor* IdResStateMonitor::GetInstance()
{
	if(NULL == mpIdResStateMonitor_Instance )
	{
		mpIdResStateMonitor_Instance = new IdResStateMonitor;
	}
	return mpIdResStateMonitor_Instance;
}



void IdResStateMonitor::DestroyInstance()
{
   delete mpIdResStateMonitor_Instance;
   mpIdResStateMonitor_Instance = NULL;
}



void IdResStateMonitor::IdResStateMonitorInit(OtgIdSmListenerType listener, void* pListenerObj)
{
   if (!mIdResSmInitialized)
   {
	  mpOtgIdResStateMonitorCb = listener;
	  mpOtgIdResListenerObj = pListenerObj;
	  mIdResSmInitialized = TRUE;

   }
}


void IdResStateMonitor::IdResStateMonitorReset(void)
{
   if (mIdResSmInitialized)
   {
      mIdResSmInitialized = FALSE;
   }
}



void IdResStateMonitor::IdResDetectTimerCb(int32 time_ms, timer_cb_data_type token)
{
   IdResStateMonitor* pIdResStateMonitor = static_cast< IdResStateMonitor* >((void*)token);
   ASSERT( pIdResStateMonitor != NULL );

   IrqDebouncerStateManager* pIdFltIrqDbStateMgr = pIdResStateMonitor->mpIdFltIrqDbStateMgr;
   IrqDebouncerStateManager* pIdGndIrqDbStateMgr = pIdResStateMonitor->mpIdGndIrqDbStateMgr;

   if(time_ms == pIdResStateMonitor->mIdResDetectTmoDelay)
   {
      //the delay value matches the actual setup value
   }

    // Tell timer that we are no longer interested in the current
    // timeout (because current timeout of interest has already happened!)
    pIdResStateMonitor->IdStopResDetectTimer();

    // Notify the listener of this state machine about the ID resistive state;
    //  race-conditions between ID_GND and ID_FLOATING comparators triggering
    //  complementary RE-FE might have made this timeout session a spurious
    //  one. This timeout is valid only if both ID_GND and ID_FLOATING are
    //  both LOW.
    if ( (IRQ_DEBOUNCER_STATUS__LOW  == pIdFltIrqDbStateMgr->GetStatus())
    	&& (IRQ_DEBOUNCER_STATUS__LOW == pIdGndIrqDbStateMgr->GetStatus()) )
    {
        pIdResStateMonitor->mIdStateInfo = OTG_ID_SM_INFO__RES;
        pIdResStateMonitor->mpOtgIdResStateMonitorCb(pIdResStateMonitor->mIdStateInfo, pIdResStateMonitor->mpOtgIdResListenerObj);
    }
}



void IdResStateMonitor::IdStartResDetectTimer(void)
{
   // If the timer was already running, restart it by stopping it first and then starting it ...
   IdStopResDetectTimer();

   #ifndef T_WINNT
      // START the timer
       timer_reg(&mIdSmType,				/*!< Timer to set */
       		  (timer_t2_cb_type)IdResDetectTimerCb,		/*!< Function to call at timer expiry */
                 (timer_cb_data_type)this,	/*!< Arbitrary data for func(time_ms, data). */
   	   		  mIdResDetectTmoDelay,		/*!< Time (in ms) until first timer expiry */
   	   		  0);						/*!< Period (in ms) between repeated expiries (0 = not periodic) */
   #endif
    
   mIdResDetectTimerRunning = TRUE;
}



void IdResStateMonitor::IdStopResDetectTimer(void)
{
   if (mIdResDetectTimerRunning)
   {
       // STOP the timer
       #ifndef T_WINNT
	     timer_clr( &mIdSmType, T_NONE );
       #endif
       
		mIdResDetectTimerRunning = FALSE;
   }
}


#endif //FEATURE_PMIC_USB_OTG
