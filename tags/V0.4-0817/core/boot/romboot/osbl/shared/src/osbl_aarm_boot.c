/*=============================================================================

                 O E M  S e c o n d a r y  B o o t  L o a d e r
                              a A R M  B o o t 

GENERAL DESCRIPTION
  This file contains the OEM SBL processing for loading and booting
  the applications ARM.

EXTERNALIZED FUNCTIONS
  osbl_load_appsbl
  osbl_auth_appsbl
  osbl_create_aarm_partition_tbl
  osbl_create_aarm_bad_block_table
  osbl_boot_aarm

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_aarm_boot.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/09/07   ly      Create

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "miheader.h"
#include "smem.h"
/* MSG related macros are defined in flash_dmov_msg.h for flash driver
 * Alter the include sequence to resolve compiler warning about redefines
 */
#include "boot_flash_partition.h"
#include "osbl.h"
#include "osbl_msm.h"
#include "osbl_flash.h"
#include "osbl_aarm_boot.h"
#include "osbl_loader.h"
#include "osbl_error_handler.h"
#include "boot_auth_if.h"
#include "osbl_hw_init.h"
#include "mibbtb.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Size of the aARM exception vector table in bytes. */
#define SIZE_OF_AARM_VECTOR_TABLE_BYTES  64

static mi_boot_image_header_type appsbl_image_header;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  OSBL_LOAD_APPSBL

DESCRIPTION
  This function loads the applications ARM boot loader to its
  destination RAM.  It may load the aARM from flash or from data
  contained within the OEM SBL image depending on OEM security
  requirements.  
  
