#ifndef SECCRYPT_H
#define SECCRYPT_H

/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
                
                 C R Y P T O G R A P H I C    I N T E R F A C E

                        H E A D E R    F I L E

DESCRIPTION
  This is the header file for the ARM-QDSP2 Cryptographic Interface. It
  defines the external interfaces for key exchange, message digest, and
  encrypt/decrypt for various algorithms. Those applications who want to
  use crypto services should include this header file.

  NOTE: Applications outside of DMSS shall use the API defined
        by secapi.h !


EXTERNALIZED FUNCTIONS

    seccrypt_new()               - create a new crypto handle
    seccrypt_delete()            - delete a crypto handle
    seccrypt_pkx_dh_set_param()  - set the parameters for DH handle
    seccrypt_pkx_dh_create_key() - create the keys using the DH algorithm
    seccrypt_pkx_rsa_set_param() - set the parameters for an RSA handle
    seccrypt_pkx_rsa_public_encrypt()   - encrypt data using RSA
    seccrypt_pkx_rsa_verify_signature() - decrypt data using RSA
    seccrypt_pkx_abort()         - abort the current key generation process
    seccrypt_hsh_set_param()     - set the parameters for a hash handle
    seccrypt_hsh_create_digest() - create a message digest
    seccrypt_hsh_create_hmac()   - create an HMAC digest
    seccrypt_enc_set_param()     - set the parameters for a cipher
    seccrypt_enc_get_param()     - get the parameters for a cipher
    seccrypt_enc_encrypt()       - encrypt using a cipher
    seccrypt_enc_decrypt()       - decrypt using a cipher

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2000-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/seccrypt.h#5 $ 
  $DateTime: 2009/01/07 15:53:15 $ 
  $Author: etarolli $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/09   ejt     added PLATFORM_LTK conditional compile
05/17/07   rv      Added support for IPubKey using CS implementatiion
04/17/07   rv      Added support for SHA256 using CS implementation
08/31/05   df      changed iovec filename to seciovec
                   Changes to support IOVEC
03/21/05   sb      Added support for Key Store API, Change to pkx_authenticate API
02/25/05   sb      Added support for IPC based SEC, Rsa signing, MSM device key based DSP commands
06/08/04   jay     Added RFC2630_PADDING
02/18/04   rwh     Added modmath parameter size definitions.
11/19/03   yhw     Added AES128 definitions
11/26/03   rwh     Added seccrypt_enc_get_padded_length().
10/21/03   jay     Removed private key parameter for DH.
09/04/03   om      Added private key parameter for DH.
06/04/03   om      Added private key length for DH.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_SEC

#include "rex.h"
#include "dsm.h"
#include "seciovec.h"
#include "secerrno.h"
#include "secapi.h"


/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Handle used by DMSS-internal users */
typedef void* seccrypt_handle_type;

/*===========================================================================

                     ALGORITHM PARAMETER DECLARATIONS

===========================================================================*/
/* Define a signal used in clients task context to support ASYNC mode of SECAPI. 
   This can be SEC_OP_COMPLETE_SIG as we are in client context. */
#ifdef PLATFORM_LTK
#error code not present
#endif

/*---------------------------------------------------------------------------
  Common Parameter Definitions (valid for all instances
---------------------------------------------------------------------------*/

/* The most significant byte contains the common parameters */
#define SECCRYPT_EXEC_MODE_PARAM_MASK       0x01000000
#define SECCRYPT_EXEC_TIMEOUT_PARAM_MASK    0x02000000
#define SECCRYPT_EXEC_PLATFORM_PARAM_MASK   0x04000000
#define SECCRYPT_ASYNC_CALLBACK_PARAM_MASK  0x08000000

