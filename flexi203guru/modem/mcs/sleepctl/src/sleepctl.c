/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                       S L E E P   C O N T R O L L E R

GENERAL DESCRIPTION

  This module contains routines for the sleep controller.

EXTERNALIZED FUNCTIONS

  sleepctl_init()
    Initializes the sleep controllers

  sleepctl_arm(id)
    Arms the given sleep controller to start on the next sleep boundary

  sleepctl_start(id)
    Starts the given sleep controller immediately

  sleepctl_block_until_sleeping(id)
    Waits until the sleep controller is running before returning

  sleepctl_set_warmup(id, usec)
    Sets the warmup period, in microseconds.  The warmup period is
    subtracted from the sleep duration to determine the wakeup point.

  sleepctl_set_wakeup_callback(id, usec)
    Sets the function to call at the wakeup event.

  sleepctl_set_ending_callback(id, usec)
    Sets the function to call at the end of sleep (rtc on) event.

  sleepctl_set_duration(id, chips)
    Sets the sleep duration, in chips.

  sleepctl_set_duration_sys_clk(id, sys_clks)
    Sets the sleep duration, in sys_clk's.  (For 1x & HDR, the sys_clk's
    are chipx8's.)

  sleepctl_set_duration_ms(id, milliseconds)
    Sets the sleep duration, in msecs.

  sleepctl_set_max_duration(id)
    Sets the sleep duration to the maximum possible duration.  This may
    be used when the wakeup needs to be changed, but the wakeup point
    is not known yet.

  sleepctl_sufficient_warmup_time(id, min_warmup_sclk)
    May be called by the wakeup_cb to determine whether or not the wakeup
    event is being processed too late.

  sleepctl_complete_wakeup(id)
    May be called by the wakeup_cb to commit to the wakeup if there is
    sufficient warmup time remaining.

  sleepctl_try_wakeup(id, min_warmup_sclk)
    Combines _sufficient_warmup_time/_complete_wakeup into one step.

  sleepctl_error_feedback(id, sys_clk_error)
    Adjust the slow clock frequency estimate, based on the sleep duration
    and the reacquisition slews.

  sleepctl_wakeup_now(id)
    Forces the sleep controller to immediately terminate operation.

  sleepctl_get_cpu_conflict_adjust(id, chips, adjust_cb)
    Returns new sleep time with CPU conflict adjustment in sys_clks.

  sleepctl_is_active(id)
    Queries whether or not the sleep controller is running or not.

  sleepctl_get_sclk_cnt(id)
    Returns the number of sleep clocks the controller has been active for.

  sleepctl_get_sys_clk_cnt(id)
    Returns the number of sys_clk's the sleep controller has been active for.

  sleepctl_get_chip_cnt(id)
    Returns the number of chips the sleep controller has been active for.

  sleepctl_get_sclk_till_wakeup(id)
    Returns the number of sleep clocks until the programmed wakeup interrupt.

  sleepctl_get_sclk_till_rtc_on(id)
    Returns the number of sleep clocks until the end of sleep specified by
    sleepctl_set_duration.

  sleepctl_get_wakeup_time(id)
    Return the wakeup time as an absolute timetick counter value.

  sleepctl_get_ending_time(id)
    Return the ending time as an absolute timetick counter value.

  sleepctl_get_next_wakeup()
    Returns the time, in slow clocks, of the next sleep controller wakeup
    event.

  sleepctl_set_warmup_enforcement(id,warmup_enforced)
    Sets the warmup enforcement mode for the specified sleep controller


INITIALIZATION AND SEQUENCING REQUIREMENTS

  sleepctl_init( ) must be called once, before any other sleepctl_ function.

  Sleep is initiated by calling sleepctl_arm() or sleepctl_start().

  While sleeping, sleepctl_set_duration() may be called to set the sleep
  duration.  It may be used to change the sleep duration.  The wakeup
  interrupt occurs "warmup_usec" before the duration set by this function.

  At the wakeup interrupt, sleepctl_sufficient_warmup_time() may be used to
  determine if sufficient time exists for RF warmup, or if processing of the
  wakeup event has been delayed too long by INTLOCKs, etc.  It may be called
  multiple times, at various points in the wakeup processing.  If sufficient
  time exists, sleepctl_complete_wakeup() may be used to commit to the wakeup.
  Alternately, sleepctl_try_wakeup combines sleepctl_sufficient_warmup_time()
  and sleepctl_complete_wakeup() together as one operation.

  If insufficient warmup time exists, sleepctl_set_duration() may be used to
  lengthen the sleep duration until the next required wakeup.

  After the sleep ending interrupt, sleepctl_error_feedback() may be used to
  correct the slow clock frequency estimate.


Copyright (c) 2004, 2005, 2006, 2007, 2008 by QUALCOMM, Inc.  
All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=


==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/sleepctl/main/latest/src/sleepctl.c#8 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   -----------------------------------------------------------
10/20/2008   sg    Added RLE trace type to collapse sequntial entries
10/07/2008   rkc   Fixed compiler error in sleepctl_get_cpu_conflict_adjust()
09/18/2008   rkc   Added sleepctl_get_cpu_conflict_adjust()
09/16/2008   clm   Added QSC6270 target
04/18/2008   cab   Added QSC1100 target
04/11/2008   cab   Bump up wait time for wakeup_now to deal with clk rgm
                   delays
03/27/2008   cab   Undef slow clk regime macros (not applicable anymore)
                   Increase time busy waiting due to inaccuracy of
                   clk_busy_wait
03/03/2008   cab   Prevent race condition when doing force wakeup
02/21/2008   cab   Enable fast clock regime sooner for UMTS targets 
01/16/2008   cab   Fix clock regime resource mgr client id for hdr
10/29/2007   cab   More updates for modem restart, rename SLEEPCTL
08/22/2007   cab   Updated for modem restart functionality
07/20/2007   gfr   Added missing msm.h header file.
03/01/2007   cab   featurization update for vdd min
03/01/2007   cab   cleaned up compiler warning
02/21/2007   cab   SC2X featurization, wakeup isr change warmup not enforced
12/21/2006   cab   Moved ISR F3 after time critical code.
12/07/2006   cab   Added conflict adjustment feature to sleepctl
10/19/2006   cab   Perform enhanced edge detection for 7xxx targets
08/23/2006   cab   Elminate warnings when MSG macros not defined
07/21/2006  pp/cab Support for 1x/umts unification.
05/31/2006   cab   Protect against get_sysclk returning an overflow
12/22/2005   ejv   Add support for T_QSC60X0.
12/05/2005   cab   Added checks for interrupt latency, trace now all 32 bit
11/30/2005   cab   Change ERR to MSG_ERR in wakeup and ending isr, add counter
10/11/2005   cab   Change ERR_FATAL to ERR in sleepctl_wakeup_isr.
09/20/2005   ejv   Change ERR_FATAL to ERR in sleepctl_ending_isr.
09/16/2005   cab   Fixed asm macros, adjusted intlocks in block until sleep
08/15/2005   ajn   Delegate writing to sclk coarse registers to asm func.
08/08/2005   ajn   Removed dependency on 1x/srch/common/msm_help.h
06/13/2005   cab   Added power collapse functionality
05/25/2005   ajn   Set sleep registers to near-maximum values at start-up.
                   Added GET_ENDING_TIME trace id.
05/19/2005   ajn   Set sleepctl interrupts to edge-sensitive, if possible
05/04/2005   ajn   Corrected sclk_till_rtc_on when called after end-point.
04/20/2005   ejv   Removed old-style GSM sleep controller instance for 6700.
04/12/2005   sst   Changed sclk_from_us to timetick_cvt_to_sclk
04/05/2005   ddh   Moved sclk conversion functions to timetick
04/04/2005   ajn   Allow spurious RTC On event [INTERRUPT_CONTROL_WORKAROUND]
04/01/2005   bt    Added sleepctl_get_ending_time.
03/31/2005   bt    sleepctl_get_wakeup_time returns the wakeup point during 
                   warmup.
03/30/2005   ajn   Avoid WARMUP->SLEEP->WARMUP in force_wakeup.
03/21/2005   ajn   Added event trace type logging
03/08/2005   ejv   Added DISARM_SLEEP macro.
02/15/2005   ejv   Added FEATURE_DUAL_CLK_RGM.
                   Replaced clkregim.h with CLKRGM_H.
01/20/2005   ajn   Refactor of initialization data and constant data
01/07/2005   ajn   Added redundant reads of SLEEP_STATUS.
                   Ensure sleep controllers are not running at start up.
12/06/2004   ejv   Added T_QUASAR to avoid access to registers which do not
                   exist on QUASAR.
11/30/2004   bt    Added FEATURE_INTERRUPT_CONTROL_WORKAROUND
11/30/2004   ajn   Temporarily disabled clock regime control.
11/11/2004   ejv   Include msm_help.h for 6700.
11/10/2004   ajn   SLEEPCTL_UMTS -> SLEEPCTL_GSM
10/25/2004   ajn   Added clock_regime() control
09/23/2004   ajn   Changing RF warmup duration no longer switches the state
                   from WARMUP to SLEEP; only set_duration will.
09/13/2004   ajn   Improved protection in _update_wakeup_point.
08/16/2004   ajn   Reworked for index-based sleep control addressing.
                   Renamed chipx8 -> sys_clk, etc.
08/03/2004   ajn   Modified extern definitions to support T32 debugging.
                   Enabled HDR portion of sleepctl.
07/06/2004   ajn   Added check for unexpected wakeup interrupts
06/30/2004   ajn   Removed poll of interrupt status for MSM7500 
06/15/2004   ajn   Added _set_duration_ms( ) for deep sleep to avoid overflow
05/28/2004   jcm   Rewrite slow clock error filter
04/20/2004   ajn   Added query wakeup time as absolute timetick counter value.
03/24/2004   ajn   set_warmup() updates the wakeup point, if the SC is running
03/24/2004   ajn   Code review feedback changes
03/22/2004   ajn   Separated setting event callback from other functions
02/17/2004   ajn   First checkin
01/23/2004   ajn   Created this file.

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "sleepcfg.h"
#include "sleepctl.h"

#include "sclk.h"
#include "sclk_asm.h"
#include "clk.h"
#include "tramp.h"
#include "err.h"
#include "msg.h"
#include "timetick.h"
#include "msm.h"

#if defined(FEATURE_POWER_COLLAPSE) || defined(FEATURE_USE_MPM_VDD_MIN_CONTROL)
#include "mao.h"
#endif /* FEATURE_POWER_COLLAPSE || FEATURE_USE_MPM_VDD_MIN_CONTROL */

#ifdef  SLEEPCTL_CLKRGM_CONTROL
#error code not present
#endif /* SLEEPCTL_CLKRGM_CONTROL */

#ifdef FEATURE_SLEEPCTL_PC
#error code not present
#endif /* FEATURE_SLEEPCTL_PC */


/*============================================================================

                             DEBUG MESSAGE MACROS

============================================================================*/

#define DBG_0( LVL, FMT ) \
        MSG( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT)

#define DBG_1( LVL, FMT, A ) \
        MSG_1( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A)

#define DBG_2( LVL, FMT, A, B ) \
        MSG_2( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A, B)

#define DBG_3( LVL, FMT, A, B, C ) \
        MSG_3( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A, B, C)


#define DBG_STR1( LVL, FMT, A ) \
        MSG_SPRINTF_1( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A)

#define DBG_STR2( LVL, FMT, A, B ) \
        MSG_SPRINTF_2( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A, B)

#define DBG_STR3( LVL, FMT, A, B, C ) \
        MSG_SPRINTF_3( MSG_SSID_SLEEP, MSG_LEGACY_ ## LVL, FMT, A, B, C)



/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/


/*----------------------------------------------------------------------------
  Maximum sleep duration constants (~1.5 days)
----------------------------------------------------------------------------*/

#define TOTAL_SYS_CLK_MAX           0xFFFFFFFFuL
#define WAKEUP_COARSE_MAX           0xFFFFFF00uL
#define EXPIRE_COARSE_MAX           0xFFFFFFF0uL
#define EXPIRE_FINE_MAX             0xFFFu



/*----------------------------------------------------------------------------
  Wakeup Now reprogramming constants
----------------------------------------------------------------------------*/

#define WAKE_NOW_WAKEUP_COARSE      11   /* About 0.3ms */
#define WAKE_NOW_ENDING_COARSE      6    /* About 0.2ms more (0.5ms) */
#define WAKE_NOW_ENDING_FINE        5

#define MIN_REPROG_SCLK             7

/* Limit the amount of time spent waiting for the "wakeup now" to complete */
/* We believe that clk_busy_wait is not accurate on some targets (7600),
   and I am adding this quick fix to get around the issue.  However,
   ultimately the clk_busy_wait needs to be fixed.  Here I simply double
   the amount of time we give to clk_busy_wait (temporarily, right?) */
#define WAKE_NOW_WAIT_US            60   /* A slow clock is ~30 us or less */
#define WAKE_NOW_MAX_LOOP           ( WAKE_NOW_WAKEUP_COARSE + \
                                      WAKE_NOW_ENDING_COARSE + 5)


/*----------------------------------------------------------------------------
  Sanity limits
----------------------------------------------------------------------------*/

/* Maximum reasonable RF warmup time */
#define RF_WARMUP_MAX_USEC          45000uL

/* Maximum reasonable interrupt latency in sclks */
#define ISR_LATENCY_THRESH_US    667
#define ISR_LATENCY_THRESH       TIMETICK_SCLK_FROM_US(ISR_LATENCY_THRESH_US)


/*----------------------------------------------------------------------------
  Conversions
----------------------------------------------------------------------------*/

#define CHIPX8_TO_CHIP_SHIFT        3



/*----------------------------------------------------------------------------
  Sleep controller state
----------------------------------------------------------------------------*/

typedef enum
{
  /* Sleep controller in not active */
  SLEEPCTL_INACTIVE_STATE,

  /* Sleep controller has been armed, but not triggered */
  SLEEPCTL_ARMED_STATE,

  /* Sleep controller is counting slow clocks until the "wakeup" point */
  SLEEPCTL_SLEEP_STATE,

  /* Wakeup point has been reached.  Next: Warmup, or back to sleep? */
  SLEEPCTL_WAKEUP_STATE,

  /* Sleep controller is counting slow clocks until the end of sleep */
  SLEEPCTL_WARMUP_STATE
}
sleepctl_state_enum;



/*----------------------------------------------------------------------------
  Sleep controller h/w status bits
----------------------------------------------------------------------------*/

typedef enum
{
  #ifdef SINGLE_INTERRUPT

  SLEEPCTL_INACTIVE             = 0x01,
  SLEEPCTL_SLEEP                = 0x02,
  SLEEPCTL_WARMUP               = 0x04,

  #else /* !SINGLE_INTERRUPT */
  
  SLEEPCTL_INACTIVE             = 0x00,

  /* Individual bits */
  SLEEPCTL_CX8_IN               = 0x01,
  SLEEPCTL_CX8_OUT              = 0x02,
  SLEEPCTL_SLEEP                = 0x04,
  SLEEPCTL_WARMUP               = 0x08,

  #endif /* SINGLE_INTERRUPT */

  /* Bit masks */
  SLEEPCTL_COUNT_SCLK           = SLEEPCTL_SLEEP  | SLEEPCTL_WARMUP
}
sleepctl_status_enum;



/*----------------------------------------------------------------------------
  Sleep arm data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Sleep arm event point, in timetick counter units */
  timetick_type                   timetick;
}
sleepctl_arm_type;