DEPENDENCIES
  aARM boot loader image has its exception vector table as the
  first 32 bytes in the image.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_load_appsbl( bl_shared_data_type *bl_shared_data )
{
  uint32 apps_image_start_addr = NULL;

  OSBL_VERIFY( ( bl_shared_data != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /* Load the aARM boot loader header and image from flash to RAM. */
  osbl_load_header_and_image( APPSBL_IMG, &appsbl_image_header );

  apps_image_start_addr = (uint32) appsbl_image_header.image_dest_ptr;

  (void)boot_clobber_add_global_protection_region( appsbl_image_header.image_dest_ptr,
                                                   SCL_APPS_BOOT_SIZE );
  
  /* Copy the aARM boot loader vector table to address 0x0 if the boot
     loader was not already loaded to address 0x0. The aARM boot
     loader image always has the vector table in the first 64 bytes
     of the image. */
  if ( apps_image_start_addr != 0x0 )
  {
    /* Initialize the reset vector for L4 with kernel entry point and
       the remaining vectors with an infinite loop. */ 

    /* Reset vector initialization is equivalent to doing a PC relative
       load of the PC register with the start address of the aARM image:
         address 0x00000000:     ldr pc, [pc, #0x18]
         address 0x00000020:     apps_image_start_addr */    
    uint32 *vector_ptr = (uint32*) 0x00000020;
    
    *vector_ptr = apps_image_start_addr;

    /* Equivalent to for ( --vector_ptr; vector_ptr; vector_ptr-- )
     * Lint complains about possible usage of null pointer
     */
    for ( --vector_ptr; vector_ptr; vector_ptr-- )
    {
      *vector_ptr = 0xEAFFFFFE;         /* Infinite loop */
    }
    *vector_ptr  = 0xE59FF018;          /* ldr pc, [pc, #0x18] */
  }
} /* osbl_load_appsbl() */

/*===========================================================================

FUNCTION  OSBL_AUTH_APPSBL

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_auth_appsbl( bl_shared_data_type *bl_shared_data )
{
  /* Don't try to authenticate the APPSBL if its not required */
  if ( osbl_target_is_appsbl_auth_required() )
  {
    /* Authenticate appsbl if necessary */
    if ( ! boot_auth_authenticate( &appsbl_image_header ) )
    {
      OSBL_ERR_FATAL( BL_ERR_OSBL );
    }
  }
  
} /* osbl_auth_appsbl() */

/*===========================================================================

FUNCTION  OSBL_CREATE_AARM_PARTITION_TBL

DESCRIPTION
  This function creates the aARM partition table from the mARM
  based partition table and places it in shared memory.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_create_aarm_partition_tbl( bl_shared_data_type *bl_shared_data )
{
  /* Pointer to partition table entry. */
  flash_partentry_ptr_type entry_ptr;
  flash_partable_t smem_aarm_partition_table;
  flash_partable_t sys_partition_table;
  uint32 numparts = 0;

  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->boot_dev_shared_data != NULL ),
               BL_ERR_NULL_PTR_PASSED );
  
  sys_partition_table =
    bl_shared_data->boot_dev_shared_data->sys_partition_ptr;

  if ( sys_partition_table != NULL ) 
  {
    smem_aarm_partition_table = smem_alloc( SMEM_AARM_PARTITION_TABLE,
                                          sizeof(*smem_aarm_partition_table) );
    OSBL_VERIFY( smem_aarm_partition_table != NULL, BL_ERR_NULL_PTR_PASSED );

    /* Initialize the aARM partition table header in shared memory. */
    smem_aarm_partition_table->magic1  = sys_partition_table->magic1;
    smem_aarm_partition_table->magic2  = sys_partition_table->magic2;  
    smem_aarm_partition_table->version = sys_partition_table->version;

    /* Read aARM image header from system partition table and
       place in shared memory aARM partition table. */
    entry_ptr = boot_flash_find_partition_entry( sys_partition_table,
                                                 MIBIB_APPS_PARTI_NAME );

    /* If the entry does not exist within the table, fail the boot. */ 
    OSBL_VERIFY( entry_ptr != NULL, BL_ERR_NULL_PTR_PASSED );
    
    memcpy( (void *) &smem_aarm_partition_table->part_entry[numparts++],  
            (void *) entry_ptr,
            sizeof( *entry_ptr ) );

    /* Read aARM EFS partition from system partition table and initialize
       shared memory aARM partition table. */
    entry_ptr = boot_flash_find_partition_entry( sys_partition_table,
                                                 MIBIB_EFS2_APPS_PARTI_NAME );

    /* If the entry does not exist within the table, fail the boot. */ 
    OSBL_VERIFY( entry_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

    memcpy( (void *) &smem_aarm_partition_table->part_entry[numparts++],
            (void *) entry_ptr,
            sizeof( *entry_ptr ) );

#ifdef FEATURE_BOOT_IMAGE_WINMOB
    /* Read FOTA partition from system partition table and initialize
       shared memory aARM partition table. */
    entry_ptr = boot_flash_find_partition_entry( sys_partition_table,
                                                 MIBIB_FOTA_PARTI_NAME );

    /* If the entry does not exist within the table, fail the boot. */ 
    OSBL_VERIFY( entry_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

    memcpy( (void *) &smem_aarm_partition_table->part_entry[numparts++],
            (void *) entry_ptr,
            sizeof( *entry_ptr ) );
#endif /* FEATURE_BOOT_IMAGE_WINMOB */

    /* Store the total number of partition entries in shared memory. */
    smem_aarm_partition_table->numparts = numparts;
  }
} /* create_aarm_partition_tbl() */

/*===========================================================================

FUNCTION  OSBL_CREATE_AARM_BAD_BLOCK_TABLE

DESCRIPTION
  This function copies the NAND bad block table to shared memory for
  use by the aARM boot loader.  The bad block table is only valid for
  a NAND boot.  This function does nothing if it is not a NAND boot.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_create_aarm_bad_block_table( bl_shared_data_type *bl_shared_data )
{
  uint32 tbl_size;
  struct bad_block_table *smem_aarm_bad_block_table;
  struct bad_block_table *nand_bad_block_table;

  OSBL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED );

  nand_bad_block_table =
    bl_shared_data->boot_dev_shared_data->nand.bad_block_table_ptr;

  if ( nand_bad_block_table != NULL )
  {
    /* Booting from NAND, so bad block table exists. */
    
    smem_aarm_bad_block_table = (struct bad_block_table*) 
      smem_alloc( SMEM_AARM_BAD_BLOCK_TABLE, sizeof(struct bad_block_table) );
    OSBL_VERIFY( smem_aarm_bad_block_table != NULL, BL_ERR_NULL_PTR_PASSED );
    
    /* Calculate size of bad block table based on number of bad block
       entries in table. */
    tbl_size = sizeof( *smem_aarm_bad_block_table ) -
               sizeof( smem_aarm_bad_block_table->bad_block ) +
               (nand_bad_block_table->num_entries *
                sizeof( smem_aarm_bad_block_table->bad_block[0] )); 

    /* Copy bad block table to shared memory. */
    memcpy( (void *) smem_aarm_bad_block_table,
            (void *) nand_bad_block_table,
            tbl_size );
  }
} /* osbl_create_aarm_bad_block_table() */

/*===========================================================================

FUNCTION  OSBL_BOOT_AARM

DESCRIPTION
  This function releases the aARM from reset to boot the aARM.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  Initiates execution of the aARM.

===========================================================================*/
void osbl_boot_aarm( bl_shared_data_type *bl_shared_data )
{
  osbl_hw_boot_aarm();
} /* osbl_boot_aarm() */

