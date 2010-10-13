#ifndef SECAPI_H
#define SECAPI_H
/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y

         E X T E R N A L   A P P L I C A T I O N   I N T E R F A C E

DESCRIPTION
  This file defines the external API for applications using
  cryptographic functionality from Security Services.
  The cryptographic API (CAPI) comprises

  + Management of Cryptographic Instances (Handles):
      - Allocation and deallocation of handles

  + Key Exchange Functions (secapi_pkx):
      - Diffie-Hellman
      - RSA (up to 1024 bit key length)

  + En- / decryption Functions (secapi_enc):
      - 3DES and DES
      - ARC4 (public domain version of RC4)

  + Hashing Functions (secapi_hsh):
      - MD2
      - MD5
      - SHA-1
      - SHA-256
      
  + Random Number Generator
      - Service for pseudo random data
      - Service for random data (based on external events)
      - Service for secure random data (suited for use in 
        cryptographic applications)

EXTERNALIZED FUNCTIONS

    secapi_new()
    secapi_delete()

    secapi_pkx_dh_set_param()
    secapi_pkx_dh_create_key()
    secapi_pkx_rsa_do_exponentiation()
    secapi_pkx_rsa_set_param()
    secapi_pkx_rsa_verify_signature()
    secapi_pkx_rsa_public_encrypt()

    secapi_enc_set_param()
    secapi_enc_encrypt()
    secapi_enc_decrypt()

    secapi_hsh_set_param()
    secapi_hsh_create_digest()

    secapi_get_random()
    secapi_get_random_set_cb()

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by Security Services
  internally and no application specific intialization is required.

 Copyright (c) 2002-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secapi.h#8 $ 
  $DateTime: 2009/05/06 08:15:36 $ 
  $Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/09   ejt     Added secapi_hsh_create_hmac_iov() & secapi_hsh_create_digest_iov()
                   secapi_enc_encrypt_iov() & secapi_enc_decrypt_iov() routines to secapi.
                   These routines take msg in/out based on a IOVEC_T pointer.
11/25/08    rv     Added SECAPI_GET_RANDOM_SET_CB
09/24/08   EJT     Added SECAPI_ENC_DEVKEY_SFS & SECAPI_HSH_DEVKEY_SFS defines  
02/19/08   pc      Added ONCRPC meta comments.
04/10/07   rv      added support for SHA256 on dsp
11/22/06   df      Add support for enc input mode
08/02/06   rv      support CTR mode encryption
08/31/05   df      changes to support IOVEC
03/21/05   sb      Added support for Key Store API, Change to pkx_authenticate API
02/25/05   sb      Added support for Rsa signing, MSM device key based DSP commands
06/08/04   jay     Added RFC2630_PADDING
04/22/04   rwh     Minor formatting change.
03/08/04   rwh     Added RFC-2104-style HMAC.
03/02/04   rwh     Removed endianess params in encrypt(), decrypt(), digest().
                   Added return status in get_padded_length().
02/19/04   rwh     Increased modmath param size to 256 bytes.
09/25/03   yhw     Added SECAPI_AES128.
10/30/03   rwh     Added secapi_pkx_dh_private_key_len_type
09/04/03   om      Added SECPKX_NO_PADDING enum value.
07/22/03   rwh     Added some authentication #define's.
05/28/03   jm      Cleanup for new seccrypt interface.
12/12/02   rwh     Corrected comment.
12/11/02   rwh     Removed DH and RSA callback param types; instead use 
                   general event handler callback.
11/18/02   rwh     Corrected DH parameter name.
10/21/02   om      Updates for new Brew requirements.
08/01/02   om      Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "rex.h"

#ifdef FEATURE_SEC

#include "secerrno.h"
#include "seciovec.h"


/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
  General CAPI types and definitions
===========================================================================*/

/*---------------------------------------------------------------------------
  Crypto session external Handle definition.
---------------------------------------------------------------------------*/
typedef void * secapi_handle_type;

/* Asynchronous execution callback registration */
typedef struct
{
  secerrno_cb_fptr_type  cb_fptr;
  void*                  usr_ptr;
} secapi_async_exec_cb_param_data_type;


/* Data Get function ptr */
typedef secerrno_enum_type (*secapi_data_get_fptr_type)(void*, void *, int *);
typedef struct
{
  secapi_data_get_fptr_type  fptr;
  void*                  usr_ptr;
} secapi_data_get_func_param_data_type;


/*---------------------------------------------------------------------------
  ENUM type for the possible Crypographic algorithms: Key exchanges, Hash
  functions, and encrypt-decrypt functions
---------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_NULL_KEY_X      = 0,      /* No key exchange protocol used        */
  SECAPI_MODEXP          = 1,      /* Generic modulo exponentiation        */
  SECAPI_DIFFIE_HELLMAN  = 2,      /* Diffie-Helman Key exchange protocol  */
  SECAPI_RSA             = 3,      /* RSA Encrypt / Key exchange protocol  */
  SECAPI_NULL_HASH       = 4,      /* No simultaneous hash with enc/dec    */
  SECAPI_SHA             = 5,      /* SHA-1 Hash function                    */
  SECAPI_SHA256          = 6,      /* SHA256 Hash function                 */ 
  SECAPI_MD5             = 7,      /* MD5 Hash function                    */
  SECAPI_NULL_CIPHER     = 8,      /* No encryption selected               */
  SECAPI_DES             = 9,      /* DES Encryption-Decryption function   */
  SECAPI_3DES            = 10,     /* 3DES Encryption-Decryption function  */
  SECAPI_ARC4            = 11,     /* ARC4 Encryption-Decryption function  */
  SECAPI_AES128          = 12,     /* AES cipher, 128-bit key              */
  SECAPI_C2              = 13,     /* C2 cipher */
  SECAPI_ALGO_MAX,                 /* Must be last in list, to size array  */
  SECAPI_RESERVED        = 0x7FFFFFFF
} secapi_algo_enum_type;

/*---------------------------------------------------------------------------
  parameter type for SECAPI_EXEC_MODE commands
---------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_ASYNC_EXEC  = 0,     /* Asynchronous Execution (ie. non-blocking) */
  SECAPI_SYNC_EXEC   = 1      /* Synchronous Execution (ie.blocking)       */
} secapi_exec_mode_enum_type;

/*---------------------------------------------------------------------------
  parameter type for the endianess of the data format
---------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_NETWORK_ORDER    = 0, /* Big endian data format                   */
  SECAPI_WORD_HOST_ORDER  = 1  /* Small endian word (16 bit) format        */
} secapi_endian_type;

