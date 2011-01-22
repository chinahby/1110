#ifndef TIME_TOD_H
#define TIME_TOD_H
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency         Granularity      Time Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS
  time_tod_init
    Initialize the time-of-day subsystem

  time_tod_set_from_pmic_rtc
    Initializes the time-of-day with the PMIC's RTC value, if available.

  time_tod_set
    Synchronize the slow-clock based time of day to a given timestamp

  time_tod_get
    Return a timestamp representing the slow-clock based time of day.

  time_tod_get_at
    Return a timestamp representing the slow-clock based time of day
    for a future slow-clock timetick value.


INITIALIZATION AND SEQUENCING REQUIREMENTS
    time_tod_init() must be called from time_init()
    time_tod_set() must be called before time_tod_get().
    A valid slow clock estimate is required for time_tod_get().


Copyright (c) 2003 - 2005 by QUALCOMM Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time_tod.h#8 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/02/06   bn      Added functions prototypes to get TOD time in ms and 20ms
06/09/06   cab     Added time_tod_status
08/11/05   jhs     Added time_tod_set_apps_bases prototype
05/10/05   ajn     PMIC RTC read moved out of time_tod_init.
11/12/04   ajn     Added time_tod_get_at() to compute future timestamps
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
08/01/03   ajn     Move _get_ms, _get_sec functions to time.h
07/18/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_svc.h"
#include "timetick.h"


/*=============================================================================

                              DATA DEFINITIONS

=============================================================================*/

typedef enum
{
  TS_INVALID = FALSE,
  TS_VALID   = TRUE,
  TS_UNKNOWN = 0xff
} time_tod_rtc_status_type;


/*=============================================================================

                        REGIONAL FUNCTION DEFINITIONS

=============================================================================*/



/*=============================================================================

FUNCTION GET_TIME_TOD_RTC_STATUS                                       REGIONAL

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

time_tod_rtc_status_type get_time_tod_rtc_status( void );


/*=============================================================================

FUNCTION TIME_TOD_INIT                                                 REGIONAL

DESCRIPTION
  Initializes the time-of-day subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
  Captures power-on time information

=============================================================================*/

void time_tod_init( void );



/*=============================================================================

FUNCTION TIME_TOD_SET_FROM_PMIC_RTC

DESCRIPTION
  Initialize the time-of-day value from the PMIC's Real Time Clock,
  if available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_set_from_pmic_rtc( void );



/*=============================================================================

FUNCTION TIME_TOD_SET                                                  REGIONAL

DESCRIPTION
  Set the slow-clock based time-of-day to the given timestamp

DEPENDENCIES
  Should be called from an INTLOCK'd context, to ensure proper time-setting.

RETURN VALUE
  None

SIDE EFFECTS
  An diagnostic message is generated if there is a sudden jump in time-of-day.

=============================================================================*/

void time_tod_set
(
  /* Timestamp to set time of day to */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_TOD_GET                                                  REGIONAL

DESCRIPTION
  Get the slow-clock based time-of-day

DEPENDENCIES
  A valid slow clock estimate must be available.
  time_{set,sync}_time_of_day( ) must have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_get
(
  /* Output: Slow-clock based time-of-day in ts_timestamp format */
  time_type                       ts_val
);



/*=============================================================================

FUNCTION TIME_TOD_GET_AT

DESCRIPTION
  Get the slow-clock based time-of-day for the corresponding slow-clock
  timetick value.

DEPENDENCIES
  A valid slow clock estimate must be available.
  time_{set,sync}_time_of_day( ) must have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_get_at
(
  /* Output: Slow-clock based time-of-day in ts_timestamp format */
  time_type                       ts_val,

  /* Slow-clock time-tick value to compute the timestamp at */
  timetick_type                   sclk
);



#if defined(FEATURE_EXPORT_TIME_REMOTE)
#if defined(IMAGE_APPS_PROC)
/*============================================================================

FUNCTION TIME_TOD_SET_APPS_BASES
 
DESCRIPTION
  Call to inform Apps of updated timestamp and timetick bases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  When called from Modem will cause RPC call to invoke this call on the Apps.

============================================================================*/

void time_tod_set_apps_bases
(
  /* Base time tick */
  timetick_type tick,

  /* Base time stamp */
  time_type stamp

);
#endif /* IMAGE_APPS_PROC */
#endif /* FEATURE_EXPORT_TIME_REMOTE */

/*=============================================================================

FUNCTION TIME_TOD_GET_MS

DESCRIPTION
  Get the time from TOD source, in # of milliseconds since "the beginning of 
  time".

DEPENDENCIES
  None

RETURN VALUE
  Milliseconds since 6 Jan 1980 00:00:00 returned to the time parameter

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_get_ms
(
  /* Out: Time in milliseconds since 6 Jan 1980 00:00:00 */
  time_type                       ts_val
);

/*=============================================================================

FUNCTION TIME_TOD_GET_20MS_FRAME_TIME

DESCRIPTION
  This procedure is returns the 64-bit system timestamp in units of
  20 millisecond frame time (traffic / paging / access channel frame
  time).

DEPENDENCIES
  None

RETURN VALUE
  20ms-time frame since 6 Jan 1980 00:00:00 returned to the time parameter 

SIDE EFFECTS
  None

=============================================================================*/

void time_tod_get_20ms_frame_time
(
  /* Address of qword in which to return 64-bit frame time. */
  time_type                       ts_val
);
#endif /* TIME_TOD_H */

