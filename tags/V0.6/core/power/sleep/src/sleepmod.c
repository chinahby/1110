/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S L E E P   M O D E M    P R O C E S S O R    C O M M O N

GENERAL DESCRIPTION
  This file containes the modem/single processor specific functions 
  used to support sleep.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

                           Copyright (c) 2006-2009
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleepmod.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/09   ajf     Check global sleep restrictions (Trace32, NV) when deciding
                   whether or not to do modem halt.
03/05/09   taw     Added a macro for maoint_get_pending_ints().
02/10/09   taw     Record the pending MAO interrupts as part of the TCXO_END
                   log message.
09/16/08   clm     Code clean up.
08/18/08   taw     Add the declaration of sleep_profile_mask.
06/10/08   clm     Re-write of sleep code.
04/15/08   taw     If sleep controller interrupt already fired, don't do
                   clk_disable and clk_enable.
03/31/08   taw     Made sleep_get_tcxo_shutdown_count() return the right thing.
03/21/08   ddh     Added calls to dem_tcxo_on
03/20/08   jfl     Added support for VDD min restrictions
                   and reverted SLEEP_UIM_OKTS_SIG removal
03/13/08   jfl     Removed SLEEP_UIM_OKTS_SIG
03/13/08   jfl     Removed SLEEP_HS_OKTS_SIG
11/03/07   bjs     Added tramp_clear_interrupt after heartbeat timer was 
                   stopped.
11/27/07   cab     Removed SLEEP_HDR_OKTS_SIG.
11/13/07   jyw     Added the power strobe event id.
10/29/07   cab     Fix old sleep logging
10/16/07   cab     Fix sleep logging for single proc targets
10/04/07   taw     Prevent sleep if sleep.allow_sleep is not set; call
		           sleep_init() before entering the main loop of sleep_task().
08/14/07   ebb     Changed sleep_log_event to once again log only the mask.
                   Name resolution is now done in smem_log.pl.
07/26/07   cab     Updated L4 header file inclusion for quartz 1.3.5
07/11/07   ebb     Updated sleep_power_down_and_halt to log new sleep vote
                   names.
07/06/07   cab     Removed SLEEP_CAMERA_OKTS_SIG
06/25/07   ebb     Added sleepmemlog.c to handle local/smem logging.
06/13/07   ebb     Merged in msm7500 orphans to bring tip to 7500I
06/12/07   ebb     Removed SLEEP_VOC_OKTS_SIG, SLEEP_SND_OKTS_SIG, 
                   SLEEP_VS_OKTS_SIG
05/14/07   cab     Use MSM_INTLOCK/MSM_INTFREE on supporting targets
05/03/07   cab     Removed SLEEP_RXC_OKTS_SIG
05/02/07   cab     Removed SLEEP_SRCH_OKTS_SIG
05/01/07   cab     Changed qword to uint64
04/23/07   cab     Updated logging to include new sleep voters
04/16/07   cab     Mainlined CLK_REGIME_UP_SLEEP__MCLK_HCLK, removed MPD ref's
04/10/07   cab     Removed SLEEP_HDRDEC_OKTS_SIG,removed LOG_SLEEP_STATS_C cond
03/20/07   cab     Change mc.h reference to tmc.h to allow for UMTS builds
03/06/07   gfr     Prevent TCXO shutdown when MPM is in reset.
01/08/07   cab     Added L4 progressive boot fixes; featurized DEM LOG EVENTS
12/06/06   cab     Added check for RF sleep before temp compensation
11/27/06   cab     Removed SLEEP_UI_OKTS_SIG
11/07/06   ddh     Removed SLEEP_MDDI_OKTS_SIG and SLEEP_MDP_OKTS_SIG. Also
                   corrected when hclk and mclk restrictions are set for
                   clients using the new sleep voting mechanism.
11/03/06   cab     Featurized rf calls for exclusion on 7200 target
10/30/06   jhs     Removed SLEEP_KYPD_OKTS_SIG.
10/27/06   cab     Added FEATURE_SLEEP_MODEM_DISABLED
10/16/06   cab     Removed sleep mode functionality for GSM 
10/16/06   ddh     Corrected logging for new style sleep voters so as not to 
                   over run the allocated packet size.
10/13/06   ddh     Removed SLEEP_BT_OKTS_SIG 
10/09/06   ddh     Removed SLEEP_APPS_OKTS_SIG after use was replaced with 
                   new voting mechanism
