#ifndef SLEEPCFG_H
#define SLEEPCFG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

         S L E E P   C O N T R O L L E R   C O N F I G U R A T I O N

GENERAL DESCRIPTION

  This file contains macros for generalized sleep controller access

EXTERNALIZED FUNCTIONS

  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


Copyright (c) 2005, 2006, 2007, 2008 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=


==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/sleepctl/main/latest/src/sleepcfg.h#4 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   -----------------------------------------------------------
12/02/2008   cab   Added LCU target
03/10/2008   cab   Added QSC1100 target
03/27/2008   hluo  Added LCU as applicable targets
01/08/2008   hluo  Added Koel and Hawk to applicable targets
11/15/2007   cab   Turn on sleepctl clk regime mgmt for 7500 targets
10/29/2007   cab   Turn on sleepctl clk regime mgmt for 7600 targets
02/21/2007   cab   Add support for T_QSC60X5.
11/22/2006   trc   Add support for T_MSM7600.
12/22/2005   ejv   Add support for T_QSC60X0.
07/13/2005   ajn   Added REG_ACCESS_CPU_AT_TCXO run-time T_MSM6800B check
06/27/2005   ejv   Added support for T_MSM6800B.
05/19/2005   ajn   6550 = 6500 + edge sensitive interrupts
02/10/2005   ajn   Created this file

============================================================================*/


/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"
#include "comdef.h"



/*============================================================================

                             TARGET CONFIGURATION

This section determines the sleep controller feature set for the various MSMs.

  - SUPPORTED
      A bitmask of the supported sleep controllers.

  - SINGLE_INTERRUPT
      All sleep controller events for a given technology are routed to a
      single TRAMP interrupt vector.  The controller's h/w status must be read
      to determine the type of event that occurred.

   - WAKEUP_LEVEL_TRIGGERED
      The wakeup interrupt is level triggered, and must be deasserted before
      the interrupt pending flag can be successfully cleared.

  - REG_ACCESS_CPU_AT_TCXO
      CPU must be slowed down to TCXO speed before accessing sleep controller
      registers.

   - SLEEPCTL_CLKRGM_CONTROL
      Turns off sleep controller clock regimes when not required.

============================================================================*/

/* Bit mask for the various sleep controller support */

#define S_1X                      0x1
#define S_HDR                     0x2
#define S_GSM                     0x4
#define S_WCDMA                   0x8


#if defined(T_MSM7600)

  #define SUPPORTED               ( S_1X | S_HDR | S_GSM | S_WCDMA )

  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #define SLEEPCTL_CLKRGM_CONTROL

#elif defined (T_QSC6270)

  #define SUPPORTED               ( S_GSM | S_WCDMA )

  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #define SLEEPCTL_CLKRGM_CONTROL

#elif defined(T_MSM7200) || defined(T_MSM6246) || defined(T_MSM6290) 

  #define SUPPORTED               ( S_GSM | S_WCDMA )

  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL

#elif defined(T_QSC1100)

  #define SUPPORTED               ( S_1X )

  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL

#elif defined(T_QSC60X0)

  #define SUPPORTED               ( S_1X )

  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL

#elif defined(T_QSC60X5)

  #define SUPPORTED               ( S_1X | S_HDR )

  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL

#elif defined(T_MSM7500)

  #define SUPPORTED               ( S_1X | S_HDR | S_GSM )

  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #define SLEEPCTL_CLKRGM_CONTROL
  
#elif defined(T_MSM6800B)

  #define SUPPORTED               ( S_1X | S_HDR | S_GSM )
  
  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL
  
#elif defined(T_MSM6700)

  #define SUPPORTED               ( S_1X | S_HDR | S_GSM )
  
  #define SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #define REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL
  
#elif defined(T_MSM6550)

  #define SUPPORTED               ( S_1X | S_HDR )

  #undef  SINGLE_INTERRUPT
  #undef  WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL
    
#elif defined(T_MSM6500)

  #define SUPPORTED               ( S_1X | S_HDR )

  #undef  SINGLE_INTERRUPT
  #define WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL
  
