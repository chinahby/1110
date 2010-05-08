/*=============================================================================

                           B o o t   L o a d e r
                  N O R  F l a s h  D e v i c e  M o d u l e

GENERAL DESCRIPTION
  Contains the implementation for the boot NOR flash device module.

EXTERNALIZED FUNCTIONS
  boot_flash_dev_nor_init_and_register

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_dev_nor.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/07/07   MJS     Initial revision, ported from MDM7800.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_dev_nor.h"
#include "boot_flash_trans_if.h"
#include "boot_error_if.h"
#include <string.h>

#ifdef USE_DM
#include "dmov_mem.h"
#endif /* USE_DM */

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Definition of private boot NOR device data structure */
typedef struct boot_flash_dev_nor_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_dev_nor_if_type pub_if;

  /* Pointer to the boot translation layer interface */
  boot_flash_trans_factory_if_type *trans_fact_if;

} boot_flash_dev_nor_type;


/* The NOR private data structure */
static boot_flash_dev_nor_type nor_dev_priv;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  DEV_NOR_READ

DESCRIPTION
  This function reads data from the NOR device and copies it to the 
  destination address in RAM.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the read was successful, otherwise FALSE.

SIDE EFFECTS
  None

===========================================================================*/
static boolean dev_nor_read_nor
(
  void   *ram_addr,                        /* Destination address in RAM */
  uint32  flash_addr,                      /* Logical flash source address */
  uint32  size                             /* Size to copy in bytes */
)
{

#ifdef USE_DM
      if( dmov_memcpy( ram_addr, (void*) flash_addr, size,
                       DMOV_MEM_CACHE_SRC_AND_DEST ) == NULL)
      {
        BL_ERR_FATAL( BL_ERR_DM_CPY_FAIL );
      }
#else /* USE_DM */
      memcpy( ram_addr, (void*) flash_addr, size );
#endif /* USE_DM */

      return TRUE;
} /* dev_nor_read_nor() */


/*=========================================================================

FUNCTION  DEV_NOR_OPEN_IMAGE

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
static boot_flash_trans_if_type* dev_nor_open_image
(
  image_type image_id                  /* Image type to open */
)
{
  uint32 image_src = 0;

  /* First verify the input parameters */
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_WRG_IMGHD_REQUESTED );

  /* Get the image header source based on the image type */
#ifdef MI_NOR_APPS_IMG_SRC_ADDR
  if ( image_id == APPS_IMG )
  {
    image_src = MI_NOR_APPS_IMG_SRC_ADDR;
  }
  else
#endif /* MI_NOR_APPS_IMG_SRC_ADDR */
#ifdef MI_NOR_OSBL_IMG_SRC_ADDR
  if ( image_id == OSBL_IMG )
  {
    image_src = MI_NOR_OSBL_IMG_SRC_ADDR;
  }
  else
#endif /* MI_NOR_OSBL_IMG_SRC_ADDR */
#ifdef MI_NOR_AMSS_IMG_SRC_ADDR
  if ( image_id == AMSS_IMG )
  {
    image_src = MI_NOR_AMSS_IMG_SRC_ADDR;
  }
  else
#endif /* MI_NOR_AMSS_IMG_SRC_ADDR */
#ifdef MI_NOR_APPSBL_IMG_SRC_ADDR
  if ( image_id == APPSBL_IMG )
  {
    image_src = MI_NOR_APPSBL_IMG_SRC_ADDR;
  }
  else
#endif /* MI_NOR_APPSBL_IMG_SRC_ADDR */
  {
    /* We don't load the other type of image */
    BL_ERR_FATAL( BL_ERR_WRG_IMGHD_REQUESTED );
  }

  /* Initialze the NOR flash translation based on the NOR image source.
     We don't support a region size for NOR since we don't use partitions. */
  return boot_flash_trans_factory_open( nor_dev_priv.trans_fact_if,
                                        image_src,
                                        0 /* Image size checking disabled */ );
  
} /* dev_nor_open_image() */

/*=========================================================================

FUNCTION  DEV_NOR_OPEN_PARTITION

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
static boot_flash_trans_if_type* dev_nor_open_partition
(
  const char *parti_name                  /* partition to open */
)
{
  /* Non-partition NOR does not have partitions */
  return NULL;
} /* dev_nor_open_partition() */

