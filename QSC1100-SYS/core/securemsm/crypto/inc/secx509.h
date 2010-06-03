#ifndef SECX509_H
#define SECX509_H

#ifdef FEATURE_SEC_X509
/*==========================================================================

                A S W   S E C U R I T Y   S E R V I C E S
           
         S S L   X 5 0 9 C E R T   I N T E R F A C E   M O D U L E  

GENERAL DESCRIPTION
  This software module contains the source code for the certificate parsing
  and verification module
  
  Below is an example of how to parse a certificate, display and get the name
  of the certificate
  
  uint8 *cert_data                            // Unparsed certificate data
  uint16 cert_len                             // Length of the certificate
  secx509_cert_type *cert = NULL;             // Certificate pointer
  secerrno_enum_type err;
  uint8 name[NAME_SIZE];
  
  // Initialize the secx509 module
  err = secx509_init();
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Parse the certificate
  err = secx509_parse_certificate( cert_data, cert_len, &cert );
  
  if ( err == E_DATA_INVALID ) 
  {
    // Certificate has invalid data, handle error
  }
  else if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Verify the certificate against its own public key
  err = secx509_verify_cert( cert, &cert->pkey );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
    
  // Log the certificate to the F3 messages
  err = secx509_log_certificate( cert );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Get the name of the certificate
  err = secx509_name_certificate( cert, &name, NAME_SIZE );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }
  
  // Use the name of the certificate
  
  // Free the memory used by the certificate
  err = secx509_free_certificate( cert );
  
  if ( err != E_SUCCESS ) 
  {
    // Error has occurred
  }  
    
EXTERNALIZED FUNCTIONS

  secx509_init()                 Initializes certificate processing

  secx509_parse_certificate()    Parses an X509 certificate

  secx509_log_certificate()      Displays a parsed certificate through 
                                 F3 Messages

  secx509_name_certificate()     Finds the name of a parsed certificate

  secx509_free_certificate()     Release memory occupied by the parsed
                                 certificate

  secx509_verify_certificate()   Verifies that a parsed certificate is
                                 valid by checking the content of fields

  secx509_verify_cert_list()     Verifies that a list of parsed certificates
                                 is valid

  secx509_process_cert_list()    Processes a lists of certificates, by
                                 parsing and verifying them
                                 
  secx509_process_certificate()  Process a single certificate, by parsing 
                                 and verifying it

INITIALIZATION AND SEQUENCING REQUIREMENTS

  secx509_init() must be called before using the other functions.

Copyright (c) 2002-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
==========================================================================*/
/*==========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secx509.h#2 $
  $DateTime: 2007/10/04 09:03:46 $ $Author: jmiera $


when       who     what, where, why
--------   ---     ---------------------------------------------------------
01/25/07   df      fix for SR890833 Auth Key Id parsing bug
                   fix for verify cert chain bug
11/01/06   avm     Renamed secssl_ses_certificate_pdata_type;
                   to secx509_ses_certificate_pdata_type.
                   Included the ASN.1 definition structs 
                   from: secx509asntypes.h.
                   Moved secx509_ses_certificate_pdata_type from secssl.h 
10/30/06   rv      removed SEC_L4_SINGLE_PD
08/10/06   lx      Add current_time as an argument for time validity check
07/10/06   lx      Add SECX509_PURPOSE_OCSP_RSP and extension field 
                   ocsp_nocheck_set
03/15/06   lx      Changes to support DRM certificate chain verification
06/11/05   sv/ssm  SSL changes to support EAP-TLS
11/12/04   rv      changes to correct LINT errors
10/21/03   jay     Cleaned up the file for better readibility
08/07/02   djb     initial creation 

==========================================================================*/

/*==========================================================================

             Include Files for Module 

==========================================================================*/
#include "comdef.h"
#include "secasn1.h"
#include "seccrypt.h"
#include "secx509asntypes.h"

