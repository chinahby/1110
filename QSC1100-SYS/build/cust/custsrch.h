#ifndef CUSTSRCH_H
#define CUSTSRCH_H
/*===========================================================================

DESCRIPTION
  Configuration for SRCH.

  Copyright (c) 2002 - 2009
                by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/build/rel/ulc/custsrch.h#19 $ $DateTime: 2009/01/13 12:29:14 $ $Author: awoodcox $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/09   adw     Increased RF_WARMUP_BASE_TIME_MS from 6ms to 8ms.
12/01/08   adw     Turn off dynamic power strobe gpio assignment.
11/12/08   adw     Turn on dynamic power strobe gpio assignment.
10/20/08   adw     Added FEATURE_SRCH_CGPS_IF in gps feature check.
09/30/08   adw     Modified power strobe feature name.
09/25/08   adw     Changes to make format consistent across all targets.
09/16/08   adw     Corrected error in ULC target verification check.
09/09/08   adw     Featurized mask to remove all messages on low mem targets.
08/28/08   adw     Cleaned up featurization and seperated feature dependencies.
                   Moved mcs specific features to custmcs.h. 
08/21/08   vks     Added FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS to remove
                   internal log packets for ULC
08/18/08   adw     Changed single ant to be based on FEATURE_MULTI_RX_CHAIN.
                   Added FEATURE_SRCH_HAS_NO_TRM to remove TRM for ULC.
08/13/08   vks     Added FEATURE_SRCH_HAS_NO_IRM and FEATURE_SRCH_HAS_NO_RTL
08/13/08   adw     Added FEATURE_SRCH_MSG_LITE for ULC.
08/13/08   adw     Changed message mask to compile out srch debug msgs.
07/15/08   adw     Added features for ULC memory reduction.
07/14/08   vks     Added FEATURE_SRCH_SINGLE_ANTENNA. Disabled FEATURE_SRCH_RX_DIV
                   FEATURE_CDMA_RX_DIVERSITY, FEATURE_SOOS, FEATURE_SRCH_RTL, 
                   FEATURE_SRCH_RTL_IF2, FEATURE_SRCH_RX1_RELA_DEMOD
06/20/08   mca     Merge from //depot
07/13/07   tjc     Turn on RDDS and BC mask diversity switching when diversity
                   is defined
06/27/07   aps/sst Update rf warmup time on Q60x5 NOR builds
                   Added CGPS overhead time define
06/26/07   aps     Added FEATURE_SRCH_UNLOCK_ONLY_IMMEDIATE
04/23/07   sst     Split QLIC Support and QLIC Algorithm into two defines
04/04/07   aps     Changes for QSC60X5 slotted mode sleep
02/23/07   sst     Define FEATURE_SRCH_SUPPORTS_QLIC
           aps     Added AFC Knob setting defines.
08/21/06   awj     Add FEATURE_SRCH_NA_CLOCKS_REMAIN_ON until HDR clock voting
                   is enabled
06/28/06   awj     Increase WARMUP_SW_LATENCY_SAFEWIN_MS by 2ms.
06/06/06   pa/awj  Enable DBG_SRCH_AFC for AFC State Machine support
04/25/06   pa/sst  Enable 1x related changes to support TCXOMGR 2.1
04/06/06   rkc     Decrease RF_WARMUP_BASE_TIME by 2ms.
01/23/06   bt      Increase RF_WARMUP_BASE_TIME by 2ms.
01/04/06   rkc     Add FEATURE_SRCH_RX1_RELA_DEMO
11/04/05   awj     Enable srch_irm msgs.
11/03/05   gs      Add FEATURE_1X_SRCH_AFC_SRL
10/18/05   bt      Added FEATURE_SRCH_RTL and FEATURE_SRCH_RTL_IF2
09/23/05   ejv     Always define FEATURE_TL_PROF.
09/14/05   ejv     Remove un-referenced features.
08/23/05   ejv     Enable srch_rx msgs.
08/16/05   sst     Enabled FEATURE_SOOS
07/05/05   ejv     Enabled FEATURE_SRCH_PC2.
05/03/05   ejv     Added 1ms to RF_WARMUP_BASE_TIME_MS.
04/27/05   ejv     Add undef FEATURE_SRCH_PC2.
04/26/05   bt      Re-add FEATURE_QPCH_DYNAMIC_THRESHOLD
04/21/05   rng     Add FEATURE_SRCH4_PCG
04/20/05   bt      Add FEATURE_ZZ2_1
04/08/05   bt      Add FEATURE_TL_PROF.
03/31/05   bt      Change warmup numbers to reflect removal of RX front
                   dependency on RTC.
03/22/05   ejv     Define FEATURE_ACQ2.  Undefine FEATURE_SEARCH4_ACQ.
03/15/05   ejv     Undefine FEATURE_ACQ2 and remove references to
                   FEATURE_FING_MASKING, SEARCH2 features,
                   FEATURE_HW_FING_MERGE_DETECT, and FEATURE_FAST_FINGER_SLEW.
01/03/05   ejv     Added another bit to the msg mask.
10/18/04   bt      Add FEATURE_QPCH_DYNAMIC_THRESHOLD.
09/30/04   sfm     Removed FEATURE_FINGER_DRIVER
09/30/04   bt      Add FEATURE_TCXO_WARMUP_IN_SLEEP_TASK and make warmup
                   memory tech. dependent.
08/20/04   sfm     added FEATURE_FINGER_DRIVER
06/08/04   gs      Added FEATURE_1X_SRCH_USES_ROTATOR_AFC.
06/01/04   bt      Added warmup defines in micro second resolution.
05/28/04   jcm     Added another bit to the msg mask
05/04/04   ejv     Define FEATURE_NONSLOT_OFREQ_NSRCH.
02/27/04   ejv     Added another bit to the msg mask
02/05/04   sst     Added another bit to the msg mask
01/26/04   bt      Change rf warmup from 5ms to 4ms.
01/11/04   gs      Added last good pilot definitions.
01/08/04   ejv     Define FEATURE_SEARCH1.
11/19/03   bt      Change rf warmup time to 5ms.
11/05/03   sst     Added SRCH_HAS_TRK_LO_CNTL
11/05/03   bgc     Updated TCXO_WARMUP_MS to 3 to match xtal spec.
10/24/03   sst     Added SRCH_HAS_PILOT_GAIN_CNTL
09/23/03   bt      Change RF warmup times for MSM6500.
09/21/03   bt      Include new message Mask.
09/15/03   ejv     Undefine FEATURE_FING_MASKING.
08/20/03   bt      Add FEATURE_QPCH_HYBRID
08/05/03   ejv     Comment cleanup.
07/09/03   ejv     Cleanup obsolete features and re-order.
07/08/03   bt      Add #undef FEATURE_WAKEUP_TCXO_CLK and warmup time comp.
06/26/03   ejv     Remove #undef FEATURE_EXT_LINEARIZER.  Update warmup
                   numbers.  Remove #define FEATURE_DYNAMIC_THROTTLE.
04/28/03   sst     Mainlined FEATURE_SD20 and SRCH_ENABLE_SD20
02/18/03   ejv     Added FEATURE_FING_MASKING.
02/12/03   gs      Moved SRCH_ENABLE_SD_20 define from srchi.h
02/11/03   bt      Remove FEATURE_PAGE_MATCH_REQUEST
02/10/03   bt      Make FEATURE_PAGE_MATCH_REQUEST dependant on FEATURE_ZZ2.
01/03/03   bt      Added FEATURE_PAGE_MATCH_REQUEST.
12/19/02   ejv     Removed old features.
12/11/02   gs      Included RF_WARMUP_BASE_TIME_MS, soon to be broken
                   out into the proper RF, CLK, and SW latency components
10/09/02   rm      Added FEATURE_SEARCH4, FEATURE_SEARCH4_ACQ, and
                   FEATURE_SIX_FINGERS
08/05/02   gs      Added FEATURE_MDSP_TASK
05/01/02   jct     Created
===========================================================================*/

