#ifndef OSBL_ERROR_HANDLER_H
#define OSBL_ERROR_HANDLER_H

/*===========================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                         E r r o r  H a n d l e r
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the OSBL
  error handler.  It is called in the event a OSBL failure is detected.
  Once this osbl_error_handler is called. It will never return. 
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_error_handler.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_error_if.h"
#include "osbl_target.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO  OSBL_VERIFY

DESCRIPTION
  Given a boolean expression, this macro will verify the condition is TRUE
  and do nothing if the condition is TRUE, otherwise it will call the
  OSBL error handler.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
    This macro never returns if the condition is FALSE.

===========================================================================*/
#define OSBL_VERIFY( xx_exp, error_type ) BL_VERIFY( xx_exp, error_type )
            
/*===========================================================================

MACRO  OSBL_ERR_FATAL

DESCRIPTION
  This macro calls the SBL error handler.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  This macro never returns.

===========================================================================*/
#define OSBL_ERR_FATAL( error_type ) BL_ERR_FATAL( error_type )


/*===========================================================================

MACRO  OSBL_VALID_ADDR_INC_IRAM 

DESCRIPTION
  This macro validates the address passed in to insure that it is within
  the address space that SDRAM, PSRAM, NOR, IMEM, or IRAM can exist on the
  MSM.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Calls the SBL error handler if the address falls outside the valid range.

===========================================================================*/
#define OSBL_VALID_ADDR_INC_IRAM( addr, error_type ) \
           BL_VERIFY( (((uint32)(addr) <= RANGE_END_ADDR)) || \
                      (((uint32)(addr) < IRAM_RANGE_END_ADDR) && \
                       ((uint32)(addr) >= IRAM_RANGE_START_ADDR)), error_type )


#endif  /* OSBL_ERROR_HANDLER_H */
