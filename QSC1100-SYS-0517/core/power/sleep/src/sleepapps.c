/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     S L E E P   A P P L I C A T I O N S   P R O C E S S O R    T A S K

GENERAL DESCRIPTION
  The sleep task determines when the apps task may vote for TCXO shutdown
  or be power collapsed.

EXTERNALIZED FUNCTIONS
  sleep_task
    This is the entry procedure for the sleep task called by rex_def_task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The sleep_task function contains the sleep task for the applications 
  processor.  It must be specified in a call to rex_def_task and started 
  by the Main Control task.

                        Copyright (c) 2005-2009
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleepapps.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/09   ajf     Check gpSleepSmemSleepDisabled against a mask, rather than
                   just verifying that it is non-zero.
01/06/09   taw     Don't subtract APPS_PWR_TURN_ON_SCLK from the power collapse
                   time; this has been moved into DEM.
12/15/08   taw     Removed the check for SMSM_SLEEP.
09/23/08   clm     Renamed disable all sleep variable.
06/27/08   clm     1H08 sleep re-work.
03/20/08   jfl     Reverted SLEEP_UIM_OKTS_SIG removal
03/13/08   jfl     Removed SLEEP_UIM_OKTS_SIG
03/13/08   jfl     Removed SLEEP_HS_OKTS_SIG
12/10/07   cab     Removed SLEEP_USB_OKTS_SIG, removed some assoc. loggin stats
11/27/07   cab     Removed SLEEP_HDR_OKTS_SIG.
10/04/07   taw     Prevent sleep if sleep.allow_sleep is not set; call
		           sleep_init() before entering the main loop of sleep_task().
09/12/07   taw     Remove the restriction to not power collapse if T_MSM7600 
                   is defined.
08/14/07   ebb     Changed sleep_log_event to once again log only the mask.
                   Name resolution is now done in smem_log.pl.
08/07/07   taw     Removed FEATURE_APPS_POWER_COLLAPSE where appropriate, to 
                   allow for compilation/operation when this feature is not
                   defined
07/20/07   taw     Removed the sleep_hs_usb_in_use global.
07/11/07   ebb     Updated sleepapps_pc_or_halt to log new sleep vote
                   names.
07/06/07   cab     Removed SLEEP_CAMERA_OKTS_SIG
06/25/07   ebb     Added sleepmemlog.c to handle local/smem logging.
06/13/07   ebb     Merged in msm7500 orphans to bring tip to 7500I
06/12/07   ebb     Removed SLEEP_VOC_OKTS_SIG, SLEEP_SND_OKTS_SIG, 
                   SLEEP_VS_OKTS_SIG
05/22/07   cab     Added support to not do SWFI if something has their SWFI
                   restriction set to SLEEP_SWFI_RESTRICT_ON.
05/22/07   cab     Support for TCM save/restore during sleep
05/22/07   cab     Check sleep_hs_usb_in_use before going into SWFI.
05/03/07   cab     Removed SLEEP_RXC_OKTS_SIG
05/02/07   cab     Removed SLEEP_SRCH_OKTS_SIG
05/01/07   cab     Changed qword to uint64
04/20/07   cab     Remove reference to SLEEP_HDRDEC_OKTS_SIG
                   Updated logging to include new sleep voters
04/12/07   cab     Removed ifdef's for LOG_SLEEP_APPS_STATS_C (mainlined)
03/20/07   taw     Check sleep_hs_usb_in_use before going into SWFI.
03/16/07   bfc     New SMEM api
12/06/06   ddh     Removed dmov_active_count as dmov is now voiting for sleep
11/27/06   cab     Removed SLEEP_UI_OKTS_SIG
11/08/06   ddh     Removed SLEEP_MDDI_OKTS_SIG and SLEEP_MDP_OKTS_SIG
10/30/06   jhs     Removed SLEEP_KYPD_OKTS_SIG.
10/27/06   cab     Added FEATURE_SLEEP_APPS_DISABLED
10/16/06   ddh     Corrected logging for new style sleep voters so as not to 
                   over run the allocated packet size.
