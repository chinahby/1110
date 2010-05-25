/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       C L O C K   S E R V I C E S

GENERAL DESCRIPTION
  This module contains procedures to manipulate the DMSS system clock
  and perform various time conversions.

  IMPORTANT NOTE: The time calculations in this module are only valid for
  dates and times from 6 Jan 1988, 00:00:00 to 28 Feb 2100, 23:59:59.  This
  is due to the fact that this module assumes that all years evenly divisible
  by 4 are leap years. Unfortunately, century years (e.g., 1900, 2000, 2100)
  must also be evenly divisible by 400 in order to be leap years, and so the
  year 2100 does not qualify as a leap year.

EXTERNALIZED FUNCTIONS
  clk_set_mclk_parms
    Lets CLK services know about the current microprocessor frequency

SPECIAL-PURPOSE EXTERNALIZED FUNCTIONS
  clk_disable
    Turns off the periodic interrupt
  clk_catnap_enable
    Called when the wakeup interrupt happens, to permit clock to turn on
    the periodic interrupt if necessary, and to do sundry time accounting if
    possible.
  clk_unexpected_enable
    Re-enables the periodic interrupt when a catnap is unexpectedly
    terminated early by an interrupt, and does some time accounting.

REGIONAL FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The clock code must be initialized by calling clk_init().

  The functions under the heading "SPECIAL-PURPOSE EXTERNALIZED FUNCTIONS"
  are not general purpose functions. Rather, they represent entry points
  which are invoked from elsewhere in the phone to permit CLOCK to determine
  very accurately how much time has passed when the periodic interrupt has
  been disabled. The primary challenge is dealing with slotted sleep and
  deep sleep. To do that, these functions must be called from very precise
  points in other modules.

  clk_disable permits anything in the phone to shut down the periodic
  interrupt. Unlike these other functions, there is no limit on who can call
  it -- but in practice it is only called just as we are going to sleep.

  clk_catnap_enable must be called out of the catnap wakeup interrupt handler.
  That's because that is the only time when CLOCK is capable of determining
  how many clock ticks were lost in the most recent catnap due to the
  periodic tick interrupt being disabled.

  clk_unexpected_enable must be called out of each and every interrupt handler
  which could conceivably cause us to wake early. (If you're not sure, put
  it in; if it is called unnecessarily it will know and won't do anything.)

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006             by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc6055/drivers/clk/clk.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/07    cr     The 02/05/07 change was completely wrong. In discussion 
                   with Bruce C, we concluded that PAUSE_TIMER writes are not 
                   interruptable. Programming max in pause timer can hold off 
                   all execution for approx. 680us.
02/08/07    cr     Fix error in max_pause_time assignment.
02/05/07    cr     Use maximum PAUSE_TIMER value in clk_pause(). In 
                   discussion with RajeevP, we concluded that programming
                   any value besides max must certainly have been to workaround
                   a bug in which FEATURE_USES_CLK_BUSY_WAIT_NOPS is defined and
                   INTLOCK is taken in clk_pause().
07/28/06   dyc     Have RUMI used clk_pause() for clk_busy_wait()

07/12/05   mpk     For L4 only builds, disable/enable general clock regime
                   in clk_disable() / clk_enable().
05/31/05   ejv     Remove include of dmod.h.
05/25/04   ap      Merged support to use TIME_VU.
03/12/04   ajn     added clk_pause() for clk_busy_wait implementation.
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
01/09/04   ajn     Rex timers moved to Time Services Subsystem, and the 5ms
                   tick is removed for MSM6500+ targets.
08/27/03   ajn     Functionality moved to the Time Service Subsystem
07/07/03   ejv     Mainline FEATURE_ENHANCED_STANDBY_III.
11/05/02   ajn     Added catnap adjustment in milliseconds
11/30/01   dsb     Ported for MSM6050. Removed clk_long_busy_wait.
05/30/01   ajn/dsb Added usr_data_ptr to clock callback timers.  Added
                   clk_reg2 in support of this.
02/27/01   dsb     Fixed reading of symbol combiner in
                   clk_predict_first_wakeup.  Removed #ifndef
                   FEATURE_UP_CLOCK_SWITCHING surrounding the disabling of
                   the general clock regime in clk_disable.  Added
                   INTLOCK and INTFREE to clk_set_mclk_parms.
02/20/01   dsb     Added new MCLK parameter vars to keep track of the
                   dynamically changing MCLK.  CLK services uses these values
                   to calculate appropriate clk_busy_wait times.
                   CLK REGIME services will call clk_set_mclk_parms to notify
                   CLK that the MCLK has changed.  Updated clk_long_busy_wait
                   to handle a dynamic MCLK (as opposed to a statically
                   compiled one).
10/09/00   dsb     Mainlined T_ARM and up to and including T_MSM31.
12/03/99   ajn     Added function for altering catnap lengths.
11/01/99   mvl     Moved TIMETEST code for 1PPS from clk_tick_isr()
                   to clk_uptime_add(). The 1PPS now occurs ones per second
                   instead of every 5ms.
09/03/99   rm/mk   Added FEATURE_UP_CLOCK_SWITCHING support.
10/01/98   scdb    Merge ARM (MSM3000) changes back into common code base.
09/01/98   scdb    Added _cdecl to some functions to save ROM
07/16/98   scdb    clk_read_ms now returns a more accurate number, including
                   fractional ticks
04/17/98   scdb    Removed comment reference to clk_set_baud_clk
03/18/98   scdb    In clk_set_ms, don't permit anyone to set TOD to 1980.
03/02/98   scdb    clk_calculate_backlog, clk_catchup_backlog became local
                   clk_predict_next_wakeup no longer exists.
                   Added clk_disable, clk_catnap_enable, clk_unexpected_enable
                   Implemented coarse granularity accounting for REX timers,
                   to save power and increase standby time.
                   clk_set_ms now sets time correctly even when running after
                   an unexpected wakeup.
                   Incorporated review changes (mostly cosmetic)
10/31/97   scdb    All aspects of maintenance of system time have been
                   completely redesigned. This resulted in extensive changes to
                   this module.
06/17/97   dhh     Replaced target condition by T_MSM2
04/28/97   jjn     Configured for Module(Charon) (T_MD)
11/02/96   jah     Configured for TGP (T_T)
09/18/96   rdh     Added alternate clk_up_time maintenance technique.
06/27/95   jah     Added tramp_queue_call() to clk_tick_handler() instead of
                   direct calls.
06/13/95   jah     Added ISS-2 and CSS-1 support to clk_busy_wait().
07/25/94   jah     Added clk_busy_wait().
04/28/94   jah     Derived from clk.c, broke out clkp, clkjul, and clktmr0.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Common Definitions and types         */
#include "qw.h"         /* Quad Word prototypes                 */
#include "misc.h"       /* Prototype for div4x2()               */
#include "clk.h"        /* Clock Services header                */
#include "clki.h"       /* Clock Services internal header       */
#include "target.h"     /* Target specific definitions          */
#include "rex.h"        /* REX real-time executive services     */
#include "msg.h"        /* Message logging services             */
#include "err.h"        /* Error reporting services             */
#include "tramp.h"      /* Interrupt trampoline service         */
#include "sleep.h"      /* Sleep and some time control services */
#include "clkregim.h"   /* Clock regime controls                */
#include "msm_drv.h"

#ifdef TIMETEST
#include "timetest.h"   /* Timing test definitions              */
#include "rex.h"        /* Access to rex_led_buf                */
#endif

#include "time_svc.h"   /* Compute time spent with 5ms tick off */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*---------------------------------------------------------------------
** The following value is used by clk to keep track of whether the timer
** tick is enabled (and running) or disabled.
**---------------------------------------------------------------------
*/

typedef enum {
  CLK_SLEEP,            /* The phone is going to sleep                  */
  CLK_NORMAL            /* The phone is operating normally              */
} clk_mode_type;

LOCAL clk_mode_type clk_mode = CLK_NORMAL;


LOCAL qword clk_sleep_ms; /* Time when 5ms tick was disabled */


int clk_backlog = 0;
   /* Contains the number of milliseconds of time spent with the periodic
      tick turned off, which hasn't yet been accounted into uptime and
      implicitly into time-of-day. If coarse accounting is not enabled
      for clock callbacks or REX timers, this is also how far off they
      are. This value will never exceed the length of a catnap. */



/*---------------------------------------------------------------------
**                Local Message Threshold
**
** This is the local message threshold for this module.
**---------------------------------------------------------------------
*/

/*
** The following declarations support coarse-granularity accounting on REX
** timers (and on clock callbacks for experimental purposes).
** "Coarse-granularity accounting" means that backlog time isn't
** accounted on catnap boundaries, but is rather deferred until the wakeup at
** the end of a full sleep period.
*/

/*
** clk_deferred_rex_time contains the number of milliseconds of catnap time
** during this entire sleep which has accumulated but has not yet
** been accounted for in the REX timers
*/
dword clk_deferred_rex_time = 0L;


/*
** The largest amount of time, in milliseconds, which can be passed to REX
** in a single call for updating of REX timers. Time intervals greater than
** this must be broken into several calls.
*/
#define CLK_MAX_REX_INCREMENT 65535U

/*
** These two flags enable (TRUE) or disable (FALSE) coarse accounting for
** REX timers and clock callbacks. When coarse timing is disabled, accounting
** is done on catnap boundaries. This improves latency at the expense of
** increased processor time, leading to decreased standby time. When coarse
** timing is enabled, accounting is done on sleep boundaries. This causes
** poorer latency but also decreases processing time and thus increases
** standby time.
** They're variables to facilitate power measurements, since it means
** that the mechanisms can be enabled or disabled using DM pokes.
*/
boolean clk_defer_rex_time = TRUE;
  /* Controls coarse accounting for REX timers. */

/* This is a throw-away location needed to implement the "CLK_NANO_WAIT"
** macro. */

volatile byte clk_nano_location;

/* Current MCLK frequency (in MHz) */
dword clk_mclk_freq_mhz = (BSP_OSCILLATOR_IN + 500)/ 1000; // Round up and set to MHz


/*
** Timer group "clock callbacks" will belong to.  This group is disabled when
** the 5ms tick was disabled, and re-enabled when the 5ms tick was re-enabled.
** Thus timers in the group emulate the historical behaviour of the clock
** callbacks based on the 5ms tick.  [Used in clk_def() macro in clk.h]
*/
timer_group_type clk_callback_group;

extern timer_group_type  rex_timer_group;





#ifndef FEATURE_USE_TIME_VU
/*===========================================================================

FUNCTION CLK_CALCULATE_BACKLOG

DESCRIPTION
  Calculates how far off the phone's uptime is by comparing that to the
  predicted uptime. The error can be caused by more than one catnap!
  The result is stored in the regional variable 'clk_backlog'.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void clk_calculate_backlog(void)
{
  qword milliseconds;

  /*
  ** How much difference is there between when we went to sleep and
  ** when we woke back up?
  */
  time_get_ms( milliseconds );
  qw_sub( milliseconds, milliseconds, clk_sleep_ms);

  clk_backlog = (int) qw_lo( milliseconds );

  if (clk_backlog < 0) {
    clk_backlog = 0;
  }
} /* end of clk_calculate_backlog */

