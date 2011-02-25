
/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

                       C D M A   T I M E   K E E P I N G

GENERAL DESCRIPTION
  Implements cdma time-keeping functions using the symbol combiner.


EXTERNALIZED FUNCTIONS
  time_cdma_set_valid
    Indicates whether or not CDMA time is available

  time_cdma_set
    Set the system timestamp value in 80 ms units.

  time_cdma_tick
    Increment the timestamp by a PN roll.

  time_cdma_inc
    Increment the 64-bit system timestamp in 26.6 ms units

  time_cdma_get
    Return the system timestamp value.

  time_cdma_get_slot_time
    Return the 64-bit system timestamp in units of slot time.

  time_cdma_get_20ms_frame_time
    Return the 64-bit system timestamp in units of 20 millisecond frame time.

  time_cdma_get_20ms_frame_offset_time
    Return the 64-bit system timestamp in units of 20 millisecond frame time,
    adjusted for a frame offset.

  time_cdma_set_20ms_frame_offset
    Set the 20ms frame offset


INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_cdma_set() needs to be called when accurate time is available from the
  base station.


Copyright(c) 1990, 1991, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 1993, 1994, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 1996, 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright(c) 2002, 2003, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time_cdma.c#8 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/30/04   ejv     Changed srch_get_vsc_time to srch_scomb_get_vsc_time.
03/11/04   ajn     Added time_cdma_sleep() & time_cdma_wake( ) functions.
                   [Replaces ts_dmod_valid(F) ... ts_inc()/ts_dmod_valid(T)]
09/09/03   ajn     time_cdma_set_valid(TRUE) now calls time_sync_time_of_day.
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
07/29/03   ajn     Corrected shift for 80ms phase
07/25/03   ajn     Transfer from services/clk/ts.c to TIME VU

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_cdma.h"
#include "timetick.h"
#include "sclk.h"
#include "srch.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------
  Constants for converting to the qword timestamp format
-----------------------------------------------------------------------------*/

/* Number of 1.25 millisecond ticks in 80 milliseconds (64) or 2^6
*/
#define TIME_CDMA_80_MS_TICKS_SHIFT 6
#define TIME_CDMA_80_MS_TICKS (1<<TIME_CDMA_80_MS_TICKS_SHIFT)

/* Number of bits to right shift time to get the milliseconds
** portion of the timestamp
*/
#define TIME_CDMA_MS_SHIFT    16

/* 80 milliseconds is 64 * 1.25 milliseconds, we then shift this up
** by 16 bits to get into the 1.25 ms counter part of the timestamp
*/
#define TIME_CDMA_80_MS       ((dword) TIME_CDMA_80_MS_TICKS << 16L)
#define TIME_CDMA_40_MS       (TIME_CDMA_80_MS >> 1)

#define TIME_CDMA_80MS_SHIFT  (TIME_CDMA_MS_SHIFT+TIME_CDMA_80_MS_TICKS_SHIFT)


#define TIME_CDMA_SUB80MS_MSK ((1<<TIME_CDMA_80MS_SHIFT)-1)

/* Max slot cycle length
*/
#define TIME_CDMA_MAX_SLOT    2048

/* Take the  2-chip counter and get phase from it by shifting it down. */
#define TIME_CDMA_PHASE_SHIFT 14


/* Convert 2-chip units to timestamp LSBs */
#define TIME_CDMA_TS_FROM_2CHIPS( cnt2 ) \
          (((dword)((cnt2) / 768) << 16) + ((dword)((cnt2) % 768) * 64))

/*-----------------------------------------------------------------------------
  CDMA Time static data
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Timestamp at start of sleep */
  time_type                       timestamp;

  /* Slow clock count at the start of CDMA sleep */
  timetick_type                   sclk;
}
time_cdma_sleep_struct_type;


typedef struct
{
  /* Timestamp at last 80ms roll */
  time_type                       t80ms;

  /* 80ms phase (0=0-26ms, 1=27-53ms, 2=54-79ms) */
  uint16                          phase80;

  /* CDMA time valid flag */
  boolean                         valid;

  /* Frame offset */
  uint8                           frame_offset;

  /* Timestamp and slow clock count at the start of sleep */
  time_cdma_sleep_struct_type     sleep;
}
time_cdma_struct_type;


