/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M                 
                        
                M O D E M   T O   A P P ' S   F U N C T I O N S

GENERAL DESCRIPTION

  Implements modem <-> applications processor time communication functions


EXTERNALIZED FUNCTIONS

  TBD
    tbd


INITIALIZATION AND SEQUENCING REQUIREMENTS
  TBD


  Copyright(c) 2005 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/services/time/time_remote.c#11 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/15/06   jhs     Trigger timer interrupt in timer_remote_restore.
05/11/06   taw     Added function, time_remote_restore, to be used to resync
                   timers after apps power collapse; switched to using
                   smsm_poll_timed when waiting for other host state changes.
04/25/06   taw     Added function, time_remote_slow_clock_sync_poll, to be 
                   called when the modem is in RESET and the timers need
                   to be synced.
04/11/06   taw     Only inform the apps of changes in the TOD bases if the apps
                   isn't sleeping or in power collapse.
04/03/06   ptm     Moved smem_slow_clock_value and smem_slow_clock_sync to
                   shared memory structure.
03/09/06   jhs     Added functionality to sync time of  day before acquiring.
                   Changed Apps update time of day delta to 10ms.
02/22/06   ajn     Use timetick_get_raw() function instead of setting offset
                   to 0x0.  Minor function name changes.
01/26/06   jhs     Added write to AGPT_ENABLE register for power restore
                   support.
01/25/06   sam     Changed two INTLOCK/INTFREEs to MSM_INTLOCK/MSM_INTFREE to
                   guarantee timing under L4.
09/26/05   jhs     Created a threshold for sending an updated time of day bases
                   to the Apps.  Changed slow clock syncing so that it may be
                   done multiple times.  This is to support power collapse.
08/11/05   jhs     Updates.
06/28/05   ajn     File created based on several msm7500/services/time files

=============================================================================*/



/*=============================================================================

                                 INCLUDE FILES

=============================================================================*/

#include "time_remote.h"
#include "dem.h"

#ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC
  #include "customer.h"
  #include "timetick.h"
  #include "err.h"
  #include "smem_data_decl.h"
  #include "tramp.h"

  #ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM
    #include "smsm.h"
  #endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM */

  #ifdef FEATURE_SMEM_LOG
    #include "smem_log.h"
  #else /* !FEATURE_SMEM_LOG */
    #define SMEM_LOG_EVENT( id, d1, d2, d3 )
  #endif /* FEATURE_SMEM_LOG */
#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC */

#ifdef FEATURE_TOD_MULTIPROC_SYNC
  #if defined(IMAGE_MODEM_PROC) && !defined(FEATURE_STANDALONE_MODEM)
    #include "oncrpc.h"
    #include "oncrpc_proxy.h"
    #include "remote_apis.h"
    #include "sclk.h"
  #endif /* IMAGE_MODEM_PROC and !FEATURE_STANDALONE_MODEM */
#endif /* FEATURE_TOD_MULTIPROC_SYNC */

#include "msm.h"

/*=============================================================================

                               MACRO DEFINITIONS

=============================================================================*/

#if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
  /* 10 ms */
  #define TIME_REMOTE_DELTA_THRESHOLD_MS       10 
  #define TIME_REMOTE_DELTA_THRESHOLD \
            TIMETICK_SCLK_FROM_MS( TIME_REMOTE_DELTA_THRESHOLD_MS )

#endif /* FEATURE_TOD_MULTIPROC_SYNC && IMAGE_MODEM_PROC */


/*=============================================================================

                               MACRO DEFINITIONS

=============================================================================*/

#ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC
  #define TIME_REMOTE_LOG_EVENT_START       (SMEM_LOG_TIMETICK_EVENT_BASE + 0)
  #define TIME_REMOTE_LOG_EVENT_GOTO_WAIT   (SMEM_LOG_TIMETICK_EVENT_BASE + 1)
  #define TIME_REMOTE_LOG_EVENT_GOTO_INIT   (SMEM_LOG_TIMETICK_EVENT_BASE + 2)
