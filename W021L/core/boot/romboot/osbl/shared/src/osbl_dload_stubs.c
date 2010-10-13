/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               O E M  S e c o n d a r y  B o o t  L o a d e r
                            D L O A D   S T U B S

GENERAL DESCRIPTION
  This module provides function stubs for dload integration. 

EXTERNALIZED FUNCTIONS
  err_put_log
  err_fatal_core_dump
  msg_send_3
  tramp_set_isr
  timer_group_enable
  timer_group_disable
  gpio_int_set_handler
  gpio_int_set_detect
  
  
  
Copyright (c) 2005 - 2006 by QUALCOMM Incorporated. All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_dload_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/07   MJS     Added rex_ints_are_locked for smem dynamic allocation.
12/21/07    ly     Fix compiler warnings
04/10/06   JHS     Stubbed out gpio_int_set_handler and gpio_int_set_detect
05/25/05   AMW     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 

#include "comdef.h"
#include "msg.h"
#include "mobile.h"
#include "tramp.h"
#include "timer.h"
#include "gpio_int.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Original definition can be found in rextime.c */
timer_group_type  rex_timer_group;

/* Mobile Firmware Marketing Software Revision also in mobile.c */
NOTICE_TYPE mob_sw_rev[] = BUILD_ASIC "-" BUILD_TARGET "-" BUILD_VER;


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