/*----------------------------------------------------------------------------
  Sleep start data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Sleep start event notification callback */
  sleepctl_event_callback_type    cb;

  /* Sleep start event point, in timetick counter units */
  timetick_type                   timetick;

  /* Sleep start event point, in residual sys_clk's (eg, chipx8's) */
  uint16                          fine;
}
sleepctl_start_type;


/*----------------------------------------------------------------------------
  Sleep restart data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Slow clock delta between restart event and start event */
  uint32                          sclk_adj;
  
  /* Sleep restart event point, in timetick counter units */
  timetick_type                   timetick;
  
  /* Sleep restart event point, in residual sys_clk's (eg, chipx8's) */
  uint16                          fine;

  /* Fast clock delta between restart event and start event */
  int16                           fine_adj;
}
sleepctl_restart_type;



/*----------------------------------------------------------------------------
  Sleep rearm data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Sleep rearm (program wakeup) event point, in timetick counter units */
  timetick_type                   timetick;
}
sleepctl_rearm_type;


/*----------------------------------------------------------------------------
  Warmup data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Warmup duration, in microseconds (Max = 65.535ms) */
  uint16                          usec;

  /* Sleep warmup event point, in timetick counter units */
  timetick_type                   timetick;

  /* Warmup duration, in sleep-clocks */
  uint16                          sclk;
}
sleepctl_warmup_type;


/*----------------------------------------------------------------------------
  Wakeup data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Wakeup event notification callback */
  sleepctl_event_callback_type    cb;

  /* Wakeup hardware interrupt timetick, in timetick counter units */
  timetick_type                   int_timetick;

  /* Wakeup ISR start timestamp, in timetick counter units */
  timetick_type                   isr_start_timetick;

  /* Wakeup ISR end timestamp, in timetick counter units */
  timetick_type                   isr_end_timetick;

  /* Wakeup event point, in sleep-clocks */
  uint32                          sclk;
}
sleepctl_wakeup_type;


/*----------------------------------------------------------------------------
  Sleep ending data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Sleep ending event notification callback */
  sleepctl_event_callback_type    cb;

  /* Sleep end hardware interrupt timetick, in timetick counter units */
  timetick_type                   int_timetick;

  /* Sleep end ISR start timestamp, in timetick counter units */
  timetick_type                   isr_start_timetick;

  /* Sleep end ISR end timestamp, in timetick counter units */
  timetick_type                   isr_end_timetick;

  /* Sleep ending event point, in sleep-clocks */
  uint32                          sclk;

  /* Sleep ending event point, in residual sys_clk's (eg, chipx8's) */
  uint16                          fine;
}
sleepctl_ending_type;



/*----------------------------------------------------------------------------
  Last sleep data - used for sleepctl_error_feedback
----------------------------------------------------------------------------*/

typedef struct
{
  /* Last sleep duration, in sys_clk's (eg, chipx8's) */
  uint32                          sys_clk;

  /* Last sleep duration, in sleep-clocks */
  uint32                          sclk;
}
sleepctl_last_sleep_type;



/*----------------------------------------------------------------------------
  Interrupt Service Routine information structure type
----------------------------------------------------------------------------*/

typedef struct
{
#ifdef SINGLE_INTERRUPT

  /* Consolidated interrupt identifier */
  tramp_isr_type                  isr_id;

  /* Consolidated interrupt service routine */
  void                            (*isr)(void);

#else /* !SINGLE_INTERRUPT */

  /* Wakeup Interrupt Identifier */
  tramp_isr_type                  wakeup_id;

  /* Ending Interrupt Identifier */
  tramp_isr_type                  ending_id;

  /* Wakeup Interrupt Service Routine */
  void                            (*wakeup_isr)(void);

  /* Ending Interrupt Service Routine */
  void                            (*ending_isr)(void);

#endif /* SINGLE_INTERRUPT */
}
sleepctl_isr_type;



#ifdef SLEEPCTL_CLKRGM_CONTROL
#error code not present
#endif /* SLEEPCTL_CLKRGM_CONTROL */



/*----------------------------------------------------------------------------
  Sleep Controller constant data
----------------------------------------------------------------------------*/

typedef struct
{
  #ifdef SLEEPCTL_CLKRGM_CONTROL
#error code not present
  #endif /* SLEEPCTL_CLKRGM_CONTROL */

  /* Interrupt service routines */
  sleepctl_isr_type               isr;
}
sleepctl_const_type;



/*----------------------------------------------------------------------------
  Sleep Controller state data
----------------------------------------------------------------------------*/

typedef struct
{
  /* Sleep controller identifier */
  const char *                         id;

  /* Sleep clock system.  1x & HDR have different sclk frequency estimates */
  const sclk_sys_enum_type             sclk_system;

  /* Software state */
  sleepctl_state_enum                  state;

  /* Arming the controller */
  sleepctl_arm_type                    arm;

  /* Start of sleep data */
  sleepctl_start_type                  start;

  /* Restart, in the case of power collapse */
  sleepctl_restart_type                restart;
  
  /* Programming the wakeup point */
  sleepctl_rearm_type                  rearm;
  
  /* Whether warmup duration is strictly enforced */
  boolean                              warmup_enforced;

  /* RF warmup time constants */
  sleepctl_warmup_type                 warmup;

  /* Wakeup event data */
  sleepctl_wakeup_type                 wakeup;

  /* End of sleep data */
  sleepctl_ending_type                 ending;

  /* Last sleep duration, for error_feedback */
  sleepctl_last_sleep_type             last_sleep;

  /* Total sleep duration, in sys_clk's */
  uint32                               total_sys_clk;

  /* Callback which will make the sleep time adjustment for CPU conflicts */
  sleepctl_adjust_wakeup_callback_type adjust_cb;

  /* Conflicting sleep controller */
  sleepctl_id_type                     conflict_id;

}
sleepctl_type, *sleepctl_ptr;




/*----------------------------------------------------------------------------
  Clock regime macros
----------------------------------------------------------------------------*/

#ifdef SLEEPCTL_CLKRGM_CONTROL
#error code not present
#else /* !SLEEPCTL_CLKRGM_CONTROL */

/* Initialization helper macro - no data, so no trailing comma */
#define SLEEPCTL_CLKRGM_INIT(id)


/* Enable/disable slow & fast clock regimes - NOP's */
#define ENABLE_SLOW_CLKRGM(id)
#define ENABLE_FAST_CLKRGM(id)
#define DISABLE_SLOW_CLKRGM(id)
#define DISABLE_FAST_CLKRGM(id)


#endif /* SLEEPCTL_CLKRGM_CONTROL */



/*----------------------------------------------------------------------------
  Interrupt Service Routine identifiers
----------------------------------------------------------------------------*/

#ifdef SINGLE_INTERRUPT

  #if defined(T_MSM7600)

    #define SLPCTL_ISR_GSM    TRAMP_SLEEP_INT0_ISR
    #define SLPCTL_ISR_1X     TRAMP_SLEEP_INT1_ISR
    #define SLPCTL_ISR_HDR    TRAMP_SLEEP_INT2_ISR
    #define SLPCTL_ISR_WCDMA  TRAMP_SLEEP_INT3_ISR

  #elif defined(T_QSC60X5)

    #define SLPCTL_ISR_1X     TRAMP_SLEEP_INT_1X
    #define SLPCTL_ISR_HDR    TRAMP_SLEEP_INT_HDR

  #elif defined(T_QSC60X0)

    #define SLPCTL_ISR_1X     TRAMP_SLEEP_INT_1X

  #elif defined(T_MSM7200) || defined(T_QSC6270)

    #define SLPCTL_ISR_GSM    TRAMP_SLEEP_INT0_ISR
    #define SLPCTL_ISR_WCDMA  TRAMP_SLEEP_INT1_ISR

  #elif defined(T_MSM7500)

    #define SLPCTL_ISR_GSM    TRAMP_SLEEP_INT_GSM
    #define SLPCTL_ISR_1X     TRAMP_SLEEP_INT_1X
    #define SLPCTL_ISR_HDR    TRAMP_SLEEP_INT_HDR

  #elif defined(T_MSM6700)

    #define SLPCTL_ISR_GSM    TRAMP_SLEEP_INT_GSM
    #define SLPCTL_ISR_1X     TRAMP_SLEEP_INT_1X
    #define SLPCTL_ISR_HDR    TRAMP_SLEEP_INT_HDR

  #elif defined(T_MSM6500)

    #define SLPCTL_ISR_1X     TRAMP_UN_SUPPORTED_ISR
    #define SLPCTL_ISR_HDR    TRAMP_UN_SUPPORTED_ISR

  #else

    #define SLPCTL_ISR_1X     TRAMP_UN_SUPPORTED_ISR

  #endif /* T_MSMxxxx */

#else /* !SINGLE_INTERRUPT */

  /* Wakeup and Sleep Ending interrupt identifiers */

  #define SLPCTL_ISRS_1X      TRAMP_WAKEUP_ISR,       TRAMP_RXCHIPX8_EN_ISR
  #define SLPCTL_ISRS_HDR     TRAMP_HDR_WAKEUP_ISR,   TRAMP_SLEEP_END_HDR_ISR
  #define SLPCTL_ISRS_GSM     TRAMP_UN_SUPPORTED_ISR, TRAMP_UN_SUPPORTED_ISR
  #define SLPCTL_ISRS_WCDMA   TRAMP_UN_SUPPORTED_ISR, TRAMP_UN_SUPPORTED_ISR

 #ifdef FEATURE_INTERRUPT_CONTROL_WORKAROUND
  /* Map 1x sleep ending interrupt to TRAMP_COMP1_INT */
  #undef  SLPCTL_ISRS_1X
  #define SLPCTL_ISRS_1X      TRAMP_WAKEUP_ISR,       TRAMP_COMP1_INT
 #endif /* FEATURE_INTERRUPT_CONTROL_WORKAROUND */

#endif /* !SINGLE_INTERRUPT */
  


/*----------------------------------------------------------------------------
  sleepctl_isr_type structure initialization macro
----------------------------------------------------------------------------*/

