#ifndef SLEEPMODI_H
#define SLEEPMODI_H
/*===========================================================================

              S L E E P M O D    C O M M O N    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations for use by the Sleep Modem task.
  This module is used in the portable only.

                             Copyright (c) 2008-2009 
                    by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleepmodi.h#3 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/09   taw     Define SLEEPMOD_MAOINT_GET_PENDING_INTS() to 0 for targets
                   that don't have MPM.
03/05/09   taw     Added a macro for maoint_get_pending_ints().
09/16/08   clm     1H08 add support for QSC6270, plus code clean up.
09/14/08   cab     Remove clki.h include
08/18/08   taw     Added defitions for TCXO_TURN_ON_SCLK and TCXO_TURN_ON_US
                   when FEATURE_TCXO_WARMUP_IN_SLEEP_TASK is not defined.
06/27/08   clm     1H08 sleep re-work.  Initial version.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "rex.h"
#include "clkregim.h"
#include "sleeplog.h"
#include "log.h"
#include "clk.h"
#include "sleep.h"
#include "sleepi.h"

/*===========================================================================

                          PREPROCESSOR DEFINITIONS AND CONSTANTS

===========================================================================*/

/*--------------------------------------------------------------------------
  Sleep constants
--------------------------------------------------------------------------*/
/* The amount of time that must be left in the sleep controller
   (in milliseconds) to allow switching of the uP clock to the
   sleep clock and shutting down the uP, if the MSM is currently
   sleeping */
#if defined(T_QSC6270)
  #define SLEEP_CHECK_TIME_MS   10
#else /* All other targets */
  #define SLEEP_CHECK_TIME_MS   30
#endif /* All other targets */

#if defined(FEATURE_USE_MPM_TCXO_CONTROL) || \
    defined(FEATURE_USE_MPM_VDD_MIN_CONTROL)
  #define SLEEPMOD_HAS_MPM_HW
#endif /* FEATURE_USE_MPM_TCXO_CONTROL || FEATURE_USE_MPM_VDD_MIN_CONTROL */

/* Amount of time, converted to sleep clocks */
#define SLEEP_CHECK_TIME_SCLK   TIMETICK_SCLK_FROM_MS( SLEEP_CHECK_TIME_MS )

#ifdef FEATURE_TCXO_WARMUP_IN_SLEEP_TASK

  #ifdef SLEEPMOD_HAS_MPM_HW
    #define TCXO_TURN_ON_US         0
  #else /* !SLEEPMOD_HAS_MPM_HW */
    #ifdef T_MSM7500
#error code not present
    #else /* !T_MSM7500 */
      #define TCXO_TURN_ON_US       CLKRGM_TCXO_WARMUP_AND_SWITCH_OVERHEAD_US
    #endif /* !T_MSM7500 */
  #endif /* !SLEEPMOD_HAS_MPM_HW */

  /* Amount of time to wakeup early by, if TCXO is turned off, to allow for
    TCXO to be turned back on, and timers & clock callbacks to expire. */
  #define TCXO_TURN_ON_SCLK       TIMETICK_SCLK_FROM_US( TCXO_TURN_ON_US )

#else /* !FEATURE_TCXO_WARMUP_IN_SLEEP_TASK */

  #define TCXO_TURN_ON_SCLK         0
  #define TCXO_TURN_ON_US           0

#endif /* !FEATURE_TCXO_WARMUP_IN_SLEEP_TASK */

/* SLEEPMOD_INTLOCK() */
#ifdef FEATURE_L4
#error code not present
#else /* !FEATURE_L4 */
  #define SLEEPMOD_INTLOCK()  INTLOCK()
  #define SLEEPMOD_INTFREE()  INTFREE()
#endif /* !FEATURE_L4 */

/* RF_CDMA_TEMP_COMP_INIT(x), RF_CDMA_DO_TEMP_COMP() */
#ifdef FEATURE_CDMA1X
  #ifdef FEATURE_MULTIMODE_RFAPI
    #ifdef FEATURE_MULTI_RX_CHAIN
      #define RF_CDMA_TEMP_COMP_INIT(x) \
                             rfm_cdma_temp_comp_init( RFCOM_TRANSCEIVER_0, &x )
      #define RF_CDMA_DO_TEMP_COMP() \
                                if (!rfm_is_sleeping( RFCOM_TRANSCEIVER_0 ) ) \
                                {rfm_cdma_do_temp_comp( RFCOM_TRANSCEIVER_0 );}
    #else /* !FEATURE_MULTI_RX_CHAIN */
      #define RF_CDMA_TEMP_COMP_INIT(x) rfm_cdma_temp_comp_init( &x )
      #define RF_CDMA_DO_TEMP_COMP() rfm_cdma_do_temp_comp()
    #endif /* !FEATURE_MULTI_RX_CHAIN */
  #else /* !FEATURE_MULTIMODE_RFAPI */
    #define RF_CDMA_TEMP_COMP_INIT(x) rf_cdma_temp_comp_init( &x )
    #define RF_CDMA_DO_TEMP_COMP() rf_cdma_do_temp_comp()
  #endif /* !FEATURE_MULTIMODE_RFAPI */
