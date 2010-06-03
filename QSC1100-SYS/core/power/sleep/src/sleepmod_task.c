/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S L E E P   M O D E M    T A S K

GENERAL DESCRIPTION
  This file containes the modem/single processor specific functions 
  used to support sleep on REX targets

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Functions to be called form sleepmod.c only.

                           Copyright (c) 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/os/rex/sleepmod_task.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/09   cab     Added gSleepRestrictions for compilation
08/22/08   clm     1H08 add support for BIO soft switch for QSC6270.
06/09/08   clm     Inital version.

===========================================================================*/

/*=========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "target.h"
#include "sleep.h"
#include "sleepi.h"
#include "sleepmodi.h"
#include "msg.h"
#include "clkregim.h"
#include "sleeplog.h"
#include "log.h"
#include "dog.h"
#include "task.h"
#include "bio.h"

#ifdef FEATURE_MULTIMODE_RFAPI
  #include "rfm.h"
#else
  #include "rf.h"
#endif /* FEATURE_MULTIMODE_RFAPI */

/* Flags instructing sleep code to disable various sleep modes.  Each of these
   flags can be set by the Trace32 CMM scripts.  If it is left FALSE by
   Trace32, NV will be consulted and may also disable some flags. */
/* Not generally used in REX builds, but declared here for alignment 
   with L4 builds */
volatile sleep_restriction_type gSleepRestrictions;

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */

/*===========================================================================

FUNCTION SLEEP_DOG_REPORT_NOTIFY

DESCRIPTION
  This procedure sets the SLEEP_RPT_TIMER_SIG for the SLEEP task, so it will
  report to the DOG task.

  In addition, it boosts the priority of the sleep task.  This ensures that
  the SLEEP task will not dog time-out when a low priority CPU intensive
  task runs.

DEPENDENCIES
  None

RETURN VALUE
  This procedure does not return.

SIDE EFFECTS
  SLEEP task's priority is raised & the SLEEP_RPT_TIMER_SIG is set.

===========================================================================*/

static void sleep_dog_report_notify
(
  /* Time elapsed since the timer was started, or restarted */
  int4 unused_time_ms
)
{
  /* Raise sleep task's priority above CPU intensive low priority tasks */
  (void) rex_task_pri( &sleep_tcb, SLEEP_PRI_HI );
    
  /* Signal that it is time to report to the DOG task */
  (void) rex_set_sigs( &sleep_tcb, SLEEP_RPT_TIMER_SIG );
} /* sleep_dog_report_notify( ) */

/*===========================================================================

FUNCTION SLEEP_TASK

DESCRIPTION
  This procedure contains the main processing loop for the sleep task.
  It continually loops, looking for a consensus that the CPU may be
  put to sleep.  Lacking a consensus, it still halts the processor, but
  in "idle" mode -- in which any normal interrupt will restart processing
  with minimal delay.

DEPENDENCIES
  This procedure should be called by rex_def_task.

RETURN VALUE
  This procedure does not return.

SIDE EFFECTS
  The CPU may be put to sleep by this task.

===========================================================================*/

