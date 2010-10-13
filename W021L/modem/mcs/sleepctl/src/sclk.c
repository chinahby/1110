/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     S L E E P  /  S L O W   C L O C K

GENERAL DESCRIPTION
  This file contains routines which manage the slow clock timing.

EXTERNALIZED FUNCTIONS (Global)
  sclk_init
  sclk_begin_estimate
  sclk_estimate_complete
  sclk_get_frequency
  sclk_begin_error_adjust
  sclk_error_adjust
  sclk_sysclk_error_adjust_gsm
  sclk_slow_clock_unstable
  sclk_from_chips
  sclk_to_chipx16
  sclk_to_timestamp
  sclk_propagate_estimate

EXTERNALIZED FUNCTIONS (Regional)
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  sclk_init( ) must be called first
  sclk_begin_estimate( ) must be called before sclk_estimate_complete()
  sclk_begin_error_adjust( ) must be called before sclk_error_adjust()
  sclk conversion functions (_to_xxx, _from_xxx) require a valid estimate

Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/sleepctl/main/latest/src/sclk.c#3 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
11/10/2008   sg    Added slow clock error tracking logic for Gobi
03/27/2008   hluo  Added LCU as applicable targets
01/06/2008   hluo  Added in Koel and Hawk as applicable targets
11/08/2007   gfr   Do not compile fee code on apps processor.
06/18/2007   rmak  SCLK P2: W, G, 1x all use common sclk_sysclk_error_adjust
02/21/2007   cab   Added sc2x featurization
10/19/2006   cab   Added includes for winmobile
09/19/2006   ejv   Added check for T_QSC60X0 in FREQ_ERROR macros.
08/24/2006   cab   Ensure the FEE source clock is set correctly
08/21/2006   cab   WCDMA integration
07/28/2006   jwob  7200 integration updates; set cx8_per_sclk in sclk_init
07/27/2006   jwob  Moved SLEEP register accessor macros from msm_drv.h
06/30/2006   ejv   Add feature and version checking for 32kHz src on QSC60X0.
06/20/2006   ejv   Added FEATURE_INTERNAL_32KHZ for QSC60X0.
04/17/2006   ejv   Additional T_QSC60X0 updates.
12/22/2005   ejv   Add support for T_QSC60X0.
12/02/2005   cab   Estimate complete data is now system specific
09/01/2005   cab   Changed estimate in sclk_begin_estimate to specify ms
07/13/2005   ajn   Added REG_ACCESS_CPU_AT_TCXO run-time T_MSM6800B check
06/10/2005   ajn   Prevent sclk.freq from becoming zero.
03/22/2005   ddh   Moved sclk_to_ms and _us functions to timetick
03/16/2005   ajn   Adjust timestamp conversion factor with GSM/WCDMA's fee.
03/14/2005   ejv   Add support for FEATURE_DUAL_CLK_RGM on 6800.
02/15/2005   ejv   Replaced clkregim.h with CLKRGM_H.
02/09/2005   ajn   Added rounding to _to_ms() function.
02/02/2005   ajn   Removed 5500 support, added GSM support
11/09/2004   ajn   Added #include "timer.h", featurized.
10/15/2004   ajn   FEATURE_INTERRUPT_CONTROL_WORKAROUND
08/24/2004   ajn   chipx16 -> sys_clk.  Eliminate sclk_enum_type.
05/26/2004   jcm   Rewrite slow clock error filter
11/11/2003   ajn   Halt the FEE circuit when an estimate is aborted.
10/20/2003   jcm   Allow sclk_from_us() to accept high XO clock speeds.
10/20/2003   jcm   Added slow_clock from microsecond function
09/26/2003   ajn   Extended range of sclk_to_chipx16( ) and sclk_to_ms( )
09/25/2003   ajn   Extended range of sclk_from_ms( ) function to > 1 minute
07/22/2003   ajn   Added slow clock to timestamp conversion function
07/20/2003   bt    T_MSM6500 updates.
04/28/2003   ajn   Reduced a MSG_ERROR to MSG_MED
01/30/2003   ajn   Allow external slow_clock_unstable indication
01/23/2003   ajn   Separated 1x & HDR error feedback calibration
05/08/2002   ajn   Corrected MSG_LOW macro.
01/30/2002   ajn   sclk_estimate_complete returns tri-state enum.
01/30/2002   ajn   Corrected typo
01/01/2002   ajn   Added parameter to control whether 1x or HDR is adjusting
                   (calibrating) the sleep clock frequency.
12/06/2001   ht    Added type cast to rid a compiler warning.
10/22/2001   ajn   Invalidate estimate when an equivalent one is requested.
10/19/2001   ajn   Added "to_ms" function
09/25/2001   ajn   Increased range of "to_chipx16"
09/24/2001   ajn   hdrerrno -> errno.  "to_chipx16" now accepts a uint32.
09/13/2001   ajn   Enable HDRSLP_CX16 clock regime during HDR FEE.
07/26/2001   ajn   SRCH/SLEEP VU tip remerge - first check in
04/30/2001   ajn   HDR/1x merge
04/11/2001   ajn   TRAMP_->TRAMP_HDR_  Corrected cx16 in sleepclk_from_chips.
11/02/2000   ajn   Added sclk reserve (typ 2 clks) to sleepclk_from_chips
10/31/2000   ajn   Created module

==========================================================================*/

/* <EJECT> */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "sclk.h"
#include "sleepcfg.h"

#include "fixed.h"
#include "msm.h"
#include CLKRGM_H
#include "tramp.h"
#include "qw.h"
#ifdef T_QSC60X0
#include "pm.h"
#include "hw.h"
#endif /* T_QSC60X0 */

#include "msg.h"
#include "err.h"


#ifdef FEATURE_INTERRUPT_CONTROL_WORKAROUND
#include "timer.h"
#endif /* FEATURE_INTERRUPT_CONTROL_WORKAROUND */

#include "timetick.h"

#ifdef FEATURE_OS_MEMORY_PROTECTION
#include "rex.h"
#endif /* FEATURE_OS_MEMORY_PROTECTION */

#if defined( FEATURE_WINCE ) && !defined( FEATURE_WINCE_OAL )
#error code not present
#endif /* FEATURE_WINCE && !FEATURE_WINCE_OAL */

/*==========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

==========================================================================*/

/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */


/*--------------------------------------------------------------------------
  Useful constants
--------------------------------------------------------------------------*/

/* System clock frequencies */
#define SYSCLK_FREQ_TIME      ( 1228800uL * 8uL)

#define SYSCLK_FREQ_IS2000    ( 1228800uL * 8uL)
#define SYSCLK_FREQ_IS856     ( 1228800uL * 8uL)
#define SYSCLK_FREQ_GSM       ( 1625000uL * 8uL)
#define SYSCLK_FREQ_WCDMA     ( 3840000uL * 8uL)

#define CHIPX8_CHIP_SHIFT     3


/*===========================================================================

  "Floating" Fixed Point

  The sleep clock (aka "slow clock", or "sclk") is a low frequency (30kHz to
  60kHz) clock, used for "coarse" sleep timing.  After a sleep duration has
  been converted from system clocks (sys_clk's) into sleep clocks, the
  residual "fine" sleep duration, in sys_clk's, is used to fine-tune the
  sleep duration to the requested total duration.
  
  In order to perform conversions between sleep clocks and sys_clk's, the
  sleep clock frequency is stored as "sys_clk's per sclk".  Since this is not
  necessarily an integral quantity, the value is stored in a fixed point
  format, by scaling the value up by a power of 2.
  
  For 1x and HDR systems, the sys_clk is the CDMA chipx8 clock, and there
  will be between 164 to 328 system clocks per sleep clock.  Other systems
  will use a different system clock, and have a different range for
  sys_clk/sclk.  To maintain reasonable accuracy over different systems,
  the power of 2, or "binary point", used by each of these systems may be
  different.

  Fixed Point Format:
    IS-2000:   14.18
    IS-856:    14.18

===========================================================================*/

#define BP_TIME               18
  /* Binary point for timestamp values */


#define BP_IS2000             18
  /* Binary point for IS-2000 systems */

#define BP_IS856              18
  /* Binary point for IS-856 systems */

#define BP_GSM                18
  /* Binary point for GSM systems */

#define BP_WCDMA              18
  /* Binary point for WCDMA systems */


#define BP_TIME_CONVERT       24
  /* Binary point for System freq to Time-stamp freq conversion */



