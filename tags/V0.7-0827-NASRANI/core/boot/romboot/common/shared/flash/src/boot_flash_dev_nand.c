/*=============================================================================

                            B o o t   L o a d e r
                 N A N D  F l a s h  D e v i c e  M o d u l e

GENERAL DESCRIPTION
  Contains the implementation for the boot NAND flash device module.

EXTERNALIZED FUNCTIONS
  boot_flash_dev_nand_init_and_register

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007, 2008 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_dev_nand.c#3 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/07/08   MJS     Use flash_open() API.
12/07/07   MJS     Initial revision, ported from MDM7800.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_error_if.h"
#include "boot_flash_dev_nand.h"
#include "boot_flash_trans_if.h"
#include "boot_flash_partition.h"
#include "boot_util.h"
#include "miprogressive.h"
#include "miparti.h"
#include <string.h>

#include "flash.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/* Definition of private flash NAND device data structure */
typedef struct boot_flash_dev_nand_type
{
  /*-----------------------------------
   * Public module interface
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_dev_nand_if_type       pub_if;

  /* NAND device public structure pointer used for all operations */
  fs_device_t                       nand_if;

  /* Pointer to the partition table */
  flash_partable_t                  partable_ptr;

  /* Pointer to NAND flash bad block table. */
  struct bad_block_table           *bad_block_tbl_ptr;

  /* Pointer to the boot translation layer interface */
  boot_flash_trans_factory_if_type *trans_fact_if;

  /* indicates block size in bytes */
  uint32                            block_size_bytes_shift_bits;

} boot_flash_dev_nand_type;


/* The NAND private data structure */
static boot_flash_dev_nand_type dev_priv;

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

FUNCTION  BOOT_FLASH_DEV_NAND_GET_PARTI_ENTRY_PTR

DESCRIPTION
  Function to find the entry in the partition table that corresponds to
  the given image type.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the partition table entry.

SIDE EFFECTS
  None

===========================================================================*/
static flash_partentry_ptr_type dev_nand_get_parti_entry_ptr
(
  image_type image_id                   /* The image type to find */
)
{
  char *name = NULL;

  /* Check parameters first */
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_WRG_IMGHD_REQUESTED );

  /* Set the partition name based on the image type */
#if defined(MIBIB_AMSS_PARTI_NAME)
  if ( image_id == AMSS_IMG )
  {
    name = MIBIB_AMSS_PARTI_NAME;
  }
  else
#endif /* MIBIB_AMSS_PARTI_NAME */
#if defined(MIBIB_APPS_PARTI_NAME)
  if ( image_id == APPS_IMG )
  {
    name = MIBIB_APPS_PARTI_NAME;
  }
  else
#endif /* MIBIB_APPS_PARTI_NAME */
#if defined(MIBIB_APPSBL_PARTI_NAME)
  if ( image_id == APPSBL_IMG )
  {
    name = MIBIB_APPSBL_PARTI_NAME;
  }
  else
#endif /* MIBIB_APPSBL_PARTI_NAME */
#if defined(MIBIB_OSBL_PARTI_NAME)
  if ( image_id == OSBL_IMG )
  {
    name = MIBIB_OSBL_PARTI_NAME;
  }
  else
#endif /* MIBIB_OSBL_PARTI_NAME */
#if defined(MIBIB_DSP1_PARTI_NAME)
  if ( image_id == DSP1_IMG )
  {
    name = MIBIB_DSP1_PARTI_NAME;
  }
  else
#endif /* MIBIB_DSP1_PARTI_NAME */
#if defined(MIBIB_DSP2_PARTI_NAME)
  if ( image_id == DSP2_IMG )
  {
    name = MIBIB_DSP2_PARTI_NAME;
  }
  else
#endif /* MIBIB_DSP2_PARTI_NAME */
  {
    /* We don't load the other type of image */
    BL_ERR_FATAL( BL_ERR_WRG_IMGHD_REQUESTED );
  }

  /* Find the entry in the partition table based on its name. */
  return boot_flash_find_partition_entry( dev_priv.partable_ptr, name );

} /* dev_nand_get_parti_entry_ptr() */

