/*=============================================================================

               OS  S e c o n d a r y   B o o t   L o a d e r
                         E r r o r  H a n d l e r

GENERAL DESCRIPTION
  This file contains the QUALCOMM Secondary Boot Loader (SBL) error handler.
  The OSBL error handler is called in the event an error is detected.

EXTERNALIZED FUNCTIONS
  osbl_error_handler
  osbl_undefined_instruction_c_handler
  osbl_swi_c_handler
  osbl_prefetch_abort_c_handler
  osbl_data_abort_c_handler
  osbl_reserved_c_handler
  osbl_irq_c_handler
  osbl_fiq_c_handler
  __rt_raise
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_error_handler.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "osbl.h"
#include "osbl_error_handler.h"
#include "osbl_hw_init.h"
#include "osbl_dload.h"
#include "osbl_flash.h"
#include "miboot.h"
#include "cache_mmu.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
 
/*---------------------------------------------------------------------------
  Definitions of OSBL_ERR_MAX_FILENAME_SIZE to be logged in on OSBL error
---------------------------------------------------------------------------*/
#define OSBL_ERR_MAX_FILENAME_SIZE 20 /* 20 * 4 = 80 chars */ 

/*---------------------------------------------------------------------------
  Define just to make sure the link list doesn't go off into the weeds.
---------------------------------------------------------------------------*/
#define OSBL_MAX_NUM_ERROR_CALLBACKS 20

/* Function prototypes */
static void osbl_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      err_code          /* Enum that describes error type */
);
static void osbl_install_error_callback
(
  bl_error_callback_func_type,
  void *,
  bl_error_callback_node_type *node_ptr
);

/*---------------------------------------------------------------------------
  Define the OSBL ERROR Log entry types. 
  ---------------------------------------------------------------------------*/
typedef struct osbl_err_log_entry_type
{
  uint32 error_type;
  uint32 line;
  uint32 filename[OSBL_ERR_MAX_FILENAME_SIZE];
} osbl_err_log_entry_type;

typedef struct osbl_err_count_type
{
  uint32 sbl_type;
  uint32 error_count;
} osbl_err_count_type;


/*---------------------------------------------------------------------------
  Define the error logging and error callback variables.
  ---------------------------------------------------------------------------*/
#pragma arm section rwdata = "ERR_DATA", zidata = "ERR_DATA"
/* Structure defining the layout of OSBL error records. */
static struct
{
  /* The primary OSBL error record. */
  osbl_err_log_entry_type primary_log;

  /* The total number of errors logged. Store this after 1st error record. */
  osbl_err_count_type     err_count;

  /* Array of records for subsequent OSBL errors. */
  osbl_err_log_entry_type secondary_log[1];
} err_log;
#pragma arm section rwdata, zidata

/* Error handler callback data structures */
static bl_error_callback_node_type *err_cb_head_ptr = NULL;

/* The global boot error interface structure */
boot_error_if_type bl_err_if = 
{
  osbl_error_handler,
  osbl_install_error_callback
};


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  OSBL_ERROR_HANDLER

DESCRIPTION
  This function is the error handler for the OSBL and records the line 
  number, filename & error type before jumping into the downloader. 
  This function is also shared by exception handler and can log
  multiple errors if called more than once.

DEPENDENCIES
  Exception handler. 
  
RETURN VALUE
  None

SIDE EFFECTS
  This function never returns.

===========================================================================*/
void osbl_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      err_code          /* Enum that describes error type */
)
{
  register osbl_err_log_entry_type *osbl_err_log_entry_ptr = NULL;
  static uint32 error_handler_entry_count = 0;
  register uint32 *fname_ptr = (uint32 *)filename_ptr;
  register uint32 error_type = err_code;
  register uint32 * const sbl_name = (uint32*)"OSBL";
  register uint32 i = 0;

  error_handler_entry_count++; /* Count the number of calls to this fn */

  if(error_handler_entry_count == 1) /* Entering Error Handler for first time */
  {
    /* First disable data/instruction caches and MMU */
    cache_mmu_disable();

    /* Pointer to the primary error log */
    osbl_err_log_entry_ptr = &err_log.primary_log;

    /* Log that this is OSBL error */
    err_log.err_count.sbl_type = *sbl_name;
  }
  else /* For all subsequent calls to this fn */
  {
    /* Pointer to the error log for subsequent errors */
    osbl_err_log_entry_ptr = &err_log.secondary_log[error_handler_entry_count];
  }

  /* Save total number of errors logged */
  err_log.err_count.error_count = error_handler_entry_count;

  if ( (void*) (osbl_err_log_entry_ptr + 1) <= 
       (void*) (OSBL_ERR_DATA_BASE + OSBL_ERR_DATA_SIZE) )
  {
    /* Log the error type */
    osbl_err_log_entry_ptr->error_type = error_type;

    /* Log the line number */
    osbl_err_log_entry_ptr->line = line;

    i= 0;

    /* Log the filename if pointer not NULL */
    if ( fname_ptr != NULL )
    {
      /* Save filename */
      while (  ( (*fname_ptr & 0xff000000) != 0x0 ) &&
               ( (*fname_ptr & 0x00ff0000) != 0x0 ) &&
               ( (*fname_ptr & 0x0000ff00) != 0x0 ) &&
               ( (*fname_ptr & 0x000000ff) != 0x0 ) &&
                 (i < (OSBL_ERR_MAX_FILENAME_SIZE - 1) )
               )
      {
        osbl_err_log_entry_ptr->filename[i++] = *fname_ptr++;
      }
      osbl_err_log_entry_ptr->filename[i] = *fname_ptr;
    }
  }

  if ( error_handler_entry_count == 1 )
  {
    /* Only try to execute the error callbacks the first time through
       the error handler in case one of the callbacks caused the
       subsequent error. */

    register bl_error_callback_node_type *node_ptr;

    for ( node_ptr = err_cb_head_ptr, i = 0;
          node_ptr != NULL && i < OSBL_MAX_NUM_ERROR_CALLBACKS;
          node_ptr = node_ptr->next_ptr, i++ )
    {
      node_ptr->data.cb_func( node_ptr->data.data_ptr );
    }
  }

  /* Enter downloader for QPST */
  osbl_dload_entry( &bl_shared_data );

  /* In case something drastic happens, enter infinite loop */
  osbl_loop_here();
  
} /* osbl_error_hander() */

