#ifndef BOOT_AUTH_IF_H
#define BOOT_AUTH_IF_H

/*===========================================================================

                          B o o t  L o a d e r
       A u t h e n t i c a t i o n  M o d u l e  I n t e r f a c e
                          H e a d e r  F i l e

DESCRIPTION
  This header file contains the definition of the public interface to
  the boot authentication module.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/shared/1_0/boot_auth_if.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Reworked for Secure Boot 2.0.
02/06/07   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "miheader.h"
#include "boot_module_base.h"
#include "boot_singleton.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

#define BOOT_AUTH_IF_VERSION 1


typedef enum
{
  BOOT_AUTH_PARAM_GET_ENABLE_JTAG,
  BOOT_AUTH_PARAM_GET_SECURE_MODE,
  BOOT_AUTH_PARAM_GET_OEM_ID,
  BOOT_AUTH_MAX_PARAM_TYPE = 0x7FFFFFFF     /* To ensure it's 32 bits wide */
} boot_auth_param_enum_type;


/* Definition of the authentication module virtual table structure */
typedef struct boot_auth_vtbl_type
{
  /*=========================================================================

  FUNCTION POINTER  VERIFY_CERTIFICATE_CHAIN

  DESCRIPTION
    Function to validate the certificate chain of an image (and perform
    implementation-specific tasks such as retrieving the debug flag)

  DEPENDENCIES
    None
  
  RETURN VALUE
    TRUE if the verification was successful or authentication was disabled,
    FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*verify_certificate_chain)
  (
    void *cert_chain_ptr,               /* Pointer to the location of the
                                           certificate chain */
    uint32 cert_chain_size              /* Size of the certificate chain */
  );

  /*=========================================================================

  FUNCTION POINTER  AUTHENTICATE

  DESCRIPTION
    Function to authenticate an image given the image header.

  DEPENDENCIES
    None

  RETURN VALUE
    TRUE if authentication was successful or authentication was disabled,
    FALSE otherwise

  SIDE EFFECTS
    None

  ===========================================================================*/
  boolean (*authenticate)
  (
    mi_boot_image_header_type *header_ptr  /* Pointer to the image header  */
  );

  /*=========================================================================

  FUNCTION POINTER  GET_VALUE

  DESCRIPTION
    Function to get an implementation-specific value corresponding to
    the given implementation-specific value of param.

  DEPENDENCIES
    None

  RETURN VALUE
    The implementation-specific value corresponding to param.

  SIDE EFFECTS
    None

  ===========================================================================*/
  uint32 (*get_value)
  (
    boot_auth_param_enum_type param      /* Identifier specifying what data
                                            to return                        */
  );

} boot_auth_vtbl_type;


/* Definition of public interface to boot authentication module */
typedef struct boot_auth_if_type
{
  /* The module version structure */
  const boot_module_version_type *vsn;

  /* The module virtual function table */
  boot_auth_vtbl_type *vtbl;

} boot_auth_if_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* Declare the singleton, do it here so we can access the singleton below */
BOOT_SINGLETON_DECLARE(boot_auth_if);

/*=========================================================================

                           PUBLIC MODULE API

===========================================================================*/

/*=========================================================================

FUNCTION  BOOT_AUTH_VERIFY_CERTIFICATE_CHAIN

DESCRIPTION
  Function to validate the certificate chain of an image (and perform
  implementation-specific tasks such as retrieving the debug flag)

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the verification was successful or authentication was disabled,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_auth_verify_certificate_chain
(
  void *cert_chain_ptr,               /* Pointer to the location of the
                                         certificate chain */
  uint32 cert_chain_size              /* Size of the certificate chain */
)
{
  return boot_auth_if_get_singleton()->vtbl->verify_certificate_chain( cert_chain_ptr, cert_chain_size );
} /* boot_auth_verify_certificate_chain() */

/*=========================================================================

FUNCTION  BOOT_AUTH_AUTHENTICATE

DESCRIPTION
  Function to authenticate an image given the image header.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if authentication was successful or authentication was disabled,
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
__inline boolean boot_auth_authenticate
(
  mi_boot_image_header_type *header_ptr  /* Pointer to the image header  */
)
{
  return boot_auth_if_get_singleton()->vtbl->authenticate( header_ptr );
} /* boot_auth_authenticate() */

/*=========================================================================

FUNCTION  BOOT_AUTH_GET_VALUE

DESCRIPTION
  Function to get an implementation-specific value corresponding to
  the given implementation-specific value of param.

DEPENDENCIES
  None

RETURN VALUE
  The implementation-specific value corresponding to param.

SIDE EFFECTS
  None

===========================================================================*/
__inline uint32 boot_auth_get_value
(
  boot_auth_param_enum_type param      /* Identifier specifying what data
                                          to return                        */
)
{
  return boot_auth_if_get_singleton()->vtbl->get_value( param );
} /* boot_auth_get_value() */


#endif /* BOOT_AUTH_IF_H */

