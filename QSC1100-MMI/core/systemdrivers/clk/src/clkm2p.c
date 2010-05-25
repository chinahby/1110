/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 M S M 2 p   T I M E R   S P E C I F I C

                       C L O C K   S E R V I C E S

GENERAL DESCRIPTION
  This module contains procedures to suppor clock functions specific to
  implementations using the MSM2p time tick.

EXTERNALIZED FUNCTIONS
  clk_init
    Initialize the timer hardware and software

REGIONAL FUNCTIONS
  clk_tickphase
    Returns the number of milliseconds since the last periodic interrupt
    (whether it was enabled or not).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The clock must be first initialized by calling clk_init().  The time should
  then be set by calling clk_set_*().  Time will not count forward until
  interrupts are enabled.

Copyright (c) 1994, 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msm6800/drivers/clk/clkm2p.c#5 $ $DateTime: 2005/05/19 10:42:11 $ $Author: rsattari $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/04   ap      Merged support to use TIME_VU.
11/14/03   ajn     Rex timers moved to Time Services Subsystem, and the 5ms
                   tick is removed for MSM6500+ targets.
08/27/03   ajn     Functionality moved to the Time Service Subsystem.
02/20/01   dsb     Moved the inclusion of msg/err.h files to only occur if
                   CLK_TIMETRACK_DEBUG is defined.
10/31/00   rmd     Removed BSP_TCXO_19P2_MHZ, BSP_TCXO_19P8_MHZ and
                   BSP_TCXO_19P68_MHZ which are already defined in bsp.h
10/09/00   dsb     Mainlined up to and including T_MSM31.  Removed TG== code.
08/22/00   dsb     Added BSP support for TCXO frequency.
06/08/00   jc      Compensate the timer_tick for TCXO =19.2 or 19.8.
11/01/99   mvl     Moved TIMETEST code for 1PPS from clk_tick_isr()
                   to clk_uptime_add(). The 1PPS now occurs ones per second
                   instead of every 5ms.
10/04/98   scdb    Merge ARM changes into the main line
03/02/98   scdb    incorporation of code review changes
03/01/98   scdb    clk_mode_ctl deleted
10/30/97   scdb    Added function clk_tickphase.
                   There has been a complete redesign of how time is maintained
                   in the phone. This affected clk_tick_isr.
09/18/96   rdh     Added alternate clk_up_time maintenance technique.
08/15/95   jah     Added clk_mode_ctl()
07/25/94   jah     Initial revision

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Common Definitions and types         */
#include "qw.h"         /* Quad-Word prototypes                 */
#include "tramp.h"      /* Interrupt trampoline service         */
#include "clk.h"        /* Clock Services header                */
#include "clki.h"       /* Clock Services internal header       */
#include "target.h"     /* Target specific definitions          */
#include "rex.h"        /* REX real-time executive services     */
#include "bsp.h"        /* Board support package                */
#include "customer.h"   /* Feature customization                */
#include "msm.h"        /* Declares CLK_TICK_CTL for MSM 3      */
#include "clkregim.h"   /* Clock regime controls                */

#ifdef CLK_TIMENUDGE
#include "task.h"
#include "sleep.h"
#endif

/*
** Uncomment the following define if you want periodic reports on the
** uptime value relative to timestamp. This is for debugging only.
*/
/*
 * #define CLK_TIMETRACK_DEBUG
 */

#ifdef CLK_TIMETRACK_DEBUG
#include "qw.h"
#include "msg.h"
#include "err.h"
#endif



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Values for the time tick control register.
**
** 1.25, 2.5, 5, and 10 millisecond tick intervals.
*/
#define CLK_TICK_1_25MS_V  0x00
#define CLK_TICK_2_5MS_V   0x01
#define CLK_TICK_5MS_V     0x02
#define CLK_TICK_10MS_V    0x03

#ifdef CLK_TIMETRACK_DEBUG
clk_uptime_type last_time;
   /* Contains the last time that we generated a MSG to permit tracking */
   /* of UPTIME relative to TIMESTAMP */
#endif


#ifndef FEATURE_USE_TIME_VU
/*===========================================================================

  FUNCTION CLK_TICK_ISR

  DESCRIPTION
    This function is the clock tick interrupt handler.  It increments
    the second and millisecond time, and calls clk_tick_handler() to handle
    the call-backs.

  DEPENDENCIES
    Called from the Trampoline ISR.

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void clk_tick_isr( void )
{
  uint32  isave;                  /* Saved copy of PSW for intlock/free  */

  INTLOCK_SAV( isave );         /* Disable interrupts                  */

  /* Tell REX that time has passed */
  rex_tick( CLK_MS_PER_TICK );

  INTFREE_SAV( isave );         /* Enable interrupts             */

} /* end of clk_tick_isr */