#ifdef SINGLE_INTERRUPT

  #define SLEEPCTL_ISR_INIT(id, name) \
    { SLPCTL_ISR_ ## id, sleepctl_ ## name ## _isr }

#else /* !SINGLE_INTERRUPT */

  #define SLEEPCTL_ISR_INIT(id, name) \
    { SLPCTL_ISRS_ ## id, \
      sleepctl_ ## name ## _wakeup_isr, \
      sleepctl_ ## name ## _ending_isr }
     
#endif /* !SINGLE_INTERRUPT */




/*----------------------------------------------------------------------------
  Sleep controller state data initialization helper macros
----------------------------------------------------------------------------*/

/* sleepctl_type structure initialization macro */

#define SLEEPCTL_INIT(id,sclksys) { id, sclksys }


/* Initialization macros for the various sleep controllers */

#define SLEEPCTL_INIT_1X          SLEEPCTL_INIT("IS-2000", SCLK_1X)
#define SLEEPCTL_INIT_HDR         SLEEPCTL_INIT("IS-856",  SCLK_HDR)
#define SLEEPCTL_INIT_GSM         SLEEPCTL_INIT("GSM",     SCLK_GSM)
#define SLEEPCTL_INIT_WCDMA       SLEEPCTL_INIT("WCDMA",   SCLK_WCDMA)



/*----------------------------------------------------------------------------
  Sleep Controller constant data initialization macros
----------------------------------------------------------------------------*/

/* sleepctl_const_type structure initialization macro */

#define SLEEPCTL_CONST(id,name) \
  { SLEEPCTL_CLKRGM_INIT(id) SLEEPCTL_ISR_INIT(id,name) }


/* Initialization macros for the various sleep controllers */

#define SLEEPCTL_CONST_1X         SLEEPCTL_CONST(1X,1x)
#define SLEEPCTL_CONST_HDR        SLEEPCTL_CONST(HDR,hdr)
#define SLEEPCTL_CONST_GSM        SLEEPCTL_CONST(GSM,gsm)
#define SLEEPCTL_CONST_WCDMA      SLEEPCTL_CONST(WCDMA,wcdma)




/*============================================================================

                                 DEBUG TRACE

  Record key values written to and read from the sleep controller

============================================================================*/

/* Number of events in circular buffer.  0 disabled tracing */
#define NUM_TRACE_EVENTS          64


#if defined(NUM_TRACE_EVENTS)  &&  (NUM_TRACE_EVENTS>0)


/* SleepCtl Trace events */
typedef enum
{
  SCT_START_INT,
  SCT_WAKEUP_INT,
  SCT_ENDING_INT,
  SCT_START_SLEEP,
  SCT_ARM_SLEEP,
  SCT_BLOCK_UNTIL,
  SCT_START_EVENT,
  SCT_UPDATE_WAKEUP,
  SCT_SET_WARMUP,
  SCT_SET_DURATION,
  SCT_SET_DURATION_MS,
  SCT_SET_MAX_DURATION,
  SCT_GET_SYSCLK_COUNT,
  SCT_GET_SCLK_TILL_WAKEUP,
  SCT_GET_SCLK_TILL_RTC_ON,
  SCT_GET_WAKEUP_TIME,
  SCT_GET_ENDING_TIME,
  SCT_COMMIT_TO_WAKEUP,
  SCT_RESTORE_FROM_PC,
  SCT_INT_PROFILE,
  SCT_FORCE_WAKEUP
}
sleepctl_trace_event_enum;


/* SleepCtl trace event record.  Id, event, 3 integers, and a time-stamp */
typedef struct
{
  sleepctl_id_type                id;
  sleepctl_trace_event_enum       event;
  uint32                          a32;
  uint32                          b32;
  uint32                          c32;
  uint32                          ts;
}
sleepctl_trace_event_type;


/* SleepCtl trace structure type */
typedef struct
{
  /* Last location to written to in circular "events" buffer. */
  unsigned                        last;

  /* Circular "events" buffer */
  sleepctl_trace_event_type       events[ NUM_TRACE_EVENTS ];
}
sleepctl_trace_type;


/* SleepCtl Trace event storage */
static sleepctl_trace_type        sleepctl_trace;


/* Macro for passing logging data to logging function */
#define TRACE(id, event, a32, b32, c32) \
  sleepctl_trace_event(id, SCT_ ## event, \
    (uint32)(a32), (uint32)(b32), (uint32)(c32))
#define TRACE_RLE(id, event, a32, b32) \
  sleepctl_trace_event_rle(id, SCT_ ## event, (uint32)(a32), (uint32)(b32) )


/*----------------------------------------------------------------------------
  Sleepctl logging function
----------------------------------------------------------------------------*/

static void sleepctl_trace_event
(
  sleepctl_id_type                id,     /* Sleep controller identifier */
  sleepctl_trace_event_enum       event,  /* Event that is being recorded */
  uint32                          a32,    /* Event dependent 32-bit data */
  uint32                          b32,    /* Event dependent 32-bit data */
  uint32                          c32     /* Event dependent 32-bit data */
)
{
  sleepctl_trace_event_type       *e;

/*--------------------------------------------------------------------------*/

  INTLOCK();

  /* Update the pointer to the last event ... */
  if ( ++sleepctl_trace.last == NUM_TRACE_EVENTS )
  {
    /* ... wrapping at buffer limit */
    sleepctl_trace.last = 0;
  }

  /* Get a pointer to the event structure, for convinence */
  e = &sleepctl_trace.events[ sleepctl_trace.last ];

  /* Record the event */
  e->id    = id;
  e->event = event;
  e->a32   = a32;
  e->b32   = b32;
  e->c32   = c32;
  e->ts    = timetick_get_safe();

  INTFREE();

} /* sleepctl_trace_event */


/*----------------------------------------------------------------------------
  log events as run length encoding - if the same id/event pair occur
  in sequence, just overwrite the event.  The count of successive
  events is stored in the third data element, so rle log data only
  takes two words of log data
----------------------------------------------------------------------------*/

static void sleepctl_trace_event_rle
(
  sleepctl_id_type                id,     /* Sleep controller identifier */
  sleepctl_trace_event_enum       event,  /* Event that is being recorded */
  uint32                          a32,    /* Event dependent 32-bit data */
  uint32                          b32     /* Event dependent 32-bit data */
)
{
  sleepctl_trace_event_type       *e;

/*--------------------------------------------------------------------------*/

  INTLOCK();

  /* Get a pointer to the event structure, for convinence */
  e = &sleepctl_trace.events[ sleepctl_trace.last ];

  /* If the new event is the same as the old event */
  if ( (e->event == event) && (e->id == id) )
  {
    /* Update the record in place */
    e->a32   = a32;
    e->b32   = b32;
    e->c32++;
    e->ts    = timetick_get_safe();
  }
  else
  {
    /* Otherwise, add a new log entry */
    sleepctl_trace_event( id, event, a32, b32, 0 );
  }
  INTFREE();

} /* sleepctl_trace_event_rle */


#else /* NUM_TRACE_EVENTS == 0 */


/* No logging is desired - TRACE macro becomes a no-op. */
#define TRACE(id, event, a32, b32, c32)   
#define TRACE_RLE(id, event, a32, b32)   


#endif /* NUM_TRACE_EVENTS */




/*============================================================================

                          H/W I/O MACRO DECLARATIONS

  These macros are used to read and write values to the different sleep
  controller hardware, based on the ID of the sleep controller.
  
  Note: SLEEP_COUNT_COARSE must be read until same value is returned!

============================================================================*/

/* Don't write a value to the WAKEUP or EXPIRE COARSE registers that is
   within this number of slow clocks of the current COUNT value. */
#define MIN_REARM_SCLK  6


#if defined(T_MSM7600) || defined(T_QSC60X0) || defined(T_QSC60X5) || \
    defined(T_MSM7200) || defined(T_MSM7500) || defined(T_MSM6700) || \
    defined(T_QSC6270)


/*----------------------------------------------------------------------------
  Hardware I/O Macros
----------------------------------------------------------------------------*/

#ifdef T_QUASAR
#error code not present
#else

/*----------------------------------------------------------------------------
  Address & Mask macros for sclk_reg_*() functions
----------------------------------------------------------------------------*/

#define COUNT(id)           HWIO_ADDRI(SLEEPn_COARSE_COUNT_VAL, id)

#define WRITE(reg,id)       HWIO_ADDRI(SLEEPn_ ## reg ## _TIME, id)
#define SYNC(reg,id)        HWIO_ADDRI(SLEEPn_ ## reg ## _TIME_LATCH, id)
#define MASK(reg)           HWIO_FMSK( SLEEPn_ ## reg ## _TIME_LATCH, LATCHING)


/*----------------------------------------------------------------------------
  Coarse SCLK register write macros
----------------------------------------------------------------------------*/

/* Set the given sleep controller sclk register to the given value, subject
   to that value being at least "min" above the current COARSE_COUNT_VAL.
   Ensure the value will latch before returning. */
#define SET_MIN_SYNC(reg, id, sclk) \
        sclk_reg_set_min_sync(sclk,           MIN_REARM_SCLK,  MASK(reg), \
                              WRITE(reg,id),  COUNT(id),       SYNC(reg,id) )

/* Set the given sleep controller sclk register to the given maximal value.
   Ensure the value will latch before returning. */
#define SET_SYNC(reg, id, sclk) \
        sclk_reg_set_sync(sclk, MASK(reg), WRITE(reg,id), SYNC(reg,id) )


/*----------------------------------------------------------------------------
  Hardware I/O Macros
----------------------------------------------------------------------------*/

/* Sleep duration output macros */

#define SET_WAKEUP_COARSE(id, sclk) SET_MIN_SYNC(WAKEUP, id, sclk)
#define SET_EXPIRE_COARSE(id, sclk) SET_MIN_SYNC(COARSE, id, sclk)
#define SET_EXPIRE_FINE(id,sys_clk) HWIO_OUTI(SLEEPn_FINE_TIME, id, sys_clk)


/* Maximum sleep duration output macros */

#define SET_WAKEUP_COARSE_MAX(id)   SET_SYNC(WAKEUP, id, WAKEUP_COARSE_MAX)
#define SET_EXPIRE_COARSE_MAX(id)   SET_SYNC(COARSE, id, EXPIRE_COARSE_MAX)
#define SET_EXPIRE_FINE_MAX(id)     SET_EXPIRE_FINE( id, EXPIRE_FINE_MAX)


/* Enable/Disable/Arm/Start macros */

#define ARM_SLEEP(id)               HWIO_OUTI(SLEEPn_RTC_ARM, id, 1)
#define DISARM_SLEEP(id)            HWIO_OUTI(SLEEPn_RTC_ARM, id, 0)

#define START_SLEEP(id)             HWIO_OUTMI(SLEEPn_MICRO_START, id, \
                                      HWIO_FMSK(SLEEPn_MICRO_START, START), \
                                      HWIO_FMSK(SLEEPn_MICRO_START, START) )
                                    
#define ENABLE_SLEEP_INT(id)        HWIO_OUTI(SLEEPn_INT_EN, id, 1)
#define DISABLE_SLEEP_INT(id)       HWIO_OUTI(SLEEPn_INT_EN, id, 0)


/* Input macros */

#define STATUS(id)                  HWIO_INI(SLEEPn_STATUS, id)
#define SLEEP_COUNT_COARSE(id)      HWIO_INI(SLEEPn_COARSE_COUNT_VAL, id)
#define SLEEP_COUNT_FINE(id)        HWIO_INI(SLEEPn_FINE_COUNT_VAL, id)

#endif /* T_QUASAR */

#elif defined(T_MSM6550)


/*----------------------------------------------------------------------------
  Hardware I/O Address Structure Type
----------------------------------------------------------------------------*/

typedef struct
{
  volatile uint32 *               wakeup_sclk;
  const volatile uint32 *         wakeup_latch_status;
  const uint32                    wakeup_latch_mask;
  volatile uint32 *               expire_sclk;
  const volatile uint32 *         expire_latch_status;
  const uint32                    expire_latch_mask;
  volatile uint32 *               expire_fine;
  volatile uint32 *               ctrl;
  volatile const uint32 *         status;
  volatile const uint32 *         count_sclk;
  volatile const uint32 *         count_fine;
}
sleepctl_hw_type;


/*----------------------------------------------------------------------------
  Hardware I/O Address Data, per sleep controller
----------------------------------------------------------------------------*/

static const sleepctl_hw_type     sleepctl_hw[SLEEPCTL_NUM_CONTROLLERS] =
{

  /*------------------------------------------------------------------------
    1x Sleep controller h/w structure
  ------------------------------------------------------------------------*/
  {
    (volatile uint32 *)           HWIO_SLEEP_WAKEUP_ADDR,
    (volatile uint32 *)           HWIO_SLEEP_WAKEUP_STATUS_ADDR,
                                  HWIO_SLEEP_WAKEUP_STATUS_SYNC_STATUS_BMSK,
    (volatile uint32 *)           HWIO_SLEEP_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_SLEEP_EXPIRE_STATUS_ADDR,
                                  HWIO_SLEEP_EXPIRE_STATUS_SYNC_STATUS_BMSK,
    (volatile uint32 *)           HWIO_SLEEP_CX8_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_SLEEP_CTL_ADDR,
    (volatile const uint32 *)     HWIO_SLEEP_STATUS_ADDR,
    (volatile const uint32 *)     HWIO_SLEEP_COUNT_ADDR,
    (volatile const uint32 *)     HWIO_SLEEP_CX8_COUNT_ADDR
  }

  /*------------------------------------------------------------------------
    HDR Sleep controller h/w structure
  ------------------------------------------------------------------------*/
, {
    (volatile uint32 *)           HWIO_HDR_SLEEP_WAKEUP_ADDR,
    (volatile uint32 *)           HWIO_HDR_SLEEP_WAKEUP_STATUS_ADDR,
                                  HWIO_HDR_SLEEP_WAKEUP_STATUS_SYNC_STATUS_BMSK,
    (volatile uint32 *)           HWIO_HDR_SLEEP_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_HDR_SLEEP_EXPIRE_STATUS_ADDR,
                                  HWIO_HDR_SLEEP_EXPIRE_STATUS_SYNC_STATUS_BMSK,
    (volatile uint32 *)           HWIO_HDR_SLEEP_CX8_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_HDR_SLEEP_CTL_ADDR,
    (volatile const uint32 *)     HWIO_HDR_SLEEP_STATUS_ADDR,
    (volatile const uint32 *)     HWIO_HDR_SLEEP_COUNT_ADDR,
    (volatile const uint32 *)     HWIO_HDR_SLEEP_CX8_COUNT_ADDR
  }  
};


/*----------------------------------------------------------------------------
  Address & Mask macros for sclk_reg_*() functions
----------------------------------------------------------------------------*/

#define COUNT(id)           (uint32) sleepctl_hw[id].count_sclk

#define WRITE(reg,id)       (uint32) sleepctl_hw[id].reg ## _sclk
#define SYNC(reg,id)        (uint32) sleepctl_hw[id].reg ## _latch_status
#define MASK(reg)           sleepctl_hw[id].reg ## _latch_mask


/*----------------------------------------------------------------------------
  Coarse SCLK register write macros
----------------------------------------------------------------------------*/

/* Set the given sleep controller sclk register to the given value, subject
   to that value being at least "min" above the current COARSE_COUNT_VAL.
   Ensure the value will latch before returning. */
#define SET_MIN_SYNC(reg, id, sclk) \
        sclk_reg_set_min_sync(sclk,           MIN_REARM_SCLK, MASK(reg), \
                              WRITE(reg,id),  COUNT(id),      SYNC(reg,id) )

/* Set the given sleep controller sclk register to the given maximal value.
   Ensure the value will latch before returning. */
#define SET_SYNC(reg, id, sclk) \
            sclk_reg_set_sync(sclk, MASK(reg), WRITE(reg,id), SYNC(reg,id) )


/*----------------------------------------------------------------------------
  Hardware I/O Macros
----------------------------------------------------------------------------*/

/* Sleep duration output macros */

#define SET_WAKEUP_COARSE(id, sclk) SET_MIN_SYNC(wakeup, id, sclk)
#define SET_EXPIRE_COARSE(id, sclk) SET_MIN_SYNC(expire, id, sclk)
#define SET_EXPIRE_FINE(id,sys_clk) (*sleepctl_hw[id].expire_fine = (sys_clk))


/* Maximum sleep duration output macros */

#define SET_WAKEUP_COARSE_MAX(id)   SET_SYNC(wakeup, id, WAKEUP_COARSE_MAX)
#define SET_EXPIRE_COARSE_MAX(id)   SET_SYNC(expire, id, EXPIRE_COARSE_MAX)
#define SET_EXPIRE_FINE_MAX(id)     SET_EXPIRE_FINE( id, EXPIRE_FINE_MAX)


/* Enable/Disable/Arm/Start macros */

#define ARM_SLEEP(id)               (*sleepctl_hw[id].ctrl = 1)
#define DISARM_SLEEP(id)            (*sleepctl_hw[id].ctrl = 0)

/* The following macro is 1x-only */
#define START_SLEEP(id)             ARM_SLEEP(id); \
                                    HWIO_OUTM( SLEEP_CMD, \
                                      HWIO_FMSK(SLEEP_CMD, SLEEP_NOW), \
                                      HWIO_FMSK(SLEEP_CMD, SLEEP_NOW) )

#define ENABLE_SLEEP_INT(id)
#define DISABLE_SLEEP_INT(id)


/* Input macros */

#define STATUS(id)                  (*sleepctl_hw[id].status)
#define SLEEP_COUNT_COARSE(id)      (*sleepctl_hw[id].count_sclk)
#define SLEEP_COUNT_FINE(id)        (*sleepctl_hw[id].count_fine)


#else /* !T_MSM6700 && !T_MSM6550 --> 6500 or earlier */


/*----------------------------------------------------------------------------
  Hardware I/O Address Structure Type
----------------------------------------------------------------------------*/

typedef struct
{
  volatile uint32 *               wakeup_sclk;
  volatile uint32 *               expire_sclk;
  volatile uint32 *               expire_fine;
  volatile uint32 *               ctrl;
  volatile const uint32 *         status;
  volatile const uint32 *         count_sclk;
  volatile const uint32 *         count_fine;
}
sleepctl_hw_type;


/*----------------------------------------------------------------------------
  Hardware I/O Address Data, per sleep controller
----------------------------------------------------------------------------*/

static const sleepctl_hw_type     sleepctl_hw[SLEEPCTL_NUM_CONTROLLERS] =
{

  /*------------------------------------------------------------------------
    1x Sleep controller h/w structure
  ------------------------------------------------------------------------*/
  {
    (volatile uint32 *)           HWIO_SLEEP_WAKEUP_ADDR,
    (volatile uint32 *)           HWIO_SLEEP_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_SLEEP_CX8_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_SLEEP_CTL_ADDR,
    (volatile const uint32 *)     HWIO_SLEEP_STATUS_ADDR,
    (volatile const uint32 *)     HWIO_SLEEP_COUNT_ADDR,
    (volatile const uint32 *)     HWIO_SLEEP_CX8_COUNT_ADDR
  }

  #if defined(T_MSM6500)
  /*------------------------------------------------------------------------
    HDR Sleep controller h/w structure
  ------------------------------------------------------------------------*/
, {
    (volatile uint32 *)           HWIO_HDR_SLEEP_WAKEUP_ADDR,
    (volatile uint32 *)           HWIO_HDR_SLEEP_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_HDR_SLEEP_CX8_EXPIRE_ADDR,
    (volatile uint32 *)           HWIO_HDR_SLEEP_CTL_ADDR,
    (volatile const uint32 *)     HWIO_HDR_SLEEP_STATUS_ADDR,
    (volatile const uint32 *)     HWIO_HDR_SLEEP_COUNT_ADDR,
    (volatile const uint32 *)     HWIO_HDR_SLEEP_CX8_COUNT_ADDR
  }
  #endif /* defined(T_MSM6500) */
  
};


/*----------------------------------------------------------------------------
  Address macros for sclk_reg_*() functions
----------------------------------------------------------------------------*/

#define COUNT(id)                   (uint32) sleepctl_hw[id].count_sclk
#define WRITE(reg,id)               (uint32) sleepctl_hw[id].reg ## _sclk


/*----------------------------------------------------------------------------
  Coarse SCLK register write macros
----------------------------------------------------------------------------*/

/* Set the given sleep controller sclk register to the given value, subject
   to that value being at least "min" above the current COARSE_COUNT_VAL.
   Ensure the value will latch before returning. */
#define SET_MIN(reg, id, sclk) \
        sclk_reg_set_min(sclk, MIN_REARM_SCLK, WRITE(reg,id), COUNT(id) )

/* Set the given sleep controller sclk register to the given maximal value.
   Ensure the value will latch before returning. */
#define SET(reg, id, sclk)          (*sleepctl_hw[id].reg ## _sclk = (sclk))


/*----------------------------------------------------------------------------
  Hardware I/O Macros
----------------------------------------------------------------------------*/

/* Sleep duration output macros */

#define SET_WAKEUP_COARSE(id, sclk) SET_MIN(wakeup, id, sclk)
#define SET_EXPIRE_COARSE(id, sclk) SET_MIN(expire, id, sclk)
#define SET_EXPIRE_FINE(id,sys_clk) (*sleepctl_hw[id].expire_fine = (sys_clk))


/* Maximum sleep duration output macros */

#define SET_WAKEUP_COARSE_MAX(id)   SET(wakeup, id, WAKEUP_COARSE_MAX)
#define SET_EXPIRE_COARSE_MAX(id)   SET(expire, id, EXPIRE_COARSE_MAX)
#define SET_EXPIRE_FINE_MAX(id)     SET_EXPIRE_FINE( id, EXPIRE_FINE_MAX)


/* Enable/Disable/Arm/Start macros */

#define ARM_SLEEP(id)               (*sleepctl_hw[id].ctrl = 1)
#define DISARM_SLEEP(id)            (*sleepctl_hw[id].ctrl = 0)

/* The following macro is 1x-only */
#define START_SLEEP(id)             ARM_SLEEP(id); \
                                    HWIO_OUTM( SLEEP_CMD, \
                                      HWIO_FMSK(SLEEP_CMD, SLEEP_NOW), \
                                      HWIO_FMSK(SLEEP_CMD, SLEEP_NOW) )

#define ENABLE_SLEEP_INT(id)
#define DISABLE_SLEEP_INT(id)


/* Input macros */

#define STATUS(id)                  (*sleepctl_hw[id].status)
#define SLEEP_COUNT_COARSE(id)      (*sleepctl_hw[id].count_sclk)
#define SLEEP_COUNT_FINE(id)        (*sleepctl_hw[id].count_fine)

#endif /* !T_MSM6700 && !T_MSM6550 */




/*============================================================================

                          H/W I/O HELPER FUNCTIONS

  These functions are used to read values from the sleep controller hardware,
  which change asynchronously, such as during the read.  If reading the value
  a second time produces a value different from the first, the value was
  changing during the read, and must be read again.

============================================================================*/



/*============================================================================

FUNCTION SLEEPCTL_GET_STATUS

DESCRIPTION
  This function returns the sleep controller hardware status

DEPENDENCIES
  Must be called from inside INTLOCK and BEGIN/END_REG_ACCESS contexts.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

static uint32 sleepctl_get_status
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Status values */
  uint32                          status, status2;

/*--------------------------------------------------------------------------*/

  /* Read the value twice */
  status  = STATUS(id);
  status2 = STATUS(id);

  /* If different values were read, the value was changing... */
  if ( status != status2 )
  {
    /* ... so now read the value again */
    status = STATUS(id);
  }

  return status;
}

#if defined(FEATURE_POWER_COLLAPSE) || defined(FEATURE_USE_MPM_VDD_MIN_CONTROL)


/*============================================================================

FUNCTION SLEEPCTL_GET_START_TIMETICK

DESCRIPTION
  Return the slow clock counter value at start of sleep cycle

DEPENDENCIES
  The given sleep controller must be running.
  Must be called from inside INTLOCK and BEGIN/END_REG_ACCESS contexts.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

static timetick_type sleepctl_get_start_timetick
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk1, sclk2; 

  /* Slow clock counter value */
  timetick_type                   timetick;

/*--------------------------------------------------------------------------*/

  /* Wait until we are at start of next sleep clock based timetick */
  mao_block_until_next_tick();

  /* Give some delay to ensure that we are in the zone within
     sleep cycle where reading MPM and Sleep controller counters
     would give actual offset. Taking these readings around sleep clock
     cycle edges could result in bad start timetick */
  clk_busy_wait(10);
  
  /* Read sleep controller and MPM sleep clock counter values */
  sclk1 = SLEEP_COUNT_COARSE(id);
  timetick = (timetick_type) MAO_READ_TIMETICK();

  /* Read sleep controller counter again */
  sclk2 = SLEEP_COUNT_COARSE(id);

  /* If the values are different, we sampled when the count was changing. */
  if ( sclk1 != sclk2)
  {
    timetick = (timetick_type) MAO_READ_TIMETICK();
    sclk2 = SLEEP_COUNT_COARSE(id);
  }

  return (timetick - sclk2);

} /* sleepctl_get_start_timetick() */

