/*=============================================================================

                           B o o t   L o a d e r
                     C l o b b e r  P r o t e c t i o n

GENERAL DESCRIPTION
  This function contains the implementation for the boot-specific clobber
  protection module, to facilitate preventing the bootloader address spaces
  from being overwritten by a flash load.

EXTERNALIZED FUNCTIONS
  boot_clobber_add_global_protection_region
  boot_clobber_check_global_address_range

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_clobber_prot.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/06/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_clobber_prot.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Data structure defining an array of memory regions to protect */
static boot_clobber_global_type clobber_table = { 0 };
  
/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


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
boolean boot_clobber_add_global_protection_region
(
  const void *start_addr,  /* Start address if the region to protect */
  uint32 size              /* Size in bytes of the region to protect */
)
{
  boolean success;
  
  /* If we have room left in the array, add to the end of the array */
  if ( clobber_table.num_entries < BOOT_CLOBBER_MAX_PROTECTED_REGIONS )
  {
    clobber_table.region[clobber_table.num_entries].start_addr =
      (const uint8*) start_addr;
    clobber_table.region[clobber_table.num_entries].size = size;
    clobber_table.num_entries++;
    success = TRUE;
  }
  else
  {
    /* If the array is full */
    success = FALSE;
  }

  BOOT_CLOBBER_VERIFY( success );
  return success;
} /* boot_clobber_add_global_protection_region() */


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
boolean boot_clobber_check_global_address_range
(
  const void *start_addr,  /* Start address if the region to check */
  uint32 size              /* Size in bytes of the region to check */
)
{
  uint32 i;
  boolean success = TRUE;

  /* Perform implementation-specific address range checking */
  BOOT_CLOBBER_GLOBAL_ADDR_CHECK( start_addr );

  /* First make sure the region size does not wrap the 32-bit address space;
   * consider this an error if it does */
  if ( (const uint8*) start_addr + size < (const uint8*) start_addr )
  {
    success = FALSE;
  }

  /* Loop through the global array */
  for ( i = 0; success && i < clobber_table.num_entries; i++ )
  {
    /* If the given address range overlaps a protected range, return FALSE */
    if ( ( (const uint8*) start_addr <
           clobber_table.region[i].start_addr + clobber_table.region[i].size ) &&
         ( (const uint8*) start_addr + size > 
           clobber_table.region[i].start_addr ) )
    {
      success = FALSE;
    }
  }

  /* If we get through the global array than we have successfully checked
     against all the regions */
  
  BOOT_CLOBBER_VERIFY( success );

  return success;
} /* boot_clobber_check_global_address_range() */

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
const boot_clobber_global_type* boot_clobber_export_global_table( void )
{
  return &clobber_table;
}

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
boolean boot_clobber_import_global_table
(
  const boot_clobber_global_type *table
)
{
  int i = 0;
  boolean success = TRUE;
  
  while ( table != NULL && success && i < table->num_entries )
  {
    success = boot_clobber_add_global_protection_region( table->region[i].start_addr,
                                                         table->region[i].size );
    i++;
  }

  return success;
}
