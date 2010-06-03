#ifndef CLK_H
#define CLK_H
/*===========================================================================

           C L O C K   S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for accessing
  the clock services.

Copyright (c) 1990, 1991, 1992 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1993, 1994, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1996, 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006             by QUALCOMM, Incorporated.  All Rights Reserve
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM6050/vcs/clk.h_v   1.5   06 Mar 2002 14:57:34   donb  $
  $Header: //depot/asic/qsc6055/drivers/clk/clk.h#2 $ $DateTime: 2007/02/08 18:52:01 $ $Author: cryan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/07   bmg     C++ compatible function prototypes
07/28/06   dyc     Have RUMI used clk_pause() for clk_busy_wait()

07/23/05   wd      L4 Merge.
08/23/04   drh     Change clk_busy_wait to support Boot Loader correctly.
07/24/04   drh     Change defines for boot loader and JNAND.  Change
                   clk_busy_wait for JNAND
05/25/04   ap      Merged support to use TIME_VU.
03/12/04   ajn     clk_busy_wait: "do { ... } while (0)" --> "clk_pause( )"
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
08/27/03   ajn     Functionality moved to the Time Service Subsystem
07/07/03   ejv     Mainline FEATURE_ENHANCED_STANDBY_III.
03/06/02   dlb     Added SLOW_CLK_PAUSE macro to take milliseconds.
02/19/02   dlb     Modified clk_busy_wait to allow it to take larger values.
11/30/01   dsb     Ported for MSM6050.
05/30/01   ajn/dsb Added usr_data_ptr to clock callback timers.  Added
                   clk_reg2 in support of this.  Removed T_CPU_CLK_MULT and
                   T_CPU_CLK_DIV.
02/20/01   dsb     Added clk_set_mclk_parms.  Added support for new
                   clk_long_busy_wait. Removed unnecessary constants left
                   over from the previous clk_long_busy_wait.
12/11/00   rmd     Adjusted PAUSE TIMER register value calculation to support
                   4/3 TCXO.
10/06/00   dsb     Mainlined up to and including T_MSM31.  Mainlined T_ARM.
                   Removed TG== code.  Improved definition of
                   CLK_WAIT_CORE_TICKS (depends on TCXO now).  Added better
                   calculation for CLK_WAIT_OVERHEAD_DELAY - it now takes the
                   CPU multipler/divider (if any) into account.
12/13/99   mk      Changed SLOW_CLK_PAUSE macro to load the pause timer.
12/03/99   ajn     Added function for altering catnap lengths.
09/11/99   rm/mk   Added FEATURE_UP_CLOCK_SWITCHING support.
07/15/99   mk      Added MSM3100 support for clk_busy_wait(), timing is based
                   on TCXO(~20MHz)/1ASB_DECODE/1WS or TCXO(~20MHz)/2WS.
10/01/98   scdb    Merge all ARM changes into the mainline
09/01/98   scdb    Added _cdecl to some functions to save ROM
05/08/98   scdb    Removed obsolete declaration of clk_set_baud_clk
03/02/98   scdb    First round of walkthrough changes, mostly cosmetic
03/01/98   scdb    Deleted clk_mode_ctl
                   Added clk_disable, clk_catnap_enable, clk_unexpected_enable
11/11/97   scdb    Changed certain #ifdefs from T_MSM2P to "T_MSM2"
10/30/97   scdb    Completely revamped the way that system time is maintained.
                   Added CLK_SAVE_TICKSTATE, clk_uptime_ms,
                   clk_predict_first_wakeup, clk_predict_next_wakeup,
                   clk_calculate_backlog, clk_catchup_backlog,
                   clk_inform_searchparams.
                   Removed clk_uptime_suspend, clk_uptime_suspended
                   Removed archaic declaration for clk_pwr_up
                   Removed clk_uptime_update
                   clk_ms_after_tick is dead code and has been commented out.
                   Though the code for them has not changed, the effective
                   behavior of clock callbacks and rex timers is different.
09/18/96   rdh     Added alternate clk_up_time maintenance technique.
08/15/95   jah     Added clk_mode_ctl() prototype & clk_mode_type, for MSM2P.
06/27/95   jah     Added tramp_call_ptr_type to call-back, for MSM2P
07/25/94   jah     Added prototype for clk_busy_wait().
06/14/93   jah     Added day_of_week to the end of clk_julian_type.
05/20/93   jah     Deleted CLK_DIV_ACP_VC, and added CLK_DIV_ACP_MC and
                   CLK_DIV_ACP_RX to really fix up the ACP speed problems.
05/19/93   jah     Added CLK_DIV_ACP_VC for ACP Voice channel speed-ups.
03/31/93   jah     Updated comments.
03/16/93   jah     Added CLK_DIVIDE() macro.
03/16/93   jah     Changed call-back to use int4 instead of int2, and added
                   clk_divide().
09/16/92   jah     Corrected documentation in clk_reg().
08/24/92   jah     Added clk_uptime()
05/12/92   jah     Revised clk_reg/dereg to sorted-linked-list algorithm.
02/28/92   jah     Ported from brassboard and added call-back services

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "target.h"     /* Target specific definitions          */
#include "bsp.h"        /* Board support package                */
#include "qw.h"         /* Definition of qword                  */
#include "tramp.h"      /* Interrupt trampoline service         */
#include "msm.h"        /* Needed for the pause timer           */
#if defined(BOOT_LOADER)
#include "clkrgmi_msm.h" /* For CLKRGM_HCLK_FREQUENCY_IN_MHZ     */
#endif