#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC */


/*=============================================================================

                               TYPE DEFINITIONS

=============================================================================*/

#ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC
#if defined(FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM) && defined(IMAGE_MODEM_PROC)
typedef enum
{
  TIME_REMOTE_SMSM_START,
  TIME_REMOTE_SMSM_TIMEWAIT,
  TIME_REMOTE_SMSM_TIMEINIT
}
time_remote_smsm_state_type;

#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM && IMAGE_MODEM_PROC */
#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC */


#if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
typedef struct
{

  /* Copy of timestamp used as base for slow-clock time-of-day on Apps */
  time_type      apps_base_timestamp;

  /* Copy of time tick corresponding to above timestamp on Apps */
  timetick_type  apps_base_timetick;

  /* Flag to indicate whether or not the initial TOD bases have been
   * sent.
   */
   boolean       initialized;

} time_remote_tod_struct_type;
#endif /* FEATURE_TOD_MULTIPROC_SYNC && IMAGE_MODEM_PROC */


/*=============================================================================

                               DATA DECLARATIONS

=============================================================================*/

#ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC
#if defined(FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM) && defined(IMAGE_MODEM_PROC)

/* Shared Memory State Machine state for slow clock sync */
static time_remote_smsm_state_type  time_remote_smsm_state;

#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM && IMAGE_MODEM_PROC */
#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC */


#if defined(FEATURE_TOD_MULTIPROC_SYNC) && defined(IMAGE_MODEM_PROC)
static time_remote_tod_struct_type  time_remote_tod;
#endif /* FEATURE_TOD_MULTIPROC_SYNC && IMAGE_MODEM_PROC */

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */
/*=============================================================================

                   INTERNAL FUNCTION DEFINITIONS

=============================================================================*/



#ifdef FEATURE_TOD_MULTIPROC_SYNC

#if defined(IMAGE_MODEM_PROC) && !defined(FEATURE_STANDALONE_MODEM)
/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_APPS_TOD_HELPER_FUNCTION

DESCRIPTION
  Call the RPC call to inform the Apps processor of the change in
  TOD bases. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_update_apps_tod_helper_func
(
  /* ONCRPC Proxy command message */
  oncrpc_proxy_cmd_client_call_type *msg
)
{
  /* Timestamp to place time of day in */
  time_type base_timestamp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  qw_set( base_timestamp, msg->data[1], msg->data[2] );
  
  /* Only inform the apps if it is not power collapsed or sleeping */
  if (!dem_apps_sleeping())
  {
    /* Set apps base timetick and timestamp */
    time_tod_set_apps_bases( (timetick_type)msg->data[0], base_timestamp );
  }

  time_remote_tod.initialized = TRUE;

} 
#endif /* IMAGE_MODEM_PROC && !FEATURE_STANDALONE_MODEM */

#endif /* FEATURE_TOD_MULTIPROC_SYNC */



#ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC

#ifdef IMAGE_MODEM_PROC
/*=============================================================================

FUNCTION TIME_REMOTE_GET_NEXT_TICK (Modem)
 
DESCRIPTION
  The processor waits for a slow clock edge (counter value change), and
  then returns the new tick value.

DEPENDENCIES
  Must be called in INTLOCK context.

RETURN VALUE
  The slow clock counter after the counter changes.

SIDE EFFECTS
  None

=============================================================================*/