/*==========================================================================

             Local Constant Declaration

==========================================================================*/
/* Alert mask definitions */
#define SECX509_ALERT_BAD_CERTIFICATE            0x00000080
#define SECX509_ALERT_BAD_CERTIFICATE_SIGN       0x00000100 
#define SECX509_ALERT_BAD_CERTIFICATE_ISSUER     0x00000200 
#define SECX509_ALERT_UNSUPPORTED_CERTIFICATE    0x00000400
#define SECX509_ALERT_CERTIFICATE_REVOKED        0x00000800   
#define SECX509_ALERT_CERTIFICATE_EXPIRED        0x00001000   
#define SECX509_ALERT_CERTIFICATE_UNKNOWN        0x00002000   
#define SECX509_ALERT_ILLEGAL_PARAMETER          0x00004000
#define SECX509_ALERT_DECODE_ERROR               0x00040000
#define SECX509_ALERT_BAD_CERTIFICATE_NAME       0x00080000 
#define SECX509_ALERT_BAD_CERTIFICATE_SERIALNUM  0x00100000 

#define SECX509_MAX_DIGEST_SIZE   MAX( SECAPI_HSH_MD5_DIGEST_SIZE,  \
                                        SECAPI_HSH_SHA_DIGEST_SIZE )
#define SECX509_CERT_CHAIN_LEN_SIZ          3
#define SECX509_CERT_LEN_SIZ                3
#define SECX509_MAX_CERT_LEN                4000
#define SECX509_ASN1_MAX_HASH_OBJ_LEN       19
#define SECX509_MAX_VRFY_SIG_LEN      \
        ( SECX509_ASN1_MAX_HASH_OBJ_LEN + SECX509_MAX_DIGEST_SIZE )

#define SECX509_MAX_DER_ALGORITHM_ID_LEN    64
#define SECX509_MAX_DER_PUBKEY_INFO_LEN     \
	    ( SECAPI_PKX_MAX_KEY_SIZE * 2 + SECX509_MAX_DER_ALGORITHM_ID_LEN )
#define SECX509_MAX_STRING_LEN              256

#define SECX509_NUM_CERTS 10

#define SECX509_MD2_BLOCK           16
#define SECX509_MD2_DIGEST_LENGTH   16

//#define SECX509_CERT_LIST_MAX_DEPTH     9
#define SECX509_CERT_LIST_MAX_DEPTH     ( SECX509_NUM_CERTS - 1 )

#define SECX509_TRUST_SSL_SERVER        3

/* SSL Purpose Values */
#define SECX509_PURPOSE_SSL_CLIENT      1
#define SECX509_PURPOSE_SSL_SERVER      2
#define SECX509_PURPOSE_NS_SSL_SERVER   3
#define SECX509_PURPOSE_SMIME_SIGN      4
#define SECX509_PURPOSE_SMIME_ENCRYPT   5
#define SECX509_PURPOSE_CRL_SIGN        6
#define SECX509_PURPOSE_ANY             7
#define SECX509_PURPOSE_DRM_RI          8
#define SECX509_PURPOSE_OCSP_RSP        9

/*==========================================================================

             Structure Definition

==========================================================================*/
/* Public Key algorithms in the certificate */
typedef enum
{
  SECX509_PUBKEY_RSA = 0,
  SECX509_PUBKEY_DSA = 1,
  SECX509_PUBKEY_DH  = 2,
  SECX509_PUBKEY_MAX               /* Last one, for error checking */

} secx509_pubkey_algo_type;

/* Certificate signature algorithm type */
typedef enum
{
  SECX509_md5WithRSAEncryption  = 0,
  SECX509_md2WithRSAEncryption  = 1,
  SECX509_sha1WithRSAEncryption = 2,
  SECX509_SIG_ALGO_MAX                  /* Last one, for error checking */

} secx509_sig_algo_type;

/* RSA public key parameters */
typedef struct 
{
  uint16  mod_len;
  uint8*  mod_data;
  uint16  exp_e_len;
  uint8*  exp_e_data;

} secx509_rsa_pubkey_type;

/* DSA public key parameters */
typedef struct
{
  uint16  dummy_len;
  uint8*  dummy;

} secx509_dsa_pubkey_type;

/* DH public key parameters */
typedef struct
{
  uint16  mod_len;
  uint8*  mod_data;
  uint16  exp_len;
  uint8*  exp_data;

} secx509_dh_pubkey_type;


/* Union of all the public key types */
typedef struct
{
  secx509_pubkey_algo_type  algo;
  union
  {
    secx509_rsa_pubkey_type  rsa;
    secx509_dsa_pubkey_type  dsa;
    secx509_dh_pubkey_type   dh;
  
  }key;

} secx509_pubkey_type;

