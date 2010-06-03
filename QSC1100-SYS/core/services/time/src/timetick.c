/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


EXTERNALIZED FUNCTIONS
  timetick_get
    Returns the current SLEEP_XTAL_TIMETICK counter's count.

  timetick_init_sclk64
    Initialize 64-bit pseudo-timetick counter system

  timetick_get_sclk64
    Retrieve the 64-bit pseudo-timetick counter

  timetick_get_safe
    Returns the current SLEEP_XTAL_TIMETICK counter's count.
    Must be called from an INTLOCK'd context.

  timetick_get_diff
    Determines the time between two timeticks, in slow clocks, milliseconds,
    seconds, etc.

  timetick_get_elapsed
    Determines the time between a past timetick and now, in slow clocks,
    milliseconds, seconds, etc.

  timetick_get_ms
    Returns a monotonically increasing millisecond count, that is not
    related to system time or time-of-day.  Speed, not accuracy, is
    the focus of this function.

  timetick_cvt_to_sclk
    Converts a time value from seconds, milliseconds, etc to slow clocks

  timetick_cvt_from_sclk
    Converts a time value from slow clocks to seconds, milliseconds, etc

  timetick_sclk_to_prec_us
    Converts sclks to microseconds with precision and full range

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None


Copyright (c) 2003 - 2007 by QUALCOMM Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/timetick.c#14 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/11/07   ebb     Added timetick_get_sclk64 API which manages 32-bit rollover
06/02/07   grl     Added sclk conversion function with precision and range.
02/22/06   ajn     Add offset btw modem & MPM sclk counter for power collapse
12/20/05   ejv     Include msm.h in place of msm_drv.h.
11/18/05   ajn     Corrected sclk-to-millisecond rounding error.
07/06/05   ajn     Added timetick_set_modem_app_sclk_offset (7500 APPS)
06/24/05   ajn     Include partial 7500 support
03/22/05   ddh     Adding sclk functions for timetick to use.
10/14/04   ajn     Added explicit "case T_SCLK:" in cvt_to/from_sclk functions.
08/24/04   ajn     Eliminated SCLK_TCXO option from sclk_*() calls
04/30/04   ajn     Added fast monotonic millisecond get function
08/08/03   ajn     Updated for coding standards
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.c
07/16/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "timetick.h"
#include "msg.h"
#include "msm.h"
#include "sclk.h"
#include "timer.h"
#include "err.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Microsecond threshold for slow clock
     50000 us * 60 kHz < 2^32 micro-cycles
-----------------------------------------------------------------------------*/
#define TIMETICK_US_THRESHOLD     50000uL

/*-----------------------------------------------------------------------------
  Millisecond threshold for slow clock
     60000 ms * 60 kHz < 2^32 milli-cycles
-----------------------------------------------------------------------------*/
#define TIMETICK_MS_THRESHOLD     60000uL
 
/*-----------------------------------------------------------------------------
  Optimal Slow Clock frequency 32768 Hz = 2^15 
-----------------------------------------------------------------------------*/
#define OPTIMAL_SLOW_CLOCK_FREQ   32768uL

/* Interval at which to update 64-bit rollover offset */
#define TIMETICK64_UPDATE_INCR    (1uL<<31)

/* Information used when acquiring the millisecond value */ 
typedef struct
{
  /* SCLK origin for millisecond tick measurements */
  uint32                          sclk;

  /* Millisecond count at sclk point */
  uint32                          msec;
}
timetick_ms_info_struct_type;


/* Info used for sclk conversions */
typedef struct
{
  /* SCLK frequency in Hz */
  uint32                          freq;
}
timetick_sclk_cvt_struct_type;


