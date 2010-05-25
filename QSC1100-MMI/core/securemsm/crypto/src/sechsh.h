#ifndef SECHSH_H
#define SECHSH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S E C H A S H

GENERAL DESCRIPTION
  This sofware module contains the source code for the Security Services
  Crypto-Interface layer for message digest. It contains a set of functions
  capable of setting up and running numerous Message Digest algorithms.

EXTERNAL FUNCTIONS

  sechsh_set_param()     - Set parameters for hash crypto handle
  sechsh_create_digest() - Creates the message digest using specified algo


Copyright (c) 2001-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/sechsh.h#1 $ 
  $DateTime: 2007/06/26 13:17:46 $ 
  $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/07   rv      Added support for SHA256 using CS implementation
08/31/05   df      Changes to support IOVEC
09/21/04   df      added proto for sechsh_swap_bytes()
03/25/04   rwh     Removed literals and extra #define's
10/30/03   jay     Removed deprecated hash stuff.
10/21/03   jay     Added leftover offset field
03/07/03   rwh     Made a logging function for hash stats.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_SEC

#include "secapi.h"
#include "seccrypt.h"
#include "seccrypti.h"

#include "sechsharm.h"
#include "sechshdsp.h"
#include "sechshhw.h"


/*===========================================================================

                                CONSTANTS

===========================================================================*/

/*---------------------------------------------------------------------------
               GLOBAL CONSTANTS - FEATURES
---------------------------------------------------------------------------*/

/* All parameters ready mask */
#define SECHSH_HASH_MODE_PARAM_MASK    0x01
#define SECHSH_DEVKEY_PARAM_MASK       SECCRYPT_DEVKEY_PARAM_MASK  // 0x08
#define SECHSH_HIPRI_PARAM_MASK        SECCRYPT_HIPRI_PARAM_MASK   // 0x10
#define SECHSH_REQUIRED_PARAMS_MASK    SECHSH_HASH_MODE_PARAM_MASK

#define SECHSH_LARGEST_BLOCK_BYTE_LEN  SECAPI_HSH_LARGEST_BLOCK_BYTE_LEN


/*===========================================================================

                          TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for a Hash instance, included by the generic seccrypt instance
---------------------------------------------------------------------------*/
struct sechsh_instance_s
{
  struct __sechsh_info_s
  {
    /* Keep track whether this instance is currently executing */
    boolean busy;

    /* Keep track of execution time */
    //seccrypt_exec_time_type time;

    /* Execution mode of the hash */
    secapi_hsh_mode_enum_type mode;

    /* The DSP and hardware accelerators expect data im block multi-   */
    /* ples. This array stores leftover bytes from commands executed   */
    /* in continuous operation.                                        */
    uint8   leftover[SECHSH_LARGEST_BLOCK_BYTE_LEN];
    uint16  leftover_size;

    /* To copy the left overs of the current operation */
    uint16  leftover_offset;
    
    /* These size fields are set up at instantiation to avoid looking up */
    /* the algorithm type every time you need the digest and block sizes. */
    uint16  block_size;
    uint16  digest_size;

    struct
    {
      /* Instance type for an HMAC (user-specified or temporary) */
      IOVEC_T hpad;      // points to ipad or opad key
      IOVEC   hpv;
      uint8   hmac_key[SECAPI_HSH_LARGEST_DIGEST_SIZE];
      uint8   hmac_ipad_key[SECHSH_LARGEST_BLOCK_BYTE_LEN];
      uint8   hmac_opad_key[SECHSH_LARGEST_BLOCK_BYTE_LEN];
      boolean hmac_key_flag;
      boolean hash_instance_is_temporary;
    } hmac;

    /* This is the state of a running hash, known by all platforms */
    struct __sechsh_ctx_s
    {
      uint32  counter[2];
      uint32  iv[16];  // is 64 byte for SHA2-512
    } state;

#if defined (CS_SHA1) || defined (CS_SHA256)
    /*  this is just a place holder for the HASH or HMAC context for the
        CS implementation; we dont know the internal structure of the 
        context, but we know that the biggest structure is 224 byte */

    uint8   cs_hsh_ctx[300]; /* comes from AEE*SHA*Ctx.bid files */
#endif /* defined (CS_SHA1) || defined (CS_SHA256) */
  } common;

#if defined (CS_SHA1) || defined (CS_SHA256)
  boolean    cs_ctx_init;
#endif /* defined (CS_SHA1) || defined (CS_SHA256) */
  /* All implementation-specific data is in here */
  union
  {
    sechsharm_instance_type  arm;
    sechshdsp_instance_type  dsp;
    sechshhw_instance_type   hw;
  } platform;

};


/*===========================================================================

                          DATA DECLARATIONS

===========================================================================*/

