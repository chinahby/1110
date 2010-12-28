/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R

GENERAL DESCRIPTION
   This file contains the hardware drivers for searcher 4.

EXTERNALIZED FUNCTIONS
   srch4_get_tasks              Returns a ptr requested queue's tasks
   srch4_queue_available        Checks if a queue is available
   srch4_tasks_left             Checks if a queue is active and returns
                                 number of tasks left to be programmed

   srch4_init                   Set up necessary parameters for srch4

   srch4_flush_s_ram            Sends a command to flush the sample RAM

   srch4_add_task               Adds task to a specified queue (HPQ/LPQ)
   srch4_start_search           Start a specified (HPQ/LPQ)
   srch4_abort_queue            Aborts a specified queue (HPQ/LPQ)
   srch4_halt                   Stops all searches and disables all callbacks

   srch4_dump_isr               Process search interrupt

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2002 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/src/srch4drv.c#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/08   sst     Update interpolate peak alg for better results
09/08/08   adw     Added SRCH4_USE_LPQ, SRCH4_AFLT_ENABLED & SRCH4_HDR_ENABLED.
07/28/08   aps     T_MSM8650B bringup changes
06/20/08   mca     Merge from //depot
05/27/08   mca     Modified srch4 differentiate between normal 1x and offtl
                   searching
04/24/08   ljl     Added carrier info to srch4 log
                   Obsolete AFLT log pkt
12/14/07   sst     Correct featurization around Sched SR Flush
11/09/07   aps     Disable Srch fing rotator for 7800 as there is a rotator in
                    the RX front-end. This is the same change as T_QSC60X5
07/22/07   ljl     Supported multi-carriers.
07/20/07   rkc     srch4_start_search() uses freq_offset if technology is HDR
04/18/07   tjc     Lint fixes
04/12/07   sst     Changes to support new VCTCXO image for QSC60x5
03/07/07   aps     Moved INTLOCK() up in srch4_add_task() to handle any race
                    condition that may occur.
12/19/06   sst     Merge the following change from 11.02.02B branch
12/01/06   sst     Disable "freq offset" in srch4 for FEATURE_XO targets
10/23/06   jyw/awj Added interface to allow HDR fetch interpolated srch results.
09/25/06   awj     Moved RxD energy translation to trans layer
09/08/06   awj     Fixed RxD energy translation
08/21/06   aps/awj removed references to trunc parameter
06/08/06   awj     Remove encapsulation violation for pilot set info
04/26/06   tjc     Check for overflow condition in peak energy interpolation
01/16/06   awj     Lint cleanup
11/28/05   sst     For logging HDR/AFLT call RF RxAGC funcs
11/14/05   awj     Lint compliance
09/23/05   ejv     Deprecate rex timer types/calls.
09/13/05   sst     Lint fixes
08/23/05   ejv     Signal MDSP after each flush command.
08/11/05   sst     Make LOG_SRCH4_1X_C and SRCH_LOG_TNG_1X_SEARCHER_DUMP_C
                    log packets mutually exclusive
07/19/05   grl     Added flushing the sample ram of the requested chain.
06/13/05   asj     Modified mask for peak position calculation to retain lsb.
05/31/05   rng     Merged from SHDR Sandbox.
05/20/05   ejv     Remove srch_pn.h.
05/13/05   ejv     Cast SRCH_SUB_ID_x to uint8.
03/22/05   sst     Removed references to srch4drv_t.h, changed reference to
                    srch4_srch_type to uint32 in logging functions
03/10/05   ddh     Changed loop over queue to start at SRCH4_MIN_Q
02/08/05   bt      Put error checking into the energy interpolation.
02/04/05   bt      Add scheduled sample ram flush.
01/18/05   sst     Interpolate peak energy from shoulders
11/17/04   sst     Update function header block of srch4_halt
                   Synchronous abort all active searches in srch4_halt
10/29/04   sq      Do not shift search results for HDR diversity search
10/07/04   kwo     Renamed gen log packets to SRCH_LOG_TNG...
09/08/04   jcm     Rename exported genlog types to include srch_
09/03/04   jcm     Added generalized srch4 subpacket
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
07/28/04   sst     Added check and correction for min window size
07/13/04   sst     Added SRCH4_NO_QUEUE = SRCH4_MAX_Q in srch4_queue_type
                   Changed SRCH4_MAX_TYPE to SRCH4_MAX_TECH in srch4_srch_type
                   Added SRCH4_NO_TECH = SRCH4_MAX_TECH in srch4_srch_type
07/13/04   ejv     Add RXDIV_SHIFT to scale RxDiv results.
07/12/04   sst     Added "active" check to srch4_queue_available()
                   Moved technology check around in srch4_abort_queue()
07/08/04   sst     Added an ERR message to indicate when no queue
                    (SRCH4_MAX_Q) was passed (indicates un-registered tech)
06/21/04   sst     Corrected naming of srch_mdsp_check_cmd_ functions
06/15/04   bt      Carry over cb_data when losing a dump.
05/26/04   sst     Fixed compile error in last checkin
05/26/04   sst     Update freq offset field in log packet to adjust for its
                    wrong size. Also add range checking to it.
05/26/04   sq      Fixed add_task's check for valid BURST_LEN_MAX
                   Removed extra whitespaces
05/25/04   sst     Added parameter to abort_cb to be able to pass number
                    of partial results returned
                   Updated handling of lost dumps, so lost dumb cb is not
                    assigned to the abort dump cb on lost dump abort
05/04/04   sst/va  Merge from srch4 3Q dev branch (dev-changes)
04/26/04   sst     dev-Changed SRCH4DRV_RET_FREE to be queue specific
04/20/04   sst     Moved tech check in abort to top of if statements
04/14/04   sst     dev-Added partial results param to srch4_abort_queue()
                   dev-Added partial results processing
                   dev-Added ptr to task struct inside queue struct
                   dev-Moved all no-err status dump messages out to
                     srch4_status_dump()
04/12/04   sst     dev-Updated abort processing
04/08/04   sst     dev-Created SRCH4_HAS_HPQ1, SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
                   dev-Created SRCH4_DEBUG, SRCH4_DEBUG_MSG()
03/25/04   sst     dev-Added string srch4_prio_str
                   dev-Moved calling of dump_cb until after all flags are cleared
03/11/04   sst     dev-Changed logging of priority field
02/25/04   sst     More lint corrections
03/04/04   sst     dev-Added function srch4_halt() to "virtually stop" srch4
03/02/04   sst     dev-Sample ram update
02/19/04   sst     Lint corrections
                   Changed srch4_tasks_left() to return int32
02/19/04   sst     Moved calling of rex_set_timer() in start and abort to
                    prevent a task switch until after all flags are set
02/02/04   sst     Added synchronous field to srch4_abort_queue() to stall
                    until the abort has completed
01/28/04   ejv     Mainline T_MSM6500.
01/20/04   sst     Removed log_free() calls in srch4_status_dump()
01/07/03   bt      Add missing INTFREE_SAV in srch4_tasks_left.
12/31/03   bt      Allocate log packet close to committing it and fix
                    truncation log param for MSM6500.
12/15/03   sst     Added a technology parameter to srch4_tasks_left()
12/03/03   ajn     Abort: Check if SRCH4 is active before check if wrong tech
11/05/03   bt      Check whether different technologies try to abort
                    each other and remove srch4_abort.
11/04/03   bt      Moved SRCH4/MDSP interface into srch4trans.
09/22/03   sst     Reworked coh_trunc field of the log packet
09/21/03   bt      Add MDSP download functionality.
09/09/03   sst     Fixed handling of invalid window sizes
07/31/03   sst     Fixed HPQ->LPQ typo
07/30/03   ejv     Removed sample server references.
07/29/03   abh     Added routine for changing Sample Server back to 1X mode
                    after tuneback from GPS. Also added some debug capability
                    for sample ram.
07/14/03   sq      Lowered message level for "upexpected srch4 dump".
07/17/03   sst     Updated srch4 log pkt, added rx_agc0/1, removed +1's from
                    num_peaks and num_tasks, now using multiple log codes
                    based on technology
07/14/03   ejv     Remove unnecessary extern.
07/01/03   sst     Added LOG_SRCH4_C log packet and processing
                   Added freq_offset, QPCH, and logging info to queue struct
06/27/03   ejv     Fixed HPQ->LPQ typo.  Update "7 Cx8" comment.
06/23/03   va      Interpolation algorithm fix
05/23/03   sst     Initialized var to remove warning
04/25/03   sst     Corrected catching lost dumps and using the callback func
04/14/03   sst     Added catch for minimum and maximum window sizes
03/24/03   sst     Modified peak position calcs to return pos in Cx16
02/18/03   va      Fixed LPQ support
01/13/03   sst     Added separate abort lost timer, moved srch4_trig_dmp_err,
                    added return values for most functions, chafed handling
                    of lost dumps
12/20/02   sst     Fixed INTLOCK error in srch4_tasks_left()
12/05/02   sst     Code Clean up
11/14/02   sst     Fixed dump logic error, Using srch4 return enum values
                   Removed unused timers
11/11/02   sst     Removed srch4_remove_last_task, deemed unneeded
11/11/02   sst     Code cleanup, Added srch4_remove_last_task
11/07/02   sst     Added support for Jaguar, moved coh_tr here
                   Code cleanup
11/05/02   sst     Merge changes, Combined HPQ and LPQ funcs to reduce
                    code duplication
11/04/02   sst     Moved srch4 win center functionality to srch4trans
10/30/02   sst     Added support for srch4's own window center var
10/30/02   sst     Implemented lost dumps and their callbacks (untested)
10/29/02   sst     Removed temporary functions
                   Separated file, created srch4trans.c .h
                   Made function calls more generic to queues
10/18/02   sst     Temporary functions to "ease" transition to tasks
10/18/02   sst     Changed HPQ to use generic task structure
                   Added support for Transmit Diversity in 1x searching
                   Added translation functions (skeletons) for 1x, HDR, AFLT
10/08/02   sst     Added support for callback functions upon dump/lost dump
                   Fixed implementation of searcher 4 aborts
10/04/02   sst     Added support for 4 & 6 searcher peaks
10/01/02   sst     Added $Header line
10/01/02   sst     Moved ERR's and MSG's out of INTLOCK context
09/26/02   sst     Removal of debug functions.
                   MSM6100 implementation, testing, and code review changes
08/02/02   gs      Initial merge for MSM6100.
07/01/02   sst     Added support for LPQ
06/07/02   bt      initial implementation

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"
#include "assert.h"
#ifdef FEATURE_IS2000_REL_A_TD
#include "cai.h"
#endif
#include "comdef.h"
#include "err.h"
#include "log.h"
#include "mdspext.h"
#include "msg.h"
#include "msgcfg.h"
#include "msm_drv.h"
#include "msm_help.h"
#include "rex.h"
#include "rfcom.h"

#include "srch_diag.h"
#include "srch_genlog.h"
#include "srch_hw.h"
#include "srchi.h"
#include "srch_sect.h"

#include "srch4drv.h"
#include "srch4drv_t.h"
#include "srch4drvi.h"
#include "srch4trans.h"
#include "srch4trans_t.h"

#ifdef SRCH4_DEBUG
#error code not present
#endif
#include "timer.h"


/*===========================================================================
             static DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants & Macros
-------------------------------------------------------------------------*/

/* strings for F3 messages */

/* technology string, see srch4_srch_type */
const char srch4_tech_str[ ( (uint16) SRCH4_NUM_TECH + 1 ) ][ 6 ] =
{
  "1X",                                /* SRCH4_1X             */
  #ifdef SRCH4_HDR_ENABLED
  "HDR_S",                             /* SRCH4_HDR_SHOULDERS  */
  "NA",                                /* not defined          */
  "HDR_A",                             /* SRCH4_HDR_ACCUM      */
  #endif /* SRCH4_HDR_ENABLED */
  #ifdef SRCH4_AFLT_ENABLED
  "AFLT",                              /* SRCH4_AFLT           */
  #endif /* SRCH4_AFLT_ENABLED */
  "None"                               /* SRCH4_NO_TECH        */
};

/* queue name string, see srch4_queue_type */
const char srch4_queue_str[ ( (uint16) SRCH4_MAX_Q + 1 ) ][ 5 ] =
{
  "HPQ0",                              /* SRCH4_HPQ0           */
  #ifdef SRCH4_USE_LPQ
  "LPQ",                               /* SRCH4_LPQ            */
  #endif /* SRCH4_USE_LPQ */
  #ifdef SRCH4_HAS_HPQ1
  "HPQ1",                              /* SRCH4_HPQ1           */
  #endif /* SRCH4_HAS_HPQ1 */
  "None"                               /* SRCH4_MAX_Q          */
};

