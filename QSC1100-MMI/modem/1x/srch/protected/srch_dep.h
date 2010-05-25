#ifndef SRCH_DEP_H
#define SRCH_DEP_H
/*===========================================================================

DESCRIPTION
  This file contains the internal feature dependencies for searcher.  The
  purpose of this file is to aid in development by providing a method of
  documenting and validating feature dependencies.  This file should be 
  updated every time a new feature is added or searcher internal feature 
  inter-dependencies change.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2008
                by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/protected/srch_dep.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
12/15/08   aps     Added deps for FEATURE_SRCH_QLIC_OLD_PC_TIMELINE
09/30/08   adw     FEATURE_SRCH_REDUCED_MEMORY must only be defined for ULC.
09/09/08   adw     Modified feature checks to support 8650 dependencies.
09/08/08   adw     Modified FEATURE_SRCHZZ_TLC_SLEEPCTL_DEBUG dependency check.
08/28/08   adw     Initial revision.
=============================================================================*/


/*===========================================================================

                        SEARCHER FEATURE REQUIREMENTS

===========================================================================*/

/* ------------------------------------------------------------------------
** General SRCH Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check Antenna Configuration */
#ifdef FEATURE_MULTI_RX_CHAIN
  /* 1X should be configured for multi-antenna operation */
  #ifdef FEATURE_SRCH_SINGLE_ANTENNA
    #error FEATURE_SRCH_SINGLE_ANTENNA not supported w/ FEATURE_MULTI_RX_CHAIN
  #endif /* FEATURE_SRCH_SINGLE_ANTENNA */
#else /* !FEATURE_MULTI_RX_CHAIN */

  /* 1X should be configured for single antenna operation */
  #ifndef FEATURE_SRCH_SINGLE_ANTENNA
    #error FEATURE_SRCH_SINGLE_ANGENNA required ifndef FEATURE_MULTI_RX_CHAIN
  #endif /* FEATURE_SRCH_SINGLE_ANTENNA */

#endif /* FEATURE_MULTI_RX_CHAIN */

/* Check Memory Reduction */
#ifdef FEATURE_LOW_MEMORY_USAGE
  #if ( defined( T_QSC1100 ) || defined( T_QSC1110 ) )
    /* 1X should be configured for reduced memory targets */
    #ifndef FEATURE_SRCH_REDUCED_MEMORY
      #error FEATURE_SRCH_REDUCED_MEMORY required for FEATURE_LOW_MEMORY_USAGE
    #endif /* FEATURE_SRCH_REDUCED_MEMORY */
  #endif /* ULC */
#endif /* FEATURE_LOW_MEMORY_USAGE */

/* Check GPS and 1X Interaction */
#ifdef FEATURE_GPSONE
#error code not present
#else /* !FEATURE_GPSONE */

  /* 1X should be configured for targets without GPS */
  #ifndef FEATURE_SRCH_HAS_NO_GPS
    #error FEATURE_SRCH_HAS_NO_GPS required ifndef FEATURE_CGPS_USES_CDMA
  #endif /* FEATURE_SRCH_HAS_NO_GPS */
#endif /* FEATURE_GPSONE */

/* Check Search Message Debug Mask */
/* MSG_BUILD_MASK_MSG_SSID_1X_SRCH     No feature related dependencies */

/* Check Enable Acq 2.0 */
/* FEATURE_ACQ2                        No feature related dependencies */

/* Check allow masking of searcher instead of slewing to absolute position */
/* FEATURE_RELATIVE_DMOD_POS           No feature related dependencies */


/* ------------------------------------------------------------------------
** Pilot Filter Gain and Trk_lo Control Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check Control the polarity of the TRK_LO_ADJUST pin */
/* FEATURE_INVERTED_TRK_LO_POLARITY    No feature related dependencies */

/* Check Configuration for XO Only Operation */
#ifdef FEATURE_XO
  /* VCTCXO Slew Rate Limiting should be disabled */
  #ifdef FEATURE_1X_SRCH_AFC_SRL
    #error FEATURE_1X_SRCH_AFC_SRL not supported w/ FEATURE_XO
  #endif /* FEATURE_1X_SRCH_AFC_SRL */
