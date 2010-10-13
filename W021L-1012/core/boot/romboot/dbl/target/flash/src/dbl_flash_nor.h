#ifndef DBL_FLASH_NOR_H
#define DBL_FLASH_NOR_H

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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/flash/dbl_flash_nor.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*=========================================================================== 
                           INCLUDE FILES
===========================================================================*/
#include "dbl_flash_if.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

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
);

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
);

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
);

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
);

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
);

#endif  /* DBL_FLASH_NOR_H */

