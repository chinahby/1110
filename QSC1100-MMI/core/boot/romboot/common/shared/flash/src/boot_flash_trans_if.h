#ifndef BOOT_FLASH_TRANS_IF_H
#define BOOT_FLASH_TRANS_IF_H

/*===========================================================================

                           B o o t  L o a d e r
             F l a s h  T r a n s l a t i o n  I n t e r f a c e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to the
  boot-specific flash translation module/layer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/boot_flash_trans_if.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "flash_trans_if.h"
#include "boot_clobber_prot.h"
#include "boot_clobber_prot_local.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_FLASH_TRANS_IF_VERSION 1

#define BOOT_FLASH_TRANS_FACTORY_IF_VERSION 1

/* Forward declaration */
struct boot_flash_trans_if_type;


/*=========================================================================

FUNCTION POINTER TYPEDEF  BOOT_FLASH_TRANS_COPY_CALLBACK_FUNC_TYPE

DESCRIPTION
  Function pointer typedef for a callback function that can be called
  intermittently when the boot flash translation module is copying data
  from flash; the callback_data pointer will be data that is associated
  with an instance of the callback, ram_addr will point to the last data
  read from flash and ram_size will indicate the size of the last data
  read from flash.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation succeeded, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
typedef void (*boot_flash_trans_copy_callback_func_type)
(
  void *callback_data,  /* Pointer to data associanted with callback instance */
  void *ram_addr,       /* Pointer to last data read from flash */
  uint32 ram_size       /* Size of last data read from flash */
);

/* Definition of the boot translation module virtual table structure */
typedef struct boot_flash_trans_vtbl_type
{
  /*----------------------------------- 
   * Generic flash module interface 
   *---------------------------------*/
  /* This _must_ be the first element of the structure.  Provides 
   * interface to generic flash routines.  Boot-specific flash routines
   * follow the generic interface and extend the interface.
   */
  flash_trans_vtbl_type parent;

  /*=========================================================================

  FUNCTION POINTER  SET_COPY_CALLBACK

  DESCRIPTION
    Function to set a callback function that will be called intermittently
    when copying data from flash; the pointer to the last data read from flash
    and the data size will be passed to the callback when it is called.

  DEPENDENCIES
    None

  RETURN VALUE
    TRUE if the operation succeeded, FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*set_copy_callback)
  (
    struct boot_flash_trans_if_type *trans_if, /* Pointer to public interface */
    boot_flash_trans_copy_callback_func_type callback_func, /* Callback func */
    void *callback_data                  /* Data to pass to callback function */
  );

} boot_flash_trans_vtbl_type;

/* Definition of public interface to boot translation module */
typedef struct boot_flash_trans_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_trans_vtbl_type *vtbl;

  /* Data structure to facilitate bootloader memory overwrite protection */
  boot_clobber_local_type clobber_tbl;

} boot_flash_trans_if_type;


/* Definition of the boot translation factory module virtual table structure */
typedef struct boot_flash_trans_factory_vtbl_type
{
  /*=========================================================================

  FUNCTION POINTER  OPEN

  DESCRIPTION
    Requests the factory to open an instance of the boot translation module.

  DEPENDENCIES
    None

  RETURN VALUE
    Pointer to the opened boot translation interface

  SIDE EFFECTS
    None

  ===========================================================================*/
  boot_flash_trans_if_type* (*open)
  (
    uint32 region_base_offset,      /* Base offset of the flash region */
    uint32 region_size              /* Maximum size of the flash region */
  );

  /*=========================================================================

  FUNCTION POINTER  CLOSE

 DESCRIPTION
    Requests the factory to close an instance of the boot translation module.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*close)
  (
    boot_flash_trans_if_type **p_trans_if
  );
} boot_flash_trans_factory_vtbl_type;


/* Definition of public interface to boot translation factory module */
typedef struct boot_flash_trans_factory_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_flash_trans_factory_vtbl_type *vtbl;

} boot_flash_trans_factory_if_type;