/*
 * Definitions for forwarding time-specific functions to the
 * Time Service Subsystem
 */
#include "time_svc.h"
#ifdef FEATURE_L4_KERNEL
#error code not present
#else
#include "timer.h"
#endif
#include "time_jul.h"

/*
 * It has been determined that there is a bug in MSM 2.2 which will also be
 * present in MSM2.3, having to do with the generator of the 5 millisecond
 * interrupt. The effect of the bug is that the periodic tick isn't really
 * happening every 5 milliseconds; it's just a tadge slow.
 *
 * The following symbol enables code which occasionally adds a bit of extra
 * time, in a controlled fashion, to uptime to compensate.
 *
 * This hardware bug will be fixed in MSM 3.
 * However, note that some of the capability will still be needed, since it
 * also compensates for the fact that TCXO changes speed when the RF
 * hardware is turned off.
 */
#define CLK_TIMENUDGE

/* If clk.h defines CLK_REGIME_UP_SLEEP__MCLK_HCLK, then the function
   clk_regime_uP_sleep needs mclk & hclk parameters.  Otherwise, the
   function takes no arguments, and instead uses global variables. */

#define CLK_REGIME_UP_SLEEP__MCLK_HCLK


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Timer group "clock callbacks" will belong to.  This group is disabled when
** the 5ms tick was disabled, and re-enabled when the 5ms tick was re-enabled.
** Thus timers in the group emulate the historical behaviour of the clock
** callbacks based on the 5ms tick. [Used in clk_def() macro in clk.h]
*/
extern timer_group_type clk_callback_group;

/* Clock call-back structure.  Note that although this structure is allocated
** by the routine calling clk_reg(), none of the fields should be modified or
** accessed by the calling module.  Any initialization or manipulation of this
** structure type is to be done by the routines in this module.
*/
#define clk_cb_type timer_type


/* Defer to time_jul's definition for Julian time structure */
#define clk_julian_type time_julian_type


/*
 * clk_uptime_type holds an "uptime" value, which is a structure containing a
 * time interval of less than 100 years (but nonetheless easier
 * to manipulate than a quad-word).
 * In proper use, the 'ms' field should be less than 1000.
 */
typedef struct {
  dword secs;    /* Number of seconds since the phone came up */
  word  ms;      /* Number of milliseconds in the most recent second, */
                 /* not yet finished or included in 'secs' */
} clk_uptime_type;


