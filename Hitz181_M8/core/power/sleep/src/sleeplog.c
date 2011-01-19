/*===========================================================================

                     S L E E P L O G   C    F I L E

GENERAL DESCRIPTION
   This file contains log types used internally by the Sleep task.
   It should NOT be included by any source file outside the Sleep task!

EXTERNALIZED FUNCTIONS

  sleep_log_event(sleep_event_type, uint32, uint32, uint32)
    Logs the event to shared memory if SMEM_LOG is defined else
    it logs to a local buffer

INITIALIZATION AND SEQUENCING REQUIREMENTS

  NONE

Copyright (c) 2007-2008 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   clm     1H08 sleep re-work.
10/16/07   cab     Remove dead function
10/04/07   taw     Added support for sleeplog_save_name(), to store a name
                   to the shared memory area.  Also, removed featurization 
                   around sleeplog_init, so it can be called from sleep_init.
08/02/07   ebb     Fixed throttling function for sleep logging.
07/18/07   ebb     Moved from data/ functions from sleepmemlog.c => 
                   sleeplog.c, integrated in sleep_diag_task_stats()
                   and sleep_diag_append_name().
07/16/07   ebb     Added log_new_sleep_voters() to pack all new dynamic sleep
                   voters into shared/ local memory.
06/01/07   ebb     Initial revision

===========================================================================*/

/*==========================================================================
                              INCLUDE FILES
============================================================================*/

#include "timetick.h"
#include "rex.h"
#include "sleep.h"
#include "sleepi.h"
#include "sleeplog.h"
#include "log_codes.h"
#include "log.h"

#ifdef FEATURE_SMEM
  #include "AEEstd.h"
  #include "smem.h"
  #include "err.h"
#endif /* FEATURE_SMEM */

/*===========================================================================
                    LOCAL DATA DECLARATIONS 
===========================================================================*/

/* Size of local buffer for logging */
#define SLEEP_LOCAL_LOG_SIZE    150
         
#ifdef IMAGE_MODEM_PROC   /* Running on Modem */
  #define SLEEP_DIAG_ID           LOG_SLEEP_STATS_C
#else                     /* Running on Apps Proc */
  #define SLEEP_DIAG_ID           LOG_SLEEP_APPS_STATS_C
#endif /* IMAGE_MODEM_PROC */

#ifdef FEATURE_SMEM
  /* Store the modem client names at the top of the static buffer */
  #ifdef IMAGE_MODEM_PROC
    #define SMEM_OFFSET()                0
  #else
    #define SMEM_OFFSET()                ( MAX_NUM_SLEEP_CLIENTS *   \
                                       (SLEEPLOG_CLIENT_LIST_MAX_STR + 1) )
  #endif /* IMAGE_MODEM_PROC */
#endif /* FEATURE_SMEM */

/* Struct to hold sleep events*/
typedef struct 
{
  /* Event that occurred */
  uint32                  identifier;

  /* Timestamp of event*/
  timetick_type           timetick;

  /* Data to log */
  uint32                  data1;
  uint32                  data2;
  uint32                  data3;

} sleep_trace_event_type;

typedef struct
{
  /* Index of last event */
  uint32                  last;

  /* Buffer to hold trace events */
  sleep_trace_event_type  events[ SLEEP_LOCAL_LOG_SIZE ];

} sleep_trace_type;

#ifndef FEATURE_SMEM_LOG
/* Sleep trace structure */
static sleep_trace_type         sleep_trace;

/* Pointer to last event, used in sleepmemlog.cmm to dump struct */
static uint32*                  sleep_event_ptr;

#endif /* FEATURE_SMEM_LOG */

/* Static variables to save previous states and 
   throttle debug output*/
static boolean              prev_uart_clk_rgm =   FALSE;
static uint64               prev_new_sleep_voters = 0xFFFFFFFF;
static rex_sigs_type        prev_old_sleep_voters = 0xFFFFFFFF;

/*** ADD TO THIS FUNCTION TO THROTTLE SLEEP LOGGING EVENTS ***/
/*===========================================================================
FUNCTION      sleep_log_throttle

DESCRIPTION   Compare current input against previously saved 
              input to see if sleep state has changed

ARGUMENTS     uint32 - event that just occured
              a32, b32, c32 - arbitrary data to be logged.

RETURN VALUE  true - sleep state has changed
              false - sleep state is the same

SIDE EFFECTS  None
===========================================================================*/