#else

  #define SUPPORTED               ( S_1X )

  #undef  SINGLE_INTERRUPT
  #define WAKEUP_LEVEL_TRIGGERED
  #undef  REG_ACCESS_CPU_AT_TCXO
  #undef  SLEEPCTL_CLKRGM_CONTROL
  
#endif /* T_MSMxxxx */




/*============================================================================

                            REG_ACCESS_CPU_AT_TCXO

This section contains macros to slow the CPU down, and restore it to its
original speed.  INTLOCK'ing is necessary around reduced code speed sections,
to prevent an interrupt from executing (possibly causing task switches) while
running at the reduced speed.

BEGIN_REG_ACCESS - will reduce the CPU speed, if necessary.
END_REG_ACCESS   - restores the CPU speed.

The above two macros contain INTLOCK/INTFREE macros if they reduce the CPU
speed, to prevent an ISR (or other tasks if the ISR marks a higher priority
task a runnable) from running at the lower clock speed.  If already inside
an INTLOCK context, the following two macros should be used instead:

BEGIN_REG_ACCESS_IN_INTLOCK - will reduce the CPU speed, if necessary.
END_REG_ACCESS_IN_INTLOCK - restores the CPU speed.

============================================================================*/

#ifdef REG_ACCESS_CPU_AT_TCXO

/*----------------------------------------------------------------------------
  Additional include files, and function prototypes for CPU speed control.
----------------------------------------------------------------------------*/

#include "clkregim.h"


/*----------------------------------------------------------------------------
  Macros to slow down and restore CPU speed, when already inside INTLOCK
----------------------------------------------------------------------------*/

 #if defined(T_QSC60X0)

 #error REG_ACCESS_CPU_AT_TCXO not supported

 #elif defined(T_MSM6800)

  /* Flag for determining if sleep register access needs to be done at TCXO */
  extern boolean sleepcfg_reg_access_cpu_at_tcxo;

  /* Reduce CPU speed for SLEEP CONTROLLER register access */
  #define BEGIN_REG_ACCESS_IN_INTLOCK() \
    if ( sleepcfg_reg_access_cpu_at_tcxo ) { \
          clk_regime_switch_mclk( CLKRGM_CPU_TCXO_SPEED ); }

  /* Restore CPU speed after SLEEP CONTROLLER registers access */
  #define END_REG_ACCESS_IN_INTLOCK() \
    if ( sleepcfg_reg_access_cpu_at_tcxo ) { \
          clk_regime_switch_mclk( CLKRGM_CPU_FULL_SPEED ); }

 #elif defined(T_MSM6700)

  /* Reduce CPU speed for SLEEP CONTROLLER register access */
  #define BEGIN_REG_ACCESS_IN_INTLOCK() \
          clk_regime_switch_mclk( CLKRGM_CPU_TCXO_SPEED )

  /* Restore CPU speed after SLEEP CONTROLLER registers access */
  #define END_REG_ACCESS_IN_INTLOCK() \
          clk_regime_switch_mclk( CLKRGM_CPU_FULL_SPEED )

 #endif


/*----------------------------------------------------------------------------
  Macros to slow down and restore CPU speed, when not inside INTLOCK
----------------------------------------------------------------------------*/

 #define BEGIN_REG_ACCESS()     INTLOCK(); BEGIN_REG_ACCESS_IN_INTLOCK()
 #define END_REG_ACCESS()       END_REG_ACCESS_IN_INTLOCK(); INTFREE()

#else /* !REG_ACCESS_CPU_AT_TCXO */

/*----------------------------------------------------------------------------
  No speed change is required - all macro's are no-op's.
----------------------------------------------------------------------------*/

 #define BEGIN_REG_ACCESS_IN_INTLOCK()
 #define END_REG_ACCESS_IN_INTLOCK()
 #define BEGIN_REG_ACCESS()
 #define END_REG_ACCESS()

#endif /* REG_ACCESS_CPU_AT_TCXO */




/*==========================================================================*/

#endif /* !SLEEPCFG_H */

