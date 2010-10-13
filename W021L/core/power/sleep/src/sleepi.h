#ifndef SLEEPI_H
#define SLEEPI_H
/*===========================================================================

              S L E E P    T A S K   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Sleep Task.
  This module is used in the portable only.

                             Copyright (c) 2006 - 2009 
                    by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleepi.h#3 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/09   ajf     Break down the sleep NV item into a bitfield and use the
                   structure to more explicitly control sleep types.
01/06/09   taw     Remove APPS_PWR_ON_MS, it's use is now in DEM.
09/14/08   cab     Remove clki.h include
06/27/08   clm     1H08 sleep re-work.
03/20/08   jfl     Added support for VDD min restrictions
10/04/07   taw     Move the shared memory pointer to the log structure, and 
                   add a boolean for withholding sleep at boot-up.
08/14/07   ebb     Added static_sleep_names_log to point to smem allocated
                   space.
06/27/07   cab     Updated MAX_NUM_SLEEP_CLIENTS, reduced to 64
05/22/07   cab     Added swfi_restriction to client structure.
05/01/07   cab     Changed qword to uint64
04/26/07   cab     Updated logging to include new sleep clients
04/16/07   cab     Mainlined CLK_REGIME_UP_SLEEP__MCLK_HCLK
04/12/07   cab     Removed LOG_SLEEP_STATS_C conditional
08/08/06   cab     Added clk.h to resolve CLK_REGIME_UP_SLEEP__MCLK_HCLK
06/14/06   jwob    Updated sleep_log_type to use sleeplog_task_stats_v2
06/06/06   ddh     Created File

===========================================================================*/
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "rex.h"
#include "clkregim.h"
#include "sleeplog.h"
#include "clk.h"
#include "sleep.h"

/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/* Number of 4.92 MHz TCXO/4 clocks in one millisecond,
   assuming that the clock error is zero. */
#define NOMINAL_CLOCKS_PER_MS    4920L

/* maximum number of sleep voting clients */
#define MAX_NUM_SLEEP_CLIENTS    64

/* Number of seconds to hold off sleep after inital boot */
#define SLEEP_BOOT_TIME_HOLDOFF	 20

#ifdef DUAL_PROC_APPS_SLEEP
  #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
  #else /* !FEATURE_APPS_POWER_COLLAPSE */
    #define SLEEP_CHECK_APPS_PC_SCLK     0
  #endif /* FEATURE_APPS_POWER_COLLAPSE */

  /* The minimum amount of time before the next timer expires to 
     allow for halting the apps processor and allowing the modem
     to perform TCXO shutdown */
  #define SLEEP_APPS_HALT_TIME_MS    3 

  /* Amount of time, converted to sleep clocks */
  #define SLEEP_CHECK_APPS_HALT_SCLK           \
          TIMETICK_SCLK_FROM_MS( SLEEP_APPS_HALT_TIME_MS )

#endif /* DUAL_PROC_APPS_SLEEP */


/* The idea here is to morph the handle into something that is not
   common, so that when clients use the handle to vote, it is
   "guaranteed" to be a valid handle returned by register.
   0x57EE9 is supposed to resemble the word SLEEP */
#define SLEEP_HANDLE_TO_INDEX( handle ) ( (handle) ^ 0x57EE9000 )
#define SLEEP_INDEX_TO_HANDLE( index  ) ( (index)  ^ 0x57EE9000 )

/* Bits of the NV / shared memory item that can disable sleep modes. */
#define SLEEP_DISABLE_PWRC_LEGACY  0x01
#define SLEEP_DISABLE_MODEM_HALT   0x02

/* Structure for sleep restrictions so we can store it in an organized way */
/* 2H08 and later will have more restrictions */
typedef struct
{
  boolean disable_pwrc_legacy;
  boolean disable_modem_halt;
} sleep_restriction_type;

extern volatile sleep_restriction_type gSleepRestrictions;

/* Used to check whether or not TCXO has been turned back on after sleep. */
extern boolean gbSleepTcxoOn;