static timetick_type time_remote_get_next_tick( void )
{
  /* Initial slow clock counter value */
  timetick_type                   start_tick;

  /* Current slow clock counter value */
  timetick_type                   tick;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Poll timer until it changes - avg time = 15 usec worse case 30 usec,
     apps processor is waiting for non-zero base time, so if we end up at 0,
     must wait another 30 usecs */

  /* Capture the current tick value, so we can tell when the counter changes */
  start_tick = timetick_get_safe();

  /* Wait for the counter to change (and not be zero) */
  do {
    /* Read the current slow clock counter value ... */
    tick = timetick_get_safe();

    /* ... until it is different from the start value (and not zero) */
  } while( tick != start_tick  &&  tick == 0 );

  /* return the new tick value */
  return tick;

} /* time_remote_get_next_tick */
#endif /* IMAGE_MODEM_PROC */



#ifdef IMAGE_APPS_PROC
/*=============================================================================

FUNCTION TIME_REMOTE_WAIT_FOR_TICK_DELTA (Apps)
 
DESCRIPTION
  The apps processor waits for modem tick value to written to shared memory,
  and then returns the difference between the modem's slow clock count and the
  apps's slow clock counter.

DEPENDENCIES
  Must be called in INTLOCK context.

RETURN VALUE
  Difference btw modem and apps slow clock counters (modem - apps)

SIDE EFFECTS
  None

=============================================================================*/

static timetick_type time_remote_wait_for_tick_delta( void )
{
  /* Initial apps slow clock counter value */
  timetick_type                   tick_apps_start;

  /* Current apps slow clock counter value */
  timetick_type                   tick_apps;

  /* Modem slow clock counter value (from shared memory) */
  timetick_type                   tick_modem;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Record h/w value when the sync loop started */
  tick_apps_start = timetick_get_raw();

  /* Poll for modem writing of slow clock value - use polling to minimize the
     latency between when the modem writes the value and when the apps sees
     the value */

  do
  {
    /* Read the modem's slow clock time value (if available) */
    tick_modem = smem_data.smem_slow_clock_value;

    /* Re-read the applications h/w slow clock time value */
    tick_apps = timetick_get_raw();

    /* Loop until the tick_modem value is written (non-zero), or timeout */
  }
  while ( tick_modem == 0  &&  tick_apps-tick_apps_start < SCLK_SYNC_TIMEOUT );


  if( tick_modem == 0 )
  {
    ERR( "Never got time from modem s=%d a=%d", tick_apps_start, tick_apps, 0);
  }


  /* Return the difference between tick values */
  return tick_modem - tick_apps;

} /* time_remote_wait_for_tick_delta */
#endif /* IMAGE_APPS_PROC */



#ifdef IMAGE_MODEM_PROC
/*=============================================================================

FUNCTION TIME_REMOTE_SLOW_CLOCK_DO_SYNC (Modem SMSM version)
 
DESCRIPTION
  Modem processor SMSM based code which actually writes the time base into
  shared memory. The code polls the timer hardware until it changes and then
  writes the new value into shared memory. It uses polling to minimize
  latencies.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  The apps processor is waiting for the value to be written to shared memory.
  Once the value is written, the apps processor will unblock.

=============================================================================*/

static void time_remote_slow_clock_do_sync( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Lock interrupts, to ensure tight timing. */
  MSM_INTLOCK();

  /* Wait for a tick, and write tick value to shared memory */
  smem_data.smem_slow_clock_value = time_remote_get_next_tick();

  MSM_INTFREE();

} /* time_remote_slow_clock_do_sync - Modem SMSM */