/* Signature Structure */
typedef struct
{
  secx509_sig_algo_type  algo_id;
  secasn1_data_type      val;

} secx509_signature_type;

/* Distinguished name structure */
typedef struct
{
  uint8              num_attrib;
  secasn1_data_type  data;

} secx509_dn_type;

/* Version structure */
typedef struct
{
  uint8              ver;
  secasn1_data_type  val;

} secx509_version_type;

/* Time structure */
typedef struct
{
  uint32             time;
  secasn1_data_type  data;

} secx509_time_type;

/* Authority Key Identifier structure */
typedef struct
{
  boolean            set;
  secasn1_data_type  key_id;        
  secx509_dn_type    name;
  secasn1_data_type  serial_number;

} secx509_auth_key_id_type;

/* Subject Key Identifier structure */
typedef struct
{
  boolean            set;
  secasn1_data_type  key_id;

} secx509_subject_key_id_type;

/* Key Usage structure */
typedef struct
{
  uint16   val;
  boolean  set;

} secx509_key_usage_type;

/* CA structure */
typedef struct
{
  boolean  set;
  boolean  val;

} secx509_ca_type;

/* Extension structure type */
typedef struct
{
  boolean                      set;
  secx509_auth_key_id_type     auth_key_id;
  secx509_subject_key_id_type  subject_key_id;
  secx509_key_usage_type       key_usage;
  secx509_key_usage_type       ex_key_usage;
  int                          path_len;
  secx509_ca_type              ca;
  boolean                      ocsp_nocheck_set;

} secx509_ext_type;

/* DER-encoded SubjectPublicKeyInfo: used for OMA-DRM */
typedef struct
{
  uint8   data[SECX509_MAX_DER_PUBKEY_INFO_LEN];
  uint16  len;
} secx509_der_pubkey_info_type;


/* Certificate information structure */
typedef struct
{
  secx509_version_type     version;
  secasn1_data_type        serial_number;
  secx509_signature_type   algorithm;
  secx509_dn_type          issuer;
  secx509_time_type        not_before;
  secx509_time_type        not_after;
  secx509_dn_type          subject;
  secx509_pubkey_type      pub_key;
  secasn1_bit_string_type  issuer_unique_id;
  secasn1_bit_string_type  subject_unique_id;
  secx509_ext_type         extension;

} secx509_cert_info_type;

/* Certificate structure */
typedef struct
{
  /* The cert_info needs to be the first member */
  secx509_cert_info_type  cert_info;

  /* Signature info on the cert */
  secx509_pubkey_type     pkey;
  secx509_sig_algo_type   sig_algo;
  uint32                  cinf_offset;
  uint16                  cinf_byte_len;
  uint8                   sig[SECX509_MAX_VRFY_SIG_LEN];
  uint16                  sig_len;

  /* for DRM to calculate Key Identifier */
  secx509_der_pubkey_info_type pubkey_info;

  /* For bookkeeping */
  boolean                 __used;

} secx509_cert_type;

/* Certificate list struct */
typedef struct secx509_cert_list_struct
{
  secx509_cert_type*  cert[SECX509_NUM_CERTS];
  uint8               size;

} secx509_cert_list_type;

/* certificate list context type */
typedef struct
{
  boolean                  ignore_expiration;
  int                      purpose;
  int                      trust;
  uint16                   depth;
  secx509_cert_list_type*  ca_list;

} secx509_cert_ctx_type;

/* MD2 context type structure */
typedef struct
{
  uint16  num;
  uint8   data[SECX509_MD2_BLOCK];
  int     cksm[SECX509_MD2_BLOCK];
  int     state[SECX509_MD2_BLOCK];    

} secx509_md2_ctx_type;

/* Parameter - Certificate Information */
typedef struct
{
  SECASN1_INTEGER  version;          /* Version number (ASN.1)      */
  SECASN1_INTEGER  serial_no;        /* Serial number  (ASN.1)      */
  SECASN1_STRING   issuer;           /* Issuer name (ASN.1)         */
  SECASN1_UTCTIME  not_before;       /* Validity start date (ASN.1) */
  SECASN1_UTCTIME  not_after;        /* Validity end date (ASN.1)   */
  SECASN1_STRING   object_organization;
  SECASN1_STRING   object_common_name;
} secx509_ses_certificate_pdata_type;