/*===========================================================================

FUNCTION CLK_CATCHUP_BACKLOG

DESCRIPTION
  If there is a stored backlog of time, then this will process that time.
  This includes expiration of clock callbacks and REX timers.

DEPENDENCIES
  This relies upon a flag which has been set by CLK_SAVE_TICKSTATE.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void clk_catchup_backlog(
  boolean last_wake
    /* If true, then we're being called during the wakeup after the last
    ** catnap of a sleep, just before waking up to run normally for a while.
    */
)
{
  /*
  ** If the backlog is zero, there's nothing to do.
  */
  if ( clk_backlog <= 0) {
    return;
  }


  /*
  ** Tell REX about it, too, unless we're using coarse accounting granularity
  */
  if (clk_defer_rex_time) {
    clk_deferred_rex_time += clk_backlog;
  } else {
    rex_tick( clk_backlog );
  }

  /*
  ** If we're waking up from a sleep, then it's time to do coarse granularity
  ** accounting, if necessary.
  */
  if (last_wake) {

    /* Account for REX time which was deferred due to coarse granularity
    ** accounting.
    */
    if (clk_deferred_rex_time > 0) {

      /* If the deferred time is too large to fit in a single call
      ** to REX, tell it a piece at a time.
      */
      while (clk_deferred_rex_time > CLK_MAX_REX_INCREMENT) {
        rex_tick( CLK_MAX_REX_INCREMENT );
        clk_deferred_rex_time -= CLK_MAX_REX_INCREMENT;
      }

      /* And take care of any residue.
      */
      rex_tick( (word)clk_deferred_rex_time );

      clk_deferred_rex_time = 0;
    }
  }

  /*
  ** There's no longer any backlog, so set clk_backlog to show this.
  ** That means clk_backlog can be trusted to always indicate how far off
  ** uptime is, even when uptime is not off at all.
  */
  clk_backlog = 0;

} /* end of clk_catchup_backlog */
#endif /* !FEATURE_USE_TIME_VU */


