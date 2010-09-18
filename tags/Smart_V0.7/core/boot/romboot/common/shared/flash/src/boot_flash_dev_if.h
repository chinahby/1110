#ifndef BOOT_FLASH_DEV_IF_H
#define BOOT_FLASH_DEV_IF_H

/*===========================================================================

                           B o o t   L o a d e r
                 F l a s h  D e v i c e  I n t e r f a c e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to the
  boot device module (e.g. functionality common between NAND and NOR devices).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_dev_if.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_module_base.h"
#include "boot_singleton.h"
#include "boot_flash_trans_if.h"
#include "miheader.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_DEV_IF_VERSION 1


/* Forward declarations */
struct bl_shared_data_type;
struct boot_flash_dev_if_type;


/* Definition of the boot device module virtual table structure */
typedef struct boot_flash_dev_vtbl_type
{
  /*=========================================================================

  FUNCTION POINTER  OPEN_IMAGE

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
  boot_flash_trans_if_type * (*open_image)
  (
    image_type image_id                     /* Image type to open */
  );

  /*=========================================================================

  FUNCTION POINTER  OPEN_PARTITION

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
  boot_flash_trans_if_type * (*open_partition)
  (
    const char *parti_name                  /* partition to open */
  );

  /*=========================================================================

  FUNCTION POINTER  CLOSE_IMAGE

  DESCRIPTION
    Close an instance of the boot translation layer.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*close_image)
  (
    boot_flash_trans_if_type **p_trans_if
  );

  /*=========================================================================

  FUNCTION POINTER  CLOSE_PARTITION

  DESCRIPTION
    Close an instance of the boot translation layer.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*close_partition)
  (
    boot_flash_trans_if_type **p_trans_if
  );

  /*=========================================================================

  FUNCTION POINTER  LOAD_MIBIB_HEADER

  DESCRIPTION
    Function to load an image header from the MIBIB of the flash device
    based on the given image type.  This function returns false
    if the device does not have headers in the MIBIB area.

  DEPENDENCIES
    None

  RETURN VALUE
    TRUE if the operation was successful, FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*load_mibib_header)
  (
    image_type image_id,
    mi_boot_image_header_type *header_ptr   /* Pointer to image header */
  );

  /*=========================================================================

  FUNCTION POINTER  SHARE_DATA

  DESCRIPTION
    Exports data shared by the boot device.

  DEPENDENCIES
    None

  RETURN VALUE
    TRUE if the operation was successful, FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*share_data)
  (
    struct bl_shared_data_type    *bl_shared_data
  );

  /*=========================================================================

  FUNCTION POINTER  SETUP_MPU

  DESCRIPTION
    Setup the MPU for the boot device to enable a trusted environment.

  DEPENDENCIES
    None

  RETURN VALUE
    TRUE if the operation was successful, FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*setup_mpu)
  (
    struct bl_shared_data_type    *bl_shared_data
  );

} boot_flash_dev_vtbl_type;


/* Definition of public interface to generic boot device module */
typedef struct boot_flash_dev_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_dev_vtbl_type *vtbl;

} boot_flash_dev_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Must declare the singleton before we can reference the get_singleton()
 * functions in the below inline functions */
BOOT_SINGLETON_DECLARE(boot_flash_dev_if);


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_OPEN_IMAGE

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
__inline boot_flash_trans_if_type* boot_flash_dev_open_image
(
  image_type image_id                     /* Image type to open */
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->open_image( image_id );
} /* boot_flash_dev_open_image() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_OPEN_PARTITION

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
__inline boot_flash_trans_if_type* boot_flash_dev_open_partition
(
  const char *parti_name                     /* partition to open */
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->open_partition( parti_name );
} /* boot_flash_dev_open_partition() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_CLOSE_IMAGE

DESCRIPTION
  Close an instance of the boot translation layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void boot_flash_dev_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_if_get_singleton()->vtbl->close_image( p_trans_if );
} /* boot_flash_dev_close_image() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_CLOSE_PARTITION

DESCRIPTION
  Close an instance of the boot translation layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void boot_flash_dev_close_partition
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_dev_if_get_singleton()->vtbl->close_partition( p_trans_if );
} /* boot_flash_dev_close_partition() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_LOAD_MIBIB_HEADER

DESCRIPTION
  Function to load an image header from the MIBIB of the flash device
  based on the given image type.  This function returns false
  if the device does not have headers in the MIBIB area.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_flash_dev_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr   /* Pointer to image header */
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->load_mibib_header( image_id, header_ptr );
} /* boot_flash_dev_load_mibib_header() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_SHARE_DATA

DESCRIPTION
  Exports data shared by the boot device.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_flash_dev_share_data
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->share_data( bl_shared_data );
} /* boot_flash_dev_share_data() */

/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_SETUP_MPU

DESCRIPTION
  Setup the MPU for the boot device to enable a trusted environment.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_flash_dev_setup_mpu
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return boot_flash_dev_if_get_singleton()->vtbl->setup_mpu( bl_shared_data );
} /* boot_flash_dev_setup_mpu() */

/*=========================================================================

FUNCTION  OSBL_FLASH_DEV_DEFAULT_STUB

DESCRIPTION
  Default no-op stub for various boot device functionality.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_flash_dev_default_stub
(
  struct bl_shared_data_type    *bl_shared_data
)
{
  return TRUE;
} /* boot_flash_dev_default_stub() */

#endif /* BOOT_FLASH_DEV_IF_H */

