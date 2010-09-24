#ifndef HW_CRASH_DEBUG_H
#define HW_CRASH_DEBUG_H

/*===========================================================================

                         H W C r a s h D e b u g   
                           H e a d e r  F i l e

DESCRIPTION
 Contains common defintion used by to determine whether crash debug option
 is available.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/hw/hw_crash_debug.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/06   anb      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  HW_CRASH_DEBUG_ALLOWED

DESCRIPTION
  Determines if identifier is present in IRAM to tell whether memory debug
  option is allowed
  
DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates that memory debug option is available

SIDE EFFECTS
  None

===========================================================================*/
boolean hw_crash_debug_allowed(void);

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  HW_CRASH_DEBUG_INIT

DESCRIPTION
 This function will init memory dump option shared data.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void hw_crash_debug_init(boolean read_status);
#endif

#endif /* HW_MEM_DEBUG_H */

