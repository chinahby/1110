/*=============================================================================

                      D e v i c e  B o o t  L o a d e r

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
  dbl_load_fsbl_image
  dbl_load_osbl_image

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007, 2008 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_loader.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/07/08   MJS     Always load the partition entry for NAND even if DBL header
                   exists.
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "dbl_loader.h"
#include "dbl_flash.h"
#include "dbl_error_handler.h"
#include "dbl_mc.h"
#include "dbl_partition.h"
#include "dbl_auth.h"
#include "dbl_fsbl_handler.h"
#include "boot_clobber_prot.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/////////////////////////////////////////////////
/*---------------------------------------------------------------------------
  Defines the data structure to verify the good block where SBL image sits.
---------------------------------------------------------------------------*/
#define MI_SBL_MAGIC1          0xFA0F129C
#define MI_SBL_MAGIC2          0x5A8Fb6C9

#define MI_FSBL_MAGIC1         0x5FE85DDF
#define MI_FSBL_MAGIC2         0x5264CEBC
////////////////////////////////////////////////

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_LOAD_HDR_AND_IMAGE

DESCRIPTION
  This function interfaces with the flash functions to load the Image for
  execution. This function:
    Loads the header
    Loads the image from location in flash to location in memory 
      specified by the DBL header.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static dbl_error_type dbl_load_hdr_and_image
(
  dbl_shared_data_type *shared_data,
  image_type           img_type,          /* image type */
  char                 *name,             /* id to locate the 
                                             entry of partition table   */
  uint32               magic_number1,     /* Magic number for the image */
  uint32               magic_number2      /* Magic number for the image */
                                             
)
{
  dbl_flash_partition_image_info_type    partition_hdr;
  static mi_boot_image_header_type       image_hdr;
  dbl_error_type                         ret_type;
  boot_sbl_header_type                   *dbl_image_hdr_ptr;
  uint32                                 image_offset;

  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);

  if (shared_data->sys_partition_pointer != NULL)
  {
    /* Device Support Partition Layout */
    
    /* Find out the entry in partition table based on ID */
    dbl_partition_load_entry( (flash_partable_t)shared_data->sys_partition_pointer,
                              &partition_hdr,
                              name );
  
    /* Store IMAGE Information */
    image_hdr.image_id   = img_type;
    partition_hdr.magic1 = magic_number1;
    partition_hdr.magic2 = magic_number2;

    image_offset = sizeof(mi_boot_image_header_type);
  }
  else
  {
    /* Device Does NOT Support Partition Layout */

    /* Store IMAGE Information */
    image_hdr.image_id   = img_type;
      
    partition_hdr.src    = NULL;
    partition_hdr.length = 0;
    partition_hdr.magic1 = magic_number1;
    partition_hdr.magic2 = magic_number2;      

    /* Since the flash device does not support partition layout get the 
     ** image related information by calling target specific function.
     */  
    dbl_flash_get_image_info( shared_data,
                              &img_type,
                              &partition_hdr );
    image_offset = 0;
  }

  /* If the PBL shares DBL header with DBL, use it to load FSBL image. 
     Otherwise, read the FSBL header from Flash */
  if( (img_type == FSBL_IMG) && 
      (dbl_get_dbl_hdr_ptr_if_shared_by_pbl( &dbl_image_hdr_ptr ) == TRUE) )
  {
    /* Create FSBL image header from DBL image header */
    image_hdr.image_id = img_type;
    image_hdr.image_src = dbl_image_hdr_ptr->fsbl_src;
    image_hdr.image_dest_ptr = dbl_image_hdr_ptr->fsbl_dest_ptr;
    image_hdr.image_size = dbl_image_hdr_ptr->fsbl_image_size;
    image_hdr.code_size = dbl_image_hdr_ptr->fsbl_image_size;
    image_offset = 0;
  }
  else
  {  
    /* Get the image header from flash */
    ret_type = dbl_flash_read_image_hdr( shared_data, 
                                         &image_hdr,
                                         &partition_hdr );
    DBL_VERIFY_RET( (ret_type == DBL_ERR_NONE), 
                    DBL_ERR_FAILED_TO_COPY_IMG_HDR );
  }
  
  /* Check header */
  if (image_hdr.image_id != img_type)
  {
    return DBL_ERR_IMG_TYPE_INVALID;
  }
  
  if (image_hdr.image_size == 0)
  {
    return DBL_ERR_IMG_SIZE_ZERO;
  }

  /* Validate the image destination address range */
  if ( ! boot_clobber_check_global_address_range( image_hdr.image_dest_ptr,
                                                  image_hdr.image_size ) )
  {
    return DBL_ERR_IMG_CLOBBER_FAILURE;
  }

  /* Found valid Image header, Start the read process to load the image. */
  ret_type = dbl_flash_read_image( shared_data, 
                                   &image_hdr, 
                                   &partition_hdr,
                                   image_offset );
  DBL_VERIFY_RET( (ret_type == DBL_ERR_NONE), 
                  DBL_ERR_FAILED_TO_COPY_IMG ); 
    
  /* Loading is done, queue for authentication */
  dbl_auth_queue_image_for_auth( shared_data->auth_info,  /* DBL's shared data  */
                                 &image_hdr );

  /* Image was loaded successfully. */    
  shared_data->entry_ptr_init = TRUE;
  shared_data->entry_ptr      = (void (*)(void*))(image_hdr.image_dest_ptr);

  return DBL_ERR_NONE;
}/* dbl_load_hdr_and_image */