/*===========================================================================

FUNCTION CLK_DISABLE

DESCRIPTION
  This disables the periodic interrupt. Uptime and Time-of-day freeze,
  REX timers and clock callbacks cease to expire, and power consumption
  decreases markedly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_disable(void)
{
  /* If the clock is not already shut down, then
  */
  if (clk_mode != CLK_SLEEP) {

    #if defined (FEATURE_L4_MSMSLEEP)
#error code not present
    #elif !defined (FEATURE_USE_TIME_VU)
    /* Mask the tick and uninstall the ISR
     */
    tramp_set_isr(TRAMP_TICK_ISR, NULL);

    /* Disable the general clock regime
    */
    #ifdef FEATURE_DUAL_CLK_RGM
    clk_regime_msm_disable( CLK_RGM_GEN_M );
    #else
    clk_regime_disable( CLK_RGM_GEN_M );
    #endif
    
    #endif /* !FEATURE_USE_TIME_VU */

    /* Remember that the clock is now off
     */
    clk_mode = CLK_SLEEP;

    /* Remember time when clock was disabled
    */
    time_get_ms( clk_sleep_ms );

    /* Emulate disabled 5ms tick behaviour by disabling the timer group
    */
    timer_group_disable( &clk_callback_group );
    timer_group_disable( &rex_timer_group );
    timer_group_disable( &timer_null_group );

  }
} /* end of clk_disable */