#else /* !FEATURE_XO */

  /* VCTCXO Slew Rate Limiting should be enabled */
  #ifndef FEATURE_1X_SRCH_AFC_SRL
    #error FEATURE_1X_SRCH_AFC_SRL required ifndef FEATURE_XO
  #endif /* FEATURE_1X_SRCH_AFC_SRL */
#endif /* FEATURE_XO */

/* Check control over pilot filter gain control register */
/* SRCH_HAS_PILOT_GAIN_CNTL            No feature related dependencies */

/* ------------------------------------------------------------------------
** Standby Enhancement Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check Page match enabling */
/* FEATURE_PAGE_MATCH_REQUEST          No feature related dependencies */

/* ------------------------------------------------------------------------
** Sleep Related Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check enable keypad sleep mask */
/* FEATURE_KEYPAD_OKTS                 No feature related dependencies */

/* Check Use of Low Frequency Sleep Oscillator */
#ifdef FEATURE_SLEEP_SLOW_CLOCK
  /* VCTCXO shutdown, TCXO/SLEEP uP clock switching should be enabled */
  #ifndef FEATURE_UP_CLOCK_SWITCHING
    #error FEATURE_UP_CLOCK_SWITCHING required for FEATURE_SLEEP_SLOW_CLOCK
  #endif /* FEATURE_UP_CLOCK_SWITCHING */
#endif /* FEATURE_SLEEP_SLOW_CLOCK */

/* Check wakeups utilize higher clock speeds */
/* FEATURE_WAKEUP_TCXO_CLK             No feature related dependencies */

/* Check allow QPCH operation in hybrid mode */
/* FEATURE_QPCH_HYBRID                 No feature related dependencies */

/* Check use dynamic QPCH thresholding */
/* FEATURE_QPCH_DYNAMIC_THRESHOLD      No feature related dependencies */ 

/* Check configure sleep task to take care of TCXO sleep */
/* FEATURE_TCXO_WARMUP_IN_SLEEP_TASK   No feature related dependencies */

/* Check eliminate the dependency of the RX front on RTC */
/* FEATURE_SRCH_RF_HAS_NO_RTC_DEP      No feature related dependencies */

/* Check Use ZZ2 */
#ifdef FEATURE_ZZ2_1
  /* 1X should use ZZ2_1 */
  #ifndef FEATURE_SRCH_RF_HAS_NO_RTC_DEP
    #error FEATURE_SRCH_RF_HAS_NO_RTC_DEP required for FEATURE_ZZ2_1
  #endif /* ! FEATURE_SRCH_RF_HAS_NO_RTC_DEP */
#endif /* FEATURE_ZZ2_1 */

/* Check enable new Inactive state for MSM6500 5.0 and onwards */
/* FEATURE_SRCH_INACTIVE_STATE         No feature related dependencies */

/* Check monitor SRCH idle state interactions with sleepctl */
/* FEATURE_SRCHZZ_TLC_SLEEPCTL_DEBUG   No feature related dependencies */


/* ------------------------------------------------------------------------
** RF and AFC Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check multiplier settings for VCO Gain and Slew Rate Limits */
/* AFC_VCO_GAIN_SRL_NORMAL         No feature related dependencies */
/* AFC_VCO_GAIN_X5_SRL_X2          No feature related dependencies */
/* AFC_VCO_GAIN_X10_SRL_X3         No feature related dependencies */    


/* ------------------------------------------------------------------------
** QLIC Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check QLIC supported by MDSP firmware and turned on */
#if ( defined( FEATURE_SRCH_QLIC ) && !defined( FEATURE_SRCH_SUPPORTS_QLIC ) )
  /* QLIC should be supported and turned on */
  #error FEATURE_SRCH_SUPPORTS_QLIC required for FEATURE_SRCH_QLIC
#endif

