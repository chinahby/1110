/*! \file 
 *  \n
 *  \brief  pmapp_IrqDebouncer.cpp ---- Debouncer related class implementation
 *  \details This file contains the implementation of the interrupt line debouncer classes for
 *  the Qualcomm Power Management Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_IrqDebouncer.cpp#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/08   aab		Added timer_group_type defination
08/08/08   aab      Made changes to properly initialize timer object
07/23/08   aab      Made changes to resolve compiler warnings
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */
#include "pmapp_IrqDebouncer.h"

/* ==========================================================================
                DEFINITIONS AND DECLARATIONS FOR MODULE
This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static timer_group_type  pm_app_timer_group;
static const uint32 IRQ_DEBOUNCER_MAGIC_PATTERN = 0xCafeBabe;

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
     class IrqDebouncerInstance											  
**************************************************************************/

IrqDebouncerInstance::IrqDebouncerInstance()
{
   debouncerInit_MagicPattern_Static = NULL;
   debouncerInit_MagicPattern_Dynamic = NULL;
}


IrqDebouncerInstance::~IrqDebouncerInstance()
{
}




/**************************************************************************
      class IrqDebouncerStateManager											  
**************************************************************************/
IrqDebouncerStateManager::IrqDebouncerStateManager()
{
}


IrqDebouncerStateManager::~IrqDebouncerStateManager()
{
   delete mpIrqDebouncerInstance;
   delete mpDbNullState;
   delete mpDbBothIrqOffState;
   delete mpDbComplIrqOnState;
   delete mpDbStabilizedState;
}



void IrqDebouncerStateManager::PreInit()
{
   mpIrqDebouncerInstance = new IrqDebouncerInstance;
   //Each debouncer state need to have ptr to IrqDebouncerStateManager up on creation
	mpDbNullState = new DebouncerNullState(this);
	mpDbBothIrqOffState = new DebouncerBothIrqOffState(this);
	mpDbComplIrqOnState = new DebouncerComplementaryIrqOnState(this);
	mpDbStabilizedState = new DebouncerStabilizedState(this);
}





pm_err_flag_type IrqDebouncerStateManager::Init(
						void*									 irqStateMonObj,
						pm_irq_hdl_type                          re_irq,
						pm_irq_hdl_type                          fe_irq,
						pm_rt_status_type                        rt_irq,
						int32                                    tmr__both_irq_off,
						int32                                    tmr__comp_irq_on,
						IrqDebouncerProcessIrqCb			     process_irq_cb )
{
   pm_err_flag_type  err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

	mpCurrState = mpDbNullState;
	mpPrevState = mpCurrState;

   if ( mpIrqDebouncerInstance )
   {
      INTLOCK();
      // static one time init processing ...
      if ( IRQ_DEBOUNCER_MAGIC_PATTERN != mpIrqDebouncerInstance->debouncerInit_MagicPattern_Static )
      {
         mpIrqDebouncerInstance->current_state_ptr = mpDbNullState; 

         mpIrqDebouncerInstance->cached_evt_handler_stack_depth = 0;
         mpIrqDebouncerInstance->reset_request_pending = FALSE;
         mpIrqDebouncerInstance->debouncerInit_MagicPattern_Static = IRQ_DEBOUNCER_MAGIC_PATTERN;
      }

      // per-init processing follows ...
      if ( IRQ_DEBOUNCER_MAGIC_PATTERN != mpIrqDebouncerInstance->debouncerInit_MagicPattern_Dynamic )
      {
		 mpIrqDebouncerInstance->irqStateMonitorObj = irqStateMonObj;
         mpIrqDebouncerInstance->re_irq = re_irq;
         mpIrqDebouncerInstance->fe_irq = fe_irq;
         mpIrqDebouncerInstance->rt_irq = rt_irq;
         mpIrqDebouncerInstance->tmr__both_irq_off = tmr__both_irq_off;
         mpIrqDebouncerInstance->tmr__comp_irq_on = tmr__comp_irq_on;
         mpIrqDebouncerInstance->process_irq_cb = process_irq_cb;
         mpIrqDebouncerInstance->status = IRQ_DEBOUNCER_STATUS__UNKNOWN;

		 mpIrqDebouncerInstance->tmo_occured = FALSE;
         mpIrqDebouncerInstance->irq_occured = FALSE;

         mpIrqDebouncerInstance->irq_service__en_irq = PM_NUM_OF_IRQ_HDL;
         IrqServiceReset();
         mpIrqDebouncerInstance->debouncerInit_MagicPattern_Dynamic = IRQ_DEBOUNCER_MAGIC_PATTERN;

         #ifndef T_WINNT
            timer_def2(&(mpIrqDebouncerInstance->mDbTimer), &pm_app_timer_group );
         #endif

      }
      INTFREE();
      
	  ProcessCachedEvt();
      err_flag = PM_ERR_FLAG__SUCCESS;

   }

   return err_flag;
}