10/03/06   ddh     Removed disabling of TCXO for 7500 builds.
09/29/06   ddh     Temporarily disable TCXO until can solve issue with USB 
                   interrupt firing after TCXO exit when no USB is attached 
                   resulting in apps power collapse being disabled. This is a
                   7500 specific change that will be removed soon.
09/13/06   cab     Shorten tcxo warmup time for 6800B
08/29/06   ddh     Adding check of MAO warmup when calculating available time
                   to sleep
08/24/06   cab     Fix compiler issues for 7500 target
08/14/06   cab     Added L4 progressive boot to sleep task
08/04/06   cab     Add mclk and hclk restrictions to new sleep interface
07/28/06   jyw     Added hdr power probes.
07/25/06  jwob     Fix: compute n_client_strings for cl_hdr
06/28/06   TMR     Added FEATURE_NO_USB_HW_ON_PROCESSOR
06/16/06  jwob     Version 2 of sleep log packets to include results of
                   function interface to okts voting mechanism
06/06/06   ddh     Created modem specific sleep file by moving modem specific 
                   functions from sleep.c

===========================================================================*/

/*=========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "sleep.h"
#include "sleepi.h"
#include "sleepmodi.h"
#include "sleepmod_keypressi.h"
#include "dog.h"
#include "tmc.h"
#include "tramp.h"
#include "sclk.h"
#include "msg.h"
#include "hw.h"
#include "clkregim.h"
#include "sleeplog.h"
#include "sleepctl.h"
#include "log.h"
#include "sleep_profile.h"
#include GPIO_H

#ifdef FEATURE_MDM_REQUIRE_NO_ARM_HALT
  #include "srchgps.h"
#endif /* FEATURE_MDM_REQUIRE_NO_ARM_HALT */

#ifdef FEATURE_MULTIMODE_RFAPI
  #include "rfm.h"
#else /* !FEATURE_MULTIMODE_RFAPI */
  #include "rf.h"
#endif /* FEATURE_MULTIMODE_RFAPI */

#ifdef T_MSM7500
#error code not present
#endif /* T_MSM7500 */

#if (defined(FEATURE_POWER_COLLAPSE) || defined(SLEEPMOD_HAS_MPM_HW))
  #include "mao.h"
  #include "maoint.h"
#endif /* FEATURE_POWER_COLLAPSE || SLEEPMOD_HAS_MPM_HW */

#if defined (FEATURE_L4_VST_VOTE) || defined (FEATURE_L4_LINUX_VOTE)
#error code not present
#endif /* FEATURE_L4_VST_VOTE || FEATURE_L4_LINUX_VOTE */

#ifdef FEATURE_HDRSRCH_POWER_PROFILE
#error code not present
#endif /* FEATURE_HDRSRCH_POWER_PROFILE  */

#if defined( FEATURE_L4 ) 
#error code not present
#endif /* FEATURE_L4 */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  Sleep global data
--------------------------------------------------------------------------*/

/* Used to indicate from BT if TCXO must be kept on or not */
boolean sleep_bt_clock_rgm = FALSE;

/* Used to check whether or not TCXO has already been turned back on
   after sleep.  Helps avoid wasting time by waiting for it to turn
   on more than once. */
boolean gbSleepTcxoOn = TRUE;

uint32 sleep_profile_mask = 0xFFFFE7FF;

/*===========================================================================

FUNCTION SLEEPMOD_DISABLE_TIMER_GROUP

DESCRIPTION
  This disables timer groups that can not wake us up from tcxo shutdown.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void sleepmod_disable_timer_group(void)
{
  MSG_LOW("Sleeping",0,0,0);

  /* Disable the clock tick before setting up powerdown,
     as any non-masked interrupt causes the CPU to disable
     the power saving modes. */
  clk_disable();

  /* Now that we have disabled the OS Heartbeat timer, we are clearing
     any potential interrupts that have happened since entering the
     sleep task. */
  tramp_clear_interrupt(TRAMP_TICK_ISR);

  /* Allow keypresses to wake up the CPU, and turn the 5ms tick back
     on, in order to debounce keypresses. */
  sleep_install_keypress_isr();

  /* When the 5ms tick is off, we can halt the CPU for long durations.
     Ensure the autokicker is keeping the dog from biting. */
  dog_autokick( TRUE );
} /* sleepmod_disable_timer_group */