#else

/*============================================================================

FUNCTION SLEEPCTL_GET_START_TIMETICK

DESCRIPTION
  Return the slow clock counter value at start of sleep cycle

DEPENDENCIES
  The given sleep controller must be running.
  Must be called from inside INTLOCK and BEGIN/END_REG_ACCESS contexts.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

static timetick_type sleepctl_get_start_timetick
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk1, sclk2; 

  /* Slow clock counter value */
  timetick_type                   timetick;

/*--------------------------------------------------------------------------*/

  /* Read the number of sleep clocks the controller has slept for */
  sclk1 = SLEEP_COUNT_COARSE(id);
  
  /* Read the slow counter value */
  timetick =  timetick_get_safe();

  /* Read it again */
  sclk2 = SLEEP_COUNT_COARSE(id);

  /* If the values are different, we sampled when the count was changing. */
  if ( sclk1 != sclk2)
  {
    /* Read the counters once again, should be stable now */
    timetick =  timetick_get_safe();
    sclk2 = SLEEP_COUNT_COARSE(id);
  }

  return (timetick - sclk2);

} /* sleepctl_get_start_timetick() */

#endif /* FEATURE_POWER_COLLAPSE || FEATURE_USE_MPM_VDD_MIN_CONTROL */


/*============================================================================

                 SLEEP CONTROLLER INTERRUPT SERVICE HANDLERS

  This structure is used to install appropriate interrupt handler for a given
  sleep controller
  
============================================================================*/


/* Forward declarations */

#ifdef SINGLE_INTERRUPT
static void sleepctl_start_isr  ( sleepctl_id_type id);
#endif /* !SINGLE_INTERRUPT */

static void sleepctl_wakeup_isr ( sleepctl_id_type id);
static void sleepctl_ending_isr ( sleepctl_id_type id);




#ifdef SINGLE_INTERRUPT
/*============================================================================

FUNCTION  SLEEPCTL_ISR

DESCRIPTION
  Extract sleep controller's hardware state to determine the appropriate
  h/w event the ISR is responding to, and pass control to the appropriate
  handler.

DEPENDENCIES
  Must only be called by sleepctl_xxx_isr()'s

RETURN VALUE
  None

SIDE EFFECTS
  Callback functions registered with the appropriate sleep controller
  will be invoked.

============================================================================*/

static void sleepctl_isr
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          status;

/*--------------------------------------------------------------------------*/

  INTLOCK();
  BEGIN_REG_ACCESS_IN_INTLOCK();

  /* Get the status of the sleep controller */
    status = sleepctl_get_status( id );

  END_REG_ACCESS_IN_INTLOCK();
  INTFREE();

  switch ( status )
  {
    /* If the hardware is now in SLEEP state, ... */
    case SLEEPCTL_SLEEP:
      /* ... then sleep just started. */
      sleepctl_start_isr( id );
      break;

    /* If the hardware is NOW in WARMUP state, ...  */
    case SLEEPCTL_WARMUP:
      /* ... then we are waking up. */
      sleepctl_wakeup_isr( id );
      break;

      /* If the hardware is NOW in INACTIVE state, ...  */
    case SLEEPCTL_INACTIVE:
      /* ... then we just finished sleep. */
      sleepctl_ending_isr( id );
      break;

    default:
      ERR("Unexpected value from get_status(%d): %d", id, status, 0);
  }

} /* sleepctl_isr() */



/*============================================================================

FUNCTIONS  SLEEPCTL_xxx_ISR's

DESCRIPTION
  Binds sleep controllers' identifier to the invocation of sleepctl_isr()

DEPENDENCIES
  Must only be called by TRAMP via tramp_set_isr( )

RETURN VALUE
  None

SIDE EFFECTS
  Callback functions registered with the appropriate sleep controller
  will be invoked.

============================================================================*/

#if SUPPORTED & S_1X
static void sleepctl_1x_isr( void )
{
  sleepctl_isr( SLEEPCTL_1X );
}
#endif /* SUPPORTED & S_1X */


#if SUPPORTED & S_HDR
static void sleepctl_hdr_isr( void )
{
  sleepctl_isr( SLEEPCTL_HDR );
}
#endif /* SUPPORTED & S_HDR */


#if SUPPORTED & S_GSM
static void sleepctl_gsm_isr( void )
{
  sleepctl_isr( SLEEPCTL_GSM );
}
#endif /* SUPPORTED & S_GSM */


#if SUPPORTED & S_WCDMA
static void sleepctl_wcdma_isr( void )
{
  sleepctl_isr( SLEEPCTL_WCDMA );
}
#endif /* SUPPORTED & S_WCDMA */



#else /* !SINGLE_INTERRUPT */

/*============================================================================

FUNCTIONS  *** INTERRUPT HANDLERS ***

DESCRIPTION
  Binds sleep controllers' static data to the call to the appropriate
  interrupt processing functions.

DEPENDENCIES
  Must only be called by TRAMP via tramp_set_isr( )

RETURN VALUE
  None

SIDE EFFECTS
  Callback functions registered with the appropriate sleep controller
  will be invoked.

============================================================================*/

#if SUPPORTED & S_1X

static void sleepctl_1x_wakeup_isr( void )
{
  sleepctl_wakeup_isr( SLEEPCTL_1X );
}


static void sleepctl_1x_ending_isr( void )
{
  sleepctl_ending_isr( SLEEPCTL_1X );
}

#endif /* SUPPORTED & S_1X */



#if SUPPORTED & S_HDR

static void sleepctl_hdr_wakeup_isr( void )
{
  sleepctl_wakeup_isr( SLEEPCTL_HDR );
}


static void sleepctl_hdr_ending_isr( void )
{
  sleepctl_ending_isr( SLEEPCTL_HDR );
}

#endif /* SUPPORTED & S_HDR */



#if SUPPORTED & S_GSM

static void sleepctl_gsm_wakeup_isr( void )
{
  sleepctl_wakeup_isr( SLEEPCTL_GSM );
}


static void sleepctl_gsm_ending_isr( void )
{
  sleepctl_ending_isr( SLEEPCTL_GSM );
}

#endif /* SUPPORTED & S_GSM */



#if SUPPORTED & S_WCDMA

static void sleepctl_wcdma_wakeup_isr( void )
{
  sleepctl_wakeup_isr( SLEEPCTL_WCDMA );
}


static void sleepctl_wcdma_ending_isr( void )
{
  sleepctl_ending_isr( SLEEPCTL_WCDMA );
}

#endif /* SUPPORTED & S_WCDMA */


#endif /* !SINGLE_INTERRUPT */


/*============================================================================

                       SLEEP CONTROLLER DATA STRUCTURES

  This array of structures holds constant sleep controller data, based on the
  ID of the sleep controller.
  
============================================================================*/


/*----------------------------------------------------------------------------
  Sleep controller state data array
----------------------------------------------------------------------------*/

/*lint -save -e785 : Omitted fields are automatically zero-initialized */

static sleepctl_type              sleepctl[SLEEPCTL_NUM_CONTROLLERS] =
{
  #if defined(T_MSM7600)
   
  SLEEPCTL_INIT_GSM,
  SLEEPCTL_INIT_1X,
  SLEEPCTL_INIT_HDR,
  SLEEPCTL_INIT_WCDMA

  #elif defined(T_QSC1100)
  
  SLEEPCTL_INIT_1X

  #elif defined(T_QSC60X5)

  SLEEPCTL_INIT_1X,
  SLEEPCTL_INIT_HDR

  #elif defined(T_QSC60X0)

  SLEEPCTL_INIT_1X

  #elif defined(T_MSM7200) || defined(T_QSC6270)
   
  SLEEPCTL_INIT_GSM,
  SLEEPCTL_INIT_WCDMA

  #elif defined(T_MSM7500)
  
  SLEEPCTL_INIT_GSM,
  SLEEPCTL_INIT_1X,
  SLEEPCTL_INIT_HDR

  #elif defined(T_MSM6700)
  
  SLEEPCTL_INIT_GSM,
  SLEEPCTL_INIT_1X,
  SLEEPCTL_INIT_HDR

  #elif defined(T_MSM6500)

  SLEEPCTL_INIT_1X,
  SLEEPCTL_INIT_HDR

  #else
  
  SLEEPCTL_INIT_1X

  #endif
};

/*lint -restore */


#ifdef FEATURE_SLEEPCTL_PC
#error code not present
#endif


/*----------------------------------------------------------------------------
  Sleep Controller constant data
----------------------------------------------------------------------------*/

static const sleepctl_const_type sleepctl_const[SLEEPCTL_NUM_CONTROLLERS] =
{
  #if defined(T_MSM7600)
  
  SLEEPCTL_CONST_GSM,
  SLEEPCTL_CONST_1X,
  SLEEPCTL_CONST_HDR,
  SLEEPCTL_CONST_WCDMA

  #elif defined(T_QSC1100)
  
  SLEEPCTL_CONST_1X

  #elif defined(T_QSC60X5)

  SLEEPCTL_CONST_1X,
  SLEEPCTL_CONST_HDR

  #elif defined(T_QSC60X0)

  SLEEPCTL_CONST_1X

  #elif defined(T_MSM7200) || defined(T_QSC6270)
   
  SLEEPCTL_CONST_GSM,
  SLEEPCTL_CONST_WCDMA

  #elif defined(T_MSM7500)
  
  SLEEPCTL_CONST_GSM,
  SLEEPCTL_CONST_1X,
  SLEEPCTL_CONST_HDR

  #elif defined(T_MSM6700)
  
  SLEEPCTL_CONST_GSM,
  SLEEPCTL_CONST_1X,
  SLEEPCTL_CONST_HDR

  #elif defined(T_MSM6500)

  SLEEPCTL_CONST_1X,
  SLEEPCTL_CONST_HDR

  #else

  SLEEPCTL_CONST_1X
  
  #endif
};




/*============================================================================

                       INTERRUPT FUNCTION DECLARATIONS

============================================================================*/


/*============================================================================

FUNCTION SLEEPCTL_START_EVENT

DESCRIPTION
  This function is called to transition the sleep controller from the
  ARMED state to the SLEEPING state.

DEPENDENCIES
  Must only be called in response to a sleep controller interrupt
  which is not for wakeup or sleep ending.

RETURN VALUE
  None

SIDE EFFECTS
  May turn off the sleep controller's FAST (sysclk) clock regime.

============================================================================*/

static void sleepctl_start_event
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  INTLOCK();

  /* Get slow clock timetick at start event point */
  sc->start.timetick = sleepctl_get_start_timetick(id);

  /* Capture the timestamp when the ending int is expected to occur */
  /* Add one extra sclk to (roughly) account for cx8 (ending.fine) time */
  sc->ending.int_timetick = sc->start.timetick + sc->ending.sclk + 1;

  /* Capture the timestamp when the wakeup int is expected to occur */
  sc->wakeup.int_timetick = sc->start.timetick + sc->wakeup.sclk;

  BEGIN_REG_ACCESS_IN_INTLOCK();
  /* Clear the ARM bit on targets which require it */
  DISARM_SLEEP(id);

  /* Get sysclks from start of sleep to first sclk */
  sc->start.fine = (uint16) SLEEP_COUNT_FINE(id);

  END_REG_ACCESS_IN_INTLOCK();

  INTFREE();

  TRACE(id, START_EVENT, sc->start.fine, sc->start.timetick, 0);

  /* Advance state from "ARMED" to "SLEEPING" */
  sc->state = SLEEPCTL_SLEEP_STATE;

  /* Disable sleep controller's fast clock regime.
     It is not needed until the wakeup has been committed to. */
  DISABLE_FAST_CLKRGM(id);

  if ( sc->start.cb != NULL )
  {
    sc->start.cb();
  }

} /* sleepctl_start_event */


