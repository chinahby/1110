/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                  F l a s h  D r i v e r  I n t e r f a c e

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

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_flash.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "boot_comdef.h"
#include "dbl_flash_mc.h"
#include "dbl_flash.h"
#include "dbl_error_handler.h"
#include "dbl_mc.h"
#include "dbl_target_accessor.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_FLASH_OP

DESCRIPTION
  Flash specific "op" function that carries out a flash command.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dbl_error_type dbl_flash_op
(
  const dbl_shared_data_type *shared_data,
  const dbl_flash_op_type    *op_type
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;
  dbl_flash_device_driver_type   *flash_dev_driver;
  
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((op_type != NULL), (uint32)DBL_ERR_NULL_PTR);  
  flash_dev_driver = shared_data->flash_dev_driver;
  ret_value = flash_dev_driver->flash_op(flash_dev_driver, op_type);
  return ret_value;
} /* dbl_flash_op */

/*===========================================================================

FUNCTION  DBL_FLASH_READ_IMAGE_HDR

DESCRIPTION
  Flash specific "read_image_hdr" function that reads Image header based
  on flash layout.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dbl_error_type dbl_flash_read_image_hdr
(
  const dbl_shared_data_type                 *shared_data,
  mi_boot_image_header_type                  *image_hdr,
  const dbl_flash_partition_image_info_type  *partition_hdr
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;
  dbl_flash_device_driver_type   *flash_dev_driver;
  
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((image_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);  
  flash_dev_driver = shared_data->flash_dev_driver;
  ret_value = flash_dev_driver->read_image_hdr( flash_dev_driver, 
                                                image_hdr, 
                                                partition_hdr
                                              );
  return ret_value;
} /* dbl_flash_read_image_hdr */

/*===========================================================================

FUNCTION  DBL_FLASH_READ_IMAGE

DESCRIPTION
  Flash specific "read_image" function that reads Image based
  on flash layout.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dbl_error_type dbl_flash_read_image
(
  const dbl_shared_data_type                 *shared_data,
  const mi_boot_image_header_type            *image_hdr,
  const dbl_flash_partition_image_info_type  *partition_hdr,
  const uint32                               image_offset
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;
  dbl_flash_device_driver_type   *flash_dev_driver;
  
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((image_hdr != NULL), (uint32)DBL_ERR_NULL_PTR);  
  flash_dev_driver = shared_data->flash_dev_driver;
  ret_value = flash_dev_driver->read_image( flash_dev_driver, 
                                            image_hdr, 
                                            partition_hdr,
                                            image_offset
                                          );
  return ret_value;  
} /* dbl_flash_read_image */

/*===========================================================================

FUNCTION  DBL_FLASH_FIND_PARTITION_TABLE

DESCRIPTION
  Flash specific "find_partition_table" function that finds partition table
  in the given flash.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_flash_find_partition_table
(
  dbl_shared_data_type *shared_data
)
{
  dbl_flash_device_driver_type   *flash_dev_driver;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);

  shared_data->dbl_state = DBL_STATE_FIND_PARTITION;
 
  flash_dev_driver = shared_data->flash_dev_driver;

  if (flash_dev_driver->find_partition_table != NULL)
  {
    shared_data->sys_partition_pointer = flash_dev_driver->find_partition_table( flash_dev_driver);
    DBL_VERIFY((shared_data->sys_partition_pointer != NULL), (uint32)DBL_ERR_MIBIB_NOT_FOUND); 
  }
  else
  {
    shared_data->sys_partition_pointer = NULL;
  }
} /* dbl_flash_find_partition_table */

/*===========================================================================

FUNCTION  DBL_FLASH_GET_IMAGE_INFO

DESCRIPTION
  Flash specific "get_image_info" function that get image related information
  from the target specific interface.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_flash_get_image_info
(
  const dbl_shared_data_type                *shared_data,
  const image_type                          *img_type,
  dbl_flash_partition_image_info_type       *partition_hdr
)
{

  dbl_error_type status;
  dbl_flash_device_driver_type   *flash_dev_driver;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);
 
  flash_dev_driver = shared_data->flash_dev_driver;

  if (flash_dev_driver->get_image_info != NULL)
  {
    status = flash_dev_driver->get_image_info( flash_dev_driver,
                                               img_type,
                                               partition_hdr
                                             );
    DBL_VERIFY((status == DBL_ERR_NONE), (uint32)DBL_ERR_FAILED_TO_GET_IMG_INFO); 
  }
  else
  {
    partition_hdr->src    = NULL;
    partition_hdr->length = 0;
  }

} /* dbl_flash_get_image_info */

/*=============================================================================

FUNCTION  DBL_FLASH_SHARE_DATA

DESCRIPTION
  Device specific function to share flash specific data in the share
  structure in the state of the DBL. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECT
  None

=============================================================================*/
void dbl_flash_share_data
(
  const dbl_shared_data_type            *shared_data,
  dbl_if_device_configuration_info_type *shared_dev_info
)
{
  dbl_flash_device_driver_type   *flash_dev_driver;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);
 
  flash_dev_driver = shared_data->flash_dev_driver;
  flash_dev_driver->share_data(flash_dev_driver, shared_dev_info);
} /* dbl_flash_share_data */

/*===========================================================================

FUNCTION  DBL_FLASH_INIT

DESCRIPTION
  This function detects the flash type and call the appropriate driver code
  to initialize the global structure.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_flash_init
(
  dbl_shared_data_type *shared_data
)
{
  static dbl_flash_device_driver_type   flash_device_driver;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  

  shared_data->flash_dev_driver = &flash_device_driver;
  /*---------------------------------------------------
    Get Flash specific Genric Driver.
  -----------------------------------------------------*/
  shared_data->dbl_state = DBL_STATE_FLASH_DRIVER_INIT;
  dbl_flash_driver_if_init(shared_data->flash_dev_driver);
}/* dbl_flash_init */

