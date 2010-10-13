/*=============================================================================

               OS  S e c o n d a r y   B o o t   L o a d e r
                      S H A - 1  H a s h  M o d u l e

GENERAL DESCRIPTION
  This header file contains the implementation  definition for the
  OSBL SHA-1 hash module.
  
EXTERNALIZED FUNCTIONS
  osbl_hash_init_and_register

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_hash.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/07/07   MJS     Initial revision, ported from MDM7800 and QSD8650.

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h> 
#include "osbl_hash.h"
#include "miprogressive.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#if (SECBOOT_SIGNATURE_SIZE != MI_PROG_BOOT_DIGEST_SIZE)
#error SECBOOT_SIGNATURE_SIZE does not match MI_PROG_BOOT_DIGEST_SIZE.
#endif


/* Definition of private OSBL standard hashing module data structure */
typedef struct osbl_hash_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  boot_hash_if_type  pub_if;

  /* Hashing module data */

  /* Store cumulative error indicator for verify_hash operations. */
  boolean verify_error;

  /* Buffer to store result of verify_hash operations. */
  uint8 verify_digest_buf[SECBOOT_SIGNATURE_SIZE];

} osbl_hash_private_type;


static osbl_hash_private_type osbl_hash_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

FUNCTION  OSBL_HASH_COMPUTE_HASH

DESCRIPTION
  Function to compute the hash of a region of data and store the hash
  in the buffer location provided.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the operation was successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean osbl_hash_compute_hash
(
  const void *buf_ptr,         /* Buffer to hash */
  uint32 buf_size,             /* Size in bytes of the buffer to hash */
  void *digest_buf_ptr         /* Location to store hash digest */
)
{
  /* verify our input parameters */
  if ( digest_buf_ptr == NULL )
  {
    return FALSE;
  }

  return ( secboot_SHA1_api ( (uint8*) buf_ptr, 
                               buf_size,
                               (uint8*) digest_buf_ptr
                            )
         );
} /* osbl_hash_compute_hash() */

/*=========================================================================

FUNCTION  OSBL_HASH_PRE_VERIFY_HASH

DESCRIPTION
  Initializes the hashing routines before the verify_hash operation is used.

DEPENDENCIES
  This function must be called before the verify_hash function is called,
  and before osbl_hash_flash_copy_callback could be called due a flash
  copy operation.

RETURN VALUE
  TRUE if no errors were encountered, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean osbl_hash_pre_verify_hash
(
  const void *buf_ptr,                /* Buffer that will be hashed */
  uint32 buf_size                     /* Size in bytes that will be hashed */
)
{
  /* Reset the error status. */
  osbl_hash_priv.verify_error = FALSE;

  secboot_init_SHA1_api( buf_size, (unsigned char*) buf_ptr );

  return TRUE;
} /* osbl_hash_pre_verify_hash() */

/*=========================================================================

FUNCTION  OSBL_HASH_FLASH_COPY_CALLBACK

DESCRIPTION
  This function is a callback that gets called from the flash translation
  module at regular intervals, to process smaller parts of an image as they
  are loaded from flash, rather than hashing the whole image after it
  is loaded.

DEPENDENCIES
  osbl_hash_pre_verify_hash() must have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_hash_flash_copy_callback
(
  void *callback_data,
  void *ram_addr,
  uint32 ram_size
)
{
  /* Cast the callback data to our private hash module type. */
  osbl_hash_private_type * const hash_priv = (osbl_hash_private_type*) callback_data;

  /* verify our input parameters */
  OSBL_VERIFY( hash_priv != NULL, BL_ERR_NULL_PTR_PASSED );

  if ( ! secboot_SHA1_hash_and_load_api( (uint8*) ram_addr, 
                                         ram_size, 
                                         FALSE,
                                         hash_priv->verify_digest_buf ) )
  {
    /* Set the error flag if there was an error performing the hashing. */
    hash_priv->verify_error = TRUE;
  }
} /* osbl_hash_flash_copy_callback() */

