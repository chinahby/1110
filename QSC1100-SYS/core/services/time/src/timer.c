/*=============================================================================

                  T I M E R   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS

  timer_init()
    Initializes the timer subsytem.  Call only from time_init( ).


EXTERNALIZED FUNCTIONS

  timer_def(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.
    Timers may also be staticly declared via the TIMER_DEF() macro

  timer_def2(timer, group, func, data)
    Defines and initializes a timer, with the clock callback semantics.
    Timers may also be staticly declared via the TIMER_DEF2() macro

  timer_set(timer, ticks, reload, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  timer_get(timer, unit)
    Get number of ticks before timer expires

  timer_pause(timer)
    Suspends an active timer

  timer_resume(timer)
    Resumes a previously "paused" active timer.

  timer_clr(timer, unit)
    Stops an active timer

  timer_clr_task_timers(tcb)
    Stops all active timers which belong to the specified task.

  timer_group_enable(timer_group)
    Enables a timer group.  Timers in the timer group which expired during the
    diabled period will have their expiry processing executed.

  timer_group_disable(timer_group)
    Disables a timer group.  Timers in the timer group will continue to count
    down, but they will not expire.


INITIALIZATION AND SEQUENCING REQUIREMENTS

    timer_init() must be called once before any other timer functions.

    timer_def() must be called, or the TIMER_DEF( ) macro used, for each
      timer, before other timer functions are used with that timer.

    A valid sclk estimate is needed before timers can be set.


Copyright (c) 2003 - 2007 by QUALCOMM Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/timer.c#43 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/20/07   gfr     Added missing msm.h header file.
04/19/07   gfr     Support for timer tracing.
01/12/07   trc     Increase MIN_REARM_SCLK from 6 to 9 slow clock cycles
12/04/06   trc     Bump up the # of timer bins from 64 to 128
10/11/06   trc     Reset match_value and set_value when sclk_delta is set
                   coming out of power collapse since they are no longer valid.
04/10/06   ajn     Prevent rounding nudge from creating a negative runtime.
04/07/06   gfr     Remove unneeded header file (msm_drv.h)
03/01/06   kap     Only unbin when timer->bin field is non-null
02/22/06   ajn     Add offset btw modem & MPM sclk counter for power collapse
02/10/06   kap/ajn Added Timer Optimization Changes (double linking/binning)
12/20/05   ejv     Added T_QSC60X0.
11/03/05   ajn     Zero-initialize the timer.cache fields in timer_define()
10/27/05   ajn     Always set set_time in timer_set_next_interrupt().
08/12/05   jhs     Moved apps processor to its own caching assembly function
                   for writing to MATCH_VAL.
08/05/05   ajn     MATCH_VAL written from caching assembly function.
07/29/05   ajn     Repeat MATCH_VAL writes until the write latches.
07/22/05   ajn     Don't write new MATCH_VAL until last write has latched.
07/06/05   ajn     Added timer_set_modem_app_sclk_offset (7500 APPS)
06/24/05   ajn     Include partial 7500 support
06/09/05   ajn     Corrected non-timer_task initialization.
05/19/05   ajn     Timer op's don't signal task or call process_active_timers.
05/09/05   ajn     Don't write to TIMETICK_MATCH_VAL if value is unchanged.
                   Fixed O(N^2) list insertion in timer_group_enable().
05/06/05   ajn     Cache time/reload values to avoid recalculating if unchanged
03/29/05   ajn     Replaced timer_ptr with timer_ptr_type, for L4 project.
03/16/05   ajn     Update next timer expiry interrupt w/o processing timers
                   when timers are removed from the active list.
03/16/05   ajn     Add ms rounding to following expiry in repeating timers.
12/06/04   lcc     In timer_process_active_timers, added reading of timetick
                   count to make sure it's non-zero before existing.  This is
                   needed for QUASar's timetick hw problem.
10/20/04   ajn     Moved timer expiry out of ISR context into a Timer task.
09/28/04   ajn     Catch use of timers w/o call to timer_define().
09/27/04   ajn     Force timer in the NULL timer group into an actual group.
08/30/04   ajn     Added "get next interrupt" and "interrupt early" functions
                   for TCXO shutdown.
07/06/04   ajn     timer_define() ERR's (not ERR_FATAL's) on active timer.
06/23/04   ajn     Corrected re-entrancy when processing active timers.
01/29/04   ajn     Corrected possible NULL pointer dereference.
01/09/04   ajn     Optimized the expiry-time advance of auto-reloading timers.
11/14/03   ajn     Added sclk timestamps for profiling timer_isr latency.
11/11/03   ajn     Added checks to detect timer_def( ) calls on active timers
10/03/03   ajn     Call added to process_active_timers() in group_disable, to
                   ensure next active timer's interrupt occurs at proper time.
08/11/03   ajn     Added timer_pause/timer_resume for rex_set_timer replacement
08/08/03   ajn     Updated for coding standards
08/06/03   ajn     Added timer_def2(), for clk_reg() replacement
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.c
07/17/03   ajn     File created.

=============================================================================*/



/*=============================================================================


  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  
  All comparisons in this module are done with respect to a moving origin,
  or "zero" point, to handle wrap-around of the 32-bit slow clock counter.
  
  For example, the conditional statement "if ( now >= expiry )" is wrong,
  since "now" could be 0xFFFFFF00, and "expiry" could be "0x00000100", ...
  "0x200" counts in the "future".
  
  The correct comparison would be "if ( now - zero >= expiry - zero )".
  This compares the current time (measured from some point in the past) to
  the expiry time (measured with respect to the same point in the past).
  If the "zero" origin was "0xFFFFFE00", with the above values, "now - zero"
  would be "0x100" and "expiry - zero" would be "0x300", making the entire
  expression FALSE; the expiry point has not been reached.

  If the expression "now - zero >= expiry - zero" returns TRUE, then it is
  safe to subtract "now - expiry" to determine how much time has passed since
  the expiry point.  If "now - zero >= expiry - zero" returns FALSE, the
  reverse subtraction is valid: "expiry - now" indicates how much time
  remains until the expiry point.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "timer.h"
#include "tramp.h"
#include "sclk.h"
#include "err.h"
#include "assert.h"

#ifdef FEATURE_TIMER_TASK
#include "task.h"
#include "tmc.h"
#endif /* FEATURE_TIMER_TASK */

#include "clk.h"
#include "msm.h"


/*=============================================================================

                             CONFIGURATION

=============================================================================*/


/*-----------------------------------------------------------------------------
 Select edge-triggered timer interrupt, if supported
-----------------------------------------------------------------------------*/

#if defined( T_MSM7500 ) || defined( T_QSC60X0 )


  #define TIMETICK_INT_EDGE_TRIGGERED

  /* All interrupts are already edge-triggered */
  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() /* No-op */


#elif defined( T_MSM6700 )


  #define TIMETICK_INT_EDGE_TRIGGERED

  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() \
    HWIO_INT_DET_REG_1_OUTM( \
      HWIO_INT_DET_REG_1_SLEEP_TIME_TICK_INT_BMSK, \
      HWIO_INT_DET_REG_1_SLEEP_TIME_TICK_INT_BMSK )


#elif defined( T_MSM6550 )


  #define TIMETICK_INT_EDGE_TRIGGERED

  /*lint -emacro(570,TIMETICK_INT_EDGE_TRIGGERED_INIT) */
  #define TIMETICK_INT_EDGE_TRIGGERED_INIT() \
    HWIO_INT_DETECT_CTL_0_OUTM( \
      HWIO_INT_DETECT_CTL_0_SLP_XTAL_TIMETICK_INT_BMSK, \
      HWIO_INT_DETECT_CTL_0_SLP_XTAL_TIMETICK_INT_BMSK )

#endif /* T_MSMxxxx */


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


#ifdef FEATURE_TIMER_TASK
/*-----------------------------------------------------------------------------
  Signals used by the timer task.
-----------------------------------------------------------------------------*/

/* Signal for a timer expiring */
#define TIMER_EXPIRY_SIG          0x00000001

/* Signal for a change in the active timer list */
#define TIMER_CHANGE_SIG          0x00000002


/* Signals which must cause the active timer list to be processed */
#define TIMER_PROCESS_SIGS        (TIMER_EXPIRY_SIG | TIMER_CHANGE_SIG)

/* Signals which must be processed to be proper citizens */
#define TIMER_TASK_SIGS           (TASK_STOP_SIG  | TASK_OFFLINE_SIG )
                                   
#endif /* FEATURE_TIMER_TASK */


/*-----------------------------------------------------------------------------
  Constants used by the timer task
-----------------------------------------------------------------------------*/

/* Reading the current time, and programming the timetick match value takes
   a finite period of time.  Ensure the match value is at least this many
   slow clocks after the value that was just read.  Note that this value may
   vary based upon hardware, but for simplicity we just use the maximum
   necessary value. */
#define  MIN_REARM_SCLK           9


/*-----------------------------------------------------------------------------
  Timer Bin Configuration
   - Modify these values to improve binning performance
-----------------------------------------------------------------------------*/

/* Number of bins available. This must be large enough to accomodate all the
   timers in the system, or an err_fatal will be reached */ 
#define TIMER_BIN_NUM             128 

/* The range (in timeticks) of each timer bin 
    - This must be (2^n - 1) to work (for bitwise optimizations)
    - 8191 = 0.25 sec bins */
#define TIMER_BIN_RANGE           8191