10/13/06   ddh     Removed SLEEP_BT_OKTS_SIG 
10/09/06   ddh     Removed SLEEP_APPS_OKTS_SIG and SLEEP_DEM_OKTS_SIG after 
                   use was replaced with new voting mechanism
06/19/06  jwob     Updated sleep_log_task_stats to use v2 of the packet
06/14/06   ddh     Removed items common with modem which are now located in
                   sleep.c and sleepi.h. Also added support for new vote 
                   mechanism.
06/12/06   ddh     If apps power collapse is disabled then do not call
                   sleepapps_pc_or_halt, as even the SWFI only call causes
                   issues with the daisy chain debugging
06/08/06   ddh     Included check of MDP vote when determining if it is okay
                   to sleep.
05/22/06   ddh     Reneabled clk before executing SWFI clk function when not 
                   power collapsing or allowing TCXO sleep. Also added in an
                   SWFI when not all OKTS votes are set.
05/10/06   ddh     Added in SWFI when can't sleep or power collapse
05/02/06   taw     Check dmov_active_count to determine whether the data mover
                   is active; if count != 0, don't sleep or power collapse.
04/21/06   taw     Added a vote from DMOV to the OKTS votes.
04/20/06   ddh     Changed clk include to use clkregim.h 
04/14/06   taw     For now, only do apps sleep if power collapse is not disabled.
04/11/06   ddh     Updated sleep_apps_power_collapse_disabled to use
                   smem_data variable
04/10/06   ddh     Updated logic for debugging info so as not to flood the log
03/31/06   ddh     Added debugging information 
03/14/06   taw     Made power collapsing dependent on shared memory variable 
                   in order to turn it on/off from cmm scripts and at runtime.
                   Also, enclosed TCXO shutdown entry point in a DEM define.
02/06/06   taw     Added testing of the power_collapse variable for logging
                   after sleep.
02/02/06   ddh     Removed #ifdef on inclusion of #dem.h
01/27/06   taw     Changed time required to power collapse.
01/25/06   ddh     HW specific support for 7500 sleep and log code cleanup
06/22/05   ddh     Initial version 

===========================================================================*/

/*=========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "sleep.h"
#include "sleepi.h"
#include "sleeplog.h"

#include "mc.h"
#include "task.h"
#include "tramp.h"
#include "msg.h"
#include "dem.h"
#include "log.h"
#include "dmov.h"

#include "smem.h"
#include "smsm.h"

#ifdef FEATURE_RESISTIVE_KEYPAD
  #include "gpio_int.h"
#endif

#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */

#ifdef FEATURE_PMEM
  #include "pmem.h"
  #include "timetick.h"
#endif /* FEATURE_PMEM */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  Sleep local data
--------------------------------------------------------------------------*/
/* Flag in shared memory used to disable sleep */
static volatile uint32 *gpSleepSmemSleepDisabled;

/*--------------------------------------------------------------------------
  Sleep global data
--------------------------------------------------------------------------*/
/* Used to check whether or not TCXO has already been turned back on
   after sleep.  Helps avoid wasting time by waiting for it to turn
   on more than once. */
boolean gbSleepTcxoOn = TRUE;

/*--------------------------------------------------------------------------
  Sleep constants
--------------------------------------------------------------------------*/

/* Signals the Sleep task must clear and respond to */
#define SLEEP_TASK_SIGS \
  (                     \
    SLEEP_STOP_SIG    | \
    SLEEP_OFFLINE_SIG | \
    SLEEP_RPT_TIMER_SIG \
  )


#define SLEEP_OK_MASK          \
  (                            \
    SLEEP_DIAG_OKTS_SIG     |  \
    SLEEP_CHGT_OKTS_SIG     |  \
    SLEEP_UIM_OKTS_SIG      |  \
    SLEEP_GPSONE_OKTS_SIG   |  \
    SLEEP_MP4_OKTS_SIG      |  \
    SLEEP_GRAPHICS_OKTS_SIG    \
  )