/* Static variables for the file */
typedef struct
{
  /* Info used to get msec */
  timetick_ms_info_struct_type    ms_info;

#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
  /* Difference between app, modem & MPM slow clock counters */
  timetick_type                   sclk_delta;
#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */

  /* Info used sclk conversion */
  timetick_sclk_cvt_struct_type   sclk;

  /* Overflow offset to maintain 64-bit timetick */
  uint64                          sclk64_overflow;

  /* Timer to track sclk overflows*/
  timer_type                      sclk64_overflow_timer;

  /* A timer group to ensure overflow_timer is never disabled. */
  timer_group_type                sclk64_overflow_timer_group;

  /* Flag to indicate 64-bit timetick has been initialized */
  boolean                         sclk64_initialized;

  /* Timetick base value for 64-bit computation */
  timetick_type                   sclk64_base;

}
timetick_struct_type;


/* Static data for timetick module */
static timetick_struct_type       timetick =
{
  /* timetick_ms_info_struct_type   ms_info */
  { 0, 0 },

#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
  /* Difference between app & modem slow clock counters */
  0,
#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */

  /* timetick_sclk_cvt_struct_type  sclk */
  { 0 },

  /* Overflow offset to maintain 64-bit timetick */
  0,

  /* Timer to track sclk overflows*/
  { NULL },

  /* A timer group to ensure overflow_timer never disabled. */
  { { NULL }, 0, 0 },

  /* Flag to indicate 64-bit timetick has been initialized */
  FALSE,

  /* Timetick base value for 64-bit computation */
  0

};



/*=============================================================================

                           MACRO DEFINITIONS

=============================================================================*/

#if defined(T_MSM7500) && defined(IMAGE_APPS_PROC)
#error code not present
#else
  #define READ_TIMETICK_COUNT()   HWIO_IN( SLEEP_XTAL_TIMETICK_COUNT )
#endif



/*=============================================================================

                   INTERNAL FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION CVT_SCLK_TO_MS

DESCRIPTION
  Converts the given slow clock value to milliseconds

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of milliseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint32 cvt_sclk_to_ms
(
  uint32      sclks
    /* Duration to be converted into milliseconds */
)
{
  uint32                          milliseconds = 0;
    /* Seconds portion of duration */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( sclks >= timetick.sclk.freq )
  {
    /* Compute seconds portion of time period first, to avoid overflows */
    milliseconds  = (sclks / timetick.sclk.freq) * 1000;
    sclks        %= timetick.sclk.freq;
  }

  milliseconds += (sclks * 1000 + timetick.sclk.freq/2) / timetick.sclk.freq;
    /* Convert remaining sclks to milliseconds, with rounding. */

  return milliseconds;

} /* cvt_sclk_to_ms */


/*=============================================================================

FUNCTION CVT_MS_TO_SCLK

DESCRIPTION
  Converts the given millisecond amound to a slow clock value

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_ms_to_sclk
(
  uint32      msecs
    /* Duration to be converted into slow clocks */
)
{
  uint32      secs;
    /* Seconds portion of duration */

  uint32      sclks;
    /* Slow clock duration */

/*--------------------------------------------------------------------------*/

  /* If duration exceeds 1 minute, compute slow clock duration in two
     parts, seconds and milliseconds, to avoid uint32 overflow in "ms*f" */

  if ( msecs >= TIMETICK_MS_THRESHOLD )
  {
    secs   = msecs / 1000;
    msecs %= 1000;
    sclks = secs * timetick.sclk.freq  +  msecs * timetick.sclk.freq / 1000;
  }
  else
  {
    sclks = msecs * timetick.sclk.freq / 1000;
  }

  return (sclks);

} /* cvt_ms_to_sclks */


