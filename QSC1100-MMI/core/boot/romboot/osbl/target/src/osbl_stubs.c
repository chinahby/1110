/*=============================================================================

                    Operating System  B o o t   L o a d e r
                                S t u b s

GENERAL DESCRIPTION
  This header file contains the implementation for the OSBL NAND flash
  device module.

EXTERNALIZED FUNCTIONS
  rex_self
  tramp_queue_call
  tramp_set_isr
  tramp_call_def
  qw_set
  qw_mul
  qw_inc
  qw_cmp
  qw_sub
  qw_equ
  qw_div
  qw_lo
  qw_add
  clk_tickphase
  amd_probe
  spansion_probe
  turn_off_micro_clk
  boot_error_entry
  boot_main_ctl
  lcd_message
  mddihost_diag_init
  log_alloc
  log_commit
  log_status
  msg_send_3
  msg_save_3
  err_fatal_core_dump
  gpio_int_set_detect
  gpio_int_set_handler
  clk_pause
  bbrx_init
  bbrx_wakeup
  rex_int_lock
  rex_int_free
  hw_set_ps_hold
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/osbl/osbl_stubs.c#5 $

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/osbl/osbl_stubs.c#5 $

when         who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/07   ly      Create
=============================================================================*/

#include "boot_comdef.h"
#include "rex.h"
#include "msg.h"
#include "gpio_int.h"
#include "custosbl.h"

#ifdef FEATURE_OSBL_MULTI_TASK
#include "log.h"
#endif

#ifndef FEATURE_OSBL_MULTI_TASK
typedef struct {
  uint32 in_use;                    /* True if in-use              */
} tramp_call_ptr_type;

typedef struct {
    word time;
    int2 reg_value;
} clk_tickphase_type;

typedef struct {
    word time;
    int2 reg_value;
} fsi_device;

typedef void (*isr_ptr_type)( void );

byte svc_stack;

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
/* For dload to use; we cheat the dload routine for this security code */
/* We have to go to download mode if anything wrong in OSBL              */
uint8 const chk_security_code_ptr[] = {0xFF, 0xFF, 0xFF, 0xFF, \
                                      0xFF, 0xFF, 0xFF, 0xFF};

/*===========================================================================
FUNCTION REX_SELF

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors. We don't have rex available when
  in osbl. The flash drivers and clk regime code that
  gets compiled in with osbl uses rex. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  0

SIDE EFFECT
  none.
=============================================================================
*/
rex_tcb_type *rex_self( void )
{
	return NULL;
} /* END rex_self */


/*=============================================================================
FUNCTION TRAMP_QUEUE_CALL

  This function is just a dummy function used to compile the
  osbl without any errors.
  
  The flash drivers and clk regime code that gets compiled in with osbl
  uses tramp. Hence, this stub is used to avoid compile errors.

DESCRIPTION

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void tramp_queue_call
(
  tramp_call_ptr_type *mytranp	/* Call-pointer to queue */
)
{
  return;
}
#if 0
/*=============================================================================
FUNCTION TRAMP_SET_ISR

  This function is just a dummy function used to compile the
  osbl without any errors.
  
  The flash drivers and clk regime code that gets compiled in with osbl
  uses tramp. Hence, this stub is used to avoid compile errors.

DESCRIPTION

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void tramp_set_isr
(
  tramp_irq_type           irq,
  tramp_void_handler_type  isr
)
{
  return;
}
#endif 

/*=============================================================================
FUNCTION TRAMP_CALL_DEF

DESCRIPTION

  This function is just a dummy function used to compile the
  osbl without any errors.
  
  The flash drivers and clk regime code that gets compiled in with osbl
  uses tramp. Hence, this stub is used to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void tramp_call_def
(
  tramp_call_ptr_type *call_ptr /* Call-pointer to initialize */
)
{
  return;
}


/*=============================================================================
FUNCTION QW_SET

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that
  gets compiled in makes a call to qw_set. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void qw_set
(
  qc_qword qw,
    /* the qword to set                */
  uint32 hi,
    /* the value for the high 32 bits */
  uint32 lo
    /* the value for the low 32 bits  */
)
{
  return;
}



