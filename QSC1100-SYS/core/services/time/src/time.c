/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using various sources.


EXTERNALIZED FUNCTIONS
  time_init( )
    Initializes the time services subsystem

  time_to_ms( )
    Converts a time stamp into milliseconds.

  time_get( )
    Retrieves time according to CDMA, HDR, GSM, or slow-clock Time-of-Day

  time_get_ms( )
    Retrieves system time in milliseconds

  time_get_sec( )
    Retrieves system time, in seconds

  time_get_local_sec( )
    Retrieves the time-zone adjusted local time time, in seconds

  time_get_uptime_ms( )
    Retrieves the up-time, in milliseconds

  time_get_uptime_secs( )
    Retrieves the up-time, in seconds

  time_sync_time_of_day( )
    Syncs the current time, from CDMA, HDR, or GSM, to the slow clock based
    time-of-day.

  time_set_from_pmic( )
    Set the time-of-day from the PMIC's RTC (if present)

  time_to_ms_native()
    Convert the current time to milliseconds in native uint64 format.

  time_get_ms_native()
    Get the current time in milliseconds in native uint64 format.    


REGIONAL FUNCTIONS
  time_compute_power_on( )
    Called when a discontinuity of time-of-day might occur, to compute the
    power-on time, so that "up-time" (system_time - power_on_time) may
    be computed.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_init( ) must be called to initalize the time subsystem state variables
  and install necessary ISRs.


Copyright (c) 2003 - 2007 by QUALCOMM Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time.c#16 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/28/07   trc     Protect against preemption in uptime calculations
07/11/07   ebb     Add 64-bit pseudo-timetick service initialization
09/21/06   trc     Changed time_sync_time_of_day() failure to a non-fatal ERR()
05/09/06   hn      Protected references to DB with FEATURE_NO_DB
02/22/06   ajn     Synchronize MPM & Modem SCLK counters on startup.
11/18/05   mod/ajn Added MediaFLO time support.
09/26/05   jhs     Added call to time_remote_init.
09/20/05   ajn/cr  Added GPS based time.
08/11/05   jhs     Determine delta between modem & app sclk counters at init.
06/24/05   ajn     Include partial 7500 support
05/10/05   ajn     Added time_set_from_pmic( ) function, to correct init order.
11/24/04   grl     Added support for getting time in ms in native uint64 types.
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
08/01/03   ajn     Moved _get_sec, get_ms from time_tod.  Added _get_local_sec
07/24/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"

#include "timer.h"
#include "sclk.h"

#ifndef FEATURE_NO_DB
  #include "db.h"
#endif /* ! FEATURE_NO_DB */

#include "err.h"

#if defined(FEATURE_SLOW_CLOCK_MULTIPROC_SYNC) || \
    defined(FEATURE_TOD_MULTIPROC_SYNC)
  #include "time_remote.h"
#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC || FEATURE_TOD_MULTIPROC_SYNC */


/*-----------------------------------------------------------------------------
  Time source includes
-----------------------------------------------------------------------------*/

#if defined(IMAGE_MODEM_PROC) || !defined(T_MSM7500)

#ifdef FEATURE_CDMA1X
#include "time_cdma.h"
#endif /* FEATURE_CDMA1X */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_GSM
#error code not present
#endif /* FEATURE_GSM */

#ifdef FEATURE_GPSONE_PE
#error code not present
#endif

#ifdef FEATURE_MFLO
#error code not present
#endif

#endif /* IMAGE_MODEM_PROC || !T_MSM7500 */

/* Time-of-day */
#include "time_tod.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------

                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Useful constants
-----------------------------------------------------------------------------*/


/* Mask to extract cx32's (lower 16 bits) from timestamp */
#define TIME_CX32_MASK            0x0ffffuL

/* To convert TS to 1.25ms units, shift out cx32 bits of timestamp */
#define TIME_TO_1p25MS_SHIFT      -16

/* Conversion from 1.25ms to 0.25ms */
#define TIME_0p25MS_PER_1p25MS    (125/25)

/* Conversion from cx32's to 0.25ms ticks */
#define TIME_CX32_PER_0p25MS      ((49152+3)/5)

/* Shift from 0.25ms units to 1ms units */
#define TIME_0p25MS_TO_1MS_SHIFT  -2

/* To convert timestamp to 40ms, first shift out cx32 bits of timestamp, and
   then shift down an addition 5, to convert 1.25ms units to 40ms units */
#define TIME_TO_40MS_SHIFT        (TIME_TO_1p25MS_SHIFT-5)

/* # of 40ms units in 1 second (1000ms) */
#define TIME_40MS_PER_SEC         (1000/40)

/* Seconds per TimeZone (30 min/zone x 60sec/min) */
#define TIME_SEC_PER_TIME_ZONE    (30 * 60)

