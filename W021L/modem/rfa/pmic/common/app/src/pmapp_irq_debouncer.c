/*===========================================================================

            P M   I R Q   D E B O U N C E R   A P P L I C A T I O N
                           S O U R C E   F I L E

DESCRIPTION
  This file contains the implementation of the interrupt line debouncer for
  the Qualcomm Power Management Chip Set.

Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_irq_debouncer.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/06   Vish    Fixed LINT warnings.
03/17/06   Vish    1. Made changes to clear/mask interrupt in IRQ handler -
                      pm_app_irq_debouncer_process_delegate_fn2(), instead of in
                      the state machine. This will guarantee interrupt clearing
                      irrespective of the nested level of 
                      pm_app_irq_debouncer_sm_process_cached_evt().
                   2. Moved reading of (volatile) cached_evt_handler_stack_depth
                      to inside of an INTLOCK'd section to ensure that exactly
                      only one instance of pm_app_irq_debouncer_sm_process_cached_evt()
                      will actively process cached events.
                   3. Replaced all 'LOCAL' macro expansion with 'static' keyword.
03/03/06   Vish    Made "both_irq_off" and "comp_irq_on" timers configurable
                   instead of hard-coding for 1ms duration.
01/03/06   Vish    1. Added INTLOCK() in pm_app_irq_debouncer_start_timer()
                      around the code where timer is started and current
                      timer session ID is saved.
                   2. Reset current debouncer status during reset processing
                      in NULL state.
12/12/05   Vish    In pm_app_irq_debouncer_sm_process_cached_evt(), added a
                   check for underflow of cached_evt_handler_stack_depth.
09/03/05   Vish    1. Rearchitected and added INTLOCK/INTFREEs around code at
                      vulnerable places where unexpected pre-emptions could
                      whack things out of sync.
                   2. Introduced the concept of NULL state (state machine
                      stays in NULL before init and during reset) 
08/31/05   Vish    Made a fundamental change to the way IRQ debouncer state machine
                   processes events - instead of processing events immediately
                   in a synchronous manner, it caches any events that occur
                   while processing of another event is already in progress.
03/17/05   Vish    Changed the way timer is used by employing target
                   independent timer type as defined in "pmapp_timer.h".
11/20/04   Vish    Added comment header for pm_app_irq_debouncer_reset().
09/26/04   Vish    Removed NULLifying the current state of the debouncer in
                   pm_app_irq_debouncer_reset() so as to properly handle
                   event processing in
                   pm_app_irq_debouncer_sm_process_cached_evt() for the case
                   when the debouncer has currently been reset.
08/04/04   Vish    Added pm_app_irq_debouncer_get_status() for getting the
                   current state of the debounced IRQ.
11/26/03   Vish    Added documentation.
10/25/03   Vish    Created.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pmapp_irq_debouncer.h"


/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/
// type for state pointer
typedef struct pm_app_irq_debouncer_state_type *pm_app_irq_debouncer_state_ptr_type;

// state entry initialization and event handlers type
typedef void (*pm_app_irq_debouncer_state_entry_fn_type) (
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
typedef pm_app_irq_debouncer_state_ptr_type (*pm_app_irq_debouncer_cached_evt_hdlr_type) (
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);

// state type
//lint -esym(754, pm_app_irq_debouncer_state_type::name) suppress "local structure member not referenced"
// Reason: Retaining for better readability and easier debugging; leaving this warning disabled at file scope
struct pm_app_irq_debouncer_state_type
{
   const char                                   *name;  // just for making debugging easier
   pm_app_irq_debouncer_state_entry_fn_type      enter;
   pm_app_irq_debouncer_cached_evt_hdlr_type     process_cached_evt;
};


/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/
// fn for asking the debouncer state machine to process cached events
static void pm_app_irq_debouncer_sm_process_cached_evt(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr);

// IRQ management service for the debouncer state machine
static void pm_app_irq_debouncer_irq_service__reset(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
static void pm_app_irq_debouncer_irq_service__enable(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr,
   pm_irq_hdl_type irq);
static void pm_app_irq_debouncer_irq_service__disable(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);

// timer management functionality for the debouncer state machine
static void pm_app_irq_debouncer_start_timer(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr,
   int32 tmo_delay);
static void pm_app_irq_debouncer_stop_timer(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);

// prototypes of various state event handlers
//   NULL
static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__null_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
//   both_irq_off
static void pm_app_irq_debouncer_state__both_irq_off_entry(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__both_irq_off_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
//   comp_irq_on
static void pm_app_irq_debouncer_state__comp_irq_on_entry(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__comp_irq_on_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
//   stabilized
static void pm_app_irq_debouncer_state__stabilized_entry(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);
static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__stabilized_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr);


/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/
static const uint32 PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN = 0xCafeBabe;

// State definitions follow ...
//   NULL
static struct pm_app_irq_debouncer_state_type pm_app_irq_debouncer_state__null =
{
   "NULL",
   NULL,
   pm_app_irq_debouncer_state__null_cached_evt_hdlr
};
//   both_irq_off
static struct pm_app_irq_debouncer_state_type pm_app_irq_debouncer_state__both_irq_off =
{
   "BOTH_IRQ_OFF",
   pm_app_irq_debouncer_state__both_irq_off_entry,
   pm_app_irq_debouncer_state__both_irq_off_cached_evt_hdlr
};
//   comp_irq_on
static struct pm_app_irq_debouncer_state_type pm_app_irq_debouncer_state__comp_irq_on =
{
   "COMP_IRQ_ON",
   pm_app_irq_debouncer_state__comp_irq_on_entry,
   pm_app_irq_debouncer_state__comp_irq_on_cached_evt_hdlr
};
//   stabilized
static struct pm_app_irq_debouncer_state_type pm_app_irq_debouncer_state__stabilized =
{
   "STABILIZED",
   pm_app_irq_debouncer_state__stabilized_entry,
   pm_app_irq_debouncer_state__stabilized_cached_evt_hdlr
};


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION  pm_app_irq_debouncer_init                        EXTERNAL FUNCTION

DESCRIPTION
   This function initializes and sets up the specified IRQ debouncer for
   detection and notification of true state of any IRQ line.

PARAMETERS
   1) Name: debouncer_ptr
      - Pointer to the debouncer object. User needs to create a debouncer for
        each IRQ line and pass it to this fn for initialization.
      Type: pm_app_irq_debouncer_instance_type*

   2) Name: re_irq
      - Raising-edge IRQ, if it is an EDGE triggered interrupt, e.g. PM_USB_BUS_VALID_RE_IRQ_HDL
      - For LEVEL triggered interrupt, use the one that indicates the
        positive/high/valid state of the interrupt, e.g. PM_VALID_CHG_IRQ_HDL
      Type: pm_irq_hdl_type

   3) Name: fe_irq
      - Falling-edge IRQ, if it is an EDGE triggered interrupt, e.g. PM_USB_BUS_VALID_FE_IRQ_HDL
      - For LEVEL triggered interrupt, use the one that indicates the
        negative/low/invalid state of the interrupt, e.g. PM_INVALID_CHG_IRQ_HDL
      Type: pm_irq_hdl_type

   4) Name: rt_irq
      - Real-time state of the IRQ line for EDGE triggered interrupts, e.g. PM_USB_BUS_VALID_RT_ST
      - For LEVEL triggered interrupt, use the one that corresponds to the
        positive/high/valid state of the interrupt, e.g. PM_VALID_CHG_RT_ST
      Type: pm_rt_status_type

   5) Name: process_irq_cb
      - User fn that is to be called back whenever the debouncer detects a new "debounced"
        interrupt on the specified IRQ line.
      Type: pm_app_irq_debouncer_process_irq_cb

   6) Name: delegate_fn1
      - A function that has to be defined by the user for each debouncer object.
        Please refer to the type definition above for details on how to code this fn.
      Type: pm_app_irq_debouncer_delegate_fn1_type

   7) Name: delegate_fn2
      - A function that has to be defined by the user for each debouncer object.
        Please refer to the type definition above for details on how to code this fn.
      Type: pm_app_irq_debouncer_delegate_fn2_type

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked during the execution of this function.
===========================================================================*/
pm_err_flag_type pm_app_irq_debouncer_init(
   pm_app_irq_debouncer_instance_type      *debouncer_ptr,
   pm_irq_hdl_type                          re_irq,
   pm_irq_hdl_type                          fe_irq,
   pm_rt_status_type                        rt_irq,
   int32                                    tmr__both_irq_off,
   int32                                    tmr__comp_irq_on,
   pm_app_irq_debouncer_process_irq_cb      process_irq_cb,
   pm_app_irq_debouncer_delegate_fn1_type   delegate_fn1,
   pm_app_irq_debouncer_delegate_fn2_type   delegate_fn2)
{
   pm_err_flag_type  err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;  // best error of all 
                                                                 //  that makes sense
                                                                 //  in our case
   if ( debouncer_ptr )
   {
      INTLOCK();
      // static one time init processing ...
      if ( PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN != debouncer_ptr->debouncer_init_magic_pattern__static )
      {
         debouncer_ptr->current_state_ptr = &pm_app_irq_debouncer_state__null;
         debouncer_ptr->cached_evt_handler_stack_depth = 0;
         debouncer_ptr->reset_request_pending = FALSE;
         debouncer_ptr->debouncer_init_magic_pattern__static = PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN;
      }

      // per-init processing follows ...
      if ( PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN != debouncer_ptr->debouncer_init_magic_pattern__dynamic )
      {
         debouncer_ptr->delegate_fn1 = delegate_fn1;
         debouncer_ptr->delegate_fn2 = delegate_fn2;
         debouncer_ptr->re_irq = re_irq;
         debouncer_ptr->fe_irq = fe_irq;
         debouncer_ptr->rt_irq = rt_irq;
         debouncer_ptr->tmr__both_irq_off = tmr__both_irq_off;
         debouncer_ptr->tmr__comp_irq_on = tmr__comp_irq_on;
         debouncer_ptr->process_irq_cb = process_irq_cb;
         debouncer_ptr->status = PM_APP_IRQ_DEBOUNCER_STATUS__UNKNOWN;
         pm_app_timer_def(
            &debouncer_ptr->timer);
         pm_app_timer_invalidate_sess_id(
            &debouncer_ptr->curr_timer_sess_id);
         debouncer_ptr->tmo_occured = FALSE;
         debouncer_ptr->irq_occured = FALSE;
         pm_app_irq_debouncer_irq_service__reset(
            debouncer_ptr);
         debouncer_ptr->debouncer_init_magic_pattern__dynamic = PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN;
      }
      INTFREE();

      pm_app_irq_debouncer_sm_process_cached_evt(
         debouncer_ptr);

      err_flag = PM_ERR_FLAG__SUCCESS;
   }

   return err_flag;
}