const char srch4_prio_str[ (uint16) SRCH4_PRIO_MAX ][ 10 ] =
{
  "MIN",                               /* min priority                   */
  "AFLT",                              /* AFLT                           */
  "ACQ",                               /* HDR Acq                        */
  "ACQ",                               /* 1x Acq                         */
  "TC-NS",                             /* HDR Traffic, HDR Non Slotted,
                                          1x Traffic, 1x Non Slotted     */
  "OFREQ-CFS",                         /* HDR Traffic CFS, HDR Ofreq,
                                          1x Traffic CFS, 1x Ofreq       */
  "REACQ_BC",                          /* HDR Reacq Broadcast            */
  "REACQ_BC",                          /* 1x Reacq Broadcast             */
  "REACQ_PG",                          /* HDR Reacq Paging               */
  "REACQ_PG"                           /* 1x Reacq Paging                */
};

/* Shift value to use on results when RxDiv is used for searching */
#define RXDIV_SHIFT  1

/* Version number for Search4 subpacket */
#define LOG_VERSION_SRCH4_DUMP  1

/* size of the header block (minus task) */
#define SRCH4_SUB_LOG_HEADER_SIZ  (FPOS(srch_genlog_srch4_sub_v1,tasks))

/* size of the parameters (minus the results) */
#define SRCH4_SUB_LOG_PARAM_SIZ  (FPOS(srch_genlog_srch4_task_type,results))

/* size of the results */
#define SRCH4_SUB_LOG_RESULTS_SIZ  (sizeof(srch_genlog_srch4_result_type))

#define SRCH4_SUB_LOG_SIZ(tasks,peaks) (SRCH4_SUB_LOG_HEADER_SIZ +        \
                             ((tasks) * (SRCH4_SUB_LOG_PARAM_SIZ +        \
                              ((peaks) * SRCH4_SUB_LOG_RESULTS_SIZ))))

/* checks the interpolated peak energy for saturation */
#define SRCH4_PEAK_ENG_SAT(peak_eng) \
  ((uint16)(((peak_eng) > 0xffff) ? 0xffff : (peak_eng)))

/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/

/* searcher 4 task buffers */
static srch4_task_struct_type    srch4_hpq0_tasks[SRCH4_MAX_HPQ_TASKS];
#ifdef SRCH4_USE_LPQ
static srch4_task_struct_type    srch4_lpq_tasks[SRCH4_MAX_LPQ_TASKS];
#endif /* SRCH4_USE_LPQ */
#ifdef SRCH4_HAS_HPQ1
static srch4_task_struct_type    srch4_hpq1_tasks[SRCH4_MAX_HPQ_TASKS];
#endif /* SRCH4_HAS_HPQ1 */

/* Rex lost dump timer for searcher queues */
static timer_type            srch4_hpq0_timer;
#ifdef SRCH4_USE_LPQ
static timer_type            srch4_lpq_timer;
#endif /* SRCH4_USE_LPQ */
#ifdef SRCH4_HAS_HPQ1
static timer_type            srch4_hpq1_timer;
#endif /* SRCH4_HAS_HPQ1 */

/* for searcher4 logging */
static LOG_SRCH4_C_type          srch4_hpq0_log_pkt;
static srch_genlog_srch4_sub_v1  srch4_hpq0_sub_log_pkt;
#ifdef SRCH4_USE_LPQ
static LOG_SRCH4_C_type          srch4_lpq_log_pkt;
static srch_genlog_srch4_sub_v1  srch4_lpq_sub_log_pkt;
#endif /* SRCH4_USE_LPQ */
#ifdef SRCH4_HAS_HPQ1
static LOG_SRCH4_C_type          srch4_hpq1_log_pkt;
static srch_genlog_srch4_sub_v1  srch4_hpq1_sub_log_pkt;
#endif /* SRCH4_HAS_HPQ1 */

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* structure for searcher 4 queue information */
typedef struct
{
  srch4_srch_type   srch_type;    /* type of search  (set at start search) */
  int16      freq_offset;         /* freq offset ( 4.6875 Hz/LSB )         */
  uint16     priority;            /* search priority (32767=high, 1=low)
                                     (logging: 4 bit resolution, 0-16)     */
  boolean    qpch;                /* qpch bit, as yet undefined            */
  boolean    active;              /* flag - srch4 is active                */
  boolean    aborted;             /* flag - current search was aborted     */
  boolean    lost;                /* flag - current search was lost        */
  boolean    partial_res;         /* flag - partial results are wanted on
                                     current aborted search                */
  uint8      num_tasks;           /* # of tasks currently programmed       */
  uint8      max_num_tasks;       /* maximum number of tasks available
                                     in the queue                          */
  uint8      last_srch_res_cnt;   /* number of srch results returned from
                                     last search                           */
  uint32     task_cnt;            /* total number of tasks                 */
  uint32     srch_cnt;            /* total number of srches (whole queue)  */
  uint32     abort_cnt;           /* total number of aborted searches      */
  void     (*dump_cb_func)(void *);   /* callback function on status dump  */
  void     (*lost_cb_func)(void *);   /* callback function on lost dump    */
  void     (*abort_cb_func)(uint32, void *);
                                      /* callback function on aborted dump */
  void     (*srch_cb_data);           /* data to be passed to callback     */
  uint16     lost_dump_time_estimate; /* estimated lost dump timeout       */
  timer_type   *lost_dump_timer;      /* pointer to lost dump timer        */
  LOG_SRCH4_C_type *log_ptr;          /* pointer to diag alloc'ed log pkt  */
  LOG_SRCH4_C_type *log_pkt;          /* searcher4's log pkt               */
  uint16     log_pkt_siz;             /* size of the log pkt               */
  srch_genlog_srch4_sub_v1 *sub_log_pkt; /* searcher4's general log pkt    */
  srch4_task_struct_type *tasks;      /* ptr to queue's task struct        */
} srch4_queue_var_type;

/* searcher 4 structure */
typedef struct
{
  srch4_queue_var_type hpq0;     /* var. struct for high priority queue 0 */
  #ifdef SRCH4_USE_LPQ
  srch4_queue_var_type lpq;      /* var. struct for low priority queue    */
  #endif /* SRCH4_USE_LPQ */
  #ifdef SRCH4_HAS_HPQ1
  srch4_queue_var_type hpq1;     /* var. struct for high priority queue 1 */
  #endif /* SRCH4_HAS_HPQ1 */
} srch4_var_type;

/* searcher 4 variable structure */
static srch4_var_type srch4 =
{
  /* HPQ 0 */
  {
    SRCH4_1X,   /* initially set to 1x                                */
    0,          /* initialize freq offset to 0                        */
    0,          /* initialize search priority to 0                    */
    FALSE,      /* initialize qpch bit to 0                           */
    FALSE,      /* initially not active                               */
    FALSE,      /* initially not aborted                              */
    FALSE,      /* initially not lost                                 */
    FALSE,      /* initially partial results are not wanted           */
    0,          /* # of tasks currently programmed                    */
    SRCH4_MAX_HPQ_TASKS,
                /* maximum number of tasks available                  */
    0,          /* num of tasks in last search "0"                    */
    0,          /* number of search results returned from last search */
    0,          /* total number of searches (whole queue)             */
    0,          /* total number of aborted searches                   */
    NULL,       /* callback_func pointer                              */
    NULL,       /* lost_dump_func pointer                             */
    NULL,       /* abort_dump_func pointer                            */
    NULL,       /* pointer callback function parameter                */
    0,          /* estimated time to set lost dump timeout            */
    &srch4_hpq0_timer,
                /* ptr to lost dump timer                             */
    NULL,       /* ptr to log pkt alloc'ed by diag                    */
    &srch4_hpq0_log_pkt,
                /* ptr to srch4 log pkt                               */
    0,          /* size of the log pkt                                */
    &srch4_hpq0_sub_log_pkt,
                /* ptr to srch4 log pkt                               */
    &(srch4_hpq0_tasks[0]),
                /* ptr to the tasks struct                            */
  },
  #ifdef SRCH4_USE_LPQ
  /* LPQ */
  {
    SRCH4_1X,   /* initially set to 1x                                */
    0,          /* initialize freq offset to 0                        */
    0,          /* initialize search priority to 0                    */
    FALSE,      /* initialize qpch bit to 0                           */
    FALSE,      /* initially not active                               */
    FALSE,      /* initially not aborted                              */
    FALSE,      /* initially not lost                                 */
    FALSE,      /* initially partial results are not wanted           */
    0,          /* # of tasks currently programmed                    */
    SRCH4_MAX_LPQ_TASKS,
                /* maximum number of tasks available                  */
    0,          /* num of tasks in last search "0"                    */
    0,          /* number of search results returned from last search */
    0,          /* total number of searches (whole queue)             */
    0,          /* total number of aborted searches                   */
    NULL,       /* callback_func pointer                              */
    NULL,       /* lost_dump_func pointer                             */
    NULL,       /* abort_dump_func pointer                            */
    NULL,       /* pointer callback function parameter                */
    0,          /* estimated time to set lost dump timeout            */
    &srch4_lpq_timer,
                /* ptr to lost dump timer                             */
    NULL,       /* ptr to log pkt alloc'ed by diag                    */
    &srch4_lpq_log_pkt,
                /* ptr to srch4 log pkt                               */
    0,          /* size of the log pkt                                */
    &srch4_lpq_sub_log_pkt,
                /* ptr to srch4 log pkt                               */
    &(srch4_lpq_tasks[0]),
                /* ptr to the tasks struct                            */
  },
  #endif /* SRCH4_USE_LPQ */
  #ifdef SRCH4_HAS_HPQ1
  /* HPQ 1 */
  {
    SRCH4_1X,   /* initially set to 1x                                */
    0,          /* initialize freq offset to 0                        */
    0,          /* initialize search priority to 0                    */
    FALSE,      /* initialize qpch bit to 0                           */
    FALSE,      /* initially not active                               */
    FALSE,      /* initially not aborted                              */
    FALSE,      /* initially not lost                                 */
    FALSE,      /* initially partial results are not wanted           */
    0,          /* # of tasks currently programmed                    */
    SRCH4_MAX_HPQ_TASKS,
                /* maximum number of tasks available                  */
    0,          /* num of tasks in last search "0"                    */
    0,          /* number of search results returned from last search */
    0,          /* total number of searches (whole queue)             */
    0,          /* total number of aborted searches                   */
    NULL,       /* callback_func pointer                              */
    NULL,       /* lost_dump_func pointer                             */
    NULL,       /* abort_dump_func pointer                            */
    NULL,       /* pointer callback function parameter                */
    0,          /* estimated time to set lost dump timeout            */
    &srch4_hpq1_timer,
                /* ptr to lost dump timer                             */
    NULL,       /* ptr to log pkt alloc'ed by diag                    */
    &srch4_hpq1_log_pkt,
                /* ptr to srch4 log pkt                               */
    0,          /* size of the log pkt                                */
    &srch4_hpq1_sub_log_pkt,
                /* ptr to srch4 log pkt                               */
    &(srch4_hpq1_tasks[0]),
                /* ptr to the tasks struct                            */
  },
  #endif /* SRCH4_HAS_HPQ1 */
};


#ifdef SRCH4_DEBUG
#error code not present
#endif /* SRCH4_DEBUG */


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/

/*===========================================================================

              S I M P L I F I C A T I O N   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_GET_QUEUE
DESCRIPTION    This function returns the requested queue.
DEPENDENCIES   Must check for a NULL return

RETURN VALUE   Ptr to requested queue structure
SIDE EFFECTS   None

===========================================================================*/
static srch4_queue_var_type *srch4_get_queue
(
  srch4_queue_type         queue_type         /* queue                 */
)
{
  srch4_queue_var_type *queue;

  /*-----------------------------------------------------------------------*/

  switch( queue_type )
  {
    case SRCH4_HPQ0:
    {
      queue = &srch4.hpq0;
      break;
    }
    #ifdef SRCH4_USE_LPQ
    case SRCH4_LPQ:
    {
      queue = &srch4.lpq;
      break;
    }
    #endif /* SRCH4_USE_LPQ */
    #ifdef SRCH4_HAS_HPQ1
    case SRCH4_HPQ1:
    {
      queue = &srch4.hpq1;
      break;
    }
    #endif /* SRCH4_HAS_HPQ1 */
    default:
    {
      /* error checking must be done outside this function */
      /* ERR(" Requesting invalid queue type (%d)", queue_type, 0, 0); */
      queue = NULL;
      break;
    }
  }

  return( queue );

} /* srch4_get_queue */


/*===========================================================================

FUNCTION       SRCH4_GET_TASKS
DESCRIPTION    This function returns the requested task structure.
               In the srch4 drivers, queue->tasks can also be used.
DEPENDENCIES   None

RETURN VALUE   Ptr to requested task structure
SIDE EFFECTS   None

===========================================================================*/
srch4_task_struct_type *srch4_get_tasks
(
  srch4_queue_type queue_type            /* queue to start                 */
)
{
  srch4_task_struct_type *tasks;

  /*-----------------------------------------------------------------------*/

  switch( queue_type )
  {
    case SRCH4_HPQ0:
    {
      tasks = srch4.hpq0.tasks;
      break;
    }
    #ifdef SRCH4_USE_LPQ
    case SRCH4_LPQ:
    {
      tasks = srch4.lpq.tasks;
      break;
    }
    #endif /* SRCH4_USE_LPQ */
    #ifdef SRCH4_HAS_HPQ1
    case SRCH4_HPQ1:
    {
      tasks = srch4.hpq1.tasks;
      break;
    }
    #endif /* SRCH4_HAS_HPQ1 */
    default:
    {
      ERR(" Requesting invalid queue type (%d)", queue_type, 0, 0);
      tasks = NULL;
      break;
    }
  }

  return( tasks );

} /* srch4_get_tasks */


