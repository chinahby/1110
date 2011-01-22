/*=============================================================================

                         B o o t  C r a s h  D e b u g
 
GENERAL DESCRIPTION
  This file contains the code deciding whether memory dumps can be 
  uploaded or written to File system when mobile crashes.
  
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

  $Header: //depot/asic/qsc1100/drivers/boot/boot_crash_debug.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/30/07   ps       Adding support for SECBOOT 2.0
05/02/06   anb      Initial revision.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "target.h"

/*=============================================================================
  If Feature is ON then the remaining portion of this file will get compiled
=============================================================================*/
#ifdef FEATURE_CRASH_DEBUG_DISABLED_CHECK

#include "boot_crash_debug.h"
#include "boot_shared_seg.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Structure to define ID to enter download mode. */ 
typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
} crash_debug_id_type;

#ifdef FEATURE_SHARED_SEGMENT
/* Magic numbers for notifying OEMSBL that NV item to disable crash debugging is set */
#define CRASH_DEBUG_MAGIC_NUM_1     0x103B5D7F
#define CRASH_DEBUG_MAGIC_NUM_2     0x92A4C6E8

typedef struct
{
    BOOT_SHARED_SEG_HEADER
    crash_debug_id_type cookies;
} crash_debug_shared_type;

#ifndef BUILD_BOOT_CHAIN
#pragma arm section zidata = "SHARED_DATA"
crash_debug_shared_type crash_debug_nv_shared;
#pragma arm section zidata
#endif
#endif

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/
#ifdef FEATURE_SHARED_SEGMENT
/*===========================================================================

FUNCTION  BOOT_CRASH_DEBUG_RESET_SHARED_ID_ADDR

DESCRIPTION
  This function returns the address of QPST download magic numbers.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Address of QPST download magic numbers in SDRAM

SIDE EFFECTS
  None
===========================================================================*/
static crash_debug_id_type * boot_crash_debug_reset_shared_id_addr( void )
{
  crash_debug_shared_type * crash_debug_shared_info_ptr = NULL;

#ifdef BUILD_BOOT_CHAIN
  /* Get the pointer to crash_debug_nv_shared */
  (void)BOOT_SHARED_SEG_GET_DATA(crash_debug_nv_shared,(void**)(&crash_debug_shared_info_ptr));
#else
  /* Allocate crash_debug_nv_shared in shared segment */
  BOOT_SHARED_SEG_ALLOC( crash_debug_nv_shared, sizeof(crash_debug_shared_type),
                         FALSE, (void**)(&crash_debug_shared_info_ptr)
                       );
#endif

  if (crash_debug_shared_info_ptr != NULL)
  {
    return &(crash_debug_shared_info_ptr->cookies);
  }

  return NULL;
}
#endif

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
boolean boot_crash_debug_allowed(void)
{
#ifdef FEATURE_SHARED_SEGMENT    
    crash_debug_id_type *crash_debug_id_ptr = 
               (crash_debug_id_type*) boot_crash_debug_reset_shared_id_addr();

    if (crash_debug_id_ptr == NULL)
    {
      return FALSE;
    }
    
    /* Check to see if magic numbers were set this will tell whether 
     * NV item was set to TRUE thus disallowing/disabling memory dump
     * option.
     */
    if ( (crash_debug_id_ptr->magic_1 == CRASH_DEBUG_MAGIC_NUM_1) &&
         (crash_debug_id_ptr->magic_2 == CRASH_DEBUG_MAGIC_NUM_2)
       )
    {
      return FALSE;
    }
#endif
  return TRUE;
}

#ifndef BUILD_BOOT_CHAIN
/*===========================================================================

FUNCTION  BOOT_CRASH_DEBUG_SHARED_INFO_INIT

DESCRIPTION
 This function will init memory dump option shared data.

DEPENDENCIES
 None.
 
RETURN VALUE
 None.                                                               

SIDE EFFECTS
 None.
===========================================================================*/
void boot_crash_debug_shared_info_init(boolean read_status)
{
#ifdef FEATURE_SHARED_SEGMENT
  crash_debug_id_type *crash_debug_id_ptr = 
              (crash_debug_id_type*) boot_crash_debug_reset_shared_id_addr();

  if (crash_debug_id_ptr == NULL)
    return;

  /* Start with clearing the magic numbers. By clearing the memory we
   * indiacte that memory dumps option is available
   */
  crash_debug_id_ptr->magic_1 = 0;
  crash_debug_id_ptr->magic_2 = 0;

  /* If the NV_MEMORY_DUMP_DISALLOWED_I item is set then we will set the 
   * magic number to indicate that memory debug option is NOT enabled
   * via NV ITEM.
   */
  if (read_status == TRUE) 
  {
    crash_debug_id_ptr->magic_1 = CRASH_DEBUG_MAGIC_NUM_1;
    crash_debug_id_ptr->magic_2 = CRASH_DEBUG_MAGIC_NUM_2;  
  }

#endif   
}
#endif /* BUILD_BOOT_CHAIN */


#endif /* #ifdef FEATURE_CRASH_DEBUG_DISABLED_CHECK */