/* <EJECT> */
/*--------------------------------------------------------------------------
  Frequency Estimation Constants
--------------------------------------------------------------------------*/

#define SCLK_32KHZ_GOOD_ESTIMATE_COUNT  32768u
  /* Approximately 1 second of slow clocks (at 32kHz) */

#define SCLK_32KHZ_ROUGH_ESTIMATE_COUNT 256u
  /* Approximately 7ms of slow clocks (at 32kHz) */



/* The SLEEP_XTAL_COUNT register is some TBD maximum count */
#ifdef HWIO_FEE_SLEEP_XTAL_COUNT_DATA_BMSK
 #define SCLK_32KHZ_MAX_ESTIMATE_COUNT  HWIO_FEE_SLEEP_XTAL_COUNT_DATA_BMSK
#elif defined(HWIO_SLEEP_XTAL_COUNT_DATA_BMSK)
 #define SCLK_32KHZ_MAX_ESTIMATE_COUNT  HWIO_SLEEP_XTAL_COUNT_DATA_BMSK
#else
 /* If the above doesn't determine the correct size, use magic numbers */
 #if defined(T_MSM6500) || defined(T_QSC60X0)
  #define SCLK_32KHZ_MAX_ESTIMATE_COUNT 65535
 #else
#error code not present
 #endif /* T_MSM6500 || T_QSC60X0 */
#endif

#define SCLK_1X_FEE_COUNT               255
  /* Length of 1x Fee sample period */

/* The following constants are used to determine if and how the 
   slow clock error should be bounded and whether to use the constant gain 
   or adaptive gain bounded input estimation algorithm.
   Refer to:
     Systems Memo Sleep Clock Tracking */

#define SCLK_ERROR_BNDRY_FLAT_CEIL      16
  /* Maximum error (cx16) allowed during first region */ 

#define SCLK_ERROR_BNDRY_FLAT_END       8379566
  /* Maximum sleep duration (chips) to use for 1st error boundary region */

#define SCLK_ERROR_BNDRY_SLOPE_END      26498517
  /* Maximum sleep duration (chips) to use for 2nd error boundary region */

#define SCLK_ERROR_BNDRY_AGBI_ALGORITHM  131072
  /* Maximum sleep duration (slow clocks) to use Adaptive Gain Bounded Input
     algorithm with */

#define MAX_OOB_CNT 1
  /* Maximum number of times error is allowed Out Of Bounds before
     applying the whole error (stop estimate drift), expection is
     if the error changes signs one extra out of bounds error is
     allowed */

/* LOOP_GAIN numbers to be applied to TE on GSM, when correcting FEE */
#define FEE_LOOP_GAIN_NUM 1
#define FEE_LOOP_GAIN_DEN 2

/*--------------------------------------------------------------------------
  Function-like Macros
--------------------------------------------------------------------------*/

#define  ABS( x ) ( ((x) < 0) ? (-(x)) : (x) )
#define LABS( x ) ( ((x) < 0) ? (-(x)) : (x) )
  /* Keep separate, just to realize the difference in types where used */
 
#define SAME_SIGN( a, b ) ( (a >= 0 ) == ( b >= 0 ) )
  /* True if signs of a and b are same */

#define MAX_SLOPED_ERROR( offset ) ( ( ( (offset) + 65536 ) >> 17 ) + 16 )
  /* Calculates maximum error for sleep duration in sloped boundary.
     offset represents the offset into the sloped bounding region 
     Refer to:
       Systems Memo Sleep Clock Tracking */


/*--------------------------------------------------------------------------
  Register Access: CPU at TCXO speed data
--------------------------------------------------------------------------*/

#ifdef REG_ACCESS_CPU_AT_TCXO
#if defined(T_MSM6800)

/* Default register accesses to be done with the CPU at TCXO sleep. */
boolean sleepcfg_reg_access_cpu_at_tcxo = TRUE;

#endif /* T_MSM6800 */
#endif /* REG_ACCESS_CPU_AT_TCXO */


/*--------------------------------------------------------------------------
  Frequency Estimation Data
--------------------------------------------------------------------------*/

typedef struct
{
  sclk_32kHz_estimate_enum_type     in_progress;
    /* Estimate currently in progress */

  sclk_32kHz_est_complete_cb_type   complete_cb;
    /* Estimate complete callback function, if any */

  sclk_sys_enum_type                sclk_system;
    /* System doing frequency estimation */

  boolean                           running;
    /* Set if estimation data collection loop is running (ISR installed) */

  uint16                            sclk_required;
    /* Number of slow clks required for the estimate */

  uint16                            sclk_in_sample;
    /* Current accumulation period. */

  uint16                            sclk_accumulated;
    /* Number of slow clks accumulated so far */

  uint32                            sysclks_accumulated;
    /* Number of chipx16's accumulated so far */

  #ifdef FEATURE_INTERRUPT_CONTROL_WORKAROUND
  timer_type                        timer;
    /* Timer to use instead of FEE interrupt */
  #endif /* FEATURE_INTERRUPT_CONTROL_WORKAROUND */
}
sclk_32kHz_estimate_struct_type;


/*--------------------------------------------------------------------------
  Slow clock stability
--------------------------------------------------------------------------*/

typedef enum
{
  SCLK_SLOW_CLOCK_STABLE,
    /* Slow clock is stable for frequency estimation. (Default) */

  SCLK_SLOW_CLOCK_UNSTABLE,
    /* Slow clock should not be used for any frequency estimates */

  SCLK_SLOW_CLOCK_STABILIZING
    /* Slow clock is now stable, but discard any in-progress sample */
}
sclk_slow_clock_stability_type;


/*--------------------------------------------------------------------------
  Slow clock stability
--------------------------------------------------------------------------*/

typedef enum
{
  SCLK_SLOW_CLOCK_ERROR_TRACKING_NORMAL,
    /* Use normal tracking error method. (Default) */

  SCLK_SLOW_CLOCK_ERROR_TRACKING_INACTIVE,
    /* Slow clock error tracking is inactive */

  SCLK_SLOW_CLOCK_ERROR_TRACKING_ACTIVE
    /* Slow clock error tracking is active */
}
sclk_slow_clock_error_tracking_state;


/* TODO: Need official feature name, or even better an NV item.
 * Ideally, the timeout value would be the NV item.  If the timeout is
 * zero then it's in default mode.
 *
 * But until that happens, use a static variable to be able to set it
 * from the debugger */
#ifdef FEATURE_SCLK_SLOW_CLOCK_ERROR_TRACKING
#error code not present
#else
static const timetick_type sclk_slow_clock_error_tracking_timeout = 0;
#endif /* FEATURE_SCLK_SLOW_CLOCK_ERROR_TRACKING */

/*--------------------------------------------------------------------------
  32 KHz Error Bounding Data 
--------------------------------------------------------------------------*/

typedef struct
{
  int32                             last_error;
    /* Indicate sign of last error for bounding, on sign is used */

  uint32                            oob_cnt;
    /* Number of times have been out of bounds in one direction */

  timetick_type                     timeout;
    /* timeout for active_tracking state */

  sclk_slow_clock_error_tracking_state tracking_state;
    /* current tracking state  */
}
sclk_boundary_type;


/*--------------------------------------------------------------------------
  Timestamp Conversion Data
--------------------------------------------------------------------------*/

typedef struct
{
  uint32                            cx8_per_sclk;
    /* Chipx8 per slow clock ... for time-stamp conversions */

  uint16                            bp;
    /* Binary point for time-stamp conversions */
}
sclk_timestamp_conversion_type;


/*--------------------------------------------------------------------------
  SCLK System Data Type
--------------------------------------------------------------------------*/

typedef struct
{
  uint32                            sysclk_freq;
    /* System clock frequency */

  uint32                            sysclk_per_sclk;
    /* Ration between system clock & sleep clock, scaled by binary point */

  uint32                            time_convert;
    /* Conversion from sysclk_per_sclk to the timestamp's sysclk/sclk */

  sclk_32kHz_estimate_enum_type     completed;
    /* Last estimate completed for this system */

  uint16                            bp;
    /* Binary point for this system */

  sclk_boundary_type                boundary;
    /* Bounding information for error feedback */
}
sclk_system_struct_type;


/*--------------------------------------------------------------------------
  SCLK Module Data Type
--------------------------------------------------------------------------*/