/*===========================================================================

                           TYPE DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Logging
--------------------------------------------------------------------------*/

typedef struct
{

  /* Sleep task statistics */
  sleeplog_task_stats_v2          task_stats;

  /* Old voters voting to keep TCXO on when last packet was generated */
  rex_sigs_type                   not_okts;

  /* Captures the number of new sleep voters when sleep decision is made */
  uint8                           num_clients;

  /* Captures the new sleep voters when sleep decision is made */
  uint64                          curr_not_okts_m;

  /* New voters voting to keep TCXO on when last packet was generated */
  uint64                          prev_not_okts_m;

  /* Names of the sleep voters in shared memory */  
  char                           *smem_names;

} sleep_log_type;

/*--------------------------------------------------------------------------
  Sleep client data structure type (clients requiring an okts vote)
--------------------------------------------------------------------------*/

typedef struct
{

  /* client vote for okts */
  boolean                     okts;

  /* client restriction on mclk (cpu halt) */
  clkrgm_mclk_sleep_type      mclk_restriction;

  /* client restriction on hclk (bus clock) */
  clkrgm_hclk_sleep_type      hclk_restriction;

  /* client vote for SWFI restriction level */
  sleep_swfi_restrict_type    swfi_restriction;

  /* client vote for vdd min restriction level */
  sleep_vdd_min_restrict_type vdd_min_restriction;

} sleep_client_info_type;

/*--------------------------------------------------------------------------
  Sleep local data structure type
--------------------------------------------------------------------------*/

typedef struct
{

  /* Data for logging */
  sleep_log_type                  log;

  /* Timer for reporting to the DOG task */
  clk_cb_type                     dog_report_timer;

  /* Used to show that the sleep task has been told to start and also 
       to indicate when it has been told to stop.  When stopped, it will
       forbid TCXO shutdown and clock shutdown */
  boolean                         task_running;

  /* Client OKTS votes and restrictions */
  sleep_client_info_type          client_info[MAX_NUM_SLEEP_CLIENTS];

  /* Client name */
  const char                     *client_name[MAX_NUM_SLEEP_CLIENTS];

  /* Not OKTS mask for all clients */
  uint64                          curr_not_okts_mask;

  /* Number of sleep clients */
  uint8                           num_clients;

  /* Used to hold off sleep, such as when we're booting up. */
  boolean                         allow_sleep;

  /* Shows which voters are against VDD minimization */
  uint64                          vdd_min_restriction_mask;

  /* Shows which voters have mclk restrictions */
  uint64                          mclk_restriction_mask;

  /* Shows which voters have hclk restrictions */  
  uint64                          hclk_restriction_mask;

  /* Shows which voters have swfi restrictions */
  uint64                          swfi_restriction_mask;

} sleep_info;


typedef uint32 sleep_type;
#define SLEEP_TYPE_NONE          0      
#define SLEEP_TYPE_MICRO         1
#define SLEEP_TYPE_TCXO_SHUTDOWN 2
#define SLEEP_TYPE_PC            3
#define SLEEP_TYPE_APPS_SLEEP    4

/*--------------------------------------------------------------------------
  Sleep global data
--------------------------------------------------------------------------*/
extern sleep_info                 gSleepInfo;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SLEEP_ACCUMULATE_ACTIVE_TIMESTATS

DESCRIPTION
  This procedure accumulates the active time stats and returns the current
  sclk time value.

DEPENDENCIES
  Called only from sleep task

RETURN VALUE
  Current Sclk Time

SIDE EFFECTS
  None

===========================================================================*/

uint32 sleep_accumulate_active_timestats( void );

/*===========================================================================

FUNCTION SLEEP_ACCUMULATE_SLEEP_TIMESTATS

DESCRIPTION
  This procedure accumulates the sleep time stats.

DEPENDENCIES
  Called only from sleep task

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleep_accumulate_sleep_timestats
( 
  /* Sclk time prior to peforming sleep */
  uint32     uPreSleepSclk,
  /* Type of sleep performed */
  sleep_type sleepType
);

#endif /* SLEEPI_H */

