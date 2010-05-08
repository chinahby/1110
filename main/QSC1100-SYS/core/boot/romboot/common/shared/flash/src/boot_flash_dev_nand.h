#ifndef BOOT_FLASH_DEV_NAND_H
#define BOOT_FLASH_DEV_NAND_H

/*===========================================================================

                           B o o t   L o a d e r
                 N A N D  F l a s h  D e v i c e  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the boot NAND flash device module, and declarations for the public
  functions in the module.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_dev_nand.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/08   MJS     Use flash_open() API.
12/07/07   MJS     Initial revision, ported from MDM7800
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash.h"
#include "boot_flash_dev_nand_if.h"
#include "miparti.h"
#include "mibbtb.h"

#include "flashi.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_NAND_VERSION 2
#define BOOT_FLASH_DEV_NAND_NAME "boot_flash_dev_nand"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_NAND_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize the NAND flash device module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void boot_flash_dev_nand_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
);

/*===========================================================================

FUNCTION  BL_FLASH_NAND_LOAD_PARTITION_TABLE

DESCRIPTION
  This function loads the NAND flash partition table.

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None

===========================================================================*/
extern flash_partable_t bl_flash_nand_load_partition_table
(
  struct bl_shared_data_type *bl_shared_data
);

/*===========================================================================

FUNCTION BL_FLASH_NAND_LOAD_BAD_BLOCK_TABLE

DESCRIPTION
  This function loads the NAND flash bad block table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern struct bad_block_table* bl_flash_nand_load_bad_block_table
(
  flash_partable_t sys_partition_ptr
);

#endif /* BOOT_FLASH_DEV_NAND_H */