/*-----------------------------------------------------------------------------
  Timer Bin structure type
    Represents a single bin used for fast insertion into the timer list
-----------------------------------------------------------------------------*/

typedef struct timer_bin_struct
{
  /* First timer in the bin */
  timer_ptr_type                  first;

  /* Last timer in the bin */
  timer_ptr_type                  last;

  /* Earliest expiry time in the bin */
  timetick_type                   lower;

  /* Latest expiry time in the bin */
  timetick_type                   upper;

  /* Number of timers in the bin */
  int                             count;

  /* Next bin pointer */
  timer_bin_ptr                   next_bin;

  /* Previous bin pointer */
  timer_bin_ptr                   prev_bin;
}
timer_bin_type;


/*-----------------------------------------------------------------------------
  Timer bin list type
-----------------------------------------------------------------------------*/

typedef struct timer_bin_list_struct
{
  /* First timer bin in list */
  timer_bin_ptr                   first;

  /* Last timer bin in list */
  timer_bin_ptr                   last;

  /* Number of timer bins */
  int                             count; 
}
timer_bin_list_type, *timer_bin_list_ptr;


/*-----------------------------------------------------------------------------
  Pool of timer bins
-----------------------------------------------------------------------------*/

static timer_bin_type              timer_bin_pool[ TIMER_BIN_NUM ];


/*-----------------------------------------------------------------------------
  Private timer data ...
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Faux timer group, used to hold list of active timers, as well as a list
     of timer groups.
     Active timers will never "belong" to this group, ie timer->group will
     never be &timers.active, since this would cause them to forget their
     true timer group. */
  timer_group_type                active;

  /* Slow clock timestamp of last write to SLEEP_XTAL_TIMETICK_MATCH */
  timetick_type                   set_time;

  /* Next timer expiration value */
  timetick_type                   set_value;

  /* Last value written to SLEEP_XTAL_TIMETICK_MATCH */
  timetick_type                   match_value;

  /* Slow clock timestamp when timer_isr is called */
  timetick_type                   isr_time;

#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
  /* Offset to remove from sclk time values before programming hardware */
  timetick_type                   sclk_delta;
#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */

  /* Flag to indicate if timers_process_active_timers() is executing */
  boolean                         processing;

  /* Active timer bin list */
  timer_bin_list_type             bins;

  /* Free timer bin list */
  timer_bin_list_type             bins_free;
}
timers_type;


static timers_type                timers =
{
  /* Faux timer group contain active timers & list of timer groups */
  {
    /* Active timers */
    {
      /* Slow clock "zero" base count for active timers list */
      0,

      /* Linked list of active timers */
      NULL
    },

    /* Disabled flag for the Faux Active Timer group.
       This group is always enabled.  When timers are disabled, they are moved
       off of this list */
    FALSE,

    /* Linked list of timer groups which have been disabled at some time. */
    &timers.active
  },

  /* Timestamp at call to _set_next_interrupt */
  0,

  /* Value passed to _set_next_interrupt */
  0,

  /* Value written to match count */
  0,

  /* Timestamp when timer isr occurred at */
  0,

#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
  /* Offset to remove from sclk time values before programming hardware */
  0,
#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */

  /* timers_process_active_timers is not executing */
  FALSE,

  /* Active timer bin list */
  {
    /* First bin in active bin list */
    NULL,

    /* Last bin in active bin list */
    NULL,

    /* Number of bins in list */
    0
  },

  /* Free timer bin list */
  {
    /* First bin in free bin list */
    NULL,

    /* Last bin in free bin list */
    NULL,

    /* Number of bins in list */
    0
  }
};


/*-----------------------------------------------------------------------------
  The NULL timer group, for timers not in any other group
-----------------------------------------------------------------------------*/

/* Timers declared to be in the NULL group are reparented to this group */
timer_group_type                  timer_null_group;

#if (defined(FEATURE_USE_TIME_VU) && defined(FEATURE_ALWAYS_ON_TIMER_PRESENT))
/*-----------------------------------------------------------------------------
  The AlwaysON timer group, for timers that should not be disabled when doing
  TCXO shutdown.

  Note:  This is a WCDMA addition and should probably NOT be in core timer
         services!  The group declaration belongs somewhere else less global.
         
-----------------------------------------------------------------------------*/
timer_group_type                  timer_always_on_group;
#endif


/*-----------------------------------------------------------------------------
  Timer trace data
-----------------------------------------------------------------------------*/

#ifdef FEATURE_TIMER_TRACE

/* Type of timer events to log */
typedef enum
{
  TIMER_TRACE_ISR,
  TIMER_TRACE_PROCESS_START,
  TIMER_TRACE_EXPIRE,
  TIMER_TRACE_PROCESS_END
} timer_trace_event_type;

/* A single timer event trace */
typedef struct
{
  timer_trace_event_type  event;
  timetick_type           ts;

  /* The following only apply for EXPIRE events */
  timer_ptr_type          timer;
  rex_tcb_type            *tcb;
  rex_sigs_type           sigs;
  timer_t1_cb_type        func1;
  timer_t2_cb_type        func2;

} timer_trace_data_type;


/* Structure of all timer log events */
#define TIMER_TRACE_LENGTH  200
typedef struct
{
  uint32                index;
  timer_trace_data_type events[TIMER_TRACE_LENGTH];
} timer_trace_type;

/* Declare the log structure */
static timer_trace_type timer_trace;

#endif /* FEATURE_TIMER_TRACE */

/* Macros to perform logging */
#ifdef FEATURE_TIMER_TRACE
  #define TIMER_TRACE(event)        timer_trace_event(TIMER_TRACE_##event, NULL)
  #define TIMER_TRACE_EXPIRE(timer) timer_trace_event(TIMER_TRACE_EXPIRE, timer)
#else
  #define TIMER_TRACE(event)
  #define TIMER_TRACE_EXPIRE(timer)
#endif




/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


#ifdef FEATURE_TIMER_TRACE
/*=============================================================================

FUNCTION TIMER_TRACE_EVENT

DESCRIPTION
  This function saved a timer trace event packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_trace_event
(
  timer_trace_event_type event,
  timer_ptr_type       timer
)
{
  /* Pointer to the trace structure to fill in */
  timer_trace_data_type *trace;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the next trace structure */
  trace = &timer_trace.events[timer_trace.index];

  /* Fill in the common fields */
  trace->event = event,
  trace->ts    = timetick_get();
  trace->timer = timer;

  /* If a timer was given, save some parameters from it */
  if (timer != NULL)
  {
    trace->tcb = timer->tcb;
    trace->sigs = timer->sigs;
    trace->func1 = timer->func1;
    trace->func2 = timer->func2;
  }
  else
  {
    trace->tcb = NULL;
    trace->sigs = 0;
    trace->func1 = NULL;
    trace->func2 = NULL;
  }

  /* Go to the next index */
  timer_trace.index++;
  if (timer_trace.index >= TIMER_TRACE_LENGTH)
  {
    timer_trace.index = 0;
  }

} /* timer_trace_event */

#endif /* FEATURE_TIMER_TRACE */



#ifdef FEATURE_TIMER_TASK
/*=============================================================================

FUNCTION TIMER_TASK_ISR

DESCRIPTION
  This function detects a timer interrupt, and forwards it to the timer task
  for processing.

DEPENDENCIES
  Must only be called from tramp.

RETURN VALUE
  None

SIDE EFFECTS
  Uninstalls the timer ISR.
  Signals the timer task to process the expiring timer, which can have
  numerous side effects, depending on the expiring timer.

=============================================================================*/