/*===========================================================================

FUNCTION SLEEPMOD_ENABLE_TIMER_GROUP

DESCRIPTION
  This re-enables timer groups disabled by sleepmod_disable_timer_group.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void sleepmod_enable_timer_group(void)
{
  /* If we have been letting the autokicker keep the dog quiet, then
     we must now stop the autokicker. */
  dog_autokick( FALSE );

  /* Enable the timers, now that TCXO is back on again. */
  clk_enable();
} /* sleepmod_enable_timer_group */

/*===========================================================================

FUNCTION SLEEPMOD_CALCULATE_NEXT_WAKEUP_SCLK

DESCRIPTION
  Returns the minimum value of next wakeup sclk, timer expire sclk, or Linux
  VST vote to wakeup.

DEPENDENCIES
  None.

RETURN VALUE
  Minimum amount of time until next wakeup event.

SIDE EFFECTS
  None

===========================================================================*/

static uint32 sleepmod_caculate_next_wakeup_sclk
(
  /* Use next expiring timers when calculating next wakeup sclk */
  boolean bUseTimers,

  /* Next sleep ctl wakeup in sclks */
  uint32 *puNextSleepCtlSclk,

  /* Next Timer wakeup in sclks */
  uint32 *puNextTimerSclk,

  /* Next Alt wakeup in sclks */
  uint32 *puNextAltSclk
)
{
  /* Time (in sclks) until the next scheduled timer expiry */
  uint32  uNextTimerSclk = 0xFFFFFFFF; 

  /* The closest future event that will limit tcxo shutdown duration */
  uint32  uMinSclk = 0;

  /* Time (in sclks) until the next scheduled alt pd event */
  uint32  uNextAltpdSclk = 0xFFFFFFFF;

  
  /* Time (in sclks) until the next sleep controller wakeup */
  uMinSclk = sleepctl_get_next_wakeup();
  *puNextSleepCtlSclk = uMinSclk;

  /* Time (in sclks) until the next scheduled timer expiry */
  uNextTimerSclk  = timer_get_sclk_till_expiry();

  if (bUseTimers)
  {
    /* Determine earlier wakeup point ... sleep controller or timer */
    uMinSclk = MIN(uNextTimerSclk, uMinSclk);
  }

  /* Get L4Linux VST wakeup time  before turning off the 5ms L4 Kernel tick */
  uNextAltpdSclk = SLEEEPMOD_GET_VST_NEXT_WAKEUP();

  uMinSclk = MIN(uMinSclk, uNextAltpdSclk);

  *puNextTimerSclk = uNextTimerSclk;
  *puNextAltSclk   = uNextAltpdSclk;

  return uMinSclk;
} /* sleepmod_caculate_next_wakeup_sclk */

/*===========================================================================

FUNCTION SLEEPMOD_OK_TCXO_SHUTDOWN

DESCRIPTION
  Check votes and other misc reasons not to perform TCXO shutdown.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  okay to perform TCXO shutdown.
  FALSE not okay to perform TCXO shutdown.

SIDE EFFECTS
  None

===========================================================================*/

static boolean sleepmod_ok_tcxo_shutdown(void)
{
  boolean        bRetVal = FALSE;
  rex_sigs_type  not_okts;

  do
  {
    if (!gSleepInfo.task_running || !gSleepInfo.allow_sleep )
    {
      /* Sleep task has been disabled, or the global variable to prevent
         sleep is set.  Not okay to sleep. */
      break;
    }

    /*---------------------------------------------------------------------*\
    |* Are any old voters not okay to sleep?                               *|
    \*---------------------------------------------------------------------*/
    not_okts = SLEEP_OK_MASK & ~rex_get_sigs( &sleep_tcb );
    if (not_okts)
    {
      /* Record who did not vote for sleep, if anyone */
      gSleepInfo.log.task_stats.not_okts = not_okts;
      sleep_log_event( NO_SLEEP_OLD, not_okts, 0, 0 );
      break;
    }

    /* snapshot the not okts votes and num voters for logging later */
    gSleepInfo.log.curr_not_okts_m = gSleepInfo.curr_not_okts_mask;
    gSleepInfo.log.num_clients = gSleepInfo.num_clients;

    /*---------------------------------------------------------------------*\
    |* Are any new voters not okay to sleep?                               *|
    \*---------------------------------------------------------------------*/
    if ( gSleepInfo.curr_not_okts_mask )
    {
      /* shoe new clients into the old sleep voting mask */
      not_okts |= 0x00008000;

      /* Record who did not vote for sleep, if anyone */
      gSleepInfo.log.task_stats.not_okts = not_okts;

      /* Log the mask of the no sleep voters */
      sleep_log_event( NO_SLEEP_NEW, MODEM_SLEEP, 
                       (uint32)(gSleepInfo.curr_not_okts_mask >> 32), 
                       (uint32)gSleepInfo.curr_not_okts_mask);
      break;
    }

    /*----------------------------------------------------------------------*\
    |* Are there any other conditions that would make it not okay to sleep? *|
    \*----------------------------------------------------------------------*/
    if (sleep_bt_clock_rgm)
    {
      /* Not okay to sleep */
      break;
    } /* end sleep_bt_clock_rgm */

    if ( sleep_uart_clock_rgm )
    {
      /* But we cannot power down TCXO if the UART is using it */
      sleep_log_event(MOD_UART_CLOCK, 0, 0, 0 );
      break;
    } /* end sleep_uart_clock_rgm */

    /* If L4Linux voted against MSMSleep
    ** disable MSMSleep
    */
    if (!SLEEPMOD_L4_LINUX_VOTE_MSLEEP_OTKS())
    {
      break;
    }

    /* If the MPM is broken we cannot do TCXO shutdown */
    if (SLEEPMOD_HW_HAS_BROKEN_MPM())
    {
      break;
    }

    bRetVal = TRUE;
  } while (FALSE);
  return bRetVal;
} /* sleepmod_ok_tcxo_shutdown */