#ifdef SINGLE_INTERRUPT
/*============================================================================

FUNCTION SLEEPCTL_START_ISR

DESCRIPTION
  This function is called on a Sleep Controller interrupt which does
  not correspond to wakeup or sleep ending.

  Sleep controllers state is advanced from ARMED to SLEEP.

DEPENDENCIES
  Must only be called in response to a sleep controller interrupt
  which is not for wakeup or sleep ending.

RETURN VALUE
  None

SIDE EFFECTS
  May turn off the sleep controller's FAST (sysclk) clock regime.

============================================================================*/

static void sleepctl_start_isr
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  switch ( sc->state )
  {
    case SLEEPCTL_ARMED_STATE:

      DBG_STR1( HIGH, "%s Start", sc->id);

      sleepctl_start_event( id );

      break;

    case SLEEPCTL_SLEEP_STATE:

      /* This is expected if "block until sleeping" is used.
         It would be an error if it occurs more than once. */
      DBG_STR1( MED, "%s Start, already started", sc->id);
  
      break;

    default:

      DBG_STR2(ERROR, "Unexpected %s Start int, state=%d", sc->id, sc->state);
  }

  TRACE(id, START_INT, sc->start.fine, 0, 0);

} /* sleepctl_start_isr */

#endif /* SINGLE_INTERRUPT */



/*============================================================================

FUNCTION SLEEPCTL_WAKEUP_ISR

DESCRIPTION
  This function is called on the Sleep Controller's "Wakeup" interrupt.
  It calls the registered callback function for the event.

DEPENDENCIES
  Must only be called in response to the Wakeup interrupt

RETURN VALUE
  None

SIDE EFFECTS
  Calls the sleep controller's "wakeup" callback.

============================================================================*/

static void sleepctl_wakeup_isr
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk;

  /* Spurious interrupt counter */
  static uint32                   spurious_wakeup_int_cnt = 0;

/*--------------------------------------------------------------------------*/

  if ( sc->state == SLEEPCTL_SLEEP_STATE )
  {

    /* Capture the timestamp when the wakeup ISR starts */
    sc->wakeup.isr_start_timetick = timetick_get(); 
      
    /* Reset the CPU adjustment callback. Must be registered for each arming. */
    sc->adjust_cb   = NULL;
    sc->conflict_id = SLEEPCTL_NUM_CONTROLLERS;

    /* Enable sleep controller's fast clock regime, for FINE counting */
    if ( !sc->warmup_enforced )
    {
      ENABLE_FAST_CLKRGM(id);
    }

    /* Determine how long the sleep controller has slept for */
    sclk = sleepctl_get_sclk_cnt(id);

    TRACE(id, WAKEUP_INT, sclk>=sc->wakeup.sclk, sclk, sc->wakeup.sclk);

    /* Reasonable wakeup? */
    if ( ( sclk >= sc->wakeup.sclk ) || ( !sc->warmup_enforced && (sclk == 0) ))
    {
      /* Update the sleep controller's software state to "wakeup" state */
      sc->state = SLEEPCTL_WAKEUP_STATE;

      /* Invoke the wakeup callback */
      if (sc->wakeup.cb != NULL )
      {
        sc->wakeup.cb();
      }

      /* If this ISR is excessively latent, print error */
      if ( sc->wakeup.isr_start_timetick - sc->wakeup.int_timetick >=
           ISR_LATENCY_THRESH ) 
      {
        DBG_STR2( ERROR, "%s Wakeup, latency: %d sclks", sc->id,
               sc->wakeup.isr_start_timetick - sc->wakeup.int_timetick);
      }
      else
      {
        DBG_STR2( MED, "%s Wakeup, latency: %d sclks", sc->id,
               sc->wakeup.isr_start_timetick - sc->wakeup.int_timetick);
      }
    }
    else
    {
      DBG_STR3( ERROR, "%s Wakeup ignored!  sclk=%lu, wake=%lu",
             sc->id, sclk, sc->wakeup.sclk);
    }

    /* Capture the timestamp when the wakeup ISR ends */
    sc->wakeup.isr_end_timetick = timetick_get(); 

    TRACE(id, INT_PROFILE, sc->wakeup.int_timetick, 
          sc->wakeup.isr_start_timetick, sc->wakeup.isr_end_timetick); 

    /* Automatically transition to warmup if it is not enforced */
    if ( !sc->warmup_enforced )
    {
      sc->state = SLEEPCTL_WARMUP_STATE;
    }
  }
  else
  {
    spurious_wakeup_int_cnt++;
    DBG_2(ERROR, "Wakeup in wrong state %d, cnt=%d", sc->state, 
          spurious_wakeup_int_cnt);
  }

  #ifdef WAKEUP_LEVEL_TRIGGERED
  /* Wait for interrupt line to de-assert. */
  tramp_block_till_deasserted( sleepctl_const[id].isr.wakeup_id, 33 /* us */);
  #endif /* !WAKEUP_LEVEL_TRIGGERED */

} /* sleepctl_wakeup_isr() */



/*============================================================================

FUNCTION SLEEPCTL_ENDING_ISR

DESCRIPTION
  This function is called on the Sleep Controller's "Sleep Ending" interrupt.
  It calls the registered callback function for the event.

DEPENDENCIES
  Must only be called in response to the Sleep Ending interrupt

RETURN VALUE
  None

SIDE EFFECTS
  Calls the sleep controller's "sleep ending" callback.

============================================================================*/

static void sleepctl_ending_isr
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

  /* Spurious interrupt counter */
  static uint32                   spurious_ending_int_cnt = 0;

/*--------------------------------------------------------------------------*/

  /* If warmups not enforced, ensure wakeup is processed before proceeding */
  if ( sc->state == SLEEPCTL_SLEEP_STATE && !sc->warmup_enforced )
  {
    sleepctl_wakeup_isr( id );
  }

  if ( sc->state == SLEEPCTL_WARMUP_STATE )
  {
    /* Capture the timestamp when the ending ISR starts */
    sc->ending.isr_start_timetick = timetick_get(); 

    /* If this ISR is excessively latent, print error */
    if ( sc->ending.isr_start_timetick - sc->ending.int_timetick >= 
         ISR_LATENCY_THRESH ) 
    {
      DBG_STR2( ERROR, "%s Sleep Ending (rtc on), latency: %d sclks", sc->id,
                sc->ending.isr_start_timetick - sc->ending.int_timetick );
    }
    else
    {
      DBG_STR2( MED, "%s Sleep Ending (rtc on), latency: %d sclks", sc->id,
                sc->ending.isr_start_timetick - sc->ending.int_timetick );
    }

    /* Disable sleep controller's clock regimes - we're no longer sleeping */
    DISABLE_FAST_CLKRGM(id);
    DISABLE_SLOW_CLKRGM(id);

    BEGIN_REG_ACCESS();
    DISABLE_SLEEP_INT(id);
    END_REG_ACCESS();

    /* Update the sleep controller's software state to "inactive" state */
    sc->state = SLEEPCTL_INACTIVE_STATE;

    /* Record the sleep duration, for error_feedback */
    sc->last_sleep.sys_clk = sc->total_sys_clk;
    sc->last_sleep.sclk    = sc->ending.sclk;

    /* Invoke the sleep ending callback */
    if (sc->ending.cb != NULL )
    {
      sc->ending.cb();
    }

    TRACE(id, ENDING_INT, sc->ending.fine, sc->ending.sclk, sc->total_sys_clk);

    /* Record the timestamp of the end of this ISR */
    sc->ending.isr_end_timetick = timetick_get(); 

    TRACE(id, INT_PROFILE, sc->ending.int_timetick, 
          sc->ending.isr_start_timetick, sc->ending.isr_end_timetick);
  }
  else
  {
    /* We might get an occassional spurious RTC On interrupt 
       when 1x bails after no RF lock.  Note it and ignore it. */
    spurious_ending_int_cnt++;
    DBG_2(ERROR, "Sleep Ending in wrong state %d, cnt=%d", sc->state, 
          spurious_ending_int_cnt);
  }

} /* sleepctl_ending_isr() */



/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/



/*============================================================================

FUNCTION SLEEPCTL_SET_START_CALLBACK

DESCRIPTION
  This function sets the callback function to call when the sleep start point 
  is reached.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void sleepctl_set_start_callback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep start callback */
  sleepctl_event_callback_type    start_cb
)
{

/*--------------------------------------------------------------------------*/

  /* Save the sleep start event callback */
  sleepctl[id].start.cb = start_cb;

} /* sleepctl_set_start_callback( ) */



/*============================================================================

FUNCTION SLEEPCTL_SET_WAKEUP_CALLBACK

DESCRIPTION
  This function sets the callback function to call when the wakeup point is
  reached.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void sleepctl_set_wakeup_callback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Wakeup callback */
  sleepctl_event_callback_type    wakeup_cb
)
{

/*--------------------------------------------------------------------------*/

  /* Save the wakeup event callback */
  sleepctl[id].wakeup.cb = wakeup_cb;

} /* sleepctl_set_wakeup_callback( ) */



/*============================================================================

FUNCTION SLEEPCTL_SET_ENDING_CALLBACK

DESCRIPTION
  This function sets the callback function to call when the ending point
  (rtc on) is reached.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void sleepctl_set_ending_callback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Wakeup callback */
  sleepctl_event_callback_type    ending_cb
)
{

/*--------------------------------------------------------------------------*/

  /* Save the ending event callback */
  sleepctl[id].ending.cb = ending_cb;

} /* sleepctl_set_ending_callback( ) */


/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK

DESCRIPTION
  Return the number of sleep clocks the sleep controller has slept for.

DEPENDENCIES
  The given sleep controller must be running.
  Must be called from inside INTLOCK and BEGIN/END_REG_ACCESS contexts.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

static uint32 sleepctl_get_sclk
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk1, sclk2;

/*--------------------------------------------------------------------------*/

  /* Read the number of sleep clocks the controller has slept for */
  sclk1 = SLEEP_COUNT_COARSE(id);

  /* Read it again */
  sclk2 = SLEEP_COUNT_COARSE(id);

  /* If the values are different, we sampled when the count was changing. */
  if ( sclk1 != sclk2)
  {
    /* Read the value a third time, and use it.  It should be stable now. */
    sclk1 = SLEEP_COUNT_COARSE(id);
  }

  return (sclk1);

} /* sleepctl_get_sclk() */


/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK_COUNT

DESCRIPTION
  Return the number of sleep clocks the sleep controller has slept for.

DEPENDENCIES
  The given sleep controller must be running.
  Must be called from inside INTLOCK and BEGIN/END_REG_ACCESS contexts.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

static uint32 sleepctl_get_sclk_count
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
/*--------------------------------------------------------------------------*/

  return (sleepctl_get_sclk(id) + sleepctl[id].restart.sclk_adj);

} /* sleepctl_get_sclk_count() */



/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK_CNT

DESCRIPTION
  INTLOCK and return the number of sleep clocks the sleep controller has 
  slept for.

DEPENDENCIES
  The given sleep controller must be running.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sclk_cnt
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk;

/*--------------------------------------------------------------------------*/

  INTLOCK();
  BEGIN_REG_ACCESS_IN_INTLOCK();

  sclk = sleepctl_get_sclk_count( id );

  END_REG_ACCESS_IN_INTLOCK();
  INTFREE();

  return sclk;

} /* sleepctl_get_sclk_cnt() */



/*===========================================================================

FUNCTION SLEEPCTL_FORCE_WAKEUP

DESCRIPTION
  This function wakes up the sleep controller as soon as possible. The
  function will not return until the sleep controller is awake.

DEPENDENCIES
  Must be called from an INTLOCK'd context.
  Sleep controller must be reinitialized afterwards, to restore ISR's.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void sleepctl_force_wakeup
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk;

  /* Number of residual sys_clk's the sleep controller has slept for */
  uint32                          fine;

  /* Loop counter */
  uint32                          cnt = 0;
  
  /* Sleep controller status */
  uint32                          status;

/*--------------------------------------------------------------------------*/

  #ifdef SINGLE_INTERRUPT

  /* Clear the interrupt service routine for this sleep controller */
  tramp_set_isr( sleepctl_const[id].isr.isr_id, NULL );

  #else /* !SINGLE_INTERRUPT */
  
  /* Clear the interrupt service routines for this sleep controller */
  tramp_set_isr( sleepctl_const[id].isr.wakeup_id, NULL );
  tramp_set_isr( sleepctl_const[id].isr.ending_id, NULL );

  #endif /* SINGLE_INTERRUPT */

  DISABLE_SLEEP_INT(id);

  /* Enable sleep controller's fast clock regime, for FINE counting */
  ENABLE_FAST_CLKRGM(id);

  BEGIN_REG_ACCESS_IN_INTLOCK();

  /* Determine how long we have been asleep for */
  sclk = sleepctl_get_sclk( id );
  fine = SLEEP_COUNT_FINE( id );
  status = sleepctl_get_status( id );

  /* Only if we are not already nearing the end do we reprogram */
  if ( (sc->ending.sclk - sclk) > MIN_REPROG_SCLK )
  {
    /* If the sleep controller is not already in the warmup state ... */
    if ( status != SLEEPCTL_WARMUP )
    {
      /* ... program a wakeup event for a little in the future */
      sclk = SET_WAKEUP_COARSE(id, sclk + WAKE_NOW_WAKEUP_COARSE );
    }

    /* Program the sleep ending event for a little after that */
    SET_EXPIRE_FINE  (id, fine + WAKE_NOW_ENDING_FINE  );
    SET_EXPIRE_COARSE(id, sclk + WAKE_NOW_ENDING_COARSE );
  }

  TRACE(id, FORCE_WAKEUP, status, sclk, fine);

  /* Wait until the sleep controller is not active anymore. */
  while( sleepctl_get_status( id ) != SLEEPCTL_INACTIVE )
  {
    if (++cnt > WAKE_NOW_MAX_LOOP )
    {
      ERR_FATAL("Unable to stop sleep controller!", id, 0, 0);
    }

    /* Wait for about 1 slow clock */
    clk_busy_wait( WAKE_NOW_WAIT_US );
  }


  END_REG_ACCESS_IN_INTLOCK();

} /* sleepctl_force_wakeup() */



#ifdef FEATURE_SLEEPCTL_PC
#error code not present
#endif /* FEATURE_SLEEPCTL_PC */

/*============================================================================

FUNCTION SLEEPCTL_INIT_CONTROLLER

DESCRIPTION
  This function initializes a Sleep Controller

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt service handlers are installed.
  Sleep controller clock regimes may be turned off.

============================================================================*/