static void timer_task_isr( void )
{
  /* Timer sclk time-stamp values */
  timetick_type                   now, set_time, set_value;

  /* Latency in ISR handling */
  timetick_type                   latency_sclk;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifndef TIMETICK_INT_EDGE_TRIGGERED
  
  /* Uninstall the timer ISR.  The task will reinstall it, if necessary. */
  tramp_set_isr( TRAMP_SLP_TICK_ISR, NULL );

  #endif /* !TIMETICK_INT_EDGE_TRIGGERED */


  INTLOCK( );

  /* Record when timer isr actually was handled */
  timers.isr_time = now = timetick_get_safe();

  /* Capture timer set values, for messaging outside of INTLOCK. */
  set_time  = timers.set_time;
  set_value = timers.set_value;

  INTFREE();


  if ( now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = now - set_value;

    if ( latency_sclk > 300 )
    {
      /* Timer interrupt was handled over btw 5 (60kHz) to 10ms (30kHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      MSG_ERROR("Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }
  }
  else
  {
    MSG_HIGH("Early Timer ISR: ST=%d SV=%d SV-IT=%d",
             set_time, set_value, set_value - now);
  }

  /* Save a trace packet */
  TIMER_TRACE(ISR);

  /* Signal the timer task of the timer interrupt event */
  (void) rex_set_sigs( &timer_tcb, TIMER_EXPIRY_SIG );

} /* timer_task_isr */

#endif /* FEATURE_TIMER_TASK */


#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )

/*=============================================================================

FUNCTION TIMER_SET_SCLK_OFFSET                                         REGIONAL

DESCRIPTION
  Stores the offset between application and modem processors' sclk counters.

  The application and modem processors' sclk h/w counters, while both count
  slow clocks, will have an offset w.r.t. each other, due to application
  processor power collapse.  This difference is eliminated by adding an offset
  to the value read from the application processor's sclk counter.  When
  programming the application processor's match register, this offset needs to
  be removed in order for the match interrupt to occur at the expected time.

DEPENDENCIES
  Regional function.  It may only be called from timetick.c

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_set_sclk_offset
(
  /* Difference between application/modem & MPM sclk counters. */
  timetick_type                   sclk_delta
)
{
  /* Current slow clock count */
  timetick_type                   ticks_now;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Record the new offset between the application/modem & MPM sclk counters */
  timers.sclk_delta = sclk_delta;

  /* Get current slow clock count */
  ticks_now = timetick_get();

  /* Initialize timers.set_value & set_time values for tests in
     set_next_interrupt().  We will assume that the ISR is still properly
     installed from within timer_init(). */
  timers.set_value = ticks_now - 2;
  timers.set_time  = ticks_now - 1;

  #ifndef FEATURE_TIMER_TASK
  /* Set the match value for the timetick counter to ~1.5 days from now,
     to prevent an unexpected interrupt. */
  timer_set_next_interrupt( ticks_now - 1, ticks_now );
  #endif /* !FEATURE_TIMER_TASK */

} /* timer_set_sclk_offset */

#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */



/*=============================================================================

FUNCTION TIMER_SET_NEXT_INTERRUPT_CORE

DESCRIPTION
  Program the SLEEP_XTAL_TIMETICK to generate an interrupt at the given value

DEPENDENCIES
  Internal timer_ function.
  Must be called from INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches given value

=============================================================================*/

timetick_type timer_set_next_interrupt_core
(
  /* Slow clock count at which next interrupt will occur */
  timetick_type                   match_count,

  /* Current slow clock count */
  timetick_type                   ticks_now, 

  /* minimum time till next scheduled timer interrupt */
  timetick_type                   min_rearm_sclk
);
/* Implementation in timer_asm.s */



/*=============================================================================

FUNCTION TIMER_SET_NEXT_INTERRUPT

DESCRIPTION
  Program the SLEEP_XTAL_TIMETICK to generate an interrupt at the given value

DEPENDENCIES
  Internal timer_ function.
  Must be called from INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches given value

=============================================================================*/

static void timer_set_next_interrupt
(
  /* Slow clock count at which next interrupt will occur */
  timetick_type                   match_count,

  /* Current slow clock count */
  timetick_type                   ticks_now
)
{                               

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Don't re-write the same value to the MATCH_VAL register. */
  if ( match_count != timers.set_value  &&  match_count != timers.match_value )
  {
    /* The match value is different from the last match count */

#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
      /* The applications processor's sclk counter operates at an offset from
         the modem processor's sclk counter.  When the applications processor's
         sclk counter is read, an offset is added to it to make the sclk
         counter origins in the application and modem processors equivalent.
         When writing back to the hardware, this offset needs to be removed. */

      /* Set the next timer with a check to make sure that we have 
         not violated the rearm time since ticks_now was sampled.
         Record the actual value written. */
      timers.match_value = timer_set_next_interrupt_core(
           match_count - timers.sclk_delta,
           ticks_now   - timers.sclk_delta,
           MIN_REARM_SCLK
         ) + timers.sclk_delta;

#else
      /* Set the next timer with a check to make sure that we have 
         not violated the rearm time since ticks_now was sampled.
         Record the actual value written. */
      timers.match_value = timer_set_next_interrupt_core(
           match_count, ticks_now, MIN_REARM_SCLK );

#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */

    /* Record when next timer should expire at */
    timers.set_value = match_count;

  } /* If the match-count was different */

  /* Record when this match value was logically written, even if we don't
     bother to write the value if it is unchanged. */
  timers.set_time = ticks_now;

} /* timer_set_next_interrupt */



/*=============================================================================

FUNCTION TIMER_UPDATE_TIMER_INTERRUPT

DESCRIPTION
  If the head of the timer list is modified, update the timer match value.
  Will cause an interrupt to be generated immediately if there is an
  expired timer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches the first timer's expiry point

=============================================================================*/

static void timer_update_timer_interrupt( void )
{
  /* Current slow clock count */
  timetick_type                   now;

  /* Time of first expiring timer */
  timetick_type                   first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  /* If a timer is being altered inside "timer_process_active_timers" ... */
  if ( timers.processing )
  {
    /* ... don't bother updating the timer match value!  Caller will. */
    return;
  }



  /* The following two tests deal with timers.set_value, which is the time
     of the Already Programmed timer interrupt, which may have no relation to
     the first timer on the active timer list. */


  /* Get the current time */
  now = timetick_get_safe( );

  /* If the timer interrupt has already occurred ... */
  if ( timers.set_value - timers.set_time <= now - timers.set_time )
  {
    /* ... let the Timer ISR/Task update the next timer match value */
    return;
  }


  /* If the timer interrupt is just a little in the future ... */
  if ( timers.set_value - now <= MIN_REARM_SCLK )
  {
    /* ... we can't stop it from been generated.  Don't try to. */
    return;
  }



  /* The remaining tests deal with the first timer (if any) on the active
     timer list. */


  /* Are there timers on the timer list? */
  if ( timers.active.list.first != NULL )
  {
    /* Get the time of the first expiring timer */
    first = timers.active.list.first->expiry;

    /* If the first expiring timer matches the timer.set_value, ... */
    if ( first == timers.set_value )
    {
      /* ... then it is already properly programmed in TIMETICK_MATCH */
      return;
    }

    /* If the first expiring timer is still in the past ... */
    if ( first - timers.active.list.zero < now - timers.active.list.zero )
    {
      /* Set the timer for "as soon as possible" (eg, "now") */
      first = now;
    }
  }
  else
  {
    /* If no timers, set "first" to as far in the future as possible */
    first = timers.active.list.zero - 1;
  }


  /* Set the next interrupt match value (unless it is unchanged) */
  timer_set_next_interrupt( first, now );

} /* timer_update_timer_interrupt */



/*=============================================================================

                           TIMER BIN FUNCTIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIMER_BIN_LIST_INSERT_BETWEEN

DESCRIPTION
  Inserts a bin into the specified list between the two specified bin positions 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_insert_between
(
  /* Bin list to insert bin into */
  timer_bin_list_ptr              list, 

  /* Bin to insert in bin list */
  timer_bin_ptr                   bin,

  /* Which node bin should be inserted after */
  timer_bin_ptr                   ptr1,

  /* Which node bin should be inserted before */
  timer_bin_ptr                   ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our bin's prev/next ptrs to point at correct bin structs */
  bin->prev_bin = ptr1; 
  bin->next_bin = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted bin */

  if( ptr1 != NULL )
  {
    ptr1->next_bin = bin;
  }
  else
  {
    list->first = bin; /* we have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev_bin = bin;
  }
  else
  {
    list->last = bin; /* we have a new end of list, update last ptr */
  }

  list->count++;

} /* timer_bin_list_insert_between */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_REMOVE

DESCRIPTION
  Removes a bin from the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_remove
(
  /* Bin list to removed bin from */
  timer_bin_list_ptr              list, 

  /* Bin to remove from bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin->prev_bin != NULL )
  {
    bin->prev_bin->next_bin = bin->next_bin;
  }
  else
  {
    /* This was the head, so update first ptr */
    list->first = bin->next_bin;
  }

  if( bin->next_bin != NULL )
  {
    bin->next_bin->prev_bin = bin->prev_bin;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    list->last = bin->prev_bin;
  }

  /* Update list's count */
  list->count--;

} /* timer_bin_list_remove */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_POP

DESCRIPTION
  Pops a bin off the head of the specified list and returns it 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_list_pop
(
  /* Bin list to remove the head bin from */
  timer_bin_list_ptr              list
) 
{
  /* Bin at head of bin list */
  timer_bin_ptr                   bin = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin != NULL )
  {
    timer_bin_list_remove( list, bin );
  }

  return bin;

} /* timer_bin_list_pop */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_PUSH

DESCRIPTION
  Pushes a bin onto the head of the specified list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_list_push
(
  /* Bin list to push bin onto */
  timer_bin_list_ptr              list, 

  /* Bin to push onto bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  timer_bin_list_insert_between( list, bin, NULL, list->first );

} /* timer_bin_list_push */



/*=============================================================================

FUNCTION TIMER_BIN_CREATE_BETWEEN

DESCRIPTION
  Takes a new bin from free bin pool and adds it between the specified bins
  on the active bin list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  The created timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_create_between
(
  /* Bin to create new bin after */
  timer_bin_ptr                   bin1, 

  /* Bin to create new bin before */
  timer_bin_ptr                   bin2,

  /* Timer which will exist in the bin (used to define bin range) */
  timer_ptr_type                  timer
) 
{ 
  /* Newly created bin */
  timer_bin_ptr                   new_bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Ensure we have a free bin in the pool */
  ASSERT( timers.bins_free.count > 0 );

  /* Pop it off */
  new_bin = timer_bin_list_pop( &timers.bins_free );

  /* Initialize the bin's info */
  new_bin->first  = NULL;
  new_bin->last   = NULL;
  new_bin->count  = 0;

  /* Determine the range of this new bin */
  new_bin->lower = timer->expiry & ~(TIMER_BIN_RANGE); 
  new_bin->upper = new_bin->lower  |  (TIMER_BIN_RANGE); 

  /* finally, compare against prev and next bins to ensure no overlap */

  if( bin1 != NULL )
  {
    /* is the bottom portion of the bin overlapping? */
    if( bin1->upper >= new_bin->lower )
    {
      /* yes, use the upper bound of the prev bin as our upper bound */
      new_bin->lower = bin1->upper + 1;
    }
  }
  if( bin2 != NULL )
  {
    /* is the top portion of the bin overlapping? */
    if( bin2->lower <= new_bin->upper )
    {
      /* yes, use the lower bound of the next bin as our upper bound */
      new_bin->upper = bin2->lower - 1;
    }
  }
        
  timer_bin_list_insert_between( &timers.bins, new_bin, bin1, bin2 );  

  return new_bin;

} /* timer_bin_create_between */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_CREATE

