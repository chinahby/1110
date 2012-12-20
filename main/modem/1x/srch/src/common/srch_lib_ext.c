
/*====*====*====*====*====*====*====*========*====*====*====*====*====*====*==*

        S E A R C H   T A S K  --
        S E A R C H   L I B R A R Y   E X T E R N A L

GENERAL DESCRIPTION
  This file contains library interfaces from non-librarized searcher code
  into the search library.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

      Copyright (c) 2004 - 2010
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/src/common/srch_lib_ext.c#3 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/18/10   sst     Added TC AGC check flags and thresholds
08/31/09   vks     Added PCH->QPCH channel estimator threshold type
09/03/08   tjc     Correct the RDDS capacity thresholds
07/28/08   aps     T_MSM8650B bringup changes
06/25/08   adw     Added min_sleep_setup_time_safewin_ms
06/20/08   mca     Merge from //depot
03/13/08   sst     Modify FTS version to be determined at runtime
01/11/08   aps     Add RTR6500_WARMUP_OVERHEAD outside of srch library
10/29/07   sst     Add (en/dis)able of GOVERN_ACQ_FAIL outside of srch library
                   Unfeaturize all code GOVERN_ACQ_FAIL code except init
06/27/07   aps     Moved cgps_sw_overhead_* featurization to custsrch.h
06/21/07   aps     Featurize cgps_sw_overhead_* for T_QSC60X5
06/21/07   sst     Added cgps_sw_overhead_*
06/18/07   mca     Removed use of nv items defines from library
04/23/07   tjc     Changed VCO gain and SRL multiplier for T_QSC60X5.
02/21/07   aps     Added VCO gain and SRL multiplier knob.
11/08/06   tjc     Moved tunable RX diversity autoswitching parameters int
                    srch_lib_ext.c/h
02/17/05   sst     Moved reference to MDSP_APP_SRCH into srch_lib_ext.c/h to
                    facilite custumer compiles will differing Feature set
01/25/05   kwo     Exported some sched parms outside of library
12/28/04   bt      implementation, first cut

=============================================================================*/


/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "task.h"
#include "srch_lib_ext.h"


/*===========================================================================
             LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
      Constants and Macros
-------------------------------------------------------------------------*/
/* REVISIT: Remove asap as changes get propogated. */
#ifndef MIN_SLEEP_SETUP_TIME_SAFEWIN_MS
#define MIN_SLEEP_SETUP_TIME_SAFEWIN_MS           2
#endif /* MIN_SLEEP_SETUP_TIME_SAFEWIN_MS */

#define CHIPS_PER_MS             1229     /* How many chips per ms */ 

const srch_lib_ext_defines_type srch_lib_defs =
{
  /* common defs */
  {
    SRCH_PRI_MAX,
    SRCH_PRI,
    MDSP_APP_SRCH,

    /* rx diversity autoswitching defs */
    #ifdef FEATURE_SRCH_RXDIV_AUTOSWITCH
    {
      7510000,   /* RC3 Tbase : -6.50dB in linear Q25 */
      3760000,   /* RC4 Tbase : -9.50dB in linear Q25 */
      6,         /* Fq      : Time const 1.27s */
      (1<<11),   /* Tq      : 0.125 in Q14 */
      500,       /* Dq      : 500=10sec */
      32,        /* FPCAdj  : 32=2dB in Q4 */
    },
    #endif /* FEATURE_SRCH_RXDIV_AUTOSWITCH */

    #ifdef FEATURE_FULL_TIME_SHDR_3
    SRCH_USE_FTS_3,
    #else
    SRCH_USE_FTS_NONE,
    #endif

    #if (defined( T_QSC60X5 ) || defined( T_MSM8650B ))
    AFC_VCO_GAIN_X5_SRL_X2,
    #else /* T_QSC60X5 || T_MSM8650B */
#error code not present
    #endif  /* T_QSC60X5 || T_MSM8650B */

    TRUE,        /* tc_rxd_agc_ck */
    25,          /* tc_rxd_agc_dis_delta */
    20,          /* tc_rxd_agc_en_delta */
  },
  /* init defs */
  {
    #ifdef FEATURE_SRCH_GOVERN_ACQ_FAIL
    5,     /* CD governor time       */
    TRUE,  /* CD governor enabled    */
    5,     /* ACQ governor time      */
    TRUE,  /* ACQ governor enabled   */
    #else
    0,     /* CD governor time       */
    FALSE, /* CD governor enabled    */
    0,     /* ACQ governor time      */
    FALSE, /* ACQ governor enabled   */
    #endif
  },
  /* idle defs */
  {
    /* warmup defs */
    {
      RF_WARMUP_BASE_TIME_MS,
      RF_WARMUP_CX8_ON_TIME_MS,
      WARMUP_SW_LATENCY_SAFEWIN_MS,
      TCXO_CLOCK_SWITCH_OVERHEAD_MS,
      TCXO_WARMUP_MS,
      CLK_OVERHEAD_MS,
      CGPS_SW_OVERHEAD_MS,
      WARMUP_RTR6500_OVERHEAD_MS,
      MIN_SLEEP_SETUP_TIME_SAFEWIN_MS,

      RF_WARMUP_BASE_TIME_US,
      RF_WARMUP_CX8_ON_TIME_US,
      WARMUP_SW_LATENCY_SAFEWIN_US,
      TCXO_CLOCK_SWITCH_OVERHEAD_US,
      TCXO_WARMUP_US,
      CLK_OVERHEAD_US,
      CGPS_SW_OVERHEAD_US,
      WARMUP_RTR6500_OVERHEAD_US,

      MIN_SLEEP_SETUP_TIME_SAFEWIN_MS * CHIPS_PER_MS
    },

    /* sched defs */
    {
      13056, /* REACQ_WIN_NOISE_THRESH_LECIO */
      3,     /* FAST_RAHO_SRCH_PENALTY */
      6,     /* FAST_RAHO_REG_PENALTY */
      200    /* FAILED_REACQ_SCAN_TIME_MS */
    },

    /* qpch defs */
    {
      #ifdef FEATURE_JCDMA
      THRESH_TYPE_OLD      /* ch_est_thresh_tab_type */
      #else
      THRESH_TYPE_NEW      /* ch_est_thresh_tab_type */
      #endif /* FEATURE_JCDMA */
    }
  },
  /* nv_items defs */
  {
    NV_CDMA_RX_DIVERSITY_CTRL_I,
    NV_CDMA_RX_CHAIN_SEL_THRESH_I,
    NV_SRCH_DBG_MASK_I,
    NV_CDMA_RX_DIV_BC_ENABLE_MASK_I
  }
};


