/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S L E E P   M O D E M    K E Y P R E S S

GENERAL DESCRIPTION
  This file containes the modem/single processor specific functions 
  used to support keypress handling in sleep.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

                           Copyright (c) 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/power/sleep/rel/1h08/src/sleepmod_keypress.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/08   clm     1H08 sleep re-work.  Initial version.

===========================================================================*/

/*=========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "rex.h"
#include "tramp.h"
#include "clk.h"
#include "sleepmod_keypressi.h"

#ifdef FEATURE_RESISTIVE_KEYPAD
  #include "gpio_int.h"
#endif /* FEATURE_RESISTIVE_KEYPAD */

/*===========================================================================

                          PREPROCESSOR DEFINITIONS AND CONSTANTS

===========================================================================*/

/* SLEEPMOD_CLR_KEYPD_SIG(x) */
#ifdef FEATURE_KEYPAD_OKTS
  #define SLEEPMOD_CLR_KEYPD_SIG(x)  rex_clr_sigs( &(x), SLEEP_KYPD_OKTS_SIG )
#else /* !FEATURE_KEYPAD_OKTS */
  #define SLEEPMOD_CLR_KEYPD_SIG(x)  rex_clr_sigs( &(x), SLEEP_HS_OKTS_SIG )
#endif /* !FEATURE_KEYPAD_OKTS */

/* SLEEPMOD_CLK_UNEXPECTED_ENABLE() */
#ifdef FEATURE_REX_PREEMPT_INTS
/* Re-enable the 5msec tick */
  #define SLEEPMOD_CLK_UNEXPECTED_ENABLE()  clk_unexpected_enable()
#else /* !FEATURE_REX_PREEMPT_INTS */
  #define SLEEPMOD_CLK_UNEXPECTED_ENABLE() 
#endif /* !FEATURE_REX_PREEMPT_INTS */

/* SLEEPMOD_INSTALL_KYPD_ISR(), SLEEPMOD_UNINSTALL_KYPD_ISR() */
#ifdef FEATURE_RESISTIVE_KEYPAD
  #define SLEEPMOD_INSTALL_KYPD_ISR() \
            gpio_int_set_handler( GPIO_INT_34, ACTIVE_LOW, sleep_keypress_isr )
  #define SLEEPMOD_UNINSTALL_KYPD_ISR() \
                          gpio_int_set_handler( GPIO_INT_34, ACTIVE_LOW, NULL )
#else /* !FEATURE_RESISTIVE_KEYPAD */
  #define SLEEPMOD_INSTALL_KYPD_ISR()
  #define SLEEPMOD_UNINSTALL_KYPD_ISR() 
#endif /* !FEATURE_RESISTIVE_KEYPAD */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains definitions for constants, macros, types, variables
  and other items needed by this module.

===========================================================================*/

#ifndef FEATURE_KEYPAD_OWNS_KEYPRESS_ISR
#error code not present
#endif /* !FEATURE_KEYPAD_OWNS_KEYPRESS_ISR */

/*===========================================================================

FUNCTION SLEEP_INSTALL_KEYPRESS_ISR

DESCRIPTION
  This installs the keypress ISR for targets that have keyboards

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void sleep_install_keypress_isr( void )
{
#if !defined(T_CH) && !defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR)
  /* Allow keypresses to wake up the CPU, and turn the 5ms tick back
     on, in order to debounce keypresses. */
  tramp_set_isr( TRAMP_KEY_SENSE_ISR, sleep_keypress_isr );

  /* Install the isr for resistive keypad sense through gpio34 */
  SLEEPMOD_INSTALL_KYPD_ISR();

#endif /* !defined(T_CH) && !defined(FEATURE_KEYPAD_OWNS_KEYPRESS_ISR) */
} /* sleep_install_keypress_isr */

