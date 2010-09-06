/*=============================================================================

       T I M E   S E R V I C E   T I M E   O F   D A Y   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Time Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS
  time_tod_init
    Initialize the time-of-day subsystem

  time_tod_set_from_pmic_rtc
    Initializes the time-of-day with the PMIC's RTC value, if available.

  time_tod_set
    Synchronize the slow-clock based time of day to a given timestamp.

  time_tod_get
    Return a timestamp representing the slow-clock based time of day.

  time_tod_get_at
    Return a ToD based timestamp for a slow-clock time in the future.


INITIALIZATION AND SEQUENCING REQUIREMENTS
    time_tod_init() must be called from time_init()
    time_tod_set() must be called before time_tod_get().
    A valid slow clock estimate is required for time_tod_get().


Copyright (c) 2003 - 2006 by QUALCOMM Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time_tod.c#19 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/02/06   bn      Added functions to get TOD time in ms and 20ms-frame
06/09/06   cab     Added time_tod_rtc_status
03/09/06   jhs     Added call to time_remote_init_time_of_day to sync time of 
                   day before acquiring.
09/26/05   jhs     Changed time_tod_set_apps_bases so that power on time is
                   recomputed when the bases were set.
08/26/05   ajn     After reading PMIC RTC at boot, update power-on time.
08/11/05   jhs     When modem updates time, let apps proc know.
07/11/05   ajn     Changed time write to PMIC from local to UTC.
05/10/05   ajn     PMIC RTC read moved out of time_tod_init.
03/24/05   ajn     Track number of resynchronization errors.
01/26/05   ajn     Corrected syntax error in TIME_TOD_SYNC_PMIC_RTC feature
11/12/04   ajn     Added time_tod_get_at() to compute future timestamps
06/08/04   bgc     Added protection to interruption in time_tod_get().
03/08/04   ajn     Update "power on" time when time-jump is detected
09/24/03   ajn     Update PMIC's RTC when time-of-day is set
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
08/01/03   ajn     Move _get_ms, _get_sec functions to time.c
07/18/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_tod.h"
#include "timetick.h"
#include "timer.h"
#include "ts.h"
#include "sclk.h"
#include "msg.h"

#if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
#include "time_remote.h"
#endif /* defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC) */



#ifdef FEATURE_PM1000_RTC
#error code not present
#elif defined(FEATURE_PMIC_RTC)

#include "pm.h"
#define TIME_TOD_SYNC_PMIC_RTC

#else

#undef TIME_TOD_SYNC_PMIC_RTC

#endif /* FEATURE_PM1000_RTC or FEATURE_PMIC_RTC */



#ifdef TIME_TOD_SYNC_PMIC_RTC
/* Use PMIC Real Time Clock to set Time-of-Day at power-up;
   Set system time to PMIC's RTC when system timing is acquired. */
#include "time_jul.h"
#endif /* TIME_TOD_SYNC_PMIC_RTC */


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */




/*-----------------------------------------------------------------------------
  Macros used for converting to/from ts_timestamp format
-----------------------------------------------------------------------------*/

/* To convert timestamp to 1.25ms units, shift out cx32 bits of timestamp */
#define TOD_TS_TO_1p25MS_SHIFT    -16

/* Conversion from 1.25ms units to millisecond units. */
#define TOD_1p25MS_TO_MS(t)       (((t) * 5 + 2) / 4)

/* 10ms error theshold, in 1.25ms units */
#define TOD_DELTA_THRESHOLD       8


/* Approximately 1/2 minute before 32kHz 32bit counter rolls over */
#define TOD_RESYNC_SCLK           0xFFF00000uL

/* The resync time should have already triggered, so any SCLK value
   greater than TOD_RESYNC_LATE_SCLK is likely be negative time and
   probably an error. */
#define TOD_RESYNC_LATE_SCLK      0xFFFFFF00uL

/* One second (800 x 1.25ms) expressed as a timestamp format value */
#define TOD_1_SEC                 (800uL << 16)