/* Parameter types for all instances */
typedef enum
{
  /* Common parameter types */
  SECCRYPT_EXEC_MODE = 0,
  SECCRYPT_EXEC_TIMEOUT = 1,
  SECCRYPT_EXEC_PLATFORM = 2,
  SECCRYPT_ASYNC_CALLBACK = 3,
  SECCRYPT_DATA_GET_FUNC = 4,   
  SECCRYPT_MAX_COMMON_PARAM = 5,
  SECCRYPT_PKX_PARAM_START = 0x1000,
  SECCRYPT_ENC_PARAM_START = 0x2000,
  SECCRYPT_HSH_PARAM_START = 0x3000,

  /* PKX-specific parameters */
  SECPKX_MODEXP = SECCRYPT_PKX_PARAM_START,
  SECPKX_DH_GROUP,
  SECPKX_DH_PRIVATE_KEY_LEN,
  SECPKX_DH_PEER_PUBLIC_KEY,
  SECPKX_RSA_MODULUS,
  SECPKX_RSA_PRIVATE_KEY,
  SECPKX_RSA_PUBLIC_KEY,
  SECPKX_RSA_PRIVATE_KEY_IV,
  SECPKX_MODEXP_PRIV_KEY_IV,
  SECPKX_RSA_KS_PARAMS,
  SECPKX_RSA_ISIGNAL,
  SECPKX_MAX_PARAM,

  /* Cipher-specific parameters */
  SECENC_INIT_DATA = SECCRYPT_ENC_PARAM_START,
  SECENC_OP_MODE,
  SECENC_CBC_STATE,
  SECENC_INPUT_MODE,
  SECENC_IV_SYNC,
  SECENC_MAC_INFO,
  SECENC_PADDING_TYPE,
  SECENC_DEVKEY,
  SECENC_MAX_PARAM,

  /* Hash-specific parameters */
  SECHSH_HASH_MODE = SECCRYPT_HSH_PARAM_START,
  SECHSH_DEVKEY,          // implies using hi priority crypto dsp image
  SECHSH_HIPRI,           // use hi priority crypto dsp image      
  SECHSH_MAX_PARAM,
  SECCRYPT_PARAM_ENUM_RESERVED = 0x7FFFFFFF

} seccrypt_param_enum_type;

/* Maximum execution time definition */
typedef uint32 seccrypt_exec_timeout_param_data_type;

/* Definitions for the different execution platforms  */
typedef enum
{
  /* Execution platforms - these are used as indices into the */
  /* state change function pointer tables - do not change.    */
  SECCRYPT_EXEC_PLATFORM_ARM = 0,
  SECCRYPT_EXEC_PLATFORM_DSP = 1,
  SECCRYPT_EXEC_PLATFORM_HW  = 2,
  SECCRYPT_EXEC_PLATFORM_SIM = 3,
  SECCRYPT_EXEC_NUM_PLATFORM = 4,

  /* The default setting for all instances */
  SECCRYPT_EXEC_PLATFORM_ACCELERATOR_PREFERRED = SECCRYPT_EXEC_NUM_PLATFORM,
  SECCRYPT_EXEC_UNKNOWN_PLATFORM,

  SECCRYPT_EXEC_MAX_PLATFORM
} seccrypt_platform_enum_type;

typedef seccrypt_platform_enum_type seccrypt_exec_platform_param_data_type;

/* Asynchronous execution callback registration */
typedef struct
{
  secerrno_cb_fptr_type  cb_fptr;
  void*                  usr_ptr;
} seccrypt_async_exec_cb_param_data_type;

/* Union for all common parameters */
typedef union
{
  secapi_exec_mode_enum_type              exec_mode;
  seccrypt_exec_timeout_param_data_type   exec_timeout;
  seccrypt_exec_platform_param_data_type  exec_platform;
  seccrypt_async_exec_cb_param_data_type  async_cb;
  secapi_data_get_func_param_data_type    data_get;
} seccrypt_param_type;

/*---------------------------------------------------------------------------
  PKX Parameter Definitions
---------------------------------------------------------------------------*/

/* Maximum supported operand byte size for modular math */
#define SECCRYPT_PKX_MAX_MOD_SIZE   SECAPI_PKX_MAX_KEY_SIZE
#define SECCRYPT_PKX_MAX_EXP_SIZE   SECAPI_PKX_MAX_KEY_SIZE
#define SECCRYPT_PKX_MAX_BASE_SIZE  SECCRYPT_PKX_MAX_MOD_SIZE

/* Parameter types for the PKX module (modulo exponentiation) */
typedef seccrypt_param_enum_type secpkx_param_enum_type;

/* Generic modulo exponentiation */
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
} secpkx_modexp_param_type;

/* Diffie-Hellman group information */
typedef struct
{
  uint8*  base_ptr;
  uint16  base_size;
  uint8*  modulus_ptr;
  uint16  modulus_size;
} secpkx_dh_group_param_type;

