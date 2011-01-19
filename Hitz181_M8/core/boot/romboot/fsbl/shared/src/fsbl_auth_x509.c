/*=============================================================================

                  F A I L  S A F E  B o o t  L o a d e r
              X . 5 0 9  A u t h e n t i c a t i o n  M o d u l e

GENERAL DESCRIPTION
  This header file contains the implementation for the FSBL X.509
  authentication module.

EXTERNALIZED FUNCTIONS
  fsbl_get_hash_data
  fsbl_auth_x509_init

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2007-2009 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/core/romboot/fsbl/shared/fsbl_auth_x509.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/03/09	tnk	   support for ULC secboot using PBL auth routines.
07/30/07   ANB     Initial version.
           
=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "fsbl_auth_x509.h"
#include "fsbl.h"
#include "boot_msm.h"
#include "fsbl_config.h"
#include "boot_hw.h"
#include "secboot_api.h"
#include <string.h>

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

typedef struct fsbl_auth_x509_type
{
  /*----------------------------------- 
   * Public module interface 
   *---------------------------------*/
  boot_auth_if_type      pub_if;

  /* Authentication module data */
  secboot_dbg_info_type  debug_info;
  pbl_secx509_cert_type  *attest_crt_ptr;
  void                   *last_cert_chain_ptr;
} fsbl_auth_x509_type;


/* FSBL authentiaction module private data structure */
static fsbl_auth_x509_type fsbl_auth_priv;

/*=============================================================================
   
                              FUNCTION DEFINITIONS

=============================================================================*/


/*=========================================================================

FUNCTION  FSBL_AUTH_X509_VERIFY_CERTIFICATE_CHAIN

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
static boolean fsbl_auth_x509_verify_certificate_chain
(
  void *cert_chain_ptr,        /* Pointer to the location of the
                                  certificate chain */
  uint32 cert_chain_size       /* Size of the certificate chain */
)
{
  if ( ! sbl_hw_is_auth_enabled() )
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
  else if ( fsbl_auth_priv.attest_crt_ptr != NULL &&
            fsbl_auth_priv.last_cert_chain_ptr == cert_chain_ptr )
  {
    /* If attest_crt_ptr is not NULL then we have already verified the certs */
    return TRUE;
  }
  
  /* Kick the dog before we proceed. */
  FSBL_KICK_WATCHDOG(); 

  /* Cache the last cert chain verified, used to test if we have already
     verified this cert chain */
  fsbl_auth_priv.last_cert_chain_ptr = cert_chain_ptr;


  /* Call authentication interface defined in secboot using information
     contained within image header.  */
  fsbl_auth_priv.attest_crt_ptr =
    secboot_verify_x509_chain_api( cert_chain_ptr, cert_chain_size,
                                   (uint8*) fsbl_config_get_root_cert(),
                                   NULL, NULL, &fsbl_auth_priv.debug_info );

  return ( fsbl_auth_priv.attest_crt_ptr != NULL );
} /* fsbl_auth_x509_verify_certificate_chain() */