IrqDebouncerStatusType IrqDebouncerStateManager::GetStatus()
{
   IrqDebouncerStatusType status = IRQ_DEBOUNCER_STATUS__UNKNOWN;

   if (mpIrqDebouncerInstance)
   {
      INTLOCK();
      status = mpIrqDebouncerInstance->status;
      INTFREE();
   }

   return status;
}



pm_err_flag_type IrqDebouncerStateManager::Reset()
{
   if ( mpIrqDebouncerInstance )
   {
      // disable IRQ service so that no further interrupts are posted to this state machine
	  IrqServiceDisable();

      INTLOCK();
      if ( IRQ_DEBOUNCER_MAGIC_PATTERN == mpIrqDebouncerInstance->debouncerInit_MagicPattern_Dynamic )
      {
         // mark the fact that a reset request is pending to be processed
         mpIrqDebouncerInstance->reset_request_pending = TRUE;

         mpIrqDebouncerInstance->debouncerInit_MagicPattern_Dynamic = 0;
      }
      INTFREE();

	  ProcessCachedEvt();
   }
   return PM_ERR_FLAG__SUCCESS;
}




void IrqDebouncerStateManager::TimerServiceCb(int32 time_ms, timer_cb_data_type token)
{
   IrqDebouncerStateManager* pIrqDbStateMgr = static_cast< IrqDebouncerStateManager* >((void*)token);
   ASSERT( pIrqDbStateMgr != NULL );

   IrqDebouncerInstance* pIrqDbInstance = pIrqDbStateMgr->mpIrqDebouncerInstance;

   if ((time_ms == pIrqDbInstance->tmr__both_irq_off)||(time_ms == pIrqDbInstance->tmr__comp_irq_on) )
   {
      //delay value value matches the actual setup value
   }


    INTLOCK();
	if ( IRQ_DEBOUNCER_MAGIC_PATTERN == pIrqDbInstance->debouncerInit_MagicPattern_Dynamic )
	{
		if (pIrqDbInstance->curr_timer_sess_id == token)
		{
			// need to do a check to be sure that a race-condition has not been caused by any pre-emptions
			//  (i.e. this callback might belong to a timer session that we had already
			//        stopped in the past!!!)
			pIrqDbInstance->tmo_occured = TRUE;               // remember that timeout has happened
		}
	}
	INTFREE();

	// ask the debouncer state machine to process the new timeout event
	pIrqDbStateMgr->ProcessCachedEvt();
}




void IrqDebouncerStateManager::IrqServiceCb(void* data)
{
   IrqDebouncerStateManager* pIrqDbStateMgr = static_cast< IrqDebouncerStateManager* >((void*)data);
   ASSERT( pIrqDbStateMgr != NULL );
	
   IrqDebouncerInstance* pIrqDbInstance = pIrqDbStateMgr->mpIrqDebouncerInstance;

	pIrqDbStateMgr->IrqServiceDisable();

    INTLOCK();
	if ( IRQ_DEBOUNCER_MAGIC_PATTERN == pIrqDbInstance->debouncerInit_MagicPattern_Dynamic )
    {
        pIrqDbInstance->irq_occured = TRUE;
    }
    INTFREE();

    // ask the debouncer state machine to process the new interrupt event
	pIrqDbStateMgr->ProcessCachedEvt();
}