/*===========================================================================
FUNCTION  pm_app_irq_debouncer_get_status                  EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current status of the IRQ line serviced by the
   indicated IRQ debouncer.

PARAMETERS
   1) Name: debouncer_ptr
      - Pointer to the debouncer object.
      Type: pm_app_irq_debouncer_instance_type*

RETURN VALUE
   Type: pm_app_irq_debouncer_status_type
   - Possible values:
     - PM_APP_IRQ_DEBOUNCER_STATUS__HIGH     -->  IRQ happened (debounced status)
     - PM_APP_IRQ_DEBOUNCER_STATUS__LOW      -->  IRQ gone     (debounced status)
     - PM_APP_IRQ_DEBOUNCER_STATUS__UNKNOWN  -->  IRQ debounce in progress
                                                     or NULL debouncer_ptr

DEPENDENCIES
   The following function(s) must have been called:
   1) pm_init()
   2) pm_app_irq_debouncer_init()

SIDE EFFECTS
   None.
===========================================================================*/
pm_app_irq_debouncer_status_type pm_app_irq_debouncer_get_status(
   const pm_app_irq_debouncer_instance_type      *debouncer_ptr)
{
   pm_app_irq_debouncer_status_type status = PM_APP_IRQ_DEBOUNCER_STATUS__UNKNOWN;

   if (debouncer_ptr)
   {
      INTLOCK();
      status = debouncer_ptr->status;
      INTFREE();
   }

   return status;
}