/*=============================================================================

FUNCTION TIME_REMOTE_SMSM_EVENT_CB (Modem SMSM version)
 
DESCRIPTION
  Called by SMSM when an event occurs.  Does all of the modem side work
  associated with synchronizing the time base with the apps processor.
  
  This work happens either when time_init is called (if the apps processor
  is already waiting) or at interrupt level. This allows the tmc task to wait
  for time/SMD/RPC synchronizing to happen with out causing deadlock.

  When this routine sees that the apps is waiting (that is, SMSM_TIMEWAIT is
  set), it sends the apps processor the current time base using shared memory.
  When it sees that the apps time code is initialized (that is, SMSM_TIMEINIT
  is set), it sets the modem SMSM state SMSM_TIMEINIT.

DEPENDENCIES
  TRAMP must be initialized
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_remote_smsm_event_cb
(
  /* Shared Memory State Machine host */
  smsm_host_type                  host,

  /* Previous state */
  smsm_state_type                 unused_prev_state,

  /* Current state */
  smsm_state_type                 curr_state
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( time_remote_smsm_state )
  {
    case TIME_REMOTE_SMSM_START:
      if( host == SMSM_OTHER_HOST && (curr_state & SMSM_TIMEWAIT) != 0 )
      {
        SMEM_LOG_EVENT( TIME_REMOTE_LOG_EVENT_GOTO_WAIT, 0, 0, 0 );
        time_remote_smsm_state = TIME_REMOTE_SMSM_TIMEWAIT;
        smsm_set_state( SMSM_THIS_HOST, SMSM_TIMEWAIT );
        time_remote_slow_clock_do_sync( );
        smsm_clr_state( SMSM_OTHER_HOST, SMSM_TIMEWAIT );
      }
      break;
  
    case TIME_REMOTE_SMSM_TIMEWAIT:
      if( host == SMSM_OTHER_HOST && (curr_state & SMSM_TIMEINIT) != 0 )
      {
        SMEM_LOG_EVENT( TIME_REMOTE_LOG_EVENT_GOTO_INIT, 0, 0, 0 );
        time_remote_smsm_state = TIME_REMOTE_SMSM_START;
        smsm_set_state( SMSM_THIS_HOST, SMSM_TIMEINIT );
        smem_data.smem_slow_clock_value = 0;
      }
      break;
  
    case TIME_REMOTE_SMSM_TIMEINIT:
      break;
  
    default:
      ERR_FATAL( "Invalid timetick smsm state %x",
                 time_remote_smsm_state, 0, 0 );
      break;
  }

} /* time_remote_smsm_event_cb - Modem SMSM */
#endif /* IMAGE_MODEM_PROC */




/*=============================================================================

                   EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/



#ifdef FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM

#ifdef IMAGE_APPS_PROC
/*=============================================================================

FUNCTION TIME_REMOTE_SLOW_CLOCK_SYNC (Apps SMSM version)
 
DESCRIPTION
  Apps processor SMSM based code which synchronizes the time base for the
  slow clock timer.

  This is a blocking algorithm that uses SMSM to signal the apps state to
  the modem, but polls shared memory to get the modem state and time base
  data. It polls shared memory to minimize latencies.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_slow_clock_sync(void)
{
  /* Difference btw modem and application slow clock counter values */
  uint32                          delta;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ( HWIO_INM(AGPT_ENABLE, HWIO_FMSK(AGPT_ENABLE, EN) ) &
        HWIO_FMSK(AGPT_ENABLE, EN) ) == 0 )
  {
    HWIO_OUTM(AGPT_ENABLE,
              HWIO_FMSK(AGPT_ENABLE, EN),
              HWIO_FMSK(AGPT_ENABLE, EN));
  }

  SMEM_LOG_EVENT( TIME_REMOTE_LOG_EVENT_GOTO_WAIT, 0, 0, 0 );

  /* Lock out interrupts to keep the minimize the latency during this code */
  MSM_INTLOCK();

  /* inform modem that apps is waiting for time sync */
  smsm_set_state( SMSM_THIS_HOST, SMSM_TIMEWAIT );

  /* Wait for modem to write its sclk counter, and get the tick difference */
  delta = time_remote_wait_for_tick_delta();

  MSM_INTFREE();

  SMEM_LOG_EVENT( TIME_REMOTE_LOG_EVENT_GOTO_INIT, 0, 0, 0 );
  smsm_clr_state( SMSM_OTHER_HOST, SMSM_TIMEWAIT );
  smsm_set_state( SMSM_THIS_HOST, SMSM_TIMEINIT );

  timetick_set_sclk_offset( delta );

} /* time_remote_slow_clock_sync - Apps smsm */