/*=============================================================================

FUNCTION CVT_US_TO_SCLKS

DESCRIPTION
  Converts the given microsecond value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

static uint32 cvt_us_to_sclk
(
  uint32      microseconds
    /* Duration to be converted into slow clocks */
)
{
  uint32                          milliseconds;
    /* Milliseconds portion of duration */

  uint32                          sclks;
    /* Duration in slow clocks */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If duration exceeds 50 miliseconds, compute slow clock duration
     in two parts, ms and us, to avoid uint32 overflow in "us*f".
     50 miliseconds was chosen as a boundary to avoid overflow and
     still accomodate for slow clock frequencies allowed in the
     specification (30-60 kHz) */

  if ( microseconds >= TIMETICK_MS_THRESHOLD )
  {
    milliseconds  = microseconds / 1000;
    microseconds %= 1000;
    sclks = cvt_ms_to_sclk( milliseconds ) +
           (microseconds * timetick.sclk.freq) / 1000000;
  }
  else
  {
    sclks = (microseconds * timetick.sclk.freq) / 1000000;
  }

  return (sclks);

} /* cvt_us_to_sclk */


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIMETICK_CVT_TO_SCLK

DESCRIPTION
  Converts the given time value to slow clocks

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of slow clocks which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_to_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /* Convert hours to minutes */
      time *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Convert minutes to seconds */
      time *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Seconds to slow clocks */
      time *= timetick.sclk.freq;
      break;

    case T_MSEC:
      /* Milliseconds to slow clocks */
      time = cvt_ms_to_sclk( time );
      break;

    case T_USEC:
      /* Microseconds to slow clocks */
      time = cvt_us_to_sclk( time );
      break;

    case T_SCLK:
      /* time = time; */
      break;

    default:
      MSG_ERROR("Invalid timetick conversion %d", unit, 0, 0);
      break;
  }

  return time;

} /* timetick_cvt_to_sclk */



/*=============================================================================

FUNCTION TIMETICK_SCLK_TO_PREC_US

DESCRIPTION
  Converts the given slow clock value to precision microseconds. This function
  also supports the full range of sclk values.

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  # of microseconds which corresponds to the given time value

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_sclk_to_prec_us
(
  uint32      sclks
    /* Duration in sclks to be converted into microseconds */
)
{
  uint64      microseconds = 0;
    /* Duration converted to microseconds */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  microseconds = ((uint64)sclks * 1000000) / (uint64)timetick.sclk.freq;
    /* Calculate the duration in microseconds. */

  return microseconds;

} /* timetick_sclk_to_prec_us */



/*=============================================================================

FUNCTION TIMETICK_CVT_FROM_SCLK

DESCRIPTION
  Converts the slow clock time value to the given unit

DEPENDENCIES
  Valid sclk estimate

RETURN VALUE
  Time in the unit requested

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_cvt_from_sclk
(
  /* Time interval to convert to slow clocks */
  timetick_type                   time,

  /* Units of time interval */
  timer_unit_type                 unit
)
{
  /* Factor for converting to hours or minutes */
  uint32                          scale = 1;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( unit )
  {
    case T_HOUR:
      /*  Scale minutes to hours */
      scale *= 60;
      /*lint -fallthrough */

    case T_MIN:
      /* Scale seconds to minutes */
      scale *= 60;
      /*lint -fallthrough */

    case T_SEC:
      /* Convert slow clocks to seconds (or minutes, or hours) */
      time /= timetick.sclk.freq * scale;
      break;

    case T_MSEC:
      /* Convert slow clocks to milliseconds */
      time = cvt_sclk_to_ms( time );
      break;

    case T_USEC:
      /* Convert slow clocks to microseconds */
      time = cvt_sclk_to_ms( time * 10 ) * 100;
      break;

    case T_SCLK:
      /* time = time; */
      break;

    default:
      MSG_ERROR("Invalid timetick conversion %d", unit, 0, 0);
      break;
  }

  return time;

} /* timetick_cvt_from_sclk */



#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
/*=============================================================================

FUNCTION TIMETICK_GET_RAW

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  Regional function.  It may only be called from time*.c

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_raw( void )
{
  /* Sleep Xtal Time Tick count */
  timetick_type                   tick;

  /* Last value of "timetick" ... used to verify value read is stable */
  timetick_type                   last_tick;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Reading the counter value once may not return an accurate value if the
     counter is in the processing of counting to the next value, and several
     bits are changing.  Instead, the counter is be repeatedly read until a
     consistant value is read.  */

  tick = (timetick_type) READ_TIMETICK_COUNT();

  do {
    last_tick = tick;
    tick = (timetick_type) READ_TIMETICK_COUNT();
  } while (tick != last_tick);


  /* The counter has returned the same value twice in a row, and hence must
     be stable. */

  return tick;

} /* timetick_get_raw */

