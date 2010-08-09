/*===========================================================================

                    D E V I C E    B O O T    L O A D E R 
                         E r r o r  H a n d l e r

DESCRIPTION
  This header file contains declarations and definitions for the DBL
  error handler.  It is called in the event a DBL failure is detected.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_error_handler.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/07   ANB     Initial version.
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "dbl.h"
#include "dbl_error_handler.h" 
#include "dbl_mc.h"
#include "dbl_fsbl_handler.h"
#include "boot_error_if.h"
#include "boot_pbl_accessor.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* DBL error log data structures */
dbl_err_log_entry_type dbl_err_log[2];
dbl_err_count_type dbl_err_count;

/* Function prototype */
static void dbl_install_error_callback
(
  bl_error_callback_func_type,
  void *,
  bl_error_callback_node_type *node_ptr
);

/* The global boot error interface structure */
boot_error_if_type bl_err_if = 
{
  dbl_error_handler,
  dbl_install_error_callback
};

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_ERROR_HANDLER

DESCRIPTION
  This function is the error handler for the DBL.It records the line number,
  filename and sbl_error_type. Then jumps into a for loop forever until JTAG
  is connected. This function is also shared by exception handler.

DEPENDENCIES
  Exception handler. 
  
RETURN VALUE
  None

SIDE EFFECTS
  This function never returns.

===========================================================================*/
/*lint -save -e550 code not reference */
void dbl_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      dbl_error         /* Enum that describes error type */
)
{
  static uint32 error_handler_entry_count  = 0;
  register uint32 *fname_ptr = (uint32 *)filename_ptr;
  register uint32 error_type = dbl_error;
  register uint32 i = 0;

  error_handler_entry_count++; /* Count the number of calls to this fn */
 
  /* Kick the dog while we proceed */
  DBL_KICK_WATCHDOG(); 

  /* Save total number of error logged in IMEM */
  dbl_err_count.error_count = error_handler_entry_count;

  /* Save sbl error type in IMEM */
  dbl_err_log[error_handler_entry_count - 1].error_type = error_type;

  /* Save the line number in IMEM */
  dbl_err_log[error_handler_entry_count - 1].line = line;

  i= 0;

  /* Save filename in IMEM if pointer not NULL */
  if ( fname_ptr != NULL )
  {
    /* Save filename to IMEM */
    while (  ( (*fname_ptr & 0xff000000) != 0x0 ) &&
             ( (*fname_ptr & 0x00ff0000) != 0x0 ) &&
             ( (*fname_ptr & 0x0000ff00) != 0x0 ) &&
             ( (*fname_ptr & 0x000000ff) != 0x0 ) &&
               (i < (MAX_FILENAME_SIZE - 1))
             )
    {
      dbl_err_log[error_handler_entry_count - 1].filename[i++] = *fname_ptr++;
    }
    dbl_err_log[error_handler_entry_count - 1].filename[i] = *fname_ptr;
  } 

  /* Kick the dog while we proceed */
  DBL_KICK_WATCHDOG(); 

  /* Try to enter the FSBL dload on a first-chance error condition. */
  if ( error_handler_entry_count < 2 )
  {
    /* This function will never return */
    dbl_fsbl_dload();
  }
  /* Try to enter the PBL dload on a first- or second-chance error condition. */
  if ( error_handler_entry_count < 3 )
  {
    /* This function will never return */
    boot_pbl_enter_downloader();
  }

  /* In case of re-entrant error handler or dload entry failure, just loop */
  dbl_loop_here();

} /* dbl_error_handler() */
/*lint -restore */

/*===========================================================================

FUNCTION  DBL_INSTALL_ERROR_CALLBACK

DESCRIPTION
  This function installs a callback function that will be executed when
  an error occurs.  The DBL does not support this feature.

DEPENDENCIES
  None 
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dbl_install_error_callback
(
  bl_error_callback_func_type cb_func,
  void *data_ptr,
  bl_error_callback_node_type *node_ptr
)
{
  DBL_ERR_FATAL( (uint32)DBL_ERR_OTHERS );
} /* dbl_install_error_callback() */

/*===========================================================================

FUNCTION  DBL_NESTED_EXCEPTION_C_HANDLER

DESCRIPTION
  This function enables the JTAG interface and never returns.  It is
  called when a nested exception occurs.

  Note: For security purposes, our main concern is concealing the hardware
        key when the JTAG interface is enabled.  This is handled by hardware.
        Because of this, it is not required that all memory and registers be
        cleaned out before enabling the JTAG interface.  The hardware key is
        never accessed or used by the QCSBL. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This function never returns.

===========================================================================*/
void dbl_nested_exception_c_handler( void )
{ 
  /* Loop here until JTAG is connected. */
  dbl_loop_here(); 
} /* dbl_nested_exception_c_handler() */

/*===========================================================================


FUNCTION  DBL_<exception_type>_C_HANDLER

DESCRIPTION
  These are the exception handlers for the DBL.  They include handlers for
  each of the following exception types:

    - undefined instruction
    - software interrupt
    - prefetch abort
    - data abort
    - reserved
    - IRQ
    - FIQ
  
  The DBL considers each of these exceptions a fatal error and handles
  them by calling the qcsbl_error_handler().  A separate handler is assigned
  to each exception for the purpose of providing additional debug
  information.  Each call to the DBL_ERR_FATAL() provides filename, file
  line number & SBL ERROR TYPE information.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  These handlers never return.

===========================================================================*/
void dbl_undefined_instruction_c_handler( void )
{
  DBL_ERR_FATAL((uint32)DBL_ERR_UNDEF_INSTR);
}

void dbl_swi_c_handler( void )
{
  DBL_ERR_FATAL((uint32)DBL_ERR_SWI );
}

void dbl_prefetch_abort_c_handler( void )
{
  DBL_ERR_FATAL((uint32) DBL_ERR_PREFETCH_ABORT );
}

void dbl_data_abort_c_handler( void )
{
  DBL_ERR_FATAL( (uint32)DBL_ERR_DATA_ABORT );
}

void dbl_reserved_c_handler( void )
{
  DBL_ERR_FATAL( (uint32)DBL_ERR_RESERVED_HANDLER );
}

void dbl_irq_c_handler( void )
{
  DBL_ERR_FATAL( (uint32)DBL_ERR_IRQ );
}

void dbl_fiq_c_handler( void )
{
  DBL_ERR_FATAL( (uint32)DBL_ERR_FIQ );
}

/*===========================================================================


FUNCTION  __RT_RAISE

DESCRIPTION
  This is the re-implementation of the C library runtime exception handler.
  This gets called from the C library, for example, when integer
  divide-by-zero occurs.  Re-implementing this handler prevents linking in
  the standard C library signal handling routines and reduces the overall
  image size.  The QCSBL considers a C library runtime exception fatal
  and handles this condition by calling the dbl_error_handler().

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This handler never returns.

===========================================================================*/
/*lint -save -e715 code not reference */
void __rt_raise( int signal, int type )
{
  DBL_ERR_FATAL( (uint32)DBL_ERR_C_LIB_RT );
}
/*lint -restore */

