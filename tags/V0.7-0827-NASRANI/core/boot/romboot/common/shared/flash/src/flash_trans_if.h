#ifndef FLASH_TRANS_IF_H
#define FLASH_TRANS_IF_H

/*===========================================================================

     G e n e r i c  F l a s h  T r a n s l a t i o n  I n t e r f a c e
                           H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the generic flash translation module/layer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/flash/flash_trans_if.h#1 $

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

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define FLASH_TRANS_IF_VERSION 1

/* Forward declaration */
struct flash_trans_if_type;


/* Definition of the translation module virtual table structure */
typedef struct flash_trans_tvbl_type
{
  /*=========================================================================

  FUNCTION POINTER  READ

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
  boolean (*read)
  (
    struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                               interface */
    void   *dest_addr,                      /* Destination address */
    uint32  byte_offset,                    /* Logical source byte address */
    uint32  size                            /* Size to copy in bytes */
  );

  /*=========================================================================

  FUNCTION POINTER  SLOPPY_READ

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
  boolean (*sloppy_read)
  (
    struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                               interface */
    void   *dest_addr,                      /* Destination address */
    uint32  byte_offset,                    /* Logical source byte address */
    uint32  size                            /* Size to copy in bytes */
  );

  /*=========================================================================

  FUNCTION POINTER  GET_LOGICAL_ADDR

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
  uint32 (*get_logical_addr)
  (
    struct flash_trans_if_type *trans_if,  /* Pointer to the translation
                                              interface */
    uint32 image_source                    /* Device-dependent physical
                                              flash source address */
  );

} flash_trans_vtbl_type;


/* Definition of public interface to the translation module */
typedef struct flash_trans_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  flash_trans_vtbl_type *vtbl;

} flash_trans_if_type;


/* Definition of the translation factory module virtual table structure */
typedef struct flash_trans_factory_vtbl_type
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
  flash_trans_if_type* (*open)
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
    flash_trans_if_type **p_trans_if
  );
} flash_trans_factory_vtbl_type;


/* Definition of public interface to translation factory module */
typedef struct flash_trans_factory_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  flash_trans_factory_vtbl_type *vtbl;

} flash_trans_factory_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  FLASH_TRANS_READ

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
__inline boolean flash_trans_read
(
  struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  return trans_if->vtbl->read( trans_if, dest_addr, byte_offset, size );
} /* flash_trans_read() */

/*=========================================================================

FUNCTION  FLASH_TRANS_SLOPPY_READ

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
__inline boolean flash_trans_sloppy_read
(
  struct  flash_trans_if_type *trans_if,  /* Pointer to the translation
                                             interface */
  void   *dest_addr,                      /* Destination address */
  uint32  byte_offset,                    /* Logical source byte address */
  uint32  size                            /* Size to copy in bytes */
)
{
  return trans_if->vtbl->sloppy_read( trans_if, dest_addr, byte_offset, size );
} /* flash_trans_sloppy_read() */

/*=========================================================================

FUNCTION  FLASH_TRANS_GET_LOGICAL_ADDR

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
__inline uint32 flash_trans_get_logical_addr
(
  struct flash_trans_if_type *trans_if,  /* Pointer to the translation
                                            interface */
  uint32 image_source                    /* Device-dependent physical
                                            flash source address */
)
{
  return trans_if->vtbl->get_logical_addr( trans_if, image_source );
} /* flash_trans_get_logical_addr() */


/*=========================================================================

FUNCTION  FLASH_TRANS_FACTORY_OPEN

DESCRIPTION
  Requests the factory to open an instance of the translation module.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the opened boot translation interface

SIDE EFFECTS
  None

===========================================================================*/
__inline flash_trans_if_type* flash_trans_factory_open
(
  flash_trans_factory_if_type *trans_fact_if,
  uint32 region_base_offset,      /* Base offset of the flash region */
  uint32 region_size              /* Maximum size of the flash region */
)
{
  return trans_fact_if->vtbl->open( region_base_offset,
                                    region_size );
} /* flash_trans_factory_open() */

/*=========================================================================
FUNCTION  FLASH_TRANS_FACTORY_CLOSE

DESCRIPTION
  Requests the factory to close an instance of the translation module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void flash_trans_factory_close
(
  flash_trans_factory_if_type *trans_fact_if,
  flash_trans_if_type **p_trans_if
)
{
  trans_fact_if->vtbl->close( p_trans_if );
} /* flash_trans_factory_close() */

#endif /* FLASH_TRANS_IF_H */