#else /* !FEATURE_CDMA1X */
  #define RF_CDMA_TEMP_COMP_INIT(x)
  #define RF_CDMA_DO_TEMP_COMP() 
#endif /* !FEATURE_CDMA1X) */

/* SLEEPMOD_HDR_PWR_PROFILE_STRB_TOGGLE(), 
   SLEEPMOD_HDR_PWR_PROFILE_STRB_ENABLE() */
#if defined( FEATURE_HDRSRCH_POWER_PROFILE ) && !defined(T_MSM7500)
  #define SLEEPMOD_HDR_PWR_PROFILE_STRB_TOGGLE() \
                      HDR_PWR_PROFILE_STRB_TOGGLE( HDRSRCH_PWR_EVENT_TCXO_OFF )
  #define SLEEPMOD_HDR_PWR_PROFILE_STRB_ENABLE() \
                       HDR_PWR_PROFILE_STRB_ENABLE( HDRSRCH_PWR_EVENT_TCXO_ON )
#else /* !FEATURE_HDRSRCH_POWER_PROFILE || T_MSM7500 */
  #define SLEEPMOD_HDR_PWR_PROFILE_STRB_TOGGLE()
  #define SLEEPMOD_HDR_PWR_PROFILE_STRB_ENABLE()
#endif /* !FEATURE_HDRSRCH_POWER_PROFILE || T_MSM7500 */

/* SLEEPMOD_NO_TIME_FOR_TCXO(x) */
#ifdef SLEEPMOD_HAS_MPM_HW
  #define SLEEPMOD_NO_TIME_FOR_TCXO(x)  (((x) < (SLEEP_CHECK_TIME_SCLK)) || \
                                         ((x) <= ( MAO_TCXO_WARMUP_TIME)))
#else /* !SLEEPMOD_HAS_MPM_HW */
  #define SLEEPMOD_NO_TIME_FOR_TCXO(x)  ((x) < (SLEEP_CHECK_TIME_SCLK))
#endif /* !FEATURE_USE_MPM_HW */

/* SLEEPMOD_DEM_TCXO_OFF(), SLEEPMOD_DEM_TCXO_ON() */
#if defined( DUAL_PROC_MODEM_SLEEP ) && defined( FEATURE_APPS_TCXO_SLEEP )
  #define SLEEPMOD_DEM_TCXO_OFF()  dem_tcxo_off()
  #define SLEEPMOD_DEM_TCXO_ON()   dem_tcxo_on()
#else /* !DUAL_PROC_MODEM_SLEEP || !FEATURE_APPS_TCXO_SLEEP */
  #define SLEEPMOD_DEM_TCXO_OFF()
  #define SLEEPMOD_DEM_TCXO_ON()
#endif /* !DUAL_PROC_MODEM_SLEEP || !FEATURE_APPS_TCXO_SLEEP */

/* SLEEEPMOD_GET_VST_NEXT_WAKEUP() */
#ifdef FEATURE_L4_VST_VOTE
  #define SLEEEPMOD_GET_VST_NEXT_WAKEUP() \
                      (timetick_cvt_to_sclk(sleepl4_vst_next_wakeup(), T_USEC))
#else /* !FEATURE_L4_VST_VOTE */
  #define SLEEEPMOD_GET_VST_NEXT_WAKEUP()  (0xFFFFFFFF)
#endif /* !FEATURE_L4_VST_VOTE */

/* SLEEPMOD_L4_LINUX_VOTE_MSLEEP_OTKS(), 
   SLEEPMOD_L4_LINUX_VOTE_USLEEP_OTKS() */
#ifdef FEATURE_L4_LINUX_VOTE
#error code not present
#else /* !FEATURE_L4_LINUX_VOTE */
  #define SLEEPMOD_L4_LINUX_VOTE_MSLEEP_OTKS() (TRUE)
  #define SLEEPMOD_L4_LINUX_VOTE_USLEEP_OTKS() (TRUE)
#endif /* !FEATURE_L4_LINUX_VOTE */

/* SLEEPMOD_HW_HAS_BROKEN_MPM() */
#if defined(SLEEPMOD_HAS_MPM_HW) && defined(HW_HAS_BROKEN_MPM)
  #define SLEEPMOD_HW_HAS_BROKEN_MPM() (HW_HAS_BROKEN_MPM())