/*=============================================================================

FUNCTION TIME_REMOTE_RESTORE
 
DESCRIPTION
  This function is used to resync the timers with the modem processor after
  apps power collapse.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  TRUE if timers were restored successfully
  FALSE if timers were not restored successfully

SIDE EFFECTS
  None

=============================================================================*/

boolean time_remote_restore(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear the states that indicate that timers are already synced */
  smsm_clr_state( SMSM_OTHER_HOST, SMSM_TIMEINIT );
  smsm_clr_state( SMSM_THIS_HOST,  SMSM_TIMEINIT );

  /* Function to sync the timers */
  time_remote_slow_clock_sync();

  /* Wait for timers to be synced, indicated by the ARM9
     entering the TIMEINIT state */
  if ( !smsm_poll_timed( SMSM_TIMEINIT, 
                         SMSM_TIMEINIT, 
                         TRUE, 
                         DEM_ST_CHG_POLL_TIME_MS ) )
  {
    /* If the poll timed out, then the sync failed. */
    return FALSE;

  } /* poll timed out */

  /* Trigger the timer interrupt, so that we will always reprocess the
     timer list. */
  tramp_set_interrupt( TRAMP_SLP_TICK_ISR );

  return TRUE;

} /* time_remote_restore */

#endif /* IMAGE_APPS_PROC */



#ifdef IMAGE_MODEM_PROC
/*=============================================================================

FUNCTION TIME_REMOTE_SLOW_CLOCK_SYNC (Modem SMSM version)
 
DESCRIPTION
  Modem processor SMSM based code which synchronizes the time base for the
  slow clock timer.

  Sets up the smsm event callback - which is where all the work occurs.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_slow_clock_sync(void)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SMEM_LOG_EVENT( TIME_REMOTE_LOG_EVENT_START, 0, 0, 0 );

  time_remote_smsm_state = TIME_REMOTE_SMSM_START;

  smsm_cb_reg( SMSM_OTHER_HOST, SMSM_TIME, time_remote_smsm_event_cb );

  /* Call the callback now, in case we've missed the event - must lock
     interrupts around the call because timetick_smsm_event_cb is called from
     interrupt level */

  INTLOCK();

  time_remote_smsm_event_cb( SMSM_OTHER_HOST,
                             (smsm_state_type) 0,
                             smsm_get_state(SMSM_OTHER_HOST) );
  INTFREE();

} /* time_remote_slow_clock_sync - Modem SMSM */


/*===========================================================================

FUNCTION TIME_REMOTE_SLOW_CLOCK_SYNC_POLL

DESCRIPTION
  This function is used to resync the timers on the apps side and the modem
  side.  This function is called after exiting from power collapse when the 
  modem is in the RESET state, and so polls for the timer state changes, and 
  then calls the time_remote callback when necessary.

PARAMETERS
  None

RETURN VALUE
  TRUE if timers were synced successfully; FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  Timers are synced with the apps processor. 

===========================================================================*/
boolean time_remote_slow_clock_sync_poll (void)
{  
#ifdef FEATURE_AMSS_RESTART

  /* Previous ARM11 state to hand to time_remote_smsm_event_cb */
  smsm_state_type                 arm11_prev_state = SMSM_UNKNOWN;

  /* Current ARM11 state to hand to time_remote_smsm_event_cb */
  smsm_state_type                 arm11_state;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Poll for ARM11 to change state to SMSM_TIMEWAIT */
  if ( !smsm_poll_timed( SMSM_TIMEWAIT, 
                         SMSM_TIMEWAIT, 
                         TRUE, 
                         DEM_ST_CHG_POLL_TIME_MS ) )
  {
    /* We are already in the RESET state, which means a fatal error
       occurred, so if the poll timed out, the whole system needs to be
       reset. */
    return FALSE;

  } /* poll timed out */
  else
  {
    time_remote_smsm_event_cb( SMSM_OTHER_HOST, 
                               arm11_prev_state, 
                               arm11_state );
    arm11_prev_state = arm11_state;
  } /* poll didn't time out */

  /* Now poll for the ARM11 to change state to SMSM_TIMEINIT */
  if ( !smsm_poll_timed( SMSM_TIMEINIT, 
                         SMSM_TIMEINIT, 
                         TRUE, 
                         DEM_ST_CHG_POLL_TIME_MS ) )
  {
    /* We are already in the RESET state, which means a fatal error
       occurred, so if the poll timed out, the whole system needs to be
       reset. */
    return FALSE;

  } /* poll timed out */              
  else
  {
    time_remote_smsm_event_cb( SMSM_OTHER_HOST, 
                               arm11_prev_state, 
                               arm11_state );
  } /* poll didn't time out */

#endif /* FEATURE_AMSS_RESTART */

  return TRUE;

}  /* time_remote_slow_clock_sync_poll - Modem SMSM */