#if defined(BOOT_LOADER)

#define  clk_mclk_freq_mhz CLKRGM_HCLK_FREQUENCY_IN_MHZ
#else
/* Current MCLK frequency (in MHz), used for clk_busy_wait */
extern dword clk_mclk_freq_mhz;
#endif


/* Milliseconds between calls to rex_tick(), updates to the time-of-day clock,
** and calls to interval call-back routines.
*/
#define CLK_MS_PER_TICK         5

/*
** The number of milliseconds in a second
*/
#define CLK_MS_PER_SEC          1000

/* The hardware clock is driven by an external input which this service
** refers to as the baud clock.  This divisor converts baud to cycles per
** CLK_MS_PER_TICK, which is the clock update interval used by this service.
** Note that this has implications on what clock tick intervals can be chosen.
** This number effects the actual hardware clock, so it must be accurate.
*/
#define CLK_BAUD_DIV    (1000 / CLK_MS_PER_TICK)

/* Clock divide control is "voted", so that multiple callers can ask
** for the clock to divide or run normally.  A counting semaphore is
** not appropriate due to the problems with the callers counting
** divide/restore's properly.  Each caller has a specifed mask which
** identifies it.
**
** There is a mask for 'nobody', which will update the hardware for
** control flows that want to update the hardware (e.g. after an interrupt)
** but do not have a vote in whether or not the clock is divided.
*/
#define CLK_DIV_NOBODY  0       /* Mask for no-change                   */
#define CLK_DIV_SND     1       /* Sound Task                           */
#define CLK_DIV_ACP     2       /* Analog Call Processing, general      */
#define CLK_DIV_CDMA    4       /* CDMA                                 */
#define CLK_DIV_ACP_MC  8       /* Analog Call Processing, main control */
#define CLK_DIV_ACP_RX  16       /* Analog Call Processing, receive      */

#define CLK_ALL_DIV_MASKS \
  (CLK_DIV_SND | CLK_DIV_ACP | CLK_DIV_CDMA | CLK_DIV_ACP_MC | CLK_DIV_ACP_RX )

#ifdef FEATURE_USES_CLK_BUSY_WAIT_NOPS
extern uint32 uses_clk_buys_wait_nops;
#endif 



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================

FUNCTION CLK_INIT

DESCRIPTION
  Initialize the timer hardware and initialize the time-of-day to 0.
  Auto-register rex_tick() to be called each CLK_MS_PER_TICK.

DEPENDENCIES
  Depends on interrupt priority CLK_INT_TIM_CTL being disabled when this
  routine is called.

RETURN VALUE
  None

SIDE EFFECTS
  Enables timer 0, disables timers 1 & 2.

===========================================================================*/
extern void clk_init( void );


/*===========================================================================

FUNCTION CLK_DEF

DESCRIPTION
  Initialize the passed clock call-back structure.  This routine must be
  called prior to passing the call-back structure to clk_reg().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define clk_def(timer) timer_def2( timer, &clk_callback_group )



/*===========================================================================

FUNCTION CLK_REG

DESCRIPTION
  Register a call-back structure to have a specified routine be called at
  the first interval (ms_first), and periodically every ms_periodic
  milliseconds after the first interval as described below.  Note that the
  order in which routines are registered does not define the order in which
  they are called-back.

DEPENDENCIES
  Clk_def() must have been called to initialize the call-back structure prior
  to passing it to this routine.

RETURN VALUE
  None

SIDE EFFECTS
  Registers a routine to be called at a later time by the clock tick ISR.

===========================================================================*/

#define clk_reg(timer, func, ms_first, ms_period, repeat) \
  do { timer_reg(timer, (timer_t2_cb_type)func, (timer_cb_data_type) NULL,\
       ms_first, ms_period); \
  } while ( 0 );