/*===========================================================================

                   U T I L I T Y   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_QUEUE_AVAILABLE
DESCRIPTION    This function checks to see if a specified queue of
                searcher 4 is available/empty
DEPENDENCIES   None

RETURN VALUE   TRUE   if the queue is available
               FALSE  if the queue is active or being programmed
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_queue_available
(
  srch4_queue_type        queue_type      /* queue to check            */
)
{
  srch4_queue_var_type   *queue;          /* to accommodate queues     */
  uint32                  isave;          /* save interrupts           */

  /*-----------------------------------------------------------------------*/

  INTLOCK_SAV( isave );

  /* grab the queue */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    return( FALSE );
  }

  /* check if the queue is active or being programed, i.e num of tasks > 0 */
  if ( ( queue->num_tasks > 0 ) ||
       ( queue->active == TRUE ) )
  {
    INTFREE_SAV( isave);

    return( FALSE );
  }

  INTFREE_SAV( isave);

  /* this queue is free */
  return( TRUE );

} /* srch4_queue_available */


/*===========================================================================

FUNCTION       SRCH4_TASKS_LEFT
DESCRIPTION    This function checks if the specified searcher queue is active
                and returns how many searches can still be programmed
                It also checks to ensure that the requesting technology
                is using this queue
DEPENDENCIES   None

RETURN VALUE   Negative if searcher is active/tech mismatch/invalid/etc
               Zero if searcher is full
               Positive value is the number of tasks available to be
                programmed
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_tasks_left
(
  srch4_srch_type         task_srch_type, /* search type               */
  srch4_queue_type        queue_type      /* queue to check            */
)
{
  int32                   tasks_left = 0; /* return value              */
  srch4_queue_var_type   *queue;          /* to accommodate queues     */
  uint32                  isave;          /* save interrupts           */

  /*-----------------------------------------------------------------------*/

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    ERR( "Tech: %d tried to req tasks left when not registered",
         task_srch_type, 0, 0 );

    return( (int32) SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    return( (int32) SRCH4DRV_RET_INVALID_QUEUE );
  }

  INTLOCK_SAV( isave );

  /* Is this queue active, and assigned to the requesting technology? */
  if ( queue->active && ( queue->srch_type == task_srch_type ) )
  {
    if ( queue->lost || queue->aborted )
    {
      /* Somehow we lost a search before and it wasn't cleaned up! */
      INTFREE_SAV( isave);
      return( (int32) SRCH4DRV_RET_UNCLEAN );
    }
    else
    {
      /* searcher queue is active, can't do anything till dump */
      INTFREE_SAV( isave);
      return( (int32) SRCH4DRV_RET_ACTIVE );
    }
  }

  tasks_left = queue->max_num_tasks - (int32)queue->num_tasks;

  INTFREE_SAV( isave);

  if ( tasks_left < 0 )
  {
    /* something is terribly wrong! */
    ERR( "More than max # tasks proged(%d)(%d/%d)",
         queue, queue->num_tasks, queue->max_num_tasks );

    return( (int32) SRCH4DRV_RET_FATAL );
  }
  /* this queue is empty, shouldn't matter who owns the queue
     it is assumed here that the tech requesting this queue is assigned
     to it in srch4_tech_mappings in srch4trans.c */
  else if ( tasks_left == queue->max_num_tasks )
  {
    switch( queue_type )
    {
      case SRCH4_HPQ0:
      {
        return( (int32) SRCH4DRV_RET_HPQ0_FREE );
      }
      #ifdef SRCH4_USE_LPQ
      case SRCH4_LPQ:
      {
        return( (int32) SRCH4DRV_RET_LPQ_FREE );
      }
      #endif /* SRCH4_USE_LPQ */
      #ifdef SRCH4_HAS_HPQ1
      case SRCH4_HPQ1:
      {
        return( (int32) SRCH4DRV_RET_HPQ1_FREE );
      }
      #endif /* SRCH4_HAS_HPQ1 */
      default:
      {
        return( (int32) SRCH4DRV_RET_INVALID_QUEUE );
      }
    }
  }
  /* a different technology owns this queue */
  else if ( queue->srch_type != task_srch_type )
  {
    return( (int32) SRCH4DRV_RET_TECH_MISMATCH );
  }

  /* return how many tasks we can still program */
  return( tasks_left );

} /* srch4_tasks_left */


/*===========================================================================

FUNCTION       SRCH4_INTERPOLATE_PEAK
DESCRIPTION    This function interpolates the peak position and energy from
                the searcher data by using the shoulder energies
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void srch4_interpolate_peak
(
  int32  pos,                  /* peak position in Cx8 with Cx2 res       */
  uint32 eng,                  /* peak energy                             */
  uint32 ls_eng,               /* left shoulder peak energy               */
  uint32 rs_eng,               /* right shoulder peak energy              */
  int32  *pos_int,             /* interpolated peak position (Cx16)       */
  uint16 *eng_int              /* interpolated peak energy                */
)
{
  int32  num      = 0;         /* Numerator of interpolation algorithm    */
  int32  den      = 0;         /* Denominator of interpolation algorithm  */
  int32  offset   = 0;         /* calculated offsets                      */

/*-----------------------------------------------------------------------*/

  if ( ( pos_int == NULL ) || ( eng_int == NULL ) )
  {
    ERR( "srch4_interpolate_peak got NULL ptrs", 0, 0, 0 );

    return;
  }

  /* Change the pos to Cx16 for higher resolution*/
  pos *= 2;

  /* Set default position (in Cx16) and Energy */
  *pos_int = pos;
  *eng_int = SRCH4_PEAK_ENG_SAT(eng);

  /* prevent division by 0 */
  if( ( ls_eng + rs_eng ) != ( 2 * eng ) )
  {

    /* use quadratic interpolation to determine the Cx16 position
       and energy of the peak */

    num = 2 * ( ls_eng - rs_eng );
    den = rs_eng + ls_eng - ( 2 * eng );

    /* calculate the offset (Cx32) of the peak wrt the peak position */
    offset  = ( 4 * num ) / den; 

    /* Make bias proportional to offset */
    offset += offset * ( 8 - ABS( offset ) ) / 8; 

    /* round down to Cx16 */
    offset  = ( offset < 0 ) ? ( ( offset - 1 ) / 2 ) : ( offset + 1 ) / 2;

    /* apply the offset */
    *pos_int = pos + offset;

      /* compute energy */
    if( offset < 0 )
      {
      *eng_int = SRCH4_PEAK_ENG_SAT(
                   eng +
                   ( ( ( ( (int32)eng - (int32)rs_eng ) *
                         ( offset * offset ) ) /
                       ( 4 - offset ) ) / 16 ) );
      }
    else if ( offset > 0 )
      {
      *eng_int = SRCH4_PEAK_ENG_SAT(
                   eng +
                   ( ( ( ( (int32)eng - (int32)ls_eng ) *
                         ( offset * offset ) ) /
                       ( 4 + offset ) ) / 16 ) );
    }
    /* offset = 0, return the inital peak energy value */
  }

} /* srch4_interpolate_peak */


/*===========================================================================

FUNCTION       SRCH4_LOST_DUMP_CB_WRAPPER
DESCRIPTION    This function is a wrapper around the lost dump callback
                function so that we can preserve the use of rex_def_timer_ex
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   Calls the lost dump callback if one was provided
===========================================================================*/
static void srch4_lost_dump_cb_wrapper
(
  uint32                  queue_type    /* queue to get cb from */
)
{
  srch4_queue_var_type   *queue    = NULL; /* to accommodate queues      */
  uint32                  msg_flag = 0;    /* message flag               */

  /*-----------------------------------------------------------------------*/

  INTLOCK();

  /* check which queue this is a dump for, then assign queue for brevity */
  queue = srch4_get_queue( (srch4_queue_type) queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    msg_flag = 3;
  }
  else if ( ( queue->aborted == FALSE ) || ( queue->lost == FALSE ) )
  {
    queue->lost = TRUE;

    /* clear the timer */
    (void) timer_clr( queue->lost_dump_timer, T_NONE );

    if ( !(queue->active) )
    {
      /* no search was active, somehow the timer wasn't stopped */
      msg_flag = 1;
    }

    /* Since SRCH4 does not work like SRCH1/2 if we trigger a status dump
       ourselves we won't get any more meaningful data because we just read
       the DSP DMA memory. Instead we will let the task level deal with this
       problem, and for now just abort the search(es)
       Also set the lost dump callback function as the abort callback
       function to notify the task */
    (void) srch4_abort_queue( queue->srch_type,
                              (srch4_queue_type) queue_type,
                              NULL,
                              queue->srch_cb_data,
                              FALSE,
                              FALSE );
  }
  else /* searcher in a lost state (lost dump timeout after an abort) */
  {
    /* odds are we're really in trouble, or
       no search was ever actually sent to the dsp
       or no abort was actually sent */
    /* send ERR to inform everyone */
    msg_flag = 2;

    /* try to recover gracefully */
    queue->active      = FALSE;
    queue->aborted     = FALSE;
    queue->lost        = FALSE;
    queue->partial_res = FALSE;
    queue->num_tasks   = 0;
    queue->log_ptr     = NULL;
    queue->lost_dump_time_estimate = 0;
    if ( queue->lost_cb_func != NULL )
    {
      (*queue->lost_cb_func)( queue->srch_cb_data );
    }
  }

  INTFREE();

  /* send MSG's and ERR's outside of INTLOCKs */
  /*lint -save -e613 queue possibly null
                     queue == NULL only if msg_flag == 3 */
  switch( msg_flag )
  {
    case 1:
    {
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
                     "Srch4 lost, abort failed (%s on %s)",
                     SRCH4_TECH_STRING( queue->srch_type ),
                     srch4_queue_str[queue_type] );
      break;
    }
    case 2:
    {
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
                     "Srch4 lost, srch not in progress (%s on %s)",
                     SRCH4_TECH_STRING( queue->srch_type ),
                     srch4_queue_str[queue_type] );
      break;
    }
    case 3:
    {
      ERR( "Srch4-Lost dump on an invalid queue (%d)", queue_type, 0, 0 );
      break;
    }
    default:
    {
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
                     "Srch4 lost (%s on %s), aborting",
                     SRCH4_TECH_STRING( queue->srch_type ),
                     srch4_queue_str[queue_type] );
      break;
    }
  }

  #ifdef SRCH4_DEBUG
#error code not present
  #endif
  /*lint -restore */

} /* srch4_lost_dump_cb_wrapper */


