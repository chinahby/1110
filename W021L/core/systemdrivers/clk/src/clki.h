#ifndef CLKI_H
#define CLKI_H
/*===========================================================================

   C L O C K   S E R V I C E S   I N T E R N A L   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the internal definitions used by the
  DMSS clock services.

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msm6800/drivers/clk/clki.h#2 $ $DateTime: 2006/07/19 18:49:04 $ $Author: rajeevp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/03   ajn     Functionality moved to the Time Service Subsystem.
02/20/01   dsb     Removed the inclusion of msg.h, since no part of this
                   file actually requires msg.h.
10/06/00   dsb     Removed TG== code.
03/01/98   scdb    clk_uptime_val becomes a regional local
                   clk_tick_isr required a regional local declaration
10/30/97   scdb    Completely revamped the way that system time is maintained.
                   Added regional functions clk_uptime_add, clk_tickphase,
                   and macro CLK_UPTIME_DIFF
09/18/96   rdh     Added alternate clk_up_time maintenance technique.
11/09/94   gb      Changed TIMER_0 INT priority to 3.
03/29/94   jah     Initial revision

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "target.h"     /* Target specific definitions          */
#include "clk.h"        /* General clock definitions            */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
**                           Base Date
**
** Time values used by the Clock Services are offsets from a base date.
** All times are based on Midnight, January 6, 1980. This is sometimes known
** as the "epoch"; it's the specified time(0) for GPS, and also the specified
** time(0) for IS95.
**---------------------------------------------------------------------------
*/

/* This is the year upon which all time values used by the Clock Services
** are based.  NOTE:  The user base day (GPS) is Jan 6 1980, but the internal
** base date is Jan 1 1980 to simplify calculations
*/
#define BASE_YEAR       1980


/*
** Some calculations are made easier by basing a date on January 1 instead
** of January 6. These constants make that conversion easy.
*/

#define CLK_OFFSET_MS   432000000L
   /* 5 days (duration between Jan 1 and Jan 6), expressed as milliseconds. */

#define CLK_OFFSET_S    432000L
   /* 5 days (duration between Jan 1 and Jan 6), expressed as seconds. */

/*---------------------------------------------------------------------------
**                     80186 Timer #0 Control
**
** The clock is setup to run continuously generating a priority 2 interrupt
** each time the max count is reached.  The count is counted up 1 for each
** pulse of the baud clock input.
**---------------------------------------------------------------------------
*/
#define CLK_TIM0_CTL \
  ( P_TIM_EN_V | P_TIM_INHBAR_V | P_TIM_INT_V | P_TIM_EXT_V | P_TIM_CONT_V )

#define CLK_INT_TIM_CTL (P_INT_PRI3_V)  /* Priority of timer interrupt */

/*---------------------------------------------------------------------------
**     Maximum time interval for a call-back update, in milliseconds
**
** When there is a large change in system time, the call-back's are called
** with the delta.  To avoid having to use qwords for the call-backs, the
** maximum update is limited to a convenient value which will fit in a
** 32 bit data type.
**---------------------------------------------------------------------------
*/
#define CLK_CB_MAX_MS_UPDATE ((int4)(1000L * 60L * 60L * 24L)) /* 1 day */


/*---------------------------------------------------------------------
**                Regional Message Threshold
**
** This is the regional message threshold for this module.
**---------------------------------------------------------------------
*/


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


/*
** The following structure is used for communication with the
** function clk_tickphase.
** The calling function creates one of these and passes a reference.
** clk_tickphase will fill it in: 'time' contains the number of
** milliseconds since the last 5-ms interrupt, 'reg_value' contains the
** raw value read from the hardware from which 'time' was calculated.
*/
typedef struct {
    word time;
    int2 reg_value;
} clk_tickphase_type;



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


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
void clk_tick_isr(void);



/*===========================================================================

  MACRO CLK_UPTIME_DIFF

  DESCRIPTION
    Calculate and return a (int2) difference in milliseconds between two
    clk_uptime_types (specifically, returns left-right)

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
#define CLK_UPTIME_DIFF(left, right)  \
   ( ((int2)(((int4)(left).secs)-((int4)(right).secs)))*CLK_MS_PER_SEC \
            + (int2)(left).ms - (int2)(right).ms)



/*===========================================================================

FUNCTION CLK_TICKPHASE

DESCRIPTION
  This function reads a hardware register and calculates from that the
  number of milliseconds since the last time a 5-ms tick took place.

DEPENDENCIES
  The calling code is responsible for masking interrupts before calling
  this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void clk_tickphase(
  clk_tickphase_type *retval  /* Structure through which to return the phase */
);

#endif /* CLKI_H */