#ifdef DUAL_PROC_APPS_SLEEP
  #ifdef T_MSM7500
#error code not present
  #else /* !T_MSM7500 */
    #define SLEEPAPPS_ADGT_DISABLE() 
    #define SLEEPAPPS_ADGT_ENABLE() 
  #endif /* T_MSM7500 */
#endif /* DUAL_PROC_APPS_SLEEP */

/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */

#ifdef DUAL_PROC_APPS_SLEEP
/*===========================================================================

FUNCTION SLEEPAPPS_CHECK_OKTS_VOTES

DESCRIPTION
  Check votes and other misc reasons not to perform app sleep or PC.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  okay to perform app sleep or PC.
  FALSE not okay to perform app sleep or PC.

SIDE EFFECTS
  None

===========================================================================*/

static boolean sleepapps_check_okts_votes(void)
{
  boolean        bRetVal = FALSE;
  rex_sigs_type  not_okts;

  do
  {
    if ( !gSleepInfo.task_running || !gSleepInfo.allow_sleep )
    {
      /* The global variable to prevent
         sleep is set.  Leave TCXO and power on. */
      break;
    }

    /* Determine which tasks (if any) have not set their OKTS signals */
    not_okts = SLEEP_OK_MASK & ~rex_get_sigs( &sleep_tcb );
    /* Record who did not vote for sleep, if anyone */
    gSleepInfo.log.task_stats.not_okts = not_okts;

    /* Record who did not vote for sleep, if anyone */
    if (not_okts)
    {
      sleep_log_event( NO_SLEEP_OLD, not_okts, 0, 0 );
      break;
    }

    /* snapshot the not okts votes and num voters for logging later */
    gSleepInfo.log.curr_not_okts_m = gSleepInfo.curr_not_okts_mask;
    gSleepInfo.log.num_clients = gSleepInfo.num_clients;

    /* process new sleep voters */
    if ( gSleepInfo.curr_not_okts_mask )
    {
      /* shoe new clients into the old sleep voting mask */
      not_okts |= 0x00008000;
      /* Record who did not vote for sleep, if anyone */
      gSleepInfo.log.task_stats.not_okts = not_okts;

      /* Log the mask of the no sleep voters */
      sleep_log_event( NO_SLEEP_NEW, APPS_SLEEP, 
                       (uint32)(gSleepInfo.curr_not_okts_mask >> 32), 
                       (uint32)gSleepInfo.curr_not_okts_mask );
      break;
    }

    if ( sleep_uart_clock_rgm )
    {
      break;
    }

    bRetVal = TRUE;
  } while(FALSE);

  return bRetVal;
} /* sleepapps_check_okts_votes */


/*===========================================================================

FUNCTION SLEEPAPPS_ALLOW_PC

DESCRIPTION
  Check to see if we can allow PC.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  okay to perform PC.
  FALSE not okay to perform PC.

SIDE EFFECTS
  Will disable timer groups, if they are not already disabled.

===========================================================================*/

static boolean sleepapps_allow_pc
(
  /* Used to determine whether timer groups need to be disabled in this 
     function.  Notify caller if timer groups are currently disabled. */
  boolean *pbDisabledTimerGroups, 
  /* Return Min sclk's until next wakeup */
  uint32  *puMinSclk
)
{
  boolean bRetVal = FALSE;
  boolean bDisabledTimerGroups = *pbDisabledTimerGroups;
  uint32  uMinSclk = 0;

  do
  {

    if (SLEEP_CHECK_APPS_PC_SCLK == 0)
    {
      /* PC is disabled */
      break;
    }

    if (*pbDisabledTimerGroups == FALSE)
    {
      /* Disable the clock tick before setting up powerdown,
         as any non-masked interrupt causes the CPU to disable
         the power saving modes. */
      clk_disable(); 
      bDisabledTimerGroups = TRUE;
      SLEEPAPPS_ADGT_DISABLE();
    }

    /* Retrieve the time (in sclks) until the next scheduled timer expiry */
    uMinSclk  = timer_get_sclk_till_expiry();

    if (uMinSclk < SLEEP_CHECK_APPS_PC_SCLK)
    {
      /* Insufficient time to halt and allow for PC */
      break;
    }

    bRetVal = TRUE;
  } while (FALSE);

  *pbDisabledTimerGroups = bDisabledTimerGroups;
  *puMinSclk = uMinSclk;
  return bRetVal;

} /* sleepapps_allow_pc */

