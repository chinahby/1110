/*=============================================================================

                             B o o t   L o a d e r
          N O R  +  N A N D  E F S  B o o t  D e v i c e  M o d u l e

GENERAL DESCRIPTION
  Contains the implementation for the NOR + NAND EFS boot device module.
  
EXTERNALIZED FUNCTIONS
  boot_flash_dev_nor_nand_efs_init_and_register

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_dev_nor_nand_efs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/07/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_dev_nor_nand_efs.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Definition of private NOR + NAND EFS device data structure */
typedef struct boot_flash_dev_nor_nand_efs_type
{
  /*----------------------------------- 
   * Generic boot flash device interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure.  Provides 
   * interface to generic boot flash device flash routines.
   * NAND-specific flash device routines and variables follow
   * the generic interface and extend the interface.
   */
  boot_flash_dev_if_type pub_if;
  
} boot_flash_dev_nor_nand_efs_type;

/* The private data structure */
static boot_flash_dev_nor_nand_efs_type dev_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  DEV_NOR_NAND_EFS_OPEN_IMAGE

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
static boot_flash_trans_if_type* dev_nor_nand_efs_open_image
(
  image_type image_id                  /* Image type to open */
)
{
  /* Redirect to the NOR device */
  return boot_flash_dev_nor_open_image( image_id );

} /* dev_nor_nand_efs_open_image() */

/*=========================================================================

FUNCTION  DEV_NOR_NAND_EFS_OPEN_PARTITION

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
static boot_flash_trans_if_type* dev_nor_nand_efs_open_partition
(
  const char *parti_name                  /* partition to open */
)
{
  /* Redirect to the NOR device */
  return boot_flash_dev_nor_open_partition( parti_name );

} /* dev_nor_nand_efs_open_partition() */

/*=========================================================================

FUNCTION  DEV_NOR_NAND_EFS_CLOSE_IMAGE

DESCRIPTION
  Close an instance of the boot translation layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dev_nor_nand_efs_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  /* Redirect to the NOR device */
  boot_flash_dev_nor_close_image( p_trans_if );

} /* dev_nor_nand_efs_close_image() */

/*=========================================================================

FUNCTION  DEV_NOR_NAND_EFS_CLOSE_PARTITION

DESCRIPTION
  Close an instance of the boot translation layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dev_nor_nand_efs_close_partition
(
  boot_flash_trans_if_type **p_trans_if
)
{
  /* Redirect to the NOR device */
  boot_flash_dev_nor_close_partition( p_trans_if );

} /* dev_nor_nand_efs_close_partition() */

/*===========================================================================

FUNCTION  DEV_NOR_NAND_EFS_LOAD_MIBIB_HEADER

DESCRIPTION
  Function to load an image header from the MIBIB of the flash device
  based on the given image type.  This function returns false
  if the device does not have headers in the MIBIB area.

DEPENDENCIES

RETURN VALUE
  TRUE if the operation was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean dev_nor_nand_efs_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *image_header_ptr  /* Pointer to image header */
)
{
  /* Redirect to the NOR device */
  return boot_flash_dev_nor_load_mibib_header( image_id, image_header_ptr );
  
} /* dev_nor_nand_efs_load_mibib_header() */

/*=========================================================================

FUNCTION  DEV_NOR_NAND_EFS_SETUP_MPU

DESCRIPTION
  Setup the MPU for the boot device to enable a trusted environment.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean dev_nor_nand_efs_setup_mpu
(
  struct bl_shared_data_type *bl_shared_data
)
{
  boolean success1, success2;

  /* Redirect to both NOR and NAND MPU setup */
  success1 = boot_flash_dev_nor_setup_mpu( bl_shared_data );
  success2 = boot_flash_dev_nand_setup_mpu( bl_shared_data );

  return (success1 && success2);
} /* dev_nor_nand_efs_setup_mpu() */

/*=========================================================================

FUNCTION  DEV_NOR_NAND_EFS_SHARE_DATA

DESCRIPTION
  Exports data shared by the boot device.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean dev_nor_nand_efs_share_data
(
  struct bl_shared_data_type *bl_shared_data
)
{
  boolean success1, success2;

  /* Redirect to both NOR and NAND share data handlers */
  success1 = boot_flash_dev_nor_share_data( bl_shared_data );
  success2 = boot_flash_dev_nand_share_data( bl_shared_data );

  return (success1 && success2);
} /* dev_nor_nand_efs_share_data() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_NOR_NAND_EFS_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize the boot NOR + NAND EFS flash device module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_flash_dev_nor_nand_efs_init_and_register
(
  boot_flash_type                    flash_type,
  boot_flash_dev_nand_init_type      nand_init,
  boot_flash_trans_factory_init_type nand_trans_factory_init,
  boot_flash_dev_nor_init_type       nor_init,
  boot_flash_trans_factory_init_type nor_trans_factory_init,
  struct bl_shared_data_type        *bl_shared_data
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_DEV_NOR_NAND_EFS_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_DEV_IF_VERSION,
                         BOOT_FLASH_DEV_NOR_NAND_EFS_VERSION )
  };
  static boot_flash_dev_vtbl_type vtbl =
  {
    dev_nor_nand_efs_open_image,
    dev_nor_nand_efs_open_partition,
    dev_nor_nand_efs_close_image,
    dev_nor_nand_efs_close_partition,
    dev_nor_nand_efs_load_mibib_header,
    dev_nor_nand_efs_share_data,
    dev_nor_nand_efs_setup_mpu
  };


  /* First verify our input parameters */
  BL_VERIFY( ( bl_shared_data != NULL &&
               nand_init != NULL &&
               nor_init != NULL &&
               nand_trans_factory_init != NULL &&
               nor_trans_factory_init != NULL ),
             BL_ERR_NULL_PTR_PASSED );

  /* Setup the module version structure and virtual function table */
  dev_priv.pub_if.vsn = &vsn;
  dev_priv.pub_if.vtbl = &vtbl;

  if ( flash_type == NOR_FLASH_NAND_EFS )
  {
    /* Initialize the NAND boot device */
    nand_init( NAND_FLASH, nand_trans_factory_init, bl_shared_data );
  }
  else
  {
    BL_ERR_FATAL( BL_ERR_FLASH_TYPE_NONE );
  }

  /* Initialize the NOR boot device */
  nor_init( NOR_FLASH, nor_trans_factory_init, bl_shared_data );

  /* Finaly, register the singleton */
  boot_flash_dev_if_register_singleton( &dev_priv.pub_if );

} /* boot_flash_dev_nor_nand_efs_init_and_register() */

