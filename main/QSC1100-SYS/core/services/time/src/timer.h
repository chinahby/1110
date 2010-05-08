#ifndef REX_H
/* Circular #include: "rex.h" includes "timer.h", which includes "rex.h".
   Ensure first half of "rex.h" is included before "timer.h" processed. */
#include "rex.h"
#endif /* REX_H */

#ifndef TIMER_H
#define TIMER_H
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

  timer_set_modem_app_sclk_offset(delta)
    Sets the modem/app sclk counter difference.  Call only from timetick.c


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


Copyright (c) 2003 - 2006 by QUALCOMM Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/timer.h#19 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/04/06   trc     extern "C" {} our API to avoid name mangling for C++ clients
02/22/06   ajn     SCLK offset now applies for both modem & apps processors.
02/10/06   kap/ajn Added Timer Optimization Changes (double linking/binning)
11/01/05   ajn     Added cache initialization to TIMER_DEF/TIMER_DEF2 macros.
10/10/05   ajn     Added TIMER_EXPIRES_AT() macro
07/06/05   ajn     Added TIMER_IS_ACTIVE() macro
07/06/05   ajn     Added timer_set_modem_app_sclk_offset (7500 APPS)
05/06/05   ajn     Cache time/reload values to avoid recalculating if unchanged
03/29/05   ajn     Replaced timer_ptr with timer_ptr_type, for L4 project.
10/20/04   ajn     Added a timer task, for timer expiry
09/28/04   ajn     Added "timer_null_group" for timers declared in NULL group
08/30/04   ajn     Added "get next interrupt" and "interrupt early" functions
                   for TCXO shutdown.
08/11/03   ajn     Added timer_pause/timer_resume for rex_set_timer replacement
08/08/03   ajn     Updated for coding standards
08/06/03   ajn     Added timer_def2(), for clk_reg() replacement
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.h
07/17/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "timetick.h"

#ifdef __cplusplus
/* Avoid name mangling for any C++ customers */
extern "C" {
#endif

/*=============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Timer 'type 1' callback arbitrary data type
-----------------------------------------------------------------------------*/

typedef unsigned long             timer_cb_data_type;

/*-----------------------------------------------------------------------------
  Timer 'type 1' callback function
-----------------------------------------------------------------------------*/

typedef void (*timer_t1_cb_type)
(
  /* Arbitrary data given to timer_def( ) to pass to this callback function */
  timer_cb_data_type              data
);


/*-----------------------------------------------------------------------------
  Timer 'type 2' callback function (for clock callback style interface)
-----------------------------------------------------------------------------*/

typedef void (*timer_t2_cb_type)
(
  /* Time elapsed from timer setting (start) */
  int32                           time_ms,

  /* Arbitrary data given to timer_def2( ) to pass to this callback function */
  timer_cb_data_type              data
);



/*-----------------------------------------------------------------------------
  Timer structure pointer types
-----------------------------------------------------------------------------*/

typedef struct timer_group_struct *timer_group_ptr;
typedef struct timer_list_struct  *timer_list_ptr;
typedef struct timer_struct       *timer_ptr_type;
typedef struct timer_bin_struct   *timer_bin_ptr;

#ifndef FEATURE_L4
/* The L4 project has issues where timer_ptr is used other than as a typedef.
   For other projects, temporarily define timer_ptr a migration aid. */
#define timer_ptr                 timer_ptr_type
#endif /* !FEATURE_L4 */


/*-----------------------------------------------------------------------------
  Timer list structure type
    Values in this structure are for private use by "timer.c" only.
-----------------------------------------------------------------------------*/

typedef struct timer_list_struct
{
  /* Slow clock "zero" base count for timer list */
  timetick_type                   zero;

  /* List of timers */
  timer_ptr_type                  first;

  /* End of that list */
  timer_ptr_type                  last;
}
timer_list_type;


/*-----------------------------------------------------------------------------
  Timer group structure type
    Values in this structure are for private use by "timer.c" only.
-----------------------------------------------------------------------------*/

typedef struct timer_group_struct
{
  /* Timers in disabled timer group */
  timer_list_type                 list;

  /* Disabled flag for the timer group */
  boolean                         disabled;

  /* Link to next timer group */
  timer_group_ptr                 next;
}
timer_group_type;


/*-----------------------------------------------------------------------------
  Timer cache structure type
    Values in this structure are for private use by "timer.c" only.
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Original set duration */
  timetick_type                   duration;

  /* Converted set duration */
  timetick_type                   duration_sclk;

  /* Original reload increment */
  timetick_type                   reload;

  /* Converted reload increment */
  timetick_type                   reload_sclk;

  /* Time units */
  timer_unit_type                 unit;
}
timer_cache_type;


/*-----------------------------------------------------------------------------
  Timer structure type
    Values in this structure are for private use by "timer.c" only.
-----------------------------------------------------------------------------*/

