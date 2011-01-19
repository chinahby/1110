/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        S L E E P     T A S K

GENERAL DESCRIPTION
  This file contains general sleep functions that are used by other
  tasks and services to enable or disable the sleep functionality.

EXTERNALIZED FUNCTIONS
  sleep_register
    Used to dynamically make a client register for okts votes
  sleep_assert_okts
    Used by client to indicate it is OK to sleep
  sleep_negate_okts
    Used by client to indicate it is NOT OK to sleep

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

      Copyright (c) 1992-2008 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleep.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/22/08   clm     Comment clean up.
06/27/08   clm     1H08 sleep re-work.
03/20/08   jfl     Added support for VDD min restrictions
10/04/07   taw     Move logging names to shared memory into sleeplog.c/h;
                   added sleep_init() to set a timer to withhold sleep 
                   during boot.
08/14/07   ebb     Setup smem_alloc'd space for sleep voter names to assist 
                   in logging.
06/27/07   cab     Fixed potential task switch issues
05/22/07   cab     Added API sleep_set_swfi_restriction.
05/01/07   cab     Changed qword to uint64
04/23/07   cab     Fixed logging for new sleep voters
04/16/07   cab     Mainlined CLK_REGIME_UP_SLEEP__MCLK_HCLK
04/03/07   cab     Task lock critical section in sleep_register
08/04/06   cab     Add mclk and hclk restrictions to new sleep interface
06/06/06   ddh     Moved modem specific functions to sleepmod.c
05/10/06   cab     First implementation of "new" voting interface functions
04/28/06   cab     Removed compiler warning
04/26/06   ejv     Added T_QSC60X0 check around DOG autokick calls.
04/17/06   ejv     Added SLEEP_MC_INIT_OKTS_SIG for T_QSC60X0.
04/06/06   ddh     Cleaned up debug information for non-7xxx targets.
04/06/06   mpk     Re-synced with L4Linux 6550 branch. Changes in L4Linux
                   voting mechanism and L4Linux voting APIs.
04/06/06   ddh     Added #define for DEM_STATE_DEBUG for non-7500 builds
04/06/06   ddh     cleaned up debugging for 7500
03/29/06   dy      Added GSM/GPRS support for multi-mode build.
03/13/06   mpk     General L4 support cleanup. Merged L4 6550 with mainline.
01/06/06   ddh     Added support for 7500 TCXO shutdown.
12/05/05   wd      Added intlock/intfree that works with TCXO shutdown.
09/09/05   jrp     TCXO now turns off for L4 build during sleep.
08/31/05   ejv     Define FEATURE_L4_SRCH_CPUSLEEP if FEATURE_L4 is defined.
06/28/05   jyw     Added probes for sleep time line profiling 
06/22/05   ddh     Moved apps sleep and power collapse support 
05/26/05   mpk     Added task signaling under FEATURE_L4.
05/23/05   mpk     Added support for FEATURE_L4_PERF_OPTIM.
05/19/05   ajn     Removed unused DS OKTS signal.
05/02/05   ajn     Renamed USE_NEW_CLK_REGIME_UP_SLEEP.
04/27/05   hxw     Added FEATURE_MDM_REQUIRE_NO_ARM_HALT for 6800 1.0
04/22/05   ejv     Define USE_NEW_CLK_REGIME_UP_SLEEP for 6700/6800.
04/18/05   ejv     Allow MCLK switch on 6800, but not 7500.
04/02/05   ddh     Added definition of usb_needs_hclk
04/02/05   ddh     Added Apps and Modem support for 7500
03/31/05   ajn     Conditional support for new clk_regime_uP_sleep signature
03/29/05   ejv     Allow ARM clock halt on 6800.
03/14/05   ejv     Do not allow TCXO powerdown or CPU halt for 6800.
02/10/05   ajn     Removed unneeded srch.h include.
02/07/05   bgc     Added TCXO shutdown prevention when sleep task is stopped.

===========================================================================*/

/*=========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "sleep.h"
#include "sleepi.h"
#include "sleeplog.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  Sleep global data
--------------------------------------------------------------------------*/

/* Global sleep data, task data and sleep voting data */
sleep_info         gSleepInfo;

/* Used to indicate if TCXO must be kept on or not for the UART */
boolean            sleep_uart_clock_rgm = FALSE;

/* Used to prevent sleep for a certain amount of time during boot */
timer_type         gSleepPreventTimer;

