/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       R E X   T I M E R  S E R V I C E S

GENERAL DESCRIPTION

    Timers are used under REX to perform software interval timing. When
    using a timer, a task specifies the length of the interval that is to
    be timed and either a signal mask to be set or a function to be called
    when the timer expires.  Notice that timers may be used in conjunction
    with other events that set signals, making it possible to perform timeout
    detection for those events.

    REX maintains a timer list that contains all active (i.e. unexpired)
    timers. When a timer is set, it is added to the list of active timers;
    upon expiration, it is pulled out of the list.

    Under REX, an arbitrary number of timers may be created although, as
    with tasks, performance deteriorates slightly with each active (i.e.,
    unexpired) timer. There is no overhead cost for timers that have
    expired.

=============================================================================

EXTERNALIZED FUNCTIONS

  rex_def_timer
    Initializes a specified timer block for use as an interval timer. Sets
    up which task to notify when the timer expires, and which signal mask
    to set.

  rex_clr_timer
    Sets the counter value for a specified timer to zero. This, in effect,
    stops the timer.

  rex_set_timer
    Sets the counter value for a specified timer to a specified value and
    starts the count down. The signal mask specified by the timer is
    cleared for the task associated with the timer, and later will be set
    when the timer expires.

  rex_get_timer
    Reads out the current counter value for a specified timer.

  rex_tick
    Decrements the counter values for all active timers by the specified
    argument value. As each timer expires (i.e., its count value goes to
    less than 1), the appropriate signal mask is set for the appropriate
    task. This procedure is generally called from a periodic interrupt.

  rex_timed_wait
    Sets the counter value of a specified timer to a specified value, and
    then suspends the calling task until at least one signal of a specified
    signal mask is set for the task. Typically, the timer, when it expires,
    will set one of the signals of the specified signal mask. Calling this
    function is equivalent to calling rex_set_timer followed by rex_wait.

  rex_def_timer_ex
    Extended version of timer-definition function. Allows the
    specification of a callback function and an argument for the
    callback. When the timer expires the callback is invoked in the
    context of the task that calls rex_def_timer_ex.

  rex_create_timer_ex
    Similar to rex_def_timer_ex, except that this function allocates
    the memory for the timer dynamically. A pointer to the timer is
    returned.

  rex_delete_timer_ex
    Frees the memory allocated for a timer by rex_create_timer_ex.

  rex_pause_timer
    Pauses a timer.

  rex_resume_timer
    Resumes a paused timer.

  rex_min_tmo
    Returns the value of the minimum timeout of all the timers on the timer
    list. Since the list is sorted by timeout, this is just the value of
    the timeout of the timer that follows the head node.

Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/rextime.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/05    ck     Ensured that interrupts are locked before calling
                   rex_decrement_timers in the function rex_tick().
12/12/04    is     Added REX profiling for 6250 FEATURE_REX_PROFILING_6250.
06/15/04    gr     Added support for low-resolution profiling.
04/16/04    gr     Minor modification to rex_insert_timer to make it more
                   efficient.
10/31/03   ajn     Delegate timer implementation to the sclk timers (6500+)
                   [services/time/timer.*]
09/10/02    gr     Fixed a bug in rex_clr_timer.
08/27/02    gr     Fixed a bug in rex_delete_task_timers.
07/11/02    gr     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef CUST_H
#include "customer.h"
#endif

#include "comdef.h"
#include "rex.h"

#ifdef T_WINNT
#error code not present
#else
#include "armasm.h"
#include "arm.h"
#endif

#include <string.h>
#include <limits.h>

#ifdef FEATURE_USE_TIME_VU
#include "timer.h"
#endif


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_USE_TIME_VU

timer_group_type  rex_timer_group;

#else

/*---------------------------------------------------------------------
** Defines
**--------------------------------------------------------------------*/

/*----------------------------------------------------------------------
** Null timer which is the last member on the timer_list
**--------------------------------------------------------------------*/
static rex_timer_type rex_null_timer;

/*----------------------------------------------------------------------
** The timer_list head node
**--------------------------------------------------------------------*/
static rex_timer_type rex_timer_list;

#endif /* FEATURE_USE_TIME_VU */

/*---------------------------------------------------------------------
** A rex_tick counter for debugging
**--------------------------------------------------------------------*/
#ifdef REX_DEBUG
#error code not present
#endif

