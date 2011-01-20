#ifndef BOOT_CLOBBER_PROTECTION_LOCAL_H
#define BOOT_CLOBBER_PROTECTION_LOCAL_H

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

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_clobber_prot_local.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_clobber_prot.h"

/* BOOT_TARGET_H supercedes BOOT_MSM_H */
#if defined(BOOT_TARGET_H)
#include BOOT_TARGET_H
#elif defined(BOOT_MSM_H)
#include BOOT_MSM_H
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/* This defines the maximum number of local holes in a local protection array */
#ifndef BOOT_CLOBBER_MAX_LOCAL_REGION_HOLES
#define BOOT_CLOBBER_MAX_LOCAL_REGION_HOLES 4
#endif /* BOOT_CLOBBER_MAX_LOCAL_REGION_HOLES */

/* Local clobber protection data structure defines array of protection holes */
typedef struct boot_clobber_local_type
{
  uint32 num_holes;  /* Number of holes present in the array */

  struct
  {
    const uint8 *start_addr;  /* Start address of a hole */
    uint32 size;              /* Size of a hole */
  } region_hole[BOOT_CLOBBER_MAX_LOCAL_REGION_HOLES];
  
} boot_clobber_local_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
extern void boot_clobber_local_init
(
  boot_clobber_local_type *clobber_tbl_ptr  /* Pointer to the local table */
);

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
extern boolean boot_clobber_add_local_hole
(
  boot_clobber_local_type *clobber_tbl_ptr,  /* Pointer to the local table */
  const void *start_addr,  /* Start address of the local hole to add */
  uint32 size              /* Size of the local hole to add in bytes */
);

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
extern boolean boot_clobber_check_local_address_range
(
  boot_clobber_local_type *clobber_tbl_ptr,  /* Pointer to the local table */
  const void *start_addr,  /* Start address of the region to check */
  uint32 size              /* Size of the region to check in bytes */
);

#endif /* BOOT_CLOBBER_PROTECTION_LOCAL_H */