/*=========================================================================

FUNCTION  FSBL_AUTH_X509_AUTHENTICATE

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
static boolean fsbl_auth_x509_authenticate
(
  mi_boot_image_header_type *header_ptr  /* Pointer to the image header  */
)
{
  /* Cast the public interface pointer to our private type */
  boolean status;

  /* Check input parameters. */
  if ( header_ptr == NULL )
  {
    return FALSE;
  }
  else if ( ! sbl_hw_is_auth_enabled() )
  {
    /* Return TRUE if authentication is disabled */
    return TRUE;
  }
  /* Verify code size is valid. */  
  if ( header_ptr->code_size == 0 )
  {
    return FALSE;
  }

  /* Kick the dog before we proceed. */
  FSBL_KICK_WATCHDOG(); 

  /* First verify the certificate chain */
  status = fsbl_auth_x509_verify_certificate_chain( header_ptr->cert_chain_ptr,
                                                    header_ptr->cert_chain_size );
  if ( !status )
  {
    return FALSE;
  }

  /* Kick the dog before we proceed. */
  FSBL_KICK_WATCHDOG(); 
  /* Image ID id 0x7 for NOR_PRG and sw_id is 0x03*/
  if((header_ptr->image_id)==0x7) fsbl_auth_priv.debug_info.sw_id=0x03;
  /* Image ID id 0x4 for AMSS_Hash and sw_id is 0x3*/
  if((header_ptr->image_id)==0x4) fsbl_auth_priv.debug_info.sw_id=0x02;

/* Verify image code signature and return the results. Use the
   global MSM hardware ID variable to do this for now. */
status = secboot_verify_code_signature_api( header_ptr->image_dest_ptr,
                                            header_ptr->code_size,
                                            sbl_hw_get_msm_hw_id(),
                                            fsbl_auth_priv.debug_info.sw_id,
                                            header_ptr->signature_ptr,
                                            header_ptr->signature_size,
                                            fsbl_auth_priv.attest_crt_ptr 
                                          );
  
  return status;
} /* fsbl_auth_x509_authenticate() */

/*=========================================================================

FUNCTION  FSBL_AUTH_x509_GET_VALUE

DESCRIPTION
  Function to get an implementation-specific value corresponding to
  the given implementation-specific value of param.  FSBL module currently
  supports the JTAG debug flag and secure boot mode.

DEPENDENCIES
  None
  
RETURN VALUE
  The implementation-specific value corresponding to param.

SIDE EFFECTS
  None

===========================================================================*/
static uint32 fsbl_auth_x509_get_value
(
  boot_auth_param_enum_type param     /* Identifier specifying what data
                                         to return                       */
)
{
  uint32 value;
  
  switch (param)
  {
  case BOOT_AUTH_PARAM_GET_ENABLE_JTAG:
    /* Just return the JTAG debug flag */
    value = fsbl_auth_priv.debug_info.enable_jtag;
    break;

  case BOOT_AUTH_PARAM_GET_SECURE_MODE:
    /* Return status of whether authentication is enabled */
    value = sbl_hw_is_auth_enabled();
    break;

  case BOOT_AUTH_PARAM_GET_OEM_ID:
    /* Just return the OEM ID */
    value = fsbl_auth_priv.debug_info.oem_id;
    break;  

  default:
    value = 0;
    break;
  }
  
  return value;
} /* fsbl_auth_x509_get_value() */

/*=========================================================================

FUNCTION  FSBL_AUTH_x509_INIT

DESCRIPTION
  Function to initialize an instance of the FSBL authentication module.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to the public auth module interface.

SIDE EFFECTS
  None

===========================================================================*/
boot_auth_if_type * fsbl_auth_x509_init( void )
{
  static const boot_module_version_type vsn =
  {
    FSBL_AUTH_X509_NAME,
    BOOT_MODULE_VERSION( BOOT_AUTH_IF_VERSION, FSBL_AUTH_X509_VERSION )
  };

  static boot_auth_vtbl_type vtbl =
  {
    fsbl_auth_x509_verify_certificate_chain,
    fsbl_auth_x509_authenticate,
    fsbl_auth_x509_get_value
  };

  /* Set the module version info and virtual table */
  fsbl_auth_priv.pub_if.vsn = &vsn;
  fsbl_auth_priv.pub_if.vtbl = &vtbl;

  /* Reset the module status variables */
  memset( &fsbl_auth_priv.debug_info, 0, sizeof(fsbl_auth_priv.debug_info) );
  fsbl_auth_priv.attest_crt_ptr         = NULL;
  fsbl_auth_priv.last_cert_chain_ptr    = NULL;

  /* Return a pointer to the public interface */
  return &fsbl_auth_priv.pub_if;
} /* fsbl_auth_x509_init() */