/*=========================================================================

FUNCTION  DEV_NAND_OPEN_IMAGE

DESCRIPTION
  Function to initialize an instance of the boot translation module, using
  information obtained from the underlying boot device.  The base address
  and region size used to initialize the translation layer are obtained
  from the boot device (e.g. partition table) based on the given image type.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the boot translation layer public interface.

SIDE EFFECTS
  None

===========================================================================*/
static boot_flash_trans_if_type* dev_nand_open_image
(
  image_type image_id                  /* Image type to open */
)
{
  /* Pointer to partition table entry */
  flash_partentry_ptr_type parti_entry_ptr;

  /* First verify our input parameters */
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_WRG_IMGHD_REQUESTED );

  /* Look up the partition table entry corresponding to the image type */
  parti_entry_ptr = dev_nand_get_parti_entry_ptr( image_id );

  /* If the entry does not exist within the table, fail the boot. */
  BL_VERIFY( parti_entry_ptr != NULL, BL_ERR_PARTI_ENTRY_NOT_FOUND );

  /* Initialize the translation interface based on the partition
     table offset and length */
  return boot_flash_trans_factory_open( dev_priv.trans_fact_if,
                                        ( parti_entry_ptr->offset <<
                                          dev_priv.block_size_bytes_shift_bits ),
                                        ( parti_entry_ptr->length <<
                                          dev_priv.block_size_bytes_shift_bits ) );

} /* dev_nand_open_image() */

/*=========================================================================

FUNCTION  DEV_NAND_OPEN_PARTITION

DESCRIPTION
  Function to initialize an instance of the boot translation module, using
  information obtained from the underlying boot device.  The base address
  and region size used to initialize the translation layer are obtained
  from the boot device (e.g. partition table) based on the given partition
  name.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the boot translation layer public interface.

SIDE EFFECTS
  None

===========================================================================*/
static boot_flash_trans_if_type* dev_nand_open_partition
(
  const char *parti_name                  /* partition to open */
)
{
  /* Pointer to partition table entry */
  flash_partentry_ptr_type parti_entry_ptr;

  /* First verify our input parameters */
  BL_VERIFY( parti_name != NULL, BL_ERR_WRG_IMGHD_REQUESTED );

  /* Look up the partition table entry corresponding to the partition name */
  parti_entry_ptr =
    boot_flash_find_partition_entry( dev_priv.partable_ptr, parti_name );

  /* If the entry does not exist within the table, fail the boot. */
  BL_VERIFY( parti_entry_ptr != NULL, BL_ERR_PARTI_ENTRY_NOT_FOUND );

  /* Initialize the translation interface based on the partition
     table offset and length */
  return boot_flash_trans_factory_open( dev_priv.trans_fact_if,
                                        ( parti_entry_ptr->offset <<
                                          dev_priv.block_size_bytes_shift_bits ),
                                        ( parti_entry_ptr->length <<
                                          dev_priv.block_size_bytes_shift_bits ) );

} /* dev_nand_open_partition() */

/*=========================================================================

FUNCTION  DEV_NAND_CLOSE_IMAGE

DESCRIPTION
  Close an instance of the boot translation layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dev_nand_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_trans_factory_close( dev_priv.trans_fact_if, p_trans_if );
} /* dev_nand_close_image() */

/*===========================================================================

FUNCTION  DEV_NAND_LOAD_MIBIB_HEADER

DESCRIPTION
  Function to load an image header from the MIBIB of the flash device
  based on the given image type passed in the image header.  This function
  returns false if the device does not have headers in the MIBIB area.

DEPENDENCIES

RETURN VALUE
  FALSE since SB2.0 NAND devices don't have image headers in the MIBIB

SIDE EFFECTS
  None

===========================================================================*/
static boolean dev_nand_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *image_header_ptr  /* Pointer to image header */
)
{
  /* SB2.0 NAND devices don't have image headers in the MIBIB */
  return FALSE;

} /* dev_nand_load_mibib_header() */

/*===========================================================================

FUNCTION POINTER  DEV_NAND_GET_NAND_IF

DESCRIPTION
  Returns a pointer to the NAND flash driver interface.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the NAND flash driver interface, NULL if an error occurred

SIDE EFFECTS
  None

===========================================================================*/
static struct fs_device_data *dev_nand_get_nand_if( void )
{
  return dev_priv.nand_if;

} /* dev_nand_get_nand_if() */