typedef struct timer_struct
{
  /* Group to which timer is a member */
  timer_group_ptr                 group;

  /* Task to signal when timer expires, or task to use for APC */
  rex_tcb_type                    *tcb;

  /* Signal(s) to set in task, when timer expires */
  rex_sigs_type                   sigs;

  /* APC (Asynchronous Procedure Call) function to call when timer expires */
  timer_t1_cb_type                func1;

  /* Clock callback style function to call when timer expires */
  timer_t2_cb_type                func2;

  /* Data for APC function, when timer expires */
  timer_cb_data_type              data;

  /* The list the timer is on.  "list" will contain:
       "&active_timers" if the timer is active;
       "&timer->group->list" if the timer is active, but in a disabled group;
       "NULL" if the timer is not active. */
  timer_list_ptr                  list;

  /* Slow clock tick count timer expires at (list != NULL),
     or remaining ticks until expiry if timer is paused (list == NULL) */
  timetick_type                   expiry;

  /* Reload offset when timer expires (0 = no reload at expiry) */
  timetick_type                   reload;

  /* Slow clock tick count value when timer was set (started) */
  timetick_type                   start;

  /* Durations and reload times, original units and converted to sclks */
  timer_cache_type                cache;

  /* Pointer to the next timer in the list (list != NULL) */
  timer_ptr_type                  next;

  /* Pointer to the previous timer in the list (list != NULL) */
  timer_ptr_type                  prev;

  /* Pointer to the bin for this timer */
  timer_bin_ptr                   bin;
}
timer_type;



/*-----------------------------------------------------------------------------
  The NULL timer group, for timers not in any other group
-----------------------------------------------------------------------------*/

/* Timers declared to be in the NULL group are reparented to this group */
extern timer_group_type           timer_null_group;

#if (defined(FEATURE_USE_TIME_VU) && defined(FEATURE_ALWAYS_ON_TIMER_PRESENT))
/*-----------------------------------------------------------------------------
  The AlwaysON timer group, for timers that should not be disabled when doing
  TCXO shutdown.

  Note:  This is a WCDMA addition and should probably NOT be in core timer
         services!  The group declaration belongs somewhere else less global.
         
-----------------------------------------------------------------------------*/
extern timer_group_type           timer_always_on_group;
#endif


/*-----------------------------------------------------------------------------
  Static timer initialization - see timer_def(), timer_def2
-----------------------------------------------------------------------------*/

#define TIMER_DEF( group, task, sigs, func1, data ) \
{ group, task, sigs, func1, NULL, data, NULL, 0, 0, 0, {0,0,0,0,T_NONE}, NULL, NULL, NULL }

#define TIMER_DEF2( group ) \
{ group, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, {0,0,0,0,T_NONE}, NULL, NULL, NULL }



/*=============================================================================

                        REGIONAL FUNCTION DECLARATIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIMER_INIT                                                    REGIONAL

DESCRIPTION
  Initializes the Timer Subsytem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May install an ISR for the 32-bit slow clock match register

=============================================================================*/

void timer_init( void );


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
  dword                           ignored
);

#endif /* FEATURE_TIMER_TASK */


/*=============================================================================

                            FUNCTION DECLARATIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIMER_GROUP_ENABLE

DESCRIPTION
  Enables a timer group.  Timers in the timer group which expired during the
  diabled period will have their expiry processing executed.

DEPENDENCIES
  None

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
);


/*=============================================================================

FUNCTION TIMER_GROUP_DISABLE

DESCRIPTION
  Disables a timer group.  Timers in the timer group will continue to count
  down, but they will not expire.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_group_disable
(
  /* Timer group to be disabled */
  timer_group_ptr                 timer_group
);


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
);



/*=============================================================================

FUNCTION TIMER_DEF2

DESCRIPTION
  Defines a 'clock callback' style timer.

  When the timer expires:
    func(time_ms, data) is called from interrupt context,

  Timers may also be staticly declared via the TIMER_DEF2() macro

DEPENDENCIES
  Timer must be initialized with timer_reg

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
);



/*=============================================================================

FUNCTION TIMER_REG

DESCRIPTION
  Initializes a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

  Timers may also be staticly declared via the TIMER_DEF2() macro

DEPENDENCIES
  Timer must have been defined using timer_def2

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
);



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
);



/*=============================================================================

FUNCTION TIMER_GET

DESCRIPTION
  Get number of ticks before timer expires

DEPENDENCIES
  None

RETURN VALUE
  Number of ticks before timer expires

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timer_get
(
  /* Timer to get */
  timer_ptr_type                  timer,

  /* Unit to measure "ticks before timer expires" in. */
  timer_unit_type                 unit
);


/*=============================================================================

MACRO TIMER_IS_ACTIVE

DESCRIPTION
  Queries whether a timer is active or not.

  A timer which has been set to expire at a certain time - even if the group it
  is a member of is disabled - is considered to be active.  A timer which is
  explicitly paused is not active, until it has been resumed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if the timer is active.
  FALSE if the timer is not active.

SIDE EFFECTS
  None

=============================================================================*/

#define TIMER_IS_ACTIVE(timer)  ( (timer)->list != NULL )



/*=============================================================================

MACRO TIMER_EXPIRES_AT

DESCRIPTION
  Returns the sclk counter value when the timer will expire at.
  See: timetick_get()

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  sclk counter value corresponding to timer expiry point.

SIDE EFFECTS
  None

=============================================================================*/

#define TIMER_EXPIRES_AT(timer)  ( (timer)->expiry )



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
);


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
);



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
);



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
);


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

timetick_type timer_get_sclk_till_expiry( void );


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
);



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
  /* Difference between application and modem processor's sclk counters. */
  timetick_type                   sclk_delta
);

#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* TIMER_H */