#endif /* IMAGE_MODEM_PROC */

#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC_SMSM */

#endif /* FEATURE_SLOW_CLOCK_MULTIPROC_SYNC */



#ifdef FEATURE_TOD_MULTIPROC_SYNC

#if defined(IMAGE_MODEM_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_INIT 
 
DESCRIPTION

  MODEM ONLY

  This is used to initialize and setup the modem copy of the time of day
  bases for the apps which is used in calculating whether or not to update
  the apps time of day bases when the modem time of day bases are updated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_init( void )
{

  /* Don't do anything for now */

} /* time_remote_init */
#endif /* IMAGE_MODEM_PROC */

#if defined(IMAGE_MODEM_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_APPS_TEST

DESCRIPTION
  Checks to see if the change in the bases will cause a large enough
  change in the time of day to warrant forwarding the change.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - Indicates if the threshold was meet or not.

SIDE EFFECTS
  None

=============================================================================*/


boolean time_remote_update_apps_test
(
  /* Timestamp used as base for slow-clock time-of-day */
  time_type                       base_timestamp,

  /* Time tick corresponding to above timestamp */
  timetick_type                   base_timetick
)
{

  /* Slow-clock based time-of-day in ts_timestamp format using old bases */
  time_type                          tod_old;

  /* Slow-clock based time-of-day in ts_timestamp format using new bases */
  time_type                          tod_new;

  /* Slow-clock based time-of-day difference in ts_timestamp format */
  time_type                          tod_diff;

  /* Slow-clock based time-of-day threshold in ts_timestamp format */
  time_type                          threshold;

  /* Slow clock counter value to convert to timestamp using old bases */
  timetick_type                      delta_old;

  /* Slow clock counter value to convert to timestamp using new bases */
  timetick_type                      delta_new;

  /* Holder to indicate if the first timestamp is less than, greater than,
     or equal to the second timestamp. */
  int32                              less_greater_equal;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* NOTE: This function has many early returns. */

  if( time_remote_tod.initialized == FALSE )
  {
    return FALSE;
  }

  /* Copy the base timestamp to the output timestamp value */
  qw_equ( tod_old, time_remote_tod.apps_base_timestamp );

  /* Get the elapsed number of ticks since recording the timetick base */
  delta_old = timetick_get_safe() - time_remote_tod.apps_base_timetick;

  /* Advance the output timestamp by the elapsed slow clocks */
  sclk_to_timestamp( tod_old, delta_old );
   
  /* Copy the base timestamp to the output timestamp value */
  qw_equ( tod_new, base_timestamp );

  /* Get the elapsed number of ticks since recording the timetick base */
  delta_new = timetick_get_safe() - base_timetick;

  /* Advance the output timestamp by the elapsed slow clocks */
  sclk_to_timestamp( tod_new, delta_new );

  /* Compare the two timestamps */
  less_greater_equal = qw_cmp( tod_new, tod_old );

  /* Get the difference between the two timestamps */
  if( less_greater_equal == -1 )
  {
    /* tod_old is greater */
    qw_sub( tod_diff, tod_old, tod_new );
  }
  else if( less_greater_equal == 1 )
  {
    /* tod_new is greater */
    qw_sub( tod_diff, tod_new, tod_old );
  }
  else
  {
    /* Equal no need to do anything. */
    return FALSE;
  }

  /* Setup the theshold values. */
  qw_set( threshold, 0, TIME_REMOTE_DELTA_THRESHOLD );

  /* See if it is within the threshold. */
  if( qw_cmp( tod_diff, threshold ) == -1 )
  {
    return FALSE;
  }

  /* Store the new bases */
  qw_equ( time_remote_tod.apps_base_timestamp, base_timestamp );
  time_remote_tod.apps_base_timetick = base_timetick;

  return TRUE;

} /* time_remote_update_apps_test */
#endif /* IMAGE_MODEM_PROC */