/* ---------------------------------------------------------------------- */
/* Hashing State Machine transition function tables                       */
/* ---------------------------------------------------------------------- */
extern seccrypts_state_x_funcs_table_type    sechsh_x_func_table;
extern seccrypts_state_x_allowed_table_type  sechsh_x_allowed_table;


/*===========================================================================

FUNCTION SECHSH_INITIALIZE_INSTANCE

DESCRIPTION
  This function sets the default parameters for the passed-in 
  hash instance.

DEPENDENCIES
  None

PARAMETERS
  instance_ptr  - Pointer to hash instance

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
secerrno_enum_type sechsh_initialize_instance
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECHSH_HASH_SET_PARAM

DESCRIPTION
  This function set the hash-related parameters for the passed instance
  handle. If the parameters needed for hashing are ready, it will set
  the state to indicate the parameters ready.

DEPENDENCIES
  The passed in handle must be properly initialized by seccrypt_new().

PARAMETERS

RETURN VALUE
  Status of request:
    E_SUCCESS if successful
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
extern secerrno_enum_type sechsh_set_param
(
  seccrypt_instance_type*   instance_ptr,
  sechsh_param_enum_type    param,
  sechsh_param_data_type*   param_data_ptr
);

/*===========================================================================

FUNCTION SECHSH_SWAP_BYTES

DESCRIPTION
  The state bytes are swapped.  This include the counter variables,
  and the digest.  This is done when the platform is switched between
  ARM and DSP.

DEPENDENCIES

PARAMETERS
  instance_ptr    - Pointer to hash instance

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void sechsh_swap_bytes( seccrypt_instance_type*  instance_ptr );

/*===========================================================================

FUNCTION SECHSH_HASH_CREATE_DIGEST

DESCRIPTION
  This function will create a message digest. Any valid hash algo may be
  used.

DEPENDENCIES
  The passed in handle must be properly initialized by seccrypt_new().

PARAMETERS

RETURN VALUE
  Status of request.
    E_SUCCESS if successful
    E_IN_PROGRES if QDSP is in use
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/

extern secerrno_enum_type sechsh_create_digest
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECHSH_CREATE_HMAC

DESCRIPTION
  This function will create an HMAC using the inner and outer data
  provided. The HMAC algorithm will either use a pre-allocated handle
  or allocate it's own temporary one, inwhich case the HMAC-type
  (MD5 or SHA-based) needs to be specified.
  The platform to execute this request on will be selected by the 
  crypto scheduler.

  NOTE: The data to be hashed can be passed in via a DSM item chain
        or a simple byte array. Hence these two parameters are
        exclusive - one of them has to be NULL.

DEPENDENCIES
  The hash mode should be properly set by calling sechsh_set_param.

PARAMETERS
  instance_ptr    - Pointer to hash instance (can be NULL)
  hmac_type       - Specifier what hash algo HMAC is based on
  inner_msg_ptr   - Pointer to DSM item chain for inner hashing data
  inner_data_ptr  - Pointer to byte array for inner hashing data
  inner_byte_len  - Length of the inner hashing data in bytes
  inner_msg_ptr   - Pointer to DSM item chain for outer hashing data
  inner_data_ptr  - Pointer to byte array for outer hashing data
  inner_byte_len  - Length of the outer hashing in bytes
  hmac_ptr        - Pointer to DSM item conatining HMAC output,
                    can be NULL

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/
secerrno_enum_type sechsh_create_hmac
(
  seccrypt_instance_type*       instance_ptr
);

/*===========================================================================

FUNCTION SECHSH_CREATE_HMAC_INTERNAL_CMD()

DESCRIPTION
  This function processes internal commands for the individual
  sub-states of an external CREATE_HMAC command.

DEPENDENCIES
  The hash mode should be properly set by calling sechsh_set_param.

PARAMETERS
  instance_ptr  - Pointer to hash instance (can be NULL)
  cmd           - Sub-command of HMAC sequence

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void sechsh_process_hmac_internal_cmd
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECHSH_OP_COMPLETE_CB()

DESCRIPTION
  Common cleanup work for hashing operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sechsh_op_complete_cb
(
  seccrypt_instance_type* instance_ptr
);

/*===========================================================================

FUNCTION SECHSH_FINISH_OP()

DESCRIPTION
  Common cleanup work for hashing operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sechsh_finish_op
(
  seccrypt_instance_type* instance_ptr
);

#ifdef FEATURE_SEC_LOGGING
/*===========================================================================

FUNCTION SECHSH_LOG_HASH_STATS

DESCRIPTION
  This function logs the hash stats.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Writes data to diag log.
===========================================================================*/

void sechsh_log_hash_stats
(
  seccrypt_instance_type     *handle_ptr
);

#endif /* FEATURE_SEC_LOGGING */

#endif /* FEATURE_SEC */

#endif /* SECHSH_H */