//add by yangdecai 2010-09-06
#include "AEEStdLib.h"
#include "nv.h"

#define TOD_JU_MONTH                1
#define TOD_FE_MONTH                2
#define TOD_MO_MONTH                3
#define TOD_FR_MONTH                4
#define TOD_MA_MONTH                5
#define TOD_JN_MONTH                6
#define TOD_JL_MONTH                7
#define TOD_AN_MONTH                8
#define TOD_ST_MONTH                9
#define TOD_OT_MONTH                10
#define TOD_NE_MONTH                11
#define TOD_DE_MONTH                12

#define TOD_TIME_ZONE               8
#define TOD_TIME_24H                24

#define TOD_LEAP_YEAR               29
#define TOD_COMMON_YEAR             28

#define TOD_BIG_MONTH               31
#define TOD_SAM_MONTH               30




//add by yangdecai 2010-09-06


/*-----------------------------------------------------------------------------
  Time of day static information
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Timestamp used as base for slow-clock time-of-day */
  time_type                       base_timestamp;

  /* Time tick corresponding to above timestamp */
  timetick_type                   base_timetick;

  /* Flag to indicate if time-of-day has ever been set before */
  boolean                         initialized;

  /* Flag to indicate if time-of-day has been read from the PMIC's RTC */
  boolean                         read_pmic_rtc;

  /* Resync timer, to ensure base values are updated before rollover  */
  timer_type                      resync_timer;

  /* A timer group, to ensure resync_timer never disabled. */
  timer_group_type                resync_timer_group;

  /* Count of "Time-of-Day" synchronization errors */
  uint32                          sync_error_count;
}
time_tod_struct_type;


static time_tod_struct_type       time_tod;

static time_tod_rtc_status_type   time_tod_rtc_status = TS_UNKNOWN;

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/



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
  timetick_type                   tick,

  /* Base time stamp */
  time_type                       stamp
)
{

  /* Current time-of-day value */
  time_type                       time_of_day;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get a comparison time-of-day value */
  time_tod_get( time_of_day );

  /* Set base timetick */
  time_tod.base_timetick = tick;

  /* Copy the passed in stamp to the base timestamp */
  qw_equ( time_tod.base_timestamp, stamp );

  /* Set time of day as initialized */
  time_tod.initialized = TRUE;

  /* Record count of number of TOD errors */
  time_tod.sync_error_count++;

   /*  The Apps processor is continiously getting updates from the Modem
       about what the time of day is.  To ensure "uptime" monotonically
       increases after these updates, use the time-jump to recompute the
       power-on point.  We always do this, because the Modem only forwards
       when the new bases cause at least a 25ms change in the time of day. */
  time_compute_power_on( time_of_day, stamp );

} /* time_tod_set_apps_bases */
#endif /* IMAGE_APPS_PROC */
#endif /* FEATURE_EXPORT_TIME_REMOTE */



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

