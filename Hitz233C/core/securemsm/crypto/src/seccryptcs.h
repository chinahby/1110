#ifndef SECCRYPTCS_H
#define SECCRYPTCS_H
/*===========================================================================

                    S E C U R I T Y    S E R V I C E S

                 C R Y P T O G R A P H I C   L I B R A R Y

              C S   C R Y P T O  I M P L E M E N  T A T I O N

DESCRIPTION
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
 Copyright (c) 2002-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/seccryptcs.h#3 $ 
  $DateTime: 2007/11/05 20:15:57 $ 
  $Author: hvimadal $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/02    rv     Created file.

===========================================================================*/
#include "customer.h"
#ifdef CS_CRYPTO

#include "AEEIEnv.h"
#include "AEEDispatcher.bid"
#include "AEECritSect.bid"
#include "AEEStdErr.h"

extern IEnv		*StaticEnv_New(void);
extern int      DummyDispatcher_CreateInstance(IEnv *piEnv, AEECLSID cls, void** ppObj);
extern int      DummyCritSect_CreateInstance(IEnv *piEnv, AEECLSID cls, void** ppObj);


#ifndef FEATURE_SEC_IPC
#define SEC_OP_COMPLETE_SIG   0x10000000
#define SECCRYPT_ASYNC_IPC_SIG				(SEC_OP_COMPLETE_SIG)
#endif /* FEATURE_SEC_IPC */
/*=========================================================*/
/* Local Functions                                         */
/*=========================================================*/
void seccryptcs_init(void);

/*=========================================================*/
/* This is the static Global pointer to the CS enviornment */
/*=========================================================*/
extern IEnv		*CSCryptoEnv;

//#include "AEEIBN.h"
#include "AEEIBNModExp.h"
#include "AEEIBNEng.h"
#include "AEEBNModExp.bid"
#include "AEEBNEng.bid"

#if defined(CS_SHA1) 
    #include "AEEIHashCtx.h"
    #include "AEEHMACSHA1Ctx.bid"
    #include "AEESHA1CTX.bid"
#endif /* CS_SHA1 */

#if defined(CS_SHA256)
    #include "AEEIHashCtx.h"
    #include "AEESHA256Ctx.bid"
    #include "AEEHMACSHA256Ctx.bid"
#endif /* CS_SHA256 */

#if defined(CS_PUBKEYRSA)
    #include "AEEIPubKeyExt.h"
    #include "AEEPubKeyExt.bid"
    #include "AEERSAParams.h"
#endif /* CS_PUBKEYRSA */

#ifdef CS_ISIGNAL
#include "AEEISignal.h"
#ifndef SECCRYPT_SVC_IPC_SIG   
#define SECCRYPT_SVC_IPC_SIG   0x10000   /* signal for IPC commands in server */
#endif
#include "ipc.h"
#include "secerrno.h"
#include "seccrypt.h"
#include "secapi.h"


#define seccryptcs_pkx_isignal_msg_id					          (IPCSVC_SEC_BASE_MSG_ID + 0x010f)
#ifdef CS_PUBKEYRSA
/* Messages for PKX CS operations */
#define seccryptcs_pkx_encrypt_req_msg_id               (IPCSVC_SEC_BASE_MSG_ID + 0x0110)
#define seccryptcs_pkx_encrypt_rsp_msg_id               (IPCSVC_SEC_BASE_MSG_ID + 0x0111)
#define seccryptcs_pkx_decrypt_req_msg_id               (IPCSVC_SEC_BASE_MSG_ID + 0x0112)
#define seccryptcs_pkx_decrypt_rsp_msg_id              	(IPCSVC_SEC_BASE_MSG_ID + 0x0113)
#define seccryptcs_pkx_authenticate_req_msg_id          (IPCSVC_SEC_BASE_MSG_ID + 0x0114)
#define seccryptcs_pkx_authenticate_rsp_msg_id          (IPCSVC_SEC_BASE_MSG_ID + 0x0115)
#define seccryptcs_pkx_sign_req_msg_id                  (IPCSVC_SEC_BASE_MSG_ID + 0x0116)
#define seccryptcs_pkx_sign_rsp_msg_id                  (IPCSVC_SEC_BASE_MSG_ID + 0x0117)
#endif /* CS_PUBKEYRSA */