/*=========================================================================

FUNCTION  DEV_NAND_GET_LAST_GOOD_BLOCK_SKIP_BYTES

DESCRIPTION
  Returns the byte offset that should be skipped when reading from a good
  NAND block.  Used to account for magic numbers indicating that FSBL/OSBL
  blocks are good.

DEPENDENCIES
  None

RETURN VALUE
  Byte offset to skip into the last good block checked

SIDE EFFECTS
  None

===========================================================================*/
static uint32 dev_nand_get_last_good_block_skip_bytes( void )
{
  /* The good block skip bytes using the NAND flash driver will always be 0 */
  return 0;
} /* dev_nand_get_last_good_block_skip_bytes() */

/*===========================================================================

FUNCTION  DEV_NAND_GET_BAD_BLOCK_TABLE

DESCRIPTION
  Returns a pointer to the bad block table.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the bad block table, NULL if one doesn't exist

SIDE EFFECTS
  None

===========================================================================*/
static struct bad_block_table *dev_nand_get_bad_block_table( void )
{
  return dev_priv.bad_block_tbl_ptr;

} /* dev_nand_get_bad_block_table() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_NAND_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize the NAND flash device module

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void boot_flash_dev_nand_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_DEV_NAND_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_DEV_NAND_IF_VERSION,
                         BOOT_FLASH_DEV_NAND_VERSION )
  };
  static boot_flash_dev_nand_vtbl_type vtbl =
  {
    dev_nand_open_image,
    dev_nand_open_partition,
    dev_nand_close_image,
    dev_nand_close_image,
    dev_nand_load_mibib_header,
    boot_flash_dev_default_stub,
    boot_flash_dev_default_stub,
    dev_nand_get_nand_if,
    dev_nand_get_last_good_block_skip_bytes,
    dev_nand_get_bad_block_table
  };

  /* First verify our input parameters */
  BL_VERIFY( bl_shared_data != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Setup the module version structure and virtual function table */
  dev_priv.pub_if.vsn = &vsn;
  dev_priv.pub_if.vtbl = &vtbl;

  /* Open the NAND flash driver for access to the whole device.
   * which_flash parameter is hard coded to 0.
   */
#ifdef MIBIB_DEVICE_ALL_PARTI_NAME
  dev_priv.nand_if = flash_open( MIBIB_DEVICE_ALL_PARTI_NAME, 0 );
#else /* MIBIB_DEVICE_ALL_PARTI_NAME */
  dev_priv.nand_if = flash_open( NULL, 0 );
#endif /* MIBIB_DEVICE_ALL_PARTI_NAME */
  BL_VERIFY( ( dev_priv.nand_if != FS_NO_DEVICE ), BL_ERR_BOOT_FLASH_ACCESS );

  /* Calculate additional NAND parameters from data retrieved. */
  dev_priv.block_size_bytes_shift_bits =
    boot_get_bit_set_position( dev_priv.nand_if->block_size( dev_priv.nand_if ) ) +
    boot_get_bit_set_position( dev_priv.nand_if->page_size( dev_priv.nand_if ) );

  /* Register the singletons, do it here before we try to load the partition
   * table and bad block table */
  boot_flash_dev_if_register_singleton(
    boot_flash_dev_nand_cast_parent_if( &dev_priv.pub_if ) );
  boot_flash_dev_nand_if_register_singleton( &dev_priv.pub_if );

  /* Load the NAND partition table */
  dev_priv.partable_ptr =
    bl_flash_nand_load_partition_table( bl_shared_data );

  /* Load the NAND bad block table */
  dev_priv.bad_block_tbl_ptr =
    bl_flash_nand_load_bad_block_table( dev_priv.partable_ptr );

  /* Finally, initialize the translation layer factory */
  dev_priv.trans_fact_if = trans_factory_init();
  BL_VERIFY( dev_priv.trans_fact_if != NULL, BL_ERR_NULL_PTR );

} /* boot_flash_dev_nand_init_and_register() */