DESCRIPTION
  Searches the active bin list for a bin corresponding to the specified timer  
  If none exists, create it and link it into the bin list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  The timer_bin_type whose range the timer fits into  

SIDE EFFECTS
  None

=============================================================================*/

static timer_bin_ptr timer_bin_find_create
( 
  /* Timer for which a bin is needed */
  timer_ptr_type                  timer
)
{
  /* Pointer to a bin.  Used for searching the list of active timer bins */
  timer_bin_ptr                   bin;

  /* Timer list's zero origin. */
  timetick_type                   zero;

  /* Timer's expiry. */
  timetick_type                   expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  ASSERT( timer->list != NULL );

  bin    = timers.bins.first;
  expiry = timer->expiry; 
  zero   = timer->list->zero; 

  while( bin != NULL )
  {
    /* Is this bin able to hold this timer? */

    if ( expiry >= bin->lower  &&  expiry <= bin->upper )
    {
      /* If this bin straddles the zero, we might need to split this bin into
         one before the zero and one after the zero */

      if( zero >= bin->lower  &&  zero <= bin->upper )
      {
        /* Check if the timer's expiry is less than the zero */

        if( timer->expiry < zero )
        {
          /* split this bin into a pre-zero bin and a post-zero bin, returns
             the first of the split bins */

          bin->lower = zero;

          return timer_bin_create_between( bin->prev_bin, bin, timer );
        }
      }
      
      /* Timer fits in this bin, and it doesn't straddle zero, return it */
      return bin;
    }


    /* Should we create a new bin before this bin? */

    if( expiry < bin->lower )
    {
      /* If it fits between this bin and the prev, or if the prev is NULL
         then create one */

      if( bin->prev_bin == NULL  ||  expiry > bin->prev_bin->upper )
      {
        return timer_bin_create_between( bin->prev_bin, bin, timer );
      }
    }

    /* Doesnt fit in this bin, or before this bin, so iterate to next */
    bin = bin->next_bin;

  } /* while( bin != NULL ) */


  /* If we've gotten to the end, create a bin after the last bin */
  return timer_bin_create_between( timers.bins.last, NULL, timer );

} /* timer_bin_find_create */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_FIRST_EXCEEDING

DESCRIPTION
  Returns the first timer on the bin list exceeding the specified 
  timer's range. If none are found, null is returned 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  The timer meeting the description's criteria.  

SIDE EFFECTS
  None

=============================================================================*/

static timer_ptr_type timer_bin_find_first_exceeding
(
  /* Timer used to determine bin range */
  timer_ptr_type                  timer
)
{
  /* First timer in bins */
  timer_ptr_type                  first;

  /* Timer bin under consideration */
  timer_bin_ptr                   bin;

  /* Origin for timer expiry comparisions */
  timetick_type                   zero   = timer->list->zero;

  /* Expiry of timer under consideration */
  timetick_type                   expiry = timer->expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  /* Search the bins until we find one with a timer exceeding this timer's
     expiry */

  /* Skip passed all the unbinned expired timers at the head, if any */
  first   = timers.active.list.first;
  while( first != NULL )
  {
    if( first->bin != NULL )
    {
      break;
    }
    first = first->next;
  }

  /* Start with the bin associated with the first on the active list */
  if( first == NULL )
  {
    return NULL;
  }

  for( bin = first->bin;  bin != NULL;  bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry - zero <= bin->upper - zero )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }

  /* now continue at the beginning of the bin list and
     progress until we hit the bin of the first timer on the active list */
  for( bin = timers.bins.first; bin != first->bin; bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry - zero <= bin->upper - zero )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }
 
  /* If we've gotten here, return the timer active list last pointer */
  return timers.active.list.last;

} /* timer_bin_find_first_exceeding */



/*=============================================================================

FUNCTION TIMER_BIN_INSERT

DESCRIPTION
  Inserts the specified timer into the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_insert
(
  /* Bin to insert timer in */
  timer_bin_ptr                   bin,

  /* Timer to insert in bin */
  timer_ptr_type                  timer 
)
{
  /* Lowest expiry time in first bin */
  timetick_type                   bin_lowest;

  /* Highest expiry time in last bin */
  timetick_type                   bin_highest;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Since it is known that a bin does not straddle a counter loop,
     these compares can be done without a zero reference */


  /* Calculate the lowest timer expiry in the bin */

  if( bin->first != NULL )
  {
    bin_lowest  = bin->first->expiry;

    /* do we need to update the first pointer? */
    if( timer->expiry < bin_lowest )
    {
      /* We have a new first pointer */

      /* Note: if the expiries are equal, the first pointer remains the same
         to match the behavior of the timer list */ 
      bin->first = timer;
    } 

  }
  else
  {
    /* If no first pointer, update first pointer with new timer */
    bin->first = timer;
  } 

  /* Calculate the highest timer expiry in the bin */
  if( bin->last != NULL )
  {
    bin_highest = bin->last->expiry;

    /* Do we need to update the last pointer? */
    if( timer->expiry >= bin_highest )
    {
      /* We have a new last pointer */

      /* Note: if the expiries are equal, the last pointer is updated 
         to match the behavior of the timer list */ 
      bin->last = timer;
    }

  }
  else
  {
    /* If no last pointer, update last pointer with new timer */
    bin->last = timer;
  }

  /* Record the bin in the timer struct */
  timer->bin = bin; 

  /* Increment the bin's count */
  bin->count++;

} /* timer_bin_insert */



/*=============================================================================

FUNCTION TIMER_BIN_REMOVE

DESCRIPTION
  Remove the specified timer from the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_bin_remove
(
  /* Timer to remove from bin list */
  timer_ptr_type                  timer
)
{
  /* Bin timer belongs to */
  timer_bin_ptr                   bin = timer->bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( bin != NULL );


  /* Do we now have an empty bin? */

  if( bin->count == 1 )
  {
    /* empty bin */
    bin->first = NULL;
    bin->last  = NULL;
  }
  else if( bin->count > 1 )
  {
    /* Check if we have a new bin first */
    if( timer == bin->first )
    { 
      bin->first = timer->next;
    }

    /* Check if we have a new bin last */ 
    if( timer == bin->last )
    {
      bin->last = timer->prev;
    }
  }

  /* Unbin the timer */ 
  timer->bin = NULL;

  /* Decrement the bin's count */
  bin->count--;  

  if( bin->count == 0 )
  {
    /* If this bin is now empty, then remove it from the active bin list
       and return it to the free list */
    timer_bin_list_remove( &timers.bins, bin );
    timer_bin_list_push( &timers.bins_free, bin);
  }

} /* timer_bin_remove */



/*=============================================================================

FUNCTION TIMER_INSERT_BETWEEN

DESCRIPTION
  Places 'timer' before 'ptr1' and after 'ptr2' in 'list'.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_insert_between
(
  /* List to insert timer into */
  timer_list_ptr                  list,

  /* Timer to insert into list */
  timer_ptr_type                  timer, 

  /* Timer to insert after */
  timer_ptr_type ptr1,

  /* Timer to insert before */
  timer_ptr_type ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our timer's prev/next ptrs to point at correct timer structs */

  timer->prev = ptr1; 
  timer->next = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted timer */

  if( ptr1 != NULL )
  {
    ptr1->next = timer;
  }
  else
  {
    list->first = timer; /* We have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev = timer;
  }
  else
  {
    list->last = timer; /* We have a new end of list, update last ptr */
  }
 
} /* timer_insert_before */



/*=============================================================================

FUNCTION TIMER_INSERT

DESCRIPTION
  Inserts a timer into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert
(
  /* List timer is to be inserted into */
  timer_list_ptr                  list,

  /* Timer to be inserted into list of active timers */
  timer_ptr_type                  timer
)
{
  /* Pointer to a timer. Used for walking list of timers */
  timer_ptr_type                  ptr;

  /* Pointer to a timer. Determines where on the list to start looking */
  timer_ptr_type                  start;

  /* Offset from "zero" time of list */
  timetick_type                   delta;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First set the list field of the timer */
  timer->list = list;

  /* if this timer is to be inserted on the active list */
  if( list == &timers.active.list )
  {

    /* If this timer is going on the active list we can use binning 
       to help us cut down on the list search time */

     /* Obtain the first timer from the bin exceeding this timer's expiry */
    start = timer_bin_find_first_exceeding( timer ); 
  }
  else
  {
    /* If this is not an active timer, do not optimize the search,
       just search from the start of the list */
    start = list->first;
  }

  /* Initialize the start position */
  if( start != NULL )
  {
    ptr = start;
  }
  else
  {
    ptr = list->first;
  }

  /* Search for appropriate list location to insert timer */
  delta = timer->expiry - list->zero;
  while ( ptr != NULL &&  ptr->expiry - list->zero <= delta )
  {
    ptr = ptr->next;
  }

  /* Insert the timer into the list */
  timer_insert_between( list, timer, ptr ? ptr->prev : list->last, ptr ); 

  /* Only bin this timer if we are inserting into the active list */
  if( list == &timers.active.list )
  {
    /* Insert thie timer into the appropriate timer list bin */
    timer_bin_insert( timer_bin_find_create(timer), timer );
  }

} /* timer_insert */