#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */



/*=============================================================================

FUNCTION TIMETICK_GET_SAFE

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  Must be called from an INTLOCK'd context.
  Use TIMETICK_GET from non-INTLOCK'd context

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_safe( void )
{
  /* Sleep Xtal Time Tick count */
  timetick_type                   tick;

  /* Last value of "timetick" ... used to verify value read is stable */
  timetick_type                   last_tick;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Reading the counter value once may not return an accurate value if the
     counter is in the processing of counting to the next value, and several
     bits are changing.  Instead, the counter is repeatedly read until a
     consistant value is read. */

  tick = (timetick_type) READ_TIMETICK_COUNT();

  do {
    last_tick = tick;
    tick = (timetick_type) READ_TIMETICK_COUNT();
  } while (tick != last_tick);


  /* The counter has returned the same value twice in a row, and hence must
     be stable. */


#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
  /* Add the difference between the app, modem & MPM sclk counters */
  tick += timetick.sclk_delta;
#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */


  return tick;

} /* timetick_get_safe */



/*=============================================================================

FUNCTION TIMETICK_GET

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  None.

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get( void )
{
  /* Sleep Xtal Time Tick count */
  timetick_type                   tick;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Provide an intlock'd context for reading the timetick counter */

  INTLOCK();

  tick = timetick_get_safe();

  INTFREE();

  return tick;

} /* timetick_get */


/*=============================================================================

FUNCTION TIMETICK64_UPDATE_OVERFLOW

DESCRIPTION
  This function is called when timetick64_overflow_timer is triggered, causing 
  the overflow count to be incremented.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  timetick.sclk64_overflow gets incremented to maintain 64-bit sclk

=============================================================================*/

static void timetick64_update_overflow
(
  timer_cb_data_type              unused_data
)
{
  unused_data = unused_data;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Ensure no context switch occurs */
  INTLOCK();

  /* Update the 64-bit overflow offset */
  timetick.sclk64_overflow += TIMETICK64_UPDATE_INCR;

  /* End critical section */
  INTFREE();

}


/*=============================================================================

FUNCTION TIMETICK_INIT_SCLK64

DESCRIPTION
  This function initializes the 64-bit timetick system by defining a timer to
  track the rollover of the slow clock. This should ONLY be called once upon
  boot, preferably before the first rollover of the slock clock (~36.5 hrs.)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  After this is called timetick_get_sclk64 can be used.

=============================================================================*/

void timetick_init_sclk64( void )
{
  /* Only initialize the 64 bit timetick system once */
  if ( timetick.sclk64_initialized != TRUE )
  {
    /* Create a timer to update overflow before the TimeTick rollover */
    timer_def( &timetick.sclk64_overflow_timer,
               &timetick.sclk64_overflow_timer_group,
               NULL,
               0,
               timetick64_update_overflow,
               NULL);

    /* Enable the timer group (forever) */
    timer_group_enable(&timetick.sclk64_overflow_timer_group);

    /* Set the timer to go off TIMETICK64_UPDATE_INCR in the future, and
       at TIMETICK64_UPDATE_INCR increments thereafter */
    timer_set( &timetick.sclk64_overflow_timer, TIMETICK64_UPDATE_INCR, 
               TIMETICK64_UPDATE_INCR, T_SCLK);

    /* Reset the overflow accumulator */
    timetick.sclk64_overflow = 0uLL;

    /* Get the start timetick for the rollover timer */
    timetick.sclk64_base = timetick.sclk64_overflow_timer.start;

    /* The 64-bit timetick system is now initialized */
    timetick.sclk64_initialized = TRUE;
  }
}