/*---------------------------------------------------------------------------
  Platform for execution
---------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_EXEC_PLATFORM_ARM = 0,
  SECAPI_EXEC_PLATFORM_ACCELERATOR_PREFERRED = 4,
  SECAPI_EXEC_MAX_PLATFORM
} secapi_platform_enum_type;

typedef secapi_platform_enum_type secapi_exec_platform_param_data_type;


/*---------------------------------------------------------------------------
  parameter type for SECAPI_CALC_MAX_TIME command.
---------------------------------------------------------------------------*/
typedef uint32 secapi_exec_timeout_param_data_type;

/* Union for all common parameters */
typedef union
{
  secapi_exec_mode_enum_type            exec_mode;
  secapi_exec_timeout_param_data_type   exec_timeout;
  secapi_exec_platform_param_data_type  exec_platform;
  secapi_async_exec_cb_param_data_type  async_cb;
  secapi_data_get_func_param_data_type  data_get;
} secapi_param_type;
/*~ PARTIAL secapi_param_type */
/*~ CASE SECAPI_EXEC_MODE secapi_param_type.exec_mode */
/*~ CASE SECAPI_EXEC_TIMEOUT secapi_param_type.exec_timeout */
/*~ CASE SECAPI_EXEC_PLATFORM secapi_param_type.exec_platform */

/*---------------------------------------------------------------------------
  Parameter types for all instances 
---------------------------------------------------------------------------*/
typedef enum
{
  /* Common parameter types */
  SECAPI_EXEC_MODE                = 0,
  SECAPI_EXEC_TIMEOUT             = 1,
  SECAPI_EXEC_PLATFORM            = 2,
  SECAPI_ASYNC_CALLBACK           = 3,
  SECAPI_DATA_GET_FUNC            = 4,
  SECAPI_MAX_COMMON_PARAM         = 5,

  /* PKX-specific parameters */
  SECAPI_PKX_PARAM_START          = 0x1000,
  SECAPI_PKX_MODEXP               = SECAPI_PKX_PARAM_START,
  SECAPI_PKX_DH_GROUP             = SECAPI_PKX_PARAM_START + 1,
  SECAPI_PKX_DH_PRIVATE_KEY_LEN   = SECAPI_PKX_PARAM_START + 2,
  SECAPI_PKX_DH_PEER_PUBLIC_KEY   = SECAPI_PKX_PARAM_START + 3,
  SECAPI_PKX_RSA_MODULUS          = SECAPI_PKX_PARAM_START + 4,
  SECAPI_PKX_RSA_PRIVATE_KEY      = SECAPI_PKX_PARAM_START + 5,
  SECAPI_PKX_RSA_PUBLIC_KEY       = SECAPI_PKX_PARAM_START + 6,
  SECAPI_PKX_RSA_PRIVATE_KEY_IV   = SECAPI_PKX_PARAM_START + 7,
  SECAPI_PKX_MODEXP_PRIV_KEY_IV   = SECAPI_PKX_PARAM_START + 8,
  SECAPI_PKX_RSA_KS_PARAMS        = SECAPI_PKX_PARAM_START + 9,
  SECAPI_PKX_RSA_ISIGNAL          = SECAPI_PKX_PARAM_START + 10,
  SECAPI_PKX_MAX_PARAM            = SECAPI_PKX_PARAM_START + 11,

  /* Cipher-specific parameters */
  SECAPI_ENC_PARAM_START          = 0x2000,
  SECAPI_ENC_INIT_DATA            = SECAPI_ENC_PARAM_START,
  SECAPI_ENC_OP_MODE              = SECAPI_ENC_PARAM_START + 1,
  SECAPI_ENC_CBC_STATE            = SECAPI_ENC_PARAM_START + 2,
  SECAPI_ENC_INPUT_MODE           = SECAPI_ENC_PARAM_START + 3,
  SECAPI_ENC_PADDING_TYPE         = SECAPI_ENC_PARAM_START + 6,
  SECAPI_ENC_DEVKEY_SFS           = SECAPI_ENC_PARAM_START + 7,  
  SECAPI_ENC_MAX_PARAM            = SECAPI_ENC_PARAM_START + 8,

  /* Hash-specific parameters */
  SECAPI_HSH_PARAM_START          = 0x3000,
  SECAPI_HSH_INPUT_MODE           = SECAPI_HSH_PARAM_START,
  SECAPI_HSH_DEVKEY_SFS,                                        
  SECAPI_HSH_HIPRI,
  SECAPI_HSH_MAX_PARAM,
  SECAPI_PARAM_ENUM_RESERVED      = 0x7FFFFFFF

} secapi_param_enum_type;


/*===========================================================================
  Key exchange interface types and definitions
===========================================================================*/

/*---------------------------------------------------------------------------
  Enumeration for the key exchange command parameter types.
---------------------------------------------------------------------------*/
typedef secapi_param_enum_type secapi_pkx_param_enum_type;

/* Maximum supported operand byte size for modular math */
#define SECAPI_PKX_MAX_KEY_SIZE       256 

/*---------------------------------------------------------------------------
  Generic modulo exponentiation 
---------------------------------------------------------------------------*/
typedef struct
{
  uint8*  base_ptr;
  uint16  base_size;
  uint8*  modulus_ptr;
  uint16  modulus_size;
  uint8*  exponent_ptr;
  uint16  exponent_size;
  uint8*  priv_key_iv_ptr;
  uint8*  priv_key_devkey_ptr;
  uint16  priv_key_iv_size;
} secapi_pkx_modexp_param_type;
/*~ FIELD secapi_pkx_modexp_param_type.base_ptr 
    VARRAY LENGTH secapi_pkx_modexp_param_type.base_size */
/*~ FIELD secapi_pkx_modexp_param_type.modulus_ptr 
    VARRAY LENGTH secapi_pkx_modexp_param_type.modulus_size */
/*~ FIELD secapi_pkx_modexp_param_type.exponent_ptr 
    VARRAY LENGTH secapi_pkx_modexp_param_type.exponent_size */
/*~ FIELD secapi_pkx_modexp_param_type.priv_key_iv_ptr 
    VARRAY SECAPI_PKX_MAX_KEY_SIZE 
    LENGTH secapi_pkx_modexp_param_type.priv_key_iv_size */
/*~ FIELD secapi_pkx_modexp_param_type.priv_key_devkey_ptr 
    VARRAY SECAPI_PKX_MAX_KEY_SIZE 
    LENGTH secapi_pkx_modexp_param_type.priv_key_iv_size */