void IrqDebouncerStateManager::ProcessCachedEvt()
{
   DebouncerBaseState*        next_state_ptr = NULL;
   boolean	allowed_to_process_cached_events = FALSE;
   boolean	cached_events_pending = FALSE;

   if ( mpIrqDebouncerInstance && mpIrqDebouncerInstance->current_state_ptr ) //???????
   {
      INTLOCK();
      mpIrqDebouncerInstance->cached_evt_handler_stack_depth ++;
      allowed_to_process_cached_events = (1 == mpIrqDebouncerInstance->cached_evt_handler_stack_depth);
      INTFREE();

      if ( allowed_to_process_cached_events )
      {
         do
         {
			 next_state_ptr = mpCurrState->CachedEvtHandler();

			// cached event could have resulted in a state transition as well as other new cached events
			while ( next_state_ptr != mpCurrState )
			{
				mpCurrState = next_state_ptr;  // Continue to process_cached_evt() as long as state transitions occur
				mpCurrState->Entry(); //entry function of the next state
				next_state_ptr = mpCurrState->CachedEvtHandler(); //process cached evt fn
			}

            INTLOCK();
            if (mpIrqDebouncerInstance->cached_evt_handler_stack_depth > 0)
            {
               mpIrqDebouncerInstance->cached_evt_handler_stack_depth --;
            }
            cached_events_pending = (mpIrqDebouncerInstance->cached_evt_handler_stack_depth > 0);
            INTFREE();

         } while ( cached_events_pending );
      } //if ( allowed_to_process_cached_events ...
   } //if ( mpIrqDebouncerInstance  &&   mpIrqDebouncerInstance->current_state_ptr .....
}






void IrqDebouncerStateManager::IrqServiceReset()
{
   if (NULL != mpIrqDebouncerInstance)
   {
      mpIrqDebouncerInstance->irq_service__initialized = FALSE;
   }
}




void IrqDebouncerStateManager::IrqServiceEnable(pm_irq_hdl_type irq)
{
   if (NULL != mpIrqDebouncerInstance)
   {
      // disable any previously enabled IRQ
      IrqServiceDisable();

      // enable chosen IRQ
      mpIrqDebouncerInstance->irq_service__en_irq = irq;
      (void)pm_clear_irq(irq);

	  (void)pm_set_irq_multi_handle(irq, IrqServiceCb, this);
   }
}



void IrqDebouncerStateManager::IrqServiceDisable() 
{
   if (NULL != mpIrqDebouncerInstance)
   {
      if ( !mpIrqDebouncerInstance->irq_service__initialized )
      {
         // clear/mask both RE and FE IRQs
         (void)pm_clear_irq_multi_handle(mpIrqDebouncerInstance->re_irq, IrqServiceCb);
         (void)pm_clear_irq_multi_handle(mpIrqDebouncerInstance->fe_irq, IrqServiceCb);
         (void)pm_clear_irq(mpIrqDebouncerInstance->re_irq);
         (void)pm_clear_irq(mpIrqDebouncerInstance->fe_irq);

         mpIrqDebouncerInstance->irq_service__en_irq = PM_NUM_OF_IRQ_HDL;
         mpIrqDebouncerInstance->irq_service__initialized = TRUE;
      }
      else if (mpIrqDebouncerInstance->irq_service__en_irq < PM_NUM_OF_IRQ_HDL)
      {
         // initialized and a valid IRQ has been enabled before
         (void)pm_clear_irq_multi_handle(mpIrqDebouncerInstance->irq_service__en_irq, IrqServiceCb);
         (void)pm_clear_irq(mpIrqDebouncerInstance->irq_service__en_irq);

         mpIrqDebouncerInstance->irq_service__en_irq = PM_NUM_OF_IRQ_HDL;
      }
      // else ==> initialized but IRQ has not been enabled currently;
      // nothing to do in this case
   }
}



void IrqDebouncerStateManager::StartTimer(int32 tmo_delay)
{
   mpIrqDebouncerInstance->tmo_occured = FALSE;    // init to indicate that timeout hasn't happened yet

   int repeatdelay = 0; //not repeated
   timer_cb_data_type _id = (timer_cb_data_type)this;

   INTLOCK();
    mpIrqDebouncerInstance->curr_timer_sess_id = _id;  //considered to be an ID

    #ifndef T_WINNT
    timer_reg(&(mpIrqDebouncerInstance->mDbTimer),          /* Timer to set */
    		  (timer_t2_cb_type)TimerServiceCb,       /* Function to call at timer expiry */
              _id,
	   		  tmo_delay,            /* Time (in ms) until first timer expiry */
	   		  repeatdelay);			/* Period (in ms) between repeated expiries (0 = not periodic) */
    #endif

   INTFREE();
}