/* This include is needed to figure out whether
   FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING is defined */
#include "custdmss.h"

/* This include is needed to figure out whether
   FEATURE_CDMA_RX_DIVERSITY is defined */
#include "custcdma.h"

/* This include is needed to figure out whether
   FEATURE_MULTI_RX_CHAIN is defined */
#include "custcdma2000.h"

/* Include the correct GPS header based on target 
   to figure out which GPS features are defined */
#if ( defined( T_QSC1100 ) || defined( T_QSC1110 ) )
  /* No file to include, target does not support GPS */
#elif ( defined( T_QSC60X5 ) || defined( T_MSM8650 ) )
  /* This include file for Aries and beyond (SC2x, 8650) */
  #include "custcgps.h"
#elif ( defined( T_MSM6800 ) )
  /* Include file for GPSONE targets (MSM6800) */
  #include "custgps.h"
#else
  #error Target specification unknown
#endif


/* ------------------------------------------------------------------------
** Target Verification
** ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
** This file contains srch feature configuration compatible only with the 
** following targets:
**   - QSC11x0
**
** All other targets may require modifications to this file.
** ------------------------------------------------------------------------ */

/* Make sure this file is only being included by the correct targets */
#if ( !defined( T_QSC1100 ) && !defined( T_QSC1110 ) )
  /* ULC specific custsrch.h included by incompatible target */
  #error SRCH attempted to include incompatible custsrch.h
