#ifndef BOOT_FLASH_PARTITION_H
#define BOOT_FLASH_PARTITION_H

/*=============================================================================

                            B o o t   L o a d e r
                        F l a s h   P a r t i t i o n
                            H e a d e r  F i l e

DESCRIPTION
  This header file contains declarations and definitions for the boot
  software that performs operation dealing with the system partition table.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_partition.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/04/07   MJS     Ported from MSM7800.

=============================================================================*/

/*=============================================================================
 
                           INCLUDE FILES

=============================================================================*/
#include "boot_comdef.h"
#include "miparti.h"


/*=============================================================================

                      PUBLIC DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                      PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*===========================================================================

FUNCTION  BOOT_FLASH_FIND_PARTITION_ENTRY

DESCRIPTION
  This function finds the requested partition table entry within
  the partition table and returns a pointer to it. 

DEPENDENCIES
  Partition table must be loaded in RAM.
  
RETURN VALUE
  Pointer to partition table entry.

SIDE EFFECTS
  Executes the error handler and never returns if an error is detected
  or the partition table entry cannot be found.

===========================================================================*/
flash_partentry_ptr_type boot_flash_find_partition_entry
(
  flash_partable_t parti_tbl_ptr,           /* Partition table pointer */
  const char *parti_entry_name              /* Name of partition table entry
                                               to find */
);

#endif /* BOOT_FLASH_PARTITION_H */

