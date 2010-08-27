#ifndef BOOT_CLOBBER_PROTECTION_H
#define BOOT_CLOBBER_PROTECTION_H

/*===========================================================================

                           B o o t   L o a d e r
                     C l o b b e r  P r o t e c t i o n
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definitions and declarations for the 
  boot-specific clobber protection module, to facilitate preventing the
  bootloader address spaces from being overwritten by a flash load.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_clobber_prot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"

/* BOOT_TARGET_H supercedes BOOT_MSM_H */
#if defined(BOOT_TARGET_H)
#include BOOT_TARGET_H
#elif defined(BOOT_MSM_H)
#include BOOT_MSM_H
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* This defines the maximum number of protected regions in the global
   protection array */
#ifndef BOOT_CLOBBER_MAX_PROTECTED_REGIONS
#define BOOT_CLOBBER_MAX_PROTECTED_REGIONS 6
#endif /* BOOT_CLOBBER_MAX_PROTECTED_REGIONS */


typedef struct boot_clobber_global_type
{
  uint32 num_entries;  /* Number of protected regions present in the array */
  
  struct
  {
    const uint8 *start_addr;  /* The start address of a protected region */
    uint32 size;              /* The size of a protected region          */
  } region[BOOT_CLOBBER_MAX_PROTECTED_REGIONS];
  
} boot_clobber_global_type;


/*=========================================================================

MACRO  BOOT_CLOBBER_VERIFY

DESCRIPTION
  Macro to verify a condition and transition to an error state if the
  condition is not met.  Default error bahavior is to return FALSE to
  the caller.  This behavior can be over-ridden by re-defining the macro
  in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  FALSE if the condition was not met.

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BOOT_CLOBBER_VERIFY
#define BOOT_CLOBBER_VERIFY( cond ) \
  do \
  { \
    if ( ! (cond) ) \
    { \
      return FALSE; \
    } \
  } while(0)
#endif /* BOOT_CLOBBER_VERIFY */

/*=========================================================================

MACRO  BOOT_CLOBBER_GLOBAL_ADDR_CHECK

DESCRIPTION
  Macro to perform a global address verification via macro test.  This 
  macro is primarily for backwards-compatibility with existing QCSBL
  functionality.  The default behavior is to do nothing, but this can
  be over-ridden by re-defining the macro in BOOT_MSM_H.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifndef BOOT_CLOBBER_GLOBAL_ADDR_CHECK
#define BOOT_CLOBBER_GLOBAL_ADDR_CHECK( addr )
#endif /* BOOT_CLOBBER_GLOBAL_ADDR_CHECK */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_CLOBBER_ADD_GLOBAL_PROTECTION_REGION

DESCRIPTION
  Function to add a region of memory to the global bootloader overwrite
  protection.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if there was space left in the global array, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean boot_clobber_add_global_protection_region
(
  const void *start_addr,  /* Start address if the region to protect */
  uint32 size              /* Size in bytes of the region to protect */
);

/*=========================================================================

FUNCTION  BOOT_CLOBBER_CHECK_GLOBAL_ADDRESS_RANGE

DESCRIPTION
  Function to check that the given address does not overlap any of the
  protection regions of memory.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the address range does not overlap a protected region,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean boot_clobber_check_global_address_range
(
  const void *start_addr,  /* Start address if the region to check */
  uint32 size              /* Size in bytes of the region to check */
);

/*=========================================================================

FUNCTION  BOOT_CLOBBER_IMPORT_GLOBAL_TABLE

DESCRIPTION
  Function to import a clobber table structure.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if there was space left in the global array, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern boolean boot_clobber_import_global_table
(
  const boot_clobber_global_type *table
);

/*=========================================================================

FUNCTION  BOOT_CLOBBER_EXPORT_GLOBAL_TABLE

DESCRIPTION
  Function to export the global clobber table structure.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the global clobber table exported

SIDE EFFECTS
  None

===========================================================================*/
extern const boot_clobber_global_type* boot_clobber_export_global_table( void );

#endif /* BOOT_CLOBBER_PROTECTION_H */