static void sleepctl_init_controller
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  /* Initialize the sleep controller's state */
  sc->state = SLEEPCTL_INACTIVE_STATE;

  #ifndef T_QUASAR

  #ifdef SINGLE_INTERRUPT
  
  /* Install the sleep controller's interrupt service routine */
  tramp_set_isr( sleepctl_const[id].isr.isr_id, sleepctl_const[id].isr.isr );

  #else /* !SINGLE_INTERRUPT */
  
  /* Install wakeup & sleep ending interrupt service routines */
  tramp_set_isr( sleepctl_const[id].isr.wakeup_id, 
                 sleepctl_const[id].isr.wakeup_isr );
  tramp_set_isr( sleepctl_const[id].isr.ending_id, 
                 sleepctl_const[id].isr.ending_isr );

  #endif /* SINGLE_INTERRUPT */
  
  #endif /* T_QUASAR */


  BEGIN_REG_ACCESS();  

  /* Program the sleep controller registers with near-maximum times,
     to prevent a spurious wakeup interrupt. */
  SET_WAKEUP_COARSE_MAX(id);
  SET_EXPIRE_COARSE_MAX(id);
  SET_EXPIRE_FINE_MAX(id);

  END_REG_ACCESS();


  /* Disable sleep controller's clock regimes - we're not sleeping */
  DISABLE_FAST_CLKRGM(id);
  DISABLE_SLOW_CLKRGM(id);

  /* Default warmup enforced */
  sc->warmup_enforced = TRUE;

  /* Use the maximum RF warmup time as a default RF warmup time */
  if ( sc->warmup.usec == 0 )
  {
    sleepctl_set_warmup(id, RF_WARMUP_MAX_USEC );
  }

} /* sleepctl_init_controller() */



/*============================================================================

FUNCTION SLEEPCTL_INIT

DESCRIPTION
  This function initializes the Sleep Controller(s)

DEPENDENCIES
  TRAMP must be initialized

RETURN VALUE
  None

SIDE EFFECTS
  Installs ISR's for the sleep controller(s)

============================================================================*/

void sleepctl_init( void )
{
  /* Loop index */
  int                             i;

  /* Sleep controller Id */
  sleepctl_id_type                id;

  /* Sleep controller status */
  uint32                          status;

/*--------------------------------------------------------------------------*/

  #ifdef FEATURE_SLEEPCTL_PC
#error code not present
  #endif /* FEATURE_SLEEPCTL_PC */

  #ifndef WAKEUP_LEVEL_TRIGGERED

  /* Set interrupt controller to use edge triggered interrupts */

  #if defined( T_MSM7500 ) || defined( T_QSC60X0 )  || defined( T_QSC60X5 )

    /* Always edge-sensitive; nothing to set. */
  
  #elif defined( T_MSM6700 )
#error code not present
  #elif defined( T_MSM6550 )
#error code not present
  #endif /* T_MSMxxxx */
  
  
  #endif /* WAKEUP_LEVEL_TRIGGERED */

  /* Initialize all the sleep controllers */
  for (i = 0; i < SLEEPCTL_NUM_CONTROLLERS; i++ )
  {
    /* Convert from integer to enumeration */
    id = (sleepctl_id_type) i;


    /* Ensure sleep controller is in an inactive state */

    INTLOCK();

    BEGIN_REG_ACCESS_IN_INTLOCK();
    status = sleepctl_get_status(id);
    END_REG_ACCESS_IN_INTLOCK();

    if ( status != SLEEPCTL_INACTIVE )
    {
      sleepctl_force_wakeup(id);

      MSG_HIGH("Sleepctl #%d was running at startup! Resetting to inactive", 
                id, status, 0);
    }

    INTFREE();


    /* Initialize the sleep controller software state information */
    sleepctl_init_controller(id);
  }

} /* sleepctl_init() */



/*============================================================================

FUNCTION SLEEPCTL_PROGRAM

DESCRIPTION
  This function is called to arm or start the sleep controller.
  The sleep duration is set to near-maximum (~1.5 days).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep controller will start running
    a) immediately, or
    b) on the next event boundary, which will be within ...
    26.7ms for the 1x controller (immediately if manually triggered)
    1.67ms for the HDR controller

============================================================================*/

static void sleepctl_program
(
  /* Sleep controller to be armed */
  sleepctl_id_type                id,

  /* Immediate start, or just armed for next event boundary */
  boolean                         start_immediately
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  /* Enable sleep controller's clock regimes */
  ENABLE_FAST_CLKRGM(id);
  ENABLE_SLOW_CLKRGM(id);

  INTLOCK();

  /* Check the software state of the sleep controller */
  if ( sc->state != SLEEPCTL_INACTIVE_STATE)
  {
    ERR_FATAL("Sleep controller #%d is active! state = %d",
              id, sc->state, 0);
  }

  /* Prepare to program the sleep controller for near-maximum time */

  sc->total_sys_clk = TOTAL_SYS_CLK_MAX;
  sc->wakeup.sclk   = WAKEUP_COARSE_MAX;
  sc->ending.sclk   = EXPIRE_COARSE_MAX;
  sc->ending.fine   = EXPIRE_FINE_MAX;

  /* Initialize the pwr collapse info */
  sc->restart.timetick = 0;
  sc->restart.fine     = 0;
  sc->restart.fine_adj = 0;
  sc->restart.sclk_adj = 0;

  BEGIN_REG_ACCESS_IN_INTLOCK();

  /* Check the state of the sleep controller hardware */
  if ( sleepctl_get_status(id) != SLEEPCTL_INACTIVE )
  {
    ERR_FATAL("Sleep controller #%d h/w is active!", id, 0, 0);
  }

  /* Program the sleep controller for near-maximum time */

  SET_WAKEUP_COARSE_MAX(id);
  SET_EXPIRE_COARSE_MAX(id);
  SET_EXPIRE_FINE_MAX(id);

  ENABLE_SLEEP_INT(id);

  if ( start_immediately )
  {
    /* Program sleep to start immediately */
    START_SLEEP(id);

    TRACE(id, START_SLEEP, sc->ending.fine, sc->ending.sclk, sc->wakeup.sclk);
  }
  else
  {
    /* Set the ARM bit, to start sleep on the next event boundary */
    ARM_SLEEP(id);

    TRACE(id, ARM_SLEEP, sc->ending.fine, sc->ending.sclk, sc->wakeup.sclk);
  }

  /* Update the sleep controller's software state to "sleeping" */
  sc->state = SLEEPCTL_ARMED_STATE;

  END_REG_ACCESS_IN_INTLOCK();

  /* Capture the slow-clock counter when sleep is armed */
  sc->arm.timetick = timetick_get_safe();

  /* Start long-duration frequency measurements */
  sclk_begin_error_adjust( sc->sclk_system);


  INTFREE();

} /* sleepctl_program() */



/*============================================================================

FUNCTION SLEEPCTL_ARM

DESCRIPTION
  This function is called to arm the sleep controller.
  The sleep duration is set to near-maximum (~1.5 days).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep controller will start running on the next event boundary,
  which will be within ...
    26.7ms for the 1x controller (unless manually manually triggered)
    1.67ms for the HDR controller

============================================================================*/

void sleepctl_arm
(
  /* Sleep controller to be armed */
  sleepctl_id_type                id
)
{
/*--------------------------------------------------------------------------*/

  sleepctl_program( id, FALSE /* Just arm, don't force an immediate start */ );

} /* sleepctl_arm() */


/*============================================================================

FUNCTION SLEEPCTL_START

DESCRIPTION
  This function is called to start the sleep controller.
  The sleep duration is set to near-maximum (~1.5 days).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The sleep controller will start running immediately

============================================================================*/

void sleepctl_start
(
  /* Sleep controller to be started */
  sleepctl_id_type                id
)
{
/*--------------------------------------------------------------------------*/

  sleepctl_program( id, TRUE /* Force an immediate start */ );

} /* sleepctl_start() */



/*============================================================================

FUNCTION SLEEPCTL_BLOCK_UNTIL_SLEEPING

DESCRIPTION
  This function waits until the sleep controller is counting sleep clocks,
  indicating that the sys_clk clock source (eg, TCXO) is no longer required.

DEPENDENCIES
  Must only be called after the sleep controller has been armed, and has
  been triggered, or the trigger event will occur within 2ms (such as an HDR
  1.67ms slot boundary)

RETURN VALUE
  None

SIDE EFFECTS
  Consumes CPU time until the sleep controller starts

============================================================================*/

void sleepctl_block_until_sleeping
(
  /* Sleep controller to monitor */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

  /* Safety timer - to avoid endless looping */
  int32                           usec = 0;

/*--------------------------------------------------------------------------*/

  INTLOCK();

  TRACE(id, BLOCK_UNTIL, sc->state, 0, 0);

  if ( sc->state == SLEEPCTL_ARMED_STATE )
  {
    BEGIN_REG_ACCESS_IN_INTLOCK();

    while ( (sleepctl_get_status(id) & SLEEPCTL_COUNT_SCLK) == 0)
    {
      if ( usec >= 4000 )
      {
        DBG_STR2( ERROR, "%s sleep controller not started in > %d us",
               sc->id, usec);
        break;
      }
  
      /* Wait a short while before checking again */
      clk_busy_wait( 100 );
  
      /* Increment length of time we've waited for */
      usec += 100;
    }

    END_REG_ACCESS_IN_INTLOCK();

    TRACE(id, BLOCK_UNTIL, sc->state, usec, 1);

    /* Advance state from "ARMED" to "SLEEPING" */
    sleepctl_start_event( id );
  }

  INTFREE();

} /* sleepctl_block_until_sleeping() */



/*============================================================================

FUNCTION SLEEPCTL_COMMIT_WAKEUP_POINT_UPDATE

DESCRIPTION
  This function sets the wakeup point based on the warmup duration and
  the sleep controller computed expiry point.

  The sleep controller's h/w expiry point is not changed.

DEPENDENCIES
  Sleep controller must be running.
  sc->warmup.sclk must be set.
  sc->ending.sclk must be set.

RETURN VALUE
  None

SIDE EFFECTS
  Changes the sleep controller's h/w wakeup point (sc->wakeup.sclk).
  Switches the sleep controller's s/w state back to SLEEP state.

============================================================================*/

static void sleepctl_commit_wakeup_point_update
(
  /* Sleep controller to monitor */
  sleepctl_id_type                id,

  /* Sleep controller */
  sleepctl_ptr                    sc
)
{
  /* Elapsed sleep duration, in slow clocks */
  uint32                          sclk;

/*--------------------------------------------------------------------------*/

  /* Compute new wakeup point */
  if ( sc->ending.sclk <= sc->warmup.sclk )
  {
    ERR_FATAL("Sleep %d <= warmup %d", sc->ending.sclk, sc->warmup.sclk, 0);
  }
  sc->wakeup.sclk = sc->ending.sclk - sc->warmup.sclk;

  /* Capture the timestamp when the wakeup int is expected to occur */
  sc->wakeup.int_timetick = sc->start.timetick + sc->wakeup.sclk;

  INTLOCK();
  BEGIN_REG_ACCESS_IN_INTLOCK();

  /* Check the state of the sleep controller hardware */
  if ( sleepctl_get_status(id) == SLEEPCTL_INACTIVE )
  {
    ERR_FATAL("Sleep controller #%d h/w is not active!", id, 0, 0);
  }

  /* Make sure we aren't trying to set the warmup point in the past */
  sclk = sleepctl_get_sclk_count(id);

  TRACE(id, UPDATE_WAKEUP, sc->state, sclk, sc->wakeup.sclk);

  if ( sc->wakeup.sclk < sclk )
  {
    ERR_FATAL("Wakeup point is in the past! n=%d w=%d s=%d",
              sclk, sc->wakeup.sclk, sc->ending.sclk);
  }

  /* Program new wakeup point */
  SET_WAKEUP_COARSE(id, sc->wakeup.sclk - sc->restart.sclk_adj);

  /* If warmup is not enforced, go ahead and prog ending point, otherwise
     wait for wakeup int before proceeding with ending programming */
  if ( !sc->warmup_enforced )
  {
    /* Program new ending points */
    SET_EXPIRE_FINE  (id, (uint16)(sc->ending.fine + sc->restart.fine_adj) - 1);
    SET_EXPIRE_COARSE(id, (sc->ending.sclk - sc->restart.sclk_adj) - 2);
  }

  END_REG_ACCESS_IN_INTLOCK();

  /* Capture the slow-clock counter when wakeup point is changed */
  sc->rearm.timetick = timetick_get_safe(); 

  INTFREE();

  #ifdef FEATURE_SLEEPCTL_PC
#error code not present
  #endif /* FEATURE_SLEEPCTL_PC */

  /* If the wakeup point is being updated in response to a "wakeup" ... */
  if ( sc->state == SLEEPCTL_WAKEUP_STATE )
  {
    /* ... return the sleep controller's software state to "sleeping" */
    sc->state = SLEEPCTL_SLEEP_STATE;
  }

} /* sleepctl_commit_wakeup_point_update( ) */



/*============================================================================

FUNCTION SLEEPCTL_GET_CPU_CONFLICT_ADJUST

DESCRIPTION
  This function calculates the adjusted sleep in sys_clks using the wakeup
  information for a given sleepctl_id_type.  A callback function is called
  with the new wakeup info and the conflicting wakeup info passed as
  arguments to calculate the new sleep time in sys_clks.  This function is useful,
  e.g., for smaller sleep cycles, when it is necessary to calculate if there
  is enough time to sleep (before committing to sleep) based on any CPU
  conflict adjustments.

INPUTS
  id                    - Sleepctl id
  sleep_dur_sysclks     - Sleep duration in sys_clks
  adjust_cb             - Callback which will calculate the sleep time adjustment

DEPENDENCIES
  adjust_cb must return an adjustment time in sclks (uint32)

RETURN VALUE
  Sleep time adjustment in sclks

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_cpu_conflict_adjust
(
  /* Sleep controller id */
  sleepctl_id_type                      id,

  /* Sleep duration, in sys_clks */
  uint32                                sleep_dur_sysclks,

  /* Callback which will calculate the sleep time adjustment */
  sleepctl_adjust_wakeup_callback_type  adjust_cb
)
{
  /* Convenience pointer to adjusting client */
  sleepctl_ptr                    sc_ptr;

  /* Convenience pointer to conflicting client */
  sleepctl_ptr                    conflict_sc_ptr;

  /* Virtual wakeup info for the client */
  sleepctl_wakeup_info_type       wi;

  /* Wakeup info for the conflicting client */
  sleepctl_wakeup_info_type       conflict_wi;

  /* Ending sclk for adjusting client */
  uint32                          sc_ending_sclk;

  /* Sleep end hardware interrupt timetick, in timetick counter units */
  timetick_type                   sc_ending_int_timetick;

/*--------------------------------------------------------------------------*/

  if ( ( id < SLEEPCTL_NUM_CONTROLLERS ) &&
       ( sleepctl[id].conflict_id < SLEEPCTL_NUM_CONTROLLERS ) &&
       ( adjust_cb != NULL ) )
  {

    /* Set the convenience pointer to the adjusting sleep controller, but do
       not change real client's sleep controller. */
    sc_ptr = &(sleepctl[id]);

    /* Convert sys_clk's to sleep clocks & sys_clk residual. */
    sc_ending_sclk = sclk_from_sysclks(sc_ptr->sclk_system,
                                       sleep_dur_sysclks,
                                       &sc_ptr->ending.fine);

    /* Capture the timestamp when the ending int is expected to occur */
    /* Add one extra sclk to (roughly) account for cx8 (ending.fine) time */
    sc_ending_int_timetick = sc_ptr->start.timetick + sc_ending_sclk + 1;

    /* Load the relevant info about the client's wakeup point */
    wi.wakeup_timetick     = sc_ending_int_timetick - sc_ptr->warmup.sclk;
    wi.ending_timetick     = sc_ending_int_timetick;
    wi.starting_timetick   = sc_ptr->start.timetick;

    /* Set the convenience pointer to the conflicting sleep controller */
    conflict_sc_ptr = &(sleepctl[sc_ptr->conflict_id]);

    /* Load the relevant info about the conflicting client's wakeup point */
    conflict_wi.wakeup_timetick   = conflict_sc_ptr->ending.int_timetick -
                                      conflict_sc_ptr->warmup.sclk;
    conflict_wi.ending_timetick   = conflict_sc_ptr->ending.int_timetick;
    conflict_wi.starting_timetick = conflict_sc_ptr->start.timetick;

    return adjust_cb( &wi, &conflict_wi, FALSE );
      /* Return the conflict adjusted sleep time in sys_clks.*/
  }
  else
  {
    /* Default to current sleep_time (passed in) if either sleep id is invalid */
    return sleep_dur_sysclks;
  }

} /* sleepctl_get_cpu_conflict_adjust */