/*===========================================================================

                   L O G G I N G   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_LOG_PARAMS
DESCRIPTION    This function fills the log parameters of the just started
                 searcher 4's search queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void srch4_log_params
(
  srch4_queue_type queue_type             /* queue to start                */
)
{
  srch4_queue_var_type   *queue       = NULL; /* to accommodate queues     */
  LOG_SRCH4_C_type       *log_pkt     = NULL; /* ptr to log pkt to fill    */
  srch4_task_struct_type *tasks       = NULL; /* ptr to task struct        */
  uint32                  i           = 0;    /* loop counter              */
  uint32                  coh_trunc   = 0;    /* temp var for coh_trunc    */
  uint32                  tmp_queue   = 0;    /* temp var for queue type   */
  log_code_type           log_code    = LOG_LAST_C;
                                              /* run time log code set for
                                                 each technology           */
  int16                   freq_offset_trunc;  /* log pkt freq offset val   */

  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue   = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    ERR( "Srch4-log parameters on an invalid queue (%d)", queue_type, 0, 0 );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  switch( queue->srch_type )
  {
    /* 1X search */
    case SRCH4_1X:
    {
      /* this log pkt is now obsolete, for 1X, only the TNG packet
         is supported */
      log_code = LOG_SRCH4_1X_C;

      return;
    }

    #ifdef SRCH4_HDR_ENABLED
    /* HDR search interpolation using shoulders */
    case SRCH4_HDR_SHOULDERS:
    /* HDR search with actual accumulation energies */
    case SRCH4_HDR_ACCUM:
    {
      #ifdef T_MDM7800
      log_code = LOG_SRCH4_MC_HDR_C;
      #else
      log_code = LOG_SRCH4_HDR_C;
      #endif /* T_MDM7800 */
      break;
    }
    #endif /* SRCH4_HDR_ENABLED */

    #ifdef SRCH4_AFLT_ENABLED
    /* AFLT search */
    case SRCH4_AFLT:
    {
      /* this log pkt is now obsolete */
      log_code = LOG_SRCH4_AFLT_C;

      return;
    }
    #endif /* SRCH4_AFLT_ENABLED */

    /* invalid technologies */
    default:
    {
      ERR( "Invalid Technology Type (%d) in Logging", queue->srch_type, 0, 0 );
      return;
    }
  }

  /* Is logging active for this log code? */
  if ( !log_status( log_code ) )
  {
    return;
  }

  /* get the log pkt */
  log_pkt = queue->log_pkt;

  /* log pkt ptr from diag, wasn't cleaned up properly last time, this
     shouldn't happen, but just in case, we'll clean it up */
  if ( queue->log_ptr != NULL )
  {
    ERR( "Log_ptr not NULL!!!", 0, 0, 0 );
    log_free(queue->log_ptr);
    queue->log_ptr = NULL;
  }

  /* header byte  1
     Technology            - bits  7: 5
     QPCH                  - bit      4
     NumPeaks              - bits  3: 0
   */
  log_pkt->Tech_QPCH_NumPeaks =
    ( ( ( uint8 ) queue->srch_type & 0x7 ) << 5 ) |
    ( ( queue->qpch ? 0x1 : 0x0 ) << 4 ) |
    ( ( SRCH4_NUM_PEAKS ) & 0xf );

  /* header byte  2
     Queue                 - bits  7: 4
     Priority              - bits  3: 0
   */
  switch( queue_type )
  {
    case SRCH4_HPQ0:
    {
      tmp_queue = 0;
      break;
    }
    #ifdef SRCH4_USE_LPQ
    case SRCH4_LPQ:
    {
      tmp_queue = 1;
      break;
    }
    #endif /* SRCH4_USE_LPQ */
    #ifdef SRCH4_HAS_HPQ1
    case SRCH4_HPQ1:
    {
      tmp_queue = 3;
      break;
    }
    #endif /* SRCH4_HAS_HPQ1 */
    default:
    {
      ERR( "Invalid Queue (%d) in Logging", queue_type, 0, 0 );
      return;
    }
  }
  log_pkt->Queue_Priority =
    ( ( tmp_queue & 0xF ) << 4 ) |
    ( ( queue->priority ) & 0xF );

  /* header byte  3
     NumTasks              - bits  7: 0
   */
  log_pkt->NumTasks =
    ( queue->num_tasks ) & 0xFF;

  /* header byte  4
     FreqOffset            - bits  7: 6
   */
  /* Round and convert Freq offset from 4.6875 Hz/LSB to 37.5 Hz/LSB as
     needed for the log pkt (only 8-bits in the log packet) */
  if ( queue->freq_offset < 0 )
  {
    freq_offset_trunc = ( queue->freq_offset - 4 ) / 8 ;
  }
  else
  {
    freq_offset_trunc =  ( queue->freq_offset + 4 ) / 8;
  }

  /* Check if the freq offset is within range for the log packet */
  if ( ( freq_offset_trunc > 127 ) || ( freq_offset_trunc < -127 ) )
  {
    /* Out of range indicator */
    freq_offset_trunc = 0x80;
  }

  log_pkt->FreqOffset =
    (int8) freq_offset_trunc;

  /* use a task ptr for brevity */
  tasks = queue->tasks;

  /* load the parameters into the log pkt */
  for( i=0; i<queue->num_tasks; i++ )
  {
    /* parameters bytes 1-2
       Pilot               - bits 15:7
       Diversity/Antenna   - bits 6 :5
       Burst Length        - bits 4 :3
       QOF                 - bits 2 :1
       WalshMSB            - bit  0
     */
    log_pkt->tasks[i].Pilot_Antenna_BurstLen_QOF_WalshMSB =
      ( ( tasks[i].pilot_inx & 0x1ff ) << 7 ) |
      ( ( ( uint16 )tasks[i].diversity & 0x3 ) << 5 ) |
      ( ( tasks[i].burst_len & 0x3 ) << 3 ) |
      ( ( tasks[i].qof & 0x3 ) << 1 ) |
      ( ( tasks[i].walsh & 0x01ff ) >> 8 );

    /* parameters bytes 3-4
       WalshLSB            - bit  14: 8
       Non-Coherent Length - bits 7 : 0
     */
    log_pkt->tasks[i].WalshLSB_NonCohLen =
      ( ( tasks[i].walsh & 0xff ) << 8 ) |
      ( tasks[i].non_coherent_len & 0xff);

    /* parameters bytes 5-6
       Coherent Truncation - bits 15:14
       Coherent Length     - bits 13: 0
     */
    /* for coherent truncation, output the number of bits truncated
       instead of what is programmed to the dsp
     */
    coh_trunc = (uint32) tasks[i].coherent_trunc + 1;

    log_pkt->tasks[i].CohTrunc_CohLen =
      ( ( coh_trunc & 0x7 ) << 13 ) |
      ( ( tasks[i].coherent_len & 0x1fff ) );

    /* parameters bytes 7-8
       PN Position (chips) - bits 15: 0
     */
    log_pkt->tasks[i].PNPos =
      tasks[i].pn_pos;

    /* parameters bytes 9-10
       Window Size (chips) - bits 15:0
     */
    log_pkt->tasks[i].WinSize =
      tasks[i].window_size;
    
    #ifdef T_MDM7800
    /* parameters bytes 11-12
       Carrier - bits 1:0
     */
    log_pkt->tasks[i].carrier =
      tasks[i].carrier;
    #endif /* T_MDM7800 */

  } /* for each task */

} /* srch4_log_params */


/*===========================================================================

FUNCTION       SRCH4_LOG_RESULTS
DESCRIPTION    This function requests and sends a searcher 4 log packet.
               It also fills the searcher 4's results into the packet
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   A log packet is requested and sent

===========================================================================*/
static void srch4_log_results
(
  srch4_queue_type queue_type,                /* queue to start            */
  int32            num_results                /* number of srches returned */
)
{
  srch4_queue_var_type   *queue       = NULL; /* to accommodate queues     */
  srch4_task_struct_type *tasks       = NULL; /* ptr to task struct to use */
  LOG_SRCH4_C_type       *log_pkt     = NULL; /* ptr to log pkt to fill    */
  log_code_type           log_code    = LOG_LAST_C;
                                              /* run time log code set for
                                                 each technology           */
  int32                   i=0, j=0;           /* loop counters             */

  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    ERR( "Srch4-log results on an invalid queue (%d)", queue_type, 0, 0 );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  switch( queue->srch_type )
  {
    /* 1X search */
    case SRCH4_1X:
    {
      /* this log pkt is now obsolete, for 1X, only the TNG packet
         is supported */
      log_code = LOG_SRCH4_1X_C;

      return;
    }

    #ifdef SRCH4_HDR_ENABLED
    /* HDR search interpolation using shoulders */
    case SRCH4_HDR_SHOULDERS:
    /* HDR search with actual accumulation energies */
    case SRCH4_HDR_ACCUM:
    {
      #ifdef T_MDM7800
      log_code = LOG_SRCH4_MC_HDR_C;
      #else
      log_code = LOG_SRCH4_HDR_C;
      #endif /* T_MDM7800 */
      break;
    }
    #endif /* SRCH4_HDR_ENABLED */

    #ifdef SRCH4_AFLT_ENABLED
    /* AFLT search */
    case SRCH4_AFLT:
    {
      /* this log pkt is now obsolete */
      log_code = LOG_SRCH4_AFLT_C;

      return;
    }
    #endif /* SRCH4_AFLT_ENABLED */

    /* invalid technologies */
    default:
    {
      ERR("Invalid Technology Type (%d) in Logging", queue->srch_type,0,0);
      return;
    }
  }

  /* Compute the size of the log packet. We don't want to send the whole
     array, just the entries that we populated */
  /* size of all fields prior to the task field plus the size of task
     structure calculation for the number of tasks and the number of
     peaks returned */
  queue->log_pkt_siz = SRCH4_LOG_SIZ( num_results, SRCH4_NUM_PEAKS );

  /* request log ptr from diag */
  queue->log_ptr = (LOG_SRCH4_C_type *) log_alloc( log_code,
                                          (unsigned int)queue->log_pkt_siz );

  /* Are we logging? */
  if ( queue->log_ptr != NULL )
  {
    /* get the log pkt */
    log_pkt = queue->log_pkt;

    #ifdef T_MDM7800
    /* header byte 1
       version               - bits  7:0
    */ 
    log_pkt->version = SRCH4_HDR_LOG_VERSION;
    #endif /* T_MDM7800 */

    /* header byte  7
       rx0_agc (ant 0)       - bits  7: 0
     */
    log_pkt->rx0_agc = rfm_get_cdma_rx_agc( RFCOM_TRANSCEIVER_0 );

    /* header byte  8
       rx1_agc (ant 1)       - bits  7: 0
     */
    log_pkt->rx1_agc = rfm_get_cdma_rx_agc( RFCOM_RECEIVER_1 );

    /* use a task ptr for brevity */
    tasks = queue->tasks;

    for( i=0; i<num_results; i++ )
    {

      for( j=0; j<SRCH4_NUM_PEAKS; j++)
      {
        /* results byte  1
           Reserved ( No need to fill this in )
        log_pkt->tasks[i].results[j].Reserved =
          0xff;
         */

        /* results bytes 2-4
           Peak Position (ChipX16) - bits 19:0
         */
        log_pkt->tasks[i].results[j].PeakPos_MSB =
          ( (tasks[i].peak_pos[j] >> 16) & 0xf );
        log_pkt->tasks[i].results[j].PeakPos_LSB =
          ( tasks[i].peak_pos[j] & 0xffff );

        /* results bytes 5-6
           Peak Energy - bits 15:0
         */
        log_pkt->tasks[i].results[j].PeakEng =
          ( tasks[i].peak_eng[j] );
      } /* for each peak */
    } /* for each task */

    /* copy the assigned diag hdr into the local hdr
       this will make copying later easier */
    memcpy( log_pkt, queue->log_ptr, sizeof(log_hdr_type) );

    /* copy the local log packet to the diag log pkt */
    /* this only works if the number of peaks returned is equal
       to SRCH4_NUM_PEAKS, which is how it is currently defined
       if this changes we will need to revisit */
    memcpy( queue->log_ptr, queue->log_pkt, queue->log_pkt_siz );

    /* send command to diag to send the packet */
    log_commit( (log_type *)queue->log_ptr );

    /* clear the ptr out */
    queue->log_ptr = NULL;

  } /* queue->log_ptr != NULL */

} /* srch4_log_results */

/*===========================================================================

FUNCTION       SRCH4_LOG_SRCH4_SUB_PARAMS
DESCRIPTION    This function fills the log parameters of the just started
                 searcher 4's 1X search queue
DEPENDENCIES   This logging is only valid for 1X

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void srch4_log_srch4_sub_params
(
  srch4_queue_type queue_type                   /* queue to log            */
)
{
  srch4_queue_var_type     *queue       = NULL; /* to accommodate queues   */
  srch_genlog_srch4_sub_v1 *sub_log_pkt = NULL; /* ptr to log pkt to fill  */
  srch4_task_struct_type   *tasks       = NULL; /* ptr to task struct      */
  uint32                   i            = 0;    /* loop counter            */
  log_code_type            log_code     = LOG_LAST_C;
                                                /* run time log code set
                                                   for each technology     */

  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );
  if ( queue == NULL )
  {
    ERR( "Srch4-log parameters on an invalid queue (%d)", queue_type, 0, 0 );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  switch( queue->srch_type )
  {
    case SRCH4_1X:
      log_code = SRCH_LOG_TNG_1X_SEARCHER_DUMP_C;
      break;

    default:
      ERR( "Invalid Technology Type (%d) in Logging", queue->srch_type, 0, 0 );
      return;
  }

  /* is logging active for this log code? */
  if ( !log_status(log_code) )
  {
    return;
  }

  /* get the log pkt */
  sub_log_pkt = queue->sub_log_pkt;

  /* Fill in parameters common for all tasks */
  sub_log_pkt->tech        = ( uint8 )queue->srch_type;
  sub_log_pkt->queue       = ( uint8 )queue_type;
  sub_log_pkt->priority    = queue->priority;
  sub_log_pkt->qpch_cntl   = queue->qpch;
  sub_log_pkt->freq_offset = queue->freq_offset;
  sub_log_pkt->num_tasks   = queue->num_tasks;
  sub_log_pkt->num_peaks   = SRCH4_NUM_PEAKS;

  /* use a task ptr for brevity */
  tasks = queue->tasks;

  /* load the parameters into the log pkt */
  for( i = 0; i < queue->num_tasks; i++ )
  {
    /* Pilot               - bits 15:6
       Diversity/Antenna   - bits 5 :4
       Burst Length        - bits 3 :2
       QOF                 - bits 1 :0 */
    sub_log_pkt->tasks[i].pilot_ant_burst_len_qof =
      ( ( tasks[i].pilot_inx & 0x3ff ) << 6 ) |
      ( ( ( uint16 )tasks[i].diversity & 0x3 ) << 4 ) |
      ( ( tasks[i].burst_len & 0x3 ) << 2 ) |
      ( ( tasks[i].qof & 0x3 ) );

    /* Coherent Truncation - bits 15:14
       Coherent Length     - bits 13: 0 */
    /* for coherent truncation, output the number of bits truncated
       instead of what is programmed to the dsp */
    sub_log_pkt->tasks[i].coh_trunc_coh_len =
      ( ( ( tasks[i].coherent_trunc + 1 ) & 0x7 ) << 13 ) |
      ( ( tasks[i].coherent_len & 0x1fff ) );

    /* Fill in the non-packed parameters */
    sub_log_pkt->tasks[i].walsh_index    = tasks[i].walsh;
    sub_log_pkt->tasks[i].noncoh_len     = tasks[i].non_coherent_len;
    sub_log_pkt->tasks[i].phase          = tasks[i].phase;
    sub_log_pkt->tasks[i].win_start      = ( tasks[i].window_pos /
                                             CHIPX8_PER_CHIP ) -
                                           tasks[i].window_size;
    sub_log_pkt->tasks[i].win_size       = tasks[i].window_size;
    sub_log_pkt->tasks[i].pilot_set_mask = tasks[i].pilot_set_mask;

  } /* for each task */

} /* srch4_log_srch4_sub_params */