static time_cdma_struct_type      time_cdma =
{
  { 0, 0 },         /* January 6, 1980, 00:00:00 */
  0,                /* Phase 0, for lack of any other phase value */
  FALSE,            /* CDMA time is _NOT_ valid */
  0,                /* No frame offset */
  { { 0, 0 }, 0 }   /* Sleep data */
};



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/



/*=============================================================================

FUNCTION TIME_CDMA_SET_VALID

DESCRIPTION
  Permits other code in the phone to tell TIME_CDMA that the contents of the
  demodulator are not meaningful. This causes TIME to fall back on an
  alternate time source, or as a last resort, "Time Of Day".

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_set_valid
(
  /* Flag indicating whether CDMA time is valid or not */
  boolean                         valid
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Store the flag */
  time_cdma.valid = valid;

  /* If CDMA time is now "valid" ... */
  if ( valid )
  {
    /* ... synchronize TOD to CDMA time reporting any significant time error */
    time_sync_time_of_day();
  }

} /* time_cdma_set_valid */



/*=============================================================================

FUNCTION TIME_CDMA_SET

DESCRIPTION
  This procedure sets the system timestamp to a specified value which is
  the combination of a multiple of 80 milliseconds and a 26.67 ms phase
  of 80 milliseconds (0, 1, 2).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the system timestamp and marks CDMA time as valid.

=============================================================================*/

void time_cdma_set
(
  /* Number of elapsed 80 ms units since base date. */
  time_type                       set_val,

  /* The current phase of the 80 ms. */
  uint16                          phase80
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Disable interrupts. */

  INTLOCK();


  /* Set timestamp count to equivalent number of 1.25 ms units
     and shift value into correct position. */

  qw_equ( time_cdma.t80ms, set_val);
  qw_shift( time_cdma.t80ms, TIME_CDMA_80MS_SHIFT );


  /* Record current phase of 80ms counter. */

  time_cdma.phase80 = phase80;


  /* Mark CDMA time as now valid */

  time_cdma_set_valid( TRUE );


  /* Restore interrupts. */

  INTFREE();

} /* time_cdma_set */



#ifndef MSM5000_IRAM_REV
/*=============================================================================

FUNCTION TIME_CDMA_GET

DESCRIPTION
  This procedure is called to read the 64-bit CDMA system timestamp.

DEPENDENCIES
  time_cdma_set() needs to be called when accurate time is available from
  the base station.  This function does not fall back on "Time Of Day".

RETURN VALUE
  TRUE if CDMA system time is available, FALSE otherwise.
  Parameter ts_val is set to the current system time, in timestamp format.

SIDE EFFECTS
  None

=============================================================================*/

boolean  time_cdma_get
(
  /* Address of qword in which to return 64-bit system timestamp. */
  time_type                       ts_val
)
{
  /* Flag indicating if CDMA time is valid */
  boolean                         valid;

  /* Virtual symbol combiner position */
  word                            cnt2;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Disable interrupts for critical section */
  INTLOCK();

  valid = time_cdma.valid;

  if ( valid )
  {
    /* Get timestamp accurate in 1.25ms units */
    qw_equ( ts_val, time_cdma.t80ms );


    /* Read the value of the symbol combiner. Since SEARCH is horsing around
       with it sometimes in order to save power and increase standby, the
       logic of this required TIME to know too much about what SEARCH is doing.
       Therefore, SEARCH provides a virtualized interface to the symbol
       combiner, and it will automatically compensate for any possible
       error induced in the symbol combiner value, and return what it ought
       to be. */

    #ifndef FEATURE_HWTC
    cnt2 = srch_scomb_get_vsc_time();
    #endif /* FEATURE_HWTC */

    /* Add in 2 chip counter. Add the count of 1.25 ms into the high 48 bits,
       and the count of chip*32's into the low 16 bits.  Note:  768 2-chip
       counts is 1.25 ms, and 64 chip/32 is 2 chips. */

    qw_inc( ts_val, TIME_CDMA_TS_FROM_2CHIPS( cnt2 ));
  }

  /* Restore interrupts. */
  INTFREE(  );

  return valid;

} /* time_cdma_get */