/*===========================================================================
FUNCTION  pm_app_irq_debouncer_reset                       EXTERNAL FUNCTION

DESCRIPTION
   This function stops the debouncer state machine in its tracks and prevents
   any future notifications of "debounced" IRQ state.
   To restart the debouncer, call pm_app_irq_debouncer_init() again. 

PARAMETERS
   1) Name: debouncer_ptr
      - Pointer to the debouncer that needs to be reset.
      Type: pm_app_irq_debouncer_instance_type*
           
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded

DEPENDENCIES
   The following function(s) must have been called:
   1) pm_init()

SIDE EFFECTS
   Interrupts are locked during the execution of this function.
===========================================================================*/
pm_err_flag_type pm_app_irq_debouncer_reset(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr)
{
   if ( debouncer_ptr )
   {
      // disable IRQ service so that no further interrupts are posted to this state machine
      pm_app_irq_debouncer_irq_service__disable(
         debouncer_ptr);

      INTLOCK();
      if ( PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN == debouncer_ptr->debouncer_init_magic_pattern__dynamic )
      {
         // mark the fact that a reset request is pending to be processed
         debouncer_ptr->reset_request_pending = TRUE;

         debouncer_ptr->debouncer_init_magic_pattern__dynamic = 0;
      }
      INTFREE();

      pm_app_irq_debouncer_sm_process_cached_evt(
         debouncer_ptr);
   }

   return PM_ERR_FLAG__SUCCESS;
}