/*-------------------------------------------------------------------------
  Static Function Declarations and Definitions
--------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION SLEEP_ATOMIC_BIT_SET

DESCRIPTION
  Set bit in 64 bit mask passed in at bit uIndex.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

static void sleep_atomic_bit_set
(
  uint64 *pu64BitMask, 
  uint32 uIndex
)
{
  uint64 ullTemp = 1;
  /* Set this client's bit in the 64 bit mask */
  ullTemp <<= uIndex;

  INTLOCK();
  TASKLOCK();

  *pu64BitMask |= ullTemp;

  TASKFREE();
  INTFREE();
} /* sleep_atomic_bit_set */

/*===========================================================================

FUNCTION SLEEP_ATOMIC_BIT_CLEAR

DESCRIPTION
  Clear bit in 64 bit mask passed in at bit uIndex.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

static void sleep_atomic_bit_clear
(
  uint64 *pu64BitMask, 
  uint32 uIndex
)
{
  uint64 ullTemp = 1;
  /* Clear this client's bit in the 64 bit mask */
  ullTemp <<= uIndex;

  INTLOCK();
  TASKLOCK();

  *pu64BitMask &= ~ullTemp;

  TASKFREE();
  INTFREE();
} /* sleep_atomic_bit_clear */

/*===========================================================================

FUNCTION SLEEP_INDEX_IS_VALID

DESCRIPTION
  Check to see if index provided is valid.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS

===========================================================================*/

static boolean sleep_index_is_valid
(
  uint32 uIndex
)
{
  return ((uIndex < gSleepInfo.num_clients)?TRUE:FALSE);
} /* sleep_index_is_valid */

/*-------------------------------------------------------------------------
  Externalized Function Definitions
--------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION SLEEP_REGISTER

DESCRIPTION
  Required at init time for any client that wants a unique vote for sleep.
  Pointer passed in must be unique, and the initial vote for sleep is
  indicated by the second parameter.

DEPENDENCIES
  None.

RETURN VALUE
  Sleep client handle, used for subsequent calls to assert and negate (always
  non-zero), or returns zero if an error occurs

SIDE EFFECTS
  After this function completes, a new client record in the internal sleep 
  array structure is reserved for the calling client.

===========================================================================*/

sleep_okts_handle sleep_register
( 
  const char *pszName, 
  boolean     bOkts
)
{
  /* loop index */
  uint32            uIndex;

  /* return handle */
  sleep_okts_handle handle = (sleep_okts_handle) 0;

  /* error flag */
  boolean           bError = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  INTLOCK();
  TASKLOCK();

  do
  {
    if ((pszName == NULL) || (gSleepInfo.num_clients >= MAX_NUM_SLEEP_CLIENTS))
    {
      /* If name is NULL or storage limit exceeded, print msg and return an
         invalid handle */
      MSG_ERROR("Registered with name=NULL or no space for further clients", 
                pszName, gSleepInfo.num_clients, 0);
      break;
    }

    /* If the name ptr is not unique, bail out */
    for (uIndex = 0; uIndex < gSleepInfo.num_clients; uIndex++)
    {
      if (pszName == gSleepInfo.client_name[uIndex])
      {
        ERR_FATAL("Sleep client already registered", 0, 0, 0);
        bError = TRUE;
        break;
      }
    }

    if (bError)
    {
      /* Exit do {...} while(FALSE) */
      break;
    }

    /* Fill out this client's sleep client record */
    gSleepInfo.client_name[gSleepInfo.num_clients] = pszName;
    gSleepInfo.client_info[gSleepInfo.num_clients].mclk_restriction = 
                                                               CLKRGM_MCLK_OFF;
    gSleepInfo.client_info[gSleepInfo.num_clients].hclk_restriction = 
                                                               CLKRGM_HCLK_OFF;
    gSleepInfo.client_info[gSleepInfo.num_clients].swfi_restriction = 
                                                       SLEEP_SWFI_RESTRICT_OFF;
    gSleepInfo.client_info[gSleepInfo.num_clients].okts = bOkts;

    /* Update the not okts mask */
    if ( !bOkts )
    {
      sleep_atomic_bit_set(&gSleepInfo.curr_not_okts_mask, gSleepInfo.num_clients);
    }

#ifdef FEATURE_SMEM
    sleeplog_save_name( pszName );
#endif /* FEATURE_SMEM */

    handle = SLEEP_INDEX_TO_HANDLE(gSleepInfo.num_clients++);
  } while (FALSE);

  TASKFREE();
  INTFREE();

  /* Return the sleep handle that the client will use in assert/negate */
  return handle;
} /* sleep_register */

/*===========================================================================

FUNCTION SLEEP_ASSERT_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is ok to sleep (tcxo off).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to TRUE.

===========================================================================*/

