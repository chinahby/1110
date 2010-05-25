/*=============================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                  F l a s h   I n i t i a l i z a t i o n

GENERAL DESCRIPTION
  This header file contains declarations and definitions for the OSBL
  software to intialize the boot flash device module.

EXTERNALIZED FUNCTIONS
  osbl_flash_target_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/osbl/osbl_flash_target.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/07/07   MJS     Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "boot_comdef.h"
#include "osbl.h"
#include "osbl_flash.h"
#include "boot_flash_dev_nor.h"
#include "osbl_error_handler.h"
#include "boot_pbl_accessor.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION  OSBL_FLASH_DEV_NOR_TARGET_SHARE_DATA

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static boolean osbl_flash_dev_nor_target_share_data
(
  struct bl_shared_data_type *bl_shared_data
)
{
  /* Check parameters first */
  BL_VERIFY( ( bl_shared_data != NULL &&
               bl_shared_data->boot_dev_shared_data != NULL ),
             BL_ERR_NULL_PTR_PASSED );

////  bl_shared_data->boot_dev_shared_data->sys_partition_ptr = dev_priv->partable_ptr;

  bl_shared_data->boot_dev_shared_data->amss_header_offset = (uint32)MI_NOR_AMSS_BOOT_ELF_HEADER_SRC_ADDR;


  return TRUE;
} /* osbl_flash_dev_nor_target_share_data() */

/*=========================================================================

FUNCTION  OSBL_FLASH_DEV_NOR_TARGET_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize the NOR flash device module

DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void osbl_flash_dev_nor_target_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
)
{
  boot_flash_dev_nor_if_type *dev_if = NULL;

  /* Initialize the NOR flash device module. */
  boot_flash_dev_nor_init_and_register( flash_type,
                                        trans_factory_init,
                                    bl_shared_data );

  /* Get the device interface so we can override some of the default 
   * behaviors with target-specific behaviors */
  dev_if = boot_flash_dev_nor_if_get_singleton();

  /* Override the share data function */
  dev_if->vtbl->parent.share_data = osbl_flash_dev_nor_target_share_data;

} /* osbl_flash_dev_nor_target_init_and_register() */

/*===========================================================================

FUNCTION  OSBL_FLASH_TARGET_INIT

DESCRIPTION
  This function initializes the flash device module according to the boot
  flash type.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_flash_target_init
(
  bl_shared_data_type *bl_shared_data
)
{
  boot_flash_type boot_device;


  BL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Get the boot device type */
  boot_device = boot_pbl_get_flash_type();
  
  /* Initialize the flash device module based on the boot flash type. */
  switch ( boot_device )
  {
    case NOR_FLASH:
      /* Initialize the NOR flash device module. */
      osbl_flash_dev_nor_target_init_and_register( boot_device,
                                                   boot_flash_trans_nor_factory_init,
                                                   bl_shared_data );
      break;

   default:
      break;
  }

  /* Lock the boot device singleton */
  boot_flash_dev_if_lock_singleton();

} /* osbl_flash_target_init() */