typedef struct
{
  sclk_timestamp_conversion_type    time;
    /* Time-stamp conversion information */

  sclk_system_struct_type           system[ SCLK_NUM_SYSTEMS ];
    /* Per-system sleep clock data */

  uint32                            freq;
    /* The measured slow clock frequency */

  uint32                            freq_in_mHz;
    /* The measured slow clock frequency in mHz*/
    
  sclk_32kHz_estimate_struct_type   estimate;
    /* 32kHz Slow Clock Frequency Estimation Data */

  sclk_slow_clock_stability_type    slow_clock_stablity;
    /* Flag indicating slow clock frequency is not stable */
}
sclk_struct_type;


/*--------------------------------------------------------------------------
  SCLK Module Data
--------------------------------------------------------------------------*/
#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */

static sclk_struct_type sclk;
#ifdef FEATURE_WINCE
#error code not present
#endif /* FEATURE_WINCE */



/*--------------------------------------------------------------------------
  SCLK system information initialization data type
--------------------------------------------------------------------------*/

typedef struct
{
  sclk_sys_enum_type                system;
    /* sclk system to be initialized */

  uint16                            bp;
    /* Binary point for this clock system */

  uint32                            sysclk_freq;
    /* System frequency for this sclk system */
} 
sclk_sys_init_type;


static const sclk_sys_init_type sclk_sys_init[] =
{
#if SUPPORTED & S_GSM
  {SCLK_GSM, BP_GSM, SYSCLK_FREQ_GSM},
#endif

#if SUPPORTED & S_WCDMA
  {SCLK_WCDMA, BP_WCDMA, SYSCLK_FREQ_WCDMA},
#endif

#if SUPPORTED & S_1X
  {SCLK_1X, BP_IS2000, SYSCLK_FREQ_IS2000},
#endif

#if SUPPORTED & S_HDR
  {SCLK_HDR, BP_IS856, SYSCLK_FREQ_IS856},
#endif

  {SCLK_NUM_SYSTEMS, 0, 0}
};



/* <EJECT> */
/*============================================================================

                         MACRO FUNCTION DEFINITIONS

============================================================================*/


/*--------------------------------------------------------------------------
 FEE SysClk (slpfast) Source Selection Macro (Target dependent)
--------------------------------------------------------------------------*/

#if defined(T_MSM7200)

  #define SELECT_FEE_SOURCE(src) /* TBD */
  
#elif defined(T_MSM7500) || defined(T_QSC60X0)

  #define SELECT_FEE_SOURCE(src) /* TBD */
   
#elif defined(T_MSM6700) || defined(T_QSC60X5)

  #ifdef FEATURE_DUAL_CLK_RGM
  
  #define SELECT_FEE_SOURCE(src) \
    clk_regime_sel_clk_src(CLKRGM_SLPFAST_FEE_CLK, \
      CLK_RGM_SRC_SEL_SLPFAST_FEE_ ## src ## _CLK_V)
  
  #else
  
  #define SELECT_FEE_SOURCE(src) \
    clk_regime_sel_clk_src(CLK_RGM_SRC_SEL_SLPFAST_FEE, \
      CLK_RGM_SRC_SEL_SLPFAST_FEE_ ## src ## _CLK_V)

  #endif /* FEATURE_DUAL_CLK_RGM */

#elif defined(T_MSM6500)
#error code not present
#else
#error code not present
#endif /* T_MSMxxxx */

/*===========================================================================

MACRO SLEEP_GET_FREQ_ERROR_ESTIMATE

DESCRIPTION     On MSM6500, returns # of slpfast clock cycles measured
                in SLEEP_XTAL_CNT slow clock cycles.

                This macro reads the number of chipx8's in the latest
                calibration period.
                
                For chips that use a fixed period of 255 slow clocks, and
                start the chipx8 count at -161 for each slow clock, the
                value return is automatically increased by 161*255. 
                
DEPENDENCIES    None.
                                
RETURN VALUE    Chipx8's in sample period.
      
SIDE EFFECTS    None.

===========================================================================*/

#ifdef T_QUASAR
#error code not present
#else
#if defined ( T_MSM6700 ) || defined ( T_MSM7200 ) || defined ( T_QSC60X0 ) || defined ( T_MSM6246 ) \
                          || defined ( T_MSM6290 ) || defined (T_QSC6270)
#define SLEEP_GET_FREQ_ERROR_ESTIMATE( ) \
  ( HWIO_INM ( FEE_SLEEP_XTAL_FREQ_ERR, \
               HWIO_FMSK( FEE_SLEEP_XTAL_FREQ_ERR, DATA ) ) \
            >> HWIO_SHFT( FEE_SLEEP_XTAL_FREQ_ERR, DATA ) )
#else
#define SLEEP_GET_FREQ_ERROR_ESTIMATE( ) \
  ( HWIO_INM ( SLEEP_XTAL_FREQ_ERR, \
               HWIO_FMSK( SLEEP_XTAL_FREQ_ERR, DATA ) ) \
            >> HWIO_SHFT( SLEEP_XTAL_FREQ_ERR, DATA ) )
#endif /* T_MSM6700 || T_MSM7200 || QSC60X0 */
#endif /* T_QUASAR */

/*===========================================================================

MACRO SLEEP_SET_FREQ_ERROR_COUNT

DESCRIPTION     Set the number of slow clocks to count slpfast clock
                cycles over to estimate the slow clock frequency error.

                Once the FEE interrupt occurs, the estimate does not
                begin again until this register is written again.
                
DEPENDENCIES    None.
                                
RETURN VALUE    None.
      
SIDE EFFECTS    None.

===========================================================================*/

#ifdef T_QUASAR
#error code not present
#else
#if defined ( T_MSM6700 ) || defined ( T_MSM7200 ) || defined ( T_QSC60X0 ) || defined ( T_MSM6246 ) \
                          || defined ( T_MSM6290 ) || defined (T_QSC6270)
#define SLEEP_SET_FREQ_ERROR_COUNT( count ) \
  { /*lint -save -e703 -e701 Shift left of signed quantity intentional */ \
    HWIO_OUTM( FEE_SLEEP_XTAL_COUNT, \
               HWIO_FMSK( FEE_SLEEP_XTAL_COUNT, DATA ),  \
               count << HWIO_SHFT( FEE_SLEEP_XTAL_COUNT, DATA ) ); \
  } /*lint -restore */
#else
#define SLEEP_SET_FREQ_ERROR_COUNT( count ) \
  { /*lint -save -e703 -e701 Shift left of signed quantity intentional */ \
    HWIO_OUTM( SLEEP_XTAL_COUNT, \
               HWIO_FMSK( SLEEP_XTAL_COUNT, DATA ), \
               count << HWIO_SHFT( SLEEP_XTAL_COUNT, DATA ) ); \
  } /*lint -restore */
#endif /* T_MSM6700 || T_MSM7200 || QSC60X0 */
#endif /* T_QUASAR */


/* <EJECT> */
/*============================================================================

                      FUNCTION  DEFINITIONS

============================================================================*/

#if !defined(IMAGE_APPS_PROC)
/*============================================================================

FUNCTION SCLK_SET_FEE_SYSTEM

DESCRIPTION
  This function programs the FEE circuitry to use the appropriate
  sysclk source.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  HWIO_MISC_CLK_SEL1_SLPFAST_FEE_CLK_SRC_SEL_BMSK

============================================================================*/

void sclk_set_fee_system( void )
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( sclk.estimate.sclk_system )
  {
    #if SUPPORTED & S_1X
    
    case SCLK_1X:
      SELECT_FEE_SOURCE( CX8 );
      break;

    #endif /* SUPPORTED & S_1X */


    #if SUPPORTED & S_HDR
    
    case SCLK_HDR:
      SELECT_FEE_SOURCE( CX8 );
      break;

    #endif /* SUPPORTED & S_HDR */


    #if SUPPORTED & S_GSM
    
    case SCLK_GSM:
      SELECT_FEE_SOURCE( GSM );
      break;

    #endif /* SUPPORTED & S_GSM */


    #if SUPPORTED & S_WCDMA
    
    case SCLK_WCDMA:
      SELECT_FEE_SOURCE( WCDMA );
      break;

    #endif /* SUPPORTED & S_WCDMA */


    default:
      ERR_FATAL("Unsupported system: %d", sclk.estimate.sclk_system, 0, 0);
  }

} /* sclk_set_fee_system */


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_SET_FEE_COUNT

DESCRIPTION
  This function programs the FEE duration, in SCLK's.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

static void sclk_set_fee_count
(
  uint32                          sclks
    /* The FEE duration, in sclk's */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(T_MSM6500) || defined(T_MSM7200) || defined(T_QSC60X0) || defined ( T_MSM6246 ) || defined ( T_MSM6290 ) || defined (T_QSC6270)

  /* Limit sample range to valid range */
  if ( sclks > SCLK_32KHZ_MAX_ESTIMATE_COUNT )
  {
    sclks = SCLK_32KHZ_MAX_ESTIMATE_COUNT;
  }

  BEGIN_REG_ACCESS();
    /* Switch CPU to low speed, if req'd */

  SLEEP_SET_FREQ_ERROR_COUNT( sclks );
    /* Program the req'd duration */

  END_REG_ACCESS();
    /* Restore CPU speed */

#else
#error code not present
#endif /* T_MSM6500 || T_MSM7200 || T_QSC60X0 */

  sclk.estimate.sclk_in_sample = (uint16) sclks;
    /* Record length of this sample */

#ifdef FEATURE_INTERRUPT_CONTROL_WORKAROUND
  timer_set(&sclk.estimate.timer, sclks+2, 0, T_SCLK); 
    /* Start the timer to measure the FEE estimation period. */
#endif /* FEATURE_INTERRUPT_CONTROL_WORKAROUND */

  MSG_MED("FEE: Programed %d slow clocks", sclks, 0, 0);

} /* sclk_set_fee_count() */



/*==========================================================================

FUNCTION SCLK_GET_FEE_SYSCLKS

DESCRIPTION
  This function programs the FEE duration, in SCLK's.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

static uint32 sclk_get_fee_sysclks( void )
{
  uint32                          sysclks;
    /* The number of system clocks accumulated in the FEE period */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  BEGIN_REG_ACCESS();
    /* Switch CPU to low speed, if req'd */

  sysclks = SLEEP_GET_FREQ_ERROR_ESTIMATE();
    /* Retrieve the accumulated system clocks */

  END_REG_ACCESS();
    /* Restore CPU speed */

  return sysclks;

} /* sclk_get_fee_sysclks() */