typedef uint16 secpkx_dh_private_key_len_type;

typedef uint8* secpkx_dh_private_key_type;
typedef uint8* secpkx_dh_peer_public_key_type;

typedef union
{
  secpkx_dh_group_param_type      group;
  secpkx_dh_private_key_len_type  private_key_len;
  secpkx_dh_private_key_type      private_key_ptr;
  secpkx_dh_peer_public_key_type  peer_public_key_ptr;
} secpkx_dh_param_type;


// Key store parameters
typedef struct secpkx_rsa_ks_params_s
{
  void*   handle;          // Key store handle
  uint16  key_size;        // Size of the key in key store
  boolean sign;            // Will perform a sign operation
} secpkx_rsa_ks_params_type;

/* RSA information */
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
} secpkx_rsa_key_param_type;

typedef struct
{
  secerrno_enum_type      *_errno;
  uint8                   *sig_output;
  uint16                  *sig_output_len;
}secpkx_rsa_isignal_usr_ptr;

typedef struct secpkx_rsa_isignal_param_s
{
  void                          *piSignal;
  secpkx_rsa_isignal_usr_ptr    *pusr_ptr;
}secpkx_rsa_isignal_param_type;


typedef union
{
  secpkx_rsa_key_param_type      key;
  secpkx_rsa_ks_params_type      ks_params;
  secpkx_rsa_isignal_param_type  isignal;
} secpkx_rsa_param_type;

/* PKX parameter type */
typedef union
{
  seccrypt_param_type         common;
  secpkx_modexp_param_type    modexp;
  secpkx_dh_param_type        dh;
  secpkx_rsa_param_type       rsa;
} secpkx_param_data_type;


/*---------------------------------------------------------------------------
  Hash Parameter Definitions
---------------------------------------------------------------------------*/
/* Enum of parameters (same as common one) */
typedef seccrypt_param_enum_type sechsh_param_enum_type;

/* Hashing modes, need to be in sync with SECAPI values */
typedef enum
{
  SECHSH_HASH_MODE_ATOMIC = 0,
  SECHSH_HASH_MODE_CONTINUE = 1,
  SECHSH_HASH_MODE_FINAL = 2
} sechsh_hash_mode_param_type;

/* HMAC types, used per invocation */
typedef enum
{
  SECHSH_HMAC_NONE = 0,
  SECHSH_HMAC_SSL_V3_TYPE,
  SECHSH_HMAC_TLS_10_TYPE,
  SECHSH_HMAC_RFC2104_TYPE,
  SECHSH_HMAC_MAX_TYPE,
  SECHSH_HMAC_ENUM_RESERVED = 0x7FFFFFFF
} seccrypt_hsh_hmac_enum_type;

typedef union
{
  seccrypt_param_type          common;
  sechsh_hash_mode_param_type  mode;
} sechsh_param_data_type;


/*---------------------------------------------------------------------------
  Cipher Parameter Definitions
---------------------------------------------------------------------------*/

#define SECCRYPT_ENC_ARC4_KEY_SIZE      16

#define SECCRYPT_ENC_DES_BLOCK_SIZE     8
#define SECCRYPT_ENC_3DES_BLOCK_SIZE    8
#define SECCRYPT_ENC_AES_BLOCK_SIZE     16

#define SECCRYPT_ENC_MAX_BLOCK_SIZE     SECCRYPT_ENC_AES_BLOCK_SIZE

#define SECCRYPT_ENC_DES_KEY_SIZE       8
#define SECCRYPT_ENC_3DES_KEY_SIZE      24
#define SECCRYPT_ENC_AES128_KEY_SIZE    16
#define SECCRYPT_ENC_AES256_KEY_SIZE    32

#define SECCRYPT_ENC_DES_IV_SIZE        SECCRYPT_ENC_DES_BLOCK_SIZE
#define SECCRYPT_ENC_3DES_IV_SIZE       SECCRYPT_ENC_DES_BLOCK_SIZE
#define SECCRYPT_ENC_AES_IV_SIZE        SECCRYPT_ENC_AES_BLOCK_SIZE

typedef seccrypt_param_enum_type secenc_param_enum_type;

