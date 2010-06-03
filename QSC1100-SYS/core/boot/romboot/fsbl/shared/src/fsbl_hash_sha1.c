/*=============================================================================

                  F A I L  S A F E  B o o t  L o a d e r
                      S H A - 1  H a s h  M o d u l e

GENERAL DESCRIPTION
  This header file contains the implementation  definition for the
  FSBL SHA-1 hash module.
  
EXTERNALIZED FUNCTIONS
  fsbl_hash_sha1_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/sbl/fsbl/1_0/fsbl_hash_sha1.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/30/07   ANB     Initial version.           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "fsbl_hash_sha1.h"
#include <string.h> 
#include "miprogressive.h"
#include "secboot_api.h"
#include "boot_flash_trans_if.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

#if (SECBOOT_SIGNATURE_SIZE != MI_PROG_BOOT_DIGEST_SIZE)
#error SECBOOT_SIGNATURE_SIZE does not match MI_PROG_BOOT_DIGEST_SIZE.
#endif

typedef struct fsbl_hash_sha1_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  boot_hash_if_type  pub_if;

  /* Hashing module data */

  /* Buffer to store result of verify_hash operations. */
  uint8 verify_digest_buf[SECBOOT_SIGNATURE_SIZE];

  /* Store cumulative error indicator for verify_hash operations. */
  boolean verify_error;
  
} fsbl_hash_sha1_type;


/* The private hash module structure */
static fsbl_hash_sha1_type fsbl_hash_priv;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/

/*=========================================================================

FUNCTION  FSBL_HASH_SHA1_COMPUTE_HASH

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
static boolean fsbl_hash_sha1_compute_hash
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

} /* fsbl_hash_sha1_compute_hash() */

/*=========================================================================

FUNCTION  FSBL_HASH_SHA1_PRE_VERIFY_HASH

DESCRIPTION
  Initializes the hashing routines before the verify_hash operation is used.

DEPENDENCIES
  This function must be called before the verify_hash function is called,
  and before fsbl_hash_sha1_flash_copy_callback could be called due a flash
  copy operation.

RETURN VALUE
  TRUE if no errors were encountered, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean fsbl_hash_sha1_pre_verify_hash
(
  const void *buf_ptr,                /* Buffer that will be hashed */
  uint32 buf_size                     /* Size in bytes that will be hashed */
)
{
  /* Reset the error status. */
  fsbl_hash_priv.verify_error = FALSE;

  secboot_init_SHA1_api( buf_size, (unsigned char*) buf_ptr );

  return TRUE;
} /* fsbl_hash_sha1_pre_verify_hash() */

/*=========================================================================

FUNCTION  FSBL_HASH_FLASH_COPY_CALLBACK

DESCRIPTION
  This function is a callback that gets called from the flash translation
  module at regular intervals, to process smaller parts of an image as they
  are loaded from flash, rather than hashing the whole image after it
  is loaded.

DEPENDENCIES
  fsbl_hash_pre_verify_hash() must have been called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void fsbl_hash_flash_copy_callback
(
  void *callback_data,
  void *ram_addr,
  uint32 ram_size
)
{
  /* Cast the callback data to our private hash module type. */
  fsbl_hash_sha1_type * const hash_priv = (fsbl_hash_sha1_type*) callback_data;
  
  /* verify our input parameters */
  if ( hash_priv == NULL )
  {
    return;
  }

  if ( ! secboot_SHA1_hash_and_load_api( (uint8*) ram_addr, 
                                         ram_size, 
                                         FALSE,
                                         hash_priv->verify_digest_buf ) )
  {
    /* Set the error flag if there was an error performing the hashing. */
    hash_priv->verify_error = TRUE;
  }
} /* fsbl_hash_flash_copy_callback() */