#endif /* QSC1100 || QSC1110 */


/* ------------------------------------------------------------------------
** General SRCH Features
** ------------------------------------------------------------------------ */

/* Antenna Configuration */
#ifdef FEATURE_MULTI_RX_CHAIN
  #undef FEATURE_SRCH_SINGLE_ANTENNA
#else
  /* Configure 1X for single antenna operation */
  #define FEATURE_SRCH_SINGLE_ANTENNA
#endif /* FEATURE_MULTI_RX_CHAIN */

/* Memory Reduction */
#ifdef FEATURE_LOW_MEMORY_USAGE
    /* Configure 1X for reduced memory targets */
    #define FEATURE_SRCH_REDUCED_MEMORY
#else /* !FEATURE_LOW_MEMORY_USAGE */
  #undef FEATURE_SRCH_REDUCED_MEMORY
#endif /* FEATURE_LOW_MEMORY_USAGE */

/* GPS and 1X Interaction */
#if ( defined( FEATURE_GPSONE ) || defined( FEATURE_SRCH_CGPS_IF ) )
  #undef FEATURE_SRCH_HAS_NO_GPS
#else /* !FEATURE_GPSONE && !FEATURE_SRCH_CGPS_IF */
  /* Configure 1X for targets without GPS */
  #define FEATURE_SRCH_HAS_NO_GPS
#endif /* FEATURE_GPSONE || FEATURE_SRCH_CGPS_IF */

/* Enable Acq 2.0 */
#define FEATURE_ACQ2

/* Allow masking of the searcher instead of slewing to absolute position. */
#define FEATURE_RELATIVE_DMOD_POS


/* ------------------------------------------------------------------------
** Pilot Filter Gain and Trk_lo Control Features
** ------------------------------------------------------------------------ */

/* Control the polarity of the TRK_LO_ADJUST pin */
#define FEATURE_INVERTED_TRK_LO_POLARITY

/* Configure slew rate limiting for XO targets */
#ifndef FEATURE_XO
  /* Enable VCTCXO Slew Rate Limiting */
  #define FEATURE_1X_SRCH_AFC_SRL
#else /* !FEATURE_XO */
  #undef FEATURE_1X_SRCH_AFC_SRL
#endif /* FEATURE_XO */

/* The next "feature" is temporary as other subsystems integrate with
   these changes. This should be mainlined fairly soon */

