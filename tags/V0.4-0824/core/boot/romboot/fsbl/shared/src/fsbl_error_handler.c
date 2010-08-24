
/*===========================================================================

                    F A I L    S A F E     B O O T    L O A D E R 
                         E r r o r  H a n d l e r

DESCRIPTION
  This header file contains declarations and definitions for the FSBL
  error handler.  It is called in the event a FSBL failure is detected.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007, 2008  by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_error_handler.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/08   PS      Initial version.
============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include <string.h>
#include "fsbl_error_handler.h"
#include "fsbl.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Function prototype */
static void fsbl_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      err_code          /* Enum that describes error type */
);
static void fsbl_install_error_callback
(
  bl_error_callback_func_type,
  void *,
  bl_error_callback_node_type *node_ptr
);

/* The global boot error interface structure */
boot_error_if_type bl_err_if = 
{
  fsbl_error_handler,
  fsbl_install_error_callback
};

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  FSBL_ERROR_HANDLER_INIT

DESCRIPTION
  This function is the error handler init for the FSBL. This function 
  initialises the error handler to be used. DBL or OSBL would call this 
  function to initialise the fsbl error handle to use dbl or osbl error 
  handler respectively.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void fsbl_error_handler_init
(
  boot_error_if_type *err_if
)
{
  if ( ( err_if != NULL  ) && 
       ( err_if->error_handler != NULL ) &&
       ( err_if->install_callback != NULL ) )
  {
    memcpy( &bl_err_if, err_if, sizeof(bl_err_if) );
  }
} /* fsbl_error_handler_init() */



/*===========================================================================

FUNCTION  FSBL_ERROR_HANDLER

DESCRIPTION
  This function is the error handler for the FSBL.  The default behavior is
  to just loop forever.  This function must be overridden by the DBL or
  OSBL error handlers using fsbl_error_handler_init() to be useful.

DEPENDENCIES
  Exception handler. 
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void fsbl_error_handler
(
  const char* filename_ptr,     /* File this function was called from -
                                   defined by __FILE__ */
  uint32      line,             /* Line number this function was called
                                   from - defined by  __LINE__ */
  uint32      error             /* Enum that describes error type */
)
{ 
  fsbl_loop_here();
} /* fsbl_error_handler() */

/*===========================================================================

FUNCTION  FSBL_INSTALL_ERROR_CALLBACK

DESCRIPTION
  This function installs a callback function that will be executed when
  an error occurs.  The FSBL does not support this feature.

DEPENDENCIES
  None 
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void fsbl_install_error_callback
(
  bl_error_callback_func_type cb_func,
  void *data_ptr,
  bl_error_callback_node_type *node_ptr
)
{
  
} /* fsbl_install_error_callback() */


