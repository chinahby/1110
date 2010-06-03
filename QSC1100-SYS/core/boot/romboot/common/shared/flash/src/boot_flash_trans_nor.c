/*=============================================================================

                           B o o t   L o a d e r
            N O R  F l a s h  T r a n s l a t i o n  M o d u l e

GENERAL DESCRIPTION
  This header file contains the implementation for the boot-specific NOR
  flash translation module/layer.

EXTERNALIZED FUNCTIONS
  boot_flash_trans_nor_factory_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_trans_nor.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_flash_trans_nor.h"
#include "boot_flash_dev_nor_if.h"
#include <string.h>

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/* Definition of private NOR flash translation module data structure */
typedef struct boot_flash_trans_nor_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure. */
  boot_flash_trans_if_type  pub_if;

  /* NOR flash module variables */
  uint32 region_base_addr;             /* Base offset of the flash region
                                          in bytes */
  uint32 region_size;                  /* Maximum size of the flash region
                                          in bytes */
  
  /* Variables for maintaining a callback function */
  boot_flash_trans_copy_callback_func_type copy_callback_func;
  void *copy_callback_data;
  
} boot_flash_trans_nor_type;


/* The boot translation factory private data structure */
typedef struct boot_flash_trans_nor_factory_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *   Note: This _must_ be the first element of the structure.
   *---------------------------------*/
  boot_flash_trans_factory_if_type pub_if;

  /* Array of the boot translation private data structures allocated
   * by factory */
  boot_flash_trans_nor_type trans_priv[BOOT_FLASH_TRANS_MAX_INSTANCES];

  /* Array indicating which elements of the boot translation private data
   * array are in use */
  boolean trans_priv_inuse[BOOT_FLASH_TRANS_MAX_INSTANCES];

} boot_flash_trans_not_factory_private_type;


/* The NOT translation factory private data */
static boot_flash_trans_not_factory_private_type fact_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_NOR_GET_LOGICAL_ADDR

DESCRIPTION
  Function to translate a the NOR physical flash source to a logical
  flash address, based on the region base.

DEPENDENCIES
  None
  
RETURN VALUE
  The logical flash address.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 boot_flash_trans_nor_get_logical_addr
(
  flash_trans_if_type *trans_if,  /* Pointer to the public interface */
  uint32 image_source              /* Block number image source */
)
{
  /* Cast the public interface pointer to our private type */
  boot_flash_trans_nor_type * const trans_priv =
    (boot_flash_trans_nor_type*)trans_if;
  
  BFA_NOR_VERIFY( trans_if != NULL );
  BFA_NOR_VERIFY2( image_source >= trans_priv->region_base_addr, 0xFFFFFFFF );
  
  return image_source - trans_priv->region_base_addr;
} /* boot_flash_trans_nor_get_logical_addr() */


/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_NOR_READ

DESCRIPTION
  Function to copy a region of data from a logical byte address to 
  the destination address.  Given the logical range to copy, this function
  ensures that the copy does not overwrite a protected bootloader memory
  region, then invokes the device-specific NOR copy routine.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the operation succeeded, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean boot_flash_trans_nor_read
(
  flash_trans_if_type *trans_if,  /* Pointer to the translation interface */
  void   *dest_addr,              /* Destination address */
  uint32  byte_offset,            /* Logical source byte address */
  uint32  size                    /* Size to copy in bytes */
)
{
  /* Cast the pointer to the public interface to our private type */
  boot_flash_trans_nor_type * const trans_priv =
    (boot_flash_trans_nor_type*)trans_if;
  uint32 phys_flash_addr;
  boolean status;

  BFA_NOR_VERIFY( trans_if != NULL );

  /* First make sure the copy is to a valid memory range */
  status = boot_clobber_check_local_address_range( &trans_priv->pub_if.clobber_tbl,
                                                   dest_addr, size );
  BFA_NOR_VERIFY( status );

  /* Next translate the logical flash offset to the physical flash address */
  phys_flash_addr = trans_priv->region_base_addr + byte_offset;

  /* Make sure that we have not exceeded to size of the region */
  if ( trans_priv->region_size > 0 )
  {
    BFA_NOR_VERIFY( ( phys_flash_addr + size ) <= 
                    ( trans_priv->region_base_addr + trans_priv->region_size ) );
  }

  /* Perform the copy if the destination address is not equal to the 
     source address (for execute-in-place) */
  if ( phys_flash_addr != (uint32) dest_addr )
  {
    status = boot_flash_dev_nor_read_nor( dest_addr, phys_flash_addr, size );
  }
  else
  {
    status = TRUE;
  }
  
  /* If the callback function has been set, call the callback function
     with the location and size of the area of RAM that was just read
     from flash */
  if ( trans_priv->copy_callback_func != NULL )
  {
    trans_priv->copy_callback_func( trans_priv->copy_callback_data, 
                                    dest_addr, size );
  }

  return status;
  
} /* boot_flash_trans_nor_read() */


/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_NOR_SET_COPY_CALLBACK

DESCRIPTION
  Function to set the callback function that will be called intermittently
  when copying data from flash; the pointer to the last data read from flash
  and the data size will be passed to the callback when it is called.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation succeeded, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean boot_flash_trans_nor_set_copy_callback