/*===========================================================================

FUNCTION SLEEPAPPS_PC

DESCRIPTION
  Perform PC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void sleepapps_pc
(
  /* Sclk's until next wakeup */
  uint32 uNextWakeupSclk
)
{
  /* Sclk's needed to perform pmem_save */
  uint32 uTimeToSaveTcm;

  /* Power Collapse the apps side */
#ifdef FEATURE_POWER_COLLAPSE
#error code not present
#endif /* FEATURE_POWER_COLLAPSE */
} /* sleepapps_pc */

/*===========================================================================

FUNCTION SLEEPAPPS_ALLOW_APPS_SLEEP

DESCRIPTION
  Check to see if we can allow TCXO shutdown.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  okay to perform TCXO shutdown.
  FALSE not okay to perform TCXO shutdown.

SIDE EFFECTS
  Will disable timer groups, if they are not already disabled.

===========================================================================*/

static boolean sleepapps_allow_apps_sleep
(
  /* Used to determine whether timer groups need to be disabled in this 
     function.  Notify caller if timer groups are currently disabled. */
  boolean *pbDisabledTimerGroups, 
  /* Return Min sclk's until next wakeup */
  uint32  *puMinSclk
)
{
  boolean bRetVal = FALSE;
  boolean bDisabledTimerGroups = *pbDisabledTimerGroups;
  uint32  uMinSclk = 0;

  do
  {
    if (*pbDisabledTimerGroups == FALSE)
    {
      /* Disable the clock tick before setting up powerdown,
        as any non-masked interrupt causes the CPU to disable
        the power saving modes. */
      clk_disable(); 
      bDisabledTimerGroups = TRUE;
      SLEEPAPPS_ADGT_DISABLE();
    }

    /* Retrieve the time (in sclks) until the next scheduled timer expiry */
    uMinSclk  = timer_get_sclk_till_expiry();

    if (uMinSclk < SLEEP_CHECK_APPS_HALT_SCLK)
    {
      /* Insufficient time to halt and allow for TCXO shut down */
      sleep_log_event( INSUF_TIME, uMinSclk, 0, 0 );
      break;
    }

    bRetVal = TRUE;
  } while (FALSE);

  *pbDisabledTimerGroups = bDisabledTimerGroups;
  *puMinSclk = uMinSclk;
  return bRetVal;
} /* sleepapps_allow_apps_sleep */

/*===========================================================================

FUNCTION SLEEPAPPS_APPS_SLEEP

DESCRIPTION
  Perform TCXO shutdown.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void sleepapps_apps_sleep
(
  /* Sclk's until next wakeup */
  uint32 uNextWakeupSclk
)
{
#ifdef FEATURE_APPS_TCXO_SLEEP
  #ifdef DEM_INCLUDE_TCXO
  /* Only do apps sleep if power collapse is not disabled for now */
  sleep_log_event( SLEEP_INFO, uNextWakeupSclk, 0, 0 );
  dem_okts( uNextWakeupSclk, CLKRGM_MCLK_OFF, CLKRGM_HCLK_OFF );
  #endif /* DEM_INCLUDE_TCXO */
#endif /* FEATURE_APPS_TCXO_SLEEP */
} /* sleepapps_apps_sleep */

/*===========================================================================

FUNCTION SLEEPAPPS_ALLOW_MICRO_PROC_SLEEP

DESCRIPTION
  Check to see if we can perform micro proc sleep.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  okay to perform micro proc sleep.
  FALSE not okay to perform micro proc sleep.

SIDE EFFECTS
  None.

===========================================================================*/

