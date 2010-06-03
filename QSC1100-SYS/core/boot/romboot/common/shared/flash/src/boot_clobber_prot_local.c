/*=============================================================================

                           B o o t   L o a d e r
                     C l o b b e r  P r o t e c t i o n

GENERAL DESCRIPTION
  This function contains the implementation for the boot-specific clobber
  protection module, to facilitate preventing the bootloader address spaces
  from being overwritten by a flash load.

EXTERNALIZED FUNCTIONS
  boot_clobber_local_init
  boot_clobber_add_local_hole
  boot_clobber_check_local_address_range

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_clobber_prot_local.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/06/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_clobber_prot_local.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  BOOT_CLOBBER_LOCAL_INIT

DESCRIPTION
  Function to initialize a local clobber protection data structure.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_clobber_local_init
(
  boot_clobber_local_type *clobber_tbl_ptr  /* Pointer to the local table */
)
{
  /* All we need to do is set the number of holes to zero */
  if ( clobber_tbl_ptr )
  {
    clobber_tbl_ptr->num_holes = 0;
  }
} /* boot_clobber_local_init() */


/*=========================================================================

FUNCTION  BOOT_CLOBBER_ADD_LOCAL_HOLE

DESCRIPTION
  Function to add a local hole to the bootloader overwrite protection regions.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if there was space left in the local array, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_clobber_add_local_hole
(
  boot_clobber_local_type *clobber_tbl_ptr,  /* Pointer to the local table */
  const void *start_addr,  /* Start address of the local hole to add */
  uint32 size              /* Size of the local hole to add in bytes */
)
{
  boolean success;
  
  if ( clobber_tbl_ptr && 
       clobber_tbl_ptr->num_holes < BOOT_CLOBBER_MAX_LOCAL_REGION_HOLES )
  {
    /* If we have room left in the array then add this hole to the end */
    clobber_tbl_ptr->region_hole[clobber_tbl_ptr->num_holes].start_addr =
      (const uint8*) start_addr;
    clobber_tbl_ptr->region_hole[clobber_tbl_ptr->num_holes].size = size;
    clobber_tbl_ptr->num_holes++;
    success = TRUE;
  }
  else
  {
    /* If we don't have room left in the local array */
    success = FALSE;
  }

  BOOT_CLOBBER_VERIFY( success );
  return success;
} /* boot_clobber_add_local_hole() */


/*=========================================================================

FUNCTION  BOOT_CLOBBER_CHECK_LOCAL_ADDRESS_RANGE

DESCRIPTION
  Function to check that the given address range either fits inside one of
  the local holes defined in the protected address regions, or that the
  given address range does not overlap any of the protected address regions.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the given address range completely falls within a local hole or
  if the given address range does not overlap with a protected region,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean boot_clobber_check_local_address_range
(
  boot_clobber_local_type *clobber_tbl_ptr,  /* Pointer to the local table */
  const void *start_addr,  /* Start address of the region to check */
  uint32 size              /* Size of the region to check in bytes */
)
{
  uint32 i;

  /* Verify our input parameter */
  BOOT_CLOBBER_VERIFY( clobber_tbl_ptr != NULL );

  /* Perform implementation-specific address range checking */
  BOOT_CLOBBER_GLOBAL_ADDR_CHECK( start_addr );
  
  /* Check to see if the address range fits inside any of the locally defined
     holes within the protected space */
  for ( i = 0; i < clobber_tbl_ptr->num_holes; i++ )
  {
    if ( ( (const uint8*) start_addr >=
           clobber_tbl_ptr->region_hole[i].start_addr ) &&
         ( (const uint8*) start_addr + size <= 
           ( clobber_tbl_ptr->region_hole[i].start_addr +
             clobber_tbl_ptr->region_hole[i].size ) ) &&
         ( size <= clobber_tbl_ptr->region_hole[i].size ) )
    {
      /* If the address range to check fits, then return TRUE */
      return TRUE;
    }
  }

  /* If we get here then the address range does not fit inside any of the holes,
     and so we need to make sure that it does not overlap any of the 
     protected regions. */
  return boot_clobber_check_global_address_range( start_addr, size );
  
} /* boot_clobber_check_local_address_range() */

