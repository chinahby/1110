#ifndef SLEEPLOG_H
#define SLEEPLOG_H
/*===========================================================================

                     S L E E P    H E A D E R    F I L E

DESCRIPTION
   This file contains log types used internally by the Sleep task.
   It should NOT be included by any source file outside the Sleep task!

    Copyright (c) 2004-2008
                    by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleeplog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   clm     1H08 sleep re-work.
10/04/07   taw     Added sleeplog_save_name() and sleeplog_init().
07/18/07   ebb     Integrated sleepmemlog.h into this file.
06/16/06   jwob    Updated to sleeplog_task_stats_v2, added
                   sleeplog_client_list_header_type for appended sub-section,
                   and SLEEPLOG_CLIENT_LIST_MAX_STR,
                   SLEEPLOG_CLIENT_LIST_MAX_NUM to control logging bandwidth.
01/25/06   ddh     Added LOG_APPS_SLEEP_STATS_C
10/05/04   ajn     Initial revision.

===========================================================================*/

#include "log_codes.h"
#include "log.h"

#ifdef FEATURE_SMEM_LOG
  #include "smem_log.h"
#endif /* FEATURE_SMEM_LOG */

/*===========================================================================

                           LOG CODES USED IN SLEEP

  Sleep Task Statistics all processors
    - LOG_SLEEP_STATS_C               

  APPS Sleep Task Statistics dual processor systems only
    - LOG_APPS_SLEEP_STATS_C 


  Sleep controller statistics ...
    1X    - LOG_1X_SLEEP_CONTROLLER_STATS_C
    HDR   - LOG_HDR_SLEEP_CONTROLLER_STATS_C
    GSM   - LOG_GSM_SLEEP_CONTROLLER_STATS_C
    WCDMA - LOG_WCDMA_SLEEP_CONTROLLER_STATS_C

  Reserved for future use ... 
    - LOG_SLEEP_STATS_RESERVED2_C
    - LOG_SLEEP_STATS_RESERVED3_C
    
===========================================================================*/


/*==========================================================================
     SHARED/LOCAL MEMORY DEFINITIONS
============================================================================*/
/*   If the target has smem facilities log events to shared memory
**   else we will log to a local buffer
*/

#ifndef FEATURE_SMEM_LOG
  #define SMEM_LOG_SLEEP_EVENT_BASE         0x00080000
#endif /* FEATURE_SMEM_LOG */

/* Event defines for using SLEEP to log what occurs during sleep and
 * power collapse. */

#define NO_SLEEP_OLD                       (SMEM_LOG_SLEEP_EVENT_BASE + 0x1)
#define INSUF_TIME                         (SMEM_LOG_SLEEP_EVENT_BASE + 0x2)                                        
#define MOD_UART_CLOCK                     (SMEM_LOG_SLEEP_EVENT_BASE + 0x3)    
#define SLEEP_INFO                         (SMEM_LOG_SLEEP_EVENT_BASE + 0x4)
#define MOD_TCXO_END                       (SMEM_LOG_SLEEP_EVENT_BASE + 0x5) 
#define MOD_ENTER_TCXO                     (SMEM_LOG_SLEEP_EVENT_BASE + 0x6)             
#define NO_SLEEP_NEW                       (SMEM_LOG_SLEEP_EVENT_BASE + 0x7)


/*===========================================================================
      General Header Data Types 
===========================================================================*/


/*-------------------------------------------------------------------------
      Version IDs 
-------------------------------------------------------------------------*/

#define SLEEPLOG_VERSION_1        1
#define SLEEPLOG_VERSION_2        2
#define SLEEPLOG_VERSION_3        3



/*-------------------------------------------------------------------------
      Packet Type IDs 
-------------------------------------------------------------------------*/

typedef enum
{                 
  SLEEPLOG_ID_TASK_STATS=1,
}
sleeplog_id_type;



/*-------------------------------------------------------------------------
      Packet header type 
-------------------------------------------------------------------------*/

typedef struct
{
  /* Size of the packet, including this header */
  uint16                         size;

  /* Id for the packet */
  sleeplog_id_type               id;

  /* Version number for the packet id type */
  uint8                          ver;
}
sleeplog_header_type;


typedef struct
{
  /* Total number of "new" clients not voting to sleep */
  uint8                         n_not_okts;

  /* Number of client ID strings that follow in this packet */
  uint8                         n_client_strings;

  /* Upper bound on client name length */
  uint8                         max_name_len;

  /* Upper bound on number of client names in packet */
  uint8                         max_names;
}
sleeplog_client_list_header_type;



/*===========================================================================

                       LOG_SLEEP_STATS_C DECLARATIONS

===========================================================================*/

typedef struct
{
  /* General packet header */
  sleeplog_header_type            hdr;

  /* Timestamp, based on 32-bit slow clock counter (~32kHz) */
  uint32                          timestamp;

  /* Total "active" (non sleep task) processing time (slow clocks)  */
  uint32                          t_active;

  /* Total "halt" time (in sleep task, with TCXO on) (slow clocks) */
  uint32                          t_halt;

  /* Total "halt" time with TCXO on only for DIAG (slow clocks) */
  uint32                          t_halt_diag;

  /* Total TCXO off time (slow clocks) */
  uint32                          t_tcxo_off;

  /* Total time spent in PC (slow clocks) */
  uint32                          t_pc;

  /* Number of halts with TCXO on */
  uint32                          n_halt;

  /* Number of halts with TCXO on only for DIAG */
  uint32                          n_halt_diag;

  /* Number of times TCXO is turned off */
  uint32                          n_tcxo_off;

  /* Number of times in PC */
  uint32                          n_pc;

  /* Votes to keep TCXO on */
  uint32                          not_okts;

  /* Sub-section header */
  sleeplog_client_list_header_type  cl_hdr;
}
sleeplog_task_stats_v2;

/* Header for Sleep Task Stats packet, version 2 */
#define SLEEPLOG_TASK_STATS_V2_HDR \
  { \
    sizeof(sleeplog_task_stats_v2), \
      /* Size must be increased as strings are appended to the packet */ \
    SLEEPLOG_ID_TASK_STATS, \
    SLEEPLOG_VERSION_2 \
  }

/* Client list parameters */

#define SLEEPLOG_CLIENT_LIST_MAX_STR 8
    /* Maximum significant characters from client name (not incl. NUL) */
#define SLEEPLOG_CLIENT_LIST_MAX_NUM 7
    /* Maximum client names to append to the log packet */


/*============================================================================
                     FUNCTION DECLARATIONS
============================================================================*/

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
);

/*===========================================================================
FUNCTION      sleep_log_new_voters

DESCRIPTION   Look at sleep.log.curr_not_okts_m and log all !okts voters if
              any votes have changed.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  !okts voters will get the first 4 characters of their 
              registered name logged to smem or a local buffer depending on 
              the target.
===========================================================================*/

void sleep_log_new_voters ( void );

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

void sleep_diag_task_stats( void );


#ifdef FEATURE_SMEM
/*===========================================================================

FUNCTION SLEEPLOG_SAVE_NAME

DESCRIPTION
  Writes the name of the const char[] passed in to shared memory.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleeplog_save_name 
( 
  const char      *name     
   /* Name of the client registering to vote */
);
#endif /* FEATURE_SMEM */


/*===========================================================================
FUNCTION      sleeplog_init

DESCRIPTION   Initialize the local sleeplog buffer.

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  sleeplog_initialized will be set
===========================================================================*/

void sleeplog_init ( void );

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
);

#endif /* SLEEPLOG_H */