/* Moves control over the pilot filter gain control register from the RF
   drivers into search. Replaces tuning calls to RF with calls to
   srch_tune_rfm() */
#define SRCH_HAS_PILOT_GAIN_CNTL


/* ------------------------------------------------------------------------
** Standby Enhancement Features
** ------------------------------------------------------------------------ */

/* Page Match enabling. When defining this feature SRCH can request a
   page match after waking up instead of always getting one. */
#define FEATURE_PAGE_MATCH_REQUEST


/* ------------------------------------------------------------------------
** Sleep Features
** ------------------------------------------------------------------------ */

/* Enable keypad sleep mask */
#define FEATURE_KEYPAD_OKTS

/* Enable use of Low Frequency Sleep Oscillator */
#define FEATURE_SLEEP_SLOW_CLOCK
#ifdef FEATURE_SLEEP_SLOW_CLOCK
  /* Enable VCTCXO shutdown, TCXO/SLEEP uP clock switching
     (SURFs with MSM3100 ASICs) */
  #define FEATURE_UP_CLOCK_SWITCHING
#endif /* FEATURE_SLEEP_SLOW_CLOCK */

/* Define this if wakeups should not utilize higher clock speeds. Warning,
   this will probably crash the timelines unless they get expanded */
#undef FEATURE_WAKEUP_TCXO_CLK

/* Allow QPCH operation in hybrid mode */
#define FEATURE_QPCH_HYBRID

/* Use dynamic QPCH thresholding */
#define FEATURE_QPCH_DYNAMIC_THRESHOLD

/* Configure sleep task to take care of TCXO sleep */
#define FEATURE_TCXO_WARMUP_IN_SLEEP_TASK

/* Eliminate the dependency of the RX front on RTC */
#define FEATURE_SRCH_RF_HAS_NO_RTC_DEP

/* Use ZZ2_1 */
#define FEATURE_ZZ2_1

/* Enable new Inactive state for MSM6500 5.0 and onwards */
#define FEATURE_SRCH_INACTIVE_STATE

/* Configure clock behavior during inactive state */
#ifndef FEATURE_SRCH_CLKREGIM_RM
  /* Until clock voting in clock regime and HDR is enabled, we must leave the
     clocks on in Inactive State */
  #define FEATURE_SRCH_NA_CLOCKS_REMAIN_ON
#else /* !FEATURE_SRCH_CLKREGIM_RM */
  #undef FEATURE_SRCH_NA_CLOCKS_REMAIN_ON
#endif /* FEATURE_SRCH_CLKREGIM_RM */

/* Enable dynamic assignment of power strobe gpio */
#undef FEATURE_SRCH_PWR_STROBE_DYN_GPIO__STRIP /* Undefine for ULC */

/* This is a temporary feature defined to monitor SRCH idle state interactions
   with sleepctl */
#undef FEATURE_SRCHZZ_TLC_SLEEPCTL_DEBUG /* Undefine for ULC */


/* ------------------------------------------------------------------------
** QLIC Features
** ------------------------------------------------------------------------ */

  /* This feature indicates that the MDSP Firmware supports QLIC
     this will route finger assignments through the MDSP FW */
  #define FEATURE_SRCH_SUPPORTS_QLIC

  /* This feature turns on the use of QLIC and it's algorithms */
  #define FEATURE_SRCH_QLIC


/* ------------------------------------------------------------------------
** Single and Multi Antenna Features
** ------------------------------------------------------------------------ */

/* With this feature, 1X will only give up it's DEMOD chain for
   immediate unlock request */
#define FEATURE_SRCH_UNLOCK_ONLY_IMMEDIATE


/* Configure 1X for single antenna operation */
#ifndef FEATURE_SRCH_SINGLE_ANTENNA

    /* Allow use of rx chain 1
       For example, config for 1x/HDR simultaneous, set the following NV items:
         Search Debug Mask = 2
         TRM Configuration = 2 (NV_TRM_RF_DUAL_RX) and 2 (mask) */
    #define FEATURE_SRCH_RX1_RELA_DEMOD