/*---------------------------------------------------------------------------
  Diffie-Helman Key generation command parameter type.
---------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_PKX_DH_PRIVATE_KEY    = 0,   /* Private key                       */
  SECAPI_PKX_DH_PUBLIC_KEY     = 1,   /* Public key                        */
  SECAPI_PKX_DH_SECRET_KEY     = 2,    /* Secret key                        */
  SECAPI_PKX_KEY_ENUM_RESERVED = 0x7FFFFFFF
} secapi_pkx_key_enum_type;

typedef struct
{
  uint8*  base_ptr;
  uint16  base_size;
  uint8*  modulus_ptr;
  uint16  modulus_size;
} secapi_pkx_dh_group_param_type;
/*~ FIELD secapi_pkx_dh_group_param_type.base_ptr 
    VARRAY LENGTH secapi_pkx_dh_group_param_type.base_size */
/*~ FIELD secapi_pkx_dh_group_param_type.modulus_ptr 
    VARRAY LENGTH secapi_pkx_dh_group_param_type.modulus_size */

typedef uint16 secapi_pkx_dh_private_key_len_type;
typedef uint8* secapi_pkx_dh_peer_public_key_type;

typedef union
{
  secapi_pkx_dh_group_param_type      group;
  secapi_pkx_dh_peer_public_key_type  peer_public_key_ptr;
  secapi_pkx_dh_private_key_len_type  private_key_len;
} secapi_pkx_dh_param_type;
/*~ PARTIAL secapi_pkx_dh_param_type */
/*~ CASE SECAPI_PKX_DH_GROUP  secapi_pkx_dh_param_type.group */
/*~ CASE SECAPI_PKX_DH_PRIVATE_KEY_LEN secapi_pkx_dh_param_type.private_key_len */


/*---------------------------------------------------------------------------
  RSA definitions, types, and parameters.
---------------------------------------------------------------------------*/

// Key store parameters
typedef struct secapi_pkx_rsa_ks_params_s
{
  void*   handle;          // Key store handle
  uint16  key_size;        // Size of the key in key store
  boolean sign;            // Will perform a sign operation
} secapi_pkx_rsa_ks_params_type;

typedef struct
{
  uint8*  modulus_ptr;
  uint16  modulus_size;
  uint8*  private_key_ptr;
  uint8*  public_key_ptr;
  uint16  key_size;
  uint8*  priv_key_iv_ptr;
  uint8*  priv_key_devkey_ptr;
  uint16  priv_key_iv_size;
} secapi_pkx_rsa_key_param_type;
/*~ FIELD secapi_pkx_rsa_key_param_type.modulus_ptr 
    VARRAY LENGTH secapi_pkx_rsa_key_param_type.modulus_size */
/*~ FIELD secapi_pkx_rsa_key_param_type.private_key_ptr 
    VARRAY LENGTH secapi_pkx_rsa_key_param_type.key_size */
/*~ FIELD secapi_pkx_rsa_key_param_type.public_key_ptr 
    VARRAY LENGTH secapi_pkx_rsa_key_param_type.key_size */
/*~ FIELD secapi_pkx_rsa_key_param_type.priv_key_iv_ptr 
    VARRAY LENGTH secapi_pkx_rsa_key_param_type.priv_key_iv_size */
/*~ FIELD secapi_pkx_rsa_key_param_type.priv_key_devkey_ptr 
    VARRAY LENGTH secapi_pkx_rsa_key_param_type.priv_key_iv_size */

typedef struct
{
    secerrno_enum_type        *_errno;
    uint8                     *sig_output;
    uint16                    *sig_output_len;
}secapi_pkx_rsa_isignal_usr_ptr;

typedef struct secapi_pkx_rsa_isignal_param_s
{
    void                              *piSignal;
    secapi_pkx_rsa_isignal_usr_ptr    *pusr_ptr;
}secapi_pkx_rsa_isignal_param_type;

/*--------------------------------------------------------------------------
  Enumeration for the RSA Encryption Encoding types. They are:
  1. Encoding method for encrypt/decrypt, using Public Key Cryptography 
     Standards 1 (PKCS1) method
  2. Encoding method for encrypt/decrypt, using Optimal Asymmetric 
     Encryption Padding (OAEP) method
  3. Encoding method for signature sign/verification, using PKCS1 method
  4. Encoding method for signature sign/verification, using Probabilistic
     Signature Scheme (PSS) method
--------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_PKX_RSA_PADDING_ENC_PKCS1_V15 = 0,
  SECAPI_PKX_RSA_PADDING_ENC_OAEP,
  SECAPI_PKX_RSA_PADDING_SIG_PKCS1_V15,
  SECAPI_PKX_RSA_PADDING_SIG_PSS,
  SECAPI_PKX_NO_PADDING,
  SECAPI_PKX_PADDING_ENUM_RESERVED
} secapi_pkx_padding_enum_type;

typedef union
{
    secapi_pkx_rsa_key_param_type             key;
    secapi_pkx_rsa_ks_params_type             ks_params;
    secapi_pkx_rsa_isignal_param_type         isignal;
} secapi_pkx_rsa_param_type;
/*~ PARTIAL secapi_pkx_rsa_param_type */
/*~ IF ( _DISC_ >= SECAPI_PKX_RSA_MODULUS && _DISC_ <=  SECAPI_PKX_RSA_PRIVATE_KEY_IV ) 
    secapi_pkx_rsa_param_type.key */


typedef union
{
  secapi_param_type             common;
  secapi_pkx_modexp_param_type  modexp;
  secapi_pkx_dh_param_type      dh;
  secapi_pkx_rsa_param_type     rsa;
} secapi_pkx_param_data_type;
/*~ IF ( _DISC_ < SECAPI_MAX_COMMON_PARAM ) secapi_pkx_param_data_type.common */
/*~ IF ( _DISC_ == SECAPI_PKX_MODEXP || _DISC_ == SECAPI_PKX_MODEXP_PRIV_KEY_IV ) 
    secapi_pkx_param_data_type.modexp */
/*~ IF ( _DISC_ >= SECAPI_PKX_DH_GROUP && _DISC_ <= SECAPI_PKX_DH_PEER_PUBLIC_KEY ) 
    secapi_pkx_param_data_type.dh */
/*~ IF ( _DISC_ >= SECAPI_PKX_RSA_MODULUS && _DISC_ <= SECAPI_PKX_RSA_PRIVATE_KEY_IV ) 
    secapi_pkx_param_data_type.rsa */