/*==========================================================================

FUNCTION SCLK_STOP_FEE

DESCRIPTION
  This function stops the FEE engine

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

static void sclk_stop_fee( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined(T_MSM6500) || defined(T_QSC60X0) || defined(T_MSM7200)

  BEGIN_REG_ACCESS();
    /* Switch CPU to low speed, if req'd */

  SLEEP_SET_FREQ_ERROR_COUNT( 0 );
    /* Program the req'd duration */

  END_REG_ACCESS();
    /* Restore CPU speed */

#endif /* T_MSM6500 || T_QSC60X0 */

  tramp_set_isr( TRAMP_SLEEP_FEE_ISR, NULL );
    /* Uninstall the FEE ISR */

#ifdef FEATURE_INTERRUPT_CONTROL_WORKAROUND

  timer_clr(&sclk.estimate.timer, T_NONE); 
    /* Stop the FEE interrupt work-around timer. */

#endif /* FEATURE_INTERRUPT_CONTROL_WORKAROUND */

  sclk.estimate.running = FALSE;
    /* Estimate data collection loop is done */

} /* sclk_stop_fee() */

#endif /* !IMAGE_APPS_PROC */

/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_INIT_SYSTEM

DESCRIPTION
  This function initializes the given sclk system.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

static void sclk_init_system
(
  sclk_sys_enum_type              sys,
    /* System to be initialized */

  uint32                          sysclk_freq,
    /* System clock frequency */

  uint16                          binary_point
    /* Binary point for this system */
)
{
  sclk_system_struct_type         *system;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  system = &sclk.system[ sys ];
    /* System structure pointer for system being initialized */

  system->sysclk_freq = sysclk_freq;
    /* Record the system frequency */

  system->bp = binary_point;
    /* Record the system binary point */

  system->completed = SCLK_32KHZ_NO_ESTIMATE;
    /* No estimate of the slow clock frequency exists */

  system->sysclk_per_sclk = fixed_divide_rounded( sysclk_freq, 
                                                  TIMETICK_NOMINAL_FREQ_HZ,
                                                  binary_point );
    /* Initial guess at the system clock / sleep clock ratio */

  system->time_convert = fixed_divide_rounded( sysclk_freq,
                                               SYSCLK_FREQ_TIME,
                                               BP_TIME_CONVERT);
    /* Conversion btw the system's sysclk/sclk and time's sysclk/sclk */


  system->boundary.last_error     = 0;
  system->boundary.oob_cnt        = 0;
  system->boundary.timeout        = 0;
  if ( sclk_slow_clock_error_tracking_timeout == 0 ) 
  { 
    system->boundary.tracking_state = SCLK_SLOW_CLOCK_ERROR_TRACKING_NORMAL;
  } 
  else 
  {
    system->boundary.tracking_state = SCLK_SLOW_CLOCK_ERROR_TRACKING_INACTIVE; 
  }

    /* Reset error bounding */

} /* sclk_init_system() */


#ifndef IMAGE_APPS_PROC
/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_PROCESS_FEE

DESCRIPTION
  This function is called by the FEE Interrupt Service Routine (ISR), and
  is used to determine the slow clock's frequency.

DEPENDENCIES
  sclk_begin_frequency_estimate

RETURN VALUE
  TRUE if enough estimation data is available.
  FALSE if more estimation data is required.

SIDE EFFECTS
  Updates sclk.estimate fields

==========================================================================*/

static boolean sclk_process_fee
(
  uint32                          sysclks
    /* Chipx16's over the last sample period */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("FEE: sysclks=%ld sclk=%d",
          sysclks, sclk.estimate.sclk_in_sample, 0);

  if ( sclk.slow_clock_stablity  == SCLK_SLOW_CLOCK_STABLE )
  {
    sclk.estimate.sysclks_accumulated += sysclks;
    sclk.estimate.sclk_accumulated    += sclk.estimate.sclk_in_sample;
      /* Accumulate slow clocks and chipx16's in this sample period */
  }
  else
  {
    /* Slow clock frequency is not stable. */

    sclk.estimate.sysclks_accumulated = 0;
    sclk.estimate.sclk_accumulated    = 0;
      /* Keep estimation statistics in initial state */

    if ( sclk.slow_clock_stablity == SCLK_SLOW_CLOCK_STABILIZING )
    {
      /* Slow clock frequency has stablized. */

      sclk.slow_clock_stablity = SCLK_SLOW_CLOCK_STABLE;
        /* Next FEE sample period should contain valid estimation data */
    }
  }

  if ( sclk.estimate.sclk_accumulated >= sclk.estimate.sclk_required )
  {
    /* Enough data has been collected for the required slow clock
       frequency estimate. */

    if ( sclk.estimate.complete_cb != NULL )
    {
      sclk.estimate.complete_cb( );
        /* Inform any interested party that the estimate data collection
           is complete */
    }

    return TRUE;
  }
  else
  {
    /* More FEE data is still required - keep collecting FEE samples. */

    return FALSE;
  }

} /* sclk_process_fee( ) */


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_PROGRAM_FEE

DESCRIPTION
  This function computes the slow clock duration of the next portion of
  the total estimate duration, and programs the Frequency Estimator
  circuit.

DEPENDENCIES
  sleepclk_begin_frequency_estimate

RETURN VALUE
  None

SIDE EFFECTS
  Updates sleepclk.estimate fields

==========================================================================*/

static void sclk_program_fee( )
{
  uint32                          sclks;
    /* The # of slow clocks in the next sample period */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sclks = sclk.estimate.sclk_required - sclk.estimate.sclk_accumulated;
    /* Compute remaining slow clocks in estimate */

  /* Program length of next sample. */
  sclk_set_fee_count( sclks );

} /* sclk_program_fee */



/*==========================================================================

FUNCTION SCLK_FEE_ISR

DESCRIPTION
  This Interrupt Service Routine (ISR) is used to determine the slow
  clock's frequency.

DEPENDENCIES
  sclk_begin_frequency_estimate

RETURN VALUE
  None

SIDE EFFECTS
  Updates sclk.estimate fields

==========================================================================*/

static void sclk_fee_isr( )
{
  uint32                          system_clks;
    /* System clocks [Chipx8's, GSM clocks] in latest sample period */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  system_clks = sclk_get_fee_sysclks();
    /* Get clock frequency estimate from the hardware */


  if ( !sclk.estimate.running )
  {
    MSG_ERROR("Unexpected FEE: sysclk=%d", system_clks, 0, 0);
  }
  else
  {
    if ( sclk_process_fee( system_clks ) == TRUE )
    {
      sclk_stop_fee();
        /* Fee estimate data collection is complete */
    }
    else
    {
      sclk_program_fee( );
        /* Fee estimate is not yet complete - program the next segment */
    }
  }

} /* sclk_fee_isr( ) */