void sleep_assert_okts
(
  sleep_okts_handle handle
)
{
  /* convert incoming handle to the client index */
  uint32 uIndex = SLEEP_HANDLE_TO_INDEX(handle);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* detect if a bogus handle was supplied */
  if (sleep_index_is_valid(uIndex) == FALSE)
  {
    MSG_ERROR("Invalid handle passed to assert", 0, 0, 0);
    return;
  }

  /* set the client sleep vote to TRUE */
  gSleepInfo.client_info[uIndex].okts = TRUE;

  /* clear this client's bit in the 64 bit mask */
  sleep_atomic_bit_clear(&gSleepInfo.curr_not_okts_mask, uIndex);
} /* sleep_assert_okts */

/*===========================================================================

FUNCTION SLEEP_NEGATE_OKTS

DESCRIPTION
  Used by each sleep client to indicate that, as far as that client is
  concerned, it is not ok to sleep (tcxo on).  Client must pass in handle
  obtained from register call at init time.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal sleep vote is set to FALSE.

===========================================================================*/

void sleep_negate_okts
(
  sleep_okts_handle handle
)
{
  /* convert incoming handle to the client index */
  uint32 uIndex = SLEEP_HANDLE_TO_INDEX(handle);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* detect if a bogus handle was supplied */
  if (sleep_index_is_valid(uIndex) == FALSE)
  {
    MSG_ERROR("Invalid handle passed to negate", 0, 0, 0);
    return;
  }

  /* set the client sleep vote to FALSE */
  gSleepInfo.client_info[uIndex].okts = FALSE;

  sleep_atomic_bit_set(&gSleepInfo.curr_not_okts_mask, uIndex);
} /* sleep_negate_okts */

/*===========================================================================

FUNCTION SLEEP_SET_MCLK_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on mclk.  If a client requires
  mclk during sleep, the client can use this function to indicate that.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal mclk_restriction is set

===========================================================================*/

void sleep_set_mclk_restriction
(
  sleep_okts_handle       handle,
  clkrgm_mclk_sleep_type  mclk_restriction
)
{
  /* convert incoming handle to the client index */
  uint32 uIndex = SLEEP_HANDLE_TO_INDEX(handle);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* detect if a bogus handle was supplied */
  if (sleep_index_is_valid(uIndex) == FALSE)
  {
    MSG_ERROR("Invalid handle passed to set_mclk_restriction", 0, 0, 0);
    return;
  }

  /* set the client mclk restriction */
  gSleepInfo.client_info[uIndex].mclk_restriction = mclk_restriction;

  if (mclk_restriction == CLKRGM_MCLK_OFF)
  {
    sleep_atomic_bit_clear(&gSleepInfo.mclk_restriction_mask, uIndex);
  }
  else
  {
    sleep_atomic_bit_set(&gSleepInfo.mclk_restriction_mask, uIndex);
  }
} /* sleep_set_mclk_restriction */


/*===========================================================================

FUNCTION SLEEP_SET_HCLK_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on hclk.  If a client requires
  hclk during sleep, the client can use this function to indicate the speed
  that is required.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal hclk_restriction is set

===========================================================================*/

void sleep_set_hclk_restriction
(
  sleep_okts_handle       handle,
  clkrgm_hclk_sleep_type  hclk_restriction
)
{
  /* convert incoming handle to the client index */
  uint32 uIndex = SLEEP_HANDLE_TO_INDEX(handle);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* detect if a bogus handle was supplied */
  if (sleep_index_is_valid(uIndex) == FALSE)
  {
    MSG_ERROR("Invalid handle passed to set_hclk_restriction", 0, 0, 0);
    return;
  }
  
  /* set the client hclk restriction */
  gSleepInfo.client_info[uIndex].hclk_restriction = hclk_restriction;

  if (hclk_restriction == CLKRGM_HCLK_OFF)
  {
    sleep_atomic_bit_clear(&gSleepInfo.hclk_restriction_mask, uIndex);
  }
  else
  {
    sleep_atomic_bit_set(&gSleepInfo.hclk_restriction_mask, uIndex);
  }
} /* sleep_set_hclk_restriction */

/*===========================================================================

FUNCTION SLEEP_SET_SWFI_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on swfi.  If a client requires 
  that SWFI (microprocessor sleep) not happen, then it can use this function
  to indicate that.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal swfi_restriction is set

===========================================================================*/

void sleep_set_swfi_restriction
(
  sleep_okts_handle          handle,
  sleep_swfi_restrict_type   swfi_restriction
)
{
  /* convert incoming handle to the client index */
  uint32 uIndex = SLEEP_HANDLE_TO_INDEX(handle);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* detect if a bogus handle was supplied */
  if (sleep_index_is_valid(uIndex) == FALSE)
  {
    MSG_ERROR("Invalid handle passed to sleep_set_swfi_restriction", 0, 0, 0);
    return;
  }

  /* set the client swfi restriction */
  gSleepInfo.client_info[uIndex].swfi_restriction = swfi_restriction;

  if (swfi_restriction == SLEEP_SWFI_RESTRICT_OFF)
  {
    sleep_atomic_bit_clear(&gSleepInfo.swfi_restriction_mask, uIndex);
  }
  else
  {
    sleep_atomic_bit_set(&gSleepInfo.swfi_restriction_mask, uIndex);
  }
} /* sleep_set_swfi_restriction */