void IrqDebouncerStateManager::StopTimer()
{
    #ifndef T_WINNT
	  timer_clr( &(mpIrqDebouncerInstance->mDbTimer), T_NONE ); 
    #endif
}



uint8 IrqDebouncerStateManager::GetCurrentStateID()
{
	uint8 currStateId = mpCurrState->mStateId;
	return currStateId;
}







/**************************************************************************
     class DebouncerBaseState											  
**************************************************************************/
DebouncerBaseState::DebouncerBaseState()
{
	mStateName ="NULL";     // USED for debugging
	mStateId =  DEBOUNCER_STATE__NULL;
}

DebouncerBaseState::~DebouncerBaseState()
{
}



/**************************************************************************
     class DebouncerNullState											  
**************************************************************************/
DebouncerNullState::DebouncerNullState(IrqDebouncerStateManager* pIrqDbSM)
{
	mpIrqDbStateMgr = pIrqDbSM;
	mpIrqDbInstance = pIrqDbSM->mpIrqDebouncerInstance;
	mStateName = "NULL"; 
	mStateId = DEBOUNCER_STATE__NULL;
}

DebouncerNullState::~DebouncerNullState()
{
}


void DebouncerNullState::Entry()
{
	//do nothing
}


DebouncerBaseState* DebouncerNullState::CachedEvtHandler()
{
	DebouncerBaseState* next_state_ptr = mpIrqDbStateMgr->mpDbNullState;

	// first perform any pending reset op
   if ( mpIrqDbInstance->reset_request_pending )
   {
      // disable any/all enabled interrupts
	  mpIrqDbStateMgr->IrqServiceDisable();

      // stop the timer (if it is running)
	  mpIrqDbStateMgr->StopTimer();

      // reset current debouncer status
      mpIrqDbInstance->status = IRQ_DEBOUNCER_STATUS__UNKNOWN;
      mpIrqDbInstance->reset_request_pending = FALSE;
   }

   // and then init (i.e. take state machine out of reset) if required
   if ( IRQ_DEBOUNCER_MAGIC_PATTERN == mpIrqDbInstance->debouncerInit_MagicPattern_Dynamic )
   {
	   next_state_ptr = mpIrqDbStateMgr->mpDbBothIrqOffState;
   }

   return next_state_ptr;
}




/**************************************************************************
     class BothIrqOffState											  
**************************************************************************/
DebouncerBothIrqOffState::DebouncerBothIrqOffState(IrqDebouncerStateManager* pIrqDbSM)
{
	mpIrqDbStateMgr = pIrqDbSM;
	mpIrqDbInstance = pIrqDbSM->mpIrqDebouncerInstance;
	mStateName = "BOTH_IRQ_OFF"; 
	mStateId = DEBOUNCER_STATE__BOTH_IRQ_OFF;
}


DebouncerBothIrqOffState::~DebouncerBothIrqOffState()
{
}


void DebouncerBothIrqOffState::Entry()
{
   //Turn all (both RE and FE) IRQs off and start a timer to filter out the line bouncing;
   mpIrqDbStateMgr->IrqServiceDisable();
   mpIrqDbInstance->status = IRQ_DEBOUNCER_STATUS__UNKNOWN;
   mpIrqDbStateMgr->StartTimer(mpIrqDbInstance->tmr__both_irq_off);
}



DebouncerBaseState* DebouncerBothIrqOffState::CachedEvtHandler()
{
	DebouncerBaseState* next_state_ptr = mpIrqDbStateMgr->mpDbBothIrqOffState;

   if (mpIrqDbInstance->reset_request_pending)
   {
      next_state_ptr = mpIrqDbStateMgr->mpDbNullState;
   }
   else if (mpIrqDbInstance->tmo_occured)
   {
      mpIrqDbInstance->tmo_occured = FALSE;
      next_state_ptr = mpIrqDbStateMgr->mpDbComplIrqOnState;
   }

   return next_state_ptr;
}