/*-----------------------------------------------------------------------------
  Time static information
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Time when phone powered on, in milliseconds */
  time_type                       power_on_ms;

  /* Time when phone powered on, in seconds */
  uint32                          power_on_sec;
}
time_struct_type;


static time_struct_type           time;



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIME_INIT

DESCRIPTION
  Initialize Timekeeping Subsystem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs timekeeping ISR, timers, etc.

=============================================================================*/

void time_init( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the slow-clock subsystem */
  sclk_init();

  #if !defined( IMAGE_APPS_PROC )  &&  defined( FEATURE_POWER_COLLAPSE )
#error code not present
  #endif /* !IMAGE_APPS_PROC && POWER_COLLAPSE */

  /* Initialize slow-clock timers, and install timer ISR */
  timer_init();

  #ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC
  /* Start sync before we read any timeticks */
  time_remote_slow_clock_sync();
  #endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC */

  /* Initialize 64-bit pseudo-timetick service. */
  timetick_init_sclk64();

  /* Initialize time-of-day subsystem, and roll-over timer */
  time_tod_init();

  #if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
  time_remote_init();
  #endif /* FEATURE_TOD_MULTIPROC_SYNC && IMAGE_MODEM_PROC */

#if defined(IMAGE_MODEM_PROC) || !defined(T_MSM7500)

  #ifdef FEATURE_CDMA1X
  /* Ensure CDMA time is invalid */
  time_cdma_set_valid( FALSE );
  #endif

  #ifdef FEATURE_HDR
#error code not present
  #endif

#endif /* IMAGE_MODEM_PROC || !T_MSM7500 */

  /* Capture the power-on time, in milliseconds, for uptime determination */
  time_get_ms( time.power_on_ms );

} /* time_init */



/*=============================================================================

FUNCTION TIME_TO_MS

DESCRIPTION
  Convert a timestamp from System Time to millisecond units

DEPENDENCIES
  None

RETURN VALUE
  The converted value is stored in the 'time' parameter.

SIDE EFFECTS
  None

=============================================================================*/