// this is the equivalent timeout fn called by the timer via our user's delegate fn!
void pm_app_irq_debouncer_process_delegate_fn1(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr,
   pm_app_irq_debouncer_delegate_fn1_type  delegate_fn,
   pm_app_timer_type                      *param1_ptr,
   pm_app_timer_sess_id_type               param2,
   int32                                   param3)
{
   //lint --e{715} suppress "delegate_fn/param1_ptr/param3 not referenced"
   //lint --e{818} suppress "param1_ptr could be declared as pointing to const"
   
   if ( debouncer_ptr )
   {
      INTLOCK();
      if ( PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN == debouncer_ptr->debouncer_init_magic_pattern__dynamic )
      {
         if (debouncer_ptr->curr_timer_sess_id == param2)
         {
            // need to do a check to be sure that a race-condition has not been caused by any pre-emptions
            //  (i.e. this callback might belong to a timer session that we had already
            //        stopped in the past!!!)
            debouncer_ptr->tmo_occured = TRUE;               // remember that timeout has happened
         }
      }
      INTFREE();

      // ask the debouncer state machine to process the new timeout event
      pm_app_irq_debouncer_sm_process_cached_evt(
         debouncer_ptr);
   }
}


// this is the equivalent IRQ called via our one of our user's two delegate fns!
void pm_app_irq_debouncer_process_delegate_fn2(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr,
   pm_app_irq_debouncer_delegate_fn2_type  delegate_fn)
{
   //lint --e{715} suppress "delegate_fn not referenced"
   if ( debouncer_ptr )
   {
      // clear this IRQ first
      pm_app_irq_debouncer_irq_service__disable(
         debouncer_ptr);

      INTLOCK();
      if ( PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN == debouncer_ptr->debouncer_init_magic_pattern__dynamic )
      {
         debouncer_ptr->irq_occured = TRUE;
      }
      INTFREE();

      // ask the debouncer state machine to process the new interrupt event
      pm_app_irq_debouncer_sm_process_cached_evt(
         debouncer_ptr);
   }
}


/*===========================================================================
FUNCTION  pm_app_irq_debouncer_sm_process_cached_evt          LOCAL FUNCTION

DESCRIPTION
   This function services any interrupts / external inputs to the IRQ debouncer
   state machine and causes state transition(s) as required by the individual
   states. The state-entry handler is called when the state machine enters a
   new state after which its cached-event handler is called to process events
   accumulated till that point. This process is continued as long as state
   transitions are caused by the cached-event handling in the individual
   states.

PARAMETERS
   ???

RETURN VALUE
   None.

DEPENDENCIES
   The following function must have been called:
   1) pm_app_irq_debouncer_init()

SIDE EFFECTS
   Interrupts are disabled during this function.
===========================================================================*/
static void pm_app_irq_debouncer_sm_process_cached_evt(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr)
{
   pm_app_irq_debouncer_state_ptr_type           next_state_ptr = NULL;
   pm_app_irq_debouncer_state_entry_fn_type      entry_fn = NULL;
   pm_app_irq_debouncer_cached_evt_hdlr_type     cached_evt_hdlr = NULL;
   boolean                                       allowed_to_process_cached_events = FALSE;
   boolean                                       cached_events_pending = FALSE;

   if ( debouncer_ptr
        && debouncer_ptr->current_state_ptr )
   {
      INTLOCK();
      debouncer_ptr->cached_evt_handler_stack_depth ++;
      allowed_to_process_cached_events = (1 == debouncer_ptr->cached_evt_handler_stack_depth);
      INTFREE();

      if ( allowed_to_process_cached_events )
      {
         do
         {
            cached_evt_hdlr = debouncer_ptr->current_state_ptr->process_cached_evt;
            if (cached_evt_hdlr)
            {
               next_state_ptr = cached_evt_hdlr(debouncer_ptr);
            }

            // cached event could have resulted in a state transition as well as
            // other new cached events
            while (next_state_ptr != debouncer_ptr->current_state_ptr)
            {
               // Continue to process_cached_evt() so long as state transitions occur
               debouncer_ptr->current_state_ptr = next_state_ptr;

               entry_fn = debouncer_ptr->current_state_ptr->enter;
               if (entry_fn)
               {
                  entry_fn(debouncer_ptr);
               }

               cached_evt_hdlr = debouncer_ptr->current_state_ptr->process_cached_evt;
               if (cached_evt_hdlr)
               {
                  next_state_ptr = cached_evt_hdlr(debouncer_ptr);
               }
            }
            INTLOCK();
            if (debouncer_ptr->cached_evt_handler_stack_depth > 0)
            {
               debouncer_ptr->cached_evt_handler_stack_depth --;
            }
            cached_events_pending = (debouncer_ptr->cached_evt_handler_stack_depth > 0);
            INTFREE();
         } while ( cached_events_pending );
      } //if ( allowed_to_process_cached_events ...
   } //if ( debouncer_ptr && debouncer_ptr->current_state_ptr ...
}