/* Typedef of factory intit routine */
typedef boot_flash_trans_factory_if_type* (*boot_flash_trans_factory_init_type)
(
  void
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_CAST_PARENT_IF

 DESCRIPTION
  Takes a pointer to the boot translation interface and casts to the parent
  interface type (generic translation interface).

DEPENDENCIES
  None
  
RETURN VALUE
  A pointer to a boot device interface

SIDE EFFECTS
  None

===========================================================================*/
__inline flash_trans_if_type* boot_flash_trans_cast_parent_if
(
  boot_flash_trans_if_type *trans_if
)
{
  return (flash_trans_if_type*) trans_if;
} /* boot_flash_trans_cast_parent_if() */

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_READ

DESCRIPTION
  Function to copy a region of data from a logical byte address
  to the destination address.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the operation succeeded, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_flash_trans_read
(
  struct  boot_flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  return trans_if->vtbl->parent.read( boot_flash_trans_cast_parent_if( trans_if ),
                                      dest_addr, byte_offset, size );
} /* boot_flash_trans_read() */

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_SLOPPY_READ

DESCRIPTION
  Function to copy a region of data from a logical byte address to the
  destination address, and to allow the copy operation to round the start
  and end addresses of the copy region up or down to the nearest page
  boundaries (for page or sector based devices) for efficiency.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation succeeded, FALSE otherwise

 SIDE EFFECTS
  This function may adjust the start address of the copy down to the next
  lowest page boundary (for NAND flash), and adjust the size of the copy
  up to match the next highest page boundary (for NAND flash).  This means
  that this function may copy data to RAM below the buffer pointer and
  beyond the limit of the buffer as defined by the size parameter.  This
  function should not be used to copy data into a fixed buffer, only to
  load whole images from flash to RAM, where it is safe to copy whole
  pages from flash to RAM (for NAND).

===========================================================================*/
__inline boolean boot_flash_trans_sloppy_read
(
  struct  boot_flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  return trans_if->vtbl->parent.sloppy_read( boot_flash_trans_cast_parent_if( trans_if ),
                                             dest_addr, byte_offset, size );
} /* boot_flash_trans_sloppy_read() */

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_GET_LOGICAL_ADDR

DESCRIPTION
  Function to translate a device-dependent physical flash source address
  to a logical byte address.

DEPENDENCIES
  None

RETURN VALUE
  The logical byte address.

SIDE EFFECTS
  None

===========================================================================*/
__inline uint32 boot_flash_trans_get_logical_addr
(
  struct boot_flash_trans_if_type *trans_if,  /* Pointer to the translation
                                            interface */
  uint32 image_source                    /* Device-dependent physical
                                            flash source address */
)
{
  return trans_if->vtbl->parent.get_logical_addr( boot_flash_trans_cast_parent_if( trans_if ),
                                                  image_source );
} /* boot_flash_trans_get_logical_addr() */

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_SET_COPY_CALLBACK

DESCRIPTION
  Function to set a callback function that will be called intermittently
  when copying data from flash; the pointer to the last data read from flash
  and the data size will be passed to the callback when it is called.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation succeeded, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_flash_trans_set_copy_callback
(
  struct boot_flash_trans_if_type *trans_if, /* Pointer to public interface */
  boot_flash_trans_copy_callback_func_type callback_func, /* Callback func */
  void *callback_data                  /* Data to pass to callback function */
)
{
  return trans_if->vtbl->set_copy_callback( trans_if, callback_func, callback_data );
} /* boot_flash_trans_set_copy_callback() */

/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_GET_CLOBBER_TBL_PTR

DESCRIPTION
  Returns a pointer to the boot translation interface local clobber
  protection table.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the clobber table

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_clobber_local_type* boot_flash_trans_get_clobber_tbl_ptr
(
  boot_flash_trans_if_type *trans_if
)
{
  return &trans_if->clobber_tbl;
} /* boot_flash_trans_get_clobber_tbl_ptr() */


/*=========================================================================

FUNCTION  BOOT_FLASH_TRANS_FACTORY_OPEN

DESCRIPTION
  Requests the factory to open an instance of the boot translation module.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the opened boot translation interface

SIDE EFFECTS
  None

===========================================================================*/
__inline boot_flash_trans_if_type* boot_flash_trans_factory_open
(
  boot_flash_trans_factory_if_type *trans_fact_if,
  uint32 region_base_offset,      /* Base offset of the flash region */
  uint32 region_size              /* Maximum size of the flash region */
)
{
  return trans_fact_if->vtbl->open( region_base_offset,
                                    region_size );
} /* boot_flash_trans_factory_open() */

/*=========================================================================
FUNCTION  BOOT_FLASH_TRANS_FACTORY_CLOSE

DESCRIPTION
  Requests the factory to close an instance of the boot translation module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void boot_flash_trans_factory_close
(
  boot_flash_trans_factory_if_type *trans_fact_if,
  boot_flash_trans_if_type **p_trans_if
)
{
  trans_fact_if->vtbl->close( p_trans_if );
} /* boot_flash_trans_factory_close() */

#endif /* BOOT_FLASH_TRANS_IF_H */