#ifdef FEATURE_REX_TICK_CONTROL
#error code not present
#endif

/* Variable used in profiling.
 */
extern unsigned long rex_total_time_samples;

/*===========================================================================
FUNCTION REX_INIT_TIMER_LIST

DESCRIPTION
  Ininitializes the REX timer list by inserting the null timer after the head
  node.

===========================================================================*/
void rex_init_timer_list( void )
{
#ifndef FEATURE_USE_TIME_VU
   rex_timer_list.cnt               = 0;
   rex_null_timer.tcb_ptr           = NULL;
   rex_null_timer.sigs              = 0x0;

   rex_null_timer.cnt               = UINT_MAX;
   rex_null_timer.tcb_ptr           = NULL;
   rex_null_timer.sigs              = 0x0;

   rex_null_timer.link.next_ptr     = NULL;
   rex_null_timer.link.prev_ptr     = &rex_timer_list;
   rex_timer_list.link.next_ptr     = &rex_null_timer;
   rex_timer_list.link.prev_ptr     = NULL;

   return;
#endif
} /* END rex_init_timer_list */


#ifndef FEATURE_USE_TIME_VU
/*===========================================================================
FUNCTION REX_INSERT_TIMER

DESCRIPTION
  Inserts a timer into the REX timer list. The timer list is sorted in order
  of expiration, with the timer that will expire soonest at the head of the
  list.

  Note 1: The timer list is assumed to contain a head node with a timeout
  value of 0 and a tail node with a timeout value of UINT_MAX. Any timer will
  be inserted somewhere between these two nodes.

  Note 2: This function assumes that the timer passed in is *not* already
  in the timer list.

===========================================================================*/
void rex_insert_timer(
   rex_timer_type    *timer_p,   /* pointer to a valid timer structure */
   rex_timer_cnt_type tmo          /* Timeout for the timer              */
)
{
   rex_timer_type *next_timer_ptr = rex_timer_list.link.next_ptr;

   /* Scan the timer list, decrementing the timer's countdown until we
   ** find a timer whose countdown is no smaller than that of the timer
   ** passed in. We are guaranteed to find such a timer, since
   ** rex_null_timer -- the last timer on the list -- has a countdown
   ** of UINT_MAX.
   */
   while( tmo > next_timer_ptr->cnt )
   {
      REX_ASSERT( next_timer_ptr != NULL );
      tmo -= next_timer_ptr->cnt;
      next_timer_ptr = next_timer_ptr->link.next_ptr;
   }

   timer_p->cnt = tmo;

   timer_p->link.prev_ptr = next_timer_ptr->link.prev_ptr;
   timer_p->link.next_ptr = next_timer_ptr;
   next_timer_ptr->link.prev_ptr->link.next_ptr = timer_p;
   next_timer_ptr->link.prev_ptr = timer_p;
   if ( next_timer_ptr != &rex_null_timer)
   {
      next_timer_ptr->cnt -= timer_p->cnt;
   }

   return;
} /* END rex_insert_timer */
#endif /* FEATURE_USE_TIME_VU */

/*===========================================================================
FUNCTION REX_DEF_TIMER

DESCRIPTION
  Initializes a new timer.

===========================================================================*/
void rex_def_timer(
   rex_timer_type  *p_timer,     /* pointer to a valid timer structure */
   rex_tcb_type    *p_tcb,       /* tcb to associate with the timer    */
   rex_sigs_type    sigs         /* sigs to set upon timer expiration  */
)
{
#ifdef FEATURE_USE_TIME_VU

   timer_def(p_timer, &rex_timer_group, p_tcb, sigs, NULL, 0);

#else

#ifdef FEATURE_REX_TIMER_NAME
   static char timer_name[ ] = "REX unnamed timer";
#endif

   REX_INTERCEPT_DEF_TIMER( p_timer, p_tcb, sigs );

#ifdef FEATURE_REX_TIMER_NAME
   strcpy( p_timer->name, timer_name );
#endif

   p_timer->link.next_ptr = NULL;
   p_timer->link.prev_ptr = NULL;
   p_timer->cnt           = 0;
   p_timer->tcb_ptr       = p_tcb;
   p_timer->sigs          = sigs;
#ifdef FEATURE_REX_TIMER_EX
   p_timer->cb_ptr     = NULL;
   p_timer->cb_param   = 0;
#endif

   return;

#endif
} /* END rex_def_timer */


