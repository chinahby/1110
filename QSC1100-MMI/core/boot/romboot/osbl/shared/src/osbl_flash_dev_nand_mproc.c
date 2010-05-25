/*=============================================================================

                OS  S e c o n d a r y   B o o t   L o a d e r
                 N A N D  F l a s h  D e v i c e  M o d u l e

GENERAL DESCRIPTION
  This header file contains the implementation for the OSBL NAND flash
  device module.
  
EXTERNALIZED FUNCTIONS
  osbl_flash_dev_nand_setup_nand_mpu

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_flash_dev_nand_mproc.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/07/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "osbl.h"
#include "osbl_flash.h"
#include "osbl_flash_dev_nand_mproc.h"
#include "osbl_error_handler.h"
#include "osbl_setup_mpu.h"
#include "flash.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION  OSBL_FLASH_DEV_NAND_SETUP_NAND_MPU

DESCRIPTION
  Configures the NAND MPU.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean osbl_flash_dev_nand_setup_nand_mpu
(
  bl_shared_data_type    *bl_shared_data
)
{
  struct fs_device_data *nand_if;


  /* Check parameters first */
  BL_VERIFY( ( bl_shared_data != NULL &&
               bl_shared_data->boot_dev_shared_data != NULL ),
             BL_ERR_NULL_PTR_PASSED );


  nand_if = boot_flash_dev_nand_get_nand_if();

  BL_VERIFY( nand_if != NULL, BL_ERR_NULL_PTR );
  
  osbl_setup_nand_mpu( nand_if->page_size( nand_if ),
                       nand_if->block_count( nand_if ),
                       bl_shared_data->boot_dev_shared_data->sys_partition_ptr );

  return TRUE;
} /* osbl_flash_dev_nand_setup_nand_mpu() */