void time_tod_set_from_pmic_rtc( void )
{
#ifdef TIME_TOD_SYNC_PMIC_RTC

  /* Julian time/date from the PMIC's Real Time Clock */
  time_julian_type                julian;

  /* Flag indicating that the PMIC RTC value is valid */
  boolean                         valid;

  /* Seconds since January 6, 1980 00:00:00 */
  uint32                          secs;

  /* Time before PMIC's RTC is used to set time-of-day */
  time_type                       orig_time;

  /* PMIC's RTC time, expressed as in time_type timestamp format */
  time_type                       pmic_time;
  MSG_FATAL("time_tod_set_from_pmic_rtc:::::::::::::1111111111111111111",0,0,0);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If time-of-day has already been read from the PMIC's RTC ... */
  if ( time_tod.read_pmic_rtc )
  {
    /* ... don't bother re-reading it - not much point. */
    return;
  }


  /* If time-of-day has already been set from CDMA or HDR system time ... */
  if ( time_tod.initialized )
  {
    /* ... we've got better than the PMIC RTC's 1 second resolution. */
    return;
  }


  #ifdef FEATURE_PM1000_RTC
#error code not present
  #elif defined(FEATURE_PMIC_RTC)

  /* Retrieve the real time clock's Julian time/date from the PMIC */
  pm_rtc_rw_cmd(PM_RTC_GET_CMD, (pm_rtc_julian_type*)(&julian));
  //add by yangdecai 2010-09-04
  {
	  	boolean daylight = 0;
	    int32   local    = 0;//LOCALTIMEOFFSET( &daylight);
	    int     timezone = 0;//local / 3600;
   
		nv_item_type nvi;
    	int32 tzoffset = 0;                 
    	nv_stat_enum_type     nv_read_status;                
   	
    
    	nv_read_status = OEMNV_Get(NV_DB_LTM_OFF_I, &nvi);

    	if (nv_read_status == NV_DONE_S)
    	{
    		int32 ltm_off = 0;
		     
		      ltm_off = (int32)(nvi.db_ltm_off & 0xFF);
		      if(ltm_off >= 128){
		        ltm_off -= 256;
		      }

		      tzoffset = ltm_off * 15 * 60;
			  timezone = tzoffset/3600;
    	}
		if(OEMNV_Get(NV_DB_DAYLT_I, &nvi) ==  NV_DONE_S)
		{
        	daylight = (boolean)nvi.db_daylt;
		}
		
	    if( daylight)
	    {
	        timezone --;
	    }
		//timezone = TOD_TIME_ZONE;
		if(timezone>0)
		{
			if(julian.hour>timezone)
			{
				julian.hour = julian.hour-timezone;  
			}
			else
			{
				if(julian.day>1)
				{
					julian.day = julian.day -1;
				}
				else
				{
					if((julian.month == TOD_MA_MONTH) ||(julian.month == TOD_JL_MONTH)||
					   (julian.month == TOD_OT_MONTH) ||(julian.month == TOD_DE_MONTH))
					{
						julian.month = julian.month-1;
						julian.day = TOD_SAM_MONTH;
					}
					else if((julian.month == TOD_FE_MONTH) ||(julian.month == TOD_FR_MONTH) ||(julian.month == TOD_JN_MONTH) ||
						    (julian.month == TOD_AN_MONTH) ||(julian.month == TOD_ST_MONTH) ||(julian.month == TOD_NE_MONTH))
					{
						julian.month = julian.month-1;
						julian.day = TOD_BIG_MONTH;
					}
					else if(julian.month == TOD_JU_MONTH)
					{
						julian.year = julian.year-1;
						julian.month = TOD_DE_MONTH;
						julian.day = TOD_BIG_MONTH;
					}
					else if(julian.month == TOD_MO_MONTH)
					{
						julian.month = TOD_FE_MONTH;
						if(((julian.year%4 ==0) &&(julian.year%100!=0))||((julian.year%100 ==0) &&(julian.year%400!=0)))
						{
							julian.day = TOD_LEAP_YEAR;
						}
						else
						{
							julian.day = TOD_COMMON_YEAR;
						}
						
					}
				}
				julian.hour = julian.hour+(TOD_TIME_24H-timezone);
			}
		}
		else
		{
			int temp = TOD_TIME_24H + timezone;
			if(julian.hour<temp)
			{
				julian.hour = julian.hour-timezone;   //add by yangdecai 2010-09-04
			}
			else
			{
				
				if(((julian.month == TOD_JU_MONTH) ||(julian.month == TOD_MA_MONTH) ||(julian.month == TOD_JL_MONTH)||
				    (julian.month == TOD_OT_MONTH) ||(julian.month == TOD_MO_MONTH)|| 
				    (julian.month == TOD_AN_MONTH))&&(julian.day == TOD_BIG_MONTH))
				{
					julian.month = julian.month + 1;
					julian.day = 1;
				}
				else if(((julian.month == TOD_FR_MONTH) ||(julian.month == TOD_JN_MONTH) ||
					     (julian.month == TOD_ST_MONTH) ||(julian.month == TOD_NE_MONTH))&&
					     (julian.day == TOD_SAM_MONTH))
				{
					julian.month = julian.month + 1;
					julian.day = 1;
				}
				else if(julian.month == TOD_DE_MONTH)
				{
					julian.year = julian.year+1;
					julian.month = TOD_JU_MONTH;
					julian.day = 1;
				}
				else if(julian.month == TOD_FE_MONTH)
				{
					
					if(((julian.year%4 ==0) &&(julian.year%100!=0))||((julian.year%100 ==0) &&(julian.year%400!=0))&&
						(julian.day == TOD_LEAP_YEAR))
					{
						julian.month = julian.month +1;
						julian.day = 1;
					}
					else if(julian.day == TOD_COMMON_YEAR)
					{
						julian.month = julian.month +1;
						julian.day = 1;
					}
				}
			    julian.hour = julian.hour-(TOD_TIME_24H+timezone);
			}
		}
  }
   //add by yangdecai 2010-09-04 end
  /* Be sure that the time/date is valid */
  valid = pm_is_rtc_valid( PM_RTC_24HR_MODE, (pm_rtc_julian_type*)(&julian) );

  time_tod_rtc_status = (time_tod_rtc_status_type) valid;

  #else

  #error "Unsupported PMIC RTC"

  #endif /* FEATURE_PM1000_RTC | FEATURE_PMIC_RTC */


  /* If the PMIC Real Time Clock's date/time is valid, convert it to the
     timestamp format for the time-of-day base timestamp.  */

  if ( valid )
  {
    /* Convert RTC value to seconds from Jan 6, 1980 00:00:00 */
    secs = time_jul_to_secs ( &julian );

    INTLOCK();

    /* Convert from seconds to time-stamp format */
    qw_set ( pmic_time, 0, secs );
    qw_mul ( pmic_time, pmic_time, TOD_1_SEC );

    /* Get original time for power_on time adjustment */
    time_get( orig_time );

    /* Set the time-of-day */
    qw_equ ( time_tod.base_timestamp, pmic_time );

    /* Capture timetick at this starting point */
    time_tod.base_timetick = timetick_get_safe();

    /* Time has been successfully read from the PMIC */
    time_tod.read_pmic_rtc = TRUE;

    /* Update power-on time origin to account for PMIC RTC's time */
    time_compute_power_on( orig_time, pmic_time );

    INTFREE();
  }

#else

  /* No PMIC RTC to read - nothing to do */

#endif /* TIME_TOD_SYNC_PMIC_RTC */

} /* time_tod_set_from_pmic_rtc */



