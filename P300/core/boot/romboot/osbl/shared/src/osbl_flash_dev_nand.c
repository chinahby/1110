/*=============================================================================

                OS  S e c o n d a r y   B o o t   L o a d e r
                 N A N D  F l a s h  D e v i c e  M o d u l e

GENERAL DESCRIPTION
  This header file contains the implementation for the OSBL NAND flash
  device module.
  
EXTERNALIZED FUNCTIONS
  bl_flash_nand_load_partition_table
  bl_flash_nand_load_bad_block_table
  osbl_flash_dev_nand_share_data
  osbl_flash_dev_nand_install_error_callback

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_flash_dev_nand.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/07/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "osbl.h"
#include "osbl_error_handler.h"
#include "osbl_flash.h"
#include "osbl_flash_dev_nand.h"
#include "boot_flash_partition.h"
#include "boot_dbl_if.h"
#include "osbl_target.h"
#include "miboot.h"
#include "miparti.h"
#include "mibbtb.h"
#include "flash.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Structure for storage of NAND partition table. */
static struct flash_partition_table mibib_partition_tbl;

/* Bad Block table structure, and pointer to table */
static struct bad_block_table bbtb;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  BL_FLASH_NAND_LOAD_PARTITION_TABLE

DESCRIPTION
  This function copies the NAND flash partition table from the DBL.

DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None

===========================================================================*/
flash_partable_t bl_flash_nand_load_partition_table
(
  bl_shared_data_type *bl_shared_data
)
{
  BL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Copy partition table to buffer passed in. */
  memcpy( (void *) &mibib_partition_tbl,
          (void *) bl_shared_data->dbl_shared_data->sys_partition_pointer,
          sizeof( struct flash_partition_table ) );

  /* Fail boot if valid partition table does not exist. */
  BL_VERIFY( (mibib_partition_tbl.magic1 == FLASH_PART_MAGIC1) &&
             (mibib_partition_tbl.magic2 == FLASH_PART_MAGIC2) &&
             (mibib_partition_tbl.version == FLASH_PARTITION_VERSION),
             BL_ERR_PARTI_TABLE_NOT_FOUND );

  return &mibib_partition_tbl;

} /* bl_flash_nand_load_partition_table() */