(
  boot_flash_trans_if_type *trans_if, /* Pointer to the boot flash interface */
  boot_flash_trans_copy_callback_func_type callback_func, /* Callback func ptr */
  void *callback_data                  /* Data to pass to callback function */
)
{
  /* Cast the pointer to the public interface to our private type */
  boot_flash_trans_nor_type * const trans_priv =
    (boot_flash_trans_nor_type*)trans_if;

  /* Verify out input parameter */
  BFA_NAND_VERIFY( trans_if != NULL );

  /* Save the callback function and data */
  trans_priv->copy_callback_func = callback_func;
  trans_priv->copy_callback_data = callback_data;

  return TRUE;
  
} /* boot_flash_trans_nor_set_copy_callback() */


/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_NOR_INIT

DESCRIPTION
  Function to initialize an instance of the boot NOR flash translation module.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the boot flash translation public interface.

SIDE EFFECTS
  None

===========================================================================*/
static void boot_flash_trans_nor_init
(
  boot_flash_trans_nor_type *trans_priv,  /* Pointer to the private boot
                                              NOR flash translation structure */
  uint32 region_base_offset,               /* Base offset of the flash region */
  uint32 region_size                       /* Maximum size of the flash region */
)
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_NOR_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_IF_VERSION,
                         BOOT_FLASH_TRANS_NOR_VERSION )
  };
  static boot_flash_trans_vtbl_type vtbl =
  {
    boot_flash_trans_nor_read,
    boot_flash_trans_nor_read,
    boot_flash_trans_nor_get_logical_addr,
    boot_flash_trans_nor_set_copy_callback
  };


  BL_VERIFY( trans_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  /* Setup the module version structure and virtual function table */
  trans_priv->pub_if.vsn = &vsn;
  trans_priv->pub_if.vtbl = &vtbl;

  /* Initialize the clobber protection local table */
  boot_clobber_local_init( &trans_priv->pub_if.clobber_tbl );

  /* Store the location and size of the flash region */
  trans_priv->region_base_addr = region_base_offset;
  trans_priv->region_size = region_size;

  /* Initialize the copy callback function pointer and data */
  trans_priv->copy_callback_func = NULL;
  trans_priv->copy_callback_data = NULL;    

} /* boot_flash_trans_nor_init() */


/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_NOR_FACTORY_OPEN

DESCRIPTION
  Requests the factory to open an instance of the boot translation module.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the opened boot translation interface

SIDE EFFECTS
  None

===========================================================================*/
static boot_flash_trans_if_type *boot_flash_trans_nor_factory_open
(
  uint32 region_base_offset,      /* Base offset of the flash region */
  uint32 region_size              /* Maximum size of the flash region */
)
{
  int i;
  boot_flash_trans_nor_type *trans_priv = NULL;  /* Pointer to the private
                                                   data structure */

  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( !fact_priv.trans_priv_inuse[i] )
    {
      trans_priv = &fact_priv.trans_priv[i];
      fact_priv.trans_priv_inuse[i] = TRUE;
      break;
    }
  }
  BL_VERIFY( trans_priv != NULL, BL_ERR_RESOURCE_NOT_AVAILABLE );

  /* Initialize the NOR translation module */
  boot_flash_trans_nor_init( trans_priv, region_base_offset, region_size );

  return &trans_priv->pub_if;
} /* boot_flash_trans_nor_factory_open() */

/*=========================================================================
FUNCTION  BOOT_FLASH_TRANS_NOR_FACTORY_CLOSE

DESCRIPTION
  Requests the factory to close an instance of the boot translation module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void boot_flash_trans_nor_factory_close
(
  boot_flash_trans_if_type **p_trans_if
)
{
  int i;

  BL_VERIFY( p_trans_if != NULL, BL_ERR_NULL_PTR_PASSED );

  for ( i = 0; i < BOOT_FLASH_TRANS_MAX_INSTANCES; i++ )
  {
    if ( *p_trans_if == (boot_flash_trans_if_type*) &fact_priv.trans_priv[i] )
    {
      BL_VERIFY( fact_priv.trans_priv_inuse[i], BL_ERR_OTHERS );
      fact_priv.trans_priv_inuse[i] = FALSE;
      *p_trans_if = NULL;
      return;
    }
  }

  /* If we have gotten here then the specified translation interface cannot be
   * found in the array */
  BL_ERR_FATAL( BL_ERR_OTHERS );
} /* boot_flash_trans_nor_factory_close() */

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_NOR_FACTORY_INIT

DESCRIPTION
  Initializes the NOR boot translation factory.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the NOR translation factory interface

SIDE EFFECTS
  None

===========================================================================*/
boot_flash_trans_factory_if_type* boot_flash_trans_nor_factory_init( void )
{
  static const boot_module_version_type vsn =
  {
    BOOT_FLASH_TRANS_FACTORY_NOR_NAME,
    BOOT_MODULE_VERSION( BOOT_FLASH_TRANS_FACTORY_IF_VERSION,
                         BOOT_FLASH_TRANS_FACTORY_NOR_VERSION )
  };
  static boot_flash_trans_factory_vtbl_type vtbl = 
  {
    boot_flash_trans_nor_factory_open,
    boot_flash_trans_nor_factory_close
  };
  static boolean init_done = FALSE;


  /* Only initialize the factory module once, if init() is called subsequently
   * it will return a pointer to the live module interface */
  if ( !init_done )
  {
    /* Setup the module version structure and virtual function table */
    fact_priv.pub_if.vsn = &vsn;
    fact_priv.pub_if.vtbl = &vtbl;

    /* Zero out the inuse marker for the private ELF data structure */
    memset( fact_priv.trans_priv_inuse, 0, sizeof(fact_priv.trans_priv_inuse) );

    init_done = TRUE;
  }

  /* Return pointer to the one NOR translation factory interface */
  return &fact_priv.pub_if;
} /* boot_flash_trans_nor_factory_init() */