static boolean sleep_log_throttle
(
  uint32              event,
  uint32              a32,
  uint32              b32,
  uint32              c32
)
{

  /* Default to true, ie: unrecognized events ALWAYS get logged */
   boolean changed = TRUE;

/*------------------------------------------------------------------*/

  switch( event )
  {
    /* If we are not sleeping see if the votes have changed */
    case NO_SLEEP_OLD:
      changed = ( ( gSleepInfo.log.task_stats.not_okts & ~0x8000 ) != 
                  ( prev_old_sleep_voters & ~0x8000 ) );
      prev_old_sleep_voters = 
        gSleepInfo.log.task_stats.not_okts;
      break;

    /* Check to see if new sleep voters have changed */
    case NO_SLEEP_NEW:
      changed = ( gSleepInfo.log.curr_not_okts_m != 
                  prev_new_sleep_voters );
      prev_new_sleep_voters = gSleepInfo.log.curr_not_okts_m;
      break;

    /* Check legacy UART variable to see if they need the clock */
    case MOD_UART_CLOCK:
      changed = ( prev_uart_clk_rgm != sleep_uart_clock_rgm );
      prev_uart_clk_rgm = sleep_uart_clock_rgm;
      break;
  }
  return changed;
} /* sleep_log_throttle */


/*===========================================================================
FUNCTION      sleeplog_init

DESCRIPTION   Initialize the local sleeplog buffer.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void sleeplog_init ( void )
{
#ifndef FEATURE_SMEM_LOG

  /* Make sure we start from the beginning */
  sleep_trace.last = 0;

  /* Clear the entire buffer */
  memset(sleep_trace.events, 0, 
         sizeof( sleep_trace.events[ 0 ] ) * SLEEP_LOCAL_LOG_SIZE );

#endif /* FEATURE_SMEM_LOG */

} /* sleeplog_init */


/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION      sleep_log_event

DESCRIPTION   Write an entry in the local memory event log.

              The log idx is treated with extreme caution to prevent writing
              outside of the event array.

ARGUMENTS     event - id tag for the event
              a32, b32, c32 - event related data items


RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void sleep_log_event 
(
  /* Event to be logged */
  uint32              event, 

  /* Arbitrary data to log */
  uint32              a32, 
  uint32              b32, 
  uint32              c32 
)
{
#ifndef FEATURE_SMEM_LOG
    /* Pointer to current sleep event */
  sleep_trace_event_type    *e;
#endif /* FEATURE_SMEM_LOG */

/*-------------------------------------------------------------------------*/

  /* Compare to previous state to throttle output*/
  if ( !sleep_log_throttle( event, a32, b32, c32 ) ) 
  {
    return;
  }

  /* If we are on a shared memory target redirect to shared memory logging 
     else log to local buffer */
#ifdef FEATURE_SMEM_LOG

  SMEM_LOG_EVENT( event, a32, b32, c32 );

#else /* !FEATURE_SMEM_LOG */

  /* Circular shared memory log buffer.
     Reached end of allocated space so reset index */
  if( sleep_trace.last == SLEEP_LOCAL_LOG_SIZE )
  {
    sleep_trace.last = 0;
  }

  /* Get pointer to current event */
  e = &sleep_trace.events[ sleep_trace.last ];

  /* Set sleep_event_ptr to next event (Used to dump event struct) */
  sleep_event_ptr = (uint32*)&sleep_trace.events[ sleep_trace.last + 1 ];

  /* write log data into shared memory buffer with valid idx */
  e->identifier = event;
  e->timetick   = timetick_get_safe();
  e->data1      = a32;
  e->data2      = b32;
  e->data3      = c32;

  /* Increment index */
  sleep_trace.last++;

#endif /* !FEATURE_SMEM_LOG */

} /* sleep_log_event */


/*===========================================================================

FUNCTION SLEEP_DIAG_APPEND_NAME

DESCRIPTION
  Copies at most SLEEPLOG_CLIENT_LIST_MAX_STR characters from src to dest,
  copying or appending the NUL terminator as necessary.

DEPENDENCIES
  There are at least SLEEPLOG_CLIENT_LIST_MAX_STR + 1 valid char locations
  starting at dest. src and dest do not overlap.

RETURN VALUE
  A pointer to the first address past the NULL terminator.

SIDE EFFECTS
  None

===========================================================================*/

static char *sleep_diag_append_name(char *d, const char *s)
{
  char  *dest = d;
  const char  *src = s;
    /* loop pointers */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while ( dest - d < SLEEPLOG_CLIENT_LIST_MAX_STR && *src != '\0')
  {
    *dest++ = *src++;
  }

  *dest = '\0';

  return dest + 1;

} /* sleep_log_append_name */