/* NOTE: HTORPC requires a discriminator for every union. The discriminator
 * of the union secapi_pkx_param_data_type is also the discriminator for the
 * union secapi_param_type and other unions contained. To achieve this we 
 * describe obj_disc as the DISC for these fields. The variable "obj_disc" is NOT a
 * HTORPC keyword, but is a the name of the discriminator variable in the 
 * generated _xdr.c code. Exert care when generating the glue code from any
 * new version of HTORPC compiler. 
 * Current HTORPC version label: MSMSHARED_HTORPC.02.00.03 
 */
/*~ FIELD secapi_pkx_param_data_type.common DISC obj_disc */
/*~ FIELD secapi_pkx_param_data_type.dh DISC obj_disc */
/*~ FIELD secapi_pkx_param_data_type.rsa DISC obj_disc */

/*----------------------------------------------------------------------------
  Cipher Key length and block length definitions.
----------------------------------------------------------------------------*/
/* ARC4 Key length */
#define SECAPI_ENC_ARC4_KEY_BYTE_LEN   16

/* DES unpacked key length. */
#define SECAPI_ENC_DES_UNPACKED_KEY_BYTE_LEN    8

/* Cipher maximum key length is 3 keys for 3DES. */
#define SECAPI_ENC_MAX_KEY_BYTE_LEN  \
          (3*SECAPI_ENC_DES_UNPACKED_KEY_BYTE_LEN)

/* DES block length */
#define SECAPI_ENC_DES_BLOCK_BYTE_LEN  8

/* AES128 block length */
#define SECAPI_ENC_AES128_BLOCK_BYTE_LEN  16

/* Cipher maximum block length */
#define SECAPI_ENC_MAX_BLOCK_BYTE_LEN  \
          (SECAPI_ENC_AES128_BLOCK_BYTE_LEN)
          
typedef enum
{
  SECAPI_ENC_CIPHER_ENCRYPT  = 0,   /* Encrypt operation, path, etc.      */
  SECAPI_ENC_CIPHER_DECRYPT  = 1    /* Decrypt operation, path, etc.      */
} secapi_enc_cipher_xform_type;

/*---------------------------------------------------------------------------
  CIPHER init vector parameters type
---------------------------------------------------------------------------*/
typedef secapi_param_enum_type secapi_enc_param_enum_type;

typedef struct
{
  struct
  {
    uint8*  key_ptr;
    uint8*  iv_ptr;
  } enc;

  struct
  {
    uint8*  key_ptr;
    uint8*  iv_ptr;
  } dec;

} secapi_enc_init_data_param_type;

/*---------------------------------------------------------------------------
  Cipher operation mode parameters type: 
    Electronic Codebook Mode: each block is encoded independently using the
                             same key.
    Cipher Block Chaining Mode: the input depends on the output of the 
                                proceeding block, or the initial vector.
---------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_ENC_OP_MODE_CBC         = 0,
  SECAPI_ENC_OP_MODE_ECB         = 0x1,
  SECAPI_ENC_OP_MODE_CTR         = 0x2
} secapi_enc_op_mode_enum_type;

typedef struct
{
  uint8*  enc_ptr;
  uint8*  dec_ptr;
} secapi_enc_cbc_state_param_type;

typedef enum
{
  SECAPI_ENC_INPUT_MODE_CONTINUE  = 0,      /* More data are coming */
  SECAPI_ENC_INPUT_MODE_FINAL     = 1       /* Last chunck of data  */
} secapi_enc_input_mode_enum_type;

typedef enum
{
  SECAPI_ENC_NO_PADDING = 0,
  SECAPI_ENC_ZERO_PADDING,
  SECAPI_ENC_SEQUENTIAL_PADDING,
  SECAPI_ENC_RANDOM_PADDING,
  SECAPI_ENC_STATIC_PADDING,
  SECAPI_ENC_LENGTH_VALUE_PADDING,
  SECAPI_ENC_RFC2630_PADDING,
  SECAPI_ENC_MAX_PADDING_TYPE
} secapi_enc_padding_enum_type;

typedef struct
{
  secapi_enc_padding_enum_type  type;
  uint8                         value;
} secapi_enc_padding_param_type;

typedef union
{
  secapi_param_type                common;
  secapi_enc_init_data_param_type  init_data;
  secapi_enc_op_mode_enum_type     op_mode;
  secapi_enc_cbc_state_param_type  cbc_state;
  secapi_enc_input_mode_enum_type  input_mode;
  secapi_enc_padding_param_type    padding;
} secapi_enc_param_data_type;
/*~ PARTIAL secapi_enc_param_data_type */
/*~ IF ( _DISC_ < SECAPI_MAX_COMMON_PARAM ) secapi_enc_param_data_type.common */
/*~ CASE SECAPI_ENC_OP_MODE secapi_enc_param_data_type.op_mode */
/*~ CASE SECAPI_ENC_PADDING_TYPE secapi_enc_param_data_type.padding */
/* See secapi_pkx_param_data_type for the usage of obj_disc as a discriminator */
/*~ FIELD secapi_enc_param_data_type.common DISC obj_disc */


/*===========================================================================
  HASH interface types and definitions
===========================================================================*/

/* MD5 digest size and block length */
#define SECAPI_HSH_MD5_DIGEST_SIZE         16
#define SECAPI_HSH_MD5_BLOCK_BYTE_LEN      64

/* SHA-1 digest size and block length */
#define SECAPI_HSH_SHA_DIGEST_SIZE         20
#define SECAPI_HSH_SHA_BLOCK_BYTE_LEN      64

/* SHA-256 digest size and block length */
#define SECAPI_HSH_SHA256_DIGEST_SIZE         32
#define SECAPI_HSH_SHA256_BLOCK_BYTE_LEN      64


#define SECAPI_HSH_LARGEST_DIGEST_SIZE     SECAPI_HSH_SHA256_DIGEST_SIZE
#define SECAPI_HSH_LARGEST_BLOCK_BYTE_LEN  SECAPI_HSH_SHA256_BLOCK_BYTE_LEN
  

typedef secapi_param_enum_type secapi_hsh_param_enum_type;

typedef enum
{
  SECAPI_HSH_INPUT_MODE_ATOMIC    = 0,
  SECAPI_HSH_INPUT_MODE_CONTINUE  = 1,
  SECAPI_HSH_INPUT_MODE_FINAL     = 2
} secapi_hsh_mode_enum_type;

