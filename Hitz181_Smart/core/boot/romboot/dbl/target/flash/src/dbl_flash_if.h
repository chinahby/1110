#ifndef DBL_FLASH_IF_H
#define DBL_FLASH_IF_H

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

  $Header: //depot/asic/qsc1100/romboot/sbl/dbl/flash/dbl_flash_if.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "miimageheader.h"
#include "boot_dbl_if.h"
#include "dbl_error_handler.h"

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/* Type of flash command to execute */
typedef enum
{
  READ_PAGES,
  READ_BYTES,
  FLASH_CMD_MAX = 0x7FFFFFFF
}dbl_flash_op_enum_type;

/* Type of flash imgae to read */
typedef enum
{
  READ_MIBIB,
  READ_IMG_HDR,
  READ_IMG,
  FLASH_READ_IMAGE_MAX = 0x7FFFFFFF
}dbl_flash_read_image_enum_type;

/* Flash device command */
typedef struct
{
  /*------------------------------------------------------
    Type of image to read
  -------------------------------------------------------*/
  dbl_flash_read_image_enum_type   img_to_read; /* Type of image to read   */
  uint32                           magic_num_1;
  uint32                           magic_num_2;
  
  /*------------------------------------------------------
    Type of opeartion to be performed to read the image
  -------------------------------------------------------*/
  dbl_flash_op_enum_type           type;        /* the type of command changes the
                                                   interpretation of the various command fields
                                                   of the command packet */
  uint32                           src;         /* src is start page, or start offset in bytes
                                                   to start reading from the flash device */
  uint8*                           dst;         /* dst is memory location to write data read
                                                   from flash */
  uint32                           length;      /* length is number of pages to read, or
                                                   number of bytes to read */

}dbl_flash_op_type;

/*---------------------------------------------------------------------------
  Strutucre to store Partition Image Info.
---------------------------------------------------------------------------*/
typedef struct dbl_flash_partition_image_info_type
{
  uint32        src;      /* src is block number, or start offset in bytes
                             to start reading from the flash device */
  uint32        length;   /* length is maximum length of the image in
                             byte */
  uint32        magic1;   /* Magic number for the image */
  uint32        magic2;   /* Magic number for the image */                                
}dbl_flash_partition_image_info_type;


typedef struct dbl_flash_device_driver_type dbl_flash_device_driver_type;

/*---------------------------------------------------------------------------
  Strutucre to store NAND/OneNAND/MDOC settings.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Should be the first entry in the struture which will be part of the union */
  /* Flash specific read function. if it needs to be shared. */
  dbl_error_type (*read)
  (
    const  dbl_flash_device_driver_type *,
    uint32 page_id,               /* Page to read               */
    uint32 byte_offset,           /* Offset into page to start copying */
    uint32 bytes_to_copy,         /* Bytes to copy */
    uint8  *data_ptr              /* Pointer to location to store
                                   data read from page        */
  );

  uint32       pages_per_block;
  uint32       no_of_blocks;
  uint32       max_num_of_bad_blocks;
}dbl_flash_nand_device_info_type;

/*---------------------------------------------------------------------------
  Strutucre to store NOR settings.
---------------------------------------------------------------------------*/
typedef struct
{
  /* Should be the first entry in the struture which will be part of the union */
  /* Flash specific read function. if it needs to be shared. */
  dbl_error_type (*read)
  (
    const  dbl_flash_device_driver_type *,
    uint32 page_id,               /* Page to read               */
    uint32 byte_offset,           /* Offset into page to start copying */
    uint32 bytes_to_copy,         /* Bytes to copy */
    uint8  *data_ptr              /* Pointer to location to store
                                   data read from page        */
  );
  uint32       sector_size;
}dbl_flash_nor_device_info_type;

/*---------------------------------------------------------------------------
  Flash data
---------------------------------------------------------------------------*/
struct dbl_flash_device_driver_type
{
  /*---------------------------------------------------
    Flash Specific Genric Driver.
  -----------------------------------------------------*/
  
  /* Flash specific "op" function that carries out flash command */
  dbl_error_type (*flash_op)
  (
    const dbl_flash_device_driver_type *,
    const dbl_flash_op_type *
  );

  /*---------------------------------------------------
    Flash Specific logic which will be using flash
    specific driver to get the data.
  -----------------------------------------------------*/
  
  /* Flash specific "read_image_hdr" function that reads Image header based
   * on flash layout
   */
  dbl_error_type (*read_image_hdr)
  (
    const dbl_flash_device_driver_type          *,
    mi_boot_image_header_type                   *,
    const dbl_flash_partition_image_info_type   *
  );

  /* Flash specific "read_image" function that reads Image header based
   * on flash layout
   */
  dbl_error_type (*read_image)
  (
    const dbl_flash_device_driver_type          *,
    const mi_boot_image_header_type             *,
    const dbl_flash_partition_image_info_type   *,
    const uint32
  );

  /*---------------------------------------------------
    Flash Device can either support
    a. partition layout
    b. non partition layout
    depending on flash device one the following function will
    be initialized.
      i> find_partition_table
            or
      i> get_image_info
  -----------------------------------------------------*/
  /* Flash specific "find_partition_table" function that finds partition table
   * in the given flash.
   */
  void *(*find_partition_table)
  (
    const dbl_flash_device_driver_type *
  );

  /* Flash specific "get_image_info" function that returns image related info
   * for a given flash.
   */
  dbl_error_type (*get_image_info)
  (
    const dbl_flash_device_driver_type    *,
    const image_type                      *,
    dbl_flash_partition_image_info_type   *
  );

  /*---------------------------------------------------
    END of partition or non-partition format.
  -----------------------------------------------------*/

  /* Flash specific "share_data" function that will share DBL's data with next
   * image.
   */
  void (*share_data)
  (
    const dbl_flash_device_driver_type          *,
    dbl_if_device_configuration_info_type       *
  );
  
  union
  {
    dbl_flash_nand_device_info_type  nand;
    dbl_flash_nor_device_info_type   nor;
  }dev_info;

};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif  /* DBL_FLASH_IF_H */