#endif /* !IMAGE_APPS_PROC */

/*==========================================================================

FUNCTION SCLK_INIT

DESCRIPTION
  This function initializes the SLEEP CLOCK module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frequency data is cleared.

==========================================================================*/

void sclk_init( void )
{
  uint32                          i;
    /* Loop index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef REG_ACCESS_CPU_AT_TCXO
  #if defined(T_MSM6800)

  /* If this is not 6800 R1.0 ... */
  if ( !HW_MSM6800_HAS_R1_V0 )
  {
    /* ... then sleep registers accesses do not need to be slowed down */
    sleepcfg_reg_access_cpu_at_tcxo = FALSE;
  }

  #endif /* T_MSM6800 */
  #endif /* REG_ACCESS_CPU_AT_TCXO */

  #ifdef T_QSC60X0
  #if defined(FEATURE_EXTERNAL_32KHZ)
  pm_xo_sel_alt_sleep_clk_src( PM_XO_32KHZ_SLEEP_CLOCK_XTAL_OSC );
    /* Use external 32kHz slow clock */
  #elif defined(FEATURE_INTERNAL_32KHZ)
  pm_xo_sel_alt_sleep_clk_src( PM_XO_19_2MHZ_XTAL_OSC );
    /* Use slow clock as derived from 19.2MHz XO source */
  #else
  if( hw_version_qsc60x0() >= HW_QSC_VERSION_1P2 )
  {
    pm_xo_sel_alt_sleep_clk_src( PM_XO_19_2MHZ_XTAL_OSC );
      /* Use slow clock as derived from 19.2MHz XO source */
  }
  else /* earlier then r1.2 */
  {
    pm_xo_sel_alt_sleep_clk_src( PM_XO_32KHZ_SLEEP_CLOCK_XTAL_OSC );
      /* Use external 32kHz slow clock */
  }
  #endif /* FEATURE_INTERNAL_32KHZ */
  #endif /* T_QSC60X0 */


  /* 32kHz Slow Clock Initialization */

  sclk.freq                   = TIMETICK_NOMINAL_FREQ_HZ;
  sclk.freq_in_mHz            = sclk.freq * 1000;
    /* Initial guess for Slow Clock Frequency */

  sclk.estimate.in_progress   = SCLK_32KHZ_NO_ESTIMATE;
    /* And no estimate of the slow clock frequency is in progress */

  timetick_update_freq(sclk.freq);
    /* Update the sclk frequency to be used by timetick */

  #ifdef FEATURE_INTERRUPT_CONTROL_WORKAROUND
  timer_def(&sclk.estimate.timer, NULL, 
            NULL, 0, 
            (timer_t1_cb_type) sclk_fee_isr, 0);
    /* Define a timer for FEE estimation period measuring */
  #endif /* FEATURE_INTERRUPT_CONTROL_WORKAROUND */

  /* System Initialization */
  for( i = 0; sclk_sys_init[i].system != SCLK_NUM_SYSTEMS ; i++)
  {
     sclk_init_system
     (
        sclk_sys_init[i].system,
        sclk_sys_init[i].sysclk_freq,
        sclk_sys_init[i].bp
     );
  }

  /* Set the ratio of IS2000 chipx8 to slow clocks (used for timestamps) */
  sclk.time.cx8_per_sclk = fixed_divide_rounded( SYSCLK_FREQ_TIME, 
                                                 TIMETICK_NOMINAL_FREQ_HZ,
                                                 BP_TIME );
  sclk.time.bp = BP_TIME;

} /* sclk_init( ) */

#ifndef IMAGE_APPS_PROC
/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_START_FEE

DESCRIPTION
  This function starts the appropriate slow clock estimation system.

DEPENDENCIES
  sclk.estimate

RETURN VALUE
  None

SIDE EFFECTS
  Installs an ISR

==========================================================================*/

static void sclk_start_fee( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED("Starting to collect %d FEE samples",
    sclk.estimate.sclk_required, 0, 0);

  INTLOCK();

  sclk.estimate.running = TRUE;
    /* Estimate data collection loop is starting */

  #ifndef FEATURE_INTERRUPT_CONTROL_WORKAROUND
  
  tramp_set_isr( TRAMP_SLEEP_FEE_ISR, sclk_fee_isr );
    /* Install the FEE ISR handler and start collecting samples */

  #endif /* ! FEATURE_INTERRUPT_CONTROL_WORKAROUND */

  sclk_program_fee();
    /* Begin Fee estimation */

  INTFREE();

} /* sclk_start_fee */


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_SLOW_CLOCK_UNSTABLE

DESCRIPTION
  This function is used to indicate if the slow clock frequency is
  unstable, due to a disturbance such as PMIC initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Prevents sclk_estimate_complete( ) from returning E_SUCCESS while
  slow clock unstable is TRUE.

==========================================================================*/

void sclk_slow_clock_unstable
(
  boolean                         unstable
    /* Slow clock stability status */
)
{
  uint32                          i;
    /* Loop index */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  INTLOCK();
    /* Begin critical section */

  if ( unstable )
  {
    sclk.slow_clock_stablity  = SCLK_SLOW_CLOCK_UNSTABLE;
      /* Update the status of the slow clock frequency stability */

    for ( i = 0; i < SCLK_NUM_SYSTEMS; i++) 
    {
      sclk.system[i].completed   = SCLK_32KHZ_NO_ESTIMATE;
        /* Invalidate any completed slow clock estimate */
    }
  }
  else
  {
    sclk.slow_clock_stablity  = SCLK_SLOW_CLOCK_STABILIZING;
      /* Update the status of the slow clock frequency stability */

    if ( sclk.estimate.in_progress != SCLK_32KHZ_NO_ESTIMATE )
    {
      if ( !sclk.estimate.running )
      {
        sclk_start_fee();
      }
    }
  }

  INTFREE();
    /* End critical section */

} /* sclk_slow_clock_unstable */


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_BEGIN_ESTIMATE

DESCRIPTION
  This function initializes causes a frequency estimate of the slow clock
  to be started.

DEPENDENCIES
  None

PARAMETERS
  estimate      - type of estimate required - rough (8 ms), fair (500 ms),
                  good (1000ms)
  complete_cb   - a callback function to signal when estimate is complete.
                  (May be NULL)

RETURN VALUE
  E_SUCCESS     - Estimation process is beginning
  E_IN_PROGRESS - An estimation is currently underway, can't start a new
                  estimate right now.
  E_NOT_ALLOWED - FEATURE_SLEEP_SLOW_CLOCK is not defined.

SIDE EFFECTS
  "est_complete( )" will be called from interrupt context.  The callback
  function should cause the interested task to call the "estimate available"
  function to complete the estimation process.

==========================================================================*/

errno_enum_type sclk_begin_estimate
(
  sclk_sys_enum_type                fee_system,
    /* Collect FEE using 1x or HDR system FEE circuitry */

  sclk_32kHz_estimate_enum_type     estimate,
    /* Specifies the required duration (in ms) of the estimate */

  sclk_32kHz_est_complete_cb_type   complete_cb
    /* Estimate complete callback function */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_SLEEP_SLOW_CLOCK

  if ( sclk.estimate.in_progress == SCLK_32KHZ_NO_ESTIMATE )
  {
    if ( sclk.system[ fee_system ].completed >= estimate )
    {
      /* A estimate this good or better is available ...
         ... but we've been requested to redo the estimate. */

      sclk.system[ fee_system ].completed = 
            (sclk_32kHz_estimate_enum_type) (estimate-1);
        /* Invalidate the current estimate level */
    }

    sclk.estimate.sclk_system          = fee_system;
    sclk.estimate.in_progress          = estimate;
      /* Record type of estimate */

    sclk_set_fee_system();

    sclk.estimate.complete_cb          = complete_cb;
      /* Function to call to indicate estimation data has been collected */

    sclk.estimate.sclk_accumulated     = 0;
    sclk.estimate.sysclks_accumulated  = 0;
      /* No data has been collected yet */

    sclk.estimate.sclk_required = 
        (uint16) timetick_cvt_to_sclk((unsigned long) estimate, T_MSEC);

    sclk_start_fee( );

    return E_SUCCESS;
  }
  else
  {
    /* Can't start a new estimate until the first one is complete */

    MSG_HIGH("SCLK Estimate already in progress", 0,0,0 );

    return E_IN_PROGRESS;
  }

#else

  MSG_HIGH("SLOW_CLOCK disabled", 0,0,0 );

  return E_NOT_ALLOWED;

#endif /* FEATURE_SLEEP_SLOW_CLOCK */

} /* sclk_begin_estimate( ) */
#endif /* !IMAGE_APPS_PROC */