#else /* !FEATURE_SRCH_SINGLE_ANTENNA */
  /* Disable RX Diversity */
  #undef FEATURE_CDMA_RX_DIVERSITY

  /* Disallow use of rx chain 1 */
  #undef FEATURE_SRCH_RX1_RELA_DEMOD

#endif /* FEATURE_SRCH_SINGLE_ANTENNA */


/* Configure 1X for targets without GPS */
#ifdef FEATURE_SRCH_HAS_NO_GPS
  /* Remove gps rotator interaction and vctcxo from afc_sm */
  #define FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL

  /* Remove the TCG state machine */
  #define FEATURE_SRCH_HAS_NO_TCG

  /* Remove search internal resource manager */
  #define FEATURE_SRCH_HAS_NO_IRM

  /* Removes the restricted timeline */
  #define FEATURE_SRCH_HAS_NO_RTL

  /* Remove the transceiver resource manager */
  #define FEATURE_SRCH_HAS_NO_TRM

#else /* !FEATURE_SRCH_HAS_NO_GPS */

  /* Do not remove gps rotator interaction and vctcxo from afc_sm */
  #undef FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL

  /* Do not remove the TCG state machine */
  #undef FEATURE_SRCH_HAS_NO_TCG

  /* Do not remove search internal resource manager */
  #undef FEATURE_SRCH_HAS_NO_IRM

  /* Do not remove the restricted timeline */
  #undef FEATURE_SRCH_HAS_NO_RTL

  /* Do not remove the transceiver resource manager */
  #undef FEATURE_SRCH_HAS_NO_TRM

#endif /* FEATURE_SRCH_HAS_NO_GPS */


/* Configure 1X when using transceiver resource manager */
#ifndef FEATURE_SRCH_HAS_NO_TRM
  /* Enable FTS */
  #define FEATURE_SRCH_IS_FTS_CAPABLE

  /* Enable FTS-3D4D operation */
  #define FEATURE_SRCH_FTS_BAND_CONFIGURABILITY

  /* Enable SOOS - switching out of simultaneous */
  #define FEATURE_SOOS

#else /* !FEATURE_SRCH_HAS_NO_TRM */
  /* Disable FTS */
  #undef FEATURE_SRCH_IS_FTS_CAPABLE

  /* Disable FTS-3D4D operation */
  #undef FEATURE_SRCH_FTS_BAND_CONFIGURABILITY

  /* Disable SOOS - switching out of simultaneous */
  #undef FEATURE_SOOS

#endif /* FEATURE_SRCH_HAS_NO_TRM */


/* Enable RX Diversity on targets with multiple antennas */
#ifdef FEATURE_CDMA_RX_DIVERSITY
  /* Turn on RDDS */
  #define FEATURE_SRCH_RXDIV_AUTOSWITCH

  /* Turn on Band Class mask switching */
  #define FEATURE_SRCH_RXDIV_BC_SWITCHING

#else /* !FEATURE_CDMA_RX_DIVERSITY */
  /* Turn off RDDS */
  #undef FEATURE_SRCH_RXDIV_AUTOSWITCH

  /* Turn off Band Class mask switching */
  #undef FEATURE_SRCH_RXDIV_BC_SWITCHING

#endif /* FEATURE_CDMA_RX_DIVERSITY */


/* ------------------------------------------------------------------------
** Memory Reduction Features
** ------------------------------------------------------------------------ */

/* Configure 1X for reduced memory targets */
#ifdef FEATURE_SRCH_REDUCED_MEMORY

  /* Remove all messages */
  #define MSG_BUILD_MASK_MSG_SSID_1X_SRCH    0x00000010

  /* Enable "lite" messages, removing superfluous messages */
  #define FEATURE_SRCH_MSG_LITE

  /* Remove the internal log packets (0x11A7 - 0x11AF) */
  #define FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS

#else /* !FEATURE_SRCH_REDUCED_MEMORY */

  /* Include messages */
  #define MSG_BUILD_MASK_MSG_SSID_1X_SRCH    0x0F7F07FE

  /* Enable System Measurement */
  #define FEATURE_SYS_MEAS

  /* Turn on timeline profiling */
  #define FEATURE_TL_PROF

#endif /* FEATURE_SRCH_REDUCED_MEMORY */


/* ------------------------------------------------------------------------
** RF and AFC Features
** ------------------------------------------------------------------------ */

/* Different multiplier settings for VCO Gain and Slew Rate Limits*/
#define AFC_VCO_GAIN_SRL_NORMAL 1 /* VCO gain and SRL values at norm settings */
#define AFC_VCO_GAIN_X5_SRL_X2  2 /* (VCO gain values x5) and (SRL x2)  */
#define AFC_VCO_GAIN_X10_SRL_X3 3 /* (VCO gain values x10) and (SRL x3) */


/* ------------------------------------------------------------------------
** Wakeup, Warmup, and Overhead Constants
** ------------------------------------------------------------------------ */

/* RF base warmup time */
/* If FEATURE_RF_SUPPORTS_1X_WARM_UP_TIME_API is defined, changing the
   RF warmup times has no effect */
    #ifdef BOOT_MODE_NAND
      #define RF_WARMUP_BASE_TIME_MS            10
    #else /* !BOOT_MODE_NAND */
      #define RF_WARMUP_BASE_TIME_MS            8
    #endif /* BOOT_MODE_NAND */

  /* Cx8 warmup time */
  #define RF_WARMUP_CX8_ON_TIME_MS              0

/* This is a value for the SW latency the srch task
   experiences at the arrival of the wakeup interrupt due
   to interrupt latency and task switching */
#ifdef BOOT_MODE_NAND
  #define WARMUP_SW_LATENCY_SAFEWIN_MS          4
#else /* !BOOT_MODE_NAND */
    #define WARMUP_SW_LATENCY_SAFEWIN_MS        5
#endif /* BOOT_MODE_NAND */

/* Overhead for RTR6500 type cards, the overhead is
   scaled to account for memory type 
   Note: The following values are used inside the library and not
   accessible through srch_lib_ext.[c/h] */
  #ifdef BOOT_MODE_NAND
    #define WARMUP_RTR6500_OVERHEAD_MS          4
  #else /* !BOOT_MODE_NAND */
    #define WARMUP_RTR6500_OVERHEAD_MS          5
  #endif  /* BOOT_MODE_NAND */

/* TCXO overhead times */
#define TCXO_CLOCK_SWITCH_OVERHEAD_MS           3
#define TCXO_WARMUP_MS                          3
#define CLK_OVERHEAD_MS                     (5 - WARMUP_SW_LATENCY_SAFEWIN_MS)
#define MIN_SLEEP_SETUP_TIME_SAFEWIN_MS         2

/* CGPS overhead time */
  #define CGPS_SW_OVERHEAD_MS                   0

/* Times in micro seconds */
#define WARMUP_RTR6500_OVERHEAD_US    (WARMUP_RTR6500_OVERHEAD_MS * 1000)
#define WARMUP_SW_LATENCY_SAFEWIN_US  (WARMUP_SW_LATENCY_SAFEWIN_MS * 1000)
#define RF_WARMUP_BASE_TIME_US        (RF_WARMUP_BASE_TIME_MS * 1000)
#define RF_WARMUP_CX8_ON_TIME_US      (RF_WARMUP_CX8_ON_TIME_MS * 1000)

#define TCXO_CLOCK_SWITCH_OVERHEAD_US (TCXO_CLOCK_SWITCH_OVERHEAD_MS * 1000)
#define TCXO_WARMUP_US                (TCXO_WARMUP_MS * 1000)
#define CLK_OVERHEAD_US               (CLK_OVERHEAD_MS * 1000)
#define CGPS_SW_OVERHEAD_US           (CGPS_SW_OVERHEAD_MS * 1000)



#endif /* CUSTSRCH_H */