#endif /* !MSM5000_IRAM_REV */



/*=============================================================================

FUNCTION TIME_CDMA_INC

DESCRIPTION
  Increment the 64-bit CDMA system timestamp in 26.6 ms units.

DEPENDENCIES
  The CDMA system clock must be set to a reasonable value in order for
  this function to perform a meaningful operation on the timestamp.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_inc
(
  /* The number of 26.6ms units to add to system time */
  unsigned int /* fast */         increment
)
{
  /* 80ms increment value */
  time_type                       inc_80ms;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** When this function is called out of deep sleep, note that the
  ** timestamp and phase are being maintained by dead reckoning. But
  ** more important, note that they won't actually be used. That's
  ** because there is code in ts_get which checks the valid flag and
  ** falls back on Time Of Day when not in a mode where the demodulator
  ** time is valid. While the phone is in deep sleep, ts_get will return
  ** TOD. So even if these values drift during deep sleep it won't matter.
  ** If we acquire a cell after coming out of deep sleep,
  ** we'll find the sync channel and process a sync channel message before
  ** going into one of the states where ts_get trusts the demodulator.
  ** Processing of the sync channel message includes a call to ts_set,
  ** which will set the values of ts_stamp and ts_phase80. So
  ** by the time ts_get is ready to use them they'll be right.
  **
  ** Special casing deep sleep here is not worthwhile.
  */

  /* Disable interrupts. */
  INTLOCK( );


  /* Determine new phase80 value */
  increment += time_cdma.phase80;

  /* For every count of 3 rolls, increment the 80ms count by 1 */
  qw_set( inc_80ms, 0, increment/3 );
  qw_shift( inc_80ms, TIME_CDMA_80MS_SHIFT);
  qw_add( time_cdma.t80ms, time_cdma.t80ms, inc_80ms);

  /* The remainder is the new phase80 */
  time_cdma.phase80 = (uint8) (increment % 3);


  /* Restore interrupts. */
  INTFREE( );

} /* time_cdma_inc */



/*=============================================================================

FUNCTION TIME_CDMA_TICK

DESCRIPTION
  This function is called at every PN Roll (26.67 ms).  It increments the
  timestamp by 80 ms each time phase80 is 0.  Phase80 being 0 indicates that
  80 ms have passed.

DEPENDENCIES
  The CDMA system clock must be set to a reasonable value in order for
  this function to perform a meaningful operation on the timestamp.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_tick
(
  /* Specifies the current phase of the 80 ms. */
  uint16                          phase80
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Disable interrupts. */
  INTLOCK( );

  /* Determine if phase80 value has rolled */

  if ( phase80 < time_cdma.phase80 )
  {
    /* phase80 value has 'rolled' */

    /* Increase coarse time by 80ms */
    qw_inc( time_cdma.t80ms, TIME_CDMA_80_MS );
  }

  /* Store new phase */
  time_cdma.phase80 = phase80;


  /* Restore interrupts. */
  INTFREE( );

} /* time_cdma_tick */



/*=============================================================================

FUNCTION TIME_CDMA_SLEEP

DESCRIPTION
  Invalidates CDMA system time, in preperation for CDMA slotted sleep.

DEPENDENCIES
  The CDMA system clock must accurate when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  Updates the slow-clock based "Time of Day" before invalidating CDMA time.

=============================================================================*/

void time_cdma_sleep( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Synchronize TOD to CDMA time, reporting any significant time error.
     This will detect a PN roll slip during the CDMA awake. */
  time_sync_time_of_day();


  /* Disable interrupts. */
  INTLOCK();

  /* Capture the start of sleep timestamp & slow-clock counter values.
     These will be used to update the CDMA timestamp at wakeup */

  (void) time_cdma_get( time_cdma.sleep.timestamp );
  time_cdma.sleep.sclk = timetick_get_safe();

  /* Restore interrupts. */
  INTFREE( );

  /* Invalidate CDMA system time. */
  time_cdma.valid = FALSE;

} /* time_cdma_sleep */



