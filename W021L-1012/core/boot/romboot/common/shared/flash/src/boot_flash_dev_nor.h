#ifndef BOOT_FLASH_DEV_NOR_H
#define BOOT_FLASH_DEV_NOR_H

/*===========================================================================

                           B o o t   L o a d e r
                      N O R  D e v i c e  M o d u l e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the private data structure
  for the boot NOR flash device module, and declarations for the public
  functions in the module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_dev_nor.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Initial revision, ported from MDM7800.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_flash.h"
#include "boot_flash_dev_nor_if.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_NOR_VERSION 1
#define BOOT_FLASH_DEV_NOR_NAME "boot_flash_dev_nor"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_NOR_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize the boot NOR device module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void boot_flash_dev_nor_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
);

#endif /* BOOT_FLASH_DEV_NOR_H */