/*===========================================================================

FUNCTION       SRCH4_BUILD_SRCH4_SUB
DESCRIPTION    Builds and commits a 1X searcher4 generalized subpacket
DEPENDENCIES   This logging is only valid for 1X

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_build_srch4_sub
(
  srch_genlog_packet_id_type  id,         /* id of packet to commit sub     */
  sub_commit_func_type        commit_fp,  /* f(x) to commit the subpacket   */
  uint32                      queue_type, /* queue to log                   */
  int32                       num_results /* number of srches returned      */
)
{
  srch4_queue_var_type     *queue       = NULL; /* to accommodate queues   */
  srch4_task_struct_type   *tasks       = NULL; /* ptr to task struct      */
  srch_genlog_srch4_sub_v1 *sub_log_pkt = NULL; /* ptr to gen log pkt      */
  int32                     i=0, j=0;           /* loop counters           */

  /*-----------------------------------------------------------------------*/

  if ( commit_fp == NULL )
  {
    ERR( "srch4_build_srch4_sub got NULL commit_fp ptr", 0, 0, 0 );

    return;
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( (srch4_queue_type) queue_type );
  if ( queue == NULL )
  {
    ERR( "Srch4-log results on an invalid queue (%d)", queue_type, 0, 0 );
    return;
  }

  /* figure out which log packet should be sent, this is based on technology
     note this is run time, allowing filtering of the searcher4 log packet
     based on which technology is sending the search request */
  if ( queue->srch_type != SRCH4_1X )
  {
    ERR( "Invalid Technology Type (%d) in Logging", queue->srch_type, 0, 0 );
    return;
  }

  /* get the log pkt */
  sub_log_pkt = queue->sub_log_pkt;

  /* use a task ptr for brevity */
  tasks = queue->tasks;

  for( i = 0; i < num_results; i++ )
  {
    for( j = 0; j < SRCH4_NUM_PEAKS; j++)
    {
      /* peak position (ChipX16) and energy*/
      sub_log_pkt->tasks[i].results[j].peak_pos  = tasks[i].peak_pos[j];
      sub_log_pkt->tasks[i].results[j].peak_eng  = tasks[i].peak_eng[j];

    } /* for each peak */
  } /* for each task */

  sub_log_pkt->hdr.id   = SRCH_SUB_ID_SEARCHER_4;
  sub_log_pkt->hdr.ver  = LOG_VERSION_SRCH4_DUMP;
  sub_log_pkt->hdr.size = SRCH4_SUB_LOG_SIZ(num_results,SRCH4_NUM_PEAKS);

  /* commit the buffer */
  commit_fp(id,&sub_log_pkt->hdr);

} /* srch4_build_srch4_sub */


/*===========================================================================

                   D R I V E R   F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_INIT
DESCRIPTION    This function initializes anything and everything srch4drv
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   Timers, Completed Tasks variables, etc. are initialized

===========================================================================*/
void srch4_init( void )
{
  srch4_queue_type        i;                /* loop var for queues    */
  srch4_queue_var_type   *queue     = NULL; /* to accommodate queues  */

  /*-----------------------------------------------------------------------*/

  /* loop through the three queues: HPQ0, HPQ1, LPQ */
  for( i = SRCH4_MIN_Q; i < SRCH4_MAX_Q; i++ )
  {
    queue = srch4_get_queue( i );

    /* check to ensure a valid queue has been assigned */
    if ( queue != NULL )
    {
      /* Set up REX timers for catching lost status dumps.
         Callback function for both is the lost_dump_cb_wrapper */
      timer_def( queue->lost_dump_timer, NULL, rex_self(), 0,
                 (timer_t1_cb_type)( srch4_lost_dump_cb_wrapper ),
                 (timer_cb_data_type)i );

      /* clear the number of results to the queues */
      SRCH4_CLR_NUM_RESULTS( i );

      queue->srch_type = SRCH4_NO_TECH;
    }
  }

} /* srch4_init */


/*===========================================================================

FUNCTION       SRCH4_FLUSH_S_RAM
DESCRIPTION    This function flushes the sample ram in the DSP if no searches
                are active
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Sample ram in DSP is flushed

===========================================================================*/
int32 srch4_flush_s_ram
(
  sample_server_rx_chain_type samp_serv_chain, /* The sample server chain */
  uint16                      position,        /* position in chips */
  boolean                     scheduled        /* do a scheduled flush? */
)
{
  srch4_queue_type  i;                                /* queue index var */

  /*-----------------------------------------------------------------------*/

  /* loop through the three queues: HPQ0, HPQ1, LPQ */
  for( i = SRCH4_MIN_Q; i < SRCH4_MAX_Q; i++ )
  {
    if ( !srch4_queue_available( i ) )
    {
      /* something was active or being programmed, don't touch S_RAM */
      ERR( "Attempted to flush sample ram while searcher is active (%i)",
           i, 0, 0 );
      return( SRCH4DRV_RET_ACTIVE );
    }
  }

  /* none of the queues are currently active */
  /* flush the DSP sample ram */
  INTLOCK();

  /* If asked to flush sample rams for both chains do so. */
  if ( samp_serv_chain == SAMPLE_SERVER_RX_CHAIN_BOTH )
  {
    if(scheduled)
    {
      #ifdef SCHED_SRAM_FLUSH_TRIGGER
      SRCH4_SCHED_FLUSH_SAMPLE_RAM( (uint8) SAMPLE_SERVER_RXTX_CHAIN,
                                     position );
      SRCH4_SIGNAL_DSP();
      SRCH4_SCHED_FLUSH_SAMPLE_RAM( (uint8) SAMPLE_SERVER_RX_CHAIN, position );
      SRCH4_SIGNAL_DSP();
      #else
      NOTUSED(position);
      MSG_ERROR("functionality not available",0,0,0);
      #endif /* SCHED_SRAM_FLUSH_TRIGGER */
    }
    else
    {
      SRCH4_FLUSH_SAMPLE_RAM( (uint8) SAMPLE_SERVER_RXTX_CHAIN );
      SRCH4_SIGNAL_DSP();
      SRCH4_FLUSH_SAMPLE_RAM( (uint8) SAMPLE_SERVER_RX_CHAIN );
      SRCH4_SIGNAL_DSP();
    }
  }
  /* Flush the sample ram for the requested chain. */
  else if ( ( samp_serv_chain == SAMPLE_SERVER_RXTX_CHAIN ) ||
            ( samp_serv_chain == SAMPLE_SERVER_RX_CHAIN ) )
  {
    if(scheduled)
    {
      #ifdef SCHED_SRAM_FLUSH_TRIGGER
      SRCH4_SCHED_FLUSH_SAMPLE_RAM( (uint8) samp_serv_chain, position );
      SRCH4_SIGNAL_DSP();
      #else
      NOTUSED(position);
      MSG_ERROR("functionality not available",0,0,0);
      #endif /* SCHED_SRAM_FLUSH_TRIGGER */
    }
    else
    {
      SRCH4_FLUSH_SAMPLE_RAM( (uint8) samp_serv_chain );
      SRCH4_SIGNAL_DSP();
    }
  }
  else
  {
    MSG_ERROR("RX device not supported for flushing",0,0,0);
  }

  #ifdef SRCH4_DEBUG
#error code not present
  #endif

  INTFREE();

  return( SRCH4DRV_RET_OK );

} /* srch4_flush_s_ram */


/*===========================================================================

FUNCTION       SRCH4_ADD_TASK
DESCRIPTION    This function adds a task to specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified, tasks are added to the DSP

===========================================================================*/
int32 srch4_add_task
(
  srch4_srch_type         task_srch_type, /* search type                  */
  srch4_queue_type        queue_type,     /* queue to add tasks to        */
  srch4_task_struct_type *task            /* task to add                  */
)
{
  srch4_queue_var_type   *queue     = NULL;/* to accommodate queues       */
  uint16                  inv_win_size = 0;/* temp. var for window size if
                                               it is invalid              */

  int32                   ret_status = SRCH4DRV_RET_OK;
                                           /* save return value to be
                                              returned out of the INTLOCK()
                                              block */

  uint32                  msg_flag = 0;    /* to save the type of error message
                                              inside the INTLOCK() block */

  /*-----------------------------------------------------------------------*/

  /* check for valid inputs */
  if ( task == NULL )
  {
    MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
                   "Addition of invalid task to srch4 %s queue",
                   srch4_queue_str[queue_type] );

    return( SRCH4DRV_RET_INVALID_PARAM );
  }

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    ERR( "Tech: %d tried to add a task a when not registered",
         task_srch_type, 0, 0 );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    ERR( "Trying to add a task on an invalid queue (%d)",
         task_srch_type, 0, 0 );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* lock the interrupts */
  INTLOCK();

  /* Did we overflow the queue? */
  if ( queue->num_tasks >= queue->max_num_tasks )
  {
    /* We are trying to program more tasks then fit in the queue */
    ret_status = SRCH4DRV_RET_FULL;
    msg_flag = 1;
  }
  /* is the queue active? */
  else if ( queue->active )
  {
    if ( queue->lost || queue->aborted )
    {
      /* somehow we didn't clean up after the last search possibile lost
         and never aborted. Try and clean this up by aborting the
         current active search */
      ret_status = SRCH4DRV_RET_UNCLEAN;
      msg_flag = 2;
    }
    else
    {
      /* We are trying to add a task and the searcher is already running */
      ret_status = SRCH4DRV_RET_ACTIVE;
      msg_flag =3;
    }
  }
  /* check to make sure we have the same search types */
  else if ( (task_srch_type   != queue->srch_type) &&
            (queue->num_tasks != 0 ) )
  {
    /* trying to program different technologies on the same search */
    ret_status = SRCH4DRV_RET_TECH_MISMATCH;
    msg_flag = 4;
  }
  #ifdef SRCH4_AFLT_ENABLED
  /* is AFLT and only AFLT on LPQ? */
  else if ( ( (task_srch_type == SRCH4_AFLT) &&
              (queue_type     != SRCH4_LPQ)     ) ||
            ( (task_srch_type != SRCH4_AFLT) &&
              (queue_type     == SRCH4_LPQ)     )    )
  {
    ret_status =  SRCH4DRV_RET_TECH_MISMATCH;
    msg_flag = 5;
  }
  #endif /* SRCH4_AFLT_ENABLED */
  /* Is the coherent length within bounds? */
  else if ( ( task->coherent_len < COH_LEN_MIN ) ||
            ( task->coherent_len > COH_LEN_MAX )
            #ifdef SRCH4_AFLT_ENABLED
            ||
            ( ( task->coherent_len > COH_LEN_AFLT_MAX ) &&
              ( task_srch_type == SRCH4_AFLT ) )
            #endif /* SRCH4_AFLT_ENABLED */
          )
  {
    ret_status = SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 6;
  }
  /* Is the non coherent length within bounds? */
  else if ( ( task->non_coherent_len < NON_COH_LEN_MIN ) ||
            ( task->non_coherent_len > NON_COH_LEN_MAX )
            #ifdef SRCH4_HDR_ENABLED
            || 
            ( ( task->non_coherent_len > NON_COH_LEN_HDR_MAX ) &&
              ( ( task_srch_type == SRCH4_HDR_SHOULDERS ) ||
                ( task_srch_type == SRCH4_HDR_ACCUM ) ) )
              #endif /* SRCH4_HDR_ENABLED */
            )
  {
    ret_status = SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 7;
  }
  /* is M*N within bounds? */
  #ifdef SRCH4_USE_LPQ
  else if ( ( ( task->non_coherent_len * task->coherent_len ) > M_X_N_MAX ) &&
            ( queue_type != SRCH4_LPQ ) )
  #else /* !SRCH4_USE_LPQ */
  else if ( ( ( task->non_coherent_len * task->coherent_len ) > M_X_N_MAX ) )
  #endif /* SRCH4_USE_LPQ */
  {
    ret_status =  SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 8;
  }
  /* is the diversity field valid? */
  else if ( task->diversity >= SRCH4_ANT_MAX )
  {
    ret_status =  SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 9;
  }
  /* is burst length valid and only used for HDR? */
  else if ( ( task->burst_len > BURST_LEN_MAX )
            #ifdef SRCH4_HDR_ENABLED
            &&
            ( ( task_srch_type == SRCH4_HDR_SHOULDERS ) ||
              ( task_srch_type == SRCH4_HDR_ACCUM ) ) 
            #endif
          )
  {
    ret_status = SRCH4DRV_RET_INVALID_PARAM;
    msg_flag = 10;
  }
  /* all's well and good */
  else
  {
    /* set the search type */
    queue->srch_type = task_srch_type;

    /* catch to make sure window size is within range */
    if ( task->window_size > WINSIZ_MAX )
    {
      /* throw in the maximum window size so as not to confuse the MDSP */
      inv_win_size      = task->window_size;
      task->window_size = WINSIZ_MAX;

      msg_flag = 11;
    }
    if ( task->window_size < WINSIZ_MIN )
    {
      /* throw in the minimum window size so as not to confuse the MDSP */
      inv_win_size      = task->window_size;
      task->window_size = WINSIZ_MIN;

      msg_flag = 12;
    }


    /* add task to the searcher4 queue */

    /* set window size */
    SRCH4_WIN_SIZE( queue_type,
                    queue->num_tasks,
                    task->window_size );

    /* set pn position */
    SRCH4_PN_POSITION( queue_type,
                       queue->num_tasks,
                       task->pn_pos );

    /* set pn phase and coherent integration length */

    #ifdef T_MDM7800
    SRCH4_CARRIER_M( queue_type,
                     queue->num_tasks,
                     task->carrier,
                     task->coherent_len );
    #else
    SRCH4_PHASE_M( queue_type,
                   queue->num_tasks,
                   task->phase,
                   task->coherent_len );
    #endif /* T_MDM7800 */

    if ( ( queue->srch_type == SRCH4_1X )
         #ifdef SRCH4_AFLT_ENABLED
         ||
         ( queue->srch_type == SRCH4_AFLT )
         #endif /* SRCH4_AFLT_ENABLED */
       )
    {
      /* set pilot index and # non coherent integrations */
      SRCH4_PILOT_N( queue_type,
                     queue->num_tasks,
                     task->pilot_inx,
                     task->non_coherent_len );
    }
    else /* srch_type == SRCH4_HDR_xxxx */
    {
      /* set pilot index, # non coherent integrations, and burst length */
      SRCH4_PILOT_BURST_N( queue_type,
                           queue->num_tasks,
                           task->pilot_inx,
                           task->burst_len,
                           task->non_coherent_len );
    }

    /* set the Coherent truncation, QOF and walsh index */
    SRCH4_DIV_CT_QOF_WALSH( queue_type,
                            queue->num_tasks,
                            (uint8) task->diversity,
                            task->coherent_trunc,
                            task->qof,
                            task->walsh );

    #ifdef SRCH4_USE_LPQ
    if( queue_type == SRCH4_LPQ )
    {
      /* update estimated lpq lost dump time */
      if ( task->window_size <= SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT )
      {
        /* window size is less than SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT,
           normal search times */
        queue->lost_dump_time_estimate += SRCH4_LPQ_DUMP_TIMEOUT_NORMAL;
      }
      else
      {
        /* window size is greater than SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT,
           expect a longer search */
        queue->lost_dump_time_estimate += SRCH4_LPQ_DUMP_TIMEOUT_MAX;
      }
    }
    else /* HPQs */
    #endif /* SRCH4_USE_LPQ */
    {
      /* update estimated hpq lost dump time */
      if ( task->window_size <= SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT )
      {
        /* window size is less than SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT,
           normal search times */
        queue->lost_dump_time_estimate += SRCH4_HPQ_DUMP_TIMEOUT_NORMAL;
      }
      else
      {
        /* window size is greater than SRCH4_WINSIZE_NORM_TIMEOUT_LIMIT,
           expect a longer search */
        queue->lost_dump_time_estimate += SRCH4_HPQ_DUMP_TIMEOUT_MAX;
      }
    }

    /* increase number of programmed tasks */
    queue->num_tasks++;

    #ifdef SRCH4_DEBUG
#error code not present
    #endif

  }

  /* unlock the interrupts */
  INTFREE();

  /* send MSG's and ERR's outside of INTLOCKs */
  switch ( msg_flag )
  {
    case 0:                   /* all tests passed */
      break;

    case 1:
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Searcher4 overflow too many tasks (%s)(tasks=%d, max=%d)",
            srch4_queue_str[queue_type], queue->num_tasks,
            queue->max_num_tasks );
      break;

    case 2:
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Srch4 was lost/aborted w/o cleanup (%s)",
            srch4_queue_str[queue_type] );

      (void) srch4_abort_queue( queue->srch_type, queue_type,
                                NULL, queue->srch_cb_data, FALSE, FALSE );
      break;

    case 3:
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Searcher 4 already running (%s)",
            srch4_queue_str[queue_type] );
      break;

    case 4:
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Srch4 Prog diff srch types to same %s queue (%s/%s)",
            srch4_queue_str[queue_type],
            SRCH4_TECH_STRING( task_srch_type ),
            srch4_queue_str[queue_type] );

    case 5:
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Trying to program a tech (%s) into an invalid queue(%s)",
            SRCH4_TECH_STRING( task_srch_type ),
            srch4_queue_str[queue_type] );
      break;

    case 6:
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Invalid Srch4 param - coh_len %d (48-3584, 48-2048 AFLT) - %s on %s",
            task->coherent_len,
            SRCH4_TECH_STRING( task_srch_type ),
            srch4_queue_str[queue_type] );
      break;

    case 7:
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Invalid Srch4 param - non_coh_len %d (1-128, 1-2 HDR) - %s on %s",
            task->non_coherent_len,
            SRCH4_TECH_STRING( task_srch_type ),
            srch4_queue_str[queue_type] );
      break;

    case 8:
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Invalid Srch4 param - tot int len %d (3948 HPQ) - %s on %s",
            ( task->non_coherent_len * task->coherent_len ),
            SRCH4_TECH_STRING( task_srch_type ),
            srch4_queue_str[queue_type] );
      break;

    case 9:
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Invalid Srch4 param - diversity %d (0-2) - %s on %s",
            task->diversity,
            SRCH4_TECH_STRING( task_srch_type ),
            srch4_queue_str[queue_type] );
      break;

    case 10:
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
           "Invalid Srch4 param - burst_len %d (1-2 HDR) - %s on %s",
            task->burst_len,
            SRCH4_TECH_STRING( task_srch_type ),
            srch4_queue_str[queue_type] );
      break;

    case 11:
      MSG_ERROR( "Invalid WIN SIZE %d for Srch4 (replaced w/ %d)(4-%d)",
                  inv_win_size, task->window_size, WINSIZ_MAX );
      break;

    case 12:
      MSG_ERROR( "Invalid WIN SIZE %d for Srch4 (replaced w/ %d)(4-%d)",
                  inv_win_size, task->window_size, WINSIZ_MAX );
      break;

    default:                  /* switch statement should never come here */
      MSG_ERROR( "srch4_add_task(): Abnormal msg_flag (%d) ", msg_flag, 0, 0);
      break;

  }

  #ifdef SRCH4_DEBUG