/*=============================================================================

FUNCTION TIME_CDMA_WAKE

DESCRIPTION
  Updates and asserts valid CDMA system time, after CDMA slotted sleep.

DEPENDENCIES
  The symbol combiner timing must be correct when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

void time_cdma_wake( void )
{
  /* Slow clock count at wakeup */
  timetick_type                   sclk;

  /* Symbol combiner position, in 2-chip units */
  uint32                          sc_pos;

  /* Timestamp at most-recent 80ms boundary */
  time_type                       t80ms;

  /* Slow-clock based timestamp at end of sleep */
  time_type                       t_wake_sclk;

  /* Symbol-combiner based timestamp at end of sleep */
  time_type                       t_wake_sc;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Disable interrupts. */
  INTLOCK();

  /* Get the slow clock count at the wakeup point */
  sclk = timetick_get_safe();

  /* Get the symbol combiner position at the wakeup point as well */
  sc_pos = srch_scomb_get_vsc_time();

  /* Compute slow-clock based end-of-sleep timestamp */
  qw_equ( t_wake_sclk, time_cdma.sleep.timestamp );
  sclk_to_timestamp( t_wake_sclk, sclk - time_cdma.sleep.sclk );

  /* Round down to nearest 80ms boundary */
  qw_equ( t80ms, t_wake_sclk );
  qw_dec( t80ms, qw_lo( t80ms ) & TIME_CDMA_SUB80MS_MSK );

  /* Compute symbol-combiner based end-of-sleep timestamp, and 80ms boundary
     computed above. */
  qw_equ( t_wake_sc, t80ms );
  qw_inc( t_wake_sc, TIME_CDMA_TS_FROM_2CHIPS( sc_pos ) );

  /* If symbol combiner based timestamp differs from the slow clock based
     timestamp by more the 40ms, the computed 80ms timestamp is off by 80ms */
  qw_dec( t_wake_sc, TIME_CDMA_40_MS );
  if (qw_cmp( t_wake_sclk, t_wake_sc) < 0)
  {
    /* t_wake_sclk < t_wake_sc - 40, therefore t80ms is 80ms too big */
    qw_dec( t80ms, TIME_CDMA_80_MS);
  }
  else
  {
    qw_inc( t_wake_sc, TIME_CDMA_80_MS ); /* -40 + 80 = +40 */
    if (qw_cmp( t_wake_sclk, t_wake_sc) > 0 )
    {
      /* t_wake_sclk > t_wake_sc + 40, therefore t80ms is 80ms too small */
      qw_inc( t80ms, TIME_CDMA_80_MS);
    }
  }

  /* Set CDMA system time */
  qw_equ( time_cdma.t80ms, t80ms);
  time_cdma.phase80 = sc_pos >> TIME_CDMA_PHASE_SHIFT;

  /* Mark CDMA system time as valid */
  time_cdma_set_valid( TRUE );

  /* Restore interrupts. */
  INTFREE();

} /* time_cdma_sleep */



/*=============================================================================

FUNCTION TIME_CDMA_GET_SLOT_TIME

DESCRIPTION
  This procedure is returns the 64-bit system timestamp in units of
  slot time, 26.6 millisecond units, modulo the maximum slot cycle
  length.

DEPENDENCIES
  The value returned by this function is meaningless unless the demodulator
  is running and contains a valid value.  This function does not fall back on
  "Time Of Day".

RETURN VALUE
  The current timestamp value, as specified above.

SIDE EFFECTS
  None

=============================================================================*/