#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION REX_CLR_TIMER

DESCRIPTION
  Clears a timer and removes it from the REX timer list, if the timer is in
  the list. Returns the timer's real timeout (the sum of the countdowns of
  all timers up to and including the specified timer). 0 is returned if
  the timer is not in the list.

  Note: We use the next_ptr field in the link structure in the timer to
  determine if the timer is in the list.

===========================================================================*/
rex_timer_cnt_type rex_clr_timer(
  rex_timer_type *timer_p       /* timer to clear */
)
{
#ifdef FEATURE_USE_TIME_VU

   return timer_clr(timer_p, T_MSEC);

#else

   rex_timer_cnt_type  prev_value = 0;    /* previous count */
   rex_timer_type     *next_timer_ptr = &rex_timer_list;

   INTLOCK( );

   REX_ASSERT(timer_p!=NULL);

   if( timer_p->link.next_ptr != NULL )
   {
      while ( next_timer_ptr != timer_p )
      {
         next_timer_ptr = next_timer_ptr->link.next_ptr;
         REX_ASSERT( next_timer_ptr != NULL );
         prev_value += next_timer_ptr->cnt;
      }

      timer_p->link.prev_ptr->link.next_ptr = timer_p->link.next_ptr;
      timer_p->link.next_ptr->link.prev_ptr = timer_p->link.prev_ptr;
      if ( timer_p->link.next_ptr != &rex_null_timer)
      {
         timer_p->link.next_ptr->cnt += timer_p->cnt;
      }

      timer_p->link.next_ptr = NULL;
      timer_p->cnt = 0;
   }

   INTFREE( );

   return prev_value;

#endif /* FEATURE_USE_TIME_VU */

} /* END rex_clr_timer */
#endif /*MSM5000_IRAM_FWD*/


/*===========================================================================
FUNCTION REX_SET_TIMER

DESCRIPTION
  Sets a timer for expiration after a specific amount of time (in milliseconds).
  If the timer is already in the active timer list, the old timer values are
  overwritten by the new timer values. If a cnt of 0 is passed in, the
  timer expires immediately.

  Returns the timer's value before setting it to the new value.

  Note: p_timer must point to an initialized timer.

===========================================================================*/
rex_timer_cnt_type rex_set_timer(
   rex_timer_type     *p_timer,  /* pointer to timer to set  */
   rex_timer_cnt_type  cnt       /* value to set the timer   */
)
{
   rex_timer_cnt_type prev_value; /* the value before the set */

   INTLOCK();

#ifdef FEATURE_USE_TIME_VU

   prev_value = timer_get(p_timer, T_MSEC);

   (void) rex_clr_sigs( p_timer->tcb, p_timer->sigs );
   timer_set(p_timer, cnt, 0, T_MSEC);
#else

   /*-------------------------------------------------------
   ** Remove the timer from the timer list and insert it back
   ** if necessary.
   **-----------------------------------------------------*/
   prev_value = rex_clr_timer( p_timer );

   if (cnt > 0)
   {
      rex_insert_timer( p_timer, cnt );
      (void) rex_clr_sigs( p_timer->tcb_ptr, p_timer->sigs );
   }
   else
   {
      /*-------------------------------------------------------
      ** If the user passed in 0, set the signals or call the
      ** callback function.
      **-----------------------------------------------------*/

      do {
#ifdef FEATURE_REX_TIMER_EX
         /* If there is a callback associated with this timer, queue an
         ** APC so that the callback is invoked in the correct context.
         */
         if ( p_timer->cb_ptr != NULL )
         {
            if ( !rex_queue_apc(
                     p_timer->cb_ptr,
                     p_timer->tcb_ptr,
                     p_timer->cb_param
                  ) )
            {
               /* If the APC cannot be queued, we need to retry. So queue
               ** this timer up again at the front of the timer list.
               */
               rex_insert_timer( p_timer, 0 );
            }
            break;
         }
#endif /* FEATURE_REX_TIMER_EX */

         if ( p_timer->sigs != 0 )
         {
            (void)rex_set_sigs( p_timer->tcb_ptr , p_timer->sigs );
         }
#ifdef FEATURE_ZREX
#error code not present
#endif
      } while (0);
   }

#endif /* FEATURE_USE_TIME_VU */

   INTFREE();

   return prev_value;
} /* END rex_set_timer */