static boolean sleepapps_allow_micro_proc_sleep(void)
{
  boolean bRetVal = FALSE;

  do
  {
    if ((gSleepInfo.curr_not_okts_mask & gSleepInfo.swfi_restriction_mask) 
        != 0)
    {
      /* If both not okts and swfi restriction mask have matching active bits
         it's not okay to micro proc sleep. */
      break;
    }

    if (tramp_is_any_interrupt_pending())
    {
      /* If we have any pending interrupts don't do micro proc sleep */
      break;
    }

    bRetVal = TRUE;
  } while (FALSE);
  return bRetVal;
} /* sleepapps_allow_micro_proc_sleep */

/*===========================================================================

FUNCTION SLEEPAPPS_MICRO_PROC_SLEEP

DESCRIPTION
  Perform micro processor sleep.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void sleepapps_micro_proc_sleep(void)
{
  clk_regime_apps_swfi(FALSE);
} /* sleepapps_micro_proc_sleep */


/*===========================================================================

FUNCTION SLEEPAPPS_PC_OR_HALT

DESCRIPTION
  This procedure determines if the apps subsystem should be power collapsed
  or if the apps CPU should be halted. If the CPU is halted any normal 
  interrupt will restart processing with minimal delay. If all of the Apps 
  OKTS signals are set AND the either the CPU is halted or power collapse is 
  initiated TCXO may be shut off.

DEPENDENCIES
  Called only from sleep task

RETURN VALUE
  None

SIDE EFFECTS
  The CPU may be put to sleep by this task.
  The power collapse procedure for the apps processor may be initiated.

===========================================================================*/

static void sleepapps_pc_or_halt( void )
{
  /* Somebody voted to sleep or other misc reasons not to sleep */
  boolean    bOKTS = FALSE;

  /* Timer groups have been disabled */
  boolean    bTimerGroupsDisabled = FALSE;

  /* Time (in sclks) until the next scheduled wakeup */
  uint32     uNextWakeupSclks = 0;

  /* Timestamp before and after sleep, in slow-clocks */
  uint32     uSleepSclk;

  /* Used to track the type of sleep that was performed */
  sleep_type sleepType = SLEEP_TYPE_NONE;

  /* Mask interrupts / context changes */
  MSM_INTLOCK();

  bOKTS = sleepapps_check_okts_votes();

  if (bOKTS && sleepapps_allow_pc(&bTimerGroupsDisabled, &uNextWakeupSclks))
  {
    /* Accumulate "active" time into stats, and get the current time-stamp 
       prior to sleeping */
    uSleepSclk = sleep_accumulate_active_timestats();

    sleepapps_pc(uNextWakeupSclks);
    sleepType = SLEEP_TYPE_PC;
  }
  else if (bOKTS && sleepapps_allow_apps_sleep(&bTimerGroupsDisabled, 
                                               &uNextWakeupSclks))
  {
    /* Accumulate "active" time into stats, and get the current time-stamp 
       prior to sleeping */
    uSleepSclk = sleep_accumulate_active_timestats();

    sleepapps_apps_sleep(uNextWakeupSclks);
    sleepType = SLEEP_TYPE_APPS_SLEEP;
  }
  else if (sleepapps_allow_micro_proc_sleep())
  {
    if (bTimerGroupsDisabled)
    {
      /* Re-enable all timers since we will not be entering Apps sleep */
      clk_enable();
      SLEEPAPPS_ADGT_ENABLE();
      bTimerGroupsDisabled = FALSE;
    }
    /* Accumulate "active" time into stats, and get the current time-stamp 
       prior to sleeping */
    uSleepSclk = sleep_accumulate_active_timestats();
    sleepapps_micro_proc_sleep();
    sleepType = SLEEP_TYPE_MICRO;
  }
  else
  {
    /* Accumulate "active" time into stats, and get the current time-stamp 
       prior to sleeping */
    uSleepSclk = sleep_accumulate_active_timestats();
  }

  /* With the CPU halted, we will not reach this point until an unmasked
    interrupt occurs. */

  /* Record the sleep task statistics, but do NOT generate the log packet
     while interrupt are locked, or the handling of the interrupt that
     caused the CPU to resume will be significantly delayed. */
  sleep_accumulate_sleep_timestats(uSleepSclk, sleepType);

  if (bTimerGroupsDisabled)
  {
    clk_enable();
    SLEEPAPPS_ADGT_ENABLE();
  }

  /* Immediately restore interrupts after wakeup.  Any interrupt which
     happened while the uP was asleep is still pending and will be
     handled as soon as interrupts are restored. */
  MSM_INTFREE();

  /* Now that the interrupt which caused the ARM to resume has been
     processed, and the ARM has finally returned to the sleep task, the
     sleep stats log packet with the data that was recorded earlier may be
     emitted. */
  sleeplog_emit_task_diag_logs(LOG_SLEEP_APPS_STATS_C);
} /* sleepapps_pc_or_halt */