/*===========================================================================

FUNCTION CLK_ENABLE

DESCRIPTION
  Reenable the periodic tick when any interrupt turns the phone on while the
  phone is asleep.  Some time accounting takes place, REX timers and clock
  callbacks can expire, uptime/TOD are brought up to date.

DEPENDENCIES
  It should be called from interrupt service routine which can
  interrupt sleep, and only from them.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_enable(void)
{

  /* Do nothing unless the clock was off.
  */
  if (clk_mode == CLK_SLEEP) {

    #if defined (FEATURE_L4_MSMSLEEP)
#error code not present
    #elif !defined (FEATURE_USE_TIME_VU)
    /* Figure out how much 'uptime' is off by, and store it.
    */
    clk_calculate_backlog();

    /* Re-enable General clock regime
    */
    #ifdef FEATURE_DUAL_CLK_RGM
    clk_regime_msm_enable( CLK_RGM_GEN_M );
    #else
    clk_regime_enable( CLK_RGM_GEN_M );
    #endif

    /* Install the clock tick ISR and unmask the interrupt
    */
    tramp_set_isr( TRAMP_TICK_ISR, clk_tick_isr );
    #endif /* !FEATURE_USE_TIME_VU */

    /* Emulate enabling of the 5ms tick behaviour by enabling the timer group
    */
    timer_group_enable( &clk_callback_group );
    timer_group_enable( &rex_timer_group );
    timer_group_enable( &timer_null_group );

    /* Remember that the clock is now running.
    */
    clk_mode = CLK_NORMAL;

    #ifndef FEATURE_USE_TIME_VU
    /* Account for the time which was lost due to catnapping.
    */
    clk_catchup_backlog(TRUE);
    #endif /* !FEATURE_USE_TIME_VU */
  }
} /* end of clk_enable */