void time_to_ms
(
  /* In: Time in time-stamp format;  Out: Time in milliseconds */
  time_type                       ts_val
)
{
  /* Lower 16 bits of the timestamp (chipx32's) */
  uint32                          cx32;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Save lower 16 bits (cx32's) of timestamp */
  cx32 = qw_lo( ts_val ) & TIME_CX32_MASK;

  /* Shift down by 16 to convert to get 1.25ms ticks */
  qw_shift( ts_val, TIME_TO_1p25MS_SHIFT);

  /* Multiply by 5 to convert 1.25ms ticks to 0.25ms ticks */
  qw_mul( ts_val, ts_val, TIME_0p25MS_PER_1p25MS);

  /* Convert cx32's to 0.25ms ticks and add, plus 2 for rounding (below) */
  qw_inc( ts_val, (cx32 / TIME_CX32_PER_0p25MS) + 2);

  /* Divide by 4 to get milliseconds ticks (rounded via +2, above) */
  qw_shift( ts_val, TIME_0p25MS_TO_1MS_SHIFT);

} /* time_to_ms */



/*=============================================================================

FUNCTION TIME_TO_SEC

DESCRIPTION
  Convert a timestamp from System Time to seconds

DEPENDENCIES
  None

RETURN VALUE
  Seconds, corresponding to the time stamp

SIDE EFFECTS
  Input argument value is destroyed.

=============================================================================*/

static uint32 time_to_sec
(
  /* In: Time in time-stamp format;  Out: Garbage */
  time_type                       ts_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Shift down to convert to 40ms ticks */
  qw_shift( ts_val, TIME_TO_40MS_SHIFT);

  /* Divide to convert 40ms ticks to seconds */
  (void) qw_div( ts_val, ts_val, TIME_40MS_PER_SEC);

  /* Return the lower-order word, giving a 136 year range */
  return qw_lo( ts_val );

} /* time_to_sec */



/*=============================================================================

FUNCTION TIME_GET

DESCRIPTION
  Returns the current time

DEPENDENCIES
  A valid CDMA time, HDR time, GSM time, or a Time-of-Day, etc.

RETURN VALUE
  Time-stamp returned to ts_val parameter.
  Time source used to determine time is returned

SIDE EFFECTS
  None

=============================================================================*/

time_source_enum_type time_get
(
  /* OUT: The current time */
  time_type                       ts_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined(IMAGE_MODEM_PROC) || !defined(T_MSM7500)

#ifdef FEATURE_CDMA1X 
  /* Attempt to get CDMA time */

  if ( time_cdma_get(ts_val) == TRUE)
  {
    return TIME_SOURCE_CDMA;
  }
#endif /* FEATURE_CDMA1X */


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_GPSONE_PE
#error code not present
#endif

#ifdef FEATURE_MFLO
#error code not present
#endif

#if 0 /* #ifdef FEATURE_GSM */
  /* Failing that, attempt to get GSM time */

  if ( time_gsm_get(ts_val) == TRUE)
  {
    return TIME_SOURCE_GSM;
  }
#endif /* FEATURE_GSM */

#endif /* IMAGE_MODEM_PROC || !T_MSM7500 */

  /* Failing that, as a last resort, fall back on time-of-day */

  time_tod_get( ts_val );

  return TIME_SOURCE_32KHZ;

} /* time_get */



/*=============================================================================

FUNCTION TIME_GET_MS

DESCRIPTION
  Get the system time, in # of milliseconds since "the beginning of time".

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Milliseconds since 6 Jan 1980 00:00:00 returned to the time parameter

SIDE EFFECTS
  None

=============================================================================*/

void time_get_ms
(
  /* Out: Time in milliseconds since 6 Jan 1980 00:00:00 */
  time_type                       ts_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read the current time of day */
  (void) time_get( ts_val );

  /* Convert time-stamp format to milliseconds */
  time_to_ms( ts_val );

} /* time_get_ms */



/*=============================================================================

FUNCTION TIME_GET_SECS

DESCRIPTION
  Get the system time, in # of seconds since "the beginning of time".
  136 year range, beginning 6 Jan 1980 00:00:00.

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Seconds since 6 Jan 1980 00:00:00

SIDE EFFECTS
  None

=============================================================================*/

uint32 time_get_secs( void )
{
  /* Timestamp */
  time_type                       ts_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read the current time of day */
  (void) time_get( ts_val );

  /* Convert to seconds, and return */
  return time_to_sec( ts_val );

} /* time_get_secs */



/*=============================================================================

FUNCTION TIME_GET_LOCAL_SECS

DESCRIPTION
  Get the local time, in # of seconds since "the beginning of time".
  136 year range, beginning 6 Jan 1980 00:00:00.

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.
  LTM_OFF and LP_SEC must be set in the DB database.

RETURN VALUE
  Seconds since 6 Jan 1980 00:00:00, adjusted for the local time zone.

SIDE EFFECTS
  None

=============================================================================*/

uint32 time_get_local_secs( void )
{
  /* Time, in seconds from 6 Jan 1980 00:00:00 */
  uint32                          sys_time;

#ifndef FEATURE_NO_DB
  /* Time zone offset, in 30-minute units */
  int32                           ltm_off;

  /* Database retrieval storage area */
  db_items_value_type             db;
#endif /* ! FEATURE_NO_DB */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current system time, in seconds */
  sys_time = time_get_secs();


#ifndef FEATURE_NO_DB
  /* Retrieve the current time-zone offset */
  db_get( DB_LTM_OFF, &db );
  ltm_off = db.ltm_off;

  /* ltm_off is a 6-bit 2's complement code. Make it signed integer */
  ltm_off &= 0x3f;
  if (ltm_off > 32)
  {
    ltm_off -= 64;
  }

  /* Convert ltm_off from 30-minute units, to seconds */
  ltm_off *= TIME_SEC_PER_TIME_ZONE;


  /* Retrieve the number of leap seconds that have elapsed */
  db_get( DB_LP_SEC, &db );

  /* Local time = SYS_TIME - LP_SEC + LTM_OFF */
  return sys_time  -  db.lp_sec  +  ltm_off;
#else /* ! FEATURE_NO_DB */
  return sys_time;
#endif /* FEATURE_NO_DB */

} /* time_get_local_secs */



/*=============================================================================

FUNCTION TIME_COMPUTE_POWER_ON                                         REGIONAL

DESCRIPTION
  Compute the power-on time, for uptime determination

DEPENDENCIES
  time_init( ) must have been called, to initialize time.power_on_ms
  May only be called from time_tod_set( ).

RETURN VALUE
  None

SIDE EFFECTS
  Updates time.power_on_ms and time.power_on_sec values.

=============================================================================*/

void time_compute_power_on
(
  /* Time value prior to updating to the correct time */
  time_type                       old_time,

  /* Time value after updating to the correct time value */
  time_type                       new_time
)
{
  /* Time, in milliseconds */
  time_type                       temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Convert the old time value to milliseconds */
  qw_equ( temp, old_time );
  time_to_ms( temp );

  /* Compute difference between when the phone was powered on, and when
     the time is being set. */
  qw_sub( time.power_on_ms, temp, time.power_on_ms);

  /* Convert the new time to milliseconds */
  qw_equ( temp, new_time );
  time_to_ms( temp );

  /* Use the current time, and the time between power on & now,
     to compute the power-on time, in milliseconds */
  qw_sub( temp, temp, time.power_on_ms );

  /* Save the power on time, in milliseconds */
  qw_equ( time.power_on_ms, temp);

  /* Convert power-on time, from milliseconds to seconds */
  (void) qw_div( temp, temp, 1000 );

  /* Save the power on time, in seconds */
  time.power_on_sec = qw_lo( temp );

} /* time_compute_power_on */



/*=============================================================================

FUNCTION TIME_GET_UPTIME_SECS

DESCRIPTION
  Get time the phone has been powered on for

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Number of seconds phone has been powered on for.

SIDE EFFECTS
  None

=============================================================================*/

uint32 time_get_uptime_secs( void )
{
  /* System time in seconds */
  uint32                          system_time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Lock to ensure time_get_secs() is consistent with time.power_on_sec */
  INTLOCK();

  /* Get the current system time */
  system_time = time_get_secs();

  /* Compute the difference btw system time & when the phone was powered on. */
  system_time -= time.power_on_sec;

  INTFREE();

  /* Return calculated value */
  return system_time;

} /* time_get_uptime_secs */



/*=============================================================================

FUNCTION TIME_GET_UPTIME_MS

DESCRIPTION
  Get time the phone has been powered on for

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Number of milliseconds phone has been powered on for returned to 'uptime_ms'

SIDE EFFECTS
  None

=============================================================================*/

void time_get_uptime_ms
(
  /* Output: Milliseconds phone has been powered on for */
  time_type                       uptime_ms
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Lock to ensure time_get_ms() is consistent with time.power_on_ms */
  INTLOCK();

  /* Get the current system time, in milliseconds */
  time_get_ms( uptime_ms );

  /* Subtract system time, in milliseconds, when the phone was powered on. */
  qw_sub( uptime_ms, uptime_ms, time.power_on_ms);

  INTFREE();

} /* time_get_uptime_ms */



/*=============================================================================

FUNCTION TIME_SYNC_TIME_OF_DAY

DESCRIPTION
  Synchronize the slow-clock based time-of-day to the current "ts" time.

DEPENDENCIES
  A valid CDMA, HDR, GSM, GPS, WCDMA (etc) time.  time_get( ) must not be
  using time_tod_get( ) when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  An diagnostic message is generated if there is a sudden jump in time-of-day.

=============================================================================*/

void time_sync_time_of_day( void )
{
  /* Timestamp to set time of day to */
  time_type                       ts_val;

  /* Timesource of timestamp */
  time_source_enum_type           source;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  INTLOCK();

  /* Get current time */
  source = time_get( ts_val );

  if ( source != TIME_SOURCE_32KHZ )
  {
    /* Set time of day to the current time from the given source */
    time_tod_set( ts_val );
  }
  else
  {
    ERR("Can't sync time-of-day to time-of-day", 0, 0, 0);
  }


  INTFREE();

} /* time_sync_time_of_day */



/*=============================================================================

FUNCTION TIME_SET_FROM_PMIC

DESCRIPTION
  Initialize the slow-clock based time-of-day to the PMIC's RTC time.

DEPENDENCIES
  Time-of-day must not have been initialized be another time source.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_set_from_pmic( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read PMIC's Real Time Clock (if available) to initialize Time-of-Day */
  time_tod_set_from_pmic_rtc();

} /* time_set_from_pmic */