/*=============================================================================

FUNCTION TIMER_INSERT_LIST

DESCRIPTION
  Inserts a sorted list of timers into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert_list
(
  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list,

  /* Linked list of timers to be inserted into the first list */
  timer_ptr_type                  src_list
)
{
  /* A timer from the second list, to be inserted in the first */
  timer_ptr_type                  timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Repeat for all timers on the source list */
  while ( src_list != NULL )
  {
    /* Retrieve a timer from the start of the source list */
    timer    = src_list;
    src_list = src_list->next;

    timer_insert( dst_list, timer );
  }

} /* timer_insert_list */



/*=============================================================================

FUNCTION TIMER_CONCATENATE_LISTS

DESCRIPTION
  Concatenates the src_list and the dst_list with the combined list taking on
  the identity of the dst_list.  src_list is cleared.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_concatenate_lists
(
  /* Linked list of timers to be inserted into the first list */
  timer_list_ptr                  src_list,

  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list
)
{
  /* timer iteration pointer */
  timer_ptr_type                  timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( src_list != NULL );

  /* Check if the source list is empty */
  if( src_list->first == NULL )
  {
    return;
  }

  /* Stitch the two lists together */

  /* Is the destination list empty? */
  if( dst_list->first != NULL )
  { 
    /* No, so update the head's previous pointer */
    dst_list->first->prev = src_list->last;

    /* Update the source list's last next pointer */
    src_list->last->next = dst_list->first; 

  }
  else
  {
    /* Yes, so update the destination list's last pointer */
    dst_list->last = src_list->last;
  }

  /* Update the destination lists's first pointer */
  dst_list->first = src_list->first;
 
  /* Now, for each of the src list items, update their structures */
  timer = src_list->first;
  while( timer != NULL )
  {
    /* Update the list membership */
    timer->list = dst_list;

    /* If this is the last of the src list, we are done */
    if( timer == src_list->last )
    {
      break;
    }

    /* Proceed to next timer */
    timer = timer->next; 
  }

  /* Finally update the source list */
  src_list->first = NULL;
  src_list->last  = NULL;
    
} /* timer_concatenate_lists */



/*=============================================================================

FUNCTION TIMER_UNLINK

DESCRIPTION
  Simply unlinks a timer from its current list of timers.
  Updates the associated list first/last ptrs

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_unlink
(
  /* Timer to remove from its current list */
  timer_ptr_type                  timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer->prev != NULL )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    /* This was the head, so update first ptr */
    timer->list->first = timer->next;
  }

  if( timer->next != NULL )
  {
    timer->next->prev = timer->prev;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    timer->list->last = timer->prev;
  }

} /* timer_unlink */



/*=============================================================================

FUNCTION TIMER_REMOVE

DESCRIPTION
  Removes a timer from the list of timers.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove
(
  /* Timer to be removed from list of active timers */
  timer_ptr_type                  timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer->list == NULL )
  {
    return;
  }

  /* if this timer is to be removed from the active list */
  if( timer->bin != NULL )
  {
    /* Unbin this timer */
    timer_bin_remove( timer );
  }


  /* Fix up links/list around this node */

  if ( timer->prev )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    timer->list->first = timer->next;
  }

  if ( timer->next )
  {
    timer->next->prev = timer->prev;
  }                                 
  else
  {
    timer->list->last = timer->prev;
  }


  timer->next = NULL;
  timer->prev = NULL;
  timer->list = NULL;

} /* timer_remove */



/*=============================================================================

FUNCTION TIMER_REMOVE_HEAD

DESCRIPTION
  Removes the head of the specified timer list.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove_head
(
  /* List to chop head off of */
  timer_list_ptr                  list
)
{
  /* Head of the timer list */
  timer_ptr_type                  head = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* New front of list */
  list->first = head->next;

  /* Fix up links/list around this node */
  if( list->first != NULL )
  {
    /* Update next node's prev ptr */
    list->first->prev = NULL;
  }
  else
  {
    /* Empty list */
    list->last = NULL;
  }


  /* NULL out deleted node's link fields */
  head->next = NULL;
  head->list = NULL;

} /* timer_remove_head */



/*=============================================================================

FUNCTION TIMER_APPEND

DESCRIPTION
  Appends the list with a timer
  The appended timer's list member is updated

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_append
(
  /* List to add timer to */
  timer_list_ptr                  list,

  /* Timer to add to list */
  timer_ptr_type                  timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update timer's links */
  timer->prev = list->last;
  timer->next = NULL;

  /* Update the old tail to point to this new timer */
  if( list->last != NULL )
  {
    list->last->next = timer;
  }
  else
  {
    /* Empty list, have a new head */
    list->first = timer;
  } 
  
  /* Always have a new tail */ 
  list->last = timer;
 
  /* We have a new list for timer */ 
  timer->list = list;

} /* timer_append */



/*=============================================================================

FUNCTION TIMER_IN_LIST

DESCRIPTION
  Returns whether the timer actually exists in the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static boolean timer_in_list
(
  /* Timer under test */
  timer_ptr_type                  timer,

  /* List timer may be a member of */
  timer_list_ptr                  list
)
{
  /* Used to walk timer list */
  timer_ptr_type                  ptr; 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Do the search */
  for( ptr = list->first; ptr != NULL; ptr = ptr->next )
  {
    if( ptr == timer )
    {
      /* Timer is in list */
      return TRUE;
    }
  }

  /* Timer was not in list */
  return FALSE;

} /* timer_in_list */



/*=============================================================================

FUNCTION TIMER_EXPIRE

DESCRIPTION
  Processes an expiring timer, according to the timer's definition.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside INTLOCK & TASKLOCK (or interrupt) contexts

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_expire
(
  /* Expiring timer to be processed */
  timer_ptr_type                  timer,

  /* Duration timer ran for. */
  timetick_type                   run_time
)
{
  /* Flag indicating if timer has been properly processed */
  boolean                         processed = FALSE;

  /* Duration timer ran for, in milliseconds */
  timetick_type                   run_time_ms = 0;

  /* Duration timer ran for, in milliseconds */
  timetick_type                   run_time_sclk;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If a conversion from sclk to milliseconds is necessary, perform it now. */
  if ( timer->func2 != NULL)
  {
    /* Compute time (in milliseconds) timer has been running for.
       This may be larger than expected if the timer expires during sleep,
       or otherwise has its expiry processing delayed */
    run_time_ms = timetick_cvt_from_sclk( run_time, T_MSEC);

    /* If a timer is a reloading timer, the millisecond conversion will have
       a rounding error.  We can adjust the start time to account for the
       residual.  This is done before signalling the timer expiry, since
       the timer expiry might alter the timer data */

    if ( timer->list != NULL )
    {
      /* Timer is still on the list.  It is a reloading timer. */

      /* Difference btw run_time & run_time_sclk is rounding error (+/-.5ms) */
      run_time_sclk = timetick_cvt_to_sclk( run_time_ms, T_MSEC);

      /* Nudge start time, so the next expiry includes this rounding error */
      if ( run_time >= run_time_sclk )
      {
        /* Rounded down: move start backwards, so next duration is longer */
        timer->start -= run_time - run_time_sclk;
      }
      else if ( timer->expiry - timer->start > run_time_sclk - run_time )
      {
        /* Rounded up: move start forward, so next duration is shorter */
        timer->start += run_time_sclk - run_time;
      }
      else
      {
        /* Rounded up, but next expiry is too close for rounding nudging. */
        timer->start = timer->expiry - 1;
      }
    }
  }


  /* If there are signals to be set, and a task to set the signals to,
     then set those signals in that task's task control block */

  if ( timer->sigs != 0  &&  timer->tcb != NULL )
  {
    (void) rex_set_sigs( timer->tcb, timer->sigs );
  }


  /* If the timer was defined with a type-1 style function callback,
     call the function, or post the function as an asynchronous call */

  if ( timer->func1 != NULL )
  {
#ifdef FEATURE_REX_APC

    /* If a task has been specificed, use the Asynchronous Procedure Call
       functions to call the function at the given task's priority,
       not from interrupt context */

    if ( timer->tcb != NULL )
    {
      if ( rex_is_in_irq_mode( ) )
      {
        processed = rex_queue_apc(  timer->func1,
                                    timer->tcb,
                                    timer->data
                                 );
      }
      else
      {
        processed = rex_queue_apc_internal(
                                    (unsigned long) timer->func1,
                                    (unsigned long) timer->tcb,
                                    timer->data
                                 );
      }
    }
#endif /* FEATURE_REX_APC */

    if ( !processed )
    {
      /* Can't queue APC -- call it ourselves */
      timer->func1( timer->data );
    }

  } /* timer->func1 != NULL */


  /* If the timer was defined with a type-2 style function callback,
     (clock callback style), call the function with the time (in
     milliseconds) between timer start and now. */

  if ( timer->func2 != NULL)
  {
    /* Call the callback function */
    timer->func2( (int32) run_time_ms, timer->data);
  }

} /* timer_expire */



