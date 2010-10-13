/*=============================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                               L o a d e r

GENERAL DESCRIPTION
  This module performs binary image and AMSS loading. 

EXTERNALIZED FUNCTIONS
  osbl_load_header_and_image
  osbl_init_amss_image
  osbl_process_amss_debug_flag
  osbl_load_amss_image

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_loader.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/08   MJS     Split AMSS authentication and loading routines.
12/07/07   MJS     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "boot_comdef.h"
#include "osbl.h"
#include "osbl_loader.h"
#include "osbl_flash.h"
#include "boot_sec_elf_loader_if.h"
#include "osbl_error_handler.h"
#include "boot_auth_if.h"
#include "osbl_flash_target.h"
#include "osbl_hw_init.h"
#include "osbl_msm.h"
#include "boot_dbl_if.h"
#include "customer.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* AMSS Translation module interfaces */
static boot_flash_trans_if_type *amss_trans_if;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  OSBL_LOAD_HEADER_AND_IMAGE

DESCRIPTION
  This function loads in the specified image and image header from 
  the boot device to RAM.  The header_ptr parameter is optional, if
  not NULL it will be filled in with the image header data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_load_header_and_image
(
  image_type image_id,
  mi_boot_image_header_type *header_ptr      /* image header       */
)
{
  mi_boot_image_header_type image_header;  /* temporary header */
  boot_flash_trans_if_type *trans_if;      /* translation interface */
  uint32 image_offset = 0;             /* offset to start reading the image */
  boolean success;

  /* Kick the dog before we start */
  OSBL_KICK_WATCHDOG();

  /* Use the local image header structure if the argument is NULL */
  if ( header_ptr == NULL )
  {
    header_ptr = &image_header;
  }

  /* Create a flash translation interface for copying the image
   * from flash to RAM */
  trans_if = boot_flash_dev_open_image( image_id );
  BL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR );

  /* First try to load the image header from the MIBIB.  This will succeed
   * on a NOR non-partition table device, for example. */
  success = boot_flash_dev_load_mibib_header( image_id, header_ptr );

  /* If we couldn't load the header from the MIBIB, assume that the header
   * is located at the base of the image/partition */
  if ( ! success )
  {
    /* Allow the address range of the header structure to be written to */
    boot_clobber_add_local_hole( boot_flash_trans_get_clobber_tbl_ptr( trans_if ),
                                 header_ptr, sizeof(*header_ptr) );

    /* Read the image header from the base of the image/partition */
    success = boot_flash_trans_read( trans_if, header_ptr,
                                     0 /* offset */, sizeof(*header_ptr) );
    BL_VERIFY( success && ( header_ptr->image_id == image_id ), BL_ERR_OTHERS );

    /* Adjust the image offset to start reading the image data following
     * the image header at the base of the image/partition */
    image_offset = sizeof(*header_ptr);
  }

  /* Sanity check the image header. */
  BL_VERIFY( header_ptr->image_size != 0, BL_ERR_IMG_SIZE_ZERO );

  /* Kick the dog before we start */
  OSBL_KICK_WATCHDOG();

  /* Copy the whole image from flash to RAM */
  success = boot_flash_trans_read( trans_if,
                                   header_ptr->image_dest_ptr,
                                   image_offset,
                                   header_ptr->image_size );

  BL_VERIFY( success, BL_ERR_OTHERS );

  /* Release the translation layer resource */
  boot_flash_dev_close_image( &trans_if );

} /* osbl_load_header_and_image() */


/*===========================================================================

FUNCTION OSBL_INIT_AMSS_IMAGE

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  
SIDE EFFECTS

===========================================================================*/
void osbl_init_amss_image( bl_shared_data_type *bl_shared_data )
{
  /* The custom AMSS partition name, if the customer has one */
  char *amss_cust_parti_name;

  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->boot_dev_shared_data != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /* Retrieve the customer's custom AMSS partition name, if they don't
   * use the default */
  amss_cust_parti_name = osbl_target_get_cust_amss_parti_name();

  if ( amss_cust_parti_name != NULL )
  {
    amss_trans_if = boot_flash_dev_open_partition( amss_cust_parti_name );
  }
  else
  {
    /* Initialize the boot device translation interface */
    amss_trans_if = boot_flash_dev_open_image( AMSS_IMG );
  }
  OSBL_VERIFY( amss_trans_if != NULL, BL_ERR_NULL_PTR );

  /* Setup hash while loading for AMSS image */
  boot_hash_install_hash_and_load_callback( amss_trans_if );

  /* Initialize elf loader */
  bl_shared_data->amss_elf_if =
    boot_sec_elf_loader_factory_open( amss_trans_if, OSBL_AMSS_ELF_FORMAT,
                                      bl_shared_data->boot_dev_shared_data->amss_header_offset,
                                      0 );
  BL_VERIFY( bl_shared_data->amss_elf_if != NULL, BL_ERR_NULL_PTR_PASSED );

} /* osbl_init_amss_image() */