/*===========================================================================

FUNCTION BL_FLASH_NAND_LOAD_BAD_BLOCK_TABLE

DESCRIPTION
  This function reads in the bad block list from NAND if the list already
  exists, otherwise it creates a bad block list and stores it in NAND.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
struct bad_block_table* bl_flash_nand_load_bad_block_table
(
  flash_partable_t sys_partition_ptr
)
{
  struct fs_device_data *nand_if;
  uint32 total_size = 0;
  uint32 byte_count = 0;
  uint32 bad_block_list_firstpage = 0;
  uint32 good_blocks = 0;
  uint32 current_block = 0;
  flash_partentry_ptr_type partentry_ptr; /* Holds the partition infomation */
  uint32 executable_start_block = 0; /* The beginning of the AMSS image */
  uint32 executable_end_block = 0;   /* The end of the last image to check */
  uint32 page_size_bytes = 0;
  uint32 block_count = 0;
  boolean create_table = FALSE;
  int i = 0;

  OSBL_VERIFY( sys_partition_ptr != NULL, BL_ERR_NULL_PTR_PASSED );

  nand_if = boot_flash_dev_nand_get_nand_if();

  BL_VERIFY( nand_if != NULL, BL_ERR_NULL_PTR );

  /* Make sure our max table can fit into one block */
  BL_VERIFY( MAX_BBTB_PAGES <= nand_if->block_size( nand_if ),
             BL_ERR_MAX_BBTB_TOO_BIG );

  /* Retrieve NAND parameters */
  page_size_bytes = nand_if->page_size( nand_if );
  block_count = nand_if->block_count( nand_if );

  /* Count from block 0 to find the BBTB_BLOCKth - 1 non bad block as 7500
     does not have a PBL so blocks are shifted down by one. */
  while( (good_blocks <= MI_BLOCK_BBTB ) &&
         (current_block < block_count) )
  {
    if( !nand_if->bad_block_check( nand_if, current_block ) )
    {
      good_blocks++;
    }
    current_block++;
  }

  /* Adjust current_block to last checked block */
  current_block--;

  BL_VERIFY( current_block < block_count, BL_ERR_OTHERS );

  /* Point to first page in found block where Bad Block List is */
  bad_block_list_firstpage =
    current_block * nand_if->block_size( nand_if );

  /* Now current_block is pointing to BadBlockTable */
  /* Three possible conditions:
   *  1. Current page is erased -- need to create new table
   *  2. Current page is not erased by does not contain BBTB --
   *     return error (table corrupt)
   *  3. Current page is not erased and contains BBTB -- need to read in BBTB
   */
  if( !nand_if->is_erased( nand_if,
                           bad_block_list_firstpage ) )
  {
    /* The Bad Block Table is already Programmed. Read the Bad Block Table and
    ** check for the magic numbers.
    */
    /* Read more pages. */
    if ( nand_if->read_page( nand_if, bad_block_list_firstpage, &bbtb ) != 
         FS_DEVICE_DONE )
    {
      OSBL_ERR_FATAL( BL_ERR_NAND_READ_FAIL );
    }

    if ((bbtb.magic1  == BBTB_MAGIC1) &&
        (bbtb.magic2  == BBTB_MAGIC2) &&
        (bbtb.version == BBTB_VERSION))
    {
      total_size = sizeof(uint32)*bbtb.num_entries + SIZEOF_BBTB_INFO;

      /* We need to check the number of bad block entries to avoid 
         a potential buffer overflow */
      BL_VERIFY(bbtb.num_entries <= MAX_BAD_BLOCKS, BL_ERR_TOO_MANY_BB );

      /* Copy enough pages here for the entire bad block list structure */
      for( i = 1, byte_count = page_size_bytes;
           byte_count < total_size;
          i++, byte_count += page_size_bytes )
      {
        if( nand_if->read_page( nand_if,
                               ( bad_block_list_firstpage + i ),
                               ( ( (uint8*) &bbtb ) +
                                 ( page_size_bytes*i) ) ) != 
            FS_DEVICE_DONE )
        {
          OSBL_ERR_FATAL( BL_ERR_NAND_READ_FAIL );
        }
      }
    }
    else
    {
      nand_if->erase_block( nand_if, current_block );
      create_table = TRUE;
    }
  }
  else
  {
    create_table = TRUE;
  }

  if ( create_table )
  {
    /* BBTB struct does not contain space for multiple entires in the
     * bad_block table.  Since we will be writing this to memory, we will use
     * the osbl_page_buf as a bad_block_table struct, and keep track to
     * ensure we have not gone over the size contraints
     */
    bbtb.magic1      = BBTB_MAGIC1;
    bbtb.magic2      = BBTB_MAGIC2;
    bbtb.version     = BBTB_VERSION;
    bbtb.num_entries = 0;

    /* To save time, we will only scan blocks in the partitions where the AMSS,
    **  APPS boot loader and APPS image reside (if applicable).
    **
    ** The starting block is the beginning of the AMSS partition.
    **
    ** The ending block is the end of a target-specific partition.
    */
    partentry_ptr = boot_flash_find_partition_entry( sys_partition_ptr,
                                                     MIBIB_AMSS_PARTI_NAME );
    executable_start_block = partentry_ptr->offset;

    partentry_ptr = boot_flash_find_partition_entry(sys_partition_ptr,
                             OSBL_FLASH_NAND_LAST_BAD_BLOCK_PARTI_NAME );
    executable_end_block = partentry_ptr->offset + partentry_ptr->length;
    
    BL_VERIFY( executable_end_block <= block_count,
               BL_ERR_IMG_SIZE_TOO_BIG );

    for(i=executable_start_block; i<executable_end_block; i++)
    {
      /* Scan all the blocks and store the block number if it is found bad,
      ** also keep track of number of bad blocks found.
      */
      if( nand_if->bad_block_check( nand_if, i ) )
      {
        BL_VERIFY(bbtb.num_entries < MAX_BAD_BLOCKS, BL_ERR_TOO_MANY_BB );
        bbtb.bad_block[bbtb.num_entries++] = i;
      }
    }

    total_size = sizeof(uint32)*bbtb.num_entries + SIZEOF_BBTB_INFO;
    for( i = 0, byte_count = 0;
         byte_count < total_size;
         i++, byte_count += page_size_bytes )
    {
      /* Reset the watchdog timer. */
      OSBL_KICK_WATCHDOG();

      if( nand_if->write_page( nand_if,
                               ( bad_block_list_firstpage + i ),
                               ( ( (uint8*) &bbtb ) +
                                 ( page_size_bytes * i ) ) ) !=
          FS_DEVICE_DONE )
      {
        OSBL_ERR_FATAL( BL_ERR_NAND_WRITE_FAIL );
      }
    }
  }

  return &bbtb;

} /* bl_flash_nand_load_bad_block_table() */