/*==========================================================================

                      PUBLIC FUNCTION DECLARATIONS

==========================================================================*/
/*===========================================================================

FUNCTION SECX509_INIT

DESCRIPTION
  This function initializes the global data structures for use, it
  initializes the certificate buffer and the cert list along with the
  ctx structure

DEPENDENCIES
  Must be called before any functions.
  
PARAMETERS
  None

RETURN VALUE
  E_SUCCESS - when processing is successful
  
SIDE EFFECTS
  If the function is called for a second time any other applications holding
  certificate using the certificate buffer will be wiped out.
===========================================================================*/
extern secerrno_enum_type secx509_init( void );

/*===========================================================================

FUNCTION SECX509_PARSE_CERTIFICATE

DESCRIPTION
  Parses an X509 certificate from the data_ptr, if *cert == NULL then the
  certificate is allocated from the memory pool, if the certificate is
  already allocated then the function just uses the pre-allocated memory

DEPENDENCIES
  None
  
PARAMETERS
  data_ptr - pointer to the raw certificate data
  cert_len - length of the certificate data
  cert - pointer to the certificate

RETURN VALUE
  E_SUCCESS if the certificate is parsed properly
  E_DATA_INVALID if the certificate cannot be parsed properly
  E_NO_MEMORY if no more memory slots are available for the certs
  E_NOT_SUPPORTED if an algorithm found in the cert is not supported
  E_INVALID_ARG if a pointer argument is NULL
  E_FAILURE if the *cert is pre-allocated but not *cert->cert or if the
            certificate data length does not match cert_len
  
SIDE EFFECTS
  None
===========================================================================*/
extern secerrno_enum_type secx509_parse_certificate
(
  uint8                         *cert_data,/* cert data in the DER format */
  uint16                        cert_len,  /* cert length of the raw data */
  secx509_cert_type             **cert     /* pointer to the parsed struct*/
);

/*==========================================================================

FUNCTION SECX509_NAME_CERTIFICATE

DESCRIPTION
  This function gets a names for the certificate. A name is returned into
  the buf, whose max size is len. The certificate name is decided as 
  follows:

  1. If the subject's organization name is valid, the name will be org
     name.
  2. Otherwise, if it's common name is valid, the name will be the common
     name.
  3. Otherwise, return the subject name in one-line.

DEPENDENCIES
  None.

PARAMETERS
  cert_ptr - pointer to the certificate to be named
  buf - location to place the certificate name
  len - the maximum length of the buf for the certificate name

RETURN VALUE
  E_SUCCESS, with a valid name.
  E_FAILURE, otherwise.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secx509_name_certificate
(
  secx509_cert_type     *cert_ptr,
  char                  *buf,
  uint16                len
);

/*==========================================================================

FUNCTION SECX509_LOG_CERTIFICATE

DESCRIPTION
  This function logs a certificate information.

DEPENDENCIES
  This function uses the MSG_STR_LOW to print out the string information.
  This is very CPU intensive, and should be only used when absolutely
  necessary. 
  
PARAMETERS
  cert_ptr - pointer to the certificate to be printed

RETURN VALUE
  E_SUCCESS,    if successful.
  E_FAILURE,    if failed.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secx509_log_certificate
(
  secx509_cert_type     *cert_ptr
);

/*=============================================================================

FUNCTION SECX509_VERIFY_CERTIFICATE

DESCRIPTION
 This function verifies a certificate by decrypting the digital signature 
 and comparing it with the calcuated hashing over the signed certificate 
 information.

DEPENDENCIES
  None.

PARAMETERS
  cert_ptr - pointer to the certificate to be verified
  pkey_ptr - public key of the server

RETURN VALUE
 E_SUCCESS, if verify_certificate is successful.
 E_DATA_INVALID, if verification failed.
 E_NOT_SUPPORTED,      if algorithm is not supported.
 E_INVALID_ARG, if either of the argument ptr are NULL
 E_FAILURE, otherwise.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secx509_verify_certificate
(
  secx509_cert_type             *cert_ptr,
  secx509_pubkey_type           *pkey_ptr
);

/*==========================================================================

FUNCTION SECX509_PROCESS_CERTIFICATE

DESCRIPTION
  This function processes a single certificate. It parses the certificate,
  checks on its validity time, and if it is a self-signed certificate, 
  verifies it with its own key. Upon return, if successful, a valid parsed 
  certificate is returned.

DEPENDENCIES
  None.

PARAMETERS
  cert_data - data_ptr to the raw certificate data
  cert_len  - the length of the certificate data
  cert      - the certificate ptr
  current_time - current time in seconds since CDMA epoch time of 
                 Jan 6th, 1980
  
RETURN VALUE
  E_SUCCESS, if verify_certificate is successful.
  E_INVALID_ARG, if a ptr argument is NULL
  E_NO_DATA, if the cert cannot be verified because phone has no sys time
  E_FAILURE, otherwise.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secx509_process_certificate
(
  uint8                 *cert_data,
  uint16                cert_len,
  secx509_cert_type     **cert,
  uint32                current_time
);

/*==========================================================================

FUNCTION SECX509_VERIFY_CERT_LIST

DESCRIPTION
  This function verifies the certificate list. It starts from the server's
  certificate and verifies each certificate until it reaches the root 
  certificate or the certificate from the CA.

  The verification involves checking of the depth of the certificate, the
  purpose, the trust, the validity, and finally verify the signature of
  the certificate. 

DEPENDENCIES
  None

PARAMETERS
  cert_list - list of parsed certificates to be verified
  ctx       - certificate list context for the cert list
  current_time - current time in seconds since CDMA epoch time of 
                 Jan 6th, 1980
  security_alert_mask_ptr - alert mask for verification errors

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
==========================================================================*/
extern secerrno_enum_type secx509_verify_cert_list
(
  secx509_cert_list_type        *cert_list,
  secx509_cert_ctx_type         *ctx,
  uint32                        current_time,
  uint32                        *security_alert_mask_ptr
);