/*===========================================================================
FUNCTION REX_GET_TIMER

DESCRIPTION
  Returns the current remaining count of a timer. 0 is returned if the timer
  is not on the list.

===========================================================================*/
rex_timer_cnt_type rex_get_timer( 
  rex_timer_type *timer_p       /* pointer to the timer to get */
)
{

#ifdef FEATURE_USE_TIME_VU

   return timer_get(timer_p, T_MSEC);

#else

   rex_timer_cnt_type  prev_value = 0;    /* previous count */
   rex_timer_type     *next_timer_ptr = &rex_timer_list;

   INTLOCK( );

   REX_ASSERT(timer_p!=NULL);

   if( timer_p->link.next_ptr != NULL )
   {
      while ( next_timer_ptr != timer_p )
      {
         next_timer_ptr = next_timer_ptr->link.next_ptr;
         REX_ASSERT( next_timer_ptr != NULL );
         prev_value += next_timer_ptr->cnt;
      }
   }

   INTFREE( );

   return prev_value;

#endif /* FEATURE_USE_TIME_VU */

} /* END rex_get_timer */


/*===========================================================================
FUNCTION REX_PAUSE_TIMER

DESCRIPTION
  Pauses a timer.
===========================================================================*/
void rex_pause_timer(
   rex_timer_type *timer_p
)
{
#ifdef FEATURE_USE_TIME_VU

   timer_pause(timer_p);

#else

  timer_p->cnt = rex_clr_timer( timer_p );
  return;

#endif /* FEATURE_USE_TIME_VU */

} /* END rex_pause_timer */


/*===========================================================================
FUNCTION REX_RESUME_TIMER

DESCRIPTION
  Resumes a paused timer.
===========================================================================*/
void rex_resume_timer(
   rex_timer_type *timer_p
)
{
#ifdef FEATURE_USE_TIME_VU

   timer_resume(timer_p);

#else

   rex_set_timer( timer_p, timer_p->cnt );
   return;

#endif /* FEATURE_USE_TIME_VU */

} /* END rex_resume_timer */


#ifndef FEATURE_USE_TIME_VU
/*===========================================================================
FUNCTION REX_DECREMENT_TIMERS

DESCRIPTION
  Decrements the timer value for all valid timers in the active timer list.
  Interrupts must be disabled before calling this function.
  If a timer expires , rex_set_sigs is called and a task swap may be induced.
===========================================================================*/
INLINE static void rex_decrement_timers(
   rex_timer_cnt_type p_ticks    /* number of rex ticks to decrement by */
)
{
   rex_timer_type *ptr;             /* points to current timer */
   rex_timer_type *next_ptr;        /* points to next timer    */
#ifdef FEATURE_REX_TIMER_EX
   unsigned long apc_queued;
#endif

#ifdef REX_DEBUG
#error code not present
#endif

   /*-------------------------------------------------------
   ** Cycle through the timer list decrementing p_ticks from
   ** each timer. If the timer expired, remove it from the
   ** timer list and set the sigs for the timer's tcb.
   **-----------------------------------------------------*/
   for(ptr  = rex_timer_list.link.next_ptr;
       ptr != &rex_null_timer;
       ptr  = next_ptr
      )
   {
      REX_ASSERT(ptr!=NULL);
      next_ptr = ptr->link.next_ptr;
      if( ptr->cnt <= p_ticks )
      {
         rex_clr_timer( ptr );

#ifdef FEATURE_REX_TIMER_EX
         /* If there is a callback associated with this timer, queue an
         ** APC so that the callback is invoked in the correct context.
         */
         if ( ptr->cb_ptr != NULL )
         {
            if ( rex_is_in_irq_mode( ) )
            {
               apc_queued = rex_queue_apc(
                               ptr->cb_ptr,
                               ptr->tcb_ptr,
                               ptr->cb_param
                            );
            }
            else
            {
               apc_queued = rex_queue_apc_internal(
                               (unsigned long) ptr->cb_ptr,
                               (unsigned long) ptr->tcb_ptr,
                               ptr->cb_param
                            );
            }
            if ( !apc_queued )
            {
               /* If the APC cannot be queued, we need to retry. So queue
               ** this timer up again at the front of the timer list.
               */
               rex_insert_timer( ptr, 0 );
            }
               continue;
            }
#endif /* FEATURE_REX_TIMER_EX */

         /*---------------------------------------------------
         ** set the sigs for this task. This may cause a
         ** reschedule if the task whose timer just expired is
         ** the new best task
         **
         ** Note: ptr->sigs = 0 is a special case. This indicates
         ** that the timer expiry corresponds to the occurrence of
         ** a synchronization event wait timeout.
         **--------------------------------------------------*/
         if ( ptr->sigs != 0 )
         {
            (void) rex_set_sigs(ptr->tcb_ptr, ptr->sigs);
         }
#ifdef FEATURE_ZREX
#error code not present
#endif
      }
      else
      {
         /*---------------------------------------------------
         ** Subtract the number of rex_ticks this call is
         ** servicing from the timer at the front of the list.
         **-------------------------------------------------*/
         ptr->cnt -= p_ticks;
         break;
      }
   }

   return;
} /* END rex_decrement_timers */
#endif /* !FEATURE_USE_TIME_VU */


