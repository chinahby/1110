#ifndef DBL_FLASH_H
#define DBL_FLASH_H

/*=============================================================================

                      D e v i c e  B o o t  L o a d e r
                            H e a d e r  F i l e
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

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_flash.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "dbl.h"
#include "dbl_target_accessor.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
extern dbl_error_type dbl_flash_op
(
  const dbl_shared_data_type *shared_data,
  const dbl_flash_op_type    *op_type
);

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
extern dbl_error_type dbl_flash_read_image_hdr
(
  const dbl_shared_data_type                *shared_data,
  mi_boot_image_header_type                 *image_hdr,
  const dbl_flash_partition_image_info_type *partition_hdr
);

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
extern dbl_error_type dbl_flash_read_image
(
  const dbl_shared_data_type                *shared_data,
  const mi_boot_image_header_type           *image_hdr,
  const dbl_flash_partition_image_info_type *partition_hdr,
  const uint32                              image_offset
);

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
extern void dbl_flash_share_data
(
  const dbl_shared_data_type            *shared_data,
  dbl_if_device_configuration_info_type *shared_dev_info
);

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
extern void dbl_flash_get_image_info
(
  const dbl_shared_data_type                *shared_data,
  const image_type                          *img_type,
  dbl_flash_partition_image_info_type       *partition_hdr
);

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
extern  void dbl_flash_find_partition_table
(
  dbl_shared_data_type *shared_data
);

#endif  /* DBL_FLASH_H */