/*===========================================================================

FUNCTION SLEEPMOD_ALLOW_TCXO_SHUTDOWN

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

static boolean sleepmod_allow_tcxo_shutdown
(
  /* Input and Output. Notify function if timer groups have been disabled.
     Notify caller if timer groups were disabled inside this function */
  boolean *pBDisabledTimerGroups, 
  /* Return Min sclk's until next wakeup */
  uint32  *puMinSclk
)
{
  boolean bRetVal = FALSE;

  boolean bDisabledTimerGroups = FALSE;

  /* The closest future event that will limit tcxo shutdown duration */
  uint32  uMinSclk = 0; 

  /* Next sleep ctl wakeup in sclks */
  uint32 uNextSleepCtlSclk = 0;

  /* Next Timer wakeup in sclks */
  uint32 uNextTimerSclk = 0;

  /* Next Alt wakeup in sclks */
  uint32 uNextAltSclk = 0;

  do
  {
    /*---------------------------------------------------------------------*\
    |* First Check votes and other misc reasons not to allow TCXO sleep    *|
    \*---------------------------------------------------------------------*/
    if (!sleepmod_ok_tcxo_shutdown())
    {
      /* Somebody votes against TCXO shutdown or there are other reasons not to
          do it */
      break;
    }

    /*-----------------------------------------------------------------------*\
    |* Do we have enough time before the next sleepctl wakeup or linux vote? *|
    \*-----------------------------------------------------------------------*/
    uMinSclk = sleepmod_caculate_next_wakeup_sclk(FALSE, &uNextSleepCtlSclk,
                                                  &uNextTimerSclk,
                                                  &uNextAltSclk);

    if (SLEEPMOD_NO_TIME_FOR_TCXO(uMinSclk))
    {
      /* Insufficient time to sleep */
      sleep_log_event( INSUF_TIME, uNextSleepCtlSclk, uNextTimerSclk, 
                       uNextAltSclk );
      break;
    }

    /*---------------------------------------------------------------------*\
    |* Disable timer group and set dog to autokick                         *|
    \*---------------------------------------------------------------------*/    
    sleepmod_disable_timer_group();
    bDisabledTimerGroups = TRUE;

    /*---------------------------------------------------------------------*\
    |* Make sure we still have time to shutdown, since disable timer       *|
    |* groups takes awhile to perform                                      *|
    \*---------------------------------------------------------------------*/
    uMinSclk = sleepmod_caculate_next_wakeup_sclk(TRUE, &uNextSleepCtlSclk,
                                                  &uNextTimerSclk,
                                                  &uNextAltSclk);

    if (SLEEPMOD_NO_TIME_FOR_TCXO(uMinSclk))
    {
      /* Insufficient time to sleep */
      sleep_log_event( INSUF_TIME, uNextSleepCtlSclk, uNextTimerSclk, 
                       uNextAltSclk );
      break;
    }

    bRetVal = TRUE;
  } while (FALSE);

  *puMinSclk   = uMinSclk;
  *pBDisabledTimerGroups = bDisabledTimerGroups;
  return bRetVal;
} /* sleepmod_allow_tcxo_shutdown */

