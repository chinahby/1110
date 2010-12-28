/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            REX Task Profiling

GENERAL DESCRIPTION
  Task profile logging, timer, and control


Copyright (c) 2002 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/rextp.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/08    cz     Fixed a Klockwork error.
07/27/05    ck     Fixed the data abort issue when rextp.per_task_tcb_ptr
                   was pointing to a dynamic task that had been killed.
12/12/04    is     Added REX profiling for 6250 FEATURE_REX_PROFILING_6250.
02/18/05    ck     Fixed a critical Lint error
08/11/04    is     Added REX profiling for new time VU.
03/10/04    gr     Changed FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING to
                   FEATURE_HIGH_RES_PROFILE.
10/15/03    as     Included FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING
08/13/03    gr     Added inclusion of string.h.
07/30/03    gr     Replaced 6100-specific code.
07/11/03    ht     Featured clkrgm_6100.h for Jaguar.
06/20/03    gr     Cleanup. Deleted unnecessary externs, renamed a feature,
                   fixed indentation, etc.
04/23/03    gr     Fixed a featurization error.
11/14/02   jct     Formatted for updated log packet and semantics.
07/22/02    gr     Created.
===========================================================================*/

/* ---------------------------------------------------------------------------
** Includes
** ---------------------------------------------------------------------------  */
#include "customer.h"
#include "comdef.h"
#include "log.h"
#include "rex.h"
#include "rexlog.h"
#include "msg.h"
#ifdef FEATURE_SI_STACK_WM
#error code not present
#endif
#include "task.h"

#include <string.h>

#ifdef FEATURE_REX_PROFILE


#ifdef FEATURE_REX_PROFILING_GPTIMERS
#include "gptmrs.h"
static gptmr_handle_type rextp_gptmr = GPTMR_INVALID;
/* so that we can put profiling data into the task structure */
extern rex_tcb_type rex_kernel_tcb;
extern rex_tcb_type  * rex_curr_task;
#endif

/* ---------------------------------------------------------------------------
** Data
** ---------------------------------------------------------------------------  */

static struct {
   boolean         enabled;
   uint32          sp_period;
   uint32          tp_period;
   uint32          num_tasks_per_period;
   rex_tcb_type   *sp_tcb_ptr;
   rex_tcb_type   *per_task_tcb_ptr;
   rex_timer_type  sp_timer;
   rex_timer_type  per_task_timer;
} rextp = { 0 };

#define REX_TASK_LIST_FRONT() (&rex_task_list)
#define REX_TASK_LIST_NEXT( tcb_ptr ) ((rex_tcb_type *) tcb_ptr->link.next_ptr )

extern rex_tcb_type rex_task_list;

#ifndef FEATURE_REX_PROFILING_SLEEP_COUNTER
extern void clk_switch_enable( void );
#endif

#if defined(FEATURE_USE_TIME_VU) || defined  (FEATURE_REX_PROFILING_GPTIMERS)

/* Stores the total number of time samples that have elapsed since
** profiling was enabled.
*/
extern unsigned long rex_total_time_samples;
/* Last time when the swap happens */
uint32 rextp_last_swap_timetick;
/* Maximum timetick took between each update */
uint32 rextp_max_elapse_timetick;
/* Number of times in IRQ */
uint32 rextp_in_irq_count;
#endif