/*=============================================================================
FUNCTION QW_MUL

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that
  gets compiled in makes a call to qw_mul. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void qw_mul
(
  qc_qword product,
    /* the result of the multiplication       */
  qc_qword multiplicand,
    /* the first argument for multiplication  */
  uint32 multiplier
    /* the second argument for multiplication */
)
{
  return;
}

/*=============================================================================
FUNCTION QW_INC

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that
  gets compiled in makes a call to qw_inc. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void qw_inc
(
  qc_qword qw1,
    /* the qword to increment */
  dword val
    /* value to add to qw1    */
)
{
  return;
}


/*=============================================================================
FUNCTION QW_CMP

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that
  gets compiled in makes a call to qw_cmp. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  0

SIDE EFFECT
  none.
=============================================================================*/
int qw_cmp
(
  qc_qword  qw1,
    /* the first qword to compare  */
  qc_qword  qw2
    /* the second qword to compare */
)
{
  return 0;
}



/*=============================================================================
FUNCTION QW_SUB

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors. We don't have rex available when
  in osbl. The flash drivers and clk regime code that
  gets compiled in with osbl uses rex. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  0

SIDE EFFECT
  none.
=============================================================================*/
void qw_sub
(
  qc_qword  difference,
    /* the result of the subtraction       */
  qc_qword  subtrahend,
    /* the first argument for subtraction  */
  qc_qword  subtractor
    /* the second argument for subtraction */
)
{

  return;
}


/*=============================================================================
FUNCTION QW_EQU

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that gets compiled in makes a 
  call to qw_set. Hence, this stub is used to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  0

SIDE EFFECT
  none.
=============================================================================*/
void qw_equ
(
  qc_qword  qw1,
    /* qw1 will be set equal to qw2       */
  qc_qword  qw2
    /* the qword qw1 will be set equal to */
)
{

  return;
}



/*=============================================================================
FUNCTION QW_DIV

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that
  gets compiled in makes a call to qw_div. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  0

SIDE EFFECT
  none.
=============================================================================*/
word qw_div
(
  qc_qword           quotient,
    /* the result from the division     */
  qc_qword           dividend,
    /* the first argument for division  */
  unsigned short  divisor
    /* the second argument for division */
)
{
  return 0;
}


/*=============================================================================
FUNCTION QW_LO

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that
  gets compiled in makes a call to qw_lo. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
uint32 qw_lo
(
  qc_qword qw
    /* qword to return the low 32 bits from */
)
{
  return 0;
}


/*=============================================================================
FUNCTION QW_ADD

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that
  gets compiled in makes a call to qw_add. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void qw_add
(
  qc_qword  sum,
    /* the result of the addition   */
  qc_qword  addend,
    /* the first addition argument  */
  qc_qword  adder
    /* the second addition argument */
)
{
  return;
}


/*=============================================================================
FUNCTION CLK_TICKPHASE

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers and clk regime code that gets compiled in makes a call
  to clk_tickphase. Hence, this stub is used to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void clk_tickphase(clk_tickphase_type *retval)
{
  return;
}


/*=============================================================================
FUNCTION AMD_PROBE

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers code that gets compiled in contains a call to this
  function. Hence, this stub is used to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  NULL

SIDE EFFECT
  none.
=============================================================================*/
fsi_device *
amd_probe (volatile word *baseaddr)
{
  return NULL;
}

/*=============================================================================
FUNCTION SPANSION_PROBE

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The flash drivers code that gets compiled in contains a call to this
  function. Hence, this stub is used to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  NULL

SIDE EFFECT
  none.
=============================================================================*/
fsi_device *
spansion_probe (volatile word *baseaddr)
{
  return NULL;
}


/*=============================================================================
FUNCTION TURN_OFF_MICRO_CLK

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  0

SIDE EFFECT
  none.
=============================================================================*/
uint32 turn_off_micro_clk ()
{
  return 0;
}



/*=============================================================================
FUNCTION BOOT_ERROR_ENTRY

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The drivers boot code that gets compiled in makes a call to boot_main_ctl.
  Hence, this stub is used to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void boot_error_entry ()
{
  return;
}


/*=============================================================================
FUNCTION BOOT_MAIN_CTL

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  The drivers boot code that gets compiled in makes a call to boot_main_ctl.
  Hence, this stub is used to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void boot_main_ctl ()
{
  return;
}


/*=============================================================================
FUNCTION LCD_MESSAGE

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  
  The flash drivers and clk regime code that
  gets compiled in makes a call to lcd_message. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  none.
  
RETURN VALUE
  none.

SIDE EFFECT
  none.
=============================================================================*/
void lcd_message ()
{
  return;
}


