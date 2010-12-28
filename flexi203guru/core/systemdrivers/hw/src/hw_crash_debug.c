/*=============================================================================

                  H a r d w a r e  C r a s h  D e b u g
 
GENERAL DESCRIPTION
  This file contains the code deciding whether memory dumps can be 
  uploaded or written to File system when a crash happens.
  
EXTERNALIZED FUNCTIONS
  None
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/drivers/hw/hw_crash_debug.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/29/06   anb      Change code to use EFUSE api's to read item from registers.
05/02/06   anb      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "hw_crash_debug.h"  
/*  To remove compiler warning moved #include "hw_crash_debug.h" from
     under #define FEATURE_CRASH_DEBUG_DISABLED_CHECK to here  */

#ifdef BUILD_BOOT_CHAIN

#else
#include "target.h"
#endif

/*=============================================================================
  If Feature is ON then the remaining portion of this file will get compiled
=============================================================================*/
#ifdef FEATURE_CRASH_DEBUG_DISABLED_CHECK

#include "boot_crash_debug.h"
#include "efuse.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/
#ifndef FEATURE_SHARED_SEGMENT
boolean hw_crash_debug_disabled = TRUE;
#endif

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
/*===========================================================================

FUNCTION  HW_CRASH_DEBUG_ALLOWED

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
boolean hw_crash_debug_allowed(void)
{
  uint32 register_val=0;
  /*  Step 1. Check whether the software crash debug EFUSE is blown
   *          If yes then no debugging feature is allowed
   *          a. No uploading of memory dump files
   *          b. No memory dumps to EFS
   *  Step 2. If EFUSE is not blown then check whether memory
   *          dump option is controlled via NV item.
   *          If yes then no debugging feature is allowed
   *          a. No uploading of memory dump files
   *          b. No memory dumps to EFS
   */

  /* Step 1 */
  if ( (efuse_get_item(SOFTWARE_CRASH_DEBUG, &register_val) == EFUSE_NO_ERR) &&
       (register_val)
     )
  {
    return FALSE;
  }
  else
  {
    /* Step 2 */
#ifdef FEATURE_SHARED_SEGMENT
    return boot_crash_debug_allowed();
#else 
    /* The NV item decides whether the crash debugging feature is disabled
     * by setting it to TRUE.
     * But this function returns NOT of the NV item value.
     */
    return (!hw_crash_debug_disabled);

#endif
  }
  return TRUE;

}

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
void hw_crash_debug_init(boolean read_status)
{
#ifdef FEATURE_SHARED_SEGMENT
  boot_crash_debug_shared_info_init(read_status);
#else
  hw_crash_debug_disabled = read_status;
#endif
}
#endif /* #ifndef BUILD_BOOT_CHAIN */

#endif /* FEATURE_CRASH_DEBUG_DISABLED_CHECK */