#ifndef FEATURE_SEC_IPC
#include "ixipc.h"

#define SEC_CLNT_SYNC_EXEC_STARTED        (0x01)
#define SEC_CLNT_ASYNC_EXEC_STARTED       (0x02)
#define SEC_CLNT_EXEC_COMPLETE_BIT        (0x04)
#define SEC_CLNT_SYNC_EXEC_COMPLETE       (SEC_CLNT_SYNC_EXEC_STARTED  | SEC_CLNT_EXEC_COMPLETE_BIT)
#define SEC_CLNT_ASYNC_EXEC_COMPLETE      (SEC_CLNT_ASYNC_EXEC_STARTED | SEC_CLNT_EXEC_COMPLETE_BIT)

/* signal for IPC commands in client context */
#define SECCRYPT_CLNT_IPC_SIG                                   (SEC_OP_COMPLETE_SIG)

/*===========================================================================

IPC-MESSAGE SECCRYPT_NEW_MSG

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

typedef struct
{
        ipc_msg_id_type            msg_id;
        ipc_node_id_type           ipc_sender;
        secapi_algo_enum_type      algo;
        seccrypt_handle_type       handle;
        secerrno_enum_type         errno;
        secerrno_enum_type         *errno_ptr;
        uint32                     async_mode;
        uint32                     usr_data[4];
} seccrypt_common_req_msg, seccrypt_common_rsp_msg;

secerrno_enum_type
seccrypt_handle_async_op_complete_sig
(
	ixipc_msg_type*  ipc_rsp_msg
);
#else /* !FEATURE_SEC_IPC */
#include "seccrypt.ipc"
#endif /* !FEATURE_SEC_IPC */


/*==========================================================================

IPC-MESSAGE SECCRYPTCS_PKX_ISIGNAL_MSG

DESCRIPTION
  This message cotains an ISignal. 

DEPENDENCIES
  None

PARAMETERS
  handle               - Handle to CAPI instance

RETURN VALUE

SIDE EFFECTS
  None
==========================================================================*/
typedef struct
{
	ipc_msg_id_type      msg_id;
    ipc_node_id_type     ipc_sender;
	ISignal              *pISignal;
} seccryptcs_pkx_isignal_msg;
   

/*=========================================================================
	
	This strcutre is passed in as the user data when creating an ISignal.
	The elements of the structure should be set before an ISignal is 
	created

*============================================================================*/