//   NULL state
static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__null_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   pm_app_irq_debouncer_state_ptr_type next_state_ptr = &pm_app_irq_debouncer_state__null;

   // first perform any pending reset op
   if ( debouncer_ptr->reset_request_pending )
   {
      // disable any/all enabled interrupts
      pm_app_irq_debouncer_irq_service__disable(
         debouncer_ptr);
      // stop the timer (if it is running)
      pm_app_irq_debouncer_stop_timer(debouncer_ptr);
      // reset current debouncer status
      debouncer_ptr->status = PM_APP_IRQ_DEBOUNCER_STATUS__UNKNOWN;

      debouncer_ptr->reset_request_pending = FALSE;
   }

   // and then init (i.e. take state machine out of reset) if required
   if ( PM_APP_IRQ_DEBOUNCER_MAGIC_PATTERN == debouncer_ptr->debouncer_init_magic_pattern__dynamic )
   {
      next_state_ptr = &pm_app_irq_debouncer_state__both_irq_off;
   }

   return next_state_ptr;
}


//   both_irq_off
static void pm_app_irq_debouncer_state__both_irq_off_entry(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   // turn all (both RE and FE) IRQs off and start a timer to filter out the line bouncing;
   pm_app_irq_debouncer_irq_service__disable(
      debouncer_ptr);

   debouncer_ptr->status = PM_APP_IRQ_DEBOUNCER_STATUS__UNKNOWN;

   pm_app_irq_debouncer_start_timer(
      debouncer_ptr,
      debouncer_ptr->tmr__both_irq_off);
}

static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__both_irq_off_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   pm_app_irq_debouncer_state_ptr_type next_state_ptr = &pm_app_irq_debouncer_state__both_irq_off;

   if (debouncer_ptr->reset_request_pending)
   {
      next_state_ptr = &pm_app_irq_debouncer_state__null;
   }
   else if (debouncer_ptr->tmo_occured)
   {
      debouncer_ptr->tmo_occured = FALSE;
      next_state_ptr = &pm_app_irq_debouncer_state__comp_irq_on;
   }

   return next_state_ptr;
}


//   comp_irq_on
static void pm_app_irq_debouncer_state__comp_irq_on_entry(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   boolean  rt_status = FALSE;

   if (PM_ERR_FLAG__SUCCESS == pm_get_rt_status(debouncer_ptr->rt_irq, &rt_status))
   {
      debouncer_ptr->status = rt_status
                                 ? PM_APP_IRQ_DEBOUNCER_STATUS__HIGH
                                 : PM_APP_IRQ_DEBOUNCER_STATUS__LOW;
      if (PM_APP_IRQ_DEBOUNCER_STATUS__HIGH == debouncer_ptr->status)
      {
         // IRQ is in a HIGH state, so look for FE transition
         // clear the IRQ just before unmasking it
         pm_app_irq_debouncer_irq_service__enable(
            debouncer_ptr,
            debouncer_ptr->fe_irq);
      }
      else
      {
         // IRQ is in a LOW state, so look for RE transition
         // clear the IRQ just before unmasking it
         pm_app_irq_debouncer_irq_service__enable(
            debouncer_ptr,
            debouncer_ptr->re_irq);
      }
   }

   pm_app_irq_debouncer_start_timer(
      debouncer_ptr,
      debouncer_ptr->tmr__comp_irq_on);

}

