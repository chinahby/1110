/*===========================================================================

                     F A I L  S A F E  B o o t  L o a d e r
                        D o w n l o a d e r  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the FSBL downloader module, and declarations for the public functions
  in the module.

EXTERNALIZED FUNCTIONS
  err_put_log
  err_fatal_core_dump
  msg_send_3
  tramp_set_isr
  timer_group_enable
  timer_group_disable
  gpio_int_set_handler
  gpio_int_set_detect

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/fsbl/fsbl_dload_stubs.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/22/09   dk      CR183127 FSBL watchdog kick fix which helps to reset the phone
07/30/07   ANB     Initial version.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "msg.h"
#include "mobile.h"
#include "bio.h"
#include "fsbl.h"
#include "tramp.h"
#include "timer.h"
//#include "gpio_int_msm.h"
#include "gpio_int.h"
#include "custfsbl.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Original definition can be found in rextime.c */
timer_group_type  rex_timer_group;

/* Mobile Firmware Marketing Software Revision also in mobile.c */
NOTICE_TYPE mob_sw_rev[] = BUILD_ASIC "-" BUILD_TARGET "-" BUILD_VER;

/* For dload to use; we cheat the dload routine for this security code */
/* We have to go to download mode if anything wrong in OEMSBL          */
uint8 const chk_security_code_ptr[] = {0xFF, 0xFF, 0xFF, 0xFF, \
                                      0xFF, 0xFF, 0xFF, 0xFF};

/* Kick Watchdog */
#define BOOTHW_KICK_WATCHDOG()       HWIO_OUT(WDOG_RESET, HWIO_FMSK(WDOG_RESET,WATCH_DOG));

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION ERR_PUT_LOG

DESCRIPTION
  Function stub, full function can be found in err.c.

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/

void err_put_log 
(
  word   line,                /* from __LINE__ */
  const char   *file_ptr      /* From __FILE__ */
)
{}

/*===========================================================================

FUNCTION ERR_FATAL_CORE_DUMP

DESCRIPTION
  Function stub, full function can be found in err.c.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE
  
SIDE EFFECTS
  NONE

===========================================================================*/

void err_fatal_core_dump 
(
  unsigned int line,      /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format   /* format string */
) 
{}

/*===========================================================================

FUNCTION REX_JETTISON_CORE

DESCRIPTION
  Function stub, full function can be found in rex_core.s.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE
  
SIDE EFFECTS
  NONE

===========================================================================*/
void rex_jettison_core ()
{}


/*===========================================================================

FUNCTION MSG_SEND_3

DESCRIPTION
  Function stub, full function can be found in msg.c.

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/

void msg_send_3 
(
  const msg_const_type * xx_msg_const_ptr, 
  uint32 xx_arg1,
  uint32 xx_arg2,
  uint32 xx_arg3
)
{}

/*===========================================================================

  FUNCTION     TRAMP_SET_ISR

  DESCRIPTION
    Function stub, full function can be found in tramp_pic.c.

  DEPENDENCIES
    NONE

  RETURN VALUE
    NONE

  SIDE EFFECTS
    NONE
===========================================================================*/
void tramp_set_isr
(
  /* Select which ISR is being set */
  tramp_isr_type       which_isr,     
  /* ISR to be installed for this interrupt */
  tramp_isr_ptr_type   isr_ptr
)
{}

/*===========================================================================

  FUNCTION     REX_INTS_ARE_LOCKED

  DESCRIPTION
    Function stub, full function can be found in intlock_ads.s.

  DEPENDENCIES
    NONE

  RETURN VALUE
    NONE

  SIDE EFFECTS
    NONE
===========================================================================*/
int rex_ints_are_locked( void )
{
  /* Return TRUE to indicate that interrupts are disabled/locked */
  return TRUE;
}

/*=============================================================================

  FUNCTION TIMER_GROUP_ENABLE

  DESCRIPTION
    Function stub, full function can be found in timer.c.

  DEPENDENCIES
    NONE

  RETURN VALUE
    NONE

  SIDE EFFECTS
    NONE

=============================================================================*/

void timer_group_enable
(
  /* Timer group to be enabled */
  timer_group_ptr                 timer_group
)
{}

/*=============================================================================

  FUNCTION TIMER_GROUP_DISABLE

  DESCRIPTION
    Function stub, full function can be found in timer.c.
  
  DEPENDENCIES
    NONE

  RETURN VALUE
    NONE

  SIDE EFFECTS
    NONE

=============================================================================*/

void timer_group_disable
(
  /* Timer group to be disabled */
  timer_group_ptr                 timer_group
)
{}

/*===========================================================================

FUNCTION GPIO_INT_SET_DETECT

DESCRIPTION
  Specify whether this GPIO interrupt is LEVEL or EDGE detected.
  
  Note: This function can be applied to NON group GPIO interrupts, 
        ie. GPIOs 0..4 as well as GPIOs 5..47.

DEPENDENCIES
  gpio_int_init() must have been previously called.

RETURN VALUE
  TRUE = Success, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean gpio_int_set_detect
(
  gpio_int_type gpio_int,
  gpio_int_detect_type gpio_int_detect
)
{
    return FALSE;
}

/*===========================================================================

FUNCTION GPIO_INT_SET_HANDLER

DESCRIPTION
  Specify the address of the handler funtion for a given GROUP
  interrupt source.  Note that this function also sets up handlers for
  GPIO2_GROUP interrupts.  
  
  If the address is NULL, the interrupt is disabled, by masking it out
  in the CDMA ASIC GROUP interrupt controller.

DEPENDENCIES
  gpio_int_init() must have been previously called.
  gpio_int_set_detect should be called first, in order to set the detection
  type (LEVEL or EDGE).

RETURN VALUE
   boolean - FALSE => set action failed
             TRUE  => set action passed

SIDE EFFECTS
  None

===========================================================================*/

boolean gpio_int_set_handler
( 
  gpio_int_type gpio_int, 
  gpio_int_polarity_type gpio_int_polarity, 
  gpio_int_handler_type gpio_int_handler
)
{
    return FALSE;
}

/*===========================================================================

FUNCTION CLK_PAUSE

DESCRIPTION
  This function is just a dummy function used to compile the
  oemsbl without any errors.
  
  AMSS and OEMSBL has different clk_busy_wait implementations.
  May be removed when they are reconciled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void clk_pause(uint32 us)
{
  clk_busy_wait(us);
}

/*===========================================================================

FUNCTION MSG_SAVE_3

DESCRIPTION
  Function stub, full function can be found in msg.c.

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/

void msg_save_3 
(
  const msg_const_type * xx_msg_const_ptr, 
  uint32 xx_arg1,
  uint32 xx_arg2,
  uint32 xx_arg3,
  msg_ext_store_type *store_type
)
{
}

/*===========================================================================

FUNCTION BOOT_HW_RESET

DESCRIPTION
  Function stub, for use in dload code

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/

void boot_hw_reset(void)
{
  BOOTHW_KICK_WATCHDOG();
  while (1)
  {
    /* Wait for reset */
  }
}