/*=============================================================================

FUNCTION TIMER_PROCESS_ACTIVE_TIMERS

DESCRIPTION
  Process timers, decrementing elapsed time from each one.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside INTLOCK & TASKLOCK (or interrupt) contexts, to
  prevent a large delay between reading the current timetick count, and
  writing the new timetick match value.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_process_active_timers( void )
{
  /* Current slow clock count */
  timetick_type                   ticks_now;

  /* Timer being processed */
  timer_ptr_type                  timer;

  /* Length of time the timer ran for */
  timetick_type                   run_time;

  /* Minimum advance required for reloading timer */
  timetick_type                   min_advance;

  /* Interrupt locking status */
  dword                           isave;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Lock interrupts while testing & manipulating the active timer list */
  INTLOCK_SAV( isave );

  /* Record the fact that timers are being processed, to prevent re-entry
     into this function, and unnecessary modification of TIMETICK_MATCH. */
  timers.processing = TRUE;

  /* Save a trace packet */
  TIMER_TRACE(PROCESS_START);

  /* Get current slow clock count */
  ticks_now = timetick_get_safe();

  /* Check to determine if the timer at head of the active timer list has
     past its expiry point.  If it has, remove it, expire the timer, and
     repeat with the new timer at the head of active timer list.
       
     Note: a timer's expiry can change timers.active.list.zero, so no local
     variable should hold a calculated value involving it from one iteration
     to the next. */

  while ( timer = timers.active.list.first,
          timer != NULL
            &&  timer->expiry - timers.active.list.zero
                 <= ticks_now - timers.active.list.zero )
  {
    if( timer->bin != NULL )
    {
      /* Unbin this timer */
      timer_bin_remove( timer );
    }

    /* Remove expiring timer from active timer list */
    timer_remove_head( &timers.active.list );
     
    /* Compute length of time the timer ran for.  This may be longer than
       programmed due to sleep or other delays in expiry processing. */
    run_time = ticks_now - timer->start;

    /* Reactivate timer, if required */
    if ( timer->reload > 0 )
    {
      /* Determine how late the timer expired; this is the minimum
         amount the timer must be advanced by for the next expiry. */
      min_advance = ticks_now - timer->expiry;

      if ( min_advance < timer->reload )
      {
        /* Common case for reloading just after timer is scheduled to expire:
           Only need to advance expiry time by 1 reload period. */
        timer->expiry += timer->reload;
      }
      else
      {
        /* Timer expired 1 or more reload period ago.  This can happen if
           the timer belongs to a timer group which gets disabled, such as
           the default timer groups during sleep. */

        /* Round min_advance up to the next multiple of reload periods. */
        min_advance += timer->reload - min_advance % timer->reload;

        /* Add the rounded-up minimum advance to the timer expiry */
        timer->expiry += min_advance;
      }


      /* Record the new start time for the next expiry */
      timer->start = ticks_now;

      /* Insert timer back in active list */
      timer_insert( &timers.active.list, timer );
    }
    else
    {
      /* Timer is no longer running */
      timer->expiry = 0;
    }

    /* We've finished manipulating the active timer list.  Unlock
       interrupts for the duration of the timer's expiry processing */
    INTFREE_SAV( isave );

    /* Save a trace packet */
    TIMER_TRACE_EXPIRE(timer);

    /* Expire timer */
    timer_expire( timer, run_time );

    /* NOTE: The above call may run user code, which can do anything ...
       including installing and uninstall other timers.  This can cause
       the value of timers.active.list.zero to change at this point.
       We must use the new value in the next iteration, so any local
       variable which holds computations involving that value must
       be considered invalid after the above call.  */


    /* Lock interrupts in preparation of the next iteration of the loop,
       which tests & manipulates the active timer list */
    INTLOCK_SAV( isave );

    /* Get current slow clock count */
    ticks_now = timetick_get_safe();

  } /* while timers on timer.active.list are expiring */


  /* At this point, all timers on the active list expire after "ticks_now".
     We may move up the zero point for the active timer list to the
     current value of "ticks_now". */

  /* Move the zero-origin up to the current time */
  timers.active.list.zero = ticks_now;



  #ifndef TIMETICK_INT_EDGE_TRIGGERED
  
  #ifdef FEATURE_TIMER_TASK

  /* Install the timer expiry ISR */
  tramp_set_isr( TRAMP_SLP_TICK_ISR, timer_task_isr );

  #endif /* FEATURE_TIMER_TASK */

  /* Ensure interrupt source has deasserted before continuing.
     This must occur before the timer_set_next_interrupt(), to prevent
     accidently clearing the interrupt for the next timer. */
  tramp_block_till_deasserted( TRAMP_SLP_TICK_ISR, 30 );

  #endif /* !TIMETICK_INT_EDGE_TRIGGERED */


  /* Timers that expire at and before "ticks_now" have been processed.
     Set interrupt for when next timer expires. */

  if ( timers.active.list.first != NULL )
  {
    /* The first timer on the active list is the timer next to expire */
    timer_set_next_interrupt(timers.active.list.first->expiry, ticks_now );
  }
  else
  {
    /* There is no next timer.  Set interrupt to a long, long time from now,
       about ~1.5 days.  This will be reset when a new timer is defined. */
    timer_set_next_interrupt( ticks_now - 1, ticks_now );
  }


  /* Timer processing has completed */
  timers.processing = FALSE;

  /* Save a trace packet */
  TIMER_TRACE(PROCESS_END);

  /* We've finished manipulating the active timer list.  */
  INTFREE_SAV( isave );



  #if defined(T_QUASAR) && !defined(T_QUASAR_X)
#error code not present
  #endif /* T_QUASAR */

} /* timer_process_active_timers */



#ifdef FEATURE_TIMER_TASK
/*=============================================================================

FUNCTION TIMER_TASK

DESCRIPTION
  Initializes the Timer Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           ignored    /*lint -esym(715,ignored) */
)
{
  /* Signals returned from rex wait procedure */
  rex_sigs_type                   sigs;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef TIMETICK_INT_EDGE_TRIGGERED
  
  /* Install the timer expiry ISR */
  tramp_set_isr( TRAMP_SLP_TICK_ISR, timer_task_isr );

  #endif /* TIMETICK_INT_EDGE_TRIGGERED */


  /* Signal the task in order to prime the timer processing mechanism */
  (void) rex_set_sigs( &timer_tcb, TIMER_CHANGE_SIG );

  for(;;)
  {
    /* Wait for some task signal */
    sigs = rex_wait( TIMER_PROCESS_SIGS | TIMER_TASK_SIGS );

    if ( sigs & TIMER_PROCESS_SIGS )
    {
      /* Clear the timer expiry signal */
      (void) rex_clr_sigs( &timer_tcb, TIMER_PROCESS_SIGS );

      /* Process expiring timer(s) */
      timer_process_active_timers();
    }
    else if ( sigs & TASK_STOP_SIG )
    {
      /* Clear the stop signal */
      (void) rex_clr_sigs( &timer_tcb, TASK_STOP_SIG );

      /* Acknowledge the task stop signal */
      task_stop();
    }
    else if ( sigs & TASK_OFFLINE_SIG )
    {
      /* Clear the offline signal */
      (void) rex_clr_sigs( &timer_tcb, TASK_OFFLINE_SIG );

      /* Acknowledge the task offline signal */
      task_offline();
    }
  }

} /* timer_task */

#else /* !FEATURE_TIMER_TASK */


/*=============================================================================

FUNCTION TIMER_ISR

DESCRIPTION
  This function locks interrupts and processes those timers which are expiring.

DEPENDENCIES
  Must only be called from tramp.

RETURN VALUE
  None

SIDE EFFECTS
  Timers may fire, signals to tasks may be generated, Asynchronous Procedure
  Calls may be made.

=============================================================================*/