/* All ciphers use keys and IVs, unused params are set to NULL */
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

} secenc_init_data_param_type;

/* Block cipher mode (CBC or ECB) */
typedef secapi_enc_op_mode_enum_type  secenc_op_mode_param_type;

/* The CBC state is used to retrieve the current CBC output */
typedef struct
{
  uint8*  enc;
  uint8*  dec;
} secenc_cbc_state_param_type;

typedef secapi_enc_input_mode_enum_type  secenc_input_mode_param_type;

typedef enum
{
  SECENC_IV_SYNC_OFF = 0,
  SECENC_IV_SYNC_ON
} secenc_iv_sync_param_type;

typedef enum
{
  SECENC_NO_PADDING = 0,
  SECENC_ZERO_PADDING,
  SECENC_SEQUENTIAL_PADDING,
  SECENC_RANDOM_PADDING,
  SECENC_STATIC_PADDING,
  SECENC_LENGTH_VALUE_PADDING,
  SECENC_RFC2630_PADDING,
  SECENC_MAX_PADDING_TYPE
} secenc_padding_enum_type;

typedef struct
{
  secenc_padding_enum_type  type;
  uint8                     value;
} secenc_padding_param_type;

typedef struct
{
  /* Mac mode and handle executing the hashing */
  seccrypt_hsh_hmac_enum_type    mode;
  seccrypt_handle_type           mac_handle;

  union
  {
    struct
    {
      struct
      {
        uint8* mac_secret_ptr;
      } enc;

      struct
      {
        uint8* mac_secret_ptr;
      } dec;

    } ssl;

  } config;

} secenc_mac_info_param_type;

typedef union
{
  seccrypt_param_type          common;
  secenc_init_data_param_type  init_data;
  secenc_op_mode_param_type    op_mode;
  secenc_cbc_state_param_type  cbc_state;
  secenc_input_mode_param_type input_mode;
  secenc_iv_sync_param_type    iv_sync;
  secenc_padding_param_type    padding;
  secenc_mac_info_param_type   mac_info;
} secenc_param_data_type;

/* Mac information passed in per packet */
typedef struct
{
  union
  {
    struct
    {
      uint8   record_type;
      uint64  seq_no;
    } ssl;

  } mac_type;

} seccrypt_mac_pkt_info_type;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  General crypto library functions
---------------------------------------------------------------------------*/
secerrno_enum_type seccrypt_new
(
  seccrypt_handle_type*  handle_ptr,
  secapi_algo_enum_type  handle_type
);

secerrno_enum_type seccrypt_delete
(
  seccrypt_handle_type*  handle_ptr
);

secerrno_enum_type seccrypt_abort
(
  seccrypt_handle_type  handle
);


/*---------------------------------------------------------------------------
  PKX library functions
---------------------------------------------------------------------------*/
secerrno_enum_type seccrypt_pkx_set_param
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secpkx_param_enum_type        param,
  secpkx_param_data_type*       param_data_ptr
);

secerrno_enum_type seccrypt_pkx_modexp
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  uint8*                        result_ptr
);

secerrno_enum_type seccrypt_pkx_create_key
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_key_enum_type      key,
  uint8*                        key_ptr
);

secerrno_enum_type seccrypt_pkx_encrypt
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        plaintext_ptr,
  uint16                        plaintext_len,
  uint8*                        ciphertext_ptr,
  uint16*                       ciphertext_len_ptr
);

secerrno_enum_type seccrypt_pkx_decrypt
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        ciphertext_ptr,
  uint16                        ciphertext_len,
  uint8*                        plaintext_ptr,
  uint16*                       plaintext_len_ptr
);

secerrno_enum_type seccrypt_pkx_sign
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        msg_hash_ptr,
  uint16                        msg_hash_len,
  uint8*                        signature_ptr,
  uint16*                       signature_len_ptr
);

secerrno_enum_type seccrypt_pkx_authenticate
(
  seccrypt_handle_type          handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        signature_ptr,
  uint16                        signature_len,
  uint8*                        msg_hash_ptr,
  uint16*                       msg_hash_len_ptr
);

/*---------------------------------------------------------------------------
  Ciphering library functions
---------------------------------------------------------------------------*/
secerrno_enum_type seccrypt_enc_set_param
(
  seccrypt_handle_type     handle,
  secapi_algo_enum_type    algo,
  secenc_param_enum_type   param,
  secenc_param_data_type*  param_data_ptr
);