/*=============================================================================

FUNCTION TIMETICK_GET_SCLK64

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter. Adds the rollover offset to provide 
  64 bit dynamic range.

DEPENDENCIES
  TIMETICK_INIT_SCLK64 must be called first to initialize the timer.

RETURN VALUE
  64-bit tick count

SIDE EFFECTS
  None

=============================================================================*/

uint64 timetick_get_sclk64( void )
{
  /* Current timetick */
  timetick_type         now;

  /* timetick in 64 bits */
  uint64                sclk64;

  /* - - - - - - - - - - - -- - - - - - - - - - - -- - - - - - - - - - - - - */

  /* Check to ensure we've been initialized */
  if( !timetick.sclk64_initialized )
  {
    ERR_FATAL("timetick_sclk64 has not been initialized!", 0, 0, 0);
  }
 
  /* We have to be a little tricky here to ensure that there are no glitches
     due to the rollover timer callback possibly being called at a later
     timetick than its scheduled one.  About the only thing we can depend
     upon is the back-to-back duration of the timer itself, with only the
     expiry point having any 'jitter'.  For this reason, we cannot allow
     the overflow timer callback to run during our calculations, and prevent
     such hazard via an INTLOCK/critical-section. */
  INTLOCK();

  /* Get the current timetick minus the original overflow timer's start base.
     This will result in a 32-bit timetick value referenced to 0 with respect
     to the overflow accumulator timer callback. */
  now = timetick_get_safe() - timetick.sclk64_base;

  /* Calculate 64 bit timetick based on overflow accumulator.  Note that we
     OR the 0-based timetick and the overflow accumulator to account for the
     31st bit possibly being set in both the timetick and overflow accumulator
     values. */
  sclk64 = timetick.sclk64_overflow | (uint64)now;

  /* Add back the timer start base so that the lower-32 bits of the 64-bit
     timestamp match the full 32 bits of the regular timestamp. */
  sclk64 += (uint64)timetick.sclk64_base;

  /* Account for the race condition where this could be called prior to 
     the overflow timer callback being called.  Basically, if the 0-based
     timetick is less than the timer duration and we're still in the
     second phase of the two-phase per rotation timer overflow callback, there
     is one update increment missing from the overflow accumulator. */
  if ( (timetick.sclk64_overflow & (uint64)TIMETICK64_UPDATE_INCR) &&
       (now < TIMETICK64_UPDATE_INCR) )
  {
    sclk64 += (uint64)TIMETICK64_UPDATE_INCR;
  }

  /* End critical section */
  INTFREE();

  /* Return 64-bit sclk */
  return sclk64;

}



/*=============================================================================

FUNCTION TIMETICK_GET_ELAPSED_TIME

DESCRIPTION
  Compute the time elapsed from a previous timetick value

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  Elapsed time, in the unit provided

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_elapsed
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Units to return time interval in */
  timer_unit_type                 unit
)
{
  /* Sleep Xtal Time Tick count */
  timetick_type                   end;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current timetick count value for the end of the interval */
  end = timetick_get();

  /* Compute and return the difference between the timestamps */
  return timetick_diff(start, end, unit);

} /* timetick_get_elapsed */



/*=============================================================================

FUNCTION TIMETICK_DIFF

DESCRIPTION
  Compute the difference between two slow clock tick counts

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  Time difference between the two slow clock tick counts, in the unit given

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_diff
(
  /* Time tick value at start of interval */
  timetick_type                   start,

  /* Time tick value at end of interval */
  timetick_type                   end,

  /* Units to return time interval in */
  timer_unit_type                 unit
)
{
  /* Sleep Xtal Time Tick count */
  timetick_type                   delta;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Compute the difference between the timetick timestamps */
  delta = end - start;

  /* Compute and return the difference between the timestamps */
  return timetick_cvt_from_sclk( delta, unit);

} /* timetick_get_elapsed */