/*===========================================================================

FUNCTION SLEEPMOD_TCXO_SHUTDOWN

DESCRIPTION
  Perform TCXO shutdown.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void sleepmod_tcxo_shutdown
(
  /* Number of slcks before next wakeup */
  uint32 uNextWakeupSclk
)
{
  /* Control value for vdd minimization */
  sleep_vdd_min_restrict_type     vdd_min_restriction = VDD_MIN_ALLOWED;

  /* check vdd min restriction */
  if (gSleepInfo.vdd_min_restriction_mask != 0)
  {
    vdd_min_restriction = VDD_MIN_NOT_ALLOWED;
  }

  /* Wakeup early, to allow for TCXO warmup & timer expiry */
  SLEEPMOD_TIMER_SET_WAKEUP( uNextWakeupSclk - TCXO_TURN_ON_SCLK );

  SLEEP_PROFILE( TCXO_OFF );
  SLEEPMOD_HDR_PWR_PROFILE_STRB_TOGGLE();
  sleep_log_event( MOD_ENTER_TCXO, uNextWakeupSclk, 0, 0 );

  SLEEPMOD_DEM_TCXO_OFF();
  SLEEPMOD_TCXO_SHUTDOWN(uNextWakeupSclk, vdd_min_restriction);
  SLEEPMOD_DEM_TCXO_ON();

  sleep_log_event( MOD_TCXO_END, TCXO_TURN_ON_SCLK,
                   SLEEPMOD_MAOINT_GET_PENDING_INTS(), 0 );

  SLEEPMOD_HDR_PWR_PROFILE_STRB_ENABLE();

  SLEEP_PROFILE( TCXO_ON );
} /* sleepmod_tcxo_shutdown */

/*===========================================================================

FUNCTION SLEEPMOD_ALLOW_MICRO_PROC_SLEEP

DESCRIPTION
  Check to see if we have enough time to perform micro proc sleep.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  okay to perform micro proc sleep.
  FALSE not okay to perform micro proc sleep.

SIDE EFFECTS
  None.

===========================================================================*/

static boolean sleepmod_allow_micro_proc_sleep(void)
{
  boolean bRetVal = FALSE;

  do
  {
    /* Check sleep restrictions */
    if (gSleepRestrictions.disable_modem_halt)
    {
      break;
    }

    if ((gSleepInfo.curr_not_okts_mask & gSleepInfo.swfi_restriction_mask) 
        != 0)
    {
      /* If both not okts and swfi restriction mask have matching active bits
         it's not okay to micro proc sleep. */
      break;
    }

    /* L4Linux vote for CPUSleep */
    if (!SLEEPMOD_L4_LINUX_VOTE_USLEEP_OTKS())
    {
      break;
    }

    /* If there are any pending interrupts, don't bother performing
       micro proc sleep. */
    if (tramp_is_any_interrupt_pending())
    {
      break;
    }

    bRetVal = TRUE;
  } while (FALSE);

  return bRetVal;
} /* sleepmod_allow_micro_proc_sleep */

/*===========================================================================

FUNCTION SLEEPMOD_MICRO_PROC_SLEEP

DESCRIPTION
  Perform micro processor sleep.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void sleepmod_micro_proc_sleep(void)
{
  /* Control value for MCLK sleep state */
  clkrgm_mclk_sleep_type  mclk_sleep = CLKRGM_MCLK_OFF;

  /* Control value for HCLK sleep state */
  clkrgm_hclk_sleep_type  hclk_sleep = CLKRGM_HCLK_OFF;

  uint32                  i;

  /* Process the hclk and mclk restricitons */
  if (gSleepInfo.hclk_restriction_mask || gSleepInfo.mclk_restriction_mask)
  {
    for (i = 0; i < gSleepInfo.num_clients; i++)
    {
      if (gSleepInfo.client_info[i].okts == FALSE)
      {
        /* set hclk restriction to the highest voted restriction */
        if (gSleepInfo.client_info[i].hclk_restriction > hclk_sleep)
        {
          hclk_sleep = gSleepInfo.client_info[i].hclk_restriction;
        }

        /* set mclk restriction to the highest voted restriction */
        if (gSleepInfo.client_info[i].mclk_restriction > mclk_sleep)
        {
          mclk_sleep = gSleepInfo.client_info[i].mclk_restriction;
        }
      } /* end sleep.client_info[i].okts == FALSE */
    } /* end loop through clients */
  }

  if ( SLEEPMOD_MDM_NEEDS_NO_ARM_HALT() )
  {
    /* Let the clock services know that MDM requires the MCLK */
    mclk_sleep = CLKRGM_MCLK_ON;
  }

  /* sleep timeline profiling output if enabled */
  SLEEP_PROFILE( ARM_HALT );

  /* Halt the uP.  ARM will exit sleep on the next interrupt.
     Side Effects: Interrupt latency is increased by ~ 52 usec */
  clk_regime_uP_sleep( mclk_sleep, hclk_sleep );

  /* sleep timeline profiling output if enabled */
  SLEEP_PROFILE( ARM_ON );
} /* sleepmod_micro_proc_sleep */