/*===========================================================================
FUNCTION REXTP_LOG_SYS_PROFILE_CB

DESCRIPTION
   Logs the system profile packet, which contains system profile information.
   This function keeps track of the last task processed by updating the
   sys_profile_tcb_ptr.
============================================================================*/
static void rextp_log_sys_profile_cb( unsigned long param )
{
   LOG_SYSPROFILE_C_type *sp_ptr;
   rex_tcb_type          *tcb_ptr;
   uint32 i;

   INTLOCK();

      for( i=0; i<rextp.num_tasks_per_period; i++ )
      {
         /* If we're at the end of the list or haven't started, then
         ** get a pointer to the front
         */
         if( rextp.sp_tcb_ptr == NULL )
         {
            rextp.sp_tcb_ptr = REX_TASK_LIST_FRONT();
         }

         /* Assure we have a valid TCB
         */
         if( rextp.sp_tcb_ptr != NULL )
         {
            tcb_ptr = rextp.sp_tcb_ptr;

            /* Allocate a log record
            */
            sp_ptr = (LOG_SYSPROFILE_C_type *)log_alloc(
                                                 LOG_SYSPROFILE_C,
                                                 sizeof(LOG_SYSPROFILE_C_type)
                     );

            /* Fill in the log if allocation worked
            */
            if( sp_ptr != NULL )
            {
               sp_ptr->total_num_tasks  = (uint32)rex_get_num_tasks();
               sp_ptr->task_id          = tcb_ptr->thread_id;
               sp_ptr->stack_size       = tcb_ptr->stack_size;
               /* Temporary hack. Until QXDM has support for displaying
               ** stack watermarks, use the stack_limit field to display
               ** this information.
               ** GR: 04/13/03
               */
               #ifdef FEATURE_SI_STACK_WM
#error code not present
               #else
               sp_ptr->stack_limit   = (uint32)tcb_ptr->stack_limit;
               #endif
               sp_ptr->max_intlock_time = 0;  /* not supported */
               sp_ptr->max_intlock_lr   = 0;  /* not supported */

               (void)memset(
                  (void*)sp_ptr->task_name,
                  0x00,
                  LOG_REX_SYSPROFILE_TASK_NAME_LEN
               );

               (void)memcpy(
                  (char *)sp_ptr->task_name,
                  (const char *)tcb_ptr->task_name,
                  REX_TASK_NAME_LEN+1
               );

               log_commit( sp_ptr );
            } /* END if valid log record */

            rextp.sp_tcb_ptr = REX_TASK_LIST_NEXT( rextp.sp_tcb_ptr );
            
         } /* END if valid tcb_ptr */
         
      } /* END for each tcb */
   INTFREE();

   (void) rex_set_timer (
      &rextp.sp_timer,
      rextp.sp_period
   );
   return;
} /* END rextp_log_sys_profile_cb */


/*===========================================================================
FUNCTION REXTP_LOG_PER_TASK_PROFILE_CB

DESCRIPTION
   Logs the per task profile packet, which contains each task profile
   information. The per task logs are combined until we have information about
   the number of tasks per period information is available. Then it sends a
   single packet. This function keeps track of the last task processed by
   updating the per_task_profile_tcb_ptr.
============================================================================*/
void rextp_log_per_task_profile_cb( unsigned long param )
{
   LOG_TASKPROFILE_C_type *tp_ptr;
   rex_tcb_type           *tcb_ptr;
   uint32 i;

   INTLOCK();

      for( i=0; i<rextp.num_tasks_per_period; i++ )
      {
         /* If we're at the end of the list or haven't started, then
         ** get a pointer to the front
         */
         if( rextp.per_task_tcb_ptr == NULL )
         {
            rextp.per_task_tcb_ptr = REX_TASK_LIST_FRONT();
         }

         /* Assure we have a valid TCB
         */
         if( rextp.per_task_tcb_ptr != NULL )
         {
            tcb_ptr = rextp.per_task_tcb_ptr;

            /* Allocate a log record
            */
            tp_ptr = (LOG_TASKPROFILE_C_type *)log_alloc(
                                              LOG_TASKPROFILE_C,
                                              sizeof(LOG_TASKPROFILE_C_type)
                     );

            /* Fill in the log if allocation worked
            */
            if( tp_ptr != NULL )
            {
               tp_ptr->task_id  = tcb_ptr->thread_id;
               tp_ptr->sp       = (uint32)tcb_ptr->sp;
               tp_ptr->sigs     = tcb_ptr->sigs;
               tp_ptr->wait     = tcb_ptr->wait;
               tp_ptr->pri      = tcb_ptr->pri;
               tp_ptr->time_samples  = tcb_ptr->time_samples;
               tp_ptr->flags        |= LOG_TASKPROFILE_REX_SLICE_TYPE_BIT;
               tp_ptr->total_samples = rex_get_total_time_samples( );

               if( tcb_ptr->suspended )
               {
                  tp_ptr->flags |= LOG_TASKPROFILE_REX_SUSPENDED_BIT;
               }

               log_commit( tp_ptr );
            } /* END if valid log record */
            
            rextp.per_task_tcb_ptr = REX_TASK_LIST_NEXT( rextp.per_task_tcb_ptr );
         
         } /* END if valid tcb_ptr */

      } /* END for each tcb */
   INTFREE();

   (void) rex_set_timer (
      &rextp.per_task_timer,
      rextp.tp_period
   );
   return;
} /* END rextp_log_per_task_profile_cb */


