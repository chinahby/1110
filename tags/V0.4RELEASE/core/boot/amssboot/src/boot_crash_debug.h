#ifndef BOOT_CRASH_DEBUG_H
#define BOOT_CRASH_DEBUG_H

/*===========================================================================

                       B o o t  D o w n l o a d e r   
                           H e a d e r  F i l e

DESCRIPTION
 Contains common defintion used by boot downloader.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msm6800b/drivers/boot/boot_crash_debug.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/02/06   anb      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifndef _ARM_ASM_
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  BOOT_CRASH_DEBUG_ALLOWED

DESCRIPTION
  Determines if identifier is present in IRAM to tell whether memory debug
  option is allowed. 
  
DEPENDENCIES
  Data in IRAM is retained across a reset.
  
RETURN VALUE
  TRUE indicates that memory debug option is available

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_crash_debug_allowed(void);

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_CRASH_DEBUG_SHARED_INFO_INIT

DESCRIPTION
 This function will init crash debug shared data.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_crash_debug_shared_info_init(boolean read_status);
#endif /* BUILD_BOOT_CHAIN */

#ifdef BUILD_BOOT_CHAIN

 
#endif /* BUILD_BOOT_CHAIN */

#endif /* _ARM_ASM_ */

#endif /* BOOT_CRASH_DEBUG_H */
