/*=============================================================================

                OS  S e c o n d a r y   B o o t   L o a d e r
                  A u t h e n t i c a t i o n  M o d u l e

GENERAL DESCRIPTION
  This header file contains the implementation for the OSBL X.509
  authentication module.

EXTERNALIZED FUNCTIONS
  osbl_auth_init_and_register

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_auth.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/09/07   ly      Create           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <string.h>
#include "boot_comdef.h"
#include "osbl_auth.h"
#include "osbl_error_handler.h"
#include "osbl.h"
#include "osbl_msm.h"
#include "osbl_target.h"
#include "secboot_api.h"
#include "boot_pbl_accessor.h"


/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/*----------------------------------- 
 * Public module interface 
 *---------------------------------*/

/* Definition of private OSBL standard authentication module data structure */
typedef struct osbl_auth_private_type
{
  /*----------------------------------- 
   * Public module interface 
   *   Note: This _must_ be the first element of the structure.
   *---------------------------------*/
  boot_auth_if_type pub_if;

  /* Authentication module data */
  pbl_secx509_cert_type *attest_crt_ptr;
  void *last_cert_chain_ptr;
  secboot_dbg_info_type debug_info;

} osbl_auth_private_type;

static osbl_auth_private_type osbl_auth_priv;


/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  OSBL_AUTH_VERIFY_CERTIFICATE_CHAIN

DESCRIPTION
  Function to validate the certificate chain of an image, and retrieve the
  JTAG debug flag from the certificates.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if the certificate chain was successful or authentication is disabled,
  otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
static boolean osbl_auth_verify_certificate_chain
(
  void *cert_chain_ptr,        /* Pointer to the location of the
                                  certificate chain */
  uint32 cert_chain_size       /* Size of the certificate chain */
)
{
  if ( ! boot_pbl_is_auth_enabled() )
  {
    /* Return TRUE if authentication is disabled */
    return TRUE;
  }
  /* Now that we know authentication is enabled, verify the remaining
     input parameters */
  else if ( cert_chain_ptr == NULL || cert_chain_size == 0 )
  {
    return FALSE;
  }
  
  else if ( osbl_auth_priv.attest_crt_ptr != NULL &&
            osbl_auth_priv.last_cert_chain_ptr == cert_chain_ptr )
  {
    /* If attest_crt_ptr is not NULL then we have already verified the certs */
    return TRUE;
  }
  
  /* Kick the dog before we proceed. */
  OSBL_KICK_WATCHDOG(); 

  /* Cache the last cert chain verified, used to test if we have already
     verified this cert chain */
  osbl_auth_priv.last_cert_chain_ptr = cert_chain_ptr;

  /* Call authentication interface defined in secboot using information
     contained within image header.  */
  osbl_auth_priv.attest_crt_ptr =
    secboot_verify_x509_chain_api( cert_chain_ptr, cert_chain_size,
                                   (uint8*) boot_pbl_get_root_cert_ptr(),
                                   NULL, NULL, &osbl_auth_priv.debug_info );

  return ( osbl_auth_priv.attest_crt_ptr != NULL );
} /* osbl_auth_verify_certificate_chain() */

/*=========================================================================

FUNCTION  OSBL_AUTH_X509_AUTHENTICATE

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
static boolean osbl_auth_authenticate
(
  mi_boot_image_header_type *header_ptr  /* Pointer to the image header  */
)
{
  boolean status = FALSE;

  /* Check input parameters. */
  if ( header_ptr == NULL )
  {
    return FALSE;
  }
  else if ( ! boot_pbl_is_auth_enabled() )
  {
    /* If authenticate is disabled then return TRUE */
    return TRUE;
  }
  /* Verify code size is valid. */  
  else if ( header_ptr->code_size == 0 )
  {
    return FALSE;
  }

  /* Kick the dog before we proceed. */
  OSBL_KICK_WATCHDOG(); 

  /* First verify the certificate chain */
  status = osbl_auth_verify_certificate_chain( header_ptr->cert_chain_ptr,
                                               header_ptr->cert_chain_size );
  if ( !status )
  {
    return FALSE;
  }

  /* Kick the dog before we proceed. */
  OSBL_KICK_WATCHDOG(); 

  /* Verify image code signature and return the results. Use the
     global MSM hardware ID variable to do this for now. */

  status = secboot_verify_code_signature_api( header_ptr->image_dest_ptr,
                                              header_ptr->code_size,
                                              boot_pbl_get_msm_hw_id(),
                                              osbl_auth_priv.debug_info.sw_id,
                                              header_ptr->signature_ptr,
                                              header_ptr->signature_size,
                                              osbl_auth_priv.attest_crt_ptr );

  return status;
} /* osbl_auth_authenticate() */

/*=========================================================================

FUNCTION  OSBL_AUTH_GET_VALUE

DESCRIPTION
  Function to get an implementation-specific value corresponding to
  the given implementation-specific value of param.  OSBL module currently
  supports the JTAG debug flag and secure boot mode.

DEPENDENCIES
  None
  
RETURN VALUE
  The implementation-specific value corresponding to param.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 osbl_auth_get_value
(
   boot_auth_param_enum_type param  /* Identifier specifying what data to return */
)
{
  uint32 value;

  switch (param)
  {
  case BOOT_AUTH_PARAM_GET_ENABLE_JTAG:
    /* Just return the JTAG debug flag */
    value = osbl_auth_priv.debug_info.enable_jtag;
    break;

  case BOOT_AUTH_PARAM_GET_SECURE_MODE:
    /* Return status of whether authentication is enabled */
    value = boot_pbl_is_auth_enabled();
    break;

  default:
    value = 0;
    break;
  }
  
  return value;
} /* osbl_auth_get_value() */

/*=========================================================================

FUNCTION  OSBL_AUTH_INIT_AND_REGISTER

DESCRIPTION
  Function to initialize an instance of the OSBL authentication module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the public auth module interface.

SIDE EFFECTS
  None

===========================================================================*/
void osbl_auth_init_and_register( void )
{
  static const boot_module_version_type vsn =
  {
    OSBL_AUTH_NAME,
    BOOT_MODULE_VERSION( BOOT_AUTH_IF_VERSION, OSBL_AUTH_VERSION )
  };
  static boot_auth_vtbl_type vtbl =
  {
    osbl_auth_verify_certificate_chain,
    osbl_auth_authenticate,
    osbl_auth_get_value,
  };

  /* Setup the module version structure and virtual function table */
  osbl_auth_priv.pub_if.vsn = &vsn;
  osbl_auth_priv.pub_if.vtbl = &vtbl;

  /* Initialize the private data */
  osbl_auth_priv.attest_crt_ptr = NULL;
  osbl_auth_priv.last_cert_chain_ptr = NULL;
  memset( &osbl_auth_priv.debug_info, 0, sizeof(osbl_auth_priv.debug_info) );

  /* Register the singleton */
  boot_auth_if_register_singleton( &osbl_auth_priv.pub_if );

} /* osbl_auth_init_and_register() */