#endif /* DUAL_PROC_APPS_SLEEP */


/*===========================================================================

                              EXTERNAL FUNCTIONS

===========================================================================*/

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
  dword unused_param
)
{
  /* Signals returned from REX */
  rex_sigs_type sleep_sigs;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sleep_init();

  tmc_task_start();

  gSleepInfo.task_running = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  gpSleepSmemSleepDisabled =
    smem_alloc (SMEM_SLEEP_POWER_COLLAPSE_DISABLED, sizeof(uint32));

  gSleepInfo.log.task_stats.hdr.size =  sizeof(sleeplog_task_stats_v2);
  gSleepInfo.log.task_stats.hdr.id = SLEEPLOG_ID_TASK_STATS;
  gSleepInfo.log.task_stats.hdr.ver = SLEEPLOG_VERSION_2;
  gSleepInfo.log.task_stats.cl_hdr.max_name_len = SLEEPLOG_CLIENT_LIST_MAX_STR;
  gSleepInfo.log.task_stats.cl_hdr.max_names = SLEEPLOG_CLIENT_LIST_MAX_NUM;

  /* Loop forever */
  for (;;)
  {
    /* Query and clear any signals the sleep task must respond to */
    sleep_sigs = rex_clr_sigs( &sleep_tcb, SLEEP_TASK_SIGS) & SLEEP_TASK_SIGS;

    /* If a task signal was set, we must respond to it */
    if ( sleep_sigs != 0 )
    {
      /* If we're supposed to stop, let's do it */
      if ( (sleep_sigs & SLEEP_STOP_SIG) != 0 )
      {
        /* Process task stop procedure from task controller. */
        gSleepInfo.task_running = FALSE;
        task_stop();
      }

      /* If we're supposed to go offline, let's do it */
      if ( (sleep_sigs & SLEEP_OFFLINE_SIG) != 0 )
      {
        /* Process task offline procedure from task controller. */
        gSleepInfo.task_running = FALSE;
        task_offline();
      }

      /* If a RPT signal was received report an error */
      if ( (sleep_sigs & SLEEP_RPT_TIMER_SIG )!= 0 )
      {
        MSG_ERROR("Invalid signal received ", 0, 0, 0);
      }

    } /* sleep_sigs != 0 */

    /* Otherwise, power down and halt to extend the battery life. */
    else
    {
      /* No other runnable higher priority tasks, and this task has
         nothing to do.  Time to extend battery life by powering down
         as much circuitry as possible, and halting the CPU. */
#ifdef DUAL_PROC_APPS_SLEEP
        if ( (*gpSleepSmemSleepDisabled & SLEEP_DISABLE_PWRC_LEGACY) == 0 )
        {
  #ifndef FEATURE_SLEEP_APPS_DISABLED
          sleepapps_pc_or_halt( );
  #endif /* FEATURE_SLEEP_APPS_DISABLED */
        } /* gpSmemSleepPowerCollapseDisabled = FALSE */
#endif /*  DUAL_PROC_APPS_SLEEP */
    }

  } /* end of infinite loop */

} /* end of sleep_task */