#if ( defined( FEATURE_SRCH_QLIC_OLD_PC_TIMELINE ) && \
      !defined( FEATURE_SRCH_SUPPORTS_QLIC ) )
#error code not present
#endif

#if ( defined ( FEATURE_SRCH_QLIC_OLD_PC_TIMELINE ) && \
      !defined( T_MSM8650B ) )
#error code not present
#endif


/* ------------------------------------------------------------------------
** Single and Multi Antenna Related Feature Dependencies
** ------------------------------------------------------------------------ */

/* Check 1X demod chain unlock */
/* FEATURE_SRCH_UNLOCK_ONLY_IMMEDIATE  No feature related dependencies */


/* Check configuration of 1X for single antenna operation */
#ifdef FEATURE_SRCH_SINGLE_ANTENNA
  /* 1X should not use diversity when configured for single antenna */
  #ifdef FEATURE_CDMA_RX_DIVERSITY
    #error FEATURE_CDMA_RX_DIVERSITY not supported w/                      \
           FEATURE_SRCH_SINGLE_ANTENNA
  #endif /* FEATURE_CDMA_RX_DIVERSITY */

  /* 1X should not us RX1 when configured for single antenna */
  #ifdef FEATURE_SRCH_RX1_RELA_DEMOD
    #error FEATURE_SRCH_RX1_RELA_DEMOD not supported w/                    \
           FEATURE_SRCH_SINGLE_ANTENNA
  #endif /* FEATURE_SRCH_RX1_RELA_DEMOD */

#endif /* FEATURE_SRCH_SINGLE_ANTENNA */


/* Check configuration of 1X for targets without GPS */
#ifdef FEATURE_SRCH_HAS_NO_GPS
  /* 1X rotator interaction with GPS should be disabled */
  #ifndef FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL
    #error FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL needed for                   \
           FEATURE_SRCH_HAS_NO_GPS
  #endif /* FEATURE_SRCH_ROT_ONLY_1X_FULL_CTRL */

  /* TCG state machine should be disabled */
  #ifndef FEATURE_SRCH_HAS_NO_TCG
    #error FEATURE_SRCH_HAS_NO_TCG needed for                              \
           FEATURE_SRCH_HAS_NO_GPS
  #endif /* FEATURE_SRCH_HAS_NO_TCG */

  /* Internal resource manager should be disabled */
  #ifndef FEATURE_SRCH_HAS_NO_IRM
    #error FEATURE_SRCH_HAS_NO_IRM needed for                              \
           FEATURE_SRCH_HAS_NO_GPS
  #endif /* FEATURE_SRCH_HAS_NO_IRM */

  /* Restricted timeline should be disabled */
  #ifndef FEATURE_SRCH_HAS_NO_RTL
    #error FEATURE_SRCH_HAS_NO_RTL needed for                              \
           FEATURE_SRCH_HAS_NO_GPS
  #endif /* FEATURE_SRCH_HAS_NO_RTL */

  /* Transceiver resource manager should be disabled */
  #ifndef FEATURE_SRCH_HAS_NO_TRM
    #error FEATURE_SRCH_HAS_NO_TRM needed for                              \
           FEATURE_SRCH_HAS_NO_GPS
  #endif /* FEATURE_SRCH_HAS_NO_TRM */

  /* Check configuration for targets without TRM */
  #ifdef FEATURE_SRCH_HAS_NO_TRM
    /* FTS should be disabled */
    #ifdef FEATURE_SRCH_IS_FTS_CAPABLE
      #error FEATURE_SRCH_IS_FTS_CAPABLE not supported w/                  \
      FEATURE_SRCH_HAS_NO_TRM
    #endif /* FEATURE_SRCH_IS_FTS_CAPABLE */

    /* Switch out of SHDR should be disabled */
    #ifdef FEATURE_SOOS
      #error FEATURE_SOOS not supported w/                                 \
             FEATURE_SRCH_HAS_NO_TRM
    #endif /* FEATURE_SOOS */
  #endif /* FEATURE_SRCH_HAS_NO_TRM */