static void timer_isr( void )
{
  /* Timer sclk time-stamp values */
  timetick_type                   now, set_time, set_value;

  /* Latency in ISR handling */
  timetick_type                   latency_sclk;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  INTLOCK( );

  /* Record when timer isr actually was handled */
  timers.isr_time = now = timetick_get_safe();

  /* Capture timer set values, for messaging outside of INTLOCK. */
  set_time  = timers.set_time;
  set_value = timers.set_value;

  INTFREE();


  if ( now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = now - set_value;

    if ( latency_sclk > 300 )
    {
      /* Timer interrupt was handled over btw 5 (60kHz) to 10ms (30kHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      MSG_ERROR("Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }

    timer_process_active_timers();
  }

} /* timer_isr */

#endif /* FEATURE_TIMER_TASK */


/*=============================================================================

FUNCTION TIMER_INIT                                                    REGIONAL

DESCRIPTION
  Initializes the 32kHz slow clock based Timer Subsytem

DEPENDENCIES
  Must only be called from time_init()

RETURN VALUE
  None

SIDE EFFECTS
  May install an ISR for the 32-bit slow clock match register

=============================================================================*/

void timer_init( void )
{
  /* Current slow clock count */
  timetick_type                   ticks_now;

  /* Loop index for timer bin initialization */
  int                             i;

  /* Timer bin */
  timer_bin_ptr                   bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef T_MSM7500
#error code not present
  #endif /* T_MSM7500 */

  /* Get current slow clock count */
  ticks_now = timetick_get();

  /* Initialize the timer_bin_pool and free list */

  for(i = 0; i < TIMER_BIN_NUM; i++)
  {
    bin = &timer_bin_pool[i]; 

    /* init the bin's data */
    bin->first    = NULL;
    bin->last     = NULL;
    bin->lower    = 0;
    bin->upper    = 0;
    bin->count    = 0;

    /* push it on the free list */
    timer_bin_list_push( &timers.bins_free, bin);
  }

  /* Initialize the list of active timers to contain no timers */
  timers.active.list.first = NULL;
  timers.active.list.last  = NULL;
  timers.active.list.zero  = ticks_now;

  /* Initialize timers.set_value & set_time values for tests in
     set_next_interrupt() and update_timer_interrupt */
  timers.set_value = ticks_now - 2;
  timers.set_time  = ticks_now - 1;

  #ifdef TIMETICK_INT_EDGE_TRIGGERED
  
  /* Setup edge-sensitive interrupt, instead of having the interrupt
     asserted for 30us */
  TIMETICK_INT_EDGE_TRIGGERED_INIT();

  #endif /* TIMETICK_INT_EDGE_TRIGGERED */


  #ifndef FEATURE_TIMER_TASK
  
  /* Set the match value for the timetick counter to ~1.5 days from now,
     to prevent an unexpected interrupt. */
  timer_set_next_interrupt( ticks_now - 1, ticks_now );

  /* Install the timer expiry ISR */
  tramp_set_isr( TRAMP_SLP_TICK_ISR, timer_isr );

  #endif /* !FEATURE_TIMER_TASK */

} /* timer_init */



/*=============================================================================

FUNCTION TIMER_GROUP_DISABLE

DESCRIPTION
  Disables a timer group.  Timers in the timer group will continue to count
  down, but they will not expire until the group is enabled.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_group_disable
(
  /* Timer group to be disabled */
  timer_group_ptr                 timer_group
)
{
  /* Pointer to timers, for walking through lists of timers */
  timer_ptr_type                  ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer_group != NULL );


  INTLOCK();

  /* Disable the timer group, if it is not already disabled */

  if ( !timer_group->disabled )
  {
    /* Mark timer-group as disabled, and record zero-basis */
    timer_group->disabled  = TRUE;
    timer_group->list.zero = timers.active.list.zero;

    /* Move given group's timers from active timer list to group timer list */
    ptr = timers.active.list.first;

    while (ptr != NULL)
    {
      /* Remember the next element on the active list, we are going
         to nuke it */
      timer_ptr_type next = ptr->next;

      if ( ptr->group == timer_group )
      {
        /* This is a timer in the given timer group */

        /* if it's binned, remove it from its bin */
        if( ptr->bin != NULL ) 
        {
          timer_bin_remove( ptr );
        }
       
        /* Unlink timer from active list */
        timer_unlink( ptr );
       
        /* Append to group timer list */
        timer_append( &timer_group->list, ptr );
      }

        /* Walk to next timer node */
      ptr = next;

    }

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt( );
  }

  INTFREE();

} /* timer_group_disable */



/*=============================================================================

FUNCTION TIMER_GROUP_ENABLE

DESCRIPTION
  Enables a timer group.  Timers in the timer group which expired during the
  diabled period will have their expiry processing executed.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_group_enable
(
  /* Timer group to be enabled */
  timer_group_ptr                 timer_group
)
{
  /* Current slow clock count */
  timetick_type                   ticks_now;

  /* Pointer to a timer in the list of timers */
  timer_ptr_type                  timer;

  /* Duration timer group was disabled for */
  timetick_type                   duration;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer_group != NULL );

  INTLOCK();

  /* Enable the timer group, if it is disabled */

  if ( timer_group->disabled )
  {
    /* Get current slow clock count */
    ticks_now = timetick_get_safe();

    /* Walk through timers in timer group list.
       Stop when we get to a timer after the head of the active list */

    timer = timer_group->list.first;
    duration = ticks_now - timer_group->list.zero;

    while( timer != NULL )
    { 
      if( timer->expiry - timer_group->list.zero >= duration )
      {
        break; 
      }

      timer = timer->next;
    }

    /* Break timer list into two parts at this point */
    if( timer != NULL )
    {
      timer_group->list.last = timer->prev;

      if( timer->prev == NULL )
      {
        timer_group->list.first = NULL;
      }
      else
      {
        timer->prev->next = NULL;
      }

      timer->prev = NULL;
    }
        
    /* Expired timers are in list pointed to by "timer_group->list" */
    /* Non-expired timers are in linked list pointed to by "timer" */

    /* Move non-expired timers into active timer list */
    timer_insert_list( &timers.active.list, timer );

    /* If there are any expired timers ... */
    if ( timer_group->list.first != NULL ) {

      /* Move them to the front of the special expired list */
      timer_concatenate_lists( &timer_group->list, &timers.active.list ); 
 
      /* Update the active timer list's zero-origin, if appropriate */
      if ( ticks_now - timer_group->list.zero > 
           ticks_now - timers.active.list.zero )
      {
        /* Group timer list zero-origin is in the past, relative to the
           active timer list zero-origin.  Move active timer list's
           zero-origin back */
        timers.active.list.zero  = timer_group->list.zero;
      }

    }

    /* Mark timer-group as enabled */
    timer_group->disabled = FALSE;

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt();
  }

  INTFREE();

} /* timer_group_enable */



/*=============================================================================

FUNCTION TIMER_DEFINE

DESCRIPTION
  Defines a timer structure, initializing members to NULL, and associates the
  timer with the given group.

DEPENDENCIES
  Timer must not already be defined and on a timer list (active).

RETURN VALUE
  None

SIDE EFFECTS
  May link timer group into linked list of timer groups

=============================================================================*/

static void timer_define
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Reparent timers in the NULL group to be in the timer_null_group */
  if ( group == NULL )
  {
    group = &timer_null_group;
  }

  /* Protect against simultaneous updates of linked lists */
  INTLOCK();

  /* If this timer group is not yet in the linked list of timer groups,
     add it to the list. */

  if ( group->next == NULL )
  {
    group->next = timers.active.next;
    timers.active.next = group;
  }


#define TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER
#ifdef TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER

  /* Check if timer is active (already on the active timer list, or a
     deactived timer group list).  If it is, we need to remove it,
     or when the structure is re-initialized, the timer linked lists
     will be corrupted. */

  if ( timer->list != NULL )
  {
    /* Timer might be active ... or it might be declared as a stack
       variable, and contain garbage values in all of the fields.
       Search for the list it claims to be a member of in the
       linked list of timer groups. */

    timer_group_ptr  grp = &timers.active;

    do
    {
      /* Is the timer a member of the "grp" group? */
      if ( timer->list == &grp->list )
      {
        /* Yes - timer claims to be a member of an existing timer group */

        ERR("timer_def(%x,%x) called for active timer (list %x)",
            timer, group, timer->list);

        /* Find timer in list */
        if( timer_in_list( timer, timer->list ) )
        {
          /* Deactivate timer before initializing timer structure */
          timer_remove( timer );
        }

        break;
      }

      /* Check next group */
      grp = grp->next;

    } while ( grp != &timers.active );
  }

#endif /* TIMER_CHECK_FOR_DEFINE_OF_ACTIVE_TIMER */

  /* End of critical section */
  INTFREE();


  /* Initialize timer structure */
  memset(timer, 0, sizeof(*timer));  /* Most fields are 0's or NULL's */
  timer->group = group;

} /* timer_define */



/*=============================================================================

FUNCTION TIMER_DEF

DESCRIPTION
  Defines and initializes a timer.

  When the timer expires:
    if sigs is non-null, those signals are set to the task given by tcb;
    if func is non-null, then
       if tcb is null, then func(data) is called from interrupt context,
       otherwise, func(data) is queued as an Asynchronous Procedure Call (APC)

  Timers may also be staticly declared via the TIMER_DEF() macro

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_def
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  /* Task to signal and/or task to queue APC call for */
  rex_tcb_type                    *tcb,

  /* Task signals to set to the tcb task when timer expires */
  rex_sigs_type                   sigs,

  /* APC (tcb!=NULL) or callback (tcb==NULL) to queue/call at timer expiry */
  timer_t1_cb_type                func,

  /* Arbitrary data for APC or callback */
  timer_cb_data_type              data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer != NULL );


  /* Initialize new timer structure */
  timer_define( timer, group );

  if ( sigs != 0  &&  tcb == NULL )
  {
    ERR_FATAL("Can't set signals to NULL task. T=%x S=%x", timer, sigs, 0);
  }

  /* Copy data to timer structure */
  timer->tcb      = tcb;
  timer->sigs     = sigs;
  timer->func1    = func;
  timer->data     = data;

} /* timer_def */



/*=============================================================================

FUNCTION TIMER_DEF2

DESCRIPTION
  Defines a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

  Timers may also be staticly declared via the TIMER_DEF2() macro

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_def2
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer != NULL );


  /* Initialize new timer structure */
  timer_define( timer, group );


  /* timer_reg() will fill in the timer members "func2" and "data" */


} /* timer_def2 */



/*=============================================================================

FUNCTION TIMER_REG

DESCRIPTION
  Initializes a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

DEPENDENCIES
  Timer must have been defined using timer_def2() or TIMER_DEF2()

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_reg
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Function to call at timer expiry */
  timer_t2_cb_type                func,

  /* Arbitrary data for func(time_ms, data) */
  timer_cb_data_type              data,

  /* Time (in ms) until first timer expiry */
  timetick_type                   time_ms,

  /* Period (in ms) between repeated expiries (0 = not periodic) */
  timetick_type                   reload_ms
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer != NULL );


  /* Copy function & function data to timer structure */
  timer->func2    = func;
  timer->data     = data;

  /* Start the timer */
  timer_set(timer, time_ms, reload_ms, T_MSEC );

} /* timer_reg */