secerrno_enum_type seccrypt_enc_get_param
(
  seccrypt_handle_type     handle,
  secapi_algo_enum_type    algo,
  secenc_param_enum_type   param,
  secenc_param_data_type*  param_data_ptr
);

secerrno_enum_type seccrypt_enc_get_padded_length
(
  seccrypt_handle_type     handle,
  secapi_algo_enum_type    algo,
  uint16                   in_msg_len,
  uint16                   *padded_len_ptr
);

secerrno_enum_type seccrypt_enc_encrypt
(
  seccrypt_handle_type         handle,
  secapi_algo_enum_type        algo,
  dsm_item_type**              in_msg_ptr,
  dsm_item_type**              out_msg_ptr,
  uint16*                      msg_byte_len,
  boolean                      consume_input,
  seccrypt_mac_pkt_info_type*  mac_pkt_info
);

secerrno_enum_type seccrypt_enc_decrypt
(
  seccrypt_handle_type         handle,
  secapi_algo_enum_type        algo,
  dsm_item_type**              in_msg_ptr,
  dsm_item_type**              out_msg_ptr,
  uint16*                      msg_byte_len,
  boolean                      consume_input,
  seccrypt_mac_pkt_info_type*  mac_pkt_info
);

secerrno_enum_type seccryptv_enc_encrypt
(
  seccrypt_handle_type         handle,
  secapi_algo_enum_type        algo,
  IOVEC_T*                     in_msg_ptr,
  IOVEC_T*                     out_msg_ptr,
  boolean                      consume_input,
  seccrypt_mac_pkt_info_type*  mac_pkt_info
);

secerrno_enum_type seccryptv_enc_decrypt
(
  seccrypt_handle_type         handle,
  secapi_algo_enum_type        algo,
  IOVEC_T*                     in_msg_ptr,
  IOVEC_T*                     out_msg_ptr,
  boolean                      consume_input,
  seccrypt_mac_pkt_info_type*  mac_pkt_info
);


/*---------------------------------------------------------------------------
  Hashing library functions
---------------------------------------------------------------------------*/
secerrno_enum_type seccrypt_hsh_set_param
(
  seccrypt_handle_type     handle,
  secapi_algo_enum_type    algo,
  sechsh_param_enum_type   param,
  sechsh_param_data_type*  param_data_ptr
);

secerrno_enum_type seccrypt_hsh_create_digest
(
  seccrypt_handle_type   handle,
  secapi_algo_enum_type  handle_type,
  dsm_item_type*         msg_ptr,
  uint8*                 data_ptr,
  uint16                 msg_byte_len,
  dsm_item_type**        msg_digest_ptr
);


secerrno_enum_type seccrypt_hsh_create_hmac
(
  seccrypt_handle_type          handle,
  seccrypt_hsh_hmac_enum_type   hmac_type,
  secapi_algo_enum_type         hmac_algo,
  dsm_item_type*                inner_msg_ptr,
  uint8*                        inner_data_ptr,
  uint16                        inner_byte_len,
  dsm_item_type*                outer_msg_ptr,
  uint8*                        outer_data_ptr,
  uint16                        outer_byte_len,
  dsm_item_type**               hmac_ptr
);

secerrno_enum_type seccryptv_hsh_create_digest
(
  seccrypt_handle_type   handle,
  secapi_algo_enum_type  handle_type,
  IOVEC_T*               msg_iovec_table_ptr,
  uint8*                 msg_digest_ptr
);


secerrno_enum_type seccryptv_hsh_create_hmac
(
  seccrypt_handle_type          handle,
  seccrypt_hsh_hmac_enum_type   hmac_type,
  secapi_algo_enum_type         hmac_algo,
  IOVEC_T*                      inner_msg_ptr,
  IOVEC_T*                      outer_msg_ptr,
  uint8*                        hmac_ptr
);

secerrno_enum_type seccrypt_register_wait_func
(
  rex_tcb_type*  tcb_ptr,
  rex_sigs_type  (*wait_fptr)( rex_sigs_type )
);

#include "seccrypt_v.h"

#endif /* FEATURE_SEC */

#endif /* SECCRYPT_H */