uint16 time_cdma_get_slot_time( void )
{
  /* Current CDMA time-stamp */
  time_type                       ts_val;

  /* Virtual symbol combiner position */
  uint16                          cnt2;

  /* Virtual symbol combiner position */
  uint16                          phase80;

  /* Resulting slot time */
  uint16                          slot_time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Disable interrupts. */
  INTLOCK( );

  /* Get timestamp accurate in 1.25ms units */
  qw_equ( ts_val, time_cdma.t80ms );


  /* Get the demodulator value from SEARCH, so that it can compensate for
  ** any funny business it's been pulling with the combiner value.
  */
  #ifndef FEATURE_HWTC
  cnt2 = srch_scomb_get_vsc_time();
  #endif /* FEATURE_HWTC */

  /* Determine the phase of the symbol combiner */
  phase80 = cnt2 >> TIME_CDMA_PHASE_SHIFT;

  if ( phase80 < time_cdma.phase80 )
  {
    /* Phase has wrapped around - increase coarse 80ms count */

    qw_inc( ts_val, TIME_CDMA_80_MS );
  }

  /* Restore interrupts. */
  INTFREE( );


  /* Get ts_val in 80ms units (in the 1.25 ms tick position) */
  (void) qw_div_by_power_of_2( ts_val, ts_val, TIME_CDMA_80_MS_TICKS_SHIFT );

  /* Take the low dword, modulo Max slot cycle length * 2, and multiply it
     by 3 to get PN Rolls. (TIME_CDMA_MAX_SLOT is a power of 2) */
  slot_time = (uint16) (qw_lo( ts_val ) >> TIME_CDMA_MS_SHIFT);
  slot_time = (slot_time & (TIME_CDMA_MAX_SLOT*2 - 1) ) * 3;

  /* Add in the phase from the symbol combiner (0, 1, or 2 extra PN Rolls). */
  slot_time = slot_time + phase80;

  return slot_time;

} /* time_cdma_get_slot_time */



#ifndef MSM5000_IRAM_REV
/*=============================================================================

FUNCTION TIME_CDMA_GET_20MS_FRAME_TIME

DESCRIPTION
  This procedure is returns the 64-bit system timestamp in units of
  20 millisecond frame time (traffic / paging / access channel frame
  time).

DEPENDENCIES
  The value returned by this function is meaningless unless the demodulator
  is running and contains a valid value.
  This function DOES fall back on "Time Of Day", if CDMA time is not available

RETURN VALUE
  The current timestamp value, as specified above.

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_get_20ms_frame_time
(
  /* Address of qword in which to return 64-bit frame time. */
  time_type                       ts_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get timestamp accurate in 1.25ms units
  */
  (void) time_get( ts_val );

  /* Divide by 64*1024 to truncate the low 16 bits of the time stamp,
  ** then divide the 1.25 ms units by 16 to get 20ms units.
  */
  /* A divide by 16*(64*1024) = 2^20. So, use a bitshift for speed */

  (void) qw_div_by_power_of_2( ts_val, ts_val, 20 );

} /* time_cdma_get_20ms_frame_time */

#endif /* !MSM5000_IRAM_REV */



/*=============================================================================

FUNCTION TIME_CDMA_SET_FRAME_OFFSET

DESCRIPTION
   Set the frame offset, so calls to time_cdma_get_20ms_frame_offset_time( )
   are adjusted by the frame offset.

DEPENDENCIES
  The CDMA system clock must be set to a reasonable value in order for
  this function to perform a meaningful operation on the timestamp.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_set_frame_offset
(
  /* New 20ms frame offset */
  uint8                           frame_offset
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  time_cdma.frame_offset = frame_offset;

} /* time_cdma_set_frame_offset */



/*=============================================================================

FUNCTION TIME_CDMA_GET_20MS_FRAME_OFFSET_TIME

DESCRIPTION
  This procedure returns the 64-bit system timestamp (in units of
  20 millisecond frame time) adjusted by frame offset.

DEPENDENCIES
  The value returned by this function is meaningless unless the demodulator
  is running and contains a valid value.  This function does not fall back on
  "Time Of Day".

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_cdma_get_20ms_frame_offset_time
(
  /* Address of qword in which to return 64-bit frame time. */
  time_type                       ts_offset_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get timestamp accurate in 1.25ms units
  */
  (void) time_cdma_get( ts_offset_val );

  /* Divide by 64*1024 to truncate the low 16 bits of the time stamp,
  */
  (void) qw_div_by_power_of_2( ts_offset_val, ts_offset_val, 16 );

  /* Subtract frame_offset from the resulting time in 1.25 ms units
  */
  qw_dec( ts_offset_val, (dword) time_cdma.frame_offset );

  /* then divide the 1.25 ms units by 16 to get 20ms units.
  */
  (void) qw_div_by_power_of_2( ts_offset_val, ts_offset_val, 4 );

} /* time_cdma_get_20ms_frame_offset_time */