/*==========================================================================

FUNCTION SECX509_PROCESS_CERT_LIST

DESCRIPTION
  This function processes a certificate list, by parsing the certificate
  from the server upto the root, and verifying that each certificate 
  is signed properly by either the CA, or the issuing certificate. 

DEPENDENCIES
  This function will free the certificate data memory from the DSM item
  and place it in the secx509_cert_chain_buf, but when this memory also
  get erased when secx509_cert_list_init() gets called at cleanup.

PARAMATERS
  msg_ptr  - pointer to the message containing the certificate list
  msg_len  - the length of the certificate list message
  pubkey   - the servers public key
  srv_cert - the server certificate
  security_alert_mask_ptr - alert mask for processing errors
  
RETURN VALUE
  The following return code will be returned:-
    E_SUCCESS, if processing is successful.
    E_DATA_INVALID, if certificate is not valid.
    E_DATA_TOO_LARGE, if the certificate is too large for the buffer.
    E_FAILURE, failed to process the cert list due to other reasons.
    E_INVALID_ARG, if an invalid argument is passed in.
  If the return code is E_SUCCESS, this function also returns:
    public key from the server certificate,
    the simple information from the server certificate.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secx509_process_cert_list
(
  dsm_item_type                      **msg_ptr,
  uint16                             msg_len,
  secx509_cert_ctx_type              *custom_ctx_ptr,
  secx509_pubkey_type                *pubkey,
  secx509_ses_certificate_pdata_type  *srv_cert,
  uint32                             *security_alert_mask_ptr,
  uint32                             current_time
);

/*===========================================================================

FUNCTION SECX509_FREE_CERTIFICATE

DESCRIPTION
  This function returns a certificate to the certificate buffer.  The 
  certificate memory is returned to the buffer by adding it to the end of
  the secx509_free_cert_list list and the end marker of the list, 
  secx509_free_cert_end is incremented.  If successful the number of 
  certs allocated, secx509_num_cert_alloc is decremented.

DEPENDENCIES
  None
  
PARAMETERS
  cert_ptr - pointer to a certificate being freed 

RETURN VALUE
  E_SUCCESS - if the certificate was successfully freed
  E_FAILURE - if there was an error freeing the certificate, such as
              the cert_ptr is pointing to a cert that is not allocated or
              the cert_ptr points to memory not in the certificate buffer
  
SIDE EFFECTS
  None  
===========================================================================*/
extern secerrno_enum_type secx509_free_certificate
(
  secx509_cert_type             *cert_ptr
);

#endif /* #ifdef FEATURE_SEC_X509 */

#endif /* #ifdef SECX509_H */