typedef union
{
  secapi_param_type          common;
  secapi_hsh_mode_enum_type  mode;
} secapi_hsh_param_data_type;
/*~ IF ( _DISC_ < SECAPI_MAX_COMMON_PARAM ) secapi_hsh_param_data_type.common */
/*~ CASE SECAPI_HSH_INPUT_MODE secapi_hsh_param_data_type.mode */
/* See secapi_pkx_param_data_type for the usage of obj_disc as a discriminator */
/*~ FIELD secapi_hsh_param_data_type.common DISC obj_disc */


/*===========================================================================
  Authentication interface types and defintions
===========================================================================*/

#define SECAPI_ASN1_MAX_HASH_OBJ_LEN       19
#define SECAPI_MAX_VRFY_SIG_LEN      \
        ( SECAPI_ASN1_MAX_HASH_OBJ_LEN + SECAPI_HSH_LARGEST_DIGEST_SIZE )
        
/*---------------------------------------------------------------------------
  Specifier for the entropy of the random data generator
---------------------------------------------------------------------------*/
typedef enum
{
  SECAPI_PSEUDO_RANDOM  = 0,   /* ARM-generated pseudo random data        */
  SECAPI_SECURE_RANDOM  = 1,   /* Random data with maximized entropy      */
  SECAPI_PKCS_RANDOM    = 2    /* As above, with no zero bytes in stream  */
} secapi_rnd_enum_type;

/*--------------------------------------------------------------------------
  Callback function pointer for SECAPI_GET_RANDOM
---------------------------------------------------------------------------*/
/* Callback fptr type */
typedef void (*secapi_get_random_cb_fptr_type)( void* );

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION SECAPI_NEW()

DESCRIPTION
  This function will create a new Crypto instance and return the instance
  handle back to the calling task.

  There is no callback function invocation upon the completion this
  operation
 
DEPENDENCIES
  Any application using Crypto handles must first use this function to get
  a handle.

PARAMETERS
  handle_ptr  - Pointer to handle, being set by function
  algorithm   - Algorith of handle

RETURN VALUE
  Status of request:
    E_SUCCESS if successful
    E_NOT_ALLOWED if running out of the free space for allocating the 
                  handle.
    E_INVALID_ARG if algorithm is not supported.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_new
(
  secapi_handle_type     *handle_ptr,
  secapi_algo_enum_type  algorithm
);
/*~ FUNCTION secapi_new */
/*~ PARAM OUT handle_ptr POINTER*/

/*===========================================================================

FUNCTION SECAPI_DELETE()

DESCRIPTION
  This function will delete the Crypto instance specified by the passed 
  handle pointer and stop any DSP operation associated with the passed
  handle.

  There is no callback function invocation upon the completion this
  operation
 
DEPENDENCIES
  None.

PARAMETERS
  handle  - Handle for instance to be freed

RETURN VALUE
  E_SUCCESS
  
SIDE EFFECTS
  If the QDSP Crypto or Modular Math applet is performing an operation for
  the passed Handle instance the QDSP operation will be aborted. 
===========================================================================*/
extern secerrno_enum_type secapi_delete
(
  secapi_handle_type*  handle
);  
/*~ FUNCTION secapi_delete */
/*~ PARAM INOUT handle POINTER*/

/*===========================================================================

FUNCTION SECAPI_REGISTER_WAIT_FUNC

DESCRIPTION
  This function allows an application to register wait functions
  for all TCB's it might be calling the crypto API from.
  A registration of a NULL wait function pointer will clear the
  entry for this task, causing rex_wait() to be used subsequenlty.

DEPENDENCIES
  Only one wait function can be registered per TCB.

RETURN VALUE
  Status of registration:
    E_SUCCESS       if successful
    E_ALREADY_DONE  if a wait function is already registered for 
                    this TCB
    E_INVALID_ARG   if called w/ a NULL fptr and this TCB is not registered
    E_NOT_ALLOWED   if the registry is full

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type secapi_register_wait_func
(
  rex_tcb_type*  tcb_ptr,
  rex_sigs_type  (*wait_fptr)( rex_sigs_type )
);

/*===========================================================================

FUNCTION SECAPI_PKX_DH_SET_PARAM()

DESCRIPTION
  This function will set specific Diffie-Helman parameters for the DH 
  instance specified by the passed instance handle.

DEPENDENCIES
  TBD - get Matt to make a list for these

PARAMETERS
  handle_ptr      - Handle to CAPI instance
  param           - Parameter to be set
  param_data_ptr  - Pointer to parameter specific data

RETURN VALUE
  E_SUCCESS
  
SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_pkx_set_param
(
  secapi_handle_type             handle,
  secapi_algo_enum_type          algo,
  secapi_pkx_param_enum_type     param,
  secapi_pkx_param_data_type*    param_data_ptr
);
/*~ FUNCTION secapi_pkx_set_param */
/*~ PARAM IN param_data_ptr POINTER DISC param */

/*===========================================================================

FUNCTION SECAPI_PKX_CREATE_KEY()

DESCRIPTION
  This function will create a a key using the Diffie-Helman algorithm. The
  key created is dependent on the passed key type: Private, Public, or 
  Secret. The Key will be written to the passed address.

  This function will use previously stored values for the base, modulus,
  and key information needed by the Diffie-Helman algorithm. Note if none
  of these values have been set (via "secapi_pkx_dh_set_param") then default
  values will be used.

DEPENDENCIES
  The passed Crypto-instance must be initialized to perform the Diffie-Helman
  algorithm
  The Maximum calculation time stored for Diffie-Helman must have a non-zero
  value.

PARAMETERS
  handle        - Handle to CAPI instance
  key           - Type of key to create
  key_data_ptr  - Pointer to memory for key generation result
        
RETURN VALUE
  Status of request.

SIDE EFFECTS
  If a Private key is requested the generated key will be stored as the
  Diffie-Helman Key for the passed instance of Diffie-Helman
===========================================================================*/
extern secerrno_enum_type secapi_pkx_create_key
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_pkx_key_enum_type     key,
  uint8*                       key_data_ptr
);

