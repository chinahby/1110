/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   D R I V E R

GENERAL DESCRIPTION
   This file contains the internal definitions for the searcher4 drivers.

EXTERNALIZED FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2003 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/src/srch4drvi.c#3 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
09/08/08   adw      Added SRCH4_HAS_LPQ to featurize low priority queue.
12/14/07   sst      Added SRCH4_MDSP2_CODE_USES_ONE_QUEUE_IMAGE
05/04/04   sst      Merge from srch4 3Q dev branch
04/12/04   sst      Removed debug include file
04/08/04   sst      Code cleanup
                    Mainlined FEATURE_SRCH4_MDSP2
                    Created SRCH4_HAS_HPQ1, SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
04/06/04   sst      Updated some featurization
02/26/04   sst      Lint corrections
02/20/04   sst      Intermediate Checkin
10/10/03   sst      Created file 
===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"

#include "mdspext.h"
#include "qw.h"
#include "srch_mdsp.h"

#include "srch4drv.h"
#include "srch4drvi.h"


/*===========================================================================
             static DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*lint -save -e740  Unusual pointer cast
                    this is the lint error for SRCH_dummy_addr in the LPQ */
/*-------------------------------------------------------------------------
      Memory Address structures
-------------------------------------------------------------------------*/
/*  these types and structures are used to simplify the macros used to
    read and write into the shared memory between the DSP and Searcher
 */

/* create dummy memory address to write to for those parameters that are not 
   valid for a specific queue (i.e. LPQ has no SearchType field). */
int32 SRCH_dummy_addr = NULL;

/* memory map between different MSM targets, and MDSP loads */
#ifdef SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
/* we are using a previous version of the MDSP image */
#define  SRCH_microHpq0SearchType       SRCH_microHpqSearchType
#define  SRCH_microHpq0SearchDone       (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microHpq0Priority         (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microHpq0AbortDone        SRCH_microHpqAbort
#define  SRCH_microHpq0NumTasks         SRCH_microHpqNumTasks
#define  SRCH_microHpq0FreqOffset       SRCH_microHpqFreqOffset
#define  SRCH_microHpq0SearchParams     SRCH_microHpqSearchParams
#define  SRCH_microHpq0NumResults       SRCH_microHpqNumResults
#define  SRCH_microHpq0HPQResults       SRCH_microHpqResults

#define  SRCH_microLpqSearchDone       (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqPriority         (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqAbortDone         SRCH_microLpqAbort

#endif /* SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE */

#ifdef SRCH4_MDSP2_CODE_USES_ONE_QUEUE_IMAGE
/* we are using a one queue version of the MDSP image */
#define  SRCH_microHpq0SearchType       SRCH_microHpqSearchType
#define  SRCH_microHpq0SearchDone       SRCH_microHpqSearchDone
#define  SRCH_microHpq0Priority         SRCH_microHpqPriority
#define  SRCH_microHpq0AbortDone        SRCH_microHpqAbortDone
#define  SRCH_microHpq0NumTasks         SRCH_microHpqNumTasks
#define  SRCH_microHpq0FreqOffset       SRCH_microHpqFreqOffset
#define  SRCH_microHpq0SearchParams     SRCH_microHpqSearchParams
#define  SRCH_microHpq0NumResults       SRCH_microHpqNumResults
#define  SRCH_microHpq0HPQResults       SRCH_microHpqHPQResults


#define  SRCH_microLpqSearchDone       (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqPriority         (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqAbortDone        (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqAbortDone        (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqSearchDone       (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqNumTasks         (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqFreqOffset       (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqSearchParams     (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqNumResults       (volatile uint16 *)(&SRCH_dummy_addr)
#define  SRCH_microLpqResults          (volatile uint16 *)(&SRCH_dummy_addr)

#endif /* SRCH4_MDSP2_CODE_USES_ONE_QUEUE_IMAGE */