#ifdef CS_PUBKEYRSA
/*==========================================================================

IPC-MESSAGE SECCRYPTCS_PKX_ENCRYPT_MSG

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
typedef struct
{
        seccrypt_common_req_msg      common;
        secapi_pkx_padding_enum_type padding_type;
        uint32                       plaintext_len;
        uint32                       ciphertext_len;
        uint16*                      ciphertext_len_ptr;
        uint8*                       ciphertext_ptr;    
        uint8                        plaintext[SECAPI_PKX_MAX_KEY_SIZE];
        uint8                        ciphertext[SECAPI_PKX_MAX_KEY_SIZE];
} seccryptcs_pkx_encrypt_req_msg, seccryptcs_pkx_encrypt_rsp_msg;

/*==========================================================================

IPC-MESSAGE SECCRYPTCS_PKX_AUTHENTICATE_MSG

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
typedef struct
{
        seccrypt_common_req_msg      common;
        secapi_pkx_padding_enum_type padding_type;
        uint32                       signature_len;
        uint32                       msg_len;
        uint16*                      msg_len_ptr;
        uint8                        signature[SECAPI_PKX_MAX_KEY_SIZE];
        uint8                        msg[SECAPI_PKX_MAX_KEY_SIZE];
}seccryptcs_pkx_authenticate_req_msg, seccryptcs_pkx_authenticate_rsp_msg;

/*==========================================================================

IPC-MESSAGE SECCRYPTCS_PKX_SIGN_MSG

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
typedef struct
{
        seccrypt_common_req_msg      common;
        secapi_pkx_padding_enum_type padding_type;
        uint32                       msg_len;
        uint32                       signature_len;
        uint16*                      signature_len_ptr;
        uint8*                       signature_ptr;
        uint8                        msg[SECAPI_PKX_MAX_KEY_SIZE];
        uint8                        signature[SECAPI_PKX_MAX_KEY_SIZE];
} seccryptcs_pkx_sign_req_msg, seccryptcs_pkx_sign_rsp_msg;


/*===========================================================================

FUNCTION SEC_SVC_INIT_AFTER_TASK_START

DESCRIPTION

  Initialize security task objects that needs to be initialized after
  reporting back to MC that CM is ready to run by calling on 
  mc_task_start().

DEPENDENCIES
  This function must be called only once at powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void seccryptcs_svc_init_after_task_start( void );

/*==========================================================================

FUNCTION SECCRYPTCS_SVC_PKX_ENCRYPT()

DESCRIPTION
  This function encrypts a message using the configured public RSA key. 

DEPENDENCIES
  None

PARAMETERS
  handle               - Handle to CAPI instance
  algo                 - Algorithm of handle
  padding_type         - Padding type according to PKCS spec
  plaintext_ptr        - Pointer to plaintext data 
  plaintext_len        - Plaintext data length
  ciphertext_ptr       - Pointer to ciphertext data (memory provided by caller)
  ciphertext_len_ptr   - Pointer to ciphertext data length

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any RSA parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
==========================================================================*/
void seccryptcs_svc_pkx_encrypt(	seccryptcs_pkx_encrypt_req_msg*  msg_ptr);

/*==========================================================================

FUNCTION SECCRYPTCS_SVC_PKX_AUTHENTICATE()

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
  algo                 - Algorithm of handle
  padding_type         - Padding type according to PKCS spec
  signature_ptr        - Pointer to the signature
  signature_len        - Length of signature 
  msg_ptr              - Pointer to plaintext data (memory provided by caller)
  msg_len_ptr          - Pointer to plaintext data length

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
==========================================================================*/
void seccryptcs_svc_pkx_authenticate( seccryptcs_pkx_authenticate_req_msg*  msg_ptr);

/*==========================================================================

FUNCTION SECCRYPTCS_SVC_PKX_SIGN()

DESCRIPTION
  This function signs a message using the configured private RSA key. 

DEPENDENCIES
  None

PARAMETERS
  handle               - Handle to CAPI instance
  algo                 - Algorithm of handle
  padding_type         - Padding type according to PKCS spec
  ciphertext_ptr       - Pointer to ciphertext data
  ciphertext_len_ptr   - Pointer to ciphertext data length
  plaintext_ptr        - Pointer to plaintext data ((memory provided by caller)
  plaintext_len        - Plaintext data length

RETURN VALUE
  E_SUCCESS     if the function executes successfully.
  E_NOT_ALLOWED if any Diffie-Helman parameters have not been set.
  E_IN_PROGRESS if Crypto-DSP interface was already busy
  E_FAILURE     if any other failures.

SIDE EFFECTS
  None
==========================================================================*/
void seccryptcs_svc_pkx_sign( seccryptcs_pkx_sign_req_msg*  msg_ptr );


void seccryptcs_process_pkx_encrypt_rsp(	seccryptcs_pkx_encrypt_rsp_msg* ipc_rsp_ptr);
void seccryptcs_process_pkx_authenticate_rsp( seccryptcs_pkx_authenticate_rsp_msg* ipc_rsp_ptr);
void seccryptcs_process_pkx_sign_rsp( seccryptcs_pkx_sign_rsp_msg* ipc_rsp_ptr);
#endif /* CS_PUBKEYRSA */

#endif /* CS_ISIGNAL */
#endif /* CS_CRYPTO */

#endif /* SECCRYPTCS_H */