/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_COMPUTE_HERTZ

DESCRIPTION
  Computes the slow clock frequency in hertz from chipx16 / sclk

DEPENDENCIES
  system->sysclk_per_sclk

RETURN VALUE
  TRUE if new frequency is valid (inside 30kHz to 60kHz)
  FALSE if frequency update is not performed

SIDE EFFECTS
  Will update sclk.time.cx8_per_sclk if new frequency is in valid range.

==========================================================================*/

static boolean sclk_compute_hertz
(
  sclk_system_struct_type *         system
    /* System structure */
)
{
  uint32                            f, mf;
    /* Frequency, in Hz and 1000ths of a Hz */

  int32                             bp_shift;
    /* Difference in binary point btw system and time format */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  f = fixed_divide_rounded( system->sysclk_freq,
                                    system->sysclk_per_sclk,
                                    system->bp + 10 );

  mf = ((f & 0x3FF) * 1000 + 512) >> 10;
  f >>= 10;
    /* Clock frequency (in Hz + mHz), based on given system */


  /* Ensure frequency is in valid range */
  if ( f < 30000  ||  f > 60000 )
  {
    ERR("Slow clock frequency out of spec: %lu.%03u Hz", f, mf, 0);

    return FALSE;
  }


  /* Set new slow clock frequency */
  MSG_HIGH("Freq = %lu.%03u Hz", f, mf, 0);

  sclk.freq = (mf >= 500) ? f+1 : f;
  sclk.freq_in_mHz = (f * 1000) + mf;

  timetick_update_freq(sclk.freq);
    /* Update the sclk frequency to be used by timetick */

  /* Convert sysclk/sclk to timestamp units */

  bp_shift = BP_TIME - system->bp;
    /* Get difference in binary points, if any */

  if ( system->time_convert == 1 << BP_TIME_CONVERT )
  {
    /* Unity conversion ... just needs shift, if binary points differ */

    if ( bp_shift >= 0 )
    {
      sclk.time.cx8_per_sclk = system->sysclk_per_sclk << bp_shift;
    }
    else if ( bp_shift < 0 )
    {
      sclk.time.cx8_per_sclk = system->sysclk_per_sclk >> -bp_shift;
    }
  }
  else
  {
    /* Convert the sysclk/sclk value to the timestamp units */
    sclk.time.cx8_per_sclk = fixed_divide_rounded(
      system->sysclk_per_sclk,
      system->time_convert,
      (uint16)(BP_TIME_CONVERT + bp_shift));
  }

  return TRUE;

} /* sclk_compute_hertz( ) */


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_COMPUTE_ESTIMATE