#else /* !SLEEPMOD_HAS_MPM_HW || !HW_HAS_BROKEN_MPM */
  #define SLEEPMOD_HW_HAS_BROKEN_MPM() (FALSE)
#endif /* !SLEEPMOD_HAS_MPM_HW || !HW_HAS_BROKEN_MPM */

/* SLEEPMOD_MDM_NEEDS_NO_ARM_HALT() */
#ifdef FEATURE_MDM_REQUIRE_NO_ARM_HALT
  #define SLEEPMOD_MDM_NEEDS_NO_ARM_HALT() (mdm_needs_no_arm_halt())
#else /* !FEATURE_MDM_REQUIRE_NO_ARM_HALT */
  #define SLEEPMOD_MDM_NEEDS_NO_ARM_HALT() (FALSE)
#endif /* !FEATURE_MDM_REQUIRE_NO_ARM_HALT */

/* SLEEPMOD_TIMER_SET_WAKEUP(x) */
#ifdef FEATURE_TCXO_WARMUP_IN_SLEEP_TASK
  /* Wakeup early, to allow for TCXO warmup & timer expiry */
  #define SLEEPMOD_TIMER_SET_WAKEUP(x)  (timer_set_wakeup(x))
#else /* !FEATURE_TCXO_WARMUP_IN_SLEEP_TASK */
  #define SLEEPMOD_TIMER_SET_WAKEUP(x) 
#endif /* !FEATURE_TCXO_WARMUP_IN_SLEEP_TASK */

/* SLEEPMOD_MAOINT_DISABLE_WAKEUP_INTS() */
#if defined(SLEEPMOD_HAS_MPM_HW)
  #define SLEEPMOD_MAOINT_DISABLE_WAKEUP_INTS()  (maoint_disable_wakeup_ints())
#else /* !SLEEPMOD_HAS_MPM_HW */
  #define SLEEPMOD_MAOINT_DISABLE_WAKEUP_INTS()
#endif /* !SLEEPMOD_HAS_MPM_HW */

/* SLEEPMOD_MAOINT_GET_PENDING_INTS() */
#if defined(SLEEPMOD_HAS_MPM_HW)
  #define SLEEPMOD_MAOINT_GET_PENDING_INTS()  (maoint_get_pending_ints())
#else /* !SLEEPMOD_HAS_MPM_HW */
  #define SLEEPMOD_MAOINT_GET_PENDING_INTS()  0
#endif /* !SLEEPMOD_HAS_MPM_HW */

/* SLEEPMOD_TCXO_SHTUDOWN(uMinSclk, vdd_min_restriction) */
#ifdef SLEEPMOD_HAS_MPM_HW
  #define SLEEPMOD_TCXO_SHUTDOWN(uMinSclk, vdd_min_restriction) \
    { mao_setup_sleep( (uMinSclk), (vdd_min_restriction) ); \
      clk_regime_tcxo_shutdown(); }
#else /* !SLEEPMOD_HAS_MPM_HW */
    /* Switch MCLK source to the sleep XTAL and wake up on the next
       non-masked interrupt, and switch back to the original MCLK */
    #define SLEEPMOD_TCXO_SHUTDOWN(uMinSclk, vdd_min_restriction) \
    {clk_regime_set_mclk_sleep();}
#endif /* !SLEEPMOD_HAS_MPM_HW */

/* Signals the Sleep task must clear and respond to */
#define SLEEP_TASK_SIGS      \
  (                          \
    SLEEP_STOP_SIG         | \
    SLEEP_OFFLINE_SIG      | \
    SLEEP_RF_TEMP_COMP_SIG | \
    SLEEP_RPT_TIMER_SIG      \
  )

#define SLEEP_OK_MASK          \
  (                            \
    SLEEP_DIAG_OKTS_SIG     |  \
    SLEEP_CHGT_OKTS_SIG     |  \
    SLEEP_UIM_OKTS_SIG      |  \
    SLEEP_GPSONE_OKTS_SIG   |  \
    SLEEP_MP4_OKTS_SIG      |  \
    SLEEP_GRAPHICS_OKTS_SIG |  \
    SLEEP_MC_INIT_OKTS_SIG  |  \
    SLEEP_KEEPER_OKTS_SIG      \
  )

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SLEEP_POWER_DOWN_AND_HALT

DESCRIPTION
  This procedure looks for a consensus that the CPU may be put to sleep.
  Lacking a consensus, it still halts the processor, but in "idle" mode
  in which any normal interrupt will restart processing with minimal delay.

DEPENDENCIES
  Called only from sleep task

RETURN VALUE
  None

SIDE EFFECTS
  The CPU may be put to sleep by this task.
  TCXO may be shut down
  The 5ms tick may be disabled.

===========================================================================*/

void sleep_power_down_and_halt( void );

#endif /* SLEEPMODI_H */

