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

  $Header: //depot/asic/msmshared/secboot2/sbl/dbl/1_0/dbl_auth.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/07   anb     Initial revision.
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "boot_comdef.h"
#include "dbl_auth.h"
#include "dbl_error_handler.h"
#include "dbl_target_accessor.h"
#include "dbl_mc.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION  DBL_AUTH_IMAGE

DESCRIPTION
  This function authenticates the image that has been "queued" for
  authentication. The image is authenticated only if authentication is
  enabled. If authentication functions fail, then this function does
  not return and goes to the error handler.

  The image contains the following in the order given:
    DBL code
    signature
    certificate chain

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dbl_auth_image
(
  dbl_shared_data_type *shared_data   /* Shared data         */ 
)
{
  dbl_auth_info_type   *auth_info;
  boolean success;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  
  shared_data->dbl_state = DBL_STATE_AUTH_IMG;
  
  auth_info = shared_data->auth_info;
  DBL_VERIFY((auth_info != NULL), (uint32)DBL_ERR_NULL_PTR);  
  
  if (dbl_is_auth_enabled())
  {
    /* Authenticate Image. */
    success = auth_info->auth_if->vtbl->authenticate( auth_info->image_header );
    DBL_VERIFY( success, (uint32) DBL_ERR_IMG_SECURITY_FAIL );
  } 
} /* dbl_auth_image */

/*===========================================================================

FUNCTION  DBL_AUTH_IMAGE

DESCRIPTION
  This function calculates hash of the image that has been "queued" for
  authentication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dbl_auth_calc_hash
(
  const dbl_shared_data_type *shared_data,   /* Shared data         */ 
  uint8                      *digest_ptr
)
{
  dbl_auth_info_type   *auth_info;
  mi_boot_image_header_type *image_header;
  
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  

  auth_info = shared_data->auth_info;
  DBL_VERIFY((auth_info != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((auth_info->image_header != NULL), (uint32)DBL_ERR_NULL_IMG_HDR_PTR);
  image_header = auth_info->image_header;
  
  if (shared_data->fsbl_init_called == FALSE)
  {
    dbl_sha1_hash( image_header->image_dest_ptr,
                   image_header->code_size,
                   digest_ptr
                 );
  }
}/* dbl_auth_calc_hash */

/*===========================================================================

FUNCTION  DBL_AUTH_QUEUE_IMAGE_FOR_AUTH

DESCRIPTION
  This function "queues" an image for authentication at a later time in the
  normal cold boot process. The "queuing" is simply to save information
  about the image to be authenticated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_auth_queue_image_for_auth
(
  dbl_auth_info_type        *auth_ptr,
  mi_boot_image_header_type *image_header
)
{
  DBL_VERIFY((auth_ptr != NULL), (uint32)DBL_ERR_NULL_PTR);
  DBL_VERIFY((image_header != NULL), (uint32)DBL_ERR_NULL_IMG_HDR_PTR);
  auth_ptr->image_header = image_header;
} /* dbl_auth_queue_image_for_auth */

/*===========================================================================

FUNCTION  DBL_AUTH_UPDATE_HASH_AUTH_IF

DESCRIPTION
  This function updates hash and authentication interface routines.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_auth_update_hash_auth_if
(
  const dbl_shared_data_type  *shared_data,   /* Shared data         */ 
  boot_hash_if_type     *hash_if,       
  boot_auth_if_type     *auth_if  
)
{
  dbl_auth_info_type   *auth_info;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  
  DBL_VERIFY((hash_if != NULL), (uint32)DBL_ERR_NULL_HASH_IF_PTR);
  DBL_VERIFY((auth_if != NULL), (uint32)DBL_ERR_NULL_AUTH_IF_PTR);
  
  auth_info = shared_data->auth_info;
  DBL_VERIFY((auth_info != NULL), (uint32)DBL_ERR_NULL_PTR);  
  
  auth_info->hash_if = hash_if;
  auth_info->auth_if = auth_if;  
} /* dbl_auth_update_hash_auth_if */

/*===========================================================================

FUNCTION  DBL_AUTH_INIT

DESCRIPTION
  This function updates the DBL structure to point to correct auth interface
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dbl_auth_init
(
  dbl_shared_data_type *shared_data   /* Shared data         */ 
)
{
  static dbl_auth_info_type   auth_info;
  DBL_VERIFY((shared_data != NULL), (uint32)DBL_ERR_NULL_PTR);  

  /*------------------------------------------------------------------
   Verify that the Authentication data has not been modified.
  ------------------------------------------------------------------*/
  dbl_verify_pbl_data();

  /*------------------------------------------------------------------
   Update local variable.
  ------------------------------------------------------------------*/
  shared_data->auth_info = &auth_info;
  auth_info.image_header = NULL;
  auth_info.hash_if = NULL;
  auth_info.auth_if = NULL;  
} /* dbl_auth_init */