/*===========================================================================

FUNCTION CLK_REG2

DESCRIPTION
  Register a call-back structure to have a specified routine be called at
  the first interval (ms_first), and periodically every ms_periodic
  milliseconds after the first interval as described below.  Note that the
  order in which routines are registered does not define the order in which
  they are called-back.

DEPENDENCIES
  Clk_def() must have been called to initialize the call-back structure prior
  to passing it to this routine.

RETURN VALUE
  None

SIDE EFFECTS
  Registers a routine to be called at a later time by the clock tick ISR.

===========================================================================*/

#define clk_reg2(timer, func, ms_first, ms_period, repeat, data) \
  do { timer_reg(timer, (timer_t2_cb_type)func, (timer_cb_data_type)(data),\
       ms_first, ms_period); \
  } while ( 0 );


/*===========================================================================

FUNCTION CLK_DEREG

DESCRIPTION
  De-register a routine registered by calling clk_reg.

DEPENDENCIES
  Clk_def() must have been called to initialize the call-back structure
  prior to passing it to this routine.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define clk_dereg(timer)  (void) timer_clr(timer, T_NONE)



/*===========================================================================

FUNCTION CLK_READ_SECS

DESCRIPTION
  This procedure returns the current value of the time-of-day clock rounded
  to the nearest second.

DEPENDENCIES
  Clk_init() must have already been called.

RETURN VALUE
  The current value of the time-of-day clock rounded to the nearest second.
  If the phone has never received and processed a Sync Channel Message
  from a CDMA cell, the value returned will be sometime in 1980.

SIDE EFFECTS
  None

===========================================================================*/

#define clk_read_secs() time_get_secs()


/*===========================================================================

FUNCTION CLK_READ_MS

DESCRIPTION
  This procedure returns the current value of the time-of-day clock in
  millisecond units.

DEPENDENCIES
  Clk_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define clk_read_ms(ms) time_get_ms(ms)


/*===========================================================================

FUNCTION CLK_UPTIME

DESCRIPTION
  Return the number of seconds the phone has been on.

DEPENDENCIES
  None

RETURN VALUE
  Number of seconds since the phone powered on. The value is not rounded.

SIDE EFFECTS
  None

===========================================================================*/

#define clk_uptime() time_get_uptime_secs()



/*===========================================================================

FUNCTION CLK_PREDICT_FIRST_WAKEUP

DESCRIPTION
  This is called during setup of the first catnap of a series of catnaps
  making up a "slotted sleep" or "deep sleep". The purpose is to calculate,
  and store, the expected "uptime" value when the wakeup interrupt happens
  at the end of the catnap.

DEPENDENCIES
  This function must only be called from sleep_set_sleep_timer.

RETURN VALUE
  Amount of time, in 4.92 Mhz clocks, that the wakeup interrupt should be
  shifted by in order to prevent a predicted collision with a periodic tick.


SIDE EFFECTS
  None

===========================================================================*/

#define clk_predict_first_wakeup(length) 0  /* No op */



/*===========================================================================

FUNCTION CLK_INFORM_SEARCHPARAMS

DESCRIPTION
  Used to inform CLOCK of the expected duration of catnaps.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function arms clk_predict_first_wakeup to run once.

===========================================================================*/

#define clk_inform_searchparams(catnap_length) /* No op */



/*===========================================================================

FUNCTION CLK_ALTER_CATNAP_LENGTH_MS

DESCRIPTION
  Used to inform CLOCK of a change duration of catnaps.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define clk_alter_catnap_length_ms(new_catnap_length_ms) /* No op */

/*===========================================================================

FUNCTION CLK_ALTER_CATNAP_LENGTH

DESCRIPTION
  Used to inform CLOCK of a change duration of catnaps.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define clk_alter_catnap_length(new_catnap_length) /* No op */