/**************************************************************************
     class ComplementaryIrqOnState											  
**************************************************************************/
DebouncerComplementaryIrqOnState::DebouncerComplementaryIrqOnState(IrqDebouncerStateManager* pIrqDbSM)
{
	mpIrqDbStateMgr = pIrqDbSM;
	mpIrqDbInstance = pIrqDbSM->mpIrqDebouncerInstance;
	mStateName = "COMP_IRQ_ON"; 
	mStateId = DEBOUNCER_STATE__COMP_IRQ_ON;
}


DebouncerComplementaryIrqOnState::~DebouncerComplementaryIrqOnState()
{
}


void DebouncerComplementaryIrqOnState::Entry()
{
   boolean  rt_status = FALSE;

   if (PM_ERR_FLAG__SUCCESS == pm_get_rt_status(mpIrqDbInstance->rt_irq, &rt_status))
   {
      mpIrqDbInstance->status = rt_status
                                 ? IRQ_DEBOUNCER_STATUS__HIGH
                                 : IRQ_DEBOUNCER_STATUS__LOW;
      if (IRQ_DEBOUNCER_STATUS__HIGH == mpIrqDbInstance->status)
      {
         // IRQ is in a HIGH state, so look for FE transition
         // clear the IRQ just before unmasking it
		 mpIrqDbStateMgr->IrqServiceEnable(mpIrqDbInstance->fe_irq);
      }
      else
      {
         // IRQ is in a LOW state, so look for RE transition
         // clear the IRQ just before unmasking it
		 mpIrqDbStateMgr->IrqServiceEnable(mpIrqDbInstance->re_irq);
      }
   }

   mpIrqDbStateMgr->StartTimer(mpIrqDbInstance->tmr__comp_irq_on);
}


DebouncerBaseState* DebouncerComplementaryIrqOnState::CachedEvtHandler()
{
   	DebouncerBaseState* next_state_ptr = mpIrqDbStateMgr->mpDbComplIrqOnState;

   if (mpIrqDbInstance->reset_request_pending)
   {
      next_state_ptr = mpIrqDbStateMgr->mpDbNullState;
   }
   else if (mpIrqDbInstance->irq_occured)
   {
      mpIrqDbInstance->irq_occured = FALSE;
      mpIrqDbStateMgr->StopTimer();
      next_state_ptr = mpIrqDbStateMgr->mpDbBothIrqOffState;
   }
   else if (mpIrqDbInstance->tmo_occured)
   {
      mpIrqDbInstance->tmo_occured = FALSE;
      next_state_ptr = mpIrqDbStateMgr->mpDbStabilizedState;
   }

   return next_state_ptr;
}



/**************************************************************************
     class StabilizedState											  
**************************************************************************/
DebouncerStabilizedState::DebouncerStabilizedState(IrqDebouncerStateManager* pIrqDbSM)
{
	mpIrqDbStateMgr = pIrqDbSM;
	mpIrqDbInstance = pIrqDbSM->mpIrqDebouncerInstance;
	mStateName = "STABILIZED"; 
	mStateId = DEBOUNCER_STATE__STABILIZED;
}


DebouncerStabilizedState::~DebouncerStabilizedState()
{
}


void DebouncerStabilizedState::Entry()
{
   if (mpIrqDbInstance->process_irq_cb)
   {
	   mpIrqDbInstance->process_irq_cb(mpIrqDbInstance->status, mpIrqDbInstance->irqStateMonitorObj);
   }
}


DebouncerBaseState* DebouncerStabilizedState::CachedEvtHandler()
{
   DebouncerBaseState* next_state_ptr = mpIrqDbStateMgr->mpDbStabilizedState;

   if (mpIrqDbInstance->reset_request_pending)
   {
      next_state_ptr = mpIrqDbStateMgr->mpDbNullState;
   }
   else if (mpIrqDbInstance->irq_occured)
   {
      mpIrqDbInstance->irq_occured = FALSE;
      next_state_ptr = mpIrqDbStateMgr->mpDbBothIrqOffState;
   }

   return next_state_ptr;
}