/*=========================================================================

FUNCTION  FSBL_HASH_SHA1_VERIFY_HASH

DESCRIPTION
  Function to compute the hash of a region of data and compare it against
  the expected hash value.  Uses any intermediate hashing state generated
  by the callback fsbl_hash_sha1_flash_copy_callback(), or hashes the
  entire buffer range if the callback wasn't used.

DEPENDENCIES
  fsbl_hash_sha1_pre_verify_hash() must have been called first.
  
RETURN VALUE
  TRUE if the hash compared successfully and no errors were encountered,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static boolean fsbl_hash_sha1_verify_hash
(
  const void *buf_ptr,             /* Buffer to hash */
  uint32 buf_size,                 /* Size in bytes of the buffer to hash */
  const void *expected_digest_ptr  /* Expected hash digest of buffer */
)
{
  /* verify our input parameters and validate the active hashing module */
  if ( expected_digest_ptr == NULL )
  {
    return FALSE;
  }


  if ( ! secboot_SHA1_hash_and_load_api( (uint8*) buf_ptr, 
                                         buf_size, 
                                         TRUE,
                                         fsbl_hash_priv.verify_digest_buf ) )
  {
    /* Set the error flag if there was an error performing the hashing. */
    fsbl_hash_priv.verify_error = TRUE;
  }

  /* Now compare the result of hash calculation against expected value */
  if ( memcmp( fsbl_hash_priv.verify_digest_buf, 
               expected_digest_ptr, 
               SECBOOT_SIGNATURE_SIZE ) != 0 )
  {
    /* Set the error flag if the computed hash doesn't match the expected hash */
    fsbl_hash_priv.verify_error = TRUE;
  }

  /* Return the result of the hashing operation. */
  if ( fsbl_hash_priv.verify_error )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
} /* fsbl_hash_sha1_verify_hash() */

/*=========================================================================

FUNCTION  FSBL_HASH_INSTALL_HASH_AND_LOAD_CALLBACK

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
static void fsbl_hash_sha1_install_hash_and_load_callback
(
  boot_flash_trans_if_type *trans_if
)
{
  /* Verify the input parameters */
  if ( trans_if == NULL )
  {
    return;
  }

  /* If the flash interface pointer was passed as input, install our
     flash copy callback routine with the flash translation module. */
  (void) boot_flash_trans_set_copy_callback( trans_if, 
                                             fsbl_hash_flash_copy_callback, 
                                             &fsbl_hash_priv.pub_if );
} /* fsbl_hash_install_hash_and_load_callback()() */

/*=========================================================================

FUNCTION  FSBL_HASH_GET_DIGEST_SIZE

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
static uint32 fsbl_hash_sha1_get_digest_size( void )
{
  return SECBOOT_SIGNATURE_SIZE;
} /* fsbl_hash_get_digest_size() */

/*=========================================================================

FUNCTION  FSBL_HASH_SHA1_INIT

DESCRIPTION
  Function to initialize an instance of the FSBL hash module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the public hash interface.

SIDE EFFECTS
  None

===========================================================================*/
boot_hash_if_type * fsbl_hash_sha1_init( void )
{
  static const boot_module_version_type vsn =
  {
    FSBL_HASH_SHA1_NAME,
    BOOT_MODULE_VERSION( BOOT_HASH_IF_VERSION, FSBL_HASH_SHA1_VERSION )
  };
  static boot_hash_vtbl_type vtbl =
  {
    fsbl_hash_sha1_compute_hash,
    fsbl_hash_sha1_verify_hash,
    fsbl_hash_sha1_pre_verify_hash,
    fsbl_hash_sha1_install_hash_and_load_callback,
    fsbl_hash_sha1_get_digest_size
  };


  /* Setup the module version info and virtual function tables */
  fsbl_hash_priv.pub_if.vsn = &vsn;
  fsbl_hash_priv.pub_if.vtbl = &vtbl;

  /* Clear the error indicator */
  fsbl_hash_priv.verify_error = FALSE;

  /* Return a pointer to the public interface */
  return &fsbl_hash_priv.pub_if;
} /* fsbl_hash_sha1_init() */