#ifndef FEATURE_USE_TIME_VU

/*===========================================================================
FUNCTION REX_TICK

DESCRIPTION
  If timers are enabled, calls rex_decrement_timers to decrement the timer
  value for all valid timers in the active timer list. Returns immediately
  if timers are disabled.
  Interrupts must be disabled before this function is called.
  If a timer expires, rex_set_sigs is called and a task swap may be induced.
===========================================================================*/
void rex_tick(
   rex_timer_cnt_type p_ticks    /* number of rex ticks to decrement everything by */
)
{
   REX_INTLOCK();

#ifdef FEATURE_REX_TICK_CONTROL
#error code not present
#endif
   {
      rex_decrement_timers( p_ticks );

#ifndef FEATURE_REX_PROFILING_SLEEP_COUNTER
#if defined( FEATURE_REX_PROFILE ) && !defined( FEATURE_HIGH_RES_PROFILE ) && \
   !defined(FEATURE_REX_PROFILING_GPTIMERS)
      rex_self( )->time_samples += p_ticks;
      rex_total_time_samples += p_ticks;
#endif
#endif
   }

   REX_INTFREE();

   return;

} /* END rex_tick */

#endif /* ! FEATURE_USE_TIME_VU */


/*===========================================================================
FUNCTION REX_TIMED_WAIT

DESCRIPTION
  Suspends a task and sets a timer. When either the timer or one of the
  signals is set and the task is the highest ready task, the task resumes.
  Returns the signal mask for the task. Causes a task swap if the signals
  are not already set for the calling task.
===========================================================================*/
rex_sigs_type rex_timed_wait(
   rex_sigs_type      sigs,         /* sigs to wait on          */
   rex_timer_type    *p_timer,      /* timer to set and wait on */
   rex_timer_cnt_type cnt           /* timer to wait            */
)
{
   REX_ASSERT( !rex_is_in_irq_mode( ) );

   /*-------------------------------------------------------
   ** A call to rex_set_timer and rex_wait will perform a
   ** timed wait
   **-----------------------------------------------------*/
   (void) rex_set_timer(p_timer, cnt);

   return rex_wait(sigs);
} /* END rex_timed_wait */


/* ==================================================================
FUNCTION REX_DELETE_TASK_TIMERS

DESCRIPTION
   Removes all the timers that belong to a specified task from the
   REX timer list.
===================================================================== */
void rex_delete_task_timers(
   rex_tcb_type *tcb_ptr
)
{
#ifdef FEATURE_USE_TIME_VU

   timer_clr_task_timers( tcb_ptr );

#else

   rex_timer_type *timer_p;
   rex_timer_type *next_timer_ptr;

   INTLOCK( );
      for( timer_p  = rex_timer_list.link.next_ptr;
           timer_p != &rex_null_timer;
           timer_p  = next_timer_ptr )
      {
         next_timer_ptr  = timer_p->link.next_ptr;
         if ( timer_p->tcb_ptr == tcb_ptr )
         {
            rex_clr_timer( timer_p );
         }
      }
   INTFREE( );

   return;

#endif /* FEATURE_TIME_VU */

} /* END rex_delete_task_timers */


#ifdef FEATURE_REX_TICK_CONTROL
#error code not present
#endif /* FEATURE_REX_TICK_CONTROL */