/*===========================================================================

FUNCTION  DBL_LOAD_IMAGE

DESCRIPTION
  This function interfaces with the flash functions to load the Image for
  execution. This function:
    Loads the header
    Loads the image from location in flash to location in memory 
      specified by the DBL header.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static dbl_error_type dbl_load_image
(
  dbl_shared_data_type *shared_data,
  image_type           img_type
)
{

  dbl_error_type ret_value = DBL_ERR_NONE;
  
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  

  switch(img_type)
  {
    case FSBL_IMG:
      shared_data->dbl_state = DBL_STATE_LOAD_FSBL_IMG;
      ret_value = dbl_load_hdr_and_image(  shared_data,img_type,
                                           MIBIB_FSBL_PARTI_NAME,
                                           MI_FSBL_MAGIC1,
                                           MI_FSBL_MAGIC2
                                        );
      break;

    case OSBL_IMG:
      shared_data->dbl_state = DBL_STATE_LOAD_OSBL_IMG;
      ret_value = dbl_load_hdr_and_image(  shared_data,img_type,
                                           MIBIB_OSBL_PARTI_NAME,
                                           MI_SBL_MAGIC1,
                                           MI_SBL_MAGIC2
                                        );
      break;

    default:
      DBL_ERR_FATAL((uint32)DBL_ERR_IMG_TYPE_INVALID);
  }
  
  return ret_value;
}/* dbl_load_image */


/*===========================================================================

FUNCTION  DBL_LOAD_FSBL_IMAGE

DESCRIPTION
  This function interfaces with the flash functions to load the Image for
  execution. This function:
    Loads the header
    Loads the image from location in flash to location in memory 
      specified by the DBL header.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void dbl_load_fsbl_image
(
  dbl_shared_data_type *shared_data
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;

  ret_value = dbl_load_image(shared_data, FSBL_IMG);
  
  /*------------------------------------------------------------------
   FSBL has been loaded verfiy the hash and call its INIT routine.
  ------------------------------------------------------------------*/
  if (ret_value == DBL_ERR_NONE)
  {
    ret_value = dbl_fsbl_init(shared_data);
  }

} /* dbl_load_fsbl_image */

/*===========================================================================

FUNCTION  DBL_LOAD_OSBL_IMAGE

DESCRIPTION
  This function interfaces with the flash functions to load the Image for
  execution. This function:
    Loads the header
    Loads the image from location in flash to location in memory 
      specified by the DBL header.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void dbl_load_osbl_image
(
  dbl_shared_data_type *shared_data
)
{
  dbl_error_type ret_value = DBL_ERR_NONE;

  ret_value = dbl_load_image(shared_data, OSBL_IMG);
  DBL_VERIFY((ret_value == DBL_ERR_NONE),  (uint32)ret_value);

} /* dbl_load_osbl_image */