#error code not present
  #endif

  /* return relevant status */
  return( ret_status );

} /* srch4_add_task */


/*===========================================================================

FUNCTION       SRCH4_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the
                specified priority queue.
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   Srch4 structure is modified. A queue of searches is started

===========================================================================*/
int32 srch4_start_search
(
  srch4_srch_type  srch4_type,            /* type of search                 */
  srch4_queue_type queue_type,            /* queue to start                 */
  int16  freq_offset,                     /* freq offset ( 4.6875 Hz/LSB )  */
  uint16 priority,                        /* search priority (32767=highest)*/
  void (*callback_func)(void *),          /* pointer to a callback function */
                                          /*  to be called upon status dump */
  void (*lost_dump_callback_func)(void *),/* pointer to a callback function */
                                          /*  to be called upon lost dump   */
  void *(cb_data)                         /* data to be passed to callback  */
)
{
  srch4_queue_var_type   *queue;          /* to accommodate queues          */
  uint32                  msg_flag = 0;   /* flag for errors and messages   */

  /*-----------------------------------------------------------------------*/

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    ERR( "Tech: %d tried to start a search when not registered",
         srch4_type, 0, 0 );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    ERR( "Trying to start a search on an invalid queue (%d)",
         queue_type, 0, 0 );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  INTLOCK();

  if ( queue->active )
  {
    /* trying to start a search while another one is active */
    msg_flag = 1;
  }
  else if ( queue->num_tasks == 0 )
  {
    /* trying to start a search without any searches in the queue */
    msg_flag = 2;
  }
  /* check to make sure we have the same search types */
  else if ( ( srch4_type != queue->srch_type ) &&
            ( queue->num_tasks != 0 ) )
  {
    msg_flag = 3;
  }
  else
  {
    /* store the frequency offset */
    queue->freq_offset = freq_offset;

    /* store the priority of this search */
    queue->priority    = priority;

    /* program final number of HPQ tasks for this search */
    SRCH4_NUM_TASKS( queue_type, queue->num_tasks );

    /* program the HPQ search type */
    SRCH4_SRCH_TYPE( queue_type, (uint8) queue->srch_type );

    /* set the search priority */
    SRCH4_SRCH_PRIORITY( queue_type, queue->priority );

    #if ( defined( FEATURE_XO ) || defined( T_QSC60X5 )  || \
          defined( T_MSM8650B ) || defined( T_MDM7800 ) )
    #ifdef SRCH4_HDR_ENABLED
    /* When technology is HDR, set the frequency offset, e.g. for binning */
    if ( ( srch4_type == SRCH4_HDR_SHOULDERS ) ||
         ( srch4_type == SRCH4_HDR_ACCUM ) )
    {
      /* program the frequency offset to apply to all searches */
      SRCH4_SET_FREQ_OFFSET( queue_type, freq_offset );
    }
    else
    #endif /* SRCH4_HDR_ENABLED */
    {
      /* For non-HDR technologies, the search rotator should not be used
         as there is a Rotator in the RX front */
      SRCH4_SET_FREQ_OFFSET( queue_type, 0 );
    }
    #else /* !(FEATURE_XO || T_QSC60X5) */
    /* program the frequency offset to apply to all searches */
    SRCH4_SET_FREQ_OFFSET( queue_type, freq_offset );
    #endif /* FEATURE_XO || T_QSC60X5 */

    /* notify the DSP that there is a new high priority task list waiting */
    SRCH4_START( queue_type );
    SRCH4_SIGNAL_DSP();

    /* store the callback functions (these could be NULL) */
    queue->dump_cb_func = callback_func;
    queue->lost_cb_func = lost_dump_callback_func;
    queue->srch_cb_data = cb_data;

    /* mark searcher4 active, not aborted, and not lost */
    queue->active      = TRUE;
    queue->aborted     = FALSE;
    queue->lost        = FALSE;
    queue->partial_res = FALSE;

    /* store parameters into the log pkt while we're searching */
    srch4_log_params( queue_type );
    if ( queue->srch_type == SRCH4_1X )
    {
      srch4_log_srch4_sub_params( queue_type );
    }

    #ifdef SRCH4_DEBUG
#error code not present
    #endif

    /* Start the Lost Dump timer
       This must be done here to ensure that all of the flags (active, aborted,
       lost) get set before a possible task switch (and thus interrupt
       enabling) that can occur with a call to timer_set() */
    timer_set( queue->lost_dump_timer,
               queue->lost_dump_time_estimate, 0, T_MSEC );
  }

  INTFREE();

  /* send MSG's and ERR's outside of INTLOCKs */
  switch( msg_flag )
  {
    case 1:
    {
      /* trying to start a search while another one is active */
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
                     "Searcher 4 %s already running",
                     srch4_queue_str[queue_type] );

      return( SRCH4DRV_RET_ACTIVE );
    }
    case 2:
    {
      /* trying to start a search without any searches in the queue */
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
                     "Searcher 4 trying to start an empty %s",
                     srch4_queue_str[queue_type] );

      switch( queue_type )
      {
        case SRCH4_HPQ0:
        {
          return( SRCH4DRV_RET_HPQ0_FREE );
        }
        #ifdef SRCH4_USE_LPQ
        case SRCH4_LPQ:
        {
          return( SRCH4DRV_RET_LPQ_FREE );
        }
        #endif /* SRCH4_USE_LPQ */
        #ifdef SRCH4_HAS_HPQ1
        case SRCH4_HPQ1:
        {
          return( SRCH4DRV_RET_HPQ1_FREE );
        }
        #endif /* SRCH4_HAS_HPQ1 */
        default:
        {
          return( SRCH4DRV_RET_INVALID_QUEUE );
        }
      }
    }
    case 3:
    {
      /* trying to start a search of a different technology */
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_ERROR,
                     "Trying to srch multi types (%s, qst=%d, rst=%d)",
                     srch4_queue_str[queue_type],
                     SRCH4_TECH_STRING( queue->srch_type),
                     SRCH4_TECH_STRING( srch4_type) );

      return( SRCH4DRV_RET_TECH_MISMATCH );
    }
    default:
    {
      /* no errors to report */

      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
                     "Started search for %s on %s with %d tasks",
                     SRCH4_TECH_STRING( srch4_type),
                     srch4_queue_str[queue_type],
                     queue->num_tasks );

      #ifdef SRCH4_DEBUG