/*==========================================================================

FUNCTION SECAPI_PKX_MODEXP()

DESCRIPTION
  This function uses previously stored values for the base, exponent,
  and modulus to compute a basic modular math exponentiation,
       (base ^ exponent) % modulus.
  The result is the size of the modulus and is stored at the result_ptr
  parameter.

DEPENDENCIES
  Crypto instance must be properly initialized by calling secapi_new() 
  and all the parameters must be set using secapi_pkx_set_param().

PARAMETERS
  handle          - Handle to CAPI instance
  result_ptr      - Pointer to memory for result

RETURN VALUE
  E_SUCCESS,       if successful
  E_NOT_SUPPORTED  Option hasn't been coded yet
  E_FALIURE        if failed to process the request.

SIDE EFFECTS
  None.
==========================================================================*/
extern secerrno_enum_type secapi_pkx_modexp
(
  secapi_handle_type           handle,
  uint8*                       result_ptr
);

/*==========================================================================

FUNCTION SECAPI_PKX_ENCRYPT()

DESCRIPTION
  This function encrypts a message using the configured public RSA key. 

DEPENDENCIES
  None

PARAMETERS
  handle               - Handle to CAPI instance
  in_msg_ptr           - Pointer to ciphertext data
  in_msg_byte_len      - Length of encrypted signature (ciphertext)
  out_msg_ptr          - Pointer to plaintext data (memory provided by caller)
  out_msg_byte_len_ptr - Pointer to plaintext data length

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secapi_pkx_encrypt
(
  secapi_handle_type             handle,
  secapi_algo_enum_type          algo,
  secapi_pkx_padding_enum_type   padding_type,
  uint8*                         plaintext_ptr,
  uint16                         plaintext_len,
  uint8*                         ciphertext_ptr,
  uint16*                        ciphertext_len_ptr
);
/*~ FUNCTION secapi_pkx_encrypt */
/*~ PARAM IN plaintext_ptr VARRAY LENGTH plaintext_len */
/*~ PARAM INOUT ciphertext_len_ptr POINTER */
/*~ PARAM OUT ciphertext_ptr
    VARRAY *ciphertext_len_ptr LENGTH *ciphertext_len_ptr */

/*==========================================================================

FUNCTION SECAPI_PKX_DECRYPT()

DESCRIPTION
  This function decrypts a message using the configured private RSA key. 

DEPENDENCIES
  None

PARAMETERS
  handle               - Handle to CAPI instance
  in_msg_ptr           - Pointer to ciphertext data
  in_msg_byte_len      - Length of encrypted signature (ciphertext)
  out_msg_ptr          - Pointer to plaintext data (memory provided by caller)
  out_msg_byte_len_ptr - Pointer to plaintext data length

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secapi_pkx_decrypt
(
  secapi_handle_type            handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        ciphertext_ptr,
  uint16                        ciphertext_len,
  uint8*                        plaintext_ptr,
  uint16*                       plaintext_len_ptr
);
/*~ FUNCTION secapi_pkx_decrypt */
/*~ PARAM IN ciphertext_ptr VARRAY LENGTH ciphertext_len */
/*~ PARAM INOUT plaintext_len_ptr POINTER */
/*~ PARAM OUT plaintext_ptr
    VARRAY *plaintext_len_ptr LENGTH *plaintext_len_ptr */


/*==========================================================================

FUNCTION SECAPI_PKX_SIGN()

DESCRIPTION
  This function signs a message using the configured private RSA key. 

DEPENDENCIES
  None

PARAMETERS
  handle               - Handle to CAPI instance
  in_msg_ptr           - Pointer to ciphertext data
  in_msg_byte_len      - Length of encrypted signature (ciphertext)
  out_msg_ptr          - Pointer to plaintext data (memory provided by caller)
  out_msg_byte_len_ptr - Pointer to plaintext data length

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secapi_pkx_sign
(
  secapi_handle_type            handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        msg_hash_ptr,
  uint16                        msg_hash_len,
  uint8*                        signature_ptr,
  uint16*                       signature_len_ptr
);
/*~ FUNCTION secapi_pkx_sign */
/*~ PARAM IN msg_hash_ptr VARRAY LENGTH msg_hash_len */
/*~ PARAM INOUT signature_len_ptr POINTER */
/*~ PARAM OUT signature_ptr
    VARRAY *signature_len_ptr LENGTH *signature_len_ptr */

/*==========================================================================

FUNCTION SECAPI_PKX_AUTHENTICATE()

DESCRIPTION
  This function will verify a RSA signature by decrypting and decoding
  the signature. Note the comparison of the hash data is not done here.
  This should be done by the calling function. Also note that the signature
  to be verified should conform to a certain restrictions. Otherwise an 
  E_FAILURE will be returned. 

DEPENDENCIES
  None

PARAMETERS
  handle               - Handle to CAPI instance
  in_msg_ptr           - Pointer to ciphertext data
  in_msg_byte_len      - Length of encrypted signature (ciphertext)
  out_msg_ptr          - Pointer to plaintext data (memory provided by caller)
  out_msg_byte_len_ptr - Pointer to plaintext data length

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
==========================================================================*/
extern secerrno_enum_type secapi_pkx_authenticate
(
  secapi_handle_type            handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        signature_ptr,
  uint16                        signature_len,
  uint8*                        msg_hash_ptr,
  uint16*                       msg_hash_len_ptr
);
/*~ FUNCTION secapi_pkx_authenticate */
/*~ PARAM IN signature_ptr VARRAY LENGTH signature_len */
/*~ PARAM INOUT msg_hash_len_ptr POINTER */
/*~ PARAM INOUT msg_hash_ptr
    VARRAY *msg_hash_len_ptr LENGTH *msg_hash_len_ptr */

/*===========================================================================

FUNCTION SECAPI_HSH_SET_PARAM()

DESCRIPTION
  This function set the hashing parameters for the passed instance handle.

  There is no callback function invocation upon the completion this
  operation

DEPENDENCIES
  None.

PARAMETERS
  handle          - Handle to CAPI instance
  hash            - Hashing algorithm specifier
  param           - Parameter to be set
  param_data_ptr  - Pointer to parameter specific data

RETURN VALUE
  Status of request:
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_hsh_set_param
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_hsh_param_enum_type   param,
  secapi_hsh_param_data_type*  param_data_ptr
);
/*~ FUNCTION secapi_hsh_set_param */
/*~ PARAM IN param_data_ptr POINTER DISC param */

