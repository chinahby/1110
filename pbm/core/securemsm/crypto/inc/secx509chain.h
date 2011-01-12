#ifndef SECX509CHAIN_H
#define SECX509CHAIN_H



/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

       X . 5 0 9   C E R T I F I C A T E   C H A I N   P A R S E R

GENERAL DESCRIPTION
  The X.509 certificate chain handler allow verification of certificates
  against a given root.

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secx509chain.h#2 $ 
  $DateTime: 2008/03/28 16:44:51 $ $Author: jayanthm $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/06   lx      Add current_time as an argument for 
                   secx509chain_validate_cert_list
04/12/06   lx      Added secx509chain_init()
10/23/00   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "secerrno.h"

#ifdef FEATURE_SEC_X509
  #include "secx509.h"

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FROM ASN.1

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
#define SECX509CHAIN_MAX_CHAIN_LEN    10

typedef struct
{
  struct
  {
    secx509_cert_type    cert;
    uint8*               cert_ptr;
    uint32               cert_len;
  } element[SECX509CHAIN_MAX_CHAIN_LEN];

  secx509_cert_list_type root_cert_list;
  secx509_cert_ctx_type  cert_ctx;

  /* Store the pointers to root certs */
  uint8*        root_cert_ptr[ SECX509_NUM_CERTS ];

} secx509chain_ctx_type;

/* <EJECT> */ 
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SECX509CHAIN_INIT

DESCRIPTION
  This function initializes the chain context.

DEPENDENCIES
  None

PARAMETERS
  context_ptr - pointer to the secx509 chain context that will be initialized
  uint16      - purpose of the certificate usage, now only support SSL Server
                authentication and OMA DRM Rights Issuer authentication

RETURN VALUE
  E_SUCCESS if the chain context is initialized properly
  E_NOT_SUPPORTED if the purpose is not supported
  E_INVALID_ARG if a pointer argument is NULL

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secx509chain_init
(
  secx509chain_ctx_type* context_ptr,
  uint16                 purpose
);

/*===========================================================================

FUNCTION SECX509CHAIN_ADD_ROOT_CERT

DESCRIPTION
  This function adds a root certificate to the chain context.
  Note: This list of root certs is used for all verifications
        made with this chain context.
        This function does _not_ copy the certificate data itself. The
        memory pointed to must stay valid for the lifetime of the 
        context.

DEPENDENCIES
  This function uses the secasn and ossl_asn functionality.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secx509chain_add_root_cert
(
  secx509chain_ctx_type* context_ptr,
  uint8*                 cert_ptr,
  uint32                 cert_len
);

/*===========================================================================

FUNCTION SECX509CHAIN_ADD_CERT

DESCRIPTION
  This function adds a certificate to the list to be verified.
  Note: This function does _not_ copy the certificate data itself. The
        memory pointed to must stay valid after the call and is free'd
        by secx509chain_clear_cert_list().

DEPENDENCIES
  This function uses the secasn and ossl_asn functionality.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secx509chain_add_cert
(
  secx509chain_ctx_type* context_ptr,
  uint8*                 cert_ptr,
  uint32                 cert_len
);

/*===========================================================================

FUNCTION SECX509CHAIN_CLEAR_CERT_LIST

DESCRIPTION
  This function frees all certs listed in the current context except
  the root cerificates.

DEPENDENCIES
  This function uses the secasn and ossl_asn functionality.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function frees the memory pointed to by the cert list elements.
===========================================================================*/
secerrno_enum_type secx509chain_clear_cert_list
(
  secx509chain_ctx_type* context_ptr
);

/*===========================================================================

FUNCTION SECX509CHAIN_VALIDATE_CERT_LIST

DESCRIPTION
  This function parses a list of certificates.

DEPENDENCIES
  This function uses the secasn and ossl_asn functionality.

PARAMETERS
  context_ptr    - pointer to secx509chain_ctx_type
  current_time   - current time in seconds since CDMA epoch time of 
                   Jan 6th, 1980
  alert_mask_ptr - alert mask for verification errors from 
                   secx509_verify_cert_list(.)

RETURN VALUE
  E_SUCCESS, if verification is successful.
  E_DATA_INVALID, certificate purpose checking failed.
  E_DATA_INVALID, certificate path length checking failed.
  E_DATA_EXPIRED, certificate is not valid yet.
  E_DATA_EXPIRED, certificate is expired.
  E_DATA_TOO_LARGE, if verification failed for too many certs in chain.
  E_INVALID_ARG, if a pointer argument is set to NULL

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type secx509chain_validate_cert_list
(
  secx509chain_ctx_type*  context_ptr,
  uint32                  current_time,         /* current time in seconds */
  uint32*                 alert_mask_ptr
);

/*===========================================================================

FUNCTION SECX509CHAIN_CLEAR_CONTEXT

DESCRIPTION
  This function frees all memory associated with the context.

DEPENDENCIES
  This function uses the secasn and ossl_asn functionality.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void secx509chain_clear_context
(
  secx509chain_ctx_type*  context_ptr
);


#endif /* FEATURE_SEC_X509 */

#endif /* SECX509CHAIN_H */