/*===========================================================================

FUNCTION CLK_SET_MCLK_PARMS

DESCRIPTION
  Accessor function to set the CLK module variables holding the MCLK
  frequency (in MHz).

  This value can be used to calculate MCLK timing.  eg. the PAUSE_TIMER in
  clk_long_busy_wait.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_set_mclk_parms(dword freq_mhz)
{
  INTLOCK();

  clk_mclk_freq_mhz = freq_mhz;

  INTFREE();
}



/*===========================================================================

FUNCTION CLK_PAUSE

DESCRIPTION
  This function is used to implement the clock busy wait, instead of
  expanding the entire clock busy wait code inline.

  Conversion from microseconds to pause time is done in the clk_busy_wait
  macro, to allow the preprocessor to compute the value as a constant, if
  clk_mclk_freq_mhz is a #define constant.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef CLK_PAUSE_TEST
int gMaxPauseTime=150;
#else
#define gMaxPauseTime 150
#endif
void clk_pause(int pause_time_remaining)
{
  int max_pause_time;

#if !defined(T_RUMI) && !defined(T_RUMI_SC2X)
  max_pause_time = CLK_MICRO_SECS_TO_PAUSE_TIMER_VAL(gMaxPauseTime);

  while (pause_time_remaining > 0)
  {
    int pause_time = MIN(max_pause_time, pause_time_remaining);

    HWIO_OUT (PAUSE_TIMER, pause_time);
    pause_time_remaining -= pause_time;
  }
#else
#error code not present
#endif /* T_RUMI */
}

#ifdef FEATURE_USES_CLK_BUSY_WAIT_NOPS
/*===========================================================================

FUNCTION CLK_BUSY_WAIT_NOPS

DESCRIPTION
  Perform a  busy wait using NOPs.  Used when Paus timer cannot be used.

DEPENDENCIES
  Interrupts should be locked by caller

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

INLINE void clk_busy_wait_nops(uint32 us)                   
{                                            
  int wait_remaining = (CLK_MICRO_SECS_TO_NOP_VAL(us))*2/3;   
  int max_pause_time = (CLK_MAX_PAUSE_TIME)*2/3;                    
  while (wait_remaining > 0)                                  
  {                                                           
    int pause_time = MIN(max_pause_time, wait_remaining);     
    INTLOCK();                               
    clk_busy_wait_nop_loops(pause_time);   
    INTFREE();                           
    wait_remaining -= pause_time;                             
  }                                                           
} /* clk_busy_wait_nops */

/*===========================================================================

FUNCTION CLK_PAUSE

DESCRIPTION
  This clock busy wait API. It defaults PAUSE_TIMER busy wait in bootup. And
  use busy wait noops after bootup to avoid ARM locking the SYS AHB.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint32 uses_clk_buys_wait_nops = 0;
void clk_busy_wait(uint32 us)
{
  if (HW_MSM6800_HAS_R1_V0)                                               
  {                                                                   
    clk_busy_wait_nops(us);                                           
  }                                                                   
  else                                                                
  {                                                                   
    clk_pause( CLK_MICRO_SECS_TO_PAUSE_TIMER_VAL(us) );               
  }                                                                   
} /* clk_busy_wait */
#endif /* FEATURE_USES_CLK_BUSY_WAIT_NOPS */