/*============================================================================

FUNCTION SLEEPCTL_CALL_ADJUST_WAKEUP_CB

DESCRIPTION
  Call the callback function for the given sleep controller and return
  whether there has been a change in the wakeup point.  

DEPENDENCIES
  None

RETURN VALUE
  Whether there has been a change in the wakeup point requiring the change
  to be committed to hardware.

SIDE EFFECTS
  None

============================================================================*/

static boolean sleepctl_call_adjust_wakeup_cb
(
  /* Sleep controller for the adjusting client */
  sleepctl_ptr                    adjust_sc,

  /* Sleep controller for the conflicting client */
  sleepctl_ptr                    conflict_sc
)
{
  /* Wakeup info for the adjusting client */
  sleepctl_wakeup_info_type       adjust_wi;

  /* Wakeup info for the conflicting client */
  sleepctl_wakeup_info_type       conflict_wi;

  /* The new wakeup duration requested by the adjusting client */
  uint32                          sys_clk;

  /* Whether an wakeup time adjustment has been made */
  boolean                         is_adjustment = FALSE;

/*--------------------------------------------------------------------------*/

  /* Load the relevant info about the adjusting client's wakeup point */
  adjust_wi.wakeup_timetick     = adjust_sc->ending.int_timetick - 
                                                     adjust_sc->warmup.sclk;
  adjust_wi.ending_timetick     = adjust_sc->ending.int_timetick;
  adjust_wi.starting_timetick   = adjust_sc->start.timetick;

  /* Load the relevant info about the conflicting client's wakeup point */
  conflict_wi.wakeup_timetick   = conflict_sc->ending.int_timetick - 
                                                     conflict_sc->warmup.sclk;
  conflict_wi.ending_timetick   = conflict_sc->ending.int_timetick;
  conflict_wi.starting_timetick = conflict_sc->start.timetick;

  /* Call the callback function and save the resulting time. */
  sys_clk = adjust_sc->adjust_cb( &adjust_wi, &conflict_wi, TRUE );

  /* If there is a change update the relevant information. */
  if ( sys_clk != adjust_sc->total_sys_clk )
  {
    /* Set the total sleep duration */
    adjust_sc->total_sys_clk = sys_clk;

    /* Convert sys_clk's to sleep clocks & sys_clk residual. */
    adjust_sc->ending.sclk = sclk_from_sysclks( adjust_sc->sclk_system,
                                                sys_clk,
                                                &adjust_sc->ending.fine);

    /* Capture the timestamp when the ending int is expected to occur */
    /* Add one extra sclk to (roughly) account for cx8 (ending.fine) time */
    adjust_sc->ending.int_timetick = adjust_sc->start.timetick + adjust_sc->ending.sclk + 1;

    /* Remember there was an adjustment for return purposes */
    is_adjustment = TRUE;
  }

  return is_adjustment;

} /* sleepctl_call_adjust_wakeup_cb */



/*============================================================================

FUNCTION SLEEPCTL_UPDATE_WAKEUP_POINT

DESCRIPTION
  This function sets the wakeup point based on the warmup duration and the
  sleep controller computed expiry point after first adjusting all wakeups
  which may be affected by this wakeup point update. This may be done due 
  to limited CPU resources to perform two wakeups at the same time.

DEPENDENCIES
  Sleep controller must be running.
  sc->warmup.sclk must be set.
  sc->ending.sclk must be set.

RETURN VALUE
  None

SIDE EFFECTS
  Changes the sleep controller's h/w wakeup point (sc->wakeup.sclk).
  Switches the sleep controller's s/w state back to SLEEP state.

============================================================================*/

static void sleepctl_update_wakeup_point
(
  /* Sleep controller to monitor */
  sleepctl_id_type                id,

  /* Sleep controller */
  sleepctl_ptr                    sc
)
{
  sleepctl_ptr                    adjust_sc;

  sleepctl_ptr                    conflict_sc;

  /* Loop index */
  int                             i;

/*--------------------------------------------------------------------------*/

  /* If the adjust callback is set for this client, give the client a chance
     to update the wakeup point due to a CPU conflict */
  if ( sc->adjust_cb ) 
  {
    /* Grab a pointer to the conflicting sleep controller */
    conflict_sc = &(sleepctl[sc->conflict_id]);

    /* Call the conflict adjustment callback of the calling client */
    sleepctl_call_adjust_wakeup_cb( sc, conflict_sc );
  }

  /* Are there any other sleep controllers which need to be adjusted due to 
     this controller's change in wakeup time? */
  for (i = 0; i < SLEEPCTL_NUM_CONTROLLERS; i++ )
  {
    /* Set the convenience pointer to the potential adjusting sleep 
       controller. */
    adjust_sc = &(sleepctl[i]);

    /* Does this sleep controller have an adjustment callback to call, is it sleeping,
       and is there a potential conflict with the calling client? */
    if ( ( adjust_sc->adjust_cb ) && ( adjust_sc->state == SLEEPCTL_SLEEP_STATE ) && 
         ( adjust_sc->conflict_id == id ) )
    {
      /* Set the convenience pointer to the conflicting sleep controller */
      conflict_sc = &(sleepctl[adjust_sc->conflict_id]);

      /* Call the conflict adjustment callback of the client */
      if ( sleepctl_call_adjust_wakeup_cb( adjust_sc, conflict_sc ) )
      {
        /* If an adjustment was made commit it to hardware */
        sleepctl_commit_wakeup_point_update( (sleepctl_id_type) i, adjust_sc );
      }
    }
  }

  /* Finally, commit this client's wakeup point to hardware */
  sleepctl_commit_wakeup_point_update( id, sc );

}



/*============================================================================

FUNCTION SLEEPCTL_SET_WARMUP

DESCRIPTION
  This function sets the interval between the wakeup interrupt and the
  sleep ending interrupt.

  The warmup duration will be subtracted from the expiry point to get the
  wakeup point.

DEPENDENCIES
  Valid slow clock estimate.

RETURN VALUE
  None

SIDE EFFECTS
  If the sleep controller is running, the wakeup point is reprogrammed,
  and the sleep controller's s/w state changed back to the SLEEP state.

============================================================================*/

void sleepctl_set_warmup
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Warmup time, in microseconds */
  uint32                          usec
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  /* Enforce a reasonable worst case value */
  if ( usec > RF_WARMUP_MAX_USEC )
  {
    usec = RF_WARMUP_MAX_USEC;
  }

  /* Convert microseconds to slow clocks only if duration has changed */
  if ( sc->warmup.usec != usec )
  {
    sc->warmup.usec = (uint16) usec;
    sc->warmup.sclk = (uint16) timetick_cvt_to_sclk( usec, T_USEC );

    INTLOCK();

    TRACE(id, SET_WARMUP, usec, sc->warmup.sclk, 0);

    /* Check the software state of the sleep controller.
       Only update the wakeup point if we're in the ARMED or SLEEP state. */
    if ( sc->state == SLEEPCTL_ARMED_STATE ||
         sc->state == SLEEPCTL_SLEEP_STATE    )
    {
      /* Only update the warmup point if a duration has been specified. */
      if ( sc->ending.sclk != EXPIRE_COARSE_MAX )
      {
        /* Update the wakeup point based on the new warmup time */
        sleepctl_update_wakeup_point(id, sc);
      }
    }
  
    INTFREE();
  }

} /* sleepctl_set_warmup() */



/*============================================================================

FUNCTION SLEEPCTL_SET_DURATION_SYS_CLK

DESCRIPTION
  This function computes the point when the sleep controller should expire,
  and sets the appropriate wakeup point to the hardware.

  The sleep controller expiry point is left at ~1.5 days.

DEPENDENCIES
  Valid slow clock estimate.
  sleepctl_set_warmup( ) must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_duration_sys_clk
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep duration, in chips */
  uint32                          sys_clk
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  /* Set the total sleep duration */
  sc->total_sys_clk = sys_clk;

  /* Convert sys_clk's to sleep clocks & sys_clk residual. */
  sc->ending.sclk = sclk_from_sysclks(sc->sclk_system,
                                      sys_clk,
                                      &sc->ending.fine);

  /* Capture the timestamp when the ending int is expected to occur */
  /* Add one extra sclk to (roughly) account for cx8 (ending.fine) time */
  sc->ending.int_timetick = sc->start.timetick + sc->ending.sclk + 1;

  INTLOCK();

  TRACE(id, SET_DURATION, sc->state, sc->ending.sclk, sys_clk);

  /* Software state of the sleep controller must be ARMED, SLEEP, or WAKEUP */
  if ( sc->state >= SLEEPCTL_ARMED_STATE  &&
       sc->state <= SLEEPCTL_WAKEUP_STATE    )
  {
    /* Update the wakeup point based on the new sleep duration */
    sleepctl_update_wakeup_point(id, sc);
  }
  else
  {
    ERR_FATAL("Sleep controller #%d in incorrect s/w state!", id, 0, 0);
  }

  INTFREE();

} /* sleepctl_set_duration_sys_clk( ) */



/*============================================================================

FUNCTION SLEEPCTL_SET_DURATION

DESCRIPTION
  This function computes the point when the sleep controller should expire,
  and sets the appropriate wakeup point to the hardware.

  The sleep controller expiry point is left at ~1.5 days.

DEPENDENCIES
  Valid slow clock estimate.
  sleepctl_set_warmup( ) must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_duration
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep duration, in chips */
  uint32                          chips
)
{

/*--------------------------------------------------------------------------*/

  /* Set the sleep duration, converting chips to sys_clk's */
  sleepctl_set_duration_sys_clk(id, chips << CHIPX8_TO_CHIP_SHIFT);

} /* sleepctl_set_duration( ) */



/*============================================================================

FUNCTION SLEEPCTL_SET_DURATION_MS

DESCRIPTION
  This function computes the point when the sleep controller should expire,
  and sets the appropriate wakeup point to the hardware.

  The sleep controller expiry point is left at ~1.5 days.

DEPENDENCIES
  Valid slow clock estimate.
  sleepctl_set_warmup( ) must have been called.
  
  *** Deprecated ***
  This function will be removed when INACTIVE state changes are implemented

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_duration_ms
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Sleep duration, in milliseconds */
  uint32                          ms
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  /* Set the total sleep duration */
  sc->total_sys_clk = TOTAL_SYS_CLK_MAX;

  /* Convert milliseconds to sleep clocks */
  sc->ending.sclk = TIMETICK_SCLK_FROM_MS(ms);

  /* Set a valid residual sys_clk end point */
  sc->ending.fine = EXPIRE_FINE_MAX;


  INTLOCK();

  TRACE(id, SET_DURATION_MS, sc->state, sc->ending.sclk, 0);

  /* Software state of the sleep controller must be ARMED, SLEEP, or WAKEUP */
  if ( sc->state >= SLEEPCTL_ARMED_STATE  &&
       sc->state <= SLEEPCTL_WAKEUP_STATE    )
  {
    /* Update the wakeup point based on the new sleep duration */
    sleepctl_update_wakeup_point(id, sc);
  }
  else
  {
    ERR_FATAL("Sleep controller #%d in incorrect s/w state!", id, 0, 0);
  }

  INTFREE();

} /* sleepctl_set_duration_ms( ) */



/*============================================================================

FUNCTION SLEEPCTL_SET_MAX_DURATION

DESCRIPTION
  This function sets the wakeup point to ~1.5 days.

DEPENDENCIES
  Sleep controller must be running

RETURN VALUE
  None

SIDE EFFECTS
  Changes the wakeup point.

============================================================================*/

void sleepctl_set_max_duration
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  INTLOCK();


  /* Program the sleep controller for near-maximum time */
  sc->total_sys_clk = TOTAL_SYS_CLK_MAX;
  sc->wakeup.sclk   = WAKEUP_COARSE_MAX;
  sc->ending.sclk   = EXPIRE_COARSE_MAX;
  sc->ending.fine   = EXPIRE_FINE_MAX;


  /* Check the software state of the sleep controller */
  if ( sc->state != SLEEPCTL_SLEEP_STATE  &&
       sc->state != SLEEPCTL_WAKEUP_STATE    )
  {
    ERR_FATAL("Sleep controller #%d in incorrect s/w state!", id, 0, 0);
  }

  BEGIN_REG_ACCESS_IN_INTLOCK();

  /* Check the state of the sleep controller hardware */
  if ( sleepctl_get_status(id) == SLEEPCTL_INACTIVE )
  {
    ERR_FATAL("Sleep controller #%d h/w is not active!", id, 0, 0);
  }

  /* Program new wakeup point */
  SET_WAKEUP_COARSE_MAX(id);

  END_REG_ACCESS_IN_INTLOCK();

  /* Capture the slow-clock counter when sleep duration is changed */
  sc->rearm.timetick = timetick_get_safe(); 

  TRACE(id, SET_DURATION_MS, sc->state, sc->wakeup.sclk, 0);

  /* Update the sleep controller's software state to "sleeping" */
  sc->state = SLEEPCTL_SLEEP_STATE;


  INTFREE();

} /* sleepctl_set_max_duration( ) */



/*============================================================================

FUNCTION SLEEPCTL_SET_CPU_CONFLICT_ADJUSTMENT_CALLBACK

DESCRIPTION
  This function sets the callback function to call when either of the two
  listed sleep controllers are programmed. The callback will adjust id's
  sleep time to account for the CPU conflict.

DEPENDENCIES
  Must be called prior to any conflicting wakeup is scheduled. This also
  must be called for earch sleep controller arming as it is only active for
  the next wakeup before it is reset.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void sleepctl_set_cpu_conflict_adjustment_callback
(
  /* Sleep controller which will adjust for a CPU conflict */
  sleepctl_id_type                      id,

  /* Conflicting sleep controller */
  sleepctl_id_type                      conflict_id,

  /* Callback which will make the sleep time adjustment */
  sleepctl_adjust_wakeup_callback_type  adjust_cb
)
{
  /* Sleep controller structure */
  sleepctl_ptr                          adjust_sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  INTLOCK();

  /* Save the adjustment callback in the client's structure within
     a critical section */
  adjust_sc->adjust_cb                = adjust_cb;
  adjust_sc->conflict_id              = conflict_id;

  INTFREE();

} /* sleepctl_set_cpu_conflict_adjustment_callback() */



/*============================================================================

FUNCTION SLEEPCTL_IS_ACTIVE

DESCRIPTION
  This function returns whether the sleep controller is active or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if the sleep controller is running
  FALSE if the sleep controller is not running

SIDE EFFECTS
  None

============================================================================*/

boolean sleepctl_is_active
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
/*--------------------------------------------------------------------------*/

  return sleepctl[id].state != SLEEPCTL_INACTIVE_STATE  ?  TRUE  :  FALSE;

} /* sleepctl_is_sleeping() */