/*===========================================================================

FUNCTION OSBL_PROCESS_AMSS_DEBUG_FLAG

DESCRIPTION
  
DEPENDENCIES
  
RETURN VALUE
  
SIDE EFFECTS

===========================================================================*/
void osbl_process_amss_debug_flag( bl_shared_data_type *bl_shared_data )
{
  /* Certificate chain data for retrieving AMSS JTAG debug flag */
  boot_sec_elf_cert_info_type cert_info;
  boot_sec_elf_error_type elf_error;
  boolean success, enable_jtag;


  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->amss_elf_if != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /*-----------------------------------------------------------------------
    Treat hash segment specially; load first so that we can also get
    debug_info filled in and decide on JTAG logic. The hash segment
    is not authenticated at this stage, it will be authenticated later
    when loading AMSS.
    ----------------------------------------------------------------------*/

  /* Get the location and size of the certificate chain */
  elf_error = boot_sec_elf_get_certificate_chain( bl_shared_data->amss_elf_if,
                                                  &cert_info );
  OSBL_VERIFY( elf_error == BOOT_ELF_ERROR_NONE, BL_ERR_NULL_ATTEST_CRT_PTR );

  /* Verify the certificate chain */  
  success = boot_auth_verify_certificate_chain( cert_info.cert_chain_ptr,
                                                cert_info.cert_chain_size );
  OSBL_VERIFY( success, BL_ERR_IMG_SECURITY_FAIL );

  enable_jtag = (boolean) boot_auth_get_value( BOOT_AUTH_PARAM_GET_ENABLE_JTAG );
  osbl_hw_set_jtag_state( enable_jtag );
  
} /* osbl_process_amss_debug_flag() */


/*===========================================================================

FUNCTION OSBL_AUTH_AMSS_IMAGE

DESCRIPTION
  This function authenticates the AMSS image if authentication is enabled.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  This function calls the error handler and does not return if a
  failure occurs.

==========================================================================*/
void osbl_auth_amss_image( bl_shared_data_type *bl_shared_data )
{
  boolean success;
  mi_boot_image_header_type *hdr_ptr;

  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->amss_elf_if != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /* First authenticate the ELF hash segment.  If authentication suceeds or
     is disabled, this function will return TRUE. */
  hdr_ptr = boot_sec_elf_get_hash_header( bl_shared_data->amss_elf_if );
  success = boot_auth_authenticate( hdr_ptr );
  BL_VERIFY( success, BL_ERR_IMG_SECURITY_FAIL );

} /* osbl_auth_amss_image() */

/*===========================================================================

FUNCTION OSBL_LOAD_AMSS_IMAGE

DESCRIPTION
  This function loads the non-paged segments of the amss image and retrieves
  the image entry point.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  This function calls the error handler and does not return if a
  failure occurs.

===========================================================================*/
void osbl_load_amss_image( bl_shared_data_type *bl_shared_data )
{
  boolean enable_hash;
  boot_elf_error_type elf_error;

  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->amss_elf_if != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /* Determine if we should enable hash checking and inform ELF loader. */

  /* First see if authentication is enabled, should always verify the hash
   * table if authentication is enabled */
  enable_hash = (boolean) boot_auth_get_value( BOOT_AUTH_PARAM_GET_SECURE_MODE );

  /* If authentication is not enabled, then see if the target always
   * requires hash checking */
  if ( ! enable_hash )
  {
    enable_hash = osbl_is_amss_hash_checking_required();
  }

  (void)boot_sec_elf_enable_hash_checking( bl_shared_data->amss_elf_if,
                                           enable_hash );

  /* Load all non-paged ELF segments. */
  elf_error = boot_sec_elf_load_filtered_segments( bl_shared_data->amss_elf_if,
                                                   elf_segment_filter_qc_default );
  BL_VERIFY( elf_error == BOOT_ELF_ERROR_NONE, BL_ERR_OTHERS );

  /* Retrieve ELF entry point. */
  bl_shared_data->amss_entry_ptr =
    (amss_entry_ptr_type) boot_sec_elf_get_entry_point( bl_shared_data->amss_elf_if );

  /* !!! NOTE: Don't relese the ELF loader or translation layer so that AMSS
   * ELF data can be used later such as by the prog boot routines */
  /* Release the ELF and translation interfaces */
  //boot_sec_elf_loader_factory_close( &bl_shared_data->amss_elf_if );
  //boot_flash_dev_close_image( &amss_trans_if );

} /* osbl_load_amss_image() */