/*=============================================================================

FUNCTION TIME_TO_MS_NATIVE

DESCRIPTION
  Convert a timestamp from System Time to millisecond units in native uint64
  format.

DEPENDENCIES
  None

RETURN VALUE
  The converted value.

SIDE EFFECTS
  None

=============================================================================*/

uint64 time_to_ms_native
(
  /* Time in timestamp format. */
  time_type                       time_stamp 
)
{
  /* Time in quadword format (time_type). */
  time_type                       temp_time;   

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
  qw_equ( temp_time, time_stamp );

  /* Use the helper function to convert the time to milliseconds. */
  time_to_ms( temp_time );

  /* Then convert to uint64 format. */
  return QW_CVT_Q2N( temp_time );

} /* time_to_ms_native */



/*=============================================================================

FUNCTION TIME_GET_MS_NATIVE

DESCRIPTION
  Get a timestamp from System Time in millisecond units from 6 Jan 1980 
  00:00:00.

DEPENDENCIES
  None

RETURN VALUE
  The time in ms from 6 Jan 1980 00:00:00.

SIDE EFFECTS
  None

=============================================================================*/

uint64 time_get_ms_native( void )
{
  /* Time in quadword format (time_type). */
  time_type                       ts_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time in ms and return it in native uint64 format. */
  time_get_ms( ts_val );

  return QW_CVT_Q2N( ts_val );

} /* time_get_ms_native */