/*===========================================================================

FUNCTION SECAPI_HSH_CREATE_DIGEST()

DESCRIPTION
  This function will create a message digest using the algorithm
  specified.

DEPENDENCIES
  The hash mode should be properly set by calling secapi_hsh_set_param.

PARAMETERS
  handle          - Handle to CAPI instance
  hash            - Hashing algorithm specifier
  msg_ptr         - Pointer to message to be authenticated
  msg_byte_len    - Length of message in bytes
  msg_digest_ptr  - Pointer to message digest (memory provided by caller)

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_IN_PROGRES if QDSP is in use
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_hsh_create_digest
(
  secapi_handle_type      handle_ptr,
  secapi_algo_enum_type   hash,
  uint8*                  msg_data_ptr,
  uint16                  msg_byte_len,
  uint8*                  msg_digest_ptr
);

/*===========================================================================

FUNCTION SECAPI_HSH_CREATE_DIGEST_IOV()

DESCRIPTION
  This function will create a message digest using the algorithm
  specified.

DEPENDENCIES
  The hash mode should be properly set by calling secapi_hsh_set_param.

PARAMETERS
  handle          - Handle to CAPI instance
  algo            - Hashing algorithm specifier
  in_msg_vect     - Pointer to iovec stuct containing message to be authenticated
  msg_digest_ptr  - Pointer to message digest (memory provided by caller)

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_IN_PROGRES if QDSP is in use
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_hsh_create_digest_iov
(
  secapi_handle_type      handle,
  secapi_algo_enum_type   algo,
  IOVEC_T*                in_msg_vect,
  uint8*                  msg_digest_ptr
);

/*===========================================================================

FUNCTION SECAPI_HSH_CREATE_HMAC()

DESCRIPTION
  This function will create a hash MAC per RFC 2104 using the 
  specified hash algorithm.

DEPENDENCIES
  None.

PARAMETERS
  handle          - Handle to CAPI instance
  algo            - Hashing algorithm specifier
  msg_ptr         - Pointer to message to be authenticated
  msg_byte_len    - Length of message in bytes
  key_ptr         - Pointer to input key to HMAC algorithm
  msg_byte_len    - Length of input key in bytes
  msg_digest_ptr  - Pointer to message digest (memory provided by caller)

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_IN_PROGRES if QDSP is in use
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_hsh_create_hmac
(
  secapi_handle_type      handle,
  secapi_algo_enum_type   algo,
  uint8*                  msg_ptr,
  uint16                  msg_len,
  uint8*                  key_ptr,
  uint16                  key_len,
  uint8*                  msg_digest_ptr
);

/*===========================================================================

FUNCTION SECAPI_HSH_CREATE_HMAC_IOV()

DESCRIPTION
  This function will create a hash MAC per RFC 2104 using the 
  specified hash algorithm.

DEPENDENCIES
  None.

PARAMETERS
  handle          - Handle to CAPI instance
  algo            - Hashing algorithm specifier
  in_msg_vect     - Pointer to iovec stuct containing message to be authenticated
  in_key_vect     - Pointer to iovec stuct containing key for HMAC algorithm
  msg_digest_ptr  - Pointer to message digest (memory provided by caller)

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_IN_PROGRES if QDSP is in use
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_hsh_create_hmac_iov
(
  secapi_handle_type      handle,
  secapi_algo_enum_type   algo,
  IOVEC_T*                in_msg_vect,
  IOVEC_T*                in_key_vect,
  uint8*                  msg_digest_ptr
);

/*===========================================================================

FUNCTION SECAPI_ENC_SET_PARAM()

DESCRIPTION
  This function will set the parameters for the encryption/decryption
  algorithm specified

DEPENDENCIES
  None.

PARAMETERS
  handle          - Handle to CAPI instance
  cipher          - Cipher algorithm specifier
  param           - Parameter to be set
  param_data_ptr  - Pointer to parameter specific data

RETURN VALUE
  Status of request.
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_enc_set_param
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_enc_param_enum_type   param,
  secapi_enc_param_data_type*  param_data_ptr
);
/*~ FUNCTION secapi_enc_set_param */
/*~ PARAM IN param_data_ptr POINTER DISC param */

/*===========================================================================

FUNCTION SECAPI_ENC_GET_PARAM()

DESCRIPTION
  This function will set the parameters for the DES encryption/decryption
  algorithm.

  There is no callback function invocation upon the completion this
  operation

DEPENDENCIES
  None.

PARAMETERS
  handle          - Handle to CAPI instance
  cipher          - Cipher algorithm specifier
  param           - Parameter to get
  param_data_ptr  - Pointer to parameter specific data

RETURN VALUE
  Status of request.
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_enc_get_param
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_enc_param_enum_type   param,
  secapi_enc_param_data_type*  param_data_ptr
);
/*~ FUNCTION secapi_enc_get_param */
/*~ PARAM OUT param_data_ptr POINTER DISC param */

/*===========================================================================

FUNCTION SECAPI_ENC_ENCRYPT()

DESCRIPTION
  This function encrypts the passed message using the specified algorithm.

DEPENDENCIES
  The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent and any cipherspecific padding.
  Cipher instance ptr must be properly initialized by secapi_new().
  Cipher algorithm parameters must be properly set by 
  secapi_enc_set_param().

PARAMETERS
  handle               - Handle to CAPI instance
  cipher               - Cipher algorithm specifier
  in_msg_ptr           - Pointer to plaintext data
  in_msg_byte_len      - Length of plaintext in bytes
  out_msg_ptr          - Pointer to ciphertext data (mem. provided by caller)
  out_msg_byte_len_ptr - Length of ciphertext in bytes, set to length of
                         ciphertext by function

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_enc_encrypt
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     cipher,
  uint8*                    in_msg_ptr,
  uint16                    in_msg_byte_len,
  uint8*                    out_msg_ptr,
  uint16*                   out_msg_byte_len_ptr
);
/*~ FUNCTION secapi_enc_encrypt */
/*~ PARAM IN in_msg_ptr VARRAY LENGTH in_msg_byte_len */
/*~ PARAM INOUT out_msg_byte_len_ptr POINTER */
/*~ PARAM OUT out_msg_ptr
    VARRAY *out_msg_byte_len_ptr LENGTH *out_msg_byte_len_ptr */