/*=============================================================================

FUNCTION TIMETICK_GET_MS

DESCRIPTION
  Returns a monotonically increasing millisecond count.

  The value returned is unrelated to CDMA, GPS, HDR, or Time of Day
  time-stamps.  It will drift with respect to the above time references.

  *** This function sacrifices accuracy for speed ***

DEPENDENCIES
  None

RETURN VALUE
  Millisecond tick count

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_ms( void )
{
  /* Current time (or delta) in sclk's */
  timetick_type                   sclk;

  /* Seconds since origin time */
  timetick_type                   secs;

  /* Milliseconds since origin time */
  timetick_type                   msecs;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  INTLOCK();

  /* Get the elapsed time since the arbitrary origin */
  sclk = timetick_get_safe() - timetick.ms_info.sclk;

  #if ( TIMETICK_NOMINAL_FREQ_HZ == OPTIMAL_SLOW_CLOCK_FREQ )
  

  /*** Optimized for slow clock frequency of 32768 Hz = 2^15 ***/

  /* Compute number of seconds that have elapsed. */
  secs = sclk >> 15;

  /* If over a second has elapsed, advance origin. */
  if ( secs > 0 )
  {
    timetick.ms_info.sclk += secs << 15;
    timetick.ms_info.msec += secs * 1000;
  }

  /* Compute millisecond timestamp */
  msecs = (((sclk & 0x7FFF) * 1000) >> 15) + timetick.ms_info.msec;

  #else /* TIMETICK_NOMINAL_FREQ_HZ != 32768 */

  /* Compute number of seconds that have elapsed. */
  secs = sclk / TIMETICK_NOMINAL_FREQ_HZ;

  /* If over a second has elapsed, advance origin. */
  if ( secs > 0 )
  {
    sclk                  -= secs * TIMETICK_NOMINAL_FREQ_HZ;
    timetick.ms_info.sclk += secs * TIMETICK_NOMINAL_FREQ_HZ;
    timetick.ms_info.msec += secs * 1000;
  }

  /* Compute millisecond timestamp */
  msecs = sclk * 1000 / TIMETICK_NOMINAL_FREQ_HZ + timetick.ms_info.msec;

  #endif /* TIMETICK_NOMINAL_FREQ_HZ != 32768 */

  INTFREE();

  return msecs;

} /* timetick_get_ms */


/*=============================================================================

FUNCTION TIMETICK_UPDATE_FREQ

DESCRIPTION
  Stores the sclk frequency in Hz for use during timetick calculations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates timetick.ms_info.sclk.freq information

=============================================================================*/

void timetick_update_freq
(
  /* The new slow clock frequency (in Hz) */
  uint32                          freq
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  timetick.sclk.freq = freq;

} /* timetick_update_freq */


#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
/*=============================================================================

FUNCTION TIMETICK_SET_SCLK_OFFSET                                      REGIONAL
 
DESCRIPTION
  Stores the offset between application and modem processors' sclk counters.

  The application and modem processors' sclk h/w counters, while both count
  slow clocks, will have an offset w.r.t. each other, due to application
  processor power collapse.  This difference is eliminated by adding an offset
  to the value read from the application processor's sclk counter.

DEPENDENCIES
  Regional function.  It may only be called from time*.c

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timetick_set_sclk_offset
( 
  /* Difference between application and modem processor's sclk counters. */
  timetick_type                   sclk_delta
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Updating the slow-clock offset in two places - must be atomic */
  INTLOCK();


  /* Update timetick's offset */
  timetick.sclk_delta = sclk_delta;

  /* Update timer's offset - it must match the timetick's new offset */
  timer_set_sclk_offset( timetick.sclk_delta );


  INTFREE();

} /* timetick_set_sclk_offset */


#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */




#if !defined( IMAGE_APPS_PROC )  &&  defined( FEATURE_POWER_COLLAPSE )
#error code not present
#endif /* !IMAGE_APPS_PROC && POWER_COLLAPSE */