#if defined(IMAGE_MODEM_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_APPS_TOD

DESCRIPTION
  Setup a the RPC proxy to call a call back in the future to make the
  RPC call to inform the Apps processor of the change in TOD bases. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_update_apps_tod
(
  /* Timestamp used as base for slow-clock time-of-day */
  time_type                       base_timestamp,

  /* Time tick corresponding to above timestamp */
  timetick_type                   base_timetick
)
{

  #if !defined(FEATURE_STANDALONE_MODEM)

  /* ONCRPC Proxy command structure */
  oncrpc_proxy_cmd_client_call_type *time_remote_update_apps_ptr;

  /* Indicator of whether or not to update the apps side */
  boolean                            send_update;

  #endif /* !FEATURE_STANDALONE_MODEM */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if !defined(FEATURE_STANDALONE_MODEM)

  /* Prevent changes to bases while comparing. */
  INTLOCK();

  /* Check to see if we should update the apps side. */
  send_update = time_remote_update_apps_test( base_timestamp, base_timetick );

  /* We have changed the bases we can unlock now. */
  INTFREE();

  /* Check to see if we need to update the apps side. */
  if( send_update == TRUE )
  {
    /* Get a message pointer */
    time_remote_update_apps_ptr = remote_apis_client_call_get();
    /* Check that the message pointer is valid */
    if( time_remote_update_apps_ptr != NULL )
    {
      /* Fill up the pointer data */
      time_remote_update_apps_ptr->client_call =
        (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
          time_remote_update_apps_tod_helper_func;
      time_remote_update_apps_ptr->data[0] = base_timetick;
      time_remote_update_apps_ptr->data[1] = qw_hi( base_timestamp );
      time_remote_update_apps_ptr->data[2] = qw_lo( base_timestamp );
      /* Queue up the message */
      oncprc_proxy_client_call_send( time_remote_update_apps_ptr );
      /* Explicitly set to NULL */
      time_remote_update_apps_ptr = NULL;
    }
  }
  
  #else /* !FEATURE_STANDALONE_MODEM */
  
  /* Do nothing, there is no Apps processor to notify. */
  
  #endif /* !FEATURE_STANDALONE_MODEM */

} /* time_remote_update_apps_tod */
#endif /* IMAGE_MODEM_PROC */

#if defined(IMAGE_MODEM_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_SMSM_SET_APPS_TOD_CB
 
DESCRIPTION
  Called by SMSM when an event occurs.  Does all of the modem side work
  associated with initializing the time of day base with the apps processor.
  
  This work happens either when time_tod_init is called (if the apps
  processor is already waiting) or at interrupt level. This allows the
  tmc task to wait for time/SMD/RPC synchronizing to happen with out
  causing deadlock.

  When this routine sees that the apps is ready (that is, SMSM_RPCINIT
  is set), it sends the apps processor the current time of day base
  using shared memory.

DEPENDENCIES
  TRAMP must be initialized
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_remote_smsm_set_apps_tod_cb
(
  /* Shared Memory State Machine host */
  smsm_host_type                  host,

  /* Previous state */
  smsm_state_type                 unused_prev_state,

  /* Current state */
  smsm_state_type                 curr_state
)
{

  #if !defined(FEATURE_STANDALONE_MODEM)
  /* Timestamp used as base for slow-clock time-of-day */
  time_type                           base_timestamp;

  /* Time tick corresponding to above timestamp */
  timetick_type                       base_timetick;

  /* ONCRPC Proxy command structure */
  oncrpc_proxy_cmd_client_call_type  *time_remote_update_apps_ptr;
  #endif /* FEATURE_STANDALONE_MODEM */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( host == SMSM_THIS_HOST && (curr_state & SMSM_RPCINIT) != 0 )
  {

    #if !defined(FEATURE_STANDALONE_MODEM)

    /* Grab the bases that were assigned in
     * time_remote_init_time_of_day
     */
    base_timetick = time_remote_tod.apps_base_timetick ;
    qw_equ( base_timestamp, time_remote_tod.apps_base_timestamp );

    /* Get a message pointer */
    time_remote_update_apps_ptr = remote_apis_client_call_get();
    /* Check that the message pointer is valid */
    if( time_remote_update_apps_ptr != NULL )
    {
      /* Fill up the pointer data */
      time_remote_update_apps_ptr->client_call =
        (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
          time_remote_update_apps_tod_helper_func;
      time_remote_update_apps_ptr->data[0] = base_timetick;
      time_remote_update_apps_ptr->data[1] = qw_hi( base_timestamp );
      time_remote_update_apps_ptr->data[2] = qw_lo( base_timestamp );
      /* Queue up the message */
      oncprc_proxy_client_call_send( time_remote_update_apps_ptr );
      /* Explicitly set to NULL */
      time_remote_update_apps_ptr = NULL;
    }

    #else /* !FEATURE_STANDALONE_MODEM */
    
    /* Do nothing, there is no Apps processor to notify. */
    
    #endif /* !FEATURE_STANDALONE_MODEM */

    /* We only do this once, deregister */
    smsm_cb_dereg( SMSM_THIS_HOST,
                   SMSM_TIME,
                   time_remote_smsm_set_apps_tod_cb );
  }

} /* time_remote_smsm_set_apps_tod_cb */
#endif /* IMAGE_MODEM_PROC */

#if defined(IMAGE_MODEM_PROC)
/*=============================================================================

FUNCTION TIME_REMOTE_INIT_TIME_OF_DAY
 
DESCRIPTION
  Modem processor SMSM based code which synchronizes the time of day base.

  Sets up the smsm time of day callback - which is where all the work occurs.

DEPENDENCIES
  SMSM must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_init_time_of_day
(
  /* Base time tick */
  timetick_type                   tick,

  /* Base time stamp */
  time_type                       stamp
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Store the new bases */
  qw_equ( time_remote_tod.apps_base_timestamp, stamp );
  time_remote_tod.apps_base_timetick = tick;

  smsm_cb_reg( SMSM_THIS_HOST,
               SMSM_TIME,
               time_remote_smsm_set_apps_tod_cb );

  /* Call the callback now, in case we've missed the event - must lock
     interrupts around the call because timetick_smsm_event_cb is called from
     interrupt level */

  INTLOCK();

  time_remote_smsm_set_apps_tod_cb( SMSM_THIS_HOST,
                                    (smsm_state_type) 0,
                                    smsm_get_state(SMSM_THIS_HOST) );
  INTFREE();

} /* time_remote_init_time_of_day */
#endif /* IMAGE_MODEM_PROC */

#endif /* FEATURE_TOD_MULTIPROC_SYNC */

