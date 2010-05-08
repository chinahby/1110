#ifndef TIMETICK_H
#define TIMETICK_H
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS

  timetick_set_modem_app_sclk_offset(delta)
    Sets the modem/app sclk counter difference.  Call only from time*.c


EXTERNALIZED FUNCTIONS

  timetick_get
    Returns the current SLEEP_XTAL_TIMETICK counter's count.

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

$Header: //depot/asic/msmshared/services/time/timetick.h#9 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/11/07   ebb     Added timetick_get_sclk64 API which manages 32-bit rollover
06/02/07   grl     Added sclk conversion function with precision and range.
02/22/06   ajn     SCLK offset now applies for both modem & apps processors.
07/06/05   ajn     Added timetick_set_modem_app_sclk_offset (7500 APPS)
04/02/05   ddh     Moved conversion functions from sclk to here
04/30/04   ajn     Added fast monotonic millisecond get function
08/08/03   ajn     Updated for coding standards
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.h
07/16/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Nominial slow clock freq in hertz.
-----------------------------------------------------------------------------*/
#ifndef TIMETICK_NOMINAL_FREQ_HZ
#define TIMETICK_NOMINAL_FREQ_HZ      32768uL
#endif /* TIMETICK_NOMINAL_FREQ */

/*-----------------------------------------------------------------------------
  Time units for setting timetick diffs, timers, etc
-----------------------------------------------------------------------------*/

typedef enum
{
  T_SCLK,
  T_USEC,
  T_MSEC,
  T_SEC,
  T_MIN,
  T_HOUR,

  /* Identifier to use if no return value from timer_clr/get is needed */
  T_NONE=T_SCLK
}
timer_unit_type;


/*-----------------------------------------------------------------------------
  Time, in slow clock counts, from 0 to 0xFFFFFFFF (~1.5 days)
-----------------------------------------------------------------------------*/

typedef uint32                    timetick_type;


/*=============================================================================

                            MACRO DEFINITIONS

=============================================================================*/

#define TIMETICK_SCLK_FROM_MS(ms) ((ms)*TIMETICK_NOMINAL_FREQ_HZ/1000)
  /* Conversion from milliseconds to slow clocks at the nominal frequency */

#define TIMETICK_SCLK_FROM_US(us) ((us)*TIMETICK_NOMINAL_FREQ_HZ/1000000)
  /* Conversion from microseconds to slow clocks at the nominal frequency */




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
);


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
);



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
);



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

timetick_type timetick_get_safe( void );


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

timetick_type timetick_get( void );


#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
/*=============================================================================

FUNCTION TIMETICK_GET_RAW                                              REGIONAL

DESCRIPTION
  Read the SLEEP_XTAL_TIMETICK counter, and return the current count.

DEPENDENCIES
  Regional function.  It may only be called from time*.c
  Must be called from an INTLOCK'd context.

RETURN VALUE
  Slow clock timetick counter value

SIDE EFFECTS
  None

=============================================================================*/

timetick_type timetick_get_raw( void );

#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */


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
);


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
);


/*=============================================================================

FUNCTION TIMETICK_GET_MS

DESCRIPTION
  Returns a monotonically increasing millisecond count, which rolls over
  after 49.7 days.

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

timetick_type timetick_get_ms( void );


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

void timetick_init_sclk64( void );


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

uint64 timetick_get_sclk64( void );


/*=============================================================================

FUNCTION TIMETICK_UPDATE_FREQ

DESCRIPTION
  Stores the sclk frequency in Hz for use during timetick calculations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates timetick_sclk.freq information

=============================================================================*/

void timetick_update_freq
(
  /* The new slow clock frequency (in Hz) */
  uint32                          freq
);


#if defined( IMAGE_APPS_PROC ) || defined( FEATURE_POWER_COLLAPSE )
/*=============================================================================

FUNCTION TIMETICK_SET_SCLK_OFFSET                                      REGIONAL
 
DESCRIPTION

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
);


#endif /* IMAGE_APPS_PROC || POWER_COLLAPSE */


#if !defined( IMAGE_APPS_PROC )  &&  defined( FEATURE_POWER_COLLAPSE )
#error code not present
#endif /* !IMAGE_APPS_PROC && POWER_COLLAPSE */


#endif /* TIMETICK_H */