void sleep_task
(
  /* Parameter received from Main Control task - ignored */
  dword  unused_param
)
{
  /* RF temp comp: Previously, as a callback, RF temp comp was uninterruptable.
     This created problems where time-critical tasks would missed their
     deadlines.  Calling RF temp comp from the sleep task (a low priority task)
     leaves the CPU interruptable and available for other higher priority
     and time-critical tasks. */
  rex_timer_type sleepRfTempCompTimer;
  /* Signals returned from REX */
  rex_sigs_type  sleepSigs;
  /* Watchdog report timer */
  rex_timer_type sleepRptTimer;

  sleep_init();

  /* Initialize the watchdog report timer for task_start(...)*/
  rex_def_timer( &sleepRptTimer, &sleep_tcb, SLEEP_RPT_TIMER_SIG );
  /* Initialize the watchdog report timer */
  clk_def( &gSleepInfo.dog_report_timer);

  gSleepInfo.log.task_stats.hdr.size =  sizeof(sleeplog_task_stats_v2);
  gSleepInfo.log.task_stats.hdr.id = SLEEPLOG_ID_TASK_STATS;
  gSleepInfo.log.task_stats.hdr.ver = SLEEPLOG_VERSION_2;
  gSleepInfo.log.task_stats.cl_hdr.max_name_len = SLEEPLOG_CLIENT_LIST_MAX_STR;
  gSleepInfo.log.task_stats.cl_hdr.max_names = SLEEPLOG_CLIENT_LIST_MAX_NUM;

  /* Initialize RF temp cal timer */
  rex_def_timer(&sleepRfTempCompTimer, &sleep_tcb, SLEEP_RF_TEMP_COMP_SIG);

  /* Give the address to the timer block to the RF driver, so that it can set
     the timer to set/clr the signals */
  RF_CDMA_TEMP_COMP_INIT(sleepRfTempCompTimer);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process task start procedure from task controller. */
  task_start( SLEEP_RPT_TIMER_SIG, DOG_SLEEP_RPT, &sleepRptTimer );
  gSleepInfo.task_running = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the task_start(...) watchdog timer after task is started. */
  (void) rex_clr_timer( &sleepRptTimer );

  /* Set watch dog timer after task is started. */
  clk_reg( &gSleepInfo.dog_report_timer, 
           sleep_dog_report_notify, 
           DOG_SLEEP_RPT_TIME,
           DOG_SLEEP_RPT_TIME, 
           TRUE );

  /* Loop forever */
  for (;;)
  {
#ifdef FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING
    clk_regime_test_cpu_idle_and_switch_mclk();
#endif /* FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING */


    /* Query and clear any signals the sleep task must respond to */
    sleepSigs = rex_clr_sigs( &sleep_tcb, SLEEP_TASK_SIGS) & SLEEP_TASK_SIGS;

    /* If a task signal was set, we must respond to it */
    if ( sleepSigs != 0 )
    {
      /* If we're supposed to stop, let's do it */
      if ( (sleepSigs & SLEEP_STOP_SIG) != 0 )
      {
        /* Process task stop procedure from task controller. */
        gSleepInfo.task_running = FALSE;
        task_stop();
      }

      /* If we're supposed to go offline, let's do it */
      if ( (sleepSigs & SLEEP_OFFLINE_SIG) != 0 )
      {
        /* Process task offline procedure from task controller. */
        gSleepInfo.task_running = FALSE;
        task_offline();
      }

      /* Check to see if the RF driver had requested us to start its
         temp comp process. */
      if ( (sleepSigs & SLEEP_RF_TEMP_COMP_SIG) != 0 )
      {
        /* Call RF temp comp so that it can update the temperature, perform
           temp comp and load the cagc linearizers.  We're doing this as a
           favor for RF.  This portion of the RF code needs to be executed
           from a task, so that it will remain interruptable. */
        RF_CDMA_DO_TEMP_COMP();
      }

      /* If watchdog report timer expired, report and restart the timer */
      if ( (sleepSigs & SLEEP_RPT_TIMER_SIG) != 0 )
      {
        dog_report(DOG_SLEEP_RPT);

        /* Interrupts must be locked for rex_set_best_task_from_all() */
        INTLOCK();

        /* Return back to our regular priority ... */
        (void) rex_task_pri( &sleep_tcb, SLEEP_PRI);

        /* ... determine best task now that our priority has dropped ... */
        rex_set_best_task_from_all();

        /* ... & switch to that task, which could still be the sleep task */
        rex_sched();

        /* Sleep Task Priority reduction complete */
        INTFREE();
      }
    }
    else  /* sleepSigs == 0 */
    {
      /* No other runnable higher priority tasks, and this task has
         nothing to do.  Time to extend battery life by powering down
         as much circuitry as possible, and halting the CPU. */

      /* Don't do TCXO shutdown or ARM halt, if QXDM dip switch is set. */
      if ( BIO_GET_SW ( BIO_SW9_M ) == 0 )
      {
#ifndef FEATURE_SLEEP_MODEM_DISABLED
        sleep_power_down_and_halt( );
#endif /* FEATURE_SLEEP_MODEM_DISABLED */
      }
    }


#if defined( FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING ) || \
    defined( FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)
    
    if (clock_frequency_state == CLKRGM_HIGH_FREQUENCY_STATE)
    {
      MSG_LOW("ARM clock in high frequency state",0,0,0);
    }
    else
    {
      MSG_LOW("ARM clock in low frequency state",0,0,0);
    }

#endif /* FEATURE_..._CLOCK_FREQUENCY_CHANGING */

  } /* end of infinite loop */
} /* end of sleep_task */