/*===========================================================================
FUNCTION REXTP_LOG_PER_TASK_PROFILE

DESCRIPTION
   Logs the per task profile packet, which contains each task profile
   information. The per task logs are combined until we have information about
   the number of tasks per period information is available. Then it sends a
   single packet. This function keeps track of the last task processed by
   updating the per_task_profile_tcb_ptr.
============================================================================*/
void rextp_log_per_task_profile( void )
{
   LOG_TASKPROFILE_C_type *tp_ptr;
   rex_tcb_type           *tcb_ptr;

   INTLOCK();

   tcb_ptr = REX_TASK_LIST_FRONT();
   while (tcb_ptr != NULL)
   {
     /* Allocate a log record
     */
     tp_ptr = (LOG_TASKPROFILE_C_type *)log_alloc(
                                       LOG_TASKPROFILE_C,
                                       sizeof(LOG_TASKPROFILE_C_type)
              );

     /* Fill in the log if allocation worked
     */
     if( tp_ptr != NULL )
     {
        tp_ptr->task_id  = tcb_ptr->thread_id;
        tp_ptr->sp       = (uint32)tcb_ptr->sp;
        tp_ptr->sigs     = tcb_ptr->sigs;
        tp_ptr->wait     = tcb_ptr->wait;
        tp_ptr->pri      = tcb_ptr->pri;
        tp_ptr->time_samples  = tcb_ptr->time_samples;
        tp_ptr->flags        |= LOG_TASKPROFILE_REX_SLICE_TYPE_BIT;
        tp_ptr->total_samples = rex_get_total_time_samples( );

        if( tcb_ptr->suspended )
        {
           tp_ptr->flags |= LOG_TASKPROFILE_REX_SUSPENDED_BIT;
        }

        log_commit( tp_ptr );
     } /* END if valid log record */
     else
     {
       /* No log packet available, cut the logging short
       ** and yield to other tasks.
       */
       break;
     }

     tcb_ptr = REX_TASK_LIST_NEXT( tcb_ptr );

   }

   INTFREE();

} /* END rextp_log_per_task_profile */

/*===========================================================================
FUNCTION REX_PROFILING_CLEAR_DATA

DESCRIPTION
  This function clears the profiling data collected by rex profiling.
============================================================================*/
void rex_profiling_clear_data( void )
{
   rex_tcb_type *tcb_ptr;

   INTLOCK();
      tcb_ptr = REX_TASK_LIST_FRONT();

      while ( tcb_ptr )
      {
         tcb_ptr->time_samples = 0;
         tcb_ptr = REX_TASK_LIST_NEXT( tcb_ptr );
      }

#ifdef FEATURE_USE_TIME_VU
      rextp_last_swap_timetick = timetick_get_safe(); /* Timetick at the last task swap */
      rextp_in_irq_count=0;
      rextp_max_elapse_timetick = 0;
#endif

#ifdef FEATURE_REX_PROFILING_GPTIMERS
      /* Free up the timer */
      (void) gptmr_stop(rextp_gptmr);
      gptmr_free(rextp_gptmr);
      rextp_gptmr = GPTMR_INVALID;
#endif

      rex_set_total_time_samples( 0 );
   INTFREE();

   return;
}


/*===========================================================================
FUNCTION REX_ENABLE_TASK_PROFILE

DESCRIPTION
  This function fills in the system profile period, task profile period and
  number of tasks per period. It calls the task profile logging function and
  system profile logging function.
============================================================================*/
void rex_enable_task_profile(
   uint32 sys_period,
   uint32 task_period,
   uint32 tasks_per_period
)
{
   if( rextp.enabled == FALSE )
   {
      rextp.enabled = TRUE;

      rextp.sp_period            = sys_period;
      rextp.tp_period            = task_period;
      rextp.num_tasks_per_period = tasks_per_period;

      rex_profiling_clear_data();

#ifdef FEATURE_REX_PROFILING_GPTIMERS
      /*  Request for GP timer and start the timer
      ** if either fails, bail out
      */
      rextp_gptmr = gptmr_alloc();
      if(!gptmr_start(rextp_gptmr))
      {
        rextp.enabled = FALSE;
        return;
      }

#endif

      rex_enable_profiling();

#ifndef FEATURE_REX_PROFILING_SLEEP_COUNTER
      /* Start the sampling timer
      */
#ifdef FEATURE_HIGH_RES_PROFILE
      clk_switch_enable();
#endif
#endif

      /* Start per task logging
      */
      rex_def_timer_ex (
         &rextp.per_task_timer,
         rextp_log_per_task_profile_cb,
         0
      );

      (void) rex_set_timer (
         &rextp.per_task_timer,
         rextp.tp_period
      );

      /* Start system state logging
      */
      rex_def_timer_ex (
         &rextp.sp_timer,
         rextp_log_sys_profile_cb,
         0
      );

      (void) rex_set_timer (
         &rextp.sp_timer,
         rextp.sp_period
      );
   }
   return;
} /* END rex_enable_task_profile */