/*===========================================================================

FUNCTION CLK_DISABLE

DESCRIPTION
  This disables the periodic interrupt. Uptime and time-of-day freeze,
  REX timers and clock callbacks cease to expire, and power consumption
  decreases markedly.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_disable(void);


/*===========================================================================

FUNCTION CLK_ENABLE

DESCRIPTION
  Reenable the periodic tick when any interrupt turns the phone on while the
  phone is asleep.  Some time accounting takes place, REX timers and clock
  callbacks can expire, uptime/TOD are brought up to date.

DEPENDENCIES
  It should be called from interrupt service routine which can
  interrupt sleep, and only from them.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_enable(void);


/* Emulate older "catnap" and "unexpected" enable interfaces
*/
#define clk_catnap_enable(remaining) clk_enable()
#define clk_unexpected_enable(void)  clk_enable()



/*===========================================================================

FUNCTION CLK_UPTIME_MS

DESCRIPTION
  Returns the phone uptime in units of milliseconds via a parameter.

DEPENDENCIES
  clk_init must have been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define clk_uptime_ms(ms) time_get_uptime_ms(ms)



/*===========================================================================

FUNCTION CLK_SECS_TO_JULIAN

DESCRIPTION
  This procedure converts a specified number of elapsed seconds
  since the base date to its equivalent Julian date and time.

DEPENDENCIES
  None

RETURN VALUE
  The specified Julian date record is filled in with equivalent date/time,
  and returned into the area pointed to by ptr.

SIDE EFFECTS
  None

===========================================================================*/

#define clk_secs_to_julian(secs,ptr)  time_jul_from_secs(secs,ptr)



/*===========================================================================

FUNCTION CLK_JULIAN_TO_SECS

DESCRIPTION
  This procedure converts a specified Julian date and time to an
  equivalent number of elapsed seconds since the base date.

DEPENDENCIES
  None

RETURN VALUE
  Number of elapsed seconds since base date.

SIDE EFFECTS
  None

===========================================================================*/

#define clk_julian_to_secs(ptr)       time_jul_to_secs(ptr)



/*===========================================================================

FUNCTION CLK_DIVIDE

DESCRIPTION
  Petition to divide the clock, or demand that the clock no longer
  be divided.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If all voters agree to divide the clock, it is divided.  If any
  disagree, the clock is restored to full-rate.

===========================================================================*/
extern void clk_divide
(
  word mask,
    /* ballot of a clock divide request associated with caller */
  boolean divide
    /* True = divide clock, False = restore clock.             */
);


/*===========================================================================

MACRO CLK_DIVIDE

DESCRIPTION
  A target-independent wrapper for clk_divide().  Compiles as a single ';'
  on Mobiles, as a clk_divide() call on Portables.

PARAMETERS
  xxx_mask      Mask of bits voting to turn down/up the clock
  xxx_divide    Value (TRUE=divide or FALSE=don't divide)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define CLK_DIVIDE(xxx_mask,xxx_divide)


/*===========================================================================

FUNCTION CLK_SET_MCLK_PARMS

DESCRIPTION
  Accessor function to set the CLK module variables holding the MCLK
  frequency (in MHz).

  This value can be used to calculate MCLK timing.  eg. the PAUSE_TIMER in
  clk_long_busy_wait.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_set_mclk_parms(dword freq_mhz);


/*===========================================================================

FUNCTION CLK_PAUSE

DESCRIPTION
  This function is used to implement the clock busy wait, instead of
  expanding the entire clock busy wait code inline.

  Conversion from microseconds to pause time is done in the clk_busy_wait
  macro, to allow the preprocessor to compute the value as a constant, if
  clk_mclk_freq_mhz is a #define constant.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_pause(int pause_time_remaining);


/* Time constants for ARM clock busy wait using the pause timer. */

/* Number of clocks to setup the pause timer in HW (should be subtracted
   off of the software programming of PAUSE_WAIT_TIMER_WH) */
#define CLK_BUSY_WAIT_OVHD_CLKS   12