static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__comp_irq_on_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   pm_app_irq_debouncer_state_ptr_type next_state_ptr = &pm_app_irq_debouncer_state__comp_irq_on;

   if (debouncer_ptr->reset_request_pending)
   {
      next_state_ptr = &pm_app_irq_debouncer_state__null;
   }
   else if (debouncer_ptr->irq_occured)
   {
      debouncer_ptr->irq_occured = FALSE;
      pm_app_irq_debouncer_stop_timer(debouncer_ptr);
      next_state_ptr = &pm_app_irq_debouncer_state__both_irq_off;
   }
   else if (debouncer_ptr->tmo_occured)
   {
      debouncer_ptr->tmo_occured = FALSE;
      next_state_ptr = &pm_app_irq_debouncer_state__stabilized;
   }

   return next_state_ptr;
}

//   stabilized
static void pm_app_irq_debouncer_state__stabilized_entry(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   //lint --e{818} suppress "debouncer_ptr could be declared as pointing to const"
   // Reason: cannot change prototype of state entry func just for this state

   if (debouncer_ptr->process_irq_cb)
   {
      debouncer_ptr->process_irq_cb(debouncer_ptr->status);
   }
}

static pm_app_irq_debouncer_state_ptr_type pm_app_irq_debouncer_state__stabilized_cached_evt_hdlr(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   pm_app_irq_debouncer_state_ptr_type next_state_ptr = &pm_app_irq_debouncer_state__stabilized;

   if (debouncer_ptr->reset_request_pending)
   {
      next_state_ptr = &pm_app_irq_debouncer_state__null;
   }
   else if (debouncer_ptr->irq_occured)
   {
      debouncer_ptr->irq_occured = FALSE;
      next_state_ptr = &pm_app_irq_debouncer_state__both_irq_off;
   }

   return next_state_ptr;
}


// IRQ management service
static void pm_app_irq_debouncer_irq_service__reset(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   if (NULL != debouncer_ptr)
   {
      debouncer_ptr->irq_service__initialized = FALSE;
   }
}

static void pm_app_irq_debouncer_irq_service__enable(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr,
   pm_irq_hdl_type irq)
{
   if (NULL != debouncer_ptr)
   {
      // disable any previously enabled IRQ
      pm_app_irq_debouncer_irq_service__disable(
         debouncer_ptr);

      // enable chosen IRQ
      debouncer_ptr->irq_service__en_irq = irq;
      (void)pm_clear_irq(irq);
      (void)pm_set_irq_handle(irq, debouncer_ptr->delegate_fn2);
   }
}

static void pm_app_irq_debouncer_irq_service__disable(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   if (NULL != debouncer_ptr)
   {
      if ( !debouncer_ptr->irq_service__initialized )
      {
         // clear/mask both RE and FE IRQs
         (void)pm_set_irq_handle(debouncer_ptr->re_irq, NULL);
         (void)pm_set_irq_handle(debouncer_ptr->fe_irq, NULL);
         (void)pm_clear_irq(debouncer_ptr->re_irq);
         (void)pm_clear_irq(debouncer_ptr->fe_irq);

         debouncer_ptr->irq_service__en_irq = PM_NUM_OF_IRQ_HDL;
         debouncer_ptr->irq_service__initialized = TRUE;
      }
      else if (debouncer_ptr->irq_service__en_irq < PM_NUM_OF_IRQ_HDL)
      {
         // initialized and a valid IRQ has been enabled before
         (void)pm_set_irq_handle(debouncer_ptr->irq_service__en_irq, NULL);
         (void)pm_clear_irq(debouncer_ptr->irq_service__en_irq);
         debouncer_ptr->irq_service__en_irq = PM_NUM_OF_IRQ_HDL;
      }
      // else ==> initialized but IRQ has not been enabled currently;
      // nothing to do in this case
   }
}


// starting and stopping timer
static void pm_app_irq_debouncer_start_timer(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr,
   int32 tmo_delay)
{
   debouncer_ptr->tmo_occured = FALSE;    // init to indicate that timeout hasn't happened yet

   INTLOCK();
   // (re)start the timer
   debouncer_ptr->curr_timer_sess_id
      = pm_app_timer_start(
           &debouncer_ptr->timer,                                    // pointer to the timer
           (pm_app_timer_cb_func_type)debouncer_ptr->delegate_fn1,   // addr of the callback func
           tmo_delay);                                               // delay in milli-sec
   INTFREE();
}

static void pm_app_irq_debouncer_stop_timer(
   pm_app_irq_debouncer_instance_type  *debouncer_ptr)
{
   pm_app_timer_invalidate_sess_id(
      &debouncer_ptr->curr_timer_sess_id);

   // stop the timer
   pm_app_timer_stop(&(debouncer_ptr->timer));
}