/*===========================================================================
FUNCTION REX_DISABLE_TASK_PROFILE

DESCRIPTION
  This function clears the timers both the rex_sys_profile_timer and
  rex_per_task_profile_timer
============================================================================*/
void rex_disable_task_profile(void)
{
   if( rextp.enabled == TRUE )
   {
      rextp.enabled = FALSE;

#ifdef FEATURE_REX_PROFILING_GPTIMERS
      /* Free up the timer */
      (void)gptmr_stop(rextp_gptmr);
      gptmr_free(rextp_gptmr);
      rextp_gptmr = GPTMR_INVALID;
#endif

      rex_disable_profiling( );

      (void) rex_clr_timer( &rextp.per_task_timer );

      (void) rex_clr_timer( &rextp.sp_timer );
   }
   return;
} /* END rex_disable_task_profile */


#ifdef FEATURE_REX_PROFILING_GPTIMERS
/*===========================================================================
FUNCTION REXTP_UPDATE_ISR_TIME_SAMPLES

DESCRIPTION
  Read GP timer register and update the time sample in ISR context
  into "REX idle task" time sample.

DEPENDENCY
  REX profing is active and GP Timer is running when this function is called
============================================================================*/

void rextp_update_isr_time_samples(void)
{
   int cnt;

   /* Read and restart the timer */
   cnt = gptmr_read(rextp_gptmr, GPTMR_READ_AND_RESTART);

   /* Check on cnt. No need to update if it is 0.
   ** If cnt is not positive, something could be wrong
   ** with the timer setup
   */
   if (cnt > 0)
   {
      cnt >>= 5; /* GP timer is ticking too fast. Slow it down to 1.28ms per tick */
      rex_kernel_tcb.time_samples += cnt;
      rex_total_time_samples += cnt;
   }
}

/*===========================================================================
FUNCTION REXTP_UPDATE_TASK_TIME_SAMPLES

DESCRIPTION
  Read GP timer register and update the time sample in current task context

DEPENDENCY
  REX profing is active and GP Timer is running when this function is called

============================================================================*/
void rextp_update_task_time_samples(void)
{
   int cnt;

   /* Read and restart the timer */
   cnt = gptmr_read(rextp_gptmr, GPTMR_READ_AND_RESTART);

   /* Check on cnt. No need to update if it is 0.
   ** If cnt is not positive, something could be wrong
   ** with the timer setup
   */
   if (cnt > 0)
   {
      cnt >>= 5; /* GP timer is ticking too fast. Slow it down to 1.28ms per tick */
      rex_curr_task->time_samples += cnt;
      rex_total_time_samples += cnt;
   }
}

#endif /* FEATURE_REX_PROFILING_GPTIMERS */

/*===========================================================================
FUNCTION REXTP_REMOVE_TASK

DESCRIPTION
  This function checks to see if rextp_per_task points to the task that is
  being removed and updates rextp_per_task to point to the next task if that
  is the case
============================================================================*/
void rextp_remove_task (
   rex_tcb_type *tcb_ptr  /* pointer to TCB */
)
{
   if (rextp.per_task_tcb_ptr == tcb_ptr)
   {
      rextp.per_task_tcb_ptr = REX_TASK_LIST_NEXT( tcb_ptr );
   }

   if (rextp.sp_tcb_ptr == tcb_ptr)
   {
      rextp.sp_tcb_ptr = REX_TASK_LIST_NEXT( tcb_ptr );
   }
}

#endif /*#ifdef FEATURE_REX_PROFILE*/