#ifdef TIME_TOD_SYNC_PMIC_RTC
/*=============================================================================

FUNCTION TIME_TOD_SET_TO_PMIC_RTC

DESCRIPTION
  Synchronize the PMIC's Real Time Clock with the current time-of-day

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_tod_set_to_pmic_rtc( void )
{
  /* Seconds since January 6, 1980 00:00:00 */
  uint32                          secs;

  /* Julian time/date from the PMIC's Real Time Clock */
  time_julian_type                julian;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the local time in seconds */
  secs = time_get_secs ();

  /* Convert seconds to Julian time/date */
  time_jul_from_secs ( secs, &julian );

  #ifdef FEATURE_PM1000_RTC
#error code not present
  #elif defined(FEATURE_PMIC_RTC)

  /* Set the RTC mode & program the PMIC's RTC with the local time */
  pm_set_rtc_display_mode ( PM_RTC_24HR_MODE );
  pm_rtc_rw_cmd (PM_RTC_SET_CMD, (pm_rtc_julian_type*)(&julian));

  #else

  #error "Unsupported PMIC RTC"

  #endif /* FEATURE_PM1000_RTC | FEATURE_PMIC_RTC */

} /* time_tod_set_to_pmic_rtc */

#endif /* TIME_TOD_SYNC_PMIC_RTC */