#endif /* !FEATURE_USE_TIME_VU */


/*===========================================================================

FUNCTION CLK_INIT

DESCRIPTION
  Initialize the timer hardware and initialize the time-of-day to 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_init( void )
{
#ifdef CLK_TIMETRACK_DEBUG
  last_time.secs = 0;   /* debugging */
  last_time.ms = 0;     /* debugging */
#endif /* CLK_TIMETRACK_DEBUG */

  /* Configure the real time tick to 5 milliseconds
  */
  HWIO_OUT( TIME_TICK_CTL, CLK_TICK_5MS_V );

#ifdef FEATURE_USE_TIME_VU

  tramp_set_isr( TRAMP_TICK_ISR, NULL );

  #ifdef FEATURE_DUAL_CLK_RGM
  clk_regime_msm_disable( CLK_RGM_GEN_M );
  #else
  clk_regime_disable( CLK_RGM_GEN_M );
  #endif

#else
  /* Clear + Un-mask the Real-Time Tick interrupt and install the ISR
  */
  tramp_set_isr( TRAMP_TICK_ISR, clk_tick_isr );

#endif /* !FEATURE_USE_TIME_VU */

} /* end of clk_init */


/*===========================================================================

FUNCTION CLK_TICKPHASE

DESCRIPTION
  This function reads the MSM 2 RT tick counter register and calculates
  from that the number of milliseconds since the last time a periodic tick
  took place.

DEPENDENCIES
  The calling code is responsible for masking interrupts before calling
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void clk_tickphase(clk_tickphase_type *retval)
{
  byte counter;      /* 8 most significant bits of the 16-bit tick counter */
  word milliseconds; /* Accumulator for the number of ms after the tick    */
  byte sample_1;     /* the first of three samples */
  byte sample_2;     /* the second of three samples */
  byte sample_3;     /* the third and last of three samples */


  /*
  ** Sample the register three times. What's critical is
  ** to get the first one in as soon after we're invoked
  ** as possible. It's also important that the third read happen
  ** less than 52 microseconds after the first. So interrupts MUST
  ** be locked before this function is invoked.
  **
  ** Sample the 8 most significant bits of the 16-bit tick counter three times.
  ** The value rolls over every 52 microseconds, and is not valid during a
  ** roll. The roll takes less than 203.3ns, so as long as there's enough
  ** delay between successive 'inp' operations, at least one of the values
  ** will be valid.
  **
  ** Actually, the time taken to store the value into the variable was probably
  ** enough of a pause, but we'll toss in an explicit delay just to be
  ** paranoid.
  */
  sample_1 = (byte) HWIO_IN( TIME_TICK_INT_MSB );

  /*
  ** Pause for a tadge to let the counter ripple if it needs to.
  */
  clk_nano_wait ();

  sample_2 = (byte) HWIO_IN( TIME_TICK_INT_MSB );

  /*
  ** Pause for a tadge to let the counter ripple if it needs to.
  */
  clk_nano_wait ();

  sample_3 = (byte) HWIO_IN( TIME_TICK_INT_MSB );

  /*
  ** Now that we have our three samples, we can process them at our leisure.
  **
  ** If sample_1==sample_2 then that's our number, because it
  ** means the register was stable.
  **
  ** If not, then we don't know what either of them contains. But we do know
  ** that sample_3 is one greater than our number. (Note that if it's is 0,
  ** then we have to wrap around.) That's because the register was in the
  ** process of incrementing during the interval when 1 or 2 were sampled.
  ** It is therefore certain that it was stable when 3 was sampled, but we
  ** want the pre-increment value because that's what the time was when this
  ** function was invoked.
  */
  if (sample_1==sample_2) {
    counter = sample_1;
  } else if (sample_3!=0) {
    counter = sample_3-1;
  } else {
    retval->time = CLK_MS_PER_TICK - 1;
    retval->reg_value = sample_3;
    return;
  }

  retval->reg_value = counter;

  /* Convert the sample to 16 bits and then divide by 19.22 to get
  ** milliseconds (x / 19.22 == (x * 100) / 1922).  Partial milliseconds
  ** are rounded.
  */
  milliseconds = (word) counter;
  milliseconds = (milliseconds * 100 + (1922/2) ) / 1922;

  retval->time = milliseconds;

  return;

}