/*=============================================================================

FUNCTION TIMER_SET

DESCRIPTION
  Sets an inactive timer to expire after a given period of time, or changes
  an active timer to expire after a given period of time.

  Optionally, specifies the timer to repeatly expire with a given period.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_set
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Time (in units below) until first timer expiry */
  timetick_type                   time,

  /* Period (in units) between repeated expiries (0 = not periodic) */
  timetick_type                   reload,

  /* Unit to measure "ticks" in. */
  timer_unit_type                 unit
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer != NULL );

  if ( timer->group == NULL )
  {
    ERR("timer_def was not called for timer %x", timer, 0, 0);
    timer->group = &timer_null_group;
  }


  /* If the time duration is different from the cached duration ... */
  if ( unit != timer->cache.unit  ||  time != timer->cache.duration )
  {
    /* ... convert given duration into slow clocks, and save in cache */
    timer->cache.duration      = time;
    timer->cache.duration_sclk = timetick_cvt_to_sclk(time, unit );
  }

  /* Retrieve converted time duration from cache */
  time = timer->cache.duration_sclk;


  /* If the reload value is non-zero ... */
  if ( reload > 0 )
  {
    /* If the reload interval is different from the cached interval ... */
    if ( unit != timer->cache.unit  ||  reload != timer->cache.reload )
    {
      /* Convert and cache given reload interval into slow clocks */
      timer->cache.reload      = reload;
      timer->cache.reload_sclk = timetick_cvt_to_sclk(reload, unit );
    }

    /* Retrieve converted reload interval from cache */
    reload = timer->cache.reload_sclk;
  }

  /* Ensure cached unit value is correct */
  timer->cache.unit = unit;


  INTLOCK();


  /* Remove timer from timer list, if any */

  timer_remove( timer );


  /* Determine when timer should expire, and set reload */

  timer->start  = timetick_get_safe();
  timer->expiry = timer->start + time;
  timer->reload = reload;


  /* Insert timer in either active timer list, or group timer list if it
     belongs to a disabled timer group */

  if ( timer->group->disabled )
  {
    /* Timer belongs to a disabled group - add it to that group's list */
    timer_insert( &timer->group->list, timer );
  }
  else
  {
    /* Timer is not a member of a disabled group - add to active list */
    timer_insert( &timers.active.list, timer);

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt();
  }

  INTFREE();

} /* timer_set */



/*=============================================================================

FUNCTION TIMER_EXPIRES_IN

DESCRIPTION
  Get number of ticks before timer expires

DEPENDENCIES
  Timer must be active (on a timer list)
  Internal function, called from INTLOCK context.

RETURN VALUE
  Number of ticks before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring.

SIDE EFFECTS
  None

=============================================================================*/

static timetick_type timer_expires_in
(
  /* Timer to get */
  timer_ptr_type                  timer
)
{
  /* Current slow clock tick count */
  timetick_type                   now;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current slow clock count value */

  now = timetick_get_safe();


  /* Determine if the timer has expired or not */

  if ( timer->expiry - timer->list->zero  >=  now - timer->list->zero )
  {
    /* Timer has not expired - compute time till expiry. */

    return timer->expiry - now;
  }
  else
  {
    /* Timer has expired - return zero */

    return 0;
  }

} /* timer_expires_in */



/*=============================================================================

FUNCTION TIMER_GET

DESCRIPTION
  Get number of ticks before timer expires

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get
(
  /* Timer to get */
  timer_ptr_type                  timer,

  /* Unit to measure "ticks before timer expires" in. */
  timer_unit_type                 unit
)
{
  /* Ticks until timer expires */
  timetick_type                   ticks;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer != NULL );
  

  INTLOCK();

  if ( timer->list != NULL )
  {
    /* Timer is active */

    ticks = timer_expires_in( timer );
  }
  else
  {
    /* Timer is not active */

    ticks = 0;
  }

  INTFREE();

  return timetick_cvt_from_sclk( ticks, unit);

} /* timer_get */



/*=============================================================================

FUNCTION TIMER_CLR

DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks remaining before expiry.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_clr
(
  /* Timer to stop */
  timer_ptr_type                  timer,

  /* Unit to measure "ticks before expiry" in.  Use T_NONE if not required */
  timer_unit_type                 unit
)
{
  /* Ticks until timer expires */
  timetick_type                   ticks;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer != NULL );
  if ( timer->group == NULL )
  {
    ERR("timer_def was not called for timer %x", timer, 0, 0);
    timer->group = &timer_null_group;
  }

  INTLOCK();

  if ( timer->list != NULL )
  {
    /* Timer is active - record remaining time and remove timer */

    ticks = timer_expires_in( timer );
    timer_remove( timer );

    /* Timer is not running */
    timer->expiry = 0;

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt( );
  }
  else
  {
    /* Timer was not active */

    ticks = 0;
  }

  INTFREE();

  return timetick_cvt_from_sclk( ticks, unit );

} /* timer_clr */



/*=============================================================================

FUNCTION TIMER_PAUSE

DESCRIPTION
  Pauses an active timer.

DEPENDENCIES
  Should only be called for a timer that is running.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_pause
(
  /* Timer to pause */
  timer_ptr_type                  timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( timer != NULL );

  if ( timer->group == NULL )
  {
    ERR("timer_def was not called for timer %x", timer, 0, 0);
    timer->group = &timer_null_group;
  }
  

  INTLOCK();

  if ( timer->list != NULL )
  {
    /* Timer is active - remove it, and record remaining time */

    timer->expiry = timer_expires_in( timer );
    timer_remove( timer );

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt( );
  }

  INTFREE();

} /* timer_pause */



/*=============================================================================

FUNCTION TIMER_RESUME

DESCRIPTION
  Resumes a previously paused timer.

  If a timer was paused 5 seconds before its expiry, and 30 seconds later
  timer_resume( ) is called, the timer will expire 5 seconds after that point.

DEPENDENCIES
  Must only be called for a timer that has been paused.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_resume
(
  /* Timer to resume */
  timer_ptr_type                  timer
)
{
  /* Ticks until timer expires */
  timetick_type                   start;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( timer != NULL );

  if ( timer->group == NULL )
  {
    ERR("timer_def was not called for timer %x", timer, 0, 0);
    timer->group = &timer_null_group;
  }
  

  INTLOCK();

  if ( timer->list == NULL )
  {
    /* Determine when timer should expire */

    start = timetick_get_safe();
    timer->expiry += start;


    /* Insert timer in either active timer list, or group timer list if it
       belongs to a disabled timer group */

    if ( timer->group->disabled )
    {
      /* Timer belongs to a disabled group - add it to that group's list */
      timer_insert( &timer->group->list, timer );
    }
    else
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.active.list, timer);

      /* Active timer list has changed - ensure next timer event is correct */
      timer_update_timer_interrupt();
    }
  }

  INTFREE();

} /* timer_resume */



/*=============================================================================

FUNCTION TIMER_CLR_TASK_TIMERS

DESCRIPTION
  Stops active timers belonging to the given task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_clr_task_timers
(
  /* Timers belonging to this task will be cleared (stopped). */
  rex_tcb_type                    *tcb
)
{
  /* Pointer to a timer group.  Used for walking list of timer groups */
  timer_group_ptr                 group;

  /* Pointer to a timer.  Used for walking list of timers */
  timer_ptr_type                  timer;

  /* Pointer to the next timer.  Used for walking list of timers */
  timer_ptr_type                  next;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  INTLOCK();


  /* Walk list of timer groups, starting with the faux active group */

  group = &timers.active;

  do
  {
    /* Walk the list of timers in this group, and
       remove timers which belong to given task */

    for( timer = group->list.first;  timer != NULL;  timer = next)
    {
      /* Save next timer in list, in case this timer's next field in NULL'd */
      next = timer->next;

      if ( timer->tcb == tcb)
      {
        /* Remove the timer from the list */
        timer_remove( timer );

        /* Timer is no longer running */
        timer->expiry = 0;
      }
    }

    /* Determine the next timer group in the chain ... stopping when we
       return to the start of the list with the faux timer group */

    group = group->next;
  }
  while ( group != &timers.active );


  /* Active timer list may have changed - ensure next timer event is correct */
  timer_update_timer_interrupt( );


  INTFREE();

} /* timer_clr_task_timers */



/*=============================================================================

FUNCTION TIMER_GET_SCLK_TILL_EXPIRY

DESCRIPTION
  Returns the number of sclks until the expiry of next timer to expire.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an INTLOCK'd context.

RETURN VALUE
  Number of sclk's until the next timer expiry.

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get_sclk_till_expiry( void )
{
  /* Current slow clock count */
  timetick_type                   ticks_now;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time */
  ticks_now = timetick_get_safe();

  if ( timers.set_value - timers.set_time > ticks_now - timers.set_time )
  {
    /* Compute & return the time until the timer is set to expire */
    return timers.set_value - ticks_now;
  }
  else
  {
    /* The next timer interrupt is already pending! */
    return 0;
  }

} /* timer_get_sclk_till_expiry */



/*=============================================================================

FUNCTION TIMER_SET_WAKEUP

DESCRIPTION
  This function sets the sclk timer interrupt to expire in advance of the
  the next expiring timer's expiry point, to allow for TCXO to turn back on.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an INTLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_set_wakeup
(
  /* Wakeup the CPU in the given number of sclk's. */
  timetick_type                   wakeup_sclks
)
{
  /* Current slow clock count */
  timetick_type                   ticks_now;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time */
  ticks_now = timetick_get_safe();

  /* Program the timer */
  timer_set_next_interrupt( ticks_now + wakeup_sclks, ticks_now );

} /* timer_set_wakeup */