/*=============================================================================

FUNCTION TIME_TOD_RESYNC

DESCRIPTION
  The 32kHz-based time-of-day is periodically resynced to system time, if
  available.  If it has not been re-synced for 1.5 days, the 32kHz counter
  will 'rollover', and time-of-day would jump backwards 1.5 days.
  To prevent this from occuring, this timer callback is used to resync
  time-of-day before the rollover will occur.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_tod_resync
(
  /* Arbitrary timer data.  Not used but required for function prototype. */
  timer_cb_data_type              unused_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* time_sync_time_of_day( ) is not used here, since we need to allow
     fallback on time-of-day, which time_sync_time_of_day does not allow */


  INTLOCK();

  /* Get current time, possibly from time-of-day, and update base value. */
  (void) time_get( time_tod.base_timestamp );

  /* Capture timetick at this synchronization point */
  time_tod.base_timetick = timetick_get_safe();

  INTFREE();

  #if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
  time_remote_update_apps_tod( time_tod.base_timestamp,
                               time_tod.base_timetick );
  #endif /* FEATURE_TOD_MULTIPROC_SYNC && IMAGE_MODEM_PROC */

} /* time_tod_resync */



/*=============================================================================

FUNCTION GET_TIME_TOD_RTC_STATUS                                       REGIONAL

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/

time_tod_rtc_status_type get_time_tod_rtc_status( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   return time_tod_rtc_status;

}

/*=============================================================================

FUNCTION TIME_TOD_INIT                                                 REGIONAL

DESCRIPTION
  Initializes the time-of-day subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
  Installs a timer to update base values before TimeTick rollover

=============================================================================*/