/*===========================================================================

FUNCTION OSBL_ERASE_BAD_BLOCK_TABLE

DESCRIPTION
  This function erases the bad block table in NAND flash. It is called when
  there is an error in the boot process to make sure that the table is
  regenerated on the next boot. If there is an error will return anyway
  because we are already in the error handler.

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
static void osbl_erase_bad_block_table_callback
(
  void *data_ptr
)
{
  uint32 bad_block_list_firstpage = 0;
  uint32 current_block = MI_BLOCK_BBTB;
  uint32 block_count = 0;
  struct fs_device_data *nand_if;

  nand_if = boot_flash_dev_nand_get_nand_if();

  if ( nand_if == NULL )
  {
    return; /* Flash driver was not configured and could have caused
               the error, so we cannot erase the table. */
  }
  
  block_count = nand_if->block_count( nand_if );
  
  /* Count from block MI_BLOCK_BBTB-1 to find the BBTB_BLOCKth - 1 non bad block 
     as 7500 does not have a PBL, so blocks are shifted down by one. */
  while( current_block < block_count )
  {
    if( !nand_if->bad_block_check( nand_if, current_block ) )
    {
      break;
    }
    current_block++;
  }

  /* Make sure we have not exceeded the device block count */
  if ( current_block >= block_count )
  {
    return;
  }

  /* Point to first page in found block where Bad Block List is */
  bad_block_list_firstpage = current_block * 
                             nand_if->block_size( nand_if );

  /* Now current_block is pointing to BadBlockTable */

  if( !nand_if->is_erased( nand_if, bad_block_list_firstpage ) )
  {
    /* Erase the block */
    nand_if->erase_block( nand_if, current_block );
  }

} /* osbl_erase_bad_block_table_callback() */

/*===========================================================================

FUNCTION  OSBL_FLASH_DEV_NAND_SHARE_DATA

DESCRIPTION
  Function to share common OSBL NAND flash data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean osbl_flash_dev_nand_share_data
(
  struct bl_shared_data_type *bl_shared_data
)
{
  /* Check parameters first */
  BL_VERIFY( ( bl_shared_data != NULL &&
               bl_shared_data->boot_dev_shared_data != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  bl_shared_data->boot_dev_shared_data->nand.bad_block_table_ptr = &bbtb;
  bl_shared_data->boot_dev_shared_data->sys_partition_ptr = &mibib_partition_tbl;

  return TRUE;
} /* osbl_flash_dev_nand_share_data() */

/*===========================================================================

FUNCTION  OSBL_FLASH_DEV_NAND_INSTALL_ERROR_CALLBACK

DESCRIPTION
  Installs OSBL NAND flash error handler callback, to erase the
  bad block table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_flash_dev_nand_install_error_callback( void )
{
  static bl_error_callback_node_type err_cb_node;

  bl_err_if.install_callback( osbl_erase_bad_block_table_callback,
                              NULL,
                              &err_cb_node ); 

} /* osbl_flash_dev_nand_install_error_callback() */