#error code not present
      #endif

      /* return that srch4 has started */
      return( SRCH4DRV_RET_OK );
    }
  }

} /* srch4_start_search */


/*===========================================================================

FUNCTION       SRCH4_ABORT_QUEUE
DESCRIPTION    This function aborts the specified priority queue
DEPENDENCIES   None

RETURN VALUE   Srch4drv error value
SIDE EFFECTS   One queue in srch4 is aborted

===========================================================================*/
int32 srch4_abort_queue
(
  srch4_srch_type         task_srch_type, /* search type                    */
  srch4_queue_type        queue_type,     /* queue to abort tasks to        */
  void                  (*abort_cb_func)( uint32, void * ),
                                          /* pointer to a callback function */
                                          /*  to be called upon abort dump  */
  void                  *(cb_data),       /* callback data                  */
  boolean                 sync,           /* synchronous abort flag         */
  boolean                 part_res        /* partial results flag           */
)
{
  srch4_queue_var_type   *queue;          /* to accommodate queues        */
  uint32                  num_tasks  = 0; /* number of tasks to be aborted*/
  uint32                  msg_flag   = 0; /* flag for errors and messages */

  /*-----------------------------------------------------------------------*/

  /* check if this technology was not assigned a queue */
  if ( queue_type == SRCH4_NO_QUEUE )
  {
    /* this is not right, mark this as an ERR */
    ERR( "Tech: %d tried to abort a search when not registered",
         task_srch_type, 0, 0 );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( queue == NULL )
  {
    ERR( "Trying to abort a search on an invalid queue (%d)",
         task_srch_type, 0, 0 );

    return( SRCH4DRV_RET_INVALID_QUEUE );
  }

  INTLOCK();

  num_tasks = queue->num_tasks;

  /* searcher is not active thus a "real" abort does not need to be issued */
  if ( !queue->active )
  {
    /* is there an unstarted queue? is it assigned to this technology? */
    if ( ( queue->num_tasks != 0 ) && ( queue->srch_type != task_srch_type ) )
    {
      /* one technology is trying to abort another's search */
      msg_flag = 2;
    }
    else
    {
      /* same technology or empty queue */

      /* send no f3 msg since no abort is sent in this case */
      msg_flag = 0;

      /* searcher 4 queue not active, or active but empty,
         just clear programmed searches */
      queue->num_tasks = 0;

      /* clear the lost dump timers */
      (void) timer_clr( queue->lost_dump_timer, T_NONE );

      /* clear the lost dump time estimate */
      queue->lost_dump_time_estimate = 0;
    }
  }
  /* one technology is trying to abort another's active search */
  else if ( queue->srch_type != task_srch_type )
  {
    msg_flag = 2;
  }
  /* searcher was not really running thus a "real" abort does not
     need to be issued */
  else if ( queue->active && ( queue->num_tasks == 0 ) )
  {
    /* send no f3 msg since no abort is sent in this case */
    msg_flag = 0;

    /* searcher 4 queue not active, or active but empty,
       just clear programmed searches */
    queue->num_tasks = 0;

    /* clear the lost dump timers */
    (void) timer_clr( queue->lost_dump_timer, T_NONE );

    /* clear the lost dump time estimate */
    queue->lost_dump_time_estimate = 0;
  }
  else /* valid abort, searcher queue was active */
  {
    /* send an f3 msg to show abort */
    msg_flag = 1;

    /* store the callback data */
    queue->abort_cb_func = abort_cb_func;
    queue->srch_cb_data  = cb_data;

    /* abort the requested queue */
    SRCH4_ABORT( queue_type );
    SRCH4_SIGNAL_DSP();

    #ifdef SRCH4_DEBUG
#error code not present
    #endif

    /* increase number of aborted searches */
    queue->abort_cnt++;

    /* flag this search as aborted */
    queue->aborted = TRUE;

    /* flag if partial results are wanted */
    queue->partial_res = part_res;

    /* Re-Start the Lost Dump timer just in case the abort is lost
       This must be done here to ensure that all of the flags (active, aborted,
       lost) get set before a possible task switch (and thus interrupt
       enabling) that can occur with a call to timer_set() */
    timer_set( queue->lost_dump_timer,
               queue->lost_dump_time_estimate, 0, T_MSEC );

    /* is this a synchronous abort, i.e complete the abort before we continue */
    if( sync )
    {
      /* wait on the DSP's semaphore */
      MDSP_CHECK_CMD_UNTIL_E( SRCH_microSrchCmd, 0 );
    }

  }

  INTFREE();

  switch ( msg_flag )
  {
    case 1:
    {
      if ( part_res )
      {
        MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_HIGH,
                      "Aborting Srch4 (%s on %s) (%d tasks prog) (Partial Res)",
                       SRCH4_TECH_STRING( task_srch_type ),
                       srch4_queue_str[queue_type],
                       num_tasks);
      }
      else
      {
        MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_LEGACY_HIGH,
                       "Aborting Srch4 (%s on %s) (%d tasks prog)",
                       SRCH4_TECH_STRING( task_srch_type ),
                       srch4_queue_str[queue_type],
                       num_tasks);
      }

      /* queue aborted */
      return( SRCH4DRV_RET_OK );
    }

    case 2:
    {
      MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_LEGACY_HIGH,
                     "No abort processed: %s tried to abort %s's srch",
                     SRCH4_TECH_STRING( task_srch_type ),
                     SRCH4_TECH_STRING( queue->srch_type ) );

      /* queue aborted */
      return( SRCH4DRV_RET_TECH_MISMATCH );
    }

    default:
    {
      /* queue not active, or empty */
      switch( queue_type )
      {
        case SRCH4_HPQ0:
        {
          return( SRCH4DRV_RET_HPQ0_FREE );
        }
        #ifdef SRCH4_USE_LPQ
        case SRCH4_LPQ:
        {
          return( SRCH4DRV_RET_LPQ_FREE );
        }
        #endif /* SRCH4_USE_LPQ */
        #ifdef SRCH4_HAS_HPQ1
        case SRCH4_HPQ1:
        {
          return( SRCH4DRV_RET_HPQ1_FREE );
        }
        #endif /* SRCH4_HAS_HPQ1 */
        default:
        {
          return( SRCH4DRV_RET_INVALID_QUEUE );
        }
      }
    }
  } /* switch */

} /* srch4_abort_queue */


/*===========================================================================

FUNCTION       SRCH4_PROCESS_STATUS_DUMP
DESCRIPTION    This function retrieves the results of the specified searcher4
                sweep. It fills the task structure then calls the specified
                callback function to process the dump.
DEPENDENCIES   Type of dump should have been checked (srch4_status_dump)
                This should not be a lost or an aborted dump (unless partial
                results are requested for aborted searches)

RETURN VALUE   None
SIDE EFFECTS   Srch4 structure is modified, processing callback is called

===========================================================================*/
static void srch4_process_status_dump
(
  srch4_queue_type        queue_type,      /* queue to process             */
  uint32                  num_completed_tasks /* # of completed tasks      */
)
{
  srch4_queue_var_type   *queue    = NULL; /* to accommodate queues        */
  srch4_task_struct_type *tasks    = NULL; /* ptr to task struct to use    */

  int32                   msg_flag    = 0; /* flag for errors and messages */

  uint32                  res_inx     = 0; /* result index variable        */
  uint32                  peak_inx    = 0; /* peak index variable          */

  int32                   peak_pos    = 0; /* peak position in Cx8         */
  uint32                  peak_pos_n  = 0; /* peak_position (normalized)   */
  uint16                  peak_eng    = 0; /* peak energy                  */
  uint16                  peak_ls_eng = 0; /* left shoulder energy         */
  uint16                  peak_rs_eng = 0; /* right shoulder energy        */

  uint32                  peak_posx2_n_raw = 0;
                                           /* uninterpolated peak position */
  uint16                  peak_eng_raw = 0; /* Uninterpolated peak energy  */
  /*-----------------------------------------------------------------------*/

  /* check which queue to use, and store ptr */
  queue = srch4_get_queue( queue_type );

  /* check to ensure a valid queue has been assigned */
  if ( ( queue == NULL ) || ( queue->tasks == NULL ) )
  {
    ERR( "Trying to process a status dump on an invalid queue (%d)",
          queue_type, 0, 0 );

    return;
  }

  tasks = queue->tasks;

  INTLOCK();

  /* store the number of results */
  queue->last_srch_res_cnt = (uint8) num_completed_tasks;

  /* Check to make sure the searcher is active, this is a sanity check as it
     was checked from the calling function */
  if ( queue->active )
  {
    /* loop over all finished tasks */
    for ( res_inx = 0; res_inx < num_completed_tasks; res_inx++ )
    {
      /* Make window pos positive 0 ... 262143 */
      tasks[res_inx].window_pos &= PN_CIRCLE_MASK;

      /* loop over all peaks */
      for ( peak_inx = 0; peak_inx < SRCH4_NUM_PEAKS; peak_inx++ )
      {
        /* read out results for this peak and task */
        peak_eng_raw = SRCH4_PEAK_ENG( queue_type, res_inx, peak_inx );
        peak_posx2_n_raw = SRCH4_PEAK_POS( queue_type, res_inx, peak_inx )
                           & PN_CIRCLE_MASK;

        peak_pos  = peak_posx2_n_raw * CHIPX8_PER_HALF_CHIP;

        peak_eng    = peak_eng_raw;
        peak_ls_eng = SRCH4_LS_ENG( queue_type, res_inx, peak_inx );
        peak_rs_eng = SRCH4_RS_ENG( queue_type, res_inx, peak_inx );

        {
          /* If this was a receive divesity search */
          if ( tasks[res_inx].diversity == SRCH4_ANT_DUAL )
          {
            /* Shift all the results down since they were accumulated
               from both antennae */
            peak_eng    >>= RXDIV_SHIFT;
            peak_ls_eng >>= RXDIV_SHIFT;
            peak_rs_eng >>= RXDIV_SHIFT;
          }

          /* Get peak position and energy via interpolation
              from shoulder energies */
          srch4_interpolate_peak( peak_pos, peak_eng, peak_ls_eng, peak_rs_eng,
                                  &peak_pos, &peak_eng );

          /* Make peak pos positive in Cx16 (0 ... 524286)
              Using ((pn mask for ChipX8) * (ChipX16 per ChipX8) + 1)*/
          peak_pos_n = peak_pos &
                       ( PN_CIRCLE_MASK * CHIPX16_PER_EIGHTH_CHIP + 1);
        }

        /* store peak pos and peak eng */
        tasks[res_inx].peak_pos[peak_inx]       = peak_pos_n;
        tasks[res_inx].peak_eng[peak_inx]       = peak_eng;
        tasks[res_inx].peak_posx2_raw[peak_inx] = peak_posx2_n_raw;
        tasks[res_inx].peak_eng_raw[peak_inx]   = peak_eng_raw;
      } /* loop over the peaks */

      /* process the search with the given function */
      if ( tasks[res_inx].process_results != NULL )
      {
        (*tasks[res_inx].process_results)( &tasks[res_inx] );
      }

      /* one more search completed */
      queue->task_cnt++;
    } /* loop over results */

    /* we processed a queue, increase counter */
    queue->srch_cnt++;
  }
  else /* searcher is not active */
  {
    msg_flag = 1;
  }

  INTFREE();

  /* send MSG's and ERR's outside of INTLOCKs */
  switch( msg_flag )
  {
    case 1:
    {
      ERR( "Srch4 %d Status Dump while inactive", queue_type, 0, 0 );
      break;
    }
    default:
    {
      /* no errors to report */
    }
  }

} /* srch4_process_status_dump */