/*===========================================================================

FUNCTION SLEEP_DIAG_TASK_STATS

DESCRIPTION
  Called from sleep task while not inside an INTLOCK, this function
  will assemble and send a log packet to diag.  This includes sending
  a header with statistical information, old sleep voters that are voting
  not okts, and information on the total number of negate voters.  This
  is followed by a list of strings that indicate the new sleep voters that
  are voting negate.  The new sleep voters are captured in the var
  sleep.log.curr_not_okts_m at the critical point in the sleep task where
  a decision on whether to sleep is made.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleep_diag_task_stats( void )
{
  /* buffer in which to merge new voter name strings */
  char name_buffer[MAX_NUM_SLEEP_CLIENTS * (SLEEPLOG_CLIENT_LIST_MAX_STR + 1)];

  /* used to traverse the buffer when new names are added */
  char   *curr_ptr = name_buffer;

  /* used to traverse log packet allocation while things are added */
  void   *dest_ptr;

  /* ptr to base of diag log packet allocation */
  uint8  *log_ptr;

  /* used to store total size needed from diag for log packet */
  uint32  packet_size;

  /* loop variable */
  uint8   i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* zero out before counting up, n_client_strings is legacy */
  gSleepInfo.log.task_stats.cl_hdr.n_not_okts = 0;
  gSleepInfo.log.task_stats.cl_hdr.n_client_strings = 0;

  /* count num not okts voters, and merge client names into local buf */
  for (i = 0; i < gSleepInfo.log.num_clients; i++)
  {
    if (gSleepInfo.log.curr_not_okts_m & 0x1)
    {
      gSleepInfo.log.task_stats.cl_hdr.n_not_okts++;
      gSleepInfo.log.task_stats.cl_hdr.n_client_strings++;
      curr_ptr = sleep_diag_append_name(curr_ptr, gSleepInfo.client_name[i]);
    }

    /* shift right */
    gSleepInfo.log.curr_not_okts_m >>= 1;
  }

  /* diag header plus sleep log header plus not okts string names */
  packet_size = sizeof(log_hdr_type) + sizeof(sleeplog_task_stats_v2) +
                (curr_ptr - name_buffer);

  /* get allocated memory from diag */
  log_ptr = (uint8 *) log_alloc(SLEEP_DIAG_ID, packet_size);

  if ( log_ptr != NULL )
  {
    dest_ptr = (sleeplog_task_stats_v2 *) (log_ptr + sizeof(log_hdr_type));

    /* clear memory to prevent garbage bits from getting through */
    memset(dest_ptr, 0, packet_size - sizeof(log_hdr_type));

    /* copy sleep log header */
    memcpy(dest_ptr, &gSleepInfo.log.task_stats, 
           sizeof(sleeplog_task_stats_v2));

    /* copy not okts client names */
    dest_ptr = (char *)dest_ptr + sizeof(sleeplog_task_stats_v2);
    memcpy(dest_ptr, name_buffer, (curr_ptr - name_buffer));

    /* send the log to diag */
    log_commit( log_ptr );
  }

} /* sleep_diag_task_stats */

  
#ifdef FEATURE_SMEM
/*===========================================================================

FUNCTION SLEEPLOG_SAVE_NAME

DESCRIPTION
  Writes the name of the const char[] passed in to shared memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleeplog_save_name 
( 
  const char      *name     
   /* Name of the client registering to vote */
)
{
  char *name_buffer;
  uint32 index = gSleepInfo.num_clients * (SLEEPLOG_CLIENT_LIST_MAX_STR + 1);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( gSleepInfo.log.smem_names == NULL )
  {
    /* Allocate enough space for all the voter names        */
    /* 2 processors *  clients/proc * 8 bytes/client + '\0' */
    gSleepInfo.log.smem_names = 
      smem_alloc( SMEM_SLEEP_STATIC, 2 * MAX_NUM_SLEEP_CLIENTS * 
          	(SLEEPLOG_CLIENT_LIST_MAX_STR + 1) ); 
       
    if( gSleepInfo.log.smem_names == NULL )
    {
      ERR_FATAL("Unable to allocate shared memory",0,0,0);
    }
       
    /* Clear the memory just in case */
    memset( gSleepInfo.log.smem_names + SMEM_OFFSET(), 0, 
            (MAX_NUM_SLEEP_CLIENTS * (SLEEPLOG_CLIENT_LIST_MAX_STR + 1) ) );
  }

  name_buffer = gSleepInfo.log.smem_names + SMEM_OFFSET();

  /* Copy client's name into static memory */
  std_strlcpy( &name_buffer[ index ], name, SLEEPLOG_CLIENT_LIST_MAX_STR + 1 );
}

#endif /* FEATURE_SMEM */

/*===========================================================================

FUNCTION SLEEPLOG_EMIT_TASK_DIAG_LOGS

DESCRIPTION
  This procedure emits the sleep task diag logs.

DEPENDENCIES
  Called only from sleep task

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleeplog_emit_task_diag_logs
( 
  log_code_type logType 
)
{
  if ( log_status(logType) )
  {
    /* If old or new sleep voting logged a change */
    if ( ( gSleepInfo.log.task_stats.not_okts != gSleepInfo.log.not_okts ) ||
         ( gSleepInfo.log.curr_not_okts_m != gSleepInfo.log.prev_not_okts_m ) )
    {
      /* move current to prev, before we call log task stats */
      gSleepInfo.log.prev_not_okts_m = gSleepInfo.log.curr_not_okts_m;
      sleep_diag_task_stats();
      gSleepInfo.log.not_okts = gSleepInfo.log.task_stats.not_okts;
    }
  }
} /* sleeplog_emit_task_diag_logs */