/*===========================================================================

FUNCTION  OSBL_INSTALL_ERROR_CALLBACK

DESCRIPTION
  This function installs a callback function that will be executed when
  an error occurs.

DEPENDENCIES
  None 
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_install_error_callback
(
  bl_error_callback_func_type cb_func,
  void *data_ptr,
  bl_error_callback_node_type *node_ptr
)
{
  BL_VERIFY( cb_func != NULL && node_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  node_ptr->data.cb_func = cb_func;
  node_ptr->data.data_ptr = data_ptr;
  node_ptr->next_ptr = err_cb_head_ptr;

  err_cb_head_ptr = node_ptr;

} /* osbl_install_error_callback() */

/*===========================================================================

FUNCTION  OSBL_NESTED_EXCEPTION_C_HANDLER

DESCRIPTION
  This function enables the JTAG interface and never returns.  It is
  called when a nested exception occurs.

  Note: For security purposes, our main concern is concealing the hardware
        key when the JTAG interface is enabled.  This is handled by hardware.
        Because of this, it is not required that all memory and registers be
        cleaned out before enabling the JTAG interface.  The hardware key is
        never accessed or used by the OSBL. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This function never returns.

===========================================================================*/
void osbl_nested_exception_c_handler( void )
{
  /* First disable data/instruction caches and MMU */
  cache_mmu_disable();

  /* Disable modem Tx, enable JTAG access, and lock e-fuse override
     registers. */
  osbl_hw_smc_lock();
  
  /* Loop here until JTAG is connected. */
  osbl_loop_here();
  
} /* osbl_nested_exception_c_handler() */

/*===========================================================================


FUNCTION  OSBL_<exception_type>_C_HANDLER

DESCRIPTION
  These are the exception handlers for the OSBL.  They include handlers for
  each of the following exception types:

    - undefined instruction
    - software interrupt
    - prefetch abort
    - data abort
    - reserved
    - IRQ
    - FIQ
  
  The OSBL considers each of these exceptions a fatal error and handles
  them by calling the osbl_error_handler().  A separate handler is assigned
  to each exception for the purpose of providing additional debug
  information.  Each call to the OSBL_ERR_FATAL() provides filename, file
  line number & SBL ERROR TYPE information.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  These handlers never return.

===========================================================================*/
void osbl_undefined_instruction_c_handler( void )
{
  OSBL_ERR_FATAL(BL_ERR_UNDEF_INSTR);
}

void osbl_swi_c_handler( void )
{
  OSBL_ERR_FATAL( BL_ERR_SWI );
}

void osbl_prefetch_abort_c_handler( void )
{
  /* Don't use the global error handler pointer in case it's invalid and
     caused the abort */
  osbl_error_handler( __FILE__, __LINE__, BL_ERR_PREFETCH_ABORT );
}

void osbl_data_abort_c_handler( void )
{
  /* Don't use the global error handler pointer in case it's invalid and
     caused the abort */
  osbl_error_handler( __FILE__, __LINE__, BL_ERR_DATA_ABORT );
}

void osbl_reserved_c_handler( void )
{
  OSBL_ERR_FATAL( BL_ERR_RESERVED_HANDLER );
}

void osbl_irq_c_handler( void )
{
  OSBL_ERR_FATAL( BL_ERR_IRQ );
}

void osbl_fiq_c_handler( void )
{
  OSBL_ERR_FATAL( BL_ERR_FIQ );
}

/*===========================================================================


FUNCTION  __RT_RAISE

DESCRIPTION
  This is the re-implementation of the C library runtime exception handler.
  This gets called from the C library, for example, when integer
  divide-by-zero occurs.  Re-implementing this handler prevents linking in
  the standard C library signal handling routines and reduces the overall
  image size.  The OSBL considers a C library runtime exception fatal.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This handler never returns.

===========================================================================*/
void __rt_raise( int signal, int type )
{
  OSBL_ERR_FATAL( BL_ERR_C_LIB_RT );
}