/*===========================================================================

FUNCTION       SRCH4_HANDLE_ABORT_DONE
DESCRIPTION    This function is called when an abort has finished.
DEPENDENCIES   None

RETURN VALUE   None.
SIDE EFFECTS   if partial results are wanted, the finished tasks results will
               be stored.
===========================================================================*/
static void srch4_handle_abort_done
(
  srch4_queue_type      queue_type,         /* HPQ0, HPQ1, or LPQ        */
  const srch4_queue_var_type
                       *queue,              /* to accommodate queues     */
  uint32                num_results,        /* >0 if partial results are
                                               wanted and are available  */
  boolean               abort_flag          /* aborted dump              */
)
{
  uint32                i,j;                /* loop counter              */
  /*-----------------------------------------------------------------------*/

  if ( queue == NULL )
  {
    ERR( "srch4_handle_abort_done got NULL queue ptr", 0, 0, 0 );

    return;
  }

  #ifdef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
  NOTUSED( abort_flag );
  #endif

  #ifdef SRCH4_DEBUG
#error code not present
  #endif

  #ifndef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
  /* Does the DSP think that this is an abort? */
  if ( !abort_flag )
  {
    ERR( "Expected Abort, instead got normal status dump (%d)",
         queue_type, 0, 0 );
  }
  #endif

  /* either way, process this like we would an abort whether or not it
     was an abort, because that's what is expected */

  /* clear the lost dump timer, so it doesn't fire while we process */
  (void) timer_clr( queue->lost_dump_timer, T_NONE );

  /* store the partial results, if we've got it! */
  if ( num_results > 0 )
  {
    /* process the returned tasks */
    srch4_process_status_dump( queue_type, num_results );

    /* clear the remaining tasks so logging can indicate which weren't
       completed */
    for ( i = num_results; i < queue->num_tasks; i++ )
    {
      for ( j = 0; j < SRCH4_NUM_PEAKS; j++ )
      {
        queue->tasks[i].peak_eng[j] = 0;
        queue->tasks[i].peak_pos[j] = 0;
      }
    }
  }

} /* srch4_handle_abort_done */


/*===========================================================================

FUNCTION       SRCH4_EVAL_STATUS_DUMP
DESCRIPTION    Checks a specified queue to determine if it triggered the
                status dump, then takes the appropriate action
DEPENDENCIES   None

RETURN VALUE   Number of tasks whose results were stored
               SRCH4_NO_DUMP (-1) if this queue did not cause the dump
SIDE EFFECTS   Search results are processed

===========================================================================*/
static int32 srch4_eval_status_dump
(
  srch4_queue_type      queue_type,         /* the queue type             */
  const srch4_queue_var_type
                       *queue,              /* to accommodate queues      */
  uint32                num_results,        /* number of results returned */
  boolean               abort_flag          /* aborted dump               */
)
{
  int32                 stored_results = SRCH4_NO_DUMP;
                                            /* num of results stored      */
  /*-----------------------------------------------------------------------*/

  if ( queue == NULL )
  {
    ERR( "srch4_eval_status_dump got NULL queue ptr", 0, 0, 0 );

    return( SRCH4_NO_DUMP );
  }

  /* check to make sure this is the queue sending the dump */
  if ( ( queue->active ) &&
       ( ( queue->lost ) ||
         ( queue->aborted ) ||
         ( queue->num_tasks == num_results ) ) )
  {
    /* clear the lost dump timer, so it doesn't fire while we process */
    (void) timer_clr( queue->lost_dump_timer, T_NONE );

    /* lost queue was aborted and has now dumped */
    if ( ( queue->lost ) && ( queue->aborted ) )
    {
      /* Handle as you would an aborted search, but ensure that all results
         are thrown away (hence the "0" for number of results!
         Call the abort callback since the lost dump callback was assigned
         to in the lost dump callback wrapper */
      srch4_handle_abort_done( queue_type, queue, 0, abort_flag );

      /* search was lost, so no results were stored */
      stored_results = 0;
    }
    /* aborted queue has dumped */
    else if ( queue->aborted )
    {
      /* call the abort handler, with notification of whether partial results
         are wanted */
      if ( queue->partial_res )
      {
        srch4_handle_abort_done( queue_type, queue, num_results, abort_flag );

        /* search was aborted, and partial results was requested */
        stored_results = num_results;
      }
      else
      {
        srch4_handle_abort_done( queue_type, queue, 0, abort_flag );

        /* search was aborted, and partial results was not requested */
        stored_results = 0;
      }
    }
    else if ( queue->lost )
    {
      /* this case should never happen, if it's lost it should also always
         be aborted */
      ERR( "Srch4 lost dump without an abort! (%d/%d)",
           queue_type, queue->srch_type, 0 );

      /* search was lost, so no results were stored */
      stored_results = 0;
    }
    /* normal dump */
    else if ( queue->num_tasks == num_results )
    {
      srch4_process_status_dump( queue_type, num_results );

      /* normal search dump, all results recorded */
      stored_results = num_results;
    }

    #ifdef SRCH4_DEBUG
#error code not present
    #endif

    /* return the number of results recorded */
    return( stored_results );

  } /* queue caused dump? */

  /* return value of SRCH4_NO_DUMP indicates this queue did
     not issue the dump */
  return( SRCH4_NO_DUMP );

} /* srch4_eval_status_dump */


/*===========================================================================

FUNCTION       SRCH4_STATUS_DUMP
DESCRIPTION    This function is used when a status dump is triggered.
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   Callbacks will be called

===========================================================================*/
static void srch4_status_dump( void )
{
  srch4_queue_var_type *queue;               /* to accommodate queues      */
  int32                 num_results = 0;     /* number of results returned */
  int32                 dump[SRCH4_MAX_Q];   /* indicates a processed dump
                                                and how many were processed*/
  boolean               dump_occured = FALSE;/* at least one dump occurred */
  srch4_queue_type      i;                   /* loop var for queues        */
  boolean               aborted = FALSE;     /* flag for callbacks         */
  boolean               lost    = FALSE;     /* flag for callbacks         */

  boolean               flag_d = TRUE, flag_a = TRUE;
                                             /* MDSP abort and dump flags  */

  /*-----------------------------------------------------------------------*/

  INTLOCK();

  /* loop through the three queues: HPQ0, HPQ1, LPQ */
  for( i = SRCH4_MIN_Q; i < SRCH4_MAX_Q; i++ )
  {
    dump[i] = SRCH4_NO_DUMP;

    queue   = srch4_get_queue( i );

    #ifdef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
    /* this flag won't be used */
    NOTUSED( flag_d );
    /* flag_a was initialized to TRUE and will be used */
    #else
    /* check if there was a search done dump for this queue */
    SRCH4_CHECK_SEARCH_DONE( i, flag_d );

    /* check if there was an abort dump for this queue */
    SRCH4_CHECK_ABORT( i, flag_a );
    #endif /* !SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE */

    #ifndef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
    if ( flag_d || flag_a )
    #endif
    {
      /* check if a valid queue was returned */
      if ( queue != NULL )
      {
        num_results = SRCH4_NUM_RESULTS( i );
        dump[i]     = srch4_eval_status_dump( i, queue, num_results, flag_a );
        if ( dump[i] != SRCH4_NO_DUMP )
        {
          /* clear num_results */
          SRCH4_CLR_NUM_RESULTS( i );
        }
      }
    } /* flags set */
  } /* loop through the queues */

  INTFREE();

  /* clear vars, fill and send log packet, etc outside of the intlock */
  /* loop through all of the three queues */
  for( i = SRCH4_MIN_Q; i < SRCH4_MAX_Q; i++ )
  {
    /* check if a dump occurred and if so log it */
    if ( dump[i] != SRCH4_NO_DUMP )
    {
      queue = srch4_get_queue( i );

      /* check if a valid queue was returned */
      if ( queue != NULL )
      {
        /* reset aborted or lost flag */
        aborted = FALSE;
        lost    = FALSE;

        /* log and report only if we had a normal status dump */
        if ( !( queue->aborted || queue->lost ) )
        {
          /* note: do not print out strings here, as this will be executed
                   in isr context! */
          MSG_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
                 "Srch4 dump for tech %d on %d with %d tasks",
                 queue->srch_type, i, queue->num_tasks );

          /* log the results and send the log pkt */
          srch4_log_results( i, dump[i] );
          if ( queue->srch_type == SRCH4_1X )
          {
            srch_genlog_1x_srch4_dump( (uint32) i, dump[i] );
          }
        }
        /* was the search lost? */
        else if ( queue->lost )
        {
          MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
                 "Srch4 Lost for tech %d on %d",
                 queue->srch_type, i );

          /* set lost flag */
          lost = TRUE;

          /* clear the log ptr (it was never assigned) */
          queue->log_ptr = NULL;
        }
        /* if we are saving partial results, log but we've already sent a msg */
        else if ( queue->aborted && queue->partial_res )
        {
          MSG_4( MSG_SSID_1X_SRCH, MSG_LEGACY_HIGH,
                 "Srch4 Partial dump for tech %d on %d with %d of %d tasks",
                 queue->srch_type, i, dump[i], queue->num_tasks );

          /* set aborted flag */
          aborted = TRUE;

          /* log the results and send the log pkt */
          srch4_log_results( i, dump[i] );
          if ( queue->srch_type == SRCH4_1X )
          {
            srch_genlog_1x_srch4_dump( (uint32) i, dump[i] );
          }
        }
        /* was the search aborted? */
        else if ( queue->aborted )
        {
          MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
                 "Srch4 successfully aborted for tech %d on %d",
                 queue->srch_type, i );

          /* set aborted or lost flag */
          aborted = TRUE;

          /* clear the log ptr (it was never assigned) */
          queue->log_ptr = NULL;
        }
        /* something weird, just clear log ptr and continue */
        else
        {
          queue->log_ptr = NULL;
        }

        /* give the translation layer a chance to clean up also */
        srch4_trans_cleanup_dump( queue->srch_type );

        /* clear num tasks */
        queue->num_tasks = 0;

        /* mark searcher as inactive and not aborted */
        queue->active  = FALSE;
        queue->aborted = FALSE;
        queue->lost    = FALSE;

        /* clear the lost dump time estimate */
        queue->lost_dump_time_estimate = 0;

        /* set the technology to "invalid" */
        queue->srch_type = SRCH4_NO_TECH;

        /* Is there a callback we are supposed to call? */
        if ( lost )
        {
          if ( queue->lost_cb_func != NULL )
          {
            /* Call the callback */
            (*queue->lost_cb_func)( queue->srch_cb_data );
          }
        }
        else if ( aborted )
        {
          if ( queue->abort_cb_func != NULL )
          {
            /* Call the callback */
            (*queue->abort_cb_func)( dump[i], queue->srch_cb_data );
          }
        }
        else /* normal status dump */
        {
          /* call the callback function */
          if ( queue->dump_cb_func != NULL )
          {
            (*queue->dump_cb_func)( queue->srch_cb_data );
          }
        }

        /* set the flag to indicate this wasn't an unexpected Dump */
        dump_occured = TRUE;

      } /* queue != NULL */
    } /* if dump */
  } /* queue loop */

  /* no queued dumped, print out an error */
  if ( !dump_occured )
  {
    MSG_MED("Searcher 4 Unexpected Dump", 0,0,0);
  }

} /* srch4_status_dump */


/*===========================================================================

FUNCTION       SRCH4_HALT
DESCRIPTION    This function should only be called in preparation for the
                MDSP app being unregistered for 1x, HDR, and AFLT. Also all
                active searches will be aborted, without any callbacks
                specified.
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   All pending searches will not be processed when a dump occurs
               Any task waiting for a callback from srch4 will NOT recieve
               them, as all callbacks have been cleared

===========================================================================*/
void srch4_halt( void )
{

  srch4_queue_var_type *queue;               /* to accommodate queues      */
  srch4_queue_type      i;                   /* loop var for queues        */

  /*-----------------------------------------------------------------------*/

  /* loop through all queues */
  for ( i = SRCH4_MIN_Q; i < SRCH4_MAX_Q; i++ )
  {
    queue = srch4_get_queue( i );

    /* check if a valid queue was returned */
    if ( queue != NULL )
    {
      /* abort any search that is active */
      if ( queue->active )
      {
        /* send a synchrous abort to ensure that searcher 4 is completely
           inactive before this function is exited */
        (void)srch4_abort_queue( queue->srch_type, i, NULL, NULL, TRUE, FALSE );
      }

      /* clear every queue's status variables */
      queue->srch_type = SRCH4_NO_TECH;
      queue->active    = FALSE;
      queue->aborted   = FALSE;
      queue->lost      = FALSE;
      queue->num_tasks = 0;
      queue->lost_dump_time_estimate = 0;

      /* clear out the callbacks so an inadvertant dump will not occur */
      queue->dump_cb_func  = NULL;
      queue->abort_cb_func = NULL;
      queue->lost_cb_func  = NULL;
    } /* queue != NULL */
  } /* queue loop */

  #ifdef SRCH4_DEBUG
#error code not present
  #endif

} /* srch4_halt */


/*===========================================================================

FUNCTION       SRCH4_DUMP_ISR
DESCRIPTION    This function is invoked to service the end of search
                interrupt generated by the DSP.
DEPENDENCIES   This function assumes that pre- and post-interrupt processing
                with respect to stacks and REX are taken care of by the caller

RETURN VALUE   None.
SIDE EFFECTS   Status dump is processed, signals are sent

===========================================================================*/
void srch4_dump_isr( void )
{
  #ifndef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
  boolean flag = FALSE;
  #endif /* SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE */

  /*-----------------------------------------------------------------------*/

  MSM_LOCK();

  #ifdef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
  /* retrieve data from last search */
  srch4_status_dump();
  #else  /* !SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE */
  /* check if the interrupt was caused by a search dump */
  SRCH4_CHECK_SEARCH_INT_FLAG( flag );
  if ( flag )
  {
    /* retrieve data from last search */
    srch4_status_dump();
  }
  #endif /* SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE */

  MSM_UNLOCK();

} /* srch4_dump_isr */

