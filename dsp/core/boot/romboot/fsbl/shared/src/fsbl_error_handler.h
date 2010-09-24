#ifndef FSBL_ERROR_HANDLER_H
#define FSBL_ERROR_HANDLER_H

/*===========================================================================

                    F A I L   S A F E    B O O T    L O A D E R 
                               E r r o r  H a n d l e r
                                  H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the FSBL
  error handler.  It is called in the event a FSBL failure is detected.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_error_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/08   PS     Initial version.
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_error_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Define the FSBL ERROR types. This classifies different types of errors 
  encountered in the SBL.
 ---------------------------------------------------------------------------*/
typedef enum
{
  FSBL_ERR_NULL_PTR = 0x2000,
  FSBL_ERR_CERT_INDEX_INVALID,
  FSBL_ERR_IMG_SECURITY_FAIL,
  FSBL_ERR_PM_INIT_FAIL,                     /* Null pointer encountered */
  FSBL_ERR_ENUM            = 0x7FFFFFFF    /* Size enum to 32 bits. */
} fsbl_error_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO  FSBL_VERIFY

DESCRIPTION
  Given a boolean expression, this macro will verify the condition is TRUE
  and do nothing if the condition is TRUE, otherwise it will call the
  SBL error handler.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  This macro never returns if the condition is FALSE.

===========================================================================*/
#define FSBL_VERIFY( xx_exp, error_type )  BL_VERIFY( xx_exp, error_type )

/*===========================================================================

MACRO  FSBL_VERIFY_RET

DESCRIPTION
  Given a boolean expression, this macro will verify the condition is TRUE
  and do nothing if the condition is TRUE, it will return the error_type 
  supplied.

DEPENDENCIES
  NONE
  
RETURN VALUE
  NONE

SIDE EFFECTS
  This macro never returns if the condition is FALSE.

===========================================================================*/
#define FSBL_VERIFY_RET( xx_exp, error_type ) \
            do { \
               if( !(xx_exp) ) \
               { \
                 return error_type;\
               } \
            } while(0)

/*===========================================================================

MACRO  FSBL_ERR_FATAL

DESCRIPTION
  This macro calls the SBL error handler.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This macro never returns.

===========================================================================*/
#define FSBL_ERR_FATAL( error_type )  BL_ERR_FATAL( error_type )


/*===========================================================================

FUNCTION  FSBL_ERROR_HANDLER_INIT

DESCRIPTION
  This function is the error handler init for the FSBL. This function 
  initialises the error handler to be used. DBL or OSBL would call this 
  function to initialise the fsbl error handle to use dbl or osbl error 
  handler respectively.
  
DEPENDENCIES
  Exception handler. 
  
RETURN VALUE
  None

SIDE EFFECTS
  This function never returns.

===========================================================================*/
extern void fsbl_error_handler_init
(
  boot_error_if_type *err_if
);


#endif  /* FSBL_ERROR_HANDLER_H */