void time_tod_init( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Mark time-of-day as being uninitialized, until time has been set. */
  time_tod.initialized   = FALSE;
  time_tod.read_pmic_rtc = FALSE;

  /* Capture timetick at this starting point */
  time_tod.base_timetick = timetick_get_safe();

  /* Create a timer to update base values before the TimeTick rollover */
  timer_def( &time_tod.resync_timer, &time_tod.resync_timer_group,
             NULL, 0, time_tod_resync, NULL);
  timer_set( &time_tod.resync_timer, TOD_RESYNC_SCLK, TOD_RESYNC_SCLK, T_SCLK);

  #if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
  time_remote_init_time_of_day( time_tod.base_timetick,
                                time_tod.base_timestamp );
  #endif /* FEATURE_TOD_MULTIPROC_SYNC && IMAGE_MODEM_PROC */

} /* time_tod_init */



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
)
{
  /* Captured timetick value */
  timetick_type                   ticks;

  /* Current time-of-day value */
  time_type                       time_of_day;

  /* Difference between set value and time-of-day */
  time_type                       delta;

  /* Difference between set value and time-of-day, in 1.25ms units */
  int32                           delta_1p25ms;

  /* Difference between set value and time-of-day, in ms */
  int32                           delta_ms;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Capture timetick at this synchronization point */
  ticks = timetick_get_safe();


  /* Get a comparison time-of-day value */
  time_tod_get( time_of_day );


  /* Update the time-of-day base values */
  time_tod.base_timetick = ticks;
  qw_equ( time_tod.base_timestamp, ts_val);


  /* If the time-of-day subsystem has been initialized, check to see if
     any large timing jump is going to occur when the time value is reset.
     If the time-of-day subsytem has not been initialized, then compute
     the power-on time, for up-time computations, and store the new
     system time to the PMIC RTC (if available) */

  if ( time_tod.initialized )
  {
    /* Time-of-day subsystem has already been initialized.
       Did time jump forward or backwards by a large amount? */


    /* Compute difference in set value and old time of day */
    qw_sub( delta, ts_val, time_of_day );

    /* Shift down to convert to 1.25ms units */
    qw_shift( delta, TOD_TS_TO_1p25MS_SHIFT);

    /* Extract as a signed 32-bit value (+/- 1 month range) */
    delta_1p25ms = (int32) qw_lo( delta );

    /* Convert to milliseconds (calculation overflows if > 6d 5h 7m 50s) */
    delta_ms = TOD_1p25MS_TO_MS(delta_1p25ms);

    /* Report discrepencies of over a certain amount */
    if ( delta_1p25ms >=  TOD_DELTA_THRESHOLD  ||
         delta_1p25ms <= -TOD_DELTA_THRESHOLD     )
    {
      /* Record count of number of TOD errors */
      time_tod.sync_error_count++;

      /* Report the suspicious time jump */
      MSG_ERROR("TOD: %+ld x 1.25ms (%+ldms)", delta_1p25ms, delta_ms, 0);

      /* In test environments, a test signal may replay over-and-over,
         causing time to periodically jump backwards at the end of the test.
         To ensure "uptime" monotonically increases during these tests,
         use the time-jump to recompute the power-on point. */
      time_compute_power_on( time_of_day, ts_val );
    }
  }
  else
  {
    /* The time-of-day subsystem had not yet been initialized. */

    /* Compute the moment the phone was powered on for up-time computations */
    time_compute_power_on( time_of_day, ts_val );

    #ifdef TIME_TOD_SYNC_PMIC_RTC
    /* Update the PMIC's RTC with the correct time-of-day. */
    time_tod_set_to_pmic_rtc();
    #endif /* TIME_TOD_SYNC_PMIC_RTC */

    /* Mark the slow-clock based time-of-day as being initialized */
    time_tod.initialized = TRUE;
  }


  /* Time-of-day base values have been synced to time-tick counter.
     We have maximal time before we will need to resync base values. */
  timer_set( &time_tod.resync_timer, TOD_RESYNC_SCLK, TOD_RESYNC_SCLK, T_SCLK);

  #if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
  time_remote_update_apps_tod( time_tod.base_timestamp,
                               time_tod.base_timetick );
  #endif /* FEATURE_TOD_MULTIPROC_SYNC && IMAGE_MODEM_PROC */

} /* time_tod_set */



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
)
{
  /* Slow clock counter value to convert to timestamp */
  timetick_type                   ticks_delta;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  INTLOCK();
  /* Prevent changes to time_tod structure while capturing values. */

  /* Copy the base timestamp to the output timestamp value */
  qw_equ( ts_val, time_tod.base_timestamp );

  /* The number of ticks which will have elapsed by the given time-point */
  ticks_delta = sclk - time_tod.base_timetick;

  INTFREE();


  /* Advance the output timestamp by the elapsed slow clocks */
  sclk_to_timestamp( ts_val, ticks_delta );

} /* time_tod_get_at */



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
)
{
  /* Slow clock counter value to convert to timestamp */
  timetick_type                   ticks_delta;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  INTLOCK();
  /* Prevent changes to time_tod structure while capturing time. */

  /* Copy the base timestamp to the output timestamp value */
  qw_equ( ts_val, time_tod.base_timestamp );

  /* Get the elapsed number of ticks since recording the timetick base */
  ticks_delta = timetick_get_safe() - time_tod.base_timetick;

  INTFREE();

  /* If ticks_delta is slightly negative, it will become a large positive
     value, and return a time in the future */
  if(ticks_delta > TOD_RESYNC_LATE_SCLK)
  {
    MSG_ERROR("time_tod suspicious SCLK count value 0x%x", ticks_delta, 0, 0);
  }

  /* Advance the output timestamp by the elapsed slow clocks */
  sclk_to_timestamp( ts_val, ticks_delta );


} /* time_tod_get */

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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read the current time of day */
  (void) time_tod_get( ts_val );

  /* Convert time-stamp format to milliseconds */
  time_to_ms( ts_val );

} /* time_tod_get_ms */

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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get timestamp accurate in 1.25ms units
  */
  (void) time_tod_get( ts_val );

  /* Divide by 64*1024 to truncate the low 16 bits of the time stamp,
  ** then divide the 1.25 ms units by 16 to get 20ms units.
  */
  /* A divide by 16*(64*1024) = 2^20. So, use a bitshift for speed */

  (void) qw_div_by_power_of_2( ts_val, ts_val, 20 );

} /* time_tod_get_20ms_frame_time */