/*===========================================================================

FUNCTION SLEEP_POWER_DOWN_AND_HALT

DESCRIPTION
  This procedure looks for a consensus that the TCXO shutdown can occur.
  Lacking a consensus, it still halts the processor, but in "idle" mode
  in which any normal interrupt will restart processing with minimal delay.

DEPENDENCIES
  Called only from sleep task

RETURN VALUE
  None

SIDE EFFECTS
  The CPU may be put to sleep by this task.
  TCXO may be shut down
  The 5ms tick may be disabled.

===========================================================================*/

void sleep_power_down_and_halt( void )
{
  /* Flag to indicate if TCXO can be turned off */
  boolean       bPowerDownTcxo = FALSE; 

  /* Flag to indicate if Timer Groups were disabled */
  boolean       bDisabledTimerGroups = FALSE;

  /* Number of Sclk's before the next wake up event */
  uint32        uMinSclk = 0;

  /* Value of timetick_get_safe() prior to sleeping */
  uint32        uSleepSclk = 0;

  /* Used to the type of sleep that was performed */
  sleep_type    sleepType = SLEEP_TYPE_NONE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Mask interrupts / context changes */
  SLEEPMOD_INTLOCK();

  /* sleep timeline profiling output if enabled */
  SLEEP_PROFILE( SLEEPTASK_SLEEP );

  bPowerDownTcxo = sleepmod_allow_tcxo_shutdown(&bDisabledTimerGroups, 
                                                &uMinSclk);

  /* Accumulate "active" time into stats, and get the current time-stamp prior
     to sleeping */
  uSleepSclk = sleep_accumulate_active_timestats();

  if (bPowerDownTcxo)
  {
    sleepType =  SLEEP_TYPE_TCXO_SHUTDOWN;
    sleepmod_tcxo_shutdown(uMinSclk);
  }
  else if (sleepmod_allow_micro_proc_sleep())
  {
    sleepType =  SLEEP_TYPE_MICRO;
    if (bDisabledTimerGroups)
    {
      sleepmod_enable_timer_group();
    }
    bDisabledTimerGroups = FALSE;
    sleepmod_micro_proc_sleep();
  }

  /* With the CPU halted, we will not reach this point until an unmasked
     interrupt occurs. */

  /* Record the sleep task statistics, but do NOT generate the log packet
     while interrupt are locked, or the handling of the interrupt that
     caused the CPU to resume will be significantly delayed. */
  sleep_accumulate_sleep_timestats(uSleepSclk, sleepType);

  /* If general purpose timers were disabled, and the autokicker ran during
     the CPU halt, disable the autokicker and re-enable the timers. */
  if ( bDisabledTimerGroups )
  {
    sleepmod_enable_timer_group();
  }

  /* sleep timeline profiling output if enabled */
  SLEEP_PROFILE( SLEEPTASK_WAKEUP );
    
  /* Immediately restore interrupts after wakeup.  Any interrupt which
     happened while the uP was asleep is still pending and will be
     handled as soon as interrupts are restored. */
  SLEEPMOD_INTFREE();

  if ( bPowerDownTcxo )
  {
    SLEEPMOD_MAOINT_DISABLE_WAKEUP_INTS();
  }

  /* Now that the interrupt which caused the ARM to resume has been
     processed, and the ARM has finally returned to the sleep task, the
     sleep stats log packet with the data that was recorded earlier may be
     emitted. */
  sleeplog_emit_task_diag_logs(LOG_SLEEP_STATS_C);
} /* sleep_power_down_and_halt( ) */

/*===========================================================================

FUNCTION SLEEP_GET_TCXO_SHUTDOWN_COUNT

DESCRIPTION
  Returns the number of times the TCXO has been shutdown since the phone
  was turned on.

DEPENDENCIES
  None

SIDE EFFECTS
  None

===========================================================================*/

uint32 sleep_get_tcxo_shutdown_count( void )
{
  return ( gSleepInfo.log.task_stats.n_tcxo_off );
} /* sleep_get_tcxo_shutdown_count */