/*============================================================================

FUNCTION SLEEPCTL_GET_SYS_CLK_CNT

DESCRIPTION
  Return the number of sys_clk's the sleep controller has slept for.

DEPENDENCIES
  The given sleep controller must be running.
  Should only be called from an INTLOCK'd context.

RETURN VALUE
  Length of sleep, in sys_clk's

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sys_clk_cnt
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk;

  /* Number of residual sys_clk's the sleep controller has slept for */
  uint32                          fine;

  /* Total number of sys_clk's the sleep controller has slept for */
  uint32                          sys_clk;

/*--------------------------------------------------------------------------*/

  INTLOCK();
  BEGIN_REG_ACCESS_IN_INTLOCK();

  /* Determine how long we have been asleep for */
  sclk = sleepctl_get_sclk_count(id);
  fine = SLEEP_COUNT_FINE(id);

  END_REG_ACCESS_IN_INTLOCK();
  INTFREE();

  /* Convert slow clocks into sys_clk's */
  sys_clk = sclk_to_sysclks( sleepctl[id].sclk_system, sclk );

  TRACE(id, GET_SYSCLK_COUNT, fine, sclk, sys_clk);

  /* Watch out for overlow; if we overflowed, don't add in fine */
  if ( sys_clk > 0xFFFFFFFFuL - fine )
    return 0xFFFFFFFFuL;
  else
    return ( sys_clk + fine );

} /* sleepctl_get_sys_clk_cnt() */



/*============================================================================

FUNCTION SLEEPCTL_GET_CHIP_CNT

DESCRIPTION
  Return the number of chips the sleep controller has slept for.

DEPENDENCIES
  The given sleep controller must be running.
  Must only be called from an INTLOCK'd context.

RETURN VALUE
  Length of sleep, in chips

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_chip_cnt
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
/*--------------------------------------------------------------------------*/

  /* Convert sys_clk's to chips, and return */
  return sleepctl_get_sys_clk_cnt(id) >> CHIPX8_TO_CHIP_SHIFT;

} /* sleepctl_get_chip_cnt() */



/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK_TILL_WAKEUP

DESCRIPTION
  Return the number of sleep clocks until the wakeup interrupt.

DEPENDENCIES
  The given sleep controller must be running.
  Should only be called from an INTLOCK'd context.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sclk_till_wakeup
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk;

/*--------------------------------------------------------------------------*/

  /* Determine how long the sleep controller has slept for */
  sclk = sleepctl_get_sclk_cnt(id);

  TRACE_RLE(id, GET_SCLK_TILL_WAKEUP, sc->wakeup.sclk, sclk );

  /* Determine if the wakeup event is in the future */
  if ( sc->wakeup.sclk > sclk )
  {
    /* The wakeup event is in the future - return the difference */
    return sc->wakeup.sclk - sclk;
  }
  else
  {
    /* The wakeup event is already in the past. */
    return 0;
  }

} /* sleepctl_get_sclk_till_wakeup() */



/*============================================================================

FUNCTION SLEEPCTL_GET_SCLK_TILL_RTC_ON

DESCRIPTION
  Return the number of sleep clocks until the sleep ending interrupt.

DEPENDENCIES
  The given sleep controller must be running.
  Should only be called from an INTLOCK'd context.

RETURN VALUE
  Number of sleep clocks

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_sclk_till_rtc_on
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

  /* Number of sleep clocks the sleep controller has slept for */
  uint32                          sclk;

  /* The sleep controller's end point */
  uint32                          ending_sclk;

  /* The sleep controller's state */
  sleepctl_state_enum             state;

/*--------------------------------------------------------------------------*/

  INTLOCK();

  /* Determine how long the sleep controller has slept for */
  sclk = sleepctl_get_sclk_cnt(id);

  /* Capture sleep controller state info */
  ending_sclk = sc->ending.sclk;
  state       = sc->state;

  TRACE_RLE(id, GET_SCLK_TILL_RTC_ON, ending_sclk, sclk);

  INTFREE();

  /* Determine if the sleep ending event is in the future */
  if ( sclk > 0  &&  ending_sclk > sclk )
  {
    /* The sleep ending event is in the future - return the difference */
    return ending_sclk - sclk;
  }

  if ( state == SLEEPCTL_ARMED_STATE  ||  state == SLEEPCTL_SLEEP_STATE )
  {
    /* The sleep controller hasn't started yet, or we're still in the
       first sleep clock cycle.  The entire sleep duration remains. */
    return ending_sclk;
  }
  else
  {
    /* The sleep ending event is already in the past. */
    return 0;
  }

} /* sleepctl_get_sclk_till_rtc_on() */



/*============================================================================

FUNCTION SLEEPCTL_GET_WAKEUP_TIME

DESCRIPTION
  Return the scheduled wakeup time as an absolute timetick counter value.

DEPENDENCIES
  The given sleep controller must be running.

RETURN VALUE
  Timetick counter value at the wakeup interrupt

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_wakeup_time
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, GET_WAKEUP_TIME, 0, sc->wakeup.int_timetick, 0);
  return sc->wakeup.int_timetick;

} /* sleepctl_get_wakeup_time() */


/*============================================================================

FUNCTION SLEEPCTL_GET_ENDING_TIME

DESCRIPTION
  Return the scheduled ending time as an absolute timetick counter value.

DEPENDENCIES
  The given sleep controller must be running.

RETURN VALUE
  Timetick counter value at the sleep ending interrupt

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_ending_time
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  TRACE(id, GET_ENDING_TIME, 0, sc->ending.int_timetick, 0);
  return sc->ending.int_timetick;

} /* sleepctl_get_ending_time() */


/*============================================================================

FUNCTION SLEEPCTL_SUFFICIENT_WARMUP_TIME

DESCRIPTION
  Determine if sufficient time exists before the computed end-of-sleep point
  to perform RF warmup.

DEPENDENCIES
  Should only be called in response to the wakeup ISR.
  Should only be called from an INTLOCK'd context.

RETURN VALUE
  TRUE  - Sufficient warmup time exists
  FALSE - Insufficient warmup time exists

SIDE EFFECTS
  None

============================================================================*/

boolean sleepctl_sufficient_warmup_time
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Minimum warmup time, in slow clocks */
  uint32                          min_warmup_sclk
)
{
  /* Time remaining until sleep ending event */
  uint32                          warmup_sclk;

/*--------------------------------------------------------------------------*/

  /* Determine the "warmup" time remaining */
  warmup_sclk = sleepctl_get_sclk_till_rtc_on( id );

  /* Is the remaining warmup time at least the minimum? */
  if ( warmup_sclk >= min_warmup_sclk )
  {
    DBG_2( MED, "Warmup %lu sclks >= %lu req'd", warmup_sclk, min_warmup_sclk);

    /* Sufficient warmup time exists */
    return TRUE;
  }
  else
  {
    DBG_2(ERROR, "Warmup %lu sclks < %lu req'd", warmup_sclk, min_warmup_sclk);

    /* Insufficient warmup time exists */
    return FALSE;
  }

} /* sleepctl_sufficient_warmup_time() */



/*============================================================================

FUNCTION SLEEPCTL_COMPLETE_WAKEUP

DESCRIPTION
  Reprogram the sleep controller with the correct ending point

DEPENDENCIES
  May only be called in response to the wakeup ISR, and should only be called
  if sleepctl_sufficient_warmup_time( ) returns TRUE.

RETURN VALUE
  None

SIDE EFFECTS
  Sleep ending interrupt will be generated.

============================================================================*/

void sleepctl_complete_wakeup
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  INTLOCK();

  if ( sc->state != SLEEPCTL_WAKEUP_STATE )
  {
    ERR_FATAL("Sleep controller #%d in incorrect s/w state=%d!",
              id, sc->state, 0);
  }

  /* Enable sleep controller fast clock regime, for FINE counting */
  ENABLE_FAST_CLKRGM(id);

  BEGIN_REG_ACCESS_IN_INTLOCK();

  /* Program new sleep ending point, taking into account h/w overhead */
  SET_EXPIRE_FINE  (id, (uint16)(sc->ending.fine + sc->restart.fine_adj) - 1);
  SET_EXPIRE_COARSE(id, (sc->ending.sclk - sc->restart.sclk_adj) - 2);

  END_REG_ACCESS_IN_INTLOCK();

  /* Capture the slow-clock counter when the wakeup is committed to. */
  sc->warmup.timetick = timetick_get_safe(); 

  /* Update the sleep controller's software state to "warmup" */
  sc->state = SLEEPCTL_WARMUP_STATE;

  TRACE(id, COMMIT_TO_WAKEUP, sc->ending.fine, sc->ending.sclk, 0);

  INTFREE();

} /* sleepctl_complete_wakeup() */



/*============================================================================

FUNCTION SLEEPCTL_TRY_WAKEUP

DESCRIPTION
  Determine if sufficient time exists before the computed end-of-sleep point
  to perform RF warmup.  If there is, commit to the wakeup.

DEPENDENCIES
  May only be called in response to the wakeup ISR.

RETURN VALUE
  TRUE  - Sufficient warmup time exists, end of sleep set
  FALSE - Insufficient warmup time exists

SIDE EFFECTS
  May generate sleep ending interrupt

============================================================================*/

boolean sleepctl_try_wakeup
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Warmup time, in slow clocks */
  uint32                          min_warmup_sclk
)
{
  /* Flag indicating whether we commit to wakeup or not */
  boolean                         commit;

/*--------------------------------------------------------------------------*/

  INTLOCK();

  /* Check if sufficient warmup time exists */
  commit = sleepctl_sufficient_warmup_time(id, min_warmup_sclk);

  if ( commit == TRUE )
  {
    /* Sufficient warmup time exists!  Commit to wakeup. */
    sleepctl_complete_wakeup(id);
  }

  INTFREE();

  return commit;

} /* sleepctl_try_wakeup */



/*===========================================================================

FUNCTION SLEEPCTL_WAKEUP_NOW

DESCRIPTION
  This function wakes up the sleep controller as soon as possible. The
  function will not return until the sleep controller is awake.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleepctl_wakeup_now
(
  /* Sleep controller */
  sleepctl_id_type                id
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  INTLOCK();

  /* If we are currently sleeping, ... */
  if ( sc->state != SLEEPCTL_INACTIVE_STATE )
  {
    /* ... generate a wakeup event */
    sleepctl_force_wakeup( id );

    /* Reinitialize the sleep controller's state and ISRs */
    sleepctl_init_controller( id );
  }
  else
  {
    DBG_STR1( ERROR, "%s not sleeping: can't force wakeup", sc->id);
  }

  INTFREE( );

} /* sleepctl_wakeup_now() */



/*============================================================================

FUNCTION SLEEPCTL_ERROR_FEEDBACK

DESCRIPTION
  This function is used to notify the sleep controller of the perceived
  "error" in the most recent sleep interval's length, which the sleep
  controller may use to fine tune its estimate of the sleep clock.

DEPENDENCIES
  An initial estimate of the sleep clock frequency must be known, from
  sleep_clock_freq_estimate_complete( ).

RETURN VALUE
  None

SIDE EFFECTS
  Sleep clock frequency estimate is update.

============================================================================*/

void sleepctl_error_feedback
(
  /* Sleep controller */
  sleepctl_id_type                id,

  /* Perceived error in the sleep duration of the most recent sleep */
  int32                           sys_clk_error
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  /* Compute sleep clock frequency adjustment based on
     the length of time that was slept for. */

  sclk_sysclk_error_adjust( sc->sclk_system,
                            sc->last_sleep.sclk,
                            sc->last_sleep.sys_clk,
                            (int16) sys_clk_error );

} /* sleepctl_error_feedback() */



/*============================================================================

FUNCTION SLEEPCTL_GET_NEXT_WAKEUP

DESCRIPTION
  This function returns the time, in sleep clocks, until the next
  wakeup event (1x, HDR, etc.)

DEPENDENCIES
  None

RETURN VALUE
  Number of sleep clocks until the next wakeup event

SIDE EFFECTS
  None

============================================================================*/

uint32 sleepctl_get_next_wakeup( void )
{
  /* Loop index */
  int                             i;

  /* Next wakeup event */
  uint32                          next_wakeup;
  
  /* Sleep clocks until the sleep controller's wakeup event. */
  uint32                          wakeup;

/*--------------------------------------------------------------------------*/

  /* Start with largest possible value */
  next_wakeup = 0xFFFFFFFFuL;

  /* Loop over all the sleep controllers */
  for (i = 0; i < SLEEPCTL_NUM_CONTROLLERS; i++ )
  {
    /* Is this sleep controller running? */
    if ( sleepctl[i].state != SLEEPCTL_INACTIVE_STATE )
    {
      /* Determine the wakeup point for the sleep controller */
      wakeup = sleepctl_get_sclk_till_wakeup( (sleepctl_id_type) i );

      /* If this wakeup is before the best wakeup point so far ... */
      if ( wakeup < next_wakeup )
      {
        /* ... update to this sleep controller's wakeup point */
        next_wakeup = wakeup;
      }
    }
  }

  return next_wakeup;

} /* sleepctl_get_next_wakeup() */


#ifdef FEATURE_SLEEPCTL_PC
#error code not present
#endif /* FEATURE_SLEEPCTL_PC */


/*============================================================================

FUNCTION SLEEPCTL_SET_WARMUP_ENFORCEMENT

DESCRIPTION
  This function sets the specified sleep controller warmup enforcement mode.
  If warmup is enforced, then the sleep controller will make sure that
  the wakeup occurs and that there is enough time for warmup before setting
  the ending expiry point.  If warmup is not enforced, the sleep controller
  wakeup point can be latent, and even if there is not enough warmup time, 
  the ending point will occur as specified in set_duration anyway.
  
DEPENDENCIES
  NONE

RETURN VALUE
  None

SIDE EFFECTS
  Changes the warmup mode for the sleep controller

============================================================================*/

void sleepctl_set_warmup_enforcement
( 
  /* Sleep controller's warmup mode to be changed */
  sleepctl_id_type                id,

  /* Indicates whether warmup should be enforced or not */
  boolean                         warmup_enforced
)
{
  /* Sleep controller structure */
  sleepctl_ptr                    sc = &sleepctl[id];

/*--------------------------------------------------------------------------*/

  sc->warmup_enforced = warmup_enforced;
}


#ifdef FEATURE_UNIFIED_SLEEP_CTLR_P1
/*============================================================================

FUNCTION SLEEPCTL_FORCE_STATE, SLEEPCTL_FORCE_WAKEUP_SCLK

DESCRIPTION
  Temporary function hacks to make WCDMA sleep work.
  TBD: These functions and calls to them should be removed once WCDMA has
       code in place to use the common sleep controller.

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void sleepctl_force_state(sleepctl_id_type id,int state)
{

  int idx = (int)id;
  
  if ((idx<0) || (idx>=SLEEPCTL_NUM_CONTROLLERS))
  {
    ERR_FATAL("Bad sleepctl_id - %d",idx,0,0);
  }

  sleepctl[idx].state = (sleepctl_state_enum)state;
}

void sleepctl_force_wakeup_sclk(sleepctl_id_type id,uint32 sclk)
{

  int idx = (int)id;

  if ((idx<0) || (idx>=SLEEPCTL_NUM_CONTROLLERS))
  {
    ERR_FATAL("Bad sleepctl_id - %d",idx,0,0);
  }

  sleepctl[idx].wakeup.sclk = sclk;
}
#endif