#endif /* FEATURE_SRCH_HAS_NO_GPS */


/* Check enable RX Diversity on targets with multiple antennas */
#if ( !defined( T_MSM8650 ) || !defined( T_MSM7800 ) )
  #ifdef FEATURE_CDMA_RX_DIVERSITY
    /* RX diversity autoswitch should be enabled */
    #ifndef FEATURE_SRCH_RXDIV_AUTOSWITCH
      #error FEATURE_SRCH_RXDIV_AUTOSWITCH required for                    \
             FEATURE_CDMA_RX_DIVERSITY
    #endif /* FEATURE_SRCH_RXDIV_AUTOSWITCH */

    /* RX diversity Band Class mask switching should be turned on */
    #ifndef FEATURE_SRCH_RXDIV_BC_SWITCHING
      #error FEATURE_SRCH_RXDIV_BC_SWITCHING required for                  \
             FEATURE_CDMA_RX_DIVERSITY
    #endif /* FEATURE_SRCH_RXDIV_BC_SWITCHING */

  #endif /* FEATURE_CDMA_RX_DIVERSITY */
#endif /* !T_MSM8650 */


/* ------------------------------------------------------------------------
** Memory Reduction Feature Dependencies
** ------------------------------------------------------------------------ */

/* Configure 1X for reduced memory targets */
#ifdef FEATURE_SRCH_REDUCED_MEMORY
  /* Timeline profiling should be disabled */
  #ifdef FEATURE_TL_PROF
    #error FEATURE_TL_PROF not supported w/ FEATURE_SRCH_REDUCED_MEMORY
  #endif /* FEATURE_TL_PROF */

  /* System measurement should be disabled */
  #ifdef FEATURE_SYS_MEAS
    #error FEATURE_SYS_MEAS not supported w/ FEATURE_SRCH_REDUCED_MEMORY
  #endif /* FEATURE_SYS_MEAS */

  /* Lite messages should be enabled */
  #ifndef FEATURE_SRCH_MSG_LITE
    #error FEATURE_SRCH_MSG_LITE needed for                                \
           FEATURE_SRCH_REDUCED_MEMORY
  #endif /* FEATURE_SRCH_MSG_LITE */

  /* Internal log packets should be removed */
  #ifndef FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS
    #error FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS needed for             \
           FEATURE_SRCH_REDUCED_MEMORY
  #endif /* FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS */

#endif /* FEATURE_SRCH_REDUCED_MEMORY */


/* ------------------------------------------------------------------------
** Wakeup, Warmup, and Overhead Constant Dependencies
** ------------------------------------------------------------------------ */

/* Check wakeup, warmup, and overhead constants */
/* RF_WARMUP_BASE_TIME_MS              No feature related dependencies */
/* RF_WARMUP_CX8_ON_TIME_MS            No feature related dependencies */
/* WARMUP_SW_LATENCY_SAFEWIN_MS        No feature related dependencies */
/* WARMUP_RTR6500_OVERHEAD_MS          No feature related dependencies */
/* TCXO_CLOCK_SWITCH_OVERHEAD_MS       No feature related dependencies */
/* TCXO_WARMUP_MS                      No feature related dependencies */
/* CGPS_SW_OVERHEAD_MS                 No feature related dependencies */

/* Check times in micro seconds */
/* WARMUP_RTR6500_OVERHEAD_US          No feature related dependencies */
/* WARMUP_SW_LATENCY_SAFEWIN_US        No feature related dependencies */
/* RF_WARMUP_BASE_TIME_US              No feature related dependencies */
/* RF_WARMUP_CX8_ON_TIME_US            No feature related dependencies */
/* TCXO_CLOCK_SWITCH_OVERHEAD_US       No feature related dependencies */
/* TCXO_WARMUP_US                      No feature related dependencies */
/* CLK_OVERHEAD_US                     No feature related dependencies */
/* CGPS_SW_OVERHEAD_US                 No feature related dependencies */



#endif /* SRCH_DEP_H */