#endif

#ifdef FEATURE_OSBL_MULTI_TASK
uint32 mdp_total_vdopkts;
uint32 mdp_in_processing;

/*===========================================================================

FUNCTION MDDIHOST_DIAG_INIT

DESCRIPTION
  This function is just a dummy function used to compile the osbl without
  any errors.
  
  drivers/mddi/mddihostdiag.c
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void mddihost_diag_init ( void )
{
}

/*===========================================================================

FUNCTION LOG_ALLOC

DESCRIPTION
  This function is just a dummy function used to compile the osbl without
  any errors.
  
  services/diag/log.c
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
PACKED void * log_alloc ( log_code_type code, unsigned int length )
{
  return NULL;
}

/*===========================================================================

FUNCTION LOG_COMMIT

DESCRIPTION
  This function is just a dummy function used to compile the osbl without
  any errors.
  
  services/diag/log.c
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void log_commit (PACKED void *ptr)
{
}

/*===========================================================================

FUNCTION LOG_STATUS

DESCRIPTION
  This function is just a dummy function used to compile the osbl without
  any errors.
  
  services/diag/log.c
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
boolean log_status (log_code_type code)
{
  return FALSE;
}


#endif


/*===========================================================================

FUNCTION MSG_SEND_3

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  
  The SBI library makes calls to MSG macros. Hence, this stub is used
  to avoid compile errors.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void msg_send_3 (const msg_const_type * const_blk, uint32 xx_arg1,
            uint32 xx_arg2, uint32 xx_arg3)
{
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

#ifndef FEATURE_OSBL_MULTI_TASK
/*===========================================================================

FUNCTION GPIO_INT_SET_DETECT

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  
  drivers/tramp/gpio_int_6250.c 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean gpio_int_set_detect(
  gpio_int_type         which_group_int,
      /* Which GPIO_INT to set */
  gpio_int_detect_type  detect
      /* Detect interrupts by LEVEL or EDGE */
)
{
  return 0;
}

/*===========================================================================

FUNCTION GPIO_INT_SET_HANDLER

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  
  drivers/tramp/gpio_int_6250.c

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean gpio_int_set_handler
(
  gpio_int_type          which_group_int,
      /* Select which GPIO_INT is being handled */
  gpio_int_polarity_type polarity,
      /* Trigger on low/high signal */
  gpio_int_handler_type  handler
      /* ISR to be executed */
)
{
  return 0;
}
#endif 

/*===========================================================================

FUNCTION CLK_PAUSE

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  
  AMSS and OSBL have different clk_busy_wait implementations.
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

FUNCTION BBRX_INIT

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void bbrx_init(uint32 rf_mode )
{
}


/*===========================================================================

FUNCTION BBRX_WAKEUP

DESCRIPTION
  This function is just a dummy function used to compile the
  osbl without any errors.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void bbrx_wakeup (uint32 path, uint32 wakeup)
{
}
/*===========================================================================

FUNCTION  REX_INT_LOCK

DESCRIPTION
  Stub function

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dword rex_int_lock(void)
{
  return 0;
}

/*===========================================================================

FUNCTION  REX_INT_FREE

DESCRIPTION
  Stub function

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dword rex_int_free(void)
{
  return 0;
}
/*===========================================================================

FUNCTION HW_SET_PS_HOLD

DESCRIPTION
  This function sets the state of the PS_HOLD line to the PMIC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hw_set_ps_hold( boolean bState )
{
}


#if defined(FEATURE_OSBL_CHG_SCREEN)
/*===========================================================================

FUNCTION REX_ENTER_CRIT_SECT

DESCRIPTION
  Function stub, for use in dload code

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/

void rex_enter_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
}

/*===========================================================================

FUNCTION REX_LEAVE_CRIT_SECT

DESCRIPTION
  Function stub, for use in dload code

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE

===========================================================================*/

void rex_leave_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
}

void rex_init_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
}

boolean mdp_disp_fatal_err_flag;

#endif