#ifdef FEATURE_UP_CLOCK_SWITCHING

  /* Pause a specific number of miliseconds using the pause timer.
     Calculation based on 32kHz sleep clock. */
  #define SLOW_CLOCK_CORE_TICKS 32

  /* Faster implemenation than clk_busy_wait, as we *know* the the
     MCLK source is the sleep xtal */
  #define SLOW_CLK_PAUSE_US(us)  \
              HWIO_OUT (PAUSE_TIMER, (us)* SLOW_CLOCK_CORE_TICKS / 1000 \
                                        - CLK_BUSY_WAIT_OVHD_CLKS + 1)

  /* Similar to SLOW_CLK_PAUSE_US, but in milliseconds, for backwards
     compatibility */
  #define SLOW_CLK_PAUSE(ms)  \
              HWIO_OUT (PAUSE_TIMER, (ms)* SLOW_CLOCK_CORE_TICKS \
                                        - CLK_BUSY_WAIT_OVHD_CLKS + 1)

#endif /* FEATURE_UP_CLOCK_SWITCHING */

/* Convert micro-seconds to a value for the Pause Timer */
#define CLK_MICRO_SECS_TO_PAUSE_TIMER_VAL(us)  \
                 ((us)*clk_mclk_freq_mhz - CLK_BUSY_WAIT_OVHD_CLKS + 1)

/* Maximum time to program the pause timer in */
#ifdef FEATURE_USES_CLK_BUSY_WAIT_NOPS              
  #define CLK_BUSY_WAIT_NOPS_INITIALIZED_VAL 0x1234CAFE

  /* Convert micro-seconds to a value for number of NOPs needed for
     clk bysy wait */
  #define CLK_MICRO_SECS_TO_NOP_VAL(us)  ((us)*clk_mclk_freq_mhz )

  #define CLK_MAX_PAUSE_TIME   CLK_MICRO_SECS_TO_NOP_VAL(150)

  /* Current ARM clk frequency (in MHz), used for clk_busy_wait_nops */
  extern void clk_busy_wait_nop_loops(uint32 num_nops);

#else

  #define CLK_MAX_PAUSE_TIME   CLK_MICRO_SECS_TO_PAUSE_TIMER_VAL(150)

#endif  /* FEATURE_USES_CLK_BUSY_WAIT_NOPS */


#if defined(BOOT_LOADER) || defined (FEATURE_L4_KERNEL) ||\
    defined(BUILD_JFLASH) || defined(BUILD_ARMPRG)


/* Perform a busy wait for a number of microseconds by programming the
   MSM's pause timer for Boot Loader */
#define clk_busy_wait(us)                                             \
     do {                                                             \
       int wait_remaining = CLK_MICRO_SECS_TO_PAUSE_TIMER_VAL(us);    \
       int max_pause_time = CLK_MICRO_SECS_TO_PAUSE_TIMER_VAL(150);   \
       while (wait_remaining > 0)                                     \
       {                                                              \
         int pause_time = MIN(max_pause_time, wait_remaining);        \
         HWIO_OUT (PAUSE_TIMER, pause_time);                          \
         wait_remaining -= pause_time;                                \
       }                                                              \
     } while(0)


#elif defined(BUILD_JNAND) || defined(BUILD_NANDPRG) || defined(BUILD_BOOT_CHAIN) || \
      defined(T_RUMI_SC2X) 

/* Call local function in JNAND or NANDPRG to perform a pause */
#define clk_busy_wait(us)                                             \
        clk_pause( (us) )

#elif defined (FEATURE_USES_CLK_BUSY_WAIT_NOPS)

/* Perform a busy wait for a number of microseconds by programming the
   MSM's pause timer for AMSS */
extern void clk_busy_wait(uint32 us);

#else

#define clk_busy_wait(us)                                             \
        clk_pause( CLK_MICRO_SECS_TO_PAUSE_TIMER_VAL(us) )


#endif




/*===========================================================================

FUNCTION CLK_NANO_WAIT

DESCRIPTION
  Perform a very short busy wait.  Takes about 200ns.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern volatile byte clk_nano_location;
#define clk_nano_wait() (clk_nano_location = 1)

#ifdef __cplusplus
}
#endif

#endif /* CLK_H */