/*=========================================================================

FUNCTION  DEV_NOR_CLOSE_IMAGE

DESCRIPTION
  Close an instance of the boot translation layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dev_nor_close_image
(
  boot_flash_trans_if_type **p_trans_if
)
{
  boot_flash_trans_factory_close( nor_dev_priv.trans_fact_if, p_trans_if );
} /* dev_nor_close_image() */

/*===========================================================================

FUNCTION  DEV_NOR_LOAD_MIBIB_HEADER

DESCRIPTION
  This function loads the header from NOR into the external RAM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static boolean dev_nor_load_mibib_header
(
  image_type image_id,
  mi_boot_image_header_type *image_header_ptr  /* Pointer to image header */
)
{
  uint32 header_src = 0;

  /* First verify our input parameters */
  BL_VERIFY( image_header_ptr != NULL, BL_ERR_NULL_PTR_PASSED );
  BL_VERIFY( image_id != NONE_IMG, BL_ERR_WRG_IMGHD_REQUESTED );

  /* Get the image source based on the image type */
#ifdef MI_NOR_OSBL_HEADER_SRC_ADDR
  if ( image_id == OSBL_IMG )
  {
    header_src = MI_NOR_OSBL_HEADER_SRC_ADDR;
  }
  else
#endif /* MI_NOR_OSBL_HEADER_SRC_ADDR */
#ifdef MI_NOR_AMSS_HEADER_SRC_ADDR
  if ( image_id == AMSS_IMG )
  {
    header_src = MI_NOR_AMSS_HEADER_SRC_ADDR;
  }
  else
#endif /* MI_NOR_AMSS_HEADER_SRC_ADDR */
#ifdef MI_NOR_APPSBL_HEADER_SRC_ADDR
  if ( image_id == APPSBL_IMG )
  {
    header_src = MI_NOR_APPSBL_HEADER_SRC_ADDR;
  }
  else
#endif /* MI_NOR_APPSBL_HEADER_SRC_ADDR */
#ifdef MI_NOR_APPS_HEADER_SRC_ADDR
  if ( image_id == APPS_IMG )
  {
    header_src = MI_NOR_APPS_HEADER_SRC_ADDR;
  }
  else
#endif /* MI_NOR_APPS_HEADER_SRC_ADDR */
  {
    /* We don't load the other type of image */
    BL_ERR_FATAL( BL_ERR_WRG_IMGHD_REQUESTED );
  }
  
  /* copy the image header from flash to RAM */
  memcpy ( image_header_ptr, (void *) header_src, sizeof(*image_header_ptr) );
  /* verify that the image header type matches the requested type */
  BL_VERIFY( image_header_ptr->image_id == image_id, 
              BL_ERR_WRG_IMGHD_REQUESTED );

  return TRUE;
  
} /* dev_nor_load_mibib_header() */


/*=========================================================================

FUNCTION  BOOT_FLASH_DEV_NOR_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize the boot NOR flash device module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void boot_flash_dev_nor_init_and_register
(
  boot_flash_type             flash_type,
  boot_flash_trans_factory_init_type trans_factory_init,
  struct bl_shared_data_type *bl_shared_data
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_DEV_NOR_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_DEV_NOR_IF_VERSION,
                         BOOT_FLASH_DEV_NOR_VERSION )
  };
  static boot_flash_dev_nor_vtbl_type vtbl =
  {
    dev_nor_open_image,
    dev_nor_open_partition,
    dev_nor_close_image,
    dev_nor_close_image,
    dev_nor_load_mibib_header,
    boot_flash_dev_default_stub,
    boot_flash_dev_default_stub,
    dev_nor_read_nor
  };


  /* Setup the module version structure and virtual function table */
  nor_dev_priv.pub_if.vsn = &vsn;
  nor_dev_priv.pub_if.vtbl = &vtbl;

  /* Register the singletons */
  boot_flash_dev_if_register_singleton( boot_flash_dev_nor_cast_parent_if( &nor_dev_priv.pub_if ) );
  boot_flash_dev_nor_if_register_singleton( &nor_dev_priv.pub_if );

  /* Finally, initialize the translation layer factory */
  nor_dev_priv.trans_fact_if = trans_factory_init();
  BL_VERIFY( nor_dev_priv.trans_fact_if != NULL, BL_ERR_NULL_PTR );

} /* boot_flash_dev_nor_init_and_register() */

