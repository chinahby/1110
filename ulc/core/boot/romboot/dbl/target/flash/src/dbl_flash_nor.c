/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/flash/dbl_flash_nor.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================
                            INCLUDE FILES FOR MODULE
=============================================================================*/
#include "boot_comdef.h"
#include "dbl_flash_nor.h"
#include <string.h>
/*=============================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
=============================================================================*/


/*=============================================================================   
                              FUNCTION DEFINITIONS
=============================================================================*/

/*===========================================================================

FUNCTION  DBL_FLASH_NOR_OP

DESCRIPTION
  Flash specific "op" function that carries out a flash command.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dbl_error_type dbl_flash_nor_op
(
  const dbl_flash_device_driver_type *flash_if,
  const dbl_flash_op_type            *op_type
)
{
  uint8 *dst;                 /* stores destination to copy data to */
  uint8 *src;
  dbl_error_type read_status = DBL_ERR_NONE;

  DBL_VERIFY((flash_if != NULL), (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY((op_type != NULL), (uint32)DBL_ERR_NULL_PTR);

  src = (uint8 *)op_type->src;
  dst = (uint8 *)op_type->dst;

  /* The only command type supported is read bytes. Otherwise is a fatal
   * error */
  DBL_VERIFY( op_type->type == READ_BYTES, (uint32)DBL_ERR_NOR_OP_TYPE_INVALID );

  if ((uint32)src == (uint32)dst)
  {
    /* Since source == dst, no need for copy. Return no error */
    return DBL_ERR_NONE;
  }

  memcpy( dst, src, op_type->length );

  return read_status;
  
}/* dbl_flash_shared_op */


/*===========================================================================

FUNCTION  DBL_FLASH_NOR_DETECTED

DESCRIPTION
  This function will get called when DBL finds that NOR flash was detected 
  by PBL. This function will update the flash structure to be used by DBL
  later to load different images.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_flash_nor_detected
(
  dbl_flash_device_driver_type *flash_if
)
{

  DBL_VERIFY((flash_if != NULL), (uint32)DBL_ERR_NULL_PTR);
  
  flash_if->flash_op = dbl_flash_nor_op;
  flash_if->share_data = dbl_flash_nor_share_data;
  flash_if->get_image_info = dbl_flash_nor_get_image_info;
  flash_if->read_image_hdr = dbl_flash_nor_read_image_hdr;
  flash_if->read_image = dbl_flash_nor_read_image;

} /* dbl_flash_nor_detected */

/*=============================================================================

FUNCTION  DBL_FLASH_NOR_SHARE_DATA

DESCRIPTION
  NOR device specific function to share flash specific data in the share
  structure in the state of the DBL. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECT
  None

=============================================================================*/
void dbl_flash_nor_share_data
(
  const dbl_flash_device_driver_type    *flash_if,
  dbl_if_device_configuration_info_type *shared_dev_info
)
{
} /* dbl_flash_nand_share_data() */

/*===========================================================================

FUNCTION  DBL_FLASH_NOR_READ_IMAGE_HDR

DESCRIPTION
  Flash specific "read_image_hdr" function that reads Image header based
  on flash layout.

DEPENDENCIES
  None
  
RETURN VALUE
  dbl_error_type

SIDE EFFECTS
  None

===========================================================================*/
dbl_error_type dbl_flash_nor_read_image_hdr
(
  const dbl_flash_device_driver_type         *flash_if,
  mi_boot_image_header_type                  *image_hdr,
  const dbl_flash_partition_image_info_type  *partition_hdr
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;
  dbl_flash_op_type op;

  DBL_VERIFY((flash_if != NULL), (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY((image_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((partition_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);

  op.img_to_read = READ_IMG_HDR;
  op.magic_num_1 = partition_hdr->magic1;
  op.magic_num_2 = partition_hdr->magic2;
  op.type        = READ_BYTES;
  op.src         = partition_hdr->src;
  op.dst         = (uint8*)image_hdr;
  /* Length must be word aligned, this is checked by the flash op */
  op.length      = sizeof(mi_boot_image_header_type);
  
  ret_value = dbl_flash_nor_op(flash_if, &op);

  return ret_value;
}/* dbl_flash_nor_read_image_hdr */

/*===========================================================================

FUNCTION  DBL_FLASH_NOR_READ_IMAGE

DESCRIPTION
  Flash specific "read_image" function that reads Image header based
  on flash layout.

DEPENDENCIES
  None
  
RETURN VALUE
  dbl_error_type

SIDE EFFECTS
  None

===========================================================================*/
dbl_error_type dbl_flash_nor_read_image
(
  const dbl_flash_device_driver_type         *flash_if,
  const mi_boot_image_header_type            *image_hdr,
  const dbl_flash_partition_image_info_type  *partition_hdr,
  const uint32                               image_offset
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;
  dbl_flash_op_type op;

  DBL_VERIFY((flash_if != NULL), (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY((partition_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((image_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((image_hdr->image_size != 0), (uint32)DBL_ERR_IMG_SIZE_ZERO);

  op.img_to_read = READ_IMG;
  op.magic_num_1 = partition_hdr->magic1;
  op.magic_num_2 = partition_hdr->magic2;
  op.type        = READ_BYTES;
  op.src  = image_hdr->image_src + image_offset;
  op.dst  = image_hdr->image_dest_ptr;
  /* Length must be word aligned, this is checked by the flash op */
  op.length = image_hdr->image_size;

  ret_value = dbl_flash_nor_op(flash_if, &op);

  return ret_value;
}/* dbl_flash_nor_read_image */

/*===========================================================================

FUNCTION  DBL_FLASH_NOR_GET_IMAGE_INFO

DESCRIPTION
  Flash specific "get_image_info" function that returns image info for 
  non-partition based NOR Targets

DEPENDENCIES
  None
  
RETURN VALUE
  dbl_error_type

SIDE EFFECTS
  None

===========================================================================*/
dbl_error_type dbl_flash_nor_get_image_info
(
  const dbl_flash_device_driver_type   *flash_dev_driver,
  const image_type                          *img_type,
  dbl_flash_partition_image_info_type       *partition_hdr
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;

  DBL_VERIFY((flash_dev_driver != NULL), (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY((partition_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY((img_type != NULL), (uint32)DBL_ERR_NULL_PTR); 

  switch (*img_type)
  {
    case FSBL_IMG:
      partition_hdr->src = (uint32)MI_NOR_DBL_HEADER_SRC_ADDR;
      partition_hdr->length = (uint32)sizeof(mi_boot_image_header_type);
      break;

    case OSBL_IMG:
      partition_hdr->src = (uint32)MI_NOR_OSBL_HEADER_SRC_ADDR;
      partition_hdr->length = (uint32)sizeof(mi_boot_image_header_type);
      break;

    default:
      ret_value = DBL_ERR_IMG_TYPE_INVALID;          
      break;
  }
  
  return ret_value;
}/* dbl_flash_nor_get_image_info */