/*=========================================================================

FUNCTION  OSBL_HASH_VERIFY_HASH

DESCRIPTION
  Function to compute the hash of a region of data and compare it against
  the expected hash value.  Uses any intermediate hashing state generated
  by the callback osbl_hash_flash_copy_callback(), or hashes the
  entire buffer range if the callback wasn't used.

DEPENDENCIES
  osbl_hash_pre_verify_hash() must have been called first.
  
RETURN VALUE
  TRUE if the hash compared successfully and no errors were encountered,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean osbl_hash_verify_hash
(
  const void *buf_ptr,             /* Buffer to hash */
  uint32 buf_size,                 /* Size in bytes of the buffer to hash */
  const void *expected_digest_ptr  /* Expected hash digest of buffer */
)
{
  /* verify our input parameters */
  if ( expected_digest_ptr == NULL )
  {
    return FALSE;
  }

  if ( ! secboot_SHA1_hash_and_load_api( (uint8*) buf_ptr, 
                                         buf_size, 
                                         TRUE, 
                                         osbl_hash_priv.verify_digest_buf ) )
  {
    /* Set the error flag if there was an error performing the hashing. */
    osbl_hash_priv.verify_error = TRUE;
  }

  /* Now compare the result of hash calculation against expected value */
  if ( memcmp( osbl_hash_priv.verify_digest_buf, 
               expected_digest_ptr, 
               SECBOOT_SIGNATURE_SIZE ) != 0 )
  {
    /* Set the error flag if the computed hash doesn't match the expected hash */
    osbl_hash_priv.verify_error = TRUE;
  }

  /* Return the result of the hashing operation. */
  if ( osbl_hash_priv.verify_error )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* osbl_hash_verify_hash() */

/*=========================================================================

FUNCTION  OSBL_HASH_INSTALL_HASH_AND_LOAD_CALLBACK

DESCRIPTION
  Function to install the hash and load callback for the boot device
  translation layer.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static void osbl_hash_install_hash_and_load_callback
(
  boot_flash_trans_if_type *trans_if
)
{
  OSBL_VERIFY( trans_if != NULL, BL_ERR_NULL_PTR_PASSED );

  /* If the flash interface pointer was passed as input, install our
     flash copy callback routine with the flash translation module. */
  (void) boot_flash_trans_set_copy_callback( trans_if, 
                                             osbl_hash_flash_copy_callback, 
                                             &osbl_hash_priv );
} /* osbl_hash_install_hash_and_load_callback()() */

/*=========================================================================

FUNCTION  OSBL_HASH_GET_DIGEST_SIZE

DESCRIPTION
  Function to return the size of the digest for the implemented hash
  algorithm.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
static uint32 osbl_hash_get_digest_size( void )
{
  return SECBOOT_SIGNATURE_SIZE;
} /* osbl_hash_get_digest_size() */

/*=========================================================================

FUNCTION  OSBL_HASH_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize and register the OSBL hash module.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_hash_init_and_register( void )
{
  static const boot_module_version_type vsn =
  {
    OSBL_HASH_NAME,
    BOOT_MODULE_VERSION( BOOT_HASH_IF_VERSION, OSBL_HASH_VERSION )
  };

  static boot_hash_vtbl_type vtbl =
  {
    osbl_hash_compute_hash,
    osbl_hash_verify_hash,
    osbl_hash_pre_verify_hash,
    osbl_hash_install_hash_and_load_callback,
    osbl_hash_get_digest_size,
  };

  /* Setup the module version structure and virtual function table */
  osbl_hash_priv.pub_if.vsn = &vsn;
  osbl_hash_priv.pub_if.vtbl = &vtbl;

  /* Initialize the private data */
  osbl_hash_priv.verify_error = FALSE;

  boot_hash_if_register_singleton( &osbl_hash_priv.pub_if );

} /* osbl_hash_init_and_register() */