DESCRIPTION
  Computes the slow clock frequency information
    (chipx16's per slow clock & slow clocks per second)

DEPENDENCIES
  sleepclk.estimate parameters

RETURN VALUE
  None

SIDE EFFECTS
  Updates sclk data

==========================================================================*/

static void sclk_compute_estimate
(
  sclk_system_struct_type *         system
    /* System structure */
)
{
  boolean                           estimate_valid;
    /* Is the slow clock frequency estimate valid (30kHz to 60kHz)? */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("%ld sysclks, %d sclks",
            sclk.estimate.sysclks_accumulated,
            sclk.estimate.sclk_accumulated,
            0);

  system->sysclk_per_sclk = fixed_divide_rounded(
            sclk.estimate.sysclks_accumulated,
            sclk.estimate.sclk_accumulated,
            system->bp);
      /* Compute the slow clock frequency ( sysclk's per slow clock ) */

  MSG_HIGH("sysclks/sclk = %ld + %ld / 2^%d",
            FIXED_FLOOR(system->sysclk_per_sclk, system->bp),
            FIXED_FRAC(system->sysclk_per_sclk,  system->bp),
            system->bp);


  estimate_valid = sclk_compute_hertz( system );
    /* Determine frequency in hertz */


  if ( estimate_valid )
  {
    system->completed = sclk.estimate.in_progress;
      /* This estimate replaces any prior (possibly lower quality)
         estimate. */
  }
  else
  {
    system->completed = SCLK_32KHZ_NO_ESTIMATE;
      /* Bad estimate - we don't know the slow clock frequency, yet. */
  }

  sclk.estimate.in_progress = SCLK_32KHZ_NO_ESTIMATE;
    /* And no estimate is currently in progress anymore */

} /* sclk_compute_freq_estimate */


#ifndef IMAGE_APPS_PROC
/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_ESTIMATE_COMPLETE

DESCRIPTION
  Attempts to complete the frequency estimate, if it has not already been
  completed.

DEPENDENCIES
  None.

RETURN VALUE
  E_SUCCESS     - an estimate is available
  E_IN_PROGRESS - the given estimate is still underway.
  E_FAILURE     - the estimate is neither available or in progress.

SIDE EFFECTS
  None

==========================================================================*/

errno_enum_type sclk_estimate_complete
(
  sclk_sys_enum_type                fee_system,
    /* Collect FEE using 1x or HDR system FEE circuitry */

  sclk_32kHz_estimate_enum_type      estimate
    /* Accuracy of the slow clock frequency estimate */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( sclk.estimate.in_progress != SCLK_32KHZ_NO_ESTIMATE )
  {
    if ( sclk.slow_clock_stablity == SCLK_SLOW_CLOCK_STABLE )
    {
      if ( sclk.estimate.sclk_accumulated >= sclk.estimate.sclk_required )
      {
        /* An estimation has been completed */

        sclk_compute_estimate( &sclk.system[ sclk.estimate.sclk_system ] );
          /* Perform the computations on the estimation data */

      } /* if estimation information is ready. */
    }
  } /* if estimate is in progress */


  if ( estimate <= sclk.system[ fee_system ].completed )
  {
    return E_SUCCESS;
      /* An estimate at least this accurate is available. */
  }
  else
  {
    /* An estimate of the desired accuracy is not available ... */

    if ( sclk.estimate.in_progress == SCLK_32KHZ_NO_ESTIMATE )
    {
      /* There is no current estimation is in progress. */

      MSG_MED("No estimate in progress or completed.", 0, 0, 0);

      return E_FAILURE;
    }
    else
    {
      return E_IN_PROGRESS;
        /* No estimate is available yet... */
    }
  }

} /* sclk_estimate_complete */


/* <EJECT> */
/*==========================================================================

FUNCTION SCLK_ABORT_ESTIMATE

DESCRIPTION
  Abort any estimate in progress.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Uninstalls FEE ISR's.

==========================================================================*/

void sclk_abort_estimate( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sclk_stop_fee();
    /* Stop the FEE circuitry from generation FEE samples */

  if ( sclk.estimate.in_progress != SCLK_32KHZ_NO_ESTIMATE )
  {
    MSG_HIGH("Aborting estimate: %d, %ld, %ld",
      sclk.estimate.in_progress,
      sclk.estimate.sclk_accumulated,
      sclk.estimate.sysclks_accumulated);

    sclk.estimate.in_progress = SCLK_32KHZ_NO_ESTIMATE;
      /* Cancel estimate */

    sclk.estimate.sclk_accumulated     = 0;
    sclk.estimate.sysclks_accumulated  = 0;
      /* Forget any accumulated data */
  }

} /* sclk_abort_estimate( ) */
#endif /* !IMAGE_APPS_PROC */

/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_GET_FREQUENCY

DESCRIPTION
  Returns the slee[ clock frequency.

DEPENDENCIES
  sclk_freq_estimate_complete( ) must return true.

RETURN VALUE
  The sleep clock frequency, in Hz

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_get_frequency( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sclk.freq;
    /* Return the estimated slow clock frequency */

} /* sclk_get_frequency( ) */


/*===========================================================================

FUNCTION SCLK_GET_mFREQUENCY

DESCRIPTION
  Returns the sleep clock frequency in mHz.

DEPENDENCIES
  sclk_estimate_complete( ) must return true.

RETURN VALUE
  The sleep clock frequency, in mHz

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_get_mfrequency( void )
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sclk.freq_in_mHz;
    /* Return the estimated slow clock frequency in mHz */

} /* sclk_get_mfrequency( ) */

/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_BEGIN_ERROR_ADJUST

DESCRIPTION
  Records the start of a long duration calibration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sclk_begin_error_adjust
(
  sclk_sys_enum_type              unused_sclk_system
    /* System which is beginning the long duration measurement */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sclk_begin_error_adjust( ) */


/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_BOUND_ERROR

DESCRIPTION
  Looks at the duration of sleep and the error accumulated during that 
  duration and determines if this error was due to multipath or not.  
  If this error does resemble multipath then limit the amount of error to
  within the error limits expected for temperature drift.  If the error
  continues to increase/decrease of multiple sleeps then the estimate must 
  be drifting and the entire error is applied.

DEPENDENCIES
  None

RETURN VALUE
  Amount of error that should be applied to the slow clock estimate

SIDE EFFECTS
  None

===========================================================================*/

static int16 sclk_bound_32khz_error
(
  sclk_sys_enum_type              sclk_system,
    /* System which has completed the measurement */  

  uint32                          sleep_chips,
    /* The duration the error measurement was computer over in chips */

  int16                           cx16_error
    /* The amount of error which accumulated over above interval */
)
{
  sclk_boundary_type             *bound;
    /* Previous bound and number of times oob */
  
  int16                           max_error;
    /* Maximum error allowed before error is considered a multipath */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  bound = &sclk.system[sclk_system].boundary;
  
  if ( sleep_chips > SCLK_ERROR_BNDRY_SLOPE_END )
  {
    bound->oob_cnt = 0;
    return cx16_error;
      /* Slept too long to bound error, use entire correction and
         reset error */
  }
  
  /* Determine the maximum error depending on duration */
  if ( sleep_chips <= SCLK_ERROR_BNDRY_FLAT_END )
  {
    max_error = SCLK_ERROR_BNDRY_FLAT_CEIL;
    MSG_LOW("Flat region bounding %d cx16",max_error,0,0);
      /* The flat region is bounded at a constant value */
  }
  else
  {
    max_error = (uint16)MAX_SLOPED_ERROR( sleep_chips - 
                                  SCLK_ERROR_BNDRY_FLAT_END );
    MSG_LOW("Sloped region bounding %d cx16",max_error,0,0);
      /* The sloped boundary region scales with sleep time */
  }
  
  /* Attempt to eliminate multipath errors by bounding the error */
  if ( ABS( cx16_error ) > max_error )
  {
    
    if ( SAME_SIGN( cx16_error, bound->last_error ) )
    {

      if ( bound->oob_cnt < MAX_OOB_CNT )
      {
        bound->oob_cnt++;
      }
      else
      {
        if ( bound->tracking_state != SCLK_SLOW_CLOCK_ERROR_TRACKING_NORMAL )
        {
          INTLOCK();
          bound->timeout = timetick_get_safe() + 
            sclk_slow_clock_error_tracking_timeout;
          INTFREE();

          bound->tracking_state = SCLK_SLOW_CLOCK_ERROR_TRACKING_ACTIVE;
        }

        MSG_LOW("Estimate drift, ignore bounding",0,0,0);
        return cx16_error;
          /* Out of bounds too long, apply entire correction */
      }
    } 
    else
    {
      bound->last_error = cx16_error;
      bound->oob_cnt = 0;
        /* Sign changed, remember last sign and reset count */
    }
    
    if ( bound->tracking_state == SCLK_SLOW_CLOCK_ERROR_TRACKING_ACTIVE )
    {      
      timetick_type now;
      INTLOCK();
      now = timetick_get_safe();
      INTFREE();

      /* Convert the delta to a signed value.  This could get confused
       * if it isn't checked for 0x8000_0000 sclk ticks, but that's
       * ~18.2 hours, so I think this should be safe */
      if ( (int32)( bound->timeout - now ) > 0 ) 
      {
        return cx16_error;
          /* Still in active tracking mode, apply entire correction */
      }
      else
      {
        bound->tracking_state = SCLK_SLOW_CLOCK_ERROR_TRACKING_INACTIVE;
          /* Switch to normal mode */
      }
    }

    if ( cx16_error < 0 )
    {
      max_error *= -1;
        /* Ensure correct sign */
    }
    
    MSG_LOW("Bounded from %d to %d cx16",cx16_error,max_error,0);
    return max_error;
  }
  else
  {
    bound->oob_cnt = 0;
    return cx16_error;
      /* Error within bounds, reset count */
  }

}


/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_SYSCLK_ERROR_ADJUST

DESCRIPTION
  Alters the sleep clock frequency based on an sys_clk error term (e.g. the
  reacquisition slew).

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sclk_sysclk_error_adjust
(
  sclk_sys_enum_type              sclk_system,
    /* System which has completed the measurement */

  uint32                          sleep_sclks,
    /* The duration the error measurement was computed over in slow clocks */
  
  uint32                          sleep_sysclk,
    /* The duration the error measurement was computed over in chips */

  int16                           sysclk_error
    /* The amount of error which accumulated over above interval */
)
{
  sclk_system_struct_type        *system;
    /* System structure */

  int32                           sysclk_per_sclk_adj;
    /* Correction to the sysclk_per_sclk value */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  system = &sclk.system[ sclk_system ];
    /* Retrieve the correct sclk system structure */

  MSG_LOW("Slept %d sclks, %d sysclks, Err %d sysclks",
           sleep_sclks,
           sleep_sysclk,
           sysclk_error);
  
  sysclk_error = sclk_bound_32khz_error(  sclk_system, 
                                          sleep_sysclk >> CHIPX8_CHIP_SHIFT,
                                          sysclk_error * 2 ) / 2;
    /* Bounding algorithm for error feedback */
  
  if ( sysclk_error == 0 )
  { 
    return;
      /* Don't waste any more time if error is zero */
  }
  
  /* Use fixed gain algorithm if the gain is too large due to long sleep */
  if ( sleep_sclks < SCLK_ERROR_BNDRY_AGBI_ALGORITHM )
  {
    sysclk_per_sclk_adj = sysclk_error * 2;  /* TODO: Rethink */
      /* What the algorithm does is apply a gain of 2^-18 to the 
         error and apply this error directly to the estimate.  What 
         is invisible here is the statement is applying the 
         gain and then shifting the result left by one to maintain 
         precision.  This means that cx16_error now represents the 
         error in 2^-18 cx16s. */  
  }
  else
  {
    /* This is a long sleep, use fixed gain algorithm */
    
    sysclk_per_sclk_adj = fixed_divide_rounded( ABS(sysclk_error),
                                                sleep_sclks,
                                                system->bp 
                                              ) / 2;
      /* Divide by slow clocks and apply gain, do calc on unsigned value */
    
    /* Return error to signed quantity */
    if ( sysclk_error < 0 )
    {
      sysclk_per_sclk_adj = -sysclk_per_sclk_adj;
    }
  }
  
  /* Don't waste time if estimate did not change */
  if ( sysclk_per_sclk_adj != 0 )
  {
    system->sysclk_per_sclk -= sysclk_per_sclk_adj;
      /* Apply correction to sysclk_per_sclk estimate.  A positive error
         (adjustment) means we didn't sleep long enough, and sysclk_per_sclk
         needs to be smaller.  A negative error (adjustment) means we need to
         increase sysclk_per_sclk */

    MSG_MED("New sysclk/sclk = %ld + %ld / 2^%d",
            FIXED_FLOOR( system->sysclk_per_sclk, system->bp ),
            FIXED_FRAC(  system->sysclk_per_sclk, system->bp ),
            system->bp);

    (void) sclk_compute_hertz( system );
      /* Compute sclk frequency in Hertz */
  }

} /* sclk_sysclk_error_adjust( ) */


/*===========================================================================

FUNCTION SCLK_SYSCLK_ERROR_ADJUST_GSM

DESCRIPTION
  Alters the sleep clock frequency based on an sys_clk error term (e.g. the
  reacquisition slew).

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sclk_sysclk_error_adjust_gsm
(
  sclk_sys_enum_type              sclk_system,
    /* System which has completed the measurement */

  uint32                          sleep_sclks,
    /* The duration the error measurement was computed over in slow clocks */
  
  uint32                          sleep_sysclk,
    /* The duration the error measurement was computed over in chips */

  int16                           sysclk_error
    /* The amount of error which accumulated over above interval */
)
{
  /* Use the common sclk_sysclk_error_adjust api */
  /* GSM sysclk_error is reported in opposite direction, so change sysclk_error sign */
  sclk_sysclk_error_adjust(sclk_system, sleep_sclks, 
                           sleep_sysclk, -1 * sysclk_error);
} /* sclk_sysclk_error_adjust_gsm( ) */


/*===========================================================================

FUNCTION SCLK_SYSCLK_ERROR_ADJUST_WCDMA

DESCRIPTION
  Alters the sleep clock frequency based on an sys_clk error term (e.g. the
  reacquisition slew).

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sclk_sysclk_error_adjust_wcdma
(
  sclk_sys_enum_type              sclk_system,
    /* System which has completed the measurement */

  uint32                          sleep_sclks,
    /* The duration the error measurement was computed over in slow clocks */
  
  uint32                          sleep_sysclk,
    /* The duration the error measurement was computed over in chips */

  int16                           sysclk_error
    /* The amount of error which accumulated over above interval */
)
{
  /* Use the common sclk_sysclk_error_adjust api */
  sclk_sysclk_error_adjust(sclk_system, sleep_sclks, 
                               sleep_sysclk, sysclk_error);
} /* sclk_sysclk_error_adjust_wcdma */

