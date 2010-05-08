#ifndef BOOT_HASH_IF_H
#define BOOT_HASH_IF_H

/*===========================================================================

                          B o o t  L o a d e r
                 H a s h  M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the boot hash module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_hash_if.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_module_base.h"
#include "boot_singleton.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_HASH_IF_VERSION 1


/* Forward declartions */
struct boot_flash_trans_if_type;


/* Definition of the hashing module virtual function table */
typedef struct boot_hash_vtbl_type
{
  /*=========================================================================

  FUNCTION POINTER  COMPUTE_HASH

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
  boolean (*compute_hash)
  (
    const void *buf_ptr,                /* Buffer to hash */
    uint32 buf_size,                    /* Size in bytes of the buffer to hash */
    void *digest_buf_ptr                /* Location to store hash digest */
  );

  /*=========================================================================

  FUNCTION POINTER  VERIFY_HASH

  DESCRIPTION
    Function to compute the hash of a region of data and compare it against
    the expected hash value.  This function may queue the hashing operation
    and return immediately if the verify_hash implementation is non-blocking.

  DEPENDENCIES
    The function pre_verify_hash must be called first.

  RETURN VALUE
    TRUE if the hash compared successfully and no errors were encountered,
    FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*verify_hash)
  (
    const void *buf_ptr,                /* Buffer to hash */
    uint32 buf_size,                    /* Size in bytes of the buffer to hash */
    const void *expected_digest_ptr     /* Expected hash digest of buffer */
  );

  /*=========================================================================

  FUNCTION POINTER  PRE_VERIFY_HASH

  DESCRIPTION
    Function that should be called to initialize the hash module for the
    verify_hash operation, for computing the hash of a region of data and
    comparing it against an expected hash value.

  DEPENDENCIES
    This function must be called before the verify_hash function is called,
    and before any potential callbacks into an implementation of the hash
    module could be called.

  RETURN VALUE
    TRUE if no errors were encountered, FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*pre_verify_hash)
  (
    const void *buf_ptr,                /* Buffer that will be hashed */
    uint32 buf_size                     /* Size in bytes that will be hashed */
  );

  /*=========================================================================

  FUNCTION POINTER  INSTALL_HASH_AND_LOAD_CALLBACK

  DESCRIPTION
    Installs the flash copy callback for the hashing module to enable
    the hash-while-loading feature.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void (*install_hash_and_load_callback)
  (
    struct boot_flash_trans_if_type *trans_if
  );
  
  /*=========================================================================

  FUNCTION POINTER  GET_DIGEST_SIZE

  DESCRIPTION
    Gets the size of the hash message digest.

  DEPENDENCIES
    None

  RETURN VALUE
    Message digest size in bytes.

  SIDE EFFECTS
    None

  ===========================================================================*/
  uint32 (*get_digest_size)( void );

} boot_hash_vtbl_type;


/* Definition of public interface to hash module */
typedef struct boot_hash_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_hash_vtbl_type *vtbl;

} boot_hash_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Must declare the singleton before we can reference the get_singleton()
 * functions in the below inline functions */
BOOT_SINGLETON_DECLARE(boot_hash_if);


/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_HASH_COMPUTE_HASH

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
__inline boolean boot_hash_compute_hash
( 
  const void *buf_ptr,                /* Buffer to hash */
  uint32 buf_size,                    /* Size in bytes of the buffer to hash */
  void *digest_buf_ptr                /* Location to store hash digest */
)
{
  return boot_hash_if_get_singleton()->vtbl->compute_hash( buf_ptr, buf_size, digest_buf_ptr );
} /* boot_hash_compute_hash() */

/*=========================================================================

FUNCTION  BOOT_HASH_VERIFY_HASH

DESCRIPTION
  Function to compute the hash of a region of data and compare it against
  the expected hash value.  This function may queue the hashing operation
  and return immediately if the verify_hash implementation is non-blocking.

DEPENDENCIES
  The function pre_verify_hash must be called first.

RETURN VALUE
  TRUE if the hash compared successfully and no errors were encountered,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_hash_verify_hash
(
  const void *buf_ptr,                /* Buffer to hash */
  uint32 buf_size,                    /* Size in bytes of the buffer to hash */
  const void *expected_digest_ptr     /* Expected hash digest of buffer */
)
{
  return boot_hash_if_get_singleton()->vtbl->verify_hash( buf_ptr, buf_size, expected_digest_ptr );
} /* boot_hash_verify_hash() */

/*=========================================================================

FUNCTION  BOOT_HASH_PRE_VERIFY_HASH

DESCRIPTION
  Function that should be called to initialize the hash module for the
  verify_hash operation, for computing the hash of a region of data and
  comparing it against an expected hash value.

DEPENDENCIES
  This function must be called before the verify_hash function is called,
  and before any potential callbacks into an implementation of the hash
  module could be called.

RETURN VALUE
  TRUE if no errors were encountered, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_hash_pre_verify_hash
(
  const void *buf_ptr,                /* Buffer that will be hashed */
  uint32 buf_size                     /* Size in bytes that will be hashed */
)
{
  return boot_hash_if_get_singleton()->vtbl->pre_verify_hash( buf_ptr, buf_size );
} /* boot_hash_pre_verify_hash() */

/*=========================================================================

FUNCTION  BOOT_HASH_INSTALL_HASH_AND_LOAD_CALLBACK

DESCRIPTION
  Installs the flash copy callback for the hashing module to enable
  the hash-while-loading feature.

 DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
__inline void boot_hash_install_hash_and_load_callback
(
  struct boot_flash_trans_if_type *trans_if
)
{
  boot_hash_if_get_singleton()->vtbl->install_hash_and_load_callback( trans_if );
} /* boot_hash_install_hash_and_load_callback() */

/*=========================================================================

FUNCTION  BOOT_HASH_GET_DIGEST_SIZE

DESCRIPTION
  Gets the size of the hash message digest.

DEPENDENCIES
  None

RETURN VALUE
  Message digest size in bytes.

SIDE EFFECTS
  None

===========================================================================*/
__inline uint32 boot_hash_get_digest_size( void )
{
  return boot_hash_if_get_singleton()->vtbl->get_digest_size();
} /* boot_hash_get_digest_size() */


#endif /* BOOT_HASH_IF_H */