/* Memory address for each type of search */
/*  for 1st array index
     if FEATURE_SRCH4_MDSP2  if !FEATURE_SRCH4_MDSP2
      0 = SRCH4_HPQ0          0 = SRCH4_HPQ0
      1 = SRCH4_LPQ           1 = SRCH4_LPQ
      2 = SRCH4_HPQ1          2 = SRCH4_MAX_Q
      3 = SRCH4_MAX_Q
    make sure this is the same mapping as: "srch4_queue_type" in srch4drv_t.h
 */
volatile uint16 *srch4_mem_map[ SRCH4_MAX_Q ][ SRCH4I_MAX_ADDR ] =
{
  /* HPQ0 */
  {
    SRCH_microHpq0AbortDone,         /* SRCH4I_ABORT_ADDR           */
    SRCH_microHpq0SearchDone,        /* SRCH4I_SEARCH_DONE_ADDR     */
    SRCH_microHpq0NumTasks,          /* SRCH4I_NUM_TASKS_ADDR       */
    SRCH_microHpq0SearchType,        /* SRCH4I_SRCH_TYPE_ADDR       */
    SRCH_microHpq0Priority,          /* SRCH4I_PRIORITY_ADDR        */
    SRCH_microHpq0FreqOffset,        /* SRCH4I_FREQ_OFFSET_ADDR     */
    SRCH_microHpq0SearchParams,      /* SRCH4I_SRCH_PARAMS_ADDR     */
    SRCH_microHpq0NumResults,        /* SRCH4I_NUM_RESULTS_ADDR     */
    SRCH_microHpq0HPQResults,        /* SRCH4I_RESULTS_ADDR         */
  },
  #ifdef SRCH4_USE_LPQ
  /* LPQ */
  {
    SRCH_microLpqAbortDone,          /* SRCH4I_ABORT_ADDR           */
    SRCH_microLpqSearchDone,         /* SRCH4I_SEARCH_DONE_ADDR     */
    SRCH_microLpqNumTasks,           /* SRCH4I_NUM_TASKS_ADDR       */
    (volatile uint16 *)(&SRCH_dummy_addr), /* SRCH4I_SRCH_TYPE_ADDR */
    (volatile uint16 *)(&SRCH_dummy_addr), /* SRCH4I_PRIORITY_ADDR  */
    SRCH_microLpqFreqOffset,         /* SRCH4I_FREQ_OFFSET_ADDR     */
    SRCH_microLpqSearchParams,       /* SRCH4I_SRCH_PARAMS_ADDR     */
    SRCH_microLpqNumResults,         /* SRCH4I_NUM_RESULTS_ADDR     */
    SRCH_microLpqResults,            /* SRCH4I_RESULTS_ADDR         */
  },
  #endif /* SRCH4_USE_LPQ */
  #ifdef SRCH4_HAS_HPQ1
  /* HPQ1 */
  {
    SRCH_microHpq1AbortDone,         /* SRCH4I_ABORT_ADDR           */
    SRCH_microHpq1SearchDone,        /* SRCH4I_SEARCH_DONE_ADDR     */
    SRCH_microHpq1NumTasks,          /* SRCH4I_NUM_TASKS_ADDR       */
    SRCH_microHpq1SearchType,        /* SRCH4I_SRCH_TYPE_ADDR       */
    SRCH_microHpq1Priority,          /* SRCH4I_PRIORITY_ADDR        */
    SRCH_microHpq1FreqOffset,        /* SRCH4I_FREQ_OFFSET_ADDR     */
    SRCH_microHpq1SearchParams,      /* SRCH4I_SRCH_PARAMS_ADDR     */
    SRCH_microHpq1NumResults,        /* SRCH4I_NUM_RESULTS_ADDR     */
    SRCH_microHpq1HPQResults,        /* SRCH4I_RESULTS_ADDR         */
  },
  #endif /* SRCH4_HAS_HPQ1 */
};

/*lint -restore */


/*===========================================================================

           I N T E R N A L   D R I V E R   F U N C T I O N S

===========================================================================*/