/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_TO_SYSCLKS

DESCRIPTION
  Determine the number of sys_clk's for a given number of slow clocks

DEPENDENCIES
  A good slow clock frequency estimate.

RETURN VALUE
  sys_clks.

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_to_sysclks
(
  sclk_sys_enum_type              sclk_system,
    /* System conversion is being done for */

  uint32                          sclks
    /* Duration to be converted into sysclks */
)
{
  sclk_system_struct_type       * system;
    /* System structure */

  qword                           duration;
    /* Duration, in various units */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  system = &sclk.system[ sclk_system ];
    /* Retrieve the correct sclk system structure */


  /* Since "sclk * sysclk_per_sclk" will exceed a uint32,
     the calculations are performed in quadword math. */


  qw_set( duration, 0, sclks);
    /* Duration in sleep clocks */

  qw_mul( duration, duration, system->sysclk_per_sclk );
    /* Duration in sys_clk's, in fixed binary point format */

  qw_shift( duration, -system->bp );
    /* Convert from fixed binary point format to sysclk's */

  if ( qw_hi( duration ) > 0 )
  {
    ERR("Overflow: sclks=%ld", sclks, 0, 0);
    return 0xFFFFFFFFuL;
  }
  else
  {
    return qw_lo( duration );
      /* Result fits in a uint32 */
  }

} /* sclk_to_sysclks */


/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_FROM_SYSCLKS

DESCRIPTION
  Determine the number of slow clocks which correspond to the given number
  of sysclks.
  
  2 slow clocks worth of sys_clk's are excluded from the conversion to
  slow clocks, and included in the remainder_sysclk value.

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  slow clocks

SIDE EFFECTS
  None

===========================================================================*/

uint32 sclk_from_sysclks
(
  sclk_sys_enum_type              sclk_system,
    /* System conversion is being done for */

  uint32                          duration_sysclks,
    /* Duration to be converted into slow clocks */

  uint16 *                        remainder_sysclks
    /* Residual sysclk count that could not be converted to slow clocks */
)
{
  sclk_system_struct_type       * system;
    /* System structure */

  uint32                          sclks;
    /* Sleep clocks */

  uint32                          sysclk_reserve;
    /* Sys_clk reserve - for slow clock phase */

  uint32                          sysclk_rem;
    /* Sys_clk remainder */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  system = &sclk.system[ sclk_system ];
    /* Retrieve the correct sclk system structure */


  sysclk_reserve  = sclk_to_sysclks( sclk_system, 2 );
    /* Reserve 2 sclk pulses worth of sysclk's for phase determination */

  sclks = fixed_divide( duration_sysclks - sysclk_reserve,
                        system->sysclk_per_sclk,
                        system->bp,
                        &sysclk_rem);
    /* Compute total number of sleep clocks to sleep for, based on the
       total duration of sleep (in chipx16) excluding reserve. */

  *remainder_sysclks = (uint16) (FIXED_ROUND(sysclk_rem, system->bp)
                     + sysclk_reserve);
    /* Combine sysclk reserve with division remainder */

  return sclks;

} /* sclk_from_sysclks( ) */



/* <EJECT> */
/*===========================================================================

FUNCTION SCLK_TO_TIMESTAMP

DESCRIPTION
  Given a base timestamp, and an sclk offset, determine the resulting
  timestamp.

DEPENDENCIES
  A valid slow clock frequency estimate.

RETURN VALUE
  None (Parameter 'timestamp' modified)

SIDE EFFECTS
  None

===========================================================================*/

void sclk_to_timestamp
(
  qword                           timestamp,
    /* In: Base timestamp;  Out: Timestamp incremented by sclks */

  uint32                          sclks
    /* Number of slow clocks to advance timestamp by */
)
{
  qword                           delta;
    /* Time elapsed since original timestamp (in various units) */

  uint32                          cx32;
    /* Lower bits of timestamp (in chipx32's) */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We cannot directly add timestamps together, since the upper
     48-bits and the lower 16 bits are not in compatible units.
     Instead, we must remove and zero the lower 16 bits of one
     timestamp, and handle the removed bits seperately.
     Since the lower 16 bits of one of the timestamps is zero'd,
     addition will be valid since there can be no carry. */

  cx32 = qw_lo(timestamp) & 0xffffuL;
    /* Get lower 16 bits of original timestamp */

  qw_dec( timestamp, cx32 );
    /* Zero the lower 16 bits of original timestamp */



  /* Convert slow clock count into chipx32s, and add in the lower
     16 bits of the original timestamp, which are in chipx32s. */

  qw_set(delta, 0, sclks);
    /* Set time elapse since original timestamp, in slow clocks */

  qw_mul(delta, delta, sclk.time.cx8_per_sclk);
    /* ... and now in chipx8s, fixed point format */

  qw_shift(delta, +2 - sclk.time.bp);
    /* Shift up by 2 to convert to chipx32's, and
       shift down by sclk.time.bp to convert from fixed point format. */

  qw_inc(delta, cx32 );
    /* Add in chipx32 adjustment from original timestamp */



  /* Convert from chipx32's to ts_timestamp format, which entails a
     1.25ms count in the upper 48 bits, and a chipx32 count in the
     lower 16 bits. */

  cx32 = qw_div( delta, delta, 0xC000u);
    /* Convert from cx32's to 1.25ms units, save remaining cx32's */

  qw_shift( delta, 16);
    /* Shift 1.25ms units up 16 bits, to match ts_timestamp format */

  qw_inc( delta, cx32 );
    /* Transfer remaining cx32 into lower 16 bits */



  /* We have a (modified) base time, in timestamp format, and
     an offset from that time, in timestamp format.
     We add these timestamps together to get the new timestamp.
     Addition is valid, since the lower 16 bits of the first
     timestamp are zero (see above). */

  qw_add( timestamp, timestamp, delta );
    /* Add delta to original timestamp, to generate new timestamp */

} /* sclk_to_timestamp( ) */


/*===========================================================================

FUNCTION SCLK_PROPAGATE_ESTIMATE

DESCRIPTION
  Convert the FEE result generated using sys_clk from_id to the equivalent
  estimate for sys_clk to_id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sclk_propagate_estimate( sclk_sys_enum_type from_id, 
                              sclk_sys_enum_type to_id )
{
   /* TODO - clean this up to use info in sclk. */
   if ( (from_id == SCLK_GSM) && (to_id == SCLK_WCDMA) )
   {
      uint32 val;

      /* 13MHz -> 30.72MHz (3072/1300 = 768/325 = (2^8*3)/(5^2*13)) 
       * Calulation is done like this to avoid overflow or
       * too much underflow. Comments show significant bits.
       */
      val = sclk.system[from_id].sysclk_per_sclk; /*  9.18 */
      val *= 16UL;                                /* 13.14 */
      val /= 25UL;                                /*  9.18 */
      val *= 16UL;                                /* 13.14 */
      val /= 13UL;                                /* 10.18 (blink) */
      val *= 3UL;                                 /*  9.16 */
      sclk.system[to_id].sysclk_per_sclk = val;
   }
   else
   {
      MSG_ERROR("sclk_propagate_error(): unsupported ids %d->%d",
                from_id, to_id, 0);
   }
}