#ifdef FEATURE_REX_TIMER_EX
/* ==================================================================
FUNCTION REX_DEF_TIMER_EX
DESCRIPTION
   Extended version of timer-definition function. Allows the
   specification of a callback function and an argument to this
   function. The callback is called when the timer expires. The
   callback happens in the context of the task that calls
   rex_def_timer_ex.
===================================================================== */
void rex_def_timer_ex(
   rex_timer_type    *timer_p,
      /* pointer to a valid timer structure */
   rex_timer_cb_type  timer_cb_ptr,
     /* pointer to timer callback           */
   unsigned long      cb_param
     /* argument passed to timer callback   */
)
{
#ifdef FEATURE_USE_TIME_VU

   timer_def( timer_p, &rex_timer_group,
              rex_self(), 0,
              timer_cb_ptr, cb_param);

#else

#ifdef FEATURE_REX_TIMER_NAME
   static char timer_name[ ] = "REX unnamed timer";
#endif

   REX_INTERCEPT_DEF_TIMER_EX( timer_p, rex_self( ), 0 );

#ifdef FEATURE_REX_TIMER_NAME
   strcpy( timer_p->name, timer_name );
#endif

   timer_p->link.next_ptr = NULL;
   timer_p->link.prev_ptr = NULL;
   timer_p->cnt           = 0;
   timer_p->tcb_ptr       = rex_self( );
   timer_p->sigs          = 0;
   timer_p->cb_ptr        = timer_cb_ptr;
   timer_p->cb_param      = cb_param;

   return;

#endif /* FEATURE_USE_TIME_VU */

} /* rex_def_timer_ex */

#ifdef FEATURE_REX_DYNA_MEM

/* ==================================================================
FUNCTION REX_CREATE_TIMER_EX
DESCRIPTION
   Similar to rex_def_timer_ex, except that this function allocates
   the memory for the timer dynamically. A pointer to the timer is
   returned.
===================================================================== */
rex_timer_type *rex_create_timer_ex(
   rex_timer_cb_type  timer_cb_ptr,
     /* pointer to timer callback           */
   unsigned long      cb_param
     /* argument passed to timer callback   */
)
{
   rex_timer_type *timer_p = NULL;

   timer_p = (rex_timer_type *) rex_malloc( sizeof( rex_timer_type ) );
   if ( timer_p == NULL )
   {
      return NULL;
   }

   rex_def_timer_ex( timer_p, timer_cb_ptr, cb_param );

   return timer_p;
} /* END rex_create_timer_ex */


/* ==================================================================
FUNCTION REX_DELETE_TIMER_EX
DESCRIPTION
   Frees the memory allocated for a timer by rex_create_timer_ex.
===================================================================== */
void rex_delete_timer_ex(
   rex_timer_type *timer_p
)
{
#ifdef REX_INTERCEPT_KILL_TIMER
   REX_INTERCEPT_KILL_TIMER( timer_p );
#endif

   REX_ASSERT( timer_p != NULL );
#ifdef FEATURE_USE_TIME_VU
   (void) timer_clr( timer_p, T_NONE );
#else
   REX_ASSERT( timer_p->link.next_ptr==NULL );
#endif

   rex_free( timer_p );

   return;
} /* rex_delete_timer_ex */

#endif /* FEATURE_REX_DYNA_MEM */

#endif /* FEATURE_REX_TIMER_EX */

/* ==================================================================
FUNCTION REX_SYS_GET_TIMER_LIST
DESCRIPTION
   Returns a list of the current rex timers. Must pass in an array
   of REX_SYS_MAX_TIMER_IDS of rex_timer_types.
===================================================================== */
#ifndef FEATURE_USE_TIME_VU
void rex_sys_get_timer_list(
   rex_timer_type **timer_list
)
{
   rex_timer_type *rex_timer_ptr;
   int ii = 0;
   INTLOCK();
      for(
          rex_timer_ptr = rex_timer_list.link.next_ptr;
          ( rex_timer_ptr != &rex_null_timer ) &&
             ( ii < REX_SYS_MAX_TIMER_IDS -1 );
          rex_timer_ptr = rex_timer_ptr->link.next_ptr
         )
      {
         timer_list[ii] = rex_timer_ptr;
         ii++;
      }

      timer_list[ii] = NULL; /* end the list */
   INTFREE();

   return;
} /* END rex_sys_get_timer_list */

#endif