/*===========================================================================

FUNCTION SECAPI_ENC_ENCRYPT_IOV()

DESCRIPTION
  This function encrypts the passed message using the specified algorithm.

DEPENDENCIES
  The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent and any cipherspecific padding.
  Cipher instance ptr must be properly initialized by secapi_new().
  Cipher algorithm parameters must be properly set by 
  secapi_enc_set_param().

PARAMETERS
  handle               - Handle to CAPI instance
  algo                 - Cipher algorithm specifier
  in_msg_vect          - Pointer to iovec stuct containing plaintext data
  out_msg_vect         - Pointer to iovec stuct containing ciphertext data

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_enc_encrypt_iov
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     algo,
  IOVEC_T*                  in_msg_vect,
  IOVEC_T*                  out_msg_vect
);

/*===========================================================================

FUNCTION SECAPI_ENC_DECRYPT()

DESCRIPTION
  This function decrypts the passed message using the specified algorithm.

DEPENDENCIES
  This function must be called with a complete block ready for
  Decryption if the algorithm is a block cipher. 
  The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent (padding is removed by the cipher).
  Cipher instance ptr must be properly initialized by secapi_new().
  Cipher algorithm parameters must be properly set by 
  secapi_enc_set_param().

PARAMETERS
  handle               - Handle to CAPI instance
  cipher               - Cipher algorithm specifier
  in_msg_ptr           - Pointer to ciphertext data
  in_msg_byte_len      - Length of ciphertext in bytes
  in_byte_order        - Indicates endianess of the ciphertext
  out_msg_ptr          - Pointer to plaintext data (mem. provided by caller)
  out_msg_byte_len_ptr - Length of plaintext in bytes, set to length of
                         plaintext by function
  out_byte_order       - Indicates endianess of the plaintext

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_enc_decrypt
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     cipher,
  uint8*                    in_msg_ptr,
  uint16                    in_msg_byte_len,
  uint8*                    out_msg_ptr,
  uint16*                   out_msg_byte_len_ptr
);
/*~ FUNCTION secapi_enc_decrypt */
/*~ PARAM IN in_msg_ptr VARRAY LENGTH in_msg_byte_len */
/*~ PARAM INOUT out_msg_byte_len_ptr POINTER */
/*~ PARAM OUT out_msg_ptr
    VARRAY *out_msg_byte_len_ptr LENGTH *out_msg_byte_len_ptr */

/*===========================================================================

FUNCTION SECAPI_ENC_DECRYPT_IOV()

DESCRIPTION
  This function decrypts the passed message using the specified algorithm.

DEPENDENCIES
  This function must be called with a complete block ready for
  Decryption if the algorithm is a block cipher. 
  The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent (padding is removed by the cipher).
  Cipher instance ptr must be properly initialized by secapi_new().
  Cipher algorithm parameters must be properly set by 
  secapi_enc_set_param().

PARAMETERS
  handle               - Handle to CAPI instance
  algo                 - Cipher algorithm specifier
  in_msg_vect          - Pointer to iovec stuct containing ciphertext data
  out_msg_vect         - Pointer to iovec stuct containing plaintext data

RETURN VALUE
  Status of request.
    E_SUCCESS        on sucessful completion
    E_BAD_DATA       Data verification failed
    E_NOT_SUPPORTED  Option hasn't been coded yet
    E_FAILURE        Any other failure to process the request

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_enc_decrypt_iov
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     algo,
  IOVEC_T*                  in_msg_vect,
  IOVEC_T*                  out_msg_vect
);

/*===========================================================================

FUNCTION SECAPI_ENC_GET_PADDED_LENGTH()

DESCRIPTION
  This function calculates the maximum data size which needs to be
  allocated by the caller of an operation involving padding or MAC
  calculations.
  Returns the aximum number of bytes to allocate for the result, 
  0 if the handle is not completely configured.
 
DEPENDENCIES
  The specified handle must be fully configured for the intended 
  operation.

PARAMETERS
  handle           - Handle for instance to be used
  algo             - Cipher algorithm specifier
  in_msg_len       - Length of data to be processed
  *padded_len_ptr  - Calculated length for padding and MAC

RETURN VALUE
  Security services error code.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_enc_get_padded_length
(
  secapi_handle_type       handle,
  secapi_algo_enum_type    algo,
  uint16                   in_msg_len,
  uint16                   *padded_len_ptr
);
/*~ FUNCTION secapi_enc_get_padded_length */
/*~ PARAM OUT padded_len_ptr POINTER */

/*===========================================================================

FUNCTION SECAPI_GET_RANDOM()

DESCRIPTION
  This function fetches random data from it's internal random buffer.
  The random buffer is either filled with noise level quanitities
  from searcher measurements or with HMAC'd pseudo random data.
 
  The required randomness is specified as follows (increasing entropy):
    - SECAPI_PSEUDO_RANDOM:  Pseudo random data generated by the ARM
    - SECAPI_SECURE_RANDOM:  Random data enhanced for maximum entropy
                             (even distributions of 0 and 1 bits),
                             suitable for commercial and cryptographic 
                             applications
 
DEPENDENCIES
  Pseudo random data :  None
  Secure random data :  Random data plus access to hash
                        algorithms (runtime on the application DSP).

PARAMETERS
  rnd_data_type - Required randomness of random data
  rnd_data_ptr  - Pointer to array to fill random data into
  rnd_data_len  - Length of data requested (in bytes)

RETURN VALUE
  Status of request:
    E_SUCCESS if successful
    E_AGAIN   if not enough random data has been collected since last call

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_get_random
(
  secapi_rnd_enum_type  rnd_data_type,
  uint8*                rnd_data_ptr,
  uint16                rnd_data_len
);
/*~ FUNCTION secapi_get_random */
/*~ PARAM OUT rnd_data_ptr VARRAY rnd_data_len LENGTH rnd_data_len */

#ifdef FEATURE_SECRND
/*===========================================================================

FUNCTION SECAPI_GET_RANDOM_SET_CB()

DESCRIPTION
  In several different scenarios, a task may require random numbers before 
  SECRND task has been initialized. In such cases the call to 
  secapi_get_random() will return E_AGAIN. It is the calling tasks 
  responsibility to register a callback, that will allow the SECRND task
  to notify the calling task when random data is available.
  Since the callback function will be fired from SECRND task context,
  users should not execute any blocking operations that would cause the
  watchdog to time out.

  The calling task should not call secapi_get_random() in the callback 
  function.
  
DEPENDENCIES

PARAMETERS
  get_random_cb - user defined callback function
  usr_data      - user defined data ( can be NULL )

RETURN VALUE
    E_SUCCESS       if successful
    E_INVALID_ARG   if CB FP is null
    E_NO_MEMORY     reached MAX number of CB FP that can be registered 
    E_FAILURE       if the SECRND task has already  been initialized

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type secapi_get_random_set_cb
(
  secapi_get_random_cb_fptr_type  get_random_cb,
  void*                           usr_data
);
/*~ FUNCTION secapi_get_random_set_cb */

#endif /* FEATURE_SECRND */

#endif /* FEATURE_SEC */

#endif /* SECAPI_H */