/*===========================================================================

FUNCTION SLEEP_SET_VDD_MIN_RESTRICTION

DESCRIPTION
  Allow sleep client to place a restriction on vdd minimization.
  This would only occur if tcxo shutdown (sleep) occurs, so is only relevant
  if the clients sleep vote is okts.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Client's internal vdd_min_restriction is set

===========================================================================*/

void sleep_set_vdd_min_restriction
(
  sleep_okts_handle             handle,
  sleep_vdd_min_restrict_type   vdd_min_restriction
)
{
  /* convert incoming handle to the client index */
  uint32 uIndex = SLEEP_HANDLE_TO_INDEX(handle);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* detect if a bogus handle was supplied */
  if (sleep_index_is_valid(uIndex) == FALSE)
  {
    MSG_ERROR("Invalid handle passed to sleep_set_vdd_min_restriction",
               0, 0, 0);
    return;
  }

  gSleepInfo.client_info[uIndex].vdd_min_restriction = vdd_min_restriction;

  if (vdd_min_restriction == VDD_MIN_ALLOWED)
  {
    sleep_atomic_bit_clear(&gSleepInfo.vdd_min_restriction_mask, uIndex);
  }
  else
  {
    sleep_atomic_bit_set(&gSleepInfo.vdd_min_restriction_mask, uIndex);
  }
} /* sleep_set_vdd_min_restriction */

/*===========================================================================

FUNCTION SLEEP_TIMER_CB

DESCRIPTION
  Callback for the timer that withholds sleep during bootup.  Sets the flag
  to allow sleep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sleep will now start occurring, as long as it is enabled, all votes are
  TRUE, and there is time.

===========================================================================*/

static void sleep_prevent_timer_cb
(
  timer_cb_data_type timer_id 
)
{
  /* The system is done booting, so we can allow more than just SWFI now */
  gSleepInfo.allow_sleep = TRUE;
} /* sleep_prevent_timer_cb */

/*===========================================================================

FUNCTION SLEEP_INIT

DESCRIPTION
  Initialize the sleep subsystem.

DEPENDENCIES
  timer_init() must already be called, and the timer task must be started.

RETURN VALUE
  None

SIDE EFFECTS
  A timer is set which causes sleep to be withheld until it expires.

===========================================================================*/

void sleep_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the sleep logging portion. */
  sleeplog_init();

  /* Set a timer in order to witthold sleep at boot-up for a certain period
   * of time, to allow everything to initialize properly. */
  timer_def( &( gSleepPreventTimer ), NULL, NULL, NULL, 
             &sleep_prevent_timer_cb, NULL );
  timer_set( &( gSleepPreventTimer ), SLEEP_BOOT_TIME_HOLDOFF, 0, T_SEC );

  /* This variable will be FALSE until the timer expires. */
  gSleepInfo.allow_sleep = FALSE;
} /* sleep_init */

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

uint32 sleep_accumulate_active_timestats( void )
{
  uint32 uCurrentSclk = timetick_get_safe();

  /* Accumulate "active" time into stats packet */
  gSleepInfo.log.task_stats.t_active
         += uCurrentSclk - gSleepInfo.log.task_stats.timestamp;

  return uCurrentSclk;
} /* sleep_accumulate_active_timestats */

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
)
{
  /* Get the timestamp at wakeup */
  gSleepInfo.log.task_stats.timestamp = timetick_get_safe();

  switch (sleepType)
  {
    case SLEEP_TYPE_PC:
      gSleepInfo.log.task_stats.t_pc += 
                           gSleepInfo.log.task_stats.timestamp - uPreSleepSclk;
      gSleepInfo.log.task_stats.n_pc++;
      break;

    case SLEEP_TYPE_TCXO_SHUTDOWN:
    case SLEEP_TYPE_APPS_SLEEP:
      gSleepInfo.log.task_stats.t_tcxo_off += 
                           gSleepInfo.log.task_stats.timestamp - uPreSleepSclk;
      gSleepInfo.log.task_stats.n_tcxo_off++;
      break;

    case SLEEP_TYPE_MICRO:
      gSleepInfo.log.task_stats.t_halt += 
                           gSleepInfo.log.task_stats.timestamp - uPreSleepSclk;
      gSleepInfo.log.task_stats.n_halt++;
      break;

    default:
      break;
  }
} /* sleep_accumulate_sleep_timestats */

